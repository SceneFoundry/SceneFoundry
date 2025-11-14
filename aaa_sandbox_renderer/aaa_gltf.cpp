#include "framework.h"
///*
//* Vulkan glTF model and texture loading class based on tinyglTF (https://github.com/syoyo/tinygltf)
//*
//* Copyright (C) 2018-2024 by Sascha Willems - www.saschawillems.de
//*
//* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
//*/
//
///*
// * Note that this isn't a complete glTF loader and not all features of the glTF 2.0 spec are supported
// * For details on how glTF 2.0 works, see the official spec at https://github.com/KhronosGroup/glTF/tree/master/specification/2.0
// *
// * If you are looking for a complete glTF implementation, check out https://github.com/SaschaWillems/Vulkan-glTF-PBR/
// */

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_USE_RAPIDJSON
#include "acme/prototype/string/str.h"
#include "gltf.h"
#include "app-graphics3d/gpu_vulkan/_gpu_vulkan.h"

namespace sandbox_renderer
{

VkDescriptorSetLayout  gltf::descriptorSetLayoutImage = VK_NULL_HANDLE;
VkDescriptorSetLayout  gltf::descriptorSetLayoutIbl = VK_NULL_HANDLE;
VkDescriptorSetLayout  gltf::descriptorSetLayoutUbo = VK_NULL_HANDLE;
VkMemoryPropertyFlags  gltf::memoryPropertyFlags = 0;
uint32_t gltf::descriptorBindingFlags =  gltf::DescriptorBindingFlags::ImageBaseColor |  gltf::DescriptorBindingFlags::ImageNormalMap;

////class device;
//

/*
	We use a custom image loading function with tinyglTF, so we can do custom stuff loading ktx textures
*/
bool loadImageDataFunc(tinygltf::Image* image, const int imageIndex, ::std::string* error, ::std::string* warning, int req_width, int req_height, const unsigned char* bytes, int size, void* userData)
{
	// KTX files will be handled by our own code
	if (!image->uri.find_last_of(".")) {
		if (image->uri.substr(image->uri.find_last_of(".") + 1) == "ktx") {
			return true;
		}
	}

	return tinygltf::LoadImageData(image, imageIndex, error, warning, req_width, req_height, bytes, size, userData);
}

bool loadImageDataFuncEmpty(tinygltf::Image* image, const int imageIndex, ::string* error, ::string* warning, int req_width, int req_height, const unsigned char* bytes, int size, void* userData)
{
	// This function will be used for samples that don't require images to be loaded
	return true;
}


/*
	glTF texture loading class
*/

void gltf::Texture::updateDescriptor()
{
	descriptor.sampler = sampler;
	descriptor.imageView = view;
	descriptor.imageLayout = imageLayout;
}

void gltf::Texture::destroy()
{
	if (pdevice)
	{
		vkDestroyImageView(pdevice->m_logicalDevice, view, nullptr);
		vkDestroyImage(pdevice->m_logicalDevice, image, nullptr);
		vkFreeMemory(pdevice->m_logicalDevice, deviceMemory, nullptr);
		vkDestroySampler(pdevice->m_logicalDevice, sampler, nullptr);
	}
}

void gltf::Texture::fromglTfImage(tinygltf::Image& gltfimage, ::string path, device* pdevice, VkQueue copyQueue, bool isSrgb)
{
	this->pdevice = pdevice;

	bool isKtx = false;
	// Image points to an external ktx file
	if (gltfimage.uri.find_last_of(".") != ::string::npos) {
		if (gltfimage.uri.substr(gltfimage.uri.find_last_of(".") + 1) == "ktx") {
			isKtx = true;
		}
	}

	VkFormat format;

	if (!isKtx) {
		// Texture was loaded using STB_Image

		unsigned char* buffer = nullptr;
		VkDeviceSize bufferSize = 0;
		bool deleteBuffer = false;
		if (gltfimage.component == 3) {
			// Most devices don't support RGB only on Vulkan so convert if necessary
			// TODO: Check actual format support and transform only if required
			bufferSize = gltfimage.width * gltfimage.height * 4;
			buffer = new unsigned char[bufferSize];
			unsigned char* rgba = buffer;
			unsigned char* rgb = &gltfimage.image[0];
			for (size_t i = 0; i < gltfimage.width * gltfimage.height; ++i) {
				for (int32_t j = 0; j < 3; ++j) {
					rgba[j] = rgb[j];
				}
				rgba += 4;
				rgb += 3;
			}
			deleteBuffer = true;
		}
		else {
			buffer = &gltfimage.image[0];
			bufferSize = gltfimage.image.size();
		}

		format = isSrgb
			? VK_FORMAT_R8G8B8A8_SRGB
			: VK_FORMAT_R8G8B8A8_UNORM;

		VkFormatProperties formatProperties;

		width = gltfimage.width;
		height = gltfimage.height;
		mipLevels = static_cast<uint32_t>(floor(log2(std::max(width, height))) + 1.0);

		vkGetPhysicalDeviceFormatProperties(pdevice->m_physicalDevice, format, &formatProperties);
		ASSERT(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT);
		ASSERT(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT);

		VkMemoryAllocateInfo memAllocInfo{};
		memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		VkMemoryRequirements memReqs{};

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingMemory;

		VkBufferCreateInfo bufferCreateInfo{};
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.size = bufferSize;
		bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		VK_CHECK_RESULT(vkCreateBuffer(pdevice->m_logicalDevice, &bufferCreateInfo, nullptr, &stagingBuffer));
		vkGetBufferMemoryRequirements(pdevice->m_logicalDevice, stagingBuffer, &memReqs);
		memAllocInfo.allocationSize = memReqs.size;
		memAllocInfo.memoryTypeIndex = pdevice->getMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		VK_CHECK_RESULT(vkAllocateMemory(pdevice->m_logicalDevice, &memAllocInfo, nullptr, &stagingMemory));
		VK_CHECK_RESULT(vkBindBufferMemory(pdevice->m_logicalDevice, stagingBuffer, stagingMemory, 0));

		uint8_t* data;
		VK_CHECK_RESULT(vkMapMemory(pdevice->m_logicalDevice, stagingMemory, 0, memReqs.size, 0, (void**)&data));
		memcpy(data, buffer, bufferSize);
		vkUnmapMemory(pdevice->m_logicalDevice, stagingMemory);

		VkImageCreateInfo imageCreateInfo{};
		imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		imageCreateInfo.format = format;
		imageCreateInfo.mipLevels = mipLevels;
		imageCreateInfo.arrayLayers = 1;
		imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageCreateInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT;
		imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageCreateInfo.extent = { width, height, 1 };
		imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		VK_CHECK_RESULT(vkCreateImage(pdevice->m_logicalDevice, &imageCreateInfo, nullptr, &image));
		vkGetImageMemoryRequirements(pdevice->m_logicalDevice, image, &memReqs);
		memAllocInfo.allocationSize = memReqs.size;
		memAllocInfo.memoryTypeIndex = pdevice->getMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		VK_CHECK_RESULT(vkAllocateMemory(pdevice->m_logicalDevice, &memAllocInfo, nullptr, &deviceMemory));
		VK_CHECK_RESULT(vkBindImageMemory(pdevice->m_logicalDevice, image, deviceMemory, 0));

		VkCommandBuffer copyCmd = pdevice->createCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);

		VkImageSubresourceRange subresourceRange = {};
		subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subresourceRange.levelCount = 1;
		subresourceRange.layerCount = 1;

		VkImageMemoryBarrier imageMemoryBarrier{};

		imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		imageMemoryBarrier.srcAccessMask = 0;
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		imageMemoryBarrier.image = image;
		imageMemoryBarrier.subresourceRange = subresourceRange;
		vkCmdPipelineBarrier(copyCmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

		VkBufferImageCopy bufferCopyRegion = {};
		bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		bufferCopyRegion.imageSubresource.mipLevel = 0;
		bufferCopyRegion.imageSubresource.baseArrayLayer = 0;
		bufferCopyRegion.imageSubresource.layerCount = 1;
		bufferCopyRegion.imageExtent.width = width;
		bufferCopyRegion.imageExtent.height = height;
		bufferCopyRegion.imageExtent.depth = 1;

		vkCmdCopyBufferToImage(copyCmd, stagingBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bufferCopyRegion);

		imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		imageMemoryBarrier.image = image;
		imageMemoryBarrier.subresourceRange = subresourceRange;
		vkCmdPipelineBarrier(copyCmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

		pdevice->flushCommandBuffer(copyCmd, copyQueue, true);

		vkDestroyBuffer(pdevice->m_logicalDevice, stagingBuffer, nullptr);
		vkFreeMemory(pdevice->m_logicalDevice, stagingMemory, nullptr);

		// Generate the mip chain (glTF uses jpg and png, so we need to create this manually)
		VkCommandBuffer blitCmd = pdevice->createCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);
		for (uint32_t i = 1; i < mipLevels; i++) {
			VkImageBlit imageBlit{};

			imageBlit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageBlit.srcSubresource.layerCount = 1;
			imageBlit.srcSubresource.mipLevel = i - 1;
			imageBlit.srcOffsets[1].x = int32_t(width >> (i - 1));
			imageBlit.srcOffsets[1].y = int32_t(height >> (i - 1));
			imageBlit.srcOffsets[1].z = 1;

			imageBlit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageBlit.dstSubresource.layerCount = 1;
			imageBlit.dstSubresource.mipLevel = i;
			imageBlit.dstOffsets[1].x = int32_t(width >> i);
			imageBlit.dstOffsets[1].y = int32_t(height >> i);
			imageBlit.dstOffsets[1].z = 1;

			VkImageSubresourceRange mipSubRange = {};
			mipSubRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			mipSubRange.baseMipLevel = i;
			mipSubRange.levelCount = 1;
			mipSubRange.layerCount = 1;

			{
				VkImageMemoryBarrier imageMemoryBarrier{};
				imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
				imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				imageMemoryBarrier.srcAccessMask = 0;
				imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				imageMemoryBarrier.image = image;
				imageMemoryBarrier.subresourceRange = mipSubRange;
				vkCmdPipelineBarrier(blitCmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
			}

			vkCmdBlitImage(blitCmd, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageBlit, VK_FILTER_LINEAR);

			{
				VkImageMemoryBarrier imageMemoryBarrier{};
				imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
				imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
				imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
				imageMemoryBarrier.image = image;
				imageMemoryBarrier.subresourceRange = mipSubRange;
				vkCmdPipelineBarrier(blitCmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
			}
		}

		subresourceRange.levelCount = mipLevels;
		imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		imageMemoryBarrier.image = image;
		imageMemoryBarrier.subresourceRange = subresourceRange;
		vkCmdPipelineBarrier(blitCmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

		if (deleteBuffer) {
			delete[] buffer;
		}

		pdevice->flushCommandBuffer(blitCmd, copyQueue, true);
	}
	else {
		// Texture is stored in an external ktx file
		::string filename = path + "/" + gltfimage.uri;

		ktxTexture* ktxTexture;

		ktxResult result = KTX_SUCCESS;
#if defined(__ANDROID__)
		AAsset* asset = AAssetManager_open(androidApp->activity->assetManager, filename.c_str(), AASSET_MODE_STREAMING);
		if (!asset) {
			vks::tools::exitFatal("Could not load texture from " + filename + "\n\nMake sure the assets submodule has been checked out and is up-to-date.", -1);
		}
		size_t size = AAsset_getLength(asset);
		ASSERT(size > 0);
		ktx_uint8_t* textureData = new ktx_uint8_t[size];
		AAsset_read(asset, textureData, size);
		AAsset_close(asset);
		result = ktxTexture_CreateFromMemory(textureData, size, KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &ktxTexture);
		delete[] textureData;
#else
		if (!tools::fileExists(filename)) {
			tools::exitFatal("Could not load texture from " + filename + "\n\nMake sure the assets submodule has been checked out and is up-to-date.", -1);
		}
		result = ktxTexture_CreateFromNamedFile(filename.c_str(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &ktxTexture);
#endif		
		ASSERT(result == KTX_SUCCESS);
		

		this->pdevice = pdevice;
	
		width = ktxTexture->baseWidth;
		height = ktxTexture->baseHeight;
		mipLevels = ktxTexture->numLevels;

		ktx_uint8_t* ktxTextureData = ktxTexture_GetData(ktxTexture);
		ktx_size_t ktxTextureSize = ktxTexture_GetSize(ktxTexture);
		format = ktxTexture_GetVkFormat(ktxTexture);

		// Get pdevice properties for the requested texture format
		VkFormatProperties formatProperties;
		vkGetPhysicalDeviceFormatProperties(pdevice->m_physicalDevice, format, &formatProperties);

		VkCommandBuffer copyCmd = pdevice->createCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingMemory;

		VkBufferCreateInfo bufferCreateInfo = vkinit::bufferCreateInfo();
		bufferCreateInfo.size = ktxTextureSize;
		// This buffer is used as a transfer source for the buffer copy
		bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		VK_CHECK_RESULT(vkCreateBuffer(pdevice->m_logicalDevice, &bufferCreateInfo, nullptr, &stagingBuffer));

		VkMemoryAllocateInfo memAllocInfo = vkinit::memoryAllocateInfo();
		VkMemoryRequirements memReqs;
		vkGetBufferMemoryRequirements(pdevice->m_logicalDevice, stagingBuffer, &memReqs);
		memAllocInfo.allocationSize = memReqs.size;
		memAllocInfo.memoryTypeIndex = pdevice->getMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		VK_CHECK_RESULT(vkAllocateMemory(pdevice->m_logicalDevice, &memAllocInfo, nullptr, &stagingMemory));
		VK_CHECK_RESULT(vkBindBufferMemory(pdevice->m_logicalDevice, stagingBuffer, stagingMemory, 0));

		uint8_t* data;
		VK_CHECK_RESULT(vkMapMemory(pdevice->m_logicalDevice, stagingMemory, 0, memReqs.size, 0, (void**)&data));
		memcpy(data, ktxTextureData, ktxTextureSize);
		vkUnmapMemory(pdevice->m_logicalDevice, stagingMemory);

		::array_base<VkBufferImageCopy> bufferCopyRegions;
		for (uint32_t i = 0; i < mipLevels; i++)
		{
			ktx_size_t offset;
			KTX_error_code result = ktxTexture_GetImageOffset(ktxTexture, i, 0, 0, &offset);
			ASSERT(result == KTX_SUCCESS);
			VkBufferImageCopy bufferCopyRegion = {};
			bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			bufferCopyRegion.imageSubresource.mipLevel = i;
			bufferCopyRegion.imageSubresource.baseArrayLayer = 0;
			bufferCopyRegion.imageSubresource.layerCount = 1;
			bufferCopyRegion.imageExtent.width = std::max(1u, ktxTexture->baseWidth >> i);
			bufferCopyRegion.imageExtent.height = std::max(1u, ktxTexture->baseHeight >> i);
			bufferCopyRegion.imageExtent.depth = 1;
			bufferCopyRegion.bufferOffset = offset;
			bufferCopyRegions.add(bufferCopyRegion);
		}
		
		// Create optimal tiled target image
		VkImageCreateInfo imageCreateInfo = vkinit::imageCreateInfo();
		imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		imageCreateInfo.format = format;
		imageCreateInfo.mipLevels = mipLevels;
		imageCreateInfo.arrayLayers = 1;
		imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageCreateInfo.extent = { width, height, 1 };
		imageCreateInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		VK_CHECK_RESULT(vkCreateImage(pdevice->m_logicalDevice, &imageCreateInfo, nullptr, &image));

		vkGetImageMemoryRequirements(pdevice->m_logicalDevice, image, &memReqs);
		memAllocInfo.allocationSize = memReqs.size;
		memAllocInfo.memoryTypeIndex = pdevice->getMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		VK_CHECK_RESULT(vkAllocateMemory(pdevice->m_logicalDevice, &memAllocInfo, nullptr, &deviceMemory));
		VK_CHECK_RESULT(vkBindImageMemory(pdevice->m_logicalDevice, image, deviceMemory, 0));

		VkImageSubresourceRange subresourceRange = {};
		subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subresourceRange.baseMipLevel = 0;
		subresourceRange.levelCount = mipLevels;
		subresourceRange.layerCount = 1;

		tools::setImageLayout(copyCmd, image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, subresourceRange);
		vkCmdCopyBufferToImage(copyCmd, stagingBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, static_cast<uint32_t>(bufferCopyRegions.size()), bufferCopyRegions.data());
		tools::setImageLayout(copyCmd, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, subresourceRange);
		pdevice->flushCommandBuffer(copyCmd, copyQueue);
		this->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		vkDestroyBuffer(pdevice->m_logicalDevice, stagingBuffer, nullptr);
		vkFreeMemory(pdevice->m_logicalDevice, stagingMemory, nullptr);

		ktxTexture_Destroy(ktxTexture);
	}

	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
	samplerInfo.compareOp = VK_COMPARE_OP_NEVER;
	samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	samplerInfo.maxAnisotropy = 1.0;
	samplerInfo.anisotropyEnable = VK_FALSE;
	samplerInfo.maxLod = (float)mipLevels;
	samplerInfo.maxAnisotropy = 8.0f;
	samplerInfo.anisotropyEnable = VK_TRUE;
	VK_CHECK_RESULT(vkCreateSampler(pdevice->m_logicalDevice, &samplerInfo, nullptr, &sampler));

	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	viewInfo.subresourceRange.layerCount = 1;
	viewInfo.subresourceRange.levelCount = mipLevels;
	VK_CHECK_RESULT(vkCreateImageView(pdevice->m_logicalDevice, &viewInfo, nullptr, &view));

	descriptor.sampler = sampler;
	descriptor.imageView = view;
	descriptor.imageLayout = imageLayout;
}

/*
	glTF material
*/
void  gltf::Material::createDescriptorSet(
	VkDescriptorPool descriptorPool,
	VkDescriptorSetLayout descriptorSetLayout,
	uint32_t descriptorBindingFlags,
	 gltf::Texture* fallbackTexture
) {
	// Allocate descriptor set
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &descriptorSetLayout;
	VK_CHECK_RESULT(vkAllocateDescriptorSets(pdevice->m_logicalDevice, &allocInfo, &descriptorSet));

	::array_base<VkWriteDescriptorSet> writeDescriptorSets;

	// Base Color Texture
	if ((descriptorBindingFlags &  DescriptorBindingFlags::ImageBaseColor) && baseColorTexture) {
		VkDescriptorImageInfo baseColorImageInfo = baseColorTexture->descriptor;
		writeDescriptorSets.add({
			VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,       // sType
			nullptr,                                      // pNext
			descriptorSet,                                // dstSet
			0,                                            // dstBinding (base color = 0)
			0,                                            // dstArrayElement
			1,                                            // descriptorCount
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,    // descriptorType
			&baseColorImageInfo,                          // pImageInfo
			nullptr, nullptr                              // pBufferInfo, pTexelBufferView
			});
	}

	// Normal Map Texture
	if (descriptorBindingFlags &  DescriptorBindingFlags::ImageNormalMap) {
		// Always bind something
		 Texture* tex = (normalTexture != nullptr) ? normalTexture : fallbackTexture;
		VkDescriptorImageInfo normalImageInfo = tex->descriptor;

		writeDescriptorSets.add({
			VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			nullptr,
			descriptorSet,
			1,  // binding = 1 for normal map
			0,
			1,
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			&normalImageInfo,
			nullptr, nullptr
			});
	}

	// Update descriptors
	if (!writeDescriptorSets.empty()) {
		vkUpdateDescriptorSets(
			pdevice->m_logicalDevice,
			static_cast<uint32_t>(writeDescriptorSets.size()),
			writeDescriptorSets.data(),
			0, nullptr
		);
	}
}



/*
	glTF primitive
*/
void  gltf::Primitive::setDimensions(floating_sequence3 min, floating_sequence3 max) {
	dimensions.min = min;
	dimensions.max = max;
	dimensions.size = max - min;
	dimensions.center = (min + max) / 2.0f;
	dimensions.radius = glm::distance(min, max) / 2.0f;
}

/*
	glTF mesh
*/
gltf::Mesh::Mesh(device* pdevice, floating_matrix4 matrix) {
	this->pdevice = pdevice;
	this->uniformBlock.matrix = matrix;
	VK_CHECK_RESULT(pdevice->createBuffer(
		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		sizeof(uniformBlock),
		&uniformBuffer.buffer,
		&uniformBuffer.memory,
		&uniformBlock));
	VK_CHECK_RESULT(vkMapMemory(pdevice->m_logicalDevice, uniformBuffer.memory, 0, sizeof(uniformBlock), 0, &uniformBuffer.mapped));
	uniformBuffer.descriptor = { uniformBuffer.buffer, 0, sizeof(uniformBlock) };
};

gltf::Mesh::~Mesh() {
	vkDestroyBuffer(pdevice->m_logicalDevice, uniformBuffer.buffer, nullptr);
	vkFreeMemory(pdevice->m_logicalDevice, uniformBuffer.memory, nullptr);
	for (auto primitive : primitives)
	{
		delete primitive;
	}
}

/*
	glTF node
*/
floating_matrix4 gltf::Node::localMatrix() {
	return glm::translate(floating_matrix4(1.0f), translation) * floating_matrix4(rotation) * glm::scale(floating_matrix4(1.0f), scale) * matrix;
}

floating_matrix4 gltf::Node::getMatrix() {
	floating_matrix4 m = localMatrix();
	 Node* p = parent;
	while (p) {
		m = p->localMatrix() * m;
		p = p->parent;
	}
	return m;
}

void gltf::Node::update() {
	if (mesh) {
		floating_matrix4 m = getMatrix();
		if (skin) {
			mesh->uniformBlock.matrix = m;
			// Update join matrices
			floating_matrix4 inverseTransform = glm::inverse(m);
			for (size_t i = 0; i < skin->joints.size(); i++) {
				 Node* jointNode = skin->joints[i];
				floating_matrix4 jointMat = jointNode->getMatrix() * skin->inverseBindMatrices[i];
				jointMat = inverseTransform * jointMat;
				mesh->uniformBlock.jointMatrix[i] = jointMat;
			}
			mesh->uniformBlock.jointcount = (float)skin->joints.size();
			memcpy(mesh->uniformBuffer.mapped, &mesh->uniformBlock, sizeof(mesh->uniformBlock));
		}
		else {
			memcpy(mesh->uniformBuffer.mapped, &m, sizeof(floating_matrix4));
		}
	}

	for (auto& child : children) {
		child->update();
	}
}

gltf::Node::~Node() {
	if (mesh) {
		delete mesh;
	}
	for (auto& child : children) {
		delete child;
	}
}

/*
	glTF default vertex layout with easy Vulkan mapping functions
*/

VkVertexInputBindingDescription   gltf::Vertex::vertexInputBindingDescription;
::array_base<VkVertexInputAttributeDescription>   gltf::Vertex::vertexInputAttributeDescriptions;
VkPipelineVertexInputStateCreateInfo   gltf::Vertex::pipelineVertexInputStateCreateInfo;

VkVertexInputBindingDescription   gltf::Vertex::inputBindingDescription(uint32_t binding) {
	return VkVertexInputBindingDescription({ binding, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX });
}

VkVertexInputAttributeDescription   gltf::Vertex::inputAttributeDescription(uint32_t binding, uint32_t location, VertexComponent component) {
	switch (component) {
	case VertexComponent::Position:
		return VkVertexInputAttributeDescription({ location, binding, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos) });
	case VertexComponent::Normal:
		return VkVertexInputAttributeDescription({ location, binding, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal) });
	case VertexComponent::UV:
		return VkVertexInputAttributeDescription({ location, binding, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv) });
	case VertexComponent::Color:
		return VkVertexInputAttributeDescription({ location, binding, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, color) });
	case VertexComponent::Tangent:
		return VkVertexInputAttributeDescription({ location, binding, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, tangent) });
	case VertexComponent::Joint0:
		return VkVertexInputAttributeDescription({ location, binding, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, joint0) });
	case VertexComponent::Weight0:
		return VkVertexInputAttributeDescription({ location, binding, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, weight0) });
	default:
		return VkVertexInputAttributeDescription({});
	}
}

