#include "physics/PhysicsSystem.h"
#include "physics/PhysicsData.h"	
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include "physics/BehaviorComponent.h"
#include <iostream>

typedef glm::vec3 vec3;
typedef glm::vec4 vec4;
typedef glm::mat3 mat3;
typedef glm::mat4 mat4;
typedef glm::quat quat;

using namespace std;

int nextid = 10;

class BehaviorComponent;    // Forward declaration of BehaviorComponent class

void PhysicsSystem::tick(float dt) {
    if (octreeMovingObjects == nullptr || octreeStaticObjects == nullptr) {
        broadphaseInit();
    }

    // Update all dynamic objects
    for (size_t i = 0; i < movingObjects.size(); ++i) {
        GameObject* obj = movingObjects[i];
        obj->physics->acceleration += glm::vec3(0, -GRAVITY * obj->physics->gravityScale, 0);
        integrate(obj, dt);

    }

    for (auto& obj : movingObjects) {
		updateGameObjectAABB(obj);
		octreeMovingObjects->updateObject(obj);
	}

	// Reset acceleration for all objects
	for (auto& obj : movingObjects) {
		obj->physics->acceleration = glm::vec3(0.0f);
	}

    checkCollisionDynamicAll();

    updateWaterLevel();

    //delete all objects marked for deletion
	deleteMarkedDynamicObjects();
	//deleteAllObjectsMarked();

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

//void PhysicsSystem::handleCollisions(GameObject* obj) {
//	// Check for collisions with static objects
//	for (auto sobj : staticObjects) {
//		AABB objAABB = getAABB(obj);
//		AABB sobjAABB = getAABB(sobj);
//		
//		pair<vec3, float> penetration = getAABBpenetration(objAABB, sobjAABB);
//		if (penetration.second > 0.0f) {
//			if (obj->behavior != nullptr) {
//				obj->behavior->resolveCollision(obj, sobj, penetration, 0);
//			} else {
//                resolveCollision(obj, sobj, penetration, 0);
//            }
//        }  
//    }
//	
//	for (auto dobj : movingObjects) {
//		if (obj->id == dobj->id) {
//			continue; // Skip self-collision
//		}
//		AABB objAABB = getAABB(obj);
//		AABB dobjAABB = getAABB(dobj);
//
//		pair<vec3, float> penetration = getAABBpenetration(objAABB, dobjAABB);
//		if (penetration.second > 0.0f) {
//			/*resolveCollision(obj, dobj, penetration, 1);*/
//			if (obj->behavior != nullptr) {
//				obj->behavior->resolveCollision(obj, dobj, penetration, 1);
//			}
//		}
//	}
//	return;
//}

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

    if (toRet == nullptr) {
		//return a dummy GameObject if no player is found
		toRet = makeGameObject();
		toRet->transform.position = pos; // Set position to the input position
		toRet->id = exclude; // Set ID to the excluded player ID
		toRet->isDynamic = false; // Mark as static
		toRet->collider->halfExtents = glm::vec3(0.1f); // Small collider for dummy object
    }
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

static float getOverlap(pair<float,float> interval1, pair<float,float> interval2) {
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
    vec3 axes[3] = { vec3(1, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1) };

	float minOverlap = 999999.0f;
	vec3 minAxis = vec3(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < 3; i++) {
        pair<float, float> interval1 = { a.min[i], a.max[i] };
        pair<float, float> interval2 = { b.min[i], b.max[i] };


		vec3 dir = getAABBDistanceCenters(a, b);

        float overlap = getOverlap(interval1, interval2);

		if (overlap <= 0.0f) {   
			return pair<vec3, float>(vec3(0.0f), overlap); // No overlap
		}

        if (overlap < minOverlap) {
            minOverlap = overlap;
            minAxis = axes[i] * (dir[i] > 0 ? 1.0f : -1.0f); // Choose the axis direction based on the distance vector
        }
    }
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

vector<vec3> PhysicsSystem::getAABBVerticesForMesh(const AABB &aabb)
{
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

vector<vec4> PhysicsSystem::convertToWorldSpaceAABB(const AABB& aabb, const glm::vec3& position, const glm::quat& rotation) {
    vector<vec3> vertices = getAABBVerticesForMesh(aabb);
    vector<vec4> worldSpaceVertices(vertices.size());

    for (size_t i = 0; i < vertices.size(); ++i) {
        vec4 vertex = vec4(vertices[i], 1.0f); // Convert to vec4
        vertex = rotation * vertex; // Rotate the vertex
        vertex += vec4(position, 0.0f); // Translate the vertex
        worldSpaceVertices[i] = vertex;
    }

	return worldSpaceVertices;
}


float getMeshMinOrMaxCoord(const vector<vec3>& positions, int coord, bool isMin) {
    assert(positions.size() > 0);
    assert(coord >= 0 && coord < 3);
    
    float result = positions[0][coord];
    for (const auto& pos : positions) {
        if (isMin) {
            result = std::min(result, pos[coord]);
        } else {
            result = std::max(result, pos[coord]);
        }
    }
    return result;
}

static vec3 getPosition(const vec3& start, const float width, const int directionOfSlice) {
    vec3 change = vec3(0.0f);
	change[directionOfSlice] = width/2.0f;
	return start + change; 
}

static vec3 getHalfExtents(const vec3& halfExtents, const int directionOfSlice, const float width) {
	vec3 halfExs = halfExtents;
	halfExs[directionOfSlice] = width * 0.5f;
	return halfExs;
}

void PhysicsSystem::generateGameObjectsForWholeThing(const vec3& position, const vec3& halfExtents, int numObjects, const int directionOfSlice, const quat& rotation) {
	float sliceWidth = halfExtents[directionOfSlice] * 2.0f / numObjects;
	vec3 startPoint = position - halfExtents;
	vec3 currPoint = startPoint;
    for (int i = 0; i < numObjects; i++) {
		makeGameObject(getPosition(currPoint, sliceWidth, directionOfSlice), rotation, getHalfExtents(halfExtents, directionOfSlice, sliceWidth));
		currPoint[directionOfSlice] += sliceWidth;
    }
}

AABB PhysicsSystem::getMeshAABB(const vector<vec3>& positions, GameObject* obj) {
    if (positions.empty()) {
        return { vec3(0.0f), vec3(0.0f) }; // Return an empty AABB if no positions are provided
    }
    return { vec3(getMeshMinOrMaxCoord(positions, 0, true), getMeshMinOrMaxCoord(positions, 1, true), getMeshMinOrMaxCoord(positions, 2, true)),
             vec3(getMeshMinOrMaxCoord(positions, 0, false), getMeshMinOrMaxCoord(positions, 1, false), getMeshMinOrMaxCoord(positions, 2, false)) };
}

void PhysicsSystem::updateGameObjectAABB(GameObject* obj) {
    if (!obj || !obj->collider) {
        return; // Ensure the object and its collider are valid
    }

	//std::cout << "Inside updateGameObjectAABB: obj = " << obj << std::endl;
	//if (obj) std::cout << "obj->id = " << obj->id << std::endl;

    obj->transform.aabb = getAABB(obj);
}

void PhysicsSystem::updateGameObjectsAABB(vector<GameObject*>& objects) {
    for (auto obj : objects) {
        updateGameObjectAABB(obj);
    }   
}

void PhysicsSystem::initOctree(vector<GameObject*>& objects, Octree*& octree) {
    updateGameObjectsAABB(objects);
	if (octree == nullptr) {
		octree = new Octree(worldBounds, objects, 8, 24);
	}
    octree->constructTree(objects);
}

void PhysicsSystem::broadphaseInit() {
	initOctree(staticObjects, octreeStaticObjects);
	initOctree(movingObjects, octreeMovingObjects);
}

void PhysicsSystem::checkCollisionOne(Octree* octree, vector<GameObject*>& objects, GameObject* obj, int status) {
    if (!octree || !obj || !obj->collider) return;
    
    vector<GameObject*> potentialCollisions;
    octree->getPotentialCollisionPairs(getAABB(obj), potentialCollisions);

    for (auto& otherObj : potentialCollisions) {
        // if (otherObj != obj && obj->id < otherObj->id) {
		if (otherObj->id != obj->id) { 
			updateGameObjectAABB(otherObj);
			// std::cout << "Calling updateGameObjectAABB: obj = " << obj << ", id = " << obj->id << std::endl;
			updateGameObjectAABB(obj);
            pair<vec3, float> penetration = getAABBpenetration(obj->transform.aabb, otherObj->transform.aabb);
            if (penetration.second > 0.0f) {
				if (obj->behavior != nullptr) {
					obj->behavior->resolveCollision(obj, otherObj, penetration, status);
				} else if (status == 0) {
					resolveCollision(obj, otherObj, penetration, status);
				}
            }
        }
    }
}

void PhysicsSystem::checkCollisionDynamicOne(GameObject* dynamicObj) {
	checkCollisionOne(octreeStaticObjects, staticObjects, dynamicObj, 0);
    checkCollisionOne(octreeMovingObjects, movingObjects, dynamicObj, 1);
	dynamicObj->physics->acceleration = glm::vec3(0);
}

void PhysicsSystem::checkCollisionDynamicAll() {
    updateGameObjectsAABB(movingObjects);
    for (auto& obj : movingObjects) {
        checkCollisionDynamicOne(obj);
    }
}

void deleteObject(GameObject* obj) {
	if (obj) {
		if (obj->behavior) {
			delete obj->behavior; // Delete the behavior component if it exists
		}
		if (obj->collider) {
			delete obj->collider; // Delete the collider component if it exists
		}
		if (obj->physics) {
			delete obj->physics; // Delete the physics component if it exists
		}
		if (obj->mesh) {
			delete obj->mesh; // Delete the mesh component if it exists
		}
		if (obj->attached) {
			delete obj->attached; // Delete the attached object if it exists
		}
		delete obj;
	}
}

void PhysicsSystem::deleteAllObjectsMarked() {
	if (dynamicObjects.size() > 0) {
		for (auto& obj : dynamicObjects) {
			if (obj->markDeleted) {
				if (octreeMovingObjects) {
					octreeMovingObjects->deleteObject(obj);
				}
				movingObjects.erase(remove(movingObjects.begin(), movingObjects.end(), obj), movingObjects.end());
				dynamicObjects.erase(remove(dynamicObjects.begin(), dynamicObjects.end(), obj), dynamicObjects.end());
				deleteObject(obj);
			}
		}
	}

	if (staticObjects.size() > 0) {
		for (auto& obj : staticObjects) {
			if (obj->markDeleted) {
				if (octreeStaticObjects) {
					octreeStaticObjects->deleteObject(obj);
				}
				staticObjects.erase(remove(staticObjects.begin(), staticObjects.end(), obj), staticObjects.end());
				deleteObject(obj);
			}
		}
	}

	if (playerObjects.size() > 0) {
		for (auto& obj : playerObjects) {
			if (obj->markDeleted) {
				if (octreeMovingObjects) {
					octreeMovingObjects->deleteObject(obj);
				}
				playerObjects.erase(remove(playerObjects.begin(), playerObjects.end(), obj), playerObjects.end());
				movingObjects.erase(remove(movingObjects.begin(), movingObjects.end(), obj), movingObjects.end());
				deleteObject(obj);
			}
		}
	}
	
	if (movingObjects.size() != dynamicObjects.size() + playerObjects.size()) {
		printf("Error: movingObjects size does not match dynamicObjects and playerObjects sizes.\n");
		printf("movingObjects size: %zu, dynamicObjects size: %zu, playerObjects size: %zu\n", 
			movingObjects.size(), dynamicObjects.size(), playerObjects.size());
	}




}