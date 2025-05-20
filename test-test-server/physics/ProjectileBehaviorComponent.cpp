#define NOMINMAX
#include "physics/BehaviorComponent.h"   // brings in both BehaviorComponent & PlayerBehaviorComponent
#include "physics/PhysicsSystem.h"      // for any phys.integrate calls
#include "physics/PhysicsData.h"        // for GameObject
#include "ServerGame.h"
#include <limits>


const float ProjectileBehaviorComponent::WOOD_PROJ_SPEED = 25.0f; // Replace 25.0f with the desired speed value
//—— integrate — called once per tick
void ProjectileBehaviorComponent::integrate(GameObject* obj,
	float deltaTime,
	PhysicsSystem& phys)
{
	//just keep going, fix the velocity, and update the position
	obj->physics->velocity = velocity;
	obj->transform.position += obj->physics->velocity * deltaTime;
	
	lifetime -= deltaTime;
	if (lifetime <= 0) {
		obj->markDeleted = true;
	}
}

//—— resolveCollision — called when this object hits another
void ProjectileBehaviorComponent::resolveCollision(GameObject* obj, GameObject* other, const pair<vec3, float>& penetration, int status)
{
	////delete myself
	if (status == 0) {
		//delete the object
		obj->markDeleted = true;
	}

	if (status == 1) {
		//only delete if it comes into contact with a dynamic object that is a non self player
		if (other->type == PLAYER && other->id != originalPlayer) {
			//delete the object
			obj->markDeleted = true;
		}
	}

}