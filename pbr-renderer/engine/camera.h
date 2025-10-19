#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "imgui/imgui.h"
//#include "windowmanager.h"

namespace SceneFoundry_pbr_renderer
{

   /**
    * A free flight first-person camera.
    */
   class CLASS_DECL_SCENEFOUNDRY_PBR_RENDERER Camera :
      virtual public ::particle
   {
   public:
      //Camera(glm::vec3 up, glm::vec3 position, float yaw, float pitch, int windowWidth, int windowHeight);

      Camera();
      ~Camera() override;


      virtual void initialize_Camera(glm::vec3 up, glm::vec3 position, float yaw, float pitch, int windowWidth, int windowHeight);

      // void setWindowDimensions(const WindowSize &windowSize);
      void deferSetWindowDimensions(int width, int height);
      glm::vec3 getPosition();
      glm::mat4 getViewMatrix();
      glm::mat4 getProjectionMatrix();
      void processActions(float frameTimeDelta, const ::set<KeymapAction> &actions);
      void processMouse(const MousePosition &mousePosition);
      void resetMouse();
      void drawDebugPanel();

      // private:
      glm::vec3 getDirection();
      float getAspectRatio();

   //private:
      float mSensitivity = 0.0001;
      float mSpeed = 5.0f;
      bool mInitialMousePositionSet = false;
      double mLastMouseX;
      double mLastMouseY;

      // view matrix stuff
      glm::vec3 mUp;
      glm::vec3 mPosition;
      float mYaw;
      float mPitch;

      // projection matrix stuff
      float mFov = 45.0f;
      int mWindowDimensions[2] = {800, 600};
      float mZNear = 0.1f;
      float mZFar = 100.0f;
   };

} // namespace SceneFoundry_pbr_renderer
