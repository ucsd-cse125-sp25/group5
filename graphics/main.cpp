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

Object* obj;

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

    obj = new Object();
    obj->create("../../assets/plant.obj");

    static Skeleton* skel;
    static Skin* skin;
    static Animation* animation;
    skel = new Skeleton();
    skin = new Skin(skel);
    animation = new Animation();
    static Player* player = new Player(skel, animation, std::chrono::steady_clock::now());
    std::vector<char *> Jointnameslist{};

    // Create the GLFW window.
    GLFWwindow* window = Window::createWindow(1200, 900);
    if (!window) exit(EXIT_FAILURE);

    // Our state
    bool show_demo_window = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    bool doJoints = !(argc == 2 && strstr(argv[1], ".skin"));

    // Print OpenGL and GLSL versions.
    print_versions();
    // Setup callbacks.
    setup_callbacks(window);
    // Setup OpenGL settings.
    setup_opengl_settings();

         //Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    std::cout << "Hello after imgui" << std::endl;

    // Initialize the shader program; exit if initialization fails.
    if (!Window::initializeProgram()) exit(EXIT_FAILURE);
    // Initialize objects/pointers for rendering; exit if initialization fails.

    std::cout << argc << std::endl;/*
    std::cout << argv[1] << std::endl;
    if (!Window::initializeObjects(argv[1])) exit(EXIT_FAILURE);*/
    if (argc == 3) {
		if (!Window::initializeObjects(argv[1], argv[2], skel, skin)) exit(EXIT_FAILURE);
	}
    else if (argc == 2) {
		if (!Window::initializeObjects(argv[1], skel, skin)) exit(EXIT_FAILURE);
    }
    else if (argc == 4) {
        if (!Window::initializeObjects(argv[1], argv[2], argv[3], skel, skin, player)) exit(EXIT_FAILURE);
    }
	else {
		if (!Window::initializeObjects("../../include/wasp.skel", "../../include/wasp.skin", 
            "../../include/wasp_walk/wasp/wasp_dance.anim", skel, skin, player)) exit(EXIT_FAILURE);
		//if (!Window::initializeObjects("../../../include/wasp.skin", skel, skin)) exit(EXIT_FAILURE);
    }

    char** JN;
    int listsize=0;
    //ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    //ImGui_ImplOpenGL3_Init();

    if (doJoints) {
        // Get the list of Joint Names
        Jointnameslist = skel->JNamelist();

         JN = (char**)malloc(Jointnameslist.size() * sizeof(char*));
        // std::cout << "Jointnameslist Size: " << Jointnameslist.size() << std::endl;
        int i = 0;
        listsize = 1;

        for (i = 0; i < Jointnameslist.size(); i++) {
            //std::cout << Jointnameslist[i] << std::endl;
            std::cout << "Inside Loop" << std::endl;

            // JN[i] = new char;
            JN[i] = (char*)malloc(256 * sizeof(char));
            JN[i] = Jointnameslist[i];
            std::cout << "JN of i" << JN[i] << std::endl;
            listsize = i+1;
        }
    }


////pseudocode
//server
//    get inputs
//    update state
//    send state
//client
//    get state
//    render
//    send inputs
//


    // Loop while GLFW window should stay open.
    while (!glfwWindowShouldClose(window)) {

        //std::cout << "3" << std::endl;
        // Main render display callback. Rendering of objects is done here.
        if (!doJoints) {
            JN = argv;
            listsize = 0;
        }
        Window::displayCallback(window, skel, skin, JN, listsize, &io);

        //std::cout << "4" << std::endl;
        // Idle callback. Updating objects, etc. can be done here.
        Window::idleCallback(skel, skin, player);
        //std::cout << "hello?" << std::endl;
    }

    if (doJoints) {

        // Turn off the ImGUI Window Frame
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

    }
    Window::cleanUp(skel, skin);
    // Destroy the window.
    glfwDestroyWindow(window);
    // Terminate GLFW.
    glfwTerminate();

    exit(EXIT_SUCCESS);
}