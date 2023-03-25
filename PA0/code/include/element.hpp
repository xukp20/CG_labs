#pragma once

#include <image.hpp>
#include <algorithm>
#include <queue>
#include <cstdio>

class Element {
public:
    virtual void draw(Image &img) = 0;
    virtual ~Element() = default;
};

class Line : public Element {

public:
    int xA, yA;
    int xB, yB;
    Vector3f color;
    void draw(Image &img) override {
        // TODO: Implement Bresenham Algorithm
        printf("Draw a line from (%d, %d) to (%d, %d) using color (%f, %f, %f)\n", xA, yA, xB, yB,
                color.x(), color.y(), color.z());
        int x0, y0, x1, y1;
        int x, y;
        float k, e, k_abs;

        if (xA == xB) {     // special case: vertical line
            if (yA > yB) {
                y0 = yB;
                y1 = yA;
            } else {
                y0 = yA;
                y1 = yB;
            }
            for (int i = y0; i <= y1; ++i) {
                img.SetPixel(xA, i, color);
            }
            return;
        }

        k = ((float)(yB - yA) / (float)(xB - xA));
        // draw from x if |k| <= 1, or draw from y
        k_abs = k >= 0 ? k : -k;

        if (k_abs <= 1){
            // compare yA and yB, draw from the lower one
            if (yA > yB) {
                x0 = xB;
                y0 = yB;
                x1 = xA;
                y1 = yA;
            } else {
                x0 = xA;
                y0 = yA;
                x1 = xB;
                y1 = yB;
            }
            // compare x0, x1
            bool reverse = x0 > x1;

            e = -0.5;
            x = x0;
            y = y0;

            if (reverse) {
                for (int i = x0; i >= x1; --i) {
                    img.SetPixel(i, y, color);
                    e += k_abs;
                    if (e >= 0) {
                        ++y;
                        e -= 1;
                    }
                }
            } else {
                for (int i = x0; i <= x1; ++i) {
                    img.SetPixel(i, y, color);
                    e += k_abs;
                    if (e >= 0) {
                        ++y;
                        e -= 1;
                    }
                }
            }
        } else {        // draw from y
            // compare xA and xB, draw from the lower one
            if (xA > xB) {
                x0 = xB;
                y0 = yB;
                x1 = xA;
                y1 = yA;
            } else {
                x0 = xA;
                y0 = yA;
                x1 = xB;
                y1 = yB;
            }

            // compare y0, y1
            bool reverse = y0 > y1;

            e = -0.5;
            x = x0;
            y = y0;
            k_abs = 1 / k_abs;

            if (reverse) {
                for (int i = y0; i >= y1; --i) {
                    img.SetPixel(x, i, color);
                    e += k_abs;
                    if (e >= 0) {
                        ++x;
                        e -= 1;
                    }
                }
            } else {
                for (int i = y0; i <= y1; ++i) {
                    img.SetPixel(x, i, color);
                    e += k_abs;
                    if (e >= 0) {
                        ++x;
                        e -= 1;
                    }
                }
            }
        }

        return;
    }
};

class Circle : public Element {

public:
    int cx, cy;
    int radius;
    Vector3f color;
    void draw(Image &img) override {
        // TODO: Implement Algorithm to draw a Circle
        printf("Draw a circle with center (%d, %d) and radius %d using color (%f, %f, %f)\n", cx, cy, radius,
               color.x(), color.y(), color.z());
    }
};

class Fill : public Element {

public:
    int cx, cy;
    Vector3f color;
    void draw(Image &img) override {
        // TODO: Flood fill
        printf("Flood fill source point = (%d, %d) using color (%f, %f, %f)\n", cx, cy,
                color.x(), color.y(), color.z());
    }
};