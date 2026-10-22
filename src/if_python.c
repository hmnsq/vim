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

#അന്തര്ഭാവ "vim.h"

#അന്തര്ഭാവ <limits.h>

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

#എങ്കിൽ defined(MSWIN) && defined(HAVE_FCNTL_H)
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

#അന്തര്ഭാവ <Python.h>

#എങ്കിൽ !defined(PY_VERSION_HEX) || PY_VERSION_HEX < 0x02050000
# undef PY_SSIZE_T_CLEAN
#endif

// these are NULL for Python 2
#define ERRORS_DECODE_ARG ശൂന്യ
#define ERRORS_ENCODE_ARG ERRORS_DECODE_ARG

#undef പ്രധാന // Defined in python.h - aargh
#undef HAVE_FCNTL_H // Clash with os_win32.h

// Perhaps leave this out for Python 2.6, which supports bytes?
#define PyBytes_FromString      PyString_FromString
#define PyBytes_Check		PyString_Check
#define PyBytes_AsStringAndSize PyString_AsStringAndSize
#define PyBytes_FromStringAndSize   PyString_FromStringAndSize

#എങ്കിൽ defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02070000
# define PY_USE_CAPSULE
#endif

#എങ്കിൽ defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02050000
# define PyInt Py_ssize_t
# define PyInquiry lenfunc
# define PyIntArgFunc ssizeargfunc
# define PyIntIntArgFunc ssizessizeargfunc
# define PyIntObjArgProc ssizeobjargproc
# define PyIntIntObjArgProc ssizessizeobjargproc
# define Py_ssize_t_fmt "n"
#അല്ലെങ്കിൽ
# define PyInt പൂർണ്ണസംഖ്യ
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
#define single_നിവേശം	256
#define file_നിവേശം	257
#define eval_നിവേശം	258

#എങ്കിൽ defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x020300F0
  // Python 2.3: can invoke ":python" recursively.
# define PY_CAN_RECURSE
#endif

#എങ്കിൽ defined(DYNAMIC_PYTHON)
# ifndef DYNAMIC_PYTHON
#  define HINSTANCE long_u		// for generating prototypes
# endif

# ifdef MSWIN
#  define load_dll vimLoadLib
#  define close_dll FreeLibrary
#  define ചിഹ്നം_from_dll GetProcAddress
#  define load_dll_error GetWin32Error
# അല്ലെങ്കിൽ
#  include <dlfcn.h>
#  define FARPROC ശൂന്യം*
#  define HINSTANCE ശൂന്യം*
#  എങ്കിൽ defined(PY_NO_RTLD_GLOBAL) && defined(PY3_NO_RTLD_GLOBAL)
#   define load_dll(n) dlopen((n), RTLD_LAZY)
#  അല്ലെങ്കിൽ
#   define load_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#  endif
#  define close_dll dlclose
#  define ചിഹ്നം_from_dll dlsym
#  define load_dll_error dlerror
# endif

// This makes if_python.c compile without warnings against Python 2.5
// on Win32 and Win64.
# undef PyRun_SimpleString
# undef PyRun_String
# undef PyArg_Parse
# undef PyArg_ParseTuple
# undef Py_Build_മൂല്യം
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
# define Py_List_തിരുകുക dll_Py_List_തിരുകുക
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
# ifdef Py_Mapping_താക്കോലുകൾ
#  define PY_NO_MAPPING_താക്കോലുകൾ
# അല്ലെങ്കിൽ
#  define Py_Mapping_താക്കോലുകൾ dll_Py_Mapping_താക്കോലുകൾ
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
# define Py_സംഖ്യ_Check dll_Py_സംഖ്യ_Check
# define Py_സംഖ്യ_Long dll_Py_സംഖ്യ_Long
# define PyImport_AddModule (*dll_PyImport_AddModule)
# define PySys_SetObject dll_PySys_SetObject
# define PySys_GetObject dll_PySys_GetObject
# define PySys_SetArgv dll_PySys_SetArgv
# define PyType_Type (*dll_PyType_Type)
# define PyFile_Type (*dll_PyFile_Type)
# define PySlice_Type (*dll_PySlice_Type)
# define PyType_Ready (*dll_PyType_Ready)
# define PyType_GenericAlloc dll_PyType_GenericAlloc
# define Py_Build_മൂല്യം dll_Py_Build_മൂല്യം
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
# അല്ലെങ്കിൽ
#  define PyObject_GC_Del dll_PyObject_GC_Del
#  define PyObject_GC_UnTrack dll_PyObject_GC_UnTrack
# endif
# എങ്കിൽ defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02070000
#  define _PyObject_NextNotImplemented (*dll__PyObject_NextNotImplemented)
# endif
# define _Py_NoneStruct (*dll__Py_NoneStruct)
# define _Py_ZeroStruct (*dll__Py_ZeroStruct)
# define _Py_TrueStruct (*dll__Py_TrueStruct)
# define PyObject_Init dll__PyObject_Init
# define PyObject_GetIter dll_PyObject_GetIter
# define PyObject_IsTrue dll_PyObject_IsTrue
# എങ്കിൽ defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02020000
#  define PyType_IsSubtype dll_PyType_IsSubtype
#  ifdef Py_DEBUG
#   define _Py_NegativeRefcount dll__Py_NegativeRefcount
#   define _Py_RefTotal (*dll__Py_RefTotal)
#   define _Py_Dealloc dll__Py_Dealloc
#  endif
# endif
# എങ്കിൽ defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02030000
#  എങ്കിൽ defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
#   define _PyObject_DebugMalloc dll__PyObject_DebugMalloc
#   define _PyObject_DebugFree dll__PyObject_DebugFree
#  അല്ലെങ്കിൽ
#   define PyObject_Malloc dll_PyObject_Malloc
#   define PyObject_Free dll_PyObject_Free
#  endif
# endif
# ifdef PY_USE_CAPSULE
#  define PyCapsule_New dll_PyCapsule_New
#  define PyCapsule_GetPointer dll_PyCapsule_GetPointer
# അല്ലെങ്കിൽ
#  define PyCObject_FromVoidPtr dll_PyCObject_FromVoidPtr
#  define PyCObject_AsVoidPtr dll_PyCObject_AsVoidPtr
# endif
# എങ്കിൽ defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02070000
#  define Py_NoSiteFlag (*dll_Py_NoSiteFlag)
# endif

