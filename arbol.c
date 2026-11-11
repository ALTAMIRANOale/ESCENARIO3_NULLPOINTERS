#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arbol.h"

/*=========================================
  UTILIDADES INTERNAS
=========================================*/

static int arbol_maximo(int a, int b)
{
    return (a > b) ? a : b;
}

static void arbol_actualizar_altura(NodoArbol *nodo)
{
    if (nodo != NULL)
    {
        nodo->altura =
            1 + arbol_maximo(
                    arbol_altura(nodo->izq),
                    arbol_altura(nodo->der)
                );
    }
}

/*
 * Inserta un nodo ya creado en un ABB.
 * No reserva memoria adicional.
 */
static NodoArbol *arbol_insertar_nodo_abb(
    NodoArbol *raiz,
    NodoArbol *nuevo
)
{
    if (nuevo == NULL)
    {
        return raiz;
    }

    if (raiz == NULL)
    {
        return nuevo;
    }

    if (nuevo->valor < raiz->valor)
    {
        raiz->izq = arbol_insertar_nodo_abb(raiz->izq, nuevo);
    }
    else
    {
        raiz->der = arbol_insertar_nodo_abb(raiz->der, nuevo);
    }

    arbol_actualizar_altura(raiz);

    return raiz;
}

/*=========================================
  OPERACIONES BASICAS
=========================================*/

int arbol_altura(const NodoArbol *nodo)
{
    if (nodo == NULL)
    {
        return 0;
    }

    return nodo->altura;
}

int arbol_factor_balance(const NodoArbol *nodo)
{
    if (nodo == NULL)
    {
        return 0;
    }

    return arbol_altura(nodo->izq) - arbol_altura(nodo->der);
}

int arbol_contar_nodos(const NodoArbol *raiz)
{
    if (raiz == NULL)
    {
        return 0;
    }

    return 1 + arbol_contar_nodos(raiz->izq) +
           arbol_contar_nodos(raiz->der);
}

int arbol_es_avl(const NodoArbol *raiz)
{
    int factor;

    if (raiz == NULL)
    {
        return 1;
    }

    factor = arbol_factor_balance(raiz);

    if (factor < -1 || factor > 1)
    {
        return 0;
    }

    return arbol_es_avl(raiz->izq) && arbol_es_avl(raiz->der);
}

/*=========================================
  CREACION E INSERCION
=========================================*/

NodoArbol *arbol_crear_muestra(
    const MuestraTelemetria *muestra
)
{
    NodoArbol *nodo;

    nodo = (NodoArbol *) malloc(sizeof(NodoArbol));

    if (nodo == NULL)
    {
        return NULL;
    }

    nodo->izq = NULL;
    nodo->der = NULL;
    nodo->altura = 1;

    if (muestra != NULL)
    {
        nodo->muestra = *muestra;
    }
    else
    {
        memset(&nodo->muestra, 0, sizeof(MuestraTelemetria));
    }

    nodo->valor = nodo->muestra.distancia;

    snprintf(
        nodo->nombre,
        MAX_NOMBRE,
        "r=%.0f",
        nodo->muestra.distancia
    );

    return nodo;
}

NodoArbol *arbol_insertar_abb(
    NodoArbol *raiz,
    const MuestraTelemetria *muestra
)
{
    if (raiz == NULL)
    {
        return arbol_crear_muestra(muestra);
    }

    if (muestra->distancia < raiz->muestra.distancia)
    {
        raiz->izq = arbol_insertar_abb(raiz->izq, muestra);
    }
    else
    {
        raiz->der = arbol_insertar_abb(raiz->der, muestra);
    }

    arbol_actualizar_altura(raiz);

    return raiz;
}

/*=========================================
  BUSQUEDA
=========================================*/

NodoArbol *arbol_buscar(
    NodoArbol *raiz,
    double clave,
    int *comparaciones
)
{
    NodoArbol *actual;
    int contador;

    actual = raiz;
    contador = 0;

    while (actual != NULL)
    {
        contador++;

        if (clave == actual->muestra.distancia)
        {
            break;
        }

        if (clave < actual->muestra.distancia)
        {
            actual = actual->izq;
        }
        else
        {
            actual = actual->der;
        }
    }

    if (comparaciones != NULL)
    {
        *comparaciones = contador;
    }

    return actual;
}

/*=========================================
  RECORRIDOS
=========================================*/

void arbol_inorden(const NodoArbol *raiz)
{
    if (raiz == NULL)
    {
        return;
    }

    arbol_inorden(raiz->izq);

    printf(
        "  r = %14.1f m | v = %9.2f m/s | "
        "K = %10.4e J | U = %10.4e J | E = %10.4e J\n",
        raiz->muestra.distancia,
        raiz->muestra.velocidad,
        raiz->muestra.energiaCinetica,
        raiz->muestra.energiaPotencial,
        raiz->muestra.energiaTotal
    );

    arbol_inorden(raiz->der);
}

void arbol_mostrar_telemetria(const NodoArbol *raiz)
{
    printf("\n========== ARBOL DE TELEMETRIA ==========\n");

    if (raiz == NULL)
    {
        printf("\nNo hay muestras cargadas.\n");
        printf("Ejecute primero la opcion 4 (Punto 3).\n");
        printf("\n=========================================\n");
        return;
    }

    printf("\nNodos: %d | Altura: %d | AVL: %s\n\n",
           arbol_contar_nodos(raiz),
           arbol_altura(raiz),
           arbol_es_avl(raiz) ? "si" : "no");

    arbol_inorden(raiz);

    printf("\n=========================================\n");
}

/*=========================================
  LIBERACION
=========================================*/

void arbol_liberar(NodoArbol *raiz)
{
    if (raiz == NULL)
    {
        return;
    }

    arbol_liberar(raiz->izq);
    arbol_liberar(raiz->der);

    free(raiz);
}

/*=========================================
  COMPATIBILIDAD CON tli.h
=========================================*/

NodoArbol* crearNodoArbol(
    const char *nombre,
    double valor
)
{
    MuestraTelemetria muestra;

    memset(&muestra, 0, sizeof(MuestraTelemetria));
    muestra.distancia = valor;

    NodoArbol *nodo = arbol_crear_muestra(&muestra);

    if (nodo != NULL && nombre != NULL)
    {
        snprintf(nodo->nombre, MAX_NOMBRE, "%s", nombre);
    }

    return nodo;
}

NodoArbol* insertarArbol(
    NodoArbol *raiz,
    const char *nombre,
    double valor
)
{
    NodoArbol *nuevo;

    nuevo = crearNodoArbol(nombre, valor);

    if (nuevo == NULL)
    {
        return raiz;
    }

    return arbol_insertar_nodo_abb(raiz, nuevo);
}

void evaluarArbol(NodoArbol *raiz)
{
    if (raiz == NULL)
    {
        return;
    }

    arbol_inorden(raiz);
}

void liberarArbol(NodoArbol *raiz)
{
    arbol_liberar(raiz);
}
