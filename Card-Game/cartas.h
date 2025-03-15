#ifndef _CARTAS_
#define _CARTAS_

#include "sources/socket.h"
#include "sources/baralho.h"

#define MAXPLAYERS 4
#define VIDAS 3

// Opcodes e estados do carteador
typedef enum carteador {
  PEDE_VIDAS = 0,
  ENVIA_CARTA,
  PEDE_PALPITE,
  ENVIA_PALPITE,
  JOGA_CARTA,
  RODADA_GANHA,
  RESULTADO,
  PASSA_BASTAO,
  FINALIZA
} CART_STATE;

typedef struct game{
  baralho_t cartas[MAXBARALHO]; // Cartas
  int num_cartas;               // Quantidade de cartas da rodada
  int palpites[MAXPLAYERS];     // Palpites de cada player
  int vidas[MAXPLAYERS];        // Vidas
  int num_jogadores;            // Quantidade de jogadores
  int direcao;                  // Sentido de aumentar ou diminuir cartas
  int maior_carta;              // Flag se jogou maior carta da rodada
  int rodadas;                  // Salva rodada atual
  int id;                       // Id com relação ao carteador 
} game_t;


#endif // !_CARTAS_

