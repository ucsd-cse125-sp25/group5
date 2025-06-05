#include <Audio.h>
#include "Camera.h"
#include <glm/gtx/string_cast.hpp>

//Name the audio file will be reference by to play and the path of the audio file
static std::unordered_map<std::string, std::string> AudioFiles = {
	{"metalA", PROJECT_SOURCE_DIR + std::string("/assets/audiofiles/metalA.wav")},
	{"metalM", PROJECT_SOURCE_DIR + std::string("/assets/audiofiles/metalM.wav")},
	{"woodA", PROJECT_SOURCE_DIR + std::string("/assets/audiofiles/woodA.wav")},
	{"woodM", PROJECT_SOURCE_DIR + std::string("/assets/audiofiles/woodM.wav")},
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
	{"capture", PROJECT_SOURCE_DIR + std::string("/assets/audiofiles/capture.wav")},
	{"transfer", PROJECT_SOURCE_DIR + std::string("/assets/audiofiles/transfer.wav")},
	{"lobbymusic", PROJECT_SOURCE_DIR + std::string("/assets/audiofiles/lobbymusic.wav")},
	{"gamemusic", PROJECT_SOURCE_DIR + std::string("/assets/audiofiles/gamemusic.wav")},
	{"paintcanvas", PROJECT_SOURCE_DIR + std::string("/assets/audiofiles/paintcanvas.wav")},
	{"ocean", PROJECT_SOURCE_DIR + std::string("/assets/audiofiles/ocean.mp3")},
	{"wind", PROJECT_SOURCE_DIR + std::string("/assets/audiofiles/wind.mp3")},
	{"healthUP", PROJECT_SOURCE_DIR + std::string("/assets/audiofiles/healthUP.wav")},
	{"manaUP", PROJECT_SOURCE_DIR + std::string("/assets/audiofiles/manaUP.wav")},
};

static std::unordered_map<std::string, FMOD::Sound*> Sounds;
//static std::unordered_map<std::string, std::tuple<FMOD::Sound*, FMOD::Channel*>> Engine;


//Loop through AudioFiles hash map and create Sound* placed in Sounds hashmap
void Audio::Init(ClientGame* client) {
	this->client = client;
	phase = PRE_GAME;
	hitStart = glfwGetTime();
	deathStart = glfwGetTime();
	manapick = glfwGetTime();
	hppick = glfwGetTime();
	FMOD::System_Create(&system);
	system->init(512, FMOD_INIT_3D_RIGHTHANDED, nullptr);

	//Load all the files and put them in a map
	for (const auto& pair : AudioFiles) {
		const std::string& track = pair.first;
		const std::string& path = pair.second;
		FMOD::Sound* s;
		if (track.find("music") != std::string::npos || track.find("ocean") != std::string::npos || track.find("wind") != std::string::npos) {
			system->createSound(path.c_str(), FMOD_2D | FMOD_CREATESAMPLE, 0, &s);
			s->setMode(FMOD_LOOP_NORMAL);
		}
		else {
			system->createSound(path.c_str(), FMOD_3D | FMOD_CREATESAMPLE, 0, &s);
			s->setMode(FMOD_LOOP_OFF);
		}
		Sounds[track] = s;
	}
	listenerPos = { 0.0f, 0.0f, 0.0f };
	listenerVel = { 0.0f, 0.0f, 0.0f };
	forward = { 0.0f, 0.0f, 1.0f };
	up = { 0.0f, 1.0f, 0.0f };
	system->set3DListenerAttributes(0, &listenerPos, &listenerVel, &forward, &up);
	system->set3DSettings(1.0f, 1.0f, 1.0f);
	system->getMasterChannelGroup(&masterGroup);
	system->createChannelGroup("mainGroup", &mainGroup);
	masterGroup->addGroup(mainGroup);
	//Sub groups
	system->createChannelGroup("sfxGroup", &sfxGroup);
	system->createChannelGroup("attackGroup", &attackGroup);

	mainGroup->addGroup(sfxGroup);
	mainGroup->addGroup(attackGroup);

	system->createDSPByType(FMOD_DSP_TYPE_LOWPASS, &lowpassDSP);
	lowpassDSP->setParameterFloat(FMOD_DSP_LOWPASS_CUTOFF, 500.0f); //originally 750.0f
	lowpassDSP->setBypass(true);
	attackGroup->addDSP(0, lowpassDSP);
}

