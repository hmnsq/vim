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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಅನ್ತರ್ಭಾವ <limits.h>

#ಯದಿ defined(MSWIN) && defined(HAVE_FCNTL_H)
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

#ಅನ್ತರ್ಭಾವ <Python.h>

#undef main // Defined in python.h - aargh
#undef HAVE_FCNTL_H // Clash with os_win32.h

// The "surrogateescape" error handler is new in Python 3.1
#ಯದಿ PY_VERSION_HEX >= 0x030100f0
# define CODEC_ERROR_HANDLER "surrogateescape"
#ಅನ್ಯಥಾ
# define CODEC_ERROR_HANDLER ರಿಕ್ತ
#endif

// Suppress Python 3.11 depreciations to see useful warnings
#ifdef __GNUC__
# pragma GCC diagnostic ತಳ್ಳು
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
#ಯದಿ PY_VERSION_HEX >= 0x030200f0
ಪ್ರಕಾರನಿರ್ದೇಶ PyObject PySliceObject_T;
#ಅನ್ಯಥಾ
ಪ್ರಕಾರನಿರ್ದೇಶ PySliceObject PySliceObject_T;
#endif

#ifndef MSWIN
# define HINSTANCE ಶೂನ್ಯ *
#endif
#ಯದಿ defined(DYNAMIC_PYTHON3) || defined(MSWIN)
ಸ್ಥಿರ HINSTANCE hinstPy3 = 0; // Instance of python.dll
#endif

#ಯದಿ defined(DYNAMIC_PYTHON3)

# ifdef MSWIN
#  define load_dll vimLoadLib
#  define close_dll FreeLibrary
#  define ಚಿಹ್ನೆ_from_dll GetProcAddress
#  define load_dll_error GetWin32Error
# ಅನ್ಯಥಾ
#  include <dlfcn.h>
#  define FARPROC ಶೂನ್ಯ*
#  ಯದಿ defined(PY_NO_RTLD_GLOBAL) && defined(PY3_NO_RTLD_GLOBAL)
#   define load_dll(n) dlopen((n), RTLD_LAZY)
#  ಅನ್ಯಥಾ
#   define load_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#  endif
#  define close_dll dlclose
#  define ಚಿಹ್ನೆ_from_dll dlsym
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
# define Py_List_ಸೇರಿಸು py3_Py_List_ಸೇರಿಸು
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
# ಯದಿ PY_VERSION_HEX >= 0x030601f0
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
# ifndef Py_Mapping_ಕೀಲಿಗಳು
#  define Py_Mapping_ಕೀಲಿಗಳು py3_Py_Mapping_ಕೀಲಿಗಳು
# endif
# ಯದಿ (defined(USE_LIMITED_API) && Py_LIMITED_API >= 0x03080000) || \
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
# ಯದಿ defined(USE_LIMITED_API) || PY_VERSION_HEX >= 0x03080000
#  define Py_IncRef py3_Py_IncRef
#  define Py_DecRef py3_Py_DecRef
# endif
# ifdef USE_LIMITED_API
#  define Py_CompileString py3_Py_CompileString
#  define PyEval_EvalCode py3_PyEval_EvalCode
# ಅನ್ಯಥಾ
#  undef PyRun_SimpleString
#  define PyRun_SimpleString py3_PyRun_SimpleString
#  undef PyRun_String
#  define PyRun_String py3_PyRun_String
# endif
# define PyObject_GetAttrString py3_PyObject_GetAttrString
# define PyObject_HasAttrString py3_PyObject_HasAttrString
# define PyObject_SetAttrString py3_PyObject_SetAttrString
# define PyObject_CallFunctionObjArgs py3_PyObject_CallFunctionObjArgs
# ಯದಿ PY_VERSION_HEX >= 0x030d0000
#  define PyObject_CallFunction py3_PyObject_CallFunction
# ಅನ್ಯಥಾ
#  define _PyObject_CallFunction_SizeT py3__PyObject_CallFunction_SizeT
# endif
# define PyObject_Call py3_PyObject_Call
# define PyEval_GetLocals py3_PyEval_GetLocals
# define PyEval_GetGlobals py3_PyEval_GetGlobals
# define PySys_SetObject py3_PySys_SetObject
# define PySys_GetObject py3_PySys_GetObject
# define PySys_SetArgv py3_PySys_SetArgv
# define PyType_Ready py3_PyType_Ready
# ಯದಿ PY_VERSION_HEX >= 0x03040000
#  define PyType_GetFlags py3_PyType_GetFlags
# endif
# undef Py_Build_ಮೌಲ್ಯ
# define Py_Build_ಮೌಲ್ಯ py3_Py_Build_ಮೌಲ್ಯ
# define Py_SetPythonHome py3_Py_SetPythonHome
# define Py_Initialize py3_Py_Initialize
# define Py_Finalize py3_Py_Finalize
# define Py_IsInitialized py3_Py_IsInitialized
# define _Py_NoneStruct (*py3__Py_NoneStruct)
# define _Py_FalseStruct (*py3__Py_FalseStruct)
# define _Py_TrueStruct (*py3__Py_TrueStruct)
# ಯದಿ !defined(USE_LIMITED_API) && PY_VERSION_HEX < 0x030D0000
// Private symbol that used to be required as part of PyIter_Check.
#  define _PyObject_NextNotImplemented (*py3__PyObject_NextNotImplemented)
# endif
# define PyModule_AddObject py3_PyModule_AddObject
# define PyImport_AppendInittab py3_PyImport_AppendInittab
# define PyImport_AddModule py3_PyImport_AddModule
# ifdef USE_LIMITED_API
#  ಯದಿ Py_LIMITED_API >= 0x030a0000
#   define PyUnicode_AsUTF8AndSize py3_PyUnicode_AsUTF8AndSize
#  endif
# ಅನ್ಯಥಾ
#  ಯದಿ PY_VERSION_HEX >= 0x030300f0
#   define PyUnicode_AsUTF8AndSize py3_PyUnicode_AsUTF8AndSize
#  ಅನ್ಯಥಾ
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
# define Py_ಸಂಖ್ಯೆ_Check (*py3_Py_ಸಂಖ್ಯೆ_Check)
# define Py_ಸಂಖ್ಯೆ_Long (*py3_Py_ಸಂಖ್ಯೆ_Long)
# define PyBool_Type (*py3_PyBool_Type)
# define PyErr_NewException py3_PyErr_NewException
# ifdef Py_DEBUG
#  define _Py_NegativeRefcount py3__Py_NegativeRefcount
#  define _Py_RefTotal (*py3__Py_RefTotal)
#  define PyModule_Create2TraceRefs py3_PyModule_Create2TraceRefs
# ಅನ್ಯಥಾ
#  define PyModule_Create2 py3_PyModule_Create2
# endif
# ಯದಿ defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
#  define _PyObject_DebugMalloc py3__PyObject_DebugMalloc
#  define _PyObject_DebugFree py3__PyObject_DebugFree
# ಅನ್ಯಥಾ
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
#  ಅನ್ಯಥಾ
#   define PyUnicodeUCS2_FromFormat py3_PyUnicodeUCS2_FromFormat
#  endif
# ಅನ್ಯಥಾ
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

