#ifndef OPENSIGMAR_PLATFORM_H
#define OPENSIGMAR_PLATFORM_H

typedef struct SigmarData {
    unsigned long wins;

    double music_volume;
    double sound_volume;
} SigmarData;

int sigmar_DataInit(SigmarData **data);

int sigmar_LoadData(SigmarData *data);

int sigmar_SaveData(SigmarData *data);

#endif //OPENSIGMAR_PLATFORM_H