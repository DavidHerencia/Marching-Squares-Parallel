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

vector<LineSegment> marching_squares(double (*f)(double, double), int grid_size, double min_v, double max_v);