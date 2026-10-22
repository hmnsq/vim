/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved    by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */
/*
 * Python extensions by Paul Moore.
 * Changes for Unix by David Leonard.
 *
 * This consists of four parts:
 * 1. Python interpreter main program
 * 2. Python output stream: writes output via [e]msg().
 * 3. Implementation of the Vim module for Python
 * 4. Utility functions for handling the interface between Vim and Python.
 */

/*
 * Roland Puntaier 2009/sept/16:
 * Adaptations to support both python3.x and python2.x
 */

// uncomment this if used with the debug version of python
// #define Py_DEBUG
// Note: most of time you can add -DPy_DEBUG to CFLAGS in place of uncommenting
// uncomment this if used with the debug version of python, but without its
// allocator
// #define Py_DEBUG_NO_PYMALLOC

#include "vim.h"

#include <limits.h>

#만약 defined(MSWIN) && defined(HAVE_FCNTL_H)
# undef HAVE_FCNTL_H
#endif

#ifdef _DEBUG
# undef _DEBUG
#endif

#ifdef F_BLANK
# undef F_BLANK
#endif

#ifdef HAVE_DUP
# undef HAVE_DUP
#endif
#ifdef HAVE_STRFTIME
# undef HAVE_STRFTIME
#endif
#ifdef HAVE_STRING_H
# undef HAVE_STRING_H
#endif
#ifdef HAVE_PUTENV
# undef HAVE_PUTENV
#endif
#ifdef _POSIX_C_SOURCE  // defined in feature.h
# undef _POSIX_C_SOURCE
#endif
#ifdef _XOPEN_SOURCE
# undef _XOPEN_SOURCE	// pyconfig.h defines it as well.
#endif

#define PY_SSIZE_T_CLEAN

#ifdef Py_LIMITED_API
# define USE_LIMITED_API // Using Python 3 limited ABI
#endif

#include <Python.h>

#undef main // Defined in python.h - aargh
#undef HAVE_FCNTL_H // Clash with os_win32.h

// The "surrogateescape" error handler is new in Python 3.1
#만약 PY_VERSION_HEX >= 0x030100f0
# define CODEC_ERROR_HANDLER "surrogateescape"
#아니면
# define CODEC_ERROR_HANDLER NULL
#endif

// Suppress Python 3.11 depreciations to see useful warnings
#ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

// Python 3 does not support CObjects, always use Capsules
#define PY_USE_CAPSULE

#define ERRORS_DECODE_ARG CODEC_ERROR_HANDLER
#define ERRORS_ENCODE_ARG ERRORS_DECODE_ARG

#define PyInt Py_ssize_t
#ifndef PyString_Check
# define PyString_Check(obj) PyUnicode_Check(obj)
#endif
#define PyString_FromString(repr) \
    PyUnicode_Decode(repr, STRLEN(repr), ENC_OPT, ERRORS_DECODE_ARG)
#define PyString_FromFormat PyUnicode_FromFormat
#ifdef PyUnicode_FromFormat
# define Py_UNICODE_USE_UCS_FUNCTIONS
#endif
#ifndef PyInt_Check
# define PyInt_Check(obj) PyLong_Check(obj)
#endif
#define PyInt_FromLong(i) PyLong_FromLong(i)
#define PyInt_AsLong(obj) PyLong_AsLong(obj)
#define Py_ssize_t_fmt "n"
#define Py_bytes_fmt "y"

#define PyIntArgFunc	ssizeargfunc
#define PyIntObjArgProc	ssizeobjargproc

/*
 * PySlice_GetIndicesEx(): first argument type changed from PySliceObject
 * to PyObject in Python 3.2 or later.
 */
#만약 PY_VERSION_HEX >= 0x030200f0
typedef PyObject PySliceObject_T;
#아니면
typedef PySliceObject PySliceObject_T;
#endif

#ifndef MSWIN
# define HINSTANCE void *
#endif
#만약 defined(DYNAMIC_PYTHON3) || defined(MSWIN)
static HINSTANCE hinstPy3 = 0; // Instance of python.dll
#endif

#만약 defined(DYNAMIC_PYTHON3)

# ifdef MSWIN
#  define load_dll vimLoadLib
#  define close_dll FreeLibrary
#  define symbol_from_dll GetProcAddress
#  define load_dll_error GetWin32Error
# 아니면
#  include <dlfcn.h>
#  define FARPROC void*
#  만약 defined(PY_NO_RTLD_GLOBAL) && defined(PY3_NO_RTLD_GLOBAL)
#   define load_dll(n) dlopen((n), RTLD_LAZY)
#  아니면
#   define load_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#  endif
#  define close_dll dlclose
#  define symbol_from_dll dlsym
#  define load_dll_error dlerror
# endif
/*
 * Wrapper defines
 */
# undef PyArg_Parse
# define PyArg_Parse py3_PyArg_Parse
# undef PyArg_ParseTuple
# define PyArg_ParseTuple py3_PyArg_ParseTuple
# define PyMem_Free py3_PyMem_Free
# define PyMem_Malloc py3_PyMem_Malloc
# define PyDict_SetItemString py3_PyDict_SetItemString
# define PyErr_BadArgument py3_PyErr_BadArgument
# define PyErr_Clear py3_PyErr_Clear
# define PyErr_Format py3_PyErr_Format
# define PyErr_PrintEx py3_PyErr_PrintEx
# define PyErr_NoMemory py3_PyErr_NoMemory
# define PyErr_Occurred py3_PyErr_Occurred
# define PyErr_SetNone py3_PyErr_SetNone
# define PyErr_SetString py3_PyErr_SetString
# define PyErr_SetObject py3_PyErr_SetObject
# define PyErr_ExceptionMatches py3_PyErr_ExceptionMatches
# define PyEval_InitThreads py3_PyEval_InitThreads
# define PyEval_RestoreThread py3_PyEval_RestoreThread
# define PyEval_SaveThread py3_PyEval_SaveThread
# define PyGILState_Ensure py3_PyGILState_Ensure
# define PyGILState_Release py3_PyGILState_Release
# define PyLong_AsLong py3_PyLong_AsLong
# define PyLong_FromLong py3_PyLong_FromLong
# define PyList_GetItem py3_PyList_GetItem
# define PyList_Append py3_PyList_Append
# define PyList_Insert py3_PyList_Insert
# define PyList_New py3_PyList_New
# define PyList_SetItem py3_PyList_SetItem
# define PyList_Size py3_PyList_Size
# define PyTuple_New py3_PyTuple_New
# define PyTuple_GetItem py3_PyTuple_GetItem
# define PyTuple_SetItem py3_PyTuple_SetItem
# define PyTuple_Size py3_PyTuple_Size
# define PySequence_Check py3_PySequence_Check
# define PySequence_Size py3_PySequence_Size
# define PySequence_GetItem py3_PySequence_GetItem
# define PySequence_Fast py3_PySequence_Fast
# 만약 PY_VERSION_HEX >= 0x030601f0
#  define PySlice_AdjustIndices py3_PySlice_AdjustIndices
#  define PySlice_Unpack py3_PySlice_Unpack
# endif
# undef PySlice_GetIndicesEx
# define PySlice_GetIndicesEx py3_PySlice_GetIndicesEx
# define PyImport_ImportModule py3_PyImport_ImportModule
# define PyObject_Init py3__PyObject_Init
# define PyDict_New py3_PyDict_New
# define PyDict_GetItemString py3_PyDict_GetItemString
# define PyDict_Next py3_PyDict_Next
# define PyMapping_Check py3_PyMapping_Check
# ifndef PyMapping_Keys
#  define PyMapping_Keys py3_PyMapping_Keys
# endif
# 만약 (defined(USE_LIMITED_API) && Py_LIMITED_API >= 0x03080000) || \
       (!defined(USE_LIMITED_API) && PY_VERSION_HEX >= 0x03080000)
