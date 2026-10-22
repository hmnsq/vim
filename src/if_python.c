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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಅನ್ತರ್ಭಾವ <limits.h>

// uncomment this if used with the debug version of python.
// Checked on 2.7.4.
// #define Py_DEBUG
// Note: most of time you can add -DPy_DEBUG to CFLAGS in place of uncommenting
// uncomment this if used with the debug version of python, but without its
// allocator
// #define Py_DEBUG_NO_PYMALLOC

// Python.h defines _POSIX_THREADS itself (if needed)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _POSIX_THREADS
# undef _POSIX_THREADS
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(MSWIN) && defined(HAVE_FCNTL_H)
# undef HAVE_FCNTL_H
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _DEBUG
# undef _DEBUG
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_STRFTIME
# undef HAVE_STRFTIME
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_STRING_H
# undef HAVE_STRING_H
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_PUTENV
# undef HAVE_PUTENV
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _POSIX_C_SOURCE
# undef _POSIX_C_SOURCE	// pyconfig.h defines it as well.
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ XOPEN_ಮೂಲ
# undef XOPEN_ಮೂಲ	// pyconfig.h defines it as well.
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು PY_SSIZE_T_CLEAN

#ಅನ್ತರ್ಭಾವ <Python.h>

#ಆದರೆ !defined(PY_ಆವೃತ್ತಿ_HEX) || PY_ಆವೃತ್ತಿ_HEX < 0x02050000
# undef PY_SSIZE_T_CLEAN
#ಆದರೆ_ಅಂತ್ಯ

// these are NULL for Python 2
#ವ್ಯಾಖ್ಯಾನಿಸು ERRORS_DECODE_ARG ಖಾಲಿ
#ವ್ಯಾಖ್ಯಾನಿಸು ERRORS_ENCODE_ARG ERRORS_DECODE_ARG

#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು ಮುಖ್ಯ // Defined in python.h - aargh
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು HAVE_FCNTL_H // Clash with os_win32.h

// Perhaps leave this out for Python 2.6, which supports bytes?
#ವ್ಯಾಖ್ಯಾನಿಸು Py_ಬೈಟುಗಳು_From_String      PyString_FromString
#ವ್ಯಾಖ್ಯಾನಿಸು Py_ಬೈಟುಗಳು_Check		PyString_Check
#ವ್ಯಾಖ್ಯಾನಿಸು Py_ಬೈಟುಗಳು_As_String_And_ಗಾತ್ರ Py_String_As_String_And_ಗಾತ್ರ
#ವ್ಯಾಖ್ಯಾನಿಸು Py_ಬೈಟುಗಳು_From_String_And_ಗಾತ್ರ   Py_String_From_String_And_ಗಾತ್ರ

#ಆದರೆ defined(PY_ಆವೃತ್ತಿ_HEX) && PY_ಆವೃತ್ತಿ_HEX >= 0x02070000
# define PY_USE_CAPSULE
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(PY_ಆವೃತ್ತಿ_HEX) && PY_ಆವೃತ್ತಿ_HEX >= 0x02050000
# define PyInt Py_ssize_t
# define PyInquiry lenfunc
# define PyIntArgFunc ssizeargfunc
# define PyIntIntArgFunc ssizessizeargfunc
# define PyIntObjArgProc ssizeobjargproc
# define PyIntIntObjArgProc ssizessizeobjargproc
# define Py_ssize_t_fmt "n"
#ಇಲ್ಲದಿದ್ದರೆ
# define PyInt ಪೂರ್ಣಾಂಕ
# define lenfunc inquiry
# define PyInquiry inquiry
# define PyIntArgFunc intargfunc
# define PyIntIntArgFunc intintargfunc
# define PyIntObjArgProc intobjargproc
# define PyIntIntObjArgProc intintobjargproc
# define Py_ssize_t_fmt "i"
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು Py_ಬೈಟುಗಳು_fmt "s"

// Parser flags
#ವ್ಯಾಖ್ಯಾನಿಸು single_ಒಳಹರಿವು	256
#ವ್ಯಾಖ್ಯಾನಿಸು file_ಒಳಹರಿವು	257
#ವ್ಯಾಖ್ಯಾನಿಸು eval_ಒಳಹರಿವು	258

#ಆದರೆ defined(PY_ಆವೃತ್ತಿ_HEX) && PY_ಆವೃತ್ತಿ_HEX >= 0x020300F0
  // Python 2.3: can invoke ":python" recursively.
# define PY_CAN_RECURSE
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(DYNAMIC_PYTHON)
# ifndef DYNAMIC_PYTHON
#  define HINSTANCE long_u		// for generating prototypes
# endif

# ifdef MSWIN
#  define ತುಂಬು_dll vim_ತುಂಬು_Lib
#  define close_dll FreeLibrary
#  define ಚಿಹ್ನೆ_from_dll Get_Proc_ವಿಳಾಸ
#  define ತುಂಬು_dll_error GetWin32Error
# ಇಲ್ಲದಿದ್ದರೆ
#  include <dlfcn.h>
#  define FARPROC ಶೂನ್ಯ*
#  define HINSTANCE ಶೂನ್ಯ*
#  ಆದರೆ defined(PY_NO_RTLD_GLOBAL) && defined(PY3_NO_RTLD_GLOBAL)
#   define ತುಂಬು_dll(n) dlopen((n), RTLD_LAZY)
#  ಇಲ್ಲದಿದ್ದರೆ
#   define ತುಂಬು_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#  endif
#  define close_dll dlclose
#  define ಚಿಹ್ನೆ_from_dll dlsym
#  define ತುಂಬು_dll_error dlerror
# endif

// This makes if_python.c compile without warnings against Python 2.5
// on Win32 and Win64.
# undef Py_ಸತತ_Simple_String
# undef Py_ಸತತ_String
# undef PyArg_Parse
# undef PyArg_ParseTuple
# undef Py_Build_ಮೌಲ್ಯ
# undef Py_InitModule4
# undef Py_InitModule4_64
# undef Py_Object_Call_ವಿಧಾನ
# undef PyObject_CallFunction

/*
 * Wrapper defines
 */
