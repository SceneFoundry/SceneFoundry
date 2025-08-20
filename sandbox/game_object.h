// IGameObject.h
#pragma once
#include "SceneFoundry/sandbox/_types.h"
//#include "SceneFoundry/sandbox/renderer_i.h"
//#include "SceneFoundry/sandbox/model_i.h"
//#include <memory>
#include <optional>
#include <glm/vec3.hpp>


namespace sandbox
{


    class IGameObject  :
        virtual public ::particle
    {
    public:
        //    virtual ~IGameObject() = default;

        virtual void onInit() {}
        virtual void onUpdate(float deltaTime) {}

        virtual ::sandbox::TransformComponent& getTransform() = 0;
        virtual IModel * getModel() const = 0;

        virtual glm::vec3 getColor() const { return glm::vec3(1.f); }
        virtual const ::sandbox::point_light_component  * getPointLight() const {return nullptr;}
        virtual long long getId() const { return 0; }

    };


    using game_object_map = ::map<long long, IGameObject>;


} // namespace sandbox


