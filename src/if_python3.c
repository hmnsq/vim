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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଅନ୍ତର୍ଭାଵ <limits.h>

#ଯଦି defined(MSWIN) && defined(HAVE_FCNTL_H)
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

#ଅନ୍ତର୍ଭାଵ <Python.h>

#undef ମୁଖ୍ୟ // Defined in python.h - aargh
#undef HAVE_FCNTL_H // Clash with os_win32.h

// The "surrogateescape" error handler is new in Python 3.1
#ଯଦି PY_VERSION_HEX >= 0x030100f0
# define CODEC_ERROR_HANDLER "surrogateescape"
#ଅନ୍ୟଥା
# define CODEC_ERROR_HANDLER ଖାଲି
#endif

// Suppress Python 3.11 depreciations to see useful warnings
#ifdef __GNUC__
# pragma GCC diagnostic ଠେଲ
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
#ଯଦି PY_VERSION_HEX >= 0x030200f0
ପ୍ରକାରବ୍ୟାଖ୍ୟା PyObject PySliceObject_T;
#ଅନ୍ୟଥା
ପ୍ରକାରବ୍ୟାଖ୍ୟା PySliceObject PySliceObject_T;
#endif

#ifndef MSWIN
# define HINSTANCE ଶୂନ୍ୟ *
#endif
#ଯଦି defined(DYNAMIC_PYTHON3) || defined(MSWIN)
ସ୍ଥିର HINSTANCE hinstPy3 = 0; // Instance of python.dll
#endif

#ଯଦି defined(DYNAMIC_PYTHON3)

# ifdef MSWIN
#  define load_dll vimLoadLib
#  define close_dll FreeLibrary
#  define ପ୍ରତୀକ_from_dll GetProcAddress
#  define load_dll_error GetWin32Error
# ଅନ୍ୟଥା
#  include <dlfcn.h>
#  define FARPROC ଶୂନ୍ୟ*
#  ଯଦି defined(PY_NO_RTLD_GLOBAL) && defined(PY3_NO_RTLD_GLOBAL)
#   define load_dll(n) dlopen((n), RTLD_LAZY)
#  ଅନ୍ୟଥା
#   define load_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#  endif
#  define close_dll dlclose
#  define ପ୍ରତୀକ_from_dll dlsym
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
# define Py_List_ଭର୍ତ୍ତି py3_Py_List_ଭର୍ତ୍ତି
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
# ଯଦି PY_VERSION_HEX >= 0x030601f0
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
# ifndef Py_Mapping_ଚାବିଗୁଡ଼ିକ
#  define Py_Mapping_ଚାବିଗୁଡ଼ିକ py3_Py_Mapping_ଚାବିଗୁଡ଼ିକ
# endif
# ଯଦି (defined(USE_LIMITED_API) && Py_LIMITED_API >= 0x03080000) || \
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
# ଯଦି defined(USE_LIMITED_API) || PY_VERSION_HEX >= 0x03080000
#  define Py_IncRef py3_Py_IncRef
#  define Py_DecRef py3_Py_DecRef
# endif
# ifdef USE_LIMITED_API
#  define Py_CompileString py3_Py_CompileString
#  define PyEval_EvalCode py3_PyEval_EvalCode
# ଅନ୍ୟଥା
#  undef PyRun_SimpleString
#  define PyRun_SimpleString py3_PyRun_SimpleString
#  undef PyRun_String
#  define PyRun_String py3_PyRun_String
# endif
# define PyObject_GetAttrString py3_PyObject_GetAttrString
# define PyObject_HasAttrString py3_PyObject_HasAttrString
# define PyObject_SetAttrString py3_PyObject_SetAttrString
# define PyObject_CallFunctionObjArgs py3_PyObject_CallFunctionObjArgs
# ଯଦି PY_VERSION_HEX >= 0x030d0000
#  define PyObject_CallFunction py3_PyObject_CallFunction
# ଅନ୍ୟଥା
#  define _PyObject_CallFunction_SizeT py3__PyObject_CallFunction_SizeT
# endif
# define PyObject_Call py3_PyObject_Call
# define PyEval_GetLocals py3_PyEval_GetLocals
# define PyEval_GetGlobals py3_PyEval_GetGlobals
# define PySys_SetObject py3_PySys_SetObject
# define PySys_GetObject py3_PySys_GetObject
# define PySys_SetArgv py3_PySys_SetArgv
# define PyType_Ready py3_PyType_Ready
# ଯଦି PY_VERSION_HEX >= 0x03040000
#  define PyType_GetFlags py3_PyType_GetFlags
# endif
# undef Py_Build_ମୂଲ୍ୟ
# define Py_Build_ମୂଲ୍ୟ py3_Py_Build_ମୂଲ୍ୟ
# define Py_SetPythonHome py3_Py_SetPythonHome
# define Py_Initialize py3_Py_Initialize
# define Py_Finalize py3_Py_Finalize
# define Py_IsInitialized py3_Py_IsInitialized
# define _Py_NoneStruct (*py3__Py_NoneStruct)
# define _Py_FalseStruct (*py3__Py_FalseStruct)
# define _Py_TrueStruct (*py3__Py_TrueStruct)
# ଯଦି !defined(USE_LIMITED_API) && PY_VERSION_HEX < 0x030D0000
// Private symbol that used to be required as part of PyIter_Check.
#  define _PyObject_NextNotImplemented (*py3__PyObject_NextNotImplemented)
# endif
# define PyModule_AddObject py3_PyModule_AddObject
# define PyImport_AppendInittab py3_PyImport_AppendInittab
# define PyImport_AddModule py3_PyImport_AddModule
# ifdef USE_LIMITED_API
#  ଯଦି Py_LIMITED_API >= 0x030a0000
#   define PyUnicode_AsUTF8AndSize py3_PyUnicode_AsUTF8AndSize
#  endif
# ଅନ୍ୟଥା
#  ଯଦି PY_VERSION_HEX >= 0x030300f0
#   define PyUnicode_AsUTF8AndSize py3_PyUnicode_AsUTF8AndSize
#  ଅନ୍ୟଥା
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
# define Py_ସଂଖ୍ୟା_Check (*py3_Py_ସଂଖ୍ୟା_Check)
# define Py_ସଂଖ୍ୟା_Long (*py3_Py_ସଂଖ୍ୟା_Long)
# define PyBool_Type (*py3_PyBool_Type)
# define PyErr_NewException py3_PyErr_NewException
# ifdef Py_DEBUG
#  define _Py_NegativeRefcount py3__Py_NegativeRefcount
#  define _Py_RefTotal (*py3__Py_RefTotal)
#  define PyModule_Create2TraceRefs py3_PyModule_Create2TraceRefs
# ଅନ୍ୟଥା
#  define PyModule_Create2 py3_PyModule_Create2
# endif
# ଯଦି defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
#  define _PyObject_DebugMalloc py3__PyObject_DebugMalloc
#  define _PyObject_DebugFree py3__PyObject_DebugFree
# ଅନ୍ୟଥା
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
#  ଅନ୍ୟଥା
#   define PyUnicodeUCS2_FromFormat py3_PyUnicodeUCS2_FromFormat
#  endif
# ଅନ୍ୟଥା
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

# ଯଦି defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
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
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*py3_PySys_SetArgv)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, wchar_t **);
ସ୍ଥିର ଶୂନ୍ୟ (*py3_Py_SetPythonHome)(wchar_t *home);
ସ୍ଥିର ଶୂନ୍ୟ (*py3_Py_Initialize)(ଶୂନ୍ୟ);
ସ୍ଥିର PyObject* (*py3_PyList_New)(Py_ssize_t size);
ସ୍ଥିର PyGILState_STATE (*py3_PyGILState_Ensure)(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ (*py3_PyGILState_Release)(PyGILState_STATE);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*py3_PySys_SetObject)(ଅକ୍ଷର *, PyObject *);
ସ୍ଥିର PyObject* (*py3_PySys_GetObject)(ଅକ୍ଷର *);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*py3_PyList_Append)(PyObject *, PyObject *);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*py3_Py_List_ଭର୍ତ୍ତି)(PyObject *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, PyObject *);
ସ୍ଥିର Py_ssize_t (*py3_PyList_Size)(PyObject *);
ସ୍ଥିର PyObject* (*py3_PyTuple_New)(Py_ssize_t size);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*py3_PyTuple_SetItem)(PyObject *, Py_ssize_t, PyObject *);
ସ୍ଥିର Py_ssize_t (*py3_PyTuple_Size)(PyObject *);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*py3_PySequence_Check)(PyObject *);
ସ୍ଥିର Py_ssize_t (*py3_PySequence_Size)(PyObject *);
ସ୍ଥିର PyObject* (*py3_PySequence_GetItem)(PyObject *, Py_ssize_t);
ସ୍ଥିର PyObject* (*py3_PySequence_Fast)(PyObject *, ଅଚଳ ଅକ୍ଷର *);
ସ୍ଥିର PyObject* (*py3_PyTuple_GetItem)(PyObject *, Py_ssize_t);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*py3_PyMapping_Check)(PyObject *);
ସ୍ଥିର PyObject* (*py3_Py_Mapping_ଚାବିଗୁଡ଼ିକ)(PyObject *);
# ଯଦି PY_VERSION_HEX >= 0x030601f0
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*py3_PySlice_AdjustIndices)(Py_ssize_t length,
		     Py_ssize_t *start, Py_ssize_t *stop, Py_ssize_t step);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*py3_PySlice_Unpack)(PyObject *slice,
		     Py_ssize_t *start, Py_ssize_t *stop, Py_ssize_t *step);
# endif
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*py3_PySlice_GetIndicesEx)(PySliceObject_T *r, Py_ssize_t length,
		     Py_ssize_t *start, Py_ssize_t *stop, Py_ssize_t *step,
		     Py_ssize_t *slicelen);
