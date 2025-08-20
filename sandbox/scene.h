// IScene.h
#pragma once

//#include <unordered_map>
//#include <memory>
//#include "SceneFoundry/sandbox/game_object_i.h"
//#include "SceneFoundry/sandbox/camera_i.h"
//#include "SceneFoundry/sandbox/renderer_i.h"   // for FrameContext
//#include <optional>
//////#include <stdexcept>
///
///

namespace sandbox
{

    class IScene  :
        virtual public ::particle
    {
    public:

        ///virtual ~IScene() = default;
        //

        virtual void init() = 0;


        virtual void update(float deltaTime) = 0;

        virtual ICamera * getCamera() = 0;

        virtual ::map<unsigned int, ::pointer<IGameObject>>&  getGameObjects() = 0;

        // virtual std::optional<std::reference_wrapper<IGameObject>>
        //     getSkyboxObject() const
        // {
        //     return std::nullopt;
        // }

        IGameObject * getSkyboxObject() = 0;

        virtual ::string getSkyboxCubemapName() const = 0;
    };


} // namespace sandbox



