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

#شامل "vim.h"

#شامل <limits.h>

#اگر defined(MSWIN) && defined(HAVE_FCNTL_H)
# undef HAVE_FCNTL_H
#اگر_ختم

#اگر_تعریف_شدہ _DEBUG
# undef _DEBUG
#اگر_ختم

#اگر_تعریف_شدہ F_BLANK
# undef F_BLANK
#اگر_ختم

#اگر_تعریف_شدہ HAVE_DUP
# undef HAVE_DUP
#اگر_ختم
#اگر_تعریف_شدہ HAVE_STRFTIME
# undef HAVE_STRFTIME
#اگر_ختم
#اگر_تعریف_شدہ HAVE_STRING_H
# undef HAVE_STRING_H
#اگر_ختم
#اگر_تعریف_شدہ HAVE_PUTENV
# undef HAVE_PUTENV
#اگر_ختم
#اگر_تعریف_شدہ _POSIX_C_SOURCE  // defined in feature.h
# undef _POSIX_C_SOURCE
#اگر_ختم
#اگر_تعریف_شدہ XOPEN_ماخذ
# undef XOPEN_ماخذ	// pyconfig.h defines it as well.
#اگر_ختم

#تعریف_کرو PY_SSIZE_T_CLEAN

#اگر_تعریف_شدہ Py_LIMITED_API
# define USE_LIMITED_API // Using Python 3 limited ABI
#اگر_ختم

#شامل <Python.h>

#تعریف_منسوخ مین // Defined in python.h - aargh
#تعریف_منسوخ HAVE_FCNTL_H // Clash with os_win32.h

// The "surrogateescape" error handler is new in Python 3.1
#اگر PY_نسخہ_HEX >= 0x030100f0
# define CODEC_ERROR_HANDLER "surrogateescape"
#ورنہ
# define CODEC_ERROR_HANDLER تہی
#اگر_ختم

// Suppress Python 3.11 depreciations to see useful warnings
#اگر_تعریف_شدہ __GNUC__
# pragma GCC diagnostic ڈالو
# pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#اگر_ختم

// Python 3 does not support CObjects, always use Capsules
#تعریف_کرو PY_USE_CAPSULE

#تعریف_کرو ERRORS_DECODE_ARG CODEC_ERROR_HANDLER
#تعریف_کرو ERRORS_ENCODE_ARG ERRORS_DECODE_ARG

#تعریف_کرو PyInt Py_ssize_t
#اگر_غیر_تعریف_شدہ PyString_Check
# define PyString_Check(obj) PyUnicode_Check(obj)
#اگر_ختم
#تعریف_کرو PyString_FromString(repr) \
    PyUnicode_Decode(repr, STRLEN(repr), ENC_OPT, ERRORS_DECODE_ARG)
#تعریف_کرو PyString_FromFormat PyUnicode_FromFormat
#اگر_تعریف_شدہ PyUnicode_FromFormat
# define Py_UNICODE_USE_UCS_FUNCTIONS
#اگر_ختم
#اگر_غیر_تعریف_شدہ PyInt_Check
# define PyInt_Check(obj) PyLong_Check(obj)
#اگر_ختم
#تعریف_کرو PyInt_FromLong(i) PyLong_FromLong(i)
#تعریف_کرو PyInt_AsLong(obj) PyLong_AsLong(obj)
#تعریف_کرو Py_ssize_t_fmt "n"
#تعریف_کرو Py_بائٹیں_fmt "y"

#تعریف_کرو PyIntArgFunc	ssizeargfunc
#تعریف_کرو PyIntObjArgProc	ssizeobjargproc

/*
 * PySlice_GetIndicesEx(): first argument type changed from PySliceObject
 * to PyObject in Python 3.2 or later.
 */
#اگر PY_نسخہ_HEX >= 0x030200f0
قسمتعریف PyObject Py_قاش_Object_T;
#ورنہ
قسمتعریف Py_قاش_Object Py_قاش_Object_T;
#اگر_ختم

#اگر_غیر_تعریف_شدہ MSWIN
# define HINSTANCE خالی *
#اگر_ختم
#اگر defined(DYNAMIC_PYTHON3) || defined(MSWIN)
ساکن HINSTANCE hinstPy3 = 0; // Instance of python.dll
#اگر_ختم

#اگر defined(DYNAMIC_PYTHON3)

# ifdef MSWIN
#  define بھرو_dll vim_بھرو_Lib
#  define close_dll FreeLibrary
#  define علامت_from_dll Get_Proc_پتہ
#  define بھرو_dll_error GetWin32Error
# ورنہ
#  include <dlfcn.h>
#  define FARPROC خالی*
#  اگر defined(PY_NO_RTLD_GLOBAL) && defined(PY3_NO_RTLD_GLOBAL)
#   define بھرو_dll(n) dlopen((n), RTLD_LAZY)
#  ورنہ
#   define بھرو_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#  endif
#  define close_dll dlclose
#  define علامت_from_dll dlsym
#  define بھرو_dll_error dlerror
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
# define Py_Dict_سیٹ_شے_String py3_Py_Dict_سیٹ_شے_String
# define Py_Err_Bad_دلیل py3_Py_Err_Bad_دلیل
# define PyErr_Clear py3_PyErr_Clear
# define PyErr_Format py3_PyErr_Format
# define PyErr_PrintEx py3_PyErr_PrintEx
# define Py_Err_No_حافظہ py3_Py_Err_No_حافظہ
# define PyErr_Occurred py3_PyErr_Occurred
# define Py_Err_سیٹ_None py3_Py_Err_سیٹ_None
# define Py_Err_سیٹ_String py3_Py_Err_سیٹ_String
# define Py_Err_سیٹ_Object py3_Py_Err_سیٹ_Object
# define PyErr_ExceptionMatches py3_PyErr_ExceptionMatches
# define Py_Eval_Init_دھاگے py3_Py_Eval_Init_دھاگے
# define PyEval_RestoreThread py3_PyEval_RestoreThread
# define PyEval_SaveThread py3_PyEval_SaveThread
# define PyGILState_Ensure py3_PyGILState_Ensure
# define PyGILState_Release py3_PyGILState_Release
# define PyLong_AsLong py3_PyLong_AsLong
# define PyLong_FromLong py3_PyLong_FromLong
# define Py_List_Get_شے py3_Py_List_Get_شے
# define PyList_Append py3_PyList_Append
# define Py_List_داخل_کرو py3_Py_List_داخل_کرو
# define PyList_New py3_PyList_New
# define Py_List_سیٹ_شے_2 py3_Py_List_سیٹ_شے
# define Py_List_سائز py3_Py_List_سائز
# define PyTuple_New py3_PyTuple_New
# define Py_Tuple_Get_شے py3_Py_Tuple_Get_شے
# define Py_Tuple_سیٹ_شے py3_Py_Tuple_سیٹ_شے
# define Py_Tuple_سائز py3_Py_Tuple_سائز
# define PySequence_Check py3_PySequence_Check
# define Py_Sequence_سائز py3_Py_Sequence_سائز
# define Py_Sequence_Get_شے py3_Py_Sequence_Get_شے
# define PySequence_Fast py3_PySequence_Fast
# اگر PY_نسخہ_HEX >= 0x030601f0
#  define Py_قاش_Adjust_Indices py3_Py_قاش_Adjust_Indices
#  define Py_قاش_Unpack py3_Py_قاش_Unpack
# endif
# undef Py_قاش_Get_Indices_Ex
# define Py_قاش_Get_Indices_Ex py3_Py_قاش_Get_Indices_Ex
# define PyImport_ImportModule py3_PyImport_ImportModule
# define PyObject_Init py3__PyObject_Init
# define PyDict_New py3_PyDict_New
# define Py_Dict_Get_شے_String py3_Py_Dict_Get_شے_String
# define Py_Dict_اگلا py3_Py_Dict_اگلا
# define PyMapping_Check py3_PyMapping_Check
# ifndef Py_Mapping_کلیدیں
#  define Py_Mapping_کلیدیں py3_Py_Mapping_کلیدیں
# endif
# اگر (defined(USE_LIMITED_API) && Py_LIMITED_API >= 0x03080000) || \
       (!defined(USE_LIMITED_API) && PY_نسخہ_HEX >= 0x03080000)
#  undef PyIter_Check
#  define PyIter_Check py3_PyIter_Check
# endif
# define Py_Iter_اگلا py3_Py_Iter_اگلا
# define PyObject_GetIter py3_PyObject_GetIter
# define PyObject_Repr py3_PyObject_Repr
# define Py_Object_Get_شے py3_Py_Object_Get_شے
# define PyObject_IsTrue py3_PyObject_IsTrue
# define PyModule_GetDict py3_PyModule_GetDict
# اگر defined(USE_LIMITED_API) || PY_نسخہ_HEX >= 0x03080000
#  define Py_IncRef py3_Py_IncRef
#  define Py_DecRef py3_Py_DecRef
# endif
# ifdef USE_LIMITED_API
#  define Py_CompileString py3_Py_CompileString
#  define Py_Eval_Eval_رمز py3_Py_Eval_Eval_رمز
# ورنہ
#  undef Py_تسلسل_Simple_String
#  define Py_تسلسل_Simple_String py3_Py_تسلسل_Simple_String
#  undef Py_تسلسل_String
#  define Py_تسلسل_String py3_Py_تسلسل_String
# endif
# define PyObject_GetAttrString py3_PyObject_GetAttrString
# define PyObject_HasAttrString py3_PyObject_HasAttrString
# define Py_Object_سیٹ_Attr_String py3_Py_Object_سیٹ_Attr_String
# define Py_Object_Call_Function_Obj_دلائل py3_Py_Object_Call_Function_Obj_دلائل
# اگر PY_نسخہ_HEX >= 0x030d0000
#  define PyObject_CallFunction py3_PyObject_CallFunction
# ورنہ
#  define Py_Object_Call_Function_سائز_T py3_Py_Object_Call_Function_سائز_T
# endif
# define PyObject_Call py3_PyObject_Call
# define PyEval_GetLocals py3_PyEval_GetLocals
# define PyEval_GetGlobals py3_PyEval_GetGlobals
# define Py_Sys_سیٹ_Object py3_Py_Sys_سیٹ_Object
# define PySys_GetObject py3_PySys_GetObject
# define Py_Sys_سیٹ_Argv py3_Py_Sys_سیٹ_Argv
# define PyType_Ready py3_PyType_Ready
# اگر PY_نسخہ_HEX >= 0x03040000
#  define PyType_GetFlags py3_PyType_GetFlags
# endif
# undef Py_Build_قدر
# define Py_Build_قدر py3_Py_Build_قدر
# define Py_سیٹ_Python_Home py3_Py_سیٹ_Python_Home
# define Py_Initialize py3_Py_Initialize
# define Py_Finalize py3_Py_Finalize
# define Py_IsInitialized py3_Py_IsInitialized
# define _Py_NoneStruct (*py3__Py_NoneStruct)
# define _Py_FalseStruct (*py3__Py_FalseStruct)
# define _Py_TrueStruct (*py3__Py_TrueStruct)
# اگر !defined(USE_LIMITED_API) && PY_نسخہ_HEX < 0x030D0000
// Private symbol that used to be required as part of PyIter_Check.
#  define Py_Object_اگلا_Not_Implemented (*py3_Py_Object_اگلا_Not_Implemented)
# endif
# define Py_Module_جوڑو_Object py3_Py_Module_جوڑو_Object
# define PyImport_AppendInittab py3_PyImport_AppendInittab
# define Py_Import_جوڑو_Module py3_Py_Import_جوڑو_Module
# ifdef USE_LIMITED_API
#  اگر Py_LIMITED_API >= 0x030a0000
#   define Py_Unicode_As_UTF8_And_سائز py3_Py_Unicode_As_UTF8_And_سائز
#  endif
# ورنہ
#  اگر PY_نسخہ_HEX >= 0x030300f0
#   define Py_Unicode_As_UTF8_And_سائز py3_Py_Unicode_As_UTF8_And_سائز
#  ورنہ
#   define _PyUnicode_AsString py3__PyUnicode_AsString
#  endif
# endif
# undef PyUnicode_CompareWithASCIIString
# define PyUnicode_CompareWithASCIIString py3_PyUnicode_CompareWithASCIIString
# undef PyUnicode_AsEncodedString
# define PyUnicode_AsEncodedString py3_PyUnicode_AsEncodedString
# undef PyUnicode_AsUTF8String
# define PyUnicode_AsUTF8String py3_PyUnicode_AsUTF8String
# undef Py_بائٹیں_As_String
# define Py_بائٹیں_As_String py3_Py_بائٹیں_As_String
# ifndef Py_بائٹیں_As_String_And_سائز
#  define Py_بائٹیں_As_String_And_سائز py3_Py_بائٹیں_As_String_And_سائز
# endif
# undef Py_بائٹیں_From_String
# define Py_بائٹیں_From_String py3_Py_بائٹیں_From_String
# undef Py_بائٹیں_From_String_And_سائز
# define Py_بائٹیں_From_String_And_سائز py3_Py_بائٹیں_From_String_And_سائز
# define PyFloat_FromDouble py3_PyFloat_FromDouble
# define PyFloat_AsDouble py3_PyFloat_AsDouble
# define PyObject_GenericGetAttr py3_PyObject_GenericGetAttr
# define PyType_Type (*py3_PyType_Type)
# ifndef USE_LIMITED_API
#  define PyStdPrinter_Type (*py3_PyStdPrinter_Type)
# endif
# define Py_قاش_Type (*py3_Py_قاش_Type)
# define PyFloat_Type (*py3_PyFloat_Type)
# define Py_نمبر_Check (*py3_Py_نمبر_Check)
# define Py_نمبر_Long (*py3_Py_نمبر_Long)
# define PyBool_Type (*py3_PyBool_Type)
# define PyErr_NewException py3_PyErr_NewException
# ifdef Py_DEBUG
#  define _Py_NegativeRefcount py3__Py_NegativeRefcount
#  define Py_Ref_کل (*py3_Py_Ref_کل)
#  define PyModule_Create2TraceRefs py3_PyModule_Create2TraceRefs
# ورنہ
#  define PyModule_Create2 py3_PyModule_Create2
# endif
# اگر defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
#  define _PyObject_DebugMalloc py3__PyObject_DebugMalloc
#  define _PyObject_DebugFree py3__PyObject_DebugFree
# ورنہ
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
#  ورنہ
#   define PyUnicodeUCS2_FromFormat py3_PyUnicodeUCS2_FromFormat
#  endif
# ورنہ
#  define PyUnicode_FromFormat py3_PyUnicode_FromFormat
# endif
# undef PyUnicode_Decode
# define PyUnicode_Decode py3_PyUnicode_Decode
# define PyType_IsSubtype py3_PyType_IsSubtype
# define PyCapsule_New py3_PyCapsule_New
# define Py_Capsule_Get_اشارہ_گر py3_Py_Capsule_Get_اشارہ_گر
# ifdef USE_LIMITED_API
#  define Py_Type_Get_خانہ py3_Py_Type_Get_خانہ
#  define PyType_FromSpec py3_PyType_FromSpec
# endif