# ಯದಿ defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
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
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ (*py3_PySys_SetArgv)(ಪೂರ್ಣಾಙ್ಕ, wchar_t **);
ಸ್ಥಿರ ಶೂನ್ಯ (*py3_Py_SetPythonHome)(wchar_t *home);
ಸ್ಥಿರ ಶೂನ್ಯ (*py3_Py_Initialize)(ಶೂನ್ಯ);
ಸ್ಥಿರ PyObject* (*py3_PyList_New)(Py_ssize_t size);
ಸ್ಥಿರ PyGILState_STATE (*py3_PyGILState_Ensure)(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ (*py3_PyGILState_Release)(PyGILState_STATE);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ (*py3_PySys_SetObject)(ವರ್ಣ *, PyObject *);
ಸ್ಥಿರ PyObject* (*py3_PySys_GetObject)(ವರ್ಣ *);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ (*py3_PyList_Append)(PyObject *, PyObject *);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ (*py3_Py_List_ಸೇರಿಸು)(PyObject *, ಪೂರ್ಣಾಙ್ಕ, PyObject *);
ಸ್ಥಿರ Py_ssize_t (*py3_PyList_Size)(PyObject *);
ಸ್ಥಿರ PyObject* (*py3_PyTuple_New)(Py_ssize_t size);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ (*py3_PyTuple_SetItem)(PyObject *, Py_ssize_t, PyObject *);
ಸ್ಥಿರ Py_ssize_t (*py3_PyTuple_Size)(PyObject *);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ (*py3_PySequence_Check)(PyObject *);
ಸ್ಥಿರ Py_ssize_t (*py3_PySequence_Size)(PyObject *);
ಸ್ಥಿರ PyObject* (*py3_PySequence_GetItem)(PyObject *, Py_ssize_t);
ಸ್ಥಿರ PyObject* (*py3_PySequence_Fast)(PyObject *, ಧ್ರುವ ವರ್ಣ *);
ಸ್ಥಿರ PyObject* (*py3_PyTuple_GetItem)(PyObject *, Py_ssize_t);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ (*py3_PyMapping_Check)(PyObject *);
ಸ್ಥಿರ PyObject* (*py3_Py_Mapping_ಕೀಲಿಗಳು)(PyObject *);
# ಯದಿ PY_VERSION_HEX >= 0x030601f0
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ (*py3_PySlice_AdjustIndices)(Py_ssize_t length,
		     Py_ssize_t *start, Py_ssize_t *stop, Py_ssize_t step);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ (*py3_PySlice_Unpack)(PyObject *slice,
		     Py_ssize_t *start, Py_ssize_t *stop, Py_ssize_t *step);
# endif
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ (*py3_PySlice_GetIndicesEx)(PySliceObject_T *r, Py_ssize_t length,
		     Py_ssize_t *start, Py_ssize_t *stop, Py_ssize_t *step,
		     Py_ssize_t *slicelen);
ಸ್ಥಿರ PyObject* (*py3_PyErr_NoMemory)(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ (*py3_Py_Finalize)(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ (*py3_PyErr_SetString)(PyObject *, ಧ್ರುವ ವರ್ಣ *);
ಸ್ಥಿರ ಶೂನ್ಯ (*py3_PyErr_SetObject)(PyObject *, PyObject *);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ (*py3_PyErr_ExceptionMatches)(PyObject *);
# ಯದಿ defined(USE_LIMITED_API) || PY_VERSION_HEX >= 0x03080000
ಸ್ಥಿರ ಶೂನ್ಯ (*py3_Py_IncRef)(PyObject *);
ಸ್ಥಿರ ಶೂನ್ಯ (*py3_Py_DecRef)(PyObject *);
# endif
# ifdef USE_LIMITED_API
ಸ್ಥಿರ PyObject* (*py3_Py_CompileString)(ಧ್ರುವ ವರ್ಣ *, ಧ್ರುವ ವರ್ಣ *, ಪೂರ್ಣಾಙ್ಕ);
ಸ್ಥಿರ PyObject* (*py3_PyEval_EvalCode)(PyObject *co, PyObject *globals, PyObject *locals);
# ಅನ್ಯಥಾ
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ (*py3_PyRun_SimpleString)(ವರ್ಣ *);
ಸ್ಥಿರ PyObject* (*py3_PyRun_String)(ವರ್ಣ *, ಪೂರ್ಣಾಙ್ಕ, PyObject *, PyObject *);
# endif
ಸ್ಥಿರ PyObject* (*py3_PyObject_GetAttrString)(PyObject *, ಧ್ರುವ ವರ್ಣ *);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ (*py3_PyObject_HasAttrString)(PyObject *, ಧ್ರುವ ವರ್ಣ *);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ (*py3_PyObject_SetAttrString)(PyObject *, ಧ್ರುವ ವರ್ಣ *, PyObject *);
ಸ್ಥಿರ PyObject* (*py3_PyObject_CallFunctionObjArgs)(PyObject *, ...);
# ಯದಿ PY_VERSION_HEX >= 0x030d0000
ಸ್ಥಿರ PyObject* (*py3_PyObject_CallFunction)(PyObject *, ವರ್ಣ *, ...);
# ಅನ್ಯಥಾ
ಸ್ಥಿರ PyObject* (*py3__PyObject_CallFunction_SizeT)(PyObject *, ವರ್ಣ *, ...);
# endif
ಸ್ಥಿರ PyObject* (*py3_PyObject_Call)(PyObject *, PyObject *, PyObject *);
ಸ್ಥಿರ PyObject* (*py3_PyEval_GetGlobals)(ಶೂನ್ಯ);
ಸ್ಥಿರ PyObject* (*py3_PyEval_GetLocals)(ಶೂನ್ಯ);
ಸ್ಥಿರ PyObject* (*py3_PyList_GetItem)(PyObject *, Py_ssize_t);
ಸ್ಥಿರ PyObject* (*py3_PyImport_ImportModule)(ಧ್ರುವ ವರ್ಣ *);
ಸ್ಥಿರ PyObject* (*py3_PyImport_AddModule)(ಧ್ರುವ ವರ್ಣ *);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ (*py3_PyErr_BadArgument)(ಶೂನ್ಯ);
ಸ್ಥಿರ PyObject* (*py3_PyErr_Occurred)(ಶೂನ್ಯ);
ಸ್ಥಿರ PyObject* (*py3_PyModule_GetDict)(PyObject *);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ (*py3_PyList_SetItem)(PyObject *, Py_ssize_t, PyObject *);
ಸ್ಥಿರ PyObject* (*py3_PyDict_GetItemString)(PyObject *, ಧ್ರುವ ವರ್ಣ *);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ (*py3_PyDict_Next)(PyObject *, Py_ssize_t *, PyObject **, PyObject **);
ಸ್ಥಿರ PyObject* (*py3_PyLong_FromLong)(long);
ಸ್ಥಿರ PyObject* (*py3_PyDict_New)(ಶೂನ್ಯ);
# ಯದಿ (defined(USE_LIMITED_API) && Py_LIMITED_API >= 0x03080000) || \
       (!defined(USE_LIMITED_API) && PY_VERSION_HEX >= 0x03080000)
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ (*py3_PyIter_Check)(PyObject *o);
# endif
ಸ್ಥಿರ PyObject* (*py3_PyIter_Next)(PyObject *);
ಸ್ಥಿರ PyObject* (*py3_PyObject_GetIter)(PyObject *);
ಸ್ಥಿರ PyObject* (*py3_PyObject_Repr)(PyObject *);
ಸ್ಥಿರ PyObject* (*py3_PyObject_GetItem)(PyObject *, PyObject *);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ (*py3_PyObject_IsTrue)(PyObject *);
ಸ್ಥಿರ PyObject* (*py3_Py_Build_ಮೌಲ್ಯ)(ವರ್ಣ *, ...);
# ಯದಿ PY_VERSION_HEX >= 0x03040000
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ (*py3_PyType_GetFlags)(PyTypeObject *o);
# endif
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ (*py3_PyType_Ready)(PyTypeObject *type);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ (*py3_PyDict_SetItemString)(PyObject *dp, ವರ್ಣ *ಕೀಲಿ, PyObject *item);
ಸ್ಥಿರ PyObject* (*py3_PyUnicode_FromString)(ಧ್ರುವ ವರ್ಣ *u);
# ifdef Py_UNICODE_USE_UCS_FUNCTIONS
#  ifdef Py_UNICODE_WIDE
ಸ್ಥಿರ PyObject* (*py3_PyUnicodeUCS4_FromFormat)(ಧ್ರುವ ವರ್ಣ *u, ...);
#  ಅನ್ಯಥಾ
ಸ್ಥಿರ PyObject* (*py3_PyUnicodeUCS2_FromFormat)(ಧ್ರುವ ವರ್ಣ *u, ...);
#  endif
# ಅನ್ಯಥಾ
ಸ್ಥಿರ PyObject* (*py3_PyUnicode_FromFormat)(ಧ್ರುವ ವರ್ಣ *u, ...);
# endif
ಸ್ಥಿರ PyObject* (*py3_PyUnicode_Decode)(ಧ್ರುವ ವರ್ಣ *u, Py_ssize_t size,
	ಧ್ರುವ ವರ್ಣ *encoding, ಧ್ರುವ ವರ್ಣ *errors);
ಸ್ಥಿರ long (*py3_PyLong_AsLong)(PyObject *);
ಸ್ಥಿರ ಶೂನ್ಯ (*py3_PyErr_SetNone)(PyObject *);
ಸ್ಥಿರ ಶೂನ್ಯ (*py3_PyEval_InitThreads)(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ(*py3_PyEval_RestoreThread)(PyThreadState *);
ಸ್ಥಿರ PyThreadState*(*py3_PyEval_SaveThread)(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ (*py3_PyArg_Parse)(PyObject *, ವರ್ಣ *, ...);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ (*py3_PyArg_ParseTuple)(PyObject *, ವರ್ಣ *, ...);
ಸ್ಥಿರ ಶೂನ್ಯ (*py3_PyMem_Free)(ಶೂನ್ಯ *);
ಸ್ಥಿರ ಶೂನ್ಯ* (*py3_PyMem_Malloc)(size_t);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ (*py3_Py_IsInitialized)(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ (*py3_PyErr_Clear)(ಶೂನ್ಯ);
ಸ್ಥಿರ PyObject* (*py3_PyErr_Format)(PyObject *, ಧ್ರುವ ವರ್ಣ *, ...);
ಸ್ಥಿರ ಶೂನ್ಯ (*py3_PyErr_PrintEx)(ಪೂರ್ಣಾಙ್ಕ);
ಸ್ಥಿರ PyObject*(*py3__PyObject_Init)(PyObject *, PyTypeObject *);
# ಯದಿ !defined(USE_LIMITED_API) && PY_VERSION_HEX < 0x030D0000
ಸ್ಥಿರ iternextfunc py3__PyObject_NextNotImplemented;
# endif
ಸ್ಥಿರ PyObject* py3__Py_NoneStruct;
ಸ್ಥಿರ PyObject* py3__Py_FalseStruct;
ಸ್ಥಿರ PyObject* py3__Py_TrueStruct;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ (*py3_PyModule_AddObject)(PyObject *m, ಧ್ರುವ ವರ್ಣ *name, PyObject *o);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ (*py3_PyImport_AppendInittab)(ಧ್ರುವ ವರ್ಣ *name, PyObject* (*initfunc)(ಶೂನ್ಯ));
# ifdef USE_LIMITED_API
#  ಯದಿ Py_LIMITED_API >= 0x030a0000
ಸ್ಥಿರ ವರ್ಣ* (*py3_PyUnicode_AsUTF8AndSize)(PyObject *unicode, Py_ssize_t *size);
#  endif
# ಅನ್ಯಥಾ
#  ಯದಿ PY_VERSION_HEX >= 0x030300f0
ಸ್ಥಿರ ವರ್ಣ* (*py3_PyUnicode_AsUTF8AndSize)(PyObject *unicode, Py_ssize_t *size);
#  ಅನ್ಯಥಾ
ಸ್ಥಿರ ವರ್ಣ* (*py3__PyUnicode_AsString)(PyObject *unicode);
#  endif
# endif
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ (*py3_PyUnicode_CompareWithASCIIString)(PyObject *unicode, ಧ್ರುವ ವರ್ಣ* string);
ಸ್ಥಿರ PyObject* (*py3_PyUnicode_AsEncodedString)(PyObject *unicode, ಧ್ರುವ ವರ್ಣ* encoding, ಧ್ರುವ ವರ್ಣ* errors);
ಸ್ಥಿರ PyObject* (*py3_PyUnicode_AsUTF8String)(PyObject *unicode);
ಸ್ಥಿರ ವರ್ಣ* (*py3_PyBytes_AsString)(PyObject *bytes);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ (*py3_PyBytes_AsStringAndSize)(PyObject *bytes, ವರ್ಣ **buffer, Py_ssize_t *length);
ಸ್ಥಿರ PyObject* (*py3_PyBytes_FromString)(ವರ್ಣ *str);
ಸ್ಥಿರ PyObject* (*py3_PyBytes_FromStringAndSize)(ವರ್ಣ *str, Py_ssize_t length);
# ಯದಿ PY_VERSION_HEX >= 0x030900b0
ಸ್ಥಿರ PyObject* (*py3__PyObject_New)(PyTypeObject *);
# endif
ಸ್ಥಿರ PyObject* (*py3_PyFloat_FromDouble)(ದ್ವಿಗುಣ ಅಂಕೆ);
ಸ್ಥಿರ ದ್ವಿಗುಣ (*py3_PyFloat_AsDouble)(PyObject *);
ಸ್ಥಿರ PyObject* (*py3_PyObject_GenericGetAttr)(PyObject *obj, PyObject *name);
ಸ್ಥಿರ PyObject* (*py3_PyType_GenericAlloc)(PyTypeObject *type, Py_ssize_t nitems);
ಸ್ಥಿರ PyObject* (*py3_PyType_GenericNew)(PyTypeObject *type, PyObject *args, PyObject *kwds);
ಸ್ಥಿರ PyTypeObject* py3_PyType_Type;
# ifndef USE_LIMITED_API
ಸ್ಥಿರ PyTypeObject* py3_PyStdPrinter_Type;
# endif
ಸ್ಥಿರ PyTypeObject* py3_PySlice_Type;
ಸ್ಥಿರ PyTypeObject* py3_PyFloat_Type;
ಸ್ಥಿರ PyTypeObject* py3_PyBool_Type;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ (*py3_Py_ಸಂಖ್ಯೆ_Check)(PyObject *);
ಸ್ಥಿರ PyObject* (*py3_Py_ಸಂಖ್ಯೆ_Long)(PyObject *);
ಸ್ಥಿರ PyObject* (*py3_PyErr_NewException)(ವರ್ಣ *name, PyObject *base, PyObject *dict);
ಸ್ಥಿರ PyObject* (*py3_PyCapsule_New)(ಶೂನ್ಯ *, ವರ್ಣ *, PyCapsule_Destructor);
ಸ್ಥಿರ ಶೂನ್ಯ* (*py3_PyCapsule_GetPointer)(PyObject *, ವರ್ಣ *);
# ifdef Py_DEBUG
ಸ್ಥಿರ ಶೂನ್ಯ (*py3__Py_NegativeRefcount)(ಧ್ರುವ ವರ್ಣ *fname, ಪೂರ್ಣಾಙ್ಕ lineno, PyObject *op);
ಸ್ಥಿರ Py_ssize_t* py3__Py_RefTotal;
ಸ್ಥಿರ PyObject* (*py3_PyModule_Create2TraceRefs)(ಸಂರಚನಾ PyModuleDef* module, ಪೂರ್ಣಾಙ್ಕ module_api_version);
# ಅನ್ಯಥಾ
ಸ್ಥಿರ PyObject* (*py3_PyModule_Create2)(ಸಂರಚನಾ PyModuleDef* module, ಪೂರ್ಣಾಙ್ಕ module_api_version);
# endif
# ಯದಿ defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
ಸ್ಥಿರ ಶೂನ್ಯ (*py3__PyObject_DebugFree)(ಶೂನ್ಯ*);
ಸ್ಥಿರ ಶೂನ್ಯ* (*py3__PyObject_DebugMalloc)(size_t);
# ಅನ್ಯಥಾ
ಸ್ಥಿರ ಶೂನ್ಯ (*py3_PyObject_Free)(ಶೂನ್ಯ*);
ಸ್ಥಿರ ಶೂನ್ಯ* (*py3_PyObject_Malloc)(size_t);
# endif
ಸ್ಥಿರ PyObject*(*py3__PyObject_GC_New)(PyTypeObject *);
ಸ್ಥಿರ ಶೂನ್ಯ(*py3_PyObject_GC_Del)(ಶೂನ್ಯ *);
ಸ್ಥಿರ ಶೂನ್ಯ(*py3_PyObject_GC_UnTrack)(ಶೂನ್ಯ *);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ (*py3_PyType_IsSubtype)(PyTypeObject *, PyTypeObject *);
# ifdef USE_LIMITED_API
ಸ್ಥಿರ ಶೂನ್ಯ* (*py3_PyType_GetSlot)(PyTypeObject *, ಪೂರ್ಣಾಙ್ಕ);
ಸ್ಥಿರ PyObject* (*py3_PyType_FromSpec)(PyType_Spec *);
# endif

// Imported exception objects
ಸ್ಥಿರ PyObject *p3imp_PyExc_AttributeError;
ಸ್ಥಿರ PyObject *p3imp_PyExc_IndexError;
ಸ್ಥಿರ PyObject *p3imp_Py_Exc_ಕೀಲಿ_Error;
ಸ್ಥಿರ PyObject *p3imp_PyExc_KeyboardInterrupt;
ಸ್ಥಿರ PyObject *p3imp_PyExc_TypeError;
ಸ್ಥಿರ PyObject *p3imp_Py_Exc_ಮೌಲ್ಯ_Error;
ಸ್ಥಿರ PyObject *p3imp_PyExc_SystemExit;
ಸ್ಥಿರ PyObject *p3imp_PyExc_RuntimeError;
ಸ್ಥಿರ PyObject *p3imp_PyExc_ImportError;
ಸ್ಥಿರ PyObject *p3imp_PyExc_OverflowError;

# define PyExc_AttributeError p3imp_PyExc_AttributeError
# define PyExc_IndexError p3imp_PyExc_IndexError
# define Py_Exc_ಕೀಲಿ_Error p3imp_Py_Exc_ಕೀಲಿ_Error
# define PyExc_KeyboardInterrupt p3imp_PyExc_KeyboardInterrupt
# define PyExc_TypeError p3imp_PyExc_TypeError
# define Py_Exc_ಮೌಲ್ಯ_Error p3imp_Py_Exc_ಮೌಲ್ಯ_Error
# define PyExc_SystemExit p3imp_PyExc_SystemExit
# define PyExc_RuntimeError p3imp_PyExc_RuntimeError
# define PyExc_ImportError p3imp_PyExc_ImportError
# define PyExc_OverflowError p3imp_PyExc_OverflowError

/*
 * Table of name to function pointer of python.
 */
# define PYTHON_PROC FARPROC
ಸ್ಥಿರ ಸಂರಚನಾ
{
    ವರ್ಣ *name;
    PYTHON_PROC *ptr;
} py3_funcname_table[] =
{
    {"PySys_SetArgv", (PYTHON_PROC*)&py3_PySys_SetArgv},
    {"Py_SetPythonHome", (PYTHON_PROC*)&py3_Py_SetPythonHome},
    {"Py_Initialize", (PYTHON_PROC*)&py3_Py_Initialize},
    {"_PyArg_ParseTuple_SizeT", (PYTHON_PROC*)&py3_PyArg_ParseTuple},
    {"_Py_BuildValue_SizeT", (PYTHON_PROC*)&py3_Py_Build_ಮೌಲ್ಯ},
    {"PyMem_Free", (PYTHON_PROC*)&py3_PyMem_Free},
    {"PyMem_Malloc", (PYTHON_PROC*)&py3_PyMem_Malloc},
    {"PyList_New", (PYTHON_PROC*)&py3_PyList_New},
    {"PyGILState_Ensure", (PYTHON_PROC*)&py3_PyGILState_Ensure},
    {"PyGILState_Release", (PYTHON_PROC*)&py3_PyGILState_Release},
    {"PySys_SetObject", (PYTHON_PROC*)&py3_PySys_SetObject},
    {"PySys_GetObject", (PYTHON_PROC*)&py3_PySys_GetObject},
    {"PyList_Append", (PYTHON_PROC*)&py3_PyList_Append},
    {"PyList_Insert", (PYTHON_PROC*)&py3_Py_List_ಸೇರಿಸು},
    {"PyList_Size", (PYTHON_PROC*)&py3_PyList_Size},
    {"PyTuple_New", (PYTHON_PROC*)&py3_PyTuple_New},
    {"PyTuple_GetItem", (PYTHON_PROC*)&py3_PyTuple_GetItem},
    {"PyTuple_SetItem", (PYTHON_PROC*)&py3_PyTuple_SetItem},
    {"PyTuple_Size", (PYTHON_PROC*)&py3_PyTuple_Size},
    {"PySequence_Check", (PYTHON_PROC*)&py3_PySequence_Check},
    {"PySequence_Size", (PYTHON_PROC*)&py3_PySequence_Size},
    {"PySequence_GetItem", (PYTHON_PROC*)&py3_PySequence_GetItem},
    {"PySequence_Fast", (PYTHON_PROC*)&py3_PySequence_Fast},
# ಯದಿ PY_VERSION_HEX >= 0x030601f0
    {"PySlice_AdjustIndices", (PYTHON_PROC*)&py3_PySlice_AdjustIndices},
    {"PySlice_Unpack", (PYTHON_PROC*)&py3_PySlice_Unpack},
# endif
    {"PySlice_GetIndicesEx", (PYTHON_PROC*)&py3_PySlice_GetIndicesEx},
    {"PyErr_NoMemory", (PYTHON_PROC*)&py3_PyErr_NoMemory},
    {"Py_Finalize", (PYTHON_PROC*)&py3_Py_Finalize},
    {"PyErr_SetString", (PYTHON_PROC*)&py3_PyErr_SetString},
    {"PyErr_SetObject", (PYTHON_PROC*)&py3_PyErr_SetObject},
    {"PyErr_ExceptionMatches", (PYTHON_PROC*)&py3_PyErr_ExceptionMatches},
# ಯದಿ defined(USE_LIMITED_API) || PY_VERSION_HEX >= 0x03080000
    {"Py_IncRef", (PYTHON_PROC*)&py3_Py_IncRef},
    {"Py_DecRef", (PYTHON_PROC*)&py3_Py_DecRef},
# endif
# ifdef USE_LIMITED_API
    {"Py_CompileString", (PYTHON_PROC*)&py3_Py_CompileString},
    {"PyEval_EvalCode", (PYTHON_PROC*)&PyEval_EvalCode},
# ಅನ್ಯಥಾ
    {"PyRun_SimpleString", (PYTHON_PROC*)&py3_PyRun_SimpleString},
    {"PyRun_String", (PYTHON_PROC*)&py3_PyRun_String},
# endif
    {"PyObject_GetAttrString", (PYTHON_PROC*)&py3_PyObject_GetAttrString},
    {"PyObject_HasAttrString", (PYTHON_PROC*)&py3_PyObject_HasAttrString},
    {"PyObject_SetAttrString", (PYTHON_PROC*)&py3_PyObject_SetAttrString},
    {"PyObject_CallFunctionObjArgs", (PYTHON_PROC*)&py3_PyObject_CallFunctionObjArgs},
# ಯದಿ PY_VERSION_HEX >= 0x030d0000
    {"PyObject_CallFunction", (PYTHON_PROC*)&py3_PyObject_CallFunction},
# ಅನ್ಯಥಾ
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
    {"PyMapping_Keys", (PYTHON_PROC*)&py3_Py_Mapping_ಕೀಲಿಗಳು},
# ಯದಿ (defined(USE_LIMITED_API) && Py_LIMITED_API >= 0x03080000) || \
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
# ಯದಿ PY_VERSION_HEX >= 0x03040000
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
# ಯದಿ !defined(USE_LIMITED_API) && PY_VERSION_HEX < 0x030D0000
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
#  ಯದಿ Py_LIMITED_API >= 0x030a0000
    {"PyUnicode_AsUTF8AndSize", (PYTHON_PROC*)&py3_PyUnicode_AsUTF8AndSize},
#  endif
# ಅನ್ಯಥಾ
#  ಯದಿ PY_VERSION_HEX >= 0x030300f0
    {"PyUnicode_AsUTF8AndSize", (PYTHON_PROC*)&py3_PyUnicode_AsUTF8AndSize},
#  ಅನ್ಯಥಾ
    {"_PyUnicode_AsString", (PYTHON_PROC*)&py3__PyUnicode_AsString},
#  endif
# endif
    {"PyUnicode_CompareWithASCIIString", (PYTHON_PROC*)&py3_PyUnicode_CompareWithASCIIString},
    {"PyUnicode_AsUTF8String", (PYTHON_PROC*)&py3_PyUnicode_AsUTF8String},
# ifdef Py_UNICODE_USE_UCS_FUNCTIONS
#  ifdef Py_UNICODE_WIDE
    {"PyUnicodeUCS4_FromFormat", (PYTHON_PROC*)&py3_PyUnicodeUCS4_FromFormat},
#  ಅನ್ಯಥಾ
    {"PyUnicodeUCS2_FromFormat", (PYTHON_PROC*)&py3_PyUnicodeUCS2_FromFormat},
#  endif
# ಅನ್ಯಥಾ
    {"PyUnicode_FromFormat", (PYTHON_PROC*)&py3_PyUnicode_FromFormat},
# endif
    {"PyBytes_AsString", (PYTHON_PROC*)&py3_PyBytes_AsString},
    {"PyBytes_AsStringAndSize", (PYTHON_PROC*)&py3_PyBytes_AsStringAndSize},
    {"PyBytes_FromString", (PYTHON_PROC*)&py3_PyBytes_FromString},
    {"PyBytes_FromStringAndSize", (PYTHON_PROC*)&py3_PyBytes_FromStringAndSize},
# ಯದಿ PY_VERSION_HEX >= 0x030900b0
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
    {"PyNumber_Check", (PYTHON_PROC*)&py3_Py_ಸಂಖ್ಯೆ_Check},
    {"PyNumber_Long", (PYTHON_PROC*)&py3_Py_ಸಂಖ್ಯೆ_Long},
    {"PyErr_NewException", (PYTHON_PROC*)&py3_PyErr_NewException},
# ifdef Py_DEBUG
    {"_Py_NegativeRefcount", (PYTHON_PROC*)&py3__Py_NegativeRefcount},
    {"_Py_RefTotal", (PYTHON_PROC*)&py3__Py_RefTotal},
    {"PyModule_Create2TraceRefs", (PYTHON_PROC*)&py3_PyModule_Create2TraceRefs},
# ಅನ್ಯಥಾ
    {"PyModule_Create2", (PYTHON_PROC*)&py3_PyModule_Create2},
# endif
# ಯದಿ defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
    {"_PyObject_DebugFree", (PYTHON_PROC*)&py3__PyObject_DebugFree},
    {"_PyObject_DebugMalloc", (PYTHON_PROC*)&py3__PyObject_DebugMalloc},
# ಅನ್ಯಥಾ
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
#  ಯದಿ PY_VERSION_HEX >= 0x03040000
    {"PyType_GetSlot", (PYTHON_PROC*)&py3_PyType_GetSlot},
#  endif
    {"PyType_FromSpec", (PYTHON_PROC*)&py3_PyType_FromSpec},
# endif
    {"", ರಿಕ್ತ},
};

# ಯದಿ defined(USE_LIMITED_API) || PY_VERSION_HEX >= 0x03080000
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

# ಯದಿ PY_VERSION_HEX >= 0x030900b0
    ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ
py3_PyType_HasFeature(PyTypeObject *type, unsigned long feature)
{
    ಪ್ರತ್ಯಾಗಮ ((PyType_GetFlags(type) & feature) != 0);
}
#  define PyType_HasFeature(t,f) py3_PyType_HasFeature(t,f)
# endif

# ಯದಿ PY_VERSION_HEX >= 0x030a00b2
    ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ
py3__PyObject_TypeCheck(PyObject *ob, PyTypeObject *type)
{
    ಪ್ರತ್ಯಾಗಮ Py_IS_TYPE(ob, type) || PyType_IsSubtype(Py_TYPE(ob), type);
}
#  ಯದಿ PY_VERSION_HEX >= 0x030b00b3
#   undef PyObject_TypeCheck
#   define PyObject_TypeCheck(o,t) py3__PyObject_TypeCheck(o,t)
#  ಅನ್ಯಥಾ
#   define _PyObject_TypeCheck(o,t) py3__PyObject_TypeCheck(o,t)
#  endif
# endif

# ಯದಿ !defined(USE_LIMITED_API) && PY_VERSION_HEX >= 0x030c00b0
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
    ಸ್ಥಿರ WCHAR *
py3_get_system_libname(ಧ್ರುವ ವರ್ಣ *libname)
{
    ಧ್ರುವ WCHAR	*pythoncore = L"Software\\Python\\PythonCore";
    ಧ್ರುವ ವರ್ಣ	*cp = libname;
    WCHAR	subkey[128];
    HKEY	h_ಕೀಲಿ;
    ಪೂರ್ಣಾಙ್ಕ		i;
    DWORD	j, len;
    LSTATUS	ret;

    ಯಾವತ್ (*cp != '\0')
    {
	ಯದಿ (*cp == ':' || *cp == '\\' || *cp == '/')
	{
	    // Bail out if "libname" contains path separator, assume it is
	    // an absolute path.
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
	++cp;
    }

    WCHAR   keyfound[32];
    HKEY    h_ಕೀಲಿ_ತುದಿ[] = {HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE};
    HKEY    h_ಕೀಲಿ_Found = ರಿಕ್ತ;
#  ifdef USE_LIMITED_API
    long    maxminor = -1;
#  endif
    ಪ್ರತಿ (i = 0; i < ARRAY_LENGTH(h_ಕೀಲಿ_ತುದಿ); i++)
    {
	long	major, minor;

	ret = Reg_Open_ಕೀಲಿ_Ex_W(h_ಕೀಲಿ_ತುದಿ[i], pythoncore, 0, ಕೀಲಿ_READ, &h_ಕೀಲಿ);
	ಯದಿ (ret != ERROR_SUCCESS)
	    ಅನುವರ್ತ;
	ಪ್ರತಿ (j = 0;; j++)
	{
	    WCHAR   keyname[32];
	    WCHAR   *wp;

	    len = ARRAY_LENGTH(keyname);
	    ret = Reg_Enum_ಕೀಲಿ_Ex_W(h_ಕೀಲಿ, j, keyname, &len,
						    ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);
	    ಯದಿ (ret == ERROR_NO_MORE_ವಸ್ತುಗಳು)
		ಭಙ್ಗ;

	    major = wcstol(keyname, &wp, 10);
	    ಯದಿ (*wp != L'.')
		ಅನುವರ್ತ;
	    minor = wcstol(wp + 1, &wp, 10);
#  ifdef _WIN64
	    ಯದಿ (*wp != L'\0')
		ಅನುವರ್ತ;
#  ಅನ್ಯಥಾ
	    ಯದಿ (wcscmp(wp, L"-32") != 0)
		ಅನುವರ್ತ;
#  endif

	    ಯದಿ (major != PY_MAJOR_VERSION)
		ಅನುವರ್ತ;
#  ifdef USE_LIMITED_API
	    // Search the latest version.
	    ಯದಿ ((minor > maxminor)
		    && (minor >= ((Py_LIMITED_API >> 16) & 0xff)))
	    {
		maxminor = minor;
		wcscpy(keyfound, keyname);
		h_ಕೀಲಿ_Found = h_ಕೀಲಿ_ತುದಿ[i];
	    }
#  ಅನ್ಯಥಾ
	    // Check if it matches with the compiled version.
	    ಯದಿ (minor == PY_MINOR_VERSION)
	    {
		wcscpy(keyfound, keyname);
		h_ಕೀಲಿ_Found = h_ಕೀಲಿ_ತುದಿ[i];
		ಭಙ್ಗ;
	    }
#  endif
	}
	Reg_Close_ಕೀಲಿ(h_ಕೀಲಿ);
#  ifdef USE_LIMITED_API
	ಯದಿ (h_ಕೀಲಿ_Found != ರಿಕ್ತ)
	    ಭಙ್ಗ;
#  endif
    }
    ಯದಿ (h_ಕೀಲಿ_Found == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    swprintf(subkey, ARRAY_LENGTH(subkey), L"%ls\\%ls\\InstallPath",
							pythoncore, keyfound);
    ret = Reg_Get_ಮೌಲ್ಯ_W(h_ಕೀಲಿ_Found, subkey, ರಿಕ್ತ, RRF_RT_REG_SZ,
							    ರಿಕ್ತ, ರಿಕ್ತ, &len);
    ಯದಿ (ret != ERROR_MORE_DATA && ret != ERROR_SUCCESS)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    size_t len2 = len / ಪರಿಮಾಣ(WCHAR) + 1 + strlen(libname);
    WCHAR *path = alloc(len2 * ಪರಿಮಾಣ(WCHAR));
    ಯದಿ (path == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ret = Reg_Get_ಮೌಲ್ಯ_W(h_ಕೀಲಿ_Found, subkey, ರಿಕ್ತ, RRF_RT_REG_SZ,
							    ರಿಕ್ತ, path, &len);
    ಯದಿ (ret != ERROR_SUCCESS)
    {
	vim_free(path);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    // Remove trailing path separators.
    size_t len3 = wcslen(path);
    ಯದಿ ((len3 > 0) && (path[len3 - 1] == L'/' || path[len3 - 1] == L'\\'))
	--len3;
    swprintf(path + len3, len2 - len3, L"\\%hs", libname);
    ಪ್ರತ್ಯಾಗಮ path;
}
# endif

/*
 * Load library and get all pointers.
 * Parameter 'libname' provides name of DLL.
 * Return OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
py3_runtime_link_init(ವರ್ಣ *libname, ಪೂರ್ಣಾಙ್ಕ verbose)
{
    ಪೂರ್ಣಾಙ್ಕ i;
    PYTHON_PROC *ucs_from_string = (PYTHON_PROC *)&py3_PyUnicode_FromString;
    PYTHON_PROC *ucs_decode = (PYTHON_PROC *)&py3_PyUnicode_Decode;
    PYTHON_PROC *ucs_as_encoded_string =
				 (PYTHON_PROC *)&py3_PyUnicode_AsEncodedString;

# ಯದಿ !(defined(PY_NO_RTLD_GLOBAL) && defined(PY3_NO_RTLD_GLOBAL)) && defined(UNIX) && defined(FEAT_PYTHON)
    // Can't have Python and Python3 loaded at the same time.
    // It causes a crash, because RTLD_GLOBAL is needed for
    // standard C extension libraries of one or both python versions.
    ಯದಿ (python_loaded())
    {
	ಯದಿ (verbose)
	    emsg(_(e_this_vim_cannot_execute_py3_after_using_python));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
# endif

    ಯದಿ (hinstPy3 != 0)
	ಪ್ರತ್ಯಾಗಮ OK;
    hinstPy3 = load_dll(libname);

# ifdef MSWIN
    ಯದಿ (!hinstPy3)
    {
	// Attempt to use the path from InstallPath as stored in the registry.
	WCHAR *syslibname = py3_get_system_libname(libname);

	ಯದಿ (syslibname != ರಿಕ್ತ)
	{
	    hinstPy3 = LoadLibraryExW(syslibname, ರಿಕ್ತ,
		    LOAD_LIBRARY_ಹುಡುಕು_DLL_LOAD_DIR |
		    LOAD_LIBRARY_ಹುಡುಕು_SYSTEM32);
	    vim_free(syslibname);
	}
    }
# endif

    ಯದಿ (!hinstPy3)
    {
	ಯದಿ (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಪ್ರತಿ (i = 0; py3_funcname_table[i].ptr; ++i)
    {
	ಯದಿ ((*py3_funcname_table[i].ptr = ಚಿಹ್ನೆ_from_dll(hinstPy3,
			py3_funcname_table[i].name)) == ರಿಕ್ತ)
	{
	    close_dll(hinstPy3);
	    hinstPy3 = 0;
	    ಯದಿ (verbose)
		semsg(_(e_could_not_load_library_function_str), py3_funcname_table[i].name);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
    }

    // Load unicode functions separately as only the ucs2 or the ucs4 functions
    // will be present in the library.
# ಯದಿ PY_VERSION_HEX >= 0x030300f0
    *ucs_from_string = ಚಿಹ್ನೆ_from_dll(hinstPy3, "PyUnicode_FromString");
    *ucs_decode = ಚಿಹ್ನೆ_from_dll(hinstPy3, "PyUnicode_Decode");
    *ucs_as_encoded_string = ಚಿಹ್ನೆ_from_dll(hinstPy3,
	    "PyUnicode_AsEncodedString");
# ಅನ್ಯಥಾ
    *ucs_from_string = ಚಿಹ್ನೆ_from_dll(hinstPy3, "PyUnicodeUCS2_FromString");
    *ucs_decode = ಚಿಹ್ನೆ_from_dll(hinstPy3,
	    "PyUnicodeUCS2_Decode");
    *ucs_as_encoded_string = ಚಿಹ್ನೆ_from_dll(hinstPy3,
	    "PyUnicodeUCS2_AsEncodedString");
    ಯದಿ (*ucs_from_string == ರಿಕ್ತ || *ucs_decode == ರಿಕ್ತ
					     || *ucs_as_encoded_string == ರಿಕ್ತ)
    {
	*ucs_from_string = ಚಿಹ್ನೆ_from_dll(hinstPy3,
		"PyUnicodeUCS4_FromString");
	*ucs_decode = ಚಿಹ್ನೆ_from_dll(hinstPy3,
		"PyUnicodeUCS4_Decode");
	*ucs_as_encoded_string = ಚಿಹ್ನೆ_from_dll(hinstPy3,
		"PyUnicodeUCS4_AsEncodedString");
    }
# endif
    ಯದಿ (*ucs_from_string == ರಿಕ್ತ || *ucs_decode == ರಿಕ್ತ
					     || *ucs_as_encoded_string == ರಿಕ್ತ)
    {
	close_dll(hinstPy3);
	hinstPy3 = 0;
	ಯದಿ (verbose)
	    semsg(_(e_could_not_load_library_function_str), "PyUnicode_UCSX_*");
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * If python is enabled (there is installed python on Windows system) return
 * TRUE, else FALSE.
 */
    ಪೂರ್ಣಾಙ್ಕ
python3_enabled(ಪೂರ್ಣಾಙ್ಕ verbose)
{
    ಪ್ರತ್ಯಾಗಮ py3_runtime_link_init((ವರ್ಣ *)p_py3dll, verbose) == OK;
}

/*
 * Load the standard Python exceptions - don't import the symbols from the
 * DLL, as this can cause errors (importing data symbols is not reliable).
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_py3_exceptions(ಶೂನ್ಯ)
{
    PyObject *exmod = PyImport_ImportModule("builtins");
    PyObject *exdict = PyModule_GetDict(exmod);
    p3imp_PyExc_AttributeError = PyDict_GetItemString(exdict, "AttributeError");
    p3imp_PyExc_IndexError = PyDict_GetItemString(exdict, "IndexError");
    p3imp_Py_Exc_ಕೀಲಿ_Error = PyDict_GetItemString(exdict, "KeyError");
    p3imp_PyExc_KeyboardInterrupt = PyDict_GetItemString(exdict, "KeyboardInterrupt");
    p3imp_PyExc_TypeError = PyDict_GetItemString(exdict, "TypeError");
    p3imp_Py_Exc_ಮೌಲ್ಯ_Error = PyDict_GetItemString(exdict, "ValueError");
    p3imp_PyExc_SystemExit = PyDict_GetItemString(exdict, "SystemExit");
    p3imp_PyExc_RuntimeError = PyDict_GetItemString(exdict, "RuntimeError");
    p3imp_PyExc_ImportError = PyDict_GetItemString(exdict, "ImportError");
    p3imp_PyExc_OverflowError = PyDict_GetItemString(exdict, "OverflowError");
    Py_XINCREF(p3imp_PyExc_AttributeError);
    Py_XINCREF(p3imp_PyExc_IndexError);
    Py_XINCREF(p3imp_Py_Exc_ಕೀಲಿ_Error);
    Py_XINCREF(p3imp_PyExc_KeyboardInterrupt);
    Py_XINCREF(p3imp_PyExc_TypeError);
    Py_XINCREF(p3imp_Py_Exc_ಮೌಲ್ಯ_Error);
    Py_XINCREF(p3imp_PyExc_SystemExit);
    Py_XINCREF(p3imp_PyExc_RuntimeError);
    Py_XINCREF(p3imp_PyExc_ImportError);
    Py_XINCREF(p3imp_PyExc_OverflowError);
    Py_XDECREF(exmod);
}
#endif // DYNAMIC_PYTHON3

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ py3initialised = 0;
#define PYINITIALISED py3initialised
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ python_end_called = FALSE;

#ifdef USE_LIMITED_API
# define DESTRUCTOR_FINISH(self) \
    ((freefunc)PyType_GetSlot(Py_TYPE((PyObject*)self), Py_tp_free))((PyObject*)self)
#ಅನ್ಯಥಾ
# define DESTRUCTOR_FINISH(self) Py_TYPE(self)->tp_free((PyObject*)self)
#endif

#define WIN_PYTHON_REF(win) win->w_python3_ref
#define BUF_PYTHON_REF(buf) buf->b_python3_ref
#define TAB_PYTHON_REF(tab) tab->tp_python3_ref

    ಸ್ಥಿರ ಶೂನ್ಯ
call_PyObject_Free(ಶೂನ್ಯ *p)
{
#ಯದಿ defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
    _PyObject_DebugFree(p);
#ಅನ್ಯಥಾ
    PyObject_Free(p);
#endif
}

    ಸ್ಥಿರ PyObject *
call_PyType_GenericNew(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    ಪ್ರತ್ಯಾಗಮ PyType_GenericNew(type,args,kwds);
}

    ಸ್ಥಿರ PyObject *
call_PyType_GenericAlloc(PyTypeObject *type, Py_ssize_t nitems)
{
    ಪ್ರತ್ಯಾಗಮ PyType_GenericAlloc(type,nitems);
}

ಸ್ಥಿರ PyObject *OutputGetattro(PyObject *, PyObject *);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ OutputSetattro(PyObject *, PyObject *, PyObject *);
ಸ್ಥಿರ PyObject *BufferGetattro(PyObject *, PyObject *);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ BufferSetattro(PyObject *, PyObject *, PyObject *);
ಸ್ಥಿರ PyObject *TabPageGetattro(PyObject *, PyObject *);
ಸ್ಥಿರ PyObject *WindowGetattro(PyObject *, PyObject *);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ WindowSetattro(PyObject *, PyObject *, PyObject *);
ಸ್ಥಿರ PyObject *RangeGetattro(PyObject *, PyObject *);
ಸ್ಥಿರ PyObject *CurrentGetattro(PyObject *, PyObject *);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ CurrentSetattro(PyObject *, PyObject *, PyObject *);
ಸ್ಥಿರ PyObject *DictionaryGetattro(PyObject *, PyObject *);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ DictionarySetattro(PyObject *, PyObject *, PyObject *);
ಸ್ಥಿರ PyObject *ListGetattro(PyObject *, PyObject *);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ListSetattro(PyObject *, PyObject *, PyObject *);
ಸ್ಥಿರ PyObject *TupleGetattro(PyObject *, PyObject *);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ TupleSetattro(PyObject *, PyObject *, PyObject *);
ಸ್ಥಿರ PyObject *FunctionGetattro(PyObject *, PyObject *);

ಸ್ಥಿರ ಸಂರಚನಾ PyModuleDef vimmodule;

#define PY_CAN_RECURSE

/*
 * Include the code shared with if_python.c
 */
#ಅನ್ತರ್ಭಾವ "if_py_both.h"

#ifdef USE_LIMITED_API
# ಯದಿ Py_LIMITED_API >= 0x030A0000
#  define PY_UNICODE_GET_UTF8_CHARS(obj) PyUnicode_AsUTF8AndSize(obj, ರಿಕ್ತ)
# ಅನ್ಯಥಾ
// Python limited API before 3.10 lack easy ways to query the raw UTF-8 chars.
// We need to first convert the string to bytes, and then extract the chars.
// This function is only used for attribute string comparisons, which have
// known short length. As such, just allocate a short static buffer to hold
// the characters instead of having to allocate/deallcoate it.
//
// An alternative would be to convert all attribute string comparisons to use
// PyUnicode_CompareWithASCIIString to skip having to extract the chars.
ಸ್ಥಿರ ವರ್ಣ py3_unicode_utf8_chars[20];
ಸ್ಥಿರ ವರ್ಣ* PY_UNICODE_GET_UTF8_CHARS(PyObject* str)
{
    py3_unicode_utf8_chars[0] = '\0';
    PyObject* bytes = PyUnicode_AsUTF8String(str);
    ಯದಿ (bytes)
    {
	ವರ್ಣ *chars;
	Py_ssize_t len;
	ಯದಿ (PyBytes_AsStringAndSize(bytes, &chars, &len) != -1)
	{
	    ಯದಿ (len < (Py_ssize_t)ಪರಿಮಾಣ(py3_unicode_utf8_chars))
		// PyBytes_AsStringAndSize guarantees null-termination
		memcpy(py3_unicode_utf8_chars, chars, len + 1);
	}
	Py_DECREF(bytes);
    }
    ಪ್ರತ್ಯಾಗಮ py3_unicode_utf8_chars;
}
# endif
#ಅನ್ಯಥಾ	// !USE_LIMITED_API
# ಯದಿ PY_VERSION_HEX >= 0x030300f0
#  define PY_UNICODE_GET_UTF8_CHARS(obj) PyUnicode_AsUTF8AndSize(obj, ರಿಕ್ತ)
# ಅನ್ಯಥಾ
#  define PY_UNICODE_GET_UTF8_CHARS _PyUnicode_AsString
# endif
#endif

// NOTE: Must always be used at the start of a block, since it declares "name".
#define GET_ATTR_STRING(name, nameobj) \
    ವರ್ಣ	*name = ""; \
    ಯದಿ (PyUnicode_Check(nameobj)) \
	name = (ವರ್ಣ *)PY_UNICODE_GET_UTF8_CHARS(nameobj)

#define PY3OBJ_DELETED(obj) (obj->ob_base.ob_refcnt<=0)

///////////////////////////////////////////////////////
// Internal function prototypes.

ಸ್ಥಿರ PyObject *Py3Init_vim(ಶೂನ್ಯ);

///////////////////////////////////////////////////////
// 1. Python interpreter main program.

    ಶೂನ್ಯ
python3_end(ಶೂನ್ಯ)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ recurse = 0;

    // If a crash occurs while doing this, don't try again.
    ಯದಿ (recurse != 0)
	ಪ್ರತ್ಯಾಗಮ;

    python_end_called = TRUE;
    ++recurse;

#ifdef DYNAMIC_PYTHON3
    ಯದಿ (hinstPy3)
#endif
    ಯದಿ (Py_IsInitialized())
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

#ಯದಿ defined(DYNAMIC_PYTHON3) && defined(DYNAMIC_PYTHON) && defined(FEAT_PYTHON) && defined(UNIX)
    ಪೂರ್ಣಾಙ್ಕ
python3_loaded(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ (hinstPy3 != 0);
}
#endif

ಸ್ಥಿರ wchar_t *py_home_buf = ರಿಕ್ತ;

#ಯದಿ defined(MSWIN) && (PY_VERSION_HEX >= 0x030500f0)
/*
 * Return TRUE if stdin is readable from Python 3.
 */
    ಸ್ಥಿರ BOOL
is_stdin_readable(ಶೂನ್ಯ)
{
    DWORD	    mode, eventnum;
    ಸಂರಚನಾ _stat    st;
    ಪೂರ್ಣಾಙ್ಕ		    fd = fileno(stdin);
    HANDLE	    hstdin = (HANDLE)_get_osfhandle(fd);

    // Check if stdin is connected to the console.
    ಯದಿ (GetConsoleMode(hstdin, &mode))
	// Check if it is opened as input.
	ಪ್ರತ್ಯಾಗಮ Get_ಸಂಖ್ಯೆ_Of_Console_ಒಳಹರಿವು_Events(hstdin, &eventnum);

    ಪ್ರತ್ಯಾಗಮ _fstat(fd, &st) == 0;
}

// Python 3.5 or later will abort inside Py_Initialize() when stdin has
// been closed (i.e. executed by "vim -").  Reconnect stdin to CONIN$.
// Note that the python DLL is linked to its own stdio DLL which can be
// differ from Vim's stdio.
    ಸ್ಥಿರ ಶೂನ್ಯ
reset_stdin(ಶೂನ್ಯ)
{
    FILE *(*py__acrt_iob_func)(unsigned) = ರಿಕ್ತ;
    FILE *(*pyfreopen)(ಧ್ರುವ ವರ್ಣ *, ಧ್ರುವ ವರ್ಣ *, FILE *) = ರಿಕ್ತ;
    HINSTANCE hinst = get_forwarded_dll(hinstPy3);

    ಯದಿ (hinst == ರಿಕ್ತ || is_stdin_readable())
	ಪ್ರತ್ಯಾಗಮ;

    // Get "freopen" and "stdin" which are used in the python DLL.
    // "stdin" is defined as "__acrt_iob_func(0)" in VC++ 2015 or later.
    py__acrt_iob_func = get_dll_import_func(hinst, "__acrt_iob_func");
    ಯದಿ (py__acrt_iob_func)
    {
	HINSTANCE hpystdiodll = find_imported_module_by_funcname(hinst,
							    "__acrt_iob_func");
	ಯದಿ (hpystdiodll)
	    pyfreopen = (ಶೂನ್ಯ *)GetProcAddress(hpystdiodll, "freopen");
    }

    // Reconnect stdin to CONIN$.
    ಯದಿ (pyfreopen != ರಿಕ್ತ)
	pyfreopen("CONIN$", "r", py__acrt_iob_func(0));
    ಅನ್ಯಥಾ
	freopen("CONIN$", "r", stdin);
}
#ಅನ್ಯಥಾ
# define reset_stdin()
#endif

// Python 3.2 or later will abort inside Py_Initialize() when mandatory
// modules cannot be loaded (e.g. 'pythonthreehome' is wrongly set.).
// Install a hook to python dll's exit() and recover from it.
#ಯದಿ defined(MSWIN) && (PY_VERSION_HEX >= 0x030200f0)
# define HOOK_EXIT
# include <setjmp.h>

ಸ್ಥಿರ jmp_buf exit_hook_jump_buf;
ಸ್ಥಿರ ಶೂನ್ಯ *orig_exit = ರಿಕ್ತ;

/*
 * Function that replaces exit() while calling Py_Initialize().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
hooked_exit(ಪೂರ್ಣಾಙ್ಕ ret)
{
    // Recover from exit.
    longjmp(exit_hook_jump_buf, 1);
}

/*
 * Install a hook to python dll's exit().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
hook_py_exit(ಶೂನ್ಯ)
{
    HINSTANCE hinst = get_forwarded_dll(hinstPy3);

    ಯದಿ (hinst == ರಿಕ್ತ || orig_exit != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    orig_exit = hook_dll_import_func(hinst, "exit", (ಶೂನ್ಯ *)hooked_exit);
}

/*
 * Remove the hook installed by hook_py_exit().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
restore_py_exit(ಶೂನ್ಯ)
{
    HINSTANCE hinst = hinstPy3;

    ಯದಿ (hinst == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (orig_exit != ರಿಕ್ತ)
	hook_dll_import_func(hinst, "exit", orig_exit);
    orig_exit = ರಿಕ್ತ;
}
#endif

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
Python3_Init(ಶೂನ್ಯ)
{
    ಯದಿ (!py3initialised)
    {
#ifdef DYNAMIC_PYTHON3
	ಯದಿ (!python3_enabled(TRUE))
	{
	    emsg(_(e_sorry_this_command_is_disabled_python_library_could_not_be_found));
	    ಗಚ್ಛ fail;
	}
#endif
	// Python 3.13 introduced a really useful feature: colorized exceptions.
	// This is great if you're reading them from the terminal, but useless
	// and broken everywhere else (such as in log files, or text editors).
	// Opt out, forcefully.
	vim_setenv((char_u*)"PYTHON_COLORS", (char_u*)"0");

	init_structs();

	ಯದಿ (*p_py3home != NUL)
	{
	    size_t len = mbstowcs(ರಿಕ್ತ, (ವರ್ಣ *)p_py3home, 0) + 1;

	    // The string must not change later, make a copy in static memory.
	    py_home_buf = ALLOC_MULT(wchar_t, len);
	    ಯದಿ (py_home_buf != ರಿಕ್ತ && mbstowcs(
			    py_home_buf, (ವರ್ಣ *)p_py3home, len) != (size_t)-1)
		Py_SetPythonHome(py_home_buf);
	}
#ifdef PYTHON3_HOME
	ಅನ್ಯಥಾ ಯದಿ (mch_getenv((char_u *)"PYTHONHOME") == ರಿಕ್ತ)
	    Py_SetPythonHome(PYTHON3_HOME);
#endif

	PyImport_AppendInittab("vim", Py3Init_vim);

#ಯದಿ !defined(DYNAMIC_PYTHON3) && defined(MSWIN)
	hinstPy3 = GetModuleHandle(PYTHON3_DLL);
#endif
	reset_stdin();

#ifdef HOOK_EXIT
	// Catch exit() called in Py_Initialize().
	hook_py_exit();
	ಯದಿ (setjmp(exit_hook_jump_buf) == 0)
	{
	    Py_Initialize();
	    restore_py_exit();
	}
	ಅನ್ಯಥಾ
	{
	    // exit() was called in Py_Initialize().
	    restore_py_exit();
	    emsg(_(e_critical_error_in_python3_initialization_check_your_installation));
	    ಗಚ್ಛ fail;
	}
#ಅನ್ಯಥಾ
	Py_Initialize();
#endif

#ಯದಿ PY_VERSION_HEX < 0x03090000
	// Initialise threads.  This is deprecated since Python 3.9.
	PyEval_InitThreads();
#endif
#ifdef DYNAMIC_PYTHON3
	get_py3_exceptions();
#endif

	ಯದಿ (PythonIO_Init_io())
	    ಗಚ್ಛ fail;

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

    ಪ್ರತ್ಯಾಗಮ 0;

fail:
    // We call PythonIO_Flush() here to print any Python errors.
    // This is OK, as it is possible to call this function even
    // if PythonIO_Init_io() has not completed successfully (it will
    // not do anything in this case).
    PythonIO_Flush();
    ಪ್ರತ್ಯಾಗಮ -1;
}

/*
 * External interface
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
DoPyCommand(ಧ್ರುವ ವರ್ಣ *cmd,
	    dict_T* locals,
	    rangeinitializer init_range,
	    runner run,
	    ಶೂನ್ಯ *arg)
{
#ಯದಿ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    ವರ್ಣ		*saved_locale;
#endif
    PyObject		*cmdstr;
    PyObject		*cmdbytes;
    PyGILState_STATE	pygilstate;

    ಯದಿ (python_end_called)
	ಗಚ್ಛ theend;

    ಯದಿ (Python3_Init())
	ಗಚ್ಛ theend;

    init_range(arg);

    Python_Release_Vim();	    // leave Vim

#ಯದಿ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    // Python only works properly when the LC_NUMERIC locale is "C".
    saved_locale = setlocale(LC_NUMERIC, ರಿಕ್ತ);
    ಯದಿ (saved_locale == ರಿಕ್ತ || STRCMP(saved_locale, "C") == 0)
	saved_locale = ರಿಕ್ತ;
    ಅನ್ಯಥಾ
    {
	// Need to make a copy, value may change when setting new locale.
	saved_locale = (ವರ್ಣ *)vim_strsave((char_u *)saved_locale);
	(ಶೂನ್ಯ)setlocale(LC_NUMERIC, "C");
    }
#endif

    pygilstate = PyGILState_Ensure();

    // PyRun_SimpleString expects a UTF-8 string. Wrong encoding may cause
    // SyntaxError (unicode error).
    cmdstr = PyUnicode_Decode(cmd, strlen(cmd),
					(ವರ್ಣ *)ENC_OPT, ERRORS_DECODE_ARG);
    cmdbytes = PyUnicode_AsEncodedString(cmdstr, "utf-8", ERRORS_ENCODE_ARG);
    Py_XDECREF(cmdstr);

    run(PyBytes_AsString(cmdbytes), locals, arg, &pygilstate);
    Py_XDECREF(cmdbytes);

    PyGILState_Release(pygilstate);

#ಯದಿ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    ಯದಿ (saved_locale != ರಿಕ್ತ)
    {
	(ಶೂನ್ಯ)setlocale(LC_NUMERIC, saved_locale);
	vim_free(saved_locale);
    }
#endif

    Python_Lock_Vim();		    // enter Vim
    PythonIO_Flush();

theend:
    ಪ್ರತ್ಯಾಗಮ;	    // keeps lint happy
}

/*
 * ":py3"
 */
    ಶೂನ್ಯ
ex_py3(exarg_T *eap)
{
    char_u *script;

    script = script_get(eap, eap->arg);
    ಯದಿ (!eap->skip)
    {
	ಯದಿ (p_pyx == 0)
	    p_pyx = 3;

	DoPyCommand(script == ರಿಕ್ತ ? (ವರ್ಣ *) eap->arg : (ವರ್ಣ *) script,
		ರಿಕ್ತ,
		init_range_cmd,
		(runner) run_cmd,
		(ಶೂನ್ಯ *) eap);
    }
    vim_free(script);
}

#define BUFFER_SIZE 2048

/*
 * ":py3file"
 */
    ಶೂನ್ಯ
ex_py3file(exarg_T *eap)
{
    ಸ್ಥಿರ ವರ್ಣ buffer[BUFFER_SIZE];
    ಧ್ರುವ ವರ್ಣ *file;
    ವರ್ಣ *p;
    ಪೂರ್ಣಾಙ್ಕ i;

    ಯದಿ (p_pyx == 0)
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

    ಪ್ರತಿ (i=0; i<2; ++i)
    {
	file = (ವರ್ಣ *)eap->arg;
	ಯಾವತ್ (*file && p < buffer + (BUFFER_SIZE - 3))
	{
	    ಯದಿ (*file == '\\' || *file == '\'')
		*p++ = '\\';
	    *p++ = *file++;
	}
	// If we didn't finish the file name, we hit a buffer overflow
	ಯದಿ (*file != '\0')
	    ಪ್ರತ್ಯಾಗಮ;
	ಯದಿ (i==0)
	{
	    strcpy(p,"','rb').read(),'");
	    p += 16;
	}
	ಅನ್ಯಥಾ
	{
	    strcpy(p,"','exec'))");
	    p += 10;
	}
    }


    // Execute the file
    DoPyCommand(buffer,
	    ರಿಕ್ತ,
	    init_range_cmd,
	    (runner) run_cmd,
	    (ಶೂನ್ಯ *) eap);
}

    ಶೂನ್ಯ
ex_py3do(exarg_T *eap)
{
    ಯದಿ (p_pyx == 0)
	p_pyx = 3;

    DoPyCommand((ವರ್ಣ *)eap->arg,
	    ರಿಕ್ತ,
	    init_range_cmd,
	    (runner)run_do,
	    (ಶೂನ್ಯ *)eap);
}

///////////////////////////////////////////////////////
// 2. Python output stream: writes output via [e]msg().

// Implementation functions

    ಸ್ಥಿರ PyObject *
OutputGetattro(PyObject *self, PyObject *nameobj)
{
    GET_ATTR_STRING(name, nameobj);

    ಯದಿ (strcmp(name, "softspace") == 0)
	ಪ್ರತ್ಯಾಗಮ PyLong_FromLong(((OutputObject *)(self))->softspace);
    ಅನ್ಯಥಾ ಯದಿ (strcmp(name, "errors") == 0)
	ಪ್ರತ್ಯಾಗಮ PyString_FromString("strict");
    ಅನ್ಯಥಾ ಯದಿ (strcmp(name, "encoding") == 0)
	ಪ್ರತ್ಯಾಗಮ PyString_FromString(ENC_OPT);

    ಪ್ರತ್ಯಾಗಮ PyObject_GenericGetAttr(self, nameobj);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
OutputSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);

    ಪ್ರತ್ಯಾಗಮ OutputSetattr(self, name, val);
}

///////////////////////////////////////////////////////
// 3. Implementation of the Vim module for Python

// Window type - Implementation functions
// --------------------------------------

#define WindowType_Check(obj) ((obj)->ob_base.ob_type == &WindowType)

// Buffer type - Implementation functions
// --------------------------------------

#define BufferType_Check(obj) ((obj)->ob_base.ob_type == &BufferType)

ಸ್ಥಿರ PyObject* BufferSubscript(PyObject *self, PyObject *idx);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ BufferAsSubscript(PyObject *self, PyObject *idx, PyObject *val);

// Line range type - Implementation functions
// --------------------------------------

#define RangeType_Check(obj) ((obj)->ob_base.ob_type == &RangeType)

ಸ್ಥಿರ PyObject* RangeSubscript(PyObject *self, PyObject *idx);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ RangeAsItem(PyObject *, Py_ssize_t, PyObject *);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ RangeAsSubscript(PyObject *self, PyObject *idx, PyObject *val);

// Current objects type - Implementation functions
// -----------------------------------------------

ಸ್ಥಿರ PySequenceMethods BufferAsSeq = {
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

ಸ್ಥಿರ PyMappingMethods BufferAsMapping = {
    /* mp_length	*/ (lenfunc)BufferLength,
    /* mp_subscript     */ (binaryfunc)BufferSubscript,
    /* mp_ass_subscript */ (objobjargproc)BufferAsSubscript,
};


// Buffer object

    ಸ್ಥಿರ PyObject *
BufferGetattro(PyObject *self, PyObject *nameobj)
{
    PyObject *r;

    GET_ATTR_STRING(name, nameobj);

    ಯದಿ ((r = BufferAttrValid((BufferObject *)(self), name)))
	ಪ್ರತ್ಯಾಗಮ r;

    ಯದಿ (CheckBuffer((BufferObject *)(self)))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    r = BufferAttr((BufferObject *)(self), name);
    ಯದಿ (r || PyErr_Occurred())
	ಪ್ರತ್ಯಾಗಮ r;
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ PyObject_GenericGetAttr(self, nameobj);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
BufferSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);

    ಪ್ರತ್ಯಾಗಮ BufferSetattr(self, name, val);
}

//////////////////

    ಸ್ಥಿರ PyObject *
BufferSubscript(PyObject *self, PyObject* idx)
{
    ಯದಿ (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	ಪ್ರತ್ಯಾಗಮ BufferItem((BufferObject *)(self), _idx);
    }
    ಅನ್ಯಥಾ ಯದಿ (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	ಯದಿ (CheckBuffer((BufferObject *) self))
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	ಯದಿ (PySlice_GetIndicesEx((PySliceObject_T *)idx,
	      (Py_ssize_t)((BufferObject *)(self))->buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ,
	      &start, &stop,
	      &step, &slicelen) < 0)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	ಪ್ರತ್ಯಾಗಮ BufferSlice((BufferObject *)(self), start, stop);
    }
    ಅನ್ಯಥಾ
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
BufferAsSubscript(PyObject *self, PyObject* idx, PyObject* val)
{
    ಯದಿ (PyLong_Check(idx))
    {
	long n = PyLong_AsLong(idx);

	ಯದಿ (CheckBuffer((BufferObject *) self))
	    ಪ್ರತ್ಯಾಗಮ -1;

	ಪ್ರತ್ಯಾಗಮ RBAsItem((BufferObject *)(self), n, val, 1,
		    (Py_ssize_t)((BufferObject *)(self))->buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ,
		    ರಿಕ್ತ);
    }
    ಅನ್ಯಥಾ ಯದಿ (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	ಯದಿ (CheckBuffer((BufferObject *) self))
	    ಪ್ರತ್ಯಾಗಮ -1;

	ಯದಿ (PySlice_GetIndicesEx((PySliceObject_T *)idx,
	      (Py_ssize_t)((BufferObject *)(self))->buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ,
	      &start, &stop,
	      &step, &slicelen) < 0)
	    ಪ್ರತ್ಯಾಗಮ -1;
	ಪ್ರತ್ಯಾಗಮ RBAsSlice((BufferObject *)(self), start, stop, val, 1,
			  (PyInt)((BufferObject *)(self))->buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ,
			  ರಿಕ್ತ);
    }
    ಅನ್ಯಥಾ
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	ಪ್ರತ್ಯಾಗಮ -1;
    }
}

ಸ್ಥಿರ PySequenceMethods RangeAsSeq = {
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

ಸ್ಥಿರ PyMappingMethods RangeAsMapping = {
    /* mp_length	*/ (lenfunc)RangeLength,
    /* mp_subscript     */ (binaryfunc)RangeSubscript,
    /* mp_ass_subscript */ (objobjargproc)RangeAsSubscript,
};

// Line range object - Implementation

    ಸ್ಥಿರ PyObject *
RangeGetattro(PyObject *self, PyObject *nameobj)
{
    GET_ATTR_STRING(name, nameobj);

    ಯದಿ (strcmp(name, "start") == 0)
	ಪ್ರತ್ಯಾಗಮ Py_Build_ಮೌಲ್ಯ("n", ((RangeObject *)(self))->start - 1);
    ಅನ್ಯಥಾ ಯದಿ (strcmp(name, "end") == 0)
	ಪ್ರತ್ಯಾಗಮ Py_Build_ಮೌಲ್ಯ("n", ((RangeObject *)(self))->end - 1);
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ PyObject_GenericGetAttr(self, nameobj);
}

////////////////

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
RangeAsItem(PyObject *self, Py_ssize_t n, PyObject *val)
{
    ಪ್ರತ್ಯಾಗಮ RBAsItem(((RangeObject *)(self))->buf, n, val,
		    ((RangeObject *)(self))->start,
		    ((RangeObject *)(self))->end,
		    &((RangeObject *)(self))->end);
}

    ಸ್ಥಿರ Py_ssize_t
RangeAsSlice(PyObject *self, Py_ssize_t lo, Py_ssize_t hi, PyObject *val)
{
    ಪ್ರತ್ಯಾಗಮ RBAsSlice(((RangeObject *)(self))->buf, lo, hi, val,
		    ((RangeObject *)(self))->start,
		    ((RangeObject *)(self))->end,
		    &((RangeObject *)(self))->end);
}

    ಸ್ಥಿರ PyObject *
RangeSubscript(PyObject *self, PyObject* idx)
{
    ಯದಿ (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	ಪ್ರತ್ಯಾಗಮ RangeItem((RangeObject *)(self), _idx);
    }
    ಅನ್ಯಥಾ ಯದಿ (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	ಯದಿ (PySlice_GetIndicesEx((PySliceObject_T *)idx,
		((RangeObject *)(self))->end-((RangeObject *)(self))->start+1,
		&start, &stop,
		&step, &slicelen) < 0)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	ಪ್ರತ್ಯಾಗಮ RangeSlice((RangeObject *)(self), start, stop);
    }
    ಅನ್ಯಥಾ
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
RangeAsSubscript(PyObject *self, PyObject *idx, PyObject *val)
{
    ಯದಿ (PyLong_Check(idx))
    {
	long n = PyLong_AsLong(idx);
	ಪ್ರತ್ಯಾಗಮ RangeAsItem(self, n, val);
    }
    ಅನ್ಯಥಾ ಯದಿ (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	ಯದಿ (PySlice_GetIndicesEx((PySliceObject_T *)idx,
		((RangeObject *)(self))->end-((RangeObject *)(self))->start+1,
		&start, &stop,
		&step, &slicelen) < 0)
	    ಪ್ರತ್ಯಾಗಮ -1;
	ಪ್ರತ್ಯಾಗಮ RangeAsSlice(self, start, stop, val);
    }
    ಅನ್ಯಥಾ
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	ಪ್ರತ್ಯಾಗಮ -1;
    }
}

// TabPage object - Implementation

    ಸ್ಥಿರ PyObject *
TabPageGetattro(PyObject *self, PyObject *nameobj)
{
    PyObject *r;

    GET_ATTR_STRING(name, nameobj);

    ಯದಿ ((r = TabPageAttrValid((TabPageObject *)(self), name)))
	ಪ್ರತ್ಯಾಗಮ r;

    ಯದಿ (CheckTabPage((TabPageObject *)(self)))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    r = TabPageAttr((TabPageObject *)(self), name);
    ಯದಿ (r || PyErr_Occurred())
	ಪ್ರತ್ಯಾಗಮ r;
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ PyObject_GenericGetAttr(self, nameobj);
}

// Window object - Implementation

    ಸ್ಥಿರ PyObject *
WindowGetattro(PyObject *self, PyObject *nameobj)
{
    PyObject *r;

    GET_ATTR_STRING(name, nameobj);

    ಯದಿ ((r = WindowAttrValid((WindowObject *)(self), name)))
	ಪ್ರತ್ಯಾಗಮ r;

    ಯದಿ (CheckWindow((WindowObject *)(self)))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    r = WindowAttr((WindowObject *)(self), name);
    ಯದಿ (r || PyErr_Occurred())
	ಪ್ರತ್ಯಾಗಮ r;
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ PyObject_GenericGetAttr(self, nameobj);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
WindowSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);

    ಪ್ರತ್ಯಾಗಮ WindowSetattr(self, name, val);
}

// Tab page list object - Definitions

ಸ್ಥಿರ PySequenceMethods TabListAsSeq = {
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

ಸ್ಥಿರ PySequenceMethods WinListAsSeq = {
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
    ಸ್ಥಿರ PyObject *
CurrentGetattro(PyObject *self, PyObject *nameobj)
{
    PyObject	*r;
    GET_ATTR_STRING(name, nameobj);
    ಯದಿ (!(r = CurrentGetattr(self, name)))
	ಪ್ರತ್ಯಾಗಮ PyObject_GenericGetAttr(self, nameobj);
    ಪ್ರತ್ಯಾಗಮ r;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
CurrentSetattro(PyObject *self, PyObject *nameobj, PyObject *ಮೌಲ್ಯ)
{
    GET_ATTR_STRING(name, nameobj);
    ಪ್ರತ್ಯಾಗಮ CurrentSetattr(self, name, ಮೌಲ್ಯ);
}

// Dictionary object - Definitions

    ಸ್ಥಿರ PyObject *
DictionaryGetattro(PyObject *self, PyObject *nameobj)
{
    DictionaryObject	*this = ((DictionaryObject *) (self));

    GET_ATTR_STRING(name, nameobj);

    ಯದಿ (strcmp(name, "locked") == 0)
	ಪ್ರತ್ಯಾಗಮ PyLong_FromLong(this->dict->dv_lock);
    ಅನ್ಯಥಾ ಯದಿ (strcmp(name, "scope") == 0)
	ಪ್ರತ್ಯಾಗಮ PyLong_FromLong(this->dict->dv_scope);

    ಪ್ರತ್ಯಾಗಮ PyObject_GenericGetAttr(self, nameobj);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
DictionarySetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);
    ಪ್ರತ್ಯಾಗಮ DictionarySetattr(self, name, val);
}

// List object - Definitions

    ಸ್ಥಿರ PyObject *
ListGetattro(PyObject *self, PyObject *nameobj)
{
    GET_ATTR_STRING(name, nameobj);

    ಯದಿ (strcmp(name, "locked") == 0)
	ಪ್ರತ್ಯಾಗಮ PyLong_FromLong(((ListObject *) (self))->list->lv_lock);

    ಪ್ರತ್ಯಾಗಮ PyObject_GenericGetAttr(self, nameobj);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ListSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);
    ಪ್ರತ್ಯಾಗಮ ListSetattr(self, name, val);
}

// Tuple object - Definitions

    ಸ್ಥಿರ PyObject *
TupleGetattro(PyObject *self, PyObject *nameobj)
{
    GET_ATTR_STRING(name, nameobj);

    ಯದಿ (strcmp(name, "locked") == 0)
	ಪ್ರತ್ಯಾಗಮ PyLong_FromLong(((TupleObject *) (self))->tuple->tv_lock);

    ಪ್ರತ್ಯಾಗಮ PyObject_GenericGetAttr(self, nameobj);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
TupleSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);
    ಪ್ರತ್ಯಾಗಮ TupleSetattr(self, name, val);
}

// Function object - Definitions

    ಸ್ಥಿರ PyObject *
FunctionGetattro(PyObject *self, PyObject *nameobj)
{
    PyObject		*r;
    FunctionObject	*this = (FunctionObject *)(self);

    GET_ATTR_STRING(name, nameobj);

    r = FunctionAttr(this, name);
    ಯದಿ (r || PyErr_Occurred())
	ಪ್ರತ್ಯಾಗಮ r;
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ PyObject_GenericGetAttr(self, nameobj);
}

// External interface

    ಶೂನ್ಯ
python3_buffer_free(buf_T *buf)
{
    BufferObject *bp = BUF_PYTHON_REF(buf);
    ಯದಿ (bp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    bp->buf = INVALID_BUFFER_ಮೌಲ್ಯ;
    BUF_PYTHON_REF(buf) = ರಿಕ್ತ;
}

    ಶೂನ್ಯ
python3_window_free(win_T *win)
{
    WindowObject *wp = WIN_PYTHON_REF(win);
    ಯದಿ (wp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    wp->win = INVALID_WINDOW_ಮೌಲ್ಯ;
    WIN_PYTHON_REF(win) = ರಿಕ್ತ;
}

    ಶೂನ್ಯ
python3_tabpage_free(tabpage_T *tab)
{
    TabPageObject *tp = TAB_PYTHON_REF(tab);
    ಯದಿ (tp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    tp->tab = INVALID_TABPAGE_ಮೌಲ್ಯ;
    TAB_PYTHON_REF(tab) = ರಿಕ್ತ;
}

    ಸ್ಥಿರ PyObject *
Py3Init_vim(ಶೂನ್ಯ)
{
    // The special value is removed from sys.path in Python3_Init().
    ಸ್ಥಿರ wchar_t *(argv[2]) = {L"/must>not&exist/foo", ರಿಕ್ತ};

    ಯದಿ (init_types())
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // Set sys.argv[] to avoid a crash in warn().
    PySys_SetArgv(1, argv);

    ಯದಿ ((vim_module = PyModule_Create(&vimmodule)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (populate_module(vim_module))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (init_sys_path())
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಪ್ರತ್ಯಾಗಮ vim_module;
}

//////////////////////////////////////////////////////////////////////////
// 4. Utility functions for handling the interface between Vim and Python.

/*
 * Convert a Vim line into a Python string.
 * All internal newlines are replaced by null characters.
 *
 * On errors, the Python exception data is set, and NULL is returned.
 */
    ಸ್ಥಿರ PyObject *
ಸಾಲು_To_String(ಧ್ರುವ ವರ್ಣ *str)
{
    PyObject *result;
    Py_ssize_t len = strlen(str);
    ವರ್ಣ *tmp, *p;

    tmp = alloc(len + 1);
    p = tmp;
    ಯದಿ (p == ರಿಕ್ತ)
    {
	PyErr_NoMemory();
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಯಾವತ್ (*str)
    {
	ಯದಿ (*str == '\n')
	    *p = '\0';
	ಅನ್ಯಥಾ
	    *p = *str;

	++p;
	++str;
    }
    *p = '\0';

    result = PyUnicode_Decode(tmp, len, (ವರ್ಣ *)ENC_OPT, ERRORS_DECODE_ARG);

    vim_free(tmp);
    ಪ್ರತ್ಯಾಗಮ result;
}

    ಶೂನ್ಯ
do_py3eval(char_u *str, dict_T *locals, typval_T *rettv)
{
    DoPyCommand((ವರ್ಣ *) str,
	    locals,
	    init_range_eval,
	    (runner) run_eval,
	    (ಶೂನ್ಯ *) rettv);
    ಯದಿ (rettv->v_type == VAR_UNKNOWN)
    {
	rettv->v_type = VAR_ಸಂಖ್ಯೆ;
	rettv->vval.v_ಸಂಖ್ಯೆ = 0;
    }
}

    ಪೂರ್ಣಾಙ್ಕ
set_ref_in_python3(ಪೂರ್ಣಾಙ್ಕ copyID)
{
    ಪ್ರತ್ಯಾಗಮ set_ref_in_py(copyID);
}

    ಪೂರ್ಣಾಙ್ಕ
python3_version(ಶೂನ್ಯ)
{
#ifdef USE_LIMITED_API
    ಪ್ರತ್ಯಾಗಮ Py_LIMITED_API;
#ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ PY_VERSION_HEX;
#endif
}
