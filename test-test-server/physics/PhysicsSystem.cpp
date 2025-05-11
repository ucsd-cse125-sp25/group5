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

int nextid = 4;

//std::array<std::pair<int,int>,12> edges = {{
//    {0,1},{1,3},{3,2},{2,0},   // bottom face
//    {4,5},{5,7},{7,6},{6,4},   // top face
//    {0,4},{1,5},{2,6},{3,7}    // vertical pillars
//}};

class BehaviorComponent;

/**
 * Cubic Bezier curve function
 * @param A Start point
 * @param B Control point 1
 * @param C Control point 2
 * @param t Parameter (0 <= t <= 1), where t represents the interpolation factor
 * @return Point on the curve at parameter t
 */
glm::vec3 bezier(const glm::vec3& A, const glm::vec3& B, const glm::vec3& C, float t) {
    glm::vec3 AB = glm::mix(A, B, t);
    glm::vec3 BC = glm::mix(B, C, t);
    return glm::mix(AB, BC, t);
}

/** 
 * update the physics system for each dynamic object
 * @param dt delta time
 * @return void
*/
void PhysicsSystem::tick(float dt) {
    // check for dt being too large to avoid large jumps
    if (dt > 0.1f) {
        dt = 0.1f;
    } else if (dt < 0.0f) {
        dt = 0.0f;
    }

    // Update all dynamic objects
    for (GameObject* obj : movingObjects) {

        // Apply gravity
        obj->physics->acceleration += glm::vec3(0, -GRAVITY * obj->physics->gravityScale, 0);

        // Integrate
        integrate(obj, dt);
    }

    // After integration is complete for all objects, start handling collision
    for (GameObject* obj : movingObjects) {
        // Collide + resolve
        handleCollisions(obj);

        // Reset per-frame data
        obj->physics->acceleration = glm::vec3(0);
    }
}

void PhysicsSystem::defaultIntegrate(GameObject* obj, float dt) {
    assert(obj != NULL);

    // apply force 
    obj->physics->velocity += obj->physics->acceleration * dt;

    //apply drag
    obj->physics->velocity *= (1.0f - obj->physics->drag * dt);

    //clamp velocity
    if (glm::length(obj->physics->velocity) > obj->physics->maxSpeed) {
        obj->physics->velocity = glm::normalize(obj->physics->velocity) * obj->physics->maxSpeed;
    }

    obj->transform.position += obj->physics->velocity * dt;

    obj->transform.aabb = getAABB(obj);
}

void PhysicsSystem::integrate(GameObject* obj, float dt) {
	if (obj->behavior != nullptr) {
		obj->behavior->integrate(obj, dt, *this);
	}
	else {
		defaultIntegrate(obj, dt);
	}
}

/**
 * Handle grapple movement
 * @param obj GameObject to be grappling
 * @param dt delta time
 * @return void
*/
void PhysicsSystem::handleGrapple(GameObject* obj, float dt) {
    assert(obj != NULL);
}

/**
 * Get the Axis-Aligned Bounding Box (AABB) of a GameObject
 * @param obj GameObject to get the AABB from
 * @return AABB of the GameObject
 * @note The AABB is calculated based on the position and half extents of the collider
 * @note The AABB is represented as a pair of vectors: min and max
 * @note The min vector is the minimum corner of the AABB
 * @note The max vector is the maximum corner of the AABB
 */
AABB PhysicsSystem::getAABB(GameObject* obj) {
    assert(obj != NULL);
    glm::vec3 min = obj->transform.position - obj->collider->halfExtents;
    glm::vec3 max = obj->transform.position + obj->collider->halfExtents;
    return { min, max };
}

