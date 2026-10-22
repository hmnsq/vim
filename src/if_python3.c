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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಅನ್ತರ್ಭಾವ <limits.h>

#ಆದರೆ defined(MSWIN) && defined(HAVE_FCNTL_H)
# undef HAVE_FCNTL_H
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _DEBUG
# undef _DEBUG
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ F_BLANK
# undef F_BLANK
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_DUP
# undef HAVE_DUP
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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _POSIX_C_SOURCE  // defined in feature.h
# undef _POSIX_C_SOURCE
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ XOPEN_ಮೂಲ
# undef XOPEN_ಮೂಲ	// pyconfig.h defines it as well.
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು PY_SSIZE_T_CLEAN

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ Py_LIMITED_API
# define USE_LIMITED_API // Using Python 3 limited ABI
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ <Python.h>

#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು ಮುಖ್ಯ // Defined in python.h - aargh
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು HAVE_FCNTL_H // Clash with os_win32.h

// The "surrogateescape" error handler is new in Python 3.1
#ಆದರೆ PY_ಆವೃತ್ತಿ_HEX >= 0x030100f0
# define CODEC_ERROR_HANDLER "surrogateescape"
#ಇಲ್ಲದಿದ್ದರೆ
# define CODEC_ERROR_HANDLER ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ

// Suppress Python 3.11 depreciations to see useful warnings
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __GNUC__
# pragma GCC diagnostic ತಳ್ಳು
# pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#ಆದರೆ_ಅಂತ್ಯ

// Python 3 does not support CObjects, always use Capsules
#ವ್ಯಾಖ್ಯಾನಿಸು PY_USE_CAPSULE

#ವ್ಯಾಖ್ಯಾನಿಸು ERRORS_DECODE_ARG CODEC_ERROR_HANDLER
#ವ್ಯಾಖ್ಯಾನಿಸು ERRORS_ENCODE_ARG ERRORS_DECODE_ARG

#ವ್ಯಾಖ್ಯಾನಿಸು PyInt Py_ssize_t
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ PyString_Check
# define PyString_Check(obj) PyUnicode_Check(obj)
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು PyString_FromString(repr) \
    PyUnicode_Decode(repr, STRLEN(repr), ENC_OPT, ERRORS_DECODE_ARG)
#ವ್ಯಾಖ್ಯಾನಿಸು PyString_FromFormat PyUnicode_FromFormat
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ PyUnicode_FromFormat
# define Py_UNICODE_USE_UCS_FUNCTIONS
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ PyInt_Check
# define PyInt_Check(obj) PyLong_Check(obj)
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು PyInt_FromLong(i) PyLong_FromLong(i)
#ವ್ಯಾಖ್ಯಾನಿಸು PyInt_AsLong(obj) PyLong_AsLong(obj)
#ವ್ಯಾಖ್ಯಾನಿಸು Py_ssize_t_fmt "n"
#ವ್ಯಾಖ್ಯಾನಿಸು Py_ಬೈಟುಗಳು_fmt "y"

#ವ್ಯಾಖ್ಯಾನಿಸು PyIntArgFunc	ssizeargfunc
#ವ್ಯಾಖ್ಯಾನಿಸು PyIntObjArgProc	ssizeobjargproc

/*
 * PySlice_GetIndicesEx(): first argument type changed from PySliceObject
 * to PyObject in Python 3.2 or later.
 */
#ಆದರೆ PY_ಆವೃತ್ತಿ_HEX >= 0x030200f0
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ PyObject Py_ತುಂಡು_Object_T;
#ಇಲ್ಲದಿದ್ದರೆ
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ Py_ತುಂಡು_Object Py_ತುಂಡು_Object_T;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ MSWIN
# define HINSTANCE ಶೂನ್ಯ *
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(DYNAMIC_PYTHON3) || defined(MSWIN)
ಸ್ಥಿರ HINSTANCE hinstPy3 = 0; // Instance of python.dll
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(DYNAMIC_PYTHON3)

# ifdef MSWIN
#  define ತುಂಬು_dll vim_ತುಂಬು_Lib
#  define close_dll FreeLibrary
#  define ಚಿಹ್ನೆ_from_dll Get_Proc_ವಿಳಾಸ
#  define ತುಂಬು_dll_error GetWin32Error
# ಇಲ್ಲದಿದ್ದರೆ
#  include <dlfcn.h>
#  define FARPROC ಶೂನ್ಯ*
#  ಆದರೆ defined(PY_NO_RTLD_GLOBAL) && defined(PY3_NO_RTLD_GLOBAL)
#   define ತುಂಬು_dll(n) dlopen((n), RTLD_LAZY)
#  ಇಲ್ಲದಿದ್ದರೆ
#   define ತುಂಬು_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#  endif
#  define close_dll dlclose
#  define ಚಿಹ್ನೆ_from_dll dlsym
#  define ತುಂಬು_dll_error dlerror
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
# define Py_Dict_ಗಣ_ಅಂಶ_String py3_Py_Dict_ಗಣ_ಅಂಶ_String
# define Py_Err_Bad_ವಾದ py3_Py_Err_Bad_ವಾದ
# define PyErr_Clear py3_PyErr_Clear
# define PyErr_Format py3_PyErr_Format
# define PyErr_PrintEx py3_PyErr_PrintEx
# define Py_Err_No_ಸ್ಮರಣೆ py3_Py_Err_No_ಸ್ಮರಣೆ
# define PyErr_Occurred py3_PyErr_Occurred
# define Py_Err_ಗಣ_None py3_Py_Err_ಗಣ_None
# define Py_Err_ಗಣ_String py3_Py_Err_ಗಣ_String
# define Py_Err_ಗಣ_Object py3_Py_Err_ಗಣ_Object
# define PyErr_ExceptionMatches py3_PyErr_ExceptionMatches
# define Py_Eval_Init_ಎಳೆಗಳು py3_Py_Eval_Init_ಎಳೆಗಳು
# define PyEval_RestoreThread py3_PyEval_RestoreThread
# define PyEval_SaveThread py3_PyEval_SaveThread
# define PyGILState_Ensure py3_PyGILState_Ensure
# define PyGILState_Release py3_PyGILState_Release
# define PyLong_AsLong py3_PyLong_AsLong
# define PyLong_FromLong py3_PyLong_FromLong
# define Py_List_Get_ಅಂಶ py3_Py_List_Get_ಅಂಶ
# define PyList_Append py3_PyList_Append
# define Py_List_ಸೇರಿಸು py3_Py_List_ಸೇರಿಸು
# define PyList_New py3_PyList_New
# define Py_List_ಗಣ_ಅಂಶ_2 py3_Py_List_ಗಣ_ಅಂಶ
# define Py_List_ಗಾತ್ರ py3_Py_List_ಗಾತ್ರ
# define PyTuple_New py3_PyTuple_New
# define Py_Tuple_Get_ಅಂಶ py3_Py_Tuple_Get_ಅಂಶ
# define Py_Tuple_ಗಣ_ಅಂಶ py3_Py_Tuple_ಗಣ_ಅಂಶ
# define Py_Tuple_ಗಾತ್ರ py3_Py_Tuple_ಗಾತ್ರ
# define PySequence_Check py3_PySequence_Check
# define Py_Sequence_ಗಾತ್ರ py3_Py_Sequence_ಗಾತ್ರ
# define Py_Sequence_Get_ಅಂಶ py3_Py_Sequence_Get_ಅಂಶ
# define PySequence_Fast py3_PySequence_Fast
# ಆದರೆ PY_ಆವೃತ್ತಿ_HEX >= 0x030601f0
#  define Py_ತುಂಡು_Adjust_Indices py3_Py_ತುಂಡು_Adjust_Indices
#  define Py_ತುಂಡು_Unpack py3_Py_ತುಂಡು_Unpack
# endif
# undef Py_ತುಂಡು_Get_Indices_Ex
# define Py_ತುಂಡು_Get_Indices_Ex py3_Py_ತುಂಡು_Get_Indices_Ex
# define PyImport_ImportModule py3_PyImport_ImportModule
# define PyObject_Init py3__PyObject_Init
# define PyDict_New py3_PyDict_New
# define Py_Dict_Get_ಅಂಶ_String py3_Py_Dict_Get_ಅಂಶ_String
# define Py_Dict_ಮುಂದಿನ py3_Py_Dict_ಮುಂದಿನ
# define PyMapping_Check py3_PyMapping_Check
# ifndef Py_Mapping_ಕೀಲಿಗಳು
#  define Py_Mapping_ಕೀಲಿಗಳು py3_Py_Mapping_ಕೀಲಿಗಳು
# endif
# ಆದರೆ (defined(USE_LIMITED_API) && Py_LIMITED_API >= 0x03080000) || \
       (!defined(USE_LIMITED_API) && PY_ಆವೃತ್ತಿ_HEX >= 0x03080000)
#  undef PyIter_Check
#  define PyIter_Check py3_PyIter_Check
# endif
# define Py_Iter_ಮುಂದಿನ py3_Py_Iter_ಮುಂದಿನ
# define PyObject_GetIter py3_PyObject_GetIter
# define PyObject_Repr py3_PyObject_Repr
# define Py_Object_Get_ಅಂಶ py3_Py_Object_Get_ಅಂಶ
# define PyObject_IsTrue py3_PyObject_IsTrue
# define PyModule_GetDict py3_PyModule_GetDict
# ಆದರೆ defined(USE_LIMITED_API) || PY_ಆವೃತ್ತಿ_HEX >= 0x03080000
#  define Py_IncRef py3_Py_IncRef
#  define Py_DecRef py3_Py_DecRef
# endif
# ifdef USE_LIMITED_API
#  define Py_CompileString py3_Py_CompileString
#  define Py_Eval_Eval_ಸಂಕೇತ py3_Py_Eval_Eval_ಸಂಕೇತ
# ಇಲ್ಲದಿದ್ದರೆ
#  undef Py_ಸತತ_Simple_String
#  define Py_ಸತತ_Simple_String py3_Py_ಸತತ_Simple_String
#  undef Py_ಸತತ_String
#  define Py_ಸತತ_String py3_Py_ಸತತ_String
# endif
# define PyObject_GetAttrString py3_PyObject_GetAttrString
# define PyObject_HasAttrString py3_PyObject_HasAttrString
# define Py_Object_ಗಣ_Attr_String py3_Py_Object_ಗಣ_Attr_String
# define Py_Object_Call_Function_Obj_ವಾದಗಳು py3_Py_Object_Call_Function_Obj_ವಾದಗಳು
# ಆದರೆ PY_ಆವೃತ್ತಿ_HEX >= 0x030d0000
#  define PyObject_CallFunction py3_PyObject_CallFunction
# ಇಲ್ಲದಿದ್ದರೆ
#  define Py_Object_Call_Function_ಗಾತ್ರ_T py3_Py_Object_Call_Function_ಗಾತ್ರ_T
# endif
# define PyObject_Call py3_PyObject_Call
# define PyEval_GetLocals py3_PyEval_GetLocals
# define PyEval_GetGlobals py3_PyEval_GetGlobals
# define Py_Sys_ಗಣ_Object py3_Py_Sys_ಗಣ_Object
# define PySys_GetObject py3_PySys_GetObject
# define Py_Sys_ಗಣ_Argv py3_Py_Sys_ಗಣ_Argv
# define PyType_Ready py3_PyType_Ready
# ಆದರೆ PY_ಆವೃತ್ತಿ_HEX >= 0x03040000
#  define PyType_GetFlags py3_PyType_GetFlags
# endif
# undef Py_Build_ಮೌಲ್ಯ
# define Py_Build_ಮೌಲ್ಯ py3_Py_Build_ಮೌಲ್ಯ
# define Py_ಗಣ_Python_Home py3_Py_ಗಣ_Python_Home
# define Py_Initialize py3_Py_Initialize
# define Py_Finalize py3_Py_Finalize
# define Py_IsInitialized py3_Py_IsInitialized
# define _Py_NoneStruct (*py3__Py_NoneStruct)
# define _Py_FalseStruct (*py3__Py_FalseStruct)
# define _Py_TrueStruct (*py3__Py_TrueStruct)
# ಆದರೆ !defined(USE_LIMITED_API) && PY_ಆವೃತ್ತಿ_HEX < 0x030D0000
// Private symbol that used to be required as part of PyIter_Check.
#  define Py_Object_ಮುಂದಿನ_Not_Implemented (*py3_Py_Object_ಮುಂದಿನ_Not_Implemented)
# endif
# define Py_Module_ಕೂಡಿಸು_Object py3_Py_Module_ಕೂಡಿಸು_Object
# define PyImport_AppendInittab py3_PyImport_AppendInittab
# define Py_Import_ಕೂಡಿಸು_Module py3_Py_Import_ಕೂಡಿಸು_Module
# ifdef USE_LIMITED_API
#  ಆದರೆ Py_LIMITED_API >= 0x030a0000
#   define Py_Unicode_As_UTF8_And_ಗಾತ್ರ py3_Py_Unicode_As_UTF8_And_ಗಾತ್ರ
#  endif
# ಇಲ್ಲದಿದ್ದರೆ
#  ಆದರೆ PY_ಆವೃತ್ತಿ_HEX >= 0x030300f0
#   define Py_Unicode_As_UTF8_And_ಗಾತ್ರ py3_Py_Unicode_As_UTF8_And_ಗಾತ್ರ
#  ಇಲ್ಲದಿದ್ದರೆ
#   define _PyUnicode_AsString py3__PyUnicode_AsString
#  endif
# endif
# undef PyUnicode_CompareWithASCIIString
# define PyUnicode_CompareWithASCIIString py3_PyUnicode_CompareWithASCIIString
# undef PyUnicode_AsEncodedString
# define PyUnicode_AsEncodedString py3_PyUnicode_AsEncodedString
# undef PyUnicode_AsUTF8String
# define PyUnicode_AsUTF8String py3_PyUnicode_AsUTF8String
# undef Py_ಬೈಟುಗಳು_As_String
# define Py_ಬೈಟುಗಳು_As_String py3_Py_ಬೈಟುಗಳು_As_String
# ifndef Py_ಬೈಟುಗಳು_As_String_And_ಗಾತ್ರ
#  define Py_ಬೈಟುಗಳು_As_String_And_ಗಾತ್ರ py3_Py_ಬೈಟುಗಳು_As_String_And_ಗಾತ್ರ
# endif
# undef Py_ಬೈಟುಗಳು_From_String
# define Py_ಬೈಟುಗಳು_From_String py3_Py_ಬೈಟುಗಳು_From_String
# undef Py_ಬೈಟುಗಳು_From_String_And_ಗಾತ್ರ
# define Py_ಬೈಟುಗಳು_From_String_And_ಗಾತ್ರ py3_Py_ಬೈಟುಗಳು_From_String_And_ಗಾತ್ರ
# define PyFloat_FromDouble py3_PyFloat_FromDouble
# define PyFloat_AsDouble py3_PyFloat_AsDouble
# define PyObject_GenericGetAttr py3_PyObject_GenericGetAttr
# define PyType_Type (*py3_PyType_Type)
# ifndef USE_LIMITED_API
#  define PyStdPrinter_Type (*py3_PyStdPrinter_Type)
# endif
# define Py_ತುಂಡು_Type (*py3_Py_ತುಂಡು_Type)
# define PyFloat_Type (*py3_PyFloat_Type)
# define Py_ಸಂಖ್ಯೆ_Check (*py3_Py_ಸಂಖ್ಯೆ_Check)
# define Py_ಸಂಖ್ಯೆ_Long (*py3_Py_ಸಂಖ್ಯೆ_Long)
# define PyBool_Type (*py3_PyBool_Type)
# define PyErr_NewException py3_PyErr_NewException
# ifdef Py_DEBUG
#  define _Py_NegativeRefcount py3__Py_NegativeRefcount
#  define Py_Ref_ಒಟ್ಟು (*py3_Py_Ref_ಒಟ್ಟು)
#  define PyModule_Create2TraceRefs py3_PyModule_Create2TraceRefs
# ಇಲ್ಲದಿದ್ದರೆ
#  define PyModule_Create2 py3_PyModule_Create2
# endif
# ಆದರೆ defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
#  define _PyObject_DebugMalloc py3__PyObject_DebugMalloc
#  define _PyObject_DebugFree py3__PyObject_DebugFree
# ಇಲ್ಲದಿದ್ದರೆ
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
#  ಇಲ್ಲದಿದ್ದರೆ
#   define PyUnicodeUCS2_FromFormat py3_PyUnicodeUCS2_FromFormat
#  endif
# ಇಲ್ಲದಿದ್ದರೆ
#  define PyUnicode_FromFormat py3_PyUnicode_FromFormat
# endif
# undef PyUnicode_Decode
# define PyUnicode_Decode py3_PyUnicode_Decode
# define PyType_IsSubtype py3_PyType_IsSubtype
# define PyCapsule_New py3_PyCapsule_New
# define Py_Capsule_Get_ಸೂಚಕ py3_Py_Capsule_Get_ಸೂಚಕ
# ifdef USE_LIMITED_API
#  define Py_Type_Get_ಸ್ಥಾನ py3_Py_Type_Get_ಸ್ಥಾನ
#  define PyType_FromSpec py3_PyType_FromSpec
# endif

