#pragma once
#include "renderer_i.h"
#include <glm/gtc/matrix_transform.hpp>
#include "SceneFoundry/sandbox_interfaces/include/transform_component.h"
#include "SceneFoundry/sandbox_interfaces/include/interfaces/camera_i.h"

class IEntity :
	virtual public ::particle
{
public:

	virtual void onInit() {};
	virtual void onUpdate(float dt) {};
	virtual void onRender(ISandboxRenderer::FrameContext& frame) {};
	virtual sandbox_game::TransformComponent& getTransform() = 0;
	//virtual ~IEntity() = default;
};