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

#அந்தர்பாவ "vim.h"

#அந்தர்பாவ <limits.h>

#எனில் defined(MSWIN) && defined(HAVE_FCNTL_H)
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

#அந்தர்பாவ <Python.h>

#undef முதன்மை // Defined in python.h - aargh
#undef HAVE_FCNTL_H // Clash with os_win32.h

// The "surrogateescape" error handler is new in Python 3.1
#எனில் PY_VERSION_HEX >= 0x030100f0
# define CODEC_ERROR_HANDLER "surrogateescape"
#இல்லையெனில்
# define CODEC_ERROR_HANDLER வெற்று
#endif

// Suppress Python 3.11 depreciations to see useful warnings
#ifdef __GNUC__
# pragma GCC diagnostic தள்ளு
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
#எனில் PY_VERSION_HEX >= 0x030200f0
வகைவரை PyObject PySliceObject_T;
#இல்லையெனில்
வகைவரை PySliceObject PySliceObject_T;
#endif

#ifndef MSWIN
# define HINSTANCE வெறுமை *
#endif
#எனில் defined(DYNAMIC_PYTHON3) || defined(MSWIN)
நிலையான HINSTANCE hinstPy3 = 0; // Instance of python.dll
#endif

#எனில் defined(DYNAMIC_PYTHON3)

# ifdef MSWIN
#  define load_dll vimLoadLib
#  define close_dll FreeLibrary
#  define குறியீடு_from_dll GetProcAddress
#  define load_dll_error GetWin32Error
# இல்லையெனில்
#  include <dlfcn.h>
#  define FARPROC வெறுமை*
#  எனில் defined(PY_NO_RTLD_GLOBAL) && defined(PY3_NO_RTLD_GLOBAL)
#   define load_dll(n) dlopen((n), RTLD_LAZY)
#  இல்லையெனில்
#   define load_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#  endif
#  define close_dll dlclose
#  define குறியீடு_from_dll dlsym
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
# define Py_List_செருகு py3_Py_List_செருகு
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
# எனில் PY_VERSION_HEX >= 0x030601f0
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
# ifndef Py_Mapping_சாவிகள்
#  define Py_Mapping_சாவிகள் py3_Py_Mapping_சாவிகள்
# endif
# எனில் (defined(USE_LIMITED_API) && Py_LIMITED_API >= 0x03080000) || \
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
# எனில் defined(USE_LIMITED_API) || PY_VERSION_HEX >= 0x03080000
#  define Py_IncRef py3_Py_IncRef
#  define Py_DecRef py3_Py_DecRef
# endif
# ifdef USE_LIMITED_API
#  define Py_CompileString py3_Py_CompileString
#  define PyEval_EvalCode py3_PyEval_EvalCode
# இல்லையெனில்
#  undef PyRun_SimpleString
#  define PyRun_SimpleString py3_PyRun_SimpleString
#  undef PyRun_String
#  define PyRun_String py3_PyRun_String
# endif
# define PyObject_GetAttrString py3_PyObject_GetAttrString
# define PyObject_HasAttrString py3_PyObject_HasAttrString
# define PyObject_SetAttrString py3_PyObject_SetAttrString
# define PyObject_CallFunctionObjArgs py3_PyObject_CallFunctionObjArgs
# எனில் PY_VERSION_HEX >= 0x030d0000
#  define PyObject_CallFunction py3_PyObject_CallFunction
# இல்லையெனில்
#  define _PyObject_CallFunction_SizeT py3__PyObject_CallFunction_SizeT
# endif
# define PyObject_Call py3_PyObject_Call
# define PyEval_GetLocals py3_PyEval_GetLocals
# define PyEval_GetGlobals py3_PyEval_GetGlobals
# define PySys_SetObject py3_PySys_SetObject
# define PySys_GetObject py3_PySys_GetObject
# define PySys_SetArgv py3_PySys_SetArgv
# define PyType_Ready py3_PyType_Ready
# எனில் PY_VERSION_HEX >= 0x03040000
#  define PyType_GetFlags py3_PyType_GetFlags
# endif
# undef Py_Build_மதிப்பு
# define Py_Build_மதிப்பு py3_Py_Build_மதிப்பு
# define Py_SetPythonHome py3_Py_SetPythonHome
# define Py_Initialize py3_Py_Initialize
# define Py_Finalize py3_Py_Finalize
# define Py_IsInitialized py3_Py_IsInitialized
# define _Py_NoneStruct (*py3__Py_NoneStruct)
# define _Py_FalseStruct (*py3__Py_FalseStruct)
# define _Py_TrueStruct (*py3__Py_TrueStruct)
# எனில் !defined(USE_LIMITED_API) && PY_VERSION_HEX < 0x030D0000
// Private symbol that used to be required as part of PyIter_Check.
#  define _PyObject_NextNotImplemented (*py3__PyObject_NextNotImplemented)
# endif
# define PyModule_AddObject py3_PyModule_AddObject
# define PyImport_AppendInittab py3_PyImport_AppendInittab
# define PyImport_AddModule py3_PyImport_AddModule
# ifdef USE_LIMITED_API
#  எனில் Py_LIMITED_API >= 0x030a0000
#   define PyUnicode_AsUTF8AndSize py3_PyUnicode_AsUTF8AndSize
#  endif
# இல்லையெனில்
#  எனில் PY_VERSION_HEX >= 0x030300f0
#   define PyUnicode_AsUTF8AndSize py3_PyUnicode_AsUTF8AndSize
#  இல்லையெனில்
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
# define Py_எண்_Check (*py3_Py_எண்_Check)
# define Py_எண்_Long (*py3_Py_எண்_Long)
# define PyBool_Type (*py3_PyBool_Type)
# define PyErr_NewException py3_PyErr_NewException
# ifdef Py_DEBUG
#  define _Py_NegativeRefcount py3__Py_NegativeRefcount
#  define _Py_RefTotal (*py3__Py_RefTotal)
#  define PyModule_Create2TraceRefs py3_PyModule_Create2TraceRefs
# இல்லையெனில்
#  define PyModule_Create2 py3_PyModule_Create2
# endif
# எனில் defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
#  define _PyObject_DebugMalloc py3__PyObject_DebugMalloc
#  define _PyObject_DebugFree py3__PyObject_DebugFree
# இல்லையெனில்
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
#  இல்லையெனில்
#   define PyUnicodeUCS2_FromFormat py3_PyUnicodeUCS2_FromFormat
#  endif
# இல்லையெனில்
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

# எனில் defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
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
நிலையான முழு (*py3_PySys_SetArgv)(முழு, wchar_t **);
நிலையான வெறுமை (*py3_Py_SetPythonHome)(wchar_t *home);
நிலையான வெறுமை (*py3_Py_Initialize)(வெறுமை);
நிலையான PyObject* (*py3_PyList_New)(Py_ssize_t size);
நிலையான PyGILState_STATE (*py3_PyGILState_Ensure)(வெறுமை);
நிலையான வெறுமை (*py3_PyGILState_Release)(PyGILState_STATE);
நிலையான முழு (*py3_PySys_SetObject)(எழுத்து *, PyObject *);
நிலையான PyObject* (*py3_PySys_GetObject)(எழுத்து *);
நிலையான முழு (*py3_PyList_Append)(PyObject *, PyObject *);
நிலையான முழு (*py3_Py_List_செருகு)(PyObject *, முழு, PyObject *);
நிலையான Py_ssize_t (*py3_PyList_Size)(PyObject *);
நிலையான PyObject* (*py3_PyTuple_New)(Py_ssize_t size);
நிலையான முழு (*py3_PyTuple_SetItem)(PyObject *, Py_ssize_t, PyObject *);
நிலையான Py_ssize_t (*py3_PyTuple_Size)(PyObject *);
நிலையான முழு (*py3_PySequence_Check)(PyObject *);
நிலையான Py_ssize_t (*py3_PySequence_Size)(PyObject *);
நிலையான PyObject* (*py3_PySequence_GetItem)(PyObject *, Py_ssize_t);
நிலையான PyObject* (*py3_PySequence_Fast)(PyObject *, மாறிலி எழுத்து *);
நிலையான PyObject* (*py3_PyTuple_GetItem)(PyObject *, Py_ssize_t);
நிலையான முழு (*py3_PyMapping_Check)(PyObject *);
நிலையான PyObject* (*py3_Py_Mapping_சாவிகள்)(PyObject *);
# எனில் PY_VERSION_HEX >= 0x030601f0
நிலையான முழு (*py3_PySlice_AdjustIndices)(Py_ssize_t length,
		     Py_ssize_t *start, Py_ssize_t *stop, Py_ssize_t step);
நிலையான முழு (*py3_PySlice_Unpack)(PyObject *slice,
		     Py_ssize_t *start, Py_ssize_t *stop, Py_ssize_t *step);
# endif
நிலையான முழு (*py3_PySlice_GetIndicesEx)(PySliceObject_T *r, Py_ssize_t length,
		     Py_ssize_t *start, Py_ssize_t *stop, Py_ssize_t *step,
		     Py_ssize_t *slicelen);
