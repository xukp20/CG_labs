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
    virtual Ray generateBlurRay(const Vector2f &point) = 0;
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
            const Vector3f &up, int imgW, int imgH, float angle,
            float f = 20, float aperture = 0.02f,
            double time0 = 0.0, double time1 = 1.0
            ) : Camera(center, direction, up, imgW, imgH) {
        // angle is in radian.
        fx = imgW / (2 * tan(angle / 2));
        fy = imgH / (2 * tan(angle / 2));
        cx = imgW / 2;
        cy = imgH / 2;

        lensRadius = aperture / 2;
        this->f = f;

        this->time0 = time0;
        this->time1 = time1;
    }

    // don't take focal and aperture into account
    Ray generateRay(const Vector2f &point) override {
        // d_rc
        Vector3f d_rc = Vector3f((point.x() - cx) / fx, (cy - point.y()) / fy, 1).normalized();

        Matrix3f R = Matrix3f(horizontal, -up, direction);
        // d_rw
        Vector3f d_rw = R * d_rc;

        return Ray(center, d_rw, time0 + rand_thres() * (time1 - time0));
    }

    Ray generateBlurRay(const Vector2f &point) {
        // d_rc
        Vector3f d_rc = Vector3f((point.x() - cx) / fx, (cy - point.y()) / fy, 1).normalized();

        Matrix3f R = Matrix3f(horizontal, -up, direction);
        // d_rw
        Vector3f d_rw = R * d_rc;

        // lens
        Vector3f lens = lensRadius * Vector3f(rand_thres() - 0.5, rand_thres() - 0.5, 0);

        return Ray(center + lens, d_rw - lens, time0 + rand_thres() * (time1 - time0));
    }

private:
    float fx;
    float fy;
    float cx, cy;

    float lensRadius;
    float f;

    double time0, time1; // for motion blur

    float rand_thres() {
        // 0 ~ 1
        return (float)rand() / RAND_MAX;
    }
};

#endif //CAMERA_H
