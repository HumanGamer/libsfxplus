#pragma once

#define MINIMP3_IMPLEMENTATION
#define MINIMP3_NO_SIMD
#include <minimp3_ex.h>
#include <sndfile.h>
#include <cstring>

#include "mp3.h"

enum class SFXIO_FORMAT
{
    MP3,
    OTHER,
};

struct SFXIO_FILE_HANDLE
{
    SFXIO_FORMAT format;
    SNDFILE *file;
    MP3File *mp3;

    const char *path;
};