#include "sfxplus.h"

#include <sndfile.h>
#include <al.h>
#include <alc.h>

bool sfx_initialized = false;

ALCdevice* alc_device = nullptr;
ALCcontext* alc_context = nullptr;

bool SFXPLUSCALL sfx_init()
{
    if (sfx_initialized)
        return true;

    alc_device = alcOpenDevice(nullptr);
    if (alc_device == nullptr)
        return false;

    ALCenum error;
    error = alGetError();
    if (error != AL_NO_ERROR)
        return false;

    alc_context = alcCreateContext(alc_device, nullptr);
    if (!alcMakeContextCurrent(alc_context))
        return false;

    error = alGetError();
    if (error != AL_NO_ERROR)
        return false;
    
    sfx_initialized = true;
    return true;
}
