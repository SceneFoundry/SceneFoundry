#include "framework.h"
#include "inputmanager.h"
#include "acme/platform/session.h"
//#include "aura/user/user/keyboard.h"
//#include "aura/user/user/user.h"
#include "bred/user/user/graphics3d.h"
//#include "glfw3.h"
//#include <imgui/imgui.h>

//#include "engineconfig.h"

namespace SceneFoundry_pbr_renderer
{

   InputManager::InputManager()
   {


   }

   InputManager::~InputManager()
   {


   }


   //void InputManager::initialize_InputManager(EngineConfig &engineConfig, ::pointer<WindowManager> windowManager) :
   void InputManager::initialize_InputManager(::user::graphics3d * pusergraphics3d)
       //m_pengineconfig(engineConfig)
   //, mWindowManager(windowManager)
   {

      m_pusergraphics3d = pusergraphics3d;

   }

   void InputManager::startup() {}

   void InputManager::update()
   {
      mCurrentActions.clear(); // clear any actions from last frame

      // glfwPollEvents();
      // glfwGetCursorPos(mWindowManager->getWindow(), &mMousePosition.x, &mMousePosition.y);

      for (auto pair: mKeymap)
      {
         auto key = pair.element1();
         //        if (glfwGetKey(mWindowManager->getWindow(), key) == GLFW_PRESS) {
         if (session()->is_key_pressed(key))
         {
            mCurrentActions.set_key(pair.element2());
         }
      }
   }

   void InputManager::enableGuiMouse()
   {
      //ImGuiIO &io = ImGui::GetIO();
      //io.ConfigFlags = io.ConfigFlags & !ImGuiConfigFlags_NoMouse;
   }

   void InputManager::disableGuiMouse()
   {
      //ImGuiIO &io = ImGui::GetIO();
      //io.ConfigFlags = io.ConfigFlags | ImGuiConfigFlags_NoMouse;
   }

   void InputManager::showMouseCursor()
   {
      //glfwSetInputMode(mWindowManager->getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
   }

   void InputManager::hideMouseCursor()
   {
      //glfwSetInputMode(mWindowManager->getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
   };

   const ::set<KeymapAction> &InputManager::getCurrentActions() { return mCurrentActions; }

   const MousePosition &InputManager::getMousePosition() { return mMousePosition; }


} // namespace SceneFoundry_pbr_renderer
