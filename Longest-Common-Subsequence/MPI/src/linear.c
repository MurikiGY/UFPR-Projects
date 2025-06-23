#include "linear.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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


// Run LCS, requirement: n_tasks <= sizeB
void MPILinearMemLCS(mtype **scoreMatrix, int sizeA, int sizeB, char *seqA, char *seqB, int my_rank, int n_tasks){
  MPI_Status status;
  int size = sizeB / n_tasks; if (my_rank < (sizeB % n_tasks)) size++;
  int block = 1024;
  short recv_nums[block+1], send_nums[block+1]; //block+1 because of the element in j-1

  int i = 0, j = 1, k = 0, l = 0;
  int count = 0;  // Number of bytes received
  if (my_rank == 0){
    for (j=1; j+block<(sizeA+1) ;j+=block){
      for (k=0; k<block ;k++){
        l = j+k;
        if (seqA[l-1] == seqB[0]) { scoreMatrix[i][l] = 1;
        } else { scoreMatrix[i][l] = scoreMatrix[i][l-1]; }
      }
      memcpy(send_nums, &scoreMatrix[i][j-1], (block+1)*sizeof(short));
      MPI_Send(send_nums, block+1, MPI_SHORT, (my_rank+1)%n_tasks, STD_TAG, MPI_COMM_WORLD);
    }
    for (k=0; j+k<(sizeA+1); k++){
      l = j+k;
      if (seqA[l-1] == seqB[0]) { scoreMatrix[i][l] = 1;
      } else { scoreMatrix[i][l] = scoreMatrix[i][l-1]; }
    }
    memcpy(send_nums, &scoreMatrix[i][j-1], (k+1)*sizeof(short));
    MPI_Send(send_nums, k+1, MPI_SHORT, (my_rank+1)%n_tasks, STD_TAG, MPI_COMM_WORLD);
  } else {
    for (j=1; j<(sizeA+1) ;j+=block){
      MPI_Recv(recv_nums, block+1, MPI_SHORT, (my_rank+n_tasks-1)%n_tasks, STD_TAG, MPI_COMM_WORLD, &status);
      MPI_Get_count(&status, MPI_SHORT, &count);
      for (k=0; k<(count-1) ;k++){
        l = j+k;
        if (seqA[l-1] == seqB[i*n_tasks+my_rank]) { scoreMatrix[i][l] = recv_nums[k] + 1;
        } else { scoreMatrix[i][l] = max(scoreMatrix[i][l-1], recv_nums[k+1]); }
      }
      memcpy(send_nums, &scoreMatrix[i][j-1], count*sizeof(short));
      MPI_Send(send_nums, count, MPI_SHORT, (my_rank+1)%n_tasks, STD_TAG, MPI_COMM_WORLD);
    }
  }

  // ---
  
  i++;
  // i [1, size-1]
  for (; i<(size-1) ;i++){
    for (j=1; j<(sizeA+1) ;j+=block){
      MPI_Recv(recv_nums, block+1, MPI_SHORT, (my_rank+n_tasks-1)%n_tasks, STD_TAG, MPI_COMM_WORLD, &status);
      MPI_Get_count(&status, MPI_SHORT, &count);
      for (k=0; k<(count-1) ;k++){
        l = j+k;
        if (seqA[l-1] == seqB[i*n_tasks+my_rank]) { scoreMatrix[i][l] = recv_nums[k] + 1;
        } else { scoreMatrix[i][l] = max(scoreMatrix[i][l-1], recv_nums[k+1]); }
      }
      memcpy(send_nums, &scoreMatrix[i][j-1], count*sizeof(short));
      MPI_Send(send_nums, count, MPI_SHORT, (my_rank+1)%n_tasks, STD_TAG, MPI_COMM_WORLD);
    }
  }

  // ---

  if (my_rank == ((sizeB-1) % n_tasks)){
    for (j=1; j<(sizeA+1) ;j+=block){
      MPI_Recv(recv_nums, block+1, MPI_SHORT, (my_rank+n_tasks-1)%n_tasks, STD_TAG, MPI_COMM_WORLD, &status);
      MPI_Get_count(&status, MPI_SHORT, &count);
      for (k=0; k<(count-1) ;k++){
        l = j+k;
        if (seqA[l-1] == seqB[i*n_tasks+my_rank]) { scoreMatrix[i][l] = recv_nums[k] + 1;
        } else { scoreMatrix[i][l] = max(scoreMatrix[i][l-1], recv_nums[k+1]); }
      }
    }
  } else {
    for (j=1; j<(sizeA+1) ;j+=block){
      MPI_Recv(recv_nums, block+1, MPI_SHORT, (my_rank+n_tasks-1)%n_tasks, STD_TAG, MPI_COMM_WORLD, &status);
      MPI_Get_count(&status, MPI_SHORT, &count);
      for (k=0; k<(count-1) ;k++){
        l = j+k;
        if (seqA[l-1] == seqB[i*n_tasks+my_rank]) { scoreMatrix[i][l] = recv_nums[k] + 1;
        } else { scoreMatrix[i][l] = max(scoreMatrix[i][l-1], recv_nums[k+1]); }
      }
      memcpy(send_nums, &scoreMatrix[i][j-1], count*sizeof(short));
      MPI_Send(send_nums, count, MPI_SHORT, (my_rank+1)%n_tasks, STD_TAG, MPI_COMM_WORLD);
    }
  }

  // Show result
  //if (my_rank == ((sizeB-1) % n_tasks))
  //  printf("Score: %d\n", scoreMatrix[size-1][sizeA]);
}


// Free matrix
void LinearFreeScoreMatrix(mtype **scoreMatrix, int sizeA, int sizeB, int my_rank, int n_tasks){
  int size = sizeB / n_tasks;
  if (my_rank < (sizeB % n_tasks)) size++;

	for (int i = 0; i < size; i++)
    free(scoreMatrix[i]);
  free(scoreMatrix);
}

