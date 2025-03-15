// GRR20203933 - Muriki Gusmao Yamanaka
// GRR20205152 - Wellington de Almeida Silva

#include "metodos.h"
#include "matrix.h"
#include "ls_solver.h"
//#include "quasi-newton.h"
#include "mymatheval.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Gera e Retorna a string da equação
char* str_equation(int sum1, int sum2){
  char *str;

  //Alocacao da string de equacao
  str = calloc( (26*sum1 + 13*sum1*sum2), sizeof(char) );
  if ( !str ){ fprintf(stderr, "Erro na string da função\n"); exit(3);}
  str[0] = '\0';

  //Gera a string da equação
  for (int i=1; i<=sum1 ;i++){
    char aux[AUX_SIZE];
    snprintf(aux, AUX_SIZE, "(x%d - (", i);
    strcat(str, aux);

    for (int j=1; j<sum2 ;j++){
      snprintf(aux, AUX_SIZE, "(x%d)^%d + ", i+j, j);
      strcat(str, aux);
    }

    if ( i == sum1 ){
      snprintf(aux, AUX_SIZE, "(x%d)^%d))^2", i+sum2, sum2);
      strcat(str, aux);
    } else {
      snprintf(aux, AUX_SIZE, "(x%d)^%d))^2 + ", i+sum2, sum2);
      strcat(str, aux);
    }
  }
  return str;
}


void get_variables(char ***var, int n){
  (*var) = malloc( n*sizeof(char *));
  for (int i=0; i<n ;i++)
    (*var)[i] = malloc( FUNC_VAR*sizeof(char) );

  for (int i=0; i<n ;i++)
    snprintf((*var)[i], AUX_SIZE, "x%d", i+1);
}


//Aloca e inicia ponteiros de equacoes e variaveis
void pointer_equation(void **f, void ***f_prim, void ****f_hess, char ***variables, int n, char *str){

  //Cria ponteiro da equacao e inicia vetor de variaveis
  *f = evaluator_create (str);
  get_variables (variables, n);

#ifdef DEBUG
  printf("Variaveis: ");
  for (int i=0; i<n ;i++)
    printf("%s ", (*variables)[i]);
  printf("\n");
#endif

  //Alocacao do vetor de derivadas e matriz hessiana
  *f_prim = calloc( n, sizeof(void *) );
  if ( !*f_prim ){ fprintf(stderr, "Erro no vetor de derivadas\n"); exit(2); }

  *f_hess = calloc( n, sizeof(void **) );
  if ( !*f_hess ){ fprintf(stderr, "Erro na matriz de hessiana\n"); free(*f_prim); exit(2); }
  for (int i=0; i<n; i++)
    (*f_hess)[i] = calloc( n, sizeof(void *) );
                                                                                   //
  //Inicia vetor de derivadas e matriz hessiana
  for (int i=0; i<n ;i++){
    (*f_prim)[i] = evaluator_derivative(*f, (*variables)[i]); 
    for (int j=0; j<n ;j++)
      (*f_hess)[i][j] = evaluator_derivative( (*f_prim)[i], (*variables)[j] );
  }
}


//Novo iniciador de funcoes
void pointer_equation_v2(functions_t *func, char ***variables, int n, int k, char *str){

  //Cria ponteiro da equacao e inicia vetor de variaveis
  func->f = evaluator_create (str);
  get_variables (variables, n);

#ifdef DEBUG
  printf("Variaveis: ");
  for (int i=0; i<n ;i++)
    printf("%s ", (*variables)[i]);
  printf("\n");
#endif

  //Alocacao do gradiente, matriz hessiana e diagonal hessiana
  func->f_grad = calloc( n, sizeof(void *) );
  if ( !func->f_grad ){ fprintf(stderr, "Erro no vetor de derivadas\n"); exit(2); }
  func->f_diag_hess = calloc(n*k, sizeof(void *));
  if ( !func->f_diag_hess ){ fprintf(stderr, "Erro no vetor de hessiana\n"); exit(2); }
  func->f_hess = calloc( n, sizeof(void **) );
  if ( !func->f_hess ){ fprintf(stderr, "Erro na matriz de hessiana\n"); free(func->f_grad); exit(2); }
  for (int i=0; i<n; i++)
    (func->f_hess)[i] = calloc( n, sizeof(void *) );

  // ---------
  
  //Inicia gradiente e hessiana
  for (int i=0; i<n ;i++){
    (func->f_grad)[i] = evaluator_derivative(func->f, (*variables)[i]); 
    for (int j=0; j<n ;j++)
      (func->f_hess)[i][j] = evaluator_derivative( (func->f_grad)[i], (*variables)[j] );
  }

  //Inicia diagonal hessiana
  //Carrega vetor das k diagonais
  for (int i=0, l=k/2+1; i<k/2 ;i++, l++)
    for (int j=0, m=k-l; j<l ;j++, m++)
      func->f_diag_hess[k*i+m] = evaluator_derivative( (func->f_grad)[i], (*variables)[j]);
  int l=0;
  for (int i=k/2; i<n-k/2 ;i++, l++)
    for (int j=0; j<k ;j++)
      func->f_diag_hess[k*i+j] = evaluator_derivative( (func->f_grad)[i], (*variables)[j+l]);
  for (int i=n-k/2; i<n ;i++, l++)
    for (int j=l, m=0; j<n ;j++, m++)
      func->f_diag_hess[k*i+m] = evaluator_derivative( (func->f_grad)[i], (*variables)[j]);
}


//guarda valores do vetor dados na linha itr da matriz
void save_state(double *data, double ***matrix, int n, int itr){
  for (int i=0; i<n ;i++)
    (*matrix)[itr][i] = data[i];
}

