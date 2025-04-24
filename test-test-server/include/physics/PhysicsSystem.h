#include <vector>
#include "PhysicsData.h"
#include "NetworkData.h"    

class PhysicsSystem {
public:
    GameObject* players[4];
    std::vector<GameObject*> dynamicObjects;
    std::vector<GameObject*> staticObjects;

    void tick(float dt);
    void applyInput(const PlayerIntentPacket& intent, int player);
    void integrate(GameObject* obj, float dt);
    void checkCollisions(GameObject* obj);
    void resolveCollisions(GameObject* obj);
    void handleGrapple(GameObject* obj, float dt);
    glm::mat4 toMatrix(const glm::vec3& position, const glm::quat& quat);
    void fromMatrix(const glm::mat4& mat, glm::vec3& outPosition, glm::vec3& outEulerRadians);
    GameObject* makeGameObject();


	void addDynamicObject(GameObject* obj) {
		dynamicObjects.push_back(obj);
	}
	void addStaticObject(GameObject* obj) {
		staticObjects.push_back(obj);
	}
};