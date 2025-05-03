#include "physics/PhysicsSystem.h"
#include "physics/PhysicsData.h"	
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/matrix_decompose.hpp>

typedef glm::vec3 vec3;
typedef glm::vec4 vec4;
typedef glm::mat3 mat3;
typedef glm::mat4 mat4;
typedef glm::quat quat;

using namespace std;

glm::vec3 bezier(const glm::vec3& A, const glm::vec3& B, const glm::vec3& C, float t) {
    glm::vec3 AB = glm::mix(A, B, t);
    glm::vec3 BC = glm::mix(B, C, t);
    return glm::mix(AB, BC, t);
}

bool AABBOverlap(const AABB& a, const AABB& b) {
    return (
        a.min.x <= b.max.x && a.max.x >= b.min.x &&
        a.min.y <= b.max.y && a.max.y >= b.min.y &&
        a.min.z <= b.max.z && a.max.z >= b.min.z
        );
}

void PhysicsSystem::tick(float dt) {

	for (GameObject* obj : playerObjects) {
        obj->physics->acceleration += glm::vec3(0, -GRAVITY * obj->physics->gravityScale, 0);

        // Integrate
        integrate(obj, dt);

        // Collide + resolve
        checkCollisions(obj);
        //resolveCollision(obj);

        // Reset per-frame data
        obj->physics->acceleration = glm::vec3(0);

		/*printf("Velocity: %f %f %f\n", obj->physics->velocity.x, obj->physics->velocity.y, obj->physics->velocity.z);
		printf("Acceleration: %f %f %f\n", obj->physics->acceleration.x, obj->physics->acceleration.y, obj->physics->acceleration.z);*/
	}

    for (GameObject* obj : dynamicObjects) {

        // Apply gravity
        obj->physics->acceleration += glm::vec3(0, -GRAVITY * obj->physics->gravityScale, 0);

        // Integrate
        integrate(obj, dt);

        // Collide + resolve
        checkCollisions(obj);
        //resolveCollision(obj);

        // Reset per-frame data
        obj->physics->acceleration = glm::vec3(0);
    }
}

void PhysicsSystem::integrate(GameObject* obj, float dt) {
    // apply force 
    obj->physics->velocity += obj->physics->acceleration * dt;

    //apply drag
	obj->physics->velocity *= (1.0f - obj->physics->drag * dt);

	//clamp velocity
	if (glm::length(obj->physics->velocity) > obj->physics->maxSpeed) {
		obj->physics->velocity = glm::normalize(obj->physics->velocity) * obj->physics->maxSpeed;
	}

    //debug
    if (obj->id == 0) {
		printf("Velocity: %f %f %f\n", obj->physics->velocity.x, obj->physics->velocity.y, obj->physics->velocity.z);
		printf("Acceleration: %f %f %f\n", obj->physics->acceleration.x, obj->physics->acceleration.y, obj->physics->acceleration.z);
		printf("Position: %f %f %f\n", obj->transform.position.x, obj->transform.position.y, obj->transform.position.z);
    }

	obj->transform.position += obj->physics->velocity * dt;
    obj->transform.aabb = getAABB(obj);
}


void PhysicsSystem::handleGrapple(GameObject* obj, float dt) {
    PhysicsComponent* phys = obj->physics;
    phys->grappleTimer += dt;

	float t = glm::clamp(phys->grappleTimer / 0.6f, 0.0f, 1.0f);
    glm::vec3 start = obj->transform.position;
    glm::vec3 mid = (start + phys->grappleTarget) * 0.5f + glm::vec3(0, 5.0f, 0);
	glm::vec3 newPos = bezier(start, mid, phys->grappleTarget, t);

	phys->velocity = (newPos - obj->transform.position) / dt;
    obj->transform.position = newPos;

	if (t >= 1.0f) {
		phys->grappling = false;
		phys->grappleTimer = 0.0f;
	}
}

AABB PhysicsSystem::getAABB(GameObject* obj) {
    glm::vec3 min = obj->transform.position - obj->collider->halfExtents;
    glm::vec3 max = obj->transform.position + obj->collider->halfExtents;
    return { min, max };
}

glm::vec3 getPenetrationVector(const AABB& a, const AABB& b) {
    glm::vec3 pen;

    pen.x = std::min(a.max.x - b.min.x, b.max.x - a.min.x);
    pen.y = std::min(a.max.y - b.min.y, b.max.y - a.min.y);
    pen.z = std::min(a.max.z - b.min.z, b.max.z - a.min.z);

    // Get signs
    pen.x *= (a.min.x < b.min.x) ? -1.0f : 1.0f;
    pen.y *= (a.min.y < b.min.y) ? -1.0f : 1.0f;
    pen.z *= (a.min.z < b.min.z) ? -1.0f : 1.0f;

    return pen;
}

