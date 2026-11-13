# ESCENARIO 3 - NULL POINTERS

## Simulación TLI (Tierra - Luna) - Trabajo Final Integrador

**Materia:** Algoritmos y Estructuras de Datos
**Comisión:** K1.3 "C"
**Escenario:** Inyección Translunar (TLI) e inercia en trayectoria libre
**Lenguaje:** C (C11)
**Repositorio:** [ALTAMIRANOale/ESCENARIO3_NULLPOINTERS](https://github.com/ALTAMIRANOale/ESCENARIO3_NULLPOINTERS)

---

## 1. Integrantes y comisión

**Comisión:** K1.3 "C"

| Integrante |
|---|---|
| Altamirano Alejandro 
| Baez Dana Paola 
| Lazarczuk Agustín 
| Ferreira Axel 

---

## 2. Escenario Artemis y reglas del juego

### 2.1 Situación - Problema 3: Inyección Translunar (TLI)

La nave Orion se encuentra en una órbita baja terrestre (LEO), a una altitud
de `h = 192.000 m`. La etapa superior ICPS ejecuta la maniobra de Inyección
Translunar (TLI) y la impulsa hasta una velocidad de inyección de
`v_TLI = 10.900 m/s`.

Inmediatamente después del encendido, la etapa ICPS se desacopla y los motores
principales se apagan por completo. A partir de ese momento la nave no tiene
propulsión y entra en una trayectoria de retorno libre (Free Return
Trajectory) hacia la Luna, ubicada a una distancia media Tierra - Luna de
`D = 384.000.000 m`. La masa de Orion es `m = 26.000 kg`.

De acuerdo con la 1.ª Ley de Newton (Ley de Inercia), la nave mantiene su
estado de movimiento mientras la fuerza gravitatoria terrestre actúa en
sentido opuesto a la velocidad, produciendo una desaceleración progresiva que
disminuye a medida que la distancia aumenta.

### 2.2 Reglas del juego y restricciones del modelo

- Solo se consideran la Tierra y la Luna.
- El movimiento se analiza sobre la línea que une ambos cuerpos.
- La Luna se modela en un marco rotante con velocidad angular constante.
- Después de la TLI no hay propulsión: la nave se mueve por inercia.
- La masa de la nave se mantiene constante durante la simulación.
- El trabajo se calcula cuasiestáticamente como variación de energía potencial.
- El tiempo de telemetría es una estimación radial, no una integración orbital completa.
- Los datos de entrada deben ser positivos; el programa permite usar los valores de ejemplo.

### 2.3 Consignas que responde el programa

1. Explicar el comportamiento de la nave por la 1.ª Ley de Newton y describir la desaceleración gravitatoria terrestre.
2. Calcular la energía cinética inicial de Orion para `v = 10.900 m/s`.
3. Determinar el trabajo externo mínimo por unidad de masa para pasar de LEO al punto de equilibrio gravitatorio Tierra - Luna.
4. Justificar energéticamente por qué se requiere más combustible para viajar de la Tierra a la Luna que para el regreso Luna - Tierra.

---

## 3. Datos del escenario

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

## 4. Módulos del proyecto

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

## 5. Funcionamiento general

El programa es un simulador de consola. Al iniciar muestra un menú y mantiene
los datos de la misión y la bitácora en memoria hasta que se elige la opción
`0` (Salir).

| Opción | Acción |
|---|---|
| `1` | Carga la masa de la nave y la velocidad inicial. Permite aceptar los valores de ejemplo con Enter. |
| `2` | Explica la 1.ª Ley de Newton y calcula la desaceleración gravitatoria por tramos entre LEO y L1. |
| `3` | Calcula la energía cinética inicial de Orion. |
| `4` | Calcula trabajo, equilibrio gravitatorio, telemetría y comparación ABB / AVL. |
| `5` | Presenta la justificación energética del consumo de combustible. |
| `6` | Muestra la bitácora de eventos registrados durante la sesión. |
| `7` | Muestra el detalle de los árboles ABB y AVL construidos con la telemetría. |
| `8` | Ejecuta las autopruebas del Punto 3 contra los valores de control. |
| `0` | Finaliza el programa y libera la memoria utilizada. |

Las opciones de cálculo (`2` a `5` y `7`) requieren haber cargado los datos
primero. La bitácora registra cada acción relevante con un número de evento y
un tiempo simulado.

### Flujo recomendado

1. Opción `1`: cargar los datos (Enter para usar `26.000 kg` y `10.900 m/s`).
2. Opción `2`: analizar la Ley de Newton y la desaceleración.
3. Opción `3`: obtener la energía cinética inicial.
4. Opción `4`: calcular trabajo, equilibrio y comparación de árboles.
5. Opción `5`: ver la justificación del combustible.
6. Opción `6`: revisar la bitácora.
7. Opción `7`: ver el detalle de los árboles.
8. Opción `8`: ejecutar las autopruebas.

---

## 6. Punto 3: trabajo, equilibrio y árboles

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

## 7. Relación entre trabajo, equilibrio y árboles

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

## 8. Modelo físico y fórmulas

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

## 9. Comparación ABB vs AVL

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

## 10. Valores de control

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

---

## 11. Compilación y ejecución

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

### Ejecución

Desde la raíz del proyecto, en Windows:

```powershell
.\tli.exe
```

También se puede ejecutar con doble clic sobre `tli.exe` o usando el objetivo
`run` del Makefile:

```sh
mingw32-make run
```

Al iniciar, elegir la opción `1` y presionar Enter dos veces para usar los
valores de ejemplo (`26.000 kg` y `10.900 m/s`). Luego recorrer las opciones
en el orden indicado en la sección 5.

---

## 12. Verificación rápida

```sh
mingw32-make
mingw32-make run
```

Luego:

1. Opción `1`: presionar Enter para usar los valores de ejemplo (`26000` kg y `10900` m/s).
2. Opción `2`: verificar la Ley de Newton y la tabla de desaceleración.
3. Opción `3`: verificar la energía cinética inicial.
4. Opción `4`: verificar trabajo, equilibrio y comparación ABB / AVL.
5. Opción `5`: verificar la justificación del combustible.
6. Opción `6`: verificar la bitácora de eventos.
7. Opción `7`: verificar el detalle de los árboles.
8. Opción `8`: ejecutar las autopruebas.

Resultado esperado de las autopruebas: `22/22 pruebas OK`.

---

## 13. Registro del uso de Inteligencia Artificial

El equipo utilizó asistentes de IA generativa como herramienta de apoyo para
generar ideas, revisar código y mejorar la documentación. La IA no reemplazó el
análisis del equipo: cada sugerencia fue revisada, adaptada y validada con la
compilación y las autopruebas del programa.

| Herramienta | Ámbito | Uso concreto | Verificación realizada |
|---|---|---|---|

| Asistente de IA para análisis | Ideas y explicación teórica | Propuestas para organizar el escenario, las consignas y la explicación del modelo físico | Contraste de fórmulas y valores con los resultados del programa y los valores de control |
| Asistente de IA para código | Depuración y mejoras | Sugerencias de nombres, mensajes, validaciones y organización modular | Compilación con `-Wall -Wextra` y pruebas de las opciones del menú |

**Registro de consultas y decisiones:** las consultas se orientaron a entender
el escenario, proponer la estructura del README, revisar fórmulas y detectar
errores de compilación o de ejecución. Las respuestas se contrastaron con el
código fuente y con los resultados de las autopruebas; las sugerencias que no
coincidían con el modelo del proyecto se descartaron o se adaptaron.



