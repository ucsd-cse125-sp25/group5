#pragma once
#include "PhysicsData.h"  // Assuming you have a class that represents objects in your game world
#include "PhysicsSystem.h"

//define a constant here for jump force


enum class PlayerMovementState {
	IDLE,
	STOMP,
    DASH,
    GRAPPLE,
	DEATH,
	MAGNET
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
   virtual void resolveCollision(GameObject* obj, GameObject* other, const pair<vec3, float>& penetration, int status) = 0;
};

class PickupBehaviorComponent : public BehaviorComponent {

public:

	EntityType type;
	float pickupTimer = 0.0f;
	glm::vec3 position;
	glm::vec3 restPosition;
	bool isActive = true;

	PickupBehaviorComponent(GameObject* self, PhysicsSystem& physicsSystem, EntityType type, glm::vec3 position)
		: BehaviorComponent(self, physicsSystem), type(type), position(position)
	{
		this->self = self;
		this->physicsSystem = physicsSystem;
		restPosition = position - vec3(0.0f, 1000.0f, 0.0f); // Adjust the rest position to be slightly above the original position
		self->type = type;
	}

	void integrate(GameObject* obj, float deltaTime, PhysicsSystem& phys) override;
	void resolveCollision(GameObject* obj, GameObject* other, const pair<vec3, float>& penetration, int status) override;
};


class PlayerBehaviorComponent : public BehaviorComponent {

const float JUMP_FORCE = 10.f;
const float DASH_TIME = 0.75f;
const float DASH_SPEED = 25.0f;
const float STOMP_TIME = 5.0f;
const float STOMP_SPEED = 20.0f;
const float GRAPPLE_SPEED = 10.0f;
const float GRAPPLE_TIME = 10.0f; //Bro does not use this :skull:
const float MAGNET_TIME = 4.0f;
const float MAGNET_SPEEED = 10.0f;
const float WOOD_PROJ_SPEED = 25.0f;


const float SLOW_TIME = 3.0f;
const float DEATH_TIME = 10.0f;


const float METAL_PROJ_COST = 10.0f;
const float METAL_MOVE_COST = 14.0f;
const float WOOD_PROJ_COST = 2.0f;
const float WOOD_MOVE_COST = 20.0f;
const float WATER_PROJ_COST = 5.0f;
const float WATER_MOVE_COST = 15.0f;
const float FIRE_PROJ_COST = 1.5f;
const float FIRE_MOVE_COST = 10.0f;
const float EARTH_PROJ_COST = 8.0f;
const float EARTH_MOVE_COST = 8.0f;


const float METAL_ATTACK_COOLDOWN = 0.9f;
const float WOOD_ATTACK_COOLDOWN = 0.25f;
const float WATER_ATTACK_COOLDOWN = 1.2f;
const float FIRE_ATTACK_COOLDOWN = 0.2f;
const float EARTH_ATTACK_COOLDOWN = 0.98f;
const float ATTACK_COOLDOWN_ARRAY[5] = { METAL_ATTACK_COOLDOWN, WOOD_ATTACK_COOLDOWN, WATER_ATTACK_COOLDOWN, FIRE_ATTACK_COOLDOWN, EARTH_ATTACK_COOLDOWN };
const float ATTACK_COST_ARRAY[5] = { METAL_PROJ_COST, WOOD_PROJ_COST, WATER_PROJ_COST, FIRE_PROJ_COST, EARTH_PROJ_COST};

const float UNDERWATER_SLOW_FACTOR = 0.5f;
const float WATER_SLOW_FACTOR = 0.4f;
const float UNDERWATER_DAMAGE_INTERVAL = 1.0f;
const float FLAG_BOOST_INTERVAL = 10.0f;
const float MOVEMENT_ABILITY_COOLDOWN = 0.1f;
const float MANA_REGEN_COOLDOWN = 1.0f;
const float START_REGEN_COOLDOWN = 10.0f;	
const int MANA_REGEN_AMOUNT = 1;


const glm::vec3 EYES_OFFSET = glm::vec3(0.0f, 1.0f, 0.0f); // Offset for the player's eyes position

const int HP_PICKUP_AMOUNT = 15;
const int MANA_PICKUP_AMOUNT = 25;

const glm::vec3 deathPoints[4] = {
		glm::vec3(-5.0f, -1000.0f, 0.0f),
		glm::vec3(0.0f, -1000.0f, 5.0f),
		glm::vec3(5.0f, -1000.0f, 0.0f),
		glm::vec3(0.0f, -1000.0f, -5.0f)
};

public:
	PlayerMovementState state = PlayerMovementState::IDLE;
    float dashTimer = 0.0f;
	float stompTimer = 0.0f;
	float magnetTimer = 0.0f;
    float grappleTimer = 0.0f;
	float deathTimer = 0.0f;
	float slowTimer = 0.0f;
	float underwaterTimer = 0.0f;
	float flagBoostTimer = 0.0f;
	float movementAbilityTimer = 0.0f;
	float manaRegenTimer = 0.0f;
	float startRegenTimer = 0.0f;
	float lowOxygenTimer = 0.0f;

