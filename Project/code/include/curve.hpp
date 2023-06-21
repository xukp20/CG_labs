#ifndef CURVE_HPP
#define CURVE_HPP

#include "object3d.hpp"
#include <vecmath.h>
#include <vector>
#include <utility>

#include <algorithm>
// The CurvePoint object stores information about a point on a curve
// after it has been tesselated: the vertex (V) and the tangent (T)
// It is the responsiblility of functions that create these objects to fill in all the data.
struct CurvePoint {
    Vector3f V; // Vertex
    Vector3f T; // Tangent  (unit)
};

class Curve : public Object3D {
protected:
    std::vector<Vector3f> controls;
public:
    explicit Curve(std::vector<Vector3f> points) : controls(std::move(points)) {}

    virtual bool intersect(const Ray &r, Hit &h, float tmin) override {
        return false;
    }

    std::vector<Vector3f> &getControls() {
        return controls;
    }
    
    // get the point on the curve at parameter phi
    virtual CurvePoint getPoint(float phi) = 0;
    // lower bound and upper bound for parameter phi
    virtual float lowerBound() = 0;
    virtual float upperBound() = 0;
};

class BezierCurve : public Curve {
public:
    double **C;
    int n, k;

    explicit BezierCurve(const std::vector<Vector3f> &points) : Curve(points) {
        if (points.size() < 4 || points.size() % 3 != 1) {
            printf("Number of control points of BezierCurve must be 3n+1!\n");
            exit(0);
        }
        n = points.size() - 1;
        k = n;
        init_C();
    }

    ~BezierCurve() override {
        delete_C();
    }

    CurvePoint getPoint(float phi) {
        CurvePoint point;
        point.V = Vector3f(0, 0, 0);
        point.T = Vector3f(0, 0, 0);
        for (int i = 0; i <= n; i++) {
            point.V += B(i, k, phi) * controls[i];
            point.T += dB(i, k, phi) * controls[i];
        }
        return point;
    }

    // useless now
    // void discretize(int resolution, std::vector<CurvePoint>& data) {
    //     data.clear();
    //     // TODO (PA2): fill in data vector
    //     unsigned int n = controls.size() - 1;
    //     unsigned int k = n;

    //     // all the ti 
    //     this->knots = std::vector<float>(n + k + 1, 0);
    //     for (int i = 0; i <= n + k + 1; i++) {
    //         knots[i] = (float)i / (n + k + 1);
    //     }

    //     // step 
    //     float step = 1.0f / resolution / (n + k + 1);

    //     // recursion memoization
    //     this->B = std::vector<std::vector<float>>(n + 1, std::vector<float>(k + 1, 0));
    //     // bitmap 
    //     this->bitmap = std::vector<std::vector<bool>>(n + 1, std::vector<bool>(k + 1, false));

    //     // go through all the ti
    //     for (int i = 0; i < n + k + 1; i++) {
    //         // divide into resolution parts
    //         for (int j = 0; j < resolution; j++) {
    //             float tij = knots[i] + j * step;
    //             Vector3f point(0, 0, 0);
    //             Vector3f tangent(0, 0, 0);
                
    //             // clear bitmap
    //             for (int a = 0; a < n + 1; a++) {
    //                 for (int b = 0; b < k + 1; b++) {
    //                     this->bitmap[a][b] = false;
    //                 }
    //             }
                
    //             // recursion: compute all Bik
    //             for (int a = 0; a < n + 1; a++) {
    //                 Vector3f control = this->controls[a];
    //                 float Bik = compute_B(a, k, tij);
    //                 point += control * Bik;
    //             }           

    //             // compute tangent
    //             for (int a = 0; a < n; a++) {
    //                 Vector3f control = this->controls[a + 1] - this->controls[a];
    //                 float Bik = compute_B(a, k - 1, tij);
    //                 tangent += control * Bik * k;
    //             }

    //             CurvePoint cp;
    //             cp.V = point;
    //             cp.T = tangent.normalized();
    //             data.push_back(cp);
    //         }
    //     }
    // }

    // float compute_B(unsigned int i, unsigned int k, float t){
    //     if (i == 0) {
    //         return pow(1 - t, k);
    //     } 
    //     if (i == k) {
    //         return pow(t, k);
    //     }

    //     if (this->bitmap[i][k]) {
    //         return this->B[i][k];
    //     }

    //     float B1 = compute_B(i, k - 1, t);
    //     float B2 = compute_B(i - 1, k - 1, t);
    //     float B = B1 * (1 - t) + B2 * t;

    //     this->bitmap[i][k] = true;
    //     this->B[i][k] = B;

    //     return B;
    // }

    void init_C() {
        C = new double*[n + 1];
        for (int i = 0; i <= n; i++) {
            C[i] = new double[k + 1];
            if (i == 0) {
                for (int j = 1; j <= k; j++) {
                    C[i][j] = 0;
                }
                C[i][0] = 1;
            } else {
                for (int j = 0; j <= k; j++) {
                    C[i][j] = C[i - 1][j];
                    if (j > 0) {
                        C[i][j] += C[i - 1][j - 1];
                    }
                }
            }
        }
    }
    
    void delete_C() {
        for (int i = 0; i <= n; i++) {
            delete[] C[i];
        }
        delete[] C;
    }

    // int C(int n, int k) {
    //     // compute Cnk
    //     int result = 1;
    //     for (int i = 1; i <= k; i++) {
    //         result *= (n - i + 1);
    //     }
    //     for (int i = 1; i <= k; i++) {
    //         result /= i;
    //     }
    //     return result;
    // }

