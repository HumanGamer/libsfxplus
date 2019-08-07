#include "core.h"

#include <al.h>
#include <alc.h>

bool sfx_initialized = false;

ALCdevice* alc_device = nullptr;
ALCcontext* alc_context = nullptr;

int sfx_last_error = SFX_NO_ERROR;

bool sfx_checkerror_internal();
void sfx_setlasterror_internal(int error);

bool SFXPLUSCALL sfx_startup()
{
    sfx_last_error = SFX_NO_ERROR;

    if (sfx_initialized)
        return true;

    alc_device = alcOpenDevice(nullptr);
    if (alc_device == nullptr)
    {
        sfx_last_error = SFX_FAIL_OPEN_DEVICE;
        return false;
    }

    alGetError();

    ALCenum error;
    error = alcGetError(alc_device);
    if (error != ALC_NO_ERROR)
    {
        sfx_setlasterror_internal(error);
        return false;
    }

    alc_context = alcCreateContext(alc_device, nullptr);
    if (!alcMakeContextCurrent(alc_context))
    {
        sfx_last_error = SFX_FAIL_CREATE_CONTEXT;
        return false;
    }

    error = alcGetError(alc_device);
    if (error != ALC_NO_ERROR)
    {
        sfx_setlasterror_internal(error);
        return false;
    }

    sfx_initialized = true;
    return true;
}

void SFXPLUSCALL sfx_shutdown()
{
    sfx_last_error = SFX_NO_ERROR;

    alc_device = alcGetContextsDevice(alc_context);
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(alc_context);
    alcCloseDevice(alc_device);
}
