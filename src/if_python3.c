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

#ضمن "vim.h"

#ضمن <limits.h>

#إذا defined(MSWIN) && defined(HAVE_FCNTL_H)
# undef HAVE_FCNTL_H
#پایان_اگر

#اگر_تعریف_شده _DEBUG
# undef _DEBUG
#پایان_اگر

#اگر_تعریف_شده F_BLANK
# undef F_BLANK
#پایان_اگر

#اگر_تعریف_شده HAVE_DUP
# undef HAVE_DUP
#پایان_اگر
#اگر_تعریف_شده HAVE_STRFTIME
# undef HAVE_STRFTIME
#پایان_اگر
#اگر_تعریف_شده HAVE_STRING_H
# undef HAVE_STRING_H
#پایان_اگر
#اگر_تعریف_شده HAVE_PUTENV
# undef HAVE_PUTENV
#پایان_اگر
#اگر_تعریف_شده _POSIX_C_SOURCE  // defined in feature.h
# undef _POSIX_C_SOURCE
#پایان_اگر
#اگر_تعریف_شده XOPEN_منبع
# undef XOPEN_منبع	// pyconfig.h defines it as well.
#پایان_اگر

#تعریف_کن PY_SSIZE_T_CLEAN

#اگر_تعریف_شده Py_LIMITED_API
# define USE_LIMITED_API // Using Python 3 limited ABI
#پایان_اگر

#ضمن <Python.h>

#لغو_تعریف اصلی // Defined in python.h - aargh
#لغو_تعریف HAVE_FCNTL_H // Clash with os_win32.h

// The "surrogateescape" error handler is new in Python 3.1
#إذا PY_نسخه_HEX >= 0x030100f0
# define CODEC_ERROR_HANDLER "surrogateescape"
#وإلا
# define CODEC_ERROR_HANDLER عدم
#پایان_اگر

// Suppress Python 3.11 depreciations to see useful warnings
#اگر_تعریف_شده __GNUC__
# pragma GCC diagnostic افزودن
# pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#پایان_اگر

// Python 3 does not support CObjects, always use Capsules
#تعریف_کن PY_USE_CAPSULE

#تعریف_کن ERRORS_DECODE_ARG CODEC_ERROR_HANDLER
#تعریف_کن ERRORS_ENCODE_ARG ERRORS_DECODE_ARG

#تعریف_کن PyInt Py_ssize_t
#اگر_تعریف_نشده PyString_Check
# define PyString_Check(obj) PyUnicode_Check(obj)
#پایان_اگر
#تعریف_کن PyString_FromString(repr) \
    PyUnicode_Decode(repr, STRLEN(repr), ENC_OPT, ERRORS_DECODE_ARG)
#تعریف_کن PyString_FromFormat PyUnicode_FromFormat
#اگر_تعریف_شده PyUnicode_FromFormat
# define Py_UNICODE_USE_UCS_FUNCTIONS
#پایان_اگر
#اگر_تعریف_نشده PyInt_Check
# define PyInt_Check(obj) PyLong_Check(obj)
#پایان_اگر
#تعریف_کن PyInt_FromLong(i) PyLong_FromLong(i)
#تعریف_کن PyInt_AsLong(obj) PyLong_AsLong(obj)
#تعریف_کن Py_ssize_t_fmt "n"
#تعریف_کن Py_بایتها_fmt "y"

#تعریف_کن PyIntArgFunc	ssizeargfunc
#تعریف_کن PyIntObjArgProc	ssizeobjargproc

/*
 * PySlice_GetIndicesEx(): first argument type changed from PySliceObject
 * to PyObject in Python 3.2 or later.
 */
#إذا PY_نسخه_HEX >= 0x030200f0
تعريفنوع PyObject Py_برش_Object_T;
#وإلا
تعريفنوع Py_برش_Object Py_برش_Object_T;
#پایان_اگر

#اگر_تعریف_نشده MSWIN
# define HINSTANCE فراغ *
#پایان_اگر
#إذا defined(DYNAMIC_PYTHON3) || defined(MSWIN)
ساكن HINSTANCE hinstPy3 = 0; // Instance of python.dll
#پایان_اگر

#إذا defined(DYNAMIC_PYTHON3)

# ifdef MSWIN
#  define بارگیری_dll vim_بارگیری_Lib
#  define close_dll FreeLibrary
#  define نماد_from_dll Get_Proc_آدرس
#  define بارگیری_dll_error GetWin32Error
# وإلا
#  include <dlfcn.h>
#  define FARPROC فراغ*
#  إذا defined(PY_NO_RTLD_GLOBAL) && defined(PY3_NO_RTLD_GLOBAL)
#   define بارگیری_dll(n) dlopen((n), RTLD_LAZY)
#  وإلا
#   define بارگیری_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#  endif
#  define close_dll dlclose
#  define نماد_from_dll dlsym
#  define بارگیری_dll_error dlerror
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
# define Py_Dict_مجموعه_قلم_String py3_Py_Dict_مجموعه_قلم_String
# define Py_Err_Bad_آرگومان py3_Py_Err_Bad_آرگومان
# define PyErr_Clear py3_PyErr_Clear
# define PyErr_Format py3_PyErr_Format
# define PyErr_PrintEx py3_PyErr_PrintEx
# define Py_Err_No_حافظه py3_Py_Err_No_حافظه
# define PyErr_Occurred py3_PyErr_Occurred
# define Py_Err_مجموعه_None py3_Py_Err_مجموعه_None
# define Py_Err_مجموعه_String py3_Py_Err_مجموعه_String
# define Py_Err_مجموعه_Object py3_Py_Err_مجموعه_Object
# define PyErr_ExceptionMatches py3_PyErr_ExceptionMatches
# define Py_Eval_Init_نخها py3_Py_Eval_Init_نخها
# define PyEval_RestoreThread py3_PyEval_RestoreThread
# define PyEval_SaveThread py3_PyEval_SaveThread
# define PyGILState_Ensure py3_PyGILState_Ensure
# define PyGILState_Release py3_PyGILState_Release
# define PyLong_AsLong py3_PyLong_AsLong
# define PyLong_FromLong py3_PyLong_FromLong
# define Py_List_Get_قلم py3_Py_List_Get_قلم
# define PyList_Append py3_PyList_Append
# define Py_List_درج py3_Py_List_درج
# define PyList_New py3_PyList_New
# define Py_List_مجموعه_قلم_2 py3_Py_List_مجموعه_قلم
# define Py_List_اندازه py3_Py_List_اندازه
# define PyTuple_New py3_PyTuple_New
# define Py_Tuple_Get_قلم py3_Py_Tuple_Get_قلم
# define Py_Tuple_مجموعه_قلم py3_Py_Tuple_مجموعه_قلم
# define Py_Tuple_اندازه py3_Py_Tuple_اندازه
# define PySequence_Check py3_PySequence_Check
# define Py_Sequence_اندازه py3_Py_Sequence_اندازه
# define Py_Sequence_Get_قلم py3_Py_Sequence_Get_قلم
# define PySequence_Fast py3_PySequence_Fast
# إذا PY_نسخه_HEX >= 0x030601f0
#  define Py_برش_Adjust_Indices py3_Py_برش_Adjust_Indices
#  define Py_برش_Unpack py3_Py_برش_Unpack
# endif
# undef Py_برش_Get_Indices_Ex
# define Py_برش_Get_Indices_Ex py3_Py_برش_Get_Indices_Ex
# define PyImport_ImportModule py3_PyImport_ImportModule
# define PyObject_Init py3__PyObject_Init
# define PyDict_New py3_PyDict_New
# define Py_Dict_Get_قلم_String py3_Py_Dict_Get_قلم_String
# define Py_Dict_بعدی py3_Py_Dict_بعدی
# define PyMapping_Check py3_PyMapping_Check
# ifndef Py_Mapping_کلیدها
#  define Py_Mapping_کلیدها py3_Py_Mapping_کلیدها
# endif
# إذا (defined(USE_LIMITED_API) && Py_LIMITED_API >= 0x03080000) || \
       (!defined(USE_LIMITED_API) && PY_نسخه_HEX >= 0x03080000)
#  undef PyIter_Check
#  define PyIter_Check py3_PyIter_Check
# endif
# define Py_Iter_بعدی py3_Py_Iter_بعدی
# define PyObject_GetIter py3_PyObject_GetIter
# define PyObject_Repr py3_PyObject_Repr
# define Py_Object_Get_قلم py3_Py_Object_Get_قلم
# define PyObject_IsTrue py3_PyObject_IsTrue
# define PyModule_GetDict py3_PyModule_GetDict
# إذا defined(USE_LIMITED_API) || PY_نسخه_HEX >= 0x03080000
#  define Py_IncRef py3_Py_IncRef
#  define Py_DecRef py3_Py_DecRef
# endif
# ifdef USE_LIMITED_API
#  define Py_CompileString py3_Py_CompileString
#  define Py_Eval_Eval_کد py3_Py_Eval_Eval_کد
# وإلا
#  undef Py_پیاپی_Simple_String
#  define Py_پیاپی_Simple_String py3_Py_پیاپی_Simple_String
#  undef Py_پیاپی_String
#  define Py_پیاپی_String py3_Py_پیاپی_String
# endif
# define PyObject_GetAttrString py3_PyObject_GetAttrString
# define PyObject_HasAttrString py3_PyObject_HasAttrString
# define Py_Object_مجموعه_Attr_String py3_Py_Object_مجموعه_Attr_String
# define Py_Object_Call_Function_Obj_آرگومانها py3_Py_Object_Call_Function_Obj_آرگومانها
# إذا PY_نسخه_HEX >= 0x030d0000
#  define PyObject_CallFunction py3_PyObject_CallFunction
# وإلا
#  define Py_Object_Call_Function_اندازه_T py3_Py_Object_Call_Function_اندازه_T
# endif
# define PyObject_Call py3_PyObject_Call
# define PyEval_GetLocals py3_PyEval_GetLocals
# define PyEval_GetGlobals py3_PyEval_GetGlobals
# define Py_Sys_مجموعه_Object py3_Py_Sys_مجموعه_Object
# define PySys_GetObject py3_PySys_GetObject
# define Py_Sys_مجموعه_Argv py3_Py_Sys_مجموعه_Argv
# define PyType_Ready py3_PyType_Ready
# إذا PY_نسخه_HEX >= 0x03040000
#  define PyType_GetFlags py3_PyType_GetFlags
# endif
# undef Py_Build_مقدار
# define Py_Build_مقدار py3_Py_Build_مقدار
# define Py_مجموعه_Python_Home py3_Py_مجموعه_Python_Home
# define Py_Initialize py3_Py_Initialize
# define Py_Finalize py3_Py_Finalize
# define Py_IsInitialized py3_Py_IsInitialized
# define _Py_NoneStruct (*py3__Py_NoneStruct)
# define _Py_FalseStruct (*py3__Py_FalseStruct)
# define _Py_TrueStruct (*py3__Py_TrueStruct)
# إذا !defined(USE_LIMITED_API) && PY_نسخه_HEX < 0x030D0000
// Private symbol that used to be required as part of PyIter_Check.
#  define Py_Object_بعدی_Not_Implemented (*py3_Py_Object_بعدی_Not_Implemented)
# endif
# define Py_Module_جمع_کن_Object py3_Py_Module_جمع_کن_Object
# define PyImport_AppendInittab py3_PyImport_AppendInittab
# define Py_Import_جمع_کن_Module py3_Py_Import_جمع_کن_Module
# ifdef USE_LIMITED_API
#  إذا Py_LIMITED_API >= 0x030a0000
#   define Py_Unicode_As_UTF8_And_اندازه py3_Py_Unicode_As_UTF8_And_اندازه
#  endif
# وإلا
#  إذا PY_نسخه_HEX >= 0x030300f0
#   define Py_Unicode_As_UTF8_And_اندازه py3_Py_Unicode_As_UTF8_And_اندازه
#  وإلا
#   define _PyUnicode_AsString py3__PyUnicode_AsString
#  endif
# endif
# undef PyUnicode_CompareWithASCIIString
# define PyUnicode_CompareWithASCIIString py3_PyUnicode_CompareWithASCIIString
# undef PyUnicode_AsEncodedString
# define PyUnicode_AsEncodedString py3_PyUnicode_AsEncodedString
# undef PyUnicode_AsUTF8String
# define PyUnicode_AsUTF8String py3_PyUnicode_AsUTF8String
# undef Py_بایتها_As_String
# define Py_بایتها_As_String py3_Py_بایتها_As_String
# ifndef Py_بایتها_As_String_And_اندازه
#  define Py_بایتها_As_String_And_اندازه py3_Py_بایتها_As_String_And_اندازه
# endif
# undef Py_بایتها_From_String
# define Py_بایتها_From_String py3_Py_بایتها_From_String
# undef Py_بایتها_From_String_And_اندازه
# define Py_بایتها_From_String_And_اندازه py3_Py_بایتها_From_String_And_اندازه
# define PyFloat_FromDouble py3_PyFloat_FromDouble
# define PyFloat_AsDouble py3_PyFloat_AsDouble
# define PyObject_GenericGetAttr py3_PyObject_GenericGetAttr
# define PyType_Type (*py3_PyType_Type)
# ifndef USE_LIMITED_API
#  define PyStdPrinter_Type (*py3_PyStdPrinter_Type)
# endif
# define Py_برش_Type (*py3_Py_برش_Type)
# define PyFloat_Type (*py3_PyFloat_Type)
# define Py_عدد_Check (*py3_Py_عدد_Check)
# define Py_عدد_Long (*py3_Py_عدد_Long)
# define PyBool_Type (*py3_PyBool_Type)
# define PyErr_NewException py3_PyErr_NewException
# ifdef Py_DEBUG
#  define _Py_NegativeRefcount py3__Py_NegativeRefcount
#  define Py_Ref_مجموع (*py3_Py_Ref_مجموع)
#  define PyModule_Create2TraceRefs py3_PyModule_Create2TraceRefs
# وإلا
#  define PyModule_Create2 py3_PyModule_Create2
# endif
# إذا defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
#  define _PyObject_DebugMalloc py3__PyObject_DebugMalloc
#  define _PyObject_DebugFree py3__PyObject_DebugFree
# وإلا
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
#  وإلا
#   define PyUnicodeUCS2_FromFormat py3_PyUnicodeUCS2_FromFormat
#  endif
# وإلا
#  define PyUnicode_FromFormat py3_PyUnicode_FromFormat
# endif
# undef PyUnicode_Decode
# define PyUnicode_Decode py3_PyUnicode_Decode
# define PyType_IsSubtype py3_PyType_IsSubtype
# define PyCapsule_New py3_PyCapsule_New
# define Py_Capsule_Get_اشاره_گر py3_Py_Capsule_Get_اشاره_گر
# ifdef USE_LIMITED_API
#  define Py_Type_Get_جایگاه py3_Py_Type_Get_جایگاه
#  define PyType_FromSpec py3_PyType_FromSpec
# endif

