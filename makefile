CC= gcc -Wall
HEADER_FILES_DIR = .
INCLUDES = -I $(HEADER_FILES_DIR)
MAIN= compilador
SRCS = main.c analizadorLexico.c analizadorSintactico.c hashmap.c sistemaEntrada.c tablaSimbolos.c gestorErrores.c
DEPS = $(HEADER_FILES_DIR)/analizadorLexico.h analizadorSintactico.h hashmap.h sistemaEntrada.h tablaSimbolos.h gestorErrores.h definiciones.h
OBJS = $(SRCS:.c=.o)
$(MAIN): $(OBJS)
	$(CC) -o $(MAIN) $(OBJS)
	rm -f *.o *~
%.o: %.c $(DEPS)
	$(CC) -c $< $(INCLUDES)
cleanall: clean
	rm -f $(MAIN)
clean:
	rm -f *.o *~
