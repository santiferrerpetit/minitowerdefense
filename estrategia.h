#ifndef ESTRATEGIA_H
#define ESTRATEGIA_H

#include "mapa.h"
#include "nivel.h"
#include "pila.h"

typedef void (*DisposicionTorres)(Nivel*, Mapa*);

void disponer(Nivel* nivel, Mapa* mapa);

int daño_potencial(Mapa *mapa, int x, int y);

int simular_turno_backtracking(Nivel *nivel, Mapa *mapa, Coordenada *torres);

int buscar_indice(Coordenada *arreglo, int cantidad, Coordenada objetivo);

void disponer_con_backtracking(Nivel *nivel, Mapa* mapa);

void disponer_custom(Nivel* nivel, Mapa* mapa);

#endif
