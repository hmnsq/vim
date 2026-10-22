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

#ضمن "vim.h"

#ضمن <limits.h>

#إذا defined(MSWIN) && defined(HAVE_FCNTL_H)
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

#ضمن <Python.h>

#undef main // Defined in python.h - aargh
#undef HAVE_FCNTL_H // Clash with os_win32.h

// The "surrogateescape" error handler is new in Python 3.1
#إذا PY_VERSION_HEX >= 0x030100f0
# define CODEC_ERROR_HANDLER "surrogateescape"
#وإلا
# define CODEC_ERROR_HANDLER عدم
#endif

// Suppress Python 3.11 depreciations to see useful warnings
#ifdef __GNUC__
# pragma GCC diagnostic پاڵنان
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
#إذا PY_VERSION_HEX >= 0x030200f0
تعريفنوع PyObject PySliceObject_T;
#وإلا
تعريفنوع PySliceObject PySliceObject_T;
#endif

#ifndef MSWIN
# define HINSTANCE فراغ *
#endif
#إذا defined(DYNAMIC_PYTHON3) || defined(MSWIN)
ساكن HINSTANCE hinstPy3 = 0; // Instance of python.dll
#endif

#إذا defined(DYNAMIC_PYTHON3)

# ifdef MSWIN
#  define load_dll vimLoadLib
#  define close_dll FreeLibrary
#  define هێما_from_dll GetProcAddress
#  define load_dll_error GetWin32Error
# وإلا
#  include <dlfcn.h>
#  define FARPROC فراغ*
#  إذا defined(PY_NO_RTLD_GLOBAL) && defined(PY3_NO_RTLD_GLOBAL)
#   define load_dll(n) dlopen((n), RTLD_LAZY)
#  وإلا
#   define load_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#  endif
#  define close_dll dlclose
#  define هێما_from_dll dlsym
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
# define Py_List_زیادکردن py3_Py_List_زیادکردن
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
# إذا PY_VERSION_HEX >= 0x030601f0
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
# ifndef Py_Mapping_کلیلەکان
#  define Py_Mapping_کلیلەکان py3_Py_Mapping_کلیلەکان
# endif
# إذا (defined(USE_LIMITED_API) && Py_LIMITED_API >= 0x03080000) || \
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
# إذا defined(USE_LIMITED_API) || PY_VERSION_HEX >= 0x03080000
#  define Py_IncRef py3_Py_IncRef
#  define Py_DecRef py3_Py_DecRef
# endif
# ifdef USE_LIMITED_API
#  define Py_CompileString py3_Py_CompileString
#  define PyEval_EvalCode py3_PyEval_EvalCode
# وإلا
#  undef PyRun_SimpleString
#  define PyRun_SimpleString py3_PyRun_SimpleString
#  undef PyRun_String
#  define PyRun_String py3_PyRun_String
# endif
# define PyObject_GetAttrString py3_PyObject_GetAttrString
# define PyObject_HasAttrString py3_PyObject_HasAttrString
# define PyObject_SetAttrString py3_PyObject_SetAttrString
# define PyObject_CallFunctionObjArgs py3_PyObject_CallFunctionObjArgs
# إذا PY_VERSION_HEX >= 0x030d0000
#  define PyObject_CallFunction py3_PyObject_CallFunction
# وإلا
#  define _PyObject_CallFunction_SizeT py3__PyObject_CallFunction_SizeT
# endif
# define PyObject_Call py3_PyObject_Call
# define PyEval_GetLocals py3_PyEval_GetLocals
# define PyEval_GetGlobals py3_PyEval_GetGlobals
# define PySys_SetObject py3_PySys_SetObject
# define PySys_GetObject py3_PySys_GetObject
# define PySys_SetArgv py3_PySys_SetArgv
# define PyType_Ready py3_PyType_Ready
# إذا PY_VERSION_HEX >= 0x03040000
#  define PyType_GetFlags py3_PyType_GetFlags
# endif
# undef Py_Build_بەها
# define Py_Build_بەها py3_Py_Build_بەها
# define Py_SetPythonHome py3_Py_SetPythonHome
# define Py_Initialize py3_Py_Initialize
# define Py_Finalize py3_Py_Finalize
# define Py_IsInitialized py3_Py_IsInitialized
# define _Py_NoneStruct (*py3__Py_NoneStruct)
# define _Py_FalseStruct (*py3__Py_FalseStruct)
# define _Py_TrueStruct (*py3__Py_TrueStruct)
# إذا !defined(USE_LIMITED_API) && PY_VERSION_HEX < 0x030D0000
// Private symbol that used to be required as part of PyIter_Check.
#  define _PyObject_NextNotImplemented (*py3__PyObject_NextNotImplemented)
# endif
# define PyModule_AddObject py3_PyModule_AddObject
# define PyImport_AppendInittab py3_PyImport_AppendInittab
# define PyImport_AddModule py3_PyImport_AddModule
# ifdef USE_LIMITED_API
#  إذا Py_LIMITED_API >= 0x030a0000
#   define PyUnicode_AsUTF8AndSize py3_PyUnicode_AsUTF8AndSize
#  endif
# وإلا
#  إذا PY_VERSION_HEX >= 0x030300f0
#   define PyUnicode_AsUTF8AndSize py3_PyUnicode_AsUTF8AndSize
#  وإلا
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
# define Py_ژمار_Check (*py3_Py_ژمار_Check)
# define Py_ژمار_Long (*py3_Py_ژمار_Long)
# define PyBool_Type (*py3_PyBool_Type)
# define PyErr_NewException py3_PyErr_NewException
# ifdef Py_DEBUG
#  define _Py_NegativeRefcount py3__Py_NegativeRefcount
#  define _Py_RefTotal (*py3__Py_RefTotal)
#  define PyModule_Create2TraceRefs py3_PyModule_Create2TraceRefs
# وإلا
#  define PyModule_Create2 py3_PyModule_Create2
# endif
# إذا defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
#  define _PyObject_DebugMalloc py3__PyObject_DebugMalloc
#  define _PyObject_DebugFree py3__PyObject_DebugFree
# وإلا
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
#  وإلا
#   define PyUnicodeUCS2_FromFormat py3_PyUnicodeUCS2_FromFormat
#  endif
# وإلا
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

# إذا defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
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
ساكن صحيح (*py3_PySys_SetArgv)(صحيح, wchar_t **);
ساكن فراغ (*py3_Py_SetPythonHome)(wchar_t *home);
ساكن فراغ (*py3_Py_Initialize)(فراغ);
ساكن PyObject* (*py3_PyList_New)(Py_ssize_t size);
ساكن PyGILState_STATE (*py3_PyGILState_Ensure)(فراغ);
ساكن فراغ (*py3_PyGILState_Release)(PyGILState_STATE);
ساكن صحيح (*py3_PySys_SetObject)(حرف *, PyObject *);
ساكن PyObject* (*py3_PySys_GetObject)(حرف *);
ساكن صحيح (*py3_PyList_Append)(PyObject *, PyObject *);
ساكن صحيح (*py3_Py_List_زیادکردن)(PyObject *, صحيح, PyObject *);
ساكن Py_ssize_t (*py3_PyList_Size)(PyObject *);
ساكن PyObject* (*py3_PyTuple_New)(Py_ssize_t size);
ساكن صحيح (*py3_PyTuple_SetItem)(PyObject *, Py_ssize_t, PyObject *);
ساكن Py_ssize_t (*py3_PyTuple_Size)(PyObject *);
ساكن صحيح (*py3_PySequence_Check)(PyObject *);
ساكن Py_ssize_t (*py3_PySequence_Size)(PyObject *);
ساكن PyObject* (*py3_PySequence_GetItem)(PyObject *, Py_ssize_t);
ساكن PyObject* (*py3_PySequence_Fast)(PyObject *, ثابت حرف *);
ساكن PyObject* (*py3_PyTuple_GetItem)(PyObject *, Py_ssize_t);
ساكن صحيح (*py3_PyMapping_Check)(PyObject *);
ساكن PyObject* (*py3_Py_Mapping_کلیلەکان)(PyObject *);
# إذا PY_VERSION_HEX >= 0x030601f0
ساكن صحيح (*py3_PySlice_AdjustIndices)(Py_ssize_t length,
		     Py_ssize_t *start, Py_ssize_t *stop, Py_ssize_t step);
ساكن صحيح (*py3_PySlice_Unpack)(PyObject *slice,
		     Py_ssize_t *start, Py_ssize_t *stop, Py_ssize_t *step);
# endif
ساكن صحيح (*py3_PySlice_GetIndicesEx)(PySliceObject_T *r, Py_ssize_t length,
		     Py_ssize_t *start, Py_ssize_t *stop, Py_ssize_t *step,
		     Py_ssize_t *slicelen);
