#pragma once

#include <vector>
#include <string>

#include "object3d.hpp"
#include "ray.hpp"
#include "hit.hpp"
#include "rand.hpp"

class Media : public Object3D {
    Object3D *obj;
    float inv_density;

public:
    Media() = delete;
    Media(Object3D *obj, float d) : obj(obj), inv_density(1/d) {}

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        Hit h1, h2;
        // only consider going in light (ray)
        if (obj->intersect(r, h1, 0)) {
            // printf("Media is intersect\n");

            // printf("test 2");
            if (!obj->intersect(r, h2, h1.getT() + 1e-5)) {
                // printf("Media not intersect\n");
                return false;
            }

            // printf("Media intersect\n");
            // get the going in light and going out light
            if (h1.getT() < tmin) {
                h1.t = tmin;
            } 
            if (h2.getT() > h.getT()) {
                h2.t = h.getT();
            }
            // no range for intersection
            if (h1.getT() >= h2.getT()) {
                return false;
            }

            // compute 
            float len = r.getDirection().length();
            float dis_inside_boundary = (h2.getT() - h1.getT()) * len;
            float hit_distance = - inv_density * log(RAND_UNIFORM);
            // printf("hit_distance = %f\n", hit_distance);
            // printf("dis_inside_boundary = %f\n", dis_inside_boundary);
            if (hit_distance < dis_inside_boundary) {
                // generate random direction, for a near zero norm
                Vector3f norm = Vector3f(SMALL_POSI, 0, 0);
                // no texture for media
                h.set(h1.getT() + hit_distance / len, h1.getMaterial(), norm);
                return true;
            } else {
                return false;
            }
        } else {
            // printf("Media not intersect\n");
            return false;
        }
    }

    bool finite() override { return obj->finite(); }
    bool bounding_box(double t0, double t1, AABB &box) override { return obj->bounding_box(t0, t1, box); }  
};