#define _POSIX_C_SOURCE 200809L
#include "hash.h"
#include "lista.h"
#include <stdlib.h>
#include <string.h>
#define TAM_INICIAL 67
#define FACTOR_CARGA_MAX 2
#define FACTOR_CARGA_MIN 0.3
#define FACTOR_ACHIQUE 3
#define FACTOR_AGRANDAMIENTO 3

/* Definiciones de estructuras de la tabla de hash */

struct hash {
    lista_t **datos;
    size_t cantidad;
    size_t tam;
    hash_destruir_dato_t destruir_dato;
};

struct hash_iter {
    const hash_t *hash;
    lista_iter_t *lista_iter;
    size_t pos;
};

/* Estructura para guardar los datos */
typedef struct nodo {
    char *clave;
    void *dato;
} hash_nodo_t;

/* *****************************************************************
 *                    Funciones del nodo                           *
 * *****************************************************************/

static hash_nodo_t *nodo_crear(const char *clave, void *dato)
{
    /* Todo nodo debe tener una clave, no se crean nodos vacios */
    if (!clave) return NULL;
    hash_nodo_t *nuevo = malloc(sizeof(*nuevo));
    if (!nuevo)
        return NULL;
    /* Copiar la clave y guardar el dato */
    nuevo->clave = strdup(clave);
    if (!(nuevo->clave)) {
        free(nuevo);
        return NULL;
    }
    nuevo->dato = dato;
    return nuevo;
}

static void nodo_destruir(hash_nodo_t *nodo, hash_destruir_dato_t destruir_dato)
{
    if (!nodo) return;
    if (destruir_dato)
        destruir_dato(nodo->dato);
    free(nodo->clave);
    free(nodo);
}

/* *****************************************************************
 *                    Funciones auxiliares                         *
 * *****************************************************************/

/* Destruye la lista del indice recibido si está vacía, sino no hace nada */
static void destruir_lista_vacia(lista_t ** lista, size_t indice)
{
    if (lista_esta_vacia(lista[indice])) {
        lista_destruir(lista[indice], NULL);
        lista[indice] = NULL;
    }
}

/* Busca y devuelve un nodo en la lista que tenga la misma clave que la dada.
 * Si debe_borrar es true, borra el nodo */
static hash_nodo_t *buscar_nodo_lista(lista_t * lista, const char * clave, bool debe_borrar)
{
    /* Si no hay una lista, el elemento no está */
    if (!lista) return NULL;
    lista_iter_t * iter = lista_iter_crear(lista);
    if (!iter) return NULL; // Es un ERROR, no que no lo encontró, causa un comportamiento inadeacuado
    while (!lista_iter_al_final(iter)) {
        hash_nodo_t * actual = lista_iter_ver_actual(iter);
        /* Compara si las claves son iguales */
        if (strcmp(actual->clave, clave) == 0) {
            if (debe_borrar) {
                lista_iter_borrar(iter);
            }
            lista_iter_destruir(iter);
            return actual;
        }
        lista_iter_avanzar(iter);
    }
    lista_iter_destruir(iter);
    return NULL;
}

/* Devuelve el indice a una lista distinta de NULL en la tabla de hash */
static size_t buscar_siguiente_lista_hash(const lista_t **tabla, size_t inicio, size_t final)
{
    size_t i;

    for (i = inicio; i < final; i++) {
        if (tabla[i])
            return i;
    }
    return final;
}

/* Actualiza el iterador de la lista, asumiendo que la posición ya fue actualizada */
static bool actualizar_lista_iter(hash_iter_t * iter)
{
    if (hash_iter_al_final(iter)) {
        iter->lista_iter = NULL;
    } else {
        iter->lista_iter = lista_iter_crear(iter->hash->datos[iter->pos]);
        if (!iter->lista_iter) {
            return false;
        }
    }
    return true;
}

/* Destruye la tabla de listas del hash. Los nodos son destruidos si debe_borrar es true */
static void hash_tabla_listas_destruir(lista_t ** tabla, size_t tam, bool debe_borrar, hash_destruir_dato_t destruir_dato)
{
    size_t i;

    for (i = 0; i < tam; i++) {
        if (tabla[i]) {
            while (!lista_esta_vacia(tabla[i])) {
                if (debe_borrar) {
                    nodo_destruir(lista_borrar_primero(tabla[i]), destruir_dato);
                } else {
                    lista_borrar_primero(tabla[i]); // Puede leakear memoria
                }
            }
            lista_destruir(tabla[i], NULL);
        }
    }
    /* Destruye la tabla al final */
    free(tabla);
}

