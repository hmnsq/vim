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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <limits.h>

#ਜੇ defined(MSWIN) && defined(HAVE_FCNTL_H)
# undef HAVE_FCNTL_H
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _DEBUG
# undef _DEBUG
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ F_BLANK
# undef F_BLANK
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_DUP
# undef HAVE_DUP
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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _POSIX_C_SOURCE  // defined in feature.h
# undef _POSIX_C_SOURCE
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ XOPEN_ਸਰੋਤ
# undef XOPEN_ਸਰੋਤ	// pyconfig.h defines it as well.
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PY_SSIZE_T_CLEAN

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ Py_LIMITED_API
# define USE_LIMITED_API // Using Python 3 limited ABI
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ <Python.h>

#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ ਮੁੱਖ // Defined in python.h - aargh
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ HAVE_FCNTL_H // Clash with os_win32.h

// The "surrogateescape" error handler is new in Python 3.1
#ਜੇ PY_ਸੰਸਕਰਣ_HEX >= 0x030100f0
# define CODEC_ERROR_HANDLER "surrogateescape"
#ਨਹੀਂਤਾਂ
# define CODEC_ERROR_HANDLER ਨਿਰਾ
#ਜੇ_ਅੰਤ

// Suppress Python 3.11 depreciations to see useful warnings
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __GNUC__
# pragma GCC diagnostic ਧੱਕੋ
# pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#ਜੇ_ਅੰਤ

// Python 3 does not support CObjects, always use Capsules
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PY_USE_CAPSULE

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ERRORS_DECODE_ARG CODEC_ERROR_HANDLER
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ERRORS_ENCODE_ARG ERRORS_DECODE_ARG

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PyInt Py_ssize_t
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ PyString_Check
# define PyString_Check(obj) PyUnicode_Check(obj)
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PyString_FromString(repr) \
    PyUnicode_Decode(repr, STRLEN(repr), ENC_OPT, ERRORS_DECODE_ARG)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PyString_FromFormat PyUnicode_FromFormat
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ PyUnicode_FromFormat
# define Py_UNICODE_USE_UCS_FUNCTIONS
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ PyInt_Check
# define PyInt_Check(obj) PyLong_Check(obj)
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PyInt_FromLong(i) PyLong_FromLong(i)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PyInt_AsLong(obj) PyLong_AsLong(obj)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Py_ssize_t_fmt "n"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Py_ਬਾਈਟਾਂ_fmt "y"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PyIntArgFunc	ssizeargfunc
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PyIntObjArgProc	ssizeobjargproc

/*
 * PySlice_GetIndicesEx(): first argument type changed from PySliceObject
 * to PyObject in Python 3.2 or later.
 */
#ਜੇ PY_ਸੰਸਕਰਣ_HEX >= 0x030200f0
ਕਿਸਮਪਰਿਭਾਸ਼ਾ PyObject Py_ਟੁਕੜਾ_Object_T;
#ਨਹੀਂਤਾਂ
ਕਿਸਮਪਰਿਭਾਸ਼ਾ Py_ਟੁਕੜਾ_Object Py_ਟੁਕੜਾ_Object_T;
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ MSWIN
# define HINSTANCE ਖਾਲੀ *
#ਜੇ_ਅੰਤ
#ਜੇ defined(DYNAMIC_PYTHON3) || defined(MSWIN)
ਸਥਿਰ HINSTANCE hinstPy3 = 0; // Instance of python.dll
#ਜੇ_ਅੰਤ

#ਜੇ defined(DYNAMIC_PYTHON3)

# ifdef MSWIN
#  define ਲੱਦੋ_dll vim_ਲੱਦੋ_Lib
#  define close_dll FreeLibrary
#  define ਚਿੰਨ੍ਹ_from_dll Get_Proc_ਪਤਾ
#  define ਲੱਦੋ_dll_error GetWin32Error
# ਨਹੀਂਤਾਂ
#  include <dlfcn.h>
#  define FARPROC ਖਾਲੀ*
#  ਜੇ defined(PY_NO_RTLD_GLOBAL) && defined(PY3_NO_RTLD_GLOBAL)
#   define ਲੱਦੋ_dll(n) dlopen((n), RTLD_LAZY)
#  ਨਹੀਂਤਾਂ
#   define ਲੱਦੋ_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#  endif
#  define close_dll dlclose
#  define ਚਿੰਨ੍ਹ_from_dll dlsym
#  define ਲੱਦੋ_dll_error dlerror
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
# define Py_Dict_ਸੈੱਟ_ਵਸਤੂ_String py3_Py_Dict_ਸੈੱਟ_ਵਸਤੂ_String
# define Py_Err_Bad_ਦਲੀਲ py3_Py_Err_Bad_ਦਲੀਲ
# define PyErr_Clear py3_PyErr_Clear
# define PyErr_Format py3_PyErr_Format
# define PyErr_PrintEx py3_PyErr_PrintEx
# define Py_Err_No_ਸਮ੍ਰਿਤੀ py3_Py_Err_No_ਸਮ੍ਰਿਤੀ
# define PyErr_Occurred py3_PyErr_Occurred
# define Py_Err_ਸੈੱਟ_None py3_Py_Err_ਸੈੱਟ_None
# define Py_Err_ਸੈੱਟ_String py3_Py_Err_ਸੈੱਟ_String
# define Py_Err_ਸੈੱਟ_Object py3_Py_Err_ਸੈੱਟ_Object
# define PyErr_ExceptionMatches py3_PyErr_ExceptionMatches
# define Py_Eval_Init_ਧਾਗੇ py3_Py_Eval_Init_ਧਾਗੇ
# define PyEval_RestoreThread py3_PyEval_RestoreThread
# define PyEval_SaveThread py3_PyEval_SaveThread
# define PyGILState_Ensure py3_PyGILState_Ensure
# define PyGILState_Release py3_PyGILState_Release
# define PyLong_AsLong py3_PyLong_AsLong
# define PyLong_FromLong py3_PyLong_FromLong
# define Py_List_Get_ਵਸਤੂ py3_Py_List_Get_ਵਸਤੂ
# define PyList_Append py3_PyList_Append
# define Py_List_ਪਾਓ py3_Py_List_ਪਾਓ
# define PyList_New py3_PyList_New
# define Py_List_ਸੈੱਟ_ਵਸਤੂ_2 py3_Py_List_ਸੈੱਟ_ਵਸਤੂ
# define Py_List_ਮਾਪ py3_Py_List_ਮਾਪ
# define PyTuple_New py3_PyTuple_New
# define Py_Tuple_Get_ਵਸਤੂ py3_Py_Tuple_Get_ਵਸਤੂ
# define Py_Tuple_ਸੈੱਟ_ਵਸਤੂ py3_Py_Tuple_ਸੈੱਟ_ਵਸਤੂ
# define Py_Tuple_ਮਾਪ py3_Py_Tuple_ਮਾਪ
# define PySequence_Check py3_PySequence_Check
# define Py_Sequence_ਮਾਪ py3_Py_Sequence_ਮਾਪ
# define Py_Sequence_Get_ਵਸਤੂ py3_Py_Sequence_Get_ਵਸਤੂ
# define PySequence_Fast py3_PySequence_Fast
# ਜੇ PY_ਸੰਸਕਰਣ_HEX >= 0x030601f0
#  define Py_ਟੁਕੜਾ_Adjust_Indices py3_Py_ਟੁਕੜਾ_Adjust_Indices
#  define Py_ਟੁਕੜਾ_Unpack py3_Py_ਟੁਕੜਾ_Unpack
# endif
# undef Py_ਟੁਕੜਾ_Get_Indices_Ex
# define Py_ਟੁਕੜਾ_Get_Indices_Ex py3_Py_ਟੁਕੜਾ_Get_Indices_Ex
# define PyImport_ImportModule py3_PyImport_ImportModule
# define PyObject_Init py3__PyObject_Init
# define PyDict_New py3_PyDict_New
# define Py_Dict_Get_ਵਸਤੂ_String py3_Py_Dict_Get_ਵਸਤੂ_String
# define Py_Dict_ਅਗਲਾ py3_Py_Dict_ਅਗਲਾ
# define PyMapping_Check py3_PyMapping_Check
# ifndef Py_Mapping_ਕੁੰਜੀਆਂ
#  define Py_Mapping_ਕੁੰਜੀਆਂ py3_Py_Mapping_ਕੁੰਜੀਆਂ
# endif
# ਜੇ (defined(USE_LIMITED_API) && Py_LIMITED_API >= 0x03080000) || \
       (!defined(USE_LIMITED_API) && PY_ਸੰਸਕਰਣ_HEX >= 0x03080000)
#  undef PyIter_Check
#  define PyIter_Check py3_PyIter_Check
# endif
# define Py_Iter_ਅਗਲਾ py3_Py_Iter_ਅਗਲਾ
# define PyObject_GetIter py3_PyObject_GetIter
# define PyObject_Repr py3_PyObject_Repr
# define Py_Object_Get_ਵਸਤੂ py3_Py_Object_Get_ਵਸਤੂ
# define PyObject_IsTrue py3_PyObject_IsTrue
# define PyModule_GetDict py3_PyModule_GetDict
# ਜੇ defined(USE_LIMITED_API) || PY_ਸੰਸਕਰਣ_HEX >= 0x03080000
#  define Py_IncRef py3_Py_IncRef
#  define Py_DecRef py3_Py_DecRef
# endif
# ifdef USE_LIMITED_API
#  define Py_CompileString py3_Py_CompileString
#  define Py_Eval_Eval_ਕੋਡ py3_Py_Eval_Eval_ਕੋਡ
# ਨਹੀਂਤਾਂ
#  undef Py_ਲੜੀਵਾਰ_Simple_String
#  define Py_ਲੜੀਵਾਰ_Simple_String py3_Py_ਲੜੀਵਾਰ_Simple_String
#  undef Py_ਲੜੀਵਾਰ_String
#  define Py_ਲੜੀਵਾਰ_String py3_Py_ਲੜੀਵਾਰ_String
# endif
# define PyObject_GetAttrString py3_PyObject_GetAttrString
# define PyObject_HasAttrString py3_PyObject_HasAttrString
# define Py_Object_ਸੈੱਟ_Attr_String py3_Py_Object_ਸੈੱਟ_Attr_String
# define Py_Object_Call_Function_Obj_ਦਲੀਲਾਂ py3_Py_Object_Call_Function_Obj_ਦਲੀਲਾਂ
# ਜੇ PY_ਸੰਸਕਰਣ_HEX >= 0x030d0000
#  define PyObject_CallFunction py3_PyObject_CallFunction
# ਨਹੀਂਤਾਂ
#  define Py_Object_Call_Function_ਮਾਪ_T py3_Py_Object_Call_Function_ਮਾਪ_T
# endif
# define PyObject_Call py3_PyObject_Call
# define PyEval_GetLocals py3_PyEval_GetLocals
# define PyEval_GetGlobals py3_PyEval_GetGlobals
# define Py_Sys_ਸੈੱਟ_Object py3_Py_Sys_ਸੈੱਟ_Object
# define PySys_GetObject py3_PySys_GetObject
# define Py_Sys_ਸੈੱਟ_Argv py3_Py_Sys_ਸੈੱਟ_Argv
# define PyType_Ready py3_PyType_Ready
# ਜੇ PY_ਸੰਸਕਰਣ_HEX >= 0x03040000
#  define PyType_GetFlags py3_PyType_GetFlags
# endif
# undef Py_Build_ਮੁੱਲ
# define Py_Build_ਮੁੱਲ py3_Py_Build_ਮੁੱਲ
# define Py_ਸੈੱਟ_Python_Home py3_Py_ਸੈੱਟ_Python_Home
# define Py_Initialize py3_Py_Initialize
# define Py_Finalize py3_Py_Finalize
# define Py_IsInitialized py3_Py_IsInitialized
# define _Py_NoneStruct (*py3__Py_NoneStruct)
# define _Py_FalseStruct (*py3__Py_FalseStruct)
# define _Py_TrueStruct (*py3__Py_TrueStruct)
# ਜੇ !defined(USE_LIMITED_API) && PY_ਸੰਸਕਰਣ_HEX < 0x030D0000
// Private symbol that used to be required as part of PyIter_Check.
#  define Py_Object_ਅਗਲਾ_Not_Implemented (*py3_Py_Object_ਅਗਲਾ_Not_Implemented)
# endif
# define Py_Module_ਜੋੜੋ_Object py3_Py_Module_ਜੋੜੋ_Object
# define PyImport_AppendInittab py3_PyImport_AppendInittab
# define Py_Import_ਜੋੜੋ_Module py3_Py_Import_ਜੋੜੋ_Module
# ifdef USE_LIMITED_API
#  ਜੇ Py_LIMITED_API >= 0x030a0000
#   define Py_Unicode_As_UTF8_And_ਮਾਪ py3_Py_Unicode_As_UTF8_And_ਮਾਪ
#  endif
# ਨਹੀਂਤਾਂ
#  ਜੇ PY_ਸੰਸਕਰਣ_HEX >= 0x030300f0
#   define Py_Unicode_As_UTF8_And_ਮਾਪ py3_Py_Unicode_As_UTF8_And_ਮਾਪ
#  ਨਹੀਂਤਾਂ
#   define _PyUnicode_AsString py3__PyUnicode_AsString
#  endif
# endif
# undef PyUnicode_CompareWithASCIIString
# define PyUnicode_CompareWithASCIIString py3_PyUnicode_CompareWithASCIIString
# undef PyUnicode_AsEncodedString
# define PyUnicode_AsEncodedString py3_PyUnicode_AsEncodedString
# undef PyUnicode_AsUTF8String
# define PyUnicode_AsUTF8String py3_PyUnicode_AsUTF8String
# undef Py_ਬਾਈਟਾਂ_As_String
# define Py_ਬਾਈਟਾਂ_As_String py3_Py_ਬਾਈਟਾਂ_As_String
# ifndef Py_ਬਾਈਟਾਂ_As_String_And_ਮਾਪ
#  define Py_ਬਾਈਟਾਂ_As_String_And_ਮਾਪ py3_Py_ਬਾਈਟਾਂ_As_String_And_ਮਾਪ
# endif
# undef Py_ਬਾਈਟਾਂ_From_String
# define Py_ਬਾਈਟਾਂ_From_String py3_Py_ਬਾਈਟਾਂ_From_String
# undef Py_ਬਾਈਟਾਂ_From_String_And_ਮਾਪ
# define Py_ਬਾਈਟਾਂ_From_String_And_ਮਾਪ py3_Py_ਬਾਈਟਾਂ_From_String_And_ਮਾਪ
# define PyFloat_FromDouble py3_PyFloat_FromDouble
# define PyFloat_AsDouble py3_PyFloat_AsDouble
# define PyObject_GenericGetAttr py3_PyObject_GenericGetAttr
# define PyType_Type (*py3_PyType_Type)
# ifndef USE_LIMITED_API
#  define PyStdPrinter_Type (*py3_PyStdPrinter_Type)
# endif
# define Py_ਟੁਕੜਾ_Type (*py3_Py_ਟੁਕੜਾ_Type)
# define PyFloat_Type (*py3_PyFloat_Type)
# define Py_ਸੰਖਿਆ_Check (*py3_Py_ਸੰਖਿਆ_Check)
# define Py_ਸੰਖਿਆ_Long (*py3_Py_ਸੰਖਿਆ_Long)
# define PyBool_Type (*py3_PyBool_Type)
# define PyErr_NewException py3_PyErr_NewException
# ifdef Py_DEBUG
#  define _Py_NegativeRefcount py3__Py_NegativeRefcount
#  define Py_Ref_ਕੁੱਲ (*py3_Py_Ref_ਕੁੱਲ)
#  define PyModule_Create2TraceRefs py3_PyModule_Create2TraceRefs
# ਨਹੀਂਤਾਂ
#  define PyModule_Create2 py3_PyModule_Create2
# endif
# ਜੇ defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
#  define _PyObject_DebugMalloc py3__PyObject_DebugMalloc
#  define _PyObject_DebugFree py3__PyObject_DebugFree
# ਨਹੀਂਤਾਂ
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
#  ਨਹੀਂਤਾਂ
#   define PyUnicodeUCS2_FromFormat py3_PyUnicodeUCS2_FromFormat
#  endif
# ਨਹੀਂਤਾਂ
#  define PyUnicode_FromFormat py3_PyUnicode_FromFormat
# endif
# undef PyUnicode_Decode
# define PyUnicode_Decode py3_PyUnicode_Decode
# define PyType_IsSubtype py3_PyType_IsSubtype
# define PyCapsule_New py3_PyCapsule_New
# define Py_Capsule_Get_ਸੂਚਕ py3_Py_Capsule_Get_ਸੂਚਕ
# ifdef USE_LIMITED_API
#  define Py_Type_Get_ਥਾਂ py3_Py_Type_Get_ਥਾਂ
#  define PyType_FromSpec py3_PyType_FromSpec
# endif

