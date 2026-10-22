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

#অন্তর্ভাৱ "vim.h"

#অন্তর্ভাৱ <limits.h>

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

#যদি defined(MSWIN) && defined(HAVE_FCNTL_H)
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

#অন্তর্ভাৱ <Python.h>

#যদি !defined(PY_VERSION_HEX) || PY_VERSION_HEX < 0x02050000
# undef PY_SSIZE_T_CLEAN
#endif

// these are NULL for Python 2
#define ERRORS_DECODE_ARG রিক্ত
#define ERRORS_ENCODE_ARG ERRORS_DECODE_ARG

#undef main // Defined in python.h - aargh
#undef HAVE_FCNTL_H // Clash with os_win32.h

// Perhaps leave this out for Python 2.6, which supports bytes?
#define PyBytes_FromString      PyString_FromString
#define PyBytes_Check		PyString_Check
#define PyBytes_AsStringAndSize PyString_AsStringAndSize
#define PyBytes_FromStringAndSize   PyString_FromStringAndSize

#যদি defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02070000
# define PY_USE_CAPSULE
#endif

#যদি defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02050000
# define PyInt Py_ssize_t
# define PyInquiry lenfunc
# define PyIntArgFunc ssizeargfunc
# define PyIntIntArgFunc ssizessizeargfunc
# define PyIntObjArgProc ssizeobjargproc
# define PyIntIntObjArgProc ssizessizeobjargproc
# define Py_ssize_t_fmt "n"
#অন্যথা
# define PyInt পূর্ণাঙ্ক
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
#define single_প্ৰবেশ	256
#define file_প্ৰবেশ	257
#define eval_প্ৰবেশ	258

#যদি defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x020300F0
  // Python 2.3: can invoke ":python" recursively.
# define PY_CAN_RECURSE
#endif

#যদি defined(DYNAMIC_PYTHON)
# ifndef DYNAMIC_PYTHON
#  define HINSTANCE long_u		// for generating prototypes
# endif

# ifdef MSWIN
#  define load_dll vimLoadLib
#  define close_dll FreeLibrary
#  define প্ৰতীক_from_dll GetProcAddress
#  define load_dll_error GetWin32Error
# অন্যথা
#  include <dlfcn.h>
#  define FARPROC শূন্য*
#  define HINSTANCE শূন্য*
#  যদি defined(PY_NO_RTLD_GLOBAL) && defined(PY3_NO_RTLD_GLOBAL)
#   define load_dll(n) dlopen((n), RTLD_LAZY)
#  অন্যথা
#   define load_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#  endif
#  define close_dll dlclose
#  define প্ৰতীক_from_dll dlsym
#  define load_dll_error dlerror
# endif

// This makes if_python.c compile without warnings against Python 2.5
// on Win32 and Win64.
# undef PyRun_SimpleString
# undef PyRun_String
# undef PyArg_Parse
# undef PyArg_ParseTuple
# undef Py_Build_মান
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
# define Py_List_ঢোকাও dll_Py_List_ঢোকাও
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
# ifdef Py_Mapping_চাবিগুলি
#  define PY_NO_MAPPING_চাবিগুলি
# অন্যথা
#  define Py_Mapping_চাবিগুলি dll_Py_Mapping_চাবিগুলি
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
# define Py_সংখ্যা_Check dll_Py_সংখ্যা_Check
# define Py_সংখ্যা_Long dll_Py_সংখ্যা_Long
# define PyImport_AddModule (*dll_PyImport_AddModule)
# define PySys_SetObject dll_PySys_SetObject
# define PySys_GetObject dll_PySys_GetObject
# define PySys_SetArgv dll_PySys_SetArgv
# define PyType_Type (*dll_PyType_Type)
# define PyFile_Type (*dll_PyFile_Type)
# define PySlice_Type (*dll_PySlice_Type)
# define PyType_Ready (*dll_PyType_Ready)
# define PyType_GenericAlloc dll_PyType_GenericAlloc
# define Py_Build_মান dll_Py_Build_মান
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
# অন্যথা
#  define PyObject_GC_Del dll_PyObject_GC_Del
#  define PyObject_GC_UnTrack dll_PyObject_GC_UnTrack
# endif
# যদি defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02070000
#  define _PyObject_NextNotImplemented (*dll__PyObject_NextNotImplemented)
# endif
# define _Py_NoneStruct (*dll__Py_NoneStruct)
# define _Py_ZeroStruct (*dll__Py_ZeroStruct)
# define _Py_TrueStruct (*dll__Py_TrueStruct)
# define PyObject_Init dll__PyObject_Init
# define PyObject_GetIter dll_PyObject_GetIter
# define PyObject_IsTrue dll_PyObject_IsTrue
# যদি defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02020000
#  define PyType_IsSubtype dll_PyType_IsSubtype
#  ifdef Py_DEBUG
#   define _Py_NegativeRefcount dll__Py_NegativeRefcount
#   define _Py_RefTotal (*dll__Py_RefTotal)
#   define _Py_Dealloc dll__Py_Dealloc
#  endif
# endif
# যদি defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02030000
#  যদি defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
#   define _PyObject_DebugMalloc dll__PyObject_DebugMalloc
#   define _PyObject_DebugFree dll__PyObject_DebugFree
#  অন্যথা
#   define PyObject_Malloc dll_PyObject_Malloc
#   define PyObject_Free dll_PyObject_Free
#  endif
# endif
# ifdef PY_USE_CAPSULE
#  define PyCapsule_New dll_PyCapsule_New
#  define PyCapsule_GetPointer dll_PyCapsule_GetPointer
# অন্যথা
#  define PyCObject_FromVoidPtr dll_PyCObject_FromVoidPtr
#  define PyCObject_AsVoidPtr dll_PyCObject_AsVoidPtr
# endif
# যদি defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02070000
#  define Py_NoSiteFlag (*dll_Py_NoSiteFlag)
# endif

/*
 * Pointers for dynamic link
 */
