#pragma once
#include "SceneFoundry/sandbox_renderer/include/vulkan_wrapper/vulkan_device.h"
#include "SceneFoundry/core_interfaces/include/interfaces/model_i.h"
#include "SceneFoundry/sandbox_renderer/include/vulkan_wrapper/vulkan_buffer.h"
#include "SceneFoundry/core_interfaces/include/interfaces/renderer_i.h"

// libs
#define GLM_FORCE_RADIANS	
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// std
//#include <memory>
//#include <vector>
#include <unordered_map>
//#include <cassert>
//#include <cstring>
//#include <stdexcept>
#include <unordered_map>
//#include <unordered_set>

class VkSandboxOBJmodel : public IModel {
public:
    struct Vertex {
        glm::vec3 position{};
        glm::vec3 color{};
        glm::vec3 normal{};
        glm::vec2 uv{};

        static ::array_base<VkVertexInputBindingDescription> getBindingDescriptions();
        static ::array_base<VkVertexInputAttributeDescription> getAttributeDescriptions();

        bool operator==(Vertex const& o) const {
            return position == o.position &&
                normal == o.normal &&
                uv == o.uv;
        }
    };

    struct SkyboxVertex {
        glm::vec3 position;
        static ::array_base<VkVertexInputBindingDescription>   getBindingDescriptions();
        static ::array_base<VkVertexInputAttributeDescription> getAttributeDescriptions();
    };
    struct PrimitiveInfo {
        uint32_t      firstIndex;
        uint32_t      indexCount;
        int           textureIndex; // index into textures[]
        glm::mat4     modelMatrix;
    };
    struct Builder {
        ::array_base<Vertex> vertices{};
        ::array_base<uint32_t> indices{};

        void loadModel(const ::scoped_string & filepath, bool isSkybox);


        bool isSkybox{ false };

    };

    static ::pointer<VkSandboxOBJmodel> createModelFromFile(
        sandbox_device& device, ::string const& filepath, bool isSkybox = false);

    VkSandboxOBJmodel(sandbox_device& device, Builder const& builder);
    ~VkSandboxOBJmodel();

    VkSandboxOBJmodel(VkSandboxOBJmodel const&) = delete;
    void operator=(VkSandboxOBJmodel const&) = delete;

    void bind(VkCommandBuffer cmd)override;
    void draw(VkCommandBuffer cmd)override;




    bool isSkybox() const { return m_bIsSkyboxModel; }
private:
    void createVertexBuffers(::array_base<Vertex> const& vertices);
    void createIndexBuffers(::array_base<uint32_t> const& indices);

    sandbox_device& m_device;
    bool m_bHasIndexBuffer{ false };
    bool m_bIsSkyboxModel{ false };

    std::unique_ptr<VkSandboxBuffer> m_vertexBuffer;
    uint32_t m_vertexCount;

    std::unique_ptr<VkSandboxBuffer> m_indexBuffer;
    uint32_t m_indexCount;


};