::array_base<VkVertexInputAttributeDescription>   gltf::Vertex::inputAttributeDescriptions(uint32_t binding, const ::array_base<VertexComponent> components) {
	::array_base<VkVertexInputAttributeDescription> result;
	uint32_t location = 0;
	for (VertexComponent component : components) {
		result.add(Vertex::inputAttributeDescription(binding, location, component));
		location++;
	}
	return result;
}

/** @brief Returns the default pipeline vertex input state create info structure for the requested vertex components */
VkPipelineVertexInputStateCreateInfo* gltf::Vertex::getPipelineVertexInputState(const ::array_base<VertexComponent> components) {
	vertexInputBindingDescription = Vertex::inputBindingDescription(0);
	Vertex::vertexInputAttributeDescriptions = Vertex::inputAttributeDescriptions(0, components);
	pipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
	pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = &Vertex::vertexInputBindingDescription;
	pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(Vertex::vertexInputAttributeDescriptions.size());
	pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = Vertex::vertexInputAttributeDescriptions.data();
	return &pipelineVertexInputStateCreateInfo;
}

gltf::Texture* gltf::Model::getTexture(uint32_t index)
{

	if (index < m_textures.size()) {
		return &m_textures[index];
	}
	return nullptr;
}

void gltf::Model::createEmptyTexture(VkQueue transferQueue)
{
	// Define the static member here (outside any class/function)


	emptyTexture.pdevice = m_pDevice;
	emptyTexture.width = 1;
	emptyTexture.height = 1;
	emptyTexture.layerCount = 1;
	emptyTexture.mipLevels = 1;

	size_t bufferSize = emptyTexture.width * emptyTexture.height * 4;
	unsigned char* buffer = new unsigned char[bufferSize];
	memset(buffer, 0, bufferSize);
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingMemory;
	VkBufferCreateInfo bufferCreateInfo = vkinit::bufferCreateInfo();
	bufferCreateInfo.size = bufferSize;
	// This buffer is used as a transfer source for the buffer copy
	bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	VK_CHECK_RESULT(vkCreateBuffer(m_pDevice->m_logicalDevice, &bufferCreateInfo, nullptr, &stagingBuffer));

	VkMemoryAllocateInfo memAllocInfo = vkinit::memoryAllocateInfo();
	VkMemoryRequirements memReqs;
	vkGetBufferMemoryRequirements(m_pDevice->m_logicalDevice, stagingBuffer, &memReqs);
	memAllocInfo.allocationSize = memReqs.size;
	memAllocInfo.memoryTypeIndex = m_pDevice->getMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	VK_CHECK_RESULT(vkAllocateMemory(m_pDevice->m_logicalDevice, &memAllocInfo, nullptr, &stagingMemory));
	VK_CHECK_RESULT(vkBindBufferMemory(m_pDevice->m_logicalDevice, stagingBuffer, stagingMemory, 0));

	// Copy texture data into staging buffer
	uint8_t* data;
	VK_CHECK_RESULT(vkMapMemory(m_pDevice->m_logicalDevice, stagingMemory, 0, memReqs.size, 0, (void**)&data));
	memcpy(data, buffer, bufferSize);
	vkUnmapMemory(m_pDevice->m_logicalDevice, stagingMemory);

	VkBufferImageCopy bufferCopyRegion = {};
	bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	bufferCopyRegion.imageSubresource.layerCount = 1;
	bufferCopyRegion.imageExtent.width = emptyTexture.width;
	bufferCopyRegion.imageExtent.height = emptyTexture.height;
	bufferCopyRegion.imageExtent.depth = 1;

	// Create optimal tiled target image
	VkImageCreateInfo imageCreateInfo = vkinit::imageCreateInfo();
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageCreateInfo.extent = { emptyTexture.width, emptyTexture.height, 1 };
	imageCreateInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	VK_CHECK_RESULT(vkCreateImage(m_pDevice->m_logicalDevice, &imageCreateInfo, nullptr, &emptyTexture.image));

	vkGetImageMemoryRequirements(m_pDevice->m_logicalDevice, emptyTexture.image, &memReqs);
	memAllocInfo.allocationSize = memReqs.size;
	memAllocInfo.memoryTypeIndex = m_pDevice->getMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	VK_CHECK_RESULT(vkAllocateMemory(m_pDevice->m_logicalDevice, &memAllocInfo, nullptr, &emptyTexture.deviceMemory));
	VK_CHECK_RESULT(vkBindImageMemory(m_pDevice->m_logicalDevice, emptyTexture.image, emptyTexture.deviceMemory, 0));

	VkImageSubresourceRange subresourceRange{};
	subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subresourceRange.baseMipLevel = 0;
	subresourceRange.levelCount = 1;
	subresourceRange.layerCount = 1;

	VkCommandBuffer copyCmd = m_pDevice->createCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);
	tools::setImageLayout(copyCmd, emptyTexture.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, subresourceRange);
	vkCmdCopyBufferToImage(copyCmd, stagingBuffer, emptyTexture.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bufferCopyRegion);
	tools::setImageLayout(copyCmd, emptyTexture.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, subresourceRange);
	m_pDevice->flushCommandBuffer(copyCmd, transferQueue);
	emptyTexture.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	// Clean up staging resources
	vkDestroyBuffer(m_pDevice->m_logicalDevice, stagingBuffer, nullptr);
	vkFreeMemory(m_pDevice->m_logicalDevice, stagingMemory, nullptr);

	VkSamplerCreateInfo samplerCreateInfo = vkinit::samplerCreateInfo();
	samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
	samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
	samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
	samplerCreateInfo.maxAnisotropy = 1.0f;
	VK_CHECK_RESULT(vkCreateSampler(m_pDevice->m_logicalDevice, &samplerCreateInfo, nullptr, &emptyTexture.sampler));

	VkImageViewCreateInfo viewCreateInfo = vkinit::imageViewCreateInfo();
	viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
	viewCreateInfo.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
	viewCreateInfo.subresourceRange.levelCount = 1;
	viewCreateInfo.image = emptyTexture.image;
	VK_CHECK_RESULT(vkCreateImageView(m_pDevice->m_logicalDevice, &viewCreateInfo, nullptr, &emptyTexture.view));

	emptyTexture.descriptor.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	emptyTexture.descriptor.imageView = emptyTexture.view;
	emptyTexture.descriptor.sampler = emptyTexture.sampler;
}

