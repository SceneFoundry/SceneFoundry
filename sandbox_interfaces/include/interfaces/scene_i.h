// IScene.h
#pragma once

//#include <unordered_map>
//#include <memory>
#include "SceneFoundry/sandbox_interfaces/include/interfaces/game_object_i.h"
#include "SceneFoundry/sandbox_interfaces/include/interfaces/camera_i.h"
#include "SceneFoundry/sandbox_interfaces/include/interfaces/renderer_i.h"   // for FrameContext
#include <optional>
//////#include <stdexcept>

class ICamera;
struct IGameObject;

struct IScene  :
	virtual public ::particle
    {
    virtual ~IScene() = default;
//

    virtual void init() = 0;


    virtual void update(float deltaTime) = 0;
    
    virtual ICamera& getCamera() = 0;

    virtual ::map<unsigned int, ::pointer<IGameObject>>&
        getGameObjects() = 0;

    virtual std::optional<std::reference_wrapper<IGameObject>>
        getSkyboxObject() const
    {
        return std::nullopt;
    }

    virtual ::string getSkyboxCubemapName() const = 0;
};