/* Función de hash:
 * Implementación sencilla de la función de hash de K&R */
static size_t hash_conseguir_indice(const char *clave, size_t tam)
{
    size_t hashval;

    for (hashval = 0; *clave != '\0'; clave++)
        hashval = *clave + 31 * hashval;
	return hashval % tam;
}

/* *****************************************************************
 *           Funciones de redimensionamiento del hash              *
 * *****************************************************************/

/* Chequea la condición para agrandar la tabla de hash */
static bool debe_agrandar(size_t cantidad, size_t tam)
{
    return (cantidad / tam > FACTOR_CARGA_MAX);
}

/* Chequea la condición para achicar la tabla de hash */
static bool debe_achicar(size_t cantidad, size_t tam)
{
    /* No se achica a menos del tamaño inicial */
    return (tam / FACTOR_ACHIQUE >= TAM_INICIAL) && (cantidad / tam < FACTOR_CARGA_MIN);
}

/* Inserta en nueva tabla los elementos de lista */
static bool rehashear_lista(lista_t * lista, lista_t ** nueva_tabla, size_t tam)
{
    hash_nodo_t * nodo_actual;
    lista_iter_t * iter = lista_iter_crear(lista);
    if (!iter) {
        return false;
    }
    while (!lista_iter_al_final(iter)) {
        nodo_actual = lista_iter_ver_actual(iter);
        size_t indice = hash_conseguir_indice(nodo_actual->clave, tam);

        if (!nueva_tabla[indice]) {
            nueva_tabla[indice] = lista_crear();
            if (!nueva_tabla[indice]) {
                lista_iter_destruir(iter);
                return false;
            }
        }
        if (!lista_insertar_primero(nueva_tabla[indice], nodo_actual)) {
            destruir_lista_vacia(nueva_tabla, indice);
            lista_iter_destruir(iter);
            return false;
        }
        lista_iter_avanzar(iter);
    }
    lista_iter_destruir(iter);
    return true;
}

/* Redimensiona la tabla de hash a un tamaño nuevo */
static bool hash_redimensionar(hash_t * hash, size_t tam_nuevo)
{
    size_t i;
    lista_t **nueva_tabla = calloc(tam_nuevo, sizeof(lista_t *));
    if (!nueva_tabla) return false;

    for (i = 0; i < hash->tam; i++) {
        if (hash->datos[i] && !rehashear_lista(hash->datos[i], nueva_tabla, tam_nuevo)) {
            /* Si había algo y falló, hay que destruir todo lo hecho preservando los nodos */
            hash_tabla_listas_destruir(nueva_tabla, tam_nuevo, false, NULL);
            return false;
        }
    }
    /* Destruye la tabla de listas original, los nodos ya están en la otra y no van a ser destruidos */
    hash_tabla_listas_destruir(hash->datos, hash->tam, false, NULL);
    hash->datos = nueva_tabla;
    hash->tam = tam_nuevo;
    return true;
}

/* *****************************************************************
 *                 Primitivas de la tabla de hash                  *
 * *****************************************************************/

hash_t *hash_crear(hash_destruir_dato_t destruir_dato)
{
    hash_t *nuevo = malloc(sizeof(*nuevo));
    lista_t **datos = calloc(TAM_INICIAL, sizeof(*datos));
    if (!nuevo || !datos) {
        free(nuevo);
        free(datos);
        return NULL;
    }
    /* Caso general */
    nuevo->datos = datos;
    nuevo->tam = TAM_INICIAL;
    nuevo->cantidad = 0;
    nuevo->destruir_dato = destruir_dato;
    return nuevo;
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato)
{
    if (!clave) return false; // Debe recibir una clave válida
    size_t indice = hash_conseguir_indice(clave, hash->tam);
    hash_nodo_t *nuevo;

    nuevo = buscar_nodo_lista(hash->datos[indice], clave, false);
    /* Si el elemento existe, hay que reemplazarlo */
    if (nuevo) {
        if (hash->destruir_dato) {
            hash->destruir_dato(nuevo->dato);
        }
        nuevo->dato = dato;
        return true;
    }
    if (!hash->datos[indice]) {
        hash->datos[indice] = lista_crear();
        if (!hash->datos[indice]) {
            return false;
        }
    }
    /* Caso de elemento no repetido */
    nuevo = nodo_crear(clave, dato);
    if (!nuevo) {
        destruir_lista_vacia(hash->datos, indice);
        return false;
    }
    if (!lista_insertar_primero(hash->datos[indice], nuevo)) {
        destruir_lista_vacia(hash->datos, indice);
        nodo_destruir(nuevo, NULL);
        return false;
    }
    ++(hash->cantidad);
    if (debe_agrandar(hash->cantidad, hash->tam)) {
        hash_redimensionar(hash, (hash->tam)*FACTOR_AGRANDAMIENTO);
    }
    return true;
}

