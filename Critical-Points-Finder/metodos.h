// GRR20203933 - Muriki Gusmao Yamanaka
// GRR20205152 - Wellington de Almeida Silva
#ifndef _METODOS_
#define _METODOS_

#define AUX_SIZE 30   //Tamanho da string auxiliar
#define FUNC_VAR 5

//Ponteiro de funcoes da matheval
typedef struct rosenbrock {
  void *f;                //Funcao de rosenbrock
  void **f_grad;          //Funcao gradiente
  void **f_diag_hess;      //Funcao da hessiana otimizada
  void ***f_hess;         //Funcao da hessiana
} functions_t;

//Estruturas dos dados
typedef struct data {
  double *x_data;         //Vetor de dados
  double *x_grad;         //Gradiente do vetor de dados
  double **x_hess;        //Hessiana do vetor de dados
  double *diag_hess;     //Hessiana diagonal
} data_t;

//Gera e Retorna a string da equação
char* str_equation(int sum1, int sum2);

//Aloca e inicia ponteiros de equacoes e variaveis
void pointer_equation(void **f, void ***f_prim, void ****f_hess, char ***variables, int n, char *str);

//Novo iniciador de funcoes
void pointer_equation_v2(functions_t *func, char ***variables, int n, int k, char *str);

//guarda valores do vetor dados na linha itr da matriz
void save_state(double *data, double ***matrix, int n, int itr);

//encontra e retorna raizes da funcao cujo gradiente e hessiana sao passados por parametro
//por meio do metodo de newton modificado
double* newton_modificado(functions_t func, data_t data, double eps, int max_it,
                          int hess_steps, char**variables, int n, double ***mat_out, int* num_it);

//Mesmo que metodo de newton modificado, porém com otimizacoes
double* newton_otimizado(functions_t func, data_t data, double eps, unsigned int max_it, unsigned int hess_steps, 
                         char**variables, unsigned int n, unsigned int k, double ***mat_out, unsigned int* num_it);

#endif 
