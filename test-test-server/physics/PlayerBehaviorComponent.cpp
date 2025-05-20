// PlayerBehaviorComponent.cpp
#define NOMINMAX
#include "physics/BehaviorComponent.h"   // brings in both BehaviorComponent & PlayerBehaviorComponent
#include "physics/PhysicsSystem.h"      // for any phys.integrate calls
#include "physics/PhysicsData.h"        // for GameObject
#include "ServerGame.h"
#include <limits>


glm::vec3 getInputDirection(const PlayerIntentPacket& intent, GameObject* obj) {
	//process player input
	GameObject* target = obj;

	float azimuth = glm::radians(-intent.azimuthIntent);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), azimuth, up);
	glm::vec3 forward = glm::normalize(glm::vec3(rotation * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)));
	//glm::vec3 translation = glm::vec3(target.cubeModel[3]);
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

glm::vec3 static getDirection(float azimuth, float incline) {
	// if azimuth & incline are in degrees:
	// azimuth = glm::radians(azimuth);
	// incline = glm::radians(incline);

	float cosInc = cos(incline);

	glm::vec3 D;
	D.x = cosInc * sin(azimuth) * -1;  // right/left
	D.y = sin(incline);           // up/down
	D.z = cosInc * cos(azimuth) * -1;  // forward/back
 // invert direction

	return glm::normalize(D);
}

//god forgive me for what I'm about to do
bool static checkBottom(GameObject* obj, PhysicsSystem& phys) {
	// 1) compute a single point just below the player's feet
	float eps = 0.1f;
	glm::vec3 foot = obj->transform.position
		- glm::vec3(0.f, obj->collider->halfExtents.y + eps, 0.f);

	// 2) test that point against every static AABB
	for (auto* s : phys.staticObjects) {
		const AABB& b = phys.getAABB(s);
		if (foot.x >= b.min.x && foot.x <= b.max.x
			&& foot.y >= b.min.y && foot.y <= b.max.y
			&& foot.z >= b.min.z && foot.z <= b.max.z)
		{
			return true;
		}
	}

	return false;
}

bool rayIntersectsAABB(const Ray& ray,
	const AABB& box,
	float& tHit)
{
	float tMin = (box.min.x - ray.origin.x) / ray.dir.x;
	float tMax = (box.max.x - ray.origin.x) / ray.dir.x;
	if (tMin > tMax) std::swap(tMin, tMax);

	for (int i = 1; i < 3; ++i) {
		float originComp = (i == 1 ? ray.origin.y : ray.origin.z);
		float dirComp = (i == 1 ? ray.dir.y : ray.dir.z);
		float bMin = (i == 1 ? box.min.y : box.min.z);
		float bMax = (i == 1 ? box.max.y : box.max.z);

		float t1 = (bMin - originComp) / dirComp;
		float t2 = (bMax - originComp) / dirComp;
		if (t1 > t2) std::swap(t1, t2);

		tMin = std::max(tMin, t1);
		tMax = std::min(tMax, t2);
		if (tMin > tMax) return false;
	}

	// at this point [tMin, tMax] is the overlap interval.
	// we want the first positive hit:
	if (tMax < 0) return false;      // box is behind ray
	tHit = tMin >= 0 ? tMin : tMax;  // if origin inside box, tMin<0 so use tMax
	return true;
}

pair<glm::vec3, float> PlayerBehaviorComponent::handlePlayerGrapple(GameObject* obj, PhysicsSystem& phys) {

	Ray ray;

	ray.origin = obj->transform.position;
	ray.dir = getDirection(
		glm::radians(-phys.PlayerIntents[obj->id].azimuthIntent),
		glm::radians(-phys.PlayerIntents[obj->id].inclineIntent)
	);

	float bestT = std::numeric_limits<float>::infinity();
	GameObject* bestHitObj = nullptr;

	for (auto* staticObj : phys.staticObjects) {
		AABB box = phys.getAABB(staticObj);
		float t;
		if (rayIntersectsAABB(ray, box, t) && t < bestT) {
			bestT = t;
			bestHitObj = staticObj;
		}
	}

	if (bestHitObj) {
		glm::vec3 hitPoint = ray.origin + ray.dir * bestT;
		grappleTarget = bestHitObj;
		return { hitPoint, bestT };
	}
	return { glm::vec3(0.0f, 0.0f, 0.0f), -1.0f };
}

