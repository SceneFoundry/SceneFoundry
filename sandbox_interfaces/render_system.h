// sandbox_renderer/include/IRenderSystem.h
#pragma once
//#include <vulkan/vulkan.h>
//#include "common/render_data.h"
// #include "SceneFoundry/sandbox_renderer/device.h"
// //#include "SceneFoundry/sandbox_renderer/descriptor.h"
// #include "SceneFoundry/sandbox_interfaces/renderer_i.h"
// #include "SceneFoundry/sandbox_interfaces/frame_info.h"

namespace sandbox_interfaces
{

    struct IBuffer  :
        virtual public ::particle
    {
    public:

        virtual int size() = 0;
        virtual void * data() = 0;
    };


    struct IRenderSystem  :
        virtual public ::particle
    {
    public:
        //virtual ~IRenderSystem() = default;

        //
        // virtual void init(
        //    sandbox_renderer::device * device,
        //     VkRenderPass            renderPass,
        //     VkDescriptorSetLayout   globalSetLayout
        //     //,   sandbox_renderer::sandbox_descriptor_pool& descriptorPool)
        // {};
        // virtual void init(
        //    sandbox_renderer::device * device,
        //     VkRenderPass            renderPass,
        //     VkDescriptorSetLayout   globalSetLayout,
        //    sandbox_renderer::sandbox_descriptor_pool& descriptorPool,
        //     size_t frameCount)
        // {};

        virtual void update(IFrame * pframe, IBuffer * pbufferGlobalUbo) = 0;

        virtual void render(IFrame * pframe)  = 0;


    };



} // namespace sandbox_interfaces
