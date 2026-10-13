#include "Sigmar.h"

#include <stdlib.h>

int sigmar_Init(Sigmar** sigmar) {
    *sigmar = (Sigmar*) malloc(sizeof(Sigmar));

    (*sigmar)->turn = 0;

    (*sigmar)->board_size = 11;

    sigmar_GenerateBoard(*sigmar);

    (*sigmar)->last_selection = -1;

    return 0;
}

int sigmar_GenerateBoard(Sigmar *sigmar) {
    const int board_len = sigmar->board_size * sigmar->board_size;
    sigmar->board = (Marble*) malloc(sizeof(Marble) * board_len);

    for (int i = 0; i < board_len; i++) {
        sigmar->board[i] = SALT;
    }

    sigmar->board[0] = AIR;
    sigmar->board[1] = AIR;
    sigmar->board[2] = WATER;
    sigmar->board[5] = EARTH;
    sigmar->board[6] = EARTH;
    sigmar->board[7] = WATER;
    sigmar->board[8] = FIRE;
    sigmar->board[10] = FIRE;

    // sorta complicated math to determine which indices in the board need to be set as invalid.
    const int center_row = (int) (sigmar->board_size * sigmar->board_size * 0.5 - sigmar->board_size * 0.5);
    for (int i = 1; i <= sigmar->board_size / 2; i++) {
        for (int x = 1; x < i + 1; x++) {
            sigmar->board[center_row - (sigmar->board_size * (i - 1)) - x] = INVALID;
            sigmar->board[center_row + (sigmar->board_size * i) + x - 1] = INVALID;
        }
    }

    return 1;
}

int sigmar_Select(Sigmar* sigmar, const unsigned int index) {
    if (sigmar == NULL) { return 1; }

    if (index >= sigmar->board_size * sigmar->board_size || index < 0) { return 2; }

    if (sigmar->board[index] == INVALID) { return 3; }

    if (sigmar->last_selection == -1) {
        sigmar->last_selection = index;

    } else if (sigmar->last_selection == index) {
        sigmar->last_selection = -1;

    } else {
        int matches = 0;
        const int rc = sigmar_Matches(sigmar->board[sigmar->last_selection], sigmar->board[index], &matches);

        if (rc == 0 && matches == 1) {
            // Do stuff
            sigmar->board[sigmar->last_selection] = EMPTY;
            sigmar->board[index] = EMPTY;
            // TEMP
            sigmar->last_selection = -1;
        }
    }

    return 0;
}

int sigmar_CalcAdjacencyOffsets(int adj[6], const Sigmar *sigmar) {
    if (sigmar == NULL) { return 1; }
    adj[0] = -1;
    adj[1] = 1;
    adj[2] = -sigmar->board_size;
    adj[3] = -sigmar->board_size - 1;
    adj[4] = sigmar->board_size;
    adj[5] = sigmar->board_size + 1;
    return 0;
}

int sigmar_Matches(const Marble a, const Marble b, int *result) {
    if (a == EMPTY || b == EMPTY) {
        return 1;
    }

    *result = 1;

    return 0;
}