cmake_minimum_required(VERSION 3.16)
project(_SceneFoundry_pbr_renderer CXX)



list(APPEND application_source main.cpp)


if(MSVC)
    list(APPEND application_source ../deployment.rc main.rc)
endif()


add_executable(${PROJECT_NAME} ${application_source})


get_filename_component(LOCAL_APP_FOLDER_PATH "${CMAKE_CURRENT_SOURCE_DIR}" DIRECTORY)
get_filename_component(LOCAL_APP_FOLDER_NAME "${LOCAL_APP_FOLDER_PATH}" NAME)

get_filename_component(LOCAL_ROOT_FOLDER_PATH "${LOCAL_APP_FOLDER_PATH}" DIRECTORY)
get_filename_component(LOCAL_ROOT_FOLDER_NAME "${LOCAL_ROOT_FOLDER_PATH}" NAME)

message(STATUS "_pbr-renderer LOCAL_ROOT_FOLDER_NAME is ${LOCAL_ROOT_FOLDER_NAME}")
message(STATUS "_pbr-renderer LOCAL_APP_FOLDER_NAME is ${LOCAL_APP_FOLDER_NAME}")

set(LOCAL_APPID ${LOCAL_ROOT_FOLDER_NAME}/${LOCAL_APP_FOLDER_NAME})

message(STATUS "_pbr-renderer LOCAL_APPID is ${LOCAL_APPID}")

# Create zip
add_custom_command(
   OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/../_matter.zip"
   COMMAND ${APPLICATION_BUILD_HELPER_BINARY} --prepare-application ${LOCAL_APPID}
   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../../..
   DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/../matter.txt"
)

# Attach .zip as dependency of resource script
set_source_files_properties(
   main.rc
   PROPERTIES
   OBJECT_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/../_matter.zip"
)

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
        SceneFoundry_pbr_renderer
)


if(${LINUX} OR ${__BSD__})


   add_custom_command(
      OUTPUT ${PROJECT_SOURCE_DIR}/../.link_object/_matter.zip.o
      COMMAND ${APPLICATION_BUILD_HELPER_BINARY} --prepare-application ${PROJECT_SOURCE_DIR}
      COMMENT "Generating ${PROJECT_SOURCE_DIR}/../.link_object/_matter.zip.o"
   )

   target_sources(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/../.link_object/_matter.zip.o)

endif()


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

add_dependencies(${PROJECT_NAME} draw2d_direct2d)
add_dependencies(${PROJECT_NAME} write_text_direct2d)
add_dependencies(${PROJECT_NAME} draw2d_directx11)
add_dependencies(${PROJECT_NAME} write_text_directx11)
add_dependencies(${PROJECT_NAME} draw2d_opengl)
add_dependencies(${PROJECT_NAME} draw2d_vulkan)

add_dependencies(${PROJECT_NAME} graphics3d_directx11)
add_dependencies(${PROJECT_NAME} graphics3d_directx12)
add_dependencies(${PROJECT_NAME} graphics3d_opengl)
add_dependencies(${PROJECT_NAME} graphics3d_vulkan)

list(APPEND executable_include_directories ${CMAKE_CURRENT_SOURCE_DIR})
list(APPEND executable_include_directories ${CMAKE_CURRENT_SOURCE_DIR}/..)
list(APPEND executable_include_directories ${CMAKE_CURRENT_SOURCE_DIR}/../..)

target_include_directories(${PROJECT_NAME} PRIVATE ${executable_include_directories})


target_link_directories(${PROJECT_NAME} PRIVATE ${library_link_directories})

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)



