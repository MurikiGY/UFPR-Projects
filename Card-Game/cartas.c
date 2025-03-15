#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "sources/socket.h"
#include "sources/baralho.h"
#include "cartas.h"

/* Regras: https://blog.copag.com.br/blog/detalhes/fodinha */

/*
 * ./cartas hostname server_port client_port 0 - Comeca enviando
 * ./cartas hostname server_port client_port 1 - Comeca ouvindo
*/

void imprime_dados(game_t jogo){
  printf("\e[1;1H\e[2J");
  // Imprime estado da rodada
  if (jogo.maior_carta == 1)      { printf("-- RODADA GANHA\n");   }
  else if (jogo.maior_carta == 0) { printf("-- RODADA PERDIDA\n"); }
  // Imprime baralho
  imprime_baralho(jogo.cartas, jogo.num_cartas);
  // Imprime vidas
  printf("VIDAS: %d\n", jogo.vidas[0]);
  // Imprime palpites
  if (jogo.palpites[0] != -1){
    printf("Palpites: ");
    for (int i=0; i<jogo.num_jogadores ;i++)
      if (i == jogo.id) { printf("\033[34m%d \033[0m", jogo.palpites[i]); }
      else              { printf("%d ", jogo.palpites[i]); }
    printf("\n");
  }
}

int inicia_rodadas(int sockfd, unsigned char *buf, struct sockaddr_in *servaddr, struct sockaddr_in *cliaddr, 
    socklen_t *len, game_t *jogo, package_t *send_pack, int *state){
  int total_cartas = jogo->num_cartas*jogo->num_jogadores;
  package_t recv_pack;
  reseta_baralho(jogo->cartas, MAXBARALHO);
  embaralhar(jogo->cartas, MAXBARALHO);

  int input;
  printf("Distribuir novas cartas?\n"
         "1 - SIM\n"
         "2 - NAO - Finaliza\n"
         "> ");
  scanf("%d", &input);
  if (input==2){
    package_setup(send_pack, ALLHOSTS, FINALIZA, 0);
    send_message(sockfd, buf, servaddr, cliaddr, len, *send_pack);
    *state = 0;
    return 1;
  }

  // Pede vidas
  send_pack->message[0] = jogo->vidas[0];
  package_setup(send_pack, ALLHOSTS, PEDE_VIDAS, 1);
  send_message(sockfd, buf, servaddr, cliaddr, len, *send_pack);
  buffer_retrieve(buf, &recv_pack);
  for (int i=0; i<recv_pack.msglen ;i++)  // Retorno
    jogo->vidas[i] = recv_pack.message[i];

  // Envia cartas
  for (int i=0, j=0; i<2*total_cartas ;i+=2, j++){
    send_pack->message[i] = jogo->cartas[j].valor;
    send_pack->message[i+1] = jogo->cartas[j].naipe;
  }
  package_setup(send_pack, ALLHOSTS, ENVIA_CARTA, 2*total_cartas);
  send_message(sockfd, buf, servaddr, cliaddr, len, *send_pack);

  imprime_dados(*jogo);

  // Pede palpites
  jogo->id = 0;
  printf("Entre com seu palpite: "); 
  scanf("%d", &jogo->palpites[0]);
  send_pack->message[0] = (unsigned char)jogo->palpites[0];
  package_setup(send_pack, ALLHOSTS, PEDE_PALPITE, 1);
  send_message(sockfd, buf, servaddr, cliaddr, len, *send_pack);
  buffer_retrieve(buf, &recv_pack);
  for (int i=0; i<recv_pack.msglen ;i++)  // Retorno
    jogo->palpites[i] = recv_pack.message[i];

  // Retransmite palpites
  for (int i=0; i<jogo->num_jogadores ;i++)
    send_pack->message[i] = (unsigned char)jogo->palpites[i];
  package_setup(send_pack, ALLHOSTS, ENVIA_PALPITE, jogo->num_jogadores);
  send_message(sockfd, buf, servaddr, cliaddr, len, *send_pack);
  return 0;
}

