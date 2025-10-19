#pragma once

//#include <map>
//#include <set>
//#include <memory>
//
//#include "glfw3.h"

//#include "engineconfig.h"
//#include "windowmanager.h"
#include "acme/constant/user_key.h"

namespace SceneFoundry_pbr_renderer
{

   // class WindowManager;

   /**
    * Manages mouse/keyboard.
    */
   class CLASS_DECL_SCENEFOUNDRY_PBR_RENDERER InputManager : virtual public ::particle
   {
   public:

      //::pointer<EngineConfig> m_pengineconfig;
      ::pointer<::user::graphics3d> m_pusergraphics3d;

      ::map<::user::enum_key, KeymapAction> mKeymap = {{::user::e_key_escape, KeymapAction::EXIT},
                                                       {::user::e_key_1, KeymapAction::ENABLE_MOUSE_CAMERA},
                                                       {::user::e_key_2, KeymapAction::DISABLE_MOUSE_CAMERA},
                                                       {::user::e_key_w, KeymapAction::MOVE_FORWARD},
                                                       {::user::e_key_a, KeymapAction::MOVE_LEFT},
                                                       {::user::e_key_s, KeymapAction::MOVE_BACKWARD},
                                                       {::user::e_key_d, KeymapAction::MOVE_RIGHT},
                                                       {::user::e_key_space, KeymapAction::MOVE_UP},
                                                       {::user::e_key_left_control, KeymapAction::MOVE_DOWN}};

      MousePosition mMousePosition;
      ::set<KeymapAction> mCurrentActions;


      InputManager();
      ~InputManager() override;

      //void initialize_InputManager(EngineConfig &engineConfig, ::user::graphics3d *pusergraphics3d);
      void initialize_InputManager(::user::graphics3d *pusergraphics3d);
      void startup();
      void update();
      void enableGuiMouse();
      void disableGuiMouse();
      void showMouseCursor();
      void hideMouseCursor();
      const ::set<KeymapAction> &getCurrentActions();
      const MousePosition &getMousePosition();
      // private:
   };

} // namespace SceneFoundry_pbr_renderer


