#include "../marching_squares.hpp"
#include "../simplex_noise.cuh"

#include <cuda_runtime.h>
#include <stdio.h>
#include <vector>
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


// Funciones device para Marching Squares
__device__ float cudaFromIndexSpace(int index, float min_v, float max_v, int grid_size)
{
    return min_v + (max_v - min_v) * float(index) / float(grid_size);
}

__device__ float cudaInterp(float A, float B)
{
    float diff = A - B;
    if (fabs(diff) < 1e-12)
        return 0.5; // Evitar división por cero
    return A / diff;
}

// Device versions of mathematical functions
__device__ float evaluateFunction(FunctionID funcId, float x, float y)
{
    const float PI = 3.14159265358979323846;
    
    switch (funcId) {
        case CIRCLE_FUNC:
            return x * x + y * y - 1.5;
            
        case CHECKERBOARD_FUNC:
            return sin(2 * PI * x) * sin(2 * PI * y);
            
        case TANGENT_FUNC:
            return tan(((x * x)) + (y * y));
            
        case NOISY_FUNC: {
            float fx = cos(2 * x) * cos(PI * x);
            float fy = sin(y) * sin(PI * y);
            return fx + fy;
        }
        
        case WAVE_FUNC:
            return sin(5 * x) + cos(5 * y) - 0.5;
            
        case SPIRAL_FUNC: {
            float r = sqrt(x * x + y * y);
            float theta = atan2(y, x);
            return sin(4 * theta + 2 * r) - 0.3;
        }
        
        case MANDELBROT_FUNC:
            return x * x * x - 3 * x * y * y + y * y - 1;
            
        case CAVE_NOISE_FUNC: {
            float3 p = make_float3(x, y, 0.0f);
            float noise = cudaNoise::simplexNoise(p, 0.5f, 42);
            return noise * 2.0f - 1.0f; // Normalizar a rango [-1, 1]
        }
            
        default:
            return x * x + y * y - 1.0; // Default to circle
    }
}

__global__ void cudaMarchingSquares(int grid_size, float min_v, float max_v, FunctionID funcId, CellOutput *d_cell_output)
{
    int i = blockIdx.y * blockDim.y + threadIdx.y; // fila
    int j = blockIdx.x * blockDim.x + threadIdx.x; // columna

    // VERIFICACIÓN CRÍTICA: Evitar acceso fuera de límites
    if (i >= grid_size || j >= grid_size)
        return;

    int cellIdx = i * grid_size + j;

    // VERIFICACIÓN ADICIONAL: Verificar bounds del array
    if (cellIdx >= grid_size * grid_size)
        return;

    const float DT = (fabs(min_v - max_v)) / float(grid_size);

    CellOutput currentCell;
    currentCell.line_count = 0;

    // Calculate the values at the corners of the grid cell
    float x_sw = cudaFromIndexSpace(j, min_v, max_v, grid_size);
    float x_ne = cudaFromIndexSpace(j + 1, min_v, max_v, grid_size);
    float y_sw = cudaFromIndexSpace(i, min_v, max_v, grid_size);
    float y_ne = cudaFromIndexSpace(i + 1, min_v, max_v, grid_size);

    float A = evaluateFunction(funcId, x_sw, y_sw); // 0 0
    float B = evaluateFunction(funcId, x_ne, y_sw); // 1 0
    float C = evaluateFunction(funcId, x_ne, y_ne); // 1 1
    float D = evaluateFunction(funcId, x_sw, y_ne); // 0 1

    float d_bottom = cudaInterp(A, B) * DT;
    float d_right = cudaInterp(B, C) * DT;
    float d_top = cudaInterp(C, D) * DT;
    float d_left = cudaInterp(D, A) * DT;

    // 4 NEIGHBOURS
    int c_case = ((D > 0) << 3) | ((C > 0) << 2) | ((B > 0) << 1) | (A > 0);

    // Lambda function to add line
    auto addLine = [&currentCell](float x1, float y1, float x2, float y2)
    {
        if (currentCell.line_count < 2)
        {
            currentCell.lines[currentCell.line_count] = CudaLineSegment(x1, y1, x2, y2, currentCell.line_count);
            currentCell.line_count++;
        }
    };

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

    d_cell_output[cellIdx] = currentCell;
}