/*
 * Pointers for dynamic link
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ(*dll_PyArg_Parse)(PyObject *, അക്ഷരം *, ...);
സ്ഥിതം പൂർണ്ണസംഖ്യ(*dll_PyArg_ParseTuple)(PyObject *, അക്ഷരം *, ...);
സ്ഥിതം ശൂന്യം(*dll_PyMem_Free)(ശൂന്യം *);
സ്ഥിതം ശൂന്യം* (*dll_PyMem_Malloc)(size_t);
സ്ഥിതം പൂർണ്ണസംഖ്യ(*dll_PyDict_SetItemString)(PyObject *dp, അക്ഷരം *താക്കോൽ, PyObject *item);
സ്ഥിതം പൂർണ്ണസംഖ്യ(*dll_PyErr_BadArgument)(ശൂന്യം);
സ്ഥിതം PyObject *(*dll_PyErr_NewException)(അക്ഷരം *, PyObject *, PyObject *);
സ്ഥിതം ശൂന്യം(*dll_PyErr_Clear)(ശൂന്യം);
സ്ഥിതം PyObject*(*dll_PyErr_Format)(PyObject *, സ്ഥിരം അക്ഷരം *, ...);
സ്ഥിതം ശൂന്യം(*dll_PyErr_PrintEx)(പൂർണ്ണസംഖ്യ);
സ്ഥിതം PyObject*(*dll_PyErr_NoMemory)(ശൂന്യം);
സ്ഥിതം PyObject*(*dll_PyErr_Occurred)(ശൂന്യം);
സ്ഥിതം ശൂന്യം(*dll_PyErr_SetNone)(PyObject *);
സ്ഥിതം ശൂന്യം(*dll_PyErr_SetString)(PyObject *, സ്ഥിരം അക്ഷരം *);
സ്ഥിതം ശൂന്യം(*dll_PyErr_SetObject)(PyObject *, PyObject *);
സ്ഥിതം പൂർണ്ണസംഖ്യ(*dll_PyErr_ExceptionMatches)(PyObject *);
സ്ഥിതം ശൂന്യം(*dll_PyEval_InitThreads)(ശൂന്യം);
സ്ഥിതം ശൂന്യം(*dll_PyEval_RestoreThread)(PyThreadState *);
സ്ഥിതം PyThreadState*(*dll_PyEval_SaveThread)(ശൂന്യം);
# ifdef PY_CAN_RECURSE
സ്ഥിതം PyGILState_STATE	(*dll_PyGILState_Ensure)(ശൂന്യം);
സ്ഥിതം ശൂന്യം (*dll_PyGILState_Release)(PyGILState_STATE);
# endif
സ്ഥിതം long(*dll_PyInt_AsLong)(PyObject *);
സ്ഥിതം PyObject*(*dll_PyInt_FromLong)(long);
സ്ഥിതം long(*dll_PyLong_AsLong)(PyObject *);
സ്ഥിതം PyObject*(*dll_PyLong_FromLong)(long);
സ്ഥിതം PyTypeObject* dll_PyBool_Type;
സ്ഥിതം PyTypeObject* dll_PyInt_Type;
സ്ഥിതം PyTypeObject* dll_PyLong_Type;
സ്ഥിതം PyObject*(*dll_PyList_GetItem)(PyObject *, PyInt);
സ്ഥിതം പൂർണ്ണസംഖ്യ(*dll_PyList_Append)(PyObject *, PyObject *);
സ്ഥിതം പൂർണ്ണസംഖ്യ(*dll_Py_List_തിരുകുക)(PyObject *, PyInt, PyObject *);
സ്ഥിതം PyObject*(*dll_PyList_New)(PyInt size);
സ്ഥിതം പൂർണ്ണസംഖ്യ(*dll_PyList_SetItem)(PyObject *, PyInt, PyObject *);
സ്ഥിതം PyInt(*dll_PyList_Size)(PyObject *);
സ്ഥിതം PyTypeObject* dll_PyList_Type;
സ്ഥിതം PyObject*(*dll_PyTuple_GetItem)(PyObject *, PyInt);
സ്ഥിതം പൂർണ്ണസംഖ്യ(*dll_PyTuple_SetItem)(PyObject *, PyInt, PyObject *);
സ്ഥിതം PyObject*(*dll_PyTuple_New)(PyInt size);
സ്ഥിതം PyInt(*dll_PyTuple_Size)(PyObject *);
സ്ഥിതം PyTypeObject* dll_PyTuple_Type;
സ്ഥിതം പൂർണ്ണസംഖ്യ (*dll_PySequence_Check)(PyObject *);
സ്ഥിതം PyInt(*dll_PySequence_Size)(PyObject *);
സ്ഥിതം PyObject*(*dll_PySequence_GetItem)(PyObject *, PyInt);
സ്ഥിതം PyObject*(*dll_PySequence_Fast)(PyObject *, സ്ഥിരം അക്ഷരം *);
സ്ഥിതം പൂർണ്ണസംഖ്യ (*dll_PySlice_GetIndicesEx)(PySliceObject *r, PyInt length,
		     PyInt *start, PyInt *stop, PyInt *step,
		     PyInt *slicelen);
സ്ഥിതം PyObject*(*dll_PyImport_ImportModule)(സ്ഥിരം അക്ഷരം *);
സ്ഥിതം PyObject*(*dll_PyDict_New)(ശൂന്യം);
സ്ഥിതം PyObject*(*dll_PyDict_GetItemString)(PyObject *, സ്ഥിരം അക്ഷരം *);
സ്ഥിതം പൂർണ്ണസംഖ്യ (*dll_PyDict_Next)(PyObject *, PyInt *, PyObject **, PyObject **);
സ്ഥിതം PyTypeObject* dll_PyDict_Type;
# ifndef PY_NO_MAPPING_താക്കോലുകൾ
സ്ഥിതം PyObject* (*dll_Py_Mapping_താക്കോലുകൾ)(PyObject *);
# endif
സ്ഥിതം PyObject* (*dll_PyObject_GetItem)(PyObject *, PyObject *);
സ്ഥിതം PyObject* (*dll_PyObject_CallMethod)(PyObject *, അക്ഷരം *, PyObject *);
സ്ഥിതം പൂർണ്ണസംഖ്യ (*dll_PyMapping_Check)(PyObject *);
സ്ഥിതം PyObject* (*dll_PyIter_Next)(PyObject *);
സ്ഥിതം PyObject*(*dll_PyModule_GetDict)(PyObject *);
സ്ഥിതം പൂർണ്ണസംഖ്യ(*dll_PyModule_AddObject)(PyObject *, സ്ഥിരം അക്ഷരം *, PyObject *);
സ്ഥിതം പൂർണ്ണസംഖ്യ(*dll_PyRun_SimpleString)(അക്ഷരം *);
സ്ഥിതം PyObject *(*dll_PyRun_String)(അക്ഷരം *, പൂർണ്ണസംഖ്യ, PyObject *, PyObject *);
സ്ഥിതം PyObject* (*dll_PyObject_GetAttrString)(PyObject *, സ്ഥിരം അക്ഷരം *);
സ്ഥിതം പൂർണ്ണസംഖ്യ (*dll_PyObject_HasAttrString)(PyObject *, സ്ഥിരം അക്ഷരം *);
സ്ഥിതം പൂർണ്ണസംഖ്യ (*dll_PyObject_SetAttrString)(PyObject *, സ്ഥിരം അക്ഷരം *, PyObject *);
സ്ഥിതം PyObject* (*dll_PyObject_CallFunctionObjArgs)(PyObject *, ...);
സ്ഥിതം PyObject* (*dll_PyObject_CallFunction)(PyObject *, അക്ഷരം *, ...);
സ്ഥിതം PyObject* (*dll_PyObject_Call)(PyObject *, PyObject *, PyObject *);
സ്ഥിതം PyObject* (*dll_PyObject_Repr)(PyObject *);
സ്ഥിതം അക്ഷരം*(*dll_PyString_AsString)(PyObject *);
സ്ഥിതം പൂർണ്ണസംഖ്യ(*dll_PyString_AsStringAndSize)(PyObject *, അക്ഷരം **, PyInt *);
സ്ഥിതം PyObject*(*dll_PyString_FromString)(സ്ഥിരം അക്ഷരം *);
സ്ഥിതം PyObject*(*dll_PyString_FromFormat)(സ്ഥിരം അക്ഷരം *, ...);
സ്ഥിതം PyObject*(*dll_PyString_FromStringAndSize)(സ്ഥിരം അക്ഷരം *, PyInt);
സ്ഥിതം PyInt(*dll_PyString_Size)(PyObject *);
സ്ഥിതം PyTypeObject* dll_PyString_Type;
സ്ഥിതം PyTypeObject* dll_PyUnicode_Type;
സ്ഥിതം PyObject *(*py_PyUnicode_AsEncodedString)(PyObject *, അക്ഷരം *, അക്ഷരം *);
സ്ഥിതം ഇരട്ട(*dll_PyFloat_AsDouble)(PyObject *);
സ്ഥിതം PyObject*(*dll_PyFloat_FromDouble)(ഇരട്ട);
സ്ഥിതം PyTypeObject* dll_PyFloat_Type;
സ്ഥിതം പൂർണ്ണസംഖ്യ(*dll_Py_സംഖ്യ_Check)(PyObject *);
സ്ഥിതം PyObject*(*dll_Py_സംഖ്യ_Long)(PyObject *);
സ്ഥിതം പൂർണ്ണസംഖ്യ(*dll_PySys_SetObject)(അക്ഷരം *, PyObject *);
സ്ഥിതം PyObject *(*dll_PySys_GetObject)(അക്ഷരം *);
സ്ഥിതം പൂർണ്ണസംഖ്യ(*dll_PySys_SetArgv)(പൂർണ്ണസംഖ്യ, അക്ഷരം **);
സ്ഥിതം PyTypeObject* dll_PyType_Type;
സ്ഥിതം PyTypeObject* dll_PyFile_Type;
സ്ഥിതം PyTypeObject* dll_PySlice_Type;
സ്ഥിതം പൂർണ്ണസംഖ്യ (*dll_PyType_Ready)(PyTypeObject *type);
സ്ഥിതം PyObject* (*dll_PyType_GenericAlloc)(PyTypeObject *type, PyInt nitems);
സ്ഥിതം PyObject*(*dll_Py_Build_മൂല്യം)(അക്ഷരം *, ...);
സ്ഥിതം PyObject*(*dll_Py_FindMethod)(ഘടന PyMethodDef[], PyObject *, അക്ഷരം *);
സ്ഥിതം PyObject*(*dll_Py_InitModule4)(അക്ഷരം *, ഘടന PyMethodDef *, അക്ഷരം *, PyObject *, പൂർണ്ണസംഖ്യ);
സ്ഥിതം PyObject*(*dll_PyImport_AddModule)(അക്ഷരം *);
സ്ഥിതം ശൂന്യം(*dll_Py_SetPythonHome)(അക്ഷരം *home);
സ്ഥിതം ശൂന്യം(*dll_Py_Initialize)(ശൂന്യം);
സ്ഥിതം ശൂന്യം(*dll_Py_Finalize)(ശൂന്യം);
സ്ഥിതം പൂർണ്ണസംഖ്യ(*dll_Py_IsInitialized)(ശൂന്യം);
സ്ഥിതം PyObject*(*dll__PyObject_New)(PyTypeObject *, PyObject *);
സ്ഥിതം PyObject*(*dll__PyObject_GC_New)(PyTypeObject *);
# ifdef Py_underscore_GC
സ്ഥിതം ശൂന്യം(*dll__PyObject_GC_Del)(ശൂന്യം *);
സ്ഥിതം ശൂന്യം(*dll__PyObject_GC_UnTrack)(ശൂന്യം *);
# അല്ലെങ്കിൽ
സ്ഥിതം ശൂന്യം(*dll_PyObject_GC_Del)(ശൂന്യം *);
സ്ഥിതം ശൂന്യം(*dll_PyObject_GC_UnTrack)(ശൂന്യം *);
# endif
സ്ഥിതം PyObject*(*dll__PyObject_Init)(PyObject *, PyTypeObject *);
സ്ഥിതം PyObject* (*dll_PyObject_GetIter)(PyObject *);
സ്ഥിതം പൂർണ്ണസംഖ്യ (*dll_PyObject_IsTrue)(PyObject *);
# എങ്കിൽ defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02070000
സ്ഥിതം iternextfunc dll__PyObject_NextNotImplemented;
# endif
സ്ഥിതം PyObject* dll__Py_NoneStruct;
സ്ഥിതം PyObject* _Py_ZeroStruct;
സ്ഥിതം PyObject* dll__Py_TrueStruct;
# എങ്കിൽ defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02020000
സ്ഥിതം പൂർണ്ണസംഖ്യ (*dll_PyType_IsSubtype)(PyTypeObject *, PyTypeObject *);
#  ifdef Py_DEBUG
സ്ഥിതം ശൂന്യം (*dll__Py_NegativeRefcount)(സ്ഥിരം അക്ഷരം *fname, പൂർണ്ണസംഖ്യ lineno, PyObject *op);
സ്ഥിതം PyInt* dll__Py_RefTotal;
സ്ഥിതം ശൂന്യം (*dll__Py_Dealloc)(PyObject *obj);
#  endif
# endif
# എങ്കിൽ defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02030000
#  എങ്കിൽ defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
സ്ഥിതം ശൂന്യം (*dll__PyObject_DebugFree)(ശൂന്യം*);
സ്ഥിതം ശൂന്യം* (*dll__PyObject_DebugMalloc)(size_t);
#  അല്ലെങ്കിൽ
സ്ഥിതം ശൂന്യം* (*dll_PyObject_Malloc)(size_t);
സ്ഥിതം ശൂന്യം (*dll_PyObject_Free)(ശൂന്യം*);
#  endif
# endif
# ifdef PY_USE_CAPSULE
സ്ഥിതം PyObject* (*dll_PyCapsule_New)(ശൂന്യം *, അക്ഷരം *, PyCapsule_Destructor);
സ്ഥിതം ശൂന്യം* (*dll_PyCapsule_GetPointer)(PyObject *, അക്ഷരം *);
# അല്ലെങ്കിൽ
സ്ഥിതം PyObject* (*dll_PyCObject_FromVoidPtr)(ശൂന്യം *cobj, ശൂന്യം (*destr)(ശൂന്യം *));
സ്ഥിതം ശൂന്യം* (*dll_PyCObject_AsVoidPtr)(PyObject *);
# endif
# എങ്കിൽ defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02070000
സ്ഥിതം പൂർണ്ണസംഖ്യ* dll_Py_NoSiteFlag;
# endif

സ്ഥിതം HINSTANCE hinstPython = 0; // Instance of python.dll

// Imported exception objects
സ്ഥിതം PyObject *imp_PyExc_AttributeError;
സ്ഥിതം PyObject *imp_PyExc_IndexError;
സ്ഥിതം PyObject *imp_Py_Exc_താക്കോൽ_Error;
സ്ഥിതം PyObject *imp_PyExc_KeyboardInterrupt;
സ്ഥിതം PyObject *imp_PyExc_TypeError;
സ്ഥിതം PyObject *imp_Py_Exc_മൂല്യം_Error;
സ്ഥിതം PyObject *imp_PyExc_SystemExit;
സ്ഥിതം PyObject *imp_PyExc_RuntimeError;
സ്ഥിതം PyObject *imp_PyExc_ImportError;
സ്ഥിതം PyObject *imp_PyExc_OverflowError;

# define PyExc_AttributeError imp_PyExc_AttributeError
# define PyExc_IndexError imp_PyExc_IndexError
# define Py_Exc_താക്കോൽ_Error imp_Py_Exc_താക്കോൽ_Error
# define PyExc_KeyboardInterrupt imp_PyExc_KeyboardInterrupt
# define PyExc_TypeError imp_PyExc_TypeError
# define Py_Exc_മൂല്യം_Error imp_Py_Exc_മൂല്യം_Error
# define PyExc_SystemExit imp_PyExc_SystemExit
# define PyExc_RuntimeError imp_PyExc_RuntimeError
# define PyExc_ImportError imp_PyExc_ImportError
# define PyExc_OverflowError imp_PyExc_OverflowError

/*
 * Table of name to function pointer of python.
 */