# إذا defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
#  undef PyObject_NEW
#  define PyObject_NEW(type, typeobj) \
( (type *) PyObject_Init( \
	(PyObject *) _PyObject_DebugMalloc( Py_Object_اندازه(typeobj) ), (typeobj)) )
# elif PY_نسخه_HEX >= 0x030900b0
#  undef PyObject_NEW
#  define PyObject_NEW(type, typeobj) \
	((type *)py3__PyObject_New(typeobj))
# endif

/*
 * Pointers for dynamic link
 */
ساكن صحيح (*py3_Py_Sys_مجموعه_Argv)(صحيح, wchar_t **);
ساكن فراغ (*py3_Py_مجموعه_Python_Home)(wchar_t *home);
ساكن فراغ (*py3_Py_Initialize)(فراغ);
ساكن PyObject* (*py3_PyList_New)(Py_ssize_t اندازه);
ساكن PyGILState_STATE (*py3_PyGILState_Ensure)(فراغ);
ساكن فراغ (*py3_PyGILState_Release)(PyGILState_STATE);
ساكن صحيح (*py3_Py_Sys_مجموعه_Object)(حرف *, PyObject *);
ساكن PyObject* (*py3_PySys_GetObject)(حرف *);
ساكن صحيح (*py3_PyList_Append)(PyObject *, PyObject *);
ساكن صحيح (*py3_Py_List_درج)(PyObject *, صحيح, PyObject *);
ساكن Py_ssize_t (*py3_Py_List_اندازه)(PyObject *);
ساكن PyObject* (*py3_PyTuple_New)(Py_ssize_t اندازه);
ساكن صحيح (*py3_Py_Tuple_مجموعه_قلم)(PyObject *, Py_ssize_t, PyObject *);
ساكن Py_ssize_t (*py3_Py_Tuple_اندازه)(PyObject *);
ساكن صحيح (*py3_PySequence_Check)(PyObject *);
ساكن Py_ssize_t (*py3_Py_Sequence_اندازه)(PyObject *);
ساكن PyObject* (*py3_Py_Sequence_Get_قلم)(PyObject *, Py_ssize_t);
ساكن PyObject* (*py3_PySequence_Fast)(PyObject *, ثابت حرف *);
ساكن PyObject* (*py3_Py_Tuple_Get_قلم)(PyObject *, Py_ssize_t);
ساكن صحيح (*py3_PyMapping_Check)(PyObject *);
ساكن PyObject* (*py3_Py_Mapping_کلیدها)(PyObject *);
# إذا PY_نسخه_HEX >= 0x030601f0
ساكن صحيح (*py3_Py_برش_Adjust_Indices)(Py_ssize_t درازی,
		     Py_ssize_t *شروع, Py_ssize_t *stop, Py_ssize_t گام);
ساكن صحيح (*py3_Py_برش_Unpack)(PyObject *برش,
		     Py_ssize_t *شروع, Py_ssize_t *stop, Py_ssize_t *گام);
# endif
ساكن صحيح (*py3_Py_برش_Get_Indices_Ex)(Py_برش_Object_T *r, Py_ssize_t درازی,
		     Py_ssize_t *شروع, Py_ssize_t *stop, Py_ssize_t *گام,
		     Py_ssize_t *slicelen);
ساكن PyObject* (*py3_Py_Err_No_حافظه)(فراغ);
ساكن فراغ (*py3_Py_Finalize)(فراغ);
ساكن فراغ (*py3_Py_Err_مجموعه_String)(PyObject *, ثابت حرف *);
ساكن فراغ (*py3_Py_Err_مجموعه_Object)(PyObject *, PyObject *);
ساكن صحيح (*py3_PyErr_ExceptionMatches)(PyObject *);
# إذا defined(USE_LIMITED_API) || PY_نسخه_HEX >= 0x03080000
ساكن فراغ (*py3_Py_IncRef)(PyObject *);
ساكن فراغ (*py3_Py_DecRef)(PyObject *);
# endif
# ifdef USE_LIMITED_API
ساكن PyObject* (*py3_Py_CompileString)(ثابت حرف *, ثابت حرف *, صحيح);
ساكن PyObject* (*py3_Py_Eval_Eval_کد)(PyObject *co, PyObject *globals, PyObject *locals);
# وإلا
ساكن صحيح (*py3_Py_پیاپی_Simple_String)(حرف *);
ساكن PyObject* (*py3_Py_پیاپی_String)(حرف *, صحيح, PyObject *, PyObject *);
# endif
ساكن PyObject* (*py3_PyObject_GetAttrString)(PyObject *, ثابت حرف *);
ساكن صحيح (*py3_PyObject_HasAttrString)(PyObject *, ثابت حرف *);
ساكن صحيح (*py3_Py_Object_مجموعه_Attr_String)(PyObject *, ثابت حرف *, PyObject *);
ساكن PyObject* (*py3_Py_Object_Call_Function_Obj_آرگومانها)(PyObject *, ...);
# إذا PY_نسخه_HEX >= 0x030d0000
ساكن PyObject* (*py3_PyObject_CallFunction)(PyObject *, حرف *, ...);
# وإلا
ساكن PyObject* (*py3_Py_Object_Call_Function_اندازه_T)(PyObject *, حرف *, ...);
# endif
ساكن PyObject* (*py3_PyObject_Call)(PyObject *, PyObject *, PyObject *);
ساكن PyObject* (*py3_PyEval_GetGlobals)(فراغ);
ساكن PyObject* (*py3_PyEval_GetLocals)(فراغ);
ساكن PyObject* (*py3_Py_List_Get_قلم)(PyObject *, Py_ssize_t);
ساكن PyObject* (*py3_PyImport_ImportModule)(ثابت حرف *);
ساكن PyObject* (*py3_Py_Import_جمع_کن_Module)(ثابت حرف *);
ساكن صحيح (*py3_Py_Err_Bad_آرگومان)(فراغ);
ساكن PyObject* (*py3_PyErr_Occurred)(فراغ);
ساكن PyObject* (*py3_PyModule_GetDict)(PyObject *);
ساكن صحيح (*py3_Py_List_مجموعه_قلم)(PyObject *, Py_ssize_t, PyObject *);
ساكن PyObject* (*py3_Py_Dict_Get_قلم_String)(PyObject *, ثابت حرف *);
ساكن صحيح (*py3_Py_Dict_بعدی)(PyObject *, Py_ssize_t *, PyObject **, PyObject **);
ساكن PyObject* (*py3_PyLong_FromLong)(دراز);
ساكن PyObject* (*py3_PyDict_New)(فراغ);
# إذا (defined(USE_LIMITED_API) && Py_LIMITED_API >= 0x03080000) || \
       (!defined(USE_LIMITED_API) && PY_نسخه_HEX >= 0x03080000)
ساكن صحيح (*py3_PyIter_Check)(PyObject *o);
# endif
ساكن PyObject* (*py3_Py_Iter_بعدی)(PyObject *);
ساكن PyObject* (*py3_PyObject_GetIter)(PyObject *);
ساكن PyObject* (*py3_PyObject_Repr)(PyObject *);
ساكن PyObject* (*py3_Py_Object_Get_قلم)(PyObject *, PyObject *);
ساكن صحيح (*py3_PyObject_IsTrue)(PyObject *);
ساكن PyObject* (*py3_Py_Build_مقدار)(حرف *, ...);
# إذا PY_نسخه_HEX >= 0x03040000
ساكن صحيح (*py3_PyType_GetFlags)(PyTypeObject *o);
# endif
ساكن صحيح (*py3_PyType_Ready)(PyTypeObject *type);
ساكن صحيح (*py3_Py_Dict_مجموعه_قلم_String)(PyObject *dp, حرف *کلید, PyObject *قلم);
ساكن PyObject* (*py3_PyUnicode_FromString)(ثابت حرف *u);
# ifdef Py_UNICODE_USE_UCS_FUNCTIONS
#  ifdef Py_UNICODE_WIDE
ساكن PyObject* (*py3_PyUnicodeUCS4_FromFormat)(ثابت حرف *u, ...);
#  وإلا
ساكن PyObject* (*py3_PyUnicodeUCS2_FromFormat)(ثابت حرف *u, ...);
#  endif
# وإلا
ساكن PyObject* (*py3_PyUnicode_FromFormat)(ثابت حرف *u, ...);
# endif
ساكن PyObject* (*py3_PyUnicode_Decode)(ثابت حرف *u, Py_ssize_t اندازه,
	ثابت حرف *encoding, ثابت حرف *errors);
