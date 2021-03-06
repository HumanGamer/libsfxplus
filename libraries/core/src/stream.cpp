#include "core.h"
#include "io.h"

#include <thread>
#include <mutex>
#include <vector>
#include <map>
#include <cstring>
#include <algorithm>
#include <al.h>

std::mutex sfx_stream_main_mutex, sfx_stream_io_mutex, sfx_stream_playback_mutex;

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
std::map<SFX_STREAM, bool> sfx_stream_io_started;
std::map<SFX_STREAM, bool> sfx_stream_openal_started;

bool sfx_stream_check_io_started(SFX_STREAM stream)
{
    std::lock(sfx_stream_main_mutex, sfx_stream_io_mutex);

    bool result = sfx_stream_io_started[stream];

    sfx_stream_main_mutex.unlock();
    sfx_stream_io_mutex.unlock();

    return result;
}

void sfx_stream_set_io_started(SFX_STREAM stream, bool started)
{
    std::lock(sfx_stream_io_mutex, sfx_stream_main_mutex);

    sfx_stream_io_started[stream] = started;

    sfx_stream_io_mutex.unlock();
    sfx_stream_main_mutex.unlock();
}

bool sfx_stream_check_openal_started(SFX_STREAM stream)
{
    std::lock(sfx_stream_main_mutex, sfx_stream_playback_mutex);

    bool result = sfx_stream_openal_started[stream];

    sfx_stream_main_mutex.unlock();
    sfx_stream_playback_mutex.unlock();

    return result;
}

void sfx_stream_set_openal_started(SFX_STREAM stream, bool started)
{
    std::lock(sfx_stream_playback_mutex, sfx_stream_main_mutex);

    sfx_stream_openal_started[stream] = started;

    sfx_stream_playback_mutex.unlock();
    sfx_stream_main_mutex.unlock();
}

