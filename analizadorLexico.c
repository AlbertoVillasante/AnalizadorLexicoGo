#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include "tablaSimbolos.h"
#include "analizadorLexico.h"
#include "sistemaEntrada.h"
#include "definiciones.h"
/*   DECLARACION DE AUTÓMATAS   */

int automataComentarios();
int automataComentariosLinea();
int automataComentariosBloque();

int automataCadenasAlfanumericas(struct elemento *resultado);

int automataStrings(struct elemento *resultado);

int automataNumeros(struct elemento *resultado, char caracter);
int automataNumerosHexadecimales(struct  elemento *resultado);
int automataNumerosFlotantes(struct elemento *resultado, int seLeyoE);
/* FIN DECLARACIÓN DE AUTÓMATAS */

/* Funcion que agrupa las letras de go
 * devuelve uno si el caracter introducido es
 * un guion bajo o
 * una letra de unicode                         */
int isletter(char caracter){
    if (caracter == '_' || isalpha(caracter))
        return 1;
    else return 0;
}

int ishex(char caracter){

    if(isdigit(caracter) || caracter == 'A' || caracter == 'a' || caracter == 'B' || caracter == 'b' || caracter == 'C' || caracter == 'c' ||
    caracter == 'D' || caracter == 'd' || caracter == 'E' || caracter == 'e' || caracter == 'F' || caracter == 'f' )
        return 1;
    else
        return 0;
}

/* Funcion encargada de enviar al analizador sintáctico el siguiente componente léxico
 * devuelve un 1 si debe seguir enviando y un 0 si es el fin de fichero lo que se ha leído
 * almacena en el struct elemento resultado el lexema y el componente lexico que se envía */

int sig_comp_lex(struct elemento *resultado){
    int seguir = 1; // variable encargada de decidir si se vuelve a invocar la funcion, se setea a 0 al detectar un EOF
    char caracterLeido;



    caracterLeido = siguienteCaracter();
    /* Si se lee un end of file no se realizan elresto de comprobaciones y se devuelve el flujo al analizador sintactico */
    if (caracterLeido == EOF){
        seguir = 0;
        return seguir;
    }
        /* Si no es un EOF hay que decidir el automata en el que se va a seguir leyendo */
        /* Si es un espacio, salto de linea etc, se ignora y se sigue leyendo */
    else if (isspace(caracterLeido)){
        seguir = 2;
        lexemaDetectado();
        //resultado->lexema = getLexema();
        //resultado->idToken = (short ) caracterLeido;
    }
        /* Si se detecta una barra es un autómata de comentarios */
    else if (caracterLeido == '/') {
        seguir = automataComentarios(resultado);
    }
        /* Si se detecta una letra o un _ es una cadena alfanumérica */
    else if (isletter(caracterLeido)){
        seguir = automataCadenasAlfanumericas(resultado);
    }
        /* 2 puntos y 2 puntos = (: :=) */
    else if (caracterLeido == ':'){
        if (siguienteCaracter() == '='){
            seguir = 1;
            resultado->lexema = getLexema();
            resultado->idToken = PUNTOSIGUAL;
        }
        else{
            devolverCaracter(); // Si es solo 2 puntos se devuelve el caracter y se proporciona como id su ascii
            seguir = 1;
            resultado->lexema = getLexema();
            resultado->idToken = (short ) caracterLeido;
        }
    }
        /* Autómata que reconoce más y masigual */
    else if (caracterLeido == '+'){
        if (siguienteCaracter() == '='){
            seguir = 1;
            resultado->lexema = getLexema();
            resultado->idToken = MASIGUAL;
        }
        else{
            devolverCaracter(); // Si es solo + se devuelve el caracter y se proporciona como id su ascii
            seguir = 1;
            resultado->lexema = getLexema();
            resultado->idToken = (short ) caracterLeido;
        }
    }
        /* Autómata que reconoce operador flecha */
    else if (caracterLeido == '<'){
        if (siguienteCaracter() == '-'){
            seguir = 1;
            resultado->lexema = getLexema();
            resultado->idToken = FLECHA;
        }
        else{
            devolverCaracter(); // Si es solo < se devuelve el caracter y se proporciona como id su ascii
            seguir = 1;
            resultado->lexema = getLexema();
            resultado->idToken = (short ) caracterLeido;
        }
    }
        /* Autómata encargado de leer strings de comilla doble*/
    else if(caracterLeido == '"'){
        seguir = automataStrings(resultado);
    }
        /* Autómata encargado de leer números */
    else if(isdigit(caracterLeido)){
        automataNumeros(resultado, caracterLeido);
    }
        /* Caso que solo lee numeros flotantes
         * si se lee un punto se comprueba que despues hay un numero,
         * en cuyo caso es un flotante, si no lo es,
         * se devuelve el caracter leido y se retorna el punto          */
    else if(caracterLeido == '.'){
        caracterLeido = siguienteCaracter();
        if(isdigit(caracterLeido)){
            devolverCaracter(); // Se devuelve el caracter para que el automata comience siempre tras leer un punto
            automataNumerosFlotantes(resultado, 0);
        } else {
            devolverCaracter();
            seguir = 1;
            resultado->lexema = getLexema();
            resultado->idToken = (short) caracterLeido;
        }
    }




        /* Operadores y puntuación que solo pueden ser eso */
    else {
        seguir = 1;
        resultado->lexema = getLexema();
        resultado->idToken = (short ) caracterLeido;
    }

    return seguir;
}

