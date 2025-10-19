#include "framework.h"
#include "engine.h"

Engine::Engine(EngineConfig &engineConfig) : m_pengineconfig(engineConfig) {
    mWindowManager = std::make_shared<WindowManager>(engineConfig);
    m_pinputmanager = std::make_shared<InputManager>(engineConfig, mWindowManager);
    mCameraManager = std::make_shared<CameraManager>(engineConfig, mWindowManager, m_pinputmanager);
    mRenderManager = std::make_shared<RenderManager>(engineConfig, mWindowManager, mCameraManager);
}

void Engine::startup(::pointer<Scene> scene)
{
    std::cout << "Engine::startup";
    mWindowManager->startup(m_pinputmanager);
    mRenderManager->startup(scene);
    m_pinputmanager->startup();
    mCameraManager->startup();
}

void Engine::run()
{
    std::cout << "Engine::run\n";
    float lastFrameTime = 0.0f;
    float frameTimeDelta = 0.0f;

    while (!mWindowManager->windowShouldClose()) {
        float currentTime = glfwGetTime();
        frameTimeDelta = currentTime - lastFrameTime;
        lastFrameTime = currentTime;

        m_pinputmanager->update();

        mRenderManager->startGuiFrame();

        mWindowManager->update();
        mCameraManager->update(frameTimeDelta);
        mRenderManager->render();

        mWindowManager->clearResizeFlag();
    }
}

void Engine::shutdown() {
    mRenderManager->shutdown();
    mWindowManager->shutdown();
}