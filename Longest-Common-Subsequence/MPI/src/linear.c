#include "linear.h"

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#ifndef max
#define max( a, b ) ( ((a) > (b)) ? (a) : (b) )
#endif

#define STD_TAG 0

// Allocate LCS score matrix
mtype **LinearAllocateScoreMatrix(int sizeA, int sizeB, int my_rank, int n_tasks){
  int size = sizeB / n_tasks;
  if (my_rank < (sizeB % n_tasks)) size++;

  //printf("Rank %d Alocando %d linhas\n", my_rank, size);
	mtype **scoreMatrix = (mtype **) malloc(size * sizeof(mtype *));

	for (int i = 0; i < size; i++)
		scoreMatrix[i] = (mtype *) malloc((sizeA + 1) * sizeof(mtype));
	return scoreMatrix;
}


// Initialize LCS score matrix
void linearInitScoreMatrix(mtype **scoreMatrix, int sizeA, int sizeB, int my_rank, int n_tasks){
  int size = sizeB / n_tasks;
  if (my_rank < (sizeB % n_tasks)) size++;

	for (int i = 0; i < size; i++)
		scoreMatrix[i][0] = 0;
}


// Run LCS
void MPILinearMemLCS(mtype **scoreMatrix, int sizeA, int sizeB, char *seqA, char *seqB, int my_rank, int n_tasks){
  MPI_Status status;
  short recv_nums[2] = {0};
  short send_nums[2] = {0};
  int size = sizeB / n_tasks;
  if (my_rank < (sizeB % n_tasks)) size++;

  int i = 0;
  if (my_rank == 0){
    for (int j=1; j<=sizeA ;j++){
      if (seqA[j-1] == seqB[0]) { scoreMatrix[i][j] = 1;
      } else { scoreMatrix[i][j] = scoreMatrix[i][j-1]; }
      send_nums[0] = scoreMatrix[i][j-1]; send_nums[1] = scoreMatrix[i][j];
      MPI_Send(send_nums, 2, MPI_SHORT, (my_rank+1)%n_tasks, STD_TAG, MPI_COMM_WORLD);
    }
  } else {
    for (int j=1; j<=sizeA ;j++){
      MPI_Recv(recv_nums, 2, MPI_SHORT, (my_rank+n_tasks-1)%n_tasks, STD_TAG, MPI_COMM_WORLD, &status);
      if (seqA[j-1] == seqB[i*n_tasks+my_rank]) { scoreMatrix[i][j] = recv_nums[0] + 1;
      } else { scoreMatrix[i][j] = max(scoreMatrix[i][j-1], recv_nums[1]); }
      send_nums[0] = scoreMatrix[i][j-1]; send_nums[1] = scoreMatrix[i][j];
      MPI_Send(send_nums, 2, MPI_SHORT, (my_rank+1)%n_tasks, STD_TAG, MPI_COMM_WORLD);
    }
  }

  i++;
  // i [1, size-1]
  for (; i<(size-1) ;i++){
    for (int j=1; j<=sizeA ;j++){
      MPI_Recv(recv_nums, 2, MPI_SHORT, (my_rank+n_tasks-1)%n_tasks, STD_TAG, MPI_COMM_WORLD, &status);
      if (seqA[j-1] == seqB[i*n_tasks+my_rank]) { scoreMatrix[i][j] = recv_nums[0] + 1;
      } else { scoreMatrix[i][j] = max(scoreMatrix[i][j-1], recv_nums[1]); }
      send_nums[0] = scoreMatrix[i][j-1]; send_nums[1] = scoreMatrix[i][j];
      MPI_Send(send_nums, 2, MPI_SHORT, (my_rank+1)%n_tasks, STD_TAG, MPI_COMM_WORLD);
    }
  }

  if (my_rank == ((sizeB-1) % n_tasks)){
    for (int j=1; j<=sizeA ;j++){
      MPI_Recv(recv_nums, 2, MPI_SHORT, (my_rank+n_tasks-1)%n_tasks, STD_TAG, MPI_COMM_WORLD, &status);
      if (seqA[j-1] == seqB[i*n_tasks+my_rank]) { scoreMatrix[i][j] = recv_nums[0] + 1;
      } else { scoreMatrix[i][j] = max(scoreMatrix[i][j-1], recv_nums[1]); }
    }
  } else {
    for (int j=1; j<=sizeA ;j++){
      MPI_Recv(recv_nums, 2, MPI_SHORT, (my_rank+n_tasks-1)%n_tasks, STD_TAG, MPI_COMM_WORLD, &status);
      if (seqA[j-1] == seqB[i*n_tasks+my_rank]) { scoreMatrix[i][j] = recv_nums[0] + 1;
      } else { scoreMatrix[i][j] = max(scoreMatrix[i][j-1], recv_nums[1]); }
      send_nums[0] = scoreMatrix[i][j-1]; send_nums[1] = scoreMatrix[i][j];
      MPI_Send(send_nums, 2, MPI_SHORT, (my_rank+1)%n_tasks, STD_TAG, MPI_COMM_WORLD);
    }
  }

  // Show result
  if (my_rank == ((sizeB-1) % n_tasks))
    printf("Score: %d\n", scoreMatrix[size-1][sizeA]);
}


// Free matrix
void LinearFreeScoreMatrix(mtype **scoreMatrix, int sizeA, int sizeB, int my_rank, int n_tasks){
  int size = sizeB / n_tasks;
  if (my_rank < (sizeB % n_tasks)) size++;

	for (int i = 0; i < size; i++)
    free(scoreMatrix[i]);
  free(scoreMatrix);
}