# define PYTHON_PROC FARPROC
സ്ഥിതം ഘടന
{
    അക്ഷരം *name;
    PYTHON_PROC *ptr;
} python_funcname_table[] =
{
# ifdef PY_SSIZE_T_CLEAN
    {"_PyArg_Parse_SizeT", (PYTHON_PROC*)&dll_PyArg_Parse},
    {"_PyArg_ParseTuple_SizeT", (PYTHON_PROC*)&dll_PyArg_ParseTuple},
    {"_Py_BuildValue_SizeT", (PYTHON_PROC*)&dll_Py_Build_മൂല്യം},
# അല്ലെങ്കിൽ
    {"PyArg_Parse", (PYTHON_PROC*)&dll_PyArg_Parse},
    {"PyArg_ParseTuple", (PYTHON_PROC*)&dll_PyArg_ParseTuple},
    {"Py_BuildValue", (PYTHON_PROC*)&dll_Py_Build_മൂല്യം},
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
    {"PyList_Insert", (PYTHON_PROC*)&dll_Py_List_തിരുകുക},
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
# ifndef PY_NO_MAPPING_താക്കോലുകൾ
    {"PyMapping_Keys", (PYTHON_PROC*)&dll_Py_Mapping_താക്കോലുകൾ},
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
    {"PyNumber_Check", (PYTHON_PROC*)&dll_Py_സംഖ്യ_Check},
    {"PyNumber_Long", (PYTHON_PROC*)&dll_Py_സംഖ്യ_Long},
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
# അല്ലെങ്കിൽ
    {"PyObject_GC_Del", (PYTHON_PROC*)&dll_PyObject_GC_Del},
    {"PyObject_GC_UnTrack", (PYTHON_PROC*)&dll_PyObject_GC_UnTrack},
# endif
    {"PyObject_Init", (PYTHON_PROC*)&dll__PyObject_Init},
    {"PyObject_GetIter", (PYTHON_PROC*)&dll_PyObject_GetIter},
    {"PyObject_IsTrue", (PYTHON_PROC*)&dll_PyObject_IsTrue},
# എങ്കിൽ defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02070000
    {"_PyObject_NextNotImplemented", (PYTHON_PROC*)&dll__PyObject_NextNotImplemented},
# endif
    {"_Py_NoneStruct", (PYTHON_PROC*)&dll__Py_NoneStruct},
    {"_Py_ZeroStruct", (PYTHON_PROC*)&dll__Py_ZeroStruct},
    {"_Py_TrueStruct", (PYTHON_PROC*)&dll__Py_TrueStruct},
# എങ്കിൽ defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02020000
#  ifdef Py_DEBUG
    {"_Py_NegativeRefcount", (PYTHON_PROC*)&dll__Py_NegativeRefcount},
    {"_Py_RefTotal", (PYTHON_PROC*)&dll__Py_RefTotal},
    {"_Py_Dealloc", (PYTHON_PROC*)&dll__Py_Dealloc},
#  endif
    {"PyType_IsSubtype", (PYTHON_PROC*)&dll_PyType_IsSubtype},
# endif
# എങ്കിൽ defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02030000
#  എങ്കിൽ defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
    {"_PyObject_DebugFree", (PYTHON_PROC*)&dll__PyObject_DebugFree},
    {"_PyObject_DebugMalloc", (PYTHON_PROC*)&dll__PyObject_DebugMalloc},
#  അല്ലെങ്കിൽ
    {"PyObject_Malloc", (PYTHON_PROC*)&dll_PyObject_Malloc},
    {"PyObject_Free", (PYTHON_PROC*)&dll_PyObject_Free},
#  endif
# endif
# എങ്കിൽ defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02050000 \
	&& SIZEOF_SIZE_T != VIM_SIZEOF_INT
#  ifdef Py_DEBUG
    {"Py_InitModule4TraceRefs_64", (PYTHON_PROC*)&dll_Py_InitModule4},
#  അല്ലെങ്കിൽ
    {"Py_InitModule4_64", (PYTHON_PROC*)&dll_Py_InitModule4},
#  endif
# അല്ലെങ്കിൽ
#  ifdef Py_DEBUG
    {"Py_InitModule4TraceRefs", (PYTHON_PROC*)&dll_Py_InitModule4},
#  അല്ലെങ്കിൽ
    {"Py_InitModule4", (PYTHON_PROC*)&dll_Py_InitModule4},
#  endif
# endif
# ifdef PY_USE_CAPSULE
    {"PyCapsule_New", (PYTHON_PROC*)&dll_PyCapsule_New},
    {"PyCapsule_GetPointer", (PYTHON_PROC*)&dll_PyCapsule_GetPointer},
# അല്ലെങ്കിൽ
    {"PyCObject_FromVoidPtr", (PYTHON_PROC*)&dll_PyCObject_FromVoidPtr},
    {"PyCObject_AsVoidPtr", (PYTHON_PROC*)&dll_PyCObject_AsVoidPtr},
# endif
# എങ്കിൽ defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02070000
    {"Py_NoSiteFlag", (PYTHON_PROC*)&dll_Py_NoSiteFlag},
# endif
    {"", ശൂന്യ},
};