স্থির পূর্ণাঙ্ক(*dll_PyArg_Parse)(PyObject *, ৱর্ণ *, ...);
স্থির পূর্ণাঙ্ক(*dll_PyArg_ParseTuple)(PyObject *, ৱর্ণ *, ...);
স্থির শূন্য(*dll_PyMem_Free)(শূন্য *);
স্থির শূন্য* (*dll_PyMem_Malloc)(size_t);
স্থির পূর্ণাঙ্ক(*dll_PyDict_SetItemString)(PyObject *dp, ৱর্ণ *চাবি, PyObject *item);
স্থির পূর্ণাঙ্ক(*dll_PyErr_BadArgument)(শূন্য);
স্থির PyObject *(*dll_PyErr_NewException)(ৱর্ণ *, PyObject *, PyObject *);
স্থির শূন্য(*dll_PyErr_Clear)(শূন্য);
স্থির PyObject*(*dll_PyErr_Format)(PyObject *, ধ্রুৱ ৱর্ণ *, ...);
স্থির শূন্য(*dll_PyErr_PrintEx)(পূর্ণাঙ্ক);
স্থির PyObject*(*dll_PyErr_NoMemory)(শূন্য);
স্থির PyObject*(*dll_PyErr_Occurred)(শূন্য);
স্থির শূন্য(*dll_PyErr_SetNone)(PyObject *);
স্থির শূন্য(*dll_PyErr_SetString)(PyObject *, ধ্রুৱ ৱর্ণ *);
স্থির শূন্য(*dll_PyErr_SetObject)(PyObject *, PyObject *);
স্থির পূর্ণাঙ্ক(*dll_PyErr_ExceptionMatches)(PyObject *);
স্থির শূন্য(*dll_PyEval_InitThreads)(শূন্য);
স্থির শূন্য(*dll_PyEval_RestoreThread)(PyThreadState *);
স্থির PyThreadState*(*dll_PyEval_SaveThread)(শূন্য);
# ifdef PY_CAN_RECURSE
স্থির PyGILState_STATE	(*dll_PyGILState_Ensure)(শূন্য);
স্থির শূন্য (*dll_PyGILState_Release)(PyGILState_STATE);
# endif
স্থির long(*dll_PyInt_AsLong)(PyObject *);
স্থির PyObject*(*dll_PyInt_FromLong)(long);
স্থির long(*dll_PyLong_AsLong)(PyObject *);
স্থির PyObject*(*dll_PyLong_FromLong)(long);
স্থির PyTypeObject* dll_PyBool_Type;
স্থির PyTypeObject* dll_PyInt_Type;
স্থির PyTypeObject* dll_PyLong_Type;
স্থির PyObject*(*dll_PyList_GetItem)(PyObject *, PyInt);
স্থির পূর্ণাঙ্ক(*dll_PyList_Append)(PyObject *, PyObject *);
স্থির পূর্ণাঙ্ক(*dll_Py_List_ঢোকাও)(PyObject *, PyInt, PyObject *);
স্থির PyObject*(*dll_PyList_New)(PyInt size);
স্থির পূর্ণাঙ্ক(*dll_PyList_SetItem)(PyObject *, PyInt, PyObject *);
স্থির PyInt(*dll_PyList_Size)(PyObject *);
স্থির PyTypeObject* dll_PyList_Type;
স্থির PyObject*(*dll_PyTuple_GetItem)(PyObject *, PyInt);
স্থির পূর্ণাঙ্ক(*dll_PyTuple_SetItem)(PyObject *, PyInt, PyObject *);
স্থির PyObject*(*dll_PyTuple_New)(PyInt size);
স্থির PyInt(*dll_PyTuple_Size)(PyObject *);
স্থির PyTypeObject* dll_PyTuple_Type;
স্থির পূর্ণাঙ্ক (*dll_PySequence_Check)(PyObject *);
স্থির PyInt(*dll_PySequence_Size)(PyObject *);
স্থির PyObject*(*dll_PySequence_GetItem)(PyObject *, PyInt);
স্থির PyObject*(*dll_PySequence_Fast)(PyObject *, ধ্রুৱ ৱর্ণ *);
স্থির পূর্ণাঙ্ক (*dll_PySlice_GetIndicesEx)(PySliceObject *r, PyInt length,
		     PyInt *start, PyInt *stop, PyInt *step,
		     PyInt *slicelen);
