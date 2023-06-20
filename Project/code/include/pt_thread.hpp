/**
 * Path Tracing 
*/
#pragma once

#include <vector>
#include <string>
#include <thread>
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

    PathTracing(SceneParser *scene, std::string output_file, int rounds=30, int max_depth=10, int step=5) : Renderer(scene, output_file) {
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

    void render_image_tile(int start, int end, int id) {
        clock_t start_time, end_time;
        start_time = time(NULL);

        // each thread compute a part of the image
        for (int i = start; i < end; i++) {
            end_time = time(NULL);
            // approximate time
            float time = (float)(end_time - start_time) / 60;
            float all_time = (float)(end_time - start_time) / (i - start + 1) * (end - start) / 60;
            printf("thread %d: %d / %d, used time: %.2f min, all time: %.2f min\n", id, i - start + 1, end - start, time, all_time);
            for (int j = 0; j < height; j++) {
                Vector3f color = Vector3f::ZERO;
                // Ray ray = camera->generateRay(Vector2f(i + rand_bias(), j + rand_bias()));
                // color += traceRay(ray, 0);  // init with color of black
                // image->SetPixel(i, j, color);
                for (int k = 0; k < rounds; k++) {
                    Ray ray = camera->generateRay(Vector2f(i + rand_bias(), j + rand_bias()));
                    color += traceRay(ray, 0);  // init with color of black
                }
                color = color / rounds;
                image->SetPixel(i, j, color);
            }
        }
    }

    void render_round_tile(int rounds, Image* image, int id) {
        clock_t start, end;
        start = time(NULL);

        // compute rounds times
        for (int i = 0; i < width; i++) {
            // compute approximate left time
            end = time(NULL);
            int time_used = end - start;
            int time_left = (int)((float)time_used / (i + 1) * (width - i - 1));
            // printf("thread %d: %d / %d, used time: %.2f min, left time: %.2f min\n", id, i, width, (float)time_used / 60, (float)time_left / 60);
            for (int j = 0; j < height; j++) {
                Vector3f color = Vector3f::ZERO;
                // Ray ray = camera->generateRay(Vector2f(i + rand_bias(), j + rand_bias()));
                // color += traceRay(ray, 0);  // init with color of black
                // image->SetPixel(i, j, color);
                for (int k = 0; k < rounds; k++) {
                    Ray ray = camera->generateRay(Vector2f(i + rand_bias(), j + rand_bias()));
                    color += traceRay(ray, 0);  // init with color of black
                }
                image->SetPixel(i, j, color);
            }
        }
    }

    void render() {
        // render_tiles();
        render_all();
        // render_rounds();
    }

    void render_all() {
        // just use mpi for scheduling
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

    void render_rounds() {
        // divide the rounds into threads
        // each thread compute all of the image

        // timer
        clock_t start, end;
        start = time(NULL);

        int num_threads = omp_get_num_procs();
        std::vector<std::thread> threads;
        std::vector<Image*> images;
        int rounds_per_thread = (rounds + num_threads - 1) / num_threads;

        for (int i = 0; i < num_threads; i++) {
            images.push_back(new Image(width, height));
            if (i == num_threads - 1) {
                threads.push_back(std::thread(&PathTracing::render_round_tile, this, rounds - i * rounds_per_thread, images[i], i));
            }
            else {
                threads.push_back(std::thread(&PathTracing::render_round_tile, this, rounds_per_thread, images[i], i));
            }
        }

        // wait for each thread to finish
        for (int i = 0; i < num_threads; i++) {
            threads[i].join();
        }

        // merge the images
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++){
                Vector3f color = Vector3f::ZERO;
                for (int k = 0; k < num_threads; k++) {
                    color = color + images[k]->GetPixel(i, j);
                }
                color = color / rounds;
                image->SetPixel(i, j, color);
            }
        }

        end = time(NULL);
        printf("\nTime: %.2f min\n", (double)(end - start)/60);

        save();
    }

    void render_tiles() {
        // Use multi-threading

        // timer
        clock_t start, end;
        start = time(NULL);

        // each thread compute a part of the image
        std::vector<std::thread> threads;
        int num_threads = omp_get_num_procs();
        int width_per_thread = (width + num_threads - 1) / num_threads;

        printf("num_threads: %d\n", num_threads);

        for (int i = 0; i < num_threads; i++) {
            threads.push_back(std::thread(&PathTracing::render_image_tile, this, i * width_per_thread, (i + 1) * width_per_thread > width ? width : (i + 1) * width_per_thread, i));
        }

        for (int i = 0; i < num_threads; i++) {
            threads[i].join();
        }

        end = time(NULL);
        printf("\nTime: %.2f min\n", (double)(end - start)/60);

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