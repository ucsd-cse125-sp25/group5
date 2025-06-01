#include "AABB.h"
#include <utility>
#include <algorithm>

float AABB::getDistanceToCenter(const vec3& point) const {
    return glm::length(getCenter() - point);
}

vec3 AABB::getDirectionVectorToCenter(const vec3& point) const {
    return getCenter() - point;
}

bool AABB::containsPoint(const vec3& point) const {
    return (point.x >= min.x && point.x <= max.x) &&
           (point.y >= min.y && point.y <= max.y) &&
           (point.z >= min.z && point.z <= max.z);
}

bool AABB::completelyEmbedded(const AABB& other) const {
    return (min.x > other.min.x && max.x < other.max.x) &&
           (min.y > other.min.y && max.y < other.max.y) &&
           (min.z > other.min.z && max.z < other.max.z);
}

bool AABB::partiallyEmbedded(const AABB& other) const {
    return getAABBPenetration(other).second > 0.0f;
}

AABB AABB::overlapRegion(const AABB& other) const {
    vec3 overlapMin = glm::max(min, other.min);
    vec3 overlapMax = glm::min(max, other.max);

    // If there's no overlap, return an empty AABB
    if (overlapMin.x > overlapMax.x || overlapMin.y > overlapMax.y || overlapMin.z > overlapMax.z) {
        return AABB(vec3(0.0f), vec3(0.0f));
    }

    return AABB(overlapMin, overlapMax);
}

void AABB::updateAABB(const vec3& newCenter, const vec3& newHalfExtents) {
    min = newCenter - newHalfExtents;
    max = newCenter + newHalfExtents;
}

pair<vec3, float> AABB::getAABBPenetration(const AABB& other) const {
    vec3 axes[3] = { vec3(1, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1) };

	float minOverlap = 999999.0f;
	vec3 minAxis = vec3(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < 3; i++) {
        pair<float, float> interval1 = { min[i], max[i] };
        pair<float, float> interval2 = { other.min[i], other.max[i] };


		vec3 dir = getDirectionVectorToCenter(other.getCenter());
        float overlap = std::min(interval1.second, interval2.second) - std::max(interval1.first, interval2.first);

		if (overlap <= 0.0f) {   
			return pair<vec3, float>(vec3(0.0f), overlap); // No overlap
		}

        if (overlap < minOverlap) {
            minOverlap = overlap;
            minAxis = axes[i] * (dir[i] > 0 ? 1.0f : -1.0f); // Choose the axis direction based on the distance vector
        }
    }
    return pair<vec3, float>(minAxis, minOverlap);
}