# اگر defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
#  undef PyObject_NEW
#  define PyObject_NEW(type, typeobj) \
( (type *) PyObject_Init( \
	(PyObject *) _PyObject_DebugMalloc( Py_Object_سائز(typeobj) ), (typeobj)) )
# elif PY_نسخہ_HEX >= 0x030900b0
#  undef PyObject_NEW
#  define PyObject_NEW(type, typeobj) \
	((type *)py3__PyObject_New(typeobj))
# endif

/*
 * Pointers for dynamic link
 */
ساکن صحیح (*py3_Py_Sys_سیٹ_Argv)(صحیح, wchar_t **);
ساکن خالی (*py3_Py_سیٹ_Python_Home)(wchar_t *home);
ساکن خالی (*py3_Py_Initialize)(خالی);
ساکن PyObject* (*py3_PyList_New)(Py_ssize_t سائز);
ساکن PyGILState_STATE (*py3_PyGILState_Ensure)(خالی);
ساکن خالی (*py3_PyGILState_Release)(PyGILState_STATE);
ساکن صحیح (*py3_Py_Sys_سیٹ_Object)(حرف *, PyObject *);
ساکن PyObject* (*py3_PySys_GetObject)(حرف *);
ساکن صحیح (*py3_PyList_Append)(PyObject *, PyObject *);
ساکن صحیح (*py3_Py_List_داخل_کرو)(PyObject *, صحیح, PyObject *);
ساکن Py_ssize_t (*py3_Py_List_سائز)(PyObject *);
ساکن PyObject* (*py3_PyTuple_New)(Py_ssize_t سائز);
ساکن صحیح (*py3_Py_Tuple_سیٹ_شے)(PyObject *, Py_ssize_t, PyObject *);
ساکن Py_ssize_t (*py3_Py_Tuple_سائز)(PyObject *);
ساکن صحیح (*py3_PySequence_Check)(PyObject *);
ساکن Py_ssize_t (*py3_Py_Sequence_سائز)(PyObject *);
ساکن PyObject* (*py3_Py_Sequence_Get_شے)(PyObject *, Py_ssize_t);
ساکن PyObject* (*py3_PySequence_Fast)(PyObject *, ثابت حرف *);
ساکن PyObject* (*py3_Py_Tuple_Get_شے)(PyObject *, Py_ssize_t);
ساکن صحیح (*py3_PyMapping_Check)(PyObject *);
ساکن PyObject* (*py3_Py_Mapping_کلیدیں)(PyObject *);
# اگر PY_نسخہ_HEX >= 0x030601f0
ساکن صحیح (*py3_Py_قاش_Adjust_Indices)(Py_ssize_t لمبائی,
		     Py_ssize_t *آغاز, Py_ssize_t *stop, Py_ssize_t قدم);
ساکن صحیح (*py3_Py_قاش_Unpack)(PyObject *قاش,
		     Py_ssize_t *آغاز, Py_ssize_t *stop, Py_ssize_t *قدم);
# endif
ساکن صحیح (*py3_Py_قاش_Get_Indices_Ex)(Py_قاش_Object_T *r, Py_ssize_t لمبائی,
		     Py_ssize_t *آغاز, Py_ssize_t *stop, Py_ssize_t *قدم,
		     Py_ssize_t *slicelen);
ساکن PyObject* (*py3_Py_Err_No_حافظہ)(خالی);
ساکن خالی (*py3_Py_Finalize)(خالی);
ساکن خالی (*py3_Py_Err_سیٹ_String)(PyObject *, ثابت حرف *);
ساکن خالی (*py3_Py_Err_سیٹ_Object)(PyObject *, PyObject *);
ساکن صحیح (*py3_PyErr_ExceptionMatches)(PyObject *);
# اگر defined(USE_LIMITED_API) || PY_نسخہ_HEX >= 0x03080000
ساکن خالی (*py3_Py_IncRef)(PyObject *);
ساکن خالی (*py3_Py_DecRef)(PyObject *);
# endif
# ifdef USE_LIMITED_API
ساکن PyObject* (*py3_Py_CompileString)(ثابت حرف *, ثابت حرف *, صحیح);
ساکن PyObject* (*py3_Py_Eval_Eval_رمز)(PyObject *co, PyObject *globals, PyObject *locals);
# ورنہ
ساکن صحیح (*py3_Py_تسلسل_Simple_String)(حرف *);
ساکن PyObject* (*py3_Py_تسلسل_String)(حرف *, صحیح, PyObject *, PyObject *);
# endif
ساکن PyObject* (*py3_PyObject_GetAttrString)(PyObject *, ثابت حرف *);
ساکن صحیح (*py3_PyObject_HasAttrString)(PyObject *, ثابت حرف *);
ساکن صحیح (*py3_Py_Object_سیٹ_Attr_String)(PyObject *, ثابت حرف *, PyObject *);
ساکن PyObject* (*py3_Py_Object_Call_Function_Obj_دلائل)(PyObject *, ...);
# اگر PY_نسخہ_HEX >= 0x030d0000
ساکن PyObject* (*py3_PyObject_CallFunction)(PyObject *, حرف *, ...);
# ورنہ
ساکن PyObject* (*py3_Py_Object_Call_Function_سائز_T)(PyObject *, حرف *, ...);
# endif
ساکن PyObject* (*py3_PyObject_Call)(PyObject *, PyObject *, PyObject *);
ساکن PyObject* (*py3_PyEval_GetGlobals)(خالی);
ساکن PyObject* (*py3_PyEval_GetLocals)(خالی);
ساکن PyObject* (*py3_Py_List_Get_شے)(PyObject *, Py_ssize_t);
ساکن PyObject* (*py3_PyImport_ImportModule)(ثابت حرف *);
ساکن PyObject* (*py3_Py_Import_جوڑو_Module)(ثابت حرف *);
ساکن صحیح (*py3_Py_Err_Bad_دلیل)(خالی);
ساکن PyObject* (*py3_PyErr_Occurred)(خالی);
ساکن PyObject* (*py3_PyModule_GetDict)(PyObject *);
ساکن صحیح (*py3_Py_List_سیٹ_شے)(PyObject *, Py_ssize_t, PyObject *);
ساکن PyObject* (*py3_Py_Dict_Get_شے_String)(PyObject *, ثابت حرف *);
ساکن صحیح (*py3_Py_Dict_اگلا)(PyObject *, Py_ssize_t *, PyObject **, PyObject **);
ساکن PyObject* (*py3_PyLong_FromLong)(طویل);
ساکن PyObject* (*py3_PyDict_New)(خالی);
# اگر (defined(USE_LIMITED_API) && Py_LIMITED_API >= 0x03080000) || \
       (!defined(USE_LIMITED_API) && PY_نسخہ_HEX >= 0x03080000)
ساکن صحیح (*py3_PyIter_Check)(PyObject *o);
# endif
ساکن PyObject* (*py3_Py_Iter_اگلا)(PyObject *);
ساکن PyObject* (*py3_PyObject_GetIter)(PyObject *);
ساکن PyObject* (*py3_PyObject_Repr)(PyObject *);
ساکن PyObject* (*py3_Py_Object_Get_شے)(PyObject *, PyObject *);
ساکن صحیح (*py3_PyObject_IsTrue)(PyObject *);
ساکن PyObject* (*py3_Py_Build_قدر)(حرف *, ...);
# اگر PY_نسخہ_HEX >= 0x03040000
ساکن صحیح (*py3_PyType_GetFlags)(PyTypeObject *o);
# endif
ساکن صحیح (*py3_PyType_Ready)(PyTypeObject *type);
ساکن صحیح (*py3_Py_Dict_سیٹ_شے_String)(PyObject *dp, حرف *کلید, PyObject *شے);
ساکن PyObject* (*py3_PyUnicode_FromString)(ثابت حرف *u);
# ifdef Py_UNICODE_USE_UCS_FUNCTIONS
#  ifdef Py_UNICODE_WIDE
ساکن PyObject* (*py3_PyUnicodeUCS4_FromFormat)(ثابت حرف *u, ...);
#  ورنہ
ساکن PyObject* (*py3_PyUnicodeUCS2_FromFormat)(ثابت حرف *u, ...);
#  endif
# ورنہ
ساکن PyObject* (*py3_PyUnicode_FromFormat)(ثابت حرف *u, ...);
# endif
ساکن PyObject* (*py3_PyUnicode_Decode)(ثابت حرف *u, Py_ssize_t سائز,
	ثابت حرف *encoding, ثابت حرف *errors);
