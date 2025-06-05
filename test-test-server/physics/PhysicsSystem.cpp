#include "physics/PhysicsSystem.h"
#include "physics/PhysicsData.h"	
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include "physics/BehaviorComponent.h"

typedef glm::vec3 vec3;
typedef glm::vec4 vec4;
typedef glm::mat3 mat3;
typedef glm::mat4 mat4;
typedef glm::quat quat;

using namespace std;

int nextid = 10;

class BehaviorComponent;    // Forward declaration of BehaviorComponent class

void PhysicsSystem::tick(float dt) {
    // Update all dynamic objects
    for (size_t i = 0; i < movingObjects.size(); ++i) {
        GameObject* obj = movingObjects[i];
        obj->physics->acceleration += glm::vec3(0, -GRAVITY * obj->physics->gravityScale, 0);
        integrate(obj, dt);
    }

    // After integration is complete for all objects, start handling collision
    for (size_t i = 0; i < movingObjects.size(); ++i) {
        GameObject* obj = movingObjects[i];
        handleCollisions(obj);
        obj->physics->acceleration = glm::vec3(0);
    }

	if (currPhase == IN_GAME) {
		updateWaterLevel();
	}

    //delete all objects marked for deletion
	deleteMarkedDynamicObjects();

	//add time to the killfeed 
	tickKillfeed(dt);
}

void PhysicsSystem::defaultIntegrate(GameObject* obj, float dt) {
	// apply force 
	obj->physics->velocity += obj->physics->acceleration * dt;

	//apply drag
	obj->physics->velocity *= (1.0f - obj->physics->drag * dt);

	//clamp velocity
	if (glm::length(obj->physics->velocity) > obj->physics->maxSpeed) {
		obj->physics->velocity = glm::normalize(obj->physics->velocity) * obj->physics->maxSpeed;
	}

	obj->transform.position += obj->physics->velocity * dt;
}

void PhysicsSystem::integrate(GameObject* obj, float dt) {
	if (obj->behavior != nullptr) {
		obj->behavior->integrate(obj, dt, *this);
	}
	else {
		defaultIntegrate(obj, dt);
	}
}

AABB PhysicsSystem::getAABB(GameObject* obj) {
	glm::vec3 min = obj->transform.position - obj->collider->halfExtents;
	glm::vec3 max = obj->transform.position + obj->collider->halfExtents;
	return { min, max };
}

void PhysicsSystem::handleCollisions(GameObject* obj) {
	// Check for collisions with static objects
	for (auto sobj : staticObjects) {
		AABB objAABB = getAABB(obj);
		AABB sobjAABB = getAABB(sobj);
		
		pair<vec3, float> penetration = getAABBpenetration(objAABB, sobjAABB);
		if (penetration.second > 0.0f) {
			if (obj->behavior != nullptr) {
				obj->behavior->resolveCollision(obj, sobj, penetration, 0);
			} else {
				resolveCollision(obj, sobj, penetration, 0);
			}
			
		}  
	}
	
	for (auto dobj : movingObjects) {
		if (obj->id == dobj->id) {
			continue; // Skip self-collision
		}
		AABB objAABB = getAABB(obj);
		AABB dobjAABB = getAABB(dobj);

		pair<vec3, float> penetration = getAABBpenetration(objAABB, dobjAABB);
		if (penetration.second > 0.0f) {
			/*resolveCollision(obj, dobj, penetration, 1);*/
			if (obj->behavior != nullptr) {
				obj->behavior->resolveCollision(obj, dobj, penetration, 1);
			}
		}
	}
	return;
}

void PhysicsSystem::updateWaterLevel() {
	if (timePassed > totalTime / 2) {
		waterLevel = (ENDING_WATER_LEVEL - STARTING_WATER_LEVEL) * ((timePassed - totalTime/2 ) / (totalTime / 2) );
	}
	
}