/*
	glTF model loading and rendering class
*/
gltf::Model::~Model()
{
	vkDestroyBuffer(m_pDevice->m_logicalDevice, vertexes.buffer, nullptr);
	vkFreeMemory(m_pDevice->m_logicalDevice, vertexes.memory, nullptr);
	vkDestroyBuffer(m_pDevice->m_logicalDevice, indexes.buffer, nullptr);
	vkFreeMemory(m_pDevice->m_logicalDevice, indexes.memory, nullptr);
	for (auto texture : m_textures) {
		texture.destroy();
	}
	for (auto node : m_nodes) {
		delete node;
	}
	for (auto skin : m_skins) {
		delete skin;
	}
	if (descriptorSetLayoutUbo != VK_NULL_HANDLE) {
		vkDestroyDescriptorSetLayout(m_pDevice->m_logicalDevice, descriptorSetLayoutUbo, nullptr);
		descriptorSetLayoutUbo = VK_NULL_HANDLE;
	}
	if (descriptorSetLayoutImage != VK_NULL_HANDLE) {
		vkDestroyDescriptorSetLayout(m_pDevice->m_logicalDevice, descriptorSetLayoutImage, nullptr);
		descriptorSetLayoutImage = VK_NULL_HANDLE;
	}
	vkDestroyDescriptorPool(m_pDevice->m_logicalDevice, m_descriptorPool, nullptr);
	emptyTexture.destroy();
}

