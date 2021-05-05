#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arbre.h"
#include "joust.h"

#define PROFONDEUR 5
#define DEBUG 0



/**
 * evaluation "local" d'un noeud. Donc on �value le noeud sans propagation
 */
int evaluationNoeud(P_NOEUD pracine)
{
    if(pracine->profondeurActuelle % 2 == 0) // si la profondeur est paire alors les mouvements que contient le noeud correspondent au mouvement possible de l'ia
        return (pracine->plateau->nbr_mouvements == 0) ? -20 : pracine->plateau->nbr_mouvements; // si l'ia ne peut plus jouer on renvoie -20. sinon son nombre de mouvement possible
    else  // profondeur impaire. Le joueur adverse � fini son tour. C'est � l'ia de jou�. Le plateau contient les d�placement possible de l'ia
        return (pracine->plateau->nbr_mouvements == 0) ? 20 : -pracine->plateau->nbr_mouvements; // si l'ia ne peut plus jouer on renvoie -20 sinon + le nombre de coup
}

/**
 * si le noeud re�u est une feuille (ne poss�de aucun enfant) alors on �value le noeud localement via la fonction evaluationNoeud
 * si le noeud re�u poss�de des enfants, on fait le min ou le max de l'�valuation de ses enfants.
 * si les enfants correspondent au choix de l'ia on fait le max
 * si les enfants correspondent au choix de l'adversaire on fait le min
 */
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

/**
 * cr�er de mani�re r�cursive l'arbre
 * renvoie le pointeur vers le noeud de la racine de l'arbre
 */
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


/**
 * d�truit l'arbre de fa�on r�cursive
 * on appelle les noeuds enfant en boucle jusqu'� arriver � une feuille de l'arbre
 * on lib�re ce que contient le noeud courant qui est allou� dynamiquement puis on fait la m�me chose au noeud lui m�me
 * une fois que tout les enfants d'un noeud sont supprimer on sort de la boucle for et on supprime donc le noeud courant
 */
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

/**
 * prend en param�tre le pointeur vers le noeud racine de l'arbre et renvoie l'adresse d'une structure coordonn�es.
 * on renvoie le mouvement qui m�ne au fils ayant la meilleur �valuation. Ce qui correspond donc au meilleur coup
 */
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


/**
 * r�cup�re le num�ro de fils. Donc si pour un noeud quelconque cela correspond � l'indice de la case du tableau de P_NOEUD fils
 * 0 pour le premier fils, 1, etc
 * si jamais le aucune correspondance n'est trouv� entre la pracine et les coups possible du p�re on renvoie 100 comme valeur erreur
 * car on sait si bien que tout valeur en dehors de l'intervale [0; 8] seront fausses
 * cette fonction n'est utilis� que par mes autres fonctions de d�buguage
 */
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

/**
 * cr�ation d'un formatage de message un peu plus est�tique utilis� pour mon d�buguage
 */
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

/**
 * fonction de d�buguage qui affiche des renseignement sur le noeud pass� en param�tre
 */
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
