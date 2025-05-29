#include "pila.h"

Pila* pila_crear() {
    Pila* p = malloc(sizeof(Pila));
    p->ultimo = 0;
    return p;
}

int pila_es_vacia(Pila* p) {
    return (p->ultimo == 0);
}

Coordenada pila_tope(Pila* p) {
    return p->datos[p->ultimo - 1];
}

void pila_apilar(Pila* p, Coordenada dato) {
    if (p->ultimo < MAX_PILA) {
        p->datos[p->ultimo] = dato;
        p->ultimo++;
    }
}

void pila_desapilar(Pila* p) {
    if (!pila_es_vacia(p)) {
        p->ultimo--;
    }
}



void pila_destruir(Pila* p) {
    free(p);
}

int pila_contiene(Pila* pila, Coordenada c) {
    for (int i = 0; i < pila->ultimo; i++) {
        if (pila->datos[i].x == c.x && pila->datos[i].y == c.y) {
            return 1;  // Encontrada
        }
    }
    return 0;  // No encontrada
}