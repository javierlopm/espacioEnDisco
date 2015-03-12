#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <dirent.h>
#include <errno.h>
#include <signal.h> 
#include "colaDirectorios.h"

#define READ  0
#define WRITE 1

#define SOFT 1
#define DIR  2
#define ARCH 3

int tipoArchivo(){
	if (S_ISLNK(fileStat.st_mode)) return 1
	if (S_ISDIR(fileStat.st_mode)) return 2 
	else return 3

}

void entrada_invalida(){
	printf("Entrada invalida.\n");
	printf("Debe ingresar argumentos validos.\n");
	printf("Para mas ayuda ingrese el comando -h\n");
	exit(0);
}


typedef struct arregloPipes
{
  int fd[2];
} arregloPipes;


void trabajar(int n_procesos,int *pidTrabajadores,arregloPipes **arreglo_pipes,int *fdInPadre){
	int nbytes;
	int i,j;
	int childpid;
	int *fd_proc;				//Pipe de un proceso particular (hijo)
	char dirTransicional[255];
	char *salida;
	char *arcActual;
	char *aux;
	int status;

	childpid = getpid();      //Id del proceso

	

    for (i=0;i<n_procesos;i++){
    	if (pidTrabajadores[i]==childpid) fd_proc = arreglo_pipes[i]->fd;
    }

    close(fd_proc[WRITE]);
    close(fdInPadre[READ]);

	while(1){
		//cierro el extremo de escritura fd[1]
		//leer de pipe
		status = read(fd_proc[0], dirTransicional, 255);
		if(status == -1){
			perror("Error de lectura:");
			exit(1);
		}
		//trabajar directorio

		while(1){printf("Hay algo por recorrer\n");}

		//escribe al padre
		status = write(fdInPadre, , strlen());
		if(status == -1){
			perror("Error de escritura:");
			exit(1);
		}
		
		//envia senal de finalizacion y luego pausa
	}

	exit(0);
}

//Crear manejador de senal para cuando un proceso termine y cambie su estado a libre
//void handler


