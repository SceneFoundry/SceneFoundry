#pragma once

#include "SceneFoundry/sandbox_interfaces/render_system.h"
//#include "SceneFoundry/sandbox_interfaces/asset_provider.h"
//#include "device.h"
//#include "SceneFoundry/sandbox_renderer/pipeline.h"
//#include "SceneFoundry/sandbox_renderer/descriptor.h"
//#include "gltf.h"
//#include <vulkan/vulkan.h>

// STD
//#include <memory>
//#include <vector>
//#include <array>

namespace sandbox_renderer
{


    class CLASS_DECL_SANDBOX_RENDERER skybox_ibl_render_system :
        public ::sandbox_interfaces::IRenderSystem
    {
    public:


        //VkDescriptorSetLayout m_skyboxLayout;
        ::pointer < ::sandbox_renderer::device > m_pgpudevice;
        ::pointer<sandbox_renderer::pipeline> m_ppipeline;
        //VkPipelineLayout m_pipelineLayout;
        //VkDescriptorImageInfo m_skyboxImageInfo{};
        //::pointer<sandbox_renderer::sandbox_descriptor_set_layout> m_skyboxSetLayout;
        //VkDescriptorSet m_skyboxDescriptorSet;

        //sandbox_renderer::sandbox_descriptor_pool* m_descriptorPool = nullptr;
        bool m_bHasCubemap = false;


        //skybox_ibl_render_system(sandbox_renderer::device * pdevice, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
        skybox_ibl_render_system();
        ~skybox_ibl_render_system();

        // skybox_ibl_render_system(const skybox_ibl_render_system&) = delete;
        // skybox_ibl_render_system& operator=(const skybox_ibl_render_system&) = delete;

        // void init(
        //    sandbox_renderer::device * pdevice,
        //     VkRenderPass            renderPass,
        //     VkDescriptorSetLayout   globalSetLayout,
        //    sandbox_renderer::sandbox_descriptor_pool& descriptorPool,
        //     size_t frameCount)override;

        // Call this inside your scene render loop, after global descriptors are bound
        void render(::sandbox_interfaces::IFrame * pframe) override;

        //
        // inline void setCubemapTexture(const VkDescriptorImageInfo& info) {
        //     m_skyboxImageInfo = info;
        //     m_bHasCubemap = true;
        //     allocateAndWriteSkyboxDescriptorSet();
        // }
        //
        // inline void setCubemapByName(const ::scoped_string & name, const IAssetProvider& provider) {
        //     VkDescriptorImageInfo desc = provider.getCubemapDescriptor(name);
        //     setCubemapTexture(desc); // <--- this is where we can handle descriptor set allocation
        // }

        void createSkyboxDescriptorSetLayout();
        void allocateAndWriteSkyboxDescriptorSet();
    //private:
        //void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
        //void createPipeline(VkRenderPass renderPass);




    };


} // namespace sandbox_renderer




