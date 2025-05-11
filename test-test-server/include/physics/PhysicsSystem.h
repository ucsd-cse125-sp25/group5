#pragma once

#include <vector>
#include <map>
#include "physics/PhysicsData.h"
#include "../include/shared/NetworkData.h"  

#define XD 100
#define YD 100
#define ZD 100

typedef glm::vec3 vec3;
typedef glm::mat4 mat4;
typedef glm::quat quat;
using namespace std;

class PhysicsSystem {

private: 
    //deleting objects
    void deleteDynamicObject(GameObject* obj) {
        //remove from moving
        auto it = std::find(movingObjects.begin(), movingObjects.end(), obj);
        if (it != movingObjects.end()) {
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
    float cellSize;

    //container stuff
    std::vector<GameObject*> movingObjects;
	std::vector<GameObject*> playerObjects;
    std::vector<GameObject*> dynamicObjects;
    std::vector<GameObject*> staticObjects;

    //player intent
	PlayerIntentPacket PlayerIntents[4];

    //game tick
    void tick(float dt);

    //takes care of rotation
    void applyInput(const PlayerIntentPacket& intent, int playerId);
    
    //moving objects 
    void integrate(GameObject* obj, float dt);
	void defaultIntegrate(GameObject* obj, float dt);
  
    //handle and resolve collisions 
    void handleCollisions(GameObject* obj);
    void resolveCollision(GameObject* go1, GameObject* go2, const pair<vec3, float>& penetration, int status);
    
    //deprecated 
    void handleGrapple(GameObject* obj, float dt);
   
    //matrix conversion for writing to game state
    mat4 toMatrix(const vec3& position, const quat& quat);
    void fromMatrix(const mat4& mat, vec3& outPosition, vec3& outEulerRadians);
    
    //game object creation
    GameObject* makeGameObject();
    GameObject* makeGameObject(glm::vec3 position, glm::quat rotation, glm::vec3 halfExtents);
	
    //AABB stuff 
    AABB getAABB(GameObject* obj);
	pair<vec3, float> getAABBpenetration(AABB& a, AABB& b);
    vec3 getImpulseVector(const vec3& normal, const vec3& relativeVelocity, float restitution);
    
    //id
    int getNextId();
    GameObject* getPlayerObjectById(int id);
    
    std::pair<float, float> getInterval(const vec3& center, const vec3& halfExtents, const vector<vec3>& normals, const vec3& axis);

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
	void addDynamicMovingObject(GameObject* obj) {
		dynamicObjects.push_back(obj);
		movingObjects.push_back(obj);
	}


	//delete all objects that are marked for deletion
    void deleteMarkedDynamicObjects() {
		for (size_t i = movingObjects.size() - 1; i >= 0; --i) {
			GameObject* obj = movingObjects[i];
			if (obj->markDeleted) {
				deleteDynamicObject(obj);
			}
		}
    }
};