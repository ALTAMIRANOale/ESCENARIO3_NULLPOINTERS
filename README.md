# ESCENARIO 3 - NULL POINTERS

## Simulación TLI (Tierra - Luna) - Trabajo Final Integrador

**Materia:** Algoritmos y Estructuras de Datos
**Escenario:** Inyección Translunar (TLI) e inercia en trayectoria libre
**Lenguaje:** C (C11)

---

## 1. Datos del escenario

| Magnitud | Valor |
|---|---|
| Masa de la nave | 26.000 kg |
| Velocidad de inyección translunar | 10.900 m/s |
| Distancia Tierra - Luna | 384.000 km |
| Altitud de la órbita de estacionamiento (LEO) | 192.000 m |

Constantes físicas utilizadas (definidas en `tli.h`):

- `G = 6,67430e-11 N·m²/kg²`
- `M_Tierra = 5,972e24 kg`
- `M_Luna = 7,342e22 kg`
- `R_Tierra = 6.371.000 m`

---

## 2. Integrantes y aportes

| Integrante | Aporte |
|---|---|
| Altamirano | Punto 1 - Primera Ley de Newton y desaceleración gravitatoria |
| Agustín | Punto 2 - Balance energético / Punto 4 - Combustible |
| Dana | Menú, estructura general y bitácora |
| Axel | **Punto 3 - Trabajo y equilibrio + árbol AVL de telemetría** |

---

## 3. Módulos del proyecto

| Archivo | Responsabilidad |
|---|---|
| `main.c` | Punto de entrada y menú principal |
| `tli.h` | Constantes, estructuras y prototipos generales |
| `punto2.c` | Punto 2 - Energía cinética |
| `trabajo.c` / `trabajo.h` | Punto 3 - Energía potencial, punto de equilibrio, trabajo externo |
| `arbol.c` / `arbol.h` | Árbol de telemetría y reporte de equilibrio (ABB; AVL en la siguiente etapa) |
| `validaciones.c` / `validaciones.h` | Validación centralizada de entradas |
| `tli_calculos.c` | Cálculos auxiliares legados |
| `tli_explicaciones.c` | Explicaciones teóricas legadas |

---

## 4. Compilación

### Opción A - Makefile (recomendado)

```sh
mingw32-make
mingw32-make run
mingw32-make clean
```

### Opción B - script de Windows

```bat
build.bat
```

### Opción C - compilación manual

```sh
gcc -std=c11 -Wall -Wextra -o tli.exe main.c punto2.c trabajo.c arbol.c validaciones.c tli_calculos.c tli_explicaciones.c -lm
```

Requiere MinGW-w64 (por ejemplo, w64devkit) con `gcc` en el `PATH`.

---

## 5. Relación entre trabajo, equilibrio y árboles

El Punto 3 trabaja un mismo concepto desde dos perspectivas:

| Concepto | Equilibrio físico (TLI) | Equilibrio estructural (árbol) |
|---|---|---|
| Punto de equilibrio | Fuerza neta igual a cero (L1) | Factor de balance igual a cero |
| Estabilidad | Segunda derivada del potencial | `abs(BF) <= 1` en todos los nodos |
| Estado inestable | Máximo del potencial efectivo | Árbol degenerado, altura `O(n)` |
| Restauración | Fuerza gravitatoria restauradora | Rotaciones LL / RR / LR / RL |
| Costo | Trabajo externo para corregir la órbita | Comparaciones para ubicar un dato |

El programa calcula el punto de equilibrio gravitatorio y el punto L1 del
sistema Tierra - Luna, el trabajo externo por unidad de masa necesario para
llegar hasta él y, en paralelo, almacena la telemetría en un árbol AVL para
comparar el costo de búsqueda `O(n)` (ABB desbalanceado) contra `O(log n)`
(AVL balanceado).

---

## 6. Valores de control

| Resultado | Valor esperado |
|---|---|
| Radio inicial LEO | `r0 = 6.563.000 m` |
| Punto de equilibrio gravitatorio | `r ≈ 3,46e8 m` |
| Punto L1 (sistema rotante) | `r ≈ 3,26e8 m` |
| Trabajo externo específico LEO - L1 | `W_ext/m ≈ 5,9e7 J/kg` |

---

## 7. Estado del proyecto

- Punto 1: pendiente (Altamirano).
- Punto 2: implementado.
- Punto 3: trabajo, equilibrio y árbol de telemetría implementados (AVL pendiente).
- Punto 4: pendiente (Agustín).
