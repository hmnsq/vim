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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <limits.h>

// uncomment this if used with the debug version of python.
// Checked on 2.7.4.
// #define Py_DEBUG
// Note: most of time you can add -DPy_DEBUG to CFLAGS in place of uncommenting
// uncomment this if used with the debug version of python, but without its
// allocator
// #define Py_DEBUG_NO_PYMALLOC

// Python.h defines _POSIX_THREADS itself (if needed)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _POSIX_THREADS
# undef _POSIX_THREADS
#ਜੇ_ਅੰਤ

#ਜੇ defined(MSWIN) && defined(HAVE_FCNTL_H)
# undef HAVE_FCNTL_H
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _DEBUG
# undef _DEBUG
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_STRFTIME
# undef HAVE_STRFTIME
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_STRING_H
# undef HAVE_STRING_H
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_PUTENV
# undef HAVE_PUTENV
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _POSIX_C_SOURCE
# undef _POSIX_C_SOURCE	// pyconfig.h defines it as well.
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ XOPEN_ਸਰੋਤ
# undef XOPEN_ਸਰੋਤ	// pyconfig.h defines it as well.
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PY_SSIZE_T_CLEAN

#ਅਨ੍ਤਰ੍ਭਾਵ <Python.h>

#ਜੇ !defined(PY_ਸੰਸਕਰਣ_HEX) || PY_ਸੰਸਕਰਣ_HEX < 0x02050000
# undef PY_SSIZE_T_CLEAN
#ਜੇ_ਅੰਤ

// these are NULL for Python 2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ERRORS_DECODE_ARG ਨਿਰਾ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ERRORS_ENCODE_ARG ERRORS_DECODE_ARG

#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ ਮੁੱਖ // Defined in python.h - aargh
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ HAVE_FCNTL_H // Clash with os_win32.h

// Perhaps leave this out for Python 2.6, which supports bytes?
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Py_ਬਾਈਟਾਂ_From_String      PyString_FromString
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Py_ਬਾਈਟਾਂ_Check		PyString_Check
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Py_ਬਾਈਟਾਂ_As_String_And_ਮਾਪ Py_String_As_String_And_ਮਾਪ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Py_ਬਾਈਟਾਂ_From_String_And_ਮਾਪ   Py_String_From_String_And_ਮਾਪ

#ਜੇ defined(PY_ਸੰਸਕਰਣ_HEX) && PY_ਸੰਸਕਰਣ_HEX >= 0x02070000
# define PY_USE_CAPSULE
#ਜੇ_ਅੰਤ

#ਜੇ defined(PY_ਸੰਸਕਰਣ_HEX) && PY_ਸੰਸਕਰਣ_HEX >= 0x02050000
# define PyInt Py_ssize_t
# define PyInquiry lenfunc
# define PyIntArgFunc ssizeargfunc
# define PyIntIntArgFunc ssizessizeargfunc
# define PyIntObjArgProc ssizeobjargproc
# define PyIntIntObjArgProc ssizessizeobjargproc
# define Py_ssize_t_fmt "n"
#ਨਹੀਂਤਾਂ
# define PyInt ਪੂਰਨ
# define lenfunc inquiry
# define PyInquiry inquiry
# define PyIntArgFunc intargfunc
# define PyIntIntArgFunc intintargfunc
# define PyIntObjArgProc intobjargproc
# define PyIntIntObjArgProc intintobjargproc
# define Py_ssize_t_fmt "i"
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Py_ਬਾਈਟਾਂ_fmt "s"

// Parser flags
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ single_ਨਿਵੇਸ਼	256
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ file_ਨਿਵੇਸ਼	257
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ eval_ਨਿਵੇਸ਼	258

#ਜੇ defined(PY_ਸੰਸਕਰਣ_HEX) && PY_ਸੰਸਕਰਣ_HEX >= 0x020300F0
  // Python 2.3: can invoke ":python" recursively.
# define PY_CAN_RECURSE
#ਜੇ_ਅੰਤ

#ਜੇ defined(DYNAMIC_PYTHON)
# ifndef DYNAMIC_PYTHON
#  define HINSTANCE long_u		// for generating prototypes
# endif

# ifdef MSWIN
#  define ਲੱਦੋ_dll vim_ਲੱਦੋ_Lib
#  define close_dll FreeLibrary
#  define ਚਿੰਨ੍ਹ_from_dll Get_Proc_ਪਤਾ
#  define ਲੱਦੋ_dll_error GetWin32Error
# ਨਹੀਂਤਾਂ
#  include <dlfcn.h>
#  define FARPROC ਖਾਲੀ*
#  define HINSTANCE ਖਾਲੀ*
#  ਜੇ defined(PY_NO_RTLD_GLOBAL) && defined(PY3_NO_RTLD_GLOBAL)
#   define ਲੱਦੋ_dll(n) dlopen((n), RTLD_LAZY)
#  ਨਹੀਂਤਾਂ
#   define ਲੱਦੋ_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#  endif
#  define close_dll dlclose
#  define ਚਿੰਨ੍ਹ_from_dll dlsym
#  define ਲੱਦੋ_dll_error dlerror
# endif

// This makes if_python.c compile without warnings against Python 2.5
// on Win32 and Win64.
# undef Py_ਲੜੀਵਾਰ_Simple_String
# undef Py_ਲੜੀਵਾਰ_String
# undef PyArg_Parse
# undef PyArg_ParseTuple
# undef Py_Build_ਮੁੱਲ
# undef Py_InitModule4
# undef Py_InitModule4_64
# undef Py_Object_Call_ਵਿਧੀ
# undef PyObject_CallFunction

/*
 * Wrapper defines
 */
# define PyArg_Parse dll_PyArg_Parse
# define PyArg_ParseTuple dll_PyArg_ParseTuple
# define PyMem_Free dll_PyMem_Free
# define PyMem_Malloc dll_PyMem_Malloc
# define Py_Dict_ਸੈੱਟ_ਵਸਤੂ_String dll_Py_Dict_ਸੈੱਟ_ਵਸਤੂ_String
# define Py_Err_Bad_ਦਲੀਲ dll_Py_Err_Bad_ਦਲੀਲ
# define PyErr_NewException dll_PyErr_NewException
# define PyErr_Clear dll_PyErr_Clear
# define PyErr_Format dll_PyErr_Format
# define PyErr_PrintEx dll_PyErr_PrintEx
# define Py_Err_No_ਸਮ੍ਰਿਤੀ dll_Py_Err_No_ਸਮ੍ਰਿਤੀ
# define PyErr_Occurred dll_PyErr_Occurred
# define Py_Err_ਸੈੱਟ_None dll_Py_Err_ਸੈੱਟ_None
# define Py_Err_ਸੈੱਟ_String dll_Py_Err_ਸੈੱਟ_String
# define Py_Err_ਸੈੱਟ_Object dll_Py_Err_ਸੈੱਟ_Object
# define PyErr_ExceptionMatches dll_PyErr_ExceptionMatches
# define Py_Eval_Init_ਧਾਗੇ dll_Py_Eval_Init_ਧਾਗੇ
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
# define Py_List_Get_ਵਸਤੂ dll_Py_List_Get_ਵਸਤੂ
# define PyList_Append dll_PyList_Append
# define Py_List_ਪਾਓ dll_Py_List_ਪਾਓ
# define PyList_New dll_PyList_New
# define Py_List_ਸੈੱਟ_ਵਸਤੂ_2 dll_Py_List_ਸੈੱਟ_ਵਸਤੂ
# define Py_List_ਮਾਪ dll_Py_List_ਮਾਪ
# define PyList_Type (*dll_PyList_Type)
# define Py_Tuple_Get_ਵਸਤੂ dll_Py_Tuple_Get_ਵਸਤੂ
# define Py_Tuple_ਸੈੱਟ_ਵਸਤੂ dll_Py_Tuple_ਸੈੱਟ_ਵਸਤੂ
# define PyTuple_New dll_PyTuple_New
# define Py_Tuple_ਮਾਪ dll_Py_Tuple_ਮਾਪ
# define PyTuple_Type (*dll_PyTuple_Type)
# define PySequence_Check dll_PySequence_Check
# define Py_Sequence_ਮਾਪ dll_Py_Sequence_ਮਾਪ
# define Py_Sequence_Get_ਵਸਤੂ dll_Py_Sequence_Get_ਵਸਤੂ
# define PySequence_Fast dll_PySequence_Fast
# define Py_ਟੁਕੜਾ_Get_Indices_Ex dll_Py_ਟੁਕੜਾ_Get_Indices_Ex
# define PyImport_ImportModule dll_PyImport_ImportModule
# define PyDict_New dll_PyDict_New
# define Py_Dict_Get_ਵਸਤੂ_String dll_Py_Dict_Get_ਵਸਤੂ_String
# define Py_Dict_ਅਗਲਾ dll_Py_Dict_ਅਗਲਾ
# define PyDict_Type (*dll_PyDict_Type)
# ifdef Py_Mapping_ਕੁੰਜੀਆਂ
#  define PY_NO_MAPPING_ਕੁੰਜੀਆਂ
# ਨਹੀਂਤਾਂ
#  define Py_Mapping_ਕੁੰਜੀਆਂ dll_Py_Mapping_ਕੁੰਜੀਆਂ
# endif
# define Py_Object_Get_ਵਸਤੂ dll_Py_Object_Get_ਵਸਤੂ
# define Py_Object_Call_ਵਿਧੀ dll_Py_Object_Call_ਵਿਧੀ
# define PyMapping_Check dll_PyMapping_Check
# define Py_Iter_ਅਗਲਾ dll_Py_Iter_ਅਗਲਾ
# define PyModule_GetDict dll_PyModule_GetDict
# define Py_Module_ਜੋੜੋ_Object dll_Py_Module_ਜੋੜੋ_Object
# define Py_ਲੜੀਵਾਰ_Simple_String dll_Py_ਲੜੀਵਾਰ_Simple_String
# define Py_ਲੜੀਵਾਰ_String dll_Py_ਲੜੀਵਾਰ_String
# define PyObject_GetAttrString dll_PyObject_GetAttrString
# define PyObject_HasAttrString dll_PyObject_HasAttrString
# define Py_Object_ਸੈੱਟ_Attr_String dll_Py_Object_ਸੈੱਟ_Attr_String
# define Py_Object_Call_Function_Obj_ਦਲੀਲਾਂ dll_Py_Object_Call_Function_Obj_ਦਲੀਲਾਂ
# define PyObject_CallFunction dll_PyObject_CallFunction
# define PyObject_Call dll_PyObject_Call
# define PyObject_Repr dll_PyObject_Repr
# define PyString_AsString dll_PyString_AsString
# define Py_String_As_String_And_ਮਾਪ dll_Py_String_As_String_And_ਮਾਪ
# define PyString_FromString dll_PyString_FromString
# define PyString_FromFormat dll_PyString_FromFormat
# define Py_String_From_String_And_ਮਾਪ dll_Py_String_From_String_And_ਮਾਪ
# define Py_String_ਮਾਪ dll_Py_String_ਮਾਪ
# define PyString_Type (*dll_PyString_Type)
# define PyUnicode_Type (*dll_PyUnicode_Type)
# undef PyUnicode_AsEncodedString
# define PyUnicode_AsEncodedString py_PyUnicode_AsEncodedString
# define PyFloat_AsDouble dll_PyFloat_AsDouble
# define PyFloat_FromDouble dll_PyFloat_FromDouble
# define PyFloat_Type (*dll_PyFloat_Type)
# define Py_ਸੰਖਿਆ_Check dll_Py_ਸੰਖਿਆ_Check
# define Py_ਸੰਖਿਆ_Long dll_Py_ਸੰਖਿਆ_Long
# define Py_Import_ਜੋੜੋ_Module (*dll_Py_Import_ਜੋੜੋ_Module)
# define Py_Sys_ਸੈੱਟ_Object dll_Py_Sys_ਸੈੱਟ_Object
# define PySys_GetObject dll_PySys_GetObject
# define Py_Sys_ਸੈੱਟ_Argv dll_Py_Sys_ਸੈੱਟ_Argv
# define PyType_Type (*dll_PyType_Type)
# define PyFile_Type (*dll_PyFile_Type)
# define Py_ਟੁਕੜਾ_Type (*dll_Py_ਟੁਕੜਾ_Type)
# define PyType_Ready (*dll_PyType_Ready)
# define PyType_GenericAlloc dll_PyType_GenericAlloc
# define Py_Build_ਮੁੱਲ dll_Py_Build_ਮੁੱਲ
# define Py_ਲੱਭੋ_ਵਿਧੀ dll_Py_ਲੱਭੋ_ਵਿਧੀ
# define Py_InitModule4 dll_Py_InitModule4
# define Py_ਸੈੱਟ_Python_Home dll_Py_ਸੈੱਟ_Python_Home
# define Py_Initialize dll_Py_Initialize
# define Py_Finalize dll_Py_Finalize
# define Py_IsInitialized dll_Py_IsInitialized
# define _PyObject_New dll__PyObject_New
# define _PyObject_GC_New dll__PyObject_GC_New
# ifdef PyObject_GC_Del
#  define Py_underscore_GC
#  define _PyObject_GC_Del dll__PyObject_GC_Del
#  define _PyObject_GC_UnTrack dll__PyObject_GC_UnTrack
# ਨਹੀਂਤਾਂ
#  define PyObject_GC_Del dll_PyObject_GC_Del
#  define PyObject_GC_UnTrack dll_PyObject_GC_UnTrack
# endif
# ਜੇ defined(PY_ਸੰਸਕਰਣ_HEX) && PY_ਸੰਸਕਰਣ_HEX >= 0x02070000
#  define Py_Object_ਅਗਲਾ_Not_Implemented (*dll_Py_Object_ਅਗਲਾ_Not_Implemented)
# endif
# define _Py_NoneStruct (*dll__Py_NoneStruct)
# define _Py_ZeroStruct (*dll__Py_ZeroStruct)
# define _Py_TrueStruct (*dll__Py_TrueStruct)
# define PyObject_Init dll__PyObject_Init
# define PyObject_GetIter dll_PyObject_GetIter
# define PyObject_IsTrue dll_PyObject_IsTrue
# ਜੇ defined(PY_ਸੰਸਕਰਣ_HEX) && PY_ਸੰਸਕਰਣ_HEX >= 0x02020000
#  define PyType_IsSubtype dll_PyType_IsSubtype
#  ifdef Py_DEBUG
#   define _Py_NegativeRefcount dll__Py_NegativeRefcount
#   define Py_Ref_ਕੁੱਲ (*dll_Py_Ref_ਕੁੱਲ)
#   define _Py_Dealloc dll__Py_Dealloc
#  endif
# endif
# ਜੇ defined(PY_ਸੰਸਕਰਣ_HEX) && PY_ਸੰਸਕਰਣ_HEX >= 0x02030000
#  ਜੇ defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
#   define _PyObject_DebugMalloc dll__PyObject_DebugMalloc
#   define _PyObject_DebugFree dll__PyObject_DebugFree
#  ਨਹੀਂਤਾਂ
#   define PyObject_Malloc dll_PyObject_Malloc
#   define PyObject_Free dll_PyObject_Free
#  endif
# endif
# ifdef PY_USE_CAPSULE
#  define PyCapsule_New dll_PyCapsule_New
#  define Py_Capsule_Get_ਸੂਚਕ dll_Py_Capsule_Get_ਸੂਚਕ
# ਨਹੀਂਤਾਂ
#  define PyCObject_FromVoidPtr dll_PyCObject_FromVoidPtr
#  define PyCObject_AsVoidPtr dll_PyCObject_AsVoidPtr
# endif
# ਜੇ defined(PY_ਸੰਸਕਰਣ_HEX) && PY_ਸੰਸਕਰਣ_HEX >= 0x02070000
#  define Py_NoSiteFlag (*dll_Py_NoSiteFlag)
# endif

