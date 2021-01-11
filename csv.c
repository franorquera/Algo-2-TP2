#define _POSIX_C_SOURCE 200809L //getline
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "csv.h"
#define SEPARADOR ','

static void eliminar_fin_linea(char* linea) {
	size_t len = strlen(linea);
	if (linea[len - 1] == '\n') {
		linea[len - 1] = '\0';
	}
}

static void eliminar_cartridge_return(char* linea) {
	size_t len = strlen(linea);
	if (linea[len - 1] == '\r') {
		linea[len - 1] = '\0';
	}
}

hash_t* csv_crear_estructura(const char* ruta_csv) {
	FILE* archivo = fopen(ruta_csv, "r");
	if (!archivo) {
		return NULL;
	}
	
	hash_t* hash = hash_crear(free);
	if (!hash) {
		fclose(archivo);
		return NULL;
	}

	char* linea = NULL;
	size_t c = 0;
	while (getline(&linea, &c, archivo) > 0) {
		eliminar_fin_linea(linea);
		char** campos = split(linea, SEPARADOR);
		eliminar_cartridge_return(campos[1]);
		void* copia_dato = strdup(campos[1]);
		hash_guardar(hash, campos[0], copia_dato);
		free_strv(campos);
	}
	free(linea);
	fclose(archivo);
	return hash;
}
