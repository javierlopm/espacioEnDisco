#include <stdlib.h>
#include "trabajadores.h"

// Crea el arreglo de trabajadores
void crearSindicato(
    sindicato *entrada, // arreglo de procesos trabajadores
    int        tam      // tamaño del arreglo
){
    int i;              // iterador auxiliar
    trabajador *aux;    // apuntador auxiliar a la estructura trabajador (proceso)

    entrada->tam = tam; 
    entrada->trabajadores = (trabajador **) malloc(sizeof(trabajador*) * tam);

    for (i = 0; i < tam; ++i){
        /*Inicializacion de un trabajador*/
        aux        = (trabajador *) malloc(sizeof(trabajador) * tam);
        aux->libre = 1;
        pipe(aux->fd);

        entrada->trabajadores[i] = aux;
    }
}

// Funcion que indica si el proceso esta libre o no
int  estaLibre(
    sindicato *entrada // arreglo de procesos trabajadores
){
    int i;      // iterador auxiliar
    int res;    // indice del primer trabajador libre

    for (i = 0; i < entrada->tam; ++i){
        if(entrada->trabajadores[i]->libre){
            res = i;
            return res; 
        }
    }

    return -1; //Indicador de que no hay trabajador 
}

// Obtiene el pipe correspondiente de un proceso
int *getPipeN(sindicato *entrada, // arreglo de procesos trabajadores
    int n                         // indice correspondiente del arreglo de trabajadores
){
    return entrada->trabajadores[n]->fd;
}

// Obtiene el pid correspondiente de un proceso
int  getPidN(sindicato *entrada,    // arreglo de procesos trabajadores
    int n                           // indice correspondiente del arreglo de trabajadores
){
    return entrada->trabajadores[n]->pid;
}

// Funcion que libera la memoria ocupada por el arreglo
void abolirSindicato(sindicato *entrada // arreglo de procesos trabajadores
){
    int i;      // iterador auxiliar

    for (i = 0; i < entrada->tam; ++i){
        free(entrada->trabajadores[i]);
    }

    free(entrada);
}
