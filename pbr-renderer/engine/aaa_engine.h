#pragma once

#include "rendermanager.h"
#include "cameramanager.h"
#include "engineconfig.h"
#include "inputmanager.h"
#include "windowmanager.h"

/**
 * Top level object that orchestrates/contains all subsystems.
 */
class Engine {
public:
    Engine(EngineConfig &config);
    void startup(::pointer<Scene> scene);
    void run();
    void shutdown();
private:
    EngineConfig m_pengineconfig;
    ::pointer<WindowManager> mWindowManager;
    ::pointer<InputManager> mInputManager;
    ::pointer<CameraManager> mCameraManager;
    ::pointer<RenderManager> mRenderManager;
};