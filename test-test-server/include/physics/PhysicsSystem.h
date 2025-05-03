#include <vector>
#include <map>
#include "physics/PhysicsData.h"
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

    // create a 3d grid for the world: each cell has coordinates (i,j,k) and is mapped to a list of GameObjects that live in that cell
    map<vec3, vector<GameObject*>> worldGrid;
    vector<float> AABBdistances;
    float cellSize;

	std::vector<GameObject*> playerObjects;
    std::vector<GameObject*> dynamicObjects;
    std::vector<GameObject*> staticObjects;

    void tick(float dt);
    void applyInput(const PlayerIntentPacket& intent, int playerId);
    void integrate(GameObject* obj, float dt);
    void handleCollisions(GameObject* obj);
    void resolveCollisionDySt(GameObject* go1, const pair<vec3, float>& penetration);
    void resolveCollisionDyDy(GameObject* go1, GameObject* go2, const pair<vec3, float>& penetration);
    void handleGrapple(GameObject* obj, float dt);
    mat4 toMatrix(const vec3& position, const quat& quat);
    void fromMatrix(const mat4& mat, vec3& outPosition, vec3& outEulerRadians);
    GameObject* makeGameObject();
    GameObject* makeGameObject(glm::vec3 position, glm::quat rotation, glm::vec3 halfExtents);
	AABB getAABB(GameObject* obj);
	pair<vec3, float> getAABBpenetration(AABB& a, AABB& b);
    vec3 getImpulseVector(const vec3& normal, const vec3& relativeVelocity, float restitution);

    std::pair<float, float> getInterval(const vec3& center, const vec3& halfExtents, const vector<vec3>& normals, const vec3& axis);
    

    // PhysicsDraftFile.cpp
	pair<vec3, float> SATOverlapTestExperimental(AABB a, AABB b);
    void getAABBsDistance(std::vector<GameObject*> gobjs);
    float getCellSize();
    void populateGrid();
    float getBoxDim(GameObject* go);
    std::pair<float, float> projetBox(GameObject *go, glm::vec3 axis, glm::mat3 rotationMat);
    std::vector<vec3> getCrossProducts(const std::vector<vec3>& normals1, const std::vector<vec3>& normals2);
    void SAT(GameObject* go1, GameObject* go2);
    void checkCollisions(GameObject* obj);
    void resolveCollision(GameObject* go1, const pair<vec3, float>& penetration);


	void addDynamicObject(GameObject* obj) {
		dynamicObjects.push_back(obj);
	}
	void addStaticObject(GameObject* obj) {
		staticObjects.push_back(obj);
	}
	void addPlayerObject(GameObject* obj) {
		playerObjects.push_back(obj);
	}
};