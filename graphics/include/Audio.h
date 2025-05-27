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
	void Filter();
private:
	FMOD::System* system = nullptr;
	FMOD_VECTOR listenerPos;
	FMOD_VECTOR listenerVel;
	FMOD_VECTOR forward;
	FMOD_VECTOR up;
	FMOD::Channel* musicChannel = nullptr;
	FMOD::ChannelGroup* mainGroup = nullptr;
	FMOD::DSP* lowpassDSP = nullptr;
	
};