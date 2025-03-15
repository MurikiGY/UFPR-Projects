//autores:
//Muriki Gusmão Yamanaka - GRR20203933
//Wellington de Almeida Silva - GRR20205152

#include <bits/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <getopt.h>
#include <time.h>

#define MILLION 1000000.0
#define MAX_CYCLE_EDGE_NUM 7
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

int search_edge (edge_t* edges, int n, int x, int y){
#ifdef DEBUG
      printf("buscando (%d,%d) \n",x, y);
#endif
    for (int i=0; i<n; i++){
      //printf("ind %d (%d,%d) == (%d,%d)? \n",i, edges[i].x+1, edges[i].y+1, x, y);
      if (edges[i].x == x && edges[i].y == y){
        return i;
#ifdef DEBUG
        printf("encontrado\n\n");
#endif
      }
    }

#ifdef DEBUG
      printf("nao encontrado\n\n");
#endif

    return -1;
}

int search_vec(int* vet, int n, int num){
    for (int i = 0; i < n; i++)
        if (vet[i] == num)
            return 1;
    return 0;
}

int get_cycle(edge_t* edges, int n, int* cycle, int edge_num, int e, int h){
  if (e == (edge_num-1)){
      int ind = 0;
      if (edges[0].y < h)
        ind = search_edge(edges+1, n-1, edges[0].y, h); 
      else if (edges[0].y > h)
        ind = search_edge(edges+1, n-1, h, edges[0].y);

      if (ind != -1){
        cycle[e] = ind+1;
        return 1;
      }

      return 0;
  }

  for (int i=1; i<n-1; i++){
    if ( ! search_vec(cycle+1, e-1, i) ){
        if (h == edges[i].x ){
          cycle[e] = i;
          if ( get_cycle(edges, n, cycle, edge_num, e+1, edges[i].y) )
              return 1;
        }
        else if ( h == edges[i].y ){
          cycle[e] = i;
          if ( get_cycle(edges, n, cycle, edge_num, e+1, edges[i].x) )
              return 1;
        }
    }
  }

  return 0;
}

void remove_edges(edge_t* vec, int n, int* rem, int rem_n){
    //insertion sort
    int menor, aux;
    for (int i = 0; i < rem_n-1; i++){
        menor = i;
        for (int j = i+1; j < rem_n; j++)
            if (rem[j] < rem[menor])
                menor = j;
        //troca
        aux = rem[i];
        rem[i] = rem[menor];
        rem[menor] = aux;
    }

    //remove edges
    int ind = 0;
    for (int i = 0; i < rem_n ; i++){
        ind = rem[i] - i;
        for (int j = ind; j < n - i -1; j++){
            vec[j] = vec[j+1];
        }
    }
}

//conta ciclos impares sem arestas em comum
int te2(edge_t* edges, int n){
    int cycle[n];
    int cont_cicle = 0;

    int cycle_edge_number=3; //numero de arestas do ciclo a ser buscado
    while ( cycle_edge_number <= MAX_CYCLE_EDGE_NUM ){

      for (int i = 0; i < n-(cycle_edge_number-1); i++){
        cycle[0] = 0;
        //if (edges[i].x != -1){
#ifdef DEBUG
          printf("n: %d i:%d\n", n, i);
          printf("cycle_edge_number: %d\n", cycle_edge_number);
          printf("edges %d: \n", i);
          for (int k = i; k < n; k++)
              printf("- (%d,%d) ", edges[k].x, edges[k].y);
          printf("\n\n");
#endif
          //tests if first edge forms a cycle with 'e' edges
          if ( get_cycle(edges+i, n-i, cycle, cycle_edge_number, 1, edges[i].x) ){
            //"eliminates" edges from triangle
#ifdef DEBUG
            printf("\nFOUND CYCLE! cycle_edge_number: %d\n\n", cycle_edge_number);
#endif
            remove_edges(edges+i, n-i, cycle+1, cycle_edge_number-1);
            n -= (cycle_edge_number-1);
            //for (int j = 0; j < cycle_edge_number; j++){
            //  edges[cycle[j]+i].x = -1;
            //  edges[cycle[j]+i].y = -1;
            //}
            cont_cicle++;
          }
      //}
    }

      cycle_edge_number+=2;
    }
    return cont_cicle;
}


