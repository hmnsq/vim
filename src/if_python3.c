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

#અન્તર્ભાવ "vim.h"

#અન્તર્ભાવ <limits.h>

#જો defined(MSWIN) && defined(HAVE_FCNTL_H)
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

#અન્તર્ભાવ <Python.h>

#undef મુખ્ય // Defined in python.h - aargh
#undef HAVE_FCNTL_H // Clash with os_win32.h

// The "surrogateescape" error handler is new in Python 3.1
#જો PY_VERSION_HEX >= 0x030100f0
# define CODEC_ERROR_HANDLER "surrogateescape"
#નહીંતર
# define CODEC_ERROR_HANDLER ખાલી
#endif

// Suppress Python 3.11 depreciations to see useful warnings
#ifdef __GNUC__
# pragma GCC diagnostic ધકેલો
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
#જો PY_VERSION_HEX >= 0x030200f0
પ્રકારવ્યાખ્યા PyObject PySliceObject_T;
#નહીંતર
પ્રકારવ્યાખ્યા PySliceObject PySliceObject_T;
#endif

#ifndef MSWIN
# define HINSTANCE શૂન્ય *
#endif
#જો defined(DYNAMIC_PYTHON3) || defined(MSWIN)
સ્થિર HINSTANCE hinstPy3 = 0; // Instance of python.dll
#endif

#જો defined(DYNAMIC_PYTHON3)

# ifdef MSWIN
#  define load_dll vimLoadLib
#  define close_dll FreeLibrary
#  define પ્રતીક_from_dll GetProcAddress
#  define load_dll_error GetWin32Error
# નહીંતર
#  include <dlfcn.h>
#  define FARPROC શૂન્ય*
#  જો defined(PY_NO_RTLD_GLOBAL) && defined(PY3_NO_RTLD_GLOBAL)
#   define load_dll(n) dlopen((n), RTLD_LAZY)
#  નહીંતર
#   define load_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#  endif
#  define close_dll dlclose
#  define પ્રતીક_from_dll dlsym
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
# define Py_List_ઉમેરો py3_Py_List_ઉમેરો
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
# જો PY_VERSION_HEX >= 0x030601f0
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
# ifndef Py_Mapping_ચાવીઓ
#  define Py_Mapping_ચાવીઓ py3_Py_Mapping_ચાવીઓ
# endif
# જો (defined(USE_LIMITED_API) && Py_LIMITED_API >= 0x03080000) || \
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
# જો defined(USE_LIMITED_API) || PY_VERSION_HEX >= 0x03080000
#  define Py_IncRef py3_Py_IncRef
#  define Py_DecRef py3_Py_DecRef
# endif
# ifdef USE_LIMITED_API
#  define Py_CompileString py3_Py_CompileString
#  define PyEval_EvalCode py3_PyEval_EvalCode
# નહીંતર
#  undef PyRun_SimpleString
#  define PyRun_SimpleString py3_PyRun_SimpleString
#  undef PyRun_String
#  define PyRun_String py3_PyRun_String
# endif
# define PyObject_GetAttrString py3_PyObject_GetAttrString
# define PyObject_HasAttrString py3_PyObject_HasAttrString
# define PyObject_SetAttrString py3_PyObject_SetAttrString
# define PyObject_CallFunctionObjArgs py3_PyObject_CallFunctionObjArgs
# જો PY_VERSION_HEX >= 0x030d0000
#  define PyObject_CallFunction py3_PyObject_CallFunction
# નહીંતર
#  define _PyObject_CallFunction_SizeT py3__PyObject_CallFunction_SizeT
# endif
# define PyObject_Call py3_PyObject_Call
# define PyEval_GetLocals py3_PyEval_GetLocals
# define PyEval_GetGlobals py3_PyEval_GetGlobals
# define PySys_SetObject py3_PySys_SetObject
# define PySys_GetObject py3_PySys_GetObject
# define PySys_SetArgv py3_PySys_SetArgv
# define PyType_Ready py3_PyType_Ready
# જો PY_VERSION_HEX >= 0x03040000
#  define PyType_GetFlags py3_PyType_GetFlags
# endif
# undef Py_Build_કિંમત
# define Py_Build_કિંમત py3_Py_Build_કિંમત
# define Py_SetPythonHome py3_Py_SetPythonHome
# define Py_Initialize py3_Py_Initialize
# define Py_Finalize py3_Py_Finalize
# define Py_IsInitialized py3_Py_IsInitialized
# define _Py_NoneStruct (*py3__Py_NoneStruct)
# define _Py_FalseStruct (*py3__Py_FalseStruct)
# define _Py_TrueStruct (*py3__Py_TrueStruct)
# જો !defined(USE_LIMITED_API) && PY_VERSION_HEX < 0x030D0000
// Private symbol that used to be required as part of PyIter_Check.
#  define _PyObject_NextNotImplemented (*py3__PyObject_NextNotImplemented)
# endif
# define PyModule_AddObject py3_PyModule_AddObject
# define PyImport_AppendInittab py3_PyImport_AppendInittab
# define PyImport_AddModule py3_PyImport_AddModule
# ifdef USE_LIMITED_API
#  જો Py_LIMITED_API >= 0x030a0000
#   define PyUnicode_AsUTF8AndSize py3_PyUnicode_AsUTF8AndSize
#  endif
# નહીંતર
#  જો PY_VERSION_HEX >= 0x030300f0
#   define PyUnicode_AsUTF8AndSize py3_PyUnicode_AsUTF8AndSize
#  નહીંતર
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
# define Py_સંખ્યા_Check (*py3_Py_સંખ્યા_Check)
# define Py_સંખ્યા_Long (*py3_Py_સંખ્યા_Long)
# define PyBool_Type (*py3_PyBool_Type)
# define PyErr_NewException py3_PyErr_NewException
# ifdef Py_DEBUG
#  define _Py_NegativeRefcount py3__Py_NegativeRefcount
#  define _Py_RefTotal (*py3__Py_RefTotal)
#  define PyModule_Create2TraceRefs py3_PyModule_Create2TraceRefs
# નહીંતર
#  define PyModule_Create2 py3_PyModule_Create2
# endif
# જો defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
#  define _PyObject_DebugMalloc py3__PyObject_DebugMalloc
#  define _PyObject_DebugFree py3__PyObject_DebugFree
# નહીંતર
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
#  નહીંતર
#   define PyUnicodeUCS2_FromFormat py3_PyUnicodeUCS2_FromFormat
#  endif
# નહીંતર
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

# જો defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
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
સ્થિર પૂર્ણાંક (*py3_PySys_SetArgv)(પૂર્ણાંક, wchar_t **);
સ્થિર શૂન્ય (*py3_Py_SetPythonHome)(wchar_t *home);
સ્થિર શૂન્ય (*py3_Py_Initialize)(શૂન્ય);
સ્થિર PyObject* (*py3_PyList_New)(Py_ssize_t size);
સ્થિર PyGILState_STATE (*py3_PyGILState_Ensure)(શૂન્ય);
સ્થિર શૂન્ય (*py3_PyGILState_Release)(PyGILState_STATE);
સ્થિર પૂર્ણાંક (*py3_PySys_SetObject)(અક્ષર *, PyObject *);
સ્થિર PyObject* (*py3_PySys_GetObject)(અક્ષર *);
સ્થિર પૂર્ણાંક (*py3_PyList_Append)(PyObject *, PyObject *);
સ્થિર પૂર્ણાંક (*py3_Py_List_ઉમેરો)(PyObject *, પૂર્ણાંક, PyObject *);
સ્થિર Py_ssize_t (*py3_PyList_Size)(PyObject *);
સ્થિર PyObject* (*py3_PyTuple_New)(Py_ssize_t size);
સ્થિર પૂર્ણાંક (*py3_PyTuple_SetItem)(PyObject *, Py_ssize_t, PyObject *);
સ્થિર Py_ssize_t (*py3_PyTuple_Size)(PyObject *);
સ્થિર પૂર્ણાંક (*py3_PySequence_Check)(PyObject *);
સ્થિર Py_ssize_t (*py3_PySequence_Size)(PyObject *);
સ્થિર PyObject* (*py3_PySequence_GetItem)(PyObject *, Py_ssize_t);
સ્થિર PyObject* (*py3_PySequence_Fast)(PyObject *, અચલ અક્ષર *);
સ્થિર PyObject* (*py3_PyTuple_GetItem)(PyObject *, Py_ssize_t);
સ્થિર પૂર્ણાંક (*py3_PyMapping_Check)(PyObject *);
સ્થિર PyObject* (*py3_Py_Mapping_ચાવીઓ)(PyObject *);
# જો PY_VERSION_HEX >= 0x030601f0
સ્થિર પૂર્ણાંક (*py3_PySlice_AdjustIndices)(Py_ssize_t length,
		     Py_ssize_t *start, Py_ssize_t *stop, Py_ssize_t step);
સ્થિર પૂર્ણાંક (*py3_PySlice_Unpack)(PyObject *slice,
		     Py_ssize_t *start, Py_ssize_t *stop, Py_ssize_t *step);
# endif
સ્થિર પૂર્ણાંક (*py3_PySlice_GetIndicesEx)(PySliceObject_T *r, Py_ssize_t length,
		     Py_ssize_t *start, Py_ssize_t *stop, Py_ssize_t *step,
		     Py_ssize_t *slicelen);