    float B(int i, int n, float t) {
        // compute Bi(t)
        return C[n][i] * pow(t, i) * pow(1 - t, n - i);
    }

    float dB(int i, int n, float t) {
        return n * (B(i-1, n-1, t) - B(i, n-1, t));
    }

    float lowerBound() {
        return 0;
    }

    float upperBound() {
        return 1;
    }

protected:
    // std::vector<float> knots;
    // std::vector<std::vector<float>> B;
    // std::vector<std::vector<bool>> bitmap;
};

class BsplineCurve : public Curve {
public:
    float* B, *dB;
    int n, k;

    BsplineCurve(const std::vector<Vector3f> &points) : Curve(points) {
        if (points.size() < 4) {
            printf("Number of control points of BspineCurve must be more than 4!\n");
            exit(0);
        }
        n = points.size() - 1;
        k = 3;  // for max pow of 3

        this->knots = std::vector<float>(n + k + 1, 0);
        for (int i = 0; i < n + k + 1; i++) {
            knots[i] = (float)i / (n + k + 1);
        }

        B = new float[n + k + 1];
        dB = new float[n + k + 1];
    }

    ~BsplineCurve() {
        delete[] B;
        delete[] dB;
    }

    CurvePoint getPoint(float phi) {
        CurvePoint cp;
        cp.V = Vector3f(0, 0, 0);
        cp.T = Vector3f(0, 0, 0);

        // for Ni1
        for (int i = 0; i < n + k + 1; i++) {
            if (phi >= knots[i] && phi < knots[i + 1]) {
                B[i] = 1;
            } else {
                B[i] = 0;
            }
        }

        // for Ni2, Ni3, ..., Nik
        for (int j = 1; j <= k; j++) {  // the final k loop for computing the k = 4
            for (int i = 0; i < n + k + 1; i++) {
                if (j == k) {
                    // compute dB
                    dB[i] = k * (B[i] / (knots[i + k] - knots[i]) - B[i + 1] / (knots[i + k + 1] - knots[i + 1]));
                }
                B[i] = B[i] * (phi - knots[i]) / (knots[i + j] - knots[i]) + B[i + 1] * (knots[i + j + 1] - phi) / (knots[i + j + 1] - knots[i + 1]);
            }
        }

        // compute V and T
        for (int i = 0; i <= n; i++) {
            cp.V += B[i] * controls[i];
            cp.T += dB[i] * controls[i];
        }
        return cp;
    }

    float lowerBound() {
        return knots[k];
    }

    float upperBound() {
        return knots[n + 1];
    }

    // void discretize(int resolution, std::vector<CurvePoint>& data) override {
    //     data.clear();
    //     // TODO (PA2): fill in data vector
    //     unsigned int n = controls.size() - 1;
    //     unsigned int k = 3;

    //     // all the ti 
    //     this->knots = std::vector<float>(n + k + 1, 0);
    //     for (int i = 0; i < n + k + 1; i++) {
    //         knots[i] = (float)i / (n + k + 1);
    //     }

    //     // step 
    //     float step = 1.0f / resolution / (n + k + 1);

    //     // recursion memoization
    //     this->B = std::vector<std::vector<float>>(n + k + 1, std::vector<float>(k + 1, 0));
    //     // bitmap 
    //     this->bitmap = std::vector<std::vector<bool>>(n + k + 1, std::vector<bool>(k + 1, false));

    //     // go through valid t
    //     for (int i = k; i < n + 1; i++) {
    //         // divide into resolution parts
    //         for (int j = 0; j < resolution; j++) {
    //             float tij = knots[i] + j * step;
    //             Vector3f point(0, 0, 0);
    //             Vector3f tangent(0, 0, 0);
                
    //             // clear bitmap
    //             for (int a = 0; a < n + k + 1; a++) {
    //                 for (int b = 0; b < k + 1; b++) {
    //                     this->bitmap[a][b] = false;
    //                 }
    //             }
                
    //             // recursion: compute all Bik
    //             for (int a = 0; a < n + 1; a++) {
    //                 Vector3f control = this->controls[a];
    //                 float Bik = compute_B(a, k, tij);
    //                 point += control * Bik;
    //                 tangent += control * k * (compute_B(a, k - 1, tij)/(knots[a + k] - knots[a]) - compute_B(a + 1, k - 1, tij)/(knots[a + k + 1] - knots[a + 1]));
    //             }

    //             CurvePoint cp;
    //             cp.V = point;
    //             cp.T = tangent.normalized();
    //             data.push_back(cp);
    //         }
    //     }
    // }

    // float compute_B(unsigned int i, unsigned int k, float t){
    //     if (k == 0) {
    //         if (t >= this->knots[i] && t < this->knots[i + 1]) {
    //             return 1;
    //         } else {
    //             return 0;
    //         }
            
    //     }

    //     if (this->bitmap[i][k]) {
    //         return this->B[i][k];
    //     }

    //     float B1 = compute_B(i, k - 1, t);
    //     float B2 = compute_B(i + 1, k - 1, t);
    //     float B = B1 * (t - this->knots[i]) / (this->knots[i + k] - this->knots[i]) + B2 * (this->knots[i + k + 1] - t) / (this->knots[i + k + 1] - this->knots[i + 1]);
    //     this->B[i][k] = B;
    //     this->bitmap[i][k] = true;
    //     return B;
    // }

protected:
    std::vector<float> knots;
    // std::vector<std::vector<float>> B;
    // std::vector<std::vector<bool>> bitmap;
};

#endif // CURVE_HPP
