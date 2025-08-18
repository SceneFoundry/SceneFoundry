#pragma once
#include "SceneFoundry/core_interfaces/include/interfaces/render_system_i.h"
#include "SceneFoundry/core_interfaces/include/interfaces/asset_provider_i.h"
#include "SceneFoundry/core_interfaces/include/interfaces/game_object_i.h"

#include "SceneFoundry/sandbox_renderer/include/vulkan_wrapper/vulkan_device.h"
#include "SceneFoundry/sandbox_renderer/include/vulkan_wrapper/vulkan_pipeline.h"
#include "SceneFoundry/sandbox_renderer/include/vulkan_wrapper/vulkan_descriptor.h"

#include "SceneFoundry/sandbox_renderer/include/vulkan_wrapper/vulkan_gltf.h"
#include "SceneFoundry/sandbox_renderer/include/vulkan_wrapper/vulkan_renderer.h"

// STD
//#include <memory>
//#include <vector>

#include <vulkan/vulkan.h>





class GltfRenderSystem : public IRenderSystem {
public:
	GltfRenderSystem(::sandbox_renderer::sandbox_device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout, IAssetProvider& assets);
	~GltfRenderSystem();

	GltfRenderSystem(const GltfRenderSystem&) = delete;
	GltfRenderSystem& operator=(const GltfRenderSystem&) = delete;

	void init(
		sandbox_renderer::sandbox_device& device,
		VkRenderPass            renderPass,
		VkDescriptorSetLayout   globalSetLayout,
		sandbox_renderer::sandbox_descriptor_pool& descriptorPool,
		size_t frameCount)override;

	void render(FrameInfo& frame) override;
private:
	void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
	void createPipeline(VkRenderPass renderPass);

	sandbox_renderer::sandbox_device& m_device;

	VkDescriptorSetLayout m_globalSetLayout;
	VkDescriptorSetLayout m_iblSetLayout;
	VkDescriptorSet m_iblDescriptorSet;

	::pointer<sandbox_renderer::sandbox_pipeline> m_opaquePipeline;
	::pointer<sandbox_renderer::sandbox_pipeline> m_maskPipeline;
	::pointer<sandbox_renderer::sandbox_pipeline> m_blendPipeline;
	VkPipelineLayout m_pipelineLayout;

	IAssetProvider& m_assets;

	::pointer<sandbox_renderer::sandbox_descriptor_set_layout> m_iblLayout;
	::array_base<VkDescriptorSet>				  m_iblDescriptorSets;
};

