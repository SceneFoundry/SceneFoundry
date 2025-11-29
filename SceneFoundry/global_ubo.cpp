// Created by camilo on 2025-08-23 21:37 <3ThomasBorregaardSorensen!!
#include "framework.h"
#include "global_ubo.h"


BEGIN_GPU_PROPERTIES(::SceneFoundry_SceneFoundry::global_ubo)
GPU_PROPERTY("projection", ::gpu::e_type_mat4)
GPU_PROPERTY("view", ::gpu::e_type_mat4)
GPU_PROPERTY("invView", ::gpu::e_type_mat4)
GPU_PROPERTY("ambientLightColor", ::gpu::e_type_seq4)
GPU_PROPERTY("cameraPosition", ::gpu::e_type_seq4)
GPU_PROPERTY("pointLights", ::gpu_properties<::gpu::point_light>(), ::SceneFoundry_SceneFoundry::global_ubo::MAX_LIGHTS)
GPU_PROPERTY("numLights", ::gpu::e_type_int)
END_GPU_PROPERTIES()



