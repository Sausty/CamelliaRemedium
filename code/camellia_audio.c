#include "camellia_audio.h"

#ifdef AUDIO_DSOUND
#include "camellia_dsound.h"
#endif

audio_engine Audio;

void AudioEngineLoad()
{
#ifdef AUDIO_DSOUND
    Audio.Init = DirectSoundInit;
    Audio.Exit = DirectSoundExit;
    Audio.InitSource = DirectSoundInitSource;
    Audio.FreeSource = DirectSoundFreeSource;
    Audio.PlaySource = DirectSoundPlaySource;
    Audio.StopSource = DirectSoundStopSource;
    Audio.PauseSource = DirectSoundPauseSource;
    Audio.SetSourceVolume = DirectSoundSetSourceVolume;
#endif
}