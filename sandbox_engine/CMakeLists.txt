# sandbox_engine/CMakeLists.txt
cmake_minimum_required(VERSION 3.24)
project(sandbox_engine)

#add_library(sandbox_engine STATIC
#)




list(APPEND library_source
        engine.h
        engine.cpp
        asset_manager.cpp
        asset_manager.h
        glfw_input.cpp
        glfw_input.h
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
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link against required libraries
target_link_libraries(${PROJECT_NAME}
    PUBLIC
        sandbox_interfaces
        sandbox_renderer
)


#target_link_libraries(sandbox_engine
#        PUBLIC
#        sandbox_renderer
#        spdlog::spdlog
#        glfw
#        json
#        core_interfaces
#)





list(APPEND library_include_directories ${CMAKE_CURRENT_SOURCE_DIR})
list(APPEND library_include_directories ${CMAKE_CURRENT_SOURCE_DIR}/..)
list(APPEND library_include_directories ${CMAKE_CURRENT_SOURCE_DIR}/../..)

target_include_directories(${PROJECT_NAME} PRIVATE ${library_include_directories})
target_include_directories(static_${PROJECT_NAME} PRIVATE ${library_include_directories})


target_compile_definitions(${PROJECT_NAME} PRIVATE _${PROJECT_NAME}_project)




