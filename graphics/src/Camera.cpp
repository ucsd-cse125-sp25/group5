////////////////////////////////////////
// Camera.cpp
////////////////////////////////////////

#include "Camera.h"

Camera::Camera() {
    Reset();
}
void Camera::Update() {
    // Compute camera world matrix
    glm::mat4 world(1);
    glm::mat4 translate(1);
    translate[3][0] = Pos.x;
    translate[3][1] = Pos.y;
    translate[3][2] = Pos.z;

    world = translate * glm::eulerAngleY(glm::radians(-Azimuth)) * glm::eulerAngleX(glm::radians(-Incline)) * world;

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
}
void Camera::Reset() {
    FOV = 45.0f;
    Aspect = 1.33f;
    NearClip = 0.1f;
    FarClip = 100.0f;

    Distance = 10.0f;
    Azimuth = 0.0f;
    Incline = 20.0f;
    Pos = glm::vec3(0, 0, 2.0);

    sensitivity = 0.6f;
}