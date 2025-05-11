#include "Animation.h"
#include "core.h"
#include <iostream>
#include <unordered_map>
#include <tuple>
#include <glm/gtc/quaternion.hpp>

std::tuple<double, double, double> QuaternionToEulerXYZ(double x, double y, double z, double w);
float unwrapAngle(float prev, float curr);

Animation::Animation() {
    this->animate = false;
    this->import = false;
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

void Animation::Load(const aiScene* scene, int animIndex) {
    this->import = true;

    std::unordered_map<aiAnimBehaviour, int> mapa;
    mapa.emplace(aiAnimBehaviour_DEFAULT, 2);
    mapa.emplace(aiAnimBehaviour_CONSTANT, 0);
    mapa.emplace(aiAnimBehaviour_LINEAR, 1);
    mapa.emplace(aiAnimBehaviour_REPEAT, 2);

    aiAnimation* anim = scene->mAnimations[animIndex];
    for (int i = 0; i < anim->mNumChannels; i++) {
        names.push_back(anim->mChannels[i]->mNodeName.C_Str());

        for (int r = 0; r < 3; r++) { //iterating over x, y, z rotation
            Channel channel;
            channel.extrapolationIn = static_cast<ExtrapolationMode>(mapa[anim->mChannels[i]->mPreState]);
            channel.extrapolationOut = static_cast<ExtrapolationMode>(mapa[anim->mChannels[i]->mPostState]);
            for (int j = 0; j < anim->mChannels[i]->mNumRotationKeys; j++) {
             
                float time = anim->mChannels[i]->mRotationKeys[j].mTime;
                glm::quat quat(anim->mChannels[i]->mRotationKeys[j].mValue.w, anim->mChannels[i]->mRotationKeys[j].mValue.x, anim->mChannels[i]->mRotationKeys[j].mValue.y, anim->mChannels[i]->mRotationKeys[j].mValue.z);
                glm::vec3 euler = glm::eulerAngles(quat);
                //std::tuple<double, double, double> tup = QuaternionToEulerXYZ(anim->mChannels[i]->mRotationKeys[j].mValue.x, anim->mChannels[i]->mRotationKeys[j].mValue.y, anim->mChannels[i]->mRotationKeys[j].mValue.z, anim->mChannels[i]->mRotationKeys[j].mValue.w);

                float value = 0;
                //switch (r) {
                //case 0: value = static_cast<float>(std::get<0>(tup)); break;
                //case 1: value = static_cast<float>(std::get<1>(tup)); break;
                //case 2: value = static_cast<float>(std::get<2>(tup)); break;
                //default: throw std::out_of_range("Invalid index");
                //}
                
                if (j > 0) {
                    value = unwrapAngle(channel.keyframes[channel.keyframes.size() - 1].value, euler[r]);
                }
                else {
                    value = euler[r];
                }

                char ruleIn = 's';
                char ruleOut = 's';

                if (anim->mChannels[i]->mRotationKeys[j].mInterpolation == aiAnimInterpolation_Linear) {
                    ruleIn = 'l';
                    ruleOut = 'l';
                }
                //std::cout << "printing value: " << value << std::endl;

                Keyframe key(time, value, 0, 0, ruleIn, ruleOut);
                key.import = true;
                key.quatValue = quat;

                channel.addKeyframe(key);
            }
            channel.precalculate();
            channels.push_back(channel);

        }

        
    }
}


std::tuple<double, double, double> QuaternionToEulerXYZ(double x, double y, double z, double w) {
    // Roll (X-axis rotation)
    double sinr_cosp = 2.0 * (w * x + y * z);
    double cosr_cosp = 1.0 - 2.0 * (x * x + y * y);
    double roll = std::atan2(sinr_cosp, cosr_cosp);

    // Pitch (Y-axis rotation)
    double sinp = 2.0 * (w * y - z * x);
    double pitch;
    if (std::abs(sinp) >= 1)
        pitch = std::copysign(AI_MATH_PI_F / 2, sinp); // Use 90 degrees if out of range
    else
        pitch = std::asin(sinp);

    // Yaw (Z-axis rotation)
    double siny_cosp = 2.0 * (w * z + x * y);
    double cosy_cosp = 1.0 - 2.0 * (y * y + z * z);
    double yaw = std::atan2(siny_cosp, cosy_cosp);

    return { roll, pitch, yaw }; // in radians
}

float unwrapAngle(float prev, float curr) {
    float delta = curr - prev;
    while (delta > glm::pi<float>()) delta -= 2 * glm::pi<float>();
    while (delta < -glm::pi<float>()) delta += 2 * glm::pi<float>();
    return prev + delta;
}