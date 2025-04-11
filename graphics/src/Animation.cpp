#include "Animation.h"
#include <iostream>

Animation::Animation() {
    this->animate = false;
}

bool Animation::Load(const char* file) {
    if (!animate) return false;

    Tokenizer tokenizer;
    if (!tokenizer.Open(file)) {
        std::cerr << "Failed to open file: " << file << std::endl;
        return false;
    }

    if (!tokenizer.FindToken("animation")) {
        std::cerr << "Failed to find animation token" << std::endl;
        return false;
    }
    tokenizer.FindToken("range");
    this->startTime = tokenizer.GetFloat();
    this->stopTime = tokenizer.GetFloat();
    tokenizer.FindToken("numchannels");
    channelCount = tokenizer.GetInt();

    channels.resize(channelCount);

    for (int i = 0; i < channelCount; ++i) {
        if (!channels[i].Load(tokenizer)) {
            std::cerr << "Failed to load channel " << i << std::endl;
            return false;
        }
    }

    tokenizer.Close();
    return true;
}


void Animation::doAnimation() {
    this->animate = true;
}