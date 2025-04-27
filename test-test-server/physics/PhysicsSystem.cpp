#include "physics/PhysicsSystem.h"
#include "physics/PhysicsData.h"	
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/matrix_decompose.hpp>



glm::vec3 bezier(const glm::vec3& A, const glm::vec3& B, const glm::vec3& C, float t) {
    glm::vec3 AB = glm::mix(A, B, t);
    glm::vec3 BC = glm::mix(B, C, t);
    return glm::mix(AB, BC, t);
}

bool AABBOverlap(const AABB& a, const AABB& b) {
    return (
        a.min.x <= b.max.x && a.max.x >= b.min.x &&
        a.min.y <= b.max.y && a.max.y >= b.min.y &&
        a.min.z <= b.max.z && a.max.z >= b.min.z
        );
}

void PhysicsSystem::tick(float dt) {
    for (GameObject* obj : dynamicObjects) {
        // Movement input already applied in applyInput()

        if (obj->physics->grappling) {
            handleGrapple(obj, dt);
            continue;
        }

        // Apply gravity
        obj->physics->acceleration += glm::vec3(0, -GRAVITY * obj->physics->gravityScale, 0);

        // Integrate
        integrate(obj, dt);

        // Collide + resolve
        checkCollisions(obj);
        //resolveCollisions(obj);

        // Reset per-frame data
        obj->physics->acceleration = glm::vec3(0);
    }
}

