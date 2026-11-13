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

/* Lee un numero positivo mostrando un valor
   de ejemplo. Si se presiona Enter, usa el
   valor de ejemplo. */
double leerDoublePositivoConEjemplo(
    const char *mensaje,
    double ejemplo
)
{
    char linea[100];
    char *inicio;
    double valor;

    while(1)
    {
        printf("%s [ejemplo: %.0f]: ", mensaje, ejemplo);

        if(fgets(linea, sizeof(linea), stdin) == NULL)
        {
            return ejemplo;
        }

        inicio = linea;

        while(*inicio == ' ' || *inicio == '\t')
        {
            inicio++;
        }

        /* Enter sin datos: se usa el ejemplo */
        if(*inicio == '\n' || *inicio == '\r' || *inicio == '\0')
        {
            return ejemplo;
        }

        if(sscanf(inicio, "%lf", &valor) == 1 && valor > 0)
        {
            return valor;
        }

        printf("Error. Ingrese un numero positivo o presione Enter.\n");
    }
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
