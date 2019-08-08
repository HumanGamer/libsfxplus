#include "core.h"

#include <vector>
#include <cstring>
#include <sndfile.h>
#include <al.h>

void SFXPLUSCALL sfx_source_open_stream(SFX_SOURCE source, const char* path)
{
    sfx_last_error = SFX_NO_ERROR;

    int bufCount = 3;

    ALuint* buffers = (ALuint*)malloc(bufCount * sizeof(ALuint));
    alGenBuffers((ALuint)bufCount, buffers);
    if (!sfx_checkerror_internal())
    {
        sfx_last_error = SFX_FAIL_CREATE_BUFFER;
        return;
    }

    SF_INFO sfinfo;
    memset(&sfinfo, 0, sizeof(sfinfo));

    SNDFILE* snd = sf_open(path, SFM_READ, &sfinfo);
    if (snd == nullptr)
    {
        sfx_last_error = SFX_FAIL_READ_FILE;
        return;
    }

    short read_buf[4096];

    size_t read_size = 0;
    if ((read_size = sf_read_short(snd, read_buf, 4096)) == 0)
        return;

    for (int i = 0; i < bufCount; i++)
    {
        if (buffers == nullptr)
            break;

        alBufferData(buffers[i],
            sfinfo.channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16,
            read_buf, read_size * sizeof(unsigned short), sfinfo.samplerate);

        if (!sfx_checkerror_internal())
        {
            sfx_last_error = SFX_FAIL_FILL_BUFFER;
            return;
        }
    }

    alSourceQueueBuffers(source, bufCount, buffers);
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

    unsigned int buf;

    while (true)
    {
        int numProcessed;
        alGetSourcei(source, AL_BUFFERS_PROCESSED, &numProcessed);
        if (!sfx_checkerror_internal())
        {
            sfx_last_error = SFX_FAIL_GET_PROPERTY;
            return;
        }

        while (numProcessed > 0)
        {
            alSourceUnqueueBuffers(source, 1, &buf);
            if (!sfx_checkerror_internal())
            {
                sfx_last_error = SFX_FAIL_UNQUEUE_BUFFER;
                return;
            }
            numProcessed--;

            if ((read_size = sf_read_short(snd, read_buf, 4096)) != 0)
            {
                alBufferData(buf, sfinfo.channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16,
                    read_buf, read_size * sizeof(unsigned short), sfinfo.samplerate);
                if (!sfx_checkerror_internal())
                {
                    sfx_last_error = SFX_FAIL_FILL_BUFFER;
                    return;
                }

                alSourceQueueBuffers(source, 1, &buf);
                if (!sfx_checkerror_internal())
                {
                    sfx_last_error = SFX_FAIL_QUEUE_BUFFER;
                    return;
                }
            }
        }

        int state;
        alGetSourcei(source, AL_SOURCE_STATE, &state);
        if (!sfx_checkerror_internal())
        {
            sfx_last_error = SFX_FAIL_GET_STATE;
            return;
        }
            
        if (state != AL_PLAYING && state != AL_PAUSED)
        {
            int queued;
            alGetSourcei(source, AL_BUFFERS_QUEUED, &queued);
            if (!sfx_checkerror_internal())
            {
                sfx_last_error = SFX_FAIL_GET_PROPERTY;
                return;
            }
            if (queued == 0)
                return;

            alSourcePlay(source);
            if (!sfx_checkerror_internal())
            {
                sfx_last_error = SFX_FAIL_PLAY_SOURCE;
                return;
            }
        }
    }

    alDeleteBuffers(bufCount, buffers);
}