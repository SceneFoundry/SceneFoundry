//
// Created by camilo on 2025-08-23 03:26 <3ThomasBorregaardSorensen!!
//
#pragma once


#include "bred/prodevian/immersion.h"


namespace SceneFoundry_SceneFoundry
{


   class CLASS_DECL_SCENEFOUNDRY_SCENEFOUNDRY immersion : 
      virtual public ::prodevian::immersion
   {
   public:
      
      
      immersion();
      ~immersion() override;

      
      ::pointer<::graphics3d::scene> create_main_scene() override;


   };


} // namespace SceneFoundry_SceneFoundry