/*
 * Load library and get all pointers.
 * Parameter 'libname' provides name of DLL.
 * Return OK or FAIL.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
python_runtime_link_init(അക്ഷരം *libname, പൂർണ്ണസംഖ്യ verbose)
{
    പൂർണ്ണസംഖ്യ i;
    PYTHON_PROC *ucs_as_encoded_string =
				   (PYTHON_PROC*)&py_PyUnicode_AsEncodedString;

# എങ്കിൽ !(defined(PY_NO_RTLD_GLOBAL) && defined(PY3_NO_RTLD_GLOBAL)) && defined(UNIX) && defined(FEAT_PYTHON3)
    // Can't have Python and Python3 loaded at the same time.
    // It causes a crash, because RTLD_GLOBAL is needed for
    // standard C extension libraries of one or both python versions.
    എങ്കിൽ (python3_loaded())
    {
	എങ്കിൽ (verbose)
	    emsg(_(e_this_vim_cannot_execute_python_after_using_py3));
	തിരികെ FAIL;
    }
# endif

    എങ്കിൽ (hinstPython)
	തിരികെ OK;
    hinstPython = load_dll(libname);
    എങ്കിൽ (!hinstPython)
    {
	എങ്കിൽ (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	തിരികെ FAIL;
    }

    വേണ്ടി (i = 0; python_funcname_table[i].ptr; ++i)
    {
	എങ്കിൽ ((*python_funcname_table[i].ptr = ചിഹ്നം_from_dll(hinstPython,
			python_funcname_table[i].name)) == ശൂന്യ)
	{
	    close_dll(hinstPython);
	    hinstPython = 0;
	    എങ്കിൽ (verbose)
		semsg(_(e_could_not_load_library_function_str), python_funcname_table[i].name);
	    തിരികെ FAIL;
	}
    }

    // Load unicode functions separately as only the ucs2 or the ucs4 functions
    // will be present in the library.
    *ucs_as_encoded_string = ചിഹ്നം_from_dll(hinstPython,
					     "PyUnicodeUCS2_AsEncodedString");
    എങ്കിൽ (*ucs_as_encoded_string == ശൂന്യ)
	*ucs_as_encoded_string = ചിഹ്നം_from_dll(hinstPython,
					     "PyUnicodeUCS4_AsEncodedString");
    എങ്കിൽ (*ucs_as_encoded_string == ശൂന്യ)
    {
	close_dll(hinstPython);
	hinstPython = 0;
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
python_enabled(പൂർണ്ണസംഖ്യ verbose)
{
    തിരികെ python_runtime_link_init((അക്ഷരം *)p_pydll, verbose) == OK;
}

/*
 * Load the standard Python exceptions - don't import the symbols from the
 * DLL, as this can cause errors (importing data symbols is not reliable).
 */
    സ്ഥിതം ശൂന്യം
