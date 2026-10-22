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

#अन्तर्भाव "vim.h"

#अन्तर्भाव <limits.h>

#जर defined(MSWIN) && defined(HAVE_FCNTL_H)
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

#अन्तर्भाव <Python.h>

#undef main // Defined in python.h - aargh
#undef HAVE_FCNTL_H // Clash with os_win32.h

// The "surrogateescape" error handler is new in Python 3.1
#जर PY_VERSION_HEX >= 0x030100f0
# define CODEC_ERROR_HANDLER "surrogateescape"
#अन्यथा
# define CODEC_ERROR_HANDLER रिक्त
#endif

// Suppress Python 3.11 depreciations to see useful warnings
#ifdef __GNUC__
# pragma GCC diagnostic ढकला
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
#जर PY_VERSION_HEX >= 0x030200f0
प्रकारव्याख्या PyObject PySliceObject_T;
#अन्यथा
प्रकारव्याख्या PySliceObject PySliceObject_T;
#endif

#ifndef MSWIN
# define HINSTANCE शून्य *
#endif
#जर defined(DYNAMIC_PYTHON3) || defined(MSWIN)
स्थिर HINSTANCE hinstPy3 = 0; // Instance of python.dll
#endif

#जर defined(DYNAMIC_PYTHON3)

# ifdef MSWIN
#  define load_dll vimLoadLib
#  define close_dll FreeLibrary
#  define चिन्ह_from_dll GetProcAddress
#  define load_dll_error GetWin32Error
# अन्यथा
#  include <dlfcn.h>
#  define FARPROC शून्य*
#  जर defined(PY_NO_RTLD_GLOBAL) && defined(PY3_NO_RTLD_GLOBAL)
#   define load_dll(n) dlopen((n), RTLD_LAZY)
#  अन्यथा
#   define load_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#  endif
#  define close_dll dlclose
#  define चिन्ह_from_dll dlsym
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
# define Py_List_घाला py3_Py_List_घाला
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
# जर PY_VERSION_HEX >= 0x030601f0
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
# ifndef Py_Mapping_किल्ल्या
#  define Py_Mapping_किल्ल्या py3_Py_Mapping_किल्ल्या
# endif
# जर (defined(USE_LIMITED_API) && Py_LIMITED_API >= 0x03080000) || \
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
# जर defined(USE_LIMITED_API) || PY_VERSION_HEX >= 0x03080000
#  define Py_IncRef py3_Py_IncRef
#  define Py_DecRef py3_Py_DecRef
# endif
# ifdef USE_LIMITED_API
#  define Py_CompileString py3_Py_CompileString
#  define PyEval_EvalCode py3_PyEval_EvalCode
# अन्यथा
#  undef PyRun_SimpleString
#  define PyRun_SimpleString py3_PyRun_SimpleString
#  undef PyRun_String
#  define PyRun_String py3_PyRun_String
# endif
# define PyObject_GetAttrString py3_PyObject_GetAttrString
# define PyObject_HasAttrString py3_PyObject_HasAttrString
# define PyObject_SetAttrString py3_PyObject_SetAttrString
# define PyObject_CallFunctionObjArgs py3_PyObject_CallFunctionObjArgs
# जर PY_VERSION_HEX >= 0x030d0000
#  define PyObject_CallFunction py3_PyObject_CallFunction
# अन्यथा
#  define _PyObject_CallFunction_SizeT py3__PyObject_CallFunction_SizeT
# endif
# define PyObject_Call py3_PyObject_Call
# define PyEval_GetLocals py3_PyEval_GetLocals
# define PyEval_GetGlobals py3_PyEval_GetGlobals
# define PySys_SetObject py3_PySys_SetObject
# define PySys_GetObject py3_PySys_GetObject
# define PySys_SetArgv py3_PySys_SetArgv
# define PyType_Ready py3_PyType_Ready
# जर PY_VERSION_HEX >= 0x03040000
#  define PyType_GetFlags py3_PyType_GetFlags
# endif
# undef Py_Build_मूल्य
# define Py_Build_मूल्य py3_Py_Build_मूल्य
# define Py_SetPythonHome py3_Py_SetPythonHome
# define Py_Initialize py3_Py_Initialize
# define Py_Finalize py3_Py_Finalize
# define Py_IsInitialized py3_Py_IsInitialized
# define _Py_NoneStruct (*py3__Py_NoneStruct)
# define _Py_FalseStruct (*py3__Py_FalseStruct)
# define _Py_TrueStruct (*py3__Py_TrueStruct)
# जर !defined(USE_LIMITED_API) && PY_VERSION_HEX < 0x030D0000
// Private symbol that used to be required as part of PyIter_Check.
#  define _PyObject_NextNotImplemented (*py3__PyObject_NextNotImplemented)
# endif
# define PyModule_AddObject py3_PyModule_AddObject
# define PyImport_AppendInittab py3_PyImport_AppendInittab
# define PyImport_AddModule py3_PyImport_AddModule
# ifdef USE_LIMITED_API
#  जर Py_LIMITED_API >= 0x030a0000
#   define PyUnicode_AsUTF8AndSize py3_PyUnicode_AsUTF8AndSize
#  endif
# अन्यथा
#  जर PY_VERSION_HEX >= 0x030300f0
#   define PyUnicode_AsUTF8AndSize py3_PyUnicode_AsUTF8AndSize
#  अन्यथा
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
# define Py_संख्या_Check (*py3_Py_संख्या_Check)
# define Py_संख्या_Long (*py3_Py_संख्या_Long)
# define PyBool_Type (*py3_PyBool_Type)
# define PyErr_NewException py3_PyErr_NewException
# ifdef Py_DEBUG
#  define _Py_NegativeRefcount py3__Py_NegativeRefcount
#  define _Py_RefTotal (*py3__Py_RefTotal)
#  define PyModule_Create2TraceRefs py3_PyModule_Create2TraceRefs
# अन्यथा
#  define PyModule_Create2 py3_PyModule_Create2
# endif
# जर defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
#  define _PyObject_DebugMalloc py3__PyObject_DebugMalloc
#  define _PyObject_DebugFree py3__PyObject_DebugFree
# अन्यथा
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
#  अन्यथा
#   define PyUnicodeUCS2_FromFormat py3_PyUnicodeUCS2_FromFormat
#  endif
# अन्यथा
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

# जर defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
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
स्थिर पूर्णांक (*py3_PySys_SetArgv)(पूर्णांक, ब_अक्षर_प्रकार **);
स्थिर शून्य (*py3_Py_SetPythonHome)(ब_अक्षर_प्रकार *home);
स्थिर शून्य (*py3_Py_Initialize)(शून्य);
स्थिर PyObject* (*py3_PyList_New)(Py_ssize_t size);
स्थिर PyGILState_STATE (*py3_PyGILState_Ensure)(शून्य);
स्थिर शून्य (*py3_PyGILState_Release)(PyGILState_STATE);
स्थिर पूर्णांक (*py3_PySys_SetObject)(अक्षर *, PyObject *);
स्थिर PyObject* (*py3_PySys_GetObject)(अक्षर *);
स्थिर पूर्णांक (*py3_PyList_Append)(PyObject *, PyObject *);
स्थिर पूर्णांक (*py3_Py_List_घाला)(PyObject *, पूर्णांक, PyObject *);
स्थिर Py_ssize_t (*py3_PyList_Size)(PyObject *);
स्थिर PyObject* (*py3_PyTuple_New)(Py_ssize_t size);
स्थिर पूर्णांक (*py3_PyTuple_SetItem)(PyObject *, Py_ssize_t, PyObject *);
स्थिर Py_ssize_t (*py3_PyTuple_Size)(PyObject *);
स्थिर पूर्णांक (*py3_PySequence_Check)(PyObject *);
स्थिर Py_ssize_t (*py3_PySequence_Size)(PyObject *);
स्थिर PyObject* (*py3_PySequence_GetItem)(PyObject *, Py_ssize_t);
स्थिर PyObject* (*py3_PySequence_Fast)(PyObject *, स्थिरांक अक्षर *);
स्थिर PyObject* (*py3_PyTuple_GetItem)(PyObject *, Py_ssize_t);
स्थिर पूर्णांक (*py3_PyMapping_Check)(PyObject *);
स्थिर PyObject* (*py3_Py_Mapping_किल्ल्या)(PyObject *);
# जर PY_VERSION_HEX >= 0x030601f0
स्थिर पूर्णांक (*py3_PySlice_AdjustIndices)(Py_ssize_t length,
		     Py_ssize_t *start, Py_ssize_t *stop, Py_ssize_t step);
स्थिर पूर्णांक (*py3_PySlice_Unpack)(PyObject *slice,
		     Py_ssize_t *start, Py_ssize_t *stop, Py_ssize_t *step);
# endif
स्थिर पूर्णांक (*py3_PySlice_GetIndicesEx)(PySliceObject_T *r, Py_ssize_t length,
		     Py_ssize_t *start, Py_ssize_t *stop, Py_ssize_t *step,
		     Py_ssize_t *slicelen);
