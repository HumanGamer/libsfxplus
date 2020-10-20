#include "mp3.h"

#include <memory>
#define MINIMP3_IMPLEMENTATION
#define MINIMP3_NO_SIMD
#include <minimp3.h>
#include <minimp3_ex.h>

#include <cassert>

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

bool open_mp3(const char *path, MP3File *mp3)
{
    mp3dec_t *dec = new mp3dec_t();
    mp3dec_file_info_t *info = new mp3dec_file_info_t();
    mp3dec_map_info_t *map_info = new mp3dec_map_info_t();
    if (mp3dec_open_file(path, map_info))
    {
        delete dec;
        delete info;
        delete map_info;
        return false;
    }

    size_t orig_buf_size = map_info->size;
    mp3d_sample_t pcm[MINIMP3_MAX_SAMPLES_PER_FRAME];
    mp3dec_frame_info_t *frame_info = new mp3dec_frame_info_t();

    mp3->dec = dec;
    mp3->file_info = info;
    mp3->map_info = map_info;
    mp3->frame_info = frame_info;

    /* skip id3 */
    mp3dec_skip_id3(&map_info->buffer, &map_info->size);
    if (!map_info->size)
    {
        delete dec;
        delete info;
        delete map_info;
        delete frame_info;
        return false;
    }
    /* try to make allocation size assumption by first frame */
    mp3dec_init(dec);
    mp3->samples = 0;
    do
    {
        mp3->samples = mp3dec_decode_frame(dec, map_info->buffer, map_info->size, pcm, mp3->frame_info);
        map_info->buffer += frame_info->frame_bytes;
        map_info->size -= frame_info->frame_bytes;
        if (mp3->samples)
            break;
    } while (frame_info->frame_bytes);
    if (!mp3->samples)
    {
        mp3dec_close_file(map_info);
        delete dec;
        delete info;
        delete map_info;
        delete frame_info;
        return false;
    }
    mp3->samples *= frame_info->channels;
    mp3->allocated = (map_info->size / frame_info->frame_bytes) * mp3->samples * sizeof(mp3d_sample_t) + MINIMP3_MAX_SAMPLES_PER_FRAME * sizeof(mp3d_sample_t);
    info->buffer = (mp3d_sample_t *)malloc(mp3->allocated);
    if (!info->buffer)
    {
        mp3dec_close_file(map_info);
        delete dec;
        delete info;
        delete map_info;
        delete frame_info;
        return false;
    }
    info->samples = mp3->samples;
    memcpy(info->buffer, pcm, info->samples * sizeof(mp3d_sample_t));
    /* save info */
    info->channels = frame_info->channels;
    info->hz = frame_info->hz;
    info->layer = frame_info->layer;

    mp3->read_pos = 0;
    mp3->loaded = false;

    return true;
}

void close_mp3(MP3File *mp3)
{
    mp3dec_close_file(mp3->map_info);
    delete(mp3->map_info);
    delete(mp3->frame_info);
    delete(mp3->file_info);
    delete(mp3->dec);
}

sfx_size_t read_mp3(MP3File *mp3, unsigned short *ptr, sfx_size_t items)
{
    size_t avg_bitrate_kbps = mp3->frame_info->bitrate_kbps;
    size_t frames = 1;

    size_t bytes_to_read = items * sizeof(unsigned short);
    size_t bytes_read = 0;

    if (mp3->loaded)
    {
        bytes_read = mp3->file_info->samples * sizeof(mp3d_sample_t) - mp3->read_pos;
    } else
    {
        /* decode rest frames */
        int frame_bytes;
        do
        {
            if ((mp3->allocated - mp3->file_info->samples * sizeof(mp3d_sample_t)) < MINIMP3_MAX_SAMPLES_PER_FRAME * sizeof(mp3d_sample_t))
            {
                mp3->allocated *= 2;
                mp3->file_info->buffer = (mp3d_sample_t *)realloc(mp3->file_info->buffer, mp3->allocated);
            }
            mp3->samples = mp3dec_decode_frame(mp3->dec, mp3->map_info->buffer, mp3->map_info->size, mp3->file_info->buffer + mp3->file_info->samples, mp3->frame_info);
            frame_bytes = mp3->frame_info->frame_bytes;
            mp3->map_info->buffer += frame_bytes;
            mp3->map_info->size -= frame_bytes;
            if (mp3->samples)
            {
                if (mp3->file_info->hz != mp3->frame_info->hz || mp3->file_info->layer != mp3->frame_info->layer)
                    break;
                if (mp3->file_info->channels && mp3->file_info->channels != mp3->frame_info->channels)
    #ifdef MINIMP3_ALLOW_MONO_STEREO_TRANSITION
                    info->channels = 0; /* mark file with mono-stereo transition */
    #else
                    break;
    #endif
                mp3->file_info->samples += mp3->samples * mp3->frame_info->channels;
                avg_bitrate_kbps += mp3->frame_info->bitrate_kbps;
                frames++;
            }

            bytes_read += mp3->samples * mp3->frame_info->channels * sizeof(int16_t);
        } while (frame_bytes && bytes_read < bytes_to_read);

        if (!frame_bytes)
        {
            mp3->loaded = true;
            bytes_read = mp3->file_info->samples * sizeof(mp3d_sample_t) - mp3->read_pos;
        }

        /* reallocate to normal buffer size */
        if (frame_bytes == 0 && mp3->allocated != mp3->file_info->samples * sizeof(mp3d_sample_t))
            mp3->file_info->buffer = (mp3d_sample_t *)realloc(mp3->file_info->buffer, mp3->file_info->samples * sizeof(mp3d_sample_t));
        mp3->file_info->avg_bitrate_kbps = avg_bitrate_kbps / frames;

    }

    size_t ret = min(bytes_to_read, bytes_read);

    if (ret == 0)
        return 0;

    uint8_t* data_read = (uint8_t*)(mp3->file_info->buffer) + mp3->read_pos;
    #if defined(__STDC_WANT_LIB_EXT1__) || defined(_MSC_VER)
        memcpy_s((uint8_t *)ptr, bytes_to_read, data_read, ret);
    #else
       memcpy((uint8_t *)ptr, data_read, ret);
    #endif

    mp3->read_pos += ret;

    return ret / 2;
}