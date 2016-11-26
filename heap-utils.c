#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "heap.h"

struct heap {
    void ** datos;
    size_t cant;
    size_t tam;
    cmp_func_t cmp;
};

void** top_k(size_t k, void** datos, size_t tam_datos, cmp_func_t cmp)
{
	heap_t* heap = heap_crear(cmp);
	int i;
	for (i = 0; i < tam_datos; i++){
		heap_encolar(heap,datos[i]);
	}
	void ** nuevo = calloc(k,sizeof(datos));
	for (i = k-1; i >= 0;i--){
		if( i >= tam_datos){
			nuevo[i] = NULL;
		}
		else {
		    nuevo[i] = heap_desencolar(heap);
		}
	}
	heap_destruir(heap,NULL);
	return nuevo;
}

void swap(void ** a, void ** b)
{
    void *aux = *a;
    *a = *b;
    *b = aux;
}

static void actualizar_prioridad(size_t indice, void ** datos)
{
    size_t padre = (indice == 0? 0 : (indice-1)/2); // Calcula el indice del padre
    if (padre>0){
        swap(datos[indice], datos[padre]);
        actualizar_prioridad(padre, datos);
    }
    if (padre == 0){
		swap(datos[indice], datos[padre]);
	}
}

void heap_actualizar_prioridad(heap_t* heap, void* dato)
{
	int indice; 
	void ** arreglo = malloc(sizeof(dato));
	for (indice = 0; dato != heap_ver_max(heap) ; indice++){
		arreglo[indice] = heap_desencolar(heap);
		arreglo=realloc(arreglo,sizeof(dato)* (indice + 2));
	}
	int i;
    for (i = 0; i<indice ; i++){
		heap_encolar(heap,arreglo[i]);
	}
	actualizar_prioridad(indice,heap->datos);
	free(arreglo);
}
