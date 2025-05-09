#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#include "src/chron.h"

#ifndef max
#define max( a, b ) ( ((a) > (b)) ? (a) : (b) )
#endif

#ifndef min
#define min( a, b ) ( ((a) > (b)) ? (b) : (a) )
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

mtype ** allocateScoreMatrix(int sizeA, int sizeB) {
  //int i;
  //Allocate memory for LCS score matrix
  mtype ** scoreMatrix = (mtype **) malloc((sizeB + 1) * sizeof(mtype *));

  for (int i = 0; i < (sizeB + 1); i++)
    scoreMatrix[i] = (mtype *) malloc((sizeA + 1) * sizeof(mtype));

  return scoreMatrix;
}

void initScoreMatrix(mtype ** scoreMatrix, int sizeA, int sizeB) {
  int i, j;
  //Fill first line of LCS score matrix with zeroes
  for (j = 0; j < (sizeA + 1); j++)
    scoreMatrix[0][j] = 0;

  //Do the same for the first collumn
  for (i = 1; i < (sizeB + 1); i++)
    scoreMatrix[i][0] = 0;
}


int shortLCS(mtype ** scoreMatrix, int sizeA, int sizeB, char * seqA, char *seqB) {

  for (int i=0, j=0; j<=sizeB && i<=sizeA ;j++){
    int n = min(j, sizeA-i);    // Size of the anti-diagonal

    #pragma omp parallel for
    for (int k=0; k<=n ;k++){   // Parallel for loop for each anti-diagonal element
      int a=i+k, b=j-k;         // For index of the diagonal elements

      if (a == 0 || b == 0)
        scoreMatrix[a][b] = 0;
      else if (seqA[a-1] == seqB[b-1])
        scoreMatrix[a][b] = scoreMatrix[a-1][b-1] + 1;
      else 
        scoreMatrix[a][b] = max(scoreMatrix[a-1][b], scoreMatrix[a][b-1]);
    }

    if (j >= sizeB){
      j=sizeA-1;
      i++;
    }
  }

  return scoreMatrix[sizeB][sizeA];
}


int myLCS(mtype ** scoreMatrix, int sizeA, int sizeB, char * seqA, char *seqB) {

  //#pragma omp parallel num_threads(7)
  //{
  //int block = omp_get_num_threads();
  int nthds = 8;
  int block = sizeB / nthds;

  int i, j;
  // Matrix blocking
  for (j=1; j<(sizeB-(sizeB%block)) ;j+=block){

    // Increasing
    for (i=1; i<block ;i++)
      #pragma omp parallel for num_threads(nthds)
      for (int k=i; k>0 ;k--)
        if (seqA[k-1] == seqB[j+i-k-1]) { scoreMatrix[j+i-k][k] = scoreMatrix[j+i-k-1][k-1] + 1; 
        } else { scoreMatrix[j+i-k][k] = max(scoreMatrix[j+i-k-1][k], scoreMatrix[j+i-k][k-1]); }

    // Constant
    for (; i<=sizeA ;i++)
      #pragma omp parallel for num_threads(nthds)
      for (int k=j; k<j+block ;k++)
        if (seqA[j+i-k-1] == seqB[k-1]) { scoreMatrix[k][j+i-k] = scoreMatrix[k-1][j+i-k-1] + 1;
        } else { scoreMatrix[k][j+i-k] = max(scoreMatrix[k-1][j+i-k], scoreMatrix[k][j+i-k-1]); }

    // Decreasing
    for (int l=j+1; l<j+block ;l++, i++)
      #pragma omp parallel for num_threads(nthds)
      for (int k=l; k<j+block ;k++)
        if (seqA[j+i-k-1] == seqB[k-1]) { scoreMatrix[k][j+i-k] = scoreMatrix[k-1][j+i-k-1] + 1;
        } else { scoreMatrix[k][j+i-k] = max(scoreMatrix[k-1][j+i-k], scoreMatrix[k][j+i-k-1]); }
  }

  if (j<=sizeB){
    block = sizeB % block;
    // Rest of matrix
    for (i=1; i<block ;i++)
      #pragma omp parallel for num_threads(block)
      for (int k=i; k>0 ;k--)
        if (seqA[k-1] == seqB[j+i-k-1]) { scoreMatrix[j+i-k][k] = scoreMatrix[j+i-k-1][k-1] + 1;
        } else { scoreMatrix[j+i-k][k] = max(scoreMatrix[j+i-k-1][k], scoreMatrix[j+i-k][k-1]); }

    // Constant
    for (; i<=sizeA ;i++)
      #pragma omp parallel for num_threads(block)
      for (int k=j; k<j+block ;k++)
        if (seqA[j+i-k-1] == seqB[k-1]) { scoreMatrix[k][j+i-k] = scoreMatrix[k-1][j+i-k-1] + 1;
        } else { scoreMatrix[k][j+i-k] = max(scoreMatrix[k-1][j+i-k], scoreMatrix[k][j+i-k-1]); }

    // Decreasing
    for (int l=j+1; l<j+block ;l++, i++)
      #pragma omp parallel for num_threads(block)
      for (int k=l; k<j+block ;k++)
        if (seqA[j+i-k-1] == seqB[k-1]) { scoreMatrix[k][j+i-k] = scoreMatrix[k-1][j+i-k-1] + 1;
        } else { scoreMatrix[k][j+i-k] = max(scoreMatrix[k-1][j+i-k], scoreMatrix[k][j+i-k-1]); }
  }
  //}

  return scoreMatrix[sizeB][sizeA];
}



