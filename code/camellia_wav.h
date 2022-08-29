/* date = August 28th 2022 10:43 pm */

#ifndef CAMELLIA_WAV_H
#define CAMELLIA_WAV_H

#include "camellia_platform.h"

typedef struct wave_header {
    i16 Format;
    i16 Channels;
    i32 SamplesPerSec;
    i32 AvgBytesPerSec;
    i16 BlockAlign;
    i16 BitsPerSample;
    i16 Size;
} wave_header;

typedef struct sound_data {
    u32 ChannelCount;
    u32 SampleCount;
    u32 SampleRate;
    i16* Samples;
} sound_data;

void WaveFileLoad(const char* Path, sound_data* Sound);
void WaveFileFree(sound_data* Sound);

#endif //CAMELLIA_WAV_H