સ્થિર PyObject* (*py3_PyErr_NoMemory)(શૂન્ય);
સ્થિર શૂન્ય (*py3_Py_Finalize)(શૂન્ય);
સ્થિર શૂન્ય (*py3_PyErr_SetString)(PyObject *, અચલ અક્ષર *);
સ્થિર શૂન્ય (*py3_PyErr_SetObject)(PyObject *, PyObject *);
સ્થિર પૂર્ણાંક (*py3_PyErr_ExceptionMatches)(PyObject *);
# જો defined(USE_LIMITED_API) || PY_VERSION_HEX >= 0x03080000
સ્થિર શૂન્ય (*py3_Py_IncRef)(PyObject *);
સ્થિર શૂન્ય (*py3_Py_DecRef)(PyObject *);
# endif
# ifdef USE_LIMITED_API
સ્થિર PyObject* (*py3_Py_CompileString)(અચલ અક્ષર *, અચલ અક્ષર *, પૂર્ણાંક);
સ્થિર PyObject* (*py3_PyEval_EvalCode)(PyObject *co, PyObject *globals, PyObject *locals);
# નહીંતર
સ્થિર પૂર્ણાંક (*py3_PyRun_SimpleString)(અક્ષર *);
સ્થિર PyObject* (*py3_PyRun_String)(અક્ષર *, પૂર્ણાંક, PyObject *, PyObject *);
# endif
સ્થિર PyObject* (*py3_PyObject_GetAttrString)(PyObject *, અચલ અક્ષર *);
સ્થિર પૂર્ણાંક (*py3_PyObject_HasAttrString)(PyObject *, અચલ અક્ષર *);
સ્થિર પૂર્ણાંક (*py3_PyObject_SetAttrString)(PyObject *, અચલ અક્ષર *, PyObject *);
સ્થિર PyObject* (*py3_PyObject_CallFunctionObjArgs)(PyObject *, ...);
# જો PY_VERSION_HEX >= 0x030d0000
સ્થિર PyObject* (*py3_PyObject_CallFunction)(PyObject *, અક્ષર *, ...);
# નહીંતર
સ્થિર PyObject* (*py3__PyObject_CallFunction_SizeT)(PyObject *, અક્ષર *, ...);
# endif
સ્થિર PyObject* (*py3_PyObject_Call)(PyObject *, PyObject *, PyObject *);
સ્થિર PyObject* (*py3_PyEval_GetGlobals)(શૂન્ય);
સ્થિર PyObject* (*py3_PyEval_GetLocals)(શૂન્ય);
સ્થિર PyObject* (*py3_PyList_GetItem)(PyObject *, Py_ssize_t);
સ્થિર PyObject* (*py3_PyImport_ImportModule)(અચલ અક્ષર *);
સ્થિર PyObject* (*py3_PyImport_AddModule)(અચલ અક્ષર *);
સ્થિર પૂર્ણાંક (*py3_PyErr_BadArgument)(શૂન્ય);
સ્થિર PyObject* (*py3_PyErr_Occurred)(શૂન્ય);
સ્થિર PyObject* (*py3_PyModule_GetDict)(PyObject *);
સ્થિર પૂર્ણાંક (*py3_PyList_SetItem)(PyObject *, Py_ssize_t, PyObject *);
સ્થિર PyObject* (*py3_PyDict_GetItemString)(PyObject *, અચલ અક્ષર *);
સ્થિર પૂર્ણાંક (*py3_PyDict_Next)(PyObject *, Py_ssize_t *, PyObject **, PyObject **);
સ્થિર PyObject* (*py3_PyLong_FromLong)(long);
સ્થિર PyObject* (*py3_PyDict_New)(શૂન્ય);
# જો (defined(USE_LIMITED_API) && Py_LIMITED_API >= 0x03080000) || \
       (!defined(USE_LIMITED_API) && PY_VERSION_HEX >= 0x03080000)
સ્થિર પૂર્ણાંક (*py3_PyIter_Check)(PyObject *o);
# endif
સ્થિર PyObject* (*py3_PyIter_Next)(PyObject *);
સ્થિર PyObject* (*py3_PyObject_GetIter)(PyObject *);
સ્થિર PyObject* (*py3_PyObject_Repr)(PyObject *);
સ્થિર PyObject* (*py3_PyObject_GetItem)(PyObject *, PyObject *);
સ્થિર પૂર્ણાંક (*py3_PyObject_IsTrue)(PyObject *);
સ્થિર PyObject* (*py3_Py_Build_કિંમત)(અક્ષર *, ...);
# જો PY_VERSION_HEX >= 0x03040000
સ્થિર પૂર્ણાંક (*py3_PyType_GetFlags)(PyTypeObject *o);
# endif
સ્થિર પૂર્ણાંક (*py3_PyType_Ready)(PyTypeObject *type);
સ્થિર પૂર્ણાંક (*py3_PyDict_SetItemString)(PyObject *dp, અક્ષર *ચાવી, PyObject *item);
સ્થિર PyObject* (*py3_PyUnicode_FromString)(અચલ અક્ષર *u);
# ifdef Py_UNICODE_USE_UCS_FUNCTIONS
#  ifdef Py_UNICODE_WIDE
સ્થિર PyObject* (*py3_PyUnicodeUCS4_FromFormat)(અચલ અક્ષર *u, ...);
#  નહીંતર
સ્થિર PyObject* (*py3_PyUnicodeUCS2_FromFormat)(અચલ અક્ષર *u, ...);
#  endif
# નહીંતર
સ્થિર PyObject* (*py3_PyUnicode_FromFormat)(અચલ અક્ષર *u, ...);
# endif
સ્થિર PyObject* (*py3_PyUnicode_Decode)(અચલ અક્ષર *u, Py_ssize_t size,
	અચલ અક્ષર *encoding, અચલ અક્ષર *errors);
સ્થિર long (*py3_PyLong_AsLong)(PyObject *);
સ્થિર શૂન્ય (*py3_PyErr_SetNone)(PyObject *);
સ્થિર શૂન્ય (*py3_PyEval_InitThreads)(શૂન્ય);
સ્થિર શૂન્ય(*py3_PyEval_RestoreThread)(PyThreadState *);
સ્થિર PyThreadState*(*py3_PyEval_SaveThread)(શૂન્ય);
સ્થિર પૂર્ણાંક (*py3_PyArg_Parse)(PyObject *, અક્ષર *, ...);
સ્થિર પૂર્ણાંક (*py3_PyArg_ParseTuple)(PyObject *, અક્ષર *, ...);
સ્થિર શૂન્ય (*py3_PyMem_Free)(શૂન્ય *);
સ્થિર શૂન્ય* (*py3_PyMem_Malloc)(size_t);
સ્થિર પૂર્ણાંક (*py3_Py_IsInitialized)(શૂન્ય);
સ્થિર શૂન્ય (*py3_PyErr_Clear)(શૂન્ય);
સ્થિર PyObject* (*py3_PyErr_Format)(PyObject *, અચલ અક્ષર *, ...);
સ્થિર શૂન્ય (*py3_PyErr_PrintEx)(પૂર્ણાંક);
સ્થિર PyObject*(*py3__PyObject_Init)(PyObject *, PyTypeObject *);
# જો !defined(USE_LIMITED_API) && PY_VERSION_HEX < 0x030D0000
સ્થિર iternextfunc py3__PyObject_NextNotImplemented;
# endif
સ્થિર PyObject* py3__Py_NoneStruct;
સ્થિર PyObject* py3__Py_FalseStruct;
સ્થિર PyObject* py3__Py_TrueStruct;
સ્થિર પૂર્ણાંક (*py3_PyModule_AddObject)(PyObject *m, અચલ અક્ષર *name, PyObject *o);
સ્થિર પૂર્ણાંક (*py3_PyImport_AppendInittab)(અચલ અક્ષર *name, PyObject* (*initfunc)(શૂન્ય));
# ifdef USE_LIMITED_API
#  જો Py_LIMITED_API >= 0x030a0000
સ્થિર અક્ષર* (*py3_PyUnicode_AsUTF8AndSize)(PyObject *unicode, Py_ssize_t *size);
#  endif
# નહીંતર
#  જો PY_VERSION_HEX >= 0x030300f0
સ્થિર અક્ષર* (*py3_PyUnicode_AsUTF8AndSize)(PyObject *unicode, Py_ssize_t *size);
#  નહીંતર
સ્થિર અક્ષર* (*py3__PyUnicode_AsString)(PyObject *unicode);
#  endif
# endif
સ્થિર પૂર્ણાંક (*py3_PyUnicode_CompareWithASCIIString)(PyObject *unicode, અચલ અક્ષર* string);
સ્થિર PyObject* (*py3_PyUnicode_AsEncodedString)(PyObject *unicode, અચલ અક્ષર* encoding, અચલ અક્ષર* errors);
સ્થિર PyObject* (*py3_PyUnicode_AsUTF8String)(PyObject *unicode);
સ્થિર અક્ષર* (*py3_PyBytes_AsString)(PyObject *bytes);
સ્થિર પૂર્ણાંક (*py3_PyBytes_AsStringAndSize)(PyObject *bytes, અક્ષર **buffer, Py_ssize_t *length);
સ્થિર PyObject* (*py3_PyBytes_FromString)(અક્ષર *str);
સ્થિર PyObject* (*py3_PyBytes_FromStringAndSize)(અક્ષર *str, Py_ssize_t length);
# જો PY_VERSION_HEX >= 0x030900b0
સ્થિર PyObject* (*py3__PyObject_New)(PyTypeObject *);
# endif
સ્થિર PyObject* (*py3_PyFloat_FromDouble)(દ્વિગુણ અંક);
સ્થિર દ્વિગુણ (*py3_PyFloat_AsDouble)(PyObject *);
સ્થિર PyObject* (*py3_PyObject_GenericGetAttr)(PyObject *obj, PyObject *name);
સ્થિર PyObject* (*py3_PyType_GenericAlloc)(PyTypeObject *type, Py_ssize_t nitems);
સ્થિર PyObject* (*py3_PyType_GenericNew)(PyTypeObject *type, PyObject *args, PyObject *kwds);
સ્થિર PyTypeObject* py3_PyType_Type;
# ifndef USE_LIMITED_API
સ્થિર PyTypeObject* py3_PyStdPrinter_Type;
# endif
સ્થિર PyTypeObject* py3_PySlice_Type;
સ્થિર PyTypeObject* py3_PyFloat_Type;
સ્થિર PyTypeObject* py3_PyBool_Type;
સ્થિર પૂર્ણાંક (*py3_Py_સંખ્યા_Check)(PyObject *);
સ્થિર PyObject* (*py3_Py_સંખ્યા_Long)(PyObject *);
સ્થિર PyObject* (*py3_PyErr_NewException)(અક્ષર *name, PyObject *base, PyObject *dict);
સ્થિર PyObject* (*py3_PyCapsule_New)(શૂન્ય *, અક્ષર *, PyCapsule_Destructor);
સ્થિર શૂન્ય* (*py3_PyCapsule_GetPointer)(PyObject *, અક્ષર *);
# ifdef Py_DEBUG
સ્થિર શૂન્ય (*py3__Py_NegativeRefcount)(અચલ અક્ષર *fname, પૂર્ણાંક lineno, PyObject *op);
સ્થિર Py_ssize_t* py3__Py_RefTotal;
સ્થિર PyObject* (*py3_PyModule_Create2TraceRefs)(સંરચના PyModuleDef* module, પૂર્ણાંક module_api_version);
# નહીંતર
સ્થિર PyObject* (*py3_PyModule_Create2)(સંરચના PyModuleDef* module, પૂર્ણાંક module_api_version);
# endif
# જો defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
સ્થિર શૂન્ય (*py3__PyObject_DebugFree)(શૂન્ય*);
સ્થિર શૂન્ય* (*py3__PyObject_DebugMalloc)(size_t);
# નહીંતર
સ્થિર શૂન્ય (*py3_PyObject_Free)(શૂન્ય*);
સ્થિર શૂન્ય* (*py3_PyObject_Malloc)(size_t);
# endif
સ્થિર PyObject*(*py3__PyObject_GC_New)(PyTypeObject *);
સ્થિર શૂન્ય(*py3_PyObject_GC_Del)(શૂન્ય *);
સ્થિર શૂન્ય(*py3_PyObject_GC_UnTrack)(શૂન્ય *);
સ્થિર પૂર્ણાંક (*py3_PyType_IsSubtype)(PyTypeObject *, PyTypeObject *);
# ifdef USE_LIMITED_API
સ્થિર શૂન્ય* (*py3_PyType_GetSlot)(PyTypeObject *, પૂર્ણાંક);
સ્થિર PyObject* (*py3_PyType_FromSpec)(PyType_Spec *);
# endif