/**
 * Handle collisions between a dynamic object and all static objects
 * @param obj dynamic GameObject to check for collisions
 * @return void
 * @note This function checks for collisions between the dynamic object and all static objects in the scene
*/
void PhysicsSystem::handleCollisions(GameObject* obj) {
    assert(obj != NULL);

    // Check for collisions with static objects
    for (auto sobj : staticObjects) {
        pair<vec3, float> penetration = getAABBpenetration(obj->transform.aabb, sobj->transform.aabb);
        if (penetration.second > 0.0f) {
            resolveCollision(obj, sobj, penetration, 0);
            //printf("Detected collision between %d and %d\n", obj->id, sobj->id);
        }  
		//printf("Detected collision between %d and %d\n", obj->id, sobj->id);
    }
	//printf("length of static objects %d", int(staticObjects.size()));

    //// Check for collisions between dynamic objects
    //for (auto dobj : movingObjects) {
    //    if (obj->id == dobj->id) {
    //        continue; // Skip self-collision
    //    }
    //    pair<vec3, float> penetration = getAABBpenetration(obj->transform.aabb, dobj->transform.aabb);
    //    if (penetration.second > 0.0f) {
    //        resolveCollision(obj, dobj, penetration, 1);
    //    }
    //}
    return;
}

/**
 * Get the impulse vector for collision resolution
 * @param normal Normal vector of the penetration vector
 * @param relativeVelocity Relative velocity of the two objects
 * @param restitution Coefficient of restitution (bounciness)
 * @return Impulse vector to apply to the object
 */
vec3 PhysicsSystem::getImpulseVector(const vec3& normal, const vec3& relativeVelocity, float restitution) {
    float velAlongNormal = glm::dot(relativeVelocity, normal);
    if (velAlongNormal < 0.0f) {
        float impulse = -(1.0f + restitution) * velAlongNormal;
        return impulse * normal;
    }
    return vec3(0.0f);
}

void PhysicsSystem::resolveCollision(GameObject* go1, GameObject* go2, const pair<vec3, float>& penetration, int status) {
    assert (go1 != NULL && go2 != NULL);

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

    // Update AABBs
    go1->transform.aabb = getAABB(go1);
    go2->transform.aabb = getAABB(go2);
}

vec3 PhysicsSystem::getInputVelocity(const PlayerIntentPacket& intent, int playerId) {
	//process player input
	GameObject* target = getPlayerObjectById(playerId);
    if (target == NULL) {
        return glm::vec3(0.0f);
    }
	
	float azimuth = glm::radians(-intent.azimuthIntent);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), azimuth, up);
	glm::vec3 forward = glm::normalize(glm::vec3(rotation * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)));
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
    
