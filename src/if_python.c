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

#அந்தர்பாவ "vim.h"

#அந்தர்பாவ <limits.h>

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

#எனில் defined(MSWIN) && defined(HAVE_FCNTL_H)
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

#அந்தர்பாவ <Python.h>

#எனில் !defined(PY_VERSION_HEX) || PY_VERSION_HEX < 0x02050000
# undef PY_SSIZE_T_CLEAN
#endif

// these are NULL for Python 2
#define ERRORS_DECODE_ARG வெற்று
#define ERRORS_ENCODE_ARG ERRORS_DECODE_ARG

#undef முதன்மை // Defined in python.h - aargh
#undef HAVE_FCNTL_H // Clash with os_win32.h

// Perhaps leave this out for Python 2.6, which supports bytes?
#define PyBytes_FromString      PyString_FromString
#define PyBytes_Check		PyString_Check
#define PyBytes_AsStringAndSize PyString_AsStringAndSize
#define PyBytes_FromStringAndSize   PyString_FromStringAndSize

#எனில் defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02070000
# define PY_USE_CAPSULE
#endif

#எனில் defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02050000
# define PyInt Py_ssize_t
# define PyInquiry lenfunc
# define PyIntArgFunc ssizeargfunc
# define PyIntIntArgFunc ssizessizeargfunc
# define PyIntObjArgProc ssizeobjargproc
# define PyIntIntObjArgProc ssizessizeobjargproc
# define Py_ssize_t_fmt "n"
#இல்லையெனில்
# define PyInt முழு
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
#define single_உள்ளீடு	256
#define file_உள்ளீடு	257
#define eval_உள்ளீடு	258

#எனில் defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x020300F0
  // Python 2.3: can invoke ":python" recursively.
# define PY_CAN_RECURSE
#endif

#எனில் defined(DYNAMIC_PYTHON)
# ifndef DYNAMIC_PYTHON
#  define HINSTANCE long_u		// for generating prototypes
# endif

# ifdef MSWIN
#  define load_dll vimLoadLib
#  define close_dll FreeLibrary
#  define குறியீடு_from_dll GetProcAddress
#  define load_dll_error GetWin32Error
# இல்லையெனில்
#  include <dlfcn.h>
#  define FARPROC வெறுமை*
#  define HINSTANCE வெறுமை*
#  எனில் defined(PY_NO_RTLD_GLOBAL) && defined(PY3_NO_RTLD_GLOBAL)
#   define load_dll(n) dlopen((n), RTLD_LAZY)
#  இல்லையெனில்
#   define load_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#  endif
#  define close_dll dlclose
#  define குறியீடு_from_dll dlsym
#  define load_dll_error dlerror
# endif

// This makes if_python.c compile without warnings against Python 2.5
// on Win32 and Win64.
# undef PyRun_SimpleString
# undef PyRun_String
# undef PyArg_Parse
# undef PyArg_ParseTuple
# undef Py_Build_மதிப்பு
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
# define Py_List_செருகு dll_Py_List_செருகு
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
# ifdef Py_Mapping_சாவிகள்
#  define PY_NO_MAPPING_சாவிகள்
# இல்லையெனில்
#  define Py_Mapping_சாவிகள் dll_Py_Mapping_சாவிகள்
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
# define Py_எண்_Check dll_Py_எண்_Check
# define Py_எண்_Long dll_Py_எண்_Long
# define PyImport_AddModule (*dll_PyImport_AddModule)
# define PySys_SetObject dll_PySys_SetObject
# define PySys_GetObject dll_PySys_GetObject
# define PySys_SetArgv dll_PySys_SetArgv
# define PyType_Type (*dll_PyType_Type)
# define PyFile_Type (*dll_PyFile_Type)
# define PySlice_Type (*dll_PySlice_Type)
# define PyType_Ready (*dll_PyType_Ready)
# define PyType_GenericAlloc dll_PyType_GenericAlloc
# define Py_Build_மதிப்பு dll_Py_Build_மதிப்பு
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
# இல்லையெனில்
#  define PyObject_GC_Del dll_PyObject_GC_Del
#  define PyObject_GC_UnTrack dll_PyObject_GC_UnTrack
# endif
# எனில் defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02070000
#  define _PyObject_NextNotImplemented (*dll__PyObject_NextNotImplemented)
# endif
# define _Py_NoneStruct (*dll__Py_NoneStruct)
# define _Py_ZeroStruct (*dll__Py_ZeroStruct)
# define _Py_TrueStruct (*dll__Py_TrueStruct)
# define PyObject_Init dll__PyObject_Init
# define PyObject_GetIter dll_PyObject_GetIter
# define PyObject_IsTrue dll_PyObject_IsTrue
# எனில் defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02020000
#  define PyType_IsSubtype dll_PyType_IsSubtype
#  ifdef Py_DEBUG
#   define _Py_NegativeRefcount dll__Py_NegativeRefcount
#   define _Py_RefTotal (*dll__Py_RefTotal)
#   define _Py_Dealloc dll__Py_Dealloc
#  endif
# endif
# எனில் defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02030000
#  எனில் defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
#   define _PyObject_DebugMalloc dll__PyObject_DebugMalloc
#   define _PyObject_DebugFree dll__PyObject_DebugFree
#  இல்லையெனில்
#   define PyObject_Malloc dll_PyObject_Malloc
#   define PyObject_Free dll_PyObject_Free
#  endif
# endif
# ifdef PY_USE_CAPSULE
#  define PyCapsule_New dll_PyCapsule_New
#  define PyCapsule_GetPointer dll_PyCapsule_GetPointer
# இல்லையெனில்
#  define PyCObject_FromVoidPtr dll_PyCObject_FromVoidPtr
#  define PyCObject_AsVoidPtr dll_PyCObject_AsVoidPtr
# endif
# எனில் defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02070000
#  define Py_NoSiteFlag (*dll_Py_NoSiteFlag)
# endif

/*
 * Pointers for dynamic link
 */
