#include <stdio.h>
#include <stdlib.h>

#include "avl.h"
#include "aux.h"

/* Cria no retornando ponteiro */
no_t *cria_no(int chave){
    no_t *no;

    /* Alocacao dinamica */
    if (!(no = malloc(sizeof(no_t)))){
        fprintf(stderr, "Falha na criacao do nodo\n");
        return NULL;
    }

    /* configura nodo */
    no->chave = chave;
    no->balanco = 0;
    no->esq = NULL;
    no->dir = NULL;
    no->pai = NULL;

    return no;
}

/* Inclui no na folha
 * realiza balanceamento */
no_t *insere(no_t *no, int chave){

    /* Caso base */
    if (!no)
        return cria_no(chave);

    /* Testa inclusao na esq ou dir */
    if (chave < no->chave){
        no->esq = insere(no->esq, chave);
        no->esq->pai = no;
    } else if (chave >= no->chave){
        no->dir = insere(no->dir, chave);
        no->dir->pai = no;
    }

    no = ajusta_bal(no);

    return no;
}

/* codigos aqui */
no_t *retira(no_t *no, int chave){

    /* Não encontrou a chave */
    if (!no){
        printf("Chave não encontrada\n");
        return no;
    }

    /* Encontrou a chave */
    if (chave == no->chave){

        if (no->esq && no->dir){                /* testa dois filhos */
            no = transplanta(no, chave);

        } else if (no->esq && !no->dir){        /* testa filho esq */
            no->chave = no->esq->chave;
            free(no->esq);
            no->esq = NULL;
        } else if (!no->esq && no->dir){        /* testa filho dir */
            no->chave = no->dir->chave;
            free(no->dir);
            no->dir = NULL;
        } else {                                /* não possui filhos */
            free(no);
            return NULL;
        }
    } else if (chave < no->chave)               /* Desce para a subarvore esquerda */
        no->esq = retira(no->esq, chave);
    else /* Desce para a subarvore direita */
        no->dir = retira(no->dir, chave);          /* Desce para a subarvore direita */

    no = ajusta_bal(no);

    return no;
}

/* Imprime toda a arvore em ordem */
void imprime_arvore(no_t *no, int h){

    /* Testa se chegou na folha */
    if (no == NULL)
        return;

    imprime_arvore(no->esq, h+1);
    printf("%d,%d\n", no->chave, h);
    imprime_arvore(no->dir, h+1);
}
