#ifndef OPENSIGMAR_SIGMAR_H
#define OPENSIGMAR_SIGMAR_H

typedef enum {
    INVALID,
    EMPTY,
    SALT,
    FIRE,
    WATER,
    EARTH,
    AIR,
    LIFE,
    DEATH,
    QUICKSILVER,
    LEAD,
    TIN,
    IRON,
    COPPER,
    SILVER,
    GOLD
} Marble;

typedef struct Sigmar {
    unsigned int turn; // Turn counter.

    unsigned short board_size; // Size of the board.
    Marble* board; // Array of Marble states for each slot on the board.

    unsigned int last_selection;
} Sigmar;

int sigmar_Init(Sigmar** sigmar);

int sigmar_GenerateBoard(Sigmar* sigmar);

// Attempts to select marble at given index, attempts match if applicable.
int sigmar_Select(Sigmar* sigmar, unsigned int index);

// Calculates the index offsets to get the six spots on the board adjacent to any given index.
int sigmar_CalcAdjacencyOffsets(int adj[6], const Sigmar* sigmar);

int sigmar_Matches(Marble a, Marble b, int* result);

#endif //OPENSIGMAR_SIGMAR_H