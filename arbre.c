#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "arbre.h"
#include "joust.h"

#define PROFONDEUR 5
#define DEBUG 0

#define Cp 0.3
#define SIMULATION 100



int totalPassage(P_NODE proot)
{
    while(proot->father != NULL)
    {
        proot = proot->father;
    }
    return proot->passage;
}


int calculUCT(P_NODE proot)
{
    return proot->victory + 2*Cp*sqrt((2*log(totalPassage(proot)))/proot->passage);
}

/**
 * return indice array of max value
 */
int indexMaxValue(float * arrayToEvaluate, int size)
{
    int index = 0;
    for(int i = 1; i < size; i++)
    {
        if(arrayToEvaluate[i] > arrayToEvaluate[index])
            index = i;
    }
    return index;
}


P_NODE selectBestSon(P_NODE proot)
{
    float *tmp = (float *) malloc(sizeof(float)); // array that will contain UCT value to each sons
    for(int i = 0; i < proot->board->nbr_mouvements; i++)
    {
        if(proot->son[i]->passage == 0 || totalPassage(proot) == 0) // infinite value
            return proot->son[i];
        tmp[i] = calculUCT(proot->son[i]);
    }
    return proot->son[indexMaxValue(tmp, proot->board->nbr_mouvements)];
}

int createSon(P_NODE proot)
{
    proot->son = (P_NODE *) malloc(sizeof(P_NODE)*proot->board->nbr_mouvements);
    PLATEAU *copyBoard = (PLATEAU *) malloc(sizeof(PLATEAU));
    cloner_jeu(copyBoard, proot->board);
    for(int i = 0; i < proot->board->nbr_mouvements; i++)
    {
        P_NODE tmp = (NODE *) malloc(sizeof(NODE));
        tmp->passage = 0;
        tmp->victory = 0;
        tmp->son = NULL;
        tmp->father = proot;
        jouer_coup(copyBoard, proot->board->mouvements[i]);
        tmp->board = copyBoard;
        tmp->movementOrigin = &proot->board->mouvements[i];
    }
}

COORDS* bestChoice(PLATEAU board)
{
    for(int i = 0; i < SIMULATION; i++)
    {
        P_NODE proot = (P_NODE *) malloc(sizeof(P_NODE));
        PLATEAU *copyBoard = (PLATEAU *) malloc(sizeof(PLATEAU));
        cloner_jeu(copyBoard, proot->board);
        proot->father = NULL;
        proot->son = NULL;
        proot->remainingSon = NULL;
        proot->victory = 0;
        proot->passage = 0;
        proot->board = copyBoard;
        proot->movementOrigin = NULL;
        while(proot->passage > 0 ||proot->father == NULL)
        {
            // looking for best sons

        }
        // simulate the current node

    }



    createSon(proot);
    for(int i = 0; i < proot->board->nbr_mouvements; i++)
    {
        if(proot->son[i]->passage == 0 || totalPassage(proot) == 0){
            rollOut();
        }
    }
}

void rollOut()
{
    return;
}