// Imported exception objects
સ્થિર PyObject *p3imp_PyExc_AttributeError;
સ્થિર PyObject *p3imp_PyExc_IndexError;
સ્થિર PyObject *p3imp_Py_Exc_ચાવી_Error;
સ્થિર PyObject *p3imp_PyExc_KeyboardInterrupt;
સ્થિર PyObject *p3imp_PyExc_TypeError;
સ્થિર PyObject *p3imp_Py_Exc_કિંમત_Error;
સ્થિર PyObject *p3imp_PyExc_SystemExit;
સ્થિર PyObject *p3imp_PyExc_RuntimeError;
સ્થિર PyObject *p3imp_PyExc_ImportError;
સ્થિર PyObject *p3imp_PyExc_OverflowError;

# define PyExc_AttributeError p3imp_PyExc_AttributeError
# define PyExc_IndexError p3imp_PyExc_IndexError
# define Py_Exc_ચાવી_Error p3imp_Py_Exc_ચાવી_Error
# define PyExc_KeyboardInterrupt p3imp_PyExc_KeyboardInterrupt
# define PyExc_TypeError p3imp_PyExc_TypeError
# define Py_Exc_કિંમત_Error p3imp_Py_Exc_કિંમત_Error
# define PyExc_SystemExit p3imp_PyExc_SystemExit
# define PyExc_RuntimeError p3imp_PyExc_RuntimeError
# define PyExc_ImportError p3imp_PyExc_ImportError
# define PyExc_OverflowError p3imp_PyExc_OverflowError

/*
 * Table of name to function pointer of python.
 */
# define PYTHON_PROC FARPROC
સ્થિર સંરચના
{
    અક્ષર *name;
    PYTHON_PROC *ptr;
} py3_funcname_table[] =
{
    {"PySys_SetArgv", (PYTHON_PROC*)&py3_PySys_SetArgv},
    {"Py_SetPythonHome", (PYTHON_PROC*)&py3_Py_SetPythonHome},
    {"Py_Initialize", (PYTHON_PROC*)&py3_Py_Initialize},
    {"_PyArg_ParseTuple_SizeT", (PYTHON_PROC*)&py3_PyArg_ParseTuple},
    {"_Py_BuildValue_SizeT", (PYTHON_PROC*)&py3_Py_Build_કિંમત},
    {"PyMem_Free", (PYTHON_PROC*)&py3_PyMem_Free},
    {"PyMem_Malloc", (PYTHON_PROC*)&py3_PyMem_Malloc},
    {"PyList_New", (PYTHON_PROC*)&py3_PyList_New},
    {"PyGILState_Ensure", (PYTHON_PROC*)&py3_PyGILState_Ensure},
    {"PyGILState_Release", (PYTHON_PROC*)&py3_PyGILState_Release},
    {"PySys_SetObject", (PYTHON_PROC*)&py3_PySys_SetObject},
    {"PySys_GetObject", (PYTHON_PROC*)&py3_PySys_GetObject},
    {"PyList_Append", (PYTHON_PROC*)&py3_PyList_Append},
    {"PyList_Insert", (PYTHON_PROC*)&py3_Py_List_ઉમેરો},
    {"PyList_Size", (PYTHON_PROC*)&py3_PyList_Size},
    {"PyTuple_New", (PYTHON_PROC*)&py3_PyTuple_New},
    {"PyTuple_GetItem", (PYTHON_PROC*)&py3_PyTuple_GetItem},
    {"PyTuple_SetItem", (PYTHON_PROC*)&py3_PyTuple_SetItem},
    {"PyTuple_Size", (PYTHON_PROC*)&py3_PyTuple_Size},
    {"PySequence_Check", (PYTHON_PROC*)&py3_PySequence_Check},
    {"PySequence_Size", (PYTHON_PROC*)&py3_PySequence_Size},
    {"PySequence_GetItem", (PYTHON_PROC*)&py3_PySequence_GetItem},
    {"PySequence_Fast", (PYTHON_PROC*)&py3_PySequence_Fast},
# જો PY_VERSION_HEX >= 0x030601f0
    {"PySlice_AdjustIndices", (PYTHON_PROC*)&py3_PySlice_AdjustIndices},
    {"PySlice_Unpack", (PYTHON_PROC*)&py3_PySlice_Unpack},
# endif
    {"PySlice_GetIndicesEx", (PYTHON_PROC*)&py3_PySlice_GetIndicesEx},
    {"PyErr_NoMemory", (PYTHON_PROC*)&py3_PyErr_NoMemory},
    {"Py_Finalize", (PYTHON_PROC*)&py3_Py_Finalize},
    {"PyErr_SetString", (PYTHON_PROC*)&py3_PyErr_SetString},
    {"PyErr_SetObject", (PYTHON_PROC*)&py3_PyErr_SetObject},
    {"PyErr_ExceptionMatches", (PYTHON_PROC*)&py3_PyErr_ExceptionMatches},
# જો defined(USE_LIMITED_API) || PY_VERSION_HEX >= 0x03080000
    {"Py_IncRef", (PYTHON_PROC*)&py3_Py_IncRef},
    {"Py_DecRef", (PYTHON_PROC*)&py3_Py_DecRef},
# endif
# ifdef USE_LIMITED_API
    {"Py_CompileString", (PYTHON_PROC*)&py3_Py_CompileString},
    {"PyEval_EvalCode", (PYTHON_PROC*)&PyEval_EvalCode},
# નહીંતર
    {"PyRun_SimpleString", (PYTHON_PROC*)&py3_PyRun_SimpleString},
    {"PyRun_String", (PYTHON_PROC*)&py3_PyRun_String},
# endif
    {"PyObject_GetAttrString", (PYTHON_PROC*)&py3_PyObject_GetAttrString},
    {"PyObject_HasAttrString", (PYTHON_PROC*)&py3_PyObject_HasAttrString},
    {"PyObject_SetAttrString", (PYTHON_PROC*)&py3_PyObject_SetAttrString},
    {"PyObject_CallFunctionObjArgs", (PYTHON_PROC*)&py3_PyObject_CallFunctionObjArgs},
# જો PY_VERSION_HEX >= 0x030d0000
    {"PyObject_CallFunction", (PYTHON_PROC*)&py3_PyObject_CallFunction},
# નહીંતર
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
    {"PyMapping_Keys", (PYTHON_PROC*)&py3_Py_Mapping_ચાવીઓ},
# જો (defined(USE_LIMITED_API) && Py_LIMITED_API >= 0x03080000) || \
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
# જો PY_VERSION_HEX >= 0x03040000
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
# જો !defined(USE_LIMITED_API) && PY_VERSION_HEX < 0x030D0000
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
#  જો Py_LIMITED_API >= 0x030a0000
    {"PyUnicode_AsUTF8AndSize", (PYTHON_PROC*)&py3_PyUnicode_AsUTF8AndSize},
#  endif
# નહીંતર
#  જો PY_VERSION_HEX >= 0x030300f0
    {"PyUnicode_AsUTF8AndSize", (PYTHON_PROC*)&py3_PyUnicode_AsUTF8AndSize},
#  નહીંતર
    {"_PyUnicode_AsString", (PYTHON_PROC*)&py3__PyUnicode_AsString},
#  endif
# endif
    {"PyUnicode_CompareWithASCIIString", (PYTHON_PROC*)&py3_PyUnicode_CompareWithASCIIString},
    {"PyUnicode_AsUTF8String", (PYTHON_PROC*)&py3_PyUnicode_AsUTF8String},
# ifdef Py_UNICODE_USE_UCS_FUNCTIONS
#  ifdef Py_UNICODE_WIDE
    {"PyUnicodeUCS4_FromFormat", (PYTHON_PROC*)&py3_PyUnicodeUCS4_FromFormat},
#  નહીંતર
    {"PyUnicodeUCS2_FromFormat", (PYTHON_PROC*)&py3_PyUnicodeUCS2_FromFormat},
#  endif
# નહીંતર
    {"PyUnicode_FromFormat", (PYTHON_PROC*)&py3_PyUnicode_FromFormat},
# endif
    {"PyBytes_AsString", (PYTHON_PROC*)&py3_PyBytes_AsString},
    {"PyBytes_AsStringAndSize", (PYTHON_PROC*)&py3_PyBytes_AsStringAndSize},
    {"PyBytes_FromString", (PYTHON_PROC*)&py3_PyBytes_FromString},
    {"PyBytes_FromStringAndSize", (PYTHON_PROC*)&py3_PyBytes_FromStringAndSize},
# જો PY_VERSION_HEX >= 0x030900b0
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
    {"PyNumber_Check", (PYTHON_PROC*)&py3_Py_સંખ્યા_Check},
    {"PyNumber_Long", (PYTHON_PROC*)&py3_Py_સંખ્યા_Long},
    {"PyErr_NewException", (PYTHON_PROC*)&py3_PyErr_NewException},
# ifdef Py_DEBUG
    {"_Py_NegativeRefcount", (PYTHON_PROC*)&py3__Py_NegativeRefcount},
    {"_Py_RefTotal", (PYTHON_PROC*)&py3__Py_RefTotal},
    {"PyModule_Create2TraceRefs", (PYTHON_PROC*)&py3_PyModule_Create2TraceRefs},
# નહીંતર
    {"PyModule_Create2", (PYTHON_PROC*)&py3_PyModule_Create2},
# endif
# જો defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
    {"_PyObject_DebugFree", (PYTHON_PROC*)&py3__PyObject_DebugFree},
    {"_PyObject_DebugMalloc", (PYTHON_PROC*)&py3__PyObject_DebugMalloc},
# નહીંતર
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
#  જો PY_VERSION_HEX >= 0x03040000
    {"PyType_GetSlot", (PYTHON_PROC*)&py3_PyType_GetSlot},
#  endif
    {"PyType_FromSpec", (PYTHON_PROC*)&py3_PyType_FromSpec},
# endif
    {"", ખાલી},
};

