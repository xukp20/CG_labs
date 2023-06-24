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
        // get the intersection point
        Vector3f origin(r.getOrigin());
        Vector3f invdir(1 / r.getDirection().x(), 1 / r.getDirection().y(), 1 / r.getDirection().z());
        std::vector<bool> sign(3);

        sign[0] = (invdir.x() < 0);
        sign[1] = (invdir.y() < 0);
        sign[2] = (invdir.z() < 0);
        float t_min = ((sign[0] ? pmax.x() : pmin.x()) - origin.x()) * invdir.x();
        float t_max = ((sign[0] ? pmin.x() : pmax.x()) - origin.x()) * invdir.x();

        float ty_min = ((sign[1] ? pmax.y() : pmin.y()) - origin.y()) * invdir.y();
        float ty_max = ((sign[1] ? pmin.y() : pmax.y()) - origin.y()) * invdir.y();

        if ((t_min > ty_max) || (ty_min > t_max))
            return false;
        if (ty_min > t_min)
            t_min = ty_min;
        if (ty_max < t_max)
            t_max = ty_max;

        float tz_min = ((sign[2] ? pmax.z() : pmin.z()) - origin.z()) * invdir.z();
        float tz_max = ((sign[2] ? pmin.z() : pmax.z()) - origin.z()) * invdir.z();

        if ((t_min > tz_max) || (tz_min > t_max))
            return false;
        if (tz_min > t_min)
            t_min = tz_min;
        if (tz_max < t_max)
            t_max = tz_max;

        // t_min -> t_max is the range of intersection
        // get the one larger than tmin
        float t0 = t_min;
        if (t0 < tmin) {
            t0 = t_max;
            if (t0 < tmin) {
                return false;
            }
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
            // std::cout << "Box intersect error" << std::endl;
            return false;
        }

        if (t0 < h.getT()) {
            // printf("Box intersect\n");
            // printf("t0: %f\n", t0);
            // if (Vector3f::dot(normal, r.getDirection()) > 0) {
                // printf("go out\n");
            // } else {
                // printf(/"go in\n");
            // }
            h.set(t0, material, normal);
            return true;
        }
        return false;
    }

    bool bounding_box(double t0, double t1, AABB &box) override {
        box = *bounding;
        return true;
    }

    bool finite() override { return true; }
    
    Vector3f pmin, pmax;
    AABB* bounding;
};