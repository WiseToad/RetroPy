#include "logger.h"
#include <cstdarg>
#include <cstring>
#include <iostream>

// class StdLogAppender

void StdLogAppender::msg(LogLevel level, const std::string &msg)
{
    switch (level)
    {
    case LogLevel::DEBUG:
        std::cerr << "DEBUG ";
        break;
    case LogLevel::INFO:
        std::cerr << "INFO  ";
        break;
    case LogLevel::WARN:
        std::cerr << "WARN  ";
        break;
    case LogLevel::ERROR:
        std::cerr << "ERROR ";
        break;
    }
    std::cerr << msg << std::endl;
}

// class Logger

Logger::Logger()
    : appender(std::make_unique<StdLogAppender>())
{
}

void Logger::setAppender(std::unique_ptr<LogAppender> appender)
{
    this->appender = appender ? std::move(appender) : std::make_unique<StdLogAppender>();
}

void Logger::resetAppender()
{
    this->appender = std::make_unique<StdLogAppender>();
}

void Logger::msg(LogLevel level, const std::string &text, ...)
{
    constexpr size_t bufSize = 4096;
    char buf[bufSize];

    va_list args;
    va_start(args, text);
    size_t n = vsnprintf(buf, bufSize, text.data(), args);
    if (n >= bufSize - 1)
    {
        strcat(buf + bufSize - 4, "...");
    }
    va_end(args);

    appender->msg(level, std::string(buf));
}
