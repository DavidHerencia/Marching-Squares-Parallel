#include <iostream>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <deque>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <omp.h>
#include "marching_squares.hpp"
#include "renderer.hpp"
#include "simplex_noise.hpp"

#define PI 3.14159265358979323846

#ifndef IT_NAME
#define IT_NAME "UNDEFINED_VERSION"
#endif

// Macro helper to convert IT_NAME to string
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

// Function to get implementation name as string
std::string get_implementation_name()
{
    return TOSTRING(IT_NAME);
}

// Structure to hold performance data for CSV output
struct PerformanceRecord
{
    std::string implementation_version;
    std::string function_name;
    int grid_size;
    int processor_count;
    double execution_time;
    unsigned long long total_flops;
    double flops_rate;
    unsigned long long lines_generated;
    double lines_per_second;
    std::string timestamp;
};

// Compute flops
const int POINTS_TO_CHECK = 4;
const int OP_PER_SPACE_CONVERTION = 4;
const int OP_PER_INTERPOLATION = 3;
const int OP_PER_LINE = 2;

unsigned long long get_FLOP(unsigned long long grid_size, unsigned long long op_per_function, unsigned long long n_lines)
{
    return grid_size * grid_size * ((POINTS_TO_CHECK * op_per_function) + // Function evaluations
                                    (OP_PER_SPACE_CONVERTION * 4) +       // Space conversions
                                    (OP_PER_INTERPOLATION * 4) +          // Interpolations
                                    (n_lines * OP_PER_LINE)               // Line segment calculations
                                   );
}

