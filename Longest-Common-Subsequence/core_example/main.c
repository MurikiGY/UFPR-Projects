#include <stdio.h>
#include <omp.h>

int main(){
  int sizeA = 8;
  int sizeB = 6;

  int block = 3;
 
  int i, j;
  // Matrix blocking
  for (j=1; j<(sizeB-(sizeB%block)) ;j+=block){

    // Increasing
    for (i=1; i<block ;i++){
      //#pragma omp parallel for
      for (int k=i; k>0 ;k--){
        printf("M[%d][%d], ", j+i-k, k);
      }
      printf("\n");
    }

    // Constant
    for (; i<=sizeA ;i++){
      //#pragma omp parallel for
      for (int k=j; k<j+block ;k++){
        printf("M[%d][%d], ", k, j+i-k);
      }
      printf("\n");
    }

    // Decreasing
    for (int l=j+1; l<j+block ;l++, i++){
      //#pragma omp parallel for
      for (int k=l; k<j+block ;k++){
        printf("M[%d][%d], ", k, j+i-k);
      }
      printf("\n");
    }
  }

  //printf("AHHHHH\n");
  if (j<sizeB){
    block = sizeB % block;
    // Rest of matrix
    for (i=1; i<block ;i++){
      //#pragma omp parallel for
      for (int k=i; k>0 ;k--){
        printf("M[%d][%d], ", j+i-k, k);
      }
      printf("\n");
    }

    // Constant
    for (; i<=sizeA ;i++){
      //#pragma omp parallel for
      for (int k=j; k<j+block ;k++){
        printf("M[%d][%d], ", k, j+i-k);
      }
      printf("\n");
    }

    // Decreasing
    for (int l=j+1; l<j+block ;l++, i++){
      //#pragma omp parallel for
      for (int k=l; k<j+block ;k++){
        printf("M[%d][%d], ", k, j+i-k);
      }
      printf("\n");
    }
  }


  return 0;
}

