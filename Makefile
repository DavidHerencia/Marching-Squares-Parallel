# Makefile for Marching Squares Parallel Project
CXX = g++
NVCC = nvcc
CXXFLAGS = -std=c++17 -fopenmp
NVCCFLAGS = -std=c++17 -Xcompiler -fopenmp

# CUDA architecture (adjust based on your GPU)
CUDA_ARCH = -arch=sm_75

# Implementation flags
IT1_FLAGS = -DIT_NAME="it1"
IT2_FLAGS = -DIT_NAME="it2" 
IT3_FLAGS = -DIT_NAME="it3"
IT4_FLAGS = -DIT_NAME="it4" -DCUDA_IMPLEMENTATION

# Directories
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin
ITERATIONS_DIR = $(SRC_DIR)/iterations

# Executables
TARGET_IT1 = $(BIN_DIR)/marching_squares_it1
TARGET_IT2 = $(BIN_DIR)/marching_squares_it2
TARGET_IT3 = $(BIN_DIR)/marching_squares_it3
TARGET_IT4 = $(BIN_DIR)/marching_squares_it4

# Default target
all: it1 it2 it3 it4

# Build targets
it1: $(TARGET_IT1)
it2: $(TARGET_IT2)
it3: $(TARGET_IT3)
it4: $(TARGET_IT4)

# Create directories
$(BUILD_DIR) $(BIN_DIR):
	mkdir -p $@
	mkdir -p $@/images

# Build executables directly (simplified)
$(TARGET_IT1): $(SRC_DIR)/main.cpp $(ITERATIONS_DIR)/it1.cpp $(SRC_DIR)/renderer.cpp $(SRC_DIR)/simplex_noise.cpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(IT1_FLAGS) -o $@ $^

$(TARGET_IT2): $(SRC_DIR)/main.cpp $(ITERATIONS_DIR)/it2.cpp $(SRC_DIR)/renderer.cpp $(SRC_DIR)/simplex_noise.cpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(IT2_FLAGS) -o $@ $^

$(TARGET_IT3): $(SRC_DIR)/main.cpp $(ITERATIONS_DIR)/it3.cpp $(SRC_DIR)/renderer.cpp $(SRC_DIR)/simplex_noise.cpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(IT3_FLAGS) -o $@ $^

# CUDA build target for it4
$(TARGET_IT4): $(SRC_DIR)/main.cpp $(ITERATIONS_DIR)/it4.cu $(SRC_DIR)/renderer.cpp $(SRC_DIR)/simplex_noise.cpp | $(BIN_DIR)
	$(NVCC) $(NVCCFLAGS) $(CUDA_ARCH) $(IT4_FLAGS) -o $@ $^

# Run targets
run: it1
	cd $(BIN_DIR) && ./marching_squares_it1

run-it1: it1
	cd $(BIN_DIR) && ./marching_squares_it1

run-it2: it2
	cd $(BIN_DIR) && ./marching_squares_it2

run-it3: it3
	cd $(BIN_DIR) && ./marching_squares_it3

run-it4: it4
	cd $(BIN_DIR) && ./marching_squares_it4

# Benchmark targets
benchmark: it1 it2 it3 it4
	@for iteration in it1 it2 it3; do \
		echo "=== Benchmark $$iteration ==="; \
		rm -f benchmark_$$iteration.csv; \
		cd $(BIN_DIR); \
		for threads in 1 2 4 8 16 32 64; do \
			for exp in 9 10 11 12 13 14; do \
				size=$$((2**$$exp)); \
				echo "Testing $$iteration: $$threads threads, grid $$size (2^$$exp)"; \
				OMP_NUM_THREADS=$$threads ./marching_squares_$$iteration $$size 400 ../benchmark_$$iteration.csv || true; \
			done; \
		done; \
		cd ..; \
	done
	@echo "=== Benchmark it4 (CUDA) ==="
	@rm -f benchmark_it4.csv
	@cd $(BIN_DIR); \
	for exp in 9 10 11 12 13 13; do \
		size=$$((2**$$exp)); \
		echo "Testing it4 (CUDA): grid $$size (2^$$exp)"; \
		./marching_squares_it4 $$size 400 ../benchmark_it4.csv || true; \
	done
	@echo "CSV files: benchmark_it1.csv, benchmark_it2.csv, benchmark_it3.csv, benchmark_it4.csv"