நிலையான முழு(*dll_PyArg_Parse)(PyObject *, எழுத்து *, ...);
நிலையான முழு(*dll_PyArg_ParseTuple)(PyObject *, எழுத்து *, ...);
நிலையான வெறுமை(*dll_PyMem_Free)(வெறுமை *);
நிலையான வெறுமை* (*dll_PyMem_Malloc)(size_t);
நிலையான முழு(*dll_PyDict_SetItemString)(PyObject *dp, எழுத்து *சாவி, PyObject *item);
நிலையான முழு(*dll_PyErr_BadArgument)(வெறுமை);
நிலையான PyObject *(*dll_PyErr_NewException)(எழுத்து *, PyObject *, PyObject *);
நிலையான வெறுமை(*dll_PyErr_Clear)(வெறுமை);
நிலையான PyObject*(*dll_PyErr_Format)(PyObject *, மாறிலி எழுத்து *, ...);
நிலையான வெறுமை(*dll_PyErr_PrintEx)(முழு);
நிலையான PyObject*(*dll_PyErr_NoMemory)(வெறுமை);
நிலையான PyObject*(*dll_PyErr_Occurred)(வெறுமை);
நிலையான வெறுமை(*dll_PyErr_SetNone)(PyObject *);
நிலையான வெறுமை(*dll_PyErr_SetString)(PyObject *, மாறிலி எழுத்து *);
நிலையான வெறுமை(*dll_PyErr_SetObject)(PyObject *, PyObject *);
நிலையான முழு(*dll_PyErr_ExceptionMatches)(PyObject *);
நிலையான வெறுமை(*dll_PyEval_InitThreads)(வெறுமை);
நிலையான வெறுமை(*dll_PyEval_RestoreThread)(PyThreadState *);
நிலையான PyThreadState*(*dll_PyEval_SaveThread)(வெறுமை);
# ifdef PY_CAN_RECURSE
நிலையான PyGILState_STATE	(*dll_PyGILState_Ensure)(வெறுமை);
நிலையான வெறுமை (*dll_PyGILState_Release)(PyGILState_STATE);
# endif
நிலையான long(*dll_PyInt_AsLong)(PyObject *);
நிலையான PyObject*(*dll_PyInt_FromLong)(long);
நிலையான long(*dll_PyLong_AsLong)(PyObject *);
நிலையான PyObject*(*dll_PyLong_FromLong)(long);
நிலையான PyTypeObject* dll_PyBool_Type;
நிலையான PyTypeObject* dll_PyInt_Type;
நிலையான PyTypeObject* dll_PyLong_Type;
நிலையான PyObject*(*dll_PyList_GetItem)(PyObject *, PyInt);
நிலையான முழு(*dll_PyList_Append)(PyObject *, PyObject *);
நிலையான முழு(*dll_Py_List_செருகு)(PyObject *, PyInt, PyObject *);
நிலையான PyObject*(*dll_PyList_New)(PyInt size);
நிலையான முழு(*dll_PyList_SetItem)(PyObject *, PyInt, PyObject *);
நிலையான PyInt(*dll_PyList_Size)(PyObject *);
நிலையான PyTypeObject* dll_PyList_Type;
நிலையான PyObject*(*dll_PyTuple_GetItem)(PyObject *, PyInt);
நிலையான முழு(*dll_PyTuple_SetItem)(PyObject *, PyInt, PyObject *);
நிலையான PyObject*(*dll_PyTuple_New)(PyInt size);
நிலையான PyInt(*dll_PyTuple_Size)(PyObject *);
நிலையான PyTypeObject* dll_PyTuple_Type;
நிலையான முழு (*dll_PySequence_Check)(PyObject *);
நிலையான PyInt(*dll_PySequence_Size)(PyObject *);
நிலையான PyObject*(*dll_PySequence_GetItem)(PyObject *, PyInt);
நிலையான PyObject*(*dll_PySequence_Fast)(PyObject *, மாறிலி எழுத்து *);
நிலையான முழு (*dll_PySlice_GetIndicesEx)(PySliceObject *r, PyInt length,
		     PyInt *start, PyInt *stop, PyInt *step,
		     PyInt *slicelen);