# ಆದರೆ defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
#  undef PyObject_NEW
#  define PyObject_NEW(type, typeobj) \
( (type *) PyObject_Init( \
	(PyObject *) _PyObject_DebugMalloc( Py_Object_ಗಾತ್ರ(typeobj) ), (typeobj)) )
# elif PY_ಆವೃತ್ತಿ_HEX >= 0x030900b0
#  undef PyObject_NEW
#  define PyObject_NEW(type, typeobj) \
	((type *)py3__PyObject_New(typeobj))
# endif

/*
 * Pointers for dynamic link
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*py3_Py_Sys_ಗಣ_Argv)(ಪೂರ್ಣಾಂಕ, wchar_t **);
ಸ್ಥಿರ ಶೂನ್ಯ (*py3_Py_ಗಣ_Python_Home)(wchar_t *home);
ಸ್ಥಿರ ಶೂನ್ಯ (*py3_Py_Initialize)(ಶೂನ್ಯ);
ಸ್ಥಿರ PyObject* (*py3_PyList_New)(Py_ssize_t ಗಾತ್ರ_2);
ಸ್ಥಿರ PyGILState_STATE (*py3_PyGILState_Ensure)(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ (*py3_PyGILState_Release)(PyGILState_STATE);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*py3_Py_Sys_ಗಣ_Object)(ಅಕ್ಷರ *, PyObject *);
ಸ್ಥಿರ PyObject* (*py3_PySys_GetObject)(ಅಕ್ಷರ *);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*py3_PyList_Append)(PyObject *, PyObject *);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*py3_Py_List_ಸೇರಿಸು)(PyObject *, ಪೂರ್ಣಾಂಕ, PyObject *);
ಸ್ಥಿರ Py_ssize_t (*py3_Py_List_ಗಾತ್ರ)(PyObject *);
ಸ್ಥಿರ PyObject* (*py3_PyTuple_New)(Py_ssize_t ಗಾತ್ರ_2);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*py3_Py_Tuple_ಗಣ_ಅಂಶ)(PyObject *, Py_ssize_t, PyObject *);
ಸ್ಥಿರ Py_ssize_t (*py3_Py_Tuple_ಗಾತ್ರ)(PyObject *);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*py3_PySequence_Check)(PyObject *);
ಸ್ಥಿರ Py_ssize_t (*py3_Py_Sequence_ಗಾತ್ರ)(PyObject *);
ಸ್ಥಿರ PyObject* (*py3_Py_Sequence_Get_ಅಂಶ)(PyObject *, Py_ssize_t);
ಸ್ಥಿರ PyObject* (*py3_PySequence_Fast)(PyObject *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *);
ಸ್ಥಿರ PyObject* (*py3_Py_Tuple_Get_ಅಂಶ)(PyObject *, Py_ssize_t);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*py3_PyMapping_Check)(PyObject *);
ಸ್ಥಿರ PyObject* (*py3_Py_Mapping_ಕೀಲಿಗಳು)(PyObject *);
# ಆದರೆ PY_ಆವೃತ್ತಿ_HEX >= 0x030601f0
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*py3_Py_ತುಂಡು_Adjust_Indices)(Py_ssize_t ಉದ್ದಳತೆ,
		     Py_ssize_t *ಆರಂಭ, Py_ssize_t *stop, Py_ssize_t ಹೆಜ್ಜೆ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*py3_Py_ತುಂಡು_Unpack)(PyObject *ತುಂಡು,
		     Py_ssize_t *ಆರಂಭ, Py_ssize_t *stop, Py_ssize_t *ಹೆಜ್ಜೆ);
# endif
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*py3_Py_ತುಂಡು_Get_Indices_Ex)(Py_ತುಂಡು_Object_T *r, Py_ssize_t ಉದ್ದಳತೆ,
		     Py_ssize_t *ಆರಂಭ, Py_ssize_t *stop, Py_ssize_t *ಹೆಜ್ಜೆ,
		     Py_ssize_t *slicelen);
ಸ್ಥಿರ PyObject* (*py3_Py_Err_No_ಸ್ಮರಣೆ)(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ (*py3_Py_Finalize)(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ (*py3_Py_Err_ಗಣ_String)(PyObject *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *);
ಸ್ಥಿರ ಶೂನ್ಯ (*py3_Py_Err_ಗಣ_Object)(PyObject *, PyObject *);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*py3_PyErr_ExceptionMatches)(PyObject *);
# ಆದರೆ defined(USE_LIMITED_API) || PY_ಆವೃತ್ತಿ_HEX >= 0x03080000
ಸ್ಥಿರ ಶೂನ್ಯ (*py3_Py_IncRef)(PyObject *);
ಸ್ಥಿರ ಶೂನ್ಯ (*py3_Py_DecRef)(PyObject *);
# endif
# ifdef USE_LIMITED_API
ಸ್ಥಿರ PyObject* (*py3_Py_CompileString)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *, ಪೂರ್ಣಾಂಕ);
ಸ್ಥಿರ PyObject* (*py3_Py_Eval_Eval_ಸಂಕೇತ)(PyObject *co, PyObject *globals, PyObject *locals);
# ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*py3_Py_ಸತತ_Simple_String)(ಅಕ್ಷರ *);
ಸ್ಥಿರ PyObject* (*py3_Py_ಸತತ_String)(ಅಕ್ಷರ *, ಪೂರ್ಣಾಂಕ, PyObject *, PyObject *);
# endif
ಸ್ಥಿರ PyObject* (*py3_PyObject_GetAttrString)(PyObject *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*py3_PyObject_HasAttrString)(PyObject *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*py3_Py_Object_ಗಣ_Attr_String)(PyObject *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *, PyObject *);
ಸ್ಥಿರ PyObject* (*py3_Py_Object_Call_Function_Obj_ವಾದಗಳು)(PyObject *, ...);
# ಆದರೆ PY_ಆವೃತ್ತಿ_HEX >= 0x030d0000
ಸ್ಥಿರ PyObject* (*py3_PyObject_CallFunction)(PyObject *, ಅಕ್ಷರ *, ...);
# ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ PyObject* (*py3_Py_Object_Call_Function_ಗಾತ್ರ_T)(PyObject *, ಅಕ್ಷರ *, ...);
# endif
ಸ್ಥಿರ PyObject* (*py3_PyObject_Call)(PyObject *, PyObject *, PyObject *);
ಸ್ಥಿರ PyObject* (*py3_PyEval_GetGlobals)(ಶೂನ್ಯ);
ಸ್ಥಿರ PyObject* (*py3_PyEval_GetLocals)(ಶೂನ್ಯ);
ಸ್ಥಿರ PyObject* (*py3_Py_List_Get_ಅಂಶ)(PyObject *, Py_ssize_t);
ಸ್ಥಿರ PyObject* (*py3_PyImport_ImportModule)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *);
ಸ್ಥಿರ PyObject* (*py3_Py_Import_ಕೂಡಿಸು_Module)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*py3_Py_Err_Bad_ವಾದ)(ಶೂನ್ಯ);
ಸ್ಥಿರ PyObject* (*py3_PyErr_Occurred)(ಶೂನ್ಯ);
ಸ್ಥಿರ PyObject* (*py3_PyModule_GetDict)(PyObject *);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*py3_Py_List_ಗಣ_ಅಂಶ)(PyObject *, Py_ssize_t, PyObject *);
ಸ್ಥಿರ PyObject* (*py3_Py_Dict_Get_ಅಂಶ_String)(PyObject *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*py3_Py_Dict_ಮುಂದಿನ)(PyObject *, Py_ssize_t *, PyObject **, PyObject **);
ಸ್ಥಿರ PyObject* (*py3_PyLong_FromLong)(ದೀರ್ಘ);
ಸ್ಥಿರ PyObject* (*py3_PyDict_New)(ಶೂನ್ಯ);
# ಆದರೆ (defined(USE_LIMITED_API) && Py_LIMITED_API >= 0x03080000) || \
       (!defined(USE_LIMITED_API) && PY_ಆವೃತ್ತಿ_HEX >= 0x03080000)
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*py3_PyIter_Check)(PyObject *o);
# endif
ಸ್ಥಿರ PyObject* (*py3_Py_Iter_ಮುಂದಿನ)(PyObject *);
ಸ್ಥಿರ PyObject* (*py3_PyObject_GetIter)(PyObject *);
ಸ್ಥಿರ PyObject* (*py3_PyObject_Repr)(PyObject *);
ಸ್ಥಿರ PyObject* (*py3_Py_Object_Get_ಅಂಶ)(PyObject *, PyObject *);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*py3_PyObject_IsTrue)(PyObject *);
ಸ್ಥಿರ PyObject* (*py3_Py_Build_ಮೌಲ್ಯ)(ಅಕ್ಷರ *, ...);
# ಆದರೆ PY_ಆವೃತ್ತಿ_HEX >= 0x03040000
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*py3_PyType_GetFlags)(PyTypeObject *o);
# endif
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*py3_PyType_Ready)(PyTypeObject *type);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*py3_Py_Dict_ಗಣ_ಅಂಶ_String)(PyObject *dp, ಅಕ್ಷರ *ಕೀಲಿ, PyObject *ಅಂಶ);
ಸ್ಥಿರ PyObject* (*py3_PyUnicode_FromString)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *u);
# ifdef Py_UNICODE_USE_UCS_FUNCTIONS
#  ifdef Py_UNICODE_WIDE
ಸ್ಥಿರ PyObject* (*py3_PyUnicodeUCS4_FromFormat)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *u, ...);
#  ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ PyObject* (*py3_PyUnicodeUCS2_FromFormat)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *u, ...);
#  endif
# ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ PyObject* (*py3_PyUnicode_FromFormat)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *u, ...);
# endif
ಸ್ಥಿರ PyObject* (*py3_PyUnicode_Decode)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *u, Py_ssize_t ಗಾತ್ರ_2,
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *encoding, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *errors);
ಸ್ಥಿರ ದೀರ್ಘ (*py3_PyLong_AsLong)(PyObject *);
ಸ್ಥಿರ ಶೂನ್ಯ (*py3_Py_Err_ಗಣ_None)(PyObject *);
ಸ್ಥಿರ ಶೂನ್ಯ (*py3_Py_Eval_Init_ಎಳೆಗಳು)(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ(*py3_PyEval_RestoreThread)(PyThreadState *);
ಸ್ಥಿರ PyThreadState*(*py3_PyEval_SaveThread)(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*py3_PyArg_Parse)(PyObject *, ಅಕ್ಷರ *, ...);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*py3_PyArg_ParseTuple)(PyObject *, ಅಕ್ಷರ *, ...);
ಸ್ಥಿರ ಶೂನ್ಯ (*py3_PyMem_Free)(ಶೂನ್ಯ *);
ಸ್ಥಿರ ಶೂನ್ಯ* (*py3_PyMem_Malloc)(size_t);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*py3_Py_IsInitialized)(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ (*py3_PyErr_Clear)(ಶೂನ್ಯ);
ಸ್ಥಿರ PyObject* (*py3_PyErr_Format)(PyObject *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *, ...);
ಸ್ಥಿರ ಶೂನ್ಯ (*py3_PyErr_PrintEx)(ಪೂರ್ಣಾಂಕ);
ಸ್ಥಿರ PyObject*(*py3__PyObject_Init)(PyObject *, PyTypeObject *);
# ಆದರೆ !defined(USE_LIMITED_API) && PY_ಆವೃತ್ತಿ_HEX < 0x030D0000
ಸ್ಥಿರ iternextfunc py3_Py_Object_ಮುಂದಿನ_Not_Implemented;
# endif
ಸ್ಥಿರ PyObject* py3__Py_NoneStruct;
ಸ್ಥಿರ PyObject* py3__Py_FalseStruct;
ಸ್ಥಿರ PyObject* py3__Py_TrueStruct;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*py3_Py_Module_ಕೂಡಿಸು_Object)(PyObject *m, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, PyObject *o);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*py3_PyImport_AppendInittab)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, PyObject* (*initfunc)(ಶೂನ್ಯ));
# ifdef USE_LIMITED_API
#  ಆದರೆ Py_LIMITED_API >= 0x030a0000
ಸ್ಥಿರ ಅಕ್ಷರ* (*py3_Py_Unicode_As_UTF8_And_ಗಾತ್ರ)(PyObject *unicode, Py_ssize_t *ಗಾತ್ರ_2);
#  endif
# ಇಲ್ಲದಿದ್ದರೆ
#  ಆದರೆ PY_ಆವೃತ್ತಿ_HEX >= 0x030300f0
ಸ್ಥಿರ ಅಕ್ಷರ* (*py3_Py_Unicode_As_UTF8_And_ಗಾತ್ರ)(PyObject *unicode, Py_ssize_t *ಗಾತ್ರ_2);
#  ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ ಅಕ್ಷರ* (*py3__PyUnicode_AsString)(PyObject *unicode);
#  endif
# endif
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*py3_PyUnicode_CompareWithASCIIString)(PyObject *unicode, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* string);
ಸ್ಥಿರ PyObject* (*py3_PyUnicode_AsEncodedString)(PyObject *unicode, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* encoding, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* errors);
ಸ್ಥಿರ PyObject* (*py3_PyUnicode_AsUTF8String)(PyObject *unicode);
ಸ್ಥಿರ ಅಕ್ಷರ* (*py3_Py_ಬೈಟುಗಳು_As_String)(PyObject *ಬೈಟುಗಳು);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*py3_Py_ಬೈಟುಗಳು_As_String_And_ಗಾತ್ರ)(PyObject *ಬೈಟುಗಳು, ಅಕ್ಷರ **ಬಫರ್, Py_ssize_t *ಉದ್ದಳತೆ);
ಸ್ಥಿರ PyObject* (*py3_Py_ಬೈಟುಗಳು_From_String)(ಅಕ್ಷರ *str);
ಸ್ಥಿರ PyObject* (*py3_Py_ಬೈಟುಗಳು_From_String_And_ಗಾತ್ರ)(ಅಕ್ಷರ *str, Py_ssize_t ಉದ್ದಳತೆ);
# ಆದರೆ PY_ಆವೃತ್ತಿ_HEX >= 0x030900b0
ಸ್ಥಿರ PyObject* (*py3__PyObject_New)(PyTypeObject *);
# endif
ಸ್ಥಿರ PyObject* (*py3_PyFloat_FromDouble)(ದ್ವಿಗುಣ ಅಂಕೆ);
ಸ್ಥಿರ ದ್ವಿಗುಣ (*py3_PyFloat_AsDouble)(PyObject *);
ಸ್ಥಿರ PyObject* (*py3_PyObject_GenericGetAttr)(PyObject *obj, PyObject *name);
ಸ್ಥಿರ PyObject* (*py3_PyType_GenericAlloc)(PyTypeObject *type, Py_ssize_t nitems);
ಸ್ಥಿರ PyObject* (*py3_PyType_GenericNew)(PyTypeObject *type, PyObject *ವಾದಗಳು, PyObject *kwds);
ಸ್ಥಿರ PyTypeObject* py3_PyType_Type;
# ifndef USE_LIMITED_API
ಸ್ಥಿರ PyTypeObject* py3_PyStdPrinter_Type;
# endif
ಸ್ಥಿರ PyTypeObject* py3_Py_ತುಂಡು_Type;
ಸ್ಥಿರ PyTypeObject* py3_PyFloat_Type;
ಸ್ಥಿರ PyTypeObject* py3_PyBool_Type;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*py3_Py_ಸಂಖ್ಯೆ_Check)(PyObject *);
ಸ್ಥಿರ PyObject* (*py3_Py_ಸಂಖ್ಯೆ_Long)(PyObject *);
ಸ್ಥಿರ PyObject* (*py3_PyErr_NewException)(ಅಕ್ಷರ *name, PyObject *ತಳ, PyObject *dict);
ಸ್ಥಿರ PyObject* (*py3_PyCapsule_New)(ಶೂನ್ಯ *, ಅಕ್ಷರ *, PyCapsule_Destructor);
ಸ್ಥಿರ ಶೂನ್ಯ* (*py3_Py_Capsule_Get_ಸೂಚಕ)(PyObject *, ಅಕ್ಷರ *);
# ifdef Py_DEBUG
ಸ್ಥಿರ ಶೂನ್ಯ (*py3__Py_NegativeRefcount)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fname, ಪೂರ್ಣಾಂಕ lineno, PyObject *op);
ಸ್ಥಿರ Py_ssize_t* py3_Py_Ref_ಒಟ್ಟು;
ಸ್ಥಿರ PyObject* (*py3_PyModule_Create2TraceRefs)(ರಚನೆ PyModuleDef* module, ಪೂರ್ಣಾಂಕ module_api_ಆವೃತ್ತಿ);
# ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ PyObject* (*py3_PyModule_Create2)(ರಚನೆ PyModuleDef* module, ಪೂರ್ಣಾಂಕ module_api_ಆವೃತ್ತಿ);
# endif
# ಆದರೆ defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
ಸ್ಥಿರ ಶೂನ್ಯ (*py3__PyObject_DebugFree)(ಶೂನ್ಯ*);
ಸ್ಥಿರ ಶೂನ್ಯ* (*py3__PyObject_DebugMalloc)(size_t);
# ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ ಶೂನ್ಯ (*py3_PyObject_Free)(ಶೂನ್ಯ*);
ಸ್ಥಿರ ಶೂನ್ಯ* (*py3_PyObject_Malloc)(size_t);
# endif
ಸ್ಥಿರ PyObject*(*py3__PyObject_GC_New)(PyTypeObject *);
ಸ್ಥಿರ ಶೂನ್ಯ(*py3_PyObject_GC_Del)(ಶೂನ್ಯ *);
ಸ್ಥಿರ ಶೂನ್ಯ(*py3_PyObject_GC_UnTrack)(ಶೂನ್ಯ *);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*py3_PyType_IsSubtype)(PyTypeObject *, PyTypeObject *);
# ifdef USE_LIMITED_API
ಸ್ಥಿರ ಶೂನ್ಯ* (*py3_Py_Type_Get_ಸ್ಥಾನ)(PyTypeObject *, ಪೂರ್ಣಾಂಕ);
ಸ್ಥಿರ PyObject* (*py3_PyType_FromSpec)(PyType_Spec *);
# endif

// Imported exception objects
ಸ್ಥಿರ PyObject *p3imp_PyExc_AttributeError;
ಸ್ಥಿರ PyObject *p3imp_Py_Exc_ಸೂಚ್ಯಂಕ_Error;
ಸ್ಥಿರ PyObject *p3imp_Py_Exc_ಕೀಲಿ_Error;
ಸ್ಥಿರ PyObject *p3imp_Py_Exc_Keyboard_ಅಡಚಣೆ;
ಸ್ಥಿರ PyObject *p3imp_PyExc_TypeError;
ಸ್ಥಿರ PyObject *p3imp_Py_Exc_ಮೌಲ್ಯ_Error;
ಸ್ಥಿರ PyObject *p3imp_PyExc_SystemExit;
ಸ್ಥಿರ PyObject *p3imp_PyExc_RuntimeError;
ಸ್ಥಿರ PyObject *p3imp_PyExc_ImportError;
ಸ್ಥಿರ PyObject *p3imp_Py_Exc_ಅತಿಪ್ರವಾಹ_Error;

# define PyExc_AttributeError p3imp_PyExc_AttributeError
# define Py_Exc_ಸೂಚ್ಯಂಕ_Error p3imp_Py_Exc_ಸೂಚ್ಯಂಕ_Error
# define Py_Exc_ಕೀಲಿ_Error p3imp_Py_Exc_ಕೀಲಿ_Error
# define Py_Exc_Keyboard_ಅಡಚಣೆ p3imp_Py_Exc_Keyboard_ಅಡಚಣೆ
# define PyExc_TypeError p3imp_PyExc_TypeError
# define Py_Exc_ಮೌಲ್ಯ_Error p3imp_Py_Exc_ಮೌಲ್ಯ_Error
# define PyExc_SystemExit p3imp_PyExc_SystemExit
# define PyExc_RuntimeError p3imp_PyExc_RuntimeError
# define PyExc_ImportError p3imp_PyExc_ImportError
# define Py_Exc_ಅತಿಪ್ರವಾಹ_Error p3imp_Py_Exc_ಅತಿಪ್ರವಾಹ_Error

/*
 * Table of name to function pointer of python.
 */
