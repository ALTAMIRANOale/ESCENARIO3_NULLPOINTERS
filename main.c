    /*
    =================================================
    PROYECTO TLI - SIMULACION TIERRA-LUNA

    Responsabilidades actuales:
    - Menu principal
    - Validacion de entradas
    - Carga de datos

    Modulos integrados:
    - Punto 1 (Altamirano): ley de Newton y
    desaceleracion por tramos en
    tli_explicaciones.c
    - Punto 2 (Agustin): implementado en punto2.c
    - Punto 3 (Axel): trabajo, equilibrio,
    telemetria y comparacion ABB / AVL
    en trabajo.c / arbol.c
    - Punto 4 (Agustin): implementado en punto4.c

    La bitacora de eventos se implementa como
    lista enlazada simple dentro de main.c.

    Las constantes fisicas se encuentran definidas
    en tli.h y se cargan automaticamente.
    =================================================
    */

    #include <stdio.h>
    #include <stdlib.h>

    #include "tli.h"
    #include "validaciones.h"
    #include "trabajo.h"
    #include "arbol.h"

    /*=========================================
    CARGA DE DATOS
    =========================================*/

    /* Carga solamente los datos variables de
    la simulacion */
    void cargarDatos(DatosTLI *datos)
    {
        printf("\n====================================\n");
        printf("      CARGA DE DATOS DE MISION\n");
        printf("====================================\n");

        printf("Constantes fisicas cargadas:\n");
        printf("Masa Tierra........... %.3e kg\n", M_TIERRA);
        printf("Masa Luna............. %.3e kg\n", M_LUNA);
        printf("Radio Tierra.......... %.0f m\n", R_TIERRA);
        printf("Distancia Tierra-Luna. %.0f m\n", DISTANCIA_TL);

        printf("\nValores de ejemplo (presione Enter para usarlos):\n");
        printf("Masa de la nave....... 26000 kg\n");
        printf("Velocidad inicial..... 10900 m/s\n");

        printf("\n");

        datos->masaNave =
            leerDoublePositivoConEjemplo(
                "Ingrese masa de la nave (kg)",
                26000.0
            );

        datos->velocidadInicial =
            leerDoublePositivoConEjemplo(
                "Ingrese velocidad inicial (m/s)",
                10900.0
            );

        datos->cargado = 1;

        printf("\nDatos cargados correctamente.\n");
        printf("Masa: %.2f kg | Velocidad inicial: %.2f m/s\n",
            datos->masaNave,
            datos->velocidadInicial);
    }

    /*=========================================
    PUNTO 1 - LEY DE NEWTON
    =========================================*/

    /*
    * Explica la primera ley de Newton y calcula
    * la desaceleracion gravitatoria por tramos
    * entre la orbita LEO y el punto L1.
    */
    void punto1_leyNewton(DatosTLI d, NodoEvento **l)
    {
        NodoTrayectoria *trayectoria = NULL;
        ResultadoBusquedaL1 busquedaL1;
        double r0;
        double rL1;
        double aLEO;
        double aL1;
        double fuerzaLEO;
        const int tramos = 10;
        int i;

        if (d.masaNave <= 0.0 || d.velocidadInicial <= 0.0)
        {
            printf("\nError: los datos de la nave no son validos.\n");
            return;
        }

        explicar_primera_ley_newton();

        r0 = R_TIERRA + H_LEO;

        busquedaL1 = trabajo_buscar_L1(
            L1_R_MIN,
            L1_R_MAX,
            L1_TOLERANCIA
        );

        if (busquedaL1.valido && busquedaL1.radio > r0)
        {
            rL1 = busquedaL1.radio;
        }
        else
        {
            rL1 = trabajo_radio_equilibrio_gravitatorio();

            printf(
                "\nAdvertencia: no se pudo validar L1; "
                "se usa r_eq como referencia.\n"
            );
        }

        aLEO = calcular_desaceleracion_gravitatoria(r0);
        aL1 = calcular_desaceleracion_gravitatoria(rL1);
        fuerzaLEO = d.masaNave * aLEO;

        printf("\nDatos de la nave:\n");
        printf("Masa: %.2f kg\n", d.masaNave);
        printf("Velocidad inicial: %.2f m/s\n", d.velocidadInicial);
        printf("Cantidad de movimiento (p = m*v): %.6e kg*m/s\n",
            d.masaNave * d.velocidadInicial);
        printf("Radio de LEO (r0): %.0f m\n", r0);
        printf("Radio de L1 (rL1): %.0f m\n", rL1);

        printf("\nDesaceleracion gravitatoria terrestre (a = G*M/r^2):\n");
        printf("En LEO: %.6f m/s^2\n", aLEO);
        printf("En L1:  %.6f m/s^2\n", aL1);
        printf("Fuerza gravitatoria en LEO: %.6e N\n", fuerzaLEO);
        printf("La aceleracion disminuye con el cuadrado de la distancia:\n");
        printf("la desaceleracion es cada vez menor a medida que la nave se aleja.\n");

        for (i = 0; i <= tramos; i++)
        {
            double r;

            r = r0 + (rL1 - r0) * (double) i / (double) tramos;

            insertar_tramo(&trayectoria, r);
        }

        mostrar_trayectoria_lista(trayectoria);

        liberar_lista(trayectoria);
        trayectoria = NULL;

        printf("\nLa lista enlazada almaceno %d tramos entre LEO y L1.\n",
            tramos + 1);
        printf("La nave mantiene su inercia; solo la gravedad terrestre la frena.\n");

        /* El evento se registra desde el menu principal */
        (void)l;
    }

    /*=========================================
    BITACORA (LISTA ENLAZADA)
    =========================================*/

    /*
    * Inserta un evento al final de la lista
    * enlazada de la bitacora.
    */
    void agregarEvento(
        NodoEvento **l,
        const char *x,
        double t
    )
    {
        NodoEvento *nuevo;
        NodoEvento *actual;

        if(l == NULL || x == NULL)
        {
            return;
        }

        nuevo = (NodoEvento *) malloc(sizeof(NodoEvento));

        if(nuevo == NULL)
        {
            printf("\nError: no se pudo registrar el evento.\n");
            return;
        }

        snprintf(nuevo->descripcion, sizeof(nuevo->descripcion), "%s", x);
        nuevo->tiempo = t;
        nuevo->siguiente = NULL;

        if(*l == NULL)
        {
            *l = nuevo;
            return;
        }

        actual = *l;

        while(actual->siguiente != NULL)
        {
            actual = actual->siguiente;
        }

        actual->siguiente = nuevo;
    }

    /*
    * Recorre y muestra todos los eventos
    * registrados en la bitacora.
    */
    void mostrarBitacora(NodoEvento *l)
    {
        NodoEvento *actual = l;
        int numero = 1;

        printf("\n========== BITACORA DE EVENTOS ==========\n");

        if(actual == NULL)
        {
            printf("\nLa bitacora esta vacia.\n");
            printf("Ejecute alguna opcion del menu para registrar eventos.\n");
            printf("==========================================\n");
            return;
        }

        printf("%-5s %-10s %s\n", "Nro", "Tiempo", "Descripcion");
        printf("------------------------------------------\n");

        while(actual != NULL)
        {
            printf("%-5d %-10.2f %s\n",
                numero,
                actual->tiempo,
                actual->descripcion);

            actual = actual->siguiente;
            numero++;
        }

        printf("------------------------------------------\n");
        printf("Total de eventos: %d\n", numero - 1);
        printf("==========================================\n");
    }

    /*
    * Libera toda la memoria utilizada por
    * la lista de eventos.
    */
    void liberarBitacora(NodoEvento *l)
    {
        NodoEvento *actual = l;

        while(actual != NULL)
        {
            NodoEvento *temporal = actual;

            actual = actual->siguiente;
            free(temporal);
        }
    }

    /*
    * Registra un evento y avanza el tiempo
    * simulado de la bitacora.
    */
    static void registrarEvento(
        NodoEvento **bitacora,
        const char *descripcion,
        double *tiempo
    )
    {
        agregarEvento(bitacora, descripcion, *tiempo);
        *tiempo += 1.0;
    }

    /*=========================================
    MAIN
    =========================================*/

    int main(void)
    {
        /* Datos generales de la simulacion */
        DatosTLI datos = {0};

        /* Resultado del balance energetico */
        BalanceEnergetico balance = {0};

        /* Lista de eventos */
        NodoEvento *bitacora = NULL;

        /* Telemetria y arboles del Punto 3 */
        TelemetriaPunto3 telemetria = {0};

        /* Historial de energias calculadas */
        double historialEnergia[MAX_TELEMETRIA];

        /* Cantidad de energias almacenadas */
        int cantidadHistorial = 0;

        /* Tiempo simulado para los eventos de la bitacora */
        double tiempoBitacora = 0.0;

        int opcion;
        

        do
        {
            printf("\n");
            printf("====================================\n");
            printf("   SIMULADOR TLI TIERRA - LUNA\n");
            printf("====================================\n");
            printf("1. Cargar datos de la nave\n");
            printf("2. Ley de Newton\n");
            printf("3. Energia\n");
            printf("4. Trabajo\n");
            printf("5. Combustible\n");
            printf("6. Mostrar bitacora\n");
            printf("7. Arboles ABB y AVL\n");
            printf("8. Verificar resultados\n");
            printf("0. Salir\n");
            printf("====================================\n");
            
            // Usamos la función de validación para acotar al rango del menú (0 a 8)
            opcion = leerEnteroEnRango("Seleccione una opcion: ", 0, 8);

            switch(opcion)
            {
                case 1:

                    /* Carga inicial de datos */
                    cargarDatos(&datos);

                    registrarEvento(
                        &bitacora,
                        "Datos de la nave cargados",
                        &tiempoBitacora
                    );

                    break;

                case 2:

                    /* Ejecuta Ley de Newton */
                    if(!datos.cargado)
                    {
                        printf(
                            "\nPrimero debe cargar "
                            "los datos.\n"
                        );
                        break;
                    }

                    punto1_leyNewton(
                        datos,
                        &bitacora
                    );

                    registrarEvento(
                        &bitacora,
                        "Ley de Newton ejecutada",
                        &tiempoBitacora
                    );

                    break;

                case 3:

                    /* Balance energetico */
                    if(!datos.cargado)
                    {
                        printf(
                            "\nPrimero debe cargar "
                            "los datos.\n"
                        );
                        break;
                    }

                    balance =
                        punto2_energia(
                            datos,
                            historialEnergia,
                            &cantidadHistorial
                        );

                    registrarEvento(
                        &bitacora,
                        "Balance energetico calculado",
                        &tiempoBitacora
                    );

                    break;

                case 4:

                    /* Trabajo mecanico */
                    if(!datos.cargado)
                    {
                        printf(
                            "\nPrimero debe cargar "
                            "los datos.\n"
                        );
                        break;
                    }

                    punto3_trabajo(
                        datos,
                        balance,
                        &telemetria
                    );

                    registrarEvento(
                        &bitacora,
                        "Trabajo y equilibrio calculados",
                        &tiempoBitacora
                    );

                    break;

                case 5:

                    /* Calculo de combustible */
                    if(!datos.cargado)
                    {
                        printf(
                            "\nPrimero debe cargar "
                            "los datos.\n"
                        );
                        break;
                    }

                    punto4_combustible(
                        datos,
                        balance
                    );

                    registrarEvento(
                        &bitacora,
                        "Consumo de combustible estimado",
                        &tiempoBitacora
                    );

                    break;

                case 6:

                    /* Mostrar bitacora */
                    mostrarBitacora(bitacora);

                    break;

                case 7:

                    /* Arboles ABB y AVL del Punto 3 */
                    if(!datos.cargado)
                    {
                        printf(
                            "\nPrimero debe cargar "
                            "los datos.\n"
                        );
                        break;
                    }

                    punto3_mostrar_arboles(&telemetria);

                    registrarEvento(
                        &bitacora,
                        "Arboles ABB y AVL mostrados",
                        &tiempoBitacora
                    );

                    break;

                case 8:

                    /* Autoverificacion del Punto 3 */
                    trabajo_verificar();

                    registrarEvento(
                        &bitacora,
                        "Verificacion de resultados ejecutada",
                        &tiempoBitacora
                    );

                    break;

                case 0:

                    printf(
                        "\nFinalizando programa...\n"
                    );

                    break;

                default:

                    printf(
                        "\nOpcion no valida.\n"
                    );
            }

        } while(opcion != 0);

        /* Liberacion final de memoria */
        liberarBitacora(bitacora);
        punto3_liberar_telemetria(&telemetria);

        return 0;
    }
