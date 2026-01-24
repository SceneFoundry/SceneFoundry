// player_input.cpp
#include "framework.h"
#include "input.h"
#include "bred/graphics3d/engine.h"
#include "bred/graphics3d/immersion_layer.h"
#include "bred/graphics3d/scene_base.h"


namespace SceneFoundry_SceneFoundry
{


   input::input()
   {


   }


      input::~input() {}


   void input::initialize_input(float moveSpeed, const float_angle & mouseSensitivity)
   {
      ::graphics3d::input::initialize_input(moveSpeed, mouseSensitivity);;
      //m_moveSpeed = moveSpeed;
      //m_mouseSensitivity = mouseSensitivity;
      
      //m_yaw = ::radians(-90.f);
      
      //m_pitch = 0.f ;
   }

   // void input::_001OnMouseMove(const ::int_point & point)
   // {
   //
   //    if (m_mousestate.m_buttons.left)
   //    {
   //
   //       if (point != m_pointLast)
   //       {
   //
   //          if (!m_bMouseOut)
   //          {
   //
   //             auto Δ = point - m_pointLast;
   //
   //             mouseCallback({(float) Δ.cx, (float) Δ.cy});
   //          }
   //
   //          m_bMouseOut = false;
   //
   //       }
   //
   //    }
   //
   //    m_pointLast = point;
   //
   // }
   //
   //
   // void input::_001OnMouseOut()
   // {
   //
   //    m_bMouseOut = true;
   //
   // }
   //
   // void input::mouseCallback(floating_sequence2 delta)
   // {
   //
   //    m_rawDelta = delta;
   //
   // }
   // //void input::update(
   // // float dt, std::shared_ptr<IWindowInput> input,
   //    //TransformComponent &transform)
   //
   //    void input::update(float dt, ::graphics3d::transform & transform)
   // {
   //    if (dt <= 0.0f)
   //       return;
   //
   //    // --- 1) Smooth raw mouse delta ---
   //    float alpha = 1.0f - std::exp(-m_smoothing * dt);
   //    m_smoothDelta += (m_rawDelta - m_smoothDelta) * alpha;
   //
   //    // convert pixels -> degrees
   //    float deltaYaw = m_smoothDelta.x * m_mouseSensitivity;
   //    float deltaPitch = -m_smoothDelta.y * m_mouseSensitivity; // invert Y for typical FPS
   //
   //    auto pcamera = m_pengine->m_pimmersionlayer->m_pscene->camera();
   //    auto &rotation = pcamera->rotation();
   //    //auto &pitch = pcamera->pitch();
   //    // --- 2) Update camera rotation ---
   //    rotation.m_angleYaw += deltaYaw;
   //    rotation.m_anglePitch += deltaPitch;
   //
   //    if (rotation.m_anglePitch > 89_f_degrees)
   //       rotation.m_anglePitch = 89_f_degrees;
   //    else if (rotation.m_anglePitch < -89_f_degrees)
   //       rotation.m_anglePitch = -89_f_degrees;
   //    rotation.m_anglePitch = geometry::clamp(rotation.m_anglePitch, -89_f_degrees, 89_f_degrees);
   //
   //    transform.m_rotation = rotation;
   //
   //    // --- 3) Compute movement basis ---
   //    floating_sequence3 front =  rotation{std::cos(yaw) * std::cos(pitch), std::sin(pitch),
   //                    std::sin(yaw) * std::cos(pitch)};
   //    front = front.normalized();
   //    floating_sequence3 right = front.crossed(floating_sequence3(0.f, 1.f, 0.f));
   //    floating_sequence3 up = floating_sequence3(0.f, 1.f, 0.f);
   //
   //    // --- 4) Apply WASDQE movement ---
   //    floating_sequence3 dir{0.f};
   //    if (IsKeyPressed(::user::e_key_w))
   //       dir += front;
   //    if (IsKeyPressed(::user::e_key_s))
   //       dir -= front;
   //    if (IsKeyPressed(::user::e_key_a))
   //       dir -= right;
   //    if (IsKeyPressed(::user::e_key_d))
   //       dir += right;
   //    if (IsKeyPressed(::user::e_key_q))
   //       dir -= up;
   //    if (IsKeyPressed(::user::e_key_e))
   //       dir += up;
   //
   //    if (dir.modulus() > 1e-6f)
   //    {
   //       dir.normalize();
   //       float speed = m_moveSpeed * (IsKeyPressed(::user::e_key_left_shift) ? 3.f : 1.f);
   //       transform.m_vec3Position += dir * speed * dt;
   //    }
   //
   //    // --- 5) Reset raw delta for next frame ---
   //    m_rawDelta = floating_sequence2(0.f);
   // }


} // namespace SceneFoundry_SceneFoundry
 