/* date = August 29th 2022 2:56 am */

#ifndef CAMELLIA_AUDIO_H
#define CAMELLIA_AUDIO_H

#include "camellia_platform.h"
#include "camellia_wav.h"

typedef struct audio_source {
    void* Handle;
    i32 PauseCursor;
    bool32 Looping;
} audio_source;

typedef struct audio_engine {
    void (*Init)(void* Window);
    void (*Exit)();
    
    void (*InitSource)(sound_data* Sound, audio_source* Source);
    void (*FreeSource)(audio_source* Source);
    void (*PlaySource)(audio_source* Source);
    void (*StopSource)(audio_source* Source);
    void (*PauseSource)(audio_source* Source);
    void (*SetSourceVolume)(audio_source* Source, i32 Volume);
} audio_engine;

extern audio_engine Audio;

void AudioEngineLoad();

#endif //CAMELLIA_AUDIO_H