/* Autómata que reconoce comentarios, implementa los 2 tipos de comentarios que hay
 * 1: // hasta final de linea
 * 2: / y * hasta */
int automataComentarios(struct elemento *resultado){
    int res = 1;
    switch (siguienteCaracter()) {
        case '/':
            res = automataComentariosLinea();
            break;
        case '*':
            res = automataComentariosBloque();
            break;
        default:
            devolverCaracter();
            resultado->lexema = getLexema();
            resultado->idToken = '/';
            res = 1;

    }
    return res;
}
/* Autómata que lee caracteres en un comentario hasta que se encuentra
 * con un EOF (fin del análisis) o
 * con un salto de línea (fin del comentario) y hay que seguir analizando */
int automataComentariosLinea(){
    char caracter;
    while ((caracter = siguienteCaracter())){
        if (caracter == EOF)
            return 0;
        if (caracter == '\n'){
            lexemaDetectado();
            return 2;
        }
    }
    return 1;
}
/* Autómata que lee caracteres en un comentario hasta que se encuentra
 * con un EOF (fin del análisis) o
 * con un * y una / (fin del comentario) y hay que seguir analizando,
 * mientras detecte astertiscos comprueba si despues hay una /       */
int automataComentariosBloque(){
    char caracter;
    while ((caracter = siguienteCaracter())){
        if (caracter == EOF)
            return 0;
        while(caracter == '*'){
            if((caracter = siguienteCaracter()) == '/'){
                lexemaDetectado();
                return 2;
            }
            if (caracter == EOF)
                return 0;
        }
    }
    return 1;
}

/* Autómata encargado de leer cadenas alfanuméricas, lee continuamente caracteres
 * hasta que encuentra algo distinto a una letra, un número o un guión bajo
 * en ese caso se devuelve el ultimo caracter leído y se acepta la cadena,
 * se comprueba si es un identificador o un keyword buscando en la tabla de simbolos */
int automataCadenasAlfanumericas(struct elemento *resultado){
    char caracter;
    while ((caracter = siguienteCaracter())){
        if (!isletter(caracter) && !isdigit(caracter)){ // Se encuentra un caracter distinto a una letra de go o un número
            devolverCaracter(); // Devolver caracter ya que se aceptó gracias a uno que no pertenece a la cadena
            resultado->lexema = getLexema();
            resultado->idToken = buscarLexema(resultado->lexema);
            if(resultado->idToken == ID) // Si es un identificador insertar en la tabla
                insertarElementoID(resultado->lexema);
            if(caracter == EOF)
                return 0; // Si se acabo de leer la cadena al final del archivo se para la lectura
            break; // Ya encontró la cadena y se rompe el bucle
        }
    }
    return 1;
}
/* Automata encargado de leer strings, devuelve tod hasta que encuentra "
 * si hay una backslash ignora esas comillas, si hay más de una no se ignoran */
int automataStrings(struct elemento *resultado){
    char caracter;
    int barras = 0; // Encargada de contar cuantas backslashes se leen
    while ((caracter = siguienteCaracter())){
        if(caracter == EOF)
            return 0; // Si se acabo de leer la cadena al final del archivo se para la lectura
        else if (caracter == '\\') { // Si se lee un backslash
            caracter = siguienteCaracter();
            while (caracter == '\\'){ // Si hay más de un backslash ya no anula las comillas
                caracter = siguienteCaracter();
                barras++;
            }
            if (caracter == '"' && !barras){ // Si hay 1 backslash se anulan las comillas
                caracter = siguienteCaracter();
            }
        }
        if (caracter == '"'){ // Se encuentra el cierre de comillas, no se devuelve caracter
            resultado->lexema = getLexema();
            resultado->idToken = STRING;
            break; // Ya encontró la cadena y se rompe el bucle
        }
    }
    return 1;
}

