/* Implements hashmap from the following repository: https://github.com/tidwall/hashmap.c */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "tablaSimbolos.h"
#include "hashmap.h"
#include "definiciones.h"


/* Crear tabla de símbolos */
/* Se utiliza un hashmap   */
struct hashmap *tablaSimbolos;

/* Array con las keywords */
struct elemento keywords[25] = {{BREAK, "break"}, {CASE, "case"}, {CHAN, "chan"}, {CONST, "const"},
        {CONTINUE, "continue"}, {DEFAULT, "default"}, {DEFER, "defer"}, {ELSE, "else"}, {FALLTHROUGH, "fallthrough"},
        {FOR, "for"}, {FUNC, "func"}, {GO, "go"}, {GOTO, "goto"}, {IF, "if"}, {IMPORT, "import"},
        {INTERFACE, "interface"}, {MAP, "map"}, {PACKAGE, "package"}, {RANGE, "range"}, {RETURN, "return"},
        {SELECT, "select"}, {STRUCT, "struct"}, {SWITCH, "switch"}, {TYPE, "type"}, {VAR, "var"}};

int numKeywords = sizeof(keywords) / sizeof(keywords[0]);
/* Funcion hash */
uint64_t elemento_hash(const void *item, uint64_t seed0, uint64_t seed1) {
    const struct elemento *elemento = item;
    return hashmap_sip(elemento->lexema, strlen(elemento->lexema), seed0, seed1);
}

/* Funcion que compara si 2 elementos son iguales */
int elemento_compare(const void *a, const void *b, void *udata) {
    const struct elemento *elementoa = a;
    const struct elemento *elementob = b;
    return strcmp(elementoa->lexema, elementob->lexema);
}

bool elemento_iter(const void *item, void *udata) {
    const struct elemento *elemento = item;
    printf("idToken: %d -> %s\n", elemento->idToken, elemento->lexema);
    return true;
}

void imprimirTablaSimbolos(){
    printf("\n * * * TABLA DE SÍMBOLOS * * * \n");
    printf("\nComp léxico  ->  Lexema \n");
    hashmap_scan(tablaSimbolos, elemento_iter, NULL);
    printf("\n * * * TABLA DE SÍMBOLOS * * * \n");
}



void inicializarTabla(){

    /*  Inicializar tabla de símbolos                                   */
    /*  Hashmap_new devuelve el nuevo hashmap                           */

    /*  Tamaño de cada elemento del hashmap,
     *  capacidad inicial,
     *  seeds para la función hash (se ponen a 0 para aleatoriedad,
     *  función hash,
     *  función de comparación,
     *  no se elimina elemento específico,
     *  no se referencian tipos de datos en el hash                     */

    tablaSimbolos = hashmap_new(sizeof(struct elemento), 0, 0, 0, elemento_hash, elemento_compare, NULL, NULL);

    /* Bucle para insertar en la tabla las palabras clave */
    for (int i = 0; i < numKeywords; ++i) {
        hashmap_set(tablaSimbolos, &(struct elemento){.idToken = keywords[i].idToken, .lexema = keywords[i].lexema}); // Insertar cada elemento en el hashmap
    }



}

/* Inserta un lexema, que es un identificador, y devuelve el tokn id */
int insertarElementoID(char *lexema){
    char *nuevoLexema = NULL;
    int t;
    t = strlen(lexema);
    nuevoLexema = (char*) malloc(t+1);
    strcpy(nuevoLexema, lexema);
    hashmap_set(tablaSimbolos, &(struct elemento){.idToken = ID, .lexema = nuevoLexema});
    return ID;
}

// AHORA HACER GUNCION BUSCAR CON EL GET Y HACER EL FREE Y QUIZAS OTRA DE INSERTAR CON ID TB

/* Busca elemento en la tabla de símbolos por su lexema
 * si se encuentra devuelve su identificador (token )
 * si no se encuentra devuelve el identificador ID */
short buscarLexema(char *lexema){
    struct elemento *elemento;
    if((elemento = hashmap_get(tablaSimbolos, &(struct elemento) {.lexema = lexema})) != NULL){ // Comprobacion de error
        if (elemento->idToken == ID)
        {
            return 1; // Comprobacion para no insertar en la tabla 2 veces el mismo ID
        }
        
        return elemento->idToken;
    }
    else
        return ID;
}

bool borrarElementos(const void *item, void *udata) {
    const struct elemento *elemento = item;
    if(elemento->idToken == ID)
        free(elemento->lexema);
    return true;
}
void borrarTabla(){
    hashmap_scan(tablaSimbolos, borrarElementos, NULL);
    hashmap_clear(tablaSimbolos, true);   
    hashmap_free(tablaSimbolos);
}


