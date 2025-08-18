#pragma once
//#include <memory>
//#include "window.h"
#include "sandbox_renderer/include/vulkan_wrapper/vulkan_device.h"
#include "sandbox_renderer/include/vulkan_wrapper/vulkan_instance.h"
#include "sandbox_renderer/include/vulkan_wrapper/vulkan_renderer.h"
#include "sandbox_interfaces/include/interfaces/game_layer_i.h"
#include "sandbox_interfaces/include/interfaces/camera_i.h"
#include "sandbox_interfaces/include/interfaces/window_input_i.h"
#include "sandbox_engine/include/common/glfw_input.h"
#include "sandbox_engine/include/asset_manager.h"



class SandboxEngine {
public:
    static constexpr uint32_t WIDTH = 1440;
    static constexpr uint32_t HEIGHT = 810;

    SandboxEngine();
    ~SandboxEngine() = default;

    void initialize();

    void initLayer(IGameLayer* game);

    void run(::pointer<IGameLayer> game);
private:
//    SandboxWindow                       m_window{ WIDTH, HEIGHT, "A vulkan place" };
   //::pointer < ::sandbox_renderer::sandbox_window> m_pwindow;
    ::pointer < ::sandbox_renderer::sandbox_instance >                   m_pinstance;
    //::sandbox_renderer::sandbox_device                     m_device{ m_vkinstance, m_window };
    ::pointer < ::sandbox_renderer::sandbox_device >                     m_pdevice;
    //::pointer < AssetManager >                        m_passetManager{ m_device };
    ::pointer < AssetManager >                        m_passetManager;
    //::sandbox_renderer::sandbox_renderer                   m_renderer{ m_device, m_window };
    ::pointer < ::sandbox_renderer::sandbox_renderer >                   m_prenderer;
 
    VkSurfaceKHR                        m_surface = VK_NULL_HANDLE;
    VkInstance                          m_vkinstance_handle= VK_NULL_HANDLE;

    ::pointer<IWindowInput>       m_windowInput;

public:
    ::pointer<IWindowInput> getInputSharedPtr() {
        return m_windowInput;
    }
    AssetManager* getAssetManager() { return m_passetManager; }

    ISandboxRenderer& renderer();
    void toggleCursorLock();
    void setupInputCallbacks();
    void processInput();

    bool m_cursorLocked = true;


};
