#include "Octree.h"

Node::Node(const AABB& boundingBox, Node* parent, int depthLevel)
    : boundingBox(boundingBox), parent(parent), depthLevel(depthLevel), isLeaf(true) {
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

bool isPointInside(const vec3& point, const AABB& box) {
    return point.x >= box.min.x && point.x <= box.max.x &&
           point.y >= box.min.y && point.y <= box.max.y &&
           point.z >= box.min.z && point.z <= box.max.z;
}

bool Node::contains(const AABB& box) {
    return isPointInside(box.min, boundingBox) && isPointInside(box.max, boundingBox);
}

bool Node::partiallyEmbedded(const AABB& box) {
    return (isPointInside(box.min, boundingBox) && !isPointInside(box.max, boundingBox)) ||
            (!isPointInside(box.min, boundingBox) && isPointInside(box.max, boundingBox));
           
}

bool Octree::shouldSubdivide(const Node* node) {
    return node->objects.size() > this.maxObjectsPerNode && node->depthLevel < this.maxDepth;
}

AABB getBoundingBox(const vec3& center, const vec3& halfExtents, int i) {
    vec3 childMin, childMax;
        
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
    if (!node->isLeaf) return;

    vec3 halfExtents = node->getHalfExtents();
    vec3 center = node->getCenter();

    // Create 8 children nodes
    for (int i = 0; i < 8; i++) {
        AABB childBox = getBoundingBox(center, halfExtents, i);
        node->children[i] = new Node(childBox, node, node->depthLevel + 1);
    }
    
    node->isLeaf = false;

    for (GameObject* obj : node->objects) {
        // Insert each object into the appropriate child node
        for (int i = 0; i < 8; i++) {
            if (node->children[i]->contains(obj->transform.aabb)) {
                node->children[i].insert(obj);
                break;
            } else if (node->children[i]->partiallyEmbedded(obj->transform.aabb)) {
                // If the object intersects with the child node, insert it
                node->children[i].insert(obj);
            }
        }
    }
}

void Node::insert(GameObject* obj, const Octree& octree) {
    if (this->isLeaf) {
        if octree.shouldSubdivide(this) {
            octree->subdivide(this);
            insert(obj, octree);
        } else {
            if (this->objects.size() < octree.maxObjectsPerNode) {
                this->objects.push_back(obj);
            } else {
                if (octree.toggle == 0) {
                    octree.toggle = 1;
                    octree.maxDepth++;
                } else {
                    octree.toggle = 0;
                    octree.maxObjectsPerNode++;
                }
                octree.reconstructTree();
                insert(obj, octree);
            }
        }
    } else {
        for (int i = 0; i < 8; i++) {
            if (this->children[i]->contains(obj->transform.aabb)) {
                this->children[i]->insert(obj, octree);
                return;
            } else if (this->children[i]->partiallyEmbedded(obj->transform.aabb)) {
                this->children[i]->insert(obj, octree);
            }
        }
    }
}

void Node::remove(GameObject* obj) {
    auto it = std::find(objects.begin(), objects.end(), obj);
    if (it != objects.end()) {
        objects.erase(it);
    }
    if !this->isLeaf {
        for (int i = 0; i < 8; i++) {
            this->children[i]->remove(obj);
        }
    }
}

void Octree::reconstructTree(const vector<GameObject*>& objects) {
    if (root) {
        delete root;
        root = nullptr;
    }

    root = new Node();

    for (GameObject* obj : objects) {
        root->insert(obj, *this);
    }
}

Octree::Octree(const AABB& boundingBox, int maxDepth, int maxObjectsPerNode)
    : boundingBox(boundingBox), maxDepth(maxDepth), maxObjectsPerNode(maxObjectsPerNode) {
    root = new Node(boundingBox);
}

Octree::~Octree() {
    delete root;
    root = nullptr;
}