void PhysicsSystem::checkCollisions(GameObject* obj) {
    //general idea
    //1. ensure all AABBs for all objects are initialized already
    //2. iterate through every object and get its AABB
    for (auto sobj : staticObjects) {
        if (obj->id == sobj->id) {
            continue; 
        }
        pair<vec3, float> SATresult = SATOverlapTestExperimental(obj->transform.aabb, sobj->transform.aabb);
        if (SATresult.second != -1.0f) {
            resolveCollision(obj, sobj, SATresult);
            printf("Detected collision between %d and %d\n", obj->id, sobj->id);
        }  
		
    }
	//printf("length of static objects %d\n", staticObjects.size());
    //3. feed the AABB of this object, and of the iterated object, to SATOverlapTestExperimental
    //4. call resolveCollisions with the result, if there is a collision (change params and return for resolveCollisions)
    //test change
    return;
}

void PhysicsSystem::resolveCollision(GameObject* go1, GameObject* go2, const pair<vec3, float>& SATresult) {

    vec3 normal = glm::normalize(SATresult.first);
    float penetration = SATresult.second;

    // Positional correction: push go1 out of go2
    go1->transform.position += normal * penetration;

    // Velocity resolution: bounce off if moving into object
    vec3 relativeVelocity = go1->physics->velocity;
    float velAlongNormal = glm::dot(relativeVelocity, normal);

    if (velAlongNormal < 0.0f) {
        float restitution = 0.1f; // tweak this if you want it more bouncy
        float impulse = -(1.0f + restitution) * velAlongNormal;
        vec3 impulseVec = impulse * normal;

        go1->physics->velocity += impulseVec;
    }
}

void PhysicsSystem::applyInput(const PlayerIntentPacket& intent, int playerId) {
    GameObject* target = nullptr;

    for (auto obj : playerObjects) {
        if (obj->id == playerId) {
            target = obj;
            break;
        }
    }

    if (!target) return;

    PhysicsComponent* phys = target->physics;

    // Movement force settings
    float moveAccel = 10.0f; // tweakable acceleration magnitude
    glm::vec3 accel = glm::vec3(0.0f);

    glm::vec3 delta = glm::vec3(0.015f);
    float azimuth = glm::radians(-intent.azimuthIntent);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), azimuth, up);
    glm::vec3 forward = glm::normalize(glm::vec3(rotation * glm::vec4(0, 0, 1, 0)));
    glm::vec3 right = glm::normalize(glm::cross(up, forward));

    // Directional input
    if (intent.moveForwardIntent)  accel += -forward;
    if (intent.moveBackIntent)     accel += forward;
    if (intent.moveLeftIntent)     accel += -right;
    if (intent.moveRightIntent)    accel += right;
    if (intent.moveUpIntent)       accel += up;
    if (intent.moveDownIntent)     accel += -up;

    // Normalize to prevent diagonal speedup, then scale
    if (glm::length(accel) > 0.0f) {
        accel = glm::normalize(accel) * moveAccel;
    }

    // Apply to physics
    phys->acceleration += accel;

    // Update facing rotation (but not position!)
    glm::quat q = glm::angleAxis(azimuth, glm::vec3(0, 1, 0));
    target->transform.rotation = q;

    // DO NOT touch target->transform.position here!


}

float getOverlap(pair<float, float> interval1, pair<float, float> interval2) {
    //float overlap = -1.0f;

    return min(interval1.second, interval2.second) - max(interval1.first, interval2.first);
}
// // case 1: intervals are separate
// if (interval1.second < interval2.first || interval2.second < interval1.first) {
//     return overlap;
// }

// //case 2: one interval is contained in the other
// if (interval1.first >= interval2.first && interval1.second <= interval2.second) {
//     overlap = interval1.second - interval1.first;
// }
// else if (interval2.first > interval1.first && interval2.second < interval1.second) {
//     overlap = interval2.second - interval2.first;
// }

// // case 3: intervals overlap
// if (interval1.first < interval2.first && interval1.second > interval2.second) {
//     overlap = interval1.second - interval2.first;
// }
// else if (interval2.first < interval1.first && interval2.second > interval1.first) {
//     overlap = interval2.second - interval1.first;
// }

// return overlap;

