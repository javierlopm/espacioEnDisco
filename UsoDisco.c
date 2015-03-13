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
#include "trabajador.h"

 
/*Constantes read/write de pipes*/
#define READ  0
#define WRITE 1

/*Constantes que definen tipo archivo*/
#define SOFT 1
#define DIRECT  2
#define ARCH 3

int tipoArchivo(struct stat archivo){
	if (S_ISLNK(archivo.st_mode)) return 1;
	if (S_ISDIR(archivo.st_mode)) return 2 ;
	else return 3;

}

void entrada_invalida(){
	printf("Entrada invalida.\n");
	printf("Debe ingresar argumentos validos.\n");
	printf("Para mas ayuda ingrese el comando -h\n");
	exit(0);
}


int trabajar(
	sindicato *arrTrab,
	int *fdInPadre,					//Pipe de salida para comunicar con el padre
	int numProc
)
{	
	/*Enteros*/
	int i;						//Iterador auxiliar
	int childpid;				//Pid del proceso actual
	int *fd_proc;				//Pipe de un proceso particular (hijo)
	int numLinks   = 0;			//Cantidad de soflinks encontrados
	int numBloques = 0;			//Cantidad de bloques encontrados
	int status;					//Status de salida de las escrituras a pipes
	int indiceLibre;

	/*Strings*/
	char dirTransicional[255];  //String para la lectura del directorio a trab
	char *salida;				//String dinamico para comunicar al padre
	char *arcActual;			//Nombre del string 
	char *aux,*auxconcat;	    //String auxiliar para concatenacion dinamica
	char * d_name;
	char *path_adicional;
	/*Estructura auxiliar*/
	colaDir *dirPorRecorrer;	//Cola de directorios
	struct dirent *entry;
	DIR           *d;	
	struct stat fileStat; 

	//Busqueda del pipe adecuado para el pid del trabajador actual
	childpid = getpid();	

    fd_proc = getPipeN(arrTrab,numProc);

    close(fd_proc[WRITE]);  //Se cierra el extremo write para leer del padre
    close(fdInPadre[READ]); //Se cierra el extremo read para escribir al padre

	while(1){
		//Limpiar string y leer directorio a trabajar


		memset(dirTransicional,'\0',254); //Aseguramos que el string no tenga basura

		status = read(fd_proc[READ], dirTransicional, 255);
		if(status == -1){
			perror("Error de lectura:");
			exit(1);
		}

		/*Inicializacion de la cola de directorios pendientes*/
		dirPorRecorrer = (colaDir *) malloc (sizeof(colaDir));
		crearCola(dirPorRecorrer);
		agregarEnCola(dirPorRecorrer,dirTransicional);

		salida = (char*) malloc(sizeof(char) * 3);
		salida[0] = '\0';

		//trabajar directorio
		while(!empty(dirPorRecorrer)){

			path_adicional = pop(dirPorRecorrer);
			

			if ((d = opendir(path_adicional)) == NULL){
				perror("opendir: ");
				printf("En directorio %s en %d \n",path_adicional,1);
				exit(0);
			}


			while((entry=readdir(d))!=NULL ){
		       
		       	if(lstat(entry->d_name,&fileStat) < 0) {
		       		perror("lstat1: ");
		       		exit(1);
       			}
		        d_name = entry->d_name;

				//Seleccionamos trabajos para cada tipo de archivo
				switch(tipoArchivo(fileStat)){
					case ARCH:   //Los archivos se imprimen al archivo de salida
						//concatenar a la ruta
						aux = (char*) malloc(sizeof(char) + strlen(dirTransicional) + strlen(path_adicional) + 20);
						sprintf(aux,"%d",(int) fileStat.st_blocks);
						strcat(aux," ");
						strcat(aux,dirTransicional);
						strcat(aux,"/");
						strcat(aux,path_adicional);
						strcat(aux,"\n");

						auxconcat = (char *) malloc(strlen(aux) + strlen(salida) + 1);
						strcpy(auxconcat,salida);
						strcpy(auxconcat,aux);

						free(salida);
						free(aux);
						salida = auxconcat;
						
						break;
					case SOFT:	//Los soft link aumentan el contador
						numLinks += 1;
						
						break;
					case DIRECT: //Los directorios se encolan
						if( 
							(strcmp(entry->d_name,"." )!= 0) &&
							(strcmp(entry->d_name,"..")!= 0)
						  ){
							aux = (char*) malloc(sizeof(char) + strlen(dirTransicional) + strlen(path_adicional) + 20);
							strcat(aux,dirTransicional);
							strcat(aux,"/");
							strcat(aux,path_adicional);
							strcat(aux,"/");

							agregarEnCola(dirPorRecorrer,aux);

							}
						  
						break;
				}
	        }
	    }



		//Escribir al padre en formato fijo
		status  = write(fdInPadre[WRITE],&childpid  , sizeof(int)); //Entero que realizo la llamada
		status += write(fdInPadre[WRITE],&numLinks  , sizeof(int)); //Links
		status += write(fdInPadre[WRITE],salida     , strlen(salida)); //Strings recorridos
		
		//Libera la cola
		eliminarCola(dirPorRecorrer);
		free(dirPorRecorrer);

	}

	exit(0);
	return (0);
}



