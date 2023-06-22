#pragma once

#include <vector>
#include <string>

#include "bounding.hpp"
#include "object3d.hpp"

class Box: public Object3D {
public:
    Box(const Vector3f &p0, const Vector3f &p1, Material *m): Object3D(m), pmin(p0), pmax(p1) {
        Vector3f min(fmin(p0.x(), p1.x()), fmin(p0.y(), p1.y()), fmin(p0.z(), p1.z()));
        Vector3f max(fmax(p0.x(), p1.x()), fmax(p0.y(), p1.y()), fmax(p0.z(), p1.z()));
        pmin = min;
        pmax = max;
        bounding = new AABB(min, max);
    }

    ~Box() override {
        delete bounding;
    }

    bool near(float x, float y) {
        return fabs(x - y) < 0.0001;
    }

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        // use bounding box to check if intersect
        float t0 = tmin;
        if (!bounding->intersect(r, t0)) {
            return false;
        }
        if (t0 < tmin) {
            return false;
        }

        // t0 is the nearest intersection point
        // check which place it is and get the normal
        Vector3f normal;
        Vector3f p = r.pointAtParameter(t0);
        Vector3f d = r.getDirection();
        if (near(p.x(), pmin.x())) {
            if (d.x() < 0) {
                normal = Vector3f(-1, 0, 0);
            } else {
                normal = Vector3f(1, 0, 0);
            }
        } else if (near(p.x(), pmax.x())) {
            if (d.x() < 0) {
                normal = Vector3f(1, 0, 0);
            } else {
                normal = Vector3f(-1, 0, 0);
            }
        } else if (near(p.y(), pmin.y())) {
            if (d.y() < 0) {
                normal = Vector3f(0, -1, 0);
            } else {
                normal = Vector3f(0, 1, 0);
            }
        } else if (near(p.y(), pmax.y())) {
            if (d.y() < 0) {
                normal = Vector3f(0, 1, 0);
            } else {
                normal = Vector3f(0, -1, 0);
            }
        } else if (near(p.z(), pmin.z())) {
            if (d.z() < 0) {
                normal = Vector3f(0, 0, -1);
            } else {
                normal = Vector3f(0, 0, 1);
            }
        } else if (near(p.z(), pmax.z())) {
            if (d.z() < 0) {
                normal = Vector3f(0, 0, 1);
            } else {
                normal = Vector3f(0, 0, -1);
            }
        } else {
            std::cout << "Box intersect error" << std::endl;
            return false;
        }

        if (t0 < h.getT()) {
            h.set(t0, material, normal);
            return true;
        }
    }

    bool bounding_box(double t0, double t1, AABB &box) override {
        box = *bounding;
        return true;
    }

    bool finite() override { return true; }
    
    Vector3f pmin, pmax;
    AABB* bounding;
};