#include "framework.h"
#include "camera.h"
#include "bred/gpu/context.h"
#include "bred/graphics3d/_functions.h"
#include "bred/graphics3d/engine.h"
#include "acme/prototype/geometry/matrix.h"
#include <algorithm>

namespace SceneFoundry_pbr_renderer
{

   Camera::Camera()
   {


   }


   Camera::~Camera()
   {


   }

      // Camera::Camera(floating_sequence3 up, floating_sequence3 position, float yaw, float pitch, int windowWidth, int windowHeight) :
   //    mUp(up), mPosition(position), mYaw(yaw), mPitch(pitch)
   //{
   //   mWindowDimensions[0] = windowWidth;
   //   mWindowDimensions[1] = windowHeight;
   //}

   void Camera::initialize_Camera(floating_sequence3 up, floating_sequence3 position, float yaw, float pitch, int windowWidth,
                                  int windowHeight)
   {
      mUp = up;

      mPosition = position;

      mYaw = yaw;

      mPitch = pitch;

      mWindowDimensions[0] = windowWidth;
      mWindowDimensions[1] = windowHeight;
   }

   floating_sequence3 Camera::getDirection()
   {
      float directionY = sin(mPitch);
      float directionX = cos(mPitch) * cos(mYaw);
      float directionZ = cos(mPitch) * sin(mYaw);

      return floating_sequence3(directionX, directionY, -directionZ).normalized();
   }

   float Camera::getAspectRatio() { return (float)mWindowDimensions[0] / mWindowDimensions[1]; }


   void Camera::deferSetWindowDimensions(int width, int height)
   {
      mWindowDimensions[0] = width;
      mWindowDimensions[1] = height;
   }

   floating_sequence3 Camera::getPosition() { return mPosition; }

   floating_matrix4 Camera::getViewMatrix()
   {
    //  auto pgpucontext = m_pengine->gpu_context();

      // return pgpucontext->lookAt(mPosition, mPosition + getDirection(), mUp); }
      return ::graphics3d::lookAt(mPosition, mPosition + getDirection(), mUp);
   //}
   }

   floating_matrix4 Camera::getProjectionMatrix()
   {
      //auto pgpucontext = m_pengine->gpu_context();
      floating_matrix4 projection = m_pengine->perspective(::radians(mFov), getAspectRatio(), mZNear, mZFar);
      return projection;
   }

   void Camera::processActions(float frameTimeDelta, const ::set<KeymapAction> &actions)
   {
      float normalizedSpeed = mSpeed * frameTimeDelta;

      if (actions.find(KeymapAction::MOVE_FORWARD) != actions.end())
      {
         mPosition += normalizedSpeed * getDirection();
      }

      if (actions.find(KeymapAction::MOVE_BACKWARD) != actions.end())
      {
         mPosition -= normalizedSpeed * getDirection();
      }

      if (actions.find(KeymapAction::MOVE_LEFT) != actions.end())
      {
         mPosition -= normalizedSpeed * getDirection().crossed(mUp);
      }

      if (actions.find(KeymapAction::MOVE_RIGHT) != actions.end())
      {
         mPosition += normalizedSpeed * getDirection().crossed(mUp);
      }

      if (actions.find(KeymapAction::MOVE_UP) != actions.end())
      {
         mPosition += normalizedSpeed * mUp;
      }

      if (actions.find(KeymapAction::MOVE_DOWN) != actions.end())
      {
         mPosition -= normalizedSpeed * mUp;
      }
   }

   void Camera::processMouse(const MousePosition &mousePosition)
   {

      if (!mInitialMousePositionSet)
      {
         mLastMouseX = mousePosition.x;
         mLastMouseY = mousePosition.y;
         mInitialMousePositionSet = true;
      }

      float dX = mLastMouseX - mousePosition.x;
      float dY = mLastMouseY - mousePosition.y;

      mLastMouseX = mousePosition.x;
      mLastMouseY = mousePosition.y;

      mYaw += dX * mSensitivity;
      mPitch += dY * mSensitivity;

      mPitch = std::min(mPitch, 89.0f); // can't go to 90 because of lookAt up vector won't work
      mPitch = std::max(mPitch, -89.0f);
   }

   void Camera::resetMouse() { mInitialMousePositionSet = false; }

   void Camera::drawDebugPanel()
   {
      //if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
      //{
      //   ImGui::Text("View");
      //   ImGui::InputFloat3("up", &mUp.x);
      //   ImGui::InputFloat3("position", &mPosition.x);

      //   //auto yawDegrees = mYaw.d;
      //   //auto pitchDegrees = ::degrees(mPitch);
      //   float aspectRatio = getAspectRatio();

      //   //ImGui::InputFloat("yaw (degrees)", &yawDegrees);
      //   //ImGui::InputFloat("pitch (degrees)", &pitchDegrees);

      //   ImGui::Separator();

      //   ImGui::Text("Projection");
      //   ImGui::SliderFloat("fov (degrees)", &mFov, 0.0f, 180.0f);
      //   ImGui::InputInt2("window", mWindowDimensions);
      //   ImGui::InputFloat("aspect ratio", &aspectRatio);
      //   ImGui::DragFloat("z-plane near", &mZNear, 0.01f, 0.0f, 1000.0f);
      //   ImGui::DragFloat("z-plane far", &mZFar, 0.01f, 0.0f, 1000.0f);
      //}
   }

} // namespace SceneFoundry_pbr_renderer
 