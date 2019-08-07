#include "sfxplus.h"

#include <sndfile.h>
#include <al.h>
#include <alc.h>

bool sfx_initialized = false;

ALCdevice* alc_device = nullptr;
ALCcontext* alc_context = nullptr;

#define INTERNAL_LAST_ERROR_DEFAULT AL_NO_ERROR

int sfx_last_error = SFX_NO_ERROR;
int sfx_internal_last_error = INTERNAL_LAST_ERROR_DEFAULT;

void sfx_setlasterror_internal();

bool SFXPLUSCALL sfx_startup()
{
    sfx_last_error = SFX_NO_ERROR;
    sfx_internal_last_error = INTERNAL_LAST_ERROR_DEFAULT;

    if (sfx_initialized)
        return true;

    alc_device = alcOpenDevice(nullptr);
    if (alc_device == nullptr)
    {
        sfx_last_error = SFX_FAIL_OPEN_DEVICE;
        return false;
    }

    ALCenum error;
    error = alGetError();
    if (error != AL_NO_ERROR)
    {
        sfx_last_error = SFX_INTERNAL_ERROR;
        sfx_internal_last_error = error;

        sfx_setlasterror_internal();
        return false;
    }

    alc_context = alcCreateContext(alc_device, nullptr);
    if (!alcMakeContextCurrent(alc_context))
    {
        sfx_last_error = SFX_FAIL_CREATE_CONTEXT;
        return false;
    }

    error = alGetError();
    if (error != AL_NO_ERROR)
    {
        sfx_last_error = SFX_INTERNAL_ERROR;
        sfx_internal_last_error = error;

        sfx_setlasterror_internal();
        return false;
    }

    sfx_initialized = true;
    return true;
}

void SFXPLUSCALL sfx_shutdown()
{
    sfx_last_error = SFX_NO_ERROR;
    sfx_internal_last_error = INTERNAL_LAST_ERROR_DEFAULT;

    alc_device = alcGetContextsDevice(alc_context);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(alc_context);
    alcCloseDevice(alc_device);
}

int SFXPLUSCALL sfx_getlasterror()
{
    return sfx_last_error;
}

const char* SFXPLUSCALL sfx_errorstring(int error)
{
    if (error == -1)
        error = sfx_last_error;

    switch (error)
    {
    case SFX_NO_ERROR:
        return "SFX_NO_ERROR";
    case SFX_INTERNAL_ERROR:
        return "SFX_INTERNAL_ERROR";
    case SFX_FAIL_OPEN_DEVICE:
        return "SFX_FAIL_OPEN_DEVICE";
    case SFX_FAIL_CREATE_CONTEXT:
        return "SFX_FAIL_CREATE_CONTEXT";
    case SFX_INVALID_DEVICE:
        return "SFX_INVALID_DEVICE";
    case SFX_INVALID_CONTEXT:
        return "SFX_INVALID_CONTEXT";
    case SFX_INVALID_ENUM:
        return "SFX_INVALID_ENUM";
    case SFX_INVALID_VALUE:
        return "SFX_INVALID_VALUE";
    case SFX_OUT_OF_MEMORY:
        return "SFX_OUT_OF_MEMORY";
    case SFX_UNKNOWN_ERROR:
    default:
        return "SFX_UNKNOWN_ERROR";
    }
}

void sfx_setlasterror_internal()
{
    switch (sfx_internal_last_error)
    {
    case ALC_INVALID_DEVICE:
        sfx_last_error = SFX_INVALID_DEVICE;
        break;
    case ALC_INVALID_CONTEXT:
        sfx_last_error = SFX_INVALID_CONTEXT;
        break;
    case ALC_INVALID_ENUM:
        sfx_last_error = SFX_INVALID_ENUM;
        break;
    case ALC_INVALID_VALUE:
        sfx_last_error = SFX_INVALID_VALUE;
        break;
    case ALC_OUT_OF_MEMORY:
        sfx_last_error = SFX_OUT_OF_MEMORY;
        break;
    }
}
