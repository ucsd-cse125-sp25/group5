#include <iostream>
#include <string>
#include <vector>
#include "Window.h"
#include "core.h"
#include "Skin.h"
#include "Animation.h"
#include "Player.h"
#include "Skeleton.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <Object.h>
#include <Scene.h>
#include "network/ClientGame.h"

Scene* scene;

void error_callback(int error, const char* description) {
    // Print error.
    std::cerr << description << std::endl;
}

void setup_callbacks(GLFWwindow* window) {
    // Set the error callback.
    glfwSetErrorCallback(error_callback);
    // Set the window resize callback.
    glfwSetWindowSizeCallback(window, Window::resizeCallback);

    // Set the key callback.
    glfwSetKeyCallback(window, Window::keyCallback);

    // Set the mouse and cursor callbacks
    glfwSetMouseButtonCallback(window, Window::mouse_callback);
    glfwSetCursorPosCallback(window, Window::cursor_callback);
}

void setup_opengl_settings() {
    // Enable depth buffering.
    glEnable(GL_DEPTH_TEST);
    // Related to shaders and z value comparisons for the depth buffer.
    glDepthFunc(GL_LEQUAL);
    // Set polygon drawing mode to fill front and back of each polygon.
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // Set clear color to black.
    glClearColor(0.0, 0.0, 0.0, 0.0);
}

void print_versions() {
    // Get info of GPU and supported OpenGL version.
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "OpenGL version supported: " << glGetString(GL_VERSION)
              << std::endl;

    // If the shading language symbol is defined.
#ifdef GL_SHADING_LANGUAGE_VERSION
    std::cout << "Supported GLSL version is: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
#endif
}

int main(int argc, char* argv[]) {
    std::cout << "Hello World" << std::endl;
    ClientGame* client = new ClientGame();

    //Create the GLFW window.
    GLFWwindow* window = Window::createWindow(1200, 900, client);
    if (!window) exit(EXIT_FAILURE);

    // Our state
    bool show_demo_window = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    //Print OpenGL and GLSL versions.
    print_versions();
    // Setup callbacks.
    setup_callbacks(window);
    // Setup OpenGL settings.
    setup_opengl_settings(); //C

    //Setup Dear ImGui context
    //IMGUI_CHECKVERSION();
    //ImGui::CreateContext();
    //ImGuiIO& io = ImGui::GetIO();
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    //std::cout << "Hello after imgui" << std::endl;

    // Initialize the shader program; exit if initialization fails.
    if (!Window::initializeProgram()) exit(EXIT_FAILURE);

    scene = new Scene();
    scene->initShaders();
    scene->createGame();
    scene->loadObjects();
    // Loop while GLFW window should stay open.
    while (!glfwWindowShouldClose(window)) {
        Window::displayCallback(window);
        Window::idleCallback();
    } //C
    Window::cleanUp();
    // Destroy the window.
    glfwDestroyWindow(window);
    //Terminate GLFW.
    glfwTerminate();

    exit(EXIT_SUCCESS);
}