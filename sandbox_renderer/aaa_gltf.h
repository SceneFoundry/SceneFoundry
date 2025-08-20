#pragma once
#include <ktx.h>
#include <ktxvulkan.h>
#include "bred/gpu/types.h"

//#include <stdlib.h>
//////#include <stdexcept>
//#include <fstream>
//#include <vector>

#include <vulkan/vulkan.h>
#include "SceneFoundry/sandbox_interfaces/model_i.h"
#include "SceneFoundry/sandbox_renderer/include/vk_tools/vk_init.h"
#include "descriptor.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define TINYGLTF_NO_STB_IMAGE_WRITE
#ifdef VK_USE_PLATFORM_ANDROID_KHR
#define TINYGLTF_ANDROID_LOAD_FROM_ASSETS
#endif

#include "tiny_gltf.h"

namespace sandbox_renderer
{


	namespace gltf {

		enum DescriptorBindingFlags {
			ImageBaseColor = 0x00000001,
			ImageNormalMap = 0x00000002
		};

		extern VkDescriptorSetLayout descriptorSetLayoutImage;
		extern VkDescriptorSetLayout descriptorSetLayoutUbo;
		extern VkDescriptorSetLayout descriptorSetLayoutIbl;
		extern VkMemoryPropertyFlags memoryPropertyFlags;
		extern uint32_t descriptorBindingFlags;

		struct Node;

		/*
			glTF texture loading class
		*/
		struct Texture {

			device* pdevice = nullptr;
			VkImage image;
			VkImageLayout imageLayout;
			VkDeviceMemory deviceMemory;
			VkImageView view;
			uint32_t width, height;
			uint32_t mipLevels;
			uint32_t layerCount;
			VkDescriptorImageInfo descriptor;
			VkSampler sampler;
			uint32_t index;
			void updateDescriptor();
			void destroy();
			void fromglTfImage(tinygltf::Image& gltfimage, ::string path, device* pdevice, VkQueue copyQueue, bool isSrgb);
		};


		/*
			glTF material class
		*/
		struct Material {
			device* pdevice = nullptr;
			enum AlphaMode { ALPHAMODE_OPAQUE, ALPHAMODE_MASK, ALPHAMODE_BLEND };
			AlphaMode alphaMode = ALPHAMODE_OPAQUE;
			float alphaCutoff = 1.0f;
			float metallicFactor = 1.0f;
			float roughnessFactor = 1.0f;
			glm::vec4 baseColorFactor = glm::vec4(1.0f);
			gltf::Texture* baseColorTexture = nullptr;
			gltf::Texture* metallicRoughnessTexture = nullptr;
			gltf::Texture* normalTexture = nullptr;
			gltf::Texture* occlusionTexture = nullptr;
			gltf::Texture* emissiveTexture = nullptr;

			gltf::Texture* specularGlossinessTexture;
			gltf::Texture* diffuseTexture;

			VkDescriptorSet descriptorSet = VK_NULL_HANDLE;

			Material(device* pdevice) : pdevice(pdevice) {};
			void createDescriptorSet(VkDescriptorPool descriptorPool, VkDescriptorSetLayout descriptorSetLayout, uint32_t descriptorBindingFlags, Texture* fallbackTexture);
		};

		/*
			glTF primitive
		*/
		struct Primitive {
			uint32_t firstIndex;
			uint32_t indexCount;
			uint32_t firstVertex;
			uint32_t vertexCount;
			Material& material;

			struct Dimensions {
				glm::vec3 min = glm::vec3(FLT_MAX);
				glm::vec3 max = glm::vec3(-FLT_MAX);
				glm::vec3 size;
				glm::vec3 center;
				float radius;
			} dimensions;

			void setDimensions(glm::vec3 min, glm::vec3 max);
			Primitive(uint32_t firstIndex, uint32_t indexCount, Material& material) : firstIndex(firstIndex), indexCount(indexCount), material(material) {};
		};

		/*
			glTF mesh
		*/
		struct Mesh {
			device* pdevice;

			::array_base<Primitive*> primitives;
			::string name;

			struct UniformBuffer {
				VkBuffer buffer;
				VkDeviceMemory memory;
				VkDescriptorBufferInfo descriptor;
				VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
				void* mapped;
			} uniformBuffer;

			struct UniformBlock {
				glm::mat4 matrix;
				glm::mat4 jointMatrix[64]{};
				float jointcount{ 0 };
			} uniformBlock;

			Mesh(device* pdevice, glm::mat4 matrix);
			~Mesh();
		};

		/*
			glTF skin
		*/
		struct Skin {
			::string name;
			Node* skeletonRoot = nullptr;
			::array_base<glm::mat4> inverseBindMatrices;
			::array_base<Node*> joints;
		};

		/*
			glTF node
		*/
		struct Node {
			Node* parent;
			uint32_t index;
			::array_base<Node*> children;
			glm::mat4 matrix;
			::string name;
			Mesh* mesh;
			Skin* skin;
			int32_t skinIndex = -1;
			glm::vec3 translation{};
			glm::vec3 scale{ 1.0f };
			glm::quat rotation{};
			glm::mat4 localMatrix();
			glm::mat4 getMatrix();
			void update();
			~Node();
		};

		/*
			glTF animation channel
		*/
		struct AnimationChannel {
			enum PathType { TRANSLATION, ROTATION, SCALE };
			PathType path;
			Node* node;
			uint32_t samplerIndex;
		};