# જો defined(USE_LIMITED_API) || PY_VERSION_HEX >= 0x03080000
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

# જો PY_VERSION_HEX >= 0x030900b0
    સ્થિર અંતઃરેખ પૂર્ણાંક
py3_PyType_HasFeature(PyTypeObject *type, unsigned long feature)
{
    પાછા ((PyType_GetFlags(type) & feature) != 0);
}
#  define PyType_HasFeature(t,f) py3_PyType_HasFeature(t,f)
# endif

# જો PY_VERSION_HEX >= 0x030a00b2
    સ્થિર અંતઃરેખ પૂર્ણાંક
py3__PyObject_TypeCheck(PyObject *ob, PyTypeObject *type)
{
    પાછા Py_IS_TYPE(ob, type) || PyType_IsSubtype(Py_TYPE(ob), type);
}
#  જો PY_VERSION_HEX >= 0x030b00b3
#   undef PyObject_TypeCheck
#   define PyObject_TypeCheck(o,t) py3__PyObject_TypeCheck(o,t)
#  નહીંતર
#   define _PyObject_TypeCheck(o,t) py3__PyObject_TypeCheck(o,t)
#  endif
# endif

# જો !defined(USE_LIMITED_API) && PY_VERSION_HEX >= 0x030c00b0
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
    સ્થિર WCHAR *
py3_get_system_libname(અચલ અક્ષર *libname)
{
    અચલ WCHAR	*pythoncore = L"Software\\Python\\PythonCore";
    અચલ અક્ષર	*cp = libname;
    WCHAR	subkey[128];
    HKEY	h_ચાવી;
    પૂર્ણાંક		i;
    DWORD	j, len;
    LSTATUS	ret;

    જ્યારે (*cp != '\0')
    {
	જો (*cp == ':' || *cp == '\\' || *cp == '/')
	{
	    // Bail out if "libname" contains path separator, assume it is
	    // an absolute path.
	    પાછા ખાલી;
	}
	++cp;
    }

    WCHAR   keyfound[32];
    HKEY    h_ચાવી_ટોચ[] = {HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE};
    HKEY    h_ચાવી_Found = ખાલી;
#  ifdef USE_LIMITED_API
    long    maxminor = -1;
#  endif
    માટે (i = 0; i < ARRAY_LENGTH(h_ચાવી_ટોચ); i++)
    {
	long	major, minor;

	ret = Reg_Open_ચાવી_Ex_W(h_ચાવી_ટોચ[i], pythoncore, 0, ચાવી_READ, &h_ચાવી);
	જો (ret != ERROR_SUCCESS)
	    ચાલુ;
	માટે (j = 0;; j++)
	{
	    WCHAR   keyname[32];
	    WCHAR   *wp;

	    len = ARRAY_LENGTH(keyname);
	    ret = Reg_Enum_ચાવી_Ex_W(h_ચાવી, j, keyname, &len,
						    ખાલી, ખાલી, ખાલી, ખાલી);
	    જો (ret == ERROR_NO_MORE_વસ્તુઓ)
		તોડો;

	    major = wcstol(keyname, &wp, 10);
	    જો (*wp != L'.')
		ચાલુ;
	    minor = wcstol(wp + 1, &wp, 10);
#  ifdef _WIN64
	    જો (*wp != L'\0')
		ચાલુ;
#  નહીંતર
	    જો (wcscmp(wp, L"-32") != 0)
		ચાલુ;
#  endif

	    જો (major != PY_MAJOR_VERSION)
		ચાલુ;
#  ifdef USE_LIMITED_API
	    // Search the latest version.
	    જો ((minor > maxminor)
		    && (minor >= ((Py_LIMITED_API >> 16) & 0xff)))
	    {
		maxminor = minor;
		wcscpy(keyfound, keyname);
		h_ચાવી_Found = h_ચાવી_ટોચ[i];
	    }
#  નહીંતર
	    // Check if it matches with the compiled version.
	    જો (minor == PY_MINOR_VERSION)
	    {
		wcscpy(keyfound, keyname);
		h_ચાવી_Found = h_ચાવી_ટોચ[i];
		તોડો;
	    }
#  endif
	}
	Reg_Close_ચાવી(h_ચાવી);
#  ifdef USE_LIMITED_API
	જો (h_ચાવી_Found != ખાલી)
	    તોડો;
#  endif
    }
    જો (h_ચાવી_Found == ખાલી)
	પાછા ખાલી;

    swprintf(subkey, ARRAY_LENGTH(subkey), L"%ls\\%ls\\InstallPath",
							pythoncore, keyfound);
    ret = Reg_Get_કિંમત_W(h_ચાવી_Found, subkey, ખાલી, RRF_RT_REG_SZ,
							    ખાલી, ખાલી, &len);
    જો (ret != ERROR_MORE_DATA && ret != ERROR_SUCCESS)
	પાછા ખાલી;
    size_t len2 = len / કદ(WCHAR) + 1 + strlen(libname);
    WCHAR *path = alloc(len2 * કદ(WCHAR));
    જો (path == ખાલી)
	પાછા ખાલી;
    ret = Reg_Get_કિંમત_W(h_ચાવી_Found, subkey, ખાલી, RRF_RT_REG_SZ,
							    ખાલી, path, &len);
    જો (ret != ERROR_SUCCESS)
    {
	vim_free(path);
	પાછા ખાલી;
    }
    // Remove trailing path separators.
    size_t len3 = wcslen(path);
    જો ((len3 > 0) && (path[len3 - 1] == L'/' || path[len3 - 1] == L'\\'))
	--len3;
    swprintf(path + len3, len2 - len3, L"\\%hs", libname);
    પાછા path;
}
# endif

/*
 * Load library and get all pointers.
 * Parameter 'libname' provides name of DLL.
 * Return OK or FAIL.
 */
    સ્થિર પૂર્ણાંક
