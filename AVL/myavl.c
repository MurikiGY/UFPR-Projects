#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "avl.h"
#include "aux.h"

int main(){
    no_t    *root = NULL;       // Ponteiro da raiz da arvore
    char    oper;               // tipo de operacao
    int     valor;              // Valor da operacao
    char    linha[10];

    while(scanf("%c %d", &oper, &valor) != EOF){

        if (oper == 'i'){
            root = insere(root, valor);

        } else 
        if (oper == 'r'){
            root = (retira(root, valor));

        } else
            fprintf(stderr, "Operação invalida\n");

        getchar();
    }

    imprime_arvore(root, 0);

    return 0;
}
