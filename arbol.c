#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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
  ROTACIONES Y BALANCEO AVL
=========================================*/

static NodoArbol *arbol_rotar_derecha(NodoArbol *y)
{
    NodoArbol *x;
    NodoArbol *subarbol;

    if (y == NULL || y->izq == NULL)
    {
        return y;
    }

    x = y->izq;
    subarbol = x->der;

    x->der = y;
    y->izq = subarbol;

    arbol_actualizar_altura(y);
    arbol_actualizar_altura(x);

    return x;
}

static NodoArbol *arbol_rotar_izquierda(NodoArbol *x)
{
    NodoArbol *y;
    NodoArbol *subarbol;

    if (x == NULL || x->der == NULL)
    {
        return x;
    }

    y = x->der;
    subarbol = y->izq;

    y->izq = x;
    x->der = subarbol;

    arbol_actualizar_altura(x);
    arbol_actualizar_altura(y);

    return y;
}

static NodoArbol *arbol_balancear(NodoArbol *nodo)
{
    int factor;

    if (nodo == NULL)
    {
        return NULL;
    }

    factor = arbol_factor_balance(nodo);

    if (factor > 1)
    {
        if (arbol_factor_balance(nodo->izq) < 0)
        {
            nodo->izq = arbol_rotar_izquierda(nodo->izq);
        }

        return arbol_rotar_derecha(nodo);
    }

    if (factor < -1)
    {
        if (arbol_factor_balance(nodo->der) > 0)
        {
            nodo->der = arbol_rotar_derecha(nodo->der);
        }

        return arbol_rotar_izquierda(nodo);
    }

    return nodo;
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

static int arbol_verificar_abb_rec(
    const NodoArbol *raiz,
    int tieneMinimo,
    double minimo,
    int tieneMaximo,
    double maximo
)
{
    if (raiz == NULL)
    {
        return 1;
    }

    /*
     * Convencion: izquierda <= nodo <= derecha.
     *
     * La insercion manda las claves iguales a la derecha,
     * pero las rotaciones AVL pueden dejarlas a la izquierda.
     * Por eso la verificacion acepta duplicados en ambos lados
     * y solo exige que el recorrido inorden sea no decreciente.
     */
    if (tieneMinimo && raiz->valor < minimo)
    {
        return 0;
    }

    if (tieneMaximo && raiz->valor > maximo)
    {
        return 0;
    }

    return arbol_verificar_abb_rec(
               raiz->izq,
               tieneMinimo,
               minimo,
               1,
               raiz->valor
           ) &&
           arbol_verificar_abb_rec(
               raiz->der,
               1,
               raiz->valor,
               tieneMaximo,
               maximo
           );
}

int arbol_verificar_abb(const NodoArbol *raiz)
{
    return arbol_verificar_abb_rec(raiz, 0, 0.0, 0, 0.0);
}

static int arbol_verificar_avl_rec(const NodoArbol *raiz)
{
    int alturaIzquierda;
    int alturaDerecha;
    int factor;

    if (raiz == NULL)
    {
        return 1;
    }

    alturaIzquierda = arbol_altura(raiz->izq);
    alturaDerecha = arbol_altura(raiz->der);

    if (raiz->altura != 1 + arbol_maximo(alturaIzquierda, alturaDerecha))
    {
        return 0;
    }

    factor = alturaIzquierda - alturaDerecha;

    if (factor < -1 || factor > 1)
    {
        return 0;
    }

    return arbol_verificar_avl_rec(raiz->izq) &&
           arbol_verificar_avl_rec(raiz->der);
}

int arbol_verificar_avl(const NodoArbol *raiz)
{
    return arbol_verificar_abb(raiz) && arbol_verificar_avl_rec(raiz);
}

int arbol_es_avl(const NodoArbol *raiz)
{
    return arbol_verificar_avl(raiz);
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
    if (muestra == NULL)
    {
        return raiz;
    }

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

NodoArbol *arbol_insertar_avl(
    NodoArbol *raiz,
    const MuestraTelemetria *muestra
)
{
    if (muestra == NULL)
    {
        return raiz;
    }

    if (raiz == NULL)
    {
        return arbol_crear_muestra(muestra);
    }

    if (muestra->distancia < raiz->muestra.distancia)
    {
        raiz->izq = arbol_insertar_avl(raiz->izq, muestra);
    }
    else
    {
        raiz->der = arbol_insertar_avl(raiz->der, muestra);
    }

    arbol_actualizar_altura(raiz);

    return arbol_balancear(raiz);
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

NodoArbol *arbol_buscar_mas_cercano(
    NodoArbol *raiz,
    double clave,
    int *comparaciones
)
{
    NodoArbol *actual;
    NodoArbol *mejor;
    double mejorDiferencia;
    int contador;

    actual = raiz;
    mejor = NULL;
    mejorDiferencia = 0.0;
    contador = 0;

    while (actual != NULL)
    {
        double diferencia;

        contador++;
        diferencia = fabs(actual->muestra.distancia - clave);

        if (mejor == NULL || diferencia < mejorDiferencia)
        {
            mejor = actual;
            mejorDiferencia = diferencia;
        }

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

    return mejor;
}

void arbol_estadisticas_busqueda(
    NodoArbol *raiz,
    const MuestraTelemetria muestras[],
    int cantidad,
    EstadisticasBusqueda *estadisticas
)
{
    int i;

    if (estadisticas == NULL)
    {
        return;
    }

    memset(estadisticas, 0, sizeof(EstadisticasBusqueda));

    if (raiz == NULL || muestras == NULL || cantidad <= 0)
    {
        return;
    }

    for (i = 0; i < cantidad; i++)
    {
        int comparaciones;
        NodoArbol *encontrado;

        encontrado = arbol_buscar(
            raiz,
            muestras[i].distancia,
            &comparaciones
        );

        estadisticas->totalComparaciones += comparaciones;

        if (comparaciones > estadisticas->maxComparaciones)
        {
            estadisticas->maxComparaciones = comparaciones;
        }

        if (encontrado != NULL)
        {
            estadisticas->encontrados++;
        }
    }

    estadisticas->promedioComparaciones =
        (double) estadisticas->totalComparaciones /
        (double) cantidad;
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