void PhysicsSystem::integrate(GameObject* obj, float dt) {
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


void PhysicsSystem::handleGrapple(GameObject* obj, float dt) {
    PhysicsComponent* phys = obj->physics;
    phys->grappleTimer += dt;

	float t = glm::clamp(phys->grappleTimer / 0.6f, 0.0f, 1.0f);
    glm::vec3 start = obj->transform.position;
    glm::vec3 mid = (start + phys->grappleTarget) * 0.5f + glm::vec3(0, 5.0f, 0);
	glm::vec3 newPos = bezier(start, mid, phys->grappleTarget, t);

	phys->velocity = (newPos - obj->transform.position) / dt;
    obj->transform.position = newPos;

	if (t >= 1.0f) {
		phys->grappling = false;
		phys->grappleTimer = 0.0f;
	}
}

AABB getAABB(GameObject* obj) {
    glm::vec3 min = obj->transform.position - obj->collider->halfExtents;
    glm::vec3 max = obj->transform.position + obj->collider->halfExtents;
    return { min, max };
}

glm::vec3 getPenetrationVector(const AABB& a, const AABB& b) {
    glm::vec3 pen;

    pen.x = std::min(a.max.x - b.min.x, b.max.x - a.min.x);
    pen.y = std::min(a.max.y - b.min.y, b.max.y - a.min.y);
    pen.z = std::min(a.max.z - b.min.z, b.max.z - a.min.z);

    // Get signs
    pen.x *= (a.min.x < b.min.x) ? -1.0f : 1.0f;
    pen.y *= (a.min.y < b.min.y) ? -1.0f : 1.0f;
    pen.z *= (a.min.z < b.min.z) ? -1.0f : 1.0f;

    return pen;
}

void PhysicsSystem::checkCollisions(GameObject* obj) {
    if (!obj->collider || !obj->physics) return;

    AABB a = getAABB(obj);

    for (GameObject* platform : staticObjects) {
        if (!platform->collider) continue;

        AABB b = getAABB(platform);

        if (!AABBOverlap(a, b)) continue;

        glm::vec3 penetration = getPenetrationVector(a, b);

        // Resolve by shallowest axis
        if (std::abs(penetration.x) < std::abs(penetration.y) &&
            std::abs(penetration.x) < std::abs(penetration.z)) {
            obj->transform.position.x += penetration.x;
            obj->physics->velocity.x = 0;
        }
        else if (std::abs(penetration.y) < std::abs(penetration.z)) {
            obj->transform.position.y += penetration.y;
            obj->physics->velocity.y = 0;

            // If pushing upward, consider it grounded
            if (penetration.y > 0) {
                obj->physics->grounded = true;
            }
        }
        else {
            obj->transform.position.z += penetration.z;
            obj->physics->velocity.z = 0;
        }

        // Update the new AABB for further collisions
        a = getAABB(obj);
    }
}

void PhysicsSystem::resolveCollisions(GameObject* o) {
    return;
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

   /* if (intent.moveLeftIntent)
    {

		target->transform.position.x -= 0.1f;
    }*/

    glm::vec3 delta = glm::vec3(0.016f);
    float azimuth = glm::radians(-intent.azimuthIntent);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), azimuth, up);
    glm::vec3 forward = glm::normalize(glm::vec3(rotation * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)));
    //glm::vec3 translation = glm::vec3(target.cubeModel[3]);
	glm::vec3 translation = target->transform.position;
    glm::vec3 right = glm::normalize(glm::cross(up, forward));

    //GameState.cubeModel = glm::rotate(GameState.cubeModel, azimuth, glm::vec3(0.0f, 1.0f, 0.0f));
    

    //process
    if (intent.moveLeftIntent)
    {
        translation += (-right) * delta;
        //GameState.setModelMatrix(glm::translate(GameState.getModelMatrix(), glm::vec3(-0.1f, 0.0f, 0.0f));
        //GameState.cubeModel = glm::translate(GameState.cubeModel, glm::vec3(-0.1f, 0.0f, 0.0f));
    }
    if (intent.moveRightIntent) {
        translation += right * delta;
        //GameState.cubeModel = glm::translate(GameState.cubeModel, glm::vec3(0.1f, 0.0f, 0.0f));
        //GameState.setModelMatrix(glm::translate(GameState.getModelMatrix(), glm::vec3(0.1f, 0.0f, 0.0f)));
    }
    if (intent.moveUpIntent) {
        translation += up * delta;
        //GameState.cubeModel = glm::translate(GameState.cubeModel, glm::vec3(0.0f, 0.1f, 0.0f));
        //GameState.setModelMatrix(glm::translate(GameState.getModelMatrix(), glm::vec3(0.0f, 0.1f, 0.0f)));

    }
    if (intent.moveDownIntent) {
        translation += (-up) * delta;
        //GameState.cubeModel = glm::translate(GameState.cubeModel, glm::vec3(0.0f, -0.1f, 0.0f));
        //GameState.setModelMatrix(glm::translate(GameState.getModelMatrix(), glm::vec3(0.0f, -0.1f, 0.0f)));
    }
    if (intent.moveForwardIntent) {
        translation += (-forward) * delta;
        //GameState.cubeModel = glm::translate(GameState.cubeModel, glm::vec3(0.0f, 0.0f, -0.1f));
    }
    if (intent.moveBackIntent) {
        translation += forward * delta;
        //GameState.cubeModel = glm::translate(GameState.cubeModel, glm::vec3(0.0f, 0.0f, 0.1f));
    }

    //glm::mat4 id = glm::mat4(1.0f);
    //GameState.cubeModel = glm::translate(GameState.cubeModel, translation);

    // Update the line causing the error to properly convert the quaternion to a vec4  
    //target->transform.rotation = glm::vec4(glm::quat_cast(rotation).x, glm::quat_cast(rotation).y, glm::quat_cast(rotation).z, glm::quat_cast(rotation).w);
    glm::quat q = glm::angleAxis(glm::radians(-intent.azimuthIntent), glm::vec3(0, 1, 0));
    target->transform.rotation = q;
	target->transform.position = translation;



}

GameObject* PhysicsSystem::makeGameObject() {
    GameObject* obj = new GameObject;
    obj->id = dynamicObjects.size();
    obj->transform.position = glm::vec3(0.0f);
	obj->transform.rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); // Identity quaternion
    obj->transform.scale = glm::vec3(1.0f);
    obj->physics = new PhysicsComponent();
    obj->collider = new ColliderComponent();

    return obj; // return reference to the stored one
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




//test