#  undef PyIter_Check
#  define PyIter_Check py3_PyIter_Check
# endif
# define PyIter_Next py3_PyIter_Next
# define PyObject_GetIter py3_PyObject_GetIter
# define PyObject_Repr py3_PyObject_Repr
# define PyObject_GetItem py3_PyObject_GetItem
# define PyObject_IsTrue py3_PyObject_IsTrue
# define PyModule_GetDict py3_PyModule_GetDict
# 만약 defined(USE_LIMITED_API) || PY_VERSION_HEX >= 0x03080000
#  define Py_IncRef py3_Py_IncRef
#  define Py_DecRef py3_Py_DecRef
# endif
# ifdef USE_LIMITED_API
#  define Py_CompileString py3_Py_CompileString
#  define PyEval_EvalCode py3_PyEval_EvalCode
# 아니면
#  undef PyRun_SimpleString
#  define PyRun_SimpleString py3_PyRun_SimpleString
#  undef PyRun_String
#  define PyRun_String py3_PyRun_String
# endif
# define PyObject_GetAttrString py3_PyObject_GetAttrString
# define PyObject_HasAttrString py3_PyObject_HasAttrString
# define PyObject_SetAttrString py3_PyObject_SetAttrString
# define PyObject_CallFunctionObjArgs py3_PyObject_CallFunctionObjArgs
# 만약 PY_VERSION_HEX >= 0x030d0000
#  define PyObject_CallFunction py3_PyObject_CallFunction
# 아니면
#  define _PyObject_CallFunction_SizeT py3__PyObject_CallFunction_SizeT
# endif
# define PyObject_Call py3_PyObject_Call
# define PyEval_GetLocals py3_PyEval_GetLocals
# define PyEval_GetGlobals py3_PyEval_GetGlobals
# define PySys_SetObject py3_PySys_SetObject
# define PySys_GetObject py3_PySys_GetObject
# define PySys_SetArgv py3_PySys_SetArgv
# define PyType_Ready py3_PyType_Ready
# 만약 PY_VERSION_HEX >= 0x03040000
#  define PyType_GetFlags py3_PyType_GetFlags
# endif
# undef Py_BuildValue
# define Py_BuildValue py3_Py_BuildValue
# define Py_SetPythonHome py3_Py_SetPythonHome
# define Py_Initialize py3_Py_Initialize
# define Py_Finalize py3_Py_Finalize
# define Py_IsInitialized py3_Py_IsInitialized
# define _Py_NoneStruct (*py3__Py_NoneStruct)
# define _Py_FalseStruct (*py3__Py_FalseStruct)
# define _Py_TrueStruct (*py3__Py_TrueStruct)
# 만약 !defined(USE_LIMITED_API) && PY_VERSION_HEX < 0x030D0000
// Private symbol that used to be required as part of PyIter_Check.
#  define _PyObject_NextNotImplemented (*py3__PyObject_NextNotImplemented)
# endif
# define PyModule_AddObject py3_PyModule_AddObject
# define PyImport_AppendInittab py3_PyImport_AppendInittab
# define PyImport_AddModule py3_PyImport_AddModule
# ifdef USE_LIMITED_API
#  만약 Py_LIMITED_API >= 0x030a0000
#   define PyUnicode_AsUTF8AndSize py3_PyUnicode_AsUTF8AndSize
#  endif
# 아니면
#  만약 PY_VERSION_HEX >= 0x030300f0
#   define PyUnicode_AsUTF8AndSize py3_PyUnicode_AsUTF8AndSize
#  아니면
#   define _PyUnicode_AsString py3__PyUnicode_AsString
#  endif
# endif
# undef PyUnicode_CompareWithASCIIString
# define PyUnicode_CompareWithASCIIString py3_PyUnicode_CompareWithASCIIString
# undef PyUnicode_AsEncodedString
# define PyUnicode_AsEncodedString py3_PyUnicode_AsEncodedString
# undef PyUnicode_AsUTF8String
# define PyUnicode_AsUTF8String py3_PyUnicode_AsUTF8String
# undef PyBytes_AsString
# define PyBytes_AsString py3_PyBytes_AsString
# ifndef PyBytes_AsStringAndSize
#  define PyBytes_AsStringAndSize py3_PyBytes_AsStringAndSize
# endif
# undef PyBytes_FromString
# define PyBytes_FromString py3_PyBytes_FromString
# undef PyBytes_FromStringAndSize
# define PyBytes_FromStringAndSize py3_PyBytes_FromStringAndSize
# define PyFloat_FromDouble py3_PyFloat_FromDouble
# define PyFloat_AsDouble py3_PyFloat_AsDouble
# define PyObject_GenericGetAttr py3_PyObject_GenericGetAttr
# define PyType_Type (*py3_PyType_Type)
# ifndef USE_LIMITED_API
#  define PyStdPrinter_Type (*py3_PyStdPrinter_Type)
# endif
# define PySlice_Type (*py3_PySlice_Type)
# define PyFloat_Type (*py3_PyFloat_Type)
# define PyNumber_Check (*py3_PyNumber_Check)
# define PyNumber_Long (*py3_PyNumber_Long)
# define PyBool_Type (*py3_PyBool_Type)
# define PyErr_NewException py3_PyErr_NewException
# ifdef Py_DEBUG
#  define _Py_NegativeRefcount py3__Py_NegativeRefcount
#  define _Py_RefTotal (*py3__Py_RefTotal)
#  define PyModule_Create2TraceRefs py3_PyModule_Create2TraceRefs
# 아니면
#  define PyModule_Create2 py3_PyModule_Create2
# endif
# 만약 defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
#  define _PyObject_DebugMalloc py3__PyObject_DebugMalloc
#  define _PyObject_DebugFree py3__PyObject_DebugFree
# 아니면
#  define PyObject_Malloc py3_PyObject_Malloc
#  define PyObject_Free py3_PyObject_Free
# endif
# define _PyObject_GC_New py3__PyObject_GC_New
# define PyObject_GC_Del py3_PyObject_GC_Del
# define PyObject_GC_UnTrack py3_PyObject_GC_UnTrack
# define PyType_GenericAlloc py3_PyType_GenericAlloc
# define PyType_GenericNew py3_PyType_GenericNew
# undef PyUnicode_FromString
# define PyUnicode_FromString py3_PyUnicode_FromString
# ifdef Py_UNICODE_USE_UCS_FUNCTIONS
#  ifdef Py_UNICODE_WIDE
#   define PyUnicodeUCS4_FromFormat py3_PyUnicodeUCS4_FromFormat
#  아니면
#   define PyUnicodeUCS2_FromFormat py3_PyUnicodeUCS2_FromFormat
#  endif
# 아니면
#  define PyUnicode_FromFormat py3_PyUnicode_FromFormat
# endif
# undef PyUnicode_Decode
# define PyUnicode_Decode py3_PyUnicode_Decode
# define PyType_IsSubtype py3_PyType_IsSubtype
# define PyCapsule_New py3_PyCapsule_New
# define PyCapsule_GetPointer py3_PyCapsule_GetPointer
# ifdef USE_LIMITED_API
#  define PyType_GetSlot py3_PyType_GetSlot
#  define PyType_FromSpec py3_PyType_FromSpec
# endif

# 만약 defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
#  undef PyObject_NEW
#  define PyObject_NEW(type, typeobj) \
( (type *) PyObject_Init( \
	(PyObject *) _PyObject_DebugMalloc( _PyObject_SIZE(typeobj) ), (typeobj)) )
# elif PY_VERSION_HEX >= 0x030900b0
#  undef PyObject_NEW
#  define PyObject_NEW(type, typeobj) \
	((type *)py3__PyObject_New(typeobj))
# endif

/*
 * Pointers for dynamic link
 */
static 정수 (*py3_PySys_SetArgv)(정수, wchar_t **);
static void (*py3_Py_SetPythonHome)(wchar_t *home);
static void (*py3_Py_Initialize)(void);
static PyObject* (*py3_PyList_New)(Py_ssize_t size);
static PyGILState_STATE (*py3_PyGILState_Ensure)(void);
static void (*py3_PyGILState_Release)(PyGILState_STATE);
static 정수 (*py3_PySys_SetObject)(char *, PyObject *);
static PyObject* (*py3_PySys_GetObject)(char *);
static 정수 (*py3_PyList_Append)(PyObject *, PyObject *);
static 정수 (*py3_PyList_Insert)(PyObject *, 정수, PyObject *);
static Py_ssize_t (*py3_PyList_Size)(PyObject *);
static PyObject* (*py3_PyTuple_New)(Py_ssize_t size);
static 정수 (*py3_PyTuple_SetItem)(PyObject *, Py_ssize_t, PyObject *);
static Py_ssize_t (*py3_PyTuple_Size)(PyObject *);
static 정수 (*py3_PySequence_Check)(PyObject *);
static Py_ssize_t (*py3_PySequence_Size)(PyObject *);
static PyObject* (*py3_PySequence_GetItem)(PyObject *, Py_ssize_t);
static PyObject* (*py3_PySequence_Fast)(PyObject *, const char *);
static PyObject* (*py3_PyTuple_GetItem)(PyObject *, Py_ssize_t);
static 정수 (*py3_PyMapping_Check)(PyObject *);
static PyObject* (*py3_PyMapping_Keys)(PyObject *);
# 만약 PY_VERSION_HEX >= 0x030601f0
static 정수 (*py3_PySlice_AdjustIndices)(Py_ssize_t length,
		     Py_ssize_t *start, Py_ssize_t *stop, Py_ssize_t step);
static 정수 (*py3_PySlice_Unpack)(PyObject *slice,
		     Py_ssize_t *start, Py_ssize_t *stop, Py_ssize_t *step);
# endif
static 정수 (*py3_PySlice_GetIndicesEx)(PySliceObject_T *r, Py_ssize_t length,
		     Py_ssize_t *start, Py_ssize_t *stop, Py_ssize_t *step,
		     Py_ssize_t *slicelen);
static PyObject* (*py3_PyErr_NoMemory)(void);
static void (*py3_Py_Finalize)(void);
static void (*py3_PyErr_SetString)(PyObject *, const char *);
static void (*py3_PyErr_SetObject)(PyObject *, PyObject *);
static 정수 (*py3_PyErr_ExceptionMatches)(PyObject *);
# 만약 defined(USE_LIMITED_API) || PY_VERSION_HEX >= 0x03080000
static void (*py3_Py_IncRef)(PyObject *);
static void (*py3_Py_DecRef)(PyObject *);
# endif
# ifdef USE_LIMITED_API
static PyObject* (*py3_Py_CompileString)(const char *, const char *, 정수);
static PyObject* (*py3_PyEval_EvalCode)(PyObject *co, PyObject *globals, PyObject *locals);
# 아니면
static 정수 (*py3_PyRun_SimpleString)(char *);
static PyObject* (*py3_PyRun_String)(char *, 정수, PyObject *, PyObject *);
# endif
static PyObject* (*py3_PyObject_GetAttrString)(PyObject *, const char *);
static 정수 (*py3_PyObject_HasAttrString)(PyObject *, const char *);
static 정수 (*py3_PyObject_SetAttrString)(PyObject *, const char *, PyObject *);
static PyObject* (*py3_PyObject_CallFunctionObjArgs)(PyObject *, ...);
# 만약 PY_VERSION_HEX >= 0x030d0000
static PyObject* (*py3_PyObject_CallFunction)(PyObject *, char *, ...);
# 아니면
static PyObject* (*py3__PyObject_CallFunction_SizeT)(PyObject *, char *, ...);
# endif
static PyObject* (*py3_PyObject_Call)(PyObject *, PyObject *, PyObject *);
static PyObject* (*py3_PyEval_GetGlobals)(void);
static PyObject* (*py3_PyEval_GetLocals)(void);
static PyObject* (*py3_PyList_GetItem)(PyObject *, Py_ssize_t);
static PyObject* (*py3_PyImport_ImportModule)(const char *);
static PyObject* (*py3_PyImport_AddModule)(const char *);
static 정수 (*py3_PyErr_BadArgument)(void);
static PyObject* (*py3_PyErr_Occurred)(void);
static PyObject* (*py3_PyModule_GetDict)(PyObject *);
static 정수 (*py3_PyList_SetItem)(PyObject *, Py_ssize_t, PyObject *);
static PyObject* (*py3_PyDict_GetItemString)(PyObject *, const char *);
static 정수 (*py3_PyDict_Next)(PyObject *, Py_ssize_t *, PyObject **, PyObject **);
static PyObject* (*py3_PyLong_FromLong)(long);
static PyObject* (*py3_PyDict_New)(void);
# 만약 (defined(USE_LIMITED_API) && Py_LIMITED_API >= 0x03080000) || \
       (!defined(USE_LIMITED_API) && PY_VERSION_HEX >= 0x03080000)
static 정수 (*py3_PyIter_Check)(PyObject *o);
# endif
static PyObject* (*py3_PyIter_Next)(PyObject *);
static PyObject* (*py3_PyObject_GetIter)(PyObject *);
static PyObject* (*py3_PyObject_Repr)(PyObject *);
static PyObject* (*py3_PyObject_GetItem)(PyObject *, PyObject *);
static 정수 (*py3_PyObject_IsTrue)(PyObject *);
static PyObject* (*py3_Py_BuildValue)(char *, ...);
# 만약 PY_VERSION_HEX >= 0x03040000
static 정수 (*py3_PyType_GetFlags)(PyTypeObject *o);
# endif
static 정수 (*py3_PyType_Ready)(PyTypeObject *type);
static 정수 (*py3_PyDict_SetItemString)(PyObject *dp, char *key, PyObject *item);
static PyObject* (*py3_PyUnicode_FromString)(const char *u);
# ifdef Py_UNICODE_USE_UCS_FUNCTIONS
#  ifdef Py_UNICODE_WIDE
static PyObject* (*py3_PyUnicodeUCS4_FromFormat)(const char *u, ...);
#  아니면
static PyObject* (*py3_PyUnicodeUCS2_FromFormat)(const char *u, ...);
#  endif
# 아니면
static PyObject* (*py3_PyUnicode_FromFormat)(const char *u, ...);
# endif
static PyObject* (*py3_PyUnicode_Decode)(const char *u, Py_ssize_t size,
	const char *encoding, const char *errors);
