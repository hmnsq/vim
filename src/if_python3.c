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

#অন্তর্ভাৱ "vim.h"

#অন্তর্ভাৱ <limits.h>

#যদি defined(MSWIN) && defined(HAVE_FCNTL_H)
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

#অন্তর্ভাৱ <Python.h>

#undef main // Defined in python.h - aargh
#undef HAVE_FCNTL_H // Clash with os_win32.h

// The "surrogateescape" error handler is new in Python 3.1
#যদি PY_VERSION_HEX >= 0x030100f0
# define CODEC_ERROR_HANDLER "surrogateescape"
#অন্যথা
# define CODEC_ERROR_HANDLER রিক্ত
#endif

// Suppress Python 3.11 depreciations to see useful warnings
#ifdef __GNUC__
# pragma GCC diagnostic ঠেলো
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
#যদি PY_VERSION_HEX >= 0x030200f0
প্রকারনির্দেশ PyObject PySliceObject_T;
#অন্যথা
প্রকারনির্দেশ PySliceObject PySliceObject_T;
#endif

#ifndef MSWIN
# define HINSTANCE শূন্য *
#endif
#যদি defined(DYNAMIC_PYTHON3) || defined(MSWIN)
স্থির HINSTANCE hinstPy3 = 0; // Instance of python.dll
#endif

#যদি defined(DYNAMIC_PYTHON3)

# ifdef MSWIN
#  define load_dll vimLoadLib
#  define close_dll FreeLibrary
#  define প্ৰতীক_from_dll GetProcAddress
#  define load_dll_error GetWin32Error
# অন্যথা
#  include <dlfcn.h>
#  define FARPROC শূন্য*
#  যদি defined(PY_NO_RTLD_GLOBAL) && defined(PY3_NO_RTLD_GLOBAL)
#   define load_dll(n) dlopen((n), RTLD_LAZY)
#  অন্যথা
#   define load_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#  endif
#  define close_dll dlclose
#  define প্ৰতীক_from_dll dlsym
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
# define Py_List_ঢোকাও py3_Py_List_ঢোকাও
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
# যদি PY_VERSION_HEX >= 0x030601f0
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
# ifndef Py_Mapping_চাবিগুলি
#  define Py_Mapping_চাবিগুলি py3_Py_Mapping_চাবিগুলি
# endif
# যদি (defined(USE_LIMITED_API) && Py_LIMITED_API >= 0x03080000) || \
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
# যদি defined(USE_LIMITED_API) || PY_VERSION_HEX >= 0x03080000
#  define Py_IncRef py3_Py_IncRef
#  define Py_DecRef py3_Py_DecRef
# endif
# ifdef USE_LIMITED_API
#  define Py_CompileString py3_Py_CompileString
#  define PyEval_EvalCode py3_PyEval_EvalCode
# অন্যথা
#  undef PyRun_SimpleString
#  define PyRun_SimpleString py3_PyRun_SimpleString
#  undef PyRun_String
#  define PyRun_String py3_PyRun_String
# endif
# define PyObject_GetAttrString py3_PyObject_GetAttrString
# define PyObject_HasAttrString py3_PyObject_HasAttrString
# define PyObject_SetAttrString py3_PyObject_SetAttrString
# define PyObject_CallFunctionObjArgs py3_PyObject_CallFunctionObjArgs
# যদি PY_VERSION_HEX >= 0x030d0000
#  define PyObject_CallFunction py3_PyObject_CallFunction
# অন্যথা
#  define _PyObject_CallFunction_SizeT py3__PyObject_CallFunction_SizeT
# endif
# define PyObject_Call py3_PyObject_Call
# define PyEval_GetLocals py3_PyEval_GetLocals
# define PyEval_GetGlobals py3_PyEval_GetGlobals
# define PySys_SetObject py3_PySys_SetObject
# define PySys_GetObject py3_PySys_GetObject
# define PySys_SetArgv py3_PySys_SetArgv
# define PyType_Ready py3_PyType_Ready
# যদি PY_VERSION_HEX >= 0x03040000
#  define PyType_GetFlags py3_PyType_GetFlags
# endif
# undef Py_Build_মান
# define Py_Build_মান py3_Py_Build_মান
# define Py_SetPythonHome py3_Py_SetPythonHome
# define Py_Initialize py3_Py_Initialize
# define Py_Finalize py3_Py_Finalize
# define Py_IsInitialized py3_Py_IsInitialized
# define _Py_NoneStruct (*py3__Py_NoneStruct)
# define _Py_FalseStruct (*py3__Py_FalseStruct)
# define _Py_TrueStruct (*py3__Py_TrueStruct)
# যদি !defined(USE_LIMITED_API) && PY_VERSION_HEX < 0x030D0000
// Private symbol that used to be required as part of PyIter_Check.
#  define _PyObject_NextNotImplemented (*py3__PyObject_NextNotImplemented)
# endif
# define PyModule_AddObject py3_PyModule_AddObject
# define PyImport_AppendInittab py3_PyImport_AppendInittab
# define PyImport_AddModule py3_PyImport_AddModule
# ifdef USE_LIMITED_API
#  যদি Py_LIMITED_API >= 0x030a0000
#   define PyUnicode_AsUTF8AndSize py3_PyUnicode_AsUTF8AndSize
#  endif
# অন্যথা
#  যদি PY_VERSION_HEX >= 0x030300f0
#   define PyUnicode_AsUTF8AndSize py3_PyUnicode_AsUTF8AndSize
#  অন্যথা
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
# define Py_সংখ্যা_Check (*py3_Py_সংখ্যা_Check)
# define Py_সংখ্যা_Long (*py3_Py_সংখ্যা_Long)
# define PyBool_Type (*py3_PyBool_Type)
# define PyErr_NewException py3_PyErr_NewException
# ifdef Py_DEBUG
#  define _Py_NegativeRefcount py3__Py_NegativeRefcount
#  define _Py_RefTotal (*py3__Py_RefTotal)
#  define PyModule_Create2TraceRefs py3_PyModule_Create2TraceRefs
# অন্যথা
#  define PyModule_Create2 py3_PyModule_Create2
# endif
# যদি defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
#  define _PyObject_DebugMalloc py3__PyObject_DebugMalloc
#  define _PyObject_DebugFree py3__PyObject_DebugFree
# অন্যথা
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
#  অন্যথা
#   define PyUnicodeUCS2_FromFormat py3_PyUnicodeUCS2_FromFormat
#  endif
# অন্যথা
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

# যদি defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
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
স্থির পূর্ণাঙ্ক (*py3_PySys_SetArgv)(পূর্ণাঙ্ক, wchar_t **);
স্থির শূন্য (*py3_Py_SetPythonHome)(wchar_t *home);
স্থির শূন্য (*py3_Py_Initialize)(শূন্য);
স্থির PyObject* (*py3_PyList_New)(Py_ssize_t size);
স্থির PyGILState_STATE (*py3_PyGILState_Ensure)(শূন্য);
স্থির শূন্য (*py3_PyGILState_Release)(PyGILState_STATE);
স্থির পূর্ণাঙ্ক (*py3_PySys_SetObject)(ৱর্ণ *, PyObject *);
স্থির PyObject* (*py3_PySys_GetObject)(ৱর্ণ *);
স্থির পূর্ণাঙ্ক (*py3_PyList_Append)(PyObject *, PyObject *);
স্থির পূর্ণাঙ্ক (*py3_Py_List_ঢোকাও)(PyObject *, পূর্ণাঙ্ক, PyObject *);
স্থির Py_ssize_t (*py3_PyList_Size)(PyObject *);
স্থির PyObject* (*py3_PyTuple_New)(Py_ssize_t size);
স্থির পূর্ণাঙ্ক (*py3_PyTuple_SetItem)(PyObject *, Py_ssize_t, PyObject *);
স্থির Py_ssize_t (*py3_PyTuple_Size)(PyObject *);
স্থির পূর্ণাঙ্ক (*py3_PySequence_Check)(PyObject *);
স্থির Py_ssize_t (*py3_PySequence_Size)(PyObject *);
স্থির PyObject* (*py3_PySequence_GetItem)(PyObject *, Py_ssize_t);
স্থির PyObject* (*py3_PySequence_Fast)(PyObject *, ধ্রুৱ ৱর্ণ *);
স্থির PyObject* (*py3_PyTuple_GetItem)(PyObject *, Py_ssize_t);
স্থির পূর্ণাঙ্ক (*py3_PyMapping_Check)(PyObject *);
স্থির PyObject* (*py3_Py_Mapping_চাবিগুলি)(PyObject *);
# যদি PY_VERSION_HEX >= 0x030601f0
স্থির পূর্ণাঙ্ক (*py3_PySlice_AdjustIndices)(Py_ssize_t length,
		     Py_ssize_t *start, Py_ssize_t *stop, Py_ssize_t step);
স্থির পূর্ণাঙ্ক (*py3_PySlice_Unpack)(PyObject *slice,
		     Py_ssize_t *start, Py_ssize_t *stop, Py_ssize_t *step);
# endif
স্থির পূর্ণাঙ্ক (*py3_PySlice_GetIndicesEx)(PySliceObject_T *r, Py_ssize_t length,
		     Py_ssize_t *start, Py_ssize_t *stop, Py_ssize_t *step,
		     Py_ssize_t *slicelen);
