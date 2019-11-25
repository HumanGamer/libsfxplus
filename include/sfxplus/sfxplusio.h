#pragma once

#include "sfxplus_config.h"

#ifndef _WIN32
#define __cdecl __attribute__((__cdecl__))
#endif

// Check windows
#if _WIN32 || _WIN64
#if _WIN64
#define SFXPLUSIO64
#else
#define SFXPLUSIO32
#endif
#elif __GNUC__ // Check GCC
#if __x86_64__ || __ppc64__
#define SFXPLUSIO64
#else
#define SFXPLUSIO32
#endif
#else // Fail and show an error
#error "Missing feature-test macro for 32/64-bit on this compiler."
#endif

#ifdef SFXPLUSIO32
#define SFXPLUSIOCALL __cdecl
#else
#define SFXPLUSIOCALL
#endif

#if defined(SFXPLUS_SHARED) && defined(_WIN32)
#ifdef SFXPLUSIO_SHARED_EXPORT
#define SFXPLUSIOEXP extern __declspec(dllexport)
#else
#define SFXPLUSIOEXP extern __declspec(dllimport)
#endif
#else
#define SFXPLUSIOEXP extern
#endif

#ifdef SFXPLUSIO64
typedef unsigned __int64 sfx_size_t;
#else
typedef unsigned int     sfx_size_t;
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    struct SFXIO_FILE_HANDLE;

    struct SFXIO_FILE
    {
        SFXIO_FILE_HANDLE *file;
        int channels;
        int sample_rate;
    };

/*
 * Initializes SFX+ IO Library
 * This must be the first thing called before any other functions in this library.
 */
SFXPLUSIOEXP bool sfxio_startup();

/*
 * Unloads the SFX+ IO Library
 * This should be called when closing the program
 */
SFXPLUSIOEXP void SFXPLUSIOCALL sfxio_shutdown();

/*
 * Opens a sound file at the specified path
 */
SFXPLUSIOEXP SFXIO_FILE *sfxio_open(const char *path);

/*
 * Closes a sound file handle
 */
SFXPLUSIOEXP void sfxio_close(SFXIO_FILE *file);

/*
 * Reads data from the sound file
 */
SFXPLUSIOEXP sfx_size_t sfxio_read(SFXIO_FILE *file, unsigned short *ptr, sfx_size_t items);

#ifdef __cplusplus
}
#endif