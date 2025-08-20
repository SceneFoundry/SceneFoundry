// IGameObject.h
#pragma once
//#include "SceneFoundry/sandbox_interfaces/transform_component.h"
//#include "SceneFoundry/sandbox_interfaces/renderer_i.h"
//#include "SceneFoundry/sandbox_interfaces/model_i.h"
//#include <memory>
#include <optional>
#include <glm/vec3.hpp>


namespace sandbox_interfaces
{




    struct IGameObject  :
        virtual public ::particle
    {
        //    virtual ~IGameObject() = default;

        virtual void onInit() {}
        virtual void onUpdate(float deltaTime) {}

        virtual sandbox_game::TransformComponent& getTransform() = 0;
        virtual ::pointer<IModel> getModel() const = 0;

        virtual glm::vec3 getColor() const { return glm::vec3(1.f); }
        virtual const PointLightComponent* getPointLight() const { return nullptr; }
        virtual uint32_t getId() const { return 0; }
    };

} // namespace sandbox_interfaces


