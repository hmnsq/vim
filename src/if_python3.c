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

#അന്തര്ഭാവ "vim.h"

#അന്തര്ഭാവ <limits.h>

#എങ്കിൽ defined(MSWIN) && defined(HAVE_FCNTL_H)
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

#അന്തര്ഭാവ <Python.h>

#undef പ്രധാന // Defined in python.h - aargh
#undef HAVE_FCNTL_H // Clash with os_win32.h

// The "surrogateescape" error handler is new in Python 3.1
#എങ്കിൽ PY_VERSION_HEX >= 0x030100f0
# define CODEC_ERROR_HANDLER "surrogateescape"
#അല്ലെങ്കിൽ
# define CODEC_ERROR_HANDLER ശൂന്യ
#endif

// Suppress Python 3.11 depreciations to see useful warnings
#ifdef __GNUC__
# pragma GCC diagnostic തള്ളുക
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
#എങ്കിൽ PY_VERSION_HEX >= 0x030200f0
തരനിർവചനം PyObject PySliceObject_T;
#അല്ലെങ്കിൽ
തരനിർവചനം PySliceObject PySliceObject_T;
#endif

#ifndef MSWIN
# define HINSTANCE ശൂന്യം *
#endif
#എങ്കിൽ defined(DYNAMIC_PYTHON3) || defined(MSWIN)
സ്ഥിതം HINSTANCE hinstPy3 = 0; // Instance of python.dll
#endif

#എങ്കിൽ defined(DYNAMIC_PYTHON3)

# ifdef MSWIN
#  define load_dll vimLoadLib
#  define close_dll FreeLibrary
#  define ചിഹ്നം_from_dll GetProcAddress
#  define load_dll_error GetWin32Error
# അല്ലെങ്കിൽ
#  include <dlfcn.h>
#  define FARPROC ശൂന്യം*
#  എങ്കിൽ defined(PY_NO_RTLD_GLOBAL) && defined(PY3_NO_RTLD_GLOBAL)
#   define load_dll(n) dlopen((n), RTLD_LAZY)
#  അല്ലെങ്കിൽ
#   define load_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#  endif
#  define close_dll dlclose
#  define ചിഹ്നം_from_dll dlsym
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
# define Py_List_തിരുകുക py3_Py_List_തിരുകുക
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
# എങ്കിൽ PY_VERSION_HEX >= 0x030601f0
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
# ifndef Py_Mapping_താക്കോലുകൾ
#  define Py_Mapping_താക്കോലുകൾ py3_Py_Mapping_താക്കോലുകൾ
# endif
# എങ്കിൽ (defined(USE_LIMITED_API) && Py_LIMITED_API >= 0x03080000) || \
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
# എങ്കിൽ defined(USE_LIMITED_API) || PY_VERSION_HEX >= 0x03080000
#  define Py_IncRef py3_Py_IncRef
#  define Py_DecRef py3_Py_DecRef
# endif
# ifdef USE_LIMITED_API
#  define Py_CompileString py3_Py_CompileString
#  define PyEval_EvalCode py3_PyEval_EvalCode
# അല്ലെങ്കിൽ
#  undef PyRun_SimpleString
#  define PyRun_SimpleString py3_PyRun_SimpleString
#  undef PyRun_String
#  define PyRun_String py3_PyRun_String
# endif
# define PyObject_GetAttrString py3_PyObject_GetAttrString
# define PyObject_HasAttrString py3_PyObject_HasAttrString
# define PyObject_SetAttrString py3_PyObject_SetAttrString
# define PyObject_CallFunctionObjArgs py3_PyObject_CallFunctionObjArgs
# എങ്കിൽ PY_VERSION_HEX >= 0x030d0000
#  define PyObject_CallFunction py3_PyObject_CallFunction
# അല്ലെങ്കിൽ
#  define _PyObject_CallFunction_SizeT py3__PyObject_CallFunction_SizeT
# endif
# define PyObject_Call py3_PyObject_Call
# define PyEval_GetLocals py3_PyEval_GetLocals
# define PyEval_GetGlobals py3_PyEval_GetGlobals
# define PySys_SetObject py3_PySys_SetObject
# define PySys_GetObject py3_PySys_GetObject
# define PySys_SetArgv py3_PySys_SetArgv
# define PyType_Ready py3_PyType_Ready
# എങ്കിൽ PY_VERSION_HEX >= 0x03040000
#  define PyType_GetFlags py3_PyType_GetFlags
# endif
# undef Py_Build_മൂല്യം
# define Py_Build_മൂല്യം py3_Py_Build_മൂല്യം
# define Py_SetPythonHome py3_Py_SetPythonHome
# define Py_Initialize py3_Py_Initialize
# define Py_Finalize py3_Py_Finalize
# define Py_IsInitialized py3_Py_IsInitialized
# define _Py_NoneStruct (*py3__Py_NoneStruct)
# define _Py_FalseStruct (*py3__Py_FalseStruct)
# define _Py_TrueStruct (*py3__Py_TrueStruct)
# എങ്കിൽ !defined(USE_LIMITED_API) && PY_VERSION_HEX < 0x030D0000
// Private symbol that used to be required as part of PyIter_Check.
#  define _PyObject_NextNotImplemented (*py3__PyObject_NextNotImplemented)
# endif
# define PyModule_AddObject py3_PyModule_AddObject
# define PyImport_AppendInittab py3_PyImport_AppendInittab
# define PyImport_AddModule py3_PyImport_AddModule
# ifdef USE_LIMITED_API
#  എങ്കിൽ Py_LIMITED_API >= 0x030a0000
#   define PyUnicode_AsUTF8AndSize py3_PyUnicode_AsUTF8AndSize
#  endif
# അല്ലെങ്കിൽ
#  എങ്കിൽ PY_VERSION_HEX >= 0x030300f0
#   define PyUnicode_AsUTF8AndSize py3_PyUnicode_AsUTF8AndSize
#  അല്ലെങ്കിൽ
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
# define Py_സംഖ്യ_Check (*py3_Py_സംഖ്യ_Check)
# define Py_സംഖ്യ_Long (*py3_Py_സംഖ്യ_Long)
# define PyBool_Type (*py3_PyBool_Type)
# define PyErr_NewException py3_PyErr_NewException
# ifdef Py_DEBUG
#  define _Py_NegativeRefcount py3__Py_NegativeRefcount
#  define _Py_RefTotal (*py3__Py_RefTotal)
#  define PyModule_Create2TraceRefs py3_PyModule_Create2TraceRefs
# അല്ലെങ്കിൽ
#  define PyModule_Create2 py3_PyModule_Create2
# endif
# എങ്കിൽ defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
#  define _PyObject_DebugMalloc py3__PyObject_DebugMalloc
#  define _PyObject_DebugFree py3__PyObject_DebugFree
# അല്ലെങ്കിൽ
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
#  അല്ലെങ്കിൽ
#   define PyUnicodeUCS2_FromFormat py3_PyUnicodeUCS2_FromFormat
#  endif
# അല്ലെങ്കിൽ
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

# എങ്കിൽ defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
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
സ്ഥിതം പൂർണ്ണസംഖ്യ (*py3_PySys_SetArgv)(പൂർണ്ണസംഖ്യ, wchar_t **);
സ്ഥിതം ശൂന്യം (*py3_Py_SetPythonHome)(wchar_t *home);
സ്ഥിതം ശൂന്യം (*py3_Py_Initialize)(ശൂന്യം);
സ്ഥിതം PyObject* (*py3_PyList_New)(Py_ssize_t size);
സ്ഥിതം PyGILState_STATE (*py3_PyGILState_Ensure)(ശൂന്യം);
സ്ഥിതം ശൂന്യം (*py3_PyGILState_Release)(PyGILState_STATE);
സ്ഥിതം പൂർണ്ണസംഖ്യ (*py3_PySys_SetObject)(അക്ഷരം *, PyObject *);
സ്ഥിതം PyObject* (*py3_PySys_GetObject)(അക്ഷരം *);
സ്ഥിതം പൂർണ്ണസംഖ്യ (*py3_PyList_Append)(PyObject *, PyObject *);
സ്ഥിതം പൂർണ്ണസംഖ്യ (*py3_Py_List_തിരുകുക)(PyObject *, പൂർണ്ണസംഖ്യ, PyObject *);
സ്ഥിതം Py_ssize_t (*py3_PyList_Size)(PyObject *);
സ്ഥിതം PyObject* (*py3_PyTuple_New)(Py_ssize_t size);
സ്ഥിതം പൂർണ്ണസംഖ്യ (*py3_PyTuple_SetItem)(PyObject *, Py_ssize_t, PyObject *);
സ്ഥിതം Py_ssize_t (*py3_PyTuple_Size)(PyObject *);
സ്ഥിതം പൂർണ്ണസംഖ്യ (*py3_PySequence_Check)(PyObject *);
സ്ഥിതം Py_ssize_t (*py3_PySequence_Size)(PyObject *);
സ്ഥിതം PyObject* (*py3_PySequence_GetItem)(PyObject *, Py_ssize_t);
സ്ഥിതം PyObject* (*py3_PySequence_Fast)(PyObject *, സ്ഥിരം അക്ഷരം *);
സ്ഥിതം PyObject* (*py3_PyTuple_GetItem)(PyObject *, Py_ssize_t);
സ്ഥിതം പൂർണ്ണസംഖ്യ (*py3_PyMapping_Check)(PyObject *);
സ്ഥിതം PyObject* (*py3_Py_Mapping_താക്കോലുകൾ)(PyObject *);
# എങ്കിൽ PY_VERSION_HEX >= 0x030601f0
സ്ഥിതം പൂർണ്ണസംഖ്യ (*py3_PySlice_AdjustIndices)(Py_ssize_t length,
		     Py_ssize_t *start, Py_ssize_t *stop, Py_ssize_t step);
സ്ഥിതം പൂർണ്ണസംഖ്യ (*py3_PySlice_Unpack)(PyObject *slice,
		     Py_ssize_t *start, Py_ssize_t *stop, Py_ssize_t *step);
# endif
സ്ഥിതം പൂർണ്ണസംഖ്യ (*py3_PySlice_GetIndicesEx)(PySliceObject_T *r, Py_ssize_t length,
		     Py_ssize_t *start, Py_ssize_t *stop, Py_ssize_t *step,
		     Py_ssize_t *slicelen);
