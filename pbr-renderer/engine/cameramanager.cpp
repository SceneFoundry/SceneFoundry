#include "framework.h"
#include "cameramanager.h"
#include "camera.h"
#include "bred/user/user/graphics3d.h"

namespace SceneFoundry_pbr_renderer
{

   CameraManager::CameraManager()
   {


   }
   

   CameraManager::~CameraManager() {}

      //void CameraManager::initialize_CameraManager(EngineConfig &engineConfig, ::pointer<WindowManager> windowManager,
      //                                          ::pointer<InputManager> inputManager) :

   void CameraManager::initialize_CameraManager(::user::graphics3d * pusergraphics3d,
                                InputManager * pinputmanager) 
      {

      m_pusergraphics3d = pusergraphics3d;
         m_pinputmanager = pinputmanager;
       //m_pengineconfig(engineConfig),
       // mWindowManager(windowManager),
       //m_pinputmanager(inputManager)
   //{
   }

   void CameraManager::startup()
   {
      // camera
      glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
      glm::vec3 cameraInitialPosition = glm::vec3(0.0f, 0.0f, 3.0f);

      float cameraInitialYaw = 0;
      float cameraInitialPitch = 0;

      m_pcamera = øcreate_new<Camera>();
      
      m_pcamera->initialize_Camera(cameraUp, cameraInitialPosition, cameraInitialYaw, cameraInitialPitch,
                                           m_pusergraphics3d->width(), m_pusergraphics3d->height());
                                         //m_pengineconfig.initialViewportWidth, m_pengineconfig.initialViewportHeight);

      m_pinputmanager->hideMouseCursor();
      m_pinputmanager->disableGuiMouse();
   }

   void CameraManager::update(float frameTimeDelta)
   {
      // pass actions to camera
      // if (mWindowManager->windowResized()) {
      //  m_pcamera->setWindowDimensions(mWindowManager->getWindowSize());
      //}
      m_pcamera->deferSetWindowDimensions(m_pusergraphics3d->width(), m_pusergraphics3d->height());

      auto actions = m_pinputmanager->getCurrentActions();

      if (actions.find(KeymapAction::DISABLE_MOUSE_CAMERA) != actions.end())
      {
         mMouseCameraEnabled = false;
         m_pinputmanager->showMouseCursor();
         m_pinputmanager->enableGuiMouse();
         m_pcamera->resetMouse();
      }

      if (actions.find(KeymapAction::ENABLE_MOUSE_CAMERA) != actions.end())
      {
         mMouseCameraEnabled = true;
         m_pinputmanager->hideMouseCursor();
         m_pinputmanager->disableGuiMouse();
      }

      if (mMouseCameraEnabled)
      {
         m_pcamera->processMouse(m_pinputmanager->getMousePosition());
         m_pcamera->processActions(frameTimeDelta, actions);
      }
   }

   void CameraManager::drawDebugPanel() { m_pcamera->drawDebugPanel(); }

   glm::vec3 CameraManager::getPosition() { return m_pcamera->getPosition(); }

   glm::mat4 CameraManager::getViewMatrix() { return m_pcamera->getViewMatrix(); }

   glm::mat4 CameraManager::getProjectionMatrix() { return m_pcamera->getProjectionMatrix(); }


} // namespace SceneFoundry_pbr_renderer
