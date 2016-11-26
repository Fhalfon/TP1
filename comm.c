#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "hash.h"
#include "cola.h"
#include "strutil.h"

// Funcion auxiliar que imprime las palabras de los archivos.
// Se le pasa un booleano que en caso de ser True imprime las lineas comunes a los dos archivos.
// Si es False imprime las lineas que son unicas para un archivo.
static void imprimir_palabras(cola_t * cola, hash_t * hash,bool imprimir_ambos) 
{
	while(!cola_esta_vacia(cola)){
		char * palabra = cola_desencolar(cola);
		if (imprimir_ambos) {
		    if (hash_pertenece(hash,palabra)){
			    printf("%s\n",palabra);
			}
		}
		else { 
		    if (!hash_pertenece(hash,palabra)){
			    printf("%s\n",palabra);
			}	
		}
		free(palabra);
	}
}
// Funcion auxiliar que guardar las palabras del archivo en un hash y en una cola.
static void guardar_en_hash(char * linea,hash_t * hash,cola_t * cola)
{
	char ** palabra = split(linea,' ');
	    if (!hash_pertenece(hash,palabra[0])) {
	        hash_guardar(hash,palabra[0],NULL);
		cola_encolar(cola,palabra[0]);
		}
	    else
		free(palabra[0]);
	free(palabra);
}
	

int main ( int argc , char *argv[] ) {
	/* Abro los archivos en modo lectura */
	FILE* arch1;
	arch1 = fopen(argv[1],"r");
	FILE* arch2;
	arch2 = fopen(argv[2],"r");
	
	char* linea1 = NULL;
	size_t capacidad1 = 0;
	ssize_t longitud1 = 0;
	char* linea2 = NULL;
	ssize_t longitud2 = 0;
	
	hash_t * hash1 = hash_crear(NULL);
	hash_t * hash2 = hash_crear(NULL);
	cola_t * cola1 = cola_crear();
	cola_t * cola2 = cola_crear();
	
    while(!feof(arch1)) {
	    longitud1=getline(&linea1, &capacidad1, arch1);
	    if (longitud1!=-1) {
	        linea1[longitud1-1] = 0;
	        guardar_en_hash(linea1,hash1,cola1);
	    }    
	}
	
	while(!feof(arch2)) {
	    longitud2=getline(&linea2, &capacidad1, arch2);
	    if (longitud2!=-1) {
	        linea2[longitud2-1] = 0;
	        guardar_en_hash(linea2,hash2,cola2);
	    }    
	}
	/* Utilizo un booleano para ver como se imprime las palabras */
	bool imprimir_ambos;
	if (argc == 3) {
		imprimir_ambos = true;
		imprimir_palabras(cola1,hash2,imprimir_ambos);
	}
	else {
		imprimir_ambos = false;
		if (strcmp(argv[3],"-1") == 0) {
		    imprimir_palabras(cola1,hash2,imprimir_ambos);
		}
	        else if (strcmp(argv[3],"-2") == 0) {
		    imprimir_palabras(cola2,hash1,imprimir_ambos);
	    }	
	}	
	free(linea1);
	free(linea2);
	cola_destruir(cola1,free);
	cola_destruir(cola2,free);
	hash_destruir(hash1);
	hash_destruir(hash2);
	fclose(arch1);
	fclose(arch2);
	return 0;
}