ساكن دراز (*py3_PyLong_AsLong)(PyObject *);
ساكن فراغ (*py3_Py_Err_مجموعه_None)(PyObject *);
ساكن فراغ (*py3_Py_Eval_Init_نخها)(فراغ);
ساكن فراغ(*py3_PyEval_RestoreThread)(PyThreadState *);
ساكن PyThreadState*(*py3_PyEval_SaveThread)(فراغ);
ساكن صحيح (*py3_PyArg_Parse)(PyObject *, حرف *, ...);
ساكن صحيح (*py3_PyArg_ParseTuple)(PyObject *, حرف *, ...);
ساكن فراغ (*py3_PyMem_Free)(فراغ *);
ساكن فراغ* (*py3_PyMem_Malloc)(size_t);
ساكن صحيح (*py3_Py_IsInitialized)(فراغ);
ساكن فراغ (*py3_PyErr_Clear)(فراغ);
ساكن PyObject* (*py3_PyErr_Format)(PyObject *, ثابت حرف *, ...);
ساكن فراغ (*py3_PyErr_PrintEx)(صحيح);
ساكن PyObject*(*py3__PyObject_Init)(PyObject *, PyTypeObject *);
# إذا !defined(USE_LIMITED_API) && PY_نسخه_HEX < 0x030D0000
ساكن iternextfunc py3_Py_Object_بعدی_Not_Implemented;
# endif
ساكن PyObject* py3__Py_NoneStruct;
ساكن PyObject* py3__Py_FalseStruct;
ساكن PyObject* py3__Py_TrueStruct;
ساكن صحيح (*py3_Py_Module_جمع_کن_Object)(PyObject *m, ثابت حرف *name, PyObject *o);
ساكن صحيح (*py3_PyImport_AppendInittab)(ثابت حرف *name, PyObject* (*initfunc)(فراغ));
# ifdef USE_LIMITED_API
#  إذا Py_LIMITED_API >= 0x030a0000
ساكن حرف* (*py3_Py_Unicode_As_UTF8_And_اندازه)(PyObject *unicode, Py_ssize_t *اندازه);
#  endif
# وإلا
#  إذا PY_نسخه_HEX >= 0x030300f0
ساكن حرف* (*py3_Py_Unicode_As_UTF8_And_اندازه)(PyObject *unicode, Py_ssize_t *اندازه);
#  وإلا
ساكن حرف* (*py3__PyUnicode_AsString)(PyObject *unicode);
#  endif
# endif
ساكن صحيح (*py3_PyUnicode_CompareWithASCIIString)(PyObject *unicode, ثابت حرف* string);
ساكن PyObject* (*py3_PyUnicode_AsEncodedString)(PyObject *unicode, ثابت حرف* encoding, ثابت حرف* errors);
ساكن PyObject* (*py3_PyUnicode_AsUTF8String)(PyObject *unicode);
ساكن حرف* (*py3_Py_بایتها_As_String)(PyObject *بایتها);
ساكن صحيح (*py3_Py_بایتها_As_String_And_اندازه)(PyObject *بایتها, حرف **میانگیر, Py_ssize_t *درازی);
ساكن PyObject* (*py3_Py_بایتها_From_String)(حرف *str);
ساكن PyObject* (*py3_Py_بایتها_From_String_And_اندازه)(حرف *str, Py_ssize_t درازی);
# إذا PY_نسخه_HEX >= 0x030900b0
ساكن PyObject* (*py3__PyObject_New)(PyTypeObject *);
# endif
ساكن PyObject* (*py3_PyFloat_FromDouble)(مزدوج رقم);
ساكن مزدوج (*py3_PyFloat_AsDouble)(PyObject *);
ساكن PyObject* (*py3_PyObject_GenericGetAttr)(PyObject *obj, PyObject *name);
ساكن PyObject* (*py3_PyType_GenericAlloc)(PyTypeObject *type, Py_ssize_t nitems);
ساكن PyObject* (*py3_PyType_GenericNew)(PyTypeObject *type, PyObject *آرگومانها, PyObject *kwds);
ساكن PyTypeObject* py3_PyType_Type;
# ifndef USE_LIMITED_API
ساكن PyTypeObject* py3_PyStdPrinter_Type;
# endif
ساكن PyTypeObject* py3_Py_برش_Type;
ساكن PyTypeObject* py3_PyFloat_Type;
ساكن PyTypeObject* py3_PyBool_Type;
ساكن صحيح (*py3_Py_عدد_Check)(PyObject *);
ساكن PyObject* (*py3_Py_عدد_Long)(PyObject *);
ساكن PyObject* (*py3_PyErr_NewException)(حرف *name, PyObject *پایه, PyObject *dict);
ساكن PyObject* (*py3_PyCapsule_New)(فراغ *, حرف *, PyCapsule_Destructor);
ساكن فراغ* (*py3_Py_Capsule_Get_اشاره_گر)(PyObject *, حرف *);
# ifdef Py_DEBUG
ساكن فراغ (*py3__Py_NegativeRefcount)(ثابت حرف *fname, صحيح lineno, PyObject *op);
ساكن Py_ssize_t* py3_Py_Ref_مجموع;
ساكن PyObject* (*py3_PyModule_Create2TraceRefs)(بنية PyModuleDef* module, صحيح module_api_نسخه);
# وإلا
ساكن PyObject* (*py3_PyModule_Create2)(بنية PyModuleDef* module, صحيح module_api_نسخه);
# endif
# إذا defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
ساكن فراغ (*py3__PyObject_DebugFree)(فراغ*);
ساكن فراغ* (*py3__PyObject_DebugMalloc)(size_t);
# وإلا
ساكن فراغ (*py3_PyObject_Free)(فراغ*);
ساكن فراغ* (*py3_PyObject_Malloc)(size_t);
# endif
ساكن PyObject*(*py3__PyObject_GC_New)(PyTypeObject *);
ساكن فراغ(*py3_PyObject_GC_Del)(فراغ *);
ساكن فراغ(*py3_PyObject_GC_UnTrack)(فراغ *);
ساكن صحيح (*py3_PyType_IsSubtype)(PyTypeObject *, PyTypeObject *);
# ifdef USE_LIMITED_API
ساكن فراغ* (*py3_Py_Type_Get_جایگاه)(PyTypeObject *, صحيح);
ساكن PyObject* (*py3_PyType_FromSpec)(PyType_Spec *);
# endif

// Imported exception objects
ساكن PyObject *p3imp_PyExc_AttributeError;
ساكن PyObject *p3imp_Py_Exc_شاخص_Error;
ساكن PyObject *p3imp_Py_Exc_کلید_Error;
ساكن PyObject *p3imp_Py_Exc_Keyboard_وقفه;
ساكن PyObject *p3imp_PyExc_TypeError;
ساكن PyObject *p3imp_Py_Exc_مقدار_Error;
ساكن PyObject *p3imp_PyExc_SystemExit;
ساكن PyObject *p3imp_PyExc_RuntimeError;
ساكن PyObject *p3imp_PyExc_ImportError;
ساكن PyObject *p3imp_Py_Exc_سرریز_Error;

# define PyExc_AttributeError p3imp_PyExc_AttributeError
# define Py_Exc_شاخص_Error p3imp_Py_Exc_شاخص_Error
# define Py_Exc_کلید_Error p3imp_Py_Exc_کلید_Error
# define Py_Exc_Keyboard_وقفه p3imp_Py_Exc_Keyboard_وقفه
# define PyExc_TypeError p3imp_PyExc_TypeError
# define Py_Exc_مقدار_Error p3imp_Py_Exc_مقدار_Error
# define PyExc_SystemExit p3imp_PyExc_SystemExit
# define PyExc_RuntimeError p3imp_PyExc_RuntimeError
# define PyExc_ImportError p3imp_PyExc_ImportError
# define Py_Exc_سرریز_Error p3imp_Py_Exc_سرریز_Error

/*
 * Table of name to function pointer of python.
 */