ساکن طویل (*py3_PyLong_AsLong)(PyObject *);
ساکن خالی (*py3_Py_Err_سیٹ_None)(PyObject *);
ساکن خالی (*py3_Py_Eval_Init_دھاگے)(خالی);
ساکن خالی(*py3_PyEval_RestoreThread)(PyThreadState *);
ساکن PyThreadState*(*py3_PyEval_SaveThread)(خالی);
ساکن صحیح (*py3_PyArg_Parse)(PyObject *, حرف *, ...);
ساکن صحیح (*py3_PyArg_ParseTuple)(PyObject *, حرف *, ...);
ساکن خالی (*py3_PyMem_Free)(خالی *);
ساکن خالی* (*py3_PyMem_Malloc)(size_t);
ساکن صحیح (*py3_Py_IsInitialized)(خالی);
ساکن خالی (*py3_PyErr_Clear)(خالی);
ساکن PyObject* (*py3_PyErr_Format)(PyObject *, ثابت حرف *, ...);
ساکن خالی (*py3_PyErr_PrintEx)(صحیح);
ساکن PyObject*(*py3__PyObject_Init)(PyObject *, PyTypeObject *);
# اگر !defined(USE_LIMITED_API) && PY_نسخہ_HEX < 0x030D0000
ساکن iternextfunc py3_Py_Object_اگلا_Not_Implemented;
# endif
ساکن PyObject* py3__Py_NoneStruct;
ساکن PyObject* py3__Py_FalseStruct;
ساکن PyObject* py3__Py_TrueStruct;
ساکن صحیح (*py3_Py_Module_جوڑو_Object)(PyObject *m, ثابت حرف *name, PyObject *o);
ساکن صحیح (*py3_PyImport_AppendInittab)(ثابت حرف *name, PyObject* (*initfunc)(خالی));
# ifdef USE_LIMITED_API
#  اگر Py_LIMITED_API >= 0x030a0000
ساکن حرف* (*py3_Py_Unicode_As_UTF8_And_سائز)(PyObject *unicode, Py_ssize_t *سائز);
#  endif
# ورنہ
#  اگر PY_نسخہ_HEX >= 0x030300f0
ساکن حرف* (*py3_Py_Unicode_As_UTF8_And_سائز)(PyObject *unicode, Py_ssize_t *سائز);
#  ورنہ
ساکن حرف* (*py3__PyUnicode_AsString)(PyObject *unicode);
#  endif
# endif
ساکن صحیح (*py3_PyUnicode_CompareWithASCIIString)(PyObject *unicode, ثابت حرف* string);
ساکن PyObject* (*py3_PyUnicode_AsEncodedString)(PyObject *unicode, ثابت حرف* encoding, ثابت حرف* errors);
ساکن PyObject* (*py3_PyUnicode_AsUTF8String)(PyObject *unicode);
ساکن حرف* (*py3_Py_بائٹیں_As_String)(PyObject *بائٹیں);
ساکن صحیح (*py3_Py_بائٹیں_As_String_And_سائز)(PyObject *بائٹیں, حرف **بفر, Py_ssize_t *لمبائی);
ساکن PyObject* (*py3_Py_بائٹیں_From_String)(حرف *str);
ساکن PyObject* (*py3_Py_بائٹیں_From_String_And_سائز)(حرف *str, Py_ssize_t لمبائی);
# اگر PY_نسخہ_HEX >= 0x030900b0
ساکن PyObject* (*py3__PyObject_New)(PyTypeObject *);
# endif
ساکن PyObject* (*py3_PyFloat_FromDouble)(دوہرا عدد);
ساکن دوہرا (*py3_PyFloat_AsDouble)(PyObject *);
ساکن PyObject* (*py3_PyObject_GenericGetAttr)(PyObject *obj, PyObject *name);
ساکن PyObject* (*py3_PyType_GenericAlloc)(PyTypeObject *type, Py_ssize_t nitems);
ساکن PyObject* (*py3_PyType_GenericNew)(PyTypeObject *type, PyObject *دلائل, PyObject *kwds);
ساکن PyTypeObject* py3_PyType_Type;
# ifndef USE_LIMITED_API
ساکن PyTypeObject* py3_PyStdPrinter_Type;
# endif
ساکن PyTypeObject* py3_Py_قاش_Type;
ساکن PyTypeObject* py3_PyFloat_Type;
ساکن PyTypeObject* py3_PyBool_Type;
ساکن صحیح (*py3_Py_نمبر_Check)(PyObject *);
ساکن PyObject* (*py3_Py_نمبر_Long)(PyObject *);
ساکن PyObject* (*py3_PyErr_NewException)(حرف *name, PyObject *بنیاد, PyObject *dict);
ساکن PyObject* (*py3_PyCapsule_New)(خالی *, حرف *, PyCapsule_Destructor);
ساکن خالی* (*py3_Py_Capsule_Get_اشارہ_گر)(PyObject *, حرف *);
# ifdef Py_DEBUG
ساکن خالی (*py3__Py_NegativeRefcount)(ثابت حرف *fname, صحیح lineno, PyObject *op);
ساکن Py_ssize_t* py3_Py_Ref_کل;
ساکن PyObject* (*py3_PyModule_Create2TraceRefs)(ساخت PyModuleDef* module, صحیح module_api_نسخہ);
# ورنہ
ساکن PyObject* (*py3_PyModule_Create2)(ساخت PyModuleDef* module, صحیح module_api_نسخہ);
# endif
# اگر defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
ساکن خالی (*py3__PyObject_DebugFree)(خالی*);
ساکن خالی* (*py3__PyObject_DebugMalloc)(size_t);
# ورنہ
ساکن خالی (*py3_PyObject_Free)(خالی*);
ساکن خالی* (*py3_PyObject_Malloc)(size_t);
# endif
ساکن PyObject*(*py3__PyObject_GC_New)(PyTypeObject *);
ساکن خالی(*py3_PyObject_GC_Del)(خالی *);
ساکن خالی(*py3_PyObject_GC_UnTrack)(خالی *);
ساکن صحیح (*py3_PyType_IsSubtype)(PyTypeObject *, PyTypeObject *);
# ifdef USE_LIMITED_API
ساکن خالی* (*py3_Py_Type_Get_خانہ)(PyTypeObject *, صحیح);
ساکن PyObject* (*py3_PyType_FromSpec)(PyType_Spec *);
# endif

// Imported exception objects
ساکن PyObject *p3imp_PyExc_AttributeError;
ساکن PyObject *p3imp_Py_Exc_اشاریہ_Error;
ساکن PyObject *p3imp_Py_Exc_کلید_Error;
ساکن PyObject *p3imp_Py_Exc_Keyboard_مداخلت;
ساکن PyObject *p3imp_PyExc_TypeError;
ساکن PyObject *p3imp_Py_Exc_قدر_Error;
ساکن PyObject *p3imp_PyExc_SystemExit;
ساکن PyObject *p3imp_PyExc_RuntimeError;
ساکن PyObject *p3imp_PyExc_ImportError;
ساکن PyObject *p3imp_Py_Exc_حد_سے_تجاوز_Error;

# define PyExc_AttributeError p3imp_PyExc_AttributeError
# define Py_Exc_اشاریہ_Error p3imp_Py_Exc_اشاریہ_Error
# define Py_Exc_کلید_Error p3imp_Py_Exc_کلید_Error
# define Py_Exc_Keyboard_مداخلت p3imp_Py_Exc_Keyboard_مداخلت
# define PyExc_TypeError p3imp_PyExc_TypeError
# define Py_Exc_قدر_Error p3imp_Py_Exc_قدر_Error
# define PyExc_SystemExit p3imp_PyExc_SystemExit
# define PyExc_RuntimeError p3imp_PyExc_RuntimeError
# define PyExc_ImportError p3imp_PyExc_ImportError
# define Py_Exc_حد_سے_تجاوز_Error p3imp_Py_Exc_حد_سے_تجاوز_Error

/*
 * Table of name to function pointer of python.
 */
