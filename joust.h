#ifndef JOUST_H_INCLUDED
#define JOUST_H_INCLUDED

// le carr� de la taille du damier (= nombre de cases du damier) doit �tre impair
// sinon le joueur 2 a une strat�gie gagnante � tous les coups
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

// d�finition d'une structure PLATEAU de jeu, qu'on passera syst�matiquement
// par adresse/pointeur pour avoir une "API" plus r�guli�re (et �conomiser de
// la place sur la pile m�moire)
typedef struct {
	DAMIER damier;
	COORDS coords[2]; // coords des 2 joueurs
	int joueur; // num�ro de joueur courant
	COORDS *mouvements; // tableau *� allouer* de mvts possible pour joueur courant
						// ce tableau sera maintenu � jour � chaque coup
	int nbr_mouvements;  // taille utile du tableau (pas forc�ment plein)
} PLATEAU;

typedef void (*FN_PLAY)(PLATEAU* plateau);

void cloner_jeu(PLATEAU *pl2, PLATEAU *pl1);

void jouer_coup(PLATEAU *plateau, COORDS coup);

void afficher_jeu(PLATEAU *plateau);

#endif // JOUST_H_INCLUDED
