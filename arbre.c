#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "arbre.h"
#include "joust.h"

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


float calculUCT(P_NODE proot)
{
    return proot->averageVictory + 2*Cp*sqrt((2*log(totalPassage(proot)))/proot->passage);
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
    float tmp[proot->board->movements_nbr]; // array that will contain UCT value to each sons
    for(int i = 0; i < proot->board->movements_nbr; i++)
    {
        #if DEBUG == 1
        printf("\nj: %d\n", i);
        #endif // DEBUG
        if(proot->son[i]->passage == 0 || totalPassage(proot) == 0){ // infinite value
            #if DEBUG == 1
            printf("\n infinite value\n");
            #endif // DEBUG
            return proot->son[i];
        }
        if(proot->son[i]->ignore == 1)
            tmp[i] = 0;
        else
            tmp[i] = calculUCT(proot->son[i]);
        #if DEBUG == 1
        printf("\nfils[%d] uct: %f\n", i, tmp[i]);
        #endif // DEBUG
    }
    return proot->son[indexMaxValue(tmp, proot->board->movements_nbr)];
}

void createSon(P_NODE proot)
{
    proot->son = (P_NODE *) malloc(sizeof(P_NODE)*proot->board->movements_nbr);
    for(int i = 0; i < proot->board->movements_nbr; i++)
    {
        BOARD *copyBoard = (BOARD *) malloc(sizeof(BOARD));
        clone_game(copyBoard, proot->board);
        P_NODE tmp = (NODE *) malloc(sizeof(NODE));
        tmp->passage = 0;
        tmp->victory = 0;
        tmp->averageVictory = 0;
        tmp->son = NULL;
        //proot->remainingSon = NULL;
        tmp->father = proot;
        play_move(copyBoard, proot->board->movements[i]);
        tmp->board = copyBoard;
        tmp->lastMovement = &proot->board->movements[i];
        proot->son[i] = tmp;
    }
}

void propagate(P_NODE proot, int victory)
{
    while(proot != NULL) // if proot == NULL, it means proot is the father of the root which doesn't exist.
    {
        proot->victory += victory;
        proot->passage += 1;
        proot->averageVictory = (float) proot->victory/proot->passage;
        proot = proot->father;
    }
}

// proot will always have at least one son
P_NODE maxNode(P_NODE proot)
{
    P_NODE tmp = proot->son[0];
    for(int i = 1; i < proot->board->movements_nbr; i++)
    {
        if(proot->son[i]->averageVictory > tmp->averageVictory)
        {
            tmp = proot->son[i];
        }
    }
    return tmp;
}

/**
 * check if all the children are ignored and if yes ignore the father
 */
void ignoreFather(P_NODE pfather)
{
    if(pfather == NULL)
        return;
    for(int i = 0; i < pfather->board->movements_nbr; i++)
    {
        if(!pfather->son[i]->ignore)
        {
            return;
        }
    }
    pfather->ignore = 1;
    ignoreFather(pfather->father);
}

int endOfBranch(int iaPlayerNumber, P_NODE pracine)
{
    BOARD *copyBoard = (BOARD *) malloc(sizeof(BOARD));
    clone_game(copyBoard, pracine->board);
    int iaWin = 1;
    for(int i = 0; i < 4; i++) // for each player
    {
        if(copyBoard->player == iaPlayerNumber && copyBoard->movements_nbr == 0) // if the ia loose
            return 1;
        if(copyBoard->player != iaPlayerNumber && copyBoard->movements_nbr > 0) // if one other player is still alive
            iaWin = 0;

        // switch the board for the next player (without make any move)
        copyBoard->player = (copyBoard->player + 1) % 4;
        copyBoard->movements_nbr = generate_possible_movements(copyBoard, copyBoard->movements, copyBoard->player);
    }
    if(iaWin) // if the ia win
        return 1;
    return 0; // if the ia is still playing
}

/**
 *
 */
void deleteTree(P_NODE proot)
{
    if(proot == NULL)
        return;
    for(int i = 0; i < proot->board->movements_nbr; i++) // for each sons
    {
        if(proot->son == NULL)
            break;
        deleteTree(proot->son[i]);
    }
    free(proot->board);
    free(proot->son);
    free(proot);
    return;

}