स्थिर PyObject* (*py3_PyErr_NoMemory)(शून्य);
स्थिर शून्य (*py3_Py_Finalize)(शून्य);
स्थिर शून्य (*py3_PyErr_SetString)(PyObject *, स्थिरांक अक्षर *);
स्थिर शून्य (*py3_PyErr_SetObject)(PyObject *, PyObject *);
स्थिर पूर्णांक (*py3_PyErr_ExceptionMatches)(PyObject *);
# जर defined(USE_LIMITED_API) || PY_VERSION_HEX >= 0x03080000
स्थिर शून्य (*py3_Py_IncRef)(PyObject *);
स्थिर शून्य (*py3_Py_DecRef)(PyObject *);
# endif
# ifdef USE_LIMITED_API
स्थिर PyObject* (*py3_Py_CompileString)(स्थिरांक अक्षर *, स्थिरांक अक्षर *, पूर्णांक);
स्थिर PyObject* (*py3_PyEval_EvalCode)(PyObject *co, PyObject *globals, PyObject *locals);
# अन्यथा
स्थिर पूर्णांक (*py3_PyRun_SimpleString)(अक्षर *);
स्थिर PyObject* (*py3_PyRun_String)(अक्षर *, पूर्णांक, PyObject *, PyObject *);
# endif
स्थिर PyObject* (*py3_PyObject_GetAttrString)(PyObject *, स्थिरांक अक्षर *);
स्थिर पूर्णांक (*py3_PyObject_HasAttrString)(PyObject *, स्थिरांक अक्षर *);
स्थिर पूर्णांक (*py3_PyObject_SetAttrString)(PyObject *, स्थिरांक अक्षर *, PyObject *);
स्थिर PyObject* (*py3_PyObject_CallFunctionObjArgs)(PyObject *, ...);
# जर PY_VERSION_HEX >= 0x030d0000
स्थिर PyObject* (*py3_PyObject_CallFunction)(PyObject *, अक्षर *, ...);
# अन्यथा
स्थिर PyObject* (*py3__PyObject_CallFunction_SizeT)(PyObject *, अक्षर *, ...);
# endif
स्थिर PyObject* (*py3_PyObject_Call)(PyObject *, PyObject *, PyObject *);
स्थिर PyObject* (*py3_PyEval_GetGlobals)(शून्य);
स्थिर PyObject* (*py3_PyEval_GetLocals)(शून्य);
स्थिर PyObject* (*py3_PyList_GetItem)(PyObject *, Py_ssize_t);
स्थिर PyObject* (*py3_PyImport_ImportModule)(स्थिरांक अक्षर *);
स्थिर PyObject* (*py3_PyImport_AddModule)(स्थिरांक अक्षर *);
स्थिर पूर्णांक (*py3_PyErr_BadArgument)(शून्य);
स्थिर PyObject* (*py3_PyErr_Occurred)(शून्य);
स्थिर PyObject* (*py3_PyModule_GetDict)(PyObject *);
स्थिर पूर्णांक (*py3_PyList_SetItem)(PyObject *, Py_ssize_t, PyObject *);
स्थिर PyObject* (*py3_PyDict_GetItemString)(PyObject *, स्थिरांक अक्षर *);
स्थिर पूर्णांक (*py3_PyDict_Next)(PyObject *, Py_ssize_t *, PyObject **, PyObject **);
स्थिर PyObject* (*py3_PyLong_FromLong)(दीर्घ);
स्थिर PyObject* (*py3_PyDict_New)(शून्य);
# जर (defined(USE_LIMITED_API) && Py_LIMITED_API >= 0x03080000) || \
       (!defined(USE_LIMITED_API) && PY_VERSION_HEX >= 0x03080000)
स्थिर पूर्णांक (*py3_PyIter_Check)(PyObject *o);
# endif
स्थिर PyObject* (*py3_PyIter_Next)(PyObject *);
स्थिर PyObject* (*py3_PyObject_GetIter)(PyObject *);
स्थिर PyObject* (*py3_PyObject_Repr)(PyObject *);
स्थिर PyObject* (*py3_PyObject_GetItem)(PyObject *, PyObject *);
स्थिर पूर्णांक (*py3_PyObject_IsTrue)(PyObject *);
स्थिर PyObject* (*py3_Py_Build_मूल्य)(अक्षर *, ...);
# जर PY_VERSION_HEX >= 0x03040000
स्थिर पूर्णांक (*py3_PyType_GetFlags)(PyTypeObject *o);
# endif
स्थिर पूर्णांक (*py3_PyType_Ready)(PyTypeObject *type);
स्थिर पूर्णांक (*py3_PyDict_SetItemString)(PyObject *dp, अक्षर *किल्ली, PyObject *item);
स्थिर PyObject* (*py3_PyUnicode_FromString)(स्थिरांक अक्षर *u);
# ifdef Py_UNICODE_USE_UCS_FUNCTIONS
#  ifdef Py_UNICODE_WIDE
स्थिर PyObject* (*py3_PyUnicodeUCS4_FromFormat)(स्थिरांक अक्षर *u, ...);
#  अन्यथा
स्थिर PyObject* (*py3_PyUnicodeUCS2_FromFormat)(स्थिरांक अक्षर *u, ...);
#  endif
# अन्यथा
स्थिर PyObject* (*py3_PyUnicode_FromFormat)(स्थिरांक अक्षर *u, ...);
# endif
स्थिर PyObject* (*py3_PyUnicode_Decode)(स्थिरांक अक्षर *u, Py_ssize_t size,
	स्थिरांक अक्षर *encoding, स्थिरांक अक्षर *errors);
स्थिर दीर्घ (*py3_PyLong_AsLong)(PyObject *);
स्थिर शून्य (*py3_PyErr_SetNone)(PyObject *);
स्थिर शून्य (*py3_PyEval_InitThreads)(शून्य);
स्थिर शून्य(*py3_PyEval_RestoreThread)(PyThreadState *);
स्थिर PyThreadState*(*py3_PyEval_SaveThread)(शून्य);
स्थिर पूर्णांक (*py3_PyArg_Parse)(PyObject *, अक्षर *, ...);
स्थिर पूर्णांक (*py3_PyArg_ParseTuple)(PyObject *, अक्षर *, ...);
स्थिर शून्य (*py3_PyMem_Free)(शून्य *);
स्थिर शून्य* (*py3_PyMem_Malloc)(माप_प्रकार);
स्थिर पूर्णांक (*py3_Py_IsInitialized)(शून्य);
स्थिर शून्य (*py3_PyErr_Clear)(शून्य);
स्थिर PyObject* (*py3_PyErr_Format)(PyObject *, स्थिरांक अक्षर *, ...);
स्थिर शून्य (*py3_PyErr_PrintEx)(पूर्णांक);
स्थिर PyObject*(*py3__PyObject_Init)(PyObject *, PyTypeObject *);
# जर !defined(USE_LIMITED_API) && PY_VERSION_HEX < 0x030D0000
स्थिर iternextfunc py3__PyObject_NextNotImplemented;
# endif
स्थिर PyObject* py3__Py_NoneStruct;
स्थिर PyObject* py3__Py_FalseStruct;
स्थिर PyObject* py3__Py_TrueStruct;
स्थिर पूर्णांक (*py3_PyModule_AddObject)(PyObject *m, स्थिरांक अक्षर *name, PyObject *o);
स्थिर पूर्णांक (*py3_PyImport_AppendInittab)(स्थिरांक अक्षर *name, PyObject* (*initfunc)(शून्य));
# ifdef USE_LIMITED_API
#  जर Py_LIMITED_API >= 0x030a0000
स्थिर अक्षर* (*py3_PyUnicode_AsUTF8AndSize)(PyObject *unicode, Py_ssize_t *size);
#  endif
# अन्यथा
#  जर PY_VERSION_HEX >= 0x030300f0
स्थिर अक्षर* (*py3_PyUnicode_AsUTF8AndSize)(PyObject *unicode, Py_ssize_t *size);
#  अन्यथा
स्थिर अक्षर* (*py3__PyUnicode_AsString)(PyObject *unicode);
#  endif
# endif
स्थिर पूर्णांक (*py3_PyUnicode_CompareWithASCIIString)(PyObject *unicode, स्थिरांक अक्षर* string);
स्थिर PyObject* (*py3_PyUnicode_AsEncodedString)(PyObject *unicode, स्थिरांक अक्षर* encoding, स्थिरांक अक्षर* errors);
स्थिर PyObject* (*py3_PyUnicode_AsUTF8String)(PyObject *unicode);
स्थिर अक्षर* (*py3_PyBytes_AsString)(PyObject *bytes);
स्थिर पूर्णांक (*py3_PyBytes_AsStringAndSize)(PyObject *bytes, अक्षर **buffer, Py_ssize_t *length);
स्थिर PyObject* (*py3_PyBytes_FromString)(अक्षर *str);
स्थिर PyObject* (*py3_PyBytes_FromStringAndSize)(अक्षर *str, Py_ssize_t length);
# जर PY_VERSION_HEX >= 0x030900b0
स्थिर PyObject* (*py3__PyObject_New)(PyTypeObject *);
# endif
स्थिर PyObject* (*py3_PyFloat_FromDouble)(द्विगुण अंक);
स्थिर द्विगुण (*py3_PyFloat_AsDouble)(PyObject *);
स्थिर PyObject* (*py3_PyObject_GenericGetAttr)(PyObject *obj, PyObject *name);
स्थिर PyObject* (*py3_PyType_GenericAlloc)(PyTypeObject *type, Py_ssize_t nitems);
स्थिर PyObject* (*py3_PyType_GenericNew)(PyTypeObject *type, PyObject *args, PyObject *kwds);
स्थिर PyTypeObject* py3_PyType_Type;
# ifndef USE_LIMITED_API
स्थिर PyTypeObject* py3_PyStdPrinter_Type;
# endif
स्थिर PyTypeObject* py3_PySlice_Type;
स्थिर PyTypeObject* py3_PyFloat_Type;
स्थिर PyTypeObject* py3_PyBool_Type;
स्थिर पूर्णांक (*py3_Py_संख्या_Check)(PyObject *);
स्थिर PyObject* (*py3_Py_संख्या_Long)(PyObject *);
स्थिर PyObject* (*py3_PyErr_NewException)(अक्षर *name, PyObject *base, PyObject *dict);
स्थिर PyObject* (*py3_PyCapsule_New)(शून्य *, अक्षर *, PyCapsule_Destructor);
स्थिर शून्य* (*py3_PyCapsule_GetPointer)(PyObject *, अक्षर *);
# ifdef Py_DEBUG
स्थिर शून्य (*py3__Py_NegativeRefcount)(स्थिरांक अक्षर *fname, पूर्णांक lineno, PyObject *op);
स्थिर Py_ssize_t* py3__Py_RefTotal;
स्थिर PyObject* (*py3_PyModule_Create2TraceRefs)(संरचना PyModuleDef* module, पूर्णांक module_api_version);
# अन्यथा
स्थिर PyObject* (*py3_PyModule_Create2)(संरचना PyModuleDef* module, पूर्णांक module_api_version);
# endif
# जर defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
स्थिर शून्य (*py3__PyObject_DebugFree)(शून्य*);
स्थिर शून्य* (*py3__PyObject_DebugMalloc)(माप_प्रकार);
# अन्यथा
स्थिर शून्य (*py3_PyObject_Free)(शून्य*);
स्थिर शून्य* (*py3_PyObject_Malloc)(माप_प्रकार);
# endif
स्थिर PyObject*(*py3__PyObject_GC_New)(PyTypeObject *);
स्थिर शून्य(*py3_PyObject_GC_Del)(शून्य *);
स्थिर शून्य(*py3_PyObject_GC_UnTrack)(शून्य *);
स्थिर पूर्णांक (*py3_PyType_IsSubtype)(PyTypeObject *, PyTypeObject *);
# ifdef USE_LIMITED_API
स्थिर शून्य* (*py3_PyType_GetSlot)(PyTypeObject *, पूर्णांक);
स्थिर PyObject* (*py3_PyType_FromSpec)(PyType_Spec *);
# endif

