#ifndef RETROLOG_H
#define RETROLOG_H

#include "libretro.h"
#include "logger.h"
#include <string>

class RetroLogAppender : public LogAppender
{
public:
    RetroLogAppender(retro_log_printf_t log_printf);

    void msg(LogLevel level, const std::string &msg) override;

private:
    retro_log_printf_t log_printf;
};

# endif // RETROLOG_H
