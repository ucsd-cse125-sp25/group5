#pragma once

class GameObject;

class ForceGenerator {
public:
    virtual ~ForceGenerator() = default;

    // Apply the force to the given object
    virtual void applyForce(GameObject* obj, float dt) = 0;
};