# define PYTHON_PROC FARPROC
ಸ್ಥಿರ ರಚನೆ
{
    ಅಕ್ಷರ *name;
    PYTHON_PROC *ptr;
} py3_funcname_ಕೋಷ್ಟಕ[] =
{
    {"PySys_SetArgv", (PYTHON_PROC*)&py3_Py_Sys_ಗಣ_Argv},
    {"Py_SetPythonHome", (PYTHON_PROC*)&py3_Py_ಗಣ_Python_Home},
    {"Py_Initialize", (PYTHON_PROC*)&py3_Py_Initialize},
    {"_PyArg_ParseTuple_SizeT", (PYTHON_PROC*)&py3_PyArg_ParseTuple},
    {"_Py_BuildValue_SizeT", (PYTHON_PROC*)&py3_Py_Build_ಮೌಲ್ಯ},
    {"PyMem_Free", (PYTHON_PROC*)&py3_PyMem_Free},
    {"PyMem_Malloc", (PYTHON_PROC*)&py3_PyMem_Malloc},
    {"PyList_New", (PYTHON_PROC*)&py3_PyList_New},
    {"PyGILState_Ensure", (PYTHON_PROC*)&py3_PyGILState_Ensure},
    {"PyGILState_Release", (PYTHON_PROC*)&py3_PyGILState_Release},
    {"PySys_SetObject", (PYTHON_PROC*)&py3_Py_Sys_ಗಣ_Object},
    {"PySys_GetObject", (PYTHON_PROC*)&py3_PySys_GetObject},
    {"PyList_Append", (PYTHON_PROC*)&py3_PyList_Append},
    {"PyList_Insert", (PYTHON_PROC*)&py3_Py_List_ಸೇರಿಸು},
    {"PyList_Size", (PYTHON_PROC*)&py3_Py_List_ಗಾತ್ರ},
    {"PyTuple_New", (PYTHON_PROC*)&py3_PyTuple_New},
    {"PyTuple_GetItem", (PYTHON_PROC*)&py3_Py_Tuple_Get_ಅಂಶ},
    {"PyTuple_SetItem", (PYTHON_PROC*)&py3_Py_Tuple_ಗಣ_ಅಂಶ},
    {"PyTuple_Size", (PYTHON_PROC*)&py3_Py_Tuple_ಗಾತ್ರ},
    {"PySequence_Check", (PYTHON_PROC*)&py3_PySequence_Check},
    {"PySequence_Size", (PYTHON_PROC*)&py3_Py_Sequence_ಗಾತ್ರ},
    {"PySequence_GetItem", (PYTHON_PROC*)&py3_Py_Sequence_Get_ಅಂಶ},
    {"PySequence_Fast", (PYTHON_PROC*)&py3_PySequence_Fast},
# ಆದರೆ PY_ಆವೃತ್ತಿ_HEX >= 0x030601f0
    {"PySlice_AdjustIndices", (PYTHON_PROC*)&py3_Py_ತುಂಡು_Adjust_Indices},
    {"PySlice_Unpack", (PYTHON_PROC*)&py3_Py_ತುಂಡು_Unpack},
# endif
    {"PySlice_GetIndicesEx", (PYTHON_PROC*)&py3_Py_ತುಂಡು_Get_Indices_Ex},
    {"PyErr_NoMemory", (PYTHON_PROC*)&py3_Py_Err_No_ಸ್ಮರಣೆ},
    {"Py_Finalize", (PYTHON_PROC*)&py3_Py_Finalize},
    {"PyErr_SetString", (PYTHON_PROC*)&py3_Py_Err_ಗಣ_String},
    {"PyErr_SetObject", (PYTHON_PROC*)&py3_Py_Err_ಗಣ_Object},
    {"PyErr_ExceptionMatches", (PYTHON_PROC*)&py3_PyErr_ExceptionMatches},
# ಆದರೆ defined(USE_LIMITED_API) || PY_ಆವೃತ್ತಿ_HEX >= 0x03080000
    {"Py_IncRef", (PYTHON_PROC*)&py3_Py_IncRef},
    {"Py_DecRef", (PYTHON_PROC*)&py3_Py_DecRef},
# endif
# ifdef USE_LIMITED_API
    {"Py_CompileString", (PYTHON_PROC*)&py3_Py_CompileString},
    {"PyEval_EvalCode", (PYTHON_PROC*)&Py_Eval_Eval_ಸಂಕೇತ},
# ಇಲ್ಲದಿದ್ದರೆ
    {"PyRun_SimpleString", (PYTHON_PROC*)&py3_Py_ಸತತ_Simple_String},
    {"PyRun_String", (PYTHON_PROC*)&py3_Py_ಸತತ_String},
# endif
    {"PyObject_GetAttrString", (PYTHON_PROC*)&py3_PyObject_GetAttrString},
    {"PyObject_HasAttrString", (PYTHON_PROC*)&py3_PyObject_HasAttrString},
    {"PyObject_SetAttrString", (PYTHON_PROC*)&py3_Py_Object_ಗಣ_Attr_String},
    {"PyObject_CallFunctionObjArgs", (PYTHON_PROC*)&py3_Py_Object_Call_Function_Obj_ವಾದಗಳು},
# ಆದರೆ PY_ಆವೃತ್ತಿ_HEX >= 0x030d0000
    {"PyObject_CallFunction", (PYTHON_PROC*)&py3_PyObject_CallFunction},
# ಇಲ್ಲದಿದ್ದರೆ
    {"_PyObject_CallFunction_SizeT", (PYTHON_PROC*)&py3_Py_Object_Call_Function_ಗಾತ್ರ_T},
# endif
    {"PyObject_Call", (PYTHON_PROC*)&py3_PyObject_Call},
    {"PyEval_GetGlobals", (PYTHON_PROC*)&py3_PyEval_GetGlobals},
    {"PyEval_GetLocals", (PYTHON_PROC*)&py3_PyEval_GetLocals},
    {"PyList_GetItem", (PYTHON_PROC*)&py3_Py_List_Get_ಅಂಶ},
    {"PyImport_ImportModule", (PYTHON_PROC*)&py3_PyImport_ImportModule},
    {"PyImport_AddModule", (PYTHON_PROC*)&py3_Py_Import_ಕೂಡಿಸು_Module},
    {"PyErr_BadArgument", (PYTHON_PROC*)&py3_Py_Err_Bad_ವಾದ},
    {"PyErr_Occurred", (PYTHON_PROC*)&py3_PyErr_Occurred},
    {"PyModule_GetDict", (PYTHON_PROC*)&py3_PyModule_GetDict},
    {"PyList_SetItem", (PYTHON_PROC*)&py3_Py_List_ಗಣ_ಅಂಶ},
    {"PyDict_GetItemString", (PYTHON_PROC*)&py3_Py_Dict_Get_ಅಂಶ_String},
    {"PyDict_Next", (PYTHON_PROC*)&py3_Py_Dict_ಮುಂದಿನ},
    {"PyMapping_Check", (PYTHON_PROC*)&py3_PyMapping_Check},
    {"PyMapping_Keys", (PYTHON_PROC*)&py3_Py_Mapping_ಕೀಲಿಗಳು},
# ಆದರೆ (defined(USE_LIMITED_API) && Py_LIMITED_API >= 0x03080000) || \
       (!defined(USE_LIMITED_API) && PY_ಆವೃತ್ತಿ_HEX >= 0x03080000)
    {"PyIter_Check", (PYTHON_PROC*)&py3_PyIter_Check},
# endif
    {"PyIter_Next", (PYTHON_PROC*)&py3_Py_Iter_ಮುಂದಿನ},
    {"PyObject_GetIter", (PYTHON_PROC*)&py3_PyObject_GetIter},
    {"PyObject_Repr", (PYTHON_PROC*)&py3_PyObject_Repr},
    {"PyObject_GetItem", (PYTHON_PROC*)&py3_Py_Object_Get_ಅಂಶ},
    {"PyObject_IsTrue", (PYTHON_PROC*)&py3_PyObject_IsTrue},
    {"PyLong_FromLong", (PYTHON_PROC*)&py3_PyLong_FromLong},
    {"PyDict_New", (PYTHON_PROC*)&py3_PyDict_New},
# ಆದರೆ PY_ಆವೃತ್ತಿ_HEX >= 0x03040000
    {"PyType_GetFlags", (PYTHON_PROC*)&py3_PyType_GetFlags},
# endif
    {"PyType_Ready", (PYTHON_PROC*)&py3_PyType_Ready},
    {"PyDict_SetItemString", (PYTHON_PROC*)&py3_Py_Dict_ಗಣ_ಅಂಶ_String},
    {"PyLong_AsLong", (PYTHON_PROC*)&py3_PyLong_AsLong},
    {"PyErr_SetNone", (PYTHON_PROC*)&py3_Py_Err_ಗಣ_None},
    {"PyEval_InitThreads", (PYTHON_PROC*)&py3_Py_Eval_Init_ಎಳೆಗಳು},
    {"PyEval_RestoreThread", (PYTHON_PROC*)&py3_PyEval_RestoreThread},
    {"PyEval_SaveThread", (PYTHON_PROC*)&py3_PyEval_SaveThread},
    {"_PyArg_Parse_SizeT", (PYTHON_PROC*)&py3_PyArg_Parse},
    {"Py_IsInitialized", (PYTHON_PROC*)&py3_Py_IsInitialized},
# ಆದರೆ !defined(USE_LIMITED_API) && PY_ಆವೃತ್ತಿ_HEX < 0x030D0000
    {"_PyObject_NextNotImplemented", (PYTHON_PROC*)&py3_Py_Object_ಮುಂದಿನ_Not_Implemented},
# endif
    {"_Py_NoneStruct", (PYTHON_PROC*)&py3__Py_NoneStruct},
    {"_Py_FalseStruct", (PYTHON_PROC*)&py3__Py_FalseStruct},
    {"_Py_TrueStruct", (PYTHON_PROC*)&py3__Py_TrueStruct},
    {"PyErr_Clear", (PYTHON_PROC*)&py3_PyErr_Clear},
    {"PyErr_Format", (PYTHON_PROC*)&py3_PyErr_Format},
    {"PyErr_PrintEx", (PYTHON_PROC*)&py3_PyErr_PrintEx},
    {"PyObject_Init", (PYTHON_PROC*)&py3__PyObject_Init},
    {"PyModule_AddObject", (PYTHON_PROC*)&py3_Py_Module_ಕೂಡಿಸು_Object},
    {"PyImport_AppendInittab", (PYTHON_PROC*)&py3_PyImport_AppendInittab},
# ifdef USE_LIMITED_API
#  ಆದರೆ Py_LIMITED_API >= 0x030a0000
    {"PyUnicode_AsUTF8AndSize", (PYTHON_PROC*)&py3_Py_Unicode_As_UTF8_And_ಗಾತ್ರ},
#  endif
# ಇಲ್ಲದಿದ್ದರೆ
#  ಆದರೆ PY_ಆವೃತ್ತಿ_HEX >= 0x030300f0
    {"PyUnicode_AsUTF8AndSize", (PYTHON_PROC*)&py3_Py_Unicode_As_UTF8_And_ಗಾತ್ರ},
#  ಇಲ್ಲದಿದ್ದರೆ
    {"_PyUnicode_AsString", (PYTHON_PROC*)&py3__PyUnicode_AsString},
#  endif
# endif
    {"PyUnicode_CompareWithASCIIString", (PYTHON_PROC*)&py3_PyUnicode_CompareWithASCIIString},
    {"PyUnicode_AsUTF8String", (PYTHON_PROC*)&py3_PyUnicode_AsUTF8String},
# ifdef Py_UNICODE_USE_UCS_FUNCTIONS
#  ifdef Py_UNICODE_WIDE
    {"PyUnicodeUCS4_FromFormat", (PYTHON_PROC*)&py3_PyUnicodeUCS4_FromFormat},
#  ಇಲ್ಲದಿದ್ದರೆ
    {"PyUnicodeUCS2_FromFormat", (PYTHON_PROC*)&py3_PyUnicodeUCS2_FromFormat},
#  endif
# ಇಲ್ಲದಿದ್ದರೆ
    {"PyUnicode_FromFormat", (PYTHON_PROC*)&py3_PyUnicode_FromFormat},
# endif
    {"PyBytes_AsString", (PYTHON_PROC*)&py3_Py_ಬೈಟುಗಳು_As_String},
    {"PyBytes_AsStringAndSize", (PYTHON_PROC*)&py3_Py_ಬೈಟುಗಳು_As_String_And_ಗಾತ್ರ},
    {"PyBytes_FromString", (PYTHON_PROC*)&py3_Py_ಬೈಟುಗಳು_From_String},
    {"PyBytes_FromStringAndSize", (PYTHON_PROC*)&py3_Py_ಬೈಟುಗಳು_From_String_And_ಗಾತ್ರ},
# ಆದರೆ PY_ಆವೃತ್ತಿ_HEX >= 0x030900b0
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
    {"PySlice_Type", (PYTHON_PROC*)&py3_Py_ತುಂಡು_Type},
    {"PyFloat_Type", (PYTHON_PROC*)&py3_PyFloat_Type},
    {"PyBool_Type", (PYTHON_PROC*)&py3_PyBool_Type},
    {"PyNumber_Check", (PYTHON_PROC*)&py3_Py_ಸಂಖ್ಯೆ_Check},
    {"PyNumber_Long", (PYTHON_PROC*)&py3_Py_ಸಂಖ್ಯೆ_Long},
    {"PyErr_NewException", (PYTHON_PROC*)&py3_PyErr_NewException},
# ifdef Py_DEBUG
    {"_Py_NegativeRefcount", (PYTHON_PROC*)&py3__Py_NegativeRefcount},
    {"_Py_RefTotal", (PYTHON_PROC*)&py3_Py_Ref_ಒಟ್ಟು},
    {"PyModule_Create2TraceRefs", (PYTHON_PROC*)&py3_PyModule_Create2TraceRefs},
# ಇಲ್ಲದಿದ್ದರೆ
    {"PyModule_Create2", (PYTHON_PROC*)&py3_PyModule_Create2},
# endif
# ಆದರೆ defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
    {"_PyObject_DebugFree", (PYTHON_PROC*)&py3__PyObject_DebugFree},
    {"_PyObject_DebugMalloc", (PYTHON_PROC*)&py3__PyObject_DebugMalloc},
# ಇಲ್ಲದಿದ್ದರೆ
    {"PyObject_Malloc", (PYTHON_PROC*)&py3_PyObject_Malloc},
    {"PyObject_Free", (PYTHON_PROC*)&py3_PyObject_Free},
# endif
    {"_PyObject_GC_New", (PYTHON_PROC*)&py3__PyObject_GC_New},
    {"PyObject_GC_Del", (PYTHON_PROC*)&py3_PyObject_GC_Del},
    {"PyObject_GC_UnTrack", (PYTHON_PROC*)&py3_PyObject_GC_UnTrack},
    {"PyType_IsSubtype", (PYTHON_PROC*)&py3_PyType_IsSubtype},
    {"PyCapsule_New", (PYTHON_PROC*)&py3_PyCapsule_New},
    {"PyCapsule_GetPointer", (PYTHON_PROC*)&py3_Py_Capsule_Get_ಸೂಚಕ},
# ifdef USE_LIMITED_API
#  ಆದರೆ PY_ಆವೃತ್ತಿ_HEX >= 0x03040000
    {"PyType_GetSlot", (PYTHON_PROC*)&py3_Py_Type_Get_ಸ್ಥಾನ},
#  endif
    {"PyType_FromSpec", (PYTHON_PROC*)&py3_PyType_FromSpec},
# endif
    {"", ಖಾಲಿ},
};

