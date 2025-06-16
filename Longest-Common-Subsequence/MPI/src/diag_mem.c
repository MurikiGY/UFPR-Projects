
#include "original.h"

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#ifndef max
#define max( a, b ) ( ((a) > (b)) ? (a) : (b) )
#endif

#define STD_TAG 0

typedef unsigned short mtype;

mtype **diagAllocateScoreMatrix(int sizeA, int sizeB){
  int num_diag = sizeA + sizeB + 1;

	//Allocate memory for LCS score matrix
	mtype **scoreMatrix = (mtype **) malloc(num_diag * sizeof(mtype *));

  int k, l, m;
  for (k=0, l=1; l<=sizeB ;l++, k++)
    scoreMatrix[k] = malloc(l * sizeof(int));

  for (m=1; m<=sizeA-sizeB+1 ;m++, k++)
    scoreMatrix[k] = malloc(l * sizeof(int));

  l--;
  for (; l >=1 ;l--, k++)
    scoreMatrix[k] = malloc(l * sizeof(int));

	return scoreMatrix;
}


void diagInitScoreMatrix(mtype ** scoreMatrix, int sizeA, int sizeB){
  int i, j;

  for (j = 0; j < (sizeB + 1); j++){
    scoreMatrix[j][0] = 0;
  }

  for (i = 1; i < (sizeB + 1); i++){
    scoreMatrix[i][i] = 0;
  }

  for (; i < (sizeA + 1); i++){
    scoreMatrix[i][sizeB] = 0;
  }
}


int diagMemLCS(mtype **scoreMatrix, int sizeA, int sizeB, char *seqA, char *seqB){
  int i;
  int lin, lin2, lin3, lin4;
  int col, col2, col3, col4;

  // Increasing
  for (i=1; i<sizeB ;i++){
    for (int k=1; k<=i ;k++){
      lin = i+1;    col = k - (lin - sizeA)*(lin/sizeA);
      lin2 = i;     col2 = (k-1) - (lin2 - sizeA)*(lin2/sizeA);
      lin3 = i;     col3 = k - (lin3 - sizeA)*(lin3/sizeA);
      lin4 = i-1;   col4 = (k-1) - (lin4 - sizeA)*(lin4/sizeA);

      if (seqA[i-k] == seqB[k-1]) { scoreMatrix[lin][col] = scoreMatrix[lin4][col4] + 1;
      } else { scoreMatrix[lin][col] = max(scoreMatrix[lin2][col2], scoreMatrix[lin3][col3]); }
    }
  }

  // Constant
  for (; i<sizeA ;i++){
    for (int k=1; k<=sizeB ;k++){
      lin = i+1;    col = k - (lin - sizeA)*(lin/sizeA);
      lin2 = i;     col2 = (k-1) - (lin2 - sizeA)*(lin2/sizeA);
      lin3 = i;     col3 = k - (lin3 - sizeA)*(lin3/sizeA);
      lin4 = i-1;   col4 = (k-1) - (lin4 - sizeA)*(lin4/sizeA);

      if (seqA[i-k] == seqB[k-1]) { scoreMatrix[lin][col] = scoreMatrix[lin4][col4] + 1;
      } else { scoreMatrix[lin][col] = max(scoreMatrix[lin2][col2], scoreMatrix[lin3][col3]); }
    }
  }

  // Decreasing
  for (int l=1; l<=sizeB ;l++, i++){
    for (int k=l; k<=sizeB ;k++){
      lin = i+1;    col = k - (lin - sizeA)*(lin/(sizeA+1));
      lin2 = i;     col2 = (k-1) - (lin2 - sizeA)*(lin2/(sizeA+1));
      lin3 = i;     col3 = k - (lin3 - sizeA)*(lin3/(sizeA+1));
      lin4 = i-1;   col4 = (k-1) - (lin4 - sizeA)*(lin4/(sizeA+1));

      if (seqA[i-k] == seqB[k-1]) { scoreMatrix[lin][col] = scoreMatrix[lin4][col4] + 1;
      } else { scoreMatrix[lin][col] = max(scoreMatrix[lin2][col2], scoreMatrix[lin3][col3]); }
    }
  }

	return scoreMatrix[sizeA+sizeB][0];
}


