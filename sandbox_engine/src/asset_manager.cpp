#include "framework.h"
// asset_manager.cpp
#include "acme/filesystem/filesystem/file_context.h"
#include "SceneFoundry/sandbox_engine/include/asset_manager.h"
#include <chrono>
//#include <json.hpp>
////#include <fstream>
////#include <spdlog/spdlog.h>
//#include <glm/glm.hpp>

//using json = nlohmann::json;

namespace sandbox_engine
{

   asset_manager::asset_manager(sandbox_renderer::sandbox_device* pdevice) :
      m_pdevice(pdevice), m_transferQueue(m_pdevice->graphicsQueue()) {

   }

   asset_manager::~asset_manager()
   {

   }
   // asset_manager.cpp

   void asset_manager::preloadGlobalAssets() {
      // Read model & cubemap list JSON
      auto modelJson = file()->as_network_payload("matter://scene_assets/default_scene_assets.json");
      //if (!in.is_open()) {
      //    throw std::runtime_error("Failed to open model list JSON.");
      //}

      //::property_set modelJson;
      //modelJson.parse_network_payload()

      // 1) Load models first (unchanged from your flow)

      auto& payloada = modelJson["models"].payload_array_reference();
      for (const auto& element : payloada)
      {

         auto& entry = element.property_set_reference();
         const ::string name = entry["name"];
         const ::string type = entry.payload("type", "obj");
         const ::string path = "matter://models/" + entry["path"].as_file_path();

         try {
            if (type == "obj") {
               auto model = loadObjModel(name, path, false);
               information("[asset_manager] Successfully loaded OBJ model '{}' from '{}'", name, path);
            }
            else if (type == "gltf") {
               uint32_t flags = entry.payload("flags", 0);  // Optional flags
               float scale = entry.payload("scale", 1.0f);  // Optional scale
               auto model = loadGLTFmodel(name, path, flags, scale);
               if (entry.payload("usage", "") == "skybox" || name == "cube") {
                  m_skyboxModel = model;
               }
               information("[asset_manager] Successfully loaded glTF model '{}' from '{}'", name, path);
            }
            else {
               warning("[asset_manager] Unknown model type '{}' for asset '{}'", type, name);
            }
         }
         catch (const ::exception& e) {
            error("[asset_manager] Failed to load model '{}': {}", name, e.get_message());
         }
      }

      // 2) Load cubemaps *before* generating BRDF / irradiance / prefiltered maps
      // Keep track of whether we loaded the environment cubemap used for IBL
      ::pointer<sandbox_renderer::sandbox_texture> loadedEnvironmentCubemap = nullptr;

      auto& payloada2 = modelJson["v"].payload_array_reference();

      for (const auto& element : payloada2) {
         auto& entry = element.property_set_reference();
         const ::string name = entry["name"];
         const ::string path = "matter://res/textures/" + entry["path"].as_file_path();

         // Map format string (if present) to VkFormat; default to R32G32B32A32_SFLOAT
         ::string fmtStr = entry.payload("format", "VK_FORMAT_R32G32B32A32_SFLOAT");
         VkFormat format = VK_FORMAT_R32G32B32A32_SFLOAT;
         if (fmtStr == "VK_FORMAT_R16G16B16A16_SFLOAT") format = VK_FORMAT_R16G16B16A16_SFLOAT;
         else if (fmtStr == "VK_FORMAT_R32G32B32A32_SFLOAT") format = VK_FORMAT_R32G32B32A32_SFLOAT;
         // Add more mappings here if you expect other formats

         try {
            // loadCubemap() must return ::pointer<sandbox_texture>
            auto cubemap = loadCubemap(
               name,
               path,
               format,
               VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
               VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
            );

            if (!cubemap) {
               error("[asset_manager] loadCubemap returned nullptr for '{}'", name);
               continue;
            }

            // Register into your caches (optional helper)
            registerTextureIfNeeded(name, cubemap, m_textures, m_textureIndexMap, m_textureList);

            information("[asset_manager] Successfully loaded cubemap '{}' from '{}'", name, path);

            // If this cubemap is the environment (skybox_hdr per your JSON), remember it.
            // Use the name you expect in your code / JSON. I see "skybox_hdr" in your example JSON.
            if (name == "skybox_hdr" || entry.payload("environment", false)) {
               // prefer explicit "environment" : true in JSON or name match
               loadedEnvironmentCubemap = cubemap;
            }
         }
         catch (const ::exception& e) {
            error("[asset_manager] Failed to load cubemap '{}': {}", name, e.get_message());
         }
      }

      // 3) Create/generate BRDF LUT and IBL assets — ensure environmentCube points to the loaded cubemap
      // If your JSON didn't mark which cubemap is the environment, we fallback to looking up "skybox_hdr"
      if (!loadedEnvironmentCubemap) {
         auto it = m_textures.find("skybox_hdr");
         if (it)
         {
            loadedEnvironmentCubemap = it->element2();
         }
      }

      if (!loadedEnvironmentCubemap) {
         warning("[asset_manager] No environment cubemap found (expected 'skybox_hdr' or 'environment':true). Using placeholder/empty environmentCube.");
         // Optionally: throw or create a debug 1x1 texture so validation doesn't fail.
         // For now we will not create an invalid shared_ptr (keeps previous behavior safer).
      }
      else {
         environmentCube = loadedEnvironmentCubemap;
      }

      // Create BRDF LUT, irradianceCube, prefilteredCube structures (these should allocate their own images)
      // Note: remove any line that reassigns environmentCube to an empty sandbox_texture (that was the bug)
      lutBrdf = øcreate_pointer<sandbox_renderer::sandbox_texture>(&m_pdevice);
      irradianceCube = øcreate_pointer<sandbox_renderer::sandbox_texture>(&m_pdevice);
      prefilteredCube = øcreate_pointer<sandbox_renderer::sandbox_texture>(&m_pdevice);

      // Generate BRDF LUT first (your existing function)
      generateBRDFlut();

      // Now generate irradiance and prefiltered maps using environmentCube (must be valid)
      if (!environmentCube) {
         error("[asset_manager] environmentCube is null - aborting IBL generation to avoid descriptor errors.");
      }
      else {
         try {
            generateIrradianceMap();
            generatePrefilteredEnvMap();
            information("[asset_manager] IBL assets generated successfully.");
         }
         catch (const ::exception& e) {
            error("[asset_manager] IBL generation failed: {}", e.get_message());
         }
      }

      information("Assets loaded");
   }


