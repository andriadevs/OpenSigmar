#ifndef OPENSIGMAR_PLATFORM_H
#define OPENSIGMAR_PLATFORM_H

typedef struct Sigmar Sigmar; // Forward declaration.

/*
 * Struct containing any data that should be retained between program executions.
 * 
 * How the data is saved, loaded, and initialized is platform-dependent.
 */
typedef struct SigmarData SigmarData;

/*
 * Initializes SigmarData to platform defaults.
 */
int sigmar_DataInit(SigmarData **data);

/*
 * Loads previously saved data according to platform implementation.
 */
int sigmar_LoadData(SigmarData *data);

/*
 * Saves SigmarData to file according to platform implementation.
 */
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