# define PyArg_Parse dll_PyArg_Parse
# define PyArg_ParseTuple dll_PyArg_ParseTuple
# define PyMem_Free dll_PyMem_Free
# define PyMem_Malloc dll_PyMem_Malloc
# define Py_Dict_ಗಣ_ಅಂಶ_String dll_Py_Dict_ಗಣ_ಅಂಶ_String
# define Py_Err_Bad_ವಾದ dll_Py_Err_Bad_ವಾದ
# define PyErr_NewException dll_PyErr_NewException
# define PyErr_Clear dll_PyErr_Clear
# define PyErr_Format dll_PyErr_Format
# define PyErr_PrintEx dll_PyErr_PrintEx
# define Py_Err_No_ಸ್ಮರಣೆ dll_Py_Err_No_ಸ್ಮರಣೆ
# define PyErr_Occurred dll_PyErr_Occurred
# define Py_Err_ಗಣ_None dll_Py_Err_ಗಣ_None
# define Py_Err_ಗಣ_String dll_Py_Err_ಗಣ_String
# define Py_Err_ಗಣ_Object dll_Py_Err_ಗಣ_Object
# define PyErr_ExceptionMatches dll_PyErr_ExceptionMatches
# define Py_Eval_Init_ಎಳೆಗಳು dll_Py_Eval_Init_ಎಳೆಗಳು
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
# define Py_List_Get_ಅಂಶ dll_Py_List_Get_ಅಂಶ
# define PyList_Append dll_PyList_Append
# define Py_List_ಸೇರಿಸು dll_Py_List_ಸೇರಿಸು
# define PyList_New dll_PyList_New
# define Py_List_ಗಣ_ಅಂಶ_2 dll_Py_List_ಗಣ_ಅಂಶ
# define Py_List_ಗಾತ್ರ dll_Py_List_ಗಾತ್ರ
# define PyList_Type (*dll_PyList_Type)
# define Py_Tuple_Get_ಅಂಶ dll_Py_Tuple_Get_ಅಂಶ
# define Py_Tuple_ಗಣ_ಅಂಶ dll_Py_Tuple_ಗಣ_ಅಂಶ
# define PyTuple_New dll_PyTuple_New
# define Py_Tuple_ಗಾತ್ರ dll_Py_Tuple_ಗಾತ್ರ
# define PyTuple_Type (*dll_PyTuple_Type)
# define PySequence_Check dll_PySequence_Check
# define Py_Sequence_ಗಾತ್ರ dll_Py_Sequence_ಗಾತ್ರ
# define Py_Sequence_Get_ಅಂಶ dll_Py_Sequence_Get_ಅಂಶ
# define PySequence_Fast dll_PySequence_Fast
# define Py_ತುಂಡು_Get_Indices_Ex dll_Py_ತುಂಡು_Get_Indices_Ex
# define PyImport_ImportModule dll_PyImport_ImportModule
# define PyDict_New dll_PyDict_New
# define Py_Dict_Get_ಅಂಶ_String dll_Py_Dict_Get_ಅಂಶ_String
# define Py_Dict_ಮುಂದಿನ dll_Py_Dict_ಮುಂದಿನ
# define PyDict_Type (*dll_PyDict_Type)
# ifdef Py_Mapping_ಕೀಲಿಗಳು
#  define PY_NO_MAPPING_ಕೀಲಿಗಳು
# ಇಲ್ಲದಿದ್ದರೆ
#  define Py_Mapping_ಕೀಲಿಗಳು dll_Py_Mapping_ಕೀಲಿಗಳು
# endif
# define Py_Object_Get_ಅಂಶ dll_Py_Object_Get_ಅಂಶ
# define Py_Object_Call_ವಿಧಾನ dll_Py_Object_Call_ವಿಧಾನ
# define PyMapping_Check dll_PyMapping_Check
# define Py_Iter_ಮುಂದಿನ dll_Py_Iter_ಮುಂದಿನ
# define PyModule_GetDict dll_PyModule_GetDict
# define Py_Module_ಕೂಡಿಸು_Object dll_Py_Module_ಕೂಡಿಸು_Object
# define Py_ಸತತ_Simple_String dll_Py_ಸತತ_Simple_String
# define Py_ಸತತ_String dll_Py_ಸತತ_String
# define PyObject_GetAttrString dll_PyObject_GetAttrString
# define PyObject_HasAttrString dll_PyObject_HasAttrString
# define Py_Object_ಗಣ_Attr_String dll_Py_Object_ಗಣ_Attr_String
# define Py_Object_Call_Function_Obj_ವಾದಗಳು dll_Py_Object_Call_Function_Obj_ವಾದಗಳು
# define PyObject_CallFunction dll_PyObject_CallFunction
# define PyObject_Call dll_PyObject_Call
# define PyObject_Repr dll_PyObject_Repr
# define PyString_AsString dll_PyString_AsString
# define Py_String_As_String_And_ಗಾತ್ರ dll_Py_String_As_String_And_ಗಾತ್ರ
# define PyString_FromString dll_PyString_FromString
# define PyString_FromFormat dll_PyString_FromFormat
# define Py_String_From_String_And_ಗಾತ್ರ dll_Py_String_From_String_And_ಗಾತ್ರ
# define Py_String_ಗಾತ್ರ dll_Py_String_ಗಾತ್ರ
# define PyString_Type (*dll_PyString_Type)
# define PyUnicode_Type (*dll_PyUnicode_Type)
# undef PyUnicode_AsEncodedString
# define PyUnicode_AsEncodedString py_PyUnicode_AsEncodedString
# define PyFloat_AsDouble dll_PyFloat_AsDouble
# define PyFloat_FromDouble dll_PyFloat_FromDouble
# define PyFloat_Type (*dll_PyFloat_Type)
# define Py_ಸಂಖ್ಯೆ_Check dll_Py_ಸಂಖ್ಯೆ_Check
# define Py_ಸಂಖ್ಯೆ_Long dll_Py_ಸಂಖ್ಯೆ_Long
# define Py_Import_ಕೂಡಿಸು_Module (*dll_Py_Import_ಕೂಡಿಸು_Module)
# define Py_Sys_ಗಣ_Object dll_Py_Sys_ಗಣ_Object
# define PySys_GetObject dll_PySys_GetObject
# define Py_Sys_ಗಣ_Argv dll_Py_Sys_ಗಣ_Argv
# define PyType_Type (*dll_PyType_Type)
# define PyFile_Type (*dll_PyFile_Type)
# define Py_ತುಂಡು_Type (*dll_Py_ತುಂಡು_Type)
# define PyType_Ready (*dll_PyType_Ready)
# define PyType_GenericAlloc dll_PyType_GenericAlloc
# define Py_Build_ಮೌಲ್ಯ dll_Py_Build_ಮೌಲ್ಯ
# define Py_ಕಂಡುಹಿಡಿ_ವಿಧಾನ dll_Py_ಕಂಡುಹಿಡಿ_ವಿಧಾನ
# define Py_InitModule4 dll_Py_InitModule4
# define Py_ಗಣ_Python_Home dll_Py_ಗಣ_Python_Home
# define Py_Initialize dll_Py_Initialize
# define Py_Finalize dll_Py_Finalize
# define Py_IsInitialized dll_Py_IsInitialized
# define _PyObject_New dll__PyObject_New
# define _PyObject_GC_New dll__PyObject_GC_New
# ifdef PyObject_GC_Del
#  define Py_underscore_GC
#  define _PyObject_GC_Del dll__PyObject_GC_Del
#  define _PyObject_GC_UnTrack dll__PyObject_GC_UnTrack
# ಇಲ್ಲದಿದ್ದರೆ
#  define PyObject_GC_Del dll_PyObject_GC_Del
#  define PyObject_GC_UnTrack dll_PyObject_GC_UnTrack
# endif
# ಆದರೆ defined(PY_ಆವೃತ್ತಿ_HEX) && PY_ಆವೃತ್ತಿ_HEX >= 0x02070000
#  define Py_Object_ಮುಂದಿನ_Not_Implemented (*dll_Py_Object_ಮುಂದಿನ_Not_Implemented)
# endif
# define _Py_NoneStruct (*dll__Py_NoneStruct)
# define _Py_ZeroStruct (*dll__Py_ZeroStruct)
# define _Py_TrueStruct (*dll__Py_TrueStruct)
# define PyObject_Init dll__PyObject_Init
# define PyObject_GetIter dll_PyObject_GetIter
# define PyObject_IsTrue dll_PyObject_IsTrue
# ಆದರೆ defined(PY_ಆವೃತ್ತಿ_HEX) && PY_ಆವೃತ್ತಿ_HEX >= 0x02020000
#  define PyType_IsSubtype dll_PyType_IsSubtype
#  ifdef Py_DEBUG
#   define _Py_NegativeRefcount dll__Py_NegativeRefcount
#   define Py_Ref_ಒಟ್ಟು (*dll_Py_Ref_ಒಟ್ಟು)
#   define _Py_Dealloc dll__Py_Dealloc
#  endif
# endif
# ಆದರೆ defined(PY_ಆವೃತ್ತಿ_HEX) && PY_ಆವೃತ್ತಿ_HEX >= 0x02030000
#  ಆದರೆ defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
#   define _PyObject_DebugMalloc dll__PyObject_DebugMalloc
#   define _PyObject_DebugFree dll__PyObject_DebugFree
#  ಇಲ್ಲದಿದ್ದರೆ
#   define PyObject_Malloc dll_PyObject_Malloc
#   define PyObject_Free dll_PyObject_Free
#  endif
# endif
# ifdef PY_USE_CAPSULE
#  define PyCapsule_New dll_PyCapsule_New
#  define Py_Capsule_Get_ಸೂಚಕ dll_Py_Capsule_Get_ಸೂಚಕ
# ಇಲ್ಲದಿದ್ದರೆ
#  define PyCObject_FromVoidPtr dll_PyCObject_FromVoidPtr
#  define PyCObject_AsVoidPtr dll_PyCObject_AsVoidPtr
# endif
# ಆದರೆ defined(PY_ಆವೃತ್ತಿ_HEX) && PY_ಆವೃತ್ತಿ_HEX >= 0x02070000
#  define Py_NoSiteFlag (*dll_Py_NoSiteFlag)
# endif