int get_triang(edge_t* edges,int n, int* tri){
  edge_t e1, e2;
  int ind;

  e1 = edges[0];
  for (int i=1; i<n-1; i++){
    e2 = edges[i];
    if (e1.x == e2.x){
      if (e1.y < e2.y){
        if ( (ind = search_edge(edges+1, n-1, e1.y, e2.y)) != -1 ){ 
#ifdef DEBUG
          printf(" tri[0] = 0; tri[1] = %d; tri[2] = %d\n", i, ind+1);
#endif
          tri[0] = 0; tri[1] = i; tri[2] = ind+1;
          return 1;
        }
      }
      else{
        if ( (ind = search_edge(edges+1, n-1, e2.y, e1.y)) != -1 ){
#ifdef DEBUG
            printf(" tri[0] = 0; tri[1] = %d; tri[2] = %d\n", i, ind+1);
#endif
          tri[0] = 0; tri[1] = i; tri[2] = ind+1;
          return 1;
        }
      }
    }
  }

  return 0;
}

int te(edge_t* edges, int n){
  int tri[3];
  int cont_tri = 0;

  for (int i = 0; i < n-1; i++){
    //tests if edge was previously removed
    if ( edges[i].x != -1 ){
      //tests if first edge is in a triagle, puts de indices of the triangle in tri
      if (get_triang(edges+i, n-i, tri)){ 
#ifdef DEBUG
        printf("triangulo: (%d,%d) (%d,%d) (%d,%d)\n", edges[i+tri[0]].x+1, edges[i+tri[0]].y+1, \
          edges[i+tri[1]].x+1, edges[i+tri[1]].y+1, edges[i+tri[2]].x+1, edges[i+tri[2]].y+1);
#endif
        //"eliminates" edges from triangle
        edges[(i+tri[1])].x = -1;
        edges[(i+tri[2])].x = -1;
        cont_tri++;
      }
    }
  }
  return cont_tri;
}

