#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/stat.h>

void main(int argc, char const *argv[]){
	if ((argc ==2)&&(strcmp(argv[1],"-h")==0)){
		printf(" NAME \n");
		printf("    Uso Disco - Calcula el espacio en disco utilizado por los archivos\n");
		printf("    regulares a partir de un directorio usando procesos.\n"); 
		printf("\n");
		printf("    Sintaxis:\n");
		printf("              Uso Disco [-h] | [-n i] [-d directorio] salida\n");  
		printf("\n");
		printf("    Comandos:\n"); 
		printf("        - h             mensaje de ayuda.\n"); 
		printf("        - n i           nivel de concurrencia solicitado.\n"); 
		printf("                        i es un entero.\n"); 
		printf("        - d directorio  especifica un directorio desde donde calcula\n");
		printf("                        el espacio utilizado. Por defecto hace el\n"); 
		printf("                        calculo desde el directorio actual.\n"); 
		printf("         salida:        archivo que contendra la lista de directorios\n");
		printf("                        y el espacio en bloques ocupado por los archivos\n");
		printf("                        en bloques ocupado por los archivos regulares y\n");
		printf("                        el numero total de enlaces logicos encontrados\n");
		printf("\n");
		exit(0);
	}
	else if ((argc ==2)&&(strcmp(argv[1],"salida")==0)){
	}
	else{
		printf("Entrada invalida.\n");
		printf("Debe ingresar argumentos validos.\n");
		printf("Para mas ayuda ingrese el comando -h\n");
	}
}