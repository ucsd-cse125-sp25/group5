#include "../include/shared/ObjectData.h"
#include "../include/shared/NetworkData.h"
#include "InputManager.h"

void InputManager::updateTracking(PlayerIntentPacket intent, int id) {
	playerIntentTrackers[id].moveLeftDuration = intent.moveLeftIntent ? playerIntentTrackers[id].moveLeftDuration + 1 : 0;
	playerIntentTrackers[id].moveRightDuration = intent.moveRightIntent ? playerIntentTrackers[id].moveRightDuration + 1 : 0;
	playerIntentTrackers[id].moveUpDuration = intent.moveUpIntent ? playerIntentTrackers[id].moveUpDuration + 1 : 0;
	playerIntentTrackers[id].moveDownDuration = intent.moveDownIntent ? playerIntentTrackers[id].moveDownDuration + 1 : 0;
	playerIntentTrackers[id].moveForwardDuration = intent.moveForwardIntent ? playerIntentTrackers[id].moveForwardDuration + 1 : 0;
	playerIntentTrackers[id].moveBackDuration = intent.moveBackIntent ? playerIntentTrackers[id].moveBackDuration + 1 : 0;

	playerIntentTrackers[id].rightClickDuration = intent.rightClickIntent ? playerIntentTrackers[id].rightClickDuration + 1 : 0;
	playerIntentTrackers[id].leftClickDuration = intent.leftClickIntent ? playerIntentTrackers[id].leftClickDuration + 1 : 0;

	playerIntentTrackers[id].scrollUpDuration = intent.scrollUpIntent ? playerIntentTrackers[id].scrollUpDuration + 1 : 0;
	playerIntentTrackers[id].scrollDownDuration = intent.scrollDownIntent ? playerIntentTrackers[id].scrollDownDuration + 1 : 0;

	playerIntentTrackers[id].hit1Duration = intent.hit1Intent ? playerIntentTrackers[id].hit1Duration + 1 : 0;
	playerIntentTrackers[id].hit2Duration = intent.hit2Intent ? playerIntentTrackers[id].hit2Duration + 1 : 0;
	playerIntentTrackers[id].hit3Duration = intent.hit3Intent ? playerIntentTrackers[id].hit3Duration + 1 : 0;
	playerIntentTrackers[id].hit4Duration = intent.hit4Intent ? playerIntentTrackers[id].hit4Duration + 1 : 0;
	playerIntentTrackers[id].hit5Duration = intent.hit5Intent ? playerIntentTrackers[id].hit5Duration + 1 : 0;
	playerIntentTrackers[id].hitEDuration = intent.hitEIntent ? playerIntentTrackers[id].hitEDuration + 1 : 0;
	playerIntentTrackers[id].hitRDuration = intent.hitRIntent ? playerIntentTrackers[id].hitRDuration + 1 : 0;
	playerIntentTrackers[id].hitTDuration = intent.hit1Intent ? playerIntentTrackers[id].hitTDuration + 1 : 0;
	playerIntentTrackers[id].hitYDuration = intent.hit2Intent ? playerIntentTrackers[id].hitYDuration + 1 : 0;
	playerIntentTrackers[id].hitQDuration = intent.hit3Intent ? playerIntentTrackers[id].hitQDuration + 1 : 0;
	// Add any other intent tracking you need here
}