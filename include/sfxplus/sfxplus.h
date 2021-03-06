#pragma once

#include "sfxplus_config.h"

#ifndef _WIN32
#define __cdecl __attribute__((__cdecl__))
#endif

// Check windows
#if _WIN32 || _WIN64
#if _WIN64
#define SFXPLUS64
#else
#define SFXPLUS32
#endif
#elif __GNUC__ // Check GCC
#if __x86_64__ || __ppc64__
#define SFXPLUS64
#else
#define SFXPLUS32
#endif
#else // Fail and show an error
#error "Missing feature-test macro for 32/64-bit on this compiler."
#endif

#ifdef SFXPLUS32
#define SFXPLUSCALL __cdecl
#else
#define SFXPLUSCALL
#endif

#if defined(SFXPLUS_SHARED) && defined(_WIN32)
#ifdef SFXPLUS_SHARED_EXPORT
#define SFXPLUSEXP extern __declspec(dllexport)
#else
#define SFXPLUSEXP extern __declspec(dllimport)
#endif
#else
#define SFXPLUSEXP extern
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#define SFX_INVALID_STREAM 0

typedef unsigned int SFX_SOURCE;
typedef unsigned int SFX_AUDIO;
typedef unsigned int SFX_STREAM;

enum SFX_ERROR
{
    SFX_NO_ERROR,
    SFX_INTERNAL_ERROR,
    SFX_FAIL_IO_INIT,
    SFX_FAIL_OPEN_DEVICE,
    SFX_FAIL_CREATE_CONTEXT,
    SFX_FAIL_CREATE_SOURCE,
    SFX_FAIL_CREATE_BUFFER,
    SFX_FAIL_DELETE_BUFFER,
    SFX_FAIL_PLAY_SOURCE,
    SFX_FAIL_PAUSE_SOURCE,
    SFX_FAIL_STOP_SOURCE,
    SFX_FAIL_FILL_BUFFER,
    SFX_FAIL_UNQUEUE_BUFFER,
    SFX_FAIL_QUEUE_BUFFER,
    SFX_FAIL_READ_FILE,
    SFX_FAIL_SET_PROPERTY,
    SFX_FAIL_GET_PROPERTY,
    SFX_FAIL_GET_STATE,
    SFX_INVALID_STATE,
    SFX_INVALID_DEVICE,
    SFX_INVALID_CONTEXT,
    SFX_INVALID_ENUM,
    SFX_INVALID_VALUE,
    SFX_OUT_OF_MEMORY,
    SFX_INVALID_OPERATION,
    SFX_UNKNOWN_ERROR = 0x10000
};

enum SFX_SOURCE_STATE
{
    SFX_SOURCE_STATE_INITIAL,
    SFX_SOURCE_STATE_PLAYING,
    SFX_SOURCE_STATE_PAUSED,
    SFX_SOURCE_STATE_STOPPED
};

/*
 * Initializes SFX+ Library
 * This must be the first thing called before any other functions in this library.
 */
SFXPLUSEXP bool SFXPLUSCALL sfx_startup();

/*
 * Unloads the SFX+ Library
 * This should be called when closing the program
 */
SFXPLUSEXP void SFXPLUSCALL sfx_shutdown();

/*
 * Gets the last error or SFX_NO_ERROR if there was no error.
 */
SFXPLUSEXP int SFXPLUSCALL sfx_error();

/*
 * Gets the specified error (or the last error) as a string.
 */
SFXPLUSEXP const char* SFXPLUSCALL sfx_error_string(
#ifdef __cplusplus
                                                    int error = -1
#else
                                                    int error
#endif
                                                    );

/*
 * Creates an audio source with the specified parameters
 */
SFXPLUSEXP SFX_SOURCE SFXPLUSCALL sfx_source_create(
#ifdef __cplusplus
                                                    bool looping = false, float pitch = 1.0f, float gain = 1.0f
#else
                                                    bool looping, float pitch, float gain
#endif
                                                    );

/*
 * Destroys an audio source
 */
SFXPLUSEXP void SFXPLUSCALL sfx_source_destroy(SFX_SOURCE source);

/*
 * Sets the pitch of the audio source
 */
SFXPLUSEXP void SFXPLUSCALL sfx_source_pitch(SFX_SOURCE source, float pitch);

/*
 * Sets the gain of the audio source
 */
SFXPLUSEXP void SFXPLUSCALL sfx_source_gain(SFX_SOURCE source, float gain);

/*
 * Sets whether or not the audio source should loop
 */
SFXPLUSEXP void SFXPLUSCALL sfx_source_looping(SFX_SOURCE source, bool looping);

/*
 * Sets the audio source position
 */
SFXPLUSEXP void SFXPLUSCALL sfx_source_position(SFX_SOURCE source, float x, float y, float z);

/*
 * Sets the audio source velocity
 */
SFXPLUSEXP void SFXPLUSCALL sfx_source_velocity(SFX_SOURCE source, float x, float y, float z);

/*
 * Attaches a sound to the specified source
 */
SFXPLUSEXP void SFXPLUSCALL sfx_source_attach_sound(SFX_SOURCE source, SFX_AUDIO audio);

/*
 * Detaches any sounds from the specified source
 */
SFXPLUSEXP void SFXPLUSCALL sfx_source_detach_sound(SFX_SOURCE source);

/*
 * Plays a source
 */
SFXPLUSEXP void SFXPLUSCALL sfx_source_play(SFX_SOURCE source);

/*
 * Pauses a source
 */
SFXPLUSEXP void SFXPLUSCALL sfx_source_pause(SFX_SOURCE source);

/*
 * Stops a source
 */
SFXPLUSEXP void SFXPLUSCALL sfx_source_stop(SFX_SOURCE source);

/*
 * Streams an audio file using the specified source
 */
SFXPLUSEXP SFX_STREAM SFXPLUSCALL sfx_stream_open(SFX_SOURCE source, const char* path,
#ifdef __cplusplus
                                                                                       int bufferCount = 20
#else
                                                                                       int bufferCount
#endif
                                                 );

/*
 * Closes an audio stream
 */
SFXPLUSEXP void SFXPLUSCALL sfx_stream_close(SFX_STREAM stream);

/*
 * Checks if the source is playing
 */
SFXPLUSEXP int SFXPLUSCALL sfx_source_getstate(SFX_SOURCE source);

/*
 * Waits for the sound to finish playing before continuing
 */
SFXPLUSEXP void SFXPLUSCALL sfx_source_wait(SFX_SOURCE source);

/*
 * Loads audio file from file
 */
SFXPLUSEXP SFX_AUDIO SFXPLUSCALL sfx_audiofile_load(const char* path);

/*
 * Destroys audio file handle
 */
SFXPLUSEXP void SFXPLUSCALL sfx_audiofile_destroy(SFX_AUDIO audio);

/*
 * Sets the listener position
 */
SFXPLUSEXP void SFXPLUSCALL sfx_listener_position(float x, float y, float z);

/*
 * Sets the listener velocity
 */
SFXPLUSEXP void SFXPLUSCALL sfx_listener_velocity(float x, float y, float z);

/*
 * Sets the listener orientation
 */
SFXPLUSEXP void SFXPLUSCALL sfx_listener_orientation(float forward_x, float forward_y, float forward_z, float up_x, float up_y, float up_z);

#ifdef __cplusplus
}
#endif