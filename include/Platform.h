#ifndef OPENSIGMAR_PLATFORM_H
#define OPENSIGMAR_PLATFORM_H

typedef struct Sigmar Sigmar; // Forward declaration.

typedef struct SigmarData {
    unsigned long wins;

    double music_volume;
    double sound_volume;
} SigmarData;

int sigmar_DataInit(SigmarData **data);

int sigmar_LoadData(SigmarData *data);

int sigmar_SaveData(SigmarData *data);

/*
 * Pregenerates 'count' boards and stores them according to the platform implementation.
 */
int sigmar_PregenerateBoards(Sigmar *sigmar, unsigned int count);

/*
 * Loads a random board previously generated using sigmar_PregenerateBoards().
 */
int sigmar_LoadBoard(Sigmar *sigmar);

#endif //OPENSIGMAR_PLATFORM_H