நிலையான PyObject*(*dll_PyImport_ImportModule)(மாறிலி எழுத்து *);
நிலையான PyObject*(*dll_PyDict_New)(வெறுமை);
நிலையான PyObject*(*dll_PyDict_GetItemString)(PyObject *, மாறிலி எழுத்து *);
நிலையான முழு (*dll_PyDict_Next)(PyObject *, PyInt *, PyObject **, PyObject **);
நிலையான PyTypeObject* dll_PyDict_Type;
# ifndef PY_NO_MAPPING_சாவிகள்
நிலையான PyObject* (*dll_Py_Mapping_சாவிகள்)(PyObject *);
# endif
நிலையான PyObject* (*dll_PyObject_GetItem)(PyObject *, PyObject *);
நிலையான PyObject* (*dll_PyObject_CallMethod)(PyObject *, எழுத்து *, PyObject *);
நிலையான முழு (*dll_PyMapping_Check)(PyObject *);
நிலையான PyObject* (*dll_PyIter_Next)(PyObject *);
நிலையான PyObject*(*dll_PyModule_GetDict)(PyObject *);
நிலையான முழு(*dll_PyModule_AddObject)(PyObject *, மாறிலி எழுத்து *, PyObject *);
நிலையான முழு(*dll_PyRun_SimpleString)(எழுத்து *);
நிலையான PyObject *(*dll_PyRun_String)(எழுத்து *, முழு, PyObject *, PyObject *);
நிலையான PyObject* (*dll_PyObject_GetAttrString)(PyObject *, மாறிலி எழுத்து *);
நிலையான முழு (*dll_PyObject_HasAttrString)(PyObject *, மாறிலி எழுத்து *);
நிலையான முழு (*dll_PyObject_SetAttrString)(PyObject *, மாறிலி எழுத்து *, PyObject *);
நிலையான PyObject* (*dll_PyObject_CallFunctionObjArgs)(PyObject *, ...);
நிலையான PyObject* (*dll_PyObject_CallFunction)(PyObject *, எழுத்து *, ...);
நிலையான PyObject* (*dll_PyObject_Call)(PyObject *, PyObject *, PyObject *);
நிலையான PyObject* (*dll_PyObject_Repr)(PyObject *);
நிலையான எழுத்து*(*dll_PyString_AsString)(PyObject *);
நிலையான முழு(*dll_PyString_AsStringAndSize)(PyObject *, எழுத்து **, PyInt *);
நிலையான PyObject*(*dll_PyString_FromString)(மாறிலி எழுத்து *);
நிலையான PyObject*(*dll_PyString_FromFormat)(மாறிலி எழுத்து *, ...);
நிலையான PyObject*(*dll_PyString_FromStringAndSize)(மாறிலி எழுத்து *, PyInt);
நிலையான PyInt(*dll_PyString_Size)(PyObject *);
நிலையான PyTypeObject* dll_PyString_Type;
நிலையான PyTypeObject* dll_PyUnicode_Type;
நிலையான PyObject *(*py_PyUnicode_AsEncodedString)(PyObject *, எழுத்து *, எழுத்து *);
நிலையான இரட்டை(*dll_PyFloat_AsDouble)(PyObject *);
நிலையான PyObject*(*dll_PyFloat_FromDouble)(இரட்டை);
நிலையான PyTypeObject* dll_PyFloat_Type;
நிலையான முழு(*dll_Py_எண்_Check)(PyObject *);
நிலையான PyObject*(*dll_Py_எண்_Long)(PyObject *);
நிலையான முழு(*dll_PySys_SetObject)(எழுத்து *, PyObject *);
நிலையான PyObject *(*dll_PySys_GetObject)(எழுத்து *);
நிலையான முழு(*dll_PySys_SetArgv)(முழு, எழுத்து **);
நிலையான PyTypeObject* dll_PyType_Type;
நிலையான PyTypeObject* dll_PyFile_Type;
நிலையான PyTypeObject* dll_PySlice_Type;
நிலையான முழு (*dll_PyType_Ready)(PyTypeObject *type);
நிலையான PyObject* (*dll_PyType_GenericAlloc)(PyTypeObject *type, PyInt nitems);
நிலையான PyObject*(*dll_Py_Build_மதிப்பு)(எழுத்து *, ...);
நிலையான PyObject*(*dll_Py_FindMethod)(கட்டமைப்பு PyMethodDef[], PyObject *, எழுத்து *);
நிலையான PyObject*(*dll_Py_InitModule4)(எழுத்து *, கட்டமைப்பு PyMethodDef *, எழுத்து *, PyObject *, முழு);
நிலையான PyObject*(*dll_PyImport_AddModule)(எழுத்து *);
நிலையான வெறுமை(*dll_Py_SetPythonHome)(எழுத்து *home);
நிலையான வெறுமை(*dll_Py_Initialize)(வெறுமை);
நிலையான வெறுமை(*dll_Py_Finalize)(வெறுமை);
நிலையான முழு(*dll_Py_IsInitialized)(வெறுமை);
நிலையான PyObject*(*dll__PyObject_New)(PyTypeObject *, PyObject *);
நிலையான PyObject*(*dll__PyObject_GC_New)(PyTypeObject *);
# ifdef Py_underscore_GC
நிலையான வெறுமை(*dll__PyObject_GC_Del)(வெறுமை *);
நிலையான வெறுமை(*dll__PyObject_GC_UnTrack)(வெறுமை *);
# இல்லையெனில்
நிலையான வெறுமை(*dll_PyObject_GC_Del)(வெறுமை *);
நிலையான வெறுமை(*dll_PyObject_GC_UnTrack)(வெறுமை *);
# endif
நிலையான PyObject*(*dll__PyObject_Init)(PyObject *, PyTypeObject *);
நிலையான PyObject* (*dll_PyObject_GetIter)(PyObject *);
நிலையான முழு (*dll_PyObject_IsTrue)(PyObject *);
# எனில் defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02070000
நிலையான iternextfunc dll__PyObject_NextNotImplemented;
# endif
நிலையான PyObject* dll__Py_NoneStruct;
நிலையான PyObject* _Py_ZeroStruct;
நிலையான PyObject* dll__Py_TrueStruct;
# எனில் defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02020000
நிலையான முழு (*dll_PyType_IsSubtype)(PyTypeObject *, PyTypeObject *);
#  ifdef Py_DEBUG
நிலையான வெறுமை (*dll__Py_NegativeRefcount)(மாறிலி எழுத்து *fname, முழு lineno, PyObject *op);
நிலையான PyInt* dll__Py_RefTotal;
நிலையான வெறுமை (*dll__Py_Dealloc)(PyObject *obj);
#  endif
# endif
# எனில் defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02030000
#  எனில் defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
நிலையான வெறுமை (*dll__PyObject_DebugFree)(வெறுமை*);
நிலையான வெறுமை* (*dll__PyObject_DebugMalloc)(size_t);
#  இல்லையெனில்
நிலையான வெறுமை* (*dll_PyObject_Malloc)(size_t);
நிலையான வெறுமை (*dll_PyObject_Free)(வெறுமை*);
#  endif
# endif
# ifdef PY_USE_CAPSULE
நிலையான PyObject* (*dll_PyCapsule_New)(வெறுமை *, எழுத்து *, PyCapsule_Destructor);
நிலையான வெறுமை* (*dll_PyCapsule_GetPointer)(PyObject *, எழுத்து *);
# இல்லையெனில்
நிலையான PyObject* (*dll_PyCObject_FromVoidPtr)(வெறுமை *cobj, வெறுமை (*destr)(வெறுமை *));
நிலையான வெறுமை* (*dll_PyCObject_AsVoidPtr)(PyObject *);
# endif
# எனில் defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02070000
நிலையான முழு* dll_Py_NoSiteFlag;
# endif

நிலையான HINSTANCE hinstPython = 0; // Instance of python.dll

// Imported exception objects
நிலையான PyObject *imp_PyExc_AttributeError;
நிலையான PyObject *imp_PyExc_IndexError;
நிலையான PyObject *imp_Py_Exc_சாவி_Error;
நிலையான PyObject *imp_PyExc_KeyboardInterrupt;
நிலையான PyObject *imp_PyExc_TypeError;
நிலையான PyObject *imp_Py_Exc_மதிப்பு_Error;
நிலையான PyObject *imp_PyExc_SystemExit;
நிலையான PyObject *imp_PyExc_RuntimeError;
நிலையான PyObject *imp_PyExc_ImportError;
நிலையான PyObject *imp_PyExc_OverflowError;

