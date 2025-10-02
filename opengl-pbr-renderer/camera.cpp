#include "framework.h"
#include "camera.h"


namespace SceneFoundry_opengl_pbr_renderer
{

   SandboxCamera::SandboxCamera()
   {


   }

      SandboxCamera::~SandboxCamera() {}

   void SandboxCamera::initialize_SandboxCamera(glm::vec3 position, float yawDeg, float pitchDeg, float zoomDeg) 
       
   {
      m_vec3Position = position;
      
      m_worldUp = {0.f, -1.f, 0.f};
      
      yaw() = yawDeg;
      
      
      pitch() = pitchDeg;
      
      
      m_zoom = zoomDeg;

      updateVectors();
      updateView();
   }

   void SandboxCamera::updateVectors()
   {
      glm::vec3 front;
      //front.x = cos(glm::radians(this->yaw())) * cos(glm::radians(this->pitch()));
      //front.y = sin(glm::radians(this->pitch()));
      //front.z = sin(glm::radians(this->yaw())) * cos(glm::radians(this->pitch()));
      front.x = cos(this->yaw()) * cos(this->pitch());
      front.y = sin(this->pitch());
      front.z = sin(this->yaw()) * cos(this->pitch());
      m_front = glm::normalize(front);

      // Recalculate Right and Up vector
      m_right = glm::normalize(glm::cross(m_front, m_worldUp)); // Right vector
      m_up = glm::normalize(glm::cross(m_right, m_front));
   }

   void SandboxCamera::updateView()
   {
      m_viewMatrix = glm::lookAt(m_vec3Position, m_vec3Position + m_front, m_up);
      m_inverseViewMatrix = glm::inverse(m_viewMatrix);
   }

   void SandboxCamera::updateProjection(float aspect, float nearZ, float farZ)
   {
      m_projMatrix = glm::perspective(glm::radians(m_zoom), aspect, nearZ, farZ);
      m_projMatrix[1][1] *= -1; // Vulkan Y-flip
   }

   void SandboxCamera::move(glm::vec3 delta)
   {
      m_vec3Position += delta;
      updateView();
   }

   void SandboxCamera::rotate(float yawOffset, float pitchOffset)
   {
      this->yaw() += yawOffset;
      this->pitch() += pitchOffset;

      this->pitch() = glm::clamp(this->pitch(), -89.f, 89.f);
      updateVectors();
      updateView();
   }

   void SandboxCamera::setZoom(float zoom) { m_zoom = glm::clamp(zoom, 1.f, 120.f); }

   void SandboxCamera::setRotation(glm::vec3 euler)
   {
      this->pitch() = euler.x;
      this->yaw() = euler.y;
   }


} // namespace SceneFoundry_opengl_pbr_renderer
 