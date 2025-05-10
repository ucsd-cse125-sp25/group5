#pragma once
#include <fmod.hpp>
#include <fmod_errors.h>
#include <unordered_map>
#include <vector>
#include <iostream>

class Audio {
public:
	void Init();
	void Update();
	void PlayAudio(std::string n);
private:
	FMOD::System* system;
	FMOD_VECTOR listenerPos;
	FMOD_VECTOR listenerVel;
	FMOD_VECTOR forward;
	FMOD_VECTOR up;
	//std::vector<FMOD::Sound*> sounds;
	//std::vector<FMOD::Channel*> channels;
	
};