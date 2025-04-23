#include <glm/glm.hpp>
#include "ForceGenerator.h"

class GameObject;

class GravityForce : public ForceGenerator {
public:
    explicit GravityForce(const glm::vec3& gravity = glm::vec3(0.0f, -9.80665f, 0.0f));

    virtual ~GravityForce() override = default;

    virtual void applyForce(GameObject* obj, float dt) override;

private:
    glm::vec3 _gravity;
};