স্থির PyObject* (*py3_PyErr_NoMemory)(শূন্য);
স্থির শূন্য (*py3_Py_Finalize)(শূন্য);
স্থির শূন্য (*py3_PyErr_SetString)(PyObject *, ধ্রুৱ ৱর্ণ *);
স্থির শূন্য (*py3_PyErr_SetObject)(PyObject *, PyObject *);
স্থির পূর্ণাঙ্ক (*py3_PyErr_ExceptionMatches)(PyObject *);
# যদি defined(USE_LIMITED_API) || PY_VERSION_HEX >= 0x03080000
স্থির শূন্য (*py3_Py_IncRef)(PyObject *);
স্থির শূন্য (*py3_Py_DecRef)(PyObject *);
# endif
# ifdef USE_LIMITED_API
স্থির PyObject* (*py3_Py_CompileString)(ধ্রুৱ ৱর্ণ *, ধ্রুৱ ৱর্ণ *, পূর্ণাঙ্ক);
স্থির PyObject* (*py3_PyEval_EvalCode)(PyObject *co, PyObject *globals, PyObject *locals);
# অন্যথা
স্থির পূর্ণাঙ্ক (*py3_PyRun_SimpleString)(ৱর্ণ *);
স্থির PyObject* (*py3_PyRun_String)(ৱর্ণ *, পূর্ণাঙ্ক, PyObject *, PyObject *);
# endif
স্থির PyObject* (*py3_PyObject_GetAttrString)(PyObject *, ধ্রুৱ ৱর্ণ *);
স্থির পূর্ণাঙ্ক (*py3_PyObject_HasAttrString)(PyObject *, ধ্রুৱ ৱর্ণ *);
স্থির পূর্ণাঙ্ক (*py3_PyObject_SetAttrString)(PyObject *, ধ্রুৱ ৱর্ণ *, PyObject *);
স্থির PyObject* (*py3_PyObject_CallFunctionObjArgs)(PyObject *, ...);
# যদি PY_VERSION_HEX >= 0x030d0000
স্থির PyObject* (*py3_PyObject_CallFunction)(PyObject *, ৱর্ণ *, ...);
# অন্যথা
স্থির PyObject* (*py3__PyObject_CallFunction_SizeT)(PyObject *, ৱর্ণ *, ...);
# endif
স্থির PyObject* (*py3_PyObject_Call)(PyObject *, PyObject *, PyObject *);
স্থির PyObject* (*py3_PyEval_GetGlobals)(শূন্য);
স্থির PyObject* (*py3_PyEval_GetLocals)(শূন্য);
স্থির PyObject* (*py3_PyList_GetItem)(PyObject *, Py_ssize_t);
স্থির PyObject* (*py3_PyImport_ImportModule)(ধ্রুৱ ৱর্ণ *);
স্থির PyObject* (*py3_PyImport_AddModule)(ধ্রুৱ ৱর্ণ *);
স্থির পূর্ণাঙ্ক (*py3_PyErr_BadArgument)(শূন্য);
স্থির PyObject* (*py3_PyErr_Occurred)(শূন্য);
স্থির PyObject* (*py3_PyModule_GetDict)(PyObject *);
স্থির পূর্ণাঙ্ক (*py3_PyList_SetItem)(PyObject *, Py_ssize_t, PyObject *);
স্থির PyObject* (*py3_PyDict_GetItemString)(PyObject *, ধ্রুৱ ৱর্ণ *);
স্থির পূর্ণাঙ্ক (*py3_PyDict_Next)(PyObject *, Py_ssize_t *, PyObject **, PyObject **);
স্থির PyObject* (*py3_PyLong_FromLong)(long);
স্থির PyObject* (*py3_PyDict_New)(শূন্য);
# যদি (defined(USE_LIMITED_API) && Py_LIMITED_API >= 0x03080000) || \
       (!defined(USE_LIMITED_API) && PY_VERSION_HEX >= 0x03080000)
স্থির পূর্ণাঙ্ক (*py3_PyIter_Check)(PyObject *o);
# endif
স্থির PyObject* (*py3_PyIter_Next)(PyObject *);
স্থির PyObject* (*py3_PyObject_GetIter)(PyObject *);
স্থির PyObject* (*py3_PyObject_Repr)(PyObject *);
স্থির PyObject* (*py3_PyObject_GetItem)(PyObject *, PyObject *);
স্থির পূর্ণাঙ্ক (*py3_PyObject_IsTrue)(PyObject *);
স্থির PyObject* (*py3_Py_Build_মান)(ৱর্ণ *, ...);
# যদি PY_VERSION_HEX >= 0x03040000
স্থির পূর্ণাঙ্ক (*py3_PyType_GetFlags)(PyTypeObject *o);
# endif
স্থির পূর্ণাঙ্ক (*py3_PyType_Ready)(PyTypeObject *type);
স্থির পূর্ণাঙ্ক (*py3_PyDict_SetItemString)(PyObject *dp, ৱর্ণ *চাবি, PyObject *item);
স্থির PyObject* (*py3_PyUnicode_FromString)(ধ্রুৱ ৱর্ণ *u);
# ifdef Py_UNICODE_USE_UCS_FUNCTIONS
#  ifdef Py_UNICODE_WIDE
স্থির PyObject* (*py3_PyUnicodeUCS4_FromFormat)(ধ্রুৱ ৱর্ণ *u, ...);
#  অন্যথা
স্থির PyObject* (*py3_PyUnicodeUCS2_FromFormat)(ধ্রুৱ ৱর্ণ *u, ...);
#  endif
# অন্যথা
স্থির PyObject* (*py3_PyUnicode_FromFormat)(ধ্রুৱ ৱর্ণ *u, ...);
# endif
স্থির PyObject* (*py3_PyUnicode_Decode)(ধ্রুৱ ৱর্ণ *u, Py_ssize_t size,
	ধ্রুৱ ৱর্ণ *encoding, ধ্রুৱ ৱর্ণ *errors);
স্থির long (*py3_PyLong_AsLong)(PyObject *);
স্থির শূন্য (*py3_PyErr_SetNone)(PyObject *);
স্থির শূন্য (*py3_PyEval_InitThreads)(শূন্য);
স্থির শূন্য(*py3_PyEval_RestoreThread)(PyThreadState *);
স্থির PyThreadState*(*py3_PyEval_SaveThread)(শূন্য);
স্থির পূর্ণাঙ্ক (*py3_PyArg_Parse)(PyObject *, ৱর্ণ *, ...);
স্থির পূর্ণাঙ্ক (*py3_PyArg_ParseTuple)(PyObject *, ৱর্ণ *, ...);
স্থির শূন্য (*py3_PyMem_Free)(শূন্য *);
স্থির শূন্য* (*py3_PyMem_Malloc)(size_t);
স্থির পূর্ণাঙ্ক (*py3_Py_IsInitialized)(শূন্য);
স্থির শূন্য (*py3_PyErr_Clear)(শূন্য);
স্থির PyObject* (*py3_PyErr_Format)(PyObject *, ধ্রুৱ ৱর্ণ *, ...);
স্থির শূন্য (*py3_PyErr_PrintEx)(পূর্ণাঙ্ক);
স্থির PyObject*(*py3__PyObject_Init)(PyObject *, PyTypeObject *);
# যদি !defined(USE_LIMITED_API) && PY_VERSION_HEX < 0x030D0000
স্থির iternextfunc py3__PyObject_NextNotImplemented;
# endif
স্থির PyObject* py3__Py_NoneStruct;
স্থির PyObject* py3__Py_FalseStruct;
স্থির PyObject* py3__Py_TrueStruct;
স্থির পূর্ণাঙ্ক (*py3_PyModule_AddObject)(PyObject *m, ধ্রুৱ ৱর্ণ *name, PyObject *o);
স্থির পূর্ণাঙ্ক (*py3_PyImport_AppendInittab)(ধ্রুৱ ৱর্ণ *name, PyObject* (*initfunc)(শূন্য));
# ifdef USE_LIMITED_API
#  যদি Py_LIMITED_API >= 0x030a0000
স্থির ৱর্ণ* (*py3_PyUnicode_AsUTF8AndSize)(PyObject *unicode, Py_ssize_t *size);
#  endif
# অন্যথা
#  যদি PY_VERSION_HEX >= 0x030300f0
স্থির ৱর্ণ* (*py3_PyUnicode_AsUTF8AndSize)(PyObject *unicode, Py_ssize_t *size);
#  অন্যথা
স্থির ৱর্ণ* (*py3__PyUnicode_AsString)(PyObject *unicode);
#  endif
# endif
স্থির পূর্ণাঙ্ক (*py3_PyUnicode_CompareWithASCIIString)(PyObject *unicode, ধ্রুৱ ৱর্ণ* string);
স্থির PyObject* (*py3_PyUnicode_AsEncodedString)(PyObject *unicode, ধ্রুৱ ৱর্ণ* encoding, ধ্রুৱ ৱর্ণ* errors);
স্থির PyObject* (*py3_PyUnicode_AsUTF8String)(PyObject *unicode);
স্থির ৱর্ণ* (*py3_PyBytes_AsString)(PyObject *bytes);
স্থির পূর্ণাঙ্ক (*py3_PyBytes_AsStringAndSize)(PyObject *bytes, ৱর্ণ **buffer, Py_ssize_t *length);
স্থির PyObject* (*py3_PyBytes_FromString)(ৱর্ণ *str);
স্থির PyObject* (*py3_PyBytes_FromStringAndSize)(ৱর্ণ *str, Py_ssize_t length);
# যদি PY_VERSION_HEX >= 0x030900b0
স্থির PyObject* (*py3__PyObject_New)(PyTypeObject *);
# endif
স্থির PyObject* (*py3_PyFloat_FromDouble)(দ্ৱিগুণ অঙ্ক);
স্থির দ্ৱিগুণ (*py3_PyFloat_AsDouble)(PyObject *);
স্থির PyObject* (*py3_PyObject_GenericGetAttr)(PyObject *obj, PyObject *name);
স্থির PyObject* (*py3_PyType_GenericAlloc)(PyTypeObject *type, Py_ssize_t nitems);
স্থির PyObject* (*py3_PyType_GenericNew)(PyTypeObject *type, PyObject *args, PyObject *kwds);
স্থির PyTypeObject* py3_PyType_Type;
# ifndef USE_LIMITED_API
স্থির PyTypeObject* py3_PyStdPrinter_Type;
# endif
স্থির PyTypeObject* py3_PySlice_Type;
স্থির PyTypeObject* py3_PyFloat_Type;
স্থির PyTypeObject* py3_PyBool_Type;
স্থির পূর্ণাঙ্ক (*py3_Py_সংখ্যা_Check)(PyObject *);
স্থির PyObject* (*py3_Py_সংখ্যা_Long)(PyObject *);
স্থির PyObject* (*py3_PyErr_NewException)(ৱর্ণ *name, PyObject *base, PyObject *dict);
স্থির PyObject* (*py3_PyCapsule_New)(শূন্য *, ৱর্ণ *, PyCapsule_Destructor);
স্থির শূন্য* (*py3_PyCapsule_GetPointer)(PyObject *, ৱর্ণ *);
# ifdef Py_DEBUG
স্থির শূন্য (*py3__Py_NegativeRefcount)(ধ্রুৱ ৱর্ণ *fname, পূর্ণাঙ্ক lineno, PyObject *op);
স্থির Py_ssize_t* py3__Py_RefTotal;
স্থির PyObject* (*py3_PyModule_Create2TraceRefs)(সংরচনা PyModuleDef* module, পূর্ণাঙ্ক module_api_version);
# অন্যথা
স্থির PyObject* (*py3_PyModule_Create2)(সংরচনা PyModuleDef* module, পূর্ণাঙ্ক module_api_version);
# endif
# যদি defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
স্থির শূন্য (*py3__PyObject_DebugFree)(শূন্য*);
স্থির শূন্য* (*py3__PyObject_DebugMalloc)(size_t);
# অন্যথা
স্থির শূন্য (*py3_PyObject_Free)(শূন্য*);
স্থির শূন্য* (*py3_PyObject_Malloc)(size_t);
# endif
স্থির PyObject*(*py3__PyObject_GC_New)(PyTypeObject *);
স্থির শূন্য(*py3_PyObject_GC_Del)(শূন্য *);
স্থির শূন্য(*py3_PyObject_GC_UnTrack)(শূন্য *);
স্থির পূর্ণাঙ্ক (*py3_PyType_IsSubtype)(PyTypeObject *, PyTypeObject *);
# ifdef USE_LIMITED_API
স্থির শূন্য* (*py3_PyType_GetSlot)(PyTypeObject *, পূর্ণাঙ্ক);
স্থির PyObject* (*py3_PyType_FromSpec)(PyType_Spec *);
# endif