get_exceptions(ശൂന്യം)
{
    PyObject *exmod = PyImport_ImportModule("exceptions");
    PyObject *exdict = PyModule_GetDict(exmod);
    imp_PyExc_AttributeError = PyDict_GetItemString(exdict, "AttributeError");
    imp_PyExc_IndexError = PyDict_GetItemString(exdict, "IndexError");
    imp_Py_Exc_താക്കോൽ_Error = PyDict_GetItemString(exdict, "KeyError");
    imp_PyExc_KeyboardInterrupt = PyDict_GetItemString(exdict, "KeyboardInterrupt");
    imp_PyExc_TypeError = PyDict_GetItemString(exdict, "TypeError");
    imp_Py_Exc_മൂല്യം_Error = PyDict_GetItemString(exdict, "ValueError");
    imp_PyExc_SystemExit = PyDict_GetItemString(exdict, "SystemExit");
    imp_PyExc_RuntimeError = PyDict_GetItemString(exdict, "RuntimeError");
    imp_PyExc_ImportError = PyDict_GetItemString(exdict, "ImportError");
    imp_PyExc_OverflowError = PyDict_GetItemString(exdict, "OverflowError");
    Py_XINCREF(imp_PyExc_AttributeError);
    Py_XINCREF(imp_PyExc_IndexError);
    Py_XINCREF(imp_Py_Exc_താക്കോൽ_Error);
    Py_XINCREF(imp_PyExc_KeyboardInterrupt);
    Py_XINCREF(imp_PyExc_TypeError);
    Py_XINCREF(imp_Py_Exc_മൂല്യം_Error);
    Py_XINCREF(imp_PyExc_SystemExit);
    Py_XINCREF(imp_PyExc_RuntimeError);
    Py_XINCREF(imp_PyExc_ImportError);
    Py_XINCREF(imp_PyExc_OverflowError);
    Py_XDECREF(exmod);
}
#endif // DYNAMIC_PYTHON

സ്ഥിതം പൂർണ്ണസംഖ്യ initialised = 0;
#define PYINITIALISED initialised
സ്ഥിതം പൂർണ്ണസംഖ്യ python_end_called = FALSE;

#define DESTRUCTOR_FINISH(self) self->ob_type->tp_free((PyObject*)self);

#define WIN_PYTHON_REF(win) win->w_python_ref
#define BUF_PYTHON_REF(buf) buf->b_python_ref
#define TAB_PYTHON_REF(tab) tab->tp_python_ref

സ്ഥിതം PyObject *OutputGetattr(PyObject *, അക്ഷരം *);
സ്ഥിതം PyObject *BufferGetattr(PyObject *, അക്ഷരം *);
സ്ഥിതം PyObject *WindowGetattr(PyObject *, അക്ഷരം *);
സ്ഥിതം PyObject *TabPageGetattr(PyObject *, അക്ഷരം *);
സ്ഥിതം PyObject *RangeGetattr(PyObject *, അക്ഷരം *);
സ്ഥിതം PyObject *DictionaryGetattr(PyObject *, അക്ഷരം*);
സ്ഥിതം PyObject *ListGetattr(PyObject *, അക്ഷരം *);
സ്ഥിതം PyObject *TupleGetattr(PyObject *, അക്ഷരം *);
സ്ഥിതം PyObject *FunctionGetattr(PyObject *, അക്ഷരം *);

#ifndef Py_VISIT
# define Py_VISIT(obj) visit(obj, arg)
#endif
#ifndef Py_CLEAR
# define Py_CLEAR(obj) \
    { \
	Py_XDECREF(obj); \
	obj = ശൂന്യ; \
    }
#endif

#എങ്കിൽ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    സ്ഥിതം ശൂന്യം *
py_memsave(ശൂന്യം *p, size_t len)
{
    ശൂന്യം	*r;

    എങ്കിൽ (!(r = PyMem_Malloc(len)))
	തിരികെ ശൂന്യ;
    mch_memmove(r, p, len);
    തിരികെ r;
}

# define PY_STRSAVE(s) ((char_u *) py_memsave(s, STRLEN(s) + 1))
#endif

തരനിർവചനം PySliceObject PySliceObject_T;

/*
 * Include the code shared with if_python3.c
 */
#അന്തര്ഭാവ "if_py_both.h"


///////////////////////////////////////////////////////
// Internal function prototypes.

സ്ഥിതം പൂർണ്ണസംഖ്യ PythonMod_Init(ശൂന്യം);


///////////////////////////////////////////////////////
// 1. Python interpreter main program.

#ifndef PY_CAN_RECURSE
സ്ഥിതം PyThreadState *saved_python_thread = ശൂന്യ;

/*
 * Suspend a thread of the Python interpreter, other threads are allowed to
 * run.
 */
    സ്ഥിതം ശൂന്യം
