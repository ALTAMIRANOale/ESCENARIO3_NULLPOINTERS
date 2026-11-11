#include <stdio.h>
#include <math.h>

#include "trabajo.h"
#include "arbol.h"

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

/* Potencial gravitatorio especifico (J/kg) */
double trabajo_potencial_especifico(double r)
{
    if (r <= 0.0)
    {
        return 0.0;
    }

    return -(G_CONST * M_TIERRA) / r;
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

/* Aceleracion efectiva sobre la linea Tierra-Luna (m/s^2) */
double trabajo_aceleracion_efectiva(double r)
{
    double omega2;

    if (r <= 0.0 || r >= DISTANCIA_TL)
    {
        return 0.0;
    }

    omega2 = trabajo_omega_cuadrada();

    return (G_CONST * M_TIERRA) / (r * r)
           - (G_CONST * M_LUNA) /
             ((DISTANCIA_TL - r) * (DISTANCIA_TL - r))
           - omega2 * r;
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

/*
 * Punto L1 del sistema rotante Tierra-Luna.
 * Se busca la raiz de la aceleracion efectiva
 * mediante biseccion recursiva.
 */
double trabajo_radio_L1_recursivo(
    double a,
    double b,
    double tolerancia,
    int *iteraciones
)
{
    double medio;

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

/* Clasifica el equilibrio segun la segunda derivada */
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

/* Nombre legible de un tipo de equilibrio */
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

/*=========================================
  CALCULO COMPLETO DEL PUNTO 3
=========================================*/

ResultadoTrabajo trabajo_calcular(const DatosTLI *datos)
{
    ResultadoTrabajo resultado = {0};

    resultado.r0 = R_TIERRA + H_LEO;
    resultado.rEquilibrio =
        trabajo_radio_equilibrio_gravitatorio();

    resultado.iteracionesL1 = 0;
    resultado.rL1 = trabajo_radio_L1_recursivo(
        L1_R_MIN,
        L1_R_MAX,
        L1_TOLERANCIA,
        &resultado.iteracionesL1
    );

    resultado.potencialEspecificoLEO =
        trabajo_potencial_especifico(resultado.r0);
    resultado.potencialEspecificoL1 =
        trabajo_potencial_efectivo_especifico(resultado.rL1);

    /*
     * Trabajo externo por unidad de masa como variacion
     * de energia potencial (W_ext = delta U).
     */
    resultado.trabajoEspecificoGravitatorio =
        trabajo_potencial_especifico(resultado.rEquilibrio) -
        trabajo_potencial_especifico(resultado.r0);

    resultado.trabajoEspecificoEfectivo =
        trabajo_potencial_efectivo_especifico(resultado.rL1) -
        trabajo_potencial_efectivo_especifico(resultado.r0);

    resultado.trabajoTotalEfectivo =
        resultado.trabajoEspecificoEfectivo * datos->masaNave;

    resultado.segundaDerivadaL1 =
        trabajo_segunda_derivada_efectiva(resultado.rL1);
    resultado.clasificacion =
        trabajo_clasificar_equilibrio(resultado.rL1);

    return resultado;
}

/*=========================================
  GENERACION DE TELEMETRIA
=========================================*/

/*
 * Genera muestras equidistantes en distancia entre
 * r0 (LEO) y rL1.
 *
 * La velocidad se obtiene a partir de la energia
 * especifica del marco rotante:
 *
 *   0.5*v^2 + U_eff/m = constante
 *
 * El tiempo es una estimacion radial acumulada.
 */
int trabajo_generar_muestras(
    const DatosTLI *datos,
    const ResultadoTrabajo *resultado,
    MuestraTelemetria muestras[],
    int maximo
)
{
    double energiaEspecifica;
    double tiempoAcumulado;
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

    energiaEspecifica =
        0.5 * datos->velocidadInicial * datos->velocidadInicial +
        trabajo_potencial_efectivo_especifico(resultado->r0);

    tiempoAcumulado = 0.0;

    for (i = 0; i < maximo; i++)
    {
        double fraccion;
        double r;
        double potencialEspecifico;
        double v2;
        double v;

        if (maximo == 1)
        {
            fraccion = 0.0;
        }
        else
        {
            fraccion = (double) i / (double) (maximo - 1);
        }

        r = resultado->r0 +
            fraccion * (resultado->rL1 - resultado->r0);

        potencialEspecifico =
            trabajo_potencial_efectivo_especifico(r);

        v2 = 2.0 * (energiaEspecifica - potencialEspecifico);

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
            datos->masaNave * potencialEspecifico;
        muestras[i].energiaTotal =
            muestras[i].energiaCinetica +
            muestras[i].energiaPotencial;

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

            if (vPromedio > 0.0)
            {
                tiempoAcumulado += dr / vPromedio;
            }

            muestras[i].tiempo = tiempoAcumulado;
        }
    }

    return maximo;
}

/*=========================================
  SALIDA POR PANTALLA
=========================================*/

void trabajo_mostrar_resultado(
    const ResultadoTrabajo *resultado,
    const DatosTLI *datos
)
{
    double diferencia;

    printf("\n========== PUNTO 3: TRABAJO Y EQUILIBRIO ==========\n");

    printf("\nDatos de la nave:\n");
    printf("Masa: %.2f kg\n", datos->masaNave);
    printf("Velocidad inicial: %.2f m/s\n", datos->velocidadInicial);

    printf("\nOrbita de estacionamiento (LEO):\n");
    printf("Radio inicial r0 = %.0f m\n", resultado->r0);
    printf("Energia potencial especifica: %.6e J/kg\n",
           resultado->potencialEspecificoLEO);

    printf("\nPunto de equilibrio gravitatorio Tierra-Luna:\n");
    printf("r_eq = %.6e m\n", resultado->rEquilibrio);
    printf("Energia potencial especifica: %.6e J/kg\n",
           trabajo_potencial_especifico(resultado->rEquilibrio));

    printf("\nPunto L1 (sistema Tierra-Luna en rotacion):\n");
    printf("r_L1 = %.6e m\n", resultado->rL1);
    printf("Iteraciones de biseccion recursiva: %d\n",
           resultado->iteracionesL1);
    printf("Aceleracion efectiva residual: %.6e m/s^2\n",
           trabajo_aceleracion_efectiva(resultado->rL1));
    printf("d2U/dr2 = %.6e J/kg/m^2\n",
           resultado->segundaDerivadaL1);
    printf("Clasificacion: %s\n",
           trabajo_nombre_equilibrio(resultado->clasificacion));

    printf("\nTrabajo externo minimo:\n");
    printf("Gravedad pura (LEO -> r_eq): %.6e J/kg\n",
           resultado->trabajoEspecificoGravitatorio);
    printf("Marco rotante (LEO -> L1):    %.6e J/kg\n",
           resultado->trabajoEspecificoEfectivo);
    printf("Equivalente en MJ/kg:         %.3f MJ/kg\n",
           resultado->trabajoEspecificoEfectivo / 1.0e6);
    printf("Trabajo total para %.0f kg:   %.6e J\n",
           datos->masaNave,
           resultado->trabajoTotalEfectivo);

    diferencia = resultado->trabajoEspecificoEfectivo -
                 resultado->trabajoEspecificoGravitatorio;

    printf("\nDiferencia entre ambos modelos: %.6e J/kg\n",
           diferencia);
    printf("(El marco rotante incluye la atraccion lunar\n");
    printf(" y el termino centrifugo del sistema.)\n");

    printf("\n====================================================\n");
}

/*=========================================
  PUNTO 3 DEL MENU
=========================================*/

void punto3_trabajo(
    DatosTLI datos,
    BalanceEnergetico balance,
    NodoArbol **raiz
)
{
    ResultadoTrabajo resultado;
    MuestraTelemetria muestras[MAX_TELEMETRIA];
    int cantidad;
    int i;

    (void)balance;

    resultado = trabajo_calcular(&datos);

    trabajo_mostrar_resultado(&resultado, &datos);

    if (raiz == NULL)
    {
        return;
    }

    /* Evita acumular muestras si se ejecuta varias veces */
    if (*raiz != NULL)
    {
        arbol_liberar(*raiz);
        *raiz = NULL;
    }

    cantidad = trabajo_generar_muestras(
        &datos,
        &resultado,
        muestras,
        MAX_TELEMETRIA
    );

    for (i = 0; i < cantidad; i++)
    {
        *raiz = arbol_insertar_abb(*raiz, &muestras[i]);
    }

    printf("\nTelemetria cargada en el arbol:\n");
    printf("Muestras: %d | Altura: %d | AVL: %s\n",
           cantidad,
           arbol_altura(*raiz),
           arbol_es_avl(*raiz) ? "si" : "no");
    printf("Use la opcion 7 del menu para ver el recorrido.\n");
}
