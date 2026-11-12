/*
=================================================
PROYECTO TLI - SIMULACION TIERRA-LUNA

Responsabilidades actuales:
- Menu principal
- Validacion de entradas
- Carga de datos

Modulos integrados:
- Punto 1 (Altamirano): ley de Newton y
  desaceleracion por tramos en
  tli_explicaciones.c
- Punto 2 (Agustin): implementado en punto2.c
- Punto 3 (Axel): trabajo, equilibrio,
  telemetria y comparacion ABB / AVL
  en trabajo.c / arbol.c
- Punto 4 (Agustin): implementado en punto4.c

Modulos pendientes:
- Lista enlazada de bitacora

Las constantes fisicas se encuentran definidas
en tli.h y se cargan automaticamente.
=================================================
*/

#include <stdio.h>
#include <stdlib.h>

#include "tli.h"
#include "validaciones.h"
#include "trabajo.h"
#include "arbol.h"

/*=========================================
  CARGA DE DATOS
=========================================*/

/* Carga solamente los datos variables de
   la simulacion */
void cargarDatos(DatosTLI *datos)
{
    printf("\n====================================\n");
    printf("      CARGA DE DATOS DE MISION\n");
    printf("====================================\n");

    printf("Constantes fisicas cargadas:\n");
    printf("Masa Tierra........... %.3e kg\n", M_TIERRA);
    printf("Masa Luna............. %.3e kg\n", M_LUNA);
    printf("Radio Tierra.......... %.0f m\n", R_TIERRA);
    printf("Distancia Tierra-Luna. %.0f m\n", DISTANCIA_TL);

    printf("\n");

    datos->masaNave =
        leerDoublePositivo(
            "Ingrese masa de la nave (kg): "
        );

    datos->velocidadInicial =
        leerDoublePositivo(
            "Ingrese velocidad inicial (m/s): "
        );

    datos->cargado = 1;

    printf("\nDatos cargados correctamente.\n");
}

/*=========================================
  PUNTO 1 - LEY DE NEWTON
=========================================*/

/*
 * Explica la primera ley de Newton y calcula
 * la desaceleracion gravitatoria por tramos
 * entre la orbita LEO y el punto L1.
 */
void punto1_leyNewton(DatosTLI d, NodoEvento **l)
{
    NodoTrayectoria *trayectoria = NULL;
    ResultadoBusquedaL1 busquedaL1;
    double r0;
    double rL1;
    double aLEO;
    double aL1;
    double fuerzaLEO;
    const int tramos = 10;
    int i;

    if (d.masaNave <= 0.0 || d.velocidadInicial <= 0.0)
    {
        printf("\nError: los datos de la nave no son validos.\n");
        return;
    }

    explicar_primera_ley_newton();

    r0 = R_TIERRA + H_LEO;

    busquedaL1 = trabajo_buscar_L1(
        L1_R_MIN,
        L1_R_MAX,
        L1_TOLERANCIA
    );

    if (busquedaL1.valido && busquedaL1.radio > r0)
    {
        rL1 = busquedaL1.radio;
    }
    else
    {
        rL1 = trabajo_radio_equilibrio_gravitatorio();

        printf(
            "\nAdvertencia: no se pudo validar L1; "
            "se usa r_eq como referencia.\n"
        );
    }

    aLEO = calcular_desaceleracion_gravitatoria(r0);
    aL1 = calcular_desaceleracion_gravitatoria(rL1);
    fuerzaLEO = d.masaNave * aLEO;

    printf("\nDatos de la nave:\n");
    printf("Masa: %.2f kg\n", d.masaNave);
    printf("Velocidad inicial: %.2f m/s\n", d.velocidadInicial);
    printf("Cantidad de movimiento (p = m*v): %.6e kg*m/s\n",
           d.masaNave * d.velocidadInicial);
    printf("Radio de LEO (r0): %.0f m\n", r0);
    printf("Radio de L1 (rL1): %.0f m\n", rL1);

    printf("\nDesaceleracion gravitatoria terrestre (a = G*M/r^2):\n");
    printf("En LEO: %.6f m/s^2\n", aLEO);
    printf("En L1:  %.6f m/s^2\n", aL1);
    printf("Fuerza gravitatoria en LEO: %.6e N\n", fuerzaLEO);
    printf("La aceleracion disminuye con el cuadrado de la distancia:\n");
    printf("la desaceleracion es cada vez menor a medida que la nave se aleja.\n");

    for (i = 0; i <= tramos; i++)
    {
        double r;

        r = r0 + (rL1 - r0) * (double) i / (double) tramos;

        insertar_tramo(&trayectoria, r);
    }

    mostrar_trayectoria_lista(trayectoria);

    liberar_lista(trayectoria);
    trayectoria = NULL;

    printf("\nLa lista enlazada almaceno %d tramos entre LEO y L1.\n",
           tramos + 1);
    printf("La nave mantiene su inercia; solo la gravedad terrestre la frena.\n");

    if (l != NULL)
    {
        agregarEvento(l, "Punto 1 - Ley de Newton ejecutado", 0.0);
    }
}

