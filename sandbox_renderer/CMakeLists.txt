# sandbox_renderer/CMakeLists.txt
project(sandbox_renderer LANGUAGES CXX)


list(APPEND library_source
        #src/window.cpp
        #src/vk_tools/vk_tools.cpp
        render_systems/gltf_render_system.cpp
        render_systems/gltf_render_system.h
        render_systems/object_render_system.cpp
        render_systems/object_render_system.h
        render_systems/point_light_render_system.cpp
        render_systems/point_light_render_system.h
        render_systems/skybox_ibl_render_system.cpp
        render_systems/skybox_ibl_render_system.h
        #instance.cpp
        #instance.h
        device.cpp
        device.h
        renderer.cpp
        renderer.h
        #swapchain.cpp
        #swapchain.h
        #buffer.cpp
        #buffer.h
        pipeline.cpp
        pipeline.h
        #descriptor.cpp
        #descriptor.h
        object.cpp
        object.h
        #gltf.cpp
        texture.cpp
        texture.h
)


add_library(${PROJECT_NAME} SHARED ${library_source})
add_library(static_${PROJECT_NAME} SHARED ${library_source})

if (NOT ${NO_PRECOMPILED_HEADER})

    target_precompile_headers(${PROJECT_NAME} PRIVATE framework.h)
    target_precompile_headers(static_${PROJECT_NAME} PRIVATE framework.h)

endif ()


set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)


target_include_directories(sandbox_renderer
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include/
)

#find_package(Vulkan REQUIRED)

# Link against required libraries
target_link_libraries(${PROJECT_NAME}
        PUBLIC
        sandbox
)

#
#target_link_libraries(${PROJECT_NAME}
#        PUBLIC
#        glfw
#        glm
#        spdlog
#        Vulkan::Vulkan
#        tinyobjloader
#        tinygltf
#        ktx
#        core_interfaces
#)


list(APPEND library_include_directories ${CMAKE_CURRENT_SOURCE_DIR})
list(APPEND library_include_directories ${CMAKE_CURRENT_SOURCE_DIR}/..)
list(APPEND library_include_directories ${CMAKE_CURRENT_SOURCE_DIR}/../..)

target_include_directories(${PROJECT_NAME} PRIVATE ${library_include_directories})
target_include_directories(static_${PROJECT_NAME} PRIVATE ${library_include_directories})


target_compile_definitions(${PROJECT_NAME} PRIVATE _${PROJECT_NAME}_project)

