typedef struct caja{
        char *contenido;           //Apuntador al string de un directorio
        struct caja *siguiente;    //Apuntador a otra caja de string
}caja;

typedef struct colaDir{
        int tam;               //Cantidad de elementos en la lista       
        struct caja *primero;  //Apuntador a la primera #caja de la lista
        struct caja *ultimo;   //Apuntador a la ultima  #caja de la lista
}colaDir;

void crearCola    (colaDir *aux);
void agregarEnCola(colaDir *aux,char *in);
void desencolar   (colaDir *aux);
int empty(colaDir *aux);
char *mostrarPrimero(colaDir *aux);
char *pop(colaDir *aux);

void eliminarCola(colaDir *aux);