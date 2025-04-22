#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <memory>

class Collider;

class GameObject {
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 force;
    glm::quat orientation;
    float mass, inverseMass;
    glm::mat3 intertiaTensor;
    glm::mat3 inverseInertiaTensor;
};

void applyForce(GameObject* rb, const glm::vec3& force);
void applyTorque(GameObject* rb, const glm::vec3& torque);
void applyForceAtPoint(GameObject* rb, const glm::vec3& force, const glm::vec3& point);
void clearForces(GameObject* rb);

