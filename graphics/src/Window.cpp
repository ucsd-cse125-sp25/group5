#include <iostream>
#include <vector>
#include <string>
#include "core.h"
#include "Window.h"
#include "Skin.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <Object.h>
#include <Scene.h>
#include "Global.h"
#include <vector>
//#include <UIManager.h>

// Window Properties
int Window::width;
int Window::height;
const char* Window::windowTitle = "Model Environment";
double startTime = 0;
double prevTime = 0;
double currTime = 0;
bool flag = false;

std::vector<Cube*> Window::cubes; // Use std::vector instead of just vector

// Camera Properties
Camera* Cam;

bool doJoints = false;
// Interaction Variables
bool LeftDown, RightDown;
int MouseX, MouseY;
bool A_Down, D_Down, W_Down, S_Down;

extern Scene* scene;

ClientGame* Window::client;
PlayerIntentPacket Window::PlayerIntent;
double scrollStart;

// Constructors and desctructors
bool Window::initializeProgram() {
    //cube = new Cube();
    scrollStart = glfwGetTime();
    return true;
}

void Window::cleanUp() {
    //delete cube;
}

// for the Window
GLFWwindow* Window::createWindow(int width, int height, ClientGame* _client) {
    // Initialize GLFW.
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return NULL;
    }

    width = WINDOWWIDTH;
    height = WINDOWHEIGHT;

    // 4x antialiasing.
    glfwWindowHint(GLFW_SAMPLES, 4);

    // Create the GLFW window.

    GLFWmonitor* primary = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primary);

    // glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, windowTitle, NULL, NULL);
    glfwSetWindowPos(window, 0, 0);
    //int WINDOWHEIGHT = mode->height;
    //int WINDOWWIDTH = mode->width;
    // Check if the window could not be created.
    if (!window) {
        std::cerr << "Failed to open GLFW window." << std::endl;
        glfwTerminate();
        return NULL;
    }

    // Make the context of the window.
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewInit();

    // Set swap interval to 1.
    glfwSwapInterval(0);

    // set up the camera
    Cam = new Camera();
    Cam->SetAspect(float(width) / float(height));

    // initialize the interaction variables
    LeftDown = RightDown = false;
    A_Down = D_Down = W_Down = S_Down = false;
  
    MouseX = width / 2;
    MouseY = height / 2;

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);


    glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_callback);
    // Call the resize callback to make sure things get drawn immediately.
    Window::resizeCallback(window, width, height);
    Window::client = _client;

    return window;
}

void Window::resizeCallback(GLFWwindow* window, int width, int height) {
    std::cout << "Resized window" << std::endl;
    Window::width = width;
    Window::height = height;
    WINDOWWIDTH = width;
    WINDOWHEIGHT = height;
    std::cout << WINDOWHEIGHT << std::endl;
    std::cout << WINDOWWIDTH << std::endl;
    // Set the viewport size.
    glViewport(0, 0, width, height);

    Cam->SetAspect(float(width) / float(height));
}

// update and draw functions
void Window::idleCallback() { 
    // Perform any updates as necessary.

    std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
    double time = std::chrono::duration<double, std::milli>(now.time_since_epoch()).count();

    prevTime = currTime;
    currTime = time;

    if (!flag) {
        startTime = time;
        flag = true;
    }
  
    client->update(PlayerIntent);
    Cam->Update(client);
    scene->update();

    //if (PlayerIntent.scrollIntentTriggered) {
    //    PlayerIntent.scrollIntentTriggered = false;
    //    PlayerIntent.scrollDownIntent = false;
    //    PlayerIntent.scrollUpIntent = false;
    //}

    //set scroll
}

void Window::displayCallback(GLFWwindow* window) {
    // Clear the color and depth buffers.
    glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    scene->draw(Cam);
	
    glfwPollEvents();

    glfwSwapBuffers(window);
}

// helper to reset the camera
void Window::resetCamera() {
    Cam->Reset();
    Cam->SetAspect(float(Window::width) / float(Window::height));
}