# define PYTHON_PROC FARPROC
ساكن بنية
{
    حرف *name;
    PYTHON_PROC *ptr;
} py3_funcname_جدول[] =
{
    {"PySys_SetArgv", (PYTHON_PROC*)&py3_Py_Sys_مجموعه_Argv},
    {"Py_SetPythonHome", (PYTHON_PROC*)&py3_Py_مجموعه_Python_Home},
    {"Py_Initialize", (PYTHON_PROC*)&py3_Py_Initialize},
    {"_PyArg_ParseTuple_SizeT", (PYTHON_PROC*)&py3_PyArg_ParseTuple},
    {"_Py_BuildValue_SizeT", (PYTHON_PROC*)&py3_Py_Build_مقدار},
    {"PyMem_Free", (PYTHON_PROC*)&py3_PyMem_Free},
    {"PyMem_Malloc", (PYTHON_PROC*)&py3_PyMem_Malloc},
    {"PyList_New", (PYTHON_PROC*)&py3_PyList_New},
    {"PyGILState_Ensure", (PYTHON_PROC*)&py3_PyGILState_Ensure},
    {"PyGILState_Release", (PYTHON_PROC*)&py3_PyGILState_Release},
    {"PySys_SetObject", (PYTHON_PROC*)&py3_Py_Sys_مجموعه_Object},
    {"PySys_GetObject", (PYTHON_PROC*)&py3_PySys_GetObject},
    {"PyList_Append", (PYTHON_PROC*)&py3_PyList_Append},
    {"PyList_Insert", (PYTHON_PROC*)&py3_Py_List_درج},
    {"PyList_Size", (PYTHON_PROC*)&py3_Py_List_اندازه},
    {"PyTuple_New", (PYTHON_PROC*)&py3_PyTuple_New},
    {"PyTuple_GetItem", (PYTHON_PROC*)&py3_Py_Tuple_Get_قلم},
    {"PyTuple_SetItem", (PYTHON_PROC*)&py3_Py_Tuple_مجموعه_قلم},
    {"PyTuple_Size", (PYTHON_PROC*)&py3_Py_Tuple_اندازه},
    {"PySequence_Check", (PYTHON_PROC*)&py3_PySequence_Check},
    {"PySequence_Size", (PYTHON_PROC*)&py3_Py_Sequence_اندازه},
    {"PySequence_GetItem", (PYTHON_PROC*)&py3_Py_Sequence_Get_قلم},
    {"PySequence_Fast", (PYTHON_PROC*)&py3_PySequence_Fast},
# إذا PY_نسخه_HEX >= 0x030601f0
    {"PySlice_AdjustIndices", (PYTHON_PROC*)&py3_Py_برش_Adjust_Indices},
    {"PySlice_Unpack", (PYTHON_PROC*)&py3_Py_برش_Unpack},
# endif
    {"PySlice_GetIndicesEx", (PYTHON_PROC*)&py3_Py_برش_Get_Indices_Ex},
    {"PyErr_NoMemory", (PYTHON_PROC*)&py3_Py_Err_No_حافظه},
    {"Py_Finalize", (PYTHON_PROC*)&py3_Py_Finalize},
    {"PyErr_SetString", (PYTHON_PROC*)&py3_Py_Err_مجموعه_String},
    {"PyErr_SetObject", (PYTHON_PROC*)&py3_Py_Err_مجموعه_Object},
    {"PyErr_ExceptionMatches", (PYTHON_PROC*)&py3_PyErr_ExceptionMatches},
# إذا defined(USE_LIMITED_API) || PY_نسخه_HEX >= 0x03080000
    {"Py_IncRef", (PYTHON_PROC*)&py3_Py_IncRef},
    {"Py_DecRef", (PYTHON_PROC*)&py3_Py_DecRef},
# endif
# ifdef USE_LIMITED_API
    {"Py_CompileString", (PYTHON_PROC*)&py3_Py_CompileString},
    {"PyEval_EvalCode", (PYTHON_PROC*)&Py_Eval_Eval_کد},
# وإلا
    {"PyRun_SimpleString", (PYTHON_PROC*)&py3_Py_پیاپی_Simple_String},
    {"PyRun_String", (PYTHON_PROC*)&py3_Py_پیاپی_String},
# endif
    {"PyObject_GetAttrString", (PYTHON_PROC*)&py3_PyObject_GetAttrString},
    {"PyObject_HasAttrString", (PYTHON_PROC*)&py3_PyObject_HasAttrString},
    {"PyObject_SetAttrString", (PYTHON_PROC*)&py3_Py_Object_مجموعه_Attr_String},
    {"PyObject_CallFunctionObjArgs", (PYTHON_PROC*)&py3_Py_Object_Call_Function_Obj_آرگومانها},
# إذا PY_نسخه_HEX >= 0x030d0000
    {"PyObject_CallFunction", (PYTHON_PROC*)&py3_PyObject_CallFunction},
# وإلا
    {"_PyObject_CallFunction_SizeT", (PYTHON_PROC*)&py3_Py_Object_Call_Function_اندازه_T},
# endif
    {"PyObject_Call", (PYTHON_PROC*)&py3_PyObject_Call},
    {"PyEval_GetGlobals", (PYTHON_PROC*)&py3_PyEval_GetGlobals},
    {"PyEval_GetLocals", (PYTHON_PROC*)&py3_PyEval_GetLocals},
    {"PyList_GetItem", (PYTHON_PROC*)&py3_Py_List_Get_قلم},
    {"PyImport_ImportModule", (PYTHON_PROC*)&py3_PyImport_ImportModule},
    {"PyImport_AddModule", (PYTHON_PROC*)&py3_Py_Import_جمع_کن_Module},
    {"PyErr_BadArgument", (PYTHON_PROC*)&py3_Py_Err_Bad_آرگومان},
    {"PyErr_Occurred", (PYTHON_PROC*)&py3_PyErr_Occurred},
    {"PyModule_GetDict", (PYTHON_PROC*)&py3_PyModule_GetDict},
    {"PyList_SetItem", (PYTHON_PROC*)&py3_Py_List_مجموعه_قلم},
    {"PyDict_GetItemString", (PYTHON_PROC*)&py3_Py_Dict_Get_قلم_String},
    {"PyDict_Next", (PYTHON_PROC*)&py3_Py_Dict_بعدی},
    {"PyMapping_Check", (PYTHON_PROC*)&py3_PyMapping_Check},
    {"PyMapping_Keys", (PYTHON_PROC*)&py3_Py_Mapping_کلیدها},
# إذا (defined(USE_LIMITED_API) && Py_LIMITED_API >= 0x03080000) || \
       (!defined(USE_LIMITED_API) && PY_نسخه_HEX >= 0x03080000)
    {"PyIter_Check", (PYTHON_PROC*)&py3_PyIter_Check},
# endif
    {"PyIter_Next", (PYTHON_PROC*)&py3_Py_Iter_بعدی},
    {"PyObject_GetIter", (PYTHON_PROC*)&py3_PyObject_GetIter},
    {"PyObject_Repr", (PYTHON_PROC*)&py3_PyObject_Repr},
    {"PyObject_GetItem", (PYTHON_PROC*)&py3_Py_Object_Get_قلم},
    {"PyObject_IsTrue", (PYTHON_PROC*)&py3_PyObject_IsTrue},
    {"PyLong_FromLong", (PYTHON_PROC*)&py3_PyLong_FromLong},
    {"PyDict_New", (PYTHON_PROC*)&py3_PyDict_New},
# إذا PY_نسخه_HEX >= 0x03040000
    {"PyType_GetFlags", (PYTHON_PROC*)&py3_PyType_GetFlags},
# endif
    {"PyType_Ready", (PYTHON_PROC*)&py3_PyType_Ready},
    {"PyDict_SetItemString", (PYTHON_PROC*)&py3_Py_Dict_مجموعه_قلم_String},
    {"PyLong_AsLong", (PYTHON_PROC*)&py3_PyLong_AsLong},
    {"PyErr_SetNone", (PYTHON_PROC*)&py3_Py_Err_مجموعه_None},
    {"PyEval_InitThreads", (PYTHON_PROC*)&py3_Py_Eval_Init_نخها},
    {"PyEval_RestoreThread", (PYTHON_PROC*)&py3_PyEval_RestoreThread},
    {"PyEval_SaveThread", (PYTHON_PROC*)&py3_PyEval_SaveThread},
    {"_PyArg_Parse_SizeT", (PYTHON_PROC*)&py3_PyArg_Parse},
    {"Py_IsInitialized", (PYTHON_PROC*)&py3_Py_IsInitialized},
# إذا !defined(USE_LIMITED_API) && PY_نسخه_HEX < 0x030D0000
    {"_PyObject_NextNotImplemented", (PYTHON_PROC*)&py3_Py_Object_بعدی_Not_Implemented},
# endif
    {"_Py_NoneStruct", (PYTHON_PROC*)&py3__Py_NoneStruct},
    {"_Py_FalseStruct", (PYTHON_PROC*)&py3__Py_FalseStruct},
    {"_Py_TrueStruct", (PYTHON_PROC*)&py3__Py_TrueStruct},
    {"PyErr_Clear", (PYTHON_PROC*)&py3_PyErr_Clear},
    {"PyErr_Format", (PYTHON_PROC*)&py3_PyErr_Format},
    {"PyErr_PrintEx", (PYTHON_PROC*)&py3_PyErr_PrintEx},
    {"PyObject_Init", (PYTHON_PROC*)&py3__PyObject_Init},
    {"PyModule_AddObject", (PYTHON_PROC*)&py3_Py_Module_جمع_کن_Object},
    {"PyImport_AppendInittab", (PYTHON_PROC*)&py3_PyImport_AppendInittab},
# ifdef USE_LIMITED_API
#  إذا Py_LIMITED_API >= 0x030a0000
    {"PyUnicode_AsUTF8AndSize", (PYTHON_PROC*)&py3_Py_Unicode_As_UTF8_And_اندازه},
#  endif
# وإلا
#  إذا PY_نسخه_HEX >= 0x030300f0
    {"PyUnicode_AsUTF8AndSize", (PYTHON_PROC*)&py3_Py_Unicode_As_UTF8_And_اندازه},
#  وإلا
    {"_PyUnicode_AsString", (PYTHON_PROC*)&py3__PyUnicode_AsString},
#  endif
# endif
    {"PyUnicode_CompareWithASCIIString", (PYTHON_PROC*)&py3_PyUnicode_CompareWithASCIIString},
    {"PyUnicode_AsUTF8String", (PYTHON_PROC*)&py3_PyUnicode_AsUTF8String},
# ifdef Py_UNICODE_USE_UCS_FUNCTIONS
#  ifdef Py_UNICODE_WIDE
    {"PyUnicodeUCS4_FromFormat", (PYTHON_PROC*)&py3_PyUnicodeUCS4_FromFormat},
#  وإلا
    {"PyUnicodeUCS2_FromFormat", (PYTHON_PROC*)&py3_PyUnicodeUCS2_FromFormat},
#  endif
# وإلا
    {"PyUnicode_FromFormat", (PYTHON_PROC*)&py3_PyUnicode_FromFormat},
# endif
    {"PyBytes_AsString", (PYTHON_PROC*)&py3_Py_بایتها_As_String},
    {"PyBytes_AsStringAndSize", (PYTHON_PROC*)&py3_Py_بایتها_As_String_And_اندازه},
    {"PyBytes_FromString", (PYTHON_PROC*)&py3_Py_بایتها_From_String},
    {"PyBytes_FromStringAndSize", (PYTHON_PROC*)&py3_Py_بایتها_From_String_And_اندازه},
# إذا PY_نسخه_HEX >= 0x030900b0
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
    {"PySlice_Type", (PYTHON_PROC*)&py3_Py_برش_Type},
    {"PyFloat_Type", (PYTHON_PROC*)&py3_PyFloat_Type},
    {"PyBool_Type", (PYTHON_PROC*)&py3_PyBool_Type},
    {"PyNumber_Check", (PYTHON_PROC*)&py3_Py_عدد_Check},
    {"PyNumber_Long", (PYTHON_PROC*)&py3_Py_عدد_Long},
    {"PyErr_NewException", (PYTHON_PROC*)&py3_PyErr_NewException},
# ifdef Py_DEBUG
    {"_Py_NegativeRefcount", (PYTHON_PROC*)&py3__Py_NegativeRefcount},
    {"_Py_RefTotal", (PYTHON_PROC*)&py3_Py_Ref_مجموع},
    {"PyModule_Create2TraceRefs", (PYTHON_PROC*)&py3_PyModule_Create2TraceRefs},
# وإلا
    {"PyModule_Create2", (PYTHON_PROC*)&py3_PyModule_Create2},
# endif
# إذا defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
    {"_PyObject_DebugFree", (PYTHON_PROC*)&py3__PyObject_DebugFree},
    {"_PyObject_DebugMalloc", (PYTHON_PROC*)&py3__PyObject_DebugMalloc},
# وإلا
    {"PyObject_Malloc", (PYTHON_PROC*)&py3_PyObject_Malloc},
    {"PyObject_Free", (PYTHON_PROC*)&py3_PyObject_Free},
# endif
    {"_PyObject_GC_New", (PYTHON_PROC*)&py3__PyObject_GC_New},
    {"PyObject_GC_Del", (PYTHON_PROC*)&py3_PyObject_GC_Del},
    {"PyObject_GC_UnTrack", (PYTHON_PROC*)&py3_PyObject_GC_UnTrack},
    {"PyType_IsSubtype", (PYTHON_PROC*)&py3_PyType_IsSubtype},
    {"PyCapsule_New", (PYTHON_PROC*)&py3_PyCapsule_New},
    {"PyCapsule_GetPointer", (PYTHON_PROC*)&py3_Py_Capsule_Get_اشاره_گر},
# ifdef USE_LIMITED_API
#  إذا PY_نسخه_HEX >= 0x03040000
    {"PyType_GetSlot", (PYTHON_PROC*)&py3_Py_Type_Get_جایگاه},
#  endif
    {"PyType_FromSpec", (PYTHON_PROC*)&py3_PyType_FromSpec},
# endif
    {"", عدم},
};

# إذا defined(USE_LIMITED_API) || PY_نسخه_HEX >= 0x03080000
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

# إذا PY_نسخه_HEX >= 0x030900b0
    ساكن مضمن صحيح
py3_PyType_HasFeature(PyTypeObject *type, بیعلامت دراز feature)
{
    أرجع ((PyType_GetFlags(type) & feature) != 0);
}
#  define PyType_HasFeature(t,f) py3_PyType_HasFeature(t,f)
# endif

# إذا PY_نسخه_HEX >= 0x030a00b2
    ساكن مضمن صحيح
py3__PyObject_TypeCheck(PyObject *ob, PyTypeObject *type)
{
    أرجع Py_IS_TYPE(ob, type) || PyType_IsSubtype(Py_TYPE(ob), type);
}
#  إذا PY_نسخه_HEX >= 0x030b00b3
#   undef PyObject_TypeCheck
#   define PyObject_TypeCheck(o,t) py3__PyObject_TypeCheck(o,t)
#  وإلا
#   define _PyObject_TypeCheck(o,t) py3__PyObject_TypeCheck(o,t)
#  endif
# endif

# إذا !defined(USE_LIMITED_API) && PY_نسخه_HEX >= 0x030c00b0
// PyTuple_GET_SIZE/PyList_GET_SIZE are inlined functions that use Py_SIZE(),
// which started to introduce linkage dependency from Python 3.12. When we
// build Python in dynamic mode, we don't link against it in build time, and
// this would fail to build. Just use the non-inlined version instead.
#  undef Py_Tuple_GET_اندازه
#  define Py_Tuple_GET_اندازه(o) Py_Tuple_اندازه(o)
#  undef Py_List_GET_اندازه
#  define Py_List_GET_اندازه(o) Py_List_اندازه(o)
# endif

# ifdef MSWIN
/*
 * Look up the library "libname" using the InstallPath registry key.
 * Return NULL when failed.  Return an allocated string when successful.
 */
    ساكن WCHAR *
