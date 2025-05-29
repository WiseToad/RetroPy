#include "libretro.h"
#include "retro.h"
#include "retrolog.h"
#include "retropy.h"
#include <cstring>

Retro retro;
std::unique_ptr<RetroPy> retroPy;

bool setControllerInfo();
bool setInputDescriptors();
void pollJoypad(unsigned port);

RETRO_CALLCONV void retro_keyboard_event(bool down, unsigned keycode, uint32_t character, uint16_t key_modifiers);

RETRO_API void retro_get_system_info(retro_system_info *system_info)
{
    Logger::get()->debug("RETRO_API callback: retro_get_system_info");

    system_info->library_name = "RetroPy";
    system_info->library_version = "1.0";
    system_info->valid_extensions = "py";
    system_info->need_fullpath = true;
    system_info->block_extract = true;
}

RETRO_API void retro_set_environment(retro_environment_t environment)
{
    Logger::get()->debug("RETRO_API callback: retro_set_environment");

    retro.callbacks.environment = environment;

    bool support_no_game = false;
    retro.callbacks.environment(RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME, &support_no_game);
}

RETRO_API void retro_init()
{
    Logger::get()->debug("RETRO_API callback: retro_init");

    retro_log_callback log_callback;
    if (retro.callbacks.environment(RETRO_ENVIRONMENT_GET_LOG_INTERFACE, &log_callback))
    {
        Logger::get()->debug("Switching to libretro logging interface");
        Logger::get()->setAppender(std::make_unique<RetroLogAppender>(log_callback.log));
    }

    retroPy = std::make_unique<RetroPy>();
}

RETRO_API void retro_deinit()
{
    retroPy.reset();
    Logger::get()->resetAppender();
}

std::string getDirectory(unsigned cmd)
{
    const char *dir = nullptr;
    if (!retro.callbacks.environment(cmd, &dir))
        dir = nullptr;

    return std::string(dir ? dir : "");
}

RETRO_API bool retro_load_game(const retro_game_info *game_info)
{
    Logger::get()->debug("RETRO_API callback: retro_load_game");

    const char *systemDir = nullptr;
    if (!retro.callbacks.environment(RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY, &systemDir))
    {
        Logger::get()->error("Failed to obtain system directory");
        return false;
    }
    if (!retroPy->init(std::string(systemDir) + "/RetroPy"))
        return false;

    if (game_info == nullptr || game_info->path == nullptr)
    {
        Logger::get()->error("No content for loading provided");
        return false;
    }
    if (!retroPy->open(game_info->path))
        return false;

    int pixel_format = RETRO_PIXEL_FORMAT_XRGB8888;
    if (!retro.callbacks.environment(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &pixel_format))
    {
        Logger::get()->error("Failed to set pixel format");
        return false;
    }

    retro_keyboard_callback keyboard_callback = {&retro_keyboard_event};
    if (!retro.callbacks.environment(RETRO_ENVIRONMENT_SET_KEYBOARD_CALLBACK, &keyboard_callback))
    {
        Logger::get()->error("Failed to set keyboard callback");
        return false;
    }

    if (!setControllerInfo() || !setInputDescriptors())
    {
        Logger::get()->error("Failed to initialize controllers");
        return false;
    }

    return true;
}

bool setControllerInfo()
{
    static retro_controller_description controller_descriptions[] = {
        {"Keyboard", RETRO_DEVICE_KEYBOARD},
        {"Joypad", RETRO_DEVICE_JOYPAD},
        {}};

    static retro_controller_info controller_info[] = {
        {controller_descriptions, 2},
        {controller_descriptions, 2},
        {}};

    return retro.callbacks.environment(RETRO_ENVIRONMENT_SET_CONTROLLER_INFO, controller_info);
}

bool setInputDescriptors()
{
    // key or button descriptors in fact
    static retro_input_descriptor input_descriptors[] = {
        {0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP, "Joypad Up"},
        {0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN, "Joypad Down"},
        {0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT, "Joypad Left"},
        {0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "Joypad Right"},
        {0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A, "Joypad Button 1"},
        {0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B, "Joypad Button 2"},
        {1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP, "Joypad Up"},
        {1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN, "Joypad Down"},
        {1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT, "Joypad Left"},
        {1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "Joypad Right"},
        {1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A, "Joypad Button 1"},
        {1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B, "Joypad Button 2"},
        {}};

    return retro.callbacks.environment(RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS, input_descriptors);
}

RETRO_API auto retro_load_game_special(unsigned game_type, const retro_game_info *game_info, size_t num_info) -> bool
{
    Logger::get()->debug("RETRO_API callback: retro_load_game_special");
    return false;
}

RETRO_API void retro_unload_game()
{
    Logger::get()->debug("RETRO_API callback: retro_unload_game");
    retroPy.reset();
}

RETRO_API void retro_reset()
{
    Logger::get()->debug("RETRO_API callback: retro_reset");
    retroPy->reset();
}

RETRO_API auto retro_api_version() -> unsigned
{
    Logger::get()->debug("RETRO_API callback: retro_api_version");
    return RETRO_API_VERSION;
}

