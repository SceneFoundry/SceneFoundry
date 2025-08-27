//
// Created by camilo on 2025-08-23 03:27 <3ThomasBorregaardSorensen!!
//
#include "framework.h"
#include "immersion.h"
#include "application.h"
#include "camera.h"
#include "input.h"
#include "main_scene.h"


namespace SceneFoundry_SceneFoundry
{


   immersion::immersion() {}


   immersion::~immersion() {}


   ::pointer<::graphics3d::scene> immersion::create_main_scene()
   {

      auto pinput = øcreate_new < ::SceneFoundry_SceneFoundry::SandboxMNKController >();


      pinput->initialize_SandboxMNKController();
      //pinput->m_fMoveSpeed = 3.f;
      //pinput->m_fLookSpeed = 1.5f;

      m_pengine->m_pinput = pinput;

      // m_pengine->m_pinput->m_pusergraphics3d = m_pengine->m_pusergraphics3d;
      pinput->m_pengine = m_pengine;


      auto psceneMain = create_scene<main_scene>("main");

      return psceneMain;
   }


   void immersion::load_camera(const ::property_set &set)
   {

      auto pos = set.get("position", ::float_array_base{0.f, 0.f, 0.f});
      auto rot = set.get("rotation", ::float_array_base{0.f, 0.f, 0.f});

      m_initialCameraPosition = {pos[0], pos[1], pos[2]};
      m_initialCameraRotation = {glm::radians(rot[0]), glm::radians(rot[1]), glm::radians(rot[2])};

      m_pscene->m_bInitialCameraLoaded = true;

      information("Camera position: ({}, {}, {}), rotation (deg): ({}, {}, {})", pos[0], pos[1], pos[2], rot[0], rot[1],
                  rot[2]);
   }


   void immersion::on_initial_camera_load()
   {


      auto pcameraLoaded = øcreate_new<SandboxCamera>();

      //pcameraLoaded->initialize_SandboxCamera(glm::vec3(0.f, 0.f, 3.f));

      pcameraLoaded->m_pengine = m_pengine;

      //         // glm::vec3 camera = glm::vec3(0.0f, 1.0f *m_pengine->m_fYScale, 3.0f);
      //glm::vec3 camera = glm::vec3(0.0f, 1.0f, 3.0f);
      //glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f); // Look at origin
      //// glm::vec3 direction = glm::normalize(target - cameraPos);
      //// camera camera{ glm::vec3(0.0f, 2.0f, -15.0f), -90.0f, 0.0f };
      //auto pcameraDefault = øcreate<SandboxCamera>();
      //pcameraDefault->m_pengine = m_pimmersionlayer->m_pengine;
      
      // pcamera->m_pimpact = m_pimpact;

      

      // pcameraDefault->m_pengine = m_pimmersionlayer->m_pengine;

      //pcameraLoaded->m_position = m_initialCameraPosition;

      //pcameraLoaded->m_pitch = m_initialCameraRotation.x;

      //pcameraLoaded->m_yaw = m_initialCameraRotation.y;
      pcameraLoaded->initialize_SandboxCamera(
         m_initialCameraPosition, 
         m_initialCameraRotation.y, 
         m_initialCameraRotation.x);

      m_pscene->set_default_camera(pcameraLoaded);
      

   }

} // namespace SceneFoundry_SceneFoundry