/*
 * Pointers for dynamic link
 */
ਸਥਿਰ ਪੂਰਨ(*dll_PyArg_Parse)(PyObject *, ਅੱਖਰ *, ...);
ਸਥਿਰ ਪੂਰਨ(*dll_PyArg_ParseTuple)(PyObject *, ਅੱਖਰ *, ...);
ਸਥਿਰ ਖਾਲੀ(*dll_PyMem_Free)(ਖਾਲੀ *);
ਸਥਿਰ ਖਾਲੀ* (*dll_PyMem_Malloc)(size_t);
ਸਥਿਰ ਪੂਰਨ(*dll_Py_Dict_ਸੈੱਟ_ਵਸਤੂ_String)(PyObject *dp, ਅੱਖਰ *ਕੁੰਜੀ, PyObject *ਵਸਤੂ);
ਸਥਿਰ ਪੂਰਨ(*dll_Py_Err_Bad_ਦਲੀਲ)(ਖਾਲੀ);
ਸਥਿਰ PyObject *(*dll_PyErr_NewException)(ਅੱਖਰ *, PyObject *, PyObject *);
ਸਥਿਰ ਖਾਲੀ(*dll_PyErr_Clear)(ਖਾਲੀ);
ਸਥਿਰ PyObject*(*dll_PyErr_Format)(PyObject *, ਅਚਲ ਅੱਖਰ *, ...);
ਸਥਿਰ ਖਾਲੀ(*dll_PyErr_PrintEx)(ਪੂਰਨ);
ਸਥਿਰ PyObject*(*dll_Py_Err_No_ਸਮ੍ਰਿਤੀ)(ਖਾਲੀ);
ਸਥਿਰ PyObject*(*dll_PyErr_Occurred)(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ(*dll_Py_Err_ਸੈੱਟ_None)(PyObject *);
ਸਥਿਰ ਖਾਲੀ(*dll_Py_Err_ਸੈੱਟ_String)(PyObject *, ਅਚਲ ਅੱਖਰ *);
ਸਥਿਰ ਖਾਲੀ(*dll_Py_Err_ਸੈੱਟ_Object)(PyObject *, PyObject *);
ਸਥਿਰ ਪੂਰਨ(*dll_PyErr_ExceptionMatches)(PyObject *);
ਸਥਿਰ ਖਾਲੀ(*dll_Py_Eval_Init_ਧਾਗੇ)(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ(*dll_PyEval_RestoreThread)(PyThreadState *);
ਸਥਿਰ PyThreadState*(*dll_PyEval_SaveThread)(ਖਾਲੀ);
# ifdef PY_CAN_RECURSE
ਸਥਿਰ PyGILState_STATE	(*dll_PyGILState_Ensure)(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ (*dll_PyGILState_Release)(PyGILState_STATE);
# endif
ਸਥਿਰ ਲੰਮਾ(*dll_PyInt_AsLong)(PyObject *);
ਸਥਿਰ PyObject*(*dll_PyInt_FromLong)(ਲੰਮਾ);
ਸਥਿਰ ਲੰਮਾ(*dll_PyLong_AsLong)(PyObject *);
ਸਥਿਰ PyObject*(*dll_PyLong_FromLong)(ਲੰਮਾ);
ਸਥਿਰ PyTypeObject* dll_PyBool_Type;
ਸਥਿਰ PyTypeObject* dll_PyInt_Type;
ਸਥਿਰ PyTypeObject* dll_PyLong_Type;
ਸਥਿਰ PyObject*(*dll_Py_List_Get_ਵਸਤੂ)(PyObject *, PyInt);
ਸਥਿਰ ਪੂਰਨ(*dll_PyList_Append)(PyObject *, PyObject *);
ਸਥਿਰ ਪੂਰਨ(*dll_Py_List_ਪਾਓ)(PyObject *, PyInt, PyObject *);
ਸਥਿਰ PyObject*(*dll_PyList_New)(PyInt ਮਾਪ);
ਸਥਿਰ ਪੂਰਨ(*dll_Py_List_ਸੈੱਟ_ਵਸਤੂ)(PyObject *, PyInt, PyObject *);
ਸਥਿਰ PyInt(*dll_Py_List_ਮਾਪ)(PyObject *);
ਸਥਿਰ PyTypeObject* dll_PyList_Type;
ਸਥਿਰ PyObject*(*dll_Py_Tuple_Get_ਵਸਤੂ)(PyObject *, PyInt);
ਸਥਿਰ ਪੂਰਨ(*dll_Py_Tuple_ਸੈੱਟ_ਵਸਤੂ)(PyObject *, PyInt, PyObject *);
ਸਥਿਰ PyObject*(*dll_PyTuple_New)(PyInt ਮਾਪ);
ਸਥਿਰ PyInt(*dll_Py_Tuple_ਮਾਪ)(PyObject *);
ਸਥਿਰ PyTypeObject* dll_PyTuple_Type;
ਸਥਿਰ ਪੂਰਨ (*dll_PySequence_Check)(PyObject *);
ਸਥਿਰ PyInt(*dll_Py_Sequence_ਮਾਪ)(PyObject *);
ਸਥਿਰ PyObject*(*dll_Py_Sequence_Get_ਵਸਤੂ)(PyObject *, PyInt);
ਸਥਿਰ PyObject*(*dll_PySequence_Fast)(PyObject *, ਅਚਲ ਅੱਖਰ *);
ਸਥਿਰ ਪੂਰਨ (*dll_Py_ਟੁਕੜਾ_Get_Indices_Ex)(Py_ਟੁਕੜਾ_Object *r, PyInt ਲੰਮਾਈ,
		     PyInt *ਸ਼ੁਰੂ, PyInt *stop, PyInt *ਕਦਮ,
		     PyInt *slicelen);
ਸਥਿਰ PyObject*(*dll_PyImport_ImportModule)(ਅਚਲ ਅੱਖਰ *);
ਸਥਿਰ PyObject*(*dll_PyDict_New)(ਖਾਲੀ);
ਸਥਿਰ PyObject*(*dll_Py_Dict_Get_ਵਸਤੂ_String)(PyObject *, ਅਚਲ ਅੱਖਰ *);
ਸਥਿਰ ਪੂਰਨ (*dll_Py_Dict_ਅਗਲਾ)(PyObject *, PyInt *, PyObject **, PyObject **);
ਸਥਿਰ PyTypeObject* dll_PyDict_Type;
# ifndef PY_NO_MAPPING_ਕੁੰਜੀਆਂ
ਸਥਿਰ PyObject* (*dll_Py_Mapping_ਕੁੰਜੀਆਂ)(PyObject *);
# endif
ਸਥਿਰ PyObject* (*dll_Py_Object_Get_ਵਸਤੂ)(PyObject *, PyObject *);
ਸਥਿਰ PyObject* (*dll_Py_Object_Call_ਵਿਧੀ)(PyObject *, ਅੱਖਰ *, PyObject *);
ਸਥਿਰ ਪੂਰਨ (*dll_PyMapping_Check)(PyObject *);
ਸਥਿਰ PyObject* (*dll_Py_Iter_ਅਗਲਾ)(PyObject *);
ਸਥਿਰ PyObject*(*dll_PyModule_GetDict)(PyObject *);
ਸਥਿਰ ਪੂਰਨ(*dll_Py_Module_ਜੋੜੋ_Object)(PyObject *, ਅਚਲ ਅੱਖਰ *, PyObject *);
ਸਥਿਰ ਪੂਰਨ(*dll_Py_ਲੜੀਵਾਰ_Simple_String)(ਅੱਖਰ *);
ਸਥਿਰ PyObject *(*dll_Py_ਲੜੀਵਾਰ_String)(ਅੱਖਰ *, ਪੂਰਨ, PyObject *, PyObject *);
ਸਥਿਰ PyObject* (*dll_PyObject_GetAttrString)(PyObject *, ਅਚਲ ਅੱਖਰ *);
ਸਥਿਰ ਪੂਰਨ (*dll_PyObject_HasAttrString)(PyObject *, ਅਚਲ ਅੱਖਰ *);
ਸਥਿਰ ਪੂਰਨ (*dll_Py_Object_ਸੈੱਟ_Attr_String)(PyObject *, ਅਚਲ ਅੱਖਰ *, PyObject *);
ਸਥਿਰ PyObject* (*dll_Py_Object_Call_Function_Obj_ਦਲੀਲਾਂ)(PyObject *, ...);
ਸਥਿਰ PyObject* (*dll_PyObject_CallFunction)(PyObject *, ਅੱਖਰ *, ...);
ਸਥਿਰ PyObject* (*dll_PyObject_Call)(PyObject *, PyObject *, PyObject *);
ਸਥਿਰ PyObject* (*dll_PyObject_Repr)(PyObject *);
ਸਥਿਰ ਅੱਖਰ*(*dll_PyString_AsString)(PyObject *);
ਸਥਿਰ ਪੂਰਨ(*dll_Py_String_As_String_And_ਮਾਪ)(PyObject *, ਅੱਖਰ **, PyInt *);
ਸਥਿਰ PyObject*(*dll_PyString_FromString)(ਅਚਲ ਅੱਖਰ *);
ਸਥਿਰ PyObject*(*dll_PyString_FromFormat)(ਅਚਲ ਅੱਖਰ *, ...);
ਸਥਿਰ PyObject*(*dll_Py_String_From_String_And_ਮਾਪ)(ਅਚਲ ਅੱਖਰ *, PyInt);
ਸਥਿਰ PyInt(*dll_Py_String_ਮਾਪ)(PyObject *);
ਸਥਿਰ PyTypeObject* dll_PyString_Type;
ਸਥਿਰ PyTypeObject* dll_PyUnicode_Type;
ਸਥਿਰ PyObject *(*py_PyUnicode_AsEncodedString)(PyObject *, ਅੱਖਰ *, ਅੱਖਰ *);
ਸਥਿਰ ਦੁੱਗਣਾ(*dll_PyFloat_AsDouble)(PyObject *);
ਸਥਿਰ PyObject*(*dll_PyFloat_FromDouble)(ਦੁੱਗਣਾ);
ਸਥਿਰ PyTypeObject* dll_PyFloat_Type;
ਸਥਿਰ ਪੂਰਨ(*dll_Py_ਸੰਖਿਆ_Check)(PyObject *);
ਸਥਿਰ PyObject*(*dll_Py_ਸੰਖਿਆ_Long)(PyObject *);
ਸਥਿਰ ਪੂਰਨ(*dll_Py_Sys_ਸੈੱਟ_Object)(ਅੱਖਰ *, PyObject *);
ਸਥਿਰ PyObject *(*dll_PySys_GetObject)(ਅੱਖਰ *);
ਸਥਿਰ ਪੂਰਨ(*dll_Py_Sys_ਸੈੱਟ_Argv)(ਪੂਰਨ, ਅੱਖਰ **);
ਸਥਿਰ PyTypeObject* dll_PyType_Type;
ਸਥਿਰ PyTypeObject* dll_PyFile_Type;
ਸਥਿਰ PyTypeObject* dll_Py_ਟੁਕੜਾ_Type;
ਸਥਿਰ ਪੂਰਨ (*dll_PyType_Ready)(PyTypeObject *type);
ਸਥਿਰ PyObject* (*dll_PyType_GenericAlloc)(PyTypeObject *type, PyInt nitems);
ਸਥਿਰ PyObject*(*dll_Py_Build_ਮੁੱਲ)(ਅੱਖਰ *, ...);
ਸਥਿਰ PyObject*(*dll_Py_ਲੱਭੋ_ਵਿਧੀ)(ਬਣਤਰ Py_ਵਿਧੀ_Def[], PyObject *, ਅੱਖਰ *);
ਸਥਿਰ PyObject*(*dll_Py_InitModule4)(ਅੱਖਰ *, ਬਣਤਰ Py_ਵਿਧੀ_Def *, ਅੱਖਰ *, PyObject *, ਪੂਰਨ);
ਸਥਿਰ PyObject*(*dll_Py_Import_ਜੋੜੋ_Module)(ਅੱਖਰ *);
ਸਥਿਰ ਖਾਲੀ(*dll_Py_ਸੈੱਟ_Python_Home)(ਅੱਖਰ *home);
ਸਥਿਰ ਖਾਲੀ(*dll_Py_Initialize)(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ(*dll_Py_Finalize)(ਖਾਲੀ);
ਸਥਿਰ ਪੂਰਨ(*dll_Py_IsInitialized)(ਖਾਲੀ);
ਸਥਿਰ PyObject*(*dll__PyObject_New)(PyTypeObject *, PyObject *);
ਸਥਿਰ PyObject*(*dll__PyObject_GC_New)(PyTypeObject *);
# ifdef Py_underscore_GC
ਸਥਿਰ ਖਾਲੀ(*dll__PyObject_GC_Del)(ਖਾਲੀ *);
ਸਥਿਰ ਖਾਲੀ(*dll__PyObject_GC_UnTrack)(ਖਾਲੀ *);
# ਨਹੀਂਤਾਂ
ਸਥਿਰ ਖਾਲੀ(*dll_PyObject_GC_Del)(ਖਾਲੀ *);
ਸਥਿਰ ਖਾਲੀ(*dll_PyObject_GC_UnTrack)(ਖਾਲੀ *);
# endif
ਸਥਿਰ PyObject*(*dll__PyObject_Init)(PyObject *, PyTypeObject *);
ਸਥਿਰ PyObject* (*dll_PyObject_GetIter)(PyObject *);
ਸਥਿਰ ਪੂਰਨ (*dll_PyObject_IsTrue)(PyObject *);
# ਜੇ defined(PY_ਸੰਸਕਰਣ_HEX) && PY_ਸੰਸਕਰਣ_HEX >= 0x02070000
ਸਥਿਰ iternextfunc dll_Py_Object_ਅਗਲਾ_Not_Implemented;
# endif
ਸਥਿਰ PyObject* dll__Py_NoneStruct;
ਸਥਿਰ PyObject* _Py_ZeroStruct;
ਸਥਿਰ PyObject* dll__Py_TrueStruct;
# ਜੇ defined(PY_ਸੰਸਕਰਣ_HEX) && PY_ਸੰਸਕਰਣ_HEX >= 0x02020000
ਸਥਿਰ ਪੂਰਨ (*dll_PyType_IsSubtype)(PyTypeObject *, PyTypeObject *);
#  ifdef Py_DEBUG
ਸਥਿਰ ਖਾਲੀ (*dll__Py_NegativeRefcount)(ਅਚਲ ਅੱਖਰ *fname, ਪੂਰਨ lineno, PyObject *op);
ਸਥਿਰ PyInt* dll_Py_Ref_ਕੁੱਲ;
ਸਥਿਰ ਖਾਲੀ (*dll__Py_Dealloc)(PyObject *obj);
#  endif
# endif
# ਜੇ defined(PY_ਸੰਸਕਰਣ_HEX) && PY_ਸੰਸਕਰਣ_HEX >= 0x02030000
#  ਜੇ defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
ਸਥਿਰ ਖਾਲੀ (*dll__PyObject_DebugFree)(ਖਾਲੀ*);
ਸਥਿਰ ਖਾਲੀ* (*dll__PyObject_DebugMalloc)(size_t);
#  ਨਹੀਂਤਾਂ
ਸਥਿਰ ਖਾਲੀ* (*dll_PyObject_Malloc)(size_t);
ਸਥਿਰ ਖਾਲੀ (*dll_PyObject_Free)(ਖਾਲੀ*);
#  endif
# endif
# ifdef PY_USE_CAPSULE
ਸਥਿਰ PyObject* (*dll_PyCapsule_New)(ਖਾਲੀ *, ਅੱਖਰ *, PyCapsule_Destructor);
ਸਥਿਰ ਖਾਲੀ* (*dll_Py_Capsule_Get_ਸੂਚਕ)(PyObject *, ਅੱਖਰ *);
# ਨਹੀਂਤਾਂ
ਸਥਿਰ PyObject* (*dll_PyCObject_FromVoidPtr)(ਖਾਲੀ *cobj, ਖਾਲੀ (*destr)(ਖਾਲੀ *));
ਸਥਿਰ ਖਾਲੀ* (*dll_PyCObject_AsVoidPtr)(PyObject *);
# endif
# ਜੇ defined(PY_ਸੰਸਕਰਣ_HEX) && PY_ਸੰਸਕਰਣ_HEX >= 0x02070000
ਸਥਿਰ ਪੂਰਨ* dll_Py_NoSiteFlag;
# endif

ਸਥਿਰ HINSTANCE hinstPython = 0; // Instance of python.dll

// Imported exception objects
ਸਥਿਰ PyObject *imp_PyExc_AttributeError;
ਸਥਿਰ PyObject *imp_Py_Exc_ਸੂਚਕਾਂਕ_Error;
ਸਥਿਰ PyObject *imp_Py_Exc_ਕੁੰਜੀ_Error;
ਸਥਿਰ PyObject *imp_Py_Exc_Keyboard_ਰੁਕਾਵਟ;
ਸਥਿਰ PyObject *imp_PyExc_TypeError;
ਸਥਿਰ PyObject *imp_Py_Exc_ਮੁੱਲ_Error;
ਸਥਿਰ PyObject *imp_PyExc_SystemExit;
ਸਥਿਰ PyObject *imp_PyExc_RuntimeError;
ਸਥਿਰ PyObject *imp_PyExc_ImportError;
ਸਥਿਰ PyObject *imp_Py_Exc_ਉਛਾਲ_Error;

# define PyExc_AttributeError imp_PyExc_AttributeError
# define Py_Exc_ਸੂਚਕਾਂਕ_Error imp_Py_Exc_ਸੂਚਕਾਂਕ_Error
# define Py_Exc_ਕੁੰਜੀ_Error imp_Py_Exc_ਕੁੰਜੀ_Error
# define Py_Exc_Keyboard_ਰੁਕਾਵਟ imp_Py_Exc_Keyboard_ਰੁਕਾਵਟ
# define PyExc_TypeError imp_PyExc_TypeError
# define Py_Exc_ਮੁੱਲ_Error imp_Py_Exc_ਮੁੱਲ_Error
# define PyExc_SystemExit imp_PyExc_SystemExit
# define PyExc_RuntimeError imp_PyExc_RuntimeError
# define PyExc_ImportError imp_PyExc_ImportError
# define Py_Exc_ਉਛਾਲ_Error imp_Py_Exc_ਉਛਾਲ_Error

/*
 * Table of name to function pointer of python.
 */
# define PYTHON_PROC FARPROC
ਸਥਿਰ ਬਣਤਰ
{
    ਅੱਖਰ *name;
    PYTHON_PROC *ptr;
} python_funcname_ਸਾਰਣੀ[] =
{
# ifdef PY_SSIZE_T_CLEAN
    {"_PyArg_Parse_SizeT", (PYTHON_PROC*)&dll_PyArg_Parse},
    {"_PyArg_ParseTuple_SizeT", (PYTHON_PROC*)&dll_PyArg_ParseTuple},
    {"_Py_BuildValue_SizeT", (PYTHON_PROC*)&dll_Py_Build_ਮੁੱਲ},
# ਨਹੀਂਤਾਂ
    {"PyArg_Parse", (PYTHON_PROC*)&dll_PyArg_Parse},
    {"PyArg_ParseTuple", (PYTHON_PROC*)&dll_PyArg_ParseTuple},
    {"Py_BuildValue", (PYTHON_PROC*)&dll_Py_Build_ਮੁੱਲ},
# endif
    {"PyMem_Free", (PYTHON_PROC*)&dll_PyMem_Free},
    {"PyMem_Malloc", (PYTHON_PROC*)&dll_PyMem_Malloc},
    {"PyDict_SetItemString", (PYTHON_PROC*)&dll_Py_Dict_ਸੈੱਟ_ਵਸਤੂ_String},
    {"PyErr_BadArgument", (PYTHON_PROC*)&dll_Py_Err_Bad_ਦਲੀਲ},
    {"PyErr_NewException", (PYTHON_PROC*)&dll_PyErr_NewException},
    {"PyErr_Clear", (PYTHON_PROC*)&dll_PyErr_Clear},
    {"PyErr_Format", (PYTHON_PROC*)&dll_PyErr_Format},
    {"PyErr_PrintEx", (PYTHON_PROC*)&dll_PyErr_PrintEx},
    {"PyErr_NoMemory", (PYTHON_PROC*)&dll_Py_Err_No_ਸਮ੍ਰਿਤੀ},
    {"PyErr_Occurred", (PYTHON_PROC*)&dll_PyErr_Occurred},
    {"PyErr_SetNone", (PYTHON_PROC*)&dll_Py_Err_ਸੈੱਟ_None},
    {"PyErr_SetString", (PYTHON_PROC*)&dll_Py_Err_ਸੈੱਟ_String},
    {"PyErr_SetObject", (PYTHON_PROC*)&dll_Py_Err_ਸੈੱਟ_Object},
    {"PyErr_ExceptionMatches", (PYTHON_PROC*)&dll_PyErr_ExceptionMatches},
    {"PyEval_InitThreads", (PYTHON_PROC*)&dll_Py_Eval_Init_ਧਾਗੇ},
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
    {"PyList_GetItem", (PYTHON_PROC*)&dll_Py_List_Get_ਵਸਤੂ},
    {"PyList_Append", (PYTHON_PROC*)&dll_PyList_Append},
    {"PyList_Insert", (PYTHON_PROC*)&dll_Py_List_ਪਾਓ},
    {"PyList_New", (PYTHON_PROC*)&dll_PyList_New},
    {"PyList_SetItem", (PYTHON_PROC*)&dll_Py_List_ਸੈੱਟ_ਵਸਤੂ},
    {"PyList_Size", (PYTHON_PROC*)&dll_Py_List_ਮਾਪ},
    {"PyList_Type", (PYTHON_PROC*)&dll_PyList_Type},
    {"PyTuple_GetItem", (PYTHON_PROC*)&dll_Py_Tuple_Get_ਵਸਤੂ},
    {"PyTuple_SetItem", (PYTHON_PROC*)&dll_Py_Tuple_ਸੈੱਟ_ਵਸਤੂ},
    {"PyTuple_New", (PYTHON_PROC*)&dll_PyTuple_New},
    {"PyTuple_Size", (PYTHON_PROC*)&dll_Py_Tuple_ਮਾਪ},
    {"PyTuple_Type", (PYTHON_PROC*)&dll_PyTuple_Type},
    {"PySequence_Size", (PYTHON_PROC*)&dll_Py_Sequence_ਮਾਪ},
    {"PySequence_Check", (PYTHON_PROC*)&dll_PySequence_Check},
    {"PySequence_GetItem", (PYTHON_PROC*)&dll_Py_Sequence_Get_ਵਸਤੂ},
    {"PySequence_Fast", (PYTHON_PROC*)&dll_PySequence_Fast},
    {"PySlice_GetIndicesEx", (PYTHON_PROC*)&dll_Py_ਟੁਕੜਾ_Get_Indices_Ex},
    {"PyImport_ImportModule", (PYTHON_PROC*)&dll_PyImport_ImportModule},
    {"PyDict_GetItemString", (PYTHON_PROC*)&dll_Py_Dict_Get_ਵਸਤੂ_String},
    {"PyDict_Next", (PYTHON_PROC*)&dll_Py_Dict_ਅਗਲਾ},
    {"PyDict_New", (PYTHON_PROC*)&dll_PyDict_New},
    {"PyDict_Type", (PYTHON_PROC*)&dll_PyDict_Type},
# ifndef PY_NO_MAPPING_ਕੁੰਜੀਆਂ
    {"PyMapping_Keys", (PYTHON_PROC*)&dll_Py_Mapping_ਕੁੰਜੀਆਂ},
# endif
    {"PyObject_GetItem", (PYTHON_PROC*)&dll_Py_Object_Get_ਵਸਤੂ},
    {"PyObject_CallMethod", (PYTHON_PROC*)&dll_Py_Object_Call_ਵਿਧੀ},
    {"PyMapping_Check", (PYTHON_PROC*)&dll_PyMapping_Check},
    {"PyIter_Next", (PYTHON_PROC*)&dll_Py_Iter_ਅਗਲਾ},
    {"PyModule_GetDict", (PYTHON_PROC*)&dll_PyModule_GetDict},
    {"PyModule_AddObject", (PYTHON_PROC*)&dll_Py_Module_ਜੋੜੋ_Object},
    {"PyRun_SimpleString", (PYTHON_PROC*)&dll_Py_ਲੜੀਵਾਰ_Simple_String},
    {"PyRun_String", (PYTHON_PROC*)&dll_Py_ਲੜੀਵਾਰ_String},
    {"PyObject_GetAttrString", (PYTHON_PROC*)&dll_PyObject_GetAttrString},
    {"PyObject_HasAttrString", (PYTHON_PROC*)&dll_PyObject_HasAttrString},
    {"PyObject_SetAttrString", (PYTHON_PROC*)&dll_Py_Object_ਸੈੱਟ_Attr_String},
    {"PyObject_CallFunctionObjArgs", (PYTHON_PROC*)&dll_Py_Object_Call_Function_Obj_ਦਲੀਲਾਂ},
    {"PyObject_CallFunction", (PYTHON_PROC*)&dll_PyObject_CallFunction},
    {"PyObject_Call", (PYTHON_PROC*)&dll_PyObject_Call},
    {"PyObject_Repr", (PYTHON_PROC*)&dll_PyObject_Repr},
    {"PyString_AsString", (PYTHON_PROC*)&dll_PyString_AsString},
    {"PyString_AsStringAndSize", (PYTHON_PROC*)&dll_Py_String_As_String_And_ਮਾਪ},
    {"PyString_FromString", (PYTHON_PROC*)&dll_PyString_FromString},
    {"PyString_FromFormat", (PYTHON_PROC*)&dll_PyString_FromFormat},
    {"PyString_FromStringAndSize", (PYTHON_PROC*)&dll_Py_String_From_String_And_ਮਾਪ},
    {"PyString_Size", (PYTHON_PROC*)&dll_Py_String_ਮਾਪ},
    {"PyString_Type", (PYTHON_PROC*)&dll_PyString_Type},
    {"PyUnicode_Type", (PYTHON_PROC*)&dll_PyUnicode_Type},
    {"PyFloat_Type", (PYTHON_PROC*)&dll_PyFloat_Type},
    {"PyFloat_AsDouble", (PYTHON_PROC*)&dll_PyFloat_AsDouble},
    {"PyFloat_FromDouble", (PYTHON_PROC*)&dll_PyFloat_FromDouble},
    {"PyImport_AddModule", (PYTHON_PROC*)&dll_Py_Import_ਜੋੜੋ_Module},
    {"PyNumber_Check", (PYTHON_PROC*)&dll_Py_ਸੰਖਿਆ_Check},
    {"PyNumber_Long", (PYTHON_PROC*)&dll_Py_ਸੰਖਿਆ_Long},
    {"PySys_SetObject", (PYTHON_PROC*)&dll_Py_Sys_ਸੈੱਟ_Object},
    {"PySys_GetObject", (PYTHON_PROC*)&dll_PySys_GetObject},
    {"PySys_SetArgv", (PYTHON_PROC*)&dll_Py_Sys_ਸੈੱਟ_Argv},
    {"PyType_Type", (PYTHON_PROC*)&dll_PyType_Type},
    {"PyFile_Type", (PYTHON_PROC*)&dll_PyFile_Type},
    {"PySlice_Type", (PYTHON_PROC*)&dll_Py_ਟੁਕੜਾ_Type},
    {"PyType_Ready", (PYTHON_PROC*)&dll_PyType_Ready},
    {"PyType_GenericAlloc", (PYTHON_PROC*)&dll_PyType_GenericAlloc},
    {"Py_FindMethod", (PYTHON_PROC*)&dll_Py_ਲੱਭੋ_ਵਿਧੀ},
    {"Py_SetPythonHome", (PYTHON_PROC*)&dll_Py_ਸੈੱਟ_Python_Home},
    {"Py_Initialize", (PYTHON_PROC*)&dll_Py_Initialize},
    {"Py_Finalize", (PYTHON_PROC*)&dll_Py_Finalize},
    {"Py_IsInitialized", (PYTHON_PROC*)&dll_Py_IsInitialized},
    {"_PyObject_New", (PYTHON_PROC*)&dll__PyObject_New},
    {"_PyObject_GC_New", (PYTHON_PROC*)&dll__PyObject_GC_New},
# ifdef Py_underscore_GC
    {"_PyObject_GC_Del", (PYTHON_PROC*)&dll__PyObject_GC_Del},
    {"_PyObject_GC_UnTrack", (PYTHON_PROC*)&dll__PyObject_GC_UnTrack},
# ਨਹੀਂਤਾਂ
    {"PyObject_GC_Del", (PYTHON_PROC*)&dll_PyObject_GC_Del},
    {"PyObject_GC_UnTrack", (PYTHON_PROC*)&dll_PyObject_GC_UnTrack},
# endif
    {"PyObject_Init", (PYTHON_PROC*)&dll__PyObject_Init},
    {"PyObject_GetIter", (PYTHON_PROC*)&dll_PyObject_GetIter},
    {"PyObject_IsTrue", (PYTHON_PROC*)&dll_PyObject_IsTrue},
# ਜੇ defined(PY_ਸੰਸਕਰਣ_HEX) && PY_ਸੰਸਕਰਣ_HEX >= 0x02070000
    {"_PyObject_NextNotImplemented", (PYTHON_PROC*)&dll_Py_Object_ਅਗਲਾ_Not_Implemented},
# endif
    {"_Py_NoneStruct", (PYTHON_PROC*)&dll__Py_NoneStruct},
    {"_Py_ZeroStruct", (PYTHON_PROC*)&dll__Py_ZeroStruct},
    {"_Py_TrueStruct", (PYTHON_PROC*)&dll__Py_TrueStruct},
# ਜੇ defined(PY_ਸੰਸਕਰਣ_HEX) && PY_ਸੰਸਕਰਣ_HEX >= 0x02020000
#  ifdef Py_DEBUG
    {"_Py_NegativeRefcount", (PYTHON_PROC*)&dll__Py_NegativeRefcount},
    {"_Py_RefTotal", (PYTHON_PROC*)&dll_Py_Ref_ਕੁੱਲ},
    {"_Py_Dealloc", (PYTHON_PROC*)&dll__Py_Dealloc},
#  endif
    {"PyType_IsSubtype", (PYTHON_PROC*)&dll_PyType_IsSubtype},
# endif
# ਜੇ defined(PY_ਸੰਸਕਰਣ_HEX) && PY_ਸੰਸਕਰਣ_HEX >= 0x02030000
#  ਜੇ defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
    {"_PyObject_DebugFree", (PYTHON_PROC*)&dll__PyObject_DebugFree},
    {"_PyObject_DebugMalloc", (PYTHON_PROC*)&dll__PyObject_DebugMalloc},
#  ਨਹੀਂਤਾਂ
    {"PyObject_Malloc", (PYTHON_PROC*)&dll_PyObject_Malloc},
    {"PyObject_Free", (PYTHON_PROC*)&dll_PyObject_Free},
#  endif
# endif
# ਜੇ defined(PY_ਸੰਸਕਰਣ_HEX) && PY_ਸੰਸਕਰਣ_HEX >= 0x02050000 \
	&& SIZEOF_ਮਾਪ_T != VIM_SIZEOF_INT
#  ifdef Py_DEBUG
    {"Py_InitModule4TraceRefs_64", (PYTHON_PROC*)&dll_Py_InitModule4},
#  ਨਹੀਂਤਾਂ
    {"Py_InitModule4_64", (PYTHON_PROC*)&dll_Py_InitModule4},
#  endif
# ਨਹੀਂਤਾਂ
#  ifdef Py_DEBUG
    {"Py_InitModule4TraceRefs", (PYTHON_PROC*)&dll_Py_InitModule4},
#  ਨਹੀਂਤਾਂ
    {"Py_InitModule4", (PYTHON_PROC*)&dll_Py_InitModule4},
#  endif
# endif
# ifdef PY_USE_CAPSULE
    {"PyCapsule_New", (PYTHON_PROC*)&dll_PyCapsule_New},
    {"PyCapsule_GetPointer", (PYTHON_PROC*)&dll_Py_Capsule_Get_ਸੂਚਕ},
# ਨਹੀਂਤਾਂ
    {"PyCObject_FromVoidPtr", (PYTHON_PROC*)&dll_PyCObject_FromVoidPtr},
    {"PyCObject_AsVoidPtr", (PYTHON_PROC*)&dll_PyCObject_AsVoidPtr},
# endif
# ਜੇ defined(PY_ਸੰਸਕਰਣ_HEX) && PY_ਸੰਸਕਰਣ_HEX >= 0x02070000
    {"Py_NoSiteFlag", (PYTHON_PROC*)&dll_Py_NoSiteFlag},
# endif
    {"", ਨਿਰਾ},
};

/*
 * Load library and get all pointers.
 * Parameter 'libname' provides name of DLL.
 * Return OK or FAIL.
 */
    ਸਥਿਰ ਪੂਰਨ
python_runtime_link_init(ਅੱਖਰ *libname, ਪੂਰਨ verbose)
{
    ਪੂਰਨ i;
    PYTHON_PROC *ucs_as_encoded_string =
				   (PYTHON_PROC*)&py_PyUnicode_AsEncodedString;

# ਜੇ !(defined(PY_NO_RTLD_GLOBAL) && defined(PY3_NO_RTLD_GLOBAL)) && defined(UNIX) && defined(FEAT_PYTHON3)
    // Can't have Python and Python3 loaded at the same time.
    // It causes a crash, because RTLD_GLOBAL is needed for
    // standard C extension libraries of one or both python versions.
    ਜੇ (python3_loaded())
    {
	ਜੇ (verbose)
	    emsg(_(e_this_vim_cannot_execute_python_after_using_py3));
	ਵਾਪਸ FAIL;
    }
# endif

    ਜੇ (hinstPython)
	ਵਾਪਸ OK;
    hinstPython = ਲੱਦੋ_dll(libname);
    ਜੇ (!hinstPython)
    {
	ਜੇ (verbose)
	    semsg(_(e_could_not_ਲੱਦੋ_library_str_str), libname, ਲੱਦੋ_dll_error());
	ਵਾਪਸ FAIL;
    }

    ਲਈ (i = 0; python_funcname_ਸਾਰਣੀ[i].ptr; ++i)
    {
	ਜੇ ((*python_funcname_ਸਾਰਣੀ[i].ptr = ਚਿੰਨ੍ਹ_from_dll(hinstPython,
			python_funcname_ਸਾਰਣੀ[i].name)) == ਨਿਰਾ)
	{
	    close_dll(hinstPython);
	    hinstPython = 0;
	    ਜੇ (verbose)
		semsg(_(e_could_not_ਲੱਦੋ_library_function_str), python_funcname_ਸਾਰਣੀ[i].name);
	    ਵਾਪਸ FAIL;
	}
    }

    // Load unicode functions separately as only the ucs2 or the ucs4 functions
    // will be present in the library.
    *ucs_as_encoded_string = ਚਿੰਨ੍ਹ_from_dll(hinstPython,
					     "PyUnicodeUCS2_AsEncodedString");
    ਜੇ (*ucs_as_encoded_string == ਨਿਰਾ)
	*ucs_as_encoded_string = ਚਿੰਨ੍ਹ_from_dll(hinstPython,
					     "PyUnicodeUCS4_AsEncodedString");
    ਜੇ (*ucs_as_encoded_string == ਨਿਰਾ)
    {
	close_dll(hinstPython);
	hinstPython = 0;
	ਜੇ (verbose)
	    semsg(_(e_could_not_ਲੱਦੋ_library_function_str), "PyUnicode_UCSX_*");
	ਵਾਪਸ FAIL;
    }

    ਵਾਪਸ OK;
}

/*
 * If python is enabled (there is installed python on Windows system) return
 * TRUE, else FALSE.
 */
    ਪੂਰਨ
python_enabled(ਪੂਰਨ verbose)
{
    ਵਾਪਸ python_runtime_link_init((ਅੱਖਰ *)p_pydll, verbose) == OK;
}

/*
 * Load the standard Python exceptions - don't import the symbols from the
 * DLL, as this can cause errors (importing data symbols is not reliable).
 */
    ਸਥਿਰ ਖਾਲੀ
get_exceptions(ਖਾਲੀ)
{
    PyObject *exmod = PyImport_ImportModule("exceptions");
    PyObject *exdict = PyModule_GetDict(exmod);
    imp_PyExc_AttributeError = Py_Dict_Get_ਵਸਤੂ_String(exdict, "AttributeError");
    imp_Py_Exc_ਸੂਚਕਾਂਕ_Error = Py_Dict_Get_ਵਸਤੂ_String(exdict, "IndexError");
    imp_Py_Exc_ਕੁੰਜੀ_Error = Py_Dict_Get_ਵਸਤੂ_String(exdict, "KeyError");
    imp_Py_Exc_Keyboard_ਰੁਕਾਵਟ = Py_Dict_Get_ਵਸਤੂ_String(exdict, "KeyboardInterrupt");
    imp_PyExc_TypeError = Py_Dict_Get_ਵਸਤੂ_String(exdict, "TypeError");
    imp_Py_Exc_ਮੁੱਲ_Error = Py_Dict_Get_ਵਸਤੂ_String(exdict, "ValueError");
    imp_PyExc_SystemExit = Py_Dict_Get_ਵਸਤੂ_String(exdict, "SystemExit");
    imp_PyExc_RuntimeError = Py_Dict_Get_ਵਸਤੂ_String(exdict, "RuntimeError");
    imp_PyExc_ImportError = Py_Dict_Get_ਵਸਤੂ_String(exdict, "ImportError");
    imp_Py_Exc_ਉਛਾਲ_Error = Py_Dict_Get_ਵਸਤੂ_String(exdict, "OverflowError");
    Py_XINCREF(imp_PyExc_AttributeError);
    Py_XINCREF(imp_Py_Exc_ਸੂਚਕਾਂਕ_Error);
    Py_XINCREF(imp_Py_Exc_ਕੁੰਜੀ_Error);
    Py_XINCREF(imp_Py_Exc_Keyboard_ਰੁਕਾਵਟ);
    Py_XINCREF(imp_PyExc_TypeError);
    Py_XINCREF(imp_Py_Exc_ਮੁੱਲ_Error);
    Py_XINCREF(imp_PyExc_SystemExit);
    Py_XINCREF(imp_PyExc_RuntimeError);
    Py_XINCREF(imp_PyExc_ImportError);
    Py_XINCREF(imp_Py_Exc_ਉਛਾਲ_Error);
    Py_XDECREF(exmod);
}
#ਜੇ_ਅੰਤ // DYNAMIC_PYTHON

ਸਥਿਰ ਪੂਰਨ initialised = 0;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PYINITIALISED initialised
ਸਥਿਰ ਪੂਰਨ python_ਸਿਰਾ_called = FALSE;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DESTRUCTOR_ਅੰਤ(self) self->ob_type->tp_free((PyObject*)self);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WIN_PYTHON_REF(win) win->w_python_ref
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BUF_PYTHON_REF(buf) buf->b_python_ref
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TAB_PYTHON_REF(tab) tab->tp_python_ref

ਸਥਿਰ PyObject *OutputGetattr(PyObject *, ਅੱਖਰ *);
ਸਥਿਰ PyObject *ਬਫਰ_Getattr(PyObject *, ਅੱਖਰ *);
ਸਥਿਰ PyObject *ਖਿੜਕੀ_Getattr(PyObject *, ਅੱਖਰ *);
ਸਥਿਰ PyObject *Tab_ਪੰਨਾ_Getattr(PyObject *, ਅੱਖਰ *);
ਸਥਿਰ PyObject *RangeGetattr(PyObject *, ਅੱਖਰ *);
ਸਥਿਰ PyObject *DictionaryGetattr(PyObject *, ਅੱਖਰ*);
ਸਥਿਰ PyObject *ListGetattr(PyObject *, ਅੱਖਰ *);
ਸਥਿਰ PyObject *TupleGetattr(PyObject *, ਅੱਖਰ *);
ਸਥਿਰ PyObject *FunctionGetattr(PyObject *, ਅੱਖਰ *);

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ Py_ਦੇਖੋ
# define Py_ਦੇਖੋ(obj) ਦੇਖੋ(obj, arg)
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ Py_CLEAR
# define Py_CLEAR(obj) \
    { \
	Py_XDECREF(obj); \
	obj = ਨਿਰਾ; \
    }
#ਜੇ_ਅੰਤ

#ਜੇ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    ਸਥਿਰ ਖਾਲੀ *
py_memsave(ਖਾਲੀ *p, size_t len)
{
    ਖਾਲੀ	*r;

    ਜੇ (!(r = PyMem_Malloc(len)))
	ਵਾਪਸ ਨਿਰਾ;
    mch_memmove(r, p, len);
    ਵਾਪਸ r;
}

# define PY_STRSAVE(s) ((char_u *) py_memsave(s, STRLEN(s) + 1))
#ਜੇ_ਅੰਤ

ਕਿਸਮਪਰਿਭਾਸ਼ਾ Py_ਟੁਕੜਾ_Object Py_ਟੁਕੜਾ_Object_T;

/*
 * Include the code shared with if_python3.c
 */
#ਅਨ੍ਤਰ੍ਭਾਵ "if_py_both.h"


///////////////////////////////////////////////////////
// Internal function prototypes.

ਸਥਿਰ ਪੂਰਨ PythonMod_Init(ਖਾਲੀ);


///////////////////////////////////////////////////////
// 1. Python interpreter main program.

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ PY_CAN_RECURSE
ਸਥਿਰ PyThreadState *saved_python_thread = ਨਿਰਾ;

/*
 * Suspend a thread of the Python interpreter, other threads are allowed to
 * run.
 */
    ਸਥਿਰ ਖਾਲੀ
Python_SaveThread(ਖਾਲੀ)
{
    saved_python_thread = PyEval_SaveThread();
}

/*
 * Restore a thread of the Python interpreter, waits for other threads to
 * block.
 */
    ਸਥਿਰ ਖਾਲੀ
Python_RestoreThread(ਖਾਲੀ)
{
    PyEval_RestoreThread(saved_python_thread);
    saved_python_thread = ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

    ਖਾਲੀ
python_ਸਿਰਾ(ਖਾਲੀ)
{
    ਸਥਿਰ ਪੂਰਨ recurse = 0;

    // If a crash occurs while doing this, don't try again.
    ਜੇ (recurse != 0)
	ਵਾਪਸ;

    python_ਸਿਰਾ_called = TRUE;
    ++recurse;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DYNAMIC_PYTHON
    ਜੇ (hinstPython && Py_IsInitialized())
    {
# ifdef PY_CAN_RECURSE
	PyGILState_Ensure();
# ਨਹੀਂਤਾਂ
	Python_RestoreThread();	    // enter python
# endif
	Py_Finalize();
    }
#ਨਹੀਂਤਾਂ
    ਜੇ (Py_IsInitialized())
    {
# ifdef PY_CAN_RECURSE
	PyGILState_Ensure();
# ਨਹੀਂਤਾਂ
	Python_RestoreThread();	    // enter python
# endif
	Py_Finalize();
    }
#ਜੇ_ਅੰਤ

    --recurse;
}

#ਜੇ defined(DYNAMIC_PYTHON) && defined(FEAT_PYTHON3)
    ਪੂਰਨ
python_loaded(ਖਾਲੀ)
{
    ਵਾਪਸ (hinstPython != 0);
}
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਅੱਖਰ *py_home_buf = ਨਿਰਾ;

    ਸਥਿਰ ਪੂਰਨ
Python_Init(ਖਾਲੀ)
{
    ਜੇ (!initialised)
    {
#ਜੇ defined(PY_ਸੰਸਕਰਣ_HEX) && PY_ਸੰਸਕਰਣ_HEX >= 0x02070000
	PyObject *site;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DYNAMIC_PYTHON
	ਜੇ (!python_enabled(TRUE))
	{
	    emsg(_(e_sorry_this_command_is_disabled_python_library_could_not_be_ਮਿਲਿਆ));
	    ਜਾਓ fail;
	}
#ਜੇ_ਅੰਤ

	ਜੇ (*p_pyhome != NUL)
	{
	    // The string must not change later, make a copy in static memory.
	    py_home_buf = (ਅੱਖਰ *)vim_strsave(p_pyhome);
	    ਜੇ (py_home_buf != ਨਿਰਾ)
		Py_ਸੈੱਟ_Python_Home(py_home_buf);
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ PYTHON_HOME
	ਨਹੀਂਤਾਂ ਜੇ (mch_getenv((char_u *)"PYTHONHOME") == ਨਿਰਾ)
	    Py_ਸੈੱਟ_Python_Home(PYTHON_HOME);
#ਜੇ_ਅੰਤ

	init_structs();

#ਜੇ defined(PY_ਸੰਸਕਰਣ_HEX) && PY_ਸੰਸਕਰਣ_HEX >= 0x02070000
	// Disable implicit 'import site', because it may cause Vim to exit
	// when it can't be found.
	Py_NoSiteFlag++;
#ਜੇ_ਅੰਤ

	Py_Initialize();

#ਜੇ defined(PY_ਸੰਸਕਰਣ_HEX) && PY_ਸੰਸਕਰਣ_HEX >= 0x02070000
	// 'import site' explicitly.
	site = PyImport_ImportModule("site");
	ਜੇ (site == ਨਿਰਾ)
	{
	    emsg(_(e_sorry_this_command_is_disabled_python_site_module_could_not_be_loaded));
	    ਜਾਓ fail;
	}
	Py_DECREF(site);
#ਜੇ_ਅੰਤ

	// Initialise threads, and below save the state using
	// PyEval_SaveThread.  Without the call to PyEval_SaveThread, thread
	// specific state (such as the system trace hook), will be lost
	// between invocations of Python code.
	Py_Eval_Init_ਧਾਗੇ();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DYNAMIC_PYTHON
	get_exceptions();
#ਜੇ_ਅੰਤ

	ਜੇ (PythonIO_Init_io())
	    ਜਾਓ fail;

	ਜੇ (PythonMod_Init())
	    ਜਾਓ fail;

	globals = PyModule_GetDict(Py_Import_ਜੋੜੋ_Module("__main__"));

	// Remove the element from sys.path that was added because of our
	// argv[0] value in PythonMod_Init().  Previously we used an empty
	// string, but depending on the OS we then get an empty entry or
	// the current directory in sys.path.
	Py_ਲੜੀਵਾਰ_Simple_String("import sys; sys.path = filter(lambda x: x != '/must>not&exist', sys.path)");

	// lock is created and acquired in PyEval_InitThreads() and thread
	// state is created in Py_Initialize()
	// there _PyGILState_NoteThreadState() also sets gilcounter to 1
	// (python must have threads enabled!)
	// so the following does both: unlock GIL and save thread state in TLS
	// without deleting thread state
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ PY_CAN_RECURSE
	saved_python_thread =
#ਜੇ_ਅੰਤ
	    PyEval_SaveThread();

	initialised = 1;
    }

    ਵਾਪਸ 0;

fail:
    // We call PythonIO_Flush() here to print any Python errors.
    // This is OK, as it is possible to call this function even
    // if PythonIO_Init_io() has not completed successfully (it will
    // not do anything in this case).
    PythonIO_Flush();
    ਵਾਪਸ -1;
}

/*
 * External interface
 */
    ਸਥਿਰ ਖਾਲੀ
DoPyCommand(
    ਅਚਲ ਅੱਖਰ		*cmd,
    dict_T*		locals,
    rangeinitializer	init_range,
    runner		ਲੜੀਵਾਰ,
    ਖਾਲੀ		*arg)
{
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ PY_CAN_RECURSE
    ਸਥਿਰ ਪੂਰਨ		recursive = 0;
#ਜੇ_ਅੰਤ
#ਜੇ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    ਅੱਖਰ		*saved_locale;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ PY_CAN_RECURSE
    PyGILState_STATE	pygilstate;
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ PY_CAN_RECURSE
    ਜੇ (recursive)
    {
	emsg(_(e_cannot_invoke_python_recursively));
	ਵਾਪਸ;
    }
    ++recursive;
#ਜੇ_ਅੰਤ
    ਜੇ (python_ਸਿਰਾ_called)
	ਵਾਪਸ;

    ਜੇ (Python_Init())
	ਜਾਓ theend;

    init_range(arg);

    Python_Release_Vim();	    // leave Vim

#ਜੇ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    // Python only works properly when the LC_NUMERIC locale is "C".
    saved_locale = setlocale(LC_NUMERIC, ਨਿਰਾ);
    ਜੇ (saved_locale == ਨਿਰਾ || STRCMP(saved_locale, "C") == 0)
	saved_locale = ਨਿਰਾ;
    ਨਹੀਂਤਾਂ
    {
	// Need to make a copy, value may change when setting new locale.
	saved_locale = (ਅੱਖਰ *) PY_STRSAVE(saved_locale);
	(ਖਾਲੀ)setlocale(LC_NUMERIC, "C");
    }
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ PY_CAN_RECURSE
    pygilstate = PyGILState_Ensure();
#ਨਹੀਂਤਾਂ
    Python_RestoreThread();	    // enter python
#ਜੇ_ਅੰਤ

    ਲੜੀਵਾਰ((ਅੱਖਰ *) cmd, locals, arg
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ PY_CAN_RECURSE
	    , &pygilstate
#ਜੇ_ਅੰਤ
	    );

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ PY_CAN_RECURSE
    PyGILState_Release(pygilstate);
#ਨਹੀਂਤਾਂ
    Python_SaveThread();	    // leave python
#ਜੇ_ਅੰਤ

#ਜੇ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    ਜੇ (saved_locale != ਨਿਰਾ)
    {
	(ਖਾਲੀ)setlocale(LC_NUMERIC, saved_locale);
	PyMem_Free(saved_locale);
    }
#ਜੇ_ਅੰਤ

    Python_ਤਾਲਾ_Vim();		    // enter vim
    PythonIO_Flush();

theend:
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ PY_CAN_RECURSE
    --recursive;
#ਜੇ_ਅੰਤ
    ਵਾਪਸ;
}

/*
 * ":python"
 */
    ਖਾਲੀ
ex_python(exarg_T *eap)
{
    char_u *script;

    script = script_get(eap, eap->arg);
    ਜੇ (!eap->skip)
    {
	ਜੇ (p_pyx == 0)
	    p_pyx = 2;

	DoPyCommand(script == ਨਿਰਾ ? (ਅੱਖਰ *) eap->arg : (ਅੱਖਰ *) script,
		ਨਿਰਾ,
		init_range_cmd,
		(runner) ਲੜੀਵਾਰ_cmd,
		(ਖਾਲੀ *) eap);
    }
    vim_free(script);
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਬਫਰ_ਮਾਪ 1024

/*
 * ":pyfile"
 */
    ਖਾਲੀ
ex_pyfile(exarg_T *eap)
{
    ਸਥਿਰ ਅੱਖਰ ਬਫਰ[ਬਫਰ_ਮਾਪ];
    ਅਚਲ ਅੱਖਰ *file = (ਅੱਖਰ *)eap->arg;
    ਅੱਖਰ *p;

    ਜੇ (p_pyx == 0)
	p_pyx = 2;

    // Have to do it like this. PyRun_SimpleFile requires you to pass a
    // stdio file pointer, but Vim and the Python DLL are compiled with
    // different options under Windows, meaning that stdio pointers aren't
    // compatible between the two. Yuk.
    //
    // Put the string "execfile('file')" into buffer. But, we need to
    // escape any backslashes or single quotes in the file name, so that
    // Python won't mangle the file name.
    strcpy(ਬਫਰ, "execfile('");
    p = ਬਫਰ + 10; // size of "execfile('"

    ਜਦੋਂ (*file && p < ਬਫਰ + (ਬਫਰ_ਮਾਪ - 3))
    {
	ਜੇ (*file == '\\' || *file == '\'')
	    *p++ = '\\';
	*p++ = *file++;
    }

    // If we didn't finish the file name, we hit a buffer overflow
    ਜੇ (*file != '\0')
	ਵਾਪਸ;

    // Put in the terminating "')" and a null
    *p++ = '\'';
    *p++ = ')';
    *p++ = '\0';

    // Execute the file
    DoPyCommand(ਬਫਰ,
	    ਨਿਰਾ,
	    init_range_cmd,
	    (runner) ਲੜੀਵਾਰ_cmd,
	    (ਖਾਲੀ *) eap);
}

    ਖਾਲੀ
ex_pydo(exarg_T *eap)
{
    ਜੇ (p_pyx == 0)
	p_pyx = 2;

    DoPyCommand((ਅੱਖਰ *)eap->arg,
	    ਨਿਰਾ,
	    init_range_cmd,
	    (runner)ਲੜੀਵਾਰ_do,
	    (ਖਾਲੀ *)eap);
}

///////////////////////////////////////////////////////
// 2. Python output stream: writes output via [e]msg().

// Implementation functions

    ਸਥਿਰ PyObject *
OutputGetattr(PyObject *self, ਅੱਖਰ *name)
{
    ਜੇ (strcmp(name, "softspace") == 0)
	ਵਾਪਸ PyInt_FromLong(((OutputObject *)(self))->softspace);
    ਨਹੀਂਤਾਂ ਜੇ (strcmp(name, "__members__") == 0)
	ਵਾਪਸ ObjectDir(ਨਿਰਾ, OutputAttrs);
    ਨਹੀਂਤਾਂ ਜੇ (strcmp(name, "errors") == 0)
	ਵਾਪਸ PyString_FromString("strict");
    ਨਹੀਂਤਾਂ ਜੇ (strcmp(name, "encoding") == 0)
	ਵਾਪਸ PyString_FromString(ENC_OPT);
    ਵਾਪਸ Py_ਲੱਭੋ_ਵਿਧੀ(OutputMethods, self, name);
}

///////////////////////////////////////////////////////
// 3. Implementation of the Vim module for Python

// Window type - Implementation functions
// --------------------------------------

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਖਿੜਕੀ_Type_Check(obj) ((obj)->ob_type == &ਖਿੜਕੀ_Type)

// Buffer type - Implementation functions
// --------------------------------------

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਬਫਰ_Type_Check(obj) ((obj)->ob_type == &ਬਫਰ_Type)

ਸਥਿਰ ਪੂਰਨ ਬਫਰ_Ass_ਵਸਤੂ(PyObject *, PyInt, PyObject *);
ਸਥਿਰ ਪੂਰਨ ਬਫਰ_Ass_ਟੁਕੜਾ(PyObject *, PyInt, PyInt, PyObject *);

// Line range type - Implementation functions
// --------------------------------------

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RangeType_Check(obj) ((obj)->ob_type == &RangeType)

ਸਥਿਰ ਪੂਰਨ Range_Ass_ਵਸਤੂ(PyObject *, PyInt, PyObject *);
ਸਥਿਰ ਪੂਰਨ Range_Ass_ਟੁਕੜਾ(PyObject *, PyInt, PyInt, PyObject *);

// Current objects type - Implementation functions
// -----------------------------------------------

ਸਥਿਰ PySequenceMethods ਬਫਰ_As_Seq = {
    (PyInquiry)		ਬਫਰ_ਲੰਮਾਈ,	    // sq_length,    len(x)
    (binaryfunc)	0,		    // BufferConcat, sq_concat, x+y
    (PyIntArgFunc)	0,		    // BufferRepeat, sq_repeat, x*n
    (PyIntArgFunc)	ਬਫਰ_ਵਸਤੂ,	    // sq_item,      x[i]
    (PyIntIntArgFunc)	ਬਫਰ_ਟੁਕੜਾ,	    // sq_slice,     x[i:j]
    (PyIntObjArgProc)	ਬਫਰ_Ass_ਵਸਤੂ,	    // sq_ass_item,  x[i]=v
    (PyIntIntObjArgProc) ਬਫਰ_Ass_ਟੁਕੜਾ,    // sq_ass_slice, x[i:j]=v
    (objobjproc)	0,
    (binaryfunc)	0,
    0,
};

// Buffer object - Implementation

    ਸਥਿਰ PyObject *
ਬਫਰ_Getattr(PyObject *self, ਅੱਖਰ *name)
{
    PyObject *r;

    ਜੇ ((r = ਬਫਰ_Attr_Valid((ਬਫਰ_Object *)(self), name)))
	ਵਾਪਸ r;

    ਜੇ (Check_ਬਫਰ((ਬਫਰ_Object *)(self)))
	ਵਾਪਸ ਨਿਰਾ;

    r = ਬਫਰ_Attr((ਬਫਰ_Object *)(self), name);
    ਜੇ (r || PyErr_Occurred())
	ਵਾਪਸ r;
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ Py_ਲੱਭੋ_ਵਿਧੀ(ਬਫਰ_Methods, self, name);
}

//////////////////

    ਸਥਿਰ ਪੂਰਨ
ਬਫਰ_Ass_ਵਸਤੂ(PyObject *self, PyInt n, PyObject *val)
{
    ਵਾਪਸ RBAs_ਵਸਤੂ((ਬਫਰ_Object *)(self), n, val, 1, -1, ਨਿਰਾ);
}

    ਸਥਿਰ ਪੂਰਨ
ਬਫਰ_Ass_ਟੁਕੜਾ(PyObject *self, PyInt lo, PyInt hi, PyObject *val)
{
    ਵਾਪਸ RBAs_ਟੁਕੜਾ((ਬਫਰ_Object *)(self), lo, hi, val, 1, -1, ਨਿਰਾ);
}

ਸਥਿਰ PySequenceMethods RangeAsSeq = {
    (PyInquiry)		Range_ਲੰਮਾਈ,	      // sq_length,    len(x)
    (binaryfunc)	0, /* RangeConcat, */ // sq_concat,    x+y
    (PyIntArgFunc)	0, /* RangeRepeat, */ // sq_repeat,    x*n
    (PyIntArgFunc)	Range_ਵਸਤੂ,	      // sq_item,      x[i]
    (PyIntIntArgFunc)	Range_ਟੁਕੜਾ,	      // sq_slice,     x[i:j]
    (PyIntObjArgProc)	Range_Ass_ਵਸਤੂ,	      // sq_ass_item,  x[i]=v
    (PyIntIntObjArgProc) Range_Ass_ਟੁਕੜਾ,	      // sq_ass_slice, x[i:j]=v
    (objobjproc)	0,
#ਜੇ PY_MAJOR_ਸੰਸਕਰਣ >= 2
    (binaryfunc)	0,
    0,
#ਜੇ_ਅੰਤ
};

// Line range object - Implementation

    ਸਥਿਰ PyObject *
RangeGetattr(PyObject *self, ਅੱਖਰ *name)
{
    ਜੇ (strcmp(name, "start") == 0)
	ਵਾਪਸ Py_Build_ਮੁੱਲ(Py_ssize_t_fmt, ((RangeObject *)(self))->ਸ਼ੁਰੂ - 1);
    ਨਹੀਂਤਾਂ ਜੇ (strcmp(name, "end") == 0)
	ਵਾਪਸ Py_Build_ਮੁੱਲ(Py_ssize_t_fmt, ((RangeObject *)(self))->ਸਿਰਾ - 1);
    ਨਹੀਂਤਾਂ ਜੇ (strcmp(name, "__members__") == 0)
	ਵਾਪਸ ObjectDir(ਨਿਰਾ, RangeAttrs);
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ Py_ਲੱਭੋ_ਵਿਧੀ(RangeMethods, self, name);
}

////////////////

    ਸਥਿਰ ਪੂਰਨ
Range_Ass_ਵਸਤੂ(PyObject *self, PyInt n, PyObject *val)
{
    ਵਾਪਸ RBAs_ਵਸਤੂ(((RangeObject *)(self))->buf, n, val,
		     ((RangeObject *)(self))->ਸ਼ੁਰੂ,
		     ((RangeObject *)(self))->ਸਿਰਾ,
		     &((RangeObject *)(self))->ਸਿਰਾ);
}

    ਸਥਿਰ ਪੂਰਨ
Range_Ass_ਟੁਕੜਾ(PyObject *self, PyInt lo, PyInt hi, PyObject *val)
{
    ਵਾਪਸ RBAs_ਟੁਕੜਾ(((RangeObject *)(self))->buf, lo, hi, val,
		      ((RangeObject *)(self))->ਸ਼ੁਰੂ,
		      ((RangeObject *)(self))->ਸਿਰਾ,
		      &((RangeObject *)(self))->ਸਿਰਾ);
}

// TabPage object - Implementation

    ਸਥਿਰ PyObject *
Tab_ਪੰਨਾ_Getattr(PyObject *self, ਅੱਖਰ *name)
{
    PyObject *r;

    ਜੇ ((r = Tab_ਪੰਨਾ_Attr_Valid((Tab_ਪੰਨਾ_Object *)(self), name)))
	ਵਾਪਸ r;

    ਜੇ (Check_Tab_ਪੰਨਾ((Tab_ਪੰਨਾ_Object *)(self)))
	ਵਾਪਸ ਨਿਰਾ;

    r = Tab_ਪੰਨਾ_Attr((Tab_ਪੰਨਾ_Object *)(self), name);
    ਜੇ (r || PyErr_Occurred())
	ਵਾਪਸ r;
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ Py_ਲੱਭੋ_ਵਿਧੀ(Tab_ਪੰਨਾ_Methods, self, name);
}

// Window object - Implementation

    ਸਥਿਰ PyObject *
ਖਿੜਕੀ_Getattr(PyObject *self, ਅੱਖਰ *name)
{
    PyObject *r;

    ਜੇ ((r = ਖਿੜਕੀ_Attr_Valid((ਖਿੜਕੀ_Object *)(self), name)))
	ਵਾਪਸ r;

    ਜੇ (Check_ਖਿੜਕੀ((ਖਿੜਕੀ_Object *)(self)))
	ਵਾਪਸ ਨਿਰਾ;

    r = ਖਿੜਕੀ_Attr((ਖਿੜਕੀ_Object *)(self), name);
    ਜੇ (r || PyErr_Occurred())
	ਵਾਪਸ r;
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ Py_ਲੱਭੋ_ਵਿਧੀ(ਖਿੜਕੀ_Methods, self, name);
}

// Tab page list object - Definitions

ਸਥਿਰ PySequenceMethods TabListAsSeq = {
    (PyInquiry)		Tab_List_ਲੰਮਾਈ,	    // sq_length,    len(x)
    (binaryfunc)	0,		    // sq_concat,    x+y
    (PyIntArgFunc)	0,		    // sq_repeat,    x*n
    (PyIntArgFunc)	Tab_List_ਵਸਤੂ,	    // sq_item,      x[i]
    (PyIntIntArgFunc)	0,		    // sq_slice,     x[i:j]
    (PyIntObjArgProc)	0,		    // sq_ass_item,  x[i]=v
    (PyIntIntObjArgProc) 0,		    // sq_ass_slice, x[i:j]=v
    (objobjproc)	0,
#ਜੇ PY_MAJOR_ਸੰਸਕਰਣ >= 2
    (binaryfunc)	0,
    0,
#ਜੇ_ਅੰਤ
};

// Window list object - Definitions

ਸਥਿਰ PySequenceMethods WinListAsSeq = {
    (PyInquiry)		Win_List_ਲੰਮਾਈ,	    // sq_length,    len(x)
    (binaryfunc)	0,		    // sq_concat,    x+y
    (PyIntArgFunc)	0,		    // sq_repeat,    x*n
    (PyIntArgFunc)	Win_List_ਵਸਤੂ,	    // sq_item,      x[i]
    (PyIntIntArgFunc)	0,		    // sq_slice,     x[i:j]
    (PyIntObjArgProc)	0,		    // sq_ass_item,  x[i]=v
    (PyIntIntObjArgProc) 0,		    // sq_ass_slice, x[i:j]=v
    (objobjproc)	0,
#ਜੇ PY_MAJOR_ਸੰਸਕਰਣ >= 2
    (binaryfunc)	0,
    0,
#ਜੇ_ਅੰਤ
};

// External interface

    ਖਾਲੀ
python_ਬਫਰ_free(buf_T *buf)
{
    ਬਫਰ_Object *bp = BUF_PYTHON_REF(buf);
    ਜੇ (bp == ਨਿਰਾ)
	ਵਾਪਸ;
    bp->buf = INVALID_ਬਫਰ_ਮੁੱਲ;
    BUF_PYTHON_REF(buf) = ਨਿਰਾ;
}

    ਖਾਲੀ
python_ਖਿੜਕੀ_free(win_T *win)
{
    ਖਿੜਕੀ_Object *wp = WIN_PYTHON_REF(win);
    ਜੇ (wp == ਨਿਰਾ)
	ਵਾਪਸ;
    wp->win = INVALID_ਖਿੜਕੀ_ਮੁੱਲ;
    WIN_PYTHON_REF(win) = ਨਿਰਾ;
}

    ਖਾਲੀ
python_tabpage_free(tabpage_T *tab)
{
    Tab_ਪੰਨਾ_Object *tp = TAB_PYTHON_REF(tab);
    ਜੇ (tp == ਨਿਰਾ)
	ਵਾਪਸ;
    tp->tab = INVALID_TABPAGE_ਮੁੱਲ;
    TAB_PYTHON_REF(tab) = ਨਿਰਾ;
}

    ਸਥਿਰ ਪੂਰਨ
PythonMod_Init(ਖਾਲੀ)
{
    // The special value is removed from sys.path in Python_Init().
    ਸਥਿਰ ਅੱਖਰ	*(argv[2]) = {"/must>not&exist/foo", ਨਿਰਾ};

    ਜੇ (init_types())
	ਵਾਪਸ -1;

    // Set sys.argv[] to avoid a crash in warn().
    Py_Sys_ਸੈੱਟ_Argv(1, argv);

    vim_module = Py_InitModule4("vim", VimMethods, (ਅੱਖਰ *)ਨਿਰਾ,
				(PyObject *)ਨਿਰਾ, PYTHON_API_ਸੰਸਕਰਣ);

    ਜੇ (populate_module(vim_module))
	ਵਾਪਸ -1;

    ਜੇ (init_sys_ਰਸਤਾ())
	ਵਾਪਸ -1;

    ਵਾਪਸ 0;
}

//////////////////////////////////////////////////////////////////////////
// 4. Utility functions for handling the interface between Vim and Python.

// Convert a Vim line into a Python string.
// All internal newlines are replaced by null characters.
//
// On errors, the Python exception data is set, and NULL is returned.
    ਸਥਿਰ PyObject *
ਸਤਰ_ਮੰਜ਼ਿਲ_String(ਅਚਲ ਅੱਖਰ *str)
{
    PyObject *ਨਤੀਜਾ;
    PyInt len = ਲੜੀ_ਲੰਬਾਈ(str);
    ਅੱਖਰ *p;

    // Allocate a Python string object, with uninitialised contents. We
    // must do it this way, so that we can modify the string in place
    // later. See the Python source, Objects/stringobject.c for details.
    ਨਤੀਜਾ = Py_String_From_String_And_ਮਾਪ(ਨਿਰਾ, len);
    ਜੇ (ਨਤੀਜਾ == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    p = PyString_AsString(ਨਤੀਜਾ);

    ਜਦੋਂ (*str)
    {
	ਜੇ (*str == '\n')
	    *p = '\0';
	ਨਹੀਂਤਾਂ
	    *p = *str;

	++p;
	++str;
    }

    ਵਾਪਸ ਨਤੀਜਾ;
}

    ਸਥਿਰ PyObject *
DictionaryGetattr(PyObject *self, ਅੱਖਰ *name)
{
    DictionaryObject	*this = ((DictionaryObject *) (self));

    ਜੇ (strcmp(name, "locked") == 0)
	ਵਾਪਸ PyInt_FromLong(this->dict->dv_ਤਾਲਾ);
    ਨਹੀਂਤਾਂ ਜੇ (strcmp(name, "scope") == 0)
	ਵਾਪਸ PyInt_FromLong(this->dict->dv_scope);
    ਨਹੀਂਤਾਂ ਜੇ (strcmp(name, "__members__") == 0)
	ਵਾਪਸ ObjectDir(ਨਿਰਾ, DictionaryAttrs);

    ਵਾਪਸ Py_ਲੱਭੋ_ਵਿਧੀ(DictionaryMethods, self, name);
}

    ਸਥਿਰ PyObject *
ListGetattr(PyObject *self, ਅੱਖਰ *name)
{
    ਜੇ (strcmp(name, "locked") == 0)
	ਵਾਪਸ PyInt_FromLong(((ListObject *)(self))->list->lv_ਤਾਲਾ);
    ਨਹੀਂਤਾਂ ਜੇ (strcmp(name, "__members__") == 0)
	ਵਾਪਸ ObjectDir(ਨਿਰਾ, ListAttrs);

    ਵਾਪਸ Py_ਲੱਭੋ_ਵਿਧੀ(ListMethods, self, name);
}

    ਸਥਿਰ PyObject *
TupleGetattr(PyObject *self, ਅੱਖਰ *name)
{
    ਜੇ (strcmp(name, "locked") == 0)
	ਵਾਪਸ PyInt_FromLong(((TupleObject *)(self))->tuple->tv_ਤਾਲਾ);
    ਨਹੀਂਤਾਂ ਜੇ (strcmp(name, "__members__") == 0)
	ਵਾਪਸ ObjectDir(ਨਿਰਾ, TupleAttrs);

    ਵਾਪਸ Py_ਲੱਭੋ_ਵਿਧੀ(TupleMethods, self, name);
}

    ਸਥਿਰ PyObject *
FunctionGetattr(PyObject *self, ਅੱਖਰ *name)
{
    PyObject	*r;

    r = FunctionAttr((FunctionObject *)(self), name);

    ਜੇ (r || PyErr_Occurred())
	ਵਾਪਸ r;
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ Py_ਲੱਭੋ_ਵਿਧੀ(FunctionMethods, self, name);
}

    ਖਾਲੀ
do_pyeval(char_u *str, dict_T *locals, typval_T *rettv)
{
    DoPyCommand((ਅੱਖਰ *) str,
	    locals,
	    init_range_eval,
	    (runner) ਲੜੀਵਾਰ_eval,
	    (ਖਾਲੀ *) rettv);
    ਜੇ (rettv->v_type == VAR_UNKNOWN)
    {
	rettv->v_type = VAR_ਸੰਖਿਆ;
	rettv->vval.v_ਸੰਖਿਆ = 0;
    }
}

    ਪੂਰਨ
ਸੈੱਟ_ref_ਵਿੱਚ_python(ਪੂਰਨ copyID)
{
    ਵਾਪਸ ਸੈੱਟ_ref_ਵਿੱਚ_py(copyID);
}
