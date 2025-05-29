#include "estrategia.h"
#include "simulador.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

static int posiciones_validas(Coordenada *validas, TipoCasilla **casillas, int alto, int ancho) {
    int cant_posiciones_validas = 0;
    
    for (int i = 0; i < alto; i++) {
        for (int j = 0; j < ancho; j++) {
            if (casillas[i][j] == VACIO) {
                validas[cant_posiciones_validas].x = i;
                validas[cant_posiciones_validas++].y = j;
            }
        }
    }

    return cant_posiciones_validas;
}

static void colocar_torre(Mapa *mapa, int x, int y, int nro_torre) {
    // actualizar torre
    mapa->torres[nro_torre].x = x;
    mapa->torres[nro_torre].y = y;

    // actualizar mapa
    mapa->casillas[x][y] = TORRE;
}

static int determinar_posicion_torre(int *casilla_elegida, int cant_validas) {
    int nueva_posicion = rand() % cant_validas;
    while(casilla_elegida[nueva_posicion])
        nueva_posicion = rand() % cant_validas;

    return nueva_posicion;
}


void disponer(Nivel* nivel, Mapa* mapa) {
    int cantidad_casillas = mapa->alto * mapa->ancho;
    Coordenada posiciones_validas_torre[cantidad_casillas];
    int casilla_elegida[cantidad_casillas];
    for(int i = 0; i < cantidad_casillas; casilla_elegida[i++] = 0);

    int cant_validas = posiciones_validas(posiciones_validas_torre, mapa->casillas, mapa->alto, mapa->ancho);

    for (int colocadas = 0; colocadas < mapa->cant_torres; colocadas++) {
        int nueva_torre = determinar_posicion_torre(casilla_elegida, cant_validas);
        casilla_elegida[nueva_torre] = 1;
        int nueva_torre_x = posiciones_validas_torre[nueva_torre].x;
        int nueva_torre_y = posiciones_validas_torre[nueva_torre].y;
        colocar_torre(mapa, nueva_torre_x, nueva_torre_y, colocadas);
    }
}

int daño_potencial(Mapa *mapa, int x, int y) {
    int daño = 0;

    for (int i = 0; i < mapa->alto; i++) {
        for (int j = 0; j < mapa->ancho; j++) {
            if (mapa->casillas[i][j] == CAMINO) {
                int dist = abs(i - x) + abs(j - y);
                if (dist <= mapa->distancia_ataque) {
                    daño++;
                }
            }
        }
    }

    return daño;
}

int simular_turno_backtracking(Mapa *mapa, Enemigos *enemigos, int cant_enemigos) {
    int max_turnos = 100;
    int vivos = cant_enemigos;

    for (int turno = 0; turno < max_turnos && vivos > 0; turno++) {
        for (int i = 0; i < cant_enemigos; i++) {
            if (enemigos[i].vida <= 0) continue;
            //calculamos el daño que le harian las torres a los enemigos sin hacer toda la simulacion completa
            for (int t = 0; t < mapa->cant_torres; t++) {
                int distancia_x = abs(enemigos->posiciones[i].x - mapa->torres[t].x);
                int distancia_y = abs(enemigos->posiciones[i].y - mapa->torres[t].y);
                if (distancia_x + distancia_y <= mapa->distancia_ataque) {
                    enemigos[i].vida -= 1;
                    if (enemigos[i].vida <= 0)
                        vivos--;
                    break; // una torre ataca una vez
                }
            }
        }
    }

    return vivos == 0;
}


void disponer_con_backtracking(Nivel* nivel, Mapa* mapa) {
    int total_casillas = mapa->alto * mapa->ancho;
    Coordenada posiciones_validas_torres[total_casillas];
    int cantidad_validas = posiciones_validas(posiciones_validas_torres, mapa->casillas, mapa->alto, mapa->ancho);

    if (cantidad_validas < mapa->cant_torres) {
        printf("Error: no hay suficientes posiciones válidas para colocar las torres.\n");
        return;
    }

    Pila* pila = pila_crear();
    int indice_actual = 0;
    int exito = 0;

    // Se usará para copiar enemigos originales
    Enemigos copia_enemigos[nivel->enemigos->cantidad];

    while (1) {
        if (pila->ultimo == mapa->cant_torres) {
            // Colocar las torres actuales en el mapa temporal
            for (int i = 0; i < pila->ultimo; i++) {
                mapa->torres[i] = pila->datos[i];
            }

            // Copiar enemigos originales para simulación
            for (int i = 0; i < nivel->enemigos->cantidad; i++)
                copia_enemigos[i] = nivel->enemigos[i];

            if (simular_turno_backtracking(mapa, copia_enemigos, nivel->enemigos->cantidad)) {
                exito = 1;
                break; // ¡Disposición ganadora encontrada!
            }

            // Backtrack
            pila_desapilar(pila);
            if (pila_es_vacia(pila)) break;
            Coordenada tope = pila_tope(pila);
            for (indice_actual = 0; indice_actual < cantidad_validas; indice_actual++)
                if (posiciones_validas_torres[indice_actual].x == tope.x && posiciones_validas_torres[indice_actual].y == tope.y)
                    break;
            indice_actual++;
            continue;
        }

        if (indice_actual >= cantidad_validas) {
            if (pila_es_vacia(pila)) break;
            pila_desapilar(pila);
            if (pila_es_vacia(pila)) break;
            Coordenada tope = pila_tope(pila);
            for (indice_actual = 0; indice_actual < cantidad_validas; indice_actual++)
                if (posiciones_validas_torres[indice_actual].x == tope.x && posiciones_validas_torres[indice_actual].y == tope.y)
                    break;
            indice_actual++;
            continue;
        }

        // Evitar repetir posiciones
        int ya_usada = 0;
        for (int i = 0; i < pila->ultimo; i++)
            if (pila->datos[i].x == posiciones_validas_torres[indice_actual].x &&
                pila->datos[i].y == posiciones_validas_torres[indice_actual].y) {
                ya_usada = 1;
                break;
            }

        if (!ya_usada) {
            pila_apilar(pila, posiciones_validas_torres[indice_actual]);
            indice_actual = 0; // empezar desde la primera casilla para la siguiente torre
        } else {
            indice_actual++;
        }
    }

    if (exito) {
        // Colocar la disposición ganadora en el mapa
        for (int i = 0; i < pila->ultimo; i++)
            colocar_torre(mapa, pila->datos[i].x, pila->datos[i].y, i);

    pila_destruir(pila);
    }
}


void disponer_custom(Nivel* nivel, Mapa* mapa) {
    int torres_colocadas = 0;

    while (torres_colocadas < mapa->cant_torres) {
        int mejor_x = -1;
        int mejor_y = -1;
        int max_daño = -1;

        for (int i = 0; i < mapa->alto; i++) {
            for (int j = 0; j < mapa->ancho; j++) {
                if (mapa->casillas[i][j] == VACIO) {
                    int daño = daño_potencial(mapa, i, j);
                    if (daño > max_daño) {
                        max_daño = daño;
                        mejor_x = i;
                        mejor_y = j;
                    }
                }
            }
        }

        if (mejor_x != -1 && mejor_y != -1) {
            colocar_torre(mapa, mejor_x, mejor_y, torres_colocadas);
            torres_colocadas++;
        } else {
            break;
        }
    }
}