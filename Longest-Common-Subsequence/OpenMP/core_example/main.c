#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#ifndef swap
#define swap(x, y) do { typeof(x) SWAP = x; x = y; y = SWAP; } while (0)
#endif


int main(){
  int sizeA = 10;
  int sizeB = 7;

  if (sizeA < sizeB)
    swap(sizeA, sizeB);

  int num_diag = sizeA + sizeB + 1;
  int **M;

  // ---

  M = malloc(num_diag * sizeof(int *));
  int k, l, m;
  k = 0;
  for (l=1; l<=sizeB ;l++, k++){
    M[k] = calloc(l, sizeof(int));
    printf("[%2d] -> ", k);
    for (int a=0; a<l-1 ;a++) { printf("[%2d] -> ", a); }
    printf("[%2d]\n", l-1);
  }

  for (m=1; m<=sizeA-sizeB+1 ;m++, k++){
    M[k] = calloc(l, sizeof(int));
    printf("[%2d] -> ", k);
    for (int a=0; a<l-1 ;a++) { printf("[%2d] -> ", a); }
    printf("[%2d]\n", l-1);
  }

  l--;
  for (; l >=1 ;l--, k++){
    M[k] = calloc(l, sizeof(int));
    printf("[%2d] -> ", k);
    for (int a=0; a<l-1 ;a++) { printf("[%2d] -> ", a); }
    printf("[%2d]\n", l-1);
  }

  // ---

  int i;
  // Increasing
  for (i=1; i<sizeB ;i++){
    for (int k=1; k<=i ;k++){
      int lin = i+1; int col = k - (lin - sizeA)*(lin/sizeA);
      printf("M[%2d,%2d], ", k, i-k+1);
      printf("M[%2d,%2d]\n", lin, col);
    }
    printf("\n");
  }

  // Constant
  for (; i<sizeA ;i++){
    for (int k=1; k<=sizeB ;k++){
      int lin = i+1; int col = k - (lin - sizeA)*(lin/sizeA);
      printf("M[%2d,%2d], ", k, i-k+1);
      printf("M[%2d,%2d]\n", lin, col);
    }
    printf("\n");
  }

  // Decreasing
  for (int l=1; l<=sizeB ;l++, i++){
    for (int k=l; k<=sizeB ;k++){
      int lin = i+1; int col = k - (lin - sizeA)*(lin/(sizeA+1));
      printf("M[%2d,%2d], ", k, i-k+1);
      printf("M[%2d,%2d]\n", lin, col);
    }
    printf("\n");
  }

  // Free alocated matrix
  for (int k=0; k<num_diag ;k++)
    free(M[k]);
  free(M);

  return 0;
}

