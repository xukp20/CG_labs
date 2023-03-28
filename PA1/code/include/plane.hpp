#ifndef PLANE_H
#define PLANE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>

// TODO: Implement Plane representing an infinite plane
// function: ax+by+cz=d
// choose your representation , add more fields and fill in the functions

class Plane : public Object3D {
public:
    Plane() {
        _n = Vector3f::UP;
        _d = 0;
    }

    Plane(const Vector3f &normal, float d, Material *m) : Object3D(m) {
        _n = normal;
        _d = d;
    }

    ~Plane() override = default;

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        // 2023/3/26
        // ray origin to plane
        
        // if parallel, no intersection
        float n_dot_rd = Vector3f::dot(_n, r.getDirection());
        if (n_dot_rd < 1e-6) {
            return false;
        }

        float t = - (_d + Vector3f::dot(_n, r.getOrigin())) / n_dot_rd;
        if (t < tmin || t <= 0) {
            return false;
        }

        if (t <= h.getT()) {
            Vector3f normal = Vector3f::dot(_n, r.getDirection()) < 0 ? _n : -_n;
            h.set(t, material, normal);
            return true;
        }
        return false;
    }

protected:
    // 2023/3/26 add n, d
    Vector3f _n;
    float _d;
};

#endif //PLANE_H
		

