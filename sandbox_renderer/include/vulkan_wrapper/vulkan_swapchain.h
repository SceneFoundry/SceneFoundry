#pragma once

#include <vulkan/vulkan.h>
//#include <vector>
//#include <memory>

class sandbox_device;

class VkSandboxSwapchain {
public:

    static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

    VkSandboxSwapchain(
        sandbox_device& deviceRef,
        VkExtent2D extent);

    VkSandboxSwapchain(sandbox_device& device,
        VkExtent2D      extent,
        ::pointer<VkSandboxSwapchain> oldSwapchain);
    ~VkSandboxSwapchain();

    VkSandboxSwapchain(const VkSandboxSwapchain&) = delete;
    VkSandboxSwapchain& operator=(const VkSandboxSwapchain&) = delete;


    VkFramebuffer getFrameBuffer(int index) { return m_swapChainFramebuffers[index]; }
    VkRenderPass getRenderPass() { return m_renderPass; }
    VkImageView getImageView(int index) { return m_swapChainImageViews[index]; }
    size_t imageCount() { return m_swapChainImages.size(); }
    VkFormat getSwapChainImageFormat() { return m_swapChainImageFormat; }
    VkExtent2D getSwapChainExtent() { return m_swapChainExtent; }
    uint32_t width() { return m_swapChainExtent.width; }
    uint32_t height() { return m_swapChainExtent.height; }

    static constexpr int GetMaxFramesInFlight(){ return MAX_FRAMES_IN_FLIGHT; }

    VkResult acquireNextImage(uint32_t* imageIndex);
    VkResult submitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex);

    float    extentAspectRatio() {
             return static_cast<float>(m_swapChainExtent.width) / static_cast<float>(m_swapChainExtent.height);
    }
    VkFormat findDepthFormat();

    bool     compareSwapFormats(const VkSandboxSwapchain& swapChain) const {
             return swapChain.m_swapChainDepthFormat == m_swapChainDepthFormat &&
             swapChain.m_swapChainImageFormat == m_swapChainImageFormat;
    }
    VkFence getFence(uint32_t frameIndex) const {
        return m_inFlightFences[frameIndex];
    }

private:
    void init();
    void createSwapChain();
    void createImageViews();
    void createDepthResources();
    void createRenderPass();
    void createFramebuffers();
    void createSyncObjects();


    VkSurfaceFormatKHR chooseSwapSurfaceFormat(
        const ::array_base<VkSurfaceFormatKHR>& availableFormats);

    VkPresentModeKHR chooseSwapPresentMode(
        const ::array_base<VkPresentModeKHR>& availablePresentModes);

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);


    VkFormat m_swapChainImageFormat;
    VkFormat m_swapChainDepthFormat;
    VkExtent2D m_swapChainExtent;

    ::array_base<VkFramebuffer> m_swapChainFramebuffers;
    VkRenderPass m_renderPass;

    ::array_base<VkImage> m_depthImages;
    ::array_base<VkDeviceMemory> m_depthImageMemory;
    ::array_base<VkImageView> m_depthImageViews;
    ::array_base<VkImage> m_swapChainImages;
    ::array_base<VkImageView> m_swapChainImageViews;

    sandbox_device& m_device;
    VkExtent2D m_windowExtent;

    VkSwapchainKHR m_swapChain;
    ::pointer<VkSandboxSwapchain> m_oldSwapChain;

    size_t                  m_swapChainImageCount = 0;
    ::array_base<VkSemaphore> m_imageAvailableSemaphores; // one per image
    ::array_base<VkSemaphore> m_renderFinishedSemaphores; // one per image
    ::array_base<VkFence>     m_inFlightFences;           // one per CPU-frame
    ::array_base<VkFence>     m_imagesInFlight;           // last-used fence per image
    size_t                  m_currentFrame = 0;
};
