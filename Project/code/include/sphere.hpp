#ifndef SPHERE_H
#define SPHERE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>

class Sphere : public Object3D {
public:
    Sphere() {
        // unit ball at the center
        _center = Vector3f::ZERO;
        _radius = 1.0f;
    }

    Sphere(const Vector3f &center, float radius, Material *material) : Object3D(material) {
        // init center and radius
        _center = center;
        _radius = radius;
    }

    ~Sphere() override = default;

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        // 2023/3/26
        // ray origin to sphere center
        Vector3f oc = _center - r.getOrigin();
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
                float u, v;
                Vector3f n = (r.pointAtParameter(t) - _center) / _radius;
                get_uv(n, u, v);
                h.set(t, material, n, u, v);
                return true;
            }
        } else {
            // two intersections, test the near one and then the far one
            float t_hc = sqrt(t_hc_squared);
            float t1 = t_ca - t_hc;
            float t2 = t_ca + t_hc;
            if (t1 >= tmin && t1 <= h.getT()) {
                float u, v;
                Vector3f n = (r.pointAtParameter(t1) - _center) / _radius;
                get_uv(n, u, v);
                h.set(t1, material, n, u, v);
                return true;
            } else if (t2 >= tmin && t2 <= h.getT()) {
                float u, v;
                Vector3f n = (r.pointAtParameter(t2) - _center) / _radius;
                get_uv(n, u, v);
                h.set(t2, material, n, u, v);
                // h.set(t2, material, (r.pointAtParameter(t2) - _center) / _radius);
                return true;
            }
        }
        return false;
    }

    bool bounding_box(double _time0, double _time1, AABB &output_box) override {
        // bounding box of a sphere
        output_box = AABB(_center - Vector3f(_radius, _radius, _radius),
                          _center + Vector3f(_radius, _radius, _radius));
        return true;
    }

    void get_uv(const Vector3f &n, float &u, float &v) {
        // https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/barycentric-coordinates
        float phi = atan2(n.z(), n.x());
        float theta = asin(n.y());

        u = 1 - (phi + M_PI) / (2 * M_PI);
        v = (theta + M_PI / 2) / M_PI;
    }

protected:
    Vector3f _center;
    float _radius;
};


#endif
