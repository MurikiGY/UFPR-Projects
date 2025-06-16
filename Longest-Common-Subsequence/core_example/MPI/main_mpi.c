#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#ifndef swap
#define swap(x, y) do { typeof(x) SWAP = x; x = y; y = SWAP; } while (0)
#endif

#define STD_TAG 0

void process(int my_rank, int it_rank, int sizeA, int i, int *nums){
  int k = it_rank + 1;
  int lin = i+1; int col = k - (lin - sizeA)*(lin/sizeA);
  printf("Rank %d, M[%2d,%2d], M[%2d,%2d], num[0] = %d, num[1] = %d\n", my_rank, k, i-k+1, lin, col, nums[0], nums[1]);
}


// Increasing
void increasing(int my_rank, int n_tasks, int sizeA, int sizeB, int *i){
  MPI_Status status;
  int nums[2] = {0};

  for (*i=1; *i<sizeB ;(*i)++){
    // diag_len = i

    // it_rank = i'th diagonal index
    for (int it_rank=my_rank; it_rank<*i ;it_rank+=n_tasks){
      if (it_rank == 0){                // Only send
        process(my_rank, it_rank, sizeA, *i, nums);
        nums[0] = it_rank+1; nums[1] = *i-it_rank;
        MPI_Send(nums, 2, MPI_INT, my_rank+1, STD_TAG, MPI_COMM_WORLD);
      } else if (it_rank == sizeB-1){   // Only receive
        MPI_Recv(nums, 2, MPI_INT, MPI_ANY_SOURCE, STD_TAG, MPI_COMM_WORLD, &status);
        process(my_rank, it_rank, sizeA, *i, nums);
      } else {                          // Receive and Send
        MPI_Recv(nums, 2, MPI_INT, MPI_ANY_SOURCE, STD_TAG, MPI_COMM_WORLD, &status);
        process(my_rank, it_rank, sizeA, *i, nums);
        nums[0] = it_rank+1; nums[1] = *i-it_rank;
        MPI_Send(nums, 2, MPI_INT, (my_rank + 1) % n_tasks, STD_TAG, MPI_COMM_WORLD);
      }
    }
  }
}


void constant(int my_rank, int n_tasks, int sizeA, int sizeB, int *i){
  MPI_Status status;
  int nums[2] = {0};

  // Constant
  for (; *i<sizeA ;(*i)++){
    // diag_len = sizeB

    for (int it_rank=my_rank; it_rank<sizeB ;it_rank+=n_tasks){
      if (it_rank == 0){
        // Only send
        process(my_rank, it_rank, sizeA, *i, nums);
        nums[0] = it_rank+1; nums[1] = *i-it_rank;
        MPI_Send(nums, 2, MPI_INT, my_rank+1, STD_TAG, MPI_COMM_WORLD);
      } else if (it_rank == sizeB-1){
        // Only receive
        MPI_Recv(nums, 2, MPI_INT, MPI_ANY_SOURCE, STD_TAG, MPI_COMM_WORLD, &status);
        process(my_rank, it_rank, sizeA, *i, nums);
      } else { // Receive and Send
        MPI_Recv(nums, 2, MPI_INT, MPI_ANY_SOURCE, STD_TAG, MPI_COMM_WORLD, &status);
        process(my_rank, it_rank, sizeA, *i, nums);
        nums[0] = it_rank+1; nums[1] = *i-it_rank;
        MPI_Send(nums, 2, MPI_INT, (my_rank + 1) % n_tasks, STD_TAG, MPI_COMM_WORLD);
      }
    }
  }
}


void dec_process(int my_rank, int it_rank, int sizeA, int i, int l, int *nums){
  int k = it_rank + 1;
  if (k >= l){
    int lin = i+1; int col = k - (lin - sizeA)*(lin/sizeA);
    printf("Rank %d, M[%2d,%2d], M[%2d,%2d], num[0] = %d, num[1] = %d\n", my_rank, k, i-k+1, lin, col, nums[0], nums[1]);
  }
}


void decreasing(int my_rank, int n_tasks, int sizeA, int sizeB, int *i){
  MPI_Status status;
  int nums[2] = {0};

  // Decreasing
  for (int l=1; l<=sizeB ;l++, (*i)++){
    // diag_len = sizeB - l + 1

    for (int it_rank=my_rank; it_rank<sizeB ;it_rank+=n_tasks){
      if (it_rank == 0){
        dec_process(my_rank, it_rank, sizeA, *i, l, nums);
        nums[0] = it_rank+1; nums[1] = *i-it_rank;
        MPI_Send(nums, 2, MPI_INT, my_rank+1, STD_TAG, MPI_COMM_WORLD);
      } else if (it_rank == sizeB-1){
        MPI_Recv(nums, 2, MPI_INT, MPI_ANY_SOURCE, STD_TAG, MPI_COMM_WORLD, &status);
        dec_process(my_rank, it_rank, sizeA, *i, l, nums);
      } else {
        MPI_Recv(nums, 2, MPI_INT, MPI_ANY_SOURCE, STD_TAG, MPI_COMM_WORLD, &status);
        dec_process(my_rank, it_rank, sizeA, *i, l, nums);
        nums[0] = it_rank+1; nums[1] = *i-it_rank;
        MPI_Send(nums, 2, MPI_INT, (my_rank + 1) % n_tasks, STD_TAG, MPI_COMM_WORLD);
      }
    }
  }
}



int main(int argc, char** argv){
  int sizeA = 9;
  int sizeB = 8;

  if (sizeA < sizeB)
    swap(sizeA, sizeB);

  int num_diag = sizeA + sizeB + 1;
  int **M;

  // MPI
  int my_rank, n_tasks;
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

  // i = Diagonal iterator
  int i;


  // 4 Horseman of comunication
  // 1 - At the start, only Send
  // 2 - At the end, only Receive
  // 3 - If rank == 0, receive from n_task-1 
  // 4 - If rank == n_task-1 send to 0
  increasing(my_rank, n_tasks, sizeA, sizeB, &i);
  constant(my_rank, n_tasks, sizeA, sizeB, &i);
  decreasing(my_rank, n_tasks, sizeA, sizeB, &i);


  // --- Finished

  MPI_Finalize();

  // Free alocated matrix
  for (int k=0; k<num_diag ;k++)
    free(M[k]);
  free(M);

  return 0;
}

