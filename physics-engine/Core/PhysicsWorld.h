#pragma once

#include <vector>
#include <memory>
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

    void addGameObject(GameObject* rb);
    void removeGameObject(GameObject* rb);

    void addCollider(Collider* collider);
    void removeCollider(Collider* collider);

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
    std::vector<ForceGenerator*> forceGenerators;

    CollisionDetection* collisionDetection;
    CollisionResolver* collisionResolver;

    glm::vec3 gravity = glm::vec3(0.0f, -9.80665f, 0.0f);
};