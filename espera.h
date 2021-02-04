#ifndef ESP_H
#define ESP_H

#include <stdbool.h>
#include "abb.h"
#include "cola.h"

typedef struct esp esp_t;

// tipo de funcion para comparar clave
typedef int (*esp_comparar_clave_t) (const char *, const char *);

// tipo de función para destruir dato
typedef void (*esp_destruir_dato_t) (void *);

/*
Crea una espera.
Post: devuelve una nueva espera vacia.
*/
esp_t* espera_crear(esp_comparar_clave_t cmp, esp_destruir_dato_t destruir_dato);

/*
Devuelve True si la espera esta vacia, False en caso contrario.
Pre: La espera fue creada.
*/
bool espera_esta_vacia(esp_t* esp);

/*
Devuelve la cantidad de elementos en espera.
Pre: la espera fue creada.
*/
size_t cantidad_espera(esp_t* esp);

/*
Devuelve True si se puede guardar el elemento de manera urgente
en la espera, False en caso contrario.
Pre: la espera fue creada.
*/
bool guardar_urgente(esp_t* esp, void *valor);

/*
Devuelve True si se puede guardar el elemento de manera regular
en la espera, False en caso contrario.
Pre: la espera fue creada.
*/
bool guardar_regular(esp_t* esp, const char *clave, void *dato);

/*
Devuelve el siguiente elemento en espera.
Pre: Hay algun elemento en espera.
*/
void* obtener_siguiente(esp_t* esp);

/*
Destruye la estructura liberando la memoria pedida y llamando a la 
funcion destruir para cada elemento.
Pre: la espera fue creada.
Post: la espera fue destruida.
*/
void espera_destruir(esp_t* esp);

#endif