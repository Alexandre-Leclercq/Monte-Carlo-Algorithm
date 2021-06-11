/*
 * jeu de Joust : à tour de rôle chacun des 2 joueurs doit déplacer son
 * cavalier (mouvement du cavalier aux échecs) sur un damier carré de taille
 * impaire, et "brûle" la chaque case qu'il vient de quitter (elle devient
 * inaccessible). Le dernier jouer à pouvoir bouger est le gagnant, son
 * adversaire,le 1er à ne plus pouvoir bouger, est le perdant. On ne peut pas
 *  jouer non plus sur la case occupée par l'autre joueur)
*/

#define DEBUG 0
#define SEED 0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>  // contient aussi memcpy (voir plus bas)
#include <time.h>


#include "mtwister.h"

#include "joust.h"

#include "arbre.h"


void place(BOARD *board, COORDS coords, char symbol) {
	// place un symbole à une coord donnée du plateau
	board->checkerboard[coords.x][coords.y] = symbol;
}

void display_game(BOARD *board) {
    // print the top row of the table
    printf("\t   ");
    for(int i = 0; i < SIZE; i++)
    {

        printf("%d ", i);
        if(i == 0)
            printf(" ");
        else{
            for(int k = 0; k < floor(log10(SIZE))-floor(log10(i)); k++){
                printf(" ");
            }
        }
    }
    printf("\n");

    printf("\t  %c", TOPLEFTCORNER);
    for(int j = 0; j < SIZE; j++)
    {
        for(int k = 0; k < log10(SIZE); k++){
            printf("%c", HORIZONTALROW);
        }
        if(j+1 < SIZE)
            printf("%c", TOPCORNER);
        else
            printf("%c", TOPRIGHTCORNER);
    }
    printf("\n");

    // print the content of the table
    for(int i = 0; i < SIZE; i++)
    {
        printf("\t");
        if(i == 0)
            printf(" ");
        else{
            for(int k = 0; k < floor(log10(SIZE))-floor(log10(i)); k++){
                printf(" ");
            }
        }
        printf("%d%c", i, VERTICALROW); // print row of content
        for(int j = 0; j < SIZE; j++)
        {
            for(int k = 0; k < log10(SIZE); k++){
                printf("%c", board->checkerboard[i][j]);
            }

            printf("%c", VERTICALROW);
        }
        printf("\n");

        if(i+1 >= SIZE)
            continue;

        printf("\t  %c", LEFTCORNER); // print separator row
        for(int j = 0; j < SIZE; j++)
        {
            for(int k = 0; k < log10(SIZE); k++){
                printf("%c", HORIZONTALROW);
            }
            if(j+1 < SIZE)
            printf("%c", CENTRALCORNER);
            else
                printf("%c", RIGHTCORNER);
        }
        printf("\n");
    }

    // print the bottom row of the table
    printf("\t  %c", BOTTOMLEFTCORNER);
    for(int j = 0; j < SIZE; j++)
    {
        for(int k = 0; k < log10(SIZE); k++){
            printf("%c", HORIZONTALROW);
        }
        if(j+1 < SIZE)
            printf("%c", BOTTOMCORNER);
        else
            printf("%c", BOTTOMRIGHTCORNER);
    }
    printf("\n\n");
}

void clone_game(BOARD *pl2, BOARD *pl1) {
	// copie pl1 dans pl2 avec la copie bloc mémoire: memcpy(*dest, *source, taille_bloc)
	memcpy(pl2, pl1, sizeof(BOARD));
	pl2->movements = (COORDS *) malloc(sizeof(COORDS) * 8); // attention c'est pointé
	memcpy(pl2->movements, pl1->movements, sizeof(COORDS)*8);
}

void freeCopyBoard(BOARD *board){
    free(board->movements);
    free(board);
}

int generate_possible_movements(BOARD *board, COORDS *mvts, int player) {
	// place dans le vecteur mvts les coords des mouvements possibles pour le
	// n° du joueur indiqué, retourne le nombre de coords effectivement écrites dans mvts

	int X[8] = {+1, +2, +2, +1, -1, -2, -2, -1};  // vecteur décalage X mvt de cavalier
	int Y[8] = {-2, -1, +1, +2, +2, +1, -1, -2};  // vecteur décalage Y mvt de cavalier
	int next = 0; // indice prochaine destination à écrire
	for (int dir = 0; dir < 8; dir++) {  // énumère les directions possibles
		// calcule la position "cible"
		COORDS tmp = board->coords[player];  // position initiale
		tmp.x += X[dir]; // ajout décalage X puis Y
		tmp.y += Y[dir];
		// test et mémorise si autorisé
		if (tmp.x >= 0 && tmp.x < SIZE && tmp.y >= 0 && tmp.y < SIZE &&
			board->checkerboard[tmp.x][tmp.y] == FREE) {
			mvts[next] = tmp; // ok, mémoriser
			next += 1;
		}
	}
	return next; // par convention: marqueur de fin de liste
}

void init_game(BOARD *board) {
    MTRand r = seedRand(SEED);
	// initialise une partie
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			board->checkerboard[i][j] = FREE;
		}
	}
	// positions initiales
	board->coords[0].x = board->coords[0].y = 1;
	board->coords[1].x = board->coords[1].y = SIZE - 2;
	board->coords[2].x = board->coords[3].y = 1;
	board->coords[2].y = board->coords[3].x = SIZE - 2;
	place(board, board->coords[0], SYMBOL[0]);  // le symbole joueur 1
	place(board, board->coords[1], SYMBOL[1]);  // le symbole joueur 2
	place(board, board->coords[2], SYMBOL[2]);  // le symbole joueur 3
	place(board, board->coords[3], SYMBOL[3]);  // le symbole joueur 4
	board->player = genRandLong(&r)%4; // détermine 1er joueur aléatoirement
	// mémorise les coups possibles (tableau avec un marqueur de fin
	// un peu comme une chaîne de caractères)
	board->movements = (COORDS *) malloc(sizeof(COORDS) * 8); // alloue la place
	board->movements_nbr = generate_possible_movements(board, board->movements, board->player); // init mouvements possibles
}

