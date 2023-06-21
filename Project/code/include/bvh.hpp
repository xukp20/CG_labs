#pragma once

#include <vector>
#include <string>
#include <float.h>
#include <algorithm>

#include "ray.hpp"
#include "group.hpp"
#include "object3d.hpp"
#include "rand.hpp"

class BVHNode : public Object3D {
public:
    BVHNode() {}
    BVHNode(std::vector<Object3D*> &objects, int start, int end, double time0, double time1) {
        int axis = int(3 * RAND_UNIFORM);
        if (axis == 0) {
            std::sort(objects.begin() + start, objects.begin() + end, box_x_compare);
        } else if (axis == 1) {
            std::sort(objects.begin() + start, objects.begin() + end, box_y_compare);
        } else {
            std::sort(objects.begin() + start, objects.begin() + end, box_z_compare);
        }

        int n = end - start;
        if (n == 1) {
            left = right = objects[start];
        } else if (n == 2) {
            left = objects[start];
            right = objects[start + 1];
        } else {
            left = new BVHNode(objects, start, start + n / 2, time0, time1);
            right = new BVHNode(objects, start + n / 2, end, time0, time1);
        }

        AABB box_left, box_right;
        if (!left->bounding_box(time0, time1, box_left) || !right->bounding_box(time0, time1, box_right)) {
            std::cerr << "No bounding box in BVHNode constructor.\n";
        }
        box = AABB::surrounding_box(box_left, box_right);
    }

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        if (!box.intersect(r, tmin, FLT_MAX)) return false;
        bool hit_left = left->intersect(r, h, tmin);
        bool hit_right = right->intersect(r, h, tmin);
        return hit_left || hit_right;
    }

    bool bounding_box(double _time0, double _time1, AABB &output_box) override {
        output_box = box;
        return true;
    }

private:
    Object3D *left;
    Object3D *right;
    AABB box;

    static inline bool box_compare(Object3D *a, Object3D *b, int axis) {
        AABB box_a, box_b;
        if (!a->bounding_box(0, 0, box_a) || !b->bounding_box(0, 0, box_b)) {
            std::cerr << "No bounding box in BVHNode constructor.\n";
        }
        return box_a.min[axis] < box_b.min[axis];
    }

    static bool box_x_compare(Object3D *a, Object3D *b) {
        return box_compare(a, b, 0);
    }

    static bool box_y_compare(Object3D *a, Object3D *b) {
        return box_compare(a, b, 1);
    }

    static bool box_z_compare(Object3D *a, Object3D *b) {
        return box_compare(a, b, 2);
    }
};