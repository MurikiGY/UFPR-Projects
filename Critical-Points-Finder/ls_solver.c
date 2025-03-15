// GRR20203933 - Muriki Gusmao Yamanaka
// GRR20205152 - Wellington de Almeida Silva

#include "ls_solver.h"
#include "matrix.h"
#include "mymatheval.h"

#include <stdio.h>
#include <stdlib.h>

typedef unsigned int uint;

//Retorna maior valor da coluna i  a partir da linha i
int encontra_Max(double **A, int n, int i){
  int max = i, col = i;

  for (; i<n ;i++)
    if ( A[i][col] > A[max][col] )
      max = i;

  return max;
}

//Troca linhas de uma matriz
void troca_linha(double **A, int i, int pivo){
  double *aux;
  aux = A[i];
  A[i] = A[pivo];
  A[pivo] = aux;
}

//Transforma matriz A em L e U juntas
//deixa no vetor 'trocas' o mapemento das linhas
//originais, apos as trocas do pivoteamento
void fatoracao_LU(double **A, int n){
  for (int i=0; i < n-1 ;i++)
    //Transformação em L e U
    for (int j=i+1; j<n ;j++){
      A[j][i] = A[j][i] / A[i][i];  //Multiplicador m
      for (int k=i+1; k<n ;k++)     //Aplica m para o restante da linha
        A[j][k] -= A[i][k] * A[j][i];
    }
}


//vec: vetor otimizado para guardar k-diagonais
//n: quantidade de linhas da matriz original
//k: k do k-diagonal
void fatoracao_LU_otimizado(double *vec, int n, int k){

  register uint i;

  //itera pela primeira metade da matriz
  //for (i=k/2; i<(n*k)-k ;i+=k){
  for (i=k/2; i<n*k-k*(k>>1) ;i+=k){
    //Transformação em L e U
    for (uint j=i+k-1; j<=i+(k/2*k) ;j+=(k-1)){
      vec[j] /= vec[i];  //Multiplicador m
      //Aplica m para o restante da linha
      uint x=j+1, y=1;
      for (x=j+1, y=1 ; x+1 <= j+(k/2)+1; x+=2, y+=2){
        vec[x] -= vec[i+y]*vec[j];
        vec[x+1] -= vec[i+y+1]*vec[j];
      }
      for ( ;x <= j+(k/2);x++, y++){
        vec[x] -= vec[i+y]*vec[j];
      }
    }
  }

  //itera pela primeira metade da matriz
  for (; i<(n*k)-k ;i+=k){
    //Transformação em L e U
    for (uint j=i+k-1, p=1; j<=i+((k/2)-p)*k ;j+=(k-1), p++){
      vec[j] /= vec[i];  //Multiplicador m
      //Aplica m para o restante da linha
      uint x=j+1, y=1;
      for (x=j+1, y=1 ; x+1 <= j+(k/2)+1; x+=2, y+=2){
        vec[x] -= vec[i+y]*vec[j];
        vec[x+1] -= vec[i+y+1]*vec[j];
      }
      for ( ;x <= j+(k/2);x++, y++){
        vec[x] -= vec[i+y]*vec[j];
      }
    }
  }

}

//Resolve sistema triangular superior
void retrossubs_up(double **A, double *b, double *x, int n){
  for (int i=n-1; i>=0 ;--i){
    x[i] = b[i];
    for (int j=i+1; j<n ;++j){
      x[i] -= A[i][j] * x[j];
    }
    x[i] /= A[i][i];
  }
}


//Resolve sistema triangular superior
void retrossubs_up_otimizado(double *vec, double *b, double *x, unsigned int n, unsigned int k){
  //m limita j
  //l indexa vetor de solucao
  int i, m, l;
//k*(k>>1)
  //for (i=n*k-k/2, m=0, l=n-1; i>n-k*k/4 ;i-=k, m++, l--){
  for (i=n*k-k/2-1, m=0, l=n-1; i>n*k-(k*k>>1) ;i-=k, m++, l--){
    x[l] = b[l];
    for (int j=1; j<=m ;j++){
      x[l] -= vec[i+j]*x[l+j];
    }
    x[l] /= vec[i];
  }
  for (; i>0 ;i-=k, l--){
    x[l] = b[l];
    for (int j=1; j<=k/2 ;j++){
      x[l] -= vec[i+j]*x[l+j];
    }
    x[l] /= vec[i];
  }
}

//-------- if (!k%2)k++;

