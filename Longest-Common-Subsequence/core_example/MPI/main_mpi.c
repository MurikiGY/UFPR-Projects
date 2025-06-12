#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#ifndef swap
#define swap(x, y) do { typeof(x) SWAP = x; x = y; y = SWAP; } while (0)
#endif


int main(int argc, char** argv){
  int sizeA = 7;
  int sizeB = 5;

  if (sizeA < sizeB)
    swap(sizeA, sizeB);

  int num_diag = sizeA + sizeB + 1;
  int **M;

  // MPI
  int my_rank, n_tasks;
  MPI_Status status;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &n_tasks);

  // ---

  M = malloc(num_diag * sizeof(int *));
  int k, l, m;
  k = 0;
  for (l=1; l<=sizeB ;l++, k++){
    M[k] = calloc(l, sizeof(int));
    //printf("[%2d] -> ", k);
    //for (int a=0; a<l-1 ;a++) { printf("[%2d] -> ", a); }
    //printf("[%2d]\n", l-1);
  }

  for (m=1; m<=sizeA-sizeB+1 ;m++, k++){
    M[k] = calloc(l, sizeof(int));
    //printf("[%2d] -> ", k);
    //for (int a=0; a<l-1 ;a++) { printf("[%2d] -> ", a); }
    //printf("[%2d]\n", l-1);
  }

  l--;
  for (; l >=1 ;l--, k++){
    M[k] = calloc(l, sizeof(int));
    //printf("[%2d] -> ", k);
    //for (int a=0; a<l-1 ;a++) { printf("[%2d] -> ", a); }
    //printf("[%2d]\n", l-1);
  }

  // ---

  int i;
  // Increasing
  for (i=1; i<sizeB ;i++){
    // diag_len = i

    for (int it_rank=my_rank; it_rank<i ;it_rank+=n_tasks){
      int k = it_rank + 1;
      int lin = i+1; int col = k - (lin - sizeA)*(lin/sizeA);
      printf("Rank %d, M[%2d,%2d], M[%2d,%2d]\n", my_rank, k, i-k+1, lin, col);
    }

    //for (int k=1; k<=i ;k++){
    //  int lin = i+1; int col = k - (lin - sizeA)*(lin/sizeA);
    //  printf("M[%2d,%2d], M[%2d,%2d]\n", k, i-k+1, lin, col);
    //}

    //if (my_rank == 0)
    //  printf("\n");
  }

  // Constant
  for (; i<sizeA ;i++){
    // diag_len = sizeB

    for (int it_rank=my_rank; it_rank<sizeB ;it_rank+=n_tasks){
      int k = it_rank + 1;
      int lin = i+1; int col = k - (lin - sizeA)*(lin/sizeA);
      printf("Rank %d, M[%2d,%2d], M[%2d,%2d]\n", my_rank, k, i-k+1, lin, col);
    }

    //for (int k=1; k<=sizeB ;k++){
    //  int lin = i+1; int col = k - (lin - sizeA)*(lin/sizeA);
    //  printf("M[%2d,%2d], M[%2d,%2d]\n", k, i-k+1, lin, col);
    //}
    //printf("\n");
  }

  // Decreasing
  for (int l=1; l<=sizeB ;l++, i++){
    // diag_len = sizeB - l + 1

    for (int it_rank=my_rank; it_rank<sizeB ;it_rank+=n_tasks){
      int k = it_rank + 1;
      if (k >= l){
        int lin = i+1; int col = k - (lin - sizeA)*(lin/sizeA);
        printf("Rank %d, M[%2d,%2d], M[%2d,%2d]\n", my_rank, k, i-k+1, lin, col);
      }
    }

    //for (int k=l; k<=sizeB ;k++){
    //  int lin = i+1; int col = k - (lin - sizeA)*(lin/(sizeA+1));
    //  printf("M[%2d,%2d], M[%2d,%2d]\n", k, i-k+1, lin, col);
    //}
    //printf("\n");
  }


  // --- Finished

  MPI_Finalize();

  // Free alocated matrix
  for (int k=0; k<num_diag ;k++)
    free(M[k]);
  free(M);

  return 0;
}