# define PYTHON_PROC FARPROC
ساکن ساخت
{
    حرف *name;
    PYTHON_PROC *ptr;
} py3_funcname_جدول[] =
{
    {"PySys_SetArgv", (PYTHON_PROC*)&py3_Py_Sys_سیٹ_Argv},
    {"Py_SetPythonHome", (PYTHON_PROC*)&py3_Py_سیٹ_Python_Home},
    {"Py_Initialize", (PYTHON_PROC*)&py3_Py_Initialize},
    {"_PyArg_ParseTuple_SizeT", (PYTHON_PROC*)&py3_PyArg_ParseTuple},
    {"_Py_BuildValue_SizeT", (PYTHON_PROC*)&py3_Py_Build_قدر},
    {"PyMem_Free", (PYTHON_PROC*)&py3_PyMem_Free},
    {"PyMem_Malloc", (PYTHON_PROC*)&py3_PyMem_Malloc},
    {"PyList_New", (PYTHON_PROC*)&py3_PyList_New},
    {"PyGILState_Ensure", (PYTHON_PROC*)&py3_PyGILState_Ensure},
    {"PyGILState_Release", (PYTHON_PROC*)&py3_PyGILState_Release},
    {"PySys_SetObject", (PYTHON_PROC*)&py3_Py_Sys_سیٹ_Object},
    {"PySys_GetObject", (PYTHON_PROC*)&py3_PySys_GetObject},
    {"PyList_Append", (PYTHON_PROC*)&py3_PyList_Append},
    {"PyList_Insert", (PYTHON_PROC*)&py3_Py_List_داخل_کرو},
    {"PyList_Size", (PYTHON_PROC*)&py3_Py_List_سائز},
    {"PyTuple_New", (PYTHON_PROC*)&py3_PyTuple_New},
    {"PyTuple_GetItem", (PYTHON_PROC*)&py3_Py_Tuple_Get_شے},
    {"PyTuple_SetItem", (PYTHON_PROC*)&py3_Py_Tuple_سیٹ_شے},
    {"PyTuple_Size", (PYTHON_PROC*)&py3_Py_Tuple_سائز},
    {"PySequence_Check", (PYTHON_PROC*)&py3_PySequence_Check},
    {"PySequence_Size", (PYTHON_PROC*)&py3_Py_Sequence_سائز},
    {"PySequence_GetItem", (PYTHON_PROC*)&py3_Py_Sequence_Get_شے},
    {"PySequence_Fast", (PYTHON_PROC*)&py3_PySequence_Fast},
# اگر PY_نسخہ_HEX >= 0x030601f0
    {"PySlice_AdjustIndices", (PYTHON_PROC*)&py3_Py_قاش_Adjust_Indices},
    {"PySlice_Unpack", (PYTHON_PROC*)&py3_Py_قاش_Unpack},
# endif
    {"PySlice_GetIndicesEx", (PYTHON_PROC*)&py3_Py_قاش_Get_Indices_Ex},
    {"PyErr_NoMemory", (PYTHON_PROC*)&py3_Py_Err_No_حافظہ},
    {"Py_Finalize", (PYTHON_PROC*)&py3_Py_Finalize},
    {"PyErr_SetString", (PYTHON_PROC*)&py3_Py_Err_سیٹ_String},
    {"PyErr_SetObject", (PYTHON_PROC*)&py3_Py_Err_سیٹ_Object},
    {"PyErr_ExceptionMatches", (PYTHON_PROC*)&py3_PyErr_ExceptionMatches},
# اگر defined(USE_LIMITED_API) || PY_نسخہ_HEX >= 0x03080000
    {"Py_IncRef", (PYTHON_PROC*)&py3_Py_IncRef},
    {"Py_DecRef", (PYTHON_PROC*)&py3_Py_DecRef},
# endif
# ifdef USE_LIMITED_API
    {"Py_CompileString", (PYTHON_PROC*)&py3_Py_CompileString},
    {"PyEval_EvalCode", (PYTHON_PROC*)&Py_Eval_Eval_رمز},
# ورنہ
    {"PyRun_SimpleString", (PYTHON_PROC*)&py3_Py_تسلسل_Simple_String},
    {"PyRun_String", (PYTHON_PROC*)&py3_Py_تسلسل_String},
# endif
    {"PyObject_GetAttrString", (PYTHON_PROC*)&py3_PyObject_GetAttrString},
    {"PyObject_HasAttrString", (PYTHON_PROC*)&py3_PyObject_HasAttrString},
    {"PyObject_SetAttrString", (PYTHON_PROC*)&py3_Py_Object_سیٹ_Attr_String},
    {"PyObject_CallFunctionObjArgs", (PYTHON_PROC*)&py3_Py_Object_Call_Function_Obj_دلائل},
# اگر PY_نسخہ_HEX >= 0x030d0000
    {"PyObject_CallFunction", (PYTHON_PROC*)&py3_PyObject_CallFunction},
# ورنہ
    {"_PyObject_CallFunction_SizeT", (PYTHON_PROC*)&py3_Py_Object_Call_Function_سائز_T},
# endif
    {"PyObject_Call", (PYTHON_PROC*)&py3_PyObject_Call},
    {"PyEval_GetGlobals", (PYTHON_PROC*)&py3_PyEval_GetGlobals},
    {"PyEval_GetLocals", (PYTHON_PROC*)&py3_PyEval_GetLocals},
    {"PyList_GetItem", (PYTHON_PROC*)&py3_Py_List_Get_شے},
    {"PyImport_ImportModule", (PYTHON_PROC*)&py3_PyImport_ImportModule},
    {"PyImport_AddModule", (PYTHON_PROC*)&py3_Py_Import_جوڑو_Module},
    {"PyErr_BadArgument", (PYTHON_PROC*)&py3_Py_Err_Bad_دلیل},
    {"PyErr_Occurred", (PYTHON_PROC*)&py3_PyErr_Occurred},
    {"PyModule_GetDict", (PYTHON_PROC*)&py3_PyModule_GetDict},
    {"PyList_SetItem", (PYTHON_PROC*)&py3_Py_List_سیٹ_شے},
    {"PyDict_GetItemString", (PYTHON_PROC*)&py3_Py_Dict_Get_شے_String},
    {"PyDict_Next", (PYTHON_PROC*)&py3_Py_Dict_اگلا},
    {"PyMapping_Check", (PYTHON_PROC*)&py3_PyMapping_Check},
    {"PyMapping_Keys", (PYTHON_PROC*)&py3_Py_Mapping_کلیدیں},
# اگر (defined(USE_LIMITED_API) && Py_LIMITED_API >= 0x03080000) || \
       (!defined(USE_LIMITED_API) && PY_نسخہ_HEX >= 0x03080000)
    {"PyIter_Check", (PYTHON_PROC*)&py3_PyIter_Check},
# endif
    {"PyIter_Next", (PYTHON_PROC*)&py3_Py_Iter_اگلا},
    {"PyObject_GetIter", (PYTHON_PROC*)&py3_PyObject_GetIter},
    {"PyObject_Repr", (PYTHON_PROC*)&py3_PyObject_Repr},
    {"PyObject_GetItem", (PYTHON_PROC*)&py3_Py_Object_Get_شے},
    {"PyObject_IsTrue", (PYTHON_PROC*)&py3_PyObject_IsTrue},
    {"PyLong_FromLong", (PYTHON_PROC*)&py3_PyLong_FromLong},
    {"PyDict_New", (PYTHON_PROC*)&py3_PyDict_New},
# اگر PY_نسخہ_HEX >= 0x03040000
    {"PyType_GetFlags", (PYTHON_PROC*)&py3_PyType_GetFlags},
# endif
    {"PyType_Ready", (PYTHON_PROC*)&py3_PyType_Ready},
    {"PyDict_SetItemString", (PYTHON_PROC*)&py3_Py_Dict_سیٹ_شے_String},
    {"PyLong_AsLong", (PYTHON_PROC*)&py3_PyLong_AsLong},
    {"PyErr_SetNone", (PYTHON_PROC*)&py3_Py_Err_سیٹ_None},
    {"PyEval_InitThreads", (PYTHON_PROC*)&py3_Py_Eval_Init_دھاگے},
    {"PyEval_RestoreThread", (PYTHON_PROC*)&py3_PyEval_RestoreThread},
    {"PyEval_SaveThread", (PYTHON_PROC*)&py3_PyEval_SaveThread},
    {"_PyArg_Parse_SizeT", (PYTHON_PROC*)&py3_PyArg_Parse},
    {"Py_IsInitialized", (PYTHON_PROC*)&py3_Py_IsInitialized},
# اگر !defined(USE_LIMITED_API) && PY_نسخہ_HEX < 0x030D0000
    {"_PyObject_NextNotImplemented", (PYTHON_PROC*)&py3_Py_Object_اگلا_Not_Implemented},
# endif
    {"_Py_NoneStruct", (PYTHON_PROC*)&py3__Py_NoneStruct},
    {"_Py_FalseStruct", (PYTHON_PROC*)&py3__Py_FalseStruct},
    {"_Py_TrueStruct", (PYTHON_PROC*)&py3__Py_TrueStruct},
    {"PyErr_Clear", (PYTHON_PROC*)&py3_PyErr_Clear},
    {"PyErr_Format", (PYTHON_PROC*)&py3_PyErr_Format},
    {"PyErr_PrintEx", (PYTHON_PROC*)&py3_PyErr_PrintEx},
    {"PyObject_Init", (PYTHON_PROC*)&py3__PyObject_Init},
    {"PyModule_AddObject", (PYTHON_PROC*)&py3_Py_Module_جوڑو_Object},
    {"PyImport_AppendInittab", (PYTHON_PROC*)&py3_PyImport_AppendInittab},
# ifdef USE_LIMITED_API
#  اگر Py_LIMITED_API >= 0x030a0000
    {"PyUnicode_AsUTF8AndSize", (PYTHON_PROC*)&py3_Py_Unicode_As_UTF8_And_سائز},
#  endif
# ورنہ
#  اگر PY_نسخہ_HEX >= 0x030300f0
    {"PyUnicode_AsUTF8AndSize", (PYTHON_PROC*)&py3_Py_Unicode_As_UTF8_And_سائز},
#  ورنہ
    {"_PyUnicode_AsString", (PYTHON_PROC*)&py3__PyUnicode_AsString},
#  endif
# endif
    {"PyUnicode_CompareWithASCIIString", (PYTHON_PROC*)&py3_PyUnicode_CompareWithASCIIString},
    {"PyUnicode_AsUTF8String", (PYTHON_PROC*)&py3_PyUnicode_AsUTF8String},
# ifdef Py_UNICODE_USE_UCS_FUNCTIONS
#  ifdef Py_UNICODE_WIDE
    {"PyUnicodeUCS4_FromFormat", (PYTHON_PROC*)&py3_PyUnicodeUCS4_FromFormat},
#  ورنہ
    {"PyUnicodeUCS2_FromFormat", (PYTHON_PROC*)&py3_PyUnicodeUCS2_FromFormat},
#  endif
# ورنہ
    {"PyUnicode_FromFormat", (PYTHON_PROC*)&py3_PyUnicode_FromFormat},
# endif
    {"PyBytes_AsString", (PYTHON_PROC*)&py3_Py_بائٹیں_As_String},
    {"PyBytes_AsStringAndSize", (PYTHON_PROC*)&py3_Py_بائٹیں_As_String_And_سائز},
    {"PyBytes_FromString", (PYTHON_PROC*)&py3_Py_بائٹیں_From_String},
    {"PyBytes_FromStringAndSize", (PYTHON_PROC*)&py3_Py_بائٹیں_From_String_And_سائز},
# اگر PY_نسخہ_HEX >= 0x030900b0
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
    {"PySlice_Type", (PYTHON_PROC*)&py3_Py_قاش_Type},
    {"PyFloat_Type", (PYTHON_PROC*)&py3_PyFloat_Type},
    {"PyBool_Type", (PYTHON_PROC*)&py3_PyBool_Type},
    {"PyNumber_Check", (PYTHON_PROC*)&py3_Py_نمبر_Check},
    {"PyNumber_Long", (PYTHON_PROC*)&py3_Py_نمبر_Long},
    {"PyErr_NewException", (PYTHON_PROC*)&py3_PyErr_NewException},
# ifdef Py_DEBUG
    {"_Py_NegativeRefcount", (PYTHON_PROC*)&py3__Py_NegativeRefcount},
    {"_Py_RefTotal", (PYTHON_PROC*)&py3_Py_Ref_کل},
    {"PyModule_Create2TraceRefs", (PYTHON_PROC*)&py3_PyModule_Create2TraceRefs},
# ورنہ
    {"PyModule_Create2", (PYTHON_PROC*)&py3_PyModule_Create2},
# endif
# اگر defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
    {"_PyObject_DebugFree", (PYTHON_PROC*)&py3__PyObject_DebugFree},
    {"_PyObject_DebugMalloc", (PYTHON_PROC*)&py3__PyObject_DebugMalloc},
# ورنہ
    {"PyObject_Malloc", (PYTHON_PROC*)&py3_PyObject_Malloc},
    {"PyObject_Free", (PYTHON_PROC*)&py3_PyObject_Free},
# endif
    {"_PyObject_GC_New", (PYTHON_PROC*)&py3__PyObject_GC_New},
    {"PyObject_GC_Del", (PYTHON_PROC*)&py3_PyObject_GC_Del},
    {"PyObject_GC_UnTrack", (PYTHON_PROC*)&py3_PyObject_GC_UnTrack},
    {"PyType_IsSubtype", (PYTHON_PROC*)&py3_PyType_IsSubtype},
    {"PyCapsule_New", (PYTHON_PROC*)&py3_PyCapsule_New},
    {"PyCapsule_GetPointer", (PYTHON_PROC*)&py3_Py_Capsule_Get_اشارہ_گر},
# ifdef USE_LIMITED_API
#  اگر PY_نسخہ_HEX >= 0x03040000
    {"PyType_GetSlot", (PYTHON_PROC*)&py3_Py_Type_Get_خانہ},
#  endif
    {"PyType_FromSpec", (PYTHON_PROC*)&py3_PyType_FromSpec},
# endif
    {"", تہی},
};

# اگر defined(USE_LIMITED_API) || PY_نسخہ_HEX >= 0x03080000
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

# اگر PY_نسخہ_HEX >= 0x030900b0
    ساکن اندرونی صحیح
py3_PyType_HasFeature(PyTypeObject *type, غیرعلامتی طویل feature)
{
    واپس ((PyType_GetFlags(type) & feature) != 0);
}
#  define PyType_HasFeature(t,f) py3_PyType_HasFeature(t,f)
# endif

# اگر PY_نسخہ_HEX >= 0x030a00b2
    ساکن اندرونی صحیح
py3__PyObject_TypeCheck(PyObject *ob, PyTypeObject *type)
{
    واپس Py_IS_TYPE(ob, type) || PyType_IsSubtype(Py_TYPE(ob), type);
}
#  اگر PY_نسخہ_HEX >= 0x030b00b3
#   undef PyObject_TypeCheck
#   define PyObject_TypeCheck(o,t) py3__PyObject_TypeCheck(o,t)
#  ورنہ
#   define _PyObject_TypeCheck(o,t) py3__PyObject_TypeCheck(o,t)
#  endif
# endif

# اگر !defined(USE_LIMITED_API) && PY_نسخہ_HEX >= 0x030c00b0
// PyTuple_GET_SIZE/PyList_GET_SIZE are inlined functions that use Py_SIZE(),
// which started to introduce linkage dependency from Python 3.12. When we
// build Python in dynamic mode, we don't link against it in build time, and
// this would fail to build. Just use the non-inlined version instead.
#  undef Py_Tuple_GET_سائز
#  define Py_Tuple_GET_سائز(o) Py_Tuple_سائز(o)
#  undef Py_List_GET_سائز
#  define Py_List_GET_سائز(o) Py_List_سائز(o)
# endif