// Imported exception objects
স্থির PyObject *p3imp_PyExc_AttributeError;
স্থির PyObject *p3imp_PyExc_IndexError;
স্থির PyObject *p3imp_Py_Exc_চাবি_Error;
স্থির PyObject *p3imp_PyExc_KeyboardInterrupt;
স্থির PyObject *p3imp_PyExc_TypeError;
স্থির PyObject *p3imp_Py_Exc_মান_Error;
স্থির PyObject *p3imp_PyExc_SystemExit;
স্থির PyObject *p3imp_PyExc_RuntimeError;
স্থির PyObject *p3imp_PyExc_ImportError;
স্থির PyObject *p3imp_PyExc_OverflowError;

# define PyExc_AttributeError p3imp_PyExc_AttributeError
# define PyExc_IndexError p3imp_PyExc_IndexError
# define Py_Exc_চাবি_Error p3imp_Py_Exc_চাবি_Error
# define PyExc_KeyboardInterrupt p3imp_PyExc_KeyboardInterrupt
# define PyExc_TypeError p3imp_PyExc_TypeError
# define Py_Exc_মান_Error p3imp_Py_Exc_মান_Error
# define PyExc_SystemExit p3imp_PyExc_SystemExit
# define PyExc_RuntimeError p3imp_PyExc_RuntimeError
# define PyExc_ImportError p3imp_PyExc_ImportError
# define PyExc_OverflowError p3imp_PyExc_OverflowError

/*
 * Table of name to function pointer of python.
 */
# define PYTHON_PROC FARPROC
স্থির সংরচনা
{
    ৱর্ণ *name;
    PYTHON_PROC *ptr;
} py3_funcname_table[] =
{
    {"PySys_SetArgv", (PYTHON_PROC*)&py3_PySys_SetArgv},
    {"Py_SetPythonHome", (PYTHON_PROC*)&py3_Py_SetPythonHome},
    {"Py_Initialize", (PYTHON_PROC*)&py3_Py_Initialize},
    {"_PyArg_ParseTuple_SizeT", (PYTHON_PROC*)&py3_PyArg_ParseTuple},
    {"_Py_BuildValue_SizeT", (PYTHON_PROC*)&py3_Py_Build_মান},
    {"PyMem_Free", (PYTHON_PROC*)&py3_PyMem_Free},
    {"PyMem_Malloc", (PYTHON_PROC*)&py3_PyMem_Malloc},
    {"PyList_New", (PYTHON_PROC*)&py3_PyList_New},
    {"PyGILState_Ensure", (PYTHON_PROC*)&py3_PyGILState_Ensure},
    {"PyGILState_Release", (PYTHON_PROC*)&py3_PyGILState_Release},
    {"PySys_SetObject", (PYTHON_PROC*)&py3_PySys_SetObject},
    {"PySys_GetObject", (PYTHON_PROC*)&py3_PySys_GetObject},
    {"PyList_Append", (PYTHON_PROC*)&py3_PyList_Append},
    {"PyList_Insert", (PYTHON_PROC*)&py3_Py_List_ঢোকাও},
    {"PyList_Size", (PYTHON_PROC*)&py3_PyList_Size},
    {"PyTuple_New", (PYTHON_PROC*)&py3_PyTuple_New},
    {"PyTuple_GetItem", (PYTHON_PROC*)&py3_PyTuple_GetItem},
    {"PyTuple_SetItem", (PYTHON_PROC*)&py3_PyTuple_SetItem},
    {"PyTuple_Size", (PYTHON_PROC*)&py3_PyTuple_Size},
    {"PySequence_Check", (PYTHON_PROC*)&py3_PySequence_Check},
    {"PySequence_Size", (PYTHON_PROC*)&py3_PySequence_Size},
    {"PySequence_GetItem", (PYTHON_PROC*)&py3_PySequence_GetItem},
    {"PySequence_Fast", (PYTHON_PROC*)&py3_PySequence_Fast},
# যদি PY_VERSION_HEX >= 0x030601f0
    {"PySlice_AdjustIndices", (PYTHON_PROC*)&py3_PySlice_AdjustIndices},
    {"PySlice_Unpack", (PYTHON_PROC*)&py3_PySlice_Unpack},
# endif
    {"PySlice_GetIndicesEx", (PYTHON_PROC*)&py3_PySlice_GetIndicesEx},
    {"PyErr_NoMemory", (PYTHON_PROC*)&py3_PyErr_NoMemory},
    {"Py_Finalize", (PYTHON_PROC*)&py3_Py_Finalize},
    {"PyErr_SetString", (PYTHON_PROC*)&py3_PyErr_SetString},
    {"PyErr_SetObject", (PYTHON_PROC*)&py3_PyErr_SetObject},
    {"PyErr_ExceptionMatches", (PYTHON_PROC*)&py3_PyErr_ExceptionMatches},
# যদি defined(USE_LIMITED_API) || PY_VERSION_HEX >= 0x03080000
    {"Py_IncRef", (PYTHON_PROC*)&py3_Py_IncRef},
    {"Py_DecRef", (PYTHON_PROC*)&py3_Py_DecRef},
# endif
# ifdef USE_LIMITED_API
    {"Py_CompileString", (PYTHON_PROC*)&py3_Py_CompileString},
    {"PyEval_EvalCode", (PYTHON_PROC*)&PyEval_EvalCode},
# অন্যথা
    {"PyRun_SimpleString", (PYTHON_PROC*)&py3_PyRun_SimpleString},
    {"PyRun_String", (PYTHON_PROC*)&py3_PyRun_String},
# endif
    {"PyObject_GetAttrString", (PYTHON_PROC*)&py3_PyObject_GetAttrString},
    {"PyObject_HasAttrString", (PYTHON_PROC*)&py3_PyObject_HasAttrString},
    {"PyObject_SetAttrString", (PYTHON_PROC*)&py3_PyObject_SetAttrString},
    {"PyObject_CallFunctionObjArgs", (PYTHON_PROC*)&py3_PyObject_CallFunctionObjArgs},
# যদি PY_VERSION_HEX >= 0x030d0000
    {"PyObject_CallFunction", (PYTHON_PROC*)&py3_PyObject_CallFunction},
# অন্যথা
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
    {"PyMapping_Keys", (PYTHON_PROC*)&py3_Py_Mapping_চাবিগুলি},
# যদি (defined(USE_LIMITED_API) && Py_LIMITED_API >= 0x03080000) || \
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
# যদি PY_VERSION_HEX >= 0x03040000
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
# যদি !defined(USE_LIMITED_API) && PY_VERSION_HEX < 0x030D0000
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
#  যদি Py_LIMITED_API >= 0x030a0000
    {"PyUnicode_AsUTF8AndSize", (PYTHON_PROC*)&py3_PyUnicode_AsUTF8AndSize},
#  endif
# অন্যথা
#  যদি PY_VERSION_HEX >= 0x030300f0
    {"PyUnicode_AsUTF8AndSize", (PYTHON_PROC*)&py3_PyUnicode_AsUTF8AndSize},
#  অন্যথা
    {"_PyUnicode_AsString", (PYTHON_PROC*)&py3__PyUnicode_AsString},
#  endif
# endif
    {"PyUnicode_CompareWithASCIIString", (PYTHON_PROC*)&py3_PyUnicode_CompareWithASCIIString},
    {"PyUnicode_AsUTF8String", (PYTHON_PROC*)&py3_PyUnicode_AsUTF8String},
# ifdef Py_UNICODE_USE_UCS_FUNCTIONS
#  ifdef Py_UNICODE_WIDE
    {"PyUnicodeUCS4_FromFormat", (PYTHON_PROC*)&py3_PyUnicodeUCS4_FromFormat},
#  অন্যথা
    {"PyUnicodeUCS2_FromFormat", (PYTHON_PROC*)&py3_PyUnicodeUCS2_FromFormat},
#  endif
# অন্যথা
    {"PyUnicode_FromFormat", (PYTHON_PROC*)&py3_PyUnicode_FromFormat},
# endif
    {"PyBytes_AsString", (PYTHON_PROC*)&py3_PyBytes_AsString},
    {"PyBytes_AsStringAndSize", (PYTHON_PROC*)&py3_PyBytes_AsStringAndSize},
    {"PyBytes_FromString", (PYTHON_PROC*)&py3_PyBytes_FromString},
    {"PyBytes_FromStringAndSize", (PYTHON_PROC*)&py3_PyBytes_FromStringAndSize},
# যদি PY_VERSION_HEX >= 0x030900b0
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
    {"PyNumber_Check", (PYTHON_PROC*)&py3_Py_সংখ্যা_Check},
    {"PyNumber_Long", (PYTHON_PROC*)&py3_Py_সংখ্যা_Long},
    {"PyErr_NewException", (PYTHON_PROC*)&py3_PyErr_NewException},
# ifdef Py_DEBUG
    {"_Py_NegativeRefcount", (PYTHON_PROC*)&py3__Py_NegativeRefcount},
    {"_Py_RefTotal", (PYTHON_PROC*)&py3__Py_RefTotal},
    {"PyModule_Create2TraceRefs", (PYTHON_PROC*)&py3_PyModule_Create2TraceRefs},
# অন্যথা
    {"PyModule_Create2", (PYTHON_PROC*)&py3_PyModule_Create2},
# endif
# যদি defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
    {"_PyObject_DebugFree", (PYTHON_PROC*)&py3__PyObject_DebugFree},
    {"_PyObject_DebugMalloc", (PYTHON_PROC*)&py3__PyObject_DebugMalloc},
# অন্যথা
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
#  যদি PY_VERSION_HEX >= 0x03040000
    {"PyType_GetSlot", (PYTHON_PROC*)&py3_PyType_GetSlot},
#  endif
    {"PyType_FromSpec", (PYTHON_PROC*)&py3_PyType_FromSpec},
# endif
    {"", রিক্ত},
};