/*
int evaluationNoeud(P_NOEUD pracine)
{
    if(pracine->profondeurActuelle % 2 == 0) // si la profondeur est paire alors les mouvements que contient le noeud correspondent au mouvement possible de l'ia
        return (pracine->plateau->nbr_mouvements == 0) ? -20 : pracine->plateau->nbr_mouvements; // si l'ia ne peut plus jouer on renvoie -20. sinon son nombre de mouvement possible
    else  // profondeur impaire. Le joueur adverse � fini son tour. C'est � l'ia de jou�. Le plateau contient les d�placement possible de l'ia
        return (pracine->plateau->nbr_mouvements == 0) ? 20 : -pracine->plateau->nbr_mouvements; // si l'ia ne peut plus jouer on renvoie -20 sinon + le nombre de coup
}

void minMaxNoeud(P_NOEUD pracine)
{
    if(pracine->profondeurActuelle == PROFONDEUR || pracine->plateau->nbr_mouvements == 0) // si on est bout d'une feuille
        pracine->evaluation = evaluationNoeud(pracine);
    else{// sinon
        if(pracine->profondeurActuelle % 2 == 1){ //les noeud situ� � une profondeur impaire correspondent au choix que peut faire l'ia
            pracine->evaluation = pracine->fils[0]->evaluation;
            for(int i = 1; i < pracine->plateau->nbr_mouvements; i++)
            {
                if(pracine->evaluation < pracine->fils[i]->evaluation)
                    pracine->evaluation = pracine->fils[i]->evaluation;
            }
        }else{ // les noeuds paires correspondent au choix de l'adversaire
            pracine->evaluation = pracine->fils[0]->evaluation;
            for(int i = 1; i < pracine->plateau->nbr_mouvements; i++)
            {
                if(pracine->evaluation > pracine->fils[i]->evaluation)
                    pracine->evaluation = pracine->fils[i]->evaluation;
            }
        }
    }
}


P_NOEUD nouveau_noeud(P_NOEUD pracinePere, PLATEAU *plateau, COORDS *coup)
{
    P_NOEUD pracine = (NOEUD *) malloc(sizeof(NOEUD)); // on cr�e le noeud

    PLATEAU *copyPlateau = (PLATEAU *) malloc(sizeof(PLATEAU));
    cloner_jeu(copyPlateau, plateau); // on copie le plateau pour ne pas modifier le plateau du noeud pr�c�dent

    // on rempli le noeud
    if(pracinePere != NULL){
        jouer_coup(copyPlateau, *coup); // on joue le coup pour avoir un plateau correspondant au choix potentiellement fait
        pracine->profondeurActuelle = pracinePere->profondeurActuelle + 1;
        pracine->mouvementOrigine = coup;
    } else {
        pracine->profondeurActuelle = 0;
        pracine->mouvementOrigine = NULL;
    }
    pracine->pere = pracinePere;

    pracine->plateau = copyPlateau;

    // on rempli le tableau des fils via r�cursion
    if(pracine->profondeurActuelle < PROFONDEUR && pracine->plateau->nbr_mouvements > 0) // si le noeud actuelle n'est pas assez profond on calcule les fils
    {
        pracine->fils = (P_NOEUD *) malloc(sizeof(P_NOEUD)*pracine->plateau->nbr_mouvements);
        for(int i = 0; i < pracine->plateau->nbr_mouvements; i++)
        {
            pracine->fils[i] = nouveau_noeud(pracine, copyPlateau, &(copyPlateau->mouvements[i]));
        }
    } else  // on ne calcule pas les fils, on a atteint la profondeur max ou bien il n'y a aucun fils
        pracine->fils = NULL;
    minMaxNoeud(pracine);

    #if DEBUG == 1
    debug_noeud(pracine);
    #endif // DEBUG

    return pracine;
}

void detruitArbre(P_NOEUD pracine)
{
    if(pracine == NULL)
        return;
    if(pracine->fils != NULL){
        for(int i = 0; i < pracine->plateau->nbr_mouvements; i++)
        {
            detruitArbre(pracine->fils[i]);
        }
        pracine->fils = NULL;
    }
    // on lib�re toutes les allocations dynamique que contient un noeud et on termine par le noeud lui m�me
    free(pracine->plateau->mouvements);
    free(pracine->plateau);
    free(pracine->fils);
    free(pracine);
    #if DEBUG == 1
    printf("noeud detruit\n");
    #endif // DEBUG
}

COORDS* meilleur_coup(P_NOEUD pracine)
{
    P_NOEUD maxFils = (NOEUD *) malloc(sizeof(NOEUD));
    maxFils->evaluation = pracine->fils[0]->evaluation;
    maxFils->mouvementOrigine = pracine->fils[0]->mouvementOrigine;
    for(int i = 1; i < pracine->plateau->nbr_mouvements; i++) // on d�termine le fils ayant la plus haut �valuation
    {
        if(maxFils->evaluation < pracine->fils[i]->evaluation)
        {
            maxFils->evaluation = pracine->fils[i]->evaluation;
            maxFils->mouvementOrigine = pracine->fils[i]->mouvementOrigine;
        }
    }
    return maxFils->mouvementOrigine;
}


/// debug fonction utiliser pour le d�buguage
/// attention certaines portions de code pourrais �tre consid�r� comme n'�tant pas propre.
/// l'id�e �tait surtout de me faire gagner du temps dans mes test.Puisque c'est fonctionnalit� ne sont pas explicitement demand� dans l'intitul� du sujet je me suis permis de les �crire en vitesse.



int get_fils_numero(P_NOEUD pracine)
{
   if(pracine->pere == NULL)
    {
        return 0;
    } else {
        for(int i = 0; i < pracine->pere->plateau->nbr_mouvements; i++)
        {
            if(pracine->pere->plateau->mouvements[i].x == pracine->mouvementOrigine->x && pracine->pere->plateau->mouvements[i].y == pracine->mouvementOrigine->y)
                return i;
        }
        return 100; // impossible donc on sait que c'est une erreur
    }
}

size_t max_ligne(char **ligne, int nombreLigne)
{
    size_t max = strlen(ligne[0]);
    for(int i = 1; i < nombreLigne; i++)
    {
        if(strlen(ligne[i]) > max)
            max = strlen(ligne[i]);
    }
    return max;
}

void affiche_message(char **ligne, int nombreLigne)
{
    printf("\t");// ligne 1
    for(size_t i = 2; i < max_ligne(ligne, nombreLigne); i++)
    {
        printf("#");
    }
    printf("\n");

    printf("\t#"); // ligne 2
    for(size_t i = 4; i < max_ligne(ligne, nombreLigne); i++)
    {
        printf(" ");
    }
    printf("#");
    printf("\n");

    for(int i = 0; i < nombreLigne; i++) // print les lignes de contenue
    {
        printf("%s", ligne[i]);
    }

    printf("\t#"); // ligne 4
    for(size_t i = 4; i < max_ligne(ligne, nombreLigne); i++)
    {
        printf(" ");
    }
    printf("#");
    printf("\n");

    printf("\t");// ligne 5
    for(size_t i = 2; i < max_ligne(ligne, nombreLigne); i++)
    {
        printf("#");
    }
    printf("\n\n");
}


void debug_noeud(P_NOEUD pracine)
{
    if(pracine != NULL)
    {
        char ligne1[100];
        char ligne2[100];
        char ligne3[100];
        char ligne4[100];
        char ligne5[100];
        int nombreLigne = 5;
        char *tline[5] = {ligne1, ligne2, ligne3, ligne4, ligne5};

        sprintf(ligne1, "\t#    profondeur: %d    fils: %d", pracine->profondeurActuelle, get_fils_numero(pracine));
        sprintf(ligne2, "\t#    evaluation: %d", pracine->evaluation);
        sprintf(ligne3, "\t#    nombre_mouvement plateau: %d", pracine->plateau->nbr_mouvements);
        sprintf(ligne4, "\t#    joueur: %d", pracine->plateau->joueur+1);
        sprintf(ligne5, "\t#    coup fait avant: [%d,%d]", pracine->mouvementOrigine->x, pracine->mouvementOrigine->y);

        size_t max = max_ligne(tline, nombreLigne);
        for(int i = 0; i < nombreLigne; i++)
        {
            int tmp = strlen(tline[i]);
            for(size_t j = 0; j < max-tmp; j++)
            {
                strcat(tline[i], " ");
            }
            strcat(tline[i], "    #\n");
        }
        affiche_message(tline, nombreLigne);
        afficher_jeu(pracine->plateau);
        printf("\n\n\n");
    }
}
*/
