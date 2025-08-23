#pragma once
//////#include <stdexcept>
//#include <vector>

#include "device.h"

namespace sandbox_renderer
{

	//
	// struct pipeline_configuration_information {
	// 	pipeline_configuration_information() = default;
	// 	pipeline_configuration_information(const pipeline_configuration_information&) = delete;
	// 	pipeline_configuration_information& operator=(const pipeline_configuration_information&) = delete;
	//
	//
	// 	::array_base<VkVertexInputBindingDescription> bindingDescriptions{};
	// 	::array_base<VkVertexInputAttributeDescription> attributeDescriptions{};
	// 	VkPipelineViewportStateCreateInfo viewportInfo;
	// 	VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
	// 	VkPipelineRasterizationStateCreateInfo rasterizationInfo;
	// 	VkPipelineMultisampleStateCreateInfo multisampleInfo;
	// 	VkPipelineColorBlendAttachmentState colorBlendAttachment;
	// 	VkPipelineColorBlendStateCreateInfo colorBlendInfo;
	// 	VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
	// 	::array_base<VkDynamicState> dynamicStateEnables;
	// 	VkPipelineDynamicStateCreateInfo dynamicStateInfo;
	// 	VkPipelineLayout pipelineLayout = nullptr;
	// 	VkRenderPass renderPass = nullptr;
	// 	uint32_t subpass = 0;
	// 	VkSpecializationInfo* fragSpecInfo = nullptr;
	//
	// 	// Push constant ranges
	// 	::array_base<VkPushConstantRange> pushConstantRanges;
	//
	// 	// Descriptor set layouts if needed
	// 	::array_base<VkDescriptorSetLayout> descriptorSetLayouts;
	// };

	class CLASS_DECL_SANDBOX_RENDERER pipeline :
		virtual public ::particle
	{
	public:

		device * m_pgpudevice;
		// VkPipeline m_graphicsPipeline;
		// VkPipelineLayout m_pipelineLayout;
		// VkShaderModule m_vertShaderModule;
		// VkShaderModule m_fragShaderModule;


		::pointer < ::gpu::shader >		m_pshader;


		pipeline(
			sandbox_renderer::device* pdevice,
			const ::scoped_string& vertFilepath,
			const ::scoped_string& fragFilepath);
		~pipeline();

		//virtual void initialize_sandbox_pipeline(
		//	device* pdevice,
		//	const ::scoped_string& vertFilepath,
		//	const ::scoped_string& fragFilepath,
		//	const pipeline_configuration_information& configInfo);
		//virtual void initialize_sandbox_pipeline();

		//pipeline(const pipeline&) = delete;
		//void operator=(const pipeline&) = delete;

		//void bind(::gpu::command_buffer * pcommandbuffer);
		void bind(::gpu::command_buffer *pcommandBuffer);

		//static void default_pipeline_configuration(pipeline_configuration_information& configInfo);
		//static void defaultSkyboxConfigInfo(pipeline_configuration_information& configInfo);
		void set_push_properties(const ::block& block);
		// Getters
		//VkPipeline getPipeline() const { return m_graphicsPipeline; }
		//VkPipelineLayout getPipelineLayout() const { return m_pipelineLayout; }
	//private:
		//static ::array_base<char> readFile(const ::scoped_string& filepath);

		void createGraphicsPipeline(
			const ::scoped_string& vertFilepath,
			const ::scoped_string& fragFilepath);
			//const pipeline_configuration_information& configInfo);

		//void createShaderModule(const ::block & block, VkShaderModule* shaderModule);

	};


} // namespace sandbox_renderer


