#include "cola.h"
#include <stdlib.h>

typedef struct nodo {
    void* dato;
    struct nodo* prox;
} nodo_t;

struct cola {
    nodo_t* prim;
    nodo_t* ult;
};

nodo_t *nodo_crear(void* valor) {
    nodo_t *nodo = malloc(sizeof(nodo_t));
    if (!nodo) return NULL;
    nodo->dato = valor;
    nodo->prox = NULL;
    return nodo;
}

cola_t *cola_crear(void) {
    cola_t *cola = malloc(sizeof(cola_t));
    if (!cola) return NULL;
    cola->prim = NULL;
    cola->ult = NULL;
    return cola;
}

bool cola_esta_vacia(const cola_t *cola) {
    return (!cola->prim && !cola->ult);
}

bool cola_encolar(cola_t *cola, void *valor) {
    void *elem = nodo_crear(valor);
    if (!elem) return false;
    if (!cola->prim) cola->prim = elem;
    else cola->ult->prox = elem;
    cola->ult = elem;
    return true;
}

void *cola_ver_primero(const cola_t *cola) {
    if (cola_esta_vacia(cola)) return NULL;
    return cola->prim->dato;
}

void *cola_desencolar(cola_t *cola) {
    if (cola_esta_vacia(cola)) return NULL;
    void* valor = cola->prim->dato;
    nodo_t* nuevo_prim = cola->prim->prox;
    free(cola->prim);
    cola->prim = nuevo_prim;
    if (!cola->prim) cola->ult = NULL;
    return valor;
}

void cola_destruir(cola_t *cola, void (*destruir_dato)(void *)) {
    while (cola->prim) {
        void* dato = cola_desencolar(cola);
        if(destruir_dato) destruir_dato(dato);
    }
    free(cola);
}