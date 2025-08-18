#include "framework.h"
#include "acme/filesystem/filesystem/file_context.h"
#include "SceneFoundry/sandbox_game/include/scene/scene.h"
#include "SceneFoundry/sandbox_game/include/entities/player.h"
#include "SceneFoundry/sandbox_game/include/entities/game_object.h"

//#include <json.hpp>


#include <glm/gtc/constants.hpp>
//#include <spdlog/spdlog.h>

namespace sandbox_game
{


   //using json = nlohmann::json;

   sandbox_scene::sandbox_scene(::pointer<IWindowInput> input, AssetManager& assetManager)
      : m_pInput(std::move(input)), m_assetManager(assetManager)
   {
   }

   void sandbox_scene::init() {
      auto player = øcreate_pointer<sandbox_player>(m_pInput);

      player->getTransform().translation = m_initialCameraPosition;
      player->getTransform().rotation = m_initialCameraRotation;
      player->onInit();

      m_players.add(player);

   }

   void sandbox_scene::update(float dt) {
      for (auto& player : m_players) {
         player->onUpdate(dt);
      }

      for (auto& [id, obj] : m_gameObjects) {
         obj->onUpdate(dt);
      }
   }
   void sandbox_scene::loadSceneFile(const ::scoped_string& fileName) {
      ::file::path path = "matter://scenes/" + fileName + ".json";

      auto sceneJson = file()->as_network_payload(path);
      //if (!inFile.is_open()) {
      //   throw std::runtime_error("Could not open scene file: " + path);
      //}

      //json sceneJson;
      //inFile >> sceneJson;


      // Parse skybox cubemap name (if present)
      if (sceneJson.property_reference().has_property("skybox")) {
         m_skyboxCubemapName = sceneJson["skybox"].as_string();
         information("Scene specifies skybox: '{}'", m_skyboxCubemapName);
      }
      else {
         warning("No skybox specified in scene file '{}', using default '{}'", fileName, m_skyboxCubemapName);
      }

      information("Loading scene file: {} ({})", fileName, path);

      if (sceneJson.has_property("camera")) {
         auto& camJson = sceneJson["camera"].property_set_reference();

         auto pos = camJson.payload("position", ::array_base<float>{0.f, 0.f, 0.f});
         auto rot = camJson.payload("rotation", ::array_base<float>{0.f, 0.f, 0.f});

         m_initialCameraPosition = { pos[0], pos[1], pos[2] };
         m_initialCameraRotation = {
             glm::radians(rot[0]),
             glm::radians(rot[1]),
             glm::radians(rot[2])
         };

         information("Camera position: ({}, {}, {}), rotation (deg): ({}, {}, {})",
            pos[0], pos[1], pos[2], rot[0], rot[1], rot[2]);
      }

      for (auto& objJson : sceneJson["objects"]) {

         if (objJson.value("special", "") == "lights") {
            int count = objJson.value("count", 1);
            float radius = objJson.value("radius", 4.8f);
            float height = objJson.value("height", -2.5f);
            float intensity = objJson.value("intensity", 15.8f);
            const auto& colorsJson = objJson["colors"];

            for (int i = 0; i < count; ++i) {
               float angle = i * glm::two_pi<float>() / count;
               glm::vec3 pos = {
                   radius * std::cos(angle),
                   height,
                   radius * std::sin(angle)
               };

               auto colorArray = colorsJson[i % colorsJson.size()];
               glm::vec3 color = {
                   colorArray[0],
                   colorArray[1],
                   colorArray[2]
               };

               auto light = sandbox_game_object::makePointLight(intensity, 0.1f, color);
               light->getTransform().translation = pos;

               information("Placed point light at ({}, {}, {})", pos.x, pos.y, pos.z);

               m_gameObjects.emplace(light->getId(), std::move(light));
            }

            continue; // Skip normal parsing for this object
         }


         auto gameObject = sandbox_game_object::createGameObject();

         if (auto it = objJson.find("model"); it != objJson.end()) {
            const ::string modelName = it->get<::string>();

            // try OBJ first
            if (auto objModel = m_assetManager.getOBJModel(modelName)) {
               gameObject->setModel(objModel);
            }
            // then try GLTF
            else if (auto gltfModel = m_assetManager.getGLTFmodel(modelName)) {
               gameObject->setModel(gltfModel);
            }
            else {
               throw std::runtime_error("Model not found in cache: " + modelName);
            }
         }


         auto pos = objJson.value("position", ::array_base<float>{0.f, 0.f, 0.f});
         auto rot = objJson.value("rotation", ::array_base<float>{0.f, 0.f, 0.f});
         auto scl = objJson.value("scale", ::array_base<float>{1.f, 1.f, 1.f});

         gameObject->m_transform.translation = { pos[0], pos[1], pos[2] };
         gameObject->m_transform.rotation = { rot[0], rot[1], rot[2] };
         gameObject->m_transform.scale = { scl[0], scl[1], scl[2] };

         information("Loaded GameObject '{}' - Pos: ({}, {}, {}), Rot: ({}, {}, {}), Scale: ({}, {}, {})",
            objJson.value("name", "unnamed"),
            pos[0], pos[1], pos[2],
            rot[0], rot[1], rot[2],
            scl[0], scl[1], scl[2]);




         bool isSkybox = objJson.value("skybox", false);
         gameObject->m_bIsSkybox = isSkybox;

         if (objJson.contains("cubemap")) {

            gameObject->m_cubemapTextureName = objJson["cubemap"].get<::string>();
         }

         // Store or fallback cubemap texture name on scene-wide variable
         if (isSkybox) {
            if (!gameObject->m_cubemapTextureName.empty()) {
               m_skyboxCubemapName = gameObject->m_cubemapTextureName;
            }
            setSkyboxObject(gameObject);
            information("GameObject '{}' marked as skybox with cubemap '{}'", objJson.value("name", "unnamed"), m_skyboxCubemapName);
         }

         // Store in map
         m_gameObjects.emplace(gameObject->getId(), std::static_pointer_cast<IGameObject>(gameObject));
      }

      information("Scene '{}' loaded. Total objects: {}", fileName, m_gameObjects.size());
   }

   std::optional<std::reference_wrapper<sandbox_game_object>> sandbox_scene::getSkyboxObject() {
      if (!m_skyboxId) return std::nullopt;
      auto it = m_gameObjects.find(*m_skyboxId);
      if (it != m_gameObjects.end()) {
         // cast back from IGameObject→sandbox_game_object
         return std::reference_wrapper(
            static_cast<sandbox_game_object&>(*it->element2()));
      }
      return std::nullopt;
   }

   // Implements the IScene interface:
   std::optional<std::reference_wrapper<IGameObject>>
      sandbox_scene::getSkyboxObject() const {
      if (!m_skyboxId) {
         return std::nullopt;
      }
      auto it = m_gameObjects.find(*m_skyboxId);
      if (it == m_gameObjects.end()) {
         return std::nullopt;
      }
      // we know it really is a sandbox_game_object, but expose it as IGameObject
      return std::make_optional<std::reference_wrapper<IGameObject>>(
         *it->element2()
      );
   }

   sandbox_camera& sandbox_scene::getCamera() {
      if (m_players.empty()) {
         throw std::runtime_error("no players available to get camera from");
      }

      auto* player = dynamic_cast<sandbox_player*>(m_players[0].get());
      if (!player) {
         throw std::runtime_error("first player is not a sandbox_player");
      }

      return player->getCamera();
   }


} // namespace sandbox_game



