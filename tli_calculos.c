#include <stdio.h>
#include <math.h>
#include "tli.h"

// Integrante 3: Punto 2
double calcular_energia_cinetica(double masa, double velocidad) {
    // K = 0.5 * m * v^2
    return 0.5 * masa * velocidad * velocidad;
}

// Integrante 4: Punto 3
double calcular_energia_potencial(double masa, double r) {
    // U = -G * M * m / r
    return -(G_CONST * M_TIERRA * masa) / r;
}

double calcular_trabajo_minimo(double masa, double altitud_leo) {
    // TODO (Integrante 4): Implementar calculo entre LEO y el punto de equilibrio Tierra-Luna
    return 0.0;
}