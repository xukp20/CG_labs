#ifndef MATERIAL_H
#define MATERIAL_H

#include <cassert>
#include <vecmath.h>

#include "ray.hpp"
#include "hit.hpp"
#include <iostream>

inline float rand_thres() {
    // 0 ~ 1
    return (float)rand() / RAND_MAX;
}

class MeterialRatio : public Vector3f {
public:
    MeterialRatio(Vector3f ratio) : Vector3f(ratio / (ratio.x() + ratio.y() + ratio.z())) {
    }

    float getDiffuseThres() {
        return x();
    }

    float getSpecularThres() {
        return x() + y();
    }
};

class Material {
public:

    explicit Material(const Vector3f &d_color, const Vector3f &s_color = Vector3f::ZERO, float s = 0,
                      const Vector3f &l_color = Vector3f::ZERO,
                      const float &n = 1.0f,
                      const Vector3f &ratio = Vector3f::ZERO
                      ) :
            diffuseColor(d_color), specularColor(s_color), shininess(s),
            selfColor(l_color), 
            n(n),
            ratio(ratio)
            {
            }

    virtual ~Material() = default;

    virtual Vector3f getDiffuseColor() const {
        return diffuseColor;
    }


    Vector3f Shade(const Ray &ray, const Hit &hit,
                   const Vector3f &dirToLight, const Vector3f &lightColor) {
        Vector3f shaded = Vector3f::ZERO;
        Vector3f normal = hit.getNormal();
        Vector3f dir = ray.getDirection();
        Vector3f L = dirToLight.normalized();   
        Vector3f V = -dir.normalized();
        Vector3f R = (2 * Vector3f::dot(L, normal) * normal - L).normalized();
        shaded += lightColor * diffuseColor * (Vector3f::dot(L, normal) > 0 ? Vector3f::dot(L, normal) : 0);
        shaded += lightColor * specularColor * pow(Vector3f::dot(R, V) > 0 ? Vector3f::dot(R, V) : 0, shininess);
        
        return shaded;
    }

public:

    Vector3f diffuseColor;  // for diffuse shading
    Vector3f specularColor; // for specular shading
    Vector3f selfColor;     // for self emission
    float n;                // for refraction index
    float shininess;
    MeterialRatio ratio;         // for ratio of diffuse / specular / self emission

    // utils
    static Vector3f reflect(const Vector3f &I, const Vector3f &N) {
        return I - 2 * Vector3f::dot(I, N) * N;
    }

    Vector3f random_unit_vector() {
        while (true) {
            Vector3f p = Vector3f(2*rand_thres() - 1, 2*rand_thres() - 1, 2*rand_thres() - 1);
            if (p.squaredLength() >= 1) continue;
            return p.normalized();
        }
    }

    bool scatter(const Ray &ray, const Hit &hit, Vector3f &attenuation, Ray &scattered, float old_n) {
        // get a rand threshold to determine the type of scattering
        float rand = rand_thres();
        if (rand < ratio.getDiffuseThres()) {
            // diffuse
            Vector3f target = hit.getNormal() + random_unit_vector();
            scattered = Ray(ray.pointAtParameter(hit.getT()), target.normalized());
            attenuation = diffuseColor;
            return true;
        } else if (rand < ratio.getSpecularThres()) {
            // specular
            Vector3f reflected = reflect(ray.getDirection().normalized(), hit.getNormal());
            scattered = Ray(ray.pointAtParameter(hit.getT()), reflected);
            attenuation = specularColor;
            return Vector3f::dot(scattered.getDirection(), hit.getNormal()) > 0;
        } else {
            // refract
            attenuation = Vector3f(1, 1, 1);

            Vector3f N = hit.getNormal();
            Vector3f V = ray.getDirection().normalized();
            float cos_theta = Vector3f::dot(-V, N);
            float sin_theta = sqrt(1 - cos_theta * cos_theta);
            float n_ratio = old_n / n;
            // check if total internal reflection
            if (n_ratio * sin_theta > 1) {
                Vector3f reflected = reflect(V, N);
                scattered = Ray(ray.pointAtParameter(hit.getT()), reflected);
                return true;
            }

            // refract
            float cos_phi = sqrt(1 - n_ratio * n_ratio * (1 - cos_theta * cos_theta));
            Vector3f refracted = n_ratio * (V + cos_theta * N) - cos_phi * N;
            scattered = Ray(ray.pointAtParameter(hit.getT()), refracted);
            return true;
        }
    }
};


#endif // MATERIAL_H
