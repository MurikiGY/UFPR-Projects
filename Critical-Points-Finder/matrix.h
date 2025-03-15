// GRR20203933 - Muriki Gusmao Yamanaka
// GRR20205152 - Wellington de Almeida Silva
#ifndef _MATRIX_
#define _MATRIX_

//Aloca matriz de n linhas e m colunas
double** alocaMatriz(int n, int m);

//recebe matriz triangularizada de coeficientes de um SL e um vetor de solucao para o SL
//plota solucao do sistema, e retorna vetor com o resultado
double* plotaSolucao(double** A, double* x, int size);

//libera matriz nxm mat
void liberaMatriz(double** mat, int n);

//cria uma copia da matriz sizexsize mat e a retorna
double** copiaMatriz(double** mat, int size);

//Multiplica matrizes e retorna
double** mat_mult(double** mat1, int n1, int m1, double** mat2, int n2, int m2);

//Subtrai matrizes e retorna
double** mat_sub(double** mat1, double** mat2, int n, int m);

//Soma matrizes e retorna
double** mat_sum(double** mat1, double** mat2, int n, int m);

//Soma matrizes coloca resultado em sum
void mat_sum_overWrite(double** mat1, double** mat2, double** sum, int n, int m);

//multiplica matriz mat nxm pelo escalar x
double** mat_mult_scalar(double** mat, int n, int m, double x);

//multiplica matriz mat nxm pelo escalar x com Unroll
void vector_mult_scalar_v2(double *vec, unsigned int n, double x);

//Retorna copia de um vetor
double *copiaVetor(double *vec, int n);

//subtrai vetores
double* vector_sub(double* v1, double* v2, int n);

//soma vetores
double* vector_sum(double* v1, double* v2, int n);

//passa vetor tamanho n para matriz nx1
double** vector_to_mat_col(double* vec, int size);

//passa vetor tamanho n para matriz 1xn
double** vector_to_mat_lin(double* vec, int size);

//Soma dois vetores e sobrescreve em v1
void vector_sum_overWrite(double *v1, double *v2, int n);

//Soma dois vetores e sobrescreve em v1 com Unroll
void vector_sum_overWrite_v2(double *v1, double *v2, unsigned int n);

//Multiplica vetor a um escalar
void vector_mult_scalar(double *vec, int n, double x);

//Retorna a norma de um vetor
double norma_vetor(double *vec, int n);

//Retorna soma dos quadrados dos elementos do vetor
double sqt_vetor(double *vec, unsigned int n);

#endif
