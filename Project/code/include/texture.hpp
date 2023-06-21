#pragma once

#include <vecmath.h>
#include <vector>
#include <string>

#include "image.hpp"


class Texture {
public:
    std::string filename;
    int width, height;

    Texture(const std::string &filename) : filename(filename) {
        // check if not empty
        if (filename != "") {
            // load image
            // TODO
        }
    }

    Vector3f getColor(float u, float v, float p) const {
        // TODO
        return Vector3f(0, 0, 0);
    }
};

class CheckerBoardTexture : public Texture {
public:
    CheckerBoardTexture(Vector3f color1, Vector3f color2) : Texture("") {
        this->color1 = color1;
        this->color2 = color2;
    }

    Vector3f getColor(float u, float v, float p) const {
        float sines = sin(10 * u) * sin(10 * v);
        if (sines < 0) {
            return color1;
        } else {
            return color2;
        }
    }

    Vector3f color1;
    Vector3f color2;
};


class NoiseTexture : public Texture {
public:
    NoiseTexture(bool color) : Texture("") {
        // use perlin noise
        shuffle();
        if (color) {
            init_ranvec();  
        } else {
            init_ranfloat();
        }
    }

    Vector3f getColor(float u, float v, float p) const {
        return Vector3f(1, 1, 1) * noise(p);
    }

    ~NoiseTexture() {
        if (ranfloat != nullptr) {
            delete[] ranfloat;
        }
        if (ranvec != nullptr) {
            delete[] ranvec;
        }
        if (perm_x != nullptr) {
            delete[] perm_x;
        }
        if (perm_y != nullptr) {
            delete[] perm_y;
        }
        if (perm_z != nullptr) {
            delete[] perm_z;
        }
    }

    static const int point_count = 256;
    double *ranfloat;
    Vector3f *ranvec;
    int *perm_x;
    int *perm_y;
    int *perm_z;

    void shuffle() {
        perm_x = perlin_generate_perm();
        perm_y = perlin_generate_perm();
        perm_z = perlin_generate_perm();
    }

    int *perlin_generate_perm() {
        int *p = new int[point_count];
        for (int i = 0; i < point_count; ++i) {
            p[i] = i;
        }
        permute(p, point_count);
        return p;
    }

    void permute(int *p, int n) {
        for (int i = n - 1; i > 0; --i) {
            int target = int(drand48() * (i + 1));
            int tmp = p[i];
            p[i] = p[target];
            p[target] = tmp;
        }
    }

    void init_ranvec() {
        for (int i = 0; i < point_count; ++i) {
            ranvec[i] = Vector3f(drand48(), drand48(), drand48());
        }
    }

    void init_ranfloat() {
        for (int i = 0; i < point_count; ++i) {
            ranfloat[i] = drand48();
        }
    }

    Vector3f noise(const Vector3f &p) const {
        int i = int(4 * p.x()) & (point_count - 1);
        int j = int(4 * p.y()) & (point_count - 1);
        int k = int(4 * p.z()) & (point_count - 1);
        if (ranfloat != nullptr) {
            double t = ranfloat[perm_x[i] ^ perm_y[j] ^ perm_z[k]];
            return Vector3f(1, 1, 1) * t;
        } else {
            return ranvec[perm_x[i] ^ perm_y[j] ^ perm_z[k]];
        }
    }
};
