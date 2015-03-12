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


typedef struct arregloPipes
{
  int fd[2];
} arregloPipes;


/*Usar para los hijos
dirTransicional    = (char *) malloc(sizeof(char) * 255);
dirTransicional[0] = '\0';
strcpy(dirTransicional,nombre_entrada); //Revisar si termina en /
strcat(dirTransicional,"/"			 );
strcat(dirTransicional,entry->d_name );

agregarEnCola(noProcesados,dirTransicional);
dirTransicional = NULL; //Apuntador tomado por un proceso o cola
*/

void trabajar(
	sindicato *arrTrab,
	int *fdInPadre					//Pipe de salida para comunicar con el padre
)
{	
	/*Enteros*/
	int i;						//Iterador auxiliar
	int childpid;				//Pid del proceso actual
	int *fd_proc;				//Pipe de un proceso particular (hijo)
	int numLinks   = 0;			//Cantidad de soflinks encontrados
	int numBloques = 0;			//Cantidad de bloques encontrados
	int status;					//Status de salida de las escrituras a pipes

	/*Strings*/
	char dirTransicional[255];  //String para la lectura del directorio a trab
	char *salida;				//String dinamico para comunicar al padre
	char *arcActual;			//Nombre del string 
	char *aux;					//String auxiliar para concatenacion dinamica
	
	/*Estructura auxiliar*/
	colaDir *dirPorRecorrer;	//Cola de directorios


	//Busqueda del pipe adecuado para el pid del trabajador actual
	childpid = getpid();	
    for (i=0;i<arrTrab->tam;i++){
    	printf("Este es el pid que estoy verificando %d\n",getPidN(arrTrab,i));
    	if (getPidN(arrTrab,i)==childpid){
    		printf("Found it no joda pid %d, i=%d\n",childpid,i);
    		fd_proc = getPipeN(arrTrab,i);
    	}
    }

    close(fd_proc[WRITE]);  //Se cierra el extremo write para leer del padre
    close(fdInPadre[READ]); //Se cierra el extremo read para escribir al padre

	while(1){
		//Limpiar string y leer directorio a trabajar
		dirTransicional[0] = '\0';
		printf("FILE DESCRIPTOR: %d\n",fd_proc[READ]);
		status = read(fd_proc[READ], dirTransicional, strlen(dirTransicional)+1);
		if(status == -1){
			perror("Error de lectura:");
			exit(1);
		}

		/*Inicializacion de la cola de directorios pendientes*/
		dirPorRecorrer = (colaDir *) malloc (sizeof(colaDir));
		crearCola(dirPorRecorrer);
		
		//trabajar directorio
		while(1){printf("Hay algo por recorrer\n");}

		//Escribir al padre en formato fijo
		status  = write(fdInPadre[WRITE],&childpid  , sizeof(int)); //Entero que realizo la llamada
		status  = write(fdInPadre[WRITE],&numBloques, sizeof(int)); //Bloques
		status += write(fdInPadre[WRITE],&numLinks  , sizeof(int)); //Links
		status += write(fdInPadre[WRITE],salida     , sizeof(char)*strlen(salida)); //Strings recorridos
		
		//Libera la cola
		eliminarCola(dirPorRecorrer);
		free(dirPorRecorrer);

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
	int numLinks   = 0;         //cantidad de enlaces logicos
	int fdInPadre[2];           // pipe de escritura de hijos al padre
	
	/*Strings*/
	char nombre_entrada[255];	// apuntador a la ruta que se obtiene por input
	char arch_salida[255];   	// nombre del archivo de salida
	char *dirTransicional;      // directorio auxiliar para las colas
	char *aux;
	char * d_name;

	/*Estructuras*/
	struct dirent *archivo;		 // estructuras para el manejo de archivos
	struct dirent *entry;
	DIR           *d;			 // apuntador auxiliar
	FILE 		  *salida;		 // salida del programa
	colaDir 	  *noProcesados; // cola de directorios a procesar por hijos
	sindicato     *arrTrab;	     // estructura para informacion de trabajadore
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
		else{  // Solo se indica el nombre del archivo de salida  

			n_procesos = 1;
			strcpy(nombre_entrada,".");
			
			// Si el directorio es vacio error
			if ((d = opendir(nombre_entrada)) == NULL){
				perror("opendir: ");
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
			exit(0);
		}
	}

	salida = fopen(argv[argc-1],"w");

    if (! d) {
        fprintf (stderr, "Cannot open directory : %s\n",
                  strerror (errno));
        exit (EXIT_FAILURE);
    }

	// ************************************************


	/*Inicializacion de la informacion de los trabajadores -------------------*/


	//Arreglo de pipes de comunicacion de padres a hijos 
	
	//arreglo_pipes = (arregloPipes**) malloc(sizeof(arregloPipes*) * n_procesos);
	//Pipe de hijos al proceso master
	arrTrab = (sindicato *) malloc(sizeof(sindicato));
	crearSindicato(arrTrab,n_procesos);

	pipe(fdInPadre);

	//Inicializacion de arreglo booleano de trabajadores libres
	//trabLibres 		= (int *) malloc(sizeof(int) * n_procesos);
	//for (i = 0; i < n_procesos; i++) trabLibres[i] = 1;

	//Init de pid de trabajadores
	//pidTrabajadores = (int *) malloc(sizeof(int) * n_procesos);
	

	// Creamos tantos pipes y procesos como indique el nivel de concurrencia
	for (i=0;i<n_procesos;i++){
		//arreglo_pipes[i] = (arregloPipes*) malloc(sizeof(arregloPipes));
		//pipe(arreglo_pipes[i]->fd);
        
        if((trabajadores = fork()) == -1)
        {
            perror("fork error");
            exit(1);
        }

        if(trabajadores == 0) break;
        else{
        	/*Almacenamiento del pid del ultimo hijo*/
        	arrTrab->trabajadores[i]->pid = trabajadores;
            /* Parent process closes up output side of pipe */
            close(arrTrab->trabajadores[i]->fd[WRITE]);
        }
    }

    if(trabajadores != 0){	//El padre recorre directorios

        //Reserva de espacio para la cola de procesos a trabajar
        noProcesados = (colaDir *) malloc(sizeof(colaDir));
        crearCola(noProcesados);

        //Cerramos el pipe para realizar lectura
        close(fdInPadre[WRITE]);
        

    	printf("Directory stream is now open\n");
    
    
    	
        //SE SUPONE QUE DEBERIA RECORRER EL DIRECTORIO
        while((entry=readdir(d))!=NULL){
	       
	        d_name = entry->d_name;
			if(lstat(entry->d_name,&fileStat) < 0) return 1;
				
			switch(tipoArchivo(fileStat)){
				case ARCH:
					fprintf(
							salida,"%ld %s/%s\n",
							fileStat.st_blocks,nombre_entrada,d_name
							);
					break;
				case SOFT:
					numLinks += 1;
					break;
				case DIRECT:
					if( 
						(strcmp(entry->d_name,"." )!= 0) &&
						(strcmp(entry->d_name,"..")!= 0)
					  )agregarEnCola(noProcesados,dirTransicional);
					break;
			}

        }
    		
    	

        if (closedir(d) == -1){
    		perror("closedir");
    		return -1;
    	}
    
    	printf("\nDirectory stream is now closed\n");
    	
    }
    //Los hijos ejecutan la funcion ciclica esperar/trabajar/enviar senal


    // Recorro el arreglo de pid para encontrar el pipe correspondiente y pasarlo como parametro al
    // procedimiento trabajar, para poder cerrar/abrir el extremo correspondiente

    else trabajar(arrTrab,fdInPadre); 

    /*Luego de finalizar los trabajos, finalizamos a cada hijo*/
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

    //Eliminar los strings
    return 0;
}