நிலையான PyObject* (*py3_PyErr_NoMemory)(வெறுமை);
நிலையான வெறுமை (*py3_Py_Finalize)(வெறுமை);
நிலையான வெறுமை (*py3_PyErr_SetString)(PyObject *, மாறிலி எழுத்து *);
நிலையான வெறுமை (*py3_PyErr_SetObject)(PyObject *, PyObject *);
நிலையான முழு (*py3_PyErr_ExceptionMatches)(PyObject *);
# எனில் defined(USE_LIMITED_API) || PY_VERSION_HEX >= 0x03080000
நிலையான வெறுமை (*py3_Py_IncRef)(PyObject *);
நிலையான வெறுமை (*py3_Py_DecRef)(PyObject *);
# endif
# ifdef USE_LIMITED_API
நிலையான PyObject* (*py3_Py_CompileString)(மாறிலி எழுத்து *, மாறிலி எழுத்து *, முழு);
நிலையான PyObject* (*py3_PyEval_EvalCode)(PyObject *co, PyObject *globals, PyObject *locals);
# இல்லையெனில்
நிலையான முழு (*py3_PyRun_SimpleString)(எழுத்து *);
நிலையான PyObject* (*py3_PyRun_String)(எழுத்து *, முழு, PyObject *, PyObject *);
# endif
நிலையான PyObject* (*py3_PyObject_GetAttrString)(PyObject *, மாறிலி எழுத்து *);
நிலையான முழு (*py3_PyObject_HasAttrString)(PyObject *, மாறிலி எழுத்து *);
நிலையான முழு (*py3_PyObject_SetAttrString)(PyObject *, மாறிலி எழுத்து *, PyObject *);
நிலையான PyObject* (*py3_PyObject_CallFunctionObjArgs)(PyObject *, ...);
# எனில் PY_VERSION_HEX >= 0x030d0000
நிலையான PyObject* (*py3_PyObject_CallFunction)(PyObject *, எழுத்து *, ...);
# இல்லையெனில்
நிலையான PyObject* (*py3__PyObject_CallFunction_SizeT)(PyObject *, எழுத்து *, ...);
# endif
நிலையான PyObject* (*py3_PyObject_Call)(PyObject *, PyObject *, PyObject *);
நிலையான PyObject* (*py3_PyEval_GetGlobals)(வெறுமை);
நிலையான PyObject* (*py3_PyEval_GetLocals)(வெறுமை);
நிலையான PyObject* (*py3_PyList_GetItem)(PyObject *, Py_ssize_t);
நிலையான PyObject* (*py3_PyImport_ImportModule)(மாறிலி எழுத்து *);
நிலையான PyObject* (*py3_PyImport_AddModule)(மாறிலி எழுத்து *);
நிலையான முழு (*py3_PyErr_BadArgument)(வெறுமை);
நிலையான PyObject* (*py3_PyErr_Occurred)(வெறுமை);
நிலையான PyObject* (*py3_PyModule_GetDict)(PyObject *);
நிலையான முழு (*py3_PyList_SetItem)(PyObject *, Py_ssize_t, PyObject *);
நிலையான PyObject* (*py3_PyDict_GetItemString)(PyObject *, மாறிலி எழுத்து *);
நிலையான முழு (*py3_PyDict_Next)(PyObject *, Py_ssize_t *, PyObject **, PyObject **);
நிலையான PyObject* (*py3_PyLong_FromLong)(long);
நிலையான PyObject* (*py3_PyDict_New)(வெறுமை);
# எனில் (defined(USE_LIMITED_API) && Py_LIMITED_API >= 0x03080000) || \
       (!defined(USE_LIMITED_API) && PY_VERSION_HEX >= 0x03080000)
நிலையான முழு (*py3_PyIter_Check)(PyObject *o);
# endif
நிலையான PyObject* (*py3_PyIter_Next)(PyObject *);
நிலையான PyObject* (*py3_PyObject_GetIter)(PyObject *);
நிலையான PyObject* (*py3_PyObject_Repr)(PyObject *);
நிலையான PyObject* (*py3_PyObject_GetItem)(PyObject *, PyObject *);
நிலையான முழு (*py3_PyObject_IsTrue)(PyObject *);
நிலையான PyObject* (*py3_Py_Build_மதிப்பு)(எழுத்து *, ...);
# எனில் PY_VERSION_HEX >= 0x03040000
நிலையான முழு (*py3_PyType_GetFlags)(PyTypeObject *o);
# endif
நிலையான முழு (*py3_PyType_Ready)(PyTypeObject *type);
நிலையான முழு (*py3_PyDict_SetItemString)(PyObject *dp, எழுத்து *சாவி, PyObject *item);
நிலையான PyObject* (*py3_PyUnicode_FromString)(மாறிலி எழுத்து *u);
# ifdef Py_UNICODE_USE_UCS_FUNCTIONS
#  ifdef Py_UNICODE_WIDE
நிலையான PyObject* (*py3_PyUnicodeUCS4_FromFormat)(மாறிலி எழுத்து *u, ...);
#  இல்லையெனில்
நிலையான PyObject* (*py3_PyUnicodeUCS2_FromFormat)(மாறிலி எழுத்து *u, ...);
#  endif
# இல்லையெனில்
நிலையான PyObject* (*py3_PyUnicode_FromFormat)(மாறிலி எழுத்து *u, ...);
# endif
நிலையான PyObject* (*py3_PyUnicode_Decode)(மாறிலி எழுத்து *u, Py_ssize_t size,
	மாறிலி எழுத்து *encoding, மாறிலி எழுத்து *errors);
நிலையான long (*py3_PyLong_AsLong)(PyObject *);
நிலையான வெறுமை (*py3_PyErr_SetNone)(PyObject *);
நிலையான வெறுமை (*py3_PyEval_InitThreads)(வெறுமை);
நிலையான வெறுமை(*py3_PyEval_RestoreThread)(PyThreadState *);
நிலையான PyThreadState*(*py3_PyEval_SaveThread)(வெறுமை);
நிலையான முழு (*py3_PyArg_Parse)(PyObject *, எழுத்து *, ...);
நிலையான முழு (*py3_PyArg_ParseTuple)(PyObject *, எழுத்து *, ...);
நிலையான வெறுமை (*py3_PyMem_Free)(வெறுமை *);
நிலையான வெறுமை* (*py3_PyMem_Malloc)(size_t);
நிலையான முழு (*py3_Py_IsInitialized)(வெறுமை);
நிலையான வெறுமை (*py3_PyErr_Clear)(வெறுமை);
நிலையான PyObject* (*py3_PyErr_Format)(PyObject *, மாறிலி எழுத்து *, ...);
நிலையான வெறுமை (*py3_PyErr_PrintEx)(முழு);
நிலையான PyObject*(*py3__PyObject_Init)(PyObject *, PyTypeObject *);
# எனில் !defined(USE_LIMITED_API) && PY_VERSION_HEX < 0x030D0000
நிலையான iternextfunc py3__PyObject_NextNotImplemented;
# endif
நிலையான PyObject* py3__Py_NoneStruct;
நிலையான PyObject* py3__Py_FalseStruct;
நிலையான PyObject* py3__Py_TrueStruct;
நிலையான முழு (*py3_PyModule_AddObject)(PyObject *m, மாறிலி எழுத்து *name, PyObject *o);
நிலையான முழு (*py3_PyImport_AppendInittab)(மாறிலி எழுத்து *name, PyObject* (*initfunc)(வெறுமை));
# ifdef USE_LIMITED_API
#  எனில் Py_LIMITED_API >= 0x030a0000
நிலையான எழுத்து* (*py3_PyUnicode_AsUTF8AndSize)(PyObject *unicode, Py_ssize_t *size);
#  endif
# இல்லையெனில்
#  எனில் PY_VERSION_HEX >= 0x030300f0
நிலையான எழுத்து* (*py3_PyUnicode_AsUTF8AndSize)(PyObject *unicode, Py_ssize_t *size);
#  இல்லையெனில்
நிலையான எழுத்து* (*py3__PyUnicode_AsString)(PyObject *unicode);
#  endif
# endif
நிலையான முழு (*py3_PyUnicode_CompareWithASCIIString)(PyObject *unicode, மாறிலி எழுத்து* string);
நிலையான PyObject* (*py3_PyUnicode_AsEncodedString)(PyObject *unicode, மாறிலி எழுத்து* encoding, மாறிலி எழுத்து* errors);
நிலையான PyObject* (*py3_PyUnicode_AsUTF8String)(PyObject *unicode);
நிலையான எழுத்து* (*py3_PyBytes_AsString)(PyObject *bytes);
நிலையான முழு (*py3_PyBytes_AsStringAndSize)(PyObject *bytes, எழுத்து **buffer, Py_ssize_t *length);
நிலையான PyObject* (*py3_PyBytes_FromString)(எழுத்து *str);
நிலையான PyObject* (*py3_PyBytes_FromStringAndSize)(எழுத்து *str, Py_ssize_t length);
# எனில் PY_VERSION_HEX >= 0x030900b0
நிலையான PyObject* (*py3__PyObject_New)(PyTypeObject *);
# endif
நிலையான PyObject* (*py3_PyFloat_FromDouble)(இரட்டை இலக்கம்);
நிலையான இரட்டை (*py3_PyFloat_AsDouble)(PyObject *);
நிலையான PyObject* (*py3_PyObject_GenericGetAttr)(PyObject *obj, PyObject *name);
நிலையான PyObject* (*py3_PyType_GenericAlloc)(PyTypeObject *type, Py_ssize_t nitems);
நிலையான PyObject* (*py3_PyType_GenericNew)(PyTypeObject *type, PyObject *args, PyObject *kwds);
நிலையான PyTypeObject* py3_PyType_Type;
# ifndef USE_LIMITED_API
நிலையான PyTypeObject* py3_PyStdPrinter_Type;
# endif
நிலையான PyTypeObject* py3_PySlice_Type;
நிலையான PyTypeObject* py3_PyFloat_Type;
நிலையான PyTypeObject* py3_PyBool_Type;
நிலையான முழு (*py3_Py_எண்_Check)(PyObject *);
நிலையான PyObject* (*py3_Py_எண்_Long)(PyObject *);
நிலையான PyObject* (*py3_PyErr_NewException)(எழுத்து *name, PyObject *base, PyObject *dict);
நிலையான PyObject* (*py3_PyCapsule_New)(வெறுமை *, எழுத்து *, PyCapsule_Destructor);
நிலையான வெறுமை* (*py3_PyCapsule_GetPointer)(PyObject *, எழுத்து *);
# ifdef Py_DEBUG
நிலையான வெறுமை (*py3__Py_NegativeRefcount)(மாறிலி எழுத்து *fname, முழு lineno, PyObject *op);
நிலையான Py_ssize_t* py3__Py_RefTotal;
நிலையான PyObject* (*py3_PyModule_Create2TraceRefs)(கட்டமைப்பு PyModuleDef* module, முழு module_api_version);
# இல்லையெனில்
நிலையான PyObject* (*py3_PyModule_Create2)(கட்டமைப்பு PyModuleDef* module, முழு module_api_version);
# endif
# எனில் defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
நிலையான வெறுமை (*py3__PyObject_DebugFree)(வெறுமை*);
நிலையான வெறுமை* (*py3__PyObject_DebugMalloc)(size_t);
# இல்லையெனில்
நிலையான வெறுமை (*py3_PyObject_Free)(வெறுமை*);
நிலையான வெறுமை* (*py3_PyObject_Malloc)(size_t);
# endif
நிலையான PyObject*(*py3__PyObject_GC_New)(PyTypeObject *);
நிலையான வெறுமை(*py3_PyObject_GC_Del)(வெறுமை *);
நிலையான வெறுமை(*py3_PyObject_GC_UnTrack)(வெறுமை *);
நிலையான முழு (*py3_PyType_IsSubtype)(PyTypeObject *, PyTypeObject *);
# ifdef USE_LIMITED_API
நிலையான வெறுமை* (*py3_PyType_GetSlot)(PyTypeObject *, முழு);
நிலையான PyObject* (*py3_PyType_FromSpec)(PyType_Spec *);
# endif

