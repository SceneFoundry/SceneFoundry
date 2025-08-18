#pragma once
//#include <vector>
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

namespace sandbox_engine
{

   struct RenderableMeshInstance {
      glm::mat4 transform;
      uint32_t materialId;
      uint32_t meshId;
   };

   struct CameraView {
      glm::mat4 view;
      glm::mat4 proj;
   };

   struct RenderFrameData {
      CameraView camera;
      ::array_base<RenderableMeshInstance> meshes;
   };




} // namespace sandbox_engine