/*=========================================
  BITACORA (LISTA ENLAZADA)
=========================================*/

/*
 * Implementacion temporal.
 * Aqui ira la logica real de la bitacora.
 */
void agregarEvento(
    NodoEvento **l,
    const char *x,
    double t
)
{
    (void)l;
    (void)x;
    (void)t;
}

/*
 * Mostrar eventos registrados.
 * Pendiente de implementacion.
 */
void mostrarBitacora(NodoEvento *l)
{
    (void)l;

    printf("\n[Bitacora aun no implementada]\n");
}

/*
 * Liberar memoria de la lista.
 * Pendiente de implementacion.
 */
void liberarBitacora(NodoEvento *l)
{
    (void)l;
}

/*=========================================
  MAIN
=========================================*/

int main(void)
{
    /* Datos generales de la simulacion */
    DatosTLI datos = {0};

    /* Resultado del balance energetico */
    BalanceEnergetico balance = {0};

    /* Lista de eventos */
    NodoEvento *bitacora = NULL;

    /* Telemetria y arboles del Punto 3 */
    TelemetriaPunto3 telemetria = {0};

    /* Historial de energias calculadas */
    double historialEnergia[MAX_TELEMETRIA];

    /* Cantidad de energias almacenadas */
    int cantidadHistorial = 0;

    int opcion;

    do
    {
        printf("\n");
        printf("====================================\n");
        printf("   SIMULADOR TLI TIERRA - LUNA\n");
        printf("====================================\n");
        printf("1. Cargar datos de la nave\n");
        printf("2. Punto 1 - Ley de Newton\n");
        printf("3. Punto 2 - Energia\n");
        printf("4. Punto 3 - Trabajo\n");
        printf("5. Punto 4 - Combustible\n");
        printf("6. Mostrar bitacora\n");
        printf("7. Punto 3 - Arboles ABB y AVL\n");
        printf("8. Punto 3 - Verificar resultados\n");
        printf("0. Salir\n");
        printf("====================================\n");
        printf("Seleccione una opcion: ");

        if(scanf("%d", &opcion) != 1)
        {
            while(getchar() != '\n');

            printf("\nOpcion invalida.\n");
            continue;
        }

        while(getchar() != '\n');

        switch(opcion)
        {
            case 1:

                /* Carga inicial de datos */
                cargarDatos(&datos);

                break;

            case 2:

                /* Ejecuta Ley de Newton */
                if(!datos.cargado)
                {
                    printf(
                        "\nPrimero debe cargar "
                        "los datos.\n"
                    );
                    break;
                }

                punto1_leyNewton(
                    datos,
                    &bitacora
                );

                break;

            case 3:

                /* Balance energetico */
                if(!datos.cargado)
                {
                    printf(
                        "\nPrimero debe cargar "
                        "los datos.\n"
                    );
                    break;
                }

                balance =
                    punto2_energia(
                        datos,
                        historialEnergia,
                        &cantidadHistorial
                    );

                break;

            case 4:

                /* Trabajo mecanico */
                if(!datos.cargado)
                {
                    printf(
                        "\nPrimero debe cargar "
                        "los datos.\n"
                    );
                    break;
                }

                punto3_trabajo(
                    datos,
                    balance,
                    &telemetria
                );

                break;

            case 5:

                /* Calculo de combustible */
                if(!datos.cargado)
                {
                    printf(
                        "\nPrimero debe cargar "
                        "los datos.\n"
                    );
                    break;
                }

                punto4_combustible(
                    datos,
                    balance
                );

                break;

            case 6:

                /* Mostrar bitacora */
                mostrarBitacora(bitacora);

                break;

            case 7:

                /* Arboles ABB y AVL del Punto 3 */
                if(!datos.cargado)
                {
                    printf(
                        "\nPrimero debe cargar "
                        "los datos.\n"
                    );
                    break;
                }

                punto3_mostrar_arboles(&telemetria);

                break;

            case 8:

                /* Autoverificacion del Punto 3 */
                trabajo_verificar();

                break;

            case 0:

                printf(
                    "\nFinalizando programa...\n"
                );

                break;

            default:

                printf(
                    "\nOpcion no valida.\n"
                );
        }

    } while(opcion != 0);

    /* Liberacion final de memoria */
    liberarBitacora(bitacora);
    punto3_liberar_telemetria(&telemetria);

    return 0;
}
