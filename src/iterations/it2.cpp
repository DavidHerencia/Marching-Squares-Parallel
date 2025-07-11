#include "../marching_squares.hpp"
#include <iostream>
#include <omp.h>
using namespace std;

vector<LineSegment> marching_squares(double (*f)(double, double), int grid_size, double min_v, double max_v)
{
    vector<LineSegment> lines; // Buffer to store line segments
    const double DT = (abs(min_v - max_v)) / (grid_size);

    #pragma omp parallel
    {
        vector<LineSegment> local_lines; 

        auto addLine = [&local_lines](double x1, double y1, double x2, double y2)
        {
            local_lines.push_back({x1, y1, x2, y2});
        };

        #pragma omp for schedule(static)
        for (int i = 0; i < grid_size; i++)
        {
            for (int j = 0; j < grid_size; j++)
            {
                // Calculate the values at the corners of the grid cell
                double x_sw = fromIndexSpace(j, min_v, max_v, grid_size);
                double x_ne = fromIndexSpace(j + 1, min_v, max_v, grid_size);
                double y_sw = fromIndexSpace(i, min_v, max_v, grid_size);
                double y_ne = fromIndexSpace(i + 1, min_v, max_v, grid_size);

                double A = f(x_sw, y_sw); // 0 0
                double B = f(x_ne, y_sw); // 1 0
                double C = f(x_ne, y_ne); // 1 1
                double D = f(x_sw, y_ne); // 0 1

                double d_bottom = interp(A, B) * DT, d_right = interp(B, C) * DT, d_top = interp(C, D) * DT, d_left = interp(D, A) * DT;

                // 4 NEIGHBOURS
                int c_case = (D > 0) << 3; // 8
                c_case |= (C > 0) << 2;    // 4
                c_case |= (B > 0) << 1;    // 2
                c_case |= (A > 0);         // 1

                // Now do cases
                switch (c_case)
                {
                case 0:
                case 15:
                    break;
                case 1:
                case 14:
                    addLine(x_sw, y_ne - d_left, x_sw + d_bottom, y_sw);
                    break;
                case 2:
                case 13:
                    addLine(x_sw + d_bottom, y_sw, x_ne, y_sw + d_right);
                    break;
                case 3:
                case 12:
                    addLine(x_sw, y_ne - d_left, x_ne, y_sw + d_right);
                    break;
                case 4:
                case 11:
                    addLine(x_ne - d_top, y_ne, x_ne, y_sw + d_right);
                    break;
                case 5:
                    addLine(x_sw, y_ne - d_left, x_ne - d_top, y_ne);
                    addLine(x_sw + d_bottom, y_sw, x_ne, y_sw + d_right);
                    break;
                case 6:
                case 9:
                    addLine(x_sw + d_bottom, y_sw, x_ne - d_top, y_ne);
                    break;

                case 7:
                case 8:
                    addLine(x_sw, y_ne - d_left, x_ne - d_top, y_ne);
                    break;
                case 10:
                    addLine(x_sw + d_bottom, y_ne, x_ne, y_sw + d_right);
                    addLine(x_sw, y_ne - d_left, x_sw + d_bottom, y_sw);
                    break;
                }
            }
        }
        
        // Merge local results into the global vector (critical section)
        #pragma omp critical
        {
            lines.insert(lines.end(), local_lines.begin(), local_lines.end());
        }
    }

    return lines; // Return the buffer containing all line segments
}