// Imported exception objects
நிலையான PyObject *p3imp_PyExc_AttributeError;
நிலையான PyObject *p3imp_PyExc_IndexError;
நிலையான PyObject *p3imp_Py_Exc_சாவி_Error;
நிலையான PyObject *p3imp_PyExc_KeyboardInterrupt;
நிலையான PyObject *p3imp_PyExc_TypeError;
நிலையான PyObject *p3imp_Py_Exc_மதிப்பு_Error;
நிலையான PyObject *p3imp_PyExc_SystemExit;
நிலையான PyObject *p3imp_PyExc_RuntimeError;
நிலையான PyObject *p3imp_PyExc_ImportError;
நிலையான PyObject *p3imp_PyExc_OverflowError;

# define PyExc_AttributeError p3imp_PyExc_AttributeError
# define PyExc_IndexError p3imp_PyExc_IndexError
# define Py_Exc_சாவி_Error p3imp_Py_Exc_சாவி_Error
# define PyExc_KeyboardInterrupt p3imp_PyExc_KeyboardInterrupt
# define PyExc_TypeError p3imp_PyExc_TypeError
# define Py_Exc_மதிப்பு_Error p3imp_Py_Exc_மதிப்பு_Error
# define PyExc_SystemExit p3imp_PyExc_SystemExit
# define PyExc_RuntimeError p3imp_PyExc_RuntimeError
# define PyExc_ImportError p3imp_PyExc_ImportError
# define PyExc_OverflowError p3imp_PyExc_OverflowError

/*
 * Table of name to function pointer of python.
 */
# define PYTHON_PROC FARPROC
நிலையான கட்டமைப்பு
{
    எழுத்து *name;
    PYTHON_PROC *ptr;
} py3_funcname_table[] =
{
    {"PySys_SetArgv", (PYTHON_PROC*)&py3_PySys_SetArgv},
    {"Py_SetPythonHome", (PYTHON_PROC*)&py3_Py_SetPythonHome},
    {"Py_Initialize", (PYTHON_PROC*)&py3_Py_Initialize},
    {"_PyArg_ParseTuple_SizeT", (PYTHON_PROC*)&py3_PyArg_ParseTuple},
    {"_Py_BuildValue_SizeT", (PYTHON_PROC*)&py3_Py_Build_மதிப்பு},
    {"PyMem_Free", (PYTHON_PROC*)&py3_PyMem_Free},
    {"PyMem_Malloc", (PYTHON_PROC*)&py3_PyMem_Malloc},
    {"PyList_New", (PYTHON_PROC*)&py3_PyList_New},
    {"PyGILState_Ensure", (PYTHON_PROC*)&py3_PyGILState_Ensure},
    {"PyGILState_Release", (PYTHON_PROC*)&py3_PyGILState_Release},
    {"PySys_SetObject", (PYTHON_PROC*)&py3_PySys_SetObject},
    {"PySys_GetObject", (PYTHON_PROC*)&py3_PySys_GetObject},
    {"PyList_Append", (PYTHON_PROC*)&py3_PyList_Append},
    {"PyList_Insert", (PYTHON_PROC*)&py3_Py_List_செருகு},
    {"PyList_Size", (PYTHON_PROC*)&py3_PyList_Size},
    {"PyTuple_New", (PYTHON_PROC*)&py3_PyTuple_New},
    {"PyTuple_GetItem", (PYTHON_PROC*)&py3_PyTuple_GetItem},
    {"PyTuple_SetItem", (PYTHON_PROC*)&py3_PyTuple_SetItem},
    {"PyTuple_Size", (PYTHON_PROC*)&py3_PyTuple_Size},
    {"PySequence_Check", (PYTHON_PROC*)&py3_PySequence_Check},
    {"PySequence_Size", (PYTHON_PROC*)&py3_PySequence_Size},
    {"PySequence_GetItem", (PYTHON_PROC*)&py3_PySequence_GetItem},
    {"PySequence_Fast", (PYTHON_PROC*)&py3_PySequence_Fast},
# எனில் PY_VERSION_HEX >= 0x030601f0
    {"PySlice_AdjustIndices", (PYTHON_PROC*)&py3_PySlice_AdjustIndices},
    {"PySlice_Unpack", (PYTHON_PROC*)&py3_PySlice_Unpack},
# endif
    {"PySlice_GetIndicesEx", (PYTHON_PROC*)&py3_PySlice_GetIndicesEx},
    {"PyErr_NoMemory", (PYTHON_PROC*)&py3_PyErr_NoMemory},
    {"Py_Finalize", (PYTHON_PROC*)&py3_Py_Finalize},
    {"PyErr_SetString", (PYTHON_PROC*)&py3_PyErr_SetString},
    {"PyErr_SetObject", (PYTHON_PROC*)&py3_PyErr_SetObject},
    {"PyErr_ExceptionMatches", (PYTHON_PROC*)&py3_PyErr_ExceptionMatches},
# எனில் defined(USE_LIMITED_API) || PY_VERSION_HEX >= 0x03080000
    {"Py_IncRef", (PYTHON_PROC*)&py3_Py_IncRef},
    {"Py_DecRef", (PYTHON_PROC*)&py3_Py_DecRef},
# endif
# ifdef USE_LIMITED_API
    {"Py_CompileString", (PYTHON_PROC*)&py3_Py_CompileString},
    {"PyEval_EvalCode", (PYTHON_PROC*)&PyEval_EvalCode},
# இல்லையெனில்
    {"PyRun_SimpleString", (PYTHON_PROC*)&py3_PyRun_SimpleString},
    {"PyRun_String", (PYTHON_PROC*)&py3_PyRun_String},
# endif
    {"PyObject_GetAttrString", (PYTHON_PROC*)&py3_PyObject_GetAttrString},
    {"PyObject_HasAttrString", (PYTHON_PROC*)&py3_PyObject_HasAttrString},
    {"PyObject_SetAttrString", (PYTHON_PROC*)&py3_PyObject_SetAttrString},
    {"PyObject_CallFunctionObjArgs", (PYTHON_PROC*)&py3_PyObject_CallFunctionObjArgs},
# எனில் PY_VERSION_HEX >= 0x030d0000
    {"PyObject_CallFunction", (PYTHON_PROC*)&py3_PyObject_CallFunction},
# இல்லையெனில்
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
    {"PyMapping_Keys", (PYTHON_PROC*)&py3_Py_Mapping_சாவிகள்},
# எனில் (defined(USE_LIMITED_API) && Py_LIMITED_API >= 0x03080000) || \
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
# எனில் PY_VERSION_HEX >= 0x03040000
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
# எனில் !defined(USE_LIMITED_API) && PY_VERSION_HEX < 0x030D0000
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
#  எனில் Py_LIMITED_API >= 0x030a0000
    {"PyUnicode_AsUTF8AndSize", (PYTHON_PROC*)&py3_PyUnicode_AsUTF8AndSize},
#  endif
# இல்லையெனில்
#  எனில் PY_VERSION_HEX >= 0x030300f0
    {"PyUnicode_AsUTF8AndSize", (PYTHON_PROC*)&py3_PyUnicode_AsUTF8AndSize},
#  இல்லையெனில்
    {"_PyUnicode_AsString", (PYTHON_PROC*)&py3__PyUnicode_AsString},
#  endif
# endif
    {"PyUnicode_CompareWithASCIIString", (PYTHON_PROC*)&py3_PyUnicode_CompareWithASCIIString},
    {"PyUnicode_AsUTF8String", (PYTHON_PROC*)&py3_PyUnicode_AsUTF8String},
# ifdef Py_UNICODE_USE_UCS_FUNCTIONS
#  ifdef Py_UNICODE_WIDE
    {"PyUnicodeUCS4_FromFormat", (PYTHON_PROC*)&py3_PyUnicodeUCS4_FromFormat},
#  இல்லையெனில்
    {"PyUnicodeUCS2_FromFormat", (PYTHON_PROC*)&py3_PyUnicodeUCS2_FromFormat},
#  endif
# இல்லையெனில்
    {"PyUnicode_FromFormat", (PYTHON_PROC*)&py3_PyUnicode_FromFormat},
# endif
    {"PyBytes_AsString", (PYTHON_PROC*)&py3_PyBytes_AsString},
    {"PyBytes_AsStringAndSize", (PYTHON_PROC*)&py3_PyBytes_AsStringAndSize},
    {"PyBytes_FromString", (PYTHON_PROC*)&py3_PyBytes_FromString},
    {"PyBytes_FromStringAndSize", (PYTHON_PROC*)&py3_PyBytes_FromStringAndSize},
# எனில் PY_VERSION_HEX >= 0x030900b0
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
    {"PyNumber_Check", (PYTHON_PROC*)&py3_Py_எண்_Check},
    {"PyNumber_Long", (PYTHON_PROC*)&py3_Py_எண்_Long},
    {"PyErr_NewException", (PYTHON_PROC*)&py3_PyErr_NewException},
# ifdef Py_DEBUG
    {"_Py_NegativeRefcount", (PYTHON_PROC*)&py3__Py_NegativeRefcount},
    {"_Py_RefTotal", (PYTHON_PROC*)&py3__Py_RefTotal},
    {"PyModule_Create2TraceRefs", (PYTHON_PROC*)&py3_PyModule_Create2TraceRefs},
# இல்லையெனில்
    {"PyModule_Create2", (PYTHON_PROC*)&py3_PyModule_Create2},
# endif
# எனில் defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
    {"_PyObject_DebugFree", (PYTHON_PROC*)&py3__PyObject_DebugFree},
    {"_PyObject_DebugMalloc", (PYTHON_PROC*)&py3__PyObject_DebugMalloc},
# இல்லையெனில்
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
#  எனில் PY_VERSION_HEX >= 0x03040000
    {"PyType_GetSlot", (PYTHON_PROC*)&py3_PyType_GetSlot},
#  endif
    {"PyType_FromSpec", (PYTHON_PROC*)&py3_PyType_FromSpec},
# endif
    {"", வெற்று},
};

