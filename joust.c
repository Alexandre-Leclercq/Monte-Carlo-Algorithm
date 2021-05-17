/*
 * jeu de Joust : � tour de r�le chacun des 2 joueurs doit d�placer son
 * cavalier (mouvement du cavalier aux �checs) sur un damier carr� de taille
 * impaire, et "br�le" la chaque case qu'il vient de quitter (elle devient
 * inaccessible). Le dernier jouer � pouvoir bouger est le gagnant, son
 * adversaire,le 1er � ne plus pouvoir bouger, est le perdant. On ne peut pas
 *  jouer non plus sur la case occup�e par l'autre joueur)
*/

#define DEBUG 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>  // contient aussi memcpy (voir plus bas)

#include "joust.h"

#include "arbre.h"


void place(BOARD *board, COORDS coords, char symbol) {
	// place un symbole � une coord donn�e du plateau
	board->checkerboard[coords.x][coords.y] = symbol;
}

void display_game(BOARD *board) {
    // print the top row of the table
    printf("\t ");
    for(int i = 0; i < SIZE; i++)
    {
        printf(" %d", i);
    }
    printf("\n");

    printf("\t %c", TOPLEFTCORNER);
    for(int j = 0; j < SIZE; j++)
    {
        printf("%c", HORIZONTALROW);

        if(j+1 < SIZE)
            printf("%c", TOPCORNER);
        else
            printf("%c", TOPRIGHTCORNER);
    }
    printf("\n");

    // print the content of the table
    for(int i = 0; i < SIZE; i++)
    {
        printf("\t%d%c", i, VERTICALROW); // print row of content
        for(int j = 0; j < SIZE; j++)
        {
            printf("%c", board->checkerboard[i][j]);
            printf("%c", VERTICALROW);
        }
        printf("\n");

        if(i+1 >= SIZE)
            continue;

        printf("\t %c", LEFTCORNER); // print separator row
        for(int j = 0; j < SIZE; j++)
        {
            printf("%c", HORIZONTALROW);
            if(j+1 < SIZE)
            printf("%c", CENTRALCORNER);
            else
                printf("%c", RIGHTCORNER);
        }
        printf("\n");
    }

    // print the bottom row of the table
    printf("\t %c", BOTTOMLEFTCORNER);
    for(int j = 0; j < SIZE; j++)
    {
        printf("%c", HORIZONTALROW);

        if(j+1 < SIZE)
            printf("%c", BOTTOMCORNER);
        else
            printf("%c", BOTTOMRIGHTCORNER);
    }
    printf("\n\n");
}

void clone_game(BOARD *pl2, BOARD *pl1) {
	// copie pl1 dans pl2 avec la copie bloc m�moire: memcpy(*dest, *source, taille_bloc)
	memcpy(pl2, pl1, sizeof(BOARD));
	pl2->movements = (COORDS *) malloc(sizeof(COORDS) * 8); // attention c'est point�
	memcpy(pl2->movements, pl1->movements, sizeof(COORDS)*8);
}

int generate_possible_movements(BOARD *board, COORDS *mvts, int player) {
	// place dans le vecteur mvts les coords des mouvements possibles pour le
	// n� du joueur indiqu�, retourne le nombre de coords effectivement �crites dans mvts

	int X[8] = {+1, +2, +2, +1, -1, -2, -2, -1};  // vecteur d�calage X mvt de cavalier
	int Y[8] = {-2, -1, +1, +2, +2, +1, -1, -2};  // vecteur d�calage Y mvt de cavalier
	int next = 0; // indice prochaine destination � �crire
	for (int dir = 0; dir < 8; dir++) {  // �num�re les directions possibles
		// calcule la position "cible"
		COORDS tmp = board->coords[player];  // position initiale
		tmp.x += X[dir]; // ajout d�calage X puis Y
		tmp.y += Y[dir];
		// teste et m�morise si autoris�
		if (tmp.x >= 0 && tmp.x < SIZE && tmp.y >= 0 && tmp.y < SIZE &&
			board->checkerboard[tmp.x][tmp.y] == FREE) {
			mvts[next] = tmp; // ok, m�moriser
			next += 1;
		}
	}
	return next; // par convention: marqueur de fin de liste
}