void *hash_borrar(hash_t *hash, const char *clave)
{
	size_t indice = hash_conseguir_indice(clave, hash->tam);
    hash_nodo_t * nodo_salida;
    void * dato_salida;

    nodo_salida = buscar_nodo_lista(hash->datos[indice], clave, true);
    if (!nodo_salida)
        return NULL;
    dato_salida = nodo_salida->dato;
    --(hash->cantidad);
    nodo_destruir(nodo_salida, NULL);
    /* Si la lista quedó vacía hay que destruirla */
    destruir_lista_vacia(hash->datos, indice);
    if (debe_achicar(hash->cantidad, hash->tam)) {
        hash_redimensionar(hash, (hash->tam)/FACTOR_ACHIQUE);
    }
    return dato_salida;
}

void *hash_obtener(const hash_t *hash, const char *clave)
{
	size_t indice = hash_conseguir_indice(clave, hash->tam);
    hash_nodo_t * nodo_salida;

    nodo_salida = buscar_nodo_lista(hash->datos[indice], clave, false);
    if (!nodo_salida)
        return NULL;
    else
        return nodo_salida->dato;
}

bool hash_pertenece(const hash_t *hash, const char *clave)
{
    size_t indice = hash_conseguir_indice(clave, hash->tam);
    if (buscar_nodo_lista(hash->datos[indice], clave, false))
        return true;
    else
        return false;
}

size_t hash_cantidad(const hash_t *hash)
{
    return hash->cantidad;
}

void hash_destruir(hash_t *hash)
{
    hash_tabla_listas_destruir(hash->datos, hash->tam, true, hash->destruir_dato);
    free(hash);
}

/* *****************************************************************
 *                 Primitivas del iterador de hash                 *
 * *****************************************************************/

hash_iter_t *hash_iter_crear(const hash_t *hash)
{
	hash_iter_t* iter = malloc(sizeof(*iter));
	if (!iter)
	    return NULL;
	iter->hash = hash;
    /* Hay que buscar una lista distinta de NULL y crear un iter para ella */
    iter->pos = buscar_siguiente_lista_hash((const lista_t **) hash->datos, 0, hash->tam);
    if (!actualizar_lista_iter(iter)) {
        free(iter);
        return NULL;
    }
	return iter;
}

bool hash_iter_avanzar(hash_iter_t *iter)
{
	if (hash_iter_al_final(iter))
	    return false;
    lista_iter_avanzar(iter->lista_iter);
	if (lista_iter_al_final(iter->lista_iter)) {
        lista_iter_destruir(iter->lista_iter);
        /* Se actualiza la posición con la siguiente lista válida */
        iter->pos = buscar_siguiente_lista_hash((const lista_t **) iter->hash->datos, iter->pos + 1, iter->hash->tam);
        if (!actualizar_lista_iter(iter))
            return false;
    }
    return true;
}

const char *hash_iter_ver_actual(const hash_iter_t *iter)
{
    hash_nodo_t * nodo_actual;
	if (hash_iter_al_final(iter))
		return NULL;
    /* La clave está dentro del nodo de hash dentro del nodo de la lista apuntado por lista_iter */
    /* lista_iter va a apuntar a algo válido, por lo que no se va a desreferenciar NULL */
    nodo_actual = lista_iter_ver_actual(iter->lista_iter);
    return nodo_actual->clave;
}

bool hash_iter_al_final(const hash_iter_t *iter)
{
    return (iter->pos == iter->hash->tam);
}

void hash_iter_destruir(hash_iter_t *iter)
{
	if (iter->lista_iter)
	    lista_iter_destruir(iter->lista_iter);
	free(iter);
}
