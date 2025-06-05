#pragma once

#include <vector>
#include <map>
#include "physics/PhysicsData.h"
#include "../include/shared/NetworkData.h"
#include "InputManager.h"

typedef glm::vec3 vec3;
typedef glm::mat4 mat4;
typedef glm::quat quat;

const float STARTING_WATER_LEVEL = -2.0f;
const float ENDING_WATER_LEVEL = 5.0f;

using namespace std;

class PhysicsSystem {

private: 
    //deleting objects
    void deleteDynamicObject(GameObject* obj) {
        //remove from moving
        auto it = std::find(movingObjects.begin(), movingObjects.end(), obj);
        if (it != movingObjects.end() && obj->type != PLAYER) {
            movingObjects.erase(it);
        }
        //remove from dynamic
        it = std::find(dynamicObjects.begin(), dynamicObjects.end(), obj);
        if (it != dynamicObjects.end()) {
            dynamicObjects.erase(it);
        }
        delete obj;
    }

public:

    // create a 3d grid for the world: each cell has coordinates (i,j,k) and is mapped to a list of GameObjects that live in that cell
    map<vec3, vector<GameObject*>> worldGrid;
    vector<float> AABBdistances;
    GamePhase currPhase;
    float cellSize;

	//water level
	float waterLevel = -2.0f;

    //atmosphere level
	float atmosphereLevel = 200.0f;

    //times for water level calculation 
	float timePassed = 0.0f;
	float totalTime = 0.0f;

    //container stuff
    std::vector<GameObject*> movingObjects;
	std::vector<GameObject*> playerObjects;
    std::vector<GameObject*> dynamicObjects;
    std::vector<GameObject*> staticObjects;

    //player intent
	PlayerIntentPacket PlayerIntents[4];
	PlayerIntentTracking PlayerTrackings[4];

    //killfeed
    std::vector<KillfeedItem> killfeed_queue;

    /**
     * @brief Updates the physics system by advancing the simulation for all dynamic objects.
     * 
     * This function performs the following steps:
     * 1. Updates the acceleration of all dynamic objects by applying gravity.
     * 2. Integrates the motion of each object over the given time step.
     * 3. Handles collisions for each object after integration.
     * 4. Resets the acceleration of each object to zero after collision handling.
     * 5. Deletes all dynamic objects that are marked for deletion.
     * 
     * @param dt The time step (in seconds) to advance the simulation.
     */
    void tick(float dt);

    //takes care of rotation
    /**
     * @brief Processes player input and applies it to the corresponding game object.
     * 
     * This function takes a player's input packet and updates the position and rotation
     * of the game object associated with the given player ID. It calculates the forward 
     * and right directions based on the player's azimuth intent and applies the rotation 
     * to the game object's transform.
     * 
     * @param intent The PlayerIntentPacket containing the player's input data.
     * @param playerId The ID of the player whose input is being processed.
     * 
     * @note If no game object is found for the given player ID, the function returns without making changes.
     */
    void applyInput(const PlayerIntentPacket& intent, int playerId);
    
    /**
     * @brief Integrates the physics properties of a GameObject over a time step.
     *
     * This function updates the velocity and position of a GameObject based on its
     * current acceleration, drag, and maximum speed. It ensures that the velocity
     * does not exceed the specified maximum speed and applies drag to simulate
     * resistance.
     *
     * @param obj Pointer to the GameObject to be integrated. Must not be NULL and
     *            must have valid physics and transform components.
     * @param dt  The time step over which to integrate, in seconds.
     *
     * @note The function assumes that the GameObject's physics and transform
     *       components are properly initialized.
     * @throws Assertion failure if `obj`, `obj->physics`, or `obj->transform` is NULL.
     */
	void defaultIntegrate(GameObject* obj, float dt);

    /**
     * @brief Integrates the physics state of a GameObject over a time step.
     *
     * This function updates the state of the given GameObject by applying its
     * behavior's integration logic if a behavior is defined. If no behavior is
     * associated with the GameObject, a default integration method is used.
     *
     * @param obj Pointer to the GameObject to be integrated.
     * @param dt The time step over which to integrate, in seconds.
     */
    void integrate(GameObject* obj, float dt);

    /**
     * @brief Handles collisions for a given game object with static objects in the physics system.
     * 
     * This function checks for collisions between the specified game object and all static objects
     * in the physics system. If a collision is detected, it calculates the penetration depth and 
     * resolves the collision using the appropriate behavior or default resolution method.
     * 
     * @param obj Pointer to the game object for which collisions are to be handled. 
     *            Must not be null.
     * 
     * @note The function assumes that the `obj` parameter is valid and not null. 
     *       An assertion is used to enforce this.
     */
    void handleCollisions(GameObject* obj);

    /**
     * @brief Resolves the collision between two game objects by adjusting their velocities and positions.
     * 
     * This function handles both velocity resolution (to simulate bouncing) and positional correction 
     * (to prevent objects from overlapping). It supports collisions between dynamic and static objects.
     * 
     * @param go1 Pointer to the first game object involved in the collision. Must not be null.
     * @param go2 Pointer to the second game object involved in the collision. Must not be null.
     * @param penetration A pair containing the collision normal (as a vec3) and the penetration depth (as a float).
     *                    The normal vector should be normalized, and the penetration depth represents the overlap distance.
     * @param status An integer indicating the type of collision:
     *               - 0: The second game object (go2) is static.
     *               - Any other value: Both objects are dynamic.
     * 
     * @note The function assumes that the physics and transform components of the game objects are properly initialized.
     *       It also assumes that the collision normal provided in the penetration parameter is valid.
     * 
     * @throws AssertionError if either go1 or go2 is null.
     */
    void resolveCollision(GameObject* go1, GameObject* go2, const pair<vec3, float>& penetration, int status);
   
