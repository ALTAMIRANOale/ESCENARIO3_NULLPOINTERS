#include <stdio.h>
#include <math.h>
#include <string.h>

#include "trabajo.h"
#include "arbol.h"

/*=========================================
  CONSTANTES INTERNAS
=========================================*/

#define TRABAJO_PASOS_INTEGRACION 2000
#define TRABAJO_MAX_ITERACIONES 200
#define TRABAJO_TOL_RESIDUO_L1 1.0e-6
#define TRABAJO_TOL_ENERGIA 1.0e-9

/*=========================================
  MODELO GRAVITATORIO
=========================================*/

/*
 * Cuadrado de la velocidad angular del sistema
 * Tierra-Luna (rad^2/s^2).
 */
static double trabajo_omega_cuadrada(void)
{
    return (G_CONST * (M_TIERRA + M_LUNA)) /
           (DISTANCIA_TL * DISTANCIA_TL * DISTANCIA_TL);
}

/* Potencial gravitatorio terrestre especifico (J/kg) */
double trabajo_potencial_especifico(double r)
{
    if (r <= 0.0)
    {
        return 0.0;
    }

    return -(G_CONST * M_TIERRA) / r;
}

/* Potencial gravitatorio total Tierra-Luna (J/kg) */
double trabajo_potencial_gravitatorio_total_especifico(double r)
{
    if (r <= 0.0 || r >= DISTANCIA_TL)
    {
        return 0.0;
    }

    return -(G_CONST * M_TIERRA) / r
           - (G_CONST * M_LUNA) / (DISTANCIA_TL - r);
}

/* Potencial efectivo especifico en el marco rotante (J/kg) */
double trabajo_potencial_efectivo_especifico(double r)
{
    double omega2;

    if (r <= 0.0 || r >= DISTANCIA_TL)
    {
        return 0.0;
    }

    omega2 = trabajo_omega_cuadrada();

    return -(G_CONST * M_TIERRA) / r
           - (G_CONST * M_LUNA) / (DISTANCIA_TL - r)
           - 0.5 * omega2 * r * r;
}

/*
 * Aceleracion efectiva sobre la linea Tierra-Luna (m/s^2).
 * Convencion: positiva hacia la Luna (r creciente).
 *
 *   F_eff = -dU_eff/dr
 */
double trabajo_aceleracion_efectiva(double r)
{
    double omega2;

    if (r <= 0.0 || r >= DISTANCIA_TL)
    {
        return 0.0;
    }

    omega2 = trabajo_omega_cuadrada();

    return -(G_CONST * M_TIERRA) / (r * r)
           + (G_CONST * M_LUNA) /
             ((DISTANCIA_TL - r) * (DISTANCIA_TL - r))
           + omega2 * r;
}

/* Segunda derivada del potencial efectivo (J/kg/m^2) */
double trabajo_segunda_derivada_efectiva(double r)
{
    double omega2;

    if (r <= 0.0 || r >= DISTANCIA_TL)
    {
        return 0.0;
    }

    omega2 = trabajo_omega_cuadrada();

    return -(2.0 * G_CONST * M_TIERRA) / (r * r * r)
           - (2.0 * G_CONST * M_LUNA) /
             ((DISTANCIA_TL - r) *
              (DISTANCIA_TL - r) *
              (DISTANCIA_TL - r))
           - omega2;
}

/* Segunda derivada del potencial gravitatorio total (J/kg/m^2) */
double trabajo_segunda_derivada_gravitatoria(double r)
{
    double distanciaLuna;

    if (r <= 0.0 || r >= DISTANCIA_TL)
    {
        return 0.0;
    }

    distanciaLuna = DISTANCIA_TL - r;

    return -(2.0 * G_CONST * M_TIERRA) / (r * r * r)
           - (2.0 * G_CONST * M_LUNA) /
             (distanciaLuna * distanciaLuna * distanciaLuna);
}

/*
 * Punto donde se igualan las atracciones gravitatorias
 * de la Tierra y la Luna:
 *
 *   G*M_T/r^2 = G*M_L/(D-r)^2
 *
 * Despejando: r = D / (1 + sqrt(M_L/M_T))
 */
double trabajo_radio_equilibrio_gravitatorio(void)
{
    double raiz;

    raiz = sqrt(M_LUNA / M_TIERRA);

    return DISTANCIA_TL / (1.0 + raiz);
}

/*=========================================
  BUSQUEDA DE L1
=========================================*/

ResultadoBusquedaL1 trabajo_buscar_L1(
    double a,
    double b,
    double tolerancia
)
{
    ResultadoBusquedaL1 resultado = {0};
    double fa;
    double fb;
    int i;

    if (a >= b || tolerancia <= 0.0)
    {
        resultado.valido = 0;
        return resultado;
    }

    fa = trabajo_aceleracion_efectiva(a);
    fb = trabajo_aceleracion_efectiva(b);

    if (fa * fb > 0.0)
    {
        resultado.radio = 0.5 * (a + b);
        resultado.residuo =
            fabs(trabajo_aceleracion_efectiva(resultado.radio));
        resultado.valido = 0;
        return resultado;
    }

    for (i = 0;
         i < TRABAJO_MAX_ITERACIONES && (b - a) > tolerancia;
         i++)
    {
        double medio;
        double fm;

        medio = 0.5 * (a + b);
        fm = trabajo_aceleracion_efectiva(medio);

        if (fabs(fm) < 1.0e-14)
        {
            a = medio;
            b = medio;
            break;
        }

        if (fa * fm <= 0.0)
        {
            b = medio;
        }
        else
        {
            a = medio;
            fa = fm;
        }
    }

    resultado.radio = 0.5 * (a + b);
    resultado.iteraciones = i;
    resultado.residuo =
        fabs(trabajo_aceleracion_efectiva(resultado.radio));
    resultado.valido =
        (resultado.residuo < TRABAJO_TOL_RESIDUO_L1) ? 1 : 0;

    return resultado;
}

