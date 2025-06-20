#ifndef CHANNEL_H
#define CHANNEL_H

#include <vector>
#include "Keyframe.h"
#include "Tokenizer.h"

enum ExtrapolationMode {
    CONSTANT,
    LINEAR,
    CYCLE,
    CYCLE_OFFSET,
    BOUNCE
};

class Channel {
public:
    std::vector<Keyframe> keyframes;
    std::vector<QuatKeyframe> quats;
    ExtrapolationMode extrapolationIn;
    ExtrapolationMode extrapolationOut;

    Channel(ExtrapolationMode inMode = CONSTANT, ExtrapolationMode outMode = CONSTANT)
        : extrapolationIn(inMode), extrapolationOut(outMode) {
    }

    void addKeyframe(const Keyframe& keyframe) {
        keyframes.push_back(keyframe);
    }

    void addQuatKeyframe(const QuatKeyframe& quatframe) {
        quats.push_back(quatframe);
    }

    float getValue(float time, float duration, float valueChange) const; 

    glm::quat getQuatVal(float time);

    bool Load(Tokenizer& tokenizer);
    void precalculate();

private:
    float evaluateCubic(float t, const Keyframe& kf) const;
    float getValueInWindow(float time) const;
};

#endif // CHANNEL_H
