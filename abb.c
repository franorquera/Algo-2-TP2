#include "abb.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"
#include "stdio.h"
#include "cola.h"
#include "hash.h"

typedef struct nodo_abb {
    struct nodo_abb *izq;
    struct nodo_abb *der;
    const char *clave;
    void *dato;
} nodo_abb_t;

struct abb {
    nodo_abb_t* raiz;
    size_t cant;
    abb_comparar_clave_t cmp;
    abb_destruir_dato_t destruir;
};

struct abb_iter{
  const abb_t* abb;
  cola_t* cola;
  size_t cant;
};


abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato) {
    abb_t* abb = malloc(sizeof(abb_t));

    if (!abb) return NULL;

    abb->raiz = NULL;
    abb->cant = 0;
    abb->cmp = cmp;
    abb->destruir = destruir_dato;

    return abb;
}

nodo_abb_t* nodo_abb_crear(const char *clave, void *dato) {
    nodo_abb_t* nodo_abb = malloc(sizeof(nodo_abb_t));

    if (!nodo_abb) return NULL;

    nodo_abb->clave = clave;
    nodo_abb->dato = dato;
    nodo_abb->der = NULL;
    nodo_abb->izq = NULL;

    return nodo_abb;

}

void _abb_guardar(abb_comparar_clave_t cmp ,nodo_abb_t *nodo_act, nodo_abb_t *nodo_nuevo) {
    if (!nodo_act) return;

    if (cmp(nodo_act->clave, nodo_nuevo->clave) < 0) {
        if (nodo_act->der) _abb_guardar(cmp, nodo_act->der, nodo_nuevo);
        else nodo_act->der = nodo_nuevo;
    }
    else {
        if (nodo_act->izq) _abb_guardar(cmp, nodo_act->izq, nodo_nuevo);
        else nodo_act->izq = nodo_nuevo;
    }
}

bool _abb_modificar_valor(nodo_abb_t *nodo_act, const char *clave, void *dato, abb_comparar_clave_t cmp, abb_destruir_dato_t destruir){
    if(cmp(nodo_act->clave, clave) == 0){
        if(destruir) destruir(nodo_act->dato);
        nodo_act->dato = dato;
        return true;
    }

    if(cmp(nodo_act->clave, clave) < 0){
        return _abb_modificar_valor(nodo_act->der,clave, dato, cmp, destruir);
    }
    return _abb_modificar_valor(nodo_act->izq,clave, dato, cmp, destruir);

}

bool abb_guardar(abb_t *arbol, const char *clave, void *dato) {

    if(abb_pertenece(arbol, clave)) return _abb_modificar_valor(arbol->raiz, clave, dato, arbol->cmp, arbol->destruir);

    const char* copia_clave = strdup(clave);
    
    if (!copia_clave) return false;

    nodo_abb_t* nodo_nuevo = nodo_abb_crear(copia_clave, dato);

    if (!arbol->raiz) arbol->raiz = nodo_nuevo;
    else _abb_guardar(arbol->cmp , arbol->raiz, nodo_nuevo);
    
    arbol->cant++;
    return true;
}

void* _abb_obtener (const nodo_abb_t *nodo_act, const char *clave, abb_comparar_clave_t cmp) {
    if (!nodo_act) return NULL;

    if (cmp(nodo_act->clave, clave) == 0) return nodo_act->dato;

    if (cmp(nodo_act->clave, clave) < 0) {
        return _abb_obtener(nodo_act->der, clave, cmp);
    }
    return _abb_obtener(nodo_act->izq, clave, cmp);
}

void *abb_obtener(const abb_t *arbol, const char *clave) {
    return _abb_obtener(arbol->raiz, clave, arbol->cmp);
}

size_t abb_cantidad(abb_t *arbol) {
    return arbol->cant;
}

bool _abb_pertenece (const nodo_abb_t *nodo_act, const char *clave, abb_comparar_clave_t cmp) {
    if (!nodo_act) return false;

    if (cmp(nodo_act->clave, clave) == 0) return true;

    if (cmp(nodo_act->clave, clave) < 0) return _abb_pertenece(nodo_act->der, clave, cmp);
    
    return _abb_pertenece(nodo_act->izq, clave, cmp);
}

bool abb_pertenece(const abb_t *arbol, const char *clave) {
    return _abb_pertenece(arbol->raiz, clave, arbol->cmp);
}

nodo_abb_t* _abb_buscar_candidato(nodo_abb_t *nodo_act) {
    if (!nodo_act->izq) return nodo_act;
    return _abb_buscar_candidato(nodo_act->izq);
}