# define PyExc_AttributeError imp_PyExc_AttributeError
# define PyExc_IndexError imp_PyExc_IndexError
# define Py_Exc_சாவி_Error imp_Py_Exc_சாவி_Error
# define PyExc_KeyboardInterrupt imp_PyExc_KeyboardInterrupt
# define PyExc_TypeError imp_PyExc_TypeError
# define Py_Exc_மதிப்பு_Error imp_Py_Exc_மதிப்பு_Error
# define PyExc_SystemExit imp_PyExc_SystemExit
# define PyExc_RuntimeError imp_PyExc_RuntimeError
# define PyExc_ImportError imp_PyExc_ImportError
# define PyExc_OverflowError imp_PyExc_OverflowError

/*
 * Table of name to function pointer of python.
 */
# define PYTHON_PROC FARPROC
நிலையான கட்டமைப்பு
{
    எழுத்து *name;
    PYTHON_PROC *ptr;
} python_funcname_table[] =
{
# ifdef PY_SSIZE_T_CLEAN
    {"_PyArg_Parse_SizeT", (PYTHON_PROC*)&dll_PyArg_Parse},
    {"_PyArg_ParseTuple_SizeT", (PYTHON_PROC*)&dll_PyArg_ParseTuple},
    {"_Py_BuildValue_SizeT", (PYTHON_PROC*)&dll_Py_Build_மதிப்பு},
# இல்லையெனில்
    {"PyArg_Parse", (PYTHON_PROC*)&dll_PyArg_Parse},
    {"PyArg_ParseTuple", (PYTHON_PROC*)&dll_PyArg_ParseTuple},
    {"Py_BuildValue", (PYTHON_PROC*)&dll_Py_Build_மதிப்பு},
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
    {"PyList_Insert", (PYTHON_PROC*)&dll_Py_List_செருகு},
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
# ifndef PY_NO_MAPPING_சாவிகள்
    {"PyMapping_Keys", (PYTHON_PROC*)&dll_Py_Mapping_சாவிகள்},
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
    {"PyNumber_Check", (PYTHON_PROC*)&dll_Py_எண்_Check},
    {"PyNumber_Long", (PYTHON_PROC*)&dll_Py_எண்_Long},
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
# இல்லையெனில்
    {"PyObject_GC_Del", (PYTHON_PROC*)&dll_PyObject_GC_Del},
    {"PyObject_GC_UnTrack", (PYTHON_PROC*)&dll_PyObject_GC_UnTrack},
# endif
    {"PyObject_Init", (PYTHON_PROC*)&dll__PyObject_Init},
    {"PyObject_GetIter", (PYTHON_PROC*)&dll_PyObject_GetIter},
    {"PyObject_IsTrue", (PYTHON_PROC*)&dll_PyObject_IsTrue},
# எனில் defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02070000
    {"_PyObject_NextNotImplemented", (PYTHON_PROC*)&dll__PyObject_NextNotImplemented},
# endif
    {"_Py_NoneStruct", (PYTHON_PROC*)&dll__Py_NoneStruct},
    {"_Py_ZeroStruct", (PYTHON_PROC*)&dll__Py_ZeroStruct},
    {"_Py_TrueStruct", (PYTHON_PROC*)&dll__Py_TrueStruct},
# எனில் defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02020000
#  ifdef Py_DEBUG
    {"_Py_NegativeRefcount", (PYTHON_PROC*)&dll__Py_NegativeRefcount},
    {"_Py_RefTotal", (PYTHON_PROC*)&dll__Py_RefTotal},
    {"_Py_Dealloc", (PYTHON_PROC*)&dll__Py_Dealloc},
#  endif
    {"PyType_IsSubtype", (PYTHON_PROC*)&dll_PyType_IsSubtype},
# endif
# எனில் defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02030000
#  எனில் defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
    {"_PyObject_DebugFree", (PYTHON_PROC*)&dll__PyObject_DebugFree},
    {"_PyObject_DebugMalloc", (PYTHON_PROC*)&dll__PyObject_DebugMalloc},
#  இல்லையெனில்
    {"PyObject_Malloc", (PYTHON_PROC*)&dll_PyObject_Malloc},
    {"PyObject_Free", (PYTHON_PROC*)&dll_PyObject_Free},
#  endif
# endif
# எனில் defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02050000 \
	&& SIZEOF_SIZE_T != VIM_SIZEOF_INT
#  ifdef Py_DEBUG
    {"Py_InitModule4TraceRefs_64", (PYTHON_PROC*)&dll_Py_InitModule4},
#  இல்லையெனில்
    {"Py_InitModule4_64", (PYTHON_PROC*)&dll_Py_InitModule4},
#  endif
# இல்லையெனில்
#  ifdef Py_DEBUG
    {"Py_InitModule4TraceRefs", (PYTHON_PROC*)&dll_Py_InitModule4},
#  இல்லையெனில்
    {"Py_InitModule4", (PYTHON_PROC*)&dll_Py_InitModule4},
#  endif
# endif
# ifdef PY_USE_CAPSULE
    {"PyCapsule_New", (PYTHON_PROC*)&dll_PyCapsule_New},
    {"PyCapsule_GetPointer", (PYTHON_PROC*)&dll_PyCapsule_GetPointer},
# இல்லையெனில்
    {"PyCObject_FromVoidPtr", (PYTHON_PROC*)&dll_PyCObject_FromVoidPtr},
    {"PyCObject_AsVoidPtr", (PYTHON_PROC*)&dll_PyCObject_AsVoidPtr},
# endif
# எனில் defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02070000
    {"Py_NoSiteFlag", (PYTHON_PROC*)&dll_Py_NoSiteFlag},
# endif
    {"", வெற்று},
};

/*
 * Load library and get all pointers.
 * Parameter 'libname' provides name of DLL.
 * Return OK or FAIL.
 */
    நிலையான முழு