// Imported exception objects
स्थिर PyObject *p3imp_PyExc_AttributeError;
स्थिर PyObject *p3imp_PyExc_IndexError;
स्थिर PyObject *p3imp_Py_Exc_किल्ली_Error;
स्थिर PyObject *p3imp_PyExc_KeyboardInterrupt;
स्थिर PyObject *p3imp_PyExc_TypeError;
स्थिर PyObject *p3imp_Py_Exc_मूल्य_Error;
स्थिर PyObject *p3imp_PyExc_SystemExit;
स्थिर PyObject *p3imp_PyExc_RuntimeError;
स्थिर PyObject *p3imp_PyExc_ImportError;
स्थिर PyObject *p3imp_PyExc_OverflowError;

# define PyExc_AttributeError p3imp_PyExc_AttributeError
# define PyExc_IndexError p3imp_PyExc_IndexError
# define Py_Exc_किल्ली_Error p3imp_Py_Exc_किल्ली_Error
# define PyExc_KeyboardInterrupt p3imp_PyExc_KeyboardInterrupt
# define PyExc_TypeError p3imp_PyExc_TypeError
# define Py_Exc_मूल्य_Error p3imp_Py_Exc_मूल्य_Error
# define PyExc_SystemExit p3imp_PyExc_SystemExit
# define PyExc_RuntimeError p3imp_PyExc_RuntimeError
# define PyExc_ImportError p3imp_PyExc_ImportError
# define PyExc_OverflowError p3imp_PyExc_OverflowError

/*
 * Table of name to function pointer of python.
 */
# define PYTHON_PROC FARPROC
स्थिर संरचना
{
    अक्षर *name;
    PYTHON_PROC *ptr;
} py3_funcname_table[] =
{
    {"PySys_SetArgv", (PYTHON_PROC*)&py3_PySys_SetArgv},
    {"Py_SetPythonHome", (PYTHON_PROC*)&py3_Py_SetPythonHome},
    {"Py_Initialize", (PYTHON_PROC*)&py3_Py_Initialize},
    {"_PyArg_ParseTuple_SizeT", (PYTHON_PROC*)&py3_PyArg_ParseTuple},
    {"_Py_BuildValue_SizeT", (PYTHON_PROC*)&py3_Py_Build_मूल्य},
    {"PyMem_Free", (PYTHON_PROC*)&py3_PyMem_Free},
    {"PyMem_Malloc", (PYTHON_PROC*)&py3_PyMem_Malloc},
    {"PyList_New", (PYTHON_PROC*)&py3_PyList_New},
    {"PyGILState_Ensure", (PYTHON_PROC*)&py3_PyGILState_Ensure},
    {"PyGILState_Release", (PYTHON_PROC*)&py3_PyGILState_Release},
    {"PySys_SetObject", (PYTHON_PROC*)&py3_PySys_SetObject},
    {"PySys_GetObject", (PYTHON_PROC*)&py3_PySys_GetObject},
    {"PyList_Append", (PYTHON_PROC*)&py3_PyList_Append},
    {"PyList_Insert", (PYTHON_PROC*)&py3_Py_List_घाला},
    {"PyList_Size", (PYTHON_PROC*)&py3_PyList_Size},
    {"PyTuple_New", (PYTHON_PROC*)&py3_PyTuple_New},
    {"PyTuple_GetItem", (PYTHON_PROC*)&py3_PyTuple_GetItem},
    {"PyTuple_SetItem", (PYTHON_PROC*)&py3_PyTuple_SetItem},
    {"PyTuple_Size", (PYTHON_PROC*)&py3_PyTuple_Size},
    {"PySequence_Check", (PYTHON_PROC*)&py3_PySequence_Check},
    {"PySequence_Size", (PYTHON_PROC*)&py3_PySequence_Size},
    {"PySequence_GetItem", (PYTHON_PROC*)&py3_PySequence_GetItem},
    {"PySequence_Fast", (PYTHON_PROC*)&py3_PySequence_Fast},
# जर PY_VERSION_HEX >= 0x030601f0
    {"PySlice_AdjustIndices", (PYTHON_PROC*)&py3_PySlice_AdjustIndices},
    {"PySlice_Unpack", (PYTHON_PROC*)&py3_PySlice_Unpack},
# endif
    {"PySlice_GetIndicesEx", (PYTHON_PROC*)&py3_PySlice_GetIndicesEx},
    {"PyErr_NoMemory", (PYTHON_PROC*)&py3_PyErr_NoMemory},
    {"Py_Finalize", (PYTHON_PROC*)&py3_Py_Finalize},
    {"PyErr_SetString", (PYTHON_PROC*)&py3_PyErr_SetString},
    {"PyErr_SetObject", (PYTHON_PROC*)&py3_PyErr_SetObject},
    {"PyErr_ExceptionMatches", (PYTHON_PROC*)&py3_PyErr_ExceptionMatches},
# जर defined(USE_LIMITED_API) || PY_VERSION_HEX >= 0x03080000
    {"Py_IncRef", (PYTHON_PROC*)&py3_Py_IncRef},
    {"Py_DecRef", (PYTHON_PROC*)&py3_Py_DecRef},
# endif
# ifdef USE_LIMITED_API
    {"Py_CompileString", (PYTHON_PROC*)&py3_Py_CompileString},
    {"PyEval_EvalCode", (PYTHON_PROC*)&PyEval_EvalCode},
# अन्यथा
    {"PyRun_SimpleString", (PYTHON_PROC*)&py3_PyRun_SimpleString},
    {"PyRun_String", (PYTHON_PROC*)&py3_PyRun_String},
# endif
    {"PyObject_GetAttrString", (PYTHON_PROC*)&py3_PyObject_GetAttrString},
    {"PyObject_HasAttrString", (PYTHON_PROC*)&py3_PyObject_HasAttrString},
    {"PyObject_SetAttrString", (PYTHON_PROC*)&py3_PyObject_SetAttrString},
    {"PyObject_CallFunctionObjArgs", (PYTHON_PROC*)&py3_PyObject_CallFunctionObjArgs},
# जर PY_VERSION_HEX >= 0x030d0000
    {"PyObject_CallFunction", (PYTHON_PROC*)&py3_PyObject_CallFunction},
# अन्यथा
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
    {"PyMapping_Keys", (PYTHON_PROC*)&py3_Py_Mapping_किल्ल्या},
# जर (defined(USE_LIMITED_API) && Py_LIMITED_API >= 0x03080000) || \
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
# जर PY_VERSION_HEX >= 0x03040000
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
# जर !defined(USE_LIMITED_API) && PY_VERSION_HEX < 0x030D0000
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
#  जर Py_LIMITED_API >= 0x030a0000
    {"PyUnicode_AsUTF8AndSize", (PYTHON_PROC*)&py3_PyUnicode_AsUTF8AndSize},
#  endif
# अन्यथा
#  जर PY_VERSION_HEX >= 0x030300f0
    {"PyUnicode_AsUTF8AndSize", (PYTHON_PROC*)&py3_PyUnicode_AsUTF8AndSize},
#  अन्यथा
    {"_PyUnicode_AsString", (PYTHON_PROC*)&py3__PyUnicode_AsString},
#  endif
# endif
    {"PyUnicode_CompareWithASCIIString", (PYTHON_PROC*)&py3_PyUnicode_CompareWithASCIIString},
    {"PyUnicode_AsUTF8String", (PYTHON_PROC*)&py3_PyUnicode_AsUTF8String},
# ifdef Py_UNICODE_USE_UCS_FUNCTIONS
#  ifdef Py_UNICODE_WIDE
    {"PyUnicodeUCS4_FromFormat", (PYTHON_PROC*)&py3_PyUnicodeUCS4_FromFormat},
#  अन्यथा
    {"PyUnicodeUCS2_FromFormat", (PYTHON_PROC*)&py3_PyUnicodeUCS2_FromFormat},
#  endif
# अन्यथा
    {"PyUnicode_FromFormat", (PYTHON_PROC*)&py3_PyUnicode_FromFormat},
# endif
    {"PyBytes_AsString", (PYTHON_PROC*)&py3_PyBytes_AsString},
    {"PyBytes_AsStringAndSize", (PYTHON_PROC*)&py3_PyBytes_AsStringAndSize},
    {"PyBytes_FromString", (PYTHON_PROC*)&py3_PyBytes_FromString},
    {"PyBytes_FromStringAndSize", (PYTHON_PROC*)&py3_PyBytes_FromStringAndSize},
# जर PY_VERSION_HEX >= 0x030900b0
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
    {"PyNumber_Check", (PYTHON_PROC*)&py3_Py_संख्या_Check},
    {"PyNumber_Long", (PYTHON_PROC*)&py3_Py_संख्या_Long},
    {"PyErr_NewException", (PYTHON_PROC*)&py3_PyErr_NewException},
# ifdef Py_DEBUG
    {"_Py_NegativeRefcount", (PYTHON_PROC*)&py3__Py_NegativeRefcount},
    {"_Py_RefTotal", (PYTHON_PROC*)&py3__Py_RefTotal},
    {"PyModule_Create2TraceRefs", (PYTHON_PROC*)&py3_PyModule_Create2TraceRefs},
# अन्यथा
    {"PyModule_Create2", (PYTHON_PROC*)&py3_PyModule_Create2},
# endif
# जर defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
    {"_PyObject_DebugFree", (PYTHON_PROC*)&py3__PyObject_DebugFree},
    {"_PyObject_DebugMalloc", (PYTHON_PROC*)&py3__PyObject_DebugMalloc},
# अन्यथा
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
#  जर PY_VERSION_HEX >= 0x03040000
    {"PyType_GetSlot", (PYTHON_PROC*)&py3_PyType_GetSlot},
#  endif
    {"PyType_FromSpec", (PYTHON_PROC*)&py3_PyType_FromSpec},
# endif
    {"", रिक्त},
};

