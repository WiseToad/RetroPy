#ifndef PY_RETROLOG_H
#define PY_RETROLOG_H

#include "py.h"

struct PyRetroLog final
{
    PyObject_HEAD;
    int logLevel;
    std::string *msg;

    static PyTypeObject pyType;
    static PyMethodDef pyMethods[];

    static PyObject *pyNew(PyTypeObject *pyType, PyObject *pyArgs, PyObject *pyKwargs);
    static int pyInit(PyRetroLog *pySelf, PyObject *pyArgs, PyObject *pyKwargs);
    static void pyDealloc(PyRetroLog *pySelf);

    static PyObject *pyWrite(PyRetroLog *pySelf, PyObject *pyArg);
    static PyObject *pyFlush(PyRetroLog *pySelf, PyObject *Py_UNUSED(ignored));
};

#endif // PY_RETROLOG_H
