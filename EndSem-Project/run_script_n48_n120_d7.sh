#!/bin/bash
#SBATCH --job-name=stencil_test
#SBATCH -N 1
#SBATCH --ntasks-per-node=48
#SBATCH --output=testwith_np48_n120_d7_%j.out
#SBATCH --error=testwith_np48_n120_d7_%j.err
#SBATCH --partition=cpu
#SBATCH --time=00:10:00

module load compiler/oneapi-2024/mpi

for i in {1..5}
do
     echo "Run $i"
     mpirun -np 48 ./src 7 48 6 4 2 240 240 240 5 1000 2 500
done
