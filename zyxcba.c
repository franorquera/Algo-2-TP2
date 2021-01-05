#include <stdio.h>
#include <string.h>
//#include "funciones_tp2.h"
#include "strutil.h"
#include "heap.h"
#include "cola.h"
#include "hash.h"
#include "mensajes.h"

#define COMANDO_PEDIR_TURNO "PEDIR_TURNO"
#define COMANDO_ATENDER "ATENDER_SIGUIENTE"
#define COMANDO_INFORME "INFORME"

void strcmp_min();

void procesar_comando(const char* comando, const char** parametros) {
	if (strcmp(comando, COMANDO_PEDIR_TURNO) == 0) {

	} else if (strcmp(comando, COMANDO_ATENDER) == 0) {

	} else if (strcmp(comando, COMANDO_INFORME) == 0) {

	} else {

	}
}

void eliminar_fin_linea(char* linea) {
	size_t len = strlen(linea);
	if (linea[len - 1] == '\n') {
		linea[len - 1] = '\0';
	}
}

void procesar_entrada() {
	char* linea = NULL;
	size_t c = 0;
	while (getline(&linea, &c, stdin) > 0) {
		eliminar_fin_linea(linea);
		char** campos = split(linea, ':');
		if (campos[1] == NULL) {
			printf(ENOENT_FORMATO, linea);
			free_strv(campos);
			continue;	
		}
		char** parametros = split(campos[1], ',');
		procesar_comando(campos[0], parametros);
		free_strv(parametros);
		free_strv(campos);
	}
	free(linea);
}

// PEDIR_TURNO  [NOMBRE_PACIENTE,NOMBRE_ESPECIALIDAD,URGENCIA]

int main(int argc, char** argv) {
	hash_t* doctores = csv_crear_estructura(argv[1]);
	hash_t*  pacientes = csv_crear_estructura(argv[2]);
	hash_iter_t* iter_doctores = hash_iter_crear(doctores);
	hash_t* especialidades_regular = hash_crear(NULL);
	hash_t* especialidades_urgencia = hash_crear(NULL);
	while (!hash_iter_al_final(iter_doctores)) {
		char* clave = hash_iter_ver_actual(iter_doctores);
		char* dato = (char*) hash_obtener(doctores, clave);
		hash_guardar(especialidades_regular, dato, heap_crear(strcmp_min));
		hash_guardar(especialidades_urgencia, dato, cola_crear());
	}
	procesar_entrada();
	return 0;
}