py3_runtime_link_init(અક્ષર *libname, પૂર્ણાંક verbose)
{
    પૂર્ણાંક i;
    PYTHON_PROC *ucs_from_string = (PYTHON_PROC *)&py3_PyUnicode_FromString;
    PYTHON_PROC *ucs_decode = (PYTHON_PROC *)&py3_PyUnicode_Decode;
    PYTHON_PROC *ucs_as_encoded_string =
				 (PYTHON_PROC *)&py3_PyUnicode_AsEncodedString;

# જો !(defined(PY_NO_RTLD_GLOBAL) && defined(PY3_NO_RTLD_GLOBAL)) && defined(UNIX) && defined(FEAT_PYTHON)
    // Can't have Python and Python3 loaded at the same time.
    // It causes a crash, because RTLD_GLOBAL is needed for
    // standard C extension libraries of one or both python versions.
    જો (python_loaded())
    {
	જો (verbose)
	    emsg(_(e_this_vim_cannot_execute_py3_after_using_python));
	પાછા FAIL;
    }
# endif

    જો (hinstPy3 != 0)
	પાછા OK;
    hinstPy3 = load_dll(libname);

# ifdef MSWIN
    જો (!hinstPy3)
    {
	// Attempt to use the path from InstallPath as stored in the registry.
	WCHAR *syslibname = py3_get_system_libname(libname);

	જો (syslibname != ખાલી)
	{
	    hinstPy3 = LoadLibraryExW(syslibname, ખાલી,
		    LOAD_LIBRARY_શોધ_DLL_LOAD_DIR |
		    LOAD_LIBRARY_શોધ_SYSTEM32);
	    vim_free(syslibname);
	}
    }
# endif

    જો (!hinstPy3)
    {
	જો (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	પાછા FAIL;
    }

    માટે (i = 0; py3_funcname_table[i].ptr; ++i)
    {
	જો ((*py3_funcname_table[i].ptr = પ્રતીક_from_dll(hinstPy3,
			py3_funcname_table[i].name)) == ખાલી)
	{
	    close_dll(hinstPy3);
	    hinstPy3 = 0;
	    જો (verbose)
		semsg(_(e_could_not_load_library_function_str), py3_funcname_table[i].name);
	    પાછા FAIL;
	}
    }

    // Load unicode functions separately as only the ucs2 or the ucs4 functions
    // will be present in the library.
# જો PY_VERSION_HEX >= 0x030300f0
    *ucs_from_string = પ્રતીક_from_dll(hinstPy3, "PyUnicode_FromString");
    *ucs_decode = પ્રતીક_from_dll(hinstPy3, "PyUnicode_Decode");
    *ucs_as_encoded_string = પ્રતીક_from_dll(hinstPy3,
	    "PyUnicode_AsEncodedString");
# નહીંતર
    *ucs_from_string = પ્રતીક_from_dll(hinstPy3, "PyUnicodeUCS2_FromString");
    *ucs_decode = પ્રતીક_from_dll(hinstPy3,
	    "PyUnicodeUCS2_Decode");
    *ucs_as_encoded_string = પ્રતીક_from_dll(hinstPy3,
	    "PyUnicodeUCS2_AsEncodedString");
    જો (*ucs_from_string == ખાલી || *ucs_decode == ખાલી
					     || *ucs_as_encoded_string == ખાલી)
    {
	*ucs_from_string = પ્રતીક_from_dll(hinstPy3,
		"PyUnicodeUCS4_FromString");
	*ucs_decode = પ્રતીક_from_dll(hinstPy3,
		"PyUnicodeUCS4_Decode");
	*ucs_as_encoded_string = પ્રતીક_from_dll(hinstPy3,
		"PyUnicodeUCS4_AsEncodedString");
    }
# endif
    જો (*ucs_from_string == ખાલી || *ucs_decode == ખાલી
					     || *ucs_as_encoded_string == ખાલી)
    {
	close_dll(hinstPy3);
	hinstPy3 = 0;
	જો (verbose)
	    semsg(_(e_could_not_load_library_function_str), "PyUnicode_UCSX_*");
	પાછા FAIL;
    }

    પાછા OK;
}

/*
 * If python is enabled (there is installed python on Windows system) return
 * TRUE, else FALSE.
 */
    પૂર્ણાંક
python3_enabled(પૂર્ણાંક verbose)
{
    પાછા py3_runtime_link_init((અક્ષર *)p_py3dll, verbose) == OK;
}

/*
 * Load the standard Python exceptions - don't import the symbols from the
 * DLL, as this can cause errors (importing data symbols is not reliable).
 */
    સ્થિર શૂન્ય
get_py3_exceptions(શૂન્ય)
{
    PyObject *exmod = PyImport_ImportModule("builtins");
    PyObject *exdict = PyModule_GetDict(exmod);
    p3imp_PyExc_AttributeError = PyDict_GetItemString(exdict, "AttributeError");
    p3imp_PyExc_IndexError = PyDict_GetItemString(exdict, "IndexError");
    p3imp_Py_Exc_ચાવી_Error = PyDict_GetItemString(exdict, "KeyError");
    p3imp_PyExc_KeyboardInterrupt = PyDict_GetItemString(exdict, "KeyboardInterrupt");
    p3imp_PyExc_TypeError = PyDict_GetItemString(exdict, "TypeError");
    p3imp_Py_Exc_કિંમત_Error = PyDict_GetItemString(exdict, "ValueError");
    p3imp_PyExc_SystemExit = PyDict_GetItemString(exdict, "SystemExit");
    p3imp_PyExc_RuntimeError = PyDict_GetItemString(exdict, "RuntimeError");
    p3imp_PyExc_ImportError = PyDict_GetItemString(exdict, "ImportError");
    p3imp_PyExc_OverflowError = PyDict_GetItemString(exdict, "OverflowError");
    Py_XINCREF(p3imp_PyExc_AttributeError);
    Py_XINCREF(p3imp_PyExc_IndexError);
    Py_XINCREF(p3imp_Py_Exc_ચાવી_Error);
    Py_XINCREF(p3imp_PyExc_KeyboardInterrupt);
    Py_XINCREF(p3imp_PyExc_TypeError);
    Py_XINCREF(p3imp_Py_Exc_કિંમત_Error);
    Py_XINCREF(p3imp_PyExc_SystemExit);
    Py_XINCREF(p3imp_PyExc_RuntimeError);
    Py_XINCREF(p3imp_PyExc_ImportError);
    Py_XINCREF(p3imp_PyExc_OverflowError);
    Py_XDECREF(exmod);
}
#endif // DYNAMIC_PYTHON3

સ્થિર પૂર્ણાંક py3initialised = 0;
#define PYINITIALISED py3initialised
સ્થિર પૂર્ણાંક python_end_called = FALSE;

#ifdef USE_LIMITED_API
# define DESTRUCTOR_FINISH(self) \
    ((freefunc)PyType_GetSlot(Py_TYPE((PyObject*)self), Py_tp_free))((PyObject*)self)
#નહીંતર
# define DESTRUCTOR_FINISH(self) Py_TYPE(self)->tp_free((PyObject*)self)
#endif

#define WIN_PYTHON_REF(win) win->w_python3_ref
#define BUF_PYTHON_REF(buf) buf->b_python3_ref
#define TAB_PYTHON_REF(tab) tab->tp_python3_ref

    સ્થિર શૂન્ય
call_PyObject_Free(શૂન્ય *p)
{
#જો defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
    _PyObject_DebugFree(p);
#નહીંતર
    PyObject_Free(p);
#endif
}

    સ્થિર PyObject *
call_PyType_GenericNew(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    પાછા PyType_GenericNew(type,args,kwds);
}

    સ્થિર PyObject *
call_PyType_GenericAlloc(PyTypeObject *type, Py_ssize_t nitems)
{
    પાછા PyType_GenericAlloc(type,nitems);
}

સ્થિર PyObject *OutputGetattro(PyObject *, PyObject *);
સ્થિર પૂર્ણાંક OutputSetattro(PyObject *, PyObject *, PyObject *);
સ્થિર PyObject *BufferGetattro(PyObject *, PyObject *);
સ્થિર પૂર્ણાંક BufferSetattro(PyObject *, PyObject *, PyObject *);
સ્થિર PyObject *TabPageGetattro(PyObject *, PyObject *);
સ્થિર PyObject *WindowGetattro(PyObject *, PyObject *);
સ્થિર પૂર્ણાંક WindowSetattro(PyObject *, PyObject *, PyObject *);
સ્થિર PyObject *RangeGetattro(PyObject *, PyObject *);
સ્થિર PyObject *CurrentGetattro(PyObject *, PyObject *);
સ્થિર પૂર્ણાંક CurrentSetattro(PyObject *, PyObject *, PyObject *);
સ્થિર PyObject *DictionaryGetattro(PyObject *, PyObject *);
સ્થિર પૂર્ણાંક DictionarySetattro(PyObject *, PyObject *, PyObject *);
સ્થિર PyObject *ListGetattro(PyObject *, PyObject *);
સ્થિર પૂર્ણાંક ListSetattro(PyObject *, PyObject *, PyObject *);
સ્થિર PyObject *TupleGetattro(PyObject *, PyObject *);
સ્થિર પૂર્ણાંક TupleSetattro(PyObject *, PyObject *, PyObject *);
સ્થિર PyObject *FunctionGetattro(PyObject *, PyObject *);

સ્થિર સંરચના PyModuleDef vimmodule;

#define PY_CAN_RECURSE

/*
 * Include the code shared with if_python.c
 */
#અન્તર્ભાવ "if_py_both.h"

#ifdef USE_LIMITED_API
# જો Py_LIMITED_API >= 0x030A0000
#  define PY_UNICODE_GET_UTF8_CHARS(obj) PyUnicode_AsUTF8AndSize(obj, ખાલી)
# નહીંતર
// Python limited API before 3.10 lack easy ways to query the raw UTF-8 chars.
// We need to first convert the string to bytes, and then extract the chars.
// This function is only used for attribute string comparisons, which have
// known short length. As such, just allocate a short static buffer to hold
// the characters instead of having to allocate/deallcoate it.
//
// An alternative would be to convert all attribute string comparisons to use
// PyUnicode_CompareWithASCIIString to skip having to extract the chars.
સ્થિર અક્ષર py3_unicode_utf8_chars[20];
સ્થિર અક્ષર* PY_UNICODE_GET_UTF8_CHARS(PyObject* str)
{
    py3_unicode_utf8_chars[0] = '\0';
    PyObject* bytes = PyUnicode_AsUTF8String(str);
    જો (bytes)
    {
	અક્ષર *chars;
	Py_ssize_t len;
	જો (PyBytes_AsStringAndSize(bytes, &chars, &len) != -1)
	{
	    જો (len < (Py_ssize_t)કદ(py3_unicode_utf8_chars))
		// PyBytes_AsStringAndSize guarantees null-termination
		memcpy(py3_unicode_utf8_chars, chars, len + 1);
	}
	Py_DECREF(bytes);
    }
    પાછા py3_unicode_utf8_chars;
}
# endif
#નહીંતર	// !USE_LIMITED_API
# જો PY_VERSION_HEX >= 0x030300f0
#  define PY_UNICODE_GET_UTF8_CHARS(obj) PyUnicode_AsUTF8AndSize(obj, ખાલી)
# નહીંતર
#  define PY_UNICODE_GET_UTF8_CHARS _PyUnicode_AsString
# endif
#endif

// NOTE: Must always be used at the start of a block, since it declares "name".
#define GET_ATTR_STRING(name, nameobj) \
    અક્ષર	*name = ""; \
    જો (PyUnicode_Check(nameobj)) \
	name = (અક્ષર *)PY_UNICODE_GET_UTF8_CHARS(nameobj)

#define PY3OBJ_DELETED(obj) (obj->ob_base.ob_refcnt<=0)

///////////////////////////////////////////////////////
// Internal function prototypes.

સ્થિર PyObject *Py3Init_vim(શૂન્ય);

///////////////////////////////////////////////////////
// 1. Python interpreter main program.

    શૂન્ય
python3_end(શૂન્ય)
{
    સ્થિર પૂર્ણાંક recurse = 0;

    // If a crash occurs while doing this, don't try again.
    જો (recurse != 0)
	પાછા;

    python_end_called = TRUE;
    ++recurse;

#ifdef DYNAMIC_PYTHON3
    જો (hinstPy3)
#endif
    જો (Py_IsInitialized())
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

#જો defined(DYNAMIC_PYTHON3) && defined(DYNAMIC_PYTHON) && defined(FEAT_PYTHON) && defined(UNIX)
    પૂર્ણાંક
python3_loaded(શૂન્ય)
{
    પાછા (hinstPy3 != 0);
}
#endif

સ્થિર wchar_t *py_home_buf = ખાલી;

#જો defined(MSWIN) && (PY_VERSION_HEX >= 0x030500f0)
/*
 * Return TRUE if stdin is readable from Python 3.
 */
    સ્થિર BOOL
is_stdin_readable(શૂન્ય)
{
    DWORD	    mode, eventnum;
    સંરચના _stat    st;
    પૂર્ણાંક		    fd = fileno(stdin);
    HANDLE	    hstdin = (HANDLE)_get_osfhandle(fd);

    // Check if stdin is connected to the console.
    જો (GetConsoleMode(hstdin, &mode))
	// Check if it is opened as input.
	પાછા Get_સંખ્યા_Of_Console_આગત_Events(hstdin, &eventnum);

    પાછા _fstat(fd, &st) == 0;
}

// Python 3.5 or later will abort inside Py_Initialize() when stdin has
// been closed (i.e. executed by "vim -").  Reconnect stdin to CONIN$.
// Note that the python DLL is linked to its own stdio DLL which can be
// differ from Vim's stdio.
    સ્થિર શૂન્ય
reset_stdin(શૂન્ય)
{
    FILE *(*py__acrt_iob_func)(unsigned) = ખાલી;
    FILE *(*pyfreopen)(અચલ અક્ષર *, અચલ અક્ષર *, FILE *) = ખાલી;
    HINSTANCE hinst = get_forwarded_dll(hinstPy3);

    જો (hinst == ખાલી || is_stdin_readable())
	પાછા;

    // Get "freopen" and "stdin" which are used in the python DLL.
    // "stdin" is defined as "__acrt_iob_func(0)" in VC++ 2015 or later.
    py__acrt_iob_func = get_dll_import_func(hinst, "__acrt_iob_func");
    જો (py__acrt_iob_func)
    {
	HINSTANCE hpystdiodll = find_imported_module_by_funcname(hinst,
							    "__acrt_iob_func");
	જો (hpystdiodll)
	    pyfreopen = (શૂન્ય *)GetProcAddress(hpystdiodll, "freopen");
    }

    // Reconnect stdin to CONIN$.
    જો (pyfreopen != ખાલી)
	pyfreopen("CONIN$", "r", py__acrt_iob_func(0));
    નહીંતર
	freopen("CONIN$", "r", stdin);
}
#નહીંતર
# define reset_stdin()
#endif

// Python 3.2 or later will abort inside Py_Initialize() when mandatory
// modules cannot be loaded (e.g. 'pythonthreehome' is wrongly set.).
// Install a hook to python dll's exit() and recover from it.
#જો defined(MSWIN) && (PY_VERSION_HEX >= 0x030200f0)
# define HOOK_EXIT
# include <setjmp.h>

સ્થિર jmp_buf exit_hook_jump_buf;
સ્થિર શૂન્ય *orig_exit = ખાલી;

/*
 * Function that replaces exit() while calling Py_Initialize().
 */
    સ્થિર શૂન્ય
hooked_exit(પૂર્ણાંક ret)
{
    // Recover from exit.
    longjmp(exit_hook_jump_buf, 1);
}

/*
 * Install a hook to python dll's exit().
 */
    સ્થિર શૂન્ય
hook_py_exit(શૂન્ય)
{
    HINSTANCE hinst = get_forwarded_dll(hinstPy3);

    જો (hinst == ખાલી || orig_exit != ખાલી)
	પાછા;

    orig_exit = hook_dll_import_func(hinst, "exit", (શૂન્ય *)hooked_exit);
}

/*
 * Remove the hook installed by hook_py_exit().
 */
    સ્થિર શૂન્ય
restore_py_exit(શૂન્ય)
{
    HINSTANCE hinst = hinstPy3;

    જો (hinst == ખાલી)
	પાછા;

    જો (orig_exit != ખાલી)
	hook_dll_import_func(hinst, "exit", orig_exit);
    orig_exit = ખાલી;
}
#endif

    સ્થિર પૂર્ણાંક
Python3_Init(શૂન્ય)
{
    જો (!py3initialised)
    {
#ifdef DYNAMIC_PYTHON3
	જો (!python3_enabled(TRUE))
	{
	    emsg(_(e_sorry_this_command_is_disabled_python_library_could_not_be_found));
	    જાઓ fail;
	}
#endif
	// Python 3.13 introduced a really useful feature: colorized exceptions.
	// This is great if you're reading them from the terminal, but useless
	// and broken everywhere else (such as in log files, or text editors).
	// Opt out, forcefully.
	vim_setenv((char_u*)"PYTHON_COLORS", (char_u*)"0");

	init_structs();

	જો (*p_py3home != NUL)
	{
	    size_t len = mbstowcs(ખાલી, (અક્ષર *)p_py3home, 0) + 1;

	    // The string must not change later, make a copy in static memory.
	    py_home_buf = ALLOC_MULT(wchar_t, len);
	    જો (py_home_buf != ખાલી && mbstowcs(
			    py_home_buf, (અક્ષર *)p_py3home, len) != (size_t)-1)
		Py_SetPythonHome(py_home_buf);
	}
#ifdef PYTHON3_HOME
	નહીંતર જો (mch_getenv((char_u *)"PYTHONHOME") == ખાલી)
	    Py_SetPythonHome(PYTHON3_HOME);
#endif

	PyImport_AppendInittab("vim", Py3Init_vim);

#જો !defined(DYNAMIC_PYTHON3) && defined(MSWIN)
	hinstPy3 = GetModuleHandle(PYTHON3_DLL);
#endif
	reset_stdin();

#ifdef HOOK_EXIT
	// Catch exit() called in Py_Initialize().
	hook_py_exit();
	જો (setjmp(exit_hook_jump_buf) == 0)
	{
	    Py_Initialize();
	    restore_py_exit();
	}
	નહીંતર
	{
	    // exit() was called in Py_Initialize().
	    restore_py_exit();
	    emsg(_(e_critical_error_in_python3_initialization_check_your_installation));
	    જાઓ fail;
	}
#નહીંતર
	Py_Initialize();
#endif

#જો PY_VERSION_HEX < 0x03090000
	// Initialise threads.  This is deprecated since Python 3.9.
	PyEval_InitThreads();
#endif
#ifdef DYNAMIC_PYTHON3
	get_py3_exceptions();
#endif

	જો (PythonIO_Init_io())
	    જાઓ fail;

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

    પાછા 0;

fail:
    // We call PythonIO_Flush() here to print any Python errors.
    // This is OK, as it is possible to call this function even
    // if PythonIO_Init_io() has not completed successfully (it will
    // not do anything in this case).
    PythonIO_Flush();
    પાછા -1;
}

/*
 * External interface
 */
    સ્થિર શૂન્ય
DoPyCommand(અચલ અક્ષર *cmd,
	    dict_T* locals,
	    rangeinitializer init_range,
	    runner run,
	    શૂન્ય *arg)
{
#જો defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    અક્ષર		*saved_locale;
#endif
    PyObject		*cmdstr;
    PyObject		*cmdbytes;
    PyGILState_STATE	pygilstate;

    જો (python_end_called)
	જાઓ theend;

    જો (Python3_Init())
	જાઓ theend;

    init_range(arg);

    Python_Release_Vim();	    // leave Vim

#જો defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    // Python only works properly when the LC_NUMERIC locale is "C".
    saved_locale = setlocale(LC_NUMERIC, ખાલી);
    જો (saved_locale == ખાલી || STRCMP(saved_locale, "C") == 0)
	saved_locale = ખાલી;
    નહીંતર
    {
	// Need to make a copy, value may change when setting new locale.
	saved_locale = (અક્ષર *)vim_strsave((char_u *)saved_locale);
	(શૂન્ય)setlocale(LC_NUMERIC, "C");
    }
#endif

    pygilstate = PyGILState_Ensure();

    // PyRun_SimpleString expects a UTF-8 string. Wrong encoding may cause
    // SyntaxError (unicode error).
    cmdstr = PyUnicode_Decode(cmd, strlen(cmd),
					(અક્ષર *)ENC_OPT, ERRORS_DECODE_ARG);
    cmdbytes = PyUnicode_AsEncodedString(cmdstr, "utf-8", ERRORS_ENCODE_ARG);
    Py_XDECREF(cmdstr);

    run(PyBytes_AsString(cmdbytes), locals, arg, &pygilstate);
    Py_XDECREF(cmdbytes);

    PyGILState_Release(pygilstate);

#જો defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    જો (saved_locale != ખાલી)
    {
	(શૂન્ય)setlocale(LC_NUMERIC, saved_locale);
	vim_free(saved_locale);
    }
#endif

    Python_Lock_Vim();		    // enter Vim
    PythonIO_Flush();

theend:
    પાછા;	    // keeps lint happy
}

/*
 * ":py3"
 */
    શૂન્ય
ex_py3(exarg_T *eap)
{
    char_u *script;

    script = script_get(eap, eap->arg);
    જો (!eap->skip)
    {
	જો (p_pyx == 0)
	    p_pyx = 3;

	DoPyCommand(script == ખાલી ? (અક્ષર *) eap->arg : (અક્ષર *) script,
		ખાલી,
		init_range_cmd,
		(runner) run_cmd,
		(શૂન્ય *) eap);
    }
    vim_free(script);
}

#define BUFFER_SIZE 2048

/*
 * ":py3file"
 */
    શૂન્ય
ex_py3file(exarg_T *eap)
{
    સ્થિર અક્ષર buffer[BUFFER_SIZE];
    અચલ અક્ષર *file;
    અક્ષર *p;
    પૂર્ણાંક i;

    જો (p_pyx == 0)
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

    માટે (i=0; i<2; ++i)
    {
	file = (અક્ષર *)eap->arg;
	જ્યારે (*file && p < buffer + (BUFFER_SIZE - 3))
	{
	    જો (*file == '\\' || *file == '\'')
		*p++ = '\\';
	    *p++ = *file++;
	}
	// If we didn't finish the file name, we hit a buffer overflow
	જો (*file != '\0')
	    પાછા;
	જો (i==0)
	{
	    strcpy(p,"','rb').read(),'");
	    p += 16;
	}
	નહીંતર
	{
	    strcpy(p,"','exec'))");
	    p += 10;
	}
    }


    // Execute the file
    DoPyCommand(buffer,
	    ખાલી,
	    init_range_cmd,
	    (runner) run_cmd,
	    (શૂન્ય *) eap);
}

    શૂન્ય
ex_py3do(exarg_T *eap)
{
    જો (p_pyx == 0)
	p_pyx = 3;

    DoPyCommand((અક્ષર *)eap->arg,
	    ખાલી,
	    init_range_cmd,
	    (runner)run_do,
	    (શૂન્ય *)eap);
}

///////////////////////////////////////////////////////
// 2. Python output stream: writes output via [e]msg().

// Implementation functions

    સ્થિર PyObject *
OutputGetattro(PyObject *self, PyObject *nameobj)
{
    GET_ATTR_STRING(name, nameobj);

    જો (strcmp(name, "softspace") == 0)
	પાછા PyLong_FromLong(((OutputObject *)(self))->softspace);
    નહીંતર જો (strcmp(name, "errors") == 0)
	પાછા PyString_FromString("strict");
    નહીંતર જો (strcmp(name, "encoding") == 0)
	પાછા PyString_FromString(ENC_OPT);

    પાછા PyObject_GenericGetAttr(self, nameobj);
}

    સ્થિર પૂર્ણાંક
OutputSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);

    પાછા OutputSetattr(self, name, val);
}

///////////////////////////////////////////////////////
// 3. Implementation of the Vim module for Python

// Window type - Implementation functions
// --------------------------------------

#define WindowType_Check(obj) ((obj)->ob_base.ob_type == &WindowType)

// Buffer type - Implementation functions
// --------------------------------------

#define BufferType_Check(obj) ((obj)->ob_base.ob_type == &BufferType)

સ્થિર PyObject* BufferSubscript(PyObject *self, PyObject *idx);
સ્થિર પૂર્ણાંક BufferAsSubscript(PyObject *self, PyObject *idx, PyObject *val);

// Line range type - Implementation functions
// --------------------------------------

#define RangeType_Check(obj) ((obj)->ob_base.ob_type == &RangeType)

સ્થિર PyObject* RangeSubscript(PyObject *self, PyObject *idx);
સ્થિર પૂર્ણાંક RangeAsItem(PyObject *, Py_ssize_t, PyObject *);
સ્થિર પૂર્ણાંક RangeAsSubscript(PyObject *self, PyObject *idx, PyObject *val);

// Current objects type - Implementation functions
// -----------------------------------------------

સ્થિર PySequenceMethods BufferAsSeq = {
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

સ્થિર PyMappingMethods BufferAsMapping = {
    /* mp_length	*/ (lenfunc)BufferLength,
    /* mp_subscript     */ (binaryfunc)BufferSubscript,
    /* mp_ass_subscript */ (objobjargproc)BufferAsSubscript,
};


// Buffer object

    સ્થિર PyObject *
BufferGetattro(PyObject *self, PyObject *nameobj)
{
    PyObject *r;

    GET_ATTR_STRING(name, nameobj);

    જો ((r = BufferAttrValid((BufferObject *)(self), name)))
	પાછા r;

    જો (CheckBuffer((BufferObject *)(self)))
	પાછા ખાલી;

    r = BufferAttr((BufferObject *)(self), name);
    જો (r || PyErr_Occurred())
	પાછા r;
    નહીંતર
	પાછા PyObject_GenericGetAttr(self, nameobj);
}

    સ્થિર પૂર્ણાંક
BufferSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);

    પાછા BufferSetattr(self, name, val);
}

