#pragma once
#include <vulkan/vulkan.h>
//////#include <stdexcept>
//#include <memory>
#include "SceneFoundry/sandbox_renderer/include/vulkan_wrapper/vulkan_gltf.h"

struct IAssetProvider :
    virtual public ::particle
{
	virtual VkDescriptorImageInfo getCubemapDescriptor(const ::scoped_string & name) const = 0;

    virtual VkDescriptorImageInfo getBRDFLUTDescriptor()    const = 0;
    virtual VkDescriptorImageInfo getIrradianceDescriptor() const = 0;
    virtual VkDescriptorImageInfo getPrefilteredDescriptor() const = 0;

    virtual VkDescriptorImageInfo getTextureDescriptor(const ::scoped_string & name) const = 0;

    virtual ::array_base<::string> listTextureNames()    const = 0;
    virtual ::pointer<vkglTF::Model> getGLTFmodel(const ::scoped_string & name) const = 0;
};