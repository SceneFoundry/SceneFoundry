#pragma once
//#include <vulkan/vulkan.h>
//////#include <stdexcept>
//#include "SceneFoundry/sandbox_renderer/texture.h"
//#include <optional>

namespace sandbox_interfaces
{

	struct IProvider :
		virtual public ::particle
	{

		virtual VkDescriptorImageInfo getCubemapDescriptor(const ::scoped_string & name) const = 0;
		//virtual const ::pointer_array_base<texture>>& getAllTextures() const {}
	};


} // namespace sandbox_interfaces

