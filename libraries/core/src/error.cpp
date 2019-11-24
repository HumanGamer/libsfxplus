#include "core.h"

#include <stdio.h>
#include <al.h>
#include <alc.h>

int SFXPLUSCALL sfx_error()
{
    return sfx_last_error;
}

const char* SFXPLUSCALL sfx_error_string(int error)
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
    case SFX_INVALID_CONTEXT:
        return "SFX_INVALID_CONTEXT";
    case SFX_FAIL_CREATE_SOURCE:
        return "SFX_FAIL_CREATE_SOURCE";
    case SFX_FAIL_CREATE_BUFFER:
        return "SFX_FAIL_CREATE_BUFFER";
    case SFX_FAIL_DELETE_BUFFER:
        return "SFX_FAIL_DELETE_BUFFER";
    case SFX_FAIL_PLAY_SOURCE:
        return "SFX_FAIL_PLAY_SOURCE";
    case SFX_FAIL_PAUSE_SOURCE:
        return "SFX_FAIL_PAUSE_SOURCE";
    case SFX_FAIL_STOP_SOURCE:
        return "SFX_FAIL_STOP_SOURCE";
    case SFX_FAIL_FILL_BUFFER:
        return "SFX_FAIL_FILL_BUFFER";
    case SFX_FAIL_UNQUEUE_BUFFER:
        return "SFX_FAIL_UNQUEUE_BUFFER";
    case SFX_FAIL_QUEUE_BUFFER:
        return "SFX_FAIL_QUEUE_BUFFER";
    case SFX_FAIL_READ_FILE:
        return "SFX_FAIL_READ_FILE";
    case SFX_FAIL_SET_PROPERTY:
        return "SFX_FAIL_SET_PROPERTY";
    case SFX_FAIL_GET_PROPERTY:
        return "SFX_FAIL_GET_PROPERTY";
    case SFX_FAIL_GET_STATE:
        return "SFX_FAIL_GET_STATE";
    case SFX_INVALID_STATE:
        return "SFX_INVALID_STATE";
    case SFX_INVALID_DEVICE:
        return "SFX_INVALID_DEVICE";
    case SFX_INVALID_ENUM:
        return "SFX_INVALID_ENUM";
    case SFX_INVALID_VALUE:
        return "SFX_INVALID_VALUE";
    case SFX_OUT_OF_MEMORY:
        return "SFX_OUT_OF_MEMORY";
    case SFX_INVALID_OPERATION:
        return "SFX_INVALID_OPERATION";
    case SFX_UNKNOWN_ERROR:
    default:
        return "SFX_UNKNOWN_ERROR";
    }
}

bool sfx_checkerror_internal()
{
    ALenum error = alGetError();
    if (error != AL_NO_ERROR)
    {
        sfx_setlasterror_internal(error);
        return false;
    }

    return true;
}

void sfx_setlasterror_internal(int error, bool alc)
{
    if (alc)
    {
        switch (error)
        {
        case ALC_NO_ERROR:
            sfx_last_error = SFX_NO_ERROR;
            break;
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
        default:
            sfx_last_error = SFX_INTERNAL_ERROR;
            break;
        }
    }
    else {
        switch (error)
        {
        case AL_NO_ERROR:
            sfx_last_error = SFX_NO_ERROR;
            break;
        case AL_INVALID_ENUM:
            sfx_last_error = SFX_INVALID_ENUM;
            break;
        case AL_INVALID_VALUE:
            sfx_last_error = SFX_INVALID_VALUE;
            break;
        case AL_OUT_OF_MEMORY:
            sfx_last_error = SFX_OUT_OF_MEMORY;
            break;
        case AL_INVALID_OPERATION:
            sfx_last_error = SFX_INVALID_OPERATION;
            break;
        default:
            sfx_last_error = SFX_INTERNAL_ERROR;
            break;
        }
    }
}