#pragma once

#include <sfxplus/sfxplusio.h>

#include <sndfile.h>
#include <cstring>

#include "mp3.h"

enum class SFXIO_FORMAT
{
    MP3,
    OTHER,
};

struct MP3File;

struct SFXIO_FILE_HANDLE
{
    SFXIO_FORMAT format;
    SNDFILE *file;
    MP3File *mp3;

    const char *path;
};