স্থির PyObject*(*dll_PyImport_ImportModule)(ধ্রুৱ ৱর্ণ *);
স্থির PyObject*(*dll_PyDict_New)(শূন্য);
স্থির PyObject*(*dll_PyDict_GetItemString)(PyObject *, ধ্রুৱ ৱর্ণ *);
স্থির পূর্ণাঙ্ক (*dll_PyDict_Next)(PyObject *, PyInt *, PyObject **, PyObject **);
স্থির PyTypeObject* dll_PyDict_Type;
# ifndef PY_NO_MAPPING_চাবিগুলি
স্থির PyObject* (*dll_Py_Mapping_চাবিগুলি)(PyObject *);
# endif
স্থির PyObject* (*dll_PyObject_GetItem)(PyObject *, PyObject *);
স্থির PyObject* (*dll_PyObject_CallMethod)(PyObject *, ৱর্ণ *, PyObject *);
স্থির পূর্ণাঙ্ক (*dll_PyMapping_Check)(PyObject *);
স্থির PyObject* (*dll_PyIter_Next)(PyObject *);
স্থির PyObject*(*dll_PyModule_GetDict)(PyObject *);
স্থির পূর্ণাঙ্ক(*dll_PyModule_AddObject)(PyObject *, ধ্রুৱ ৱর্ণ *, PyObject *);
স্থির পূর্ণাঙ্ক(*dll_PyRun_SimpleString)(ৱর্ণ *);
স্থির PyObject *(*dll_PyRun_String)(ৱর্ণ *, পূর্ণাঙ্ক, PyObject *, PyObject *);
স্থির PyObject* (*dll_PyObject_GetAttrString)(PyObject *, ধ্রুৱ ৱর্ণ *);
স্থির পূর্ণাঙ্ক (*dll_PyObject_HasAttrString)(PyObject *, ধ্রুৱ ৱর্ণ *);
স্থির পূর্ণাঙ্ক (*dll_PyObject_SetAttrString)(PyObject *, ধ্রুৱ ৱর্ণ *, PyObject *);
স্থির PyObject* (*dll_PyObject_CallFunctionObjArgs)(PyObject *, ...);
স্থির PyObject* (*dll_PyObject_CallFunction)(PyObject *, ৱর্ণ *, ...);
স্থির PyObject* (*dll_PyObject_Call)(PyObject *, PyObject *, PyObject *);
স্থির PyObject* (*dll_PyObject_Repr)(PyObject *);
স্থির ৱর্ণ*(*dll_PyString_AsString)(PyObject *);
স্থির পূর্ণাঙ্ক(*dll_PyString_AsStringAndSize)(PyObject *, ৱর্ণ **, PyInt *);
স্থির PyObject*(*dll_PyString_FromString)(ধ্রুৱ ৱর্ণ *);
স্থির PyObject*(*dll_PyString_FromFormat)(ধ্রুৱ ৱর্ণ *, ...);
স্থির PyObject*(*dll_PyString_FromStringAndSize)(ধ্রুৱ ৱর্ণ *, PyInt);
স্থির PyInt(*dll_PyString_Size)(PyObject *);
স্থির PyTypeObject* dll_PyString_Type;
স্থির PyTypeObject* dll_PyUnicode_Type;
স্থির PyObject *(*py_PyUnicode_AsEncodedString)(PyObject *, ৱর্ণ *, ৱর্ণ *);
স্থির দ্ৱিগুণ(*dll_PyFloat_AsDouble)(PyObject *);
স্থির PyObject*(*dll_PyFloat_FromDouble)(দ্ৱিগুণ);
স্থির PyTypeObject* dll_PyFloat_Type;
স্থির পূর্ণাঙ্ক(*dll_Py_সংখ্যা_Check)(PyObject *);
স্থির PyObject*(*dll_Py_সংখ্যা_Long)(PyObject *);
স্থির পূর্ণাঙ্ক(*dll_PySys_SetObject)(ৱর্ণ *, PyObject *);
স্থির PyObject *(*dll_PySys_GetObject)(ৱর্ণ *);
স্থির পূর্ণাঙ্ক(*dll_PySys_SetArgv)(পূর্ণাঙ্ক, ৱর্ণ **);
স্থির PyTypeObject* dll_PyType_Type;
স্থির PyTypeObject* dll_PyFile_Type;
স্থির PyTypeObject* dll_PySlice_Type;
স্থির পূর্ণাঙ্ক (*dll_PyType_Ready)(PyTypeObject *type);
স্থির PyObject* (*dll_PyType_GenericAlloc)(PyTypeObject *type, PyInt nitems);
স্থির PyObject*(*dll_Py_Build_মান)(ৱর্ণ *, ...);
স্থির PyObject*(*dll_Py_FindMethod)(সংরচনা PyMethodDef[], PyObject *, ৱর্ণ *);
স্থির PyObject*(*dll_Py_InitModule4)(ৱর্ণ *, সংরচনা PyMethodDef *, ৱর্ণ *, PyObject *, পূর্ণাঙ্ক);
স্থির PyObject*(*dll_PyImport_AddModule)(ৱর্ণ *);
স্থির শূন্য(*dll_Py_SetPythonHome)(ৱর্ণ *home);
স্থির শূন্য(*dll_Py_Initialize)(শূন্য);
স্থির শূন্য(*dll_Py_Finalize)(শূন্য);
স্থির পূর্ণাঙ্ক(*dll_Py_IsInitialized)(শূন্য);
স্থির PyObject*(*dll__PyObject_New)(PyTypeObject *, PyObject *);
স্থির PyObject*(*dll__PyObject_GC_New)(PyTypeObject *);
# ifdef Py_underscore_GC
স্থির শূন্য(*dll__PyObject_GC_Del)(শূন্য *);
স্থির শূন্য(*dll__PyObject_GC_UnTrack)(শূন্য *);
# অন্যথা
স্থির শূন্য(*dll_PyObject_GC_Del)(শূন্য *);
স্থির শূন্য(*dll_PyObject_GC_UnTrack)(শূন্য *);
# endif
স্থির PyObject*(*dll__PyObject_Init)(PyObject *, PyTypeObject *);
স্থির PyObject* (*dll_PyObject_GetIter)(PyObject *);
স্থির পূর্ণাঙ্ক (*dll_PyObject_IsTrue)(PyObject *);
# যদি defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02070000
স্থির iternextfunc dll__PyObject_NextNotImplemented;
# endif
স্থির PyObject* dll__Py_NoneStruct;
স্থির PyObject* _Py_ZeroStruct;
স্থির PyObject* dll__Py_TrueStruct;
# যদি defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02020000
স্থির পূর্ণাঙ্ক (*dll_PyType_IsSubtype)(PyTypeObject *, PyTypeObject *);
#  ifdef Py_DEBUG
স্থির শূন্য (*dll__Py_NegativeRefcount)(ধ্রুৱ ৱর্ণ *fname, পূর্ণাঙ্ক lineno, PyObject *op);
স্থির PyInt* dll__Py_RefTotal;
স্থির শূন্য (*dll__Py_Dealloc)(PyObject *obj);
#  endif
# endif
# যদি defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02030000
#  যদি defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
স্থির শূন্য (*dll__PyObject_DebugFree)(শূন্য*);
স্থির শূন্য* (*dll__PyObject_DebugMalloc)(size_t);
#  অন্যথা
স্থির শূন্য* (*dll_PyObject_Malloc)(size_t);
স্থির শূন্য (*dll_PyObject_Free)(শূন্য*);
#  endif
# endif
# ifdef PY_USE_CAPSULE
স্থির PyObject* (*dll_PyCapsule_New)(শূন্য *, ৱর্ণ *, PyCapsule_Destructor);
স্থির শূন্য* (*dll_PyCapsule_GetPointer)(PyObject *, ৱর্ণ *);
# অন্যথা
স্থির PyObject* (*dll_PyCObject_FromVoidPtr)(শূন্য *cobj, শূন্য (*destr)(শূন্য *));
স্থির শূন্য* (*dll_PyCObject_AsVoidPtr)(PyObject *);
# endif
# যদি defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02070000
স্থির পূর্ণাঙ্ক* dll_Py_NoSiteFlag;
# endif

স্থির HINSTANCE hinstPython = 0; // Instance of python.dll

// Imported exception objects
স্থির PyObject *imp_PyExc_AttributeError;
স্থির PyObject *imp_PyExc_IndexError;
স্থির PyObject *imp_Py_Exc_চাবি_Error;
স্থির PyObject *imp_PyExc_KeyboardInterrupt;
স্থির PyObject *imp_PyExc_TypeError;
স্থির PyObject *imp_Py_Exc_মান_Error;
স্থির PyObject *imp_PyExc_SystemExit;
স্থির PyObject *imp_PyExc_RuntimeError;
স্থির PyObject *imp_PyExc_ImportError;
স্থির PyObject *imp_PyExc_OverflowError;