int finaliza_rodadas(int sockfd, unsigned char *buf, struct sockaddr_in *servaddr, struct sockaddr_in *cliaddr, 
    socklen_t *len, int clntport, int *bastao, package_t *send_pack, game_t *jogo, int *vitorias, int *state){
  //printf("Vidas: ");
  //for (int i=0; i<jogo->num_jogadores ;i++)
  //  printf("%d ", jogo->vidas[i]);
  //printf("\n");

  //printf("palpites: ");
  //for (int i=0; i<jogo->num_jogadores ;i++)
  //  printf("%d ", jogo->palpites[i]);
  //printf("\n");

  //printf("vitorias: ");
  //for (int i=0; i<jogo->num_jogadores ;i++)
  //  printf("%d ", vitorias[i]);
  //printf("\n");

  // Decresce as vidas
  for (int i=0; i<jogo->num_jogadores; i++)
    if (vitorias[i] != jogo->palpites[i])
      jogo->vidas[i]--;

   //if (vitorias[0] != jogo->palpites[0]) jogo->vidas[0]--;
  // Transmite resultados
  for (int i=jogo->num_jogadores-1, j=0; i>0 ;i--,j++)
    if (vitorias[i] == jogo->palpites[i]) { send_pack->message[j] = 1; } // Acertou palpite
    else                                  { send_pack->message[j] = 0; } // Errou
  package_setup(send_pack, ALLHOSTS, RESULTADO, jogo->num_jogadores-1);
  send_message(sockfd, buf, servaddr, cliaddr, len, *send_pack);
  
  // Verifica se o jogo terminou
  int count = 0;
  for (int i=0; i<jogo->num_jogadores ;i++)
    if (jogo->vidas[i] > 0) { count++; }
  if (count <= 1){
    package_setup(send_pack, ALLHOSTS, FINALIZA, 0);
    send_message(sockfd, buf, servaddr, cliaddr, len, *send_pack);
    *state = 0;
    imprime_dados(*jogo);
    return 1;
  }

  // Passa bastao
  *bastao = 0;
  send_pack->message[0] = '\0';
  package_setup(send_pack, clntport, PASSA_BASTAO, 0);
  send_message(sockfd, buf, servaddr, cliaddr, len, *send_pack);

  // Diminui/aumenta cartas
  if (jogo->direcao == -1)  { jogo->num_cartas--; }
  else                      { jogo->num_cartas++; }
  // Inverte direcao
  if ((jogo->num_cartas==1) || (jogo->num_cartas==MAXCARDS/MAXPLAYERS))
    jogo->direcao*=-1;
  return 0;
}

