#include "framework.h"
#include "SceneFoundry/sandbox_renderer/include/vk_tools/vk_descriptor_registry.h"
//#include <map>
//#include <set>
#include "SceneFoundry/core_interfaces/include/set.h"


namespace sandbox_renderer
{

   DescriptorRegistry::DescriptorRegistry(sandbox_device& device) : m_device(device)
   {

   }

   DescriptorRegistry::~DescriptorRegistry() {

   }

   void DescriptorRegistry::registerSystem(
      const ::scoped_string& name,
      const DescriptorRequirement& req)
   {
      m_entries[name]->requirement = req;
   }
   void DescriptorRegistry::build(uint32_t maxFrames)
   {
      // 1) Tally up (setIndex, descriptorType) counts across all systems
      ::map<::pair<uint32_t, VkDescriptorType>, uint32_t> counts;
      for (auto& [name, entry] : m_entries) {
         for (auto& b : entry->requirement.bindings) {
            counts[{b.setIndex, b.type}] += b.count * maxFrames;
         }
      }

      // === NEW: compute total descriptor-sets PER FRAME ===
      uint32_t setsPerFrame = 0;
      for (auto& [name, entry] : m_entries) {
         ::class_set_base<uint32_t> uniqueSets;
         for (auto& b : entry->requirement.bindings) {
            uniqueSets.add(b.setIndex);
         }
         setsPerFrame += static_cast<uint32_t>(uniqueSets.size());
      }

      // 2) Build our single DescriptorPool via the sandbox Builder
      sandbox_renderer::sandbox_descriptor_pool::Builder poolBuilder{ m_device };
      poolBuilder
         // <-- use setsPerFrame * maxFrames, not m_entries.size()
         .setMaxSets(setsPerFrame * maxFrames)
         .setPoolFlags(VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT);
      for (auto& [key, cnt] : counts) {
         poolBuilder.addPoolSize(key.element2(), cnt);
      }
      m_pool = poolBuilder.build();

      // 3) For each registered system, build its layouts & allocate all its sets
      for (auto& [name, entry] : m_entries) {
         // Group its DescriptorBinding list by setIndex
         ::map<uint32_t, ::array_base<DescriptorBinding>> bySet;
         for (auto& b : entry->requirement.bindings) {
            bySet[b.setIndex].add(b);
         }

         // Build one sandbox_descriptor_set_layout per setIndex
         entry->layouts.clear();
         entry->layouts.reserve(bySet.size());
         for (auto& [setIdx, bindings] : bySet) {
            sandbox_descriptor_set_layout::Builder layoutBuilder{ m_device };
            for (auto& b : bindings) {
               if (b.count == 1) {
                  layoutBuilder.addBinding(b.binding, b.type, b.stages);
               }
               else {
                  layoutBuilder.addBinding(
                     b.binding, b.type, b.stages, b.count, /*flags=*/0u);
               }
            }
            entry->layouts.add(layoutBuilder.build());
         }

         // Allocate every frame’s VkDescriptorSet handles for this system
         entry->sets.assign(maxFrames,
            ::array_base<VkDescriptorSet>(entry->layouts.size()));
         for (uint32_t f = 0; f < maxFrames; f++) {
            for (size_t s = 0; s < entry->layouts.size(); s++) {
               VkDescriptorSet rawSet;
               bool ok = m_pool->allocateDescriptor(
                  entry->layouts[s]->getDescriptorSetLayout(),
                  rawSet,
                  /* variableDescriptorCount = */ 0u
               );
               if (!ok) {
                  throw std::runtime_error(
                     "DescriptorRegistry: failed to allocate descriptor set for system \"" +
                     name + "\""
                  );
               }
               entry->sets[f][s] = rawSet;
            }
         }
      }
   }


   const ::pointer_array<sandbox_descriptor_set_layout>&
      DescriptorRegistry::getDescriptorSetLayouts(const ::scoped_string& sys) const
   {
      return m_entries[sys]->layouts;
   }

   const ::array_base<::array_base<VkDescriptorSet>>&
      DescriptorRegistry::getDescriptorSets(const ::scoped_string& sys) const
   {
      return m_entries[sys]->sets;
   }


} // namespace sandbox_renderer


