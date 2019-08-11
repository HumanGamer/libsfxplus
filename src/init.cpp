#include "core.h"

#include <thread>
#include <csignal>
#include <al.h>
#include <alc.h>
#include <stdio.h>

#ifdef _WIN32
#include <Windows.h>
#endif

bool sfx_initialized = false;

bool sfx_shuttingdown = false;

bool sfx_signal_kill = false;

ALCdevice* alc_device = nullptr;
ALCcontext* alc_context = nullptr;

std::thread sfx_source_loop_thread;

int sfx_last_error = SFX_NO_ERROR;

void sfx_signal_handler_internal(int signum);

#ifdef _WIN32
BOOL ctrl_handler(DWORD event)
{
    if (event == CTRL_CLOSE_EVENT) {
        sfx_signal_kill = true;
        sfx_shutdown();
        return TRUE;
    }
    return FALSE;
}
#endif

bool SFXPLUSCALL sfx_startup()
{
    sfx_last_error = SFX_NO_ERROR;

    if (sfx_initialized)
        return true;

    signal(SIGINT, sfx_signal_handler_internal);

#ifdef _WIN32
    SetConsoleCtrlHandler((PHANDLER_ROUTINE)(ctrl_handler), TRUE);
#endif

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
        sfx_setlasterror_internal(error, true);
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
        sfx_setlasterror_internal(error, true);
        return false;
    }

    sfx_source_loop_thread = std::thread(sfx_source_run_loop_thread_internal);

    sfx_initialized = true;
    return true;
}

void SFXPLUSCALL sfx_shutdown()
{
    if (!sfx_initialized)
        return;
    sfx_last_error = SFX_NO_ERROR;

    sfx_shuttingdown = true;

    if (sfx_source_loop_thread.joinable())
        sfx_source_loop_thread.join();

    sfx_stream_close_streams_internal();

    alc_device = alcGetContextsDevice(alc_context);
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(alc_context);
    alcCloseDevice(alc_device);
}

void sfx_signal_handler_internal(int signum)
{
    sfx_signal_kill = true;
    sfx_shutdown();
}
