// From continuum (V0idsEmbrace@Twitch)
// by camilo on 2025-05-07 02:18 <3ThomasBorregaardSorensen!!
#pragma once


#include "apex/platform/app_consumer.h"
#include "bred/graphics3d/sky_box.h"
#include "bred/graphics3d/scene.h"


namespace SceneFoundry_SceneFoundry
{



   struct global_ubo
   {

      glm::mat4                           projection{ 1.f };
      glm::mat4                           view{ 1.f };
      glm::mat4                           inverseView{ 1.f };
      glm::vec4                           ambientLightColor{ 1.f, 1.f, 1.f, .02f };
      ::graphics3d::point_light_t         pointLights[::graphics3d::point_light_t::MAX_LIGHTS];
      int                                 numLights;

   };


   class main_scene :
      virtual public ::app_consumer < application, ::graphics3d::scene >
   {
   public:


      //::pointer< ::graphics3d::sky_box> m_Skybox;


      ::pointer < simple_render_system >		m_psimplerendersystem;
      ::pointer < point_light_system >			m_ppointlightsystem;

      string_map < ::pointer < ::graphics3d::sky_box > > m_mapSkybox;

      main_scene();
      ~main_scene() override;

      
      void on_initialize_scene() override;

      virtual ::graphics3d::sky_box * get_skybox();

      ::graphics3d::camera *camera() override;


      void on_load_scene(::gpu::context* pgpucontext) override;


      void on_update_global_ubo(::gpu::context* pgpucontext) override;
      void on_render(::gpu::context * pgpucontext) override;


   };


} // namespace SceneFoundry_SceneFoundry







BEGIN_GPU_PROPERTIES(::SceneFoundry_SceneFoundry::global_ubo)
GPU_PROPERTY("projection", ::gpu::e_type_mat4)
GPU_PROPERTY("view", ::gpu::e_type_mat4)
GPU_PROPERTY("invView", ::gpu::e_type_mat4)
GPU_PROPERTY("ambientLightColor", ::gpu::e_type_seq4)
GPU_PROPERTY("pointLights", ::gpu_properties<::graphics3d::point_light>(), ::graphics3d::point_light_t::MAX_LIGHTS)
GPU_PROPERTY("numLights", ::gpu::e_type_int)
GPU_PROPERTY("padding1", ::gpu::e_type_float)
GPU_PROPERTY("padding2", ::gpu::e_type_float)
GPU_PROPERTY("padding3", ::gpu::e_type_float)
END_GPU_PROPERTIES()

