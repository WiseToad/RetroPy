#ifndef LOGGER_H
#define LOGGER_H

#include <memory>
#include <string>

enum class LogLevel
{
    DEBUG,
    INFO,
    WARN,
    ERROR
};

class LogAppender
{
public:
    virtual ~LogAppender() = default;
    virtual void msg(LogLevel level, const std::string &msg) = 0;
};

class StdLogAppender : public LogAppender
{
public:
    void msg(LogLevel level, const std::string &msg) override;
};

class Logger final
{
public:
    static Logger *get()
    {
        static Logger logger;
        return &logger;
    }

    Logger(const Logger &) = delete;
    Logger &operator=(const Logger &) = delete;

    void setAppender(std::unique_ptr<LogAppender> appender);
    void resetAppender();

    template <typename... Args>
    void debug(const std::string &text, Args &&...args)
    {
        msg(LogLevel::DEBUG, text, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void info(const std::string &text, Args &&...args)
    {
        msg(LogLevel::INFO, text, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void warn(const std::string &text, Args &&...args)
    {
        msg(LogLevel::WARN, text, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void error(const std::string &text, Args &&...args)
    {
        msg(LogLevel::ERROR, text, std::forward<Args>(args)...);
    }

    void msg(LogLevel level, const std::string &text, ...);

private:
    Logger();
    ~Logger() = default;

    std::unique_ptr<LogAppender> appender;
};

#endif // LOGGER_H
