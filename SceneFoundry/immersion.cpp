//
// Created by camilo on 2025-08-23 03:27 <3ThomasBorregaardSorensen!!
//
#include "framework.h"
#include "application.h"
#include "immersion.h"
#include "main_scene.h"


namespace SceneFoundry_SceneFoundry
{


   immersion::immersion()
   {


   }


   immersion::~immersion()
   {


   }


   ::pointer<::graphics3d::scene> immersion::create_main_scene()
   {

      auto psceneMain = create_scene<main_scene>("main");

      return psceneMain;

   }


} // namespace SceneFoundry_SceneFoundry