int loser(BOARD *board) {
	// joueur courant perdant si et seulement si plus de mouvement possible
	return (board->movements_nbr == 0);
}

/**
 * the game is over if the remaning player is inferior or equal to one
 * return true if over
 * return false if not
 */
int end_game(BOARD *board) {
    BOARD *copyBoard = (BOARD *) malloc(sizeof(BOARD));
    clone_game(copyBoard, board);
    int remaning_player = 0;
    int victory = 0;
    for(int i = 0; i < 4; i++)
    {
        if(copyBoard->movements_nbr > 0){
            remaning_player += 1;
        }
        copyBoard->player = (copyBoard->player + 1) % 4;
        copyBoard->movements_nbr = generate_possible_movements(copyBoard, copyBoard->movements, copyBoard->player);
    }
    if(remaning_player <= 1)
        victory = 1;
    freeCopyBoard(copyBoard);
    return victory;
}

void finish_game_randomly(BOARD *board) {
    MTRand r = seedRand(SEED);
	// termine aléatoirement la partie courante, qui est modifiée (donc la cloner avant si nécessaire)
	while (!end_game(board)) {
		// tirer au sort un numéro de mouvement (attention : tirage de faible qualité ici)
		int choice = genRandLong(&r) % board->movements_nbr;
		// jouer coup et changer de joueur
		play_move(board, board->movements[choice]);
	}
}

// return the winner number of the player
// only use when the game is over
int winner(BOARD *board) {
    BOARD *copyBoard = (BOARD *) malloc(sizeof(BOARD));
    clone_game(copyBoard, board);
    for(int i = 0; i < 4; i++)
    {
        if(copyBoard->movements_nbr > 0){
            free(copyBoard);
            return copyBoard->player;
        }
        copyBoard->player = (copyBoard->player + 1) % 4;
        copyBoard->movements_nbr = generate_possible_movements(copyBoard, copyBoard->movements, copyBoard->player);
    }
    freeCopyBoard(copyBoard);
}

void play_move(BOARD *board, COORDS chosenMove) {
	// effectue le coup du joueur courant à l'endroit
	// spécifié par les coords de coup (ne vérifie pas si le coup est correct)

	// rendre la case du joueur courant inaccessible
	place(board, board->coords[board->player], BURNED);
	// changer les coords du joueur courant
	board->coords[board->player].x = chosenMove.x;
	board->coords[board->player].y = chosenMove.y;
	// placer le symbole joueur courant
	place(board, board->coords[board->player], SYMBOL[board->player]);
	// passer au joueur suivant
	board->player = (board->player + 1) % 4;
	// re-générer les mouvements possibles (ne pas oublier)
	board->movements_nbr = generate_possible_movements(board, board->movements, board->player);
    while(board->movements_nbr == 0 && !end_game(board))
    {
        board->player = (board->player + 1) % 4;
        board->movements_nbr = generate_possible_movements(board, board->movements, board->player);
    }
}

/**
*   role 1: humain
*   role 2: aléatoire
*   role 3: intelligence artificielle
*/
int* role()
{
    int* playerRole = (int *) malloc(sizeof(int) * 4);
    for(int i = 0; i < 4; i++)
    {
        playerRole[i] = 0;
        while(playerRole[i] != 1 && playerRole[i] != 2 && playerRole[i] != 3){
            printf("========    Player %d    ========\n", i+1);
            printf("1. human\n");
            printf("2. random\n");
            printf("3. AI\n");
            scanf(" %d", (playerRole+i));
            printf("\n\n");
            getchar(); // on vide le buffer pour la prochaine boucle si jamais ce qu'à rentré l'utilisateur ne match pas avec le paterne du scanf
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
    MTRand r = seedRand(SEED);
    int choice = genRandLong(&r) % board->movements_nbr;
    play_move(board, board->movements[choice]);
}


void aiPlay(BOARD* board)
{
    COORDS* movement = bestChoice(board);
    play_move(board, *movement);
    free(movement);
}

void testCp()
{
    int playerRole[4] = {3, 2, 2, 2};
    FN_PLAY play[] = {humanPlay, randomPlay, aiPlay};
    int p1_winner = 0;
    BOARD board;
    for(int i = 0; i < 100; i++) // number of game simulate
    {

        init_game(&board);
        while(!end_game(&board))
        {
            play[playerRole[board.player]-1](&board); // le joueur courant joue son tour
            //display_game(&board);
            getchar();
        }
        int pWinner = winner(&board);
        printf("winner is player %c\n", SYMBOL[pWinner]);
        if(pWinner == 0)
            p1_winner++;
    }
    printf("\n the ia won %d times", p1_winner);
}

int main()
{
	int *playerRole = role();

	BOARD board;
	FN_PLAY play[] = {humanPlay, randomPlay, aiPlay};
	init_game(&board);
	display_game(&board);

	#if DEBUG == 1 // on appelle juste une fois la fonction iaJoue pour test
        aiPlay(&board);
        return 0;
    #endif // DEBUG
	while(!end_game(&board))
    {
            /*if(loser(&board))
                break;*/
            play[playerRole[board.player]-1](&board); // le joueur courant joue son tour
            display_game(&board);
    }
    printf("winner is player %c\n", SYMBOL[winner(&board)]);
    free(playerRole);
}