//Given name of audio, from AudioFiles hash map, place audio track into channel and then play it
//Also pass in the position for spatial audio
void Audio::PlayAudio(std::string n, glm::vec3 pos, float volume) {

	FMOD_VECTOR soundPos = { pos.x, pos.y, pos.z };
	FMOD_VECTOR soundVel = { 0.0f, 0.0f, 0.0f };

	if (n.find("music") != std::string::npos) {
		StopAudio();
		system->playSound(Sounds[n], nullptr, true, &musicChannel);
		musicChannel->set3DAttributes(&soundPos, &soundVel);

		musicChannel->setVolume(volume * 0.75);
		musicChannel->set3DMinMaxDistance(0.5f, 95.0f);
		musicChannel->setChannelGroup(sfxGroup);
		musicChannel->setPaused(false);
	}
	else if (n.find("ocean") != std::string::npos) {
		system->playSound(Sounds[n], nullptr, true, &waterChannel);
		waterChannel->setVolume(volume);
		waterChannel->set3DMinMaxDistance(0.5f, 95.0f);
		waterChannel->setChannelGroup(sfxGroup);
		waterChannel->setPaused(false);
	}
	else if (n.find("wind") != std::string::npos) {
		system->playSound(Sounds[n], nullptr, true, &windChannel);
		windChannel->setVolume(volume);
		windChannel->set3DMinMaxDistance(0.5f, 95.0f);
		windChannel->setChannelGroup(sfxGroup);
		windChannel->setPaused(false);
	}
	else {

		FMOD::Channel* channel = nullptr;

		system->playSound(Sounds[n], nullptr, true, &channel);
		channel->set3DAttributes(&soundPos, &soundVel);
		float dec = 1.0f;
		channel->setVolume(volume * dec);
		channel->set3DMinMaxDistance(0.5f, 95.0f);
		bool isUnfiltered = (
			n == "death" || n == "defeat" || n == "victory" ||
			n == "hit" || n == "ticktock" || n == "respawn" ||
			n == "capture" || n == "transfer"
			);
		channel->setChannelGroup(isUnfiltered ? sfxGroup : attackGroup);
		channel->setPaused(false);
	}
}

//Can only stop the background music
void Audio::StopAudio() {
	musicChannel->stop();
}

void Audio::UpdateAmbient(PlayerStats & p) {
	if (client->GameState.phase == GamePhase::IN_GAME) {
		windChannel->setVolume(p.windAudioFlag * 0.35);
		waterChannel->setVolume(p.waveAudioFlag * 0.35);
	}
	else {
		windChannel->setVolume(0.0f);
		waterChannel->setVolume(0.0f);
	}
}

//FMOD::System* automatically handles playing audio
void Audio::Update(Camera* cam, UIData &p) {
	float volume = 0.9f;
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
		isAlive = false;
	}
	if (!isAlive && p.currHP > 0) {
		isAlive = true;
	}
	if (p.currHP > 20) {
		this->Filter(false);
	}
	else if (p.currHP <= 20) {
		this->Filter(true);
	}

	float now = glfwGetTime();
	if (p.dealtDamage && now - hitStart > 0.1f) {
		hitStart = now;
		std::string cs = "hit";
		this->PlayAudio(cs, pos, volume);
	}

	PlayerStats ps = client->GameState.player_stats[client->playerId];
	if (ps.hpPickupFlag && now - hppick > 1.0f) {
		hppick = now;
		std::string hpUP = "healthUP";
		this->PlayAudio(hpUP, pos, volume);
	}

	if (ps.manaPickupFlag && now - manapick > 1.0f) {
		manapick = now;
		std::string manaUP = "manaUP";
		this->PlayAudio(manaUP, pos, volume);
	}
	
	for (int i = 0; i < client->GameState.num_players; i++) {
		int id = client->GameState.players[i].id;
		CFS[id] = client->GameState.player_stats[id].hasFlag;
	}

	bool playTransfer = false;
	bool youStole = false;
	for (int i = 0; i < client->GameState.num_players; i++) {
		int id = client->GameState.players[i].id;
		if (!LFS[id] && CFS[id] && client->playerId == id) { //Play "capture" for yourself
			LFS[id] = true;
			std::string cap = "capture";
			this->PlayAudio(cap, pos, volume);
			youStole = true;
		}
		else if (LFS[id] && !CFS[id] && client->GameState.player_stats[id].alive) {
			LFS[id] = false;
			playTransfer = true;
		}
	}

	//Play transfer if anybody got their flag stolen and you did not steal the flag
	if (playTransfer && !youStole) {
		std::string trs = "transfer";
		this->PlayAudio(trs, pos, volume);
	}

	//Update last flag state
	for (int i = 0; i < client->GameState.num_players; i++) {
		int id = client->GameState.players[i].id;
		LFS[id] = CFS[id];
	}

	//if last alive and now dead play death
	if (lastState && !isAlive) {
		lastState = false;
		std::string dt = "death";
		this->PlayAudio(dt, pos, volume);
	}
	//if last dead and now alive play respawn
	else if (!lastState && isAlive) {
		lastState = true;
		std::string rs = "respawn";
		this->PlayAudio(rs, pos, volume);
	}
	if (p.seconds < 61 && !paintOut && phase == IN_GAME) {
		paintOut = true;
		std::string pc = "paintcanvas";
		this->PlayAudio(pc, pos, volume);
	}

	if (p.seconds < 30 && !timeOut && phase == IN_GAME) {
		timeOut = true;
		std::string tt = "ticktock";
		this->PlayAudio(tt, pos, volume);
	}

	if (!decision && selfState == 1) {
		decision = true;
		std::string d = "defeat";
		this->PlayAudio(d, pos, 0.70f);
	}
	else if (!decision && selfState == 2) {
		decision = true;
		std::string w = "victory";
		this->PlayAudio(w, pos, 0.70f);
	}
}

void Audio::Filter(bool enable) {
	bool bypass = false;
	if (lowpassDSP->getBypass(&bypass) == FMOD_OK) {
		bool shouldBypass = !enable;
		if (bypass == shouldBypass) {
			return;
		}
		std::cout << "Setting filter to: " << enable << std::endl;
		lowpassDSP->setBypass(shouldBypass);
	}
}