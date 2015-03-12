typedef struct trabajador trabajador;	// estructura para el proceso trabajador
typedef struct sindicato sindicato;		// arreglo de procesos trabajadores

struct trabajador
{
    int pid;	// pid del proceso trabajador
    int libre;	// booleano que indica si el proceso esta libre o no
    int fd[2];	// arreglo de enteros para el pipe del proceso
} ;

struct sindicato
{
    int tam;					// tamaño del arreglo
    trabajador **trabajadores;	// apuntador de apuntadores a la estructura trabajador (proceso)
};

void crearSindicato (sindicato *entrada, int tam);	// crea el arreglo de trabajadores
int  estaLibre      (sindicato *entrada);			// funcion que indica si el proceso esta libre o no
int *getPipeN       (sindicato *entrada,int n);		// obtiene el pipe correspondiente de un proceso
int  getPidN        (sindicato *entrada,int n);		// obtiene el pid correspondiente de un proceso
void abolirSindicato(sindicato *entrada);			// funcion que libera la memoria ocupada por el arreglo