//////////////////

    સ્થિર PyObject *
BufferSubscript(PyObject *self, PyObject* idx)
{
    જો (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	પાછા BufferItem((BufferObject *)(self), _idx);
    }
    નહીંતર જો (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	જો (CheckBuffer((BufferObject *) self))
	    પાછા ખાલી;

	જો (PySlice_GetIndicesEx((PySliceObject_T *)idx,
	      (Py_ssize_t)((BufferObject *)(self))->buf->b_ml.ml_લીટી_ગણતરી,
	      &start, &stop,
	      &step, &slicelen) < 0)
	    પાછા ખાલી;
	પાછા BufferSlice((BufferObject *)(self), start, stop);
    }
    નહીંતર
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	પાછા ખાલી;
    }
}

    સ્થિર પૂર્ણાંક
BufferAsSubscript(PyObject *self, PyObject* idx, PyObject* val)
{
    જો (PyLong_Check(idx))
    {
	long n = PyLong_AsLong(idx);

	જો (CheckBuffer((BufferObject *) self))
	    પાછા -1;

	પાછા RBAsItem((BufferObject *)(self), n, val, 1,
		    (Py_ssize_t)((BufferObject *)(self))->buf->b_ml.ml_લીટી_ગણતરી,
		    ખાલી);
    }
    નહીંતર જો (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	જો (CheckBuffer((BufferObject *) self))
	    પાછા -1;

	જો (PySlice_GetIndicesEx((PySliceObject_T *)idx,
	      (Py_ssize_t)((BufferObject *)(self))->buf->b_ml.ml_લીટી_ગણતરી,
	      &start, &stop,
	      &step, &slicelen) < 0)
	    પાછા -1;
	પાછા RBAsSlice((BufferObject *)(self), start, stop, val, 1,
			  (PyInt)((BufferObject *)(self))->buf->b_ml.ml_લીટી_ગણતરી,
			  ખાલી);
    }
    નહીંતર
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	પાછા -1;
    }
}

