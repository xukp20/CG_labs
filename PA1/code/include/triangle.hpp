#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>
#include <iostream>
using namespace std;

// TODO: implement this class and add more fields as necessary,
class Triangle: public Object3D {

public:
	Triangle() = delete;

    // a b c are three vertex positions of the triangle
	Triangle( const Vector3f& a, const Vector3f& b, const Vector3f& c, Material* m) : Object3D(m) {
		vertices[0] = a;
		vertices[1] = b;
		vertices[2] = c;
		normal = Vector3f::cross(b - a, c - a);
		normal.normalize();
	}

	bool intersect( const Ray& ray,  Hit& hit , float tmin) override {
        // 2023/3/26
		Vector3f e1 = vertices[0] - vertices[1];
		Vector3f e2 = vertices[0] - vertices[2];
		Vector3f s = vertices[0] - ray.getOrigin();

		Vector3f result = Vector3f::ZERO;
		float under = det(ray.getDirection(), e1, e2);
		if (under < 1e-6) {
			return false;
		}
		result[0] = det(s, e1, e2) / under;
		result[1] = det(ray.getDirection(), s, e2) / under;
		result[2] = det(ray.getDirection(), e1, s) / under;

		if (result[0] < tmin || result[0] > hit.getT() || result[1] < 0 || result[2] < 0 || result[1] + result[2] > 1) {
			return false;
		} else {
			// the direction is opposite to the ray direction?
			Vector3f n = Vector3f::dot(normal, ray.getDirection()) < 0 ? normal : -normal;
			hit.set(result[0], material, n);
			return true;
		}
	}

	// new tool function, to compute the det of three vectors
	float det(const Vector3f& a, const Vector3f& b, const Vector3f& c) {
		// det equals to the dot product of a and the cross product of b and c -> mixed product
		return Vector3f::dot(a, Vector3f::cross(b, c));
	}

	Vector3f normal;
	Vector3f vertices[3];
protected:

};

#endif //TRIANGLE_H
