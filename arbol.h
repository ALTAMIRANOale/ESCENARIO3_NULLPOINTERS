#ifndef ARBOL_H
#define ARBOL_H

#include "tli.h"

/*
=================================================
ARBOL DE TELEMETRIA

Almacena muestras de la trayectoria LEO - L1
ordenadas por distancia.

Incluye un ABB simple y un AVL real para comparar
el costo de busqueda entre un arbol degenerado
O(n) y uno balanceado O(log n).
=================================================
*/

/* Resultado de un conjunto de busquedas */
typedef struct
{
    int totalComparaciones;
    int maxComparaciones;
    int encontrados;
    double promedioComparaciones;

} EstadisticasBusqueda;

/* Crea un nodo a partir de una muestra */
NodoArbol *arbol_crear_muestra(
    const MuestraTelemetria *muestra
);

/* Inserta una muestra en un ABB simple */
NodoArbol *arbol_insertar_abb(
    NodoArbol *raiz,
    const MuestraTelemetria *muestra
);

/* Inserta una muestra en un AVL balanceado */
NodoArbol *arbol_insertar_avl(
    NodoArbol *raiz,
    const MuestraTelemetria *muestra
);

/* Busca una distancia y cuenta las comparaciones */
NodoArbol *arbol_buscar(
    NodoArbol *raiz,
    double clave,
    int *comparaciones
);

/* Busca la muestra cuya distancia sea la mas cercana */
NodoArbol *arbol_buscar_mas_cercano(
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

/* Verificaciones estructurales completas */
int arbol_verificar_abb(const NodoArbol *raiz);
int arbol_verificar_avl(const NodoArbol *raiz);

/* Acumula metricas de busqueda sobre un arreglo de muestras */
void arbol_estadisticas_busqueda(
    NodoArbol *raiz,
    const MuestraTelemetria muestras[],
    int cantidad,
    EstadisticasBusqueda *estadisticas
);

/* Recorrido inorden y salida por pantalla */
void arbol_inorden(const NodoArbol *raiz);
void arbol_mostrar_telemetria(const NodoArbol *raiz);

/* Libera toda la memoria del arbol */
void arbol_liberar(NodoArbol *raiz);

#endif
