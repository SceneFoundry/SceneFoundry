// From continuum (V0idsEmbrace@Twitch)
// by camilo on 2025-05-07 02:18 <3ThomasBorregaardSorensen!!
#include "framework.h"
#include "main_scene.h"
#include "application.h"
#include "camera.h"
#include "immersion.h"
#include "impact.h"
#include "input.h"
#include "bred/gpu/block.h"
#include "bred/gpu/context.h"
#include "bred/graphics3d/global_ubo1.h"
#include "app-graphics3d/graphics3d/render_system/gltf_render_system.h"
#include "bred/graphics3d/render_system/wavefront_obj_render_system.h"
#include "bred/graphics3d/render_system/point_light_render_system.h"
#include "app-graphics3d/graphics3d/render_system/scene_render_system.h"
#include "bred/graphics3d/render_system/skybox_render_system.h"
#include "bred/graphics3d/camera.h"
#include "bred/graphics3d/engine.h"
#include "bred/graphics3d/point_light.h"
#include "bred/graphics3d/scene_object.h"
#include "bred/prodevian/actor.h"
#include "bred/prodevian/game_object.h"




namespace SceneFoundry_pbr_renderer
{


   main_scene::main_scene()
   {

   }


   main_scene::~main_scene()
   {

   }


   void main_scene::on_initialize_scene()
   {

      scene_base::on_initialize_particle();

      auto pprodevianactor = øcreate_new<::prodevian::actor>();

      pprodevianactor->initialize_prodevian_actor(this);

      ::cast<immersion> pimmersion = m_pimmersionlayer;

      pprodevianactor->m_sequence3Translation = pimmersion->m_initialCameraPosition;
      pprodevianactor->m_matrixRotation = pimmersion->m_rotationInitialCamera.as_rotation_matrix();

      m_prodevianactora.add(pprodevianactor);


      m_papp->m_pmainscene = this;

      if (!m_pcameraDefault)
      {

      }

   }


   void main_scene::on_load_scene(::gpu::context* pgpucontext)
   {

      //m_gpupropertiesGlobalUbo.set<::graphics3d::global_ubo1>();

      loadSceneFile("default_scene");

      øconstruct(m_pscenerendersystem);

      m_pscenerendersystem->initialize_render_system(m_pimmersionlayer->m_pengine);

      m_pscenerendersystem->prepare(pgpucontext);

      øconstruct(m_pgltfrendersystem);

      m_pgltfrendersystem->m_bDisableAlbedo = false;
      m_pgltfrendersystem->m_bDisableMetallicRoughness = false;
      m_pgltfrendersystem->m_bDisableNormal = false;
      m_pgltfrendersystem->m_bDisableAmbientOcclusion = false;
      m_pgltfrendersystem->m_bDisableEmissive = false;

      m_pgltfrendersystem->m_bForceDefaultAlbedo = false;
      m_pgltfrendersystem->m_bForceDefaultMetallicFactor = false;
      m_pgltfrendersystem->m_bForceDefaultRoughnessFactor = false;
      m_pgltfrendersystem->m_bForceDefaultAmbientOcclusionFactor = false;
      m_pgltfrendersystem->m_bForceDefaultEmission = false;
      m_pgltfrendersystem->m_seq3DefaultAlbedo = {1.0f, 1.0f, 1.0f};
      m_pgltfrendersystem->m_fDefaultMetallicFactor = 0.0f;
      m_pgltfrendersystem->m_fDefaultRoughnessFactor = 0.0f;
      m_pgltfrendersystem->m_fDefaultAmbientOcclusionFactor = 1.0f;
      m_pgltfrendersystem->m_seq3DefaultEmission = {};

      m_pgltfrendersystem->initialize_render_system(m_pimmersionlayer->m_pengine);

      m_pgltfrendersystem->prepare(pgpucontext);


      øconstruct(m_pwavefrontobjrendersystem);

      m_pwavefrontobjrendersystem->initialize_render_system(m_pimmersionlayer->m_pengine);

      m_pwavefrontobjrendersystem->prepare(pgpucontext);
      //m_prenderer->getRenderPass(),
      //globalSetLayout->getDescriptorSetLayout() };

      øconstruct_new(m_ppointlightrendersystem);

      m_ppointlightrendersystem->initialize_render_system(m_pimmersionlayer->m_pengine);

      m_ppointlightrendersystem->prepare(pgpucontext);

      øconstruct(m_pskyboxrendersystem);

      m_pskyboxrendersystem->initialize_render_system(m_pimmersionlayer->m_pengine);

      m_pskyboxrendersystem->prepare(pgpucontext);

      //    // Try to get skybox object from scene
      //if (auto skyboxOpt = getSkyboxObject())
      //{
      //   IGameObject &skyboxObj = skyboxOpt.value().get();

      //   // Get and set model
      //   if (auto skyboxModelBase = provider.getGLTFmodel("cube"))
      //   {
      //      skyboxSystem->setSkyboxModel(skyboxModelBase);
      //   }
      //   else
      //   {
      //      warning("Skybox object has no model");
      //   }

      //   // Get and set cubemap
      //   auto cubemapName = skyboxObj.getCubemapTextureName();
      //   try
      //   {
      //      VkDescriptorImageInfo cubemapDesc = provider.getCubemapDescriptor(cubemapName);

      //      spdlog::info("[Renderer] got cubemapDesc: view=0x{:x}, sampler=0x{:x}, layout={}",
      //                   (uintptr_t)cubemapDesc.imageView, (uintptr_t)cubemapDesc.sampler,
      //                   (int)cubemapDesc.imageLayout);

      //      if (cubemapDesc.imageView == VK_NULL_HANDLE || cubemapDesc.sampler == VK_NULL_HANDLE)
      //      {
      //         spdlog::error(
      //            "[Renderer] cubemap descriptor has null view or sampler! This will produce garbage in shader.");
      //      }

      //      skyboxSystem->setCubemapTexture(cubemapDesc);
      //   }
      //   catch (const std::exception &e)
      //   {
      //      warning("Skybox cubemap '{}' not found: {}", cubemapName, e.what());
      //   }
      //}
      //else
      //{
      //   warning("No skybox object found in scene");
      //}



   }