ساكن PyObject* (*py3_PyErr_NoMemory)(فراغ);
ساكن فراغ (*py3_Py_Finalize)(فراغ);
ساكن فراغ (*py3_PyErr_SetString)(PyObject *, ثابت حرف *);
ساكن فراغ (*py3_PyErr_SetObject)(PyObject *, PyObject *);
ساكن صحيح (*py3_PyErr_ExceptionMatches)(PyObject *);
# إذا defined(USE_LIMITED_API) || PY_VERSION_HEX >= 0x03080000
ساكن فراغ (*py3_Py_IncRef)(PyObject *);
ساكن فراغ (*py3_Py_DecRef)(PyObject *);
# endif
# ifdef USE_LIMITED_API
ساكن PyObject* (*py3_Py_CompileString)(ثابت حرف *, ثابت حرف *, صحيح);
ساكن PyObject* (*py3_PyEval_EvalCode)(PyObject *co, PyObject *globals, PyObject *locals);
# وإلا
ساكن صحيح (*py3_PyRun_SimpleString)(حرف *);
ساكن PyObject* (*py3_PyRun_String)(حرف *, صحيح, PyObject *, PyObject *);
# endif
ساكن PyObject* (*py3_PyObject_GetAttrString)(PyObject *, ثابت حرف *);
ساكن صحيح (*py3_PyObject_HasAttrString)(PyObject *, ثابت حرف *);
ساكن صحيح (*py3_PyObject_SetAttrString)(PyObject *, ثابت حرف *, PyObject *);
ساكن PyObject* (*py3_PyObject_CallFunctionObjArgs)(PyObject *, ...);
# إذا PY_VERSION_HEX >= 0x030d0000
ساكن PyObject* (*py3_PyObject_CallFunction)(PyObject *, حرف *, ...);
# وإلا
ساكن PyObject* (*py3__PyObject_CallFunction_SizeT)(PyObject *, حرف *, ...);
# endif
ساكن PyObject* (*py3_PyObject_Call)(PyObject *, PyObject *, PyObject *);
ساكن PyObject* (*py3_PyEval_GetGlobals)(فراغ);
ساكن PyObject* (*py3_PyEval_GetLocals)(فراغ);
ساكن PyObject* (*py3_PyList_GetItem)(PyObject *, Py_ssize_t);
ساكن PyObject* (*py3_PyImport_ImportModule)(ثابت حرف *);
ساكن PyObject* (*py3_PyImport_AddModule)(ثابت حرف *);
ساكن صحيح (*py3_PyErr_BadArgument)(فراغ);
ساكن PyObject* (*py3_PyErr_Occurred)(فراغ);
ساكن PyObject* (*py3_PyModule_GetDict)(PyObject *);
ساكن صحيح (*py3_PyList_SetItem)(PyObject *, Py_ssize_t, PyObject *);
ساكن PyObject* (*py3_PyDict_GetItemString)(PyObject *, ثابت حرف *);
ساكن صحيح (*py3_PyDict_Next)(PyObject *, Py_ssize_t *, PyObject **, PyObject **);
ساكن PyObject* (*py3_PyLong_FromLong)(long);
ساكن PyObject* (*py3_PyDict_New)(فراغ);
# إذا (defined(USE_LIMITED_API) && Py_LIMITED_API >= 0x03080000) || \
       (!defined(USE_LIMITED_API) && PY_VERSION_HEX >= 0x03080000)
ساكن صحيح (*py3_PyIter_Check)(PyObject *o);
# endif
ساكن PyObject* (*py3_PyIter_Next)(PyObject *);
ساكن PyObject* (*py3_PyObject_GetIter)(PyObject *);
ساكن PyObject* (*py3_PyObject_Repr)(PyObject *);
ساكن PyObject* (*py3_PyObject_GetItem)(PyObject *, PyObject *);
ساكن صحيح (*py3_PyObject_IsTrue)(PyObject *);
ساكن PyObject* (*py3_Py_Build_بەها)(حرف *, ...);
# إذا PY_VERSION_HEX >= 0x03040000
ساكن صحيح (*py3_PyType_GetFlags)(PyTypeObject *o);
# endif
ساكن صحيح (*py3_PyType_Ready)(PyTypeObject *type);
ساكن صحيح (*py3_PyDict_SetItemString)(PyObject *dp, حرف *کلیل, PyObject *item);
ساكن PyObject* (*py3_PyUnicode_FromString)(ثابت حرف *u);
# ifdef Py_UNICODE_USE_UCS_FUNCTIONS
#  ifdef Py_UNICODE_WIDE
ساكن PyObject* (*py3_PyUnicodeUCS4_FromFormat)(ثابت حرف *u, ...);
#  وإلا
ساكن PyObject* (*py3_PyUnicodeUCS2_FromFormat)(ثابت حرف *u, ...);
#  endif
# وإلا
ساكن PyObject* (*py3_PyUnicode_FromFormat)(ثابت حرف *u, ...);
# endif
ساكن PyObject* (*py3_PyUnicode_Decode)(ثابت حرف *u, Py_ssize_t size,
	ثابت حرف *encoding, ثابت حرف *errors);
ساكن long (*py3_PyLong_AsLong)(PyObject *);
ساكن فراغ (*py3_PyErr_SetNone)(PyObject *);
ساكن فراغ (*py3_PyEval_InitThreads)(فراغ);
ساكن فراغ(*py3_PyEval_RestoreThread)(PyThreadState *);
ساكن PyThreadState*(*py3_PyEval_SaveThread)(فراغ);
ساكن صحيح (*py3_PyArg_Parse)(PyObject *, حرف *, ...);
ساكن صحيح (*py3_PyArg_ParseTuple)(PyObject *, حرف *, ...);
ساكن فراغ (*py3_PyMem_Free)(فراغ *);
ساكن فراغ* (*py3_PyMem_Malloc)(size_t);
ساكن صحيح (*py3_Py_IsInitialized)(فراغ);
ساكن فراغ (*py3_PyErr_Clear)(فراغ);
ساكن PyObject* (*py3_PyErr_Format)(PyObject *, ثابت حرف *, ...);
ساكن فراغ (*py3_PyErr_PrintEx)(صحيح);
ساكن PyObject*(*py3__PyObject_Init)(PyObject *, PyTypeObject *);
# إذا !defined(USE_LIMITED_API) && PY_VERSION_HEX < 0x030D0000
ساكن iternextfunc py3__PyObject_NextNotImplemented;
# endif
ساكن PyObject* py3__Py_NoneStruct;
ساكن PyObject* py3__Py_FalseStruct;
ساكن PyObject* py3__Py_TrueStruct;
ساكن صحيح (*py3_PyModule_AddObject)(PyObject *m, ثابت حرف *name, PyObject *o);
ساكن صحيح (*py3_PyImport_AppendInittab)(ثابت حرف *name, PyObject* (*initfunc)(فراغ));
# ifdef USE_LIMITED_API
#  إذا Py_LIMITED_API >= 0x030a0000
ساكن حرف* (*py3_PyUnicode_AsUTF8AndSize)(PyObject *unicode, Py_ssize_t *size);
#  endif
# وإلا
#  إذا PY_VERSION_HEX >= 0x030300f0
ساكن حرف* (*py3_PyUnicode_AsUTF8AndSize)(PyObject *unicode, Py_ssize_t *size);
#  وإلا
ساكن حرف* (*py3__PyUnicode_AsString)(PyObject *unicode);
#  endif
# endif
ساكن صحيح (*py3_PyUnicode_CompareWithASCIIString)(PyObject *unicode, ثابت حرف* string);
ساكن PyObject* (*py3_PyUnicode_AsEncodedString)(PyObject *unicode, ثابت حرف* encoding, ثابت حرف* errors);
ساكن PyObject* (*py3_PyUnicode_AsUTF8String)(PyObject *unicode);
ساكن حرف* (*py3_PyBytes_AsString)(PyObject *bytes);
ساكن صحيح (*py3_PyBytes_AsStringAndSize)(PyObject *bytes, حرف **buffer, Py_ssize_t *length);
ساكن PyObject* (*py3_PyBytes_FromString)(حرف *str);
ساكن PyObject* (*py3_PyBytes_FromStringAndSize)(حرف *str, Py_ssize_t length);
# إذا PY_VERSION_HEX >= 0x030900b0
ساكن PyObject* (*py3__PyObject_New)(PyTypeObject *);
# endif
ساكن PyObject* (*py3_PyFloat_FromDouble)(مزدوج ژمارە);
ساكن مزدوج (*py3_PyFloat_AsDouble)(PyObject *);
ساكن PyObject* (*py3_PyObject_GenericGetAttr)(PyObject *obj, PyObject *name);
ساكن PyObject* (*py3_PyType_GenericAlloc)(PyTypeObject *type, Py_ssize_t nitems);
ساكن PyObject* (*py3_PyType_GenericNew)(PyTypeObject *type, PyObject *args, PyObject *kwds);
ساكن PyTypeObject* py3_PyType_Type;
# ifndef USE_LIMITED_API
ساكن PyTypeObject* py3_PyStdPrinter_Type;
# endif
ساكن PyTypeObject* py3_PySlice_Type;
ساكن PyTypeObject* py3_PyFloat_Type;
ساكن PyTypeObject* py3_PyBool_Type;
ساكن صحيح (*py3_Py_ژمار_Check)(PyObject *);
ساكن PyObject* (*py3_Py_ژمار_Long)(PyObject *);
ساكن PyObject* (*py3_PyErr_NewException)(حرف *name, PyObject *base, PyObject *dict);
ساكن PyObject* (*py3_PyCapsule_New)(فراغ *, حرف *, PyCapsule_Destructor);
ساكن فراغ* (*py3_PyCapsule_GetPointer)(PyObject *, حرف *);
# ifdef Py_DEBUG
ساكن فراغ (*py3__Py_NegativeRefcount)(ثابت حرف *fname, صحيح lineno, PyObject *op);
ساكن Py_ssize_t* py3__Py_RefTotal;
ساكن PyObject* (*py3_PyModule_Create2TraceRefs)(بنية PyModuleDef* module, صحيح module_api_version);
# وإلا
ساكن PyObject* (*py3_PyModule_Create2)(بنية PyModuleDef* module, صحيح module_api_version);
# endif
# إذا defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
ساكن فراغ (*py3__PyObject_DebugFree)(فراغ*);
ساكن فراغ* (*py3__PyObject_DebugMalloc)(size_t);
# وإلا
ساكن فراغ (*py3_PyObject_Free)(فراغ*);
ساكن فراغ* (*py3_PyObject_Malloc)(size_t);
# endif
ساكن PyObject*(*py3__PyObject_GC_New)(PyTypeObject *);
ساكن فراغ(*py3_PyObject_GC_Del)(فراغ *);
ساكن فراغ(*py3_PyObject_GC_UnTrack)(فراغ *);
ساكن صحيح (*py3_PyType_IsSubtype)(PyTypeObject *, PyTypeObject *);
# ifdef USE_LIMITED_API
ساكن فراغ* (*py3_PyType_GetSlot)(PyTypeObject *, صحيح);
ساكن PyObject* (*py3_PyType_FromSpec)(PyType_Spec *);
# endif