	float curCooldownArray[5] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

	float curSlowFactor = 1.0f;
	float curUnderwaterSlowFactor = 1.0f;
	unsigned int maxHP = 120.0f;
	glm::vec3 deathPos;


	GameObject* grappleTarget = nullptr;
    PlayerStats playerStats;
    int debugVar = 0;
    // just forward to the base
    PlayerBehaviorComponent(GameObject* self, PhysicsSystem& physicsSystem)
        : BehaviorComponent(self, physicsSystem)
    {
    }

	pair<glm::vec3,float> handlePlayerGrapple(GameObject* obj, PhysicsSystem& phys);

	void changePlayerPower(GameObject* player, PhysicsSystem& phys, PlayerIntentPacket& intent);
	void spawnProjectile(GameObject* player, PowerType type, PhysicsSystem& phys);
	void updateParticleFlags();
	void manageCooldowns(GameObject* player, PhysicsSystem& phys, float deltaTime);

    // override the abstract methods
    void integrate(GameObject* obj, float deltaTime, PhysicsSystem& phys) override;
    // Update the base class declaration to match the derived class method signature
    void resolveCollision(GameObject* obj, GameObject* other, const pair<vec3, float>& penetration, int status) override;
};

class ProjectileBehaviorComponent : public BehaviorComponent {

public:
   glm::vec3 velocity; // Add this missing member
   float damage;
   static const float WOOD_PROJ_SPEED;
   int originalPlayer;
   float lifetime = 10.0f;

   ProjectileBehaviorComponent(GameObject* self, PhysicsSystem& physicsSystem, glm::vec3 velocity, float damage, int originalPlayer)
	   : BehaviorComponent(self, physicsSystem), velocity(velocity), damage(damage), originalPlayer(originalPlayer)
   {
	   this->self = self;
	   this->physicsSystem = physicsSystem;
	   this->velocity = velocity;
   }

   ProjectileBehaviorComponent(GameObject* self, PhysicsSystem& physicsSystem, glm::vec3 velocity, float damage, int originalPlayer, float lifetime)
	   : BehaviorComponent(self, physicsSystem), velocity(velocity), damage(damage), originalPlayer(originalPlayer), lifetime(lifetime)
   {
	   this->self = self;
	   this->physicsSystem = physicsSystem;
	   this->velocity = velocity;
   }

   void integrate(GameObject* obj, float deltaTime, PhysicsSystem& phys) override;
   void resolveCollision(GameObject* obj, GameObject* other, const pair<vec3, float>& penetration, int status) override;
};

class MetalProjectileBehaviorComponent : public ProjectileBehaviorComponent {
public:
	MetalProjectileBehaviorComponent(GameObject* self, PhysicsSystem& physicsSystem, glm::vec3 velocity, float damage, int originalPlayer)
		: ProjectileBehaviorComponent(self, physicsSystem, velocity, damage, originalPlayer)
	{
		this->self = self;
		this->physicsSystem = physicsSystem;
	}
	void integrate(GameObject* obj, float deltaTime, PhysicsSystem& phys) override;
	void resolveCollision(GameObject* obj, GameObject* other, const pair<vec3, float>& penetration, int status) override;
};

class FlagBehaviorComponent : public BehaviorComponent {

	const float TAG_TRANSFER_TIME = 3.0f;
public:
	int owningPlayer = -1;
	float tagTransferTimer = 3.0f;
	bool inCooldown = false;
	GameObject* owningGameObject = nullptr;

	FlagBehaviorComponent(GameObject* self, PhysicsSystem& physicsSystem)
		: BehaviorComponent(self, physicsSystem)
	{
		this->self = self;
		this->physicsSystem = physicsSystem;
	}

	void integrate(GameObject* obj, float deltaTime, PhysicsSystem& phys) override;
	void resolveCollision(GameObject* obj, GameObject* other, const pair<vec3, float>& penetration, int status) override;
};