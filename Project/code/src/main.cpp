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

#include "pt.hpp"

#include <string>

using namespace std;

int main(int argc, char *argv[]) {
    for (int argNum = 1; argNum < argc; ++argNum) {
        std::cout << "Argument " << argNum << " is: " << argv[argNum] << std::endl;
    }

    if (argc != 6) {
        cout << "Usage: ./build/PT <input scene file> <output bmp file> <rounds> <max_depth> <step>" << endl;
        return 1;
    }
    string inputFile = argv[1];
    string outputFile = argv[2];  // only bmp is allowed.
    int rounds = atoi(argv[3]);
    int max_depth = atoi(argv[4]);
    int step = atoi(argv[5]);

    cout << "Hello! Computer Graphics!" << endl;

    SceneParser sceneParser(inputFile.c_str());
    // Path Tracing
    PathTracing pathTracing(&sceneParser, outputFile, rounds, max_depth, step);
    pathTracing.render();
    // save the image
    pathTracing.save();
    
    return 0;
}

