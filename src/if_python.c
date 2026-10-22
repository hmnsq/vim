/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଅନ୍ତର୍ଭାଵ <limits.h>

// uncomment this if used with the debug version of python.
// Checked on 2.7.4.
// #define Py_DEBUG
// Note: most of time you can add -DPy_DEBUG to CFLAGS in place of uncommenting
// uncomment this if used with the debug version of python, but without its
// allocator
// #define Py_DEBUG_NO_PYMALLOC

// Python.h defines _POSIX_THREADS itself (if needed)
#ifdef _POSIX_THREADS
# undef _POSIX_THREADS
#endif

#ଯଦି defined(MSWIN) && defined(HAVE_FCNTL_H)
# undef HAVE_FCNTL_H
#endif

#ifdef _DEBUG
# undef _DEBUG
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
#ifdef _POSIX_C_SOURCE
# undef _POSIX_C_SOURCE	// pyconfig.h defines it as well.
#endif
#ifdef _XOPEN_SOURCE
# undef _XOPEN_SOURCE	// pyconfig.h defines it as well.
#endif

#define PY_SSIZE_T_CLEAN

#ଅନ୍ତର୍ଭାଵ <Python.h>

#ଯଦି !defined(PY_VERSION_HEX) || PY_VERSION_HEX < 0x02050000
# undef PY_SSIZE_T_CLEAN
#endif

// these are NULL for Python 2
#define ERRORS_DECODE_ARG ଖାଲି
#define ERRORS_ENCODE_ARG ERRORS_DECODE_ARG

#undef ମୁଖ୍ୟ // Defined in python.h - aargh
#undef HAVE_FCNTL_H // Clash with os_win32.h

// Perhaps leave this out for Python 2.6, which supports bytes?
#define PyBytes_FromString      PyString_FromString
#define PyBytes_Check		PyString_Check
#define PyBytes_AsStringAndSize PyString_AsStringAndSize
#define PyBytes_FromStringAndSize   PyString_FromStringAndSize

#ଯଦି defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02070000
# define PY_USE_CAPSULE
#endif

#ଯଦି defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02050000
# define PyInt Py_ssize_t
# define PyInquiry lenfunc
# define PyIntArgFunc ssizeargfunc
# define PyIntIntArgFunc ssizessizeargfunc
# define PyIntObjArgProc ssizeobjargproc
# define PyIntIntObjArgProc ssizessizeobjargproc
# define Py_ssize_t_fmt "n"
#ଅନ୍ୟଥା
# define PyInt ପୂର୍ଣ୍ଣସଂଖ୍ୟା
# define lenfunc inquiry
# define PyInquiry inquiry
# define PyIntArgFunc intargfunc
# define PyIntIntArgFunc intintargfunc
# define PyIntObjArgProc intobjargproc
# define PyIntIntObjArgProc intintobjargproc
# define Py_ssize_t_fmt "i"
#endif
#define Py_bytes_fmt "s"

// Parser flags
#define single_ନିବେଶ	256
#define file_ନିବେଶ	257
#define eval_ନିବେଶ	258

#ଯଦି defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x020300F0
  // Python 2.3: can invoke ":python" recursively.
# define PY_CAN_RECURSE
#endif

#ଯଦି defined(DYNAMIC_PYTHON)
# ifndef DYNAMIC_PYTHON
#  define HINSTANCE long_u		// for generating prototypes
# endif

# ifdef MSWIN
#  define load_dll vimLoadLib
#  define close_dll FreeLibrary
#  define ପ୍ରତୀକ_from_dll GetProcAddress
#  define load_dll_error GetWin32Error
# ଅନ୍ୟଥା
#  include <dlfcn.h>
#  define FARPROC ଶୂନ୍ୟ*
#  define HINSTANCE ଶୂନ୍ୟ*
#  ଯଦି defined(PY_NO_RTLD_GLOBAL) && defined(PY3_NO_RTLD_GLOBAL)
#   define load_dll(n) dlopen((n), RTLD_LAZY)
#  ଅନ୍ୟଥା
#   define load_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#  endif
#  define close_dll dlclose
#  define ପ୍ରତୀକ_from_dll dlsym
#  define load_dll_error dlerror
# endif

// This makes if_python.c compile without warnings against Python 2.5
// on Win32 and Win64.
# undef PyRun_SimpleString
# undef PyRun_String
# undef PyArg_Parse
# undef PyArg_ParseTuple
# undef Py_Build_ମୂଲ୍ୟ
# undef Py_InitModule4
# undef Py_InitModule4_64
# undef PyObject_CallMethod
# undef PyObject_CallFunction

/*
 * Wrapper defines
 */
# define PyArg_Parse dll_PyArg_Parse
# define PyArg_ParseTuple dll_PyArg_ParseTuple
# define PyMem_Free dll_PyMem_Free
# define PyMem_Malloc dll_PyMem_Malloc
# define PyDict_SetItemString dll_PyDict_SetItemString
# define PyErr_BadArgument dll_PyErr_BadArgument
# define PyErr_NewException dll_PyErr_NewException
# define PyErr_Clear dll_PyErr_Clear
# define PyErr_Format dll_PyErr_Format
# define PyErr_PrintEx dll_PyErr_PrintEx
# define PyErr_NoMemory dll_PyErr_NoMemory
# define PyErr_Occurred dll_PyErr_Occurred
# define PyErr_SetNone dll_PyErr_SetNone
# define PyErr_SetString dll_PyErr_SetString
# define PyErr_SetObject dll_PyErr_SetObject
# define PyErr_ExceptionMatches dll_PyErr_ExceptionMatches
# define PyEval_InitThreads dll_PyEval_InitThreads
# define PyEval_RestoreThread dll_PyEval_RestoreThread
# define PyEval_SaveThread dll_PyEval_SaveThread
# ifdef PY_CAN_RECURSE
#  define PyGILState_Ensure dll_PyGILState_Ensure
#  define PyGILState_Release dll_PyGILState_Release
# endif
# define PyInt_AsLong dll_PyInt_AsLong
# define PyInt_FromLong dll_PyInt_FromLong
# define PyLong_AsLong dll_PyLong_AsLong
# define PyLong_FromLong dll_PyLong_FromLong
# define PyBool_Type (*dll_PyBool_Type)
# define PyInt_Type (*dll_PyInt_Type)
# define PyLong_Type (*dll_PyLong_Type)
# define PyList_GetItem dll_PyList_GetItem
# define PyList_Append dll_PyList_Append
# define Py_List_ଭର୍ତ୍ତି dll_Py_List_ଭର୍ତ୍ତି
# define PyList_New dll_PyList_New
# define PyList_SetItem dll_PyList_SetItem
# define PyList_Size dll_PyList_Size
# define PyList_Type (*dll_PyList_Type)
# define PyTuple_GetItem dll_PyTuple_GetItem
# define PyTuple_SetItem dll_PyTuple_SetItem
# define PyTuple_New dll_PyTuple_New
# define PyTuple_Size dll_PyTuple_Size
# define PyTuple_Type (*dll_PyTuple_Type)
# define PySequence_Check dll_PySequence_Check
# define PySequence_Size dll_PySequence_Size
# define PySequence_GetItem dll_PySequence_GetItem
# define PySequence_Fast dll_PySequence_Fast
# define PySlice_GetIndicesEx dll_PySlice_GetIndicesEx
# define PyImport_ImportModule dll_PyImport_ImportModule
# define PyDict_New dll_PyDict_New
# define PyDict_GetItemString dll_PyDict_GetItemString
# define PyDict_Next dll_PyDict_Next
# define PyDict_Type (*dll_PyDict_Type)
# ifdef Py_Mapping_ଚାବିଗୁଡ଼ିକ
#  define PY_NO_MAPPING_ଚାବିଗୁଡ଼ିକ
# ଅନ୍ୟଥା
#  define Py_Mapping_ଚାବିଗୁଡ଼ିକ dll_Py_Mapping_ଚାବିଗୁଡ଼ିକ
# endif
# define PyObject_GetItem dll_PyObject_GetItem
# define PyObject_CallMethod dll_PyObject_CallMethod
# define PyMapping_Check dll_PyMapping_Check
# define PyIter_Next dll_PyIter_Next
# define PyModule_GetDict dll_PyModule_GetDict
# define PyModule_AddObject dll_PyModule_AddObject
# define PyRun_SimpleString dll_PyRun_SimpleString
# define PyRun_String dll_PyRun_String
# define PyObject_GetAttrString dll_PyObject_GetAttrString
# define PyObject_HasAttrString dll_PyObject_HasAttrString
# define PyObject_SetAttrString dll_PyObject_SetAttrString
# define PyObject_CallFunctionObjArgs dll_PyObject_CallFunctionObjArgs
# define PyObject_CallFunction dll_PyObject_CallFunction
# define PyObject_Call dll_PyObject_Call
# define PyObject_Repr dll_PyObject_Repr
# define PyString_AsString dll_PyString_AsString
# define PyString_AsStringAndSize dll_PyString_AsStringAndSize
# define PyString_FromString dll_PyString_FromString
# define PyString_FromFormat dll_PyString_FromFormat
# define PyString_FromStringAndSize dll_PyString_FromStringAndSize
# define PyString_Size dll_PyString_Size
# define PyString_Type (*dll_PyString_Type)
# define PyUnicode_Type (*dll_PyUnicode_Type)
# undef PyUnicode_AsEncodedString
# define PyUnicode_AsEncodedString py_PyUnicode_AsEncodedString
# define PyFloat_AsDouble dll_PyFloat_AsDouble
# define PyFloat_FromDouble dll_PyFloat_FromDouble
# define PyFloat_Type (*dll_PyFloat_Type)
# define Py_ସଂଖ୍ୟା_Check dll_Py_ସଂଖ୍ୟା_Check
# define Py_ସଂଖ୍ୟା_Long dll_Py_ସଂଖ୍ୟା_Long
# define PyImport_AddModule (*dll_PyImport_AddModule)
# define PySys_SetObject dll_PySys_SetObject
# define PySys_GetObject dll_PySys_GetObject
# define PySys_SetArgv dll_PySys_SetArgv
# define PyType_Type (*dll_PyType_Type)
# define PyFile_Type (*dll_PyFile_Type)
# define PySlice_Type (*dll_PySlice_Type)
# define PyType_Ready (*dll_PyType_Ready)
# define PyType_GenericAlloc dll_PyType_GenericAlloc
# define Py_Build_ମୂଲ୍ୟ dll_Py_Build_ମୂଲ୍ୟ
# define Py_FindMethod dll_Py_FindMethod
# define Py_InitModule4 dll_Py_InitModule4
# define Py_SetPythonHome dll_Py_SetPythonHome
# define Py_Initialize dll_Py_Initialize
# define Py_Finalize dll_Py_Finalize
# define Py_IsInitialized dll_Py_IsInitialized
# define _PyObject_New dll__PyObject_New
# define _PyObject_GC_New dll__PyObject_GC_New
# ifdef PyObject_GC_Del
#  define Py_underscore_GC
#  define _PyObject_GC_Del dll__PyObject_GC_Del
#  define _PyObject_GC_UnTrack dll__PyObject_GC_UnTrack
# ଅନ୍ୟଥା
#  define PyObject_GC_Del dll_PyObject_GC_Del
#  define PyObject_GC_UnTrack dll_PyObject_GC_UnTrack
# endif
# ଯଦି defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02070000
#  define _PyObject_NextNotImplemented (*dll__PyObject_NextNotImplemented)
# endif
# define _Py_NoneStruct (*dll__Py_NoneStruct)
# define _Py_ZeroStruct (*dll__Py_ZeroStruct)
# define _Py_TrueStruct (*dll__Py_TrueStruct)
# define PyObject_Init dll__PyObject_Init
# define PyObject_GetIter dll_PyObject_GetIter
# define PyObject_IsTrue dll_PyObject_IsTrue
# ଯଦି defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02020000
#  define PyType_IsSubtype dll_PyType_IsSubtype
#  ifdef Py_DEBUG
#   define _Py_NegativeRefcount dll__Py_NegativeRefcount
#   define _Py_RefTotal (*dll__Py_RefTotal)
#   define _Py_Dealloc dll__Py_Dealloc
#  endif
# endif
# ଯଦି defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02030000
#  ଯଦି defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
#   define _PyObject_DebugMalloc dll__PyObject_DebugMalloc
#   define _PyObject_DebugFree dll__PyObject_DebugFree
#  ଅନ୍ୟଥା
#   define PyObject_Malloc dll_PyObject_Malloc
#   define PyObject_Free dll_PyObject_Free
#  endif
# endif
# ifdef PY_USE_CAPSULE
#  define PyCapsule_New dll_PyCapsule_New
#  define PyCapsule_GetPointer dll_PyCapsule_GetPointer
# ଅନ୍ୟଥା
#  define PyCObject_FromVoidPtr dll_PyCObject_FromVoidPtr
#  define PyCObject_AsVoidPtr dll_PyCObject_AsVoidPtr
# endif
# ଯଦି defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02070000
#  define Py_NoSiteFlag (*dll_Py_NoSiteFlag)
# endif

