#include "core.h"

#include <thread>
#include <mutex>
#include <vector>
#include <map>
#include <cstring>
#include <algorithm>
#include <sndfile.h>
#include <al.h>

std::mutex sfx_stream_io_mutex, sfx_stream_playback_mutex;

std::vector<SFX_STREAM> sfx_stream_ids;

struct SFX_STREAM_THREADS
{
    std::shared_ptr<std::thread> io;
    std::shared_ptr<std::thread> playback;
};

std::map<SFX_STREAM, SFX_SOURCE> sfx_stream_source;
std::map<SFX_STREAM, SFX_STREAM_THREADS> sfx_stream_thread;
std::map<SFX_STREAM, bool> sfx_stream_running;
std::map<SFX_STREAM, std::vector<unsigned short>> sfx_stream_data;
std::map<SFX_STREAM, size_t> sfx_stream_current;

void sfx_add_data_internal(SFX_STREAM stream, float* buffer, unsigned int bufferSize)
{
    std::lock(sfx_stream_io_mutex, sfx_stream_playback_mutex);

    for (int i = 0; i < bufferSize; i++)
    {
        if (!sfx_stream_running[stream])
            break;
        sfx_stream_data[stream].push_back((unsigned short)(buffer[i] * 10000.0f));
    }

    sfx_stream_io_mutex.unlock();
    sfx_stream_playback_mutex.unlock();
}

bool sfx_buffer_data_internal(SFX_STREAM stream, ALuint buf, ALenum format, ALsizei freq)
{
    std::lock(sfx_stream_playback_mutex, sfx_stream_io_mutex);
    unsigned short* buffer = (&sfx_stream_data[stream].front() + sfx_stream_current[stream]);

    size_t size = sfx_stream_data[stream].size() - sfx_stream_current[stream];
    if (size > 2048)
        size = 2048;

    sfx_stream_current[stream] += size;

    alBufferData(buf, format,
        buffer, size * sizeof(unsigned short), freq);

    sfx_stream_playback_mutex.unlock();
    sfx_stream_io_mutex.unlock();

    if (size == 0)
        return false;

    return true;
}

void sfx_run_stream_io_internal(SFX_STREAM stream, SNDFILE* snd)
{
    sfx_stream_data[stream].clear();

    float read_buf[2048];

    size_t read_size = 0;
    while (sfx_stream_running[stream] && (read_size = sf_read_float(snd, read_buf, 2048)) != 0)
        sfx_add_data_internal(stream, read_buf, read_size);
}

void sfx_run_stream_openal_internal(SFX_STREAM stream, SFX_SOURCE source, int bufCount, SF_INFO sfinfo)
{
    sfx_stream_current[stream] = 0;
    ALuint* buffers = new ALuint[bufCount];
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

        sfx_buffer_data_internal(stream, buffers[i],
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

    while (sfx_stream_running[stream])
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

            if (sfx_buffer_data_internal(stream, buf,
                sfinfo.channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16,
                sfinfo.samplerate))
            {
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
            {
                sfx_stream_running[stream] = false;
                return;
            }

            alSourcePlay(source);
            if (!sfx_checkerror_internal())
            {
                sfx_last_error = SFX_FAIL_PLAY_SOURCE;
                return;
            }
        }
    }

    alSourceStop(source);
    if (!sfx_checkerror_internal())
    {
        sfx_last_error = SFX_FAIL_STOP_SOURCE;
        return;
    }

    int numProcessed;
    alGetSourcei(source, AL_BUFFERS_PROCESSED, &numProcessed);
    if (!sfx_checkerror_internal())
    {
        sfx_last_error = SFX_FAIL_GET_PROPERTY;
        return;
    }

    while(numProcessed > 0)
    {
        alSourceUnqueueBuffers(source, 1, &buf);
        if (!sfx_checkerror_internal())
        {
            sfx_last_error = SFX_FAIL_UNQUEUE_BUFFER;
            return;
        }
        numProcessed--;
    }

    alDeleteBuffers(bufCount, buffers);
    if (!sfx_checkerror_internal())
    {
        sfx_last_error = SFX_FAIL_DELETE_BUFFER;
        return;
    }

    delete[](buffers);
}

SFX_STREAM SFXPLUSCALL sfx_source_open_stream(SFX_SOURCE source, const char* path, int bufferCount)
{
    sfx_last_error = SFX_NO_ERROR;

    // Start IDs at 1 so that 0 means invalid
    SFX_STREAM stream = 1;

    std::sort(sfx_stream_ids.begin(), sfx_stream_ids.end());

    for (const auto& id : sfx_stream_ids)
    {
        if (id == stream)
            stream++;
    }

    sfx_stream_ids.push_back(stream);

    sfx_stream_source[stream] = source;
    sfx_stream_running[stream] = true;

    SF_INFO sfinfo;
    memset(&sfinfo, 0, sizeof(sfinfo));

    SNDFILE* snd = sf_open(path, SFM_READ, &sfinfo);
    if (snd == nullptr)
    {
        sfx_last_error = SFX_FAIL_READ_FILE;
        return SFX_INVALID_STREAM;
    }

    SFX_STREAM_THREADS threads;
    threads.io = std::make_shared<std::thread>(sfx_run_stream_io_internal, stream, snd);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    threads.playback = std::make_shared<std::thread>(sfx_run_stream_openal_internal, stream, source, bufferCount, sfinfo);

    sfx_stream_thread[stream] = threads;

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    return stream;
}

void SFXPLUSCALL sfx_source_close_stream(SFX_STREAM stream)
{
    sfx_last_error = SFX_NO_ERROR;

    if (std::find(sfx_stream_ids.begin(), sfx_stream_ids.end(), stream) == sfx_stream_ids.end())
        return;

    sfx_stream_running[stream] = false;

    if (!sfx_signal_kill)
    {
        auto threads = sfx_stream_thread[stream];
        auto io = threads.io;
        auto playback = threads.playback;

        if (io != nullptr && io->joinable())
            io->join();

        if (playback != nullptr && playback->joinable())
            playback->join();

        sfx_stream_source.erase(stream);
        sfx_stream_thread.erase(stream);
        sfx_stream_running.erase(stream);
        sfx_stream_data.erase(stream);
        sfx_stream_current.erase(stream);

        std::remove(sfx_stream_ids.begin(), sfx_stream_ids.end(), stream);
    }
}

void sfx_source_close_streams_internal()
{
    for (const auto& stream : sfx_stream_ids)
    {
        sfx_last_error = SFX_NO_ERROR;

        auto threads = sfx_stream_thread[stream];
        auto io = threads.io;
        auto playback = threads.playback;

        sfx_stream_running[stream] = false;

        if (io != nullptr && io->joinable())
            io->join();

        if (playback != nullptr && playback->joinable())
            playback->join();
    }
}