# define PyExc_AttributeError imp_PyExc_AttributeError
# define PyExc_IndexError imp_PyExc_IndexError
# define Py_Exc_চাবি_Error imp_Py_Exc_চাবি_Error
# define PyExc_KeyboardInterrupt imp_PyExc_KeyboardInterrupt
# define PyExc_TypeError imp_PyExc_TypeError
# define Py_Exc_মান_Error imp_Py_Exc_মান_Error
# define PyExc_SystemExit imp_PyExc_SystemExit
# define PyExc_RuntimeError imp_PyExc_RuntimeError
# define PyExc_ImportError imp_PyExc_ImportError
# define PyExc_OverflowError imp_PyExc_OverflowError

/*
 * Table of name to function pointer of python.
 */
# define PYTHON_PROC FARPROC
স্থির সংরচনা
{
    ৱর্ণ *name;
    PYTHON_PROC *ptr;
} python_funcname_table[] =
{
# ifdef PY_SSIZE_T_CLEAN
    {"_PyArg_Parse_SizeT", (PYTHON_PROC*)&dll_PyArg_Parse},
    {"_PyArg_ParseTuple_SizeT", (PYTHON_PROC*)&dll_PyArg_ParseTuple},
    {"_Py_BuildValue_SizeT", (PYTHON_PROC*)&dll_Py_Build_মান},
# অন্যথা
    {"PyArg_Parse", (PYTHON_PROC*)&dll_PyArg_Parse},
    {"PyArg_ParseTuple", (PYTHON_PROC*)&dll_PyArg_ParseTuple},
    {"Py_BuildValue", (PYTHON_PROC*)&dll_Py_Build_মান},
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
    {"PyList_Insert", (PYTHON_PROC*)&dll_Py_List_ঢোকাও},
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
# ifndef PY_NO_MAPPING_চাবিগুলি
    {"PyMapping_Keys", (PYTHON_PROC*)&dll_Py_Mapping_চাবিগুলি},
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
    {"PyNumber_Check", (PYTHON_PROC*)&dll_Py_সংখ্যা_Check},
    {"PyNumber_Long", (PYTHON_PROC*)&dll_Py_সংখ্যা_Long},
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
# অন্যথা
    {"PyObject_GC_Del", (PYTHON_PROC*)&dll_PyObject_GC_Del},
    {"PyObject_GC_UnTrack", (PYTHON_PROC*)&dll_PyObject_GC_UnTrack},
# endif
    {"PyObject_Init", (PYTHON_PROC*)&dll__PyObject_Init},
    {"PyObject_GetIter", (PYTHON_PROC*)&dll_PyObject_GetIter},
    {"PyObject_IsTrue", (PYTHON_PROC*)&dll_PyObject_IsTrue},
# যদি defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02070000
    {"_PyObject_NextNotImplemented", (PYTHON_PROC*)&dll__PyObject_NextNotImplemented},
# endif
    {"_Py_NoneStruct", (PYTHON_PROC*)&dll__Py_NoneStruct},
    {"_Py_ZeroStruct", (PYTHON_PROC*)&dll__Py_ZeroStruct},
    {"_Py_TrueStruct", (PYTHON_PROC*)&dll__Py_TrueStruct},
# যদি defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02020000
#  ifdef Py_DEBUG
    {"_Py_NegativeRefcount", (PYTHON_PROC*)&dll__Py_NegativeRefcount},
    {"_Py_RefTotal", (PYTHON_PROC*)&dll__Py_RefTotal},
    {"_Py_Dealloc", (PYTHON_PROC*)&dll__Py_Dealloc},
#  endif
    {"PyType_IsSubtype", (PYTHON_PROC*)&dll_PyType_IsSubtype},
# endif
# যদি defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02030000
#  যদি defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
    {"_PyObject_DebugFree", (PYTHON_PROC*)&dll__PyObject_DebugFree},
    {"_PyObject_DebugMalloc", (PYTHON_PROC*)&dll__PyObject_DebugMalloc},
#  অন্যথা
    {"PyObject_Malloc", (PYTHON_PROC*)&dll_PyObject_Malloc},
    {"PyObject_Free", (PYTHON_PROC*)&dll_PyObject_Free},
#  endif
# endif
# যদি defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02050000 \
	&& SIZEOF_SIZE_T != VIM_SIZEOF_INT
#  ifdef Py_DEBUG
    {"Py_InitModule4TraceRefs_64", (PYTHON_PROC*)&dll_Py_InitModule4},
#  অন্যথা
    {"Py_InitModule4_64", (PYTHON_PROC*)&dll_Py_InitModule4},
#  endif
# অন্যথা
#  ifdef Py_DEBUG
    {"Py_InitModule4TraceRefs", (PYTHON_PROC*)&dll_Py_InitModule4},
#  অন্যথা
    {"Py_InitModule4", (PYTHON_PROC*)&dll_Py_InitModule4},
#  endif
# endif
# ifdef PY_USE_CAPSULE
    {"PyCapsule_New", (PYTHON_PROC*)&dll_PyCapsule_New},
    {"PyCapsule_GetPointer", (PYTHON_PROC*)&dll_PyCapsule_GetPointer},
# অন্যথা
    {"PyCObject_FromVoidPtr", (PYTHON_PROC*)&dll_PyCObject_FromVoidPtr},
    {"PyCObject_AsVoidPtr", (PYTHON_PROC*)&dll_PyCObject_AsVoidPtr},
# endif
# যদি defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02070000
    {"Py_NoSiteFlag", (PYTHON_PROC*)&dll_Py_NoSiteFlag},
# endif
    {"", রিক্ত},
};

/*
 * Load library and get all pointers.
 * Parameter 'libname' provides name of DLL.
 * Return OK or FAIL.
 */
    স্থির পূর্ণাঙ্ক