# எனில் defined(USE_LIMITED_API) || PY_VERSION_HEX >= 0x03080000
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

# எனில் PY_VERSION_HEX >= 0x030900b0
    நிலையான உள்வரி முழு
py3_PyType_HasFeature(PyTypeObject *type, unsigned long feature)
{
    திரும்பு ((PyType_GetFlags(type) & feature) != 0);
}
#  define PyType_HasFeature(t,f) py3_PyType_HasFeature(t,f)
# endif

# எனில் PY_VERSION_HEX >= 0x030a00b2
    நிலையான உள்வரி முழு
py3__PyObject_TypeCheck(PyObject *ob, PyTypeObject *type)
{
    திரும்பு Py_IS_TYPE(ob, type) || PyType_IsSubtype(Py_TYPE(ob), type);
}
#  எனில் PY_VERSION_HEX >= 0x030b00b3
#   undef PyObject_TypeCheck
#   define PyObject_TypeCheck(o,t) py3__PyObject_TypeCheck(o,t)
#  இல்லையெனில்
#   define _PyObject_TypeCheck(o,t) py3__PyObject_TypeCheck(o,t)
#  endif
# endif

# எனில் !defined(USE_LIMITED_API) && PY_VERSION_HEX >= 0x030c00b0
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
    நிலையான WCHAR *
py3_get_system_libname(மாறிலி எழுத்து *libname)
{
    மாறிலி WCHAR	*pythoncore = L"Software\\Python\\PythonCore";
    மாறிலி எழுத்து	*cp = libname;
    WCHAR	subkey[128];
    HKEY	h_சாவி;
    முழு		i;
    DWORD	j, len;
    LSTATUS	ret;

    போது (*cp != '\0')
    {
	எனில் (*cp == ':' || *cp == '\\' || *cp == '/')
	{
	    // Bail out if "libname" contains path separator, assume it is
	    // an absolute path.
	    திரும்பு வெற்று;
	}
	++cp;
    }

    WCHAR   keyfound[32];
    HKEY    h_சாவி_உச்சி[] = {HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE};
    HKEY    h_சாவி_Found = வெற்று;
#  ifdef USE_LIMITED_API
    long    maxminor = -1;
#  endif
    க்கு (i = 0; i < ARRAY_LENGTH(h_சாவி_உச்சி); i++)
    {
	long	major, minor;

	ret = Reg_Open_சாவி_Ex_W(h_சாவி_உச்சி[i], pythoncore, 0, சாவி_READ, &h_சாவி);
	எனில் (ret != ERROR_SUCCESS)
	    தொடர்;
	க்கு (j = 0;; j++)
	{
	    WCHAR   keyname[32];
	    WCHAR   *wp;

	    len = ARRAY_LENGTH(keyname);
	    ret = Reg_Enum_சாவி_Ex_W(h_சாவி, j, keyname, &len,
						    வெற்று, வெற்று, வெற்று, வெற்று);
	    எனில் (ret == ERROR_NO_MORE_பொருட்கள்)
		நிறுத்து;

	    major = wcstol(keyname, &wp, 10);
	    எனில் (*wp != L'.')
		தொடர்;
	    minor = wcstol(wp + 1, &wp, 10);
#  ifdef _WIN64
	    எனில் (*wp != L'\0')
		தொடர்;
#  இல்லையெனில்
	    எனில் (wcscmp(wp, L"-32") != 0)
		தொடர்;
#  endif

	    எனில் (major != PY_MAJOR_VERSION)
		தொடர்;
#  ifdef USE_LIMITED_API
	    // Search the latest version.
	    எனில் ((minor > maxminor)
		    && (minor >= ((Py_LIMITED_API >> 16) & 0xff)))
	    {
		maxminor = minor;
		wcscpy(keyfound, keyname);
		h_சாவி_Found = h_சாவி_உச்சி[i];
	    }
#  இல்லையெனில்
	    // Check if it matches with the compiled version.
	    எனில் (minor == PY_MINOR_VERSION)
	    {
		wcscpy(keyfound, keyname);
		h_சாவி_Found = h_சாவி_உச்சி[i];
		நிறுத்து;
	    }
#  endif
	}
	Reg_Close_சாவி(h_சாவி);
#  ifdef USE_LIMITED_API
	எனில் (h_சாவி_Found != வெற்று)
	    நிறுத்து;
#  endif
    }
    எனில் (h_சாவி_Found == வெற்று)
	திரும்பு வெற்று;

    swprintf(subkey, ARRAY_LENGTH(subkey), L"%ls\\%ls\\InstallPath",
							pythoncore, keyfound);
    ret = Reg_Get_மதிப்பு_W(h_சாவி_Found, subkey, வெற்று, RRF_RT_REG_SZ,
							    வெற்று, வெற்று, &len);
    எனில் (ret != ERROR_MORE_DATA && ret != ERROR_SUCCESS)
	திரும்பு வெற்று;
    size_t len2 = len / அளவு(WCHAR) + 1 + strlen(libname);
    WCHAR *path = alloc(len2 * அளவு(WCHAR));
    எனில் (path == வெற்று)
	திரும்பு வெற்று;
    ret = Reg_Get_மதிப்பு_W(h_சாவி_Found, subkey, வெற்று, RRF_RT_REG_SZ,
							    வெற்று, path, &len);
    எனில் (ret != ERROR_SUCCESS)
    {
	vim_free(path);
	திரும்பு வெற்று;
    }
    // Remove trailing path separators.
    size_t len3 = wcslen(path);
    எனில் ((len3 > 0) && (path[len3 - 1] == L'/' || path[len3 - 1] == L'\\'))
	--len3;
    swprintf(path + len3, len2 - len3, L"\\%hs", libname);
    திரும்பு path;
}
# endif

/*
 * Load library and get all pointers.
 * Parameter 'libname' provides name of DLL.
 * Return OK or FAIL.
 */
    நிலையான முழு