# ಆದರೆ defined(USE_LIMITED_API) || PY_ಆವೃತ್ತಿ_HEX >= 0x03080000
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

# ಆದರೆ PY_ಆವೃತ್ತಿ_HEX >= 0x030900b0
    ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ
py3_PyType_HasFeature(PyTypeObject *type, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ feature)
{
    ಹಿಂತಿರುಗಿ ((PyType_GetFlags(type) & feature) != 0);
}
#  define PyType_HasFeature(t,f) py3_PyType_HasFeature(t,f)
# endif

# ಆದರೆ PY_ಆವೃತ್ತಿ_HEX >= 0x030a00b2
    ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ
py3__PyObject_TypeCheck(PyObject *ob, PyTypeObject *type)
{
    ಹಿಂತಿರುಗಿ Py_IS_TYPE(ob, type) || PyType_IsSubtype(Py_TYPE(ob), type);
}
#  ಆದರೆ PY_ಆವೃತ್ತಿ_HEX >= 0x030b00b3
#   undef PyObject_TypeCheck
#   define PyObject_TypeCheck(o,t) py3__PyObject_TypeCheck(o,t)
#  ಇಲ್ಲದಿದ್ದರೆ
#   define _PyObject_TypeCheck(o,t) py3__PyObject_TypeCheck(o,t)
#  endif
# endif

# ಆದರೆ !defined(USE_LIMITED_API) && PY_ಆವೃತ್ತಿ_HEX >= 0x030c00b0
// PyTuple_GET_SIZE/PyList_GET_SIZE are inlined functions that use Py_SIZE(),
// which started to introduce linkage dependency from Python 3.12. When we
// build Python in dynamic mode, we don't link against it in build time, and
// this would fail to build. Just use the non-inlined version instead.
#  undef Py_Tuple_GET_ಗಾತ್ರ
#  define Py_Tuple_GET_ಗಾತ್ರ(o) Py_Tuple_ಗಾತ್ರ(o)
#  undef Py_List_GET_ಗಾತ್ರ
#  define Py_List_GET_ಗಾತ್ರ(o) Py_List_ಗಾತ್ರ(o)
# endif

# ifdef MSWIN
/*
 * Look up the library "libname" using the InstallPath registry key.
 * Return NULL when failed.  Return an allocated string when successful.
 */
    ಸ್ಥಿರ WCHAR *
