#pragma once

#include "camera.h"
//#include "engineconfig.h"
#include "inputmanager.h"
//#include "windowmanager.h"

namespace SceneFoundry_pbr_renderer
{


   /**
    * Manages creating and updating cameras.
    */
   class CLASS_DECL_SCENEFOUNDRY_PBR_RENDERER CameraManager :
      virtual public ::particle
   {
   public:


      //EngineConfig m_pengineconfig;
      //::pointer<WindowManager> mWindowManager;
      ::pointer<::user::graphics3d> m_pusergraphics3d;
      ::pointer<InputManager> m_pinputmanager;

      ::pointer<Camera> m_pcamera;
      bool mMouseCameraEnabled = true;


      //CameraManager(EngineConfig &engineConfig, ::pointer<WindowManager> windowManager,
        //            ::pointer<InputManager> inputManager);
      CameraManager();
      ~CameraManager() override;
      virtual void initialize_CameraManager(
         ::user::graphics3d * pusergraphics3d,
         InputManager * pinputmanager);
      void startup();
      void update(float frameTimeDelta);
      void drawDebugPanel();

      floating_sequence3 getPosition();
      floating_matrix4 getViewMatrix();
      floating_matrix4 getProjectionMatrix();
      // private:
   };


} // namespace SceneFoundry_pbr_renderer



