#include "Sigmar.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int sigmar_Init(Sigmar** sigmar) {
    *sigmar = (Sigmar*) malloc(sizeof(Sigmar));

    (*sigmar)->turn = 0;

    (*sigmar)->board_size = 11;

    memset((*sigmar)->marble_counts, 0, (int) NUM_MARBLES);

    sigmar_CalcAdjacencyOffsets((*sigmar), (*sigmar)->adjacency_offsets);

    sigmar_GenerateBoard(*sigmar);

    (*sigmar)->num_selected = 0;

    return 0;
}

int sigmar_GenerateBoard(Sigmar *sigmar) {
    const int board_len = sigmar->board_size * sigmar->board_size;
    sigmar->board = (Marble*) malloc(sizeof(Marble) * board_len);

    for (int i = 0; i < board_len; i++) {
        sigmar->board[i] = EMPTY;
    }

    // Sorta complicated math to determine which indices in the board need to be set as invalid.
    const int center_row = (int) (sigmar->board_size * sigmar->board_size * 0.5 - sigmar->board_size * 0.5);
    for (int i = 1; i <= sigmar->board_size / 2; i++) {
        for (int x = 1; x < i + 1; x++) {
            sigmar->board[center_row - (sigmar->board_size * (i - 1)) - x] = INVALID;
            sigmar->board[center_row + (sigmar->board_size * i) + x - 1] = INVALID;
        }
    }

    // EXTREMELY RUDIMENTARY BOARD GENERATION, AVERT YOUR EYES
    // TODO: Make board generation good instead of bad.

    Marble bucket[][2] = {{SALT, SALT},
                          {SALT, SALT},
                          {FIRE, FIRE},
                          {FIRE, FIRE},
                          {FIRE, FIRE},
                          {FIRE, FIRE},
                          {WATER, WATER},
                          {WATER, WATER},
                          {WATER, WATER},
                          {WATER, WATER},
                          {EARTH, EARTH},
                          {EARTH, EARTH},
                          {EARTH, EARTH},
                          {EARTH, EARTH},
                          {AIR, AIR},
                          {AIR, AIR},
                          {AIR, AIR},
                          {AIR, AIR},
                          {VITAE, VITAE},
                          {VITAE, VITAE},
                          {MORS, MORS},
                          {MORS, MORS},
                          {QUICKSILVER, SILVER},
                          {QUICKSILVER, COPPER},
                          {QUICKSILVER, IRON},
                          {QUICKSILVER, TIN},
                          {QUICKSILVER, LEAD}
                        };

    srand(time(0));
    
    // Goes through pattern and collects all valid indices.

    char states[sigmar->board_size * sigmar->board_size];
    memset(&states, 0, sizeof(states));
    int valid_indices[sigmar->board_size * sigmar->board_size];

    valid_indices[0] = (int) sigmar->board_size * sigmar->board_size * 0.5;
    int i = 0;
    int next = 1;

    states[valid_indices[i]] = 1;

    while (valid_indices[i] != -1) {
        // Go through adjacent tiles
        for (int adj = 0; adj < 6; adj++) {
            valid_indices[next] = valid_indices[i] + sigmar->adjacency_offsets[adj];

            if (valid_indices[next] >= 0 && valid_indices[next] < sigmar->board_size * sigmar->board_size && pattern[valid_indices[next]] == 1) {
                // Valid
                if (states[valid_indices[next]] == 0) {
                    states[valid_indices[next]] = 1;
                    next++;
                }

            } else {
                // Invalid
                valid_indices[next] = -1; // Set priority[next] to -1. if curr catches up to next and reached -1, then we've run out of free tiles to select.

            }
        }
        
        i++;
    }

    sigmar->marble_counts[GOLD] += 1;
    sigmar->board[valid_indices[0]] = GOLD;

    // Sets marbles in pairs in order from valid_indices.
    for (int i = 1; i < next; i+=2) {
        int picked = 0;
        while (picked == 0) {
            int b = rand() % 27;
            if (bucket[b][0] != INVALID) {
                sigmar->marble_counts[bucket[b][0]] += 1; // Increment marble_counts, but I'll implement this better later.
                sigmar->marble_counts[bucket[b][1]] += 1; // Increment marble_counts, but I'll implement this better later.
                sigmar->board[valid_indices[i]] = bucket[b][0];
                sigmar->board[valid_indices[i + 1]] = bucket[b][1];
                bucket[b][0] = INVALID;
                picked = 1;
            }
        }
    }

    return 1;
}

