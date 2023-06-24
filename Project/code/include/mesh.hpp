#ifndef MESH_H
#define MESH_H

#include <vector>

#include "object3d.hpp"
#include "triangle.hpp"
#include "Vector2f.h"
#include "Vector3f.h"
#include "bounding.hpp"


class Mesh : public Object3D {

public:
    Mesh(const char *filename, Material *m, bool use_inter=false);

    struct TriangleIndex {
        TriangleIndex() {
            x[0] = 0; x[1] = 0; x[2] = 0;
        }
        int &operator[](const int i) { return x[i]; }
        // By Computer Graphics convention, counterclockwise winding is front face
        int x[3]{};
    };

    std::vector<Vector3f> v;
    std::vector<Vector3f> vn;
    std::vector<TriangleIndex> t;
    std::vector<Vector3f> n;
    bool use_inter;

    bool intersect(const Ray &r, Hit &h, float tmin) override;
    bool bounding_box(double time0, double time1, AABB &output_box) { 
        // the bounding box is outside the all triangles
        if (box == nullptr) {
            float min_x = FLT_MAX, min_y = FLT_MAX, min_z = FLT_MAX;
            float max_x = -1e10, max_y = -1e10, max_z = -1e10;

            for (int i = 0; i < v.size(); i++) {
                min_x = std::min(min_x, v[i].x());
                min_y = std::min(min_y, v[i].y());
                min_z = std::min(min_z, v[i].z());
                max_x = std::max(max_x, v[i].x());
                max_y = std::max(max_y, v[i].y());
                max_z = std::max(max_z, v[i].z());
            }
            box = new AABB(Vector3f(min_x, min_y, min_z), Vector3f(max_x, max_y, max_z));
        }
        output_box = *box;
        return true;  
    }

    bool finite() override { return true; }
private:
    AABB* box = nullptr;

    // Normal can be used for light estimation
    void computeNormal();
};

#endif