സ്ഥിതം PyObject* (*py3_PyErr_NoMemory)(ശൂന്യം);
സ്ഥിതം ശൂന്യം (*py3_Py_Finalize)(ശൂന്യം);
സ്ഥിതം ശൂന്യം (*py3_PyErr_SetString)(PyObject *, സ്ഥിരം അക്ഷരം *);
സ്ഥിതം ശൂന്യം (*py3_PyErr_SetObject)(PyObject *, PyObject *);
സ്ഥിതം പൂർണ്ണസംഖ്യ (*py3_PyErr_ExceptionMatches)(PyObject *);
# എങ്കിൽ defined(USE_LIMITED_API) || PY_VERSION_HEX >= 0x03080000
സ്ഥിതം ശൂന്യം (*py3_Py_IncRef)(PyObject *);
സ്ഥിതം ശൂന്യം (*py3_Py_DecRef)(PyObject *);
# endif
# ifdef USE_LIMITED_API
സ്ഥിതം PyObject* (*py3_Py_CompileString)(സ്ഥിരം അക്ഷരം *, സ്ഥിരം അക്ഷരം *, പൂർണ്ണസംഖ്യ);
സ്ഥിതം PyObject* (*py3_PyEval_EvalCode)(PyObject *co, PyObject *globals, PyObject *locals);
# അല്ലെങ്കിൽ
സ്ഥിതം പൂർണ്ണസംഖ്യ (*py3_PyRun_SimpleString)(അക്ഷരം *);
സ്ഥിതം PyObject* (*py3_PyRun_String)(അക്ഷരം *, പൂർണ്ണസംഖ്യ, PyObject *, PyObject *);
# endif
സ്ഥിതം PyObject* (*py3_PyObject_GetAttrString)(PyObject *, സ്ഥിരം അക്ഷരം *);
സ്ഥിതം പൂർണ്ണസംഖ്യ (*py3_PyObject_HasAttrString)(PyObject *, സ്ഥിരം അക്ഷരം *);
സ്ഥിതം പൂർണ്ണസംഖ്യ (*py3_PyObject_SetAttrString)(PyObject *, സ്ഥിരം അക്ഷരം *, PyObject *);
സ്ഥിതം PyObject* (*py3_PyObject_CallFunctionObjArgs)(PyObject *, ...);
# എങ്കിൽ PY_VERSION_HEX >= 0x030d0000
സ്ഥിതം PyObject* (*py3_PyObject_CallFunction)(PyObject *, അക്ഷരം *, ...);
# അല്ലെങ്കിൽ
സ്ഥിതം PyObject* (*py3__PyObject_CallFunction_SizeT)(PyObject *, അക്ഷരം *, ...);
# endif
സ്ഥിതം PyObject* (*py3_PyObject_Call)(PyObject *, PyObject *, PyObject *);
സ്ഥിതം PyObject* (*py3_PyEval_GetGlobals)(ശൂന്യം);
സ്ഥിതം PyObject* (*py3_PyEval_GetLocals)(ശൂന്യം);
സ്ഥിതം PyObject* (*py3_PyList_GetItem)(PyObject *, Py_ssize_t);
സ്ഥിതം PyObject* (*py3_PyImport_ImportModule)(സ്ഥിരം അക്ഷരം *);
സ്ഥിതം PyObject* (*py3_PyImport_AddModule)(സ്ഥിരം അക്ഷരം *);
സ്ഥിതം പൂർണ്ണസംഖ്യ (*py3_PyErr_BadArgument)(ശൂന്യം);
സ്ഥിതം PyObject* (*py3_PyErr_Occurred)(ശൂന്യം);
സ്ഥിതം PyObject* (*py3_PyModule_GetDict)(PyObject *);
സ്ഥിതം പൂർണ്ണസംഖ്യ (*py3_PyList_SetItem)(PyObject *, Py_ssize_t, PyObject *);
സ്ഥിതം PyObject* (*py3_PyDict_GetItemString)(PyObject *, സ്ഥിരം അക്ഷരം *);
സ്ഥിതം പൂർണ്ണസംഖ്യ (*py3_PyDict_Next)(PyObject *, Py_ssize_t *, PyObject **, PyObject **);
സ്ഥിതം PyObject* (*py3_PyLong_FromLong)(long);
സ്ഥിതം PyObject* (*py3_PyDict_New)(ശൂന്യം);
# എങ്കിൽ (defined(USE_LIMITED_API) && Py_LIMITED_API >= 0x03080000) || \
       (!defined(USE_LIMITED_API) && PY_VERSION_HEX >= 0x03080000)
സ്ഥിതം പൂർണ്ണസംഖ്യ (*py3_PyIter_Check)(PyObject *o);
# endif
സ്ഥിതം PyObject* (*py3_PyIter_Next)(PyObject *);
സ്ഥിതം PyObject* (*py3_PyObject_GetIter)(PyObject *);
സ്ഥിതം PyObject* (*py3_PyObject_Repr)(PyObject *);
സ്ഥിതം PyObject* (*py3_PyObject_GetItem)(PyObject *, PyObject *);
സ്ഥിതം പൂർണ്ണസംഖ്യ (*py3_PyObject_IsTrue)(PyObject *);
സ്ഥിതം PyObject* (*py3_Py_Build_മൂല്യം)(അക്ഷരം *, ...);
# എങ്കിൽ PY_VERSION_HEX >= 0x03040000
സ്ഥിതം പൂർണ്ണസംഖ്യ (*py3_PyType_GetFlags)(PyTypeObject *o);
# endif
സ്ഥിതം പൂർണ്ണസംഖ്യ (*py3_PyType_Ready)(PyTypeObject *type);
സ്ഥിതം പൂർണ്ണസംഖ്യ (*py3_PyDict_SetItemString)(PyObject *dp, അക്ഷരം *താക്കോൽ, PyObject *item);
സ്ഥിതം PyObject* (*py3_PyUnicode_FromString)(സ്ഥിരം അക്ഷരം *u);
# ifdef Py_UNICODE_USE_UCS_FUNCTIONS
#  ifdef Py_UNICODE_WIDE
സ്ഥിതം PyObject* (*py3_PyUnicodeUCS4_FromFormat)(സ്ഥിരം അക്ഷരം *u, ...);
#  അല്ലെങ്കിൽ
സ്ഥിതം PyObject* (*py3_PyUnicodeUCS2_FromFormat)(സ്ഥിരം അക്ഷരം *u, ...);
#  endif
# അല്ലെങ്കിൽ
സ്ഥിതം PyObject* (*py3_PyUnicode_FromFormat)(സ്ഥിരം അക്ഷരം *u, ...);
# endif
സ്ഥിതം PyObject* (*py3_PyUnicode_Decode)(സ്ഥിരം അക്ഷരം *u, Py_ssize_t size,
	സ്ഥിരം അക്ഷരം *encoding, സ്ഥിരം അക്ഷരം *errors);
സ്ഥിതം long (*py3_PyLong_AsLong)(PyObject *);
സ്ഥിതം ശൂന്യം (*py3_PyErr_SetNone)(PyObject *);
സ്ഥിതം ശൂന്യം (*py3_PyEval_InitThreads)(ശൂന്യം);
സ്ഥിതം ശൂന്യം(*py3_PyEval_RestoreThread)(PyThreadState *);
സ്ഥിതം PyThreadState*(*py3_PyEval_SaveThread)(ശൂന്യം);
സ്ഥിതം പൂർണ്ണസംഖ്യ (*py3_PyArg_Parse)(PyObject *, അക്ഷരം *, ...);
സ്ഥിതം പൂർണ്ണസംഖ്യ (*py3_PyArg_ParseTuple)(PyObject *, അക്ഷരം *, ...);
സ്ഥിതം ശൂന്യം (*py3_PyMem_Free)(ശൂന്യം *);
സ്ഥിതം ശൂന്യം* (*py3_PyMem_Malloc)(size_t);
സ്ഥിതം പൂർണ്ണസംഖ്യ (*py3_Py_IsInitialized)(ശൂന്യം);
സ്ഥിതം ശൂന്യം (*py3_PyErr_Clear)(ശൂന്യം);
സ്ഥിതം PyObject* (*py3_PyErr_Format)(PyObject *, സ്ഥിരം അക്ഷരം *, ...);
സ്ഥിതം ശൂന്യം (*py3_PyErr_PrintEx)(പൂർണ്ണസംഖ്യ);
സ്ഥിതം PyObject*(*py3__PyObject_Init)(PyObject *, PyTypeObject *);
# എങ്കിൽ !defined(USE_LIMITED_API) && PY_VERSION_HEX < 0x030D0000
സ്ഥിതം iternextfunc py3__PyObject_NextNotImplemented;
# endif
സ്ഥിതം PyObject* py3__Py_NoneStruct;
സ്ഥിതം PyObject* py3__Py_FalseStruct;
സ്ഥിതം PyObject* py3__Py_TrueStruct;
സ്ഥിതം പൂർണ്ണസംഖ്യ (*py3_PyModule_AddObject)(PyObject *m, സ്ഥിരം അക്ഷരം *name, PyObject *o);
സ്ഥിതം പൂർണ്ണസംഖ്യ (*py3_PyImport_AppendInittab)(സ്ഥിരം അക്ഷരം *name, PyObject* (*initfunc)(ശൂന്യം));
# ifdef USE_LIMITED_API
#  എങ്കിൽ Py_LIMITED_API >= 0x030a0000
സ്ഥിതം അക്ഷരം* (*py3_PyUnicode_AsUTF8AndSize)(PyObject *unicode, Py_ssize_t *size);
#  endif
# അല്ലെങ്കിൽ
#  എങ്കിൽ PY_VERSION_HEX >= 0x030300f0
സ്ഥിതം അക്ഷരം* (*py3_PyUnicode_AsUTF8AndSize)(PyObject *unicode, Py_ssize_t *size);
#  അല്ലെങ്കിൽ
സ്ഥിതം അക്ഷരം* (*py3__PyUnicode_AsString)(PyObject *unicode);
#  endif
# endif
സ്ഥിതം പൂർണ്ണസംഖ്യ (*py3_PyUnicode_CompareWithASCIIString)(PyObject *unicode, സ്ഥിരം അക്ഷരം* string);
സ്ഥിതം PyObject* (*py3_PyUnicode_AsEncodedString)(PyObject *unicode, സ്ഥിരം അക്ഷരം* encoding, സ്ഥിരം അക്ഷരം* errors);
സ്ഥിതം PyObject* (*py3_PyUnicode_AsUTF8String)(PyObject *unicode);
സ്ഥിതം അക്ഷരം* (*py3_PyBytes_AsString)(PyObject *bytes);
സ്ഥിതം പൂർണ്ണസംഖ്യ (*py3_PyBytes_AsStringAndSize)(PyObject *bytes, അക്ഷരം **buffer, Py_ssize_t *length);
സ്ഥിതം PyObject* (*py3_PyBytes_FromString)(അക്ഷരം *str);
സ്ഥിതം PyObject* (*py3_PyBytes_FromStringAndSize)(അക്ഷരം *str, Py_ssize_t length);
# എങ്കിൽ PY_VERSION_HEX >= 0x030900b0
സ്ഥിതം PyObject* (*py3__PyObject_New)(PyTypeObject *);
# endif
സ്ഥിതം PyObject* (*py3_PyFloat_FromDouble)(ഇരട്ട അക്കം);
സ്ഥിതം ഇരട്ട (*py3_PyFloat_AsDouble)(PyObject *);
സ്ഥിതം PyObject* (*py3_PyObject_GenericGetAttr)(PyObject *obj, PyObject *name);
സ്ഥിതം PyObject* (*py3_PyType_GenericAlloc)(PyTypeObject *type, Py_ssize_t nitems);
സ്ഥിതം PyObject* (*py3_PyType_GenericNew)(PyTypeObject *type, PyObject *args, PyObject *kwds);
സ്ഥിതം PyTypeObject* py3_PyType_Type;
# ifndef USE_LIMITED_API
സ്ഥിതം PyTypeObject* py3_PyStdPrinter_Type;
# endif
സ്ഥിതം PyTypeObject* py3_PySlice_Type;
സ്ഥിതം PyTypeObject* py3_PyFloat_Type;
സ്ഥിതം PyTypeObject* py3_PyBool_Type;
സ്ഥിതം പൂർണ്ണസംഖ്യ (*py3_Py_സംഖ്യ_Check)(PyObject *);
സ്ഥിതം PyObject* (*py3_Py_സംഖ്യ_Long)(PyObject *);
സ്ഥിതം PyObject* (*py3_PyErr_NewException)(അക്ഷരം *name, PyObject *base, PyObject *dict);
സ്ഥിതം PyObject* (*py3_PyCapsule_New)(ശൂന്യം *, അക്ഷരം *, PyCapsule_Destructor);
സ്ഥിതം ശൂന്യം* (*py3_PyCapsule_GetPointer)(PyObject *, അക്ഷരം *);
# ifdef Py_DEBUG
സ്ഥിതം ശൂന്യം (*py3__Py_NegativeRefcount)(സ്ഥിരം അക്ഷരം *fname, പൂർണ്ണസംഖ്യ lineno, PyObject *op);
സ്ഥിതം Py_ssize_t* py3__Py_RefTotal;
സ്ഥിതം PyObject* (*py3_PyModule_Create2TraceRefs)(ഘടന PyModuleDef* module, പൂർണ്ണസംഖ്യ module_api_version);
# അല്ലെങ്കിൽ
സ്ഥിതം PyObject* (*py3_PyModule_Create2)(ഘടന PyModuleDef* module, പൂർണ്ണസംഖ്യ module_api_version);
# endif
# എങ്കിൽ defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
സ്ഥിതം ശൂന്യം (*py3__PyObject_DebugFree)(ശൂന്യം*);
സ്ഥിതം ശൂന്യം* (*py3__PyObject_DebugMalloc)(size_t);
# അല്ലെങ്കിൽ
സ്ഥിതം ശൂന്യം (*py3_PyObject_Free)(ശൂന്യം*);
സ്ഥിതം ശൂന്യം* (*py3_PyObject_Malloc)(size_t);
# endif
സ്ഥിതം PyObject*(*py3__PyObject_GC_New)(PyTypeObject *);
സ്ഥിതം ശൂന്യം(*py3_PyObject_GC_Del)(ശൂന്യം *);
സ്ഥിതം ശൂന്യം(*py3_PyObject_GC_UnTrack)(ശൂന്യം *);
സ്ഥിതം പൂർണ്ണസംഖ്യ (*py3_PyType_IsSubtype)(PyTypeObject *, PyTypeObject *);
# ifdef USE_LIMITED_API
സ്ഥിതം ശൂന്യം* (*py3_PyType_GetSlot)(PyTypeObject *, പൂർണ്ണസംഖ്യ);
സ്ഥിതം PyObject* (*py3_PyType_FromSpec)(PyType_Spec *);
# endif

