#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "texture.hpp"

ImageTexture::ImageTexture(const std::string &filename) : Texture(filename) {
        if (filename == "") {
            printf("ImageTexture: filename is empty\n");
            return;
        }
        pic = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
        printf("ImageTexture: %s, width: %d, height: %d, nrChannels: %d\n", filename.c_str(), width, height, nrChannels);
    }

Vector3f ImageTexture::getColor(float u, float v, Vector3f p) {
    if (pic == nullptr) {
        return Vector3f(0, 1, 1);
    }
    int i = u * width;
    int j = (1 - v) * height - 0.001;
    if (i < 0) {
        i = 0;
    }
    if (j < 0) {
        j = 0;
    }
    if (i > width - 1) {
        i = width - 1;
    }
    if (j > height - 1) {
        j = height - 1;
    }
    
    // // interpolate
    // int i1 = int(i);
    // int i2 = i1 + 1; 
    // i2 = i2 > width - 1 ? width - 1 : i2;
    // int j1 = int(j);
    // int j2 = j1 + 1;
    // j2 = j2 > height - 1 ? height - 1 : j2;
    // float s = i - i1, t = j - j1;
    // Vector3f c1 = Vector3f::ZERO;
    // int idx1 = (i1 + j1 * width) * nrChannels;
    // c1 += (1 - s) * (1 - t) * Vector3f(pic[idx1], pic[idx1 + 1], pic[idx1 + 2]);
    // int idx2 = (i2 + j1 * width) * nrChannels;
    // c1 += s * (1 - t) * Vector3f(pic[idx2], pic[idx2 + 1], pic[idx2 + 2]);
    // int idx3 = (i1 + j2 * width) * nrChannels;
    // c1 += (1 - s) * t * Vector3f(pic[idx3], pic[idx3 + 1], pic[idx3 + 2]);
    // int idx4 = (i2 + j2 * width) * nrChannels;
    // c1 += s * t * Vector3f(pic[idx4], pic[idx4 + 1], pic[idx4 + 2]);
    // c1 = c1 / 255.0;

    // no interpolate
    Vector3f c1 = Vector3f::ZERO;
    int idx1 = (i + j * width) * nrChannels;
    c1 += Vector3f(pic[idx1], pic[idx1 + 1], pic[idx1 + 2]);
    c1 = c1 / 255.0;

    return c1;
}


