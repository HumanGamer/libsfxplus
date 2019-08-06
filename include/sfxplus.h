#pragma once

#define __SFXPLUSCALL __cdecl
#define __SFXPLUSEXP extern

#ifdef __cplusplus
extern "C"
{
#endif

__SFXPLUSEXP bool __SFXPLUSCALL sfx_init(); // Initializes SFX+ Library

#ifdef __cplusplus
}
#endif