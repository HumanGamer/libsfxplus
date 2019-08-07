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

#ifdef __cplusplus
}
#endif