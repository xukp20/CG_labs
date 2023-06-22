#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <vecmath.h>
#include "object3d.hpp"

// transforms a 3D point using a matrix, returning a 3D point
static Vector3f transformPoint(const Matrix4f &mat, const Vector3f &point) {
    return (mat * Vector4f(point, 1)).xyz();
}

// transform a 3D directino using a matrix, returning a direction
static Vector3f transformDirection(const Matrix4f &mat, const Vector3f &dir) {
    return (mat * Vector4f(dir, 0)).xyz();
}

// TODO: implement this class so that the intersect function first transforms the ray
class Transform : public Object3D {
public:
    Transform() {}

    Transform(const Matrix4f &m, Object3D *obj) : o(obj) {
        transform = m.inverse();
    }

    ~Transform() {
    }

    virtual bool intersect(const Ray &r, Hit &h, float tmin) {
        Vector3f trSource = transformPoint(transform, r.getOrigin());
        Vector3f trDirection = transformDirection(transform, r.getDirection());
        Ray tr(trSource, trDirection);
        bool inter = o->intersect(tr, h, tmin);
        if (inter) {
            h.set(h.getT(), h.getMaterial(), transformDirection(transform.transposed(), h.getNormal()).normalized(), h.getU(), h.getV());
        }
        return inter;
    }

    bool bounding_box(double time0, double time1, AABB &output_box) {
        if (box == nullptr) {
            if (o->bounding_box(time0, time1, output_box)) {
                Vector3f min = output_box.min;
                Vector3f max = output_box.max;
                Vector3f p[8] = {
                    Vector3f(min.x(), min.y(), min.z()),
                    Vector3f(min.x(), min.y(), max.z()),
                    Vector3f(min.x(), max.y(), min.z()),
                    Vector3f(min.x(), max.y(), max.z()),
                    Vector3f(max.x(), min.y(), min.z()),
                    Vector3f(max.x(), min.y(), max.z()),
                    Vector3f(max.x(), max.y(), min.z()),
                    Vector3f(max.x(), max.y(), max.z())
                };
                for (int i = 0; i < 8; i++) {
                    // use transform or reverse transform?
                    p[i] = transformPoint(transform, p[i]);
                }
                Vector3f newMin = p[0];
                Vector3f newMax = p[0];
                for(int j = 1; j < 8; j++) {
                    for (int k = 0; k < 3; k++) {
                        newMin[k] = std::min(newMin[k], p[j][k]);
                        newMax[k] = std::max(newMax[k], p[j][k]);
                    } 
                }
                box = new AABB(newMin, newMax);
                output_box = AABB(newMin, newMax);
                return true;
            }
            return false;
        }
        output_box = *box;
        return true;
    }

    bool finite() override {
        return o->finite();
    }

protected:
    Object3D *o; //un-transformed object
    Matrix4f transform;
    AABB* box = nullptr;
};

#endif //TRANSFORM_H
