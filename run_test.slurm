#!/bin/bash

#SBATCH --job-name=marching_squares_test
#SBATCH --partition=big-mem
#SBATCH --account=investigacion1
#SBATCH --ntasks=1 --cpus-per-task=48
#SBATCH --mem=16G
#SBATCH --time=01:00:00
#SBATCH --output=mi_salida.out
#SBATCH --error=mi_error.err

make benchmark-it3
