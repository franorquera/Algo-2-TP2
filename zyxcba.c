#include <stdio.h>
#include <string.h>
//#include "funciones_tp2.h"
#include "strutil.h"
#include "heap.h"
#include "cola.h"
#include "hash.h"
#include "mensajes.h"
#include "csv.h"
#include <stdlib.h>


#define COMANDO_PEDIR_TURNO "PEDIR_TURNO"
#define COMANDO_ATENDER "ATENDER_SIGUIENTE"
#define COMANDO_INFORME "INFORME"
#define URGENTE "URGENTE"
#define REGULAR "REGULAR"

// source:  https://stackoverflow.com/questions/4497680/c-strcmp-source-code/4497723
int strcmp_min(const char *s1, const char *s2) {
  int ret = 0;

  while (!(ret = *(unsigned char *) s1 - *(unsigned char *) s2) && *s2) s1++, s2++;

  if (ret > 0) ret = -1;
  else if (ret < 0) ret = 1 ;

  return ret;
}

int charcmp(const void* a, const void* b){
    char* a_aux = (char*)a;
    char* b_aux = (char*)b;
    return strcmp_min(a_aux, b_aux);
}

void agendar_turno_urgente(char* nombre, char* especialidad, hash_t* pacientes, hash_t* especialidades_urgencia) {
	//printf("%s \n", especialidad);
	hash_iter_t* iter = hash_iter_crear(especialidades_urgencia);
	while (!hash_iter_al_final(iter)) {
		const char* clave = hash_iter_ver_actual(iter);
		printf("%s\n", clave);
		hash_iter_avanzar(iter);
	}
	void* cola_urgencia = hash_obtener(especialidades_urgencia, especialidad);
	//printf("%p \n", cola_urgencia);
	cola_encolar(cola_urgencia, nombre);
	printf(PACIENTE_ENCOLADO, nombre);
}

//void agendar_turno_regular(char* nombre, char* especialidad, hash_t* pacientes, hash_t* especialidades_regular);

void procesar_comando(const char* comando, char** parametros, hash_t* doctores, hash_t* pacientes, hash_t* especialidades_regular, hash_t* especialidades_urgencia) {
	if (strcmp(comando, COMANDO_PEDIR_TURNO) == 0) {
		
		if (strcmp(parametros[2], URGENTE) == 0) agendar_turno_urgente(parametros[0], parametros[1], pacientes, especialidades_urgencia);
		
		//else agendar_turno_regular(parametros[0], parametros[1], pacientes, especialidades_regular);
	
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

void procesar_entrada(hash_t* doctores, hash_t* pacientes, hash_t* especialidades_regular, hash_t* especialidades_urgencia) {
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
		procesar_comando(campos[0], parametros, doctores, pacientes, especialidades_regular, especialidades_urgencia);
		free_strv(parametros);
		free_strv(campos);
	}
	free(linea);
}

// PEDIR_TURNO  [NOMBRE_PACIENTE,NOMBRE_ESPECIALIDAD,URGENCIA]

int main(int argc, char** argv) {
	hash_t* doctores = csv_crear_estructura(argv[1]);
	hash_t*  pacientes = csv_crear_estructura(argv[2]);
	//printf("%s \n", hash_pertenece(doctores, "orquera") ? "true" : "false");
	hash_iter_t* iter_doctores = hash_iter_crear(doctores);
	hash_t* especialidades_regular = hash_crear(NULL);
	hash_t* especialidades_urgencia = hash_crear(NULL);
	while (!hash_iter_al_final(iter_doctores)) {
		const char* clave = hash_iter_ver_actual(iter_doctores);
		char* dato = hash_obtener(doctores, clave);
		//printf("%s\n", dato);
		cola_t* cola = cola_crear();
		//printf("%s \n", hash_pertenece(especialidades_urgencia, dato) ? "true" : "false");
		hash_guardar(especialidades_regular, dato, heap_crear(charcmp));
		hash_guardar(especialidades_urgencia, dato, cola);
		hash_iter_avanzar(iter_doctores);
	}
	
	hash_iter_t* iter = hash_iter_crear(especialidades_urgencia);
	while (!hash_iter_al_final(iter)) {
		const char* clave = hash_iter_ver_actual(iter);
		printf("%s\n", clave);
		hash_iter_avanzar(iter);
	}

	procesar_entrada(doctores, pacientes, especialidades_regular, especialidades_urgencia);
	/*
	hash_iter_t* iter_doctores_verificar = hash_iter_crear(doctores);
	hash_iter_t* iter_pacientes_verificar = hash_iter_crear(pacientes);
	while (!hash_iter_al_final(iter_doctores_verificar)) {
		const char* clave = hash_iter_ver_actual(iter_doctores_verificar);
		char* dato = (char*) hash_obtener(doctores, clave);
		printf(" %s %s \n", clave, dato);
		hash_iter_avanzar(iter_doctores_verificar);
	}

	while (!hash_iter_al_final(iter_pacientes_verificar)) {
		const char* clave = hash_iter_ver_actual(iter_pacientes_verificar);
		char* dato = (char*) hash_obtener(pacientes, clave);
		printf(" %s %s \n", clave, dato);
		hash_iter_avanzar(iter_pacientes_verificar);
	}
	*/
	return 0;
}