void init_game(BOARD *board) {
	// initialise une partie
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			board->checkerboard[i][j] = FREE;
		}
	}
	// positions initiales
	board->coords[0].x = board->coords[0].y = 1;
	board->coords[1].x = board->coords[1].y = SIZE - 2;
	place(board, board->coords[0], SYMBOL[0]);  // le symbole joueur 1
	place(board, board->coords[1], SYMBOL[1]);  // le symbole joueur 2
	board->player = rand()%2; // d�termine 1er joueur al�atoirement
	// m�morise les coups possibles (tableau avec un marqueur de fin
	// un peu comme une cha�ne de caract�res)
	board->movements = (COORDS *) malloc(sizeof(COORDS) * 8); // alloue la place
	board->movements_nbr = generate_possible_movements(board, board->movements, board->player); // init mouvements possibles
}

int loser(BOARD *board) {
	// joueur courant perdant si et seulement si plus de mouvement possible
	return (board->movements_nbr == 0);
}

int end_game(BOARD *board) {
	// fin du jeu seulement si le joueur courant est perdant
	return loser(board);
}

void finish_game_randomly(BOARD *board) {
	// termine al�atoirement la partie courante, qui est modifi�e (donc la cloner avant si n�cessaire)
	while (!end_game(board)) {
		// tirer au sort un num�ro de mouvement (attention : tirage de faible qualit� ici)
		int choice = rand() % board->movements_nbr;
		// jouer coup et changer de joueur
		play_move(board, board->movements[choice]);
	}
}

int winner(BOARD *board) {
	if (board->movements_nbr == 0)
		return (board->player + 1) % 2;
	else
        return -1; // convention pas encore de gagnant
}

void play_move(BOARD *board, COORDS chosenMove) {
	// effectue le coup du joueur courant � l'endroit
	// sp�cifi� par les coords de coup (ne v�rifie pas si le coup est correct)

	// rendre la case du joueur courant inaccessible
	place(board, board->coords[board->player], BURNED);
	// changer les coords du joueur courant
	board->coords[board->player].x = chosenMove.x;
	board->coords[board->player].y = chosenMove.y;
	// placer le symbole joueur courant
	place(board, board->coords[board->player], SYMBOL[board->player]);
	// passer au joueur suivant
	board->player = (board->player + 1) % 2;
	// re-g�n�rer les mouvements possibles (ne pas oublier)
	board->movements_nbr = generate_possible_movements(board,
															board->movements,
															 board->player);
}

/**
*   role 1: humain
*   role 2: al�atoire
*   role 3: intelligence artificielle
*/
int* role()
{
    int* playerRole = (int *) malloc(sizeof(int) * 2);
    for(int i = 0; i < 2; i++)
    {
        playerRole[i] = 0;
        while(playerRole[i] != 1 && playerRole[i] != 2 && playerRole[i] != 3){
            printf("========    Player %d    ========\n", i+1);
            printf("1. human\n");
            printf("2. random\n");
            printf("3. AI\n");
            scanf(" %d", (playerRole+i));
            printf("\n\n");
            getchar(); // on vide le buffer pour la prochaine boucle si jamais ce qu'� rentr� l'utilisateur ne match pas avec le paterne du scanf
        }
    }
    return playerRole;
}

void humanPlay(BOARD* board)
{
    int choice = 0;
    printf("========    PLAYER %d    ========\n", board->player+1);
    while(choice < 1 || choice > board->movements_nbr)
    {
        for(int i = 0; i < board->movements_nbr; i++)
        {
            printf("\t%d. [%d, %d]\n", i+1, ((board->movements)+i)->x, ((board->movements)+i)->y);
        }
        printf("\n");
        scanf( "%d", &choice);
        printf("\n choice: %d\n", choice);
        printf("\n");
    }
    play_move(board, board->movements[choice-1]);
}

void randomPlay(BOARD* board)
{
    int choice = rand() % board->movements_nbr;
    play_move(board, board->movements[choice]);
}


void aiPlay(BOARD* board)
{
    COORDS* movement = bestChoice(board);
    printf("\n[%d, %d]\n", movement->x, movement->y);
    //play_move(board, *movement);
}

int main()
{
	int *playerRole = role();

	BOARD board;
	FN_PLAY play[] = {humanPlay, randomPlay, aiPlay};

    long graine_alea = 0;
	srand(graine_alea); // init g�n�rateur (pseudo-al�atoire)

	init_game(&board);
	display_game(&board);

	#if DEBUG == 1 // on appelle juste une fois la fonction iaJoue pour test
        aiPlay(&board);
        return 0;
    #endif // DEBUG
	while(!loser(&board))
    {
            if(loser(&board))
                break;
            play[playerRole[board.player]-1](&board); // le joueur courant joue son tour
            display_game(&board);
    }
    printf("winner is player %c\n", SYMBOL[winner(&board)]);
}