int main(int argc, char const *argv[]){
	
	/*Enteros*/
	int n_procesos;    			// numero de procesos que realizaran el trabajo
	int i;						// Variable auxiliar de iterador
	int *trabLibres;			// Arreglo booleano de 
	int *pidTrabajadores;		// Arreglo con pid de trabajadores
	int status;					// Variable auxiliar para status de procesos
	pid_t   trabajadores;		// id de los procesos trabajadores		
	int info_archivo;           // guarda la info del archivo que da stat
	int numLinks   = 0;         // cantidad de enlaces logicos
	int fdInPadre[2];           // pipe de escritura de hijos al padre
	int pidAux;				    // enteros auxiliare para pids
	int childpid;				
	int numProc;				// argumento indice para cada trabajador
	int indiceLibre;		    // Almacena el indice del 1er trabajador libre
	int auxLinks;				// Almacenar los valores obtenidos de los trab.

	/*Strings*/
	char nombre_entrada[255];	// apuntador a la ruta que se obtiene por input
	char arch_salida[255];   	// nombre del archivo de salida
	char *dirTransicional;      // directorio auxiliar para las colas
	char *aux;					// Auxiliar de transferencia
	char * d_name;				// Nombre de archivo/dir encontrado
	char * byteRead;			// string aux para lectura de un byte en pipe

	/*Estructuras*/
	struct dirent *archivo;		 // estructuras para el manejo de archivos
	struct dirent *entry;
	DIR           *d;			 // apuntador auxiliar
	FILE 		  *salida;		 // salida del programa
	colaDir 	  *noProcesados; // cola de directorios a procesar por hijos
	sindicato     *arrTrab;	     // estructura para informacion de trabajadore
	struct stat fileStat;        // para obtener la info de los archivos

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
		else{  // Solo se indica el nombre del archivo de salida  

			n_procesos = 1;
			strcpy(nombre_entrada,".");
			
			// Si el directorio es vacio error
			if ((d = opendir(nombre_entrada)) == NULL){
				perror("opendir: ");
				printf("En directorio %s en %d \n",nombre_entrada,2);
				exit(0);
			}
		}
		// Si los comandos ejecutados son distintos a -h o salida
	}
	else if (argc == 4){

		if (strcmp(argv[1],"-n") == 0){

			n_procesos = atoi(argv[2]);
			strcpy(nombre_entrada,".");
		}

		else if (strcmp(argv[1],"-d") == 0){

			strcpy(nombre_entrada,argv[2]);
			n_procesos = 1;
		}
		else entrada_invalida();

		if ((d = opendir(nombre_entrada)) == NULL){
			perror("opendir: ");
			printf("En directorio %s en %d \n",nombre_entrada,3);
			exit(0);
		}
	}
	else if (argc == 6){

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



		if ((d = opendir(nombre_entrada)) == NULL){
			// Si el directorio es vacio
			perror("opendir: ");
			printf("En directorio %s en %d \n",nombre_entrada,4);
			exit(0);
		}
	}


	salida = fopen(argv[argc-1],"w");

	/*Inicializacion de la informacion de los trabajadores -------------------*/

	//Pipe de hijos al proceso master
	pipe(fdInPadre);

	arrTrab = (sindicato *) malloc(sizeof(sindicato));
	crearSindicato(arrTrab,n_procesos);

	
	for (i=0;i<n_procesos;i++){
        
        if((trabajadores = fork()) == -1){
            perror("fork error");
            exit(1);
        }

        if(trabajadores == 0){
        	//Almacenamos posicion en el arreglo del proc
        	numProc = i;
        	break;
        }
        else{
        	//Almacenamiento del pid del ultimo hijo
        	arrTrab->trabajadores[i]->pid = trabajadores;
            //cerramos el extremo inutilizado
            close(arrTrab->trabajadores[i]->fd[READ]);
        }
    }

    /* Trabajos a realizar por cada proceso  ---------------------------------*/

    if(trabajadores != 0){	//El padre recorre directorios luego asigna trabajos

        //Reserva de espacio para la cola de procesos a trabajar
        noProcesados = (colaDir *) malloc(sizeof(colaDir));
        crearCola(noProcesados);

        //Cerramos el pipe para realizar lectura
        close(fdInPadre[WRITE]);
        
        //Ciclo para recorrer el directorio del comando
        while((entry=readdir(d))!=NULL){
	       
	        d_name = entry->d_name;
			if(lstat(entry->d_name,&fileStat) < 0) return 1;
			
			//Seleccionamos trabajos para cada tipo de archivo
			switch(tipoArchivo(fileStat)){
				case ARCH:   //Los archivos se imprimen al archivo de salida
					fprintf(
							salida,"%ld %s/%s\n",
							fileStat.st_blocks,nombre_entrada,d_name
							);
					break;
				case SOFT:	//Los soft link aumentan el contador
					numLinks += 1;
					break;
				case DIRECT: //Los directorios se encolan
					
					if( 
						(strcmp(entry->d_name,"." )!= 0) &&
						(strcmp(entry->d_name,"..")!= 0)
					  )agregarEnCola(noProcesados,d_name);
					break;
			}
        }
    	
    	//Clausura del directorio
        if (closedir(d) == -1){
    		perror("closedir");
    		return -1;
    	}
    	
    }else trabajar(arrTrab,fdInPadre,numProc); // Funcion para proc hijos

    //Algoritmo para la asignacion de trabajos por parte del padre

    byteRead = (char *) malloc(sizeof(char) * 3);



    while(!empty(noProcesados)){
    	indiceLibre = estaLibre(arrTrab);

    	//Si se encuentra un proceso libre...
    	if(indiceLibre != -1){
    		//Se cambia el estado del trabajador encontrado
    		cambiarLibre(arrTrab,indiceLibre,0);

    		//Inicio el trabajo
    		aux = pop(noProcesados);

    		printf("Soy el main y paso a mi hijo %s\n",aux);

    		write(
    			  getPipeN(arrTrab,indiceLibre)[WRITE], //Pipe a escribir
    			  aux,								    //primer string de cola
    			  strlen(aux) * sizeof(char)			//tam string
    			  );
    	}
    	else{
    		//Espero la escritura de algun hijo y leo

    		//Modificamos el estado del trabajador que termino
			read(fdInPadre[READ],&childpid  , sizeof(int)); 
			cambiarLibre(arrTrab,getIndicePid(arrTrab,childpid),1);

			//Tomamos los links que encontro
			read(fdInPadre[READ],&auxLinks  , sizeof(int)); 
			numLinks += auxLinks;

			strcpy(byteRead,"m"); //Caracter de control
			while( strcmp(byteRead,"\0") ){
				read(fdInPadre[READ],byteRead,1);

				if( strcmp(byteRead,"\0") ){
					fprintf(salida,"%c",byteRead[0]);
				}

			} //Strings recorridos
    	}
    }

    fprintf(salida,"Total de enlaces logicos: %d",numLinks);
    
    //El programa debe finalizar, se envia senal para matar a los hijos
    for (i = 0; i < n_procesos; i++){
    	status = kill(getPidN(arrTrab,i),SIGKILL);
    	if(status == -1){
    		perror("Error eliminando proc: ");
    		exit(1);
    	}
    }


    eliminarCola(noProcesados);
    free(noProcesados);

    fclose(salida);
    return 0;
}
