#pragma once
#include "SceneFoundry/core_interfaces/include/interfaces/game_object_i.h"
#include "SceneFoundry/core_interfaces/include/interfaces/window_input_i.h"
#include "SceneFoundry/sandbox_game/include/input/player_input.h"
#include "SceneFoundry/core_interfaces/include/interfaces/camera_i.h"
#include "SceneFoundry/core_interfaces/include/transform_component.h"
#include "SceneFoundry/sandbox_game/include/camera/camera.h"
#include "SceneFoundry/core_interfaces/include/interfaces/renderer_i.h"

#include "SceneFoundry/sandbox_renderer/include/window.h"

#include <glm/glm.hpp>
//#include <memory>

class SandboxPlayer : public IGameObject
{
public:
    SandboxPlayer(::pointer<IWindowInput> input);

	void onInit() override;
	void onUpdate(float deltaTime) override;

    TransformComponent& getTransform() override;

    ::pointer<IModel> getModel() const override;
    SandboxCamera& getCamera();

 

private:
    ::pointer<IWindowInput>       m_pInput;
    TransformComponent m_transform;
    SandboxCamera m_camera;
    SandboxMNKController m_controller;

    // Configuration
    float m_mouseSensitivity = 0.0025f;
    float m_moveSpeed = 30.0f;

};