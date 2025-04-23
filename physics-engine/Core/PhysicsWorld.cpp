#pragma once

#include "PhysicsWorld.h"
#include "GameObject.h"
#include "Collider.h"
#include "ForceGenerator.h"
#include "CollisionDetection.h"
#include "CollisionResolver.h"
#include "Integrator.h"

PhysicsWorld::PhysicsWorld() : 
    collisionDetection(nullptr), 
    collisionResolver(nullptr),
    gravity(0.0f, -9.81f, 0.0f) 
{}

PhysicsWorld::~PhysicsWorld() {
    for (GameObject* obj : dynamicObjects) {
        delete obj;
    }
    for (GameObject* obj : staticObjects) {
        delete obj;
    }
    for (Collider* collider : colliders) {
        delete collider;
    }
    for (ForceGenerator* forceGen : forceGenerators) {
        delete forceGen;
    }
}

void PhysicsWorld::tick(float dt) {
    clearForces();
    applyForces(dt);
    integrateVelocities(dt);
    detectCollisions();
    resolveCollisions();
    integratePositions(dt);
}

void PhysicsWorld::addGameObject(GameObject* rb, bool isStatic) {
    if (isStatic) {
        staticObjects.push_back(rb);
    } else {
        dynamicObjects.push_back(rb);
    }
}

void PhysicsWorld::removeGameObject(GameObject* rb) {
    auto it = std::find(dynamicObjects.begin(), dynamicObjects.end(), rb);
    if (it != dynamicObjects.end()) {
        dynamicObjects.erase(it);
    } else {
        it = std::find(staticObjects.begin(), staticObjects.end(), rb);
        if (it != staticObjects.end()) {
            staticObjects.erase(it);
        }
    }

    for (auto& pair : ownersOfCollidersMap) {
        if (pair.second == rb) {
            ownersOfCollidersMap.erase(pair.first);
        }
    }
}

void PhysicsWorld::addCollider(Collider* collider, GameObject* owner) {
    colliders.push_back(collider);
    ownersOfCollidersMap[collider] = owner;
}

void PhysicsWorld::removeCollider(Collider* collider, GameObject* owner) {
    colliders.erase(std::remove(colliders.begin(), colliders.end(), collider), colliders.end());
    ownersOfCollidersMap.erase(collider);
}