/*
 * Pointers for dynamic link
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା(*dll_PyArg_Parse)(PyObject *, ଅକ୍ଷର *, ...);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା(*dll_PyArg_ParseTuple)(PyObject *, ଅକ୍ଷର *, ...);
ସ୍ଥିର ଶୂନ୍ୟ(*dll_PyMem_Free)(ଶୂନ୍ୟ *);
ସ୍ଥିର ଶୂନ୍ୟ* (*dll_PyMem_Malloc)(size_t);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା(*dll_PyDict_SetItemString)(PyObject *dp, ଅକ୍ଷର *ଚାବି, PyObject *item);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା(*dll_PyErr_BadArgument)(ଶୂନ୍ୟ);
ସ୍ଥିର PyObject *(*dll_PyErr_NewException)(ଅକ୍ଷର *, PyObject *, PyObject *);
ସ୍ଥିର ଶୂନ୍ୟ(*dll_PyErr_Clear)(ଶୂନ୍ୟ);
ସ୍ଥିର PyObject*(*dll_PyErr_Format)(PyObject *, ଅଚଳ ଅକ୍ଷର *, ...);
ସ୍ଥିର ଶୂନ୍ୟ(*dll_PyErr_PrintEx)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ସ୍ଥିର PyObject*(*dll_PyErr_NoMemory)(ଶୂନ୍ୟ);
ସ୍ଥିର PyObject*(*dll_PyErr_Occurred)(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ(*dll_PyErr_SetNone)(PyObject *);
ସ୍ଥିର ଶୂନ୍ୟ(*dll_PyErr_SetString)(PyObject *, ଅଚଳ ଅକ୍ଷର *);
ସ୍ଥିର ଶୂନ୍ୟ(*dll_PyErr_SetObject)(PyObject *, PyObject *);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା(*dll_PyErr_ExceptionMatches)(PyObject *);
ସ୍ଥିର ଶୂନ୍ୟ(*dll_PyEval_InitThreads)(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ(*dll_PyEval_RestoreThread)(PyThreadState *);
ସ୍ଥିର PyThreadState*(*dll_PyEval_SaveThread)(ଶୂନ୍ୟ);
# ifdef PY_CAN_RECURSE
ସ୍ଥିର PyGILState_STATE	(*dll_PyGILState_Ensure)(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ (*dll_PyGILState_Release)(PyGILState_STATE);
# endif
ସ୍ଥିର long(*dll_PyInt_AsLong)(PyObject *);
ସ୍ଥିର PyObject*(*dll_PyInt_FromLong)(long);
ସ୍ଥିର long(*dll_PyLong_AsLong)(PyObject *);
ସ୍ଥିର PyObject*(*dll_PyLong_FromLong)(long);
ସ୍ଥିର PyTypeObject* dll_PyBool_Type;
ସ୍ଥିର PyTypeObject* dll_PyInt_Type;
ସ୍ଥିର PyTypeObject* dll_PyLong_Type;
ସ୍ଥିର PyObject*(*dll_PyList_GetItem)(PyObject *, PyInt);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା(*dll_PyList_Append)(PyObject *, PyObject *);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା(*dll_Py_List_ଭର୍ତ୍ତି)(PyObject *, PyInt, PyObject *);
ସ୍ଥିର PyObject*(*dll_PyList_New)(PyInt size);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା(*dll_PyList_SetItem)(PyObject *, PyInt, PyObject *);
ସ୍ଥିର PyInt(*dll_PyList_Size)(PyObject *);
ସ୍ଥିର PyTypeObject* dll_PyList_Type;
ସ୍ଥିର PyObject*(*dll_PyTuple_GetItem)(PyObject *, PyInt);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା(*dll_PyTuple_SetItem)(PyObject *, PyInt, PyObject *);
ସ୍ଥିର PyObject*(*dll_PyTuple_New)(PyInt size);
ସ୍ଥିର PyInt(*dll_PyTuple_Size)(PyObject *);
ସ୍ଥିର PyTypeObject* dll_PyTuple_Type;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dll_PySequence_Check)(PyObject *);
ସ୍ଥିର PyInt(*dll_PySequence_Size)(PyObject *);
ସ୍ଥିର PyObject*(*dll_PySequence_GetItem)(PyObject *, PyInt);
ସ୍ଥିର PyObject*(*dll_PySequence_Fast)(PyObject *, ଅଚଳ ଅକ୍ଷର *);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dll_PySlice_GetIndicesEx)(PySliceObject *r, PyInt length,
		     PyInt *start, PyInt *stop, PyInt *step,
		     PyInt *slicelen);
ସ୍ଥିର PyObject*(*dll_PyImport_ImportModule)(ଅଚଳ ଅକ୍ଷର *);
ସ୍ଥିର PyObject*(*dll_PyDict_New)(ଶୂନ୍ୟ);
ସ୍ଥିର PyObject*(*dll_PyDict_GetItemString)(PyObject *, ଅଚଳ ଅକ୍ଷର *);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dll_PyDict_Next)(PyObject *, PyInt *, PyObject **, PyObject **);
ସ୍ଥିର PyTypeObject* dll_PyDict_Type;
# ifndef PY_NO_MAPPING_ଚାବିଗୁଡ଼ିକ
ସ୍ଥିର PyObject* (*dll_Py_Mapping_ଚାବିଗୁଡ଼ିକ)(PyObject *);
# endif
ସ୍ଥିର PyObject* (*dll_PyObject_GetItem)(PyObject *, PyObject *);
ସ୍ଥିର PyObject* (*dll_PyObject_CallMethod)(PyObject *, ଅକ୍ଷର *, PyObject *);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dll_PyMapping_Check)(PyObject *);
ସ୍ଥିର PyObject* (*dll_PyIter_Next)(PyObject *);
ସ୍ଥିର PyObject*(*dll_PyModule_GetDict)(PyObject *);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା(*dll_PyModule_AddObject)(PyObject *, ଅଚଳ ଅକ୍ଷର *, PyObject *);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା(*dll_PyRun_SimpleString)(ଅକ୍ଷର *);
ସ୍ଥିର PyObject *(*dll_PyRun_String)(ଅକ୍ଷର *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, PyObject *, PyObject *);
ସ୍ଥିର PyObject* (*dll_PyObject_GetAttrString)(PyObject *, ଅଚଳ ଅକ୍ଷର *);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dll_PyObject_HasAttrString)(PyObject *, ଅଚଳ ଅକ୍ଷର *);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dll_PyObject_SetAttrString)(PyObject *, ଅଚଳ ଅକ୍ଷର *, PyObject *);
ସ୍ଥିର PyObject* (*dll_PyObject_CallFunctionObjArgs)(PyObject *, ...);
ସ୍ଥିର PyObject* (*dll_PyObject_CallFunction)(PyObject *, ଅକ୍ଷର *, ...);
ସ୍ଥିର PyObject* (*dll_PyObject_Call)(PyObject *, PyObject *, PyObject *);
ସ୍ଥିର PyObject* (*dll_PyObject_Repr)(PyObject *);
ସ୍ଥିର ଅକ୍ଷର*(*dll_PyString_AsString)(PyObject *);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା(*dll_PyString_AsStringAndSize)(PyObject *, ଅକ୍ଷର **, PyInt *);
ସ୍ଥିର PyObject*(*dll_PyString_FromString)(ଅଚଳ ଅକ୍ଷର *);
ସ୍ଥିର PyObject*(*dll_PyString_FromFormat)(ଅଚଳ ଅକ୍ଷର *, ...);
ସ୍ଥିର PyObject*(*dll_PyString_FromStringAndSize)(ଅଚଳ ଅକ୍ଷର *, PyInt);
ସ୍ଥିର PyInt(*dll_PyString_Size)(PyObject *);
ସ୍ଥିର PyTypeObject* dll_PyString_Type;
ସ୍ଥିର PyTypeObject* dll_PyUnicode_Type;
ସ୍ଥିର PyObject *(*py_PyUnicode_AsEncodedString)(PyObject *, ଅକ୍ଷର *, ଅକ୍ଷର *);
ସ୍ଥିର ଦ୍ୱିଗୁଣ(*dll_PyFloat_AsDouble)(PyObject *);
ସ୍ଥିର PyObject*(*dll_PyFloat_FromDouble)(ଦ୍ୱିଗୁଣ);
ସ୍ଥିର PyTypeObject* dll_PyFloat_Type;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା(*dll_Py_ସଂଖ୍ୟା_Check)(PyObject *);
ସ୍ଥିର PyObject*(*dll_Py_ସଂଖ୍ୟା_Long)(PyObject *);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା(*dll_PySys_SetObject)(ଅକ୍ଷର *, PyObject *);
ସ୍ଥିର PyObject *(*dll_PySys_GetObject)(ଅକ୍ଷର *);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା(*dll_PySys_SetArgv)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ଅକ୍ଷର **);
ସ୍ଥିର PyTypeObject* dll_PyType_Type;
ସ୍ଥିର PyTypeObject* dll_PyFile_Type;
ସ୍ଥିର PyTypeObject* dll_PySlice_Type;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dll_PyType_Ready)(PyTypeObject *type);
ସ୍ଥିର PyObject* (*dll_PyType_GenericAlloc)(PyTypeObject *type, PyInt nitems);
ସ୍ଥିର PyObject*(*dll_Py_Build_ମୂଲ୍ୟ)(ଅକ୍ଷର *, ...);
ସ୍ଥିର PyObject*(*dll_Py_FindMethod)(ସଂରଚନା PyMethodDef[], PyObject *, ଅକ୍ଷର *);
ସ୍ଥିର PyObject*(*dll_Py_InitModule4)(ଅକ୍ଷର *, ସଂରଚନା PyMethodDef *, ଅକ୍ଷର *, PyObject *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ସ୍ଥିର PyObject*(*dll_PyImport_AddModule)(ଅକ୍ଷର *);
ସ୍ଥିର ଶୂନ୍ୟ(*dll_Py_SetPythonHome)(ଅକ୍ଷର *home);
ସ୍ଥିର ଶୂନ୍ୟ(*dll_Py_Initialize)(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ(*dll_Py_Finalize)(ଶୂନ୍ୟ);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା(*dll_Py_IsInitialized)(ଶୂନ୍ୟ);
ସ୍ଥିର PyObject*(*dll__PyObject_New)(PyTypeObject *, PyObject *);
ସ୍ଥିର PyObject*(*dll__PyObject_GC_New)(PyTypeObject *);
# ifdef Py_underscore_GC
ସ୍ଥିର ଶୂନ୍ୟ(*dll__PyObject_GC_Del)(ଶୂନ୍ୟ *);
ସ୍ଥିର ଶୂନ୍ୟ(*dll__PyObject_GC_UnTrack)(ଶୂନ୍ୟ *);
# ଅନ୍ୟଥା
ସ୍ଥିର ଶୂନ୍ୟ(*dll_PyObject_GC_Del)(ଶୂନ୍ୟ *);
ସ୍ଥିର ଶୂନ୍ୟ(*dll_PyObject_GC_UnTrack)(ଶୂନ୍ୟ *);
# endif
ସ୍ଥିର PyObject*(*dll__PyObject_Init)(PyObject *, PyTypeObject *);
ସ୍ଥିର PyObject* (*dll_PyObject_GetIter)(PyObject *);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dll_PyObject_IsTrue)(PyObject *);
# ଯଦି defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02070000
ସ୍ଥିର iternextfunc dll__PyObject_NextNotImplemented;
# endif
ସ୍ଥିର PyObject* dll__Py_NoneStruct;
ସ୍ଥିର PyObject* _Py_ZeroStruct;
ସ୍ଥିର PyObject* dll__Py_TrueStruct;
# ଯଦି defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02020000
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dll_PyType_IsSubtype)(PyTypeObject *, PyTypeObject *);
#  ifdef Py_DEBUG
ସ୍ଥିର ଶୂନ୍ୟ (*dll__Py_NegativeRefcount)(ଅଚଳ ଅକ୍ଷର *fname, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lineno, PyObject *op);
ସ୍ଥିର PyInt* dll__Py_RefTotal;
ସ୍ଥିର ଶୂନ୍ୟ (*dll__Py_Dealloc)(PyObject *obj);
#  endif
# endif
# ଯଦି defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02030000
#  ଯଦି defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
ସ୍ଥିର ଶୂନ୍ୟ (*dll__PyObject_DebugFree)(ଶୂନ୍ୟ*);
ସ୍ଥିର ଶୂନ୍ୟ* (*dll__PyObject_DebugMalloc)(size_t);
#  ଅନ୍ୟଥା
ସ୍ଥିର ଶୂନ୍ୟ* (*dll_PyObject_Malloc)(size_t);
ସ୍ଥିର ଶୂନ୍ୟ (*dll_PyObject_Free)(ଶୂନ୍ୟ*);
#  endif
# endif
# ifdef PY_USE_CAPSULE
ସ୍ଥିର PyObject* (*dll_PyCapsule_New)(ଶୂନ୍ୟ *, ଅକ୍ଷର *, PyCapsule_Destructor);
ସ୍ଥିର ଶୂନ୍ୟ* (*dll_PyCapsule_GetPointer)(PyObject *, ଅକ୍ଷର *);
# ଅନ୍ୟଥା
ସ୍ଥିର PyObject* (*dll_PyCObject_FromVoidPtr)(ଶୂନ୍ୟ *cobj, ଶୂନ୍ୟ (*destr)(ଶୂନ୍ୟ *));
ସ୍ଥିର ଶୂନ୍ୟ* (*dll_PyCObject_AsVoidPtr)(PyObject *);
# endif
# ଯଦି defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02070000
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା* dll_Py_NoSiteFlag;
# endif

ସ୍ଥିର HINSTANCE hinstPython = 0; // Instance of python.dll

// Imported exception objects
ସ୍ଥିର PyObject *imp_PyExc_AttributeError;
ସ୍ଥିର PyObject *imp_PyExc_IndexError;
ସ୍ଥିର PyObject *imp_Py_Exc_ଚାବି_Error;
ସ୍ଥିର PyObject *imp_PyExc_KeyboardInterrupt;
ସ୍ଥିର PyObject *imp_PyExc_TypeError;
ସ୍ଥିର PyObject *imp_Py_Exc_ମୂଲ୍ୟ_Error;
ସ୍ଥିର PyObject *imp_PyExc_SystemExit;
ସ୍ଥିର PyObject *imp_PyExc_RuntimeError;
ସ୍ଥିର PyObject *imp_PyExc_ImportError;
ସ୍ଥିର PyObject *imp_PyExc_OverflowError;

# define PyExc_AttributeError imp_PyExc_AttributeError
# define PyExc_IndexError imp_PyExc_IndexError
# define Py_Exc_ଚାବି_Error imp_Py_Exc_ଚାବି_Error
# define PyExc_KeyboardInterrupt imp_PyExc_KeyboardInterrupt
# define PyExc_TypeError imp_PyExc_TypeError
# define Py_Exc_ମୂଲ୍ୟ_Error imp_Py_Exc_ମୂଲ୍ୟ_Error
# define PyExc_SystemExit imp_PyExc_SystemExit
# define PyExc_RuntimeError imp_PyExc_RuntimeError
# define PyExc_ImportError imp_PyExc_ImportError
# define PyExc_OverflowError imp_PyExc_OverflowError

/*
 * Table of name to function pointer of python.
 */
