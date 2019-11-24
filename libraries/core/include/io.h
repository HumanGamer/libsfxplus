#pragma once

#include <cstddef>

struct SFX_FILE_HANDLE;

struct SFX_FILE
{
    SFX_FILE_HANDLE* file;
    int channels;
    int sample_rate;
};

extern SFX_FILE* sfx_io_open(const char* path);
extern void sfx_io_close(SFX_FILE* file);
extern size_t sfx_io_read(SFX_FILE* file, unsigned short* ptr, size_t items);
