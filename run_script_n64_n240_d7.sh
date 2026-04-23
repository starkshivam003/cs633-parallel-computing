#!/bin/bash
#SBATCH --job-name=stencil_test
#SBATCH -N 2
#SBATCH --ntasks-per-node=32
#SBATCH --output=testwith_np64_n240_d7_%j.out
#SBATCH --error=testwith_np64_n240_d7_%j.err
#SBATCH --partition=cpu
#SBATCH --time=00:10:00

module load compiler/oneapi-2024/mpi

for i in {1..5}
do
     echo "Run $i"
     mpirun -np 64 ./src 7 32 4 4 4 240 240 240 5 1000 2 500
done