// Function to get current timestamp string
std::string get_timestamp()
{
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// Function to write CSV header only if file doesn't exist
void write_csv_header(const std::string &filename)
{
    std::ifstream check_file(filename);
    if (!check_file.good()) {
        // File doesn't exist, create it with header
        std::ofstream file(filename);
        if (file.is_open())
        {
            file << "IMPLEMENTATION,FUNCTION,GRIDSIZE,PROCESSORS,TIME,FLOPS,FLOPS_RATE,LINES,LINES_RATE,TIMESTAMP\n";
            file.close();
        }
    }
    check_file.close();
}

// Function to append performance record to CSV
void append_to_csv(const std::string &filename, const PerformanceRecord &record)
{
    std::ofstream file(filename, std::ios::app);
    if (file.is_open())
    {
        file << std::fixed << std::setprecision(6);
        file << "\"" << record.implementation_version << "\","
             << "\"" << record.function_name << "\","
             << record.grid_size << ","
             << record.processor_count << ","
             << record.execution_time << ","
             << record.total_flops << ","
             << record.flops_rate << ","
             << record.lines_generated << ","
             << record.lines_per_second << ","
             << "\"" << record.timestamp << "\"\n";
        file.close();
    }
}

const std::string dir = "images/"; // Directory to save images


#ifndef CUDA_IMPLEMENTATION
double circle_function(double x, double y)
{
    return x * x + y * y - 1.5;
}

double checker_board_function(double x, double y)
{
    return sin(2 * PI * x) * sin(2 * PI * y);
}

double tanXY_function(double x, double y)
{
    return tan(((x * x)) + (y * y));
}

double noisy_func(double x, double y)
{
    // x *= 4; // Scale x to increase complexity
    // y *= 4; // Scale y to increase complexity
    double fx = cos(2 * x) * cos(PI * x);
    double fy = sin(y) * sin(PI * y);
    return fx + fy;
}

// Additional test functions for variety
double wave_function(double x, double y)
{
    return sin(5 * x) + cos(5 * y) - 0.5;
}

double spiral_function(double x, double y)
{
    double r = sqrt(x * x + y * y);
    double theta = atan2(y, x);
    return sin(4 * theta + 2 * r) - 0.3;
}

double mandelbrot_like(double x, double y)
{
    return x * x * x - 3 * x * y * y + y * y - 1;
}

// CAVE GENERATION
double cave_noise(double x, double y)
{
    return SimplexNoise::noise(x, y);
}

// Structure to hold test function information
struct TestFunction
{
    std::string name;
    std::string description;
    double (*function)(double, double);
    unsigned long long estimated_flops;
    std::string filename;
};

// Dynamic test runner function
void run_test(const TestFunction &test, int grid_size, double min_v, double max_v, int img_size,
              const std::string &csv_filename = "", bool save_csv = false)
{
    std::cout << "\n--- " << test.name << " ---" << std::endl;
    std::cout << "Description: " << test.description << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    deque<LineSegment> lines = marching_squares(test.function, grid_size, min_v, max_v);
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed = end - start;
    unsigned long long n_lines = lines.size();
    unsigned long long flops = get_FLOP(grid_size, test.estimated_flops, n_lines);
    double flops_rate = flops / elapsed.count();
    double lines_per_second = n_lines / elapsed.count();
    int processor_count = omp_get_max_threads();
    std::cout << "Results: " << n_lines << " lines | " << elapsed.count() << "s | " << flops_rate << " FLOPS | " << lines_per_second << " lines/s" << std::endl;
    
    //Render
    std::string file_n = get_implementation_name() + string("_p") + std::to_string(processor_count) + "_g" + std::to_string(grid_size) + "_" + test.filename;
    render_to_image(lines, dir + file_n, min_v, max_v, img_size, img_size);
    std::cout << "Image saved as: " << file_n << std::endl;

    // Save to CSV if requested
    if (save_csv && !csv_filename.empty())
    {
        PerformanceRecord record;
        record.implementation_version = get_implementation_name();
        record.function_name = test.name;
        record.grid_size = grid_size;
        record.processor_count = processor_count;
        record.execution_time = elapsed.count();
        record.total_flops = flops;
        record.flops_rate = flops_rate;
        record.lines_generated = n_lines;
        record.lines_per_second = lines_per_second;
        record.timestamp = get_timestamp();

        append_to_csv(csv_filename, record);
        std::cout << "Performance data saved to: " << csv_filename << std::endl;
    }
}

int main(int argc, char *argv[])
{
    int grid_size = (1 << 8); // Default grid resolution
    double min_v = -10.0;      // Minimum coordinate value
    double max_v = 10.0;       // Maximum coordinate value
    int img_size = 1024;       // Default image size
    std::string csv_filename = "performance_results_.csv";
    bool save_csv = true;

    // Parse command line arguments
    if (argc > 1)
    {
        grid_size = std::atoi(argv[1]);
        if (grid_size <= 0)
        {
            std::cerr << "Error: Grid size must be a positive integer." << std::endl;
            return 1;
        }
    }
    if (argc > 2)
    {
        img_size = std::atoi(argv[2]);
        if (img_size <= 0)
        {
            std::cerr << "Error: Image size must be a positive integer." << std::endl;
            return 1;
        }
    }
    if (argc > 3)
    {
        // Third argument enables CSV output and optionally specifies filename
        save_csv = true;
        std::string csv_arg = argv[3];
        if (csv_arg != "csv" && csv_arg != "CSV")
        {
            csv_filename = csv_arg; // Use custom filename if provided
        }
    }

    std::cout << "=== Marching Squares Parallel Test Suite ===" << std::endl;
    std::cout << "Implementation: " << get_implementation_name() << std::endl;
    std::cout << "Grid size: " << grid_size << "x" << grid_size << std::endl;
    std::cout << "Domain: [" << min_v << ", " << max_v << "]" << std::endl;
    std::cout << "Image size: " << img_size << "x" << img_size << std::endl;
    std::cout << "OpenMP threads (p): " << omp_get_max_threads() << std::endl;

    if (save_csv)
    {
        std::cout << "CSV output enabled: " << csv_filename << std::endl;
        // Initialize CSV file with header
        write_csv_header(csv_filename);
    }

    // Define all available test functions
    std::vector<TestFunction> tests = {
        {"Circle Function", "x² + y² - 1.5 = 0 (Simple polynomial)", circle_function, 4, "circle_contour.png"},
        {"Checkerboard Pattern", "sin(2πx) * sin(2πy) = 0 (Trigonometric grid)", checker_board_function, 5, "checkerboard_contour.png"},
        {"Tangent Function", "tan(10x² + 10y²) = 0 (Complex transcendental)", tanXY_function, 6, "tangent_contour.png"},
        {"Noisy Function", "cos(2x)cos(πx) + sin(y)sin(πy) = 0 (Multi-frequency)", noisy_func, 8, "noisy_contour.png"},
        {"Wave Function", "sin(5x) + cos(5y) - 0.5 = 0 (Wave interference)", wave_function, 6, "wave_contour.png"},
        {"Spiral Function", "sin(4θ + 2r) - 0.3 = 0 (Polar coordinates)", spiral_function, 10, "spiral_contour.png"},
        {"Mandelbrot-like", "x³ - 3xy² + y² - 1 = 0 (Cubic polynomial)", mandelbrot_like, 9, "mandelbrot_contour.png"},
        {"cave_noise", "Cave generation function (Complex noise)", cave_noise, 7, "cave_contour.png"}};

    // Get user selection (interactive mode) or run all (batch mode)
    std::vector<int> selection;

    // Batch mode - run all tests
    for (size_t i = 0; i < tests.size(); i++)
    {
        selection.push_back(i);
    }
    std::cout << "\nRunning all " << tests.size() << " tests in batch mode..." << std::endl;

    // Performance summary variables
    double total_time = 0.0;
    unsigned long long total_lines = 0;
    unsigned long long total_flops = 0;

    // Run selected tests
    for (int test_idx : selection)
    {
        auto start_total = std::chrono::high_resolution_clock::now();
        run_test(tests[test_idx], grid_size, min_v, max_v, img_size, csv_filename, save_csv);
        auto end_total = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double> test_time = end_total - start_total;
        total_time += test_time.count();
    }

    std::cout << "\nAll images saved to current directory." << std::endl;
    if (save_csv)
    {
        std::cout << "Performance data saved to: " << csv_filename << std::endl;
    }
    return 0;
}
#else 
struct TestCUDAFunction
{
    std::string name;
    std::string description;
    FunctionID function; // Use FunctionID for CUDA compatibility
    unsigned long long estimated_flops;
    std::string filename;
};

// Dynamic test runner function
void run_test(const TestCUDAFunction &test, int grid_size, double min_v, double max_v, int img_size,
              const std::string &csv_filename = "", bool save_csv = false)
{
    std::cout << "\n--- " << test.name << " ---" << std::endl;
    std::cout << "Description: " << test.description << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    deque<CellOutput> cudaLines = marching_squares(test.function, grid_size, min_v, max_v);
    auto end = std::chrono::high_resolution_clock::now();

    //Now convert CellOutput to LineSegment
    deques<LineSegment> lines;
    for (auto &cell : cudaLines)
    {
        for (int i = 0; i < cell.line_count; i++)
        {
            CudaLineSegment &line = cell.lines[i];
            lines.push_back({line.x1, line.y1, line.x2, line.y2});
        }
    }

    std::chrono::duration<double> elapsed = end - start;
    unsigned long long n_lines = lines.size();
    unsigned long long flops = get_FLOP(grid_size, test.estimated_flops, n_lines);
    double flops_rate = flops / elapsed.count();
    double lines_per_second = n_lines / elapsed.count();
    int processor_count = omp_get_max_threads();
    std::cout << "Results: " << n_lines << " lines | " << elapsed.count() << "s | " << flops_rate << " FLOPS | " << lines_per_second << " lines/s" << std::endl;
    
    //Render
    std::string file_n = get_implementation_name() + string("_p") + std::to_string(processor_count) + "_g" + std::to_string(grid_size) + "_" + test.filename;
    render_to_image(lines, dir + file_n, min_v, max_v, img_size, img_size);
    std::cout << "Image saved as: " << file_n << std::endl;

    // Save to CSV if requested
    if (save_csv && !csv_filename.empty())
    {
        PerformanceRecord record;
        record.implementation_version = get_implementation_name();
        record.function_name = test.name;
        record.grid_size = grid_size;
        record.processor_count = processor_count;
        record.execution_time = elapsed.count();
        record.total_flops = flops;
        record.flops_rate = flops_rate;
        record.lines_generated = n_lines;
        record.lines_per_second = lines_per_second;
        record.timestamp = get_timestamp();

        append_to_csv(csv_filename, record);
        std::cout << "Performance data saved to: " << csv_filename << std::endl;
    }
}

// CUDA implementation just like the same but only iterate over the grid and call the CUDA kernel
int main(int argc, char *argv[])
{
    int grid_size = (1 << 8); // Default grid resolution
    double min_v = -10.0;      // Minimum coordinate value
    double max_v = 10.0;       // Maximum coordinate value
    int img_size = 1024;       // Default image size
    std::string csv_filename = "performance_results_.csv";
    bool save_csv = true;

    // Parse command line arguments
    if (argc > 1)
    {
        grid_size = std::atoi(argv[1]);
        if (grid_size <= 0)
        {
            std::cerr << "Error: Grid size must be a positive integer." << std::endl;
            return 1;
        }
    }
    if (argc > 2)
    {
        img_size = std::atoi(argv[2]);
        if (img_size <= 0)
        {
            std::cerr << "Error: Image size must be a positive integer." << std::endl;
            return 1;
        }
    }
    if (argc > 3)
    {
        // Third argument enables CSV output and optionally specifies filename
        save_csv = true;
        std::string csv_arg = argv[3];
        if (csv_arg != "csv" && csv_arg != "CSV")
        {
            csv_filename = csv_arg; // Use custom filename if provided
        }
    }

    std::cout << "=== Marching Squares Parallel Test Suite - CUDA VERSION===" << std::endl;
    std::cout << "Implementation: " << get_implementation_name() << std::endl;
    std::cout << "Grid size: " << grid_size << "x" << grid_size << std::endl;
    std::cout << "Domain: [" << min_v << ", " << max_v << "]" << std::endl;
    std::cout << "Image size: " << img_size << "x" << img_size << std::endl;

    if (save_csv)
    {
        std::cout << "CSV output enabled: " << csv_filename << std::endl;
        // Initialize CSV file with header
        write_csv_header(csv_filename);
    }

    vector<TestCUDAFunction> tests = {
        {"Circle Function", "x² + y² - 1.5 = 0 (Simple polynomial)", CIRCLE_FUNC, 4, "circle_contour.png"},
        {"Checkerboard Pattern", "sin(2πx) * sin(2πy) = 0 (Trigonometric grid)", CHECKERBOARD_FUNC, 5, "checkerboard_contour.png"},
        {"Tangent Function", "tan(10x² + 10y²) = 0 (Complex transcendental)", TANGENT_FUNC, 6, "tangent_contour.png"},
        {"Noisy Function", "cos(2x)cos(πx) + sin(y)sin(πy) = 0 (Multi-frequency)", NOISY_FUNC, 8, "noisy_contour.png"},
        {"Wave Function", "sin(5x) + cos(5y) - 0.5 = 0 (Wave interference)", WAVE_FUNC, 6, "wave_contour.png"},
        {"Spiral Function", "sin(4θ + 2r) - 0.3 = 0 (Polar coordinates)", SPIRAL_FUNC, 10, "spiral_contour.png"},
        {"Mandelbrot-like", "x³ - 3xy² + y² - 1 = 0 (Cubic polynomial)", MANDELBROT_FUNC, 9, "mandelbrot_contour.png"},
        {"cave_noise", "Cave generation function (Complex noise)", CAVE_NOISE_FUNC, 7, "cave_contour.png"}};

    // Get user selection (interactive mode) or run all (batch mode)
    std::vector<int> selection;
    // Batch mode - run all tests
    for (size_t i = 0; i < tests.size(); i++)
    {
        selection.push_back(i);
    }
    std::cout << "\nRunning all " << tests.size() << " tests in batch mode..." << std::endl;

    // Performance summary variables
    double total_time = 0.0;
    unsigned long long total_lines = 0;
    unsigned long long total_flops = 0;

    // Run selected tests
    for (int test_idx : selection)
    {
        auto start_total = std::chrono::high_resolution_clock::now();
        run_test(tests[test_idx], grid_size, min_v, max_v, img_size, csv_filename, save_csv);
        auto end_total = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double> test_time = end_total - start_total;
        total_time += test_time.count();
    }

    std::cout << "\nAll images saved to current directory." << std::endl;
    if (save_csv)
    {
        std::cout << "Performance data saved to: " << csv_filename << std::endl;
    }
    return 0;

} 
#endif