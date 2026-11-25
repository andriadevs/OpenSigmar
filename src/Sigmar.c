#include "Sigmar.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

const unsigned char bag_counts[NUM_MARBLES] = {
    [MARBLE_SALT] = 4,
    [MARBLE_FIRE] = 8,
    [MARBLE_WATER] = 8,
    [MARBLE_EARTH] = 8,
    [MARBLE_AIR] = 8,
    [MARBLE_VITAE] = 4,
    [MARBLE_MORS] = 4,
    [MARBLE_QUICKSILVER] = 5,
    [MARBLE_LEAD] = 1,
    [MARBLE_TIN] = 1,
    [MARBLE_IRON] = 1,
    [MARBLE_COPPER] = 1,
    [MARBLE_SILVER] = 1,
    [MARBLE_GOLD] = 1
};

const unsigned char patterns[NUM_PATTERNS][55] = {
    [0] = {4,11,12,13,14,15,16,23,24,27,28,34,36,37,38,40,45,47,48,49,50,52,53,56,57,58,59,60,61,62,63,64,67,68,70,71,72,73,75,80,82,83,84,86,92,93,96,97,104,105,106,107,108,109,116},
    [1] = {4,11,12,13,14,15,16,23,24,27,28,34,36,37,38,40,45,47,48,49,50,52,53,56,57,58,59,60,61,62,63,64,67,68,70,71,72,73,75,80,82,83,84,86,92,93,96,97,104,105,106,107,108,109,116}
};

int sigmar_Init(Sigmar** sigmar) {
    *sigmar = (Sigmar*) malloc(sizeof(Sigmar));

    sigmar_CalcAdjacencyOffsets((*sigmar), (*sigmar)->adjacency_offsets);

    sigmar_Reset(*sigmar);

    return 0;
}

void sigmar_Bind(Sigmar *sigmar, EventID event, EventCallback callback) { sigmar->callback[event] = callback; }

void sigmar_Unbind(Sigmar *sigmar, EventID event) { sigmar->callback[event] = NULL; }

void sigmar_Reset(Sigmar *sigmar) {
    if (sigmar == NULL) { return; }

    sigmar->turn = 0;
    sigmar->num_selected = 0;

    memset(sigmar->marble_counts, 0, (int) NUM_MARBLES);

    for (int i = 0; i < BOARD_LENGTH; i++) { sigmar->board[i].type = MARBLE_EMPTY; sigmar->board[i].state = MARBLE_LOCKED; }

    // This chunk of code calculates the corners of the board that should be chopped off to make the board properly hexagonal.
    for (int i = 1; i <= BOARD_SIZE / 2; i++) {
        for (int x = 1; x < i + 1; x++) {
            sigmar->board[CENTER_ROW - (BOARD_SIZE * (i - 1)) - x].type = MARBLE_INVALID;
            sigmar->board[CENTER_ROW + (BOARD_SIZE * i) + x - 1].type = MARBLE_INVALID;
        }
    }

    if (sigmar->callback[EVENT_BOARD_RESET] != NULL) { sigmar->callback[EVENT_BOARD_RESET](); }
}