py3_runtime_link_init(எழுத்து *libname, முழு verbose)
{
    முழு i;
    PYTHON_PROC *ucs_from_string = (PYTHON_PROC *)&py3_PyUnicode_FromString;
    PYTHON_PROC *ucs_decode = (PYTHON_PROC *)&py3_PyUnicode_Decode;
    PYTHON_PROC *ucs_as_encoded_string =
				 (PYTHON_PROC *)&py3_PyUnicode_AsEncodedString;

# எனில் !(defined(PY_NO_RTLD_GLOBAL) && defined(PY3_NO_RTLD_GLOBAL)) && defined(UNIX) && defined(FEAT_PYTHON)
    // Can't have Python and Python3 loaded at the same time.
    // It causes a crash, because RTLD_GLOBAL is needed for
    // standard C extension libraries of one or both python versions.
    எனில் (python_loaded())
    {
	எனில் (verbose)
	    emsg(_(e_this_vim_cannot_execute_py3_after_using_python));
	திரும்பு FAIL;
    }
# endif

    எனில் (hinstPy3 != 0)
	திரும்பு OK;
    hinstPy3 = load_dll(libname);

# ifdef MSWIN
    எனில் (!hinstPy3)
    {
	// Attempt to use the path from InstallPath as stored in the registry.
	WCHAR *syslibname = py3_get_system_libname(libname);

	எனில் (syslibname != வெற்று)
	{
	    hinstPy3 = LoadLibraryExW(syslibname, வெற்று,
		    LOAD_LIBRARY_தேடல்_DLL_LOAD_DIR |
		    LOAD_LIBRARY_தேடல்_SYSTEM32);
	    vim_free(syslibname);
	}
    }
# endif

    எனில் (!hinstPy3)
    {
	எனில் (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	திரும்பு FAIL;
    }

    க்கு (i = 0; py3_funcname_table[i].ptr; ++i)
    {
	எனில் ((*py3_funcname_table[i].ptr = குறியீடு_from_dll(hinstPy3,
			py3_funcname_table[i].name)) == வெற்று)
	{
	    close_dll(hinstPy3);
	    hinstPy3 = 0;
	    எனில் (verbose)
		semsg(_(e_could_not_load_library_function_str), py3_funcname_table[i].name);
	    திரும்பு FAIL;
	}
    }

    // Load unicode functions separately as only the ucs2 or the ucs4 functions
    // will be present in the library.
# எனில் PY_VERSION_HEX >= 0x030300f0
    *ucs_from_string = குறியீடு_from_dll(hinstPy3, "PyUnicode_FromString");
    *ucs_decode = குறியீடு_from_dll(hinstPy3, "PyUnicode_Decode");
    *ucs_as_encoded_string = குறியீடு_from_dll(hinstPy3,
	    "PyUnicode_AsEncodedString");
# இல்லையெனில்
    *ucs_from_string = குறியீடு_from_dll(hinstPy3, "PyUnicodeUCS2_FromString");
    *ucs_decode = குறியீடு_from_dll(hinstPy3,
	    "PyUnicodeUCS2_Decode");
    *ucs_as_encoded_string = குறியீடு_from_dll(hinstPy3,
	    "PyUnicodeUCS2_AsEncodedString");
    எனில் (*ucs_from_string == வெற்று || *ucs_decode == வெற்று
					     || *ucs_as_encoded_string == வெற்று)
    {
	*ucs_from_string = குறியீடு_from_dll(hinstPy3,
		"PyUnicodeUCS4_FromString");
	*ucs_decode = குறியீடு_from_dll(hinstPy3,
		"PyUnicodeUCS4_Decode");
	*ucs_as_encoded_string = குறியீடு_from_dll(hinstPy3,
		"PyUnicodeUCS4_AsEncodedString");
    }
# endif
    எனில் (*ucs_from_string == வெற்று || *ucs_decode == வெற்று
					     || *ucs_as_encoded_string == வெற்று)
    {
	close_dll(hinstPy3);
	hinstPy3 = 0;
	எனில் (verbose)
	    semsg(_(e_could_not_load_library_function_str), "PyUnicode_UCSX_*");
	திரும்பு FAIL;
    }

    திரும்பு OK;
}

/*
 * If python is enabled (there is installed python on Windows system) return
 * TRUE, else FALSE.
 */
    முழு
python3_enabled(முழு verbose)
{
    திரும்பு py3_runtime_link_init((எழுத்து *)p_py3dll, verbose) == OK;
}

/*
 * Load the standard Python exceptions - don't import the symbols from the
 * DLL, as this can cause errors (importing data symbols is not reliable).
 */
    நிலையான வெறுமை
get_py3_exceptions(வெறுமை)
{
    PyObject *exmod = PyImport_ImportModule("builtins");
    PyObject *exdict = PyModule_GetDict(exmod);
    p3imp_PyExc_AttributeError = PyDict_GetItemString(exdict, "AttributeError");
    p3imp_PyExc_IndexError = PyDict_GetItemString(exdict, "IndexError");
    p3imp_Py_Exc_சாவி_Error = PyDict_GetItemString(exdict, "KeyError");
    p3imp_PyExc_KeyboardInterrupt = PyDict_GetItemString(exdict, "KeyboardInterrupt");
    p3imp_PyExc_TypeError = PyDict_GetItemString(exdict, "TypeError");
    p3imp_Py_Exc_மதிப்பு_Error = PyDict_GetItemString(exdict, "ValueError");
    p3imp_PyExc_SystemExit = PyDict_GetItemString(exdict, "SystemExit");
    p3imp_PyExc_RuntimeError = PyDict_GetItemString(exdict, "RuntimeError");
    p3imp_PyExc_ImportError = PyDict_GetItemString(exdict, "ImportError");
    p3imp_PyExc_OverflowError = PyDict_GetItemString(exdict, "OverflowError");
    Py_XINCREF(p3imp_PyExc_AttributeError);
    Py_XINCREF(p3imp_PyExc_IndexError);
    Py_XINCREF(p3imp_Py_Exc_சாவி_Error);
    Py_XINCREF(p3imp_PyExc_KeyboardInterrupt);
    Py_XINCREF(p3imp_PyExc_TypeError);
    Py_XINCREF(p3imp_Py_Exc_மதிப்பு_Error);
    Py_XINCREF(p3imp_PyExc_SystemExit);
    Py_XINCREF(p3imp_PyExc_RuntimeError);
    Py_XINCREF(p3imp_PyExc_ImportError);
    Py_XINCREF(p3imp_PyExc_OverflowError);
    Py_XDECREF(exmod);
}
#endif // DYNAMIC_PYTHON3

நிலையான முழு py3initialised = 0;
#define PYINITIALISED py3initialised
நிலையான முழு python_end_called = FALSE;

#ifdef USE_LIMITED_API
# define DESTRUCTOR_FINISH(self) \
    ((freefunc)PyType_GetSlot(Py_TYPE((PyObject*)self), Py_tp_free))((PyObject*)self)
#இல்லையெனில்
# define DESTRUCTOR_FINISH(self) Py_TYPE(self)->tp_free((PyObject*)self)
#endif

#define WIN_PYTHON_REF(win) win->w_python3_ref
#define BUF_PYTHON_REF(buf) buf->b_python3_ref
#define TAB_PYTHON_REF(tab) tab->tp_python3_ref

    நிலையான வெறுமை
call_PyObject_Free(வெறுமை *p)
{
#எனில் defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
    _PyObject_DebugFree(p);
#இல்லையெனில்
    PyObject_Free(p);
#endif
}

    நிலையான PyObject *
call_PyType_GenericNew(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    திரும்பு PyType_GenericNew(type,args,kwds);
}

    நிலையான PyObject *
call_PyType_GenericAlloc(PyTypeObject *type, Py_ssize_t nitems)
{
    திரும்பு PyType_GenericAlloc(type,nitems);
}

நிலையான PyObject *OutputGetattro(PyObject *, PyObject *);
நிலையான முழு OutputSetattro(PyObject *, PyObject *, PyObject *);
நிலையான PyObject *BufferGetattro(PyObject *, PyObject *);
நிலையான முழு BufferSetattro(PyObject *, PyObject *, PyObject *);
நிலையான PyObject *TabPageGetattro(PyObject *, PyObject *);
நிலையான PyObject *WindowGetattro(PyObject *, PyObject *);
நிலையான முழு WindowSetattro(PyObject *, PyObject *, PyObject *);
நிலையான PyObject *RangeGetattro(PyObject *, PyObject *);
நிலையான PyObject *CurrentGetattro(PyObject *, PyObject *);
நிலையான முழு CurrentSetattro(PyObject *, PyObject *, PyObject *);
நிலையான PyObject *DictionaryGetattro(PyObject *, PyObject *);
நிலையான முழு DictionarySetattro(PyObject *, PyObject *, PyObject *);
நிலையான PyObject *ListGetattro(PyObject *, PyObject *);
நிலையான முழு ListSetattro(PyObject *, PyObject *, PyObject *);
நிலையான PyObject *TupleGetattro(PyObject *, PyObject *);
நிலையான முழு TupleSetattro(PyObject *, PyObject *, PyObject *);
நிலையான PyObject *FunctionGetattro(PyObject *, PyObject *);

நிலையான கட்டமைப்பு PyModuleDef vimmodule;

#define PY_CAN_RECURSE

/*
 * Include the code shared with if_python.c
 */
#அந்தர்பாவ "if_py_both.h"

#ifdef USE_LIMITED_API
# எனில் Py_LIMITED_API >= 0x030A0000
#  define PY_UNICODE_GET_UTF8_CHARS(obj) PyUnicode_AsUTF8AndSize(obj, வெற்று)
# இல்லையெனில்
// Python limited API before 3.10 lack easy ways to query the raw UTF-8 chars.
// We need to first convert the string to bytes, and then extract the chars.
// This function is only used for attribute string comparisons, which have
// known short length. As such, just allocate a short static buffer to hold
// the characters instead of having to allocate/deallcoate it.
//
// An alternative would be to convert all attribute string comparisons to use
// PyUnicode_CompareWithASCIIString to skip having to extract the chars.
நிலையான எழுத்து py3_unicode_utf8_chars[20];
நிலையான எழுத்து* PY_UNICODE_GET_UTF8_CHARS(PyObject* str)
{
    py3_unicode_utf8_chars[0] = '\0';
    PyObject* bytes = PyUnicode_AsUTF8String(str);
    எனில் (bytes)
    {
	எழுத்து *chars;
	Py_ssize_t len;
	எனில் (PyBytes_AsStringAndSize(bytes, &chars, &len) != -1)
	{
	    எனில் (len < (Py_ssize_t)அளவு(py3_unicode_utf8_chars))
		// PyBytes_AsStringAndSize guarantees null-termination
		memcpy(py3_unicode_utf8_chars, chars, len + 1);
	}
	Py_DECREF(bytes);
    }
    திரும்பு py3_unicode_utf8_chars;
}
# endif
#இல்லையெனில்	// !USE_LIMITED_API
# எனில் PY_VERSION_HEX >= 0x030300f0
#  define PY_UNICODE_GET_UTF8_CHARS(obj) PyUnicode_AsUTF8AndSize(obj, வெற்று)
# இல்லையெனில்
#  define PY_UNICODE_GET_UTF8_CHARS _PyUnicode_AsString
# endif
#endif

// NOTE: Must always be used at the start of a block, since it declares "name".
#define GET_ATTR_STRING(name, nameobj) \
    எழுத்து	*name = ""; \
    எனில் (PyUnicode_Check(nameobj)) \
	name = (எழுத்து *)PY_UNICODE_GET_UTF8_CHARS(nameobj)

#define PY3OBJ_DELETED(obj) (obj->ob_base.ob_refcnt<=0)

///////////////////////////////////////////////////////
// Internal function prototypes.

நிலையான PyObject *Py3Init_vim(வெறுமை);

///////////////////////////////////////////////////////
// 1. Python interpreter main program.

    வெறுமை
python3_end(வெறுமை)
{
    நிலையான முழு recurse = 0;

    // If a crash occurs while doing this, don't try again.
    எனில் (recurse != 0)
	திரும்பு;

    python_end_called = TRUE;
    ++recurse;

#ifdef DYNAMIC_PYTHON3
    எனில் (hinstPy3)
#endif
    எனில் (Py_IsInitialized())
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

#எனில் defined(DYNAMIC_PYTHON3) && defined(DYNAMIC_PYTHON) && defined(FEAT_PYTHON) && defined(UNIX)
    முழு
python3_loaded(வெறுமை)
{
    திரும்பு (hinstPy3 != 0);
}
#endif

நிலையான wchar_t *py_home_buf = வெற்று;

#எனில் defined(MSWIN) && (PY_VERSION_HEX >= 0x030500f0)
/*
 * Return TRUE if stdin is readable from Python 3.
 */
    நிலையான BOOL
is_stdin_readable(வெறுமை)
{
    DWORD	    mode, eventnum;
    கட்டமைப்பு _stat    st;
    முழு		    fd = fileno(stdin);
    HANDLE	    hstdin = (HANDLE)_get_osfhandle(fd);

    // Check if stdin is connected to the console.
    எனில் (GetConsoleMode(hstdin, &mode))
	// Check if it is opened as input.
	திரும்பு Get_எண்_Of_Console_உள்ளீடு_Events(hstdin, &eventnum);

    திரும்பு _fstat(fd, &st) == 0;
}

// Python 3.5 or later will abort inside Py_Initialize() when stdin has
// been closed (i.e. executed by "vim -").  Reconnect stdin to CONIN$.
// Note that the python DLL is linked to its own stdio DLL which can be
// differ from Vim's stdio.
    நிலையான வெறுமை
reset_stdin(வெறுமை)
{
    FILE *(*py__acrt_iob_func)(unsigned) = வெற்று;
    FILE *(*pyfreopen)(மாறிலி எழுத்து *, மாறிலி எழுத்து *, FILE *) = வெற்று;
    HINSTANCE hinst = get_forwarded_dll(hinstPy3);

    எனில் (hinst == வெற்று || is_stdin_readable())
	திரும்பு;

    // Get "freopen" and "stdin" which are used in the python DLL.
    // "stdin" is defined as "__acrt_iob_func(0)" in VC++ 2015 or later.
    py__acrt_iob_func = get_dll_import_func(hinst, "__acrt_iob_func");
    எனில் (py__acrt_iob_func)
    {
	HINSTANCE hpystdiodll = find_imported_module_by_funcname(hinst,
							    "__acrt_iob_func");
	எனில் (hpystdiodll)
	    pyfreopen = (வெறுமை *)GetProcAddress(hpystdiodll, "freopen");
    }

    // Reconnect stdin to CONIN$.
    எனில் (pyfreopen != வெற்று)
	pyfreopen("CONIN$", "r", py__acrt_iob_func(0));
    இல்லையெனில்
	freopen("CONIN$", "r", stdin);
}
#இல்லையெனில்
# define reset_stdin()
#endif

// Python 3.2 or later will abort inside Py_Initialize() when mandatory
// modules cannot be loaded (e.g. 'pythonthreehome' is wrongly set.).
// Install a hook to python dll's exit() and recover from it.
#எனில் defined(MSWIN) && (PY_VERSION_HEX >= 0x030200f0)
# define HOOK_EXIT
# include <setjmp.h>

நிலையான jmp_buf exit_hook_jump_buf;
நிலையான வெறுமை *orig_exit = வெற்று;

/*
 * Function that replaces exit() while calling Py_Initialize().
 */
    நிலையான வெறுமை
hooked_exit(முழு ret)
{
    // Recover from exit.
    longjmp(exit_hook_jump_buf, 1);
}

/*
 * Install a hook to python dll's exit().
 */
    நிலையான வெறுமை
hook_py_exit(வெறுமை)
{
    HINSTANCE hinst = get_forwarded_dll(hinstPy3);

    எனில் (hinst == வெற்று || orig_exit != வெற்று)
	திரும்பு;

    orig_exit = hook_dll_import_func(hinst, "exit", (வெறுமை *)hooked_exit);
}

/*
 * Remove the hook installed by hook_py_exit().
 */
    நிலையான வெறுமை
restore_py_exit(வெறுமை)
{
    HINSTANCE hinst = hinstPy3;

    எனில் (hinst == வெற்று)
	திரும்பு;

    எனில் (orig_exit != வெற்று)
	hook_dll_import_func(hinst, "exit", orig_exit);
    orig_exit = வெற்று;
}
#endif

    நிலையான முழு
Python3_Init(வெறுமை)
{
    எனில் (!py3initialised)
    {
#ifdef DYNAMIC_PYTHON3
	எனில் (!python3_enabled(TRUE))
	{
	    emsg(_(e_sorry_this_command_is_disabled_python_library_could_not_be_found));
	    செல் fail;
	}
#endif
	// Python 3.13 introduced a really useful feature: colorized exceptions.
	// This is great if you're reading them from the terminal, but useless
	// and broken everywhere else (such as in log files, or text editors).
	// Opt out, forcefully.
	vim_setenv((char_u*)"PYTHON_COLORS", (char_u*)"0");

	init_structs();

	எனில் (*p_py3home != NUL)
	{
	    size_t len = mbstowcs(வெற்று, (எழுத்து *)p_py3home, 0) + 1;

	    // The string must not change later, make a copy in static memory.
	    py_home_buf = ALLOC_MULT(wchar_t, len);
	    எனில் (py_home_buf != வெற்று && mbstowcs(
			    py_home_buf, (எழுத்து *)p_py3home, len) != (size_t)-1)
		Py_SetPythonHome(py_home_buf);
	}
#ifdef PYTHON3_HOME
	இல்லையெனில் எனில் (mch_getenv((char_u *)"PYTHONHOME") == வெற்று)
	    Py_SetPythonHome(PYTHON3_HOME);
#endif

	PyImport_AppendInittab("vim", Py3Init_vim);

#எனில் !defined(DYNAMIC_PYTHON3) && defined(MSWIN)
	hinstPy3 = GetModuleHandle(PYTHON3_DLL);
#endif
	reset_stdin();

#ifdef HOOK_EXIT
	// Catch exit() called in Py_Initialize().
	hook_py_exit();
	எனில் (setjmp(exit_hook_jump_buf) == 0)
	{
	    Py_Initialize();
	    restore_py_exit();
	}
	இல்லையெனில்
	{
	    // exit() was called in Py_Initialize().
	    restore_py_exit();
	    emsg(_(e_critical_error_in_python3_initialization_check_your_installation));
	    செல் fail;
	}
#இல்லையெனில்
	Py_Initialize();
#endif

#எனில் PY_VERSION_HEX < 0x03090000
	// Initialise threads.  This is deprecated since Python 3.9.
	PyEval_InitThreads();
#endif
#ifdef DYNAMIC_PYTHON3
	get_py3_exceptions();
#endif

	எனில் (PythonIO_Init_io())
	    செல் fail;

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

    திரும்பு 0;

fail:
    // We call PythonIO_Flush() here to print any Python errors.
    // This is OK, as it is possible to call this function even
    // if PythonIO_Init_io() has not completed successfully (it will
    // not do anything in this case).
    PythonIO_Flush();
    திரும்பு -1;
}

/*
 * External interface
 */
    நிலையான வெறுமை
DoPyCommand(மாறிலி எழுத்து *cmd,
	    dict_T* locals,
	    rangeinitializer init_range,
	    runner run,
	    வெறுமை *arg)
{
#எனில் defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    எழுத்து		*saved_locale;
#endif
    PyObject		*cmdstr;
    PyObject		*cmdbytes;
    PyGILState_STATE	pygilstate;

    எனில் (python_end_called)
	செல் theend;

    எனில் (Python3_Init())
	செல் theend;

    init_range(arg);

    Python_Release_Vim();	    // leave Vim

#எனில் defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    // Python only works properly when the LC_NUMERIC locale is "C".
    saved_locale = setlocale(LC_NUMERIC, வெற்று);
    எனில் (saved_locale == வெற்று || STRCMP(saved_locale, "C") == 0)
	saved_locale = வெற்று;
    இல்லையெனில்
    {
	// Need to make a copy, value may change when setting new locale.
	saved_locale = (எழுத்து *)vim_strsave((char_u *)saved_locale);
	(வெறுமை)setlocale(LC_NUMERIC, "C");
    }
#endif

    pygilstate = PyGILState_Ensure();

    // PyRun_SimpleString expects a UTF-8 string. Wrong encoding may cause
    // SyntaxError (unicode error).
    cmdstr = PyUnicode_Decode(cmd, strlen(cmd),
					(எழுத்து *)ENC_OPT, ERRORS_DECODE_ARG);
    cmdbytes = PyUnicode_AsEncodedString(cmdstr, "utf-8", ERRORS_ENCODE_ARG);
    Py_XDECREF(cmdstr);

    run(PyBytes_AsString(cmdbytes), locals, arg, &pygilstate);
    Py_XDECREF(cmdbytes);

    PyGILState_Release(pygilstate);

#எனில் defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    எனில் (saved_locale != வெற்று)
    {
	(வெறுமை)setlocale(LC_NUMERIC, saved_locale);
	vim_free(saved_locale);
    }
#endif

    Python_Lock_Vim();		    // enter Vim
    PythonIO_Flush();

theend:
    திரும்பு;	    // keeps lint happy
}

/*
 * ":py3"
 */
    வெறுமை
ex_py3(exarg_T *eap)
{
    char_u *script;

    script = script_get(eap, eap->arg);
    எனில் (!eap->skip)
    {
	எனில் (p_pyx == 0)
	    p_pyx = 3;

	DoPyCommand(script == வெற்று ? (எழுத்து *) eap->arg : (எழுத்து *) script,
		வெற்று,
		init_range_cmd,
		(runner) run_cmd,
		(வெறுமை *) eap);
    }
    vim_free(script);
}

#define BUFFER_SIZE 2048

/*
 * ":py3file"
 */
    வெறுமை
ex_py3file(exarg_T *eap)
{
    நிலையான எழுத்து buffer[BUFFER_SIZE];
    மாறிலி எழுத்து *file;
    எழுத்து *p;
    முழு i;

    எனில் (p_pyx == 0)
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

    க்கு (i=0; i<2; ++i)
    {
	file = (எழுத்து *)eap->arg;
	போது (*file && p < buffer + (BUFFER_SIZE - 3))
	{
	    எனில் (*file == '\\' || *file == '\'')
		*p++ = '\\';
	    *p++ = *file++;
	}
	// If we didn't finish the file name, we hit a buffer overflow
	எனில் (*file != '\0')
	    திரும்பு;
	எனில் (i==0)
	{
	    strcpy(p,"','rb').read(),'");
	    p += 16;
	}
	இல்லையெனில்
	{
	    strcpy(p,"','exec'))");
	    p += 10;
	}
    }


    // Execute the file
    DoPyCommand(buffer,
	    வெற்று,
	    init_range_cmd,
	    (runner) run_cmd,
	    (வெறுமை *) eap);
}

    வெறுமை
ex_py3do(exarg_T *eap)
{
    எனில் (p_pyx == 0)
	p_pyx = 3;

    DoPyCommand((எழுத்து *)eap->arg,
	    வெற்று,
	    init_range_cmd,
	    (runner)run_do,
	    (வெறுமை *)eap);
}

///////////////////////////////////////////////////////
// 2. Python output stream: writes output via [e]msg().

// Implementation functions

    நிலையான PyObject *
OutputGetattro(PyObject *self, PyObject *nameobj)
{
    GET_ATTR_STRING(name, nameobj);

    எனில் (strcmp(name, "softspace") == 0)
	திரும்பு PyLong_FromLong(((OutputObject *)(self))->softspace);
    இல்லையெனில் எனில் (strcmp(name, "errors") == 0)
	திரும்பு PyString_FromString("strict");
    இல்லையெனில் எனில் (strcmp(name, "encoding") == 0)
	திரும்பு PyString_FromString(ENC_OPT);

    திரும்பு PyObject_GenericGetAttr(self, nameobj);
}

    நிலையான முழு
OutputSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);

    திரும்பு OutputSetattr(self, name, val);
}

///////////////////////////////////////////////////////
// 3. Implementation of the Vim module for Python

// Window type - Implementation functions
// --------------------------------------

#define WindowType_Check(obj) ((obj)->ob_base.ob_type == &WindowType)

// Buffer type - Implementation functions
// --------------------------------------

#define BufferType_Check(obj) ((obj)->ob_base.ob_type == &BufferType)

நிலையான PyObject* BufferSubscript(PyObject *self, PyObject *idx);
நிலையான முழு BufferAsSubscript(PyObject *self, PyObject *idx, PyObject *val);

// Line range type - Implementation functions
// --------------------------------------

#define RangeType_Check(obj) ((obj)->ob_base.ob_type == &RangeType)

நிலையான PyObject* RangeSubscript(PyObject *self, PyObject *idx);
நிலையான முழு RangeAsItem(PyObject *, Py_ssize_t, PyObject *);
நிலையான முழு RangeAsSubscript(PyObject *self, PyObject *idx, PyObject *val);

// Current objects type - Implementation functions
// -----------------------------------------------

நிலையான PySequenceMethods BufferAsSeq = {
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

நிலையான PyMappingMethods BufferAsMapping = {
    /* mp_length	*/ (lenfunc)BufferLength,
    /* mp_subscript     */ (binaryfunc)BufferSubscript,
    /* mp_ass_subscript */ (objobjargproc)BufferAsSubscript,
};


// Buffer object

    நிலையான PyObject *
BufferGetattro(PyObject *self, PyObject *nameobj)
{
    PyObject *r;

    GET_ATTR_STRING(name, nameobj);

    எனில் ((r = BufferAttrValid((BufferObject *)(self), name)))
	திரும்பு r;

    எனில் (CheckBuffer((BufferObject *)(self)))
	திரும்பு வெற்று;

    r = BufferAttr((BufferObject *)(self), name);
    எனில் (r || PyErr_Occurred())
	திரும்பு r;
    இல்லையெனில்
	திரும்பு PyObject_GenericGetAttr(self, nameobj);
}

    நிலையான முழு
BufferSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);

    திரும்பு BufferSetattr(self, name, val);
}

