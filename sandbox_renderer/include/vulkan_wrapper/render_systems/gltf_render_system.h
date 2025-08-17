#pragma once
#include "interfaces/render_system_i.h"
#include "interfaces/asset_provider_i.h"
#include "interfaces/game_object_i.h"

#include "vulkan_wrapper/vulkan_device.h"
#include "vulkan_wrapper/vulkan_pipeline.h"
#include "vulkan_wrapper/vulkan_descriptor.h"

#include "vulkan_wrapper/vulkan_gltf.h"
#include "vulkan_wrapper/vulkan_renderer.h"

// STD
//#include <memory>
//#include <vector>

#include <vulkan/vulkan.h>





class GltfRenderSystem : public IRenderSystem {
public:
	GltfRenderSystem(sandbox_device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout, IAssetProvider& assets);
	~GltfRenderSystem();

	GltfRenderSystem(const GltfRenderSystem&) = delete;
	GltfRenderSystem& operator=(const GltfRenderSystem&) = delete;

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
	VkDescriptorSetLayout m_iblSetLayout;
	VkDescriptorSet m_iblDescriptorSet;

	std::unique_ptr<VkSandboxPipeline> m_opaquePipeline;
	std::unique_ptr<VkSandboxPipeline> m_maskPipeline;
	std::unique_ptr<VkSandboxPipeline> m_blendPipeline;
	VkPipelineLayout m_pipelineLayout;

	IAssetProvider& m_assets;

	std::unique_ptr<sandbox_descriptor_set_layout> m_iblLayout;
	::array_base<VkDescriptorSet>				  m_iblDescriptorSets;
};

