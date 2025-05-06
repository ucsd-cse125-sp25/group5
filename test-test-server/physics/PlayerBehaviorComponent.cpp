// PlayerBehaviorComponent.cpp
#include "physics/BehaviorComponent.h"   // brings in both BehaviorComponent & PlayerBehaviorComponent
#include "physics/PhysicsSystem.h"      // for any phys.integrate calls
#include "physics/PhysicsData.h"        // for GameObject




glm::vec3 getInputDirection(const PlayerIntentPacket& intent, GameObject* obj) {
	//process player input
	GameObject* target = obj;

	float azimuth = glm::radians(-intent.azimuthIntent);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), azimuth, up);
	glm::vec3 forward = glm::normalize(glm::vec3(rotation * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)));
	//glm::vec3 translation = glm::vec3(target.cubeModel[3]);
	glm::vec3 translation = target->transform.position;
	glm::vec3 right = glm::normalize(glm::cross(up, forward));


	glm::vec3 toRet = glm::vec3(0.0f);

	if (intent.moveLeftIntent) {
		toRet += (-right);
	}
	if (intent.moveRightIntent) {
		toRet += right;
	}
	if (intent.moveForwardIntent) {
		toRet += (-forward);
	}
	if (intent.moveBackIntent) {
		toRet += forward;
	}

	return toRet;
}

//god forgive me for what I'm about to do
bool checkBottom(GameObject* obj, PhysicsSystem& phys) {
	// 1) compute a single point just below the player's feet
	float eps = 0.1f;
	glm::vec3 foot = obj->transform.position
		- glm::vec3(0.f, obj->collider->halfExtents.y + eps, 0.f);

	// 2) test that point against every static AABB
	for (auto* s : phys.staticObjects) {
		const AABB& b = s->transform.aabb;
		if (foot.x >= b.min.x && foot.x <= b.max.x
			&& foot.y >= b.min.y && foot.y <= b.max.y
			&& foot.z >= b.min.z && foot.z <= b.max.z)
		{
			return true;
		}
	}

	return false;
}

//—— integrate — called once per tick
void PlayerBehaviorComponent::integrate(GameObject* obj,
    float deltaTime,
    PhysicsSystem& phys)
{
	// apply force 
	obj->physics->velocity += obj->physics->acceleration * deltaTime;

	//apply drag
	obj->physics->velocity *= (1.0f - obj->physics->drag * deltaTime);

	

	//clamp velocity
	if (glm::length(obj->physics->velocity) > obj->physics->maxSpeed) {
		obj->physics->velocity = glm::normalize(obj->physics->velocity) * obj->physics->maxSpeed;
	}

	//detect collision from bottom for jump
	if (checkBottom(obj, phys) && physicsSystem.PlayerIntents[obj->id].moveUpIntent) {
		obj->physics->velocity += glm::vec3(0.0f, 3.0f, 0.0f);
	}

	//only apply the player velocity for movement
	//obj->physics->velocity += getInputDirection(physicsSystem.PlayerIntents[obj->id], obj);


	obj->transform.position += obj->physics->velocity * deltaTime;
	obj->transform.position += getInputDirection(physicsSystem.PlayerIntents[obj->id], obj) * deltaTime;

	//remove it after
	//obj->physics->velocity -= getInputDirection(physicsSystem.PlayerIntents[obj->id], obj);

	obj->transform.aabb = physicsSystem.getAABB(obj);
    
}

//—— handleCollision — called when this object hits another
void PlayerBehaviorComponent::handleCollision(GameObject* obj,
    const GameObject& other)
{

}