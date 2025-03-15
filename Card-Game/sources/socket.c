#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "socket.h"

int dgram_socket(char *hostname, struct sockaddr_in *servaddr, struct sockaddr_in *cliaddr, 
    int serv_port, int cli_port, socklen_t *len){ 
  struct hostent *he;
  int sockfd;

  // Creating socket file descriptor
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) { 
    perror("Socket creation failed"); exit(EXIT_FAILURE); 
  }

  // server information
  memset(servaddr, 0, sizeof(*servaddr));
  servaddr->sin_family      = AF_INET; // IPv4
  servaddr->sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr->sin_port        = htons(serv_port);

  // client information
  he = gethostbyname(hostname);
  if (he == NULL) { perror("gethostbyname"); exit(1); }
  memset(cliaddr, 0, sizeof(*cliaddr));
  cliaddr->sin_family = AF_INET;
  cliaddr->sin_port   = htons(cli_port);
  cliaddr->sin_addr   = *((struct in_addr *)he->h_addr);
  *len = sizeof(*cliaddr);

  // Bind the socket with the server address
  if (bind(sockfd, (const struct sockaddr *)servaddr, sizeof(*servaddr)) < 0) {
      perror("Bind failed"); exit(EXIT_FAILURE);
  }

  return sockfd;
}

// Configura Buffer
void buffer_setup(unsigned char *buf, package_t pack){
  int i, j;
  buf[0] = pack.origem & 0xFF;
  buf[1] = (pack.origem >> 8) & 0xFF;
  buf[2] = pack.destino & 0xFF;
  buf[3] = (pack.destino >> 8) & 0xFF;
  buf[4] = pack.opcode;
  buf[5] = pack.received;
  buf[6] = pack.msglen & 0xFF;
  buf[7] = (pack.msglen >> 8) & 0xFF;
  for (i=OVERHEAD,j=0; j<pack.msglen ;i++,j++)
    buf[i] = pack.message[j];
  buf[i] = '\0';
}

void buffer_retrieve(unsigned char *buf, package_t *pack){
  int i, j;
  pack->origem   = buf[0] | (buf[1]<<8);
  pack->destino  = buf[2] | (buf[3]<<8);
  pack->opcode   = buf[4];
  pack->received = buf[5];
  pack->msglen   = buf[6] | (buf[7]<<8);
  for (i=0, j=OVERHEAD; i<pack->msglen ;i++,j++)
    pack->message[i] = buf[j];
  pack->message[i] = '\0';
}


// Envia e aguarda resposta
void send_message(int sockfd, unsigned char *buf, struct sockaddr_in *servaddr, struct sockaddr_in *cliaddr, socklen_t *len, package_t pack){
  int numbytes;
  buffer_setup(buf, pack);
  sendto(sockfd, buf, OVERHEAD+pack.msglen, MSG_CONFIRM, (struct sockaddr *)cliaddr, sizeof(struct sockaddr_in));
  numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1, MSG_WAITALL, (struct sockaddr *)servaddr, len);
  buf[numbytes] = '\0';
  if ( pack.destino != ALLHOSTS )
    if (buf[5] == 0)
      printf("Mensagem destino não encontrado\n");
}


// Aguarda por mensagem
void receive_message(int sockfd, unsigned char *buf, struct sockaddr_in *servaddr, socklen_t *len, package_t *pack){
  int numbytes;
  printf("Aguardando dados...\n");
  numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1, MSG_WAITALL, (struct sockaddr *)servaddr, len);
  buf[numbytes] = '\0';
  buffer_retrieve(buf, pack);
}


void package_setup(package_t* pack, int destino, int opcode, int msglen){
  pack->destino = destino;
  pack->opcode = opcode;
  pack->msglen = msglen;
}

