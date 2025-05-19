#ifndef ANIMATION_H
#define ANIMATION_H

#include <vector>
#include "Channel.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


class Animation {
public:
    Animation();
    float startTime;
    float stopTime;
    int channelCount;
    std::vector<Channel> channels;
    std::vector<Channel> positions;
    std::vector<std::string> names;
    bool Load(const char* file);
    bool animate;
    bool import;
    void doAnimation();
    void Load(const aiScene* scene, int animIndex);
};

#endif