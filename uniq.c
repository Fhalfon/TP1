#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hash.h"
#include "strutil.h"


int main ( int argc , char *argv[] ) {
	/* Verifico que el programa recibe un unico parametro de invocacion */
	if (argc!=2) {
		return -1;
	}
	/* Abro el archivo en modo lectura */
	FILE* arch;
	arch = fopen(argv[1],"r");
	if (!arch) {
		 printf("No existe el archivo\n");
		 return -1;
	}
	
    hash_t * hash = hash_crear(NULL);
    if (!hash) {
		return -1;
	}
	/*Guardo la primera linea del archivo*/
	char* linea = NULL;
	size_t capacidad = 0;
	ssize_t longitud; 
	longitud = getline(&linea, &capacidad, arch);
	if(longitud!=-1) {
        linea[longitud-1] = 0;
    }
    /*Separo la linea en un arreglo dinamico terminado en NULL, pasando ' ' como separador*/
    char ** arreglo = split(linea,' ');
    int i;
    while (!feof(arch)) {
		for (i=0 ; arreglo[i] ; i++){
			size_t * cont = malloc(sizeof(size_t));
			* cont = 1;
		    if (!hash_pertenece(hash,arreglo[i])) {
			    hash_guardar(hash,arreglo[i],cont);
		    }
		    else {
			    cont = hash_obtener(hash,arreglo[i]);
			    ++*cont;
			    hash_guardar(hash,arreglo[i],cont);
		    }
		}
		longitud = getline(&linea, &capacidad, arch);
		if(longitud!=-1) {
            linea[longitud-1] = 0;
        }
        arreglo = split(linea,' ');
	}
	/* Creo un iterador para recorrer el hash */
	hash_iter_t* iter = hash_iter_crear(hash);
	const char * palabra;
	while (!hash_iter_al_final(iter)) {
		palabra = hash_iter_ver_actual(iter);
		size_t * cont = (size_t*) hash_obtener(hash,palabra);
		printf("%d %s\n",*cont,palabra);
		hash_iter_avanzar(iter);
	}
	hash_iter_destruir(iter);
    hash_destruir(hash);
    free(linea);
    free_strv(arreglo);
    return 0;
}		
			
    
