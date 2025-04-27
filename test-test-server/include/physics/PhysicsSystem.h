#include <vector>
#include <map>
#include "PhysicsData.h"
#include "shared/NetworkData.h"    

class PhysicsSystem {
public:
    std::vector<GameObject*> dynamicObjects;
    std::vector<GameObject*> staticObjects;
    // create a 3d grid for the world: each cell has coordinates (i,j,k) and is mapped to a list of GameObjects that live in that cell
    map<glm::vec3, vector<GameObject*>> worldGrid;
    vector<float> AABBdistances;
    double cellSize;

    void tick(float dt);
    void applyInput(const PlayerIntentPacket& intent, int playerId);
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