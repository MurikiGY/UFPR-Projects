/*********************************************************
 * Implementação da biblioteca matheval específica
 * para a Variante da Função de Rosenbrock usada no
 * Trabalho 01 da disciplina Introdução à Computação
 * Científica - DInf - UFPR. 
 *
 * Prof. Guilherme Derenievicz
 *
 * Detalhes:
 * - string f não é usada em evaluator_create
 * - __dF armazena os índices dos argumentos:
 *          - se ambos forem zero   => função original
 *          - se idx_var_d_1st != 0 => derivada primeira
 *          - se idx_var_d_2nd != 0 => derivada segunda
 * - considera que as variáveis são x1, x2, ..., xn
 * - considera k = 5 (fixo)
 * - vetor var não é usado em evaluator_evaluate
 *          - considera que x é um vetor de tamanho n com
 *            x: [x1,x2,...,xn]
 *********************************************************/

#ifndef __MYMATHEVAL__
#define __MYMATHEVAL__

#include <stdio.h>
#include <stdlib.h>

typedef struct {
	int idx_var_d_1st;
	int idx_var_d_2nd;
} __dF;

void *evaluator_create(char *f);
void *evaluator_derivative(void *p, char *x);
void evaluator_destroy(void *p);
double evaluator_evaluate(void *p, int n, char **var, double *x);
char *evaluator_get_string(void *p);

double __Rosenbrock(double *x, int n);
double __Rosenbrock_d_1st(double *x, int n, int i);
double __Rosenbrock_d_2nd(double *x, int n, int i, int j);

#endif