COORDS* bestChoice(BOARD* board)
{
    P_NODE proot = (NODE *) malloc(sizeof(NODE));
    BOARD *copyBoard = (BOARD *) malloc(sizeof(BOARD));
    clone_game(copyBoard, board);
    proot->father = NULL;
    proot->son = NULL;
    //proot->remainingSon = NULL;
    proot->victory = 0;
    proot->averageVictory = 0;
    proot->ignore = 0;
    proot->passage = 0;
    proot->board = copyBoard;
    proot->lastMovement = NULL;
    for(int i = 0; i < SIMULATION; i++)
    {

        #if DEBUG == 1
        printf("\n i: %d \n", i);
        #endif // DEBUG
        P_NODE tmp = proot;
        if(proot->ignore == 1)
            break;
        //printf("a");
        while(tmp->passage > 0 || tmp->father == NULL)
        {
            // looking for best sons
            //printf("b");
            if(tmp->son == NULL)
                createSon(tmp);
            //printf("c");
            tmp = selectBestSon(tmp); // TODO rajouter la création des enfants s'il n'existe pass
            //printf("d");
            #if DEBUG == 1
            printf("\n === Selected Son ===\n");
            debug_node(tmp);
            #endif // DEBUG
        }
        //printf("z");
        //printf("player ia: %d\n", proot->board->player);
        //printf("current player: %d\n", tmp->board->player);
        if(endOfBranch(proot->board->player, tmp)){ // if the game is over for the ia
            tmp->ignore = 1;
            //printf("e");
            if(winner(tmp->board) == proot->board->player) // if the ia won
                propagate(tmp, 1);
            else
                propagate(tmp, 0);
            //printf("f");
            ignoreFather(tmp->father);
            //printf("g");
        } else {
            // simulate the current node
            // 1. we end the game randomly
            //printf("h");
            BOARD *tmpBoard = (BOARD *) malloc(sizeof(BOARD));
            //printf("i");
            clone_game(tmpBoard, tmp->board);
            //printf("j");
            finish_game_randomly(tmpBoard);
            //printf("k");
            // 2. we check if we lose or win
            if(tmpBoard->player != proot->board->player) // victory
            {
                propagate(tmp, 1); // 3. we propagate the result
            } else { // loss
                propagate(tmp, 0); // 3. we propagate the result
            }
            //printf("l");
            #if DEBUG == 1
            printf("\n === chosen node === \n");
            //debug_node(tmp);
            #endif // DEBUG
        }
    }
    printf("m");

    COORDS* bestMovement = maxNode(proot)->lastMovement;
    deleteTree(proot);
    return bestMovement;
}


// TODO destroy useless part of the tree






/// debug fonction utiliser pour le débuguage
/// attention certaines portions de code pourrais être considéré comme n'étant pas propre.
/// l'idée était surtout de me faire gagner du temps dans mes test.Puisque ces fonctionnalités ne sont pas explicitement demandé dans l'intitulé du sujet je me suis permis de les �crire en vitesse.

int get_son_number(P_NODE proot)
{
    if(proot->father == NULL)
        return 0;
    else
    {
        for(int i = 0; i < proot->father->board->movements_nbr; i++)
        {
            if(proot->father->board->movements[i].x == proot->lastMovement->x && proot->father->board->movements[i].y == proot->lastMovement->y)
                return i;
        }
        return 10000; // impossible donc on sait que c'est une erreur
    }
}

size_t max_row(char **row, int rowNumber)
{
    size_t max = strlen(row[0]);
    for(int i = 1; i < rowNumber; i++)
    {
        if(strlen(row[i]) > max)
            max = strlen(row[i]);
    }
    return max;
}

void display_message(char **row, int rowNumber)
{
    printf("\n\n\n\n");
    printf("\t");// ligne 1
    for(size_t i = 2; i < max_row(row, rowNumber); i++)
    {
        printf("#");
    }
    printf("\n");

    printf("\t#"); // ligne 2
    for(size_t i = 4; i < max_row(row, rowNumber); i++)
    {
        printf(" ");
    }
    printf("#");
    printf("\n");

    for(int i = 0; i < rowNumber; i++) // print les lignes de contenue
    {
        printf("%s", row[i]);
    }

    printf("\t#"); // ligne 4
    for(size_t i = 4; i < max_row(row, rowNumber); i++)
    {
        printf(" ");
    }
    printf("#");
    printf("\n");

    printf("\t");// ligne 5
    for(size_t i = 2; i < max_row(row, rowNumber); i++)
    {
        printf("#");
    }
    printf("\n\n");
}


void debug_node(P_NODE proot)
{
    if(proot != NULL)
    {
        char row1[100];
        char row2[100];
        char row3[100];
        char row4[100];
        char row5[100];
        char row6[100];
        char row7[100];
        int rowNumber = 5;
        char *rowArray[5] = {row1, row2, row3, row4, row5, row6, row7};
        sprintf(row1, "\t#    passage: %d", proot->passage);
        sprintf(row2, "\t#    victory: %d", proot->victory);
        sprintf(row3, "\t#    averageVictory: %f", proot->averageVictory);
        sprintf(row4, "\t#    ignore: %d", proot->ignore);
        if(proot->father == NULL){
            sprintf(row5, "\t#    last move: [NULL]");
            sprintf(row6, "\t#    son number: NULL");
        } else {
            sprintf(row5, "\t#    last move: [%d, %d]", proot->lastMovement->x, proot->lastMovement->y);
            sprintf(row6, "\t#    son number: %d", get_son_number(proot));
        }
        sprintf(row7, "\t#    player: %d", proot->board->player+1);
        size_t max = max_row(rowArray, rowNumber);
        for(int i = 0; i < rowNumber; i++)
        {
            int tmp = strlen(rowArray[i]);
            for(size_t j = 0; j < max-tmp; j++)
            {
                strcat(rowArray[i], " ");
            }
            strcat(rowArray[i], "    #\n");
        }
        display_message(rowArray, rowNumber);
        display_game(proot->board);
        printf("\n\n\n\n");
    }
}

