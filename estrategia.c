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

void disponer_con_backtracking(Nivel* nivel, Mapa* mapa) {
    /* A cargo de la/el estudiante */
    return;
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