ସ୍ଥିର PyObject* (*py3_PyErr_NoMemory)(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ (*py3_Py_Finalize)(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ (*py3_PyErr_SetString)(PyObject *, ଅଚଳ ଅକ୍ଷର *);
ସ୍ଥିର ଶୂନ୍ୟ (*py3_PyErr_SetObject)(PyObject *, PyObject *);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*py3_PyErr_ExceptionMatches)(PyObject *);
# ଯଦି defined(USE_LIMITED_API) || PY_VERSION_HEX >= 0x03080000
ସ୍ଥିର ଶୂନ୍ୟ (*py3_Py_IncRef)(PyObject *);
ସ୍ଥିର ଶୂନ୍ୟ (*py3_Py_DecRef)(PyObject *);
# endif
# ifdef USE_LIMITED_API
ସ୍ଥିର PyObject* (*py3_Py_CompileString)(ଅଚଳ ଅକ୍ଷର *, ଅଚଳ ଅକ୍ଷର *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ସ୍ଥିର PyObject* (*py3_PyEval_EvalCode)(PyObject *co, PyObject *globals, PyObject *locals);
# ଅନ୍ୟଥା
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*py3_PyRun_SimpleString)(ଅକ୍ଷର *);
ସ୍ଥିର PyObject* (*py3_PyRun_String)(ଅକ୍ଷର *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, PyObject *, PyObject *);
# endif
ସ୍ଥିର PyObject* (*py3_PyObject_GetAttrString)(PyObject *, ଅଚଳ ଅକ୍ଷର *);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*py3_PyObject_HasAttrString)(PyObject *, ଅଚଳ ଅକ୍ଷର *);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*py3_PyObject_SetAttrString)(PyObject *, ଅଚଳ ଅକ୍ଷର *, PyObject *);
ସ୍ଥିର PyObject* (*py3_PyObject_CallFunctionObjArgs)(PyObject *, ...);
# ଯଦି PY_VERSION_HEX >= 0x030d0000
ସ୍ଥିର PyObject* (*py3_PyObject_CallFunction)(PyObject *, ଅକ୍ଷର *, ...);
# ଅନ୍ୟଥା
ସ୍ଥିର PyObject* (*py3__PyObject_CallFunction_SizeT)(PyObject *, ଅକ୍ଷର *, ...);
# endif
ସ୍ଥିର PyObject* (*py3_PyObject_Call)(PyObject *, PyObject *, PyObject *);
ସ୍ଥିର PyObject* (*py3_PyEval_GetGlobals)(ଶୂନ୍ୟ);
ସ୍ଥିର PyObject* (*py3_PyEval_GetLocals)(ଶୂନ୍ୟ);
ସ୍ଥିର PyObject* (*py3_PyList_GetItem)(PyObject *, Py_ssize_t);
ସ୍ଥିର PyObject* (*py3_PyImport_ImportModule)(ଅଚଳ ଅକ୍ଷର *);
ସ୍ଥିର PyObject* (*py3_PyImport_AddModule)(ଅଚଳ ଅକ୍ଷର *);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*py3_PyErr_BadArgument)(ଶୂନ୍ୟ);
ସ୍ଥିର PyObject* (*py3_PyErr_Occurred)(ଶୂନ୍ୟ);
ସ୍ଥିର PyObject* (*py3_PyModule_GetDict)(PyObject *);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*py3_PyList_SetItem)(PyObject *, Py_ssize_t, PyObject *);
ସ୍ଥିର PyObject* (*py3_PyDict_GetItemString)(PyObject *, ଅଚଳ ଅକ୍ଷର *);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*py3_PyDict_Next)(PyObject *, Py_ssize_t *, PyObject **, PyObject **);
ସ୍ଥିର PyObject* (*py3_PyLong_FromLong)(long);
ସ୍ଥିର PyObject* (*py3_PyDict_New)(ଶୂନ୍ୟ);
# ଯଦି (defined(USE_LIMITED_API) && Py_LIMITED_API >= 0x03080000) || \
       (!defined(USE_LIMITED_API) && PY_VERSION_HEX >= 0x03080000)
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*py3_PyIter_Check)(PyObject *o);
# endif
ସ୍ଥିର PyObject* (*py3_PyIter_Next)(PyObject *);
ସ୍ଥିର PyObject* (*py3_PyObject_GetIter)(PyObject *);
ସ୍ଥିର PyObject* (*py3_PyObject_Repr)(PyObject *);
ସ୍ଥିର PyObject* (*py3_PyObject_GetItem)(PyObject *, PyObject *);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*py3_PyObject_IsTrue)(PyObject *);
ସ୍ଥିର PyObject* (*py3_Py_Build_ମୂଲ୍ୟ)(ଅକ୍ଷର *, ...);
# ଯଦି PY_VERSION_HEX >= 0x03040000
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*py3_PyType_GetFlags)(PyTypeObject *o);
# endif
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*py3_PyType_Ready)(PyTypeObject *type);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*py3_PyDict_SetItemString)(PyObject *dp, ଅକ୍ଷର *ଚାବି, PyObject *item);
ସ୍ଥିର PyObject* (*py3_PyUnicode_FromString)(ଅଚଳ ଅକ୍ଷର *u);
# ifdef Py_UNICODE_USE_UCS_FUNCTIONS
#  ifdef Py_UNICODE_WIDE
ସ୍ଥିର PyObject* (*py3_PyUnicodeUCS4_FromFormat)(ଅଚଳ ଅକ୍ଷର *u, ...);
#  ଅନ୍ୟଥା
ସ୍ଥିର PyObject* (*py3_PyUnicodeUCS2_FromFormat)(ଅଚଳ ଅକ୍ଷର *u, ...);
#  endif
# ଅନ୍ୟଥା
ସ୍ଥିର PyObject* (*py3_PyUnicode_FromFormat)(ଅଚଳ ଅକ୍ଷର *u, ...);
# endif
ସ୍ଥିର PyObject* (*py3_PyUnicode_Decode)(ଅଚଳ ଅକ୍ଷର *u, Py_ssize_t size,
	ଅଚଳ ଅକ୍ଷର *encoding, ଅଚଳ ଅକ୍ଷର *errors);
