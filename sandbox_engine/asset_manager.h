#pragma once
//////#include <stdexcept>
//#include <unordered_map>
//#include <memory>
//#include <vector>

#include "SceneFoundry/sandbox_renderer/device.h"
#include "SceneFoundry/sandbox_renderer/object.h"
//#include "SceneFoundry/sandbox_renderer/gltf.h"
#include "SceneFoundry/sandbox_renderer/texture.h"
#include "SceneFoundry/sandbox_renderer/pipeline.h"

#include "SceneFoundry/sandbox/asset_provider.h"


namespace sandbox_engine
{


   class asset_manager :
      public ::sandbox::IAssetProvider
   {
   public:


      asset_manager();
      ~asset_manager();


      virtual void initialize_asset_manager(sandbox_renderer::device* pdevice);


      void preloadGlobalAssets();
      ::pointer<sandbox_renderer::object_model> loadObjModel(const ::scoped_string& name, const ::scoped_string& filepath, bool isSkybox = false);
      ::pointer<sandbox_renderer::gltf::Model> loadGLTFmodel(const ::scoped_string& name, const ::scoped_string& filepath, uint32_t gltfFlags = 0u, float scale = 1.f);
      ::pointer<sandbox_renderer::texture> loadCubemap(
         const ::scoped_string& name,
         const ::scoped_string& ktxFilename,
         VkFormat format,
         VkImageUsageFlags usageFlags,
         VkImageLayout initialLayout
      );

      void generateBRDFlut();
      void generateIrradianceMap();
      void generatePrefilteredEnvMap();

      using OBJmodelHandle = ::pointer<sandbox_renderer::sandbox_object_model>;
      using GLTFmodelHandle = ::pointer<sandbox_renderer::gltf::Model>;
      //using TextureHandle  = ::pointer<VulkanTexture>;
      //using ShaderHandle = ::pointer<ShaderModule>;

      VkDescriptorImageInfo getCubemapDescriptor(const ::scoped_string& name) const override {
         auto it = m_textures.find(name);
         if (it == m_textures.end()) {
            throw std::runtime_error("Cubemap not found: " + name);
         }
         return it->element2()->GetDescriptor();
      }

      // Inline getters
      ::pointer<sandbox_renderer::sandbox_object_model> getOBJModel(const ::scoped_string& name) const {
         auto it = m_objModelCache.find(name);
         return (it != m_objModelCache.end()) ? it->element2() : nullptr;
      }

      ::pointer<sandbox_renderer::gltf::Model> getGLTFmodel(const ::scoped_string& name) const override {
         auto it = m_gltfModelCache.find(name);
         return (it != m_gltfModelCache.end()) ? it->element2() : nullptr;
      }

      ::pointer<sandbox_renderer::texture> getTexture(const ::scoped_string& name) const {
         auto it = m_textures.find(name);
         if (it == m_textures.end()) {
            throw std::runtime_error("Texture not found: " + name);
         }
         return it->element2();
      }

      ::pointer<sandbox_renderer::texture> getTexture(size_t index) const {
         if (index >= (size_t)m_textureList.size()) {
            throw std::runtime_error("Texture index out of range: " + std::to_string(index));
         }
         return m_textureList[index];
      }

      size_t getTextureIndex(const ::scoped_string& name) const {
         auto it = m_textureIndexMap.find(name);
         if (it == m_textureIndexMap.end()) {
            throw std::runtime_error("Texture not found in index map: " + name);
         }
         return it->element2();
      }

      const ::pointer_array_base<sandbox_renderer::texture>& getAllTextures() const {
         return m_textureList;
      }

      bool hasTexture(const ::scoped_string& name) const {
         return m_textures.find(name) != m_textures.end();
      }


      VkDescriptorImageInfo getBRDFLUTDescriptor()    const override { return lutBrdf->GetDescriptor(); }
      VkDescriptorImageInfo getIrradianceDescriptor() const override { return irradianceCube->GetDescriptor(); }
      VkDescriptorImageInfo getPrefilteredDescriptor() const override { return prefilteredCube->GetDescriptor(); }

      VkDescriptorImageInfo getTextureDescriptor(const ::scoped_string& name) const override {
         return getTexture(name)->GetDescriptor();
      }
      GLTFmodelHandle getSkyboxModel() const { return m_skyboxModel; }// make this override if necessary

      ::array_base<::string> listTextureNames()    const override {
         ::array_base<::string> keys;
         keys.reserve(m_textures.size());
         for (const auto& [n, _] : m_textures) keys.add(n);
         return keys;
      }

   private:
      ::string_map < OBJmodelHandle> m_objModelCache;
      ::string_map < GLTFmodelHandle> m_gltfModelCache;

      ::string_map < ::pointer<sandbox_renderer::texture>>  m_textures; // name → texture
      ::string_map < size_t>                      m_textureIndexMap; // name → index
      ::pointer_array_base<sandbox_renderer::texture>                   m_textureList; // index → texture

      ::pointer < sandbox_renderer::device >			m_pgpudevice;
      VkQueue						m_transferQueue;

      // caches
      ::pointer<sandbox_renderer::texture> lutBrdf, irradianceCube, prefilteredCube, environmentCube;

      GLTFmodelHandle m_skyboxModel;

      static void registerTextureIfNeeded(
         const ::scoped_string& name,
         const ::pointer<sandbox_renderer::texture>& tex,
         ::string_map< ::pointer<sandbox_renderer::texture>>& textures,
         ::string_map< size_t>& textureIndexMap,
         ::pointer_array_base<sandbox_renderer::texture>& textureList);



   };


} // namespace sandbox_engine



