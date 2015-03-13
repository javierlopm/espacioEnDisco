#include <stdio.h>
#include <stdlib.h>
#include "colaDirectorios.h"

// Asignacion de valores iniciales
void crearCola(colaDir *aux){
    aux->tam = 0;
    aux->primero = NULL;
    aux->ultimo  = NULL;
}

// Caja de contenido, auxiliar para la cola
void crearCaja(caja *aux,char *in){
    aux->contenido = in;
    aux->siguiente = NULL;
}

// Agregar un string a la cola
void agregarEnCola(colaDir *aux,char *in){
    caja *cajaAuxiliar;

    //Creacion de nueva caja
    cajaAuxiliar = (caja*) malloc(sizeof(caja));
    crearCaja(cajaAuxiliar,in);

    //Proceso de enlazado
    if(aux->tam > 0){
        aux->ultimo->siguiente = cajaAuxiliar;
    } 
    else{
        aux->primero = cajaAuxiliar;
    }

    //Actualizacion de ultimo apuntador y contador de elementos
    aux->ultimo = cajaAuxiliar;
    aux->tam = aux->tam + 1;
}

// Eliminar primer elemento
void desencolar(colaDir *aux){
    caja *segundo;
    if(aux->primero != NULL){
        aux->tam = (aux->tam) -1;
        segundo = (aux->primero)->siguiente;

        free(aux->primero); //Eliminamos la caja y no su contenido
        if(aux->tam == 0){
            aux->primero = NULL;
            aux->ultimo  = NULL;
        }else{
            aux->primero = segundo;
        }
        
    }
}

// Muestra el tope
char *mostrarPrimero(colaDir *aux){
    if(aux->primero != NULL){
        return aux->primero->contenido;
    }
    else{
        return 0;
    }
}

// extrar tope y desencolar
char *pop(colaDir *aux){
    char *salida;

    salida = aux->primero->contenido;
    desencolar(aux);

    return salida;
}

//Indica si la cola esta vacia
int empty(colaDir *aux){
    return (aux->tam == 0);
}

//Liberar memoria ocupada por cola
void eliminarCola(colaDir *aux){
    caja *iterador,*anterior;

    iterador = aux->primero;
    anterior = iterador;

    //Iteracion liberando todos los espacios ocupados por las cajas
    //pero no por sus contenidos
    while(iterador!= NULL){
        iterador=iterador->siguiente;
        //free(anterior->contenido); No se elminan los contenidos
        free(anterior);
        anterior = iterador;
    }

    //Reinicio de valores
    aux->primero = NULL;
    aux->ultimo  = NULL;
    aux->tam     = 0;
}