# जर defined(USE_LIMITED_API) || PY_VERSION_HEX >= 0x03080000
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

# जर PY_VERSION_HEX >= 0x030900b0
    स्थिर ओळीत पूर्णांक
py3_PyType_HasFeature(PyTypeObject *type, अचिन्हित दीर्घ feature)
{
    परत ((PyType_GetFlags(type) & feature) != 0);
}
#  define PyType_HasFeature(t,f) py3_PyType_HasFeature(t,f)
# endif

# जर PY_VERSION_HEX >= 0x030a00b2
    स्थिर ओळीत पूर्णांक
py3__PyObject_TypeCheck(PyObject *ob, PyTypeObject *type)
{
    परत Py_IS_TYPE(ob, type) || PyType_IsSubtype(Py_TYPE(ob), type);
}
#  जर PY_VERSION_HEX >= 0x030b00b3
#   undef PyObject_TypeCheck
#   define PyObject_TypeCheck(o,t) py3__PyObject_TypeCheck(o,t)
#  अन्यथा
#   define _PyObject_TypeCheck(o,t) py3__PyObject_TypeCheck(o,t)
#  endif
# endif

# जर !defined(USE_LIMITED_API) && PY_VERSION_HEX >= 0x030c00b0
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
    स्थिर WCHAR *
py3_get_system_libname(स्थिरांक अक्षर *libname)
{
    स्थिरांक WCHAR	*pythoncore = L"Software\\Python\\PythonCore";
    स्थिरांक अक्षर	*cp = libname;
    WCHAR	subkey[128];
    HKEY	h_किल्ली;
    पूर्णांक		i;
    DWORD	j, len;
    LSTATUS	ret;

    पर्यंत (*cp != '\0')
    {
	जर (*cp == ':' || *cp == '\\' || *cp == '/')
	{
	    // Bail out if "libname" contains path separator, assume it is
	    // an absolute path.
	    परत रिक्त;
	}
	++cp;
    }

    WCHAR   keyfound[32];
    HKEY    h_किल्ली_शीर्ष[] = {HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE};
    HKEY    h_किल्ली_Found = रिक्त;
#  ifdef USE_LIMITED_API
    दीर्घ    maxminor = -1;
#  endif
    साठी (i = 0; i < ARRAY_LENGTH(h_किल्ली_शीर्ष); i++)
    {
	दीर्घ	major, minor;

	ret = Reg_Open_किल्ली_Ex_W(h_किल्ली_शीर्ष[i], pythoncore, 0, किल्ली_READ, &h_किल्ली);
	जर (ret != ERROR_SUCCESS)
	    सुरूठेवा;
	साठी (j = 0;; j++)
	{
	    WCHAR   keyname[32];
	    WCHAR   *wp;

	    len = ARRAY_LENGTH(keyname);
	    ret = Reg_Enum_किल्ली_Ex_W(h_किल्ली, j, keyname, &len,
						    रिक्त, रिक्त, रिक्त, रिक्त);
	    जर (ret == ERROR_NO_MORE_वस्तुसंच)
		थांबा;

	    major = wcstol(keyname, &wp, 10);
	    जर (*wp != L'.')
		सुरूठेवा;
	    minor = wcstol(wp + 1, &wp, 10);
#  ifdef _WIN64
	    जर (*wp != L'\0')
		सुरूठेवा;
#  अन्यथा
	    जर (wcscmp(wp, L"-32") != 0)
		सुरूठेवा;
#  endif

	    जर (major != PY_MAJOR_VERSION)
		सुरूठेवा;
#  ifdef USE_LIMITED_API
	    // Search the latest version.
	    जर ((minor > maxminor)
		    && (minor >= ((Py_LIMITED_API >> 16) & 0xff)))
	    {
		maxminor = minor;
		wcscpy(keyfound, keyname);
		h_किल्ली_Found = h_किल्ली_शीर्ष[i];
	    }
#  अन्यथा
	    // Check if it matches with the compiled version.
	    जर (minor == PY_MINOR_VERSION)
	    {
		wcscpy(keyfound, keyname);
		h_किल्ली_Found = h_किल्ली_शीर्ष[i];
		थांबा;
	    }
#  endif
	}
	Reg_Close_किल्ली(h_किल्ली);
#  ifdef USE_LIMITED_API
	जर (h_किल्ली_Found != रिक्त)
	    थांबा;
#  endif
    }
    जर (h_किल्ली_Found == रिक्त)
	परत रिक्त;

    swprintf(subkey, ARRAY_LENGTH(subkey), L"%ls\\%ls\\InstallPath",
							pythoncore, keyfound);
    ret = Reg_Get_मूल्य_W(h_किल्ली_Found, subkey, रिक्त, RRF_RT_REG_SZ,
							    रिक्त, रिक्त, &len);
    जर (ret != ERROR_MORE_DATA && ret != ERROR_SUCCESS)
	परत रिक्त;
    माप_प्रकार len2 = len / आकार(WCHAR) + 1 + म_माप(libname);
    WCHAR *path = alloc(len2 * आकार(WCHAR));
    जर (path == रिक्त)
	परत रिक्त;
    ret = Reg_Get_मूल्य_W(h_किल्ली_Found, subkey, रिक्त, RRF_RT_REG_SZ,
							    रिक्त, path, &len);
    जर (ret != ERROR_SUCCESS)
    {
	vim_free(path);
	परत रिक्त;
    }
    // Remove trailing path separators.
    माप_प्रकार len3 = wcslen(path);
    जर ((len3 > 0) && (path[len3 - 1] == L'/' || path[len3 - 1] == L'\\'))
	--len3;
    swprintf(path + len3, len2 - len3, L"\\%hs", libname);
    परत path;
}
# endif

/*
 * Load library and get all pointers.
 * Parameter 'libname' provides name of DLL.
 * Return OK or FAIL.
 */
    स्थिर पूर्णांक
