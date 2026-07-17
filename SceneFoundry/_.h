#pragma once


#include "core/_.h"
#include "app-graphics3d/graphics3d/_.h"


#if defined(_SceneFoundry_SceneFoundry_project)
#define CLASS_DECL_SCENEFOUNDRY_SCENEFOUNDRY  CLASS_DECL_EXPORT
#else
#define CLASS_DECL_SCENEFOUNDRY_SCENEFOUNDRY  CLASS_DECL_IMPORT
#endif


namespace SceneFoundry_SceneFoundry
{

   CLASS_DECL_SCENEFOUNDRY_SCENEFOUNDRY ::pointer_array < ::string_array >
      parse_comma_separated_file(const ::scoped_string & scopedstrCsv);

   class application;
   class pane_impact;
   class document;
   class impact;


   class main_scene;
   class simple_render_system;
   class point_light_system;


   class GlobalUbo;

   class immersion;


} // namespace SceneFoundry_SceneFoundry



