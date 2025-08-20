//
// Created by camilo on 2025-08-19 <3ThomasBorregaardSorensen!!
//
#pragma once


//#include "acme/prototype/prototype/particle.h"
#include "bred/graphics3d/scene_object.h"


namespace sandbox_interfaces
{


    class IFrameContext :
        virtual public ::particle

    {
    public:
        // ::array_base<VkCommandBuffer> graphicsCommandBuffers;
        // VkCommandBuffer primaryGraphicsCommandBuffer = VK_NULL_HANDLE;
        // VkFence frameFence = VK_NULL_HANDLE;
        // uint32_t frameIndex = 0;

        virtual int frame_index() const = 0;
        virtual bool isValid() const = 0;


        virtual int frameIndex() = 0;
        virtual float frameTime() = 0;
        //VkCommandBuffer commandBuffer;
        //::pointer < ::gpu::command_buffer > m_pcommandbuffer;
        virtual ::gpu::command_buffer * getCommandBuffer() = 0;
        virtual ICamera * camera() = 0;
        //VkDescriptorSet globalDescriptorSet;
        virtual ::graphics3d::scene_object::map_base * game_objects() = 0;
        virtual IScene * scene() = 0;
    };



} // namespace sandbox_interfaces