static long (*py3_PyLong_AsLong)(PyObject *);
static void (*py3_PyErr_SetNone)(PyObject *);
static void (*py3_PyEval_InitThreads)(void);
static void(*py3_PyEval_RestoreThread)(PyThreadState *);
static PyThreadState*(*py3_PyEval_SaveThread)(void);
static 정수 (*py3_PyArg_Parse)(PyObject *, char *, ...);
static 정수 (*py3_PyArg_ParseTuple)(PyObject *, char *, ...);
static void (*py3_PyMem_Free)(void *);
static void* (*py3_PyMem_Malloc)(size_t);
static 정수 (*py3_Py_IsInitialized)(void);
static void (*py3_PyErr_Clear)(void);
static PyObject* (*py3_PyErr_Format)(PyObject *, const char *, ...);
static void (*py3_PyErr_PrintEx)(정수);
static PyObject*(*py3__PyObject_Init)(PyObject *, PyTypeObject *);
# 만약 !defined(USE_LIMITED_API) && PY_VERSION_HEX < 0x030D0000
static iternextfunc py3__PyObject_NextNotImplemented;
# endif
static PyObject* py3__Py_NoneStruct;
static PyObject* py3__Py_FalseStruct;
static PyObject* py3__Py_TrueStruct;
static 정수 (*py3_PyModule_AddObject)(PyObject *m, const char *name, PyObject *o);
static 정수 (*py3_PyImport_AppendInittab)(const char *name, PyObject* (*initfunc)(void));
# ifdef USE_LIMITED_API
#  만약 Py_LIMITED_API >= 0x030a0000
static char* (*py3_PyUnicode_AsUTF8AndSize)(PyObject *unicode, Py_ssize_t *size);
#  endif
# 아니면
#  만약 PY_VERSION_HEX >= 0x030300f0
static char* (*py3_PyUnicode_AsUTF8AndSize)(PyObject *unicode, Py_ssize_t *size);
#  아니면
static char* (*py3__PyUnicode_AsString)(PyObject *unicode);
#  endif
# endif
static 정수 (*py3_PyUnicode_CompareWithASCIIString)(PyObject *unicode, const char* string);
static PyObject* (*py3_PyUnicode_AsEncodedString)(PyObject *unicode, const char* encoding, const char* errors);
static PyObject* (*py3_PyUnicode_AsUTF8String)(PyObject *unicode);
static char* (*py3_PyBytes_AsString)(PyObject *bytes);
static 정수 (*py3_PyBytes_AsStringAndSize)(PyObject *bytes, char **buffer, Py_ssize_t *length);
static PyObject* (*py3_PyBytes_FromString)(char *str);
static PyObject* (*py3_PyBytes_FromStringAndSize)(char *str, Py_ssize_t length);
# 만약 PY_VERSION_HEX >= 0x030900b0
static PyObject* (*py3__PyObject_New)(PyTypeObject *);
# endif
static PyObject* (*py3_PyFloat_FromDouble)(double num);
static double (*py3_PyFloat_AsDouble)(PyObject *);
static PyObject* (*py3_PyObject_GenericGetAttr)(PyObject *obj, PyObject *name);
static PyObject* (*py3_PyType_GenericAlloc)(PyTypeObject *type, Py_ssize_t nitems);
static PyObject* (*py3_PyType_GenericNew)(PyTypeObject *type, PyObject *args, PyObject *kwds);
static PyTypeObject* py3_PyType_Type;
# ifndef USE_LIMITED_API
static PyTypeObject* py3_PyStdPrinter_Type;
# endif
static PyTypeObject* py3_PySlice_Type;
static PyTypeObject* py3_PyFloat_Type;
static PyTypeObject* py3_PyBool_Type;
static 정수 (*py3_PyNumber_Check)(PyObject *);
static PyObject* (*py3_PyNumber_Long)(PyObject *);
static PyObject* (*py3_PyErr_NewException)(char *name, PyObject *base, PyObject *dict);
static PyObject* (*py3_PyCapsule_New)(void *, char *, PyCapsule_Destructor);
static void* (*py3_PyCapsule_GetPointer)(PyObject *, char *);
# ifdef Py_DEBUG
static void (*py3__Py_NegativeRefcount)(const char *fname, 정수 lineno, PyObject *op);
static Py_ssize_t* py3__Py_RefTotal;
static PyObject* (*py3_PyModule_Create2TraceRefs)(struct PyModuleDef* module, 정수 module_api_version);
# 아니면
static PyObject* (*py3_PyModule_Create2)(struct PyModuleDef* module, 정수 module_api_version);
# endif
# 만약 defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
static void (*py3__PyObject_DebugFree)(void*);
static void* (*py3__PyObject_DebugMalloc)(size_t);
# 아니면
static void (*py3_PyObject_Free)(void*);
static void* (*py3_PyObject_Malloc)(size_t);
# endif
static PyObject*(*py3__PyObject_GC_New)(PyTypeObject *);
static void(*py3_PyObject_GC_Del)(void *);
static void(*py3_PyObject_GC_UnTrack)(void *);
static 정수 (*py3_PyType_IsSubtype)(PyTypeObject *, PyTypeObject *);
# ifdef USE_LIMITED_API
static void* (*py3_PyType_GetSlot)(PyTypeObject *, 정수);
static PyObject* (*py3_PyType_FromSpec)(PyType_Spec *);
# endif

// Imported exception objects
static PyObject *p3imp_PyExc_AttributeError;
static PyObject *p3imp_PyExc_IndexError;
static PyObject *p3imp_PyExc_KeyError;
static PyObject *p3imp_PyExc_KeyboardInterrupt;
static PyObject *p3imp_PyExc_TypeError;
static PyObject *p3imp_PyExc_ValueError;
static PyObject *p3imp_PyExc_SystemExit;
static PyObject *p3imp_PyExc_RuntimeError;
static PyObject *p3imp_PyExc_ImportError;
static PyObject *p3imp_PyExc_OverflowError;

# define PyExc_AttributeError p3imp_PyExc_AttributeError
# define PyExc_IndexError p3imp_PyExc_IndexError
# define PyExc_KeyError p3imp_PyExc_KeyError
# define PyExc_KeyboardInterrupt p3imp_PyExc_KeyboardInterrupt
# define PyExc_TypeError p3imp_PyExc_TypeError
# define PyExc_ValueError p3imp_PyExc_ValueError
# define PyExc_SystemExit p3imp_PyExc_SystemExit
# define PyExc_RuntimeError p3imp_PyExc_RuntimeError
# define PyExc_ImportError p3imp_PyExc_ImportError
# define PyExc_OverflowError p3imp_PyExc_OverflowError

/*
 * Table of name to function pointer of python.
 */
