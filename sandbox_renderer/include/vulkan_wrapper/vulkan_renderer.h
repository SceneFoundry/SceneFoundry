// vulkan_renderer.h
#pragma once
//#include <memory>
#include "SceneFoundry/core_interfaces/include/interfaces/renderer_i.h"
#include "SceneFoundry/core_interfaces/include/interfaces/render_system_i.h"
#include "SceneFoundry/core_interfaces/include/interfaces/asset_provider_i.h"
#include "SceneFoundry/sandbox_renderer/include/vulkan_wrapper/render_systems/obj_render_system.h"
#include "SceneFoundry/sandbox_renderer/include/vulkan_wrapper/render_systems/gltf_render_system.h"
#include "SceneFoundry/sandbox_renderer/include/vulkan_wrapper/render_systems/skybox_ibl_rs.h"
#include "SceneFoundry/sandbox_renderer/include/vulkan_wrapper/render_systems/point_light_rs.h"
#include "SceneFoundry/sandbox_renderer/include/vulkan_wrapper/window.h"
#include "SceneFoundry/core_interfaces/include/vulkan_wrapper/vulkan_device.h"
#include "SceneFoundry/core_interfaces/include/vulkan_wrapper/vulkan_swapchain.h"
#include "SceneFoundry/core_interfaces/include/vulkan_wrapper/vulkan_descriptor.h"
#include "SceneFoundry/core_interfaces/include/vulkan_wrapper/vulkan_buffer.h"
//#include <vector>
//#include <array>

class VkSandboxRenderer : public ISandboxRenderer
{
public:

	static constexpr size_t FrameCount =
		VkSandboxSwapchain::MAX_FRAMES_IN_FLIGHT;


	VkSandboxRenderer(sandbox_device& device, SandboxWindow& window);
	VkSandboxRenderer(const VkSandboxRenderer&) = delete;
	VkSandboxRenderer& operator=(const VkSandboxRenderer&) = delete;
	~VkSandboxRenderer() override;
	


	FrameContext beginFrame() override;
	
	void endFrame(FrameContext& frame) override;
	void beginSwapChainRenderPass(FrameContext& frame)override;
	void endSwapChainRenderPass(FrameContext& frame)override;

	void initializeSystems(IAssetProvider& assets);
	void initSkyboxSystem();
	void renderSystems(FrameInfo& frame)override;

	void waitDeviceIdle() override;

	void updateSystems(FrameInfo& frame, GlobalUbo& ubo, float deltaTime)override;
	
	// Inline helpers
	inline VkRenderPass getSwapChainRenderPass() const { return m_swapchain->getRenderPass(); }
	inline float getAspectRatio() const { return m_swapchain->extentAspectRatio(); }
	inline bool isFrameInProgress() const { return m_bIsFrameStarted; }

	inline VkCommandBuffer getCurrentCommandBuffer() const {
		assert(m_bIsFrameStarted && "Cannot get command buffer when frame not in progress");
		return m_commandBuffers[m_currentFrameIndex];
	}

	inline 	int getFrameIndex() const {
		assert(m_bIsFrameStarted && "Cannot get frame index when frame not in progress");
		return m_currentFrameIndex;
	}

	inline const ::array_base<VkDescriptorSet>& getGlobalDescriptorSet() const {
		return m_globalDescriptorSets;
	}
	inline const ::array_base<std::unique_ptr<VkSandboxBuffer>>& getUboBuffers() const {
		return m_uboBuffers;
	}


	std::unique_ptr<VkSandboxDescriptorPool>                      m_pool;
private:

	::array_base<VkCommandBuffer>					    m_commandBuffers;
	VkCommandPool										   m_commandPool = VK_NULL_HANDLE;
	uint32_t								         m_currentImageIndex = 0;
	int												 m_currentFrameIndex = 0;
	bool										       m_bIsFrameStarted = false;

	std::unique_ptr<sandbox_descriptor_set_layout>		  m_globalLayout;

	sandbox_device&											m_device;
	SandboxWindow&											    m_window;
	::array_base<std::unique_ptr<IRenderSystem>>				   m_systems;

	std::unique_ptr<VkSandboxSwapchain>					     m_swapchain;
	::pointer<VkSandboxSwapchain>					  m_oldSwapchain;
	VkInstance												  m_instance = VK_NULL_HANDLE;
	
	uint32_t								      m_width{ 0 }, m_height{ 0 };
	::array_base<std::unique_ptr<VkSandboxBuffer>>			m_uboBuffers;
	::array_base<VkDescriptorSet>				  m_globalDescriptorSets;
	::array_base<VkFence>							    m_inFlightFences;

	void createGlobalDescriptorObjects();
	void allocateGlobalDescriptors();
	

	void createSwapChain();
	void createCommandBuffers();
	void freeCommandBuffers();
	void recreateSwapchain();


};