void   gltf::Model::loadNode( Node* parent, const tinygltf::Node& node, uint32_t nodeIndex, const tinygltf::Model& model, ::array_base<uint32_t>& indexBuffer, ::array_base<Vertex>& vertexBuffer, float globalscale)
{
	 Node* newNode = new Node{};
	newNode->index = nodeIndex;
	newNode->parent = parent;
	newNode->name = node.name;
	newNode->skinIndex = node.skin;
	newNode->matrix = floating_matrix4(1.0f);

	// Generate local node matrix
	floating_sequence3 translation = floating_sequence3(0.0f);
	if (node.translation.size() == 3) {
		translation = glm::make_vec3(node.translation.data());
		newNode->translation = translation;
	}
	floating_matrix4 rotation = floating_matrix4(1.0f);
	if (node.rotation.size() == 4) {
		glm::quat q = glm::make_quat(node.rotation.data());
		newNode->rotation = floating_matrix4(q);
	}
	floating_sequence3 scale = floating_sequence3(1.0f);
	if (node.scale.size() == 3) {
		scale = glm::make_vec3(node.scale.data());
		newNode->scale = scale;
	}
	if (node.matrix.size() == 16) {
		newNode->matrix = glm::make_mat4x4(node.matrix.data());
		if (globalscale != 1.0f) {
			//newNode->matrix = glm::scale(newNode->matrix, floating_sequence3(globalscale));
		}
	};

	// Node with children
	if (node.children.size() > 0) {
		for (auto i = 0; i < node.children.size(); i++) {
			loadNode(newNode, model.nodes[node.children[i]], node.children[i], model, indexBuffer, vertexBuffer, globalscale);
		}
	}

	// Node contains mesh data
	if (node.mesh > -1) {
		const tinygltf::Mesh mesh = model.meshes[node.mesh];
		Mesh* newMesh = new Mesh(m_pDevice, newNode->matrix);
		newMesh->name = mesh.name;
		for (size_t j = 0; j < mesh.primitives.size(); j++) {
			const tinygltf::Primitive& primitive = mesh.primitives[j];
			if (primitive.indexes < 0) {
				continue;
			}
			uint32_t indexStart = static_cast<uint32_t>(indexBuffer.size());
			uint32_t vertexStart = static_cast<uint32_t>(vertexBuffer.size());
			uint32_t indexCount = 0;
			uint32_t vertexCount = 0;
			floating_sequence3 posMin{};
			floating_sequence3 posMax{};
			bool hasSkin = false;
			// Vertices
			{
				const float* bufferPos = nullptr;
				const float* bufferNormals = nullptr;
				const float* bufferTexCoords = nullptr;
				const float* bufferColors = nullptr;
				const float* bufferTangents = nullptr;
				uint32_t numColorComponents;
				const uint16_t* bufferJoints = nullptr;
				const float* bufferWeights = nullptr;

				// Position attribute is required
				ASSERT(primitive.attributes.find("POSITION") != primitive.attributes.end());

				const tinygltf::Accessor& posAccessor = model.accessors[primitive.attributes.find("POSITION")->element2()];
				const tinygltf::BufferView& posView = model.bufferViews[posAccessor.bufferView];
				bufferPos = reinterpret_cast<const float*>(&(model.buffers[posView.buffer].data[posAccessor.byteOffset + posView.byteOffset]));
				posMin = floating_sequence3(posAccessor.minValues[0], posAccessor.minValues[1], posAccessor.minValues[2]);
				posMax = floating_sequence3(posAccessor.maxValues[0], posAccessor.maxValues[1], posAccessor.maxValues[2]);

				if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
					const tinygltf::Accessor& normAccessor = model.accessors[primitive.attributes.find("NORMAL")->element2()];
					const tinygltf::BufferView& normView = model.bufferViews[normAccessor.bufferView];
					bufferNormals = reinterpret_cast<const float*>(&(model.buffers[normView.buffer].data[normAccessor.byteOffset + normView.byteOffset]));
				}

				if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
					const tinygltf::Accessor& uvAccessor = model.accessors[primitive.attributes.find("TEXCOORD_0")->element2()];
					const tinygltf::BufferView& uvView = model.bufferViews[uvAccessor.bufferView];
					bufferTexCoords = reinterpret_cast<const float*>(&(model.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
				}

				if (primitive.attributes.find("COLOR_0") != primitive.attributes.end())
				{
					const tinygltf::Accessor& colorAccessor = model.accessors[primitive.attributes.find("COLOR_0")->element2()];
					const tinygltf::BufferView& colorView = model.bufferViews[colorAccessor.bufferView];
					// Color buffer are either of type vec3 or vec4
					numColorComponents = colorAccessor.type == TINYGLTF_PARAMETER_TYPE_FLOAT_VEC3 ? 3 : 4;
					bufferColors = reinterpret_cast<const float*>(&(model.buffers[colorView.buffer].data[colorAccessor.byteOffset + colorView.byteOffset]));
				}

				if (primitive.attributes.find("TANGENT") != primitive.attributes.end())
				{
					const tinygltf::Accessor& tangentAccessor = model.accessors[primitive.attributes.find("TANGENT")->element2()];
					const tinygltf::BufferView& tangentView = model.bufferViews[tangentAccessor.bufferView];
					bufferTangents = reinterpret_cast<const float*>(&(model.buffers[tangentView.buffer].data[tangentAccessor.byteOffset + tangentView.byteOffset]));
				}

				// Skinning
				// Joints
				if (primitive.attributes.find("JOINTS_0") != primitive.attributes.end()) {
					const tinygltf::Accessor& jointAccessor = model.accessors[primitive.attributes.find("JOINTS_0")->element2()];
					const tinygltf::BufferView& jointView = model.bufferViews[jointAccessor.bufferView];
					bufferJoints = reinterpret_cast<const uint16_t*>(&(model.buffers[jointView.buffer].data[jointAccessor.byteOffset + jointView.byteOffset]));
				}

				if (primitive.attributes.find("WEIGHTS_0") != primitive.attributes.end()) {
					const tinygltf::Accessor& uvAccessor = model.accessors[primitive.attributes.find("WEIGHTS_0")->element2()];
					const tinygltf::BufferView& uvView = model.bufferViews[uvAccessor.bufferView];
					bufferWeights = reinterpret_cast<const float*>(&(model.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
				}

				hasSkin = (bufferJoints && bufferWeights);

				vertexCount = static_cast<uint32_t>(posAccessor.count);

				for (size_t v = 0; v < posAccessor.count; v++) {
					Vertex vert{};
					vert.pos = floating_sequence4(glm::make_vec3(&bufferPos[v * 3]), 1.0f);
					vert.normal = glm::normalize(floating_sequence3(bufferNormals ? glm::make_vec3(&bufferNormals[v * 3]) : floating_sequence3(0.0f)));
					vert.uv = bufferTexCoords ? glm::make_vec2(&bufferTexCoords[v * 2]) : floating_sequence3(0.0f);
					if (bufferColors) {
						switch (numColorComponents) {
						case 3:
							vert.color = floating_sequence4(glm::make_vec3(&bufferColors[v * 3]), 1.0f);
						case 4:
							vert.color = glm::make_vec4(&bufferColors[v * 4]);
						}
					}
					else {
						vert.color = floating_sequence4(1.0f);
					}
					vert.tangent = bufferTangents ? floating_sequence4(glm::make_vec4(&bufferTangents[v * 4])) : floating_sequence4(0.0f);
					vert.joint0 = hasSkin ? floating_sequence4(glm::make_vec4(&bufferJoints[v * 4])) : floating_sequence4(0.0f);
					vert.weight0 = hasSkin ? glm::make_vec4(&bufferWeights[v * 4]) : floating_sequence4(0.0f);
					vertexBuffer.add(vert);
				}
			}
			// Indices
			{
				const tinygltf::Accessor& accessor = model.accessors[primitive.indexes];
				const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
				const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

				indexCount = static_cast<uint32_t>(accessor.count);

				switch (accessor.componentType) {
				case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT: {
					uint32_t* buf = new uint32_t[accessor.count];
					memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint32_t));
					for (size_t index = 0; index < accessor.count; index++) {
						indexBuffer.add(buf[index] + vertexStart);
					}
					delete[] buf;
					break;
				}
				case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
					uint16_t* buf = new uint16_t[accessor.count];
					memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint16_t));
					for (size_t index = 0; index < accessor.count; index++) {
						indexBuffer.add(buf[index] + vertexStart);
					}
					delete[] buf;
					break;
				}
				case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
					uint8_t* buf = new uint8_t[accessor.count];
					memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint8_t));
					for (size_t index = 0; index < accessor.count; index++) {
						indexBuffer.add(buf[index] + vertexStart);
					}
					delete[] buf;
					break;
				}
				default:
					error() << "Index component type " << accessor.componentType << " not supported!" ;
					return;
				}
			}
			Primitive* newPrimitive = new Primitive(indexStart, indexCount, primitive.material > -1 ? m_materials[primitive.material] : m_materials.back());
			newPrimitive->firstVertex = vertexStart;
			newPrimitive->vertexCount = vertexCount;
			newPrimitive->setDimensions(posMin, posMax);
			newMesh->primitives.add(newPrimitive);
		}
		newNode->mesh = newMesh;
	}
	if (parent) {
		parent->children.add(newNode);
	}
	else {
		m_nodes.add(newNode);
	}
	m_linearNodes.add(newNode);
}

