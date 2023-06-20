/**
 * Path Tracing 
*/
#pragma once

#include <vector>
#include <string>

#include "renderer.hpp"
#include "image.hpp"
#include "camera.hpp"
#include "ray.hpp"
#include "group.hpp"
#include "hit.hpp"




class PathTracing : public Renderer {
public:
    // come from the scene parser, for convenience
    Camera *camera;     // the camera of the scene
    Group *group;       // the group of objects in the scene

    // new variables created or needed to store the data
    Image *image;       // image created, to be written to output file

    // parameters
    int rounds;         // number of rounds of path tracing for each pixel
    int max_depth;      // max depth of the path tracing

    // attributes
    int width, height;  // width and height of the image

    PathTracing(SceneParser *scene, std::string output_file, int rounds=50, int max_depth=20) : Renderer(scene, output_file) {
        camera = scene->getCamera();
        group = scene->getGroup();
        width = camera->getWidth();
        height = camera->getHeight();
        image = new Image(width, height);

        this->rounds = rounds;
        this->max_depth = max_depth;

        fprintf(stderr, "PathTracing: %d rounds, %d max_depth\n", rounds, max_depth);
    }

    ~PathTracing() {
        delete image;
    }

    static inline float rand_bias() {
        // -1 ~ 1
        return 2 * ((float)rand() / RAND_MAX) - 1;
    }

    static inline float rand_thres() {
        // 0 ~ 1
        return (float)rand() / RAND_MAX;
    }

    void render() {
        // timer
        clock_t start, end;
        start = time(NULL);
#pragma omp parallel for schedule(dynamic, 1)
        for (int i = 0; i < width; i++) {
            // printf("i: %d\n", i);
            float ratio = (float)(1 + i) / width;
            fprintf(stderr, "\rProgress: %.2f%%", ratio * 100);
            // approximate time
            end = time(NULL);
            float time = (float)(end - start) / 60;
            float time_left = time / ratio - time;
            fprintf(stderr, ", Time elapsed: %.2fmin", time);
            fprintf(stderr, ", Time left: %.2fmin", time_left);
            for (int j = 0; j < height; j++) {
                Vector3f color = Vector3f::ZERO;
                for (int k = 0; k < rounds; k++) {
                    Ray ray = camera->generateRay(Vector2f(i + rand_bias(), j + rand_bias()));
                    color += traceRay(ray, 0);  // init with color of black
                
                    // printf("round: %d, color: (%f, %f, %f)\n", k, color.x(), color.y(), color.z());
                }
                color = color / rounds;
                image->SetPixel(i, j, color);
            }
        }
        printf("\n");
    }
    

