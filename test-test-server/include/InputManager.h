#pragma once
#include "../include/shared/ObjectData.h"
#include "../include/shared/NetworkData.h"

struct PlayerIntentTracking {
	//movement
	size_t moveLeftDuration = 0; 
	size_t moveRightDuration = 0;
	size_t moveUpDuration = 0;
	size_t moveDownDuration = 0;
	size_t moveForwardDuration = 0;
	size_t moveBackDuration = 0;

	//attack triggers
	size_t rightClickDuration = 0;
	size_t leftClickDuration = 0;
	size_t scrollUpDuration = 0;
	size_t scrollDownDuration = 0;

	//powers
	size_t hit1Duration = 0;
	size_t hit2Duration = 0;
	size_t hit3Duration = 0;
	size_t hit4Duration = 0;
	size_t hit5Duration = 0;

	size_t hitEDuration = 0;
	size_t hitRDuration = 0;
	size_t hitTDuration = 0;
	size_t hitYDuration = 0;
	size_t hitQDuration = 0;

};

class InputManager {

public:
	PlayerIntentTracking playerIntentTrackers[MAX_PLAYERS];


	void updateTracking(PlayerIntentPacket intent, int id);


};