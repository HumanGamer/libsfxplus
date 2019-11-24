#pragma once

#include <sndfile.h>
#include <cstring>

struct SFXIO_FILE_HANDLE
{
    SNDFILE *file;
    const char *path;
};