py3_get_system_libname(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *libname)
{
    ಸ್ಥಿರಾಂಕ WCHAR	*pythoncore = L"Software\\Python\\PythonCore";
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ	*cp = libname;
    WCHAR	subkey[128];
    HKEY	h_ಕೀಲಿ;
    ಪೂರ್ಣಾಂಕ		i;
    DWORD	j, len;
    LSTATUS	ret;

    ಆಗ (*cp != '\0')
    {
	ಆದರೆ (*cp == ':' || *cp == '\\' || *cp == '/')
	{
	    // Bail out if "libname" contains path separator, assume it is
	    // an absolute path.
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
	++cp;
    }

    WCHAR   keyfound[32];
    HKEY    h_ಕೀಲಿ_ತುದಿ[] = {HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE};
    HKEY    h_ಕೀಲಿ_ಸಿಕ್ಕಿತು = ಖಾಲಿ;
#  ifdef USE_LIMITED_API
    ದೀರ್ಘ    maxminor = -1;
#  endif
    ಗಾಗಿ (i = 0; i < ARRAY_ಉದ್ದಳತೆ(h_ಕೀಲಿ_ತುದಿ); i++)
    {
	ದೀರ್ಘ	major, minor;

	ret = Reg_Open_ಕೀಲಿ_Ex_W(h_ಕೀಲಿ_ತುದಿ[i], pythoncore, 0, ಕೀಲಿ_READ, &h_ಕೀಲಿ);
	ಆದರೆ (ret != ERROR_SUCCESS)
	    ಮುಂದುವರಿ;
	ಗಾಗಿ (j = 0;; j++)
	{
	    WCHAR   keyname[32];
	    WCHAR   *wp;

	    len = ARRAY_ಉದ್ದಳತೆ(keyname);
	    ret = Reg_Enum_ಕೀಲಿ_Ex_W(h_ಕೀಲಿ, j, keyname, &len,
						    ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
	    ಆದರೆ (ret == ERROR_NO_ಇನ್ನಷ್ಟು_ವಸ್ತುಗಳು)
		ನಿಲ್ಲಿಸು;

	    major = wcstol(keyname, &wp, 10);
	    ಆದರೆ (*wp != L'.')
		ಮುಂದುವರಿ;
	    minor = wcstol(wp + 1, &wp, 10);
#  ifdef _WIN64
	    ಆದರೆ (*wp != L'\0')
		ಮುಂದುವರಿ;
#  ಇಲ್ಲದಿದ್ದರೆ
	    ಆದರೆ (wcscmp(wp, L"-32") != 0)
		ಮುಂದುವರಿ;
#  endif

	    ಆದರೆ (major != PY_MAJOR_ಆವೃತ್ತಿ)
		ಮುಂದುವರಿ;
#  ifdef USE_LIMITED_API
	    // Search the latest version.
	    ಆದರೆ ((minor > maxminor)
		    && (minor >= ((Py_LIMITED_API >> 16) & 0xff)))
	    {
		maxminor = minor;
		wcscpy(keyfound, keyname);
		h_ಕೀಲಿ_ಸಿಕ್ಕಿತು = h_ಕೀಲಿ_ತುದಿ[i];
	    }
#  ಇಲ್ಲದಿದ್ದರೆ
	    // Check if it matches with the compiled version.
	    ಆದರೆ (minor == PY_MINOR_ಆವೃತ್ತಿ)
	    {
		wcscpy(keyfound, keyname);
		h_ಕೀಲಿ_ಸಿಕ್ಕಿತು = h_ಕೀಲಿ_ತುದಿ[i];
		ನಿಲ್ಲಿಸು;
	    }
#  endif
	}
	Reg_Close_ಕೀಲಿ(h_ಕೀಲಿ);
#  ifdef USE_LIMITED_API
	ಆದರೆ (h_ಕೀಲಿ_ಸಿಕ್ಕಿತು != ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;
#  endif
    }
    ಆದರೆ (h_ಕೀಲಿ_ಸಿಕ್ಕಿತು == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    swprintf(subkey, ARRAY_ಉದ್ದಳತೆ(subkey), L"%ls\\%ls\\InstallPath",
							pythoncore, keyfound);
    ret = Reg_Get_ಮೌಲ್ಯ_W(h_ಕೀಲಿ_ಸಿಕ್ಕಿತು, subkey, ಖಾಲಿ, RRF_RT_REG_SZ,
							    ಖಾಲಿ, ಖಾಲಿ, &len);
    ಆದರೆ (ret != ERROR_ಇನ್ನಷ್ಟು_ದತ್ತಾಂಶ && ret != ERROR_SUCCESS)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    size_t len2 = len / ಗಾತ್ರ(WCHAR) + 1 + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(libname);
    WCHAR *ಮಾರ್ಗ = alloc(len2 * ಗಾತ್ರ(WCHAR));
    ಆದರೆ (ಮಾರ್ಗ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ret = Reg_Get_ಮೌಲ್ಯ_W(h_ಕೀಲಿ_ಸಿಕ್ಕಿತು, subkey, ಖಾಲಿ, RRF_RT_REG_SZ,
							    ಖಾಲಿ, ಮಾರ್ಗ, &len);
    ಆದರೆ (ret != ERROR_SUCCESS)
    {
	vim_free(ಮಾರ್ಗ);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    // Remove trailing path separators.
    size_t len3 = wcslen(ಮಾರ್ಗ);
    ಆದರೆ ((len3 > 0) && (ಮಾರ್ಗ[len3 - 1] == L'/' || ಮಾರ್ಗ[len3 - 1] == L'\\'))
	--len3;
    swprintf(ಮಾರ್ಗ + len3, len2 - len3, L"\\%hs", libname);
    ಹಿಂತಿರುಗಿ ಮಾರ್ಗ;
}
# endif

/*
 * Load library and get all pointers.
 * Parameter 'libname' provides name of DLL.
 * Return OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
py3_runtime_link_init(ಅಕ್ಷರ *libname, ಪೂರ್ಣಾಂಕ verbose)
{
    ಪೂರ್ಣಾಂಕ i;
    PYTHON_PROC *ucs_from_string = (PYTHON_PROC *)&py3_PyUnicode_FromString;
    PYTHON_PROC *ucs_decode = (PYTHON_PROC *)&py3_PyUnicode_Decode;
    PYTHON_PROC *ucs_as_encoded_string =
				 (PYTHON_PROC *)&py3_PyUnicode_AsEncodedString;

# ಆದರೆ !(defined(PY_NO_RTLD_GLOBAL) && defined(PY3_NO_RTLD_GLOBAL)) && defined(UNIX) && defined(FEAT_PYTHON)
    // Can't have Python and Python3 loaded at the same time.
    // It causes a crash, because RTLD_GLOBAL is needed for
    // standard C extension libraries of one or both python versions.
    ಆದರೆ (python_loaded())
    {
	ಆದರೆ (verbose)
	    emsg(_(e_this_vim_cannot_execute_py3_after_using_python));
	ಹಿಂತಿರುಗಿ FAIL;
    }
# endif

    ಆದರೆ (hinstPy3 != 0)
	ಹಿಂತಿರುಗಿ OK;
    hinstPy3 = ತುಂಬು_dll(libname);

# ifdef MSWIN
    ಆದರೆ (!hinstPy3)
    {
	// Attempt to use the path from InstallPath as stored in the registry.
	WCHAR *syslibname = py3_get_system_libname(libname);

	ಆದರೆ (syslibname != ಖಾಲಿ)
	{
	    hinstPy3 = ತುಂಬು_Library_Ex_W(syslibname, ಖಾಲಿ,
		    ತುಂಬು_LIBRARY_ಹುಡುಕು_DLL_ತುಂಬು_DIR |
		    ತುಂಬು_LIBRARY_ಹುಡುಕು_SYSTEM32);
	    vim_free(syslibname);
	}
    }
# endif

    ಆದರೆ (!hinstPy3)
    {
	ಆದರೆ (verbose)
	    semsg(_(e_could_not_ತುಂಬು_library_str_str), libname, ತುಂಬು_dll_error());
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಗಾಗಿ (i = 0; py3_funcname_ಕೋಷ್ಟಕ[i].ptr; ++i)
    {
	ಆದರೆ ((*py3_funcname_ಕೋಷ್ಟಕ[i].ptr = ಚಿಹ್ನೆ_from_dll(hinstPy3,
			py3_funcname_ಕೋಷ್ಟಕ[i].name)) == ಖಾಲಿ)
	{
	    close_dll(hinstPy3);
	    hinstPy3 = 0;
	    ಆದರೆ (verbose)
		semsg(_(e_could_not_ತುಂಬು_library_function_str), py3_funcname_ಕೋಷ್ಟಕ[i].name);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
    }

    // Load unicode functions separately as only the ucs2 or the ucs4 functions
    // will be present in the library.
# ಆದರೆ PY_ಆವೃತ್ತಿ_HEX >= 0x030300f0
    *ucs_from_string = ಚಿಹ್ನೆ_from_dll(hinstPy3, "PyUnicode_FromString");
    *ucs_decode = ಚಿಹ್ನೆ_from_dll(hinstPy3, "PyUnicode_Decode");
    *ucs_as_encoded_string = ಚಿಹ್ನೆ_from_dll(hinstPy3,
	    "PyUnicode_AsEncodedString");
# ಇಲ್ಲದಿದ್ದರೆ
    *ucs_from_string = ಚಿಹ್ನೆ_from_dll(hinstPy3, "PyUnicodeUCS2_FromString");
    *ucs_decode = ಚಿಹ್ನೆ_from_dll(hinstPy3,
	    "PyUnicodeUCS2_Decode");
    *ucs_as_encoded_string = ಚಿಹ್ನೆ_from_dll(hinstPy3,
	    "PyUnicodeUCS2_AsEncodedString");
    ಆದರೆ (*ucs_from_string == ಖಾಲಿ || *ucs_decode == ಖಾಲಿ
					     || *ucs_as_encoded_string == ಖಾಲಿ)
    {
	*ucs_from_string = ಚಿಹ್ನೆ_from_dll(hinstPy3,
		"PyUnicodeUCS4_FromString");
	*ucs_decode = ಚಿಹ್ನೆ_from_dll(hinstPy3,
		"PyUnicodeUCS4_Decode");
	*ucs_as_encoded_string = ಚಿಹ್ನೆ_from_dll(hinstPy3,
		"PyUnicodeUCS4_AsEncodedString");
    }
# endif
    ಆದರೆ (*ucs_from_string == ಖಾಲಿ || *ucs_decode == ಖಾಲಿ
					     || *ucs_as_encoded_string == ಖಾಲಿ)
    {
	close_dll(hinstPy3);
	hinstPy3 = 0;
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
python3_enabled(ಪೂರ್ಣಾಂಕ verbose)
{
    ಹಿಂತಿರುಗಿ py3_runtime_link_init((ಅಕ್ಷರ *)p_py3dll, verbose) == OK;
}

/*
 * Load the standard Python exceptions - don't import the symbols from the
 * DLL, as this can cause errors (importing data symbols is not reliable).
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_py3_exceptions(ಶೂನ್ಯ)
{
    PyObject *exmod = PyImport_ImportModule("builtins");
    PyObject *exdict = PyModule_GetDict(exmod);
    p3imp_PyExc_AttributeError = Py_Dict_Get_ಅಂಶ_String(exdict, "AttributeError");
    p3imp_Py_Exc_ಸೂಚ್ಯಂಕ_Error = Py_Dict_Get_ಅಂಶ_String(exdict, "IndexError");
    p3imp_Py_Exc_ಕೀಲಿ_Error = Py_Dict_Get_ಅಂಶ_String(exdict, "KeyError");
    p3imp_Py_Exc_Keyboard_ಅಡಚಣೆ = Py_Dict_Get_ಅಂಶ_String(exdict, "KeyboardInterrupt");
    p3imp_PyExc_TypeError = Py_Dict_Get_ಅಂಶ_String(exdict, "TypeError");
    p3imp_Py_Exc_ಮೌಲ್ಯ_Error = Py_Dict_Get_ಅಂಶ_String(exdict, "ValueError");
    p3imp_PyExc_SystemExit = Py_Dict_Get_ಅಂಶ_String(exdict, "SystemExit");
    p3imp_PyExc_RuntimeError = Py_Dict_Get_ಅಂಶ_String(exdict, "RuntimeError");
    p3imp_PyExc_ImportError = Py_Dict_Get_ಅಂಶ_String(exdict, "ImportError");
    p3imp_Py_Exc_ಅತಿಪ್ರವಾಹ_Error = Py_Dict_Get_ಅಂಶ_String(exdict, "OverflowError");
    Py_XINCREF(p3imp_PyExc_AttributeError);
    Py_XINCREF(p3imp_Py_Exc_ಸೂಚ್ಯಂಕ_Error);
    Py_XINCREF(p3imp_Py_Exc_ಕೀಲಿ_Error);
    Py_XINCREF(p3imp_Py_Exc_Keyboard_ಅಡಚಣೆ);
    Py_XINCREF(p3imp_PyExc_TypeError);
    Py_XINCREF(p3imp_Py_Exc_ಮೌಲ್ಯ_Error);
    Py_XINCREF(p3imp_PyExc_SystemExit);
    Py_XINCREF(p3imp_PyExc_RuntimeError);
    Py_XINCREF(p3imp_PyExc_ImportError);
    Py_XINCREF(p3imp_Py_Exc_ಅತಿಪ್ರವಾಹ_Error);
    Py_XDECREF(exmod);
}
#ಆದರೆ_ಅಂತ್ಯ // DYNAMIC_PYTHON3

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ py3initialised = 0;
#ವ್ಯಾಖ್ಯಾನಿಸು PYINITIALISED py3initialised
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ python_ಕೊನೆ_called = FALSE;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_LIMITED_API
# define DESTRUCTOR_ಮುಕ್ತಾಯ(self) \
    ((freefunc)Py_Type_Get_ಸ್ಥಾನ(Py_TYPE((PyObject*)self), Py_tp_free))((PyObject*)self)
#ಇಲ್ಲದಿದ್ದರೆ
# define DESTRUCTOR_ಮುಕ್ತಾಯ(self) Py_TYPE(self)->tp_free((PyObject*)self)
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು WIN_PYTHON_REF(win) win->w_python3_ref
#ವ್ಯಾಖ್ಯಾನಿಸು BUF_PYTHON_REF(buf) buf->b_python3_ref
#ವ್ಯಾಖ್ಯಾನಿಸು TAB_PYTHON_REF(tab) tab->tp_python3_ref

    ಸ್ಥಿರ ಶೂನ್ಯ
call_PyObject_Free(ಶೂನ್ಯ *p)
{
#ಆದರೆ defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
    _PyObject_DebugFree(p);
#ಇಲ್ಲದಿದ್ದರೆ
    PyObject_Free(p);
#ಆದರೆ_ಅಂತ್ಯ
}

    ಸ್ಥಿರ PyObject *
call_PyType_GenericNew(PyTypeObject *type, PyObject *ವಾದಗಳು, PyObject *kwds)
{
    ಹಿಂತಿರುಗಿ PyType_GenericNew(type,ವಾದಗಳು,kwds);
}

    ಸ್ಥಿರ PyObject *
call_PyType_GenericAlloc(PyTypeObject *type, Py_ssize_t nitems)
{
    ಹಿಂತಿರುಗಿ PyType_GenericAlloc(type,nitems);
}

ಸ್ಥಿರ PyObject *OutputGetattro(PyObject *, PyObject *);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ OutputSetattro(PyObject *, PyObject *, PyObject *);
ಸ್ಥಿರ PyObject *ಬಫರ್_Getattro(PyObject *, PyObject *);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಬಫರ್_Setattro(PyObject *, PyObject *, PyObject *);
ಸ್ಥಿರ PyObject *Tab_ಪುಟ_Getattro(PyObject *, PyObject *);
ಸ್ಥಿರ PyObject *ಕಿಟಕಿ_Getattro(PyObject *, PyObject *);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಕಿಟಕಿ_Setattro(PyObject *, PyObject *, PyObject *);
ಸ್ಥಿರ PyObject *RangeGetattro(PyObject *, PyObject *);
ಸ್ಥಿರ PyObject *CurrentGetattro(PyObject *, PyObject *);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ CurrentSetattro(PyObject *, PyObject *, PyObject *);
ಸ್ಥಿರ PyObject *DictionaryGetattro(PyObject *, PyObject *);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ DictionarySetattro(PyObject *, PyObject *, PyObject *);
ಸ್ಥಿರ PyObject *ListGetattro(PyObject *, PyObject *);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ListSetattro(PyObject *, PyObject *, PyObject *);
ಸ್ಥಿರ PyObject *TupleGetattro(PyObject *, PyObject *);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ TupleSetattro(PyObject *, PyObject *, PyObject *);
ಸ್ಥಿರ PyObject *FunctionGetattro(PyObject *, PyObject *);

ಸ್ಥಿರ ರಚನೆ PyModuleDef vimmodule;

#ವ್ಯಾಖ್ಯಾನಿಸು PY_CAN_RECURSE

/*
 * Include the code shared with if_python.c
 */
#ಅನ್ತರ್ಭಾವ "if_py_both.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_LIMITED_API
# ಆದರೆ Py_LIMITED_API >= 0x030A0000
#  define PY_UNICODE_GET_UTF8_CHARS(obj) Py_Unicode_As_UTF8_And_ಗಾತ್ರ(obj, ಖಾಲಿ)
# ಇಲ್ಲದಿದ್ದರೆ
// Python limited API before 3.10 lack easy ways to query the raw UTF-8 chars.
// We need to first convert the string to bytes, and then extract the chars.
// This function is only used for attribute string comparisons, which have
// known short length. As such, just allocate a short static buffer to hold
// the characters instead of having to allocate/deallcoate it.
//
// An alternative would be to convert all attribute string comparisons to use
// PyUnicode_CompareWithASCIIString to skip having to extract the chars.
ಸ್ಥಿರ ಅಕ್ಷರ py3_unicode_utf8_chars[20];
ಸ್ಥಿರ ಅಕ್ಷರ* PY_UNICODE_GET_UTF8_CHARS(PyObject* str)
{
    py3_unicode_utf8_chars[0] = '\0';
    PyObject* ಬೈಟುಗಳು = PyUnicode_AsUTF8String(str);
    ಆದರೆ (ಬೈಟುಗಳು)
    {
	ಅಕ್ಷರ *chars;
	Py_ssize_t len;
	ಆದರೆ (Py_ಬೈಟುಗಳು_As_String_And_ಗಾತ್ರ(ಬೈಟುಗಳು, &chars, &len) != -1)
	{
	    ಆದರೆ (len < (Py_ssize_t)ಗಾತ್ರ(py3_unicode_utf8_chars))
		// PyBytes_AsStringAndSize guarantees null-termination
		ಸ್ಮರಣೆ_ನಕಲು(py3_unicode_utf8_chars, chars, len + 1);
	}
	Py_DECREF(ಬೈಟುಗಳು);
    }
    ಹಿಂತಿರುಗಿ py3_unicode_utf8_chars;
}
# endif
#ಇಲ್ಲದಿದ್ದರೆ	// !USE_LIMITED_API
# ಆದರೆ PY_ಆವೃತ್ತಿ_HEX >= 0x030300f0
#  define PY_UNICODE_GET_UTF8_CHARS(obj) Py_Unicode_As_UTF8_And_ಗಾತ್ರ(obj, ಖಾಲಿ)
# ಇಲ್ಲದಿದ್ದರೆ
#  define PY_UNICODE_GET_UTF8_CHARS _PyUnicode_AsString
# endif
#ಆದರೆ_ಅಂತ್ಯ

// NOTE: Must always be used at the start of a block, since it declares "name".
#ವ್ಯಾಖ್ಯಾನಿಸು GET_ATTR_STRING(name, nameobj) \
    ಅಕ್ಷರ	*name = ""; \
    ಆದರೆ (PyUnicode_Check(nameobj)) \
	name = (ಅಕ್ಷರ *)PY_UNICODE_GET_UTF8_CHARS(nameobj)

#ವ್ಯಾಖ್ಯಾನಿಸು PY3OBJ_DELETED(obj) (obj->ob_ತಳ.ob_refcnt<=0)

///////////////////////////////////////////////////////
// Internal function prototypes.

ಸ್ಥಿರ PyObject *Py3Init_vim(ಶೂನ್ಯ);

///////////////////////////////////////////////////////
// 1. Python interpreter main program.

    ಶೂನ್ಯ
python3_ಕೊನೆ(ಶೂನ್ಯ)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ recurse = 0;

    // If a crash occurs while doing this, don't try again.
    ಆದರೆ (recurse != 0)
	ಹಿಂತಿರುಗಿ;

    python_ಕೊನೆ_called = TRUE;
    ++recurse;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DYNAMIC_PYTHON3
    ಆದರೆ (hinstPy3)
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (Py_IsInitialized())
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_LIMITED_API
	shutdown_types();
#ಆದರೆ_ಅಂತ್ಯ

	// acquire lock before finalizing
	PyGILState_Ensure();

	Py_Finalize();
    }

    --recurse;
}

#ಆದರೆ defined(DYNAMIC_PYTHON3) && defined(DYNAMIC_PYTHON) && defined(FEAT_PYTHON) && defined(UNIX)
    ಪೂರ್ಣಾಂಕ
python3_loaded(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (hinstPy3 != 0);
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ wchar_t *py_home_buf = ಖಾಲಿ;

#ಆದರೆ defined(MSWIN) && (PY_ಆವೃತ್ತಿ_HEX >= 0x030500f0)
/*
 * Return TRUE if stdin is readable from Python 3.
 */
    ಸ್ಥಿರ BOOL
is_stdin_readable(ಶೂನ್ಯ)
{
    DWORD	    ಬಹುಲಕ, eventnum;
    ರಚನೆ _stat    st;
    ಪೂರ್ಣಾಂಕ		    fd = fileno(stdin);
    HANDLE	    hstdin = (HANDLE)_get_osfhandle(fd);

    // Check if stdin is connected to the console.
    ಆದರೆ (Get_Console_ಬಹುಲಕ(hstdin, &ಬಹುಲಕ))
	// Check if it is opened as input.
	ಹಿಂತಿರುಗಿ Get_ಸಂಖ್ಯೆ_ದ_Console_ಒಳಹರಿವು_Events(hstdin, &eventnum);

    ಹಿಂತಿರುಗಿ _fstat(fd, &st) == 0;
}

// Python 3.5 or later will abort inside Py_Initialize() when stdin has
// been closed (i.e. executed by "vim -").  Reconnect stdin to CONIN$.
// Note that the python DLL is linked to its own stdio DLL which can be
// differ from Vim's stdio.
    ಸ್ಥಿರ ಶೂನ್ಯ
reset_stdin(ಶೂನ್ಯ)
{
    FILE *(*py__acrt_iob_func)(ಚಿಹ್ನೆರಹಿತ) = ಖಾಲಿ;
    FILE *(*pyfreopen)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *, FILE *) = ಖಾಲಿ;
    HINSTANCE hinst = get_forwarded_dll(hinstPy3);

    ಆದರೆ (hinst == ಖಾಲಿ || is_stdin_readable())
	ಹಿಂತಿರುಗಿ;

    // Get "freopen" and "stdin" which are used in the python DLL.
    // "stdin" is defined as "__acrt_iob_func(0)" in VC++ 2015 or later.
    py__acrt_iob_func = get_dll_import_func(hinst, "__acrt_iob_func");
    ಆದರೆ (py__acrt_iob_func)
    {
	HINSTANCE hpystdiodll = ಕಂಡುಹಿಡಿ_imported_module_by_funcname(hinst,
							    "__acrt_iob_func");
	ಆದರೆ (hpystdiodll)
	    pyfreopen = (ಶೂನ್ಯ *)Get_Proc_ವಿಳಾಸ(hpystdiodll, "freopen");
    }

    // Reconnect stdin to CONIN$.
    ಆದರೆ (pyfreopen != ಖಾಲಿ)
	pyfreopen("CONIN$", "r", py__acrt_iob_func(0));
    ಇಲ್ಲದಿದ್ದರೆ
	freopen("CONIN$", "r", stdin);
}
#ಇಲ್ಲದಿದ್ದರೆ
# define reset_stdin()
#ಆದರೆ_ಅಂತ್ಯ

// Python 3.2 or later will abort inside Py_Initialize() when mandatory
// modules cannot be loaded (e.g. 'pythonthreehome' is wrongly set.).
// Install a hook to python dll's exit() and recover from it.
#ಆದರೆ defined(MSWIN) && (PY_ಆವೃತ್ತಿ_HEX >= 0x030200f0)
# define HOOK_EXIT
# include <setjmp.h>

ಸ್ಥಿರ jmp_buf exit_hook_jump_buf;
ಸ್ಥಿರ ಶೂನ್ಯ *orig_exit = ಖಾಲಿ;

/*
 * Function that replaces exit() while calling Py_Initialize().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
hooked_exit(ಪೂರ್ಣಾಂಕ ret)
{
    // Recover from exit.
    longjmp(exit_hook_jump_buf, 1);
}

/*
 * Install a hook to python dll's exit().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
hook_py_exit(ಶೂನ್ಯ)
{
    HINSTANCE hinst = get_forwarded_dll(hinstPy3);

    ಆದರೆ (hinst == ಖಾಲಿ || orig_exit != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    orig_exit = hook_dll_import_func(hinst, "exit", (ಶೂನ್ಯ *)hooked_exit);
}

/*
 * Remove the hook installed by hook_py_exit().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
restore_py_exit(ಶೂನ್ಯ)
{
    HINSTANCE hinst = hinstPy3;

    ಆದರೆ (hinst == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (orig_exit != ಖಾಲಿ)
	hook_dll_import_func(hinst, "exit", orig_exit);
    orig_exit = ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
Python3_Init(ಶೂನ್ಯ)
{
    ಆದರೆ (!py3initialised)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DYNAMIC_PYTHON3
	ಆದರೆ (!python3_enabled(TRUE))
	{
	    emsg(_(e_sorry_this_command_is_disabled_python_library_could_not_be_ಸಿಕ್ಕಿತು));
	    ಹೋಗು fail;
	}
#ಆದರೆ_ಅಂತ್ಯ
	// Python 3.13 introduced a really useful feature: colorized exceptions.
	// This is great if you're reading them from the terminal, but useless
	// and broken everywhere else (such as in log files, or text editors).
	// Opt out, forcefully.
	vim_setenv((char_u*)"PYTHON_COLORS", (char_u*)"0");

	init_structs();

	ಆದರೆ (*p_py3home != NUL)
	{
	    size_t len = mbstowcs(ಖಾಲಿ, (ಅಕ್ಷರ *)p_py3home, 0) + 1;

	    // The string must not change later, make a copy in static memory.
	    py_home_buf = ALLOC_MULT(wchar_t, len);
	    ಆದರೆ (py_home_buf != ಖಾಲಿ && mbstowcs(
			    py_home_buf, (ಅಕ್ಷರ *)p_py3home, len) != (size_t)-1)
		Py_ಗಣ_Python_Home(py_home_buf);
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ PYTHON3_HOME
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (mch_getenv((char_u *)"PYTHONHOME") == ಖಾಲಿ)
	    Py_ಗಣ_Python_Home(PYTHON3_HOME);
#ಆದರೆ_ಅಂತ್ಯ

	PyImport_AppendInittab("vim", Py3Init_vim);

#ಆದರೆ !defined(DYNAMIC_PYTHON3) && defined(MSWIN)
	hinstPy3 = GetModuleHandle(PYTHON3_DLL);
#ಆದರೆ_ಅಂತ್ಯ
	reset_stdin();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HOOK_EXIT
	// Catch exit() called in Py_Initialize().
	hook_py_exit();
	ಆದರೆ (setjmp(exit_hook_jump_buf) == 0)
	{
	    Py_Initialize();
	    restore_py_exit();
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // exit() was called in Py_Initialize().
	    restore_py_exit();
	    emsg(_(e_critical_error_ಒಳ_python3_initialization_check_your_installation));
	    ಹೋಗು fail;
	}
#ಇಲ್ಲದಿದ್ದರೆ
	Py_Initialize();
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ PY_ಆವೃತ್ತಿ_HEX < 0x03090000
	// Initialise threads.  This is deprecated since Python 3.9.
	Py_Eval_Init_ಎಳೆಗಳು();
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DYNAMIC_PYTHON3
	get_py3_exceptions();
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ (PythonIO_Init_io())
	    ಹೋಗು fail;

	globals = PyModule_GetDict(Py_Import_ಕೂಡಿಸು_Module("__main__"));

	// Remove the element from sys.path that was added because of our
	// argv[0] value in Py3Init_vim().  Previously we used an empty
	// string, but depending on the OS we then get an empty entry or
	// the current directory in sys.path.
	// Only after vim has been imported, the element does exist in
	// sys.path.
	Py_ಸತತ_Simple_String("import vim; import sys; sys.path = list(filter(lambda x: not x.endswith('must>not&exist'), sys.path))");

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
DoPyCommand(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *cmd,
	    dict_T* locals,
	    rangeinitializer init_range,
	    runner ಸತತ,
	    ಶೂನ್ಯ *arg)
{
#ಆದರೆ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    ಅಕ್ಷರ		*saved_locale;
#ಆದರೆ_ಅಂತ್ಯ
    PyObject		*cmdstr;
    PyObject		*cmdbytes;
    PyGILState_STATE	pygilstate;

    ಆದರೆ (python_ಕೊನೆ_called)
	ಹೋಗು theend;

    ಆದರೆ (Python3_Init())
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
	saved_locale = (ಅಕ್ಷರ *)vim_strsave((char_u *)saved_locale);
	(ಶೂನ್ಯ)setlocale(LC_NUMERIC, "C");
    }
#ಆದರೆ_ಅಂತ್ಯ

    pygilstate = PyGILState_Ensure();

    // PyRun_SimpleString expects a UTF-8 string. Wrong encoding may cause
    // SyntaxError (unicode error).
    cmdstr = PyUnicode_Decode(cmd, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(cmd),
					(ಅಕ್ಷರ *)ENC_OPT, ERRORS_DECODE_ARG);
    cmdbytes = PyUnicode_AsEncodedString(cmdstr, "utf-8", ERRORS_ENCODE_ARG);
    Py_XDECREF(cmdstr);

    ಸತತ(Py_ಬೈಟುಗಳು_As_String(cmdbytes), locals, arg, &pygilstate);
    Py_XDECREF(cmdbytes);

    PyGILState_Release(pygilstate);

#ಆದರೆ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    ಆದರೆ (saved_locale != ಖಾಲಿ)
    {
	(ಶೂನ್ಯ)setlocale(LC_NUMERIC, saved_locale);
	vim_free(saved_locale);
    }
#ಆದರೆ_ಅಂತ್ಯ

    Python_ಬೀಗ_Vim();		    // enter Vim
    PythonIO_Flush();

theend:
    ಹಿಂತಿರುಗಿ;	    // keeps lint happy
}

/*
 * ":py3"
 */
    ಶೂನ್ಯ
ex_py3(exarg_T *eap)
{
    char_u *script;

    script = script_get(eap, eap->arg);
    ಆದರೆ (!eap->skip)
    {
	ಆದರೆ (p_pyx == 0)
	    p_pyx = 3;

	DoPyCommand(script == ಖಾಲಿ ? (ಅಕ್ಷರ *) eap->arg : (ಅಕ್ಷರ *) script,
		ಖಾಲಿ,
		init_range_cmd,
		(runner) ಸತತ_cmd,
		(ಶೂನ್ಯ *) eap);
    }
    vim_free(script);
}

#ವ್ಯಾಖ್ಯಾನಿಸು ಬಫರ್_ಗಾತ್ರ 2048

/*
 * ":py3file"
 */
    ಶೂನ್ಯ
ex_py3file(exarg_T *eap)
{
    ಸ್ಥಿರ ಅಕ್ಷರ ಬಫರ್[ಬಫರ್_ಗಾತ್ರ];
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file;
    ಅಕ್ಷರ *p;
    ಪೂರ್ಣಾಂಕ i;

    ಆದರೆ (p_pyx == 0)
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

    strcpy(ಬಫರ್, "exec(compile(open('");
    p = ಬಫರ್ + 19; // size of "exec(compile(open('"

    ಗಾಗಿ (i=0; i<2; ++i)
    {
	file = (ಅಕ್ಷರ *)eap->arg;
	ಆಗ (*file && p < ಬಫರ್ + (ಬಫರ್_ಗಾತ್ರ - 3))
	{
	    ಆದರೆ (*file == '\\' || *file == '\'')
		*p++ = '\\';
	    *p++ = *file++;
	}
	// If we didn't finish the file name, we hit a buffer overflow
	ಆದರೆ (*file != '\0')
	    ಹಿಂತಿರುಗಿ;
	ಆದರೆ (i==0)
	{
	    strcpy(p,"','rb').read(),'");
	    p += 16;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    strcpy(p,"','exec'))");
	    p += 10;
	}
    }


    // Execute the file
    DoPyCommand(ಬಫರ್,
	    ಖಾಲಿ,
	    init_range_cmd,
	    (runner) ಸತತ_cmd,
	    (ಶೂನ್ಯ *) eap);
}

    ಶೂನ್ಯ
ex_py3do(exarg_T *eap)
{
    ಆದರೆ (p_pyx == 0)
	p_pyx = 3;

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
OutputGetattro(PyObject *self, PyObject *nameobj)
{
    GET_ATTR_STRING(name, nameobj);

    ಆದರೆ (strcmp(name, "softspace") == 0)
	ಹಿಂತಿರುಗಿ PyLong_FromLong(((OutputObject *)(self))->softspace);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(name, "errors") == 0)
	ಹಿಂತಿರುಗಿ PyString_FromString("strict");
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(name, "encoding") == 0)
	ಹಿಂತಿರುಗಿ PyString_FromString(ENC_OPT);

    ಹಿಂತಿರುಗಿ PyObject_GenericGetAttr(self, nameobj);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
OutputSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);

    ಹಿಂತಿರುಗಿ OutputSetattr(self, name, val);
}

///////////////////////////////////////////////////////
// 3. Implementation of the Vim module for Python

// Window type - Implementation functions
// --------------------------------------

#ವ್ಯಾಖ್ಯಾನಿಸು ಕಿಟಕಿ_Type_Check(obj) ((obj)->ob_ತಳ.ob_type == &ಕಿಟಕಿ_Type)

// Buffer type - Implementation functions
// --------------------------------------

#ವ್ಯಾಖ್ಯಾನಿಸು ಬಫರ್_Type_Check(obj) ((obj)->ob_ತಳ.ob_type == &ಬಫರ್_Type)

ಸ್ಥಿರ PyObject* ಬಫರ್_Subscript(PyObject *self, PyObject *idx);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಬಫರ್_As_Subscript(PyObject *self, PyObject *idx, PyObject *val);

// Line range type - Implementation functions
// --------------------------------------

#ವ್ಯಾಖ್ಯಾನಿಸು RangeType_Check(obj) ((obj)->ob_ತಳ.ob_type == &RangeType)

ಸ್ಥಿರ PyObject* RangeSubscript(PyObject *self, PyObject *idx);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ Range_As_ಅಂಶ(PyObject *, Py_ssize_t, PyObject *);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ RangeAsSubscript(PyObject *self, PyObject *idx, PyObject *val);

// Current objects type - Implementation functions
// -----------------------------------------------

ಸ್ಥಿರ PySequenceMethods ಬಫರ್_As_Seq = {
    (lenfunc)		ಬಫರ್_ಉದ್ದಳತೆ,	    // sq_length,    len(x)
    (binaryfunc)	0,		    // sq_concat,    x+y
    (ssizeargfunc)	0,		    // sq_repeat,    x*n
    (ssizeargfunc)	ಬಫರ್_ಅಂಶ,	    // sq_item,      x[i]
    0,					    // was_sq_slice,	 x[i:j]
    0,					    // sq_ass_item,  x[i]=v
    0,					    // sq_ass_slice, x[i:j]=v
    0,					    // sq_contains
    0,					    // sq_inplace_concat
    0,					    // sq_inplace_repeat
};

ಸ್ಥಿರ PyMappingMethods ಬಫರ್_As_Mapping = {
    /* mp_length	*/ (lenfunc)ಬಫರ್_ಉದ್ದಳತೆ,
    /* mp_subscript     */ (binaryfunc)ಬಫರ್_Subscript,
    /* mp_ass_subscript */ (objobjargproc)ಬಫರ್_As_Subscript,
};


// Buffer object

    ಸ್ಥಿರ PyObject *
ಬಫರ್_Getattro(PyObject *self, PyObject *nameobj)
{
    PyObject *r;

    GET_ATTR_STRING(name, nameobj);

    ಆದರೆ ((r = ಬಫರ್_Attr_Valid((ಬಫರ್_Object *)(self), name)))
	ಹಿಂತಿರುಗಿ r;

    ಆದರೆ (Check_ಬಫರ್((ಬಫರ್_Object *)(self)))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    r = ಬಫರ್_Attr((ಬಫರ್_Object *)(self), name);
    ಆದರೆ (r || PyErr_Occurred())
	ಹಿಂತಿರುಗಿ r;
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ PyObject_GenericGetAttr(self, nameobj);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಬಫರ್_Setattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);

    ಹಿಂತಿರುಗಿ ಬಫರ್_Setattr(self, name, val);
}

//////////////////

    ಸ್ಥಿರ PyObject *
ಬಫರ್_Subscript(PyObject *self, PyObject* idx)
{
    ಆದರೆ (PyLong_Check(idx))
    {
	ದೀರ್ಘ _idx = PyLong_AsLong(idx);
	ಹಿಂತಿರುಗಿ ಬಫರ್_ಅಂಶ((ಬಫರ್_Object *)(self), _idx);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (Py_ತುಂಡು_Check(idx))
    {
	Py_ssize_t ಆರಂಭ, stop, ಹೆಜ್ಜೆ, slicelen;

	ಆದರೆ (Check_ಬಫರ್((ಬಫರ್_Object *) self))
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	ಆದರೆ (Py_ತುಂಡು_Get_Indices_Ex((Py_ತುಂಡು_Object_T *)idx,
	      (Py_ssize_t)((ಬಫರ್_Object *)(self))->buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ,
	      &ಆರಂಭ, &stop,
	      &ಹೆಜ್ಜೆ, &slicelen) < 0)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	ಹಿಂತಿರುಗಿ ಬಫರ್_ತುಂಡು((ಬಫರ್_Object *)(self), ಆರಂಭ, stop);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	RAISE_INVALID_ಸೂಚ್ಯಂಕ_TYPE(idx);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಬಫರ್_As_Subscript(PyObject *self, PyObject* idx, PyObject* val)
{
    ಆದರೆ (PyLong_Check(idx))
    {
	ದೀರ್ಘ n = PyLong_AsLong(idx);

	ಆದರೆ (Check_ಬಫರ್((ಬಫರ್_Object *) self))
	    ಹಿಂತಿರುಗಿ -1;

	ಹಿಂತಿರುಗಿ RBAs_ಅಂಶ((ಬಫರ್_Object *)(self), n, val, 1,
		    (Py_ssize_t)((ಬಫರ್_Object *)(self))->buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ,
		    ಖಾಲಿ);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (Py_ತುಂಡು_Check(idx))
    {
	Py_ssize_t ಆರಂಭ, stop, ಹೆಜ್ಜೆ, slicelen;

	ಆದರೆ (Check_ಬಫರ್((ಬಫರ್_Object *) self))
	    ಹಿಂತಿರುಗಿ -1;

	ಆದರೆ (Py_ತುಂಡು_Get_Indices_Ex((Py_ತುಂಡು_Object_T *)idx,
	      (Py_ssize_t)((ಬಫರ್_Object *)(self))->buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ,
	      &ಆರಂಭ, &stop,
	      &ಹೆಜ್ಜೆ, &slicelen) < 0)
	    ಹಿಂತಿರುಗಿ -1;
	ಹಿಂತಿರುಗಿ RBAs_ತುಂಡು((ಬಫರ್_Object *)(self), ಆರಂಭ, stop, val, 1,
			  (PyInt)((ಬಫರ್_Object *)(self))->buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ,
			  ಖಾಲಿ);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	RAISE_INVALID_ಸೂಚ್ಯಂಕ_TYPE(idx);
	ಹಿಂತಿರುಗಿ -1;
    }
}

ಸ್ಥಿರ PySequenceMethods RangeAsSeq = {
    (lenfunc)		Range_ಉದ್ದಳತೆ,	 // sq_length,	  len(x)
    (binaryfunc)	0,		 // RangeConcat, sq_concat,  x+y
    (ssizeargfunc)	0,		 // RangeRepeat, sq_repeat,  x*n
    (ssizeargfunc)	Range_ಅಂಶ,	 // sq_item,	  x[i]
    0,					 // was_sq_slice,     x[i:j]
    (ssizeobjargproc)	Range_As_ಅಂಶ,	 // sq_as_item,  x[i]=v
    0,					 // sq_ass_slice, x[i:j]=v
    0,					 // sq_contains
    0,					 // sq_inplace_concat
    0,					 // sq_inplace_repeat
};

ಸ್ಥಿರ PyMappingMethods RangeAsMapping = {
    /* mp_length	*/ (lenfunc)Range_ಉದ್ದಳತೆ,
    /* mp_subscript     */ (binaryfunc)RangeSubscript,
    /* mp_ass_subscript */ (objobjargproc)RangeAsSubscript,
};

// Line range object - Implementation

    ಸ್ಥಿರ PyObject *
RangeGetattro(PyObject *self, PyObject *nameobj)
{
    GET_ATTR_STRING(name, nameobj);

    ಆದರೆ (strcmp(name, "start") == 0)
	ಹಿಂತಿರುಗಿ Py_Build_ಮೌಲ್ಯ("n", ((RangeObject *)(self))->ಆರಂಭ - 1);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(name, "end") == 0)
	ಹಿಂತಿರುಗಿ Py_Build_ಮೌಲ್ಯ("n", ((RangeObject *)(self))->ಕೊನೆ - 1);
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ PyObject_GenericGetAttr(self, nameobj);
}

////////////////

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
Range_As_ಅಂಶ(PyObject *self, Py_ssize_t n, PyObject *val)
{
    ಹಿಂತಿರುಗಿ RBAs_ಅಂಶ(((RangeObject *)(self))->buf, n, val,
		    ((RangeObject *)(self))->ಆರಂಭ,
		    ((RangeObject *)(self))->ಕೊನೆ,
		    &((RangeObject *)(self))->ಕೊನೆ);
}

    ಸ್ಥಿರ Py_ssize_t
Range_As_ತುಂಡು(PyObject *self, Py_ssize_t lo, Py_ssize_t hi, PyObject *val)
{
    ಹಿಂತಿರುಗಿ RBAs_ತುಂಡು(((RangeObject *)(self))->buf, lo, hi, val,
		    ((RangeObject *)(self))->ಆರಂಭ,
		    ((RangeObject *)(self))->ಕೊನೆ,
		    &((RangeObject *)(self))->ಕೊನೆ);
}

    ಸ್ಥಿರ PyObject *
RangeSubscript(PyObject *self, PyObject* idx)
{
    ಆದರೆ (PyLong_Check(idx))
    {
	ದೀರ್ಘ _idx = PyLong_AsLong(idx);
	ಹಿಂತಿರುಗಿ Range_ಅಂಶ((RangeObject *)(self), _idx);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (Py_ತುಂಡು_Check(idx))
    {
	Py_ssize_t ಆರಂಭ, stop, ಹೆಜ್ಜೆ, slicelen;

	ಆದರೆ (Py_ತುಂಡು_Get_Indices_Ex((Py_ತುಂಡು_Object_T *)idx,
		((RangeObject *)(self))->ಕೊನೆ-((RangeObject *)(self))->ಆರಂಭ+1,
		&ಆರಂಭ, &stop,
		&ಹೆಜ್ಜೆ, &slicelen) < 0)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	ಹಿಂತಿರುಗಿ Range_ತುಂಡು((RangeObject *)(self), ಆರಂಭ, stop);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	RAISE_INVALID_ಸೂಚ್ಯಂಕ_TYPE(idx);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
RangeAsSubscript(PyObject *self, PyObject *idx, PyObject *val)
{
    ಆದರೆ (PyLong_Check(idx))
    {
	ದೀರ್ಘ n = PyLong_AsLong(idx);
	ಹಿಂತಿರುಗಿ Range_As_ಅಂಶ(self, n, val);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (Py_ತುಂಡು_Check(idx))
    {
	Py_ssize_t ಆರಂಭ, stop, ಹೆಜ್ಜೆ, slicelen;

	ಆದರೆ (Py_ತುಂಡು_Get_Indices_Ex((Py_ತುಂಡು_Object_T *)idx,
		((RangeObject *)(self))->ಕೊನೆ-((RangeObject *)(self))->ಆರಂಭ+1,
		&ಆರಂಭ, &stop,
		&ಹೆಜ್ಜೆ, &slicelen) < 0)
	    ಹಿಂತಿರುಗಿ -1;
	ಹಿಂತಿರುಗಿ Range_As_ತುಂಡು(self, ಆರಂಭ, stop, val);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	RAISE_INVALID_ಸೂಚ್ಯಂಕ_TYPE(idx);
	ಹಿಂತಿರುಗಿ -1;
    }
}

// TabPage object - Implementation

    ಸ್ಥಿರ PyObject *
Tab_ಪುಟ_Getattro(PyObject *self, PyObject *nameobj)
{
    PyObject *r;

    GET_ATTR_STRING(name, nameobj);

    ಆದರೆ ((r = Tab_ಪುಟ_Attr_Valid((Tab_ಪುಟ_Object *)(self), name)))
	ಹಿಂತಿರುಗಿ r;

    ಆದರೆ (Check_Tab_ಪುಟ((Tab_ಪುಟ_Object *)(self)))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    r = Tab_ಪುಟ_Attr((Tab_ಪುಟ_Object *)(self), name);
    ಆದರೆ (r || PyErr_Occurred())
	ಹಿಂತಿರುಗಿ r;
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ PyObject_GenericGetAttr(self, nameobj);
}

// Window object - Implementation

    ಸ್ಥಿರ PyObject *
ಕಿಟಕಿ_Getattro(PyObject *self, PyObject *nameobj)
{
    PyObject *r;

    GET_ATTR_STRING(name, nameobj);

    ಆದರೆ ((r = ಕಿಟಕಿ_Attr_Valid((ಕಿಟಕಿ_Object *)(self), name)))
	ಹಿಂತಿರುಗಿ r;

    ಆದರೆ (Check_ಕಿಟಕಿ((ಕಿಟಕಿ_Object *)(self)))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    r = ಕಿಟಕಿ_Attr((ಕಿಟಕಿ_Object *)(self), name);
    ಆದರೆ (r || PyErr_Occurred())
	ಹಿಂತಿರುಗಿ r;
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ PyObject_GenericGetAttr(self, nameobj);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಕಿಟಕಿ_Setattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);

    ಹಿಂತಿರುಗಿ ಕಿಟಕಿ_Setattr(self, name, val);
}

// Tab page list object - Definitions

ಸ್ಥಿರ PySequenceMethods TabListAsSeq = {
    (lenfunc)	     Tab_List_ಉದ್ದಳತೆ,	    // sq_length,    len(x)
    (binaryfunc)     0,			    // sq_concat,    x+y
    (ssizeargfunc)   0,			    // sq_repeat,    x*n
    (ssizeargfunc)   Tab_List_ಅಂಶ,	    // sq_item,      x[i]
    0,					    // sq_slice,     x[i:j]
    (ssizeobjargproc)0,			    // sq_as_item,  x[i]=v
    0,					    // sq_ass_slice, x[i:j]=v
    0,					    // sq_contains
    0,					    // sq_inplace_concat
    0,					    // sq_inplace_repeat
};

// Window list object - Definitions

ಸ್ಥಿರ PySequenceMethods WinListAsSeq = {
    (lenfunc)	     Win_List_ಉದ್ದಳತೆ,	    // sq_length,    len(x)
    (binaryfunc)     0,			    // sq_concat,    x+y
    (ssizeargfunc)   0,			    // sq_repeat,    x*n
    (ssizeargfunc)   Win_List_ಅಂಶ,	    // sq_item,      x[i]
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
    ಸ್ಥಿರ PyObject *
CurrentGetattro(PyObject *self, PyObject *nameobj)
{
    PyObject	*r;
    GET_ATTR_STRING(name, nameobj);
    ಆದರೆ (!(r = CurrentGetattr(self, name)))
	ಹಿಂತಿರುಗಿ PyObject_GenericGetAttr(self, nameobj);
    ಹಿಂತಿರುಗಿ r;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
CurrentSetattro(PyObject *self, PyObject *nameobj, PyObject *ಮೌಲ್ಯ)
{
    GET_ATTR_STRING(name, nameobj);
    ಹಿಂತಿರುಗಿ CurrentSetattr(self, name, ಮೌಲ್ಯ);
}

// Dictionary object - Definitions

    ಸ್ಥಿರ PyObject *
DictionaryGetattro(PyObject *self, PyObject *nameobj)
{
    DictionaryObject	*this = ((DictionaryObject *) (self));

    GET_ATTR_STRING(name, nameobj);

    ಆದರೆ (strcmp(name, "locked") == 0)
	ಹಿಂತಿರುಗಿ PyLong_FromLong(this->dict->dv_ಬೀಗ);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(name, "scope") == 0)
	ಹಿಂತಿರುಗಿ PyLong_FromLong(this->dict->dv_scope);

    ಹಿಂತಿರುಗಿ PyObject_GenericGetAttr(self, nameobj);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
DictionarySetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);
    ಹಿಂತಿರುಗಿ DictionarySetattr(self, name, val);
}

// List object - Definitions

    ಸ್ಥಿರ PyObject *
ListGetattro(PyObject *self, PyObject *nameobj)
{
    GET_ATTR_STRING(name, nameobj);

    ಆದರೆ (strcmp(name, "locked") == 0)
	ಹಿಂತಿರುಗಿ PyLong_FromLong(((ListObject *) (self))->list->lv_ಬೀಗ);

    ಹಿಂತಿರುಗಿ PyObject_GenericGetAttr(self, nameobj);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ListSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);
    ಹಿಂತಿರುಗಿ ListSetattr(self, name, val);
}

// Tuple object - Definitions

    ಸ್ಥಿರ PyObject *
TupleGetattro(PyObject *self, PyObject *nameobj)
{
    GET_ATTR_STRING(name, nameobj);

    ಆದರೆ (strcmp(name, "locked") == 0)
	ಹಿಂತಿರುಗಿ PyLong_FromLong(((TupleObject *) (self))->tuple->tv_ಬೀಗ);

    ಹಿಂತಿರುಗಿ PyObject_GenericGetAttr(self, nameobj);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
TupleSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);
    ಹಿಂತಿರುಗಿ TupleSetattr(self, name, val);
}

// Function object - Definitions

    ಸ್ಥಿರ PyObject *
FunctionGetattro(PyObject *self, PyObject *nameobj)
{
    PyObject		*r;
    FunctionObject	*this = (FunctionObject *)(self);

    GET_ATTR_STRING(name, nameobj);

    r = FunctionAttr(this, name);
    ಆದರೆ (r || PyErr_Occurred())
	ಹಿಂತಿರುಗಿ r;
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ PyObject_GenericGetAttr(self, nameobj);
}

// External interface

    ಶೂನ್ಯ
python3_ಬಫರ್_free(buf_T *buf)
{
    ಬಫರ್_Object *bp = BUF_PYTHON_REF(buf);
    ಆದರೆ (bp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    bp->buf = INVALID_ಬಫರ್_ಮೌಲ್ಯ;
    BUF_PYTHON_REF(buf) = ಖಾಲಿ;
}

    ಶೂನ್ಯ
python3_ಕಿಟಕಿ_free(win_T *win)
{
    ಕಿಟಕಿ_Object *wp = WIN_PYTHON_REF(win);
    ಆದರೆ (wp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    wp->win = INVALID_ಕಿಟಕಿ_ಮೌಲ್ಯ;
    WIN_PYTHON_REF(win) = ಖಾಲಿ;
}

    ಶೂನ್ಯ
python3_tabpage_free(tabpage_T *tab)
{
    Tab_ಪುಟ_Object *tp = TAB_PYTHON_REF(tab);
    ಆದರೆ (tp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    tp->tab = INVALID_TABPAGE_ಮೌಲ್ಯ;
    TAB_PYTHON_REF(tab) = ಖಾಲಿ;
}

    ಸ್ಥಿರ PyObject *
Py3Init_vim(ಶೂನ್ಯ)
{
    // The special value is removed from sys.path in Python3_Init().
    ಸ್ಥಿರ wchar_t *(argv[2]) = {L"/must>not&exist/foo", ಖಾಲಿ};

    ಆದರೆ (init_types())
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // Set sys.argv[] to avoid a crash in warn().
    Py_Sys_ಗಣ_Argv(1, argv);

    ಆದರೆ ((vim_module = PyModule_Create(&vimmodule)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (populate_module(vim_module))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (init_sys_ಮಾರ್ಗ())
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಹಿಂತಿರುಗಿ vim_module;
}

//////////////////////////////////////////////////////////////////////////
// 4. Utility functions for handling the interface between Vim and Python.

/*
 * Convert a Vim line into a Python string.
 * All internal newlines are replaced by null characters.
 *
 * On errors, the Python exception data is set, and NULL is returned.
 */
    ಸ್ಥಿರ PyObject *
ಸಾಲು_ಗಮ್ಯ_String(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str)
{
    PyObject *ಫಲಿತಾಂಶ;
    Py_ssize_t len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(str);
    ಅಕ್ಷರ *tmp, *p;

    tmp = alloc(len + 1);
    p = tmp;
    ಆದರೆ (p == ಖಾಲಿ)
    {
	Py_Err_No_ಸ್ಮರಣೆ();
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಆಗ (*str)
    {
	ಆದರೆ (*str == '\n')
	    *p = '\0';
	ಇಲ್ಲದಿದ್ದರೆ
	    *p = *str;

	++p;
	++str;
    }
    *p = '\0';

    ಫಲಿತಾಂಶ = PyUnicode_Decode(tmp, len, (ಅಕ್ಷರ *)ENC_OPT, ERRORS_DECODE_ARG);

    vim_free(tmp);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

    ಶೂನ್ಯ
do_py3eval(char_u *str, dict_T *locals, typval_T *rettv)
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
ಗಣ_ref_ಒಳ_python3(ಪೂರ್ಣಾಂಕ copyID)
{
    ಹಿಂತಿರುಗಿ ಗಣ_ref_ಒಳ_py(copyID);
}

    ಪೂರ್ಣಾಂಕ
python3_ಆವೃತ್ತಿ(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_LIMITED_API
    ಹಿಂತಿರುಗಿ Py_LIMITED_API;
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ PY_ಆವೃತ್ತಿ_HEX;
#ಆದರೆ_ಅಂತ್ಯ
}
