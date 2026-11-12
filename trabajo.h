#ifndef TRABAJO_H
#define TRABAJO_H

#include "tli.h"
#include "arbol.h"

/*
=================================================
PUNTO 3 - TRABAJO Y EQUILIBRIO

Modelo gravitatorio del escenario TLI:
- Energia potencial gravitatoria total.
- Potencial efectivo Tierra-Luna.
- Punto de equilibrio gravitatorio y punto L1.
- Clasificacion del equilibrio.
- Trabajo externo e integracion numerica.
- Telemetria y comparacion ABB vs AVL.
=================================================
*/

/* Altitud de la orbita de estacionamiento (LEO) en metros */
#define H_LEO 192000.0

/* Intervalo y tolerancia para buscar el punto L1 */
#define L1_R_MIN 1.0e7
#define L1_R_MAX (DISTANCIA_TL - 1.0e7)
#define L1_TOLERANCIA 1.0

/* Tipos de equilibrio mecanico */
typedef enum
{
    EQUILIBRIO_ESTABLE = 0,
    EQUILIBRIO_INESTABLE = 1,
    EQUILIBRIO_NEUTRO = 2

} TipoEquilibrio;

/* Resultado de la busqueda robusta de L1 */
typedef struct
{
    double radio;
    int iteraciones;
    double residuo;
    int valido;

} ResultadoBusquedaL1;

/* Resultado completo del Punto 3 */
typedef struct
{
    double r0;
    double rEquilibrio;
    double rL1;

    double potencialEspecificoLEO;
    double potencialEspecificoL1;
    double potencialTotalLEO;
    double potencialTotalEquilibrio;

    /* Trabajo externo por unidad de masa (J/kg) */
    double trabajoEspecificoGravitatorio;
    double trabajoEspecificoEfectivo;
    double trabajoIntegradoEfectivo;
    double errorRelativoIntegracion;

    /* Trabajo externo total (J) */
    double trabajoTotalGravitatorio;
    double trabajoTotalEfectivo;

    double segundaDerivadaL1;
    double segundaDerivadaGravitatoriaEquilibrio;
    TipoEquilibrio clasificacion;
    TipoEquilibrio clasificacionGravitatoria;
    int iteracionesL1;
    double residualL1;
    int busquedaL1Valida;

    double energiaEspecificaInicial;
    double rGiro;
    int alcanzaL1;
    double desviacionMaximaEnergia;

} ResultadoTrabajo;

/* Telemetria completa del Punto 3 */
typedef struct TelemetriaPunto3
{
    MuestraTelemetria muestras[MAX_TELEMETRIA];
    int cantidad;

    NodoArbol *abb;
    NodoArbol *avl;

    EstadisticasBusqueda estadisticasAbb;
    EstadisticasBusqueda estadisticasAvl;

} TelemetriaPunto3;

/* Resultado de las autopruebas */
typedef struct
{
    int pruebasTotales;
    int pruebasOk;
    int pruebasFallidas;

} ResultadoVerificacion;

/* Potencial gravitatorio terrestre especifico (J/kg) */
double trabajo_potencial_especifico(double r);

/* Potencial gravitatorio total Tierra-Luna (J/kg) */
double trabajo_potencial_gravitatorio_total_especifico(double r);

/* Potencial efectivo especifico en el sistema
   Tierra-Luna, en el marco rotante (J/kg) */
double trabajo_potencial_efectivo_especifico(double r);

/* Aceleracion efectiva sobre la linea Tierra-Luna
   en el marco rotante (m/s^2) */
double trabajo_aceleracion_efectiva(double r);

/* Segunda derivada del potencial efectivo (J/kg/m^2) */
double trabajo_segunda_derivada_efectiva(double r);

/* Segunda derivada del potencial gravitatorio total */
double trabajo_segunda_derivada_gravitatoria(double r);

/* Punto de equilibrio gravitatorio puro (m) */
double trabajo_radio_equilibrio_gravitatorio(void);

/* Busqueda robusta de L1 por biseccion */
ResultadoBusquedaL1 trabajo_buscar_L1(
    double a,
    double b,
    double tolerancia
);

/* Punto L1 por biseccion recursiva (compatibilidad) */
double trabajo_radio_L1_recursivo(
    double a,
    double b,
    double tolerancia,
    int *iteraciones
);

/* Clasifica el equilibrio segun la segunda derivada efectiva */
TipoEquilibrio trabajo_clasificar_equilibrio(double r);

/* Clasifica el equilibrio gravitatorio total */
TipoEquilibrio trabajo_clasificar_equilibrio_gravitatorio(double r);

/* Nombre legible de un tipo de equilibrio */
const char *trabajo_nombre_equilibrio(TipoEquilibrio tipo);

/* Integra el trabajo externo con el metodo de Simpson */
double trabajo_integrar_fuerza(
    double r0,
    double r1,
    int pasos
);

/* Radio de giro si la nave no alcanza L1.
   Devuelve -1.0 si no se puede determinar. */
double trabajo_radio_giro(
    const DatosTLI *datos,
    const ResultadoTrabajo *resultado
);

/* Potencia media a partir del trabajo y el tiempo */
double trabajo_potencia_media(
    double trabajoTotal,
    double tiempoTotal
);

/* Calcula todos los resultados del Punto 3 */
ResultadoTrabajo trabajo_calcular(const DatosTLI *datos);

/* Muestra los resultados del Punto 3 */
void trabajo_mostrar_resultado(
    const ResultadoTrabajo *resultado,
    const DatosTLI *datos
);

/* Muestra la tabla de puntos clave del equilibrio */
void trabajo_mostrar_tabla_equilibrio(
    const ResultadoTrabajo *resultado,
    const DatosTLI *datos
);

/* Genera muestras de telemetria entre LEO y el punto
   alcanzable. Actualiza rGiro y la desviacion de energia. */
int trabajo_generar_muestras(
    const DatosTLI *datos,
    ResultadoTrabajo *resultado,
    MuestraTelemetria muestras[],
    int maximo
);

/* Punto 3 del menu */
void punto3_trabajo(
    DatosTLI datos,
    BalanceEnergetico balance,
    TelemetriaPunto3 *telemetria
);

/* Muestra el detalle de los arboles ABB y AVL */
void punto3_mostrar_arboles(
    const TelemetriaPunto3 *telemetria
);

/* Libera los arboles y limpia la estructura */
void punto3_liberar_telemetria(
    TelemetriaPunto3 *telemetria
);

/* Ejecuta autopruebas contra los valores de control */
ResultadoVerificacion trabajo_verificar(void);

#endif
