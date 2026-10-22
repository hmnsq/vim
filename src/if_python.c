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

#अन्तर्भाव "vim.h"

#अन्तर्भाव <limits.h>

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

#यदि defined(MSWIN) && defined(HAVE_FCNTL_H)
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

#अन्तर्भाव <Python.h>

#यदि !defined(PY_VERSION_HEX) || PY_VERSION_HEX < 0x02050000
# undef PY_SSIZE_T_CLEAN
#endif

// these are NULL for Python 2
#define ERRORS_DECODE_ARG रिक्त
#define ERRORS_ENCODE_ARG ERRORS_DECODE_ARG

#undef main // Defined in python.h - aargh
#undef HAVE_FCNTL_H // Clash with os_win32.h

// Perhaps leave this out for Python 2.6, which supports bytes?
#define PyBytes_FromString      PyString_FromString
#define PyBytes_Check		PyString_Check
#define PyBytes_AsStringAndSize PyString_AsStringAndSize
#define PyBytes_FromStringAndSize   PyString_FromStringAndSize

#यदि defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02070000
# define PY_USE_CAPSULE
#endif

#यदि defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02050000
# define PyInt Py_ssize_t
# define PyInquiry lenfunc
# define PyIntArgFunc ssizeargfunc
# define PyIntIntArgFunc ssizessizeargfunc
# define PyIntObjArgProc ssizeobjargproc
# define PyIntIntObjArgProc ssizessizeobjargproc
# define Py_ssize_t_fmt "n"
#अन्यथा
# define PyInt पूर्णाङ्क
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
#define single_निवेश	256
#define file_निवेश	257
#define eval_निवेश	258

#यदि defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x020300F0
  // Python 2.3: can invoke ":python" recursively.
# define PY_CAN_RECURSE
#endif

#यदि defined(DYNAMIC_PYTHON)
# ifndef DYNAMIC_PYTHON
#  define HINSTANCE long_u		// for generating prototypes
# endif

# ifdef MSWIN
#  define load_dll vimLoadLib
#  define close_dll FreeLibrary
#  define प्रतीक_from_dll GetProcAddress
#  define load_dll_error GetWin32Error
# अन्यथा
#  include <dlfcn.h>
#  define FARPROC शून्य*
#  define HINSTANCE शून्य*
#  यदि defined(PY_NO_RTLD_GLOBAL) && defined(PY3_NO_RTLD_GLOBAL)
#   define load_dll(n) dlopen((n), RTLD_LAZY)
#  अन्यथा
#   define load_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#  endif
#  define close_dll dlclose
#  define प्रतीक_from_dll dlsym
#  define load_dll_error dlerror
# endif

// This makes if_python.c compile without warnings against Python 2.5
// on Win32 and Win64.
# undef PyRun_SimpleString
# undef PyRun_String
# undef PyArg_Parse
# undef PyArg_ParseTuple
# undef Py_Build_मूल्य
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
# define Py_List_जोड़ो dll_Py_List_जोड़ो
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
# ifdef Py_Mapping_कुंजियाँ
#  define PY_NO_MAPPING_कुंजियाँ
# अन्यथा
#  define Py_Mapping_कुंजियाँ dll_Py_Mapping_कुंजियाँ
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
# define Py_संख्या_Check dll_Py_संख्या_Check
# define Py_संख्या_Long dll_Py_संख्या_Long
# define PyImport_AddModule (*dll_PyImport_AddModule)
# define PySys_SetObject dll_PySys_SetObject
# define PySys_GetObject dll_PySys_GetObject
# define PySys_SetArgv dll_PySys_SetArgv
# define PyType_Type (*dll_PyType_Type)
# define PyFile_Type (*dll_PyFile_Type)
# define PySlice_Type (*dll_PySlice_Type)
# define PyType_Ready (*dll_PyType_Ready)
# define PyType_GenericAlloc dll_PyType_GenericAlloc
# define Py_Build_मूल्य dll_Py_Build_मूल्य
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
# अन्यथा
#  define PyObject_GC_Del dll_PyObject_GC_Del
#  define PyObject_GC_UnTrack dll_PyObject_GC_UnTrack
# endif
# यदि defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02070000
#  define _PyObject_NextNotImplemented (*dll__PyObject_NextNotImplemented)
# endif
# define _Py_NoneStruct (*dll__Py_NoneStruct)
# define _Py_ZeroStruct (*dll__Py_ZeroStruct)
# define _Py_TrueStruct (*dll__Py_TrueStruct)
# define PyObject_Init dll__PyObject_Init
# define PyObject_GetIter dll_PyObject_GetIter
# define PyObject_IsTrue dll_PyObject_IsTrue
# यदि defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02020000
#  define PyType_IsSubtype dll_PyType_IsSubtype
#  ifdef Py_DEBUG
#   define _Py_NegativeRefcount dll__Py_NegativeRefcount
#   define _Py_RefTotal (*dll__Py_RefTotal)
#   define _Py_Dealloc dll__Py_Dealloc
#  endif
# endif
# यदि defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02030000
#  यदि defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
#   define _PyObject_DebugMalloc dll__PyObject_DebugMalloc
#   define _PyObject_DebugFree dll__PyObject_DebugFree
#  अन्यथा
#   define PyObject_Malloc dll_PyObject_Malloc
#   define PyObject_Free dll_PyObject_Free
#  endif
# endif
# ifdef PY_USE_CAPSULE
#  define PyCapsule_New dll_PyCapsule_New
#  define PyCapsule_GetPointer dll_PyCapsule_GetPointer
# अन्यथा
#  define PyCObject_FromVoidPtr dll_PyCObject_FromVoidPtr
#  define PyCObject_AsVoidPtr dll_PyCObject_AsVoidPtr
# endif
# यदि defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02070000
#  define Py_NoSiteFlag (*dll_Py_NoSiteFlag)
# endif

/*
 * Pointers for dynamic link
 */