/*
 * Version recursiva conservada por compatibilidad.
 * El codigo nuevo usa trabajo_buscar_L1(), que valida
 * el cambio de signo y el residuo.
 */
double trabajo_radio_L1_recursivo(
    double a,
    double b,
    double tolerancia,
    int *iteraciones
)
{
    double medio;

    if (a >= b)
    {
        return 0.5 * (a + b);
    }

    if (iteraciones != NULL)
    {
        (*iteraciones)++;
    }

    medio = 0.5 * (a + b);

    if ((b - a) <= tolerancia)
    {
        return medio;
    }

    if (trabajo_aceleracion_efectiva(a) *
        trabajo_aceleracion_efectiva(b) > 0.0)
    {
        return medio;
    }

    if (trabajo_aceleracion_efectiva(a) *
        trabajo_aceleracion_efectiva(medio) <= 0.0)
    {
        return trabajo_radio_L1_recursivo(
            a,
            medio,
            tolerancia,
            iteraciones
        );
    }

    return trabajo_radio_L1_recursivo(
        medio,
        b,
        tolerancia,
        iteraciones
    );
}

/*=========================================
  CLASIFICACION DEL EQUILIBRIO
=========================================*/

TipoEquilibrio trabajo_clasificar_equilibrio(double r)
{
    double derivada2;

    derivada2 = trabajo_segunda_derivada_efectiva(r);

    if (derivada2 > 1.0e-14)
    {
        return EQUILIBRIO_ESTABLE;
    }

    if (derivada2 < -1.0e-14)
    {
        return EQUILIBRIO_INESTABLE;
    }

    return EQUILIBRIO_NEUTRO;
}

TipoEquilibrio trabajo_clasificar_equilibrio_gravitatorio(double r)
{
    double derivada2;

    derivada2 = trabajo_segunda_derivada_gravitatoria(r);

    if (derivada2 > 1.0e-14)
    {
        return EQUILIBRIO_ESTABLE;
    }

    if (derivada2 < -1.0e-14)
    {
        return EQUILIBRIO_INESTABLE;
    }

    return EQUILIBRIO_NEUTRO;
}

const char *trabajo_nombre_equilibrio(TipoEquilibrio tipo)
{
    switch (tipo)
    {
        case EQUILIBRIO_ESTABLE:
            return "ESTABLE (minimo del potencial)";

        case EQUILIBRIO_INESTABLE:
            return "INESTABLE (maximo del potencial)";

        case EQUILIBRIO_NEUTRO:
            return "NEUTRO (potencial constante)";

        default:
            return "DESCONOCIDO";
    }
}

static const char *trabajo_nombre_equilibrio_corto(TipoEquilibrio tipo)
{
    switch (tipo)
    {
        case EQUILIBRIO_ESTABLE:
            return "ESTABLE";

        case EQUILIBRIO_INESTABLE:
            return "INESTABLE";

        case EQUILIBRIO_NEUTRO:
            return "NEUTRO";

        default:
            return "?";
    }
}

/*=========================================
  TRABAJO E INTEGRACION
=========================================*/

double trabajo_integrar_fuerza(
    double r0,
    double r1,
    int pasos
)
{
    double h;
    double suma;
    int i;

    if (r0 <= 0.0 || r1 >= DISTANCIA_TL ||
        r0 >= r1 || pasos < 2)
    {
        return 0.0;
    }

    if ((pasos % 2) != 0)
    {
        pasos++;
    }

    h = (r1 - r0) / (double) pasos;

    /*
     * El trabajo externo para mover la nave cuasiestaticamente
     * es -integral(F_eff dr), que equivale a delta U_eff.
     */
    suma = -trabajo_aceleracion_efectiva(r0)
           - trabajo_aceleracion_efectiva(r1);

    for (i = 1; i < pasos; i++)
    {
        double x;
        double f;

        x = r0 + (double) i * h;
        f = -trabajo_aceleracion_efectiva(x);

        suma += ((i % 2) == 0) ? (2.0 * f) : (4.0 * f);
    }

    return suma * h / 3.0;
}

double trabajo_radio_giro(
    const DatosTLI *datos,
    const ResultadoTrabajo *resultado
)
{
    double energia;
    double uL1;
    double a;
    double b;
    double fa;
    double fb;
    int i;

    if (datos == NULL || resultado == NULL ||
        resultado->rL1 <= resultado->r0 ||
        datos->velocidadInicial < 0.0)
    {
        return -1.0;
    }

    energia =
        0.5 * datos->velocidadInicial * datos->velocidadInicial +
        trabajo_potencial_efectivo_especifico(resultado->r0);
    uL1 = trabajo_potencial_efectivo_especifico(resultado->rL1);

    if (energia >= uL1)
    {
        return resultado->rL1;
    }

    a = resultado->r0;
    b = resultado->rL1;
    fa = energia - trabajo_potencial_efectivo_especifico(a);
    fb = energia - trabajo_potencial_efectivo_especifico(b);

    if (fa * fb > 0.0)
    {
        return -1.0;
    }

    for (i = 0;
         i < TRABAJO_MAX_ITERACIONES && (b - a) > 1.0;
         i++)
    {
        double medio;
        double fm;

        medio = 0.5 * (a + b);
        fm = energia - trabajo_potencial_efectivo_especifico(medio);

        if (fa * fm <= 0.0)
        {
            b = medio;
        }
        else
        {
            a = medio;
            fa = fm;
        }
    }

    return 0.5 * (a + b);
}

