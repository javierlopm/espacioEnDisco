all: UsoDisco

UsoDisco: UsoDisco.o colaDirectorios.o
	gcc -g-Wall-Wextra UsoDisco.o colaDirectorios.o -o UsoDisco



UsoDisco.o:  UsoDisco.c colaDirectorios.h
	gcc -g -c UsoDisco.c

colaDirectorios.o:  colaDirectorios.h colaDirectorios.c
	gcc -g -c colaDirectorios.c
	
clean:
	rm *.o