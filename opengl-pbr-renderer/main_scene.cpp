// From continuum (V0idsEmbrace@Twitch)
// by camilo on 2025-05-07 02:18 <3ThomasBorregaardSorensen!!
#include "framework.h"
#include "main_scene.h"
#include "application.h"
#include "camera.h"
#include "global_ubo.h"
#include "immersion.h"
#include "impact.h"
#include "input.h"
#include "bred/gpu/context.h"
#include "bred/graphics3d/render_systems/gltf_render_system.h"
#include "bred/graphics3d/render_systems/wavefront_obj_render_system.h"
#include "bred/graphics3d/render_systems/point_light_render_system.h"
#include "bred/graphics3d/render_systems/scene_render_system.h"
#include "bred/graphics3d/render_systems/skybox_ibl_render_system.h"
#include "bred/graphics3d/camera.h"
#include "bred/graphics3d/engine.h"
#include "bred/graphics3d/point_light.h"
#include "bred/graphics3d/scene_object.h"
#include "bred/prodevian/actor.h"
#include "bred/prodevian/game_object.h"


namespace SceneFoundry_opengl_pbr_renderer
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

      //m_pusergraphics3d = m_pengine->m_pusergraphics3d;

      auto pprodevianactor = øcreate_new<::prodevian::actor>();

      pprodevianactor->initialize_prodevian_actor(this);

      ::cast<immersion> pimmersion = m_pimmersionlayer;

      pprodevianactor->transform().m_vec3Position = pimmersion->m_initialCameraPosition;
      pprodevianactor->transform().m_vec3Rotation = pimmersion->m_initialCameraRotation;
      // pprodevianactor->onInit();

      m_prodevianactora.add(pprodevianactor);


      m_papp->m_pmainscene = this;

      if (!m_pcameraDefault)
      {

      }

   }


   //
   //
   //
   // ::pointer < ::graphics3d::camera > main_scene::get_default_camera()
   // {
   //    //glm::vec3 camera = glm::vec3(0.0f, 1.0f *m_pengine->m_fYScale, 3.0f);
   //    glm::vec3 camera = glm::vec3(0.0f, 1.0f , 3.0f);
   //    glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f); // Look at origin
   //    //glm::vec3 direction = glm::normalize(target - cameraPos);
   //    //camera camera{ glm::vec3(0.0f, 2.0f, -15.0f), -90.0f, 0.0f };
   //    auto pcamera = øcreate < ::graphics3d::camera>();
   //    pcamera->m_pengine = m_pengine;
   //    pcamera->initialize_camera(target, camera);
   //    //pcamera->m_pimpact = m_pimpact;
   //    return pcamera;
   //
   //
   // }

   //void main_scene::on_load_scene(::gpu::context* pgpucontext)
   //{


   //}


   void main_scene::on_load_scene(::gpu::context* pgpucontext)
   {

      m_gpupropertiesGlobalUbo.set<::SceneFoundry_opengl_pbr_renderer::global_ubo>();

      loadSceneFile("default_scene");


//
//      //::graphics3d::sky_box::cube cube = {
//
//      //   // Cloudy skybox
//      //  /* "D:/Users/Admin/source/repos/MyHell2024/MyHell2024/res/Textures/CloudySkybox/bluecloud_ft.jpg",
//      //   "D:/Users/Admin/source/repos/MyHell2024/MyHell2024/res/Textures/CloudySkybox/bluecloud_bk.jpg",
//      //   "D:/Users/Admin/source/repos/MyHell2024/MyHell2024/res/Textures/CloudySkybox/bluecloud_dn.jpg",
//      //   "D:/Users/Admin/source/repos/MyHell2024/MyHell2024/res/Textures/CloudySkybox/bluecloud_up.jpg",
//      //   "D:/Users/Admin/source/repos/MyHell2024/MyHell2024/res/Textures/CloudySkybox/bluecloud_rt.jpg",
//      //   "D:/Users/Admin/source/repos/MyHell2024/MyHell2024/res/Textures/CloudySkybox/bluecloud_lf.jpg",*/
//
//      //   // Hell skybox
//      //  /* "D:/Users/Admin/source/repos/MyHell2024/MyHell2024/res/Textures/CoolSkybox/NightSky_Front.png",
//      //   "D:/Users/Admin/source/repos/MyHell2024/MyHell2024/res/Textures/CoolSkybox/NightSky_Back.png",
//
//      //    "D:/Users/Admin/source/repos/MyHell2024/MyHell2024/res/Textures/CoolSkybox/NightSky_Top.png",
//      //    "D:/Users/Admin/source/repos/MyHell2024/MyHell2024/res/Textures/CoolSkybox/NightSky_Bottom.png",
//
//      //   "D:/Users/Admin/source/repos/MyHell2024/MyHell2024/res/Textures/CoolSkybox/NightSky_Left.png",
//      //   "D:/Users/Admin/source/repos/MyHell2024/MyHell2024/res/Textures/CoolSkybox/NightSky_Right.png",
//      // */
//
//
//      // // Space sky_box
//      //   {"matter://textures/SpaceSkybox/right.png",
//      //  "matter://textures/SpaceSkybox/left.png",
//      //  "matter://textures/SpaceSkybox/top.png",
//      //  "matter://textures/SpaceSkybox/bottom.png",
//      //  "matter://textures/SpaceSkybox/front.png",
//      //  "matter://textures/SpaceSkybox/back.png"
//      //  }
//      //};
//
////      m_Skybox = øallocate::graphics3d::sky_box();
//
//      for (auto& strSkybox : m_papp->m_straSkybox)
//      {
//
//         auto& pskybox = m_mapSkybox[strSkybox];
//
//         ødefer_construct_new(pskybox);
//
//         pskybox->initialize_sky_box(m_pengine, strSkybox);
//
//      }
//
//
//
//      float fXScale;
//
//      fXScale = m_pengine->m_fYScale;
//
//      {
//
//         auto & flatVase = scene_object("matter://models/flat_vase.obj");
//         flatVase.translate({ -.5f, 0.f, 0.f });
//         flatVase.scale({3.f, -1.5f, 3.f * fXScale }); // The vase is upside down.
//
//      }
//
//      {
//
//         auto & floor = scene_object("matter://models/quad.obj");
//         floor.translate({0.f, 0.f, 0.f});
//         floor.scale({5.f, -1.f, 5.f * fXScale });
//
//      }
//
//      {
//
//         auto & smoothVase = scene_object("matter://models/smooth_vase.obj");
//         smoothVase.translate({.5f, .0f, 0.f});
//         smoothVase.scale({3.f, -1.5f, 3.f * fXScale }); // The vase is upside down.
//
//      }
//
//      {
//
//         auto & stoneSphere = scene_object("matter://models/StoneSphere.obj");
//         stoneSphere.translate({ .0f, 0.0f, 0.f });
//         stoneSphere.scale({.25f, .25f, .25f });
//
//      }
//
//      {
//
//         auto & woodBarrel = scene_object("matter://models/Barrel_OBJ.obj");
//         woodBarrel.translate({ 1.f, 0.f, 1.0f });
//         woodBarrel.scale({1.f, 1.f, 1.f });
//
//      }
//
//      float fLo = 0.5f;
//
//      ::array_base <::color::color > lightColors;
//
//      lightColors.add(::rgb(1.f, fLo, fLo));
//      lightColors.add(::rgb(fLo, fLo, 1.f));
//      lightColors.add(::rgb(fLo, 1.f, fLo));
//      lightColors.add(::rgb(1.f, 1.f, fLo));
//      lightColors.add(::rgb(fLo, 1.f, 1.f));
//      lightColors.add(::rgb(1.f, 1.f, 1.));
//
//      for (int i = 0; i < lightColors.size(); i++) 
//      {
//
//         auto ppointlight = create_point_light();
//
//         ppointlight->m_color = lightColors[i];
//
//         auto rotateLight = glm::rotate(
//            glm::mat4(1.f),
//            (i * glm::two_pi<float>()) / lightColors.size(),
//            { 0.f, 1.f, 0.f });
//
//
//         ppointlight->m_fLightIntensity = 1.0f;
//
//         ppointlight->transform().m_vec3Translation = glm::vec3(rotateLight * glm::vec4(-1.f, 1.7f, 0.5f , 1.f));
//         //add_object(pointLight);
//
//      }
      øconstruct(m_pscenerendersystem);

      m_pscenerendersystem->initialize_render_system(m_pimmersionlayer->m_pengine);

      m_pscenerendersystem->prepare(pgpucontext);

      øconstruct(m_pgltfrendersystem);


      m_pgltfrendersystem->m_bDisableAlbedo = false;
      m_pgltfrendersystem->m_bDisableMetallicRoughness = false;
      m_pgltfrendersystem->m_bDisableNormal = false;
      m_pgltfrendersystem->m_bDisableAmbientOcclusion = false;
      m_pgltfrendersystem->m_bDisableEmissive = false;

      m_pgltfrendersystem->m_bForceDefaultAlbedo = true;
      m_pgltfrendersystem->m_bForceDefaultMetallicFactor = true;
      m_pgltfrendersystem->m_bForceDefaultRoughnessFactor = true;
      m_pgltfrendersystem->m_bForceDefaultAmbientOcclusionFactor = true;
      m_pgltfrendersystem->m_bForceDefaultEmission = true;
      m_pgltfrendersystem->m_seq3DefaultAlbedo = {};
      m_pgltfrendersystem->m_fDefaultMetallicFactor = 0.0f;
      m_pgltfrendersystem->m_fDefaultRoughnessFactor = 1.0f;
      m_pgltfrendersystem->m_fDefaultAmbientOcclusionFactor = 1.0f;
      m_pgltfrendersystem->m_seq3DefaultEmission = {};


      m_pgltfrendersystem->initialize_render_system(m_pimmersionlayer->m_pengine);

      m_pgltfrendersystem->prepare(pgpucontext);


      øconstruct_new(m_pwavefrontobjrendersystem);

      m_pwavefrontobjrendersystem->initialize_render_system(m_pimmersionlayer->m_pengine);

      m_pwavefrontobjrendersystem->prepare(pgpucontext);
      //m_prenderer->getRenderPass(),
      //globalSetLayout->getDescriptorSetLayout() };

      øconstruct_new(m_ppointlightrendersystem);

      m_ppointlightrendersystem->initialize_render_system(m_pimmersionlayer->m_pengine);

      m_ppointlightrendersystem->prepare(pgpucontext);

      øconstruct_new(m_pskyboxiblrendersystem);

      m_pskyboxiblrendersystem->initialize_render_system(m_pimmersionlayer->m_pengine);

      m_pskyboxiblrendersystem->prepare(pgpucontext);

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

      //m_pskyboxiblrendersystem->set_skybox(get_skybox());

      auto& globalubo = this->global_ubo();



      //pgpucontext->clear(::argb(.5f, 0.f, 0.f, 0.5f));

      //::graphics3d::GlobalUbo ubo{};

      auto pimmersion = m_pimmersionlayer;

      auto pscene = pimmersion->m_pscene;

      auto pgpucamera = pscene->camera();

      ::cast<SandboxCamera> pcamera = pgpucamera;

      auto dt = m_pimmersionlayer->m_pengine->dt();
                   
      ::cast<SandboxMNKController> pinput = m_pimmersionlayer->m_pengine->m_pinput;
      //double dx = 0, dy = 0;
      //m_pInput->getMouseDelta(dx, dy);
      //m_controller.mouseCallback(glm::vec2(dx, dy));
      pinput->update(dt, m_pimmersionlayer->m_pengine->m_transform);

      pcamera->setPosition(m_pimmersionlayer->m_pengine->m_transform.m_vec3Position);
      pcamera->setRotation(m_pimmersionlayer->m_pengine->m_transform.m_vec3Rotation);


      //float aspect = h == 0 ? 1.0f : static_cast<float>(w) / h;
      auto aspect = m_pimmersionlayer->m_pengine->m_pusergraphics3d->getAspectRatio();
      pcamera->updateProjection(aspect, 0.1f, 300.f);


      pcamera->updateVectors();
      pcamera->updateView();


      auto projection = pcamera->getProjectionMatrix();
      globalubo["projection"] = projection;

      auto view = pcamera->getViewMatrix();
      globalubo["view"] = view;

      auto viewPos = glm::vec4(pcamera->getPosition(), 1.0f);
      globalubo["viewPos"] = viewPos;

      //auto inverseView = pcamera->getInverseView();
      //globalubo["invView"] = inverseView;

      debug() << "main_scene::on_update";

      if (m_ppointlightrendersystem)
      {

         m_ppointlightrendersystem->update(pgpucontext, this);

      }


   }


   //::graphics3d::skybox* main_scene::get_skybox()
   //{

   //   ::string strSkybox = m_papp->m_strSkybox;

   //   return m_mapSkybox[strSkybox];

   //}

   
   void main_scene::on_render(::gpu::context * pgpucontext)
   {

      //pgpucontext->clear(rgba(0.5f, 0.75f, 1.0f, 1.0f)); // Clear with a light blue color

      auto pskyboxiblrendersystem = m_pskyboxiblrendersystem;

      if (pskyboxiblrendersystem)
      {

         pskyboxiblrendersystem->render(pgpucontext, this);

      }

      auto pscenerendersystem = m_pscenerendersystem;

      if (pscenerendersystem)
      {

         pscenerendersystem->render(pgpucontext, this);
      }

      auto pgltfrendersystem = m_pgltfrendersystem;

      if (pgltfrendersystem)
      {

         pgltfrendersystem->render(pgpucontext, this);

      }

      //return;

      auto pwavefrontobjrendersystem = m_pwavefrontobjrendersystem;

      if (pwavefrontobjrendersystem)
      {

         pwavefrontobjrendersystem->render(pgpucontext, this);

      }

      auto ppointlightrendersystem = m_ppointlightrendersystem;

      if(ppointlightrendersystem)
      {

         ppointlightrendersystem->render(pgpucontext, this);

      }

   }


} // namespace SceneFoundry_opengl_pbr_renderer



