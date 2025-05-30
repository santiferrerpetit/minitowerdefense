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

static int determinar_posicion_torre(int *casilla_elegida, int cant_pos_validas) {
    int nueva_posicion = rand() % cant_pos_validas;
    while(casilla_elegida[nueva_posicion])
        nueva_posicion = rand() % cant_pos_validas;

    return nueva_posicion;
}


void disponer(Nivel* nivel, Mapa* mapa) {
    int cantidad_casillas = mapa->alto * mapa->ancho;
    Coordenada pos_val_torre[cantidad_casillas];
    int casilla_elegida[cantidad_casillas];
    for(int i = 0; i < cantidad_casillas; casilla_elegida[i++] = 0);

    int cant_pos_validas = posiciones_validas(pos_val_torre, mapa->casillas, mapa->alto, mapa->ancho);

    for (int colocadas = 0; colocadas < mapa->cant_torres; colocadas++) {
        int nueva_torre = determinar_posicion_torre(casilla_elegida, cant_pos_validas);
        casilla_elegida[nueva_torre] = 1;
        int nueva_torre_x = pos_val_torre[nueva_torre].x;
        int nueva_torre_y = pos_val_torre[nueva_torre].y;
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

int simular_turno_backtracking(Nivel *nivel, Mapa *mapa, Coordenada *torres) {
    int total_daño = 0;

    for (int t = 0; t < mapa->cant_torres; t++) {
        int x = torres[t].x;
        int y = torres[t].y;
        total_daño += daño_potencial(mapa, x, y);
    }

    if (total_daño >= nivel->enemigos->vida_inicial) {
        return 1;  // Éxito: esta configuración puede eliminar a todos los enemigos
    } else {
        return 0;  // Fallo: no es suficiente
    }
}



int buscar_indice(Coordenada *arreglo, int cantidad, Coordenada objetivo) {
    for (int i = 0; i < cantidad; i++) {
        if (arreglo[i].x == objetivo.x && arreglo[i].y == objetivo.y) {
            return i;
        }
    }
    return -1; 
}

void disponer_con_backtracking(Nivel* nivel, Mapa* mapa) {
    int cantidad_casillas = mapa->alto * mapa->ancho;
    Coordenada pos_val_torre[cantidad_casillas];
    
    int cant_pos_validas = posiciones_validas(pos_val_torre, mapa->casillas, mapa->alto, mapa->ancho);

    int usadas[cant_pos_validas]; // Bitmap de coordenadas usadas 
    Pila *stack = pila_crear();
    int idx = 0;
    
    for (int i = 0; i < cant_pos_validas; i++) {
        usadas[i] = 0; 
    }
    int funciono = 0;
    while (!funciono) { 
        // Paso 1: Intentar agregar una nueva coordenada al stack
        if (stack->ultimo+1 < mapa->cant_torres) { 
            int bandera_encontrado = 0; 
            
            // Buscamos la próxima coordenada disponible
            for (int i = idx; i < cant_pos_validas; i++) {
                if (usadas[i] == 0) { 
                    pila_apilar(stack, pos_val_torre[i]);
                    usadas[i] = 1; 
                    idx = i + 1;
                    bandera_encontrado = 1;
                    break;
                }
            }

            if (bandera_encontrado == 0) { 
    
                if (pila_es_vacia(stack)) {
                    printf("no hay ninguna solucion\n");
                    disponer(nivel, mapa);
                    funciono = 1;
                }

                Coordenada ultima = pila_tope(stack);
                pila_desapilar(stack);

                // Buscamos su índice
                int ultimo_indice = buscar_indice(pos_val_torre, cant_pos_validas, ultima);

                if (ultimo_indice != -1) { 
                   usadas[ultimo_indice] = 0;
                }
                idx = ultimo_indice + 1;
            }
        }
        else {
            //aca evaluamos las torres en el stack, si funciona, terminar.
            if(simular_turno_backtracking(nivel, mapa, stack->datos) == 1){
                funciono = 1;
                for (int torres_colocadas = 0; torres_colocadas < mapa->cant_torres; torres_colocadas++) {
                    int nueva_torre_x = stack->datos[torres_colocadas].x;
                    int nueva_torre_y = stack->datos[torres_colocadas].y;
                    colocar_torre(mapa, nueva_torre_x, nueva_torre_y, torres_colocadas);
                }
            }
            // Hacemos backtrack si la combinación no es válida
            if(funciono == 0){
                Coordenada ultima = pila_tope(stack);
                pila_desapilar(stack);
            
                int ultimo_indice = buscar_indice(pos_val_torre, cant_pos_validas, ultima);
                if (ultimo_indice != -1) {
                    usadas[ultimo_indice] = 0;
                }
                idx = ultimo_indice + 1;
            }
        }
    }
    pila_destruir(stack);
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