void   gltf::Model::loadSkins(tinygltf::Model& gltfModel)
{
	for (tinygltf::Skin& source : gltfModel.skins) {
		Skin* newSkin = new Skin{};
		newSkin->name = source.name;

		// Find skeleton root node
		if (source.skeleton > -1) {
			newSkin->skeletonRoot = nodeFromIndex(source.skeleton);
		}

		// Find joint nodes
		for (int jointIndex : source.joints) {
			Node* node = nodeFromIndex(jointIndex);
			if (node) {
				newSkin->joints.add(nodeFromIndex(jointIndex));
			}
		}

		// Get inverse bind matrices from buffer
		if (source.inverseBindMatrices > -1) {
			const tinygltf::Accessor& accessor = gltfModel.accessors[source.inverseBindMatrices];
			const tinygltf::BufferView& bufferView = gltfModel.bufferViews[accessor.bufferView];
			const tinygltf::Buffer& buffer = gltfModel.buffers[bufferView.buffer];
			newSkin->inverseBindMatrices.resize(accessor.count);
			memcpy(newSkin->inverseBindMatrices.data(), &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(floating_matrix4));
		}

		m_skins.add(newSkin);
	}
}

void gltf::Model::loadImages(tinygltf::Model& gltfModel, device* pdevice, VkQueue transferQueue)
{
	for (tinygltf::Image& image : gltfModel.images) {
		 Texture texture;
		texture.fromglTfImage(image, m_path, pdevice, transferQueue, false);
		texture.index = static_cast<uint32_t>(m_textures.size());
		m_textures.add(texture);
	}
	// Create an empty texture to be used for empty material images
	createEmptyTexture(transferQueue);
	emptyTexture.descriptor.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
}
void gltf::Model::loadMaterials(tinygltf::Model& gltfModel)
{
	for (tinygltf::Material& mat : gltfModel.materials) {
		 Material material(m_pDevice);
		if (mat.values.find("baseColorTexture") != mat.values.end()) {
			material.baseColorTexture = getTexture(gltfModel.textures[mat.values["baseColorTexture"].TextureIndex()].source);
		}
		// Metallic roughness workflow
		if (mat.values.find("metallicRoughnessTexture") != mat.values.end()) {
			material.metallicRoughnessTexture = getTexture(gltfModel.textures[mat.values["metallicRoughnessTexture"].TextureIndex()].source);
		}
		if (mat.values.find("roughnessFactor") != mat.values.end()) {
			material.roughnessFactor = static_cast<float>(mat.values["roughnessFactor"].Factor());
		}
		if (mat.values.find("metallicFactor") != mat.values.end()) {
			material.metallicFactor = static_cast<float>(mat.values["metallicFactor"].Factor());
		}
		if (mat.values.find("baseColorFactor") != mat.values.end()) {
			material.baseColorFactor = glm::make_vec4(mat.values["baseColorFactor"].ColorFactor().data());
		}
		if (mat.additionalValues.find("normalTexture") != mat.additionalValues.end()) {
			material.normalTexture = getTexture(gltfModel.textures[mat.additionalValues["normalTexture"].TextureIndex()].source);
		}
		else {
			material.normalTexture = &emptyTexture;
		}
		if (mat.additionalValues.find("emissiveTexture") != mat.additionalValues.end()) {
			material.emissiveTexture = getTexture(gltfModel.textures[mat.additionalValues["emissiveTexture"].TextureIndex()].source);
		}
		if (mat.additionalValues.find("occlusionTexture") != mat.additionalValues.end()) {
			material.occlusionTexture = getTexture(gltfModel.textures[mat.additionalValues["occlusionTexture"].TextureIndex()].source);
		}
		material.alphaMode = Material::ALPHAMODE_OPAQUE;

		auto itAlpha = mat.additionalValues.find("alphaMode");
		if (itAlpha != mat.additionalValues.end()) {
			const ::scoped_string & mode = itAlpha->element2().string_value;
			if (mode == "BLEND") {
				material.alphaMode = Material::ALPHAMODE_BLEND;
			}
			else if (mode == "MASK") {
				material.alphaMode = Material::ALPHAMODE_MASK;
			}
			// else leave as OPAQUE
		}
		if (mat.additionalValues.find("alphaCutoff") != mat.additionalValues.end()) {
			material.alphaCutoff = static_cast<float>(mat.additionalValues["alphaCutoff"].Factor());
		}

		m_materials.add(material);
	}
	// Push a default material at the end of the list for meshes with no material assigned
	m_materials.add(Material(m_pDevice));
}