double trabajo_potencia_media(
    double trabajoTotal,
    double tiempoTotal
)
{
    if (tiempoTotal <= 0.0)
    {
        return 0.0;
    }

    return trabajoTotal / tiempoTotal;
}

/*=========================================
  CALCULO COMPLETO DEL PUNTO 3
=========================================*/

ResultadoTrabajo trabajo_calcular(const DatosTLI *datos)
{
    ResultadoTrabajo resultado = {0};
    ResultadoBusquedaL1 busquedaL1;
    double ueff0;
    double ueffL1;

    if (datos == NULL)
    {
        return resultado;
    }

    resultado.r0 = R_TIERRA + H_LEO;
    resultado.rEquilibrio =
        trabajo_radio_equilibrio_gravitatorio();

    busquedaL1 = trabajo_buscar_L1(
        L1_R_MIN,
        L1_R_MAX,
        L1_TOLERANCIA
    );

    resultado.rL1 = busquedaL1.radio;
    resultado.iteracionesL1 = busquedaL1.iteraciones;
    resultado.residualL1 = busquedaL1.residuo;
    resultado.busquedaL1Valida = busquedaL1.valido;

    ueff0 = trabajo_potencial_efectivo_especifico(resultado.r0);
    ueffL1 = trabajo_potencial_efectivo_especifico(resultado.rL1);

    resultado.potencialEspecificoLEO =
        trabajo_potencial_especifico(resultado.r0);
    resultado.potencialEspecificoL1 = ueffL1;

    resultado.potencialTotalLEO =
        trabajo_potencial_gravitatorio_total_especifico(resultado.r0);
    resultado.potencialTotalEquilibrio =
        trabajo_potencial_gravitatorio_total_especifico(
            resultado.rEquilibrio
        );

    /*
     * Trabajo externo por unidad de masa como variacion
     * de energia potencial.
     */
    resultado.trabajoEspecificoGravitatorio =
        resultado.potencialTotalEquilibrio -
        resultado.potencialTotalLEO;

    resultado.trabajoTotalGravitatorio =
        resultado.trabajoEspecificoGravitatorio * datos->masaNave;

    resultado.trabajoEspecificoEfectivo =
        ueffL1 - ueff0;

    resultado.trabajoTotalEfectivo =
        resultado.trabajoEspecificoEfectivo * datos->masaNave;

    resultado.trabajoIntegradoEfectivo =
        trabajo_integrar_fuerza(
            resultado.r0,
            resultado.rL1,
            TRABAJO_PASOS_INTEGRACION
        );

    if (fabs(resultado.trabajoEspecificoEfectivo) > 1.0e-12)
    {
        resultado.errorRelativoIntegracion =
            fabs(resultado.trabajoIntegradoEfectivo -
                 resultado.trabajoEspecificoEfectivo) /
            fabs(resultado.trabajoEspecificoEfectivo);
    }
    else
    {
        resultado.errorRelativoIntegracion = 0.0;
    }

    resultado.segundaDerivadaL1 =
        trabajo_segunda_derivada_efectiva(resultado.rL1);

    resultado.segundaDerivadaGravitatoriaEquilibrio =
        trabajo_segunda_derivada_gravitatoria(resultado.rEquilibrio);

    resultado.clasificacion =
        trabajo_clasificar_equilibrio(resultado.rL1);

    resultado.clasificacionGravitatoria =
        trabajo_clasificar_equilibrio_gravitatorio(
            resultado.rEquilibrio
        );

    resultado.energiaEspecificaInicial =
        0.5 * datos->velocidadInicial * datos->velocidadInicial +
        ueff0;

    resultado.rGiro = trabajo_radio_giro(datos, &resultado);
    resultado.alcanzaL1 =
        (resultado.rGiro >= resultado.rL1 - 1.0) ? 1 : 0;

    return resultado;
}

/*=========================================
  GENERACION DE TELEMETRIA
=========================================*/

