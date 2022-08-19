/* date = August 19th 2022 10:10 am */

#ifndef CAMELLIA_DSOUND_H
#define CAMELLIA_DSOUND_H

#include "camellia_platform.h"

#include <dsound.h>

typedef struct direct_sound_state {
    IDirectSound8* Device;
    DSCAPS DeviceCaps;
    IDirectSoundBuffer* PrimaryBuffer;
} direct_sound_state;

void DirectSoundInit(HWND Window);
void DirectSoundExit();

#endif //CAMELLIA_DSOUND_H
