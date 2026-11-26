#ifndef OPENSIGMAR_SIGMAR_H
#define OPENSIGMAR_SIGMAR_H

/**
 * TODO: Replace sound effects and music with custom sound effects and music.
 *
 *
 */

#define ADJACENCY_OFFSETS(BOARD_SIZE) {-(BOARD_SIZE), 1, (BOARD_SIZE) + 1, (BOARD_SIZE), -1, -(BOARD_SIZE) - 1}

#define MAX_SELECTED (4)

#define BOARD_SIZE (11)
#define BOARD_LENGTH (BOARD_SIZE * BOARD_SIZE)
#define CENTER_TILE ((int)(BOARD_LENGTH * 0.5)) // Index of the center tile of the board.
#define CENTER_ROW ((CENTER_TILE - (int)(BOARD_SIZE * 0.5))) // Index of the first tile of the center row of the board.

#define NUM_PATTERNS (10)
#define PATTERN_LENGTH (55) // Length of each of the preset patterns, AKA, the total number of marbles on a newly-generated board.

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

#define NUM_MARBLES (MARBLE_GOLD + 1) // Should be set to whatever the last "normal" marble type is, plus one.

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

/*
 * Game state structure.
 */
typedef struct Sigmar {
    int adjacency_offsets[6]; // Stores the offsets used to calculate the six adjacent tile indices from any given index.

    unsigned int turn; // Turn counter. (Unused) (Probably forever lol)

    unsigned char pattern; // Pattern used by the current board.
    Marble board[BOARD_LENGTH]; // Array of Marble states for each slot on the board.
    char marble_counts[NUM_MARBLES]; // Counts of each marble type on the board.

    // Array of previously selected marbles types & indicies.
    // Typically only the first index is used except in cases where more than two marbles can be matched.
    // The first index is considered the primary selection, the rest are considered auxillary selections.
    MarbleType prev_selected[MAX_SELECTED];
    int prev_selected_indices[MAX_SELECTED];
    int num_selected; // Number of previously selected marbles.

    EventCallback callback[EVENT_INVALID]; // Callback functions for each valid EventID.
} Sigmar;

extern const unsigned char patterns[NUM_PATTERNS][55];
extern const unsigned char pattern_anchors[NUM_PATTERNS][6]; // Indices from which board generation originates when using sigmar_ShuffleEasy().

/*
 * Initializes a new Sigmar instance.
 */
int sigmar_Init(Sigmar **sigmar);

/*
 * Binds an event callback function to the given EventID.
 */
void sigmar_Bind(Sigmar *sigmar, EventID event, EventCallback callback);

/*
 * Unbinds the event callback function from the given EventID.
 */
void sigmar_Unbind(Sigmar *sigmar, EventID event);

/*
 * Resets the game state & calls bound callback for EVENT_BOARD_RESET.
 */
void sigmar_Reset(Sigmar *sigmar);

/*
 * Cleans the board to a blank slate.
 */
void sigmar_CleanBoard(Sigmar *sigmar);

/*
 * Do not call outside of sigmar_GenerateBoard().
 *
 * Generates a shuffled array of tile indices using an algorithm that is more likely to generate easier puzzles.
 */
int sigmar_ShuffleEasy(Sigmar *sigmar, unsigned char *shuffled_tiles, unsigned char *shuffled_tiles_length);

/*
 * Do not call outside of sigmar_GenerateBoard().
 *
 * Generates a shuffled array of tile indices using an algorithm that is more likely to generate harder puzzles.
 */
int sigmar_ShuffleHard(Sigmar *sigmar, unsigned char *shuffled_tiles, unsigned char *shuffled_tiles_length);

/*
 * Attempts to generate a new board.
 * 
 * If difficulty is set to 0, uses "easy" board generation. If set to 1, uses "hard" board generation.
 * 
 * Returns GENERATION_FAIL if board generation gets stuck, otherwise returns GENERATION_SUCCESS.
 */
GenerationResult sigmar_GenerateBoard(Sigmar *sigmar, unsigned int seed, unsigned char difficulty);

/*
 * Checks if a board is winnable.
 *
 * Returns 1 if it can be cleared, otherwise returns 0;
 */
int sigmar_IsBoardWinnable(Sigmar *sigmar);

/*
 * Attempts to select marble at given index and attempts a match if applicable.
 * 
 * If a match occurs and as a result the board is cleared, calls bound callback for EVENT_BOARD_CLEAR.
 */
SelectResult sigmar_Select(Sigmar* sigmar, unsigned int index);

/*
 * Calculates the offsets of the six indices adjacent to any given index.
 */
void sigmar_CalcAdjacencyOffsets(const Sigmar* sigmar, int adj[6]);

/*
 * Checks whether or not the marble at a given index is free.
 */
void sigmar_IsFree(Sigmar* sigmar, unsigned int index, MarbleState* result);

/*
 * Checks whether or not the marble at a given index is blocked by adjacent marbles.
 */
void sigmar_IsBlocked(Sigmar* sigmar, unsigned int index, MarbleState* result);

/*
 * Checks whether a given marble can be matched with previously selected marbles.
 * 
 * Returns MATCH_SUCCESS if match is successful, or MATCH_FAIL if match is unsuccessful.
 */
MatchResult sigmar_Matches(MarbleType *prev_selected, int num_selected, MarbleType selection);

/*
 * Returns 1 if the board has been cleared, 0 otherwise.
 */
int sigmar_IsBoardClear(Sigmar *sigmar);

/*
 * Updates the state of all marbles on the board.
 */
void sigmar_UpdateMarbles(Sigmar *sigmar);

/*
 * Deserializes board data from a previously serialized board.
 */
int sigmar_DeserializeBoard(Sigmar *sigmar, const char *board_data);

/*
 * Serializes the current board to a character array that can be later read by sigmar_ImportBoard()
 */
int sigmar_SerializeBoard(Sigmar *sigmar, char *board_data);

#endif //OPENSIGMAR_SIGMAR_H