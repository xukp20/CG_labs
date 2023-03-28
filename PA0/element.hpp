#pragma once

#include <image.hpp>
#include <algorithm>
#include <queue>
#include <cstdio>

class Element {
public:
    virtual void draw(Image &img) = 0;
    virtual ~Element() = default;

    bool isInside(int x, int y, Image &img) {
        return x >= 0 && x < img.Width() && y >= 0 && y < img.Height();
    }
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
                if (isInside(xA, i, img))
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
                    if (isInside(i, y, img))
                        img.SetPixel(i, y, color);
                    e += k_abs;
                    if (e >= 0) {
                        ++y;
                        e -= 1;
                    }
                }
            } else {
                for (int i = x0; i <= x1; ++i) {
                    if (isInside(i, y, img))
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
                    if (isInside(x, i, img))
                        img.SetPixel(x, i, color);
                    e += k_abs;
                    if (e >= 0) {
                        ++x;
                        e -= 1;
                    }
                }
            } else {
                for (int i = y0; i <= y1; ++i) {
                    if (isInside(x, i, img))
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

        // find 1/8 circle, x>0, y>0 part
        int x = 0;
        int y = radius;
        float d = 1.5 - radius;
        circlePoints(img, x, y);
        while (x <= y) {
            if (d < 0) {
                d += 2 * x + 3;
            } else {
                d += 2 * (x - y) + 5;
                --y;
            }
            ++x;
            circlePoints(img, x, y);
        }
    }

    // draw 8 points, x,y should be the relative position to cx, cy
    void circlePoints(Image &img, int x, int y) {
        img.SetPixel(cx + x, cy + y, color);
        img.SetPixel(cx - x, cy + y, color);
        img.SetPixel(cx + x, cy - y, color);
        img.SetPixel(cx - x, cy - y, color);
        img.SetPixel(cx + y, cy + x, color);
        img.SetPixel(cx - y, cy + x, color);
        img.SetPixel(cx + y, cy - x, color);
        img.SetPixel(cx - y, cy - x, color);
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

        // use a queue for BFS
        std::queue<int> qx;
        std::queue<int> qy;
        int x = cx;
        int y = cy;

        // get the color of the source point
        Vector3f sourceColor = img.GetPixel(x, y);
        // push the source point into the queue
        qx.push(x);
        qy.push(y);

        // loop through the queue
        while (!qx.empty()) {
            x = qx.front();
            y = qy.front();
            qx.pop();
            qy.pop();

            // check if the point is in the image
            if (x < 0 || x >= img.Width() || y < 0 || y >= img.Height()) {
                continue;
            }

            // check if the point is the same color as the source point
            if (img.GetPixel(x, y) != sourceColor) {
                // already filled, so the children of this point are already in the queue
                continue;
            }

            // set the point to the target color
            img.SetPixel(x, y, color);

            // push the 4 adjacent points into the queue
            qx.push(x); qy.push(y + 1);
            qx.push(x); qy.push(y - 1);
            qx.push(x - 1); qy.push(y);
            qx.push(x + 1); qy.push(y);
        }
        return;
    }
};