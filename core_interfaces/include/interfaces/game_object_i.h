// IGameObject.h
#pragma once
#include "SceneFoundry/core_interfaces/include/transform_component.h"
#include "SceneFoundry/core_interfaces/include/interfaces/renderer_i.h"
#include "SceneFoundry/core_interfaces/include/interfaces/model_i.h"
//#include <memory>
#include <optional>
#include <glm/vec3.hpp>

struct IModel;


struct PointLightComponent :
   virtual public ::particle
{
    float lightIntensity = 1.0f;
};


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