    // traceRay: trace a ray for once
    Vector3f traceRay(Ray ray, int depth, float refr_index = 1.0) {
        Hit hit;
        if (depth >= max_depth) {
            // reach the max depth
            // printf("diffRatio: (%f, %f, %f)\n", diffRatio.x(), diffRatio.y(), diffRatio.z());
            // printf("color: (%f, %f, %f)\n", color.x(), color.y(), color.z());
            return Vector3f::ZERO;
        }

        // while (depth < max_depth && diffRatio.x() >= 1e-3 && diffRatio.y() >= 1e-3 && diffRatio.z() >= 1e-3) {
        //     if (group->intersect(ray, hit, 1e-3)) {
        //         // has intersection

        //         // move the ray
        //         Vector3f new_origin = ray.pointAtParameter(hit.getT());
        //         Vector3f new_dir;

        //         // get direction
        //         Material *material = hit.getMaterial();
        //         float rand_x = rand_thres();
        //         // if (material->ratio.getDiffuseThres() < 1) {
        //         //     printf("rand_x: %f\n", rand_x);
        //         //     printf("diffuseThres: %f\n", material->ratio.getDiffuseThres());
        //         // }
        //         // if (material->selfColor.x() != 0) {
        //         //     printf("selfColor: (%f, %f, %f)\n", material->selfColor.x(), material->selfColor.y(), material->selfColor.z());
        //         //     printf("diffRatio: (%f, %f, %f)\n", diffRatio.x(), diffRatio.y(), diffRatio.z());
        //         //     printf("diffuseColor: (%f, %f, %f)\n", material->diffuseColor.x(), material->diffuseColor.y(), material->diffuseColor.z());
        //         // }

        //         Vector3f norm = hit.getNormal();
        //         if (rand_x < material->ratio.getDiffuseThres()) {
        //             // use random unit sphere to get a random direction
        //             Vector3f rand_dir = getRandDirInSphere();
        //             // get the new ray
        //             new_dir = (norm + rand_dir).normalized();
        //             // printf("diffuse\n");
        //             // printf("rand_dir: (%f, %f, %f)\n", rand_dir.x(), rand_dir.y(), rand_dir.z());
        //         } else if (rand_x < material->ratio.getSpecularThres()) {
        //             // specular
        //             new_dir = ray.getDirection() - 2 * Vector3f::dot(norm, ray.getDirection()) * norm;
        //             new_dir = new_dir.normalized();
        //             // printf("specular\n");
        //         } else {
        //             // refraction
        //             float cos_theta = Vector3f::dot(norm, ray.getDirection());
        //             float sin_theta = sqrt(1 - cos_theta * cos_theta);
        //             float relative_index = refr_index / material->n;
        //             // check if total internal reflection
        //             if (relative_index * sin_theta >= 1) {
        //                 // total internal reflection
        //                 new_dir = ray.getDirection() - 2 * Vector3f::dot(norm, ray.getDirection()) * norm;
        //                 new_dir = new_dir.normalized();
        //             } else {
        //                 // refraction
        //                 // ! sqrt may be slow
        //                 float cos_theta2 = sqrt(1 - relative_index * relative_index * (1 - cos_theta * cos_theta));
        //                 new_dir = (relative_index * ray.getDirection() + (relative_index * cos_theta - cos_theta2) * norm).normalized();
        //             }
        //             printf("refraction\n");
        //         }

        //         // get the new ray
        //         ray = Ray(new_origin, new_dir);

        //         // get the color
        //         // self-emission
        //         color += material->selfColor * diffRatio;
        //         diffRatio = material->diffuseColor * diffRatio;

        //         if (color.x() != 0) {
        //             fprintf(stderr, "depth: %d, color: (%f, %f, %f)\n", depth, color.x(), color.y(), color.z());
        //             fprintf(stderr, "diffRatio: (%f, %f, %f)\n", diffRatio.x(), diffRatio.y(), diffRatio.z());
        //             fprintf(stderr, "new_dir: (%f, %f, %f)\n", new_dir.x(), new_dir.y(), new_dir.z());
        //             fprintf(stderr, "new_origin: (%f, %f, %f)\n", new_origin.x(), new_origin.y(), new_origin.z());
        //             fprintf(stderr, "selfColor: (%f, %f, %f)\n", material->selfColor.x(), material->selfColor.y(), material->selfColor.z());
        //         }

        //         // trace the new ray
        //         depth++;
        //     } else {
        //         // no intersection
        //         color += scene->getBackgroundColor();
        //         return color;
        //     }
        // }

        if (group->intersect(ray, hit, 0.001)) {
            // hit
            // move the ray
            Ray scattered(Vector3f::ZERO, Vector3f::ZERO);
            Vector3f attenuation;
            if (hit.getMaterial()->scatter(ray, hit, attenuation, scattered, refr_index)) {
                // printf("attenuation: (%f, %f, %f)\n", attenuation.x(), attenuation.y(), attenuation.z());
                // printf("selfColor: (%f, %f, %f)\n", hit.getMaterial()->selfColor.x(), hit.getMaterial()->selfColor.y(), hit.getMaterial()->selfColor.z());
                return hit.getMaterial()->selfColor + attenuation * traceRay(scattered, depth + 1, refr_index);
            } else {
                return Vector3f::ZERO;
            }
        } else {
            // no hit
            return scene->getBackgroundColor();
        }
    }

    // utils
    // get a direction inside the unit sphere
    Vector3f getRandDirInSphere() {
        Vector3f dir;
        do {
            dir = Vector3f(rand_bias(), rand_bias(), rand_bias());
        } while (dir.squaredLength() > 1);
        return dir;
    }

    void save(){
        image->SaveBMP(output_file.c_str());
    }
};