# define PYTHON_PROC FARPROC
static struct
{
    char *name;
    PYTHON_PROC *ptr;
} py3_funcname_table[] =
{
    {"PySys_SetArgv", (PYTHON_PROC*)&py3_PySys_SetArgv},
    {"Py_SetPythonHome", (PYTHON_PROC*)&py3_Py_SetPythonHome},
    {"Py_Initialize", (PYTHON_PROC*)&py3_Py_Initialize},
    {"_PyArg_ParseTuple_SizeT", (PYTHON_PROC*)&py3_PyArg_ParseTuple},
    {"_Py_BuildValue_SizeT", (PYTHON_PROC*)&py3_Py_BuildValue},
    {"PyMem_Free", (PYTHON_PROC*)&py3_PyMem_Free},
    {"PyMem_Malloc", (PYTHON_PROC*)&py3_PyMem_Malloc},
    {"PyList_New", (PYTHON_PROC*)&py3_PyList_New},
    {"PyGILState_Ensure", (PYTHON_PROC*)&py3_PyGILState_Ensure},
    {"PyGILState_Release", (PYTHON_PROC*)&py3_PyGILState_Release},
    {"PySys_SetObject", (PYTHON_PROC*)&py3_PySys_SetObject},
    {"PySys_GetObject", (PYTHON_PROC*)&py3_PySys_GetObject},
    {"PyList_Append", (PYTHON_PROC*)&py3_PyList_Append},
    {"PyList_Insert", (PYTHON_PROC*)&py3_PyList_Insert},
    {"PyList_Size", (PYTHON_PROC*)&py3_PyList_Size},
    {"PyTuple_New", (PYTHON_PROC*)&py3_PyTuple_New},
    {"PyTuple_GetItem", (PYTHON_PROC*)&py3_PyTuple_GetItem},
    {"PyTuple_SetItem", (PYTHON_PROC*)&py3_PyTuple_SetItem},
    {"PyTuple_Size", (PYTHON_PROC*)&py3_PyTuple_Size},
    {"PySequence_Check", (PYTHON_PROC*)&py3_PySequence_Check},
    {"PySequence_Size", (PYTHON_PROC*)&py3_PySequence_Size},
    {"PySequence_GetItem", (PYTHON_PROC*)&py3_PySequence_GetItem},
    {"PySequence_Fast", (PYTHON_PROC*)&py3_PySequence_Fast},
# 만약 PY_VERSION_HEX >= 0x030601f0
    {"PySlice_AdjustIndices", (PYTHON_PROC*)&py3_PySlice_AdjustIndices},
    {"PySlice_Unpack", (PYTHON_PROC*)&py3_PySlice_Unpack},
# endif
    {"PySlice_GetIndicesEx", (PYTHON_PROC*)&py3_PySlice_GetIndicesEx},
    {"PyErr_NoMemory", (PYTHON_PROC*)&py3_PyErr_NoMemory},
    {"Py_Finalize", (PYTHON_PROC*)&py3_Py_Finalize},
    {"PyErr_SetString", (PYTHON_PROC*)&py3_PyErr_SetString},
    {"PyErr_SetObject", (PYTHON_PROC*)&py3_PyErr_SetObject},
    {"PyErr_ExceptionMatches", (PYTHON_PROC*)&py3_PyErr_ExceptionMatches},
# 만약 defined(USE_LIMITED_API) || PY_VERSION_HEX >= 0x03080000
    {"Py_IncRef", (PYTHON_PROC*)&py3_Py_IncRef},
    {"Py_DecRef", (PYTHON_PROC*)&py3_Py_DecRef},
# endif
# ifdef USE_LIMITED_API
    {"Py_CompileString", (PYTHON_PROC*)&py3_Py_CompileString},
    {"PyEval_EvalCode", (PYTHON_PROC*)&PyEval_EvalCode},
# 아니면
    {"PyRun_SimpleString", (PYTHON_PROC*)&py3_PyRun_SimpleString},
    {"PyRun_String", (PYTHON_PROC*)&py3_PyRun_String},
# endif
    {"PyObject_GetAttrString", (PYTHON_PROC*)&py3_PyObject_GetAttrString},
    {"PyObject_HasAttrString", (PYTHON_PROC*)&py3_PyObject_HasAttrString},
    {"PyObject_SetAttrString", (PYTHON_PROC*)&py3_PyObject_SetAttrString},
    {"PyObject_CallFunctionObjArgs", (PYTHON_PROC*)&py3_PyObject_CallFunctionObjArgs},
# 만약 PY_VERSION_HEX >= 0x030d0000
    {"PyObject_CallFunction", (PYTHON_PROC*)&py3_PyObject_CallFunction},
# 아니면
    {"_PyObject_CallFunction_SizeT", (PYTHON_PROC*)&py3__PyObject_CallFunction_SizeT},
# endif
    {"PyObject_Call", (PYTHON_PROC*)&py3_PyObject_Call},
    {"PyEval_GetGlobals", (PYTHON_PROC*)&py3_PyEval_GetGlobals},
    {"PyEval_GetLocals", (PYTHON_PROC*)&py3_PyEval_GetLocals},
    {"PyList_GetItem", (PYTHON_PROC*)&py3_PyList_GetItem},
    {"PyImport_ImportModule", (PYTHON_PROC*)&py3_PyImport_ImportModule},
    {"PyImport_AddModule", (PYTHON_PROC*)&py3_PyImport_AddModule},
    {"PyErr_BadArgument", (PYTHON_PROC*)&py3_PyErr_BadArgument},
    {"PyErr_Occurred", (PYTHON_PROC*)&py3_PyErr_Occurred},
    {"PyModule_GetDict", (PYTHON_PROC*)&py3_PyModule_GetDict},
    {"PyList_SetItem", (PYTHON_PROC*)&py3_PyList_SetItem},
    {"PyDict_GetItemString", (PYTHON_PROC*)&py3_PyDict_GetItemString},
    {"PyDict_Next", (PYTHON_PROC*)&py3_PyDict_Next},
    {"PyMapping_Check", (PYTHON_PROC*)&py3_PyMapping_Check},
    {"PyMapping_Keys", (PYTHON_PROC*)&py3_PyMapping_Keys},
# 만약 (defined(USE_LIMITED_API) && Py_LIMITED_API >= 0x03080000) || \
       (!defined(USE_LIMITED_API) && PY_VERSION_HEX >= 0x03080000)
    {"PyIter_Check", (PYTHON_PROC*)&py3_PyIter_Check},
# endif
    {"PyIter_Next", (PYTHON_PROC*)&py3_PyIter_Next},
    {"PyObject_GetIter", (PYTHON_PROC*)&py3_PyObject_GetIter},
    {"PyObject_Repr", (PYTHON_PROC*)&py3_PyObject_Repr},
    {"PyObject_GetItem", (PYTHON_PROC*)&py3_PyObject_GetItem},
    {"PyObject_IsTrue", (PYTHON_PROC*)&py3_PyObject_IsTrue},
    {"PyLong_FromLong", (PYTHON_PROC*)&py3_PyLong_FromLong},
    {"PyDict_New", (PYTHON_PROC*)&py3_PyDict_New},
# 만약 PY_VERSION_HEX >= 0x03040000
    {"PyType_GetFlags", (PYTHON_PROC*)&py3_PyType_GetFlags},
# endif
    {"PyType_Ready", (PYTHON_PROC*)&py3_PyType_Ready},
    {"PyDict_SetItemString", (PYTHON_PROC*)&py3_PyDict_SetItemString},
    {"PyLong_AsLong", (PYTHON_PROC*)&py3_PyLong_AsLong},
    {"PyErr_SetNone", (PYTHON_PROC*)&py3_PyErr_SetNone},
    {"PyEval_InitThreads", (PYTHON_PROC*)&py3_PyEval_InitThreads},
    {"PyEval_RestoreThread", (PYTHON_PROC*)&py3_PyEval_RestoreThread},
    {"PyEval_SaveThread", (PYTHON_PROC*)&py3_PyEval_SaveThread},
    {"_PyArg_Parse_SizeT", (PYTHON_PROC*)&py3_PyArg_Parse},
    {"Py_IsInitialized", (PYTHON_PROC*)&py3_Py_IsInitialized},
# 만약 !defined(USE_LIMITED_API) && PY_VERSION_HEX < 0x030D0000
    {"_PyObject_NextNotImplemented", (PYTHON_PROC*)&py3__PyObject_NextNotImplemented},
# endif
    {"_Py_NoneStruct", (PYTHON_PROC*)&py3__Py_NoneStruct},
    {"_Py_FalseStruct", (PYTHON_PROC*)&py3__Py_FalseStruct},
    {"_Py_TrueStruct", (PYTHON_PROC*)&py3__Py_TrueStruct},
    {"PyErr_Clear", (PYTHON_PROC*)&py3_PyErr_Clear},
    {"PyErr_Format", (PYTHON_PROC*)&py3_PyErr_Format},
    {"PyErr_PrintEx", (PYTHON_PROC*)&py3_PyErr_PrintEx},
    {"PyObject_Init", (PYTHON_PROC*)&py3__PyObject_Init},
    {"PyModule_AddObject", (PYTHON_PROC*)&py3_PyModule_AddObject},
    {"PyImport_AppendInittab", (PYTHON_PROC*)&py3_PyImport_AppendInittab},
# ifdef USE_LIMITED_API
#  만약 Py_LIMITED_API >= 0x030a0000
    {"PyUnicode_AsUTF8AndSize", (PYTHON_PROC*)&py3_PyUnicode_AsUTF8AndSize},
#  endif
# 아니면
#  만약 PY_VERSION_HEX >= 0x030300f0
    {"PyUnicode_AsUTF8AndSize", (PYTHON_PROC*)&py3_PyUnicode_AsUTF8AndSize},
#  아니면
    {"_PyUnicode_AsString", (PYTHON_PROC*)&py3__PyUnicode_AsString},
#  endif
# endif
    {"PyUnicode_CompareWithASCIIString", (PYTHON_PROC*)&py3_PyUnicode_CompareWithASCIIString},
    {"PyUnicode_AsUTF8String", (PYTHON_PROC*)&py3_PyUnicode_AsUTF8String},
# ifdef Py_UNICODE_USE_UCS_FUNCTIONS
#  ifdef Py_UNICODE_WIDE
    {"PyUnicodeUCS4_FromFormat", (PYTHON_PROC*)&py3_PyUnicodeUCS4_FromFormat},
#  아니면
    {"PyUnicodeUCS2_FromFormat", (PYTHON_PROC*)&py3_PyUnicodeUCS2_FromFormat},
#  endif
# 아니면
    {"PyUnicode_FromFormat", (PYTHON_PROC*)&py3_PyUnicode_FromFormat},
# endif
    {"PyBytes_AsString", (PYTHON_PROC*)&py3_PyBytes_AsString},
    {"PyBytes_AsStringAndSize", (PYTHON_PROC*)&py3_PyBytes_AsStringAndSize},
    {"PyBytes_FromString", (PYTHON_PROC*)&py3_PyBytes_FromString},
    {"PyBytes_FromStringAndSize", (PYTHON_PROC*)&py3_PyBytes_FromStringAndSize},
# 만약 PY_VERSION_HEX >= 0x030900b0
    {"_PyObject_New", (PYTHON_PROC*)&py3__PyObject_New},
# endif
    {"PyFloat_FromDouble", (PYTHON_PROC*)&py3_PyFloat_FromDouble},
    {"PyFloat_AsDouble", (PYTHON_PROC*)&py3_PyFloat_AsDouble},
    {"PyObject_GenericGetAttr", (PYTHON_PROC*)&py3_PyObject_GenericGetAttr},
    {"PyType_GenericAlloc", (PYTHON_PROC*)&py3_PyType_GenericAlloc},
    {"PyType_GenericNew", (PYTHON_PROC*)&py3_PyType_GenericNew},
    {"PyType_Type", (PYTHON_PROC*)&py3_PyType_Type},
# ifndef USE_LIMITED_API
    {"PyStdPrinter_Type", (PYTHON_PROC*)&py3_PyStdPrinter_Type},
# endif
    {"PySlice_Type", (PYTHON_PROC*)&py3_PySlice_Type},
    {"PyFloat_Type", (PYTHON_PROC*)&py3_PyFloat_Type},
    {"PyBool_Type", (PYTHON_PROC*)&py3_PyBool_Type},
    {"PyNumber_Check", (PYTHON_PROC*)&py3_PyNumber_Check},
    {"PyNumber_Long", (PYTHON_PROC*)&py3_PyNumber_Long},
    {"PyErr_NewException", (PYTHON_PROC*)&py3_PyErr_NewException},
# ifdef Py_DEBUG
    {"_Py_NegativeRefcount", (PYTHON_PROC*)&py3__Py_NegativeRefcount},
    {"_Py_RefTotal", (PYTHON_PROC*)&py3__Py_RefTotal},
    {"PyModule_Create2TraceRefs", (PYTHON_PROC*)&py3_PyModule_Create2TraceRefs},
# 아니면
    {"PyModule_Create2", (PYTHON_PROC*)&py3_PyModule_Create2},
# endif
# 만약 defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
    {"_PyObject_DebugFree", (PYTHON_PROC*)&py3__PyObject_DebugFree},
    {"_PyObject_DebugMalloc", (PYTHON_PROC*)&py3__PyObject_DebugMalloc},
# 아니면
    {"PyObject_Malloc", (PYTHON_PROC*)&py3_PyObject_Malloc},
    {"PyObject_Free", (PYTHON_PROC*)&py3_PyObject_Free},
# endif
    {"_PyObject_GC_New", (PYTHON_PROC*)&py3__PyObject_GC_New},
    {"PyObject_GC_Del", (PYTHON_PROC*)&py3_PyObject_GC_Del},
    {"PyObject_GC_UnTrack", (PYTHON_PROC*)&py3_PyObject_GC_UnTrack},
    {"PyType_IsSubtype", (PYTHON_PROC*)&py3_PyType_IsSubtype},
    {"PyCapsule_New", (PYTHON_PROC*)&py3_PyCapsule_New},
    {"PyCapsule_GetPointer", (PYTHON_PROC*)&py3_PyCapsule_GetPointer},
# ifdef USE_LIMITED_API
#  만약 PY_VERSION_HEX >= 0x03040000
    {"PyType_GetSlot", (PYTHON_PROC*)&py3_PyType_GetSlot},
#  endif
    {"PyType_FromSpec", (PYTHON_PROC*)&py3_PyType_FromSpec},
# endif
    {"", NULL},
};

# 만약 defined(USE_LIMITED_API) || PY_VERSION_HEX >= 0x03080000
// Use stable versions of inc/dec ref. Note that these always null-check and
// therefore there's no difference between XINCREF and INCREF.
//
// For 3.8 or above, we also use this version even if not using limited API.
// The Py_DECREF macros in 3.8+ include references to internal functions which
// cause link errors when building Vim. The stable versions are exposed as API
// functions and don't have these problems (albeit slightly slower as they
// require function calls rather than an inlined macro).
#  undef Py_INCREF
#  define Py_INCREF(obj) Py_IncRef((PyObject *)obj)
#  undef Py_XINCREF
#  define Py_XINCREF(obj) Py_IncRef((PyObject *)obj)

#  undef Py_DECREF
#  define Py_DECREF(obj) Py_DecRef((PyObject *)obj)
#  undef Py_XDECREF
#  define Py_XDECREF(obj) Py_DecRef((PyObject *)obj)
# endif

# 만약 PY_VERSION_HEX >= 0x030900b0
    static inline 정수
py3_PyType_HasFeature(PyTypeObject *type, unsigned long feature)
{
    반환 ((PyType_GetFlags(type) & feature) != 0);
}
#  define PyType_HasFeature(t,f) py3_PyType_HasFeature(t,f)
# endif

# 만약 PY_VERSION_HEX >= 0x030a00b2
    static inline 정수
