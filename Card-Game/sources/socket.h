#ifndef __SOCKET__
#define __SOCKET__

// input buffer size
#include <netdb.h>
#include <sys/socket.h>

#define MAXBUFLEN 1500
#define OVERHEAD 8
#define ALLHOSTS 0

typedef struct package {
  int origem;
  int destino;
  int opcode;
  int received;
  int msglen;
  unsigned char message[MAXBUFLEN-OVERHEAD-4];
} package_t;


int dgram_socket(char *hostname, struct sockaddr_in *servaddr, struct sockaddr_in *cliaddr, 
    int serv_port, int cli_port, socklen_t *len);

// Configura Buffer
void buffer_setup(unsigned char *buf, package_t pack);

void buffer_retrieve(unsigned char *buf, package_t *pack);

// Envia e aguarda resposta
void send_message(int sockfd, unsigned char *buf, struct sockaddr_in *servaddr, struct sockaddr_in *cliaddr, socklen_t *len, package_t pack);

// Aguarda por mensagem
void receive_message(int sockfd, unsigned char *buf, struct sockaddr_in *servaddr, socklen_t *len, package_t *pack);

void package_setup(package_t* pack, int destino, int opcode, int msglen);

#endif // !__SOCKET__
