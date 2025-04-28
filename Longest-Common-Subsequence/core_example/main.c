#include <stdio.h>

int main(){
  int sizeA = 4;
  int sizeB = 4;

  // Increasing diagonal
  for (int i=1; i<=sizeA ;i++){
    for (int j=1; j<=i && j<=sizeB ;j++){
      printf("M[%d][%d], ", j, i-j+1);
    }
    printf("\n");
  }

  int i = 2;
  for (; i<=sizeB ;i++){
    for (int j=sizeA, k=i; k<=sizeB && j>0 ;j--, k++){
      printf("M[%d][%d], ", k, j);
    }
    printf("\n");
  }

  return 0;
}

