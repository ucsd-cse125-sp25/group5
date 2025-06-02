#include "../include/physics/Octree.h"
#include "../include/physics/PhysicsSystem.h"
#include <ServerGame.h>

static bool overlap(const AABB& box1, const AABB& box2) {
    return (box1.min.x <= box2.max.x && box1.max.x >= box2.min.x) &&
        (box1.min.y <= box2.max.y && box1.max.y >= box2.min.y) &&
        (box1.min.z <= box2.max.z && box1.max.z >= box2.min.z);
}

static bool isPointInside(const vec3& point, const AABB& box) {
    return point.x >= box.min.x && point.x <= box.max.x &&
        point.y >= box.min.y && point.y <= box.max.y &&
        point.z >= box.min.z && point.z <= box.max.z;
}

static void updateAABB(vec3& position, vec3& halfExtents, AABB& aabb) {
    aabb.min = position - halfExtents;
    aabb.max = position + halfExtents;
}

static void updateAABBGameObject(GameObject* obj) {
    if (obj == nullptr || obj->collider == nullptr) {
        return; // Ensure the object and its collider are valid
    }
    updateAABB(obj->transform.position, obj->collider->halfExtents, obj->transform.aabb);
}

static void updateAABBGameObjects(vector<GameObject*>& objects) {
    for (auto obj : objects) {
        updateAABBGameObject(obj);
    }
}

Node::Node(const AABB& boundingBox, int depthLevel)
    : boundingBox(boundingBox), depthLevel(depthLevel), isLeaf(true) {
	halfExtents = (boundingBox.max - boundingBox.min) * 0.5f;
	center = boundingBox.min + halfExtents;
    for (int i = 0; i < 8; i++) {
        children[i] = nullptr;
    }
}

Node::~Node() {
    if (!isLeaf) {
        for (int i = 0; i < 8; i++) {
            delete children[i];
            children[i] = nullptr;
        }
    }
    objects.clear();
}

bool Node::contains(const AABB& box) const {
    return (box.min.x >= boundingBox.min.x && box.max.x <= boundingBox.max.x) &&
           (box.min.y >= boundingBox.min.y && box.max.y <= boundingBox.max.y) &&
		   (box.min.z >= boundingBox.min.z && box.max.z <= boundingBox.max.z);
}

bool Node::partiallyEmbedded(const AABB& box) const {
    return overlap(boundingBox, box) || !contains(box);
}

bool Octree::shouldSubdivide(const Node* node) const {
	if (!node) return false;
    return node->getObjects().size() > this->getMaxObjectsPerNode() && node->getDepthLevel() < this->getMaxDepth();
}

AABB getBoundingBox(const vec3& center, const vec3& halfExtents, int i) {
    vec3 childMin{}, childMax{};
        
    if (i & 1) {
        childMin.x = center.x;
        childMax.x = center.x + halfExtents.x; 
    } else {
        childMin.x = center.x - halfExtents.x;
        childMax.x = center.x;
    }
    if (i & 2) {
        childMin.y = center.y;
        childMax.y = center.y + halfExtents.y; 
    } else {
        childMin.y = center.y - halfExtents.y;
        childMax.y = center.y;
    }
    if (i & 4) {
        childMin.z = center.z;
        childMax.z = center.z + halfExtents.z; 
    } else {
        childMin.z = center.z - halfExtents.z;
        childMax.z = center.z;
    }

    return AABB{childMin, childMax};
}

void Octree::subdivide(Node* node) {
    if (!node->isLeafNode()) return;

    vec3 halfExtents = node->getHalfExtents();
    vec3 center = node->getCenter();

    // Create 8 children nodes
    for (int i = 0; i < 8; i++) {
        AABB childBox = getBoundingBox(center, halfExtents, i);
		node->setChild(i, new Node(childBox, node->getDepthLevel() + 1));
    }
    
	node->setLeaf(false);

	vector<GameObject*> objects = node->getObjects();
    for (GameObject* obj : objects) {
        for (int i = 0; i < 8; i++) {
			// Check if the object is fully contained in the child node's bounding box
			Node* childNode = node->getChild(i);
            if (childNode->contains(obj->transform.aabb)) {
                insert(obj, childNode);
                break;
            } else if (childNode->partiallyEmbedded(obj->transform.aabb)) {
                // If the object intersects with the child node, insert it
                insert(obj, childNode);
            }
        }
    }
}

void Octree::insert(GameObject* obj, Node* node) {
    if (!node) return;

    if (node->isLeafNode()) {
        if (shouldSubdivide(node)) {
            subdivide(node);
            insert(obj, node);
        } else {
            if (node->getObjects().size() < maxObjectsPerNode) {
                node->addObject(obj);
            } else {
                if (toggle == 0) {
                    toggle = 1;
                    maxDepth *= 2;
                } else {
                    toggle = 0;
                    maxObjectsPerNode *= 2;
                }
                constructTree(objectsInTree);
                insert(obj, node);
            }
        }
    } else {
        for (int i = 0; i < 8; i++) {
            Node* childNode = node->getChild(i);
            if (childNode->contains(obj->transform.aabb)) {
                insert(obj, childNode);
                return;
            } else if (childNode->partiallyEmbedded(obj->transform.aabb)) {
                insert(obj, childNode);
            }
        }
    }
}

void Node::removeObject(GameObject* obj) {
    auto it = std::find(objects.begin(), objects.end(), obj);
    if (it != objects.end()) {
        objects.erase(it);
    } else if (!this->isLeaf) {
        for (int i = 0; i < 8; i++) {
            this->children[i]->removeObject(obj);
        }
    }
}

void Octree::constructTree(const vector<GameObject*>& objects) {
    if (root) delete root;

    root = new Node(boundingBox);
    for (GameObject* obj : objects) {
        insert(obj, root);
    }
}

Octree::Octree(const AABB& boundingBox, vector<GameObject*>& objectsInTree, int maxDepth, int maxObjectsPerNode)
    : boundingBox(boundingBox), objectsInTree(objectsInTree), maxDepth(maxDepth), maxObjectsPerNode(maxObjectsPerNode) {
    root = new Node(boundingBox);
}

Octree::~Octree() {
    delete root;
    root = nullptr;
}

void Octree::getPotentialCollisionPairs(const AABB& box, vector<GameObject*>& potentialCollisions) const {  
    if (!root) return;  

    vector<Node*> nodesToCheck;  
    nodesToCheck.push_back(root);  

    while (!nodesToCheck.empty()) {  
        Node* currentNode = nodesToCheck.back();  
        nodesToCheck.pop_back();  

        if (overlap(currentNode->getBoundingBox(), box)) {  
            if (currentNode->isLeafNode()) {  
                for (GameObject* obj : currentNode->getObjects()) {  
                    if (find(potentialCollisions.begin(), potentialCollisions.end(), obj) != potentialCollisions.end()) {  
                        continue; // Skip if already checked or added  
                    }  
                    potentialCollisions.push_back(obj);  
                }  
            } else {  
                for (int i = 0; i < 8; i++) {
                    Node* child = currentNode->getChild(i);
                    if (child) nodesToCheck.push_back(child);  
                }  
            }  
        }  
    }  
}

void Octree::updateObject(GameObject* obj) {  
    if (!root) return;  

    // Remove the object from the current tree  
    root->removeObject(obj);  

    // Reinsert the object into the tree  
	updateAABBGameObject(obj);
    insert(obj, root);  
}