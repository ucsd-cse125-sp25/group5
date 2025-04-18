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

// Window Properties
int Window::width;
int Window::height;
const char* Window::windowTitle = "Model Environment";

// Objects to render
//Skeleton* Window::skel;

// Camera Properties
Camera* Cam;

bool doJoints = false;
// Interaction Variables
bool LeftDown, RightDown;
int MouseX, MouseY;

extern Scene* scene;

// Constructors and desctructors
bool Window::initializeProgram() {
    //nothing for now
    return true;
}

//maintain this as model for how to load an animation 
//bool Window::initializeObjects(char* fileOne, char* fileTwo, char* fileThree, Skeleton* skel, Skin* skin, Player* player) {
//    skin->doSkinning();
//    skel->doSkel();
//    player->animation->doAnimation();
//    skel->Load(fileOne);
//    skin->Load(fileTwo);
//    player->animation->Load(fileThree);
//    doJoints = true;
//
//    return true;
//}


void Window::cleanUp() {
    //TODO
}

// for the Window
GLFWwindow* Window::createWindow(int width, int height) {
    // Initialize GLFW.
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return NULL;
    }

    // 4x antialiasing.
    glfwWindowHint(GLFW_SAMPLES, 4);

    // Create the GLFW window.
    GLFWwindow* window = glfwCreateWindow(width, height, windowTitle, NULL, NULL);

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
    MouseX = width/2;
    MouseY = height / 2;

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    // Call the resize callback to make sure things get drawn immediately.
    Window::resizeCallback(window, width, height);

    return window;
}

void Window::resizeCallback(GLFWwindow* window, int width, int height) {
    Window::width = width;
    Window::height = height;
    // Set the viewport size.
    glViewport(0, 0, width, height);

    Cam->SetAspect(float(width) / float(height));
}

// update and draw functions
void Window::idleCallback() {
    // Perform any updates as necessary.
    Cam->Update();
    scene->update();

 //   skel->update();
 //   skin->update();
 //   player->update();
	
}

void Window::displayCallback(GLFWwindow* window) {
    // Clear the color and depth buffers.
    glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//skel->draw(Cam->GetViewProjectMtx(), Window::shaderProgram);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//skin->draw(Cam->GetViewProjectMtx(), Window::shaderProgram);

    scene->draw(Cam);
    // Gets events, including input such as keyboard and mouse or window resizing.
    // if (!io->WantCaptureMouse) {
      //   glfwPollEvents();
    // }
    glfwPollEvents();

    if (doJoints) {
        // (Your code calls glfwPollEvents())
            // ...
            // Start the Dear ImGui frame
    /*    ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();*/
        // ImGui::ShowDemoWindow(); // Show demo window! :)

        static float x = 0.0f;
        static float y = 0.0f;
        static float z = 0.0f;
        static int jnum = 0;

        static float c1 = 0.5f;
        static float c2 = 0.5f;
        static float c3 = 0.5f;

        //{
        //    ImGui::Begin("DOF");                          // Create a window called "Hello, world!" and append into it.

        //    ImGui::Text("Joint Control.");
        //    // Combo Box drop down was not working, ended up using listbox
        //    // ImGui::Combo("Joint", &jnum, &Namelist, 10, 1);
        //    // ImGui::Combo("Joint", &jnum, Namelist, IM_ARRAYSIZE(Namelist));
        //    // ImGui::Combo("Joint", &jnum, it, 3);
        //    ImGui::ListBox("Joint List", &jnum, Namelist, listsize, 3);

        //    //        std::cout << "Jnum: " << jnum << std::endl;
        //     //       std::cout << "Skel Joint Name: " << skel->joints[jnum]->name << std::endl;
        //      //      skel->joints.at(jnum)->xDof->SetValue(20.0f);
        //       //     std::cout << "Skel xDof: " << skel->joints[jnum]->xDof->GetValue() << std::endl;
        //        //    std::cout << "Skel yDof: " << skel->joints[jnum]->yDof->GetValue() << std::endl;
        //         //   std::cout << "Skel zDof: " << skel->joints[jnum]->zDof->GetValue() << std::endl;

        //          //  skel->joints[jnum]->zDof->SetValue((float) 20.0);
        //           // skel->joints.at(jnum)->xDof->SetValue(20.0f);

        //            // Sliders
        //            // ImGui::SliderFloat("Degree of Freedom X", &skel->joints[jnum]->xDof, 0.0f, 1.0f);
        //            // ImGui::SliderFloat("Degree of Freedom X", &x, (float) skel->joints.at(jnum)->xDof->min(), skel->joints[jnum]->xDof->min());

        //    ImGui::SliderFloat("Degree of Freedom X", &skel->joints[jnum]->xDof->value, (float)skel->joints[jnum]->xDof->min, (float)skel->joints[jnum]->xDof->max);
        //    ImGui::SliderFloat("Degree of Freedom Y", &skel->joints[jnum]->yDof->value, (float)skel->joints[jnum]->yDof->min, (float)skel->joints[jnum]->yDof->max);
        //    ImGui::SliderFloat("Degree of Freedom Z", &skel->joints[jnum]->zDof->value, (float)skel->joints[jnum]->zDof->min, (float)skel->joints[jnum]->zDof->max);
        //    ImGui::SliderFloat("Red ", &c1, 0.0f, 1.0f);
        //    ImGui::SliderFloat("Green ", &c2, 0.0f, 1.0f);
        //    ImGui::SliderFloat("Blue ", &c3, 0.0f, 1.0f);

        //    // skin->tri->color = glm::vec3(c1, c2, c3);
        //    skin->tri[jnum].color = glm::vec3(c1, c2, c3);
        //    skel->joints[jnum]->box->color = glm::vec3(c1, c2, c3);

        //    ImGui::End();
        //}



        // Rendering
        // (Your code clears your framebuffer, renders your other stuff etc.)
        /*ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());*/
        // (Your code calls glfwSwapBuffers() etc.)

    }
        // Swap buffers.
        glfwSwapBuffers(window);
}

// helper to reset the camera
void Window::resetCamera() {
    Cam->Reset();
    Cam->SetAspect(float(Window::width) / float(Window::height));
}

// callbacks - for Interaction
void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    /*
     * TODO: Modify below to add your key callbacks.
     */

    // Check for a key press.
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_ESCAPE:
                // Close the window. This causes the program to also terminate.
                glfwSetWindowShouldClose(window, GL_TRUE);
                break;

            case GLFW_KEY_R:
                resetCamera();
                break;

            default:
                break;
        }
    }
}

void Window::mouse_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        LeftDown = (action == GLFW_PRESS);
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        RightDown = (action == GLFW_PRESS);
    }
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
    Cam->SetAzimuth(Cam->GetAzimuth() + dx * rate);
    Cam->SetIncline(glm::clamp(Cam->GetIncline() - dy * rate, -90.0f, 90.0f));
    //}
    
    //if (RightDown) {
    //    const float rate = 0.005f;
    //    float dist = glm::clamp(Cam->GetDistance() * (1.0f - dx * rate), 0.01f, 1000.0f);
    //    Cam->SetDistance(dist);
    //}
}