/**
 * Apply player input to the GameObject
 * @param intent PlayerIntentPacket containing the input data
 * @param playerId ID of the player
 * @return void
 * 
*/
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
    //glm::vec3 translation = glm::vec3(target.cubeModel[3]);
	glm::vec3 translation = target->transform.position;
    glm::vec3 right = glm::normalize(glm::cross(up, forward));

    //GameState.cubeModel = glm::rotate(GameState.cubeModel, azimuth, glm::vec3(0.0f, 1.0f, 0.0f));
    

    //process
  //  if (intent.moveLeftIntent)
  //  {
		//target->physics->velocity += (-right) * delta;  
  //      //translation += (-right) * delta;
  //      //GameState.setModelMatrix(glm::translate(GameState.getModelMatrix(), glm::vec3(-0.1f, 0.0f, 0.0f));
  //      //GameState.cubeModel = glm::translate(GameState.cubeModel, glm::vec3(-0.1f, 0.0f, 0.0f));
  //  }
  //  if (intent.moveRightIntent) {
		//target->physics->velocity += right * delta;
  //      //translation += right * delta;
  //      //GameState.cubeModel = glm::translate(GameState.cubeModel, glm::vec3(0.1f, 0.0f, 0.0f));
  //      //GameState.setModelMatrix(glm::translate(GameState.getModelMatrix(), glm::vec3(0.1f, 0.0f, 0.0f)));
  //  }
  //  if (intent.moveUpIntent) {
  //      translation += up * delta;
  //      //GameState.cubeModel = glm::translate(GameState.cubeModel, glm::vec3(0.0f, 0.1f, 0.0f));
  //      //GameState.setModelMatrix(glm::translate(GameState.getModelMatrix(), glm::vec3(0.0f, 0.1f, 0.0f)));
  //      
  //  }
  //  if (intent.moveDownIntent) {
  //      translation += (-up) * delta;
  //      //GameState.cubeModel = glm::translate(GameState.cubeModel, glm::vec3(0.0f, -0.1f, 0.0f));
  //      //GameState.setModelMatrix(glm::translate(GameState.getModelMatrix(), glm::vec3(0.0f, -0.1f, 0.0f)));
  //  }
  //  if (intent.moveForwardIntent) {
		//target->physics->velocity += -forward * delta;
  //      //translation += (-forward) * delta;
  //      //GameState.cubeModel = glm::translate(GameState.cubeModel, glm::vec3(0.0f, 0.0f, -0.1f));
  //  }
  //  if (intent.moveBackIntent) {
		//target->physics->velocity += (forward) * delta;
  //      //translation += forward * delta;
  //      //GameState.cubeModel = glm::translate(GameState.cubeModel, glm::vec3(0.0f, 0.0f, 0.1f));
  //  }

    //glm::mat4 id = glm::mat4(1.0f);
    //GameState.cubeModel = glm::translate(GameState.cubeModel, translation);

    // Update the line causing the error to properly convert the quaternion to a vec4  
    //target->transform.rotation = glm::vec4(glm::quat_cast(rotation).x, glm::quat_cast(rotation).y, glm::quat_cast(rotation).z, glm::quat_cast(rotation).w);
    glm::quat q = glm::angleAxis(glm::radians(-intent.azimuthIntent), glm::vec3(0, 1, 0));
    target->transform.rotation = q;
	//target->transform.position = translation;
    target->transform.aabb = getAABB(target);

}

GameObject* PhysicsSystem::makeGameObject() {
    GameObject* obj = new GameObject;
    obj->id = getNextId();
    obj->transform.position = glm::vec3(0.0f);
	obj->transform.rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); // Identity quaternion
    obj->transform.scale = glm::vec3(1.0f);

    obj->physics = new PhysicsComponent();
    obj->collider = new ColliderComponent();

    obj->collider->halfExtents = glm::vec3(1.0f, 1.0f, 1.0f);

    obj->isDynamic = false;
    obj->transform.aabb = getAABB(obj);

    return obj; // return reference to the stored one
}


GameObject* PhysicsSystem::makeGameObject(glm::vec3 position, glm::quat rotation, glm::vec3 halfExtents) {

	GameObject* obj = makeGameObject();

	obj->transform.position = position;
	obj->transform.rotation = rotation;

	obj->collider->halfExtents = halfExtents;

	obj->transform.aabb = getAABB(obj);

    return obj;
}

// Convert position + quaternion to mat4
glm::mat4 PhysicsSystem::toMatrix(const glm::vec3& position, const glm::quat& quaternion) {
    glm::mat4 T = glm::translate(glm::mat4(1.0f), position);
	glm::mat4 R = glm::toMat4(glm::quat(quaternion));
	//glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
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

pair<vec3, float> PhysicsSystem::getAABBpenetration(AABB&  a, AABB&b) {
    vec3 axes[3] = { vec3(1, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1) };

    float minOverlap = 999999.0f;
    vec3 minAxis = vec3(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < 3; i++) {
        pair<float, float> interval1 = { a.min[i], a.max[i] };
        pair<float, float> interval2 = { b.min[i], b.max[i] };

        float overlap = getOverlap(interval1, interval2);
        if (overlap <= 0.0f) {   
            return pair<vec3, float>(vec3(0.0f), overlap); // No overlap
        }

        if (overlap <= minOverlap) {
            minOverlap = overlap;
            minAxis = axes[i];
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

vector<vec3> getAABBVertices(const AABB& aabb) {
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



