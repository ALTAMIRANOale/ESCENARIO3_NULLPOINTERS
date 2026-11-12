#include <stdio.h>
#include <stdlib.h>
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


void explicar_primera_ley_newton(void) {
    printf("\n===================================================\n");
    printf("  1. PRIMERA LEY DE NEWTON Y DESACELERACION TLI   \n");
    printf("===================================================\n");
    printf("-> Una vez apagados los propulsores de la etapa ICPS y desacoplada,\n");
    printf("   la nave Orion (m = 26.000 kg) se desplaza por el espacio debido a su INERCIA\n");
    printf("   (Principio de Inercia: un cuerpo mantiene su estado de movimiento a menos que\n");
    printf("   una fuerza externa actue sobre el).\n");
    printf("-> Sin embargo, la fuerza gravitatoria de la Tierra actua de manera opuesta al\n");
    printf("   vector velocidad, generando una DESACELERACION PROGRESSIVA que reduce\n");
    printf("   lentamente su velocidad conforme aumenta la distancia 'r'.\n");
}

// Calcula la desaceleración gravitatoria en un punto dado: a = (G * M) / r^2
double calcular_desaceleracion_gravitatoria(double distancia) {
    if (distancia <= 0) return 0.0;
    return (G_CONST * M_TIERRA) / (distancia * distancia);
}

// Crea un nuevo nodo para la lista enlazada de tramos
NodoTrayectoria* crear_nodo(double distancia) {
    Nodo* nuevo = (Nodo*) malloc(sizeof(NodoTrayectoria));
    if (nuevo == NULL) {
        printf("Error: No se pudo asignar memoria para el nodo.\n");
        exit(1);
    }
    nuevo->distancia_centro = distancia;
    nuevo->desaceleracion = calcular_desaceleracion_gravitatoria(distancia);
    nuevo->siguiente = NULL;
    return nuevo;
}