pair<vec3, float> PhysicsSystem::SATOverlapTestExperimental(AABB a, AABB b) {
    //vector<vec3> normals1 = getFaceNormals(go1);
    //vector<vec3> normals2 = getFaceNormals(go2);

    //vector<vec3> axes = getCrossProducts(normals1, normals2);
    //addNormalsToAxes(axes, normals1);
    //addNormalsToAxes(axes, normals2);

    vec3 axes[3] = { vec3(1, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1) };

    float minOverlap = 999999.0f;
    vec3 minAxis = vec3(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < 3; i++) {
        pair<float, float> interval1 = { a.min[i], a.max[i] };
        pair<float, float> interval2 = { b.min[i], b.max[i] };

        float overlap = getOverlap(interval1, interval2);
        //      if (overlap < 0.0f) {   // or <= ?????
        //          return pair<vec3, float>(vec3(0.0f, 0.0f, 0.0f), -1.0f);
        //      }

              //// check for small epsilon interval, not 0 to avoid floating point arithmetic issues
        //      if (minOverlap == 0.0f || overlap < minOverlap) {
        //          minOverlap = overlap;
        //          minAxis = axes[i];
        //      }

        if (overlap <= 0.0f) {  // use <= to be safe, objects just touching
            return pair<vec3, float>(vec3(0.0f), -1.0f);
        }

        if (overlap < minOverlap) {
            minOverlap = overlap;
            minAxis = axes[i];
        }
    }

    return pair<vec3, float>(minAxis, minOverlap);
}

GameObject* PhysicsSystem::makeGameObject() {
    GameObject* obj = new GameObject;
    obj->id = dynamicObjects.size() + staticObjects.size() + 10;
    obj->transform.position = glm::vec3(0.0f);
	obj->transform.rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); // Identity quaternion
    obj->transform.scale = glm::vec3(1.0f);

    obj->physics = new PhysicsComponent();
    obj->collider = new ColliderComponent();

    obj->collider->halfExtents = glm::vec3(1.0f, 1.0f, 1.0f);

    obj->isDynamic = false;
    obj->transform.aabb = getAABB(obj);

    return obj; // return reference to the stored one
}


GameObject* PhysicsSystem::makeGameObject(glm::vec3 position, glm::quat rotation, glm::vec3 halfExtents) {
	GameObject* obj = new GameObject;
	obj->id = dynamicObjects.size() + staticObjects.size() + 10;
	obj->transform.position = position;
	obj->transform.rotation = rotation;
	obj->transform.scale = glm::vec3(1.0f);

	obj->physics = new PhysicsComponent();
	obj->collider = new ColliderComponent();

	obj->collider->halfExtents = halfExtents;

	obj->isDynamic = false;
	obj->transform.aabb = getAABB(obj);

    return obj;
}

// Convert position + quaternion to mat4
glm::mat4 PhysicsSystem::toMatrix(const glm::vec3& position, const glm::quat& quaternion) {
    glm::mat4 T = glm::translate(glm::mat4(1.0f), position);
	glm::mat4 R = glm::toMat4(glm::quat(quaternion));
	//glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
	return T * R;
}

// Extract position and euler angles (in radians) from a mat4
void PhysicsSystem::fromMatrix(const glm::mat4& mat, glm::vec3& outPosition, glm::vec3& outEulerRadians) {
    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;

    glm::decompose(mat, scale, rotation, translation, skew, perspective);

    outPosition = translation;
    outEulerRadians = glm::eulerAngles(rotation);
}

// ignore for now
void PhysicsSystem::getAABBsDistance(std::vector<GameObject*> gobjs) {
    AABB temp;
    for (GameObject* go : gobjs) {
        temp = getAABB(go);
        AABBdistances.push_back(glm::distance(temp.min, temp.max));
    }
}

// ignore for now
float PhysicsSystem::getCellSize() {
    //getAABBsDistance(staticObjects);
    //getAABBsDistance(dynamicObjects);

    //// get 90th percentile size
    //sort(AABBdistances.begin(), AABBdistances.end(), [](const float& a, const float& b) { return a > b; });
    //int index = (int) AABBdistances.size() * 0.9;
    //
    //return AABBdistances.at(index);
	return 1.0f; // placeholder
}

// ignore for now
void PhysicsSystem::populateGrid() {
    cellSize = getCellSize();

    int numCellsX = (int)(XD / cellSize);
    int numCellsY = (int)(YD / cellSize);
    int numCellsZ = (int)(ZD / cellSize);
    

    for (GameObject* go : staticObjects) {
        glm::vec3 pos = go->transform.position;

        int cellX = (int)(pos[0] / numCellsZ);
        int cellY = (int)(pos[1] / numCellsY);
        int cellZ = (int)(pos[2] / numCellsZ);

        vector<int> cellsX;
        vector<int> cellsY;
        vector<int> cellsZ;

        float minx = go->transform.aabb.min[0];
        float maxx = go->transform.aabb.max[0];
        
        while (minx < maxx) {
            int index = (int) (minx / numCellsX);
            cellsX.push_back(index);
            minx += cellSize;
        }
    }
}

// ignore for now
float PhysicsSystem::getBoxDim(GameObject* go) {
    float AABBMag = glm::distance(go->transform.aabb.min, go->transform.aabb.max);
    return AABBMag / sqrtf(3.0f);
}

