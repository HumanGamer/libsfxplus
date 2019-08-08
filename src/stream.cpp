#include "core.h"

#include <thread>
#include <mutex>
#include <vector>
#include <cstring>
#include <sndfile.h>
#include <al.h>

std::mutex sfx_stream_io_mutex, sfx_stream_play_mutex;
std::vector<unsigned short> sfx_stream_data;
size_t current;

void sfx_add_data_internal(short* first, short* last)
{
    std::lock(sfx_stream_io_mutex, sfx_stream_play_mutex);
    sfx_stream_data.insert(sfx_stream_data.end(), first, last);

    sfx_stream_io_mutex.unlock();
    sfx_stream_play_mutex.unlock();
}

void sfx_buffer_data_internal(ALuint buf, ALenum format, ALsizei freq)
{
    std::lock(sfx_stream_play_mutex, sfx_stream_io_mutex);
    unsigned short* buffer = (&sfx_stream_data.front() + current);

    size_t size = sfx_stream_data.size() - current;
    if (size > 4096)
        size = 4096;

    current += size;

    alBufferData(buf, format,
        buffer, size * sizeof(unsigned short), freq);

    sfx_stream_play_mutex.unlock();
    sfx_stream_io_mutex.unlock();
}

void sfx_run_stream_io_internal(SNDFILE* snd)
{
    sfx_stream_data.clear();

    short read_buf[4096];

    size_t read_size = 0;
    while ((read_size = sf_read_short(snd, read_buf, 4096)) != 0)
        sfx_add_data_internal(read_buf, read_buf + read_size);
}

void sfx_run_stream_openal_internal(SFX_SOURCE source, int bufCount, SF_INFO sfinfo)
{
    current = 0;
    ALuint* buffers = (ALuint*)malloc(bufCount * sizeof(ALuint));
    alGenBuffers((ALuint)bufCount, buffers);
    if (!sfx_checkerror_internal())
    {
        sfx_last_error = SFX_FAIL_CREATE_BUFFER;
        return;
    }

    for (int i = 0; i < bufCount; i++)
    {
        if (buffers == nullptr) // This check is here to prevent warning C6011
            break;

        sfx_buffer_data_internal(buffers[i],
            sfinfo.channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16,
            sfinfo.samplerate);

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

            sfx_buffer_data_internal(buf,
                sfinfo.channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16,
                sfinfo.samplerate);

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
    if (!sfx_checkerror_internal())
    {
        sfx_last_error = SFX_FAIL_PLAY_SOURCE;
        return;
    }
}

void SFXPLUSCALL sfx_source_open_stream(SFX_SOURCE source, const char* path)
{
    sfx_last_error = SFX_NO_ERROR;

    int bufCount = 6;

    SF_INFO sfinfo;
    memset(&sfinfo, 0, sizeof(sfinfo));

    SNDFILE* snd = sf_open(path, SFM_READ, &sfinfo);
    if (snd == nullptr)
    {
        sfx_last_error = SFX_FAIL_READ_FILE;
        return;
    }

    std::thread io_thread(sfx_run_stream_io_internal, snd);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::thread openal_thread(sfx_run_stream_openal_internal, source, bufCount, sfinfo);

    io_thread.join();
    openal_thread.join();
}