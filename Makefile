all: UsoDisco

UsoDisco: UsoDisco.o colaDirectorios.o trabajadores.o

	gcc -g-Wall-Wextra UsoDisco.o colaDirectorios.o trabajadores.o -o UsoDisco

UsoDisco.o:  UsoDisco.c colaDirectorios.h
	gcc -g -c UsoDisco.c

colaDirectorios.o:  colaDirectorios.h colaDirectorios.c
	gcc -g -c colaDirectorios.c

trabajadores.o: trabajadores.h trabajadores.c 
	gcc -g -c trabajadores.c
	
clean:
	rm *.o