/*
 * jeu de Joust : à tour de rôle chacun des 2 joueurs doit déplacer son
 * cavalier (mouvement du cavalier aux échecs) sur un damier carré de taille
 * impaire, et "brûle" la chaque case qu'il vient de quitter (elle devient
 * inaccessible). Le dernier jouer à pouvoir bouger est le gagnant, son
 * adversaire,le 1er à ne plus pouvoir bouger, est le perdant. On ne peut pas
 *  jouer non plus sur la case occupée par l'autre joueur)
*/

#define DEBUG 0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>  // contient aussi memcpy (voir plus bas)

#include "joust.h"

#include "arbre.h"


void placer(PLATEAU *plateau, COORDS coords, char symbole) {
	// place un symbole à une coord donnée du plateau
	plateau->damier[coords.x][coords.y] = symbole;
}

void afficher_jeu(PLATEAU *plateau) {
    // print the top row of the table
    printf("\t ");
    for(int i = 0; i < TAILLE; i++)
    {
        printf(" %d", i);
    }
    printf("\n");

    printf("\t %c", TOPLEFTCORNER);
    for(int j = 0; j < TAILLE; j++)
    {
        printf("%c", HORIZONTALROW);

        if(j+1 < TAILLE)
            printf("%c", TOPCORNER);
        else
            printf("%c", TOPRIGHTCORNER);
    }
    printf("\n");

    // print the content of the table
    for(int i = 0; i < TAILLE; i++)
    {
        printf("\t%d%c", i, VERTICALROW); // print row of content
        for(int j = 0; j < TAILLE; j++)
        {
            printf("%c", plateau->damier[i][j]);
            printf("%c", VERTICALROW);
        }
        printf("\n");

        if(i+1 >= TAILLE)
            continue;

        printf("\t %c", LEFTCORNER); // print separator row
        for(int j = 0; j < TAILLE; j++)
        {
            printf("%c", HORIZONTALROW);
            if(j+1 < TAILLE)
            printf("%c", CENTRALCORNER);
            else
                printf("%c", RIGHTCORNER);
        }
        printf("\n");
    }

    // print the bottom row of the table
    printf("\t %c", BOTTOMLEFTCORNER);
    for(int j = 0; j < TAILLE; j++)
    {
        printf("%c", HORIZONTALROW);

        if(j+1 < TAILLE)
            printf("%c", BOTTOMCORNER);
        else
            printf("%c", BOTTOMRIGHTCORNER);
    }
    printf("\n\n");
}

void cloner_jeu(PLATEAU *pl2, PLATEAU *pl1) {
	// copie pl1 dans pl2 avec la copie bloc mémoire: memcpy(*dest, *source, taille_bloc)
	memcpy(pl2, pl1, sizeof(PLATEAU));
	pl2->mouvements = (COORDS *) malloc(sizeof(COORDS) * 8); // attention c'est pointé
	memcpy(pl2->mouvements, pl1->mouvements, sizeof(COORDS)*8);
}

int generer_mouvements_possibles(PLATEAU *plateau, COORDS *mvts, int joueur) {
	// place dans le vecteur mvts les coords des mouvements possibles pour le
	// n° du joueur indiqué, retourne le nombre de coords effectivement écrites dans mvts

	int X[8] = {+1, +2, +2, +1, -1, -2, -2, -1};  // vecteur décalage X mvt de cavalier
	int Y[8] = {-2, -1, +1, +2, +2, +1, -1, -2};  // vecteur décalage Y mvt de cavalier
	int prochain = 0; // indice prochaine destination à écrire
	for (int dir = 0; dir < 8; dir++) {  // énumère les directions possibles
		// calcule la position "cible"
		COORDS tmp = plateau->coords[joueur];  // position initiale
		tmp.x += X[dir]; // ajout décalage X puis Y
		tmp.y += Y[dir];
		// teste et mémorise si autorisé
		if (tmp.x >= 0 && tmp.x < TAILLE && tmp.y >= 0 && tmp.y < TAILLE &&
			plateau->damier[tmp.x][tmp.y] == LIBRE) {
			mvts[prochain] = tmp; // ok, mémoriser
			prochain += 1;
		}
	}
	return prochain; // par convention: marqueur de fin de liste
}

void init_jeu(PLATEAU *plateau) {
	// initialise une partie
	for (int i = 0; i < TAILLE; i++) {
		for (int j = 0; j < TAILLE; j++) {
			plateau->damier[i][j] = LIBRE;
		}
	}
	// positions initiales
	plateau->coords[0].x = plateau->coords[0].y = 1;
	plateau->coords[1].x = plateau->coords[1].y = TAILLE - 2;
	placer(plateau, plateau->coords[0], SYMBOLE[0]);  // le symbole joueur 1
	placer(plateau, plateau->coords[1], SYMBOLE[1]);  // le symbole joueur 2
	plateau->joueur = rand()%2; // détermine 1er joueur aléatoirement
	// mémorise les coups possibles (tableau avec un marqueur de fin
	// un peu comme une chaîne de caractères)
	plateau->mouvements = (COORDS *) malloc(sizeof(COORDS) * 8); // alloue la place
	plateau->nbr_mouvements = generer_mouvements_possibles(plateau, plateau->mouvements, plateau->joueur); // init mouvements possibles
}