स्थिर पूर्णाङ्क(*dll_PyArg_Parse)(PyObject *, वर्ण *, ...);
स्थिर पूर्णाङ्क(*dll_PyArg_ParseTuple)(PyObject *, वर्ण *, ...);
स्थिर शून्य(*dll_PyMem_Free)(शून्य *);
स्थिर शून्य* (*dll_PyMem_Malloc)(size_t);
स्थिर पूर्णाङ्क(*dll_PyDict_SetItemString)(PyObject *dp, वर्ण *कुंजी, PyObject *item);
स्थिर पूर्णाङ्क(*dll_PyErr_BadArgument)(शून्य);
स्थिर PyObject *(*dll_PyErr_NewException)(वर्ण *, PyObject *, PyObject *);
स्थिर शून्य(*dll_PyErr_Clear)(शून्य);
स्थिर PyObject*(*dll_PyErr_Format)(PyObject *, ध्रुव वर्ण *, ...);
स्थिर शून्य(*dll_PyErr_PrintEx)(पूर्णाङ्क);
स्थिर PyObject*(*dll_PyErr_NoMemory)(शून्य);
स्थिर PyObject*(*dll_PyErr_Occurred)(शून्य);
स्थिर शून्य(*dll_PyErr_SetNone)(PyObject *);
स्थिर शून्य(*dll_PyErr_SetString)(PyObject *, ध्रुव वर्ण *);
स्थिर शून्य(*dll_PyErr_SetObject)(PyObject *, PyObject *);
स्थिर पूर्णाङ्क(*dll_PyErr_ExceptionMatches)(PyObject *);
स्थिर शून्य(*dll_PyEval_InitThreads)(शून्य);
स्थिर शून्य(*dll_PyEval_RestoreThread)(PyThreadState *);
स्थिर PyThreadState*(*dll_PyEval_SaveThread)(शून्य);
# ifdef PY_CAN_RECURSE
स्थिर PyGILState_STATE	(*dll_PyGILState_Ensure)(शून्य);
स्थिर शून्य (*dll_PyGILState_Release)(PyGILState_STATE);
# endif
स्थिर long(*dll_PyInt_AsLong)(PyObject *);
स्थिर PyObject*(*dll_PyInt_FromLong)(long);
स्थिर long(*dll_PyLong_AsLong)(PyObject *);
स्थिर PyObject*(*dll_PyLong_FromLong)(long);
स्थिर PyTypeObject* dll_PyBool_Type;
स्थिर PyTypeObject* dll_PyInt_Type;
स्थिर PyTypeObject* dll_PyLong_Type;
स्थिर PyObject*(*dll_PyList_GetItem)(PyObject *, PyInt);
स्थिर पूर्णाङ्क(*dll_PyList_Append)(PyObject *, PyObject *);
स्थिर पूर्णाङ्क(*dll_Py_List_जोड़ो)(PyObject *, PyInt, PyObject *);
स्थिर PyObject*(*dll_PyList_New)(PyInt size);
स्थिर पूर्णाङ्क(*dll_PyList_SetItem)(PyObject *, PyInt, PyObject *);
स्थिर PyInt(*dll_PyList_Size)(PyObject *);
स्थिर PyTypeObject* dll_PyList_Type;
स्थिर PyObject*(*dll_PyTuple_GetItem)(PyObject *, PyInt);
स्थिर पूर्णाङ्क(*dll_PyTuple_SetItem)(PyObject *, PyInt, PyObject *);
स्थिर PyObject*(*dll_PyTuple_New)(PyInt size);
स्थिर PyInt(*dll_PyTuple_Size)(PyObject *);
स्थिर PyTypeObject* dll_PyTuple_Type;
स्थिर पूर्णाङ्क (*dll_PySequence_Check)(PyObject *);
स्थिर PyInt(*dll_PySequence_Size)(PyObject *);
स्थिर PyObject*(*dll_PySequence_GetItem)(PyObject *, PyInt);
स्थिर PyObject*(*dll_PySequence_Fast)(PyObject *, ध्रुव वर्ण *);
स्थिर पूर्णाङ्क (*dll_PySlice_GetIndicesEx)(PySliceObject *r, PyInt length,
		     PyInt *start, PyInt *stop, PyInt *step,
		     PyInt *slicelen);
