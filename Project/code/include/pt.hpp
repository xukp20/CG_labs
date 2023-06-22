/**
 * Path Tracing 
*/
#pragma once

#include <vector>
#include <string>
#include <omp.h>

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
    int step;           // step of saving the image

    // attributes
    int width, height;  // width and height of the image

    PathTracing(SceneParser *scene, std::string output_file, int rounds=100, int max_depth=10, int step=20

    ) : Renderer(scene, output_file) {
        camera = scene->getCamera();
        group = scene->getGroup();
        width = camera->getWidth();
        height = camera->getHeight();
        image = new Image(width, height);

        this->rounds = rounds;
        this->max_depth = max_depth;
        this->step = step;

        fprintf(stderr, "PathTracing: %d rounds, %d max_depth\n", rounds, max_depth);
    }

    ~PathTracing() {
        delete image;
    }

    static inline float rand_bias() {
        // -1 ~ 1
        return RAND_SIGNED;
    }

    static inline float rand_thres() {
        // 0 ~ 1
        return RAND_UNIFORM;
    }

    void render() {
        // timer
        clock_t start, end;
        start = time(NULL);

#pragma omp parallel for schedule(dynamic, 1)
        for (int i = 0; i < width; i++) {
            if (i == 0) {
                int num = omp_get_num_threads();
                fprintf(stderr, "Number of threads: %d\n", num);
            }
            float ratio = (float)(i + 0.0001) / (width);
            // approximate time
            end = time(NULL);
            float time = (float)(end - start) / 60;
            float time_left = time / ratio - time;
            fprintf(stderr, "\rProgress: %.2f%%, Time: %.2fmin, Time left: %.2fmin", ratio * 100, time, time_left);
            fflush(stderr);
            for (int j = 0; j < height; j++) {
                Vector3f color = Vector3f::ZERO;
                for (int k = 0; k < rounds; k++) {
                    Ray ray = camera->generateBlurRay(Vector2f(i + rand_bias(), j + rand_bias()));
                    color += traceRay(ray, 0);  // init with color of black
                }
                color = color / rounds;
                image->SetPixel(i, j, color);
            }
        }
        printf("\n");
        save();
    }
    

    // traceRay: trace a ray for once
    Vector3f traceRay(Ray ray, int depth) {
        Vector3f color = Vector3f::ZERO;
        Vector3f cf = Vector3f(1.0, 1.0, 1.0);
        Hit hit;
        
        // bool trace = false;
        while(true) {
            if (++depth > max_depth || cf.x() < 1e-3 || cf.y() < 1e-3 || cf.z() < 1e-3) {
                break;
            }
            hit = Hit();
            if (group->intersect(ray, hit, 0.001)) {
                // printf("hit at point: %f %f %f\n", ray.pointAtParameter(hit.getT()).x(), ray.pointAtParameter(hit.getT()).y(), ray.pointAtParameter(hit.getT()).z());
                // printf("T = %f\n", hit.getT());
                // hit
                // move the ray
                Ray scattered(Vector3f::ZERO, Vector3f::ZERO);
                Vector3f attenuation;
                // check if entering the object
                // note: the normal is always pointing outwards
                bool front = Vector3f::dot(ray.getDirection(), hit.getNormal()) < 0;
                if (hit.getMaterial()->scatter(ray, hit, attenuation, scattered, front)) {
                    color += cf * hit.getMaterial()->selfColor;
                    ray = scattered;
                    // printf("Attenuation: %f %f %f\n", attenuation.x(), attenuation.y(), attenuation.z());
                    cf = cf * attenuation;
                } else {
                    break;
                }
            } else {
                // no hit
                color += cf * scene->getBackgroundColor();
                break;
            }
        }
        // if (trace) {
        //     printf("final color: %f %f %f\n", color.x(), color.y(), color.z());
        // }
        return color;

        // if (depth > max_depth) {
        //     return Vector3f::ZERO;
        // }

        // if (group->intersect(ray, hit, 0.001)) {
        //     // hit
        //     // move the ray
        //     Ray scattered(Vector3f::ZERO, Vector3f::ZERO);
        //     Vector3f attenuation;
        //     // check if entering the object
        //     // note: the normal is always pointing outwards
        //     bool front = Vector3f::dot(ray.getDirection(), hit.getNormal()) < 0;
        //     // printf("T: %f\n", hit.getT());
        //     // printf("hit point: (%f, %f, %f)\n", ray.pointAtParameter(hit.getT()).x(), ray.pointAtParameter(hit.getT()).y(), ray.pointAtParameter(hit.getT()).z());
        //     if (hit.getMaterial()->scatter(ray, hit, attenuation, scattered, front)) {
        //         // printf("attenuation: (%f, %f, %f)\n", attenuation.x(), attenuation.y(), attenuation.z());
        //         // printf("selfColor: (%f, %f, %f)\n", hit.getMaterial()->selfColor.x(), hit.getMaterial()->selfColor.y(), hit.getMaterial()->selfColor.z());

        //         return hit.getMaterial()->selfColor + attenuation * traceRay(scattered, depth + 1);
        //     } else {
        //         return Vector3f::ZERO;
        //     }
        // } else {
        //     // no hit
        //     return scene->getBackgroundColor();
        // }
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