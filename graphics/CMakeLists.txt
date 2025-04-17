cmake_minimum_required(VERSION 3.16)

project(menv)

file(GLOB SOURCES
    *.h
    *.cpp
    imgui/*.h
    imgui/*.cpp
    include/*.h
    src/*.cpp
)

add_subdirectory(lib/assimp)

# Require GL
find_package(OpenGL REQUIRED)

# Add include directories
include_directories(
    include "lib/assimp/include"
)

# Add library directories
link_directories(
    lib
)


 

# Add executable
add_executable(${PROJECT_NAME} ${SOURCES}  "include/imgui.h" "include/imgui_impl_opengl3.h" "include/imconfig.h" "include/stb_image.h" "include/imgui_impl_opengl3_loader.h" "include/imgui_impl_win32.h" "include/imgui_impl_glfw.h" "src/imgui_impl_glfw.cpp" "src/Keyframe.cpp" "include/Keyframe.h" "include/Channel.h" "include/Animation.h" "src/Animation.cpp" "src/Channel.cpp" "src/Player.cpp" "include/Player.h" "src/Object.cpp" "include/Object.h" "src/Scene.cpp" "include/Scene.h" "include/Skybox.h" "src/Skybox.cpp" "include/UIManager.h" "src/UIManager.cpp" "include/Global.h" "src/UIImg.cpp" "include/UIImg.h")

# Link libraries
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} glew32s.lib glfw3 assimp)


# Move assets to .exe
add_custom_target(CopyShaders ALL
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	"${PROJECT_SOURCE_DIR}/shaders"
	"${CMAKE_BINARY_DIR}/shaders"
)
add_dependencies(menv CopyShaders)

