#ifndef LISTA_H
#define LISTA_H

#include <stdbool.h>
#include <stddef.h>

/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

typedef struct lista lista_t;
typedef struct lista_iter lista_iter_t;

/* ******************************************************************
 *                    PRIMITIVAS DE LA LISTA
 * *****************************************************************/
 
// Crea una lista.
// Post: devuelve una nueva lista vacía.
lista_t *lista_crear(void);


// Devuelve verdadero o falso, según si la lista tiene o no elementos.
// Pre: la lista fue creada.
bool lista_esta_vacia(const lista_t *lista);

// Agrega un elemento en la primera posicion de la lista.
// Devuelve falso en caso de error.
// Pre: la lista fue creada.
// Post: se agregó un nuevo elemento a la lista, dato se encuentra al principio
// de la lista. Devuelve false en caso de error.
bool lista_insertar_primero(lista_t *lista, void *dato);

// Agrega un elemento en la ultima posicion de la lista.
// Devuelve falso en caso de error.
// Pre: la lista fue creada.
// Post: se agregó un nuevo elemento a la lista, dato se encuentra al final
// de la lista. Devuelve false en caso de error.
bool lista_insertar_ultimo(lista_t *lista, void *dato);

// Saca el primer elemento de la lista. Si la lista tiene elementos, se quita el
// primero de la lista, y se devuelve su valor, si está vacía, devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el valor del primer elemento anterior, la lista
// contiene un elemento menos, si la lista  estaba vacía devuelve NULL.
void *lista_borrar_primero(lista_t *lista);

// Obtiene el valor del primer elemento de la lista. Si la lista tiene
// elementos, se devuelve el valor del primero, si está vacía devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el primer elemento de la lista, cuando no está vacía.
void *lista_ver_primero(const lista_t *lista);

// Obtiene el valor del ultimo elemento de la lista. Si la lista tiene
// elementos, se devuelve el valor del ultimo, si está vacía devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el primer elemento de la lista, cuando no está vacía.
void *lista_ver_ultimo(const lista_t* lista);

// Obtiene el largo de la lista.
// Pre: la lista fue creada.
// Post: se devolvio el largo de la lista.
size_t lista_largo(const lista_t *lista);

// Destruye la lista. Si se recibe la función destruir_dato por parámetro,
// para cada uno de los elementos de la lista llama a destruir_dato.
// Pre: la lista fue creada. destruir_dato es una función capaz de destruir
// los datos de la lista, o NULL en caso de que no se la utilice.
// Post: se eliminaron todos los elementos de la lista.
void lista_destruir(lista_t *lista, void destruir_dato(void *));


/* ******************************************************************
 *                    PRIMITIVAS DE ITERACIÓN
 * *****************************************************************/
 
// Obtiene un iterador de la lista.
// Pre: la lista fue creada.
// Post: se inicializó un iterador.
lista_iter_t *lista_iter_crear(lista_t *lista);

// Avanza una posición en la lista.
// Pre: el iterador fue creado.
// Post: Se avanzó la posición actual en el iterador. Devuelve false si ya esta
// Al final de la lista o esta vacía.
bool lista_iter_avanzar(lista_iter_t *iter);

// Devuelve el valor del dato que se encuentra en la posicion actual.
// Pre: el iterador fue creado.
// Post: Se devolvió el dato actual. Devuelve NULL si esta la lista esta vacía. 
void *lista_iter_ver_actual(const lista_iter_t *iter);

// Verifica si ya está al final de la lista
// Pre: el iterador fue creado
// Post: Devuelve true si actual se encuentra al final de la lista, false en caso contrario.
bool lista_iter_al_final(const lista_iter_t *iter);

// Destruye el iterador.
// Pre: el iterador fue creado.
// Post: el iterador fue destruido.
void lista_iter_destruir(lista_iter_t *iter);

// Agrega un elemento en la posición actual
// Pre: la lista y el iterador fueron creados
// Post: se insertó un nuevo nodo antes de la posición actual indicada por el
// iterador
bool lista_iter_insertar(lista_iter_t *iter, void *dato);


// Elimina el elemento de la posición actual
// Pre: la lista y el iterador fueron creados
// Post: se eliminó el nodo que se encontraba en la posición actual indicada por el
// iterador.Devuelve el dato que estaba almacenado o devuelve NULL si la lista esta vacía
void *lista_iter_borrar(lista_iter_t *iter);

/* ******************************************************************
 *                    PRIMITIVA DEL ITERADOR INTERNO
 * *****************************************************************/
//la función de callback "visitar" recibe el dato y un puntero extra, 
//y devuelve true si se debe seguir iterando, false en caso contrario
void lista_iterar(lista_t *lista, bool (*visitar)(void *dato, void *extra), void *extra);
/* *****************************************************************
 *                      PRUEBAS UNITARIAS
 * *****************************************************************/
// Realiza pruebas sobre la implementación del alumno.
//
// Las pruebas deben emplazarse en el archivo ‘pruebas_alumno.c’, y
// solamente pueden emplear la interfaz pública tal y como aparece en lista.h
// (esto es, las pruebas no pueden acceder a los miembros del struct lista).
//
// Para la implementación de las pruebas se debe emplear la función
// print_test(), como se ha visto en TPs anteriores.
void pruebas_lista_alumno(void);

#endif // LISTA_H