benchmark-it1: it1
	@echo "=== Benchmark IT1 ==="
	@rm -f benchmark_it1.csv
	@cd $(BIN_DIR); \
	for threads in 1 2 4 8 16 32 64; do \
		for exp in 9 10 11 12 13 14; do \
			size=$$((2**$$exp)); \
			echo "Testing it1: $$threads threads, grid $$size (2^$$exp)"; \
			OMP_NUM_THREADS=$$threads ./marching_squares_it1 $$size 400 ../benchmark_it1.csv || true; \
		done; \
	done
	@echo "CSV file: benchmark_it1.csv"

benchmark-it2: it2
	@echo "=== Benchmark IT2 ==="
	@rm -f benchmark_it2.csv
	@cd $(BIN_DIR); \
	for threads in 1 2 4 8 16 32 64; do \
		for exp in 9 10 11 12 13 14; do \
			size=$$((2**$$exp)); \
			echo "Testing it2: $$threads threads, grid $$size (2^$$exp)"; \
			OMP_NUM_THREADS=$$threads ./marching_squares_it2 $$size 400 ../benchmark_it2.csv || true; \
		done; \
	done
	@echo "CSV file: benchmark_it2.csv"

benchmark-it3: it3
	@echo "=== Benchmark IT3 ==="
	@rm -f benchmark_it3.csv
	@cd $(BIN_DIR); \
	for threads in 1 2 4 8 16 32 48 64; do \
		for exp in 9 10 11 12 13 14; do \
			size=$$((2**$$exp)); \
			echo "Testing it3: $$threads threads, grid $$size (2^$$exp)"; \
			OMP_NUM_THREADS=$$threads ./marching_squares_it3 $$size 400 ../benchmark_it3.csv || true; \
		done; \
	done
	@echo "CSV file: benchmark_it3.csv"

benchmark-it4: it4
	@echo "=== Benchmark IT4 (CUDA) ==="
	@rm -f benchmark_it4.csv
	@cd $(BIN_DIR); \
	for exp in 9 10 11 12 13; do \
		size=$$((2**$$exp)); \
		echo "Testing it4 (CUDA): grid $$size (2^$$exp)"; \
		./marching_squares_it4 $$size 400 ../benchmark_it4.csv || true; \
	done
	@echo "CSV file: benchmark_it4.csv"

# Clean
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR) *.csv

help:
	@echo "Makefile commands:"
	@echo "  all          - Build all iterations (including CUDA)"
	@echo "  it1          - Build iteration 1 (OpenMP)"
	@echo "  it2          - Build iteration 2 (OpenMP)"
	@echo "  it3          - Build iteration 3 (OpenMP)"
	@echo "  it4          - Build iteration 4 (CUDA)"
	@echo "  run          - Run default iteration (it1)"
	@echo "  run-it1      - Run iteration 1"
	@echo "  run-it2      - Run iteration 2"
	@echo "  run-it3      - Run iteration 3"
	@echo "  run-it4      - Run iteration 4 (CUDA)"
	@echo "  benchmark    - Run benchmarks for all iterations"
	@echo "  benchmark-it1- Run benchmark for iteration 1"
	@echo "  benchmark-it2- Run benchmark for iteration 2"
	@echo "  benchmark-it3- Run benchmark for iteration 3"
	@echo "  benchmark-it4- Run benchmark for iteration 4 (CUDA)"
	@echo "  clean        - Clean build and output files"
	@echo ""
	@echo "Requirements:"
	@echo "  - For it1-it3: g++ with OpenMP support"
	@echo "  - For it4: NVIDIA CUDA Toolkit with nvcc"

.PHONY: all it1 it2 it3 it4 run run-it1 run-it2 run-it3 run-it4 benchmark benchmark-it1 benchmark-it2 benchmark-it3 benchmark-it4 clean help
