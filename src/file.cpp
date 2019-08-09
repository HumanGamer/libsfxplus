#include "core.h"

#include <vector>
#include <cstring>
#include <sndfile.h>
#include <al.h>

SFX_AUDIO SFXPLUSCALL sfx_file_load(const char* path)
{
    sfx_last_error = SFX_NO_ERROR;

    ALuint buffer;
    alGenBuffers((ALuint)1, &buffer);
    if (!sfx_checkerror_internal())
    {
        sfx_last_error = SFX_FAIL_CREATE_BUFFER;
        return 0;
    }

    SF_INFO sfinfo;
    memset(&sfinfo, 0, sizeof(sfinfo));

    SNDFILE* snd = sf_open(path, SFM_READ, &sfinfo);
    if (snd == nullptr)
    {
        sfx_last_error = SFX_FAIL_READ_FILE;
        return 0;
    }

    sf_command(snd, SFC_SET_SCALE_FLOAT_INT_READ, (void*)SF_TRUE, sizeof(SF_TRUE));

    std::vector<unsigned short> data;

    short read_buf[2048];

    size_t read_size = 0;
    while ((read_size = sf_read_short(snd, read_buf, 2048)) != 0)
    {
        data.insert(data.end(), read_buf, read_buf + read_size);
    }

    alBufferData(buffer, sfinfo.channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16,
        &data.front(), data.size() * sizeof(unsigned short), sfinfo.samplerate);
    if (!sfx_checkerror_internal())
    {
        sfx_last_error = SFX_FAIL_FILL_BUFFER;
        return 0;
    }

    return buffer;
}

void SFXPLUSCALL sfx_file_destroy(SFX_AUDIO audio)
{
    alDeleteBuffers(1, &audio);
}