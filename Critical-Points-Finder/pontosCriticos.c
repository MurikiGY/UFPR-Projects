// GRR20203933 - Muriki Gusmao Yamanaka
// GRR20205152 - Wellington de Almeida Silva

#include "matrix.h"
#include "ls_solver.h"
//#include "quasi-newton.h"
#include "metodos.h"
#include "mymatheval.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <dirent.h>
#include <math.h>
#include <likwid.h>
#include <likwid-marker.h>


int main(int argc, char **argv){
  FILE *outfile = NULL;                 //Arquivo de saida
  int n, k, max_it, hess_steps;         //Parametros do programa
  int sum1, sum2;                       //Limites dos somatorios
                                        
  int num_it1;                          //Numero de iteracoes necessarias para satisfazer testes
  unsigned int num_it2;                 //Numero de iteracoes necessarias para satisfazer testes
                                        
  double x_ini, eps;                    //Parametros do programa
  char **variables;                     //Vetor de variaveis
  char *eq;                             //String da equação
  double **newton1, **newton2;          //Matriz de coeficientes de cada metodo
  //double *x_data, *x_grad, **x_hess;    //Vetores e matrizes de dados
  //void *f, **f_prim, ***f_hess;         //Equação e vetor de derivadas e hessiana
  
  //Dev
  //double *k_diag;
  functions_t func;
  data_t data;
  
  //---------------------
  //leitura de parametros
  int option;
  while ( (option = getopt(argc, argv, "o:")) != -1 )
    switch (option){
      case 'o':
        outfile = fopen(optarg, "a");
        if ( !outfile ){
          fprintf(stderr,"Falha na criação do arquivo %s\n", optarg);
          exit(1); }
        break;
    }
  
  //Leitura dos dodas
  scanf("%d %d %lf %lf %d %d", &n, &k, &x_ini, &eps, &max_it, &hess_steps);
  //---------------------

  //---------------------
  //Gera string com equacao
  sum1 = n - k/2;
  sum2 = k/2;
  eq = str_equation(sum1, sum2);
  //Aloca e inicia ponteiros de equacoes (derivadas e hessiana)
  //pointer_equation(&f, &f_prim, &f_hess, &variables, n, eq);
  pointer_equation_v2(&func, &variables, n, k, eq);
  //---------------------

  //---------------------
  //Alocacao das variaveis de dados
  data.x_data = calloc(n, sizeof(double));
  data.x_grad = calloc(n, sizeof(double));
  data.diag_hess = calloc(n*k, sizeof(double));

  data.x_hess = calloc(n, sizeof(double *));
  for (int i=0; i<n ;i++)
    data.x_hess[i] = calloc(n, sizeof(double));

  newton1 = alocaMatriz(max_it+1, n);    //Matriz de saida do metodo1
  newton2 = alocaMatriz(max_it+1, n);    //Matriz de saida do metodo2
  //---------------------

  //---------------------
  //Carregamento dos dados iniciais
  for (int i=0; i<n ;i++)
    data.x_data[i] = x_ini;
  for (int i=0; i<n ;i++){
    data.x_grad[i] = evaluator_evaluate(func.f_grad[i], n, variables, data.x_data);
    for (int j=0; j<n ;j++)
      data.x_hess[i][j] = evaluator_evaluate(func.f_hess[i][j], n, variables, data.x_data);
  }

  //Carrega vetor das k diagonais
  for (int i=0, l=k/2+1; i<k/2 ;i++, l++)
    for (int j=0, m=k-l; j<l ;j++, m++)
      data.diag_hess[k*i+m] = data.x_hess[i][j];
  int l=0;
  for (int i=k/2; i<n-k/2 ;i++, l++)
    for (int j=0; j<k ;j++)
      data.diag_hess[k*i+j] = data.x_hess[i][j+l];
  for (int i=n-k/2; i<n ;i++, l++)
    for (int j=l, m=0; j<n ;j++, m++)
      data.diag_hess[k*i+m] = data.x_hess[i][j];

  //---------------------
  //Execucao dos metodos
  LIKWID_MARKER_INIT;

  //Metodo de Newton modificado
  LIKWID_MARKER_START("NEWTON_MOD");
  newton_modificado(func, data, eps, max_it, hess_steps, variables, n, &newton1, &num_it1);
  LIKWID_MARKER_STOP("NEWTON_MOD");

  //Restaura data
  for (int i=0; i<n ;i++)
    data.x_data[i] = x_ini;
  for (int i=0; i<n ;i++)
    data.x_grad[i] = evaluator_evaluate(func.f_grad[i], n, variables, data.x_data);

  //Metodo de Newton modificado
  LIKWID_MARKER_START("NEWTON_MOD_OPT");
  newton_otimizado(func, data, eps, max_it, hess_steps, variables, n, k, &newton2, &num_it2);
  LIKWID_MARKER_STOP("NEWTON_MOD_OPT");

  LIKWID_MARKER_CLOSE;
  //---------------------

  //---------------------
  //Impressao dos dados
  if (outfile){
      fprintf(outfile,"%d %d %lf %lf %d %d\n#\n", n, k, x_ini, eps, max_it, hess_steps);
      fprintf(outfile,"Iteração | Newton Modificado | Newton Otimizado\n");
      for (int i=0; i<=max_it ;i++){
        fprintf(outfile, "%4d       |", i);
        if ( i <= num_it1)
            fprintf(outfile,"   %15e |", evaluator_evaluate(func.f, n, variables, newton1[i]));
        else
            fprintf(outfile,"                   |");

        if ( i <= num_it2)
            fprintf(outfile,"  %15e", evaluator_evaluate(func.f, n, variables, newton2[i]));
        fprintf(outfile,"\n");
      }
  }
  else{
      printf("%d %d %lf %lf %d %d\n#\n", n, k, x_ini, eps, max_it, hess_steps);
      printf("Iteração | Newton Modificado | Newton Otimizado\n");
      for (int i=0; i<=max_it ;i++){
        printf("   %4d  |", i);
        if ( i <= num_it1)
            printf("   %15e |", evaluator_evaluate(func.f, n, variables, newton1[i]));
        else
            printf("                   |");

        if ( i <= num_it2)
            printf("  %15e", evaluator_evaluate(func.f, n, variables, newton2[i]));
        printf("\n");
      }
  }
  //---------------------

  //---------------------
  //Destroi tudo
  free(eq);
  evaluator_destroy (func.f);
  for (int i=0; i<n ;i++)
    evaluator_destroy(func.f_grad[i]);
  for (int i=0; i<n ;i++)
    for (int j=0; j<n ;j++)
      evaluator_destroy(func.f_hess[i][j]);
  free(data.x_data);
  free(data.x_grad);
  free(data.diag_hess);
  liberaMatriz(newton1, max_it+1);
  liberaMatriz(newton2, max_it+1);
  //---------------------
  return 0;
}
