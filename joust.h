#ifndef JOUST_H_INCLUDED
#define JOUST_H_INCLUDED

// le carré de la taille du damier (= nombre de cases du damier) doit être impair
// sinon le joueur 2 a une stratégie gagnante à tous les coups
#define TAILLE 7

// ascii constants
#define TOPLEFTCORNER 201
#define TOPRIGHTCORNER 187
#define TOPCORNER 203
#define HORIZONTALROW 205
#define VERTICALROW 186
#define LEFTCORNER 204
#define RIGHTCORNER 185
#define CENTRALCORNER 206
#define BOTTOMLEFTCORNER 200
#define BOTTOMCORNER 202
#define BOTTOMRIGHTCORNER 188

// symboles "graphiques"
#define LIBRE ' '
#define BRULE 219 // ascii value
static char SYMBOLE[2] = {'1', '2'}; // symboles joueurs

typedef char DAMIER[TAILLE][TAILLE];
typedef struct {
	int x, y;
} COORDS;

// définition d'une structure PLATEAU de jeu, qu'on passera systématiquement
// par adresse/pointeur pour avoir une "API" plus régulière (et économiser de
// la place sur la pile mémoire)
typedef struct {
	DAMIER damier;
	COORDS coords[2]; // coords des 2 joueurs
	int joueur; // numéro de joueur courant
	COORDS *mouvements; // tableau *à allouer* de mvts possible pour joueur courant
						// ce tableau sera maintenu à jour à chaque coup
	int nbr_mouvements;  // taille utile du tableau (pas forcément plein)
} PLATEAU;

typedef void (*FN_PLAY)(PLATEAU* plateau);

void cloner_jeu(PLATEAU *pl2, PLATEAU *pl1);

void jouer_coup(PLATEAU *plateau, COORDS coup);

void afficher_jeu(PLATEAU *plateau);

#endif // JOUST_H_INCLUDED
