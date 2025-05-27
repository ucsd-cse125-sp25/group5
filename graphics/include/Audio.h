#pragma once
#include <fmod.hpp>
#include <fmod_errors.h>
#include <unordered_map>
#include <vector>
#include <iostream>
#include "Camera.h"

class Audio {
public:
	void Init();
	void Update(Camera* cam);
	void PlayAudio(std::string n, glm::vec3 pos);
	void StopAudio();
private:
	FMOD::System* system;
	FMOD_VECTOR listenerPos;
	FMOD_VECTOR listenerVel;
	FMOD_VECTOR forward;
	FMOD_VECTOR up;
	FMOD::Channel* musicChannel;
	
};