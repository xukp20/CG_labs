#ifndef MOVING_SPHERE_H
#define MOVING_SPHERE_H

#include "sphere.hpp"
#include <vecmath.h>
#include <cmath>

class MovingSphere : public Sphere {
public:
    MovingSphere(): Sphere() {
        // init center and radius
        _center = Vector3f(0, 0, 0);
        _radius = 0;
    }

    MovingSphere(const Vector3f &center, float radius, Material *material,
        const Vector3f &center2, double t0, double t1) : Sphere(center, radius, material) {
        this->center2 = center2;
        this->t0 = t0;
        this->t1 = t1;  
    }

    ~MovingSphere() override = default;

    Vector3f center(double time) const {
        return _center + ((time - t0) / (t1 - t0)) * (center2 - _center);
    }

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        double time = r.getTime();
        Vector3f center = this->center(time);

        // ray origin to sphere center
        Vector3f oc = center - r.getOrigin();
        // distance between ray origin and sphere center
        float oc_squared_len = oc.squaredLength();

        // if in the sphere
        bool in_sphere = oc_squared_len < _radius * _radius;

        float t_ca = Vector3f::dot(oc, r.getDirection().normalized());

        // if center of sphere is behind the ray
        if (t_ca < 0 && !in_sphere) {
            return false;
        }

        float t_hc_squared = _radius * _radius - oc_squared_len + t_ca * t_ca;

        // if t_hc^2 < 0, for D > R, there is no intersection
        if (t_hc_squared < 0) {
            return false;
        } else if (t_hc_squared == 0) {
            // if t_hc^2 = 0, for D = R, there is one intersection
            float t = t_ca;
            if (t >= tmin && t <= h.getT()) {
                h.set(t, material, (r.pointAtParameter(t) - center) / _radius);
                return true;
            }
        } else {
            // two intersections, test the near one and then the far one
            float t_hc = sqrt(t_hc_squared);
            float t1 = t_ca - t_hc;
            float t2 = t_ca + t_hc;
            if (t1 >= tmin && t1 <= h.getT()) {
                h.set(t1, material, (r.pointAtParameter(t1) - center) / _radius);
                return true;
            } else if (t2 >= tmin && t2 <= h.getT()) {
                h.set(t2, material, (r.pointAtParameter(t2) - center) / _radius);
                return true;
            }
        }
        return false;
    }

    bool bounding_box(double time0, double time1, AABB &output_box) override {
        AABB box0(center(time0) - Vector3f(_radius, _radius, _radius),
            center(time0) + Vector3f(_radius, _radius, _radius));
        AABB box1(center(time1) - Vector3f(_radius, _radius, _radius),
            center(time1) + Vector3f(_radius, _radius, _radius));
        output_box = AABB::surrounding_box(box0, box1);
        return true;
    }

protected:
    Vector3f center2;
    double t0, t1;
};


#endif