# ifdef MSWIN
/*
 * Look up the library "libname" using the InstallPath registry key.
 * Return NULL when failed.  Return an allocated string when successful.
 */
    ساکن WCHAR *
py3_get_system_libname(ثابت حرف *libname)
{
    ثابت WCHAR	*pythoncore = L"Software\\Python\\PythonCore";
    ثابت حرف	*cp = libname;
    WCHAR	subkey[128];
    HKEY	h_کلید;
    صحیح		i;
    DWORD	j, len;
    LSTATUS	ret;

    جب (*cp != '\0')
    {
	اگر (*cp == ':' || *cp == '\\' || *cp == '/')
	{
	    // Bail out if "libname" contains path separator, assume it is
	    // an absolute path.
	    واپس تہی;
	}
	++cp;
    }

    WCHAR   keyfound[32];
    HKEY    h_کلید_چوٹی[] = {HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE};
    HKEY    h_کلید_ملا = تہی;
#  ifdef USE_LIMITED_API
    طویل    maxminor = -1;
#  endif
    برائے (i = 0; i < ARRAY_لمبائی(h_کلید_چوٹی); i++)
    {
	طویل	major, minor;

	ret = Reg_Open_کلید_Ex_W(h_کلید_چوٹی[i], pythoncore, 0, کلید_READ, &h_کلید);
	اگر (ret != ERROR_SUCCESS)
	    جاری;
	برائے (j = 0;; j++)
	{
	    WCHAR   keyname[32];
	    WCHAR   *wp;

	    len = ARRAY_لمبائی(keyname);
	    ret = Reg_Enum_کلید_Ex_W(h_کلید, j, keyname, &len,
						    تہی, تہی, تہی, تہی);
	    اگر (ret == ERROR_NO_مزید_اشیاء)
		توڑو;

	    major = wcstol(keyname, &wp, 10);
	    اگر (*wp != L'.')
		جاری;
	    minor = wcstol(wp + 1, &wp, 10);
#  ifdef _WIN64
	    اگر (*wp != L'\0')
		جاری;
#  ورنہ
	    اگر (wcscmp(wp, L"-32") != 0)
		جاری;
#  endif

	    اگر (major != PY_MAJOR_نسخہ)
		جاری;
#  ifdef USE_LIMITED_API
	    // Search the latest version.
	    اگر ((minor > maxminor)
		    && (minor >= ((Py_LIMITED_API >> 16) & 0xff)))
	    {
		maxminor = minor;
		wcscpy(keyfound, keyname);
		h_کلید_ملا = h_کلید_چوٹی[i];
	    }
#  ورنہ
	    // Check if it matches with the compiled version.
	    اگر (minor == PY_MINOR_نسخہ)
	    {
		wcscpy(keyfound, keyname);
		h_کلید_ملا = h_کلید_چوٹی[i];
		توڑو;
	    }
#  endif
	}
	Reg_Close_کلید(h_کلید);
#  ifdef USE_LIMITED_API
	اگر (h_کلید_ملا != تہی)
	    توڑو;
#  endif
    }
    اگر (h_کلید_ملا == تہی)
	واپس تہی;

    swprintf(subkey, ARRAY_لمبائی(subkey), L"%ls\\%ls\\InstallPath",
							pythoncore, keyfound);
    ret = Reg_Get_قدر_W(h_کلید_ملا, subkey, تہی, RRF_RT_REG_SZ,
							    تہی, تہی, &len);
    اگر (ret != ERROR_مزید_ڈیٹا && ret != ERROR_SUCCESS)
	واپس تہی;
    size_t len2 = len / حجم(WCHAR) + 1 + لڑی_لمبائی(libname);
    WCHAR *راستہ = alloc(len2 * حجم(WCHAR));
    اگر (راستہ == تہی)
	واپس تہی;
    ret = Reg_Get_قدر_W(h_کلید_ملا, subkey, تہی, RRF_RT_REG_SZ,
							    تہی, راستہ, &len);
    اگر (ret != ERROR_SUCCESS)
    {
	vim_free(راستہ);
	واپس تہی;
    }
    // Remove trailing path separators.
    size_t len3 = wcslen(راستہ);
    اگر ((len3 > 0) && (راستہ[len3 - 1] == L'/' || راستہ[len3 - 1] == L'\\'))
	--len3;
    swprintf(راستہ + len3, len2 - len3, L"\\%hs", libname);
    واپس راستہ;
}
# endif

/*
 * Load library and get all pointers.
 * Parameter 'libname' provides name of DLL.
 * Return OK or FAIL.
 */
    ساکن صحیح
py3_runtime_link_init(حرف *libname, صحیح verbose)
{
    صحیح i;
    PYTHON_PROC *ucs_from_string = (PYTHON_PROC *)&py3_PyUnicode_FromString;
    PYTHON_PROC *ucs_decode = (PYTHON_PROC *)&py3_PyUnicode_Decode;
    PYTHON_PROC *ucs_as_encoded_string =
				 (PYTHON_PROC *)&py3_PyUnicode_AsEncodedString;

# اگر !(defined(PY_NO_RTLD_GLOBAL) && defined(PY3_NO_RTLD_GLOBAL)) && defined(UNIX) && defined(FEAT_PYTHON)
    // Can't have Python and Python3 loaded at the same time.
    // It causes a crash, because RTLD_GLOBAL is needed for
    // standard C extension libraries of one or both python versions.
    اگر (python_loaded())
    {
	اگر (verbose)
	    emsg(_(e_this_vim_cannot_execute_py3_after_using_python));
	واپس FAIL;
    }
# endif

    اگر (hinstPy3 != 0)
	واپس OK;
    hinstPy3 = بھرو_dll(libname);

# ifdef MSWIN
    اگر (!hinstPy3)
    {
	// Attempt to use the path from InstallPath as stored in the registry.
	WCHAR *syslibname = py3_get_system_libname(libname);

	اگر (syslibname != تہی)
	{
	    hinstPy3 = بھرو_Library_Ex_W(syslibname, تہی,
		    بھرو_LIBRARY_تلاش_DLL_بھرو_DIR |
		    بھرو_LIBRARY_تلاش_SYSTEM32);
	    vim_free(syslibname);
	}
    }
# endif

    اگر (!hinstPy3)
    {
	اگر (verbose)
	    semsg(_(e_could_not_بھرو_library_str_str), libname, بھرو_dll_error());
	واپس FAIL;
    }

    برائے (i = 0; py3_funcname_جدول[i].ptr; ++i)
    {
	اگر ((*py3_funcname_جدول[i].ptr = علامت_from_dll(hinstPy3,
			py3_funcname_جدول[i].name)) == تہی)
	{
	    close_dll(hinstPy3);
	    hinstPy3 = 0;
	    اگر (verbose)
		semsg(_(e_could_not_بھرو_library_function_str), py3_funcname_جدول[i].name);
	    واپس FAIL;
	}
    }

    // Load unicode functions separately as only the ucs2 or the ucs4 functions
    // will be present in the library.
# اگر PY_نسخہ_HEX >= 0x030300f0
    *ucs_from_string = علامت_from_dll(hinstPy3, "PyUnicode_FromString");
    *ucs_decode = علامت_from_dll(hinstPy3, "PyUnicode_Decode");
    *ucs_as_encoded_string = علامت_from_dll(hinstPy3,
	    "PyUnicode_AsEncodedString");
# ورنہ
    *ucs_from_string = علامت_from_dll(hinstPy3, "PyUnicodeUCS2_FromString");
    *ucs_decode = علامت_from_dll(hinstPy3,
	    "PyUnicodeUCS2_Decode");
    *ucs_as_encoded_string = علامت_from_dll(hinstPy3,
	    "PyUnicodeUCS2_AsEncodedString");
    اگر (*ucs_from_string == تہی || *ucs_decode == تہی
					     || *ucs_as_encoded_string == تہی)
    {
	*ucs_from_string = علامت_from_dll(hinstPy3,
		"PyUnicodeUCS4_FromString");
	*ucs_decode = علامت_from_dll(hinstPy3,
		"PyUnicodeUCS4_Decode");
	*ucs_as_encoded_string = علامت_from_dll(hinstPy3,
		"PyUnicodeUCS4_AsEncodedString");
    }
# endif
    اگر (*ucs_from_string == تہی || *ucs_decode == تہی
					     || *ucs_as_encoded_string == تہی)
    {
	close_dll(hinstPy3);
	hinstPy3 = 0;
	اگر (verbose)
	    semsg(_(e_could_not_بھرو_library_function_str), "PyUnicode_UCSX_*");
	واپس FAIL;
    }

    واپس OK;
}

/*
 * If python is enabled (there is installed python on Windows system) return
 * TRUE, else FALSE.
 */
    صحیح
python3_enabled(صحیح verbose)
{
    واپس py3_runtime_link_init((حرف *)p_py3dll, verbose) == OK;
}

/*
 * Load the standard Python exceptions - don't import the symbols from the
 * DLL, as this can cause errors (importing data symbols is not reliable).
 */
    ساکن خالی
get_py3_exceptions(خالی)
{
    PyObject *exmod = PyImport_ImportModule("builtins");
    PyObject *exdict = PyModule_GetDict(exmod);
    p3imp_PyExc_AttributeError = Py_Dict_Get_شے_String(exdict, "AttributeError");
    p3imp_Py_Exc_اشاریہ_Error = Py_Dict_Get_شے_String(exdict, "IndexError");
    p3imp_Py_Exc_کلید_Error = Py_Dict_Get_شے_String(exdict, "KeyError");
    p3imp_Py_Exc_Keyboard_مداخلت = Py_Dict_Get_شے_String(exdict, "KeyboardInterrupt");
    p3imp_PyExc_TypeError = Py_Dict_Get_شے_String(exdict, "TypeError");
    p3imp_Py_Exc_قدر_Error = Py_Dict_Get_شے_String(exdict, "ValueError");
    p3imp_PyExc_SystemExit = Py_Dict_Get_شے_String(exdict, "SystemExit");
    p3imp_PyExc_RuntimeError = Py_Dict_Get_شے_String(exdict, "RuntimeError");
    p3imp_PyExc_ImportError = Py_Dict_Get_شے_String(exdict, "ImportError");
    p3imp_Py_Exc_حد_سے_تجاوز_Error = Py_Dict_Get_شے_String(exdict, "OverflowError");
    Py_XINCREF(p3imp_PyExc_AttributeError);
    Py_XINCREF(p3imp_Py_Exc_اشاریہ_Error);
    Py_XINCREF(p3imp_Py_Exc_کلید_Error);
    Py_XINCREF(p3imp_Py_Exc_Keyboard_مداخلت);
    Py_XINCREF(p3imp_PyExc_TypeError);
    Py_XINCREF(p3imp_Py_Exc_قدر_Error);
    Py_XINCREF(p3imp_PyExc_SystemExit);
    Py_XINCREF(p3imp_PyExc_RuntimeError);
    Py_XINCREF(p3imp_PyExc_ImportError);
    Py_XINCREF(p3imp_Py_Exc_حد_سے_تجاوز_Error);
    Py_XDECREF(exmod);
}
#اگر_ختم // DYNAMIC_PYTHON3

ساکن صحیح py3initialised = 0;
#تعریف_کرو PYINITIALISED py3initialised
ساکن صحیح python_انتہا_called = FALSE;

#اگر_تعریف_شدہ USE_LIMITED_API
# define DESTRUCTOR_اختتام(self) \
    ((freefunc)Py_Type_Get_خانہ(Py_TYPE((PyObject*)self), Py_tp_free))((PyObject*)self)
#ورنہ
# define DESTRUCTOR_اختتام(self) Py_TYPE(self)->tp_free((PyObject*)self)
#اگر_ختم

