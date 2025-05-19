#define NOMINMAX
#include "physics/BehaviorComponent.h"   // brings in both BehaviorComponent & PlayerBehaviorComponent
#include "physics/PhysicsSystem.h"      // for any phys.integrate calls
#include "physics/PhysicsData.h"        // for GameObject
#include "ServerGame.h"

void FlagBehaviorComponent::integrate(GameObject* obj,
	float deltaTime,
	PhysicsSystem& phys)
{
	//inCooldown = tagTransferTimer > 0;

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
				obj->transform.position = player->transform.position + glm::vec3(0, 7, 0);
				break;
			}
		}
	} else {
		physicsSystem.defaultIntegrate(obj, deltaTime);
	}

	//also get the AABB
	//obj->transform.aabb = phys.getAABB(obj);
	//obj->collider->aabb = phys.getAABB(obj);
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
			other->attached = obj;
			printf("Flag transferred to player %d\n", other->id);
		}
		//someone tried to capture the flag while it is owned
		else if (owningPlayer != -1) {
			if (other->id == owningPlayer) {
				//do nothing, already attached to this player
			}
			else if (!inCooldown) {
				//this is a different player, so drop the flag

				//rely on the fact that playerID is its object id
				owningPlayer = other->id;
				//attach the flag to this player
				other->attached = other;
				//start the timer
				tagTransferTimer = TAG_TRANSFER_TIME;

				inCooldown = true;

				printf("Flag transferred to player %d\n", other->id);
			}
		}
	}
}