py3_get_system_libname(ثابت حرف *libname)
{
    ثابت WCHAR	*pythoncore = L"Software\\Python\\PythonCore";
    ثابت حرف	*cp = libname;
    WCHAR	subkey[128];
    HKEY	h_کلید;
    صحيح		i;
    DWORD	j, len;
    LSTATUS	ret;

    طالما (*cp != '\0')
    {
	إذا (*cp == ':' || *cp == '\\' || *cp == '/')
	{
	    // Bail out if "libname" contains path separator, assume it is
	    // an absolute path.
	    أرجع عدم;
	}
	++cp;
    }

    WCHAR   keyfound[32];
    HKEY    h_کلید_بالاترین[] = {HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE};
    HKEY    h_کلید_یافت = عدم;
#  ifdef USE_LIMITED_API
    دراز    maxminor = -1;
#  endif
    لكل (i = 0; i < ARRAY_درازی(h_کلید_بالاترین); i++)
    {
	دراز	major, minor;

	ret = Reg_Open_کلید_Ex_W(h_کلید_بالاترین[i], pythoncore, 0, کلید_READ, &h_کلید);
	إذا (ret != ERROR_SUCCESS)
	    استمر;
	لكل (j = 0;; j++)
	{
	    WCHAR   keyname[32];
	    WCHAR   *wp;

	    len = ARRAY_درازی(keyname);
	    ret = Reg_Enum_کلید_Ex_W(h_کلید, j, keyname, &len,
						    عدم, عدم, عدم, عدم);
	    إذا (ret == ERROR_NO_بیشتر_اقلام)
		اكسر;

	    major = wcstol(keyname, &wp, 10);
	    إذا (*wp != L'.')
		استمر;
	    minor = wcstol(wp + 1, &wp, 10);
#  ifdef _WIN64
	    إذا (*wp != L'\0')
		استمر;
#  وإلا
	    إذا (wcscmp(wp, L"-32") != 0)
		استمر;
#  endif

	    إذا (major != PY_MAJOR_نسخه)
		استمر;
#  ifdef USE_LIMITED_API
	    // Search the latest version.
	    إذا ((minor > maxminor)
		    && (minor >= ((Py_LIMITED_API >> 16) & 0xff)))
	    {
		maxminor = minor;
		wcscpy(keyfound, keyname);
		h_کلید_یافت = h_کلید_بالاترین[i];
	    }
#  وإلا
	    // Check if it matches with the compiled version.
	    إذا (minor == PY_MINOR_نسخه)
	    {
		wcscpy(keyfound, keyname);
		h_کلید_یافت = h_کلید_بالاترین[i];
		اكسر;
	    }
#  endif
	}
	Reg_Close_کلید(h_کلید);
#  ifdef USE_LIMITED_API
	إذا (h_کلید_یافت != عدم)
	    اكسر;
#  endif
    }
    إذا (h_کلید_یافت == عدم)
	أرجع عدم;

    swprintf(subkey, ARRAY_درازی(subkey), L"%ls\\%ls\\InstallPath",
							pythoncore, keyfound);
    ret = Reg_Get_مقدار_W(h_کلید_یافت, subkey, عدم, RRF_RT_REG_SZ,
							    عدم, عدم, &len);
    إذا (ret != ERROR_بیشتر_داده && ret != ERROR_SUCCESS)
	أرجع عدم;
    size_t len2 = len / حجم(WCHAR) + 1 + طول_رشته(libname);
    WCHAR *مسیر = alloc(len2 * حجم(WCHAR));
    إذا (مسیر == عدم)
	أرجع عدم;
    ret = Reg_Get_مقدار_W(h_کلید_یافت, subkey, عدم, RRF_RT_REG_SZ,
							    عدم, مسیر, &len);
    إذا (ret != ERROR_SUCCESS)
    {
	vim_free(مسیر);
	أرجع عدم;
    }
    // Remove trailing path separators.
    size_t len3 = wcslen(مسیر);
    إذا ((len3 > 0) && (مسیر[len3 - 1] == L'/' || مسیر[len3 - 1] == L'\\'))
	--len3;
    swprintf(مسیر + len3, len2 - len3, L"\\%hs", libname);
    أرجع مسیر;
}
# endif

/*
 * Load library and get all pointers.
 * Parameter 'libname' provides name of DLL.
 * Return OK or FAIL.
 */
    ساكن صحيح
py3_runtime_link_init(حرف *libname, صحيح verbose)
{
    صحيح i;
    PYTHON_PROC *ucs_from_string = (PYTHON_PROC *)&py3_PyUnicode_FromString;
    PYTHON_PROC *ucs_decode = (PYTHON_PROC *)&py3_PyUnicode_Decode;
    PYTHON_PROC *ucs_as_encoded_string =
				 (PYTHON_PROC *)&py3_PyUnicode_AsEncodedString;

# إذا !(defined(PY_NO_RTLD_GLOBAL) && defined(PY3_NO_RTLD_GLOBAL)) && defined(UNIX) && defined(FEAT_PYTHON)
    // Can't have Python and Python3 loaded at the same time.
    // It causes a crash, because RTLD_GLOBAL is needed for
    // standard C extension libraries of one or both python versions.
    إذا (python_loaded())
    {
	إذا (verbose)
	    emsg(_(e_this_vim_cannot_execute_py3_after_using_python));
	أرجع FAIL;
    }
# endif

    إذا (hinstPy3 != 0)
	أرجع OK;
    hinstPy3 = بارگیری_dll(libname);

# ifdef MSWIN
    إذا (!hinstPy3)
    {
	// Attempt to use the path from InstallPath as stored in the registry.
	WCHAR *syslibname = py3_get_system_libname(libname);

	إذا (syslibname != عدم)
	{
	    hinstPy3 = بارگیری_Library_Ex_W(syslibname, عدم,
		    بارگیری_LIBRARY_جستجو_DLL_بارگیری_DIR |
		    بارگیری_LIBRARY_جستجو_SYSTEM32);
	    vim_free(syslibname);
	}
    }
# endif

    إذا (!hinstPy3)
    {
	إذا (verbose)
	    semsg(_(e_could_not_بارگیری_library_str_str), libname, بارگیری_dll_error());
	أرجع FAIL;
    }

    لكل (i = 0; py3_funcname_جدول[i].ptr; ++i)
    {
	إذا ((*py3_funcname_جدول[i].ptr = نماد_from_dll(hinstPy3,
			py3_funcname_جدول[i].name)) == عدم)
	{
	    close_dll(hinstPy3);
	    hinstPy3 = 0;
	    إذا (verbose)
		semsg(_(e_could_not_بارگیری_library_function_str), py3_funcname_جدول[i].name);
	    أرجع FAIL;
	}
    }

    // Load unicode functions separately as only the ucs2 or the ucs4 functions
    // will be present in the library.
# إذا PY_نسخه_HEX >= 0x030300f0
    *ucs_from_string = نماد_from_dll(hinstPy3, "PyUnicode_FromString");
    *ucs_decode = نماد_from_dll(hinstPy3, "PyUnicode_Decode");
    *ucs_as_encoded_string = نماد_from_dll(hinstPy3,
	    "PyUnicode_AsEncodedString");
# وإلا
    *ucs_from_string = نماد_from_dll(hinstPy3, "PyUnicodeUCS2_FromString");
    *ucs_decode = نماد_from_dll(hinstPy3,
	    "PyUnicodeUCS2_Decode");
    *ucs_as_encoded_string = نماد_from_dll(hinstPy3,
	    "PyUnicodeUCS2_AsEncodedString");
    إذا (*ucs_from_string == عدم || *ucs_decode == عدم
					     || *ucs_as_encoded_string == عدم)
    {
	*ucs_from_string = نماد_from_dll(hinstPy3,
		"PyUnicodeUCS4_FromString");
	*ucs_decode = نماد_from_dll(hinstPy3,
		"PyUnicodeUCS4_Decode");
	*ucs_as_encoded_string = نماد_from_dll(hinstPy3,
		"PyUnicodeUCS4_AsEncodedString");
    }
# endif
    إذا (*ucs_from_string == عدم || *ucs_decode == عدم
					     || *ucs_as_encoded_string == عدم)
    {
	close_dll(hinstPy3);
	hinstPy3 = 0;
	إذا (verbose)
	    semsg(_(e_could_not_بارگیری_library_function_str), "PyUnicode_UCSX_*");
	أرجع FAIL;
    }

    أرجع OK;
}

/*
 * If python is enabled (there is installed python on Windows system) return
 * TRUE, else FALSE.
 */
    صحيح
python3_enabled(صحيح verbose)
{
    أرجع py3_runtime_link_init((حرف *)p_py3dll, verbose) == OK;
}

/*
 * Load the standard Python exceptions - don't import the symbols from the
 * DLL, as this can cause errors (importing data symbols is not reliable).
 */
    ساكن فراغ
get_py3_exceptions(فراغ)
{
    PyObject *exmod = PyImport_ImportModule("builtins");
    PyObject *exdict = PyModule_GetDict(exmod);
    p3imp_PyExc_AttributeError = Py_Dict_Get_قلم_String(exdict, "AttributeError");
    p3imp_Py_Exc_شاخص_Error = Py_Dict_Get_قلم_String(exdict, "IndexError");
    p3imp_Py_Exc_کلید_Error = Py_Dict_Get_قلم_String(exdict, "KeyError");
    p3imp_Py_Exc_Keyboard_وقفه = Py_Dict_Get_قلم_String(exdict, "KeyboardInterrupt");
    p3imp_PyExc_TypeError = Py_Dict_Get_قلم_String(exdict, "TypeError");
    p3imp_Py_Exc_مقدار_Error = Py_Dict_Get_قلم_String(exdict, "ValueError");
    p3imp_PyExc_SystemExit = Py_Dict_Get_قلم_String(exdict, "SystemExit");
    p3imp_PyExc_RuntimeError = Py_Dict_Get_قلم_String(exdict, "RuntimeError");
    p3imp_PyExc_ImportError = Py_Dict_Get_قلم_String(exdict, "ImportError");
    p3imp_Py_Exc_سرریز_Error = Py_Dict_Get_قلم_String(exdict, "OverflowError");
    Py_XINCREF(p3imp_PyExc_AttributeError);
    Py_XINCREF(p3imp_Py_Exc_شاخص_Error);
    Py_XINCREF(p3imp_Py_Exc_کلید_Error);
    Py_XINCREF(p3imp_Py_Exc_Keyboard_وقفه);
    Py_XINCREF(p3imp_PyExc_TypeError);
    Py_XINCREF(p3imp_Py_Exc_مقدار_Error);
    Py_XINCREF(p3imp_PyExc_SystemExit);
    Py_XINCREF(p3imp_PyExc_RuntimeError);
    Py_XINCREF(p3imp_PyExc_ImportError);
    Py_XINCREF(p3imp_Py_Exc_سرریز_Error);
    Py_XDECREF(exmod);
}
#پایان_اگر // DYNAMIC_PYTHON3

ساكن صحيح py3initialised = 0;
#تعریف_کن PYINITIALISED py3initialised
ساكن صحيح python_انتها_called = FALSE;

#اگر_تعریف_شده USE_LIMITED_API
# define DESTRUCTOR_پایان(self) \
    ((freefunc)Py_Type_Get_جایگاه(Py_TYPE((PyObject*)self), Py_tp_free))((PyObject*)self)
#وإلا
# define DESTRUCTOR_پایان(self) Py_TYPE(self)->tp_free((PyObject*)self)
#پایان_اگر

#تعریف_کن WIN_PYTHON_REF(win) win->w_python3_ref
#تعریف_کن BUF_PYTHON_REF(buf) buf->b_python3_ref
#تعریف_کن TAB_PYTHON_REF(tab) tab->tp_python3_ref

    ساكن فراغ
