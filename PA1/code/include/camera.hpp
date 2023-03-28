#ifndef CAMERA_H
#define CAMERA_H

#include "ray.hpp"
#include <vecmath.h>
#include <float.h>
#include <cmath>


class Camera {
public:
    Camera(const Vector3f &center, const Vector3f &direction, const Vector3f &up, int imgW, int imgH) {
        this->center = center;
        this->direction = direction.normalized();
        this->horizontal = Vector3f::cross(this->direction, up).normalized();
        this->up = Vector3f::cross(this->horizontal, this->direction);
        this->width = imgW;
        this->height = imgH;
    }

    // Generate rays for each screen-space coordinate
    virtual Ray generateRay(const Vector2f &point) = 0;
    virtual ~Camera() = default;

    int getWidth() const { return width; }
    int getHeight() const { return height; }

protected:
    // Extrinsic parameters
    Vector3f center;
    Vector3f direction;
    Vector3f up;
    Vector3f horizontal;
    // Intrinsic parameters
    int width;
    int height;
};

// TODO: Implement Perspective camera
// You can add new functions or variables whenever needed.
class PerspectiveCamera : public Camera {

public:
    PerspectiveCamera(const Vector3f &center, const Vector3f &direction,
            const Vector3f &up, int imgW, int imgH, float angle) : Camera(center, direction, up, imgW, imgH) {
        // angle is in radian.
        fx = width / (2 * tan(angle / 2));
        fy = height / (2 * tan(angle / 2));
        cx = width / 2;
        cy = height / 2;
        R = Matrix3f(horizontal, -up, direction, true);
    }

    Ray generateRay(const Vector2f &point) override {
        // O_rw
        Vector3f O_rw = center;
        // d_rc
        Vector3f d_rc = Vector3f((point.x() - cx) / fx, (cy - point.y()) / fy, 1).normalized();

        // d_rw
        Vector3f d_rw = R * d_rc;

        return Ray(O_rw, d_rw);
    }

private:
    float fx;
    float fy;
    float cx, cy;
    Matrix3f R;
};

#endif //CAMERA_H
