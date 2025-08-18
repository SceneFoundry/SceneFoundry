#pragma once
//////#include <stdexcept>
//#include <unordered_map>
//#include <memory>
//#include <vector>

#include "SceneFoundry/sandbox_renderer/include/vulkan_wrapper/vulkan_device.h"
#include "SceneFoundry/sandbox_renderer/include/vulkan_wrapper/vulkan_obj.h"
#include "SceneFoundry/sandbox_renderer/include/vulkan_wrapper/vulkan_gltf.h"
#include "SceneFoundry/sandbox_renderer/include/vulkan_wrapper/vulkan_texture.h"
#include "SceneFoundry/sandbox_renderer/include/vulkan_wrapper/vulkan_pipeline.h"

#include "SceneFoundry/sandbox_interfaces/include/interfaces/asset_provider_i.h"



class AssetManager : public IAssetProvider {
public:
	AssetManager(sandbox_renderer::sandbox_device& device);
	~AssetManager();
	void preloadGlobalAssets();
	::pointer<sandbox_renderer::sandbox_object_model> loadObjModel(const ::scoped_string & name, const ::scoped_string & filepath, bool isSkybox = false);
	::pointer<sandbox_renderer::gltf::Model> loadGLTFmodel(const ::scoped_string & name, const ::scoped_string & filepath, uint32_t gltfFlags = 0u, float scale = 1.f);
	::pointer<sandbox_renderer::sandbox_texture> loadCubemap(
		const ::scoped_string & name,
		const ::scoped_string & ktxFilename,
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

	VkDescriptorImageInfo getCubemapDescriptor(const ::scoped_string & name) const override {
		auto it = m_textures.find(name);
		if (it == m_textures.end()) {
			throw std::runtime_error("Cubemap not found: " + name);
		}
		return it->element2()->GetDescriptor();
	}

    // Inline getters
    ::pointer<sandbox_renderer::sandbox_object_model> getOBJModel(const ::scoped_string & name) const {
        auto it = m_objModelCache.find(name);
        return (it != m_objModelCache.end()) ? it->element2() : nullptr;
    }

    ::pointer<sandbox_renderer::gltf::Model> getGLTFmodel(const ::scoped_string & name) const override {
        auto it = m_gltfModelCache.find(name);
        return (it != m_gltfModelCache.end()) ? it->element2() : nullptr;
    }

    ::pointer<sandbox_renderer::sandbox_texture> getTexture(const ::scoped_string & name) const {
        auto it = m_textures.find(name);
        if (it == m_textures.end()) {
            throw std::runtime_error("Texture not found: " + name);
        }
        return it->element2();
    }

    ::pointer<sandbox_renderer::sandbox_texture> getTexture(size_t index) const {
        if (index >= (size_t) m_textureList.size()) {
            throw std::runtime_error("Texture index out of range: " + std::to_string(index));
        }
        return m_textureList[index];
    }

    size_t getTextureIndex(const ::scoped_string & name) const {
        auto it = m_textureIndexMap.find(name);
        if (it == m_textureIndexMap.end()) {
            throw std::runtime_error("Texture not found in index map: " + name);
        }
        return it->element2();
    }

    const ::array_base<::pointer<sandbox_renderer::sandbox_texture>>& getAllTextures() const {
        return m_textureList;
    }

    bool hasTexture(const ::scoped_string & name) const {
        return m_textures.find(name) != m_textures.end();
    }


    VkDescriptorImageInfo getBRDFLUTDescriptor()    const override { return lutBrdf->GetDescriptor(); }
    VkDescriptorImageInfo getIrradianceDescriptor() const override { return irradianceCube->GetDescriptor(); }
    VkDescriptorImageInfo getPrefilteredDescriptor() const override { return prefilteredCube->GetDescriptor(); }

    VkDescriptorImageInfo getTextureDescriptor(const ::scoped_string & name) const override {
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
	::map<::string, OBJmodelHandle> m_objModelCache;
	::map<::string, GLTFmodelHandle> m_gltfModelCache;

	::map<::string, ::pointer<sandbox_renderer::sandbox_texture>>  m_textures; // name → texture
	::map<::string, size_t>                      m_textureIndexMap; // name → index
	::array_base<::pointer<sandbox_renderer::sandbox_texture>>                   m_textureList; // index → texture

   sandbox_renderer::sandbox_device&			m_device;
	VkQueue						m_transferQueue;

	// caches
	::pointer<sandbox_renderer::sandbox_texture> lutBrdf, irradianceCube, prefilteredCube, environmentCube;

    GLTFmodelHandle m_skyboxModel;

	static void registerTextureIfNeeded(
		const ::scoped_string & name,
		const ::pointer<sandbox_renderer::sandbox_texture>& tex,
		::map<::string, ::pointer<sandbox_renderer::sandbox_texture>>& textures,
		::map<::string, size_t>& textureIndexMap,
		::array_base<::pointer<sandbox_renderer::sandbox_texture>>& textureList);



};