# define PYTHON_PROC FARPROC
ସ୍ଥିର ସଂରଚନା
{
    ଅକ୍ଷର *name;
    PYTHON_PROC *ptr;
} python_funcname_table[] =
{
# ifdef PY_SSIZE_T_CLEAN
    {"_PyArg_Parse_SizeT", (PYTHON_PROC*)&dll_PyArg_Parse},
    {"_PyArg_ParseTuple_SizeT", (PYTHON_PROC*)&dll_PyArg_ParseTuple},
    {"_Py_BuildValue_SizeT", (PYTHON_PROC*)&dll_Py_Build_ମୂଲ୍ୟ},
# ଅନ୍ୟଥା
    {"PyArg_Parse", (PYTHON_PROC*)&dll_PyArg_Parse},
    {"PyArg_ParseTuple", (PYTHON_PROC*)&dll_PyArg_ParseTuple},
    {"Py_BuildValue", (PYTHON_PROC*)&dll_Py_Build_ମୂଲ୍ୟ},
# endif
    {"PyMem_Free", (PYTHON_PROC*)&dll_PyMem_Free},
    {"PyMem_Malloc", (PYTHON_PROC*)&dll_PyMem_Malloc},
    {"PyDict_SetItemString", (PYTHON_PROC*)&dll_PyDict_SetItemString},
    {"PyErr_BadArgument", (PYTHON_PROC*)&dll_PyErr_BadArgument},
    {"PyErr_NewException", (PYTHON_PROC*)&dll_PyErr_NewException},
    {"PyErr_Clear", (PYTHON_PROC*)&dll_PyErr_Clear},
    {"PyErr_Format", (PYTHON_PROC*)&dll_PyErr_Format},
    {"PyErr_PrintEx", (PYTHON_PROC*)&dll_PyErr_PrintEx},
    {"PyErr_NoMemory", (PYTHON_PROC*)&dll_PyErr_NoMemory},
    {"PyErr_Occurred", (PYTHON_PROC*)&dll_PyErr_Occurred},
    {"PyErr_SetNone", (PYTHON_PROC*)&dll_PyErr_SetNone},
    {"PyErr_SetString", (PYTHON_PROC*)&dll_PyErr_SetString},
    {"PyErr_SetObject", (PYTHON_PROC*)&dll_PyErr_SetObject},
    {"PyErr_ExceptionMatches", (PYTHON_PROC*)&dll_PyErr_ExceptionMatches},
    {"PyEval_InitThreads", (PYTHON_PROC*)&dll_PyEval_InitThreads},
    {"PyEval_RestoreThread", (PYTHON_PROC*)&dll_PyEval_RestoreThread},
    {"PyEval_SaveThread", (PYTHON_PROC*)&dll_PyEval_SaveThread},
# ifdef PY_CAN_RECURSE
    {"PyGILState_Ensure", (PYTHON_PROC*)&dll_PyGILState_Ensure},
    {"PyGILState_Release", (PYTHON_PROC*)&dll_PyGILState_Release},
# endif
    {"PyInt_AsLong", (PYTHON_PROC*)&dll_PyInt_AsLong},
    {"PyInt_FromLong", (PYTHON_PROC*)&dll_PyInt_FromLong},
    {"PyLong_AsLong", (PYTHON_PROC*)&dll_PyLong_AsLong},
    {"PyLong_FromLong", (PYTHON_PROC*)&dll_PyLong_FromLong},
    {"PyBool_Type", (PYTHON_PROC*)&dll_PyBool_Type},
    {"PyInt_Type", (PYTHON_PROC*)&dll_PyInt_Type},
    {"PyLong_Type", (PYTHON_PROC*)&dll_PyLong_Type},
    {"PyList_GetItem", (PYTHON_PROC*)&dll_PyList_GetItem},
    {"PyList_Append", (PYTHON_PROC*)&dll_PyList_Append},
    {"PyList_Insert", (PYTHON_PROC*)&dll_Py_List_ଭର୍ତ୍ତି},
    {"PyList_New", (PYTHON_PROC*)&dll_PyList_New},
    {"PyList_SetItem", (PYTHON_PROC*)&dll_PyList_SetItem},
    {"PyList_Size", (PYTHON_PROC*)&dll_PyList_Size},
    {"PyList_Type", (PYTHON_PROC*)&dll_PyList_Type},
    {"PyTuple_GetItem", (PYTHON_PROC*)&dll_PyTuple_GetItem},
    {"PyTuple_SetItem", (PYTHON_PROC*)&dll_PyTuple_SetItem},
    {"PyTuple_New", (PYTHON_PROC*)&dll_PyTuple_New},
    {"PyTuple_Size", (PYTHON_PROC*)&dll_PyTuple_Size},
    {"PyTuple_Type", (PYTHON_PROC*)&dll_PyTuple_Type},
    {"PySequence_Size", (PYTHON_PROC*)&dll_PySequence_Size},
    {"PySequence_Check", (PYTHON_PROC*)&dll_PySequence_Check},
    {"PySequence_GetItem", (PYTHON_PROC*)&dll_PySequence_GetItem},
    {"PySequence_Fast", (PYTHON_PROC*)&dll_PySequence_Fast},
    {"PySlice_GetIndicesEx", (PYTHON_PROC*)&dll_PySlice_GetIndicesEx},
    {"PyImport_ImportModule", (PYTHON_PROC*)&dll_PyImport_ImportModule},
    {"PyDict_GetItemString", (PYTHON_PROC*)&dll_PyDict_GetItemString},
    {"PyDict_Next", (PYTHON_PROC*)&dll_PyDict_Next},
    {"PyDict_New", (PYTHON_PROC*)&dll_PyDict_New},
    {"PyDict_Type", (PYTHON_PROC*)&dll_PyDict_Type},
# ifndef PY_NO_MAPPING_ଚାବିଗୁଡ଼ିକ
    {"PyMapping_Keys", (PYTHON_PROC*)&dll_Py_Mapping_ଚାବିଗୁଡ଼ିକ},
# endif
    {"PyObject_GetItem", (PYTHON_PROC*)&dll_PyObject_GetItem},
    {"PyObject_CallMethod", (PYTHON_PROC*)&dll_PyObject_CallMethod},
    {"PyMapping_Check", (PYTHON_PROC*)&dll_PyMapping_Check},
    {"PyIter_Next", (PYTHON_PROC*)&dll_PyIter_Next},
    {"PyModule_GetDict", (PYTHON_PROC*)&dll_PyModule_GetDict},
    {"PyModule_AddObject", (PYTHON_PROC*)&dll_PyModule_AddObject},
    {"PyRun_SimpleString", (PYTHON_PROC*)&dll_PyRun_SimpleString},
    {"PyRun_String", (PYTHON_PROC*)&dll_PyRun_String},
    {"PyObject_GetAttrString", (PYTHON_PROC*)&dll_PyObject_GetAttrString},
    {"PyObject_HasAttrString", (PYTHON_PROC*)&dll_PyObject_HasAttrString},
    {"PyObject_SetAttrString", (PYTHON_PROC*)&dll_PyObject_SetAttrString},
    {"PyObject_CallFunctionObjArgs", (PYTHON_PROC*)&dll_PyObject_CallFunctionObjArgs},
    {"PyObject_CallFunction", (PYTHON_PROC*)&dll_PyObject_CallFunction},
    {"PyObject_Call", (PYTHON_PROC*)&dll_PyObject_Call},
    {"PyObject_Repr", (PYTHON_PROC*)&dll_PyObject_Repr},
    {"PyString_AsString", (PYTHON_PROC*)&dll_PyString_AsString},
    {"PyString_AsStringAndSize", (PYTHON_PROC*)&dll_PyString_AsStringAndSize},
    {"PyString_FromString", (PYTHON_PROC*)&dll_PyString_FromString},
    {"PyString_FromFormat", (PYTHON_PROC*)&dll_PyString_FromFormat},
    {"PyString_FromStringAndSize", (PYTHON_PROC*)&dll_PyString_FromStringAndSize},
    {"PyString_Size", (PYTHON_PROC*)&dll_PyString_Size},
    {"PyString_Type", (PYTHON_PROC*)&dll_PyString_Type},
    {"PyUnicode_Type", (PYTHON_PROC*)&dll_PyUnicode_Type},
    {"PyFloat_Type", (PYTHON_PROC*)&dll_PyFloat_Type},
    {"PyFloat_AsDouble", (PYTHON_PROC*)&dll_PyFloat_AsDouble},
    {"PyFloat_FromDouble", (PYTHON_PROC*)&dll_PyFloat_FromDouble},
    {"PyImport_AddModule", (PYTHON_PROC*)&dll_PyImport_AddModule},
    {"PyNumber_Check", (PYTHON_PROC*)&dll_Py_ସଂଖ୍ୟା_Check},
    {"PyNumber_Long", (PYTHON_PROC*)&dll_Py_ସଂଖ୍ୟା_Long},
    {"PySys_SetObject", (PYTHON_PROC*)&dll_PySys_SetObject},
    {"PySys_GetObject", (PYTHON_PROC*)&dll_PySys_GetObject},
    {"PySys_SetArgv", (PYTHON_PROC*)&dll_PySys_SetArgv},
    {"PyType_Type", (PYTHON_PROC*)&dll_PyType_Type},
    {"PyFile_Type", (PYTHON_PROC*)&dll_PyFile_Type},
    {"PySlice_Type", (PYTHON_PROC*)&dll_PySlice_Type},
    {"PyType_Ready", (PYTHON_PROC*)&dll_PyType_Ready},
    {"PyType_GenericAlloc", (PYTHON_PROC*)&dll_PyType_GenericAlloc},
    {"Py_FindMethod", (PYTHON_PROC*)&dll_Py_FindMethod},
    {"Py_SetPythonHome", (PYTHON_PROC*)&dll_Py_SetPythonHome},
    {"Py_Initialize", (PYTHON_PROC*)&dll_Py_Initialize},
    {"Py_Finalize", (PYTHON_PROC*)&dll_Py_Finalize},
    {"Py_IsInitialized", (PYTHON_PROC*)&dll_Py_IsInitialized},
    {"_PyObject_New", (PYTHON_PROC*)&dll__PyObject_New},
    {"_PyObject_GC_New", (PYTHON_PROC*)&dll__PyObject_GC_New},
# ifdef Py_underscore_GC
    {"_PyObject_GC_Del", (PYTHON_PROC*)&dll__PyObject_GC_Del},
    {"_PyObject_GC_UnTrack", (PYTHON_PROC*)&dll__PyObject_GC_UnTrack},
# ଅନ୍ୟଥା
    {"PyObject_GC_Del", (PYTHON_PROC*)&dll_PyObject_GC_Del},
    {"PyObject_GC_UnTrack", (PYTHON_PROC*)&dll_PyObject_GC_UnTrack},
# endif
    {"PyObject_Init", (PYTHON_PROC*)&dll__PyObject_Init},
    {"PyObject_GetIter", (PYTHON_PROC*)&dll_PyObject_GetIter},
    {"PyObject_IsTrue", (PYTHON_PROC*)&dll_PyObject_IsTrue},
# ଯଦି defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02070000
    {"_PyObject_NextNotImplemented", (PYTHON_PROC*)&dll__PyObject_NextNotImplemented},
# endif
    {"_Py_NoneStruct", (PYTHON_PROC*)&dll__Py_NoneStruct},
    {"_Py_ZeroStruct", (PYTHON_PROC*)&dll__Py_ZeroStruct},
    {"_Py_TrueStruct", (PYTHON_PROC*)&dll__Py_TrueStruct},
# ଯଦି defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02020000
#  ifdef Py_DEBUG
    {"_Py_NegativeRefcount", (PYTHON_PROC*)&dll__Py_NegativeRefcount},
    {"_Py_RefTotal", (PYTHON_PROC*)&dll__Py_RefTotal},
    {"_Py_Dealloc", (PYTHON_PROC*)&dll__Py_Dealloc},
#  endif
    {"PyType_IsSubtype", (PYTHON_PROC*)&dll_PyType_IsSubtype},
# endif
# ଯଦି defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02030000
#  ଯଦି defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
    {"_PyObject_DebugFree", (PYTHON_PROC*)&dll__PyObject_DebugFree},
    {"_PyObject_DebugMalloc", (PYTHON_PROC*)&dll__PyObject_DebugMalloc},
#  ଅନ୍ୟଥା
    {"PyObject_Malloc", (PYTHON_PROC*)&dll_PyObject_Malloc},
    {"PyObject_Free", (PYTHON_PROC*)&dll_PyObject_Free},
#  endif
# endif
# ଯଦି defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02050000 \
	&& SIZEOF_SIZE_T != VIM_SIZEOF_INT
#  ifdef Py_DEBUG
    {"Py_InitModule4TraceRefs_64", (PYTHON_PROC*)&dll_Py_InitModule4},
#  ଅନ୍ୟଥା
    {"Py_InitModule4_64", (PYTHON_PROC*)&dll_Py_InitModule4},
#  endif
# ଅନ୍ୟଥା
#  ifdef Py_DEBUG
    {"Py_InitModule4TraceRefs", (PYTHON_PROC*)&dll_Py_InitModule4},
#  ଅନ୍ୟଥା
    {"Py_InitModule4", (PYTHON_PROC*)&dll_Py_InitModule4},
#  endif
# endif
# ifdef PY_USE_CAPSULE
    {"PyCapsule_New", (PYTHON_PROC*)&dll_PyCapsule_New},
    {"PyCapsule_GetPointer", (PYTHON_PROC*)&dll_PyCapsule_GetPointer},
# ଅନ୍ୟଥା
    {"PyCObject_FromVoidPtr", (PYTHON_PROC*)&dll_PyCObject_FromVoidPtr},
    {"PyCObject_AsVoidPtr", (PYTHON_PROC*)&dll_PyCObject_AsVoidPtr},
# endif
# ଯଦି defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02070000
    {"Py_NoSiteFlag", (PYTHON_PROC*)&dll_Py_NoSiteFlag},
# endif
    {"", ଖାଲି},
};