// Imported exception objects
സ്ഥിതം PyObject *p3imp_PyExc_AttributeError;
സ്ഥിതം PyObject *p3imp_PyExc_IndexError;
സ്ഥിതം PyObject *p3imp_Py_Exc_താക്കോൽ_Error;
സ്ഥിതം PyObject *p3imp_PyExc_KeyboardInterrupt;
സ്ഥിതം PyObject *p3imp_PyExc_TypeError;
സ്ഥിതം PyObject *p3imp_Py_Exc_മൂല്യം_Error;
സ്ഥിതം PyObject *p3imp_PyExc_SystemExit;
സ്ഥിതം PyObject *p3imp_PyExc_RuntimeError;
സ്ഥിതം PyObject *p3imp_PyExc_ImportError;
സ്ഥിതം PyObject *p3imp_PyExc_OverflowError;

# define PyExc_AttributeError p3imp_PyExc_AttributeError
# define PyExc_IndexError p3imp_PyExc_IndexError
# define Py_Exc_താക്കോൽ_Error p3imp_Py_Exc_താക്കോൽ_Error
# define PyExc_KeyboardInterrupt p3imp_PyExc_KeyboardInterrupt
# define PyExc_TypeError p3imp_PyExc_TypeError
# define Py_Exc_മൂല്യം_Error p3imp_Py_Exc_മൂല്യം_Error
# define PyExc_SystemExit p3imp_PyExc_SystemExit
# define PyExc_RuntimeError p3imp_PyExc_RuntimeError
# define PyExc_ImportError p3imp_PyExc_ImportError
# define PyExc_OverflowError p3imp_PyExc_OverflowError

/*
 * Table of name to function pointer of python.
 */
# define PYTHON_PROC FARPROC
സ്ഥിതം ഘടന
{
    അക്ഷരം *name;
    PYTHON_PROC *ptr;
} py3_funcname_table[] =
{
    {"PySys_SetArgv", (PYTHON_PROC*)&py3_PySys_SetArgv},
    {"Py_SetPythonHome", (PYTHON_PROC*)&py3_Py_SetPythonHome},
    {"Py_Initialize", (PYTHON_PROC*)&py3_Py_Initialize},
    {"_PyArg_ParseTuple_SizeT", (PYTHON_PROC*)&py3_PyArg_ParseTuple},
    {"_Py_BuildValue_SizeT", (PYTHON_PROC*)&py3_Py_Build_മൂല്യം},
    {"PyMem_Free", (PYTHON_PROC*)&py3_PyMem_Free},
    {"PyMem_Malloc", (PYTHON_PROC*)&py3_PyMem_Malloc},
    {"PyList_New", (PYTHON_PROC*)&py3_PyList_New},
    {"PyGILState_Ensure", (PYTHON_PROC*)&py3_PyGILState_Ensure},
    {"PyGILState_Release", (PYTHON_PROC*)&py3_PyGILState_Release},
    {"PySys_SetObject", (PYTHON_PROC*)&py3_PySys_SetObject},
    {"PySys_GetObject", (PYTHON_PROC*)&py3_PySys_GetObject},
    {"PyList_Append", (PYTHON_PROC*)&py3_PyList_Append},
    {"PyList_Insert", (PYTHON_PROC*)&py3_Py_List_തിരുകുക},
    {"PyList_Size", (PYTHON_PROC*)&py3_PyList_Size},
    {"PyTuple_New", (PYTHON_PROC*)&py3_PyTuple_New},
    {"PyTuple_GetItem", (PYTHON_PROC*)&py3_PyTuple_GetItem},
    {"PyTuple_SetItem", (PYTHON_PROC*)&py3_PyTuple_SetItem},
    {"PyTuple_Size", (PYTHON_PROC*)&py3_PyTuple_Size},
    {"PySequence_Check", (PYTHON_PROC*)&py3_PySequence_Check},
    {"PySequence_Size", (PYTHON_PROC*)&py3_PySequence_Size},
    {"PySequence_GetItem", (PYTHON_PROC*)&py3_PySequence_GetItem},
    {"PySequence_Fast", (PYTHON_PROC*)&py3_PySequence_Fast},
# എങ്കിൽ PY_VERSION_HEX >= 0x030601f0
    {"PySlice_AdjustIndices", (PYTHON_PROC*)&py3_PySlice_AdjustIndices},
    {"PySlice_Unpack", (PYTHON_PROC*)&py3_PySlice_Unpack},
# endif
    {"PySlice_GetIndicesEx", (PYTHON_PROC*)&py3_PySlice_GetIndicesEx},
    {"PyErr_NoMemory", (PYTHON_PROC*)&py3_PyErr_NoMemory},
    {"Py_Finalize", (PYTHON_PROC*)&py3_Py_Finalize},
    {"PyErr_SetString", (PYTHON_PROC*)&py3_PyErr_SetString},
    {"PyErr_SetObject", (PYTHON_PROC*)&py3_PyErr_SetObject},
    {"PyErr_ExceptionMatches", (PYTHON_PROC*)&py3_PyErr_ExceptionMatches},
# എങ്കിൽ defined(USE_LIMITED_API) || PY_VERSION_HEX >= 0x03080000
    {"Py_IncRef", (PYTHON_PROC*)&py3_Py_IncRef},
    {"Py_DecRef", (PYTHON_PROC*)&py3_Py_DecRef},
# endif
# ifdef USE_LIMITED_API
    {"Py_CompileString", (PYTHON_PROC*)&py3_Py_CompileString},
    {"PyEval_EvalCode", (PYTHON_PROC*)&PyEval_EvalCode},
# അല്ലെങ്കിൽ
    {"PyRun_SimpleString", (PYTHON_PROC*)&py3_PyRun_SimpleString},
    {"PyRun_String", (PYTHON_PROC*)&py3_PyRun_String},
# endif
    {"PyObject_GetAttrString", (PYTHON_PROC*)&py3_PyObject_GetAttrString},
    {"PyObject_HasAttrString", (PYTHON_PROC*)&py3_PyObject_HasAttrString},
    {"PyObject_SetAttrString", (PYTHON_PROC*)&py3_PyObject_SetAttrString},
    {"PyObject_CallFunctionObjArgs", (PYTHON_PROC*)&py3_PyObject_CallFunctionObjArgs},
# എങ്കിൽ PY_VERSION_HEX >= 0x030d0000
    {"PyObject_CallFunction", (PYTHON_PROC*)&py3_PyObject_CallFunction},
# അല്ലെങ്കിൽ
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
    {"PyMapping_Keys", (PYTHON_PROC*)&py3_Py_Mapping_താക്കോലുകൾ},
# എങ്കിൽ (defined(USE_LIMITED_API) && Py_LIMITED_API >= 0x03080000) || \
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
# എങ്കിൽ PY_VERSION_HEX >= 0x03040000
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
# എങ്കിൽ !defined(USE_LIMITED_API) && PY_VERSION_HEX < 0x030D0000
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
#  എങ്കിൽ Py_LIMITED_API >= 0x030a0000
    {"PyUnicode_AsUTF8AndSize", (PYTHON_PROC*)&py3_PyUnicode_AsUTF8AndSize},
#  endif
# അല്ലെങ്കിൽ
#  എങ്കിൽ PY_VERSION_HEX >= 0x030300f0
    {"PyUnicode_AsUTF8AndSize", (PYTHON_PROC*)&py3_PyUnicode_AsUTF8AndSize},
#  അല്ലെങ്കിൽ
    {"_PyUnicode_AsString", (PYTHON_PROC*)&py3__PyUnicode_AsString},
#  endif
# endif
    {"PyUnicode_CompareWithASCIIString", (PYTHON_PROC*)&py3_PyUnicode_CompareWithASCIIString},
    {"PyUnicode_AsUTF8String", (PYTHON_PROC*)&py3_PyUnicode_AsUTF8String},
# ifdef Py_UNICODE_USE_UCS_FUNCTIONS
#  ifdef Py_UNICODE_WIDE
    {"PyUnicodeUCS4_FromFormat", (PYTHON_PROC*)&py3_PyUnicodeUCS4_FromFormat},
#  അല്ലെങ്കിൽ
    {"PyUnicodeUCS2_FromFormat", (PYTHON_PROC*)&py3_PyUnicodeUCS2_FromFormat},
#  endif
# അല്ലെങ്കിൽ
    {"PyUnicode_FromFormat", (PYTHON_PROC*)&py3_PyUnicode_FromFormat},
# endif
    {"PyBytes_AsString", (PYTHON_PROC*)&py3_PyBytes_AsString},
    {"PyBytes_AsStringAndSize", (PYTHON_PROC*)&py3_PyBytes_AsStringAndSize},
    {"PyBytes_FromString", (PYTHON_PROC*)&py3_PyBytes_FromString},
    {"PyBytes_FromStringAndSize", (PYTHON_PROC*)&py3_PyBytes_FromStringAndSize},
# എങ്കിൽ PY_VERSION_HEX >= 0x030900b0
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
    {"PyNumber_Check", (PYTHON_PROC*)&py3_Py_സംഖ്യ_Check},
    {"PyNumber_Long", (PYTHON_PROC*)&py3_Py_സംഖ്യ_Long},
    {"PyErr_NewException", (PYTHON_PROC*)&py3_PyErr_NewException},
# ifdef Py_DEBUG
    {"_Py_NegativeRefcount", (PYTHON_PROC*)&py3__Py_NegativeRefcount},
    {"_Py_RefTotal", (PYTHON_PROC*)&py3__Py_RefTotal},
    {"PyModule_Create2TraceRefs", (PYTHON_PROC*)&py3_PyModule_Create2TraceRefs},
# അല്ലെങ്കിൽ
    {"PyModule_Create2", (PYTHON_PROC*)&py3_PyModule_Create2},
# endif
# എങ്കിൽ defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
    {"_PyObject_DebugFree", (PYTHON_PROC*)&py3__PyObject_DebugFree},
    {"_PyObject_DebugMalloc", (PYTHON_PROC*)&py3__PyObject_DebugMalloc},
# അല്ലെങ്കിൽ
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
#  എങ്കിൽ PY_VERSION_HEX >= 0x03040000
    {"PyType_GetSlot", (PYTHON_PROC*)&py3_PyType_GetSlot},
#  endif
    {"PyType_FromSpec", (PYTHON_PROC*)&py3_PyType_FromSpec},
# endif
    {"", ശൂന്യ},
};

