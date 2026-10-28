#include "Sigmar.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <stdio.h>

int sigmar_Init(Sigmar** sigmar) {
    int rc;

    *sigmar = (Sigmar*) malloc(sizeof(Sigmar));

    (*sigmar)->turn = 0;

    (*sigmar)->board_size = 11;

    memset((*sigmar)->marble_counts, 0, (int) NUM_MARBLES);

    sigmar_CalcAdjacencyOffsets((*sigmar), (*sigmar)->adjacency_offsets);

    rc = 1;
    while (rc != 0) {
        rc = sigmar_GenerateBoard(*sigmar);
    }

    //if (rc != 0) { return 1; }

    (*sigmar)->num_selected = 0;

    return 0;
}

int sigmar_GenerateBoard(Sigmar *sigmar) {
    const int board_length = sigmar->board_size * sigmar->board_size;

    sigmar->board = (Marble*) malloc(sizeof(Marble) * board_length);
    for (int i = 0; i < board_length; i++) { sigmar->board[i].type = MARBLE_EMPTY; sigmar->board[i].state = MARBLE_FREE; }

    MarbleType bucket[][2] = {{MARBLE_SALT, MARBLE_SALT},
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

    srand(time(NULL));

    int next_index[MAX_SELECTED + 1];
    int next_free_tile[MAX_SELECTED + 1];
    int match_size = 2;
    char is_next_valid = 0;
    char fails = 0;

    int adjacent_tile;
    int aux_adjacent_tile;
    int aux_contiguous_free_count;

    int center_tile = (int) sigmar->board_size * sigmar->board_size * 0.5; // Center index of the board, suffled_indices starts from here.

    int valid_tiles[] = PATTERN;

    int shuffled_tiles[board_length];
    int shuffled_tiles_end = 0;

    int free_tiles[board_length];
    int free_tiles_end = 0;

    for (int i = 0; i < 6; i++) {
        int adj = center_tile + sigmar->adjacency_offsets[i];
        if (adj >= 0 &&
            adj < board_length &&
            valid_tiles[adj] == 1) { sigmar->board[adj].type = MARBLE_PLACEHOLDER; free_tiles[free_tiles_end] = adj; free_tiles_end++; }
    }

    const int center_row = (int) (sigmar->board_size * sigmar->board_size * 0.5 - sigmar->board_size * 0.5);
    for (int i = 1; i <= sigmar->board_size / 2; i++) {
        for (int x = 1; x < i + 1; x++) {
            sigmar->board[center_row - (sigmar->board_size * (i - 1)) - x].type = MARBLE_INVALID;
            sigmar->board[center_row + (sigmar->board_size * i) + x - 1].type = MARBLE_INVALID;
        }
    }

    sigmar->board[center_tile].type = MARBLE_GOLD;
    sigmar->marble_counts[MARBLE_GOLD] += 1;

    while (free_tiles_end > 0) {
        // Pick some tiles
        for (int i = 0; i < match_size; i++) { next_free_tile[i] = -1; }

        for (int i = 0; i < shuffled_tiles_end; i++) {
            printf("%d, ", shuffled_tiles[i]);
        }
        printf("\n--\n");
        for (int i = 0; i < free_tiles_end; i++) {
            printf("%d, ", free_tiles[i]);
        }
        printf("\n++\n");
        fflush(NULL);

        //if (free_tiles_end > 54) {
        //    exit(0);
        //}

        for (int i = 0; i < match_size; i++) {
            next_index[i] = rand() % free_tiles_end;
            next_free_tile[i] = free_tiles[next_index[i]];
            free_tiles[next_index[i]] = free_tiles[free_tiles_end - 1];
            free_tiles_end--;

            sigmar->board[next_free_tile[i]].type = MARBLE_SALT;
        }

        for (int i = 0; i < match_size; i++) {

            for (int adj = 0; adj < 6; adj++) {
                adjacent_tile = next_free_tile[i] + sigmar->adjacency_offsets[adj];
                aux_contiguous_free_count = 0;
                is_next_valid = 1;

                if (adjacent_tile < 0 ||
                    adjacent_tile >= board_length ||
                    valid_tiles[adjacent_tile] == 0 ||
                    (sigmar->board[adjacent_tile].type != MARBLE_EMPTY && sigmar->board[adjacent_tile].type != MARBLE_PLACEHOLDER))
                    { continue; } // If index is already in shuffled indices, skip it.
                
                for (int aux_adj = 0; aux_adj < 9; aux_adj++) {
                    aux_adjacent_tile = adjacent_tile + sigmar->adjacency_offsets[aux_adj % 6];

                    if (aux_adjacent_tile >= 0 &&
                        aux_adjacent_tile < board_length &&
                        sigmar->board[aux_adjacent_tile].type != MARBLE_EMPTY &&
                        sigmar->board[aux_adjacent_tile].type != MARBLE_PLACEHOLDER &&
                        sigmar->board[aux_adjacent_tile].type != MARBLE_INVALID) { aux_contiguous_free_count = 0; }
                    else { aux_contiguous_free_count++; }

                    if (aux_contiguous_free_count == 3) { break; }
                }

                // If sub_contiguous < 3, free_index would cause adjacent_index to no longer be free.
                // So you need to pick a different index.
                //if (aux_free == 0) { is_next_valid = 0; break; }
                if (aux_contiguous_free_count < 3) { is_next_valid = 0; break; }
            }

            if (is_next_valid == 0) { fails++; break; }
        }

        if (is_next_valid == 0) {
            for (int i = 0; i < match_size; i++) {
                if (next_free_tile[i] == -1) { continue; }

                sigmar->board[next_free_tile[i]].type = MARBLE_PLACEHOLDER;

                free_tiles[free_tiles_end] = next_free_tile[i];
                free_tiles_end++;
            }
        } else {
            // Once we have a valid pair, do stuff

            fails = 0;

            for (int i = 0; i < match_size; i++) {
                shuffled_tiles[shuffled_tiles_end] = next_free_tile[i];
                shuffled_tiles_end++;

                // TEMP
                sigmar->board[next_free_tile[i]].type = MARBLE_SALT;
                // END TEMP

                for (int adj = 0; adj < 6; adj++) {
                    adjacent_tile = next_free_tile[i] + sigmar->adjacency_offsets[adj];

                    if (adjacent_tile >= 0 &&
                        adjacent_tile < board_length &&
                        pattern[adjacent_tile] == 1 &&
                        sigmar->board[adjacent_tile].type == MARBLE_EMPTY)
                    { sigmar->board[adjacent_tile].type = MARBLE_PLACEHOLDER; free_tiles[free_tiles_end] = adjacent_tile; free_tiles_end++; }
                }
            }
        }

        if (fails >= 100) {
            
            
            return -1;
        }
    }

    // Holy fuck I think it actually works now
    // I just need to verify
    // The current version of the bucket picker is the one that doesn't really work for metals
    /*for (int i = 0; i < shuffled_tiles_end; i+=2) {
        int picked = 0;
        while (picked == 0) {
            int b = rand() % 27;
            if (bucket[b][0] != INVALID) {
                sigmar->marble_counts[bucket[b][0]] += 1; // Increment marble_counts, but I'll implement this better later.
                sigmar->marble_counts[bucket[b][1]] += 1; // Increment marble_counts, but I'll implement this better later.
                sigmar->board[shuffled_tiles[i]] = bucket[b][0];
                sigmar->board[shuffled_tiles[i + 1]] = bucket[b][1];
                bucket[b][0] = INVALID;
                picked = 1;
            }
        }
    }*/

    

    for (int i = 0; i < shuffled_tiles_end; i+=2) {
        int picked = 0;
        while (picked == 0) {
            int b = rand() % 27;

            if (b < 22) {
                if (bucket[b][0] != MARBLE_INVALID) {
                    sigmar->marble_counts[bucket[b][0]] += 1; // Increment marble_counts, but I'll implement this better later.
                    sigmar->marble_counts[bucket[b][1]] += 1; // Increment marble_counts, but I'll implement this better later.
                    sigmar->board[shuffled_tiles[i]].type = bucket[b][0];
                    sigmar->board[shuffled_tiles[i + 1]].type = bucket[b][1];
                    bucket[b][0] = MARBLE_INVALID;
                    picked = 1;
                }
            } else {
                for (int x = 22; x < 27; x++) {
                    if (bucket[x][0] != MARBLE_INVALID) {
                        sigmar->marble_counts[bucket[x][0]] += 1; // Increment marble_counts, but I'll implement this better later.
                        sigmar->marble_counts[bucket[x][1]] += 1; // Increment marble_counts, but I'll implement this better later.
                        sigmar->board[shuffled_tiles[i]].type = bucket[x][0];
                        sigmar->board[shuffled_tiles[i + 1]].type = bucket[x][1];
                        bucket[x][0] = MARBLE_INVALID;
                        picked = 1;
                        break;
                    }
                }
            }
        }
    }

    return 0;
}

SelectResult sigmar_Select(Sigmar* sigmar, const unsigned int index) {
    if (sigmar == NULL) { return SELECT_ERROR; }

    if (index >= sigmar->board_size * sigmar->board_size || index < 0) { return SELECT_ERROR; }

    if (sigmar->board[index].type == MARBLE_INVALID) { return SELECT_ERROR; }

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
                    sigmar->marble_counts[s.marble.type] -= 1;
                    sigmar->board[s.index].type = MARBLE_EMPTY;
                }

                sigmar->marble_counts[sigmar->board[index].type] -= 1;
                sigmar->board[index].type = MARBLE_EMPTY;

                sigmar->num_selected = 0;

                sigmar_UpdateMarbles(sigmar);

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

int sigmar_IsFree(Sigmar* sigmar, unsigned int index, MarbleState *result) {
    if (sigmar == NULL) { return 1; }

    int rc;

    *result = MARBLE_LOCKED;

    switch (sigmar->board[index].type) {
        case MARBLE_TIN:
        case MARBLE_IRON:
        case MARBLE_COPPER:
        case MARBLE_SILVER:
        case MARBLE_GOLD:
            // If the marble is a metal, it can only be free if all metals of lesser tiers are depleted.
            if (sigmar->marble_counts[sigmar->board[index].type - 1] > 0) { *result = MARBLE_LOCKED; return 0; }
            // Otherwise, continues into default block.
        default:
            // Checks if there are at least three contiguous free indices adjacent to given index.
            rc = sigmar_IsBlocked(sigmar, index, result);
            if (rc == 1) { return 1; }
            break;
    }

    return 0;
}

int sigmar_IsBlocked(Sigmar *sigmar, unsigned int index, MarbleState *result) {
    if (sigmar == NULL) { return 1; }

    int contiguous = 0; // Contiguous free adjacent index counter.

    *result = MARBLE_LOCKED;

    for (int i = 0; i < 9; i++) {
        int adj = index + sigmar->adjacency_offsets[i % 6];

        if (adj >= 0 &&
            adj < sigmar->board_size * sigmar->board_size &&
            sigmar->board[adj].type != MARBLE_EMPTY &&
            sigmar->board[adj].type != MARBLE_PLACEHOLDER &&
            sigmar->board[adj].type != MARBLE_INVALID) { contiguous = 0; }
        else { contiguous++; }

        if (contiguous == 3) { *result = MARBLE_FREE; return 0; }
    }

    return 0;
}

MatchResult sigmar_Matches(Sigmar *sigmar, SelectedMarble *prev_selected, int num_selected, Marble selection) {
    if (selection.type == MARBLE_EMPTY) { return MATCH_ERROR; }

    // Only gold can be matched with nothing else, pass MATCH_CONTINUE for every other marble type.
    if (num_selected == 0 && selection.type != MARBLE_GOLD) { return MATCH_CONTINUE; }

    // Some things to note:
    //  Metals can *technically* always be matched as normal for their ruleset,
    //  however, they only become free when all metals of the previous tier are depleted.
    switch (selection.type) {
        case MARBLE_SALT:
            if (prev_selected[0].marble.type == MARBLE_FIRE ||
                prev_selected[0].marble.type == MARBLE_EARTH ||
                prev_selected[0].marble.type == MARBLE_WATER ||
                prev_selected[0].marble.type == MARBLE_AIR ||
                prev_selected[0].marble.type == selection.type) { return MATCH_SUCCESS; };
            break;
        case MARBLE_FIRE:
        case MARBLE_EARTH:
        case MARBLE_WATER:
        case MARBLE_AIR:
            if (prev_selected[0].marble.type == MARBLE_SALT ||
                prev_selected[0].marble.type == selection.type) { return MATCH_SUCCESS; };
            break;
        case MARBLE_VITAE:
            if (prev_selected[0].marble.type == MARBLE_MORS) { return MATCH_SUCCESS; };
            break;
        case MARBLE_MORS:
            if (prev_selected[0].marble.type == MARBLE_VITAE) { return MATCH_SUCCESS; };
            break;
        case MARBLE_QUICKSILVER:
            if (prev_selected[0].marble.type == MARBLE_LEAD ||
                prev_selected[0].marble.type == MARBLE_TIN ||
                prev_selected[0].marble.type == MARBLE_IRON ||
                prev_selected[0].marble.type == MARBLE_COPPER ||
                prev_selected[0].marble.type == MARBLE_SILVER) { return MATCH_SUCCESS; }
            break;
        case MARBLE_LEAD:
        case MARBLE_TIN:
        case MARBLE_IRON:
        case MARBLE_COPPER:
        case MARBLE_SILVER:
            if (prev_selected[0].marble.type == MARBLE_QUICKSILVER) { return MATCH_SUCCESS; }
            break;
        case MARBLE_GOLD:
            if (num_selected == 0) { return MATCH_SUCCESS; }
            break;
        default:
            break;
    }

    return MATCH_FAIL;
}

int sigmar_UpdateMarbles(Sigmar *sigmar) {
    if (sigmar == NULL) { return 1; }

    for (int i = 0; i < sigmar->board_size * sigmar->board_size; i++) {
        sigmar_IsFree(sigmar, i, &sigmar->board[i].state);
    }

    return 0;
}