void jogador(int sockfd, unsigned char *buf, struct sockaddr_in *servaddr, struct sockaddr_in *cliaddr, socklen_t *len, 
    int servport, int *bastao, game_t *jogo, int *state, package_t *recv_pack){
  int carta = -1;

  if (recv_pack->destino==ALLHOSTS || recv_pack->destino==servport){
    switch ( recv_pack->opcode ) {
      case PEDE_VIDAS:
        buf[OVERHEAD+recv_pack->msglen] = jogo->vidas[0];
        buf[6] = (recv_pack->msglen+1) & 0xFF;
        buf[7] = ((recv_pack->msglen+1) >> 8) & 0xFF;
        break;

      case ENVIA_CARTA:
        for (int i=recv_pack->msglen-(2*jogo->num_cartas), j=0 ;i<recv_pack->msglen ;i+=2,j++){
          jogo->cartas[j].valor = recv_pack->message[i];
          jogo->cartas[j].naipe = recv_pack->message[i+1];
          jogo->cartas[j].jogado = 0;
        }
        buf[6] = (recv_pack->msglen-2*jogo->num_cartas) & 0xFF;
        buf[7] = ((recv_pack->msglen-2*jogo->num_cartas) >> 8) & 0xFF;
        // Reinicia palpites
        for (int i=0; i<jogo->num_jogadores ;i++)
          jogo->palpites[i] = -1;
        break;

      case PEDE_PALPITE:
        printf("Entre com seu palpite: ");  scanf("%d", &jogo->palpites[0]);
        buf[OVERHEAD+recv_pack->msglen] = jogo->palpites[0];
        buf[6] = (recv_pack->msglen+1) & 0xFF;
        buf[7] = ((recv_pack->msglen+1) >> 8) & 0xFF;
        jogo->id = recv_pack->msglen;
        break;

      case ENVIA_PALPITE:
        for (int i=OVERHEAD, j=0; i<OVERHEAD+recv_pack->msglen ;i++,j++)
          jogo->palpites[j] = buf[i];
        break;

      case JOGA_CARTA:
        imprime_baralho_buffer(recv_pack->message, recv_pack->msglen);
        printf("Escolha uma carta: ");  scanf("%d", &carta);
        jogo->cartas[carta].jogado = 1;
        buf[OVERHEAD+recv_pack->msglen] = jogo->cartas[carta].valor;
        buf[OVERHEAD+recv_pack->msglen+1] = jogo->cartas[carta].naipe;
        buf[6] = (recv_pack->msglen+2) & 0xFF;
        buf[7] = ((recv_pack->msglen+2) >> 8) & 0xFF;
        jogo->rodadas--;
        break;

      case RODADA_GANHA:
        jogo->maior_carta = recv_pack->message[recv_pack->msglen-1];
        buf[6] = (recv_pack->msglen-1) & 0xFF;
        buf[7] = ((recv_pack->msglen-1) >> 8) & 0xFF;
        break;

      case RESULTADO:
        // Decresce vidas
        if (recv_pack->message[recv_pack->msglen-1] == 0) { jogo->vidas[0]--; } // Le ultimo valor
        buf[6] = (recv_pack->msglen-1) & 0xFF;
        buf[7] = ((recv_pack->msglen-1) >> 8) & 0xFF;
        // Diminui/aumenta cartas
        if (jogo->direcao == -1)  { jogo->num_cartas--; }
        else                      { jogo->num_cartas++; }
        // Inverte direcao
        if ((jogo->num_cartas==1) || (jogo->num_cartas==MAXCARDS/MAXPLAYERS))
          jogo->direcao*=-1;
        break;

      case PASSA_BASTAO:
        buf[5] = 1;
        *bastao = 1;
        break;

      case FINALIZA:
        buf[5] = 1;
        *state = 0;
        break;

      default:
        printf("Opcode da mensagem não entendida\n");
        break;
    }
  }
  // Retransmite
  //printf("Retransmitindo(%d) %s\n", buf[6] | (buf[7]<<8), buf+OVERHEAD);
  sendto(sockfd, buf, OVERHEAD+(buf[6] | (buf[7]<<8)), MSG_CONFIRM, (struct sockaddr *)cliaddr, sizeof(struct sockaddr_in));
}

