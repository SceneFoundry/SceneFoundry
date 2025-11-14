#pragma once

//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
#include "imgui/imgui.h"
#include "bred/graphics3d/camera.h"

namespace SceneFoundry_pbr_renderer
{

   /**
    * A camera that rotates in a circle around a point.
    */
   class CLASS_DECL_SCENEFOUNDRY_PBR_RENDERER RotatingCamera : 
      virtual public ::graphics3d::camera
   {
   public:
      RotatingCamera(floating_sequence3 up, int windowWidth, int windowHeight);

      float mTime = 0.0f;
      float mTimePerCircle = 10;
      float mRadius = 4.0f;
      // view matrix stuff
      floating_sequence3 mUp;
      floating_sequence3 mPosition = floating_sequence3(3.0f, 1.0f, 3.0f);
      // projection matrix stuff
      float mFov = 45.0f;
      int mWindowDimensions[2] = {800, 600};
      float mZNear = 0.1f;
      float mZFar = 100.0f;

      void setWindowDimensions(int width, int height);
      floating_sequence3 getPosition();
      floating_matrix4 getViewMatrix();
      floating_matrix4 getProjectionMatrix();
      void update(float frameTimeDelta);
      void drawDebugPanel();

      // private:
      floating_sequence3 getDirection();
      float getAspectRatio();

      // private:
   };


} // namespace SceneFoundry_pbr_renderer