call_PyObject_Free(فراغ *p)
{
#إذا defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
    _PyObject_DebugFree(p);
#وإلا
    PyObject_Free(p);
#پایان_اگر
}

    ساكن PyObject *
call_PyType_GenericNew(PyTypeObject *type, PyObject *آرگومانها, PyObject *kwds)
{
    أرجع PyType_GenericNew(type,آرگومانها,kwds);
}

    ساكن PyObject *
call_PyType_GenericAlloc(PyTypeObject *type, Py_ssize_t nitems)
{
    أرجع PyType_GenericAlloc(type,nitems);
}

ساكن PyObject *OutputGetattro(PyObject *, PyObject *);
ساكن صحيح OutputSetattro(PyObject *, PyObject *, PyObject *);
ساكن PyObject *میانگیر_Getattro(PyObject *, PyObject *);
ساكن صحيح میانگیر_Setattro(PyObject *, PyObject *, PyObject *);
ساكن PyObject *Tab_صفحه_Getattro(PyObject *, PyObject *);
ساكن PyObject *پنجره_Getattro(PyObject *, PyObject *);
ساكن صحيح پنجره_Setattro(PyObject *, PyObject *, PyObject *);
ساكن PyObject *RangeGetattro(PyObject *, PyObject *);
ساكن PyObject *CurrentGetattro(PyObject *, PyObject *);
ساكن صحيح CurrentSetattro(PyObject *, PyObject *, PyObject *);
ساكن PyObject *DictionaryGetattro(PyObject *, PyObject *);
ساكن صحيح DictionarySetattro(PyObject *, PyObject *, PyObject *);
ساكن PyObject *ListGetattro(PyObject *, PyObject *);
ساكن صحيح ListSetattro(PyObject *, PyObject *, PyObject *);
ساكن PyObject *TupleGetattro(PyObject *, PyObject *);
ساكن صحيح TupleSetattro(PyObject *, PyObject *, PyObject *);
ساكن PyObject *FunctionGetattro(PyObject *, PyObject *);

ساكن بنية PyModuleDef vimmodule;

#تعریف_کن PY_CAN_RECURSE

/*
 * Include the code shared with if_python.c
 */
#ضمن "if_py_both.h"

#اگر_تعریف_شده USE_LIMITED_API
# إذا Py_LIMITED_API >= 0x030A0000
#  define PY_UNICODE_GET_UTF8_CHARS(obj) Py_Unicode_As_UTF8_And_اندازه(obj, عدم)
# وإلا
// Python limited API before 3.10 lack easy ways to query the raw UTF-8 chars.
// We need to first convert the string to bytes, and then extract the chars.
// This function is only used for attribute string comparisons, which have
// known short length. As such, just allocate a short static buffer to hold
// the characters instead of having to allocate/deallcoate it.
//
// An alternative would be to convert all attribute string comparisons to use
// PyUnicode_CompareWithASCIIString to skip having to extract the chars.
ساكن حرف py3_unicode_utf8_chars[20];
ساكن حرف* PY_UNICODE_GET_UTF8_CHARS(PyObject* str)
{
    py3_unicode_utf8_chars[0] = '\0';
    PyObject* بایتها = PyUnicode_AsUTF8String(str);
    إذا (بایتها)
    {
	حرف *chars;
	Py_ssize_t len;
	إذا (Py_بایتها_As_String_And_اندازه(بایتها, &chars, &len) != -1)
	{
	    إذا (len < (Py_ssize_t)حجم(py3_unicode_utf8_chars))
		// PyBytes_AsStringAndSize guarantees null-termination
		کپی_حافظه(py3_unicode_utf8_chars, chars, len + 1);
	}
	Py_DECREF(بایتها);
    }
    أرجع py3_unicode_utf8_chars;
}
# endif
#وإلا	// !USE_LIMITED_API
# إذا PY_نسخه_HEX >= 0x030300f0
#  define PY_UNICODE_GET_UTF8_CHARS(obj) Py_Unicode_As_UTF8_And_اندازه(obj, عدم)
# وإلا
#  define PY_UNICODE_GET_UTF8_CHARS _PyUnicode_AsString
# endif
#پایان_اگر

// NOTE: Must always be used at the start of a block, since it declares "name".
#تعریف_کن GET_ATTR_STRING(name, nameobj) \
    حرف	*name = ""; \
    إذا (PyUnicode_Check(nameobj)) \
	name = (حرف *)PY_UNICODE_GET_UTF8_CHARS(nameobj)

#تعریف_کن PY3OBJ_DELETED(obj) (obj->ob_پایه.ob_refcnt<=0)

///////////////////////////////////////////////////////
// Internal function prototypes.

ساكن PyObject *Py3Init_vim(فراغ);

///////////////////////////////////////////////////////
// 1. Python interpreter main program.

    فراغ
python3_انتها(فراغ)
{
    ساكن صحيح recurse = 0;

    // If a crash occurs while doing this, don't try again.
    إذا (recurse != 0)
	أرجع;

    python_انتها_called = TRUE;
    ++recurse;

#اگر_تعریف_شده DYNAMIC_PYTHON3
    إذا (hinstPy3)
#پایان_اگر
    إذا (Py_IsInitialized())
    {
#اگر_تعریف_شده USE_LIMITED_API
	shutdown_types();
#پایان_اگر

	// acquire lock before finalizing
	PyGILState_Ensure();

	Py_Finalize();
    }

    --recurse;
}

#إذا defined(DYNAMIC_PYTHON3) && defined(DYNAMIC_PYTHON) && defined(FEAT_PYTHON) && defined(UNIX)
    صحيح
python3_loaded(فراغ)
{
    أرجع (hinstPy3 != 0);
}
#پایان_اگر

ساكن wchar_t *py_home_buf = عدم;

#إذا defined(MSWIN) && (PY_نسخه_HEX >= 0x030500f0)
/*
 * Return TRUE if stdin is readable from Python 3.
 */
    ساكن BOOL
is_stdin_readable(فراغ)
{
    DWORD	    نما, eventnum;
    بنية _stat    st;
    صحيح		    fd = fileno(stdin);
    HANDLE	    hstdin = (HANDLE)_get_osfhandle(fd);

    // Check if stdin is connected to the console.
    إذا (Get_Console_نما(hstdin, &نما))
	// Check if it is opened as input.
	أرجع Get_عدد_از_Console_درون_داد_Events(hstdin, &eventnum);

    أرجع _fstat(fd, &st) == 0;
}

// Python 3.5 or later will abort inside Py_Initialize() when stdin has
// been closed (i.e. executed by "vim -").  Reconnect stdin to CONIN$.
// Note that the python DLL is linked to its own stdio DLL which can be
// differ from Vim's stdio.
    ساكن فراغ
reset_stdin(فراغ)
{
    FILE *(*py__acrt_iob_func)(بیعلامت) = عدم;
    FILE *(*pyfreopen)(ثابت حرف *, ثابت حرف *, FILE *) = عدم;
    HINSTANCE hinst = get_forwarded_dll(hinstPy3);

    إذا (hinst == عدم || is_stdin_readable())
	أرجع;

    // Get "freopen" and "stdin" which are used in the python DLL.
    // "stdin" is defined as "__acrt_iob_func(0)" in VC++ 2015 or later.
    py__acrt_iob_func = get_dll_import_func(hinst, "__acrt_iob_func");
    إذا (py__acrt_iob_func)
    {
	HINSTANCE hpystdiodll = پیدا_کن_imported_module_by_funcname(hinst,
							    "__acrt_iob_func");
	إذا (hpystdiodll)
	    pyfreopen = (فراغ *)Get_Proc_آدرس(hpystdiodll, "freopen");
    }

    // Reconnect stdin to CONIN$.
    إذا (pyfreopen != عدم)
	pyfreopen("CONIN$", "r", py__acrt_iob_func(0));
    وإلا
	freopen("CONIN$", "r", stdin);
}
#وإلا
# define reset_stdin()
#پایان_اگر

// Python 3.2 or later will abort inside Py_Initialize() when mandatory
// modules cannot be loaded (e.g. 'pythonthreehome' is wrongly set.).
// Install a hook to python dll's exit() and recover from it.
#إذا defined(MSWIN) && (PY_نسخه_HEX >= 0x030200f0)
# define HOOK_EXIT
# include <setjmp.h>

ساكن jmp_buf exit_hook_jump_buf;
ساكن فراغ *orig_exit = عدم;

/*
 * Function that replaces exit() while calling Py_Initialize().
 */
    ساكن فراغ
hooked_exit(صحيح ret)
{
    // Recover from exit.
    longjmp(exit_hook_jump_buf, 1);
}

/*
 * Install a hook to python dll's exit().
 */
    ساكن فراغ
hook_py_exit(فراغ)
{
    HINSTANCE hinst = get_forwarded_dll(hinstPy3);

    إذا (hinst == عدم || orig_exit != عدم)
	أرجع;

    orig_exit = hook_dll_import_func(hinst, "exit", (فراغ *)hooked_exit);
}

/*
 * Remove the hook installed by hook_py_exit().
 */
    ساكن فراغ
restore_py_exit(فراغ)
{
    HINSTANCE hinst = hinstPy3;

    إذا (hinst == عدم)
	أرجع;

    إذا (orig_exit != عدم)
	hook_dll_import_func(hinst, "exit", orig_exit);
    orig_exit = عدم;
}
#پایان_اگر

    ساكن صحيح
