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

int comparacion_int(const char* a, const char* b){
	int res;
	char* comparar_a = strdup(a);
	char* comparar_b = strdup(b);
	int numero_uno = atoi(comparar_a);
	int numero_dos = atoi(comparar_b);
	if(numero_uno>numero_dos) res = 1;
	else if(numero_dos>numero_uno) res = -1;
	else res = 0;
	free(comparar_a);
	free(comparar_b);
	return res;
}

bool comprobar_errores_pedir_turno(char** parametros, hash_t* pacientes, hash_t* especialidades, int cantidad_parametros){
	bool todo_ok = true;
	if(cantidad_parametros != 3){
		printf(ENOENT_PARAMS, COMANDO_PEDIR_TURNO);
		return false;
	}

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

bool comprobar_errores_atender(char** parametros, hash_t* doctores, int cantidad_parametros){
	bool todo_ok = true;
	if(cantidad_parametros != 1){
		printf(ENOENT_PARAMS, COMANDO_ATENDER);
		return false;
	} 

	if(!hash_pertenece(doctores, parametros[0])){
		printf(ENOENT_DOCTOR, parametros[0]);
		todo_ok = false;
	}

	return todo_ok;
}

bool comprobar_errores_informe(char** parametros, hash_t* doctores, int cantidad_parametros){
	bool todo_ok = true;
	if(cantidad_parametros != 2){
		printf(ENOENT_PARAMS, COMANDO_INFORME);
		return false;
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
	guardar_regular(espera, (char*)hash_obtener(pacientes, nombre), nombre_copia);
	printf(PACIENTE_ENCOLADO, nombre);
	printf(CANT_PACIENTES_ENCOLADOS, cantidad_espera(espera), especialidad);
}

bool atender_paciente(hash_t* especialidades, char* especialidad){
	bool todo_ok = true;
	esp_t* espera = hash_obtener(especialidades, especialidad);
	if(espera_esta_vacia(espera)){
		printf(SIN_PACIENTES);
		todo_ok = false;
	}else{
		void* paciente = obtener_siguiente(espera);
		printf(PACIENTE_ATENDIDO, (char*)paciente);
		free(paciente);
	}
	return todo_ok;
}

void pedir_planilla(char* inicio, char* final, abb_t* doctores_atendidos, hash_t* doctores) {
	if(strcmp(inicio,"") == 0) inicio = NULL;
	if(strcmp(final,"") == 0) final = NULL;
	abb_iter_t* iter = abb_iter_in_crear(doctores_atendidos, inicio, final);
	size_t incremento = 1;

	printf(DOCTORES_SISTEMA, abb_iter_in_cant(iter));

	while (!abb_iter_in_al_final(iter)) {

		const char* doctor = abb_iter_in_ver_actual(iter);
		int cant_pacientes = *(int*) abb_obtener(doctores_atendidos, doctor);
		char* especialidad = (char*) hash_obtener(doctores, doctor);

		printf(INFORME_DOCTOR, incremento, doctor, especialidad, cant_pacientes);

		incremento++;

		abb_iter_in_avanzar(iter);
	}

	abb_iter_in_destruir(iter);
}

void procesar_comando(const char* comando, char** parametros, hash_t* doctores, hash_t* pacientes, hash_t* especialidades, abb_t* doctores_atendidos, int cantidad_parametros) {
	
	if (strcmp(comando, COMANDO_PEDIR_TURNO) == 0) {
		if(comprobar_errores_pedir_turno(parametros, pacientes, especialidades, cantidad_parametros)){
			if (strcmp(parametros[2], URGENTE) == 0) agendar_turno_urgente(parametros[0], parametros[1], especialidades);
			else agendar_turno_regular(parametros[0], parametros[1], pacientes, especialidades);
		}
		
	
	} else if (strcmp(comando, COMANDO_ATENDER) == 0) {
		if(comprobar_errores_atender(parametros, doctores, cantidad_parametros)){
			char* especialidad = hash_obtener(doctores, parametros[0]);
			if(atender_paciente(especialidades, especialidad)){
				int* cant_pacientes_atendidos = abb_obtener(doctores_atendidos, parametros[0]);
				int* nueva_cantidad_paciente = malloc(sizeof(int)); 
				*cant_pacientes_atendidos += 1;
				*nueva_cantidad_paciente = *cant_pacientes_atendidos;
				abb_guardar(doctores_atendidos, parametros[0], nueva_cantidad_paciente);
			}
		}


	} else if (strcmp(comando, COMANDO_INFORME) == 0) {
		if(comprobar_errores_informe(parametros,doctores, cantidad_parametros)){
			pedir_planilla(parametros[0], parametros[1],doctores_atendidos, doctores);
		}
	
	} else {
		printf(ENOENT_CMD, comando);
	}
}

void eliminar_fin_linea(char* linea) {
	size_t len = strlen(linea);
	if (linea[len - 1] == '\n') {
		linea[len - 1] = '\0';
	}
}
// PEDIR_TURNO:PARAMETROS
void procesar_entrada(hash_t* doctores, hash_t* pacientes, hash_t* especialidades, abb_t* doctores_atendidos) {
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
		int cantidad_campos = 0;
		int cantidad_parametros = 0;
		for(int i = 0; campos[i] != NULL; i++){
			cantidad_campos++;
		}
		for(int i = 0; parametros[i] != NULL; i++){
			cantidad_parametros++;
		}
		if(cantidad_campos != 2) printf(ENOENT_FORMATO, linea);
		else procesar_comando(campos[0], parametros, doctores, pacientes, especialidades, doctores_atendidos, cantidad_parametros);
		free_strv(parametros);
		free_strv(campos);
	}

	free(linea);
}

int main(int argc, char** argv) {
	hash_t* doctores = csv_crear_estructura(argv[1]);
	hash_t* pacientes = csv_crear_estructura(argv[2]);
    hash_t* especialidades = hash_crear(destruir_espera);
	abb_t* doctores_atendidos = abb_crear(strcmp, free);

	hash_iter_t* iter_doctores = hash_iter_crear(doctores);
    
	while (!hash_iter_al_final(iter_doctores)) {
		const char* clave = hash_iter_ver_actual(iter_doctores);
		int* pacientes_atendidos_inicialmente = malloc(sizeof(int));
		*pacientes_atendidos_inicialmente = 0;
		abb_guardar(doctores_atendidos, clave, pacientes_atendidos_inicialmente);
		char* dato = hash_obtener(doctores, clave);
		esp_t* espera = espera_crear(comparacion_int, free);
		hash_guardar(especialidades, dato, espera);
		hash_iter_avanzar(iter_doctores);
	}
	
    procesar_entrada(doctores, pacientes, especialidades, doctores_atendidos);

	hash_iter_destruir(iter_doctores);
	hash_destruir(especialidades);
	hash_destruir(doctores);
	hash_destruir(pacientes);
	abb_destruir(doctores_atendidos);
	
    return 0;
}


/*

"800" 800*/