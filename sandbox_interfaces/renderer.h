#pragma once
//#include <memory>
//#include <cstdint>
//#include <vulkan/vulkan.h>
//#include <vector>
#include <glm/glm.hpp>
#include "SceneFoundry/sandbox_interfaces/frame_info.h"


namespace sandbox_interfaces
{
	class IRenderer :
		virtual public ::particle
	{
	public:
		// struct FrameContext {
		// 	::array_base<VkCommandBuffer> graphicsCommandBuffers;
		// 	VkCommandBuffer primaryGraphicsCommandBuffer = VK_NULL_HANDLE;
		// 	VkFence frameFence = VK_NULL_HANDLE;
		// 	uint32_t frameIndex = 0;
		//
		// 	bool isValid() const { return primaryGraphicsCommandBuffer != VK_NULL_HANDLE; }
		// };
		//

		// :
		//virtual public ::particlevirtual ~ISandboxRenderer() = default;

		virtual void renderSystems(FrameInfo& frame) = 0;
		virtual void updateSystems(FrameInfo& frame, GlobalUbo& ubo, float deltaTime) {};
		virtual IFrame * beginFrame() = 0;
		virtual void beginSwapChainRenderPass(IFrame * pframe) = 0;
		virtual void endSwapChainRenderPass(IFrame * pframe) = 0;
		virtual void endFrame(IFrame * pframe) = 0;

		virtual void waitDeviceIdle() = 0;

	};
} // namespace sandbox_interfaces


