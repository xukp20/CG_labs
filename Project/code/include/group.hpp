#ifndef GROUP_H
#define GROUP_H

#include <iostream>
#include <vector>

#include "object3d.hpp"
#include "ray.hpp"
#include "hit.hpp"
#include "bvh.hpp"
#include "moving_sphere.hpp"


class Group : public Object3D {

public:
    bool use_bvh = false;
    std::vector<Object3D*> finite_objects;
    std::vector<Object3D*> infinite_objects;

    Group() {
        group_size = 0;
        objects = std::vector<Object3D*>();
    }

    explicit Group (int num_objects) {
        objects = std::vector<Object3D*>(num_objects);
        group_size = num_objects;
    }

    ~Group() override {

    }

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        bool isIntersect = false;
        // printf("group tmin = %f\n", tmin);
        if (use_bvh) {
            // check all the finite objects
            if (root->intersect(r, h, tmin)) {
                isIntersect = true;
                // printf("intersect with bvh\n");
                // printf("tmin = %f\n", tmin);
            }
            // check all the infinite objects
            for (int i = 0; i < infinite_objects.size(); i++) {
                if (infinite_objects[i]->intersect(r, h, tmin)) {
                    isIntersect = true;
                }
                // printf("intersect with infinite %d\n", i);
                // printf("tmin = %f\n", tmin);
            }
        } else {
            for (int i = 0; i < group_size; i++) {
                if (objects[i] && objects[i]->intersect(r, h, tmin)) {
                    isIntersect = true;
                }
            }
        }
        
        return isIntersect;
    }

    bool bounding_box(double _time0, double _time1, AABB &output_box) override {
        if (group_size < 1) return false;
        AABB temp_box;
        bool first_true = objects[0]->bounding_box(_time0, _time1, temp_box);
        if (!first_true) return false;
        else output_box = temp_box;
        for (int i = 1; i < group_size; i++) {
            if (objects[i]->bounding_box(_time0, _time1, temp_box)) {
                output_box = AABB::surrounding_box(output_box, temp_box);
            } else {
                return false;
            }
        }
        return true;
    }

    void addObject(int index, Object3D *obj) {
        objects[index] = obj;

        if (index == group_size - 1) {
            // final
            if (use_bvh) {
                double t0=0, t1=0;

                // get time interval
                for (int i = 0; i < group_size; i++) {
                    if (dynamic_cast<MovingSphere*>(objects[i])) {
                        MovingSphere *ms = dynamic_cast<MovingSphere*>(objects[i]);
                        t0 = fmin(t0, ms->t0);
                        t1 = fmax(t1, ms->t1);
                    }
                }

                // get all the finite objects and infinite objects
                AABB temp_box;
                // printf("group size %d\n", group_size);
                finite_objects = std::vector<Object3D*>();
                infinite_objects = std::vector<Object3D*>();
                for (int i = 0; i < group_size; i++) {
                    // printf("object %d\n", i);
                    if (objects[i]->finite()) {
                        // printf("finite object %d\n", i);
                        fflush(stdout);
                        finite_objects.push_back(objects[i]);
                    } else {
                        // like plane and triangle
                        // printf("infinite object %d\n", i);
                        fflush(stdout);
                        infinite_objects.push_back(objects[i]);
                    }
                }

                root = new BVHNode(finite_objects, 0, finite_objects.size(), t0, t1);
            }
        }
    }

    int getGroupSize() {
        return group_size;
    }

    bool finite() override {
        for (int i = 0; i < group_size; i++) {
            if (!objects[i]->finite()) {
                return false;
            }
        }
        return true;
    }

private:
    int group_size;
    std::vector<Object3D*> objects;
    BVHNode *root;
};

#endif
	