સ્થિર PySequenceMethods RangeAsSeq = {
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

સ્થિર PyMappingMethods RangeAsMapping = {
    /* mp_length	*/ (lenfunc)RangeLength,
    /* mp_subscript     */ (binaryfunc)RangeSubscript,
    /* mp_ass_subscript */ (objobjargproc)RangeAsSubscript,
};

// Line range object - Implementation

    સ્થિર PyObject *
RangeGetattro(PyObject *self, PyObject *nameobj)
{
    GET_ATTR_STRING(name, nameobj);

    જો (strcmp(name, "start") == 0)
	પાછા Py_Build_કિંમત("n", ((RangeObject *)(self))->start - 1);
    નહીંતર જો (strcmp(name, "end") == 0)
	પાછા Py_Build_કિંમત("n", ((RangeObject *)(self))->end - 1);
    નહીંતર
	પાછા PyObject_GenericGetAttr(self, nameobj);
}

////////////////

    સ્થિર પૂર્ણાંક
RangeAsItem(PyObject *self, Py_ssize_t n, PyObject *val)
{
    પાછા RBAsItem(((RangeObject *)(self))->buf, n, val,
		    ((RangeObject *)(self))->start,
		    ((RangeObject *)(self))->end,
		    &((RangeObject *)(self))->end);
}

    સ્થિર Py_ssize_t
RangeAsSlice(PyObject *self, Py_ssize_t lo, Py_ssize_t hi, PyObject *val)
{
    પાછા RBAsSlice(((RangeObject *)(self))->buf, lo, hi, val,
		    ((RangeObject *)(self))->start,
		    ((RangeObject *)(self))->end,
		    &((RangeObject *)(self))->end);
}

    સ્થિર PyObject *
RangeSubscript(PyObject *self, PyObject* idx)
{
    જો (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	પાછા RangeItem((RangeObject *)(self), _idx);
    }
    નહીંતર જો (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	જો (PySlice_GetIndicesEx((PySliceObject_T *)idx,
		((RangeObject *)(self))->end-((RangeObject *)(self))->start+1,
		&start, &stop,
		&step, &slicelen) < 0)
	    પાછા ખાલી;
	પાછા RangeSlice((RangeObject *)(self), start, stop);
    }
    નહીંતર
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	પાછા ખાલી;
    }
}

    સ્થિર પૂર્ણાંક
