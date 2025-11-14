#pragma once


#include "bred/graphics3d/point_light.h"
#include "bred/user/user/graphics3d.h"
#include "pbr-renderer/_.h"


namespace SceneFoundry_SceneFoundry
{


//#define MAX_LIGHTS 10


   //struct PointLight
   //   floating_sequence4 position{};  // ignore w
   // floating_sequence4 color{};     // w is intensity

//BEGIN_GPU_PROPERTIES(point_light)
//GPU_PROPERTY("position", ::gpu::e_type_seq4)
//GPU_PROPERTY("color", ::gpu::e_type_seq4)
//END_GPU_PROPERTIES()

//GPU_PROPERTY("radius", ::gpu::e_type_float)

   // GlobalUbo :
   // floating_matrix4 projection{ 1.f };
   // floating_matrix4 view{ 1.f };
   // floating_matrix4 invView{ 1.f };
   // floating_sequence4 ambientLightColor{ 1.f, 1.f, 1.f, .02f };
   // PointLight pointLights[MAX_LIGHTS];
   // int numLights;
   // 3XINT PADDING



//#define MAX_LIGHTS 10

   //struct point_light {
   //   floating_sequence4 position{};  // ignore w
   //   floating_sequence4 color{};     // w is intensity
   //};


   struct global_ubo {
      inline static const int MAX_LIGHTS{10};
      floating_matrix4 projection{ 1.f };
      floating_matrix4 view{ 1.f };
      floating_sequence4 ambientLightColor{ 1.f, 1.f, 1.f, .02f };
      floating_sequence4 viewPos;
      gpu::point_light pointLights[MAX_LIGHTS];
      int numLights;
   };



} // namespace SceneFoundry_SceneFoundry


DECLARE_GPU_PROPERTIES(CLASS_DECL_SCENEFOUNDRY_PBR_RENDERER, ::SceneFoundry_SceneFoundry::global_ubo)



