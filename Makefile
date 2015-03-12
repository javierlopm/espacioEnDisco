all: UsoDisco

UsoDisco: UsoDisco.o colaDirectorios.o trabajador.o

	gcc -g-Wall-Wextra UsoDisco.o colaDirectorios.o trabajador.o -o UsoDisco

UsoDisco.o:  UsoDisco.c colaDirectorios.h
	gcc -g -c UsoDisco.c

colaDirectorios.o:  colaDirectorios.h colaDirectorios.c
	gcc -g -c colaDirectorios.c

trabajador.o: trabajador.h trabajador.c 
	gcc -g -c trabajador.c
	
clean:
	rm *.o