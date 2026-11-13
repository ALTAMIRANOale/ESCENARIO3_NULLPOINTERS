#include <stdio.h>
#include <math.h>
#include "tli.h"
#include "trabajo.h"

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
    // Trabajo externo minimo entre LEO y L1 en el marco rotante:
    // W = m * (U_eff(L1) - U_eff(LEO))
    ResultadoBusquedaL1 busqueda;
    double r0;
    double rL1;
    double u0;
    double uL1;

    if (masa <= 0.0 || altitud_leo < 0.0) {
        return 0.0;
    }

    r0 = R_TIERRA + altitud_leo;

    busqueda = trabajo_buscar_L1(L1_R_MIN, L1_R_MAX, L1_TOLERANCIA);

    if (busqueda.valido && busqueda.radio > r0) {
        rL1 = busqueda.radio;
    } else {
        rL1 = trabajo_radio_equilibrio_gravitatorio();
    }

    u0 = trabajo_potencial_efectivo_especifico(r0);
    uL1 = trabajo_potencial_efectivo_especifico(rL1);

    return masa * (uL1 - u0);
}
