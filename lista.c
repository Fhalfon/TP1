/* *****************************************************************
 *                    FACUNDO HALFON 95452
 *                  AYUDANTE: JAVIER CHOQUE
 * *****************************************************************/

#include "lista.h"
#include <stdlib.h>
#include <stdio.h>

/* Definición del struct lista */
typedef struct nodo_lista{
	void* dato;
	struct nodo_lista* siguiente;
} nodo_lista_t;


typedef struct lista{
	nodo_lista_t* primero;
	nodo_lista_t* ultimo;
	size_t largo;
} lista_t;
/* Definicio del struct iterador */

typedef struct lista_iter{
	nodo_lista_t* actual;
	nodo_lista_t* anterior;
	lista_t* lista;
}lista_iter_t;

/* *****************************************************************
 *                    PRIMITIVAS DE LA LISTA
 * *****************************************************************/
 
lista_t* lista_crear(void){
	lista_t* lista=malloc(sizeof(lista_t));
	if (lista==NULL)
		return NULL;
	lista->primero=NULL;
	lista->ultimo=NULL;
	lista->largo=0;
	return lista;
}
	

bool lista_esta_vacia(const lista_t *lista){
	if (lista->primero==NULL)
	    return true;
	else
	    return false;
}

bool lista_insertar_primero(lista_t *lista, void *dato){
	nodo_lista_t* nodo=malloc(sizeof(nodo_lista_t));
	if (nodo==NULL)
	    return false;
	nodo->dato=dato;
	nodo->siguiente=lista->primero;
	if (lista_esta_vacia(lista)){
	    lista->primero=nodo;
	    lista->ultimo=nodo;
	}
	else
	    lista->primero=nodo;
	lista->largo++;
	return true;
}

bool lista_insertar_ultimo(lista_t *lista, void *dato){
	nodo_lista_t* nodo=malloc(sizeof(nodo_lista_t));
	if (nodo==NULL)
	    return false;
	nodo->dato=dato;
	nodo->siguiente=NULL;
	if (lista_esta_vacia(lista)){
		lista->primero=nodo;
	    lista->ultimo=nodo;
	}
	else{
	    lista->ultimo->siguiente=nodo;
		lista->ultimo=nodo;
	}
	lista->largo++;
	return true;
}

void *lista_borrar_primero(lista_t *lista){
	if (lista_esta_vacia(lista))
	    return NULL;
	nodo_lista_t* nodo=lista->primero;
	lista->primero=lista->primero->siguiente;
	if (lista->largo==1)
	    lista->ultimo=NULL;
	void* valor=nodo->dato;
	lista->largo--;
	free(nodo);
	return valor;
}

void *lista_ver_primero(const lista_t *lista){
	if (lista_esta_vacia(lista))
	    return NULL;
	else
	    return lista->primero->dato;
}

void *lista_ver_ultimo(const lista_t* lista){
	if (lista_esta_vacia(lista))
	    return NULL;
	else
	    return lista->ultimo->dato;
}

size_t lista_largo(const lista_t *lista){
	return lista->largo;
}

void lista_destruir(lista_t *lista, void destruir_dato(void *)){
	while(!lista_esta_vacia(lista)){
	    if (destruir_dato!=NULL){
	         destruir_dato(lista_borrar_primero(lista));
	    }
	    else
	        lista_borrar_primero(lista);
	}
	free(lista);
}

lista_iter_t *lista_iter_crear(lista_t *lista){
	lista_iter_t* iter=malloc(sizeof(lista_iter_t));
	if (iter==NULL)
	    return NULL;
    iter->actual=lista->primero;
    iter->anterior=NULL;
    iter->lista=lista;
    return iter;
}

bool lista_iter_avanzar(lista_iter_t *iter){
	if (lista_iter_al_final(iter))
	    return false;   
	else{
	    iter->anterior=iter->actual;
	    iter->actual=iter->actual->siguiente;
	}
	return true;
}
    
void *lista_iter_ver_actual(const lista_iter_t *iter){
	if(iter->actual==NULL)
		return NULL;
	else
	    return iter->actual->dato;	
}

bool lista_iter_al_final(const lista_iter_t *iter){
	if(iter->actual==NULL)
	    return true;
	else
	    return false;
}

void lista_iter_destruir(lista_iter_t *iter){
	free(iter);
}

bool lista_iter_insertar(lista_iter_t *iter, void *dato){
	nodo_lista_t* nodo=malloc(sizeof(nodo_lista_t));
	if(nodo==NULL)
	    return false;
	nodo->dato=dato; 
	nodo->siguiente=iter->actual;
	if(iter->anterior==NULL){
	   if (lista_esta_vacia(iter->lista)){
	        iter->lista->primero=nodo;
	        iter->lista->ultimo=nodo;
	   }
	   else
	       iter->lista->primero=nodo;
	   iter->lista->largo++;
	   iter->anterior=NULL;
	   iter->actual=nodo;
	   return true;
	}
	else if(iter->actual==NULL){
		if (lista_esta_vacia(iter->lista)){
		    iter->lista->primero=nodo;
	        iter->lista->ultimo=nodo;
	    }
	    else{
	        iter->lista->ultimo->siguiente=nodo;
		    iter->lista->ultimo=nodo;
	    }
	    iter->lista->largo++;
		iter->anterior->siguiente=nodo;
		iter->actual=nodo;
		return true;
	}
    else{ 
		nodo_lista_t* aux=iter->lista->primero;
		while(aux->siguiente!=iter->actual){
			aux=aux->siguiente;
		}
		nodo->siguiente=aux->siguiente;
		aux->siguiente=nodo;
	    iter->actual=nodo;
	    iter->lista->largo++;
	    return true;
    }
    free(nodo);
    return false;
}    
	
void *lista_iter_borrar(lista_iter_t *iter){
	void* dato;
	if(iter->actual==NULL){
	    return NULL;
	}  
	nodo_lista_t* nodo=iter->actual;
	dato=nodo->dato; 
	if(iter->anterior==NULL){
		iter->anterior=NULL;
		iter->actual=iter->actual->siguiente;	
		iter->lista->primero=iter->actual;
		iter->lista->largo--;
		free(nodo);
		return dato;
	}
	if(iter->actual->siguiente==NULL){
	    iter->actual=iter->actual->siguiente=NULL;
	    iter->lista->ultimo=iter->anterior;
	}
	else{
		iter->actual=iter->actual->siguiente;
	}
	iter->anterior->siguiente=iter->actual;	   
	iter->lista->largo--;
	free(nodo);
	return dato;
}
	
void lista_iterar(lista_t *lista, bool (*visitar)(void *dato, void *extra), void *extra){
	lista_iter_t* iter=lista_iter_crear(lista);
	while(!lista_iter_al_final(iter)){
		 if(visitar(iter->actual->dato,extra)){
		    lista_iter_avanzar(iter);
		 }
		 else
		    iter->actual=NULL;
	}
	free(iter);
}
	