# എങ്കിൽ defined(USE_LIMITED_API) || PY_VERSION_HEX >= 0x03080000
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

# എങ്കിൽ PY_VERSION_HEX >= 0x030900b0
    സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ
py3_PyType_HasFeature(PyTypeObject *type, unsigned long feature)
{
    തിരികെ ((PyType_GetFlags(type) & feature) != 0);
}
#  define PyType_HasFeature(t,f) py3_PyType_HasFeature(t,f)
# endif

# എങ്കിൽ PY_VERSION_HEX >= 0x030a00b2
    സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ
py3__PyObject_TypeCheck(PyObject *ob, PyTypeObject *type)
{
    തിരികെ Py_IS_TYPE(ob, type) || PyType_IsSubtype(Py_TYPE(ob), type);
}
#  എങ്കിൽ PY_VERSION_HEX >= 0x030b00b3
#   undef PyObject_TypeCheck
#   define PyObject_TypeCheck(o,t) py3__PyObject_TypeCheck(o,t)
#  അല്ലെങ്കിൽ
#   define _PyObject_TypeCheck(o,t) py3__PyObject_TypeCheck(o,t)
#  endif
# endif

# എങ്കിൽ !defined(USE_LIMITED_API) && PY_VERSION_HEX >= 0x030c00b0
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
    സ്ഥിതം WCHAR *
py3_get_system_libname(സ്ഥിരം അക്ഷരം *libname)
{
    സ്ഥിരം WCHAR	*pythoncore = L"Software\\Python\\PythonCore";
    സ്ഥിരം അക്ഷരം	*cp = libname;
    WCHAR	subkey[128];
    HKEY	h_താക്കോൽ;
    പൂർണ്ണസംഖ്യ		i;
    DWORD	j, len;
    LSTATUS	ret;

    ആകുമ്പോൾ (*cp != '\0')
    {
	എങ്കിൽ (*cp == ':' || *cp == '\\' || *cp == '/')
	{
	    // Bail out if "libname" contains path separator, assume it is
	    // an absolute path.
	    തിരികെ ശൂന്യ;
	}
	++cp;
    }

    WCHAR   keyfound[32];
    HKEY    h_താക്കോൽ_മുകൾ[] = {HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE};
    HKEY    h_താക്കോൽ_Found = ശൂന്യ;
#  ifdef USE_LIMITED_API
    long    maxminor = -1;
#  endif
    വേണ്ടി (i = 0; i < ARRAY_LENGTH(h_താക്കോൽ_മുകൾ); i++)
    {
	long	major, minor;

	ret = Reg_Open_താക്കോൽ_Ex_W(h_താക്കോൽ_മുകൾ[i], pythoncore, 0, താക്കോൽ_READ, &h_താക്കോൽ);
	എങ്കിൽ (ret != ERROR_SUCCESS)
	    തുടരുക;
	വേണ്ടി (j = 0;; j++)
	{
	    WCHAR   keyname[32];
	    WCHAR   *wp;

	    len = ARRAY_LENGTH(keyname);
	    ret = Reg_Enum_താക്കോൽ_Ex_W(h_താക്കോൽ, j, keyname, &len,
						    ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ);
	    എങ്കിൽ (ret == ERROR_NO_MORE_ഇനങ്ങൾ)
		നിർത്തുക;

	    major = wcstol(keyname, &wp, 10);
	    എങ്കിൽ (*wp != L'.')
		തുടരുക;
	    minor = wcstol(wp + 1, &wp, 10);
#  ifdef _WIN64
	    എങ്കിൽ (*wp != L'\0')
		തുടരുക;
#  അല്ലെങ്കിൽ
	    എങ്കിൽ (wcscmp(wp, L"-32") != 0)
		തുടരുക;
#  endif

	    എങ്കിൽ (major != PY_MAJOR_VERSION)
		തുടരുക;
#  ifdef USE_LIMITED_API
	    // Search the latest version.
	    എങ്കിൽ ((minor > maxminor)
		    && (minor >= ((Py_LIMITED_API >> 16) & 0xff)))
	    {
		maxminor = minor;
		wcscpy(keyfound, keyname);
		h_താക്കോൽ_Found = h_താക്കോൽ_മുകൾ[i];
	    }
#  അല്ലെങ്കിൽ
	    // Check if it matches with the compiled version.
	    എങ്കിൽ (minor == PY_MINOR_VERSION)
	    {
		wcscpy(keyfound, keyname);
		h_താക്കോൽ_Found = h_താക്കോൽ_മുകൾ[i];
		നിർത്തുക;
	    }
#  endif
	}
	Reg_Close_താക്കോൽ(h_താക്കോൽ);
#  ifdef USE_LIMITED_API
	എങ്കിൽ (h_താക്കോൽ_Found != ശൂന്യ)
	    നിർത്തുക;
#  endif
    }
    എങ്കിൽ (h_താക്കോൽ_Found == ശൂന്യ)
	തിരികെ ശൂന്യ;

    swprintf(subkey, ARRAY_LENGTH(subkey), L"%ls\\%ls\\InstallPath",
							pythoncore, keyfound);
    ret = Reg_Get_മൂല്യം_W(h_താക്കോൽ_Found, subkey, ശൂന്യ, RRF_RT_REG_SZ,
							    ശൂന്യ, ശൂന്യ, &len);
    എങ്കിൽ (ret != ERROR_MORE_DATA && ret != ERROR_SUCCESS)
	തിരികെ ശൂന്യ;
    size_t len2 = len / വലുപ്പം(WCHAR) + 1 + strlen(libname);
    WCHAR *path = alloc(len2 * വലുപ്പം(WCHAR));
    എങ്കിൽ (path == ശൂന്യ)
	തിരികെ ശൂന്യ;
    ret = Reg_Get_മൂല്യം_W(h_താക്കോൽ_Found, subkey, ശൂന്യ, RRF_RT_REG_SZ,
							    ശൂന്യ, path, &len);
    എങ്കിൽ (ret != ERROR_SUCCESS)
    {
	vim_free(path);
	തിരികെ ശൂന്യ;
    }
    // Remove trailing path separators.
    size_t len3 = wcslen(path);
    എങ്കിൽ ((len3 > 0) && (path[len3 - 1] == L'/' || path[len3 - 1] == L'\\'))
	--len3;
    swprintf(path + len3, len2 - len3, L"\\%hs", libname);
    തിരികെ path;
}
# endif

