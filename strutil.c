#define _POSIX_C_SOURCE 200809L
#include "strutil.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char** split(const char* str, char sep){
	char* palabra;
	char** arreglo;
	char* string=strdup(str);
	char separador[2];
	separador[1]='\0';
	separador[0]=sep;
	int cont=0;
	palabra=(char*)strtok(string,separador);
	arreglo=(char**)malloc(1*sizeof(char*));
	if(!arreglo) return NULL;
	arreglo[cont]=NULL;
	int j=0;
	while(str[j]==sep){
		arreglo[cont]=strdup("");
		cont++;
		arreglo=(char**)realloc(arreglo,sizeof(char*)* (cont + 1));
		j++;
	}
	size_t pos=j;
	if (palabra==NULL && str[0]!=sep){
		arreglo[cont]=strdup("");
		cont++;
		arreglo=(char**)realloc(arreglo,sizeof(char*)* (cont + 1));
	}	
	if(palabra!=NULL)pos=pos+strlen(palabra);
	while(palabra!=NULL){
		arreglo[cont]=(char*)strdup(palabra);
		cont++;
		arreglo=(char**)realloc(arreglo,sizeof(char*)* (cont + 1));
		while(str[pos]==sep && str[pos+1]==sep){
			arreglo[cont]=strdup("");
			cont++;
		    arreglo=(char**)realloc(arreglo,sizeof(char*)* (cont + 1));
            pos++;
        }
        palabra=(char*)strtok(NULL,separador);
        if(palabra!=NULL)pos=pos+strlen(palabra)+1;
    }
    if (str[strlen(str)-1]==sep){
		arreglo[cont]=strdup("");
		cont++;
		arreglo=(char**)realloc(arreglo,sizeof(char*)* (cont + 1));
	}
    arreglo[cont]=NULL;
    free(string);
    return arreglo;
}
	
	
char* join(char** strv, char sep){
	char* cadena;
	char separador[2];
	separador[1]='\0';
	separador[0]=sep;
	int i=1;
	if(strv[0]==NULL){
	   cadena=strdup("");
	   return cadena;
	}
	cadena=realloc(NULL,strlen(strv[0])+1);	
	strcpy(cadena,strv[0]);
	while(strv[i]!=NULL){
		cadena=(char*)realloc(cadena,strlen(cadena)+strlen(strv[i])+strlen(separador)+1);
		strcat(cadena,separador);
		strcat(cadena,strv[i]);
		i++;
	}
	return cadena;	
}		


void free_strv(char* strv[]){
	int i=0;
	for(i=0;strv[i]!=NULL;i++)
        free(strv[i]);
	free(strv);
}
		
