#include "framework.h"
#include "camera.h"
#include "bred/graphics3d/engine.h"
#include "bred/gpu/context.h"


namespace SceneFoundry_pbr_renderer
{

   camera::camera()
   {


   }


   camera::~camera()
   {
   

   }

   void camera::initialize_camera(const ::floating_sequence3 &position,
                                     const ::graphics3d::floating_rotation &rotation,
                                     const floating_angle &zoom)
   {

      ::graphics3d::camera::initialize_camera(position, rotation, zoom);
      // m_sequence3Position = position;
      //
      // m_worldUp = {0.f, 1.f, 0.f};
      //
      // m_rotation = rotation;
      //
      // m_zoom = zoom;

      //updateVectors();

      //updateView();

   }


   // void camera::updateVectors()
   // {
   //
   //    floating_sequence3 front;
   //
   //    //front.x = cos(::radians(this->yaw())) * cos(::radians(this->pitch()));
   //    //front.y = sin(::radians(this->pitch()));
   //    //front.z = sin(::radians(this->yaw())) * cos(::radians(this->pitch()));
   //
   //    front.x = cos(this->yaw()) * cos(this->pitch());
   //    front.y = sin(this->pitch());
   //    front.z = sin(this->yaw()) * cos(this->pitch());
   //
   //    m_front = front.normalized();
   //
   //    // Recalculate Right and Up vector
   //    m_right = m_front.crossed(m_worldUp).normalized(); // Right vector
   //
   //    m_up = m_right.crossed(m_front).normalized();
   //
   // }
   //
   // // opengl z+ (out of the screen) towards the viewer
   //
   // void camera::updateView()
   // {
   //
   //    auto pgpucontext = m_pengine->gpu_context();
   //    m_viewMatrix = pgpucontext->lookAt(m_vec3Position, m_vec3Position + m_front, m_up);
   //    m_inverseViewMatrix = m_viewMatrix.inversed();
   // }
   //
   // void SandboxCamera::updateProjection(float aspect, float nearZ, float farZ)
   // {
   //    auto pgpucontext = m_pengine->gpu_context();
   //    m_projMatrix = pgpucontext->perspective(::radians(m_zoom), aspect, nearZ, farZ);
   //    m_projMatrix[1][1] *= -1; // Vulkan Y-flip
   // }

   // void SandboxCamera::move(floating_sequence3 delta)
   // {
   //    m_vec3Position += delta;
   //    updateView();
   // }
   //
   // void SandboxCamera::rotate(float yawOffset, float pitchOffset)
   // {
   //    this->yaw() += yawOffset;
   //    this->pitch() += pitchOffset;
   //
   //    this->pitch() = geometry::clamp(this->pitch(), -89.f, 89.f);
   //    updateVectors();
   //    updateView();
   // }
   //
   // void SandboxCamera::setZoom(float zoom) { m_zoom = geometry::clamp(zoom, 1.f, 120.f); }
   //
   // void SandboxCamera::setRotation(floating_sequence3 euler)
   // {
   //    this->pitch() = euler.x;
   //    this->yaw() = euler.y;
   // }


} // namespace SceneFoundry_pbr_renderer
 