//////////////////

    நிலையான PyObject *
BufferSubscript(PyObject *self, PyObject* idx)
{
    எனில் (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	திரும்பு BufferItem((BufferObject *)(self), _idx);
    }
    இல்லையெனில் எனில் (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	எனில் (CheckBuffer((BufferObject *) self))
	    திரும்பு வெற்று;

	எனில் (PySlice_GetIndicesEx((PySliceObject_T *)idx,
	      (Py_ssize_t)((BufferObject *)(self))->buf->b_ml.ml_வரி_மொத்தம்,
	      &start, &stop,
	      &step, &slicelen) < 0)
	    திரும்பு வெற்று;
	திரும்பு BufferSlice((BufferObject *)(self), start, stop);
    }
    இல்லையெனில்
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	திரும்பு வெற்று;
    }
}

    நிலையான முழு
BufferAsSubscript(PyObject *self, PyObject* idx, PyObject* val)
{
    எனில் (PyLong_Check(idx))
    {
	long n = PyLong_AsLong(idx);

	எனில் (CheckBuffer((BufferObject *) self))
	    திரும்பு -1;

	திரும்பு RBAsItem((BufferObject *)(self), n, val, 1,
		    (Py_ssize_t)((BufferObject *)(self))->buf->b_ml.ml_வரி_மொத்தம்,
		    வெற்று);
    }
    இல்லையெனில் எனில் (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	எனில் (CheckBuffer((BufferObject *) self))
	    திரும்பு -1;

	எனில் (PySlice_GetIndicesEx((PySliceObject_T *)idx,
	      (Py_ssize_t)((BufferObject *)(self))->buf->b_ml.ml_வரி_மொத்தம்,
	      &start, &stop,
	      &step, &slicelen) < 0)
	    திரும்பு -1;
	திரும்பு RBAsSlice((BufferObject *)(self), start, stop, val, 1,
			  (PyInt)((BufferObject *)(self))->buf->b_ml.ml_வரி_மொத்தம்,
			  வெற்று);
    }
    இல்லையெனில்
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	திரும்பு -1;
    }
}