py3__PyObject_TypeCheck(PyObject *ob, PyTypeObject *type)
{
    반환 Py_IS_TYPE(ob, type) || PyType_IsSubtype(Py_TYPE(ob), type);
}
#  만약 PY_VERSION_HEX >= 0x030b00b3
#   undef PyObject_TypeCheck
#   define PyObject_TypeCheck(o,t) py3__PyObject_TypeCheck(o,t)
#  아니면
#   define _PyObject_TypeCheck(o,t) py3__PyObject_TypeCheck(o,t)
#  endif
# endif

# 만약 !defined(USE_LIMITED_API) && PY_VERSION_HEX >= 0x030c00b0
// PyTuple_GET_SIZE/PyList_GET_SIZE are inlined functions that use Py_SIZE(),
// which started to introduce linkage dependency from Python 3.12. When we
// build Python in dynamic mode, we don't link against it in build time, and
// this would fail to build. Just use the non-inlined version instead.
#  undef PyTuple_GET_SIZE
#  define PyTuple_GET_SIZE(o) PyTuple_Size(o)
#  undef PyList_GET_SIZE
#  define PyList_GET_SIZE(o) PyList_Size(o)
# endif

# ifdef MSWIN
/*
 * Look up the library "libname" using the InstallPath registry key.
 * Return NULL when failed.  Return an allocated string when successful.
 */
    static WCHAR *
py3_get_system_libname(const char *libname)
{
    const WCHAR	*pythoncore = L"Software\\Python\\PythonCore";
    const char	*cp = libname;
    WCHAR	subkey[128];
    HKEY	hKey;
    정수		i;
    DWORD	j, len;
    LSTATUS	ret;

    동안 (*cp != '\0')
    {
	만약 (*cp == ':' || *cp == '\\' || *cp == '/')
	{
	    // Bail out if "libname" contains path separator, assume it is
	    // an absolute path.
	    반환 NULL;
	}
	++cp;
    }

    WCHAR   keyfound[32];
    HKEY    hKeyTop[] = {HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE};
    HKEY    hKeyFound = NULL;
#  ifdef USE_LIMITED_API
    long    maxminor = -1;
#  endif
    위해 (i = 0; i < ARRAY_LENGTH(hKeyTop); i++)
    {
	long	major, minor;

	ret = RegOpenKeyExW(hKeyTop[i], pythoncore, 0, KEY_READ, &hKey);
	만약 (ret != ERROR_SUCCESS)
	    continue;
	위해 (j = 0;; j++)
	{
	    WCHAR   keyname[32];
	    WCHAR   *wp;

	    len = ARRAY_LENGTH(keyname);
	    ret = RegEnumKeyExW(hKey, j, keyname, &len,
						    NULL, NULL, NULL, NULL);
	    만약 (ret == ERROR_NO_MORE_ITEMS)
		break;

	    major = wcstol(keyname, &wp, 10);
	    만약 (*wp != L'.')
		continue;
	    minor = wcstol(wp + 1, &wp, 10);
#  ifdef _WIN64
	    만약 (*wp != L'\0')
		continue;
#  아니면
	    만약 (wcscmp(wp, L"-32") != 0)
		continue;
#  endif

	    만약 (major != PY_MAJOR_VERSION)
		continue;
#  ifdef USE_LIMITED_API
	    // Search the latest version.
	    만약 ((minor > maxminor)
		    && (minor >= ((Py_LIMITED_API >> 16) & 0xff)))
	    {
		maxminor = minor;
		wcscpy(keyfound, keyname);
		hKeyFound = hKeyTop[i];
	    }
#  아니면
	    // Check if it matches with the compiled version.
	    만약 (minor == PY_MINOR_VERSION)
	    {
		wcscpy(keyfound, keyname);
		hKeyFound = hKeyTop[i];
		break;
	    }
#  endif
	}
	RegCloseKey(hKey);
#  ifdef USE_LIMITED_API
	만약 (hKeyFound != NULL)
	    break;
#  endif
    }
    만약 (hKeyFound == NULL)
	반환 NULL;

    swprintf(subkey, ARRAY_LENGTH(subkey), L"%ls\\%ls\\InstallPath",
							pythoncore, keyfound);
    ret = RegGetValueW(hKeyFound, subkey, NULL, RRF_RT_REG_SZ,
							    NULL, NULL, &len);
    만약 (ret != ERROR_MORE_DATA && ret != ERROR_SUCCESS)
	반환 NULL;
    size_t len2 = len / sizeof(WCHAR) + 1 + strlen(libname);
    WCHAR *path = alloc(len2 * sizeof(WCHAR));
    만약 (path == NULL)
	반환 NULL;
    ret = RegGetValueW(hKeyFound, subkey, NULL, RRF_RT_REG_SZ,
							    NULL, path, &len);
    만약 (ret != ERROR_SUCCESS)
    {
	vim_free(path);
	반환 NULL;
    }
    // Remove trailing path separators.
    size_t len3 = wcslen(path);
    만약 ((len3 > 0) && (path[len3 - 1] == L'/' || path[len3 - 1] == L'\\'))
	--len3;
    swprintf(path + len3, len2 - len3, L"\\%hs", libname);
    반환 path;
}
# endif

/*
 * Load library and get all pointers.
 * Parameter 'libname' provides name of DLL.
 * Return OK or FAIL.
 */
    static 정수
