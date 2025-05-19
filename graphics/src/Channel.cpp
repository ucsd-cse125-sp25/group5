#include "Channel.h"
#include <algorithm>
#include <string>
#include <iostream>

float Channel::evaluateCubic(float t, const Keyframe& kf) const {
    return kf.a * t * t * t + kf.b * t * t + kf.c * t + kf.d;
}

bool Channel::Load(Tokenizer& tokenizer) {
    if (!tokenizer.FindToken("extrapolate")) return false;
    char token[64];
    tokenizer.GetToken(token);
    if (std::string(token) == "constant") extrapolationIn = CONSTANT;
    else if (std::string(token) == "linear") extrapolationIn = LINEAR;
    else if (std::string(token) == "cycle") extrapolationIn = CYCLE;
    else if (std::string(token) == "cycle_offset") extrapolationIn = CYCLE_OFFSET;
    else if (std::string(token) == "bounce") extrapolationIn = BOUNCE;

    tokenizer.GetToken(token);
    if (std::string(token) == "constant") extrapolationOut = CONSTANT;
    else if (std::string(token) == "linear") extrapolationOut = LINEAR;
    else if (std::string(token) == "cycle") extrapolationOut = CYCLE;
    else if (std::string(token) == "cycle_offset") extrapolationOut = CYCLE_OFFSET;
    else if (std::string(token) == "bounce") extrapolationOut = BOUNCE;

    if (!tokenizer.FindToken("keys")) return false;
    int numKeys = tokenizer.GetInt();

    tokenizer.FindToken("{");
    for (int i = 0; i < numKeys; ++i) {
        float time = tokenizer.GetFloat();
        float value = tokenizer.GetFloat();
        char ruleInStr[32];
        char ruleOutStr[32];
        tokenizer.GetToken(ruleInStr);
        tokenizer.GetToken(ruleOutStr);

        float tangentIn = 0.0f;
        float tangentOut = 0.0f;
        char ruleIn = ruleInStr[0];
        char ruleOut = ruleOutStr[0];

        if (std::isdigit(ruleInStr[0]) || ruleInStr[0] == '-' || ruleInStr[0] == '+') {
            tangentIn = std::stof(ruleInStr);
            ruleIn = 'n'; // 'n' for numeric
        }

        if (std::isdigit(ruleOutStr[0]) || ruleOutStr[0] == '-' || ruleOutStr[0] == '+') {
            tangentOut = std::stof(ruleOutStr);
            ruleOut = 'n'; // 'n' for numeric
        }

        Keyframe keyframe(time, value, tangentIn, tangentOut, ruleIn, ruleOut);
        addKeyframe(keyframe);
    }
    precalculate();

    return true;
}

void Channel::precalculate() {
    //for (size_t i = 0; i < keyframes.size(); ++i) {
    //    Keyframe& kf = keyframes[i];

    //    if (kf.ruleIn == 'f') { // flat
    //        kf.tangentIn = 0.0f;
    //    }
    //    else if (kf.ruleIn == 'l' || i == (keyframes.size()-1) ) { // linear
    //        if (i > 0) {
    //            kf.tangentIn = (kf.value - keyframes[i - 1].value) / (kf.time - keyframes[i - 1].time);
    //        }
    //    }
    //    else if (kf.ruleIn == 's') { // smooth
    //        if (i > 0 && i < keyframes.size() - 1) {
    //            float prevValue = keyframes[i - 1].value;
    //            float prevTime = keyframes[i - 1].time;
    //            float nextValue = keyframes[i + 1].value;
    //            float nextTime = keyframes[i + 1].time;
    //            kf.tangentIn = (nextValue - prevValue) / (nextTime - prevTime);
    //        }
    //    }

    //    if (kf.ruleOut == 'f') { // flat
    //        kf.tangentOut = 0.0f;
    //    }
    //    else if (kf.ruleOut == 'l' || i == 0 ) { // linear
    //        if (i < keyframes.size() - 1) {
    //            kf.tangentOut = (keyframes[i + 1].value - kf.value) / (keyframes[i + 1].time - kf.time);
    //        }
    //    }
    //    else if (kf.ruleOut == 's') { // smooth
    //        if (i > 0 && i < keyframes.size() - 1) {
    //            float prevValue = keyframes[i - 1].value;
    //            float prevTime = keyframes[i - 1].time;
    //            float nextValue = keyframes[i + 1].value;
    //            float nextTime = keyframes[i + 1].time;
    //            kf.tangentOut = (nextValue - prevValue) / (nextTime - prevTime);
    //        }
    //    }
    //}

    //for (size_t i = 0; i < keyframes.size() - 1; ++i) {
    //    keyframes[i].computeCoefficients(keyframes[i + 1].value, keyframes[i + 1].time);
    //}
    for (size_t i = 0; i < keyframes.size(); ++i) {
        Keyframe& kf = keyframes[i];

        if (kf.ruleIn == 'f') { // flat
            kf.tangentIn = 0.0f;
        }
        else if (kf.ruleIn == 'l' || i == (keyframes.size() - 1)) { // linear
            if (i > 0) {
                kf.tangentIn = (kf.value - keyframes[i - 1].value) / (kf.time - keyframes[i - 1].time);
            }
        }
        else if (kf.ruleIn == 's') { // smooth
            if (i > 0 && i < keyframes.size() - 1) {
                float prevValue = keyframes[i - 1].value;
                float prevTime = keyframes[i - 1].time;
                float nextValue = keyframes[i + 1].value;
                float nextTime = keyframes[i + 1].time;
                kf.tangentIn = (nextValue - prevValue) / (nextTime - prevTime);
            }
        }

        if (kf.ruleOut == 'f') { // flat
            kf.tangentOut = 0.0f;
        }
        else if (kf.ruleOut == 'l' || i == 0) { // linear
            if (i < keyframes.size() - 1) {
                kf.tangentOut = (keyframes[i + 1].value - kf.value) / (keyframes[i + 1].time - kf.time);
            }
        }
        else if (kf.ruleOut == 's') { // smooth
            if (i > 0 && i < keyframes.size() - 1) {
                float prevValue = keyframes[i - 1].value;
                float prevTime = keyframes[i - 1].time;
                float nextValue = keyframes[i + 1].value;
                float nextTime = keyframes[i + 1].time;
                kf.tangentOut = (nextValue - prevValue) / (nextTime - prevTime);
            }
        }
    }

    for (size_t i = 0; i < keyframes.size() - 1; ++i) {
        keyframes[i].computeCoefficients(keyframes[i + 1].value, keyframes[i + 1].time, keyframes[i+1].tangentIn);
    }
}