/*
 * Load library and get all pointers.
 * Parameter 'libname' provides name of DLL.
 * Return OK or FAIL.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
py3_runtime_link_init(അക്ഷരം *libname, പൂർണ്ണസംഖ്യ verbose)
{
    പൂർണ്ണസംഖ്യ i;
    PYTHON_PROC *ucs_from_string = (PYTHON_PROC *)&py3_PyUnicode_FromString;
    PYTHON_PROC *ucs_decode = (PYTHON_PROC *)&py3_PyUnicode_Decode;
    PYTHON_PROC *ucs_as_encoded_string =
				 (PYTHON_PROC *)&py3_PyUnicode_AsEncodedString;

# എങ്കിൽ !(defined(PY_NO_RTLD_GLOBAL) && defined(PY3_NO_RTLD_GLOBAL)) && defined(UNIX) && defined(FEAT_PYTHON)
    // Can't have Python and Python3 loaded at the same time.
    // It causes a crash, because RTLD_GLOBAL is needed for
    // standard C extension libraries of one or both python versions.
    എങ്കിൽ (python_loaded())
    {
	എങ്കിൽ (verbose)
	    emsg(_(e_this_vim_cannot_execute_py3_after_using_python));
	തിരികെ FAIL;
    }
# endif

    എങ്കിൽ (hinstPy3 != 0)
	തിരികെ OK;
    hinstPy3 = load_dll(libname);

# ifdef MSWIN
    എങ്കിൽ (!hinstPy3)
    {
	// Attempt to use the path from InstallPath as stored in the registry.
	WCHAR *syslibname = py3_get_system_libname(libname);

	എങ്കിൽ (syslibname != ശൂന്യ)
	{
	    hinstPy3 = LoadLibraryExW(syslibname, ശൂന്യ,
		    LOAD_LIBRARY_തിരയൽ_DLL_LOAD_DIR |
		    LOAD_LIBRARY_തിരയൽ_SYSTEM32);
	    vim_free(syslibname);
	}
    }
# endif

    എങ്കിൽ (!hinstPy3)
    {
	എങ്കിൽ (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	തിരികെ FAIL;
    }

    വേണ്ടി (i = 0; py3_funcname_table[i].ptr; ++i)
    {
	എങ്കിൽ ((*py3_funcname_table[i].ptr = ചിഹ്നം_from_dll(hinstPy3,
			py3_funcname_table[i].name)) == ശൂന്യ)
	{
	    close_dll(hinstPy3);
	    hinstPy3 = 0;
	    എങ്കിൽ (verbose)
		semsg(_(e_could_not_load_library_function_str), py3_funcname_table[i].name);
	    തിരികെ FAIL;
	}
    }

    // Load unicode functions separately as only the ucs2 or the ucs4 functions
    // will be present in the library.
# എങ്കിൽ PY_VERSION_HEX >= 0x030300f0
    *ucs_from_string = ചിഹ്നം_from_dll(hinstPy3, "PyUnicode_FromString");
    *ucs_decode = ചിഹ്നം_from_dll(hinstPy3, "PyUnicode_Decode");
    *ucs_as_encoded_string = ചിഹ്നം_from_dll(hinstPy3,
	    "PyUnicode_AsEncodedString");
# അല്ലെങ്കിൽ
    *ucs_from_string = ചിഹ്നം_from_dll(hinstPy3, "PyUnicodeUCS2_FromString");
    *ucs_decode = ചിഹ്നം_from_dll(hinstPy3,
	    "PyUnicodeUCS2_Decode");
    *ucs_as_encoded_string = ചിഹ്നം_from_dll(hinstPy3,
	    "PyUnicodeUCS2_AsEncodedString");
    എങ്കിൽ (*ucs_from_string == ശൂന്യ || *ucs_decode == ശൂന്യ
					     || *ucs_as_encoded_string == ശൂന്യ)
    {
	*ucs_from_string = ചിഹ്നം_from_dll(hinstPy3,
		"PyUnicodeUCS4_FromString");
	*ucs_decode = ചിഹ്നം_from_dll(hinstPy3,
		"PyUnicodeUCS4_Decode");
	*ucs_as_encoded_string = ചിഹ്നം_from_dll(hinstPy3,
		"PyUnicodeUCS4_AsEncodedString");
    }
# endif
    എങ്കിൽ (*ucs_from_string == ശൂന്യ || *ucs_decode == ശൂന്യ
					     || *ucs_as_encoded_string == ശൂന്യ)
    {
	close_dll(hinstPy3);
	hinstPy3 = 0;
	എങ്കിൽ (verbose)
	    semsg(_(e_could_not_load_library_function_str), "PyUnicode_UCSX_*");
	തിരികെ FAIL;
    }

    തിരികെ OK;
}

/*
 * If python is enabled (there is installed python on Windows system) return
 * TRUE, else FALSE.
 */
    പൂർണ്ണസംഖ്യ
python3_enabled(പൂർണ്ണസംഖ്യ verbose)
{
    തിരികെ py3_runtime_link_init((അക്ഷരം *)p_py3dll, verbose) == OK;
}

/*
 * Load the standard Python exceptions - don't import the symbols from the
 * DLL, as this can cause errors (importing data symbols is not reliable).
 */
    സ്ഥിതം ശൂന്യം
get_py3_exceptions(ശൂന്യം)
{
    PyObject *exmod = PyImport_ImportModule("builtins");
    PyObject *exdict = PyModule_GetDict(exmod);
    p3imp_PyExc_AttributeError = PyDict_GetItemString(exdict, "AttributeError");
    p3imp_PyExc_IndexError = PyDict_GetItemString(exdict, "IndexError");
    p3imp_Py_Exc_താക്കോൽ_Error = PyDict_GetItemString(exdict, "KeyError");
    p3imp_PyExc_KeyboardInterrupt = PyDict_GetItemString(exdict, "KeyboardInterrupt");
    p3imp_PyExc_TypeError = PyDict_GetItemString(exdict, "TypeError");
    p3imp_Py_Exc_മൂല്യം_Error = PyDict_GetItemString(exdict, "ValueError");
    p3imp_PyExc_SystemExit = PyDict_GetItemString(exdict, "SystemExit");
    p3imp_PyExc_RuntimeError = PyDict_GetItemString(exdict, "RuntimeError");
    p3imp_PyExc_ImportError = PyDict_GetItemString(exdict, "ImportError");
    p3imp_PyExc_OverflowError = PyDict_GetItemString(exdict, "OverflowError");
    Py_XINCREF(p3imp_PyExc_AttributeError);
    Py_XINCREF(p3imp_PyExc_IndexError);
    Py_XINCREF(p3imp_Py_Exc_താക്കോൽ_Error);
    Py_XINCREF(p3imp_PyExc_KeyboardInterrupt);
    Py_XINCREF(p3imp_PyExc_TypeError);
    Py_XINCREF(p3imp_Py_Exc_മൂല്യം_Error);
    Py_XINCREF(p3imp_PyExc_SystemExit);
    Py_XINCREF(p3imp_PyExc_RuntimeError);
    Py_XINCREF(p3imp_PyExc_ImportError);
    Py_XINCREF(p3imp_PyExc_OverflowError);
    Py_XDECREF(exmod);
}
#endif // DYNAMIC_PYTHON3

സ്ഥിതം പൂർണ്ണസംഖ്യ py3initialised = 0;
#define PYINITIALISED py3initialised
സ്ഥിതം പൂർണ്ണസംഖ്യ python_end_called = FALSE;

#ifdef USE_LIMITED_API
# define DESTRUCTOR_FINISH(self) \
    ((freefunc)PyType_GetSlot(Py_TYPE((PyObject*)self), Py_tp_free))((PyObject*)self)
#അല്ലെങ്കിൽ
# define DESTRUCTOR_FINISH(self) Py_TYPE(self)->tp_free((PyObject*)self)
#endif

#define WIN_PYTHON_REF(win) win->w_python3_ref
#define BUF_PYTHON_REF(buf) buf->b_python3_ref
#define TAB_PYTHON_REF(tab) tab->tp_python3_ref

    സ്ഥിതം ശൂന്യം
call_PyObject_Free(ശൂന്യം *p)
{
#എങ്കിൽ defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
    _PyObject_DebugFree(p);
#അല്ലെങ്കിൽ
    PyObject_Free(p);
#endif
}

    സ്ഥിതം PyObject *
call_PyType_GenericNew(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    തിരികെ PyType_GenericNew(type,args,kwds);
}

    സ്ഥിതം PyObject *
call_PyType_GenericAlloc(PyTypeObject *type, Py_ssize_t nitems)
{
    തിരികെ PyType_GenericAlloc(type,nitems);
}

സ്ഥിതം PyObject *OutputGetattro(PyObject *, PyObject *);
സ്ഥിതം പൂർണ്ണസംഖ്യ OutputSetattro(PyObject *, PyObject *, PyObject *);
സ്ഥിതം PyObject *BufferGetattro(PyObject *, PyObject *);
സ്ഥിതം പൂർണ്ണസംഖ്യ BufferSetattro(PyObject *, PyObject *, PyObject *);
സ്ഥിതം PyObject *TabPageGetattro(PyObject *, PyObject *);
സ്ഥിതം PyObject *WindowGetattro(PyObject *, PyObject *);
സ്ഥിതം പൂർണ്ണസംഖ്യ WindowSetattro(PyObject *, PyObject *, PyObject *);
സ്ഥിതം PyObject *RangeGetattro(PyObject *, PyObject *);
സ്ഥിതം PyObject *CurrentGetattro(PyObject *, PyObject *);
സ്ഥിതം പൂർണ്ണസംഖ്യ CurrentSetattro(PyObject *, PyObject *, PyObject *);
സ്ഥിതം PyObject *DictionaryGetattro(PyObject *, PyObject *);
സ്ഥിതം പൂർണ്ണസംഖ്യ DictionarySetattro(PyObject *, PyObject *, PyObject *);
സ്ഥിതം PyObject *ListGetattro(PyObject *, PyObject *);
സ്ഥിതം പൂർണ്ണസംഖ്യ ListSetattro(PyObject *, PyObject *, PyObject *);
സ്ഥിതം PyObject *TupleGetattro(PyObject *, PyObject *);
സ്ഥിതം പൂർണ്ണസംഖ്യ TupleSetattro(PyObject *, PyObject *, PyObject *);
സ്ഥിതം PyObject *FunctionGetattro(PyObject *, PyObject *);

സ്ഥിതം ഘടന PyModuleDef vimmodule;

#define PY_CAN_RECURSE

/*
 * Include the code shared with if_python.c
 */
#അന്തര്ഭാവ "if_py_both.h"

