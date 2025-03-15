
#ifndef _AVL_
#define _AVL_

/* Estrutura do no da arvore */
struct no {
    int         chave;      /* Chave do nodo                    */
    int         balanco;    /* fator de balanceamento           */
    struct no   *pai;       /* Ponteiro para o Pai              */
    struct no   *esq;       /* Ponteiro para o filho esquerdo   */
    struct no   *dir;       /* Ponteiro para o filho da direita */
};
typedef struct no no_t;

/* Cria no retornando ponteiro */
no_t *cria_no(int chave);

/* Inclui no na folha
 * realiza balanceamento */
no_t *insere(no_t *no, int chave);

/* codigos aqui */
no_t *retira(no_t *no, int chave);

/* Imprime toda a arvore em ordem */
void imprime_arvore(no_t *no, int h);

#endif
