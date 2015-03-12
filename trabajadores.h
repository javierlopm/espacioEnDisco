struct trabajador
{
    int pid;
    int libre = 1;
    int fd[2];
}trabajador;

struct sindicato
{
    int tam;
    trabajador **trabajadores;
}sindicato;

void crearSindicato(*sindicato entrada, int tam);
int  estaLibre(*sindicato entrada);
int *getPipeN(*sindicato entrada,int n);
int  getPidN(*sindicato entrada,int n);
void abolirSindicato(*sindicato entrada);