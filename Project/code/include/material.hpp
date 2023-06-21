#ifndef MATERIAL_H
#define MATERIAL_H

#include <cassert>
#include <vecmath.h>
#include <iostream>

#include "ray.hpp"
#include "hit.hpp"
#include "texture.hpp"
#include "rand.hpp"

inline float rand_thres() {
    // 0 ~ 1
    return RAND_UNIFORM;
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
                      const Vector3f &ratio = Vector3f::ZERO,
                      const char* texturePath = nullptr
                      ) :
            diffuseColor(d_color), specularColor(s_color), shininess(s),
            selfColor(l_color), 
            n(n),
            ratio(ratio)
            {
                std::string textureStr = texturePath;
                if (!textureStr.empty()) {
                    // if "checkerboard_<color1>_<color2>_<ratio>" <color1> = <>&<>&<>
                    if (textureStr.find("checkerboard") != std::string::npos) {
                        Vector3f c1 = Vector3f::ZERO;
                        Vector3f c2 = Vector3f::ZERO;
                        float ratio = 0;
                        sscanf(textureStr.c_str(), "checkerboard_%f&%f&%f_%f&%f&%f_%f", &c1.x(), &c1.y(), &c1.z(), &c2.x(), &c2.y(), &c2.z(), &ratio);
                        texture = new CheckerBoardTexture(c1, c2, ratio);

                    // if "perlin_<bool>_<ratio>"
                    } else if (textureStr.find("perlin") != std::string::npos) {
                        std::string boolStr = textureStr.substr(textureStr.find("_") + 1, textureStr.find("_", textureStr.find("_") + 1) - textureStr.find("_") - 1);
                        bool isColor = boolStr == "color";
                        float ratio = 0;
                        std::string ratioStr = textureStr.substr(textureStr.rfind("_") + 1);
                        sscanf(ratioStr.c_str(), "%f", &ratio);
                        texture = new NoiseTexture(isColor, ratio);
                    } else {
                        texture = new ImageTexture(textureStr);
                    }
                } else {
                    texture = nullptr;
                }
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
    Texture *texture;

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

    bool scatter(const Ray &ray, const Hit &hit, Vector3f &attenuation, Ray &scattered, bool front=true) {
        // get a rand threshold to determine the type of scattering
        float rand = rand_thres();
        if (rand < ratio.getDiffuseThres()) {
            Vector3f target = hit.getNormal() + random_unit_vector();
            scattered = Ray(ray.pointAtParameter(hit.getT()), target.normalized(), ray.time);
            attenuation = diffuseColor;

            // If there is a texture, use the texture color
            if (texture != nullptr) {
                // printf("texture\n");
                // fflush(stdout);
                Vector3f textureColor = texture->getColor(hit.getU(), hit.getV(), ray.pointAtParameter(hit.getT()));
                attenuation = attenuation * textureColor;
                // printf("textureColor: %f %f %f\n", textureColor.x(), textureColor.y(), textureColor.z());
                // printf("attenuation: %f %f %f\n", attenuation.x(), attenuation.y(), attenuation.z());
            }
            return true;
        } else if (rand < ratio.getSpecularThres()) {
            // specular
            // printf("specular\n");
            Vector3f reflected = reflect(ray.getDirection().normalized(), hit.getNormal()).normalized();
            scattered = Ray(ray.pointAtParameter(hit.getT()), reflected, ray.time);
            attenuation = specularColor;
            return Vector3f::dot(scattered.getDirection(), hit.getNormal()) > 0;
        } else {
            // refract
            attenuation = Vector3f(1, 1, 1);

            // Vector3f N = hit.getNormal();
            // Vector3f V = ray.getDirection().normalized();
            // float cos_theta = Vector3f::dot(-V, N);
            // float sin_theta = sqrt(1 - cos_theta * cos_theta);
            // float n_ratio = old_n / n;
            // // check if total internal reflection
            // if (n_ratio * sin_theta > 1) {
            //     Vector3f reflected = reflect(V, N);
            //     scattered = Ray(ray.pointAtParameter(hit.getT()), reflected);
            //     return true;
            // }

            // // refract
            // float cos_phi = sqrt(1 - n_ratio * n_ratio * (1 - cos_theta * cos_theta));
            // Vector3f refracted = n_ratio * (V + cos_theta * N) - cos_phi * N;
            // scattered = Ray(ray.pointAtParameter(hit.getT()), refracted);
            // return true;


            float R0 = pow((1 - n) / (1 + n), 2);
            double refraction_ratio = front ? (1.0 / n) : n;

            Vector3f unit_direction = ray.getDirection().normalized();
            Vector3f norm = front ? hit.getNormal() : -hit.getNormal();
            float cos_theta = Vector3f::dot(-unit_direction, norm);
            float cos2 = 1 - refraction_ratio * refraction_ratio * (1 - cos_theta * cos_theta);
            bool full_reflection = cos2 < 0;
            Vector3f direction;

            if (full_reflection) {
                direction = reflect(unit_direction, norm);
            } else {
                float R = R0 + (1 - R0) * pow(1 - cos_theta, 5);
                if (rand_thres() < R) {
                    direction = reflect(unit_direction, norm);
                } else {
                    // printf("refract\n");
                    // printf("cos_theta: %f\n", cos_theta);
                    // printf("cos2: %f\n", sqrt(cos2));
                    // printf("refraction_ratio: %f\n", refraction_ratio);
                    // if (front) {
                    //     printf("front\n");
                    // } else {
                    //     printf("back\n");
                    // }
                    // Vector3f r_out_prep = refraction_ratio * (unit_direction + cos_theta * hit.getNormal());
                    direction = (refraction_ratio * (unit_direction + cos_theta * norm) - sqrt(cos2) * norm).normalized();
                }
            }

            scattered = Ray(ray.pointAtParameter(hit.getT()), direction, ray.time);

            // scattered = Ray(ray.pointAtParameter(hit.getT()), ray.getDirection());
            // printf("refract\n");
            return true;
        }
    }
};


#endif // MATERIAL_H
