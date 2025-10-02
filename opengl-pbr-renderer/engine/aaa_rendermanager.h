#pragma once

//#include "glad/glad.h"
////#include "glfw3.h"
//
////#include "bloomframebuffer.h"
////#include "cameramanager.h"
////#include "engineconfig.h"
////#include "framebuffer.h"
////#include "fullscreenquad.h"
////#include "scene.h"
////#include "skybox.h"
////#include "shader.h"
////#include "model.h"
////#include "ibl/diffuseirradiancemap.h"
////#include "ibl/equirectangularcubemap.h"
////#include "ibl/specularmap.h"
////#include "windowmanager.h"
//
//namespace SceneFoundry_opengl_pbr_renderer
//{
//
//   const int TEXTURE_UNIT_DIFFUSE_IRRADIANCE_MAP = 10;
//   const int TEXTURE_UNIT_PREFILTERED_ENV_MAP = 11;
//   const int TEXTURE_UNIT_BRDF_CONVOLUTION_MAP = 12;
//
//   enum BloomDirection
//   {
//      BOTH = 0,
//      HORIZONTAL = 1,
//      VERTICAL = 2
//   };
//
//   /**
//    * Manages rendering.
//    */
//   class RenderManager :
//      virtual public ::particle
//   {
//   public:
//      //RenderManager(EngineConfig &engineConfig, ::pointer<WindowManager> windowManager,
//      //              ::pointer<CameraManager> cameraManager);
//      virtual void initialize_RenderManager(::user::graphics3d * pusergraphics3d,
//                    CameraManager * pcameramanager);
//      void startup(::graphics3<Scene> scene);
//      void shutdown();
//      void startGuiFrame();
//      void render();
//   private:
//      void renderBloom();
//   private:
//      EngineConfig m_pengineconfig;
//      ::pointer<WindowManager> mWindowManager;
//      ::pointer<CameraManager> mCameraManager;
//
//      // framebuffers
//      ::pointer<Framebuffer> mFramebuffer;
//      ::pointer<BloomFramebuffer> mBloomFramebuffers[2];
//      unsigned int mBloomFramebufferResult;
//
//      // pre-computed IBL stuff
//      ::pointer<EquirectangularCubemap> mIblEquirectangularCubemap;
//      ::pointer<DiffuseIrradianceMap> mIblDiffuseIrradianceMap;
//      ::pointer<SpecularMap> mIblSpecularMap;
//
//      // skybox
//      ::pointer<Skybox> mSkybox;
//
//      // scene
//      ::pointer<Scene> mScene;
//
//      // post-processing
//      bool mBloomEnabled = true;
//      float mBloomIntensity = 1.0;
//      int mBloomIterations = 10;
//      int mBloomDirection = BloomDirection::BOTH;
//      bool mTonemappingEnabled = false;
//      float mGammaCorrectionFactor = 2.2;
//      float mBloomBrightnessCutoff = 1.0; // fragments with brightness above this are blurred
//      ::pointer<FullscreenQuad> mFullscreenQuad;
//
//      // shaders
//      ::pointer<Shader> mPbrShader;
//      ::pointer<Shader> mBloomShader;
//      ::pointer<Shader> mPostShader;
//      ::pointer<Shader> mSkyboxShader;
//   };
//
//
//} // namespace SceneFoundry_opengl_pbr_renderer
//
//
