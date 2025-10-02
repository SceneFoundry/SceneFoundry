#pragma once


#include "bred/graphics3d/point_light.h"
#include "bred/user/user/graphics3d.h"


namespace SceneFoundry_opengl_pbr_renderer
{


//#define MAX_LIGHTS 10


   //struct PointLight
   //   glm::vec4 position{};  // ignore w
   // glm::vec4 color{};     // w is intensity

//BEGIN_GPU_PROPERTIES(point_light)
//GPU_PROPERTY("position", ::gpu::e_type_seq4)
//GPU_PROPERTY("color", ::gpu::e_type_seq4)
//END_GPU_PROPERTIES()

//GPU_PROPERTY("radius", ::gpu::e_type_float)

   // GlobalUbo :
   // glm::mat4 projection{ 1.f };
   // glm::mat4 view{ 1.f };
   // glm::mat4 invView{ 1.f };
   // glm::vec4 ambientLightColor{ 1.f, 1.f, 1.f, .02f };
   // PointLight pointLights[MAX_LIGHTS];
   // int numLights;
   // 3XINT PADDING



//#define MAX_LIGHTS 10

   //struct point_light {
   //   glm::vec4 position{};  // ignore w
   //   glm::vec4 color{};     // w is intensity
   //};


   struct global_ubo {
      inline static const int MAX_LIGHTS{10};
      glm::mat4 projection{ 1.f };
      glm::mat4 view{ 1.f };
      glm::vec4 ambientLightColor{ 1.f, 1.f, 1.f, .02f };
      glm::vec4 viewPos;
      gpu::point_light pointLights[MAX_LIGHTS];
      int numLights;
   };



} // namespace SceneFoundry_opengl_pbr_renderer


DECLARE_GPU_PROPERTIES(CLASS_DECL_SCENEFOUNDRY_OPENGL_PBR_RENDERER, ::SceneFoundry_opengl_pbr_renderer::global_ubo)



