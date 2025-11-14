#pragma once

//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
#include "imgui/imgui.h"
#include "bred/graphics3d/camera.h"
//#include "windowmanager.h"

namespace SceneFoundry_pbr_renderer
{

   /**
    * A free flight first-person camera.
    */
   class CLASS_DECL_SCENEFOUNDRY_PBR_RENDERER Camera :
      virtual public ::graphics3d::camera
   {
   public:
      //Camera(floating_sequence3 up, floating_sequence3 position, float yaw, float pitch, int windowWidth, int windowHeight);

      Camera();
      ~Camera() override;


      virtual void initialize_Camera(floating_sequence3 up, floating_sequence3 position, float yaw, float pitch, int windowWidth, int windowHeight);

      // void setWindowDimensions(const WindowSize &windowSize);
      void deferSetWindowDimensions(int width, int height);
      floating_sequence3 getPosition();
      floating_matrix4 getViewMatrix();
      floating_matrix4 getProjectionMatrix();
      void processActions(float frameTimeDelta, const ::set<KeymapAction> &actions);
      void processMouse(const MousePosition &mousePosition);
      void resetMouse();
      void drawDebugPanel();

      // private:
      floating_sequence3 getDirection();
      float getAspectRatio();

   //private:
      float mSensitivity = 0.0001;
      float mSpeed = 5.0f;
      bool mInitialMousePositionSet = false;
      double mLastMouseX;
      double mLastMouseY;

      // view matrix stuff
      floating_sequence3 mUp;
      floating_sequence3 mPosition;
      float mYaw;
      float mPitch;

      // projection matrix stuff
      float mFov = 45.0f;
      int mWindowDimensions[2] = {800, 600};
      float mZNear = 0.1f;
      float mZFar = 100.0f;
   };

} // namespace SceneFoundry_pbr_renderer
