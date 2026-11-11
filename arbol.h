#ifndef ARBOL_H
#define ARBOL_H

#include "tli.h"

/*
=================================================
ARBOL DE TELEMETRIA

Almacena muestras de la trayectoria LEO - L1
ordenadas por distancia.

Permite comparar el costo de busqueda entre
un ABB desbalanceado y un AVL balanceado.
=================================================
*/

/* Crea un nodo a partir de una muestra */
NodoArbol *arbol_crear_muestra(
    const MuestraTelemetria *muestra
);

/* Inserta una muestra en un ABB simple */
NodoArbol *arbol_insertar_abb(
    NodoArbol *raiz,
    const MuestraTelemetria *muestra
);

/* Busca una distancia y cuenta las comparaciones */
NodoArbol *arbol_buscar(
    NodoArbol *raiz,
    double clave,
    int *comparaciones
);

/* Altura y factor de balance de un nodo */
int arbol_altura(const NodoArbol *nodo);
int arbol_factor_balance(const NodoArbol *nodo);

/* Cantidad total de nodos */
int arbol_contar_nodos(const NodoArbol *raiz);

/* Verifica la condicion de equilibrio AVL */
int arbol_es_avl(const NodoArbol *raiz);

/* Recorrido inorden y salida por pantalla */
void arbol_inorden(const NodoArbol *raiz);
void arbol_mostrar_telemetria(const NodoArbol *raiz);

/* Libera toda la memoria del arbol */
void arbol_liberar(NodoArbol *raiz);

#endif