int trabajo_generar_muestras(
    const DatosTLI *datos,
    ResultadoTrabajo *resultado,
    MuestraTelemetria muestras[],
    int maximo
)
{
    double energia;
    double uL1;
    double rMax;
    double tiempoAcumulado;
    double desviacionMaxima;
    int i;

    if (datos == NULL || resultado == NULL ||
        muestras == NULL || maximo <= 0)
    {
        return 0;
    }

    if (resultado->rL1 <= resultado->r0)
    {
        return 0;
    }

    energia =
        0.5 * datos->velocidadInicial * datos->velocidadInicial +
        trabajo_potencial_efectivo_especifico(resultado->r0);

    uL1 = trabajo_potencial_efectivo_especifico(resultado->rL1);
    rMax = resultado->rL1;

    if (energia < uL1)
    {
        double giro;

        giro = trabajo_radio_giro(datos, resultado);

        if (giro <= resultado->r0)
        {
            resultado->desviacionMaximaEnergia = 0.0;
            return 0;
        }

        rMax = giro;
    }

    tiempoAcumulado = 0.0;
    desviacionMaxima = 0.0;

    for (i = 0; i < maximo; i++)
    {
        double fraccion;
        double r;
        double potencial;
        double v2;
        double v;
        double energiaMuestra;
        double desviacion;

        if (maximo == 1)
        {
            fraccion = 0.0;
        }
        else
        {
            fraccion = (double) i / (double) (maximo - 1);
        }

        r = resultado->r0 + fraccion * (rMax - resultado->r0);

        potencial = trabajo_potencial_efectivo_especifico(r);

        v2 = 2.0 * (energia - potencial);

        if (v2 < 0.0)
        {
            v2 = 0.0;
        }

        v = sqrt(v2);

        muestras[i].distancia = r;
        muestras[i].velocidad = v;
        muestras[i].energiaCinetica =
            datos->masaNave * 0.5 * v * v;
        muestras[i].energiaPotencial =
            datos->masaNave * potencial;
        muestras[i].energiaTotal =
            muestras[i].energiaCinetica +
            muestras[i].energiaPotencial;

        energiaMuestra = 0.5 * v * v + potencial;
        desviacion =
            fabs(energiaMuestra - energia) /
            fmax(1.0, fabs(energia));

        if (desviacion > desviacionMaxima)
        {
            desviacionMaxima = desviacion;
        }

        if (i == 0)
        {
            muestras[i].tiempo = 0.0;
        }
        else
        {
            double dr;
            double vPromedio;

            dr = muestras[i].distancia -
                 muestras[i - 1].distancia;
            vPromedio = 0.5 *
                        (v + muestras[i - 1].velocidad);

            if (vPromedio > 1.0e-9)
            {
                tiempoAcumulado += dr / vPromedio;
            }

            muestras[i].tiempo = tiempoAcumulado;
        }
    }

    resultado->rGiro = rMax;
    resultado->alcanzaL1 =
        (rMax >= resultado->rL1 - 1.0) ? 1 : 0;
    resultado->desviacionMaximaEnergia = desviacionMaxima;

    return maximo;
}

/*=========================================
  SALIDA POR PANTALLA
=========================================*/

void trabajo_mostrar_tabla_equilibrio(
    const ResultadoTrabajo *resultado,
    const DatosTLI *datos
)
{
    double rLuna;

    if (resultado == NULL || datos == NULL)
    {
        return;
    }

    rLuna = DISTANCIA_TL - 1.0e6;

    printf("\nPuntos clave del potencial efectivo (marco rotante):\n");
    printf("%-10s %15s %16s %16s %16s %s\n",
           "Punto",
           "r (m)",
           "U_eff (J/kg)",
           "F_eff (m/s2)",
           "d2U_eff (J/kg/m2)",
           "Equilibrio");

    printf("%-10s %15.0f %16.6e %16.6e %16.6e %s\n",
           "LEO",
           resultado->r0,
           trabajo_potencial_efectivo_especifico(resultado->r0),
           trabajo_aceleracion_efectiva(resultado->r0),
           trabajo_segunda_derivada_efectiva(resultado->r0),
           "-");

    printf("%-10s %15.0f %16.6e %16.6e %16.6e %s\n",
           "r_eq",
           resultado->rEquilibrio,
           trabajo_potencial_efectivo_especifico(
               resultado->rEquilibrio
           ),
           trabajo_aceleracion_efectiva(resultado->rEquilibrio),
           trabajo_segunda_derivada_efectiva(
               resultado->rEquilibrio
           ),
           "-");

    printf("%-10s %15.0f %16.6e %16.6e %16.6e %s\n",
           "L1",
           resultado->rL1,
           trabajo_potencial_efectivo_especifico(resultado->rL1),
           trabajo_aceleracion_efectiva(resultado->rL1),
           resultado->segundaDerivadaL1,
           trabajo_nombre_equilibrio_corto(
               resultado->clasificacion
           ));

    printf("%-10s %15.0f %16.6e %16.6e %16.6e %s\n",
           "Luna*",
           rLuna,
           trabajo_potencial_efectivo_especifico(rLuna),
           trabajo_aceleracion_efectiva(rLuna),
           trabajo_segunda_derivada_efectiva(rLuna),
           "-");

    printf("\n* Punto de referencia cercano a la Luna (D - 1.000.000 m).\n");
    printf("En r_eq se anula la fuerza gravitatoria pura, no la efectiva.\n");
    printf("d2U_grav/dr2 en r_eq: %.6e J/kg/m^2 (%s)\n",
           resultado->segundaDerivadaGravitatoriaEquilibrio,
           trabajo_nombre_equilibrio_corto(
               resultado->clasificacionGravitatoria
           ));
    printf("L1 se clasifica con el potencial efectivo: %s\n",
           trabajo_nombre_equilibrio_corto(
               resultado->clasificacion
           ));
}

