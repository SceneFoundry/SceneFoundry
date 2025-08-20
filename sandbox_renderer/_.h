#pragma once


#include "bred/_.h"
#include "SceneFoundry/sandbox/_.h"


#if defined(_sandbox_renderer_project)
#define CLASS_DECL_SANDBOX_RENDERER  CLASS_DECL_EXPORT
#else
#define CLASS_DECL_SANDBOX_RENDERER  CLASS_DECL_IMPORT
#endif
//
//
//namespace app_graphics3d_continuum
//{
//
//   class application;
//   class pane_impact;
//   class document;
//   class impact;
//
//
//   class main_scene;
//   class simple_render_system;
//   class point_light_system;
//
//
//   class GlobalUbo;
//
//
//} // namespace flag
//

//s#include "include/vulkan_wrapper/_.h"



namespace sandbox_renderer
{


    class device;
    class pipeline;
    class texture;


} // namespace sandbox_renderer



