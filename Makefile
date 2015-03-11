all: UsoDisco

UsoDisco: UsoDisco.o colaDirectorios.o
	gcc UsoDisco.o colaDirectorios.o -o UsoDisco



UsoDisco.o:  UsoDisco.c colaDirectorios.h
	gcc -c UsoDisco.c

colaDirectorios.o:  colaDirectorios.h colaDirectorios.c
	gcc -c colaDirectorios.c
	
clean:
	rm *.o