#include "retrolog.h"

RetroLogAppender::RetroLogAppender(retro_log_printf_t log_printf)
    : log_printf(log_printf)
{
}

void RetroLogAppender::msg(LogLevel level, const std::string &msg)
{
    retro_log_level log_level = retro_log_level::RETRO_LOG_INFO;
    switch (level)
    {
    case LogLevel::DEBUG:
        log_level = retro_log_level::RETRO_LOG_DEBUG;
        break;
    case LogLevel::INFO:
        log_level = retro_log_level::RETRO_LOG_INFO;
        break;
    case LogLevel::WARN:
        log_level = retro_log_level::RETRO_LOG_WARN;
        break;
    case LogLevel::ERROR:
        log_level = retro_log_level::RETRO_LOG_ERROR;
        break;
    }
    log_printf(log_level, "%s\n", msg.data());
}