   void asset_manager::generateIrradianceMap() {
      auto tStart = std::chrono::high_resolution_clock::now();

      const VkFormat format = VK_FORMAT_R32G32B32A32_SFLOAT;
      const int32_t dim = 64;
      const uint32_t numMips = static_cast<uint32_t>(floor(log2(dim))) + 1;

      // create irradiance cubemap (same as before)
      VkImageCreateInfo imageCI = vkinit::imageCreateInfo();
      imageCI.imageType = VK_IMAGE_TYPE_2D;
      imageCI.format = format;
      imageCI.extent.width = dim;
      imageCI.extent.height = dim;
      imageCI.extent.depth = 1;
      imageCI.mipLevels = numMips;
      imageCI.arrayLayers = 6;
      imageCI.samples = VK_SAMPLE_COUNT_1_BIT;
      imageCI.tiling = VK_IMAGE_TILING_OPTIMAL;
      imageCI.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
      imageCI.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
      VK_CHECK_RESULT(vkCreateImage(m_pdevice->device(), &imageCI, nullptr, &irradianceCube->m_image));

      VkMemoryRequirements memReqs;
      vkGetImageMemoryRequirements(m_pdevice->device(), irradianceCube->m_image, &memReqs);
      VkMemoryAllocateInfo memAlloc = vkinit::memoryAllocateInfo();
      memAlloc.allocationSize = memReqs.size;
      memAlloc.memoryTypeIndex = m_pdevice->getMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
      VK_CHECK_RESULT(vkAllocateMemory(m_pdevice->device(), &memAlloc, nullptr, &irradianceCube->m_deviceMemory));
      VK_CHECK_RESULT(vkBindImageMemory(m_pdevice->device(), irradianceCube->m_image, irradianceCube->m_deviceMemory, 0));

      // view & sampler
      VkImageViewCreateInfo viewCI = vkinit::imageViewCreateInfo();
      viewCI.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
      viewCI.format = format;
      viewCI.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      viewCI.subresourceRange.baseMipLevel = 0;
      viewCI.subresourceRange.levelCount = numMips;
      viewCI.subresourceRange.baseArrayLayer = 0;
      viewCI.subresourceRange.layerCount = 6;
      viewCI.image = irradianceCube->m_image;
      VK_CHECK_RESULT(vkCreateImageView(m_pdevice->device(), &viewCI, nullptr, &irradianceCube->m_view));

      VkSamplerCreateInfo samplerCI = vkinit::samplerCreateInfo();
      samplerCI.magFilter = VK_FILTER_LINEAR;
      samplerCI.minFilter = VK_FILTER_LINEAR;
      samplerCI.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
      samplerCI.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
      samplerCI.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
      samplerCI.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
      samplerCI.minLod = 0.0f;
      samplerCI.maxLod = static_cast<float>(numMips);
      samplerCI.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
      VK_CHECK_RESULT(vkCreateSampler(m_pdevice->device(), &samplerCI, nullptr, &irradianceCube->m_sampler));

      irradianceCube->m_descriptor.imageView = irradianceCube->m_view;
      irradianceCube->m_descriptor.sampler = irradianceCube->m_sampler;
      irradianceCube->m_descriptor.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      irradianceCube->m_pDevice = &m_pdevice;

      // --- create offscreen renderpass/framebuffer (unchanged) ---
      VkAttachmentDescription attDesc = {};
      attDesc.format = format;
      attDesc.samples = VK_SAMPLE_COUNT_1_BIT;
      attDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
      attDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
      attDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
      attDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
      attDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      attDesc.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
      VkAttachmentReference colorReference = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };

