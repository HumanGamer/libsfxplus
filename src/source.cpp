#include "core.h"

#include <al.h>

SFX_SOURCE SFXPLUSCALL sfx_create_source(float pitch, float gain, bool looping)
{
    ALuint source;
    alGenSources((ALuint)1, &source);
    if (!sfx_checkerror_internal())
    {
        sfx_last_error = SFX_FAIL_CREATE_SOURCE;
        return 0;
    }

    alSourcef(source, AL_PITCH, pitch);
    if (!sfx_checkerror_internal())
    {
        sfx_last_error = SFX_FAIL_CREATE_SOURCE;
        return 0;
    }

    alSourcef(source, AL_GAIN, gain);
    if (!sfx_checkerror_internal())
    {
        sfx_last_error = SFX_FAIL_CREATE_SOURCE;
        return 0;
    }

    alSourcei(source, AL_LOOPING, looping ? AL_TRUE : AL_FALSE);
    if (!sfx_checkerror_internal())
    {
        sfx_last_error = SFX_FAIL_CREATE_SOURCE;
        return 0;
    }

    return source;
}

void SFXPLUSCALL sfx_source_play_sound(SFX_SOURCE source, SFX_AUDIO audio)
{
    alSourcei(source, AL_BUFFER, audio);
    if (!sfx_checkerror_internal())
    {
        sfx_last_error = SFX_FAIL_CREATE_BUFFER;
        return;
    }

    alSourcePlay(source);
    if (!sfx_checkerror_internal())
    {
        sfx_last_error = SFX_FAIL_CREATE_BUFFER;
        return;
    }
}

int SFXPLUSCALL sfx_source_getstate(SFX_SOURCE source)
{
    int source_state;
    alGetSourcei(source, AL_SOURCE_STATE, &source_state);
    if (!sfx_checkerror_internal())
    {
        sfx_last_error = SFX_FAIL_CREATE_BUFFER;
        return -1;
    }

    switch (source_state)
    {
    case AL_INITIAL:
        return SFX_SOURCE_STATE_INITIAL;
    case AL_PLAYING:
        return SFX_SOURCE_STATE_PLAYING;
    case AL_PAUSED:
        return SFX_SOURCE_STATE_PAUSED;
    case AL_STOPPED:
        return SFX_SOURCE_STATE_STOPPED;
    default:
        sfx_last_error = SFX_INVALID_STATE;
        return -1;
    }
}

void SFXPLUSCALL sfx_source_wait(SFX_SOURCE source)
{
    while (sfx_source_getstate(source) == SFX_SOURCE_STATE_PLAYING);
}
