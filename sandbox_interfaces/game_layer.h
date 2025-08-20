#pragma once
//#include "SceneFoundry/sandbox_interfaces/renderer_i.h"
//#include "SceneFoundry/sandbox_interfaces/scene_i.h"


namespace sandbox_interfaces
{

	class IGameLayer  :
		virtual public ::particle
	{
	public:


		virtual void onInit() = 0;
		virtual void onUpdate(float deltaTime) = 0;
		virtual void onRender(ISandboxRenderer::FrameContext& frame) {};
		virtual IScene& getSceneInterface() = 0;
		//virtual ~IGameLayer() = default;


	};


} // namespace sandbox_interfaces