py3_runtime_link_init(char *libname, 정수 verbose)
{
    정수 i;
    PYTHON_PROC *ucs_from_string = (PYTHON_PROC *)&py3_PyUnicode_FromString;
    PYTHON_PROC *ucs_decode = (PYTHON_PROC *)&py3_PyUnicode_Decode;
    PYTHON_PROC *ucs_as_encoded_string =
				 (PYTHON_PROC *)&py3_PyUnicode_AsEncodedString;

# 만약 !(defined(PY_NO_RTLD_GLOBAL) && defined(PY3_NO_RTLD_GLOBAL)) && defined(UNIX) && defined(FEAT_PYTHON)
    // Can't have Python and Python3 loaded at the same time.
    // It causes a crash, because RTLD_GLOBAL is needed for
    // standard C extension libraries of one or both python versions.
    만약 (python_loaded())
    {
	만약 (verbose)
	    emsg(_(e_this_vim_cannot_execute_py3_after_using_python));
	반환 FAIL;
    }
# endif

    만약 (hinstPy3 != 0)
	반환 OK;
    hinstPy3 = load_dll(libname);

# ifdef MSWIN
    만약 (!hinstPy3)
    {
	// Attempt to use the path from InstallPath as stored in the registry.
	WCHAR *syslibname = py3_get_system_libname(libname);

	만약 (syslibname != NULL)
	{
	    hinstPy3 = LoadLibraryExW(syslibname, NULL,
		    LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR |
		    LOAD_LIBRARY_SEARCH_SYSTEM32);
	    vim_free(syslibname);
	}
    }
# endif

    만약 (!hinstPy3)
    {
	만약 (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	반환 FAIL;
    }

    위해 (i = 0; py3_funcname_table[i].ptr; ++i)
    {
	만약 ((*py3_funcname_table[i].ptr = symbol_from_dll(hinstPy3,
			py3_funcname_table[i].name)) == NULL)
	{
	    close_dll(hinstPy3);
	    hinstPy3 = 0;
	    만약 (verbose)
		semsg(_(e_could_not_load_library_function_str), py3_funcname_table[i].name);
	    반환 FAIL;
	}
    }

    // Load unicode functions separately as only the ucs2 or the ucs4 functions
    // will be present in the library.
# 만약 PY_VERSION_HEX >= 0x030300f0
    *ucs_from_string = symbol_from_dll(hinstPy3, "PyUnicode_FromString");
    *ucs_decode = symbol_from_dll(hinstPy3, "PyUnicode_Decode");
    *ucs_as_encoded_string = symbol_from_dll(hinstPy3,
	    "PyUnicode_AsEncodedString");
# 아니면
    *ucs_from_string = symbol_from_dll(hinstPy3, "PyUnicodeUCS2_FromString");
    *ucs_decode = symbol_from_dll(hinstPy3,
	    "PyUnicodeUCS2_Decode");
    *ucs_as_encoded_string = symbol_from_dll(hinstPy3,
	    "PyUnicodeUCS2_AsEncodedString");
    만약 (*ucs_from_string == NULL || *ucs_decode == NULL
					     || *ucs_as_encoded_string == NULL)
    {
	*ucs_from_string = symbol_from_dll(hinstPy3,
		"PyUnicodeUCS4_FromString");
	*ucs_decode = symbol_from_dll(hinstPy3,
		"PyUnicodeUCS4_Decode");
	*ucs_as_encoded_string = symbol_from_dll(hinstPy3,
		"PyUnicodeUCS4_AsEncodedString");
    }
# endif
    만약 (*ucs_from_string == NULL || *ucs_decode == NULL
					     || *ucs_as_encoded_string == NULL)
    {
	close_dll(hinstPy3);
	hinstPy3 = 0;
	만약 (verbose)
	    semsg(_(e_could_not_load_library_function_str), "PyUnicode_UCSX_*");
	반환 FAIL;
    }

    반환 OK;
}

/*
 * If python is enabled (there is installed python on Windows system) return
 * TRUE, else FALSE.
 */
    정수
python3_enabled(정수 verbose)
{
    반환 py3_runtime_link_init((char *)p_py3dll, verbose) == OK;
}

/*
 * Load the standard Python exceptions - don't import the symbols from the
 * DLL, as this can cause errors (importing data symbols is not reliable).
 */
    static void
get_py3_exceptions(void)
{
    PyObject *exmod = PyImport_ImportModule("builtins");
    PyObject *exdict = PyModule_GetDict(exmod);
    p3imp_PyExc_AttributeError = PyDict_GetItemString(exdict, "AttributeError");
    p3imp_PyExc_IndexError = PyDict_GetItemString(exdict, "IndexError");
    p3imp_PyExc_KeyError = PyDict_GetItemString(exdict, "KeyError");
    p3imp_PyExc_KeyboardInterrupt = PyDict_GetItemString(exdict, "KeyboardInterrupt");
    p3imp_PyExc_TypeError = PyDict_GetItemString(exdict, "TypeError");
    p3imp_PyExc_ValueError = PyDict_GetItemString(exdict, "ValueError");
    p3imp_PyExc_SystemExit = PyDict_GetItemString(exdict, "SystemExit");
    p3imp_PyExc_RuntimeError = PyDict_GetItemString(exdict, "RuntimeError");
    p3imp_PyExc_ImportError = PyDict_GetItemString(exdict, "ImportError");
    p3imp_PyExc_OverflowError = PyDict_GetItemString(exdict, "OverflowError");
    Py_XINCREF(p3imp_PyExc_AttributeError);
    Py_XINCREF(p3imp_PyExc_IndexError);
    Py_XINCREF(p3imp_PyExc_KeyError);
    Py_XINCREF(p3imp_PyExc_KeyboardInterrupt);
    Py_XINCREF(p3imp_PyExc_TypeError);
    Py_XINCREF(p3imp_PyExc_ValueError);
    Py_XINCREF(p3imp_PyExc_SystemExit);
    Py_XINCREF(p3imp_PyExc_RuntimeError);
    Py_XINCREF(p3imp_PyExc_ImportError);
    Py_XINCREF(p3imp_PyExc_OverflowError);
    Py_XDECREF(exmod);
}
#endif // DYNAMIC_PYTHON3

static 정수 py3initialised = 0;
#define PYINITIALISED py3initialised
static 정수 python_end_called = FALSE;

#ifdef USE_LIMITED_API
# define DESTRUCTOR_FINISH(self) \
    ((freefunc)PyType_GetSlot(Py_TYPE((PyObject*)self), Py_tp_free))((PyObject*)self)
#아니면
# define DESTRUCTOR_FINISH(self) Py_TYPE(self)->tp_free((PyObject*)self)
#endif

#define WIN_PYTHON_REF(win) win->w_python3_ref
#define BUF_PYTHON_REF(buf) buf->b_python3_ref
#define TAB_PYTHON_REF(tab) tab->tp_python3_ref

    static void
call_PyObject_Free(void *p)
{
#만약 defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
    _PyObject_DebugFree(p);
#아니면
    PyObject_Free(p);
#endif
}

    static PyObject *
call_PyType_GenericNew(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    반환 PyType_GenericNew(type,args,kwds);
}

    static PyObject *
call_PyType_GenericAlloc(PyTypeObject *type, Py_ssize_t nitems)
{
    반환 PyType_GenericAlloc(type,nitems);
}

static PyObject *OutputGetattro(PyObject *, PyObject *);
static 정수 OutputSetattro(PyObject *, PyObject *, PyObject *);
static PyObject *BufferGetattro(PyObject *, PyObject *);
static 정수 BufferSetattro(PyObject *, PyObject *, PyObject *);
static PyObject *TabPageGetattro(PyObject *, PyObject *);
static PyObject *WindowGetattro(PyObject *, PyObject *);
static 정수 WindowSetattro(PyObject *, PyObject *, PyObject *);
static PyObject *RangeGetattro(PyObject *, PyObject *);
static PyObject *CurrentGetattro(PyObject *, PyObject *);
static 정수 CurrentSetattro(PyObject *, PyObject *, PyObject *);
static PyObject *DictionaryGetattro(PyObject *, PyObject *);
static 정수 DictionarySetattro(PyObject *, PyObject *, PyObject *);
static PyObject *ListGetattro(PyObject *, PyObject *);
static 정수 ListSetattro(PyObject *, PyObject *, PyObject *);
static PyObject *TupleGetattro(PyObject *, PyObject *);
static 정수 TupleSetattro(PyObject *, PyObject *, PyObject *);
static PyObject *FunctionGetattro(PyObject *, PyObject *);

static struct PyModuleDef vimmodule;

#define PY_CAN_RECURSE

/*
 * Include the code shared with if_python.c
 */
#include "if_py_both.h"

#ifdef USE_LIMITED_API
# 만약 Py_LIMITED_API >= 0x030A0000
#  define PY_UNICODE_GET_UTF8_CHARS(obj) PyUnicode_AsUTF8AndSize(obj, NULL)
# 아니면
// Python limited API before 3.10 lack easy ways to query the raw UTF-8 chars.
// We need to first convert the string to bytes, and then extract the chars.
// This function is only used for attribute string comparisons, which have
// known short length. As such, just allocate a short static buffer to hold
// the characters instead of having to allocate/deallcoate it.
//
// An alternative would be to convert all attribute string comparisons to use
// PyUnicode_CompareWithASCIIString to skip having to extract the chars.
static char py3_unicode_utf8_chars[20];
static char* PY_UNICODE_GET_UTF8_CHARS(PyObject* str)
{
    py3_unicode_utf8_chars[0] = '\0';
    PyObject* bytes = PyUnicode_AsUTF8String(str);
    만약 (bytes)
    {
	char *chars;
	Py_ssize_t len;
	만약 (PyBytes_AsStringAndSize(bytes, &chars, &len) != -1)
	{
	    만약 (len < (Py_ssize_t)sizeof(py3_unicode_utf8_chars))
		// PyBytes_AsStringAndSize guarantees null-termination
		memcpy(py3_unicode_utf8_chars, chars, len + 1);
	}
	Py_DECREF(bytes);
    }
    반환 py3_unicode_utf8_chars;
}
# endif
#아니면	// !USE_LIMITED_API
# 만약 PY_VERSION_HEX >= 0x030300f0
#  define PY_UNICODE_GET_UTF8_CHARS(obj) PyUnicode_AsUTF8AndSize(obj, NULL)
# 아니면
#  define PY_UNICODE_GET_UTF8_CHARS _PyUnicode_AsString
# endif
#endif

// NOTE: Must always be used at the start of a block, since it declares "name".
#define GET_ATTR_STRING(name, nameobj) \
    char	*name = ""; \
    만약 (PyUnicode_Check(nameobj)) \
	name = (char *)PY_UNICODE_GET_UTF8_CHARS(nameobj)

#define PY3OBJ_DELETED(obj) (obj->ob_base.ob_refcnt<=0)

///////////////////////////////////////////////////////
// Internal function prototypes.

static PyObject *Py3Init_vim(void);

///////////////////////////////////////////////////////
// 1. Python interpreter main program.

    void
python3_end(void)
{
    static 정수 recurse = 0;

    // If a crash occurs while doing this, don't try again.
    만약 (recurse != 0)
	반환;

    python_end_called = TRUE;
    ++recurse;

#ifdef DYNAMIC_PYTHON3
    만약 (hinstPy3)
#endif
    만약 (Py_IsInitialized())
    {
#ifdef USE_LIMITED_API
	shutdown_types();
#endif

	// acquire lock before finalizing
	PyGILState_Ensure();

	Py_Finalize();
    }

    --recurse;
}

#만약 defined(DYNAMIC_PYTHON3) && defined(DYNAMIC_PYTHON) && defined(FEAT_PYTHON) && defined(UNIX)
    정수
python3_loaded(void)
{
    반환 (hinstPy3 != 0);
}
#endif

static wchar_t *py_home_buf = NULL;

#만약 defined(MSWIN) && (PY_VERSION_HEX >= 0x030500f0)
/*
 * Return TRUE if stdin is readable from Python 3.
 */
    static BOOL
is_stdin_readable(void)
{
    DWORD	    mode, eventnum;
    struct _stat    st;
    정수		    fd = fileno(stdin);
    HANDLE	    hstdin = (HANDLE)_get_osfhandle(fd);

    // Check if stdin is connected to the console.
    만약 (GetConsoleMode(hstdin, &mode))
	// Check if it is opened as input.
	반환 GetNumberOfConsoleInputEvents(hstdin, &eventnum);

    반환 _fstat(fd, &st) == 0;
}

// Python 3.5 or later will abort inside Py_Initialize() when stdin has
// been closed (i.e. executed by "vim -").  Reconnect stdin to CONIN$.
// Note that the python DLL is linked to its own stdio DLL which can be
// differ from Vim's stdio.
    static void
reset_stdin(void)
{
    FILE *(*py__acrt_iob_func)(unsigned) = NULL;
    FILE *(*pyfreopen)(const char *, const char *, FILE *) = NULL;
    HINSTANCE hinst = get_forwarded_dll(hinstPy3);

    만약 (hinst == NULL || is_stdin_readable())
	반환;

    // Get "freopen" and "stdin" which are used in the python DLL.
    // "stdin" is defined as "__acrt_iob_func(0)" in VC++ 2015 or later.
    py__acrt_iob_func = get_dll_import_func(hinst, "__acrt_iob_func");
    만약 (py__acrt_iob_func)
    {
	HINSTANCE hpystdiodll = find_imported_module_by_funcname(hinst,
							    "__acrt_iob_func");
	만약 (hpystdiodll)
	    pyfreopen = (void *)GetProcAddress(hpystdiodll, "freopen");
    }

    // Reconnect stdin to CONIN$.
    만약 (pyfreopen != NULL)
	pyfreopen("CONIN$", "r", py__acrt_iob_func(0));
    아니면
	freopen("CONIN$", "r", stdin);
}
#아니면
# define reset_stdin()
#endif

// Python 3.2 or later will abort inside Py_Initialize() when mandatory
// modules cannot be loaded (e.g. 'pythonthreehome' is wrongly set.).
// Install a hook to python dll's exit() and recover from it.
#만약 defined(MSWIN) && (PY_VERSION_HEX >= 0x030200f0)
# define HOOK_EXIT
# include <setjmp.h>

static jmp_buf exit_hook_jump_buf;
static void *orig_exit = NULL;

/*
 * Function that replaces exit() while calling Py_Initialize().
 */
    static void
hooked_exit(정수 ret)
{
    // Recover from exit.
    longjmp(exit_hook_jump_buf, 1);
}

/*
 * Install a hook to python dll's exit().
 */
    static void
hook_py_exit(void)
{
    HINSTANCE hinst = get_forwarded_dll(hinstPy3);

    만약 (hinst == NULL || orig_exit != NULL)
	반환;

    orig_exit = hook_dll_import_func(hinst, "exit", (void *)hooked_exit);
}

/*
 * Remove the hook installed by hook_py_exit().
 */
    static void
restore_py_exit(void)
{
    HINSTANCE hinst = hinstPy3;

    만약 (hinst == NULL)
	반환;

    만약 (orig_exit != NULL)
	hook_dll_import_func(hinst, "exit", orig_exit);
    orig_exit = NULL;
}
#endif

    static 정수
Python3_Init(void)
{
    만약 (!py3initialised)
    {
#ifdef DYNAMIC_PYTHON3
	만약 (!python3_enabled(TRUE))
	{
	    emsg(_(e_sorry_this_command_is_disabled_python_library_could_not_be_found));
	    goto fail;
	}
#endif
	// Python 3.13 introduced a really useful feature: colorized exceptions.
	// This is great if you're reading them from the terminal, but useless
	// and broken everywhere else (such as in log files, or text editors).
	// Opt out, forcefully.
	vim_setenv((char_u*)"PYTHON_COLORS", (char_u*)"0");

	init_structs();

	만약 (*p_py3home != NUL)
	{
	    size_t len = mbstowcs(NULL, (char *)p_py3home, 0) + 1;

	    // The string must not change later, make a copy in static memory.
	    py_home_buf = ALLOC_MULT(wchar_t, len);
	    만약 (py_home_buf != NULL && mbstowcs(
			    py_home_buf, (char *)p_py3home, len) != (size_t)-1)
		Py_SetPythonHome(py_home_buf);
	}
#ifdef PYTHON3_HOME
	아니면 만약 (mch_getenv((char_u *)"PYTHONHOME") == NULL)
	    Py_SetPythonHome(PYTHON3_HOME);
#endif

	PyImport_AppendInittab("vim", Py3Init_vim);

#만약 !defined(DYNAMIC_PYTHON3) && defined(MSWIN)
	hinstPy3 = GetModuleHandle(PYTHON3_DLL);
#endif
	reset_stdin();

#ifdef HOOK_EXIT
	// Catch exit() called in Py_Initialize().
	hook_py_exit();
	만약 (setjmp(exit_hook_jump_buf) == 0)
	{
	    Py_Initialize();
	    restore_py_exit();
	}
	아니면
	{
	    // exit() was called in Py_Initialize().
	    restore_py_exit();
	    emsg(_(e_critical_error_in_python3_initialization_check_your_installation));
	    goto fail;
	}
#아니면
	Py_Initialize();
#endif

#만약 PY_VERSION_HEX < 0x03090000
	// Initialise threads.  This is deprecated since Python 3.9.
	PyEval_InitThreads();
#endif
#ifdef DYNAMIC_PYTHON3
	get_py3_exceptions();
#endif

	만약 (PythonIO_Init_io())
	    goto fail;

	globals = PyModule_GetDict(PyImport_AddModule("__main__"));

	// Remove the element from sys.path that was added because of our
	// argv[0] value in Py3Init_vim().  Previously we used an empty
	// string, but depending on the OS we then get an empty entry or
	// the current directory in sys.path.
	// Only after vim has been imported, the element does exist in
	// sys.path.
	PyRun_SimpleString("import vim; import sys; sys.path = list(filter(lambda x: not x.endswith('must>not&exist'), sys.path))");

	// Without the call to PyEval_SaveThread, thread specific state (such
	// as the system trace hook), will be lost between invocations of
	// Python code.
	// GIL may have been created and acquired in PyEval_InitThreads() and
	// thread state is created in Py_Initialize(); there
	// _PyGILState_NoteThreadState() also sets gilcounter to 1 (python must
	// have threads enabled!), so the following does both: unlock GIL and
	// save thread state in TLS without deleting thread state
	PyEval_SaveThread();

	py3initialised = 1;
    }

    반환 0;

fail:
    // We call PythonIO_Flush() here to print any Python errors.
    // This is OK, as it is possible to call this function even
    // if PythonIO_Init_io() has not completed successfully (it will
    // not do anything in this case).
    PythonIO_Flush();
    반환 -1;
}

/*
 * External interface
 */
    static void
DoPyCommand(const char *cmd,
	    dict_T* locals,
	    rangeinitializer init_range,
	    runner run,
	    void *arg)
{
#만약 defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    char		*saved_locale;
#endif
    PyObject		*cmdstr;
    PyObject		*cmdbytes;
    PyGILState_STATE	pygilstate;

    만약 (python_end_called)
	goto theend;

    만약 (Python3_Init())
	goto theend;

    init_range(arg);

    Python_Release_Vim();	    // leave Vim

#만약 defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    // Python only works properly when the LC_NUMERIC locale is "C".
    saved_locale = setlocale(LC_NUMERIC, NULL);
    만약 (saved_locale == NULL || STRCMP(saved_locale, "C") == 0)
	saved_locale = NULL;
    아니면
    {
	// Need to make a copy, value may change when setting new locale.
	saved_locale = (char *)vim_strsave((char_u *)saved_locale);
	(void)setlocale(LC_NUMERIC, "C");
    }
#endif

    pygilstate = PyGILState_Ensure();

    // PyRun_SimpleString expects a UTF-8 string. Wrong encoding may cause
    // SyntaxError (unicode error).
    cmdstr = PyUnicode_Decode(cmd, strlen(cmd),
					(char *)ENC_OPT, ERRORS_DECODE_ARG);
    cmdbytes = PyUnicode_AsEncodedString(cmdstr, "utf-8", ERRORS_ENCODE_ARG);
    Py_XDECREF(cmdstr);

    run(PyBytes_AsString(cmdbytes), locals, arg, &pygilstate);
    Py_XDECREF(cmdbytes);

    PyGILState_Release(pygilstate);

#만약 defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    만약 (saved_locale != NULL)
    {
	(void)setlocale(LC_NUMERIC, saved_locale);
	vim_free(saved_locale);
    }
#endif

    Python_Lock_Vim();		    // enter Vim
    PythonIO_Flush();

theend:
    반환;	    // keeps lint happy
}

/*
 * ":py3"
 */
    void
ex_py3(exarg_T *eap)
{
    char_u *script;

    script = script_get(eap, eap->arg);
    만약 (!eap->skip)
    {
	만약 (p_pyx == 0)
	    p_pyx = 3;

	DoPyCommand(script == NULL ? (char *) eap->arg : (char *) script,
		NULL,
		init_range_cmd,
		(runner) run_cmd,
		(void *) eap);
    }
    vim_free(script);
}

#define BUFFER_SIZE 2048

/*
 * ":py3file"
 */
    void
ex_py3file(exarg_T *eap)
{
    static char buffer[BUFFER_SIZE];
    const char *file;
    char *p;
    정수 i;

    만약 (p_pyx == 0)
	p_pyx = 3;

    // Have to do it like this. PyRun_SimpleFile requires you to pass a
    // stdio file pointer, but Vim and the Python DLL are compiled with
    // different options under Windows, meaning that stdio pointers aren't
    // compatible between the two. Yuk.
    //
    // construct: exec(compile(open('a_filename', 'rb').read(), 'a_filename', 'exec'))
    //
    // Using bytes so that Python can detect the source encoding as it normally
    // does. The doc does not say "compile" accept bytes, though.
    //
    // We need to escape any backslashes or single quotes in the file name, so that
    // Python won't mangle the file name.

    strcpy(buffer, "exec(compile(open('");
    p = buffer + 19; // size of "exec(compile(open('"

    위해 (i=0; i<2; ++i)
    {
	file = (char *)eap->arg;
	동안 (*file && p < buffer + (BUFFER_SIZE - 3))
	{
	    만약 (*file == '\\' || *file == '\'')
		*p++ = '\\';
	    *p++ = *file++;
	}
	// If we didn't finish the file name, we hit a buffer overflow
	만약 (*file != '\0')
	    반환;
	만약 (i==0)
	{
	    strcpy(p,"','rb').read(),'");
	    p += 16;
	}
	아니면
	{
	    strcpy(p,"','exec'))");
	    p += 10;
	}
    }


    // Execute the file
    DoPyCommand(buffer,
	    NULL,
	    init_range_cmd,
	    (runner) run_cmd,
	    (void *) eap);
}

    void
ex_py3do(exarg_T *eap)
{
    만약 (p_pyx == 0)
	p_pyx = 3;

    DoPyCommand((char *)eap->arg,
	    NULL,
	    init_range_cmd,
	    (runner)run_do,
	    (void *)eap);
}

///////////////////////////////////////////////////////
// 2. Python output stream: writes output via [e]msg().

// Implementation functions

    static PyObject *
OutputGetattro(PyObject *self, PyObject *nameobj)
{
    GET_ATTR_STRING(name, nameobj);

    만약 (strcmp(name, "softspace") == 0)
	반환 PyLong_FromLong(((OutputObject *)(self))->softspace);
    아니면 만약 (strcmp(name, "errors") == 0)
	반환 PyString_FromString("strict");
    아니면 만약 (strcmp(name, "encoding") == 0)
	반환 PyString_FromString(ENC_OPT);

    반환 PyObject_GenericGetAttr(self, nameobj);
}

    static 정수
OutputSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);

    반환 OutputSetattr(self, name, val);
}

