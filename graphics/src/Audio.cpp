#include <Audio.h>

static std::unordered_map<std::string, std::string> AudioFiles = {
	{"firesound", PROJECT_SOURCE_DIR + std::string("/assets/audiofiles/firesound.wav")},
	{"matchsong", PROJECT_SOURCE_DIR + std::string("/assets/audiofiles/LastSurprise.mp3")}
};

static std::unordered_map<std::string, FMOD::Sound*> Sounds;

void Audio::Init() {
	FMOD::System_Create(&system);
	system->init(512, FMOD_INIT_3D_RIGHTHANDED, nullptr);

	//Load all the files and put them in a map
	for (const auto& pair : AudioFiles) {
		const std::string& track = pair.first;
		const std::string& path = pair.second;
		FMOD::Sound* s;
		system->createSound(path.c_str(), FMOD_3D | FMOD_LOOP_NORMAL, 0, &s);
		Sounds[track] = s;
	}
	listenerPos = { 0.0f, 0.0f, 0.0f };
	listenerVel = { 0.0f, 0.0f, 0.0f };
	forward = { 0.0f, 0.0f, 1.0f };
	up = { 0.0f, 1.0f, 0.0f };
	system->set3DListenerAttributes(0, &listenerPos, &listenerVel, &forward, &up);
	system->set3DSettings(1.0f, 1.0f, 1.0f);
}

void Audio::PlayAudio(std::string n) {
	FMOD::Channel* channel = nullptr;
	system->playSound(Sounds[n], nullptr, true, &channel);
	FMOD_VECTOR soundPos;
	if (strcmp(n.c_str(), "matchsong") == 0 ){
		soundPos = { 5.0f, 0.0f, 0.0f };
	}
	else {
		soundPos = { 0.0f, 1.0f, 0.0f };
	}
	FMOD_VECTOR soundVel = { 0.0f, 0.0f, 0.0f };
	channel->set3DAttributes(&soundPos, &soundVel);
	channel->setPaused(false);  // Unpause to start playing
	channel->set3DMinMaxDistance(1.0f, 100.0f);
}

void Audio::Update() {
	system->update();
}