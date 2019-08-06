#pragma once

#include "sfxplus_config.h"

#define SFXPLUSCALL __cdecl

#ifdef SFXPLUS_SHARED
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
SFXPLUSEXP bool SFXPLUSCALL sfx_init();

#ifdef __cplusplus
}
#endif