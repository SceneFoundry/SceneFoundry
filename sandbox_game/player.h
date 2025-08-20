#pragma once
#include "SceneFoundry/sandbox_interfaces/game_object.h"
#include "SceneFoundry/sandbox_interfaces/window_input.h"
#include "SceneFoundry/sandbox_game/player_input.h"
#include "SceneFoundry/sandbox_interfaces/camera.h"
#include "SceneFoundry/sandbox_interfaces/transform_component.h"
#include "SceneFoundry/sandbox_game/camera.h"
#include "SceneFoundry/sandbox_interfaces/renderer.h"

//#include "SceneFoundry/sandbox_renderer/include/window.h"

#include <glm/glm.hpp>
//#include <memory>
namespace sandbox_game
{


   class sandbox_player :
      virtual public ::sandbox_interfaces::IGameObject
   {
   public:


      sandbox_player(::sandbox_interfaces::IWindowInput * pinput);

      void onInit() override;
      void onUpdate(float deltaTime) override;

      TransformComponent& getTransform() override;

      ::pointer<::sandbox_interfaces::IModel> getModel() const override;
      sandbox_camera& getCamera();



   //private:
      ::pointer<::sandbox_interfaces::IWindowInput>       m_pInput;
      TransformComponent m_transform;
      sandbox_camera m_camera;
      sandbox_mnk_controller m_controller;

      // Configuration
      float m_mouseSensitivity = 0.0025f;
      float m_moveSpeed = 30.0f;

   };



} // namespace sandbox_game


