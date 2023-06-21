#pragma once

#include <vector>
#include <string>
#include <float.h>
#include <cmath>

#include "ray.hpp"

// AABB slab
class AABB {
public:
    AABB() {}
    AABB(const Vector3f &min, const Vector3f &max) : min(min), max(max) {}

    Vector3f min;
    Vector3f max;

    // intersect slab
    bool intersect(const Ray &r, float tmin, float tmax=FLT_MAX) const {
        for (int i = 0; i < 3; i++) {
            float invD = 1.0f / r.getDirection()[i];
            float f0 = (min[i] - r.getOrigin()[i]) * invD;
            float f1 = (max[i] - r.getOrigin()[i]) * invD;
            float t0 = fmin(f0, f1);
            float t1 = fmax(f0, f1);

            tmin = fmax(t0, tmin);
            tmax = fmin(t1, tmax);
            if (tmax <= tmin) {
                return false;
            }
        }
        return true;
    }

    static AABB surrounding_box(AABB box0, AABB box1) {
        Vector3f small(fmin(box0.min.x(), box1.min.x()),
            fmin(box0.min.y(), box1.min.y()),
            fmin(box0.min.z(), box1.min.z()));
        Vector3f big(fmax(box0.max.x(), box1.max.x()),
            fmax(box0.max.y(), box1.max.y()),
            fmax(box0.max.z(), box1.max.z()));
        return AABB(small, big);
    }
};