/*
 * Load library and get all pointers.
 * Parameter 'libname' provides name of DLL.
 * Return OK or FAIL.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
python_runtime_link_init(ଅକ୍ଷର *libname, ପୂର୍ଣ୍ଣସଂଖ୍ୟା verbose)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
    PYTHON_PROC *ucs_as_encoded_string =
				   (PYTHON_PROC*)&py_PyUnicode_AsEncodedString;

# ଯଦି !(defined(PY_NO_RTLD_GLOBAL) && defined(PY3_NO_RTLD_GLOBAL)) && defined(UNIX) && defined(FEAT_PYTHON3)
    // Can't have Python and Python3 loaded at the same time.
    // It causes a crash, because RTLD_GLOBAL is needed for
    // standard C extension libraries of one or both python versions.
    ଯଦି (python3_loaded())
    {
	ଯଦି (verbose)
	    emsg(_(e_this_vim_cannot_execute_python_after_using_py3));
	ଫେରନ୍ତୁ FAIL;
    }
# endif

    ଯଦି (hinstPython)
	ଫେରନ୍ତୁ OK;
    hinstPython = load_dll(libname);
    ଯଦି (!hinstPython)
    {
	ଯଦି (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	ଫେରନ୍ତୁ FAIL;
    }

    ପାଇଁ (i = 0; python_funcname_table[i].ptr; ++i)
    {
	ଯଦି ((*python_funcname_table[i].ptr = ପ୍ରତୀକ_from_dll(hinstPython,
			python_funcname_table[i].name)) == ଖାଲି)
	{
	    close_dll(hinstPython);
	    hinstPython = 0;
	    ଯଦି (verbose)
		semsg(_(e_could_not_load_library_function_str), python_funcname_table[i].name);
	    ଫେରନ୍ତୁ FAIL;
	}
    }

    // Load unicode functions separately as only the ucs2 or the ucs4 functions
    // will be present in the library.
    *ucs_as_encoded_string = ପ୍ରତୀକ_from_dll(hinstPython,
					     "PyUnicodeUCS2_AsEncodedString");
    ଯଦି (*ucs_as_encoded_string == ଖାଲି)
	*ucs_as_encoded_string = ପ୍ରତୀକ_from_dll(hinstPython,
					     "PyUnicodeUCS4_AsEncodedString");
    ଯଦି (*ucs_as_encoded_string == ଖାଲି)
    {
	close_dll(hinstPython);
	hinstPython = 0;
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
python_enabled(ପୂର୍ଣ୍ଣସଂଖ୍ୟା verbose)
{
    ଫେରନ୍ତୁ python_runtime_link_init((ଅକ୍ଷର *)p_pydll, verbose) == OK;
}

/*
 * Load the standard Python exceptions - don't import the symbols from the
 * DLL, as this can cause errors (importing data symbols is not reliable).
 */
    ସ୍ଥିର ଶୂନ୍ୟ
