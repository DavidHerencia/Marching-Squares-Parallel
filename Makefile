# Makefile for Marching Squares Parallel Project
CXX = g++
CXXFLAGS = -std=c++17 -O3 -fopenmp

# Implementation flags
IT1_FLAGS = -DIT_NAME="it1"
IT2_FLAGS = -DIT_NAME="it2" 
IT3_FLAGS = -DIT_NAME="it3"

# Directories
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin
ITERATIONS_DIR = $(SRC_DIR)/iterations

# Executables
TARGET_IT1 = $(BIN_DIR)/marching_squares_it1
TARGET_IT2 = $(BIN_DIR)/marching_squares_it2
TARGET_IT3 = $(BIN_DIR)/marching_squares_it3

# Default target
all: it1 it2 it3

# Build targets
it1: $(TARGET_IT1)
it2: $(TARGET_IT2)
it3: $(TARGET_IT3)

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

# Run targets
run: it1
	cd $(BIN_DIR) && ./marching_squares_it1

run-it1: it1
	cd $(BIN_DIR) && ./marching_squares_it1

run-it2: it2
	cd $(BIN_DIR) && ./marching_squares_it2

run-it3: it3
	cd $(BIN_DIR) && ./marching_squares_it3

# Benchmark targets
benchmark: it1 it2 it3
	@for iteration in it1 it2 it3; do \
		echo "=== Benchmark $$iteration ==="; \
		rm -f benchmark_$$iteration.csv; \
		cd $(BIN_DIR); \
		for threads in 1 2 4 8 16 32 64 128; do \
			for exp in 9 10 11 12 13 14 15 16; do \
				size=$$((2**$$exp)); \
				echo "Testing $$iteration: $$threads threads, grid $$size (2^$$exp)"; \
				OMP_NUM_THREADS=$$threads ./marching_squares_$$iteration $$size 400 ../benchmark_$$iteration.csv || true; \
			done; \
		done; \
		cd ..; \
	done
	@echo "CSV files: benchmark_it1.csv, benchmark_it2.csv, benchmark_it3.csv"

benchmark-it1: it1
	@echo "=== Benchmark IT1 ==="
	@rm -f benchmark_it1.csv
	@cd $(BIN_DIR); \
	for threads in 1 2 4 8 16 32 64 128; do \
		for exp in 9 10 11 12 13; do \
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
	for threads in 1 2 4 8 16 32 64 128; do \
		for exp in 9 10 11 12 13; do \
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
	for threads in 1 2 4 8 16 32 64 128; do \
		for exp in 9 10 11 12 13; do \
			size=$$((2**$$exp)); \
			echo "Testing it3: $$threads threads, grid $$size (2^$$exp)"; \
			OMP_NUM_THREADS=$$threads ./marching_squares_it3 $$size 400 ../benchmark_it3.csv || true; \
		done; \
	done
	@echo "CSV file: benchmark_it3.csv"

# Clean
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR) *.csv

help:
	@echo "Makefile commands:"
	@echo "  all          - Build all iterations"
	@echo "  it1          - Build iteration 1"
	@echo "  it2          - Build iteration 2"
	@echo "  it3          - Build iteration 3"
	@echo "  run          - Run default iteration (it1)"
	@echo "  run-it1      - Run iteration 1"
	@echo "  run-it2      - Run iteration 2"
	@echo "  run-it3      - Run iteration 3"
	@echo "  benchmark    - Run benchmarks for all iterations"
	@echo "  benchmark-it1- Run benchmark for iteration 1"
	@echo "  benchmark-it2- Run benchmark for iteration 2"
	@echo "  benchmark-it3- Run benchmark for iteration 3"
	@echo "  clean        - Clean build and output files"

.PHONY: all it1 it2 it3 run run-it1 run-it2 run-it3 benchmark benchmark-it1 benchmark-it2 benchmark-it3 clean help