GameObject* PhysicsSystem::getClosestPlayerObject(glm::vec3 pos, int exclude) {
	float closest = 1000000.0f; // Initialize with a large value
	GameObject* toRet = nullptr;
	for (auto obj : playerObjects) {
		if (glm::distance(obj->transform.position, pos) < closest && obj->id != exclude) {
			closest = glm::distance(obj->transform.position, pos);
			toRet = obj;
		}
	}

  //  if (toRet == nullptr) {
		////return a dummy GameObject if no player is found
		//toRet = makeGameObject();
		//toRet->transform.position = pos; // Set position to the input position
		//toRet->id = exclude; // Set ID to the excluded player ID
		//toRet->isDynamic = false; // Mark as static
		//toRet->collider->halfExtents = glm::vec3(0.1f); // Small collider for dummy object
  //  }
    return toRet;
}

vec3 PhysicsSystem::getImpulseVector(const vec3& normal, const vec3& relativeVelocity, float restitution) {
	float velAlongNormal = glm::dot(relativeVelocity, normal);
	if (velAlongNormal < 0.0f) {
		float impulse = -(1.0f + restitution) * velAlongNormal;
		return impulse * normal;
	}
	return vec3(0.0f);
}

void PhysicsSystem::resolveCollision(GameObject* go1, GameObject* go2, const pair<vec3, float>& penetration, int status) {
	//printf("penetration %f, %f, %f\n", penetration.first.x, penetration.first.y, penetration.first.z);
	vec3 normal = glm::normalize(penetration.first);
	float overlap = penetration.second;

	float overlapFraction = 0.5f;
	if (status == 0) { // go2 is static
		overlapFraction = 1.0f;
	}

	// Velocity resolution: bounce off if moving into each other
	go1->physics->velocity += getImpulseVector(normal, go1->physics->velocity - go2->physics->velocity, 0.1f);
	go2->physics->velocity -= getImpulseVector(normal, go1->physics->velocity - go2->physics->velocity, 0.1f);

	// Positional correction: push both objects out of each other
	go1->transform.position += normal * (overlap * overlapFraction);
	go2->transform.position -= normal * (overlap * (1.0f - overlapFraction));
}

void PhysicsSystem::applyInput(const PlayerIntentPacket& intent, int playerId) {
	//process player input
	GameObject* target = NULL;

	for (auto obj : playerObjects) {
		if (obj->id == playerId) {
			target = obj;
			break;
		}
	}

	if (target == NULL) {
		return;
	}

	glm::vec3 delta = glm::vec3(0.015f);
	float azimuth = glm::radians(-intent.azimuthIntent);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), azimuth, up);
	glm::vec3 forward = glm::normalize(glm::vec3(rotation * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)));
	glm::vec3 translation = target->transform.position;
	glm::vec3 right = glm::normalize(glm::cross(up, forward));    
	glm::quat q = glm::angleAxis(glm::radians(-intent.azimuthIntent), glm::vec3(0, 1, 0));
	target->transform.rotation = q;
}

GameObject* PhysicsSystem::makeGameObject() {
	//create the new game object 
	GameObject* obj = new GameObject;

	//set its id
	obj->id = getNextId();
	
	//position rotation scale 
	obj->transform.position = glm::vec3(0.0f);
	obj->transform.rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); // Identity quaternion
	obj->transform.scale = glm::vec3(1.0f);

	//physics and collider 
	obj->physics = new PhysicsComponent();
	obj->collider = new ColliderComponent();

	//collider half extents 
	obj->collider->halfExtents = glm::vec3(1.0f, 1.0f, 1.0f);

	return obj; // return reference to the new object
}

GameObject* PhysicsSystem::makeGameObject(glm::vec3 position, glm::quat rotation, glm::vec3 halfExtents) {
	GameObject* obj = makeGameObject();

	obj->transform.position = position;
	obj->transform.rotation = rotation;
	obj->collider->halfExtents = halfExtents;

	return obj;
}

glm::mat4 PhysicsSystem::toMatrix(const glm::vec3& position, const glm::quat& quaternion) {
	glm::mat4 T = glm::translate(glm::mat4(1.0f), position);
	glm::mat4 R = glm::toMat4(glm::quat(quaternion));
	return T * R;
}

