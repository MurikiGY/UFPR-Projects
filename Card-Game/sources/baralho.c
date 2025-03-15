#include <stdio.h>
#include <stdlib.h>

#include "baralho.h"

void reseta_baralho(baralho_t *cartas, int size){
  int i=0;
  for (char j='1'; i<size ;i+=4, j++){
    if (j == ';'){
      // Valete
      cartas[i].valor  = cartas[i+1].valor  = cartas[i+2].valor  = cartas[i+3].valor  = 'J';
      cartas[i].jogado = cartas[i+1].jogado = cartas[i+2].jogado = cartas[i+3].jogado = 0;
      cartas[i].naipe = OUROS; cartas[i+1].naipe = ESPADAS; cartas[i+2].naipe = COPAS; cartas[i+3].naipe = PAUS;
    } else if (j == '<') {
      // Dama
      cartas[i].valor  = cartas[i+1].valor  = cartas[i+2].valor  = cartas[i+3].valor = 'Q';
      cartas[i].jogado = cartas[i+1].jogado = cartas[i+2].jogado = cartas[i+3].jogado = 0;
      cartas[i].naipe = OUROS; cartas[i+1].naipe = ESPADAS; cartas[i+2].naipe = COPAS; cartas[i+3].naipe = PAUS;
    } else if (j == '=') {
      // Rei
      cartas[i].valor  = cartas[i+1].valor  = cartas[i+2].valor  = cartas[i+3].valor = 'K';
      cartas[i].jogado = cartas[i+1].jogado = cartas[i+2].jogado = cartas[i+3].jogado = 0;
      cartas[i].naipe = OUROS; cartas[i+1].naipe = ESPADAS; cartas[i+2].naipe = COPAS; cartas[i+3].naipe = PAUS;
    } else {
      cartas[i].valor  = cartas[i+1].valor  = cartas[i+2].valor  = cartas[i+3].valor  = j;
      cartas[i].jogado = cartas[i+1].jogado = cartas[i+2].jogado = cartas[i+3].jogado = 0;
      cartas[i].naipe = OUROS; cartas[i+1].naipe = ESPADAS; cartas[i+2].naipe = COPAS; cartas[i+3].naipe = PAUS;
    }
  }
}

void embaralhar(baralho_t *cartas, int size) {
  baralho_t temp;
	for (int i=0; i<size; i++) {
		int r = rand() % size;
		temp.valor = cartas[i].valor;
		temp.naipe = cartas[i].naipe;
		cartas[i].valor = cartas[r].valor;
		cartas[i].naipe = cartas[r].naipe;
		cartas[r].valor = temp.valor;
		cartas[r].naipe = temp.naipe;
	}
} 

void imprime_baralho(baralho_t *cartas, int size){
  printf("CARTAS:\n");

  for (int i=0; i<size ;i++)
    if (cartas[i].jogado ){
      if (i < 10) { printf("\033[31m" " %d     " "\033[0m", i); }
      else        { printf("\033[31m" " %d    "  "\033[0m", i);  }
    } else {
      if (i < 10) { printf(" %d     ", i); }
      else        { printf(" %d    ", i);  }
    }
  printf("\n");

  for (int i=0; i<size ;i++)
    if (cartas[i].jogado) { printf("\033[31m" "┌────┐ " "\033[0m"); }
    else                  { printf("┌────┐ "); }
  printf("\n");

  for (int i=0; i<size ;i++)
    if (cartas[i].jogado) {
      if (cartas[i].valor == '1')       { printf("\033[31m" "│A   │ "  "\033[0m"); }
      else if (cartas[i].valor == ':')  { printf("\033[31m" "│10  │ "  "\033[0m"); }
      else                              { printf("\033[31m" "│%c   │ " "\033[0m", cartas[i].valor); }
    } else {
      if (cartas[i].valor == '1')       { printf("│A   │ "); }
      else if (cartas[i].valor == ':')  { printf("│10  │ "); }
      else                              { printf("│%c   │ ", cartas[i].valor); }
    }
  printf("\n");

  for (int i=0; i<size ;i++)
    if (cartas[i].jogado) { printf("\033[31m" "│    │ " "\033[0m"); }
    else                  { printf("│    │ "); }
  printf("\n");

  for (int i=0; i<size ;i++)
    if (cartas[i].jogado){
      if (cartas[i].naipe == OUROS)        { printf("\033[31m" "│   ♦│ " "\033[0m"); }
      else if (cartas[i].naipe == ESPADAS) { printf("\033[31m" "│   ♠│ " "\033[0m"); }
      else if (cartas[i].naipe == COPAS)   { printf("\033[31m" "│   ♥│ " "\033[0m"); }
      else if (cartas[i].naipe == PAUS)    { printf("\033[31m" "│   ♣│ " "\033[0m"); }
    } else {
      if (cartas[i].naipe == OUROS)        { printf("│   ♦│ "); }
      else if (cartas[i].naipe == ESPADAS) { printf("│   ♠│ "); }
      else if (cartas[i].naipe == COPAS)   { printf("│   ♥│ "); }
      else if (cartas[i].naipe == PAUS)    { printf("│   ♣│ "); }
    }
  printf("\n");

  for (int i=0; i<size ;i++)
    if (cartas[i].jogado) { printf("\033[31m" "└────┘ " "\033[0m"); }
    else                  { printf("└────┘ "); }
  printf("\n");
}


void imprime_baralho_buffer(unsigned char *buffer, int size){
  if (size > 0){
    printf("Cartas jogadas: \n");
    for (int i=0; i<size/2 ;i++)   { printf("┌────┐ "); }
    printf("\n");
    for (int i=0; i<size/2 ;i++)
      if (buffer[i*2] == '1')       { printf("│A   │ "); }
      else if (buffer[i*2] == ':')  { printf("│10  │ "); }
      else                                      { printf("│%c   │ ", buffer[i*2]); }
    printf("\n");
    for (int i=0; i<size/2 ;i++)   { printf("│    │ "); }
    printf("\n");
    for (int i=0; i<size/2 ;i++)
      if (buffer[i*2+1] == OUROS)        { printf("│   ♦│ "); }
      else if (buffer[i*2+1] == ESPADAS) { printf("│   ♠│ "); }
      else if (buffer[i*2+1] == COPAS)   { printf("│   ♥│ "); }
      else if (buffer[i*2+1] == PAUS)    { printf("│   ♣│ "); }
    printf("\n");
    for (int i=0; i<size/2 ;i++)        { printf("└────┘ "); }
    printf("\n");
  }
}