int perdant(PLATEAU *plateau) {
	// joueur courant perdant si et seulement si plus de mouvement possible
	return (plateau->nbr_mouvements == 0);
}

int gagnant(PLATEAU *plateau) {
	if (plateau->nbr_mouvements == 0)
		return (plateau->joueur + 1) % 2;
	else
        return -1; // convention pas encore de gagnant
}

void jouer_coup(PLATEAU *plateau, COORDS coup) {
	// effectue le coup du joueur courant à l'endroit
	// spécifié par les coords de coup (ne vérifie pas si le coup est correct)

	// rendre la case du joueur courant inaccessible
	placer(plateau, plateau->coords[plateau->joueur], BRULE);
	// changer les coords du joueur courant
	plateau->coords[plateau->joueur].x = coup.x;
	plateau->coords[plateau->joueur].y = coup.y;
	// placer le symbole joueur courant
	placer(plateau, plateau->coords[plateau->joueur], SYMBOLE[plateau->joueur]);
	// passer au joueur suivant
	plateau->joueur = (plateau->joueur + 1) % 2;
	// re-générer les mouvements possibles (ne pas oublier)
	plateau->nbr_mouvements = generer_mouvements_possibles(plateau,
															plateau->mouvements,
															 plateau->joueur);
}

/**
*   role 1: humain
*   role 2: aléatoire
*   role 3: intelligence artificielle
*/
int* role()
{
    int* joueurRole = (int *) malloc(sizeof(int) * 2);
    for(int i = 0; i < 2; i++)
    {
        joueurRole[i] = 0;
        while(joueurRole[i] != 1 && joueurRole[i] != 2 && joueurRole[i] != 3){
            printf("========    Joueur %d    ========\n", i+1);
            printf("1. humain\n");
            printf("2. aleatoire\n");
            printf("3. ia\n");
            scanf(" %d", (joueurRole+i));
            printf("\n\n");
            getchar(); // on vide le buffer pour la prochaine boucle si jamais ce qu'à rentré l'utilisateur ne match pas avec le paterne du scanf
        }
    }
    return joueurRole;
}

void humainJoue(PLATEAU* plateau)
{
    int choix = 0;
    printf("========    JOUEUR %d    ========\n", plateau->joueur+1);
    while(choix < 1 || choix > plateau->nbr_mouvements)
    {
        for(int i = 0; i < plateau->nbr_mouvements; i++)
        {
            printf("\t%d. [%d, %d]\n", i+1, ((plateau->mouvements)+i)->x, ((plateau->mouvements)+i)->y);
        }
        printf("\n");
        scanf( "%d", &choix);
        printf("\n choix: %d\n", choix);
        printf("\n");
    }
    jouer_coup(plateau, plateau->mouvements[choix-1]);
}

void randomJoue(PLATEAU* plateau)
{
    int choix = rand() % plateau->nbr_mouvements;
    jouer_coup(plateau, plateau->mouvements[choix]);
}

void iaJoue(PLATEAU* plateau)
{
    P_NOEUD pracine = nouveau_noeud(NULL, plateau, NULL); // on créer l'arbre
    COORDS *mouvement = meilleur_coup(pracine); // on récupère le meilleur coup
    printf("l\'IA joue: [%d, %d]\n\n", mouvement->x, mouvement->y); // on l'affiche pour que le joueur humain sache quel coup viens de faire l'ia
    #if DEBUG == 1
    return; // en mode debug on veut juste afficher l'arbre et le meilleur coup. On ne fait pas une partie
    #endif // DEBUG
    jouer_coup(plateau, *mouvement); // on joue le coup
    detruitArbre(pracine); // on détruit l'arbre
}

int main()
{
	int *joueurRole = role();

	PLATEAU plateau;
	FN_PLAY jouer[] = {humainJoue, randomJoue, iaJoue};

    long graine_alea = 0;
	srand(graine_alea); // init générateur (pseudo-aléatoire)

	init_jeu(&plateau);
	afficher_jeu(&plateau);

	#if DEBUG == 1 // on appelle juste une fois la fonction iaJoue pour test
        iaJoue(&plateau);
        return 0;
    #endif // DEBUG
	while(!perdant(&plateau))
    {
            if(perdant(&plateau))
                break;
            jouer[joueurRole[plateau.joueur]-1](&plateau); // le joueur courant joue son tour
            afficher_jeu(&plateau);
    }
    printf("le gagnant est le joueur %c\n", SYMBOLE[gagnant(&plateau)]);
}

