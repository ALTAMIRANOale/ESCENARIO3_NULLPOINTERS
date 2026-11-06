#ifndef TLI_H
#define TLI_H

/*
=================================================
PROYECTO TLI - SIMULACION TIERRA-LUNA

Este archivo contiene:

- Constantes físicas globales.
- Estructuras de datos.
- Prototipos de funciones.
- Configuración general del proyecto.

Las constantes físicas se cargan
automáticamente y NO deben solicitarse
al usuario.

El usuario únicamente debe ingresar
los datos variables de la misión.
=================================================
*/

/*=========================================
  CONSTANTES FISICAS
=========================================*/

/* Constante gravitacional universal */
#define G_CONST 6.67430e-11

/* Masa de la Tierra (kg) */
#define M_TIERRA 5.972e24

/* Masa de la Luna (kg) */
#define M_LUNA 7.342e22

/* Radio medio terrestre (m) */
#define R_TIERRA 6371000.0

/* Distancia promedio Tierra-Luna (m) */
#define DISTANCIA_TL 384000000.0

/*=========================================
  CONFIGURACION GENERAL
=========================================*/

/* Máximo de registros energéticos */
#define MAX_TELEMETRIA 100

/* Tamaño máximo para nombres */
#define MAX_NOMBRE 50

/*=========================================
  DATOS PRINCIPALES DE LA MISION
=========================================*/

/*
 * Guarda únicamente datos variables
 * ingresados por el usuario.
 */
typedef struct
{
    double masaNave;
    double velocidadInicial;

    /* Indica si los datos fueron cargados */
    int cargado;

} DatosTLI;

/*=========================================
  BALANCE ENERGETICO
=========================================*/

/*
 * Resultados calculados por
 * el Punto 2.
 */
typedef struct
{
    double energiaCinetica;
    double energiaPotencial;
    double energiaTotal;
    double combustibleNecesario;

} BalanceEnergetico;

/*=========================================
  BITACORA DE EVENTOS
=========================================*/

/*
 * Lista enlazada simple para registrar
 * eventos durante la simulación.
 */
typedef struct NodoEvento
{
    char descripcion[100];

    double tiempo;

    struct NodoEvento *siguiente;

} NodoEvento;

/* Operaciones de la bitácora */
void agregarEvento(
    NodoEvento **lista,
    const char *descripcion,
    double tiempo
);

void mostrarBitacora(
    NodoEvento *lista
);

void liberarBitacora(
    NodoEvento *lista
);

/*=========================================
  ARBOL BINARIO DE EVALUACION
=========================================*/

/*
 * Árbol utilizado para almacenar
 * valores calculados.
 */
typedef struct NodoArbol
{
    char nombre[MAX_NOMBRE];

    double valor;

    struct NodoArbol *izq;
    struct NodoArbol *der;

} NodoArbol;

/* Operaciones del árbol */
NodoArbol* crearNodoArbol(
    const char *nombre,
    double valor
);

NodoArbol* insertarArbol(
    NodoArbol *raiz,
    const char *nombre,
    double valor
);

void evaluarArbol(
    NodoArbol *raiz
);

void liberarArbol(
    NodoArbol *raiz
);

/*=========================================
  FUNCIONES PRINCIPALES
=========================================*/

/*
 * Punto 1
 * Ley de Newton
 */
void punto1_leyNewton(
    DatosTLI datos,
    NodoEvento **bitacora
);

/*
 * Punto 2
 * Balance energético
 */
BalanceEnergetico punto2_energia(
    DatosTLI datos,
    double historialEnergia[],
    int *cantidad
);

/*
 * Punto 3
 * Trabajo mecánico
 */
void punto3_trabajo(
    DatosTLI datos,
    BalanceEnergetico balance,
    NodoArbol **raiz
);

/*
 * Punto 4
 * Consumo de combustible
 */
void punto4_combustible(
    DatosTLI datos,
    BalanceEnergetico balance
);

/*=========================================
  FUNCIONES DEL MENU
=========================================*/

/*
 * Solicita al usuario los datos
 * necesarios para la simulación.
 */
void cargarDatos(
    DatosTLI *datos
);

/*
 * Lee un número positivo validado.
 */
double leerDoublePositivo(
    const char *mensaje
);

#endif
