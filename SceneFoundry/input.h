#pragma once

//#include <functional>
//#include <glm/glm.hpp>
//#include <memory>
//#include "interfaces/window_input_i.h"
//#include "transform_component.h"
#include "bred/graphics3d/input.h"


namespace SceneFoundry_SceneFoundry
{


   class input :
      virtual public ::graphics3d::input
   {
   public:

      //::int_point m_pointLast;
      //bool m_bMouseOut = true;
      input();
      ~input() override;
      
      
      void initialize_input(float moveSpeed = 7.f, const floating_angle &angleCursorPixel = 0.1_f_degree);
      //
      // //void update(float dt, std::shared_ptr<IWindowInput> input, ::graphics3d::transform &transform);
      // void update(float dt, ::graphics3d::transform &transform);
      // void mouseCallback(floating_sequence2 delta);
      // virtual void _001OnMouseMove(const ::int_point &point);
      // virtual void _001OnMouseOut();
      // //float getYaw() const { return m_yaw; }
      // //float getPitch() const { return m_pitch; }
      //
      //
      // float m_moveSpeed;
      // float m_mouseSensitivity;
      // //float m_yaw;
      // //float m_pitch;
      //
      // float m_smoothing = 15.f;
      // floating_sequence2 m_rawDelta{0.f};
      // floating_sequence2 m_smoothDelta{0.f};
      //
      // floating_sequence2 m_mouseDelta{0.f};
   };


} // namespace SceneFoundry_SceneFoundry
 


