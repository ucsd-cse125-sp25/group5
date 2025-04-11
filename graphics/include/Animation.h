#ifndef ANIMATION_H
#define ANIMATION_H

#include <vector>
#include "Channel.h"


class Animation {
public:
    Animation();
    float startTime;
    float stopTime;
    int channelCount;
    std::vector<Channel> channels;
    bool Load(const char* file);
    bool animate;
    void doAnimation();
};

#endif