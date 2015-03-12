#include "trabajadores.h"

void crearSindicato(*sindicato entrada,int tam){
    int i;
    trabajador *aux;

    entrada->tam = tam;
    entrada->trabajadores = (trabajador **) malloc(sizeof(trabajador*) * tam);

    for (i = 0; i < tam; ++i){
        aux        = (trabajador *) malloc(sizeof(trabajador) * tam);
        entrada[i] = aux;
    }
}

int  estaLibre(*sindicato entrada){
    int i;
    int res;

    for (int i = 0; i < entrada->tam; ++i){
        if(entrada->trabajadores[i]->libre){
            res = i;
            return res; //Indice del primer trabajador libre
        }
    }

    return -1; //Indicador de que no hay trabajador 
}

int *getPipeN(*sindicato entrada,int n){
    return entrada->trabajadores[n]->fd;
}

int  getPidN(*sindicato entrada,int n){
    return entrada->trabajadores[n]->pid;
}

void abolirSindicato(*sindicato entrada){
    for (i = 0; i < entrada->tam; ++i){
        free(entrada[i]);
    }

    free(entrada);
}