int sigmar_ShuffleEasy(Sigmar *sigmar, unsigned char *shuffled_tiles, unsigned char *shuffled_tiles_length) {
    // Attempts to generate a winnable board, generally generates an easier boards than pure random generation + checking if board is winnable afterwards.
    int next_index[MAX_SELECTED + 1];
    int next_free_tile[MAX_SELECTED + 1];
    int match_size = 2;
    char is_next_valid = 0;
    char fails = 0;

    int adjacent_tile;
    int aux_adjacent_tile;
    int aux_contiguous_free_count;

    unsigned char valid_tiles[BOARD_LENGTH];
    memset(valid_tiles, 0, BOARD_LENGTH);
    for (int i = 0; i < 55; i++) { valid_tiles[patterns[sigmar->pattern][i]] = 1; }

    int free_tiles[BOARD_LENGTH];
    int free_tiles_length = 0;

    for (int i = 0; i < 6; i++) {
        int adj = CENTER_TILE + sigmar->adjacency_offsets[i];
        if (adj >= 0 &&
            adj < BOARD_LENGTH &&
            valid_tiles[adj] == 1) { sigmar->board[adj].type = MARBLE_PLACEHOLDER; free_tiles[free_tiles_length] = adj; free_tiles_length++; }
    }

    while (free_tiles_length > 0) {
        for (int i = 0; i < match_size; i++) { next_free_tile[i] = -1; }

        for (int i = 0; i < match_size; i++) {
            next_index[i] = rand() % free_tiles_length;
            next_free_tile[i] = free_tiles[next_index[i]];
            free_tiles[next_index[i]] = free_tiles[free_tiles_length - 1];
            free_tiles_length--;

            sigmar->board[next_free_tile[i]].type = MARBLE_SALT;
        }

        for (int i = 0; i < match_size; i++) {
            for (int adj = 0; adj < 6; adj++) {
                adjacent_tile = next_free_tile[i] + sigmar->adjacency_offsets[adj];
                aux_contiguous_free_count = 0;
                is_next_valid = 1;

                if (adjacent_tile < 0 ||
                    adjacent_tile >= BOARD_LENGTH ||
                    valid_tiles[adjacent_tile] == 0 ||
                    (sigmar->board[adjacent_tile].type != MARBLE_EMPTY && sigmar->board[adjacent_tile].type != MARBLE_PLACEHOLDER))
                    { continue; } // If index is already in shuffled indices, skip it.
                
                for (int aux_adj = 0; aux_adj < 9; aux_adj++) {
                    aux_adjacent_tile = adjacent_tile + sigmar->adjacency_offsets[aux_adj % 6];

                    if (aux_adjacent_tile >= 0 &&
                        aux_adjacent_tile < BOARD_LENGTH &&
                        sigmar->board[aux_adjacent_tile].type != MARBLE_EMPTY &&
                        sigmar->board[aux_adjacent_tile].type != MARBLE_PLACEHOLDER &&
                        sigmar->board[aux_adjacent_tile].type != MARBLE_INVALID) { aux_contiguous_free_count = 0; }
                    else { aux_contiguous_free_count++; }

                    if (aux_contiguous_free_count == 3) { break; }
                }

                // If aux_contiguous_free_count < 3, free_tile would cause adjacent_tile to no longer be free.
                if (aux_contiguous_free_count < 3) { is_next_valid = 0; break; }
            }

            if (is_next_valid == 0) { fails++; break; }
        }

        if (is_next_valid == 0) {
            for (int i = 0; i < match_size; i++) {
                if (next_free_tile[i] == -1) { continue; }

                sigmar->board[next_free_tile[i]].type = MARBLE_PLACEHOLDER;

                free_tiles[free_tiles_length] = next_free_tile[i];
                free_tiles_length++;
            }
        } else {
            fails = 0;

            for (int i = 0; i < match_size; i++) {
                shuffled_tiles[*shuffled_tiles_length] = next_free_tile[i];
                (*shuffled_tiles_length)++;
                
                sigmar->board[next_free_tile[i]].type = MARBLE_SALT;

                for (int adj = 0; adj < 6; adj++) {
                    adjacent_tile = next_free_tile[i] + sigmar->adjacency_offsets[adj];

                    if (adjacent_tile >= 0 &&
                        adjacent_tile < BOARD_LENGTH &&
                        valid_tiles[adjacent_tile] &&
                        sigmar->board[adjacent_tile].type == MARBLE_EMPTY)
                    { sigmar->board[adjacent_tile].type = MARBLE_PLACEHOLDER; free_tiles[free_tiles_length] = adjacent_tile; free_tiles_length++; }
                }
            }
        }

        if (fails >= 100) { return 0; }
    }

    return 1;
}

int sigmar_ShuffleHard(Sigmar *sigmar, unsigned char *shuffled_tiles, unsigned char *shuffled_tiles_length) {
    // Completely randomizes the shuffled_tiles array.
    unsigned char i, r, t;

    for (i = 0; i < 55; i++) {
        if (patterns[sigmar->pattern][i] != CENTER_TILE) { shuffled_tiles[*shuffled_tiles_length] = patterns[sigmar->pattern][i]; (*shuffled_tiles_length)++; }
    }

    for (i = 0; i < *shuffled_tiles_length; i++) {
        r = rand() % *shuffled_tiles_length;
        t = shuffled_tiles[r];
        shuffled_tiles[r] = shuffled_tiles[i];
        shuffled_tiles[i] = t;
    }

    return 1;
}