/*
 * Pointers for dynamic link
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ(*dll_PyArg_Parse)(PyObject *, ಅಕ್ಷರ *, ...);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ(*dll_PyArg_ParseTuple)(PyObject *, ಅಕ್ಷರ *, ...);
ಸ್ಥಿರ ಶೂನ್ಯ(*dll_PyMem_Free)(ಶೂನ್ಯ *);
ಸ್ಥಿರ ಶೂನ್ಯ* (*dll_PyMem_Malloc)(size_t);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ(*dll_Py_Dict_ಗಣ_ಅಂಶ_String)(PyObject *dp, ಅಕ್ಷರ *ಕೀಲಿ, PyObject *ಅಂಶ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ(*dll_Py_Err_Bad_ವಾದ)(ಶೂನ್ಯ);
ಸ್ಥಿರ PyObject *(*dll_PyErr_NewException)(ಅಕ್ಷರ *, PyObject *, PyObject *);
ಸ್ಥಿರ ಶೂನ್ಯ(*dll_PyErr_Clear)(ಶೂನ್ಯ);
ಸ್ಥಿರ PyObject*(*dll_PyErr_Format)(PyObject *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *, ...);
ಸ್ಥಿರ ಶೂನ್ಯ(*dll_PyErr_PrintEx)(ಪೂರ್ಣಾಂಕ);
ಸ್ಥಿರ PyObject*(*dll_Py_Err_No_ಸ್ಮರಣೆ)(ಶೂನ್ಯ);
ಸ್ಥಿರ PyObject*(*dll_PyErr_Occurred)(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ(*dll_Py_Err_ಗಣ_None)(PyObject *);
ಸ್ಥಿರ ಶೂನ್ಯ(*dll_Py_Err_ಗಣ_String)(PyObject *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *);
ಸ್ಥಿರ ಶೂನ್ಯ(*dll_Py_Err_ಗಣ_Object)(PyObject *, PyObject *);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ(*dll_PyErr_ExceptionMatches)(PyObject *);
ಸ್ಥಿರ ಶೂನ್ಯ(*dll_Py_Eval_Init_ಎಳೆಗಳು)(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ(*dll_PyEval_RestoreThread)(PyThreadState *);
ಸ್ಥಿರ PyThreadState*(*dll_PyEval_SaveThread)(ಶೂನ್ಯ);
# ifdef PY_CAN_RECURSE
ಸ್ಥಿರ PyGILState_STATE	(*dll_PyGILState_Ensure)(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_PyGILState_Release)(PyGILState_STATE);
# endif
ಸ್ಥಿರ ದೀರ್ಘ(*dll_PyInt_AsLong)(PyObject *);
ಸ್ಥಿರ PyObject*(*dll_PyInt_FromLong)(ದೀರ್ಘ);
ಸ್ಥಿರ ದೀರ್ಘ(*dll_PyLong_AsLong)(PyObject *);
ಸ್ಥಿರ PyObject*(*dll_PyLong_FromLong)(ದೀರ್ಘ);
ಸ್ಥಿರ PyTypeObject* dll_PyBool_Type;
ಸ್ಥಿರ PyTypeObject* dll_PyInt_Type;
ಸ್ಥಿರ PyTypeObject* dll_PyLong_Type;
ಸ್ಥಿರ PyObject*(*dll_Py_List_Get_ಅಂಶ)(PyObject *, PyInt);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ(*dll_PyList_Append)(PyObject *, PyObject *);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ(*dll_Py_List_ಸೇರಿಸು)(PyObject *, PyInt, PyObject *);
ಸ್ಥಿರ PyObject*(*dll_PyList_New)(PyInt ಗಾತ್ರ_2);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ(*dll_Py_List_ಗಣ_ಅಂಶ)(PyObject *, PyInt, PyObject *);
ಸ್ಥಿರ PyInt(*dll_Py_List_ಗಾತ್ರ)(PyObject *);
ಸ್ಥಿರ PyTypeObject* dll_PyList_Type;
ಸ್ಥಿರ PyObject*(*dll_Py_Tuple_Get_ಅಂಶ)(PyObject *, PyInt);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ(*dll_Py_Tuple_ಗಣ_ಅಂಶ)(PyObject *, PyInt, PyObject *);
ಸ್ಥಿರ PyObject*(*dll_PyTuple_New)(PyInt ಗಾತ್ರ_2);
ಸ್ಥಿರ PyInt(*dll_Py_Tuple_ಗಾತ್ರ)(PyObject *);
ಸ್ಥಿರ PyTypeObject* dll_PyTuple_Type;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*dll_PySequence_Check)(PyObject *);
ಸ್ಥಿರ PyInt(*dll_Py_Sequence_ಗಾತ್ರ)(PyObject *);
ಸ್ಥಿರ PyObject*(*dll_Py_Sequence_Get_ಅಂಶ)(PyObject *, PyInt);
ಸ್ಥಿರ PyObject*(*dll_PySequence_Fast)(PyObject *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*dll_Py_ತುಂಡು_Get_Indices_Ex)(Py_ತುಂಡು_Object *r, PyInt ಉದ್ದಳತೆ,
		     PyInt *ಆರಂಭ, PyInt *stop, PyInt *ಹೆಜ್ಜೆ,
		     PyInt *slicelen);
ಸ್ಥಿರ PyObject*(*dll_PyImport_ImportModule)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *);
ಸ್ಥಿರ PyObject*(*dll_PyDict_New)(ಶೂನ್ಯ);
ಸ್ಥಿರ PyObject*(*dll_Py_Dict_Get_ಅಂಶ_String)(PyObject *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*dll_Py_Dict_ಮುಂದಿನ)(PyObject *, PyInt *, PyObject **, PyObject **);
ಸ್ಥಿರ PyTypeObject* dll_PyDict_Type;
# ifndef PY_NO_MAPPING_ಕೀಲಿಗಳು
ಸ್ಥಿರ PyObject* (*dll_Py_Mapping_ಕೀಲಿಗಳು)(PyObject *);
# endif
ಸ್ಥಿರ PyObject* (*dll_Py_Object_Get_ಅಂಶ)(PyObject *, PyObject *);
ಸ್ಥಿರ PyObject* (*dll_Py_Object_Call_ವಿಧಾನ)(PyObject *, ಅಕ್ಷರ *, PyObject *);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*dll_PyMapping_Check)(PyObject *);
ಸ್ಥಿರ PyObject* (*dll_Py_Iter_ಮುಂದಿನ)(PyObject *);
ಸ್ಥಿರ PyObject*(*dll_PyModule_GetDict)(PyObject *);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ(*dll_Py_Module_ಕೂಡಿಸು_Object)(PyObject *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *, PyObject *);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ(*dll_Py_ಸತತ_Simple_String)(ಅಕ್ಷರ *);
ಸ್ಥಿರ PyObject *(*dll_Py_ಸತತ_String)(ಅಕ್ಷರ *, ಪೂರ್ಣಾಂಕ, PyObject *, PyObject *);
ಸ್ಥಿರ PyObject* (*dll_PyObject_GetAttrString)(PyObject *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*dll_PyObject_HasAttrString)(PyObject *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*dll_Py_Object_ಗಣ_Attr_String)(PyObject *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *, PyObject *);
ಸ್ಥಿರ PyObject* (*dll_Py_Object_Call_Function_Obj_ವಾದಗಳು)(PyObject *, ...);
ಸ್ಥಿರ PyObject* (*dll_PyObject_CallFunction)(PyObject *, ಅಕ್ಷರ *, ...);
ಸ್ಥಿರ PyObject* (*dll_PyObject_Call)(PyObject *, PyObject *, PyObject *);
ಸ್ಥಿರ PyObject* (*dll_PyObject_Repr)(PyObject *);
ಸ್ಥಿರ ಅಕ್ಷರ*(*dll_PyString_AsString)(PyObject *);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ(*dll_Py_String_As_String_And_ಗಾತ್ರ)(PyObject *, ಅಕ್ಷರ **, PyInt *);
ಸ್ಥಿರ PyObject*(*dll_PyString_FromString)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *);
ಸ್ಥಿರ PyObject*(*dll_PyString_FromFormat)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *, ...);
ಸ್ಥಿರ PyObject*(*dll_Py_String_From_String_And_ಗಾತ್ರ)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *, PyInt);
ಸ್ಥಿರ PyInt(*dll_Py_String_ಗಾತ್ರ)(PyObject *);
ಸ್ಥಿರ PyTypeObject* dll_PyString_Type;
ಸ್ಥಿರ PyTypeObject* dll_PyUnicode_Type;
ಸ್ಥಿರ PyObject *(*py_PyUnicode_AsEncodedString)(PyObject *, ಅಕ್ಷರ *, ಅಕ್ಷರ *);
ಸ್ಥಿರ ದ್ವಿಗುಣ(*dll_PyFloat_AsDouble)(PyObject *);
ಸ್ಥಿರ PyObject*(*dll_PyFloat_FromDouble)(ದ್ವಿಗುಣ);
ಸ್ಥಿರ PyTypeObject* dll_PyFloat_Type;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ(*dll_Py_ಸಂಖ್ಯೆ_Check)(PyObject *);
ಸ್ಥಿರ PyObject*(*dll_Py_ಸಂಖ್ಯೆ_Long)(PyObject *);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ(*dll_Py_Sys_ಗಣ_Object)(ಅಕ್ಷರ *, PyObject *);
ಸ್ಥಿರ PyObject *(*dll_PySys_GetObject)(ಅಕ್ಷರ *);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ(*dll_Py_Sys_ಗಣ_Argv)(ಪೂರ್ಣಾಂಕ, ಅಕ್ಷರ **);
ಸ್ಥಿರ PyTypeObject* dll_PyType_Type;
ಸ್ಥಿರ PyTypeObject* dll_PyFile_Type;
ಸ್ಥಿರ PyTypeObject* dll_Py_ತುಂಡು_Type;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*dll_PyType_Ready)(PyTypeObject *type);
ಸ್ಥಿರ PyObject* (*dll_PyType_GenericAlloc)(PyTypeObject *type, PyInt nitems);
ಸ್ಥಿರ PyObject*(*dll_Py_Build_ಮೌಲ್ಯ)(ಅಕ್ಷರ *, ...);
ಸ್ಥಿರ PyObject*(*dll_Py_ಕಂಡುಹಿಡಿ_ವಿಧಾನ)(ರಚನೆ Py_ವಿಧಾನ_Def[], PyObject *, ಅಕ್ಷರ *);
ಸ್ಥಿರ PyObject*(*dll_Py_InitModule4)(ಅಕ್ಷರ *, ರಚನೆ Py_ವಿಧಾನ_Def *, ಅಕ್ಷರ *, PyObject *, ಪೂರ್ಣಾಂಕ);
ಸ್ಥಿರ PyObject*(*dll_Py_Import_ಕೂಡಿಸು_Module)(ಅಕ್ಷರ *);
ಸ್ಥಿರ ಶೂನ್ಯ(*dll_Py_ಗಣ_Python_Home)(ಅಕ್ಷರ *home);
ಸ್ಥಿರ ಶೂನ್ಯ(*dll_Py_Initialize)(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ(*dll_Py_Finalize)(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ(*dll_Py_IsInitialized)(ಶೂನ್ಯ);
ಸ್ಥಿರ PyObject*(*dll__PyObject_New)(PyTypeObject *, PyObject *);
ಸ್ಥಿರ PyObject*(*dll__PyObject_GC_New)(PyTypeObject *);
# ifdef Py_underscore_GC
ಸ್ಥಿರ ಶೂನ್ಯ(*dll__PyObject_GC_Del)(ಶೂನ್ಯ *);
ಸ್ಥಿರ ಶೂನ್ಯ(*dll__PyObject_GC_UnTrack)(ಶೂನ್ಯ *);
# ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ ಶೂನ್ಯ(*dll_PyObject_GC_Del)(ಶೂನ್ಯ *);
ಸ್ಥಿರ ಶೂನ್ಯ(*dll_PyObject_GC_UnTrack)(ಶೂನ್ಯ *);
# endif
ಸ್ಥಿರ PyObject*(*dll__PyObject_Init)(PyObject *, PyTypeObject *);
ಸ್ಥಿರ PyObject* (*dll_PyObject_GetIter)(PyObject *);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*dll_PyObject_IsTrue)(PyObject *);
# ಆದರೆ defined(PY_ಆವೃತ್ತಿ_HEX) && PY_ಆವೃತ್ತಿ_HEX >= 0x02070000
ಸ್ಥಿರ iternextfunc dll_Py_Object_ಮುಂದಿನ_Not_Implemented;
# endif
ಸ್ಥಿರ PyObject* dll__Py_NoneStruct;
ಸ್ಥಿರ PyObject* _Py_ZeroStruct;
ಸ್ಥಿರ PyObject* dll__Py_TrueStruct;
# ಆದರೆ defined(PY_ಆವೃತ್ತಿ_HEX) && PY_ಆವೃತ್ತಿ_HEX >= 0x02020000
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*dll_PyType_IsSubtype)(PyTypeObject *, PyTypeObject *);
#  ifdef Py_DEBUG
ಸ್ಥಿರ ಶೂನ್ಯ (*dll__Py_NegativeRefcount)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fname, ಪೂರ್ಣಾಂಕ lineno, PyObject *op);
ಸ್ಥಿರ PyInt* dll_Py_Ref_ಒಟ್ಟು;
ಸ್ಥಿರ ಶೂನ್ಯ (*dll__Py_Dealloc)(PyObject *obj);
#  endif
# endif
# ಆದರೆ defined(PY_ಆವೃತ್ತಿ_HEX) && PY_ಆವೃತ್ತಿ_HEX >= 0x02030000
#  ಆದರೆ defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
ಸ್ಥಿರ ಶೂನ್ಯ (*dll__PyObject_DebugFree)(ಶೂನ್ಯ*);
ಸ್ಥಿರ ಶೂನ್ಯ* (*dll__PyObject_DebugMalloc)(size_t);
#  ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ ಶೂನ್ಯ* (*dll_PyObject_Malloc)(size_t);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_PyObject_Free)(ಶೂನ್ಯ*);
#  endif
# endif
# ifdef PY_USE_CAPSULE
ಸ್ಥಿರ PyObject* (*dll_PyCapsule_New)(ಶೂನ್ಯ *, ಅಕ್ಷರ *, PyCapsule_Destructor);
ಸ್ಥಿರ ಶೂನ್ಯ* (*dll_Py_Capsule_Get_ಸೂಚಕ)(PyObject *, ಅಕ್ಷರ *);
# ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ PyObject* (*dll_PyCObject_FromVoidPtr)(ಶೂನ್ಯ *cobj, ಶೂನ್ಯ (*destr)(ಶೂನ್ಯ *));
ಸ್ಥಿರ ಶೂನ್ಯ* (*dll_PyCObject_AsVoidPtr)(PyObject *);
# endif
# ಆದರೆ defined(PY_ಆವೃತ್ತಿ_HEX) && PY_ಆವೃತ್ತಿ_HEX >= 0x02070000
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ* dll_Py_NoSiteFlag;
# endif

ಸ್ಥಿರ HINSTANCE hinstPython = 0; // Instance of python.dll

// Imported exception objects
ಸ್ಥಿರ PyObject *imp_PyExc_AttributeError;
ಸ್ಥಿರ PyObject *imp_Py_Exc_ಸೂಚ್ಯಂಕ_Error;
ಸ್ಥಿರ PyObject *imp_Py_Exc_ಕೀಲಿ_Error;
ಸ್ಥಿರ PyObject *imp_Py_Exc_Keyboard_ಅಡಚಣೆ;
ಸ್ಥಿರ PyObject *imp_PyExc_TypeError;
ಸ್ಥಿರ PyObject *imp_Py_Exc_ಮೌಲ್ಯ_Error;
ಸ್ಥಿರ PyObject *imp_PyExc_SystemExit;
ಸ್ಥಿರ PyObject *imp_PyExc_RuntimeError;
ಸ್ಥಿರ PyObject *imp_PyExc_ImportError;
ಸ್ಥಿರ PyObject *imp_Py_Exc_ಅತಿಪ್ರವಾಹ_Error;

# define PyExc_AttributeError imp_PyExc_AttributeError
# define Py_Exc_ಸೂಚ್ಯಂಕ_Error imp_Py_Exc_ಸೂಚ್ಯಂಕ_Error
# define Py_Exc_ಕೀಲಿ_Error imp_Py_Exc_ಕೀಲಿ_Error
# define Py_Exc_Keyboard_ಅಡಚಣೆ imp_Py_Exc_Keyboard_ಅಡಚಣೆ
# define PyExc_TypeError imp_PyExc_TypeError
# define Py_Exc_ಮೌಲ್ಯ_Error imp_Py_Exc_ಮೌಲ್ಯ_Error
# define PyExc_SystemExit imp_PyExc_SystemExit
# define PyExc_RuntimeError imp_PyExc_RuntimeError
# define PyExc_ImportError imp_PyExc_ImportError
# define Py_Exc_ಅತಿಪ್ರವಾಹ_Error imp_Py_Exc_ಅತಿಪ್ರವಾಹ_Error

/*
 * Table of name to function pointer of python.
 */
