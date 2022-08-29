#include "camellia_wav.h"

#include <stdlib.h>

void WaveFileLoad(const char* Path, sound_data* Sound)
{   
    buffer SoundBuffer;
    PlatformState.ReadFile(Path, &SoundBuffer);
    
    wave_header Header;
    
    i32 Riff = 0;
    u16 DataChunkOffset = 0;
    bool32 DataChunkFound = false;
    i32 DataChunkSize = 0;
    i32 DataChunkSearcher = 0;
    
    // Copy riff data
    memcpy(&Riff, SoundBuffer.Data, sizeof(i32));
    Assert(Riff == 0x46464952); // "RIFF" backwards
    
    // 20 bytes header
    memcpy(&Header, (u8*)SoundBuffer.Data + 20, sizeof(wave_header));
    
    // Only support i16 PCM
    Assert(Header.BlockAlign == ((Header.Channels * Header.BitsPerSample / 8)));
    Assert(Header.Format == 1);
    Assert(Header.BitsPerSample == 16);
    
    // Search for data chunk
    while (DataChunkFound == false)
    {
        memcpy(&DataChunkSearcher, (u8*)SoundBuffer.Data + DataChunkOffset, sizeof(i32));
        
        if (DataChunkSearcher == 0x61746164)
        {
            DataChunkFound = true;
            break;
        }
        else
        {
            DataChunkOffset += 4;
        }
        
        Assert(DataChunkOffset < 256);
    }
    
    memcpy(&DataChunkSize, (u8*)SoundBuffer.Data + DataChunkOffset + 4, sizeof(i32));
    
    Sound->ChannelCount = Header.Channels;
    Sound->SampleCount = DataChunkSize / (Header.Channels * sizeof(i16));
    Sound->SampleRate = Header.SamplesPerSec;
    Sound->Samples = (i16*)PlatformState.HeapAlloc(DataChunkSize);
    memcpy(Sound->Samples, (u8*)SoundBuffer.Data + DataChunkOffset + 8, DataChunkSize);
    
    PlatformState.HeapFree(SoundBuffer.Data);
}

void WaveFileFree(sound_data* Sound)
{
    PlatformState.HeapFree(Sound->Samples);
}