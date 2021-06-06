#ifndef ARBRE_H_INCLUDED
#define ARBRE_H_INCLUDED

#include "joust.h"


/*
typedef struct noeud{
    struct noeud *pere;
    PLATEAU *plateau;
    int evaluation;
    COORDS *mouvementOrigine;
    int profondeurActuelle;
    struct noeud **fils; // tableau de pointeur noeud fils
} NOEUD, *P_NOEUD;*/

typedef struct node{
    struct node *father;
    struct node **son;
    //struct node **remainingSon;
    float averageVictory; // totalVictory / totalPassage du noeud --> always between 0 and 1
    int victory;
    int passage;
    int ignore; // use as a boolean to say if we can select or not the node
    BOARD *board;
    COORDS* lastMovement;
} NODE, *P_NODE;

// fonction publique du module

COORDS* bestChoice(BOARD* board);

/*
P_NOEUD nouveau_noeud(P_NOEUD pracinePere, PLATEAU *plateau, COORDS *coup);

void detruitArbre(P_NOEUD pracine);

COORDS* meilleur_coup(P_NOEUD pracine);
*/
#endif // ARBRE_H_INCLUDED
