#define NOMINMAX
#include "physics/BehaviorComponent.h"   // brings in both BehaviorComponent & PlayerBehaviorComponent
#include "physics/PhysicsSystem.h"      // for any phys.integrate calls
#include "physics/PhysicsData.h"        // for GameObject
#include "ServerGame.h"
#include <limits>

//—— integrate — called once per tick
void ProjectileBehaviorComponent::integrate(GameObject* obj,
	float deltaTime,
	PhysicsSystem& phys)
{

}

//—— handleCollision — called when this object hits another
void ProjectileBehaviorComponent::handleCollision(GameObject* obj,
	const GameObject& other)
{

}