get_exceptions(ଶୂନ୍ୟ)
{
    PyObject *exmod = PyImport_ImportModule("exceptions");
    PyObject *exdict = PyModule_GetDict(exmod);
    imp_PyExc_AttributeError = PyDict_GetItemString(exdict, "AttributeError");
    imp_PyExc_IndexError = PyDict_GetItemString(exdict, "IndexError");
    imp_Py_Exc_ଚାବି_Error = PyDict_GetItemString(exdict, "KeyError");
    imp_PyExc_KeyboardInterrupt = PyDict_GetItemString(exdict, "KeyboardInterrupt");
    imp_PyExc_TypeError = PyDict_GetItemString(exdict, "TypeError");
    imp_Py_Exc_ମୂଲ୍ୟ_Error = PyDict_GetItemString(exdict, "ValueError");
    imp_PyExc_SystemExit = PyDict_GetItemString(exdict, "SystemExit");
    imp_PyExc_RuntimeError = PyDict_GetItemString(exdict, "RuntimeError");
    imp_PyExc_ImportError = PyDict_GetItemString(exdict, "ImportError");
    imp_PyExc_OverflowError = PyDict_GetItemString(exdict, "OverflowError");
    Py_XINCREF(imp_PyExc_AttributeError);
    Py_XINCREF(imp_PyExc_IndexError);
    Py_XINCREF(imp_Py_Exc_ଚାବି_Error);
    Py_XINCREF(imp_PyExc_KeyboardInterrupt);
    Py_XINCREF(imp_PyExc_TypeError);
    Py_XINCREF(imp_Py_Exc_ମୂଲ୍ୟ_Error);
    Py_XINCREF(imp_PyExc_SystemExit);
    Py_XINCREF(imp_PyExc_RuntimeError);
    Py_XINCREF(imp_PyExc_ImportError);
    Py_XINCREF(imp_PyExc_OverflowError);
    Py_XDECREF(exmod);
}
#endif // DYNAMIC_PYTHON

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା initialised = 0;
#define PYINITIALISED initialised
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା python_end_called = FALSE;

