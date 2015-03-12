typedef struct trabajador trabajador;
typedef struct sindicato sindicato;

struct trabajador
{
    int pid;
    int libre;
    int fd[2];
} ;

struct sindicato
{
    int tam;
    trabajador **trabajadores;
};

void crearSindicato (sindicato *entrada, int tam);
int  estaLibre      (sindicato *entrada);
int *getPipeN       (sindicato *entrada,int n);
int  getPidN        (sindicato *entrada,int n);
void abolirSindicato(sindicato *entrada);