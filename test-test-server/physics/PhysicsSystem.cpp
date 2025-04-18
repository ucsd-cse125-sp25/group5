#include "physics/PhysicsSystem.h"
#include "physics/PhysicsData.h"	



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
        resolveCollisions(obj);

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



//test