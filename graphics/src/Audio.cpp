#include <Audio.h>
#include "Camera.h"
#include <glm/gtx/string_cast.hpp>

//Name the audio file will be reference by to play and the path of the audio file
static std::unordered_map<std::string, std::string> AudioFiles = {
	{"fireA", PROJECT_SOURCE_DIR + std::string("/assets/audiofiles/fireA.wav")},
	{"fireM", PROJECT_SOURCE_DIR + std::string("/assets/audiofiles/fireM.wav")},
	{"waterA", PROJECT_SOURCE_DIR + std::string("/assets/audiofiles/waterA.wav")},
	{"waterM", PROJECT_SOURCE_DIR + std::string("/assets/audiofiles/waterM.wav")},
	{"earthA", PROJECT_SOURCE_DIR + std::string("/assets/audiofiles/earthA.wav")},
	{"earthM", PROJECT_SOURCE_DIR + std::string("/assets/audiofiles/earthM.wav")},
	{"death", PROJECT_SOURCE_DIR + std::string("/assets/audiofiles/death.wav")},
	{"defeat", PROJECT_SOURCE_DIR + std::string("/assets/audiofiles/defeat.wav")},
	{"victory", PROJECT_SOURCE_DIR + std::string("/assets/audiofiles/victory.wav")},
	{"hit", PROJECT_SOURCE_DIR + std::string("/assets/audiofiles/hit.wav")},
	{"respawn", PROJECT_SOURCE_DIR + std::string("/assets/audiofiles/respawn.wav")},
	{"ticktock", PROJECT_SOURCE_DIR + std::string("/assets/audiofiles/ticktock.wav")},
	{"capture", PROJECT_SOURCE_DIR + std::string("/assets/audiofiles/capture.wav")}
};

static std::unordered_map<std::string, FMOD::Sound*> Sounds;
//static std::unordered_map<std::string, std::tuple<FMOD::Sound*, FMOD::Channel*>> Engine;


//Loop through AudioFiles hash map and create Sound* placed in Sounds hashmap
void Audio::Init() {
	hitStart = glfwGetTime();
	deathStart = glfwGetTime();
	FMOD::System_Create(&system);
	system->init(512, FMOD_INIT_3D_RIGHTHANDED, nullptr);

	//Load all the files and put them in a map
	for (const auto& pair : AudioFiles) {
		const std::string& track = pair.first;
		const std::string& path = pair.second;
		FMOD::Sound* s;
		system->createSound(path.c_str(), FMOD_3D, 0, &s);
		s->setMode(FMOD_LOOP_OFF);
		Sounds[track] = s;
	}
	listenerPos = { 0.0f, 0.0f, 0.0f };
	listenerVel = { 0.0f, 0.0f, 0.0f };
	forward = { 0.0f, 0.0f, 1.0f };
	up = { 0.0f, 1.0f, 0.0f };
	system->set3DListenerAttributes(0, &listenerPos, &listenerVel, &forward, &up);
	system->set3DSettings(1.0f, 1.0f, 1.0f);
	system->getMasterChannelGroup(&mainGroup);

	system->createDSPByType(FMOD_DSP_TYPE_LOWPASS, &lowpassDSP);
	lowpassDSP->setParameterFloat(FMOD_DSP_LOWPASS_CUTOFF, 750.0f);
	lowpassDSP->setBypass(true);
	mainGroup->addDSP(0, lowpassDSP);
}

//Given name of audio, from AudioFiles hash map, place audio track into channel and then play it
//Also pass in the position for spatial audio
void Audio::PlayAudio(std::string n, glm::vec3 pos) {
	std::cout << "Going to play audio at: " << glm::to_string(pos) << std::endl;
	FMOD_VECTOR soundPos = {pos.x, pos.y, pos.z};
	FMOD_VECTOR soundVel = { 0.0f, 0.0f, 0.0f };
	FMOD::Channel* channel = nullptr;
	system->playSound(Sounds[n], nullptr, true, &channel);
	channel->set3DAttributes(&soundPos, &soundVel);
	channel->setPaused(false);  // Unpause to start playing
	channel->set3DMinMaxDistance(1.0f, 100.0f);
}

//Can only stop the background music
void Audio::StopAudio() {
	musicChannel->stop();
}

//FMOD::System* automatically handles playing audio
void Audio::Update(Camera* cam, UIData &p) {
	glm::vec3 pos = cam->GetPosition();
	glm::vec3 f = glm::normalize(cam->GetCameraForwardVector());
	//Set the position up and forward
	listenerPos = { pos.x, pos.y, pos.z };
	listenerVel = { 0.0f, 0.0f, 0.0f };
	forward = { f.x, f.y, f.z };
	up = { 0.0f, 1.0f, 0.0f };
	system->set3DListenerAttributes(0, &listenerPos, &listenerVel, &forward, &up);
	system->update();
	if (isAlive && p.currHP <= 0) {
		std::cout << "Has died" << std::endl;
		isAlive = false;
		//this->Filter();

	}
	if (!isAlive && p.currHP > 0) {
		std::cout << "Has come to life" << std::endl;
		isAlive = true;
		//this->Filter();
	}

	float now = glfwGetTime();
	if (dealtDamage && now - hitStart > 0.1f) {
		hitStart = now;
		std::string cs = "hit";
		this->PlayAudio(cs, pos);
	}

	//if last alive and now dead play death
	if (lastState && !isAlive) {
		lastState = false;
		std::string dt = "death";
		this->PlayAudio(dt, pos);
	}
	//if last dead and now alive play respawn
	else if (!lastState && isAlive) {
		lastState = true;
		std::string rs = "respawn";
		this->PlayAudio(rs, pos);
	}

	if (!lastFlagState && p.hasFlag) {
		lastFlagState = true;
		std::string cap = "capture";
		this->PlayAudio(cap, pos);
	}
	else if (!p.hasFlag) {
		lastFlagState = false;
		//lost the flag
	}

	if (!decision && selfState == 1) {
		decision = true;
		std::string d = "defeat";
		this->PlayAudio(d, pos);
	}
	else if (!decision && selfState == 2) {
		decision = true;
		std::string w = "victory";
		this->PlayAudio(w, pos);
	}
}

void Audio::Filter() {
	bool active = false;
	FMOD_RESULT result = lowpassDSP->getBypass(&active);
	if (result == FMOD_OK) { lowpassDSP->setBypass(!active); }
}