GenerationResult sigmar_GenerateBoard(Sigmar *sigmar, unsigned int seed) {
    if (sigmar == NULL) { return GENERATION_ERROR; }

    sigmar->pattern = 1;

    unsigned char shuffled_tiles[BOARD_LENGTH];
    unsigned char shuffled_tiles_length = 0;

    srand(seed);

    do {
        sigmar_Reset(sigmar);

        MarbleType bag[][2] = {
            {MARBLE_SALT, MARBLE_SALT},
            {MARBLE_SALT, MARBLE_SALT},
            {MARBLE_FIRE, MARBLE_FIRE},
            {MARBLE_FIRE, MARBLE_FIRE},
            {MARBLE_FIRE, MARBLE_FIRE},
            {MARBLE_FIRE, MARBLE_FIRE},
            {MARBLE_WATER, MARBLE_WATER},
            {MARBLE_WATER, MARBLE_WATER},
            {MARBLE_WATER, MARBLE_WATER},
            {MARBLE_WATER, MARBLE_WATER},
            {MARBLE_EARTH, MARBLE_EARTH},
            {MARBLE_EARTH, MARBLE_EARTH},
            {MARBLE_EARTH, MARBLE_EARTH},
            {MARBLE_EARTH, MARBLE_EARTH},
            {MARBLE_AIR, MARBLE_AIR},
            {MARBLE_AIR, MARBLE_AIR},
            {MARBLE_AIR, MARBLE_AIR},
            {MARBLE_AIR, MARBLE_AIR},
            {MARBLE_VITAE, MARBLE_MORS},
            {MARBLE_VITAE, MARBLE_MORS},
            {MARBLE_VITAE, MARBLE_MORS},
            {MARBLE_VITAE, MARBLE_MORS},
            {MARBLE_QUICKSILVER, MARBLE_SILVER},
            {MARBLE_QUICKSILVER, MARBLE_COPPER},
            {MARBLE_QUICKSILVER, MARBLE_IRON},
            {MARBLE_QUICKSILVER, MARBLE_TIN},
            {MARBLE_QUICKSILVER, MARBLE_LEAD}
        };

        sigmar->board[CENTER_TILE].type = MARBLE_GOLD;
        sigmar->marble_counts[MARBLE_GOLD] += 1;

        shuffled_tiles_length = 0;

        if (!sigmar_ShuffleEasy(sigmar, shuffled_tiles, &shuffled_tiles_length)) { continue; }
        //if (!sigmar_ShuffleHard(sigmar, shuffled_tiles, &shuffled_tiles_length)) { return GENERATION_FAIL; }

        for (int i = 0; i < shuffled_tiles_length; i+=2) {
            int picked = 0;

            while (!picked) {
                int c = rand() % 27;

                if (c < 22) {
                    if (bag[c][0] != MARBLE_INVALID) {
                        sigmar->marble_counts[bag[c][0]]++;
                        sigmar->marble_counts[bag[c][1]]++;
                        sigmar->board[shuffled_tiles[i]].type = bag[c][0];
                        sigmar->board[shuffled_tiles[i + 1]].type = bag[c][1];
                        bag[c][0] = MARBLE_INVALID;
                        picked = 1;
                    }
                } else {
                    for (int x = 22; x < 27; x++) {
                        if (bag[x][0] != MARBLE_INVALID) {
                            sigmar->marble_counts[bag[x][0]]++;
                            sigmar->marble_counts[bag[x][1]]++;
                            sigmar->board[shuffled_tiles[i]].type = bag[x][0];
                            sigmar->board[shuffled_tiles[i + 1]].type = bag[x][1];
                            bag[x][0] = MARBLE_INVALID;
                            picked = 1;
                            break;
                        }
                    }
                }
            }
        }

        sigmar_UpdateMarbles(sigmar);
    } while (!sigmar_IsBoardWinnable(sigmar));

    return GENERATION_SUCCESS;
}

int sigmar_recIsBoardWinnable(Sigmar *prev_state, unsigned char *prev_free_tiles, unsigned char prev_free_tiles_length) {
    Sigmar curr_state;
    memcpy(&curr_state, prev_state, sizeof(Sigmar));
    unsigned char free_tiles[BOARD_LENGTH];
    unsigned char free_tiles_length = 0;

    for (int i = 0; i < BOARD_LENGTH; i++) {
        if (curr_state.board[i].state == MARBLE_FREE) {
            free_tiles[free_tiles_length] = i;
            free_tiles_length++;
        }
    }

    SelectResult rc;
    for (int i = 0; i < free_tiles_length; i++) {
        rc = sigmar_Select(&curr_state, free_tiles[i]);

        if (rc == SELECT_MATCH) {
            if (sigmar_IsBoardClear(&curr_state) ||
                sigmar_recIsBoardWinnable(&curr_state, free_tiles, free_tiles_length))
            { return 1; }
        } else {
            sigmar_Select(&curr_state, free_tiles[i]); // Deselect

            for (int j = i + 1; j < free_tiles_length; j++) {
                sigmar_Select(&curr_state, free_tiles[i]);
                rc = sigmar_Select(&curr_state, free_tiles[j]);
                
                if (rc == SELECT_MATCH) {
                    if (sigmar_IsBoardClear(&curr_state) ||
                        sigmar_recIsBoardWinnable(&curr_state, free_tiles, free_tiles_length))
                    { return 1; }
                }

                sigmar_Select(&curr_state, free_tiles[j]); // Deselect
            }
        }
    }

    return 0;
}