# ਜੇ defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
#  undef PyObject_NEW
#  define PyObject_NEW(type, typeobj) \
( (type *) PyObject_Init( \
	(PyObject *) _PyObject_DebugMalloc( Py_Object_ਮਾਪ(typeobj) ), (typeobj)) )
# elif PY_ਸੰਸਕਰਣ_HEX >= 0x030900b0
#  undef PyObject_NEW
#  define PyObject_NEW(type, typeobj) \
	((type *)py3__PyObject_New(typeobj))
# endif

/*
 * Pointers for dynamic link
 */
ਸਥਿਰ ਪੂਰਨ (*py3_Py_Sys_ਸੈੱਟ_Argv)(ਪੂਰਨ, wchar_t **);
ਸਥਿਰ ਖਾਲੀ (*py3_Py_ਸੈੱਟ_Python_Home)(wchar_t *home);
ਸਥਿਰ ਖਾਲੀ (*py3_Py_Initialize)(ਖਾਲੀ);
ਸਥਿਰ PyObject* (*py3_PyList_New)(Py_ssize_t ਮਾਪ);
ਸਥਿਰ PyGILState_STATE (*py3_PyGILState_Ensure)(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ (*py3_PyGILState_Release)(PyGILState_STATE);
ਸਥਿਰ ਪੂਰਨ (*py3_Py_Sys_ਸੈੱਟ_Object)(ਅੱਖਰ *, PyObject *);
ਸਥਿਰ PyObject* (*py3_PySys_GetObject)(ਅੱਖਰ *);
ਸਥਿਰ ਪੂਰਨ (*py3_PyList_Append)(PyObject *, PyObject *);
ਸਥਿਰ ਪੂਰਨ (*py3_Py_List_ਪਾਓ)(PyObject *, ਪੂਰਨ, PyObject *);
ਸਥਿਰ Py_ssize_t (*py3_Py_List_ਮਾਪ)(PyObject *);
ਸਥਿਰ PyObject* (*py3_PyTuple_New)(Py_ssize_t ਮਾਪ);
ਸਥਿਰ ਪੂਰਨ (*py3_Py_Tuple_ਸੈੱਟ_ਵਸਤੂ)(PyObject *, Py_ssize_t, PyObject *);
ਸਥਿਰ Py_ssize_t (*py3_Py_Tuple_ਮਾਪ)(PyObject *);
ਸਥਿਰ ਪੂਰਨ (*py3_PySequence_Check)(PyObject *);
ਸਥਿਰ Py_ssize_t (*py3_Py_Sequence_ਮਾਪ)(PyObject *);
ਸਥਿਰ PyObject* (*py3_Py_Sequence_Get_ਵਸਤੂ)(PyObject *, Py_ssize_t);
ਸਥਿਰ PyObject* (*py3_PySequence_Fast)(PyObject *, ਅਚਲ ਅੱਖਰ *);
ਸਥਿਰ PyObject* (*py3_Py_Tuple_Get_ਵਸਤੂ)(PyObject *, Py_ssize_t);
ਸਥਿਰ ਪੂਰਨ (*py3_PyMapping_Check)(PyObject *);
ਸਥਿਰ PyObject* (*py3_Py_Mapping_ਕੁੰਜੀਆਂ)(PyObject *);
# ਜੇ PY_ਸੰਸਕਰਣ_HEX >= 0x030601f0
ਸਥਿਰ ਪੂਰਨ (*py3_Py_ਟੁਕੜਾ_Adjust_Indices)(Py_ssize_t ਲੰਮਾਈ,
		     Py_ssize_t *ਸ਼ੁਰੂ, Py_ssize_t *stop, Py_ssize_t ਕਦਮ);
ਸਥਿਰ ਪੂਰਨ (*py3_Py_ਟੁਕੜਾ_Unpack)(PyObject *ਟੁਕੜਾ,
		     Py_ssize_t *ਸ਼ੁਰੂ, Py_ssize_t *stop, Py_ssize_t *ਕਦਮ);
# endif
ਸਥਿਰ ਪੂਰਨ (*py3_Py_ਟੁਕੜਾ_Get_Indices_Ex)(Py_ਟੁਕੜਾ_Object_T *r, Py_ssize_t ਲੰਮਾਈ,
		     Py_ssize_t *ਸ਼ੁਰੂ, Py_ssize_t *stop, Py_ssize_t *ਕਦਮ,
		     Py_ssize_t *slicelen);
ਸਥਿਰ PyObject* (*py3_Py_Err_No_ਸਮ੍ਰਿਤੀ)(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ (*py3_Py_Finalize)(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ (*py3_Py_Err_ਸੈੱਟ_String)(PyObject *, ਅਚਲ ਅੱਖਰ *);
ਸਥਿਰ ਖਾਲੀ (*py3_Py_Err_ਸੈੱਟ_Object)(PyObject *, PyObject *);
ਸਥਿਰ ਪੂਰਨ (*py3_PyErr_ExceptionMatches)(PyObject *);
# ਜੇ defined(USE_LIMITED_API) || PY_ਸੰਸਕਰਣ_HEX >= 0x03080000
ਸਥਿਰ ਖਾਲੀ (*py3_Py_IncRef)(PyObject *);
ਸਥਿਰ ਖਾਲੀ (*py3_Py_DecRef)(PyObject *);
# endif
# ifdef USE_LIMITED_API
ਸਥਿਰ PyObject* (*py3_Py_CompileString)(ਅਚਲ ਅੱਖਰ *, ਅਚਲ ਅੱਖਰ *, ਪੂਰਨ);
ਸਥਿਰ PyObject* (*py3_Py_Eval_Eval_ਕੋਡ)(PyObject *co, PyObject *globals, PyObject *locals);
# ਨਹੀਂਤਾਂ
ਸਥਿਰ ਪੂਰਨ (*py3_Py_ਲੜੀਵਾਰ_Simple_String)(ਅੱਖਰ *);
ਸਥਿਰ PyObject* (*py3_Py_ਲੜੀਵਾਰ_String)(ਅੱਖਰ *, ਪੂਰਨ, PyObject *, PyObject *);
# endif
ਸਥਿਰ PyObject* (*py3_PyObject_GetAttrString)(PyObject *, ਅਚਲ ਅੱਖਰ *);
ਸਥਿਰ ਪੂਰਨ (*py3_PyObject_HasAttrString)(PyObject *, ਅਚਲ ਅੱਖਰ *);
ਸਥਿਰ ਪੂਰਨ (*py3_Py_Object_ਸੈੱਟ_Attr_String)(PyObject *, ਅਚਲ ਅੱਖਰ *, PyObject *);
ਸਥਿਰ PyObject* (*py3_Py_Object_Call_Function_Obj_ਦਲੀਲਾਂ)(PyObject *, ...);
# ਜੇ PY_ਸੰਸਕਰਣ_HEX >= 0x030d0000
ਸਥਿਰ PyObject* (*py3_PyObject_CallFunction)(PyObject *, ਅੱਖਰ *, ...);
# ਨਹੀਂਤਾਂ
ਸਥਿਰ PyObject* (*py3_Py_Object_Call_Function_ਮਾਪ_T)(PyObject *, ਅੱਖਰ *, ...);
# endif
ਸਥਿਰ PyObject* (*py3_PyObject_Call)(PyObject *, PyObject *, PyObject *);
ਸਥਿਰ PyObject* (*py3_PyEval_GetGlobals)(ਖਾਲੀ);
ਸਥਿਰ PyObject* (*py3_PyEval_GetLocals)(ਖਾਲੀ);
ਸਥਿਰ PyObject* (*py3_Py_List_Get_ਵਸਤੂ)(PyObject *, Py_ssize_t);
ਸਥਿਰ PyObject* (*py3_PyImport_ImportModule)(ਅਚਲ ਅੱਖਰ *);
ਸਥਿਰ PyObject* (*py3_Py_Import_ਜੋੜੋ_Module)(ਅਚਲ ਅੱਖਰ *);
ਸਥਿਰ ਪੂਰਨ (*py3_Py_Err_Bad_ਦਲੀਲ)(ਖਾਲੀ);
ਸਥਿਰ PyObject* (*py3_PyErr_Occurred)(ਖਾਲੀ);
ਸਥਿਰ PyObject* (*py3_PyModule_GetDict)(PyObject *);
ਸਥਿਰ ਪੂਰਨ (*py3_Py_List_ਸੈੱਟ_ਵਸਤੂ)(PyObject *, Py_ssize_t, PyObject *);
ਸਥਿਰ PyObject* (*py3_Py_Dict_Get_ਵਸਤੂ_String)(PyObject *, ਅਚਲ ਅੱਖਰ *);
ਸਥਿਰ ਪੂਰਨ (*py3_Py_Dict_ਅਗਲਾ)(PyObject *, Py_ssize_t *, PyObject **, PyObject **);
ਸਥਿਰ PyObject* (*py3_PyLong_FromLong)(ਲੰਮਾ);
ਸਥਿਰ PyObject* (*py3_PyDict_New)(ਖਾਲੀ);
# ਜੇ (defined(USE_LIMITED_API) && Py_LIMITED_API >= 0x03080000) || \
       (!defined(USE_LIMITED_API) && PY_ਸੰਸਕਰਣ_HEX >= 0x03080000)
ਸਥਿਰ ਪੂਰਨ (*py3_PyIter_Check)(PyObject *o);
# endif
ਸਥਿਰ PyObject* (*py3_Py_Iter_ਅਗਲਾ)(PyObject *);
ਸਥਿਰ PyObject* (*py3_PyObject_GetIter)(PyObject *);
ਸਥਿਰ PyObject* (*py3_PyObject_Repr)(PyObject *);
ਸਥਿਰ PyObject* (*py3_Py_Object_Get_ਵਸਤੂ)(PyObject *, PyObject *);
ਸਥਿਰ ਪੂਰਨ (*py3_PyObject_IsTrue)(PyObject *);
ਸਥਿਰ PyObject* (*py3_Py_Build_ਮੁੱਲ)(ਅੱਖਰ *, ...);
# ਜੇ PY_ਸੰਸਕਰਣ_HEX >= 0x03040000
ਸਥਿਰ ਪੂਰਨ (*py3_PyType_GetFlags)(PyTypeObject *o);
# endif
ਸਥਿਰ ਪੂਰਨ (*py3_PyType_Ready)(PyTypeObject *type);
ਸਥਿਰ ਪੂਰਨ (*py3_Py_Dict_ਸੈੱਟ_ਵਸਤੂ_String)(PyObject *dp, ਅੱਖਰ *ਕੁੰਜੀ, PyObject *ਵਸਤੂ);
ਸਥਿਰ PyObject* (*py3_PyUnicode_FromString)(ਅਚਲ ਅੱਖਰ *u);
# ifdef Py_UNICODE_USE_UCS_FUNCTIONS
#  ifdef Py_UNICODE_WIDE
ਸਥਿਰ PyObject* (*py3_PyUnicodeUCS4_FromFormat)(ਅਚਲ ਅੱਖਰ *u, ...);
#  ਨਹੀਂਤਾਂ
ਸਥਿਰ PyObject* (*py3_PyUnicodeUCS2_FromFormat)(ਅਚਲ ਅੱਖਰ *u, ...);
#  endif
# ਨਹੀਂਤਾਂ
ਸਥਿਰ PyObject* (*py3_PyUnicode_FromFormat)(ਅਚਲ ਅੱਖਰ *u, ...);
# endif
ਸਥਿਰ PyObject* (*py3_PyUnicode_Decode)(ਅਚਲ ਅੱਖਰ *u, Py_ssize_t ਮਾਪ,
	ਅਚਲ ਅੱਖਰ *encoding, ਅਚਲ ਅੱਖਰ *errors);
ਸਥਿਰ ਲੰਮਾ (*py3_PyLong_AsLong)(PyObject *);
ਸਥਿਰ ਖਾਲੀ (*py3_Py_Err_ਸੈੱਟ_None)(PyObject *);
ਸਥਿਰ ਖਾਲੀ (*py3_Py_Eval_Init_ਧਾਗੇ)(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ(*py3_PyEval_RestoreThread)(PyThreadState *);
ਸਥਿਰ PyThreadState*(*py3_PyEval_SaveThread)(ਖਾਲੀ);
ਸਥਿਰ ਪੂਰਨ (*py3_PyArg_Parse)(PyObject *, ਅੱਖਰ *, ...);
ਸਥਿਰ ਪੂਰਨ (*py3_PyArg_ParseTuple)(PyObject *, ਅੱਖਰ *, ...);
ਸਥਿਰ ਖਾਲੀ (*py3_PyMem_Free)(ਖਾਲੀ *);
ਸਥਿਰ ਖਾਲੀ* (*py3_PyMem_Malloc)(size_t);
ਸਥਿਰ ਪੂਰਨ (*py3_Py_IsInitialized)(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ (*py3_PyErr_Clear)(ਖਾਲੀ);
ਸਥਿਰ PyObject* (*py3_PyErr_Format)(PyObject *, ਅਚਲ ਅੱਖਰ *, ...);
ਸਥਿਰ ਖਾਲੀ (*py3_PyErr_PrintEx)(ਪੂਰਨ);
ਸਥਿਰ PyObject*(*py3__PyObject_Init)(PyObject *, PyTypeObject *);
# ਜੇ !defined(USE_LIMITED_API) && PY_ਸੰਸਕਰਣ_HEX < 0x030D0000
ਸਥਿਰ iternextfunc py3_Py_Object_ਅਗਲਾ_Not_Implemented;
# endif
ਸਥਿਰ PyObject* py3__Py_NoneStruct;
ਸਥਿਰ PyObject* py3__Py_FalseStruct;
ਸਥਿਰ PyObject* py3__Py_TrueStruct;
ਸਥਿਰ ਪੂਰਨ (*py3_Py_Module_ਜੋੜੋ_Object)(PyObject *m, ਅਚਲ ਅੱਖਰ *name, PyObject *o);
ਸਥਿਰ ਪੂਰਨ (*py3_PyImport_AppendInittab)(ਅਚਲ ਅੱਖਰ *name, PyObject* (*initfunc)(ਖਾਲੀ));
# ifdef USE_LIMITED_API
#  ਜੇ Py_LIMITED_API >= 0x030a0000
ਸਥਿਰ ਅੱਖਰ* (*py3_Py_Unicode_As_UTF8_And_ਮਾਪ)(PyObject *unicode, Py_ssize_t *ਮਾਪ);
#  endif
# ਨਹੀਂਤਾਂ
#  ਜੇ PY_ਸੰਸਕਰਣ_HEX >= 0x030300f0
ਸਥਿਰ ਅੱਖਰ* (*py3_Py_Unicode_As_UTF8_And_ਮਾਪ)(PyObject *unicode, Py_ssize_t *ਮਾਪ);
#  ਨਹੀਂਤਾਂ
ਸਥਿਰ ਅੱਖਰ* (*py3__PyUnicode_AsString)(PyObject *unicode);
#  endif
# endif
ਸਥਿਰ ਪੂਰਨ (*py3_PyUnicode_CompareWithASCIIString)(PyObject *unicode, ਅਚਲ ਅੱਖਰ* string);
ਸਥਿਰ PyObject* (*py3_PyUnicode_AsEncodedString)(PyObject *unicode, ਅਚਲ ਅੱਖਰ* encoding, ਅਚਲ ਅੱਖਰ* errors);
ਸਥਿਰ PyObject* (*py3_PyUnicode_AsUTF8String)(PyObject *unicode);
ਸਥਿਰ ਅੱਖਰ* (*py3_Py_ਬਾਈਟਾਂ_As_String)(PyObject *ਬਾਈਟਾਂ);
ਸਥਿਰ ਪੂਰਨ (*py3_Py_ਬਾਈਟਾਂ_As_String_And_ਮਾਪ)(PyObject *ਬਾਈਟਾਂ, ਅੱਖਰ **ਬਫਰ, Py_ssize_t *ਲੰਮਾਈ);
ਸਥਿਰ PyObject* (*py3_Py_ਬਾਈਟਾਂ_From_String)(ਅੱਖਰ *str);
ਸਥਿਰ PyObject* (*py3_Py_ਬਾਈਟਾਂ_From_String_And_ਮਾਪ)(ਅੱਖਰ *str, Py_ssize_t ਲੰਮਾਈ);
# ਜੇ PY_ਸੰਸਕਰਣ_HEX >= 0x030900b0
ਸਥਿਰ PyObject* (*py3__PyObject_New)(PyTypeObject *);
# endif
ਸਥਿਰ PyObject* (*py3_PyFloat_FromDouble)(ਦੁੱਗਣਾ ਅੰਕ);
ਸਥਿਰ ਦੁੱਗਣਾ (*py3_PyFloat_AsDouble)(PyObject *);
ਸਥਿਰ PyObject* (*py3_PyObject_GenericGetAttr)(PyObject *obj, PyObject *name);
ਸਥਿਰ PyObject* (*py3_PyType_GenericAlloc)(PyTypeObject *type, Py_ssize_t nitems);
ਸਥਿਰ PyObject* (*py3_PyType_GenericNew)(PyTypeObject *type, PyObject *ਦਲੀਲਾਂ, PyObject *kwds);
ਸਥਿਰ PyTypeObject* py3_PyType_Type;
# ifndef USE_LIMITED_API
ਸਥਿਰ PyTypeObject* py3_PyStdPrinter_Type;
# endif
ਸਥਿਰ PyTypeObject* py3_Py_ਟੁਕੜਾ_Type;
ਸਥਿਰ PyTypeObject* py3_PyFloat_Type;
ਸਥਿਰ PyTypeObject* py3_PyBool_Type;
ਸਥਿਰ ਪੂਰਨ (*py3_Py_ਸੰਖਿਆ_Check)(PyObject *);
ਸਥਿਰ PyObject* (*py3_Py_ਸੰਖਿਆ_Long)(PyObject *);
ਸਥਿਰ PyObject* (*py3_PyErr_NewException)(ਅੱਖਰ *name, PyObject *ਅਧਾਰ, PyObject *dict);
ਸਥਿਰ PyObject* (*py3_PyCapsule_New)(ਖਾਲੀ *, ਅੱਖਰ *, PyCapsule_Destructor);
ਸਥਿਰ ਖਾਲੀ* (*py3_Py_Capsule_Get_ਸੂਚਕ)(PyObject *, ਅੱਖਰ *);
# ifdef Py_DEBUG
ਸਥਿਰ ਖਾਲੀ (*py3__Py_NegativeRefcount)(ਅਚਲ ਅੱਖਰ *fname, ਪੂਰਨ lineno, PyObject *op);
ਸਥਿਰ Py_ssize_t* py3_Py_Ref_ਕੁੱਲ;
ਸਥਿਰ PyObject* (*py3_PyModule_Create2TraceRefs)(ਬਣਤਰ PyModuleDef* module, ਪੂਰਨ module_api_ਸੰਸਕਰਣ);
# ਨਹੀਂਤਾਂ
ਸਥਿਰ PyObject* (*py3_PyModule_Create2)(ਬਣਤਰ PyModuleDef* module, ਪੂਰਨ module_api_ਸੰਸਕਰਣ);
# endif
# ਜੇ defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
ਸਥਿਰ ਖਾਲੀ (*py3__PyObject_DebugFree)(ਖਾਲੀ*);
ਸਥਿਰ ਖਾਲੀ* (*py3__PyObject_DebugMalloc)(size_t);
# ਨਹੀਂਤਾਂ
ਸਥਿਰ ਖਾਲੀ (*py3_PyObject_Free)(ਖਾਲੀ*);
ਸਥਿਰ ਖਾਲੀ* (*py3_PyObject_Malloc)(size_t);
# endif
ਸਥਿਰ PyObject*(*py3__PyObject_GC_New)(PyTypeObject *);
ਸਥਿਰ ਖਾਲੀ(*py3_PyObject_GC_Del)(ਖਾਲੀ *);
ਸਥਿਰ ਖਾਲੀ(*py3_PyObject_GC_UnTrack)(ਖਾਲੀ *);
ਸਥਿਰ ਪੂਰਨ (*py3_PyType_IsSubtype)(PyTypeObject *, PyTypeObject *);
# ifdef USE_LIMITED_API
ਸਥਿਰ ਖਾਲੀ* (*py3_Py_Type_Get_ਥਾਂ)(PyTypeObject *, ਪੂਰਨ);
ਸਥਿਰ PyObject* (*py3_PyType_FromSpec)(PyType_Spec *);
# endif

// Imported exception objects
ਸਥਿਰ PyObject *p3imp_PyExc_AttributeError;
ਸਥਿਰ PyObject *p3imp_Py_Exc_ਸੂਚਕਾਂਕ_Error;
ਸਥਿਰ PyObject *p3imp_Py_Exc_ਕੁੰਜੀ_Error;
ਸਥਿਰ PyObject *p3imp_Py_Exc_Keyboard_ਰੁਕਾਵਟ;
ਸਥਿਰ PyObject *p3imp_PyExc_TypeError;
ਸਥਿਰ PyObject *p3imp_Py_Exc_ਮੁੱਲ_Error;
ਸਥਿਰ PyObject *p3imp_PyExc_SystemExit;
ਸਥਿਰ PyObject *p3imp_PyExc_RuntimeError;
ਸਥਿਰ PyObject *p3imp_PyExc_ImportError;
ਸਥਿਰ PyObject *p3imp_Py_Exc_ਉਛਾਲ_Error;

# define PyExc_AttributeError p3imp_PyExc_AttributeError
# define Py_Exc_ਸੂਚਕਾਂਕ_Error p3imp_Py_Exc_ਸੂਚਕਾਂਕ_Error
# define Py_Exc_ਕੁੰਜੀ_Error p3imp_Py_Exc_ਕੁੰਜੀ_Error
# define Py_Exc_Keyboard_ਰੁਕਾਵਟ p3imp_Py_Exc_Keyboard_ਰੁਕਾਵਟ
# define PyExc_TypeError p3imp_PyExc_TypeError
# define Py_Exc_ਮੁੱਲ_Error p3imp_Py_Exc_ਮੁੱਲ_Error
# define PyExc_SystemExit p3imp_PyExc_SystemExit
# define PyExc_RuntimeError p3imp_PyExc_RuntimeError
# define PyExc_ImportError p3imp_PyExc_ImportError
# define Py_Exc_ਉਛਾਲ_Error p3imp_Py_Exc_ਉਛਾਲ_Error

/*
 * Table of name to function pointer of python.
 */
# define PYTHON_PROC FARPROC
ਸਥਿਰ ਬਣਤਰ
{
    ਅੱਖਰ *name;
    PYTHON_PROC *ptr;
} py3_funcname_ਸਾਰਣੀ[] =
{
    {"PySys_SetArgv", (PYTHON_PROC*)&py3_Py_Sys_ਸੈੱਟ_Argv},
    {"Py_SetPythonHome", (PYTHON_PROC*)&py3_Py_ਸੈੱਟ_Python_Home},
    {"Py_Initialize", (PYTHON_PROC*)&py3_Py_Initialize},
    {"_PyArg_ParseTuple_SizeT", (PYTHON_PROC*)&py3_PyArg_ParseTuple},
    {"_Py_BuildValue_SizeT", (PYTHON_PROC*)&py3_Py_Build_ਮੁੱਲ},
    {"PyMem_Free", (PYTHON_PROC*)&py3_PyMem_Free},
    {"PyMem_Malloc", (PYTHON_PROC*)&py3_PyMem_Malloc},
    {"PyList_New", (PYTHON_PROC*)&py3_PyList_New},
    {"PyGILState_Ensure", (PYTHON_PROC*)&py3_PyGILState_Ensure},
    {"PyGILState_Release", (PYTHON_PROC*)&py3_PyGILState_Release},
    {"PySys_SetObject", (PYTHON_PROC*)&py3_Py_Sys_ਸੈੱਟ_Object},
    {"PySys_GetObject", (PYTHON_PROC*)&py3_PySys_GetObject},
    {"PyList_Append", (PYTHON_PROC*)&py3_PyList_Append},
    {"PyList_Insert", (PYTHON_PROC*)&py3_Py_List_ਪਾਓ},
    {"PyList_Size", (PYTHON_PROC*)&py3_Py_List_ਮਾਪ},
    {"PyTuple_New", (PYTHON_PROC*)&py3_PyTuple_New},
    {"PyTuple_GetItem", (PYTHON_PROC*)&py3_Py_Tuple_Get_ਵਸਤੂ},
    {"PyTuple_SetItem", (PYTHON_PROC*)&py3_Py_Tuple_ਸੈੱਟ_ਵਸਤੂ},
    {"PyTuple_Size", (PYTHON_PROC*)&py3_Py_Tuple_ਮਾਪ},
    {"PySequence_Check", (PYTHON_PROC*)&py3_PySequence_Check},
    {"PySequence_Size", (PYTHON_PROC*)&py3_Py_Sequence_ਮਾਪ},
    {"PySequence_GetItem", (PYTHON_PROC*)&py3_Py_Sequence_Get_ਵਸਤੂ},
    {"PySequence_Fast", (PYTHON_PROC*)&py3_PySequence_Fast},
# ਜੇ PY_ਸੰਸਕਰਣ_HEX >= 0x030601f0
    {"PySlice_AdjustIndices", (PYTHON_PROC*)&py3_Py_ਟੁਕੜਾ_Adjust_Indices},
    {"PySlice_Unpack", (PYTHON_PROC*)&py3_Py_ਟੁਕੜਾ_Unpack},
# endif
    {"PySlice_GetIndicesEx", (PYTHON_PROC*)&py3_Py_ਟੁਕੜਾ_Get_Indices_Ex},
    {"PyErr_NoMemory", (PYTHON_PROC*)&py3_Py_Err_No_ਸਮ੍ਰਿਤੀ},
    {"Py_Finalize", (PYTHON_PROC*)&py3_Py_Finalize},
    {"PyErr_SetString", (PYTHON_PROC*)&py3_Py_Err_ਸੈੱਟ_String},
    {"PyErr_SetObject", (PYTHON_PROC*)&py3_Py_Err_ਸੈੱਟ_Object},
    {"PyErr_ExceptionMatches", (PYTHON_PROC*)&py3_PyErr_ExceptionMatches},
# ਜੇ defined(USE_LIMITED_API) || PY_ਸੰਸਕਰਣ_HEX >= 0x03080000
    {"Py_IncRef", (PYTHON_PROC*)&py3_Py_IncRef},
    {"Py_DecRef", (PYTHON_PROC*)&py3_Py_DecRef},
# endif
# ifdef USE_LIMITED_API
    {"Py_CompileString", (PYTHON_PROC*)&py3_Py_CompileString},
    {"PyEval_EvalCode", (PYTHON_PROC*)&Py_Eval_Eval_ਕੋਡ},
# ਨਹੀਂਤਾਂ
    {"PyRun_SimpleString", (PYTHON_PROC*)&py3_Py_ਲੜੀਵਾਰ_Simple_String},
    {"PyRun_String", (PYTHON_PROC*)&py3_Py_ਲੜੀਵਾਰ_String},
# endif
    {"PyObject_GetAttrString", (PYTHON_PROC*)&py3_PyObject_GetAttrString},
    {"PyObject_HasAttrString", (PYTHON_PROC*)&py3_PyObject_HasAttrString},
    {"PyObject_SetAttrString", (PYTHON_PROC*)&py3_Py_Object_ਸੈੱਟ_Attr_String},
    {"PyObject_CallFunctionObjArgs", (PYTHON_PROC*)&py3_Py_Object_Call_Function_Obj_ਦਲੀਲਾਂ},
# ਜੇ PY_ਸੰਸਕਰਣ_HEX >= 0x030d0000
    {"PyObject_CallFunction", (PYTHON_PROC*)&py3_PyObject_CallFunction},
# ਨਹੀਂਤਾਂ
    {"_PyObject_CallFunction_SizeT", (PYTHON_PROC*)&py3_Py_Object_Call_Function_ਮਾਪ_T},
# endif
    {"PyObject_Call", (PYTHON_PROC*)&py3_PyObject_Call},
    {"PyEval_GetGlobals", (PYTHON_PROC*)&py3_PyEval_GetGlobals},
    {"PyEval_GetLocals", (PYTHON_PROC*)&py3_PyEval_GetLocals},
    {"PyList_GetItem", (PYTHON_PROC*)&py3_Py_List_Get_ਵਸਤੂ},
    {"PyImport_ImportModule", (PYTHON_PROC*)&py3_PyImport_ImportModule},
    {"PyImport_AddModule", (PYTHON_PROC*)&py3_Py_Import_ਜੋੜੋ_Module},
    {"PyErr_BadArgument", (PYTHON_PROC*)&py3_Py_Err_Bad_ਦਲੀਲ},
    {"PyErr_Occurred", (PYTHON_PROC*)&py3_PyErr_Occurred},
    {"PyModule_GetDict", (PYTHON_PROC*)&py3_PyModule_GetDict},
    {"PyList_SetItem", (PYTHON_PROC*)&py3_Py_List_ਸੈੱਟ_ਵਸਤੂ},
    {"PyDict_GetItemString", (PYTHON_PROC*)&py3_Py_Dict_Get_ਵਸਤੂ_String},
    {"PyDict_Next", (PYTHON_PROC*)&py3_Py_Dict_ਅਗਲਾ},
    {"PyMapping_Check", (PYTHON_PROC*)&py3_PyMapping_Check},
    {"PyMapping_Keys", (PYTHON_PROC*)&py3_Py_Mapping_ਕੁੰਜੀਆਂ},
# ਜੇ (defined(USE_LIMITED_API) && Py_LIMITED_API >= 0x03080000) || \
       (!defined(USE_LIMITED_API) && PY_ਸੰਸਕਰਣ_HEX >= 0x03080000)
    {"PyIter_Check", (PYTHON_PROC*)&py3_PyIter_Check},
# endif
    {"PyIter_Next", (PYTHON_PROC*)&py3_Py_Iter_ਅਗਲਾ},
    {"PyObject_GetIter", (PYTHON_PROC*)&py3_PyObject_GetIter},
    {"PyObject_Repr", (PYTHON_PROC*)&py3_PyObject_Repr},
    {"PyObject_GetItem", (PYTHON_PROC*)&py3_Py_Object_Get_ਵਸਤੂ},
    {"PyObject_IsTrue", (PYTHON_PROC*)&py3_PyObject_IsTrue},
    {"PyLong_FromLong", (PYTHON_PROC*)&py3_PyLong_FromLong},
    {"PyDict_New", (PYTHON_PROC*)&py3_PyDict_New},
# ਜੇ PY_ਸੰਸਕਰਣ_HEX >= 0x03040000
    {"PyType_GetFlags", (PYTHON_PROC*)&py3_PyType_GetFlags},
# endif
    {"PyType_Ready", (PYTHON_PROC*)&py3_PyType_Ready},
    {"PyDict_SetItemString", (PYTHON_PROC*)&py3_Py_Dict_ਸੈੱਟ_ਵਸਤੂ_String},
    {"PyLong_AsLong", (PYTHON_PROC*)&py3_PyLong_AsLong},
    {"PyErr_SetNone", (PYTHON_PROC*)&py3_Py_Err_ਸੈੱਟ_None},
    {"PyEval_InitThreads", (PYTHON_PROC*)&py3_Py_Eval_Init_ਧਾਗੇ},
    {"PyEval_RestoreThread", (PYTHON_PROC*)&py3_PyEval_RestoreThread},
    {"PyEval_SaveThread", (PYTHON_PROC*)&py3_PyEval_SaveThread},
    {"_PyArg_Parse_SizeT", (PYTHON_PROC*)&py3_PyArg_Parse},
    {"Py_IsInitialized", (PYTHON_PROC*)&py3_Py_IsInitialized},
# ਜੇ !defined(USE_LIMITED_API) && PY_ਸੰਸਕਰਣ_HEX < 0x030D0000
    {"_PyObject_NextNotImplemented", (PYTHON_PROC*)&py3_Py_Object_ਅਗਲਾ_Not_Implemented},
# endif
    {"_Py_NoneStruct", (PYTHON_PROC*)&py3__Py_NoneStruct},
    {"_Py_FalseStruct", (PYTHON_PROC*)&py3__Py_FalseStruct},
    {"_Py_TrueStruct", (PYTHON_PROC*)&py3__Py_TrueStruct},
    {"PyErr_Clear", (PYTHON_PROC*)&py3_PyErr_Clear},
    {"PyErr_Format", (PYTHON_PROC*)&py3_PyErr_Format},
    {"PyErr_PrintEx", (PYTHON_PROC*)&py3_PyErr_PrintEx},
    {"PyObject_Init", (PYTHON_PROC*)&py3__PyObject_Init},
    {"PyModule_AddObject", (PYTHON_PROC*)&py3_Py_Module_ਜੋੜੋ_Object},
    {"PyImport_AppendInittab", (PYTHON_PROC*)&py3_PyImport_AppendInittab},
# ifdef USE_LIMITED_API
#  ਜੇ Py_LIMITED_API >= 0x030a0000
    {"PyUnicode_AsUTF8AndSize", (PYTHON_PROC*)&py3_Py_Unicode_As_UTF8_And_ਮਾਪ},
#  endif
# ਨਹੀਂਤਾਂ
#  ਜੇ PY_ਸੰਸਕਰਣ_HEX >= 0x030300f0
    {"PyUnicode_AsUTF8AndSize", (PYTHON_PROC*)&py3_Py_Unicode_As_UTF8_And_ਮਾਪ},
#  ਨਹੀਂਤਾਂ
    {"_PyUnicode_AsString", (PYTHON_PROC*)&py3__PyUnicode_AsString},
#  endif
# endif
    {"PyUnicode_CompareWithASCIIString", (PYTHON_PROC*)&py3_PyUnicode_CompareWithASCIIString},
    {"PyUnicode_AsUTF8String", (PYTHON_PROC*)&py3_PyUnicode_AsUTF8String},
# ifdef Py_UNICODE_USE_UCS_FUNCTIONS
#  ifdef Py_UNICODE_WIDE
    {"PyUnicodeUCS4_FromFormat", (PYTHON_PROC*)&py3_PyUnicodeUCS4_FromFormat},
#  ਨਹੀਂਤਾਂ
    {"PyUnicodeUCS2_FromFormat", (PYTHON_PROC*)&py3_PyUnicodeUCS2_FromFormat},
#  endif
# ਨਹੀਂਤਾਂ
    {"PyUnicode_FromFormat", (PYTHON_PROC*)&py3_PyUnicode_FromFormat},
# endif
    {"PyBytes_AsString", (PYTHON_PROC*)&py3_Py_ਬਾਈਟਾਂ_As_String},
    {"PyBytes_AsStringAndSize", (PYTHON_PROC*)&py3_Py_ਬਾਈਟਾਂ_As_String_And_ਮਾਪ},
    {"PyBytes_FromString", (PYTHON_PROC*)&py3_Py_ਬਾਈਟਾਂ_From_String},
    {"PyBytes_FromStringAndSize", (PYTHON_PROC*)&py3_Py_ਬਾਈਟਾਂ_From_String_And_ਮਾਪ},
# ਜੇ PY_ਸੰਸਕਰਣ_HEX >= 0x030900b0
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
    {"PySlice_Type", (PYTHON_PROC*)&py3_Py_ਟੁਕੜਾ_Type},
    {"PyFloat_Type", (PYTHON_PROC*)&py3_PyFloat_Type},
    {"PyBool_Type", (PYTHON_PROC*)&py3_PyBool_Type},
    {"PyNumber_Check", (PYTHON_PROC*)&py3_Py_ਸੰਖਿਆ_Check},
    {"PyNumber_Long", (PYTHON_PROC*)&py3_Py_ਸੰਖਿਆ_Long},
    {"PyErr_NewException", (PYTHON_PROC*)&py3_PyErr_NewException},
# ifdef Py_DEBUG
    {"_Py_NegativeRefcount", (PYTHON_PROC*)&py3__Py_NegativeRefcount},
    {"_Py_RefTotal", (PYTHON_PROC*)&py3_Py_Ref_ਕੁੱਲ},
    {"PyModule_Create2TraceRefs", (PYTHON_PROC*)&py3_PyModule_Create2TraceRefs},
# ਨਹੀਂਤਾਂ
    {"PyModule_Create2", (PYTHON_PROC*)&py3_PyModule_Create2},
# endif
# ਜੇ defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
    {"_PyObject_DebugFree", (PYTHON_PROC*)&py3__PyObject_DebugFree},
    {"_PyObject_DebugMalloc", (PYTHON_PROC*)&py3__PyObject_DebugMalloc},
# ਨਹੀਂਤਾਂ
    {"PyObject_Malloc", (PYTHON_PROC*)&py3_PyObject_Malloc},
    {"PyObject_Free", (PYTHON_PROC*)&py3_PyObject_Free},
# endif
    {"_PyObject_GC_New", (PYTHON_PROC*)&py3__PyObject_GC_New},
    {"PyObject_GC_Del", (PYTHON_PROC*)&py3_PyObject_GC_Del},
    {"PyObject_GC_UnTrack", (PYTHON_PROC*)&py3_PyObject_GC_UnTrack},
    {"PyType_IsSubtype", (PYTHON_PROC*)&py3_PyType_IsSubtype},
    {"PyCapsule_New", (PYTHON_PROC*)&py3_PyCapsule_New},
    {"PyCapsule_GetPointer", (PYTHON_PROC*)&py3_Py_Capsule_Get_ਸੂਚਕ},
# ifdef USE_LIMITED_API
#  ਜੇ PY_ਸੰਸਕਰਣ_HEX >= 0x03040000
    {"PyType_GetSlot", (PYTHON_PROC*)&py3_Py_Type_Get_ਥਾਂ},
#  endif
    {"PyType_FromSpec", (PYTHON_PROC*)&py3_PyType_FromSpec},
# endif
    {"", ਨਿਰਾ},
};

# ਜੇ defined(USE_LIMITED_API) || PY_ਸੰਸਕਰਣ_HEX >= 0x03080000
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

# ਜੇ PY_ਸੰਸਕਰਣ_HEX >= 0x030900b0
    ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ
py3_PyType_HasFeature(PyTypeObject *type, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ feature)
{
    ਵਾਪਸ ((PyType_GetFlags(type) & feature) != 0);
}
#  define PyType_HasFeature(t,f) py3_PyType_HasFeature(t,f)
# endif

# ਜੇ PY_ਸੰਸਕਰਣ_HEX >= 0x030a00b2
    ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ
py3__PyObject_TypeCheck(PyObject *ob, PyTypeObject *type)
{
    ਵਾਪਸ Py_IS_TYPE(ob, type) || PyType_IsSubtype(Py_TYPE(ob), type);
}
#  ਜੇ PY_ਸੰਸਕਰਣ_HEX >= 0x030b00b3
#   undef PyObject_TypeCheck
#   define PyObject_TypeCheck(o,t) py3__PyObject_TypeCheck(o,t)
#  ਨਹੀਂਤਾਂ
#   define _PyObject_TypeCheck(o,t) py3__PyObject_TypeCheck(o,t)
#  endif
# endif

# ਜੇ !defined(USE_LIMITED_API) && PY_ਸੰਸਕਰਣ_HEX >= 0x030c00b0
// PyTuple_GET_SIZE/PyList_GET_SIZE are inlined functions that use Py_SIZE(),
// which started to introduce linkage dependency from Python 3.12. When we
// build Python in dynamic mode, we don't link against it in build time, and
// this would fail to build. Just use the non-inlined version instead.
#  undef Py_Tuple_GET_ਮਾਪ
#  define Py_Tuple_GET_ਮਾਪ(o) Py_Tuple_ਮਾਪ(o)
#  undef Py_List_GET_ਮਾਪ
#  define Py_List_GET_ਮਾਪ(o) Py_List_ਮਾਪ(o)
# endif

# ifdef MSWIN
/*
 * Look up the library "libname" using the InstallPath registry key.
 * Return NULL when failed.  Return an allocated string when successful.
 */
    ਸਥਿਰ WCHAR *
py3_get_system_libname(ਅਚਲ ਅੱਖਰ *libname)
{
    ਅਚਲ WCHAR	*pythoncore = L"Software\\Python\\PythonCore";
    ਅਚਲ ਅੱਖਰ	*cp = libname;
    WCHAR	subkey[128];
    HKEY	h_ਕੁੰਜੀ;
    ਪੂਰਨ		i;
    DWORD	j, len;
    LSTATUS	ret;

    ਜਦੋਂ (*cp != '\0')
    {
	ਜੇ (*cp == ':' || *cp == '\\' || *cp == '/')
	{
	    // Bail out if "libname" contains path separator, assume it is
	    // an absolute path.
	    ਵਾਪਸ ਨਿਰਾ;
	}
	++cp;
    }

    WCHAR   keyfound[32];
    HKEY    h_ਕੁੰਜੀ_ਸਿਖਰ[] = {HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE};
    HKEY    h_ਕੁੰਜੀ_ਮਿਲਿਆ = ਨਿਰਾ;
#  ifdef USE_LIMITED_API
    ਲੰਮਾ    maxminor = -1;
#  endif
    ਲਈ (i = 0; i < ARRAY_ਲੰਮਾਈ(h_ਕੁੰਜੀ_ਸਿਖਰ); i++)
    {
	ਲੰਮਾ	major, minor;

	ret = Reg_Open_ਕੁੰਜੀ_Ex_W(h_ਕੁੰਜੀ_ਸਿਖਰ[i], pythoncore, 0, ਕੁੰਜੀ_READ, &h_ਕੁੰਜੀ);
	ਜੇ (ret != ERROR_SUCCESS)
	    ਜਾਰੀ;
	ਲਈ (j = 0;; j++)
	{
	    WCHAR   keyname[32];
	    WCHAR   *wp;

	    len = ARRAY_ਲੰਮਾਈ(keyname);
	    ret = Reg_Enum_ਕੁੰਜੀ_Ex_W(h_ਕੁੰਜੀ, j, keyname, &len,
						    ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);
	    ਜੇ (ret == ERROR_NO_ਹੋਰ_ਚੀਜ਼ਾਂ)
		ਰੋਕੋ;

	    major = wcstol(keyname, &wp, 10);
	    ਜੇ (*wp != L'.')
		ਜਾਰੀ;
	    minor = wcstol(wp + 1, &wp, 10);
#  ifdef _WIN64
	    ਜੇ (*wp != L'\0')
		ਜਾਰੀ;
#  ਨਹੀਂਤਾਂ
	    ਜੇ (wcscmp(wp, L"-32") != 0)
		ਜਾਰੀ;
#  endif

	    ਜੇ (major != PY_MAJOR_ਸੰਸਕਰਣ)
		ਜਾਰੀ;
#  ifdef USE_LIMITED_API
	    // Search the latest version.
	    ਜੇ ((minor > maxminor)
		    && (minor >= ((Py_LIMITED_API >> 16) & 0xff)))
	    {
		maxminor = minor;
		wcscpy(keyfound, keyname);
		h_ਕੁੰਜੀ_ਮਿਲਿਆ = h_ਕੁੰਜੀ_ਸਿਖਰ[i];
	    }
#  ਨਹੀਂਤਾਂ
	    // Check if it matches with the compiled version.
	    ਜੇ (minor == PY_MINOR_ਸੰਸਕਰਣ)
	    {
		wcscpy(keyfound, keyname);
		h_ਕੁੰਜੀ_ਮਿਲਿਆ = h_ਕੁੰਜੀ_ਸਿਖਰ[i];
		ਰੋਕੋ;
	    }
#  endif
	}
	Reg_Close_ਕੁੰਜੀ(h_ਕੁੰਜੀ);
#  ifdef USE_LIMITED_API
	ਜੇ (h_ਕੁੰਜੀ_ਮਿਲਿਆ != ਨਿਰਾ)
	    ਰੋਕੋ;
#  endif
    }
    ਜੇ (h_ਕੁੰਜੀ_ਮਿਲਿਆ == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    swprintf(subkey, ARRAY_ਲੰਮਾਈ(subkey), L"%ls\\%ls\\InstallPath",
							pythoncore, keyfound);
    ret = Reg_Get_ਮੁੱਲ_W(h_ਕੁੰਜੀ_ਮਿਲਿਆ, subkey, ਨਿਰਾ, RRF_RT_REG_SZ,
							    ਨਿਰਾ, ਨਿਰਾ, &len);
    ਜੇ (ret != ERROR_ਹੋਰ_ਅੰਕੜੇ && ret != ERROR_SUCCESS)
	ਵਾਪਸ ਨਿਰਾ;
    size_t len2 = len / ਆਕਾਰ(WCHAR) + 1 + ਲੜੀ_ਲੰਬਾਈ(libname);
    WCHAR *ਰਸਤਾ = alloc(len2 * ਆਕਾਰ(WCHAR));
    ਜੇ (ਰਸਤਾ == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    ret = Reg_Get_ਮੁੱਲ_W(h_ਕੁੰਜੀ_ਮਿਲਿਆ, subkey, ਨਿਰਾ, RRF_RT_REG_SZ,
							    ਨਿਰਾ, ਰਸਤਾ, &len);
    ਜੇ (ret != ERROR_SUCCESS)
    {
	vim_free(ਰਸਤਾ);
	ਵਾਪਸ ਨਿਰਾ;
    }
    // Remove trailing path separators.
    size_t len3 = wcslen(ਰਸਤਾ);
    ਜੇ ((len3 > 0) && (ਰਸਤਾ[len3 - 1] == L'/' || ਰਸਤਾ[len3 - 1] == L'\\'))
	--len3;
    swprintf(ਰਸਤਾ + len3, len2 - len3, L"\\%hs", libname);
    ਵਾਪਸ ਰਸਤਾ;
}
# endif

/*
 * Load library and get all pointers.
 * Parameter 'libname' provides name of DLL.
 * Return OK or FAIL.
 */
    ਸਥਿਰ ਪੂਰਨ
py3_runtime_link_init(ਅੱਖਰ *libname, ਪੂਰਨ verbose)
{
    ਪੂਰਨ i;
    PYTHON_PROC *ucs_from_string = (PYTHON_PROC *)&py3_PyUnicode_FromString;
    PYTHON_PROC *ucs_decode = (PYTHON_PROC *)&py3_PyUnicode_Decode;
    PYTHON_PROC *ucs_as_encoded_string =
				 (PYTHON_PROC *)&py3_PyUnicode_AsEncodedString;

# ਜੇ !(defined(PY_NO_RTLD_GLOBAL) && defined(PY3_NO_RTLD_GLOBAL)) && defined(UNIX) && defined(FEAT_PYTHON)
    // Can't have Python and Python3 loaded at the same time.
    // It causes a crash, because RTLD_GLOBAL is needed for
    // standard C extension libraries of one or both python versions.
    ਜੇ (python_loaded())
    {
	ਜੇ (verbose)
	    emsg(_(e_this_vim_cannot_execute_py3_after_using_python));
	ਵਾਪਸ FAIL;
    }
# endif

    ਜੇ (hinstPy3 != 0)
	ਵਾਪਸ OK;
    hinstPy3 = ਲੱਦੋ_dll(libname);

# ifdef MSWIN
    ਜੇ (!hinstPy3)
    {
	// Attempt to use the path from InstallPath as stored in the registry.
	WCHAR *syslibname = py3_get_system_libname(libname);

	ਜੇ (syslibname != ਨਿਰਾ)
	{
	    hinstPy3 = ਲੱਦੋ_Library_Ex_W(syslibname, ਨਿਰਾ,
		    ਲੱਦੋ_LIBRARY_ਖੋਜ_DLL_ਲੱਦੋ_DIR |
		    ਲੱਦੋ_LIBRARY_ਖੋਜ_SYSTEM32);
	    vim_free(syslibname);
	}
    }
# endif

    ਜੇ (!hinstPy3)
    {
	ਜੇ (verbose)
	    semsg(_(e_could_not_ਲੱਦੋ_library_str_str), libname, ਲੱਦੋ_dll_error());
	ਵਾਪਸ FAIL;
    }

    ਲਈ (i = 0; py3_funcname_ਸਾਰਣੀ[i].ptr; ++i)
    {
	ਜੇ ((*py3_funcname_ਸਾਰਣੀ[i].ptr = ਚਿੰਨ੍ਹ_from_dll(hinstPy3,
			py3_funcname_ਸਾਰਣੀ[i].name)) == ਨਿਰਾ)
	{
	    close_dll(hinstPy3);
	    hinstPy3 = 0;
	    ਜੇ (verbose)
		semsg(_(e_could_not_ਲੱਦੋ_library_function_str), py3_funcname_ਸਾਰਣੀ[i].name);
	    ਵਾਪਸ FAIL;
	}
    }

    // Load unicode functions separately as only the ucs2 or the ucs4 functions
    // will be present in the library.
# ਜੇ PY_ਸੰਸਕਰਣ_HEX >= 0x030300f0
    *ucs_from_string = ਚਿੰਨ੍ਹ_from_dll(hinstPy3, "PyUnicode_FromString");
    *ucs_decode = ਚਿੰਨ੍ਹ_from_dll(hinstPy3, "PyUnicode_Decode");
    *ucs_as_encoded_string = ਚਿੰਨ੍ਹ_from_dll(hinstPy3,
	    "PyUnicode_AsEncodedString");
# ਨਹੀਂਤਾਂ
    *ucs_from_string = ਚਿੰਨ੍ਹ_from_dll(hinstPy3, "PyUnicodeUCS2_FromString");
    *ucs_decode = ਚਿੰਨ੍ਹ_from_dll(hinstPy3,
	    "PyUnicodeUCS2_Decode");
    *ucs_as_encoded_string = ਚਿੰਨ੍ਹ_from_dll(hinstPy3,
	    "PyUnicodeUCS2_AsEncodedString");
    ਜੇ (*ucs_from_string == ਨਿਰਾ || *ucs_decode == ਨਿਰਾ
					     || *ucs_as_encoded_string == ਨਿਰਾ)
    {
	*ucs_from_string = ਚਿੰਨ੍ਹ_from_dll(hinstPy3,
		"PyUnicodeUCS4_FromString");
	*ucs_decode = ਚਿੰਨ੍ਹ_from_dll(hinstPy3,
		"PyUnicodeUCS4_Decode");
	*ucs_as_encoded_string = ਚਿੰਨ੍ਹ_from_dll(hinstPy3,
		"PyUnicodeUCS4_AsEncodedString");
    }
# endif
    ਜੇ (*ucs_from_string == ਨਿਰਾ || *ucs_decode == ਨਿਰਾ
					     || *ucs_as_encoded_string == ਨਿਰਾ)
    {
	close_dll(hinstPy3);
	hinstPy3 = 0;
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
python3_enabled(ਪੂਰਨ verbose)
{
    ਵਾਪਸ py3_runtime_link_init((ਅੱਖਰ *)p_py3dll, verbose) == OK;
}

/*
 * Load the standard Python exceptions - don't import the symbols from the
 * DLL, as this can cause errors (importing data symbols is not reliable).
 */
    ਸਥਿਰ ਖਾਲੀ
get_py3_exceptions(ਖਾਲੀ)
{
    PyObject *exmod = PyImport_ImportModule("builtins");
    PyObject *exdict = PyModule_GetDict(exmod);
    p3imp_PyExc_AttributeError = Py_Dict_Get_ਵਸਤੂ_String(exdict, "AttributeError");
    p3imp_Py_Exc_ਸੂਚਕਾਂਕ_Error = Py_Dict_Get_ਵਸਤੂ_String(exdict, "IndexError");
    p3imp_Py_Exc_ਕੁੰਜੀ_Error = Py_Dict_Get_ਵਸਤੂ_String(exdict, "KeyError");
    p3imp_Py_Exc_Keyboard_ਰੁਕਾਵਟ = Py_Dict_Get_ਵਸਤੂ_String(exdict, "KeyboardInterrupt");
    p3imp_PyExc_TypeError = Py_Dict_Get_ਵਸਤੂ_String(exdict, "TypeError");
    p3imp_Py_Exc_ਮੁੱਲ_Error = Py_Dict_Get_ਵਸਤੂ_String(exdict, "ValueError");
    p3imp_PyExc_SystemExit = Py_Dict_Get_ਵਸਤੂ_String(exdict, "SystemExit");
    p3imp_PyExc_RuntimeError = Py_Dict_Get_ਵਸਤੂ_String(exdict, "RuntimeError");
    p3imp_PyExc_ImportError = Py_Dict_Get_ਵਸਤੂ_String(exdict, "ImportError");
    p3imp_Py_Exc_ਉਛਾਲ_Error = Py_Dict_Get_ਵਸਤੂ_String(exdict, "OverflowError");
    Py_XINCREF(p3imp_PyExc_AttributeError);
    Py_XINCREF(p3imp_Py_Exc_ਸੂਚਕਾਂਕ_Error);
    Py_XINCREF(p3imp_Py_Exc_ਕੁੰਜੀ_Error);
    Py_XINCREF(p3imp_Py_Exc_Keyboard_ਰੁਕਾਵਟ);
    Py_XINCREF(p3imp_PyExc_TypeError);
    Py_XINCREF(p3imp_Py_Exc_ਮੁੱਲ_Error);
    Py_XINCREF(p3imp_PyExc_SystemExit);
    Py_XINCREF(p3imp_PyExc_RuntimeError);
    Py_XINCREF(p3imp_PyExc_ImportError);
    Py_XINCREF(p3imp_Py_Exc_ਉਛਾਲ_Error);
    Py_XDECREF(exmod);
}
#ਜੇ_ਅੰਤ // DYNAMIC_PYTHON3

ਸਥਿਰ ਪੂਰਨ py3initialised = 0;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PYINITIALISED py3initialised
ਸਥਿਰ ਪੂਰਨ python_ਸਿਰਾ_called = FALSE;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_LIMITED_API
# define DESTRUCTOR_ਅੰਤ(self) \
    ((freefunc)Py_Type_Get_ਥਾਂ(Py_TYPE((PyObject*)self), Py_tp_free))((PyObject*)self)
#ਨਹੀਂਤਾਂ
# define DESTRUCTOR_ਅੰਤ(self) Py_TYPE(self)->tp_free((PyObject*)self)
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WIN_PYTHON_REF(win) win->w_python3_ref
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BUF_PYTHON_REF(buf) buf->b_python3_ref
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TAB_PYTHON_REF(tab) tab->tp_python3_ref

    ਸਥਿਰ ਖਾਲੀ
call_PyObject_Free(ਖਾਲੀ *p)
{
#ਜੇ defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
    _PyObject_DebugFree(p);
#ਨਹੀਂਤਾਂ
    PyObject_Free(p);
#ਜੇ_ਅੰਤ
}

    ਸਥਿਰ PyObject *
call_PyType_GenericNew(PyTypeObject *type, PyObject *ਦਲੀਲਾਂ, PyObject *kwds)
{
    ਵਾਪਸ PyType_GenericNew(type,ਦਲੀਲਾਂ,kwds);
}

    ਸਥਿਰ PyObject *
call_PyType_GenericAlloc(PyTypeObject *type, Py_ssize_t nitems)
{
    ਵਾਪਸ PyType_GenericAlloc(type,nitems);
}

ਸਥਿਰ PyObject *OutputGetattro(PyObject *, PyObject *);
ਸਥਿਰ ਪੂਰਨ OutputSetattro(PyObject *, PyObject *, PyObject *);
ਸਥਿਰ PyObject *ਬਫਰ_Getattro(PyObject *, PyObject *);
ਸਥਿਰ ਪੂਰਨ ਬਫਰ_Setattro(PyObject *, PyObject *, PyObject *);
ਸਥਿਰ PyObject *Tab_ਪੰਨਾ_Getattro(PyObject *, PyObject *);
ਸਥਿਰ PyObject *ਖਿੜਕੀ_Getattro(PyObject *, PyObject *);
ਸਥਿਰ ਪੂਰਨ ਖਿੜਕੀ_Setattro(PyObject *, PyObject *, PyObject *);
ਸਥਿਰ PyObject *RangeGetattro(PyObject *, PyObject *);
ਸਥਿਰ PyObject *CurrentGetattro(PyObject *, PyObject *);
ਸਥਿਰ ਪੂਰਨ CurrentSetattro(PyObject *, PyObject *, PyObject *);
ਸਥਿਰ PyObject *DictionaryGetattro(PyObject *, PyObject *);
ਸਥਿਰ ਪੂਰਨ DictionarySetattro(PyObject *, PyObject *, PyObject *);
ਸਥਿਰ PyObject *ListGetattro(PyObject *, PyObject *);
ਸਥਿਰ ਪੂਰਨ ListSetattro(PyObject *, PyObject *, PyObject *);
ਸਥਿਰ PyObject *TupleGetattro(PyObject *, PyObject *);
ਸਥਿਰ ਪੂਰਨ TupleSetattro(PyObject *, PyObject *, PyObject *);
ਸਥਿਰ PyObject *FunctionGetattro(PyObject *, PyObject *);

ਸਥਿਰ ਬਣਤਰ PyModuleDef vimmodule;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PY_CAN_RECURSE

/*
 * Include the code shared with if_python.c
 */
#ਅਨ੍ਤਰ੍ਭਾਵ "if_py_both.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_LIMITED_API
# ਜੇ Py_LIMITED_API >= 0x030A0000
#  define PY_UNICODE_GET_UTF8_CHARS(obj) Py_Unicode_As_UTF8_And_ਮਾਪ(obj, ਨਿਰਾ)
# ਨਹੀਂਤਾਂ
// Python limited API before 3.10 lack easy ways to query the raw UTF-8 chars.
// We need to first convert the string to bytes, and then extract the chars.
// This function is only used for attribute string comparisons, which have
// known short length. As such, just allocate a short static buffer to hold
// the characters instead of having to allocate/deallcoate it.
//
// An alternative would be to convert all attribute string comparisons to use
// PyUnicode_CompareWithASCIIString to skip having to extract the chars.
ਸਥਿਰ ਅੱਖਰ py3_unicode_utf8_chars[20];
ਸਥਿਰ ਅੱਖਰ* PY_UNICODE_GET_UTF8_CHARS(PyObject* str)
{
    py3_unicode_utf8_chars[0] = '\0';
    PyObject* ਬਾਈਟਾਂ = PyUnicode_AsUTF8String(str);
    ਜੇ (ਬਾਈਟਾਂ)
    {
	ਅੱਖਰ *chars;
	Py_ssize_t len;
	ਜੇ (Py_ਬਾਈਟਾਂ_As_String_And_ਮਾਪ(ਬਾਈਟਾਂ, &chars, &len) != -1)
	{
	    ਜੇ (len < (Py_ssize_t)ਆਕਾਰ(py3_unicode_utf8_chars))
		// PyBytes_AsStringAndSize guarantees null-termination
		ਸਮ੍ਰਿਤੀ_ਨਕਲ(py3_unicode_utf8_chars, chars, len + 1);
	}
	Py_DECREF(ਬਾਈਟਾਂ);
    }
    ਵਾਪਸ py3_unicode_utf8_chars;
}
# endif
#ਨਹੀਂਤਾਂ	// !USE_LIMITED_API
# ਜੇ PY_ਸੰਸਕਰਣ_HEX >= 0x030300f0
#  define PY_UNICODE_GET_UTF8_CHARS(obj) Py_Unicode_As_UTF8_And_ਮਾਪ(obj, ਨਿਰਾ)
# ਨਹੀਂਤਾਂ
#  define PY_UNICODE_GET_UTF8_CHARS _PyUnicode_AsString
# endif
#ਜੇ_ਅੰਤ

// NOTE: Must always be used at the start of a block, since it declares "name".
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GET_ATTR_STRING(name, nameobj) \
    ਅੱਖਰ	*name = ""; \
    ਜੇ (PyUnicode_Check(nameobj)) \
	name = (ਅੱਖਰ *)PY_UNICODE_GET_UTF8_CHARS(nameobj)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PY3OBJ_DELETED(obj) (obj->ob_ਅਧਾਰ.ob_refcnt<=0)

///////////////////////////////////////////////////////
// Internal function prototypes.

ਸਥਿਰ PyObject *Py3Init_vim(ਖਾਲੀ);

///////////////////////////////////////////////////////
// 1. Python interpreter main program.

    ਖਾਲੀ
python3_ਸਿਰਾ(ਖਾਲੀ)
{
    ਸਥਿਰ ਪੂਰਨ recurse = 0;

    // If a crash occurs while doing this, don't try again.
    ਜੇ (recurse != 0)
	ਵਾਪਸ;

    python_ਸਿਰਾ_called = TRUE;
    ++recurse;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DYNAMIC_PYTHON3
    ਜੇ (hinstPy3)
#ਜੇ_ਅੰਤ
    ਜੇ (Py_IsInitialized())
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_LIMITED_API
	shutdown_types();
#ਜੇ_ਅੰਤ

	// acquire lock before finalizing
	PyGILState_Ensure();

	Py_Finalize();
    }

    --recurse;
}

#ਜੇ defined(DYNAMIC_PYTHON3) && defined(DYNAMIC_PYTHON) && defined(FEAT_PYTHON) && defined(UNIX)
    ਪੂਰਨ
python3_loaded(ਖਾਲੀ)
{
    ਵਾਪਸ (hinstPy3 != 0);
}
#ਜੇ_ਅੰਤ

ਸਥਿਰ wchar_t *py_home_buf = ਨਿਰਾ;

#ਜੇ defined(MSWIN) && (PY_ਸੰਸਕਰਣ_HEX >= 0x030500f0)
/*
 * Return TRUE if stdin is readable from Python 3.
 */
    ਸਥਿਰ BOOL
is_stdin_readable(ਖਾਲੀ)
{
    DWORD	    ਬਹੁਲਕ, eventnum;
    ਬਣਤਰ _stat    st;
    ਪੂਰਨ		    fd = fileno(stdin);
    HANDLE	    hstdin = (HANDLE)_get_osfhandle(fd);

    // Check if stdin is connected to the console.
    ਜੇ (Get_Console_ਬਹੁਲਕ(hstdin, &ਬਹੁਲਕ))
	// Check if it is opened as input.
	ਵਾਪਸ Get_ਸੰਖਿਆ_ਦਾ_Console_ਨਿਵੇਸ਼_Events(hstdin, &eventnum);

    ਵਾਪਸ _fstat(fd, &st) == 0;
}

// Python 3.5 or later will abort inside Py_Initialize() when stdin has
// been closed (i.e. executed by "vim -").  Reconnect stdin to CONIN$.
// Note that the python DLL is linked to its own stdio DLL which can be
// differ from Vim's stdio.
    ਸਥਿਰ ਖਾਲੀ
reset_stdin(ਖਾਲੀ)
{
    FILE *(*py__acrt_iob_func)(ਬਿਨਾਨਿਸ਼ਾਨੀ) = ਨਿਰਾ;
    FILE *(*pyfreopen)(ਅਚਲ ਅੱਖਰ *, ਅਚਲ ਅੱਖਰ *, FILE *) = ਨਿਰਾ;
    HINSTANCE hinst = get_forwarded_dll(hinstPy3);

    ਜੇ (hinst == ਨਿਰਾ || is_stdin_readable())
	ਵਾਪਸ;

    // Get "freopen" and "stdin" which are used in the python DLL.
    // "stdin" is defined as "__acrt_iob_func(0)" in VC++ 2015 or later.
    py__acrt_iob_func = get_dll_import_func(hinst, "__acrt_iob_func");
    ਜੇ (py__acrt_iob_func)
    {
	HINSTANCE hpystdiodll = ਲੱਭੋ_imported_module_by_funcname(hinst,
							    "__acrt_iob_func");
	ਜੇ (hpystdiodll)
	    pyfreopen = (ਖਾਲੀ *)Get_Proc_ਪਤਾ(hpystdiodll, "freopen");
    }

    // Reconnect stdin to CONIN$.
    ਜੇ (pyfreopen != ਨਿਰਾ)
	pyfreopen("CONIN$", "r", py__acrt_iob_func(0));
    ਨਹੀਂਤਾਂ
	freopen("CONIN$", "r", stdin);
}
#ਨਹੀਂਤਾਂ
# define reset_stdin()
#ਜੇ_ਅੰਤ

// Python 3.2 or later will abort inside Py_Initialize() when mandatory
// modules cannot be loaded (e.g. 'pythonthreehome' is wrongly set.).
// Install a hook to python dll's exit() and recover from it.
#ਜੇ defined(MSWIN) && (PY_ਸੰਸਕਰਣ_HEX >= 0x030200f0)
# define HOOK_EXIT
# include <setjmp.h>

ਸਥਿਰ jmp_buf exit_hook_jump_buf;
ਸਥਿਰ ਖਾਲੀ *orig_exit = ਨਿਰਾ;

/*
 * Function that replaces exit() while calling Py_Initialize().
 */
    ਸਥਿਰ ਖਾਲੀ
hooked_exit(ਪੂਰਨ ret)
{
    // Recover from exit.
    longjmp(exit_hook_jump_buf, 1);
}

/*
 * Install a hook to python dll's exit().
 */
    ਸਥਿਰ ਖਾਲੀ
hook_py_exit(ਖਾਲੀ)
{
    HINSTANCE hinst = get_forwarded_dll(hinstPy3);

    ਜੇ (hinst == ਨਿਰਾ || orig_exit != ਨਿਰਾ)
	ਵਾਪਸ;

    orig_exit = hook_dll_import_func(hinst, "exit", (ਖਾਲੀ *)hooked_exit);
}

/*
 * Remove the hook installed by hook_py_exit().
 */
    ਸਥਿਰ ਖਾਲੀ
restore_py_exit(ਖਾਲੀ)
{
    HINSTANCE hinst = hinstPy3;

    ਜੇ (hinst == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (orig_exit != ਨਿਰਾ)
	hook_dll_import_func(hinst, "exit", orig_exit);
    orig_exit = ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

    ਸਥਿਰ ਪੂਰਨ
Python3_Init(ਖਾਲੀ)
{
    ਜੇ (!py3initialised)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DYNAMIC_PYTHON3
	ਜੇ (!python3_enabled(TRUE))
	{
	    emsg(_(e_sorry_this_command_is_disabled_python_library_could_not_be_ਮਿਲਿਆ));
	    ਜਾਓ fail;
	}
#ਜੇ_ਅੰਤ
	// Python 3.13 introduced a really useful feature: colorized exceptions.
	// This is great if you're reading them from the terminal, but useless
	// and broken everywhere else (such as in log files, or text editors).
	// Opt out, forcefully.
	vim_setenv((char_u*)"PYTHON_COLORS", (char_u*)"0");

	init_structs();

	ਜੇ (*p_py3home != NUL)
	{
	    size_t len = mbstowcs(ਨਿਰਾ, (ਅੱਖਰ *)p_py3home, 0) + 1;

	    // The string must not change later, make a copy in static memory.
	    py_home_buf = ALLOC_MULT(wchar_t, len);
	    ਜੇ (py_home_buf != ਨਿਰਾ && mbstowcs(
			    py_home_buf, (ਅੱਖਰ *)p_py3home, len) != (size_t)-1)
		Py_ਸੈੱਟ_Python_Home(py_home_buf);
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ PYTHON3_HOME
	ਨਹੀਂਤਾਂ ਜੇ (mch_getenv((char_u *)"PYTHONHOME") == ਨਿਰਾ)
	    Py_ਸੈੱਟ_Python_Home(PYTHON3_HOME);
#ਜੇ_ਅੰਤ

	PyImport_AppendInittab("vim", Py3Init_vim);

#ਜੇ !defined(DYNAMIC_PYTHON3) && defined(MSWIN)
	hinstPy3 = GetModuleHandle(PYTHON3_DLL);
#ਜੇ_ਅੰਤ
	reset_stdin();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HOOK_EXIT
	// Catch exit() called in Py_Initialize().
	hook_py_exit();
	ਜੇ (setjmp(exit_hook_jump_buf) == 0)
	{
	    Py_Initialize();
	    restore_py_exit();
	}
	ਨਹੀਂਤਾਂ
	{
	    // exit() was called in Py_Initialize().
	    restore_py_exit();
	    emsg(_(e_critical_error_ਵਿੱਚ_python3_initialization_check_your_installation));
	    ਜਾਓ fail;
	}
#ਨਹੀਂਤਾਂ
	Py_Initialize();
#ਜੇ_ਅੰਤ

#ਜੇ PY_ਸੰਸਕਰਣ_HEX < 0x03090000
	// Initialise threads.  This is deprecated since Python 3.9.
	Py_Eval_Init_ਧਾਗੇ();
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DYNAMIC_PYTHON3
	get_py3_exceptions();
#ਜੇ_ਅੰਤ

	ਜੇ (PythonIO_Init_io())
	    ਜਾਓ fail;

	globals = PyModule_GetDict(Py_Import_ਜੋੜੋ_Module("__main__"));

	// Remove the element from sys.path that was added because of our
	// argv[0] value in Py3Init_vim().  Previously we used an empty
	// string, but depending on the OS we then get an empty entry or
	// the current directory in sys.path.
	// Only after vim has been imported, the element does exist in
	// sys.path.
	Py_ਲੜੀਵਾਰ_Simple_String("import vim; import sys; sys.path = list(filter(lambda x: not x.endswith('must>not&exist'), sys.path))");

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
DoPyCommand(ਅਚਲ ਅੱਖਰ *cmd,
	    dict_T* locals,
	    rangeinitializer init_range,
	    runner ਲੜੀਵਾਰ,
	    ਖਾਲੀ *arg)
{
#ਜੇ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    ਅੱਖਰ		*saved_locale;
#ਜੇ_ਅੰਤ
    PyObject		*cmdstr;
    PyObject		*cmdbytes;
    PyGILState_STATE	pygilstate;

    ਜੇ (python_ਸਿਰਾ_called)
	ਜਾਓ theend;

    ਜੇ (Python3_Init())
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
	saved_locale = (ਅੱਖਰ *)vim_strsave((char_u *)saved_locale);
	(ਖਾਲੀ)setlocale(LC_NUMERIC, "C");
    }
#ਜੇ_ਅੰਤ

    pygilstate = PyGILState_Ensure();

    // PyRun_SimpleString expects a UTF-8 string. Wrong encoding may cause
    // SyntaxError (unicode error).
    cmdstr = PyUnicode_Decode(cmd, ਲੜੀ_ਲੰਬਾਈ(cmd),
					(ਅੱਖਰ *)ENC_OPT, ERRORS_DECODE_ARG);
    cmdbytes = PyUnicode_AsEncodedString(cmdstr, "utf-8", ERRORS_ENCODE_ARG);
    Py_XDECREF(cmdstr);

    ਲੜੀਵਾਰ(Py_ਬਾਈਟਾਂ_As_String(cmdbytes), locals, arg, &pygilstate);
    Py_XDECREF(cmdbytes);

    PyGILState_Release(pygilstate);

#ਜੇ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    ਜੇ (saved_locale != ਨਿਰਾ)
    {
	(ਖਾਲੀ)setlocale(LC_NUMERIC, saved_locale);
	vim_free(saved_locale);
    }
#ਜੇ_ਅੰਤ

    Python_ਤਾਲਾ_Vim();		    // enter Vim
    PythonIO_Flush();

theend:
    ਵਾਪਸ;	    // keeps lint happy
}

/*
 * ":py3"
 */
    ਖਾਲੀ
ex_py3(exarg_T *eap)
{
    char_u *script;

    script = script_get(eap, eap->arg);
    ਜੇ (!eap->skip)
    {
	ਜੇ (p_pyx == 0)
	    p_pyx = 3;

	DoPyCommand(script == ਨਿਰਾ ? (ਅੱਖਰ *) eap->arg : (ਅੱਖਰ *) script,
		ਨਿਰਾ,
		init_range_cmd,
		(runner) ਲੜੀਵਾਰ_cmd,
		(ਖਾਲੀ *) eap);
    }
    vim_free(script);
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਬਫਰ_ਮਾਪ 2048

/*
 * ":py3file"
 */
    ਖਾਲੀ
ex_py3file(exarg_T *eap)
{
    ਸਥਿਰ ਅੱਖਰ ਬਫਰ[ਬਫਰ_ਮਾਪ];
    ਅਚਲ ਅੱਖਰ *file;
    ਅੱਖਰ *p;
    ਪੂਰਨ i;

    ਜੇ (p_pyx == 0)
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

    strcpy(ਬਫਰ, "exec(compile(open('");
    p = ਬਫਰ + 19; // size of "exec(compile(open('"

    ਲਈ (i=0; i<2; ++i)
    {
	file = (ਅੱਖਰ *)eap->arg;
	ਜਦੋਂ (*file && p < ਬਫਰ + (ਬਫਰ_ਮਾਪ - 3))
	{
	    ਜੇ (*file == '\\' || *file == '\'')
		*p++ = '\\';
	    *p++ = *file++;
	}
	// If we didn't finish the file name, we hit a buffer overflow
	ਜੇ (*file != '\0')
	    ਵਾਪਸ;
	ਜੇ (i==0)
	{
	    strcpy(p,"','rb').read(),'");
	    p += 16;
	}
	ਨਹੀਂਤਾਂ
	{
	    strcpy(p,"','exec'))");
	    p += 10;
	}
    }


    // Execute the file
    DoPyCommand(ਬਫਰ,
	    ਨਿਰਾ,
	    init_range_cmd,
	    (runner) ਲੜੀਵਾਰ_cmd,
	    (ਖਾਲੀ *) eap);
}

    ਖਾਲੀ
ex_py3do(exarg_T *eap)
{
    ਜੇ (p_pyx == 0)
	p_pyx = 3;

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
OutputGetattro(PyObject *self, PyObject *nameobj)
{
    GET_ATTR_STRING(name, nameobj);

    ਜੇ (strcmp(name, "softspace") == 0)
	ਵਾਪਸ PyLong_FromLong(((OutputObject *)(self))->softspace);
    ਨਹੀਂਤਾਂ ਜੇ (strcmp(name, "errors") == 0)
	ਵਾਪਸ PyString_FromString("strict");
    ਨਹੀਂਤਾਂ ਜੇ (strcmp(name, "encoding") == 0)
	ਵਾਪਸ PyString_FromString(ENC_OPT);

    ਵਾਪਸ PyObject_GenericGetAttr(self, nameobj);
}

    ਸਥਿਰ ਪੂਰਨ
OutputSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);

    ਵਾਪਸ OutputSetattr(self, name, val);
}

///////////////////////////////////////////////////////
// 3. Implementation of the Vim module for Python

// Window type - Implementation functions
// --------------------------------------

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਖਿੜਕੀ_Type_Check(obj) ((obj)->ob_ਅਧਾਰ.ob_type == &ਖਿੜਕੀ_Type)

// Buffer type - Implementation functions
// --------------------------------------

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਬਫਰ_Type_Check(obj) ((obj)->ob_ਅਧਾਰ.ob_type == &ਬਫਰ_Type)

ਸਥਿਰ PyObject* ਬਫਰ_Subscript(PyObject *self, PyObject *idx);
ਸਥਿਰ ਪੂਰਨ ਬਫਰ_As_Subscript(PyObject *self, PyObject *idx, PyObject *val);

// Line range type - Implementation functions
// --------------------------------------

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RangeType_Check(obj) ((obj)->ob_ਅਧਾਰ.ob_type == &RangeType)

ਸਥਿਰ PyObject* RangeSubscript(PyObject *self, PyObject *idx);
ਸਥਿਰ ਪੂਰਨ Range_As_ਵਸਤੂ(PyObject *, Py_ssize_t, PyObject *);
ਸਥਿਰ ਪੂਰਨ RangeAsSubscript(PyObject *self, PyObject *idx, PyObject *val);

// Current objects type - Implementation functions
// -----------------------------------------------

ਸਥਿਰ PySequenceMethods ਬਫਰ_As_Seq = {
    (lenfunc)		ਬਫਰ_ਲੰਮਾਈ,	    // sq_length,    len(x)
    (binaryfunc)	0,		    // sq_concat,    x+y
    (ssizeargfunc)	0,		    // sq_repeat,    x*n
    (ssizeargfunc)	ਬਫਰ_ਵਸਤੂ,	    // sq_item,      x[i]
    0,					    // was_sq_slice,	 x[i:j]
    0,					    // sq_ass_item,  x[i]=v
    0,					    // sq_ass_slice, x[i:j]=v
    0,					    // sq_contains
    0,					    // sq_inplace_concat
    0,					    // sq_inplace_repeat
};

ਸਥਿਰ PyMappingMethods ਬਫਰ_As_Mapping = {
    /* mp_length	*/ (lenfunc)ਬਫਰ_ਲੰਮਾਈ,
    /* mp_subscript     */ (binaryfunc)ਬਫਰ_Subscript,
    /* mp_ass_subscript */ (objobjargproc)ਬਫਰ_As_Subscript,
};


// Buffer object

    ਸਥਿਰ PyObject *
ਬਫਰ_Getattro(PyObject *self, PyObject *nameobj)
{
    PyObject *r;

    GET_ATTR_STRING(name, nameobj);

    ਜੇ ((r = ਬਫਰ_Attr_Valid((ਬਫਰ_Object *)(self), name)))
	ਵਾਪਸ r;

    ਜੇ (Check_ਬਫਰ((ਬਫਰ_Object *)(self)))
	ਵਾਪਸ ਨਿਰਾ;

    r = ਬਫਰ_Attr((ਬਫਰ_Object *)(self), name);
    ਜੇ (r || PyErr_Occurred())
	ਵਾਪਸ r;
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ PyObject_GenericGetAttr(self, nameobj);
}

    ਸਥਿਰ ਪੂਰਨ
ਬਫਰ_Setattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);

    ਵਾਪਸ ਬਫਰ_Setattr(self, name, val);
}

//////////////////

    ਸਥਿਰ PyObject *
ਬਫਰ_Subscript(PyObject *self, PyObject* idx)
{
    ਜੇ (PyLong_Check(idx))
    {
	ਲੰਮਾ _idx = PyLong_AsLong(idx);
	ਵਾਪਸ ਬਫਰ_ਵਸਤੂ((ਬਫਰ_Object *)(self), _idx);
    }
    ਨਹੀਂਤਾਂ ਜੇ (Py_ਟੁਕੜਾ_Check(idx))
    {
	Py_ssize_t ਸ਼ੁਰੂ, stop, ਕਦਮ, slicelen;

	ਜੇ (Check_ਬਫਰ((ਬਫਰ_Object *) self))
	    ਵਾਪਸ ਨਿਰਾ;

	ਜੇ (Py_ਟੁਕੜਾ_Get_Indices_Ex((Py_ਟੁਕੜਾ_Object_T *)idx,
	      (Py_ssize_t)((ਬਫਰ_Object *)(self))->buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ,
	      &ਸ਼ੁਰੂ, &stop,
	      &ਕਦਮ, &slicelen) < 0)
	    ਵਾਪਸ ਨਿਰਾ;
	ਵਾਪਸ ਬਫਰ_ਟੁਕੜਾ((ਬਫਰ_Object *)(self), ਸ਼ੁਰੂ, stop);
    }
    ਨਹੀਂਤਾਂ
    {
	RAISE_INVALID_ਸੂਚਕਾਂਕ_TYPE(idx);
	ਵਾਪਸ ਨਿਰਾ;
    }
}

    ਸਥਿਰ ਪੂਰਨ
ਬਫਰ_As_Subscript(PyObject *self, PyObject* idx, PyObject* val)
{
    ਜੇ (PyLong_Check(idx))
    {
	ਲੰਮਾ n = PyLong_AsLong(idx);

	ਜੇ (Check_ਬਫਰ((ਬਫਰ_Object *) self))
	    ਵਾਪਸ -1;

	ਵਾਪਸ RBAs_ਵਸਤੂ((ਬਫਰ_Object *)(self), n, val, 1,
		    (Py_ssize_t)((ਬਫਰ_Object *)(self))->buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ,
		    ਨਿਰਾ);
    }
    ਨਹੀਂਤਾਂ ਜੇ (Py_ਟੁਕੜਾ_Check(idx))
    {
	Py_ssize_t ਸ਼ੁਰੂ, stop, ਕਦਮ, slicelen;

	ਜੇ (Check_ਬਫਰ((ਬਫਰ_Object *) self))
	    ਵਾਪਸ -1;

	ਜੇ (Py_ਟੁਕੜਾ_Get_Indices_Ex((Py_ਟੁਕੜਾ_Object_T *)idx,
	      (Py_ssize_t)((ਬਫਰ_Object *)(self))->buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ,
	      &ਸ਼ੁਰੂ, &stop,
	      &ਕਦਮ, &slicelen) < 0)
	    ਵਾਪਸ -1;
	ਵਾਪਸ RBAs_ਟੁਕੜਾ((ਬਫਰ_Object *)(self), ਸ਼ੁਰੂ, stop, val, 1,
			  (PyInt)((ਬਫਰ_Object *)(self))->buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ,
			  ਨਿਰਾ);
    }
    ਨਹੀਂਤਾਂ
    {
	RAISE_INVALID_ਸੂਚਕਾਂਕ_TYPE(idx);
	ਵਾਪਸ -1;
    }
}

ਸਥਿਰ PySequenceMethods RangeAsSeq = {
    (lenfunc)		Range_ਲੰਮਾਈ,	 // sq_length,	  len(x)
    (binaryfunc)	0,		 // RangeConcat, sq_concat,  x+y
    (ssizeargfunc)	0,		 // RangeRepeat, sq_repeat,  x*n
    (ssizeargfunc)	Range_ਵਸਤੂ,	 // sq_item,	  x[i]
    0,					 // was_sq_slice,     x[i:j]
    (ssizeobjargproc)	Range_As_ਵਸਤੂ,	 // sq_as_item,  x[i]=v
    0,					 // sq_ass_slice, x[i:j]=v
    0,					 // sq_contains
    0,					 // sq_inplace_concat
    0,					 // sq_inplace_repeat
};

ਸਥਿਰ PyMappingMethods RangeAsMapping = {
    /* mp_length	*/ (lenfunc)Range_ਲੰਮਾਈ,
    /* mp_subscript     */ (binaryfunc)RangeSubscript,
    /* mp_ass_subscript */ (objobjargproc)RangeAsSubscript,
};

// Line range object - Implementation

    ਸਥਿਰ PyObject *
RangeGetattro(PyObject *self, PyObject *nameobj)
{
    GET_ATTR_STRING(name, nameobj);

    ਜੇ (strcmp(name, "start") == 0)
	ਵਾਪਸ Py_Build_ਮੁੱਲ("n", ((RangeObject *)(self))->ਸ਼ੁਰੂ - 1);
    ਨਹੀਂਤਾਂ ਜੇ (strcmp(name, "end") == 0)
	ਵਾਪਸ Py_Build_ਮੁੱਲ("n", ((RangeObject *)(self))->ਸਿਰਾ - 1);
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ PyObject_GenericGetAttr(self, nameobj);
}

////////////////

    ਸਥਿਰ ਪੂਰਨ
Range_As_ਵਸਤੂ(PyObject *self, Py_ssize_t n, PyObject *val)
{
    ਵਾਪਸ RBAs_ਵਸਤੂ(((RangeObject *)(self))->buf, n, val,
		    ((RangeObject *)(self))->ਸ਼ੁਰੂ,
		    ((RangeObject *)(self))->ਸਿਰਾ,
		    &((RangeObject *)(self))->ਸਿਰਾ);
}

    ਸਥਿਰ Py_ssize_t
Range_As_ਟੁਕੜਾ(PyObject *self, Py_ssize_t lo, Py_ssize_t hi, PyObject *val)
{
    ਵਾਪਸ RBAs_ਟੁਕੜਾ(((RangeObject *)(self))->buf, lo, hi, val,
		    ((RangeObject *)(self))->ਸ਼ੁਰੂ,
		    ((RangeObject *)(self))->ਸਿਰਾ,
		    &((RangeObject *)(self))->ਸਿਰਾ);
}

    ਸਥਿਰ PyObject *
RangeSubscript(PyObject *self, PyObject* idx)
{
    ਜੇ (PyLong_Check(idx))
    {
	ਲੰਮਾ _idx = PyLong_AsLong(idx);
	ਵਾਪਸ Range_ਵਸਤੂ((RangeObject *)(self), _idx);
    }
    ਨਹੀਂਤਾਂ ਜੇ (Py_ਟੁਕੜਾ_Check(idx))
    {
	Py_ssize_t ਸ਼ੁਰੂ, stop, ਕਦਮ, slicelen;

	ਜੇ (Py_ਟੁਕੜਾ_Get_Indices_Ex((Py_ਟੁਕੜਾ_Object_T *)idx,
		((RangeObject *)(self))->ਸਿਰਾ-((RangeObject *)(self))->ਸ਼ੁਰੂ+1,
		&ਸ਼ੁਰੂ, &stop,
		&ਕਦਮ, &slicelen) < 0)
	    ਵਾਪਸ ਨਿਰਾ;
	ਵਾਪਸ Range_ਟੁਕੜਾ((RangeObject *)(self), ਸ਼ੁਰੂ, stop);
    }
    ਨਹੀਂਤਾਂ
    {
	RAISE_INVALID_ਸੂਚਕਾਂਕ_TYPE(idx);
	ਵਾਪਸ ਨਿਰਾ;
    }
}

    ਸਥਿਰ ਪੂਰਨ
RangeAsSubscript(PyObject *self, PyObject *idx, PyObject *val)
{
    ਜੇ (PyLong_Check(idx))
    {
	ਲੰਮਾ n = PyLong_AsLong(idx);
	ਵਾਪਸ Range_As_ਵਸਤੂ(self, n, val);
    }
    ਨਹੀਂਤਾਂ ਜੇ (Py_ਟੁਕੜਾ_Check(idx))
    {
	Py_ssize_t ਸ਼ੁਰੂ, stop, ਕਦਮ, slicelen;

	ਜੇ (Py_ਟੁਕੜਾ_Get_Indices_Ex((Py_ਟੁਕੜਾ_Object_T *)idx,
		((RangeObject *)(self))->ਸਿਰਾ-((RangeObject *)(self))->ਸ਼ੁਰੂ+1,
		&ਸ਼ੁਰੂ, &stop,
		&ਕਦਮ, &slicelen) < 0)
	    ਵਾਪਸ -1;
	ਵਾਪਸ Range_As_ਟੁਕੜਾ(self, ਸ਼ੁਰੂ, stop, val);
    }
    ਨਹੀਂਤਾਂ
    {
	RAISE_INVALID_ਸੂਚਕਾਂਕ_TYPE(idx);
	ਵਾਪਸ -1;
    }
}

// TabPage object - Implementation

    ਸਥਿਰ PyObject *
Tab_ਪੰਨਾ_Getattro(PyObject *self, PyObject *nameobj)
{
    PyObject *r;

    GET_ATTR_STRING(name, nameobj);

    ਜੇ ((r = Tab_ਪੰਨਾ_Attr_Valid((Tab_ਪੰਨਾ_Object *)(self), name)))
	ਵਾਪਸ r;

    ਜੇ (Check_Tab_ਪੰਨਾ((Tab_ਪੰਨਾ_Object *)(self)))
	ਵਾਪਸ ਨਿਰਾ;

    r = Tab_ਪੰਨਾ_Attr((Tab_ਪੰਨਾ_Object *)(self), name);
    ਜੇ (r || PyErr_Occurred())
	ਵਾਪਸ r;
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ PyObject_GenericGetAttr(self, nameobj);
}

// Window object - Implementation

    ਸਥਿਰ PyObject *
ਖਿੜਕੀ_Getattro(PyObject *self, PyObject *nameobj)
{
    PyObject *r;

    GET_ATTR_STRING(name, nameobj);

    ਜੇ ((r = ਖਿੜਕੀ_Attr_Valid((ਖਿੜਕੀ_Object *)(self), name)))
	ਵਾਪਸ r;

    ਜੇ (Check_ਖਿੜਕੀ((ਖਿੜਕੀ_Object *)(self)))
	ਵਾਪਸ ਨਿਰਾ;

    r = ਖਿੜਕੀ_Attr((ਖਿੜਕੀ_Object *)(self), name);
    ਜੇ (r || PyErr_Occurred())
	ਵਾਪਸ r;
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ PyObject_GenericGetAttr(self, nameobj);
}

    ਸਥਿਰ ਪੂਰਨ
ਖਿੜਕੀ_Setattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);

    ਵਾਪਸ ਖਿੜਕੀ_Setattr(self, name, val);
}

// Tab page list object - Definitions

ਸਥਿਰ PySequenceMethods TabListAsSeq = {
    (lenfunc)	     Tab_List_ਲੰਮਾਈ,	    // sq_length,    len(x)
    (binaryfunc)     0,			    // sq_concat,    x+y
    (ssizeargfunc)   0,			    // sq_repeat,    x*n
    (ssizeargfunc)   Tab_List_ਵਸਤੂ,	    // sq_item,      x[i]
    0,					    // sq_slice,     x[i:j]
    (ssizeobjargproc)0,			    // sq_as_item,  x[i]=v
    0,					    // sq_ass_slice, x[i:j]=v
    0,					    // sq_contains
    0,					    // sq_inplace_concat
    0,					    // sq_inplace_repeat
};

// Window list object - Definitions

ਸਥਿਰ PySequenceMethods WinListAsSeq = {
    (lenfunc)	     Win_List_ਲੰਮਾਈ,	    // sq_length,    len(x)
    (binaryfunc)     0,			    // sq_concat,    x+y
    (ssizeargfunc)   0,			    // sq_repeat,    x*n
    (ssizeargfunc)   Win_List_ਵਸਤੂ,	    // sq_item,      x[i]
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
    ਸਥਿਰ PyObject *
CurrentGetattro(PyObject *self, PyObject *nameobj)
{
    PyObject	*r;
    GET_ATTR_STRING(name, nameobj);
    ਜੇ (!(r = CurrentGetattr(self, name)))
	ਵਾਪਸ PyObject_GenericGetAttr(self, nameobj);
    ਵਾਪਸ r;
}

    ਸਥਿਰ ਪੂਰਨ
CurrentSetattro(PyObject *self, PyObject *nameobj, PyObject *ਮੁੱਲ)
{
    GET_ATTR_STRING(name, nameobj);
    ਵਾਪਸ CurrentSetattr(self, name, ਮੁੱਲ);
}

// Dictionary object - Definitions

    ਸਥਿਰ PyObject *
DictionaryGetattro(PyObject *self, PyObject *nameobj)
{
    DictionaryObject	*this = ((DictionaryObject *) (self));

    GET_ATTR_STRING(name, nameobj);

    ਜੇ (strcmp(name, "locked") == 0)
	ਵਾਪਸ PyLong_FromLong(this->dict->dv_ਤਾਲਾ);
    ਨਹੀਂਤਾਂ ਜੇ (strcmp(name, "scope") == 0)
	ਵਾਪਸ PyLong_FromLong(this->dict->dv_scope);

    ਵਾਪਸ PyObject_GenericGetAttr(self, nameobj);
}

    ਸਥਿਰ ਪੂਰਨ
DictionarySetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);
    ਵਾਪਸ DictionarySetattr(self, name, val);
}

// List object - Definitions

    ਸਥਿਰ PyObject *
ListGetattro(PyObject *self, PyObject *nameobj)
{
    GET_ATTR_STRING(name, nameobj);

    ਜੇ (strcmp(name, "locked") == 0)
	ਵਾਪਸ PyLong_FromLong(((ListObject *) (self))->list->lv_ਤਾਲਾ);

    ਵਾਪਸ PyObject_GenericGetAttr(self, nameobj);
}

    ਸਥਿਰ ਪੂਰਨ
ListSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);
    ਵਾਪਸ ListSetattr(self, name, val);
}

// Tuple object - Definitions

    ਸਥਿਰ PyObject *
TupleGetattro(PyObject *self, PyObject *nameobj)
{
    GET_ATTR_STRING(name, nameobj);

    ਜੇ (strcmp(name, "locked") == 0)
	ਵਾਪਸ PyLong_FromLong(((TupleObject *) (self))->tuple->tv_ਤਾਲਾ);

    ਵਾਪਸ PyObject_GenericGetAttr(self, nameobj);
}

    ਸਥਿਰ ਪੂਰਨ
TupleSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);
    ਵਾਪਸ TupleSetattr(self, name, val);
}

// Function object - Definitions

    ਸਥਿਰ PyObject *
FunctionGetattro(PyObject *self, PyObject *nameobj)
{
    PyObject		*r;
    FunctionObject	*this = (FunctionObject *)(self);

    GET_ATTR_STRING(name, nameobj);

    r = FunctionAttr(this, name);
    ਜੇ (r || PyErr_Occurred())
	ਵਾਪਸ r;
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ PyObject_GenericGetAttr(self, nameobj);
}

// External interface

    ਖਾਲੀ
python3_ਬਫਰ_free(buf_T *buf)
{
    ਬਫਰ_Object *bp = BUF_PYTHON_REF(buf);
    ਜੇ (bp == ਨਿਰਾ)
	ਵਾਪਸ;
    bp->buf = INVALID_ਬਫਰ_ਮੁੱਲ;
    BUF_PYTHON_REF(buf) = ਨਿਰਾ;
}

    ਖਾਲੀ
python3_ਖਿੜਕੀ_free(win_T *win)
{
    ਖਿੜਕੀ_Object *wp = WIN_PYTHON_REF(win);
    ਜੇ (wp == ਨਿਰਾ)
	ਵਾਪਸ;
    wp->win = INVALID_ਖਿੜਕੀ_ਮੁੱਲ;
    WIN_PYTHON_REF(win) = ਨਿਰਾ;
}

    ਖਾਲੀ
python3_tabpage_free(tabpage_T *tab)
{
    Tab_ਪੰਨਾ_Object *tp = TAB_PYTHON_REF(tab);
    ਜੇ (tp == ਨਿਰਾ)
	ਵਾਪਸ;
    tp->tab = INVALID_TABPAGE_ਮੁੱਲ;
    TAB_PYTHON_REF(tab) = ਨਿਰਾ;
}

    ਸਥਿਰ PyObject *
Py3Init_vim(ਖਾਲੀ)
{
    // The special value is removed from sys.path in Python3_Init().
    ਸਥਿਰ wchar_t *(argv[2]) = {L"/must>not&exist/foo", ਨਿਰਾ};

    ਜੇ (init_types())
	ਵਾਪਸ ਨਿਰਾ;

    // Set sys.argv[] to avoid a crash in warn().
    Py_Sys_ਸੈੱਟ_Argv(1, argv);

    ਜੇ ((vim_module = PyModule_Create(&vimmodule)) == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (populate_module(vim_module))
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (init_sys_ਰਸਤਾ())
	ਵਾਪਸ ਨਿਰਾ;

    ਵਾਪਸ vim_module;
}

//////////////////////////////////////////////////////////////////////////
// 4. Utility functions for handling the interface between Vim and Python.

/*
 * Convert a Vim line into a Python string.
 * All internal newlines are replaced by null characters.
 *
 * On errors, the Python exception data is set, and NULL is returned.
 */
    ਸਥਿਰ PyObject *
ਸਤਰ_ਮੰਜ਼ਿਲ_String(ਅਚਲ ਅੱਖਰ *str)
{
    PyObject *ਨਤੀਜਾ;
    Py_ssize_t len = ਲੜੀ_ਲੰਬਾਈ(str);
    ਅੱਖਰ *tmp, *p;

    tmp = alloc(len + 1);
    p = tmp;
    ਜੇ (p == ਨਿਰਾ)
    {
	Py_Err_No_ਸਮ੍ਰਿਤੀ();
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਜਦੋਂ (*str)
    {
	ਜੇ (*str == '\n')
	    *p = '\0';
	ਨਹੀਂਤਾਂ
	    *p = *str;

	++p;
	++str;
    }
    *p = '\0';

    ਨਤੀਜਾ = PyUnicode_Decode(tmp, len, (ਅੱਖਰ *)ENC_OPT, ERRORS_DECODE_ARG);

    vim_free(tmp);
    ਵਾਪਸ ਨਤੀਜਾ;
}

    ਖਾਲੀ
do_py3eval(char_u *str, dict_T *locals, typval_T *rettv)
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
ਸੈੱਟ_ref_ਵਿੱਚ_python3(ਪੂਰਨ copyID)
{
    ਵਾਪਸ ਸੈੱਟ_ref_ਵਿੱਚ_py(copyID);
}

    ਪੂਰਨ
python3_ਸੰਸਕਰਣ(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_LIMITED_API
    ਵਾਪਸ Py_LIMITED_API;
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ PY_ਸੰਸਕਰਣ_HEX;
#ਜੇ_ਅੰਤ
}