स्थिर PyObject*(*dll_PyImport_ImportModule)(ध्रुव वर्ण *);
स्थिर PyObject*(*dll_PyDict_New)(शून्य);
स्थिर PyObject*(*dll_PyDict_GetItemString)(PyObject *, ध्रुव वर्ण *);
स्थिर पूर्णाङ्क (*dll_PyDict_Next)(PyObject *, PyInt *, PyObject **, PyObject **);
स्थिर PyTypeObject* dll_PyDict_Type;
# ifndef PY_NO_MAPPING_कुंजियाँ
स्थिर PyObject* (*dll_Py_Mapping_कुंजियाँ)(PyObject *);
# endif
स्थिर PyObject* (*dll_PyObject_GetItem)(PyObject *, PyObject *);
स्थिर PyObject* (*dll_PyObject_CallMethod)(PyObject *, वर्ण *, PyObject *);
स्थिर पूर्णाङ्क (*dll_PyMapping_Check)(PyObject *);
स्थिर PyObject* (*dll_PyIter_Next)(PyObject *);
स्थिर PyObject*(*dll_PyModule_GetDict)(PyObject *);
स्थिर पूर्णाङ्क(*dll_PyModule_AddObject)(PyObject *, ध्रुव वर्ण *, PyObject *);
स्थिर पूर्णाङ्क(*dll_PyRun_SimpleString)(वर्ण *);
स्थिर PyObject *(*dll_PyRun_String)(वर्ण *, पूर्णाङ्क, PyObject *, PyObject *);
स्थिर PyObject* (*dll_PyObject_GetAttrString)(PyObject *, ध्रुव वर्ण *);
स्थिर पूर्णाङ्क (*dll_PyObject_HasAttrString)(PyObject *, ध्रुव वर्ण *);
स्थिर पूर्णाङ्क (*dll_PyObject_SetAttrString)(PyObject *, ध्रुव वर्ण *, PyObject *);
स्थिर PyObject* (*dll_PyObject_CallFunctionObjArgs)(PyObject *, ...);
स्थिर PyObject* (*dll_PyObject_CallFunction)(PyObject *, वर्ण *, ...);
स्थिर PyObject* (*dll_PyObject_Call)(PyObject *, PyObject *, PyObject *);
स्थिर PyObject* (*dll_PyObject_Repr)(PyObject *);
स्थिर वर्ण*(*dll_PyString_AsString)(PyObject *);
स्थिर पूर्णाङ्क(*dll_PyString_AsStringAndSize)(PyObject *, वर्ण **, PyInt *);
स्थिर PyObject*(*dll_PyString_FromString)(ध्रुव वर्ण *);
स्थिर PyObject*(*dll_PyString_FromFormat)(ध्रुव वर्ण *, ...);
स्थिर PyObject*(*dll_PyString_FromStringAndSize)(ध्रुव वर्ण *, PyInt);
स्थिर PyInt(*dll_PyString_Size)(PyObject *);
स्थिर PyTypeObject* dll_PyString_Type;
स्थिर PyTypeObject* dll_PyUnicode_Type;
स्थिर PyObject *(*py_PyUnicode_AsEncodedString)(PyObject *, वर्ण *, वर्ण *);
स्थिर द्विगुण(*dll_PyFloat_AsDouble)(PyObject *);
स्थिर PyObject*(*dll_PyFloat_FromDouble)(द्विगुण);
स्थिर PyTypeObject* dll_PyFloat_Type;
स्थिर पूर्णाङ्क(*dll_Py_संख्या_Check)(PyObject *);
स्थिर PyObject*(*dll_Py_संख्या_Long)(PyObject *);
स्थिर पूर्णाङ्क(*dll_PySys_SetObject)(वर्ण *, PyObject *);
स्थिर PyObject *(*dll_PySys_GetObject)(वर्ण *);
स्थिर पूर्णाङ्क(*dll_PySys_SetArgv)(पूर्णाङ्क, वर्ण **);
स्थिर PyTypeObject* dll_PyType_Type;
स्थिर PyTypeObject* dll_PyFile_Type;
स्थिर PyTypeObject* dll_PySlice_Type;
स्थिर पूर्णाङ्क (*dll_PyType_Ready)(PyTypeObject *type);
स्थिर PyObject* (*dll_PyType_GenericAlloc)(PyTypeObject *type, PyInt nitems);
स्थिर PyObject*(*dll_Py_Build_मूल्य)(वर्ण *, ...);
स्थिर PyObject*(*dll_Py_FindMethod)(संरचना PyMethodDef[], PyObject *, वर्ण *);
स्थिर PyObject*(*dll_Py_InitModule4)(वर्ण *, संरचना PyMethodDef *, वर्ण *, PyObject *, पूर्णाङ्क);
स्थिर PyObject*(*dll_PyImport_AddModule)(वर्ण *);
स्थिर शून्य(*dll_Py_SetPythonHome)(वर्ण *home);
स्थिर शून्य(*dll_Py_Initialize)(शून्य);
स्थिर शून्य(*dll_Py_Finalize)(शून्य);
स्थिर पूर्णाङ्क(*dll_Py_IsInitialized)(शून्य);
स्थिर PyObject*(*dll__PyObject_New)(PyTypeObject *, PyObject *);
स्थिर PyObject*(*dll__PyObject_GC_New)(PyTypeObject *);
# ifdef Py_underscore_GC
स्थिर शून्य(*dll__PyObject_GC_Del)(शून्य *);
स्थिर शून्य(*dll__PyObject_GC_UnTrack)(शून्य *);
# अन्यथा
स्थिर शून्य(*dll_PyObject_GC_Del)(शून्य *);
स्थिर शून्य(*dll_PyObject_GC_UnTrack)(शून्य *);
# endif
स्थिर PyObject*(*dll__PyObject_Init)(PyObject *, PyTypeObject *);
स्थिर PyObject* (*dll_PyObject_GetIter)(PyObject *);
स्थिर पूर्णाङ्क (*dll_PyObject_IsTrue)(PyObject *);
# यदि defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02070000
स्थिर iternextfunc dll__PyObject_NextNotImplemented;
# endif
स्थिर PyObject* dll__Py_NoneStruct;
स्थिर PyObject* _Py_ZeroStruct;
स्थिर PyObject* dll__Py_TrueStruct;
# यदि defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02020000
स्थिर पूर्णाङ्क (*dll_PyType_IsSubtype)(PyTypeObject *, PyTypeObject *);
#  ifdef Py_DEBUG
स्थिर शून्य (*dll__Py_NegativeRefcount)(ध्रुव वर्ण *fname, पूर्णाङ्क lineno, PyObject *op);
स्थिर PyInt* dll__Py_RefTotal;
स्थिर शून्य (*dll__Py_Dealloc)(PyObject *obj);
#  endif
# endif
# यदि defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02030000
#  यदि defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
स्थिर शून्य (*dll__PyObject_DebugFree)(शून्य*);
स्थिर शून्य* (*dll__PyObject_DebugMalloc)(size_t);
#  अन्यथा
स्थिर शून्य* (*dll_PyObject_Malloc)(size_t);
स्थिर शून्य (*dll_PyObject_Free)(शून्य*);
#  endif
# endif
# ifdef PY_USE_CAPSULE
स्थिर PyObject* (*dll_PyCapsule_New)(शून्य *, वर्ण *, PyCapsule_Destructor);
स्थिर शून्य* (*dll_PyCapsule_GetPointer)(PyObject *, वर्ण *);
# अन्यथा
स्थिर PyObject* (*dll_PyCObject_FromVoidPtr)(शून्य *cobj, शून्य (*destr)(शून्य *));
स्थिर शून्य* (*dll_PyCObject_AsVoidPtr)(PyObject *);
# endif
# यदि defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02070000
स्थिर पूर्णाङ्क* dll_Py_NoSiteFlag;
# endif

स्थिर HINSTANCE hinstPython = 0; // Instance of python.dll

// Imported exception objects
स्थिर PyObject *imp_PyExc_AttributeError;
स्थिर PyObject *imp_PyExc_IndexError;
स्थिर PyObject *imp_Py_Exc_कुंजी_Error;
स्थिर PyObject *imp_PyExc_KeyboardInterrupt;
स्थिर PyObject *imp_PyExc_TypeError;
स्थिर PyObject *imp_Py_Exc_मूल्य_Error;
स्थिर PyObject *imp_PyExc_SystemExit;
स्थिर PyObject *imp_PyExc_RuntimeError;
स्थिर PyObject *imp_PyExc_ImportError;
स्थिर PyObject *imp_PyExc_OverflowError;

# define PyExc_AttributeError imp_PyExc_AttributeError
# define PyExc_IndexError imp_PyExc_IndexError
# define Py_Exc_कुंजी_Error imp_Py_Exc_कुंजी_Error
# define PyExc_KeyboardInterrupt imp_PyExc_KeyboardInterrupt
# define PyExc_TypeError imp_PyExc_TypeError
# define Py_Exc_मूल्य_Error imp_Py_Exc_मूल्य_Error
# define PyExc_SystemExit imp_PyExc_SystemExit
# define PyExc_RuntimeError imp_PyExc_RuntimeError
# define PyExc_ImportError imp_PyExc_ImportError
# define PyExc_OverflowError imp_PyExc_OverflowError

/*
 * Table of name to function pointer of python.
 */
