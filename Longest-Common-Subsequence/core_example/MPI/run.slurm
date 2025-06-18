#!/bin/bash
#SBATCH --job-name=core_example
#SBATCH --partition cpu			# Partition name (-p)
#SBATCH --nodes=2			# Number of nodes (-n)
#SBATCH --ntasks=12			# Number of tasks
#SBATCH --cpus-per-task=1		# Number of CPUs each task uses (-c)
#SBATCH --output=output_%j.log		# Standard output and error log (-o)
#SBATCH --mem=15g 			# Memory per node (in MB)
#SBATCH --time=01:00:00			# Time limit (hh:mm:ss) (-t)

# -----
# In the ppr:x:node parameter, x must be less or equal to the number of cpus in each node
#
# For multithread programs it is better to allocate one task per node and
# use the number of cores from each cpu
#
# For embarrassingly parallel programs were each task is independent of the others
# then it is better to run nodes*cpupernode tasks
# -----

# Run the MPI program
mpirun -np 12 --map-by ppr:6:node ./main_mpi
