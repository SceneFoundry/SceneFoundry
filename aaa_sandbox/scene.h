// IScene.h
#pragma once

//#include <unordered_map>
//#include <memory>
//#include "SceneFoundry/graphics3d/game_object_i.h"
//#include "SceneFoundry/graphics3d/camera_i.h"
//#include "SceneFoundry/graphics3d/renderer_i.h"   // for FrameContext
//#include <optional>
//////#include <stdexcept>
///
///

namespace graphics3d
{

    class IScene  :
        virtual public ::particle
    {
    public:

        ///virtual ~IScene() = default;
        //

        virtual void init() = 0;


        virtual void update(float deltaTime) = 0;

        virtual ICamera & getCamera() = 0;

        virtual ::graphics3d::scene_object_map&  getGameObjects() = 0;

        // virtual std::optional<std::reference_wrapper<IGameObject>>
        //     getSkyboxObject() const
        // {
        //     return std::nullopt;
        // }

virtual        IGameObject * getSkyboxObject() = 0;

        virtual ::string getSkyboxCubemapName() const = 0;
    };


} // namespace graphics3d



