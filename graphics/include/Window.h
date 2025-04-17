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


// Objects to render



class Window {
public:
    // Window Properties
    static int width;
    static int height;
    static const char* windowTitle;

    static Cube* cube;

    //the client
	static ClientGame* client;

    // Shader Program
    static GLuint shaderProgram;

    // Act as Constructors and desctructors
    static bool initializeProgram();
    static bool initializeObjects(char* fileOne, char* fileTwo, char* fileThree, Skeleton* skel, Skin* skin, Player * player);
	static bool initializeObjects(char* fileOne, char* fileTwo, Skeleton* skel, Skin* skin);
    static bool initializeObjects(char * file, Skeleton* skel, Skin* skin);
    static void cleanUp(Skeleton* skel, Skin* skin);

    // for the Window
    static GLFWwindow* createWindow(int width, int height, ClientGame* _client);
    static void resizeCallback(GLFWwindow* window, int width, int height);

    // update and draw functions
    static void idleCallback(Skeleton* skel, Skin* skin, Player * player);
    static void displayCallback(GLFWwindow*, Skeleton* skel, Skin* skin,char* Namelist[],int listsize,  ImGuiIO* io);

    // helper to reset the camera
    static void resetCamera();

    // callbacks - for interaction
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouse_callback(GLFWwindow* window, int button, int action, int mods);
    static void cursor_callback(GLFWwindow* window, double currX, double currY);

    static PlayerIntentPacket PlayerIntent;
};

#endif