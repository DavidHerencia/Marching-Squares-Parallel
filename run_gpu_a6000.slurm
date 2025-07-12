#!/bin/bash

#SBATCH --job-name=marching_squares_test_gpu
#SBATCH --partition=gpu
#SBATCH --nodelist=g002
#SBATCH --account=investigacion1
#SBATCH --ntasks=1 --cpus-per-task=8
#SBATCH --mem=16G
#SBATCH --time=01:00:00
#SBATCH --output=mi_salida.out
#SBATCH --error=mi_error.err

make benchmark-it4
