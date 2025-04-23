#include <glm/glm.hpp>
#include "ForceGenerator.h"

class GameObject;

class GravityForce : public ForceGenerator {
public:
    virtual ~GravityForce() override = default;
    
    void applyForce(GameObject* obj, float dt) override;

private:
};