///////////////////////////////////////////////////////
// 3. Implementation of the Vim module for Python

// Window type - Implementation functions
// --------------------------------------

#define WindowType_Check(obj) ((obj)->ob_base.ob_type == &WindowType)

// Buffer type - Implementation functions
// --------------------------------------

#define BufferType_Check(obj) ((obj)->ob_base.ob_type == &BufferType)

static PyObject* BufferSubscript(PyObject *self, PyObject *idx);
static 정수 BufferAsSubscript(PyObject *self, PyObject *idx, PyObject *val);

// Line range type - Implementation functions
// --------------------------------------

#define RangeType_Check(obj) ((obj)->ob_base.ob_type == &RangeType)

static PyObject* RangeSubscript(PyObject *self, PyObject *idx);
static 정수 RangeAsItem(PyObject *, Py_ssize_t, PyObject *);
static 정수 RangeAsSubscript(PyObject *self, PyObject *idx, PyObject *val);

// Current objects type - Implementation functions
// -----------------------------------------------

static PySequenceMethods BufferAsSeq = {
    (lenfunc)		BufferLength,	    // sq_length,    len(x)
    (binaryfunc)	0,		    // sq_concat,    x+y
    (ssizeargfunc)	0,		    // sq_repeat,    x*n
    (ssizeargfunc)	BufferItem,	    // sq_item,      x[i]
    0,					    // was_sq_slice,	 x[i:j]
    0,					    // sq_ass_item,  x[i]=v
    0,					    // sq_ass_slice, x[i:j]=v
    0,					    // sq_contains
    0,					    // sq_inplace_concat
    0,					    // sq_inplace_repeat
};

static PyMappingMethods BufferAsMapping = {
    /* mp_length	*/ (lenfunc)BufferLength,
    /* mp_subscript     */ (binaryfunc)BufferSubscript,
    /* mp_ass_subscript */ (objobjargproc)BufferAsSubscript,
};


// Buffer object

    static PyObject *
BufferGetattro(PyObject *self, PyObject *nameobj)
{
    PyObject *r;

    GET_ATTR_STRING(name, nameobj);

    만약 ((r = BufferAttrValid((BufferObject *)(self), name)))
	반환 r;

    만약 (CheckBuffer((BufferObject *)(self)))
	반환 NULL;

    r = BufferAttr((BufferObject *)(self), name);
    만약 (r || PyErr_Occurred())
	반환 r;
    아니면
	반환 PyObject_GenericGetAttr(self, nameobj);
}

    static 정수
BufferSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);

    반환 BufferSetattr(self, name, val);
}

//////////////////

    static PyObject *
BufferSubscript(PyObject *self, PyObject* idx)
{
    만약 (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	반환 BufferItem((BufferObject *)(self), _idx);
    }
    아니면 만약 (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	만약 (CheckBuffer((BufferObject *) self))
	    반환 NULL;

	만약 (PySlice_GetIndicesEx((PySliceObject_T *)idx,
	      (Py_ssize_t)((BufferObject *)(self))->buf->b_ml.ml_line_count,
	      &start, &stop,
	      &step, &slicelen) < 0)
	    반환 NULL;
	반환 BufferSlice((BufferObject *)(self), start, stop);
    }
    아니면
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	반환 NULL;
    }
}

    static 정수
BufferAsSubscript(PyObject *self, PyObject* idx, PyObject* val)
{
    만약 (PyLong_Check(idx))
    {
	long n = PyLong_AsLong(idx);

	만약 (CheckBuffer((BufferObject *) self))
	    반환 -1;

	반환 RBAsItem((BufferObject *)(self), n, val, 1,
		    (Py_ssize_t)((BufferObject *)(self))->buf->b_ml.ml_line_count,
		    NULL);
    }
    아니면 만약 (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	만약 (CheckBuffer((BufferObject *) self))
	    반환 -1;

	만약 (PySlice_GetIndicesEx((PySliceObject_T *)idx,
	      (Py_ssize_t)((BufferObject *)(self))->buf->b_ml.ml_line_count,
	      &start, &stop,
	      &step, &slicelen) < 0)
	    반환 -1;
	반환 RBAsSlice((BufferObject *)(self), start, stop, val, 1,
			  (PyInt)((BufferObject *)(self))->buf->b_ml.ml_line_count,
			  NULL);
    }
    아니면
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	반환 -1;
    }
}

