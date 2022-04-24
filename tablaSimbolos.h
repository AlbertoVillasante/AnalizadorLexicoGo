
#ifndef P1_TABLASIMBOLOS_H
#define P1_TABLASIMBOLOS_H

/* Cada elemento de la tabla, formado por un identificador del token y su lexema correspondiente */
struct elemento{
    short idToken;      // VALUE
    char *lexema;       // KEY
};

/* Crea e inicializa la tabla de símbolos */
void inicializarTabla();
void imprimirTablaSimbolos();
short buscarLexema(char *lexema);
int insertarElementoID(char *lexema);
void borrarTabla();



#endif //P1_TABLASIMBOLOS_H