void  gltf::Model::loadFromFile(::string filename, device* pdevice, VkQueue transferQueue, uint32_t fileLoadingFlags, float scale)
{
	tinygltf::Model gltfModel;
	tinygltf::TinyGLTF gltfContext;

	if (fileLoadingFlags & FileLoadingFlags::DontLoadImages) {
		gltfContext.SetImageLoader(loadImageDataFuncEmpty, nullptr);
	}
	else {
		gltfContext.SetImageLoader(loadImageDataFunc, nullptr);
	}
#if defined(__ANDROID__)
	// On Android all assets are packed with the apk in a compressed form, so we need to open them using the asset manager
	// We let tinygltf handle this, by passing the asset manager of our app
	tinygltf::asset_manager = androidApp->activity->assetManager;
#endif
	size_t pos = filename.find_last_of('/');
	m_path = filename.substr(0, pos);

	::string error, warning;

	this->m_pDevice = pdevice;

#if defined(__ANDROID__)
	// On Android all assets are packed with the apk in a compressed form, so we need to open them using the asset manager
	// We let tinygltf handle this, by passing the asset manager of our app
	tinygltf::asset_manager = androidApp->activity->assetManager;
#endif
	bool fileLoaded = gltfContext.LoadASCIIFromFile(&gltfModel, &error, &warning, filename);

	::array_base<uint32_t> indexBuffer;
	::array_base<Vertex> vertexBuffer;

	if (fileLoaded) {
		if (!(fileLoadingFlags & FileLoadingFlags::DontLoadImages)) {
			loadImages(gltfModel, pdevice, transferQueue);
		}
		loadMaterials(gltfModel);
		const tinygltf::Scene& scene = gltfModel.scenes[gltfModel.defaultScene > -1 ? gltfModel.defaultScene : 0];
		for (size_t i = 0; i < scene.nodes.size(); i++) {
			const tinygltf::Node node = gltfModel.nodes[scene.nodes[i]];
			loadNode(nullptr, node, scene.nodes[i], gltfModel, indexBuffer, vertexBuffer, scale);
		}
		if (gltfModel.animations.size() > 0) {
			loadAnimations(gltfModel);
		}
		loadSkins(gltfModel);

		for (auto node : m_linearNodes) {
			// Assign skins
			if (node->skinIndex > -1) {
				node->skin = m_skins[node->skinIndex];
			}
			// Initial pose
			if (node->mesh) {
				node->update();
			}
		}
	}
	else {
		tools::exitFatal("Could not load glTF file \"" + filename + "\": " + error, -1);
		return;
	}

	// Pre-Calculations for requested features
	if ((fileLoadingFlags & FileLoadingFlags::PreTransformVertices) || (fileLoadingFlags & FileLoadingFlags::PreMultiplyVertexColors) || (fileLoadingFlags & FileLoadingFlags::FlipY)) {
		const bool preTransform = fileLoadingFlags & FileLoadingFlags::PreTransformVertices;
		const bool preMultiplyColor = fileLoadingFlags & FileLoadingFlags::PreMultiplyVertexColors;
		const bool flipY = fileLoadingFlags & FileLoadingFlags::FlipY;
		for (Node* node : m_linearNodes) {
			if (node->mesh) {
				const floating_matrix4 localMatrix = node->getMatrix();
				for (Primitive* primitive : node->mesh->primitives) {
					for (uint32_t i = 0; i < primitive->vertexCount; i++) {
						Vertex& vertex = vertexBuffer[primitive->firstVertex + i];
						// Pre-transform vertex positions by node-hierarchy
						if (preTransform) {
							vertex.pos = floating_sequence3(localMatrix * floating_sequence4(vertex.pos, 1.0f));
							vertex.normal = glm::normalize(floating_matrix3(localMatrix) * vertex.normal);
						}
						// Flip Y-Axis of vertex positions
						if (flipY) {
							vertex.pos.y *= -1.0f;
							vertex.normal.y *= -1.0f;
						}
						// Pre-Multiply vertex colors with material base color
						if (preMultiplyColor) {
							vertex.color = primitive->material.baseColorFactor * vertex.color;
						}
					}
				}
			}
		}
	}

	for (auto extension : gltfModel.extensionsUsed) {
		if (extension == "KHR_materials_pbrSpecularGlossiness") {
			information() << "Required extension: " << extension;
			m_bMetallicRoughnessWorkflow = false;
		}
	}

	size_t vertexBufferSize = vertexBuffer.size() * sizeof(Vertex);
	size_t indexBufferSize = indexBuffer.size() * sizeof(uint32_t);
	indexes.count = static_cast<uint32_t>(indexBuffer.size());
	vertexes.count = static_cast<uint32_t>(vertexBuffer.size());

	ASSERT((vertexBufferSize > 0) && (indexBufferSize > 0));

	struct StagingBuffer {
		VkBuffer buffer;
		VkDeviceMemory memory;
	} vertexStaging, indexStaging;


	// Create staging buffers
	// Vertex data
	VK_CHECK_RESULT(pdevice->createBuffer(
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		vertexBufferSize,
		&vertexStaging.buffer,
		&vertexStaging.memory,
		vertexBuffer.data()));
	// Index data
	VK_CHECK_RESULT(pdevice->createBuffer(
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		indexBufferSize,
		&indexStaging.buffer,
		&indexStaging.memory,
		indexBuffer.data()));

	// Create pdevice local buffers
	// Vertex buffer
	VK_CHECK_RESULT(pdevice->createBuffer(
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | memoryPropertyFlags,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		vertexBufferSize,
		&vertexes.buffer,
		&vertexes.memory));
	// Index buffer
	VK_CHECK_RESULT(pdevice->createBuffer(
		VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | memoryPropertyFlags,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		indexBufferSize,
		&indexes.buffer,
		&indexes.memory));


	// Copy from staging buffers
	VkCommandBuffer copyCmd = pdevice->createCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);

	VkBufferCopy copyRegion = {};

	copyRegion.size = vertexBufferSize;
	vkCmdCopyBuffer(copyCmd, vertexStaging.buffer, vertexes.buffer, 1, &copyRegion);

	copyRegion.size = indexBufferSize;
	vkCmdCopyBuffer(copyCmd, indexStaging.buffer, indexes.buffer, 1, &copyRegion);

	pdevice->flushCommandBuffer(copyCmd, transferQueue, true);

	vkDestroyBuffer(pdevice->m_logicalDevice, vertexStaging.buffer, nullptr);
	vkFreeMemory(pdevice->m_logicalDevice, vertexStaging.memory, nullptr);
	vkDestroyBuffer(pdevice->m_logicalDevice, indexStaging.buffer, nullptr);
	vkFreeMemory(pdevice->m_logicalDevice, indexStaging.memory, nullptr);

	getSceneDimensions();

	// Setup descriptors
	uint32_t uboCount{ 0 };
	uint32_t imageCount{ 0 };
	for (auto node : m_linearNodes) {
		if (node->mesh) {
			uboCount++;
		}
	}
	for (auto material : m_materials) {
		if (material.baseColorTexture != nullptr) {
			imageCount++;
		}
	}
	uint32_t materialCount = 0;
	for (auto& m : m_materials) {
		if (m.baseColorTexture) ++materialCount;
	}

	::array_base<VkDescriptorPoolSize> poolSizes;

	if (uboCount > 0) {
		poolSizes.add({ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, uboCount });
	}

	uint32_t samplerCount = materialCount * 2;  // baseColor + normal per material
	if (samplerCount > 0) {
		poolSizes.add({ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, samplerCount });
	}

	VkDescriptorPoolCreateInfo descriptorPoolCI{};
	descriptorPoolCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolCI.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	descriptorPoolCI.pPoolSizes = poolSizes.data();
	descriptorPoolCI.maxSets = uboCount + materialCount;
	VK_CHECK_RESULT(vkCreateDescriptorPool(pdevice->m_logicalDevice, &descriptorPoolCI, nullptr, &m_descriptorPool));

	// Descriptors for per-node uniform buffers
	{
		{
			// Layout is global, so only create if it hasn't already been created before
			if (descriptorSetLayoutUbo == VK_NULL_HANDLE) {
				::array_base<VkDescriptorSetLayoutBinding> setLayoutBindings = {
					vkinit::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 0),
				};
				VkDescriptorSetLayoutCreateInfo descriptorLayoutCI{};
				descriptorLayoutCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
				descriptorLayoutCI.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
				descriptorLayoutCI.pBindings = setLayoutBindings.data();
				VK_CHECK_RESULT(vkCreateDescriptorSetLayout(pdevice->m_logicalDevice, &descriptorLayoutCI, nullptr, &descriptorSetLayoutUbo));
			}
			for (auto node : m_nodes) {
				prepareNodeDescriptor(node, descriptorSetLayoutUbo);
			}
		}

		// Descriptors for per-material images
		{
			// Layout is global, so only create if it hasn't already been created before
			if (descriptorSetLayoutImage == VK_NULL_HANDLE) {
				// Always push exactly two bindings:
				//  • binding 0 = base-color sampler
				//  • binding 1 = normal-map sampler
				::preallocated_array_base< ::array_base <VkDescriptorSetLayoutBinding, 2> > setLayoutBindings = {
					// binding 0 → base-color
					vkinit::descriptorSetLayoutBinding(
						VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
						VK_SHADER_STAGE_FRAGMENT_BIT,
						/*binding=*/ 0
					),
						// binding 1 → normal map
						vkinit::descriptorSetLayoutBinding(
							VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
							VK_SHADER_STAGE_FRAGMENT_BIT,
							/*binding=*/ 1
						)
				};
				//::preallocated_array_base< ::array_base <VkDescriptorSetLayoutBinding, 4> > setLayoutBindings = {
				//	// baseColor
				//	vkinit::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0),
				//	// normal
				//	vkinit::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1),
				//	// metallic-roughness
				//	vkinit::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 2),
				//	// ambient occlusion
				//	vkinit::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 3)
				//};

				VkDescriptorSetLayoutCreateInfo descriptorLayoutCI{};
				descriptorLayoutCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
				descriptorLayoutCI.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
				descriptorLayoutCI.pBindings = setLayoutBindings.data();
				VK_CHECK_RESULT(vkCreateDescriptorSetLayout(
					pdevice->m_logicalDevice,
					&descriptorLayoutCI,
					nullptr,
					&descriptorSetLayoutImage
				));
			}

			for (auto& material : m_materials) {
				if (material.baseColorTexture != nullptr) {
					material.createDescriptorSet(m_descriptorPool,  descriptorSetLayoutImage, descriptorBindingFlags, &emptyTexture);
				}
			}
		}
	}
}



