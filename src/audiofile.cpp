#include "core.h"
#include "io.h"

#include <vector>
#include <cstring>
#include <al.h>

SFX_AUDIO SFXPLUSCALL sfx_audiofile_load(const char* path)
{
    sfx_last_error = SFX_NO_ERROR;

    ALuint buffer;
    alGenBuffers((ALuint)1, &buffer);
    if (!sfx_checkerror_internal())
    {
        sfx_last_error = SFX_FAIL_CREATE_BUFFER;
        return 0;
    }

    SFX_FILE* file = sfx_io_open(path);
    if (file == nullptr)
    {
        sfx_last_error = SFX_FAIL_READ_FILE;
        return 0;
    }

    std::vector<unsigned short> data;

    unsigned short read_buf[2048];

    size_t read_size = 0;
    while ((read_size = sfx_io_read(file, read_buf, 2048)) != 0)
    {
        data.insert(data.end(), read_buf, read_buf + read_size);
    }

    alBufferData(buffer, file->channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16,
        &data.front(), data.size() * sizeof(unsigned short), file->sample_rate);
    if (!sfx_checkerror_internal())
    {
        sfx_last_error = SFX_FAIL_FILL_BUFFER;
        return 0;
    }

    return buffer;
}

void SFXPLUSCALL sfx_audiofile_destroy(SFX_AUDIO audio)
{
    sfx_last_error = SFX_NO_ERROR;

    alDeleteBuffers(1, &audio);
    if (!sfx_checkerror_internal())
    {
        sfx_last_error = SFX_FAIL_DELETE_BUFFER;
        return;
    }
}