py3_runtime_link_init(अक्षर *libname, पूर्णांक verbose)
{
    पूर्णांक i;
    PYTHON_PROC *ucs_from_string = (PYTHON_PROC *)&py3_PyUnicode_FromString;
    PYTHON_PROC *ucs_decode = (PYTHON_PROC *)&py3_PyUnicode_Decode;
    PYTHON_PROC *ucs_as_encoded_string =
				 (PYTHON_PROC *)&py3_PyUnicode_AsEncodedString;

# जर !(defined(PY_NO_RTLD_GLOBAL) && defined(PY3_NO_RTLD_GLOBAL)) && defined(UNIX) && defined(FEAT_PYTHON)
    // Can't have Python and Python3 loaded at the same time.
    // It causes a crash, because RTLD_GLOBAL is needed for
    // standard C extension libraries of one or both python versions.
    जर (python_loaded())
    {
	जर (verbose)
	    emsg(_(e_this_vim_cannot_execute_py3_after_using_python));
	परत FAIL;
    }
# endif

    जर (hinstPy3 != 0)
	परत OK;
    hinstPy3 = load_dll(libname);

# ifdef MSWIN
    जर (!hinstPy3)
    {
	// Attempt to use the path from InstallPath as stored in the registry.
	WCHAR *syslibname = py3_get_system_libname(libname);

	जर (syslibname != रिक्त)
	{
	    hinstPy3 = LoadLibraryExW(syslibname, रिक्त,
		    LOAD_LIBRARY_शोध_DLL_LOAD_DIR |
		    LOAD_LIBRARY_शोध_SYSTEM32);
	    vim_free(syslibname);
	}
    }
# endif

    जर (!hinstPy3)
    {
	जर (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	परत FAIL;
    }

    साठी (i = 0; py3_funcname_table[i].ptr; ++i)
    {
	जर ((*py3_funcname_table[i].ptr = चिन्ह_from_dll(hinstPy3,
			py3_funcname_table[i].name)) == रिक्त)
	{
	    close_dll(hinstPy3);
	    hinstPy3 = 0;
	    जर (verbose)
		semsg(_(e_could_not_load_library_function_str), py3_funcname_table[i].name);
	    परत FAIL;
	}
    }

    // Load unicode functions separately as only the ucs2 or the ucs4 functions
    // will be present in the library.
# जर PY_VERSION_HEX >= 0x030300f0
    *ucs_from_string = चिन्ह_from_dll(hinstPy3, "PyUnicode_FromString");
    *ucs_decode = चिन्ह_from_dll(hinstPy3, "PyUnicode_Decode");
    *ucs_as_encoded_string = चिन्ह_from_dll(hinstPy3,
	    "PyUnicode_AsEncodedString");
# अन्यथा
    *ucs_from_string = चिन्ह_from_dll(hinstPy3, "PyUnicodeUCS2_FromString");
    *ucs_decode = चिन्ह_from_dll(hinstPy3,
	    "PyUnicodeUCS2_Decode");
    *ucs_as_encoded_string = चिन्ह_from_dll(hinstPy3,
	    "PyUnicodeUCS2_AsEncodedString");
    जर (*ucs_from_string == रिक्त || *ucs_decode == रिक्त
					     || *ucs_as_encoded_string == रिक्त)
    {
	*ucs_from_string = चिन्ह_from_dll(hinstPy3,
		"PyUnicodeUCS4_FromString");
	*ucs_decode = चिन्ह_from_dll(hinstPy3,
		"PyUnicodeUCS4_Decode");
	*ucs_as_encoded_string = चिन्ह_from_dll(hinstPy3,
		"PyUnicodeUCS4_AsEncodedString");
    }
# endif
    जर (*ucs_from_string == रिक्त || *ucs_decode == रिक्त
					     || *ucs_as_encoded_string == रिक्त)
    {
	close_dll(hinstPy3);
	hinstPy3 = 0;
	जर (verbose)
	    semsg(_(e_could_not_load_library_function_str), "PyUnicode_UCSX_*");
	परत FAIL;
    }

    परत OK;
}

/*
 * If python is enabled (there is installed python on Windows system) return
 * TRUE, else FALSE.
 */
    पूर्णांक
python3_enabled(पूर्णांक verbose)
{
    परत py3_runtime_link_init((अक्षर *)p_py3dll, verbose) == OK;
}

/*
 * Load the standard Python exceptions - don't import the symbols from the
 * DLL, as this can cause errors (importing data symbols is not reliable).
 */
    स्थिर शून्य
get_py3_exceptions(शून्य)
{
    PyObject *exmod = PyImport_ImportModule("builtins");
    PyObject *exdict = PyModule_GetDict(exmod);
    p3imp_PyExc_AttributeError = PyDict_GetItemString(exdict, "AttributeError");
    p3imp_PyExc_IndexError = PyDict_GetItemString(exdict, "IndexError");
    p3imp_Py_Exc_किल्ली_Error = PyDict_GetItemString(exdict, "KeyError");
    p3imp_PyExc_KeyboardInterrupt = PyDict_GetItemString(exdict, "KeyboardInterrupt");
    p3imp_PyExc_TypeError = PyDict_GetItemString(exdict, "TypeError");
    p3imp_Py_Exc_मूल्य_Error = PyDict_GetItemString(exdict, "ValueError");
    p3imp_PyExc_SystemExit = PyDict_GetItemString(exdict, "SystemExit");
    p3imp_PyExc_RuntimeError = PyDict_GetItemString(exdict, "RuntimeError");
    p3imp_PyExc_ImportError = PyDict_GetItemString(exdict, "ImportError");
    p3imp_PyExc_OverflowError = PyDict_GetItemString(exdict, "OverflowError");
    Py_XINCREF(p3imp_PyExc_AttributeError);
    Py_XINCREF(p3imp_PyExc_IndexError);
    Py_XINCREF(p3imp_Py_Exc_किल्ली_Error);
    Py_XINCREF(p3imp_PyExc_KeyboardInterrupt);
    Py_XINCREF(p3imp_PyExc_TypeError);
    Py_XINCREF(p3imp_Py_Exc_मूल्य_Error);
    Py_XINCREF(p3imp_PyExc_SystemExit);
    Py_XINCREF(p3imp_PyExc_RuntimeError);
    Py_XINCREF(p3imp_PyExc_ImportError);
    Py_XINCREF(p3imp_PyExc_OverflowError);
    Py_XDECREF(exmod);
}
#endif // DYNAMIC_PYTHON3

स्थिर पूर्णांक py3initialised = 0;
#define PYINITIALISED py3initialised
स्थिर पूर्णांक python_end_called = FALSE;

#ifdef USE_LIMITED_API
# define DESTRUCTOR_FINISH(self) \
    ((freefunc)PyType_GetSlot(Py_TYPE((PyObject*)self), Py_tp_free))((PyObject*)self)
#अन्यथा
# define DESTRUCTOR_FINISH(self) Py_TYPE(self)->tp_free((PyObject*)self)
#endif

#define WIN_PYTHON_REF(win) win->w_python3_ref
#define BUF_PYTHON_REF(buf) buf->b_python3_ref
#define TAB_PYTHON_REF(tab) tab->tp_python3_ref

    स्थिर शून्य
call_PyObject_Free(शून्य *p)
{
#जर defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
    _PyObject_DebugFree(p);
#अन्यथा
    PyObject_Free(p);
#endif
}

    स्थिर PyObject *
call_PyType_GenericNew(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    परत PyType_GenericNew(type,args,kwds);
}

    स्थिर PyObject *
call_PyType_GenericAlloc(PyTypeObject *type, Py_ssize_t nitems)
{
    परत PyType_GenericAlloc(type,nitems);
}

स्थिर PyObject *OutputGetattro(PyObject *, PyObject *);
स्थिर पूर्णांक OutputSetattro(PyObject *, PyObject *, PyObject *);
स्थिर PyObject *BufferGetattro(PyObject *, PyObject *);
स्थिर पूर्णांक BufferSetattro(PyObject *, PyObject *, PyObject *);
स्थिर PyObject *TabPageGetattro(PyObject *, PyObject *);
स्थिर PyObject *WindowGetattro(PyObject *, PyObject *);
स्थिर पूर्णांक WindowSetattro(PyObject *, PyObject *, PyObject *);
स्थिर PyObject *RangeGetattro(PyObject *, PyObject *);
स्थिर PyObject *CurrentGetattro(PyObject *, PyObject *);
स्थिर पूर्णांक CurrentSetattro(PyObject *, PyObject *, PyObject *);
स्थिर PyObject *DictionaryGetattro(PyObject *, PyObject *);
स्थिर पूर्णांक DictionarySetattro(PyObject *, PyObject *, PyObject *);
स्थिर PyObject *ListGetattro(PyObject *, PyObject *);
स्थिर पूर्णांक ListSetattro(PyObject *, PyObject *, PyObject *);
स्थिर PyObject *TupleGetattro(PyObject *, PyObject *);
स्थिर पूर्णांक TupleSetattro(PyObject *, PyObject *, PyObject *);
स्थिर PyObject *FunctionGetattro(PyObject *, PyObject *);

स्थिर संरचना PyModuleDef vimmodule;

#define PY_CAN_RECURSE

/*
 * Include the code shared with if_python.c
 */
#अन्तर्भाव "if_py_both.h"

#ifdef USE_LIMITED_API
# जर Py_LIMITED_API >= 0x030A0000
#  define PY_UNICODE_GET_UTF8_CHARS(obj) PyUnicode_AsUTF8AndSize(obj, रिक्त)
# अन्यथा
// Python limited API before 3.10 lack easy ways to query the raw UTF-8 chars.
// We need to first convert the string to bytes, and then extract the chars.
// This function is only used for attribute string comparisons, which have
// known short length. As such, just allocate a short static buffer to hold
// the characters instead of having to allocate/deallcoate it.
//
// An alternative would be to convert all attribute string comparisons to use
// PyUnicode_CompareWithASCIIString to skip having to extract the chars.
स्थिर अक्षर py3_unicode_utf8_chars[20];
स्थिर अक्षर* PY_UNICODE_GET_UTF8_CHARS(PyObject* str)
{
    py3_unicode_utf8_chars[0] = '\0';
    PyObject* bytes = PyUnicode_AsUTF8String(str);
    जर (bytes)
    {
	अक्षर *chars;
	Py_ssize_t len;
	जर (PyBytes_AsStringAndSize(bytes, &chars, &len) != -1)
	{
	    जर (len < (Py_ssize_t)आकार(py3_unicode_utf8_chars))
		// PyBytes_AsStringAndSize guarantees null-termination
		स_नकल(py3_unicode_utf8_chars, chars, len + 1);
	}
	Py_DECREF(bytes);
    }
    परत py3_unicode_utf8_chars;
}
# endif
#अन्यथा	// !USE_LIMITED_API
# जर PY_VERSION_HEX >= 0x030300f0
#  define PY_UNICODE_GET_UTF8_CHARS(obj) PyUnicode_AsUTF8AndSize(obj, रिक्त)
# अन्यथा
#  define PY_UNICODE_GET_UTF8_CHARS _PyUnicode_AsString
# endif
#endif

// NOTE: Must always be used at the start of a block, since it declares "name".
#define GET_ATTR_STRING(name, nameobj) \
    अक्षर	*name = ""; \
    जर (PyUnicode_Check(nameobj)) \
	name = (अक्षर *)PY_UNICODE_GET_UTF8_CHARS(nameobj)

#define PY3OBJ_DELETED(obj) (obj->ob_base.ob_refcnt<=0)

///////////////////////////////////////////////////////
// Internal function prototypes.

स्थिर PyObject *Py3Init_vim(शून्य);

///////////////////////////////////////////////////////
// 1. Python interpreter main program.

    शून्य
python3_end(शून्य)
{
    स्थिर पूर्णांक recurse = 0;

    // If a crash occurs while doing this, don't try again.
    जर (recurse != 0)
	परत;

    python_end_called = TRUE;
    ++recurse;

#ifdef DYNAMIC_PYTHON3
    जर (hinstPy3)
#endif
    जर (Py_IsInitialized())
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

#जर defined(DYNAMIC_PYTHON3) && defined(DYNAMIC_PYTHON) && defined(FEAT_PYTHON) && defined(UNIX)
    पूर्णांक
python3_loaded(शून्य)
{
    परत (hinstPy3 != 0);
}
#endif

स्थिर ब_अक्षर_प्रकार *py_home_buf = रिक्त;

#जर defined(MSWIN) && (PY_VERSION_HEX >= 0x030500f0)
/*
 * Return TRUE if stdin is readable from Python 3.
 */
    स्थिर BOOL
is_stdin_readable(शून्य)
{
    DWORD	    mode, eventnum;
    संरचना _stat    st;
    पूर्णांक		    fd = fileno(मानक_निवेश);
    HANDLE	    hstdin = (HANDLE)_get_osfhandle(fd);

    // Check if stdin is connected to the console.
    जर (GetConsoleMode(hstdin, &mode))
	// Check if it is opened as input.
	परत Get_संख्या_Of_Console_आदान_Events(hstdin, &eventnum);

    परत _fstat(fd, &st) == 0;
}

// Python 3.5 or later will abort inside Py_Initialize() when stdin has
// been closed (i.e. executed by "vim -").  Reconnect stdin to CONIN$.
// Note that the python DLL is linked to its own stdio DLL which can be
// differ from Vim's stdio.
    स्थिर शून्य
reset_stdin(शून्य)
{
    खाता *(*py__acrt_iob_func)(अचिन्हित) = रिक्त;
    खाता *(*pyfreopen)(स्थिरांक अक्षर *, स्थिरांक अक्षर *, खाता *) = रिक्त;
    HINSTANCE hinst = get_forwarded_dll(hinstPy3);

    जर (hinst == रिक्त || is_stdin_readable())
	परत;

    // Get "freopen" and "stdin" which are used in the python DLL.
    // "stdin" is defined as "__acrt_iob_func(0)" in VC++ 2015 or later.
    py__acrt_iob_func = get_dll_import_func(hinst, "__acrt_iob_func");
    जर (py__acrt_iob_func)
    {
	HINSTANCE hpystdiodll = find_imported_module_by_funcname(hinst,
							    "__acrt_iob_func");
	जर (hpystdiodll)
	    pyfreopen = (शून्य *)GetProcAddress(hpystdiodll, "freopen");
    }

    // Reconnect stdin to CONIN$.
    जर (pyfreopen != रिक्त)
	pyfreopen("CONIN$", "r", py__acrt_iob_func(0));
    अन्यथा
	ख_व_खोलो("CONIN$", "r", मानक_निवेश);
}
#अन्यथा
# define reset_stdin()
#endif

// Python 3.2 or later will abort inside Py_Initialize() when mandatory
// modules cannot be loaded (e.g. 'pythonthreehome' is wrongly set.).
// Install a hook to python dll's exit() and recover from it.
#जर defined(MSWIN) && (PY_VERSION_HEX >= 0x030200f0)
# define HOOK_EXIT
# include <setjmp.h>

स्थिर लाँघ_बफ exit_hook_jump_buf;
स्थिर शून्य *orig_exit = रिक्त;

/*
 * Function that replaces exit() while calling Py_Initialize().
 */
    स्थिर शून्य
hooked_exit(पूर्णांक ret)
{
    // Recover from exit.
    दीर्घ_लाँघ(exit_hook_jump_buf, 1);
}

/*
 * Install a hook to python dll's exit().
 */
    स्थिर शून्य
hook_py_exit(शून्य)
{
    HINSTANCE hinst = get_forwarded_dll(hinstPy3);

    जर (hinst == रिक्त || orig_exit != रिक्त)
	परत;

    orig_exit = hook_dll_import_func(hinst, "exit", (शून्य *)hooked_exit);
}

/*
 * Remove the hook installed by hook_py_exit().
 */
    स्थिर शून्य
restore_py_exit(शून्य)
{
    HINSTANCE hinst = hinstPy3;

    जर (hinst == रिक्त)
	परत;

    जर (orig_exit != रिक्त)
	hook_dll_import_func(hinst, "exit", orig_exit);
    orig_exit = रिक्त;
}
#endif

    स्थिर पूर्णांक
Python3_Init(शून्य)
{
    जर (!py3initialised)
    {
#ifdef DYNAMIC_PYTHON3
	जर (!python3_enabled(TRUE))
	{
	    emsg(_(e_sorry_this_command_is_disabled_python_library_could_not_be_found));
	    जा fail;
	}
#endif
	// Python 3.13 introduced a really useful feature: colorized exceptions.
	// This is great if you're reading them from the terminal, but useless
	// and broken everywhere else (such as in log files, or text editors).
	// Opt out, forcefully.
	vim_setenv((char_u*)"PYTHON_COLORS", (char_u*)"0");

	init_structs();

	जर (*p_py3home != NUL)
	{
	    माप_प्रकार len = mbstowcs(रिक्त, (अक्षर *)p_py3home, 0) + 1;

	    // The string must not change later, make a copy in static memory.
	    py_home_buf = ALLOC_MULT(ब_अक्षर_प्रकार, len);
	    जर (py_home_buf != रिक्त && mbstowcs(
			    py_home_buf, (अक्षर *)p_py3home, len) != (माप_प्रकार)-1)
		Py_SetPythonHome(py_home_buf);
	}
#ifdef PYTHON3_HOME
	अन्यथा जर (mch_getenv((char_u *)"PYTHONHOME") == रिक्त)
	    Py_SetPythonHome(PYTHON3_HOME);
#endif

	PyImport_AppendInittab("vim", Py3Init_vim);

#जर !defined(DYNAMIC_PYTHON3) && defined(MSWIN)
	hinstPy3 = GetModuleHandle(PYTHON3_DLL);
#endif
	reset_stdin();

#ifdef HOOK_EXIT
	// Catch exit() called in Py_Initialize().
	hook_py_exit();
	जर (बनाओ_लाँघ(exit_hook_jump_buf) == 0)
	{
	    Py_Initialize();
	    restore_py_exit();
	}
	अन्यथा
	{
	    // exit() was called in Py_Initialize().
	    restore_py_exit();
	    emsg(_(e_critical_error_in_python3_initialization_check_your_installation));
	    जा fail;
	}
#अन्यथा
	Py_Initialize();
#endif

#जर PY_VERSION_HEX < 0x03090000
	// Initialise threads.  This is deprecated since Python 3.9.
	PyEval_InitThreads();
#endif
#ifdef DYNAMIC_PYTHON3
	get_py3_exceptions();
#endif

	जर (PythonIO_Init_io())
	    जा fail;

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

    परत 0;

fail:
    // We call PythonIO_Flush() here to print any Python errors.
    // This is OK, as it is possible to call this function even
    // if PythonIO_Init_io() has not completed successfully (it will
    // not do anything in this case).
    PythonIO_Flush();
    परत -1;
}

/*
 * External interface
 */
    स्थिर शून्य
DoPyCommand(स्थिरांक अक्षर *cmd,
	    dict_T* locals,
	    rangeinitializer init_range,
	    runner run,
	    शून्य *arg)
{
#जर defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    अक्षर		*saved_locale;
#endif
    PyObject		*cmdstr;
    PyObject		*cmdbytes;
    PyGILState_STATE	pygilstate;

    जर (python_end_called)
	जा theend;

    जर (Python3_Init())
	जा theend;

    init_range(arg);

    Python_Release_Vim();	    // leave Vim

#जर defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    // Python only works properly when the LC_NUMERIC locale is "C".
    saved_locale = रखो_क्षेत्र(LC_NUMERIC, रिक्त);
    जर (saved_locale == रिक्त || STRCMP(saved_locale, "C") == 0)
	saved_locale = रिक्त;
    अन्यथा
    {
	// Need to make a copy, value may change when setting new locale.
	saved_locale = (अक्षर *)vim_strsave((char_u *)saved_locale);
	(शून्य)रखो_क्षेत्र(LC_NUMERIC, "C");
    }
#endif

    pygilstate = PyGILState_Ensure();

    // PyRun_SimpleString expects a UTF-8 string. Wrong encoding may cause
    // SyntaxError (unicode error).
    cmdstr = PyUnicode_Decode(cmd, म_माप(cmd),
					(अक्षर *)ENC_OPT, ERRORS_DECODE_ARG);
    cmdbytes = PyUnicode_AsEncodedString(cmdstr, "utf-8", ERRORS_ENCODE_ARG);
    Py_XDECREF(cmdstr);

    run(PyBytes_AsString(cmdbytes), locals, arg, &pygilstate);
    Py_XDECREF(cmdbytes);

    PyGILState_Release(pygilstate);

#जर defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    जर (saved_locale != रिक्त)
    {
	(शून्य)रखो_क्षेत्र(LC_NUMERIC, saved_locale);
	vim_free(saved_locale);
    }
#endif

    Python_Lock_Vim();		    // enter Vim
    PythonIO_Flush();

theend:
    परत;	    // keeps lint happy
}

/*
 * ":py3"
 */
    शून्य
ex_py3(exarg_T *eap)
{
    char_u *script;

    script = script_get(eap, eap->arg);
    जर (!eap->skip)
    {
	जर (p_pyx == 0)
	    p_pyx = 3;

	DoPyCommand(script == रिक्त ? (अक्षर *) eap->arg : (अक्षर *) script,
		रिक्त,
		init_range_cmd,
		(runner) run_cmd,
		(शून्य *) eap);
    }
    vim_free(script);
}

#define BUFFER_SIZE 2048

/*
 * ":py3file"
 */
    शून्य
ex_py3file(exarg_T *eap)
{
    स्थिर अक्षर buffer[BUFFER_SIZE];
    स्थिरांक अक्षर *file;
    अक्षर *p;
    पूर्णांक i;

    जर (p_pyx == 0)
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

    म_नकल(buffer, "exec(compile(open('");
    p = buffer + 19; // size of "exec(compile(open('"

    साठी (i=0; i<2; ++i)
    {
	file = (अक्षर *)eap->arg;
	पर्यंत (*file && p < buffer + (BUFFER_SIZE - 3))
	{
	    जर (*file == '\\' || *file == '\'')
		*p++ = '\\';
	    *p++ = *file++;
	}
	// If we didn't finish the file name, we hit a buffer overflow
	जर (*file != '\0')
	    परत;
	जर (i==0)
	{
	    म_नकल(p,"','rb').read(),'");
	    p += 16;
	}
	अन्यथा
	{
	    म_नकल(p,"','exec'))");
	    p += 10;
	}
    }


    // Execute the file
    DoPyCommand(buffer,
	    रिक्त,
	    init_range_cmd,
	    (runner) run_cmd,
	    (शून्य *) eap);
}

    शून्य
ex_py3do(exarg_T *eap)
{
    जर (p_pyx == 0)
	p_pyx = 3;

    DoPyCommand((अक्षर *)eap->arg,
	    रिक्त,
	    init_range_cmd,
	    (runner)run_do,
	    (शून्य *)eap);
}

///////////////////////////////////////////////////////
// 2. Python output stream: writes output via [e]msg().

// Implementation functions

    स्थिर PyObject *
OutputGetattro(PyObject *self, PyObject *nameobj)
{
    GET_ATTR_STRING(name, nameobj);

    जर (म_भेद(name, "softspace") == 0)
	परत PyLong_FromLong(((OutputObject *)(self))->softspace);
    अन्यथा जर (म_भेद(name, "errors") == 0)
	परत PyString_FromString("strict");
    अन्यथा जर (म_भेद(name, "encoding") == 0)
	परत PyString_FromString(ENC_OPT);

    परत PyObject_GenericGetAttr(self, nameobj);
}

    स्थिर पूर्णांक
OutputSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);

    परत OutputSetattr(self, name, val);
}

///////////////////////////////////////////////////////
// 3. Implementation of the Vim module for Python

// Window type - Implementation functions
// --------------------------------------

#define WindowType_Check(obj) ((obj)->ob_base.ob_type == &WindowType)

// Buffer type - Implementation functions
// --------------------------------------

#define BufferType_Check(obj) ((obj)->ob_base.ob_type == &BufferType)

स्थिर PyObject* BufferSubscript(PyObject *self, PyObject *idx);
स्थिर पूर्णांक BufferAsSubscript(PyObject *self, PyObject *idx, PyObject *val);

// Line range type - Implementation functions
// --------------------------------------

#define RangeType_Check(obj) ((obj)->ob_base.ob_type == &RangeType)

स्थिर PyObject* RangeSubscript(PyObject *self, PyObject *idx);
स्थिर पूर्णांक RangeAsItem(PyObject *, Py_ssize_t, PyObject *);
स्थिर पूर्णांक RangeAsSubscript(PyObject *self, PyObject *idx, PyObject *val);

// Current objects type - Implementation functions
// -----------------------------------------------

स्थिर PySequenceMethods BufferAsSeq = {
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

स्थिर PyMappingMethods BufferAsMapping = {
    /* mp_length	*/ (lenfunc)BufferLength,
    /* mp_subscript     */ (binaryfunc)BufferSubscript,
    /* mp_ass_subscript */ (objobjargproc)BufferAsSubscript,
};


// Buffer object

    स्थिर PyObject *
BufferGetattro(PyObject *self, PyObject *nameobj)
{
    PyObject *r;

    GET_ATTR_STRING(name, nameobj);

    जर ((r = BufferAttrValid((BufferObject *)(self), name)))
	परत r;

    जर (CheckBuffer((BufferObject *)(self)))
	परत रिक्त;

    r = BufferAttr((BufferObject *)(self), name);
    जर (r || PyErr_Occurred())
	परत r;
    अन्यथा
	परत PyObject_GenericGetAttr(self, nameobj);
}

    स्थिर पूर्णांक
BufferSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);

    परत BufferSetattr(self, name, val);
}