# define PYTHON_PROC FARPROC
ಸ್ಥಿರ ರಚನೆ
{
    ಅಕ್ಷರ *name;
    PYTHON_PROC *ptr;
} python_funcname_ಕೋಷ್ಟಕ[] =
{
# ifdef PY_SSIZE_T_CLEAN
    {"_PyArg_Parse_SizeT", (PYTHON_PROC*)&dll_PyArg_Parse},
    {"_PyArg_ParseTuple_SizeT", (PYTHON_PROC*)&dll_PyArg_ParseTuple},
    {"_Py_BuildValue_SizeT", (PYTHON_PROC*)&dll_Py_Build_ಮೌಲ್ಯ},
# ಇಲ್ಲದಿದ್ದರೆ
    {"PyArg_Parse", (PYTHON_PROC*)&dll_PyArg_Parse},
    {"PyArg_ParseTuple", (PYTHON_PROC*)&dll_PyArg_ParseTuple},
    {"Py_BuildValue", (PYTHON_PROC*)&dll_Py_Build_ಮೌಲ್ಯ},
# endif
    {"PyMem_Free", (PYTHON_PROC*)&dll_PyMem_Free},
    {"PyMem_Malloc", (PYTHON_PROC*)&dll_PyMem_Malloc},
    {"PyDict_SetItemString", (PYTHON_PROC*)&dll_Py_Dict_ಗಣ_ಅಂಶ_String},
    {"PyErr_BadArgument", (PYTHON_PROC*)&dll_Py_Err_Bad_ವಾದ},
    {"PyErr_NewException", (PYTHON_PROC*)&dll_PyErr_NewException},
    {"PyErr_Clear", (PYTHON_PROC*)&dll_PyErr_Clear},
    {"PyErr_Format", (PYTHON_PROC*)&dll_PyErr_Format},
    {"PyErr_PrintEx", (PYTHON_PROC*)&dll_PyErr_PrintEx},
    {"PyErr_NoMemory", (PYTHON_PROC*)&dll_Py_Err_No_ಸ್ಮರಣೆ},
    {"PyErr_Occurred", (PYTHON_PROC*)&dll_PyErr_Occurred},
    {"PyErr_SetNone", (PYTHON_PROC*)&dll_Py_Err_ಗಣ_None},
    {"PyErr_SetString", (PYTHON_PROC*)&dll_Py_Err_ಗಣ_String},
    {"PyErr_SetObject", (PYTHON_PROC*)&dll_Py_Err_ಗಣ_Object},
    {"PyErr_ExceptionMatches", (PYTHON_PROC*)&dll_PyErr_ExceptionMatches},
    {"PyEval_InitThreads", (PYTHON_PROC*)&dll_Py_Eval_Init_ಎಳೆಗಳು},
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
    {"PyList_GetItem", (PYTHON_PROC*)&dll_Py_List_Get_ಅಂಶ},
    {"PyList_Append", (PYTHON_PROC*)&dll_PyList_Append},
    {"PyList_Insert", (PYTHON_PROC*)&dll_Py_List_ಸೇರಿಸು},
    {"PyList_New", (PYTHON_PROC*)&dll_PyList_New},
    {"PyList_SetItem", (PYTHON_PROC*)&dll_Py_List_ಗಣ_ಅಂಶ},
    {"PyList_Size", (PYTHON_PROC*)&dll_Py_List_ಗಾತ್ರ},
    {"PyList_Type", (PYTHON_PROC*)&dll_PyList_Type},
    {"PyTuple_GetItem", (PYTHON_PROC*)&dll_Py_Tuple_Get_ಅಂಶ},
    {"PyTuple_SetItem", (PYTHON_PROC*)&dll_Py_Tuple_ಗಣ_ಅಂಶ},
    {"PyTuple_New", (PYTHON_PROC*)&dll_PyTuple_New},
    {"PyTuple_Size", (PYTHON_PROC*)&dll_Py_Tuple_ಗಾತ್ರ},
    {"PyTuple_Type", (PYTHON_PROC*)&dll_PyTuple_Type},
    {"PySequence_Size", (PYTHON_PROC*)&dll_Py_Sequence_ಗಾತ್ರ},
    {"PySequence_Check", (PYTHON_PROC*)&dll_PySequence_Check},
    {"PySequence_GetItem", (PYTHON_PROC*)&dll_Py_Sequence_Get_ಅಂಶ},
    {"PySequence_Fast", (PYTHON_PROC*)&dll_PySequence_Fast},
    {"PySlice_GetIndicesEx", (PYTHON_PROC*)&dll_Py_ತುಂಡು_Get_Indices_Ex},
    {"PyImport_ImportModule", (PYTHON_PROC*)&dll_PyImport_ImportModule},
    {"PyDict_GetItemString", (PYTHON_PROC*)&dll_Py_Dict_Get_ಅಂಶ_String},
    {"PyDict_Next", (PYTHON_PROC*)&dll_Py_Dict_ಮುಂದಿನ},
    {"PyDict_New", (PYTHON_PROC*)&dll_PyDict_New},
    {"PyDict_Type", (PYTHON_PROC*)&dll_PyDict_Type},
# ifndef PY_NO_MAPPING_ಕೀಲಿಗಳು
    {"PyMapping_Keys", (PYTHON_PROC*)&dll_Py_Mapping_ಕೀಲಿಗಳು},
# endif
    {"PyObject_GetItem", (PYTHON_PROC*)&dll_Py_Object_Get_ಅಂಶ},
    {"PyObject_CallMethod", (PYTHON_PROC*)&dll_Py_Object_Call_ವಿಧಾನ},
    {"PyMapping_Check", (PYTHON_PROC*)&dll_PyMapping_Check},
    {"PyIter_Next", (PYTHON_PROC*)&dll_Py_Iter_ಮುಂದಿನ},
    {"PyModule_GetDict", (PYTHON_PROC*)&dll_PyModule_GetDict},
    {"PyModule_AddObject", (PYTHON_PROC*)&dll_Py_Module_ಕೂಡಿಸು_Object},
    {"PyRun_SimpleString", (PYTHON_PROC*)&dll_Py_ಸತತ_Simple_String},
    {"PyRun_String", (PYTHON_PROC*)&dll_Py_ಸತತ_String},
    {"PyObject_GetAttrString", (PYTHON_PROC*)&dll_PyObject_GetAttrString},
    {"PyObject_HasAttrString", (PYTHON_PROC*)&dll_PyObject_HasAttrString},
    {"PyObject_SetAttrString", (PYTHON_PROC*)&dll_Py_Object_ಗಣ_Attr_String},
    {"PyObject_CallFunctionObjArgs", (PYTHON_PROC*)&dll_Py_Object_Call_Function_Obj_ವಾದಗಳು},
    {"PyObject_CallFunction", (PYTHON_PROC*)&dll_PyObject_CallFunction},
    {"PyObject_Call", (PYTHON_PROC*)&dll_PyObject_Call},
    {"PyObject_Repr", (PYTHON_PROC*)&dll_PyObject_Repr},
    {"PyString_AsString", (PYTHON_PROC*)&dll_PyString_AsString},
    {"PyString_AsStringAndSize", (PYTHON_PROC*)&dll_Py_String_As_String_And_ಗಾತ್ರ},
    {"PyString_FromString", (PYTHON_PROC*)&dll_PyString_FromString},
    {"PyString_FromFormat", (PYTHON_PROC*)&dll_PyString_FromFormat},
    {"PyString_FromStringAndSize", (PYTHON_PROC*)&dll_Py_String_From_String_And_ಗಾತ್ರ},
    {"PyString_Size", (PYTHON_PROC*)&dll_Py_String_ಗಾತ್ರ},
    {"PyString_Type", (PYTHON_PROC*)&dll_PyString_Type},
    {"PyUnicode_Type", (PYTHON_PROC*)&dll_PyUnicode_Type},
    {"PyFloat_Type", (PYTHON_PROC*)&dll_PyFloat_Type},
    {"PyFloat_AsDouble", (PYTHON_PROC*)&dll_PyFloat_AsDouble},
    {"PyFloat_FromDouble", (PYTHON_PROC*)&dll_PyFloat_FromDouble},
    {"PyImport_AddModule", (PYTHON_PROC*)&dll_Py_Import_ಕೂಡಿಸು_Module},
    {"PyNumber_Check", (PYTHON_PROC*)&dll_Py_ಸಂಖ್ಯೆ_Check},
    {"PyNumber_Long", (PYTHON_PROC*)&dll_Py_ಸಂಖ್ಯೆ_Long},
    {"PySys_SetObject", (PYTHON_PROC*)&dll_Py_Sys_ಗಣ_Object},
    {"PySys_GetObject", (PYTHON_PROC*)&dll_PySys_GetObject},
    {"PySys_SetArgv", (PYTHON_PROC*)&dll_Py_Sys_ಗಣ_Argv},
    {"PyType_Type", (PYTHON_PROC*)&dll_PyType_Type},
    {"PyFile_Type", (PYTHON_PROC*)&dll_PyFile_Type},
    {"PySlice_Type", (PYTHON_PROC*)&dll_Py_ತುಂಡು_Type},
    {"PyType_Ready", (PYTHON_PROC*)&dll_PyType_Ready},
    {"PyType_GenericAlloc", (PYTHON_PROC*)&dll_PyType_GenericAlloc},
    {"Py_FindMethod", (PYTHON_PROC*)&dll_Py_ಕಂಡುಹಿಡಿ_ವಿಧಾನ},
    {"Py_SetPythonHome", (PYTHON_PROC*)&dll_Py_ಗಣ_Python_Home},
    {"Py_Initialize", (PYTHON_PROC*)&dll_Py_Initialize},
    {"Py_Finalize", (PYTHON_PROC*)&dll_Py_Finalize},
    {"Py_IsInitialized", (PYTHON_PROC*)&dll_Py_IsInitialized},
    {"_PyObject_New", (PYTHON_PROC*)&dll__PyObject_New},
    {"_PyObject_GC_New", (PYTHON_PROC*)&dll__PyObject_GC_New},
# ifdef Py_underscore_GC
    {"_PyObject_GC_Del", (PYTHON_PROC*)&dll__PyObject_GC_Del},
    {"_PyObject_GC_UnTrack", (PYTHON_PROC*)&dll__PyObject_GC_UnTrack},
# ಇಲ್ಲದಿದ್ದರೆ
    {"PyObject_GC_Del", (PYTHON_PROC*)&dll_PyObject_GC_Del},
    {"PyObject_GC_UnTrack", (PYTHON_PROC*)&dll_PyObject_GC_UnTrack},
# endif
    {"PyObject_Init", (PYTHON_PROC*)&dll__PyObject_Init},
    {"PyObject_GetIter", (PYTHON_PROC*)&dll_PyObject_GetIter},
    {"PyObject_IsTrue", (PYTHON_PROC*)&dll_PyObject_IsTrue},
# ಆದರೆ defined(PY_ಆವೃತ್ತಿ_HEX) && PY_ಆವೃತ್ತಿ_HEX >= 0x02070000
    {"_PyObject_NextNotImplemented", (PYTHON_PROC*)&dll_Py_Object_ಮುಂದಿನ_Not_Implemented},
# endif
    {"_Py_NoneStruct", (PYTHON_PROC*)&dll__Py_NoneStruct},
    {"_Py_ZeroStruct", (PYTHON_PROC*)&dll__Py_ZeroStruct},
    {"_Py_TrueStruct", (PYTHON_PROC*)&dll__Py_TrueStruct},
# ಆದರೆ defined(PY_ಆವೃತ್ತಿ_HEX) && PY_ಆವೃತ್ತಿ_HEX >= 0x02020000
#  ifdef Py_DEBUG
    {"_Py_NegativeRefcount", (PYTHON_PROC*)&dll__Py_NegativeRefcount},
    {"_Py_RefTotal", (PYTHON_PROC*)&dll_Py_Ref_ಒಟ್ಟು},
    {"_Py_Dealloc", (PYTHON_PROC*)&dll__Py_Dealloc},
#  endif
    {"PyType_IsSubtype", (PYTHON_PROC*)&dll_PyType_IsSubtype},
# endif
# ಆದರೆ defined(PY_ಆವೃತ್ತಿ_HEX) && PY_ಆವೃತ್ತಿ_HEX >= 0x02030000
#  ಆದರೆ defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
    {"_PyObject_DebugFree", (PYTHON_PROC*)&dll__PyObject_DebugFree},
    {"_PyObject_DebugMalloc", (PYTHON_PROC*)&dll__PyObject_DebugMalloc},
#  ಇಲ್ಲದಿದ್ದರೆ
    {"PyObject_Malloc", (PYTHON_PROC*)&dll_PyObject_Malloc},
    {"PyObject_Free", (PYTHON_PROC*)&dll_PyObject_Free},
#  endif
# endif
# ಆದರೆ defined(PY_ಆವೃತ್ತಿ_HEX) && PY_ಆವೃತ್ತಿ_HEX >= 0x02050000 \
	&& SIZEOF_ಗಾತ್ರ_T != VIM_SIZEOF_INT
#  ifdef Py_DEBUG
    {"Py_InitModule4TraceRefs_64", (PYTHON_PROC*)&dll_Py_InitModule4},
#  ಇಲ್ಲದಿದ್ದರೆ
    {"Py_InitModule4_64", (PYTHON_PROC*)&dll_Py_InitModule4},
#  endif
# ಇಲ್ಲದಿದ್ದರೆ
#  ifdef Py_DEBUG
    {"Py_InitModule4TraceRefs", (PYTHON_PROC*)&dll_Py_InitModule4},
#  ಇಲ್ಲದಿದ್ದರೆ
    {"Py_InitModule4", (PYTHON_PROC*)&dll_Py_InitModule4},
#  endif
# endif
# ifdef PY_USE_CAPSULE
    {"PyCapsule_New", (PYTHON_PROC*)&dll_PyCapsule_New},
    {"PyCapsule_GetPointer", (PYTHON_PROC*)&dll_Py_Capsule_Get_ಸೂಚಕ},
# ಇಲ್ಲದಿದ್ದರೆ
    {"PyCObject_FromVoidPtr", (PYTHON_PROC*)&dll_PyCObject_FromVoidPtr},
    {"PyCObject_AsVoidPtr", (PYTHON_PROC*)&dll_PyCObject_AsVoidPtr},
# endif
# ಆದರೆ defined(PY_ಆವೃತ್ತಿ_HEX) && PY_ಆವೃತ್ತಿ_HEX >= 0x02070000
    {"Py_NoSiteFlag", (PYTHON_PROC*)&dll_Py_NoSiteFlag},
# endif
    {"", ಖಾಲಿ},
};

