#!/bin/bash
#SBATCH --job-name=mpi_job        # Job name
#SBATCH --nodes=1                  # Number of nodes
#SBATCH --ntasks=8                 # Total number of tasks (processes)
#SBATCH --cpus-per-task=6          # Number of CPUs per task
#SBATCH --mem=15360                 # Memory per node
#SBATCH --time=01:00:00            # Time limit hrs:min:sec
#SBATCH --output=mpi_output.txt    # Standard output and error log

# Load the MPI module
#module load mpi/openmpi

# Run the MPI program
srun  $SLURM_NTASKS ./main

