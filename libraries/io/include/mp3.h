#pragma once

#include "io.h"
#include <minimp3_ex.h>

struct MP3File
{
    mp3dec_t *dec;
    mp3dec_file_info_t *file_info;
    mp3dec_map_info_t *map_info;
    mp3dec_frame_info_t *frame_info;
    int samples;
    size_t allocated;
    size_t read_pos;
};

extern bool open_mp3(const char *path, MP3File *mp3);
extern void close_mp3(MP3File *mp3);
extern sfx_size_t read_mp3(MP3File *mp3, unsigned short *ptr, sfx_size_t items);