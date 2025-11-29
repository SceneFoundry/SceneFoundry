#include "framework.h"
#include "main_frame.h"
#include "acme/constant/message.h"
#include "acme/platform/application.h"

namespace SceneFoundry_pbr_renderer
{


   main_frame::main_frame()
   {

      window_enable_full_screen();

      m_etranslucencyFrame = ::user::e_translucency_present;

   }


   main_frame::~main_frame()
   {

   }


   ::pointer < ::experience::frame > main_frame::frame_experience()
   {

      auto pframe = ::simple_frame_window::frame_experience(nullptr, "022", "LightBlue");

      return pframe;

   }


   void main_frame::install_message_routing(::channel * pchannel)
   {

      ::simple_frame_window::install_message_routing(pchannel);

      USER_MESSAGE_LINK(::user::e_message_create, pchannel, this, &main_frame::on_message_create);

   }


   void main_frame::on_message_create(::message::message * pmessage)
   {

      //set_bitmap_source("Simple papp!!");

      set_timer(100100, 1_s,
                [this]()
                {

            ::string strTitle;
                   strTitle = "pbr-renderer";

                   strTitle += " draw2d_" + system()->component_factory_implementation_name("draw2d");
                   strTitle += " gpu_" + system()->component_factory_implementation_name("gpu");

                   set_window_text(strTitle);
                });

   }


} // namespace SceneFoundry_pbr_renderer