void sfx_add_data_internal(SFX_STREAM stream, unsigned short* buffer, unsigned int bufferSize)
{
    std::lock(sfx_stream_io_mutex, sfx_stream_playback_mutex);

    sfx_stream_data[stream].insert(sfx_stream_data[stream].end(), buffer, buffer + bufferSize);

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

void sfx_run_stream_io_internal(SFX_STREAM stream, SFX_FILE* snd)
{
    sfx_stream_data[stream].clear();

    unsigned short read_buf[2048];

    long long loops = 0;

    // The amount of loops before starting the playback thread.
    // This may need to be adjusted. Needs more testing...
    long long loopsToStart = 3;

    size_t read_size = 0;
    while (snd != nullptr && sfx_stream_running[stream] && (read_size = sfx_io_read(snd, read_buf, 2048)) != 0)
    {
        sfx_add_data_internal(stream, read_buf, read_size);

        if (++loops >= loopsToStart || read_size < 2047)
            sfx_stream_set_io_started(stream, true);
    }
}

void sfx_run_stream_openal_internal(SFX_STREAM stream, SFX_SOURCE source, int bufCount, SFX_FILE* file)
{
    sfx_stream_current[stream] = 0;
    ALuint* buffers = new ALuint[bufCount];
    alGenBuffers((ALuint)bufCount, buffers);
    if (!sfx_checkerror_internal())
    {
        sfx_last_error = SFX_FAIL_CREATE_BUFFER;

        // Prevent getting stuck in an infinite loop on the main thread!
        sfx_stream_set_openal_started(stream, true);
        delete[](buffers);
        sfx_io_close(file);
        return;
    }

    for (int i = 0; i < bufCount; i++)
    {
        if (buffers == nullptr) // This check is here to prevent warning C6011
            break;

        sfx_buffer_data_internal(stream, buffers[i],
            file->channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16,
            file->sample_rate);

        if (!sfx_checkerror_internal())
        {
            sfx_last_error = SFX_FAIL_FILL_BUFFER;

            // Prevent getting stuck in an infinite loop on the main thread!
            sfx_stream_set_openal_started(stream, true);
            delete[](buffers);
            sfx_io_close(file);
            return;
        }
    }

    alSourceQueueBuffers(source, bufCount, buffers);
    if (!sfx_checkerror_internal())
    {
        sfx_last_error = SFX_FAIL_QUEUE_BUFFER;

        // Prevent getting stuck in an infinite loop on the main thread!
        sfx_stream_set_openal_started(stream, true);
        delete[](buffers);
        sfx_io_close(file);
        return;
    }

    sfx_stream_set_openal_started(stream, true);

    while (true)
    {
        int state;
        alGetSourcei(source, AL_SOURCE_STATE, &state);
        if (!sfx_checkerror_internal())
        {
            sfx_last_error = SFX_FAIL_GET_STATE;
            delete[](buffers);
            sfx_io_close(file);
            return;
        }

        if (state == AL_PLAYING)
            break;

        if (!sfx_stream_running[stream])
        {
            delete[](buffers);
            sfx_io_close(file);
            return;
        }
    }

    unsigned int buf;

    while (sfx_stream_running[stream])
    {
        int numProcessed;
        alGetSourcei(source, AL_BUFFERS_PROCESSED, &numProcessed);
        if (!sfx_checkerror_internal())
        {
            sfx_last_error = SFX_FAIL_GET_PROPERTY;
            delete[](buffers);
            sfx_io_close(file);
            return;
        }

        while (numProcessed > 0)
        {
            alSourceUnqueueBuffers(source, 1, &buf);
            if (!sfx_checkerror_internal())
            {
                //sfx_last_error = SFX_FAIL_UNQUEUE_BUFFER;
                //delete[](buffers);
                //sfx_io_close(file);
                //return;
                // TODO: Fix this error correctly instead of ignoring it, it's caused by AL_INVALID_VALUE
                break;
            }
            numProcessed--;

            if (sfx_buffer_data_internal(stream, buf,
                file->channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16,
                file->sample_rate))
            {
                if (!sfx_checkerror_internal())
                {
                    sfx_last_error = SFX_FAIL_FILL_BUFFER;
                    delete[](buffers);
                    sfx_io_close(file);
                    return;
                }

                alSourceQueueBuffers(source, 1, &buf);
                if (!sfx_checkerror_internal())
                {
                    sfx_last_error = SFX_FAIL_QUEUE_BUFFER;
                    delete[](buffers);
                    sfx_io_close(file);
                    return;
                }
            }
        }

        int state;
        alGetSourcei(source, AL_SOURCE_STATE, &state);
        if (!sfx_checkerror_internal())
        {
            sfx_last_error = SFX_FAIL_GET_STATE;
            sfx_io_close(file);
            return;
        }

        if (state != AL_PLAYING && state != AL_PAUSED)
        {
            int queued;
            alGetSourcei(source, AL_BUFFERS_QUEUED, &queued);
            if (!sfx_checkerror_internal())
            {
                sfx_last_error = SFX_FAIL_GET_PROPERTY;
                delete[](buffers);
                sfx_io_close(file);
                return;
            }
            if (queued == 0)
            {
                delete[](buffers);
                if (sfx_source_get_looping_internal(source))
                    sfx_run_stream_openal_internal(stream, source, bufCount, file);
                else
                {
                    sfx_stream_running[stream] = false;

                    sfx_io_close(file);
                }

                return;
            }

            alSourcePlay(source);
            if (!sfx_checkerror_internal())
            {
                sfx_last_error = SFX_FAIL_PLAY_SOURCE;
                delete[](buffers);
                sfx_io_close(file);
                return;
            }
        }
    }

    sfx_io_close(file);

    alSourceStop(source);
    if (!sfx_checkerror_internal())
    {
        sfx_last_error = SFX_FAIL_STOP_SOURCE;
        delete[](buffers);
        return;
    }

    int numProcessed;
    alGetSourcei(source, AL_BUFFERS_PROCESSED, &numProcessed);
    if (!sfx_checkerror_internal())
    {
        sfx_last_error = SFX_FAIL_GET_PROPERTY;
        delete[](buffers);
        return;
    }

    while(numProcessed > 0)
    {
        alSourceUnqueueBuffers(source, 1, &buf);
        if (!sfx_checkerror_internal())
        {
            sfx_last_error = SFX_FAIL_UNQUEUE_BUFFER;
            delete[](buffers);
            return;
        }
        numProcessed--;
    }

    alDeleteBuffers(bufCount, buffers);
    if (!sfx_checkerror_internal())
    {
        sfx_last_error = SFX_FAIL_DELETE_BUFFER;
        delete[](buffers);
        return;
    }

    delete[](buffers);
}

SFX_STREAM SFXPLUSCALL sfx_stream_open(SFX_SOURCE source, const char* path, int bufferCount)
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
    sfx_stream_io_started[stream] = false;
    sfx_stream_openal_started[stream] = false;

    SFX_FILE* file = sfx_io_open(path);
    if (file == nullptr)
    {
        sfx_last_error = SFX_FAIL_READ_FILE;
        return SFX_INVALID_STREAM;
    }

    SFX_STREAM_THREADS threads;
    threads.io = std::make_shared<std::thread>(sfx_run_stream_io_internal, stream, file);

    // Wait for there to be enough data before continuing...
    while (true)
    {
        if (sfx_stream_check_io_started(stream))
            break;
    }

    threads.playback = std::make_shared<std::thread>(sfx_run_stream_openal_internal, stream, source, bufferCount, file);

    sfx_stream_thread[stream] = threads;

    // Wait for stream to be initialized before continuing...
    while (true)
    {
        if (sfx_stream_check_openal_started(stream))
            break;
    }

    return stream;
}

void SFXPLUSCALL sfx_stream_close(SFX_STREAM stream)
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

void sfx_stream_close_streams_internal()
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
