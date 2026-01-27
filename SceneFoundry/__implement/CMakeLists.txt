cmake_minimum_required(VERSION 3.16)
project(_SceneFoundry_SceneFoundry CXX)



list(APPEND application_source main.cpp)


if(${WIN32})

    list(APPEND application_source ../deployment.rc main.rc)
endif()


add_executable(${PROJECT_NAME} ${application_source})


list(APPEND executable_references
        acme
        apex
        aqua
        aura
        axis
        berg
        bred
        core
        #sandbox
        #sandbox_renderer
        #sandbox_engine
        #sandbox_game
        SceneFoundry_SceneFoundry
)




if (MSVC)


    target_compile_options(${PROJECT_NAME} PRIVATE "/MD$<$<CONFIG:Debug>:d>")
    target_link_options(${PROJECT_NAME} PRIVATE "/DEFAULTLIB:msvcrt$<$<CONFIG:Debug>:d>")
    target_link_options(${PROJECT_NAME} PRIVATE "/SUBSYSTEM:WINDOWS")
    target_link_libraries(${PROJECT_NAME} PRIVATE ${executable_references})


elseif(${LINUX} OR ${__BSD__})


    target_compile_options(${PROJECT_NAME} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-fpermissive  ${EXTRA_CXX_TARGET_COMPILER_OPTIONS}>)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${executable_references})
    target_link_options(${PROJECT_NAME} PRIVATE -Wl,-z,defs)


endif ()


add_dependencies(${PROJECT_NAME} ${default_node})
add_dependencies(${PROJECT_NAME} ${default_draw2d})
add_dependencies(${PROJECT_NAME} ${default_write_text})
add_dependencies(${PROJECT_NAME} ${app_common_dependencies})
add_dependencies(${PROJECT_NAME} crypto_openssl)
add_dependencies(${PROJECT_NAME} experience_core)
add_dependencies(${PROJECT_NAME} folder_zip)

if(${WITH_X11_OPERATING_AMBIENT})
   add_dependencies(${PROJECT_NAME} operating_ambient_x11)
endif()
if(${INCLUDE_OPENGL})
   add_dependencies(${PROJECT_NAME} typeface_freetype)
endif()



if(${WIN32})

add_dependencies(${PROJECT_NAME} draw2d_direct2d)
add_dependencies(${PROJECT_NAME} write_text_direct2d)
add_dependencies(${PROJECT_NAME} draw2d_directx11)
add_dependencies(${PROJECT_NAME} write_text_directx11)
endif()
add_dependencies(${PROJECT_NAME} draw2d_opengl)
add_dependencies(${PROJECT_NAME} draw2d_vulkan)

if(${WIN32})

add_dependencies(${PROJECT_NAME} graphics3d_directx11)
add_dependencies(${PROJECT_NAME} graphics3d_directx12)
endif()
add_dependencies(${PROJECT_NAME} graphics3d_opengl)
add_dependencies(${PROJECT_NAME} graphics3d_vulkan)

list(APPEND executable_include_directories ${CMAKE_CURRENT_SOURCE_DIR})
list(APPEND executable_include_directories ${CMAKE_CURRENT_SOURCE_DIR}/..)
list(APPEND executable_include_directories ${CMAKE_CURRENT_SOURCE_DIR}/../..)

target_include_directories(${PROJECT_NAME} PRIVATE ${executable_include_directories})


target_link_directories(${PROJECT_NAME} PRIVATE ${library_link_directories})

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)



