#ifndef RAY_H
#define RAY_H

#include <cassert>
#include <iostream>
#include <Vector3f.h>


// Ray class mostly copied from Peter Shirley and Keith Morley
class Ray {
public:

    Ray() = delete;
    Ray(const Vector3f &orig, const Vector3f &dir, double _time = 0.0) {
        origin = orig;
        direction = dir;
        time = _time;
    }

    Ray(const Ray &r) {
        origin = r.origin;
        direction = r.direction;
        time = r.time;
    }

    const Vector3f &getOrigin() const {
        return origin;
    }

    const Vector3f &getDirection() const {
        return direction;
    }

    const double &getTime() const {
        return time;
    }

    Vector3f pointAtParameter(float t) const {
        return origin + direction * t;
    }
    
    double time;    // for motion blur

public:
    Vector3f origin;
    Vector3f direction;
};

inline std::ostream &operator<<(std::ostream &os, const Ray &r) {
    os << "Ray <" << r.getOrigin() << ", " << r.getDirection() << ">";
    return os;
}

#endif // RAY_H