// ----- MPI

void process(mtype **scoreMatrix, int sizeA, char *seqA, char *seqB, int my_rank, int it_rank, int i, short *nums){
  int k = it_rank + 1;
  //int lin = i+1; int col = k - (lin - sizeA)*(lin/sizeA);
  //printf("Rank %d, M[%2d,%2d], M[%2d,%2d], num[0] = %d, num[1] = %d\n", my_rank, k, i-k+1, lin, col, nums[0], nums[1]);
  
  int lin = i+1;    int col = k - (lin - sizeA)*(lin/sizeA);
  //int lin2 = i;     int col2 = (k-1) - (lin2 - sizeA)*(lin2/sizeA);   // Up position
  int lin3 = i;     int col3 = k - (lin3 - sizeA)*(lin3/sizeA);         // Left position
  //int lin4 = i-1;   int col4 = (k-1) - (lin4 - sizeA)*(lin4/sizeA);   // Diag position

  if (seqA[i-k] == seqB[k-1]) { scoreMatrix[lin][col] = nums[1] + 1;
  } else { scoreMatrix[lin][col] = max(nums[0], scoreMatrix[lin3][col3]); }

  nums[0] = scoreMatrix[lin][col]; nums[1] = scoreMatrix[lin3][col3];
  //nums[0] = k; nums[1] = i-k+1;
}


void dec_process(mtype **scoreMatrix, int sizeA, char *seqA, char *seqB, int my_rank, int it_rank, int i, int l, short *nums){
  int k = it_rank + 1;
  if (k >= l){
    //int lin = i+1; int col = k - (lin - sizeA)*(lin/sizeA);
    //printf("Rank %d, M[%2d,%2d], M[%2d,%2d], num[0] = %d, num[1] = %d\n", my_rank, k, i-k+1, lin, col, nums[0], nums[1]);
    
    int lin = i+1;    int col = k - (lin - sizeA)*(lin/sizeA);
    //int lin2 = i;     int col2 = (k-1) - (lin2 - sizeA)*(lin2/sizeA);   // Up position
    int lin3 = i;     int col3 = k - (lin3 - sizeA)*(lin3/sizeA);         // Left position
    //int lin4 = i-1;   int col4 = (k-1) - (lin4 - sizeA)*(lin4/sizeA);   // Diag position

    if (seqA[i-k] == seqB[k-1]) { scoreMatrix[lin][col] = nums[1] + 1;
    } else { scoreMatrix[lin][col] = max(nums[0], scoreMatrix[lin3][col3]); }

    nums[0] = scoreMatrix[lin][col]; nums[1] = scoreMatrix[lin3][col3];
    //nums[0] = k; nums[1] = i-k+1;
  }
}


// Increasing
void increasing(mtype **scoreMatrix, int sizeA, int sizeB, char *seqA, char *seqB, int my_rank, int n_tasks, int *i){
  MPI_Status status;
  short nums[2] = {0};

  for (*i=1; *i<sizeB ;(*i)++){
    // diag_len = i

    // it_rank = i'th diagonal index
    for (int it_rank=my_rank; it_rank<*i ;it_rank+=n_tasks){
      if (it_rank == 0){
        // Only send
        process(scoreMatrix, sizeA, seqA, seqB, my_rank, it_rank, *i, nums);
        MPI_Send(nums, 2, MPI_SHORT, my_rank+1, STD_TAG, MPI_COMM_WORLD);
      } else if (it_rank == sizeB-1){
        // Only receive
        MPI_Recv(nums, 2, MPI_SHORT, MPI_ANY_SOURCE, STD_TAG, MPI_COMM_WORLD, &status);
        process(scoreMatrix, sizeA, seqA, seqB, my_rank, it_rank, *i, nums);
      } else {
        // Receive and Send
        MPI_Recv(nums, 2, MPI_SHORT, MPI_ANY_SOURCE, STD_TAG, MPI_COMM_WORLD, &status);
        process(scoreMatrix, sizeA, seqA, seqB, my_rank, it_rank, *i, nums);
        MPI_Send(nums, 2, MPI_SHORT, (my_rank + 1) % n_tasks, STD_TAG, MPI_COMM_WORLD);
      }
    }
  }
}


