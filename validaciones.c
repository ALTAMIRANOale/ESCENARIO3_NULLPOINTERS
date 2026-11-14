#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "validaciones.h"

/*=========================================
  VALIDACION DE DATOS
========================================*/

/* Lee un numero positivo evitando letras,
   negativos, comas y ceros */
double leerDoublePositivo(const char *mensaje)
{
    char linea[100];
    char *inicio;
    double valor;

    while(1)
    {
        printf("%s", mensaje);

        if(fgets(linea, sizeof(linea), stdin) == NULL)
        {
            continue;
        }

        // Eliminar salto de línea
        linea[strcspn(linea, "\r\n")] = 0;

        inicio = linea;
        while(*inicio == ' ' || *inicio == '\t')
        {
            inicio++;
        }

        if(*inicio == '\0')
        {
            printf("Error. No se ingreso ningun valor.\n");
            continue;
        }

        // Validación estricta carácter por carácter
        int valido = 1;
        int puntos = 0;
        int digitos = 0;

        for (int i = 0; inicio[i] != '\0'; i++)
        {
            char c = inicio[i];
            if (isdigit(c)) {
                digitos++;
            } else if (c == '.') {
                puntos++;
                if (puntos > 1) {
                    valido = 0;
                    break;
                }
            } else {
                // Cualquier otro carácter (letras, comas ',', signos '-', etc.) invalida
                valido = 0;
                break;
            }
        }

        if (valido && digitos > 0) {
            valor = atof(inicio);
            if (valor > 0) {
                return valor;
            }
        }

        printf("Error. Ingrese un numero positivo valido (sin letras, comas ni negativos).\n");
        printf("Ejemplo de formato correcto: 26000\n");
    }
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

        // Eliminar salto de línea
        linea[strcspn(linea, "\r\n")] = 0;

        inicio = linea;
        while(*inicio == ' ' || *inicio == '\t')
        {
            inicio++;
        }

        /* Enter sin datos: se usa el ejemplo */
        if(*inicio == '\0')
        {
            return ejemplo;
        }

        // Validación estricta carácter por carácter
        int valido = 1;
        int puntos = 0;
        int digitos = 0;

        for (int i = 0; inicio[i] != '\0'; i++)
        {
            char c = inicio[i];
            if (isdigit(c)) {
                digitos++;
            } else if (c == '.') {
                puntos++;
                if (puntos > 1) {
                    valido = 0;
                    break;
                }
            } else {
                // Rechaza comas ',', signos '-', letras, etc.
                valido = 0;
                break;
            }
        }

        if (valido && digitos > 0) {
            valor = atof(inicio);
            if (valor > 0) {
                return valor;
            }
        }

        printf("Error. Formato incorrecto. Ingrese un numero positivo o presione Enter.\n");
        printf("Ejemplo de formato correcto: %.0f\n", ejemplo);
    }
}

/* Lee un numero dentro de un rango cerrado */
double leerDoubleEnRango(
    const char *mensaje,
    double minimo,
    double maximo
)
{
    char linea[100];
    char *inicio;
    double valor;

    while(1)
    {
        printf("%s", mensaje);

        if(fgets(linea, sizeof(linea), stdin) == NULL)
        {
            continue;
        }

        linea[strcspn(linea, "\r\n")] = 0;

        inicio = linea;
        while(*inicio == ' ' || *inicio == '\t')
        {
            inicio++;
        }

        if(*inicio == '\0') continue;

        int valido = 1;
        int puntos = 0;
        int digitos = 0;

        for (int i = 0; inicio[i] != '\0'; i++)
        {
            char c = inicio[i];
            if (isdigit(c)) {
                digitos++;
            } else if (c == '-') {
                if (i != 0) { valido = 0; break; } // El menos solo se permite al inicio si hay negativos, pero acá pedimos rango
            } else if (c == '.') {
                puntos++;
                if (puntos > 1) { valido = 0; break; }
            } else {
                valido = 0;
                break;
            }
        }

        if (valido && digitos > 0)
        {
            valor = atof(inicio);
            if (valor >= minimo && valor <= maximo)
            {
                return valor;
            }
        }

        printf("Error. Ingrese un valor entre %.6g y %.6g.\n", minimo, maximo);
    }
}

/* Lee un entero dentro de un rango cerrado */
int leerEnteroEnRango(
    const char *mensaje,
    int minimo,
    int maximo
)
{
    char linea[100];
    char *inicio;
    int valor;

    while(1)
    {
        printf("%s", mensaje);

        if(fgets(linea, sizeof(linea), stdin) == NULL)
        {
            continue;
        }

        linea[strcspn(linea, "\r\n")] = 0;

        inicio = linea;
        while(*inicio == ' ' || *inicio == '\t')
        {
            inicio++;
        }

        if(*inicio == '\0') continue;

        int valido = 1;
        int digitos = 0;

        for (int i = 0; inicio[i] != '\0'; i++)
        {
            char c = inicio[i];
            if (isdigit(c)) {
                digitos++;
            } else if (c == '-' && i == 0 && minimo < 0) {
                // Permitir signo negativo solo si el mínimo del rango lo permite
                continue;
            } else {
                valido = 0;
                break;
            }
        }

        if (valido && digitos > 0)
        {
            valor = atoi(inicio);
            if (valor >= minimo && valor <= maximo)
            {
                return valor;
            }
        }

        printf("Error. Ingrese un entero entre %d y %d.\n", minimo, maximo);
    }
}