#include "Player.h"
#include "Animation.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include "Skeleton.h"
#include "Joint.h"
#include <iostream>

//ALL ANIMATION CLIP TIME CONSTANTS
long long JUMP_START_RT = 0;
long long JUMP_TOP_RT = 19;
long long JUMP_TOP2_RT = 23;
long long JUMP_END_RT = 38;

long long WALK_START_RT = 80;
long long WALK_CYCLE_START_RT = 90;
long long WALK_CYCLE_END_RT = 120;
long long WALK_END_RT = 130;

float scale = 100.0f;
float walkscale = 10.0f;

Player::Player(Skeleton* skeleton, Animation* animation, std::chrono::steady_clock::time_point realStartTime)
    : skeleton(skeleton), animation(animation) {

    this->realStartTime = std::chrono::high_resolution_clock::now();
    tAdj = 17.0f;
}

void Player::Jump() {
    anim = 0;
    mode = 1;
    tAdj = JUMP_START_RT;
    this->realStartTime = std::chrono::high_resolution_clock::now();
}

void Player::Land() {
    anim = 0;
    mode = 2;
    tAdj = JUMP_TOP_RT;
    this->realStartTime = std::chrono::high_resolution_clock::now();
}

void Player::Walk() {
    anim = 1;
    mode = 1;
    tAdj = WALK_START_RT;
    this->realStartTime = std::chrono::high_resolution_clock::now();

}
void Player::Stop() {
    anim = 1;
    mode = 2;
    tAdj = WALK_CYCLE_END_RT;
    this->realStartTime = std::chrono::high_resolution_clock::now();
}

void Player::update() {
    if (animation->import) {
        //std::cout << "doing anim update" << std::endl;
        if (!skeleton || !animation || !animation->animate) return;

        std::chrono::duration<double> time_clock = std::chrono::high_resolution_clock::now() - realStartTime;
        float time = time_clock.count();
        time = anim == 1 ? (time * scale) + tAdj : (time * walkscale) + tAdj;
        
        //time constraints based on animation state
        if (anim == 0 && mode == 1) {
            if (time > JUMP_TOP_RT) {
                time = JUMP_TOP_RT;
            }
        }
        else if (anim == 0 && mode == 2) {
            if (time > JUMP_END_RT) {
                time = JUMP_END_RT;
            }
        }
        else if (anim == 1 && mode == 1) {
            if (time > WALK_CYCLE_END_RT) {
                this->realStartTime = std::chrono::high_resolution_clock::now();
                tAdj = WALK_CYCLE_START_RT;
            }
        }
        else if (anim == 1 && mode == 2) {
            if (time > WALK_END_RT) {
                time = WALK_END_RT;
            }
        }

        std::cout << "time: " << time << std::endl;

        //std::cout << time << std::endl;
        //75 joints
        //animation->channels.size();

        //i = 252 -  i < 253 is the wonky leg but its fine in the standing pose

        /*
        for (size_t i = 0; i < animation->channels.size(); ++i) {
            std::cout << "Bone name: " << animation->names[i] << std::endl;
            //std::cout << "bone name: " << animation->names[i] << std::endl;
            auto it = skeleton->JNameMap.find(animation->names[i]);
            if (it == skeleton->JNameMap.end()) {
                std::cerr << "Missing joint for: " << animation->names[i] << std::endl;
                continue;
            }
            size_t jointIndex = it->second;

            if (jointIndex >= skeleton->joints.size()) break;

            glm::quat q = animation->channels.at(i).getQuatVal(time);
            skeleton->joints[jointIndex]->useQuat = true;
            skeleton->joints[jointIndex]->currQuat = q;

        }
        */
        for (size_t i = 0; i < animation->channels.size(); i += 6) {
            int nameIndex = i / 6;
            //std::cout << "bone name: " << animation->names[nameIndex] << std::endl;
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
                animation->channels[i + 1].getValue(time, animation->channels[i + 1].keyframes.back().time - animation->channels[i + 1].keyframes.front().time, animation->channels[i + 1].keyframes.back().value - animation->channels[i + 1].keyframes.front().value),
                animation->channels[i + 2].getValue(time, animation->channels[i + 2].keyframes.back().time - animation->channels[i + 2].keyframes.front().time, animation->channels[i + 2].keyframes.back().value - animation->channels[i + 2].keyframes.front().value)
            );
            //rotation.x += glm::pi<float>();
            skeleton->joints[jointIndex]->setRotation(rotation);

            // Translation
            glm::vec3 translation(
                animation->channels[i + 3].getValue(time, animation->channels[i + 3].keyframes.back().time - animation->channels[i + 3].keyframes.front().time, animation->channels[i + 3].keyframes.back().value - animation->channels[i + 3].keyframes.front().value),
                animation->channels[i + 4].getValue(time, animation->channels[i + 4].keyframes.back().time - animation->channels[i + 4].keyframes.front().time, animation->channels[i + 4].keyframes.back().value - animation->channels[i + 4].keyframes.front().value),
                animation->channels[i + 5].getValue(time, animation->channels[i + 5].keyframes.back().time - animation->channels[i + 5].keyframes.front().time, animation->channels[i + 5].keyframes.back().value - animation->channels[i + 5].keyframes.front().value)
            );
            skeleton->joints[jointIndex]->setTranslation(translation);

            //std::cout << "Rotation: " << i << " : " << rotation.x << " " << rotation.y << " " << rotation.z << " + time = " << time << std::endl;
            
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