// sandbox_renderer/include/IRenderSystem.h
#pragma once
#include <vulkan/vulkan.h>
//#include "common/render_data.h"
#include "SceneFoundry/sandbox_renderer/include/vulkan_wrapper/vulkan_device.h"
#include "SceneFoundry/sandbox_renderer/include/vulkan_wrapper/vulkan_descriptor.h"
#include "SceneFoundry/core_interfaces/include/interfaces/renderer_i.h"
#include "SceneFoundry/core_interfaces/include/frame_info.h"


struct IRenderSystem  :
	virtual public ::particle{
public:
    //virtual ~IRenderSystem() = default;


    virtual void init(
        vulkan::sandbox_device& device,
        VkRenderPass            renderPass,
        VkDescriptorSetLayout   globalSetLayout,
        vulkan::sandbox_descriptor_pool& descriptorPool)
    {};   
    virtual void init(
        vulkan::sandbox_device& device,
        VkRenderPass            renderPass,
        VkDescriptorSetLayout   globalSetLayout,
        vulkan::sandbox_descriptor_pool& descriptorPool,
        size_t frameCount)
    {};

    virtual void update(FrameInfo& frame, GlobalUbo& ubo) {
  
    }

    virtual void render(FrameInfo& frame) {}


};