#define DESTRUCTOR_FINISH(self) self->ob_type->tp_free((PyObject*)self);

#define WIN_PYTHON_REF(win) win->w_python_ref
#define BUF_PYTHON_REF(buf) buf->b_python_ref
#define TAB_PYTHON_REF(tab) tab->tp_python_ref

ସ୍ଥିର PyObject *OutputGetattr(PyObject *, ଅକ୍ଷର *);
ସ୍ଥିର PyObject *BufferGetattr(PyObject *, ଅକ୍ଷର *);
ସ୍ଥିର PyObject *WindowGetattr(PyObject *, ଅକ୍ଷର *);
ସ୍ଥିର PyObject *TabPageGetattr(PyObject *, ଅକ୍ଷର *);
ସ୍ଥିର PyObject *RangeGetattr(PyObject *, ଅକ୍ଷର *);
ସ୍ଥିର PyObject *DictionaryGetattr(PyObject *, ଅକ୍ଷର*);
ସ୍ଥିର PyObject *ListGetattr(PyObject *, ଅକ୍ଷର *);
ସ୍ଥିର PyObject *TupleGetattr(PyObject *, ଅକ୍ଷର *);
ସ୍ଥିର PyObject *FunctionGetattr(PyObject *, ଅକ୍ଷର *);

#ifndef Py_VISIT
# define Py_VISIT(obj) visit(obj, arg)
#endif
#ifndef Py_CLEAR
# define Py_CLEAR(obj) \
    { \
	Py_XDECREF(obj); \
	obj = ଖାଲି; \
    }
#endif

#ଯଦି defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    ସ୍ଥିର ଶୂନ୍ୟ *
py_memsave(ଶୂନ୍ୟ *p, size_t len)
{
    ଶୂନ୍ୟ	*r;

    ଯଦି (!(r = PyMem_Malloc(len)))
	ଫେରନ୍ତୁ ଖାଲି;
    mch_memmove(r, p, len);
    ଫେରନ୍ତୁ r;
}

# define PY_STRSAVE(s) ((char_u *) py_memsave(s, STRLEN(s) + 1))
#endif

ପ୍ରକାରବ୍ୟାଖ୍ୟା PySliceObject PySliceObject_T;

/*
 * Include the code shared with if_python3.c
 */
#ଅନ୍ତର୍ଭାଵ "if_py_both.h"


///////////////////////////////////////////////////////
// Internal function prototypes.

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା PythonMod_Init(ଶୂନ୍ୟ);


///////////////////////////////////////////////////////
// 1. Python interpreter main program.

#ifndef PY_CAN_RECURSE
ସ୍ଥିର PyThreadState *saved_python_thread = ଖାଲି;

/*
 * Suspend a thread of the Python interpreter, other threads are allowed to
 * run.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
Python_SaveThread(ଶୂନ୍ୟ)
{
    saved_python_thread = PyEval_SaveThread();
}

/*
 * Restore a thread of the Python interpreter, waits for other threads to
 * block.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
Python_RestoreThread(ଶୂନ୍ୟ)
{
    PyEval_RestoreThread(saved_python_thread);
    saved_python_thread = ଖାଲି;
}
#endif

    ଶୂନ୍ୟ
python_end(ଶୂନ୍ୟ)
{
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା recurse = 0;

    // If a crash occurs while doing this, don't try again.
    ଯଦି (recurse != 0)
	ଫେରନ୍ତୁ;

    python_end_called = TRUE;
    ++recurse;

#ifdef DYNAMIC_PYTHON
    ଯଦି (hinstPython && Py_IsInitialized())
    {
# ifdef PY_CAN_RECURSE
	PyGILState_Ensure();
# ଅନ୍ୟଥା
	Python_RestoreThread();	    // enter python
# endif
	Py_Finalize();
    }
#ଅନ୍ୟଥା
    ଯଦି (Py_IsInitialized())
    {
# ifdef PY_CAN_RECURSE
	PyGILState_Ensure();
# ଅନ୍ୟଥା
	Python_RestoreThread();	    // enter python
# endif
	Py_Finalize();
    }
#endif

    --recurse;
}

#ଯଦି defined(DYNAMIC_PYTHON) && defined(FEAT_PYTHON3)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
python_loaded(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ (hinstPython != 0);
}
#endif

ସ୍ଥିର ଅକ୍ଷର *py_home_buf = ଖାଲି;

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
Python_Init(ଶୂନ୍ୟ)
{
    ଯଦି (!initialised)
    {
#ଯଦି defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02070000
	PyObject *site;
#endif

#ifdef DYNAMIC_PYTHON
	ଯଦି (!python_enabled(TRUE))
	{
	    emsg(_(e_sorry_this_command_is_disabled_python_library_could_not_be_found));
	    ଯାଅ fail;
	}
#endif

	ଯଦି (*p_pyhome != NUL)
	{
	    // The string must not change later, make a copy in static memory.
	    py_home_buf = (ଅକ୍ଷର *)vim_strsave(p_pyhome);
	    ଯଦି (py_home_buf != ଖାଲି)
		Py_SetPythonHome(py_home_buf);
	}
#ifdef PYTHON_HOME
	ଅନ୍ୟଥା ଯଦି (mch_getenv((char_u *)"PYTHONHOME") == ଖାଲି)
	    Py_SetPythonHome(PYTHON_HOME);
#endif

	init_structs();

#ଯଦି defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02070000
	// Disable implicit 'import site', because it may cause Vim to exit
	// when it can't be found.
	Py_NoSiteFlag++;
#endif

	Py_Initialize();

#ଯଦି defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02070000
	// 'import site' explicitly.
	site = PyImport_ImportModule("site");
	ଯଦି (site == ଖାଲି)
	{
	    emsg(_(e_sorry_this_command_is_disabled_python_site_module_could_not_be_loaded));
	    ଯାଅ fail;
	}
	Py_DECREF(site);
#endif

	// Initialise threads, and below save the state using
	// PyEval_SaveThread.  Without the call to PyEval_SaveThread, thread
	// specific state (such as the system trace hook), will be lost
	// between invocations of Python code.
	PyEval_InitThreads();
#ifdef DYNAMIC_PYTHON
	get_exceptions();
#endif

	ଯଦି (PythonIO_Init_io())
	    ଯାଅ fail;

	ଯଦି (PythonMod_Init())
	    ଯାଅ fail;

	globals = PyModule_GetDict(PyImport_AddModule("__main__"));

	// Remove the element from sys.path that was added because of our
	// argv[0] value in PythonMod_Init().  Previously we used an empty
	// string, but depending on the OS we then get an empty entry or
	// the current directory in sys.path.
	PyRun_SimpleString("import sys; sys.path = filter(lambda x: x != '/must>not&exist', sys.path)");

	// lock is created and acquired in PyEval_InitThreads() and thread
	// state is created in Py_Initialize()
	// there _PyGILState_NoteThreadState() also sets gilcounter to 1
	// (python must have threads enabled!)
	// so the following does both: unlock GIL and save thread state in TLS
	// without deleting thread state
#ifndef PY_CAN_RECURSE
	saved_python_thread =
#endif
	    PyEval_SaveThread();

	initialised = 1;
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
DoPyCommand(
    ଅଚଳ ଅକ୍ଷର		*cmd,
    dict_T*		locals,
    rangeinitializer	init_range,
    runner		run,
    ଶୂନ୍ୟ		*arg)
{
#ifndef PY_CAN_RECURSE
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା		recursive = 0;
#endif
#ଯଦି defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    ଅକ୍ଷର		*saved_locale;
#endif
#ifdef PY_CAN_RECURSE
    PyGILState_STATE	pygilstate;
#endif

#ifndef PY_CAN_RECURSE
    ଯଦି (recursive)
    {
	emsg(_(e_cannot_invoke_python_recursively));
	ଫେରନ୍ତୁ;
    }
    ++recursive;
#endif
    ଯଦି (python_end_called)
	ଫେରନ୍ତୁ;

    ଯଦି (Python_Init())
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
	saved_locale = (ଅକ୍ଷର *) PY_STRSAVE(saved_locale);
	(ଶୂନ୍ୟ)setlocale(LC_NUMERIC, "C");
    }
#endif

#ifdef PY_CAN_RECURSE
    pygilstate = PyGILState_Ensure();
#ଅନ୍ୟଥା
    Python_RestoreThread();	    // enter python
#endif

    run((ଅକ୍ଷର *) cmd, locals, arg
#ifdef PY_CAN_RECURSE
	    , &pygilstate
#endif
	    );

#ifdef PY_CAN_RECURSE
    PyGILState_Release(pygilstate);
#ଅନ୍ୟଥା
    Python_SaveThread();	    // leave python
#endif

#ଯଦି defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    ଯଦି (saved_locale != ଖାଲି)
    {
	(ଶୂନ୍ୟ)setlocale(LC_NUMERIC, saved_locale);
	PyMem_Free(saved_locale);
    }
#endif

    Python_Lock_Vim();		    // enter vim
    PythonIO_Flush();

theend:
#ifndef PY_CAN_RECURSE
    --recursive;
#endif
    ଫେରନ୍ତୁ;
}

/*
 * ":python"
 */
    ଶୂନ୍ୟ