int main(int argc, char **argv) {
  struct sockaddr_in servaddr, cliaddr;
  socklen_t len;
  int sockfd, servport, clntport;
  int bastao = 0;
  game_t jogo = {
    .num_cartas = MAXCARDS/MAXPLAYERS,  // Numero de cartas das rodadas
    .palpites = {-1},                   // Palpites de todos os jogadores
    .vidas = {VIDAS},                     // Vidas dos jogadores
    .num_jogadores = MAXPLAYERS,        // Quantidae de jogadores 
    .direcao = -1,                      // Cartas aumentando ou diminuindo
    .maior_carta = -1,                  // Flag de rodada ganha ou perdida
    .rodadas = MAXCARDS/MAXPLAYERS,
    .id = 0
  };
  unsigned char buf[MAXBUFLEN];

  srand( time(NULL) );
  servport = atoi(argv[2]);
  clntport = atoi(argv[3]);
  sockfd = dgram_socket(argv[1], &servaddr, &cliaddr, servport, clntport, &len);

  if (atoi(argv[4])==1) { bastao = 1; } // Primeiro jogador

  // ----------------------------------

  printf("\e[1;1H\e[2J");

  int jogo_ativo = 1;
  while (jogo_ativo){
    if (jogo_ativo == 1) { jogo_ativo++;        }
    else                 { imprime_dados(jogo); }

    if (bastao){
      package_t send_pack; send_pack.origem = servport; send_pack.received = 0;
      int carta = -1;                       // Indice carta jogada/maior carta
      int vitorias[jogo.num_jogadores];     // Salva ganhadores de cada rodada
      jogo.rodadas = jogo.num_cartas;

      if (inicia_rodadas(sockfd, buf, &servaddr, &cliaddr, &len, &jogo, &send_pack, &jogo_ativo))
        continue;
      memset(vitorias, 0, jogo.num_jogadores*sizeof(int));
      while (jogo.rodadas > 0) {
        package_t recv_pack;
        imprime_dados(jogo);

        // Joga cartas da rodada
        package_setup(&send_pack, ALLHOSTS, JOGA_CARTA, 0);
        send_message(sockfd, buf, &servaddr, &cliaddr, &len, send_pack);
        buffer_retrieve(buf, &recv_pack);
        imprime_baralho_buffer(recv_pack.message, recv_pack.msglen);

        // Joga carta
        printf("Escolha uma carta: "); scanf("%d", &carta);
        jogo.cartas[carta].jogado = 1;

        // Alinha cartas com vitorias e palpites 
        for (int i=(jogo.num_jogadores-1)*2; i>1 ;i-=2){    // cartas:   [1,1], [2,2], [3,3], [0,0] -> [0,0], [1,1], [2,2], [3,3]                           
          recv_pack.message[i] = recv_pack.message[i-2];    // palpites: 0, 1, 2, 3                                              
          recv_pack.message[i+1] = recv_pack.message[i-1];  // vitorias: 0, 1, 2, 3                                              
        }                                                   
        recv_pack.message[0] = jogo.cartas[carta].valor;
        recv_pack.message[1] = jogo.cartas[carta].naipe;
        recv_pack.msglen+=2;

        // Busca carta mais alta
        carta = 0; 
        for (int i=2; i<recv_pack.msglen ;i+=2)
          if (recv_pack.message[carta] < recv_pack.message[i]) { carta = i; }
        carta/=2;
        vitorias[carta]++;

        // Transmite ganhadores da rodada
        if (carta == 0) { jogo.maior_carta = 1; }
        else            { jogo.maior_carta = 0; }
        for (int i=jogo.num_jogadores-1, j=0; i>0 ;i--,j++)
          if ( i == carta ) { send_pack.message[j] = 1; }
          else              { send_pack.message[j] = 0; }
        package_setup(&send_pack, ALLHOSTS, RODADA_GANHA, jogo.num_jogadores-1);
        send_message(sockfd, buf, &servaddr, &cliaddr, &len, send_pack);
        jogo.rodadas--;
      }

      finaliza_rodadas(sockfd, buf, &servaddr, &cliaddr, &len, clntport, &bastao, &send_pack, &jogo, vitorias, &jogo_ativo);
    } else {
      package_t recv_pack;
      receive_message(sockfd, buf, &servaddr, &len, &recv_pack);

      if (jogo.vidas[0] <= 0){
        if (buf[4] == PASSA_BASTAO){
          buf[2] = clntport & 0xFF;
          buf[3] = (clntport >> 8) & 0xFF;
        }
        if (buf[4] == FINALIZA){
          jogo_ativo = 0;
        }
        sendto(sockfd, buf, OVERHEAD+(buf[6] | (buf[7]<<8)), MSG_CONFIRM, (struct sockaddr *)&cliaddr, sizeof(struct sockaddr_in));
      }
      else {
        jogador(sockfd, buf, &servaddr, &cliaddr, &len, servport, &bastao, &jogo, &jogo_ativo, &recv_pack);
      }
    }
  }

  // Finaliza programa
  printf("Finishing Graciously\n");
  close(sockfd);
  return 0;
}