//////////////////

    स्थिर PyObject *
BufferSubscript(PyObject *self, PyObject* idx)
{
    जर (PyLong_Check(idx))
    {
	दीर्घ _idx = PyLong_AsLong(idx);
	परत BufferItem((BufferObject *)(self), _idx);
    }
    अन्यथा जर (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	जर (CheckBuffer((BufferObject *) self))
	    परत रिक्त;

	जर (PySlice_GetIndicesEx((PySliceObject_T *)idx,
	      (Py_ssize_t)((BufferObject *)(self))->buf->b_ml.ml_ओळ_मोजणी,
	      &start, &stop,
	      &step, &slicelen) < 0)
	    परत रिक्त;
	परत BufferSlice((BufferObject *)(self), start, stop);
    }
    अन्यथा
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	परत रिक्त;
    }
}

    स्थिर पूर्णांक
BufferAsSubscript(PyObject *self, PyObject* idx, PyObject* val)
{
    जर (PyLong_Check(idx))
    {
	दीर्घ n = PyLong_AsLong(idx);

	जर (CheckBuffer((BufferObject *) self))
	    परत -1;

	परत RBAsItem((BufferObject *)(self), n, val, 1,
		    (Py_ssize_t)((BufferObject *)(self))->buf->b_ml.ml_ओळ_मोजणी,
		    रिक्त);
    }
    अन्यथा जर (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	जर (CheckBuffer((BufferObject *) self))
	    परत -1;

	जर (PySlice_GetIndicesEx((PySliceObject_T *)idx,
	      (Py_ssize_t)((BufferObject *)(self))->buf->b_ml.ml_ओळ_मोजणी,
	      &start, &stop,
	      &step, &slicelen) < 0)
	    परत -1;
	परत RBAsSlice((BufferObject *)(self), start, stop, val, 1,
			  (PyInt)((BufferObject *)(self))->buf->b_ml.ml_ओळ_मोजणी,
			  रिक्त);
    }
    अन्यथा
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	परत -1;
    }
}

