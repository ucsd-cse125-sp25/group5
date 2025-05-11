cmake_minimum_required(VERSION 3.16)

project(menv)

file(GLOB SOURCES
    *.h
    *.cpp
    imgui/*.h
    imgui/*.cpp
    include/*.h
    src/*.cpp
    include/network/*.h
    src/network/*.cpp
)

add_subdirectory(lib/assimp)

# Require GL
find_package(OpenGL REQUIRED)

set(FMOD_DIR "${PROJECT_SOURCE_DIR}/lib/fmod/api/core")

# Add include directories
include_directories(
    include 
    "lib/assimp/include"
    ${FMOD_DIR}/inc
    "../include"
)

# Add library directories
link_directories(
    lib
    ${FMOD_DIR}/lib/x64
)


# Add executable
add_executable(${PROJECT_NAME} ${SOURCES}  "include/imgui.h" "include/imgui_impl_opengl3.h" "include/imconfig.h" "include/imgui_impl_opengl3_loader.h" "include/imgui_impl_win32.h" "include/imgui_impl_glfw.h" "src/imgui_impl_glfw.cpp" "src/Keyframe.cpp" "include/Keyframe.h" "include/Channel.h" "include/Animation.h" "src/Animation.cpp" "src/Channel.cpp" "src/Player.cpp" "include/Player.h" "src/Object.cpp" "include/Object.h" "src/Scene.cpp" "include/Scene.h" "include/Skybox.h" "src/Skybox.cpp" "src/Mesh.cpp" "include/Mesh.h" "include/stb_image.h" "src/stb_image_definer.cpp" "src/stb_image_impl.cpp" "include/network/ClientGame.h" "include/Global.h" "include/UIManager.h" "src/UIManager.cpp" "include/UIImg.h" "src/UIImg.cpp" "include/Light.h" "src/Light.cpp" "src/PlayerObject.cpp" "include/PlayerObject.h" "include/Audio.h" "src/Audio.cpp")

target_compile_definitions(${PROJECT_NAME} PRIVATE 
    PROJECT_SOURCE_DIR="${PROJECT_SOURCE_DIR}"
)

# Link libraries
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} glew32s.lib glfw3 assimp fmod_vc)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${FMOD_DIR}/lib/x64/fmod.dll"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
)


# Move assets to .exe
add_custom_target(CopyShaders ALL
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	"${PROJECT_SOURCE_DIR}/shaders"
	"${CMAKE_BINARY_DIR}/shaders"
)
add_dependencies(menv CopyShaders)