// Imported exception objects
ساكن PyObject *p3imp_PyExc_AttributeError;
ساكن PyObject *p3imp_PyExc_IndexError;
ساكن PyObject *p3imp_Py_Exc_کلیل_Error;
ساكن PyObject *p3imp_PyExc_KeyboardInterrupt;
ساكن PyObject *p3imp_PyExc_TypeError;
ساكن PyObject *p3imp_Py_Exc_بەها_Error;
ساكن PyObject *p3imp_PyExc_SystemExit;
ساكن PyObject *p3imp_PyExc_RuntimeError;
ساكن PyObject *p3imp_PyExc_ImportError;
ساكن PyObject *p3imp_PyExc_OverflowError;

# define PyExc_AttributeError p3imp_PyExc_AttributeError
# define PyExc_IndexError p3imp_PyExc_IndexError
# define Py_Exc_کلیل_Error p3imp_Py_Exc_کلیل_Error
# define PyExc_KeyboardInterrupt p3imp_PyExc_KeyboardInterrupt
# define PyExc_TypeError p3imp_PyExc_TypeError
# define Py_Exc_بەها_Error p3imp_Py_Exc_بەها_Error
# define PyExc_SystemExit p3imp_PyExc_SystemExit
# define PyExc_RuntimeError p3imp_PyExc_RuntimeError
# define PyExc_ImportError p3imp_PyExc_ImportError
# define PyExc_OverflowError p3imp_PyExc_OverflowError

/*
 * Table of name to function pointer of python.
 */
# define PYTHON_PROC FARPROC
ساكن بنية
{
    حرف *name;
    PYTHON_PROC *ptr;
} py3_funcname_table[] =
{
    {"PySys_SetArgv", (PYTHON_PROC*)&py3_PySys_SetArgv},
    {"Py_SetPythonHome", (PYTHON_PROC*)&py3_Py_SetPythonHome},
    {"Py_Initialize", (PYTHON_PROC*)&py3_Py_Initialize},
    {"_PyArg_ParseTuple_SizeT", (PYTHON_PROC*)&py3_PyArg_ParseTuple},
    {"_Py_BuildValue_SizeT", (PYTHON_PROC*)&py3_Py_Build_بەها},
    {"PyMem_Free", (PYTHON_PROC*)&py3_PyMem_Free},
    {"PyMem_Malloc", (PYTHON_PROC*)&py3_PyMem_Malloc},
    {"PyList_New", (PYTHON_PROC*)&py3_PyList_New},
    {"PyGILState_Ensure", (PYTHON_PROC*)&py3_PyGILState_Ensure},
    {"PyGILState_Release", (PYTHON_PROC*)&py3_PyGILState_Release},
    {"PySys_SetObject", (PYTHON_PROC*)&py3_PySys_SetObject},
    {"PySys_GetObject", (PYTHON_PROC*)&py3_PySys_GetObject},
    {"PyList_Append", (PYTHON_PROC*)&py3_PyList_Append},
    {"PyList_Insert", (PYTHON_PROC*)&py3_Py_List_زیادکردن},
    {"PyList_Size", (PYTHON_PROC*)&py3_PyList_Size},
    {"PyTuple_New", (PYTHON_PROC*)&py3_PyTuple_New},
    {"PyTuple_GetItem", (PYTHON_PROC*)&py3_PyTuple_GetItem},
    {"PyTuple_SetItem", (PYTHON_PROC*)&py3_PyTuple_SetItem},
    {"PyTuple_Size", (PYTHON_PROC*)&py3_PyTuple_Size},
    {"PySequence_Check", (PYTHON_PROC*)&py3_PySequence_Check},
    {"PySequence_Size", (PYTHON_PROC*)&py3_PySequence_Size},
    {"PySequence_GetItem", (PYTHON_PROC*)&py3_PySequence_GetItem},
    {"PySequence_Fast", (PYTHON_PROC*)&py3_PySequence_Fast},
# إذا PY_VERSION_HEX >= 0x030601f0
    {"PySlice_AdjustIndices", (PYTHON_PROC*)&py3_PySlice_AdjustIndices},
    {"PySlice_Unpack", (PYTHON_PROC*)&py3_PySlice_Unpack},
# endif
    {"PySlice_GetIndicesEx", (PYTHON_PROC*)&py3_PySlice_GetIndicesEx},
    {"PyErr_NoMemory", (PYTHON_PROC*)&py3_PyErr_NoMemory},
    {"Py_Finalize", (PYTHON_PROC*)&py3_Py_Finalize},
    {"PyErr_SetString", (PYTHON_PROC*)&py3_PyErr_SetString},
    {"PyErr_SetObject", (PYTHON_PROC*)&py3_PyErr_SetObject},
    {"PyErr_ExceptionMatches", (PYTHON_PROC*)&py3_PyErr_ExceptionMatches},
# إذا defined(USE_LIMITED_API) || PY_VERSION_HEX >= 0x03080000
    {"Py_IncRef", (PYTHON_PROC*)&py3_Py_IncRef},
    {"Py_DecRef", (PYTHON_PROC*)&py3_Py_DecRef},
# endif
# ifdef USE_LIMITED_API
    {"Py_CompileString", (PYTHON_PROC*)&py3_Py_CompileString},
    {"PyEval_EvalCode", (PYTHON_PROC*)&PyEval_EvalCode},
# وإلا
    {"PyRun_SimpleString", (PYTHON_PROC*)&py3_PyRun_SimpleString},
    {"PyRun_String", (PYTHON_PROC*)&py3_PyRun_String},
# endif
    {"PyObject_GetAttrString", (PYTHON_PROC*)&py3_PyObject_GetAttrString},
    {"PyObject_HasAttrString", (PYTHON_PROC*)&py3_PyObject_HasAttrString},
    {"PyObject_SetAttrString", (PYTHON_PROC*)&py3_PyObject_SetAttrString},
    {"PyObject_CallFunctionObjArgs", (PYTHON_PROC*)&py3_PyObject_CallFunctionObjArgs},
# إذا PY_VERSION_HEX >= 0x030d0000
    {"PyObject_CallFunction", (PYTHON_PROC*)&py3_PyObject_CallFunction},
# وإلا
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
    {"PyMapping_Keys", (PYTHON_PROC*)&py3_Py_Mapping_کلیلەکان},
# إذا (defined(USE_LIMITED_API) && Py_LIMITED_API >= 0x03080000) || \
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
# إذا PY_VERSION_HEX >= 0x03040000
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
# إذا !defined(USE_LIMITED_API) && PY_VERSION_HEX < 0x030D0000
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
#  إذا Py_LIMITED_API >= 0x030a0000
    {"PyUnicode_AsUTF8AndSize", (PYTHON_PROC*)&py3_PyUnicode_AsUTF8AndSize},
#  endif
# وإلا
#  إذا PY_VERSION_HEX >= 0x030300f0
    {"PyUnicode_AsUTF8AndSize", (PYTHON_PROC*)&py3_PyUnicode_AsUTF8AndSize},
#  وإلا
    {"_PyUnicode_AsString", (PYTHON_PROC*)&py3__PyUnicode_AsString},
#  endif
# endif
    {"PyUnicode_CompareWithASCIIString", (PYTHON_PROC*)&py3_PyUnicode_CompareWithASCIIString},
    {"PyUnicode_AsUTF8String", (PYTHON_PROC*)&py3_PyUnicode_AsUTF8String},
# ifdef Py_UNICODE_USE_UCS_FUNCTIONS
#  ifdef Py_UNICODE_WIDE
    {"PyUnicodeUCS4_FromFormat", (PYTHON_PROC*)&py3_PyUnicodeUCS4_FromFormat},
#  وإلا
    {"PyUnicodeUCS2_FromFormat", (PYTHON_PROC*)&py3_PyUnicodeUCS2_FromFormat},
#  endif
# وإلا
    {"PyUnicode_FromFormat", (PYTHON_PROC*)&py3_PyUnicode_FromFormat},
# endif
    {"PyBytes_AsString", (PYTHON_PROC*)&py3_PyBytes_AsString},
    {"PyBytes_AsStringAndSize", (PYTHON_PROC*)&py3_PyBytes_AsStringAndSize},
    {"PyBytes_FromString", (PYTHON_PROC*)&py3_PyBytes_FromString},
    {"PyBytes_FromStringAndSize", (PYTHON_PROC*)&py3_PyBytes_FromStringAndSize},
# إذا PY_VERSION_HEX >= 0x030900b0
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
    {"PyNumber_Check", (PYTHON_PROC*)&py3_Py_ژمار_Check},
    {"PyNumber_Long", (PYTHON_PROC*)&py3_Py_ژمار_Long},
    {"PyErr_NewException", (PYTHON_PROC*)&py3_PyErr_NewException},
# ifdef Py_DEBUG
    {"_Py_NegativeRefcount", (PYTHON_PROC*)&py3__Py_NegativeRefcount},
    {"_Py_RefTotal", (PYTHON_PROC*)&py3__Py_RefTotal},
    {"PyModule_Create2TraceRefs", (PYTHON_PROC*)&py3_PyModule_Create2TraceRefs},
# وإلا
    {"PyModule_Create2", (PYTHON_PROC*)&py3_PyModule_Create2},
# endif
# إذا defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
    {"_PyObject_DebugFree", (PYTHON_PROC*)&py3__PyObject_DebugFree},
    {"_PyObject_DebugMalloc", (PYTHON_PROC*)&py3__PyObject_DebugMalloc},
# وإلا
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
#  إذا PY_VERSION_HEX >= 0x03040000
    {"PyType_GetSlot", (PYTHON_PROC*)&py3_PyType_GetSlot},
#  endif
    {"PyType_FromSpec", (PYTHON_PROC*)&py3_PyType_FromSpec},
# endif
    {"", عدم},
};

