
#ifndef PILA_H
#define PILA_H
#define MAX_PILA 100
#include "nivel.h"

typedef struct {
    Coordenada datos[MAX_PILA];
    int ultimo;
} Pila;

Pila* pila_crear();
int pila_es_vacia(Pila* p);
Coordenada pila_tope(Pila* p);
void pila_apilar(Pila* p, Coordenada dato);
void pila_desapilar(Pila* p);
void pila_imprimir(Pila* p);
void pila_destruir(Pila* p);
int pila_contiene(Pila* pila, Coordenada c);

#endif