#ifndef RETRO_H
#define RETRO_H

#include "libretro.h"
#include <string>
#include <array>

struct Retro final
{
    struct Callbacks final
    {
        retro_environment_t environment = nullptr;
        retro_video_refresh_t video_refresh = nullptr;
        retro_audio_sample_batch_t audio_sample_batch = nullptr;
        retro_input_poll_t input_poll = nullptr;
        retro_input_state_t input_state = nullptr;
    };

    Callbacks callbacks;

    std::array<unsigned, 2> controller_port_devices;
};

#endif // RETRO_H
