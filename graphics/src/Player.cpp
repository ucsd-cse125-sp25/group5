#include "Player.h"
#include "Animation.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include "Skeleton.h"
#include "Joint.h"
#include <iostream>

Player::Player(Skeleton* skeleton, Animation* animation, std::chrono::steady_clock::time_point realStartTime)
    : skeleton(skeleton), animation(animation) {

    this->realStartTime = std::chrono::high_resolution_clock::now();
}

void Player::update() {
    if (animation->import) {
        std::cout << "doing anim update" << std::endl;
        if (!skeleton || !animation || !animation->animate) return;

        std::chrono::duration<double> time_clock = std::chrono::high_resolution_clock::now() - realStartTime;
        float time = time_clock.count();
        time = time * 1000.0f/60.0f;

        for (size_t i = 0; i < animation->channels.size(); i += 3) {
            int nameIndex = i / 3;
            auto it = skeleton->JNameMap.find(animation->names[nameIndex]);
            if (it == skeleton->JNameMap.end()) {
                std::cerr << "Missing joint for: " << animation->names[nameIndex] << std::endl;
                continue;
            }
            size_t jointIndex = it->second;
            //std::cout << "jointindex: " << jointIndex << std::endl;

            //jointIndex++;
            if (jointIndex >= skeleton->joints.size()) break;

            glm::vec3 rotation(
                animation->channels[i].getValue(time, animation->channels[i].keyframes.back().time - animation->channels[i].keyframes.front().time, animation->channels[i].keyframes.back().value - animation->channels[i].keyframes.front().value),
                animation->channels[i + 1].getValue(time, animation->channels[i+1].keyframes.back().time - animation->channels[i+1].keyframes.front().time, animation->channels[i+1].keyframes.back().value - animation->channels[i+1].keyframes.front().value),
                animation->channels[i + 2].getValue(time, animation->channels[i+2].keyframes.back().time - animation->channels[i+2].keyframes.front().time, animation->channels[i+2].keyframes.back().value - animation->channels[i+2].keyframes.front().value)
            );

            skeleton->joints[jointIndex]->setRotation(rotation);

            if (i < 30) {

                std::cout << "Rotation: " << i << " : " << rotation.x << " " << rotation.y << " " << rotation.z << " + time = " << time << std::endl;
            }
            
        }

        //std::cout << "Number of Animation Channels" << animation->channels.size() << std::endl;
    }
    else {
        if (!skeleton || !animation || !animation->animate) return;

        std::chrono::duration<double> time_clock = std::chrono::high_resolution_clock::now() - realStartTime;
        float time = time_clock.count();
        // Use the first three channels to translate the root node
        glm::vec3 translation(
            animation->channels[0].getValue(time, animation->channels[0].keyframes.back().time - animation->channels[0].keyframes.front().time, animation->channels[0].keyframes.back().value - animation->channels[0].keyframes.front().value),
            animation->channels[1].getValue(time, animation->channels[1].keyframes.back().time - animation->channels[1].keyframes.front().time, animation->channels[1].keyframes.back().value - animation->channels[1].keyframes.front().value),
            animation->channels[2].getValue(time, animation->channels[2].keyframes.back().time - animation->channels[2].keyframes.front().time, animation->channels[2].keyframes.back().value - animation->channels[2].keyframes.front().value)
        );

        //skeleton->root->setTranslation(translation);
        skeleton->root->setTranslation(glm::vec3(0));
        // Use the rest of the channels to update the joint rotations
        for (size_t i = 6; i < animation->channels.size(); i += 3) {
            size_t jointIndex = (i - 3) / 3;
            //jointIndex++;
            if (jointIndex >= skeleton->joints.size()) break;

            glm::vec3 rotation(
                animation->channels[i].getValue(time, animation->channels[i].keyframes.back().time - animation->channels[i].keyframes.front().time, animation->channels[i].keyframes.back().value - animation->channels[i].keyframes.front().value),
                animation->channels[i + 1].getValue(time, animation->channels[i].keyframes.back().time - animation->channels[i].keyframes.front().time, animation->channels[i].keyframes.back().value - animation->channels[i].keyframes.front().value),
                animation->channels[i + 2].getValue(time, animation->channels[i].keyframes.back().time - animation->channels[i].keyframes.front().time, animation->channels[i].keyframes.back().value - animation->channels[i].keyframes.front().value)
            );

            skeleton->joints[jointIndex]->setRotation(rotation);
        }
    }
    
}