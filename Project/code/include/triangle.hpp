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
		has_normal = false;
	}

	void setNormals(const Vector3f& na, const Vector3f& nb, const Vector3f& nc) {
		normals[0] = na;
		normals[1] = nb;
		normals[2] = nc;
		has_normal = true;
	}

	bool intersect( const Ray& ray,  Hit& hit , float tmin) override {
        // 2023/3/26
		// printf("triangle intersect\n");
		Vector3f e1 = vertices[0] - vertices[1];
		Vector3f e2 = vertices[0] - vertices[2];
		Vector3f s = vertices[0] - ray.getOrigin();

		Vector3f result = Vector3f::ZERO;
		float under = det(ray.getDirection(), e1, e2);
		if (fabs(under) < 1e-6) {
			return false;
		}
		result[0] = det(s, e1, e2) / under;
		result[1] = det(ray.getDirection(), s, e2) / under;
		result[2] = det(ray.getDirection(), e1, s) / under;

		if (result[0] < tmin || result[0] > hit.getT() || result[1] < 0 || result[2] < 0 || result[1] + result[2] > 1) {
			return false;
		} else {
			// if normal interpolation is needed
			if (has_normal) {
				// use the gravity center of the tiangle to interpolate, area weighted
				Vector3f pos = ray.pointAtParameter(result[0]);
				Vector3f to0 = vertices[0] - pos;
				Vector3f to1 = vertices[1] - pos;
				Vector3f to2 = vertices[2] - pos;
				float a0 = Vector3f::cross(to1, to2).length();
				float a1 = Vector3f::cross(to2, to0).length();
				float a2 = Vector3f::cross(to0, to1).length();
				float sum = a0 + a1 + a2;
				Vector3f n = (a0 * normals[0] + a1 * normals[1] + a2 * normals[2]) / sum;
				n.normalize();
				hit.set(result[0], material, n);
			} else {
				hit.set(result[0], material, normal);
			}
			return true;
		}
	}

	bool bounding_box(double time0, double time1, AABB& output_box) override {
		Vector3f min = vertices[0];
		Vector3f max = vertices[0];
		for (int i = 1; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				min[j] = std::min(min[j], vertices[i][j]);
				max[j] = std::max(max[j], vertices[i][j]);
			}
		}
		output_box = AABB(min, max);
		return true;
	}

	bool finite() override {
		return true;
	}

	// new tool function, to compute the det of three vectors
	float det(const Vector3f& a, const Vector3f& b, const Vector3f& c) {
		// det equals to the dot product of a and the cross product of b and c -> mixed product
		return Vector3f::dot(a, Vector3f::cross(b, c));
	}

	Vector3f normal;
	Vector3f vertices[3];

	// add norm interpolation
	Vector3f normals[3];
	bool has_normal;
protected:

};

#endif //TRIANGLE_H
