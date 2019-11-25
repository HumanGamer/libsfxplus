#include "io.h"

bool initialized = false;

bool sfxio_startup()
{
    if (initialized)
        return true;

    // Nothing for now

    initialized = true;

    return true;
}

void sfxio_shutdown()
{
    // Nothing for now
}

SFXIO_FILE *sfxio_open(const char *path)
{
    if (!initialized)
        return nullptr;

    MP3File *mp3 = new MP3File();

    if (open_mp3(path, mp3))
    {
        // success

        SFXIO_FILE_HANDLE *sfxfile = new SFXIO_FILE_HANDLE();
        sfxfile->mp3 = mp3;
        sfxfile->format = SFXIO_FORMAT::MP3;
        sfxfile->path = path;

        SFXIO_FILE *info = new SFXIO_FILE();
        info->file = sfxfile;
        info->channels = mp3->file_info->channels;
        info->sample_rate = mp3->file_info->hz;

        return info;

    } else {

        // TODO: Find a better way to clean this up
        delete mp3; // Clean up from mp3 attempt

        SF_INFO sfinfo;
        memset(&sfinfo, 0, sizeof(sfinfo));

        SNDFILE *snd = sf_open(path, SFM_READ, &sfinfo);
        if (snd == nullptr)
            return nullptr;

        sf_command(snd, SFC_SET_SCALE_FLOAT_INT_READ, (void *)SF_TRUE, sizeof(SF_TRUE));

        SFXIO_FILE_HANDLE *sfxfile = new SFXIO_FILE_HANDLE();
        sfxfile->file = snd;
        sfxfile->format = SFXIO_FORMAT::OTHER;
        sfxfile->path = path;

        SFXIO_FILE *info = new SFXIO_FILE();
        info->file = sfxfile;
        info->channels = sfinfo.channels;
        info->sample_rate = sfinfo.samplerate;

        return info;
    }
}

void sfxio_close(SFXIO_FILE *file)
{
    if (!initialized)
        return;

    SFXIO_FORMAT format = file->file->format;

    switch (format)
    {
        case SFXIO_FORMAT::OTHER:
            sf_close(file->file->file);
            break;
        case SFXIO_FORMAT::MP3:
        {
            close_mp3(file->file->mp3);
            delete(file->file->mp3);
            break;
        }
    }
    delete(file->file);
    delete(file);
}

sfx_size_t sfxio_read(SFXIO_FILE *file, unsigned short *ptr, sfx_size_t items)
{
    if (!initialized)
        return 0;

    SFXIO_FORMAT format = file->file->format;

    switch (format)
    {
        case SFXIO_FORMAT::MP3:
            return read_mp3(file->file->mp3, ptr, items);
        case SFXIO_FORMAT::OTHER:
            return sf_read_short(file->file->file, (short *)ptr, items);
    }

}
