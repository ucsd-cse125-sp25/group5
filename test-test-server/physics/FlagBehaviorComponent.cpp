#define NOMINMAX
#include "physics/BehaviorComponent.h"   // brings in both BehaviorComponent & PlayerBehaviorComponent
#include "physics/PhysicsSystem.h"      // for any phys.integrate calls
#include "physics/PhysicsData.h"        // for GameObject
#include "ServerGame.h"

void FlagBehaviorComponent::integrate(GameObject* obj,
	float deltaTime,
	PhysicsSystem& phys)
{
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
	obj->transform.aabb = phys.getAABB(obj);
}

//—— resolveCollision — called when this object hits another
void FlagBehaviorComponent::resolveCollision(GameObject* obj, GameObject* other, const pair<vec3, float>& penetration, int status)
{
	if (status == 0) {
		physicsSystem.resolveCollision(obj, other, penetration, status);
	}

	if (status == 1 && other->type == PLAYER) {
		if (owningPlayer == -1) {
			owningPlayer = other->id;
			other->attached = obj;
		}
	}
}