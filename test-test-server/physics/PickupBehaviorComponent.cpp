#define NOMINMAX
#include "physics/BehaviorComponent.h"   // brings in both BehaviorComponent & PlayerBehaviorComponent
#include "physics/PhysicsSystem.h"      // for any phys.integrate calls
#include "physics/PhysicsData.h"        // for GameObject
#include "ServerGame.h"
#include <limits>


const float PICKUP_COOLDOWN = 20.0f; // cooldown time for pickup behavior
//—— integrate — called once per tick
void PickupBehaviorComponent::integrate(GameObject* obj,
	float deltaTime,
	PhysicsSystem& phys)
{
	//fixed position

	if(pickupTimer > 0) {
		obj->transform.position = restPosition;
		pickupTimer -= deltaTime;
	}
	if (pickupTimer <= 0) {
		isActive = true;
		obj->transform.position = position; // reset position to original position
	}
}

//—— resolveCollision — called when this object hits another
void PickupBehaviorComponent::resolveCollision(GameObject* obj, GameObject* other, const pair<vec3, float>& penetration, int status)
{
	if (status == 1) {
		//only delete if it comes into contact with a dynamic object that is a non self player
		if (other->type == PLAYER) {
			pickupTimer = PICKUP_COOLDOWN; // reset the timer
			isActive = false; // deactivate the pickup object
		}
	}
}