# define PYTHON_PROC FARPROC
स्थिर संरचना
{
    वर्ण *name;
    PYTHON_PROC *ptr;
} python_funcname_table[] =
{
# ifdef PY_SSIZE_T_CLEAN
    {"_PyArg_Parse_SizeT", (PYTHON_PROC*)&dll_PyArg_Parse},
    {"_PyArg_ParseTuple_SizeT", (PYTHON_PROC*)&dll_PyArg_ParseTuple},
    {"_Py_BuildValue_SizeT", (PYTHON_PROC*)&dll_Py_Build_मूल्य},
# अन्यथा
    {"PyArg_Parse", (PYTHON_PROC*)&dll_PyArg_Parse},
    {"PyArg_ParseTuple", (PYTHON_PROC*)&dll_PyArg_ParseTuple},
    {"Py_BuildValue", (PYTHON_PROC*)&dll_Py_Build_मूल्य},
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
    {"PyList_Insert", (PYTHON_PROC*)&dll_Py_List_जोड़ो},
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
# ifndef PY_NO_MAPPING_कुंजियाँ
    {"PyMapping_Keys", (PYTHON_PROC*)&dll_Py_Mapping_कुंजियाँ},
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
    {"PyNumber_Check", (PYTHON_PROC*)&dll_Py_संख्या_Check},
    {"PyNumber_Long", (PYTHON_PROC*)&dll_Py_संख्या_Long},
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
# अन्यथा
    {"PyObject_GC_Del", (PYTHON_PROC*)&dll_PyObject_GC_Del},
    {"PyObject_GC_UnTrack", (PYTHON_PROC*)&dll_PyObject_GC_UnTrack},
# endif
    {"PyObject_Init", (PYTHON_PROC*)&dll__PyObject_Init},
    {"PyObject_GetIter", (PYTHON_PROC*)&dll_PyObject_GetIter},
    {"PyObject_IsTrue", (PYTHON_PROC*)&dll_PyObject_IsTrue},
# यदि defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02070000
    {"_PyObject_NextNotImplemented", (PYTHON_PROC*)&dll__PyObject_NextNotImplemented},
# endif
    {"_Py_NoneStruct", (PYTHON_PROC*)&dll__Py_NoneStruct},
    {"_Py_ZeroStruct", (PYTHON_PROC*)&dll__Py_ZeroStruct},
    {"_Py_TrueStruct", (PYTHON_PROC*)&dll__Py_TrueStruct},
# यदि defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02020000
#  ifdef Py_DEBUG
    {"_Py_NegativeRefcount", (PYTHON_PROC*)&dll__Py_NegativeRefcount},
    {"_Py_RefTotal", (PYTHON_PROC*)&dll__Py_RefTotal},
    {"_Py_Dealloc", (PYTHON_PROC*)&dll__Py_Dealloc},
#  endif
    {"PyType_IsSubtype", (PYTHON_PROC*)&dll_PyType_IsSubtype},
# endif
# यदि defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02030000
#  यदि defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
    {"_PyObject_DebugFree", (PYTHON_PROC*)&dll__PyObject_DebugFree},
    {"_PyObject_DebugMalloc", (PYTHON_PROC*)&dll__PyObject_DebugMalloc},
#  अन्यथा
    {"PyObject_Malloc", (PYTHON_PROC*)&dll_PyObject_Malloc},
    {"PyObject_Free", (PYTHON_PROC*)&dll_PyObject_Free},
#  endif
# endif
# यदि defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02050000 \
	&& SIZEOF_SIZE_T != VIM_SIZEOF_INT
#  ifdef Py_DEBUG
    {"Py_InitModule4TraceRefs_64", (PYTHON_PROC*)&dll_Py_InitModule4},
#  अन्यथा
    {"Py_InitModule4_64", (PYTHON_PROC*)&dll_Py_InitModule4},
#  endif
# अन्यथा
#  ifdef Py_DEBUG
    {"Py_InitModule4TraceRefs", (PYTHON_PROC*)&dll_Py_InitModule4},
#  अन्यथा
    {"Py_InitModule4", (PYTHON_PROC*)&dll_Py_InitModule4},
#  endif
# endif
# ifdef PY_USE_CAPSULE
    {"PyCapsule_New", (PYTHON_PROC*)&dll_PyCapsule_New},
    {"PyCapsule_GetPointer", (PYTHON_PROC*)&dll_PyCapsule_GetPointer},
# अन्यथा
    {"PyCObject_FromVoidPtr", (PYTHON_PROC*)&dll_PyCObject_FromVoidPtr},
    {"PyCObject_AsVoidPtr", (PYTHON_PROC*)&dll_PyCObject_AsVoidPtr},
# endif
# यदि defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02070000
    {"Py_NoSiteFlag", (PYTHON_PROC*)&dll_Py_NoSiteFlag},
# endif
    {"", रिक्त},
};

/*
 * Load library and get all pointers.
 * Parameter 'libname' provides name of DLL.
 * Return OK or FAIL.
 */
    स्थिर पूर्णाङ्क
