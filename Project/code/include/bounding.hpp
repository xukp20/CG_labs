#pragma once

#include <vector>
#include <string>

#include "ray.hpp"

// AABB slab
class AABB {
public:
    AABB() {}
    AABB(const Vector3f &min, const Vector3f &max) : min(min), max(max) {}

    Vector3f min;
    Vector3f max;

    bool hit(const Ray &r, float tmin, float tmax) const {
        for (int i = 0; i < 3; i++) {
            float invD = 1.0f / r.getDirection()[i];
            float t0 = (min[i] - r.getOrigin()[i]) * invD;
            float t1 = (max[i] - r.getOrigin()[i]) * invD;
            if (invD < 0.0f) {
                std::swap(t0, t1);
            }
            tmin = t0 > tmin ? t0 : tmin;
            tmax = t1 < tmax ? t1 : tmax;
            if (tmax <= tmin) {
                return false;
            }
        }
        return true;
    }
}