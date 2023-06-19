#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>

#include "scene_parser.hpp"
#include "image.hpp"
#include "camera.hpp"
#include "group.hpp"
#include "light.hpp"

#include <string>

using namespace std;

int main(int argc, char *argv[]) {
    for (int argNum = 1; argNum < argc; ++argNum) {
        std::cout << "Argument " << argNum << " is: " << argv[argNum] << std::endl;
    }

    if (argc != 3) {
        cout << "Usage: ./bin/PA1 <input scene file> <output bmp file>" << endl;
        return 1;
    }
    string inputFile = argv[1];
    string outputFile = argv[2];  // only bmp is allowed.

    // TODO: Main RayCasting Logic
    // First, parse the scene using SceneParser.
    // Then loop over each pixel in the image, shooting a ray
    // through that pixel and finding its intersection with
    // the scene.  Write the color at the intersection to that
    // pixel in your output image.
    cout << "Hello! Computer Graphics!" << endl;

    SceneParser sceneParser(inputFile.c_str());
    Image image(sceneParser.getCamera()->getWidth(), sceneParser.getCamera()->getHeight());

    // Ray Casting
    for (int x = 0; x < sceneParser.getCamera()->getWidth(); ++x) {
        for (int y = 0; y < sceneParser.getCamera()->getHeight(); ++y) {
            Ray ray = sceneParser.getCamera()->generateRay(Vector2f(x, y));
            Group *group = sceneParser.getGroup();
            Hit hit;

            // find the closest intersection
            bool isIntersect = group->intersect(ray, hit, 0);
            if (isIntersect) {
                Vector3f finalColor = Vector3f::ZERO;
                // sum up the light from all the lights
                for (int li = 0; li < sceneParser.getNumLights(); ++li) {
                    Light *light = sceneParser.getLight(li);
                    Vector3f L, lightColor;
                    // get illumination from the light
                    light->getIllumination(ray.pointAtParameter(hit.getT()), L, lightColor);
                    finalColor += hit.getMaterial()->Shade(ray, hit, L, lightColor);
                }
                // write the color to the image
                image.SetPixel(x, y, finalColor);
            } else {
                // background color
                image.SetPixel(x, y, sceneParser.getBackgroundColor());
            }
        }
    }

    image.SaveImage(outputFile.c_str());
    
    return 0;
}