ସ୍ଥିର long (*py3_PyLong_AsLong)(PyObject *);
ସ୍ଥିର ଶୂନ୍ୟ (*py3_PyErr_SetNone)(PyObject *);
ସ୍ଥିର ଶୂନ୍ୟ (*py3_PyEval_InitThreads)(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ(*py3_PyEval_RestoreThread)(PyThreadState *);
ସ୍ଥିର PyThreadState*(*py3_PyEval_SaveThread)(ଶୂନ୍ୟ);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*py3_PyArg_Parse)(PyObject *, ଅକ୍ଷର *, ...);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*py3_PyArg_ParseTuple)(PyObject *, ଅକ୍ଷର *, ...);
ସ୍ଥିର ଶୂନ୍ୟ (*py3_PyMem_Free)(ଶୂନ୍ୟ *);
ସ୍ଥିର ଶୂନ୍ୟ* (*py3_PyMem_Malloc)(size_t);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*py3_Py_IsInitialized)(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ (*py3_PyErr_Clear)(ଶୂନ୍ୟ);
ସ୍ଥିର PyObject* (*py3_PyErr_Format)(PyObject *, ଅଚଳ ଅକ୍ଷର *, ...);
ସ୍ଥିର ଶୂନ୍ୟ (*py3_PyErr_PrintEx)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ସ୍ଥିର PyObject*(*py3__PyObject_Init)(PyObject *, PyTypeObject *);
# ଯଦି !defined(USE_LIMITED_API) && PY_VERSION_HEX < 0x030D0000
ସ୍ଥିର iternextfunc py3__PyObject_NextNotImplemented;
# endif
ସ୍ଥିର PyObject* py3__Py_NoneStruct;
ସ୍ଥିର PyObject* py3__Py_FalseStruct;
ସ୍ଥିର PyObject* py3__Py_TrueStruct;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*py3_PyModule_AddObject)(PyObject *m, ଅଚଳ ଅକ୍ଷର *name, PyObject *o);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*py3_PyImport_AppendInittab)(ଅଚଳ ଅକ୍ଷର *name, PyObject* (*initfunc)(ଶୂନ୍ୟ));
# ifdef USE_LIMITED_API
#  ଯଦି Py_LIMITED_API >= 0x030a0000
ସ୍ଥିର ଅକ୍ଷର* (*py3_PyUnicode_AsUTF8AndSize)(PyObject *unicode, Py_ssize_t *size);
#  endif
# ଅନ୍ୟଥା
#  ଯଦି PY_VERSION_HEX >= 0x030300f0
ସ୍ଥିର ଅକ୍ଷର* (*py3_PyUnicode_AsUTF8AndSize)(PyObject *unicode, Py_ssize_t *size);
#  ଅନ୍ୟଥା
ସ୍ଥିର ଅକ୍ଷର* (*py3__PyUnicode_AsString)(PyObject *unicode);
#  endif
# endif
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*py3_PyUnicode_CompareWithASCIIString)(PyObject *unicode, ଅଚଳ ଅକ୍ଷର* string);
ସ୍ଥିର PyObject* (*py3_PyUnicode_AsEncodedString)(PyObject *unicode, ଅଚଳ ଅକ୍ଷର* encoding, ଅଚଳ ଅକ୍ଷର* errors);
ସ୍ଥିର PyObject* (*py3_PyUnicode_AsUTF8String)(PyObject *unicode);
ସ୍ଥିର ଅକ୍ଷର* (*py3_PyBytes_AsString)(PyObject *bytes);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*py3_PyBytes_AsStringAndSize)(PyObject *bytes, ଅକ୍ଷର **buffer, Py_ssize_t *length);
ସ୍ଥିର PyObject* (*py3_PyBytes_FromString)(ଅକ୍ଷର *str);
ସ୍ଥିର PyObject* (*py3_PyBytes_FromStringAndSize)(ଅକ୍ଷର *str, Py_ssize_t length);
# ଯଦି PY_VERSION_HEX >= 0x030900b0
ସ୍ଥିର PyObject* (*py3__PyObject_New)(PyTypeObject *);
# endif
ସ୍ଥିର PyObject* (*py3_PyFloat_FromDouble)(ଦ୍ୱିଗୁଣ ଅଙ୍କ);
ସ୍ଥିର ଦ୍ୱିଗୁଣ (*py3_PyFloat_AsDouble)(PyObject *);
ସ୍ଥିର PyObject* (*py3_PyObject_GenericGetAttr)(PyObject *obj, PyObject *name);
ସ୍ଥିର PyObject* (*py3_PyType_GenericAlloc)(PyTypeObject *type, Py_ssize_t nitems);
ସ୍ଥିର PyObject* (*py3_PyType_GenericNew)(PyTypeObject *type, PyObject *args, PyObject *kwds);
ସ୍ଥିର PyTypeObject* py3_PyType_Type;
# ifndef USE_LIMITED_API
ସ୍ଥିର PyTypeObject* py3_PyStdPrinter_Type;
# endif
ସ୍ଥିର PyTypeObject* py3_PySlice_Type;
ସ୍ଥିର PyTypeObject* py3_PyFloat_Type;
ସ୍ଥିର PyTypeObject* py3_PyBool_Type;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*py3_Py_ସଂଖ୍ୟା_Check)(PyObject *);
ସ୍ଥିର PyObject* (*py3_Py_ସଂଖ୍ୟା_Long)(PyObject *);
ସ୍ଥିର PyObject* (*py3_PyErr_NewException)(ଅକ୍ଷର *name, PyObject *base, PyObject *dict);
ସ୍ଥିର PyObject* (*py3_PyCapsule_New)(ଶୂନ୍ୟ *, ଅକ୍ଷର *, PyCapsule_Destructor);
ସ୍ଥିର ଶୂନ୍ୟ* (*py3_PyCapsule_GetPointer)(PyObject *, ଅକ୍ଷର *);
# ifdef Py_DEBUG
ସ୍ଥିର ଶୂନ୍ୟ (*py3__Py_NegativeRefcount)(ଅଚଳ ଅକ୍ଷର *fname, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lineno, PyObject *op);
ସ୍ଥିର Py_ssize_t* py3__Py_RefTotal;
ସ୍ଥିର PyObject* (*py3_PyModule_Create2TraceRefs)(ସଂରଚନା PyModuleDef* module, ପୂର୍ଣ୍ଣସଂଖ୍ୟା module_api_version);
# ଅନ୍ୟଥା
ସ୍ଥିର PyObject* (*py3_PyModule_Create2)(ସଂରଚନା PyModuleDef* module, ପୂର୍ଣ୍ଣସଂଖ୍ୟା module_api_version);
# endif
# ଯଦି defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
ସ୍ଥିର ଶୂନ୍ୟ (*py3__PyObject_DebugFree)(ଶୂନ୍ୟ*);
ସ୍ଥିର ଶୂନ୍ୟ* (*py3__PyObject_DebugMalloc)(size_t);
# ଅନ୍ୟଥା
ସ୍ଥିର ଶୂନ୍ୟ (*py3_PyObject_Free)(ଶୂନ୍ୟ*);
ସ୍ଥିର ଶୂନ୍ୟ* (*py3_PyObject_Malloc)(size_t);
# endif
ସ୍ଥିର PyObject*(*py3__PyObject_GC_New)(PyTypeObject *);
ସ୍ଥିର ଶୂନ୍ୟ(*py3_PyObject_GC_Del)(ଶୂନ୍ୟ *);
ସ୍ଥିର ଶୂନ୍ୟ(*py3_PyObject_GC_UnTrack)(ଶୂନ୍ୟ *);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*py3_PyType_IsSubtype)(PyTypeObject *, PyTypeObject *);
# ifdef USE_LIMITED_API
ସ୍ଥିର ଶୂନ୍ୟ* (*py3_PyType_GetSlot)(PyTypeObject *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ସ୍ଥିର PyObject* (*py3_PyType_FromSpec)(PyType_Spec *);
# endif

// Imported exception objects
ସ୍ଥିର PyObject *p3imp_PyExc_AttributeError;
ସ୍ଥିର PyObject *p3imp_PyExc_IndexError;
ସ୍ଥିର PyObject *p3imp_Py_Exc_ଚାବି_Error;
ସ୍ଥିର PyObject *p3imp_PyExc_KeyboardInterrupt;
ସ୍ଥିର PyObject *p3imp_PyExc_TypeError;
ସ୍ଥିର PyObject *p3imp_Py_Exc_ମୂଲ୍ୟ_Error;
ସ୍ଥିର PyObject *p3imp_PyExc_SystemExit;
ସ୍ଥିର PyObject *p3imp_PyExc_RuntimeError;
ସ୍ଥିର PyObject *p3imp_PyExc_ImportError;
ସ୍ଥିର PyObject *p3imp_PyExc_OverflowError;

# define PyExc_AttributeError p3imp_PyExc_AttributeError
# define PyExc_IndexError p3imp_PyExc_IndexError
# define Py_Exc_ଚାବି_Error p3imp_Py_Exc_ଚାବି_Error
# define PyExc_KeyboardInterrupt p3imp_PyExc_KeyboardInterrupt
# define PyExc_TypeError p3imp_PyExc_TypeError
# define Py_Exc_ମୂଲ୍ୟ_Error p3imp_Py_Exc_ମୂଲ୍ୟ_Error
# define PyExc_SystemExit p3imp_PyExc_SystemExit
# define PyExc_RuntimeError p3imp_PyExc_RuntimeError
# define PyExc_ImportError p3imp_PyExc_ImportError
# define PyExc_OverflowError p3imp_PyExc_OverflowError

/*
 * Table of name to function pointer of python.
 */
# define PYTHON_PROC FARPROC
ସ୍ଥିର ସଂରଚନା
{
    ଅକ୍ଷର *name;
    PYTHON_PROC *ptr;
} py3_funcname_table[] =
{
    {"PySys_SetArgv", (PYTHON_PROC*)&py3_PySys_SetArgv},
    {"Py_SetPythonHome", (PYTHON_PROC*)&py3_Py_SetPythonHome},
    {"Py_Initialize", (PYTHON_PROC*)&py3_Py_Initialize},
    {"_PyArg_ParseTuple_SizeT", (PYTHON_PROC*)&py3_PyArg_ParseTuple},
    {"_Py_BuildValue_SizeT", (PYTHON_PROC*)&py3_Py_Build_ମୂଲ୍ୟ},
    {"PyMem_Free", (PYTHON_PROC*)&py3_PyMem_Free},
    {"PyMem_Malloc", (PYTHON_PROC*)&py3_PyMem_Malloc},
    {"PyList_New", (PYTHON_PROC*)&py3_PyList_New},
    {"PyGILState_Ensure", (PYTHON_PROC*)&py3_PyGILState_Ensure},
    {"PyGILState_Release", (PYTHON_PROC*)&py3_PyGILState_Release},
    {"PySys_SetObject", (PYTHON_PROC*)&py3_PySys_SetObject},
    {"PySys_GetObject", (PYTHON_PROC*)&py3_PySys_GetObject},
    {"PyList_Append", (PYTHON_PROC*)&py3_PyList_Append},
    {"PyList_Insert", (PYTHON_PROC*)&py3_Py_List_ଭର୍ତ୍ତି},
    {"PyList_Size", (PYTHON_PROC*)&py3_PyList_Size},
    {"PyTuple_New", (PYTHON_PROC*)&py3_PyTuple_New},
    {"PyTuple_GetItem", (PYTHON_PROC*)&py3_PyTuple_GetItem},
    {"PyTuple_SetItem", (PYTHON_PROC*)&py3_PyTuple_SetItem},
    {"PyTuple_Size", (PYTHON_PROC*)&py3_PyTuple_Size},
    {"PySequence_Check", (PYTHON_PROC*)&py3_PySequence_Check},
    {"PySequence_Size", (PYTHON_PROC*)&py3_PySequence_Size},
    {"PySequence_GetItem", (PYTHON_PROC*)&py3_PySequence_GetItem},
    {"PySequence_Fast", (PYTHON_PROC*)&py3_PySequence_Fast},
# ଯଦି PY_VERSION_HEX >= 0x030601f0
    {"PySlice_AdjustIndices", (PYTHON_PROC*)&py3_PySlice_AdjustIndices},
    {"PySlice_Unpack", (PYTHON_PROC*)&py3_PySlice_Unpack},
# endif
    {"PySlice_GetIndicesEx", (PYTHON_PROC*)&py3_PySlice_GetIndicesEx},
    {"PyErr_NoMemory", (PYTHON_PROC*)&py3_PyErr_NoMemory},
    {"Py_Finalize", (PYTHON_PROC*)&py3_Py_Finalize},
    {"PyErr_SetString", (PYTHON_PROC*)&py3_PyErr_SetString},
    {"PyErr_SetObject", (PYTHON_PROC*)&py3_PyErr_SetObject},
    {"PyErr_ExceptionMatches", (PYTHON_PROC*)&py3_PyErr_ExceptionMatches},
# ଯଦି defined(USE_LIMITED_API) || PY_VERSION_HEX >= 0x03080000
    {"Py_IncRef", (PYTHON_PROC*)&py3_Py_IncRef},
    {"Py_DecRef", (PYTHON_PROC*)&py3_Py_DecRef},
# endif
# ifdef USE_LIMITED_API
    {"Py_CompileString", (PYTHON_PROC*)&py3_Py_CompileString},
    {"PyEval_EvalCode", (PYTHON_PROC*)&PyEval_EvalCode},
# ଅନ୍ୟଥା
    {"PyRun_SimpleString", (PYTHON_PROC*)&py3_PyRun_SimpleString},
    {"PyRun_String", (PYTHON_PROC*)&py3_PyRun_String},
# endif
    {"PyObject_GetAttrString", (PYTHON_PROC*)&py3_PyObject_GetAttrString},
    {"PyObject_HasAttrString", (PYTHON_PROC*)&py3_PyObject_HasAttrString},
    {"PyObject_SetAttrString", (PYTHON_PROC*)&py3_PyObject_SetAttrString},
    {"PyObject_CallFunctionObjArgs", (PYTHON_PROC*)&py3_PyObject_CallFunctionObjArgs},
# ଯଦି PY_VERSION_HEX >= 0x030d0000
    {"PyObject_CallFunction", (PYTHON_PROC*)&py3_PyObject_CallFunction},
# ଅନ୍ୟଥା
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
    {"PyMapping_Keys", (PYTHON_PROC*)&py3_Py_Mapping_ଚାବିଗୁଡ଼ିକ},
# ଯଦି (defined(USE_LIMITED_API) && Py_LIMITED_API >= 0x03080000) || \
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
# ଯଦି PY_VERSION_HEX >= 0x03040000
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
# ଯଦି !defined(USE_LIMITED_API) && PY_VERSION_HEX < 0x030D0000
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
#  ଯଦି Py_LIMITED_API >= 0x030a0000
    {"PyUnicode_AsUTF8AndSize", (PYTHON_PROC*)&py3_PyUnicode_AsUTF8AndSize},
#  endif
# ଅନ୍ୟଥା
#  ଯଦି PY_VERSION_HEX >= 0x030300f0
    {"PyUnicode_AsUTF8AndSize", (PYTHON_PROC*)&py3_PyUnicode_AsUTF8AndSize},
#  ଅନ୍ୟଥା
    {"_PyUnicode_AsString", (PYTHON_PROC*)&py3__PyUnicode_AsString},
#  endif
# endif
    {"PyUnicode_CompareWithASCIIString", (PYTHON_PROC*)&py3_PyUnicode_CompareWithASCIIString},
    {"PyUnicode_AsUTF8String", (PYTHON_PROC*)&py3_PyUnicode_AsUTF8String},
# ifdef Py_UNICODE_USE_UCS_FUNCTIONS
#  ifdef Py_UNICODE_WIDE
    {"PyUnicodeUCS4_FromFormat", (PYTHON_PROC*)&py3_PyUnicodeUCS4_FromFormat},
#  ଅନ୍ୟଥା
    {"PyUnicodeUCS2_FromFormat", (PYTHON_PROC*)&py3_PyUnicodeUCS2_FromFormat},
#  endif
# ଅନ୍ୟଥା
    {"PyUnicode_FromFormat", (PYTHON_PROC*)&py3_PyUnicode_FromFormat},
# endif
    {"PyBytes_AsString", (PYTHON_PROC*)&py3_PyBytes_AsString},
    {"PyBytes_AsStringAndSize", (PYTHON_PROC*)&py3_PyBytes_AsStringAndSize},
    {"PyBytes_FromString", (PYTHON_PROC*)&py3_PyBytes_FromString},
    {"PyBytes_FromStringAndSize", (PYTHON_PROC*)&py3_PyBytes_FromStringAndSize},
# ଯଦି PY_VERSION_HEX >= 0x030900b0
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
    {"PyNumber_Check", (PYTHON_PROC*)&py3_Py_ସଂଖ୍ୟା_Check},
    {"PyNumber_Long", (PYTHON_PROC*)&py3_Py_ସଂଖ୍ୟା_Long},
    {"PyErr_NewException", (PYTHON_PROC*)&py3_PyErr_NewException},
# ifdef Py_DEBUG
    {"_Py_NegativeRefcount", (PYTHON_PROC*)&py3__Py_NegativeRefcount},
    {"_Py_RefTotal", (PYTHON_PROC*)&py3__Py_RefTotal},
    {"PyModule_Create2TraceRefs", (PYTHON_PROC*)&py3_PyModule_Create2TraceRefs},
# ଅନ୍ୟଥା
    {"PyModule_Create2", (PYTHON_PROC*)&py3_PyModule_Create2},
# endif
# ଯଦି defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
    {"_PyObject_DebugFree", (PYTHON_PROC*)&py3__PyObject_DebugFree},
    {"_PyObject_DebugMalloc", (PYTHON_PROC*)&py3__PyObject_DebugMalloc},
# ଅନ୍ୟଥା
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
#  ଯଦି PY_VERSION_HEX >= 0x03040000
    {"PyType_GetSlot", (PYTHON_PROC*)&py3_PyType_GetSlot},
#  endif
    {"PyType_FromSpec", (PYTHON_PROC*)&py3_PyType_FromSpec},
# endif
    {"", ଖାଲି},
};

# ଯଦି defined(USE_LIMITED_API) || PY_VERSION_HEX >= 0x03080000
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

# ଯଦି PY_VERSION_HEX >= 0x030900b0
    ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା
py3_PyType_HasFeature(PyTypeObject *type, unsigned long feature)
{
    ଫେରନ୍ତୁ ((PyType_GetFlags(type) & feature) != 0);
}
#  define PyType_HasFeature(t,f) py3_PyType_HasFeature(t,f)
# endif

# ଯଦି PY_VERSION_HEX >= 0x030a00b2
    ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା
py3__PyObject_TypeCheck(PyObject *ob, PyTypeObject *type)
{
    ଫେରନ୍ତୁ Py_IS_TYPE(ob, type) || PyType_IsSubtype(Py_TYPE(ob), type);
}
#  ଯଦି PY_VERSION_HEX >= 0x030b00b3
#   undef PyObject_TypeCheck
#   define PyObject_TypeCheck(o,t) py3__PyObject_TypeCheck(o,t)
#  ଅନ୍ୟଥା
#   define _PyObject_TypeCheck(o,t) py3__PyObject_TypeCheck(o,t)
#  endif
# endif

# ଯଦି !defined(USE_LIMITED_API) && PY_VERSION_HEX >= 0x030c00b0
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
    ସ୍ଥିର WCHAR *
py3_get_system_libname(ଅଚଳ ଅକ୍ଷର *libname)
{
    ଅଚଳ WCHAR	*pythoncore = L"Software\\Python\\PythonCore";
    ଅଚଳ ଅକ୍ଷର	*cp = libname;
    WCHAR	subkey[128];
    HKEY	h_ଚାବି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    DWORD	j, len;
    LSTATUS	ret;

    ଯେତେବେଳେ (*cp != '\0')
    {
	ଯଦି (*cp == ':' || *cp == '\\' || *cp == '/')
	{
	    // Bail out if "libname" contains path separator, assume it is
	    // an absolute path.
	    ଫେରନ୍ତୁ ଖାଲି;
	}
	++cp;
    }

    WCHAR   keyfound[32];
    HKEY    h_ଚାବି_ଶୀର୍ଷ[] = {HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE};
    HKEY    h_ଚାବି_Found = ଖାଲି;
#  ifdef USE_LIMITED_API
    long    maxminor = -1;
#  endif
    ପାଇଁ (i = 0; i < ARRAY_LENGTH(h_ଚାବି_ଶୀର୍ଷ); i++)
    {
	long	major, minor;

	ret = Reg_Open_ଚାବି_Ex_W(h_ଚାବି_ଶୀର୍ଷ[i], pythoncore, 0, ଚାବି_READ, &h_ଚାବି);
	ଯଦି (ret != ERROR_SUCCESS)
	    ଜାରି;
	ପାଇଁ (j = 0;; j++)
	{
	    WCHAR   keyname[32];
	    WCHAR   *wp;

	    len = ARRAY_LENGTH(keyname);
	    ret = Reg_Enum_ଚାବି_Ex_W(h_ଚାବି, j, keyname, &len,
						    ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି);
	    ଯଦି (ret == ERROR_NO_MORE_ବସ୍ତୁଗୁଡ଼ିକ)
		ଭାଙ୍ଗ;

	    major = wcstol(keyname, &wp, 10);
	    ଯଦି (*wp != L'.')
		ଜାରି;
	    minor = wcstol(wp + 1, &wp, 10);
#  ifdef _WIN64
	    ଯଦି (*wp != L'\0')
		ଜାରି;
#  ଅନ୍ୟଥା
	    ଯଦି (wcscmp(wp, L"-32") != 0)
		ଜାରି;
#  endif

	    ଯଦି (major != PY_MAJOR_VERSION)
		ଜାରି;
#  ifdef USE_LIMITED_API
	    // Search the latest version.
	    ଯଦି ((minor > maxminor)
		    && (minor >= ((Py_LIMITED_API >> 16) & 0xff)))
	    {
		maxminor = minor;
		wcscpy(keyfound, keyname);
		h_ଚାବି_Found = h_ଚାବି_ଶୀର୍ଷ[i];
	    }
#  ଅନ୍ୟଥା
	    // Check if it matches with the compiled version.
	    ଯଦି (minor == PY_MINOR_VERSION)
	    {
		wcscpy(keyfound, keyname);
		h_ଚାବି_Found = h_ଚାବି_ଶୀର୍ଷ[i];
		ଭାଙ୍ଗ;
	    }
#  endif
	}
	Reg_Close_ଚାବି(h_ଚାବି);
#  ifdef USE_LIMITED_API
	ଯଦି (h_ଚାବି_Found != ଖାଲି)
	    ଭାଙ୍ଗ;
#  endif
    }
    ଯଦି (h_ଚାବି_Found == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    swprintf(subkey, ARRAY_LENGTH(subkey), L"%ls\\%ls\\InstallPath",
							pythoncore, keyfound);
    ret = Reg_Get_ମୂଲ୍ୟ_W(h_ଚାବି_Found, subkey, ଖାଲି, RRF_RT_REG_SZ,
							    ଖାଲି, ଖାଲି, &len);
    ଯଦି (ret != ERROR_MORE_DATA && ret != ERROR_SUCCESS)
	ଫେରନ୍ତୁ ଖାଲି;
    size_t len2 = len / ଆକାର(WCHAR) + 1 + strlen(libname);
    WCHAR *path = alloc(len2 * ଆକାର(WCHAR));
    ଯଦି (path == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    ret = Reg_Get_ମୂଲ୍ୟ_W(h_ଚାବି_Found, subkey, ଖାଲି, RRF_RT_REG_SZ,
							    ଖାଲି, path, &len);
    ଯଦି (ret != ERROR_SUCCESS)
    {
	vim_free(path);
	ଫେରନ୍ତୁ ଖାଲି;
    }
    // Remove trailing path separators.
    size_t len3 = wcslen(path);
    ଯଦି ((len3 > 0) && (path[len3 - 1] == L'/' || path[len3 - 1] == L'\\'))
	--len3;
    swprintf(path + len3, len2 - len3, L"\\%hs", libname);
    ଫେରନ୍ତୁ path;
}
# endif

/*
 * Load library and get all pointers.
 * Parameter 'libname' provides name of DLL.
 * Return OK or FAIL.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
py3_runtime_link_init(ଅକ୍ଷର *libname, ପୂର୍ଣ୍ଣସଂଖ୍ୟା verbose)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
    PYTHON_PROC *ucs_from_string = (PYTHON_PROC *)&py3_PyUnicode_FromString;
    PYTHON_PROC *ucs_decode = (PYTHON_PROC *)&py3_PyUnicode_Decode;
    PYTHON_PROC *ucs_as_encoded_string =
				 (PYTHON_PROC *)&py3_PyUnicode_AsEncodedString;

# ଯଦି !(defined(PY_NO_RTLD_GLOBAL) && defined(PY3_NO_RTLD_GLOBAL)) && defined(UNIX) && defined(FEAT_PYTHON)
    // Can't have Python and Python3 loaded at the same time.
    // It causes a crash, because RTLD_GLOBAL is needed for
    // standard C extension libraries of one or both python versions.
    ଯଦି (python_loaded())
    {
	ଯଦି (verbose)
	    emsg(_(e_this_vim_cannot_execute_py3_after_using_python));
	ଫେରନ୍ତୁ FAIL;
    }
# endif

    ଯଦି (hinstPy3 != 0)
	ଫେରନ୍ତୁ OK;
    hinstPy3 = load_dll(libname);

# ifdef MSWIN
    ଯଦି (!hinstPy3)
    {
	// Attempt to use the path from InstallPath as stored in the registry.
	WCHAR *syslibname = py3_get_system_libname(libname);

	ଯଦି (syslibname != ଖାଲି)
	{
	    hinstPy3 = LoadLibraryExW(syslibname, ଖାଲି,
		    LOAD_LIBRARY_ସନ୍ଧାନ_DLL_LOAD_DIR |
		    LOAD_LIBRARY_ସନ୍ଧାନ_SYSTEM32);
	    vim_free(syslibname);
	}
    }
# endif

    ଯଦି (!hinstPy3)
    {
	ଯଦି (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	ଫେରନ୍ତୁ FAIL;
    }

    ପାଇଁ (i = 0; py3_funcname_table[i].ptr; ++i)
    {
	ଯଦି ((*py3_funcname_table[i].ptr = ପ୍ରତୀକ_from_dll(hinstPy3,
			py3_funcname_table[i].name)) == ଖାଲି)
	{
	    close_dll(hinstPy3);
	    hinstPy3 = 0;
	    ଯଦି (verbose)
		semsg(_(e_could_not_load_library_function_str), py3_funcname_table[i].name);
	    ଫେରନ୍ତୁ FAIL;
	}
    }

    // Load unicode functions separately as only the ucs2 or the ucs4 functions
    // will be present in the library.
# ଯଦି PY_VERSION_HEX >= 0x030300f0
    *ucs_from_string = ପ୍ରତୀକ_from_dll(hinstPy3, "PyUnicode_FromString");
    *ucs_decode = ପ୍ରତୀକ_from_dll(hinstPy3, "PyUnicode_Decode");
    *ucs_as_encoded_string = ପ୍ରତୀକ_from_dll(hinstPy3,
	    "PyUnicode_AsEncodedString");
# ଅନ୍ୟଥା
    *ucs_from_string = ପ୍ରତୀକ_from_dll(hinstPy3, "PyUnicodeUCS2_FromString");
    *ucs_decode = ପ୍ରତୀକ_from_dll(hinstPy3,
	    "PyUnicodeUCS2_Decode");
    *ucs_as_encoded_string = ପ୍ରତୀକ_from_dll(hinstPy3,
	    "PyUnicodeUCS2_AsEncodedString");
    ଯଦି (*ucs_from_string == ଖାଲି || *ucs_decode == ଖାଲି
					     || *ucs_as_encoded_string == ଖାଲି)
    {
	*ucs_from_string = ପ୍ରତୀକ_from_dll(hinstPy3,
		"PyUnicodeUCS4_FromString");
	*ucs_decode = ପ୍ରତୀକ_from_dll(hinstPy3,
		"PyUnicodeUCS4_Decode");
	*ucs_as_encoded_string = ପ୍ରତୀକ_from_dll(hinstPy3,
		"PyUnicodeUCS4_AsEncodedString");
    }
# endif
    ଯଦି (*ucs_from_string == ଖାଲି || *ucs_decode == ଖାଲି
					     || *ucs_as_encoded_string == ଖାଲି)
    {
	close_dll(hinstPy3);
	hinstPy3 = 0;
	ଯଦି (verbose)
	    semsg(_(e_could_not_load_library_function_str), "PyUnicode_UCSX_*");
	ଫେରନ୍ତୁ FAIL;
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * If python is enabled (there is installed python on Windows system) return
 * TRUE, else FALSE.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
python3_enabled(ପୂର୍ଣ୍ଣସଂଖ୍ୟା verbose)
{
    ଫେରନ୍ତୁ py3_runtime_link_init((ଅକ୍ଷର *)p_py3dll, verbose) == OK;
}

/*
 * Load the standard Python exceptions - don't import the symbols from the
 * DLL, as this can cause errors (importing data symbols is not reliable).
 */
    ସ୍ଥିର ଶୂନ୍ୟ
get_py3_exceptions(ଶୂନ୍ୟ)
{
    PyObject *exmod = PyImport_ImportModule("builtins");
    PyObject *exdict = PyModule_GetDict(exmod);
    p3imp_PyExc_AttributeError = PyDict_GetItemString(exdict, "AttributeError");
    p3imp_PyExc_IndexError = PyDict_GetItemString(exdict, "IndexError");
    p3imp_Py_Exc_ଚାବି_Error = PyDict_GetItemString(exdict, "KeyError");
    p3imp_PyExc_KeyboardInterrupt = PyDict_GetItemString(exdict, "KeyboardInterrupt");
    p3imp_PyExc_TypeError = PyDict_GetItemString(exdict, "TypeError");
    p3imp_Py_Exc_ମୂଲ୍ୟ_Error = PyDict_GetItemString(exdict, "ValueError");
    p3imp_PyExc_SystemExit = PyDict_GetItemString(exdict, "SystemExit");
    p3imp_PyExc_RuntimeError = PyDict_GetItemString(exdict, "RuntimeError");
    p3imp_PyExc_ImportError = PyDict_GetItemString(exdict, "ImportError");
    p3imp_PyExc_OverflowError = PyDict_GetItemString(exdict, "OverflowError");
    Py_XINCREF(p3imp_PyExc_AttributeError);
    Py_XINCREF(p3imp_PyExc_IndexError);
    Py_XINCREF(p3imp_Py_Exc_ଚାବି_Error);
    Py_XINCREF(p3imp_PyExc_KeyboardInterrupt);
    Py_XINCREF(p3imp_PyExc_TypeError);
    Py_XINCREF(p3imp_Py_Exc_ମୂଲ୍ୟ_Error);
    Py_XINCREF(p3imp_PyExc_SystemExit);
    Py_XINCREF(p3imp_PyExc_RuntimeError);
    Py_XINCREF(p3imp_PyExc_ImportError);
    Py_XINCREF(p3imp_PyExc_OverflowError);
    Py_XDECREF(exmod);
}
#endif // DYNAMIC_PYTHON3

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା py3initialised = 0;
#define PYINITIALISED py3initialised
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା python_end_called = FALSE;

#ifdef USE_LIMITED_API
# define DESTRUCTOR_FINISH(self) \
    ((freefunc)PyType_GetSlot(Py_TYPE((PyObject*)self), Py_tp_free))((PyObject*)self)
#ଅନ୍ୟଥା
# define DESTRUCTOR_FINISH(self) Py_TYPE(self)->tp_free((PyObject*)self)
#endif

#define WIN_PYTHON_REF(win) win->w_python3_ref
#define BUF_PYTHON_REF(buf) buf->b_python3_ref
#define TAB_PYTHON_REF(tab) tab->tp_python3_ref

    ସ୍ଥିର ଶୂନ୍ୟ
call_PyObject_Free(ଶୂନ୍ୟ *p)
{
#ଯଦି defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
    _PyObject_DebugFree(p);
#ଅନ୍ୟଥା
    PyObject_Free(p);
#endif
}

    ସ୍ଥିର PyObject *
call_PyType_GenericNew(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    ଫେରନ୍ତୁ PyType_GenericNew(type,args,kwds);
}

    ସ୍ଥିର PyObject *
call_PyType_GenericAlloc(PyTypeObject *type, Py_ssize_t nitems)
{
    ଫେରନ୍ତୁ PyType_GenericAlloc(type,nitems);
}

ସ୍ଥିର PyObject *OutputGetattro(PyObject *, PyObject *);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା OutputSetattro(PyObject *, PyObject *, PyObject *);
ସ୍ଥିର PyObject *BufferGetattro(PyObject *, PyObject *);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା BufferSetattro(PyObject *, PyObject *, PyObject *);
ସ୍ଥିର PyObject *TabPageGetattro(PyObject *, PyObject *);
ସ୍ଥିର PyObject *WindowGetattro(PyObject *, PyObject *);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା WindowSetattro(PyObject *, PyObject *, PyObject *);
ସ୍ଥିର PyObject *RangeGetattro(PyObject *, PyObject *);
ସ୍ଥିର PyObject *CurrentGetattro(PyObject *, PyObject *);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା CurrentSetattro(PyObject *, PyObject *, PyObject *);
ସ୍ଥିର PyObject *DictionaryGetattro(PyObject *, PyObject *);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା DictionarySetattro(PyObject *, PyObject *, PyObject *);
ସ୍ଥିର PyObject *ListGetattro(PyObject *, PyObject *);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ListSetattro(PyObject *, PyObject *, PyObject *);
ସ୍ଥିର PyObject *TupleGetattro(PyObject *, PyObject *);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା TupleSetattro(PyObject *, PyObject *, PyObject *);
ସ୍ଥିର PyObject *FunctionGetattro(PyObject *, PyObject *);

ସ୍ଥିର ସଂରଚନା PyModuleDef vimmodule;

#define PY_CAN_RECURSE

/*
 * Include the code shared with if_python.c
 */
#ଅନ୍ତର୍ଭାଵ "if_py_both.h"

#ifdef USE_LIMITED_API
# ଯଦି Py_LIMITED_API >= 0x030A0000
#  define PY_UNICODE_GET_UTF8_CHARS(obj) PyUnicode_AsUTF8AndSize(obj, ଖାଲି)
# ଅନ୍ୟଥା
// Python limited API before 3.10 lack easy ways to query the raw UTF-8 chars.
// We need to first convert the string to bytes, and then extract the chars.
// This function is only used for attribute string comparisons, which have
// known short length. As such, just allocate a short static buffer to hold
// the characters instead of having to allocate/deallcoate it.
//
// An alternative would be to convert all attribute string comparisons to use
// PyUnicode_CompareWithASCIIString to skip having to extract the chars.
ସ୍ଥିର ଅକ୍ଷର py3_unicode_utf8_chars[20];
ସ୍ଥିର ଅକ୍ଷର* PY_UNICODE_GET_UTF8_CHARS(PyObject* str)
{
    py3_unicode_utf8_chars[0] = '\0';
    PyObject* bytes = PyUnicode_AsUTF8String(str);
    ଯଦି (bytes)
    {
	ଅକ୍ଷର *chars;
	Py_ssize_t len;
	ଯଦି (PyBytes_AsStringAndSize(bytes, &chars, &len) != -1)
	{
	    ଯଦି (len < (Py_ssize_t)ଆକାର(py3_unicode_utf8_chars))
		// PyBytes_AsStringAndSize guarantees null-termination
		memcpy(py3_unicode_utf8_chars, chars, len + 1);
	}
	Py_DECREF(bytes);
    }
    ଫେରନ୍ତୁ py3_unicode_utf8_chars;
}
# endif
#ଅନ୍ୟଥା	// !USE_LIMITED_API
# ଯଦି PY_VERSION_HEX >= 0x030300f0
#  define PY_UNICODE_GET_UTF8_CHARS(obj) PyUnicode_AsUTF8AndSize(obj, ଖାଲି)
# ଅନ୍ୟଥା
#  define PY_UNICODE_GET_UTF8_CHARS _PyUnicode_AsString
# endif
#endif

// NOTE: Must always be used at the start of a block, since it declares "name".
#define GET_ATTR_STRING(name, nameobj) \
    ଅକ୍ଷର	*name = ""; \
    ଯଦି (PyUnicode_Check(nameobj)) \
	name = (ଅକ୍ଷର *)PY_UNICODE_GET_UTF8_CHARS(nameobj)

#define PY3OBJ_DELETED(obj) (obj->ob_base.ob_refcnt<=0)

///////////////////////////////////////////////////////
// Internal function prototypes.

ସ୍ଥିର PyObject *Py3Init_vim(ଶୂନ୍ୟ);

///////////////////////////////////////////////////////
// 1. Python interpreter main program.

    ଶୂନ୍ୟ
python3_end(ଶୂନ୍ୟ)
{
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା recurse = 0;

    // If a crash occurs while doing this, don't try again.
    ଯଦି (recurse != 0)
	ଫେରନ୍ତୁ;

    python_end_called = TRUE;
    ++recurse;

#ifdef DYNAMIC_PYTHON3
    ଯଦି (hinstPy3)
#endif
    ଯଦି (Py_IsInitialized())
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

#ଯଦି defined(DYNAMIC_PYTHON3) && defined(DYNAMIC_PYTHON) && defined(FEAT_PYTHON) && defined(UNIX)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
python3_loaded(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ (hinstPy3 != 0);
}
#endif

ସ୍ଥିର wchar_t *py_home_buf = ଖାଲି;

#ଯଦି defined(MSWIN) && (PY_VERSION_HEX >= 0x030500f0)
/*
 * Return TRUE if stdin is readable from Python 3.
 */
    ସ୍ଥିର BOOL
is_stdin_readable(ଶୂନ୍ୟ)
{
    DWORD	    mode, eventnum;
    ସଂରଚନା _stat    st;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    fd = fileno(stdin);
    HANDLE	    hstdin = (HANDLE)_get_osfhandle(fd);

    // Check if stdin is connected to the console.
    ଯଦି (GetConsoleMode(hstdin, &mode))
	// Check if it is opened as input.
	ଫେରନ୍ତୁ Get_ସଂଖ୍ୟା_Of_Console_ନିବେଶ_Events(hstdin, &eventnum);

    ଫେରନ୍ତୁ _fstat(fd, &st) == 0;
}

// Python 3.5 or later will abort inside Py_Initialize() when stdin has
// been closed (i.e. executed by "vim -").  Reconnect stdin to CONIN$.
// Note that the python DLL is linked to its own stdio DLL which can be
// differ from Vim's stdio.
    ସ୍ଥିର ଶୂନ୍ୟ
reset_stdin(ଶୂନ୍ୟ)
{
    FILE *(*py__acrt_iob_func)(unsigned) = ଖାଲି;
    FILE *(*pyfreopen)(ଅଚଳ ଅକ୍ଷର *, ଅଚଳ ଅକ୍ଷର *, FILE *) = ଖାଲି;
    HINSTANCE hinst = get_forwarded_dll(hinstPy3);

    ଯଦି (hinst == ଖାଲି || is_stdin_readable())
	ଫେରନ୍ତୁ;

    // Get "freopen" and "stdin" which are used in the python DLL.
    // "stdin" is defined as "__acrt_iob_func(0)" in VC++ 2015 or later.
    py__acrt_iob_func = get_dll_import_func(hinst, "__acrt_iob_func");
    ଯଦି (py__acrt_iob_func)
    {
	HINSTANCE hpystdiodll = find_imported_module_by_funcname(hinst,
							    "__acrt_iob_func");
	ଯଦି (hpystdiodll)
	    pyfreopen = (ଶୂନ୍ୟ *)GetProcAddress(hpystdiodll, "freopen");
    }

    // Reconnect stdin to CONIN$.
    ଯଦି (pyfreopen != ଖାଲି)
	pyfreopen("CONIN$", "r", py__acrt_iob_func(0));
    ଅନ୍ୟଥା
	freopen("CONIN$", "r", stdin);
}
#ଅନ୍ୟଥା
# define reset_stdin()
#endif

// Python 3.2 or later will abort inside Py_Initialize() when mandatory
// modules cannot be loaded (e.g. 'pythonthreehome' is wrongly set.).
// Install a hook to python dll's exit() and recover from it.
#ଯଦି defined(MSWIN) && (PY_VERSION_HEX >= 0x030200f0)
# define HOOK_EXIT
# include <setjmp.h>

ସ୍ଥିର jmp_buf exit_hook_jump_buf;
ସ୍ଥିର ଶୂନ୍ୟ *orig_exit = ଖାଲି;

/*
 * Function that replaces exit() while calling Py_Initialize().
 */
    ସ୍ଥିର ଶୂନ୍ୟ
hooked_exit(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret)
{
    // Recover from exit.
    longjmp(exit_hook_jump_buf, 1);
}

/*
 * Install a hook to python dll's exit().
 */
    ସ୍ଥିର ଶୂନ୍ୟ
hook_py_exit(ଶୂନ୍ୟ)
{
    HINSTANCE hinst = get_forwarded_dll(hinstPy3);

    ଯଦି (hinst == ଖାଲି || orig_exit != ଖାଲି)
	ଫେରନ୍ତୁ;

    orig_exit = hook_dll_import_func(hinst, "exit", (ଶୂନ୍ୟ *)hooked_exit);
}

/*
 * Remove the hook installed by hook_py_exit().
 */
    ସ୍ଥିର ଶୂନ୍ୟ
restore_py_exit(ଶୂନ୍ୟ)
{
    HINSTANCE hinst = hinstPy3;

    ଯଦି (hinst == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (orig_exit != ଖାଲି)
	hook_dll_import_func(hinst, "exit", orig_exit);
    orig_exit = ଖାଲି;
}
#endif

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
Python3_Init(ଶୂନ୍ୟ)
{
    ଯଦି (!py3initialised)
    {
#ifdef DYNAMIC_PYTHON3
	ଯଦି (!python3_enabled(TRUE))
	{
	    emsg(_(e_sorry_this_command_is_disabled_python_library_could_not_be_found));
	    ଯାଅ fail;
	}
#endif
	// Python 3.13 introduced a really useful feature: colorized exceptions.
	// This is great if you're reading them from the terminal, but useless
	// and broken everywhere else (such as in log files, or text editors).
	// Opt out, forcefully.
	vim_setenv((char_u*)"PYTHON_COLORS", (char_u*)"0");

	init_structs();

	ଯଦି (*p_py3home != NUL)
	{
	    size_t len = mbstowcs(ଖାଲି, (ଅକ୍ଷର *)p_py3home, 0) + 1;

	    // The string must not change later, make a copy in static memory.
	    py_home_buf = ALLOC_MULT(wchar_t, len);
	    ଯଦି (py_home_buf != ଖାଲି && mbstowcs(
			    py_home_buf, (ଅକ୍ଷର *)p_py3home, len) != (size_t)-1)
		Py_SetPythonHome(py_home_buf);
	}
#ifdef PYTHON3_HOME
	ଅନ୍ୟଥା ଯଦି (mch_getenv((char_u *)"PYTHONHOME") == ଖାଲି)
	    Py_SetPythonHome(PYTHON3_HOME);
#endif

	PyImport_AppendInittab("vim", Py3Init_vim);

#ଯଦି !defined(DYNAMIC_PYTHON3) && defined(MSWIN)
	hinstPy3 = GetModuleHandle(PYTHON3_DLL);
#endif
	reset_stdin();

#ifdef HOOK_EXIT
	// Catch exit() called in Py_Initialize().
	hook_py_exit();
	ଯଦି (setjmp(exit_hook_jump_buf) == 0)
	{
	    Py_Initialize();
	    restore_py_exit();
	}
	ଅନ୍ୟଥା
	{
	    // exit() was called in Py_Initialize().
	    restore_py_exit();
	    emsg(_(e_critical_error_in_python3_initialization_check_your_installation));
	    ଯାଅ fail;
	}
#ଅନ୍ୟଥା
	Py_Initialize();
#endif

#ଯଦି PY_VERSION_HEX < 0x03090000
	// Initialise threads.  This is deprecated since Python 3.9.
	PyEval_InitThreads();
#endif
#ifdef DYNAMIC_PYTHON3
	get_py3_exceptions();
#endif

	ଯଦି (PythonIO_Init_io())
	    ଯାଅ fail;

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

    ଫେରନ୍ତୁ 0;

fail:
    // We call PythonIO_Flush() here to print any Python errors.
    // This is OK, as it is possible to call this function even
    // if PythonIO_Init_io() has not completed successfully (it will
    // not do anything in this case).
    PythonIO_Flush();
    ଫେରନ୍ତୁ -1;
}

/*
 * External interface
 */
    ସ୍ଥିର ଶୂନ୍ୟ
DoPyCommand(ଅଚଳ ଅକ୍ଷର *cmd,
	    dict_T* locals,
	    rangeinitializer init_range,
	    runner run,
	    ଶୂନ୍ୟ *arg)
{
#ଯଦି defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    ଅକ୍ଷର		*saved_locale;
#endif
    PyObject		*cmdstr;
    PyObject		*cmdbytes;
    PyGILState_STATE	pygilstate;

    ଯଦି (python_end_called)
	ଯାଅ theend;

    ଯଦି (Python3_Init())
	ଯାଅ theend;

    init_range(arg);

    Python_Release_Vim();	    // leave Vim

#ଯଦି defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    // Python only works properly when the LC_NUMERIC locale is "C".
    saved_locale = setlocale(LC_NUMERIC, ଖାଲି);
    ଯଦି (saved_locale == ଖାଲି || STRCMP(saved_locale, "C") == 0)
	saved_locale = ଖାଲି;
    ଅନ୍ୟଥା
    {
	// Need to make a copy, value may change when setting new locale.
	saved_locale = (ଅକ୍ଷର *)vim_strsave((char_u *)saved_locale);
	(ଶୂନ୍ୟ)setlocale(LC_NUMERIC, "C");
    }
#endif

    pygilstate = PyGILState_Ensure();

    // PyRun_SimpleString expects a UTF-8 string. Wrong encoding may cause
    // SyntaxError (unicode error).
    cmdstr = PyUnicode_Decode(cmd, strlen(cmd),
					(ଅକ୍ଷର *)ENC_OPT, ERRORS_DECODE_ARG);
    cmdbytes = PyUnicode_AsEncodedString(cmdstr, "utf-8", ERRORS_ENCODE_ARG);
    Py_XDECREF(cmdstr);

    run(PyBytes_AsString(cmdbytes), locals, arg, &pygilstate);
    Py_XDECREF(cmdbytes);

    PyGILState_Release(pygilstate);

#ଯଦି defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    ଯଦି (saved_locale != ଖାଲି)
    {
	(ଶୂନ୍ୟ)setlocale(LC_NUMERIC, saved_locale);
	vim_free(saved_locale);
    }
#endif

    Python_Lock_Vim();		    // enter Vim
    PythonIO_Flush();

theend:
    ଫେରନ୍ତୁ;	    // keeps lint happy
}

/*
 * ":py3"
 */
    ଶୂନ୍ୟ
ex_py3(exarg_T *eap)
{
    char_u *script;

    script = script_get(eap, eap->arg);
    ଯଦି (!eap->skip)
    {
	ଯଦି (p_pyx == 0)
	    p_pyx = 3;

	DoPyCommand(script == ଖାଲି ? (ଅକ୍ଷର *) eap->arg : (ଅକ୍ଷର *) script,
		ଖାଲି,
		init_range_cmd,
		(runner) run_cmd,
		(ଶୂନ୍ୟ *) eap);
    }
    vim_free(script);
}

#define BUFFER_SIZE 2048

/*
 * ":py3file"
 */
    ଶୂନ୍ୟ
ex_py3file(exarg_T *eap)
{
    ସ୍ଥିର ଅକ୍ଷର buffer[BUFFER_SIZE];
    ଅଚଳ ଅକ୍ଷର *file;
    ଅକ୍ଷର *p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    ଯଦି (p_pyx == 0)
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

    ପାଇଁ (i=0; i<2; ++i)
    {
	file = (ଅକ୍ଷର *)eap->arg;
	ଯେତେବେଳେ (*file && p < buffer + (BUFFER_SIZE - 3))
	{
	    ଯଦି (*file == '\\' || *file == '\'')
		*p++ = '\\';
	    *p++ = *file++;
	}
	// If we didn't finish the file name, we hit a buffer overflow
	ଯଦି (*file != '\0')
	    ଫେରନ୍ତୁ;
	ଯଦି (i==0)
	{
	    strcpy(p,"','rb').read(),'");
	    p += 16;
	}
	ଅନ୍ୟଥା
	{
	    strcpy(p,"','exec'))");
	    p += 10;
	}
    }


    // Execute the file
    DoPyCommand(buffer,
	    ଖାଲି,
	    init_range_cmd,
	    (runner) run_cmd,
	    (ଶୂନ୍ୟ *) eap);
}

    ଶୂନ୍ୟ
ex_py3do(exarg_T *eap)
{
    ଯଦି (p_pyx == 0)
	p_pyx = 3;

    DoPyCommand((ଅକ୍ଷର *)eap->arg,
	    ଖାଲି,
	    init_range_cmd,
	    (runner)run_do,
	    (ଶୂନ୍ୟ *)eap);
}

///////////////////////////////////////////////////////
// 2. Python output stream: writes output via [e]msg().

// Implementation functions

    ସ୍ଥିର PyObject *
OutputGetattro(PyObject *self, PyObject *nameobj)
{
    GET_ATTR_STRING(name, nameobj);

    ଯଦି (strcmp(name, "softspace") == 0)
	ଫେରନ୍ତୁ PyLong_FromLong(((OutputObject *)(self))->softspace);
    ଅନ୍ୟଥା ଯଦି (strcmp(name, "errors") == 0)
	ଫେରନ୍ତୁ PyString_FromString("strict");
    ଅନ୍ୟଥା ଯଦି (strcmp(name, "encoding") == 0)
	ଫେରନ୍ତୁ PyString_FromString(ENC_OPT);

    ଫେରନ୍ତୁ PyObject_GenericGetAttr(self, nameobj);
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
OutputSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);

    ଫେରନ୍ତୁ OutputSetattr(self, name, val);
}

///////////////////////////////////////////////////////
// 3. Implementation of the Vim module for Python

// Window type - Implementation functions
// --------------------------------------

#define WindowType_Check(obj) ((obj)->ob_base.ob_type == &WindowType)

// Buffer type - Implementation functions
// --------------------------------------

#define BufferType_Check(obj) ((obj)->ob_base.ob_type == &BufferType)

ସ୍ଥିର PyObject* BufferSubscript(PyObject *self, PyObject *idx);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା BufferAsSubscript(PyObject *self, PyObject *idx, PyObject *val);

// Line range type - Implementation functions
// --------------------------------------

#define RangeType_Check(obj) ((obj)->ob_base.ob_type == &RangeType)

ସ୍ଥିର PyObject* RangeSubscript(PyObject *self, PyObject *idx);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା RangeAsItem(PyObject *, Py_ssize_t, PyObject *);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା RangeAsSubscript(PyObject *self, PyObject *idx, PyObject *val);

// Current objects type - Implementation functions
// -----------------------------------------------

ସ୍ଥିର PySequenceMethods BufferAsSeq = {
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

ସ୍ଥିର PyMappingMethods BufferAsMapping = {
    /* mp_length	*/ (lenfunc)BufferLength,
    /* mp_subscript     */ (binaryfunc)BufferSubscript,
    /* mp_ass_subscript */ (objobjargproc)BufferAsSubscript,
};


// Buffer object

    ସ୍ଥିର PyObject *
BufferGetattro(PyObject *self, PyObject *nameobj)
{
    PyObject *r;

    GET_ATTR_STRING(name, nameobj);

    ଯଦି ((r = BufferAttrValid((BufferObject *)(self), name)))
	ଫେରନ୍ତୁ r;

    ଯଦି (CheckBuffer((BufferObject *)(self)))
	ଫେରନ୍ତୁ ଖାଲି;

    r = BufferAttr((BufferObject *)(self), name);
    ଯଦି (r || PyErr_Occurred())
	ଫେରନ୍ତୁ r;
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ PyObject_GenericGetAttr(self, nameobj);
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
BufferSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);

    ଫେରନ୍ତୁ BufferSetattr(self, name, val);
}

//////////////////

    ସ୍ଥିର PyObject *
BufferSubscript(PyObject *self, PyObject* idx)
{
    ଯଦି (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	ଫେରନ୍ତୁ BufferItem((BufferObject *)(self), _idx);
    }
    ଅନ୍ୟଥା ଯଦି (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	ଯଦି (CheckBuffer((BufferObject *) self))
	    ଫେରନ୍ତୁ ଖାଲି;

	ଯଦି (PySlice_GetIndicesEx((PySliceObject_T *)idx,
	      (Py_ssize_t)((BufferObject *)(self))->buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା,
	      &start, &stop,
	      &step, &slicelen) < 0)
	    ଫେରନ୍ତୁ ଖାଲି;
	ଫେରନ୍ତୁ BufferSlice((BufferObject *)(self), start, stop);
    }
    ଅନ୍ୟଥା
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	ଫେରନ୍ତୁ ଖାଲି;
    }
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
BufferAsSubscript(PyObject *self, PyObject* idx, PyObject* val)
{
    ଯଦି (PyLong_Check(idx))
    {
	long n = PyLong_AsLong(idx);

	ଯଦି (CheckBuffer((BufferObject *) self))
	    ଫେରନ୍ତୁ -1;

	ଫେରନ୍ତୁ RBAsItem((BufferObject *)(self), n, val, 1,
		    (Py_ssize_t)((BufferObject *)(self))->buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା,
		    ଖାଲି);
    }
    ଅନ୍ୟଥା ଯଦି (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	ଯଦି (CheckBuffer((BufferObject *) self))
	    ଫେରନ୍ତୁ -1;

	ଯଦି (PySlice_GetIndicesEx((PySliceObject_T *)idx,
	      (Py_ssize_t)((BufferObject *)(self))->buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା,
	      &start, &stop,
	      &step, &slicelen) < 0)
	    ଫେରନ୍ତୁ -1;
	ଫେରନ୍ତୁ RBAsSlice((BufferObject *)(self), start, stop, val, 1,
			  (PyInt)((BufferObject *)(self))->buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା,
			  ଖାଲି);
    }
    ଅନ୍ୟଥା
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	ଫେରନ୍ତୁ -1;
    }
}

ସ୍ଥିର PySequenceMethods RangeAsSeq = {
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

ସ୍ଥିର PyMappingMethods RangeAsMapping = {
    /* mp_length	*/ (lenfunc)RangeLength,
    /* mp_subscript     */ (binaryfunc)RangeSubscript,
    /* mp_ass_subscript */ (objobjargproc)RangeAsSubscript,
};

// Line range object - Implementation

    ସ୍ଥିର PyObject *
RangeGetattro(PyObject *self, PyObject *nameobj)
{
    GET_ATTR_STRING(name, nameobj);

    ଯଦି (strcmp(name, "start") == 0)
	ଫେରନ୍ତୁ Py_Build_ମୂଲ୍ୟ("n", ((RangeObject *)(self))->start - 1);
    ଅନ୍ୟଥା ଯଦି (strcmp(name, "end") == 0)
	ଫେରନ୍ତୁ Py_Build_ମୂଲ୍ୟ("n", ((RangeObject *)(self))->end - 1);
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ PyObject_GenericGetAttr(self, nameobj);
}

////////////////

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
RangeAsItem(PyObject *self, Py_ssize_t n, PyObject *val)
{
    ଫେରନ୍ତୁ RBAsItem(((RangeObject *)(self))->buf, n, val,
		    ((RangeObject *)(self))->start,
		    ((RangeObject *)(self))->end,
		    &((RangeObject *)(self))->end);
}

    ସ୍ଥିର Py_ssize_t
RangeAsSlice(PyObject *self, Py_ssize_t lo, Py_ssize_t hi, PyObject *val)
{
    ଫେରନ୍ତୁ RBAsSlice(((RangeObject *)(self))->buf, lo, hi, val,
		    ((RangeObject *)(self))->start,
		    ((RangeObject *)(self))->end,
		    &((RangeObject *)(self))->end);
}

    ସ୍ଥିର PyObject *
RangeSubscript(PyObject *self, PyObject* idx)
{
    ଯଦି (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	ଫେରନ୍ତୁ RangeItem((RangeObject *)(self), _idx);
    }
    ଅନ୍ୟଥା ଯଦି (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	ଯଦି (PySlice_GetIndicesEx((PySliceObject_T *)idx,
		((RangeObject *)(self))->end-((RangeObject *)(self))->start+1,
		&start, &stop,
		&step, &slicelen) < 0)
	    ଫେରନ୍ତୁ ଖାଲି;
	ଫେରନ୍ତୁ RangeSlice((RangeObject *)(self), start, stop);
    }
    ଅନ୍ୟଥା
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	ଫେରନ୍ତୁ ଖାଲି;
    }
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
RangeAsSubscript(PyObject *self, PyObject *idx, PyObject *val)
{
    ଯଦି (PyLong_Check(idx))
    {
	long n = PyLong_AsLong(idx);
	ଫେରନ୍ତୁ RangeAsItem(self, n, val);
    }
    ଅନ୍ୟଥା ଯଦି (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	ଯଦି (PySlice_GetIndicesEx((PySliceObject_T *)idx,
		((RangeObject *)(self))->end-((RangeObject *)(self))->start+1,
		&start, &stop,
		&step, &slicelen) < 0)
	    ଫେରନ୍ତୁ -1;
	ଫେରନ୍ତୁ RangeAsSlice(self, start, stop, val);
    }
    ଅନ୍ୟଥା
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	ଫେରନ୍ତୁ -1;
    }
}

// TabPage object - Implementation

    ସ୍ଥିର PyObject *
TabPageGetattro(PyObject *self, PyObject *nameobj)
{
    PyObject *r;

    GET_ATTR_STRING(name, nameobj);

    ଯଦି ((r = TabPageAttrValid((TabPageObject *)(self), name)))
	ଫେରନ୍ତୁ r;

    ଯଦି (CheckTabPage((TabPageObject *)(self)))
	ଫେରନ୍ତୁ ଖାଲି;

    r = TabPageAttr((TabPageObject *)(self), name);
    ଯଦି (r || PyErr_Occurred())
	ଫେରନ୍ତୁ r;
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ PyObject_GenericGetAttr(self, nameobj);
}

// Window object - Implementation

    ସ୍ଥିର PyObject *
WindowGetattro(PyObject *self, PyObject *nameobj)
{
    PyObject *r;

    GET_ATTR_STRING(name, nameobj);

    ଯଦି ((r = WindowAttrValid((WindowObject *)(self), name)))
	ଫେରନ୍ତୁ r;

    ଯଦି (CheckWindow((WindowObject *)(self)))
	ଫେରନ୍ତୁ ଖାଲି;

    r = WindowAttr((WindowObject *)(self), name);
    ଯଦି (r || PyErr_Occurred())
	ଫେରନ୍ତୁ r;
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ PyObject_GenericGetAttr(self, nameobj);
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
WindowSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);

    ଫେରନ୍ତୁ WindowSetattr(self, name, val);
}

// Tab page list object - Definitions

ସ୍ଥିର PySequenceMethods TabListAsSeq = {
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

ସ୍ଥିର PySequenceMethods WinListAsSeq = {
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
    ସ୍ଥିର PyObject *
CurrentGetattro(PyObject *self, PyObject *nameobj)
{
    PyObject	*r;
    GET_ATTR_STRING(name, nameobj);
    ଯଦି (!(r = CurrentGetattr(self, name)))
	ଫେରନ୍ତୁ PyObject_GenericGetAttr(self, nameobj);
    ଫେରନ୍ତୁ r;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
CurrentSetattro(PyObject *self, PyObject *nameobj, PyObject *ମୂଲ୍ୟ)
{
    GET_ATTR_STRING(name, nameobj);
    ଫେରନ୍ତୁ CurrentSetattr(self, name, ମୂଲ୍ୟ);
}

// Dictionary object - Definitions

    ସ୍ଥିର PyObject *
DictionaryGetattro(PyObject *self, PyObject *nameobj)
{
    DictionaryObject	*this = ((DictionaryObject *) (self));

    GET_ATTR_STRING(name, nameobj);

    ଯଦି (strcmp(name, "locked") == 0)
	ଫେରନ୍ତୁ PyLong_FromLong(this->dict->dv_lock);
    ଅନ୍ୟଥା ଯଦି (strcmp(name, "scope") == 0)
	ଫେରନ୍ତୁ PyLong_FromLong(this->dict->dv_scope);

    ଫେରନ୍ତୁ PyObject_GenericGetAttr(self, nameobj);
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
DictionarySetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);
    ଫେରନ୍ତୁ DictionarySetattr(self, name, val);
}

// List object - Definitions

    ସ୍ଥିର PyObject *
ListGetattro(PyObject *self, PyObject *nameobj)
{
    GET_ATTR_STRING(name, nameobj);

    ଯଦି (strcmp(name, "locked") == 0)
	ଫେରନ୍ତୁ PyLong_FromLong(((ListObject *) (self))->list->lv_lock);

    ଫେରନ୍ତୁ PyObject_GenericGetAttr(self, nameobj);
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ListSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);
    ଫେରନ୍ତୁ ListSetattr(self, name, val);
}

// Tuple object - Definitions

    ସ୍ଥିର PyObject *
TupleGetattro(PyObject *self, PyObject *nameobj)
{
    GET_ATTR_STRING(name, nameobj);

    ଯଦି (strcmp(name, "locked") == 0)
	ଫେରନ୍ତୁ PyLong_FromLong(((TupleObject *) (self))->tuple->tv_lock);

    ଫେରନ୍ତୁ PyObject_GenericGetAttr(self, nameobj);
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
TupleSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);
    ଫେରନ୍ତୁ TupleSetattr(self, name, val);
}

// Function object - Definitions

    ସ୍ଥିର PyObject *
FunctionGetattro(PyObject *self, PyObject *nameobj)
{
    PyObject		*r;
    FunctionObject	*this = (FunctionObject *)(self);

    GET_ATTR_STRING(name, nameobj);

    r = FunctionAttr(this, name);
    ଯଦି (r || PyErr_Occurred())
	ଫେରନ୍ତୁ r;
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ PyObject_GenericGetAttr(self, nameobj);
}

// External interface

    ଶୂନ୍ୟ
python3_buffer_free(buf_T *buf)
{
    BufferObject *bp = BUF_PYTHON_REF(buf);
    ଯଦି (bp == ଖାଲି)
	ଫେରନ୍ତୁ;
    bp->buf = INVALID_BUFFER_ମୂଲ୍ୟ;
    BUF_PYTHON_REF(buf) = ଖାଲି;
}

    ଶୂନ୍ୟ
python3_window_free(win_T *win)
{
    WindowObject *wp = WIN_PYTHON_REF(win);
    ଯଦି (wp == ଖାଲି)
	ଫେରନ୍ତୁ;
    wp->win = INVALID_WINDOW_ମୂଲ୍ୟ;
    WIN_PYTHON_REF(win) = ଖାଲି;
}

    ଶୂନ୍ୟ
python3_tabpage_free(tabpage_T *tab)
{
    TabPageObject *tp = TAB_PYTHON_REF(tab);
    ଯଦି (tp == ଖାଲି)
	ଫେରନ୍ତୁ;
    tp->tab = INVALID_TABPAGE_ମୂଲ୍ୟ;
    TAB_PYTHON_REF(tab) = ଖାଲି;
}

    ସ୍ଥିର PyObject *
Py3Init_vim(ଶୂନ୍ୟ)
{
    // The special value is removed from sys.path in Python3_Init().
    ସ୍ଥିର wchar_t *(argv[2]) = {L"/must>not&exist/foo", ଖାଲି};

    ଯଦି (init_types())
	ଫେରନ୍ତୁ ଖାଲି;

    // Set sys.argv[] to avoid a crash in warn().
    PySys_SetArgv(1, argv);

    ଯଦି ((vim_module = PyModule_Create(&vimmodule)) == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (populate_module(vim_module))
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (init_sys_path())
	ଫେରନ୍ତୁ ଖାଲି;

    ଫେରନ୍ତୁ vim_module;
}

//////////////////////////////////////////////////////////////////////////
// 4. Utility functions for handling the interface between Vim and Python.

/*
 * Convert a Vim line into a Python string.
 * All internal newlines are replaced by null characters.
 *
 * On errors, the Python exception data is set, and NULL is returned.
 */
    ସ୍ଥିର PyObject *
ଧାଡ଼ି_To_String(ଅଚଳ ଅକ୍ଷର *str)
{
    PyObject *result;
    Py_ssize_t len = strlen(str);
    ଅକ୍ଷର *tmp, *p;

    tmp = alloc(len + 1);
    p = tmp;
    ଯଦି (p == ଖାଲି)
    {
	PyErr_NoMemory();
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ଯେତେବେଳେ (*str)
    {
	ଯଦି (*str == '\n')
	    *p = '\0';
	ଅନ୍ୟଥା
	    *p = *str;

	++p;
	++str;
    }
    *p = '\0';

    result = PyUnicode_Decode(tmp, len, (ଅକ୍ଷର *)ENC_OPT, ERRORS_DECODE_ARG);

    vim_free(tmp);
    ଫେରନ୍ତୁ result;
}

    ଶୂନ୍ୟ
do_py3eval(char_u *str, dict_T *locals, typval_T *rettv)
{
    DoPyCommand((ଅକ୍ଷର *) str,
	    locals,
	    init_range_eval,
	    (runner) run_eval,
	    (ଶୂନ୍ୟ *) rettv);
    ଯଦି (rettv->v_type == VAR_UNKNOWN)
    {
	rettv->v_type = VAR_ସଂଖ୍ୟା;
	rettv->vval.v_ସଂଖ୍ୟା = 0;
    }
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
set_ref_in_python3(ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID)
{
    ଫେରନ୍ତୁ set_ref_in_py(copyID);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
python3_version(ଶୂନ୍ୟ)
{
#ifdef USE_LIMITED_API
    ଫେରନ୍ତୁ Py_LIMITED_API;
#ଅନ୍ୟଥା
    ଫେରନ୍ତୁ PY_VERSION_HEX;
#endif
}