/*
 * Load library and get all pointers.
 * Parameter 'libname' provides name of DLL.
 * Return OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
python_runtime_link_init(ಅಕ್ಷರ *libname, ಪೂರ್ಣಾಂಕ verbose)
{
    ಪೂರ್ಣಾಂಕ i;
    PYTHON_PROC *ucs_as_encoded_string =
				   (PYTHON_PROC*)&py_PyUnicode_AsEncodedString;

# ಆದರೆ !(defined(PY_NO_RTLD_GLOBAL) && defined(PY3_NO_RTLD_GLOBAL)) && defined(UNIX) && defined(FEAT_PYTHON3)
    // Can't have Python and Python3 loaded at the same time.
    // It causes a crash, because RTLD_GLOBAL is needed for
    // standard C extension libraries of one or both python versions.
    ಆದರೆ (python3_loaded())
    {
	ಆದರೆ (verbose)
	    emsg(_(e_this_vim_cannot_execute_python_after_using_py3));
	ಹಿಂತಿರುಗಿ FAIL;
    }
# endif

    ಆದರೆ (hinstPython)
	ಹಿಂತಿರುಗಿ OK;
    hinstPython = ತುಂಬು_dll(libname);
    ಆದರೆ (!hinstPython)
    {
	ಆದರೆ (verbose)
	    semsg(_(e_could_not_ತುಂಬು_library_str_str), libname, ತುಂಬು_dll_error());
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಗಾಗಿ (i = 0; python_funcname_ಕೋಷ್ಟಕ[i].ptr; ++i)
    {
	ಆದರೆ ((*python_funcname_ಕೋಷ್ಟಕ[i].ptr = ಚಿಹ್ನೆ_from_dll(hinstPython,
			python_funcname_ಕೋಷ್ಟಕ[i].name)) == ಖಾಲಿ)
	{
	    close_dll(hinstPython);
	    hinstPython = 0;
	    ಆದರೆ (verbose)
		semsg(_(e_could_not_ತುಂಬು_library_function_str), python_funcname_ಕೋಷ್ಟಕ[i].name);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
    }

    // Load unicode functions separately as only the ucs2 or the ucs4 functions
    // will be present in the library.
    *ucs_as_encoded_string = ಚಿಹ್ನೆ_from_dll(hinstPython,
					     "PyUnicodeUCS2_AsEncodedString");
    ಆದರೆ (*ucs_as_encoded_string == ಖಾಲಿ)
	*ucs_as_encoded_string = ಚಿಹ್ನೆ_from_dll(hinstPython,
					     "PyUnicodeUCS4_AsEncodedString");
    ಆದರೆ (*ucs_as_encoded_string == ಖಾಲಿ)
    {
	close_dll(hinstPython);
	hinstPython = 0;
	ಆದರೆ (verbose)
	    semsg(_(e_could_not_ತುಂಬು_library_function_str), "PyUnicode_UCSX_*");
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * If python is enabled (there is installed python on Windows system) return
 * TRUE, else FALSE.
 */
    ಪೂರ್ಣಾಂಕ
python_enabled(ಪೂರ್ಣಾಂಕ verbose)
{
    ಹಿಂತಿರುಗಿ python_runtime_link_init((ಅಕ್ಷರ *)p_pydll, verbose) == OK;
}

