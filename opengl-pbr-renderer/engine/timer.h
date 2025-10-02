#pragma once

#include <chrono>
//#include <iostream>

namespace SceneFoundry_opengl_pbr_renderer
{
   /**
    * Class for timing anything.
    */
   class CLASS_DECL_SCENEFOUNDRY_OPENGL_PBR_RENDERER Timer
   {
   public:
      Timer();
      void logDifference(const ::scoped_string &logText);

   //private:
      std::chrono::steady_clock::time_point start;
   };


} // namespace SceneFoundry_opengl_pbr_renderer