Python_SaveThread(ശൂന്യം)
{
    saved_python_thread = PyEval_SaveThread();
}

/*
 * Restore a thread of the Python interpreter, waits for other threads to
 * block.
 */
    സ്ഥിതം ശൂന്യം
Python_RestoreThread(ശൂന്യം)
{
    PyEval_RestoreThread(saved_python_thread);
    saved_python_thread = ശൂന്യ;
}
#endif

    ശൂന്യം
python_end(ശൂന്യം)
{
    സ്ഥിതം പൂർണ്ണസംഖ്യ recurse = 0;

    // If a crash occurs while doing this, don't try again.
    എങ്കിൽ (recurse != 0)
	തിരികെ;

    python_end_called = TRUE;
    ++recurse;

#ifdef DYNAMIC_PYTHON
    എങ്കിൽ (hinstPython && Py_IsInitialized())
    {
# ifdef PY_CAN_RECURSE
	PyGILState_Ensure();
# അല്ലെങ്കിൽ
	Python_RestoreThread();	    // enter python
# endif
	Py_Finalize();
    }
#അല്ലെങ്കിൽ
    എങ്കിൽ (Py_IsInitialized())
    {
# ifdef PY_CAN_RECURSE
	PyGILState_Ensure();
# അല്ലെങ്കിൽ
	Python_RestoreThread();	    // enter python
# endif
	Py_Finalize();
    }
#endif

    --recurse;
}

#എങ്കിൽ defined(DYNAMIC_PYTHON) && defined(FEAT_PYTHON3)
    പൂർണ്ണസംഖ്യ
python_loaded(ശൂന്യം)
{
    തിരികെ (hinstPython != 0);
}
#endif

സ്ഥിതം അക്ഷരം *py_home_buf = ശൂന്യ;

    സ്ഥിതം പൂർണ്ണസംഖ്യ