#ifdef USE_LIMITED_API
# എങ്കിൽ Py_LIMITED_API >= 0x030A0000
#  define PY_UNICODE_GET_UTF8_CHARS(obj) PyUnicode_AsUTF8AndSize(obj, ശൂന്യ)
# അല്ലെങ്കിൽ
// Python limited API before 3.10 lack easy ways to query the raw UTF-8 chars.
// We need to first convert the string to bytes, and then extract the chars.
// This function is only used for attribute string comparisons, which have
// known short length. As such, just allocate a short static buffer to hold
// the characters instead of having to allocate/deallcoate it.
//
// An alternative would be to convert all attribute string comparisons to use
// PyUnicode_CompareWithASCIIString to skip having to extract the chars.
സ്ഥിതം അക്ഷരം py3_unicode_utf8_chars[20];
സ്ഥിതം അക്ഷരം* PY_UNICODE_GET_UTF8_CHARS(PyObject* str)
{
    py3_unicode_utf8_chars[0] = '\0';
    PyObject* bytes = PyUnicode_AsUTF8String(str);
    എങ്കിൽ (bytes)
    {
	അക്ഷരം *chars;
	Py_ssize_t len;
	എങ്കിൽ (PyBytes_AsStringAndSize(bytes, &chars, &len) != -1)
	{
	    എങ്കിൽ (len < (Py_ssize_t)വലുപ്പം(py3_unicode_utf8_chars))
		// PyBytes_AsStringAndSize guarantees null-termination
		memcpy(py3_unicode_utf8_chars, chars, len + 1);
	}
	Py_DECREF(bytes);
    }
    തിരികെ py3_unicode_utf8_chars;
}
# endif
#അല്ലെങ്കിൽ	// !USE_LIMITED_API
# എങ്കിൽ PY_VERSION_HEX >= 0x030300f0
#  define PY_UNICODE_GET_UTF8_CHARS(obj) PyUnicode_AsUTF8AndSize(obj, ശൂന്യ)
# അല്ലെങ്കിൽ
#  define PY_UNICODE_GET_UTF8_CHARS _PyUnicode_AsString
# endif
#endif

// NOTE: Must always be used at the start of a block, since it declares "name".
#define GET_ATTR_STRING(name, nameobj) \
    അക്ഷരം	*name = ""; \
    എങ്കിൽ (PyUnicode_Check(nameobj)) \
	name = (അക്ഷരം *)PY_UNICODE_GET_UTF8_CHARS(nameobj)

#define PY3OBJ_DELETED(obj) (obj->ob_base.ob_refcnt<=0)

///////////////////////////////////////////////////////
// Internal function prototypes.

സ്ഥിതം PyObject *Py3Init_vim(ശൂന്യം);

///////////////////////////////////////////////////////
// 1. Python interpreter main program.

    ശൂന്യം