# যদি defined(USE_LIMITED_API) || PY_VERSION_HEX >= 0x03080000
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

# যদি PY_VERSION_HEX >= 0x030900b0
    স্থির অন্তঃস্থ পূর্ণাঙ্ক
py3_PyType_HasFeature(PyTypeObject *type, unsigned long feature)
{
    ঘূৰাই ((PyType_GetFlags(type) & feature) != 0);
}
#  define PyType_HasFeature(t,f) py3_PyType_HasFeature(t,f)
# endif

# যদি PY_VERSION_HEX >= 0x030a00b2
    স্থির অন্তঃস্থ পূর্ণাঙ্ক
py3__PyObject_TypeCheck(PyObject *ob, PyTypeObject *type)
{
    ঘূৰাই Py_IS_TYPE(ob, type) || PyType_IsSubtype(Py_TYPE(ob), type);
}
#  যদি PY_VERSION_HEX >= 0x030b00b3
#   undef PyObject_TypeCheck
#   define PyObject_TypeCheck(o,t) py3__PyObject_TypeCheck(o,t)
#  অন্যথা
#   define _PyObject_TypeCheck(o,t) py3__PyObject_TypeCheck(o,t)
#  endif
# endif

# যদি !defined(USE_LIMITED_API) && PY_VERSION_HEX >= 0x030c00b0
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
    স্থির WCHAR *
py3_get_system_libname(ধ্রুৱ ৱর্ণ *libname)
{
    ধ্রুৱ WCHAR	*pythoncore = L"Software\\Python\\PythonCore";
    ধ্রুৱ ৱর্ণ	*cp = libname;
    WCHAR	subkey[128];
    HKEY	h_চাবি;
    পূর্ণাঙ্ক		i;
    DWORD	j, len;
    LSTATUS	ret;

    যেতিয়ালৈ (*cp != '\0')
    {
	যদি (*cp == ':' || *cp == '\\' || *cp == '/')
	{
	    // Bail out if "libname" contains path separator, assume it is
	    // an absolute path.
	    ঘূৰাই রিক্ত;
	}
	++cp;
    }

    WCHAR   keyfound[32];
    HKEY    h_চাবি_শীৰ্ষ[] = {HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE};
    HKEY    h_চাবি_Found = রিক্ত;
#  ifdef USE_LIMITED_API
    long    maxminor = -1;
#  endif
    বাবে (i = 0; i < ARRAY_LENGTH(h_চাবি_শীৰ্ষ); i++)
    {
	long	major, minor;

	ret = Reg_Open_চাবি_Ex_W(h_চাবি_শীৰ্ষ[i], pythoncore, 0, চাবি_READ, &h_চাবি);
	যদি (ret != ERROR_SUCCESS)
	    অনুৱর্ত;
	বাবে (j = 0;; j++)
	{
	    WCHAR   keyname[32];
	    WCHAR   *wp;

	    len = ARRAY_LENGTH(keyname);
	    ret = Reg_Enum_চাবি_Ex_W(h_চাবি, j, keyname, &len,
						    রিক্ত, রিক্ত, রিক্ত, রিক্ত);
	    যদি (ret == ERROR_NO_MORE_বস্তুসমূহ)
		ভঙ্গ;

	    major = wcstol(keyname, &wp, 10);
	    যদি (*wp != L'.')
		অনুৱর্ত;
	    minor = wcstol(wp + 1, &wp, 10);
#  ifdef _WIN64
	    যদি (*wp != L'\0')
		অনুৱর্ত;
#  অন্যথা
	    যদি (wcscmp(wp, L"-32") != 0)
		অনুৱর্ত;
#  endif

	    যদি (major != PY_MAJOR_VERSION)
		অনুৱর্ত;
#  ifdef USE_LIMITED_API
	    // Search the latest version.
	    যদি ((minor > maxminor)
		    && (minor >= ((Py_LIMITED_API >> 16) & 0xff)))
	    {
		maxminor = minor;
		wcscpy(keyfound, keyname);
		h_চাবি_Found = h_চাবি_শীৰ্ষ[i];
	    }
#  অন্যথা
	    // Check if it matches with the compiled version.
	    যদি (minor == PY_MINOR_VERSION)
	    {
		wcscpy(keyfound, keyname);
		h_চাবি_Found = h_চাবি_শীৰ্ষ[i];
		ভঙ্গ;
	    }
#  endif
	}
	Reg_Close_চাবি(h_চাবি);
#  ifdef USE_LIMITED_API
	যদি (h_চাবি_Found != রিক্ত)
	    ভঙ্গ;
#  endif
    }
    যদি (h_চাবি_Found == রিক্ত)
	ঘূৰাই রিক্ত;

    swprintf(subkey, ARRAY_LENGTH(subkey), L"%ls\\%ls\\InstallPath",
							pythoncore, keyfound);
    ret = Reg_Get_মান_W(h_চাবি_Found, subkey, রিক্ত, RRF_RT_REG_SZ,
							    রিক্ত, রিক্ত, &len);
    যদি (ret != ERROR_MORE_DATA && ret != ERROR_SUCCESS)
	ঘূৰাই রিক্ত;
    size_t len2 = len / পরিমাণ(WCHAR) + 1 + strlen(libname);
    WCHAR *path = alloc(len2 * পরিমাণ(WCHAR));
    যদি (path == রিক্ত)
	ঘূৰাই রিক্ত;
    ret = Reg_Get_মান_W(h_চাবি_Found, subkey, রিক্ত, RRF_RT_REG_SZ,
							    রিক্ত, path, &len);
    যদি (ret != ERROR_SUCCESS)
    {
	vim_free(path);
	ঘূৰাই রিক্ত;
    }
    // Remove trailing path separators.
    size_t len3 = wcslen(path);
    যদি ((len3 > 0) && (path[len3 - 1] == L'/' || path[len3 - 1] == L'\\'))
	--len3;
    swprintf(path + len3, len2 - len3, L"\\%hs", libname);
    ঘূৰাই path;
}
# endif

/*
 * Load library and get all pointers.
 * Parameter 'libname' provides name of DLL.
 * Return OK or FAIL.
 */
    স্থির পূর্ণাঙ্ক
