#pragma once
//#include <vector>
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

namespace sandbox_engine
{

   struct RenderableMeshInstance {
      floating_matrix4 transform;
      uint32_t materialId;
      uint32_t meshId;
   };

   struct CameraView {
      floating_matrix4 view;
      floating_matrix4 proj;
   };

   struct RenderFrameData {
      CameraView camera;
      ::array_base<RenderableMeshInstance> meshes;
   };




} // namespace sandbox_engine