		/*
			glTF animation sampler
		*/
		struct AnimationSampler {
			enum InterpolationType { LINEAR, STEP, CUBICSPLINE };
			InterpolationType interpolation;
			::array_base<float> inputs;
			::array_base<glm::vec4> outputsVec4;
		};

		/*
			glTF animation
		*/
		struct Animation {
			::string name;
			::array_base<AnimationSampler> samplers;
			::array_base<AnimationChannel> channels;
			float start = std::numeric_limits<float>::max();
			float end = std::numeric_limits<float>::min();
		};

		/*
			glTF default vertex layout with easy Vulkan mapping functions
		*/
		enum class VertexComponent { Position, Normal, UV, Color, Tangent, Joint0, Weight0 };

		struct Vertex {
			glm::vec3 pos;
			glm::vec3 normal;
			glm::vec2 uv;
			glm::vec4 color;
			glm::vec4 joint0;
			glm::vec4 weight0;
			glm::vec4 tangent;
			static VkVertexInputBindingDescription vertexInputBindingDescription;
			static ::array_base<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions;
			static VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo;
			static VkVertexInputBindingDescription inputBindingDescription(uint32_t binding);
			static VkVertexInputAttributeDescription inputAttributeDescription(uint32_t binding, uint32_t location, VertexComponent component);
			static ::array_base<VkVertexInputAttributeDescription> inputAttributeDescriptions(uint32_t binding, const ::array_base<VertexComponent> components);
			/** @brief Returns the default pipeline vertex input state create info structure for the requested vertex components */
			static VkPipelineVertexInputStateCreateInfo* getPipelineVertexInputState(const ::array_base<VertexComponent> components);
		};

		enum FileLoadingFlags {
			None = 0x00000000,
			PreTransformVertices = 0x00000001,
			PreMultiplyVertexColors = 0x00000002,
			FlipY = 0x00000004,
			DontLoadImages = 0x00000008
		};

		enum RenderFlags {
			BindImages = 0x00000001,
			RenderOpaqueNodes = 0x00000002,
			RenderAlphaMaskedNodes = 0x00000004,
			RenderAlphaBlendedNodes = 0x00000008
		};

		/*
			glTF model loading and rendering class
		*/
		class Model : public IModel {
		private:
			gltf::Texture* getTexture(uint32_t index);
			gltf::Texture emptyTexture;
			void createEmptyTexture(VkQueue transferQueue);
		public:
			device* m_pDevice;
			VkDescriptorPool m_descriptorPool;

			struct Vertices {
				int count;
				VkBuffer buffer;
				VkDeviceMemory memory;
			} vertexes;
			struct Indices {
				int count;
				VkBuffer buffer;
				VkDeviceMemory memory;
			} indexes;

			::array_base<Node*> m_nodes;
			::array_base<Node*> m_linearNodes;

			::array_base<Skin*> m_skins;


			::array_base<gltf::Texture> m_textures;
			::array_base<Material> m_materials;
			::array_base<Animation> m_animations;

			struct Dimensions {
				glm::vec3 min = glm::vec3(FLT_MAX);
				glm::vec3 max = glm::vec3(-FLT_MAX);
				glm::vec3 size;
				glm::vec3 center;
				float radius;
			} dimensions;

			bool m_bMetallicRoughnessWorkflow = true;
			bool m_bBuffersBound = false;
			::string m_path;

			Model() {};
			~Model();
			void loadNode(gltf::Node* parent, const tinygltf::Node& node, uint32_t nodeIndex, const tinygltf::Model& model, ::array_base<uint32_t>& indexBuffer, ::array_base<Vertex>& vertexBuffer, float globalscale);
			void loadSkins(tinygltf::Model& gltfModel);
			void loadImages(tinygltf::Model& gltfModel, device* pdevice, VkQueue transferQueue);
			void loadMaterials(tinygltf::Model& gltfModel);
			void loadAnimations(tinygltf::Model& gltfModel);

			void loadFromFile(::string filename, device* pdevice, VkQueue transferQueue, uint32_t fileLoadingFlags = FileLoadingFlags::None, float scale = 1.0f);

			void bind(::gpu::command_buffer * pcommandbuffer)override;

			void gltfDraw(
				VkCommandBuffer cmd,
				uint32_t renderFlags = 0,
				VkPipelineLayout pipelineLayout = VK_NULL_HANDLE,
				uint32_t bindImageSet = 1
			)override;

			void drawNode(Node* node, VkCommandBuffer commandBuffer, uint32_t renderFlags = 0, VkPipelineLayout pipelineLayout = VK_NULL_HANDLE, uint32_t bindImageSet = 1);


			void getNodeDimensions(Node* node, glm::vec3& min, glm::vec3& max);
			void getSceneDimensions();
			void updateAnimation(uint32_t index, float time);
			Node* findNode(Node* parent, uint32_t index);
			Node* nodeFromIndex(uint32_t index);
			void prepareNodeDescriptor(gltf::Node* node, VkDescriptorSetLayout descriptorSetLayout);
		};

	}


} // namespace sandbox_renderer



template < >
inline ::hash32 as_hash32<::sandbox_renderer::gltf::Vertex>(
	const ::sandbox_renderer::gltf::Vertex& vertex)
{

	hash32 seed{};

	::gpu::hash_combine(
		seed,
		vertex.pos,
		vertex.normal,
		vertex.uv,
		vertex.color,
		vertex.joint0,
		vertex.weight0,
		vertex.tangent);

	return seed;

}
