#ifndef GROUP_H
#define GROUP_H


#include "object3d.hpp"
#include "ray.hpp"
#include "hit.hpp"
#include <iostream>
#include <vector>


// TODO: Implement Group - add data structure to store a list of Object*
class Group : public Object3D {

public:

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
        for (int i = 0; i < group_size; i++) {
            if (objects[i] && objects[i]->intersect(r, h, tmin)) {
                isIntersect = true;
                // printf("intersect with %d\n", i);
            }
        }
        return isIntersect;
    }

    void addObject(int index, Object3D *obj) {
        objects[index] = obj;
    }

    int getGroupSize() {
        return group_size;
    }

private:
    int group_size;
    std::vector<Object3D*> objects;
};

#endif
	