int sigmar_IsBoardWinnable(Sigmar *sigmar) { return sigmar_recIsBoardWinnable(sigmar, NULL, 0); }

SelectResult sigmar_Select(Sigmar* sigmar, const unsigned int index) {
    if (sigmar == NULL ||
        index >= BOARD_LENGTH ||
        index < 0 ||
        sigmar->board[index].type == MARBLE_INVALID) { return SELECT_ERROR; }
    
    if (sigmar->num_selected > 0 && sigmar->prev_selected_indices[sigmar->num_selected - 1] == index) {
        sigmar->board[index].state = MARBLE_FREE;
        sigmar->num_selected -= 1;
        return SELECT_DESELECT;
    }

    int was_board_clear = 0;
    MatchResult rc = sigmar_Matches(sigmar->prev_selected, sigmar->num_selected, sigmar->board[index].type);

    if (rc == MATCH_ERROR) { return SELECT_ERROR; }

    switch (rc) {
        case MATCH_FAIL: // MATCH_FAIL exists purely for readability.
        default:
            for (int i = 0; i < sigmar->num_selected; i++) { sigmar->board[sigmar->prev_selected_indices[i]].state = MARBLE_FREE; }
            sigmar->num_selected = 1;
            sigmar->board[index].state = MARBLE_SELECTED;
            sigmar->prev_selected_indices[0] = index;
            sigmar->prev_selected[0] = sigmar->board[index].type;

            break;
        case MATCH_CONTINUE: // Adds the selection to the prev_selected array and awaits another selection.
            sigmar->board[index].state = MARBLE_SELECTED;
            sigmar->prev_selected_indices[sigmar->num_selected] = index;
            sigmar->prev_selected[sigmar->num_selected] = sigmar->board[index].type;
            sigmar->num_selected += 1;

            break;
        case MATCH_SUCCESS: // Removes all selected marbles and decrements marble_counts accordingly.
            for (int i = 0; i < sigmar->num_selected; i++) {
                MarbleType marble_type = sigmar->prev_selected[i];
                int marble_index = sigmar->prev_selected_indices[i];
                sigmar->marble_counts[marble_type] -= 1;
                sigmar->board[marble_index].type = MARBLE_EMPTY;
                sigmar->board[marble_index].state = MARBLE_FREE;
            }

            sigmar->marble_counts[sigmar->board[index].type] -= 1;
            sigmar->board[index].type = MARBLE_EMPTY;
            sigmar->board[index].state = MARBLE_FREE;

            sigmar->num_selected = 0;

            sigmar_UpdateMarbles(sigmar);

            if (sigmar_IsBoardClear(sigmar) &&
                !was_board_clear &&
                sigmar->callback[EVENT_BOARD_CLEAR] != NULL) { sigmar->callback[EVENT_BOARD_CLEAR](); }

            return SELECT_MATCH;            
    }

    return SELECT_SUCCESS;
}

void sigmar_CalcAdjacencyOffsets(const Sigmar *sigmar, int adj[6]) {
    // Adjacency offsets are ordered from top-right clockwise to top-left, so that sigmar_IsFree() can be calculated by just looping through 1.5x.
    if (sigmar == NULL) { return; }
    adj[0] = -BOARD_SIZE;
    adj[1] = 1;
    adj[2] = BOARD_SIZE + 1;
    adj[3] = BOARD_SIZE;
    adj[4] = -1;
    adj[5] = -BOARD_SIZE - 1;
}

void sigmar_IsFree(Sigmar* sigmar, unsigned int index, MarbleState *result) {
    if (sigmar == NULL) { return; }

    *result = MARBLE_LOCKED;

    switch (sigmar->board[index].type) {
        case MARBLE_TIN:
        case MARBLE_IRON:
        case MARBLE_COPPER:
        case MARBLE_SILVER:
        case MARBLE_GOLD:
            // If the marble is a metal, it can only be free if all metals of lesser tiers are depleted.
            if (sigmar->marble_counts[sigmar->board[index].type - 1] > 0) { *result = MARBLE_LOCKED; return; }
            // Otherwise, continues into default block.
        default:
            // Checks if there are at least three contiguous free indices adjacent to given index.
            sigmar_IsBlocked(sigmar, index, result);
            break;
    }
}