void* _abb_borrar(abb_t *arbol, nodo_abb_t *nodo_padre, nodo_abb_t *nodo_hijo, const char *clave, abb_comparar_clave_t cmp) {
    if (!nodo_hijo) return NULL;

    if (cmp(nodo_hijo->clave, clave) == 0) {
        if (!nodo_hijo->izq && !nodo_hijo->der) {
            
            if (nodo_padre && nodo_padre->izq && cmp(nodo_padre->izq->clave, nodo_hijo->clave) == 0) nodo_padre->izq = NULL;
            
            else if (nodo_padre && nodo_padre->der && cmp(nodo_padre->der->clave, nodo_hijo->clave) == 0) nodo_padre->der = NULL;
            
            else if (!nodo_padre) arbol->raiz = NULL;
            
            void* valor_hijo = nodo_hijo->dato;
            free((char*) nodo_hijo->clave);
            free(nodo_hijo);
            return valor_hijo;
            
        }

        else if ((!nodo_hijo->izq && nodo_hijo->der) || (!nodo_hijo->der && nodo_hijo->izq)) {
            nodo_abb_t* seleccionado;

            if(nodo_hijo->izq) seleccionado = nodo_hijo->izq;
            else seleccionado = nodo_hijo->der;

            if (nodo_padre && nodo_padre->izq && cmp(nodo_padre->izq->clave, nodo_hijo->clave) == 0) nodo_padre->izq = seleccionado;
                
            else if (nodo_padre && nodo_padre->der && cmp(nodo_padre->der->clave, nodo_hijo->clave) == 0) nodo_padre->der = seleccionado;
                
            else if (!nodo_padre) arbol->raiz = seleccionado;

            
            void* valor_hijo = nodo_hijo->dato;
            free((char*) nodo_hijo->clave);
            free(nodo_hijo);
            return valor_hijo;
        }

        else if (nodo_hijo->izq && nodo_hijo->der) {
            nodo_abb_t* nodo_remplazante = _abb_buscar_candidato(nodo_hijo->der);
            const char* nodo_remplazante_clave = nodo_remplazante->clave;
            const char* copia_clave = strdup(nodo_remplazante_clave);


            void* nodo_remplazante_dato = _abb_borrar(arbol, NULL, arbol->raiz, nodo_remplazante_clave, cmp);
            free((char*) nodo_hijo->clave);

            void* valor_hijo = nodo_hijo->dato;
            nodo_hijo->clave = copia_clave;
            nodo_hijo->dato = nodo_remplazante_dato;
            return valor_hijo;
        }

    }
    
    else if (cmp(nodo_hijo->clave, clave) < 0) return _abb_borrar(arbol, nodo_hijo ,nodo_hijo->der, clave, cmp);
    return _abb_borrar(arbol, nodo_hijo, nodo_hijo->izq, clave, cmp);
}

void *abb_borrar(abb_t *arbol, const char *clave) {
    if (!abb_pertenece(arbol, clave)) return NULL;
    
    arbol->cant--;
    return _abb_borrar(arbol ,NULL, arbol->raiz, clave, arbol->cmp);
}

void _abb_destruir(nodo_abb_t *nodo_act, abb_destruir_dato_t destruir) {
    if (!nodo_act) return;

    _abb_destruir(nodo_act->izq, destruir);
    _abb_destruir(nodo_act->der, destruir);
    free( (char*) nodo_act->clave);

    if (destruir) destruir(nodo_act->dato);

    free(nodo_act);
    
}

void abb_destruir(abb_t *arbol) {
    _abb_destruir(arbol->raiz, arbol->destruir);
    free(arbol);
}

// ITERADOR INTERNO

void _abb_in_order(nodo_abb_t *nodo_act, bool visitar(const char *, void *, void *), void *extra, bool* seguir) {
    if (!nodo_act) return;

    _abb_in_order(nodo_act->izq, visitar, extra, seguir);
    if(visitar && *seguir) *seguir = visitar(nodo_act->clave, nodo_act->dato, extra);
    if(!seguir) return;
    _abb_in_order(nodo_act->der, visitar, extra, seguir);
}

void abb_in_order(abb_t *arbol, bool visitar(const char *, void *, void *), void *extra) {
    bool seguir = true;
    _abb_in_order(arbol->raiz, visitar, extra, &seguir);
}



