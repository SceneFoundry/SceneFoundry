#pragma once

#include "SceneFoundry/core_interfaces/include/interfaces/entity_i.h"
#include "SceneFoundry/core_interfaces/include/interfaces/model_i.h"
#include "SceneFoundry/sandbox_renderer/include/vulkan_wrapper/vulkan_obj.h"
#include <glm/gtc/matrix_transform.hpp>

// std
//#include <memory>
#include <unordered_map>
//////#include <stdexcept>

namespace sandbox_game
{


   class sandbox_game_object : public IGameObject {
   public:

      using id_t = unsigned int;
      using Map = ::map<id_t, sandbox_game_object>;

      static ::pointer<sandbox_game_object> createGameObject() {
         static id_t currentId = 0;
         return __allocate sandbox_game_object(currentId++);
      }

      static ::pointer<sandbox_game_object> makePointLight(float intensity = 10.f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.f)) {
         auto gameObj = sandbox_game_object::createGameObject();
         gameObj->m_color = color;
         gameObj->m_transform.scale.x = radius;
         gameObj->m_pointLight = __allocate PointLightComponent();
         gameObj->m_pointLight->lightIntensity = intensity;
         return gameObj;
      }

      sandbox_game_object(id_t objId);
      sandbox_game_object() = default;
      sandbox_game_object(const sandbox_game_object&) = delete;
      sandbox_game_object& operator=(const sandbox_game_object&) = delete;
      sandbox_game_object(sandbox_game_object&&) = default;
      sandbox_game_object& operator=(sandbox_game_object&&) = default;


      uint32_t getId() const override { return m_id; }

      glm::vec3 getColor() const override { return m_color; }

      PointLightComponent* getPointLight() const override {
         return m_pointLight.get();
      }

      TransformComponent& getTransform() override {
         return m_transform;
      }

      ::pointer<IModel> getModel() const override {
         return m_pModel;
      }

      void setModel(const ::pointer<IModel>& model) {
         m_pModel = model;
      }


      TransformComponent m_transform;
      ::pointer<IModel> m_pModel;
      glm::vec3 m_color{};
      bool m_bIsOBJ{ false };
      ::pointer<PointLightComponent> m_pointLight;

      ::string m_cubemapTextureName;
      id_t m_id;
      bool m_bIsSkybox = false;

   };


} // namespace sandbox_game


