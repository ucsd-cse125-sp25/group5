// PlayerBehaviorComponent.cpp
#include "physics/BehaviorComponent.h"   // brings in both BehaviorComponent & PlayerBehaviorComponent
#include "physics/PhysicsSystem.h"      // for any phys.integrate calls
#include "physics/PhysicsData.h"        // for GameObject




glm::vec3 getInputVelocity(const PlayerIntentPacket& intent, GameObject* obj) {
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

	//only apply the player velocity for movement
	obj->physics->velocity += getInputVelocity(physicsSystem.PlayerIntents[obj->id], obj);

	obj->transform.position += obj->physics->velocity * deltaTime;

	//remove it after
	obj->physics->velocity -= getInputVelocity(physicsSystem.PlayerIntents[obj->id], obj);

	obj->transform.aabb = physicsSystem.getAABB(obj);
    
}

//—— handleCollision — called when this object hits another
void PlayerBehaviorComponent::handleCollision(GameObject* obj,
    const GameObject& other)
{

}