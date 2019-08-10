#include "core.h"

#include <thread>
#include <map>
#include <vector>
#include <mutex>
#include <algorithm>
#include <al.h>

std::mutex sfx_source_check_thread_mutex, sfx_source_check_normal_mutex;

extern std::mutex sfx_stream_playback_mutex;

std::map<SFX_SOURCE, bool> sfx_source_should_loop_list;
std::map<SFX_SOURCE, bool> sfx_source_looping_list;
std::vector<SFX_SOURCE> sfx_source_list;

bool sfx_source_get_looping_internal(SFX_SOURCE source)
{
    std::lock(sfx_source_check_thread_mutex, sfx_stream_playback_mutex, sfx_source_check_normal_mutex);
    sfx_last_error = SFX_NO_ERROR;

    bool result = sfx_source_looping_list[source];

    sfx_source_check_thread_mutex.unlock();
    sfx_stream_playback_mutex.unlock();
    sfx_source_check_normal_mutex.unlock();

    return result;
}

bool sfx_source_loop_check_internal(SFX_SOURCE source)
{
    int source_state;
    alGetSourcei(source, AL_SOURCE_STATE, &source_state);
    if (!sfx_checkerror_internal())
    {
        sfx_last_error = SFX_FAIL_GET_STATE;
        return false;
    }

    if (source_state == AL_STOPPED)
        return true;

    return false;
}

void sfx_source_check_sources_internal()
{
    std::lock(sfx_source_check_normal_mutex, sfx_source_check_thread_mutex);
    int current = sfx_source_list.size() - 1;
    while (!sfx_shuttingdown && current >= 0 && current < sfx_source_list.size())
    {
        SFX_SOURCE source = sfx_source_list[current];

        if (!sfx_source_should_loop_list[source] || !sfx_source_looping_list[source])
        {
            current--;
            continue;
        }

        if (sfx_source_loop_check_internal(source))
        {
            alSourcePlay(source);
            if (!sfx_checkerror_internal())
            {
                sfx_last_error = SFX_FAIL_PLAY_SOURCE;
                sfx_source_check_thread_mutex.unlock();
                sfx_source_check_normal_mutex.unlock();
                return;
            }
        }

        current--;
    }

    sfx_source_check_normal_mutex.unlock();
    sfx_source_check_thread_mutex.unlock();
}

void sfx_source_run_loop_thread_internal()
{
    while (!sfx_shuttingdown)
    {
        sfx_source_check_sources_internal();
    }
}

SFX_SOURCE SFXPLUSCALL sfx_source_create(bool looping, float pitch, float gain)
{
    std::lock(sfx_source_check_thread_mutex, sfx_source_check_normal_mutex);
    sfx_last_error = SFX_NO_ERROR;

    ALuint source;
    alGenSources((ALuint)1, &source);
    if (!sfx_checkerror_internal())
    {
        sfx_last_error = SFX_FAIL_CREATE_SOURCE;
        sfx_source_check_thread_mutex.unlock();
        sfx_source_check_normal_mutex.unlock();
        return 0;
    }

    alSourcef(source, AL_PITCH, pitch);
    if (!sfx_checkerror_internal())
    {
        sfx_last_error = SFX_FAIL_CREATE_SOURCE;
        sfx_source_check_thread_mutex.unlock();
        sfx_source_check_normal_mutex.unlock();
        return 0;
    }

    alSourcef(source, AL_GAIN, gain);
    if (!sfx_checkerror_internal())
    {
        sfx_last_error = SFX_FAIL_CREATE_SOURCE;
        sfx_source_check_thread_mutex.unlock();
        sfx_source_check_normal_mutex.unlock();
        return 0;
    }

    sfx_source_list.push_back(source);
    sfx_source_should_loop_list[source] = looping;
    sfx_source_looping_list[source] = false;

    sfx_source_check_thread_mutex.unlock();
    sfx_source_check_normal_mutex.unlock();

    return source;
}