void  gltf::Model::drawNode(Node* node, VkCommandBuffer commandBuffer, uint32_t renderFlags, VkPipelineLayout pipelineLayout, uint32_t bindImageSet)
{
	if (node->mesh) {
		for (Primitive* primitive : node->mesh->primitives) {
			bool skip = false;
			const  Material& material = primitive->material;
			if (renderFlags & RenderFlags::RenderOpaqueNodes) {
				skip = (material.alphaMode != Material::ALPHAMODE_OPAQUE);
			}
			if (renderFlags & RenderFlags::RenderAlphaMaskedNodes) {
				skip = (material.alphaMode != Material::ALPHAMODE_MASK);
			}
			if (renderFlags & RenderFlags::RenderAlphaBlendedNodes) {
				skip = (material.alphaMode != Material::ALPHAMODE_BLEND);
			}
			if (!skip) {
				if (renderFlags & RenderFlags::BindImages) {
					vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, bindImageSet, 1, &material.descriptorSet, 0, nullptr);
				}
				vkCmdDrawIndexed(commandBuffer, primitive->indexCount, 1, primitive->firstIndex, 0, 0);
			}
		}
	}
	for (auto& child : node->children) {
		drawNode(child, commandBuffer, renderFlags, pipelineLayout, bindImageSet);
	}
}
void  gltf::Model::bind(VkCommandBuffer commandBuffer)
{
	const VkDeviceSize offsets[1] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexes.buffer, offsets);
	vkCmdBindIndexBuffer(commandBuffer, indexes.buffer, 0, VK_INDEX_TYPE_UINT32);
	m_bBuffersBound = true;
}
void  gltf::Model::gltfDraw(VkCommandBuffer commandBuffer, uint32_t renderFlags, VkPipelineLayout pipelineLayout, uint32_t bindImageSet)
{
	if (!m_bBuffersBound) {
		const VkDeviceSize offsets[1] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexes.buffer, offsets);
		vkCmdBindIndexBuffer(commandBuffer, indexes.buffer, 0, VK_INDEX_TYPE_UINT32);
	}
	for (auto& node : m_nodes) {
		drawNode(node, commandBuffer, renderFlags, pipelineLayout, bindImageSet);
	}
}


void  gltf::Model::getNodeDimensions(Node* node, floating_sequence3& min, floating_sequence3& max)
{
	if (node->mesh) {
		for (Primitive* primitive : node->mesh->primitives) {
			floating_sequence4 locMin = floating_sequence4(primitive->dimensions.min, 1.0f) * node->getMatrix();
			floating_sequence4 locMax = floating_sequence4(primitive->dimensions.max, 1.0f) * node->getMatrix();
			if (locMin.x < min.x) { min.x = locMin.x; }
			if (locMin.y < min.y) { min.y = locMin.y; }
			if (locMin.z < min.z) { min.z = locMin.z; }
			if (locMax.x > max.x) { max.x = locMax.x; }
			if (locMax.y > max.y) { max.y = locMax.y; }
			if (locMax.z > max.z) { max.z = locMax.z; }
		}
	}
	for (auto child : node->children) {
		getNodeDimensions(child, min, max);
	}
}

