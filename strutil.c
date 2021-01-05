#define _POSIX_C_SOURCE 200809L
#include "strutil.h"
#include <string.h>
#include <stdlib.h>
#include "stdio.h"

char *substr(const char *str, size_t n) {
    return strndup(str, n);
}

char **split(const char *str, char sep) {
    int cant_sep = 0, pos_cad = 0, pos_letra = 0;
    size_t len = strlen(str);
    
    if (!len) len = 1;
    
    for (int i = 0; i < len; i++) if (str[i] == sep) cant_sep++;
   
    char** arreglo = calloc(len + cant_sep + 1, sizeof(char*));
    
    if (!arreglo) return NULL;
    
    for (int i = 0; i < len; i++) {
        if (str[i] == sep) {
            arreglo[pos_cad] = substr(str + pos_letra, i - pos_letra);
            pos_cad++;
            pos_letra = i + 1;
        }
        if (i == len - 1) arreglo[pos_cad] = substr(str + pos_letra, i + 1);
    }
    return arreglo;
}

void calcular_largo(char **strv, size_t* len, size_t* cant_p) {
    for (int i = 0; strv[i] != NULL; i++) {
        *len += strlen(strv[i]);
        *cant_p += 1;
    }
}

char *join(char **strv, char sep) {
    size_t len = 0; size_t cant_p = 0;

    calcular_largo(strv, &len, &cant_p);

    if (!len) len = 1;

    char* palabra = calloc(cant_p + len, sizeof(char));

    if (!palabra) return NULL;

    char* puntero_final;

    for (int i = 0; strv[i] != NULL; i++) {
        if (i == 0) puntero_final = stpcpy(palabra, strv[i]);
        else puntero_final = stpcpy(puntero_final, strv[i]);
    
        if(sep == '\0') continue;

        if (cant_p == 1) break;

        *puntero_final = sep;
        puntero_final++;
        cant_p--;
    }
    return palabra;
}

void free_strv(char *strv[]) {
    for (int i = 0; strv[i] != NULL; i++) {
        free(strv[i]);
    }
    free(strv);
}