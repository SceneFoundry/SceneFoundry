#include "framework.h"
#include "SceneFoundry/sandbox_renderer/include/vulkan_wrapper/render_systems/skybox_ibl_render_system.h"
#include "SceneFoundry/sandbox_interfaces/include/interfaces/game_object_i.h"


SkyboxIBLrenderSystem::SkyboxIBLrenderSystem(::sandbox_renderer::sandbox_device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout)
	: m_device{ device }, m_pipelineLayout{ VK_NULL_HANDLE }, m_skyboxDescriptorSet(VK_NULL_HANDLE), m_bHasCubemap(false)
{
}



SkyboxIBLrenderSystem::~SkyboxIBLrenderSystem() {
	// destroy the pipeline layout you created
	vkDestroyPipelineLayout(m_device.device(), m_pipelineLayout, nullptr);
	// (the sandbox_pipeline unique_ptr will destroy the VkPipeline)
}
void SkyboxIBLrenderSystem::init(
	sandbox_renderer::sandbox_device& device,
	VkRenderPass renderPass,
	VkDescriptorSetLayout globalSetLayout,
	sandbox_renderer::sandbox_descriptor_pool& descriptorPool,
	size_t frameCount)
{
	ASSERT(&device == &m_device);
	m_descriptorPool = &descriptorPool;

	createSkyboxDescriptorSetLayout();

	if (m_bHasCubemap) {
		allocateAndWriteSkyboxDescriptorSet();
	}

	createPipelineLayout(globalSetLayout);
	createPipeline(renderPass);
}

void SkyboxIBLrenderSystem::createSkyboxDescriptorSetLayout() {
	m_skyboxSetLayout = sandbox_renderer::sandbox_descriptor_set_layout::Builder(m_device)
		.addBinding(
			0,
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			VK_SHADER_STAGE_FRAGMENT_BIT,
			1,
			0) // no binding flags needed here
		.build();
}


void SkyboxIBLrenderSystem::allocateAndWriteSkyboxDescriptorSet() {
	ASSERT(m_descriptorPool && "Descriptor pool must be set before allocating descriptors");
	ASSERT(m_skyboxSetLayout && "Descriptor set layout must be created before allocating");

	sandbox_renderer::sandbox_descriptor_writer writer(*m_skyboxSetLayout, *m_descriptorPool);
	writer.writeImage(0, &m_skyboxImageInfo);
	bool success = writer.build(m_skyboxDescriptorSet);
	ASSERT(success && "Failed to build skybox descriptor set");
}

void SkyboxIBLrenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {

	VkDescriptorSetLayout skyboxLayoutHandle =
		m_skyboxSetLayout->getDescriptorSetLayout();

	::preallocated_array_base< ::array_base <VkDescriptorSetLayout>, 2 > layouts;
	
	layouts = {
		globalSetLayout,
		skyboxLayoutHandle // from createSkyboxDescriptorSetLayout()
	};

	VkPipelineLayoutCreateInfo layoutInfo{ VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
	layoutInfo.setLayoutCount = (uint32_t)layouts.size();
	layoutInfo.pSetLayouts = layouts.data();
	layoutInfo.pushConstantRangeCount = 0;
	layoutInfo.pPushConstantRanges = nullptr;

	if (vkCreatePipelineLayout(m_device.device(), &layoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create skybox pipeline layout");
	}
}

void SkyboxIBLrenderSystem::render(FrameInfo& frameInfo) {

	if (!m_bHasCubemap) return;

	//if (!m_skyboxModel) return;
	auto skyOpt = frameInfo.scene.getSkyboxObject();
	if (!skyOpt.has_value()) {
		return; // nothing to draw
	}
	IGameObject& skyObj = skyOpt->get();

	ASSERT(m_skyboxDescriptorSet != VK_NULL_HANDLE && "Skybox descriptor set is not allocated!");

	m_pipeline->bind(frameInfo.commandBuffer);
	// Bind two descriptor sets: 0=global UBO, 1=skybox cubemap
	::preallocated_array_base< ::array_base <VkDescriptorSet>, 2 > sets;
		
		sets= {
		frameInfo.globalDescriptorSet,
		m_skyboxDescriptorSet
	};
	vkCmdBindDescriptorSets(
		frameInfo.commandBuffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		m_pipelineLayout,
		0, // firstSet = 0 (binds set 0 and 1 in this call)
		static_cast<uint32_t>(sets.size()),
		sets.data(),
		0,
		nullptr
	);



	auto model = skyObj.getModel();
	if (model) {
		model->bind(frameInfo.commandBuffer);
		model->gltfDraw(frameInfo.commandBuffer);
	}
}


void SkyboxIBLrenderSystem::createPipeline(VkRenderPass renderPass) {
	ASSERT(m_pipelineLayout != VK_NULL_HANDLE && "Pipeline layout must be created before pipeline");

	sandbox_renderer::pipeline_configuration_information config{};
	sandbox_renderer::sandbox_pipeline::defaultPipelineConfigInfo(config);

	::array_base<VkVertexInputBindingDescription>   bindings = {
		vkinit::vertexInputBindingDescription(
			0,
			sizeof(sandbox_renderer::gltf::Vertex),
			VK_VERTEX_INPUT_RATE_VERTEX)
	};
	::array_base<VkVertexInputAttributeDescription> attributes = {
		vkinit::vertexInputAttributeDescription(
			/*binding=*/0,
			/*location=*/0,
			/*format=*/VK_FORMAT_R32G32B32_SFLOAT,
			/*offset=*/offsetof(sandbox_renderer::gltf::Vertex, pos))
	};

	config.bindingDescriptions = bindings;
	config.attributeDescriptions = attributes;
	config.renderPass = renderPass;
	config.pipelineLayout = m_pipelineLayout;
	config.depthStencilInfo.depthTestEnable = VK_TRUE;
	config.depthStencilInfo.depthWriteEnable = VK_FALSE;
	config.depthStencilInfo.depthWriteEnable = VK_COMPARE_OP_LESS_OR_EQUAL;


	::string vertPath = "matter://shaders/spirV/skybox_ibl.vert.spv";
	::string fragPath = "matter://shaders/spirV/skybox_ibl.frag.spv";

	m_pipeline = øcreate_pointer<sandbox_renderer::sandbox_pipeline>(
		m_device,
		vertPath.c_str(),
		fragPath.c_str(),
		config
	);
}
