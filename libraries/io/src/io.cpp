#include <sfxplus/sfxplusio.h>
#include "io.h"

#include <sndfile.h>
#include <cstring>

struct SFXIO_FILE_HANDLE
{
    SNDFILE *file;
    const char *path;
};

SFXIO_FILE *sfxio_open(const char *path)
{
    SF_INFO sfinfo;
    memset(&sfinfo, 0, sizeof(sfinfo));

    SNDFILE *snd = sf_open(path, SFM_READ, &sfinfo);
    if (snd == nullptr)
        return nullptr;

    sf_command(snd, SFC_SET_SCALE_FLOAT_INT_READ, (void *)SF_TRUE, sizeof(SF_TRUE));

    SFXIO_FILE_HANDLE *sfxfile = new SFXIO_FILE_HANDLE();
    sfxfile->file = snd;
    sfxfile->path = path;

    SFXIO_FILE *info = new SFXIO_FILE();
    info->file = sfxfile;
    info->channels = sfinfo.channels;
    info->sample_rate = sfinfo.samplerate;

    return info;
}

void sfxio_close(SFXIO_FILE *file)
{
    sf_close(file->file->file);
    delete(file->file);
    delete(file);
}

sfx_size_t sfxio_read(SFXIO_FILE *file, unsigned short *ptr, sfx_size_t items)
{
    return sf_read_short(file->file->file, (short *)ptr, items);
}
