#include "camellia_dsound.h"

#include "camellia_math.h"

#define CO_INITIALIZE_EX(name) HRESULT name(LPVOID pvReserved, DWORD dwCoInit)
typedef CO_INITIALIZE_EX(co_initialize_ex);

#define CO_UNINITIALIZE(name) void name()
typedef CO_UNINITIALIZE(co_uninitialize);

#define DIRECT_SOUND_CREATE_8(name) HRESULT name(LPCGUID lpcGuidDevice, LPDIRECTSOUND8 *ppDS8, LPUNKNOWN pUnkOuter)
typedef DIRECT_SOUND_CREATE_8(direct_sound_create_8);

CO_INITIALIZE_EX(CoInitializeExStub) { return S_FALSE; }
CO_UNINITIALIZE(CoUninitializeStub) { return; }
DIRECT_SOUND_CREATE_8(DirectSoundCreate8Stub) { return !DS_OK; }

global co_initialize_ex *coInitializeEx_ = CoInitializeExStub;
global co_uninitialize *coUninitialize_ = CoUninitializeStub;
global direct_sound_create_8 *directSoundCreate8_ = DirectSoundCreate8Stub;

#define CoInitializeEx coInitializeEx_
#define CoUninitialize coUninitialize_
#define DirectSoundCreate8 directSoundCreate8_

global direct_sound_state DSound;

void DirectSoundLoadOle()
{
    HMODULE Library = LoadLibraryA("ole32.dll");
    if (Library)
    {
        CoInitializeEx = (co_initialize_ex*)GetProcAddress(Library, "CoInitializeEx");
        CoUninitialize = (co_uninitialize*)GetProcAddress(Library, "CoUninitialize");
    }
}

void DirectSoundLoad()
{
    HMODULE Library = LoadLibraryA("dsound.dll");
    if (Library)
    {
        DirectSoundCreate8 = (direct_sound_create_8*)GetProcAddress(Library, "DirectSoundCreate8");
    }
}

void DirectSoundInit(void* Window)
{
    DirectSoundLoadOle();
    DirectSoundLoad();
    
    Assert(SUCCEEDED(CoInitializeEx(NULL, COINIT_MULTITHREADED)));
    
    HRESULT Result = DirectSoundCreate8(NULL, &DSound.Device, NULL);
    Assert(Result == DS_OK);
    Result = IDirectSound8_SetCooperativeLevel(DSound.Device, (HWND)Window, DSSCL_PRIORITY);
    Assert(Result == DS_OK);
    
    DSound.DeviceCaps.dwSize = sizeof(DSCAPS);
    IDirectSound8_GetCaps(DSound.Device, &DSound.DeviceCaps);
    
    WAVEFORMATEX WaveFormat;
    ZeroMemory(&WaveFormat, sizeof(WaveFormat));
    WaveFormat.wFormatTag = WAVE_FORMAT_PCM;
    WaveFormat.nChannels = 2;
    WaveFormat.nSamplesPerSec = 48000;
    WaveFormat.nBlockAlign = 4;
    WaveFormat.nAvgBytesPerSec = WaveFormat.nSamplesPerSec * WaveFormat.nBlockAlign;
    WaveFormat.wBitsPerSample = 16;
    
    DSBUFFERDESC PrimaryBufferDesc;
    ZeroMemory(&PrimaryBufferDesc, sizeof(PrimaryBufferDesc));
    PrimaryBufferDesc.dwSize = sizeof(PrimaryBufferDesc);
    PrimaryBufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRL3D;
    PrimaryBufferDesc.dwBufferBytes = 0;
    PrimaryBufferDesc.lpwfxFormat = NULL;
    PrimaryBufferDesc.guid3DAlgorithm = GUID_NULL;
    
    Result = IDirectSound8_CreateSoundBuffer(DSound.Device, &PrimaryBufferDesc, &DSound.PrimaryBuffer, NULL);
    Assert(Result == DS_OK);
    IDirectSoundBuffer8_SetFormat(DSound.PrimaryBuffer, &WaveFormat);
}

