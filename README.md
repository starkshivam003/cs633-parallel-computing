# CS633 Endsem Project

MPI-based 3D stencil simulation with halo exchange, repeated benchmarking on SLURM, timing extraction, and boxplot visualization.

## What This Project Does

- Runs a distributed 3D stencil update on decomposed subdomains using MPI.
- Exchanges halo values in x/y/z directions each timestep.
- Counts isovalue-crossing edges (x, y, z neighbors) per field after each timestep.
- Prints total runtime for each run.
- Aggregates runtimes from SLURM output files into CSV.
- Generates comparison boxplots for two global grid sizes.

## Full File Inventory (Project Root)

### Source and executable

- `src.c`: Main MPI implementation.
- `src`: Compiled ELF executable built from `src.c`.

### Run scripts (SLURM)

- `run_script_n32_n120_d7.sh`
- `run_script_n32_n240_d7.sh`
- `run_script_n48_n120_d7.sh`
- `run_script_n48_n240_d7.sh`
- `run_script_n64_n120_d7.sh`
- `run_script_n64_n240_d7.sh`
- `run_script_n96_n120_d7.sh`
- `run_script_n96_n240_d7.sh`

Each script submits 5 runs for one `(P, size)` configuration and writes logs to matching `testwith_*.out`/`.err` files.

### Post-processing and plotting

- `extract_times.sh`: Parses all `*.out` files and writes `timings.csv`.
- `plot.py`: Reads `timings.csv` and creates `final_plot.png` with boxplots for size 120 and 240.

### Data and generated artifacts

- `timings.csv`: Extracted timing dataset used for plotting.
- `final_plot.png`: Final performance boxplot figure.
- `Group31.zip`: Archived submission package.

### Hidden/version-control metadata

- `.git/`: Standard Git metadata and object store files (auto-managed, not part of project logic).

### SLURM output logs

- `testwith_np32_n120_d7_18737.out`
- `testwith_np32_n240_d7_18752.out`
- `testwith_np48_n120_d7_18736.out`
- `testwith_np48_n240_d7_18751.out`
- `testwith_np64_n120_d7_18741.out`
- `testwith_np64_n240_d7_18747.out`
- `testwith_np96_n120_d7_18742.out`
- `testwith_np96_n240_d7_18745.out`

These files include SLURM metadata, per-timestep isovalue counts, and one runtime line per run.

## Build

Compile with MPI and math library:

```bash
mpicc -O3 src.c -o src -lm
```

## Program Interface

Run format:

```bash
mpirun -np <procs> ./src d ppn px py pz nx ny nz T seed F isovalue
```

Arguments:

- `d`: Stencil diameter, with radius `r = (d - 1) / 6`.
- `ppn`: Processes per node (accepted but not used in computation logic).
- `px py pz`: Process grid decomposition.
- `nx ny nz`: Local subdomain size per process.
- `T`: Number of timesteps.
- `seed`: RNG seed for initialization.
- `F`: Number of fields.
- `isovalue`: Threshold used for crossing counts.

Constraint:

- Must satisfy `px * py * pz == number_of_processes`.

Program output behavior:

- At each timestep, rank 0 prints `F` integer counts (isovalue crossings).
- After all timesteps, rank 0 prints total wall-clock time in seconds.

## How To Reproduce The Existing Pipeline

1. Compile:

```bash
mpicc -O3 src.c -o src -lm
```

2. Submit chosen SLURM scripts, for example:

```bash
sbatch run_script_n32_n120_d7.sh
sbatch run_script_n32_n240_d7.sh
sbatch run_script_n64_n120_d7.sh
sbatch run_script_n64_n240_d7.sh
```

3. Extract runtimes:

```bash
bash extract_times.sh
```

4. Plot:

```bash
python3 plot.py
```

## Timing Summary From `timings.csv`

Mean and standard deviation over 5 runs per configuration:

| P | Size | Mean Time (s) | Std Dev (s) |
|---:|---:|---:|---:|
| 32 | 120 | 1.542744 | 0.281860 |
| 48 | 120 | 1.701286 | 0.268766 |
| 64 | 120 | 1.520771 | 0.040687 |
| 96 | 120 | 4.353402 | 0.297608 |
| 32 | 240 | 9.521504 | 0.185603 |
| 48 | 240 | 9.680423 | 0.366016 |
| 64 | 240 | 9.902364 | 0.055895 |
| 96 | 240 | 12.926068 | 0.158346 |

## Notes And Observations

- `extract_times.sh` correctly extracts numeric-only lines as runtime entries.
- `plot.py` expects `timings.csv` columns: `P,size,run,time`.
- `run_script_n48_n120_d7.sh` currently launches with `240 240 240` (same as `run_script_n48_n240_d7.sh`) even though its filename says `n120`. If you want strict naming/parameter consistency, this script should be corrected.
- There is no existing `README.md` before this file; this README is newly created.

## Environment Assumptions

- Linux + SLURM cluster.
- MPI runtime available (scripts use `module load compiler/oneapi-2024/mpi`).
- Python 3 with `pandas` and `matplotlib` for plotting.
