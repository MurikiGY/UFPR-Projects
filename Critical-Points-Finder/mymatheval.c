/*****************************************************
 * Implementação da biblioteca matheval específica
 * para a Variante da Função de Rosenbrock usada no
 * Trabalho 01 da disciplina Introdução à Computação
 * Científica - DInf - UFPR.
 *
 * Prof. Guilherme Derenievicz
 *****************************************************/

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

#include "mymatheval.h"

#include <stdio.h>
#include <stdlib.h>

void *evaluator_create(char *f) {
	__dF *p = (__dF *) malloc(sizeof(__dF));
	p->idx_var_d_1st = 0;
	p->idx_var_d_2nd = 0;
	return (void *) p;
}

void *evaluator_derivative(void *p, char *x) {
	__dF *dp = (__dF *) malloc(sizeof(__dF));

	__dF *pf = (__dF *) p;

	if (!pf->idx_var_d_1st) {
		dp->idx_var_d_1st = atoi(x+1);
		dp->idx_var_d_2nd = 0;
	}
	else {
		dp->idx_var_d_1st = pf->idx_var_d_1st;
		dp->idx_var_d_2nd = atoi(x+1);
	}

	return (void *) dp;
}

void evaluator_destroy(void *p) {
	if (p) free(p);
}

double evaluator_evaluate(void *p, int n, char **var, double *x) {
	__dF *pf = (__dF *) p;
	if (!pf->idx_var_d_1st)
		return __Rosenbrock(x, n);
	else if (!pf->idx_var_d_2nd)
		return __Rosenbrock_d_1st(x, n, pf->idx_var_d_1st);
	else
		return __Rosenbrock_d_2nd(x, n, pf->idx_var_d_1st, pf->idx_var_d_2nd);
}

char *evaluator_get_string(void *p) {
	return "Variante de Rosenbrock com k=5";
}

double __Rosenbrock(double *x, int n) {
	double s = 0.0;
	for (int i = 0; i < n-2; ++i) {
		double t = x[i]-x[i+1]-(x[i+2]*x[i+2]);
		s += t*t;
	}
	return s;
}

double __Rosenbrock_d_1st(double *x, int n, int i) {

	if (i == 1) {
		double x2q = x[2]*x[2];
		return 2*(x[0]-x[1]-x2q);
	}
	if (i == 2) {
		double x2q = x[2]*x[2];
		double x3q = x[3]*x[3];
		return -2*(x[0]-2*x[1]-x2q+x[2]+x3q);
	}
	if (i == n-1) {
		double xn2c = x[n-2]*x[n-2]*x[n-2];
		double xn1q = x[n-1]*x[n-1];
		return 2*(-2*x[n-4]*x[n-2]+x[n-3]*(2*x[n-2]-1)+2*xn2c+x[n-2]+xn1q);
	}
	if (i == n) {
		double xn1q = x[n-1]*x[n-1];
		return -4*x[n-1]*(x[n-3]-x[n-2]-xn1q);
	}

	i--;
	double xic = x[i]*x[i]*x[i];
	double xi1q = x[i+1]*x[i+1];
	double xi2q = x[i+2]*x[i+2];
	return -2*(2*(x[i-2]-1)*x[i]-2*x[i-1]*x[i]+x[i-1]-2*xic-xi1q+x[i+1]+xi2q);
}

double __Rosenbrock_d_2nd(double *x, int n, int i, int j) {

	if (j == i+2)
		return -4*x[j-1];
	if (j == i-2)
		return -4*x[i-1];
	if (j == i+1) {
		if (j == 2)
			return -2;
		if (j == n)
			return 4*x[n-1];
		return 4*x[j-1]-2;
	}
	if (j == i-1) {
		if (j == 1)
			return -2;
		if (j == n-1)
			return 4*x[n-1];
		return 4*x[i-1]-2;
	}
	if (j == i) {
		if (i == 1)
			return 2;
		if (i == 2)
			return 4;
        if (i == n-1) {
		    double xi1q = x[i-1]*x[i-1];
		    return -4*(x[i-3]-x[i-2]-xi1q)+8*xi1q+2;
		}
	    if (i == n) {
			double xn1q = x[n-1]*x[n-1];
			return 8*xn1q-4*(x[n-3]-x[n-2]-xn1q);
		}
		double xi1q = x[i-1]*x[i-1];
		return -4*(x[i-3]-x[i-2]-xi1q)+8*xi1q+4;
	}
	return 0;
}
