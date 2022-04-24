
#include "gestorErrores.h"
#include <stdio.h>

void error(short numError){
    printf("\n* * * ERROR * * *\n");
    switch (numError) {
        case 1:
            printf("\nEl fichero no existe\n");
            break;
        case 2:
            printf("\nTamaño máximo del lexema superado");
            break;
        default:
            printf("\nNo existe descripción de error asociada");
            break;
    }
}
// Respecto a la gestion de erroes
// 1: que exista el fichero
// 2: tamaño maximo de lexema
// debe estar desacoplada
// recibe error 1 y con un switch hace printf del error exactamente y ya