#include "io.h"

#include <sfxplus/sfxplusio.h>
#include <cstring>

struct SFX_FILE_HANDLE
{
    SFXIO_FILE* file;
    const char* path;
};

SFX_FILE* sfx_io_open(const char* path)
{
    SFXIO_FILE* snd = sfxio_open(path);
    if (snd == nullptr)
        return nullptr;

    SFX_FILE_HANDLE* sfxfile = new SFX_FILE_HANDLE();
    sfxfile->file = snd;
    sfxfile->path = path;

    SFX_FILE* info = new SFX_FILE();
    info->file = sfxfile;
    info->channels = snd->channels;
    info->sample_rate = snd->sample_rate;

    return info;
}

void sfx_io_close(SFX_FILE* file)
{
    sfxio_close(file->file->file);
    delete(file->file);
    delete(file);
}

size_t sfx_io_read(SFX_FILE* file, unsigned short* ptr, size_t items)
{
    return sfxio_read(file->file->file, ptr, items);
}
