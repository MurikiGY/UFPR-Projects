#include <stdio.h>
#include <stdlib.h>

#include "avl.h"
#include "aux.h"

/* Retorna diferenca entre balanceamento dos filhos*/
int testa_bal(no_t *no){
    int bal_esq;
    int bal_dir;

    /* Guarda o balanco dos filhos */
    bal_esq = fb(no->esq);
    bal_dir = fb(no->dir);

    /*  Testa o balanceamento */
    return bal_esq - bal_dir;
}

/* Operacao de rotacao a esquerda */
no_t *rot_esquerda(no_t *no){
    no_t *q = no->dir;   /* Ponteiro do no que "sobe" */

    (*no).dir = q->esq;
    q->pai = no->pai;
    (*no).pai = q;

    if (q->esq != NULL)
        q->esq->pai = no;
    q->esq = no;

    /* Ajusta ponteiro do pai de no */
    if (q->pai != NULL){
        if (q->pai->esq == no)
            q->pai->esq = q;
        else
            if (q->pai->dir == no)
                q->pai->dir = q;
    }

    /* Ajusta balanco */
    no->balanco = 1 + max(fb(no->esq), fb(no->dir));
    q->balanco = 1 + max(fb(q->esq), fb(q->dir));

    /* Retorna o no que subiu para a raiz */
    return q;
}

/* Operacao de rotacao a direita */
no_t *rot_direita(no_t *no){
    no_t *q = no->esq;    /* Ponteiro do no que sobe */

    (*no).esq = q->dir;
    q->pai = no->pai;
    (*no).pai = q;

    if (q->dir != NULL)
        q->dir->pai = no;
    q->dir = no;

    /* Ajusta ponteiro do pai de no */
    if (q->pai != NULL){
        if (q->pai->dir == no)
            q->pai->dir = q;
        else
            if (q->pai->dir == no)
            q->pai->dir = q;
    }

    /* Ajusta balanco */
    no->balanco = 1 + max(fb(no->esq), fb(no->dir));
    q->balanco = 1 + max(fb(q->esq), fb(q->dir));

    /* Retorna o no que subiu para a raiz */
    return q;
}

/* transplanta por antecessor e efetua balanceamento */
no_t *transplanta(no_t* no, int chave){
    no_t *ant;

    /* suc recebe sucessor */
    ant = busca_max(no->esq);

    /* copia a chave */
    no->chave = ant->chave;

    /* retira o sucessor */
    no->esq = retira(no->esq, ant->chave);
    return no;
}

/* Ajusta balanceamento do no por meio de rotacoes */
no_t* ajusta_bal(no_t *no){
    int bal_filho;
    int bal_test;
    
    /* Ajusta balanceamento do no */
    no->balanco = 1 + max(fb(no->esq), fb(no->dir));

    /*  Testa o balanceamento */
    bal_test = testa_bal(no);

    /* no esquerdo mais pesado */
    if (bal_test > 1){

        bal_filho = testa_bal(no->esq);
        if (bal_filho > 0)
            return rot_direita(no); //zig zig
        else{
            no->esq = rot_esquerda(no->esq); //zig zag
            return rot_direita(no);
        }
    }

    /* no direito mais pesado */
    if (bal_test < -1){

        bal_filho = testa_bal(no->dir);
        if (bal_filho < 0)
            return rot_esquerda(no); //zig zig
        else{
            no->dir = rot_direita(no->dir); //zig zag
            return rot_esquerda(no);
        }
    }
    
    return no;
}


/* Retorna o no com maior valor da subarvore */
no_t *busca_max(no_t *no){

    /* Se nao ha mais nodos a esquerda,
     * retorna o nodo */
    while( no->dir != NULL)
        no =  no->dir;
    return no;
}

/* Retorna o maior valor entre dois numeros */
int max(int x, int y){
    if (x > y)
        return x;
    return y;
}

/* retorna o fator de balanceamento */
int fb(no_t *no){
    if (!no)
        return -1;
    return no->balanco;
}
