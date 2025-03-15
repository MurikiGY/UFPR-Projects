// GRR20203933 - Muriki Gusmao Yamanaka
// GRR20205152 - Wellington de Almeida Silva
#ifndef _LS_SOLVER_
#define _LS_SOLVER_

//Retorna maior valor da coluna i a partir de i a n
int encontra_Max(double **A, int n, int i);

//Troca linhas de uma matriz
void troca_linha(double **A, int i, int pivo);

//Transforma matriz A em L e U juntas
//deixa no vetor 'trocas' o mapemento das linhas
//originais, apos as trocas do pivoteamento
void fatoracao_LU(double **A, int n);

//vec: vetor otimizado para guardar k-diagonais
//n: quantidade de linhas da matriz original
//k: k do k-diagonal
void fatoracao_LU_otimizado(double *vec, int n, int k);

//Resolve sistema triangular superior
void retrossubs_up(double **A, double *b, double *x, int n);

//Resolve sistema triangular superior
void retrossubs_up_otimizado(double *vec, double *b, double *x, unsigned int n, unsigned int k);

//Resolve sistema triangular inferior
void retrossubs_down(double **A, double *b, double *x, int n);

//Resolve sistema triangular inferior
void retrossubs_down_otimizado(double *vec, double *b, double *x, unsigned int n, unsigned int k);

//Ajusta vetor b com base no vetor de trocas
void vetor_troca_pivoteamento(int *troca, double **b, int n);

//avalia hessiana com valores de xdata, coloca resultado em x_hess
void atualiza_hessiana(void ***f_hess, double **x_hess, double *x_data, char **variables, int n);

//avalia hessiana com valores de xdata, coloca resultado em diag_hess
void atualiza_hessiana_v2(void **f_diag_hess, double *diag_hess, double *x_data, char **variables, unsigned int n, unsigned int k);

//avalia gradiente com valores de xdata, coloca resultado em x_deriv
void atualiza_gradiente(void **f_prim, double *x_data, double *x_deriv, char **variables, int n);

//avalia gradiente com valores de xdata, coloca resultado em x_deriv com Unroll
void atualiza_gradiente_v2(void **f_prim, double *x_data, double *x_deriv, char **variables, unsigned int n);

#endif
