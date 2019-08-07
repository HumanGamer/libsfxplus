#include "sfxplus.h"

#include <sndfile.h>
#include <al.h>
#include <alc.h>
#include <vector>
#include <cstring>

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

SFX_AUDIO SFXPLUSCALL sfx_load_audio(const char* path)
{
    ALuint buffer;
    alGenBuffers((ALuint)1, &buffer);
    if (!sfx_checkerror_internal())
    {
        sfx_last_error = SFX_FAIL_CREATE_BUFFER;
        return 0;
    }

    SF_INFO sfinfo;
    memset(&sfinfo, 0, sizeof(sfinfo));

    SNDFILE* snd = sf_open(path, SFM_READ, &sfinfo);
    if (snd == nullptr)
    {
        sfx_last_error = SFX_FAIL_READ_FILE;
        return 0;
    }

    std::vector<unsigned short> data;

    short read_buf[4096];

    size_t read_size = 0;
    while ((read_size = sf_read_short(snd, read_buf, 4096)) != 0)
    {
        data.insert(data.end(), read_buf, read_buf + read_size);
    }

    alBufferData(buffer, sfinfo.channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16,
        &data.front(), data.size() * sizeof(unsigned short), sfinfo.samplerate);
    if (!sfx_checkerror_internal())
    {
        sfx_last_error = SFX_FAIL_CREATE_BUFFER;
        return 0;
    }

    return buffer;
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

void SFXPLUSCALL sfx_shutdown()
{
    sfx_last_error = SFX_NO_ERROR;

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
    case SFX_INVALID_CONTEXT:
        return "SFX_INVALID_CONTEXT";
    case SFX_FAIL_CREATE_SOURCE:
        return "SFX_FAIL_CREATE_SOURCE";
    case SFX_FAIL_CREATE_BUFFER:
        return "SFX_FAIL_CREATE_BUFFER";
    case SFX_FAIL_READ_FILE:
        return "SFX_FAIL_READ_FILE";
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

void sfx_setlasterror_internal(int error)
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