py3_runtime_link_init(ৱর্ণ *libname, পূর্ণাঙ্ক verbose)
{
    পূর্ণাঙ্ক i;
    PYTHON_PROC *ucs_from_string = (PYTHON_PROC *)&py3_PyUnicode_FromString;
    PYTHON_PROC *ucs_decode = (PYTHON_PROC *)&py3_PyUnicode_Decode;
    PYTHON_PROC *ucs_as_encoded_string =
				 (PYTHON_PROC *)&py3_PyUnicode_AsEncodedString;

# যদি !(defined(PY_NO_RTLD_GLOBAL) && defined(PY3_NO_RTLD_GLOBAL)) && defined(UNIX) && defined(FEAT_PYTHON)
    // Can't have Python and Python3 loaded at the same time.
    // It causes a crash, because RTLD_GLOBAL is needed for
    // standard C extension libraries of one or both python versions.
    যদি (python_loaded())
    {
	যদি (verbose)
	    emsg(_(e_this_vim_cannot_execute_py3_after_using_python));
	ঘূৰাই FAIL;
    }
# endif

    যদি (hinstPy3 != 0)
	ঘূৰাই OK;
    hinstPy3 = load_dll(libname);

# ifdef MSWIN
    যদি (!hinstPy3)
    {
	// Attempt to use the path from InstallPath as stored in the registry.
	WCHAR *syslibname = py3_get_system_libname(libname);

	যদি (syslibname != রিক্ত)
	{
	    hinstPy3 = LoadLibraryExW(syslibname, রিক্ত,
		    LOAD_LIBRARY_অনুসন্ধান_DLL_LOAD_DIR |
		    LOAD_LIBRARY_অনুসন্ধান_SYSTEM32);
	    vim_free(syslibname);
	}
    }
# endif

    যদি (!hinstPy3)
    {
	যদি (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	ঘূৰাই FAIL;
    }

    বাবে (i = 0; py3_funcname_table[i].ptr; ++i)
    {
	যদি ((*py3_funcname_table[i].ptr = প্ৰতীক_from_dll(hinstPy3,
			py3_funcname_table[i].name)) == রিক্ত)
	{
	    close_dll(hinstPy3);
	    hinstPy3 = 0;
	    যদি (verbose)
		semsg(_(e_could_not_load_library_function_str), py3_funcname_table[i].name);
	    ঘূৰাই FAIL;
	}
    }

    // Load unicode functions separately as only the ucs2 or the ucs4 functions
    // will be present in the library.
# যদি PY_VERSION_HEX >= 0x030300f0
    *ucs_from_string = প্ৰতীক_from_dll(hinstPy3, "PyUnicode_FromString");
    *ucs_decode = প্ৰতীক_from_dll(hinstPy3, "PyUnicode_Decode");
    *ucs_as_encoded_string = প্ৰতীক_from_dll(hinstPy3,
	    "PyUnicode_AsEncodedString");
# অন্যথা
    *ucs_from_string = প্ৰতীক_from_dll(hinstPy3, "PyUnicodeUCS2_FromString");
    *ucs_decode = প্ৰতীক_from_dll(hinstPy3,
	    "PyUnicodeUCS2_Decode");
    *ucs_as_encoded_string = প্ৰতীক_from_dll(hinstPy3,
	    "PyUnicodeUCS2_AsEncodedString");
    যদি (*ucs_from_string == রিক্ত || *ucs_decode == রিক্ত
					     || *ucs_as_encoded_string == রিক্ত)
    {
	*ucs_from_string = প্ৰতীক_from_dll(hinstPy3,
		"PyUnicodeUCS4_FromString");
	*ucs_decode = প্ৰতীক_from_dll(hinstPy3,
		"PyUnicodeUCS4_Decode");
	*ucs_as_encoded_string = প্ৰতীক_from_dll(hinstPy3,
		"PyUnicodeUCS4_AsEncodedString");
    }
# endif
    যদি (*ucs_from_string == রিক্ত || *ucs_decode == রিক্ত
					     || *ucs_as_encoded_string == রিক্ত)
    {
	close_dll(hinstPy3);
	hinstPy3 = 0;
	যদি (verbose)
	    semsg(_(e_could_not_load_library_function_str), "PyUnicode_UCSX_*");
	ঘূৰাই FAIL;
    }

    ঘূৰাই OK;
}

/*
 * If python is enabled (there is installed python on Windows system) return
 * TRUE, else FALSE.
 */
    পূর্ণাঙ্ক
python3_enabled(পূর্ণাঙ্ক verbose)
{
    ঘূৰাই py3_runtime_link_init((ৱর্ণ *)p_py3dll, verbose) == OK;
}

/*
 * Load the standard Python exceptions - don't import the symbols from the
 * DLL, as this can cause errors (importing data symbols is not reliable).
 */
    স্থির শূন্য
get_py3_exceptions(শূন্য)
{
    PyObject *exmod = PyImport_ImportModule("builtins");
    PyObject *exdict = PyModule_GetDict(exmod);
    p3imp_PyExc_AttributeError = PyDict_GetItemString(exdict, "AttributeError");
    p3imp_PyExc_IndexError = PyDict_GetItemString(exdict, "IndexError");
    p3imp_Py_Exc_চাবি_Error = PyDict_GetItemString(exdict, "KeyError");
    p3imp_PyExc_KeyboardInterrupt = PyDict_GetItemString(exdict, "KeyboardInterrupt");
    p3imp_PyExc_TypeError = PyDict_GetItemString(exdict, "TypeError");
    p3imp_Py_Exc_মান_Error = PyDict_GetItemString(exdict, "ValueError");
    p3imp_PyExc_SystemExit = PyDict_GetItemString(exdict, "SystemExit");
    p3imp_PyExc_RuntimeError = PyDict_GetItemString(exdict, "RuntimeError");
    p3imp_PyExc_ImportError = PyDict_GetItemString(exdict, "ImportError");
    p3imp_PyExc_OverflowError = PyDict_GetItemString(exdict, "OverflowError");
    Py_XINCREF(p3imp_PyExc_AttributeError);
    Py_XINCREF(p3imp_PyExc_IndexError);
    Py_XINCREF(p3imp_Py_Exc_চাবি_Error);
    Py_XINCREF(p3imp_PyExc_KeyboardInterrupt);
    Py_XINCREF(p3imp_PyExc_TypeError);
    Py_XINCREF(p3imp_Py_Exc_মান_Error);
    Py_XINCREF(p3imp_PyExc_SystemExit);
    Py_XINCREF(p3imp_PyExc_RuntimeError);
    Py_XINCREF(p3imp_PyExc_ImportError);
    Py_XINCREF(p3imp_PyExc_OverflowError);
    Py_XDECREF(exmod);
}
#endif // DYNAMIC_PYTHON3

স্থির পূর্ণাঙ্ক py3initialised = 0;
#define PYINITIALISED py3initialised
স্থির পূর্ণাঙ্ক python_end_called = FALSE;

#ifdef USE_LIMITED_API
# define DESTRUCTOR_FINISH(self) \
    ((freefunc)PyType_GetSlot(Py_TYPE((PyObject*)self), Py_tp_free))((PyObject*)self)
#অন্যথা
# define DESTRUCTOR_FINISH(self) Py_TYPE(self)->tp_free((PyObject*)self)
#endif

#define WIN_PYTHON_REF(win) win->w_python3_ref
#define BUF_PYTHON_REF(buf) buf->b_python3_ref
#define TAB_PYTHON_REF(tab) tab->tp_python3_ref

    স্থির শূন্য
call_PyObject_Free(শূন্য *p)
{
#যদি defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
    _PyObject_DebugFree(p);
#অন্যথা
    PyObject_Free(p);
#endif
}

    স্থির PyObject *
call_PyType_GenericNew(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    ঘূৰাই PyType_GenericNew(type,args,kwds);
}

    স্থির PyObject *
call_PyType_GenericAlloc(PyTypeObject *type, Py_ssize_t nitems)
{
    ঘূৰাই PyType_GenericAlloc(type,nitems);
}

স্থির PyObject *OutputGetattro(PyObject *, PyObject *);
স্থির পূর্ণাঙ্ক OutputSetattro(PyObject *, PyObject *, PyObject *);
স্থির PyObject *BufferGetattro(PyObject *, PyObject *);
স্থির পূর্ণাঙ্ক BufferSetattro(PyObject *, PyObject *, PyObject *);
স্থির PyObject *TabPageGetattro(PyObject *, PyObject *);
স্থির PyObject *WindowGetattro(PyObject *, PyObject *);
স্থির পূর্ণাঙ্ক WindowSetattro(PyObject *, PyObject *, PyObject *);
স্থির PyObject *RangeGetattro(PyObject *, PyObject *);
স্থির PyObject *CurrentGetattro(PyObject *, PyObject *);
স্থির পূর্ণাঙ্ক CurrentSetattro(PyObject *, PyObject *, PyObject *);
স্থির PyObject *DictionaryGetattro(PyObject *, PyObject *);
স্থির পূর্ণাঙ্ক DictionarySetattro(PyObject *, PyObject *, PyObject *);
স্থির PyObject *ListGetattro(PyObject *, PyObject *);
স্থির পূর্ণাঙ্ক ListSetattro(PyObject *, PyObject *, PyObject *);
স্থির PyObject *TupleGetattro(PyObject *, PyObject *);
স্থির পূর্ণাঙ্ক TupleSetattro(PyObject *, PyObject *, PyObject *);
স্থির PyObject *FunctionGetattro(PyObject *, PyObject *);

স্থির সংরচনা PyModuleDef vimmodule;

#define PY_CAN_RECURSE

/*
 * Include the code shared with if_python.c
 */
#অন্তর্ভাৱ "if_py_both.h"

#ifdef USE_LIMITED_API
# যদি Py_LIMITED_API >= 0x030A0000
#  define PY_UNICODE_GET_UTF8_CHARS(obj) PyUnicode_AsUTF8AndSize(obj, রিক্ত)
# অন্যথা
// Python limited API before 3.10 lack easy ways to query the raw UTF-8 chars.
// We need to first convert the string to bytes, and then extract the chars.
// This function is only used for attribute string comparisons, which have
// known short length. As such, just allocate a short static buffer to hold
// the characters instead of having to allocate/deallcoate it.
//
// An alternative would be to convert all attribute string comparisons to use
// PyUnicode_CompareWithASCIIString to skip having to extract the chars.
স্থির ৱর্ণ py3_unicode_utf8_chars[20];
স্থির ৱর্ণ* PY_UNICODE_GET_UTF8_CHARS(PyObject* str)
{
    py3_unicode_utf8_chars[0] = '\0';
    PyObject* bytes = PyUnicode_AsUTF8String(str);
    যদি (bytes)
    {
	ৱর্ণ *chars;
	Py_ssize_t len;
	যদি (PyBytes_AsStringAndSize(bytes, &chars, &len) != -1)
	{
	    যদি (len < (Py_ssize_t)পরিমাণ(py3_unicode_utf8_chars))
		// PyBytes_AsStringAndSize guarantees null-termination
		memcpy(py3_unicode_utf8_chars, chars, len + 1);
	}
	Py_DECREF(bytes);
    }
    ঘূৰাই py3_unicode_utf8_chars;
}
# endif
#অন্যথা	// !USE_LIMITED_API
# যদি PY_VERSION_HEX >= 0x030300f0
#  define PY_UNICODE_GET_UTF8_CHARS(obj) PyUnicode_AsUTF8AndSize(obj, রিক্ত)
# অন্যথা
#  define PY_UNICODE_GET_UTF8_CHARS _PyUnicode_AsString
# endif
#endif

// NOTE: Must always be used at the start of a block, since it declares "name".
#define GET_ATTR_STRING(name, nameobj) \
    ৱর্ণ	*name = ""; \
    যদি (PyUnicode_Check(nameobj)) \
	name = (ৱর্ণ *)PY_UNICODE_GET_UTF8_CHARS(nameobj)

#define PY3OBJ_DELETED(obj) (obj->ob_base.ob_refcnt<=0)

///////////////////////////////////////////////////////
// Internal function prototypes.

স্থির PyObject *Py3Init_vim(শূন্য);

///////////////////////////////////////////////////////
// 1. Python interpreter main program.

    শূন্য
python3_end(শূন্য)
{
    স্থির পূর্ণাঙ্ক recurse = 0;

    // If a crash occurs while doing this, don't try again.
    যদি (recurse != 0)
	ঘূৰাই;

    python_end_called = TRUE;
    ++recurse;

#ifdef DYNAMIC_PYTHON3
    যদি (hinstPy3)
#endif
    যদি (Py_IsInitialized())
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

#যদি defined(DYNAMIC_PYTHON3) && defined(DYNAMIC_PYTHON) && defined(FEAT_PYTHON) && defined(UNIX)
    পূর্ণাঙ্ক
python3_loaded(শূন্য)
{
    ঘূৰাই (hinstPy3 != 0);
}
#endif

স্থির wchar_t *py_home_buf = রিক্ত;

#যদি defined(MSWIN) && (PY_VERSION_HEX >= 0x030500f0)
/*
 * Return TRUE if stdin is readable from Python 3.
 */
    স্থির BOOL
is_stdin_readable(শূন্য)
{
    DWORD	    mode, eventnum;
    সংরচনা _stat    st;
    পূর্ণাঙ্ক		    fd = fileno(stdin);
    HANDLE	    hstdin = (HANDLE)_get_osfhandle(fd);

    // Check if stdin is connected to the console.
    যদি (GetConsoleMode(hstdin, &mode))
	// Check if it is opened as input.
	ঘূৰাই Get_সংখ্যা_Of_Console_প্ৰবেশ_Events(hstdin, &eventnum);

    ঘূৰাই _fstat(fd, &st) == 0;
}

// Python 3.5 or later will abort inside Py_Initialize() when stdin has
// been closed (i.e. executed by "vim -").  Reconnect stdin to CONIN$.
// Note that the python DLL is linked to its own stdio DLL which can be
// differ from Vim's stdio.
    স্থির শূন্য
reset_stdin(শূন্য)
{
    FILE *(*py__acrt_iob_func)(unsigned) = রিক্ত;
    FILE *(*pyfreopen)(ধ্রুৱ ৱর্ণ *, ধ্রুৱ ৱর্ণ *, FILE *) = রিক্ত;
    HINSTANCE hinst = get_forwarded_dll(hinstPy3);

    যদি (hinst == রিক্ত || is_stdin_readable())
	ঘূৰাই;

    // Get "freopen" and "stdin" which are used in the python DLL.
    // "stdin" is defined as "__acrt_iob_func(0)" in VC++ 2015 or later.
    py__acrt_iob_func = get_dll_import_func(hinst, "__acrt_iob_func");
    যদি (py__acrt_iob_func)
    {
	HINSTANCE hpystdiodll = find_imported_module_by_funcname(hinst,
							    "__acrt_iob_func");
	যদি (hpystdiodll)
	    pyfreopen = (শূন্য *)GetProcAddress(hpystdiodll, "freopen");
    }

    // Reconnect stdin to CONIN$.
    যদি (pyfreopen != রিক্ত)
	pyfreopen("CONIN$", "r", py__acrt_iob_func(0));
    অন্যথা
	freopen("CONIN$", "r", stdin);
}
#অন্যথা
# define reset_stdin()
#endif

// Python 3.2 or later will abort inside Py_Initialize() when mandatory
// modules cannot be loaded (e.g. 'pythonthreehome' is wrongly set.).
// Install a hook to python dll's exit() and recover from it.
#যদি defined(MSWIN) && (PY_VERSION_HEX >= 0x030200f0)
# define HOOK_EXIT
# include <setjmp.h>

স্থির jmp_buf exit_hook_jump_buf;
স্থির শূন্য *orig_exit = রিক্ত;

/*
 * Function that replaces exit() while calling Py_Initialize().
 */
    স্থির শূন্য
hooked_exit(পূর্ণাঙ্ক ret)
{
    // Recover from exit.
    longjmp(exit_hook_jump_buf, 1);
}

/*
 * Install a hook to python dll's exit().
 */
    স্থির শূন্য
hook_py_exit(শূন্য)
{
    HINSTANCE hinst = get_forwarded_dll(hinstPy3);

    যদি (hinst == রিক্ত || orig_exit != রিক্ত)
	ঘূৰাই;

    orig_exit = hook_dll_import_func(hinst, "exit", (শূন্য *)hooked_exit);
}

/*
 * Remove the hook installed by hook_py_exit().
 */
    স্থির শূন্য
restore_py_exit(শূন্য)
{
    HINSTANCE hinst = hinstPy3;

    যদি (hinst == রিক্ত)
	ঘূৰাই;

    যদি (orig_exit != রিক্ত)
	hook_dll_import_func(hinst, "exit", orig_exit);
    orig_exit = রিক্ত;
}
#endif

    স্থির পূর্ণাঙ্ক
Python3_Init(শূন্য)
{
    যদি (!py3initialised)
    {
#ifdef DYNAMIC_PYTHON3
	যদি (!python3_enabled(TRUE))
	{
	    emsg(_(e_sorry_this_command_is_disabled_python_library_could_not_be_found));
	    গচ্ছ fail;
	}
#endif
	// Python 3.13 introduced a really useful feature: colorized exceptions.
	// This is great if you're reading them from the terminal, but useless
	// and broken everywhere else (such as in log files, or text editors).
	// Opt out, forcefully.
	vim_setenv((char_u*)"PYTHON_COLORS", (char_u*)"0");

	init_structs();

	যদি (*p_py3home != NUL)
	{
	    size_t len = mbstowcs(রিক্ত, (ৱর্ণ *)p_py3home, 0) + 1;

	    // The string must not change later, make a copy in static memory.
	    py_home_buf = ALLOC_MULT(wchar_t, len);
	    যদি (py_home_buf != রিক্ত && mbstowcs(
			    py_home_buf, (ৱর্ণ *)p_py3home, len) != (size_t)-1)
		Py_SetPythonHome(py_home_buf);
	}
#ifdef PYTHON3_HOME
	অন্যথা যদি (mch_getenv((char_u *)"PYTHONHOME") == রিক্ত)
	    Py_SetPythonHome(PYTHON3_HOME);
#endif

	PyImport_AppendInittab("vim", Py3Init_vim);

#যদি !defined(DYNAMIC_PYTHON3) && defined(MSWIN)
	hinstPy3 = GetModuleHandle(PYTHON3_DLL);
#endif
	reset_stdin();

#ifdef HOOK_EXIT
	// Catch exit() called in Py_Initialize().
	hook_py_exit();
	যদি (setjmp(exit_hook_jump_buf) == 0)
	{
	    Py_Initialize();
	    restore_py_exit();
	}
	অন্যথা
	{
	    // exit() was called in Py_Initialize().
	    restore_py_exit();
	    emsg(_(e_critical_error_in_python3_initialization_check_your_installation));
	    গচ্ছ fail;
	}
#অন্যথা
	Py_Initialize();
#endif

#যদি PY_VERSION_HEX < 0x03090000
	// Initialise threads.  This is deprecated since Python 3.9.
	PyEval_InitThreads();
#endif
#ifdef DYNAMIC_PYTHON3
	get_py3_exceptions();
#endif

	যদি (PythonIO_Init_io())
	    গচ্ছ fail;

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

    ঘূৰাই 0;

fail:
    // We call PythonIO_Flush() here to print any Python errors.
    // This is OK, as it is possible to call this function even
    // if PythonIO_Init_io() has not completed successfully (it will
    // not do anything in this case).
    PythonIO_Flush();
    ঘূৰাই -1;
}

/*
 * External interface
 */
    স্থির শূন্য
DoPyCommand(ধ্রুৱ ৱর্ণ *cmd,
	    dict_T* locals,
	    rangeinitializer init_range,
	    runner run,
	    শূন্য *arg)
{
#যদি defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    ৱর্ণ		*saved_locale;
#endif
    PyObject		*cmdstr;
    PyObject		*cmdbytes;
    PyGILState_STATE	pygilstate;

    যদি (python_end_called)
	গচ্ছ theend;

    যদি (Python3_Init())
	গচ্ছ theend;

    init_range(arg);

    Python_Release_Vim();	    // leave Vim

#যদি defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    // Python only works properly when the LC_NUMERIC locale is "C".
    saved_locale = setlocale(LC_NUMERIC, রিক্ত);
    যদি (saved_locale == রিক্ত || STRCMP(saved_locale, "C") == 0)
	saved_locale = রিক্ত;
    অন্যথা
    {
	// Need to make a copy, value may change when setting new locale.
	saved_locale = (ৱর্ণ *)vim_strsave((char_u *)saved_locale);
	(শূন্য)setlocale(LC_NUMERIC, "C");
    }
#endif

    pygilstate = PyGILState_Ensure();

    // PyRun_SimpleString expects a UTF-8 string. Wrong encoding may cause
    // SyntaxError (unicode error).
    cmdstr = PyUnicode_Decode(cmd, strlen(cmd),
					(ৱর্ণ *)ENC_OPT, ERRORS_DECODE_ARG);
    cmdbytes = PyUnicode_AsEncodedString(cmdstr, "utf-8", ERRORS_ENCODE_ARG);
    Py_XDECREF(cmdstr);

    run(PyBytes_AsString(cmdbytes), locals, arg, &pygilstate);
    Py_XDECREF(cmdbytes);

    PyGILState_Release(pygilstate);

#যদি defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    যদি (saved_locale != রিক্ত)
    {
	(শূন্য)setlocale(LC_NUMERIC, saved_locale);
	vim_free(saved_locale);
    }
#endif

    Python_Lock_Vim();		    // enter Vim
    PythonIO_Flush();

theend:
    ঘূৰাই;	    // keeps lint happy
}

/*
 * ":py3"
 */
    শূন্য
ex_py3(exarg_T *eap)
{
    char_u *script;

    script = script_get(eap, eap->arg);
    যদি (!eap->skip)
    {
	যদি (p_pyx == 0)
	    p_pyx = 3;

	DoPyCommand(script == রিক্ত ? (ৱর্ণ *) eap->arg : (ৱর্ণ *) script,
		রিক্ত,
		init_range_cmd,
		(runner) run_cmd,
		(শূন্য *) eap);
    }
    vim_free(script);
}

#define BUFFER_SIZE 2048

/*
 * ":py3file"
 */
    শূন্য
ex_py3file(exarg_T *eap)
{
    স্থির ৱর্ণ buffer[BUFFER_SIZE];
    ধ্রুৱ ৱর্ণ *file;
    ৱর্ণ *p;
    পূর্ণাঙ্ক i;

    যদি (p_pyx == 0)
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

    বাবে (i=0; i<2; ++i)
    {
	file = (ৱর্ণ *)eap->arg;
	যেতিয়ালৈ (*file && p < buffer + (BUFFER_SIZE - 3))
	{
	    যদি (*file == '\\' || *file == '\'')
		*p++ = '\\';
	    *p++ = *file++;
	}
	// If we didn't finish the file name, we hit a buffer overflow
	যদি (*file != '\0')
	    ঘূৰাই;
	যদি (i==0)
	{
	    strcpy(p,"','rb').read(),'");
	    p += 16;
	}
	অন্যথা
	{
	    strcpy(p,"','exec'))");
	    p += 10;
	}
    }


    // Execute the file
    DoPyCommand(buffer,
	    রিক্ত,
	    init_range_cmd,
	    (runner) run_cmd,
	    (শূন্য *) eap);
}

    শূন্য
ex_py3do(exarg_T *eap)
{
    যদি (p_pyx == 0)
	p_pyx = 3;

    DoPyCommand((ৱর্ণ *)eap->arg,
	    রিক্ত,
	    init_range_cmd,
	    (runner)run_do,
	    (শূন্য *)eap);
}

///////////////////////////////////////////////////////
// 2. Python output stream: writes output via [e]msg().

// Implementation functions

    স্থির PyObject *
OutputGetattro(PyObject *self, PyObject *nameobj)
{
    GET_ATTR_STRING(name, nameobj);

    যদি (strcmp(name, "softspace") == 0)
	ঘূৰাই PyLong_FromLong(((OutputObject *)(self))->softspace);
    অন্যথা যদি (strcmp(name, "errors") == 0)
	ঘূৰাই PyString_FromString("strict");
    অন্যথা যদি (strcmp(name, "encoding") == 0)
	ঘূৰাই PyString_FromString(ENC_OPT);

    ঘূৰাই PyObject_GenericGetAttr(self, nameobj);
}

    স্থির পূর্ণাঙ্ক
OutputSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);

    ঘূৰাই OutputSetattr(self, name, val);
}

///////////////////////////////////////////////////////
// 3. Implementation of the Vim module for Python

// Window type - Implementation functions
// --------------------------------------

#define WindowType_Check(obj) ((obj)->ob_base.ob_type == &WindowType)

// Buffer type - Implementation functions
// --------------------------------------

#define BufferType_Check(obj) ((obj)->ob_base.ob_type == &BufferType)

স্থির PyObject* BufferSubscript(PyObject *self, PyObject *idx);
স্থির পূর্ণাঙ্ক BufferAsSubscript(PyObject *self, PyObject *idx, PyObject *val);

// Line range type - Implementation functions
// --------------------------------------

#define RangeType_Check(obj) ((obj)->ob_base.ob_type == &RangeType)

স্থির PyObject* RangeSubscript(PyObject *self, PyObject *idx);
স্থির পূর্ণাঙ্ক RangeAsItem(PyObject *, Py_ssize_t, PyObject *);
স্থির পূর্ণাঙ্ক RangeAsSubscript(PyObject *self, PyObject *idx, PyObject *val);

// Current objects type - Implementation functions
// -----------------------------------------------

স্থির PySequenceMethods BufferAsSeq = {
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

স্থির PyMappingMethods BufferAsMapping = {
    /* mp_length	*/ (lenfunc)BufferLength,
    /* mp_subscript     */ (binaryfunc)BufferSubscript,
    /* mp_ass_subscript */ (objobjargproc)BufferAsSubscript,
};


// Buffer object

    স্থির PyObject *
BufferGetattro(PyObject *self, PyObject *nameobj)
{
    PyObject *r;

    GET_ATTR_STRING(name, nameobj);

    যদি ((r = BufferAttrValid((BufferObject *)(self), name)))
	ঘূৰাই r;

    যদি (CheckBuffer((BufferObject *)(self)))
	ঘূৰাই রিক্ত;

    r = BufferAttr((BufferObject *)(self), name);
    যদি (r || PyErr_Occurred())
	ঘূৰাই r;
    অন্যথা
	ঘূৰাই PyObject_GenericGetAttr(self, nameobj);
}

    স্থির পূর্ণাঙ্ক
BufferSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);

    ঘূৰাই BufferSetattr(self, name, val);
}

