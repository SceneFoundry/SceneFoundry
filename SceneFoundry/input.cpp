// player_input.cpp
#include "framework.h"
#include "input.h"


namespace SceneFoundry_SceneFoundry
{


   SandboxMNKController::SandboxMNKController()
   {


   }


      SandboxMNKController::~SandboxMNKController() {}


   void SandboxMNKController::initialize_SandboxMNKController(float moveSpeed, float mouseSensitivity)
   {
      m_moveSpeed = moveSpeed;
      m_mouseSensitivity = mouseSensitivity;
      
      m_yaw = -90.f;
      
      m_pitch = 0.f ;
   }

   void SandboxMNKController::_001OnMouseMove(const ::int_point & point)
   {

      if (point != m_pointLast)
      {

         auto Δ = point - m_pointLast;

         mouseCallback({Δ.cx(), Δ.cy() });

      }
   }

   void SandboxMNKController::mouseCallback(glm::vec2 delta) { m_rawDelta = delta; };
   //void SandboxMNKController::update(
   // float dt, std::shared_ptr<IWindowInput> input,
      //TransformComponent &transform) 
      
      void SandboxMNKController::update(float dt, ::graphics3d::transform & transform)
   {
      if (dt <= 0.0f)
         return;

      // --- 1) Smooth raw mouse delta ---
      float alpha = 1.0f - std::exp(-m_smoothing * dt);
      m_smoothDelta += (m_rawDelta - m_smoothDelta) * alpha;

      // convert pixels -> degrees
      float deltaYaw = m_smoothDelta.x * m_mouseSensitivity;
      float deltaPitch = -m_smoothDelta.y * m_mouseSensitivity; // invert Y for typical FPS

      // --- 2) Update camera rotation ---
      m_yaw += deltaYaw;
      m_pitch += deltaPitch;
      m_pitch = glm::clamp(m_pitch, -89.f, 89.f);

      transform.m_vec3Rotation = glm::vec3(glm::radians(m_pitch), glm::radians(m_yaw), 0.f);

      // --- 3) Compute movement basis ---
      glm::vec3 front{std::cos(glm::radians(m_yaw)) * std::cos(glm::radians(m_pitch)), std::sin(glm::radians(m_pitch)),
                      std::sin(glm::radians(m_yaw)) * std::cos(glm::radians(m_pitch))};
      front = glm::normalize(front);
      glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0.f, 1.f, 0.f)));
      glm::vec3 up = glm::vec3(0.f, 1.f, 0.f);

      // --- 4) Apply WASDQE movement ---
      glm::vec3 dir{0.f};
      if (IsKeyPressed(::user::e_key_w))
         dir += front;
      if (IsKeyPressed(::user::e_key_s))
         dir -= front;
      if (IsKeyPressed(::user::e_key_a))
         dir -= right;
      if (IsKeyPressed(::user::e_key_d))
         dir += right;
      if (IsKeyPressed(::user::e_key_q))
         dir -= up;
      if (IsKeyPressed(::user::e_key_e))
         dir += up;

      if (glm::length(dir) > 1e-6f)
      {
         dir = glm::normalize(dir);
         float speed = m_moveSpeed * (IsKeyPressed(::user::e_key_left_shift) ? 3.f : 1.f);
         transform.m_vec3Translation += dir * speed * dt;
      }

      // --- 5) Reset raw delta for next frame ---
      m_rawDelta = glm::vec2(0.f);
   }


} // namespace SceneFoundry_SceneFoundry
 