python3_end(ശൂന്യം)
{
    സ്ഥിതം പൂർണ്ണസംഖ്യ recurse = 0;

    // If a crash occurs while doing this, don't try again.
    എങ്കിൽ (recurse != 0)
	തിരികെ;

    python_end_called = TRUE;
    ++recurse;

#ifdef DYNAMIC_PYTHON3
    എങ്കിൽ (hinstPy3)
#endif
    എങ്കിൽ (Py_IsInitialized())
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

#എങ്കിൽ defined(DYNAMIC_PYTHON3) && defined(DYNAMIC_PYTHON) && defined(FEAT_PYTHON) && defined(UNIX)
    പൂർണ്ണസംഖ്യ
python3_loaded(ശൂന്യം)
{
    തിരികെ (hinstPy3 != 0);
}
#endif

സ്ഥിതം wchar_t *py_home_buf = ശൂന്യ;

#എങ്കിൽ defined(MSWIN) && (PY_VERSION_HEX >= 0x030500f0)
/*
 * Return TRUE if stdin is readable from Python 3.
 */
    സ്ഥിതം BOOL
is_stdin_readable(ശൂന്യം)
{
    DWORD	    mode, eventnum;
    ഘടന _stat    st;
    പൂർണ്ണസംഖ്യ		    fd = fileno(stdin);
    HANDLE	    hstdin = (HANDLE)_get_osfhandle(fd);

    // Check if stdin is connected to the console.
    എങ്കിൽ (GetConsoleMode(hstdin, &mode))
	// Check if it is opened as input.
	തിരികെ Get_സംഖ്യ_Of_Console_നിവേശം_Events(hstdin, &eventnum);

    തിരികെ _fstat(fd, &st) == 0;
}

// Python 3.5 or later will abort inside Py_Initialize() when stdin has
// been closed (i.e. executed by "vim -").  Reconnect stdin to CONIN$.
// Note that the python DLL is linked to its own stdio DLL which can be
// differ from Vim's stdio.
    സ്ഥിതം ശൂന്യം
reset_stdin(ശൂന്യം)
{
    FILE *(*py__acrt_iob_func)(unsigned) = ശൂന്യ;
    FILE *(*pyfreopen)(സ്ഥിരം അക്ഷരം *, സ്ഥിരം അക്ഷരം *, FILE *) = ശൂന്യ;
    HINSTANCE hinst = get_forwarded_dll(hinstPy3);

    എങ്കിൽ (hinst == ശൂന്യ || is_stdin_readable())
	തിരികെ;

    // Get "freopen" and "stdin" which are used in the python DLL.
    // "stdin" is defined as "__acrt_iob_func(0)" in VC++ 2015 or later.
    py__acrt_iob_func = get_dll_import_func(hinst, "__acrt_iob_func");
    എങ്കിൽ (py__acrt_iob_func)
    {
	HINSTANCE hpystdiodll = find_imported_module_by_funcname(hinst,
							    "__acrt_iob_func");
	എങ്കിൽ (hpystdiodll)
	    pyfreopen = (ശൂന്യം *)GetProcAddress(hpystdiodll, "freopen");
    }

    // Reconnect stdin to CONIN$.
    എങ്കിൽ (pyfreopen != ശൂന്യ)
	pyfreopen("CONIN$", "r", py__acrt_iob_func(0));
    അല്ലെങ്കിൽ
	freopen("CONIN$", "r", stdin);
}
#അല്ലെങ്കിൽ
# define reset_stdin()
#endif

// Python 3.2 or later will abort inside Py_Initialize() when mandatory
// modules cannot be loaded (e.g. 'pythonthreehome' is wrongly set.).
// Install a hook to python dll's exit() and recover from it.
#എങ്കിൽ defined(MSWIN) && (PY_VERSION_HEX >= 0x030200f0)
# define HOOK_EXIT
# include <setjmp.h>

സ്ഥിതം jmp_buf exit_hook_jump_buf;
സ്ഥിതം ശൂന്യം *orig_exit = ശൂന്യ;

/*
 * Function that replaces exit() while calling Py_Initialize().
 */
    സ്ഥിതം ശൂന്യം
hooked_exit(പൂർണ്ണസംഖ്യ ret)
{
    // Recover from exit.
    longjmp(exit_hook_jump_buf, 1);
}

/*
 * Install a hook to python dll's exit().
 */
    സ്ഥിതം ശൂന്യം
hook_py_exit(ശൂന്യം)
{
    HINSTANCE hinst = get_forwarded_dll(hinstPy3);

    എങ്കിൽ (hinst == ശൂന്യ || orig_exit != ശൂന്യ)
	തിരികെ;

    orig_exit = hook_dll_import_func(hinst, "exit", (ശൂന്യം *)hooked_exit);
}

/*
 * Remove the hook installed by hook_py_exit().
 */
    സ്ഥിതം ശൂന്യം
restore_py_exit(ശൂന്യം)
{
    HINSTANCE hinst = hinstPy3;

    എങ്കിൽ (hinst == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (orig_exit != ശൂന്യ)
	hook_dll_import_func(hinst, "exit", orig_exit);
    orig_exit = ശൂന്യ;
}
#endif

    സ്ഥിതം പൂർണ്ണസംഖ്യ
Python3_Init(ശൂന്യം)
{
    എങ്കിൽ (!py3initialised)
    {
#ifdef DYNAMIC_PYTHON3
	എങ്കിൽ (!python3_enabled(TRUE))
	{
	    emsg(_(e_sorry_this_command_is_disabled_python_library_could_not_be_found));
	    പോകുക fail;
	}
#endif
	// Python 3.13 introduced a really useful feature: colorized exceptions.
	// This is great if you're reading them from the terminal, but useless
	// and broken everywhere else (such as in log files, or text editors).
	// Opt out, forcefully.
	vim_setenv((char_u*)"PYTHON_COLORS", (char_u*)"0");

	init_structs();

	എങ്കിൽ (*p_py3home != NUL)
	{
	    size_t len = mbstowcs(ശൂന്യ, (അക്ഷരം *)p_py3home, 0) + 1;

	    // The string must not change later, make a copy in static memory.
	    py_home_buf = ALLOC_MULT(wchar_t, len);
	    എങ്കിൽ (py_home_buf != ശൂന്യ && mbstowcs(
			    py_home_buf, (അക്ഷരം *)p_py3home, len) != (size_t)-1)
		Py_SetPythonHome(py_home_buf);
	}
#ifdef PYTHON3_HOME
	അല്ലെങ്കിൽ എങ്കിൽ (mch_getenv((char_u *)"PYTHONHOME") == ശൂന്യ)
	    Py_SetPythonHome(PYTHON3_HOME);
#endif

	PyImport_AppendInittab("vim", Py3Init_vim);

#എങ്കിൽ !defined(DYNAMIC_PYTHON3) && defined(MSWIN)
	hinstPy3 = GetModuleHandle(PYTHON3_DLL);
#endif
	reset_stdin();

#ifdef HOOK_EXIT
	// Catch exit() called in Py_Initialize().
	hook_py_exit();
	എങ്കിൽ (setjmp(exit_hook_jump_buf) == 0)
	{
	    Py_Initialize();
	    restore_py_exit();
	}
	അല്ലെങ്കിൽ
	{
	    // exit() was called in Py_Initialize().
	    restore_py_exit();
	    emsg(_(e_critical_error_in_python3_initialization_check_your_installation));
	    പോകുക fail;
	}
#അല്ലെങ്കിൽ
	Py_Initialize();
#endif

#എങ്കിൽ PY_VERSION_HEX < 0x03090000
	// Initialise threads.  This is deprecated since Python 3.9.
	PyEval_InitThreads();
#endif
#ifdef DYNAMIC_PYTHON3
	get_py3_exceptions();
#endif

	എങ്കിൽ (PythonIO_Init_io())
	    പോകുക fail;

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

    തിരികെ 0;

fail:
    // We call PythonIO_Flush() here to print any Python errors.
    // This is OK, as it is possible to call this function even
    // if PythonIO_Init_io() has not completed successfully (it will
    // not do anything in this case).
    PythonIO_Flush();
    തിരികെ -1;
}

/*
 * External interface
 */
    സ്ഥിതം ശൂന്യം
DoPyCommand(സ്ഥിരം അക്ഷരം *cmd,
	    dict_T* locals,
	    rangeinitializer init_range,
	    runner run,
	    ശൂന്യം *arg)
{
#എങ്കിൽ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    അക്ഷരം		*saved_locale;
#endif
    PyObject		*cmdstr;
    PyObject		*cmdbytes;
    PyGILState_STATE	pygilstate;

    എങ്കിൽ (python_end_called)
	പോകുക theend;

    എങ്കിൽ (Python3_Init())
	പോകുക theend;

    init_range(arg);

    Python_Release_Vim();	    // leave Vim

#എങ്കിൽ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    // Python only works properly when the LC_NUMERIC locale is "C".
    saved_locale = setlocale(LC_NUMERIC, ശൂന്യ);
    എങ്കിൽ (saved_locale == ശൂന്യ || STRCMP(saved_locale, "C") == 0)
	saved_locale = ശൂന്യ;
    അല്ലെങ്കിൽ
    {
	// Need to make a copy, value may change when setting new locale.
	saved_locale = (അക്ഷരം *)vim_strsave((char_u *)saved_locale);
	(ശൂന്യം)setlocale(LC_NUMERIC, "C");
    }
#endif

    pygilstate = PyGILState_Ensure();

    // PyRun_SimpleString expects a UTF-8 string. Wrong encoding may cause
    // SyntaxError (unicode error).
    cmdstr = PyUnicode_Decode(cmd, strlen(cmd),
					(അക്ഷരം *)ENC_OPT, ERRORS_DECODE_ARG);
    cmdbytes = PyUnicode_AsEncodedString(cmdstr, "utf-8", ERRORS_ENCODE_ARG);
    Py_XDECREF(cmdstr);

    run(PyBytes_AsString(cmdbytes), locals, arg, &pygilstate);
    Py_XDECREF(cmdbytes);

    PyGILState_Release(pygilstate);

#എങ്കിൽ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    എങ്കിൽ (saved_locale != ശൂന്യ)
    {
	(ശൂന്യം)setlocale(LC_NUMERIC, saved_locale);
	vim_free(saved_locale);
    }
#endif

    Python_Lock_Vim();		    // enter Vim
    PythonIO_Flush();

theend:
    തിരികെ;	    // keeps lint happy
}

/*
 * ":py3"
 */
    ശൂന്യം
ex_py3(exarg_T *eap)
{
    char_u *script;

    script = script_get(eap, eap->arg);
    എങ്കിൽ (!eap->skip)
    {
	എങ്കിൽ (p_pyx == 0)
	    p_pyx = 3;

	DoPyCommand(script == ശൂന്യ ? (അക്ഷരം *) eap->arg : (അക്ഷരം *) script,
		ശൂന്യ,
		init_range_cmd,
		(runner) run_cmd,
		(ശൂന്യം *) eap);
    }
    vim_free(script);
}

#define BUFFER_SIZE 2048

/*
 * ":py3file"
 */
    ശൂന്യം
ex_py3file(exarg_T *eap)
{
    സ്ഥിതം അക്ഷരം buffer[BUFFER_SIZE];
    സ്ഥിരം അക്ഷരം *file;
    അക്ഷരം *p;
    പൂർണ്ണസംഖ്യ i;

    എങ്കിൽ (p_pyx == 0)
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

    വേണ്ടി (i=0; i<2; ++i)
    {
	file = (അക്ഷരം *)eap->arg;
	ആകുമ്പോൾ (*file && p < buffer + (BUFFER_SIZE - 3))
	{
	    എങ്കിൽ (*file == '\\' || *file == '\'')
		*p++ = '\\';
	    *p++ = *file++;
	}
	// If we didn't finish the file name, we hit a buffer overflow
	എങ്കിൽ (*file != '\0')
	    തിരികെ;
	എങ്കിൽ (i==0)
	{
	    strcpy(p,"','rb').read(),'");
	    p += 16;
	}
	അല്ലെങ്കിൽ
	{
	    strcpy(p,"','exec'))");
	    p += 10;
	}
    }


    // Execute the file
    DoPyCommand(buffer,
	    ശൂന്യ,
	    init_range_cmd,
	    (runner) run_cmd,
	    (ശൂന്യം *) eap);
}

    ശൂന്യം
ex_py3do(exarg_T *eap)
{
    എങ്കിൽ (p_pyx == 0)
	p_pyx = 3;

    DoPyCommand((അക്ഷരം *)eap->arg,
	    ശൂന്യ,
	    init_range_cmd,
	    (runner)run_do,
	    (ശൂന്യം *)eap);
}

///////////////////////////////////////////////////////
// 2. Python output stream: writes output via [e]msg().

// Implementation functions

    സ്ഥിതം PyObject *
OutputGetattro(PyObject *self, PyObject *nameobj)
{
    GET_ATTR_STRING(name, nameobj);

    എങ്കിൽ (strcmp(name, "softspace") == 0)
	തിരികെ PyLong_FromLong(((OutputObject *)(self))->softspace);
    അല്ലെങ്കിൽ എങ്കിൽ (strcmp(name, "errors") == 0)
	തിരികെ PyString_FromString("strict");
    അല്ലെങ്കിൽ എങ്കിൽ (strcmp(name, "encoding") == 0)
	തിരികെ PyString_FromString(ENC_OPT);

    തിരികെ PyObject_GenericGetAttr(self, nameobj);
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
OutputSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);

    തിരികെ OutputSetattr(self, name, val);
}

///////////////////////////////////////////////////////
// 3. Implementation of the Vim module for Python

// Window type - Implementation functions
// --------------------------------------

#define WindowType_Check(obj) ((obj)->ob_base.ob_type == &WindowType)

// Buffer type - Implementation functions
// --------------------------------------

#define BufferType_Check(obj) ((obj)->ob_base.ob_type == &BufferType)

സ്ഥിതം PyObject* BufferSubscript(PyObject *self, PyObject *idx);
സ്ഥിതം പൂർണ്ണസംഖ്യ BufferAsSubscript(PyObject *self, PyObject *idx, PyObject *val);

// Line range type - Implementation functions
// --------------------------------------

#define RangeType_Check(obj) ((obj)->ob_base.ob_type == &RangeType)

സ്ഥിതം PyObject* RangeSubscript(PyObject *self, PyObject *idx);
സ്ഥിതം പൂർണ്ണസംഖ്യ RangeAsItem(PyObject *, Py_ssize_t, PyObject *);
സ്ഥിതം പൂർണ്ണസംഖ്യ RangeAsSubscript(PyObject *self, PyObject *idx, PyObject *val);

// Current objects type - Implementation functions
// -----------------------------------------------

സ്ഥിതം PySequenceMethods BufferAsSeq = {
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

സ്ഥിതം PyMappingMethods BufferAsMapping = {
    /* mp_length	*/ (lenfunc)BufferLength,
    /* mp_subscript     */ (binaryfunc)BufferSubscript,
    /* mp_ass_subscript */ (objobjargproc)BufferAsSubscript,
};


// Buffer object

    സ്ഥിതം PyObject *
BufferGetattro(PyObject *self, PyObject *nameobj)
{
    PyObject *r;

    GET_ATTR_STRING(name, nameobj);

    എങ്കിൽ ((r = BufferAttrValid((BufferObject *)(self), name)))
	തിരികെ r;

    എങ്കിൽ (CheckBuffer((BufferObject *)(self)))
	തിരികെ ശൂന്യ;

    r = BufferAttr((BufferObject *)(self), name);
    എങ്കിൽ (r || PyErr_Occurred())
	തിരികെ r;
    അല്ലെങ്കിൽ
	തിരികെ PyObject_GenericGetAttr(self, nameobj);
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
BufferSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);

    തിരികെ BufferSetattr(self, name, val);
}

//////////////////

    സ്ഥിതം PyObject *
BufferSubscript(PyObject *self, PyObject* idx)
{
    എങ്കിൽ (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	തിരികെ BufferItem((BufferObject *)(self), _idx);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	എങ്കിൽ (CheckBuffer((BufferObject *) self))
	    തിരികെ ശൂന്യ;

	എങ്കിൽ (PySlice_GetIndicesEx((PySliceObject_T *)idx,
	      (Py_ssize_t)((BufferObject *)(self))->buf->b_ml.ml_വരി_എണ്ണം,
	      &start, &stop,
	      &step, &slicelen) < 0)
	    തിരികെ ശൂന്യ;
	തിരികെ BufferSlice((BufferObject *)(self), start, stop);
    }
    അല്ലെങ്കിൽ
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	തിരികെ ശൂന്യ;
    }
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
BufferAsSubscript(PyObject *self, PyObject* idx, PyObject* val)
{
    എങ്കിൽ (PyLong_Check(idx))
    {
	long n = PyLong_AsLong(idx);

	എങ്കിൽ (CheckBuffer((BufferObject *) self))
	    തിരികെ -1;

	തിരികെ RBAsItem((BufferObject *)(self), n, val, 1,
		    (Py_ssize_t)((BufferObject *)(self))->buf->b_ml.ml_വരി_എണ്ണം,
		    ശൂന്യ);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	എങ്കിൽ (CheckBuffer((BufferObject *) self))
	    തിരികെ -1;

	എങ്കിൽ (PySlice_GetIndicesEx((PySliceObject_T *)idx,
	      (Py_ssize_t)((BufferObject *)(self))->buf->b_ml.ml_വരി_എണ്ണം,
	      &start, &stop,
	      &step, &slicelen) < 0)
	    തിരികെ -1;
	തിരികെ RBAsSlice((BufferObject *)(self), start, stop, val, 1,
			  (PyInt)((BufferObject *)(self))->buf->b_ml.ml_വരി_എണ്ണം,
			  ശൂന്യ);
    }
    അല്ലെങ്കിൽ
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	തിരികെ -1;
    }
}

സ്ഥിതം PySequenceMethods RangeAsSeq = {
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

സ്ഥിതം PyMappingMethods RangeAsMapping = {
    /* mp_length	*/ (lenfunc)RangeLength,
    /* mp_subscript     */ (binaryfunc)RangeSubscript,
    /* mp_ass_subscript */ (objobjargproc)RangeAsSubscript,
};

// Line range object - Implementation

    സ്ഥിതം PyObject *
RangeGetattro(PyObject *self, PyObject *nameobj)
{
    GET_ATTR_STRING(name, nameobj);

    എങ്കിൽ (strcmp(name, "start") == 0)
	തിരികെ Py_Build_മൂല്യം("n", ((RangeObject *)(self))->start - 1);
    അല്ലെങ്കിൽ എങ്കിൽ (strcmp(name, "end") == 0)
	തിരികെ Py_Build_മൂല്യം("n", ((RangeObject *)(self))->end - 1);
    അല്ലെങ്കിൽ
	തിരികെ PyObject_GenericGetAttr(self, nameobj);
}

////////////////

    സ്ഥിതം പൂർണ്ണസംഖ്യ
RangeAsItem(PyObject *self, Py_ssize_t n, PyObject *val)
{
    തിരികെ RBAsItem(((RangeObject *)(self))->buf, n, val,
		    ((RangeObject *)(self))->start,
		    ((RangeObject *)(self))->end,
		    &((RangeObject *)(self))->end);
}

    സ്ഥിതം Py_ssize_t
RangeAsSlice(PyObject *self, Py_ssize_t lo, Py_ssize_t hi, PyObject *val)
{
    തിരികെ RBAsSlice(((RangeObject *)(self))->buf, lo, hi, val,
		    ((RangeObject *)(self))->start,
		    ((RangeObject *)(self))->end,
		    &((RangeObject *)(self))->end);
}

    സ്ഥിതം PyObject *
RangeSubscript(PyObject *self, PyObject* idx)
{
    എങ്കിൽ (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	തിരികെ RangeItem((RangeObject *)(self), _idx);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	എങ്കിൽ (PySlice_GetIndicesEx((PySliceObject_T *)idx,
		((RangeObject *)(self))->end-((RangeObject *)(self))->start+1,
		&start, &stop,
		&step, &slicelen) < 0)
	    തിരികെ ശൂന്യ;
	തിരികെ RangeSlice((RangeObject *)(self), start, stop);
    }
    അല്ലെങ്കിൽ
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	തിരികെ ശൂന്യ;
    }
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
RangeAsSubscript(PyObject *self, PyObject *idx, PyObject *val)
{
    എങ്കിൽ (PyLong_Check(idx))
    {
	long n = PyLong_AsLong(idx);
	തിരികെ RangeAsItem(self, n, val);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	എങ്കിൽ (PySlice_GetIndicesEx((PySliceObject_T *)idx,
		((RangeObject *)(self))->end-((RangeObject *)(self))->start+1,
		&start, &stop,
		&step, &slicelen) < 0)
	    തിരികെ -1;
	തിരികെ RangeAsSlice(self, start, stop, val);
    }
    അല്ലെങ്കിൽ
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	തിരികെ -1;
    }
}

// TabPage object - Implementation

    സ്ഥിതം PyObject *
TabPageGetattro(PyObject *self, PyObject *nameobj)
{
    PyObject *r;

    GET_ATTR_STRING(name, nameobj);

    എങ്കിൽ ((r = TabPageAttrValid((TabPageObject *)(self), name)))
	തിരികെ r;

    എങ്കിൽ (CheckTabPage((TabPageObject *)(self)))
	തിരികെ ശൂന്യ;

    r = TabPageAttr((TabPageObject *)(self), name);
    എങ്കിൽ (r || PyErr_Occurred())
	തിരികെ r;
    അല്ലെങ്കിൽ
	തിരികെ PyObject_GenericGetAttr(self, nameobj);
}

// Window object - Implementation

    സ്ഥിതം PyObject *
WindowGetattro(PyObject *self, PyObject *nameobj)
{
    PyObject *r;

    GET_ATTR_STRING(name, nameobj);

    എങ്കിൽ ((r = WindowAttrValid((WindowObject *)(self), name)))
	തിരികെ r;

    എങ്കിൽ (CheckWindow((WindowObject *)(self)))
	തിരികെ ശൂന്യ;

    r = WindowAttr((WindowObject *)(self), name);
    എങ്കിൽ (r || PyErr_Occurred())
	തിരികെ r;
    അല്ലെങ്കിൽ
	തിരികെ PyObject_GenericGetAttr(self, nameobj);
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
WindowSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);

    തിരികെ WindowSetattr(self, name, val);
}