// ignore for now
std::pair<float, float> PhysicsSystem::projetBox(GameObject *go, glm::vec3 axis, glm::mat3 rotationMat) {
    float center = glm::dot((go->transform.position), axis);
    float radius = 0;
    for (int i = 0; i < 3; i++) {
        radius += go->collider->halfExtents[i] * abs(glm::dot(rotationMat[i], axis));
    }
    return pair<float, float>(center - radius, center + radius);
}

// ignore for now
void PhysicsSystem::SAT(GameObject* go1, GameObject* go2) {
    float box1dim = getBoxDim(go1);
    float box2dim = getBoxDim(go2);

    mat3 rotationMat1 = glm::mat3_cast(go1->transform.rotation);
    mat3 rotationMat2 = glm::mat3_cast(go2->transform.rotation);

    vector<vec3> crossProds;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            vec3 prod = cross(rotationMat1[i], rotationMat2[j]);
            crossProds.push_back(prod);
        }
    }

    glm::vec3 smallestAxis = glm::vec3(0.0f, 0.0f, 0.0f);

    // for (glm::vec3 axis : crossProds) {
    //     std::pair<float, float> interval1 = projectBox(go1, axis, rotationMat1);
    //     std::pair<float, float> interval2 = projectBox(go2, axis, rotationMat2);

    //     // if (interval[0] <= interval2[0] && interval1[1] >= interval2[0]) {

    // }
}

vector<vec3> getFaceNormals(GameObject* go) {
    return { go->transform.rotation * vec3(1, 0, 0),
             go->transform.rotation * vec3(0, 1, 0),
             go->transform.rotation * vec3(0, 0, 1) };
}

bool areParallel(vec3 crossProd) {
    return glm::dot(crossProd, crossProd) < 1e-6;
}

vector<vec3> getCrossProducts(const vector<vec3>& normals1, const vector<vec3>& normals2) {
    vector<vec3> crossProducts;
    for (int i = 0; i < normals1.size(); i++) {
        for (int j = 0; j < normals2.size(); j++) {
            vec3 crossProd = glm::cross(normals1[i], normals2[j]);
            if (!areParallel(crossProd)) {
                crossProducts.push_back(glm::normalize(crossProd));
            }
        }
    }
    return crossProducts;
}

void addNormalsToAxes(vector<vec3>& axes, const vector<vec3>& normals) {
    for (int i = 0; i < normals.size(); i++) {
        axes.push_back(normals[i]);
    }
}

pair<float, float> getInterval(const vec3& center, const vec3& halfExtents, const vector<vec3>& normals, const vec3& axis) {
    float centerProj = glm::dot(center, axis);
    float halfRadius = 0.0f;

    for (int i = 0; i < 3; i++) {
        halfRadius += halfExtents[i] * fabs(glm::dot(normals[i], axis));
    }

    return pair<float,float>(centerProj - halfRadius, centerProj + halfRadius);
}


pair<vec3, float> SATOverlapTest(GameObject* go1, GameObject* go2) {
    vector<vec3> normals1 = getFaceNormals(go1);
    vector<vec3> normals2 = getFaceNormals(go2);

    vector<vec3> axes = getCrossProducts(normals1, normals2);
    addNormalsToAxes(axes, normals1);
    addNormalsToAxes(axes, normals2);

    float minOverlap = 0.0f;
    vec3 minAxis = vec3(0.0f, 0.0f, 0.0f);

    for (vec3& axis : axes) {
        pair<float, float> interval1 = getInterval(go1->transform.position, go1->collider->halfExtents, normals1, axis);
        pair<float, float> interval2 = getInterval(go2->transform.position, go2->collider->halfExtents, normals2, axis);


        //get overlap    return min(interval1.second, interval2.second) - max(interval1.first, interval2.first);

        float overlap = getOverlap(interval1, interval2);
        if (overlap < 0.0f) {   // or <= ?????
            return pair<vec3, float>(vec3(0.0f, 0.0f, 0.0f), -1.0f);
        }
        if (minOverlap == 0.0f || overlap < minOverlap) {
            minOverlap = overlap;
            minAxis = axis;
        }
    }

    return pair<vec3, float>(minAxis, minOverlap);
}
vec3 getPointOfContact(GameObject* go1, GameObject* go2) {

    pair<vec3, float> overlapData = SATOverlapTest(go1, go2);
    if (overlapData.second < 0.0f) {
        return vec3(0.0f, 0.0f, 0.0f);
    }
    float overlap = overlapData.second;
    vec3 axis = glm::normalize(overlapData.first);

    if (glm::dot(go2->transform.position-go1->transform.position, axis) < 0.0f) {
        axis = -axis;
    }

    vec3 center1Translated = go1->transform.position + 0.5f * overlap * axis;
    vec3 center2Translated = go2->transform.position - 0.5f * overlap * axis;

    return (center1Translated + center2Translated) * 0.5f;
}
//test