#!/bin/bash
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=16
#SBATCH --time=00:10:00
#SBATCH --output=experiment_output.txt

scontrol show hostnames $SLURM_JOB_NODELIST > hostfile

mpicc src.c -o src -lm

echo "P,M,Time" > timing_data.txt

D1=2
D2=4
T=10
SEED=1000

for P in 8 16 32; do
    for M in 262144 1048576; do
        echo "Running P=$P M=$M..."
        for run in {1..5}; do
            OUTPUT=$(mpirun -np $P ./src $M $D1 $D2 $T $SEED)
            
            TIME=$(echo "$OUTPUT" | tail -n 1 | awk '{print $3}')
            
            echo "$P,$M,$TIME" >> timing_data.txt
        done
    done
done

echo "Experiments completed. Data saved to timing_data.txt"

