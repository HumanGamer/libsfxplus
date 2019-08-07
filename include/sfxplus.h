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

typedef unsigned int SFX_SOURCE;
typedef unsigned int SFX_AUDIO;

enum SFX_ERROR
{
    SFX_NO_ERROR,
    SFX_INTERNAL_ERROR,
    SFX_FAIL_OPEN_DEVICE,
    SFX_FAIL_CREATE_CONTEXT,
    SFX_FAIL_CREATE_SOURCE,
    SFX_FAIL_CREATE_BUFFER,
    SFX_FAIL_READ_FILE,
    SFX_FAIL_SET_PROPERTY,
    SFX_FAIL_GET_STATE,
    SFX_INVALID_STATE,
    SFX_INVALID_DEVICE,
    SFX_INVALID_CONTEXT,
    SFX_INVALID_ENUM,
    SFX_INVALID_VALUE,
    SFX_OUT_OF_MEMORY,
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
SFXPLUSEXP int SFXPLUSCALL sfx_getlasterror();

/*
 * Gets the specified error (or the last error) as a string.
 */
SFXPLUSEXP const char* SFXPLUSCALL sfx_errorstring(int error = -1);

/*
 * Creates an audio source with the specified parameters
 */
SFXPLUSEXP SFX_SOURCE SFXPLUSCALL sfx_create_source(float pitch = 1.0f, float gain = 1.0f, bool looping = false);

/*
 * Sets the pitch of the audio source
 */
SFXPLUSEXP void SFXPLUSCALL sfx_source_pitch(SFX_SOURCE source, float pitch);

/*
 * Sets the gain of the audio source
 */
void SFXPLUSCALL sfx_source_gain(SFX_SOURCE source, float gain);

/*
 * Set whether or not the audio source should loop
 */
void SFXPLUSCALL sfx_source_looping(SFX_SOURCE source, bool looping);

/*
 * Loads audio from file
 */
SFXPLUSEXP SFX_AUDIO SFXPLUSCALL sfx_load_audio(const char* path);

/*
 * Plays a sound using the specified source
 */
SFXPLUSEXP void SFXPLUSCALL sfx_source_play_sound(SFX_SOURCE source, SFX_AUDIO audio);

/*
 * Checks if the source is playing
 */
SFXPLUSEXP int SFXPLUSCALL sfx_source_getstate(SFX_SOURCE source);

/*
 * Waits for the sound to finish playing before continuing
 */
SFXPLUSEXP void SFXPLUSCALL sfx_source_wait(SFX_SOURCE source);

#ifdef __cplusplus
}
#endif