#تعریف_کرو WIN_PYTHON_REF(win) win->w_python3_ref
#تعریف_کرو BUF_PYTHON_REF(buf) buf->b_python3_ref
#تعریف_کرو TAB_PYTHON_REF(tab) tab->tp_python3_ref

    ساکن خالی
call_PyObject_Free(خالی *p)
{
#اگر defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
    _PyObject_DebugFree(p);
#ورنہ
    PyObject_Free(p);
#اگر_ختم
}

    ساکن PyObject *
call_PyType_GenericNew(PyTypeObject *type, PyObject *دلائل, PyObject *kwds)
{
    واپس PyType_GenericNew(type,دلائل,kwds);
}

    ساکن PyObject *
call_PyType_GenericAlloc(PyTypeObject *type, Py_ssize_t nitems)
{
    واپس PyType_GenericAlloc(type,nitems);
}

ساکن PyObject *OutputGetattro(PyObject *, PyObject *);
ساکن صحیح OutputSetattro(PyObject *, PyObject *, PyObject *);
ساکن PyObject *بفر_Getattro(PyObject *, PyObject *);
ساکن صحیح بفر_Setattro(PyObject *, PyObject *, PyObject *);
ساکن PyObject *Tab_صفحہ_Getattro(PyObject *, PyObject *);
ساکن PyObject *کھڑکی_Getattro(PyObject *, PyObject *);
ساکن صحیح کھڑکی_Setattro(PyObject *, PyObject *, PyObject *);
ساکن PyObject *RangeGetattro(PyObject *, PyObject *);
ساکن PyObject *CurrentGetattro(PyObject *, PyObject *);
ساکن صحیح CurrentSetattro(PyObject *, PyObject *, PyObject *);
ساکن PyObject *DictionaryGetattro(PyObject *, PyObject *);
ساکن صحیح DictionarySetattro(PyObject *, PyObject *, PyObject *);
ساکن PyObject *ListGetattro(PyObject *, PyObject *);
ساکن صحیح ListSetattro(PyObject *, PyObject *, PyObject *);
ساکن PyObject *TupleGetattro(PyObject *, PyObject *);
ساکن صحیح TupleSetattro(PyObject *, PyObject *, PyObject *);
ساکن PyObject *FunctionGetattro(PyObject *, PyObject *);

ساکن ساخت PyModuleDef vimmodule;

#تعریف_کرو PY_CAN_RECURSE

/*
 * Include the code shared with if_python.c
 */
#شامل "if_py_both.h"

#اگر_تعریف_شدہ USE_LIMITED_API
# اگر Py_LIMITED_API >= 0x030A0000
#  define PY_UNICODE_GET_UTF8_CHARS(obj) Py_Unicode_As_UTF8_And_سائز(obj, تہی)
# ورنہ
// Python limited API before 3.10 lack easy ways to query the raw UTF-8 chars.
// We need to first convert the string to bytes, and then extract the chars.
// This function is only used for attribute string comparisons, which have
// known short length. As such, just allocate a short static buffer to hold
// the characters instead of having to allocate/deallcoate it.
//
// An alternative would be to convert all attribute string comparisons to use
// PyUnicode_CompareWithASCIIString to skip having to extract the chars.
ساکن حرف py3_unicode_utf8_chars[20];
ساکن حرف* PY_UNICODE_GET_UTF8_CHARS(PyObject* str)
{
    py3_unicode_utf8_chars[0] = '\0';
    PyObject* بائٹیں = PyUnicode_AsUTF8String(str);
    اگر (بائٹیں)
    {
	حرف *chars;
	Py_ssize_t len;
	اگر (Py_بائٹیں_As_String_And_سائز(بائٹیں, &chars, &len) != -1)
	{
	    اگر (len < (Py_ssize_t)حجم(py3_unicode_utf8_chars))
		// PyBytes_AsStringAndSize guarantees null-termination
		حافظہ_نقل(py3_unicode_utf8_chars, chars, len + 1);
	}
	Py_DECREF(بائٹیں);
    }
    واپس py3_unicode_utf8_chars;
}
# endif
#ورنہ	// !USE_LIMITED_API
# اگر PY_نسخہ_HEX >= 0x030300f0
#  define PY_UNICODE_GET_UTF8_CHARS(obj) Py_Unicode_As_UTF8_And_سائز(obj, تہی)
# ورنہ
#  define PY_UNICODE_GET_UTF8_CHARS _PyUnicode_AsString
# endif
#اگر_ختم

// NOTE: Must always be used at the start of a block, since it declares "name".
#تعریف_کرو GET_ATTR_STRING(name, nameobj) \
    حرف	*name = ""; \
    اگر (PyUnicode_Check(nameobj)) \
	name = (حرف *)PY_UNICODE_GET_UTF8_CHARS(nameobj)

#تعریف_کرو PY3OBJ_DELETED(obj) (obj->ob_بنیاد.ob_refcnt<=0)

///////////////////////////////////////////////////////
// Internal function prototypes.

ساکن PyObject *Py3Init_vim(خالی);

///////////////////////////////////////////////////////
// 1. Python interpreter main program.

    خالی
python3_انتہا(خالی)
{
    ساکن صحیح recurse = 0;

    // If a crash occurs while doing this, don't try again.
    اگر (recurse != 0)
	واپس;

    python_انتہا_called = TRUE;
    ++recurse;

#اگر_تعریف_شدہ DYNAMIC_PYTHON3
    اگر (hinstPy3)
#اگر_ختم
    اگر (Py_IsInitialized())
    {
#اگر_تعریف_شدہ USE_LIMITED_API
	shutdown_types();
#اگر_ختم

	// acquire lock before finalizing
	PyGILState_Ensure();

	Py_Finalize();
    }

    --recurse;
}

#اگر defined(DYNAMIC_PYTHON3) && defined(DYNAMIC_PYTHON) && defined(FEAT_PYTHON) && defined(UNIX)
    صحیح
python3_loaded(خالی)
{
    واپس (hinstPy3 != 0);
}
#اگر_ختم

ساکن wchar_t *py_home_buf = تہی;

#اگر defined(MSWIN) && (PY_نسخہ_HEX >= 0x030500f0)
/*
 * Return TRUE if stdin is readable from Python 3.
 */
    ساکن BOOL
is_stdin_readable(خالی)
{
    DWORD	    منوال, eventnum;
    ساخت _stat    st;
    صحیح		    fd = fileno(stdin);
    HANDLE	    hstdin = (HANDLE)_get_osfhandle(fd);

    // Check if stdin is connected to the console.
    اگر (Get_Console_منوال(hstdin, &منوال))
	// Check if it is opened as input.
	واپس Get_نمبر_کا_Console_داخلہ_Events(hstdin, &eventnum);

    واپس _fstat(fd, &st) == 0;
}

// Python 3.5 or later will abort inside Py_Initialize() when stdin has
// been closed (i.e. executed by "vim -").  Reconnect stdin to CONIN$.
// Note that the python DLL is linked to its own stdio DLL which can be
// differ from Vim's stdio.
    ساکن خالی
reset_stdin(خالی)
{
    FILE *(*py__acrt_iob_func)(غیرعلامتی) = تہی;
    FILE *(*pyfreopen)(ثابت حرف *, ثابت حرف *, FILE *) = تہی;
    HINSTANCE hinst = get_forwarded_dll(hinstPy3);

    اگر (hinst == تہی || is_stdin_readable())
	واپس;

    // Get "freopen" and "stdin" which are used in the python DLL.
    // "stdin" is defined as "__acrt_iob_func(0)" in VC++ 2015 or later.
    py__acrt_iob_func = get_dll_import_func(hinst, "__acrt_iob_func");
    اگر (py__acrt_iob_func)
    {
	HINSTANCE hpystdiodll = ڈھونڈو_imported_module_by_funcname(hinst,
							    "__acrt_iob_func");
	اگر (hpystdiodll)
	    pyfreopen = (خالی *)Get_Proc_پتہ(hpystdiodll, "freopen");
    }

    // Reconnect stdin to CONIN$.
    اگر (pyfreopen != تہی)
	pyfreopen("CONIN$", "r", py__acrt_iob_func(0));
    ورنہ
	freopen("CONIN$", "r", stdin);
}
#ورنہ
# define reset_stdin()
#اگر_ختم

// Python 3.2 or later will abort inside Py_Initialize() when mandatory
// modules cannot be loaded (e.g. 'pythonthreehome' is wrongly set.).
// Install a hook to python dll's exit() and recover from it.
#اگر defined(MSWIN) && (PY_نسخہ_HEX >= 0x030200f0)
# define HOOK_EXIT
# include <setjmp.h>

ساکن jmp_buf exit_hook_jump_buf;
ساکن خالی *orig_exit = تہی;

/*
 * Function that replaces exit() while calling Py_Initialize().
 */
    ساکن خالی
hooked_exit(صحیح ret)
{
    // Recover from exit.
    longjmp(exit_hook_jump_buf, 1);
}

/*
 * Install a hook to python dll's exit().
 */
    ساکن خالی
hook_py_exit(خالی)
{
    HINSTANCE hinst = get_forwarded_dll(hinstPy3);

    اگر (hinst == تہی || orig_exit != تہی)
	واپس;

    orig_exit = hook_dll_import_func(hinst, "exit", (خالی *)hooked_exit);
}

/*
 * Remove the hook installed by hook_py_exit().
 */
    ساکن خالی
restore_py_exit(خالی)
{
    HINSTANCE hinst = hinstPy3;

    اگر (hinst == تہی)
	واپس;

    اگر (orig_exit != تہی)
	hook_dll_import_func(hinst, "exit", orig_exit);
    orig_exit = تہی;
}
#اگر_ختم

    ساکن صحیح