Python_Init(ശൂന്യം)
{
    എങ്കിൽ (!initialised)
    {
#എങ്കിൽ defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02070000
	PyObject *site;
#endif

#ifdef DYNAMIC_PYTHON
	എങ്കിൽ (!python_enabled(TRUE))
	{
	    emsg(_(e_sorry_this_command_is_disabled_python_library_could_not_be_found));
	    പോകുക fail;
	}
#endif

	എങ്കിൽ (*p_pyhome != NUL)
	{
	    // The string must not change later, make a copy in static memory.
	    py_home_buf = (അക്ഷരം *)vim_strsave(p_pyhome);
	    എങ്കിൽ (py_home_buf != ശൂന്യ)
		Py_SetPythonHome(py_home_buf);
	}
#ifdef PYTHON_HOME
	അല്ലെങ്കിൽ എങ്കിൽ (mch_getenv((char_u *)"PYTHONHOME") == ശൂന്യ)
	    Py_SetPythonHome(PYTHON_HOME);
#endif

	init_structs();

#എങ്കിൽ defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02070000
	// Disable implicit 'import site', because it may cause Vim to exit
	// when it can't be found.
	Py_NoSiteFlag++;
#endif

	Py_Initialize();

#എങ്കിൽ defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02070000
	// 'import site' explicitly.
	site = PyImport_ImportModule("site");
	എങ്കിൽ (site == ശൂന്യ)
	{
	    emsg(_(e_sorry_this_command_is_disabled_python_site_module_could_not_be_loaded));
	    പോകുക fail;
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

	എങ്കിൽ (PythonIO_Init_io())
	    പോകുക fail;

	എങ്കിൽ (PythonMod_Init())
	    പോകുക fail;

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
DoPyCommand(
    സ്ഥിരം അക്ഷരം		*cmd,
    dict_T*		locals,
    rangeinitializer	init_range,
    runner		run,
    ശൂന്യം		*arg)
{
#ifndef PY_CAN_RECURSE
    സ്ഥിതം പൂർണ്ണസംഖ്യ		recursive = 0;
#endif
#എങ്കിൽ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    അക്ഷരം		*saved_locale;
#endif
#ifdef PY_CAN_RECURSE
    PyGILState_STATE	pygilstate;
#endif

#ifndef PY_CAN_RECURSE
    എങ്കിൽ (recursive)
    {
	emsg(_(e_cannot_invoke_python_recursively));
	തിരികെ;
    }
    ++recursive;
#endif
    എങ്കിൽ (python_end_called)
	തിരികെ;

    എങ്കിൽ (Python_Init())
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
	saved_locale = (അക്ഷരം *) PY_STRSAVE(saved_locale);
	(ശൂന്യം)setlocale(LC_NUMERIC, "C");
    }
#endif

#ifdef PY_CAN_RECURSE
    pygilstate = PyGILState_Ensure();
#അല്ലെങ്കിൽ
    Python_RestoreThread();	    // enter python
#endif

    run((അക്ഷരം *) cmd, locals, arg
#ifdef PY_CAN_RECURSE
	    , &pygilstate
#endif
	    );

#ifdef PY_CAN_RECURSE
    PyGILState_Release(pygilstate);
#അല്ലെങ്കിൽ
    Python_SaveThread();	    // leave python
#endif

#എങ്കിൽ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    എങ്കിൽ (saved_locale != ശൂന്യ)
    {
	(ശൂന്യം)setlocale(LC_NUMERIC, saved_locale);
	PyMem_Free(saved_locale);
    }
#endif

    Python_Lock_Vim();		    // enter vim
    PythonIO_Flush();

theend:
#ifndef PY_CAN_RECURSE
    --recursive;
#endif
    തിരികെ;
}

/*
 * ":python"
 */
    ശൂന്യം
ex_python(exarg_T *eap)
{
    char_u *script;

    script = script_get(eap, eap->arg);
    എങ്കിൽ (!eap->skip)
    {
	എങ്കിൽ (p_pyx == 0)
	    p_pyx = 2;

	DoPyCommand(script == ശൂന്യ ? (അക്ഷരം *) eap->arg : (അക്ഷരം *) script,
		ശൂന്യ,
		init_range_cmd,
		(runner) run_cmd,
		(ശൂന്യം *) eap);
    }
    vim_free(script);
}

#define BUFFER_SIZE 1024

/*
 * ":pyfile"
 */
    ശൂന്യം
ex_pyfile(exarg_T *eap)
{
    സ്ഥിതം അക്ഷരം buffer[BUFFER_SIZE];
    സ്ഥിരം അക്ഷരം *file = (അക്ഷരം *)eap->arg;
    അക്ഷരം *p;

    എങ്കിൽ (p_pyx == 0)
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

    ആകുമ്പോൾ (*file && p < buffer + (BUFFER_SIZE - 3))
    {
	എങ്കിൽ (*file == '\\' || *file == '\'')
	    *p++ = '\\';
	*p++ = *file++;
    }

    // If we didn't finish the file name, we hit a buffer overflow
    എങ്കിൽ (*file != '\0')
	തിരികെ;

    // Put in the terminating "')" and a null
    *p++ = '\'';
    *p++ = ')';
    *p++ = '\0';

    // Execute the file
    DoPyCommand(buffer,
	    ശൂന്യ,
	    init_range_cmd,
	    (runner) run_cmd,
	    (ശൂന്യം *) eap);
}

    ശൂന്യം
ex_pydo(exarg_T *eap)
{
    എങ്കിൽ (p_pyx == 0)
	p_pyx = 2;

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
OutputGetattr(PyObject *self, അക്ഷരം *name)
{
    എങ്കിൽ (strcmp(name, "softspace") == 0)
	തിരികെ PyInt_FromLong(((OutputObject *)(self))->softspace);
    അല്ലെങ്കിൽ എങ്കിൽ (strcmp(name, "__members__") == 0)
	തിരികെ ObjectDir(ശൂന്യ, OutputAttrs);
    അല്ലെങ്കിൽ എങ്കിൽ (strcmp(name, "errors") == 0)
	തിരികെ PyString_FromString("strict");
    അല്ലെങ്കിൽ എങ്കിൽ (strcmp(name, "encoding") == 0)
	തിരികെ PyString_FromString(ENC_OPT);
    തിരികെ Py_FindMethod(OutputMethods, self, name);
}

///////////////////////////////////////////////////////
// 3. Implementation of the Vim module for Python

// Window type - Implementation functions
// --------------------------------------

#define WindowType_Check(obj) ((obj)->ob_type == &WindowType)

// Buffer type - Implementation functions
// --------------------------------------

#define BufferType_Check(obj) ((obj)->ob_type == &BufferType)

സ്ഥിതം പൂർണ്ണസംഖ്യ BufferAssItem(PyObject *, PyInt, PyObject *);
സ്ഥിതം പൂർണ്ണസംഖ്യ BufferAssSlice(PyObject *, PyInt, PyInt, PyObject *);

// Line range type - Implementation functions
// --------------------------------------

#define RangeType_Check(obj) ((obj)->ob_type == &RangeType)

സ്ഥിതം പൂർണ്ണസംഖ്യ RangeAssItem(PyObject *, PyInt, PyObject *);
സ്ഥിതം പൂർണ്ണസംഖ്യ RangeAssSlice(PyObject *, PyInt, PyInt, PyObject *);

// Current objects type - Implementation functions
// -----------------------------------------------

സ്ഥിതം PySequenceMethods BufferAsSeq = {
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

    സ്ഥിതം PyObject *
BufferGetattr(PyObject *self, അക്ഷരം *name)
{
    PyObject *r;

    എങ്കിൽ ((r = BufferAttrValid((BufferObject *)(self), name)))
	തിരികെ r;

    എങ്കിൽ (CheckBuffer((BufferObject *)(self)))
	തിരികെ ശൂന്യ;

    r = BufferAttr((BufferObject *)(self), name);
    എങ്കിൽ (r || PyErr_Occurred())
	തിരികെ r;
    അല്ലെങ്കിൽ
	തിരികെ Py_FindMethod(BufferMethods, self, name);
}

//////////////////

    സ്ഥിതം പൂർണ്ണസംഖ്യ
BufferAssItem(PyObject *self, PyInt n, PyObject *val)
{
    തിരികെ RBAsItem((BufferObject *)(self), n, val, 1, -1, ശൂന്യ);
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
BufferAssSlice(PyObject *self, PyInt lo, PyInt hi, PyObject *val)
{
    തിരികെ RBAsSlice((BufferObject *)(self), lo, hi, val, 1, -1, ശൂന്യ);
}

സ്ഥിതം PySequenceMethods RangeAsSeq = {
    (PyInquiry)		RangeLength,	      // sq_length,    len(x)
    (binaryfunc)	0, /* RangeConcat, */ // sq_concat,    x+y
    (PyIntArgFunc)	0, /* RangeRepeat, */ // sq_repeat,    x*n
    (PyIntArgFunc)	RangeItem,	      // sq_item,      x[i]
    (PyIntIntArgFunc)	RangeSlice,	      // sq_slice,     x[i:j]
    (PyIntObjArgProc)	RangeAssItem,	      // sq_ass_item,  x[i]=v
    (PyIntIntObjArgProc) RangeAssSlice,	      // sq_ass_slice, x[i:j]=v
    (objobjproc)	0,
#എങ്കിൽ PY_MAJOR_VERSION >= 2
    (binaryfunc)	0,
    0,
#endif
};

// Line range object - Implementation

    സ്ഥിതം PyObject *
RangeGetattr(PyObject *self, അക്ഷരം *name)
{
    എങ്കിൽ (strcmp(name, "start") == 0)
	തിരികെ Py_Build_മൂല്യം(Py_ssize_t_fmt, ((RangeObject *)(self))->start - 1);
    അല്ലെങ്കിൽ എങ്കിൽ (strcmp(name, "end") == 0)
	തിരികെ Py_Build_മൂല്യം(Py_ssize_t_fmt, ((RangeObject *)(self))->end - 1);
    അല്ലെങ്കിൽ എങ്കിൽ (strcmp(name, "__members__") == 0)
	തിരികെ ObjectDir(ശൂന്യ, RangeAttrs);
    അല്ലെങ്കിൽ
	തിരികെ Py_FindMethod(RangeMethods, self, name);
}

////////////////

    സ്ഥിതം പൂർണ്ണസംഖ്യ
RangeAssItem(PyObject *self, PyInt n, PyObject *val)
{
    തിരികെ RBAsItem(((RangeObject *)(self))->buf, n, val,
		     ((RangeObject *)(self))->start,
		     ((RangeObject *)(self))->end,
		     &((RangeObject *)(self))->end);
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
RangeAssSlice(PyObject *self, PyInt lo, PyInt hi, PyObject *val)
{
    തിരികെ RBAsSlice(((RangeObject *)(self))->buf, lo, hi, val,
		      ((RangeObject *)(self))->start,
		      ((RangeObject *)(self))->end,
		      &((RangeObject *)(self))->end);
}

// TabPage object - Implementation

    സ്ഥിതം PyObject *
TabPageGetattr(PyObject *self, അക്ഷരം *name)
{
    PyObject *r;

    എങ്കിൽ ((r = TabPageAttrValid((TabPageObject *)(self), name)))
	തിരികെ r;

    എങ്കിൽ (CheckTabPage((TabPageObject *)(self)))
	തിരികെ ശൂന്യ;

    r = TabPageAttr((TabPageObject *)(self), name);
    എങ്കിൽ (r || PyErr_Occurred())
	തിരികെ r;
    അല്ലെങ്കിൽ
	തിരികെ Py_FindMethod(TabPageMethods, self, name);
}

// Window object - Implementation

    സ്ഥിതം PyObject *
WindowGetattr(PyObject *self, അക്ഷരം *name)
{
    PyObject *r;

    എങ്കിൽ ((r = WindowAttrValid((WindowObject *)(self), name)))
	തിരികെ r;

    എങ്കിൽ (CheckWindow((WindowObject *)(self)))
	തിരികെ ശൂന്യ;

    r = WindowAttr((WindowObject *)(self), name);
    എങ്കിൽ (r || PyErr_Occurred())
	തിരികെ r;
    അല്ലെങ്കിൽ
	തിരികെ Py_FindMethod(WindowMethods, self, name);
}

// Tab page list object - Definitions

സ്ഥിതം PySequenceMethods TabListAsSeq = {
    (PyInquiry)		TabListLength,	    // sq_length,    len(x)
    (binaryfunc)	0,		    // sq_concat,    x+y
    (PyIntArgFunc)	0,		    // sq_repeat,    x*n
    (PyIntArgFunc)	TabListItem,	    // sq_item,      x[i]
    (PyIntIntArgFunc)	0,		    // sq_slice,     x[i:j]
    (PyIntObjArgProc)	0,		    // sq_ass_item,  x[i]=v
    (PyIntIntObjArgProc) 0,		    // sq_ass_slice, x[i:j]=v
    (objobjproc)	0,
#എങ്കിൽ PY_MAJOR_VERSION >= 2
    (binaryfunc)	0,
    0,
#endif
};

// Window list object - Definitions

സ്ഥിതം PySequenceMethods WinListAsSeq = {
    (PyInquiry)		WinListLength,	    // sq_length,    len(x)
    (binaryfunc)	0,		    // sq_concat,    x+y
    (PyIntArgFunc)	0,		    // sq_repeat,    x*n
    (PyIntArgFunc)	WinListItem,	    // sq_item,      x[i]
    (PyIntIntArgFunc)	0,		    // sq_slice,     x[i:j]
    (PyIntObjArgProc)	0,		    // sq_ass_item,  x[i]=v
    (PyIntIntObjArgProc) 0,		    // sq_ass_slice, x[i:j]=v
    (objobjproc)	0,
#എങ്കിൽ PY_MAJOR_VERSION >= 2
    (binaryfunc)	0,
    0,
#endif
};

// External interface

    ശൂന്യം
python_buffer_free(buf_T *buf)
{
    BufferObject *bp = BUF_PYTHON_REF(buf);
    എങ്കിൽ (bp == ശൂന്യ)
	തിരികെ;
    bp->buf = INVALID_BUFFER_മൂല്യം;
    BUF_PYTHON_REF(buf) = ശൂന്യ;
}

    ശൂന്യം
python_window_free(win_T *win)
{
    WindowObject *wp = WIN_PYTHON_REF(win);
    എങ്കിൽ (wp == ശൂന്യ)
	തിരികെ;
    wp->win = INVALID_WINDOW_മൂല്യം;
    WIN_PYTHON_REF(win) = ശൂന്യ;
}

    ശൂന്യം
python_tabpage_free(tabpage_T *tab)
{
    TabPageObject *tp = TAB_PYTHON_REF(tab);
    എങ്കിൽ (tp == ശൂന്യ)
	തിരികെ;
    tp->tab = INVALID_TABPAGE_മൂല്യം;
    TAB_PYTHON_REF(tab) = ശൂന്യ;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
PythonMod_Init(ശൂന്യം)
{
    // The special value is removed from sys.path in Python_Init().
    സ്ഥിതം അക്ഷരം	*(argv[2]) = {"/must>not&exist/foo", ശൂന്യ};

    എങ്കിൽ (init_types())
	തിരികെ -1;

    // Set sys.argv[] to avoid a crash in warn().
    PySys_SetArgv(1, argv);

    vim_module = Py_InitModule4("vim", VimMethods, (അക്ഷരം *)ശൂന്യ,
				(PyObject *)ശൂന്യ, PYTHON_API_VERSION);

    എങ്കിൽ (populate_module(vim_module))
	തിരികെ -1;

    എങ്കിൽ (init_sys_path())
	തിരികെ -1;

    തിരികെ 0;
}

//////////////////////////////////////////////////////////////////////////
// 4. Utility functions for handling the interface between Vim and Python.

// Convert a Vim line into a Python string.
// All internal newlines are replaced by null characters.
//
// On errors, the Python exception data is set, and NULL is returned.
    സ്ഥിതം PyObject *
വരി_To_String(സ്ഥിരം അക്ഷരം *str)
{
    PyObject *result;
    PyInt len = strlen(str);
    അക്ഷരം *p;

    // Allocate a Python string object, with uninitialised contents. We
    // must do it this way, so that we can modify the string in place
    // later. See the Python source, Objects/stringobject.c for details.
    result = PyString_FromStringAndSize(ശൂന്യ, len);
    എങ്കിൽ (result == ശൂന്യ)
	തിരികെ ശൂന്യ;

    p = PyString_AsString(result);

    ആകുമ്പോൾ (*str)
    {
	എങ്കിൽ (*str == '\n')
	    *p = '\0';
	അല്ലെങ്കിൽ
	    *p = *str;

	++p;
	++str;
    }

    തിരികെ result;
}

    സ്ഥിതം PyObject *
DictionaryGetattr(PyObject *self, അക്ഷരം *name)
{
    DictionaryObject	*this = ((DictionaryObject *) (self));

    എങ്കിൽ (strcmp(name, "locked") == 0)
	തിരികെ PyInt_FromLong(this->dict->dv_lock);
    അല്ലെങ്കിൽ എങ്കിൽ (strcmp(name, "scope") == 0)
	തിരികെ PyInt_FromLong(this->dict->dv_scope);
    അല്ലെങ്കിൽ എങ്കിൽ (strcmp(name, "__members__") == 0)
	തിരികെ ObjectDir(ശൂന്യ, DictionaryAttrs);

    തിരികെ Py_FindMethod(DictionaryMethods, self, name);
}

    സ്ഥിതം PyObject *
ListGetattr(PyObject *self, അക്ഷരം *name)
{
    എങ്കിൽ (strcmp(name, "locked") == 0)
	തിരികെ PyInt_FromLong(((ListObject *)(self))->list->lv_lock);
    അല്ലെങ്കിൽ എങ്കിൽ (strcmp(name, "__members__") == 0)
	തിരികെ ObjectDir(ശൂന്യ, ListAttrs);

    തിരികെ Py_FindMethod(ListMethods, self, name);
}

    സ്ഥിതം PyObject *
TupleGetattr(PyObject *self, അക്ഷരം *name)
{
    എങ്കിൽ (strcmp(name, "locked") == 0)
	തിരികെ PyInt_FromLong(((TupleObject *)(self))->tuple->tv_lock);
    അല്ലെങ്കിൽ എങ്കിൽ (strcmp(name, "__members__") == 0)
	തിരികെ ObjectDir(ശൂന്യ, TupleAttrs);

    തിരികെ Py_FindMethod(TupleMethods, self, name);
}

    സ്ഥിതം PyObject *
FunctionGetattr(PyObject *self, അക്ഷരം *name)
{
    PyObject	*r;

    r = FunctionAttr((FunctionObject *)(self), name);

    എങ്കിൽ (r || PyErr_Occurred())
	തിരികെ r;
    അല്ലെങ്കിൽ
	തിരികെ Py_FindMethod(FunctionMethods, self, name);
}

    ശൂന്യം
do_pyeval(char_u *str, dict_T *locals, typval_T *rettv)
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
set_ref_in_python(പൂർണ്ണസംഖ്യ copyID)
{
    തിരികെ set_ref_in_py(copyID);
}
