#ifndef TRABAJO_H
#define TRABAJO_H

#include "tli.h"

/*
=================================================
PUNTO 3 - TRABAJO Y EQUILIBRIO

Modelo gravitatorio del escenario TLI:
- Energia potencial gravitatoria.
- Potencial efectivo Tierra-Luna.
- Punto de equilibrio gravitatorio y punto L1.
- Clasificacion del equilibrio.
- Trabajo externo entre LEO y el equilibrio.
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

/* Resultado completo del Punto 3 */
typedef struct
{
    double r0;
    double rEquilibrio;
    double rL1;

    double potencialEspecificoLEO;
    double potencialEspecificoL1;

    /* Trabajo externo por unidad de masa (J/kg) */
    double trabajoEspecificoGravitatorio;
    double trabajoEspecificoEfectivo;

    /* Trabajo externo total (J) */
    double trabajoTotalEfectivo;

    double segundaDerivadaL1;
    TipoEquilibrio clasificacion;
    int iteracionesL1;

} ResultadoTrabajo;

/* Potencial gravitatorio especifico (J/kg) */
double trabajo_potencial_especifico(double r);

/* Potencial efectivo especifico en el sistema
   Tierra-Luna, en el marco rotante (J/kg) */
double trabajo_potencial_efectivo_especifico(double r);

/* Aceleracion efectiva sobre la linea Tierra-Luna
   en el marco rotante (m/s^2) */
double trabajo_aceleracion_efectiva(double r);

/* Segunda derivada del potencial efectivo (J/kg/m^2) */
double trabajo_segunda_derivada_efectiva(double r);

/* Punto de equilibrio gravitatorio puro (m) */
double trabajo_radio_equilibrio_gravitatorio(void);

/* Punto L1 por biseccion recursiva (m) */
double trabajo_radio_L1_recursivo(
    double a,
    double b,
    double tolerancia,
    int *iteraciones
);

/* Clasifica el equilibrio segun la segunda derivada */
TipoEquilibrio trabajo_clasificar_equilibrio(double r);

/* Nombre legible de un tipo de equilibrio */
const char *trabajo_nombre_equilibrio(TipoEquilibrio tipo);

/* Calcula todos los resultados del Punto 3 */
ResultadoTrabajo trabajo_calcular(const DatosTLI *datos);

/* Muestra los resultados del Punto 3 */
void trabajo_mostrar_resultado(
    const ResultadoTrabajo *resultado,
    const DatosTLI *datos
);

/* Genera muestras de telemetria entre LEO y L1 */
int trabajo_generar_muestras(
    const DatosTLI *datos,
    const ResultadoTrabajo *resultado,
    MuestraTelemetria muestras[],
    int maximo
);

/* Punto 3 del menu */
void punto3_trabajo(
    DatosTLI datos,
    BalanceEnergetico balance,
    NodoArbol **raiz
);

#endif
