#include <iostream>
#include <vector>
#include "marching_squares.hpp"
#include "renderer.hpp"

double test_circle(double x, double y)
{
    return x * x + y * y - 1.0;
}

int main()
{
    const int grid_size = 256;
    const double min_v = -10.0;
    const double max_v = 10.0;
    const int img_size = 1024;

    std::cout << "Running Marching Squares with cave_noise...\n";

    std::vector<LineSegment> lines = marching_squares(test_circle, grid_size, min_v, max_v);

    std::string output_file = "circle.png";
    render_to_image(lines, output_file, min_v, max_v, img_size, img_size);
    std::cout << "Image saved as: " << output_file << std::endl;
    return 0;
}
