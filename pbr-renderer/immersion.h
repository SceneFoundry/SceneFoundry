//
// Created by camilo on 2025-08-23 03:26 <3ThomasBorregaardSorensen!!
//
#pragma once


#include "bred/prodevian/immersion.h"


namespace SceneFoundry_pbr_renderer
{


   class CLASS_DECL_SCENEFOUNDRY_PBR_RENDERER immersion :
      virtual public ::prodevian::immersion
   {
   public:


      floating_sequence3 m_initialCameraPosition{0.f};
      floating_sequence3 m_initialCameraRotation{0.f};

      
      immersion();
      ~immersion() override;

      
      ::pointer<::graphics3d::scene_base> create_main_scene() override;


      void load_camera(const ::property_set &set) override;
      void on_initial_camera_load() override;

   };


} // namespace SceneFoundry_pbr_renderer