/*
 * Load the standard Python exceptions - don't import the symbols from the
 * DLL, as this can cause errors (importing data symbols is not reliable).
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_exceptions(ಶೂನ್ಯ)
{
    PyObject *exmod = PyImport_ImportModule("exceptions");
    PyObject *exdict = PyModule_GetDict(exmod);
    imp_PyExc_AttributeError = Py_Dict_Get_ಅಂಶ_String(exdict, "AttributeError");
    imp_Py_Exc_ಸೂಚ್ಯಂಕ_Error = Py_Dict_Get_ಅಂಶ_String(exdict, "IndexError");
    imp_Py_Exc_ಕೀಲಿ_Error = Py_Dict_Get_ಅಂಶ_String(exdict, "KeyError");
    imp_Py_Exc_Keyboard_ಅಡಚಣೆ = Py_Dict_Get_ಅಂಶ_String(exdict, "KeyboardInterrupt");
    imp_PyExc_TypeError = Py_Dict_Get_ಅಂಶ_String(exdict, "TypeError");
    imp_Py_Exc_ಮೌಲ್ಯ_Error = Py_Dict_Get_ಅಂಶ_String(exdict, "ValueError");
    imp_PyExc_SystemExit = Py_Dict_Get_ಅಂಶ_String(exdict, "SystemExit");
    imp_PyExc_RuntimeError = Py_Dict_Get_ಅಂಶ_String(exdict, "RuntimeError");
    imp_PyExc_ImportError = Py_Dict_Get_ಅಂಶ_String(exdict, "ImportError");
    imp_Py_Exc_ಅತಿಪ್ರವಾಹ_Error = Py_Dict_Get_ಅಂಶ_String(exdict, "OverflowError");
    Py_XINCREF(imp_PyExc_AttributeError);
    Py_XINCREF(imp_Py_Exc_ಸೂಚ್ಯಂಕ_Error);
    Py_XINCREF(imp_Py_Exc_ಕೀಲಿ_Error);
    Py_XINCREF(imp_Py_Exc_Keyboard_ಅಡಚಣೆ);
    Py_XINCREF(imp_PyExc_TypeError);
    Py_XINCREF(imp_Py_Exc_ಮೌಲ್ಯ_Error);
    Py_XINCREF(imp_PyExc_SystemExit);
    Py_XINCREF(imp_PyExc_RuntimeError);
    Py_XINCREF(imp_PyExc_ImportError);
    Py_XINCREF(imp_Py_Exc_ಅತಿಪ್ರವಾಹ_Error);
    Py_XDECREF(exmod);
}
#ಆದರೆ_ಅಂತ್ಯ // DYNAMIC_PYTHON

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ initialised = 0;
#ವ್ಯಾಖ್ಯಾನಿಸು PYINITIALISED initialised
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ python_ಕೊನೆ_called = FALSE;

#ವ್ಯಾಖ್ಯಾನಿಸು DESTRUCTOR_ಮುಕ್ತಾಯ(self) self->ob_type->tp_free((PyObject*)self);

#ವ್ಯಾಖ್ಯಾನಿಸು WIN_PYTHON_REF(win) win->w_python_ref
#ವ್ಯಾಖ್ಯಾನಿಸು BUF_PYTHON_REF(buf) buf->b_python_ref
#ವ್ಯಾಖ್ಯಾನಿಸು TAB_PYTHON_REF(tab) tab->tp_python_ref

ಸ್ಥಿರ PyObject *OutputGetattr(PyObject *, ಅಕ್ಷರ *);
ಸ್ಥಿರ PyObject *ಬಫರ್_Getattr(PyObject *, ಅಕ್ಷರ *);
ಸ್ಥಿರ PyObject *ಕಿಟಕಿ_Getattr(PyObject *, ಅಕ್ಷರ *);
ಸ್ಥಿರ PyObject *Tab_ಪುಟ_Getattr(PyObject *, ಅಕ್ಷರ *);
ಸ್ಥಿರ PyObject *RangeGetattr(PyObject *, ಅಕ್ಷರ *);
ಸ್ಥಿರ PyObject *DictionaryGetattr(PyObject *, ಅಕ್ಷರ*);
ಸ್ಥಿರ PyObject *ListGetattr(PyObject *, ಅಕ್ಷರ *);
ಸ್ಥಿರ PyObject *TupleGetattr(PyObject *, ಅಕ್ಷರ *);
ಸ್ಥಿರ PyObject *FunctionGetattr(PyObject *, ಅಕ್ಷರ *);

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ Py_ಭೇಟಿ
# define Py_ಭೇಟಿ(obj) ಭೇಟಿ(obj, arg)
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ Py_CLEAR
# define Py_CLEAR(obj) \
    { \
	Py_XDECREF(obj); \
	obj = ಖಾಲಿ; \
    }
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    ಸ್ಥಿರ ಶೂನ್ಯ *
py_memsave(ಶೂನ್ಯ *p, size_t len)
{
    ಶೂನ್ಯ	*r;

    ಆದರೆ (!(r = PyMem_Malloc(len)))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    mch_memmove(r, p, len);
    ಹಿಂತಿರುಗಿ r;
}

# define PY_STRSAVE(s) ((char_u *) py_memsave(s, STRLEN(s) + 1))
#ಆದರೆ_ಅಂತ್ಯ

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ Py_ತುಂಡು_Object Py_ತುಂಡು_Object_T;

/*
 * Include the code shared with if_python3.c
 */
#ಅನ್ತರ್ಭಾವ "if_py_both.h"


///////////////////////////////////////////////////////
// Internal function prototypes.

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ PythonMod_Init(ಶೂನ್ಯ);


///////////////////////////////////////////////////////
// 1. Python interpreter main program.

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ PY_CAN_RECURSE
ಸ್ಥಿರ PyThreadState *saved_python_thread = ಖಾಲಿ;

/*
 * Suspend a thread of the Python interpreter, other threads are allowed to
 * run.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
Python_SaveThread(ಶೂನ್ಯ)
{
    saved_python_thread = PyEval_SaveThread();
}

/*
 * Restore a thread of the Python interpreter, waits for other threads to
 * block.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
Python_RestoreThread(ಶೂನ್ಯ)
{
    PyEval_RestoreThread(saved_python_thread);
    saved_python_thread = ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

    ಶೂನ್ಯ
python_ಕೊನೆ(ಶೂನ್ಯ)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ recurse = 0;

    // If a crash occurs while doing this, don't try again.
    ಆದರೆ (recurse != 0)
	ಹಿಂತಿರುಗಿ;

    python_ಕೊನೆ_called = TRUE;
    ++recurse;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DYNAMIC_PYTHON
    ಆದರೆ (hinstPython && Py_IsInitialized())
    {
# ifdef PY_CAN_RECURSE
	PyGILState_Ensure();
# ಇಲ್ಲದಿದ್ದರೆ
	Python_RestoreThread();	    // enter python
# endif
	Py_Finalize();
    }
#ಇಲ್ಲದಿದ್ದರೆ
    ಆದರೆ (Py_IsInitialized())
    {
# ifdef PY_CAN_RECURSE
	PyGILState_Ensure();
# ಇಲ್ಲದಿದ್ದರೆ
	Python_RestoreThread();	    // enter python
# endif
	Py_Finalize();
    }
#ಆದರೆ_ಅಂತ್ಯ

    --recurse;
}

#ಆದರೆ defined(DYNAMIC_PYTHON) && defined(FEAT_PYTHON3)
    ಪೂರ್ಣಾಂಕ
python_loaded(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (hinstPython != 0);
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಅಕ್ಷರ *py_home_buf = ಖಾಲಿ;

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
Python_Init(ಶೂನ್ಯ)
{
    ಆದರೆ (!initialised)
    {
#ಆದರೆ defined(PY_ಆವೃತ್ತಿ_HEX) && PY_ಆವೃತ್ತಿ_HEX >= 0x02070000
	PyObject *site;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DYNAMIC_PYTHON
	ಆದರೆ (!python_enabled(TRUE))
	{
	    emsg(_(e_sorry_this_command_is_disabled_python_library_could_not_be_ಸಿಕ್ಕಿತು));
	    ಹೋಗು fail;
	}
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ (*p_pyhome != NUL)
	{
	    // The string must not change later, make a copy in static memory.
	    py_home_buf = (ಅಕ್ಷರ *)vim_strsave(p_pyhome);
	    ಆದರೆ (py_home_buf != ಖಾಲಿ)
		Py_ಗಣ_Python_Home(py_home_buf);
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ PYTHON_HOME
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (mch_getenv((char_u *)"PYTHONHOME") == ಖಾಲಿ)
	    Py_ಗಣ_Python_Home(PYTHON_HOME);
#ಆದರೆ_ಅಂತ್ಯ

	init_structs();

#ಆದರೆ defined(PY_ಆವೃತ್ತಿ_HEX) && PY_ಆವೃತ್ತಿ_HEX >= 0x02070000
	// Disable implicit 'import site', because it may cause Vim to exit
	// when it can't be found.
	Py_NoSiteFlag++;
#ಆದರೆ_ಅಂತ್ಯ

	Py_Initialize();

#ಆದರೆ defined(PY_ಆವೃತ್ತಿ_HEX) && PY_ಆವೃತ್ತಿ_HEX >= 0x02070000
	// 'import site' explicitly.
	site = PyImport_ImportModule("site");
	ಆದರೆ (site == ಖಾಲಿ)
	{
	    emsg(_(e_sorry_this_command_is_disabled_python_site_module_could_not_be_loaded));
	    ಹೋಗು fail;
	}
	Py_DECREF(site);
#ಆದರೆ_ಅಂತ್ಯ

	// Initialise threads, and below save the state using
	// PyEval_SaveThread.  Without the call to PyEval_SaveThread, thread
	// specific state (such as the system trace hook), will be lost
	// between invocations of Python code.
	Py_Eval_Init_ಎಳೆಗಳು();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DYNAMIC_PYTHON
	get_exceptions();
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ (PythonIO_Init_io())
	    ಹೋಗು fail;

	ಆದರೆ (PythonMod_Init())
	    ಹೋಗು fail;

	globals = PyModule_GetDict(Py_Import_ಕೂಡಿಸು_Module("__main__"));

	// Remove the element from sys.path that was added because of our
	// argv[0] value in PythonMod_Init().  Previously we used an empty
	// string, but depending on the OS we then get an empty entry or
	// the current directory in sys.path.
	Py_ಸತತ_Simple_String("import sys; sys.path = filter(lambda x: x != '/must>not&exist', sys.path)");

	// lock is created and acquired in PyEval_InitThreads() and thread
	// state is created in Py_Initialize()
	// there _PyGILState_NoteThreadState() also sets gilcounter to 1
	// (python must have threads enabled!)
	// so the following does both: unlock GIL and save thread state in TLS
	// without deleting thread state
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ PY_CAN_RECURSE
	saved_python_thread =
#ಆದರೆ_ಅಂತ್ಯ
	    PyEval_SaveThread();

	initialised = 1;
    }

    ಹಿಂತಿರುಗಿ 0;

fail:
    // We call PythonIO_Flush() here to print any Python errors.
    // This is OK, as it is possible to call this function even
    // if PythonIO_Init_io() has not completed successfully (it will
    // not do anything in this case).
    PythonIO_Flush();
    ಹಿಂತಿರುಗಿ -1;
}

/*
 * External interface
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
DoPyCommand(
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ		*cmd,
    dict_T*		locals,
    rangeinitializer	init_range,
    runner		ಸತತ,
    ಶೂನ್ಯ		*arg)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ PY_CAN_RECURSE
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ		recursive = 0;
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    ಅಕ್ಷರ		*saved_locale;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ PY_CAN_RECURSE
    PyGILState_STATE	pygilstate;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ PY_CAN_RECURSE
    ಆದರೆ (recursive)
    {
	emsg(_(e_cannot_invoke_python_recursively));
	ಹಿಂತಿರುಗಿ;
    }
    ++recursive;
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (python_ಕೊನೆ_called)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (Python_Init())
	ಹೋಗು theend;

    init_range(arg);

    Python_Release_Vim();	    // leave Vim

#ಆದರೆ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    // Python only works properly when the LC_NUMERIC locale is "C".
    saved_locale = setlocale(LC_NUMERIC, ಖಾಲಿ);
    ಆದರೆ (saved_locale == ಖಾಲಿ || STRCMP(saved_locale, "C") == 0)
	saved_locale = ಖಾಲಿ;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Need to make a copy, value may change when setting new locale.
	saved_locale = (ಅಕ್ಷರ *) PY_STRSAVE(saved_locale);
	(ಶೂನ್ಯ)setlocale(LC_NUMERIC, "C");
    }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ PY_CAN_RECURSE
    pygilstate = PyGILState_Ensure();
#ಇಲ್ಲದಿದ್ದರೆ
    Python_RestoreThread();	    // enter python
#ಆದರೆ_ಅಂತ್ಯ

    ಸತತ((ಅಕ್ಷರ *) cmd, locals, arg
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ PY_CAN_RECURSE
	    , &pygilstate
#ಆದರೆ_ಅಂತ್ಯ
	    );

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ PY_CAN_RECURSE
    PyGILState_Release(pygilstate);
#ಇಲ್ಲದಿದ್ದರೆ
    Python_SaveThread();	    // leave python
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    ಆದರೆ (saved_locale != ಖಾಲಿ)
    {
	(ಶೂನ್ಯ)setlocale(LC_NUMERIC, saved_locale);
	PyMem_Free(saved_locale);
    }
#ಆದರೆ_ಅಂತ್ಯ

    Python_ಬೀಗ_Vim();		    // enter vim
    PythonIO_Flush();

theend:
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ PY_CAN_RECURSE
    --recursive;
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ;
}

/*
 * ":python"
 */
    ಶೂನ್ಯ
