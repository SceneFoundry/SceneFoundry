#pragma once
#include "SceneFoundry/sandbox_interfaces/include/interfaces/game_layer_i.h"
#include "SceneFoundry/sandbox_interfaces/include/interfaces/window_input_i.h"
#include "SceneFoundry/sandbox_interfaces/include/interfaces/renderer_i.h"
#include "SceneFoundry/sandbox_game/include/entities/player.h"
#include "SceneFoundry/sandbox_game/include/scene/scene.h"
//#include <iostream>
//#include <memory>

namespace sandbox_game
{


   class MyGameLayer : 
      public IGameLayer 
   {
   public:

      MyGameLayer(::pointer<IWindowInput> input, AssetManager& assets);


      void onInit() override;
      void onUpdate(float dt) override;

      IScene& getSceneInterface() override;
   private:
      ::pointer<sandbox_scene> m_scene;
      ::pointer<IWindowInput> m_windowInput;
      AssetManager& m_assetManager;

   };


} // namespace sandbox_game