python_runtime_link_init(எழுத்து *libname, முழு verbose)
{
    முழு i;
    PYTHON_PROC *ucs_as_encoded_string =
				   (PYTHON_PROC*)&py_PyUnicode_AsEncodedString;

# எனில் !(defined(PY_NO_RTLD_GLOBAL) && defined(PY3_NO_RTLD_GLOBAL)) && defined(UNIX) && defined(FEAT_PYTHON3)
    // Can't have Python and Python3 loaded at the same time.
    // It causes a crash, because RTLD_GLOBAL is needed for
    // standard C extension libraries of one or both python versions.
    எனில் (python3_loaded())
    {
	எனில் (verbose)
	    emsg(_(e_this_vim_cannot_execute_python_after_using_py3));
	திரும்பு FAIL;
    }
# endif

    எனில் (hinstPython)
	திரும்பு OK;
    hinstPython = load_dll(libname);
    எனில் (!hinstPython)
    {
	எனில் (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	திரும்பு FAIL;
    }

    க்கு (i = 0; python_funcname_table[i].ptr; ++i)
    {
	எனில் ((*python_funcname_table[i].ptr = குறியீடு_from_dll(hinstPython,
			python_funcname_table[i].name)) == வெற்று)
	{
	    close_dll(hinstPython);
	    hinstPython = 0;
	    எனில் (verbose)
		semsg(_(e_could_not_load_library_function_str), python_funcname_table[i].name);
	    திரும்பு FAIL;
	}
    }

    // Load unicode functions separately as only the ucs2 or the ucs4 functions
    // will be present in the library.
    *ucs_as_encoded_string = குறியீடு_from_dll(hinstPython,
					     "PyUnicodeUCS2_AsEncodedString");
    எனில் (*ucs_as_encoded_string == வெற்று)
	*ucs_as_encoded_string = குறியீடு_from_dll(hinstPython,
					     "PyUnicodeUCS4_AsEncodedString");
    எனில் (*ucs_as_encoded_string == வெற்று)
    {
	close_dll(hinstPython);
	hinstPython = 0;
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
python_enabled(முழு verbose)
{
    திரும்பு python_runtime_link_init((எழுத்து *)p_pydll, verbose) == OK;
}

/*
 * Load the standard Python exceptions - don't import the symbols from the
 * DLL, as this can cause errors (importing data symbols is not reliable).
 */
    நிலையான வெறுமை
get_exceptions(வெறுமை)
{
    PyObject *exmod = PyImport_ImportModule("exceptions");
    PyObject *exdict = PyModule_GetDict(exmod);
    imp_PyExc_AttributeError = PyDict_GetItemString(exdict, "AttributeError");
    imp_PyExc_IndexError = PyDict_GetItemString(exdict, "IndexError");
    imp_Py_Exc_சாவி_Error = PyDict_GetItemString(exdict, "KeyError");
    imp_PyExc_KeyboardInterrupt = PyDict_GetItemString(exdict, "KeyboardInterrupt");
    imp_PyExc_TypeError = PyDict_GetItemString(exdict, "TypeError");
    imp_Py_Exc_மதிப்பு_Error = PyDict_GetItemString(exdict, "ValueError");
    imp_PyExc_SystemExit = PyDict_GetItemString(exdict, "SystemExit");
    imp_PyExc_RuntimeError = PyDict_GetItemString(exdict, "RuntimeError");
    imp_PyExc_ImportError = PyDict_GetItemString(exdict, "ImportError");
    imp_PyExc_OverflowError = PyDict_GetItemString(exdict, "OverflowError");
    Py_XINCREF(imp_PyExc_AttributeError);
    Py_XINCREF(imp_PyExc_IndexError);
    Py_XINCREF(imp_Py_Exc_சாவி_Error);
    Py_XINCREF(imp_PyExc_KeyboardInterrupt);
    Py_XINCREF(imp_PyExc_TypeError);
    Py_XINCREF(imp_Py_Exc_மதிப்பு_Error);
    Py_XINCREF(imp_PyExc_SystemExit);
    Py_XINCREF(imp_PyExc_RuntimeError);
    Py_XINCREF(imp_PyExc_ImportError);
    Py_XINCREF(imp_PyExc_OverflowError);
    Py_XDECREF(exmod);
}
#endif // DYNAMIC_PYTHON

நிலையான முழு initialised = 0;
#define PYINITIALISED initialised
நிலையான முழு python_end_called = FALSE;

#define DESTRUCTOR_FINISH(self) self->ob_type->tp_free((PyObject*)self);

#define WIN_PYTHON_REF(win) win->w_python_ref
#define BUF_PYTHON_REF(buf) buf->b_python_ref
#define TAB_PYTHON_REF(tab) tab->tp_python_ref

நிலையான PyObject *OutputGetattr(PyObject *, எழுத்து *);
நிலையான PyObject *BufferGetattr(PyObject *, எழுத்து *);
நிலையான PyObject *WindowGetattr(PyObject *, எழுத்து *);
நிலையான PyObject *TabPageGetattr(PyObject *, எழுத்து *);
நிலையான PyObject *RangeGetattr(PyObject *, எழுத்து *);
நிலையான PyObject *DictionaryGetattr(PyObject *, எழுத்து*);
நிலையான PyObject *ListGetattr(PyObject *, எழுத்து *);
நிலையான PyObject *TupleGetattr(PyObject *, எழுத்து *);
நிலையான PyObject *FunctionGetattr(PyObject *, எழுத்து *);

#ifndef Py_VISIT
# define Py_VISIT(obj) visit(obj, arg)
#endif
#ifndef Py_CLEAR
# define Py_CLEAR(obj) \
    { \
	Py_XDECREF(obj); \
	obj = வெற்று; \
    }
#endif

#எனில் defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    நிலையான வெறுமை *
py_memsave(வெறுமை *p, size_t len)
{
    வெறுமை	*r;

    எனில் (!(r = PyMem_Malloc(len)))
	திரும்பு வெற்று;
    mch_memmove(r, p, len);
    திரும்பு r;
}

# define PY_STRSAVE(s) ((char_u *) py_memsave(s, STRLEN(s) + 1))
#endif

வகைவரை PySliceObject PySliceObject_T;

/*
 * Include the code shared with if_python3.c
 */
#அந்தர்பாவ "if_py_both.h"


///////////////////////////////////////////////////////
// Internal function prototypes.

நிலையான முழு PythonMod_Init(வெறுமை);


///////////////////////////////////////////////////////
// 1. Python interpreter main program.

#ifndef PY_CAN_RECURSE
நிலையான PyThreadState *saved_python_thread = வெற்று;

/*
 * Suspend a thread of the Python interpreter, other threads are allowed to
 * run.
 */
    நிலையான வெறுமை
Python_SaveThread(வெறுமை)
{
    saved_python_thread = PyEval_SaveThread();
}

/*
 * Restore a thread of the Python interpreter, waits for other threads to
 * block.
 */
    நிலையான வெறுமை
Python_RestoreThread(வெறுமை)
{
    PyEval_RestoreThread(saved_python_thread);
    saved_python_thread = வெற்று;
}
#endif

    வெறுமை
python_end(வெறுமை)
{
    நிலையான முழு recurse = 0;

    // If a crash occurs while doing this, don't try again.
    எனில் (recurse != 0)
	திரும்பு;

    python_end_called = TRUE;
    ++recurse;

#ifdef DYNAMIC_PYTHON
    எனில் (hinstPython && Py_IsInitialized())
    {
# ifdef PY_CAN_RECURSE
	PyGILState_Ensure();
# இல்லையெனில்
	Python_RestoreThread();	    // enter python
# endif
	Py_Finalize();
    }
#இல்லையெனில்
    எனில் (Py_IsInitialized())
    {
# ifdef PY_CAN_RECURSE
	PyGILState_Ensure();
# இல்லையெனில்
	Python_RestoreThread();	    // enter python
# endif
	Py_Finalize();
    }
#endif

    --recurse;
}

#எனில் defined(DYNAMIC_PYTHON) && defined(FEAT_PYTHON3)
    முழு
python_loaded(வெறுமை)
{
    திரும்பு (hinstPython != 0);
}
#endif

நிலையான எழுத்து *py_home_buf = வெற்று;

    நிலையான முழு
Python_Init(வெறுமை)
{
    எனில் (!initialised)
    {
#எனில் defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02070000
	PyObject *site;
#endif

#ifdef DYNAMIC_PYTHON
	எனில் (!python_enabled(TRUE))
	{
	    emsg(_(e_sorry_this_command_is_disabled_python_library_could_not_be_found));
	    செல் fail;
	}
#endif

	எனில் (*p_pyhome != NUL)
	{
	    // The string must not change later, make a copy in static memory.
	    py_home_buf = (எழுத்து *)vim_strsave(p_pyhome);
	    எனில் (py_home_buf != வெற்று)
		Py_SetPythonHome(py_home_buf);
	}
#ifdef PYTHON_HOME
	இல்லையெனில் எனில் (mch_getenv((char_u *)"PYTHONHOME") == வெற்று)
	    Py_SetPythonHome(PYTHON_HOME);
#endif

	init_structs();

#எனில் defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02070000
	// Disable implicit 'import site', because it may cause Vim to exit
	// when it can't be found.
	Py_NoSiteFlag++;
#endif

	Py_Initialize();

#எனில் defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02070000
	// 'import site' explicitly.
	site = PyImport_ImportModule("site");
	எனில் (site == வெற்று)
	{
	    emsg(_(e_sorry_this_command_is_disabled_python_site_module_could_not_be_loaded));
	    செல் fail;
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

	எனில் (PythonIO_Init_io())
	    செல் fail;

	எனில் (PythonMod_Init())
	    செல் fail;

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
DoPyCommand(
    மாறிலி எழுத்து		*cmd,
    dict_T*		locals,
    rangeinitializer	init_range,
    runner		run,
    வெறுமை		*arg)
{
#ifndef PY_CAN_RECURSE
    நிலையான முழு		recursive = 0;
#endif
#எனில் defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    எழுத்து		*saved_locale;
#endif
#ifdef PY_CAN_RECURSE
    PyGILState_STATE	pygilstate;
#endif

#ifndef PY_CAN_RECURSE
    எனில் (recursive)
    {
	emsg(_(e_cannot_invoke_python_recursively));
	திரும்பு;
    }
    ++recursive;
#endif
    எனில் (python_end_called)
	திரும்பு;

    எனில் (Python_Init())
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
	saved_locale = (எழுத்து *) PY_STRSAVE(saved_locale);
	(வெறுமை)setlocale(LC_NUMERIC, "C");
    }
#endif

#ifdef PY_CAN_RECURSE
    pygilstate = PyGILState_Ensure();
#இல்லையெனில்
    Python_RestoreThread();	    // enter python
#endif

    run((எழுத்து *) cmd, locals, arg
#ifdef PY_CAN_RECURSE
	    , &pygilstate
#endif
	    );

#ifdef PY_CAN_RECURSE
    PyGILState_Release(pygilstate);
#இல்லையெனில்
    Python_SaveThread();	    // leave python
#endif

#எனில் defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    எனில் (saved_locale != வெற்று)
    {
	(வெறுமை)setlocale(LC_NUMERIC, saved_locale);
	PyMem_Free(saved_locale);
    }
#endif

    Python_Lock_Vim();		    // enter vim
    PythonIO_Flush();

theend:
#ifndef PY_CAN_RECURSE
    --recursive;
#endif
    திரும்பு;
}

/*
 * ":python"
 */
    வெறுமை
ex_python(exarg_T *eap)
{
    char_u *script;

    script = script_get(eap, eap->arg);
    எனில் (!eap->skip)
    {
	எனில் (p_pyx == 0)
	    p_pyx = 2;

	DoPyCommand(script == வெற்று ? (எழுத்து *) eap->arg : (எழுத்து *) script,
		வெற்று,
		init_range_cmd,
		(runner) run_cmd,
		(வெறுமை *) eap);
    }
    vim_free(script);
}

#define BUFFER_SIZE 1024

/*
 * ":pyfile"
 */
    வெறுமை
ex_pyfile(exarg_T *eap)
{
    நிலையான எழுத்து buffer[BUFFER_SIZE];
    மாறிலி எழுத்து *file = (எழுத்து *)eap->arg;
    எழுத்து *p;

    எனில் (p_pyx == 0)
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

    போது (*file && p < buffer + (BUFFER_SIZE - 3))
    {
	எனில் (*file == '\\' || *file == '\'')
	    *p++ = '\\';
	*p++ = *file++;
    }

    // If we didn't finish the file name, we hit a buffer overflow
    எனில் (*file != '\0')
	திரும்பு;

    // Put in the terminating "')" and a null
    *p++ = '\'';
    *p++ = ')';
    *p++ = '\0';

    // Execute the file
    DoPyCommand(buffer,
	    வெற்று,
	    init_range_cmd,
	    (runner) run_cmd,
	    (வெறுமை *) eap);
}

    வெறுமை
ex_pydo(exarg_T *eap)
{
    எனில் (p_pyx == 0)
	p_pyx = 2;

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
OutputGetattr(PyObject *self, எழுத்து *name)
{
    எனில் (strcmp(name, "softspace") == 0)
	திரும்பு PyInt_FromLong(((OutputObject *)(self))->softspace);
    இல்லையெனில் எனில் (strcmp(name, "__members__") == 0)
	திரும்பு ObjectDir(வெற்று, OutputAttrs);
    இல்லையெனில் எனில் (strcmp(name, "errors") == 0)
	திரும்பு PyString_FromString("strict");
    இல்லையெனில் எனில் (strcmp(name, "encoding") == 0)
	திரும்பு PyString_FromString(ENC_OPT);
    திரும்பு Py_FindMethod(OutputMethods, self, name);
}

///////////////////////////////////////////////////////
// 3. Implementation of the Vim module for Python

// Window type - Implementation functions
// --------------------------------------

#define WindowType_Check(obj) ((obj)->ob_type == &WindowType)

// Buffer type - Implementation functions
// --------------------------------------

#define BufferType_Check(obj) ((obj)->ob_type == &BufferType)

நிலையான முழு BufferAssItem(PyObject *, PyInt, PyObject *);
நிலையான முழு BufferAssSlice(PyObject *, PyInt, PyInt, PyObject *);

// Line range type - Implementation functions
// --------------------------------------

#define RangeType_Check(obj) ((obj)->ob_type == &RangeType)

நிலையான முழு RangeAssItem(PyObject *, PyInt, PyObject *);
நிலையான முழு RangeAssSlice(PyObject *, PyInt, PyInt, PyObject *);

// Current objects type - Implementation functions
// -----------------------------------------------

நிலையான PySequenceMethods BufferAsSeq = {
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

    நிலையான PyObject *
BufferGetattr(PyObject *self, எழுத்து *name)
{
    PyObject *r;

    எனில் ((r = BufferAttrValid((BufferObject *)(self), name)))
	திரும்பு r;

    எனில் (CheckBuffer((BufferObject *)(self)))
	திரும்பு வெற்று;

    r = BufferAttr((BufferObject *)(self), name);
    எனில் (r || PyErr_Occurred())
	திரும்பு r;
    இல்லையெனில்
	திரும்பு Py_FindMethod(BufferMethods, self, name);
}

//////////////////

    நிலையான முழு
BufferAssItem(PyObject *self, PyInt n, PyObject *val)
{
    திரும்பு RBAsItem((BufferObject *)(self), n, val, 1, -1, வெற்று);
}

    நிலையான முழு
BufferAssSlice(PyObject *self, PyInt lo, PyInt hi, PyObject *val)
{
    திரும்பு RBAsSlice((BufferObject *)(self), lo, hi, val, 1, -1, வெற்று);
}

நிலையான PySequenceMethods RangeAsSeq = {
    (PyInquiry)		RangeLength,	      // sq_length,    len(x)
    (binaryfunc)	0, /* RangeConcat, */ // sq_concat,    x+y
    (PyIntArgFunc)	0, /* RangeRepeat, */ // sq_repeat,    x*n
    (PyIntArgFunc)	RangeItem,	      // sq_item,      x[i]
    (PyIntIntArgFunc)	RangeSlice,	      // sq_slice,     x[i:j]
    (PyIntObjArgProc)	RangeAssItem,	      // sq_ass_item,  x[i]=v
    (PyIntIntObjArgProc) RangeAssSlice,	      // sq_ass_slice, x[i:j]=v
    (objobjproc)	0,
#எனில் PY_MAJOR_VERSION >= 2
    (binaryfunc)	0,
    0,
#endif
};

// Line range object - Implementation

    நிலையான PyObject *
RangeGetattr(PyObject *self, எழுத்து *name)
{
    எனில் (strcmp(name, "start") == 0)
	திரும்பு Py_Build_மதிப்பு(Py_ssize_t_fmt, ((RangeObject *)(self))->start - 1);
    இல்லையெனில் எனில் (strcmp(name, "end") == 0)
	திரும்பு Py_Build_மதிப்பு(Py_ssize_t_fmt, ((RangeObject *)(self))->end - 1);
    இல்லையெனில் எனில் (strcmp(name, "__members__") == 0)
	திரும்பு ObjectDir(வெற்று, RangeAttrs);
    இல்லையெனில்
	திரும்பு Py_FindMethod(RangeMethods, self, name);
}

////////////////

    நிலையான முழு
RangeAssItem(PyObject *self, PyInt n, PyObject *val)
{
    திரும்பு RBAsItem(((RangeObject *)(self))->buf, n, val,
		     ((RangeObject *)(self))->start,
		     ((RangeObject *)(self))->end,
		     &((RangeObject *)(self))->end);
}

    நிலையான முழு
RangeAssSlice(PyObject *self, PyInt lo, PyInt hi, PyObject *val)
{
    திரும்பு RBAsSlice(((RangeObject *)(self))->buf, lo, hi, val,
		      ((RangeObject *)(self))->start,
		      ((RangeObject *)(self))->end,
		      &((RangeObject *)(self))->end);
}

// TabPage object - Implementation

    நிலையான PyObject *
TabPageGetattr(PyObject *self, எழுத்து *name)
{
    PyObject *r;

    எனில் ((r = TabPageAttrValid((TabPageObject *)(self), name)))
	திரும்பு r;

    எனில் (CheckTabPage((TabPageObject *)(self)))
	திரும்பு வெற்று;

    r = TabPageAttr((TabPageObject *)(self), name);
    எனில் (r || PyErr_Occurred())
	திரும்பு r;
    இல்லையெனில்
	திரும்பு Py_FindMethod(TabPageMethods, self, name);
}

// Window object - Implementation

    நிலையான PyObject *
WindowGetattr(PyObject *self, எழுத்து *name)
{
    PyObject *r;

    எனில் ((r = WindowAttrValid((WindowObject *)(self), name)))
	திரும்பு r;

    எனில் (CheckWindow((WindowObject *)(self)))
	திரும்பு வெற்று;

    r = WindowAttr((WindowObject *)(self), name);
    எனில் (r || PyErr_Occurred())
	திரும்பு r;
    இல்லையெனில்
	திரும்பு Py_FindMethod(WindowMethods, self, name);
}

// Tab page list object - Definitions

நிலையான PySequenceMethods TabListAsSeq = {
    (PyInquiry)		TabListLength,	    // sq_length,    len(x)
    (binaryfunc)	0,		    // sq_concat,    x+y
    (PyIntArgFunc)	0,		    // sq_repeat,    x*n
    (PyIntArgFunc)	TabListItem,	    // sq_item,      x[i]
    (PyIntIntArgFunc)	0,		    // sq_slice,     x[i:j]
    (PyIntObjArgProc)	0,		    // sq_ass_item,  x[i]=v
    (PyIntIntObjArgProc) 0,		    // sq_ass_slice, x[i:j]=v
    (objobjproc)	0,
#எனில் PY_MAJOR_VERSION >= 2
    (binaryfunc)	0,
    0,
#endif
};

// Window list object - Definitions

நிலையான PySequenceMethods WinListAsSeq = {
    (PyInquiry)		WinListLength,	    // sq_length,    len(x)
    (binaryfunc)	0,		    // sq_concat,    x+y
    (PyIntArgFunc)	0,		    // sq_repeat,    x*n
    (PyIntArgFunc)	WinListItem,	    // sq_item,      x[i]
    (PyIntIntArgFunc)	0,		    // sq_slice,     x[i:j]
    (PyIntObjArgProc)	0,		    // sq_ass_item,  x[i]=v
    (PyIntIntObjArgProc) 0,		    // sq_ass_slice, x[i:j]=v
    (objobjproc)	0,
#எனில் PY_MAJOR_VERSION >= 2
    (binaryfunc)	0,
    0,
#endif
};

// External interface

    வெறுமை
python_buffer_free(buf_T *buf)
{
    BufferObject *bp = BUF_PYTHON_REF(buf);
    எனில் (bp == வெற்று)
	திரும்பு;
    bp->buf = INVALID_BUFFER_மதிப்பு;
    BUF_PYTHON_REF(buf) = வெற்று;
}

    வெறுமை
python_window_free(win_T *win)
{
    WindowObject *wp = WIN_PYTHON_REF(win);
    எனில் (wp == வெற்று)
	திரும்பு;
    wp->win = INVALID_WINDOW_மதிப்பு;
    WIN_PYTHON_REF(win) = வெற்று;
}

    வெறுமை
python_tabpage_free(tabpage_T *tab)
{
    TabPageObject *tp = TAB_PYTHON_REF(tab);
    எனில் (tp == வெற்று)
	திரும்பு;
    tp->tab = INVALID_TABPAGE_மதிப்பு;
    TAB_PYTHON_REF(tab) = வெற்று;
}

    நிலையான முழு
PythonMod_Init(வெறுமை)
{
    // The special value is removed from sys.path in Python_Init().
    நிலையான எழுத்து	*(argv[2]) = {"/must>not&exist/foo", வெற்று};

    எனில் (init_types())
	திரும்பு -1;

    // Set sys.argv[] to avoid a crash in warn().
    PySys_SetArgv(1, argv);

    vim_module = Py_InitModule4("vim", VimMethods, (எழுத்து *)வெற்று,
				(PyObject *)வெற்று, PYTHON_API_VERSION);

    எனில் (populate_module(vim_module))
	திரும்பு -1;

    எனில் (init_sys_path())
	திரும்பு -1;

    திரும்பு 0;
}

//////////////////////////////////////////////////////////////////////////
// 4. Utility functions for handling the interface between Vim and Python.

// Convert a Vim line into a Python string.
// All internal newlines are replaced by null characters.
//
// On errors, the Python exception data is set, and NULL is returned.
    நிலையான PyObject *
வரி_To_String(மாறிலி எழுத்து *str)
{
    PyObject *result;
    PyInt len = strlen(str);
    எழுத்து *p;

    // Allocate a Python string object, with uninitialised contents. We
    // must do it this way, so that we can modify the string in place
    // later. See the Python source, Objects/stringobject.c for details.
    result = PyString_FromStringAndSize(வெற்று, len);
    எனில் (result == வெற்று)
	திரும்பு வெற்று;

    p = PyString_AsString(result);

    போது (*str)
    {
	எனில் (*str == '\n')
	    *p = '\0';
	இல்லையெனில்
	    *p = *str;

	++p;
	++str;
    }

    திரும்பு result;
}

    நிலையான PyObject *
DictionaryGetattr(PyObject *self, எழுத்து *name)
{
    DictionaryObject	*this = ((DictionaryObject *) (self));

    எனில் (strcmp(name, "locked") == 0)
	திரும்பு PyInt_FromLong(this->dict->dv_lock);
    இல்லையெனில் எனில் (strcmp(name, "scope") == 0)
	திரும்பு PyInt_FromLong(this->dict->dv_scope);
    இல்லையெனில் எனில் (strcmp(name, "__members__") == 0)
	திரும்பு ObjectDir(வெற்று, DictionaryAttrs);

    திரும்பு Py_FindMethod(DictionaryMethods, self, name);
}

    நிலையான PyObject *
ListGetattr(PyObject *self, எழுத்து *name)
{
    எனில் (strcmp(name, "locked") == 0)
	திரும்பு PyInt_FromLong(((ListObject *)(self))->list->lv_lock);
    இல்லையெனில் எனில் (strcmp(name, "__members__") == 0)
	திரும்பு ObjectDir(வெற்று, ListAttrs);

    திரும்பு Py_FindMethod(ListMethods, self, name);
}

    நிலையான PyObject *
TupleGetattr(PyObject *self, எழுத்து *name)
{
    எனில் (strcmp(name, "locked") == 0)
	திரும்பு PyInt_FromLong(((TupleObject *)(self))->tuple->tv_lock);
    இல்லையெனில் எனில் (strcmp(name, "__members__") == 0)
	திரும்பு ObjectDir(வெற்று, TupleAttrs);

    திரும்பு Py_FindMethod(TupleMethods, self, name);
}

    நிலையான PyObject *
FunctionGetattr(PyObject *self, எழுத்து *name)
{
    PyObject	*r;

    r = FunctionAttr((FunctionObject *)(self), name);

    எனில் (r || PyErr_Occurred())
	திரும்பு r;
    இல்லையெனில்
	திரும்பு Py_FindMethod(FunctionMethods, self, name);
}

    வெறுமை
do_pyeval(char_u *str, dict_T *locals, typval_T *rettv)
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
set_ref_in_python(முழு copyID)
{
    திரும்பு set_ref_in_py(copyID);
}