void trabajo_mostrar_resultado(
    const ResultadoTrabajo *resultado,
    const DatosTLI *datos
)
{
    double diferencia;

    if (resultado == NULL || datos == NULL)
    {
        return;
    }

    printf("\n========== PUNTO 3: TRABAJO Y EQUILIBRIO ==========\n");

    printf("\nDatos de la nave:\n");
    printf("Masa: %.2f kg\n", datos->masaNave);
    printf("Velocidad inicial: %.2f m/s\n", datos->velocidadInicial);

    printf("\nOrbita de estacionamiento (LEO):\n");
    printf("Radio inicial r0 = %.0f m\n", resultado->r0);
    printf("Energia potencial terrestre: %.6e J/kg\n",
           resultado->potencialEspecificoLEO);
    printf("Energia potencial total Tierra-Luna: %.6e J/kg\n",
           resultado->potencialTotalLEO);

    printf("\nPunto de equilibrio gravitatorio Tierra-Luna:\n");
    printf("r_eq = %.6e m\n", resultado->rEquilibrio);
    printf("Energia potencial total: %.6e J/kg\n",
           resultado->potencialTotalEquilibrio);
    printf("d2U_grav/dr2 = %.6e J/kg/m^2\n",
           resultado->segundaDerivadaGravitatoriaEquilibrio);
    printf("Clasificacion: %s\n",
           trabajo_nombre_equilibrio(
               resultado->clasificacionGravitatoria
           ));

    printf("\nPunto L1 (sistema Tierra-Luna en rotacion):\n");
    printf("r_L1 = %.6e m\n", resultado->rL1);
    printf("Iteraciones de biseccion: %d\n",
           resultado->iteracionesL1);
    printf("Busqueda valida: %s\n",
           resultado->busquedaL1Valida ? "si" : "no");
    printf("Residuo |F_eff|: %.6e m/s^2\n",
           resultado->residualL1);
    printf("d2U_eff/dr2 = %.6e J/kg/m^2\n",
           resultado->segundaDerivadaL1);
    printf("Clasificacion: %s\n",
           trabajo_nombre_equilibrio(resultado->clasificacion));

    printf("\nTrabajo externo minimo:\n");
    printf("Gravedad total (LEO -> r_eq): %.6e J/kg\n",
           resultado->trabajoEspecificoGravitatorio);
    printf("Marco rotante (LEO -> L1):    %.6e J/kg\n",
           resultado->trabajoEspecificoEfectivo);
    printf("Integracion numerica:         %.6e J/kg\n",
           resultado->trabajoIntegradoEfectivo);
    printf("Error relativo integracion:   %.3e\n",
           resultado->errorRelativoIntegracion);
    printf("Equivalente en MJ/kg:         %.3f MJ/kg\n",
           resultado->trabajoEspecificoEfectivo / 1.0e6);
    printf("Trabajo total para %.0f kg:   %.6e J\n",
           datos->masaNave,
           resultado->trabajoTotalEfectivo);
    printf("Trabajo total gravitatorio:   %.6e J\n",
           resultado->trabajoTotalGravitatorio);

    diferencia = resultado->trabajoEspecificoEfectivo -
                 resultado->trabajoEspecificoGravitatorio;

    printf("\nDiferencia entre ambos modelos: %.6e J/kg\n",
           diferencia);
    printf("(El marco rotante incluye la atraccion lunar\n");
    printf(" y el termino centrifugo del sistema.)\n");

    printf("\nEnergia especifica inicial: %.6e J/kg\n",
           resultado->energiaEspecificaInicial);

    if (resultado->alcanzaL1)
    {
        printf("La nave alcanza L1 con la velocidad inicial.\n");
        printf("Radio de giro: %.6e m (L1)\n", resultado->rGiro);
    }
    else
    {
        printf("La nave NO alcanza L1 con la velocidad inicial.\n");

        if (resultado->rGiro > 0.0)
        {
            printf("Punto de giro: %.6e m\n", resultado->rGiro);
        }
        else
        {
            printf("No se pudo determinar el punto de giro.\n");
        }
    }

    printf("Desviacion maxima de energia: %.3e (relativa)\n",
           resultado->desviacionMaximaEnergia);

    trabajo_mostrar_tabla_equilibrio(resultado, datos);

    printf("\n====================================================\n");
}

static void trabajo_mostrar_comparacion_arboles(
    const TelemetriaPunto3 *telemetria
)
{
    int nodosAbb;
    int nodosAvl;

    if (telemetria == NULL)
    {
        return;
    }

    nodosAbb = arbol_contar_nodos(telemetria->abb);
    nodosAvl = arbol_contar_nodos(telemetria->avl);

    printf("\n========== COMPARACION ABB vs AVL ==========\n");
    printf("Muestras generadas: %d\n", telemetria->cantidad);
    printf("%-30s %12s %12s\n", "Metrica", "ABB", "AVL");
    printf("%-30s %12d %12d\n", "Nodos", nodosAbb, nodosAvl);
    printf("%-30s %12d %12d\n",
           "Altura",
           arbol_altura(telemetria->abb),
           arbol_altura(telemetria->avl));
    printf("%-30s %12s %12s\n",
           "Cumple AVL",
           arbol_es_avl(telemetria->abb) ? "si" : "no",
           arbol_es_avl(telemetria->avl) ? "si" : "no");
    printf("%-30s %12.2f %12.2f\n",
           "Comparaciones promedio",
           telemetria->estadisticasAbb.promedioComparaciones,
           telemetria->estadisticasAvl.promedioComparaciones);
    printf("%-30s %12d %12d\n",
           "Comparaciones maximas",
           telemetria->estadisticasAbb.maxComparaciones,
           telemetria->estadisticasAvl.maxComparaciones);
    printf("%-30s %12d %12d\n",
           "Busquedas encontradas",
           telemetria->estadisticasAbb.encontrados,
           telemetria->estadisticasAvl.encontrados);
    printf("%-30s %12s %12s\n",
           "Complejidad teorica",
           "O(n)",
           "O(log n)");

    if (telemetria->cantidad > 0)
    {
        double cota;

        cota = 1.44 *
               log((double) (telemetria->cantidad + 2)) /
               log(2.0);

        printf("Altura teorica maxima esperada para AVL: %.1f\n",
               cota);
    }

    if (nodosAbb > 0 &&
        arbol_altura(telemetria->abb) == nodosAbb)
    {
        printf("El ABB degenero en una lista por insercion ordenada.\n");
    }

    printf("============================================\n");
}

