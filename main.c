#include <stdio.h>
#include "tli.h"

int main(void) {
    DatosNave orion = {
        .masa_orion = 26000.0,      // kg
        .v_tli = 10900.0,           // m/s
        .altitud_leo = 192000.0     // m
    };

    printf("===================================================\n");
    printf("  SIMULADOR TLI: INYECCION TRANSLUNAR - NAVE ORION \n");
    printf("===================================================\n");

    // 1. Explicacion Ley de Newton (Integrante 2)
    explicar_primera_ley_newton();

    // 2. Energia Cinetica Inicial (Integrante 3)
    double k_inicial = calcular_energia_cinetica(orion.masa_orion, orion.v_tli);
    printf("\n--- 2. ENERGIA CINETICA INICIAL ---\n");
    printf("Energia cinetica (Ki): %.4e Joules\n", k_inicial);

    // 3. Trabajo adicional minimo (Integrante 4)
    printf("\n--- 3. TRABAJO MINIMO AL PUNTO DE EQUILIBRIO ---\n");
    printf("Calculo pendiente de integracion por Integrante 4...\n");

    // 4. Justificacion de Combustible (Integrante 3)
    justificar_diferencia_combustible();

    return 0;
}