float Channel::getValue(float time, float duration, float valueChange) const {
    if (keyframes.empty()) return 0.0f;

    // Handle extrapolation before the first keyframe
    if (time <= keyframes.front().time) {
        switch (extrapolationIn) {
        case CONSTANT:
            return keyframes.front().value;
        case LINEAR:
            return keyframes.front().value + (time - keyframes.front().time) * keyframes.front().tangentIn;
        case CYCLE:
            time = fmod(time - keyframes.front().time, duration) + keyframes.front().time;
            break;
        case CYCLE_OFFSET: {
            float distanceOffset = valueChange * floor((time - keyframes.back().time) / duration);
            time = fmod(time - keyframes.front().time, duration) + keyframes.front().time;
            return getValueInWindow(time) + distanceOffset;
        }
        case BOUNCE:
            if (static_cast<int>(floor((time - keyframes.front().time) / duration)) % 2 == 0) {
                time = fmod(time - keyframes.front().time, duration) + keyframes.front().time;
            }
            else {
                time = keyframes.back().time - fmod(time - keyframes.front().time, duration);
            }
            break;
        default:
            return keyframes.front().value;
        }
    }

    // Handle extrapolation after the last keyframe
    if (time >= keyframes.back().time) {
        switch (extrapolationOut) {
        case CONSTANT:
            return keyframes.back().value;
        case LINEAR:
            return keyframes.back().value + (time - keyframes.back().time) * keyframes.back().tangentOut;
        case CYCLE:
            time = fmod(time - keyframes.front().time, duration) + keyframes.front().time;
            break;
        case CYCLE_OFFSET: {
                float distanceOffset = valueChange * floor((time - keyframes.back().time) / duration);
                time = fmod(time - keyframes.front().time, duration) + keyframes.front().time;
                return getValueInWindow(time) + distanceOffset;
            }
        case BOUNCE:
            if (static_cast<int>(floor((time - keyframes.front().time) / duration)) % 2 == 0) {
                time = fmod(time - keyframes.front().time, duration) + keyframes.front().time;
            }
            else {
                time = keyframes.back().time - fmod(time - keyframes.front().time, duration);
            }
            break;
        default:
            return keyframes.back().value;
        }
    }

    return getValueInWindow(time);
}


float Channel::getValueInWindow(float time) const {
    //for (size_t i = 0; i < keyframes.size() - 1; ++i) {
    //    if (time >= keyframes[i].time && time <= keyframes[i + 1].time) {
    //        float u = (time - keyframes[i].time) / (keyframes[i + 1].time - keyframes[i].time); // u = t-t0/(t1-t0)
    //        return evaluateCubic(u, keyframes[i]);
    //    }
    //}
    for (size_t i = 0; i < keyframes.size() - 1; ++i) {
        if (time >= keyframes[i].time && time <= keyframes[i + 1].time) {
            float u = (time - keyframes[i].time) / (keyframes[i + 1].time - keyframes[i].time); // u = t-t0/(t1-t0)
            float value = evaluateCubic(u, keyframes[i]);
            // Debug print statement
            //std::cout << "Interpolating: time=" << time << ", u=" << u << ", value=" << value << std::endl;
            return value;
        }
    }

    return 0.0f;
}

glm::quat Channel::getQuatVal(float time) {
    for (size_t i = 0; i < quats.size() - 1; ++i) {
        if (time >= quats[i].time && time <= quats[i + 1].time) {
            float t = (time - quats[i].time) / (quats[i + 1].time - quats[i].time);
            if (glm::dot(quats[i].quatern, quats[i + 1].quatern) < 0 ) {
                return glm::slerp(-quats[i].quatern, quats[i + 1].quatern, time);
            }
            else {
                return glm::slerp(quats[i].quatern, quats[i + 1].quatern, time);
            }
            return quats[i].quatern;
        }
    }
    if (!quats.empty()) {
        if (time < quats.front().time) return quats.front().quatern;
        return quats.back().quatern;
    }
    return glm::quat(1, 0, 0, 0);
}