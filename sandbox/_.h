#pragma once


#include "bred/_.h"

#if defined(_sandbox_project)
#define CLASS_DECL_SANDBOX  CLASS_DECL_EXPORT
#else
#define CLASS_DECL_SANDBOX  CLASS_DECL_IMPORT
#endif


namespace sandbox
{

    class IScene;
    class ICamera;
    class IGameObject;
    class IRenderer;
    class IProvider;
    class IModel;
    class IAssetProvider;
    class IEntity;
    class IFrame;
    class IRenderSystem;
    class IBuffer;
    class IWindowInput;


} // namespace sandbox


#include "SceneFoundry/sandbox/_constant.h"
#include "SceneFoundry/sandbox/_types.h"