Python3_Init(فراغ)
{
    إذا (!py3initialised)
    {
#اگر_تعریف_شده DYNAMIC_PYTHON3
	إذا (!python3_enabled(TRUE))
	{
	    emsg(_(e_sorry_this_command_is_disabled_python_library_could_not_be_یافت));
	    اذهب fail;
	}
#پایان_اگر
	// Python 3.13 introduced a really useful feature: colorized exceptions.
	// This is great if you're reading them from the terminal, but useless
	// and broken everywhere else (such as in log files, or text editors).
	// Opt out, forcefully.
	vim_setenv((char_u*)"PYTHON_COLORS", (char_u*)"0");

	init_structs();

	إذا (*p_py3home != NUL)
	{
	    size_t len = mbstowcs(عدم, (حرف *)p_py3home, 0) + 1;

	    // The string must not change later, make a copy in static memory.
	    py_home_buf = ALLOC_MULT(wchar_t, len);
	    إذا (py_home_buf != عدم && mbstowcs(
			    py_home_buf, (حرف *)p_py3home, len) != (size_t)-1)
		Py_مجموعه_Python_Home(py_home_buf);
	}
#اگر_تعریف_شده PYTHON3_HOME
	وإلا إذا (mch_getenv((char_u *)"PYTHONHOME") == عدم)
	    Py_مجموعه_Python_Home(PYTHON3_HOME);
#پایان_اگر

	PyImport_AppendInittab("vim", Py3Init_vim);

#إذا !defined(DYNAMIC_PYTHON3) && defined(MSWIN)
	hinstPy3 = GetModuleHandle(PYTHON3_DLL);
#پایان_اگر
	reset_stdin();

#اگر_تعریف_شده HOOK_EXIT
	// Catch exit() called in Py_Initialize().
	hook_py_exit();
	إذا (setjmp(exit_hook_jump_buf) == 0)
	{
	    Py_Initialize();
	    restore_py_exit();
	}
	وإلا
	{
	    // exit() was called in Py_Initialize().
	    restore_py_exit();
	    emsg(_(e_critical_error_درون_python3_initialization_check_your_installation));
	    اذهب fail;
	}
#وإلا
	Py_Initialize();
#پایان_اگر

#إذا PY_نسخه_HEX < 0x03090000
	// Initialise threads.  This is deprecated since Python 3.9.
	Py_Eval_Init_نخها();
#پایان_اگر
#اگر_تعریف_شده DYNAMIC_PYTHON3
	get_py3_exceptions();
#پایان_اگر

	إذا (PythonIO_Init_io())
	    اذهب fail;

	globals = PyModule_GetDict(Py_Import_جمع_کن_Module("__main__"));

	// Remove the element from sys.path that was added because of our
	// argv[0] value in Py3Init_vim().  Previously we used an empty
	// string, but depending on the OS we then get an empty entry or
	// the current directory in sys.path.
	// Only after vim has been imported, the element does exist in
	// sys.path.
	Py_پیاپی_Simple_String("import vim; import sys; sys.path = list(filter(lambda x: not x.endswith('must>not&exist'), sys.path))");

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

    أرجع 0;

fail:
    // We call PythonIO_Flush() here to print any Python errors.
    // This is OK, as it is possible to call this function even
    // if PythonIO_Init_io() has not completed successfully (it will
    // not do anything in this case).
    PythonIO_Flush();
    أرجع -1;
}

/*
 * External interface
 */
    ساكن فراغ
DoPyCommand(ثابت حرف *cmd,
	    dict_T* locals,
	    rangeinitializer init_range,
	    runner پیاپی,
	    فراغ *arg)
{
#إذا defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    حرف		*saved_locale;
#پایان_اگر
    PyObject		*cmdstr;
    PyObject		*cmdbytes;
    PyGILState_STATE	pygilstate;

    إذا (python_انتها_called)
	اذهب theend;

    إذا (Python3_Init())
	اذهب theend;

    init_range(arg);

    Python_Release_Vim();	    // leave Vim

#إذا defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    // Python only works properly when the LC_NUMERIC locale is "C".
    saved_locale = setlocale(LC_NUMERIC, عدم);
    إذا (saved_locale == عدم || STRCMP(saved_locale, "C") == 0)
	saved_locale = عدم;
    وإلا
    {
	// Need to make a copy, value may change when setting new locale.
	saved_locale = (حرف *)vim_strsave((char_u *)saved_locale);
	(فراغ)setlocale(LC_NUMERIC, "C");
    }
#پایان_اگر

    pygilstate = PyGILState_Ensure();

    // PyRun_SimpleString expects a UTF-8 string. Wrong encoding may cause
    // SyntaxError (unicode error).
    cmdstr = PyUnicode_Decode(cmd, طول_رشته(cmd),
					(حرف *)ENC_OPT, ERRORS_DECODE_ARG);
    cmdbytes = PyUnicode_AsEncodedString(cmdstr, "utf-8", ERRORS_ENCODE_ARG);
    Py_XDECREF(cmdstr);

    پیاپی(Py_بایتها_As_String(cmdbytes), locals, arg, &pygilstate);
    Py_XDECREF(cmdbytes);

    PyGILState_Release(pygilstate);

#إذا defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    إذا (saved_locale != عدم)
    {
	(فراغ)setlocale(LC_NUMERIC, saved_locale);
	vim_free(saved_locale);
    }
#پایان_اگر

    Python_قفل_Vim();		    // enter Vim
    PythonIO_Flush();

theend:
    أرجع;	    // keeps lint happy
}

/*
 * ":py3"
 */
    فراغ
ex_py3(exarg_T *eap)
{
    char_u *script;

    script = script_get(eap, eap->arg);
    إذا (!eap->skip)
    {
	إذا (p_pyx == 0)
	    p_pyx = 3;

	DoPyCommand(script == عدم ? (حرف *) eap->arg : (حرف *) script,
		عدم,
		init_range_cmd,
		(runner) پیاپی_cmd,
		(فراغ *) eap);
    }
    vim_free(script);
}

#تعریف_کن میانگیر_اندازه 2048

/*
 * ":py3file"
 */
    فراغ
ex_py3file(exarg_T *eap)
{
    ساكن حرف میانگیر[میانگیر_اندازه];
    ثابت حرف *file;
    حرف *p;
    صحيح i;

    إذا (p_pyx == 0)
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

    strcpy(میانگیر, "exec(compile(open('");
    p = میانگیر + 19; // size of "exec(compile(open('"

    لكل (i=0; i<2; ++i)
    {
	file = (حرف *)eap->arg;
	طالما (*file && p < میانگیر + (میانگیر_اندازه - 3))
	{
	    إذا (*file == '\\' || *file == '\'')
		*p++ = '\\';
	    *p++ = *file++;
	}
	// If we didn't finish the file name, we hit a buffer overflow
	إذا (*file != '\0')
	    أرجع;
	إذا (i==0)
	{
	    strcpy(p,"','rb').read(),'");
	    p += 16;
	}
	وإلا
	{
	    strcpy(p,"','exec'))");
	    p += 10;
	}
    }


    // Execute the file
    DoPyCommand(میانگیر,
	    عدم,
	    init_range_cmd,
	    (runner) پیاپی_cmd,
	    (فراغ *) eap);
}

    فراغ
ex_py3do(exarg_T *eap)
{
    إذا (p_pyx == 0)
	p_pyx = 3;

    DoPyCommand((حرف *)eap->arg,
	    عدم,
	    init_range_cmd,
	    (runner)پیاپی_do,
	    (فراغ *)eap);
}

///////////////////////////////////////////////////////
// 2. Python output stream: writes output via [e]msg().

// Implementation functions

    ساكن PyObject *
OutputGetattro(PyObject *self, PyObject *nameobj)
{
    GET_ATTR_STRING(name, nameobj);

    إذا (strcmp(name, "softspace") == 0)
	أرجع PyLong_FromLong(((OutputObject *)(self))->softspace);
    وإلا إذا (strcmp(name, "errors") == 0)
	أرجع PyString_FromString("strict");
    وإلا إذا (strcmp(name, "encoding") == 0)
	أرجع PyString_FromString(ENC_OPT);

    أرجع PyObject_GenericGetAttr(self, nameobj);
}

    ساكن صحيح
OutputSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);

    أرجع OutputSetattr(self, name, val);
}

///////////////////////////////////////////////////////
// 3. Implementation of the Vim module for Python

// Window type - Implementation functions
// --------------------------------------

#تعریف_کن پنجره_Type_Check(obj) ((obj)->ob_پایه.ob_type == &پنجره_Type)

// Buffer type - Implementation functions
// --------------------------------------

#تعریف_کن میانگیر_Type_Check(obj) ((obj)->ob_پایه.ob_type == &میانگیر_Type)

ساكن PyObject* میانگیر_Subscript(PyObject *self, PyObject *idx);
ساكن صحيح میانگیر_As_Subscript(PyObject *self, PyObject *idx, PyObject *val);

// Line range type - Implementation functions
// --------------------------------------

#تعریف_کن RangeType_Check(obj) ((obj)->ob_پایه.ob_type == &RangeType)

ساكن PyObject* RangeSubscript(PyObject *self, PyObject *idx);
ساكن صحيح Range_As_قلم(PyObject *, Py_ssize_t, PyObject *);
ساكن صحيح RangeAsSubscript(PyObject *self, PyObject *idx, PyObject *val);

// Current objects type - Implementation functions
// -----------------------------------------------

ساكن PySequenceMethods میانگیر_As_Seq = {
    (lenfunc)		میانگیر_درازی,	    // sq_length,    len(x)
    (binaryfunc)	0,		    // sq_concat,    x+y
    (ssizeargfunc)	0,		    // sq_repeat,    x*n
    (ssizeargfunc)	میانگیر_قلم,	    // sq_item,      x[i]
    0,					    // was_sq_slice,	 x[i:j]
    0,					    // sq_ass_item,  x[i]=v
    0,					    // sq_ass_slice, x[i:j]=v
    0,					    // sq_contains
    0,					    // sq_inplace_concat
    0,					    // sq_inplace_repeat
};

ساكن PyMappingMethods میانگیر_As_Mapping = {
    /* mp_length	*/ (lenfunc)میانگیر_درازی,
    /* mp_subscript     */ (binaryfunc)میانگیر_Subscript,
    /* mp_ass_subscript */ (objobjargproc)میانگیر_As_Subscript,
};


// Buffer object

    ساكن PyObject *
میانگیر_Getattro(PyObject *self, PyObject *nameobj)
{
    PyObject *r;

    GET_ATTR_STRING(name, nameobj);

    إذا ((r = میانگیر_Attr_Valid((میانگیر_Object *)(self), name)))
	أرجع r;

    إذا (Check_میانگیر((میانگیر_Object *)(self)))
	أرجع عدم;

    r = میانگیر_Attr((میانگیر_Object *)(self), name);
    إذا (r || PyErr_Occurred())
	أرجع r;
    وإلا
	أرجع PyObject_GenericGetAttr(self, nameobj);
}

    ساكن صحيح
میانگیر_Setattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);

    أرجع میانگیر_Setattr(self, name, val);
}

//////////////////

    ساكن PyObject *
میانگیر_Subscript(PyObject *self, PyObject* idx)
{
    إذا (PyLong_Check(idx))
    {
	دراز _idx = PyLong_AsLong(idx);
	أرجع میانگیر_قلم((میانگیر_Object *)(self), _idx);
    }
    وإلا إذا (Py_برش_Check(idx))
    {
	Py_ssize_t شروع, stop, گام, slicelen;

	إذا (Check_میانگیر((میانگیر_Object *) self))
	    أرجع عدم;

	إذا (Py_برش_Get_Indices_Ex((Py_برش_Object_T *)idx,
	      (Py_ssize_t)((میانگیر_Object *)(self))->buf->b_ml.ml_سطر_شمار,
	      &شروع, &stop,
	      &گام, &slicelen) < 0)
	    أرجع عدم;
	أرجع میانگیر_برش((میانگیر_Object *)(self), شروع, stop);
    }
    وإلا
    {
	RAISE_INVALID_شاخص_TYPE(idx);
	أرجع عدم;
    }
}

    ساكن صحيح
میانگیر_As_Subscript(PyObject *self, PyObject* idx, PyObject* val)
{
    إذا (PyLong_Check(idx))
    {
	دراز n = PyLong_AsLong(idx);

	إذا (Check_میانگیر((میانگیر_Object *) self))
	    أرجع -1;

	أرجع RBAs_قلم((میانگیر_Object *)(self), n, val, 1,
		    (Py_ssize_t)((میانگیر_Object *)(self))->buf->b_ml.ml_سطر_شمار,
		    عدم);
    }
    وإلا إذا (Py_برش_Check(idx))
    {
	Py_ssize_t شروع, stop, گام, slicelen;

	إذا (Check_میانگیر((میانگیر_Object *) self))
	    أرجع -1;

	إذا (Py_برش_Get_Indices_Ex((Py_برش_Object_T *)idx,
	      (Py_ssize_t)((میانگیر_Object *)(self))->buf->b_ml.ml_سطر_شمار,
	      &شروع, &stop,
	      &گام, &slicelen) < 0)
	    أرجع -1;
	أرجع RBAs_برش((میانگیر_Object *)(self), شروع, stop, val, 1,
			  (PyInt)((میانگیر_Object *)(self))->buf->b_ml.ml_سطر_شمار,
			  عدم);
    }
    وإلا
    {
	RAISE_INVALID_شاخص_TYPE(idx);
	أرجع -1;
    }
}

ساكن PySequenceMethods RangeAsSeq = {
    (lenfunc)		Range_درازی,	 // sq_length,	  len(x)
    (binaryfunc)	0,		 // RangeConcat, sq_concat,  x+y
    (ssizeargfunc)	0,		 // RangeRepeat, sq_repeat,  x*n
    (ssizeargfunc)	Range_قلم,	 // sq_item,	  x[i]
    0,					 // was_sq_slice,     x[i:j]
    (ssizeobjargproc)	Range_As_قلم,	 // sq_as_item,  x[i]=v
    0,					 // sq_ass_slice, x[i:j]=v
    0,					 // sq_contains
    0,					 // sq_inplace_concat
    0,					 // sq_inplace_repeat
};

