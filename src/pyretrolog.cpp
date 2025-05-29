#include "pyretrolog.h"
#include "logger.h"

PyTypeObject PyRetroLog::pyType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "RetroLog",
    .tp_basicsize = sizeof(PyRetroLog),
    .tp_itemsize = 0,
    .tp_dealloc = (destructor)pyDealloc,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_methods = pyMethods,
    .tp_init = (initproc)pyInit,
    .tp_new = pyNew
};

PyMethodDef PyRetroLog::pyMethods[] = {
    {"write", (PyCFunction)pyWrite, METH_O},
    {"flush", (PyCFunction)pyFlush, METH_NOARGS},
    {NULL}};

PyObject *PyRetroLog::pyNew(PyTypeObject *pyType, PyObject *pyArgs, PyObject *pyKwargs)
{
    auto pySelf = py_unique_ptr(pyType->tp_alloc(pyType, 0));
    if (!pySelf)
        return nullptr;

    ((PyRetroLog *)pySelf.get())->logLevel = (int)LogLevel::INFO;
    ((PyRetroLog *)pySelf.get())->msg = new std::string();

    return pySelf.release();
}

int PyRetroLog::pyInit(PyRetroLog *pySelf, PyObject *pyArgs, PyObject *pyKwargs)
{
    static const char *kwNames[] = {"", nullptr};

    if (!PyArg_ParseTupleAndKeywords(pyArgs, pyKwargs, "|i", (char **)kwNames, &pySelf->logLevel))
        return -1;

    if (pySelf->logLevel < (int)LogLevel::DEBUG || pySelf->logLevel > (int)LogLevel::ERROR)
        pySelf->logLevel = (int)LogLevel::INFO;

    return 0;
}

void PyRetroLog::pyDealloc(PyRetroLog *pySelf)
{
    delete pySelf->msg;
    Py_TYPE(pySelf)->tp_free((PyObject *)pySelf);
}

PyObject *PyRetroLog::pyWrite(PyRetroLog *pySelf, PyObject *pyArg)
{
    const char *str;
    size_t len;
    if (!PyArg_Parse(pyArg, "s#:write", &str, &len))
        return nullptr;

    size_t wrote = 0;
    while (true)
    {
        auto p = (const char *)memchr(str, '\n', len);
        size_t n = p ? p - str : len;

        pySelf->msg->append(str, n);
        wrote += n;

        if (!p)
            break;

        Logger::get()->msg((LogLevel)pySelf->logLevel, "%s", pySelf->msg->data());
        pySelf->msg->clear();

        str = p + 1;
        len -= (n + 1);
    }

    return PyLong_FromLong(wrote);
}

PyObject *PyRetroLog::pyFlush(PyRetroLog *pySelf, PyObject *Py_UNUSED(ignored))
{
    if (!pySelf->msg->empty())
        Logger::get()->msg((LogLevel)pySelf->logLevel, "%s", pySelf->msg->data());

    return Py_BuildValue(""); // None
}
