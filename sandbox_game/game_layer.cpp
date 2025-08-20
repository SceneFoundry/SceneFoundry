#include "framework.h"
// game_layer.cpp
#include "SceneFoundry/sandbox_game/game_layer.h"
//#include <spdlog/spdlog.h>

namespace sandbox_game
{


   gameLayer::gameLayer(::pointer<IWindowInput> input, ::sandbox_engine::asset_manager * Passets)
      : m_windowInput(std::move(input))
      , m_assetManager(assets)
   {

      m_interlockedcountGameObject = 1;

   }


   void gameLayer::onInit()
   {
      information("gameLayer::onInit");
      øconstruct(m_scene);
      //m_scene = øcreate_pointer<sandbox_scene>(m_windowInput, m_assetManager);
      m_pscene->initialize_scene(m_windowInput, m_assetManager);
      m_scene->loadSceneFile("default_scene"); // TODO: Eventually specify which scene file to load in a better way than this probably via UI 
      m_scene->init();
   }

   void gameLayer::onUpdate(float dt)
   {
      m_scene->update(dt);
   }


   IScene& gameLayer::getSceneInterface() {
      return *m_scene;
   }


   ::pointer<game_object> gameLayer::createGameObject()
   {

      auto pgameobject = øcreate_new <game_object >();

      auto idGameObject = m_interlockedcountGameObject++;

      pgameobject->initialize_game_object(idGameObject);

      return pgameobject;

   }


   static ::pointer<sandbox_game_object> gameLayer::makePointLight(float intensity = 10.f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.f)) {
      auto gameObj = sandbox_game_object::createGameObject();
      gameObj->m_color = color;
      gameObj->m_transform.scale.x = radius;
      gameObj->m_pointLight = øallocate PointLightComponent();
      gameObj->m_pointLight->lightIntensity = intensity;
      return gameObj;
   }



} // namespace sandbox_game


