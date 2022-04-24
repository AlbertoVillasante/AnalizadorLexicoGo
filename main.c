#include "tablaSimbolos.h"
#include "sistemaEntrada.h"
#include "analizadorSintactico.h"

int main() {
    /* Nombre del fichero                                                   */
    char *filename = "concurrentSum.go";
    /* Crear tabla de símbolos e insertar en ella las palabras clave        */
    inicializarTabla();
    /* Imprimir por pantalla la tabla de símbolos antes de analizar         */
    imprimirTablaSimbolos();
    /* Inicializar sistema de entrada                                       */
    inicializarSistemaEntrada(filename);
    /* Inicializar analizador sintáctico                                    */
    inicializarAnalizadorSintactico();
    /* Imprimir por pantalla la tabla de símbolos al finalizar el análisis  */
    imprimirTablaSimbolos();
    /* Liberar memoria (estructuras dinámicas), punteros etc                */
    limpiarSistemaEntrada();
    borrarTabla();
    return 0;
}
