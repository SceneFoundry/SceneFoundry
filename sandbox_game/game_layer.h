#pragma once
#include "sandbox_renderer/device.h"
#include "SceneFoundry/sandbox/game_layer.h"
#include "SceneFoundry/sandbox/window_input.h"
#include "SceneFoundry/sandbox/renderer.h"
#include "SceneFoundry/sandbox_game/player.h"
#include "SceneFoundry/sandbox_game/scene.h"
//#include <iostream>
//#include <memory>

namespace sandbox_game
{


   class game_lLayer :
      virtual public ::sandbox_renderer::device
   {
   public:

      ::interlocked_count m_interlockedcountGameObject;
      ::pointer<sandbox_scene> m_pscene;
      ::pointer<::sandbox::IWindowInput> m_pwindowinput;
      ::pointer < sandbox_engine::asset_manager > m_passetmanager;


      game_layer();


      void initialize_game_layer(::sandbox::IWindowInput * pinput, ::sandbox_engine::asset_manager & assets);


      ::pointer<game_object> createGameObject();


       ::pointer<game_object> makePointLight(float intensity = 10.f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.f));


      void onInit() override;
      void onUpdate(float dt) override;

      IScene& getSceneInterface() override;
   ///private:

   };


} // namespace sandbox_game


