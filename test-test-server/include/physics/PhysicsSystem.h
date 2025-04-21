#include <vector>
#include "PhysicsData.h"
#include "NetworkData.h"    

class PhysicsSystem {
public:
    std::vector<GameObject*> dynamicObjects;
    std::vector<GameObject*> staticObjects;

    void tick(float dt);
    void applyInput(GameObject* obj, const PlayerIntentPacket& intent);
    void integrate(GameObject* obj, float dt);
    void checkCollisions(GameObject* obj);
    void resolveCollisions(GameObject* obj);
    void handleGrapple(GameObject* obj, float dt);



	void addDynamicObject(GameObject* obj) {
		dynamicObjects.push_back(obj);
	}
	void addStaticObject(GameObject* obj) {
		staticObjects.push_back(obj);
	}
};