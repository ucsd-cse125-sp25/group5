#include <Audio.h>
#include "Camera.h"

//Name the audio file will be reference by to play and the path of the audio file
static std::unordered_map<std::string, std::string> AudioFiles = {
	{"firesound", PROJECT_SOURCE_DIR + std::string("/assets/audiofiles/firesound.wav")},
	{"fireA", PROJECT_SOURCE_DIR + std::string("/assets/audiofiles/fireA.wav")},
	{"fireM", PROJECT_SOURCE_DIR + std::string("/assets/audiofiles/fireM.wav")},
	{"waterA", PROJECT_SOURCE_DIR + std::string("/assets/audiofiles/waterA.wav")},
	{"waterM", PROJECT_SOURCE_DIR + std::string("/assets/audiofiles/waterM.wav")}
	//{"matchsong", PROJECT_SOURCE_DIR + std::string("/assets/audiofiles/LastSurprise.mp3")}
};

static std::unordered_map<std::string, FMOD::Sound*> Sounds;
//static std::unordered_map<std::string, std::tuple<FMOD::Sound*, FMOD::Channel*>> Engine;


//Loop through AudioFiles hash map and create Sound* placed in Sounds hashmap
void Audio::Init() {
	FMOD::System_Create(&system);
	system->init(512, FMOD_INIT_3D_RIGHTHANDED, nullptr);

	//Load all the files and put them in a map
	for (const auto& pair : AudioFiles) {
		const std::string& track = pair.first;
		const std::string& path = pair.second;
		FMOD::Sound* s;
		system->createSound(path.c_str(), FMOD_3D, 0, &s);
		Sounds[track] = s;
	}
	listenerPos = { 0.0f, 0.0f, 0.0f };
	listenerVel = { 0.0f, 0.0f, 0.0f };
	forward = { 0.0f, 0.0f, 1.0f };
	up = { 0.0f, 1.0f, 0.0f };
	system->set3DListenerAttributes(0, &listenerPos, &listenerVel, &forward, &up);
	system->set3DSettings(1.0f, 1.0f, 1.0f);
}

//Given name of audio, from AudioFiles hash map, place audio track into channel and then play it
//Also pass in the position for spatial audio
void Audio::PlayAudio(std::string n, glm::vec3 pos) {
	FMOD_VECTOR soundPos = {pos.x, pos.y, pos.z};
	FMOD_VECTOR soundVel = { 0.0f, 0.0f, 0.0f };
	FMOD::Channel* channel = nullptr;
	system->playSound(Sounds[n], nullptr, true, &channel);
	channel->set3DAttributes(&soundPos, &soundVel);
	channel->setPaused(false);  // Unpause to start playing
	channel->set3DMinMaxDistance(1.0f, 100.0f);
}

/*
if (isMatching) {
	musicChannel = channel;
	soundPos = { 5.0f, 0.0f, 0.0f };
}
else {
	soundPos = { 0.0f, 1.0f, 0.0f };
}
*/

//Can only stop the background music
void Audio::StopAudio() {
	musicChannel->stop();
}

//FMOD::System* automatically handles playing audio
void Audio::Update(Camera* cam) {
	//Set the position up and forward
	glm::vec3 pos = cam->GetPosition();
	glm::vec3 f = glm::normalize(cam->GetCameraForwardVector());
	listenerPos = { pos.x, pos.y, pos.z };
	listenerVel = { 0.0f, 0.0f, 0.0f };
	forward = { f.x, f.y, f.z };
	up = { 0.0f, 1.0f, 0.0f };
	system->set3DListenerAttributes(0, &listenerPos, &listenerVel, &forward, &up);
	system->update();
}