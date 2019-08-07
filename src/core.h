#pragma once

#include "sfxplus.h"

typedef struct ALCdevice_struct ALCdevice;
typedef struct ALCcontext_struct ALCcontext;

extern bool sfx_initialized;

extern ALCdevice* alc_device;
extern ALCcontext* alc_context;

extern int sfx_last_error;

extern bool sfx_checkerror_internal();
extern void sfx_setlasterror_internal(int error);
