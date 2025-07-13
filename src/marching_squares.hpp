#pragma once
#include <deque>
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
    deque<LineSegment> marching_squares(double (*f)(double, double), int grid_size, double min_v, double max_v);
#else
    #ifndef __CUDACC__
    #define __host__
    #define __device__
    #endif
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
    
    struct CudaLineSegment
    {
        float x1, y1, x2, y2; // Coordenadas de los extremos
        int id;

        __host__ __device__ CudaLineSegment() : x1(0.0), y1(0.0), x2(0.0), y2(0.0), id(-1) {}
        __host__ __device__ CudaLineSegment(float a, float b, float c, float d, int segment_id = -1)
            : x1(a), y1(b), x2(c), y2(d), id(segment_id) {}
    };
    struct CellOutput
    {
        CudaLineSegment lines[2]; // Máximo 2 líneas por celda en marching squares

        int line_count;
        __host__ __device__ CellOutput() : line_count(0) {}
    };

    vector<CellOutput> marching_squares(FunctionID func, int grid_size, double min_v, double max_v);
#endif