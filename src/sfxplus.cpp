#include "sfxplus.h"

#include <sndfile.h>
#include <al.h>
#include <alc.h>

bool sfx_initialized = false;

ALCdevice* alc_device = nullptr;
ALCcontext* alc_context = nullptr;

int sfx_last_error = SFX_NO_ERROR;

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

    ALCenum error;
    error = alGetError();
    if (error != AL_NO_ERROR)
    {
        sfx_last_error = SFX_INTERNAL_ERROR;
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
        return false;
    }
    
    sfx_initialized = true;
    return true;
}

void SFXPLUSCALL sfx_shutdown()
{
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
    /*const char* err;
    switch (error)
    {
        case SFX_NO_ERROR:
            err = "SFX_NO_ERROR";
            break;
        case SFX_INTERNAL_ERROR:
            err = "SFX_INTERNAL_ERROR";
            break;
        case SFX_FAIL_OPEN_DEVICE:
            err = "SFX_FAIL_OPEN_DEVICE";
            break;
        case SFX_FAIL_CREATE_CONTEXT:
            err = "SFX_FAIL_CREATE_CONTEXT";
            break;
        case SFX_UNKNOWN_ERROR:
        default:
            err = "SFX_UNKNOWN_ERROR";
            break;
    }*/

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
    case SFX_UNKNOWN_ERROR:
    default:
        return "SFX_UNKNOWN_ERROR";
    }
}
