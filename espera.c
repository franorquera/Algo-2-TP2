#include "espera.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"
#include "cola.h"
#include "abb.h"
#include <stdio.h>

struct esp {
    cola_t* cola_urgente;
    abb_t* abb_regular;
    size_t cant;
    esp_comparar_clave_t cmp;
    esp_destruir_dato_t destruir;
};

void destruir_cola(void* cola) {
	cola_destruir((cola_t*) cola, free); // Cuidado con el free
}

esp_t* espera_crear(esp_comparar_clave_t cmp, esp_destruir_dato_t destruir_dato) {
    esp_t* esp = malloc(sizeof(esp_t));

    if (!esp) return NULL;

    esp->destruir = destruir_dato;
    esp->cmp = cmp;
    esp->cola_urgente = cola_crear();
    
    if (!esp->cola_urgente) {
        free(esp);
        return NULL;
    }
    
    esp->abb_regular = abb_crear(esp->cmp, destruir_cola);

    if (!esp->abb_regular) {
        free(esp);
        free(esp->cola_urgente);
        return NULL;
    }

    esp->cant = 0;
    return esp;
}


bool espera_esta_vacia(esp_t* esp){
    return (esp->cant == 0);
}

size_t cantidad_espera(esp_t* esp) {
    return esp->cant;
}
/*
size_t cantidad_urgencia(esp_t* esp) {
    return cola_tam(esp->cola_urgente);
}

size_t cantidad_regular(esp_t* esp) {
    return abb_cantidad(esp->abb_regular);
}
*/
bool guardar_urgente(esp_t* esp, void *valor) {
    bool todo_ok = cola_encolar(esp->cola_urgente, valor);
    if (todo_ok) esp->cant++;
    return todo_ok;
}

bool guardar_regular(esp_t* esp, const char *clave, void *dato) {
    bool todo_ok;
    if (!abb_pertenece(esp->abb_regular, clave)) {
        cola_t* cola = cola_crear();
        todo_ok = abb_guardar(esp->abb_regular, clave, cola);
    }
    cola_t* cola_obtenida = abb_obtener(esp->abb_regular, clave);
    todo_ok = cola_encolar(cola_obtenida, dato);
    if (todo_ok) esp->cant++;
    return todo_ok;
    // Reminder: cuando se vacie la cola borrar el anio y destruir la cola
}
/*
void* obtener_urgente(esp_t* esp) {
    return cola_ver_primero(esp->cola_urgente);
}
void* obtener_regular(esp_t* esp, const char *clave) {
    return abb_obtener(esp->abb_regular, clave);
}
*/
void* obtener_siguiente(esp_t* esp) {
    if (!cola_esta_vacia(esp->cola_urgente)) {
        esp->cant--;
        return cola_desencolar(esp->cola_urgente);
    }

    abb_iter_t* iter = abb_iter_in_crear(esp->abb_regular,NULL);
    const char* primero = abb_iter_in_ver_actual(iter); //anio
    cola_t* cola_regular = abb_obtener(esp->abb_regular, primero);
    void* siguiente = cola_desencolar(cola_regular);

    if(cola_esta_vacia(cola_regular)){
        cola_destruir(cola_regular, esp->destruir);
        abb_borrar(esp->abb_regular, primero);
    }
    abb_iter_in_destruir(iter);
    esp->cant--;
    return siguiente;
}
/*
void* retirar_urgente(esp_t* esp) {
    void* elem = cola_desencolar(esp->cola_urgente);
    if (elem) esp->cant--;
    return elem;
}

void* retirar_regular(esp_t* esp, const char *clave) {
    void* elem = abb_borrar(esp->abb_regular, clave);
    if (elem) esp->cant--;
    return elem;
}*/

void espera_destruir(esp_t* esp) {
    // Crear iterador del abb para destruir las colas del guardar_regular
    /*
    abb_iter_t* iter = abb_iter_in_crear(esp->abb_regular);

    while(!abb_iter_in_al_final(iter)) {
        const char* primero = abb_iter_in_ver_actual(iter);
        cola_t* cola_regular = abb_obtener(esp->abb_regular, primero);
        cola_destruir(cola_regular, esp->destruir);
        abb_iter_in_avanzar(iter);
    }

    abb_iter_in_destruir(iter);
    */
    cola_destruir(esp->cola_urgente, esp->destruir);
    abb_destruir(esp->abb_regular);
    free(esp);
}