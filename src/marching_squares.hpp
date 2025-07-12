#pragma once
#include <vector>
using namespace std;

inline double fromIndexSpace(int v, double min_v, double max_v, int grid_size)
{
    return min_v + (max_v - min_v) * v / (grid_size);
}

inline double interp(double A, double B)
{
    return (-A) / (B - A);
}

struct LineSegment {
    double x1, y1, x2, y2; // Coordinates of the endpoints
};


#ifndef CUDA_IMPLEMENTATION
    vector<LineSegment> marching_squares(double (*f)(double, double), int grid_size, double min_v, double max_v);
#else
    // Function IDs for different mathematical functions
    enum FunctionID {
        CIRCLE_FUNC = 0,
        CHECKERBOARD_FUNC = 1,
        TANGENT_FUNC = 2,
        NOISY_FUNC = 3,
        WAVE_FUNC = 4,
        SPIRAL_FUNC = 5,
        MANDELBROT_FUNC = 6,
        CAVE_NOISE_FUNC = 7
    };
    vector<LineSegment> marching_squares(FunctionID func, int grid_size, double min_v, double max_v);
#endif