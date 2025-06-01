#include <stdafx.h>
#include "../include/shared/ObjectData.h"
#include "physics/AABB.h"

#pragma once
using namespace std;

typedef glm::vec3 vec3;

// Forward declarations for components
struct PhysicsComponent;
struct ColliderComponent;
class BehaviorComponent;

const float GRAVITY = 9.8f * 0.1f;

struct Transform {
	glm::vec3 position = glm::vec3(0.0f,0.0f,0.0f);
	glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	glm::vec3 scale = glm::vec3(1.0f);
};

struct PhysicsComponent {
	glm::vec3 velocity = glm::vec3(0.0f);
	glm::vec3 acceleration = glm::vec3(0.0f);

	bool grounded = false;
	bool grappling = false;
	float grappleTimer = 0.0f;
	glm::vec3 grappleTarget;

	float maxSpeed = 10.0f;
	float gravityScale = 1.0f;
	float drag = 0.2f;
};

struct ColliderComponent {
	glm::vec3 halfExtents = glm::vec3(0.5f, 0.9f, 0.5f); // width, height, depth
	bool isTrigger = false; // no resolution if true
};

struct Ray {
	glm::vec3 origin;
	glm::vec3 dir; // *must* be normalized
};

struct GameObject {
	int id;
	Transform transform;
	PhysicsComponent* physics = nullptr;
	ColliderComponent* collider = nullptr;
	BehaviorComponent* behavior = nullptr; // Pointer to the behavior component
	EntityType type = EntityType::ENTITY; // Default to ENTITY

	GameObject* attached = nullptr; // flag purposes

	bool isDynamic = true; // true if dynamic, false if static
	bool markDeleted = false; // true if marked for deletion
};