//Resolve sistema triangular inferior
void retrossubs_down(double **A, double *b, double *x, int n){
  for (int i=0; i<n ;i++){
    x[i] = b[i];

    for (int j=0; j<i ;j++)
      x[i] -= A[i][j] * x[j];
  }
}


//Resolve sistema triangular inferior
void retrossubs_down_otimizado(double *vec, double *b, double *x, unsigned int n, unsigned int k){

  register uint l = 0;
  register uint i;
  //i itera pelas linhas
  //l indexa vetores b e x e seta iniciam j 
  //m itera pelas colunas
  for (i=k/2; i<k*(k>>1) ;i+=k){
    x[l] = b[l];
    for (uint j = i-l, m = 0; j<i ;j++, m++)
      x[l] -= vec[j] * x[m];
    l++;
  }
  for (uint m=0; i<=n*k ;i+=k, l++, m++){
    x[l] = b[l];
    for (uint j = i-(k>>1), n = 0; j<i ;j++, n++)
      x[l] -= vec[j] * x[m+n];
  }
}


//Ajusta vetor b com base no vetor de trocas
void vetor_troca_pivoteamento(int *troca, double **b, int n){
  double *aux;
  if ( !(aux = malloc( n*sizeof(double) ))){ fprintf(stderr, "Erro troca pivoteamento\n");
    exit(1); }

  for (int i=0; i<n ;i++)
    aux[i] = (*b)[troca[i]];

  free(*b);
  *b = aux;
}


//avalia hessiana com valores de xdata, coloca resultado em x_hess
void atualiza_hessiana(void ***f_hess, double **x_hess, double *x_data, char **variables, int n){
  for (int i=0; i<n ;i++)
    for (int j=0; j<n ;j++)
      x_hess[i][j] = evaluator_evaluate(f_hess[i][j], n, variables, x_data);
}

//avalia hessiana com valores de xdata, coloca resultado em diag_hess
void atualiza_hessiana_v2(void **f_diag_hess, double *diag_hess, double *x_data, char **variables, unsigned int n, unsigned int k){

  //Carrega vetor das k diagonais
  for (unsigned int i=0, l=k/2+1; i<k/2 ;i++, l++)
    for (unsigned int j=0, m=k-l; j<l ;j++, m++)
      diag_hess[k*i+m] = evaluator_evaluate(f_diag_hess[k*i+m], n, variables, x_data);

  unsigned int l=0;
  for (unsigned int i=k/2; i<n-k/2 ;i++, l++){
    unsigned int j;
    for (j=0; j+4<k ;j+=5){
      diag_hess[k*i+j] = evaluator_evaluate(f_diag_hess[k*i+j], n, variables, x_data);
      diag_hess[k*i+j+1] = evaluator_evaluate(f_diag_hess[k*i+j+1], n, variables, x_data);
      diag_hess[k*i+j+2] = evaluator_evaluate(f_diag_hess[k*i+j+2], n, variables, x_data);
      diag_hess[k*i+j+3] = evaluator_evaluate(f_diag_hess[k*i+j+3], n, variables, x_data);
      diag_hess[k*i+j+4] = evaluator_evaluate(f_diag_hess[k*i+j+4], n, variables, x_data);
    }
    for (; j<k ;j++)
      diag_hess[k*i+j] = evaluator_evaluate(f_diag_hess[k*i+j], n, variables, x_data);
  }

  for (unsigned int i=n-k/2; i<n ;i++, l++){
    for (unsigned int j=l, m=0; j<n ;j++, m++){
      diag_hess[k*i+m] = evaluator_evaluate(f_diag_hess[k*i+m], n, variables, x_data);
    }
  }

}


//avalia gradiente com valores de xdata, coloca resultado em x_deriv
void atualiza_gradiente(void **f_prim, double *x_data, double *x_deriv, char **variables, int n){
  for (int i=0; i<n ;i++)
    x_deriv[i] = evaluator_evaluate(f_prim[i], n, variables, x_data);
}

void atualiza_gradiente_v2(void **f_prim, double *x_data, double *x_deriv, char **variables, unsigned int n){
  unsigned int i;
  for (i=0; i+3<n ;i+=4){
    x_deriv[i] = evaluator_evaluate(f_prim[i], n, variables, x_data);
    x_deriv[i+1] = evaluator_evaluate(f_prim[i+1], n, variables, x_data);
    x_deriv[i+2] = evaluator_evaluate(f_prim[i+2], n, variables, x_data);
    x_deriv[i+3] = evaluator_evaluate(f_prim[i+3], n, variables, x_data);
  }
  for (; i<n ;i++)
    x_deriv[i] = evaluator_evaluate(f_prim[i], n, variables, x_data);
}
