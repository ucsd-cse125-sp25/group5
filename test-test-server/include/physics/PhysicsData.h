#include <stdafx.h>
#include "../include/shared/ObjectData.h"


#pragma once
using namespace std;

typedef glm::vec3 vec3;

// Forward declarations for components
struct PhysicsComponent;
struct ColliderComponent;
struct TestTestBehaviorComponent;
class BehaviorComponent;

const float GRAVITY = 9.8f * 0.1f;

struct AABB {
	vec3 min;
	vec3 max;
};

struct Transform {
	glm::vec3 position = glm::vec3(0.0f,0.0f,0.0f);
	//glm::vec3 rotation = glm::vec3(0.0f,0.0f,0.0f);
	//glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); // Quaternion for rotation
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

enum class MoveState {
	Idle,
	Walking,
	Dashing,
	Grappling,
	InAir
};

struct TestTestBehaviorComponent {
	MoveState state = MoveState::Idle;

	float dashCooldown = 0.0f;
	float grappleCooldown = 0.0f;
};

struct GameObject {
	int id;
	Transform transform;
	PhysicsComponent* physics = nullptr;
	ColliderComponent* collider = nullptr;
	//TestTestBehaviorComponent* behavior = nullptr;
	BehaviorComponent* behavior = nullptr; // Pointer to the behavior component
	EntityType type = EntityType::ENTITY; // Default to ENTITY

	bool markDeleted = false; // true if marked for deletion
};

