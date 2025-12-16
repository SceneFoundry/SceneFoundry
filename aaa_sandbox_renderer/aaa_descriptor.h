#pragma once
//
// #include "SceneFoundry/sandbox_renderer/device.h"
//
// // std
// //#include <memory>
// //#include <unordered_map>
// //#include <vector>
//
// namespace sandbox_renderer
// {
//
//    class sandbox_descriptor_set_layout :
//       virtual public ::particle
//    {
//    public:
//       class Builder
//       {
//       public:
//          Builder(device * pdevice) : m_pgpudevice{ pdevice } {}
//
//          Builder& addBinding(
//             uint32_t binding,
//             VkDescriptorType descriptorType,
//             VkShaderStageFlags stageFlags,
//             uint32_t count,
//             VkDescriptorBindingFlags flags);
//
//          Builder& addBinding(
//             uint32_t binding,
//             VkDescriptorType descriptorType,
//             VkShaderStageFlags stageFlags);
//
//          ::pointer < sandbox_descriptor_set_layout > build() const;
//
//       private:
//          device * m_pgpudevice;
//          ::map<uint32_t, aaaVkDescriptorSetLayoutBinding> m_bindings{};
//          ::map<uint32_t, VkDescriptorBindingFlags> m_bindingFlags{};
//       };
//
//       sandbox_descriptor_set_layout(
//          device * pdevice,
//          const ::array_base<aaaVkDescriptorSetLayoutBinding>& bindingsVec,
//          aaaVkDescriptorSetLayout layout
//       );
//       sandbox_descriptor_set_layout(device * pdevice, aaaVkDescriptorSetLayout layout)
//          : m_pgpudevice{ pdevice }, m_descriptorSetLayout{ layout } {
//       }
//       ~sandbox_descriptor_set_layout();
//       sandbox_descriptor_set_layout(const sandbox_descriptor_set_layout&) = delete;
//       sandbox_descriptor_set_layout& operator=(const sandbox_descriptor_set_layout&) = delete;
//
//       aaaVkDescriptorSetLayout getDescriptorSetLayout() const { return m_descriptorSetLayout; }
//
//    private:
//       device * m_pgpudevice;
//       aaaVkDescriptorSetLayout m_descriptorSetLayout;
//       friend class VkcDescriptorWriter;
//    public:
//       ::map<uint32_t, aaaVkDescriptorSetLayoutBinding> m_bindings;
//
//
//
//    };
//
//    class sandbox_descriptor_pool :
//       virtual public ::particle
//    {
//    public:
//       class Builder
//       {
//       public:
//          Builder(device * pdevice) : m_pgpudevice{ pdevice } {}
//
//          Builder& addPoolSize(VkDescriptorType descriptorType, uint32_t count);
//          Builder& setPoolFlags(VkDescriptorPoolCreateFlags flags);
//          Builder& setMaxSets(uint32_t count);
//          ::pointer<sandbox_renderer::sandbox_descriptor_pool> build() const;
//
//       private:
//          device * m_pgpudevice;
//          ::array_base<VkDescriptorPoolSize> m_poolSizes{};
//          uint32_t m_maxSets = 1000;
//          VkDescriptorPoolCreateFlags m_poolFlags = 0;
//       };
//
//       sandbox_descriptor_pool(
//          device * vkcDevice,
//          uint32_t maxSets,
//          VkDescriptorPoolCreateFlags poolFlags,
//          const ::array_base<VkDescriptorPoolSize>& poolSizes);
//       ~sandbox_descriptor_pool();
//       sandbox_descriptor_pool(const sandbox_descriptor_pool&) = delete;
//       sandbox_descriptor_pool& operator=(const sandbox_descriptor_pool&) = delete;
//
//       bool allocateDescriptor(
//          const aaaVkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor, uint32_t variableDescriptorCount) const;
//
//       void freeDescriptors(::array_base<VkDescriptorSet>& descriptors) const;
//
//       void resetPool();
//
//       VkDescriptorPool getHandle() const { return m_descriptorPool; }
//
//
//       device * m_pgpudevice;
//       VkDescriptorPool m_descriptorPool;
//    private:
//
//
//
//       friend class VkcDescriptorWriter;
//    };
//
//    class sandbox_descriptor_writer
//    {
//    public:
//       sandbox_descriptor_writer(sandbox_descriptor_set_layout& setLayout, sandbox_renderer::sandbox_descriptor_pool& pool);
//
//       sandbox_descriptor_writer& writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
//       sandbox_descriptor_writer& writeImage(uint32_t binding, const VkDescriptorImageInfo* imageInfo);
//       sandbox_descriptor_writer& writeImage(uint32_t binding, const VkDescriptorImageInfo* imageInfos, uint32_t count);
//
//       bool build(VkDescriptorSet& set);
//       void overwrite(VkDescriptorSet& set);
//
//    private:
//       sandbox_descriptor_set_layout& m_setLayout;
//       sandbox_renderer::sandbox_descriptor_pool& m_pool;
//       ::array_base<VkWriteDescriptorSet> m_writes;
//       uint32_t m_variableDescriptorCount = 0;
//    };
//
//
// } // namespace sandbox_renderer
//