void sigmar_IsBlocked(Sigmar *sigmar, unsigned int index, MarbleState *result) {
    if (sigmar == NULL) { return; }

    int contiguous = 0; // Contiguous free adjacent index counter.

    *result = MARBLE_LOCKED;

    for (int i = 0; i < 9; i++) {
        int adj = index + sigmar->adjacency_offsets[i % 6];

        if (adj >= 0 &&
            adj < BOARD_LENGTH &&
            sigmar->board[adj].type != MARBLE_EMPTY &&
            sigmar->board[adj].type != MARBLE_PLACEHOLDER &&
            sigmar->board[adj].type != MARBLE_INVALID) { contiguous = 0; }
        else { contiguous++; }

        if (contiguous == 3) { *result = MARBLE_FREE; return; }
    }
}

MatchResult sigmar_Matches(MarbleType *prev_selected, int num_selected, MarbleType selection) {
    if (selection == MARBLE_EMPTY) { return MATCH_ERROR; }
    
    // Only gold can be matched with nothing else, pass MATCH_CONTINUE for every other marble type.
    if (num_selected == 0 && selection != MARBLE_GOLD) { return MATCH_CONTINUE; }

    // Something to note:
    //  Metals can *technically* always be matched as normal for their ruleset,
    //  however, they only become free when all metals of the previous tier are depleted.
    switch (selection) {
        case MARBLE_SALT:
            if (prev_selected[0] == MARBLE_FIRE ||
                prev_selected[0] == MARBLE_EARTH ||
                prev_selected[0] == MARBLE_WATER ||
                prev_selected[0] == MARBLE_AIR ||
                prev_selected[0] == selection) { return MATCH_SUCCESS; };
            break;
        case MARBLE_FIRE:
        case MARBLE_EARTH:
        case MARBLE_WATER:
        case MARBLE_AIR:
            if (prev_selected[0] == MARBLE_SALT ||
                prev_selected[0] == selection) { return MATCH_SUCCESS; };
            break;
        case MARBLE_VITAE:
            if (prev_selected[0] == MARBLE_MORS) { return MATCH_SUCCESS; };
            break;
        case MARBLE_MORS:
            if (prev_selected[0] == MARBLE_VITAE) { return MATCH_SUCCESS; };
            break;
        case MARBLE_QUICKSILVER:
            if (prev_selected[0] == MARBLE_LEAD ||
                prev_selected[0] == MARBLE_TIN ||
                prev_selected[0] == MARBLE_IRON ||
                prev_selected[0] == MARBLE_COPPER ||
                prev_selected[0] == MARBLE_SILVER) { return MATCH_SUCCESS; }
            break;
        case MARBLE_LEAD:
        case MARBLE_TIN:
        case MARBLE_IRON:
        case MARBLE_COPPER:
        case MARBLE_SILVER:
            if (prev_selected[0] == MARBLE_QUICKSILVER) { return MATCH_SUCCESS; }
            break;
        case MARBLE_GOLD:
            if (num_selected == 0) { return MATCH_SUCCESS; }
            break;
        default:
            break;
    }

    return MATCH_FAIL;
}

int sigmar_IsBoardClear(Sigmar *sigmar) {
    if (sigmar == NULL) { return 0; }

    for (int i = 0; i < NUM_MARBLES; i++) {
        if (sigmar->marble_counts[i] > 0) { return 0; }
    }

    return 1;
}

void sigmar_UpdateMarbles(Sigmar *sigmar) {
    if (sigmar == NULL) { return; }

    for (int i = 0; i < BOARD_LENGTH; i++) {
        if (sigmar->board[i].type != MARBLE_EMPTY &&
            sigmar->board[i].type != MARBLE_INVALID) { sigmar_IsFree(sigmar, i, &sigmar->board[i].state); }
        else { sigmar->board[i].state = MARBLE_LOCKED; }
    }
}

int sigmar_DeserializeBoard(Sigmar *sigmar, const char *board_data) {
    if (board_data[0] != 1) { return 1; }

    memset(sigmar->marble_counts, 0, NUM_MARBLES);
    unsigned char pattern = board_data[0];

    for (int i = 1; i < 56; i++) { sigmar->board[patterns[pattern][i - 1]].type = board_data[i]; sigmar->marble_counts[board_data[i]]++; }

    sigmar_UpdateMarbles(sigmar);

    return 0;
}

int sigmar_SerializeBoard(Sigmar *sigmar, char *board_data) {
    board_data[0] = sigmar->pattern;

    for (int i = 1; i < 56; i++) { board_data[i] = sigmar->board[patterns[sigmar->pattern][i - 1]].type; }

    return 0;
}