// Tab page list object - Definitions

സ്ഥിതം PySequenceMethods TabListAsSeq = {
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

സ്ഥിതം PySequenceMethods WinListAsSeq = {
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
    സ്ഥിതം PyObject *
CurrentGetattro(PyObject *self, PyObject *nameobj)
{
    PyObject	*r;
    GET_ATTR_STRING(name, nameobj);
    എങ്കിൽ (!(r = CurrentGetattr(self, name)))
	തിരികെ PyObject_GenericGetAttr(self, nameobj);
    തിരികെ r;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
CurrentSetattro(PyObject *self, PyObject *nameobj, PyObject *മൂല്യം)
{
    GET_ATTR_STRING(name, nameobj);
    തിരികെ CurrentSetattr(self, name, മൂല്യം);
}

// Dictionary object - Definitions

    സ്ഥിതം PyObject *
DictionaryGetattro(PyObject *self, PyObject *nameobj)
{
    DictionaryObject	*this = ((DictionaryObject *) (self));

    GET_ATTR_STRING(name, nameobj);

    എങ്കിൽ (strcmp(name, "locked") == 0)
	തിരികെ PyLong_FromLong(this->dict->dv_lock);
    അല്ലെങ്കിൽ എങ്കിൽ (strcmp(name, "scope") == 0)
	തിരികെ PyLong_FromLong(this->dict->dv_scope);

    തിരികെ PyObject_GenericGetAttr(self, nameobj);
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
DictionarySetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);
    തിരികെ DictionarySetattr(self, name, val);
}

// List object - Definitions

    സ്ഥിതം PyObject *
ListGetattro(PyObject *self, PyObject *nameobj)
{
    GET_ATTR_STRING(name, nameobj);

    എങ്കിൽ (strcmp(name, "locked") == 0)
	തിരികെ PyLong_FromLong(((ListObject *) (self))->list->lv_lock);

    തിരികെ PyObject_GenericGetAttr(self, nameobj);
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
ListSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);
    തിരികെ ListSetattr(self, name, val);
}

// Tuple object - Definitions

    സ്ഥിതം PyObject *
TupleGetattro(PyObject *self, PyObject *nameobj)
{
    GET_ATTR_STRING(name, nameobj);

    എങ്കിൽ (strcmp(name, "locked") == 0)
	തിരികെ PyLong_FromLong(((TupleObject *) (self))->tuple->tv_lock);

    തിരികെ PyObject_GenericGetAttr(self, nameobj);
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
TupleSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);
    തിരികെ TupleSetattr(self, name, val);
}

// Function object - Definitions

    സ്ഥിതം PyObject *
FunctionGetattro(PyObject *self, PyObject *nameobj)
{
    PyObject		*r;
    FunctionObject	*this = (FunctionObject *)(self);

    GET_ATTR_STRING(name, nameobj);

    r = FunctionAttr(this, name);
    എങ്കിൽ (r || PyErr_Occurred())
	തിരികെ r;
    അല്ലെങ്കിൽ
	തിരികെ PyObject_GenericGetAttr(self, nameobj);
}

// External interface

    ശൂന്യം
python3_buffer_free(buf_T *buf)
{
    BufferObject *bp = BUF_PYTHON_REF(buf);
    എങ്കിൽ (bp == ശൂന്യ)
	തിരികെ;
    bp->buf = INVALID_BUFFER_മൂല്യം;
    BUF_PYTHON_REF(buf) = ശൂന്യ;
}

    ശൂന്യം
python3_window_free(win_T *win)
{
    WindowObject *wp = WIN_PYTHON_REF(win);
    എങ്കിൽ (wp == ശൂന്യ)
	തിരികെ;
    wp->win = INVALID_WINDOW_മൂല്യം;
    WIN_PYTHON_REF(win) = ശൂന്യ;
}

    ശൂന്യം
python3_tabpage_free(tabpage_T *tab)
{
    TabPageObject *tp = TAB_PYTHON_REF(tab);
    എങ്കിൽ (tp == ശൂന്യ)
	തിരികെ;
    tp->tab = INVALID_TABPAGE_മൂല്യം;
    TAB_PYTHON_REF(tab) = ശൂന്യ;
}

    സ്ഥിതം PyObject *
Py3Init_vim(ശൂന്യം)
{
    // The special value is removed from sys.path in Python3_Init().
    സ്ഥിതം wchar_t *(argv[2]) = {L"/must>not&exist/foo", ശൂന്യ};

    എങ്കിൽ (init_types())
	തിരികെ ശൂന്യ;

    // Set sys.argv[] to avoid a crash in warn().
    PySys_SetArgv(1, argv);

    എങ്കിൽ ((vim_module = PyModule_Create(&vimmodule)) == ശൂന്യ)
	തിരികെ ശൂന്യ;

    എങ്കിൽ (populate_module(vim_module))
	തിരികെ ശൂന്യ;

    എങ്കിൽ (init_sys_path())
	തിരികെ ശൂന്യ;

    തിരികെ vim_module;
}

//////////////////////////////////////////////////////////////////////////
// 4. Utility functions for handling the interface between Vim and Python.

/*
 * Convert a Vim line into a Python string.
 * All internal newlines are replaced by null characters.
 *
 * On errors, the Python exception data is set, and NULL is returned.
 */
    സ്ഥിതം PyObject *
വരി_To_String(സ്ഥിരം അക്ഷരം *str)
{
    PyObject *result;
    Py_ssize_t len = strlen(str);
    അക്ഷരം *tmp, *p;

    tmp = alloc(len + 1);
    p = tmp;
    എങ്കിൽ (p == ശൂന്യ)
    {
	PyErr_NoMemory();
	തിരികെ ശൂന്യ;
    }

    ആകുമ്പോൾ (*str)
    {
	എങ്കിൽ (*str == '\n')
	    *p = '\0';
	അല്ലെങ്കിൽ
	    *p = *str;

	++p;
	++str;
    }
    *p = '\0';

    result = PyUnicode_Decode(tmp, len, (അക്ഷരം *)ENC_OPT, ERRORS_DECODE_ARG);

    vim_free(tmp);
    തിരികെ result;
}

    ശൂന്യം
do_py3eval(char_u *str, dict_T *locals, typval_T *rettv)
{
    DoPyCommand((അക്ഷരം *) str,
	    locals,
	    init_range_eval,
	    (runner) run_eval,
	    (ശൂന്യം *) rettv);
    എങ്കിൽ (rettv->v_type == VAR_UNKNOWN)
    {
	rettv->v_type = VAR_സംഖ്യ;
	rettv->vval.v_സംഖ്യ = 0;
    }
}

    പൂർണ്ണസംഖ്യ
set_ref_in_python3(പൂർണ്ണസംഖ്യ copyID)
{
    തിരികെ set_ref_in_py(copyID);
}

    പൂർണ്ണസംഖ്യ
python3_version(ശൂന്യം)
{
#ifdef USE_LIMITED_API
    തിരികെ Py_LIMITED_API;
#അല്ലെങ്കിൽ
    തിരികെ PY_VERSION_HEX;
#endif
}
