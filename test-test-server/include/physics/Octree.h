#include <vector>
#include <glm/glm.hpp>
#include "PhysicsSystem.h"
#include "PhysicsData.h"

class Node {
    private:
        AABB boundingBox;
        vec3 halfExtents = (boundingBox.max - boundingBox.min) * 0.5f;
        vec3 center = boundingBox.min + halfExtents;
        Node* children[8];
        Node* parent;
        vector<GameObject*> objects;
        bool isLeaf;
        int depthLevel;
    
    public:
        Node(const AABB& boundingBox, Node* parent = nullptr, int depthLevel = 0);
        ~Node();

        bool contains(const AABB& box);
        bool partiallyEmbedded(const AABB& box);
        
        // Getters
        const AABB& getBoundingBox() const { return boundingBox; }
        const vector<GameObject*>& getObjects() const { return objects; }
        bool isLeafNode() const { return isLeaf; }
        int getDepthLevel() const { return depthLevel; }
        Node* getParent() const { return parent; }
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
        Octree(const AABB& boundingBox, vector<GameObject*> objectsInTree, int maxDepth, int maxObjectsPerNode);
        ~Octree();

        bool shouldSubdivide(const Node* node);
        void subdivide(Node* node);

        void constructTree(const vector<GameObject*>& objects);
        void reconstructTree(const vector<GameObject*>& objects);
        void insert(GameObject* obj, Node* node);

		int getMaxDepth() const { return maxDepth; }
        int getMaxObjectsPerNode() const { return maxObjectsPerNode; }
        Node* getRoot() const { return root; }
};