#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <memory>

class Collider;

struct RigidBody {
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 force;
};