ex_python(exarg_T *eap)
{
    char_u *script;

    script = script_get(eap, eap->arg);
    ଯଦି (!eap->skip)
    {
	ଯଦି (p_pyx == 0)
	    p_pyx = 2;

	DoPyCommand(script == ଖାଲି ? (ଅକ୍ଷର *) eap->arg : (ଅକ୍ଷର *) script,
		ଖାଲି,
		init_range_cmd,
		(runner) run_cmd,
		(ଶୂନ୍ୟ *) eap);
    }
    vim_free(script);
}

#define BUFFER_SIZE 1024

/*
 * ":pyfile"
 */
    ଶୂନ୍ୟ
ex_pyfile(exarg_T *eap)
{
    ସ୍ଥିର ଅକ୍ଷର buffer[BUFFER_SIZE];
    ଅଚଳ ଅକ୍ଷର *file = (ଅକ୍ଷର *)eap->arg;
    ଅକ୍ଷର *p;

    ଯଦି (p_pyx == 0)
	p_pyx = 2;

    // Have to do it like this. PyRun_SimpleFile requires you to pass a
    // stdio file pointer, but Vim and the Python DLL are compiled with
    // different options under Windows, meaning that stdio pointers aren't
    // compatible between the two. Yuk.
    //
    // Put the string "execfile('file')" into buffer. But, we need to
    // escape any backslashes or single quotes in the file name, so that
    // Python won't mangle the file name.
    strcpy(buffer, "execfile('");
    p = buffer + 10; // size of "execfile('"

    ଯେତେବେଳେ (*file && p < buffer + (BUFFER_SIZE - 3))
    {
	ଯଦି (*file == '\\' || *file == '\'')
	    *p++ = '\\';
	*p++ = *file++;
    }

    // If we didn't finish the file name, we hit a buffer overflow
    ଯଦି (*file != '\0')
	ଫେରନ୍ତୁ;

    // Put in the terminating "')" and a null
    *p++ = '\'';
    *p++ = ')';
    *p++ = '\0';

    // Execute the file
    DoPyCommand(buffer,
	    ଖାଲି,
	    init_range_cmd,
	    (runner) run_cmd,
	    (ଶୂନ୍ୟ *) eap);
}

    ଶୂନ୍ୟ
