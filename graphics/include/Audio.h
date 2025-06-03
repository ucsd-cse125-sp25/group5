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
	void Update(Camera* cam, UIData &p);
	void PlayAudio(std::string n, glm::vec3 pos, float volume);
	void StopAudio();
	void Filter(bool enable);
	GamePhase phase;
	int selfState = 0; //1 = lost, 2 = won
private:
	bool isAlive = true;
	bool isUnderwater;
	bool dealtDamage = false;
	bool lastState = true; //false = dead, true = alive
	bool lastFlagState = false; //false = not have
	bool decision = false;
	bool timeOut = false;
	FMOD::System* system = nullptr;
	FMOD_VECTOR listenerPos;
	FMOD_VECTOR listenerVel;
	FMOD_VECTOR forward;
	FMOD_VECTOR up;
	FMOD::Channel* musicChannel = nullptr;
	FMOD::ChannelGroup* masterGroup = nullptr;
	FMOD::ChannelGroup* mainGroup = nullptr;
	FMOD::ChannelGroup* sfxGroup = nullptr;
	FMOD::ChannelGroup* attackGroup = nullptr;
	FMOD::DSP* lowpassDSP = nullptr;
	float hitStart;
	float deathStart;
};