#ifndef PY_H
#define PY_H

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <memory>
#include <string>

struct PyObjectDeleter final
{
    void operator()(PyObject *obj)
    {
        Py_XDECREF(obj);
    }
};

using py_unique_ptr = std::unique_ptr<PyObject, PyObjectDeleter>;

#endif // PY_H