/*=========================================
  PUNTO 3 DEL MENU
=========================================*/

void punto3_trabajo(
    DatosTLI datos,
    BalanceEnergetico balance,
    TelemetriaPunto3 *telemetria
)
{
    ResultadoTrabajo resultado;
    int cantidad;
    int i;

    resultado = trabajo_calcular(&datos);

    if (telemetria == NULL)
    {
        trabajo_mostrar_resultado(&resultado, &datos);
        return;
    }

    if (telemetria->abb != NULL)
    {
        arbol_liberar(telemetria->abb);
    }

    if (telemetria->avl != NULL)
    {
        arbol_liberar(telemetria->avl);
    }

    memset(telemetria, 0, sizeof(TelemetriaPunto3));

    cantidad = trabajo_generar_muestras(
        &datos,
        &resultado,
        telemetria->muestras,
        MAX_TELEMETRIA
    );

    telemetria->cantidad = cantidad;

    trabajo_mostrar_resultado(&resultado, &datos);

    printf("\nConsistencia con el Punto 2:\n");

    if (balance.energiaCinetica > 0.0 && datos.masaNave > 0.0)
    {
        double v2Balance;
        double v2Datos;
        double diferenciaRelativa;

        v2Balance =
            2.0 * balance.energiaCinetica / datos.masaNave;
        v2Datos =
            datos.velocidadInicial * datos.velocidadInicial;

        diferenciaRelativa =
            fabs(v2Balance - v2Datos) /
            fmax(1.0, fabs(v2Datos));

        printf("Energia cinetica del Punto 2: %.6e J\n",
               balance.energiaCinetica);
        printf("Energia cinetica equivalente: %.6e J\n",
               0.5 * datos.masaNave * v2Datos);
        printf("Diferencia relativa: %.3e\n",
               diferenciaRelativa);

        if (diferenciaRelativa > 1.0e-9)
        {
            printf("Advertencia: el balance del Punto 2 no coincide.\n");
        }
        else
        {
            printf("El balance del Punto 2 es consistente.\n");
        }
    }
    else
    {
        printf("Punto 2 no ejecutado o sin energia valida.\n");
        printf("Se usa 0.5*m*v0^2 como respaldo.\n");
    }

    if (cantidad > 0)
    {
        double tiempoFinal;

        for (i = 0; i < cantidad; i++)
        {
            telemetria->abb = arbol_insertar_abb(
                telemetria->abb,
                &telemetria->muestras[i]
            );

            telemetria->avl = arbol_insertar_avl(
                telemetria->avl,
                &telemetria->muestras[i]
            );
        }

        arbol_estadisticas_busqueda(
            telemetria->abb,
            telemetria->muestras,
            cantidad,
            &telemetria->estadisticasAbb
        );

        arbol_estadisticas_busqueda(
            telemetria->avl,
            telemetria->muestras,
            cantidad,
            &telemetria->estadisticasAvl
        );

        trabajo_mostrar_comparacion_arboles(telemetria);

        tiempoFinal = telemetria->muestras[cantidad - 1].tiempo;

        if (tiempoFinal > 0.0)
        {
            printf("\nPotencia media estimada: %.6e W\n",
                   trabajo_potencia_media(
                       resultado.trabajoTotalEfectivo,
                       tiempoFinal
                   ));
            printf("(Tiempo radial aproximado: %.3e s)\n",
                   tiempoFinal);
        }

        printf("\nUse la opcion 7 para ver el detalle de los arboles.\n");
    }
    else
    {
        printf("\nNo se generaron muestras de telemetria.\n");
        printf("La nave no tiene energia suficiente para avanzar.\n");
    }
}

void punto3_mostrar_arboles(
    const TelemetriaPunto3 *telemetria
)
{
    printf("\n========== ARBOLES DE TELEMETRIA ==========\n");

    if (telemetria == NULL ||
        (telemetria->abb == NULL && telemetria->avl == NULL))
    {
        printf("\nNo hay muestras cargadas.\n");
        printf("Ejecute primero la opcion 4 (Punto 3).\n");
        printf("\n==========================================\n");
        return;
    }

    trabajo_mostrar_comparacion_arboles(telemetria);

    printf("\nRecorrido inorden del ABB:\n");
    arbol_inorden(telemetria->abb);
    printf("\nEl AVL contiene las mismas muestras en el mismo orden.\n");
    printf("Solo se repite el recorrido del ABB para no duplicar salida.\n");

    printf("\n==========================================\n");
}

void punto3_liberar_telemetria(
    TelemetriaPunto3 *telemetria
)
{
    if (telemetria == NULL)
    {
        return;
    }

    arbol_liberar(telemetria->abb);
    arbol_liberar(telemetria->avl);

    memset(telemetria, 0, sizeof(TelemetriaPunto3));
}

/*=========================================
  AUTOVERIFICACION
=========================================*/