நிலையான PySequenceMethods RangeAsSeq = {
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

நிலையான PyMappingMethods RangeAsMapping = {
    /* mp_length	*/ (lenfunc)RangeLength,
    /* mp_subscript     */ (binaryfunc)RangeSubscript,
    /* mp_ass_subscript */ (objobjargproc)RangeAsSubscript,
};

// Line range object - Implementation

    நிலையான PyObject *
RangeGetattro(PyObject *self, PyObject *nameobj)
{
    GET_ATTR_STRING(name, nameobj);

    எனில் (strcmp(name, "start") == 0)
	திரும்பு Py_Build_மதிப்பு("n", ((RangeObject *)(self))->start - 1);
    இல்லையெனில் எனில் (strcmp(name, "end") == 0)
	திரும்பு Py_Build_மதிப்பு("n", ((RangeObject *)(self))->end - 1);
    இல்லையெனில்
	திரும்பு PyObject_GenericGetAttr(self, nameobj);
}

////////////////

    நிலையான முழு
RangeAsItem(PyObject *self, Py_ssize_t n, PyObject *val)
{
    திரும்பு RBAsItem(((RangeObject *)(self))->buf, n, val,
		    ((RangeObject *)(self))->start,
		    ((RangeObject *)(self))->end,
		    &((RangeObject *)(self))->end);
}

    நிலையான Py_ssize_t
RangeAsSlice(PyObject *self, Py_ssize_t lo, Py_ssize_t hi, PyObject *val)
{
    திரும்பு RBAsSlice(((RangeObject *)(self))->buf, lo, hi, val,
		    ((RangeObject *)(self))->start,
		    ((RangeObject *)(self))->end,
		    &((RangeObject *)(self))->end);
}

    நிலையான PyObject *
RangeSubscript(PyObject *self, PyObject* idx)
{
    எனில் (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	திரும்பு RangeItem((RangeObject *)(self), _idx);
    }
    இல்லையெனில் எனில் (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	எனில் (PySlice_GetIndicesEx((PySliceObject_T *)idx,
		((RangeObject *)(self))->end-((RangeObject *)(self))->start+1,
		&start, &stop,
		&step, &slicelen) < 0)
	    திரும்பு வெற்று;
	திரும்பு RangeSlice((RangeObject *)(self), start, stop);
    }
    இல்லையெனில்
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	திரும்பு வெற்று;
    }
}

    நிலையான முழு
