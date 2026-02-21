#include "framework.h"
#include "rotatingcamera.h"
#include "bred/gpu/context.h"
#include "bred/graphics3d/engine.h"
#include "bred/graphics3d/_functions.h"

//#include "gpu/gltf/_constant.h"


namespace SceneFoundry_pbr_renderer
{

   float RotatingCamera::getAspectRatio() { return (float)mWindowDimensions[0] / mWindowDimensions[1]; }

   RotatingCamera::RotatingCamera(floating_sequence3 up, int windowWidth, int windowHeight) : mUp(up)
   {
      mWindowDimensions[0] = windowWidth;
      mWindowDimensions[1] = windowHeight;
   }

   void RotatingCamera::setWindowDimensions(int width, int height)
   {
      mWindowDimensions[0] = width;
      mWindowDimensions[1] = height;
   }

   floating_sequence3 RotatingCamera::getPosition() { return mPosition; }

   floating_matrix4 RotatingCamera::getViewMatrix() { 
      //auto pgpucontext = m_pengine->gpu_context();
      
      //return pgpucontext->look_at(mPosition, ::gpu::gltf::origin, mUp); 
      return ::graphics3d::lookAt(mPosition, ::graphics3d::origin, mUp); 
   }

   floating_matrix4 RotatingCamera::getProjectionMatrix()
   {

      //auto pgpucontext = m_pengine->gpu_context();
      floating_matrix4 projection = m_pengine->perspective(::radians(mFov), getAspectRatio(), mZNear, mZFar);
      return projection;
   }

   void RotatingCamera::update(float frameTimeDelta)
   {
      mTime += frameTimeDelta;

      float percentOfCircle = mTime / mTimePerCircle;
      float angle = percentOfCircle * 2.f * πf;

      mPosition.x = sin(angle) * mRadius;
      mPosition.z = cos(angle) * mRadius;
   }

   void RotatingCamera::drawDebugPanel()
   {
      //if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
      //{
      //   ImGui::Text("View");
      //   ImGui::InputFloat3("up", &mUp.x);
      //   ImGui::InputFloat3("position", &mPosition.x);

      //   float aspectRatio = getAspectRatio();

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


