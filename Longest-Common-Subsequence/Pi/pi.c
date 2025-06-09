#include <stdio.h>
#include <sys/time.h>
#include <mpi.h>

#include "./src/chron.h"

#define STD_TAG 0

static long num_steps = 1200000;

double step;

int main(int argc, char **argv) {
  // Global execution time
  struct timeval global_time = start_timer();
  // Pi vars
  double pi, sum = 0.0;
  // MPI vars
  int my_rank, n_tasks;
  MPI_Status status;

  step = 1.0/(double) num_steps;

  // --- Parallel context

  // Init and context
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &n_tasks);
  int block_size = num_steps / n_tasks;

  for (int i=my_rank*block_size; i<(my_rank+1)*block_size ;i++){
    double x = (i + 0.5) * step;
    sum += 4.0 / (1.0 + x*x);
  }

  // Sync data with rank 0
  if (my_rank != 0){
    MPI_Send(&sum, 1, MPI_DOUBLE, 0, STD_TAG, MPI_COMM_WORLD);
  } else { // rank 0
    pi = sum;
    for (int i=1; i<n_tasks ;i++){
      MPI_Recv(&sum, 1, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
      pi += sum;
    }
    pi *= step;

    printf("Pi value: %f\n", pi);

    // Show global time
    struct timeval stop;
    gettimeofday(&stop, NULL);
    long milliseconds = (stop.tv_sec - global_time.tv_sec) * 1000 + (stop.tv_usec - global_time.tv_usec) / 1000;
    fprintf(stderr, "Global time: %lu\n", milliseconds);
  }

  MPI_Finalize();

  return 0;
} 
