#pragma once

#include <vulkan/vulkan.h>
#include "SceneFoundry/sandbox_renderer/include/vulkan_wrapper/vulkan_device.h"


namespace vulkan
{



   class sandbox_image {
   public:
      sandbox_image() = default;
      ~sandbox_image();

      sandbox_image(const sandbox_image&) = delete;
      sandbox_image& operator=(const sandbox_image&) = delete;

      void create(
         sandbox_device* device,
         uint32_t width,
         uint32_t height,
         VkFormat format,
         VkImageUsageFlags usage,
         VkImageAspectFlags aspectMask,
         VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT,
         VkImageLayout initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
         VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL,
         VkMemoryPropertyFlags memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
      );

      void destroy();

      VkImage getImage() const { return m_image; }
      VkImageView getImageView() const { return m_imageView; }
      VkFormat getFormat() const { return m_format; }
      uint32_t getWidth() const { return m_width; }
      uint32_t getHeight() const { return m_height; }

   private:
      sandbox_device* m_pDevice = nullptr;

      uint32_t m_width = 0;
      uint32_t m_height = 0;
      VkFormat m_format = VK_FORMAT_UNDEFINED;

      VkImage m_image = VK_NULL_HANDLE;
      VkDeviceMemory m_memory = VK_NULL_HANDLE;
      VkImageView m_imageView = VK_NULL_HANDLE;
   };


} // namespace vulkan


