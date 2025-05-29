#include "retropy.h"
#include "pyretrolog.h"
#include "logger.h"

#include <dlfcn.h>
#include <filesystem>
#include <fstream>

RetroPy::RetroPy()
    : pythonLib(nullptr), valid(false), width(0), height(0), fps(0)
{
}

RetroPy::~RetroPy()
{
    deinit();
}

bool RetroPy::init(const std::string &systemDir)
{
    deinit();

    if (!loadConfig(systemDir + "/RetroPy.conf"))
        return false;

    if (!loadPythonLib())
        return false;

    Py_InitializeEx(0);
    redirectPyStd();

    return true;
}

void RetroPy::deinit()
{
    close();

    if (pythonLib)
    {
        if (Py_IsInitialized())
            Py_Finalize();

        dlclose(pythonLib);
        pythonLib = nullptr;
    }
}

bool RetroPy::loadConfig(const std::string &fileName)
{
    std::ifstream configFile(fileName);
    if (!configFile.is_open())
    {
        Logger::get()->warn("Config file missing: %s", fileName.data());
        return false;
    }

    if (!config.load(configFile))
    {
        Logger::get()->error("Invalid config: %s", fileName.data());
        return false;
    }

    return true;
}

bool RetroPy::loadPythonLib()
{
    std::string libName = config.get("pythonLib");
    if (libName.empty())
    {
        Logger::get()->error("Config parameter missing: %s", "pythonLib");
        return false;
    }

    pythonLib = dlopen(libName.data(), RTLD_NOW | RTLD_GLOBAL);
    if (!pythonLib)
    {
        Logger::get()->error("Failed to load Python library %s, error: %s", libName.data(), dlerror());
        return false;
    }

    return true;
}

void RetroPy::redirectPyStd()
{
    if (PyType_Ready(&PyRetroLog::pyType) < 0)
    {
        Logger::get()->warn("Failed to initialize RetroLog type, Python's stdout/stderr won't redirect to libretro log");
        PyErr_Print();
        return;
    }

    auto pyOut = py_unique_ptr(PyObject_CallFunction((PyObject *)&PyRetroLog::pyType, "(i)", (int)LogLevel::INFO));
    if (!pyOut || PySys_SetObject("stdout", pyOut.get()) < 0)
    {
        Logger::get()->warn("Failed to redirect Python's stdout to libretro log");
        PyErr_Print();
    }

    auto pyErr = py_unique_ptr(PyObject_CallFunction((PyObject *)&PyRetroLog::pyType, "(i)", (int)LogLevel::ERROR));
    if (!pyErr || PySys_SetObject("stderr", pyErr.get()) < 0)
    {
        Logger::get()->warn("Failed to redirect Python's stderr to libretro log");
        PyErr_Print();
    }
}

bool RetroPy::open(const std::string &fileName)
{
    close();

    py_unique_ptr pyCore = loadPyCore(fileName);
    if (!pyCore)
        return false;

    py_unique_ptr pyValue;
    pyValue = PyErr_Occurred() ? nullptr : py_unique_ptr(PyObject_GetAttrString(pyCore.get(), "width"));
    width = pyValue ? PyLong_AsUnsignedLong(pyValue.get()) : -1;
    pyValue = PyErr_Occurred() ? nullptr : py_unique_ptr(PyObject_GetAttrString(pyCore.get(), "height"));
    height = pyValue ? PyLong_AsUnsignedLong(pyValue.get()) : -1;
    pyValue = PyErr_Occurred() ? nullptr : py_unique_ptr(PyObject_GetAttrString(pyCore.get(), "fps"));
    fps = pyValue ? PyLong_AsUnsignedLong(pyValue.get()) : -1;
    if (PyErr_Occurred())
    {
        Logger::get()->error("Failed to retrieve width, height or fps from Python core");
        PyErr_Print();
        return false;
    }

    pyNextFrameFunc = py_unique_ptr(PyObject_GetAttrString(pyCore.get(), "nextFrame"));
    if (!pyNextFrameFunc)
    {
        Logger::get()->error("No frame refresh callback defined in Python core");
        PyErr_Print();
        return false;
    }

    pyJoypadEventFunc = py_unique_ptr(PyObject_GetAttrString(pyCore.get(), "joypadEvent"));
    pyKeyboardEventFunc = py_unique_ptr(PyObject_GetAttrString(pyCore.get(), "keyboardEvent"));
    if (!pyJoypadEventFunc && !pyKeyboardEventFunc)
    {
        Logger::get()->warn("No callback(s) for joypad and keyboard events defined in Python core");
        PyErr_Clear();
    }

    pyResetFunc = py_unique_ptr(PyObject_GetAttrString(pyCore.get(), "reset"));
    if (!pyResetFunc)
        PyErr_Clear();

    valid = true;
    return true;
}

