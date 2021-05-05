#ifndef ARBRE_H_INCLUDED
#define ARBRE_H_INCLUDED

#include "joust.h"

typedef struct noeud{
    struct noeud *pere;
    PLATEAU *plateau;
    int evaluation;
    COORDS *mouvementOrigine;
    int profondeurActuelle;
    struct noeud **fils; // tableau de pointeur noeud fils
} NOEUD, *P_NOEUD;

// fonction publique du module

P_NOEUD nouveau_noeud(P_NOEUD pracinePere, PLATEAU *plateau, COORDS *coup);

void detruitArbre(P_NOEUD pracine);

COORDS* meilleur_coup(P_NOEUD pracine);

#endif // ARBRE_H_INCLUDED
