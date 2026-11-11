
#include <stdio.h>
#include <stdlib.h>
#include "TLI.h"

/* =====================================================
   FUNCIONES AUXILIARES
   ===================================================== */

double leerDoublePositivo(const char mensaje[])
{
    double valor;

    do
    {
        printf("%s", mensaje);

        if (scanf("%lf", &valor) != 1)
        {
            printf("Entrada invalida. Ingrese un numero.\n");

            while (getchar() != '\n');
            valor = -1;
        }
        else if (valor <= 0)
        {
            printf("El valor debe ser mayor que cero.\n");
        }

    } while (valor <= 0);

    return valor;
}

void cargarDatos(DatosTLI *datos)
{
    printf("\n========== CARGA DE DATOS ==========\n");

    datos->masaNave = leerDoublePositivo(
        "Ingrese la masa de la nave (kg): "
    );

    datos->velocidadInicial = leerDoublePositivo(
        "Ingrese la velocidad inicial (m/s): "
    );

    datos->cargado = 1;

    printf("\nDatos cargados correctamente.\n");
    printf("Masa: %.2f kg\n", datos->masaNave);
    printf("Velocidad inicial: %.2f m/s\n",
           datos->velocidadInicial);

    printf("====================================\n");
}

/* =====================================================
   STUBS TEMPORALES DE LOS PUNTOS DE LOS COMPAÑEROS
   ===================================================== */

/*
   Estos stubs se mantienen solamente hasta que se
   incorporen los archivos reales de los puntos 1 y 3.
*/

void punto1_leyNewton(DatosTLI datos, NodoEvento *bitacora)
{
    (void)datos;
    (void)bitacora;

    printf("\n[Punto 1: pendiente de integrar]\n");
}

void punto3_trabajo(
    DatosTLI datos,
    BalanceEnergetico balance,
    NodoArbol **arbol
)
{
    (void)datos;
    (void)balance;
    (void)arbol;

    printf("\n[Punto 3: pendiente de integrar]\n");
}

/* =====================================================
   PROGRAMA PRINCIPAL
   ===================================================== */

int main(void)
{
    DatosTLI datos = {0, 0, 0};
    BalanceEnergetico balance = {0, 0, 0, 0};

    double historialEnergia[MAX_TELEMETRIA] = {0};
    int cantidadHistorial = 0;

    NodoEvento *bitacora = NULL;
    NodoArbol *arbol = NULL;

    int opcion;

    do
    {
        printf("\n");
        printf("========================================\n");
        printf("       SIMULACION TIERRA-LUNA (TLI)\n");
        printf("========================================\n");
        printf("1. Cargar datos de la nave\n");
        printf("2. Punto 1 - Ley de Newton\n");
        printf("3. Punto 2 - Energia\n");
        printf("4. Punto 3 - Trabajo\n");
        printf("5. Punto 4 - Combustible\n");
        printf("6. Mostrar bitacora\n");
        printf("0. Salir\n");
        printf("========================================\n");
        printf("Seleccione una opcion: ");

        if (scanf("%d", &opcion) != 1)
        {
            printf("\nEntrada invalida. Ingrese una opcion numerica.\n");

            while (getchar() != '\n');

            opcion = -1;
            continue;
        }

        switch (opcion)
        {
            case 1:
                cargarDatos(&datos);
                break;

            case 2:
                /* Punto 1 - Ley de Newton */
                if (!datos.cargado)
                {
                    printf("\nPrimero debe cargar los datos.\n");
                    break;
                }

                punto1_leyNewton(datos, &bitacora);
                break;

            case 3:
                /* Punto 2 - Energia */
                if (!datos.cargado)
                {
                    printf("\nPrimero debe cargar los datos.\n");
                    break;
                }

                balance = punto2_energia(
                    datos,
                    historialEnergia,
                    &cantidadHistorial
                );
                break;

            case 4:
                /* Punto 3 - Trabajo */
                if (!datos.cargado)
                {
                    printf("\nPrimero debe cargar los datos.\n");
                    break;
                }

                punto3_trabajo(datos, balance, &arbol);
                break;

            case 5:
                /* Punto 4 - Combustible */
                if (!datos.cargado)
                {
                    printf("\nPrimero debe cargar los datos.\n");
                    break;
                }

                punto4_combustible(datos, balance);
                break;

            case 6:
                printf("\n========== BITACORA ==========\n");

                if (bitacora == NULL)
                {
                    printf("La bitacora esta vacia.\n");
                }
                else
                {
                    /*
                       Aqui se debe llamar a la funcion que
                       implementa el Punto 1 para mostrar
                       la bitacora, cuando este integrada.
                    */
                    printf("La bitacora contiene eventos.\n");
                }

                printf("==============================\n");
                break;

            case 0:
                printf("\nFinalizando simulacion...\n");
                break;

            default:
                printf("\nOpcion incorrecta. Intente nuevamente.\n");
                break;
        }

    } while (opcion != 0);

    return 0;
}