   void main_scene::on_update(::gpu::context* pgpucontext)
   {

      auto pblockGlobalUbo1 = this->global_ubo1(pgpucontext);

      auto &globalUbo1=*pblockGlobalUbo1;

      auto pimmersion = m_pimmersionlayer;

      auto pscene = pimmersion->m_pscene;

      auto pgpucamera = pscene->camera();

      ::cast<::SceneFoundry_pbr_renderer::camera> pcamera = pgpucamera;

      auto dt = m_pimmersionlayer->m_pengine->dt();
                   
      ::cast<input> pinput = m_pimmersionlayer->m_pengine->m_pinput;

      auto &transform = m_pimmersionlayer->m_pengine->m_transform;

      pinput->_017Update(dt, transform);

      auto positionTransform = transform.m_sequence3Position;

      pcamera->m_sequence3Position = positionTransform;
      
      pcamera->m_rotation = transform.m_rotation;

      auto aspect = m_pimmersionlayer->m_pengine->m_pusergraphics3d->getAspectRatio();

      pcamera->m_fAspectRatio = aspect;

      pcamera->m_fNearZ = 0.1f;

      pcamera->m_fFarZ = 100.0f;

      pcamera->m_angleFovY = 60.0_f_degrees;

      pcamera->update_vectors();

      pcamera->update();

      auto projection = pcamera->projection();
      globalUbo1["projection"] = projection;


      auto impact = pcamera->impact();
      globalUbo1["view"] = impact;

      ::floating_sequence4 seq4AmbientLightColor(0.2f, 0.2f, 0.2f, 0.2f);
      globalUbo1["ambientLightColor"] = seq4AmbientLightColor;

      // auto inversedImpact = pcamera->inversed_impact();
      auto inversedImpact = impact.inversed();
      globalUbo1["invView"] = inversedImpact;



      auto cameraPosition = pcamera->position();
      globalUbo1["cameraPosition"] = cameraPosition;

      //auto inverseView = pcamera->getInverseView();
      //globalubo["invView"] = inverseView;

      debug() << "main_scene::on_update";

      if (m_ppointlightrendersystem)
      {

         m_ppointlightrendersystem->update(pgpucontext, this);

      }


   }


   void main_scene::on_render(::gpu::context * pgpucontext)
   {

      //pgpucontext->clear(pgpucontext->current_target_texture(::gpu::current_frame()), argb(1.0f, 0.5f, 0.75f, 1.0f)); // Clear with a light blue color
      //pgpucontext->clear(pgpucontext->current_target_texture(::gpu::current_frame()), ::color::transparent); // Clear with a transparent
      pgpucontext->clear(pgpucontext->current_target_texture(::gpu::current_frame()),argb(0.95f, 0.85f, 0.75f, 0.25f)); // Clear with a transparent

      auto pskyboxrendersystem = m_pskyboxrendersystem;

      if (pskyboxrendersystem)
      {

         pskyboxrendersystem->render(pgpucontext, this);

      }

    /*  auto pscenerendersystem = m_pscenerendersystem;

      if (pscenerendersystem)
      {

         pscenerendersystem->render(pgpucontext, this);

      }*/

      auto pgltfrendersystem = m_pgltfrendersystem;

      if (pgltfrendersystem)
      {

         pgltfrendersystem->render(pgpucontext, this);

      }

      //return;

    /*  auto pwavefrontobjrendersystem = m_pwavefrontobjrendersystem;

      if (pwavefrontobjrendersystem)
      {

         pwavefrontobjrendersystem->render(pgpucontext, this);

      }

      auto ppointlightrendersystem = m_ppointlightrendersystem;

      if(ppointlightrendersystem)
      {

         ppointlightrendersystem->render(pgpucontext, this);

      }*/

//      pgpucontext->clear(pgpucontext->current_target_texture(::gpu::current_frame()),argb(0.95f, 0.95f, 0.25f, 0.90f)); // Clear with a transparent

   }


} // namespace SceneFoundry_pbr_renderer



