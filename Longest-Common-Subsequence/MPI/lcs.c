#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <mpi.h>

#include "./src/chron.h"
#include "./src/original.h"
#include "./src/diag_mem.h"
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


 void originalLCS(int sizeA, int sizeB, char *seqA, char *seqB){
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
	mtype score = LCS(scoreMatrix, sizeA, sizeB, seqA, seqB);
  //show_time("LCS time", lcs_time);

	/* if you wish to see the entire score matrix, for debug purposes, define DEBUGMATRIX. */
#ifdef DEBUGMATRIX
	printMatrix(seqA, seqB, scoreMatrix, sizeA, sizeB);
#endif

	//print score and free matrix
	printf("Score: %d\n", score);
	freeScoreMatrix(scoreMatrix, sizeB);
}



void diagLCS(int sizeA, int sizeB, char *seqA, char *seqB, int my_rank, int n_tasks){

  // Swap strings to matrix always have more columns than lines
  if (sizeB > sizeA){ swap(seqA, seqB); swap(sizeA, sizeB); }
  //printf("--> SizeA: %d\n", sizeA);
  //printf("--> SizeB: %d\n", sizeB);

	// Allocate LCS score matrix
  //struct timeval matrix_alloc_time = start_timer();
	mtype **scoreMatrix = diagAllocateScoreMatrix(sizeA, sizeB);
  //show_time("Matrix allocate time", matrix_alloc_time);

	// Initialize LCS score matrix
  //struct timeval matrix_init_time = start_timer();
	diagInitScoreMatrix(scoreMatrix, sizeA, sizeB);
  //show_time("Matrix init time", matrix_init_time);

	// Fill up the rest of the matrix and return final score
  //struct timeval lcs_time = start_timer();
	//diagMemLCS(scoreMatrix, sizeA, sizeB, seqA, seqB);
	MPIDiagMemLCS(scoreMatrix, sizeA, sizeB, seqA, seqB, my_rank, n_tasks);
  //show_time("LCS time", lcs_time);

	// Print score and free matrix
	//printf("Score: %d\n", score);
	diagFreeScoreMatrix(scoreMatrix, sizeA, sizeB);
}


void linearLCS(int sizeA, int sizeB, char *seqA, char *seqB, int my_rank, int n_tasks){
  mtype **scoreMatrix;

  // Swap strings to matrix always have more columns than lines
  if (sizeB > sizeA){ swap(seqA, seqB); swap(sizeA, sizeB); }
  //printf("--> SizeA: %d\n", sizeA);
  //printf("--> SizeB: %d\n", sizeB);

	// Allocate LCS score matrix
  scoreMatrix = LinearAllocateScoreMatrix(sizeA, sizeB, my_rank, n_tasks);

	// Initialize LCS score matrix
	linearInitScoreMatrix(scoreMatrix, sizeA, sizeB, my_rank, n_tasks);

  // Run LCS
	MPILinearMemLCS(scoreMatrix, sizeA, sizeB, seqA, seqB, my_rank, n_tasks);

  // Free matrix
	LinearFreeScoreMatrix(scoreMatrix, sizeA, sizeB, my_rank, n_tasks);
}



int main(int argc, char** argv) {
  // Global exec time
  struct timeval global_time = start_timer();

  // Sequence pointers and sizes for both sequences
  char *seqA, *seqB;
  int sizeA, sizeB;

  // Read both sequences
  seqA = read_seq("./inputs/fileA10.in");
  seqB = read_seq("./inputs/fileB10.in");

  //find out sizes
  sizeA = strlen(seqA);
  sizeB = strlen(seqB);

  // Initialize MPI environment
  int my_rank, n_tasks;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &n_tasks);

  // --- Start Tests
  
  //originalLCS(sizeA, sizeB, seqA, seqB);
  //diagLCS(sizeA, sizeB, seqA, seqB, my_rank, n_tasks);
  linearLCS(sizeA, sizeB, seqA, seqB, my_rank, n_tasks);

  // --- Comunication

  //  // Show exec time
  if (my_rank == 0)
    show_time("Gloal time", global_time);

  // --- Finished

  // Finalize MPI environment.
  MPI_Finalize();
}
