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


   class MyGameLayer : 
      virtual public ::sandbox_renderer::device
   {
   public:


      ::pointer<sandbox_scene> m_pscene;
      ::pointer<IWindowInput> m_pwindowinput;
      ::pointer < sandbox_engine::asset_manager > m_passetmanager;


      MyGameLayer(::pointer<IWindowInput> input, ::sandbox_engine::asset_manager & assets);


      void onInit() override;
      void onUpdate(float dt) override;

      IScene& getSceneInterface() override;
   ///private:

   };


} // namespace sandbox_game


