#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Collider.h"

class GameObject {
public:
    GameObject(float mass, Collider* collider);
    ~GameObject();

    //unsigned int getID() const { return id_; }

    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 force;
    
    glm::quat orientation;
    glm::vec3 angularVelocity;
    glm::vec3 torque;

    float mass, inverseMass;
    glm::mat3 intertiaTensor, inverseInertiaTensor;
};

void applyForce(GameObject* rb, const glm::vec3& force);
void applyTorque(GameObject* rb, const glm::vec3& torque);
void applyForceAtPoint(GameObject* rb, const glm::vec3& force, const glm::vec3& point);
void integrateVelocities(GameObject* rb, float dt);
void integratePositions(GameObject* rb, float dt);
void clearForces(GameObject* rb);

void isStatic(GameObject* rb);
void isDynamic(GameObject* rb);