# إذا defined(USE_LIMITED_API) || PY_VERSION_HEX >= 0x03080000
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

# إذا PY_VERSION_HEX >= 0x030900b0
    ساكن مضمن صحيح
py3_PyType_HasFeature(PyTypeObject *type, unsigned long feature)
{
    أرجع ((PyType_GetFlags(type) & feature) != 0);
}
#  define PyType_HasFeature(t,f) py3_PyType_HasFeature(t,f)
# endif

# إذا PY_VERSION_HEX >= 0x030a00b2
    ساكن مضمن صحيح
py3__PyObject_TypeCheck(PyObject *ob, PyTypeObject *type)
{
    أرجع Py_IS_TYPE(ob, type) || PyType_IsSubtype(Py_TYPE(ob), type);
}
#  إذا PY_VERSION_HEX >= 0x030b00b3
#   undef PyObject_TypeCheck
#   define PyObject_TypeCheck(o,t) py3__PyObject_TypeCheck(o,t)
#  وإلا
#   define _PyObject_TypeCheck(o,t) py3__PyObject_TypeCheck(o,t)
#  endif
# endif

# إذا !defined(USE_LIMITED_API) && PY_VERSION_HEX >= 0x030c00b0
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
    ساكن WCHAR *
py3_get_system_libname(ثابت حرف *libname)
{
    ثابت WCHAR	*pythoncore = L"Software\\Python\\PythonCore";
    ثابت حرف	*cp = libname;
    WCHAR	subkey[128];
    HKEY	h_کلیل;
    صحيح		i;
    DWORD	j, len;
    LSTATUS	ret;

    طالما (*cp != '\0')
    {
	إذا (*cp == ':' || *cp == '\\' || *cp == '/')
	{
	    // Bail out if "libname" contains path separator, assume it is
	    // an absolute path.
	    أرجع عدم;
	}
	++cp;
    }

    WCHAR   keyfound[32];
    HKEY    h_کلیل_لووتکە[] = {HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE};
    HKEY    h_کلیل_Found = عدم;
#  ifdef USE_LIMITED_API
    long    maxminor = -1;
#  endif
    لكل (i = 0; i < ARRAY_LENGTH(h_کلیل_لووتکە); i++)
    {
	long	major, minor;

	ret = Reg_Open_کلیل_Ex_W(h_کلیل_لووتکە[i], pythoncore, 0, کلیل_READ, &h_کلیل);
	إذا (ret != ERROR_SUCCESS)
	    استمر;
	لكل (j = 0;; j++)
	{
	    WCHAR   keyname[32];
	    WCHAR   *wp;

	    len = ARRAY_LENGTH(keyname);
	    ret = Reg_Enum_کلیل_Ex_W(h_کلیل, j, keyname, &len,
						    عدم, عدم, عدم, عدم);
	    إذا (ret == ERROR_NO_MORE_کاڵاکان)
		اكسر;

	    major = wcstol(keyname, &wp, 10);
	    إذا (*wp != L'.')
		استمر;
	    minor = wcstol(wp + 1, &wp, 10);
#  ifdef _WIN64
	    إذا (*wp != L'\0')
		استمر;
#  وإلا
	    إذا (wcscmp(wp, L"-32") != 0)
		استمر;
#  endif

	    إذا (major != PY_MAJOR_VERSION)
		استمر;
#  ifdef USE_LIMITED_API
	    // Search the latest version.
	    إذا ((minor > maxminor)
		    && (minor >= ((Py_LIMITED_API >> 16) & 0xff)))
	    {
		maxminor = minor;
		wcscpy(keyfound, keyname);
		h_کلیل_Found = h_کلیل_لووتکە[i];
	    }
#  وإلا
	    // Check if it matches with the compiled version.
	    إذا (minor == PY_MINOR_VERSION)
	    {
		wcscpy(keyfound, keyname);
		h_کلیل_Found = h_کلیل_لووتکە[i];
		اكسر;
	    }
#  endif
	}
	Reg_Close_کلیل(h_کلیل);
#  ifdef USE_LIMITED_API
	إذا (h_کلیل_Found != عدم)
	    اكسر;
#  endif
    }
    إذا (h_کلیل_Found == عدم)
	أرجع عدم;

    swprintf(subkey, ARRAY_LENGTH(subkey), L"%ls\\%ls\\InstallPath",
							pythoncore, keyfound);
    ret = Reg_Get_بەها_W(h_کلیل_Found, subkey, عدم, RRF_RT_REG_SZ,
							    عدم, عدم, &len);
    إذا (ret != ERROR_MORE_DATA && ret != ERROR_SUCCESS)
	أرجع عدم;
    size_t len2 = len / حجم(WCHAR) + 1 + strlen(libname);
    WCHAR *path = alloc(len2 * حجم(WCHAR));
    إذا (path == عدم)
	أرجع عدم;
    ret = Reg_Get_بەها_W(h_کلیل_Found, subkey, عدم, RRF_RT_REG_SZ,
							    عدم, path, &len);
    إذا (ret != ERROR_SUCCESS)
    {
	vim_free(path);
	أرجع عدم;
    }
    // Remove trailing path separators.
    size_t len3 = wcslen(path);
    إذا ((len3 > 0) && (path[len3 - 1] == L'/' || path[len3 - 1] == L'\\'))
	--len3;
    swprintf(path + len3, len2 - len3, L"\\%hs", libname);
    أرجع path;
}
# endif

/*
 * Load library and get all pointers.
 * Parameter 'libname' provides name of DLL.
 * Return OK or FAIL.
 */
    ساكن صحيح