void DirectSoundExit()
{
    IDirectSoundBuffer8_Release(DSound.PrimaryBuffer);
    IDirectSound8_Release(DSound.Device);
    
    CoUninitialize();
}

void DirectSoundInitSource(sound_data* Sound, audio_source* Source)
{
    IDirectSoundBuffer8* Buffer;
    
    WAVEFORMATEX WaveFormat;
    ZeroMemory(&WaveFormat, sizeof(WAVEFORMATEX));
    WaveFormat.wFormatTag = WAVE_FORMAT_PCM;
    WaveFormat.nChannels = Sound->ChannelCount;
    WaveFormat.nSamplesPerSec = Sound->SampleRate;
    WaveFormat.wBitsPerSample = 16;
    WaveFormat.nBlockAlign = (WaveFormat.nChannels * WaveFormat.wBitsPerSample) / 8;
    WaveFormat.nAvgBytesPerSec = (WaveFormat.nSamplesPerSec * WaveFormat.nBlockAlign);
    WaveFormat.cbSize = 0;
    
    DSBUFFERDESC BufferDesc;
    ZeroMemory(&BufferDesc, sizeof(DSBUFFERDESC));
    BufferDesc.dwSize = sizeof(DSBUFFERDESC);
    BufferDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_GLOBALFOCUS;
    BufferDesc.dwBufferBytes = Sound->SampleCount * Sound->ChannelCount * sizeof(i16);
    BufferDesc.lpwfxFormat = &WaveFormat;
    BufferDesc.guid3DAlgorithm = GUID_NULL;
    
    Assert(BufferDesc.dwBufferBytes < DSBSIZE_MAX);
    
    IDirectSoundBuffer* Temp;
    Assert(SUCCEEDED(IDirectSound8_CreateSoundBuffer(DSound.Device, &BufferDesc, &Temp, NULL)));
    Assert(SUCCEEDED(IDirectSoundBuffer_QueryInterface(Temp, &IID_IDirectSoundBuffer8, (void**)&Buffer)));
    IDirectSoundBuffer_Release(Temp);
    
    void* Write;
    DWORD Length;
    Assert(SUCCEEDED(IDirectSoundBuffer8_Lock(Buffer, 0, 0, &Write, &Length, NULL, NULL, DSBLOCK_ENTIREBUFFER)));
    memcpy(Write, Sound->Samples, Length);
    IDirectSoundBuffer8_Unlock(Buffer, Write, Length, NULL, 0);
    
    Source->Handle = Buffer;
}

void DirectSoundFreeSource(audio_source* Source)
{
    IDirectSoundBuffer8_Release((IDirectSoundBuffer8*)Source->Handle);
}

void DirectSoundPlaySource(audio_source* Source)
{
    IDirectSoundBuffer8* Buffer = (IDirectSoundBuffer8*)Source->Handle;
    IDirectSoundBuffer8_SetCurrentPosition(Buffer, Source->PauseCursor);
    Assert(SUCCEEDED(IDirectSoundBuffer8_Play(Buffer, 0, 0, Source->Looping ? DSBPLAY_LOOPING : 0)));
}

void DirectSoundStopSource(audio_source* Source)
{
    IDirectSoundBuffer8* Buffer = (IDirectSoundBuffer8*)Source->Handle;
    Assert(SUCCEEDED(IDirectSoundBuffer8_Stop(Buffer)));
}

void DirectSoundPauseSource(audio_source* Source)
{
    IDirectSoundBuffer8* Buffer = (IDirectSoundBuffer8*)Source->Handle;
    Assert(SUCCEEDED(IDirectSoundBuffer8_GetCurrentPosition(Buffer, (LPDWORD)&Source->PauseCursor, NULL)));
    DirectSoundStopSource(Source);
}

void DirectSoundSetSourceVolume(audio_source* Source, i32 Volume)
{
    IDirectSoundBuffer8* Buffer = (IDirectSoundBuffer8*)Source->Handle;
    Assert(SUCCEEDED(IDirectSoundBuffer8_SetVolume(Buffer, (LONG)Volume)));
}