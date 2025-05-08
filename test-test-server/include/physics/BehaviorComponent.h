#pragma once
#include "PhysicsData.h"  // Assuming you have a class that represents objects in your game world
#include "PhysicsSystem.h"

//define a constant here for jump force


enum class PlayerMovementState {
	IDLE,
	STOMP,
    DASH,
    GRAPPLE
};

class BehaviorComponent {  
   

public:  
    PhysicsSystem& physicsSystem;
	PlayerIntentPacket PlayerIntent;
    GameObject* self;

	explicit BehaviorComponent(GameObject* self, PhysicsSystem& physicsSystem) : self(self), physicsSystem(physicsSystem)
	{
		this->self = self;
		this->physicsSystem = physicsSystem;
	}

   virtual ~BehaviorComponent() {}  

   // Called every update tick to advance physics or custom logic  
   virtual void integrate(GameObject* self, float deltaTime, PhysicsSystem& physicsSystem) = 0;  

   // Called when a collision is detected with another object  
   virtual void handleCollision(GameObject* self, const GameObject& other) = 0;  
};

class PlayerBehaviorComponent : public BehaviorComponent {

const float JUMP_FORCE = 10.0f;
const float DASH_TIME = 0.5f;
const float DASH_SPEED = 20.0f;
const float STOMP_TIME = 3.0f;
const float STOMP_SPEED = 30.0f;
const float GRAPPLE_SPEED = 15.0f;
const float GRAPPLE_TIME = 10.0f;


public:
	PlayerMovementState state = PlayerMovementState::IDLE;
    float dashTimer = 0.0f;
	float stompTimer = 0.0f;
    float grappleTimer = 0.0f;
	GameObject* grappleTarget = nullptr;
    // just forward to the base
    PlayerBehaviorComponent(GameObject* self, PhysicsSystem& physicsSystem)
        : BehaviorComponent(self, physicsSystem)
    {
    }

	pair<glm::vec3,float> handlePlayerGrapple(GameObject* obj, PhysicsSystem& phys);

    // override the abstract methods
    void integrate(GameObject* obj, float deltaTime, PhysicsSystem& phys) override;
    void handleCollision(GameObject* obj, const GameObject& other) override;
};

class ProjectileBehaviorComponent : public BehaviorComponent {

public:
	float speed;
	float damage;

	ProjectileBehaviorComponent(GameObject* self, PhysicsSystem& physicsSystem, float speed, float damage)
		: BehaviorComponent(self, physicsSystem), velocity(velocity), damage(damage)
	{
	}
	void integrate(GameObject* obj, float deltaTime, PhysicsSystem& phys) override;
	void handleCollision(GameObject* obj, const GameObject& other) override;
}