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
    return node->getObjects().size() > this->getMaxObjectsPerNode() && node->getDepthLevel() < this->getMaxDepth();
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
    if (!node->isLeafNode()) return;

    vec3 halfExtents = node->getHalfExtents();
    vec3 center = node->getCenter();

    // Create 8 children nodes
    for (int i = 0; i < 8; i++) {
        AABB childBox = getBoundingBox(center, halfExtents, i);
		node->setChild(i, new Node(childBox, node, node->getDepthLevel() + 1));
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

void Octree::insert(GameObject* obj, const Node* node) {
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
                    maxDepth++;
                } else {
                    toggle = 0;
                    maxObjectsPerNode++;
                }
                reconstructTree();
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