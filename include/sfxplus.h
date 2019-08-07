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

enum SFX_ERROR
{
    SFX_NO_ERROR,
    SFX_INTERNAL_ERROR,
    SFX_FAIL_OPEN_DEVICE,
    SFX_FAIL_CREATE_CONTEXT,
    SFX_INVALID_DEVICE,
    SFX_INVALID_CONTEXT,
    SFX_INVALID_ENUM,
    SFX_INVALID_VALUE,
    SFX_OUT_OF_MEMORY,
    SFX_UNKNOWN_ERROR = 0x10000
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

#ifdef __cplusplus
}
#endif