//////////////////

    স্থির PyObject *
BufferSubscript(PyObject *self, PyObject* idx)
{
    যদি (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	ঘূৰাই BufferItem((BufferObject *)(self), _idx);
    }
    অন্যথা যদি (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	যদি (CheckBuffer((BufferObject *) self))
	    ঘূৰাই রিক্ত;

	যদি (PySlice_GetIndicesEx((PySliceObject_T *)idx,
	      (Py_ssize_t)((BufferObject *)(self))->buf->b_ml.ml_সাৰি_গুনতি,
	      &start, &stop,
	      &step, &slicelen) < 0)
	    ঘূৰাই রিক্ত;
	ঘূৰাই BufferSlice((BufferObject *)(self), start, stop);
    }
    অন্যথা
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	ঘূৰাই রিক্ত;
    }
}

    স্থির পূর্ণাঙ্ক
BufferAsSubscript(PyObject *self, PyObject* idx, PyObject* val)
{
    যদি (PyLong_Check(idx))
    {
	long n = PyLong_AsLong(idx);

	যদি (CheckBuffer((BufferObject *) self))
	    ঘূৰাই -1;

	ঘূৰাই RBAsItem((BufferObject *)(self), n, val, 1,
		    (Py_ssize_t)((BufferObject *)(self))->buf->b_ml.ml_সাৰি_গুনতি,
		    রিক্ত);
    }
    অন্যথা যদি (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	যদি (CheckBuffer((BufferObject *) self))
	    ঘূৰাই -1;

	যদি (PySlice_GetIndicesEx((PySliceObject_T *)idx,
	      (Py_ssize_t)((BufferObject *)(self))->buf->b_ml.ml_সাৰি_গুনতি,
	      &start, &stop,
	      &step, &slicelen) < 0)
	    ঘূৰাই -1;
	ঘূৰাই RBAsSlice((BufferObject *)(self), start, stop, val, 1,
			  (PyInt)((BufferObject *)(self))->buf->b_ml.ml_সাৰি_গুনতি,
			  রিক্ত);
    }
    অন্যথা
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	ঘূৰাই -1;
    }
}

