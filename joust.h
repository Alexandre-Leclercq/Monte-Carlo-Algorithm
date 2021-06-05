#ifndef JOUST_H_INCLUDED
#define JOUST_H_INCLUDED

// le carr� de la taille du damier (= nombre de cases du damier) doit �tre impair
// sinon le joueur 2 a une strat�gie gagnante � tous les coups
#define SIZE 15

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
#define FREE ' '
#define BURNED 219 // ascii value
static char SYMBOL[4] = {'1', '2', '3', '4'}; // symboles joueurs

typedef char CHECKERBOARD[SIZE][SIZE];
typedef struct {
	int x, y;
} COORDS;

// d�finition d'une structure PLATEAU de jeu, qu'on passera syst�matiquement
// par adresse/pointeur pour avoir une "API" plus r�guli�re (et �conomiser de
// la place sur la pile m�moire)
typedef struct {
	CHECKERBOARD checkerboard;
	COORDS coords[4]; // coords des 2 joueurs
	int player; // num�ro de joueur courant
	COORDS *movements; // tableau *� allouer* de mvts possible pour joueur courant
						// ce tableau sera maintenu � jour � chaque coup
	int movements_nbr;  // taille utile du tableau (pas forc�ment plein)
} BOARD;

typedef void (*FN_PLAY)(BOARD* board);

void clone_game(BOARD *pl2, BOARD *pl1);

void play_move(BOARD *plateau, COORDS chosenMove);

void display_game(BOARD *board);

#endif // JOUST_H_INCLUDED