python_runtime_link_init(ৱর্ণ *libname, পূর্ণাঙ্ক verbose)
{
    পূর্ণাঙ্ক i;
    PYTHON_PROC *ucs_as_encoded_string =
				   (PYTHON_PROC*)&py_PyUnicode_AsEncodedString;

# যদি !(defined(PY_NO_RTLD_GLOBAL) && defined(PY3_NO_RTLD_GLOBAL)) && defined(UNIX) && defined(FEAT_PYTHON3)
    // Can't have Python and Python3 loaded at the same time.
    // It causes a crash, because RTLD_GLOBAL is needed for
    // standard C extension libraries of one or both python versions.
    যদি (python3_loaded())
    {
	যদি (verbose)
	    emsg(_(e_this_vim_cannot_execute_python_after_using_py3));
	ঘূৰাই FAIL;
    }
# endif

    যদি (hinstPython)
	ঘূৰাই OK;
    hinstPython = load_dll(libname);
    যদি (!hinstPython)
    {
	যদি (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	ঘূৰাই FAIL;
    }

    বাবে (i = 0; python_funcname_table[i].ptr; ++i)
    {
	যদি ((*python_funcname_table[i].ptr = প্ৰতীক_from_dll(hinstPython,
			python_funcname_table[i].name)) == রিক্ত)
	{
	    close_dll(hinstPython);
	    hinstPython = 0;
	    যদি (verbose)
		semsg(_(e_could_not_load_library_function_str), python_funcname_table[i].name);
	    ঘূৰাই FAIL;
	}
    }

    // Load unicode functions separately as only the ucs2 or the ucs4 functions
    // will be present in the library.
    *ucs_as_encoded_string = প্ৰতীক_from_dll(hinstPython,
					     "PyUnicodeUCS2_AsEncodedString");
    যদি (*ucs_as_encoded_string == রিক্ত)
	*ucs_as_encoded_string = প্ৰতীক_from_dll(hinstPython,
					     "PyUnicodeUCS4_AsEncodedString");
    যদি (*ucs_as_encoded_string == রিক্ত)
    {
	close_dll(hinstPython);
	hinstPython = 0;
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
python_enabled(পূর্ণাঙ্ক verbose)
{
    ঘূৰাই python_runtime_link_init((ৱর্ণ *)p_pydll, verbose) == OK;
}

/*
 * Load the standard Python exceptions - don't import the symbols from the
 * DLL, as this can cause errors (importing data symbols is not reliable).
 */
    স্থির শূন্য
get_exceptions(শূন্য)
{
    PyObject *exmod = PyImport_ImportModule("exceptions");
    PyObject *exdict = PyModule_GetDict(exmod);
    imp_PyExc_AttributeError = PyDict_GetItemString(exdict, "AttributeError");
    imp_PyExc_IndexError = PyDict_GetItemString(exdict, "IndexError");
    imp_Py_Exc_চাবি_Error = PyDict_GetItemString(exdict, "KeyError");
    imp_PyExc_KeyboardInterrupt = PyDict_GetItemString(exdict, "KeyboardInterrupt");
    imp_PyExc_TypeError = PyDict_GetItemString(exdict, "TypeError");
    imp_Py_Exc_মান_Error = PyDict_GetItemString(exdict, "ValueError");
    imp_PyExc_SystemExit = PyDict_GetItemString(exdict, "SystemExit");
    imp_PyExc_RuntimeError = PyDict_GetItemString(exdict, "RuntimeError");
    imp_PyExc_ImportError = PyDict_GetItemString(exdict, "ImportError");
    imp_PyExc_OverflowError = PyDict_GetItemString(exdict, "OverflowError");
    Py_XINCREF(imp_PyExc_AttributeError);
    Py_XINCREF(imp_PyExc_IndexError);
    Py_XINCREF(imp_Py_Exc_চাবি_Error);
    Py_XINCREF(imp_PyExc_KeyboardInterrupt);
    Py_XINCREF(imp_PyExc_TypeError);
    Py_XINCREF(imp_Py_Exc_মান_Error);
    Py_XINCREF(imp_PyExc_SystemExit);
    Py_XINCREF(imp_PyExc_RuntimeError);
    Py_XINCREF(imp_PyExc_ImportError);
    Py_XINCREF(imp_PyExc_OverflowError);
    Py_XDECREF(exmod);
}
#endif // DYNAMIC_PYTHON

স্থির পূর্ণাঙ্ক initialised = 0;
#define PYINITIALISED initialised
স্থির পূর্ণাঙ্ক python_end_called = FALSE;

#define DESTRUCTOR_FINISH(self) self->ob_type->tp_free((PyObject*)self);

#define WIN_PYTHON_REF(win) win->w_python_ref
#define BUF_PYTHON_REF(buf) buf->b_python_ref
#define TAB_PYTHON_REF(tab) tab->tp_python_ref

স্থির PyObject *OutputGetattr(PyObject *, ৱর্ণ *);
স্থির PyObject *BufferGetattr(PyObject *, ৱর্ণ *);
স্থির PyObject *WindowGetattr(PyObject *, ৱর্ণ *);
স্থির PyObject *TabPageGetattr(PyObject *, ৱর্ণ *);
স্থির PyObject *RangeGetattr(PyObject *, ৱর্ণ *);
স্থির PyObject *DictionaryGetattr(PyObject *, ৱর্ণ*);
স্থির PyObject *ListGetattr(PyObject *, ৱর্ণ *);
স্থির PyObject *TupleGetattr(PyObject *, ৱর্ণ *);
স্থির PyObject *FunctionGetattr(PyObject *, ৱর্ণ *);

#ifndef Py_VISIT
# define Py_VISIT(obj) visit(obj, arg)
#endif
#ifndef Py_CLEAR
# define Py_CLEAR(obj) \
    { \
	Py_XDECREF(obj); \
	obj = রিক্ত; \
    }
#endif

#যদি defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    স্থির শূন্য *
py_memsave(শূন্য *p, size_t len)
{
    শূন্য	*r;

    যদি (!(r = PyMem_Malloc(len)))
	ঘূৰাই রিক্ত;
    mch_memmove(r, p, len);
    ঘূৰাই r;
}

# define PY_STRSAVE(s) ((char_u *) py_memsave(s, STRLEN(s) + 1))
#endif

প্রকারনির্দেশ PySliceObject PySliceObject_T;

/*
 * Include the code shared with if_python3.c
 */
#অন্তর্ভাৱ "if_py_both.h"


///////////////////////////////////////////////////////
// Internal function prototypes.

স্থির পূর্ণাঙ্ক PythonMod_Init(শূন্য);


///////////////////////////////////////////////////////
// 1. Python interpreter main program.

#ifndef PY_CAN_RECURSE
স্থির PyThreadState *saved_python_thread = রিক্ত;

/*
 * Suspend a thread of the Python interpreter, other threads are allowed to
 * run.
 */
    স্থির শূন্য
Python_SaveThread(শূন্য)
{
    saved_python_thread = PyEval_SaveThread();
}

/*
 * Restore a thread of the Python interpreter, waits for other threads to
 * block.
 */
    স্থির শূন্য
Python_RestoreThread(শূন্য)
{
    PyEval_RestoreThread(saved_python_thread);
    saved_python_thread = রিক্ত;
}
#endif

    শূন্য
python_end(শূন্য)
{
    স্থির পূর্ণাঙ্ক recurse = 0;

    // If a crash occurs while doing this, don't try again.
    যদি (recurse != 0)
	ঘূৰাই;

    python_end_called = TRUE;
    ++recurse;

#ifdef DYNAMIC_PYTHON
    যদি (hinstPython && Py_IsInitialized())
    {
# ifdef PY_CAN_RECURSE
	PyGILState_Ensure();
# অন্যথা
	Python_RestoreThread();	    // enter python
# endif
	Py_Finalize();
    }
#অন্যথা
    যদি (Py_IsInitialized())
    {
# ifdef PY_CAN_RECURSE
	PyGILState_Ensure();
# অন্যথা
	Python_RestoreThread();	    // enter python
# endif
	Py_Finalize();
    }
#endif

    --recurse;
}

#যদি defined(DYNAMIC_PYTHON) && defined(FEAT_PYTHON3)
    পূর্ণাঙ্ক
python_loaded(শূন্য)
{
    ঘূৰাই (hinstPython != 0);
}
#endif

স্থির ৱর্ণ *py_home_buf = রিক্ত;

    স্থির পূর্ণাঙ্ক
Python_Init(শূন্য)
{
    যদি (!initialised)
    {
#যদি defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02070000
	PyObject *site;
#endif

#ifdef DYNAMIC_PYTHON
	যদি (!python_enabled(TRUE))
	{
	    emsg(_(e_sorry_this_command_is_disabled_python_library_could_not_be_found));
	    গচ্ছ fail;
	}
#endif

	যদি (*p_pyhome != NUL)
	{
	    // The string must not change later, make a copy in static memory.
	    py_home_buf = (ৱর্ণ *)vim_strsave(p_pyhome);
	    যদি (py_home_buf != রিক্ত)
		Py_SetPythonHome(py_home_buf);
	}
#ifdef PYTHON_HOME
	অন্যথা যদি (mch_getenv((char_u *)"PYTHONHOME") == রিক্ত)
	    Py_SetPythonHome(PYTHON_HOME);
#endif

	init_structs();

#যদি defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02070000
	// Disable implicit 'import site', because it may cause Vim to exit
	// when it can't be found.
	Py_NoSiteFlag++;
#endif

	Py_Initialize();

#যদি defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02070000
	// 'import site' explicitly.
	site = PyImport_ImportModule("site");
	যদি (site == রিক্ত)
	{
	    emsg(_(e_sorry_this_command_is_disabled_python_site_module_could_not_be_loaded));
	    গচ্ছ fail;
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

	যদি (PythonIO_Init_io())
	    গচ্ছ fail;

	যদি (PythonMod_Init())
	    গচ্ছ fail;

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
DoPyCommand(
    ধ্রুৱ ৱর্ণ		*cmd,
    dict_T*		locals,
    rangeinitializer	init_range,
    runner		run,
    শূন্য		*arg)
{
#ifndef PY_CAN_RECURSE
    স্থির পূর্ণাঙ্ক		recursive = 0;
#endif
#যদি defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    ৱর্ণ		*saved_locale;
#endif
#ifdef PY_CAN_RECURSE
    PyGILState_STATE	pygilstate;
#endif

#ifndef PY_CAN_RECURSE
    যদি (recursive)
    {
	emsg(_(e_cannot_invoke_python_recursively));
	ঘূৰাই;
    }
    ++recursive;
#endif
    যদি (python_end_called)
	ঘূৰাই;

    যদি (Python_Init())
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
	saved_locale = (ৱর্ণ *) PY_STRSAVE(saved_locale);
	(শূন্য)setlocale(LC_NUMERIC, "C");
    }
#endif

#ifdef PY_CAN_RECURSE
    pygilstate = PyGILState_Ensure();
#অন্যথা
    Python_RestoreThread();	    // enter python
#endif

    run((ৱর্ণ *) cmd, locals, arg
#ifdef PY_CAN_RECURSE
	    , &pygilstate
#endif
	    );

#ifdef PY_CAN_RECURSE
    PyGILState_Release(pygilstate);
#অন্যথা
    Python_SaveThread();	    // leave python
#endif

#যদি defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    যদি (saved_locale != রিক্ত)
    {
	(শূন্য)setlocale(LC_NUMERIC, saved_locale);
	PyMem_Free(saved_locale);
    }
#endif

    Python_Lock_Vim();		    // enter vim
    PythonIO_Flush();

theend:
#ifndef PY_CAN_RECURSE
    --recursive;
#endif
    ঘূৰাই;
}

/*
 * ":python"
 */
    শূন্য
ex_python(exarg_T *eap)
{
    char_u *script;

    script = script_get(eap, eap->arg);
    যদি (!eap->skip)
    {
	যদি (p_pyx == 0)
	    p_pyx = 2;

	DoPyCommand(script == রিক্ত ? (ৱর্ণ *) eap->arg : (ৱর্ণ *) script,
		রিক্ত,
		init_range_cmd,
		(runner) run_cmd,
		(শূন্য *) eap);
    }
    vim_free(script);
}

#define BUFFER_SIZE 1024

/*
 * ":pyfile"
 */
    শূন্য
ex_pyfile(exarg_T *eap)
{
    স্থির ৱর্ণ buffer[BUFFER_SIZE];
    ধ্রুৱ ৱর্ণ *file = (ৱর্ণ *)eap->arg;
    ৱর্ণ *p;

    যদি (p_pyx == 0)
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

    যেতিয়ালৈ (*file && p < buffer + (BUFFER_SIZE - 3))
    {
	যদি (*file == '\\' || *file == '\'')
	    *p++ = '\\';
	*p++ = *file++;
    }

    // If we didn't finish the file name, we hit a buffer overflow
    যদি (*file != '\0')
	ঘূৰাই;

    // Put in the terminating "')" and a null
    *p++ = '\'';
    *p++ = ')';
    *p++ = '\0';

    // Execute the file
    DoPyCommand(buffer,
	    রিক্ত,
	    init_range_cmd,
	    (runner) run_cmd,
	    (শূন্য *) eap);
}

    শূন্য
ex_pydo(exarg_T *eap)
{
    যদি (p_pyx == 0)
	p_pyx = 2;

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
OutputGetattr(PyObject *self, ৱর্ণ *name)
{
    যদি (strcmp(name, "softspace") == 0)
	ঘূৰাই PyInt_FromLong(((OutputObject *)(self))->softspace);
    অন্যথা যদি (strcmp(name, "__members__") == 0)
	ঘূৰাই ObjectDir(রিক্ত, OutputAttrs);
    অন্যথা যদি (strcmp(name, "errors") == 0)
	ঘূৰাই PyString_FromString("strict");
    অন্যথা যদি (strcmp(name, "encoding") == 0)
	ঘূৰাই PyString_FromString(ENC_OPT);
    ঘূৰাই Py_FindMethod(OutputMethods, self, name);
}

///////////////////////////////////////////////////////
// 3. Implementation of the Vim module for Python

// Window type - Implementation functions
// --------------------------------------

#define WindowType_Check(obj) ((obj)->ob_type == &WindowType)

// Buffer type - Implementation functions
// --------------------------------------

#define BufferType_Check(obj) ((obj)->ob_type == &BufferType)

স্থির পূর্ণাঙ্ক BufferAssItem(PyObject *, PyInt, PyObject *);
স্থির পূর্ণাঙ্ক BufferAssSlice(PyObject *, PyInt, PyInt, PyObject *);

// Line range type - Implementation functions
// --------------------------------------

#define RangeType_Check(obj) ((obj)->ob_type == &RangeType)

স্থির পূর্ণাঙ্ক RangeAssItem(PyObject *, PyInt, PyObject *);
স্থির পূর্ণাঙ্ক RangeAssSlice(PyObject *, PyInt, PyInt, PyObject *);

// Current objects type - Implementation functions
// -----------------------------------------------

স্থির PySequenceMethods BufferAsSeq = {
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

    স্থির PyObject *
BufferGetattr(PyObject *self, ৱর্ণ *name)
{
    PyObject *r;

    যদি ((r = BufferAttrValid((BufferObject *)(self), name)))
	ঘূৰাই r;

    যদি (CheckBuffer((BufferObject *)(self)))
	ঘূৰাই রিক্ত;

    r = BufferAttr((BufferObject *)(self), name);
    যদি (r || PyErr_Occurred())
	ঘূৰাই r;
    অন্যথা
	ঘূৰাই Py_FindMethod(BufferMethods, self, name);
}

//////////////////

    স্থির পূর্ণাঙ্ক
BufferAssItem(PyObject *self, PyInt n, PyObject *val)
{
    ঘূৰাই RBAsItem((BufferObject *)(self), n, val, 1, -1, রিক্ত);
}

    স্থির পূর্ণাঙ্ক
BufferAssSlice(PyObject *self, PyInt lo, PyInt hi, PyObject *val)
{
    ঘূৰাই RBAsSlice((BufferObject *)(self), lo, hi, val, 1, -1, রিক্ত);
}

স্থির PySequenceMethods RangeAsSeq = {
    (PyInquiry)		RangeLength,	      // sq_length,    len(x)
    (binaryfunc)	0, /* RangeConcat, */ // sq_concat,    x+y
    (PyIntArgFunc)	0, /* RangeRepeat, */ // sq_repeat,    x*n
    (PyIntArgFunc)	RangeItem,	      // sq_item,      x[i]
    (PyIntIntArgFunc)	RangeSlice,	      // sq_slice,     x[i:j]
    (PyIntObjArgProc)	RangeAssItem,	      // sq_ass_item,  x[i]=v
    (PyIntIntObjArgProc) RangeAssSlice,	      // sq_ass_slice, x[i:j]=v
    (objobjproc)	0,
#যদি PY_MAJOR_VERSION >= 2
    (binaryfunc)	0,
    0,
#endif
};

// Line range object - Implementation

    স্থির PyObject *
RangeGetattr(PyObject *self, ৱর্ণ *name)
{
    যদি (strcmp(name, "start") == 0)
	ঘূৰাই Py_Build_মান(Py_ssize_t_fmt, ((RangeObject *)(self))->start - 1);
    অন্যথা যদি (strcmp(name, "end") == 0)
	ঘূৰাই Py_Build_মান(Py_ssize_t_fmt, ((RangeObject *)(self))->end - 1);
    অন্যথা যদি (strcmp(name, "__members__") == 0)
	ঘূৰাই ObjectDir(রিক্ত, RangeAttrs);
    অন্যথা
	ঘূৰাই Py_FindMethod(RangeMethods, self, name);
}

////////////////

    স্থির পূর্ণাঙ্ক
RangeAssItem(PyObject *self, PyInt n, PyObject *val)
{
    ঘূৰাই RBAsItem(((RangeObject *)(self))->buf, n, val,
		     ((RangeObject *)(self))->start,
		     ((RangeObject *)(self))->end,
		     &((RangeObject *)(self))->end);
}

    স্থির পূর্ণাঙ্ক
RangeAssSlice(PyObject *self, PyInt lo, PyInt hi, PyObject *val)
{
    ঘূৰাই RBAsSlice(((RangeObject *)(self))->buf, lo, hi, val,
		      ((RangeObject *)(self))->start,
		      ((RangeObject *)(self))->end,
		      &((RangeObject *)(self))->end);
}

// TabPage object - Implementation

    স্থির PyObject *
TabPageGetattr(PyObject *self, ৱর্ণ *name)
{
    PyObject *r;

    যদি ((r = TabPageAttrValid((TabPageObject *)(self), name)))
	ঘূৰাই r;

    যদি (CheckTabPage((TabPageObject *)(self)))
	ঘূৰাই রিক্ত;

    r = TabPageAttr((TabPageObject *)(self), name);
    যদি (r || PyErr_Occurred())
	ঘূৰাই r;
    অন্যথা
	ঘূৰাই Py_FindMethod(TabPageMethods, self, name);
}

// Window object - Implementation

    স্থির PyObject *
WindowGetattr(PyObject *self, ৱর্ণ *name)
{
    PyObject *r;

    যদি ((r = WindowAttrValid((WindowObject *)(self), name)))
	ঘূৰাই r;

    যদি (CheckWindow((WindowObject *)(self)))
	ঘূৰাই রিক্ত;

    r = WindowAttr((WindowObject *)(self), name);
    যদি (r || PyErr_Occurred())
	ঘূৰাই r;
    অন্যথা
	ঘূৰাই Py_FindMethod(WindowMethods, self, name);
}

// Tab page list object - Definitions

স্থির PySequenceMethods TabListAsSeq = {
    (PyInquiry)		TabListLength,	    // sq_length,    len(x)
    (binaryfunc)	0,		    // sq_concat,    x+y
    (PyIntArgFunc)	0,		    // sq_repeat,    x*n
    (PyIntArgFunc)	TabListItem,	    // sq_item,      x[i]
    (PyIntIntArgFunc)	0,		    // sq_slice,     x[i:j]
    (PyIntObjArgProc)	0,		    // sq_ass_item,  x[i]=v
    (PyIntIntObjArgProc) 0,		    // sq_ass_slice, x[i:j]=v
    (objobjproc)	0,
#যদি PY_MAJOR_VERSION >= 2
    (binaryfunc)	0,
    0,
#endif
};

// Window list object - Definitions

স্থির PySequenceMethods WinListAsSeq = {
    (PyInquiry)		WinListLength,	    // sq_length,    len(x)
    (binaryfunc)	0,		    // sq_concat,    x+y
    (PyIntArgFunc)	0,		    // sq_repeat,    x*n
    (PyIntArgFunc)	WinListItem,	    // sq_item,      x[i]
    (PyIntIntArgFunc)	0,		    // sq_slice,     x[i:j]
    (PyIntObjArgProc)	0,		    // sq_ass_item,  x[i]=v
    (PyIntIntObjArgProc) 0,		    // sq_ass_slice, x[i:j]=v
    (objobjproc)	0,
#যদি PY_MAJOR_VERSION >= 2
    (binaryfunc)	0,
    0,
#endif
};

// External interface

    শূন্য
python_buffer_free(buf_T *buf)
{
    BufferObject *bp = BUF_PYTHON_REF(buf);
    যদি (bp == রিক্ত)
	ঘূৰাই;
    bp->buf = INVALID_BUFFER_মান;
    BUF_PYTHON_REF(buf) = রিক্ত;
}

    শূন্য
python_window_free(win_T *win)
{
    WindowObject *wp = WIN_PYTHON_REF(win);
    যদি (wp == রিক্ত)
	ঘূৰাই;
    wp->win = INVALID_WINDOW_মান;
    WIN_PYTHON_REF(win) = রিক্ত;
}

    শূন্য
python_tabpage_free(tabpage_T *tab)
{
    TabPageObject *tp = TAB_PYTHON_REF(tab);
    যদি (tp == রিক্ত)
	ঘূৰাই;
    tp->tab = INVALID_TABPAGE_মান;
    TAB_PYTHON_REF(tab) = রিক্ত;
}

    স্থির পূর্ণাঙ্ক
PythonMod_Init(শূন্য)
{
    // The special value is removed from sys.path in Python_Init().
    স্থির ৱর্ণ	*(argv[2]) = {"/must>not&exist/foo", রিক্ত};

    যদি (init_types())
	ঘূৰাই -1;

    // Set sys.argv[] to avoid a crash in warn().
    PySys_SetArgv(1, argv);

    vim_module = Py_InitModule4("vim", VimMethods, (ৱর্ণ *)রিক্ত,
				(PyObject *)রিক্ত, PYTHON_API_VERSION);

    যদি (populate_module(vim_module))
	ঘূৰাই -1;

    যদি (init_sys_path())
	ঘূৰাই -1;

    ঘূৰাই 0;
}

//////////////////////////////////////////////////////////////////////////
// 4. Utility functions for handling the interface between Vim and Python.

// Convert a Vim line into a Python string.
// All internal newlines are replaced by null characters.
//
// On errors, the Python exception data is set, and NULL is returned.
    স্থির PyObject *
সাৰি_To_String(ধ্রুৱ ৱর্ণ *str)
{
    PyObject *result;
    PyInt len = strlen(str);
    ৱর্ণ *p;

    // Allocate a Python string object, with uninitialised contents. We
    // must do it this way, so that we can modify the string in place
    // later. See the Python source, Objects/stringobject.c for details.
    result = PyString_FromStringAndSize(রিক্ত, len);
    যদি (result == রিক্ত)
	ঘূৰাই রিক্ত;

    p = PyString_AsString(result);

    যেতিয়ালৈ (*str)
    {
	যদি (*str == '\n')
	    *p = '\0';
	অন্যথা
	    *p = *str;

	++p;
	++str;
    }

    ঘূৰাই result;
}

    স্থির PyObject *
DictionaryGetattr(PyObject *self, ৱর্ণ *name)
{
    DictionaryObject	*this = ((DictionaryObject *) (self));

    যদি (strcmp(name, "locked") == 0)
	ঘূৰাই PyInt_FromLong(this->dict->dv_lock);
    অন্যথা যদি (strcmp(name, "scope") == 0)
	ঘূৰাই PyInt_FromLong(this->dict->dv_scope);
    অন্যথা যদি (strcmp(name, "__members__") == 0)
	ঘূৰাই ObjectDir(রিক্ত, DictionaryAttrs);

    ঘূৰাই Py_FindMethod(DictionaryMethods, self, name);
}

    স্থির PyObject *
ListGetattr(PyObject *self, ৱর্ণ *name)
{
    যদি (strcmp(name, "locked") == 0)
	ঘূৰাই PyInt_FromLong(((ListObject *)(self))->list->lv_lock);
    অন্যথা যদি (strcmp(name, "__members__") == 0)
	ঘূৰাই ObjectDir(রিক্ত, ListAttrs);

    ঘূৰাই Py_FindMethod(ListMethods, self, name);
}

    স্থির PyObject *
TupleGetattr(PyObject *self, ৱর্ণ *name)
{
    যদি (strcmp(name, "locked") == 0)
	ঘূৰাই PyInt_FromLong(((TupleObject *)(self))->tuple->tv_lock);
    অন্যথা যদি (strcmp(name, "__members__") == 0)
	ঘূৰাই ObjectDir(রিক্ত, TupleAttrs);

    ঘূৰাই Py_FindMethod(TupleMethods, self, name);
}

    স্থির PyObject *
FunctionGetattr(PyObject *self, ৱর্ণ *name)
{
    PyObject	*r;

    r = FunctionAttr((FunctionObject *)(self), name);

    যদি (r || PyErr_Occurred())
	ঘূৰাই r;
    অন্যথা
	ঘূৰাই Py_FindMethod(FunctionMethods, self, name);
}

    শূন্য
do_pyeval(char_u *str, dict_T *locals, typval_T *rettv)
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
set_ref_in_python(পূর্ণাঙ্ক copyID)
{
    ঘূৰাই set_ref_in_py(copyID);
}