py3_runtime_link_init(حرف *libname, صحيح verbose)
{
    صحيح i;
    PYTHON_PROC *ucs_from_string = (PYTHON_PROC *)&py3_PyUnicode_FromString;
    PYTHON_PROC *ucs_decode = (PYTHON_PROC *)&py3_PyUnicode_Decode;
    PYTHON_PROC *ucs_as_encoded_string =
				 (PYTHON_PROC *)&py3_PyUnicode_AsEncodedString;

# إذا !(defined(PY_NO_RTLD_GLOBAL) && defined(PY3_NO_RTLD_GLOBAL)) && defined(UNIX) && defined(FEAT_PYTHON)
    // Can't have Python and Python3 loaded at the same time.
    // It causes a crash, because RTLD_GLOBAL is needed for
    // standard C extension libraries of one or both python versions.
    إذا (python_loaded())
    {
	إذا (verbose)
	    emsg(_(e_this_vim_cannot_execute_py3_after_using_python));
	أرجع FAIL;
    }
# endif

    إذا (hinstPy3 != 0)
	أرجع OK;
    hinstPy3 = load_dll(libname);

# ifdef MSWIN
    إذا (!hinstPy3)
    {
	// Attempt to use the path from InstallPath as stored in the registry.
	WCHAR *syslibname = py3_get_system_libname(libname);

	إذا (syslibname != عدم)
	{
	    hinstPy3 = LoadLibraryExW(syslibname, عدم,
		    LOAD_LIBRARY_گەڕان_DLL_LOAD_DIR |
		    LOAD_LIBRARY_گەڕان_SYSTEM32);
	    vim_free(syslibname);
	}
    }
# endif

    إذا (!hinstPy3)
    {
	إذا (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	أرجع FAIL;
    }

    لكل (i = 0; py3_funcname_table[i].ptr; ++i)
    {
	إذا ((*py3_funcname_table[i].ptr = هێما_from_dll(hinstPy3,
			py3_funcname_table[i].name)) == عدم)
	{
	    close_dll(hinstPy3);
	    hinstPy3 = 0;
	    إذا (verbose)
		semsg(_(e_could_not_load_library_function_str), py3_funcname_table[i].name);
	    أرجع FAIL;
	}
    }

    // Load unicode functions separately as only the ucs2 or the ucs4 functions
    // will be present in the library.
# إذا PY_VERSION_HEX >= 0x030300f0
    *ucs_from_string = هێما_from_dll(hinstPy3, "PyUnicode_FromString");
    *ucs_decode = هێما_from_dll(hinstPy3, "PyUnicode_Decode");
    *ucs_as_encoded_string = هێما_from_dll(hinstPy3,
	    "PyUnicode_AsEncodedString");
# وإلا
    *ucs_from_string = هێما_from_dll(hinstPy3, "PyUnicodeUCS2_FromString");
    *ucs_decode = هێما_from_dll(hinstPy3,
	    "PyUnicodeUCS2_Decode");
    *ucs_as_encoded_string = هێما_from_dll(hinstPy3,
	    "PyUnicodeUCS2_AsEncodedString");
    إذا (*ucs_from_string == عدم || *ucs_decode == عدم
					     || *ucs_as_encoded_string == عدم)
    {
	*ucs_from_string = هێما_from_dll(hinstPy3,
		"PyUnicodeUCS4_FromString");
	*ucs_decode = هێما_from_dll(hinstPy3,
		"PyUnicodeUCS4_Decode");
	*ucs_as_encoded_string = هێما_from_dll(hinstPy3,
		"PyUnicodeUCS4_AsEncodedString");
    }
# endif
    إذا (*ucs_from_string == عدم || *ucs_decode == عدم
					     || *ucs_as_encoded_string == عدم)
    {
	close_dll(hinstPy3);
	hinstPy3 = 0;
	إذا (verbose)
	    semsg(_(e_could_not_load_library_function_str), "PyUnicode_UCSX_*");
	أرجع FAIL;
    }

    أرجع OK;
}

/*
 * If python is enabled (there is installed python on Windows system) return
 * TRUE, else FALSE.
 */
    صحيح
python3_enabled(صحيح verbose)
{
    أرجع py3_runtime_link_init((حرف *)p_py3dll, verbose) == OK;
}

/*
 * Load the standard Python exceptions - don't import the symbols from the
 * DLL, as this can cause errors (importing data symbols is not reliable).
 */
    ساكن فراغ
get_py3_exceptions(فراغ)
{
    PyObject *exmod = PyImport_ImportModule("builtins");
    PyObject *exdict = PyModule_GetDict(exmod);
    p3imp_PyExc_AttributeError = PyDict_GetItemString(exdict, "AttributeError");
    p3imp_PyExc_IndexError = PyDict_GetItemString(exdict, "IndexError");
    p3imp_Py_Exc_کلیل_Error = PyDict_GetItemString(exdict, "KeyError");
    p3imp_PyExc_KeyboardInterrupt = PyDict_GetItemString(exdict, "KeyboardInterrupt");
    p3imp_PyExc_TypeError = PyDict_GetItemString(exdict, "TypeError");
    p3imp_Py_Exc_بەها_Error = PyDict_GetItemString(exdict, "ValueError");
    p3imp_PyExc_SystemExit = PyDict_GetItemString(exdict, "SystemExit");
    p3imp_PyExc_RuntimeError = PyDict_GetItemString(exdict, "RuntimeError");
    p3imp_PyExc_ImportError = PyDict_GetItemString(exdict, "ImportError");
    p3imp_PyExc_OverflowError = PyDict_GetItemString(exdict, "OverflowError");
    Py_XINCREF(p3imp_PyExc_AttributeError);
    Py_XINCREF(p3imp_PyExc_IndexError);
    Py_XINCREF(p3imp_Py_Exc_کلیل_Error);
    Py_XINCREF(p3imp_PyExc_KeyboardInterrupt);
    Py_XINCREF(p3imp_PyExc_TypeError);
    Py_XINCREF(p3imp_Py_Exc_بەها_Error);
    Py_XINCREF(p3imp_PyExc_SystemExit);
    Py_XINCREF(p3imp_PyExc_RuntimeError);
    Py_XINCREF(p3imp_PyExc_ImportError);
    Py_XINCREF(p3imp_PyExc_OverflowError);
    Py_XDECREF(exmod);
}
#endif // DYNAMIC_PYTHON3

ساكن صحيح py3initialised = 0;
#define PYINITIALISED py3initialised
ساكن صحيح python_end_called = FALSE;

#ifdef USE_LIMITED_API
# define DESTRUCTOR_FINISH(self) \
    ((freefunc)PyType_GetSlot(Py_TYPE((PyObject*)self), Py_tp_free))((PyObject*)self)
#وإلا
# define DESTRUCTOR_FINISH(self) Py_TYPE(self)->tp_free((PyObject*)self)
#endif

#define WIN_PYTHON_REF(win) win->w_python3_ref
#define BUF_PYTHON_REF(buf) buf->b_python3_ref
#define TAB_PYTHON_REF(tab) tab->tp_python3_ref

    ساكن فراغ
call_PyObject_Free(فراغ *p)
{
#إذا defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
    _PyObject_DebugFree(p);
#وإلا
    PyObject_Free(p);
#endif
}

    ساكن PyObject *
call_PyType_GenericNew(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    أرجع PyType_GenericNew(type,args,kwds);
}

    ساكن PyObject *
call_PyType_GenericAlloc(PyTypeObject *type, Py_ssize_t nitems)
{
    أرجع PyType_GenericAlloc(type,nitems);
}

ساكن PyObject *OutputGetattro(PyObject *, PyObject *);
ساكن صحيح OutputSetattro(PyObject *, PyObject *, PyObject *);
ساكن PyObject *BufferGetattro(PyObject *, PyObject *);
ساكن صحيح BufferSetattro(PyObject *, PyObject *, PyObject *);
ساكن PyObject *TabPageGetattro(PyObject *, PyObject *);
ساكن PyObject *WindowGetattro(PyObject *, PyObject *);
ساكن صحيح WindowSetattro(PyObject *, PyObject *, PyObject *);
ساكن PyObject *RangeGetattro(PyObject *, PyObject *);
ساكن PyObject *CurrentGetattro(PyObject *, PyObject *);
ساكن صحيح CurrentSetattro(PyObject *, PyObject *, PyObject *);
ساكن PyObject *DictionaryGetattro(PyObject *, PyObject *);
ساكن صحيح DictionarySetattro(PyObject *, PyObject *, PyObject *);
ساكن PyObject *ListGetattro(PyObject *, PyObject *);
ساكن صحيح ListSetattro(PyObject *, PyObject *, PyObject *);
ساكن PyObject *TupleGetattro(PyObject *, PyObject *);
ساكن صحيح TupleSetattro(PyObject *, PyObject *, PyObject *);
ساكن PyObject *FunctionGetattro(PyObject *, PyObject *);

ساكن بنية PyModuleDef vimmodule;

#define PY_CAN_RECURSE

/*
 * Include the code shared with if_python.c
 */
#ضمن "if_py_both.h"

#ifdef USE_LIMITED_API
# إذا Py_LIMITED_API >= 0x030A0000
#  define PY_UNICODE_GET_UTF8_CHARS(obj) PyUnicode_AsUTF8AndSize(obj, عدم)
# وإلا
// Python limited API before 3.10 lack easy ways to query the raw UTF-8 chars.
// We need to first convert the string to bytes, and then extract the chars.
// This function is only used for attribute string comparisons, which have
// known short length. As such, just allocate a short static buffer to hold
// the characters instead of having to allocate/deallcoate it.
//
// An alternative would be to convert all attribute string comparisons to use
// PyUnicode_CompareWithASCIIString to skip having to extract the chars.
ساكن حرف py3_unicode_utf8_chars[20];
ساكن حرف* PY_UNICODE_GET_UTF8_CHARS(PyObject* str)
{
    py3_unicode_utf8_chars[0] = '\0';
    PyObject* bytes = PyUnicode_AsUTF8String(str);
    إذا (bytes)
    {
	حرف *chars;
	Py_ssize_t len;
	إذا (PyBytes_AsStringAndSize(bytes, &chars, &len) != -1)
	{
	    إذا (len < (Py_ssize_t)حجم(py3_unicode_utf8_chars))
		// PyBytes_AsStringAndSize guarantees null-termination
		memcpy(py3_unicode_utf8_chars, chars, len + 1);
	}
	Py_DECREF(bytes);
    }
    أرجع py3_unicode_utf8_chars;
}
# endif
#وإلا	// !USE_LIMITED_API
# إذا PY_VERSION_HEX >= 0x030300f0
#  define PY_UNICODE_GET_UTF8_CHARS(obj) PyUnicode_AsUTF8AndSize(obj, عدم)
# وإلا
#  define PY_UNICODE_GET_UTF8_CHARS _PyUnicode_AsString
# endif
#endif

// NOTE: Must always be used at the start of a block, since it declares "name".
#define GET_ATTR_STRING(name, nameobj) \
    حرف	*name = ""; \
    إذا (PyUnicode_Check(nameobj)) \
	name = (حرف *)PY_UNICODE_GET_UTF8_CHARS(nameobj)

#define PY3OBJ_DELETED(obj) (obj->ob_base.ob_refcnt<=0)

///////////////////////////////////////////////////////
// Internal function prototypes.

ساكن PyObject *Py3Init_vim(فراغ);

///////////////////////////////////////////////////////
// 1. Python interpreter main program.

    فراغ
python3_end(فراغ)
{
    ساكن صحيح recurse = 0;

    // If a crash occurs while doing this, don't try again.
    إذا (recurse != 0)
	أرجع;

    python_end_called = TRUE;
    ++recurse;

#ifdef DYNAMIC_PYTHON3
    إذا (hinstPy3)
#endif
    إذا (Py_IsInitialized())
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

#إذا defined(DYNAMIC_PYTHON3) && defined(DYNAMIC_PYTHON) && defined(FEAT_PYTHON) && defined(UNIX)
    صحيح
python3_loaded(فراغ)
{
    أرجع (hinstPy3 != 0);
}
#endif

ساكن wchar_t *py_home_buf = عدم;

#إذا defined(MSWIN) && (PY_VERSION_HEX >= 0x030500f0)
/*
 * Return TRUE if stdin is readable from Python 3.
 */
    ساكن BOOL
is_stdin_readable(فراغ)
{
    DWORD	    mode, eventnum;
    بنية _stat    st;
    صحيح		    fd = fileno(stdin);
    HANDLE	    hstdin = (HANDLE)_get_osfhandle(fd);

    // Check if stdin is connected to the console.
    إذا (GetConsoleMode(hstdin, &mode))
	// Check if it is opened as input.
	أرجع Get_ژمار_Of_Console_تێخستن_Events(hstdin, &eventnum);

    أرجع _fstat(fd, &st) == 0;
}

// Python 3.5 or later will abort inside Py_Initialize() when stdin has
// been closed (i.e. executed by "vim -").  Reconnect stdin to CONIN$.
// Note that the python DLL is linked to its own stdio DLL which can be
// differ from Vim's stdio.
    ساكن فراغ
reset_stdin(فراغ)
{
    FILE *(*py__acrt_iob_func)(unsigned) = عدم;
    FILE *(*pyfreopen)(ثابت حرف *, ثابت حرف *, FILE *) = عدم;
    HINSTANCE hinst = get_forwarded_dll(hinstPy3);

    إذا (hinst == عدم || is_stdin_readable())
	أرجع;

    // Get "freopen" and "stdin" which are used in the python DLL.
    // "stdin" is defined as "__acrt_iob_func(0)" in VC++ 2015 or later.
    py__acrt_iob_func = get_dll_import_func(hinst, "__acrt_iob_func");
    إذا (py__acrt_iob_func)
    {
	HINSTANCE hpystdiodll = find_imported_module_by_funcname(hinst,
							    "__acrt_iob_func");
	إذا (hpystdiodll)
	    pyfreopen = (فراغ *)GetProcAddress(hpystdiodll, "freopen");
    }

    // Reconnect stdin to CONIN$.
    إذا (pyfreopen != عدم)
	pyfreopen("CONIN$", "r", py__acrt_iob_func(0));
    وإلا
	freopen("CONIN$", "r", stdin);
}
#وإلا
# define reset_stdin()
#endif

// Python 3.2 or later will abort inside Py_Initialize() when mandatory
// modules cannot be loaded (e.g. 'pythonthreehome' is wrongly set.).
// Install a hook to python dll's exit() and recover from it.
#إذا defined(MSWIN) && (PY_VERSION_HEX >= 0x030200f0)
# define HOOK_EXIT
# include <setjmp.h>

ساكن jmp_buf exit_hook_jump_buf;
ساكن فراغ *orig_exit = عدم;

/*
 * Function that replaces exit() while calling Py_Initialize().
 */
    ساكن فراغ
hooked_exit(صحيح ret)
{
    // Recover from exit.
    longjmp(exit_hook_jump_buf, 1);
}

/*
 * Install a hook to python dll's exit().
 */
    ساكن فراغ
hook_py_exit(فراغ)
{
    HINSTANCE hinst = get_forwarded_dll(hinstPy3);

    إذا (hinst == عدم || orig_exit != عدم)
	أرجع;

    orig_exit = hook_dll_import_func(hinst, "exit", (فراغ *)hooked_exit);
}

/*
 * Remove the hook installed by hook_py_exit().
 */
    ساكن فراغ
restore_py_exit(فراغ)
{
    HINSTANCE hinst = hinstPy3;

    إذا (hinst == عدم)
	أرجع;

    إذا (orig_exit != عدم)
	hook_dll_import_func(hinst, "exit", orig_exit);
    orig_exit = عدم;
}
#endif

    ساكن صحيح
Python3_Init(فراغ)
{
    إذا (!py3initialised)
    {
#ifdef DYNAMIC_PYTHON3
	إذا (!python3_enabled(TRUE))
	{
	    emsg(_(e_sorry_this_command_is_disabled_python_library_could_not_be_found));
	    اذهب fail;
	}
#endif
	// Python 3.13 introduced a really useful feature: colorized exceptions.
	// This is great if you're reading them from the terminal, but useless
	// and broken everywhere else (such as in log files, or text editors).
	// Opt out, forcefully.
	vim_setenv((char_u*)"PYTHON_COLORS", (char_u*)"0");

	init_structs();

	إذا (*p_py3home != NUL)
	{
	    size_t len = mbstowcs(عدم, (حرف *)p_py3home, 0) + 1;

	    // The string must not change later, make a copy in static memory.
	    py_home_buf = ALLOC_MULT(wchar_t, len);
	    إذا (py_home_buf != عدم && mbstowcs(
			    py_home_buf, (حرف *)p_py3home, len) != (size_t)-1)
		Py_SetPythonHome(py_home_buf);
	}
#ifdef PYTHON3_HOME
	وإلا إذا (mch_getenv((char_u *)"PYTHONHOME") == عدم)
	    Py_SetPythonHome(PYTHON3_HOME);
#endif

	PyImport_AppendInittab("vim", Py3Init_vim);

#إذا !defined(DYNAMIC_PYTHON3) && defined(MSWIN)
	hinstPy3 = GetModuleHandle(PYTHON3_DLL);
#endif
	reset_stdin();

#ifdef HOOK_EXIT
	// Catch exit() called in Py_Initialize().
	hook_py_exit();
	إذا (setjmp(exit_hook_jump_buf) == 0)
	{
	    Py_Initialize();
	    restore_py_exit();
	}
	وإلا
	{
	    // exit() was called in Py_Initialize().
	    restore_py_exit();
	    emsg(_(e_critical_error_in_python3_initialization_check_your_installation));
	    اذهب fail;
	}
#وإلا
	Py_Initialize();
#endif

#إذا PY_VERSION_HEX < 0x03090000
	// Initialise threads.  This is deprecated since Python 3.9.
	PyEval_InitThreads();
#endif
#ifdef DYNAMIC_PYTHON3
	get_py3_exceptions();
#endif

	إذا (PythonIO_Init_io())
	    اذهب fail;

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

    أرجع 0;

fail:
    // We call PythonIO_Flush() here to print any Python errors.
    // This is OK, as it is possible to call this function even
    // if PythonIO_Init_io() has not completed successfully (it will
    // not do anything in this case).
    PythonIO_Flush();
    أرجع -1;
}

/*
 * External interface
 */
    ساكن فراغ
DoPyCommand(ثابت حرف *cmd,
	    dict_T* locals,
	    rangeinitializer init_range,
	    runner run,
	    فراغ *arg)
{
#إذا defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    حرف		*saved_locale;
#endif
    PyObject		*cmdstr;
    PyObject		*cmdbytes;
    PyGILState_STATE	pygilstate;

    إذا (python_end_called)
	اذهب theend;

    إذا (Python3_Init())
	اذهب theend;

    init_range(arg);

    Python_Release_Vim();	    // leave Vim

#إذا defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    // Python only works properly when the LC_NUMERIC locale is "C".
    saved_locale = setlocale(LC_NUMERIC, عدم);
    إذا (saved_locale == عدم || STRCMP(saved_locale, "C") == 0)
	saved_locale = عدم;
    وإلا
    {
	// Need to make a copy, value may change when setting new locale.
	saved_locale = (حرف *)vim_strsave((char_u *)saved_locale);
	(فراغ)setlocale(LC_NUMERIC, "C");
    }
#endif

    pygilstate = PyGILState_Ensure();

    // PyRun_SimpleString expects a UTF-8 string. Wrong encoding may cause
    // SyntaxError (unicode error).
    cmdstr = PyUnicode_Decode(cmd, strlen(cmd),
					(حرف *)ENC_OPT, ERRORS_DECODE_ARG);
    cmdbytes = PyUnicode_AsEncodedString(cmdstr, "utf-8", ERRORS_ENCODE_ARG);
    Py_XDECREF(cmdstr);

    run(PyBytes_AsString(cmdbytes), locals, arg, &pygilstate);
    Py_XDECREF(cmdbytes);

    PyGILState_Release(pygilstate);

#إذا defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    إذا (saved_locale != عدم)
    {
	(فراغ)setlocale(LC_NUMERIC, saved_locale);
	vim_free(saved_locale);
    }
#endif

    Python_Lock_Vim();		    // enter Vim
    PythonIO_Flush();

theend:
    أرجع;	    // keeps lint happy
}

/*
 * ":py3"
 */
    فراغ
ex_py3(exarg_T *eap)
{
    char_u *script;

    script = script_get(eap, eap->arg);
    إذا (!eap->skip)
    {
	إذا (p_pyx == 0)
	    p_pyx = 3;

	DoPyCommand(script == عدم ? (حرف *) eap->arg : (حرف *) script,
		عدم,
		init_range_cmd,
		(runner) run_cmd,
		(فراغ *) eap);
    }
    vim_free(script);
}

#define BUFFER_SIZE 2048

/*
 * ":py3file"
 */
    فراغ
ex_py3file(exarg_T *eap)
{
    ساكن حرف buffer[BUFFER_SIZE];
    ثابت حرف *file;
    حرف *p;
    صحيح i;

    إذا (p_pyx == 0)
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

    لكل (i=0; i<2; ++i)
    {
	file = (حرف *)eap->arg;
	طالما (*file && p < buffer + (BUFFER_SIZE - 3))
	{
	    إذا (*file == '\\' || *file == '\'')
		*p++ = '\\';
	    *p++ = *file++;
	}
	// If we didn't finish the file name, we hit a buffer overflow
	إذا (*file != '\0')
	    أرجع;
	إذا (i==0)
	{
	    strcpy(p,"','rb').read(),'");
	    p += 16;
	}
	وإلا
	{
	    strcpy(p,"','exec'))");
	    p += 10;
	}
    }


    // Execute the file
    DoPyCommand(buffer,
	    عدم,
	    init_range_cmd,
	    (runner) run_cmd,
	    (فراغ *) eap);
}

    فراغ
ex_py3do(exarg_T *eap)
{
    إذا (p_pyx == 0)
	p_pyx = 3;

    DoPyCommand((حرف *)eap->arg,
	    عدم,
	    init_range_cmd,
	    (runner)run_do,
	    (فراغ *)eap);
}

///////////////////////////////////////////////////////
// 2. Python output stream: writes output via [e]msg().

// Implementation functions

    ساكن PyObject *
OutputGetattro(PyObject *self, PyObject *nameobj)
{
    GET_ATTR_STRING(name, nameobj);

    إذا (strcmp(name, "softspace") == 0)
	أرجع PyLong_FromLong(((OutputObject *)(self))->softspace);
    وإلا إذا (strcmp(name, "errors") == 0)
	أرجع PyString_FromString("strict");
    وإلا إذا (strcmp(name, "encoding") == 0)
	أرجع PyString_FromString(ENC_OPT);

    أرجع PyObject_GenericGetAttr(self, nameobj);
}

    ساكن صحيح
OutputSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);

    أرجع OutputSetattr(self, name, val);
}

///////////////////////////////////////////////////////
// 3. Implementation of the Vim module for Python

// Window type - Implementation functions
// --------------------------------------

#define WindowType_Check(obj) ((obj)->ob_base.ob_type == &WindowType)

// Buffer type - Implementation functions
// --------------------------------------

#define BufferType_Check(obj) ((obj)->ob_base.ob_type == &BufferType)

ساكن PyObject* BufferSubscript(PyObject *self, PyObject *idx);
ساكن صحيح BufferAsSubscript(PyObject *self, PyObject *idx, PyObject *val);

// Line range type - Implementation functions
// --------------------------------------

#define RangeType_Check(obj) ((obj)->ob_base.ob_type == &RangeType)

ساكن PyObject* RangeSubscript(PyObject *self, PyObject *idx);
ساكن صحيح RangeAsItem(PyObject *, Py_ssize_t, PyObject *);
ساكن صحيح RangeAsSubscript(PyObject *self, PyObject *idx, PyObject *val);

// Current objects type - Implementation functions
// -----------------------------------------------

ساكن PySequenceMethods BufferAsSeq = {
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

ساكن PyMappingMethods BufferAsMapping = {
    /* mp_length	*/ (lenfunc)BufferLength,
    /* mp_subscript     */ (binaryfunc)BufferSubscript,
    /* mp_ass_subscript */ (objobjargproc)BufferAsSubscript,
};


// Buffer object

    ساكن PyObject *
BufferGetattro(PyObject *self, PyObject *nameobj)
{
    PyObject *r;

    GET_ATTR_STRING(name, nameobj);

    إذا ((r = BufferAttrValid((BufferObject *)(self), name)))
	أرجع r;

    إذا (CheckBuffer((BufferObject *)(self)))
	أرجع عدم;

    r = BufferAttr((BufferObject *)(self), name);
    إذا (r || PyErr_Occurred())
	أرجع r;
    وإلا
	أرجع PyObject_GenericGetAttr(self, nameobj);
}

    ساكن صحيح
BufferSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);

    أرجع BufferSetattr(self, name, val);
}

