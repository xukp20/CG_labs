/**
 * Main implementation of sppm
 * a render class
*/
#pragma once

#include <vector>
#include <string>

#include "renderer.hpp"
#include "image.hpp"
#include "camera.hpp"
#include "kdtree.hpp"
#include "hitpoint.hpp"
#include "ray.hpp"
#include "group.hpp"


// utility functions
inline float rand_bias() {
    // -1 ~ 1
    return 2 * ((float)rand() / RAND_MAX) - 1;
}

// SPPM Renderer class
class SPPM : public Renderer {
public:
    // come from the scene parser, for convenience
    Camera *camera;     // the camera of the scene
    Group *group;       // the group of objects in the scene

    // new variables created or needed to store the data
    Image *image;       // image created, to be written to output file
    KDTree *kdtree;     // kdtree for building the photon map
    std::vector<HitPoint*> hitPoints;  // hit points of each pixel at one iteration

    // parameter settings
    int rounds;         // total iteration rounds
    int max_depth;      // max depth of the photon map
    int totalPhotons;   // total photon number
    int step;           // step of saving the image

    // image info
    int width;
    int height;

    // constructor
    SPPM(SceneParser *scene, std::string output_file,
        int rounds=100,
        int max_depth=10, int totalPhotons=500, int step=10          // optional
        
    ) : Renderer(scene, output_file), max_depth(max_depth), totalPhotons(totalPhotons), step(step), rounds(rounds)
    {
        camera = scene->getCamera();
        image = new Image(camera->getWidth(), camera->getHeight());
        kdtree = nullptr;

        width = camera->getWidth();
        height = camera->getHeight();
    }

    // main interface
    void render() {
        // init hit points
        initHitPoints();

        for (int i = 0; i < rounds; i++) {
            photonTracingPass();
            photonMapping();
            rayTracingPass();
            if (i % step == 0 && i != 0) {
                std::string checkpoint_name = output_file + "_" + std::to_string(i);
                image->SaveBMP(checkpoint_name.c_str());
            }
        }
        image->SaveBMP(output_file.c_str());
    }

    // steps
    void photonTracingPass() {
        // looping through the pixels
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; i++) {
                // sample a ray with a random bias from the pixel
                Ray ray = camera->generateRay(Vector2f(i + rand_bias(), j + rand_bias()));
                // store the ray information
                // TODO
                // trace the ray
                photonTracing(ray, 0);
            }
        }
    }

    void photonTracing(Ray ray, int depth) {
        // TODO

    }

    void photonMapping() {

    }

    void rayTracingPass() {

    }

    // tools 
    void initHitPoints() {
        // init hit points
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; i++) {
                hitPoints.emplace_back(new HitPoint());
            }
        }
    }
};