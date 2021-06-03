#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "arbre.h"
#include "joust.h"

#define DEBUG 1

#define Cp 0.3
#define SIMULATION 10



int totalPassage(P_NODE proot)
{
    while(proot->father != NULL)
    {
        proot = proot->father;
    }
    printf("\n total passage: %d", proot->passage);
    return proot->passage;
}


float calculUCT(P_NODE proot)
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
    float tmp[proot->board->movements_nbr]; // array that will contain UCT value to each sons
    for(int i = 0; i < proot->board->movements_nbr; i++)
    {
        printf("\nj: %d\n", i);
        if(proot->son[i]->passage == 0 || totalPassage(proot) == 0){ // infinite value
            printf("\n infinite value\n");
            return proot->son[i];
        }
        tmp[i] = calculUCT(proot->son[i]);
        printf("\nfils[%d] uct: %f\n", i, tmp[i]);
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
        tmp->son = NULL;
        proot->remainingSon = NULL;
        tmp->father = proot;
        play_move(copyBoard, proot->board->movements[i]);
        tmp->board = copyBoard;
        tmp->movementOrigin = &proot->board->movements[i];
        proot->son[i] = tmp;
    }
}

void propagate(P_NODE proot, int victory)
{
    while(proot->father != NULL)
    {
        proot->victory += victory;
        proot->passage += 1;
        proot = proot->father;
    }
    proot->victory += victory;
    proot->passage += 1;

}

// proot will always have at least one son
P_NODE maxNode(P_NODE proot)
{
    P_NODE tmp = proot->son[0];
    for(int i = 1; i < proot->board->movements_nbr; i++)
    {
        if(proot->son[i]->victory > tmp->victory)
        {
            tmp = proot->son[i];
        }
    }
    return tmp;
}

COORDS* bestChoice(BOARD* board)
{
    P_NODE proot = (NODE *) malloc(sizeof(NODE));
    BOARD *copyBoard = (BOARD *) malloc(sizeof(BOARD));
    clone_game(copyBoard, board);
    proot->father = NULL;
    proot->son = NULL;
    proot->remainingSon = NULL;
    proot->victory = 0;
    proot->passage = 0;
    proot->board = copyBoard;
    proot->movementOrigin = NULL;
    for(int i = 0; i < SIMULATION; i++)
    {
        printf("\n i: %d \n", i);
        P_NODE tmp = proot;
        printf("ok1");
        while(tmp->passage > 0 || tmp->father == NULL)
        {
            printf("ok2");
            // looking for best sons
            if(tmp->son == NULL){
                printf("ok3");
                createSon(tmp);
                printf("ok5");
            }
            tmp = selectBestSon(tmp); // TODO rajouter la création des enfants s'il n'existe pass
            printf("\n === Selected Son ===\n");
            debug_node(tmp);
            printf("ok6");
        }
        // simulate the current node
        // 1. we end the game randomly
        BOARD *tmpBoard = (BOARD *) malloc(sizeof(BOARD));
        clone_game(tmpBoard, tmp->board);
        finish_game_randomly(tmpBoard);
        printf("ok7");

        // 2. we check if we lose or win
        if(tmpBoard->player != proot->board->player) // victory
        {
            printf("ok8");
            propagate(tmp, 1); // 3. we propagate the result
        } else { // loss
            printf("ok9");
            propagate(tmp, 0); // 3. we propagate the result
        }
        #if DEBUG == 1
        printf("\n === chosen node === \n");
        debug_node(tmp);
        #endif // DEBUG
    }
    printf("ok10");
    return maxNode(proot)->movementOrigin;
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
            if(proot->father->board->movements[i].x == proot->movementOrigin->x && proot->father->board->movements[i].y == proot->movementOrigin->y)
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
        int rowNumber = 5;
        char *rowArray[5] = {row1, row2, row3, row4, row5};

        sprintf(row1, "\t#    passage: %d", proot->passage);
        sprintf(row2, "\t#    victory: %d", proot->victory);
        if(proot->father == NULL){
            sprintf(row3, "\t#    movementOrigin: [NULL]");
            sprintf(row4, "\t#    son number: NULL");
        } else {
            sprintf(row3, "\t#    movementOrigin: [%d, %d]", proot->movementOrigin->x, proot->movementOrigin->y);
            sprintf(row4, "\t#    son number: %d", get_son_number(proot));
        }
        sprintf(row5, "\t#    player: %d", proot->board->player+1);

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