      VkSubpassDescription subpassDescription = {};
      subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
      subpassDescription.colorAttachmentCount = 1;
      subpassDescription.pColorAttachments = &colorReference;

      ::preallocated_array_base< ::array_base <VkSubpassDependency, 2> > dependencies;
      dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
      dependencies[0].dstSubpass = 0;
      dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
      dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
      dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
      dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
      dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
      dependencies[1].srcSubpass = 0;
      dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
      dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
      dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
      dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
      dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
      dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

      VkRenderPassCreateInfo renderPassCI = vkinit::renderPassCreateInfo();
      renderPassCI.attachmentCount = 1;
      renderPassCI.pAttachments = &attDesc;
      renderPassCI.subpassCount = 1;
      renderPassCI.pSubpasses = &subpassDescription;
      renderPassCI.dependencyCount = static_cast<uint32_t>(dependencies.size());
      renderPassCI.pDependencies = dependencies.data();
      VkRenderPass renderpass;
      VK_CHECK_RESULT(vkCreateRenderPass(m_pdevice->device(), &renderPassCI, nullptr, &renderpass));

      // offscreen color image (1 mip, reused for all mips/faces)
      struct {
         VkImage image;
         VkImageView view;
         VkDeviceMemory memory;
         VkFramebuffer framebuffer;
      } offscreen;

      {
         VkImageCreateInfo imageCreateInfo = vkinit::imageCreateInfo();
         imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
         imageCreateInfo.format = format;
         imageCreateInfo.extent.width = dim;
         imageCreateInfo.extent.height = dim;
         imageCreateInfo.extent.depth = 1;
         imageCreateInfo.mipLevels = 1;
         imageCreateInfo.arrayLayers = 1;
         imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
         imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
         imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
         imageCreateInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
         imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
         VK_CHECK_RESULT(vkCreateImage(m_pdevice->device(), &imageCreateInfo, nullptr, &offscreen.image));

         vkGetImageMemoryRequirements(m_pdevice->device(), offscreen.image, &memReqs);
         memAlloc.allocationSize = memReqs.size;
         memAlloc.memoryTypeIndex = m_pdevice->getMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
         VK_CHECK_RESULT(vkAllocateMemory(m_pdevice->device(), &memAlloc, nullptr, &offscreen.memory));
         VK_CHECK_RESULT(vkBindImageMemory(m_pdevice->device(), offscreen.image, offscreen.memory, 0));

         VkImageViewCreateInfo colorImageView = vkinit::imageViewCreateInfo();
         colorImageView.viewType = VK_IMAGE_VIEW_TYPE_2D;
         colorImageView.format = format;
         colorImageView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
         colorImageView.subresourceRange.baseMipLevel = 0;
         colorImageView.subresourceRange.levelCount = 1;
         colorImageView.subresourceRange.baseArrayLayer = 0;
         colorImageView.subresourceRange.layerCount = 1;
         colorImageView.image = offscreen.image;
         VK_CHECK_RESULT(vkCreateImageView(m_pdevice->device(), &colorImageView, nullptr, &offscreen.view));

         VkFramebufferCreateInfo fbufCreateInfo = vkinit::framebufferCreateInfo();
         fbufCreateInfo.renderPass = renderpass;
         fbufCreateInfo.attachmentCount = 1;
         fbufCreateInfo.pAttachments = &offscreen.view;
         fbufCreateInfo.width = dim;
         fbufCreateInfo.height = dim;
         fbufCreateInfo.layers = 1;
         VK_CHECK_RESULT(vkCreateFramebuffer(m_pdevice->device(), &fbufCreateInfo, nullptr, &offscreen.framebuffer));

         VkCommandBuffer layoutCmd = m_pdevice->createCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);
         tools::setImageLayout(layoutCmd, offscreen.image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
         m_pdevice->flushCommandBuffer(layoutCmd, m_transferQueue, true);
      }