ex_python(exarg_T *eap)
{
    char_u *script;

    script = script_get(eap, eap->arg);
    ಆದರೆ (!eap->skip)
    {
	ಆದರೆ (p_pyx == 0)
	    p_pyx = 2;

	DoPyCommand(script == ಖಾಲಿ ? (ಅಕ್ಷರ *) eap->arg : (ಅಕ್ಷರ *) script,
		ಖಾಲಿ,
		init_range_cmd,
		(runner) ಸತತ_cmd,
		(ಶೂನ್ಯ *) eap);
    }
    vim_free(script);
}

#ವ್ಯಾಖ್ಯಾನಿಸು ಬಫರ್_ಗಾತ್ರ 1024

/*
 * ":pyfile"
 */
    ಶೂನ್ಯ
ex_pyfile(exarg_T *eap)
{
    ಸ್ಥಿರ ಅಕ್ಷರ ಬಫರ್[ಬಫರ್_ಗಾತ್ರ];
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file = (ಅಕ್ಷರ *)eap->arg;
    ಅಕ್ಷರ *p;

    ಆದರೆ (p_pyx == 0)
	p_pyx = 2;

    // Have to do it like this. PyRun_SimpleFile requires you to pass a
    // stdio file pointer, but Vim and the Python DLL are compiled with
    // different options under Windows, meaning that stdio pointers aren't
    // compatible between the two. Yuk.
    //
    // Put the string "execfile('file')" into buffer. But, we need to
    // escape any backslashes or single quotes in the file name, so that
    // Python won't mangle the file name.
    strcpy(ಬಫರ್, "execfile('");
    p = ಬಫರ್ + 10; // size of "execfile('"

    ಆಗ (*file && p < ಬಫರ್ + (ಬಫರ್_ಗಾತ್ರ - 3))
    {
	ಆದರೆ (*file == '\\' || *file == '\'')
	    *p++ = '\\';
	*p++ = *file++;
    }

    // If we didn't finish the file name, we hit a buffer overflow
    ಆದರೆ (*file != '\0')
	ಹಿಂತಿರುಗಿ;

    // Put in the terminating "')" and a null
    *p++ = '\'';
    *p++ = ')';
    *p++ = '\0';

    // Execute the file
    DoPyCommand(ಬಫರ್,
	    ಖಾಲಿ,
	    init_range_cmd,
	    (runner) ಸತತ_cmd,
	    (ಶೂನ್ಯ *) eap);
}

    ಶೂನ್ಯ
ex_pydo(exarg_T *eap)
{
    ಆದರೆ (p_pyx == 0)
	p_pyx = 2;

    DoPyCommand((ಅಕ್ಷರ *)eap->arg,
	    ಖಾಲಿ,
	    init_range_cmd,
	    (runner)ಸತತ_do,
	    (ಶೂನ್ಯ *)eap);
}

///////////////////////////////////////////////////////
// 2. Python output stream: writes output via [e]msg().

// Implementation functions

    ಸ್ಥಿರ PyObject *
OutputGetattr(PyObject *self, ಅಕ್ಷರ *name)
{
    ಆದರೆ (strcmp(name, "softspace") == 0)
	ಹಿಂತಿರುಗಿ PyInt_FromLong(((OutputObject *)(self))->softspace);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(name, "__members__") == 0)
	ಹಿಂತಿರುಗಿ ObjectDir(ಖಾಲಿ, OutputAttrs);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(name, "errors") == 0)
	ಹಿಂತಿರುಗಿ PyString_FromString("strict");
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(name, "encoding") == 0)
	ಹಿಂತಿರುಗಿ PyString_FromString(ENC_OPT);
    ಹಿಂತಿರುಗಿ Py_ಕಂಡುಹಿಡಿ_ವಿಧಾನ(OutputMethods, self, name);
}

///////////////////////////////////////////////////////
// 3. Implementation of the Vim module for Python

// Window type - Implementation functions
// --------------------------------------

#ವ್ಯಾಖ್ಯಾನಿಸು ಕಿಟಕಿ_Type_Check(obj) ((obj)->ob_type == &ಕಿಟಕಿ_Type)

// Buffer type - Implementation functions
// --------------------------------------

#ವ್ಯಾಖ್ಯಾನಿಸು ಬಫರ್_Type_Check(obj) ((obj)->ob_type == &ಬಫರ್_Type)

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಬಫರ್_Ass_ಅಂಶ(PyObject *, PyInt, PyObject *);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಬಫರ್_Ass_ತುಂಡು(PyObject *, PyInt, PyInt, PyObject *);

// Line range type - Implementation functions
// --------------------------------------

#ವ್ಯಾಖ್ಯಾನಿಸು RangeType_Check(obj) ((obj)->ob_type == &RangeType)

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ Range_Ass_ಅಂಶ(PyObject *, PyInt, PyObject *);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ Range_Ass_ತುಂಡು(PyObject *, PyInt, PyInt, PyObject *);

// Current objects type - Implementation functions
// -----------------------------------------------

ಸ್ಥಿರ PySequenceMethods ಬಫರ್_As_Seq = {
    (PyInquiry)		ಬಫರ್_ಉದ್ದಳತೆ,	    // sq_length,    len(x)
    (binaryfunc)	0,		    // BufferConcat, sq_concat, x+y
    (PyIntArgFunc)	0,		    // BufferRepeat, sq_repeat, x*n
    (PyIntArgFunc)	ಬಫರ್_ಅಂಶ,	    // sq_item,      x[i]
    (PyIntIntArgFunc)	ಬಫರ್_ತುಂಡು,	    // sq_slice,     x[i:j]
    (PyIntObjArgProc)	ಬಫರ್_Ass_ಅಂಶ,	    // sq_ass_item,  x[i]=v
    (PyIntIntObjArgProc) ಬಫರ್_Ass_ತುಂಡು,    // sq_ass_slice, x[i:j]=v
    (objobjproc)	0,
    (binaryfunc)	0,
    0,
};

// Buffer object - Implementation

    ಸ್ಥಿರ PyObject *
ಬಫರ್_Getattr(PyObject *self, ಅಕ್ಷರ *name)
{
    PyObject *r;

    ಆದರೆ ((r = ಬಫರ್_Attr_Valid((ಬಫರ್_Object *)(self), name)))
	ಹಿಂತಿರುಗಿ r;

    ಆದರೆ (Check_ಬಫರ್((ಬಫರ್_Object *)(self)))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    r = ಬಫರ್_Attr((ಬಫರ್_Object *)(self), name);
    ಆದರೆ (r || PyErr_Occurred())
	ಹಿಂತಿರುಗಿ r;
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ Py_ಕಂಡುಹಿಡಿ_ವಿಧಾನ(ಬಫರ್_Methods, self, name);
}

//////////////////

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಬಫರ್_Ass_ಅಂಶ(PyObject *self, PyInt n, PyObject *val)
{
    ಹಿಂತಿರುಗಿ RBAs_ಅಂಶ((ಬಫರ್_Object *)(self), n, val, 1, -1, ಖಾಲಿ);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಬಫರ್_Ass_ತುಂಡು(PyObject *self, PyInt lo, PyInt hi, PyObject *val)
{
    ಹಿಂತಿರುಗಿ RBAs_ತುಂಡು((ಬಫರ್_Object *)(self), lo, hi, val, 1, -1, ಖಾಲಿ);
}

ಸ್ಥಿರ PySequenceMethods RangeAsSeq = {
    (PyInquiry)		Range_ಉದ್ದಳತೆ,	      // sq_length,    len(x)
    (binaryfunc)	0, /* RangeConcat, */ // sq_concat,    x+y
    (PyIntArgFunc)	0, /* RangeRepeat, */ // sq_repeat,    x*n
    (PyIntArgFunc)	Range_ಅಂಶ,	      // sq_item,      x[i]
    (PyIntIntArgFunc)	Range_ತುಂಡು,	      // sq_slice,     x[i:j]
    (PyIntObjArgProc)	Range_Ass_ಅಂಶ,	      // sq_ass_item,  x[i]=v
    (PyIntIntObjArgProc) Range_Ass_ತುಂಡು,	      // sq_ass_slice, x[i:j]=v
    (objobjproc)	0,
#ಆದರೆ PY_MAJOR_ಆವೃತ್ತಿ >= 2
    (binaryfunc)	0,
    0,
#ಆದರೆ_ಅಂತ್ಯ
};

// Line range object - Implementation

    ಸ್ಥಿರ PyObject *
RangeGetattr(PyObject *self, ಅಕ್ಷರ *name)
{
    ಆದರೆ (strcmp(name, "start") == 0)
	ಹಿಂತಿರುಗಿ Py_Build_ಮೌಲ್ಯ(Py_ssize_t_fmt, ((RangeObject *)(self))->ಆರಂಭ - 1);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(name, "end") == 0)
	ಹಿಂತಿರುಗಿ Py_Build_ಮೌಲ್ಯ(Py_ssize_t_fmt, ((RangeObject *)(self))->ಕೊನೆ - 1);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(name, "__members__") == 0)
	ಹಿಂತಿರುಗಿ ObjectDir(ಖಾಲಿ, RangeAttrs);
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ Py_ಕಂಡುಹಿಡಿ_ವಿಧಾನ(RangeMethods, self, name);
}