ساكن PyMappingMethods RangeAsMapping = {
    /* mp_length	*/ (lenfunc)Range_درازی,
    /* mp_subscript     */ (binaryfunc)RangeSubscript,
    /* mp_ass_subscript */ (objobjargproc)RangeAsSubscript,
};

// Line range object - Implementation

    ساكن PyObject *
RangeGetattro(PyObject *self, PyObject *nameobj)
{
    GET_ATTR_STRING(name, nameobj);

    إذا (strcmp(name, "start") == 0)
	أرجع Py_Build_مقدار("n", ((RangeObject *)(self))->شروع - 1);
    وإلا إذا (strcmp(name, "end") == 0)
	أرجع Py_Build_مقدار("n", ((RangeObject *)(self))->انتها - 1);
    وإلا
	أرجع PyObject_GenericGetAttr(self, nameobj);
}

////////////////

    ساكن صحيح
Range_As_قلم(PyObject *self, Py_ssize_t n, PyObject *val)
{
    أرجع RBAs_قلم(((RangeObject *)(self))->buf, n, val,
		    ((RangeObject *)(self))->شروع,
		    ((RangeObject *)(self))->انتها,
		    &((RangeObject *)(self))->انتها);
}

    ساكن Py_ssize_t
Range_As_برش(PyObject *self, Py_ssize_t lo, Py_ssize_t hi, PyObject *val)
{
    أرجع RBAs_برش(((RangeObject *)(self))->buf, lo, hi, val,
		    ((RangeObject *)(self))->شروع,
		    ((RangeObject *)(self))->انتها,
		    &((RangeObject *)(self))->انتها);
}

    ساكن PyObject *
RangeSubscript(PyObject *self, PyObject* idx)
{
    إذا (PyLong_Check(idx))
    {
	دراز _idx = PyLong_AsLong(idx);
	أرجع Range_قلم((RangeObject *)(self), _idx);
    }
    وإلا إذا (Py_برش_Check(idx))
    {
	Py_ssize_t شروع, stop, گام, slicelen;

	إذا (Py_برش_Get_Indices_Ex((Py_برش_Object_T *)idx,
		((RangeObject *)(self))->انتها-((RangeObject *)(self))->شروع+1,
		&شروع, &stop,
		&گام, &slicelen) < 0)
	    أرجع عدم;
	أرجع Range_برش((RangeObject *)(self), شروع, stop);
    }
    وإلا
    {
	RAISE_INVALID_شاخص_TYPE(idx);
	أرجع عدم;
    }
}

    ساكن صحيح
RangeAsSubscript(PyObject *self, PyObject *idx, PyObject *val)
{
    إذا (PyLong_Check(idx))
    {
	دراز n = PyLong_AsLong(idx);
	أرجع Range_As_قلم(self, n, val);
    }
    وإلا إذا (Py_برش_Check(idx))
    {
	Py_ssize_t شروع, stop, گام, slicelen;

	إذا (Py_برش_Get_Indices_Ex((Py_برش_Object_T *)idx,
		((RangeObject *)(self))->انتها-((RangeObject *)(self))->شروع+1,
		&شروع, &stop,
		&گام, &slicelen) < 0)
	    أرجع -1;
	أرجع Range_As_برش(self, شروع, stop, val);
    }
    وإلا
    {
	RAISE_INVALID_شاخص_TYPE(idx);
	أرجع -1;
    }
}

// TabPage object - Implementation

    ساكن PyObject *
Tab_صفحه_Getattro(PyObject *self, PyObject *nameobj)
{
    PyObject *r;

    GET_ATTR_STRING(name, nameobj);

    إذا ((r = Tab_صفحه_Attr_Valid((Tab_صفحه_Object *)(self), name)))
	أرجع r;

    إذا (Check_Tab_صفحه((Tab_صفحه_Object *)(self)))
	أرجع عدم;

    r = Tab_صفحه_Attr((Tab_صفحه_Object *)(self), name);
    إذا (r || PyErr_Occurred())
	أرجع r;
    وإلا
	أرجع PyObject_GenericGetAttr(self, nameobj);
}

// Window object - Implementation

    ساكن PyObject *
پنجره_Getattro(PyObject *self, PyObject *nameobj)
{
    PyObject *r;

    GET_ATTR_STRING(name, nameobj);

    إذا ((r = پنجره_Attr_Valid((پنجره_Object *)(self), name)))
	أرجع r;

    إذا (Check_پنجره((پنجره_Object *)(self)))
	أرجع عدم;

    r = پنجره_Attr((پنجره_Object *)(self), name);
    إذا (r || PyErr_Occurred())
	أرجع r;
    وإلا
	أرجع PyObject_GenericGetAttr(self, nameobj);
}

    ساكن صحيح
پنجره_Setattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);

    أرجع پنجره_Setattr(self, name, val);
}

// Tab page list object - Definitions

ساكن PySequenceMethods TabListAsSeq = {
    (lenfunc)	     Tab_List_درازی,	    // sq_length,    len(x)
    (binaryfunc)     0,			    // sq_concat,    x+y
    (ssizeargfunc)   0,			    // sq_repeat,    x*n
    (ssizeargfunc)   Tab_List_قلم,	    // sq_item,      x[i]
    0,					    // sq_slice,     x[i:j]
    (ssizeobjargproc)0,			    // sq_as_item,  x[i]=v
    0,					    // sq_ass_slice, x[i:j]=v
    0,					    // sq_contains
    0,					    // sq_inplace_concat
    0,					    // sq_inplace_repeat
};

// Window list object - Definitions

ساكن PySequenceMethods WinListAsSeq = {
    (lenfunc)	     Win_List_درازی,	    // sq_length,    len(x)
    (binaryfunc)     0,			    // sq_concat,    x+y
    (ssizeargfunc)   0,			    // sq_repeat,    x*n
    (ssizeargfunc)   Win_List_قلم,	    // sq_item,      x[i]
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
    ساكن PyObject *
CurrentGetattro(PyObject *self, PyObject *nameobj)
{
    PyObject	*r;
    GET_ATTR_STRING(name, nameobj);
    إذا (!(r = CurrentGetattr(self, name)))
	أرجع PyObject_GenericGetAttr(self, nameobj);
    أرجع r;
}

    ساكن صحيح
CurrentSetattro(PyObject *self, PyObject *nameobj, PyObject *مقدار)
{
    GET_ATTR_STRING(name, nameobj);
    أرجع CurrentSetattr(self, name, مقدار);
}

// Dictionary object - Definitions

    ساكن PyObject *
DictionaryGetattro(PyObject *self, PyObject *nameobj)
{
    DictionaryObject	*this = ((DictionaryObject *) (self));

    GET_ATTR_STRING(name, nameobj);

    إذا (strcmp(name, "locked") == 0)
	أرجع PyLong_FromLong(this->dict->dv_قفل);
    وإلا إذا (strcmp(name, "scope") == 0)
	أرجع PyLong_FromLong(this->dict->dv_scope);

    أرجع PyObject_GenericGetAttr(self, nameobj);
}

    ساكن صحيح
DictionarySetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);
    أرجع DictionarySetattr(self, name, val);
}

// List object - Definitions

    ساكن PyObject *
ListGetattro(PyObject *self, PyObject *nameobj)
{
    GET_ATTR_STRING(name, nameobj);

    إذا (strcmp(name, "locked") == 0)
	أرجع PyLong_FromLong(((ListObject *) (self))->list->lv_قفل);

    أرجع PyObject_GenericGetAttr(self, nameobj);
}

    ساكن صحيح
ListSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);
    أرجع ListSetattr(self, name, val);
}

// Tuple object - Definitions

    ساكن PyObject *
TupleGetattro(PyObject *self, PyObject *nameobj)
{
    GET_ATTR_STRING(name, nameobj);

    إذا (strcmp(name, "locked") == 0)
	أرجع PyLong_FromLong(((TupleObject *) (self))->tuple->tv_قفل);

    أرجع PyObject_GenericGetAttr(self, nameobj);
}

    ساكن صحيح
TupleSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);
    أرجع TupleSetattr(self, name, val);
}

// Function object - Definitions

    ساكن PyObject *
FunctionGetattro(PyObject *self, PyObject *nameobj)
{
    PyObject		*r;
    FunctionObject	*this = (FunctionObject *)(self);

    GET_ATTR_STRING(name, nameobj);

    r = FunctionAttr(this, name);
    إذا (r || PyErr_Occurred())
	أرجع r;
    وإلا
	أرجع PyObject_GenericGetAttr(self, nameobj);
}

// External interface

    فراغ
python3_میانگیر_free(buf_T *buf)
{
    میانگیر_Object *bp = BUF_PYTHON_REF(buf);
    إذا (bp == عدم)
	أرجع;
    bp->buf = INVALID_میانگیر_مقدار;
    BUF_PYTHON_REF(buf) = عدم;
}

    فراغ
python3_پنجره_free(win_T *win)
{
    پنجره_Object *wp = WIN_PYTHON_REF(win);
    إذا (wp == عدم)
	أرجع;
    wp->win = INVALID_پنجره_مقدار;
    WIN_PYTHON_REF(win) = عدم;
}

    فراغ
python3_tabpage_free(tabpage_T *tab)
{
    Tab_صفحه_Object *tp = TAB_PYTHON_REF(tab);
    إذا (tp == عدم)
	أرجع;
    tp->tab = INVALID_TABPAGE_مقدار;
    TAB_PYTHON_REF(tab) = عدم;
}

    ساكن PyObject *
Py3Init_vim(فراغ)
{
    // The special value is removed from sys.path in Python3_Init().
    ساكن wchar_t *(argv[2]) = {L"/must>not&exist/foo", عدم};

    إذا (init_types())
	أرجع عدم;

    // Set sys.argv[] to avoid a crash in warn().
    Py_Sys_مجموعه_Argv(1, argv);

    إذا ((vim_module = PyModule_Create(&vimmodule)) == عدم)
	أرجع عدم;

    إذا (populate_module(vim_module))
	أرجع عدم;

    إذا (init_sys_مسیر())
	أرجع عدم;

    أرجع vim_module;
}

//////////////////////////////////////////////////////////////////////////
// 4. Utility functions for handling the interface between Vim and Python.

/*
 * Convert a Vim line into a Python string.
 * All internal newlines are replaced by null characters.
 *
 * On errors, the Python exception data is set, and NULL is returned.
 */
    ساكن PyObject *
سطر_مقصد_String(ثابت حرف *str)
{
    PyObject *نتیجه;
    Py_ssize_t len = طول_رشته(str);
    حرف *tmp, *p;

    tmp = alloc(len + 1);
    p = tmp;
    إذا (p == عدم)
    {
	Py_Err_No_حافظه();
	أرجع عدم;
    }

    طالما (*str)
    {
	إذا (*str == '\n')
	    *p = '\0';
	وإلا
	    *p = *str;

	++p;
	++str;
    }
    *p = '\0';

    نتیجه = PyUnicode_Decode(tmp, len, (حرف *)ENC_OPT, ERRORS_DECODE_ARG);

    vim_free(tmp);
    أرجع نتیجه;
}

    فراغ
do_py3eval(char_u *str, dict_T *locals, typval_T *rettv)
{
    DoPyCommand((حرف *) str,
	    locals,
	    init_range_eval,
	    (runner) پیاپی_eval,
	    (فراغ *) rettv);
    إذا (rettv->v_type == VAR_UNKNOWN)
    {
	rettv->v_type = VAR_عدد;
	rettv->vval.v_عدد = 0;
    }
}

    صحيح
مجموعه_ref_درون_python3(صحيح copyID)
{
    أرجع مجموعه_ref_درون_py(copyID);
}

    صحيح
python3_نسخه(فراغ)
{
#اگر_تعریف_شده USE_LIMITED_API
    أرجع Py_LIMITED_API;
#وإلا
    أرجع PY_نسخه_HEX;
#پایان_اگر
}