//////////////////

    ساكن PyObject *
BufferSubscript(PyObject *self, PyObject* idx)
{
    إذا (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	أرجع BufferItem((BufferObject *)(self), _idx);
    }
    وإلا إذا (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	إذا (CheckBuffer((BufferObject *) self))
	    أرجع عدم;

	إذا (PySlice_GetIndicesEx((PySliceObject_T *)idx,
	      (Py_ssize_t)((BufferObject *)(self))->buf->b_ml.ml_هێڵ_ژماردن,
	      &start, &stop,
	      &step, &slicelen) < 0)
	    أرجع عدم;
	أرجع BufferSlice((BufferObject *)(self), start, stop);
    }
    وإلا
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	أرجع عدم;
    }
}

    ساكن صحيح
BufferAsSubscript(PyObject *self, PyObject* idx, PyObject* val)
{
    إذا (PyLong_Check(idx))
    {
	long n = PyLong_AsLong(idx);

	إذا (CheckBuffer((BufferObject *) self))
	    أرجع -1;

	أرجع RBAsItem((BufferObject *)(self), n, val, 1,
		    (Py_ssize_t)((BufferObject *)(self))->buf->b_ml.ml_هێڵ_ژماردن,
		    عدم);
    }
    وإلا إذا (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	إذا (CheckBuffer((BufferObject *) self))
	    أرجع -1;

	إذا (PySlice_GetIndicesEx((PySliceObject_T *)idx,
	      (Py_ssize_t)((BufferObject *)(self))->buf->b_ml.ml_هێڵ_ژماردن,
	      &start, &stop,
	      &step, &slicelen) < 0)
	    أرجع -1;
	أرجع RBAsSlice((BufferObject *)(self), start, stop, val, 1,
			  (PyInt)((BufferObject *)(self))->buf->b_ml.ml_هێڵ_ژماردن,
			  عدم);
    }
    وإلا
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	أرجع -1;
    }
}

