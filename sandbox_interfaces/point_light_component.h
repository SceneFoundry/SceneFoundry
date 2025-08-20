//
// From point_light_render_system by camilo on 2025-08-19 <3ThomasBorregaardSorensen!!
//
#pragma once


namespace sandbox_interfaces
{
    struct point_light_component :
       virtual public ::particle
    {
        float lightIntensity = 1.0f;
    };


} // namespace sandbox_interfaces