RETRO_API auto retro_get_region(void) -> unsigned
{
    Logger::get()->debug("RETRO_API callback: retro_get_region");
    return RETRO_REGION_PAL;
}

RETRO_API void retro_set_video_refresh(retro_video_refresh_t video_refresh)
{
    Logger::get()->debug("RETRO_API callback: retro_set_video_refresh");
    retro.callbacks.video_refresh = video_refresh;
}

RETRO_API void retro_set_audio_sample(retro_audio_sample_t audio_sample)
{
    Logger::get()->debug("RETRO_API callback: retro_set_audio_sample");
}

RETRO_API void retro_set_audio_sample_batch(retro_audio_sample_batch_t audio_sample_batch)
{
    Logger::get()->debug("RETRO_API callback: retro_set_audio_sample_batch");
    retro.callbacks.audio_sample_batch = audio_sample_batch;
}

RETRO_API void retro_set_input_state(retro_input_state_t input_state)
{
    Logger::get()->debug("RETRO_API callback: retro_set_input_state");
    retro.callbacks.input_state = input_state;
}

RETRO_API void retro_set_input_poll(retro_input_poll_t input_poll)
{
    Logger::get()->debug("RETRO_API callback: retro_set_input_poll");
    retro.callbacks.input_poll = input_poll;
}

RETRO_API void retro_get_system_av_info(retro_system_av_info *av_info)
{
    Logger::get()->debug("RETRO_API callback: retro_get_system_av_info");

    int width = retroPy->getWidth();
    int height = retroPy->getHeight();
    av_info->geometry.base_width = width;
    av_info->geometry.base_height = height;
    av_info->geometry.max_width = width;
    av_info->geometry.max_height = height;
    av_info->geometry.aspect_ratio = 0.0;

    av_info->timing.fps = double(retroPy->getFps());
    av_info->timing.sample_rate = 0.0;
}

RETRO_API void retro_set_controller_port_device(unsigned port, unsigned device)
{
    Logger::get()->debug("RETRO_API callback: retro_set_controller_port_device, port=%d device=%d", port, device);

    if (port < retro.controller_port_devices.size())
        retro.controller_port_devices[port] = device;
}

RETRO_API void retro_run()
{
    retro.callbacks.input_poll();

    for (size_t i = 0; i < retro.controller_port_devices.size(); ++i)
    {
        if (retro.controller_port_devices[i] == RETRO_DEVICE_JOYPAD)
            pollJoypad(i);
    }

    retroPy->nextFrame(retro.callbacks.video_refresh);
}

void pollJoypad(unsigned port)
{
    static constexpr unsigned buttons[] = {
        RETRO_DEVICE_ID_JOYPAD_UP,
        RETRO_DEVICE_ID_JOYPAD_DOWN,
        RETRO_DEVICE_ID_JOYPAD_LEFT,
        RETRO_DEVICE_ID_JOYPAD_RIGHT,
        RETRO_DEVICE_ID_JOYPAD_SELECT,
        RETRO_DEVICE_ID_JOYPAD_START,
        RETRO_DEVICE_ID_JOYPAD_A,
        RETRO_DEVICE_ID_JOYPAD_B,
        RETRO_DEVICE_ID_JOYPAD_X,
        RETRO_DEVICE_ID_JOYPAD_Y,
        RETRO_DEVICE_ID_JOYPAD_L,
        RETRO_DEVICE_ID_JOYPAD_R,
        RETRO_DEVICE_ID_JOYPAD_L2,
        RETRO_DEVICE_ID_JOYPAD_R2,
        RETRO_DEVICE_ID_JOYPAD_L3,
        RETRO_DEVICE_ID_JOYPAD_R3};

    for (unsigned button : buttons)
    {
        bool down = retro.callbacks.input_state(port, RETRO_DEVICE_JOYPAD, 0, button);
        retroPy->joypadEvent(port, button, down);
    }
}

RETRO_CALLCONV void retro_keyboard_event(bool down, unsigned keycode, uint32_t character, uint16_t key_modifiers)
{
    retroPy->keyboardEvent(keycode, down, character, key_modifiers);
}

RETRO_API size_t retro_serialize_size()
{
    Logger::get()->debug("RETRO_API callback: retro_serialize_size");
    return 0;
}

RETRO_API bool retro_serialize(void *data, size_t size)
{
    Logger::get()->debug("RETRO_API callback: retro_serialize");
    return false;
}

RETRO_API bool retro_unserialize(const void *data, size_t size)
{
    Logger::get()->debug("RETRO_API callback: retro_unserialize");
    return false;
}

RETRO_API void retro_cheat_reset()
{
    Logger::get()->debug("RETRO_API callback: retro_cheat_reset");
}

RETRO_API void retro_cheat_set(unsigned index, bool enabled, const char *code)
{
    Logger::get()->debug("RETRO_API callback: retro_cheat_set");
}

RETRO_API void *retro_get_memory_data(unsigned id)
{
    Logger::get()->debug("RETRO_API callback: retro_get_memory_data");
    return nullptr;
}

RETRO_API size_t retro_get_memory_size(unsigned id)
{
    Logger::get()->debug("RETRO_API callback: retro_get_memory_size");
    return 0;
}
