#include "core.h"

#include <al.h>

void SFXPLUSCALL sfx_listener_position(float x, float y, float z)
{
    alListener3f(AL_POSITION, x, y, z);
    if (!sfx_checkerror_internal())
    {
        sfx_last_error = SFX_FAIL_SET_PROPERTY;
        return;
    }
}

void SFXPLUSCALL sfx_listener_velocity(float x, float y, float z)
{
    alListener3f(AL_VELOCITY, x, y, z);
    if (!sfx_checkerror_internal())
    {
        sfx_last_error = SFX_FAIL_SET_PROPERTY;
        return;
    }
}

void SFXPLUSCALL sfx_listener_orientation(float forward_x, float forward_y, float forward_z, float up_x, float up_y, float up_z)
{
    ALfloat orientation[] = { forward_x, forward_y, forward_z, up_x, up_y, up_z};
    alListenerfv(AL_ORIENTATION, orientation);
    if (!sfx_checkerror_internal())
    {
        sfx_last_error = SFX_FAIL_SET_PROPERTY;
        return;
    }
}
