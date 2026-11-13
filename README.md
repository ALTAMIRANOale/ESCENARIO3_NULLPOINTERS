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
| Axel | **Punto 3 - Trabajo y equilibrio + telemetría ABB / AVL** |

---

## 3. Módulos del proyecto

| Archivo | Responsabilidad |
|---|---|
| `main.c` | Punto de entrada y menú principal |
| `tli.h` | Constantes, estructuras y prototipos generales |
| `punto2.c` | Punto 2 - Energía cinética |
| `punto4.c` | Punto 4 - Consumo de combustible |
| `trabajo.c` / `trabajo.h` | Punto 3 - Potencial, equilibrio, trabajo, telemetría y autopruebas |
| `arbol.c` / `arbol.h` | ABB y AVL de telemetría, búsquedas y comparación de costos |
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
gcc -std=c11 -Wall -Wextra -o tli.exe main.c punto2.c punto4.c trabajo.c arbol.c validaciones.c tli_calculos.c tli_explicaciones.c -lm
```

Requiere MinGW-w64 (por ejemplo, w64devkit) con `gcc` en el `PATH`.

---

## 5. Menú del Punto 3

| Opción | Acción |
|---|---|
| `4` | Calcula trabajo, equilibrio, telemetría y comparación ABB / AVL |
| `7` | Muestra el detalle de los árboles ABB y AVL |
| `8` | Ejecuta las autopruebas contra los valores de control |

La opción `4` construye simultáneamente:

- un **ABB simple** con las muestras en orden creciente de distancia;
- un **AVL real** con las mismas muestras;
- las métricas de búsqueda de ambos árboles.

El ABB se conserva para el recorrido inorden. El AVL se conserva para comparar
altura y comparaciones. Ambos viven en una única estructura `TelemetriaPunto3`,
por lo que no se duplica lógica ni se recalculan los datos.

---

## 6. Relación entre trabajo, equilibrio y árboles

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
llegar hasta él y, en paralelo, almacena la telemetría en un ABB y en un AVL
para comparar el costo de búsqueda `O(n)` contra `O(log n)`.

---

## 7. Modelo físico y fórmulas

### Potencial gravitatorio total

```text
U_grav(r) = -G·M_Tierra/r - G·M_Luna/(D - r)
```

### Potencial efectivo en el marco rotante

```text
U_eff(r) = U_grav(r) - 0.5·ω²·r²
ω² = G·(M_Tierra + M_Luna)/D³
```

### Aceleración efectiva

Convención: positiva hacia la Luna (r creciente).

```text
F_eff(r) = -G·M_Tierra/r² + G·M_Luna/(D - r)² + ω²·r
```

### Segunda derivada del potencial efectivo

```text
d²U_eff/dr² = -2·G·M_Tierra/r³ - 2·G·M_Luna/(D - r)³ - ω²
```

### Trabajo externo

```text
W_ext = ΔU = ∫ -F_eff dr
```

El programa compara dos caminos:

1. **Gravedad total:** LEO → `r_eq`, usando el potencial gravitatorio total.
2. **Marco rotante:** LEO → L1, usando el potencial efectivo.

Además, integra numéricamente `-F_eff` con el método de Simpson y calcula el
error relativo contra la variación de potencial. Para el escenario nominal, el
error es del orden de `4,8e-8`.

### Clasificación del equilibrio

- `d²U/dr² > 0`: equilibrio estable (mínimo del potencial).
- `d²U/dr² < 0`: equilibrio inestable (máximo del potencial).
- `d²U/dr² ≈ 0`: equilibrio neutro.

`r_eq` se clasifica con el potencial gravitatorio total. L1 se clasifica con el
potencial efectivo. Ambos resultan inestables en este modelo.

### Supuestos

- Movimiento sobre la línea Tierra - Luna.
- Marco rotante con velocidad angular constante.
- Solo se consideran Tierra y Luna.
- El trabajo se calcula cuasiestáticamente como variación de potencial.
- El tiempo de telemetría es una estimación radial, no una integración orbital completa.

---

## 8. Comparación ABB vs AVL

Las muestras se insertan en orden creciente de distancia. Eso degenera el ABB
en una lista, mientras que el AVL se mantiene balanceado mediante rotaciones.

Resultado con 100 muestras:

| Métrica | ABB | AVL |
|---|---|---|
| Nodos | 100 | 100 |
| Altura | 100 | 7 |
| Cumple AVL | No | Sí |
| Comparaciones promedio | 50,50 | 5,80 |
| Comparaciones máximas | 100 | 7 |
| Complejidad teórica | `O(n)` | `O(log n)` |

La altura teórica máxima esperada para un AVL de 100 nodos es aproximadamente
`9,6`, por lo que la altura 7 queda dentro de la cota.

---

## 9. Valores de control

| Resultado | Valor esperado |
|---|---|
| Radio inicial LEO | `r0 = 6.563.000 m` |
| Punto de equilibrio gravitatorio | `r_eq ≈ 3,456724e8 m` |
| Punto L1 (sistema rotante) | `r_L1 ≈ 3,256346e8 m` |
| Trabajo externo específico LEO - L1 | `W_ext/m ≈ 5,906011e7 J/kg` |
| Trabajo gravitatorio total LEO - r_eq | `W_grav/m ≈ 5,946482e7 J/kg` |
| Error relativo de integración | `< 1e-6` |
| Desviación máxima de energía | `< 1e-9` |

Las autopruebas de la opción `8` verifican estos valores, la clasificación del
equilibrio, la conservación de energía, la altura del AVL, el orden del ABB y la
reducción de comparaciones.

