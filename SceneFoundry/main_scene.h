// From continuum (V0idsEmbrace@Twitch)
// by camilo on 2025-05-07 02:18 <3ThomasBorregaardSorensen!!
#pragma once


#include "apex/platform/app_consumer.h"
#include "bred/graphics3d/skybox.h"
#include "bred/graphics3d/scene.h"


namespace SceneFoundry_SceneFoundry
{




   class CLASS_DECL_SCENEFOUNDRY_SCENEFOUNDRY main_scene :
      virtual public ::app_consumer < application, ::graphics3d::scene >
   {
   public:


      //::pointer< ::graphics3d::sky_box> m_Skybox;

      ::pointer < ::graphics3d::gltf_render_system >              m_pgltfrendersystem;
      ::pointer < ::graphics3d::wavefront_obj_render_system >		m_pwavefrontobjrendersystem;
      ::pointer < ::graphics3d::point_light_render_system >			m_ppointlightrendersystem;
      ::pointer < ::graphics3d::scene_render_system >             m_pscenerendersystem;
      ::pointer < ::graphics3d::skybox_ibl_render_system >			m_pskyboxiblrendersystem;


      main_scene();
      ~main_scene() override;

      
      void on_initialize_scene() override;

      virtual ::graphics3d::skybox * get_skybox();

      //::graphics3d::camera *camera() override;


      void on_load_scene(::gpu::context* pgpucontext) override;


      void on_update(::gpu::context* pgpucontext) override;
      void on_render(::gpu::context * pgpucontext) override;


   };


} // namespace SceneFoundry_SceneFoundry







//BEGIN_GPU_PROPERTIES(::SceneFoundry_SceneFoundry::global_ubo)
//GPU_PROPERTY("projection", ::gpu::e_type_mat4)
//GPU_PROPERTY("view", ::gpu::e_type_mat4)
//GPU_PROPERTY("invView", ::gpu::e_type_mat4)
//GPU_PROPERTY("ambientLightColor", ::gpu::e_type_seq4)
//GPU_PROPERTY("pointLights", ::gpu_properties<::gpu::point_light>(), ::SceneFoundry_SceneFoundry::global_ubo::MAX_LIGHTS)
//GPU_PROPERTY("numLights", ::gpu::e_type_int)
//GPU_PROPERTY("padding1", ::gpu::e_type_float)
//GPU_PROPERTY("padding2", ::gpu::e_type_float)
//GPU_PROPERTY("padding3", ::gpu::e_type_float)
//END_GPU_PROPERTIES()
//
