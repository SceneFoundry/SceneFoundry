#pragma once


#include "core/_.h"
#include "app-graphics3d/graphics3d/_.h"


#if defined(_SceneFoundry_pbr_renderer_project)
#define CLASS_DECL_SCENEFOUNDRY_PBR_RENDERER  CLASS_DECL_EXPORT
#else
#define CLASS_DECL_SCENEFOUNDRY_PBR_RENDERER  CLASS_DECL_IMPORT
#endif

/**
 * Actions produced by input depending on the mapping.
 */
enum class KeymapAction
{
   EXIT,
   ENABLE_MOUSE_CAMERA,
   DISABLE_MOUSE_CAMERA,

   // movement
   MOVE_FORWARD,
   MOVE_BACKWARD,
   MOVE_LEFT,
   MOVE_RIGHT,
   MOVE_UP,
   MOVE_DOWN
};

struct MousePosition
{
   double x;
   double y;
};





namespace SceneFoundry_pbr_renderer
{

   class application;
   class pane_impact;
   class document;
   class impact;


   class main_scene;
   class simple_render_system;
   class point_light_system;


   class GlobalUbo;

   class immersion;

   class InputManager;
   class CameraManager;
   class RenderManager;


} // namespace SceneFoundry_pbr_renderer