ساكن PySequenceMethods RangeAsSeq = {
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

ساكن PyMappingMethods RangeAsMapping = {
    /* mp_length	*/ (lenfunc)RangeLength,
    /* mp_subscript     */ (binaryfunc)RangeSubscript,
    /* mp_ass_subscript */ (objobjargproc)RangeAsSubscript,
};

// Line range object - Implementation

    ساكن PyObject *
RangeGetattro(PyObject *self, PyObject *nameobj)
{
    GET_ATTR_STRING(name, nameobj);

    إذا (strcmp(name, "start") == 0)
	أرجع Py_Build_بەها("n", ((RangeObject *)(self))->start - 1);
    وإلا إذا (strcmp(name, "end") == 0)
	أرجع Py_Build_بەها("n", ((RangeObject *)(self))->end - 1);
    وإلا
	أرجع PyObject_GenericGetAttr(self, nameobj);
}

////////////////

    ساكن صحيح
RangeAsItem(PyObject *self, Py_ssize_t n, PyObject *val)
{
    أرجع RBAsItem(((RangeObject *)(self))->buf, n, val,
		    ((RangeObject *)(self))->start,
		    ((RangeObject *)(self))->end,
		    &((RangeObject *)(self))->end);
}

    ساكن Py_ssize_t
RangeAsSlice(PyObject *self, Py_ssize_t lo, Py_ssize_t hi, PyObject *val)
{
    أرجع RBAsSlice(((RangeObject *)(self))->buf, lo, hi, val,
		    ((RangeObject *)(self))->start,
		    ((RangeObject *)(self))->end,
		    &((RangeObject *)(self))->end);
}

    ساكن PyObject *
RangeSubscript(PyObject *self, PyObject* idx)
{
    إذا (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	أرجع RangeItem((RangeObject *)(self), _idx);
    }
    وإلا إذا (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	إذا (PySlice_GetIndicesEx((PySliceObject_T *)idx,
		((RangeObject *)(self))->end-((RangeObject *)(self))->start+1,
		&start, &stop,
		&step, &slicelen) < 0)
	    أرجع عدم;
	أرجع RangeSlice((RangeObject *)(self), start, stop);
    }
    وإلا
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	أرجع عدم;
    }
}

    ساكن صحيح
