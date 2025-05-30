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

    // Inicializar un arreglo para marcar qué posiciones ya fueron usadas
    int torres_usadas[cant_pos_validas];

    for (int i = 0; i < cant_pos_validas; i++) {
        torres_usadas[i] = 0;
    }

    Pila *pila_de_torres = pila_crear();
    int idx = 0; 
    int solucion_encontrada = 0;

    // Backtracking se ejecuta hasta encontrar una solución
    while (!solucion_encontrada) {
        // Si aún no hemos colocado todas las torres
        if (pila_de_torres->ultimo + 1 < mapa->cant_torres) {
            int encontrado = 0;  // Bandera para saber si encontramos una posición libre

            // Buscar la siguiente posición válida y libre para colocar la torre
            for (int i = idx; i < cant_pos_validas; i++) {
                if (torres_usadas[i] == 0) {
                    pila_apilar(pila_de_torres, pos_val_torre[i]);
                    torres_usadas[i] = 1;  // Marcar esta posición como usada
                    idx = i + 1; 
                    encontrado = 1;
                    break;
                }
            }

            if (!encontrado) {
                // Si no encontramos posiciones disponibles y la pila está vacía, no hay solución posible
                if (pila_es_vacia(pila_de_torres)) {
                    disponer(nivel, mapa);  // Como no hay solucion disponemos las torres de manera aleatoria
                    solucion_encontrada = 1;
                } else {
                    // Hacer backtrack: quitar la última torre colocada
                    Coordenada ultima = pila_tope(pila_de_torres);
                    pila_desapilar(pila_de_torres);
                    int ultimo_indice = buscar_indice(pos_val_torre, cant_pos_validas, ultima);
                    if (ultimo_indice != -1) torres_usadas[ultimo_indice] = 0;  // Marcar como disponible
                    idx = ultimo_indice + 1;  // Reanudar búsqueda desde la siguiente posición
                }
            }
        } else {
            // Evaluar la configuración
            if (simular_turno_backtracking(nivel, mapa, pila_de_torres->datos) == 1) {
                // Colocar torres definitivamente en el mapa
                solucion_encontrada = 1;
                for (int i = 0; i < mapa->cant_torres; i++) {
                    int x = pila_de_torres->datos[i].x;
                    int y = pila_de_torres->datos[i].y;
                    colocar_torre(mapa, x, y, i);
                }
            } else {
                // Configuración no válida: hacer backtrack
                Coordenada ultima = pila_tope(pila_de_torres);
                pila_desapilar(pila_de_torres);
                int ultimo_indice = buscar_indice(pos_val_torre, cant_pos_validas, ultima);
                if (ultimo_indice != -1) torres_usadas[ultimo_indice] = 0;  // Marcar posición como disponible
                idx = ultimo_indice + 1;  // Continuar búsqueda desde siguiente posición
            }
        }
    }

    // Liberamos la memoria usada por la pila
    pila_destruir(pila_de_torres);
}

void disponer_custom(Nivel* nivel, Mapa* mapa) {
    int torres_colocadas = 0;

    while (torres_colocadas < mapa->cant_torres) {
        int mejor_x = -1, mejor_y = -1, max_daño = -1;

        // Buscar la casilla vacía con el máximo daño potencial
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

        // Colocar torre si se encontró una posición válida
        if (mejor_x != -1 && mejor_y != -1) {
            colocar_torre(mapa, mejor_x, mejor_y, torres_colocadas);
            torres_colocadas++;
        } else {
            break;  // Salir si no hay más posiciones válidas
        }
    }
}
