#ifndef ABB_H
#define ABB_H

#include <stdbool.h>
#include <stddef.h>

typedef struct abb abb_t;
typedef struct abb_iter abb_iter_t;

// tipo de funcion para comparar clave
typedef int (*abb_comparar_clave_t) (const char *, const char *);

// tipo de función para destruir dato
typedef void (*abb_destruir_dato_t) (void *);

// Crea el abb
abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato);

/*
Guarda un elemento en el abb, si la clave ya se encuentra en la estructura,
la remplaza. De no poder guardarla devuelve false.
Pre: La estructura fue creada.
Post: Se almaceno el par clave, dato
*/
bool abb_guardar(abb_t *arbol, const char *clave, void *dato);

/*
Borra un elemento del abb y devuelve el dato asociado, si el dato
no estaba devuelve NULL.
Pre: la estructura abb fue inicializada.
Post: el elemento fue borrado de la estructura y se devuelve el dato, en 
el caso de que estuviera guardado.
*/
void *abb_borrar(abb_t *arbol, const char *clave);

/*
Obtiene el dato de un elemento del abb, si la clave no se encuentra 
debuelve NULL.
Pre: el abb fue inicializado.
*/
void *abb_obtener(const abb_t *arbol, const char *clave);

/*
Determina si la clave pertenece o no al abb.
Pre: el abb fue inicializado.
*/
bool abb_pertenece(const abb_t *arbol, const char *clave);

/*
Devuelve la cantidad de elementos del abb.
Pre: el abb fue inicializado.
*/
size_t abb_cantidad(abb_t *arbol);

/*
Destruye la estructura liberando la memoria pedida y llamando a la 
funcion destruir para cada par clave, dato.
Pre: el abb fue inicializado.
Post: el abb fue destruido.
*/
void abb_destruir(abb_t *arbol);

// ITERADOR INTERNO DEL ABB

/*
Recorre el abb. Si se recibe la funcion visitar por parametro,
para cada uno de los elementos del abb llama a visitar.
Pre: el abb fue creado. visitar es una funcion capaz de modificar
cada valor y aplicarle un extra el cual recibe la funcion visitar.
*/
void abb_in_order(abb_t *arbol, bool visitar(const char *, void *, void *), void *extra);


// ITERADOR EXTERNO DEL ABB

// Crea el iterador.
abb_iter_t *abb_iter_in_crear(const abb_t *arbol);

// Avanza el iterador.
bool abb_iter_in_avanzar(abb_iter_t *iter);

// Devuelve la clave del actual, dicha clave no se puede modificar ni liberar.
const char *abb_iter_in_ver_actual(const abb_iter_t *iter);

// Comprueba si termino la iteracion.
bool abb_iter_in_al_final(const abb_iter_t *iter);

// Destryue el iterador
void abb_iter_in_destruir(abb_iter_t* iter);

//pruebas
void pruebas_abb_estudiante(void);
#endif  // ABB_H