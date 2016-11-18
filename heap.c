/* Horacio Lisdero Padrón: 100132
 * Facundo Halfon Padrón: 95452
 * Corrector: Alejandro Levinas
 */
#include <stdlib.h>
#include <string.h>
#include "heap.h"
#define TAM_INICIAL 32

/* *****************************************************************
 *            Definición de las estructuras de datos               *
 * *****************************************************************/

struct heap {
    void ** datos;
    size_t cant;
    size_t tam;
    cmp_func_t cmp;
};

/* *****************************************************************
 *                    Funciones auxiliares                         *
 * *****************************************************************/

/* Intercambiar dos punteros genéricos */
void intercambiar(void ** a, void ** b)
{
    void *aux = *a;
    *a = *b;
    *b = aux;
}

/* Mueve el elemento de la posición índice hacia arriba en el heap
 * (hacia índices menores en el arreglo) hasta que se respete la propiedad de heap
 */
static void upheap(size_t indice, void ** datos, cmp_func_t cmp)
{
    size_t padre = (indice == 0? 0 : (indice-1)/2); // Calcula el indice del padre

    if (cmp(datos[indice], datos[padre]) > 0) {
        intercambiar(datos + indice, datos + padre);
        upheap(padre, datos, cmp);
    }
}

/* Downheap, basado en el pseudocódigo de MAX_HEAPIFY del Cormen */
/* Mueve el elemento de la posición índice hacia abajo en el heap
 * (hacia índices mayores en el arreglo) hasta que se respete la propiedad de heap
 */
static void downheap(size_t indice, void ** datos, size_t cant, cmp_func_t cmp)
{
    size_t izq = (2*indice + 1);      // Calcula el indice del hijo izquierdo
    size_t der = (2*indice + 2);      // Calcula el indice del hijo derecho
    size_t mayor;

    if (izq < cant && cmp(datos[izq], datos[indice]) > 0) {
        mayor = izq;
    } else {
        mayor = indice;
    }
    if (der < cant && cmp(datos[der], datos[mayor]) > 0) {
        mayor = der;
    }
    if (mayor != indice) {
        intercambiar(datos + indice, datos + mayor);
        downheap(mayor, datos, cant, cmp);
    }
}

/* Condición para agrandar: ocupado más del 75% */
static bool debe_agrandar(size_t cant, size_t tam)
{
    return (4*cant > 3*tam);
}

/* Condición para achicar: ocupado menos del 25% */
static bool debe_achicar(size_t cant, size_t tam)
{
    return (4*cant < tam);
}

/* Redimensiona el heap, recibe el tam nuevo y modifica el heap solo si realloc no falla */
static bool redimensionar_heap(heap_t * heap, size_t tam_nuevo)
{
    void * aux = realloc(heap->datos, tam_nuevo*sizeof(void *));

    if (!aux) {
        return false;
    }
    heap->datos = aux;
    heap->tam = tam_nuevo;
    return true;
}

/* Función de heapsort genérica. Esta función ordena mediante heap_sort
 * un arreglo de punteros opacos, para lo cual requiere que se
 * le pase una función de comparación. Modifica el arreglo "in-place".
 * Nótese que esta función NO es formalmente parte del TAD Heap.
 */
/* Basada sobre el pseudocódigo del Cormen */
void heap_sort(void *elementos[], size_t cant, cmp_func_t cmp)
{
    size_t i;

    /* Convierto el arreglo en un heap de máximos */
    for (i = (cant-1)/2; i > 0; i--) {
        downheap(i, elementos, cant, cmp);
    }
    downheap(0, elementos, cant, cmp);

    /* Ordeno el arreglo, sacando el máximo mientras mantengo la condición de heap */
    for (i = cant-1; i > 0; i--) {
        intercambiar(elementos, elementos + i);
        --cant;
        downheap(0, elementos, cant, cmp);
    }
}

/* *****************************************************************
 *                    Primitivas del Heap                          *
 * *****************************************************************/

heap_t *heap_crear(cmp_func_t cmp)
{
    heap_t * nuevo = malloc(sizeof(heap_t));

    if (!nuevo) {
        return NULL;
    }
    nuevo->datos = malloc(sizeof(void *)*TAM_INICIAL);
    if (!(nuevo->datos)) {
        free(nuevo);
        return NULL;
    }
    nuevo->cant = 0;
    nuevo->tam = TAM_INICIAL;
    nuevo->cmp = cmp;
    return nuevo;
}

heap_t *heap_crear_arr(void *arreglo[], size_t n, cmp_func_t cmp)
{
    size_t i;
    heap_t * nuevo = malloc(sizeof(heap_t));

    if (!nuevo) {
        return NULL;
    }
    nuevo->datos = malloc(sizeof(void*)*n);
    if (!(nuevo->datos)) {
        free(nuevo);
        return NULL;
    }
    /* Debe copiar el arreglo, para poder redimensionarlo después */
    memcpy(nuevo->datos, arreglo, n*sizeof(void *));
    nuevo->cant = n;
    nuevo->tam = n;
    nuevo->cmp = cmp;

    for (i = (n-1)/2; i > 0; i--) {
        downheap(i, nuevo->datos, nuevo->cant, nuevo->cmp);
    }
    downheap(0, nuevo->datos, nuevo->cant, nuevo->cmp);
    return nuevo;
}

void heap_destruir(heap_t *heap, void destruir_elemento(void *e))
{
    size_t i;

    if (destruir_elemento) {
        for (i = 0; i < heap->cant; i++) {
            destruir_elemento(heap->datos[i]);
        }
    }
    free(heap->datos);
    free(heap);
}

size_t heap_cantidad(const heap_t *heap)
{
    return heap->cant;
}

bool heap_esta_vacio(const heap_t *heap)
{
    return (heap->cant == 0);
}

bool heap_encolar(heap_t *heap, void *elem)
{
    if (debe_agrandar(heap->cant, heap->tam) && !redimensionar_heap(heap, 2*heap->tam)) {
        return false;
    }
    /* Inserto el elemento nuevo al final */
    heap->datos[heap->cant] = elem;
    /* Reestablezco la propiedad de heap */
    upheap(heap->cant, heap->datos, heap->cmp);
    ++(heap->cant);
    return true;
}

void *heap_ver_max(const heap_t *heap)
{
    return (heap_esta_vacio(heap)? NULL : heap->datos[0]);
}

void *heap_desencolar(heap_t *heap)
{
    void * dato_salida;

    if (heap_esta_vacio(heap)) {
        return NULL;
    }
    dato_salida = heap->datos[0];
    /* Decrementa la cant, heap->cant es ahora el indice del último elemento, lo pone en 0 */
    heap->datos[0] = heap->datos[--(heap->cant)];
    /* Reestablezco la propiedad de heap */
    downheap(0, heap->datos, heap->cant, heap->cmp);
    if (debe_achicar(heap->cant, heap->tam) && !redimensionar_heap(heap, (heap->tam)/2)) {
        return NULL;
    }
    return dato_salida;
}
