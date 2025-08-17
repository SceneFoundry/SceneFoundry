#pragma once

#include "SceneFoundry/core_interfaces/include/interfaces/entity_i.h"
#include "SceneFoundry/core_interfaces/include/interfaces/model_i.h"
#include "SceneFoundry/sandbox_renderer/include/vulkan_wrapper/vulkan_obj.h"
#include <glm/gtc/matrix_transform.hpp>

// std
//#include <memory>
#include <unordered_map>
//////#include <stdexcept>


class SandboxGameObject : public IGameObject{
public:

    using id_t = unsigned int;
    using Map = ::map<id_t, SandboxGameObject>;

    static ::pointer<SandboxGameObject> createGameObject() {
        static id_t currentId = 0;
        return std::make_shared<SandboxGameObject>(currentId++);
    }

    static ::pointer<SandboxGameObject> makePointLight(float intensity = 10.f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.f)) {
        auto gameObj = SandboxGameObject::createGameObject();
        gameObj->m_color = color;
        gameObj->m_transform.scale.x = radius;
        gameObj->m_pointLight = std::make_unique<PointLightComponent>();
        gameObj->m_pointLight->lightIntensity = intensity;
        return gameObj;
    }

    SandboxGameObject(id_t objId);
    SandboxGameObject() = default;
    SandboxGameObject(const SandboxGameObject&) = delete;
    SandboxGameObject& operator=(const SandboxGameObject&) = delete;
    SandboxGameObject(SandboxGameObject&&) = default;
    SandboxGameObject& operator=(SandboxGameObject&&) = default;


    uint32_t getId() const override { return m_id; }

    glm::vec3 getColor() const override { return m_color; }

    PointLightComponent* getPointLight() const override {
        return m_pointLight.get();
    }

    TransformComponent& getTransform() override {
        return m_transform;
    }

    ::pointer<IModel> getModel() const override {
        return m_pModel;
    }

    void setModel(const ::pointer<IModel>& model) {
        m_pModel = model;
    }


    TransformComponent m_transform;
    ::pointer<IModel> m_pModel;
    glm::vec3 m_color{};
    bool m_bIsOBJ{ false };
    std::unique_ptr<PointLightComponent> m_pointLight = nullptr;

    ::string m_cubemapTextureName;
    id_t m_id;
    bool m_bIsSkybox = false;

};