স্থির PySequenceMethods RangeAsSeq = {
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

স্থির PyMappingMethods RangeAsMapping = {
    /* mp_length	*/ (lenfunc)RangeLength,
    /* mp_subscript     */ (binaryfunc)RangeSubscript,
    /* mp_ass_subscript */ (objobjargproc)RangeAsSubscript,
};

// Line range object - Implementation

    স্থির PyObject *
RangeGetattro(PyObject *self, PyObject *nameobj)
{
    GET_ATTR_STRING(name, nameobj);

    যদি (strcmp(name, "start") == 0)
	ঘূৰাই Py_Build_মান("n", ((RangeObject *)(self))->start - 1);
    অন্যথা যদি (strcmp(name, "end") == 0)
	ঘূৰাই Py_Build_মান("n", ((RangeObject *)(self))->end - 1);
    অন্যথা
	ঘূৰাই PyObject_GenericGetAttr(self, nameobj);
}

////////////////

    স্থির পূর্ণাঙ্ক
RangeAsItem(PyObject *self, Py_ssize_t n, PyObject *val)
{
    ঘূৰাই RBAsItem(((RangeObject *)(self))->buf, n, val,
		    ((RangeObject *)(self))->start,
		    ((RangeObject *)(self))->end,
		    &((RangeObject *)(self))->end);
}

    স্থির Py_ssize_t
RangeAsSlice(PyObject *self, Py_ssize_t lo, Py_ssize_t hi, PyObject *val)
{
    ঘূৰাই RBAsSlice(((RangeObject *)(self))->buf, lo, hi, val,
		    ((RangeObject *)(self))->start,
		    ((RangeObject *)(self))->end,
		    &((RangeObject *)(self))->end);
}

    স্থির PyObject *
RangeSubscript(PyObject *self, PyObject* idx)
{
    যদি (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	ঘূৰাই RangeItem((RangeObject *)(self), _idx);
    }
    অন্যথা যদি (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	যদি (PySlice_GetIndicesEx((PySliceObject_T *)idx,
		((RangeObject *)(self))->end-((RangeObject *)(self))->start+1,
		&start, &stop,
		&step, &slicelen) < 0)
	    ঘূৰাই রিক্ত;
	ঘূৰাই RangeSlice((RangeObject *)(self), start, stop);
    }
    অন্যথা
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	ঘূৰাই রিক্ত;
    }
}

    স্থির পূর্ণাঙ্ক
