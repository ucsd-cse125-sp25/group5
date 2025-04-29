#include <vector>
#include <map>
#include "PhysicsData.h"
#include "shared/NetworkData.h"    

#define XD 100
#define YD 100
#define ZD 100

typedef glm::vec3 vec3;
typedef glm::mat4 mat4;
typedef glm::quat quat;
using namespace std;

class PhysicsSystem {
public:
    vector<GameObject*> dynamicObjects;
    vector<GameObject*> staticObjects;
    // create a 3d grid for the world: each cell has coordinates (i,j,k) and is mapped to a list of GameObjects that live in that cell
    map<vec3, vector<GameObject*>> worldGrid;
    vector<float> AABBdistances;
    float cellSize;

    void tick(float dt);
    void applyInput(const PlayerIntentPacket& intent, int playerId);
    void integrate(GameObject* obj, float dt);
    void checkCollisions(GameObject* obj);
    void resolveCollisions(GameObject* obj);
    void handleGrapple(GameObject* obj, float dt);
    mat4 toMatrix(const vec3& position, const quat& quat);
    void fromMatrix(const mat4& mat, vec3& outPosition, vec3& outEulerRadians);
    GameObject* makeGameObject();

    void getAABBsDistance(std::vector<GameObject*> gobjs);
    float getCellSize();
    void populateGrid();
    


	void addDynamicObject(GameObject* obj) {
		dynamicObjects.push_back(obj);
	}
	void addStaticObject(GameObject* obj) {
		staticObjects.push_back(obj);
	}
};