void RetroPy::close()
{
    valid = false;

    pyResetFunc.reset();
    pyKeyboardEventFunc.reset();
    pyJoypadEventFunc.reset();
    pyNextFrameFunc.reset();

    width = height = 0;
    fps = 0;
}

void RetroPy::reset()
{
    resetPyCore();
}

py_unique_ptr RetroPy::loadPyCore(const std::string &fileName)
{
    std::filesystem::path filePath = absolute(std::filesystem::path(fileName));
    std::string fileDir = filePath.parent_path().string();
    std::string moduleName = filePath.stem().string();

    auto pyFileDir = py_unique_ptr(PyUnicode_DecodeFSDefault(fileDir.data()));
    if (!pyFileDir || PyList_Append(PySys_GetObject("path"), pyFileDir.get()) < 0)
    {
        Logger::get()->error("Failed to add Python module dir to sys.path: %s", fileDir.data());
        PyErr_Print();
        return nullptr;
    }

    auto pyModuleName = py_unique_ptr(PyUnicode_DecodeFSDefault(moduleName.data()));
    auto pyModule = pyModuleName ? py_unique_ptr(PyImport_Import(pyModuleName.get())) : nullptr;
    if (!pyModule)
    {
        Logger::get()->error("Failed to import Python module: %s", moduleName.data());
        PyErr_Print();
        return nullptr;
    }

    auto pyArgv = py_unique_ptr(PyList_New(1));
    if (!pyArgv || PyList_SetItem(pyArgv.get(), 0, PyUnicode_DecodeFSDefault(fileName.data())) < 0 ||
        PySys_SetObject("argv", pyArgv.get()) < 0)
    {
        Logger::get()->error("Failed to set sys.argv for Python module");
        PyErr_Print();
        return nullptr;
    }

    auto pyCore = py_unique_ptr(PyObject_CallMethod(pyModule.get(), "getCore", nullptr));
    if (!pyCore)
    {
        Logger::get()->error("Failed to obtain instance of Python core");
        PyErr_Print();
        return nullptr;
    }

    return pyCore;
}

void RetroPy::resetPyCore()
{
    if (!pyResetFunc)
    {
        Logger::get()->warn("Reset not implemented in Python core");
        return;
    }

    auto pyResult = py_unique_ptr(PyObject_CallNoArgs(pyResetFunc.get()));
    if (!pyResult)
    {
        Logger::get()->error("Failed to reset from Python core");
        PyErr_Print();
    }
}

void RetroPy::nextFrame(VideoRefreshFunc videoRefreshCallback)
{
    if (!valid)
    {
        (*videoRefreshCallback)(nullptr, 0, 0, 0);
        return;
    }

    Py_buffer buffer;
    auto pyBuffer = py_unique_ptr(PyObject_CallObject(pyNextFrameFunc.get(), nullptr));
    if (!pyBuffer || PyObject_GetBuffer(pyBuffer.get(), &buffer, PyBUF_SIMPLE) < 0)
    {
        Logger::get()->error("Frame rendering error in Python core, aborting further attempts");
        PyErr_Print();
        valid = false;
        return;
    }

    (*videoRefreshCallback)(buffer.buf, width, height, width * sizeof(uint32_t));
    PyBuffer_Release(&buffer);
}

void RetroPy::joypadEvent(unsigned port, unsigned button, bool pressed)
{
    if (!pyJoypadEventFunc)
        return;

    auto pyResult = py_unique_ptr(PyObject_CallFunction(
        pyJoypadEventFunc.get(), "IIN",
        port, button, PyBool_FromLong(pressed ? 1 : 0)));
    if (!pyResult)
    {
        Logger::get()->error("Error handling joypad event in Python core");
        PyErr_Print();
    }
}

void RetroPy::keyboardEvent(unsigned key, bool pressed, uint32_t character, uint16_t modifiers)
{
    if (!pyKeyboardEventFunc)
        return;

    auto pyResult = py_unique_ptr(PyObject_CallFunction(
        pyKeyboardEventFunc.get(), "INII",
        key, PyBool_FromLong(pressed ? 1 : 0), 0, 0));
    if (!pyResult)
    {
        Logger::get()->error("Error handling keyboard event in Python core");
        PyErr_Print();
    }
}