Python3_Init(خالی)
{
    اگر (!py3initialised)
    {
#اگر_تعریف_شدہ DYNAMIC_PYTHON3
	اگر (!python3_enabled(TRUE))
	{
	    emsg(_(e_sorry_this_command_is_disabled_python_library_could_not_be_ملا));
	    جاؤ fail;
	}
#اگر_ختم
	// Python 3.13 introduced a really useful feature: colorized exceptions.
	// This is great if you're reading them from the terminal, but useless
	// and broken everywhere else (such as in log files, or text editors).
	// Opt out, forcefully.
	vim_setenv((char_u*)"PYTHON_COLORS", (char_u*)"0");

	init_structs();

	اگر (*p_py3home != NUL)
	{
	    size_t len = mbstowcs(تہی, (حرف *)p_py3home, 0) + 1;

	    // The string must not change later, make a copy in static memory.
	    py_home_buf = ALLOC_MULT(wchar_t, len);
	    اگر (py_home_buf != تہی && mbstowcs(
			    py_home_buf, (حرف *)p_py3home, len) != (size_t)-1)
		Py_سیٹ_Python_Home(py_home_buf);
	}
#اگر_تعریف_شدہ PYTHON3_HOME
	ورنہ اگر (mch_getenv((char_u *)"PYTHONHOME") == تہی)
	    Py_سیٹ_Python_Home(PYTHON3_HOME);
#اگر_ختم

	PyImport_AppendInittab("vim", Py3Init_vim);

#اگر !defined(DYNAMIC_PYTHON3) && defined(MSWIN)
	hinstPy3 = GetModuleHandle(PYTHON3_DLL);
#اگر_ختم
	reset_stdin();

#اگر_تعریف_شدہ HOOK_EXIT
	// Catch exit() called in Py_Initialize().
	hook_py_exit();
	اگر (setjmp(exit_hook_jump_buf) == 0)
	{
	    Py_Initialize();
	    restore_py_exit();
	}
	ورنہ
	{
	    // exit() was called in Py_Initialize().
	    restore_py_exit();
	    emsg(_(e_critical_error_اندر_python3_initialization_check_your_installation));
	    جاؤ fail;
	}
#ورنہ
	Py_Initialize();
#اگر_ختم

#اگر PY_نسخہ_HEX < 0x03090000
	// Initialise threads.  This is deprecated since Python 3.9.
	Py_Eval_Init_دھاگے();
#اگر_ختم
#اگر_تعریف_شدہ DYNAMIC_PYTHON3
	get_py3_exceptions();
#اگر_ختم

	اگر (PythonIO_Init_io())
	    جاؤ fail;

	globals = PyModule_GetDict(Py_Import_جوڑو_Module("__main__"));

	// Remove the element from sys.path that was added because of our
	// argv[0] value in Py3Init_vim().  Previously we used an empty
	// string, but depending on the OS we then get an empty entry or
	// the current directory in sys.path.
	// Only after vim has been imported, the element does exist in
	// sys.path.
	Py_تسلسل_Simple_String("import vim; import sys; sys.path = list(filter(lambda x: not x.endswith('must>not&exist'), sys.path))");

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

    واپس 0;

fail:
    // We call PythonIO_Flush() here to print any Python errors.
    // This is OK, as it is possible to call this function even
    // if PythonIO_Init_io() has not completed successfully (it will
    // not do anything in this case).
    PythonIO_Flush();
    واپس -1;
}

/*
 * External interface
 */
    ساکن خالی
DoPyCommand(ثابت حرف *cmd,
	    dict_T* locals,
	    rangeinitializer init_range,
	    runner تسلسل,
	    خالی *arg)
{
#اگر defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    حرف		*saved_locale;
#اگر_ختم
    PyObject		*cmdstr;
    PyObject		*cmdbytes;
    PyGILState_STATE	pygilstate;

    اگر (python_انتہا_called)
	جاؤ theend;

    اگر (Python3_Init())
	جاؤ theend;

    init_range(arg);

    Python_Release_Vim();	    // leave Vim

#اگر defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    // Python only works properly when the LC_NUMERIC locale is "C".
    saved_locale = setlocale(LC_NUMERIC, تہی);
    اگر (saved_locale == تہی || STRCMP(saved_locale, "C") == 0)
	saved_locale = تہی;
    ورنہ
    {
	// Need to make a copy, value may change when setting new locale.
	saved_locale = (حرف *)vim_strsave((char_u *)saved_locale);
	(خالی)setlocale(LC_NUMERIC, "C");
    }
#اگر_ختم

    pygilstate = PyGILState_Ensure();

    // PyRun_SimpleString expects a UTF-8 string. Wrong encoding may cause
    // SyntaxError (unicode error).
    cmdstr = PyUnicode_Decode(cmd, لڑی_لمبائی(cmd),
					(حرف *)ENC_OPT, ERRORS_DECODE_ARG);
    cmdbytes = PyUnicode_AsEncodedString(cmdstr, "utf-8", ERRORS_ENCODE_ARG);
    Py_XDECREF(cmdstr);

    تسلسل(Py_بائٹیں_As_String(cmdbytes), locals, arg, &pygilstate);
    Py_XDECREF(cmdbytes);

    PyGILState_Release(pygilstate);

#اگر defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    اگر (saved_locale != تہی)
    {
	(خالی)setlocale(LC_NUMERIC, saved_locale);
	vim_free(saved_locale);
    }
#اگر_ختم

    Python_تالا_Vim();		    // enter Vim
    PythonIO_Flush();

theend:
    واپس;	    // keeps lint happy
}

/*
 * ":py3"
 */
    خالی
ex_py3(exarg_T *eap)
{
    char_u *script;

    script = script_get(eap, eap->arg);
    اگر (!eap->skip)
    {
	اگر (p_pyx == 0)
	    p_pyx = 3;

	DoPyCommand(script == تہی ? (حرف *) eap->arg : (حرف *) script,
		تہی,
		init_range_cmd,
		(runner) تسلسل_cmd,
		(خالی *) eap);
    }
    vim_free(script);
}

#تعریف_کرو بفر_سائز 2048

/*
 * ":py3file"
 */
    خالی
ex_py3file(exarg_T *eap)
{
    ساکن حرف بفر[بفر_سائز];
    ثابت حرف *file;
    حرف *p;
    صحیح i;

    اگر (p_pyx == 0)
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

    strcpy(بفر, "exec(compile(open('");
    p = بفر + 19; // size of "exec(compile(open('"

    برائے (i=0; i<2; ++i)
    {
	file = (حرف *)eap->arg;
	جب (*file && p < بفر + (بفر_سائز - 3))
	{
	    اگر (*file == '\\' || *file == '\'')
		*p++ = '\\';
	    *p++ = *file++;
	}
	// If we didn't finish the file name, we hit a buffer overflow
	اگر (*file != '\0')
	    واپس;
	اگر (i==0)
	{
	    strcpy(p,"','rb').read(),'");
	    p += 16;
	}
	ورنہ
	{
	    strcpy(p,"','exec'))");
	    p += 10;
	}
    }


    // Execute the file
    DoPyCommand(بفر,
	    تہی,
	    init_range_cmd,
	    (runner) تسلسل_cmd,
	    (خالی *) eap);
}

    خالی
ex_py3do(exarg_T *eap)
{
    اگر (p_pyx == 0)
	p_pyx = 3;

    DoPyCommand((حرف *)eap->arg,
	    تہی,
	    init_range_cmd,
	    (runner)تسلسل_do,
	    (خالی *)eap);
}

///////////////////////////////////////////////////////
// 2. Python output stream: writes output via [e]msg().

// Implementation functions

    ساکن PyObject *
OutputGetattro(PyObject *self, PyObject *nameobj)
{
    GET_ATTR_STRING(name, nameobj);

    اگر (strcmp(name, "softspace") == 0)
	واپس PyLong_FromLong(((OutputObject *)(self))->softspace);
    ورنہ اگر (strcmp(name, "errors") == 0)
	واپس PyString_FromString("strict");
    ورنہ اگر (strcmp(name, "encoding") == 0)
	واپس PyString_FromString(ENC_OPT);

    واپس PyObject_GenericGetAttr(self, nameobj);
}

    ساکن صحیح
OutputSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);

    واپس OutputSetattr(self, name, val);
}

///////////////////////////////////////////////////////
// 3. Implementation of the Vim module for Python

// Window type - Implementation functions
// --------------------------------------

#تعریف_کرو کھڑکی_Type_Check(obj) ((obj)->ob_بنیاد.ob_type == &کھڑکی_Type)

// Buffer type - Implementation functions
// --------------------------------------

#تعریف_کرو بفر_Type_Check(obj) ((obj)->ob_بنیاد.ob_type == &بفر_Type)

ساکن PyObject* بفر_Subscript(PyObject *self, PyObject *idx);
ساکن صحیح بفر_As_Subscript(PyObject *self, PyObject *idx, PyObject *val);

// Line range type - Implementation functions
// --------------------------------------

#تعریف_کرو RangeType_Check(obj) ((obj)->ob_بنیاد.ob_type == &RangeType)

ساکن PyObject* RangeSubscript(PyObject *self, PyObject *idx);
ساکن صحیح Range_As_شے(PyObject *, Py_ssize_t, PyObject *);
ساکن صحیح RangeAsSubscript(PyObject *self, PyObject *idx, PyObject *val);

// Current objects type - Implementation functions
// -----------------------------------------------

ساکن PySequenceMethods بفر_As_Seq = {
    (lenfunc)		بفر_لمبائی,	    // sq_length,    len(x)
    (binaryfunc)	0,		    // sq_concat,    x+y
    (ssizeargfunc)	0,		    // sq_repeat,    x*n
    (ssizeargfunc)	بفر_شے,	    // sq_item,      x[i]
    0,					    // was_sq_slice,	 x[i:j]
    0,					    // sq_ass_item,  x[i]=v
    0,					    // sq_ass_slice, x[i:j]=v
    0,					    // sq_contains
    0,					    // sq_inplace_concat
    0,					    // sq_inplace_repeat
};

ساکن PyMappingMethods بفر_As_Mapping = {
    /* mp_length	*/ (lenfunc)بفر_لمبائی,
    /* mp_subscript     */ (binaryfunc)بفر_Subscript,
    /* mp_ass_subscript */ (objobjargproc)بفر_As_Subscript,
};


// Buffer object

    ساکن PyObject *
بفر_Getattro(PyObject *self, PyObject *nameobj)
{
    PyObject *r;

    GET_ATTR_STRING(name, nameobj);

    اگر ((r = بفر_Attr_Valid((بفر_Object *)(self), name)))
	واپس r;

    اگر (Check_بفر((بفر_Object *)(self)))
	واپس تہی;

    r = بفر_Attr((بفر_Object *)(self), name);
    اگر (r || PyErr_Occurred())
	واپس r;
    ورنہ
	واپس PyObject_GenericGetAttr(self, nameobj);
}

    ساکن صحیح
بفر_Setattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);

    واپس بفر_Setattr(self, name, val);
}

//////////////////

    ساکن PyObject *
بفر_Subscript(PyObject *self, PyObject* idx)
{
    اگر (PyLong_Check(idx))
    {
	طویل _idx = PyLong_AsLong(idx);
	واپس بفر_شے((بفر_Object *)(self), _idx);
    }
    ورنہ اگر (Py_قاش_Check(idx))
    {
	Py_ssize_t آغاز, stop, قدم, slicelen;

	اگر (Check_بفر((بفر_Object *) self))
	    واپس تہی;

	اگر (Py_قاش_Get_Indices_Ex((Py_قاش_Object_T *)idx,
	      (Py_ssize_t)((بفر_Object *)(self))->buf->b_ml.ml_سطر_گنتی,
	      &آغاز, &stop,
	      &قدم, &slicelen) < 0)
	    واپس تہی;
	واپس بفر_قاش((بفر_Object *)(self), آغاز, stop);
    }
    ورنہ
    {
	RAISE_INVALID_اشاریہ_TYPE(idx);
	واپس تہی;
    }
}

    ساکن صحیح
بفر_As_Subscript(PyObject *self, PyObject* idx, PyObject* val)
{
    اگر (PyLong_Check(idx))
    {
	طویل n = PyLong_AsLong(idx);

	اگر (Check_بفر((بفر_Object *) self))
	    واپس -1;

	واپس RBAs_شے((بفر_Object *)(self), n, val, 1,
		    (Py_ssize_t)((بفر_Object *)(self))->buf->b_ml.ml_سطر_گنتی,
		    تہی);
    }
    ورنہ اگر (Py_قاش_Check(idx))
    {
	Py_ssize_t آغاز, stop, قدم, slicelen;

	اگر (Check_بفر((بفر_Object *) self))
	    واپس -1;

	اگر (Py_قاش_Get_Indices_Ex((Py_قاش_Object_T *)idx,
	      (Py_ssize_t)((بفر_Object *)(self))->buf->b_ml.ml_سطر_گنتی,
	      &آغاز, &stop,
	      &قدم, &slicelen) < 0)
	    واپس -1;
	واپس RBAs_قاش((بفر_Object *)(self), آغاز, stop, val, 1,
			  (PyInt)((بفر_Object *)(self))->buf->b_ml.ml_سطر_گنتی,
			  تہی);
    }
    ورنہ
    {
	RAISE_INVALID_اشاریہ_TYPE(idx);
	واپس -1;
    }
}

ساکن PySequenceMethods RangeAsSeq = {
    (lenfunc)		Range_لمبائی,	 // sq_length,	  len(x)
    (binaryfunc)	0,		 // RangeConcat, sq_concat,  x+y
    (ssizeargfunc)	0,		 // RangeRepeat, sq_repeat,  x*n
    (ssizeargfunc)	Range_شے,	 // sq_item,	  x[i]
    0,					 // was_sq_slice,     x[i:j]
    (ssizeobjargproc)	Range_As_شے,	 // sq_as_item,  x[i]=v
    0,					 // sq_ass_slice, x[i:j]=v
    0,					 // sq_contains
    0,					 // sq_inplace_concat
    0,					 // sq_inplace_repeat
};