RangeAsSubscript(PyObject *self, PyObject *idx, PyObject *val)
{
    જો (PyLong_Check(idx))
    {
	long n = PyLong_AsLong(idx);
	પાછા RangeAsItem(self, n, val);
    }
    નહીંતર જો (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	જો (PySlice_GetIndicesEx((PySliceObject_T *)idx,
		((RangeObject *)(self))->end-((RangeObject *)(self))->start+1,
		&start, &stop,
		&step, &slicelen) < 0)
	    પાછા -1;
	પાછા RangeAsSlice(self, start, stop, val);
    }
    નહીંતર
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	પાછા -1;
    }
}

// TabPage object - Implementation

    સ્થિર PyObject *
TabPageGetattro(PyObject *self, PyObject *nameobj)
{
    PyObject *r;

    GET_ATTR_STRING(name, nameobj);

    જો ((r = TabPageAttrValid((TabPageObject *)(self), name)))
	પાછા r;

    જો (CheckTabPage((TabPageObject *)(self)))
	પાછા ખાલી;

    r = TabPageAttr((TabPageObject *)(self), name);
    જો (r || PyErr_Occurred())
	પાછા r;
    નહીંતર
	પાછા PyObject_GenericGetAttr(self, nameobj);
}

// Window object - Implementation

    સ્થિર PyObject *
WindowGetattro(PyObject *self, PyObject *nameobj)
{
    PyObject *r;

    GET_ATTR_STRING(name, nameobj);

    જો ((r = WindowAttrValid((WindowObject *)(self), name)))
	પાછા r;

    જો (CheckWindow((WindowObject *)(self)))
	પાછા ખાલી;

    r = WindowAttr((WindowObject *)(self), name);
    જો (r || PyErr_Occurred())
	પાછા r;
    નહીંતર
	પાછા PyObject_GenericGetAttr(self, nameobj);
}

    સ્થિર પૂર્ણાંક
WindowSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);

    પાછા WindowSetattr(self, name, val);
}

// Tab page list object - Definitions

સ્થિર PySequenceMethods TabListAsSeq = {
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

સ્થિર PySequenceMethods WinListAsSeq = {
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
    સ્થિર PyObject *
CurrentGetattro(PyObject *self, PyObject *nameobj)
{
    PyObject	*r;
    GET_ATTR_STRING(name, nameobj);
    જો (!(r = CurrentGetattr(self, name)))
	પાછા PyObject_GenericGetAttr(self, nameobj);
    પાછા r;
}

    સ્થિર પૂર્ણાંક
CurrentSetattro(PyObject *self, PyObject *nameobj, PyObject *કિંમત)
{
    GET_ATTR_STRING(name, nameobj);
    પાછા CurrentSetattr(self, name, કિંમત);
}

// Dictionary object - Definitions

    સ્થિર PyObject *
DictionaryGetattro(PyObject *self, PyObject *nameobj)
{
    DictionaryObject	*this = ((DictionaryObject *) (self));

    GET_ATTR_STRING(name, nameobj);

    જો (strcmp(name, "locked") == 0)
	પાછા PyLong_FromLong(this->dict->dv_lock);
    નહીંતર જો (strcmp(name, "scope") == 0)
	પાછા PyLong_FromLong(this->dict->dv_scope);

    પાછા PyObject_GenericGetAttr(self, nameobj);
}

    સ્થિર પૂર્ણાંક
DictionarySetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);
    પાછા DictionarySetattr(self, name, val);
}

// List object - Definitions

    સ્થિર PyObject *
ListGetattro(PyObject *self, PyObject *nameobj)
{
    GET_ATTR_STRING(name, nameobj);

    જો (strcmp(name, "locked") == 0)
	પાછા PyLong_FromLong(((ListObject *) (self))->list->lv_lock);

    પાછા PyObject_GenericGetAttr(self, nameobj);
}

    સ્થિર પૂર્ણાંક
ListSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);
    પાછા ListSetattr(self, name, val);
}

// Tuple object - Definitions

    સ્થિર PyObject *
TupleGetattro(PyObject *self, PyObject *nameobj)
{
    GET_ATTR_STRING(name, nameobj);

    જો (strcmp(name, "locked") == 0)
	પાછા PyLong_FromLong(((TupleObject *) (self))->tuple->tv_lock);

    પાછા PyObject_GenericGetAttr(self, nameobj);
}

    સ્થિર પૂર્ણાંક
TupleSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);
    પાછા TupleSetattr(self, name, val);
}

// Function object - Definitions

    સ્થિર PyObject *
FunctionGetattro(PyObject *self, PyObject *nameobj)
{
    PyObject		*r;
    FunctionObject	*this = (FunctionObject *)(self);

    GET_ATTR_STRING(name, nameobj);

    r = FunctionAttr(this, name);
    જો (r || PyErr_Occurred())
	પાછા r;
    નહીંતર
	પાછા PyObject_GenericGetAttr(self, nameobj);
}

// External interface

    શૂન્ય
python3_buffer_free(buf_T *buf)
{
    BufferObject *bp = BUF_PYTHON_REF(buf);
    જો (bp == ખાલી)
	પાછા;
    bp->buf = INVALID_BUFFER_કિંમત;
    BUF_PYTHON_REF(buf) = ખાલી;
}

    શૂન્ય
python3_window_free(win_T *win)
{
    WindowObject *wp = WIN_PYTHON_REF(win);
    જો (wp == ખાલી)
	પાછા;
    wp->win = INVALID_WINDOW_કિંમત;
    WIN_PYTHON_REF(win) = ખાલી;
}

    શૂન્ય
python3_tabpage_free(tabpage_T *tab)
{
    TabPageObject *tp = TAB_PYTHON_REF(tab);
    જો (tp == ખાલી)
	પાછા;
    tp->tab = INVALID_TABPAGE_કિંમત;
    TAB_PYTHON_REF(tab) = ખાલી;
}

    સ્થિર PyObject *
Py3Init_vim(શૂન્ય)
{
    // The special value is removed from sys.path in Python3_Init().
    સ્થિર wchar_t *(argv[2]) = {L"/must>not&exist/foo", ખાલી};

    જો (init_types())
	પાછા ખાલી;

    // Set sys.argv[] to avoid a crash in warn().
    PySys_SetArgv(1, argv);

    જો ((vim_module = PyModule_Create(&vimmodule)) == ખાલી)
	પાછા ખાલી;

    જો (populate_module(vim_module))
	પાછા ખાલી;

    જો (init_sys_path())
	પાછા ખાલી;

    પાછા vim_module;
}

//////////////////////////////////////////////////////////////////////////
// 4. Utility functions for handling the interface between Vim and Python.

/*
 * Convert a Vim line into a Python string.
 * All internal newlines are replaced by null characters.
 *
 * On errors, the Python exception data is set, and NULL is returned.
 */
    સ્થિર PyObject *
લીટી_To_String(અચલ અક્ષર *str)
{
    PyObject *result;
    Py_ssize_t len = strlen(str);
    અક્ષર *tmp, *p;

    tmp = alloc(len + 1);
    p = tmp;
    જો (p == ખાલી)
    {
	PyErr_NoMemory();
	પાછા ખાલી;
    }

    જ્યારે (*str)
    {
	જો (*str == '\n')
	    *p = '\0';
	નહીંતર
	    *p = *str;

	++p;
	++str;
    }
    *p = '\0';

    result = PyUnicode_Decode(tmp, len, (અક્ષર *)ENC_OPT, ERRORS_DECODE_ARG);

    vim_free(tmp);
    પાછા result;
}

    શૂન્ય
do_py3eval(char_u *str, dict_T *locals, typval_T *rettv)
{
    DoPyCommand((અક્ષર *) str,
	    locals,
	    init_range_eval,
	    (runner) run_eval,
	    (શૂન્ય *) rettv);
    જો (rettv->v_type == VAR_UNKNOWN)
    {
	rettv->v_type = VAR_સંખ્યા;
	rettv->vval.v_સંખ્યા = 0;
    }
}

    પૂર્ણાંક
set_ref_in_python3(પૂર્ણાંક copyID)
{
    પાછા set_ref_in_py(copyID);
}

    પૂર્ણાંક
python3_version(શૂન્ય)
{
#ifdef USE_LIMITED_API
    પાછા Py_LIMITED_API;
#નહીંતર
    પાછા PY_VERSION_HEX;
#endif
}
