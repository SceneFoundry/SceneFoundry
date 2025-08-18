#pragma once
#include "SceneFoundry/core_interfaces/include/interfaces/render_system_i.h"
#include "SceneFoundry/sandbox_renderer/include/vulkan_wrapper/vulkan_device.h"
#include "SceneFoundry/sandbox_renderer/include/vulkan_wrapper/vulkan_pipeline.h"
#include <vulkan/vulkan.h>

// STD
//#include <memory>
//#include <vector>


class ObjRenderSystem : public IRenderSystem {
public:
	ObjRenderSystem(::sandbox_renderer::sandbox_device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
	~ObjRenderSystem();

	ObjRenderSystem(const ObjRenderSystem&) = delete;
	ObjRenderSystem& operator=(const ObjRenderSystem&) = delete;

	void init(
		::sandbox_renderer::sandbox_device& device,
		VkRenderPass            renderPass,
		VkDescriptorSetLayout   globalSetLayout,
		::sandbox_renderer::sandbox_descriptor_pool& descriptorPool,
		size_t frameCount)override;

	void render(FrameInfo& frame) override;
private:
	void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
	void createPipeline(VkRenderPass renderPass);

	::sandbox_renderer::sandbox_device& m_device;

	VkDescriptorSetLayout m_globalSetLayout;

	::pointer<::sandbox_renderer::sandbox_pipeline> m_pipeline;
	VkPipelineLayout m_pipelineLayout;
};