    //matrix conversion for writing to game state
    /**
     * @brief Converts a position vector and a quaternion rotation into a transformation matrix.
     *
     * This function creates a 4x4 transformation matrix by combining a translation matrix
     * derived from the given position vector and a rotation matrix derived from the given quaternion.
     *
     * @param position The position vector (glm::vec3) representing the translation component.
     * @param quaternion The quaternion (glm::quat) representing the rotation component.
     * @return A 4x4 transformation matrix (glm::mat4) that combines the translation and rotation.
     */
    mat4 toMatrix(const vec3& position, const quat& quat);

    /**
     * @brief Decomposes a transformation matrix into position and rotation components.
     * 
     * This function extracts the translation (position) and rotation (in Euler angles, radians)
     * from a given 4x4 transformation matrix. The scale, skew, and perspective components
     * of the matrix are ignored.
     * 
     * @param mat The input 4x4 transformation matrix to decompose.
     * @param outPosition A reference to a glm::vec3 where the extracted position (translation) will be stored.
     * @param outEulerRadians A reference to a glm::vec3 where the extracted rotation (in Euler angles, radians) will be stored.
     */
    void fromMatrix(const mat4& mat, vec3& outPosition, vec3& outEulerRadians);
    
    //game object creation
    /**
     * @brief Creates a new GameObject instance with default properties.
     * 
     * This function initializes a new GameObject with the following default settings:
     * - A unique ID assigned using the `getNextId()` function.
     * - Default transform properties:
     *   - Position set to (0.0f, 0.0f, 0.0f).
     *   - Rotation set to the identity quaternion (1.0f, 0.0f, 0.0f, 0.0f).
     *   - Scale set to (1.0f, 1.0f, 1.0f).
     * - A new PhysicsComponent instance for handling physics-related behavior.
     * - A new ColliderComponent instance for collision detection, with default
     *   half extents set to (1.0f, 1.0f, 1.0f).
     * 
     * @return A pointer to the newly created GameObject instance.
     */
    GameObject* makeGameObject();

    /**
     * @brief Creates a new GameObject with the specified position, rotation, and collider half-extents.
     * 
     * This function initializes a GameObject with the given transform properties and collider dimensions.
     * 
     * @param position The position of the GameObject in world space (glm::vec3).
     * @param rotation The rotation of the GameObject in world space (glm::quat).
     * @param halfExtents The half-extents of the GameObject's collider (glm::vec3).
     * @return A pointer to the newly created GameObject.
     */
    GameObject* makeGameObject(glm::vec3 position, glm::quat rotation, glm::vec3 halfExtents);
	
    //AABB stuff 
    /**
     * @brief Computes the Axis-Aligned Bounding Box (AABB) for a given GameObject.
     * 
     * This function calculates the minimum and maximum corners of the AABB
     * based on the GameObject's position and its collider's half extents.
     * 
     * @param obj Pointer to the GameObject for which the AABB is to be calculated.
     *            Must not be NULL, and the GameObject must have a valid collider
     *            and transform.
     * 
     * @return AABB structure containing the minimum and maximum corners of the
     *         bounding box.
     * 
     * @note The function uses assertions to ensure that the input GameObject,
     *       its collider, and its transform are not NULL.
     */
    AABB getAABB(GameObject* obj);
    vec3 getAABBCenter(AABB& a);
    vec3 getAABBDistanceCenters(AABB& a, AABB& b);
	pair<vec3, float> getAABBpenetration(AABB& a, AABB& b);

    /**
     * @brief Calculates the impulse vector for a collision based on the normal, 
     *        relative velocity, and restitution coefficient.
     * 
     * @param normal The collision normal vector, representing the direction of the collision.
     * @param relativeVelocity The relative velocity vector between the two colliding objects.
     * @param restitution The coefficient of restitution, representing the elasticity of the collision.
     *                     A value of 1.0 indicates a perfectly elastic collision, while 0.0 indicates
     *                     a perfectly inelastic collision.
     * 
     * @return A vec3 representing the impulse vector. If the velocity along the normal is non-negative,
     *         a zero vector is returned, indicating no impulse is applied.
     */
    vec3 getImpulseVector(const vec3& normal, const vec3& relativeVelocity, float restitution);
    
    //id
    int getNextId();
    GameObject* getPlayerObjectById(int id);

    GameObject* getClosestPlayerObject(glm::vec3 pos, int exclude);
    
    //adding objects
	void addDynamicObject(GameObject* obj) {
		dynamicObjects.push_back(obj);
	}
	void addStaticObject(GameObject* obj) {
		staticObjects.push_back(obj);
	}
	void addPlayerObject(GameObject* obj) {
		playerObjects.push_back(obj);
	}
    void addMovingObject(GameObject* obj) {
        movingObjects.push_back(obj);
    }

    void updateWaterLevel();

    void addKillfeedItem(KillfeedItem item) {
        killfeed_queue.push_back(item);
        //if length of queue exceeds maximum, pop an item
        if (killfeed_queue.size() > KILLFEED_LENGTH) {
            killfeed_queue.erase(killfeed_queue.begin());
        }
    }

    void tickKillfeed(float deltaTime) {
        for (int i = 0; i < killfeed_queue.size(); i++) {
            killfeed_queue[i].lifetime += deltaTime;
        }
    }

	//delete all objects that are marked for deletion
    void deleteMarkedDynamicObjects() {
		for (int i = movingObjects.size() - 1; i >= 0; i--) {
			GameObject* obj = movingObjects[i];
			if (obj->markDeleted) {
				deleteDynamicObject(obj);
			}
		}
    }
};