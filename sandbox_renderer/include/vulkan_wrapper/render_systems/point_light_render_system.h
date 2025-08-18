#pragma once
#include "SceneFoundry/core_interfaces/include/interfaces/render_system_i.h"
#include "SceneFoundry/sandbox_renderer/include/vulkan_wrapper/vulkan_device.h"
#include "SceneFoundry/sandbox_renderer/include/vulkan_wrapper/vulkan_pipeline.h"
#include <vulkan/vulkan.h>

// STD
//#include <memory>
//#include <vector>

struct PointLightComponent;

class PointLightRS : public IRenderSystem {
public:

	PointLightRS(const PointLightRS&) = delete;
	PointLightRS& operator=(const PointLightRS&) = delete;

	PointLightRS(vulkan::sandbox_device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
	~PointLightRS();

	void init(
		::vulkan::sandbox_device& device,
		VkRenderPass            renderPass,
		VkDescriptorSetLayout   globalSetLayout,
		vulkan::sandbox_descriptor_pool& descriptorPool,
		size_t frameCount)override;

	void update(FrameInfo& frame, GlobalUbo& ubo) override;
	void render(FrameInfo& frame) override;
private:
	void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
	void createPipeline(VkRenderPass renderPass);

	sandbox_device& m_device;

	VkDescriptorSetLayout m_globalSetLayout;

	std::unique_ptr<sandbox_pipeline> m_pipeline;
	VkPipelineLayout m_pipelineLayout;

	float m_rotationSpeed = 0.2f;
};