SelectResult sigmar_Select(Sigmar* sigmar, const unsigned int index) {
    if (sigmar == NULL) { return SELECT_ERROR; }

    if (index >= sigmar->board_size * sigmar->board_size || index < 0) { return SELECT_ERROR; }

    if (sigmar->board[index] == INVALID) { return SELECT_ERROR; }

    if (sigmar->prev_selected[sigmar->num_selected - 1].index == index) {
        sigmar->num_selected -= 1;

    } else {
        MatchResult rc = sigmar_Matches(sigmar, sigmar->prev_selected, sigmar->num_selected, sigmar->board[index]);

        if (rc == MATCH_ERROR) { return SELECT_ERROR; }

        switch (rc) {
            case MATCH_FAIL: // MATCH_FAIL exists purely for readability.
            default:
                break;
            case MATCH_CONTINUE: // Adds the selection to the prev_selected array and awaits another selection.
                sigmar->prev_selected[sigmar->num_selected].index = index;
                sigmar->prev_selected[sigmar->num_selected].marble = sigmar->board[index];
                sigmar->num_selected += 1;

                break;
            case MATCH_SUCCESS: // Removes all selected marbles and decrements marble_counts accordingly.
                for (int i = 0; i < sigmar->num_selected; i++) {
                    SelectedMarble s = sigmar->prev_selected[i];
                    sigmar->marble_counts[s.marble] -= 1;
                    sigmar->board[s.index] = EMPTY;
                }

                sigmar->marble_counts[sigmar->board[index]] -= 1;
                sigmar->board[index] = EMPTY;

                sigmar->num_selected = 0;

                return SELECT_MATCH;            
        }
    }

    return SELECT_SUCCESS;
}

int sigmar_CalcAdjacencyOffsets(const Sigmar *sigmar, int adj[6]) {
    // Adjacency offsets are ordered from top-right clockwise to top-left, so that sigmar_IsFree() can be calculated by just looping through 1.5x.
    if (sigmar == NULL) { return 1; }
    adj[0] = -sigmar->board_size;
    adj[1] = 1;
    adj[2] = sigmar->board_size + 1;
    adj[3] = sigmar->board_size;
    adj[4] = -1;
    adj[5] = -sigmar->board_size - 1;
    return 0;
}

int sigmar_IsFree(Sigmar* sigmar, unsigned int index, int* result) {
    if (sigmar == NULL) { return 1; }

    int contiguous = 0; // Contiguous free adjacent index counter.

    *result = 0;

    switch (sigmar->board[index]) {
        case TIN:
        case IRON:
        case COPPER:
        case SILVER:
        case GOLD:
            // If the marble is a metal, it can only be free if all metals of lesser tiers are depleted.
            if (sigmar->marble_counts[sigmar->board[index] - 1] > 0) { *result = 0; return 0; }
            // Otherwise, continues into default block.
        default:
            // Checks if there are at least three contiguous free indices adjacent to given index.
            for (int i = 0; i < 9; i++) {
                int adj = index + sigmar->adjacency_offsets[i % 6];

                if (adj >= 0 &&
                    adj < sigmar->board_size * sigmar->board_size &&
                    sigmar->board[adj] != EMPTY &&
                    sigmar->board[adj] != INVALID) { contiguous = 0; }
                else { contiguous++; }

                if (contiguous == 3) { *result = 1; return 0; }
            }
            break;
    }

    return 0;
}

MatchResult sigmar_Matches(Sigmar *sigmar, SelectedMarble *prev_selected, int num_selected, Marble selection) {
    if (selection == EMPTY) { return MATCH_ERROR; }

    // Only gold can be matched with nothing else, pass MATCH_CONTINUE for every other marble type.
    if (num_selected == 0 && selection != GOLD) { return MATCH_CONTINUE; }

    // Some things to note:
    //  Metals can *technically* always be matched as normal for their ruleset,
    //  however, they only become free when all metals of the previous tier are depleted.
    switch (selection) {
        case SALT:
            if (prev_selected[0].marble == FIRE ||
                prev_selected[0].marble == EARTH ||
                prev_selected[0].marble == WATER ||
                prev_selected[0].marble == AIR ||
                prev_selected[0].marble == selection) { return MATCH_SUCCESS; };
            break;
        case FIRE:
        case EARTH:
        case WATER:
        case AIR:
            if (prev_selected[0].marble == SALT ||
                prev_selected[0].marble == selection) { return MATCH_SUCCESS; };
            break;
        case VITAE:
            if (prev_selected[0].marble == MORS) { return MATCH_SUCCESS; };
            break;
        case MORS:
            if (prev_selected[0].marble == VITAE) { return MATCH_SUCCESS; };
            break;
        case QUICKSILVER:
            if (prev_selected[0].marble == LEAD ||
                prev_selected[0].marble == TIN ||
                prev_selected[0].marble == IRON ||
                prev_selected[0].marble == COPPER ||
                prev_selected[0].marble == SILVER) { return MATCH_SUCCESS; }
            break;
        case LEAD:
        case TIN:
        case IRON:
        case COPPER:
        case SILVER:
            if (prev_selected[0].marble == QUICKSILVER) { return MATCH_SUCCESS; }
            break;
        case GOLD:
            if (num_selected == 0) { return MATCH_SUCCESS; }
            break;
        default:
            break;
    }

    return MATCH_FAIL;
}