ex_pydo(exarg_T *eap)
{
    ଯଦି (p_pyx == 0)
	p_pyx = 2;

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
OutputGetattr(PyObject *self, ଅକ୍ଷର *name)
{
    ଯଦି (strcmp(name, "softspace") == 0)
	ଫେରନ୍ତୁ PyInt_FromLong(((OutputObject *)(self))->softspace);
    ଅନ୍ୟଥା ଯଦି (strcmp(name, "__members__") == 0)
	ଫେରନ୍ତୁ ObjectDir(ଖାଲି, OutputAttrs);
    ଅନ୍ୟଥା ଯଦି (strcmp(name, "errors") == 0)
	ଫେରନ୍ତୁ PyString_FromString("strict");
    ଅନ୍ୟଥା ଯଦି (strcmp(name, "encoding") == 0)
	ଫେରନ୍ତୁ PyString_FromString(ENC_OPT);
    ଫେରନ୍ତୁ Py_FindMethod(OutputMethods, self, name);
}

///////////////////////////////////////////////////////
// 3. Implementation of the Vim module for Python

// Window type - Implementation functions
// --------------------------------------

#define WindowType_Check(obj) ((obj)->ob_type == &WindowType)

// Buffer type - Implementation functions
// --------------------------------------

#define BufferType_Check(obj) ((obj)->ob_type == &BufferType)

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା BufferAssItem(PyObject *, PyInt, PyObject *);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା BufferAssSlice(PyObject *, PyInt, PyInt, PyObject *);

// Line range type - Implementation functions
// --------------------------------------

#define RangeType_Check(obj) ((obj)->ob_type == &RangeType)

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା RangeAssItem(PyObject *, PyInt, PyObject *);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା RangeAssSlice(PyObject *, PyInt, PyInt, PyObject *);

// Current objects type - Implementation functions
// -----------------------------------------------

ସ୍ଥିର PySequenceMethods BufferAsSeq = {
    (PyInquiry)		BufferLength,	    // sq_length,    len(x)
    (binaryfunc)	0,		    // BufferConcat, sq_concat, x+y
    (PyIntArgFunc)	0,		    // BufferRepeat, sq_repeat, x*n
    (PyIntArgFunc)	BufferItem,	    // sq_item,      x[i]
    (PyIntIntArgFunc)	BufferSlice,	    // sq_slice,     x[i:j]
    (PyIntObjArgProc)	BufferAssItem,	    // sq_ass_item,  x[i]=v
    (PyIntIntObjArgProc) BufferAssSlice,    // sq_ass_slice, x[i:j]=v
    (objobjproc)	0,
    (binaryfunc)	0,
    0,
};

// Buffer object - Implementation

    ସ୍ଥିର PyObject *
BufferGetattr(PyObject *self, ଅକ୍ଷର *name)
{
    PyObject *r;

    ଯଦି ((r = BufferAttrValid((BufferObject *)(self), name)))
	ଫେରନ୍ତୁ r;

    ଯଦି (CheckBuffer((BufferObject *)(self)))
	ଫେରନ୍ତୁ ଖାଲି;

    r = BufferAttr((BufferObject *)(self), name);
    ଯଦି (r || PyErr_Occurred())
	ଫେରନ୍ତୁ r;
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ Py_FindMethod(BufferMethods, self, name);
}

//////////////////

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
BufferAssItem(PyObject *self, PyInt n, PyObject *val)
{
    ଫେରନ୍ତୁ RBAsItem((BufferObject *)(self), n, val, 1, -1, ଖାଲି);
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
BufferAssSlice(PyObject *self, PyInt lo, PyInt hi, PyObject *val)
{
    ଫେରନ୍ତୁ RBAsSlice((BufferObject *)(self), lo, hi, val, 1, -1, ଖାଲି);
}

ସ୍ଥିର PySequenceMethods RangeAsSeq = {
    (PyInquiry)		RangeLength,	      // sq_length,    len(x)
    (binaryfunc)	0, /* RangeConcat, */ // sq_concat,    x+y
    (PyIntArgFunc)	0, /* RangeRepeat, */ // sq_repeat,    x*n
    (PyIntArgFunc)	RangeItem,	      // sq_item,      x[i]
    (PyIntIntArgFunc)	RangeSlice,	      // sq_slice,     x[i:j]
    (PyIntObjArgProc)	RangeAssItem,	      // sq_ass_item,  x[i]=v
    (PyIntIntObjArgProc) RangeAssSlice,	      // sq_ass_slice, x[i:j]=v
    (objobjproc)	0,
#ଯଦି PY_MAJOR_VERSION >= 2
    (binaryfunc)	0,
    0,
#endif
};

// Line range object - Implementation

    ସ୍ଥିର PyObject *
RangeGetattr(PyObject *self, ଅକ୍ଷର *name)
{
    ଯଦି (strcmp(name, "start") == 0)
	ଫେରନ୍ତୁ Py_Build_ମୂଲ୍ୟ(Py_ssize_t_fmt, ((RangeObject *)(self))->start - 1);
    ଅନ୍ୟଥା ଯଦି (strcmp(name, "end") == 0)
	ଫେରନ୍ତୁ Py_Build_ମୂଲ୍ୟ(Py_ssize_t_fmt, ((RangeObject *)(self))->end - 1);
    ଅନ୍ୟଥା ଯଦି (strcmp(name, "__members__") == 0)
	ଫେରନ୍ତୁ ObjectDir(ଖାଲି, RangeAttrs);
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ Py_FindMethod(RangeMethods, self, name);
}

////////////////

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
RangeAssItem(PyObject *self, PyInt n, PyObject *val)
{
    ଫେରନ୍ତୁ RBAsItem(((RangeObject *)(self))->buf, n, val,
		     ((RangeObject *)(self))->start,
		     ((RangeObject *)(self))->end,
		     &((RangeObject *)(self))->end);
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
RangeAssSlice(PyObject *self, PyInt lo, PyInt hi, PyObject *val)
{
    ଫେରନ୍ତୁ RBAsSlice(((RangeObject *)(self))->buf, lo, hi, val,
		      ((RangeObject *)(self))->start,
		      ((RangeObject *)(self))->end,
		      &((RangeObject *)(self))->end);
}

// TabPage object - Implementation

    ସ୍ଥିର PyObject *
TabPageGetattr(PyObject *self, ଅକ୍ଷର *name)
{
    PyObject *r;

    ଯଦି ((r = TabPageAttrValid((TabPageObject *)(self), name)))
	ଫେରନ୍ତୁ r;

    ଯଦି (CheckTabPage((TabPageObject *)(self)))
	ଫେରନ୍ତୁ ଖାଲି;

    r = TabPageAttr((TabPageObject *)(self), name);
    ଯଦି (r || PyErr_Occurred())
	ଫେରନ୍ତୁ r;
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ Py_FindMethod(TabPageMethods, self, name);
}

// Window object - Implementation

    ସ୍ଥିର PyObject *
WindowGetattr(PyObject *self, ଅକ୍ଷର *name)
{
    PyObject *r;

    ଯଦି ((r = WindowAttrValid((WindowObject *)(self), name)))
	ଫେରନ୍ତୁ r;

    ଯଦି (CheckWindow((WindowObject *)(self)))
	ଫେରନ୍ତୁ ଖାଲି;

    r = WindowAttr((WindowObject *)(self), name);
    ଯଦି (r || PyErr_Occurred())
	ଫେରନ୍ତୁ r;
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ Py_FindMethod(WindowMethods, self, name);
}

// Tab page list object - Definitions

ସ୍ଥିର PySequenceMethods TabListAsSeq = {
    (PyInquiry)		TabListLength,	    // sq_length,    len(x)
    (binaryfunc)	0,		    // sq_concat,    x+y
    (PyIntArgFunc)	0,		    // sq_repeat,    x*n
    (PyIntArgFunc)	TabListItem,	    // sq_item,      x[i]
    (PyIntIntArgFunc)	0,		    // sq_slice,     x[i:j]
    (PyIntObjArgProc)	0,		    // sq_ass_item,  x[i]=v
    (PyIntIntObjArgProc) 0,		    // sq_ass_slice, x[i:j]=v
    (objobjproc)	0,
#ଯଦି PY_MAJOR_VERSION >= 2
    (binaryfunc)	0,
    0,
#endif
};

// Window list object - Definitions

ସ୍ଥିର PySequenceMethods WinListAsSeq = {
    (PyInquiry)		WinListLength,	    // sq_length,    len(x)
    (binaryfunc)	0,		    // sq_concat,    x+y
    (PyIntArgFunc)	0,		    // sq_repeat,    x*n
    (PyIntArgFunc)	WinListItem,	    // sq_item,      x[i]
    (PyIntIntArgFunc)	0,		    // sq_slice,     x[i:j]
    (PyIntObjArgProc)	0,		    // sq_ass_item,  x[i]=v
    (PyIntIntObjArgProc) 0,		    // sq_ass_slice, x[i:j]=v
    (objobjproc)	0,
#ଯଦି PY_MAJOR_VERSION >= 2
    (binaryfunc)	0,
    0,
#endif
};

// External interface

    ଶୂନ୍ୟ
python_buffer_free(buf_T *buf)
{
    BufferObject *bp = BUF_PYTHON_REF(buf);
    ଯଦି (bp == ଖାଲି)
	ଫେରନ୍ତୁ;
    bp->buf = INVALID_BUFFER_ମୂଲ୍ୟ;
    BUF_PYTHON_REF(buf) = ଖାଲି;
}

    ଶୂନ୍ୟ
python_window_free(win_T *win)
{
    WindowObject *wp = WIN_PYTHON_REF(win);
    ଯଦି (wp == ଖାଲି)
	ଫେରନ୍ତୁ;
    wp->win = INVALID_WINDOW_ମୂଲ୍ୟ;
    WIN_PYTHON_REF(win) = ଖାଲି;
}

    ଶୂନ୍ୟ
python_tabpage_free(tabpage_T *tab)
{
    TabPageObject *tp = TAB_PYTHON_REF(tab);
    ଯଦି (tp == ଖାଲି)
	ଫେରନ୍ତୁ;
    tp->tab = INVALID_TABPAGE_ମୂଲ୍ୟ;
    TAB_PYTHON_REF(tab) = ଖାଲି;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
PythonMod_Init(ଶୂନ୍ୟ)
{
    // The special value is removed from sys.path in Python_Init().
    ସ୍ଥିର ଅକ୍ଷର	*(argv[2]) = {"/must>not&exist/foo", ଖାଲି};

    ଯଦି (init_types())
	ଫେରନ୍ତୁ -1;

    // Set sys.argv[] to avoid a crash in warn().
    PySys_SetArgv(1, argv);

    vim_module = Py_InitModule4("vim", VimMethods, (ଅକ୍ଷର *)ଖାଲି,
				(PyObject *)ଖାଲି, PYTHON_API_VERSION);

    ଯଦି (populate_module(vim_module))
	ଫେରନ୍ତୁ -1;

    ଯଦି (init_sys_path())
	ଫେରନ୍ତୁ -1;

    ଫେରନ୍ତୁ 0;
}

//////////////////////////////////////////////////////////////////////////
// 4. Utility functions for handling the interface between Vim and Python.

// Convert a Vim line into a Python string.
// All internal newlines are replaced by null characters.
//
// On errors, the Python exception data is set, and NULL is returned.
    ସ୍ଥିର PyObject *
ଧାଡ଼ି_To_String(ଅଚଳ ଅକ୍ଷର *str)
{
    PyObject *result;
    PyInt len = strlen(str);
    ଅକ୍ଷର *p;

    // Allocate a Python string object, with uninitialised contents. We
    // must do it this way, so that we can modify the string in place
    // later. See the Python source, Objects/stringobject.c for details.
    result = PyString_FromStringAndSize(ଖାଲି, len);
    ଯଦି (result == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    p = PyString_AsString(result);

    ଯେତେବେଳେ (*str)
    {
	ଯଦି (*str == '\n')
	    *p = '\0';
	ଅନ୍ୟଥା
	    *p = *str;

	++p;
	++str;
    }

    ଫେରନ୍ତୁ result;
}

    ସ୍ଥିର PyObject *
DictionaryGetattr(PyObject *self, ଅକ୍ଷର *name)
{
    DictionaryObject	*this = ((DictionaryObject *) (self));

    ଯଦି (strcmp(name, "locked") == 0)
	ଫେରନ୍ତୁ PyInt_FromLong(this->dict->dv_lock);
    ଅନ୍ୟଥା ଯଦି (strcmp(name, "scope") == 0)
	ଫେରନ୍ତୁ PyInt_FromLong(this->dict->dv_scope);
    ଅନ୍ୟଥା ଯଦି (strcmp(name, "__members__") == 0)
	ଫେରନ୍ତୁ ObjectDir(ଖାଲି, DictionaryAttrs);

    ଫେରନ୍ତୁ Py_FindMethod(DictionaryMethods, self, name);
}

    ସ୍ଥିର PyObject *
ListGetattr(PyObject *self, ଅକ୍ଷର *name)
{
    ଯଦି (strcmp(name, "locked") == 0)
	ଫେରନ୍ତୁ PyInt_FromLong(((ListObject *)(self))->list->lv_lock);
    ଅନ୍ୟଥା ଯଦି (strcmp(name, "__members__") == 0)
	ଫେରନ୍ତୁ ObjectDir(ଖାଲି, ListAttrs);

    ଫେରନ୍ତୁ Py_FindMethod(ListMethods, self, name);
}

    ସ୍ଥିର PyObject *
TupleGetattr(PyObject *self, ଅକ୍ଷର *name)
{
    ଯଦି (strcmp(name, "locked") == 0)
	ଫେରନ୍ତୁ PyInt_FromLong(((TupleObject *)(self))->tuple->tv_lock);
    ଅନ୍ୟଥା ଯଦି (strcmp(name, "__members__") == 0)
	ଫେରନ୍ତୁ ObjectDir(ଖାଲି, TupleAttrs);

    ଫେରନ୍ତୁ Py_FindMethod(TupleMethods, self, name);
}

    ସ୍ଥିର PyObject *
FunctionGetattr(PyObject *self, ଅକ୍ଷର *name)
{
    PyObject	*r;

    r = FunctionAttr((FunctionObject *)(self), name);

    ଯଦି (r || PyErr_Occurred())
	ଫେରନ୍ତୁ r;
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ Py_FindMethod(FunctionMethods, self, name);
}

    ଶୂନ୍ୟ
do_pyeval(char_u *str, dict_T *locals, typval_T *rettv)
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
set_ref_in_python(ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID)
{
    ଫେରନ୍ତୁ set_ref_in_py(copyID);
}
