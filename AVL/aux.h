#ifndef _aux_
#define _aux_

/* Retorna diferenca entre balanceamento dos filhos*/
int testa_bal(no_t *no);

/* Operacao de rotacao a esquerda */
no_t *rot_esquerda(no_t *no);

/* Operacao de rotacao a direita */
no_t *rot_direita(no_t *no);

/* transplanta por antecessor e efetua balanceamento */
no_t *transplanta(no_t* no, int chave);

/* Ajusta balanceamento do no por meio de rotacoes */
no_t* ajusta_bal(no_t *no);

/* Retorna o no com maior valor da subarvore */
no_t *busca_max(no_t *no);

/* Retorna o maior valor entre dois numeros */
int max(int x, int y);

/* retorna o fator de balanceamento */
int fb(no_t *no);

#endif