RangeAsSubscript(PyObject *self, PyObject *idx, PyObject *val)
{
    যদি (PyLong_Check(idx))
    {
	long n = PyLong_AsLong(idx);
	ঘূৰাই RangeAsItem(self, n, val);
    }
    অন্যথা যদি (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	যদি (PySlice_GetIndicesEx((PySliceObject_T *)idx,
		((RangeObject *)(self))->end-((RangeObject *)(self))->start+1,
		&start, &stop,
		&step, &slicelen) < 0)
	    ঘূৰাই -1;
	ঘূৰাই RangeAsSlice(self, start, stop, val);
    }
    অন্যথা
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	ঘূৰাই -1;
    }
}

// TabPage object - Implementation

    স্থির PyObject *
TabPageGetattro(PyObject *self, PyObject *nameobj)
{
    PyObject *r;

    GET_ATTR_STRING(name, nameobj);

    যদি ((r = TabPageAttrValid((TabPageObject *)(self), name)))
	ঘূৰাই r;

    যদি (CheckTabPage((TabPageObject *)(self)))
	ঘূৰাই রিক্ত;

    r = TabPageAttr((TabPageObject *)(self), name);
    যদি (r || PyErr_Occurred())
	ঘূৰাই r;
    অন্যথা
	ঘূৰাই PyObject_GenericGetAttr(self, nameobj);
}

// Window object - Implementation

    স্থির PyObject *
WindowGetattro(PyObject *self, PyObject *nameobj)
{
    PyObject *r;

    GET_ATTR_STRING(name, nameobj);

    যদি ((r = WindowAttrValid((WindowObject *)(self), name)))
	ঘূৰাই r;

    যদি (CheckWindow((WindowObject *)(self)))
	ঘূৰাই রিক্ত;

    r = WindowAttr((WindowObject *)(self), name);
    যদি (r || PyErr_Occurred())
	ঘূৰাই r;
    অন্যথা
	ঘূৰাই PyObject_GenericGetAttr(self, nameobj);
}

    স্থির পূর্ণাঙ্ক
WindowSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);

    ঘূৰাই WindowSetattr(self, name, val);
}

// Tab page list object - Definitions

স্থির PySequenceMethods TabListAsSeq = {
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

স্থির PySequenceMethods WinListAsSeq = {
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
    স্থির PyObject *
CurrentGetattro(PyObject *self, PyObject *nameobj)
{
    PyObject	*r;
    GET_ATTR_STRING(name, nameobj);
    যদি (!(r = CurrentGetattr(self, name)))
	ঘূৰাই PyObject_GenericGetAttr(self, nameobj);
    ঘূৰাই r;
}

    স্থির পূর্ণাঙ্ক
CurrentSetattro(PyObject *self, PyObject *nameobj, PyObject *মান)
{
    GET_ATTR_STRING(name, nameobj);
    ঘূৰাই CurrentSetattr(self, name, মান);
}

// Dictionary object - Definitions

    স্থির PyObject *
DictionaryGetattro(PyObject *self, PyObject *nameobj)
{
    DictionaryObject	*this = ((DictionaryObject *) (self));

    GET_ATTR_STRING(name, nameobj);

    যদি (strcmp(name, "locked") == 0)
	ঘূৰাই PyLong_FromLong(this->dict->dv_lock);
    অন্যথা যদি (strcmp(name, "scope") == 0)
	ঘূৰাই PyLong_FromLong(this->dict->dv_scope);

    ঘূৰাই PyObject_GenericGetAttr(self, nameobj);
}

    স্থির পূর্ণাঙ্ক
DictionarySetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);
    ঘূৰাই DictionarySetattr(self, name, val);
}

// List object - Definitions

    স্থির PyObject *
ListGetattro(PyObject *self, PyObject *nameobj)
{
    GET_ATTR_STRING(name, nameobj);

    যদি (strcmp(name, "locked") == 0)
	ঘূৰাই PyLong_FromLong(((ListObject *) (self))->list->lv_lock);

    ঘূৰাই PyObject_GenericGetAttr(self, nameobj);
}

    স্থির পূর্ণাঙ্ক
ListSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);
    ঘূৰাই ListSetattr(self, name, val);
}

// Tuple object - Definitions

    স্থির PyObject *
TupleGetattro(PyObject *self, PyObject *nameobj)
{
    GET_ATTR_STRING(name, nameobj);

    যদি (strcmp(name, "locked") == 0)
	ঘূৰাই PyLong_FromLong(((TupleObject *) (self))->tuple->tv_lock);

    ঘূৰাই PyObject_GenericGetAttr(self, nameobj);
}

    স্থির পূর্ণাঙ্ক
TupleSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);
    ঘূৰাই TupleSetattr(self, name, val);
}

// Function object - Definitions

    স্থির PyObject *
FunctionGetattro(PyObject *self, PyObject *nameobj)
{
    PyObject		*r;
    FunctionObject	*this = (FunctionObject *)(self);

    GET_ATTR_STRING(name, nameobj);

    r = FunctionAttr(this, name);
    যদি (r || PyErr_Occurred())
	ঘূৰাই r;
    অন্যথা
	ঘূৰাই PyObject_GenericGetAttr(self, nameobj);
}

// External interface

    শূন্য
python3_buffer_free(buf_T *buf)
{
    BufferObject *bp = BUF_PYTHON_REF(buf);
    যদি (bp == রিক্ত)
	ঘূৰাই;
    bp->buf = INVALID_BUFFER_মান;
    BUF_PYTHON_REF(buf) = রিক্ত;
}

    শূন্য
python3_window_free(win_T *win)
{
    WindowObject *wp = WIN_PYTHON_REF(win);
    যদি (wp == রিক্ত)
	ঘূৰাই;
    wp->win = INVALID_WINDOW_মান;
    WIN_PYTHON_REF(win) = রিক্ত;
}

    শূন্য
python3_tabpage_free(tabpage_T *tab)
{
    TabPageObject *tp = TAB_PYTHON_REF(tab);
    যদি (tp == রিক্ত)
	ঘূৰাই;
    tp->tab = INVALID_TABPAGE_মান;
    TAB_PYTHON_REF(tab) = রিক্ত;
}

    স্থির PyObject *
Py3Init_vim(শূন্য)
{
    // The special value is removed from sys.path in Python3_Init().
    স্থির wchar_t *(argv[2]) = {L"/must>not&exist/foo", রিক্ত};

    যদি (init_types())
	ঘূৰাই রিক্ত;

    // Set sys.argv[] to avoid a crash in warn().
    PySys_SetArgv(1, argv);

    যদি ((vim_module = PyModule_Create(&vimmodule)) == রিক্ত)
	ঘূৰাই রিক্ত;

    যদি (populate_module(vim_module))
	ঘূৰাই রিক্ত;

    যদি (init_sys_path())
	ঘূৰাই রিক্ত;

    ঘূৰাই vim_module;
}

//////////////////////////////////////////////////////////////////////////
// 4. Utility functions for handling the interface between Vim and Python.

/*
 * Convert a Vim line into a Python string.
 * All internal newlines are replaced by null characters.
 *
 * On errors, the Python exception data is set, and NULL is returned.
 */
    স্থির PyObject *
সাৰি_To_String(ধ্রুৱ ৱর্ণ *str)
{
    PyObject *result;
    Py_ssize_t len = strlen(str);
    ৱর্ণ *tmp, *p;

    tmp = alloc(len + 1);
    p = tmp;
    যদি (p == রিক্ত)
    {
	PyErr_NoMemory();
	ঘূৰাই রিক্ত;
    }

    যেতিয়ালৈ (*str)
    {
	যদি (*str == '\n')
	    *p = '\0';
	অন্যথা
	    *p = *str;

	++p;
	++str;
    }
    *p = '\0';

    result = PyUnicode_Decode(tmp, len, (ৱর্ণ *)ENC_OPT, ERRORS_DECODE_ARG);

    vim_free(tmp);
    ঘূৰাই result;
}

    শূন্য
do_py3eval(char_u *str, dict_T *locals, typval_T *rettv)
{
    DoPyCommand((ৱর্ণ *) str,
	    locals,
	    init_range_eval,
	    (runner) run_eval,
	    (শূন্য *) rettv);
    যদি (rettv->v_type == VAR_UNKNOWN)
    {
	rettv->v_type = VAR_সংখ্যা;
	rettv->vval.v_সংখ্যা = 0;
    }
}

    পূর্ণাঙ্ক
set_ref_in_python3(পূর্ণাঙ্ক copyID)
{
    ঘূৰাই set_ref_in_py(copyID);
}

    পূর্ণাঙ্ক
python3_version(শূন্য)
{
#ifdef USE_LIMITED_API
    ঘূৰাই Py_LIMITED_API;
#অন্যথা
    ঘূৰাই PY_VERSION_HEX;
#endif
}
