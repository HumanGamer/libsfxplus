#include "core.h"

#include <al.h>

SFX_SOURCE SFXPLUSCALL sfx_source_create(float pitch, float gain, bool looping)
{
    sfx_last_error = SFX_NO_ERROR;

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

void SFXPLUSCALL sfx_source_destroy(SFX_SOURCE source)
{
    sfx_last_error = SFX_NO_ERROR;

    alDeleteSources(1, &source);
}

void SFXPLUSCALL sfx_source_pitch(SFX_SOURCE source, float pitch)
{
    sfx_last_error = SFX_NO_ERROR;

    alSourcef(source, AL_PITCH, pitch);
    if (!sfx_checkerror_internal())
    {
        sfx_last_error = SFX_FAIL_SET_PROPERTY;
        return;
    }
}

void SFXPLUSCALL sfx_source_gain(SFX_SOURCE source, float gain)
{
    sfx_last_error = SFX_NO_ERROR;

    alSourcef(source, AL_GAIN, gain);
    if (!sfx_checkerror_internal())
    {
        sfx_last_error = SFX_FAIL_SET_PROPERTY;
        return;
    }
}

void SFXPLUSCALL sfx_source_looping(SFX_SOURCE source, bool looping)
{
    sfx_last_error = SFX_NO_ERROR;

    alSourcei(source, AL_LOOPING, looping ? AL_TRUE : AL_FALSE);
    if (!sfx_checkerror_internal())
    {
        sfx_last_error = SFX_FAIL_SET_PROPERTY;
        return;
    }
}

void SFXPLUSCALL sfx_source_play_sound(SFX_SOURCE source, SFX_AUDIO audio)
{
    sfx_last_error = SFX_NO_ERROR;

    alSourcei(source, AL_BUFFER, audio);
    if (!sfx_checkerror_internal())
    {
        sfx_last_error = SFX_FAIL_QUEUE_BUFFER;
        return;
    }

    alSourcePlay(source);
    if (!sfx_checkerror_internal())
    {
        sfx_last_error = SFX_FAIL_PLAY_SOURCE;
        return;
    }
}

int SFXPLUSCALL sfx_source_getstate(SFX_SOURCE source)
{
    sfx_last_error = SFX_NO_ERROR;

    int source_state;
    alGetSourcei(source, AL_SOURCE_STATE, &source_state);
    if (!sfx_checkerror_internal())
    {
        sfx_last_error = SFX_FAIL_GET_STATE;
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
    sfx_last_error = SFX_NO_ERROR;

    while (sfx_source_getstate(source) == SFX_SOURCE_STATE_PLAYING);
}
