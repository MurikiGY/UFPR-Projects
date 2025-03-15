//autores:
//Muriki Gusmão Yamanaka - GRR20203933
//Wellington de Almeida Silva - GRR20205152

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <getopt.h>

#define CONFLICT 1
#define AFINITY 2
#define IN 1
#define OUT 0

//Struct de relacoes entre herois
typedef struct {
  int *relation;   //Lista de relação com outros herois
} relation_t;

typedef struct {
  int x,y;
} edge_t;

void backtracking(int l, int n, relation_t *H, int *X,
    int *auxX, int viab, int opt, int *count, int *globConf){

  //Chegou na folha, testa
  if ( l == n ){
#ifdef DEBUG
    //Avisa que chegou na folha
    printf("Folha [ "); 
    for (int i=0; i<n ;i++)
      printf("%d ", auxX[i]);
    printf("]\n");
#endif

    //Conta conflitos existentes
    int count = 0;
    for (int i=0; i<n ;i++)
      for (int j=i+1; j<n ;j++)
        //Se houver conflito e ambos estão no mesmo grupo
        if ((H[i].relation[j]==CONFLICT) && (auxX[i]==auxX[j]))
          count++;

    //Se não calculou nenhum conflito ou achou uma solucao melhor
    if ( ((*globConf)==-1) || (count<(*globConf))){
      (*globConf) = count;
      memcpy(X, auxX, n*sizeof(int));
    }
    return;
  }

  //Teste de viabilidade
  //Retorna se afinidade em grupos diferentes
  if ( viab )
    for (int i=0; i<l ;i++)
      for (int j=i+1; j<l ;j++)
        //Se encontrou afinidade e grupos diferentes
        if ((H[i].relation[j]==AFINITY) && (auxX[i]!=auxX[j]))
          return;

  //Teste de otimalidade
  if ( opt )
    if ( (*globConf) > -1){
      //Calcula conflitos gerados ate o momento
      int confCount = 0;
      for (int i=0; i<l ;i++)
        for (int j=i+1; j<n ;j++)
          //Se houver conflito e ambos estão no mesmo grupo
          if ((H[i].relation[j]==CONFLICT) && (auxX[i]==auxX[j]))
            confCount++;
      if ( confCount >= (*globConf) )
        return;
    }

  //Testa funcao limitante

  //Desce para a esquerda da arvore
  auxX[l] = IN;
  (*count)++;
  backtracking(l+1, n, H, X, auxX, viab, opt, count, globConf);

  //Desce para a direita da arvore
  auxX[l] = OUT;
  (*count)++;
  backtracking(l+1, n, H, X, auxX, viab, opt, count, globConf);

  return;
}


int main(int argc, char **argv){
  int n, k, m;              //Valores de entrada
  relation_t *heroes;       //Estrutura de herois
  int *solX, *auxX;         //Vetor percorre solução e auxiliar
  int viab = 1, opt = 1;    //Flags de algoritmo
  int guedesFunc = 0;       //Flag de algoritmo dado pelo professor
  int nodeCount = 1;        //Contador de nós que percorreu
  int conflictCount = -1;   //Menor conflito encontrado

  //Teste de parametros
  int option;
  while ( (option = getopt(argc, argv, "foa")) != -1 )
    switch (option){
      case 'f':  //Desliga viabilidade
        viab = 0;
        break;
      case 'o':  //Desliga otimalidade
        opt = 0;
        break;
      case 'a':  //Ativa função do professor
        guedesFunc = 1;
        break;
    }

  //Le entrada primaria
  scanf("%d", &n); scanf("%d", &k); scanf("%d", &m);

  //Aloca herois
  heroes = calloc( n, sizeof(relation_t));
  solX = calloc( n, sizeof(int));
  auxX = calloc( n, sizeof(int));
  if ( !heroes || !solX ) return -1;
  for (int i=0; i<n ;i++)
    heroes[i].relation = calloc( n, sizeof(int));

  int aux1, aux2;
  //Le relacoes de conflito
  for (int i=0; i<k ;i++){
    scanf("%d %d", &aux1, &aux2);
    heroes[aux1-1].relation[aux2-1] = CONFLICT;
    heroes[aux2-1].relation[aux1-1] = CONFLICT;
  }
  //Le relacoes de afinidade
  for (int i=0; i<m ;i++){
    scanf("%d %d", &aux1, &aux2);
    heroes[aux1-1].relation[aux2-1] = AFINITY;
    heroes[aux2-1].relation[aux1-1] = AFINITY;
  }

#ifdef DEBUG
  //Lista de relações
  for (int i=0; i<n ;i++)
    for (int j=0; j<n ;j++)
      if ( heroes[i].relation[j] == CONFLICT )
        printf("Heroi %d tem conflito com %d\n", i+1, j+1);
      else if ( heroes[i].relation[j] == AFINITY )
        printf("Heroi %d tem afinidade com %d\n", i+1, j+1);
#endif

  solX[0] = IN;  //Heroi 1 sempre esta no primeiro grupo
  auxX[0] = IN;
  backtracking(1, n, heroes, solX, auxX, viab, opt, &nodeCount, &conflictCount);

  printf("Resultado:\n");
  printf("Percorreu %d nós da arvore\n", nodeCount);
  printf("Solução encontrada com %d conflitos\n", conflictCount);
  for (int i=0; i<n ;i++){
    if (solX[i] == 1)
      printf(" %d ", i+1);
  }
  //for (int i=0; i<n ;i++)
  //  printf(" %d ", solX[i]);
  printf("\n");

  //Libera estruturas
  for (int i=0; i<n ;i++)
    free(heroes[i].relation);
  free(heroes);
  free(solX);
  free(auxX);
  return 0;
}

//Observações:
//Vetor de herois vai de 0 a n-1 representando os n herois
//Vetor de solX vai de 0 a n-1 também representando os n herois
//Considera-se que o primeiro herói sempre esta no grupo 1
