#pragma once


#include <vulkan/vulkan.h>

//////#include <stdexcept>
//#include <fstream>
//#include <stdlib.h>
//////#include <stdexcept>
//#include <vector>
//#include <array>

#include "SceneFoundry/sandbox_renderer/include/vk_tools/vk_init.h"
#include "SceneFoundry/sandbox_renderer/include/vk_tools/vk_tools.h"

#include <ktx.h>
#include <ktxvulkan.h>

namespace sandbox_renderer
{


	class sandbox_device;

	class sandbox_texture :
		virtual public ::particle
	{
	public:
		sandbox_texture() = default;
		sandbox_texture(sandbox_device* device);
		~sandbox_texture();

		bool STBLoadFromFile(const ::scoped_string& filename);
		bool KTXLoadFromFile(
			const ::scoped_string& filename,
			VkFormat           format,
			sandbox_device* device,
			VkQueue            copyQueue,
			VkImageUsageFlags  imageUsageFlags,
			VkImageLayout      imageLayout,
			bool               forceLinear
		);
		bool LoadCubemap(const ::preallocated_array_base< ::array_base <::string>, 6 >& faceFilePaths);
		void KtxLoadCubemapFromFile(
			const ::scoped_string& name,
			::string filename,
			VkFormat format,
			sandbox_device* device,
			VkQueue copyQueue,
			VkImageUsageFlags imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT,
			VkImageLayout imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		void Destroy();
		void UpdateDescriptor();
		void fromBuffer(
			void* buffer,
			VkDeviceSize       bufferSize,
			VkFormat           format,
			uint32_t           texWidth,
			uint32_t           texHeight,
			sandbox_device* device,
			VkQueue            copyQueue,
			VkFilter           filter = VK_FILTER_LINEAR,
			VkImageUsageFlags  imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT,
			VkImageLayout      imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		VkDescriptorImageInfo GetDescriptor() const { return m_descriptor; }
		VkSampler GetSampler() const { return m_sampler; }
		VkImageView GetImageView() const { return m_view; }
	private:
		bool CreateImage(
			uint32_t width, uint32_t height,
			VkFormat format,
			VkImageTiling tiling,
			VkImageUsageFlags usage,
			VkMemoryPropertyFlags properties,
			uint32_t arrayLayers,
			VkImageCreateFlags flags = 0);
		void CreateImageView(
			VkFormat format,
			VkImageViewType viewType,
			uint32_t layerCount);

		void CreateSampler();
		void TransitionImageLayout(
			VkImageLayout oldLayout,
			VkImageLayout newLayout,
			uint32_t    layerCount);

		void CopyBufferToImage(VkBuffer buffer, uint32_t width, uint32_t height, uint32_t layerCount);
		VkDeviceMemory AllocateMemory(VkMemoryRequirements memRequirements, VkMemoryPropertyFlags properties);



	public:
		sandbox_device* m_pDevice;
		uint32_t              m_width{ 0 }, m_height{ 0 };
		VkImage m_image = VK_NULL_HANDLE;
		VkDeviceMemory m_deviceMemory = VK_NULL_HANDLE;
		VkImageView m_view = VK_NULL_HANDLE;
		VkSampler m_sampler = VK_NULL_HANDLE;

		VkImageLayout m_imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		VkDescriptorImageInfo m_descriptor{};

		VkFormat m_format = VK_FORMAT_UNDEFINED;
		uint32_t m_mipLevels = 1;
		uint32_t m_layerCount = 1;


		void destroy();
		ktxResult loadKTXFile(::string filename, ktxTexture** target);

		bool m_bIsCubemap{ false };
		bool IsCubemap() const { return m_bIsCubemap; }
	};



} // namespace sandbox_renderer


