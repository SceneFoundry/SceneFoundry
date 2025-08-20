# sandbox_renderer/CMakeLists.txt
project(sandbox_interfaces LANGUAGES CXX)


list(APPEND library_source
        _.cpp
        _.h
        _.h

        _sandbox_interfaces.h
        point_light_component.h
        asset_provider.h
        camera.h
        class_set.h
        class_set1.h
        entity.h
        #frame_info.h
        framework.cpp
        framework.h
        game_layer.h
        game_object.h
        key_codes.h
        model.h
        provider.h
        render_system.h
        renderer.h
        scene.h
        transform_component.h
        window_input.h
        asset_provider.h
        camera.h
        class_set.h
        class_set1.h
        entity.h
        frame.h
        frame_info.h
        game_layer.h
        game_object.h
        key_codes.h
        model.h
        provider.h
        render_system.h
        renderer.h
        scene.h
        transform_component.h
        window_input.h
        #         src/window.cpp
        #         src/vk_tools/vk_tools.cpp
        #         src/vulkan_wrapper/vulkan_instance.cpp
        #         src/vulkan_wrapper/vulkan_device.cpp
        #         src/vulkan_wrapper/vulkan_renderer.cpp
        #         src/vulkan_wrapper/vulkan_swapchain.cpp
        #         src/vulkan_wrapper/vulkan_buffer.cpp
        #         src/vulkan_wrapper/vulkan_pipeline.cpp
        #         src/vulkan_wrapper/vulkan_descriptor.cpp
        #         src/vulkan_wrapper/vulkan_obj.cpp
        #         src/vulkan_wrapper/vulkan_gltf.cpp
        #         src/vulkan_wrapper/vulkan_texture.cpp
        #         src/vulkan_wrapper/render_systems/obj_render_system.cpp
        #         src/vulkan_wrapper/render_systems/gltf_render_system.cpp
        #         src/vulkan_wrapper/render_systems/point_light_rs.cpp
        #         src/vulkan_wrapper/render_systems/skybox_ibl_rs.cpp
)


add_library(${PROJECT_NAME} SHARED ${library_source})
add_library(static_${PROJECT_NAME} SHARED ${library_source})


if (NOT ${NO_PRECOMPILED_HEADER})

    target_precompile_headers(${PROJECT_NAME} PRIVATE framework.h)
    target_precompile_headers(static_${PROJECT_NAME} PRIVATE framework.h)

endif ()


set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)


target_include_directories(${PROJECT_NAME}
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include/
)

#find_package(Vulkan REQUIRED)

## Link against required libraries
#target_link_libraries(${PROJECT_NAME}
#    PUBLIC
#        sandbox_interfaces
#)

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




