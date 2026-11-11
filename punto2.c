#include <stdio.h>
#include "tli.h"

BalanceEnergetico punto2_energia(DatosTLI d, double h[], int *c)
{
    BalanceEnergetico b = {0, 0, 0, 0};

    if (d.masaNave <= 0 || d.velocidadInicial <= 0)
    {
        printf("\nError: los datos ingresados no son validos.\n");
        return b;
    }

    /* Calculo de energia cinetica */
    b.energiaCinetica = 0.5 * d.masaNave *
                        d.velocidadInicial * d.velocidadInicial;

    b.energiaTotal = b.energiaCinetica;

    printf("\n========== PUNTO 2: ENERGIA ==========\n");

    printf("\nDatos de Orion:\n");
    printf("Masa: %.2f kg\n", d.masaNave);
    printf("Velocidad inicial: %.2f m/s\n", d.velocidadInicial);

    printf("\nCalculo:\n");
    printf("Ki = 1/2 * m * v^2\n");
    printf("Ki = 1/2 * %.2f * (%.2f)^2\n",
           d.masaNave, d.velocidadInicial);

    printf("\nEnergia cinetica inicial: %.2f J\n",
           b.energiaCinetica);

    /* Guardar energia en el historial */
    if (h != NULL && c != NULL && *c < MAX_TELEMETRIA)
    {
        h[*c] = b.energiaCinetica;
        (*c)++;
    }

    printf("\nExplicacion teorica:\n");
    printf("La energia cinetica es la energia que posee\n");
    printf("un cuerpo debido a su movimiento.\n");
    printf("En este caso, representa la energia de Orion\n");
    printf("despues de la maniobra de inyeccion translunar.\n");

    printf("\n=====================================\n");

    return b;
}