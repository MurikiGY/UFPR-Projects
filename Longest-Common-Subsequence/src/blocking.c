
#include "blocking.h"

#include <omp.h>

#ifndef max
#define max( a, b ) ( ((a) > (b)) ? (a) : (b) )
#endif

typedef unsigned short mtype;

int blockingLCS(mtype **scoreMatrix, int sizeA, int sizeB, char * seqA, char *seqB){
  int nthds = 4;

  #pragma omp parallel num_threads(nthds)
  {
    //int block = sizeB / omp_get_num_threads();
    //int block = sizeB / nthds;
    int block = nthds * 1024;

    int i, j;
    // Matrix blocking
    for (j=1; j<(sizeB-(sizeB%block)) ;j+=block){

      // Increasing
      for (i=1; i<block ;i++)
        #pragma omp for
        for (int k=i; k>0 ;k--)
          if (seqA[k-1] == seqB[j+i-k-1]) { scoreMatrix[j+i-k][k] = scoreMatrix[j+i-k-1][k-1] + 1; 
          } else { scoreMatrix[j+i-k][k] = max(scoreMatrix[j+i-k-1][k], scoreMatrix[j+i-k][k-1]); }

      // Constant
      for (; i<=sizeA ;i++)
        #pragma omp for
        for (int k=j; k<j+block ;k++)
          if (seqA[j+i-k-1] == seqB[k-1]) { scoreMatrix[k][j+i-k] = scoreMatrix[k-1][j+i-k-1] + 1;
          } else { scoreMatrix[k][j+i-k] = max(scoreMatrix[k-1][j+i-k], scoreMatrix[k][j+i-k-1]); }

      // Decreasing
      for (int l=j+1; l<j+block ;l++, i++)
        #pragma omp for
        for (int k=l; k<j+block ;k++)
          if (seqA[j+i-k-1] == seqB[k-1]) { scoreMatrix[k][j+i-k] = scoreMatrix[k-1][j+i-k-1] + 1;
          } else { scoreMatrix[k][j+i-k] = max(scoreMatrix[k-1][j+i-k], scoreMatrix[k][j+i-k-1]); }
    }

    if (j<=sizeB){
      block = sizeB % block;
      // Rest of matrix
      for (i=1; i<block ;i++)
        #pragma omp for
        for (int k=i; k>0 ;k--)
          if (seqA[k-1] == seqB[j+i-k-1]) { scoreMatrix[j+i-k][k] = scoreMatrix[j+i-k-1][k-1] + 1;
          } else { scoreMatrix[j+i-k][k] = max(scoreMatrix[j+i-k-1][k], scoreMatrix[j+i-k][k-1]); }

      // Constant
      for (; i<=sizeA ;i++)
        #pragma omp for
        for (int k=j; k<j+block ;k++)
          if (seqA[j+i-k-1] == seqB[k-1]) { scoreMatrix[k][j+i-k] = scoreMatrix[k-1][j+i-k-1] + 1;
          } else { scoreMatrix[k][j+i-k] = max(scoreMatrix[k-1][j+i-k], scoreMatrix[k][j+i-k-1]); }

      // Decreasing
      for (int l=j+1; l<j+block ;l++, i++)
        #pragma omp for
        for (int k=l; k<j+block ;k++)
          if (seqA[j+i-k-1] == seqB[k-1]) { scoreMatrix[k][j+i-k] = scoreMatrix[k-1][j+i-k-1] + 1;
          } else { scoreMatrix[k][j+i-k] = max(scoreMatrix[k-1][j+i-k], scoreMatrix[k][j+i-k-1]); }
    }
  }

  return scoreMatrix[sizeB][sizeA];
}
