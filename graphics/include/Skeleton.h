#pragma once

#ifndef SKEL_DEF
#define SKEL_DEF
#include "core.h"
#include "glm/gtx/euler_angles.hpp"
#include <vector>
#include <string>
#include <unordered_map>

// The Skeleton class provides a simple means to controlling the 3D Skeleton. It could
// be extended to support more interactive controls. Ultimately. the Skeleton sets the
// GL projection and viewing matrices.

class Joint;

class Skeleton {
public:
    Skeleton();
    Joint* root;
    std::vector<char*>  JNamelist();
    std::unordered_map<std::string, int> JNameMap;
    bool doSkeleton = false;
    void doSkel();
    std::vector<Joint*> joints;
    bool Load(const char* file);
    void update();
    void draw(const glm::mat4& viewProjMtx, GLuint shader);
    void updateWorldMat(glm::mat4 newWorldMat);

private:
    glm::mat4 worldMat;
};

#endif