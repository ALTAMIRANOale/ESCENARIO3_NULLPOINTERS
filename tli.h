#ifndef TLI_H
#define TLI_H

// Constantes físicas del problema
#define G_CONST 6.67430e-11        // N m^2 / kg^2
#define M_TIERRA 5.972e24          // kg
#define M_LUNA 7.342e22            // kg
#define R_TIERRA 6371000.0         // m (Radio terrestre)
#define DISTANCIA_TL 384000000.0   // m (Tierra-Luna)

// Estructura para almacenar los datos de entrada
typedef struct {
    double masa_orion;      // kg
    double v_tli;           // m/s
    double altitud_leo;     // m
} DatosNave;

// Prototipos de funciones - Integrante 2
void explicar_primera_ley_newton(void);
double calcular_desaceleracion_gravitatoria(double distancia);

// Prototipos de funciones - Integrante 3
double calcular_energia_cinetica(double masa, double velocidad);
void justificar_diferencia_combustible(void);

// Prototipos de funciones - Integrante 4
double calcular_energia_potencial(double masa, double r);
double calcular_trabajo_minimo(double masa, double altitud_leo);

#endif // TLI_H

// Nodo para la lista enlazada que registrará los tramos de la trayectoria y su desaceleracion
typedef struct NodoTrayectoria {
    double distancia_centro;        // Distancia desde el centro de la Tierra (metros)
    double desaceleracion;          // Aceleración de frenado en ese punto (m/s^2)
    struct NodoTrayectoria* siguiente; // Puntero al siguiente nodo
} NodoTrayectoria;

// Tus prototipos de funciones
void explicar_primera_ley_newton(void);
double calcular_desaceleracion_gravitatoria(double distancia);
NodoTrayectoria* crear_nodo(double distancia);
void insertar_tramo(NodoTrayectoria** cabeza, double distancia);
void mostrar_trayectoria_lista(NodoTrayectoria* cabeza);
void liberar_lista(NodoTrayectoria* cabeza);