#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <mpi.h>

#include "./src/original.h"
//#include "./src/diag_mem.h"
#include "./src/linear.h"

#ifndef swap
#define swap(x, y) do { typeof(x) SWAP = x; x = y; y = SWAP; } while (0)
#endif

typedef unsigned short mtype;

/* Read sequence from a file to a char vector.
 Filename is passed as parameter */

char* read_seq(char *fname) {
  //file pointer
  FILE *fseq = NULL;
  //sequence size
  long size = 0;
  //sequence pointer
  char *seq = NULL;
  //sequence index
  int i = 0;

  //open file
  fseq = fopen(fname, "rt");
  if (fseq == NULL ) {
    printf("Error reading file %s\n", fname);
    exit(1);
  }

  //find out sequence size to allocate memory afterwards
  fseek(fseq, 0L, SEEK_END);
  size = ftell(fseq);
  rewind(fseq);

  //allocate memory (sequence)
  seq = (char *) calloc(size + 1, sizeof(char));
  if (seq == NULL ) {
    printf("Erro allocating memory for sequence %s.\n", fname);
    exit(1);
  }

  //read sequence from file
  while (!feof(fseq)) {
    seq[i] = fgetc(fseq);
    if ((seq[i] != '\n') && (seq[i] != EOF))
      i++;
  }
  //insert string terminator
  seq[i] = '\0';

  //close file
  fclose(fseq);

  //return sequence pointer
  return seq;
}


 void originalLCS(int sizeA, int sizeB, char *seqA, char *seqB, int cat){
  double start, finish;
  //printf("--> SizeA: %d\n", sizeA);
  //printf("--> SizeB: %d\n", sizeB);

	// Allocate LCS score matrix
  //struct timeval matrix_alloc_time = start_timer();
	mtype **scoreMatrix = allocateScoreMatrix(sizeA, sizeB);
  //show_time("Matrix allocate time", matrix_alloc_time);

	// Initialize LCS score matrix
  //struct timeval matrix_init_time = start_timer();
	initScoreMatrix(scoreMatrix, sizeA, sizeB);
  //show_time("Matrix init time", matrix_init_time);

	// Fill up the rest of the matrix and return final score (element locate at the last line and collumn)
  //struct timeval lcs_time = start_timer();
  MPI_Barrier(MPI_COMM_WORLD);
  start = MPI_Wtime();
	LCS(scoreMatrix, sizeA, sizeB, seqA, seqB);
  MPI_Barrier(MPI_COMM_WORLD);
  finish = MPI_Wtime();
  printf("Cat %d: %.3f s \n", cat, finish - start);
	//printf("Score: %d\n", score);
  //show_time("LCS time", lcs_time);

	/* if you wish to see the entire score matrix, for debug purposes, define DEBUGMATRIX. */
#ifdef DEBUGMATRIX
	printMatrix(seqA, seqB, scoreMatrix, sizeA, sizeB);
#endif

	// free matrix
	freeScoreMatrix(scoreMatrix, sizeB);
}



//void diagLCS(int sizeA, int sizeB, char *seqA, char *seqB, int my_rank, int n_tasks){
//
//  // Swap strings to matrix always have more columns than lines
//  if (sizeB > sizeA){ swap(seqA, seqB); swap(sizeA, sizeB); }
//  //printf("--> SizeA: %d\n", sizeA);
//  //printf("--> SizeB: %d\n", sizeB);
//
//	// Allocate LCS score matrix
//  //struct timeval matrix_alloc_time = start_timer();
//	mtype **scoreMatrix = diagAllocateScoreMatrix(sizeA, sizeB);
//  //show_time("Matrix allocate time", matrix_alloc_time);
//
//	// Initialize LCS score matrix
//  //struct timeval matrix_init_time = start_timer();
//	diagInitScoreMatrix(scoreMatrix, sizeA, sizeB);
//  //show_time("Matrix init time", matrix_init_time);
//
//	// Fill up the rest of the matrix and return final score
//  //struct timeval lcs_time = start_timer();
//	//diagMemLCS(scoreMatrix, sizeA, sizeB, seqA, seqB);
//	MPIDiagMemLCS(scoreMatrix, sizeA, sizeB, seqA, seqB, my_rank, n_tasks);
//  //show_time("LCS time", lcs_time);
//
//	// Print score and free matrix
//	//printf("Score: %d\n", score);
//	diagFreeScoreMatrix(scoreMatrix, sizeA, sizeB);
//}