RangeAsSubscript(PyObject *self, PyObject *idx, PyObject *val)
{
    إذا (PyLong_Check(idx))
    {
	long n = PyLong_AsLong(idx);
	أرجع RangeAsItem(self, n, val);
    }
    وإلا إذا (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	إذا (PySlice_GetIndicesEx((PySliceObject_T *)idx,
		((RangeObject *)(self))->end-((RangeObject *)(self))->start+1,
		&start, &stop,
		&step, &slicelen) < 0)
	    أرجع -1;
	أرجع RangeAsSlice(self, start, stop, val);
    }
    وإلا
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	أرجع -1;
    }
}

// TabPage object - Implementation

    ساكن PyObject *
TabPageGetattro(PyObject *self, PyObject *nameobj)
{
    PyObject *r;

    GET_ATTR_STRING(name, nameobj);

    إذا ((r = TabPageAttrValid((TabPageObject *)(self), name)))
	أرجع r;

    إذا (CheckTabPage((TabPageObject *)(self)))
	أرجع عدم;

    r = TabPageAttr((TabPageObject *)(self), name);
    إذا (r || PyErr_Occurred())
	أرجع r;
    وإلا
	أرجع PyObject_GenericGetAttr(self, nameobj);
}

// Window object - Implementation

    ساكن PyObject *
WindowGetattro(PyObject *self, PyObject *nameobj)
{
    PyObject *r;

    GET_ATTR_STRING(name, nameobj);

    إذا ((r = WindowAttrValid((WindowObject *)(self), name)))
	أرجع r;

    إذا (CheckWindow((WindowObject *)(self)))
	أرجع عدم;

    r = WindowAttr((WindowObject *)(self), name);
    إذا (r || PyErr_Occurred())
	أرجع r;
    وإلا
	أرجع PyObject_GenericGetAttr(self, nameobj);
}

    ساكن صحيح
WindowSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);

    أرجع WindowSetattr(self, name, val);
}

// Tab page list object - Definitions

ساكن PySequenceMethods TabListAsSeq = {
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

ساكن PySequenceMethods WinListAsSeq = {
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
    ساكن PyObject *
CurrentGetattro(PyObject *self, PyObject *nameobj)
{
    PyObject	*r;
    GET_ATTR_STRING(name, nameobj);
    إذا (!(r = CurrentGetattr(self, name)))
	أرجع PyObject_GenericGetAttr(self, nameobj);
    أرجع r;
}

    ساكن صحيح
CurrentSetattro(PyObject *self, PyObject *nameobj, PyObject *بەها)
{
    GET_ATTR_STRING(name, nameobj);
    أرجع CurrentSetattr(self, name, بەها);
}

// Dictionary object - Definitions

    ساكن PyObject *
DictionaryGetattro(PyObject *self, PyObject *nameobj)
{
    DictionaryObject	*this = ((DictionaryObject *) (self));

    GET_ATTR_STRING(name, nameobj);

    إذا (strcmp(name, "locked") == 0)
	أرجع PyLong_FromLong(this->dict->dv_lock);
    وإلا إذا (strcmp(name, "scope") == 0)
	أرجع PyLong_FromLong(this->dict->dv_scope);

    أرجع PyObject_GenericGetAttr(self, nameobj);
}

    ساكن صحيح
DictionarySetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);
    أرجع DictionarySetattr(self, name, val);
}

// List object - Definitions

    ساكن PyObject *
ListGetattro(PyObject *self, PyObject *nameobj)
{
    GET_ATTR_STRING(name, nameobj);

    إذا (strcmp(name, "locked") == 0)
	أرجع PyLong_FromLong(((ListObject *) (self))->list->lv_lock);

    أرجع PyObject_GenericGetAttr(self, nameobj);
}

    ساكن صحيح
ListSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);
    أرجع ListSetattr(self, name, val);
}

// Tuple object - Definitions

    ساكن PyObject *
TupleGetattro(PyObject *self, PyObject *nameobj)
{
    GET_ATTR_STRING(name, nameobj);

    إذا (strcmp(name, "locked") == 0)
	أرجع PyLong_FromLong(((TupleObject *) (self))->tuple->tv_lock);

    أرجع PyObject_GenericGetAttr(self, nameobj);
}

    ساكن صحيح
TupleSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);
    أرجع TupleSetattr(self, name, val);
}

// Function object - Definitions

    ساكن PyObject *
FunctionGetattro(PyObject *self, PyObject *nameobj)
{
    PyObject		*r;
    FunctionObject	*this = (FunctionObject *)(self);

    GET_ATTR_STRING(name, nameobj);

    r = FunctionAttr(this, name);
    إذا (r || PyErr_Occurred())
	أرجع r;
    وإلا
	أرجع PyObject_GenericGetAttr(self, nameobj);
}

// External interface

    فراغ
python3_buffer_free(buf_T *buf)
{
    BufferObject *bp = BUF_PYTHON_REF(buf);
    إذا (bp == عدم)
	أرجع;
    bp->buf = INVALID_BUFFER_بەها;
    BUF_PYTHON_REF(buf) = عدم;
}

    فراغ
python3_window_free(win_T *win)
{
    WindowObject *wp = WIN_PYTHON_REF(win);
    إذا (wp == عدم)
	أرجع;
    wp->win = INVALID_WINDOW_بەها;
    WIN_PYTHON_REF(win) = عدم;
}

    فراغ
python3_tabpage_free(tabpage_T *tab)
{
    TabPageObject *tp = TAB_PYTHON_REF(tab);
    إذا (tp == عدم)
	أرجع;
    tp->tab = INVALID_TABPAGE_بەها;
    TAB_PYTHON_REF(tab) = عدم;
}

    ساكن PyObject *
Py3Init_vim(فراغ)
{
    // The special value is removed from sys.path in Python3_Init().
    ساكن wchar_t *(argv[2]) = {L"/must>not&exist/foo", عدم};

    إذا (init_types())
	أرجع عدم;

    // Set sys.argv[] to avoid a crash in warn().
    PySys_SetArgv(1, argv);

    إذا ((vim_module = PyModule_Create(&vimmodule)) == عدم)
	أرجع عدم;

    إذا (populate_module(vim_module))
	أرجع عدم;

    إذا (init_sys_path())
	أرجع عدم;

    أرجع vim_module;
}

//////////////////////////////////////////////////////////////////////////
// 4. Utility functions for handling the interface between Vim and Python.

/*
 * Convert a Vim line into a Python string.
 * All internal newlines are replaced by null characters.
 *
 * On errors, the Python exception data is set, and NULL is returned.
 */
    ساكن PyObject *
هێڵ_To_String(ثابت حرف *str)
{
    PyObject *result;
    Py_ssize_t len = strlen(str);
    حرف *tmp, *p;

    tmp = alloc(len + 1);
    p = tmp;
    إذا (p == عدم)
    {
	PyErr_NoMemory();
	أرجع عدم;
    }

    طالما (*str)
    {
	إذا (*str == '\n')
	    *p = '\0';
	وإلا
	    *p = *str;

	++p;
	++str;
    }
    *p = '\0';

    result = PyUnicode_Decode(tmp, len, (حرف *)ENC_OPT, ERRORS_DECODE_ARG);

    vim_free(tmp);
    أرجع result;
}

    فراغ
do_py3eval(char_u *str, dict_T *locals, typval_T *rettv)
{
    DoPyCommand((حرف *) str,
	    locals,
	    init_range_eval,
	    (runner) run_eval,
	    (فراغ *) rettv);
    إذا (rettv->v_type == VAR_UNKNOWN)
    {
	rettv->v_type = VAR_ژمار;
	rettv->vval.v_ژمار = 0;
    }
}

    صحيح
set_ref_in_python3(صحيح copyID)
{
    أرجع set_ref_in_py(copyID);
}

    صحيح
python3_version(فراغ)
{
#ifdef USE_LIMITED_API
    أرجع Py_LIMITED_API;
#وإلا
    أرجع PY_VERSION_HEX;
#endif
}
