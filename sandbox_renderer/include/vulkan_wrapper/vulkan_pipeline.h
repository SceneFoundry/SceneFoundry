#pragma once
//////#include <stdexcept>
//#include <vector>

#include "SceneFoundry/sandbox_renderer/include/vulkan_wrapper/vulkan_device.h"

struct PipelineConfigInfo {
	PipelineConfigInfo() = default;
	PipelineConfigInfo(const PipelineConfigInfo&) = delete;
	PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;


	::array_base<VkVertexInputBindingDescription> bindingDescriptions{};
	::array_base<VkVertexInputAttributeDescription> attributeDescriptions{};
	VkPipelineViewportStateCreateInfo viewportInfo;
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
	VkPipelineRasterizationStateCreateInfo rasterizationInfo;
	VkPipelineMultisampleStateCreateInfo multisampleInfo;
	VkPipelineColorBlendAttachmentState colorBlendAttachment;
	VkPipelineColorBlendStateCreateInfo colorBlendInfo;
	VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
	::array_base<VkDynamicState> dynamicStateEnables;
	VkPipelineDynamicStateCreateInfo dynamicStateInfo;
	VkPipelineLayout pipelineLayout = nullptr;
	VkRenderPass renderPass = nullptr;
	uint32_t subpass = 0;
	VkSpecializationInfo* fragSpecInfo = nullptr;

	// Push constant ranges
	::array_base<VkPushConstantRange> pushConstantRanges;

	// Descriptor set layouts if needed
	::array_base<VkDescriptorSetLayout> descriptorSetLayouts;
};

class VkSandboxPipeline {
public:
	VkSandboxPipeline(
		sandbox_device& device,
		const ::scoped_string & vertFilepath,
		const ::scoped_string & fragFilepath,
		const PipelineConfigInfo& configInfo);
	~VkSandboxPipeline();

	VkSandboxPipeline(const VkSandboxPipeline&) = delete;
	void operator=(const VkSandboxPipeline&) = delete;

	void bind(VkCommandBuffer commandBuffer);

	static void defaultPipelineConfigInfo(PipelineConfigInfo& configInfo);
	static void defaultSkyboxConfigInfo(PipelineConfigInfo& configInfo);

	// Getters
	VkPipeline getPipeline() const { return m_graphicsPipeline; }
	VkPipelineLayout getPipelineLayout() const { return m_pipelineLayout; }
private:
	static ::array_base<char> readFile(const ::scoped_string & filepath);

	void createGraphicsPipeline(
		const ::scoped_string & vertFilepath,
		const ::scoped_string & fragFilepath,
		const PipelineConfigInfo& configInfo);

	void createShaderModule(const ::array_base<char>& code, VkShaderModule* shaderModule);

	sandbox_device& m_device;
	VkPipeline m_graphicsPipeline;
	VkPipelineLayout m_pipelineLayout;
	VkShaderModule m_vertShaderModule;
	VkShaderModule m_fragShaderModule;
};