vector<CellOutput> marching_squares(FunctionID f, int grid_size, double min_v, double max_v)
{
    // Determine which function we're using
    int numCells = grid_size * grid_size;
    size_t cellOutputSize = numCells * sizeof(CellOutput);
    CellOutput *d_cell_output;

    // VERIFICACIÓN CRÍTICA: Comprobar malloc
    cudaError_t mallocError = cudaMalloc((void **)&d_cell_output, cellOutputSize);
    if (mallocError != cudaSuccess)
    {
        printf("ERROR CRÍTICO: cudaMalloc falló: %s\n", cudaGetErrorString(mallocError));
        return std::vector<CellOutput>(); // Retornar vector vacío en caso de error
    }

    // Configuración de bloques y threads
    int threadsPerBlockX = 16;
    int threadsPerBlockY = 16;
    dim3 dimBlock(threadsPerBlockX, threadsPerBlockY);
    dim3 dimGrid(
        (grid_size + threadsPerBlockX - 1) / threadsPerBlockX,
        (grid_size + threadsPerBlockY - 1) / threadsPerBlockY);

    // Ejecutar kernel de marching squares
    cudaMarchingSquares<<<dimGrid, dimBlock>>>(grid_size, min_v, max_v, f, d_cell_output);

    // VERIFICACIÓN CRÍTICA: Comprobar errores del kernel
    cudaError_t kernelError = cudaGetLastError();
    if (kernelError != cudaSuccess)
    {
        printf("ERROR CRÍTICO: Kernel falló: %s\n", cudaGetErrorString(kernelError));
        cudaFree(d_cell_output);
        return std::vector<CellOutput>();
    }

    cudaDeviceSynchronize();

    // VERIFICACIÓN CRÍTICA: Comprobar sincronización
    cudaError_t syncError = cudaGetLastError();
    if (syncError != cudaSuccess)
    {
        printf("ERROR CRÍTICO: Sincronización falló: %s\n", cudaGetErrorString(syncError));
        cudaFree(d_cell_output);
        return std::vector<CellOutput>();
    }

    std::vector<CellOutput> h_cell_output(numCells);

    // VERIFICACIÓN CRÍTICA: Comprobar memcpy
    cudaError_t copyError = cudaMemcpy(h_cell_output.data(), d_cell_output, cellOutputSize, cudaMemcpyDeviceToHost);
    if (copyError != cudaSuccess)
    {
        printf("ERROR CRÍTICO: cudaMemcpy falló: %s\n", cudaGetErrorString(copyError));
        cudaFree(d_cell_output);
        return std::vector<CellOutput>();
    }
    cudaFree(d_cell_output);
    return h_cell_output;
}

/*
int main() {
    int grid_size = 512;  // Tamaño del grid para marching squares
    float min_v = -2.0;
    float max_v = 2.0;
    printf("Ejecutando Marching Squares en CUDA con grid %d x %d...\n", grid_size, grid_size);
    // Verificar errores de CUDA
    cudaError_t error = cudaGetLastError();
    if (error != cudaSuccess) {
        printf("Error CUDA: %s\n", cudaGetErrorString(error));
        return -1;
    }
    // Copiar resultados de vuelta al host
    // Contar líneas generadas
    int totalLines = 0;
    int cellsWithLines = 0;
    for (int i = 0; i < numCells; ++i) {
        if (h_cell_output[i].line_count > 0) {
            cellsWithLines++;
            totalLines += h_cell_output[i].line_count;
        }
    }
    printf("\nResultados:\n");
    printf("Total de celdas con líneas: %d\n", cellsWithLines);
    printf("Total de líneas generadas: %d\n", totalLines);
    // Mostrar algunas líneas de ejemplo (primeras 10)
    printf("\nPrimeras 10 líneas generadas:\n");
    int linesShown = 0;
    for (int i = 0; i < numCells && linesShown < 10; ++i) {
        if (h_cell_output[i].line_count > 0) {
            int row = i / grid_size;
            int col = i % grid_size;
            for (int j = 0; j < h_cell_output[i].line_count && linesShown < 10; ++j) {
                LineSegment line = h_cell_output[i].lines[j];
                printf("Celda (%d,%d) - Línea %d: (%.3f,%.3f) -> (%.3f,%.3f)\n",
                       row, col, j, line.p1.x, line.p1.y, line.p2.x, line.p2.y);
                linesShown++;
            }
        }
    }
    // Renderizar a imagen
    printf("\nGenerando imagen de resultado...\n");
    render_cuda_results(h_cell_output, grid_size, "cuda_marching_squares.png", min_v, max_v, 1024, 1024);
    printf("Imagen guardada como: cuda_marching_squares.png\n");
    return 0;
}
*/