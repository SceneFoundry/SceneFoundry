#include "framework.h"

#include "SceneFoundry/sandbox_renderer/include/vulkan_wrapper/render_systems/point_light_render_system.h"
#include "SceneFoundry/core_interfaces/include/frame_info.h"
#include "SceneFoundry/core_interfaces/include/interfaces/game_object_i.h"
// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/transform.hpp>
// std
//#include <array>
//#include <cassert>
//#include <map>
//#include <stdexcept>
//#include <cassert>


// Define light constants
struct PointLightPushConstants {
    glm::vec4 position{};
    glm::vec4 color{};
    float radius;
};

PointLightRS::PointLightRS(sandbox_renderer::sandbox_device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout)
    : m_device(device), m_globalSetLayout(globalSetLayout)
{

}
void PointLightRS::init(
   sandbox_renderer::sandbox_device& device,
    VkRenderPass renderPass,
    VkDescriptorSetLayout globalSetLayout,
   sandbox_renderer::sandbox_descriptor_pool& descriptorPool,
    size_t frameCount)
{
    // Optional: assert device consistency
    assert(&device == &m_device);

    createPipelineLayout(globalSetLayout);
    createPipeline(renderPass);
}

PointLightRS::~PointLightRS() {
    vkDestroyPipelineLayout(m_device.device(), m_pipelineLayout, nullptr);
}

void PointLightRS::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(PointLightPushConstants);

    ::array_base<VkDescriptorSetLayout> setLayouts = { globalSetLayout };

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(setLayouts.size());
    pipelineLayoutInfo.pSetLayouts = setLayouts.data();
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

    if (vkCreatePipelineLayout(m_device.device(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create PointLightRS pipeline layout!");
    }
}

void PointLightRS::createPipeline(VkRenderPass renderPass) {
    assert(m_pipelineLayout != VK_NULL_HANDLE && "Cannot create pipeline before pipeline layout");

    sandbox_renderer::pipeline_configuration_information pipelineConfig{};
    sandbox_renderer::sandbox_pipeline::defaultPipelineConfigInfo(pipelineConfig);
    pipelineConfig.bindingDescriptions.clear();
    pipelineConfig.attributeDescriptions.clear();
    pipelineConfig.renderPass = renderPass;
    pipelineConfig.pipelineLayout = m_pipelineLayout;

    ::string vertShaderPath = "matter://shaders/spirV/point_light.vert.spv";
    ::string fragShaderPath = "matter://shaders/spirV/point_light.frag.spv";

    m_pipeline = øallocate sandbox_pipeline(
        m_device,
        vertShaderPath.c_str(),
        fragShaderPath.c_str(),
        pipelineConfig
    );
}

void PointLightRS::render(FrameInfo& frame) {
    ::map<float, uint32_t> sorted;
    for (auto& [id, obj] : frame.gameObjects) {
        const auto* light = obj->getPointLight();
        if (!light) continue;
        glm::vec3 offset = frame.camera.getPosition() - obj->getTransform().translation;// need to implement getPosition because ICamera has no defintion
        float distanceSquared = glm::dot(offset, offset);
        sorted[distanceSquared] = obj->getId();
    }
    m_pipeline->bind(frame.commandBuffer);

    vkCmdBindDescriptorSets(
        frame.commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        m_pipelineLayout,
        0,
        1,
        &frame.globalDescriptorSet,
        0,
        nullptr
    );

    for (auto it = sorted.rbegin(); it != sorted.rend(); ++it) {
        auto& obj = frame.gameObjects.at(it ->second);
        const auto* light = obj->getPointLight();
        PointLightPushConstants push{};
        push.position = glm::vec4(obj->getTransform().translation, 1.0f);
        push.color = glm::vec4(obj->getColor(), light->lightIntensity);
        push.radius = obj->getTransform().scale.x;

        vkCmdPushConstants(
            frame.commandBuffer,
            m_pipelineLayout,
            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            0,
            sizeof(PointLightPushConstants),
            &push
        );

        vkCmdDraw(frame.commandBuffer, 6, 1, 0, 0);
    }

}void PointLightRS::update(FrameInfo& frame, GlobalUbo& ubo) {
    auto rotateLight = glm::rotate(glm::mat4(1.f), m_rotationSpeed * frame.frameTime, { 0.f, -1.f, 0.f });

    int lightIndex = 0;
    for (auto& [id, obj] : frame.gameObjects) {
        auto pointLight = obj->getPointLight();
        if (!pointLight)
            continue;

        assert(lightIndex < MAX_LIGHTS && "Point lights exceed maximum supported.");

        auto& transform = obj->getTransform();
        transform.translation = glm::vec3(rotateLight * glm::vec4(transform.translation, 1.f));

        ubo.pointLights[lightIndex].position = glm::vec4(transform.translation, 1.f);
        ubo.pointLights[lightIndex].color = glm::vec4(obj->getColor(), pointLight->lightIntensity);

        ++lightIndex;
    }

    ubo.numLights = lightIndex;
}