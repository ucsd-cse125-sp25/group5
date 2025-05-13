#include "../include/shared/ObjectData.h"
#include "../include/shared/NetworkData.h"

struct PlayerIntentTracking {
	//movement
	int moveLeftDuration = 0; 
	int moveRightDuration = 0;
	int moveUpDuration = 0;
	int moveDownDuration = 0;
	int moveForwardDuration = 0;
	int moveBackDuration = 0;

	//attack triggers
	int rightClickDuration = 0;
	int leftClickDuration = 0;
	int scrollUpDuration = 0;
	int scrollDownDuration = 0;

	//powers
	int hit1Duration = 0;
	int hit2Duration = 0;
	int hit3Duration = 0;
	int hit4Duration = 0;
	int hit5Duration = 0;

	int hitEDuration = 0;
	int hitRDuration = 0;
	int hitTDuration = 0;
	int hitYDuration = 0;
	int hitQDuration = 0;

};

class InputManager {

};