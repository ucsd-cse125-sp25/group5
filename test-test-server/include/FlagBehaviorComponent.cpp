#define NOMINMAX
#include "physics/BehaviorComponent.h"   // brings in both BehaviorComponent & PlayerBehaviorComponent
#include "physics/PhysicsSystem.h"      // for any phys.integrate calls
#include "physics/PhysicsData.h"        // for GameObject
#include "ServerGame.h"
#include "../include/shared/ObjectData.h"
#include <algorithm>

void FlagBehaviorComponent::integrate(GameObject* obj,
	float deltaTime,
	PhysicsSystem& phys) {

	if(tagTransferTimer > 0) {
		tagTransferTimer -= deltaTime;
		if(tagTransferTimer <= 0) {
			tagTransferTimer = 0;
			inCooldown = false;
		}
	}

	//just keep going, fix the velocity, and update the position
	if (owningPlayer != -1) {
		for (auto player : physicsSystem.playerObjects) {
			if (player->id == owningPlayer) {
				obj->transform.position = player->transform.position + glm::vec3(0,4,0);
				obj->transform.rotation = player->transform.rotation;
				break;
			}
		}
	} else {
		physicsSystem.defaultIntegrate(obj, deltaTime);
		float maxY = std::max(obj->transform.position.y, phys.waterLevel);

		obj->transform.position.y = maxY; // keep the flag above the water level
	}
}

//—— resolveCollision — called when this object hits another
void FlagBehaviorComponent::resolveCollision(GameObject* obj, GameObject* other, const pair<vec3, float>& penetration, int status)
{
	if (status == 0) {
		physicsSystem.resolveCollision(obj, other, penetration, status);
	}

	if (status == 1 && other->type == PLAYER) {
		//the flag got captured while it is not owned
		if (owningPlayer == -1) {
			owningPlayer = other->id;
			owningGameObject = other;
			other->attached = obj;
			printf("Flag transferred to player %d\n", other->id);

			//killfeed for flag being picked up
			struct KillfeedItem item = { -1, other->id, FLAGPICKUP, 0.0f };
			physicsSystem.addKillfeedItem(item);
		}
		//someone tried to capture the flag while it is owned
		else if (owningPlayer != -1) {
			if (other->id == owningPlayer) {
				//do nothing, already attached to this player
			}
			else if (!inCooldown) {
				//this is a different player, so drop the flag

				int originalOwningPlayer = owningPlayer;
				//rely on the fact that playerID is its object id
				owningPlayer = other->id;

				//unattach the player
				owningGameObject->attached = nullptr;

				//we are now owned by someobdy else
				owningGameObject = other;

				//attach the flag to this player
				other->attached = obj;
				//start the timer
				tagTransferTimer = TAG_TRANSFER_TIME;

				inCooldown = true;


				printf("Flag transferred to player %d\n", other->id);

				//killfeed for flag being stolen from one player to another
				struct KillfeedItem item = { originalOwningPlayer, other->id, FLAGSTEAL, 0.0f };
				physicsSystem.addKillfeedItem(item);
			}
		}
	}
}