#include <stdio.h>
#include <stddef.h>

void punto1_leyNewton(DatosTLI d, NodoEvento **l) {
    (void)d; (void)l;
    printf("\n[Axel todavia no subio el Punto 1]\n");
}

BalanceEnergetico punto2_energia(DatosTLI d, double h[], int *c) {
    (void)d; (void)h; (void)c;
    BalanceEnergetico b = {0, 0, 0, 0};
    printf("\n[Agustin todavia no subio el Punto 2]\n");
    return b;
}

void punto3_trabajo(DatosTLI d, BalanceEnergetico b, NodoArbol **r) {
    (void)d; (void)b; (void)r;
    printf("\n[Dana todavia no subio el Punto 3]\n");
}

void punto4_combustible(DatosTLI d, BalanceEnergetico b) {
    (void)d; (void)b;
    printf("\n[Agustin todavia no subio el Punto 4]\n");
}

void agregarEvento(NodoEvento **l, const char *x, double t) { (void)l; (void)x; (void)t; }
void mostrarBitacora(NodoEvento *l) { (void)l; }
void liberarBitacora(NodoEvento *l) { (void)l; }
NodoArbol* crearNodoArbol(const char *n, double v) { (void)n; (void)v; return NULL; }
NodoArbol* insertarArbol(NodoArbol *r, const char *n, double v) { (void)n; (void)v; return r; }
void evaluarArbol(NodoArbol *r) { (void)r; }
void liberarArbol(NodoArbol *r) { (void)r; }