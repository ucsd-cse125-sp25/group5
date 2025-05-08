// PlayerBehaviorComponent.cpp
#define NOMINMAX
#include "physics/BehaviorComponent.h"   // brings in both BehaviorComponent & PlayerBehaviorComponent
#include "physics/PhysicsSystem.h"      // for any phys.integrate calls
#include "physics/PhysicsData.h"        // for GameObject
#include "ServerGame.h"
#include <limits>




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

glm::vec3 getDirection(float azimuth, float incline) {
	// if azimuth & incline are in degrees:
	// azimuth = glm::radians(azimuth);
	// incline = glm::radians(incline);

	float cosInc = cos(incline);

	glm::vec3 D;
	D.x = cosInc * sin(azimuth) * -1;  // right/left
	D.y = sin(incline);           // up/down
	D.z = cosInc * cos(azimuth) * -1;  // forward/back
 // invert direction

	return glm::normalize(D);
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

bool rayIntersectsAABB(const Ray& ray,
	const AABB& box,
	float& tHit)
{
	float tMin = (box.min.x - ray.origin.x) / ray.dir.x;
	float tMax = (box.max.x - ray.origin.x) / ray.dir.x;
	if (tMin > tMax) std::swap(tMin, tMax);

	for (int i = 1; i < 3; ++i) {
		float originComp = (i == 1 ? ray.origin.y : ray.origin.z);
		float dirComp = (i == 1 ? ray.dir.y : ray.dir.z);
		float bMin = (i == 1 ? box.min.y : box.min.z);
		float bMax = (i == 1 ? box.max.y : box.max.z);

		float t1 = (bMin - originComp) / dirComp;
		float t2 = (bMax - originComp) / dirComp;
		if (t1 > t2) std::swap(t1, t2);

		tMin = std::max(tMin, t1);
		tMax = std::min(tMax, t2);
		if (tMin > tMax) return false;
	}

	// at this point [tMin, tMax] is the overlap interval.
	// we want the first positive hit:
	if (tMax < 0) return false;      // box is behind ray
	tHit = tMin >= 0 ? tMin : tMax;  // if origin inside box, tMin<0 so use tMax
	return true;
}

pair<glm::vec3, float> PlayerBehaviorComponent::handlePlayerGrapple(GameObject* obj, PhysicsSystem& phys) {

	Ray ray;

	ray.origin = obj->transform.position;
	ray.dir = getDirection(
		glm::radians(-phys.PlayerIntents[obj->id].azimuthIntent),
		glm::radians(-phys.PlayerIntents[obj->id].inclineIntent)
	);

	float bestT = std::numeric_limits<float>::infinity();
	GameObject* bestHitObj = nullptr;

	for (auto* staticObj : phys.staticObjects) {
		AABB box = staticObj->transform.aabb;
		float t;
		if (rayIntersectsAABB(ray, box, t) && t < bestT) {
			bestT = t;
			bestHitObj = staticObj;
		}
	}

	if (bestHitObj) {
		glm::vec3 hitPoint = ray.origin + ray.dir * bestT;
		grappleTarget = bestHitObj;
		return { hitPoint, bestT };
	}
	return { glm::vec3(0.0f, 0.0f, 0.0f), -1.0f };
}


//—— integrate — called once per tick
void PlayerBehaviorComponent::integrate(GameObject* obj,
    float deltaTime,
    PhysicsSystem& phys)
{	

	PlayerIntentPacket& intent = physicsSystem.PlayerIntents[obj->id];
	
	//dash movement
	if (state == PlayerMovementState::DASH) {
		dashTimer -= deltaTime;

		//once we're done, exit dash
		if (dashTimer <= 0.0f) {
			state = PlayerMovementState::IDLE;
			dashTimer = 0.0f;
			//slow down our player 
			obj->physics->velocity *= 0.1f;
		}
	}
	else if (state == PlayerMovementState::STOMP) {
		//straight DOWN
		
		stompTimer -= deltaTime;

		//once we're done, exit stomp
		if (stompTimer <= 0.0f || checkBottom(obj, phys)) {
			state = PlayerMovementState::IDLE;
			stompTimer = 0.0f;
		}
	}
	else if (state == PlayerMovementState::GRAPPLE) {

		grappleTimer -= deltaTime;
		//see if we've collided, this whole thing could be optimized if we use the time as well 
		pair<vec3, float> penetration = phys.getAABBpenetration(obj->transform.aabb, grappleTarget->transform.aabb);

		//if we've collided with our target object, or if we've run out of time, release the grapple
		if (grappleTimer <= 0.0f) {
			printf(
				"grapple timer expired or collided with target\n");

			printf("grapple timer %f\n", grappleTimer);
			printf("penetration %f\n", penetration.second);
			obj->physics->velocity *= 0.1f;
			state = PlayerMovementState::IDLE;
			grappleTimer = 0.0f;
			grappleTarget = nullptr;
		}
	}
	

	//regular movement
	if (state == PlayerMovementState::IDLE) {
		//check for movement powers 
		if (intent.hit4Intent) {
			state = PlayerMovementState::DASH;
			dashTimer = DASH_TIME;

			//fix the velocity to the direction we wish to dash in
			obj->physics->velocity = getDirection(
				glm::radians(-intent.azimuthIntent),
				glm::radians(-intent.inclineIntent)
			) * DASH_SPEED;

			return;
		}
		else if (intent.hit5Intent) {
			state = PlayerMovementState::STOMP;
			stompTimer = STOMP_TIME;

			//fix the velocity
			obj->physics->velocity = glm::vec3(0.0f, -STOMP_SPEED, 0.0f);

			return;
		}
		else if (intent.hit3Intent) {
			//high jump
			obj->physics->velocity += glm::vec3(0.0f, JUMP_FORCE * 2.0f, 0.0f);
		}
		else if (intent.hit2Intent) {
			//our target point, we've also set the target object, this could probably use some restructuring
			

			//result
			pair<glm::vec3, float> result = handlePlayerGrapple(obj, phys);

			glm::vec3 target = result.first;

			//we have a target
			if (target != glm::vec3(0.0f, 0.0f, 0.0f)) {
				//only start grappling if we have a target 
				state = PlayerMovementState::GRAPPLE;
				grappleTimer = result.second / GRAPPLE_SPEED;
				//get our direction
				glm::vec3 direction = target - obj->transform.position;
				glm::vec3 normalizedDirection = glm::normalize(direction);
				//lock the velocity
				obj->physics->velocity = normalizedDirection * GRAPPLE_SPEED;
			}
		}

		// apply force 
		obj->physics->velocity += obj->physics->acceleration * deltaTime;

		//apply drag
		obj->physics->velocity *= (1.0f - obj->physics->drag * deltaTime);



		//clamp velocity
		if (glm::length(obj->physics->velocity) > obj->physics->maxSpeed) {
			obj->physics->velocity = glm::normalize(obj->physics->velocity) * obj->physics->maxSpeed;
		}

		//detect collision from bottom for jump, and make the player jump
		if (checkBottom(obj, phys) && physicsSystem.PlayerIntents[obj->id].moveUpIntent) {
			obj->physics->velocity += glm::vec3(0.0f, 3.0f, 0.0f);
		}

		//apply player movement
		obj->transform.position += getInputDirection(physicsSystem.PlayerIntents[obj->id], obj) * deltaTime;
	}




	//only apply the player velocity for movement
	//obj->physics->velocity += getInputDirection(physicsSystem.PlayerIntents[obj->id], obj);

	//the important line
	obj->transform.position += obj->physics->velocity * deltaTime;
	//

	//remove it after
	//obj->physics->velocity -= getInputDirection(physicsSystem.PlayerIntents[obj->id], obj);

	obj->transform.aabb = physicsSystem.getAABB(obj);
    
}

//—— handleCollision — called when this object hits another
void PlayerBehaviorComponent::handleCollision(GameObject* obj,
    const GameObject& other)
{

}