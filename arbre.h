#ifndef ARBRE_H_INCLUDED
#define ARBRE_H_INCLUDED

#include "joust.h"


typedef struct node{
    struct node *father;
    struct node **son;
    float averageVictory; // totalVictory / totalPassage du noeud --> always between 0 and 1
    int victory;
    int passage;
    int ignore; // use as a boolean to say if we can select or not the node
    BOARD *board;
    COORDS* lastMovement;
} NODE, *P_NODE;

// fonction publique du module

COORDS* bestChoice(BOARD* board);

#endif // ARBRE_H_INCLUDED
