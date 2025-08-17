#pragma once
#include "interfaces/render_system_i.h"
#include "vulkan_wrapper/vulkan_device.h"
#include "vulkan_wrapper/vulkan_pipeline.h"
#include <vulkan/vulkan.h>

// STD
//#include <memory>
//#include <vector>


class ObjRenderSystem : public IRenderSystem {
public:
	ObjRenderSystem(sandbox_device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
	~ObjRenderSystem();

	ObjRenderSystem(const ObjRenderSystem&) = delete;
	ObjRenderSystem& operator=(const ObjRenderSystem&) = delete;

	void init(
		sandbox_device& device,
		VkRenderPass            renderPass,
		VkDescriptorSetLayout   globalSetLayout,
		VkSandboxDescriptorPool& descriptorPool,
		size_t frameCount)override;

	void render(FrameInfo& frame) override;
private:
	void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
	void createPipeline(VkRenderPass renderPass);

	sandbox_device& m_device;

	VkDescriptorSetLayout m_globalSetLayout;

	std::unique_ptr<VkSandboxPipeline> m_pipeline;
	VkPipelineLayout m_pipelineLayout;
};