void branchAndBound(int l, int n, relation_t *H, int *X,
    int *auxX, int viab, int opt, int *count, int *globConf, edge_t* edges, int guedesFunc){

  int cont_conf=0;
  for (int i=0; i<l ;i++)
    for (int j=i+1; j<l ;j++)
      if ((H[i].relation[j]==CONFLICT) && (auxX[i]==auxX[j]))
        cont_conf++;

#ifdef DEBUG
  printf("selecionados: %d \n", l);
  for (int i=0; i<l ;i++)
    if (auxX[i])
      printf(" %d ", i+1);
  printf(" | ");
  for (int i=0; i<l ;i++)
    if (!auxX[i])
      printf(" %d ", i+1);
  printf("conflitos: %d \n", cont_conf);
  printf("------------------");
  printf("\n");
#endif

  //Chegou na folha, testa
  if ( l == n ){

    //testa se solucao satisfaz restricoes
    for (int i=0; i<n ;i++)
      for (int j=i+1; j<n ;j++)
        //Se encontrou afinidade e grupos diferentes
        if ((H[i].relation[j]==AFINITY) && (auxX[i]!=auxX[j]))
          return;

    //Se não calculou nenhum conflito ou achou uma solucao melhor
    if ( ((*globConf)==-1) || (cont_conf < (*globConf))){
      (*globConf) = cont_conf;
      memcpy(X, auxX, n*sizeof(int));
    }
    return;
  }

  //Teste de viabilidade
  //Retorna se afinidade em grupos diferentes
  if ( viab ){
    for (int i=0; i<l ;i++)
      for (int j=i+1; j<l ;j++)
        //Se encontrou afinidade e grupos diferentes
        if ((H[i].relation[j]==AFINITY) && (auxX[i]!=auxX[j]))
          return;
  }

  if ( (*globConf) != -1){
      //Teste de otimalidade
      if ( opt ){
          //Calcula conflitos gerados ate o momento
          if ( cont_conf >= (*globConf) )
            return;
      }

      //-----------------------------
      //Testa funcao limitante
      int k = 0;
      //for(int i=0; i<n; i++)
      //  for (int j=l+i; j<n ;j++)
      for(int i=l; i<n; i++)
        for (int j=i+1; j<n ;j++){
          if ( H[i].relation[j]==CONFLICT){
              edges[k].x = i;
              edges[k].y = j;
              k++;
          }
        }

      //atencao: te() altera edges
      int t;
      if (guedesFunc) t = te(edges, k);
      else t = te2(edges, k);
      
#ifdef DEBUG
      //printf("funcao limitante ( (%d + %d) >= %d )\n", cont_conf, t, *globConf);
#endif
      if ( (cont_conf + t) >= *globConf ){
          //printf("return\n");
          return;
      }
    //-----------------------------
  }

  //Desce para a esquerda da arvore
  auxX[l] = IN;
  (*count)++;
  branchAndBound(l+1, n, H, X, auxX, viab, opt, count, globConf, edges, guedesFunc);

  //Desce para a direita da arvore
  auxX[l] = OUT;
  (*count)++;
  branchAndBound(l+1, n, H, X, auxX, viab, opt, count, globConf, edges, guedesFunc);

  return;
}


int main(int argc, char **argv){
  int n, k, m;                  //Valores de entrada
  relation_t *heroes;           //Estrutura de herois
  int *solX, *auxX;             //Vetor percorre solução e auxiliar
  int viab = 1, opt = 1;        //Flags de algoritmo
  int guedesFunc = 0;           //Flag de algoritmo dado pelo professor
  int nodeCount = 1;            //Contador de nós que percorreu
  int conflictCount = -1;       //Menor conflito encontrado
  //struct timespec start, end;   //Variaveis de temporizacao
  double time_spent = 0;        //Tempo final de execucao

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

  edge_t* conf_edges;
  conf_edges = calloc( (n*n)/2, sizeof(int));

  //Seta heroi 1 no primeiro grupo
  solX[0] = IN; auxX[0] = IN;

  //Registra tempo atual
  //clock_gettime(CLOCK_REALTIME, &start);
  
  clock_t t = clock();
  branchAndBound(1, n, heroes, solX, auxX, viab, opt, &nodeCount, &conflictCount, conf_edges, guedesFunc);
  t = clock()-t;
  time_spent = ((double)t)/CLOCKS_PER_SEC;

  //Calcula tempo
  //clock_gettime(CLOCK_REALTIME, &end);
  //time_spent = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / MILLION;

  printf("\nResultado:\n");
  //printf("Tempo de execução: %f milisegundos\n", time_spent);
  printf("Tempo de execução: %f segundos\n", time_spent);
  printf("Percorreu %d nós da arvore\n", nodeCount);
  printf("Solução encontrada com %d conflitos\n", conflictCount);
  for (int i=0; i<n ;i++){
    if (solX[i] == 1)
      printf(" %d ", i+1);
  }
  printf("\n");

  //free(heroes[24].relation);
  //Libera estruturas
  for (int i=0; i<n ;i++)
    free(heroes[i].relation);
  free(heroes);
  free(solX);
  free(conf_edges);
  free(auxX);
  return 0;
}

//Observações:
//Vetor de herois vai de 0 a n-1 representando os n herois
//Vetor de solX vai de 0 a n-1 também representando os n herois
//Considera-se que o primeiro herói sempre esta no grupo 1
