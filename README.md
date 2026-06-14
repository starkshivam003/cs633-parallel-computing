# CS633 Project Workspace

This repository contains two separate MPI-based projects for CS633: one from the EndSem submission and one from the MidSem submission. Each subproject has its own `README.md` with detailed build and run instructions.

## Workspace Overview

- `EndSem-Project/`
  - End-semester MPI stencil benchmarking project.
  - Includes SLURM scripts, output logs, timing extraction, and plots.
  - See `EndSem-Project/README.md` for full details.

- `MidSem-Project/`
  - Mid-semester MPI communication experiment project.
  - Includes a SLURM run script, timing collection, and plotting script.
  - See `MidSem-Project/README.md` for full details.

- `.git/`
  - Git repository metadata.

## How the Two Projects Differ

### EndSem-Project

- Implements a 3D stencil simulation with halo exchange in x/y/z directions.
- Uses multiple SLURM batch scripts for varying process counts and global grid sizes.
- Extracts runtimes from SLURM output logs and produces a boxplot comparison between grid sizes.

### MidSem-Project

- Implements a simpler MPI communication pattern with two neighbor offsets (`D1`, `D2`).
- Executes repeated forward/backward data exchange and local transformation operations.
- Runs a parameter sweep across process counts and array sizes, then plots runtime distributions.

## Top-Level Usage

### Inspect subproject documentation

```bash
cat EndSem-Project/README.md
cat MidSem-Project/README.md
```

### Run the EndSem project pipeline

1. Change into the EndSem directory:

```bash
cd EndSem-Project
```

2. Compile the MPI program:

```bash
mpicc -O3 src.c -o src -lm
```

3. Submit one or more SLURM scripts, for example:

```bash
sbatch run_script_n32_n120_d7.sh
sbatch run_script_n64_n240_d7.sh
```

4. Extract timings after the jobs complete:

```bash
bash extract_times.sh
```

5. Generate the plot:

```bash
python3 plot.py
```

### Run the MidSem project pipeline

1. Change into the MidSem directory:

```bash
cd MidSem-Project
```

2. Compile the MPI program:

```bash
mpicc src.c -o src -lm
```

3. Submit the experiment job:

```bash
sbatch run_experiments.sh
```

4. Generate the plot after the job finishes:

```bash
python3 plot_script.py
```

## Notes

- Both subprojects assume a Linux environment with MPI available.
- `python3`, `pandas`, `matplotlib`, and `seaborn` are required for plotting in the MidSem project.
- The EndSem project also depends on `python3` for plotting and uses `bash` for extraction.

## Recommended Structure

Keep each subproject isolated in its own directory. Use the subproject-specific README for detailed configuration, and use this top-level README as the entry point to the workspace.
