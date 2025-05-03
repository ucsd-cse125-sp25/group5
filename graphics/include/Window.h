#pragma once

#ifndef WINDOW_DEF
#define WINDOW_DEF
#include "Camera.h"
#include "Cube.h"
#include "Shader.h"
#include "core.h"
#include "Skeleton.h"
#include "Skin.h"
#include <vector>
#include <string>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Animation.h"
#include "Player.h"
#include "network/ClientGame.h"
#include <vector>

// Objects to render



class Window {
public:
    // Window Properties
    static int width;
    static int height;
    static const char* windowTitle;

    static Cube* cube;


    static std::vector<Cube*> cubes; // Use std::vector instead of just vector
    //the client
    static ClientGame* client;

    // Shader Program
    static GLuint shaderProgram;

    // Act as Constructors and desctructors
    static bool initializeProgram();
    static void cleanUp();

    // for the Window
    static GLFWwindow* createWindow(int width, int height, ClientGame* _client);
    static void resizeCallback(GLFWwindow* window, int width, int height);

    // update and draw functions
    static void idleCallback();
    static void displayCallback(GLFWwindow*);

    // helper to reset the camera
    static void resetCamera();

    // callbacks - for interaction
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouse_callback(GLFWwindow* window, int button, int action, int mods);
    static void cursor_callback(GLFWwindow* window, double currX, double currY);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

    static PlayerIntentPacket PlayerIntent;
};

#endif