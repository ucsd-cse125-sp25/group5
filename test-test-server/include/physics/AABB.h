#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

typedef glm::vec3 vec3;
using namespace std;

class AABB {
    private:
        vec3 min;
        vec3 max;

    public:
        AABB(const vec3& min, const vec3& max) : min(min), max(max) {}
        AABB() : min(vec3(0.0f)), max(vec3(0.0f)) {}

        const vec3& getMin() const { return min; }
        const vec3& getMax() const { return max; }
        vec3 getCenter() const {
            return (min + max) * 0.5f;
        }
        vec3 getHalfExtents() const {
            return (max - min) * 0.5f;
        }
        float getDistanceToCenter(const vec3& point) const;
        vec3 getDirectionVectorToCenter(const vec3& point) const;
        pair<vec3, float> getAABBPenetration(const AABB& other) const;

        void setMin(const vec3& newMin) { min = newMin; }
        void setMax(const vec3& newMax) { max = newMax; }

        bool containsPoint(const vec3 &point) const;

        bool partiallyEmbedded(const AABB& other) const;
        bool completelyEmbedded(const AABB& other) const;
        AABB overlapRegion(const AABB& other) const;

        void updateAABB(const vec3& newCenter, const vec3& newHalfExtents);
};