स्थिर PySequenceMethods RangeAsSeq = {
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

स्थिर PyMappingMethods RangeAsMapping = {
    /* mp_length	*/ (lenfunc)RangeLength,
    /* mp_subscript     */ (binaryfunc)RangeSubscript,
    /* mp_ass_subscript */ (objobjargproc)RangeAsSubscript,
};

// Line range object - Implementation

    स्थिर PyObject *
RangeGetattro(PyObject *self, PyObject *nameobj)
{
    GET_ATTR_STRING(name, nameobj);

    जर (म_भेद(name, "start") == 0)
	परत Py_Build_मूल्य("n", ((RangeObject *)(self))->start - 1);
    अन्यथा जर (म_भेद(name, "end") == 0)
	परत Py_Build_मूल्य("n", ((RangeObject *)(self))->end - 1);
    अन्यथा
	परत PyObject_GenericGetAttr(self, nameobj);
}

////////////////

    स्थिर पूर्णांक
RangeAsItem(PyObject *self, Py_ssize_t n, PyObject *val)
{
    परत RBAsItem(((RangeObject *)(self))->buf, n, val,
		    ((RangeObject *)(self))->start,
		    ((RangeObject *)(self))->end,
		    &((RangeObject *)(self))->end);
}

    स्थिर Py_ssize_t
RangeAsSlice(PyObject *self, Py_ssize_t lo, Py_ssize_t hi, PyObject *val)
{
    परत RBAsSlice(((RangeObject *)(self))->buf, lo, hi, val,
		    ((RangeObject *)(self))->start,
		    ((RangeObject *)(self))->end,
		    &((RangeObject *)(self))->end);
}

    स्थिर PyObject *
RangeSubscript(PyObject *self, PyObject* idx)
{
    जर (PyLong_Check(idx))
    {
	दीर्घ _idx = PyLong_AsLong(idx);
	परत RangeItem((RangeObject *)(self), _idx);
    }
    अन्यथा जर (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	जर (PySlice_GetIndicesEx((PySliceObject_T *)idx,
		((RangeObject *)(self))->end-((RangeObject *)(self))->start+1,
		&start, &stop,
		&step, &slicelen) < 0)
	    परत रिक्त;
	परत RangeSlice((RangeObject *)(self), start, stop);
    }
    अन्यथा
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	परत रिक्त;
    }
}

    स्थिर पूर्णांक
