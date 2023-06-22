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
    bool intersect(const Ray &r, float &tmin) const {
        Vector3f origin(r.getOrigin());
        Vector3f invdir(1 / r.getDirection().x(), 1 / r.getDirection().y(), 1 / r.getDirection().z());
        std::vector<bool> sign(3);

        sign[0] = (invdir.x() < 0);
        sign[1] = (invdir.y() < 0);
        sign[2] = (invdir.z() < 0);
        float t_min = ((sign[0] ? max.x() : min.x()) - origin.x()) * invdir.x();
        float t_max = ((sign[0] ? min.x() : max.x()) - origin.x()) * invdir.x();

        float ty_min = ((sign[1] ? max.y() : min.y()) - origin.y()) * invdir.y();
        float ty_max = ((sign[1] ? min.y() : max.y()) - origin.y()) * invdir.y();

        if ((t_min > ty_max) || (ty_min > t_max))
            return false;
        if (ty_min > t_min)
            t_min = ty_min;
        if (ty_max < t_max)
            t_max = ty_max;
        
        float tz_min = ((sign[2] ? max.z() : min.z()) - origin.z()) * invdir.z();
        float tz_max = ((sign[2] ? min.z() : max.z()) - origin.z()) * invdir.z();

        if ((t_min > tz_max) || (tz_min > t_max))
            return false;
        if (tz_min > t_min)
            t_min = tz_min;
        if (tz_max < t_max)
            t_max = tz_max;

        // if (t_min < tmin) {
        //     tmin = t_min;
        //     return true;
        // } else {
        //     return false;
        // }
        tmin = t_min;
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

    bool inside(Vector3f p) const {
        return p.x() >= min.x() && p.x() <= max.x() &&
            p.y() >= min.y() && p.y() <= max.y() &&
            p.z() >= min.z() && p.z() <= max.z();
    }
};

