#pragma once

#include <vector>
#include <glm/glm.hpp>
// "PhysicsSystem.h"
#include "PhysicsData.h"

class GameObject; // Forward declaration
struct AABB; // Forward declaration
using namespace std;
typedef glm::vec3 vec3;

class Node {
    private:
        AABB boundingBox;
        vec3 halfExtents;
        vec3 center;
        Node* children[8];
        vector<GameObject*> objects;
        bool isLeaf;
        int depthLevel;
    
    public:
        Node(const AABB& boundingBox, int depthLevel = 0);
        ~Node();

        bool contains(const AABB& box) const;
        bool partiallyEmbedded(const AABB& box) const;
        
        // Getters
        const AABB& getBoundingBox() const { return boundingBox; }
        const vector<GameObject*>& getObjects() const { return objects; }
        bool isLeafNode() const { return isLeaf; }
        int getDepthLevel() const { return depthLevel; }
        Node* getChild(int index) const { return children[index]; }
        const vec3& getCenter() const { return center; }
        const vec3& getHalfExtents() const { return halfExtents; }

		// Setters
		void setChild(int index, Node* child) { children[index] = child; }
		void setLeaf(bool leaf) { isLeaf = leaf; }

        // Methods
        void addObject(GameObject* obj) {
            objects.push_back(obj);
        }
        void removeObject(GameObject* obj);
};

class Octree {
    private:
        Node* root;
        int maxDepth;
        int maxObjectsPerNode;
        int toggle = 0;
        AABB boundingBox;
		vector<GameObject*>& objectsInTree;

    public:
        Octree(const AABB& boundingBox, vector<GameObject*>& objectsInTree, int maxDepth, int maxObjectsPerNode);
        ~Octree();

        bool shouldSubdivide(const Node* node) const;
        void subdivide(Node* node);

        void constructTree(const vector<GameObject*>& objects);
        void insert(GameObject* obj, Node* node);
        void deleteObject(GameObject* obj);

        void getPotentialCollisionPairs(const AABB& box, vector<GameObject*>& potentialCollisions) const;

        void updateObject(GameObject* obj);

        // getters
		int getMaxDepth() const { return maxDepth; }
        int getMaxObjectsPerNode() const { return maxObjectsPerNode; }
        Node* getRoot() const { return root; }
};