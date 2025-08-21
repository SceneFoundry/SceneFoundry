#pragma once


#include "bred/_.h"

#if defined(_sandbox_project)
#define CLASS_DECL_SANDBOX  CLASS_DECL_EXPORT
#else
#define CLASS_DECL_SANDBOX  CLASS_DECL_IMPORT
#endif


namespace graphics3d
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
    //class IBuffer;
    class IWindowInput;


} // namespace graphics3d


#include "SceneFoundry/graphics3d/_constant.h"
#include "SceneFoundry/graphics3d/_types.h"