void SFXPLUSCALL sfx_source_destroy(SFX_SOURCE source)
{
    std::lock(sfx_source_check_thread_mutex, sfx_source_check_normal_mutex);
    sfx_last_error = SFX_NO_ERROR;

    std::remove(sfx_source_list.begin(), sfx_source_list.end(), source);

    alDeleteSources(1, &source);

    sfx_source_check_thread_mutex.unlock();
    sfx_source_check_normal_mutex.unlock();
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
    std::lock(sfx_source_check_thread_mutex, sfx_source_check_normal_mutex);
    sfx_last_error = SFX_NO_ERROR;

    sfx_source_should_loop_list[source] = looping;
    sfx_source_looping_list[source] = looping;

    sfx_source_check_thread_mutex.unlock();
    sfx_source_check_normal_mutex.unlock();
}

void SFXPLUSCALL sfx_source_position(SFX_SOURCE source, float x, float y, float z)
{
    sfx_last_error = SFX_NO_ERROR;

    alSource3f(source, AL_POSITION, x, y, z);
    if (!sfx_checkerror_internal())
    {
        sfx_last_error = SFX_FAIL_SET_PROPERTY;
        return;
    }
}

void SFXPLUSCALL sfx_source_velocity(SFX_SOURCE source, float x, float y, float z)
{
    sfx_last_error = SFX_NO_ERROR;

    alSource3f(source, AL_VELOCITY, x, y, z);
    if (!sfx_checkerror_internal())
    {
        sfx_last_error = SFX_FAIL_SET_PROPERTY;
        return;
    }
}

void SFXPLUSCALL sfx_source_attach_sound(SFX_SOURCE source, SFX_AUDIO audio)
{
    sfx_last_error = SFX_NO_ERROR;

    alSourcei(source, AL_BUFFER, audio);
    if (!sfx_checkerror_internal())
    {
        sfx_last_error = SFX_FAIL_QUEUE_BUFFER;
        return;
    }
}

void SFXPLUSCALL sfx_source_detach_sound(SFX_SOURCE source)
{
    sfx_last_error = SFX_NO_ERROR;

    alSourcei(source, AL_BUFFER, 0);
    if (!sfx_checkerror_internal())
    {
        sfx_last_error = SFX_FAIL_QUEUE_BUFFER;
        return;
    }
}

void SFXPLUSCALL sfx_source_play(SFX_SOURCE source)
{
    std::lock(sfx_source_check_thread_mutex, sfx_source_check_normal_mutex);

    sfx_source_looping_list[source] = sfx_source_should_loop_list[source];

    sfx_last_error = SFX_NO_ERROR;

    alSourcePlay(source);
    if (!sfx_checkerror_internal())
    {
        sfx_last_error = SFX_FAIL_PLAY_SOURCE;
        sfx_source_check_thread_mutex.unlock();
        sfx_source_check_normal_mutex.unlock();
        return;
    }

    sfx_source_check_thread_mutex.unlock();
    sfx_source_check_normal_mutex.unlock();
}

void SFXPLUSCALL sfx_source_pause(SFX_SOURCE source)
{
    sfx_last_error = SFX_NO_ERROR;

    alSourcePause(source);
    if (!sfx_checkerror_internal())
    {
        sfx_last_error = SFX_FAIL_PAUSE_SOURCE;
        return;
    }
}

void SFXPLUSCALL sfx_source_stop(SFX_SOURCE source)
{
    std::lock(sfx_source_check_thread_mutex, sfx_source_check_normal_mutex);

    sfx_source_looping_list[source] = false;

    sfx_last_error = SFX_NO_ERROR;

    alSourceStop(source);
    if (!sfx_checkerror_internal())
    {
        sfx_last_error = SFX_FAIL_STOP_SOURCE;
        sfx_source_check_thread_mutex.unlock();
        sfx_source_check_normal_mutex.unlock();
        return;
    }

    sfx_source_check_thread_mutex.unlock();
    sfx_source_check_normal_mutex.unlock();
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

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    int state = 0;

    do
    {
        state = sfx_source_getstate(source);
        if (sfx_error() != SFX_NO_ERROR)
        {
            if (sfx_signal_kill)
                break;
            return;
        }
    } while (sfx_source_get_looping_internal(source) || (state == SFX_SOURCE_STATE_PLAYING || state == SFX_SOURCE_STATE_PAUSED));

    if (sfx_signal_kill)
        sfx_last_error = SFX_NO_ERROR;
    else
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
}
