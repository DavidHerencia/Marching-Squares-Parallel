# High-Performance Marching Squares

[![C++17](https://img.shields.io/badge/C++-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![OpenMP](https://img.shields.io/badge/Parallel-OpenMP-green.svg)](https://www.openmp.org/)
[![CUDA](https://img.shields.io/badge/GPU%20Accelerated-CUDA-brightgreen.svg)](https://developer.nvidia.com/cuda-zone)
[![Makefile](https://img.shields.io/badge/Build-Makefile-yellow.svg)](https://www.gnu.org/software/make/)

Este repositorio contiene una implementación de alto rendimiento del algoritmo **Marching Squares**, con un enfoque en la paralelización para arquitecturas multinúcleo (OpenMP) y de propósito general sobre GPU (CUDA). El proyecto no solo implementa el algoritmo, sino que también proporciona un análisis teórico y empírico de su rendimiento y escalabilidad.



---

## 🎯 Características Principales

-   **📈 Múltiples Versiones:** Implementaciones desde un enfoque secuencial base hasta versiones altamente optimizadas con OpenMP y CUDA.
-   **⚙️ Benchmarking Automatizado:** Scripts y `Makefile` para ejecutar pruebas de rendimiento, medir FLOPs y generar reportes en formato CSV.
-   **🖼️ Visualización Integrada:** Generación automática de imágenes PNG de alta calidad para cada contorno calculado.
-   **🧪 8 Funciones de Prueba:** Un conjunto diverso de funciones matemáticas, desde geometrías simples hasta ruido procedural (Simplex) y singularidades.
-   **📄 Análisis Teórico Profundo:** Formalización del algoritmo bajo el modelo PRAM y análisis de la escalabilidad teórica.

---

## 🚀 Guía de Inicio Rápido

### 1. Prerrequisitos

-   Compilador C++ con soporte para C++17 y OpenMP (`g++`).
-   NVIDIA CUDA Toolkit (`nvcc`) para la versión de GPU.
-   `make` para una compilación sencilla.

### 2. Compilación

Puedes compilar todas las versiones del algoritmo con un solo comando:

```bash
make all
```

O compilar una versión específica:

```bash
make it3  # Compila la versión optimizada con OpenMP
make it4  # Compila la versión con CUDA
```

### 3. Ejecución de Benchmarks

La forma más fácil de ver el proyecto en acción es ejecutar los benchmarks. Los resultados se guardarán automáticamente en el directorio `results/`.

```bash
# Ejecuta un conjunto completo de pruebas en la versión más optimizada
make benchmark-it3
```

El script ejecutará el programa con diferentes tamaños de grilla y número de hilos, generando archivos `.csv` y las imágenes de los contornos.

---

## 🔬 Análisis de Rendimiento y Escalabilidad

El rendimiento de este algoritmo se analizó teórica y empíricamente, obteniendo un modelo que predice su comportamiento.

### Modelo de Rendimiento

El tiempo de ejecución paralelo (`T_p`) se modeló considerando tanto el trabajo computacional como el overhead de la paralelización:

$T_p = \frac{n^2}{p} + p $

Donde `n` es la dimensión de la grilla y `p` es el número de procesadores. El término `+p` representa el costo adicional por la sincronización y la agregación de resultados.

### Relación Clave de Escalabilidad Débil

A partir de este modelo, la eficiencia paralela (`E`) es:

$E = \frac{T_s}{p \cdot T_p} = \frac{n^2}{p(\frac{n^2}{p} + p)} = \frac{1}{1 + \frac{p^2}{n^2}}$

Para mantener una eficiencia constante (el objetivo de la escalabilidad débil), la relación $\frac{p^2}{n^2}$ debe ser constante. Esto nos lleva a una conclusión clave del modelo:

**`n ∝ p`**

A diferencia de problemas con overhead despreciable (donde `n ∝ √p`), en esta implementación el tamaño del problema debe **crecer linealmente** con el número de procesadores para mantener la eficiencia, lo que demuestra la importancia de la fase de reducción de resultados.

---

## 🖼️ Galería de Resultados

Las imágenes generadas se almacenan automáticamente en `results/images/`.

| Checkerboard | Tangente | Cueva (Noise) |
| :----------: | :------: | :-----------: |
| ![Checkerboard](https://github.com/DavidHerencia/Marching-Squares-Parallel/blob/main/results/images/it3_p64_g1024_checkerboard_contour.png) | ![Tangent](https://github.com/DavidHerencia/Marching-Squares-Parallel/blob/main/results/images/it3_p64_g16384_tangent_contour.png) | ![Cave Noise](https://github.com/DavidHerencia/Marching-Squares-Parallel/blob/main/results/images/it3_p64_g8192_cave_contour.png) |

---

## 🛠️ Funciones de Prueba Implementadas

Se incluye un conjunto variado de funciones para probar el algoritmo en diferentes escenarios.

| Función      | Ecuación Matemática                     | Características           |
| :----------- | :-------------------------------------- | :------------------------ |
| **Circle**   | `x² + y² - 1.5 = 0`                     | Geometría simple          |
| **Checkerboard** | `sin(2πx)sin(2πy) = 0`                | Patrón periódico          |
| **Tangent**  | `tan(x² + y²) = 0`                      | Singularidades y densidad |
| **Noisy**    | `cos(2x)cos(πx) + sin(y)sin(πy) = 0`      | Multi-frecuencia        |
| **Wave**     | `sin(5x) + cos(5y) - 0.5 = 0`             | Interferencia de ondas    |
| **Spiral**   | `sin(4θ + 2r) - 0.3 = 0`                  | Coordenadas polares       |
| **Mandelbrot** | `x³ - 3xy² + y² - 1 = 0`                | Polinomio cúbico          |
| **Cave Noise** | `SimplexNoise(x,y) = 0`                 | Ruido procedural coherente|