glm::quat getRotationFromAzimuthIncline(float azimuth, float incline) {
	glm::quat yaw = glm::angleAxis(azimuth, glm::vec3(0.0f, 1.0f, 0.0f)); // yaw (Y-axis)
	glm::quat pitch = glm::angleAxis(incline, glm::vec3(1.0f, 0.0f, 0.0f)); // pitch (X-axis)

	// Order matters: yaw * pitch rotates first by pitch, then yaw
	return yaw * pitch;
}

void PlayerBehaviorComponent::spawnProjectile(GameObject* player, PowerType type, PhysicsSystem& phys) {

	//get the direction that the player is facing, that will be our projectile direction
	glm::vec3 facingDirection = getDirection(
		glm::radians(-phys.PlayerIntents[player->id].azimuthIntent),
		glm::radians(-phys.PlayerIntents[player->id].inclineIntent)
	);
	//get the proper rotation of the projectile
	glm::quat rotation = getRotationFromAzimuthIncline(
		glm::radians(-phys.PlayerIntents[player->id].azimuthIntent),
		glm::radians(-phys.PlayerIntents[player->id].inclineIntent)
	);

	if (type == WOOD && playerStats.mana[1] >= WOOD_PROJ_COST) {
		//create a new projectile, start it off at the position of the player, at the proper rotation, and give it the size of the wood projectile 
		GameObject* obj = phys.makeGameObject(player->transform.position, rotation, woodProjExtents);

		//give it the behavior of a projectile object, and make it good type
		obj->behavior = new ProjectileBehaviorComponent(obj, phys, facingDirection * woodProjSpeed, 10.0f, player->id);
		obj->type = WOOD_PROJ;
		obj->isDynamic = true;

		//add it to both dynamic and moving (because the way our physics is structured is kind of cursed)
		phys.addDynamicObject(obj);
		phys.addMovingObject(obj);
		//print velocity
		printf("Projectile velocity %f %f %f\n", obj->physics->velocity.x, obj->physics->velocity.y, obj->physics->velocity.z);

		//reduce mana
		playerStats.mana[1] -= WOOD_PROJ_COST;
	}
	else if (type == METAL && playerStats.mana[0] >= METAL_PROJ_COST) {
		//create a new projectile, start it off at the position of the player, at the proper rotation, and give it the size of the wood projectile 
		GameObject* obj = phys.makeGameObject(player->transform.position, rotation, woodProjExtents);
		//give it the behavior of a projectile object, and make it good type
		obj->behavior = new ProjectileBehaviorComponent(obj, phys, facingDirection * woodProjSpeed, 10.0f, player->id);
		obj->type = METAL_PROJ;
		obj->isDynamic = true;
		//add it to both dynamic and moving (because the way our physics is structured is kind of cursed)
		phys.addDynamicObject(obj);
		phys.addMovingObject(obj);

		playerStats.mana[0] -= METAL_PROJ_COST;
	}
	else if (type == WATER && playerStats.mana[2] >= WATER_PROJ_COST) {
		//create a new projectile, start it off at the position of the player, at the proper rotation, and give it the size of the wood projectile 
		GameObject* obj = phys.makeGameObject(player->transform.position, rotation, woodProjExtents);
		//give it the behavior of a projectile object, and make it good type
		obj->behavior = new ProjectileBehaviorComponent(obj, phys, facingDirection * woodProjSpeed, 10.0f, player->id);
		obj->type = WATER_PROJ;
		obj->isDynamic = true;
		//add it to both dynamic and moving (because the way our physics is structured is kind of cursed)
		phys.addDynamicObject(obj);
		phys.addMovingObject(obj);

		playerStats.mana[2] -= WATER_PROJ_COST;
	}
	else if (type == FIRE && playerStats.mana[3] >= FIRE_PROJ_COST) {
		//create a new projectile, start it off at the position of the player, at the proper rotation, and give it the size of the wood projectile 
		GameObject* obj = phys.makeGameObject(player->transform.position, rotation, fireProjExtents);
		//give it the behavior of a projectile object, and make it good type
		obj->behavior = new ProjectileBehaviorComponent(obj, phys, facingDirection * fireProjSpeed, 10.0f, player->id, 1.0f);
		obj->type = FIRE_PROJ;
		obj->isDynamic = true;
		//add it to both dynamic and moving (because the way our physics is structured is kind of cursed)
		phys.addDynamicObject(obj);
		phys.addMovingObject(obj);

		playerStats.mana[3] -= FIRE_PROJ_COST;
	}
	else if (type == EARTH && playerStats.mana[4] >= EARTH_MOVE_COST) {
		//create a new projectile, start it off at the position of the player, at the proper rotation, and give it the size of the wood projectile 
		GameObject* obj = phys.makeGameObject(player->transform.position, rotation, woodProjExtents);
		//give it the behavior of a projectile object, and make it good type
		obj->behavior = new ProjectileBehaviorComponent(obj, phys, facingDirection * woodProjSpeed, 10.0f, player->id);
		obj->type = EARTH_PROJ;
		obj->isDynamic = true;
		//add it to both dynamic and moving (because the way our physics is structured is kind of cursed)
		phys.addDynamicObject(obj);
		phys.addMovingObject(obj);

		playerStats.mana[4] -= EARTH_PROJ_COST;
	}
	
}

