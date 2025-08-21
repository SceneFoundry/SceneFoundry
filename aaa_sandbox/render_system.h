// sandbox_renderer/include/IRenderSystem.h
#pragma once

namespace graphics3d
{

    class IRenderSystem  :
        virtual public ::particle
    {
    public:


        virtual void on_prepare() = 0;
        virtual void on_update(IFrame * pframe) = 0;
        virtual void render(IFrame * pframe)  = 0;


    };



} // namespace graphics3d
