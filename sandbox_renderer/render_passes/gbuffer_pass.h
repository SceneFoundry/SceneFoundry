#pragma once
#include "image.h"
#include <vulkan/vulkan.h>

namespace sandbox_renderer
{

   struct GBuffer {
      sandbox_image albedo;
      sandbox_image normal;
      sandbox_image material;
      sandbox_image depth;

      VkFramebuffer framebuffer = VK_NULL_HANDLE;

      void destroy(device* pdevice) {
         framebuffer = VK_NULL_HANDLE; // Destroyed by swapchain system
         albedo.destroy();
         normal.destroy();
         material.destroy();
         depth.destroy();
      }
   };


} // namespace sandbox_renderer

