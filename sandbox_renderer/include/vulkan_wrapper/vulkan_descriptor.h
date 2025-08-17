#pragma once

#include "vulkan_device.h"

// std
//#include <memory>
#include <unordered_map>
//#include <vector>

namespace vulkan
{

   class sandbox_descriptor_set_layout :
      virtual public ::particle
   {
   public:
      class Builder
      {
      public:
         Builder(sandbox_device& device) : m_device{ device } {}

         Builder& addBinding(
            uint32_t binding,
            VkDescriptorType descriptorType,
            VkShaderStageFlags stageFlags,
            uint32_t count,
            VkDescriptorBindingFlags flags);

         Builder& addBinding(
            uint32_t binding,
            VkDescriptorType descriptorType,
            VkShaderStageFlags stageFlags);

         ::pointer < sandbox_descriptor_set_layout > build() const;

      private:
         sandbox_device& m_device;
         ::map<uint32_t, VkDescriptorSetLayoutBinding> m_bindings{};
         ::map<uint32_t, VkDescriptorBindingFlags> m_bindingFlags{};
      };

      sandbox_descriptor_set_layout(
         sandbox_device& device,
         const ::array_base<VkDescriptorSetLayoutBinding>& bindingsVec,
         VkDescriptorSetLayout layout
      );
      sandbox_descriptor_set_layout(sandbox_device& device, VkDescriptorSetLayout layout)
         : m_device{ device }, m_descriptorSetLayout{ layout } {
      }
      ~sandbox_descriptor_set_layout();
      sandbox_descriptor_set_layout(const sandbox_descriptor_set_layout&) = delete;
      sandbox_descriptor_set_layout& operator=(const sandbox_descriptor_set_layout&) = delete;

      VkDescriptorSetLayout getDescriptorSetLayout() const { return m_descriptorSetLayout; }

   private:
      sandbox_device& m_device;
      VkDescriptorSetLayout m_descriptorSetLayout;
      friend class VkcDescriptorWriter;
   public:
      ::map<uint32_t, VkDescriptorSetLayoutBinding> m_bindings;



   };

   class VkSandboxDescriptorPool
   {
   public:
      class Builder
      {
      public:
         Builder(sandbox_device& device) : m_device{ device } {}

         Builder& addPoolSize(VkDescriptorType descriptorType, uint32_t count);
         Builder& setPoolFlags(VkDescriptorPoolCreateFlags flags);
         Builder& setMaxSets(uint32_t count);
         std::unique_ptr<VkSandboxDescriptorPool> build() const;

      private:
         sandbox_device& m_device;
         ::array_base<VkDescriptorPoolSize> m_poolSizes{};
         uint32_t m_maxSets = 1000;
         VkDescriptorPoolCreateFlags m_poolFlags = 0;
      };

      VkSandboxDescriptorPool(
         sandbox_device& vkcDevice,
         uint32_t maxSets,
         VkDescriptorPoolCreateFlags poolFlags,
         const ::array_base<VkDescriptorPoolSize>& poolSizes);
      ~VkSandboxDescriptorPool();
      VkSandboxDescriptorPool(const VkSandboxDescriptorPool&) = delete;
      VkSandboxDescriptorPool& operator=(const VkSandboxDescriptorPool&) = delete;

      bool allocateDescriptor(
         const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor, uint32_t variableDescriptorCount) const;

      void freeDescriptors(::array_base<VkDescriptorSet>& descriptors) const;

      void resetPool();

      VkDescriptorPool getHandle() const { return m_descriptorPool; }


      sandbox_device& m_device;
      VkDescriptorPool m_descriptorPool;
   private:



      friend class VkcDescriptorWriter;
   };

   class VkSandboxDescriptorWriter
   {
   public:
      VkSandboxDescriptorWriter(sandbox_descriptor_set_layout& setLayout, VkSandboxDescriptorPool& pool);

      VkSandboxDescriptorWriter& writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
      VkSandboxDescriptorWriter& writeImage(uint32_t binding, const VkDescriptorImageInfo* imageInfo);
      VkSandboxDescriptorWriter& writeImage(uint32_t binding, const VkDescriptorImageInfo* imageInfos, uint32_t count);

      bool build(VkDescriptorSet& set);
      void overwrite(VkDescriptorSet& set);

   private:
      sandbox_descriptor_set_layout& m_setLayout;
      VkSandboxDescriptorPool& m_pool;
      ::array_base<VkWriteDescriptorSet> m_writes;
      uint32_t m_variableDescriptorCount = 0;
   };


} // namespace vulkan

