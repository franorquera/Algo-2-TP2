#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "espera.h"
#include "hash.h"
#include "strutil.h"
#include "csv.h"
#include "mensajes.h"

#define COMANDO_PEDIR_TURNO "PEDIR_TURNO"
#define COMANDO_ATENDER "ATENDER_SIGUIENTE"
#define COMANDO_INFORME "INFORME"
#define URGENTE "URGENTE"
#define REGULAR "REGULAR"

void destruir_espera(void* esp) {
	espera_destruir((esp_t*) esp);
}

bool comprobar_errores_pedir_turno(char** parametros, hash_t* pacientes, hash_t* especialidades){
	bool todo_ok = true;
	if(!hash_pertenece(pacientes, parametros[0])){
		printf(ENOENT_PACIENTE, parametros[0]);
		todo_ok = false;
	}
	if(!hash_pertenece(especialidades, parametros[1])){
		printf(ENOENT_ESPECIALIDAD, parametros[1]);
		todo_ok = false;
	}
	if(strcmp(parametros[2],URGENTE) != 0 && strcmp(parametros[2],REGULAR) != 0){
		printf(ENOENT_URGENCIA, parametros[2]);
		todo_ok = false;
	}
	return todo_ok;
}

bool comprobar_errores_atender(char** parametros, hash_t* doctores){
	bool todo_ok = true;
	if(!hash_pertenece(doctores, parametros[0])){
		printf(ENOENT_DOCTOR, parametros[0]);
		todo_ok = false;
	}
	return todo_ok;
}

void agendar_turno_urgente(char* nombre, char* especialidad, hash_t* especialidades) {
	esp_t* espera = hash_obtener(especialidades, especialidad);
	char* nombre_copia = strdup(nombre);
	guardar_urgente(espera, nombre_copia);
	printf(PACIENTE_ENCOLADO, nombre);
	printf(CANT_PACIENTES_ENCOLADOS, cantidad_espera(espera), especialidad);
}

void agendar_turno_regular(char* nombre, char* especialidad, hash_t* pacientes, hash_t* especialidades){
	esp_t* espera = hash_obtener(especialidades, especialidad);
	char* nombre_copia = strdup(nombre);
	guardar_regular(espera, hash_obtener(pacientes, nombre), nombre_copia);
	printf(PACIENTE_ENCOLADO, nombre);
	printf(CANT_PACIENTES_ENCOLADOS, cantidad_espera(espera), especialidad);

}

void atender_paciente(hash_t* especialidades, char* especialidad){
	esp_t* espera = hash_obtener(especialidades, especialidad);
	if(espera_esta_vacia(espera)){
		printf(SIN_PACIENTES);
	}else{
		void* paciente = obtener_siguiente(espera);
		printf(PACIENTE_ATENDIDO, (char*)paciente);
	}
	
}

void procesar_comando(const char* comando, char** parametros, hash_t* doctores, hash_t* pacientes, hash_t* especialidades) {
	if (strcmp(comando, COMANDO_PEDIR_TURNO) == 0) {
		if(comprobar_errores_pedir_turno(parametros, pacientes, especialidades)){
			if (strcmp(parametros[2], URGENTE) == 0) agendar_turno_urgente(parametros[0], parametros[1], especialidades);
			else agendar_turno_regular(parametros[0], parametros[1], pacientes, especialidades);
		}
		
	
	} else if (strcmp(comando, COMANDO_ATENDER) == 0) {
		if(comprobar_errores_atender(parametros, doctores)){
			char* especialidad = hash_obtener(doctores, parametros[0]);
			atender_paciente(especialidades, especialidad);
		}


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

void procesar_entrada(hash_t* doctores, hash_t* pacientes, hash_t* especialidades) {
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
		procesar_comando(campos[0], parametros, doctores, pacientes, especialidades);
		free_strv(parametros);
		free_strv(campos);
	}

	free(linea);
}

int main(int argc, char** argv) {
	hash_t* doctores = csv_crear_estructura(argv[1]);
	hash_t* pacientes = csv_crear_estructura(argv[2]);
    hash_t* especialidades = hash_crear(destruir_espera);

	hash_iter_t* iter_doctores = hash_iter_crear(doctores);
    
	while (!hash_iter_al_final(iter_doctores)) {
		const char* clave = hash_iter_ver_actual(iter_doctores);
		char* dato = hash_obtener(doctores, clave);
		esp_t* espera = espera_crear(strcmp, free);
		hash_guardar(especialidades, dato, espera);
		hash_iter_avanzar(iter_doctores);
	}
	
    procesar_entrada(doctores, pacientes, especialidades);

	hash_iter_destruir(iter_doctores);
	hash_destruir(especialidades);
	hash_destruir(doctores);
	hash_destruir(pacientes);
	
    return 0;
}