
#include "original.h"

#include <stdio.h>
#include <stdlib.h>

#ifndef max
#define max( a, b ) ( ((a) > (b)) ? (a) : (b) )
#endif

typedef unsigned short mtype;

mtype ** allocateScoreMatrix(int sizeA, int sizeB) {
	int i;
	//Allocate memory for LCS score matrix
	mtype ** scoreMatrix = (mtype **) malloc((sizeB + 1) * sizeof(mtype *));
	for (i = 0; i < (sizeB + 1); i++)
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

int LCS(mtype ** scoreMatrix, int sizeA, int sizeB, char * seqA, char *seqB) {
	int i, j;
	for (i = 1; i < sizeB + 1; i++) {
		for (j = 1; j < sizeA + 1; j++) {
			if (seqA[j - 1] == seqB[i - 1]) {
				/* if elements in both sequences match,
				 the corresponding score will be the score from
				 previous elements + 1*/
				scoreMatrix[i][j] = scoreMatrix[i - 1][j - 1] + 1;
			} else {
				/* else, pick the maximum value (score) from left and upper elements*/
				scoreMatrix[i][j] =
						max(scoreMatrix[i-1][j], scoreMatrix[i][j-1]);
			}
		}
	}
	return scoreMatrix[sizeB][sizeA];
}

void printMatrix(char * seqA, char * seqB, mtype ** scoreMatrix, int sizeA, int sizeB) {
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

