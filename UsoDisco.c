#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <dirent.h>
#include <errno.h>

void entrada_invalida(){
	printf("Entrada invalida.\n");
	printf("Debe ingresar argumentos validos.\n");
	printf("Para mas ayuda ingrese el comando -h\n");
	exit(0);
}

void main(int argc, char const *argv[]){
	
	int n_procesos, i;    		/*entero	n_procesos: numero de procesos que realizaran el trabajo
											i:  		iterador
								*/
	pid_t   trabajadores;		// id de los procesos trabajadores		
	char* nombre_entrada;		// apuntador a la ruta que se obtiene por input
	char arch_salida[15];   	// nombre del archivo de salida
		
	DIR *direct=opendir(".");	// apuntador al directorio
								// con opendir abri el directorio actual
	struct dirent *archivo;		// estructura para el manejo de archivos
	size_t t = 1;				// iterador para recorrer los directorios
	FILE *salida;				// apuntador al archivo que obtendra la salida del programa
	typedef struct arregloPipes
	{
	  int fd[2];
	} ARREGLO;

	n_procesos = 1;				// si no se ingresa el numero de procesos, por defecto es 1 


	// Numero de argumentos invalido
	if (argc < 2 | argc == 3 | argc == 5 | argc > 6) {
		entrada_invalida();	
	}
	
	if (argc == 2){
		// Manual del programa
		if (strcmp(argv[1],"-h")==0){
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
		// Solo se indica el nombre del archivo de salida
		else if (strcmp(argv[1],"salida")==0){    //!!!!!!!!!No hay que comparar con el string salida, hay que ver si es diferente a -h y luego si el archivo existe

			strcpy(arch_salida,argv[1]);
			n_procesos = 1;
			nombre_entrada = getcwd(NULL,0);
			// Si el directorio es vacio
			if ((direct = opendir(nombre_entrada)) == NULL){
				perror("opendir: ");
				exit(0);
			}

			}
		// Si los comandos ejecutados son distintos a -h o salida
		

		else

			entrada_invalida();

	}

	if (argc == 4){

		if (strcmp(argv[1],"-n") == 0){

			n_procesos = atoi(argv[2]);
			nombre_entrada = getcwd(NULL,0);
			// Si el directorio es vacio
			if ((direct = opendir(nombre_entrada)) == NULL){
				perror("opendir: ");
				exit(0);
			}
		}

		else if (strcmp(argv[1],"-d") == 0){

			strcpy(nombre_entrada,argv[2]);
			// Si el directorio es vacio
			if ((direct = opendir(nombre_entrada)) == NULL){
				perror("opendir: ");
				exit(0);
			}
			n_procesos = 1;
		}
		else

			entrada_invalida();

		strcpy(arch_salida,argv[3]);

	}
	
	
	if (argc == 6){

		if (strcmp(argv[1], "-n") == 0){
			
			n_procesos = atoi(argv[2]);
			
			if (strcmp(argv[3], "-d") == 0){
			
				strcpy(nombre_entrada,argv[4]);
				// Si el directorio es vacio
				if ((direct = opendir(nombre_entrada)) == NULL){
					perror("opendir: ");
					exit(0);
				}
			}

			strcpy(arch_salida,argv[5]);

		}

		if (strcmp(argv[1], "-d") == 0){

			strcpy(nombre_entrada,argv[2]);
			// Si el directorio es vacio
			if ((direct = opendir(nombre_entrada)) == NULL){
				perror("opendir: ");
				exit(0);
			}

			if (strcmp(argv[3], "-n") == 0)
				
				n_procesos = atoi(argv[4]);	

			strcpy(arch_salida,argv[5]);
			
		}

		else
		
			entrada_invalida();

	}


	// Arreglo de pipes de acuerdo al numero de procesos
	struct arregloPipes* arreglo_pipes;
	arreglo_pipes = (struct arregloPipes*) malloc(sizeof(struct arregloPipes*) * n_procesos);

	// Crea tantos pipes como trabajadores haya
	for (i=0;i<n_procesos;i++){
		pipe(arreglo_pipes.fd);
	}

        
        if((trabajadores = fork()) == -1)
        {
                perror("fork");
                exit(1);
        }

        if(trabajadores == 0)
        {
                /* Child process closes up input side of pipe */
                close(fd[0]);

                /* Send "string" through the output side of pipe */
                
                // ABAJITO ESTA EXPLICADO POR QUE ESTA COMENTADO AUN

                //write(fd[1], string, (strlen(string)+1));
                
                // AQUI ARRIBA EN STRING NO ESTOY CLARA CUAL NOMBRE 
                //DE ARCHIVO VA, O SI ES LOS DATOS DE SALIDA
                
                exit(0);
        }
        else
        {
                /* Parent process closes up output side of pipe */
                close(fd[1]);

                /* Read in a string from the pipe */
                
                // LOS DE ABAJO ESTAN COMENTADOS PARA QUE NO EXPLOTE, HAY QUE PONER 
                // BIEN EL STRING DE ARRIBA

                //nbytes = read(fd[0], readbuffer, sizeof(readbuffer));
                //printf("Received string: %s", readbuffer);
        }

        //recorrer directorio
        if ((direct = opendir("src")) == NULL){
			perror("opendir");
			return -1;
			}

		printf("Directory stream is now open\n");


        //SE SUPONE QUE DEBERIA RECORRER EL DIRECTORIO
        while((archivo=readdir(direct)!=NULL))
			
			printf("%s\n", archivo->d_name);
			
        if (closedir(direct) == -1){
			perror("closedir");
			return -1;
		}

		printf("\nDirectory stream is now closed\n");
        
}

/*NOTA IMPORTANTE:
	EN ESTA PAGINA
		http://totaki.com/poesiabinaria/2011/09/listar-archivos-dentro-de-un-directorio-o-carpeta-en-c/

	HAY INFO DE COMO RECORRER DIRECTORIOS Y A PARTE PROCESAR UN ARCHIVO!! :)
*/