python_runtime_link_init(वर्ण *libname, पूर्णाङ्क verbose)
{
    पूर्णाङ्क i;
    PYTHON_PROC *ucs_as_encoded_string =
				   (PYTHON_PROC*)&py_PyUnicode_AsEncodedString;

# यदि !(defined(PY_NO_RTLD_GLOBAL) && defined(PY3_NO_RTLD_GLOBAL)) && defined(UNIX) && defined(FEAT_PYTHON3)
    // Can't have Python and Python3 loaded at the same time.
    // It causes a crash, because RTLD_GLOBAL is needed for
    // standard C extension libraries of one or both python versions.
    यदि (python3_loaded())
    {
	यदि (verbose)
	    emsg(_(e_this_vim_cannot_execute_python_after_using_py3));
	प्रत्यागम FAIL;
    }
# endif

    यदि (hinstPython)
	प्रत्यागम OK;
    hinstPython = load_dll(libname);
    यदि (!hinstPython)
    {
	यदि (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	प्रत्यागम FAIL;
    }

    प्रति (i = 0; python_funcname_table[i].ptr; ++i)
    {
	यदि ((*python_funcname_table[i].ptr = प्रतीक_from_dll(hinstPython,
			python_funcname_table[i].name)) == रिक्त)
	{
	    close_dll(hinstPython);
	    hinstPython = 0;
	    यदि (verbose)
		semsg(_(e_could_not_load_library_function_str), python_funcname_table[i].name);
	    प्रत्यागम FAIL;
	}
    }

    // Load unicode functions separately as only the ucs2 or the ucs4 functions
    // will be present in the library.
    *ucs_as_encoded_string = प्रतीक_from_dll(hinstPython,
					     "PyUnicodeUCS2_AsEncodedString");
    यदि (*ucs_as_encoded_string == रिक्त)
	*ucs_as_encoded_string = प्रतीक_from_dll(hinstPython,
					     "PyUnicodeUCS4_AsEncodedString");
    यदि (*ucs_as_encoded_string == रिक्त)
    {
	close_dll(hinstPython);
	hinstPython = 0;
	यदि (verbose)
	    semsg(_(e_could_not_load_library_function_str), "PyUnicode_UCSX_*");
	प्रत्यागम FAIL;
    }

    प्रत्यागम OK;
}

/*
 * If python is enabled (there is installed python on Windows system) return
 * TRUE, else FALSE.
 */
    पूर्णाङ्क
python_enabled(पूर्णाङ्क verbose)
{
    प्रत्यागम python_runtime_link_init((वर्ण *)p_pydll, verbose) == OK;
}

/*
 * Load the standard Python exceptions - don't import the symbols from the
 * DLL, as this can cause errors (importing data symbols is not reliable).
 */
    स्थिर शून्य
get_exceptions(शून्य)
{
    PyObject *exmod = PyImport_ImportModule("exceptions");
    PyObject *exdict = PyModule_GetDict(exmod);
    imp_PyExc_AttributeError = PyDict_GetItemString(exdict, "AttributeError");
    imp_PyExc_IndexError = PyDict_GetItemString(exdict, "IndexError");
    imp_Py_Exc_कुंजी_Error = PyDict_GetItemString(exdict, "KeyError");
    imp_PyExc_KeyboardInterrupt = PyDict_GetItemString(exdict, "KeyboardInterrupt");
    imp_PyExc_TypeError = PyDict_GetItemString(exdict, "TypeError");
    imp_Py_Exc_मूल्य_Error = PyDict_GetItemString(exdict, "ValueError");
    imp_PyExc_SystemExit = PyDict_GetItemString(exdict, "SystemExit");
    imp_PyExc_RuntimeError = PyDict_GetItemString(exdict, "RuntimeError");
    imp_PyExc_ImportError = PyDict_GetItemString(exdict, "ImportError");
    imp_PyExc_OverflowError = PyDict_GetItemString(exdict, "OverflowError");
    Py_XINCREF(imp_PyExc_AttributeError);
    Py_XINCREF(imp_PyExc_IndexError);
    Py_XINCREF(imp_Py_Exc_कुंजी_Error);
    Py_XINCREF(imp_PyExc_KeyboardInterrupt);
    Py_XINCREF(imp_PyExc_TypeError);
    Py_XINCREF(imp_Py_Exc_मूल्य_Error);
    Py_XINCREF(imp_PyExc_SystemExit);
    Py_XINCREF(imp_PyExc_RuntimeError);
    Py_XINCREF(imp_PyExc_ImportError);
    Py_XINCREF(imp_PyExc_OverflowError);
    Py_XDECREF(exmod);
}
#endif // DYNAMIC_PYTHON

स्थिर पूर्णाङ्क initialised = 0;
#define PYINITIALISED initialised
स्थिर पूर्णाङ्क python_end_called = FALSE;

#define DESTRUCTOR_FINISH(self) self->ob_type->tp_free((PyObject*)self);

#define WIN_PYTHON_REF(win) win->w_python_ref
#define BUF_PYTHON_REF(buf) buf->b_python_ref
#define TAB_PYTHON_REF(tab) tab->tp_python_ref

स्थिर PyObject *OutputGetattr(PyObject *, वर्ण *);
स्थिर PyObject *BufferGetattr(PyObject *, वर्ण *);
स्थिर PyObject *WindowGetattr(PyObject *, वर्ण *);
स्थिर PyObject *TabPageGetattr(PyObject *, वर्ण *);
स्थिर PyObject *RangeGetattr(PyObject *, वर्ण *);
स्थिर PyObject *DictionaryGetattr(PyObject *, वर्ण*);
स्थिर PyObject *ListGetattr(PyObject *, वर्ण *);
स्थिर PyObject *TupleGetattr(PyObject *, वर्ण *);
स्थिर PyObject *FunctionGetattr(PyObject *, वर्ण *);

#ifndef Py_VISIT
# define Py_VISIT(obj) visit(obj, arg)
#endif
#ifndef Py_CLEAR
# define Py_CLEAR(obj) \
    { \
	Py_XDECREF(obj); \
	obj = रिक्त; \
    }
#endif

#यदि defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    स्थिर शून्य *
py_memsave(शून्य *p, size_t len)
{
    शून्य	*r;

    यदि (!(r = PyMem_Malloc(len)))
	प्रत्यागम रिक्त;
    mch_memmove(r, p, len);
    प्रत्यागम r;
}

# define PY_STRSAVE(s) ((char_u *) py_memsave(s, STRLEN(s) + 1))
#endif

प्रकारनिर्देश PySliceObject PySliceObject_T;

/*
 * Include the code shared with if_python3.c
 */
#अन्तर्भाव "if_py_both.h"


///////////////////////////////////////////////////////
// Internal function prototypes.

स्थिर पूर्णाङ्क PythonMod_Init(शून्य);


///////////////////////////////////////////////////////
// 1. Python interpreter main program.

#ifndef PY_CAN_RECURSE
स्थिर PyThreadState *saved_python_thread = रिक्त;

/*
 * Suspend a thread of the Python interpreter, other threads are allowed to
 * run.
 */
    स्थिर शून्य
Python_SaveThread(शून्य)
{
    saved_python_thread = PyEval_SaveThread();
}

/*
 * Restore a thread of the Python interpreter, waits for other threads to
 * block.
 */
    स्थिर शून्य
Python_RestoreThread(शून्य)
{
    PyEval_RestoreThread(saved_python_thread);
    saved_python_thread = रिक्त;
}
#endif

    शून्य
python_end(शून्य)
{
    स्थिर पूर्णाङ्क recurse = 0;

    // If a crash occurs while doing this, don't try again.
    यदि (recurse != 0)
	प्रत्यागम;

    python_end_called = TRUE;
    ++recurse;

#ifdef DYNAMIC_PYTHON
    यदि (hinstPython && Py_IsInitialized())
    {
# ifdef PY_CAN_RECURSE
	PyGILState_Ensure();
# अन्यथा
	Python_RestoreThread();	    // enter python
# endif
	Py_Finalize();
    }
#अन्यथा
    यदि (Py_IsInitialized())
    {
# ifdef PY_CAN_RECURSE
	PyGILState_Ensure();
# अन्यथा
	Python_RestoreThread();	    // enter python
# endif
	Py_Finalize();
    }
#endif

    --recurse;
}

#यदि defined(DYNAMIC_PYTHON) && defined(FEAT_PYTHON3)
    पूर्णाङ्क
python_loaded(शून्य)
{
    प्रत्यागम (hinstPython != 0);
}
#endif

स्थिर वर्ण *py_home_buf = रिक्त;

    स्थिर पूर्णाङ्क
Python_Init(शून्य)
{
    यदि (!initialised)
    {
#यदि defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02070000
	PyObject *site;
#endif

#ifdef DYNAMIC_PYTHON
	यदि (!python_enabled(TRUE))
	{
	    emsg(_(e_sorry_this_command_is_disabled_python_library_could_not_be_found));
	    गच्छ fail;
	}
#endif

	यदि (*p_pyhome != NUL)
	{
	    // The string must not change later, make a copy in static memory.
	    py_home_buf = (वर्ण *)vim_strsave(p_pyhome);
	    यदि (py_home_buf != रिक्त)
		Py_SetPythonHome(py_home_buf);
	}
#ifdef PYTHON_HOME
	अन्यथा यदि (mch_getenv((char_u *)"PYTHONHOME") == रिक्त)
	    Py_SetPythonHome(PYTHON_HOME);
#endif

	init_structs();

#यदि defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02070000
	// Disable implicit 'import site', because it may cause Vim to exit
	// when it can't be found.
	Py_NoSiteFlag++;
#endif

	Py_Initialize();

#यदि defined(PY_VERSION_HEX) && PY_VERSION_HEX >= 0x02070000
	// 'import site' explicitly.
	site = PyImport_ImportModule("site");
	यदि (site == रिक्त)
	{
	    emsg(_(e_sorry_this_command_is_disabled_python_site_module_could_not_be_loaded));
	    गच्छ fail;
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

	यदि (PythonIO_Init_io())
	    गच्छ fail;

	यदि (PythonMod_Init())
	    गच्छ fail;

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

    प्रत्यागम 0;

fail:
    // We call PythonIO_Flush() here to print any Python errors.
    // This is OK, as it is possible to call this function even
    // if PythonIO_Init_io() has not completed successfully (it will
    // not do anything in this case).
    PythonIO_Flush();
    प्रत्यागम -1;
}

/*
 * External interface
 */
    स्थिर शून्य
DoPyCommand(
    ध्रुव वर्ण		*cmd,
    dict_T*		locals,
    rangeinitializer	init_range,
    runner		run,
    शून्य		*arg)
{
#ifndef PY_CAN_RECURSE
    स्थिर पूर्णाङ्क		recursive = 0;
#endif
#यदि defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    वर्ण		*saved_locale;
#endif
#ifdef PY_CAN_RECURSE
    PyGILState_STATE	pygilstate;
#endif

#ifndef PY_CAN_RECURSE
    यदि (recursive)
    {
	emsg(_(e_cannot_invoke_python_recursively));
	प्रत्यागम;
    }
    ++recursive;
#endif
    यदि (python_end_called)
	प्रत्यागम;

    यदि (Python_Init())
	गच्छ theend;

    init_range(arg);

    Python_Release_Vim();	    // leave Vim

#यदि defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    // Python only works properly when the LC_NUMERIC locale is "C".
    saved_locale = setlocale(LC_NUMERIC, रिक्त);
    यदि (saved_locale == रिक्त || STRCMP(saved_locale, "C") == 0)
	saved_locale = रिक्त;
    अन्यथा
    {
	// Need to make a copy, value may change when setting new locale.
	saved_locale = (वर्ण *) PY_STRSAVE(saved_locale);
	(शून्य)setlocale(LC_NUMERIC, "C");
    }
#endif

#ifdef PY_CAN_RECURSE
    pygilstate = PyGILState_Ensure();
#अन्यथा
    Python_RestoreThread();	    // enter python
#endif

    run((वर्ण *) cmd, locals, arg
#ifdef PY_CAN_RECURSE
	    , &pygilstate
#endif
	    );

#ifdef PY_CAN_RECURSE
    PyGILState_Release(pygilstate);
#अन्यथा
    Python_SaveThread();	    // leave python
#endif

#यदि defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    यदि (saved_locale != रिक्त)
    {
	(शून्य)setlocale(LC_NUMERIC, saved_locale);
	PyMem_Free(saved_locale);
    }
#endif

    Python_Lock_Vim();		    // enter vim
    PythonIO_Flush();

theend:
#ifndef PY_CAN_RECURSE
    --recursive;
#endif
    प्रत्यागम;
}

/*
 * ":python"
 */
    शून्य
ex_python(exarg_T *eap)
{
    char_u *script;

    script = script_get(eap, eap->arg);
    यदि (!eap->skip)
    {
	यदि (p_pyx == 0)
	    p_pyx = 2;

	DoPyCommand(script == रिक्त ? (वर्ण *) eap->arg : (वर्ण *) script,
		रिक्त,
		init_range_cmd,
		(runner) run_cmd,
		(शून्य *) eap);
    }
    vim_free(script);
}

#define BUFFER_SIZE 1024

/*
 * ":pyfile"
 */
    शून्य
ex_pyfile(exarg_T *eap)
{
    स्थिर वर्ण buffer[BUFFER_SIZE];
    ध्रुव वर्ण *file = (वर्ण *)eap->arg;
    वर्ण *p;

    यदि (p_pyx == 0)
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

    यावत् (*file && p < buffer + (BUFFER_SIZE - 3))
    {
	यदि (*file == '\\' || *file == '\'')
	    *p++ = '\\';
	*p++ = *file++;
    }

    // If we didn't finish the file name, we hit a buffer overflow
    यदि (*file != '\0')
	प्रत्यागम;

    // Put in the terminating "')" and a null
    *p++ = '\'';
    *p++ = ')';
    *p++ = '\0';

    // Execute the file
    DoPyCommand(buffer,
	    रिक्त,
	    init_range_cmd,
	    (runner) run_cmd,
	    (शून्य *) eap);
}

    शून्य
ex_pydo(exarg_T *eap)
{
    यदि (p_pyx == 0)
	p_pyx = 2;

    DoPyCommand((वर्ण *)eap->arg,
	    रिक्त,
	    init_range_cmd,
	    (runner)run_do,
	    (शून्य *)eap);
}

///////////////////////////////////////////////////////
// 2. Python output stream: writes output via [e]msg().

// Implementation functions

    स्थिर PyObject *
OutputGetattr(PyObject *self, वर्ण *name)
{
    यदि (strcmp(name, "softspace") == 0)
	प्रत्यागम PyInt_FromLong(((OutputObject *)(self))->softspace);
    अन्यथा यदि (strcmp(name, "__members__") == 0)
	प्रत्यागम ObjectDir(रिक्त, OutputAttrs);
    अन्यथा यदि (strcmp(name, "errors") == 0)
	प्रत्यागम PyString_FromString("strict");
    अन्यथा यदि (strcmp(name, "encoding") == 0)
	प्रत्यागम PyString_FromString(ENC_OPT);
    प्रत्यागम Py_FindMethod(OutputMethods, self, name);
}

///////////////////////////////////////////////////////
// 3. Implementation of the Vim module for Python

// Window type - Implementation functions
// --------------------------------------

#define WindowType_Check(obj) ((obj)->ob_type == &WindowType)

// Buffer type - Implementation functions
// --------------------------------------

#define BufferType_Check(obj) ((obj)->ob_type == &BufferType)

स्थिर पूर्णाङ्क BufferAssItem(PyObject *, PyInt, PyObject *);
स्थिर पूर्णाङ्क BufferAssSlice(PyObject *, PyInt, PyInt, PyObject *);

// Line range type - Implementation functions
// --------------------------------------

#define RangeType_Check(obj) ((obj)->ob_type == &RangeType)

स्थिर पूर्णाङ्क RangeAssItem(PyObject *, PyInt, PyObject *);
स्थिर पूर्णाङ्क RangeAssSlice(PyObject *, PyInt, PyInt, PyObject *);

// Current objects type - Implementation functions
// -----------------------------------------------

स्थिर PySequenceMethods BufferAsSeq = {
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

    स्थिर PyObject *
BufferGetattr(PyObject *self, वर्ण *name)
{
    PyObject *r;

    यदि ((r = BufferAttrValid((BufferObject *)(self), name)))
	प्रत्यागम r;

    यदि (CheckBuffer((BufferObject *)(self)))
	प्रत्यागम रिक्त;

    r = BufferAttr((BufferObject *)(self), name);
    यदि (r || PyErr_Occurred())
	प्रत्यागम r;
    अन्यथा
	प्रत्यागम Py_FindMethod(BufferMethods, self, name);
}

//////////////////

    स्थिर पूर्णाङ्क
BufferAssItem(PyObject *self, PyInt n, PyObject *val)
{
    प्रत्यागम RBAsItem((BufferObject *)(self), n, val, 1, -1, रिक्त);
}

    स्थिर पूर्णाङ्क
BufferAssSlice(PyObject *self, PyInt lo, PyInt hi, PyObject *val)
{
    प्रत्यागम RBAsSlice((BufferObject *)(self), lo, hi, val, 1, -1, रिक्त);
}

स्थिर PySequenceMethods RangeAsSeq = {
    (PyInquiry)		RangeLength,	      // sq_length,    len(x)
    (binaryfunc)	0, /* RangeConcat, */ // sq_concat,    x+y
    (PyIntArgFunc)	0, /* RangeRepeat, */ // sq_repeat,    x*n
    (PyIntArgFunc)	RangeItem,	      // sq_item,      x[i]
    (PyIntIntArgFunc)	RangeSlice,	      // sq_slice,     x[i:j]
    (PyIntObjArgProc)	RangeAssItem,	      // sq_ass_item,  x[i]=v
    (PyIntIntObjArgProc) RangeAssSlice,	      // sq_ass_slice, x[i:j]=v
    (objobjproc)	0,
#यदि PY_MAJOR_VERSION >= 2
    (binaryfunc)	0,
    0,
#endif
};

// Line range object - Implementation

    स्थिर PyObject *
RangeGetattr(PyObject *self, वर्ण *name)
{
    यदि (strcmp(name, "start") == 0)
	प्रत्यागम Py_Build_मूल्य(Py_ssize_t_fmt, ((RangeObject *)(self))->start - 1);
    अन्यथा यदि (strcmp(name, "end") == 0)
	प्रत्यागम Py_Build_मूल्य(Py_ssize_t_fmt, ((RangeObject *)(self))->end - 1);
    अन्यथा यदि (strcmp(name, "__members__") == 0)
	प्रत्यागम ObjectDir(रिक्त, RangeAttrs);
    अन्यथा
	प्रत्यागम Py_FindMethod(RangeMethods, self, name);
}

////////////////

    स्थिर पूर्णाङ्क
RangeAssItem(PyObject *self, PyInt n, PyObject *val)
{
    प्रत्यागम RBAsItem(((RangeObject *)(self))->buf, n, val,
		     ((RangeObject *)(self))->start,
		     ((RangeObject *)(self))->end,
		     &((RangeObject *)(self))->end);
}

    स्थिर पूर्णाङ्क
RangeAssSlice(PyObject *self, PyInt lo, PyInt hi, PyObject *val)
{
    प्रत्यागम RBAsSlice(((RangeObject *)(self))->buf, lo, hi, val,
		      ((RangeObject *)(self))->start,
		      ((RangeObject *)(self))->end,
		      &((RangeObject *)(self))->end);
}

// TabPage object - Implementation

    स्थिर PyObject *
TabPageGetattr(PyObject *self, वर्ण *name)
{
    PyObject *r;

    यदि ((r = TabPageAttrValid((TabPageObject *)(self), name)))
	प्रत्यागम r;

    यदि (CheckTabPage((TabPageObject *)(self)))
	प्रत्यागम रिक्त;

    r = TabPageAttr((TabPageObject *)(self), name);
    यदि (r || PyErr_Occurred())
	प्रत्यागम r;
    अन्यथा
	प्रत्यागम Py_FindMethod(TabPageMethods, self, name);
}

// Window object - Implementation

    स्थिर PyObject *
WindowGetattr(PyObject *self, वर्ण *name)
{
    PyObject *r;

    यदि ((r = WindowAttrValid((WindowObject *)(self), name)))
	प्रत्यागम r;

    यदि (CheckWindow((WindowObject *)(self)))
	प्रत्यागम रिक्त;

    r = WindowAttr((WindowObject *)(self), name);
    यदि (r || PyErr_Occurred())
	प्रत्यागम r;
    अन्यथा
	प्रत्यागम Py_FindMethod(WindowMethods, self, name);
}

// Tab page list object - Definitions

स्थिर PySequenceMethods TabListAsSeq = {
    (PyInquiry)		TabListLength,	    // sq_length,    len(x)
    (binaryfunc)	0,		    // sq_concat,    x+y
    (PyIntArgFunc)	0,		    // sq_repeat,    x*n
    (PyIntArgFunc)	TabListItem,	    // sq_item,      x[i]
    (PyIntIntArgFunc)	0,		    // sq_slice,     x[i:j]
    (PyIntObjArgProc)	0,		    // sq_ass_item,  x[i]=v
    (PyIntIntObjArgProc) 0,		    // sq_ass_slice, x[i:j]=v
    (objobjproc)	0,
#यदि PY_MAJOR_VERSION >= 2
    (binaryfunc)	0,
    0,
#endif
};

// Window list object - Definitions

स्थिर PySequenceMethods WinListAsSeq = {
    (PyInquiry)		WinListLength,	    // sq_length,    len(x)
    (binaryfunc)	0,		    // sq_concat,    x+y
    (PyIntArgFunc)	0,		    // sq_repeat,    x*n
    (PyIntArgFunc)	WinListItem,	    // sq_item,      x[i]
    (PyIntIntArgFunc)	0,		    // sq_slice,     x[i:j]
    (PyIntObjArgProc)	0,		    // sq_ass_item,  x[i]=v
    (PyIntIntObjArgProc) 0,		    // sq_ass_slice, x[i:j]=v
    (objobjproc)	0,
#यदि PY_MAJOR_VERSION >= 2
    (binaryfunc)	0,
    0,
#endif
};

// External interface

    शून्य
python_buffer_free(buf_T *buf)
{
    BufferObject *bp = BUF_PYTHON_REF(buf);
    यदि (bp == रिक्त)
	प्रत्यागम;
    bp->buf = INVALID_BUFFER_मूल्य;
    BUF_PYTHON_REF(buf) = रिक्त;
}

    शून्य
python_window_free(win_T *win)
{
    WindowObject *wp = WIN_PYTHON_REF(win);
    यदि (wp == रिक्त)
	प्रत्यागम;
    wp->win = INVALID_WINDOW_मूल्य;
    WIN_PYTHON_REF(win) = रिक्त;
}

    शून्य
python_tabpage_free(tabpage_T *tab)
{
    TabPageObject *tp = TAB_PYTHON_REF(tab);
    यदि (tp == रिक्त)
	प्रत्यागम;
    tp->tab = INVALID_TABPAGE_मूल्य;
    TAB_PYTHON_REF(tab) = रिक्त;
}

    स्थिर पूर्णाङ्क
PythonMod_Init(शून्य)
{
    // The special value is removed from sys.path in Python_Init().
    स्थिर वर्ण	*(argv[2]) = {"/must>not&exist/foo", रिक्त};

    यदि (init_types())
	प्रत्यागम -1;

    // Set sys.argv[] to avoid a crash in warn().
    PySys_SetArgv(1, argv);

    vim_module = Py_InitModule4("vim", VimMethods, (वर्ण *)रिक्त,
				(PyObject *)रिक्त, PYTHON_API_VERSION);

    यदि (populate_module(vim_module))
	प्रत्यागम -1;

    यदि (init_sys_path())
	प्रत्यागम -1;

    प्रत्यागम 0;
}

//////////////////////////////////////////////////////////////////////////
// 4. Utility functions for handling the interface between Vim and Python.

// Convert a Vim line into a Python string.
// All internal newlines are replaced by null characters.
//
// On errors, the Python exception data is set, and NULL is returned.
    स्थिर PyObject *
पंक्ति_To_String(ध्रुव वर्ण *str)
{
    PyObject *result;
    PyInt len = strlen(str);
    वर्ण *p;

    // Allocate a Python string object, with uninitialised contents. We
    // must do it this way, so that we can modify the string in place
    // later. See the Python source, Objects/stringobject.c for details.
    result = PyString_FromStringAndSize(रिक्त, len);
    यदि (result == रिक्त)
	प्रत्यागम रिक्त;

    p = PyString_AsString(result);

    यावत् (*str)
    {
	यदि (*str == '\n')
	    *p = '\0';
	अन्यथा
	    *p = *str;

	++p;
	++str;
    }

    प्रत्यागम result;
}

    स्थिर PyObject *
DictionaryGetattr(PyObject *self, वर्ण *name)
{
    DictionaryObject	*this = ((DictionaryObject *) (self));

    यदि (strcmp(name, "locked") == 0)
	प्रत्यागम PyInt_FromLong(this->dict->dv_lock);
    अन्यथा यदि (strcmp(name, "scope") == 0)
	प्रत्यागम PyInt_FromLong(this->dict->dv_scope);
    अन्यथा यदि (strcmp(name, "__members__") == 0)
	प्रत्यागम ObjectDir(रिक्त, DictionaryAttrs);

    प्रत्यागम Py_FindMethod(DictionaryMethods, self, name);
}

    स्थिर PyObject *
ListGetattr(PyObject *self, वर्ण *name)
{
    यदि (strcmp(name, "locked") == 0)
	प्रत्यागम PyInt_FromLong(((ListObject *)(self))->list->lv_lock);
    अन्यथा यदि (strcmp(name, "__members__") == 0)
	प्रत्यागम ObjectDir(रिक्त, ListAttrs);

    प्रत्यागम Py_FindMethod(ListMethods, self, name);
}

    स्थिर PyObject *
TupleGetattr(PyObject *self, वर्ण *name)
{
    यदि (strcmp(name, "locked") == 0)
	प्रत्यागम PyInt_FromLong(((TupleObject *)(self))->tuple->tv_lock);
    अन्यथा यदि (strcmp(name, "__members__") == 0)
	प्रत्यागम ObjectDir(रिक्त, TupleAttrs);

    प्रत्यागम Py_FindMethod(TupleMethods, self, name);
}

    स्थिर PyObject *
FunctionGetattr(PyObject *self, वर्ण *name)
{
    PyObject	*r;

    r = FunctionAttr((FunctionObject *)(self), name);

    यदि (r || PyErr_Occurred())
	प्रत्यागम r;
    अन्यथा
	प्रत्यागम Py_FindMethod(FunctionMethods, self, name);
}

    शून्य
do_pyeval(char_u *str, dict_T *locals, typval_T *rettv)
{
    DoPyCommand((वर्ण *) str,
	    locals,
	    init_range_eval,
	    (runner) run_eval,
	    (शून्य *) rettv);
    यदि (rettv->v_type == VAR_UNKNOWN)
    {
	rettv->v_type = VAR_संख्या;
	rettv->vval.v_संख्या = 0;
    }
}

    पूर्णाङ्क
set_ref_in_python(पूर्णाङ्क copyID)
{
    प्रत्यागम set_ref_in_py(copyID);
}
