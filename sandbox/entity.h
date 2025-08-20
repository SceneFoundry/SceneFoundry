#pragma once
//#include "renderer_i.h"
//#include <glm/gtc/matrix_transform.hpp>
//#include "SceneFoundry/sandbox/transform_component.h"
//#include "SceneFoundry/sandbox/camera_i.h"
namespace sandbox
{

	class IEntity :
		virtual public ::particle
	{
	public:

		virtual void onInit() {};
		virtual void onUpdate(float dt) {};
		virtual void onRender(IFrame * pframe) {};
		virtual TransformComponent& getTransform() = 0;
		//virtual ~IEntity() = default;
	};


} // namespace sandbox