void PlayerBehaviorComponent::changePlayerPower(GameObject* player, PhysicsSystem& phys, PlayerIntentPacket& intent) {
	playerStats.activePower = PowerType(phys.PlayerIntents[player->id].changeToPower);
}

//—— integrate — called once per tick
void PlayerBehaviorComponent::integrate(GameObject* obj,
    float deltaTime,
    PhysicsSystem& phys)
{	



	PlayerIntentPacket& intent = physicsSystem.PlayerIntents[obj->id];

	//when death first happens 
	if (playerStats.hp <= 0 && state != PlayerMovementState::DEATH) {
		//set state to death, start the death timer, do tags
		state = PlayerMovementState::DEATH;
		deathTimer = DEATH_TIME;
		playerStats.alive = false;

		//drop the flag 
		if (obj->attached != nullptr && obj->attached->type == FLAG) {
			FlagBehaviorComponent* behavior = dynamic_cast<FlagBehaviorComponent*>(obj->attached->behavior);
			behavior->owningPlayer = -1;
			obj->attached = nullptr;
			playerStats.hasFlag = false;
		}

		//no collider
		obj->collider->halfExtents = glm::vec3(0.0f, 0.0f, 0.0f);
	}
	
	changePlayerPower(obj, phys, intent);
	//dash movement
	if(state == PlayerMovementState::DEATH) {
		deathTimer -= deltaTime;

		//freeze the player
		obj->physics->velocity = glm::vec3(0.0f, 0.0f, 0.0f);

		playerStats.inAir = false;

		if (deathTimer <= 0.0f) {
			
			playerStats.hp = 100.0f;
			playerStats.alive = true;
			state = PlayerMovementState::IDLE;
			deathTimer = 0.0f;

			//half the amount of mana
			for (int i = 0; i < 5; i++) {
				playerStats.mana[i] /= 2.0f;
			}

			//turn collider back on
			//TODO: set extents to player defaults
			obj->collider->halfExtents = glm::vec3(1.0f, 1.0f, 1.0f);
		}
		return;
	}
	else if (state == PlayerMovementState::DASH) {
		dashTimer -= deltaTime;

		//once we're done, exit dash
		if (dashTimer <= 0.0f) {
			state = PlayerMovementState::IDLE;
			dashTimer = 0.0f;
			//slow down our player 
			obj->physics->velocity *= 0.1f;
		}
	}
	else if (state == PlayerMovementState::STOMP) {
		//straight DOWN
		
		stompTimer -= deltaTime;

		//once we're done, exit stomp
		if (stompTimer <= 0.0f || checkBottom(obj, phys)) {
			state = PlayerMovementState::IDLE;
			stompTimer = 0.0f;
		}
	}
	else if (state == PlayerMovementState::GRAPPLE) {

		grappleTimer -= deltaTime;
		//see if we've collided, this whole thing could be optimized if we use the time as well 
		pair<vec3, float> penetration = phys.getAABBpenetration(phys.getAABB(obj), phys.getAABB(grappleTarget));

		//if we've collided with our target object, or if we've run out of time, release the grapple
		if (grappleTimer <= 0.0f) {
			printf(
				"grapple timer expired or collided with target\n");

			printf("grapple timer %f\n", grappleTimer);
			printf("penetration %f\n", penetration.second);
			obj->physics->velocity *= 0.1f;
			state = PlayerMovementState::IDLE;
			grappleTimer = 0.0f;
			grappleTarget = nullptr;
		}
	}
	

	//regular movement
	if (state == PlayerMovementState::IDLE) {
		//check for if a player is in the air
		playerStats.inAir = !checkBottom(obj, phys);
		//check for movement powers 
		if (intent.rightClickIntent && phys.PlayerTrackings[obj->id].rightClickDuration == 1) {

			printf("Metal mana %d\n", playerStats.mana[0]);
			printf("Wood mana %d\n", playerStats.mana[1]);
			printf("Water mana %d\n", playerStats.mana[2]);
			printf("Fire mana %d\n", playerStats.mana[3]);
			printf("Earth mana %d\n", playerStats.mana[4]);
			if (playerStats.activePower == FIRE && playerStats.mana[3] >= FIRE_MOVE_COST) {
				state = PlayerMovementState::DASH;
				dashTimer = DASH_TIME;

				//fix the velocity to the direction we wish to dash in
				obj->physics->velocity = getDirection(
					glm::radians(-intent.azimuthIntent),
					glm::radians(-intent.inclineIntent)
				) * DASH_SPEED;

				playerStats.mana[3] -= FIRE_MOVE_COST;

				return;
			}
			else if (playerStats.activePower == EARTH && playerStats.mana[4] >= EARTH_MOVE_COST) {
				state = PlayerMovementState::STOMP;
				stompTimer = STOMP_TIME;

				//fix the velocity
				obj->physics->velocity = glm::vec3(0.0f, -STOMP_SPEED, 0.0f);

				playerStats.mana[4] -= EARTH_MOVE_COST;

				return;
			}
			else if (playerStats.activePower == WATER && playerStats.mana[2] >= WATER_MOVE_COST) {
				//high jump
				obj->physics->velocity += glm::vec3(0.0f, JUMP_FORCE * 2.0f, 0.0f);

				playerStats.mana[2] -= WATER_MOVE_COST;
			}
			else if (playerStats.activePower == WOOD && playerStats.mana[1] >= EARTH_MOVE_COST) {
				//our target point, we've also set the target object, this could probably use some restructuring


				//result
				pair<glm::vec3, float> result = handlePlayerGrapple(obj, phys);

				glm::vec3 target = result.first;

				//we have a target
				if (target != glm::vec3(0.0f, 0.0f, 0.0f)) {
					//only start grappling if we have a target 
					state = PlayerMovementState::GRAPPLE;
					grappleTimer = result.second / GRAPPLE_SPEED;
					//get our direction
					glm::vec3 direction = target - obj->transform.position;
					glm::vec3 normalizedDirection = glm::normalize(direction);
					//lock the velocity
					obj->physics->velocity = normalizedDirection * GRAPPLE_SPEED;
				}

				playerStats.mana[1] -= WOOD_MOVE_COST;
			}

			printf("Metal mana %d\n", playerStats.mana[0]);
			printf("Wood mana %d\n", playerStats.mana[1]);
			printf("Water mana %d\n", playerStats.mana[2]);
			printf("Fire mana %d\n", playerStats.mana[3]);
			printf("Earth mana %d\n", playerStats.mana[4]);
		}
		

		//if (intent.hit1Intent && obj->attached != nullptr && obj->attached->type == FLAG) {
		//	FlagBehaviorComponent* behavior = dynamic_cast<FlagBehaviorComponent*>(obj->attached->behavior);
		//	playerStats.hasFlag = false;
		//	behavior->owningPlayer = -1;
		//	obj->attached = nullptr;
		//}

		//if (intent.hit2Intent) {
		//	//kill self
		//	playerStats.hp = 0.0f;
		//}

		//check for attacks
		//printf("rightClickDuration is %d\n", phys.PlayerTrackings[obj->id].leftClickDuration);
		if (intent.leftClickIntent) {
			if (playerStats.activePower == FIRE && phys.PlayerTrackings[obj->id].leftClickDuration >= 1) {
				spawnProjectile(obj, playerStats.activePower, phys);
			}
			else if (phys.PlayerTrackings[obj->id].leftClickDuration == 1) {
				spawnProjectile(obj, playerStats.activePower, phys);
				printf("Hit e\n");
				printf("Physics system size %d\n", int(phys.dynamicObjects.size()));
			}
			
		}





		// apply force 
		obj->physics->velocity += obj->physics->acceleration * deltaTime;

		//apply drag
		obj->physics->velocity *= (1.0f - obj->physics->drag * deltaTime);



		//clamp velocity
		if (glm::length(obj->physics->velocity) > obj->physics->maxSpeed) {
			obj->physics->velocity = glm::normalize(obj->physics->velocity) * obj->physics->maxSpeed;
		}

		//detect collision from bottom for jump, and make the player jump
		if (checkBottom(obj, phys) && physicsSystem.PlayerIntents[obj->id].moveUpIntent) {
			obj->physics->velocity += glm::vec3(0.0f, 3.0f, 0.0f);
		}

		//apply player movement
		glm::vec3 inputDirection = getInputDirection(physicsSystem.PlayerIntents[obj->id], obj);
		//set moving flag
		playerStats.moving = inputDirection != glm::vec3(0.0f, 0.0f, 0.0f);
		//apply transformation
		obj->transform.position += inputDirection * deltaTime;

		

	}




	//only apply the player velocity for movement
	//obj->physics->velocity += getInputDirection(physicsSystem.PlayerIntents[obj->id], obj);
	playerStats.hasFlag = obj->attached != nullptr && obj->attached->type == FLAG;
	//the important line
	obj->transform.position += obj->physics->velocity * deltaTime;
	//

	//remove it after
	//obj->physics->velocity -= getInputDirection(physicsSystem.PlayerIntents[obj->id], obj);
    
}

//—— resolveCollision — called when this object hits another
void PlayerBehaviorComponent::resolveCollision(GameObject* obj, GameObject* other, const pair<vec3, float>& penetration, int status)
{
	if (status == 0) {
		physicsSystem.resolveCollision(obj, other, penetration, status);
	}
	else if (status == 1) {
		//this is fucking terrible 
		printf("Detected a collision between %d and %d\n", obj->id, other->id);

		//make sure its a projectile
		if (other->type >= 5 && other->type <= 9) {
			ProjectileBehaviorComponent* pb = dynamic_cast<ProjectileBehaviorComponent*>(other->behavior);
			printf("pb %d\n", pb);
			//make sure we didn't fire it
			if (pb != nullptr && pb->originalPlayer != obj->id) {
				playerStats.hp -= pb->damage;
				printf("Player %d took %f damage from projectile %d\n", obj->id, pb->damage, other->id);
			}
		}
	}
}