int main(int argc, char const *argv[]){
	
	/*Enteros*/
	int n_procesos;    			//numero de procesos que realizaran el trabajo
	int i;						//Variable auxiliar de iterador
	int *trabLibres;			//Arreglo booleano de 
	int *pidTrabajadores;		//Arreglo con pid de trabajadores
	int status;					//Variable auxiliar para status de procesos
	pid_t   trabajadores;		// id de los procesos trabajadores		
	int info_archivo;           // guarda la info del archivo que da stat
	
	/*Strings*/
	char nombre_entrada[255];		// apuntador a la ruta que se obtiene por input
	char arch_salida[255];   	// nombre del archivo de salida
	char *dirTransicional;      // directorio auxiliar para las colas
	char *aux;
	DIR  *d;
	
	/*stuff*/
	DIR *direct;	// apuntador al directorio
	struct dirent *archivo;		// estructura para el manejo de archivos
	DIR *subdirect;	// apuntador al directorio
	struct dirent *subarchivo;		// estructura para el manejo de archivos
	size_t t = 1;				// iterador para recorrer los directorios
	FILE *salida;				// apuntador al archivo que obtendra la salida del programa
	colaDir *noProcesados;

	arregloPipes **arreglo_pipes;
	int fdInPadre[2];


	
	struct stat fileStat;       // para obtener la info de los archivos

	n_procesos = 1;				// si no se ingresa el numero de procesos, por defecto es 1 

	// Numero de argumentos invalido
	if (argc < 2 | argc == 3 | argc == 5 | argc > 6) entrada_invalida();	
	
	/*Inicio de la lectura de argumentos -------------------------------------*/
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
			aux = getcwd(NULL,0);
			strcpy(nombre_entrada,aux);
			// Si el directorio es vacio
			if ((direct = opendir(nombre_entrada)) == NULL){
				perror("opendir: ");
				exit(0);
			}

		}
		// Si los comandos ejecutados son distintos a -h o salida
		else entrada_invalida();

	}
	if (argc == 4){

		if (strcmp(argv[1],"-n") == 0){

			n_procesos = atoi(argv[2]);
			//nombre_entrada = getcwd(NULL,0);
			aux = getcwd(NULL,0);
			strcpy(nombre_entrada,aux);
			// Si el directorio es vacio
			if ((direct = opendir(nombre_entrada)) == NULL){
				perror("opendir: ");
				exit(0);
			}
		}

		else if (strcmp(argv[1],"-d") == 0){

			strcpy(nombre_entrada,argv[2]);
			// Si el directorio es vacio
			n_procesos = 1;
		}
		else entrada_invalida();

		if ((direct = opendir(nombre_entrada)) == NULL){
			perror("opendir: ");
			exit(0);
		}

		strcpy(arch_salida,argv[3]);

	}
	if (argc == 6){

		if ((strcmp(argv[1], "-n") == 0) && (strcmp(argv[3], "-d") == 0)) {
			
			//Extraccion de argumentos propios de -n -d
			n_procesos = atoi(argv[2]);
			strcpy(nombre_entrada,argv[4]);
		}
		if ((strcmp(argv[1], "-d") == 0) && (strcmp(argv[3], "-n") == 0)) {
			
			//Extraccion de argumentos propios de -d -n
			n_procesos = atoi(argv[4]);
			strcpy(nombre_entrada,argv[2]);
		}
		else entrada_invalida(); 

		//Extraccion de la salida, comun a ambos formatos
		strcpy(arch_salida,argv[5]);

		if ((direct = opendir(nombre_entrada)) == NULL){
			// Si el directorio es vacio
			perror("opendir: ");
			exit(0);
		}

	}

	/*
	// Este caso solo para el directorio actual (cuando no hay -d)
	direct = opendir(".");
	if (direct){
		while ((archivo= readdir(direct))){
			printf("%s\n",archivo->d_name);
			
			// Imprime el numero de bloques para los archivos encontrados en el directorio
			// En el stat esta la info necesaria para los archivos
			
			if(stat(archivo->d_name,&fileStat) < 0)    // aqui asigno el stat 
        		return 1;
        	printf("Num bloques: %ld\n",fileStat.st_blocks);
		}
		closedir(direct);
	}
	*/

	//   ******   OTRA MANERA   ******
	
	
	d = direct;
    /* Open the directory specified by "dir_name". */

    //d = opendir (".");

    /* Check it was opened. */
    if (! d) {
        fprintf (stderr, "Cannot open directory : %s\n",
                  strerror (errno));
        exit (EXIT_FAILURE);
    }
    while (1) {
        struct dirent * entry;
        char * d_name;

        /* "Readdir" gets subsequent entries from "d". */
        entry = readdir (d);
        if (! entry) {
            /* There are no more entries in this directory, so break
               out of the while loop. */
            break;
        }
        //printf("**ESTE ES UN DIRECTORIO: ");
        d_name = entry->d_name;
        /* Print the name of the file and directory. */
		printf ("%s\n", d_name);

		if(lstat(entry->d_name,&fileStat) < 0)    // aqui asigno el stat 
        		return 1;
        	printf("Num bloques: %ld\n",fileStat.st_blocks);
        	
        	switch (fileStat.st_mode & S_IFMT) {
        		
        	case S_IFLNK:  printf("symlink\n"); 
        }
        	printf("The file %s a symbolic link\n", (S_ISLNK(fileStat.st_mode)) ? "is" : "is not");

        	//Dice si es archivo o directorio
			printf( (S_ISDIR(fileStat.st_mode)) ? "*directorio*\n" : "*archivo*\n");
			

			if (
				S_ISDIR(fileStat.st_mode) 		 && 
				(strcmp(entry->d_name,"." )!= 0) && //Ignoramos el dir actual
				(strcmp(entry->d_name,"..")!= 0)    //y su padre
			   ){
				printf("SOY DIRECTORIO\n");

				printf("Y ESTE ES MI NOMBRE**: ");
				printf("%s\n",d_name);

				
				/*Creacion del string para pasar a los hijos*/
				dirTransicional    = (char *) malloc(sizeof(char) * 255);
				dirTransicional[0] = '\0';
				strcpy(dirTransicional,nombre_entrada); //Revisar si termina en /
				strcat(dirTransicional,"/"			 );
				strcat(dirTransicional,entry->d_name );

				for (i = 0; i < n_procesos; i++){
					//Revisamos si hay algun proceso libre y le asignamos el dir
					if(trabLibres[i]){
						trabLibres[i] = 0;

						/*Escribir en su pipe MARIIIII*/
						write(arreglo_pipes[i]->fd[1],dirTransicional,sizeof(dirTransicional));
						//free(dirTransicional)
						break;
					}

					//Si no se encuentra encolamos el directorio a la cola de
					//no procesados
					if((i == n_procesos) && !(trabLibres[i])){
						agregarEnCola(noProcesados,dirTransicional);
					}
				}
				dirTransicional = NULL; //Apuntador tomado por un proceso o cola

			}
			else
				printf("NO SOY DIRECTORIO\n");

			printf("\n");
			
	}	

	// ************************************************




	/*Inicializacion de la informacion de los trabajadores -------------------*/


	//Arreglo de pipes de comunicacion de padres a hijos 
	arreglo_pipes = (arregloPipes**) malloc(sizeof(arregloPipes*) * n_procesos);
	//Pipe de hijos al proceso master
	pipe(fdInPadre);

	//Inicializacion de arreglo booleano de trabajadores libres
	trabLibres 		= (int *) malloc(sizeof(int) * n_procesos);
	for (i = 0; i < n_procesos; i++) trabLibres[i] = 1;

	//Init de pid de trabajadores
	pidTrabajadores = (int *) malloc(sizeof(int) * n_procesos);
	

	// Creamos tantos pipes y procesos como indique el nivel de concurrencia
	for (i=0;i<n_procesos;i++){
		arreglo_pipes[i] = (arregloPipes*) malloc(sizeof(arregloPipes));
		pipe(arreglo_pipes[i]->fd);
        
        if((trabajadores = fork()) == -1)
        {
            perror("fork error");
            exit(1);
        }

        if(trabajadores == 0) break;
        else{
        	/*Almacenamiento del pid del ultimo hijo*/
        	pidTrabajadores[i] = trabajadores;
            /* Parent process closes up output side of pipe */
            close(arreglo_pipes[i]->fd[1]);
        }
    }

    if(trabajadores != 0){	//El padre recorre directorios
        //Reserva de espacio para la cola de procesos a trabajar
        noProcesados = (colaDir *) malloc(sizeof(colaDir));

        //Cerramos el pipe para realizar lectura
        close(fdInPadre[WRITE]);
        



        //recorrer directorio
        if ((direct = opendir("src")) == NULL){
    		perror("opendir");
    		return -1;
    	}
    
    	printf("Directory stream is now open\n");
    
    
        //SE SUPONE QUE DEBERIA RECORRER EL DIRECTORIO
        while((archivo=readdir(direct))!=NULL) printf("%s\n", archivo->d_name);
    		
        if (closedir(direct) == -1){
    		perror("closedir");
    		return -1;
    	}
    
    	printf("\nDirectory stream is now closed\n");

    	
    }
    //Los hijos ejecutan la funcion ciclica esperar/trabajar/enviar senal


    // Recorro el arreglo de pid para encontrar el pipe correspondiente y pasarlo como parametro al
    // procedimiento trabajar, para poder cerrar/abrir el extremo correspondiente

    else trabajar(n_procesos,pidTrabajadores,arreglo_pipes,fdInPadre); 

    /*Luego de finalizar los trabajos, finalizamos a cada hijo*/
    for (i = 0; i < n_procesos; i++){
    	status = kill(pidTrabajadores[i],SIGKILL);
    	if(status == -1){
    		perror("Error eliminando proc: ");
    		exit(1);
    	}
    }

    eliminarLista(noProcesados);
    free(noProcesados);

    //Eliminar los strings
    return 0;
}
	
/*NOTA IMPORTANTE:
	EN ESTA PAGINA
		http://totaki.com/poesiabinaria/2011/09/listar-archivos-dentro-de-un-directorio-o-carpeta-en-c/

	HAY INFO DE COMO RECORRER DIRECTORIOS Y A PARTE PROCESAR UN ARCHIVO!! :)
*/

/*MAS IMPORTANTE:
	http://codewiki.wikidot.com/c:system-calls:stat

	ES LA QUE TAL
*/
