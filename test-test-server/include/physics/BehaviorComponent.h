#pragma once
#include "PhysicsData.h"  // Assuming you have a class that represents objects in your game world
#include "PhysicsSystem.h"

//define a constant here for jump force


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

public:
    // just forward to the base
    PlayerBehaviorComponent(GameObject* self, PhysicsSystem& physicsSystem)
        : BehaviorComponent(self, physicsSystem)
    {
    }

    // override the abstract methods
    void integrate(GameObject* obj, float deltaTime, PhysicsSystem& phys) override;
    void handleCollision(GameObject* obj, const GameObject& other) override;
};