// Extract position and euler angles (in radians) from a mat4
void PhysicsSystem::fromMatrix(const glm::mat4& mat, glm::vec3& outPosition, glm::vec3& outEulerRadians) {
	glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 translation;
	glm::vec3 skew;
	glm::vec4 perspective;

	glm::decompose(mat, scale, rotation, translation, skew, perspective);

	outPosition = translation;
	outEulerRadians = glm::eulerAngles(rotation);
}

float getOverlap(pair<float,float> interval1, pair<float,float> interval2) {
	return min(interval1.second, interval2.second) - max(interval1.first, interval2.first);
}

vec3 PhysicsSystem::getAABBCenter(AABB& a) {
	return (a.min + a.max) * 0.5f;
}

vec3 PhysicsSystem::getAABBDistanceCenters(AABB& a, AABB& b) {
	vec3 aCenterAABB = (a.min + a.max) * 0.5f;
	vec3 bCenterAABB = (b.min + b.max) * 0.5f;
	return aCenterAABB - bCenterAABB;
}
pair<vec3, float> PhysicsSystem::getAABBpenetration(AABB&  a, AABB&b) {
	/*printf("getAABBpenetration\n");
	printf("aabb1: (%f, %f, %f) (%f, %f, %f)\n", a.min.x, a.min.y, a.min.z, a.max.x, a.max.y, a.max.z);
	printf("aabb2: (%f, %f, %f) (%f, %f, %f)\n", b.min.x, b.min.y, b.min.z, b.max.x, b.max.y, b.max.z);*/
	vec3 axes[3] = { vec3(1, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1) };

	float minOverlap = 999999.0f;
	vec3 minAxis = vec3(0.0f, 0.0f, 0.0f);

	for (int i = 0; i < 3; i++) {
		pair<float, float> interval1 = { a.min[i], a.max[i] };
		pair<float, float> interval2 = { b.min[i], b.max[i] };
		/*printf("interval1: (%f, %f)\n", interval1.first, interval1.second);
		printf("interval2: (%f, %f)\n", interval2.first, interval2.second);*/

		vec3 dir = getAABBDistanceCenters(a, b);

		float overlap = getOverlap(interval1, interval2);
		//printf("Overlap %d: %f\n", i, overlap);

		if (overlap <= 0.0f) {   
			return pair<vec3, float>(vec3(0.0f), overlap); // No overlap
		}

		if (overlap < minOverlap) {
			minOverlap = overlap;
			minAxis = axes[i] * (dir[i] > 0 ? 1.0f : -1.0f); // Choose the axis direction based on the distance vector
		}
	}
	//printf("minAxis: (%f, %f, %f), minOverlap: %f\n", minAxis.x, minAxis.y, minAxis.z, minOverlap);
	return pair<vec3, float>(minAxis, minOverlap);
}


int PhysicsSystem::getNextId() {
	nextid++;
	return nextid-1;
}

GameObject* PhysicsSystem::getPlayerObjectById(int id) {
	for (auto obj : playerObjects) {
		if (obj->id == id) {
			return obj;
		}
	}
	return nullptr;
}

vector<vec3> getAABBVertices(const AABB &aabb) {
	vector<vec3> vertices(8);
	vec3 min = aabb.min;
	vec3 max = aabb.max;

	vertices[0] = vec3(min.x, min.y, min.z);
	vertices[1] = vec3(max.x, min.y, min.z);
	vertices[2] = vec3(min.x, max.y, min.z);
	vertices[3] = vec3(max.x, max.y, min.z);
	vertices[4] = vec3(min.x, min.y, max.z);
	vertices[5] = vec3(max.x, min.y, max.z);
	vertices[6] = vec3(min.x, max.y, max.z);
	vertices[7] = vec3(max.x, max.y, max.z);

	return vertices;
}

vector<vec4> convertToWorldSpaceAABB(const AABB& aabb, const glm::vec3& position, const glm::quat& rotation) {
	vector<vec3> vertices = getAABBVertices(aabb);
	vector<vec4> worldSpaceVertices(vertices.size());

	for (size_t i = 0; i < vertices.size(); ++i) {
		vec4 vertex = glm::vec4(vertices[i], 1.0f);
		worldSpaceVertices[i] = rotation * vertex + vec4(position, 0.0f);
	}

	return worldSpaceVertices;
}