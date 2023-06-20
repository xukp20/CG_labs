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

    PathTracing(SceneParser *scene, std::string output_file, int rounds=30, int max_depth=5, int step=5

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
        for (int k = 0; k < rounds; k++) {
            float ratio = (float)(k + 0.0001) / (rounds);
            // approximate time
            end = time(NULL);
            float time = (float)(end - start) / 60;
            float time_left = time / ratio - time;
            fprintf(stderr, "\rProgress: %.2f%%, Time: %.2fmin, Time left: %.2fmin", ratio * 100, time, time_left);
            fflush(stderr);
#pragma omp parallel for schedule(dynamic, 1)
            for (int i = 0; i < width; i++) {
                for (int j = 0; j < height; j++) {
                    Vector3f color = image->GetPixel(i, j);
                    // Ray ray = camera->generateBlurRay(Vector2f(i + rand_bias(), j + rand_bias()));
                    Ray ray = camera->generateRay(Vector2f(i + rand_bias(), j + rand_bias()));
                    color += traceRay(ray, 0);  // init with color of black
                    image->SetPixel(i, j, color);
                }
            }

            if (k % step == 0 && k != 0) {
                // save the image
                Image *new_image = new Image(width, height);
                for (int i = 0; i < width; i++) {
                    for (int j = 0; j < height; j++) { 
                        Vector3f color = image->GetPixel(i, j);
                        color = color / (k + 1);
                        new_image->SetPixel(i, j, color);
                    }
                }
                std::string filename = output_file.substr(0, output_file.find_last_of(".")) + "_" + std::to_string(k) + ".bmp";
                new_image->SaveBMP(filename.c_str());
            }
        }
        printf("\n");

        // normalize the image
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
                Vector3f color = image->GetPixel(i, j);
                color = color / rounds;
                image->SetPixel(i, j, color);
            }
        }
        save();
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