      // Descriptor layout/pool/set (same as before)
      VkDescriptorSetLayout descriptorsetlayout;
      ::array_base<VkDescriptorSetLayoutBinding> setLayoutBindings = {
          vkinit::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0),
      };
      VkDescriptorSetLayoutCreateInfo descriptorsetlayoutCI = vkinit::descriptorSetLayoutCreateInfo(setLayoutBindings);
      VK_CHECK_RESULT(vkCreateDescriptorSetLayout(m_pdevice->device(), &descriptorsetlayoutCI, nullptr, &descriptorsetlayout));

      ::array_base<VkDescriptorPoolSize> poolSizes = { vkinit::descriptorPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1) };
      VkDescriptorPoolCreateInfo descriptorPoolCI = vkinit::descriptorPoolCreateInfo(poolSizes, 2);
      VkDescriptorPool descriptorpool;
      VK_CHECK_RESULT(vkCreateDescriptorPool(m_pdevice->device(), &descriptorPoolCI, nullptr, &descriptorpool));

      VkDescriptorSet descriptorset;
      VkDescriptorSetAllocateInfo allocInfo = vkinit::descriptorSetAllocateInfo(descriptorpool, &descriptorsetlayout, 1);
      VK_CHECK_RESULT(vkAllocateDescriptorSets(m_pdevice->device(), &allocInfo, &descriptorset));
      VkWriteDescriptorSet writeDescriptorSet = vkinit::writeDescriptorSet(descriptorset, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0, &environmentCube->m_descriptor);
      vkUpdateDescriptorSets(m_pdevice->device(), 1, &writeDescriptorSet, 0, nullptr);

      // Push block
      struct PushBlock {
         glm::mat4 mvp;
         float deltaPhi = (2.0f * float(M_PI)) / 180.0f;
         float deltaTheta = (0.5f * float(M_PI)) / 64.0f;
      } pushBlock;

      // Pipeline config — IMPORTANT: provide vertex input descriptions to match shader (location 0)
      pipeline_configuration_information cfg{};
      sandbox_pipeline::defaultPipelineConfigInfo(cfg);

      // Vertex input: location 0 is a vec3 position (adjust if your skybox vertex layout differs)
      VkVertexInputBindingDescription bindingDesc{};
      bindingDesc.binding = 0;
      bindingDesc.stride = sizeof(glm::vec3);
      bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
      VkVertexInputAttributeDescription attrDesc{};
      attrDesc.binding = 0;
      attrDesc.location = 0;
      attrDesc.format = VK_FORMAT_R32G32B32_SFLOAT; // vec3
      attrDesc.offset = 0;

      cfg.bindingDescriptions = { bindingDesc };
      cfg.attributeDescriptions = { attrDesc };

      cfg.renderPass = renderpass;
      cfg.pipelineLayout = VK_NULL_HANDLE;
      cfg.dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
      cfg.dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
      cfg.dynamicStateInfo.pDynamicStates = cfg.dynamicStateEnables.data();
      cfg.dynamicStateInfo.dynamicStateCount = uint32_t(cfg.dynamicStateEnables.size());
      cfg.descriptorSetLayouts = { descriptorsetlayout };

      VkPushConstantRange pushRange{};
      pushRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
      pushRange.offset = 0;
      pushRange.size = sizeof(PushBlock);
      cfg.pushConstantRanges = { pushRange };

      cfg.pipelineLayout = VK_NULL_HANDLE;

      ::string vert = ::string(PROJECT_ROOT_DIR) + "/res/shaders/spirV/filtered_cube.vert.spv";
      ::string frag = ::string(PROJECT_ROOT_DIR) + "/res/shaders/spirV/irradiance_cube.frag.spv";
      sandbox_pipeline irradiancePipeline{ m_pdevice, vert, frag, cfg };

      // COMMAND RECORDING
      VkCommandBuffer cmdBuf = m_pdevice->createCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);

      // Transition irradiance cubemap to TRANSFER_DST (outside any renderpass)
      VkImageSubresourceRange cubemapRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, numMips, 0, 6 };
      tools::setImageLayout(cmdBuf, irradianceCube->m_image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, cubemapRange);

      // Setup matrices (same as Sascha)
      ::array_base<glm::mat4> matrices = {
          glm::rotate(glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
          glm::rotate(glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
          glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
          glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
          glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
          glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
      };

      // Main loop: mips and faces (matches Sascha's approach)
      for (uint32_t m = 0; m < numMips; ++m) {
         uint32_t mipDim = static_cast<uint32_t>(dim * std::pow(0.5f, (float)m));
         VkViewport vp = vkinit::viewport((float)mipDim, (float)mipDim, 0.0f, 1.0f);
         VkRect2D sc = vkinit::rect2D(mipDim, mipDim, 0, 0);

         for (uint32_t face = 0; face < 6; ++face) {
            // Begin render pass into offscreen framebuffer
            VkClearValue clear{ {{0.0f, 0.0f, 0.2f, 0.0f}} };
            VkRenderPassBeginInfo rpBI = vkinit::renderPassBeginInfo();
            rpBI.renderPass = renderpass;
            rpBI.framebuffer = offscreen.framebuffer;
            rpBI.renderArea.extent = { mipDim, mipDim };
            rpBI.clearValueCount = 1;
            rpBI.pClearValues = &clear;
            vkCmdBeginRenderPass(cmdBuf, &rpBI, VK_SUBPASS_CONTENTS_INLINE);

            vkCmdSetViewport(cmdBuf, 0, 1, &vp);
            vkCmdSetScissor(cmdBuf, 0, 1, &sc);

            // push constants
            pushBlock.mvp = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 512.0f) * matrices[face];
            vkCmdPushConstants(cmdBuf, irradiancePipeline.getPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushBlock), &pushBlock);

            // bind pipeline and descriptor set (USE the allocated VkDescriptorSet)
            irradiancePipeline.bind(cmdBuf);
            vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, irradiancePipeline.getPipelineLayout(), 0, 1, &descriptorset, 0, nullptr);

            // draw skybox — ensure your skybox.draw binds the vertex buffer that matches location 0 vec3 pos
            if (!m_skyboxModel) {
               error("[asset_manager] No skybox model loaded - skipping draw in generateIrradianceMap()");
            }
            else {
               // m_skyboxModel is your GLTFmodelHandle (likely a shared_ptr to gltf::Model)
               m_skyboxModel->draw(cmdBuf);// this matches Sascha's models.skybox.draw(cmdBuf)
            }


            // END render pass BEFORE any barriers/copies
            vkCmdEndRenderPass(cmdBuf);

            // Transition offscreen image -> TRANSFER_SRC and copy to target cubemap mip/face
            tools::setImageLayout(cmdBuf, offscreen.image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

            VkImageCopy copyRegion{};
            copyRegion.srcSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
            copyRegion.dstSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, m, face, 1 };
            copyRegion.extent = { mipDim, mipDim, 1 };

            vkCmdCopyImage(cmdBuf,
               offscreen.image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
               irradianceCube->m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
               1, &copyRegion);

            // restore offscreen layout for next render
            tools::setImageLayout(cmdBuf, offscreen.image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
         }
      }

      // final transition for cubemap to shader read layout
      tools::setImageLayout(cmdBuf, irradianceCube->m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, cubemapRange);

      m_pdevice->flushCommandBuffer(cmdBuf, m_transferQueue);
      vkQueueWaitIdle(m_transferQueue);

      // cleanup (destroy created renderpass/framebuffer)
      vkDestroyFramebuffer(m_pdevice->device(), offscreen.framebuffer, nullptr);
      vkDestroyRenderPass(m_pdevice->device(), renderpass, nullptr);

      auto tEnd = std::chrono::high_resolution_clock::now();
      auto tDiff = std::chrono::duration<double, std::milli>(tEnd - tStart).count();
      information("Generating irradiance cube took {} ms", tDiff);
   }

   void asset_manager::generateBRDFlut() {
      auto tStart = std::chrono::high_resolution_clock::now();

      const VkFormat format = VK_FORMAT_R16G16_SFLOAT;	// R16G16 is supported pretty much everywhere
      const int32_t dim = 512;

      // Image
      VkImageCreateInfo imageCI = vkinit::imageCreateInfo();
      imageCI.imageType = VK_IMAGE_TYPE_2D;
      imageCI.format = format;
      imageCI.extent.width = dim;
      imageCI.extent.height = dim;
      imageCI.extent.depth = 1;
      imageCI.mipLevels = 1;
      imageCI.arrayLayers = 1;
      imageCI.samples = VK_SAMPLE_COUNT_1_BIT;
      imageCI.tiling = VK_IMAGE_TILING_OPTIMAL;
      imageCI.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
      VK_CHECK_RESULT(vkCreateImage(m_pdevice->device(), &imageCI, nullptr, &lutBrdf->m_image));
      VkMemoryAllocateInfo memAlloc = vkinit::memoryAllocateInfo();
      VkMemoryRequirements memReqs;
      vkGetImageMemoryRequirements(m_pdevice->device(), lutBrdf->m_image, &memReqs);
      memAlloc.allocationSize = memReqs.size;
      memAlloc.memoryTypeIndex = m_pdevice->getMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
      VK_CHECK_RESULT(vkAllocateMemory(m_pdevice->device(), &memAlloc, nullptr, &lutBrdf->m_deviceMemory));
      VK_CHECK_RESULT(vkBindImageMemory(m_pdevice->device(), lutBrdf->m_image, lutBrdf->m_deviceMemory, 0));
      // Image view
      VkImageViewCreateInfo viewCI = vkinit::imageViewCreateInfo();
      viewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
      viewCI.format = format;
      viewCI.subresourceRange = {};
      viewCI.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      viewCI.subresourceRange.levelCount = 1;
      viewCI.subresourceRange.layerCount = 1;
      viewCI.image = lutBrdf->m_image;
      VK_CHECK_RESULT(vkCreateImageView(m_pdevice->device(), &viewCI, nullptr, &lutBrdf->m_view));
      // Sampler
      VkSamplerCreateInfo samplerCI = vkinit::samplerCreateInfo();
      samplerCI.magFilter = VK_FILTER_LINEAR;
      samplerCI.minFilter = VK_FILTER_LINEAR;
      samplerCI.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
      samplerCI.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
      samplerCI.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
      samplerCI.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
      samplerCI.minLod = 0.0f;
      samplerCI.maxLod = 1.0f;
      samplerCI.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
      VK_CHECK_RESULT(vkCreateSampler(m_pdevice->device(), &samplerCI, nullptr, &lutBrdf->m_sampler));

      lutBrdf->m_descriptor.imageView = lutBrdf->m_view;
      lutBrdf->m_descriptor.sampler = lutBrdf->m_sampler;
      lutBrdf->m_descriptor.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      lutBrdf->m_pDevice = &m_pdevice;

      // FB, Att, RP, Pipe, etc.
      VkAttachmentDescription attDesc = {};
      // Color attachment
      attDesc.format = format;
      attDesc.samples = VK_SAMPLE_COUNT_1_BIT;
      attDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
      attDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
      attDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
      attDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
      attDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      attDesc.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      VkAttachmentReference colorReference = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };

      VkSubpassDescription subpassDescription = {};
      subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
      subpassDescription.colorAttachmentCount = 1;
      subpassDescription.pColorAttachments = &colorReference;

      // Use subpass dependencies for layout transitions
      ::preallocated_array_base< ::array_base <VkSubpassDependency, 2> > dependencies;
      dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
      dependencies[0].dstSubpass = 0;
      dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
      dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
      dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
      dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
      dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
      dependencies[1].srcSubpass = 0;
      dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
      dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
      dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
      dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
      dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
      dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

      // Create the actual renderpass
      VkRenderPassCreateInfo renderPassCI{};
      renderPassCI.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
      renderPassCI.attachmentCount = 1;
      renderPassCI.pAttachments = &attDesc;
      renderPassCI.subpassCount = 1;
      renderPassCI.pSubpasses = &subpassDescription;
      renderPassCI.dependencyCount = static_cast<uint32_t>(dependencies.size());
      renderPassCI.pDependencies = dependencies.data();

      VkRenderPass renderpass = VK_NULL_HANDLE;
      VK_CHECK_RESULT(vkCreateRenderPass(m_pdevice->device(), &renderPassCI, nullptr, &renderpass));

      VkFramebufferCreateInfo framebufferCI = vkinit::framebufferCreateInfo();
      framebufferCI.renderPass = renderpass;
      framebufferCI.attachmentCount = 1;
      framebufferCI.pAttachments = &lutBrdf->m_view;
      framebufferCI.width = dim;
      framebufferCI.height = dim;
      framebufferCI.layers = 1;

      VkFramebuffer framebuffer;
      VK_CHECK_RESULT(vkCreateFramebuffer(m_pdevice->device(), &framebufferCI, nullptr, &framebuffer));

      // Descriptors
      VkDescriptorSetLayout descriptorsetlayout;
      ::array_base<VkDescriptorSetLayoutBinding> setLayoutBindings = {};
      VkDescriptorSetLayoutCreateInfo descriptorsetlayoutCI = vkinit::descriptorSetLayoutCreateInfo(setLayoutBindings);
      VK_CHECK_RESULT(vkCreateDescriptorSetLayout(m_pdevice->device(), &descriptorsetlayoutCI, nullptr, &descriptorsetlayout));

      // Descriptor Pool
      ::array_base<VkDescriptorPoolSize> poolSizes = { vkinit::descriptorPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1) };
      VkDescriptorPoolCreateInfo descriptorPoolCI = vkinit::descriptorPoolCreateInfo(poolSizes, 2);
      VkDescriptorPool descriptorpool;
      VK_CHECK_RESULT(vkCreateDescriptorPool(m_pdevice->device(), &descriptorPoolCI, nullptr, &descriptorpool));

      // Descriptor sets
      VkDescriptorSet descriptorset;
      VkDescriptorSetAllocateInfo allocInfo = vkinit::descriptorSetAllocateInfo(descriptorpool, &descriptorsetlayout, 1);
      VK_CHECK_RESULT(vkAllocateDescriptorSets(m_pdevice->device(), &allocInfo, &descriptorset));

      // Pipeline layout
      VkPipelineLayout pipelinelayout;
      VkPipelineLayoutCreateInfo pipelineLayoutCI = vkinit::pipelineLayoutCreateInfo(&descriptorsetlayout, 1);
      VK_CHECK_RESULT(vkCreatePipelineLayout(m_pdevice->device(), &pipelineLayoutCI, nullptr, &pipelinelayout));

      // Pipeline
      VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = vkinit::pipelineInputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 0, VK_FALSE);
      VkPipelineRasterizationStateCreateInfo rasterizationState = vkinit::pipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE);
      VkPipelineColorBlendAttachmentState blendAttachmentState = vkinit::pipelineColorBlendAttachmentState(0xf, VK_FALSE);
      VkPipelineColorBlendStateCreateInfo colorBlendState = vkinit::pipelineColorBlendStateCreateInfo(1, &blendAttachmentState);
      VkPipelineDepthStencilStateCreateInfo depthStencilState = vkinit::pipelineDepthStencilStateCreateInfo(VK_FALSE, VK_FALSE, VK_COMPARE_OP_LESS_OR_EQUAL);
      VkPipelineViewportStateCreateInfo viewportState = vkinit::pipelineViewportStateCreateInfo(1, 1);
      VkPipelineMultisampleStateCreateInfo multisampleState = vkinit::pipelineMultisampleStateCreateInfo(VK_SAMPLE_COUNT_1_BIT);
      ::array_base<VkDynamicState> dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
      VkPipelineDynamicStateCreateInfo dynamicState = vkinit::pipelineDynamicStateCreateInfo(dynamicStateEnables);
      VkPipelineVertexInputStateCreateInfo emptyInputState = vkinit::pipelineVertexInputStateCreateInfo();
      ::preallocated_array_base< ::array_base <VkPipelineShaderStageCreateInfo, 2> > shaderStages;

      VkGraphicsPipelineCreateInfo pipelineCI = vkinit::pipelineCreateInfo(pipelinelayout, renderpass);
      pipelineCI.pInputAssemblyState = &inputAssemblyState;
      pipelineCI.pRasterizationState = &rasterizationState;
      pipelineCI.pColorBlendState = &colorBlendState;
      pipelineCI.pMultisampleState = &multisampleState;
      pipelineCI.pViewportState = &viewportState;
      pipelineCI.pDepthStencilState = &depthStencilState;
      pipelineCI.pDynamicState = &dynamicState;
      pipelineCI.stageCount = 2;
      pipelineCI.pStages = shaderStages.data();
      pipelineCI.pVertexInputState = &emptyInputState;


      // 4) Fill your pipeline_configuration_information
      pipeline_configuration_information cfg{};
      sandbox_pipeline::defaultPipelineConfigInfo(cfg);

      cfg.bindingDescriptions.clear();
      cfg.attributeDescriptions.clear();
      cfg.renderPass = renderpass;
      cfg.pipelineLayout = pipelinelayout;
      // viewport & scissor will be dynamic
      cfg.dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
      cfg.dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
      cfg.dynamicStateInfo.pDynamicStates = cfg.dynamicStateEnables.data();
      cfg.dynamicStateInfo.dynamicStateCount = (uint32_t)cfg.dynamicStateEnables.size();

      // Look-up-table (from BRDF) pipeline
      ::string vert = ::string(PROJECT_ROOT_DIR) + "/res/shaders/spirV/gen_brdflut.vert.spv";
      ::string frag = ::string(PROJECT_ROOT_DIR) + "/res/shaders/spirV/gen_brdflut.frag.spv";
      sandbox_pipeline brdfPipeline{ m_pdevice, vert, frag, cfg };

      // Render
      VkClearValue clearValues[1];
      clearValues[0].color = { { 0.0f, 0.0f, 0.0f, 1.0f } };

      VkRenderPassBeginInfo renderPassBeginInfo = vkinit::renderPassBeginInfo();
      renderPassBeginInfo.renderPass = renderpass;
      renderPassBeginInfo.renderArea.extent.width = dim;
      renderPassBeginInfo.renderArea.extent.height = dim;
      renderPassBeginInfo.clearValueCount = 1;
      renderPassBeginInfo.pClearValues = clearValues;
      renderPassBeginInfo.framebuffer = framebuffer;

      VkCommandBuffer cmdBuf = m_pdevice->createCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);
      vkCmdBeginRenderPass(cmdBuf, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
      VkViewport viewport = vkinit::viewport((float)dim, (float)dim, 0.0f, 1.0f);
      VkRect2D scissor = vkinit::rect2D(dim, dim, 0, 0);
      vkCmdSetViewport(cmdBuf, 0, 1, &viewport);
      vkCmdSetScissor(cmdBuf, 0, 1, &scissor);

      brdfPipeline.bind(cmdBuf);
      vkCmdDraw(cmdBuf, 3, 1, 0, 0);
      vkCmdEndRenderPass(cmdBuf);
      m_pdevice->flushCommandBuffer(cmdBuf, m_transferQueue);

      vkQueueWaitIdle(m_transferQueue);

      vkDestroyFramebuffer(m_pdevice->device(), framebuffer, nullptr);
      vkDestroyRenderPass(m_pdevice->device(), renderpass, nullptr);

      auto tEnd = std::chrono::high_resolution_clock::now();
      auto tDiff = std::chrono::duration<double, std::milli>(tEnd - tStart).count();
      information() << "Generating BRDF LUT took " << tDiff << " ms";
   }

   void asset_manager::generatePrefilteredEnvMap() {

   }


   ::pointer<sandbox_object_model> asset_manager::loadObjModel(
      const ::scoped_string& name,
      const ::scoped_string& filepath,
      bool isSkybox
   ) {
      // 1) cache check
      if (auto it = m_objModelCache.find(name); it != m_objModelCache.end())
         return it->element2();

      // 2) load
      auto model = sandbox_object_model::createModelFromFile(m_pdevice, filepath, isSkybox);

      // 3) cache & return
      m_objModelCache[name] = model;
      return model;
   }


   ::pointer<gltf::Model> asset_manager::loadGLTFmodel(
      const ::scoped_string& name,
      const ::scoped_string& filepath,
      uint32_t gltfFlags,
      float scale
   ) {
      if (auto it = m_gltfModelCache.find(name); it != m_gltfModelCache.end())
         return it->element2();

      auto model = øcreate_pointer<gltf::Model>();
      model->loadFromFile(filepath, &m_pdevice, m_pdevice->graphicsQueue(), gltfFlags, scale);

      m_gltfModelCache[name] = model;
      return model;
   }

   ::pointer<sandbox_texture> asset_manager::loadCubemap(
      const ::scoped_string& name,
      const ::scoped_string& ktxFilename,
      VkFormat format,
      VkImageUsageFlags usageFlags,
      VkImageLayout initialLayout)
   {
      if (auto it = m_textures.find(name); it != m_textures.end())
         return it->element2();

      auto tex = øcreate_pointer<sandbox_texture>();
      tex->m_pDevice = &m_pdevice;
      try {
         tex->KtxLoadCubemapFromFile(
            name,
            ktxFilename,
            format,
            &m_pdevice,
            m_pdevice->graphicsQueue(),
            usageFlags,
            initialLayout
         );
      }
      catch (const ::exception& e) {
         throw std::runtime_error("Failed to load HDR cubemap '" + name + "': " + e.what());
      }

      registerTextureIfNeeded(name, tex, m_textures, m_textureIndexMap, m_textureList);
      return tex;
   }




   void asset_manager::registerTextureIfNeeded(
      const ::scoped_string& name,
      const ::pointer<sandbox_texture>& tex,
      ::string_map< ::pointer<sandbox_texture>>& textures,
      ::string_map< size_t>& textureIndexMap,
      ::pointer_array_base<sandbox_texture >> &textureList)
   {
      if (textures.find(name) == textures.end()) {
         textures[name] = tex;
         textureList.add(tex);
         textureIndexMap[name] = textureList.size() - 1;
      }
   }


} // namespace sandbox_engine



