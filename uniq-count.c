#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hash.h"
#include "strutil.h"
#include "cola.h"

 

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
    /* Creo una cola para guardar las palabras que aparecen en el texto*/
    cola_t * cola = cola_crear();
    int i;
    size_t * cont;
    while (!feof(arch)) {
		for (i=0 ; arreglo[i] ; i++){
			/* Si la palabra no se encuentra en el hash, la guardo y la tambien la encolo*/
		    if (!hash_pertenece(hash,arreglo[i])) {
				cont = malloc(sizeof(size_t));
				* cont = 1;
			    hash_guardar(hash,arreglo[i],cont);
			    cola_encolar(cola,arreglo[i]);
		    }
		    /* Si ya esta en el hash, obtengo el cont ,lo incremento y vuelvo a guardar */
		    else {
			    cont = hash_obtener(hash,arreglo[i]);
			    ++*cont;
			    hash_guardar(hash,arreglo[i],cont);
			    free(arreglo[i]);
		    }
		}
		free(arreglo);
		longitud = getline(&linea, &capacidad, arch);
		if(longitud!=-1) {
                    linea[longitud-1] = 0;
                    arreglo = split(linea,' ');
                }
	}
	/* Desencolo la palabra y la busco en el hash para obtener el cont */
	while(!cola_esta_vacia(cola)){
		char * palabra = cola_desencolar(cola);
		cont = hash_obtener(hash,palabra);
		printf("%d %s\n",*cont,palabra);
		free(cont);
		free(palabra);
	}
	cola_destruir(cola,NULL);
    hash_destruir(hash);
    free(linea);
    fclose(arch);
    return 0;
}
