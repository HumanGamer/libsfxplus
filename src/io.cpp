#include "io.h"

#include <sndfile.h>
#include <cstring>

struct SFX_FILE_HANDLE
{
    SNDFILE* file;
    const char* path;
};

SFX_FILE* sfx_io_open(const char* path)
{
    SF_INFO sfinfo;
    memset(&sfinfo, 0, sizeof(sfinfo));

    SNDFILE* snd = sf_open(path, SFM_READ, &sfinfo);
    if (snd == nullptr)
        return nullptr;

    sf_command(snd, SFC_SET_SCALE_FLOAT_INT_READ, (void*)SF_TRUE, sizeof(SF_TRUE));

    SFX_FILE_HANDLE* sfxfile = new SFX_FILE_HANDLE();
    sfxfile->file = snd;
    sfxfile->path = path;

    SFX_FILE* info = new SFX_FILE();
    info->file = sfxfile;
    info->channels = sfinfo.channels;
    info->sample_rate = sfinfo.samplerate;

    return info;
}

void sfx_io_close(SFX_FILE* file)
{
    sf_close(file->file->file);
    delete(file->file);
    delete(file);
}

size_t sfx_io_read(SFX_FILE* file, unsigned short* ptr, size_t items)
{
    return sf_read_short(file->file->file, (short*)ptr, items);
}
