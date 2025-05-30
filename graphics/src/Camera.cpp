////////////////////////////////////////
// Camera.cpp
////////////////////////////////////////

#include "Camera.h"
#include "network/ClientGame.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>

Camera::Camera(ClientGame* client) {
    this->client = client;
    Reset();
    Rot = glm::mat4(1.0f);
}

void Camera::Update() {
    if (client->GameState.phase != GamePhase::IN_GAME) {
        Pos = glm::vec3(0, 0, 2.0);
        glm::mat4 world(1);
        glm::mat4 translate(1);
        //0.0008f
        Rot = glm::rotate(Rot, 0.01f, glm::vec3(0.0f, 1.0f, 0.0f));

       // world = translate * glm::eulerAngleY(glm::radians(-Azimuth)) * translateAugment * glm::eulerAngleX(glm::radians(-Incline)) * world;

        world = Rot;// *glm::eulerAngleX(glm::radians(-Incline));
        //world[3] = glm::vec4(0.0f, 2.0f, -10.0f, 1.0f);
        world = glm::translate(Rot, glm::vec3(0.0f, 12.0f, 32.0f)) * glm::eulerAngleX(glm::radians(-22.0f));
        
        //std::cout << "Camera position!" << glm::to_string(world[3]) << std::endl;
        //world[3][2] = Distance;
        //world = glm::eulerAngleY(glm::radians(-Azimuth)) * glm::eulerAngleX(glm::radians(-Incline)) * world;

        // Compute view matrix (inverse of world matrix)
        glm::mat4 view = glm::inverse(world);

        // Compute perspective projection matrix
        glm::mat4 project = glm::perspective(glm::radians(FOV), Aspect, NearClip, FarClip);

        // Compute final view-projection matrix
        ViewProjectMtx = project * view;
        ViewMtx = view;
        ProjMtx = project;

        //std::cout << "CAM POSITION" << glm::to_string(GetPosition()) << std::endl;
    } else {
        // Compute camera world matrix
        glm::mat4 world(1);
        glm::mat4 translate(1);

        Pos = glm::vec3(client->playerModel[3]);
        std::cout << "cam pos: " << Pos.x << " " << Pos.y << " " << Pos.z << std::endl;
        translate[3][0] = Pos.x;
        translate[3][1] = Pos.y;
        translate[3][2] = Pos.z;

        glm::mat4 translateAugment(1);
        ////translateAugment[3][0] =  0.0f;
        ////translateAugment[3][1] =  1.5f;
        ////translateAugment[3][2] =  2.0f;

        world = translate * glm::eulerAngleY(glm::radians(-Azimuth)) * translateAugment * glm::eulerAngleX(glm::radians(-Incline)) * world;

        //std::cout << "Camera position!" << glm::to_string(world[3]) << std::endl;
        //world[3][2] = Distance;
        //world = glm::eulerAngleY(glm::radians(-Azimuth)) * glm::eulerAngleX(glm::radians(-Incline)) * world;

        // Compute view matrix (inverse of world matrix)
        glm::mat4 view = glm::inverse(world);

        // Compute perspective projection matrix
        glm::mat4 project = glm::perspective(glm::radians(FOV), Aspect, NearClip, FarClip);

        // Compute final view-projection matrix
        ViewProjectMtx = project * view;
        ViewMtx = view;
        ProjMtx = project;

        //std::cout << "CAM POSITION" << glm::to_string(GetPosition()) << std::endl;
    }
}
void Camera::Reset() {
    FOV = 80.0f;
    Aspect = 1.33f;
    NearClip = 0.1f;
    FarClip = 100.0f;

    Distance = 10.0f;
    Azimuth = 0.0f;
    Incline = 20.0f;
    Pos = glm::vec3(0, 0, 2.0);

    sensitivity = 0.6f;
}

glm::vec3 Camera::GetCameraForwardVector() {
    glm::mat4 camWorld = glm::inverse(ViewMtx); // Convert view -> world
    glm::vec3 forward = -glm::normalize(glm::vec3(camWorld[2])); // -Z axis
    return forward;
}