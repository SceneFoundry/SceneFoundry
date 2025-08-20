#pragma once
//#include "renderer_i.h"
//#include <glm/gtc/matrix_transform.hpp>
#include "SceneFoundry/sandbox_interfaces/transform_component.h"
//#include "SceneFoundry/sandbox_interfaces/camera_i.h"
namespace sandbox_interfaces
{

	class IEntity :
		virtual public ::particle
	{
	public:

		virtual void onInit() {};
		virtual void onUpdate(float dt) {};
		virtual void onRender(Frame * pframe) {};
		virtual sandbox_game::TransformComponent& getTransform() = 0;
		//virtual ~IEntity() = default;
	};


} // namespace sandbox_interfaces

