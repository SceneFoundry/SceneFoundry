#pragma once
#include <vulkan/vulkan.h>
//////#include <stdexcept>
#include "SceneFoundry/sandbox_renderer/include/vulkan_wrapper/vulkan_texture.h"
#include <optional>
struct IProvider :
	virtual public ::particle
	 {
	virtual VkDescriptorImageInfo getCubemapDescriptor(const ::scoped_string & name) const = 0;
	//virtual const ::pointer_array_base<sandbox_texture>>& getAllTextures() const {}
};