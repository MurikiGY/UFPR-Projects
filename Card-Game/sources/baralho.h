#ifndef _BARALHO_
#define _BARALHO_

#define OUROS 'O'
#define ESPADAS 'E'
#define COPAS 'C'
#define PAUS 'P'

#define MAXBARALHO 52  // Tamanho do baralho
#define MAXCARDS 12    // Numero de cartas das rodadas

typedef struct baralho {
  unsigned char valor;
  unsigned char naipe;
  unsigned char jogado;
} baralho_t;

void reseta_baralho(baralho_t *cartas, int size);

void embaralhar(baralho_t *cartas, int size);

void imprime_baralho(baralho_t *cartas, int size);

void imprime_baralho_buffer(unsigned char *buffer, int size);

#endif // !_BARALHO_
