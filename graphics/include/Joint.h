#pragma once

#ifndef JOINT_DEF
#define JOINT_DEF
#include "core.h"
#include "glm/gtx/euler_angles.hpp"
#include "Cube.h"
#include "Skeleton.h"
#include "DOF.h"
#include "Tokenizer.h"
#include <vector>
#include <string>

class Skeleton;

class Joint {
public:
    Joint(Skeleton* jointsskel, Joint * parent);
    Joint* parent;
    char name[256] = "";
    Skeleton* skely;
    void Update(glm::mat4& parent);
    void AddChild(Joint * child);
    void Draw(GLuint shader);
    bool Load(Tokenizer& t);
    glm::mat4 L;
    glm::mat4 W;
    DOF* xDof;
    DOF* yDof;
    DOF* zDof;
    Cube* box;
    glm::mat4 Joint::GetWorldMatrix();

    void setTranslation(const glm::vec3& translation);
    void setRotation(const glm::vec3& rotation);

private:
    glm::vec3 offset;
    glm::vec3 boxmin;
    glm::vec3 boxmax;
    glm::vec2 rotxlimit;
    glm::vec2 rotylimit;
    glm::vec2 rotzlimit;
    glm::vec3 pose;
    std::vector<Joint*> children;
};

#endif