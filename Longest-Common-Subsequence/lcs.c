#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#include "src/chron.h"
#include "src/original.h"
#include "src/blocking.h"
#include "src/diag_mem.h"

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


void diagLCS(int sizeA, int sizeB, char *seqA, char *seqB){

  // Swap strings to matrix always have more columns than lines
  if (sizeB > sizeA){
    swap(seqA, seqB);
    swap(sizeA, sizeB);
  }
  //printf("--> SizeA: %d\n", sizeA);
  //printf("--> SizeB: %d\n", sizeB);

	// allocate LCS score matrix
  //struct timeval matrix_alloc_time = start_timer();
	mtype **scoreMatrix = diagAllocateScoreMatrix(sizeA, sizeB);
  //show_time("Matrix allocate time", matrix_alloc_time);

	//initialize LCS score matrix
  //struct timeval matrix_init_time = start_timer();
	diagInitScoreMatrix(scoreMatrix, sizeA, sizeB);
  //show_time("Matrix init time", matrix_init_time);

	//fill up the rest of the matrix and return final score
  struct timeval lcs_time = start_timer();
	mtype score = diagMemLCS(scoreMatrix, sizeA, sizeB, seqA, seqB);
  show_time("LCS time", lcs_time);

	//print score and free matrix
	printf("Score: %d\n", score);
	diagFreeScoreMatrix(scoreMatrix, sizeA, sizeB);
}


void blockLCS(int sizeA, int sizeB, char *seqA, char *seqB){

  // Swap strings to matrix always have more columns than lines
  if (sizeB > sizeA){
    swap(seqA, seqB);
    swap(sizeA, sizeB);
  }
  //printf("--> SizeA: %d\n", sizeA);
  //printf("--> SizeB: %d\n", sizeB);

	// allocate LCS score matrix
  //struct timeval matrix_alloc_time = start_timer();
	mtype **scoreMatrix = allocateScoreMatrix(sizeA, sizeB);
  //show_time("Matrix allocate time", matrix_alloc_time);

	//initialize LCS score matrix
  //struct timeval matrix_init_time = start_timer();
	initScoreMatrix(scoreMatrix, sizeA, sizeB);
  //show_time("Matrix init time", matrix_init_time);

	//fill up the rest of the matrix and return final score
  struct timeval lcs_time = start_timer();
	mtype score = blockingLCS(scoreMatrix, sizeA, sizeB, seqA, seqB);
  show_time("LCS time", lcs_time);

	/* if you wish to see the entire score matrix,
	 for debug purposes, define DEBUGMATRIX. */
#ifdef DEBUGMATRIX
	printMatrix(seqA, seqB, scoreMatrix, sizeA, sizeB);
#endif

	//print score and free matrix
	printf("Score: %d\n", score);
	freeScoreMatrix(scoreMatrix, sizeB);
}


void originalLCS(int sizeA, int sizeB, char *seqA, char *seqB){
  //printf("--> SizeA: %d\n", sizeA);
  //printf("--> SizeB: %d\n", sizeB);

	// allocate LCS score matrix
  //struct timeval matrix_alloc_time = start_timer();
	mtype **scoreMatrix = allocateScoreMatrix(sizeA, sizeB);
  //show_time("Matrix allocate time", matrix_alloc_time);

	//initialize LCS score matrix
  //struct timeval matrix_init_time = start_timer();
	initScoreMatrix(scoreMatrix, sizeA, sizeB);
  //show_time("Matrix init time", matrix_init_time);

	//fill up the rest of the matrix and return final score (element locate at the last line and collumn)
  struct timeval lcs_time = start_timer();
	mtype score = LCS(scoreMatrix, sizeA, sizeB, seqA, seqB);
  show_time("LCS time", lcs_time);

	/* if you wish to see the entire score matrix,
	 for debug purposes, define DEBUGMATRIX. */
#ifdef DEBUGMATRIX
	printMatrix(seqA, seqB, scoreMatrix, sizeA, sizeB);
#endif

	//print score and free matrix
	printf("Score: %d\n", score);
	freeScoreMatrix(scoreMatrix, sizeB);
}



int main(int argc, char ** argv) {
  // Global execution time
  //struct timeval global_time = start_timer();

  // Sequence pointers and sizes for both sequences
  char *seqA, *seqB;
  int sizeA, sizeB;

  //read both sequences
  //struct timeval read_time = start_timer();
  if (argc < 2){
    seqA = read_seq("./fileA.in");
    seqB = read_seq("./fileB.in");
    printf("--> A: %s\n", "./fileA.in");
    printf("--> B: %s\n", "./fileB.in");
  } else {
    seqA = read_seq(argv[1]);
    seqB = read_seq(argv[2]);
    printf("--> A: %s\n", argv[1]);
    printf("--> B: %s\n", argv[2]);
  }
  //show_time("Strings read time", read_time);

  //find out sizes
  sizeA = strlen(seqA);
  sizeB = strlen(seqB);

  // ----- Start tests
  
  int op = 1;
  switch (op) {
    case 1:
      //printf("--> Running original LCS\n");
      originalLCS(sizeA, sizeB, seqA, seqB);
    break;

    case 2:
      //printf("--> Running blocking LCS\n");
      blockLCS(sizeA, sizeB, seqA, seqB);
    break;

    case 3:
      //printf("--> Running diagonal memory LCS\n");
      diagLCS(sizeA, sizeB, seqA, seqB);
    break;
  }

  // ----- End tests

  // Show exec time
  //show_time("Gloal time", global_time);
  printf("\n");

  //free seq
  free(seqA);
  free(seqB);

  return EXIT_SUCCESS;
}
