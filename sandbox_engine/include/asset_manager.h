#pragma once
//////#include <stdexcept>
#include <unordered_map>
//#include <memory>
//#include <vector>

#include "SceneFoundry/sandbox_renderer/include/vulkan_wrapper/vulkan_device.h"
#include "SceneFoundry/sandbox_renderer/include/vulkan_wrapper/vulkan_obj.h"
#include "SceneFoundry/sandbox_renderer/include/vulkan_wrapper/vulkan_gltf.h"
#include "SceneFoundry/sandbox_renderer/include/vulkan_wrapper/vulkan_texture.h"
#include "SceneFoundry/sandbox_renderer/include/vulkan_wrapper/vulkan_pipeline.h"

#include "SceneFoundry/core_interfaces/interfaces/asset_provider_i.h"



class AssetManager : public IAssetProvider {
public:
	AssetManager(VkSandboxDevice& device);
	~AssetManager();
	void preloadGlobalAssets();
	::pointer<VkSandboxOBJmodel> loadObjModel(const ::scoped_string & name, const ::scoped_string & filepath, bool isSkybox = false);
	::pointer<vkglTF::Model> loadGLTFmodel(const ::scoped_string & name, const ::scoped_string & filepath, uint32_t gltfFlags = 0u, float scale = 1.f);
	::pointer<VkSandboxTexture> loadCubemap(
		const ::scoped_string & name,
		const ::scoped_string & ktxFilename,
		VkFormat format,
		VkImageUsageFlags usageFlags,
		VkImageLayout initialLayout
	);

    void generateBRDFlut();
    void generateIrradianceMap();
    void generatePrefilteredEnvMap();
	
	using OBJmodelHandle = ::pointer<VkSandboxOBJmodel>;
	using GLTFmodelHandle = ::pointer<vkglTF::Model>;
	//using TextureHandle  = ::pointer<VulkanTexture>;
	//using ShaderHandle = ::pointer<ShaderModule>;

	VkDescriptorImageInfo getCubemapDescriptor(const ::scoped_string & name) const override {
		auto it = m_textures.find(name);
		if (it == m_textures.end()) {
			throw std::runtime_error("Cubemap not found: " + name);
		}
		return it->second->GetDescriptor();
	}

    // Inline getters
    ::pointer<VkSandboxOBJmodel> getOBJModel(const ::scoped_string & name) const {
        auto it = m_objModelCache.find(name);
        return (it != m_objModelCache.end()) ? it->second : nullptr;
    }

    ::pointer<vkglTF::Model> getGLTFmodel(const ::scoped_string & name) const override {
        auto it = m_gltfModelCache.find(name);
        return (it != m_gltfModelCache.end()) ? it->second : nullptr;
    }

    ::pointer<VkSandboxTexture> getTexture(const ::scoped_string & name) const {
        auto it = m_textures.find(name);
        if (it == m_textures.end()) {
            throw std::runtime_error("Texture not found: " + name);
        }
        return it->second;
    }

    ::pointer<VkSandboxTexture> getTexture(size_t index) const {
        if (index >= m_textureList.size()) {
            throw std::runtime_error("Texture index out of range: " + std::to_string(index));
        }
        return m_textureList[index];
    }

    size_t getTextureIndex(const ::scoped_string & name) const {
        auto it = m_textureIndexMap.find(name);
        if (it == m_textureIndexMap.end()) {
            throw std::runtime_error("Texture not found in index map: " + name);
        }
        return it->second;
    }

    const ::array_base<::pointer<VkSandboxTexture>>& getAllTextures() const {
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
        for (const auto& [n, _] : m_textures) keys.push_back(n);
        return keys;
    }

private:
	::map<::string, OBJmodelHandle> m_objModelCache;
	::map<::string, GLTFmodelHandle> m_gltfModelCache;

	::map<::string, ::pointer<VkSandboxTexture>>  m_textures; // name → texture
	::map<::string, size_t>                      m_textureIndexMap; // name → index
	::array_base<::pointer<VkSandboxTexture>>                   m_textureList; // index → texture

	VkSandboxDevice&			m_device;
	VkQueue						m_transferQueue;

	// caches
	::pointer<VkSandboxTexture> lutBrdf, irradianceCube, prefilteredCube, environmentCube;

    GLTFmodelHandle m_skyboxModel;

	static void registerTextureIfNeeded(
		const ::scoped_string & name,
		const ::pointer<VkSandboxTexture>& tex,
		::map<::string, ::pointer<VkSandboxTexture>>& textures,
		::map<::string, size_t>& textureIndexMap,
		::array_base<::pointer<VkSandboxTexture>>& textureList);



};