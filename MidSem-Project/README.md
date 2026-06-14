# CS633 Midsem Project

MPI-based communication experiments with two neighbor distances, iterative message passing, performance logging, and boxplot visualization.

## What This Project Does

- Builds an MPI program that exchanges data between processes at two communication offsets (`D1` and `D2`).
- Performs repeated iterations of forward/backward sends and receives.
- Applies local transformations on received data before sending it back.
- Computes process-local maxima and reduces them to rank 0.
- Measures wall-clock runtime for a complete experiment.
- Aggregates timing results into `timing_data.txt`.
- Generates a boxplot comparing runtimes across process counts and data sizes.

## Full File Inventory (Project Root)

### Source and executable

- `src.c`: Main MPI implementation.
- `src`: Compiled MPI executable.

### Scripts and pipeline

- `run_experiments.sh`: SLURM batch script that compiles `src.c`, runs experiments for multiple process counts and data sizes, and writes timings to `timing_data.txt`.
- `plot_script.py`: Reads `timing_data.txt` and saves `time_vs_processes_boxplot.png`.

### Data and artifacts

- `timing_data.txt`: Collected runtime data in CSV format.
- `time_vs_processes_boxplot.png`: Generated runtime distribution plot.
- `experiment_output.txt`: SLURM output from `run_experiments.sh`.

## Build

Compile with MPI and the math library:

```bash
mpicc src.c -o src -lm
```

## Program Interface

Run format:

```bash
mpirun -np <P> ./src M D1 D2 T seed
```

Arguments:

- `M`: Size of the data array per process (default `1048576`).
- `D1`: First communication distance (default `2`).
- `D2`: Second communication distance (default `4`).
- `T`: Number of iterations (default `10`).
- `seed`: RNG seed for initialization (default `42`).

The program prints three values from rank 0:

1. global maximum after `D1` processing
2. global maximum after `D2` processing
3. total wall-clock time in seconds

## How To Reproduce The Existing Pipeline

1. Compile the program:

```bash
mpicc src.c -o src -lm
```

2. Submit the SLURM batch script:

```bash
sbatch run_experiments.sh
```

3. After the job completes, plot the results:

```bash
python3 plot_script.py
```

## Experiment Configuration

The current `run_experiments.sh` configuration uses:

- `P = 8, 16, 32`
- `M = 262144, 1048576`
- `T = 10`
- `D1 = 2`
- `D2 = 4`
- 5 repeated runs per `(P, M)` configuration

This produces a `timing_data.txt` file with headers:

```csv
P,M,Time
```

## Notes

- `run_experiments.sh` also creates `hostfile` from SLURM host nodes.
- `plot_script.py` uses `pandas`, `matplotlib`, and `seaborn`.
- If you want to change the experiment sizes or iteration count, update `run_experiments.sh` and rerun the script.

## Environment Assumptions

- Linux with SLURM and MPI runtime available.
- `python3` with `pandas`, `matplotlib`, and `seaborn` installed.
