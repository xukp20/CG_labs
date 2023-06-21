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
            // the curve is on xy plane, and rotate around y axis
            // get the max distance from y axis, and record the mix and min of y
            float max_dist = 0;
            float min_y = FLT_MAX, max_y = -FLT_MAX;
            // go through the control points
            for (const auto &cp : pCurve->getControls()) {
                min_y = fmin(min_y, cp.y());
                max_y = fmax(max_y, cp.y());
                max_dist = fmax(max_dist, fabs(cp.x()));
            }
            // get the bounding box
            box = new AABB(Vector3f(-max_dist, min_y - 0.01, -max_dist), Vector3f(max_dist, max_y + 0.01, max_dist));
        }
        output_box = *box;
        return true;
    }

    bool set_bounding_box()  {
        if (box == nullptr) {
            // the curve is on xy plane, and rotate around y axis
            // get the max distance from y axis, and record the mix and min of y
            float max_dist = 0;
            float y_min = FLT_MAX, y_max = -FLT_MAX;
            // go through the control points
            for (const auto &cp : pCurve->getControls()) {
                y_min = fmin(y_min, cp.y());
                y_max = fmax(y_max, cp.y());
                max_dist = fmax(max_dist, fabs(cp.x()));
            }
            // get the bounding box
            box = new AABB(Vector3f(-max_dist, y_min - 0.01, -max_dist), Vector3f(max_dist, y_max + 0.01, max_dist));
        }
        return true;
    }

    ~RevSurface() override {
        delete pCurve;
    }

    // use newton's method to find the intersection
    bool intersect(const Ray &r, Hit &h, float tmin) override {
        // first check the bounding box
        float t0;
        if (!box->intersect(r, t0)) {
            return false;
        }
        // if t0 smaller than tmin, set t0 to tmin, check if tmin is inside the box
        if (t0 < tmin) {
            t0 = tmin;
            if (!box->inside(r.pointAtParameter(t0))) {
                return false;
            }
        }
        if (t0 > h.getT()) {
            // further than the previous hit point
            return false;
        }

        // start from the hit point of the bounding box
        float t = t0;
        bool isHit = newtonIterate(r, h, tmin, t);
    }

    bool newtonIterate(const Ray &r, Hit &h, float tmin, float &t) {
        // start from the hit point of the bounding box
        // if hit, set t and h
        // if the final t is smaller than tmin, return false

        // use t to describe ray, theta and phi to describe the point on the curve
        // theta for the angle around y axis, phi for the ratio in y (equal to use y as the parameter, which is param t in curve)
        float theta, phi;
        float ti = t;
        Vector3f p = r.pointAtParameter(ti);
        theta = atan2(-p.z(), p.x()) + M_PI;
        phi = (y_max - p.y()) / (y_max - y_min);

        // use newton's method to find the intersection
        Vector3f dphi, dtheta;
        for (int i=0; i < MAX_ITER; i++) {
            // get the point on the curve
            CurvePoint cp = pCurve->getPoint(phi);
            // to rotate the point and normal according to theta
            // Quat4f rot; rot.setAxisAngle(theta, Vector3f(0, 1, 0));
            Matrix3f rot_mat = Matrix3f::rotation(Vector3f(0, 1, 0), theta);
            // rotate the point and normal
            p = rot_mat * cp.V;
            dphi = rot_mat * cp.T;
            dtheta = Vector3f(-cp.V.x() * sin(theta), 0, -cp.V.x() * cos(theta));
            // normal is vertical to the tangent plane
            Vector3f n = Vector3f::cross(dphi, dtheta);
            
            // note that different from P107, S(u,v) - C(t), so the final update are reversed
            Vector3f dis = p - r.pointAtParameter(ti);
            float f = dis.squaredLength();

            // distance smaller than epsilon, return true
            if (f < EPSILON) {
                // newton's method converge
                // TODO check normal
                if (ti < tmin || ti > h.getT() || phi < pCurve->lowerBound() || phi > pCurve->upperBound()) {
                    // the final t is smaller than tmin, return false
                    // or further than the previous hit point
                    return false;
                }
                // set t and h
                t = ti;
                h.set(t, material, n.normalized(), theta/2/M_PI, phi);
            }

            // P107 method, get D
            float D = Vector3f::dot(r.getDirection(), n);
            // update t, theta, phi
            ti -= Vector3f::dot(dphi, Vector3f::cross(dtheta, dis)) / D;
            phi -= Vector3f::dot(r.getDirection(), Vector3f::cross(dtheta, dis)) / D;
            theta += Vector3f::dot(r.getDirection(), Vector3f::cross(dphi, dis)) / D;
        }
        return false;   // newton's method not converge
    }

    const static int MAX_ITER = 20;
    const static float EPSILON = 0.0001;
};

#endif //REVSURFACE_HPP
