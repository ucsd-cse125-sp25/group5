#pragma once

#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>

// Forward declarations
class GameObject;
class Collider;
class ForceGenerator;
class CollisionDetection;
class CollisionResolver;
struct CollisionManifold;

class PhysicsWorld {
public:
    PhysicsWorld();
    ~PhysicsWorld();
    
    void tick(float dt);

    void addGameObject(GameObject* rb, bool isStatic = false);
    void removeGameObject(GameObject* rb);

    void addCollider(Collider* collider, GameObject* owner);
    void removeCollider(Collider* collider, GameObject* owner);

    void addForceGenerator(ForceGenerator* forceGenerator);
    void removeForceGenerator(ForceGenerator* forceGenerator);
    
    void setCollisionDetection(CollisionDetection* dedector);
    void setCollisionResolver(CollisionResolver* resolver);

    void setGravity(const glm::vec3& gravity);
    const glm::vec3& getGravity() const;

private:
    void clearForces();
    void applyForces(float dt);
    void integrateVelocities(float dt);
    void integratePositions(float dt);
    void detectCollisions();
    void resolveCollisions();

    std::vector<GameObject*> dynamicObjects;
    std::vector<GameObject*> staticObjects;
    std::vector<Collider*> colliders;
    std::unordered_map<Collider*, GameObject*> ownersOfCollidersMap;
    
    std::vector<ForceGenerator*> forceGenerators;
    std::vector<CollisionManifold> collisionManifolds;

    CollisionDetection* collisionDetection = nullptr;
    CollisionResolver* collisionResolver = nullptr;

    glm::vec3 gravity = glm::vec3(0.0f, -9.81f, 0.0f);
};