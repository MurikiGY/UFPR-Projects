// PingPongOS - PingPong Operating System
// Muriki Gusmão Yamanaka
// GRR 20203933
// Definição e operações em uma fila genérica.

#include "queue.h"

#include <stdio.h>

//------------------------------------------------------------------------------
// Conta o numero de elementos na fila
// Retorno: numero de elementos na fila

int queue_size (queue_t *queue){
  queue_t *aux = queue;
  int count = 0;

  if ( !aux )
    return 0;
  else do {
      count++;
      aux = aux->next;
  } while ( aux != queue );

  return count;
}

//------------------------------------------------------------------------------
// Percorre a fila e imprime na tela seu conteúdo. A impressão de cada
// elemento é feita por uma função externa, definida pelo programa que
// usa a biblioteca. Essa função deve ter o seguinte protótipo:
//
// void print_elem (void *ptr) ; // ptr aponta para o elemento a imprimir

void queue_print (char *name, queue_t *queue, void print_elem (void*) ){

  printf("%s [ ", name);

  //Se ha elementos na lista
  if ( queue ){
    queue_t *aux = queue;
    do {
      print_elem(aux);
      aux = aux->next;
      printf(" ");
    } while ( aux != queue );
  }

  printf("]\n");
}

//------------------------------------------------------------------------------
// Insere um elemento no final da fila.
// Condicoes a verificar, gerando msgs de erro:
// - a fila deve existir
// - o elemento deve existir
// - o elemento nao deve estar em outra fila
// Retorno: 0 se sucesso, <0 se ocorreu algum erro

int queue_append (queue_t **queue, queue_t *elem){
  //Testa de lista existe
  if ( !queue )
    return -1;

  //Testa se elem existe
  if ( !elem )
    return -2;

  //Testa elemento inconsistente
  if ( elem->prev || elem->next )
    return -3;

  //Testa lista vazia
  if ( !(*queue) ){
    (*queue) = elem;
    elem->prev = elem;
    elem->next = elem;
  } else {
    //Ajusta elem
    elem->prev = (*queue)->prev;
    elem->next = (*queue);
    //Ajusta lista
    (*queue)->prev->next = elem;
    (*queue)->prev = elem;
  }

  return 0;
}

//------------------------------------------------------------------------------
// Remove o elemento indicado da fila, sem o destruir.
// Condicoes a verificar, gerando msgs de erro:
// - a fila deve existir
// - a fila nao deve estar vazia
// - o elemento deve existir
// - o elemento deve pertencer a fila indicada
// Retorno: 0 se sucesso, <0 se ocorreu algum erro

int queue_remove (queue_t **queue, queue_t *elem){
  //Testa fila existe
  if ( !queue )
    return -1;

  //Testa fila vazia ou elem existe
  if ( !((*queue) && elem) )
    return -2;

  //Testa se elemento existe na fila
  queue_t *aux = (*queue);
  do {
    if ( aux == elem ){

      //Testa se apenas um elemento
      if ( (*queue)->next == (*queue) ){
        (*queue)->next = NULL;
        (*queue)->prev = NULL;
        (*queue) = NULL;
      } else {
        if ( (*queue) == elem )
          (*queue) = (*queue)->next;
        elem->prev->next = elem->next;
        elem->next->prev = elem->prev;
        elem->next = NULL;
        elem->prev = NULL;
      }
      return 0;

    }
    aux = aux->next;
  } while ( aux != (*queue) );

  //aux nunca foi igual a elem
  return -2;
}

