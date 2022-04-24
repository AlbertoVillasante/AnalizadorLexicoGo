#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "analizadorSintactico.h"
#include "analizadorLexico.h"
#include "tablaSimbolos.h"

void inicializarAnalizadorSintactico(){
    struct elemento comp_lex;
    int resultado = 1;
    while((resultado=sig_comp_lex(&comp_lex))){
        if (resultado != 2){
            printf("\n<%s, %d>", comp_lex.lexema, comp_lex.idToken);
        }
        if(comp_lex.lexema != NULL){
            free(comp_lex.lexema);
            comp_lex.lexema = NULL;
        }

    }

    printf("\n* * * Final del análisis * * *\n");
}
