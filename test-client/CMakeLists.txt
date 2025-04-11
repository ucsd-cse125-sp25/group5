cmake_minimum_required(VERSION 3.14)
project(OpenGLCube)

set(CMAKE_CXX_STANDARD 17)

# Set default build type
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# FetchContent is the easiest way to grab GLFW, GLAD, GLM
include(FetchContent)

# GLFW
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG latest
)
FetchContent_MakeAvailable(glfw)

# GLAD
FetchContent_Declare(
  glad
  GIT_REPOSITORY https://github.com/Dav1dde/glad.git
  GIT_TAG v0.1.36
)
FetchContent_MakeAvailable(glad)

# GLM (for matrix math)
FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG 0.9.9.8
)
FetchContent_MakeAvailable(glm)

# Define executable
add_executable(OpenGLCube main.cpp)

# Include dirs
target_include_directories(OpenGLCube PRIVATE
  ${glad_SOURCE_DIR}/include
  ${glm_SOURCE_DIR}
)

# Link dependencies
target_link_libraries(OpenGLCube
  glfw
  glad
)

# macOS special case
if(APPLE)
  target_link_libraries(OpenGLCube "-framework OpenGL")
elseif(UNIX)
  target_link_libraries(OpenGLCube dl GL)
elseif(WIN32)
  target_link_libraries(OpenGLCube opengl32)
endif()
