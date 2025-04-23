#pragma once

#include <glm/glm.hpp>

// Forward declaration
class GameObject;

enum class ColliderType {
    Sphere, Box, Capsule, ConvexHull
};

class Collider {
public:
    enum Type {
        SPHERE,
        BOX,
    };
    Collider(Type type);
    virtual ~Collider() = default;
    
private:
    GameObject* rb;
    
virtual void computeAABB() = 0;
    
};