/* Autómata encargado de leer números, repartiendo en subautómatas la labor */
int automataNumeros(struct elemento *resultado, char caracter){
    if(caracter == '0'){
        if((caracter =  siguienteCaracter()) == 'x' || caracter == 'X')
            return automataNumerosHexadecimales(resultado);
        else if(caracter == '.'){
            return automataNumerosFlotantes(resultado, 0);
        }
        else if(caracter == 'e' || caracter == 'E'){
            automataNumerosFlotantes(resultado, 1); // Se comprueba que se leyó una e, para ver que no hay más
        }  else
            devolverCaracter(); // Si no fue ninguno de esos casos se vuelve atrás
    }
    while ((caracter = siguienteCaracter())){
        if(caracter == '.'){
            return automataNumerosFlotantes(resultado, 0);
        }
        else if(caracter == 'e' || caracter == 'E'){
            return automataNumerosFlotantes(resultado, 1); // Se comprueba que se leyó una e, para ver que no hay más
        }
        else if (!isdigit(caracter)){
                if(caracter == EOF){
                    resultado->lexema = getLexema();
                    resultado->idToken = INTEGER;
                    return 0;
                }
                else if (caracter == '_'){
                    if(siguienteCaracter() == '_'){
                        // Si se leen 2 guiones bajos seguidos se retroceden los 2 y se devuelve el numero anterior
                        devolverCaracter();
                        devolverCaracter();
                        resultado->lexema  = getLexema();
                        resultado->idToken = INTEGER;
                        break;
                    } else
                        devolverCaracter(); // Si se leyó otra cosa se vuelve a atras para realizar el resto de comprobaciones

                } else if(caracter == 'i'){ // Si se lee una i es un imaginario
                    resultado->lexema = getLexema();
                    resultado->idToken = IMAGINARY;
                    break;
                } else{
                    devolverCaracter();
                    resultado->lexema  = getLexema();
                    resultado->idToken = INTEGER;
                    break;
                }
            }
        }

    return 1;
}
/* Practicamente igual que el automata de numeros pero con la comprobacion ishex */
int automataNumerosHexadecimales(struct elemento *resultado){
    char caracter;
    while ((caracter = siguienteCaracter())){
        if (!ishex(caracter)){
            if(caracter == EOF){
                resultado->lexema = getLexema();
                resultado->idToken = HEX;
                return 0;
            }
            else if (caracter == '_'){
                if(siguienteCaracter() == '_'){
                    // Si se leen 2 guiones bajos seguidos se retroceden los 2 y se devuelve el numero anterior
                    devolverCaracter();
                    devolverCaracter();
                    resultado->lexema  = getLexema();
                    resultado->idToken = HEX;
                    break;
                } else
                    devolverCaracter(); // Si se leyó otra cosa se vuelve a atras para realizar el resto de comprobaciones

            } else if(caracter == 'i'){ // Si se lee una i es un imaginario
                resultado->lexema = getLexema();
                resultado->idToken = IMAGINARY;
                break;
            } else{
                devolverCaracter();
                resultado->lexema  = getLexema();
                resultado->idToken = HEX;
                break;
            }
        }
    }

    return 1;
}

int automataNumerosFlotantes(struct elemento *resultado, int seLeyoE) {
    char caracter;
    if(seLeyoE) { // Si se leyó una e se vuelve atrás para no tener que escribir más casos y simplificar el programa
        devolverCaracter();
        seLeyoE = 0;
    }
    while ((caracter = siguienteCaracter())) {
        if (!isdigit(caracter)) {
            if (caracter == EOF) {
                resultado->lexema = getLexema();
                resultado->idToken = FLOAT;
                return 0;
            } else if (caracter == '_') {
                if (siguienteCaracter() == '_') {
                    // Si se leen 2 guiones bajos seguidos se retroceden los 2 y se devuelve el numero anterior
                    devolverCaracter();
                    devolverCaracter();
                    resultado->lexema = getLexema();
                    resultado->idToken = FLOAT;
                    break;
                } else
                    devolverCaracter(); // Si se leyó otra cosa se vuelve a atras para realizar el resto de comprobaciones

            } else if ((caracter == 'e' || caracter == 'E') && !seLeyoE) { // Si se lee la primera e
                while ((caracter = siguienteCaracter())){
                    if(!isdigit(caracter) && caracter != '+' && caracter != '-'){
                        if(caracter == 'i'){
                            resultado->lexema = getLexema();
                            resultado->idToken = IMAGINARY;
                            break; // Se rompe el bucle
                        }else{
                            devolverCaracter();
                            resultado->lexema = getLexema();
                            resultado->idToken = FLOAT;
                            break; // Se rompe el bucle
                        }

                    }
                }
                break; // Si se sale del ultimo bucle se sale del grande tambien
            } else if(caracter == 'i'){
                resultado->lexema = getLexema();
                resultado->idToken = IMAGINARY;
                break;
            }
        }
    }
    return 1;
}

