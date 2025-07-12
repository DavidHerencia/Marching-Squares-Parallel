# Marching Squares Parallel Implementation

[![OpenMP](https://img.shields.io/badge/OpenMP-Parallel-green.svg)](https://www.openmp.org/)
[![CUDA](https://img.shields.io/badge/CUDA-GPU_Accelerated-green.svg)](https://developer.nvidia.com/cuda-zone)
[![C++17](https://img.shields.io/badge/C++-17-blue.svg)](https://en.cppreference.com/w/cpp/17)

## 🚀 Descripción del Proyecto

Una implementación de alto rendimiento del algoritmo **Marching Squares** utilizando técnicas de paralelización avanzadas. Este proyecto explora diferentes estrategias de paralelización en el contexto del modelo **PRAM (Parallel Random Access Machine)** y proporciona análisis exhaustivos de rendimiento incluyendo mediciones de FLOPS y comparaciones arquitectónicas.

### 🎯 Características Principales

- **4 Implementaciones Paralelas**: Desde secuencial hasta CUDA GPU
- **Análisis PRAM Teórico**: Formalización matemática del algoritmo
- **8 Funciones de Prueba**: Desde círculos simples hasta funciones complejas con ruido
- **Benchmarking Exhaustivo**: Medición de FLOPS, escalabilidad y rendimiento
- **Visualización Automática**: Generación de imágenes de contornos en PNG
- **Análisis CSV**: Exportación de métricas de rendimiento para análisis posterior

## 📊 Resultados de Rendimiento

### Resumen de Implementaciones

| Versión | Técnica | Máx FLOPS/s | Máx Líneas/s | Escalabilidad |
|---------|---------|-------------|---------------|---------------|
| **it3** | OpenMP Optimizado | **~2.8T** | **~21.7M** | **~130x** |
| **it4** | CUDA GPU | En desarrollo | En desarrollo | ~1000x+ |

### Funciones Más Desafiantes

1. **Tangent Function** (`tan(x² + y²)`): 65,152 líneas, 2.8T FLOPS/s
2. **Checkerboard Pattern** (`sin(2πx)sin(2πy)`): 39,858 líneas, 1.3T FLOPS/s  
3. **Cave Noise** (Simplex): 30,284 líneas, 1.9T FLOPS/s

## 🔬 Algoritmo Marching Squares

### Fundamentos Teóricos

El algoritmo Marching Squares es una técnica de gráficos por computadora para generar contornos de isolíneas a partir de campos escalares 2D. Dado una función `f(x,y)` y un valor de contorno `c`, el algoritmo encuentra todas las líneas donde `f(x,y) = c`.

### Modelo PRAM (Parallel Random Access Machine)

```mathematica
INPUT: f(x,y), grid_size, min_v, max_v
OUTPUT: R = {((x₁,y₁), (x₂,y₂)) ∈ ℝ² × ℝ² | f(x₁,y₁) = f(x₂,y₂) = c}

PARALLEL ALGORITHM:
  ΔT = (max_v - min_v) / grid_size
  lines = ∅  // Global result set
  
  for i=1 to grid_size pardo:
    for j=1 to grid_size:
      // Evaluar función en las 4 esquinas de la celda
      A = f(x_sw, y_sw)  B = f(x_ne, y_sw) 
      C = f(x_ne, y_ne)  D = f(x_sw, y_ne)
      
      // Determinar configuración (16 casos posibles)
      case = (D>0)<<3 | (C>0)<<2 | (B>0)<<1 | (A>0)
      
      // Generar segmentos según caso
      switch(case): generate_line_segments()
      
      // Sección crítica para agregar resultados
      critical_section: lines = lines ∪ {new_segments}
```

### Complejidad Teórica

- **Trabajo Total**: O(grid_size²)
- **Tiempo Paralelo**: O(grid_size²/p + log p) donde p = procesadores
- **Eficiencia**: O(grid_size²/(grid_size²/p + p·log p))
- **Escalabilidad Óptima**: p ≤ grid_size²/log(grid_size²)


## 🎨 Funciones de Prueba Implementadas

| Función | Ecuación Matemática | Complejidad | Características |
|---------|-------------------|-------------|-----------------|
| **Circle** | `x² + y² - 1.5 = 0` | 4 FLOPS | Geometría simple |
| **Checkerboard** | `sin(2πx)sin(2πy) = 0` | 5 FLOPS | Patrón periódico |
| **Tangent** | `tan(x² + y²) = 0` | 6 FLOPS | Singularidades |
| **Noisy** | `cos(2x)cos(πx) + sin(y)sin(πy) = 0` | 8 FLOPS | Multi-frecuencia |
| **Wave** | `sin(5x) + cos(5y) - 0.5 = 0` | 6 FLOPS | Interferencia |
| **Spiral** | `sin(4θ + 2r) - 0.3 = 0` | 10 FLOPS | Coordenadas polares |
| **Mandelbrot** | `x³ - 3xy² + y² - 1 = 0` | 9 FLOPS | Polinomio cúbico |
| **Cave Noise** | `SimplexNoise(x,y) = 0` | 7 FLOPS | Ruido procedural |

## 🛠️ Compilación y Uso

### Prerrequisitos

```bash
# Compiladores requeridos
g++ (con soporte OpenMP)
nvcc (NVIDIA CUDA Toolkit) # Para versión GPU

# Librerías
OpenMP
CUDA Runtime (opcional)
```

### Compilación

```bash
# Compilar todas las versiones
make all

# Compilar versiones específicas
make it1  # Secuencial
make it2  # OpenMP básico  
make it3  # OpenMP optimizado
make it4  # CUDA GPU

# Limpiar archivos compilados
make clean
```

### Ejecución

```bash
# Ejecutar con parámetros por defecto (resultados van a results/)
./bin/marching_squares_it3

# Ejecutar con parámetros personalizados
./bin/marching_squares_it3 <grid_size> <image_size> [csv_output]

# Ejemplos con salida organizada en results/
./bin/marching_squares_it3 512 1024                    # Grid 512×512, imagen 1024×1024
./bin/marching_squares_it3 256 800 results/benchmarks/test.csv  # Con exportación CSV
```

### Benchmarking Automatizado

```bash
# Ejecutar benchmarks completos (resultados en results/)
make benchmark

# Benchmarks por versión con organización automática
make benchmark-it1  # → results/benchmarks/scalability_it1_[timestamp].csv
make benchmark-it2  # → results/benchmarks/scalability_it2_[timestamp].csv  
make benchmark-it3  # → results/benchmarks/scalability_it3_[timestamp].csv
make benchmark-it4  # → results/benchmarks/scalability_it4_[timestamp].csv

# Script automatizado con organización completa
python scripts/run_benchmarks.py --all --organize-results
```

## 🖼️ Galería de Resultados

Las imágenes generadas se almacenan automáticamente en `results/images/`:

- `it3_p16_g256_circle_contour.png` - Contorno circular simple
- `it3_p16_g256_checkerboard_contour.png` - Patrón de tablero de ajedrez
- `it3_p16_g256_tangent_contour.png` - Función tangente con singularidades
- `it3_p16_g256_spiral_contour.png` - Espiral logarítmica compleja
- `it3_p16_g256_mandelbrot_contour.png` - Curva algebraica cúbica
- `it3_p16_g256_cave_contour.png` - Generación procedural de cuevas