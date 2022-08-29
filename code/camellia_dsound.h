/* date = August 19th 2022 10:10 am */

#ifndef CAMELLIA_DSOUND_H
#define CAMELLIA_DSOUND_H

#include "camellia_platform.h"
#include "camellia_audio.h"

#include <dsound.h>

typedef struct direct_sound_state {
    IDirectSound8* Device;
    DSCAPS DeviceCaps;
    IDirectSoundBuffer* PrimaryBuffer;
} direct_sound_state;

void DirectSoundInit(void* Window);
void DirectSoundExit();

void DirectSoundInitSource(sound_data* Sound, audio_source* Source);
void DirectSoundFreeSource(audio_source* Source);
void DirectSoundPlaySource(audio_source* Source);
void DirectSoundStopSource(audio_source* Source);
void DirectSoundPauseSource(audio_source* Source);
void DirectSoundSetSourceVolume(audio_source* Source, i32 Volume);

#endif //CAMELLIA_DSOUND_H
