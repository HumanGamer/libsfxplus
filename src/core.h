#pragma once

#include "sfxplus.h"

typedef struct ALCdevice_struct ALCdevice;
typedef struct ALCcontext_struct ALCcontext;

extern bool sfx_initialized;

extern bool sfx_shuttingdown;

extern bool sfx_signal_kill;

extern ALCdevice* alc_device;
extern ALCcontext* alc_context;

extern int sfx_last_error;

extern bool sfx_checkerror_internal();
extern void sfx_setlasterror_internal(int error, bool alc = false);

extern void sfx_stream_close_streams_internal();

extern void sfx_source_run_loop_thread_internal();

extern bool sfx_source_get_looping_internal(SFX_SOURCE source);