void  gltf::Model::getSceneDimensions()
{
	dimensions.min = floating_sequence3(FLT_MAX);
	dimensions.max = floating_sequence3(-FLT_MAX);
	for (auto node : m_nodes) {
		getNodeDimensions(node, dimensions.min, dimensions.max);
	}
	dimensions.size = dimensions.max - dimensions.min;
	dimensions.center = (dimensions.min + dimensions.max) / 2.0f;
	dimensions.radius = glm::distance(dimensions.min, dimensions.max) / 2.0f;
}

void gltf::Model::loadAnimations(tinygltf::Model& gltfModel)
{
	for (tinygltf::Animation& anim : gltfModel.animations) {
		 Animation animation{};
		animation.name = anim.name;
		if (anim.name.empty()) {
			animation.name = ::as_string(m_animations.size());
		}

		// Samplers
		for (auto& samp : anim.samplers) {
			 AnimationSampler sampler{};

			if (samp.interpolation == "LINEAR") {
				sampler.interpolation = AnimationSampler::InterpolationType::LINEAR;
			}
			if (samp.interpolation == "STEP") {
				sampler.interpolation = AnimationSampler::InterpolationType::STEP;
			}
			if (samp.interpolation == "CUBICSPLINE") {
				sampler.interpolation = AnimationSampler::InterpolationType::CUBICSPLINE;
			}

			// Read sampler input time values
			{
				const tinygltf::Accessor& accessor = gltfModel.accessors[samp.input];
				const tinygltf::BufferView& bufferView = gltfModel.bufferViews[accessor.bufferView];
				const tinygltf::Buffer& buffer = gltfModel.buffers[bufferView.buffer];

				ASSERT(accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);

				float* buf = new float[accessor.count];
				memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(float));
				for (size_t index = 0; index < accessor.count; index++) {
					sampler.inputs.add(buf[index]);
				}
				delete[] buf;
				for (auto input : sampler.inputs) {
					if (input < animation.start) {
						animation.start = input;
					};
					if (input > animation.end) {
						animation.end = input;
					}
				}
			}

			// Read sampler output T/R/S values 
			{
				const tinygltf::Accessor& accessor = gltfModel.accessors[samp.output];
				const tinygltf::BufferView& bufferView = gltfModel.bufferViews[accessor.bufferView];
				const tinygltf::Buffer& buffer = gltfModel.buffers[bufferView.buffer];

				ASSERT(accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);

				switch (accessor.type) {
				case TINYGLTF_TYPE_VEC3: {
					floating_sequence3* buf = new floating_sequence3[accessor.count];
					memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(floating_sequence3));
					for (size_t index = 0; index < accessor.count; index++) {
						sampler.outputsVec4.add(floating_sequence4(buf[index], 0.0f));
					}
					delete[] buf;
					break;
				}
				case TINYGLTF_TYPE_VEC4: {
					floating_sequence4* buf = new floating_sequence4[accessor.count];
					memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(floating_sequence4));
					for (size_t index = 0; index < accessor.count; index++) {
						sampler.outputsVec4.add(buf[index]);
					}
					delete[] buf;
					break;
				}
				default: {
					information() << "unknown type" ;
					break;
				}
				}
			}

			animation.samplers.add(sampler);
		}

		// Channels
		for (auto& source : anim.channels) {
			 AnimationChannel channel{};

			if (source.target_path == "rotation") {
				channel.path = AnimationChannel::PathType::ROTATION;
			}
			if (source.target_path == "translation") {
				channel.path = AnimationChannel::PathType::TRANSLATION;
			}
			if (source.target_path == "scale") {
				channel.path = AnimationChannel::PathType::SCALE;
			}
			if (source.target_path == "weights") {
				information() << "weights not yet supported, skipping channel" ;
				continue;
			}
			channel.samplerIndex = source.sampler;
			channel.node = nodeFromIndex(source.target_node);
			if (!channel.node) {
				continue;
			}

			animation.channels.add(channel);
		}

		m_animations.add(animation);
	}
}

void  gltf::Model::updateAnimation(uint32_t index, float time)
{
	if (index > static_cast<uint32_t>(m_animations.size()) - 1) {
		information() << "No animation with index " << index ;
		return;
	}
	Animation& animation = m_animations[index];

	bool updated = false;
	for (auto& channel : animation.channels) {
		 AnimationSampler& sampler = animation.samplers[channel.samplerIndex];
		if (sampler.inputs.size() > sampler.outputsVec4.size()) {
			continue;
		}

		for (auto i = 0; i < sampler.inputs.size() - 1; i++) {
			if ((time >= sampler.inputs[i]) && (time <= sampler.inputs[i + 1])) {
				float u = std::max(0.0f, time - sampler.inputs[i]) / (sampler.inputs[i + 1] - sampler.inputs[i]);
				if (u <= 1.0f) {
					switch (channel.path) {
					case  AnimationChannel::PathType::TRANSLATION: {
						floating_sequence4 trans = glm::mix(sampler.outputsVec4[i], sampler.outputsVec4[i + 1], u);
						channel.node->translation = floating_sequence3(trans);
						break;
					}
					case  AnimationChannel::PathType::SCALE: {
						floating_sequence4 trans = glm::mix(sampler.outputsVec4[i], sampler.outputsVec4[i + 1], u);
						channel.node->scale = floating_sequence3(trans);
						break;
					}
					case  AnimationChannel::PathType::ROTATION: {
						glm::quat q1;
						q1.x = sampler.outputsVec4[i].x;
						q1.y = sampler.outputsVec4[i].y;
						q1.z = sampler.outputsVec4[i].z;
						q1.w = sampler.outputsVec4[i].w;
						glm::quat q2;
						q2.x = sampler.outputsVec4[i + 1].x;
						q2.y = sampler.outputsVec4[i + 1].y;
						q2.z = sampler.outputsVec4[i + 1].z;
						q2.w = sampler.outputsVec4[i + 1].w;
						channel.node->rotation = glm::normalize(glm::slerp(q1, q2, u));
						break;
					}
					}
					updated = true;
				}
			}
		}
	}
	if (updated) {
		for (auto& node : m_nodes) {
			node->update();
		}
	}
}

/*
	Helper functions
*/
gltf::Node* gltf::Model::findNode(Node* parent, uint32_t index) {
	Node* nodeFound = nullptr;
	if (parent->index == index) {
		return parent;
	}
	for (auto& child : parent->children) {
		nodeFound = findNode(child, index);
		if (nodeFound) {
			break;
		}
	}
	return nodeFound;
}

gltf::Node* gltf::Model::nodeFromIndex(uint32_t index) {
	Node* nodeFound = nullptr;
	for (auto& node : m_nodes) {
		nodeFound = findNode(node, index);
		if (nodeFound) {
			break;
		}
	}
	return nodeFound;
}

void  gltf::Model::prepareNodeDescriptor( Node* node, VkDescriptorSetLayout descriptorSetLayout) {
	if (node->mesh) {
		VkDescriptorSetAllocateInfo descriptorSetAllocInfo{};
		descriptorSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptorSetAllocInfo.descriptorPool = m_descriptorPool;
		descriptorSetAllocInfo.pSetLayouts = &descriptorSetLayout;
		descriptorSetAllocInfo.descriptorSetCount = 1;
		VK_CHECK_RESULT(vkAllocateDescriptorSets(m_pDevice->device(), &descriptorSetAllocInfo, &node->mesh->uniformBuffer.descriptorSet));

		VkWriteDescriptorSet writeDescriptorSet{};
		writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		writeDescriptorSet.descriptorCount = 1;
		writeDescriptorSet.dstSet = node->mesh->uniformBuffer.descriptorSet;
		writeDescriptorSet.dstBinding = 0;
		writeDescriptorSet.pBufferInfo = &node->mesh->uniformBuffer.descriptor;

		vkUpdateDescriptorSets(m_pDevice->device(), 1, &writeDescriptorSet, 0, nullptr);
	}
	for (auto& child : node->children) {
		prepareNodeDescriptor(child, descriptorSetLayout);
	}
}


} // namespace sandbox_renderer


