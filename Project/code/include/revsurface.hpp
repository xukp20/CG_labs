#ifndef REVSURFACE_HPP
#define REVSURFACE_HPP

#include <tuple>
#include <float.h>

#include "object3d.hpp"
#include "curve.hpp"
#include "triangle.hpp"
#include "bounding.hpp"


class RevSurface : public Object3D {

    Curve *pCurve;
    AABB *box;

    float y_max, y_min;

    int intersect_time = 0;
    int box_intersect_time = 0;

public:
    RevSurface(Curve *pCurve, Material* material) : pCurve(pCurve), Object3D(material) {
        // Check flat.
        // for (const auto &cp : pCurve->getControls()) {
        //     if (cp.z() != 0.0) {
        //         printf("Profile of revSurface must be flat on xy plane.\n");
        //         exit(0);
        //     }
        // }
        // skip checking for faster
        box = nullptr;
        set_bounding_box();
    }

    bool bounding_box(double time0, double time1, AABB &output_box) override {
        if (box == nullptr) {
            set_bounding_box();
        }
        output_box = *box;
        return true;
    }

    bool set_bounding_box()  {
        if (box == nullptr) {
            // printf("set bounding box\n");
            // the curve is on xy plane, and rotate around y axis
            // get the max distance from y axis, and record the mix and min of y
            float max_dist = 0;
            y_min = 1e10, y_max = -1e10;
            // go through the control points
            for (const auto &cp : pCurve->getControls()) {
                y_min = fmin(y_min, cp.y());
                y_max = fmax(y_max, cp.y());
                max_dist = fmax(max_dist, fabs(cp.x()));
            }
            // get the bounding box
            box = new AABB(Vector3f(-max_dist, y_min - 3, -max_dist), Vector3f(max_dist, y_max + 3, max_dist));
            // printf("bbox y_min: %f, y_max: %f\n", y_min, y_max);
            // printf("bbox min: %f %f %f\n", box->min.x(), box->min.y(), box->min.z());
            // printf("bbox max: %f %f %f\n", box->max.x(), box->max.y(), box->max.z());
        }
        return true;
    }

    ~RevSurface() override {
        delete pCurve;
    }

    // use newton's method to find the intersection
    bool intersect(const Ray &r, Hit &h, float tmin) override {
        return newtonIterate(r, h, tmin);
    }

    bool newtonIterate(const Ray &r, Hit &h, float tmin) {
        // start from the hit point of the bounding box
        // if hit, set t and h
        // if the final t is smaller than tmin, return false

        // use t to describe ray, theta and phi to describe the point on the curve
        // theta for the angle around y axis, phi for the ratio in y (equal to use y as the parameter, which is param t in curve)
        float theta, phi, t=1e10;
        if (!box->intersect(r, t)) {
            return false;
        }
        if (t < tmin || t > h.getT()) {
            box_intersect_time++;
            return false;
        }
        Vector3f p = r.pointAtParameter(t);
        theta = atan2(-p.z(), p.x()) + M_PI;
        phi = (y_max - p.y()) / (y_max - y_min);
        // printf("init theta: %f, phi: %f\n", theta, phi);

        // use newton's method to find the intersection
        Vector3f dphi, dtheta;
        for (int i=0; i < MAX_ITER; i++) {
            phi = phi <= 0 ? 1e-5 : phi;
            phi = phi >= 1 ? 1 - 1e-5 : phi;
            // printf("phi: %f\n", phi);
            theta = theta < 0 ? theta + 2 * M_PI : theta;
            theta = theta >= 2 * M_PI ? fmod(theta, 2 * M_PI) : theta;
            // get the point on the curve
            // printf("get curve point\n");
            CurvePoint cp = pCurve->getPoint(phi);
            // printf("got curve point\n");
            // to rotate the point and normal according to theta
            Quat4f rot; rot.setAxisAngle(theta, Vector3f(0, 1, 0));
            // Matrix3f rot_mat = Matrix3f::rotation(Vector3f(0, 1, 0), theta);
            Matrix3f rot_mat = Matrix3f::rotation(rot);
            // rotate the point and normal
            p = rot_mat * cp.V;
            dphi = rot_mat * cp.T;
            dtheta = Vector3f(-cp.V.x() * sin(theta), 0, -cp.V.x() * cos(theta));
            // normal is vertical to the tangent plane
            Vector3f n = Vector3f::cross(dphi, dtheta);
            
            // note that different from P107, S(u,v) - C(t), so the final update are reversed
            Vector3f dis = r.pointAtParameter(t) - p;
            float f = dis.squaredLength();
            // printf("f: %f\n", f);

            // distance smaller than epsilon, return true
            if (f < EPSILON) {
                // printf("revsurface intersect true\n");
                // newton's method converge
                // TODO check normal
                if (t < tmin || t > h.getT() || phi < pCurve->lowerBound() || phi > pCurve->upperBound()) {
                    // the final t is smaller than tmin, return false
                    // or further than the previous hit point
                    // printf("revsurface intersect false\n");
                    return false;
                }
                // set  h
                h.set(t, material, n.normalized(), theta/2/M_PI, phi);
                return true;
            }

            // P107 method, get D
            float D = Vector3f::dot(r.getDirection(), n);
            // update t, theta, phi
            t -= Vector3f::dot(dphi, Vector3f::cross(dtheta, dis)) / D;
            phi -= Vector3f::dot(r.getDirection(), Vector3f::cross(dtheta, dis)) / D;
            theta += Vector3f::dot(r.getDirection(), Vector3f::cross(dphi, dis)) / D;
        }
        return false;   // newton's method not converge
    }

    constexpr static int MAX_ITER = 20;
    constexpr static float EPSILON = 0.0001;
};

#endif //REVSURFACE_HPP