////////////////

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
Range_Ass_ಅಂಶ(PyObject *self, PyInt n, PyObject *val)
{
    ಹಿಂತಿರುಗಿ RBAs_ಅಂಶ(((RangeObject *)(self))->buf, n, val,
		     ((RangeObject *)(self))->ಆರಂಭ,
		     ((RangeObject *)(self))->ಕೊನೆ,
		     &((RangeObject *)(self))->ಕೊನೆ);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
Range_Ass_ತುಂಡು(PyObject *self, PyInt lo, PyInt hi, PyObject *val)
{
    ಹಿಂತಿರುಗಿ RBAs_ತುಂಡು(((RangeObject *)(self))->buf, lo, hi, val,
		      ((RangeObject *)(self))->ಆರಂಭ,
		      ((RangeObject *)(self))->ಕೊನೆ,
		      &((RangeObject *)(self))->ಕೊನೆ);
}

// TabPage object - Implementation

    ಸ್ಥಿರ PyObject *
Tab_ಪುಟ_Getattr(PyObject *self, ಅಕ್ಷರ *name)
{
    PyObject *r;

    ಆದರೆ ((r = Tab_ಪುಟ_Attr_Valid((Tab_ಪುಟ_Object *)(self), name)))
	ಹಿಂತಿರುಗಿ r;

    ಆದರೆ (Check_Tab_ಪುಟ((Tab_ಪುಟ_Object *)(self)))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    r = Tab_ಪುಟ_Attr((Tab_ಪುಟ_Object *)(self), name);
    ಆದರೆ (r || PyErr_Occurred())
	ಹಿಂತಿರುಗಿ r;
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ Py_ಕಂಡುಹಿಡಿ_ವಿಧಾನ(Tab_ಪುಟ_Methods, self, name);
}

// Window object - Implementation

    ಸ್ಥಿರ PyObject *
ಕಿಟಕಿ_Getattr(PyObject *self, ಅಕ್ಷರ *name)
{
    PyObject *r;

    ಆದರೆ ((r = ಕಿಟಕಿ_Attr_Valid((ಕಿಟಕಿ_Object *)(self), name)))
	ಹಿಂತಿರುಗಿ r;

    ಆದರೆ (Check_ಕಿಟಕಿ((ಕಿಟಕಿ_Object *)(self)))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    r = ಕಿಟಕಿ_Attr((ಕಿಟಕಿ_Object *)(self), name);
    ಆದರೆ (r || PyErr_Occurred())
	ಹಿಂತಿರುಗಿ r;
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ Py_ಕಂಡುಹಿಡಿ_ವಿಧಾನ(ಕಿಟಕಿ_Methods, self, name);
}

// Tab page list object - Definitions

ಸ್ಥಿರ PySequenceMethods TabListAsSeq = {
    (PyInquiry)		Tab_List_ಉದ್ದಳತೆ,	    // sq_length,    len(x)
    (binaryfunc)	0,		    // sq_concat,    x+y
    (PyIntArgFunc)	0,		    // sq_repeat,    x*n
    (PyIntArgFunc)	Tab_List_ಅಂಶ,	    // sq_item,      x[i]
    (PyIntIntArgFunc)	0,		    // sq_slice,     x[i:j]
    (PyIntObjArgProc)	0,		    // sq_ass_item,  x[i]=v
    (PyIntIntObjArgProc) 0,		    // sq_ass_slice, x[i:j]=v
    (objobjproc)	0,
#ಆದರೆ PY_MAJOR_ಆವೃತ್ತಿ >= 2
    (binaryfunc)	0,
    0,
#ಆದರೆ_ಅಂತ್ಯ
};

// Window list object - Definitions

ಸ್ಥಿರ PySequenceMethods WinListAsSeq = {
    (PyInquiry)		Win_List_ಉದ್ದಳತೆ,	    // sq_length,    len(x)
    (binaryfunc)	0,		    // sq_concat,    x+y
    (PyIntArgFunc)	0,		    // sq_repeat,    x*n
    (PyIntArgFunc)	Win_List_ಅಂಶ,	    // sq_item,      x[i]
    (PyIntIntArgFunc)	0,		    // sq_slice,     x[i:j]
    (PyIntObjArgProc)	0,		    // sq_ass_item,  x[i]=v
    (PyIntIntObjArgProc) 0,		    // sq_ass_slice, x[i:j]=v
    (objobjproc)	0,
#ಆದರೆ PY_MAJOR_ಆವೃತ್ತಿ >= 2
    (binaryfunc)	0,
    0,
#ಆದರೆ_ಅಂತ್ಯ
};

// External interface

    ಶೂನ್ಯ
python_ಬಫರ್_free(buf_T *buf)
{
    ಬಫರ್_Object *bp = BUF_PYTHON_REF(buf);
    ಆದರೆ (bp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    bp->buf = INVALID_ಬಫರ್_ಮೌಲ್ಯ;
    BUF_PYTHON_REF(buf) = ಖಾಲಿ;
}

    ಶೂನ್ಯ
python_ಕಿಟಕಿ_free(win_T *win)
{
    ಕಿಟಕಿ_Object *wp = WIN_PYTHON_REF(win);
    ಆದರೆ (wp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    wp->win = INVALID_ಕಿಟಕಿ_ಮೌಲ್ಯ;
    WIN_PYTHON_REF(win) = ಖಾಲಿ;
}

    ಶೂನ್ಯ
python_tabpage_free(tabpage_T *tab)
{
    Tab_ಪುಟ_Object *tp = TAB_PYTHON_REF(tab);
    ಆದರೆ (tp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    tp->tab = INVALID_TABPAGE_ಮೌಲ್ಯ;
    TAB_PYTHON_REF(tab) = ಖಾಲಿ;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
PythonMod_Init(ಶೂನ್ಯ)
{
    // The special value is removed from sys.path in Python_Init().
    ಸ್ಥಿರ ಅಕ್ಷರ	*(argv[2]) = {"/must>not&exist/foo", ಖಾಲಿ};

    ಆದರೆ (init_types())
	ಹಿಂತಿರುಗಿ -1;

    // Set sys.argv[] to avoid a crash in warn().
    Py_Sys_ಗಣ_Argv(1, argv);

    vim_module = Py_InitModule4("vim", VimMethods, (ಅಕ್ಷರ *)ಖಾಲಿ,
				(PyObject *)ಖಾಲಿ, PYTHON_API_ಆವೃತ್ತಿ);

    ಆದರೆ (populate_module(vim_module))
	ಹಿಂತಿರುಗಿ -1;

    ಆದರೆ (init_sys_ಮಾರ್ಗ())
	ಹಿಂತಿರುಗಿ -1;

    ಹಿಂತಿರುಗಿ 0;
}

//////////////////////////////////////////////////////////////////////////
// 4. Utility functions for handling the interface between Vim and Python.

// Convert a Vim line into a Python string.
// All internal newlines are replaced by null characters.
//
// On errors, the Python exception data is set, and NULL is returned.
    ಸ್ಥಿರ PyObject *
ಸಾಲು_ಗಮ್ಯ_String(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str)
{
    PyObject *ಫಲಿತಾಂಶ;
    PyInt len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(str);
    ಅಕ್ಷರ *p;

    // Allocate a Python string object, with uninitialised contents. We
    // must do it this way, so that we can modify the string in place
    // later. See the Python source, Objects/stringobject.c for details.
    ಫಲಿತಾಂಶ = Py_String_From_String_And_ಗಾತ್ರ(ಖಾಲಿ, len);
    ಆದರೆ (ಫಲಿತಾಂಶ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    p = PyString_AsString(ಫಲಿತಾಂಶ);

    ಆಗ (*str)
    {
	ಆದರೆ (*str == '\n')
	    *p = '\0';
	ಇಲ್ಲದಿದ್ದರೆ
	    *p = *str;

	++p;
	++str;
    }

    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

    ಸ್ಥಿರ PyObject *
DictionaryGetattr(PyObject *self, ಅಕ್ಷರ *name)
{
    DictionaryObject	*this = ((DictionaryObject *) (self));

    ಆದರೆ (strcmp(name, "locked") == 0)
	ಹಿಂತಿರುಗಿ PyInt_FromLong(this->dict->dv_ಬೀಗ);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(name, "scope") == 0)
	ಹಿಂತಿರುಗಿ PyInt_FromLong(this->dict->dv_scope);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(name, "__members__") == 0)
	ಹಿಂತಿರುಗಿ ObjectDir(ಖಾಲಿ, DictionaryAttrs);

    ಹಿಂತಿರುಗಿ Py_ಕಂಡುಹಿಡಿ_ವಿಧಾನ(DictionaryMethods, self, name);
}

    ಸ್ಥಿರ PyObject *
ListGetattr(PyObject *self, ಅಕ್ಷರ *name)
{
    ಆದರೆ (strcmp(name, "locked") == 0)
	ಹಿಂತಿರುಗಿ PyInt_FromLong(((ListObject *)(self))->list->lv_ಬೀಗ);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(name, "__members__") == 0)
	ಹಿಂತಿರುಗಿ ObjectDir(ಖಾಲಿ, ListAttrs);

    ಹಿಂತಿರುಗಿ Py_ಕಂಡುಹಿಡಿ_ವಿಧಾನ(ListMethods, self, name);
}

    ಸ್ಥಿರ PyObject *
TupleGetattr(PyObject *self, ಅಕ್ಷರ *name)
{
    ಆದರೆ (strcmp(name, "locked") == 0)
	ಹಿಂತಿರುಗಿ PyInt_FromLong(((TupleObject *)(self))->tuple->tv_ಬೀಗ);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(name, "__members__") == 0)
	ಹಿಂತಿರುಗಿ ObjectDir(ಖಾಲಿ, TupleAttrs);

    ಹಿಂತಿರುಗಿ Py_ಕಂಡುಹಿಡಿ_ವಿಧಾನ(TupleMethods, self, name);
}

    ಸ್ಥಿರ PyObject *
FunctionGetattr(PyObject *self, ಅಕ್ಷರ *name)
{
    PyObject	*r;

    r = FunctionAttr((FunctionObject *)(self), name);

    ಆದರೆ (r || PyErr_Occurred())
	ಹಿಂತಿರುಗಿ r;
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ Py_ಕಂಡುಹಿಡಿ_ವಿಧಾನ(FunctionMethods, self, name);
}

    ಶೂನ್ಯ
do_pyeval(char_u *str, dict_T *locals, typval_T *rettv)
{
    DoPyCommand((ಅಕ್ಷರ *) str,
	    locals,
	    init_range_eval,
	    (runner) ಸತತ_eval,
	    (ಶೂನ್ಯ *) rettv);
    ಆದರೆ (rettv->v_type == VAR_UNKNOWN)
    {
	rettv->v_type = VAR_ಸಂಖ್ಯೆ;
	rettv->vval.v_ಸಂಖ್ಯೆ = 0;
    }
}

    ಪೂರ್ಣಾಂಕ
ಗಣ_ref_ಒಳ_python(ಪೂರ್ಣಾಂಕ copyID)
{
    ಹಿಂತಿರುಗಿ ಗಣ_ref_ಒಳ_py(copyID);
}
