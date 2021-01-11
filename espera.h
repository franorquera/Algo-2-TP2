#ifndef ESP_H
#define ESP_H

#include <stdbool.h>
#include "abb.h"
#include "cola.h"

typedef struct esp esp_t;
typedef int (*esp_comparar_clave_t) (const char *, const char *);
typedef void (*esp_destruir_dato_t) (void *);

esp_t* espera_crear(esp_comparar_clave_t cmp, esp_destruir_dato_t destruir_dato);

bool espera_esta_vacia(esp_t* esp);

size_t cantidad_espera(esp_t* esp);
/*
size_t cantidad_urgencia(esp_t* esp);

size_t cantidad_regular(esp_t* esp);
*/
bool guardar_urgente(esp_t* esp, void *valor);

bool guardar_regular(esp_t* esp, const char *clave, void *dato);

/*
Pre: Hay algun elemento en espera
*/
void* obtener_siguiente(esp_t* esp);
/*
void* obtener_urgente(esp_t* esp);

void* obtener_regular(esp_t* esp, const char *clave);

void* retirar_urgente(esp_t* esp);

void* retirar_regular(esp_t* esp, const char *clave);
*/
void espera_destruir(esp_t* esp);

#endif