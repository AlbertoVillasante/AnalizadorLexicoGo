#include <stdio.h>
#include <stdlib.h>
#include "sistemaEntrada.h"
#include "gestorErrores.h"
#define TAM 16 // Tamaño de la unidad de asignacion

// A LA TABLA DE SIMBOLOS SE LE PREGUNTA AL IDENTIFICAR UNA CADENA ALFANUMERICA (ESTO NO ES DE AQUI PERO LO DIJO EN CLASE Y ES IMPORTANTE)

FILE *fp;                       // Fichero
char *bufferA;           // Bloque mitad de memoria intermedia
char *bufferB;              // Bloque mitad de memoria intermedia
char *bufferActual;          // Buffer de lectura actual (A o B)
short inicio;               // Se usa como puntero a inicio del lexema que se está leyendo
short delantero;            // Se usa como puntero a la siguiente direccion a leer
int tamActual;
/* Funcion encargada de rellenar el buffer actual y situar al principio el puntero delantero */
void recargarBufferActual(){
    int bytesLeidos;
    delantero = 0;                                           // Delantero sera el primer caracter del siguiente buffer
    bytesLeidos = fread(bufferActual, 1, TAM, fp);      // Leer del fichero
    /* Si no se lee un bloque completo es que se alcanzó el fin de fichero por lo que se añade al final */
    if(bytesLeidos < TAM){
        bufferActual[bytesLeidos] = EOF;
    }
}

void inicializarSistemaEntrada(char *filename){
    /* Abrir el fichero .go, que debe existir previamente, para lectura */
    fp = fopen(filename, "r");
    /* Comprobación de error en lectura*/
    if(!fp) {
        error(1);
    }
    /*   Reservar memoria para los buffers   */
    bufferA = (char*) malloc(TAM+1);
    bufferB = (char*) malloc(TAM+1);
    /* Se empieza a almacenar en el buffer A */
    bufferActual = bufferA;
    /* Inicializar valores de punteros delantero e inicio */
    inicio    = 0;
    delantero = 0;
    /* Insertar centinelas al final del bloque */
    bufferA[TAM] = EOF;
    bufferB[TAM] = EOF;
    /* Situarse correctamente en el fichero, leer bloque */
    recargarBufferActual();
    tamActual=0;
}

/* Función encargada de eliminar la memoria asociada al sistema de entrada */
void limpiarSistemaEntrada(){
    free(bufferA);
    free(bufferB);
    fclose(fp);
}

/* Funcion que actualiza el buffer actual, si:
 * el buffer actual era A -> el buffer actual pasa a ser B
 * el buffer actual era B -> el buffer actual para a ser A */

void actualizarBufferActual(){
    if (bufferActual == bufferA)
        bufferActual = bufferB;
    else
        bufferActual = bufferA;
}



char siguienteCaracter(){
    char caracter; // Caracter que se lee y devuelve la función

    caracter = bufferActual[delantero]; // Leer caracter
    if(caracter == EOF){
    /* Si el caracter es el fin de fichero puede ser el de verdad o el centinela */
        if(!feof(fp)){   // Case: centinela
            /* Se sobrepasa el bloque por lo que se pasa al siguiente y se vuelve a intentar leer el caracter
             * Hay que cargar los siguientes caracteres del fichero en él */
            actualizarBufferActual();                       // Cambiar buffer actual
            recargarBufferActual();                         // Recargas el buffer
            caracter = siguienteCaracter();                 // Leer nuevamente el caracter
        }
    }else{
        delantero++; // Incrementar el puntero (siguiente posicion a leer)
    }
    tamActual++;
    return caracter;
}

/* Funcion encargada de devolver el lexema */
char* getLexema(){
    char *lexema;                             // Puntero al lexema leído y aceptado
    int tamLexema;
    if(delantero >= inicio)
        tamLexema = delantero-inicio;    // Tamaño que tendrá el lexema
    else
        tamLexema = delantero-inicio+TAM;

    /* Reservar memoria para el lexema,  */
    lexema = (char*) malloc(sizeof(tamLexema) + 1);
    /* CASE: Lexema está en el mismo buffer */
    if(inicio < delantero){
        for (int i = 0; i < tamLexema; ++i) {
            lexema[i] = bufferActual[inicio]; // Se copia el valor del buffer en el lexema
            inicio++;                         // Se incrementa el valor del puntero a inicio
        }
    }
    /* Lexema dividido en los 2 bloques */
    else{
        /* Volver al buffer anterior para comenzar a leer desde él */
        actualizarBufferActual();
        for (int i = 0; i < tamLexema; ++i) {
            if(inicio == TAM){
                inicio = 0;
                actualizarBufferActual();
            }
            lexema[i] = bufferActual[inicio]; // Se copia el valor del buffer en el lexema
            inicio++;                         // Se incrementa el valor del puntero a inicio

            /* Si se llega al centinela
             * se apunta al principio del
             * siguiente (o anterior bloque)
             * el puntero inicio se situa al comienzo           */
            if(inicio == TAM){
                inicio = 0;
                actualizarBufferActual();
            }
        }
    }
    lexema[tamLexema] = '\0'; // Añadir fin de cadena
    if(tamActual > TAM)
        error(2);
    tamActual=0;
    return lexema;
}

/* Funcion encargada de situar correctamente el puntero inicio
 * al comienzo de un nuevo lexema tras detectar uno válido */
void lexemaDetectado(){
    inicio = delantero; // El puntero inicio será la siguiente posición a leer
    tamActual=0;
}

/* Funcion encargada de devolver un caracter (retroceder) */
void devolverCaracter(){
    delantero--; // Simplemente se vuelve atras la posicion de delantero
}