RangeAsSubscript(PyObject *self, PyObject *idx, PyObject *val)
{
    जर (PyLong_Check(idx))
    {
	दीर्घ n = PyLong_AsLong(idx);
	परत RangeAsItem(self, n, val);
    }
    अन्यथा जर (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	जर (PySlice_GetIndicesEx((PySliceObject_T *)idx,
		((RangeObject *)(self))->end-((RangeObject *)(self))->start+1,
		&start, &stop,
		&step, &slicelen) < 0)
	    परत -1;
	परत RangeAsSlice(self, start, stop, val);
    }
    अन्यथा
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	परत -1;
    }
}

// TabPage object - Implementation

    स्थिर PyObject *
TabPageGetattro(PyObject *self, PyObject *nameobj)
{
    PyObject *r;

    GET_ATTR_STRING(name, nameobj);

    जर ((r = TabPageAttrValid((TabPageObject *)(self), name)))
	परत r;

    जर (CheckTabPage((TabPageObject *)(self)))
	परत रिक्त;

    r = TabPageAttr((TabPageObject *)(self), name);
    जर (r || PyErr_Occurred())
	परत r;
    अन्यथा
	परत PyObject_GenericGetAttr(self, nameobj);
}

// Window object - Implementation

    स्थिर PyObject *
WindowGetattro(PyObject *self, PyObject *nameobj)
{
    PyObject *r;

    GET_ATTR_STRING(name, nameobj);

    जर ((r = WindowAttrValid((WindowObject *)(self), name)))
	परत r;

    जर (CheckWindow((WindowObject *)(self)))
	परत रिक्त;

    r = WindowAttr((WindowObject *)(self), name);
    जर (r || PyErr_Occurred())
	परत r;
    अन्यथा
	परत PyObject_GenericGetAttr(self, nameobj);
}

    स्थिर पूर्णांक
WindowSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);

    परत WindowSetattr(self, name, val);
}

// Tab page list object - Definitions

स्थिर PySequenceMethods TabListAsSeq = {
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

स्थिर PySequenceMethods WinListAsSeq = {
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
    स्थिर PyObject *
CurrentGetattro(PyObject *self, PyObject *nameobj)
{
    PyObject	*r;
    GET_ATTR_STRING(name, nameobj);
    जर (!(r = CurrentGetattr(self, name)))
	परत PyObject_GenericGetAttr(self, nameobj);
    परत r;
}

    स्थिर पूर्णांक
CurrentSetattro(PyObject *self, PyObject *nameobj, PyObject *मूल्य)
{
    GET_ATTR_STRING(name, nameobj);
    परत CurrentSetattr(self, name, मूल्य);
}

// Dictionary object - Definitions

    स्थिर PyObject *
DictionaryGetattro(PyObject *self, PyObject *nameobj)
{
    DictionaryObject	*this = ((DictionaryObject *) (self));

    GET_ATTR_STRING(name, nameobj);

    जर (म_भेद(name, "locked") == 0)
	परत PyLong_FromLong(this->dict->dv_lock);
    अन्यथा जर (म_भेद(name, "scope") == 0)
	परत PyLong_FromLong(this->dict->dv_scope);

    परत PyObject_GenericGetAttr(self, nameobj);
}

    स्थिर पूर्णांक
DictionarySetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);
    परत DictionarySetattr(self, name, val);
}

// List object - Definitions

    स्थिर PyObject *
ListGetattro(PyObject *self, PyObject *nameobj)
{
    GET_ATTR_STRING(name, nameobj);

    जर (म_भेद(name, "locked") == 0)
	परत PyLong_FromLong(((ListObject *) (self))->list->lv_lock);

    परत PyObject_GenericGetAttr(self, nameobj);
}

    स्थिर पूर्णांक
ListSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);
    परत ListSetattr(self, name, val);
}

// Tuple object - Definitions

    स्थिर PyObject *
TupleGetattro(PyObject *self, PyObject *nameobj)
{
    GET_ATTR_STRING(name, nameobj);

    जर (म_भेद(name, "locked") == 0)
	परत PyLong_FromLong(((TupleObject *) (self))->tuple->tv_lock);

    परत PyObject_GenericGetAttr(self, nameobj);
}

    स्थिर पूर्णांक
TupleSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);
    परत TupleSetattr(self, name, val);
}

// Function object - Definitions

    स्थिर PyObject *
FunctionGetattro(PyObject *self, PyObject *nameobj)
{
    PyObject		*r;
    FunctionObject	*this = (FunctionObject *)(self);

    GET_ATTR_STRING(name, nameobj);

    r = FunctionAttr(this, name);
    जर (r || PyErr_Occurred())
	परत r;
    अन्यथा
	परत PyObject_GenericGetAttr(self, nameobj);
}

// External interface

    शून्य
python3_buffer_free(buf_T *buf)
{
    BufferObject *bp = BUF_PYTHON_REF(buf);
    जर (bp == रिक्त)
	परत;
    bp->buf = INVALID_BUFFER_मूल्य;
    BUF_PYTHON_REF(buf) = रिक्त;
}

    शून्य
python3_window_free(win_T *win)
{
    WindowObject *wp = WIN_PYTHON_REF(win);
    जर (wp == रिक्त)
	परत;
    wp->win = INVALID_WINDOW_मूल्य;
    WIN_PYTHON_REF(win) = रिक्त;
}

    शून्य
python3_tabpage_free(tabpage_T *tab)
{
    TabPageObject *tp = TAB_PYTHON_REF(tab);
    जर (tp == रिक्त)
	परत;
    tp->tab = INVALID_TABPAGE_मूल्य;
    TAB_PYTHON_REF(tab) = रिक्त;
}

    स्थिर PyObject *
Py3Init_vim(शून्य)
{
    // The special value is removed from sys.path in Python3_Init().
    स्थिर ब_अक्षर_प्रकार *(argv[2]) = {L"/must>not&exist/foo", रिक्त};

    जर (init_types())
	परत रिक्त;

    // Set sys.argv[] to avoid a crash in warn().
    PySys_SetArgv(1, argv);

    जर ((vim_module = PyModule_Create(&vimmodule)) == रिक्त)
	परत रिक्त;

    जर (populate_module(vim_module))
	परत रिक्त;

    जर (init_sys_path())
	परत रिक्त;

    परत vim_module;
}

//////////////////////////////////////////////////////////////////////////
// 4. Utility functions for handling the interface between Vim and Python.

/*
 * Convert a Vim line into a Python string.
 * All internal newlines are replaced by null characters.
 *
 * On errors, the Python exception data is set, and NULL is returned.
 */
    स्थिर PyObject *
ओळ_To_String(स्थिरांक अक्षर *str)
{
    PyObject *result;
    Py_ssize_t len = म_माप(str);
    अक्षर *tmp, *p;

    tmp = alloc(len + 1);
    p = tmp;
    जर (p == रिक्त)
    {
	PyErr_NoMemory();
	परत रिक्त;
    }

    पर्यंत (*str)
    {
	जर (*str == '\n')
	    *p = '\0';
	अन्यथा
	    *p = *str;

	++p;
	++str;
    }
    *p = '\0';

    result = PyUnicode_Decode(tmp, len, (अक्षर *)ENC_OPT, ERRORS_DECODE_ARG);

    vim_free(tmp);
    परत result;
}

    शून्य
do_py3eval(char_u *str, dict_T *locals, typval_T *rettv)
{
    DoPyCommand((अक्षर *) str,
	    locals,
	    init_range_eval,
	    (runner) run_eval,
	    (शून्य *) rettv);
    जर (rettv->v_type == VAR_UNKNOWN)
    {
	rettv->v_type = VAR_संख्या;
	rettv->vval.v_संख्या = 0;
    }
}

    पूर्णांक
set_ref_in_python3(पूर्णांक copyID)
{
    परत set_ref_in_py(copyID);
}

    पूर्णांक
python3_version(शून्य)
{
#ifdef USE_LIMITED_API
    परत Py_LIMITED_API;
#अन्यथा
    परत PY_VERSION_HEX;
#endif
}
