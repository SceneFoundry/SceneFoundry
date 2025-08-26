#pragma once

//#include <functional>
//#include <glm/glm.hpp>
//#include <memory>
//#include "interfaces/window_input_i.h"
//#include "transform_component.h"
#include "bred/graphics3d/input.h"


namespace SceneFoundry_SceneFoundry
{


   class SandboxMNKController :
      virtual public ::graphics3d::input
   {
   public:

      ::int_point m_pointLast;
      
      SandboxMNKController();
      ~SandboxMNKController() override;
      
      
      void initialize_SandboxMNKController(float moveSpeed = 7.f, float mouseSensitivity = 0.08f);

      //void update(float dt, std::shared_ptr<IWindowInput> input, ::graphics3d::transform &transform);
      void update(float dt, ::graphics3d::transform &transform);
      void mouseCallback(glm::vec2 delta);
      virtual void _001OnMouseMove(const ::int_point &point);
      float getYaw() const { return m_yaw; }
      float getPitch() const { return m_pitch; }


      float m_moveSpeed;
      float m_mouseSensitivity;
      float m_yaw;
      float m_pitch;

      float m_smoothing = 15.f;
      glm::vec2 m_rawDelta{0.f};
      glm::vec2 m_smoothDelta{0.f};

      glm::vec2 m_mouseDelta{0.f};
   };


} // namespace SceneFoundry_SceneFoundry
 


