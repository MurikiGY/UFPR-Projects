// GRR20203933 - Muriki Gusmao Yamanaka
// GRR20205152 - Wellington de Almeida Silva

#include "matrix.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

//Aloca matrix de n linhas e m colunas
double** alocaMatriz(int n, int m){
  double** mat;

  if ( ! (mat = malloc( n*sizeof(double) )) ) return NULL;

  for (int i = 0; i < n; i++)
    if ( !( mat[i] = malloc(m * sizeof(double)) ) ){
      for (int j = 0; j < i; j++)
        free(mat[i]);
      free(mat);
      return NULL;
    }

  return mat;
}

//recebe matriz triangularizada de coeficientes de um SL e um vetor de solucao para o SL
//plota solucao do sistema, e retorna vetor com o resultado
double* plotaSolucao(double** A, double* x, int size){
  double* y;
  if ( ! (y = malloc( size*sizeof(double) )) )
    return NULL;

  for (int i = 0; i < size; i++)
    for (int j = i; j < size; j++)
      y[i] += A[i][j]*x[j];

  return y;
}

//libera matriz nxm mat
void liberaMatriz(double** mat, int n){
  for (int i = 0; i < n; i++)
    free( mat[i] );
  free(mat);
}

//cria uma copia da matriz sizexsize mat e a retorna
double** copiaMatriz(double** mat, int size){
  double** copia = alocaMatriz(size, size);
  if (!copia)
    return NULL;

  for (int i = 0; i < size; i++)
    for (int j = 0; j < size; j++)
      copia[i][j] = mat[i][j];

  return copia;
}

//vector point multiplication
double** mat_mult(double** mat1, int n1, int m1, double** mat2, int n2, int m2){
  double** mult = alocaMatriz(n1, m2);

  double* aux_mult_lin_col;
  if ( !(aux_mult_lin_col = malloc (sizeof(double)*m1 )) ) return NULL;

  for (int i = 0; i < n1; i++)
    for (int j = 0; j < m2; j++){
      mult[i][j] = 0.0;

      //---------------------
      //for increased precision
      for (int k = 0; k < m1; k++)
        aux_mult_lin_col[k] = mat1[i][k]*mat2[k][j];
      for (int k = 0; k < m1; k++)
        mult[i][j] += aux_mult_lin_col[k];
      //---------------------
    }

  free(aux_mult_lin_col);

  return mult;
}

//Subtrai matrizes e retorna
double** mat_sub(double** mat1, double** mat2, int n, int m){
  double** sub = alocaMatriz(n, m);
  for (int i = 0; i < n; i++)
    for (int j = 0; j < m; j++)
      sub[i][j] = mat1[i][j]-mat2[i][j];
  return sub;
}

//Soma matrizes e retorna
double** mat_sum(double** mat1, double** mat2, int n, int m){
  double** sum = alocaMatriz(n, m);
  for (int i = 0; i < n; i++)
    for (int j = 0; j < m; j++){
      sum[i][j] = mat1[i][j]+mat2[i][j];
    }
  return sum;
}

void mat_sum_overWrite(double** mat1, double** mat2, double** sum, int n, int m){
  for (int i = 0; i < n; i++)
    for (int j = 0; j < m; j++)
      sum[i][j] = mat1[i][j]+mat2[i][j];
}

//Multiplica por escalar e retorna
double** mat_mult_scalar(double** mat, int n, int m, double x){
  double** mult = alocaMatriz(n, m);
  for (int i = 0; i < n; i++)
    for (int j = 0; j < m; j++)
      mult[i][j] = mat[i][j]*x;
  return mult;
}

//Retorna copia de um vetor
double *copiaVetor(double *vec, int n){
  double *v_cpy;
  if ( !(v_cpy = malloc (sizeof(double)*n )) ) return NULL;
  for (int i=0; i<n ;i++)
    v_cpy[i] = vec[i];
  return v_cpy;
}

//subtrai vetores e retorna
double* vector_sub(double* v1, double* v2, int n){
  double* vsub;
  if (! (vsub = malloc(sizeof(double) * n)) ) return NULL;
  for (int i = 0; i < n; i++)
    vsub[i] = v1[i]-v2[i];
  return vsub;
}

//soma vetores e retorna
double* vector_sum(double* v1, double* v2, int n){
  double* vsum;
  if (! (vsum = malloc(sizeof(double)*n)) ) return NULL;
  for (int i = 0; i < n; i++)
    vsum[i] = v1[i]+v2[i];
  return vsum;
}

//passa vetor tamanho size para matriz nx1
double** vector_to_mat_col(double* vec, int size){
  double** mat = alocaMatriz(size, 1);
  for (int i = 0; i < size; i++)
    mat[i][0] = vec[i];
  return mat;
}

//passa vetor tamanho size para matriz 1xn
double** vector_to_mat_lin(double* vec, int size){
  double** mat = alocaMatriz(1, size);
  for (int j = 0; j < size; j++)
    mat[0][j] = vec[j];
  return mat;
}

//Soma dois vetores e sobrescreve em v1
void vector_sum_overWrite(double *v1, double *v2, int n){
  for (int i=0; i<n ;i++)
    v1[i] += v2[i];
}

//Soma dois vetores e sobrescreve em v1
void vector_sum_overWrite_v2(double *v1, double *v2, unsigned int n){
  unsigned int i;
  for (i=0; i+3<n ;i+=4){
    v1[i] += v2[i];
    v1[i+1] += v2[i+1];
    v1[i+2] += v2[i+2];
    v1[i+3] += v2[i+3];
  }
  for (; i<n ;i++)
    v1[i] += v2[i];
}

//Multiplica vetor a um escalar
void vector_mult_scalar(double *vec, int n, double x){
  for (int i=0; i<n ;i++)
    vec[i] = vec[i]*x;
}

//Multiplica vetor a um escalar
void vector_mult_scalar_v2(double *vec, unsigned int n, double x){
  unsigned int i;
  for (i=0; i+3<n ;i+=4){
    vec[i] = vec[i]*x;
    vec[i+1] = vec[i+1]*x;
    vec[i+2] = vec[i+2]*x;
    vec[i+3] = vec[i+3]*x;
  }
  for (; i<n ;i++)
    vec[i] = vec[i]*x;
}

//Retorna a norma de um vetor
double norma_vetor(double *vec, int n){
  double soma = 0;
  for (int i=0; i<n; i++)
    soma += vec[i]*vec[i];
  return sqrt(soma);
}

//Retorna a norma de um vetor
double sqt_vetor(double *vec, unsigned int n){
  double soma = 0;
  unsigned int i;
  for (i=0; i+3<n; i+=4){
    soma += vec[i]*vec[i];
    soma += vec[i+1]*vec[i+1];
    soma += vec[i+2]*vec[i+2];
    soma += vec[i+3]*vec[i+3];
  }
  for (; i<n; i++)
    soma += vec[i]*vec[i];
  return soma;
}