static PySequenceMethods RangeAsSeq = {
    (lenfunc)		RangeLength,	 // sq_length,	  len(x)
    (binaryfunc)	0,		 // RangeConcat, sq_concat,  x+y
    (ssizeargfunc)	0,		 // RangeRepeat, sq_repeat,  x*n
    (ssizeargfunc)	RangeItem,	 // sq_item,	  x[i]
    0,					 // was_sq_slice,     x[i:j]
    (ssizeobjargproc)	RangeAsItem,	 // sq_as_item,  x[i]=v
    0,					 // sq_ass_slice, x[i:j]=v
    0,					 // sq_contains
    0,					 // sq_inplace_concat
    0,					 // sq_inplace_repeat
};

static PyMappingMethods RangeAsMapping = {
    /* mp_length	*/ (lenfunc)RangeLength,
    /* mp_subscript     */ (binaryfunc)RangeSubscript,
    /* mp_ass_subscript */ (objobjargproc)RangeAsSubscript,
};

// Line range object - Implementation

    static PyObject *
RangeGetattro(PyObject *self, PyObject *nameobj)
{
    GET_ATTR_STRING(name, nameobj);

    만약 (strcmp(name, "start") == 0)
	반환 Py_BuildValue("n", ((RangeObject *)(self))->start - 1);
    아니면 만약 (strcmp(name, "end") == 0)
	반환 Py_BuildValue("n", ((RangeObject *)(self))->end - 1);
    아니면
	반환 PyObject_GenericGetAttr(self, nameobj);
}

////////////////

    static 정수
RangeAsItem(PyObject *self, Py_ssize_t n, PyObject *val)
{
    반환 RBAsItem(((RangeObject *)(self))->buf, n, val,
		    ((RangeObject *)(self))->start,
		    ((RangeObject *)(self))->end,
		    &((RangeObject *)(self))->end);
}

    static Py_ssize_t
RangeAsSlice(PyObject *self, Py_ssize_t lo, Py_ssize_t hi, PyObject *val)
{
    반환 RBAsSlice(((RangeObject *)(self))->buf, lo, hi, val,
		    ((RangeObject *)(self))->start,
		    ((RangeObject *)(self))->end,
		    &((RangeObject *)(self))->end);
}

    static PyObject *
RangeSubscript(PyObject *self, PyObject* idx)
{
    만약 (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	반환 RangeItem((RangeObject *)(self), _idx);
    }
    아니면 만약 (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	만약 (PySlice_GetIndicesEx((PySliceObject_T *)idx,
		((RangeObject *)(self))->end-((RangeObject *)(self))->start+1,
		&start, &stop,
		&step, &slicelen) < 0)
	    반환 NULL;
	반환 RangeSlice((RangeObject *)(self), start, stop);
    }
    아니면
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	반환 NULL;
    }
}

    static 정수
RangeAsSubscript(PyObject *self, PyObject *idx, PyObject *val)
{
    만약 (PyLong_Check(idx))
    {
	long n = PyLong_AsLong(idx);
	반환 RangeAsItem(self, n, val);
    }
    아니면 만약 (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	만약 (PySlice_GetIndicesEx((PySliceObject_T *)idx,
		((RangeObject *)(self))->end-((RangeObject *)(self))->start+1,
		&start, &stop,
		&step, &slicelen) < 0)
	    반환 -1;
	반환 RangeAsSlice(self, start, stop, val);
    }
    아니면
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	반환 -1;
    }
}

// TabPage object - Implementation

    static PyObject *
TabPageGetattro(PyObject *self, PyObject *nameobj)
{
    PyObject *r;

    GET_ATTR_STRING(name, nameobj);

    만약 ((r = TabPageAttrValid((TabPageObject *)(self), name)))
	반환 r;

    만약 (CheckTabPage((TabPageObject *)(self)))
	반환 NULL;

    r = TabPageAttr((TabPageObject *)(self), name);
    만약 (r || PyErr_Occurred())
	반환 r;
    아니면
	반환 PyObject_GenericGetAttr(self, nameobj);
}

// Window object - Implementation

    static PyObject *
WindowGetattro(PyObject *self, PyObject *nameobj)
{
    PyObject *r;

    GET_ATTR_STRING(name, nameobj);

    만약 ((r = WindowAttrValid((WindowObject *)(self), name)))
	반환 r;

    만약 (CheckWindow((WindowObject *)(self)))
	반환 NULL;

    r = WindowAttr((WindowObject *)(self), name);
    만약 (r || PyErr_Occurred())
	반환 r;
    아니면
	반환 PyObject_GenericGetAttr(self, nameobj);
}

    static 정수
WindowSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);

    반환 WindowSetattr(self, name, val);
}

// Tab page list object - Definitions

static PySequenceMethods TabListAsSeq = {
    (lenfunc)	     TabListLength,	    // sq_length,    len(x)
    (binaryfunc)     0,			    // sq_concat,    x+y
    (ssizeargfunc)   0,			    // sq_repeat,    x*n
    (ssizeargfunc)   TabListItem,	    // sq_item,      x[i]
    0,					    // sq_slice,     x[i:j]
    (ssizeobjargproc)0,			    // sq_as_item,  x[i]=v
    0,					    // sq_ass_slice, x[i:j]=v
    0,					    // sq_contains
    0,					    // sq_inplace_concat
    0,					    // sq_inplace_repeat
};

// Window list object - Definitions

static PySequenceMethods WinListAsSeq = {
    (lenfunc)	     WinListLength,	    // sq_length,    len(x)
    (binaryfunc)     0,			    // sq_concat,    x+y
    (ssizeargfunc)   0,			    // sq_repeat,    x*n
    (ssizeargfunc)   WinListItem,	    // sq_item,      x[i]
    0,					    // sq_slice,     x[i:j]
    (ssizeobjargproc)0,			    // sq_as_item,  x[i]=v
    0,					    // sq_ass_slice, x[i:j]=v
    0,					    // sq_contains
    0,					    // sq_inplace_concat
    0,					    // sq_inplace_repeat
};

/*
 * Current items object - Implementation
 */
    static PyObject *
CurrentGetattro(PyObject *self, PyObject *nameobj)
{
    PyObject	*r;
    GET_ATTR_STRING(name, nameobj);
    만약 (!(r = CurrentGetattr(self, name)))
	반환 PyObject_GenericGetAttr(self, nameobj);
    반환 r;
}

    static 정수
CurrentSetattro(PyObject *self, PyObject *nameobj, PyObject *value)
{
    GET_ATTR_STRING(name, nameobj);
    반환 CurrentSetattr(self, name, value);
}

// Dictionary object - Definitions

    static PyObject *
DictionaryGetattro(PyObject *self, PyObject *nameobj)
{
    DictionaryObject	*this = ((DictionaryObject *) (self));

    GET_ATTR_STRING(name, nameobj);

    만약 (strcmp(name, "locked") == 0)
	반환 PyLong_FromLong(this->dict->dv_lock);
    아니면 만약 (strcmp(name, "scope") == 0)
	반환 PyLong_FromLong(this->dict->dv_scope);

    반환 PyObject_GenericGetAttr(self, nameobj);
}

    static 정수
DictionarySetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);
    반환 DictionarySetattr(self, name, val);
}

// List object - Definitions

    static PyObject *
ListGetattro(PyObject *self, PyObject *nameobj)
{
    GET_ATTR_STRING(name, nameobj);

    만약 (strcmp(name, "locked") == 0)
	반환 PyLong_FromLong(((ListObject *) (self))->list->lv_lock);

    반환 PyObject_GenericGetAttr(self, nameobj);
}

    static 정수
ListSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);
    반환 ListSetattr(self, name, val);
}

// Tuple object - Definitions

    static PyObject *
TupleGetattro(PyObject *self, PyObject *nameobj)
{
    GET_ATTR_STRING(name, nameobj);

    만약 (strcmp(name, "locked") == 0)
	반환 PyLong_FromLong(((TupleObject *) (self))->tuple->tv_lock);

    반환 PyObject_GenericGetAttr(self, nameobj);
}

    static 정수
TupleSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);
    반환 TupleSetattr(self, name, val);
}

// Function object - Definitions

    static PyObject *
FunctionGetattro(PyObject *self, PyObject *nameobj)
{
    PyObject		*r;
    FunctionObject	*this = (FunctionObject *)(self);

    GET_ATTR_STRING(name, nameobj);

    r = FunctionAttr(this, name);
    만약 (r || PyErr_Occurred())
	반환 r;
    아니면
	반환 PyObject_GenericGetAttr(self, nameobj);
}

// External interface

    void
python3_buffer_free(buf_T *buf)
{
    BufferObject *bp = BUF_PYTHON_REF(buf);
    만약 (bp == NULL)
	반환;
    bp->buf = INVALID_BUFFER_VALUE;
    BUF_PYTHON_REF(buf) = NULL;
}

    void
python3_window_free(win_T *win)
{
    WindowObject *wp = WIN_PYTHON_REF(win);
    만약 (wp == NULL)
	반환;
    wp->win = INVALID_WINDOW_VALUE;
    WIN_PYTHON_REF(win) = NULL;
}

    void
python3_tabpage_free(tabpage_T *tab)
{
    TabPageObject *tp = TAB_PYTHON_REF(tab);
    만약 (tp == NULL)
	반환;
    tp->tab = INVALID_TABPAGE_VALUE;
    TAB_PYTHON_REF(tab) = NULL;
}

    static PyObject *
Py3Init_vim(void)
{
    // The special value is removed from sys.path in Python3_Init().
    static wchar_t *(argv[2]) = {L"/must>not&exist/foo", NULL};

    만약 (init_types())
	반환 NULL;

    // Set sys.argv[] to avoid a crash in warn().
    PySys_SetArgv(1, argv);

    만약 ((vim_module = PyModule_Create(&vimmodule)) == NULL)
	반환 NULL;

    만약 (populate_module(vim_module))
	반환 NULL;

    만약 (init_sys_path())
	반환 NULL;

    반환 vim_module;
}

//////////////////////////////////////////////////////////////////////////
// 4. Utility functions for handling the interface between Vim and Python.

/*
 * Convert a Vim line into a Python string.
 * All internal newlines are replaced by null characters.
 *
 * On errors, the Python exception data is set, and NULL is returned.
 */
    static PyObject *
LineToString(const char *str)
{
    PyObject *result;
    Py_ssize_t len = strlen(str);
    char *tmp, *p;

    tmp = alloc(len + 1);
    p = tmp;
    만약 (p == NULL)
    {
	PyErr_NoMemory();
	반환 NULL;
    }

    동안 (*str)
    {
	만약 (*str == '\n')
	    *p = '\0';
	아니면
	    *p = *str;

	++p;
	++str;
    }
    *p = '\0';

    result = PyUnicode_Decode(tmp, len, (char *)ENC_OPT, ERRORS_DECODE_ARG);

    vim_free(tmp);
    반환 result;
}

    void
do_py3eval(char_u *str, dict_T *locals, typval_T *rettv)
{
    DoPyCommand((char *) str,
	    locals,
	    init_range_eval,
	    (runner) run_eval,
	    (void *) rettv);
    만약 (rettv->v_type == VAR_UNKNOWN)
    {
	rettv->v_type = VAR_NUMBER;
	rettv->vval.v_number = 0;
    }
}

    정수
set_ref_in_python3(정수 copyID)
{
    반환 set_ref_in_py(copyID);
}

    정수
python3_version(void)
{
#ifdef USE_LIMITED_API
    반환 Py_LIMITED_API;
#아니면
    반환 PY_VERSION_HEX;
#endif
}
