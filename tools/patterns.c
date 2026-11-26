/*
 * Since I couldn't find a good way to directly extract patterns from Opus Magnum, I instead resorted to taking screenshots of the game and reconstructing them.
 * 
 * This file describes the method and includes everything I used to reconstruct the patterns that are currently in OpenSigmar.
 * 
 * It's straight-forward, but it's also tedious, which is why I've stopped at ten. I mostly just created this file for when I eventually reconstruct the rest of them.
 */

#include <stdio.h>

#define PATTERN_LENGTH (91) // Length of reconstructed patterns, there are 91 valid tiles on a Sigmar's Garden board.

// These are the equivalent tile indices for each index in a reconstructed pattern.
const char pattern_tiles[] = {0,1,2,3,4,5,11,12,13,14,15,16,17,22,23,24,25,26,27,28,29,33,34,35,36,37,38,39,40,41,44,45,46,47,48,49,50,51,52,53,55,56,57,58,59,60,61,62,63,64,65,67,68,69,70,71,72,73,74,75,76,79,80,81,82,83,84,85,86,87,91,92,93,94,95,96,97,98,103,104,105,106,107,108,109,115,116,117,118,119,120};

// Patterns should be written as an array of 91 1's and 0's.
// With a screenshot of a board from Sigmar's Garden, starting from the top-left, write 1 for each tile with a marble on it and 0 for each empty tile.
const char pattern[PATTERN_LENGTH] = {};

int main() {
    // This will print a string of 55 numbers, this is the pattern used by OpenSigmar.
    for (int i = 0; i < PATTERN_LENGTH; i++) {
        if (pattern[i]) { printf("%i,", pattern_tiles[i]); }
    }
    printf("\n");

    return 0;
}