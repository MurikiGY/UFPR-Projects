
#include "original.h"

#include <stdlib.h>

#ifndef max
#define max( a, b ) ( ((a) > (b)) ? (a) : (b) )
#endif

typedef unsigned short mtype;

mtype **diagAllocateScoreMatrix(int sizeA, int sizeB){
  int num_diag = sizeA + sizeB + 1;

	//Allocate memory for LCS score matrix
	mtype **scoreMatrix = (mtype **) malloc(num_diag * sizeof(mtype *));

  int k, l, m;
  for (k=0, l=1; l<=sizeB ;l++, k++)
    scoreMatrix[k] = malloc(l * sizeof(int));
    //scoreMatrix[k] = calloc(l, sizeof(int));

  for (m=1; m<=sizeA-sizeB+1 ;m++, k++)
    scoreMatrix[k] = malloc(l * sizeof(int));
    //scoreMatrix[k] = calloc(l, sizeof(int));

  l--;
  for (; l >=1 ;l--, k++)
    scoreMatrix[k] = malloc(l * sizeof(int));
    //scoreMatrix[k] = calloc(l, sizeof(int));

	return scoreMatrix;
}


void diagInitScoreMatrix(mtype ** scoreMatrix, int sizeA, int sizeB){
  int i, j;

  for (j = 0; j < (sizeB + 1); j++){
    //scoreMatrix[0][j] = 0;
    scoreMatrix[j][0] = 0;
  }

  for (i = 1; i < (sizeB + 1); i++){
    //scoreMatrix[i][0] = 0;
    scoreMatrix[i][i] = 0;
  }

  for (; i < (sizeA + 1); i++){
    //scoreMatrix[i][0] = 0;
    scoreMatrix[i][sizeB] = 0;
  }
}


int diagMemLCS(mtype **scoreMatrix, int sizeA, int sizeB, char * seqA, char *seqB){
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


void diagFreeScoreMatrix(mtype **scoreMatrix, int sizeA, int sizeB){
  int num_diag = sizeA + sizeB + 1;

  // Free alocated matrix
  for (int k=0; k<num_diag ;k++)
    free(scoreMatrix[k]);
  free(scoreMatrix);
}