// ITERADOR EXTERNO
void llenar_iterador_rango(cola_t *cola, nodo_abb_t* nodo_act, abb_comparar_clave_t cmp,size_t* cant, char *inicio, bool* encontrado, hash_t* hash_aux){
    if(!nodo_act) return;

    if (!(*encontrado) && cmp(inicio, nodo_act->clave) > 0)llenar_iterador_rango(cola,nodo_act->der, cmp, cant, inicio, encontrado, hash_aux);
    if (!(*encontrado) && cmp(inicio, nodo_act->clave) < 0)llenar_iterador_rango(cola,nodo_act->izq, cmp, cant, inicio, encontrado, hash_aux);
    if (!(*encontrado) && cmp(inicio, nodo_act->clave) == 0){
        cola_encolar(cola, (char*)nodo_act->clave);
        *cant +=1;
        hash_guardar(hash_aux, (char*)nodo_act->clave, NULL);
        *encontrado = true;
        llenar_iterador_rango(cola,nodo_act->der, cmp, cant, inicio,encontrado, hash_aux);
        return;
    }

    if((*encontrado)){
        // si ya esta en el hash o es mas chico que inicio, return
        if(hash_pertenece(hash_aux,(char*)nodo_act->clave) || cmp(inicio, nodo_act->clave)>0) return;
        llenar_iterador_rango(cola,nodo_act->izq, cmp, cant, inicio, encontrado, hash_aux);
        cola_encolar(cola, (char*)nodo_act->clave);
        *cant +=1;
        hash_guardar(hash_aux, (char*)nodo_act->clave, NULL);
        llenar_iterador_rango(cola,nodo_act->der, cmp, cant, inicio, encontrado, hash_aux);
    }

    
}

void llenar_iterador(cola_t *cola, nodo_abb_t* nodo_act, abb_comparar_clave_t cmp, size_t* cant){
    if(!nodo_act) return;    
    llenar_iterador(cola,nodo_act->izq, cmp, cant);
    cola_encolar(cola, (char*)nodo_act->clave);
    *cant +=1;
    llenar_iterador(cola,nodo_act->der, cmp, cant);
}

void _abb_buscar_inicio(const abb_t *arbol, nodo_abb_t *nodo_act, char* clave_inicio, nodo_abb_t** auxiliar, nodo_abb_t** candidato) {
    if(!nodo_act) return;

    if (arbol->cmp(nodo_act->clave,clave_inicio) == 0){
        *candidato = nodo_act;
        return;
    }

    if (arbol->cmp(nodo_act->clave,clave_inicio) > 0){
        _abb_buscar_inicio(arbol,nodo_act->izq,clave_inicio, auxiliar, candidato);
    }else{
        _abb_buscar_inicio(arbol,nodo_act->der,clave_inicio, auxiliar,candidato);
    }
    if(!(*candidato)){
        if(!(*auxiliar) && arbol->cmp(nodo_act->clave,clave_inicio)>0){
            *auxiliar = nodo_act;
        }else if((*auxiliar) && arbol->cmp(nodo_act->clave,(*auxiliar)->clave)<0 && arbol->cmp(nodo_act->clave,clave_inicio)>0){
            *auxiliar = nodo_act;
        }
    }
}

abb_iter_t *abb_iter_in_crear(const abb_t *arbol, char* inicio, char* final){
    abb_iter_t *abb_iter = malloc(sizeof(abb_iter_t));
    if(!abb_iter) return NULL;

    abb_iter->cola = cola_crear();
    size_t cantidad = 0;
    if(inicio){
        nodo_abb_t* candidato = NULL;
        nodo_abb_t* auxiliar = NULL;
        _abb_buscar_inicio(arbol, arbol->raiz, inicio, &auxiliar, &candidato);
        if(auxiliar) inicio = (char*)auxiliar->clave;
        if(!auxiliar && !candidato) return abb_iter;
        bool encontrado = false;
        hash_t* hash_aux = hash_crear(NULL);
        llenar_iterador_rango(abb_iter->cola, arbol->raiz, arbol->cmp,&cantidad, inicio, &encontrado, hash_aux);
        hash_destruir(hash_aux);
    }else llenar_iterador(abb_iter->cola, arbol->raiz, arbol->cmp, &cantidad);
    abb_iter->cant = cantidad;
    return abb_iter;
}

size_t abb_iter_in_cant(abb_iter_t *iter){
    return iter->cant;
}

bool abb_iter_in_avanzar(abb_iter_t *iter){
    if(abb_iter_in_al_final(iter)) return false;
    cola_desencolar(iter->cola);
    return true;
}

const char *abb_iter_in_ver_actual(const abb_iter_t *iter){
    if(abb_iter_in_al_final(iter)) return NULL;
    const char* clave = cola_ver_primero(iter->cola);
    return clave;
}

bool abb_iter_in_al_final(const abb_iter_t *iter){
    return cola_esta_vacia(iter->cola);
}

void abb_iter_in_destruir(abb_iter_t* iter){
    cola_destruir(iter->cola,NULL);
    free(iter);
}