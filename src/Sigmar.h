#ifndef OPENSIGMAR_SIGMAR_H
#define OPENSIGMAR_SIGMAR_H

/**
 * TODO: Replace sound effects and music with sound effects and music that aren't directly ripped from the game.
 *
 *
 */

#define ADJACENCY_OFFSETS(BOARD_SIZE) {-(BOARD_SIZE), 1, (BOARD_SIZE) + 1, (BOARD_SIZE), -1, -(BOARD_SIZE) - 1}

#define MAX_SELECTED 4

// DO NOT MODIFY THIS ENUM UNLESS YOU KNOW WHAT YOU ARE DOING
// It is important to several calculations that they are ordered as they are.
//
typedef enum : char {
    MARBLE_SALT,
    MARBLE_FIRE,
    MARBLE_WATER,
    MARBLE_EARTH,
    MARBLE_AIR,
    MARBLE_VITAE,
    MARBLE_MORS,
    MARBLE_QUICKSILVER,
    MARBLE_LEAD,
    MARBLE_TIN,
    MARBLE_IRON,
    MARBLE_COPPER,
    MARBLE_SILVER,
    MARBLE_GOLD,
    MARBLE_INVALID,
    MARBLE_EMPTY, // Invalid marble type but still represents a valid location on the board.
    MARBLE_PLACEHOLDER // Used during board generation.
} MarbleType;

#define NUM_MARBLES MARBLE_GOLD + 1 // Should be set to whatever the last "normal" marble type is, plus one.

typedef enum : char {
    MARBLE_LOCKED,
    MARBLE_FREE,
    MARBLE_SELECTED
} MarbleState;

typedef struct Marble {
    MarbleType type;
    MarbleState state;
} Marble;

typedef enum : char {
    GENERATION_FAIL,
    GENERATION_SUCCESS,
    GENERATION_ERROR
} GenerationResult;

typedef enum : char {
    MATCH_FAIL,
    MATCH_SUCCESS,
    MATCH_CONTINUE,
    MATCH_ERROR
} MatchResult;

typedef enum : char {
    SELECT_FAIL,
    SELECT_SUCCESS,
    SELECT_DESELECT,
    SELECT_MATCH,
    SELECT_ERROR
} SelectResult;

typedef struct SelectedMarble {
    unsigned short index;
    Marble marble;
} SelectedMarble;

typedef enum : char {
    EVENT_BOARD_RESET,
    EVENT_BOARD_CLEAR,
    EVENT_INVALID
} EventID;

typedef void (* EventCallback)(void);

// Temp pattern for testing
#define PATTERN {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,\
                 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,\
                 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0,\
                 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0,\
                 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0,\
                 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,\
                 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 0,\
                 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0,\
                 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0,\
                 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1,\
                 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0}

static char pattern[] = PATTERN;

/*
 * Game state structure.
 */
typedef struct Sigmar {
    int adjacency_offsets[6]; // Stores the offsets used to calculate the six adjacent tile indices from any given index.

    unsigned int turn; // Turn counter. (Unused)

    unsigned short board_size; // Size of the board.
    Marble *board; // Array of Marble states for each slot on the board.

    char marble_counts[NUM_MARBLES]; // Counts of each marble type on the board.

    // Array of previously selected marbles.
    // Typically only the first index is used except in cases where more than two marbles can be matched.
    // The first index is considered the primary selection, the rest are considered auxillary selections.
    SelectedMarble prev_selected[MAX_SELECTED];
    int num_selected; // Number of previously selected marbles.

    EventCallback callback[EVENT_INVALID]; // Callback functions for each valid EventID.
} Sigmar;

/*
 * Initializes a new Sigmar instance.
 */
int sigmar_Init(Sigmar **sigmar);

/*
 * Binds an event callback function to the given EventID.
 */
int sigmar_Bind(Sigmar *sigmar, EventID event, EventCallback callback);

/*
 * Unbinds the event callback function from the given EventID.
 */
int sigmar_Unbind(Sigmar *sigmar, EventID event);

/*
 * Resets the game & generates a new board.
 */
int sigmar_ResetBoard(Sigmar *sigmar);

/*
 * Attempts to generate a new board.
 * 
 * Returns GENERATION_FAIL if board generation gets stuck, otherwise returns GENERATION_SUCCESS.
 */
GenerationResult sigmar_GenerateBoard(Sigmar* sigmar);

/*
 * TODO: Implement this.
 * Checks if a board is winnable.
 *
 * Returns 1 if it can be cleared, otherwise returns 0;
 */
int sigmar_IsBoardWinnable(Sigmar *sigmar);

/*
 * Attempts to select marble at given index and attempts a match if applicable.
 */
SelectResult sigmar_Select(Sigmar* sigmar, unsigned int index);

/*
 * Calculates the offsets of the six indices adjacent to any given index.
 */
int sigmar_CalcAdjacencyOffsets(const Sigmar* sigmar, int adj[6]);

/*
 * Checks whether or not the marble at a given index is free.
 */
int sigmar_IsFree(Sigmar* sigmar, unsigned int index, MarbleState* result);

/*
 * Checks whether or not the marble at a given index is blocked by adjacent marbles.
 */
int sigmar_IsBlocked(Sigmar* sigmar, unsigned int index, MarbleState* result);

/*
 * Checks whether a given marble can be matched with previously selected marbles.
 * 
 * Returns MATCH_SUCCESS if match is successful, or MATCH_FAIL if match is unsuccessful.
 */
MatchResult sigmar_Matches(Sigmar *sigmar, SelectedMarble *prev_selected, int num_selected, Marble selection);

/*
 * Returns 1 if the board has been cleared, 0 otherwise.
 */
int sigmar_IsBoardClear(Sigmar *sigmar);

/*
 * Updates the state of all marbles on the board.
 */
int sigmar_UpdateMarbles(Sigmar *sigmar);

#endif //OPENSIGMAR_SIGMAR_H