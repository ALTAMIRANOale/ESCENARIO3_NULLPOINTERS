#include <stdio.h>

#include "validaciones.h"

/*=========================================
  VALIDACION DE DATOS
========================================*/

/* Lee un numero positivo evitando letras,
   negativos y ceros */
double leerDoublePositivo(const char *mensaje)
{
    double valor;
    int valido;

    do
    {
        printf("%s", mensaje);

        valido = scanf("%lf", &valor);

        /* Limpia buffer */
        while(getchar() != '\n');

        if(valido != 1 || valor <= 0)
        {
            printf("Error. Ingrese un numero positivo.\n");
        }

    } while(valido != 1 || valor <= 0);

    return valor;
}

/* Lee un numero dentro de un rango cerrado */
double leerDoubleEnRango(
    const char *mensaje,
    double minimo,
    double maximo
)
{
    double valor;
    int valido;

    do
    {
        printf("%s", mensaje);

        valido = scanf("%lf", &valor);

        /* Limpia buffer */
        while(getchar() != '\n');

        if(valido != 1 || valor < minimo || valor > maximo)
        {
            printf(
                "Error. Ingrese un valor entre %.6g y %.6g.\n",
                minimo,
                maximo
            );
        }

    } while(valido != 1 || valor < minimo || valor > maximo);

    return valor;
}

/* Lee un entero dentro de un rango cerrado */
int leerEnteroEnRango(
    const char *mensaje,
    int minimo,
    int maximo
)
{
    int valor;
    int valido;

    do
    {
        printf("%s", mensaje);

        valido = scanf("%d", &valor);

        /* Limpia buffer */
        while(getchar() != '\n');

        if(valido != 1 || valor < minimo || valor > maximo)
        {
            printf(
                "Error. Ingrese un entero entre %d y %d.\n",
                minimo,
                maximo
            );
        }

    } while(valido != 1 || valor < minimo || valor > maximo);

    return valor;
}