int LCS(mtype ** scoreMatrix, int sizeA, int sizeB, char * seqA, char *seqB) {
  //int i, j;

  for (int i = 1; i < sizeB + 1; i++) {
    for (int j = 1; j < sizeA + 1; j++) {
      if (seqA[j - 1] == seqB[i - 1]) {
        /* if elements in both sequences match,
         the corresponding score will be the score from
         previous elements + 1*/
        scoreMatrix[i][j] = scoreMatrix[i - 1][j - 1] + 1;
      } else {
        /* else, pick the maximum value (score) from left and upper elements*/
        scoreMatrix[i][j] = max(scoreMatrix[i-1][j], scoreMatrix[i][j-1]);
      }
    }
  }

  //// Increasing diagonal
  //for (int i=1; i<=sizeA ;i++){
  //  for (int j=1; j<=i && j<=sizeB ;j++){
  //    if (seqA[i-j] == seqB[j-1])
  //      scoreMatrix[j][i-j+1] = scoreMatrix[j-1][i-j] + 1;
  //    else
  //      scoreMatrix[j][i-j+1] = max(scoreMatrix[j-1][i-j+1], scoreMatrix[j][i-j]);
  //  }
  //}

  //// Decreasing diagonal
  //int i = 2;
  //for (; i<=sizeB ;i++){
  //  for (int j=sizeA, k=i; k<=sizeB && j>0 ;j--, k++){
  //    if (seqA[j-1] == seqB[k-1])
  //      scoreMatrix[k][j] = scoreMatrix[k-1][j-1] + 1;
  //    else
  //      scoreMatrix[k][j] = max(scoreMatrix[k-1][j], scoreMatrix[k][j-1]);
  //  }
  //}

  return scoreMatrix[sizeB][sizeA];
}

void printMatrix(char * seqA, char * seqB, mtype ** scoreMatrix, int sizeA,
                 int sizeB) {
  int i, j;

  //print header
  printf("Score Matrix:\n");
  printf("========================================\n");

  //print LCS score matrix allong with sequences

  printf("    ");
  printf("%5c   ", ' ');

  for (j = 0; j < sizeA; j++)
    printf("%5c   ", seqA[j]);
  printf("\n");
  for (i = 0; i < sizeB + 1; i++) {
    if (i == 0)
      printf("    ");
    else
      printf("%c   ", seqB[i - 1]);
    for (j = 0; j < sizeA + 1; j++) {
      printf("%5d   ", scoreMatrix[i][j]);
    }
    printf("\n");
  }
  printf("========================================\n");
}

void freeScoreMatrix(mtype **scoreMatrix, int sizeB) {
  int i;
  for (i = 0; i < (sizeB + 1); i++)
    free(scoreMatrix[i]);
  free(scoreMatrix);
}



int main(int argc, char ** argv) {
  // Exec time
  struct timeval global_time = start_timer();

  // sequence pointers for both sequences
  char *seqA, *seqB;

  // sizes of both sequences
  int sizeA, sizeB;

  //read both sequences
  struct timeval read_time = start_timer();
  //seqA = read_seq("./inputs/fileA.in");
  //seqB = read_seq("./inputs/fileB.in");
  seqA = read_seq("fileA.in");
  seqB = read_seq("fileB.in");
  show_time("Strings read time", read_time);

  //find out sizes
  sizeA = strlen(seqA);
  sizeB = strlen(seqB);
  printf("SizeA: %d\n", sizeA);
  printf("SizeB: %d\n", sizeB);

  // allocate LCS score matrix
  struct timeval matrix_alloc_time = start_timer();
  mtype ** scoreMatrix = allocateScoreMatrix(sizeA, sizeB);
  show_time("matrix allocate time", matrix_alloc_time);

  //initialize LCS score matrix
  struct timeval matrix_init_time = start_timer();
  initScoreMatrix(scoreMatrix, sizeA, sizeB);
  show_time("matrix init time", matrix_init_time);

  //fill up the rest of the matrix and return final score (element locate at the last line and collumn)
  struct timeval lcs_time = start_timer();
  mtype score = myLCS(scoreMatrix, sizeA, sizeB, seqA, seqB);
  show_time("LCS time", lcs_time);


  /* if you wish to see the entire score matrix,
   for debug purposes, define DEBUGMATRIX. */
#ifdef DEBUGMATRIX
  printMatrix(seqA, seqB, scoreMatrix, sizeA, sizeB);
#endif

  //print score
  printf("\nScore: %d\n", score);

  // ---

  // Show exec time
  show_time("Gloal time", global_time);

  //free score matrix
  freeScoreMatrix(scoreMatrix, sizeB);
  free(seqA);
  free(seqB);

  return EXIT_SUCCESS;
}