static void trabajo_registrar_prueba(
    ResultadoVerificacion *verificacion,
    int correcta,
    const char *nombre,
    const char *detalle
)
{
    if (verificacion == NULL)
    {
        return;
    }

    verificacion->pruebasTotales++;

    if (correcta)
    {
        verificacion->pruebasOk++;
        printf("[OK]    %s", nombre);
    }
    else
    {
        verificacion->pruebasFallidas++;
        printf("[FALLA] %s", nombre);
    }

    if (detalle != NULL && detalle[0] != '\0')
    {
        printf(" (%s)", detalle);
    }

    printf("\n");
}

ResultadoVerificacion trabajo_verificar(void)
{
    ResultadoVerificacion verificacion = {0};
    DatosTLI datos = {26000.0, 10900.0, 1};
    DatosTLI datosLentos = {26000.0, 1000.0, 1};
    ResultadoTrabajo resultado;
    ResultadoTrabajo resultadoLento;
    ResultadoBusquedaL1 busqueda;
    TelemetriaPunto3 telemetria = {0};
    MuestraTelemetria muestrasLentas[MAX_TELEMETRIA];
    NodoArbol *avlInverso = NULL;
    NodoArbol *avlDuplicado = NULL;
    int cantidad;
    int cantidadLenta;
    int duplicados;
    int i;
    char detalle[160];

    printf("\n========== VERIFICACION PUNTO 3 ==========\n");

    resultado = trabajo_calcular(&datos);
    busqueda = trabajo_buscar_L1(L1_R_MIN, L1_R_MAX, L1_TOLERANCIA);

    snprintf(detalle, sizeof(detalle),
             "r_eq = %.6e m", resultado.rEquilibrio);
    trabajo_registrar_prueba(
        &verificacion,
        fabs(resultado.rEquilibrio - 3.456724e8) < 1.0e4,
        "r_eq contra valor de control",
        detalle
    );

    snprintf(detalle, sizeof(detalle),
             "r_L1 = %.6e m", resultado.rL1);
    trabajo_registrar_prueba(
        &verificacion,
        fabs(resultado.rL1 - 3.256346e8) < 1.0e4,
        "r_L1 contra valor de control",
        detalle
    );

    snprintf(detalle, sizeof(detalle),
             "residuo = %.3e m/s^2", resultado.residualL1);
    trabajo_registrar_prueba(
        &verificacion,
        resultado.busquedaL1Valida == 1 &&
        busqueda.valido == 1 &&
        resultado.residualL1 < TRABAJO_TOL_RESIDUO_L1,
        "busqueda de L1 valida",
        detalle
    );

    snprintf(detalle, sizeof(detalle),
             "W = %.6e J/kg", resultado.trabajoEspecificoEfectivo);
    trabajo_registrar_prueba(
        &verificacion,
        fabs(resultado.trabajoEspecificoEfectivo - 5.9e7) /
        fabs(5.9e7) < 1.0e-2,
        "trabajo especifico contra valor de control",
        detalle
    );

    snprintf(detalle, sizeof(detalle),
             "error = %.3e", resultado.errorRelativoIntegracion);
    trabajo_registrar_prueba(
        &verificacion,
        resultado.errorRelativoIntegracion < 1.0e-6,
        "integracion numerica vs potencial",
        detalle
    );

    snprintf(detalle, sizeof(detalle),
             "d2U = %.3e", resultado.segundaDerivadaL1);
    trabajo_registrar_prueba(
        &verificacion,
        resultado.segundaDerivadaL1 < 0.0,
        "L1 es maximo del potencial efectivo",
        detalle
    );

    trabajo_registrar_prueba(
        &verificacion,
        resultado.clasificacion == EQUILIBRIO_INESTABLE,
        "clasificacion de L1",
        trabajo_nombre_equilibrio_corto(resultado.clasificacion)
    );

    trabajo_registrar_prueba(
        &verificacion,
        resultado.clasificacionGravitatoria == EQUILIBRIO_INESTABLE,
        "clasificacion de r_eq",
        trabajo_nombre_equilibrio_corto(
            resultado.clasificacionGravitatoria
        )
    );

    trabajo_registrar_prueba(
        &verificacion,
        resultado.alcanzaL1 == 1,
        "la nave alcanza L1 con 10.900 m/s",
        NULL
    );

    cantidad = trabajo_generar_muestras(
        &datos,
        &resultado,
        telemetria.muestras,
        MAX_TELEMETRIA
    );

    telemetria.cantidad = cantidad;

    snprintf(detalle, sizeof(detalle),
             "muestras = %d", cantidad);
    trabajo_registrar_prueba(
        &verificacion,
        cantidad == MAX_TELEMETRIA,
        "generacion de telemetria completa",
        detalle
    );

    snprintf(detalle, sizeof(detalle),
             "desviacion = %.3e",
             resultado.desviacionMaximaEnergia);
    trabajo_registrar_prueba(
        &verificacion,
        resultado.desviacionMaximaEnergia < TRABAJO_TOL_ENERGIA,
        "conservacion de energia",
        detalle
    );

    for (i = 0; i < cantidad; i++)
    {
        telemetria.abb = arbol_insertar_abb(
            telemetria.abb,
            &telemetria.muestras[i]
        );

        telemetria.avl = arbol_insertar_avl(
            telemetria.avl,
            &telemetria.muestras[i]
        );
    }

    arbol_estadisticas_busqueda(
        telemetria.abb,
        telemetria.muestras,
        cantidad,
        &telemetria.estadisticasAbb
    );

    arbol_estadisticas_busqueda(
        telemetria.avl,
        telemetria.muestras,
        cantidad,
        &telemetria.estadisticasAvl
    );

    trabajo_registrar_prueba(
        &verificacion,
        arbol_verificar_abb(telemetria.abb) == 1,
        "ABB cumple orden y alturas",
        NULL
    );

    trabajo_registrar_prueba(
        &verificacion,
        arbol_verificar_avl(telemetria.avl) == 1,
        "AVL cumple balance y alturas",
        NULL
    );

    for (i = cantidad - 1; i >= 0; i--)
    {
        avlInverso = arbol_insertar_avl(
            avlInverso,
            &telemetria.muestras[i]
        );
    }

    trabajo_registrar_prueba(
        &verificacion,
        arbol_verificar_avl(avlInverso) == 1 &&
        arbol_contar_nodos(avlInverso) == cantidad,
        "AVL con insercion inversa",
        NULL
    );

    arbol_liberar(avlInverso);
    avlInverso = NULL;

    duplicados = (cantidad < 10) ? cantidad : 10;

    for (i = 0; i < duplicados; i++)
    {
        avlDuplicado = arbol_insertar_avl(
            avlDuplicado,
            &telemetria.muestras[i]
        );

        avlDuplicado = arbol_insertar_avl(
            avlDuplicado,
            &telemetria.muestras[i]
        );
    }

    snprintf(detalle, sizeof(detalle),
             "nodos = %d",
             arbol_contar_nodos(avlDuplicado));

    trabajo_registrar_prueba(
        &verificacion,
        arbol_verificar_avl(avlDuplicado) == 1 &&
        arbol_contar_nodos(avlDuplicado) == 2 * duplicados,
        "AVL con claves duplicadas",
        detalle
    );

    arbol_liberar(avlDuplicado);
    avlDuplicado = NULL;

    trabajo_registrar_prueba(
        &verificacion,
        arbol_es_avl(telemetria.abb) == 0 &&
        arbol_altura(telemetria.abb) == cantidad,
        "ABB degenerado por insercion ordenada",
        NULL
    );

    if (cantidad > 0)
    {
        double cota;

        cota = 1.44 *
               log((double) (cantidad + 2)) /
               log(2.0);

        snprintf(detalle, sizeof(detalle),
                 "altura = %d, cota = %.1f",
                 arbol_altura(telemetria.avl),
                 cota);

        trabajo_registrar_prueba(
            &verificacion,
            arbol_altura(telemetria.avl) <= cota,
            "altura AVL dentro de la cota teorica",
            detalle
        );
    }

    snprintf(detalle, sizeof(detalle),
             "ABB = %.2f, AVL = %.2f",
             telemetria.estadisticasAbb.promedioComparaciones,
             telemetria.estadisticasAvl.promedioComparaciones);

    trabajo_registrar_prueba(
        &verificacion,
        telemetria.estadisticasAvl.promedioComparaciones <
        telemetria.estadisticasAbb.promedioComparaciones,
        "AVL reduce las comparaciones promedio",
        detalle
    );

    trabajo_registrar_prueba(
        &verificacion,
        telemetria.estadisticasAbb.encontrados == cantidad &&
        telemetria.estadisticasAvl.encontrados == cantidad,
        "todas las muestras son encontradas",
        NULL
    );

    if (cantidad > 0)
    {
        NodoArbol *cercano;
        double clave;
        int comparacionesCercano;

        clave = telemetria.muestras[cantidad / 2].distancia + 1.0;
        comparacionesCercano = 0;
        cercano = arbol_buscar_mas_cercano(
            telemetria.avl,
            clave,
            &comparacionesCercano
        );

        snprintf(detalle, sizeof(detalle),
                 "r = %.1f, comparaciones = %d",
                 cercano != NULL ? cercano->muestra.distancia : -1.0,
                 comparacionesCercano);

        trabajo_registrar_prueba(
            &verificacion,
            cercano != NULL &&
            fabs(cercano->muestra.distancia -
                 telemetria.muestras[cantidad / 2].distancia) < 1.0,
            "busqueda por cercania",
            detalle
        );
    }

    resultadoLento = trabajo_calcular(&datosLentos);
    cantidadLenta = trabajo_generar_muestras(
        &datosLentos,
        &resultadoLento,
        muestrasLentas,
        MAX_TELEMETRIA
    );

    snprintf(detalle, sizeof(detalle),
             "r_giro = %.6e m, muestras = %d",
             resultadoLento.rGiro,
             cantidadLenta);

    trabajo_registrar_prueba(
        &verificacion,
        resultadoLento.alcanzaL1 == 0 &&
        cantidadLenta > 0 &&
        resultadoLento.rGiro > resultadoLento.r0,
        "velocidad insuficiente: punto de giro",
        detalle
    );

    trabajo_registrar_prueba(
        &verificacion,
        cantidadLenta > 0 &&
        muestrasLentas[cantidadLenta - 1].distancia <=
        resultadoLento.rGiro + 1.0,
        "telemetria limitada al punto de giro",
        NULL
    );

    arbol_liberar(telemetria.abb);
    arbol_liberar(telemetria.avl);

    printf("\nResultado: %d/%d pruebas OK\n",
           verificacion.pruebasOk,
           verificacion.pruebasTotales);

    if (verificacion.pruebasFallidas > 0)
    {
        printf("Pruebas fallidas: %d\n",
               verificacion.pruebasFallidas);
    }
    else
    {
        printf("Todas las pruebas pasaron.\n");
    }

    printf("==========================================\n");

    return verificacion;
}
