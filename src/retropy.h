#ifndef RETROPY_H
#define RETROPY_H

#include "py.h"
#include "config.h"

using VideoRefreshFunc = void (*)(const void *buf, unsigned width, unsigned height, size_t pitch);

class RetroPy final
{
public:
    RetroPy();
    ~RetroPy();

    bool init(const std::string &systemDir);
    void deinit();

    bool open(const std::string &fileName);
    void close();
    void reset();

    unsigned getWidth() const { return width; }
    unsigned getHeight() const { return height; }
    unsigned getFps() const { return fps; }

    void nextFrame(VideoRefreshFunc videoRefreshCallback);

    void joypadEvent(unsigned port, unsigned button, bool pressed);
    void keyboardEvent(unsigned key, bool pressed, uint32_t character, uint16_t modifiers);

private:
    Config config;
    void *pythonLib;

    bool valid;

    unsigned width, height;
    unsigned fps;

    py_unique_ptr pyNextFrameFunc;
    py_unique_ptr pyJoypadEventFunc;
    py_unique_ptr pyKeyboardEventFunc;
    py_unique_ptr pyResetFunc;

    bool loadConfig(const std::string &fileName);
    bool loadPythonLib();
    void redirectPyStd();

    py_unique_ptr loadPyCore(const std::string &fileName);
    void resetPyCore();
};

#endif // RETROPY_H