ساکن PyMappingMethods RangeAsMapping = {
    /* mp_length	*/ (lenfunc)Range_لمبائی,
    /* mp_subscript     */ (binaryfunc)RangeSubscript,
    /* mp_ass_subscript */ (objobjargproc)RangeAsSubscript,
};

// Line range object - Implementation

    ساکن PyObject *
RangeGetattro(PyObject *self, PyObject *nameobj)
{
    GET_ATTR_STRING(name, nameobj);

    اگر (strcmp(name, "start") == 0)
	واپس Py_Build_قدر("n", ((RangeObject *)(self))->آغاز - 1);
    ورنہ اگر (strcmp(name, "end") == 0)
	واپس Py_Build_قدر("n", ((RangeObject *)(self))->انتہا - 1);
    ورنہ
	واپس PyObject_GenericGetAttr(self, nameobj);
}

////////////////

    ساکن صحیح
Range_As_شے(PyObject *self, Py_ssize_t n, PyObject *val)
{
    واپس RBAs_شے(((RangeObject *)(self))->buf, n, val,
		    ((RangeObject *)(self))->آغاز,
		    ((RangeObject *)(self))->انتہا,
		    &((RangeObject *)(self))->انتہا);
}

    ساکن Py_ssize_t
Range_As_قاش(PyObject *self, Py_ssize_t lo, Py_ssize_t hi, PyObject *val)
{
    واپس RBAs_قاش(((RangeObject *)(self))->buf, lo, hi, val,
		    ((RangeObject *)(self))->آغاز,
		    ((RangeObject *)(self))->انتہا,
		    &((RangeObject *)(self))->انتہا);
}

    ساکن PyObject *
RangeSubscript(PyObject *self, PyObject* idx)
{
    اگر (PyLong_Check(idx))
    {
	طویل _idx = PyLong_AsLong(idx);
	واپس Range_شے((RangeObject *)(self), _idx);
    }
    ورنہ اگر (Py_قاش_Check(idx))
    {
	Py_ssize_t آغاز, stop, قدم, slicelen;

	اگر (Py_قاش_Get_Indices_Ex((Py_قاش_Object_T *)idx,
		((RangeObject *)(self))->انتہا-((RangeObject *)(self))->آغاز+1,
		&آغاز, &stop,
		&قدم, &slicelen) < 0)
	    واپس تہی;
	واپس Range_قاش((RangeObject *)(self), آغاز, stop);
    }
    ورنہ
    {
	RAISE_INVALID_اشاریہ_TYPE(idx);
	واپس تہی;
    }
}

    ساکن صحیح
RangeAsSubscript(PyObject *self, PyObject *idx, PyObject *val)
{
    اگر (PyLong_Check(idx))
    {
	طویل n = PyLong_AsLong(idx);
	واپس Range_As_شے(self, n, val);
    }
    ورنہ اگر (Py_قاش_Check(idx))
    {
	Py_ssize_t آغاز, stop, قدم, slicelen;

	اگر (Py_قاش_Get_Indices_Ex((Py_قاش_Object_T *)idx,
		((RangeObject *)(self))->انتہا-((RangeObject *)(self))->آغاز+1,
		&آغاز, &stop,
		&قدم, &slicelen) < 0)
	    واپس -1;
	واپس Range_As_قاش(self, آغاز, stop, val);
    }
    ورنہ
    {
	RAISE_INVALID_اشاریہ_TYPE(idx);
	واپس -1;
    }
}

// TabPage object - Implementation

    ساکن PyObject *
Tab_صفحہ_Getattro(PyObject *self, PyObject *nameobj)
{
    PyObject *r;

    GET_ATTR_STRING(name, nameobj);

    اگر ((r = Tab_صفحہ_Attr_Valid((Tab_صفحہ_Object *)(self), name)))
	واپس r;

    اگر (Check_Tab_صفحہ((Tab_صفحہ_Object *)(self)))
	واپس تہی;

    r = Tab_صفحہ_Attr((Tab_صفحہ_Object *)(self), name);
    اگر (r || PyErr_Occurred())
	واپس r;
    ورنہ
	واپس PyObject_GenericGetAttr(self, nameobj);
}

// Window object - Implementation

    ساکن PyObject *
کھڑکی_Getattro(PyObject *self, PyObject *nameobj)
{
    PyObject *r;

    GET_ATTR_STRING(name, nameobj);

    اگر ((r = کھڑکی_Attr_Valid((کھڑکی_Object *)(self), name)))
	واپس r;

    اگر (Check_کھڑکی((کھڑکی_Object *)(self)))
	واپس تہی;

    r = کھڑکی_Attr((کھڑکی_Object *)(self), name);
    اگر (r || PyErr_Occurred())
	واپس r;
    ورنہ
	واپس PyObject_GenericGetAttr(self, nameobj);
}

    ساکن صحیح
کھڑکی_Setattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);

    واپس کھڑکی_Setattr(self, name, val);
}

// Tab page list object - Definitions

ساکن PySequenceMethods TabListAsSeq = {
    (lenfunc)	     Tab_List_لمبائی,	    // sq_length,    len(x)
    (binaryfunc)     0,			    // sq_concat,    x+y
    (ssizeargfunc)   0,			    // sq_repeat,    x*n
    (ssizeargfunc)   Tab_List_شے,	    // sq_item,      x[i]
    0,					    // sq_slice,     x[i:j]
    (ssizeobjargproc)0,			    // sq_as_item,  x[i]=v
    0,					    // sq_ass_slice, x[i:j]=v
    0,					    // sq_contains
    0,					    // sq_inplace_concat
    0,					    // sq_inplace_repeat
};

// Window list object - Definitions

ساکن PySequenceMethods WinListAsSeq = {
    (lenfunc)	     Win_List_لمبائی,	    // sq_length,    len(x)
    (binaryfunc)     0,			    // sq_concat,    x+y
    (ssizeargfunc)   0,			    // sq_repeat,    x*n
    (ssizeargfunc)   Win_List_شے,	    // sq_item,      x[i]
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
    ساکن PyObject *
CurrentGetattro(PyObject *self, PyObject *nameobj)
{
    PyObject	*r;
    GET_ATTR_STRING(name, nameobj);
    اگر (!(r = CurrentGetattr(self, name)))
	واپس PyObject_GenericGetAttr(self, nameobj);
    واپس r;
}

    ساکن صحیح
CurrentSetattro(PyObject *self, PyObject *nameobj, PyObject *قدر)
{
    GET_ATTR_STRING(name, nameobj);
    واپس CurrentSetattr(self, name, قدر);
}

// Dictionary object - Definitions

    ساکن PyObject *
DictionaryGetattro(PyObject *self, PyObject *nameobj)
{
    DictionaryObject	*this = ((DictionaryObject *) (self));

    GET_ATTR_STRING(name, nameobj);

    اگر (strcmp(name, "locked") == 0)
	واپس PyLong_FromLong(this->dict->dv_تالا);
    ورنہ اگر (strcmp(name, "scope") == 0)
	واپس PyLong_FromLong(this->dict->dv_scope);

    واپس PyObject_GenericGetAttr(self, nameobj);
}

    ساکن صحیح
DictionarySetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);
    واپس DictionarySetattr(self, name, val);
}

// List object - Definitions

    ساکن PyObject *
ListGetattro(PyObject *self, PyObject *nameobj)
{
    GET_ATTR_STRING(name, nameobj);

    اگر (strcmp(name, "locked") == 0)
	واپس PyLong_FromLong(((ListObject *) (self))->list->lv_تالا);

    واپس PyObject_GenericGetAttr(self, nameobj);
}

    ساکن صحیح
ListSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);
    واپس ListSetattr(self, name, val);
}

// Tuple object - Definitions

    ساکن PyObject *
TupleGetattro(PyObject *self, PyObject *nameobj)
{
    GET_ATTR_STRING(name, nameobj);

    اگر (strcmp(name, "locked") == 0)
	واپس PyLong_FromLong(((TupleObject *) (self))->tuple->tv_تالا);

    واپس PyObject_GenericGetAttr(self, nameobj);
}

    ساکن صحیح
TupleSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);
    واپس TupleSetattr(self, name, val);
}

// Function object - Definitions

    ساکن PyObject *
FunctionGetattro(PyObject *self, PyObject *nameobj)
{
    PyObject		*r;
    FunctionObject	*this = (FunctionObject *)(self);

    GET_ATTR_STRING(name, nameobj);

    r = FunctionAttr(this, name);
    اگر (r || PyErr_Occurred())
	واپس r;
    ورنہ
	واپس PyObject_GenericGetAttr(self, nameobj);
}

// External interface

    خالی
python3_بفر_free(buf_T *buf)
{
    بفر_Object *bp = BUF_PYTHON_REF(buf);
    اگر (bp == تہی)
	واپس;
    bp->buf = INVALID_بفر_قدر;
    BUF_PYTHON_REF(buf) = تہی;
}

    خالی
python3_کھڑکی_free(win_T *win)
{
    کھڑکی_Object *wp = WIN_PYTHON_REF(win);
    اگر (wp == تہی)
	واپس;
    wp->win = INVALID_کھڑکی_قدر;
    WIN_PYTHON_REF(win) = تہی;
}

    خالی
python3_tabpage_free(tabpage_T *tab)
{
    Tab_صفحہ_Object *tp = TAB_PYTHON_REF(tab);
    اگر (tp == تہی)
	واپس;
    tp->tab = INVALID_TABPAGE_قدر;
    TAB_PYTHON_REF(tab) = تہی;
}

    ساکن PyObject *
Py3Init_vim(خالی)
{
    // The special value is removed from sys.path in Python3_Init().
    ساکن wchar_t *(argv[2]) = {L"/must>not&exist/foo", تہی};

    اگر (init_types())
	واپس تہی;

    // Set sys.argv[] to avoid a crash in warn().
    Py_Sys_سیٹ_Argv(1, argv);

    اگر ((vim_module = PyModule_Create(&vimmodule)) == تہی)
	واپس تہی;

    اگر (populate_module(vim_module))
	واپس تہی;

    اگر (init_sys_راستہ())
	واپس تہی;

    واپس vim_module;
}

//////////////////////////////////////////////////////////////////////////
// 4. Utility functions for handling the interface between Vim and Python.

/*
 * Convert a Vim line into a Python string.
 * All internal newlines are replaced by null characters.
 *
 * On errors, the Python exception data is set, and NULL is returned.
 */
    ساکن PyObject *
سطر_مقصد_String(ثابت حرف *str)
{
    PyObject *نتیجہ;
    Py_ssize_t len = لڑی_لمبائی(str);
    حرف *tmp, *p;

    tmp = alloc(len + 1);
    p = tmp;
    اگر (p == تہی)
    {
	Py_Err_No_حافظہ();
	واپس تہی;
    }

    جب (*str)
    {
	اگر (*str == '\n')
	    *p = '\0';
	ورنہ
	    *p = *str;

	++p;
	++str;
    }
    *p = '\0';

    نتیجہ = PyUnicode_Decode(tmp, len, (حرف *)ENC_OPT, ERRORS_DECODE_ARG);

    vim_free(tmp);
    واپس نتیجہ;
}

    خالی
do_py3eval(char_u *str, dict_T *locals, typval_T *rettv)
{
    DoPyCommand((حرف *) str,
	    locals,
	    init_range_eval,
	    (runner) تسلسل_eval,
	    (خالی *) rettv);
    اگر (rettv->v_type == VAR_UNKNOWN)
    {
	rettv->v_type = VAR_نمبر;
	rettv->vval.v_نمبر = 0;
    }
}

    صحیح
سیٹ_ref_اندر_python3(صحیح copyID)
{
    واپس سیٹ_ref_اندر_py(copyID);
}

    صحیح
python3_نسخہ(خالی)
{
#اگر_تعریف_شدہ USE_LIMITED_API
    واپس Py_LIMITED_API;
#ورنہ
    واپس PY_نسخہ_HEX;
#اگر_ختم
}