RangeAsSubscript(PyObject *self, PyObject *idx, PyObject *val)
{
    எனில் (PyLong_Check(idx))
    {
	long n = PyLong_AsLong(idx);
	திரும்பு RangeAsItem(self, n, val);
    }
    இல்லையெனில் எனில் (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	எனில் (PySlice_GetIndicesEx((PySliceObject_T *)idx,
		((RangeObject *)(self))->end-((RangeObject *)(self))->start+1,
		&start, &stop,
		&step, &slicelen) < 0)
	    திரும்பு -1;
	திரும்பு RangeAsSlice(self, start, stop, val);
    }
    இல்லையெனில்
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	திரும்பு -1;
    }
}

// TabPage object - Implementation

    நிலையான PyObject *
TabPageGetattro(PyObject *self, PyObject *nameobj)
{
    PyObject *r;

    GET_ATTR_STRING(name, nameobj);

    எனில் ((r = TabPageAttrValid((TabPageObject *)(self), name)))
	திரும்பு r;

    எனில் (CheckTabPage((TabPageObject *)(self)))
	திரும்பு வெற்று;

    r = TabPageAttr((TabPageObject *)(self), name);
    எனில் (r || PyErr_Occurred())
	திரும்பு r;
    இல்லையெனில்
	திரும்பு PyObject_GenericGetAttr(self, nameobj);
}

// Window object - Implementation

    நிலையான PyObject *
WindowGetattro(PyObject *self, PyObject *nameobj)
{
    PyObject *r;

    GET_ATTR_STRING(name, nameobj);

    எனில் ((r = WindowAttrValid((WindowObject *)(self), name)))
	திரும்பு r;

    எனில் (CheckWindow((WindowObject *)(self)))
	திரும்பு வெற்று;

    r = WindowAttr((WindowObject *)(self), name);
    எனில் (r || PyErr_Occurred())
	திரும்பு r;
    இல்லையெனில்
	திரும்பு PyObject_GenericGetAttr(self, nameobj);
}

    நிலையான முழு
WindowSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);

    திரும்பு WindowSetattr(self, name, val);
}

// Tab page list object - Definitions

நிலையான PySequenceMethods TabListAsSeq = {
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

நிலையான PySequenceMethods WinListAsSeq = {
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
    நிலையான PyObject *
CurrentGetattro(PyObject *self, PyObject *nameobj)
{
    PyObject	*r;
    GET_ATTR_STRING(name, nameobj);
    எனில் (!(r = CurrentGetattr(self, name)))
	திரும்பு PyObject_GenericGetAttr(self, nameobj);
    திரும்பு r;
}

    நிலையான முழு
CurrentSetattro(PyObject *self, PyObject *nameobj, PyObject *மதிப்பு)
{
    GET_ATTR_STRING(name, nameobj);
    திரும்பு CurrentSetattr(self, name, மதிப்பு);
}

// Dictionary object - Definitions

    நிலையான PyObject *
DictionaryGetattro(PyObject *self, PyObject *nameobj)
{
    DictionaryObject	*this = ((DictionaryObject *) (self));

    GET_ATTR_STRING(name, nameobj);

    எனில் (strcmp(name, "locked") == 0)
	திரும்பு PyLong_FromLong(this->dict->dv_lock);
    இல்லையெனில் எனில் (strcmp(name, "scope") == 0)
	திரும்பு PyLong_FromLong(this->dict->dv_scope);

    திரும்பு PyObject_GenericGetAttr(self, nameobj);
}

    நிலையான முழு
DictionarySetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);
    திரும்பு DictionarySetattr(self, name, val);
}

// List object - Definitions

    நிலையான PyObject *
ListGetattro(PyObject *self, PyObject *nameobj)
{
    GET_ATTR_STRING(name, nameobj);

    எனில் (strcmp(name, "locked") == 0)
	திரும்பு PyLong_FromLong(((ListObject *) (self))->list->lv_lock);

    திரும்பு PyObject_GenericGetAttr(self, nameobj);
}

    நிலையான முழு
ListSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);
    திரும்பு ListSetattr(self, name, val);
}

// Tuple object - Definitions

    நிலையான PyObject *
TupleGetattro(PyObject *self, PyObject *nameobj)
{
    GET_ATTR_STRING(name, nameobj);

    எனில் (strcmp(name, "locked") == 0)
	திரும்பு PyLong_FromLong(((TupleObject *) (self))->tuple->tv_lock);

    திரும்பு PyObject_GenericGetAttr(self, nameobj);
}

    நிலையான முழு
TupleSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);
    திரும்பு TupleSetattr(self, name, val);
}

// Function object - Definitions

    நிலையான PyObject *
FunctionGetattro(PyObject *self, PyObject *nameobj)
{
    PyObject		*r;
    FunctionObject	*this = (FunctionObject *)(self);

    GET_ATTR_STRING(name, nameobj);

    r = FunctionAttr(this, name);
    எனில் (r || PyErr_Occurred())
	திரும்பு r;
    இல்லையெனில்
	திரும்பு PyObject_GenericGetAttr(self, nameobj);
}

// External interface

    வெறுமை
python3_buffer_free(buf_T *buf)
{
    BufferObject *bp = BUF_PYTHON_REF(buf);
    எனில் (bp == வெற்று)
	திரும்பு;
    bp->buf = INVALID_BUFFER_மதிப்பு;
    BUF_PYTHON_REF(buf) = வெற்று;
}

    வெறுமை
python3_window_free(win_T *win)
{
    WindowObject *wp = WIN_PYTHON_REF(win);
    எனில் (wp == வெற்று)
	திரும்பு;
    wp->win = INVALID_WINDOW_மதிப்பு;
    WIN_PYTHON_REF(win) = வெற்று;
}

    வெறுமை
python3_tabpage_free(tabpage_T *tab)
{
    TabPageObject *tp = TAB_PYTHON_REF(tab);
    எனில் (tp == வெற்று)
	திரும்பு;
    tp->tab = INVALID_TABPAGE_மதிப்பு;
    TAB_PYTHON_REF(tab) = வெற்று;
}

    நிலையான PyObject *
Py3Init_vim(வெறுமை)
{
    // The special value is removed from sys.path in Python3_Init().
    நிலையான wchar_t *(argv[2]) = {L"/must>not&exist/foo", வெற்று};

    எனில் (init_types())
	திரும்பு வெற்று;

    // Set sys.argv[] to avoid a crash in warn().
    PySys_SetArgv(1, argv);

    எனில் ((vim_module = PyModule_Create(&vimmodule)) == வெற்று)
	திரும்பு வெற்று;

    எனில் (populate_module(vim_module))
	திரும்பு வெற்று;

    எனில் (init_sys_path())
	திரும்பு வெற்று;

    திரும்பு vim_module;
}

//////////////////////////////////////////////////////////////////////////
// 4. Utility functions for handling the interface between Vim and Python.

/*
 * Convert a Vim line into a Python string.
 * All internal newlines are replaced by null characters.
 *
 * On errors, the Python exception data is set, and NULL is returned.
 */
    நிலையான PyObject *
வரி_To_String(மாறிலி எழுத்து *str)
{
    PyObject *result;
    Py_ssize_t len = strlen(str);
    எழுத்து *tmp, *p;

    tmp = alloc(len + 1);
    p = tmp;
    எனில் (p == வெற்று)
    {
	PyErr_NoMemory();
	திரும்பு வெற்று;
    }

    போது (*str)
    {
	எனில் (*str == '\n')
	    *p = '\0';
	இல்லையெனில்
	    *p = *str;

	++p;
	++str;
    }
    *p = '\0';

    result = PyUnicode_Decode(tmp, len, (எழுத்து *)ENC_OPT, ERRORS_DECODE_ARG);

    vim_free(tmp);
    திரும்பு result;
}

    வெறுமை
do_py3eval(char_u *str, dict_T *locals, typval_T *rettv)
{
    DoPyCommand((எழுத்து *) str,
	    locals,
	    init_range_eval,
	    (runner) run_eval,
	    (வெறுமை *) rettv);
    எனில் (rettv->v_type == VAR_UNKNOWN)
    {
	rettv->v_type = VAR_எண்;
	rettv->vval.v_எண் = 0;
    }
}

    முழு
set_ref_in_python3(முழு copyID)
{
    திரும்பு set_ref_in_py(copyID);
}

    முழு
python3_version(வெறுமை)
{
#ifdef USE_LIMITED_API
    திரும்பு Py_LIMITED_API;
#இல்லையெனில்
    திரும்பு PY_VERSION_HEX;
#endif
}
