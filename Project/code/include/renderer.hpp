/**
 * Renderer for the image
 * only virtual function render() save() is needed, only base class in this file, see sppm for the sppm implementation 
 * xkp
 */

#pragma once
#include <string>

#include "scene_parser.hpp"

class Renderer {
protected:
    const SceneParser *scene;
    std::string output_file;

public:
    Renderer(SceneParser *scene, std::string output_file) : scene(scene), output_file(output_file) {}

    virtual ~Renderer() {}
    virtual void render() = 0;
    virtual void save() = 0;
};