//encontra e retorna raizes da funcao cujo gradiente e hessiana sao passados por parametro
//por meio do metodo de newton modificado
double* newton_modificado(functions_t func, data_t data, double eps, int max_it,
                          int hess_steps, char**variables, int n, double ***mat_out, int* num_it){
  double *delta = calloc( n, sizeof(double) );        //Solucao do sistema linear
  double *y_vect = calloc( n, sizeof(double) );       //Vetor da saida do SL em L
  double *data_cpy_old = calloc( n, sizeof(double) );  
  *num_it=max_it;
                                                      
  //Salva estado dos vetores
  save_state(data.x_data, mat_out, n, 0); //nota: alterado para salvar estado inicial e final

  for (int i=0; i<max_it ;i++){
    atualiza_gradiente(func.f_grad, data.x_data, data.x_grad, variables, n);
    vector_mult_scalar(data.x_grad, n, -1);
 
    //Se norma do Vetor < epsilon
    if ( norma_vetor(data.x_grad, n) < eps ){
      free(delta);
      free(y_vect);
      free(data_cpy_old);
      *num_it=i;
      return data.x_data;
    }

    //atualizacao da matriz hessiana e resolucao do sistema linear
    if ( ((i%hess_steps)==0) ){
      //Atualiza e transforma da hessiana em L e U
      atualiza_hessiana(func.f_hess, data.x_hess, data.x_data, variables, n);
#ifdef DEBUG
      printf("\nIteração: %d\n", i);
      printf("Matriz hessiana:\n");
      for (int i=0; i<n ;i++){
          for (int j=0; j<n; j++)
              printf("%lf ", data.x_hess[i][j]);
          printf("\n");
      }
      printf("\n");
#endif
      fatoracao_LU(data.x_hess, n);              
    }
    retrossubs_down(data.x_hess, data.x_grad, y_vect, n);   
    retrossubs_up(data.x_hess, y_vect, delta, n);        
    
    memcpy(data_cpy_old, data.x_data, n*sizeof(double));
    vector_sum_overWrite(data.x_data, delta, n);         
    //Salva estado dos vetores
    save_state(data.x_data, mat_out, n, i+1);

    //calcula delta "efetivo"
    for (int i = 0; i<n; i++)
        delta[i] = data.x_data[i]-data_cpy_old[i];

    //Se norma do delta < epsilon
    if ( norma_vetor(delta, n) < eps ){
      free(delta);
      free(y_vect);
      free(data_cpy_old);
      *num_it=i+1;
      return data.x_data;
    }
  }
    
  free(delta);
  free(y_vect);
  free(data_cpy_old);
  return NULL;
}

//Mesmo que metodo de newton modificado, porém com otimizacoes
double* newton_otimizado(functions_t func, data_t data, double eps, unsigned int max_it, unsigned int hess_steps, 
                         char**variables, unsigned int n, unsigned int k, double ***mat_out, unsigned int* num_it){
  double *delta = calloc( n, sizeof(double) );        //Solucao do sistema linear
  double *y_vect = calloc( n, sizeof(double) );       //Vetor da saida do SL em L
  double *data_cpy_old = calloc( n, sizeof(double) );  
  *num_it=max_it;

  //Salva estado dos vetores
  save_state(data.x_data, mat_out, n, 0); //nota: alterado para salvar estado inicial e final

  for (unsigned int iter=0; iter<max_it ;iter++){
    atualiza_gradiente_v2(func.f_grad, data.x_data, data.x_grad, variables, n);
    vector_mult_scalar_v2(data.x_grad, n, -1);
                                                   
    //Se norma do Vetor < epsilon
    if ( sqt_vetor(data.x_grad, n) < eps*eps ){
      free(delta);
      free(y_vect);
      free(data_cpy_old);
      *num_it=iter;
      return data.x_data;
    }

    //atualizacao da matriz hessiana e resolucao do sistema linear
    if ( !(iter%hess_steps) ){
      //Atualiza e transforma da hessiana em L e U
      atualiza_hessiana_v2(func.f_diag_hess, data.diag_hess, data.x_data, variables, n, k);
#ifdef DEBUG
      printf("\nIteração: %d\n", i);
      printf("Vetor diagonal:\n");
      for (int i=0; i<n ;i++){
          for (int j=0; j<k ;j++)
              printf("%lf ", data.diag_hess[k*i+j]);
          printf("\n");
      }
      printf("\n");
#endif
      fatoracao_LU_otimizado(data.diag_hess, n, k);              
    }
    retrossubs_down_otimizado(data.diag_hess, data.x_grad, y_vect, n, k);   
    retrossubs_up_otimizado(data.diag_hess, y_vect, delta, n, k);
    
    //Atualiza x_data
    memcpy(data_cpy_old, data.x_data, n*sizeof(double));
    vector_sum_overWrite_v2(data.x_data, delta, n);         
    //Salva estado dos vetores
    save_state(data.x_data, mat_out, n, iter+1);

    //calcula delta "efetivo"
    unsigned int i;
    for ( i = 0; i+3<n; i+=4){
        delta[i] = data.x_data[i]-data_cpy_old[i];
        delta[i+1] = data.x_data[i+1]-data_cpy_old[i+1];
        delta[i+2] = data.x_data[i+2]-data_cpy_old[i+2];
        delta[i+3] = data.x_data[i+3]-data_cpy_old[i+3];
    }
    for (; i<n; i++)
        delta[i] = data.x_data[i]-data_cpy_old[i];

    //Se norma do delta < epsilon
    if ( sqt_vetor(delta, n) < eps*eps ){
      free(delta);
      free(y_vect);
      free(data_cpy_old);
      *num_it=iter+1;
      return data.x_data;
    }
  }

  free(delta);
  free(y_vect);
  free(data_cpy_old);
  return NULL;
}

