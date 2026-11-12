#ifndef VALIDACIONES_H
#define VALIDACIONES_H

/*
=================================================
MODULO DE VALIDACIONES

Centraliza la lectura y validacion de datos
ingresados por el usuario.
=================================================
*/

/* Lee un numero positivo evitando letras,
   negativos y ceros */
double leerDoublePositivo(const char *mensaje);

/* Lee un numero dentro de un rango cerrado */
double leerDoubleEnRango(
    const char *mensaje,
    double minimo,
    double maximo
);

/* Lee un entero dentro de un rango cerrado */
int leerEnteroEnRango(
    const char *mensaje,
    int minimo,
    int maximo
);

#endif
