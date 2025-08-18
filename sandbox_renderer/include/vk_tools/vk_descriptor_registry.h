// vk_descriptor_reg.h
#pragma once
#include "vk_descriptor_requirement.h"
#include <vulkan/vulkan.h>
#include <unordered_map>
//////#include <stdexcept>
#include "SceneFoundry/sandbox_renderer/include/vulkan_wrapper/vulkan_device.h"
#include "SceneFoundry/sandbox_renderer/include/vulkan_wrapper/vulkan_descriptor.h"
//#include <memory>
//#include <vector>

namespace vulkan
{

   class DescriptorRegistry 
   {
   public:
      DescriptorRegistry(sandbox_device& device);
      ~DescriptorRegistry();

      // 1) Call once per system, before init()
      void registerSystem(
         const ::scoped_string& name,
         const DescriptorRequirement& req);

      // 2) After all systems registered, build everything:
      void build(uint32_t maxFramesInFlight);

      // 3) Pull back the sandbox‐layout wrappers so you can build your pipeline
      //    layouts in each RenderSystem::init()
      const ::pointer_array<sandbox_descriptor_set_layout>&
         getDescriptorSetLayouts(const ::scoped_string& systemName) const;

      // 4) Pull back the raw VkDescriptorSet handles for binding at draw time
      //    [frame][setIndex]
      const ::array_base<::array_base<VkDescriptorSet>>&
         getDescriptorSets(const ::scoped_string& systemName) const;

   private:
      sandbox_device& m_device;
      ::pointer<vulkan::sandbox_descriptor_pool>  m_pool;

      // pooled by (setIndex, type) to sum counts:
      struct PoolSizeKey { uint32_t setIndex; VkDescriptorType type; };
      ::array_base<VkDescriptorPoolSize>          m_poolSizes;

      // for each system:
      struct Entry :
      virtual public ::particle
      {
         DescriptorRequirement                  requirement;
         ::pointer_array<sandbox_descriptor_set_layout> layouts;
         ::array_base<::array_base<VkDescriptorSet>>  sets;     // [frame][setIndex]
      };

      ::string_map<::pointer < Entry >>     m_entries;
   };



} // namespace vulkan