void linearLCS(int sizeA, int sizeB, char *seqA, char *seqB, int my_rank, int n_tasks, int cat){
  double start, finish;

  // Swap strings to matrix always have more columns than lines
  if (sizeB > sizeA){ swap(seqA, seqB); swap(sizeA, sizeB); }
  //printf("--> SizeA: %d\n", sizeA);
  //printf("--> SizeB: %d\n", sizeB);

	// Allocate LCS score matrix
  //MPI_Barrier(MPI_COMM_WORLD);
  //start = MPI_Wtime();
  mtype **scoreMatrix = LinearAllocateScoreMatrix(sizeA, sizeB, my_rank, n_tasks);
  //MPI_Barrier(MPI_COMM_WORLD);
  //finish = MPI_Wtime();
  //if (my_rank == 0) printf("Alocate: %f s \n", finish - start);

	// Initialize LCS score matrix
  //MPI_Barrier(MPI_COMM_WORLD);
  //start = MPI_Wtime();
	linearInitScoreMatrix(scoreMatrix, sizeA, sizeB, my_rank, n_tasks);
  //MPI_Barrier(MPI_COMM_WORLD);
  //finish = MPI_Wtime();
  //if (my_rank == 0) printf("Init: %f s \n", finish - start);

  // Run LCS
  MPI_Barrier(MPI_COMM_WORLD);
  start = MPI_Wtime();
	MPILinearMemLCS(scoreMatrix, sizeA, sizeB, seqA, seqB, my_rank, n_tasks);
  MPI_Barrier(MPI_COMM_WORLD);
  finish = MPI_Wtime();
  if (my_rank == 0) //printf("LCS %d time: %f s \n", sizeA, finish - start);
    printf("Cat %d: %.3f s \n", cat, finish - start);

  // Free matrix
	LinearFreeScoreMatrix(scoreMatrix, sizeA, sizeB, my_rank, n_tasks);
}



int main(int argc, char** argv) {
  // Sequence pointers and sizes for both sequences
  char *seqA, *seqB;
  int sizeA, sizeB;

  // Read both sequences
  seqA = read_seq("./inputs/fileA40.in");
  seqB = read_seq("./inputs/fileB40.in");

  //find out sizes
  //sizeA = strlen(seqA);
  //sizeB = strlen(seqB);

  // Initialize MPI environment
  int my_rank, n_tasks;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &n_tasks);

  // Timer
  //MPI_Barrier(MPI_COMM_WORLD);
  //double start = MPI_Wtime();

  // --- Start Tests
  int step = 10000, cat = 0;
  if (n_tasks == 1){
    for (sizeA=step; sizeA<=4*step ;sizeA+=step){
      for (sizeB=sizeA; sizeB<=4*step ;sizeB+=step){
        for (int i=0; i<20 ;i++)
          originalLCS(sizeA, sizeB, seqA, seqB, cat);
        cat++;
      }
    }
  } else {
    for (sizeA=step; sizeA<=4*step ;sizeA+=step){
      for (sizeB=sizeA; sizeB<=4*step ;sizeB+=step){
        for (int i=0; i<20 ;i++)
          linearLCS(sizeA, sizeB, seqA, seqB, my_rank, n_tasks, cat);
        cat++;
      }
    }
  }

  //MPI_Barrier(MPI_COMM_WORLD);
  //double finish = MPI_Wtime();
  //if (my_rank == 0)
  //  printf("Tempo: %f s \n", finish - start);

  // --- Finished

  // Finalize MPI environment.
  MPI_Finalize();
}