void constant(mtype **scoreMatrix, int sizeA, int sizeB, char *seqA, char *seqB, int my_rank, int n_tasks, int *i){
  MPI_Status status;
  short nums[2] = {0};

  // Constant
  for (; *i<sizeA ;(*i)++){
    // diag_len = sizeB

    for (int it_rank=my_rank; it_rank<sizeB ;it_rank+=n_tasks){
      if (it_rank == 0){
        // Only send
        process(scoreMatrix, sizeA, seqA, seqB, my_rank, it_rank, *i, nums);
        MPI_Send(nums, 2, MPI_SHORT, my_rank+1, STD_TAG, MPI_COMM_WORLD);
      } else if (it_rank == sizeB-1){
        // Only receive
        MPI_Recv(nums, 2, MPI_SHORT, MPI_ANY_SOURCE, STD_TAG, MPI_COMM_WORLD, &status);
        process(scoreMatrix, sizeA, seqA, seqB, my_rank, it_rank, *i, nums);
      } else {
        // Receive and Send
        MPI_Recv(nums, 2, MPI_SHORT, MPI_ANY_SOURCE, STD_TAG, MPI_COMM_WORLD, &status);
        process(scoreMatrix, sizeA, seqA, seqB, my_rank, it_rank, *i, nums);
        MPI_Send(nums, 2, MPI_SHORT, (my_rank + 1) % n_tasks, STD_TAG, MPI_COMM_WORLD);
      }
    }
  }
}


void decreasing(mtype **scoreMatrix, int sizeA, int sizeB, char *seqA, char *seqB, int my_rank, int n_tasks, int *i){
  MPI_Status status;
  short nums[2] = {0};

  // Decreasing
  for (int l=1; l<=sizeB ;l++, (*i)++){
    // diag_len = sizeB - l + 1

    for (int it_rank=my_rank; it_rank<sizeB ;it_rank+=n_tasks){
      if (it_rank == 0){
        // Only send
        dec_process(scoreMatrix, sizeA, seqA, seqB, my_rank,it_rank, *i, l, nums);
        MPI_Send(nums, 2, MPI_SHORT, my_rank+1, STD_TAG, MPI_COMM_WORLD);
      } else if (it_rank == sizeB-1){
        // Only receive
        MPI_Recv(nums, 2, MPI_SHORT, MPI_ANY_SOURCE, STD_TAG, MPI_COMM_WORLD, &status);
        dec_process(scoreMatrix, sizeA, seqA, seqB, my_rank,it_rank, *i, l, nums);
      } else {
        // Receive and Send
        MPI_Recv(nums, 2, MPI_SHORT, MPI_ANY_SOURCE, STD_TAG, MPI_COMM_WORLD, &status);
        dec_process(scoreMatrix, sizeA, seqA, seqB, my_rank,it_rank, *i, l, nums);
        MPI_Send(nums, 2, MPI_SHORT, (my_rank + 1) % n_tasks, STD_TAG, MPI_COMM_WORLD);
      }
    }
  }
}


void MPIDiagMemLCS(mtype **scoreMatrix, int sizeA, int sizeB, char *seqA, char *seqB, int my_rank, int n_tasks){
  int i;

  increasing(scoreMatrix, sizeA, sizeB, seqA, seqB, my_rank, n_tasks, &i);
  constant(scoreMatrix, sizeA, sizeB, seqA, seqB, my_rank, n_tasks, &i);
  decreasing(scoreMatrix, sizeA, sizeB, seqA, seqB, my_rank, n_tasks, &i);

  //if (my_rank == (n_tasks+1)%sizeB)
  printf("Rank: %d, Score: %d\n", my_rank, scoreMatrix[sizeA+sizeB][0]);
}


void diagFreeScoreMatrix(mtype **scoreMatrix, int sizeA, int sizeB){
  int num_diag = sizeA + sizeB + 1;

  // Free alocated matrix
  for (int k=0; k<num_diag ;k++)
    free(scoreMatrix[k]);
  free(scoreMatrix);
}
