#include <stdafx.h>



#pragma once
// Forward declarations for components
struct PhysicsComponent;
struct ColliderComponent;
struct BehaviorComponent;

const float GRAVITY = 9.8f;

struct AABB {
	glm::vec3 min;
	glm::vec3 max;
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

enum class MoveState {
	Idle,
	Walking,
	Dashing,
	Grappling,
	InAir
};

struct BehaviorComponent {
	MoveState state = MoveState::Idle;

	float dashCooldown = 0.0f;
	float grappleCooldown = 0.0f;
};

struct GameObject {
	int id;
	Transform transform;
	PhysicsComponent* physics = nullptr;
	ColliderComponent* collider = nullptr;
	BehaviorComponent* behavior = nullptr;

	bool isDynamic = true; // true if dynamic, false if static
};