// callbacks - for Interaction
void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // Check for a key press.
    int BURST = 10;

	PlayerIntent.hit1Intent = false;
	PlayerIntent.hit2Intent = false;
	PlayerIntent.hit3Intent = false;
	PlayerIntent.hit4Intent = false;
	PlayerIntent.hit5Intent = false;
	PlayerIntent.hitEIntent = false;
	PlayerIntent.hitRIntent = false;
	PlayerIntent.hitTIntent = false;
	PlayerIntent.hitYIntent = false;
	PlayerIntent.hitUIntent = false;



    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_ESCAPE:
                // Close the window. This causes the program to also terminate.
                glfwSetWindowShouldClose(window, GL_TRUE);
                break;
			case GLFW_KEY_1:
				PlayerIntent.hit1Intent = true;
				//cube->setColor(1.0f, 0.0f, 0.0f);
				break;
			case GLFW_KEY_2:
				PlayerIntent.hit2Intent = true;
				//cube->setColor(0.0f, 1.0f, 0.0f);
				break;
			case GLFW_KEY_3:
				PlayerIntent.hit3Intent = true;
				//cube->setColor(0.0f, 0.0f, 1.0f);
				break;
			case GLFW_KEY_4:
				PlayerIntent.hit4Intent = true;
				//cube->setColor(1.0f, 1.0f, 0.0f);
				break;
			case GLFW_KEY_5:
				PlayerIntent.hit5Intent = true;
				//cube->setColor(0.0f, 1.0f, 1.0f);
				break;
            //next for the Keys ,E,R,T,Y,U
            case GLFW_KEY_E:
				PlayerIntent.hitEIntent = true;
				//cube->setColor(1.0f, 0.0f, 0.0f); 
				break;
			case GLFW_KEY_R:
				PlayerIntent.hitRIntent = true;
				//cube->setColor(0.0f, 1.0f, 0.0f);
				break;
			case GLFW_KEY_T:
				PlayerIntent.hitTIntent = true;
				//cube->setColor(0.0f, 0.0f, 1.0f);
				break;
			case GLFW_KEY_Y:
				PlayerIntent.hitYIntent = true;
				//cube->setColor(1.0f, 1.0f, 0.0f);
				break;
			case GLFW_KEY_U:
				PlayerIntent.hitUIntent = true;
				//cube->setColor(0.0f, 1.0f, 1.0f);
				break;
      default:
        break;
        }

    }
   

    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) { scene->TriggerAnim(2); } //trigger repawn anim (branch growing back)

    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) { scene->dummy.currHP = scene->dummy.currHP - 20; }
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) { scene->dummy.currHP = scene->dummy.currHP + 20; }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) { scene->uimanager->NextGamePhase(); }
    PlayerIntent.moveLeftIntent = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
    PlayerIntent.moveRightIntent = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;
    PlayerIntent.moveUpIntent = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
    PlayerIntent.moveDownIntent = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
    PlayerIntent.moveForwardIntent = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
    PlayerIntent.moveBackIntent = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
	PlayerIntent.leftClickIntent = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
	PlayerIntent.rightClickIntent = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
	//PlayerIntent.scrollUpIntent = glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS;
	//PlayerIntent.scrollDownIntent = glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS;
	//if (key == GLFW_KEY_W && action == GLFW_PRESS) {
	//    Cam->SetDistance(Cam->GetDistance() - BURST);
	//}
	//if (key == GLFW_KEY_S && action == GLFW_PRESS) {
	//    Cam->SetDistance(Cam->GetDistance() + BURST);
	//}

}

void Window::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{

    double time = glfwGetTime();

    if (time - scrollStart < 0.1) {
        return;
    }
    if (yoffset > 0) {
        PlayerIntent.scrollIntentTriggered = true;
		PlayerIntent.scrollUpIntent = true;
		PlayerIntent.scrollDownIntent = false;

		

	}
    else if(yoffset < 0){
        PlayerIntent.scrollIntentTriggered = true;
		PlayerIntent.scrollDownIntent = true;
		PlayerIntent.scrollUpIntent = false;
    }


	printf("Scroll: %f %f\n", xoffset, yoffset);

    scrollStart = glfwGetTime();
    if (PlayerIntent.scrollDownIntent) { 
        scene->TriggerAnim(0); 
        PlayerIntent.changeToPower = (PowerType)(((int)PlayerIntent.changeToPower + 1) % 5);
    }; //Rotate UI CCW
    if (PlayerIntent.scrollUpIntent) { 
        scene->TriggerAnim(1); 
        PlayerIntent.changeToPower = (PowerType)(((int)(PlayerIntent.changeToPower) - 1 + 5) % 5);
    }; //Rotate UI CW
}

void Window::mouse_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        LeftDown = (action == GLFW_PRESS);
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        RightDown = (action == GLFW_PRESS);
    }
	PlayerIntent.leftClickIntent = LeftDown;
	PlayerIntent.rightClickIntent = RightDown;
	//std::cout << "LeftDown: " << LeftDown << std::endl;
	//std::cout << "RightDown: " << RightDown << std::endl;
	//std::cout << "MouseX: " << MouseX << std::endl;
	//std::cout << "MouseY: " << MouseY << std::endl;
}

void Window::cursor_callback(GLFWwindow* window, double currX, double currY) {
    int maxDelta = 100;
    int dx = Cam->sensitivity * glm::clamp((int)currX - MouseX, -maxDelta, maxDelta);
    int dy = Cam->sensitivity * glm::clamp(-((int)currY - MouseY), -maxDelta, maxDelta);

    //MouseX = (int)currX;
    //MouseY = (int)currY;
    glfwSetCursorPos(window, MouseX, MouseY);

    // Move camera
    // NOTE: this should really be part of Camera::Update()

    //if (LeftDown) {
    const float rate = 1.0f;

    PlayerIntent.azimuthIntent = Cam->GetAzimuth() + dx * rate;
    PlayerIntent.inclineIntent = glm::clamp(Cam->GetIncline() - dy * rate, -90.0f, 90.0f);

    Cam->SetAzimuth(Cam->GetAzimuth() + dx * rate);
    Cam->SetIncline(glm::clamp(Cam->GetIncline() - dy * rate, -90.0f, 90.0f));
    //}
    
    //if (RightDown) {
    //    const float rate = 0.005f;
    //    float dist = glm::clamp(Cam->GetDistance() * (1.0f - dx * rate), 0.01f, 1000.0f);
    //    Cam->SetDistance(dist);
    //}
}