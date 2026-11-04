#include <stdio.h>
#include <math.h>
#include "tli.h"

// Integrante 2: Punto 1
void explicar_primera_ley_newton(void) {
    printf("\n--- 1. PRIMERA LEY DE NEWTON Y DESACELERACION GRAVITATORIA ---\n");
    printf("Una vez apagados los motores (ICPS desacoplado), la nave Orion continua en movimiento\n");
    printf("debido a su INERCIA. Ninguna fuerza impulsora externa actua sobre ella.\n");
    printf("Sin embargo, la fuerza gravitatoria de la Tierra ejerce una atraccion continua hacia el centro,\n");
    printf("actuando como una aceleracion negativa (desaceleracion progresiva) que reduce su velocidad.\n");
}

double calcular_desaceleracion_gravitatoria(double distancia) {
    // a = G * M_tierra / r^2
    return (G_CONST * M_TIERRA) / (distancia * distancia);
}

// Integrante 3: Punto 4
void justificar_diferencia_combustible(void) {
    printf("\n--- 4. JUSTIFICACION ENERGETICA DE COMBUSTIBLE (TIERRA-LUNA VS LUNA-TIERRA) ---\n");
    printf("Para salir de la Tierra, la nave debe escapar de un 'pozo gravitatorio' mucho mas profundo\n");
    printf("(la masa de la Tierra es ~81 veces mayor a la de la Luna). Esto requiere vencer la gravedad\n");
    printf("terrestre y alcanzar una velocidad de ~10.9 km/s desde LEO, exigiendo una cantidad masiva de propelente.\n");
    printf("Para el regreso desde la Luna, la velocidad de escape lunar es de solo ~2.38 km/s, requiriendo\n");
    printf("significativamente menos energia y combustible.\n");
}