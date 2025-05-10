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
	//just keep going
	obj->physics->velocity = velocity;
	obj->transform.position += obj->physics->velocity * deltaTime;

	obj->transform.aabb = phys.getAABB(obj);
}

//—— resolveCollision — called when this object hits another
void ProjectileBehaviorComponent::resolveCollision(GameObject* obj, GameObject* other, const pair<vec3, float>& penetration, int status)
{
	////delete myself
	printf("Projectile hit %d\n", other->id);
	physicsSystem.deleteDynamicObject(obj);
	delete this;
}