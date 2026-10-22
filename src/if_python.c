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

#شامل "vim.h"

#شامل <limits.h>

// uncomment this if used with the debug version of python.
// Checked on 2.7.4.
// #define Py_DEBUG
// Note: most of time you can add -DPy_DEBUG to CFLAGS in place of uncommenting
// uncomment this if used with the debug version of python, but without its
// allocator
// #define Py_DEBUG_NO_PYMALLOC

// Python.h defines _POSIX_THREADS itself (if needed)
#اگر_تعریف_شدہ _POSIX_THREADS
# undef _POSIX_THREADS
#اگر_ختم

#اگر defined(MSWIN) && defined(HAVE_FCNTL_H)
# undef HAVE_FCNTL_H
#اگر_ختم

#اگر_تعریف_شدہ _DEBUG
# undef _DEBUG
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
#اگر_تعریف_شدہ _POSIX_C_SOURCE
# undef _POSIX_C_SOURCE	// pyconfig.h defines it as well.
#اگر_ختم
#اگر_تعریف_شدہ XOPEN_ماخذ
# undef XOPEN_ماخذ	// pyconfig.h defines it as well.
#اگر_ختم

#تعریف_کرو PY_SSIZE_T_CLEAN

#شامل <Python.h>

#اگر !defined(PY_نسخہ_HEX) || PY_نسخہ_HEX < 0x02050000
# undef PY_SSIZE_T_CLEAN
#اگر_ختم

// these are NULL for Python 2
#تعریف_کرو ERRORS_DECODE_ARG تہی
#تعریف_کرو ERRORS_ENCODE_ARG ERRORS_DECODE_ARG

#تعریف_منسوخ مین // Defined in python.h - aargh
#تعریف_منسوخ HAVE_FCNTL_H // Clash with os_win32.h

// Perhaps leave this out for Python 2.6, which supports bytes?
#تعریف_کرو Py_بائٹیں_From_String      PyString_FromString
#تعریف_کرو Py_بائٹیں_Check		PyString_Check
#تعریف_کرو Py_بائٹیں_As_String_And_سائز Py_String_As_String_And_سائز
#تعریف_کرو Py_بائٹیں_From_String_And_سائز   Py_String_From_String_And_سائز

#اگر defined(PY_نسخہ_HEX) && PY_نسخہ_HEX >= 0x02070000
# define PY_USE_CAPSULE
#اگر_ختم

#اگر defined(PY_نسخہ_HEX) && PY_نسخہ_HEX >= 0x02050000
# define PyInt Py_ssize_t
# define PyInquiry lenfunc
# define PyIntArgFunc ssizeargfunc
# define PyIntIntArgFunc ssizessizeargfunc
# define PyIntObjArgProc ssizeobjargproc
# define PyIntIntObjArgProc ssizessizeobjargproc
# define Py_ssize_t_fmt "n"
#ورنہ
# define PyInt صحیح
# define lenfunc inquiry
# define PyInquiry inquiry
# define PyIntArgFunc intargfunc
# define PyIntIntArgFunc intintargfunc
# define PyIntObjArgProc intobjargproc
# define PyIntIntObjArgProc intintobjargproc
# define Py_ssize_t_fmt "i"
#اگر_ختم
#تعریف_کرو Py_بائٹیں_fmt "s"

// Parser flags
#تعریف_کرو single_داخلہ	256
#تعریف_کرو file_داخلہ	257
#تعریف_کرو eval_داخلہ	258

#اگر defined(PY_نسخہ_HEX) && PY_نسخہ_HEX >= 0x020300F0
  // Python 2.3: can invoke ":python" recursively.
# define PY_CAN_RECURSE
#اگر_ختم

#اگر defined(DYNAMIC_PYTHON)
# ifndef DYNAMIC_PYTHON
#  define HINSTANCE long_u		// for generating prototypes
# endif

# ifdef MSWIN
#  define بھرو_dll vim_بھرو_Lib
#  define close_dll FreeLibrary
#  define علامت_from_dll Get_Proc_پتہ
#  define بھرو_dll_error GetWin32Error
# ورنہ
#  include <dlfcn.h>
#  define FARPROC خالی*
#  define HINSTANCE خالی*
#  اگر defined(PY_NO_RTLD_GLOBAL) && defined(PY3_NO_RTLD_GLOBAL)
#   define بھرو_dll(n) dlopen((n), RTLD_LAZY)
#  ورنہ
#   define بھرو_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#  endif
#  define close_dll dlclose
#  define علامت_from_dll dlsym
#  define بھرو_dll_error dlerror
# endif

// This makes if_python.c compile without warnings against Python 2.5
// on Win32 and Win64.
# undef Py_تسلسل_Simple_String
# undef Py_تسلسل_String
# undef PyArg_Parse
# undef PyArg_ParseTuple
# undef Py_Build_قدر
# undef Py_InitModule4
# undef Py_InitModule4_64
# undef Py_Object_Call_طریقہ
# undef PyObject_CallFunction

/*
 * Wrapper defines
 */
# define PyArg_Parse dll_PyArg_Parse
# define PyArg_ParseTuple dll_PyArg_ParseTuple
# define PyMem_Free dll_PyMem_Free
# define PyMem_Malloc dll_PyMem_Malloc
# define Py_Dict_سیٹ_شے_String dll_Py_Dict_سیٹ_شے_String
# define Py_Err_Bad_دلیل dll_Py_Err_Bad_دلیل
# define PyErr_NewException dll_PyErr_NewException
# define PyErr_Clear dll_PyErr_Clear
# define PyErr_Format dll_PyErr_Format
# define PyErr_PrintEx dll_PyErr_PrintEx
# define Py_Err_No_حافظہ dll_Py_Err_No_حافظہ
# define PyErr_Occurred dll_PyErr_Occurred
# define Py_Err_سیٹ_None dll_Py_Err_سیٹ_None
# define Py_Err_سیٹ_String dll_Py_Err_سیٹ_String
# define Py_Err_سیٹ_Object dll_Py_Err_سیٹ_Object
# define PyErr_ExceptionMatches dll_PyErr_ExceptionMatches
# define Py_Eval_Init_دھاگے dll_Py_Eval_Init_دھاگے
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
# define Py_List_Get_شے dll_Py_List_Get_شے
# define PyList_Append dll_PyList_Append
# define Py_List_داخل_کرو dll_Py_List_داخل_کرو
# define PyList_New dll_PyList_New
# define Py_List_سیٹ_شے_2 dll_Py_List_سیٹ_شے
# define Py_List_سائز dll_Py_List_سائز
# define PyList_Type (*dll_PyList_Type)
# define Py_Tuple_Get_شے dll_Py_Tuple_Get_شے
# define Py_Tuple_سیٹ_شے dll_Py_Tuple_سیٹ_شے
# define PyTuple_New dll_PyTuple_New
# define Py_Tuple_سائز dll_Py_Tuple_سائز
# define PyTuple_Type (*dll_PyTuple_Type)
# define PySequence_Check dll_PySequence_Check
# define Py_Sequence_سائز dll_Py_Sequence_سائز
# define Py_Sequence_Get_شے dll_Py_Sequence_Get_شے
# define PySequence_Fast dll_PySequence_Fast
# define Py_قاش_Get_Indices_Ex dll_Py_قاش_Get_Indices_Ex
# define PyImport_ImportModule dll_PyImport_ImportModule
# define PyDict_New dll_PyDict_New
# define Py_Dict_Get_شے_String dll_Py_Dict_Get_شے_String
# define Py_Dict_اگلا dll_Py_Dict_اگلا
# define PyDict_Type (*dll_PyDict_Type)
# ifdef Py_Mapping_کلیدیں
#  define PY_NO_MAPPING_کلیدیں
# ورنہ
#  define Py_Mapping_کلیدیں dll_Py_Mapping_کلیدیں
# endif
# define Py_Object_Get_شے dll_Py_Object_Get_شے
# define Py_Object_Call_طریقہ dll_Py_Object_Call_طریقہ
# define PyMapping_Check dll_PyMapping_Check
# define Py_Iter_اگلا dll_Py_Iter_اگلا
# define PyModule_GetDict dll_PyModule_GetDict
# define Py_Module_جوڑو_Object dll_Py_Module_جوڑو_Object
# define Py_تسلسل_Simple_String dll_Py_تسلسل_Simple_String
# define Py_تسلسل_String dll_Py_تسلسل_String
# define PyObject_GetAttrString dll_PyObject_GetAttrString
# define PyObject_HasAttrString dll_PyObject_HasAttrString
# define Py_Object_سیٹ_Attr_String dll_Py_Object_سیٹ_Attr_String
# define Py_Object_Call_Function_Obj_دلائل dll_Py_Object_Call_Function_Obj_دلائل
# define PyObject_CallFunction dll_PyObject_CallFunction
# define PyObject_Call dll_PyObject_Call
# define PyObject_Repr dll_PyObject_Repr
# define PyString_AsString dll_PyString_AsString
# define Py_String_As_String_And_سائز dll_Py_String_As_String_And_سائز
# define PyString_FromString dll_PyString_FromString
# define PyString_FromFormat dll_PyString_FromFormat
# define Py_String_From_String_And_سائز dll_Py_String_From_String_And_سائز
# define Py_String_سائز dll_Py_String_سائز
# define PyString_Type (*dll_PyString_Type)
# define PyUnicode_Type (*dll_PyUnicode_Type)
# undef PyUnicode_AsEncodedString
# define PyUnicode_AsEncodedString py_PyUnicode_AsEncodedString
# define PyFloat_AsDouble dll_PyFloat_AsDouble
# define PyFloat_FromDouble dll_PyFloat_FromDouble
# define PyFloat_Type (*dll_PyFloat_Type)
# define Py_نمبر_Check dll_Py_نمبر_Check
# define Py_نمبر_Long dll_Py_نمبر_Long
# define Py_Import_جوڑو_Module (*dll_Py_Import_جوڑو_Module)
# define Py_Sys_سیٹ_Object dll_Py_Sys_سیٹ_Object
# define PySys_GetObject dll_PySys_GetObject
# define Py_Sys_سیٹ_Argv dll_Py_Sys_سیٹ_Argv
# define PyType_Type (*dll_PyType_Type)
# define PyFile_Type (*dll_PyFile_Type)
# define Py_قاش_Type (*dll_Py_قاش_Type)
# define PyType_Ready (*dll_PyType_Ready)
# define PyType_GenericAlloc dll_PyType_GenericAlloc
# define Py_Build_قدر dll_Py_Build_قدر
# define Py_ڈھونڈو_طریقہ dll_Py_ڈھونڈو_طریقہ
# define Py_InitModule4 dll_Py_InitModule4
# define Py_سیٹ_Python_Home dll_Py_سیٹ_Python_Home
# define Py_Initialize dll_Py_Initialize
# define Py_Finalize dll_Py_Finalize
# define Py_IsInitialized dll_Py_IsInitialized
# define _PyObject_New dll__PyObject_New
# define _PyObject_GC_New dll__PyObject_GC_New
# ifdef PyObject_GC_Del
#  define Py_underscore_GC
#  define _PyObject_GC_Del dll__PyObject_GC_Del
#  define _PyObject_GC_UnTrack dll__PyObject_GC_UnTrack
# ورنہ
#  define PyObject_GC_Del dll_PyObject_GC_Del
#  define PyObject_GC_UnTrack dll_PyObject_GC_UnTrack
# endif
# اگر defined(PY_نسخہ_HEX) && PY_نسخہ_HEX >= 0x02070000
#  define Py_Object_اگلا_Not_Implemented (*dll_Py_Object_اگلا_Not_Implemented)
# endif
# define _Py_NoneStruct (*dll__Py_NoneStruct)
# define _Py_ZeroStruct (*dll__Py_ZeroStruct)
# define _Py_TrueStruct (*dll__Py_TrueStruct)
# define PyObject_Init dll__PyObject_Init
# define PyObject_GetIter dll_PyObject_GetIter
# define PyObject_IsTrue dll_PyObject_IsTrue
# اگر defined(PY_نسخہ_HEX) && PY_نسخہ_HEX >= 0x02020000
#  define PyType_IsSubtype dll_PyType_IsSubtype
#  ifdef Py_DEBUG
#   define _Py_NegativeRefcount dll__Py_NegativeRefcount
#   define Py_Ref_کل (*dll_Py_Ref_کل)
#   define _Py_Dealloc dll__Py_Dealloc
#  endif
# endif
# اگر defined(PY_نسخہ_HEX) && PY_نسخہ_HEX >= 0x02030000
#  اگر defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
#   define _PyObject_DebugMalloc dll__PyObject_DebugMalloc
#   define _PyObject_DebugFree dll__PyObject_DebugFree
#  ورنہ
#   define PyObject_Malloc dll_PyObject_Malloc
#   define PyObject_Free dll_PyObject_Free
#  endif
# endif
# ifdef PY_USE_CAPSULE
#  define PyCapsule_New dll_PyCapsule_New
#  define Py_Capsule_Get_اشارہ_گر dll_Py_Capsule_Get_اشارہ_گر
# ورنہ
#  define PyCObject_FromVoidPtr dll_PyCObject_FromVoidPtr
#  define PyCObject_AsVoidPtr dll_PyCObject_AsVoidPtr
# endif
# اگر defined(PY_نسخہ_HEX) && PY_نسخہ_HEX >= 0x02070000
#  define Py_NoSiteFlag (*dll_Py_NoSiteFlag)
# endif

/*
 * Pointers for dynamic link
 */
ساکن صحیح(*dll_PyArg_Parse)(PyObject *, حرف *, ...);
ساکن صحیح(*dll_PyArg_ParseTuple)(PyObject *, حرف *, ...);
ساکن خالی(*dll_PyMem_Free)(خالی *);
ساکن خالی* (*dll_PyMem_Malloc)(size_t);
ساکن صحیح(*dll_Py_Dict_سیٹ_شے_String)(PyObject *dp, حرف *کلید, PyObject *شے);
ساکن صحیح(*dll_Py_Err_Bad_دلیل)(خالی);
ساکن PyObject *(*dll_PyErr_NewException)(حرف *, PyObject *, PyObject *);
ساکن خالی(*dll_PyErr_Clear)(خالی);
ساکن PyObject*(*dll_PyErr_Format)(PyObject *, ثابت حرف *, ...);
ساکن خالی(*dll_PyErr_PrintEx)(صحیح);
ساکن PyObject*(*dll_Py_Err_No_حافظہ)(خالی);
ساکن PyObject*(*dll_PyErr_Occurred)(خالی);
ساکن خالی(*dll_Py_Err_سیٹ_None)(PyObject *);
ساکن خالی(*dll_Py_Err_سیٹ_String)(PyObject *, ثابت حرف *);
ساکن خالی(*dll_Py_Err_سیٹ_Object)(PyObject *, PyObject *);
ساکن صحیح(*dll_PyErr_ExceptionMatches)(PyObject *);
ساکن خالی(*dll_Py_Eval_Init_دھاگے)(خالی);
ساکن خالی(*dll_PyEval_RestoreThread)(PyThreadState *);
ساکن PyThreadState*(*dll_PyEval_SaveThread)(خالی);
# ifdef PY_CAN_RECURSE
ساکن PyGILState_STATE	(*dll_PyGILState_Ensure)(خالی);
ساکن خالی (*dll_PyGILState_Release)(PyGILState_STATE);
# endif
ساکن طویل(*dll_PyInt_AsLong)(PyObject *);
ساکن PyObject*(*dll_PyInt_FromLong)(طویل);
ساکن طویل(*dll_PyLong_AsLong)(PyObject *);
ساکن PyObject*(*dll_PyLong_FromLong)(طویل);
ساکن PyTypeObject* dll_PyBool_Type;
ساکن PyTypeObject* dll_PyInt_Type;
ساکن PyTypeObject* dll_PyLong_Type;
ساکن PyObject*(*dll_Py_List_Get_شے)(PyObject *, PyInt);
ساکن صحیح(*dll_PyList_Append)(PyObject *, PyObject *);
ساکن صحیح(*dll_Py_List_داخل_کرو)(PyObject *, PyInt, PyObject *);
ساکن PyObject*(*dll_PyList_New)(PyInt سائز);
ساکن صحیح(*dll_Py_List_سیٹ_شے)(PyObject *, PyInt, PyObject *);
ساکن PyInt(*dll_Py_List_سائز)(PyObject *);
ساکن PyTypeObject* dll_PyList_Type;
ساکن PyObject*(*dll_Py_Tuple_Get_شے)(PyObject *, PyInt);
ساکن صحیح(*dll_Py_Tuple_سیٹ_شے)(PyObject *, PyInt, PyObject *);
ساکن PyObject*(*dll_PyTuple_New)(PyInt سائز);
ساکن PyInt(*dll_Py_Tuple_سائز)(PyObject *);
ساکن PyTypeObject* dll_PyTuple_Type;
ساکن صحیح (*dll_PySequence_Check)(PyObject *);
ساکن PyInt(*dll_Py_Sequence_سائز)(PyObject *);
ساکن PyObject*(*dll_Py_Sequence_Get_شے)(PyObject *, PyInt);
ساکن PyObject*(*dll_PySequence_Fast)(PyObject *, ثابت حرف *);
ساکن صحیح (*dll_Py_قاش_Get_Indices_Ex)(Py_قاش_Object *r, PyInt لمبائی,
		     PyInt *آغاز, PyInt *stop, PyInt *قدم,
		     PyInt *slicelen);
ساکن PyObject*(*dll_PyImport_ImportModule)(ثابت حرف *);
ساکن PyObject*(*dll_PyDict_New)(خالی);
ساکن PyObject*(*dll_Py_Dict_Get_شے_String)(PyObject *, ثابت حرف *);
ساکن صحیح (*dll_Py_Dict_اگلا)(PyObject *, PyInt *, PyObject **, PyObject **);
ساکن PyTypeObject* dll_PyDict_Type;
# ifndef PY_NO_MAPPING_کلیدیں
ساکن PyObject* (*dll_Py_Mapping_کلیدیں)(PyObject *);
# endif
ساکن PyObject* (*dll_Py_Object_Get_شے)(PyObject *, PyObject *);
ساکن PyObject* (*dll_Py_Object_Call_طریقہ)(PyObject *, حرف *, PyObject *);
ساکن صحیح (*dll_PyMapping_Check)(PyObject *);
ساکن PyObject* (*dll_Py_Iter_اگلا)(PyObject *);
ساکن PyObject*(*dll_PyModule_GetDict)(PyObject *);
ساکن صحیح(*dll_Py_Module_جوڑو_Object)(PyObject *, ثابت حرف *, PyObject *);
ساکن صحیح(*dll_Py_تسلسل_Simple_String)(حرف *);
ساکن PyObject *(*dll_Py_تسلسل_String)(حرف *, صحیح, PyObject *, PyObject *);
ساکن PyObject* (*dll_PyObject_GetAttrString)(PyObject *, ثابت حرف *);
ساکن صحیح (*dll_PyObject_HasAttrString)(PyObject *, ثابت حرف *);
ساکن صحیح (*dll_Py_Object_سیٹ_Attr_String)(PyObject *, ثابت حرف *, PyObject *);
ساکن PyObject* (*dll_Py_Object_Call_Function_Obj_دلائل)(PyObject *, ...);
ساکن PyObject* (*dll_PyObject_CallFunction)(PyObject *, حرف *, ...);
ساکن PyObject* (*dll_PyObject_Call)(PyObject *, PyObject *, PyObject *);
ساکن PyObject* (*dll_PyObject_Repr)(PyObject *);
ساکن حرف*(*dll_PyString_AsString)(PyObject *);
ساکن صحیح(*dll_Py_String_As_String_And_سائز)(PyObject *, حرف **, PyInt *);
ساکن PyObject*(*dll_PyString_FromString)(ثابت حرف *);
ساکن PyObject*(*dll_PyString_FromFormat)(ثابت حرف *, ...);
ساکن PyObject*(*dll_Py_String_From_String_And_سائز)(ثابت حرف *, PyInt);
ساکن PyInt(*dll_Py_String_سائز)(PyObject *);
ساکن PyTypeObject* dll_PyString_Type;
ساکن PyTypeObject* dll_PyUnicode_Type;
ساکن PyObject *(*py_PyUnicode_AsEncodedString)(PyObject *, حرف *, حرف *);
ساکن دوہرا(*dll_PyFloat_AsDouble)(PyObject *);
ساکن PyObject*(*dll_PyFloat_FromDouble)(دوہرا);
ساکن PyTypeObject* dll_PyFloat_Type;
ساکن صحیح(*dll_Py_نمبر_Check)(PyObject *);
ساکن PyObject*(*dll_Py_نمبر_Long)(PyObject *);
ساکن صحیح(*dll_Py_Sys_سیٹ_Object)(حرف *, PyObject *);
ساکن PyObject *(*dll_PySys_GetObject)(حرف *);
ساکن صحیح(*dll_Py_Sys_سیٹ_Argv)(صحیح, حرف **);
ساکن PyTypeObject* dll_PyType_Type;
ساکن PyTypeObject* dll_PyFile_Type;
ساکن PyTypeObject* dll_Py_قاش_Type;
ساکن صحیح (*dll_PyType_Ready)(PyTypeObject *type);
ساکن PyObject* (*dll_PyType_GenericAlloc)(PyTypeObject *type, PyInt nitems);
ساکن PyObject*(*dll_Py_Build_قدر)(حرف *, ...);
ساکن PyObject*(*dll_Py_ڈھونڈو_طریقہ)(ساخت Py_طریقہ_Def[], PyObject *, حرف *);
ساکن PyObject*(*dll_Py_InitModule4)(حرف *, ساخت Py_طریقہ_Def *, حرف *, PyObject *, صحیح);
ساکن PyObject*(*dll_Py_Import_جوڑو_Module)(حرف *);
ساکن خالی(*dll_Py_سیٹ_Python_Home)(حرف *home);
ساکن خالی(*dll_Py_Initialize)(خالی);
ساکن خالی(*dll_Py_Finalize)(خالی);
ساکن صحیح(*dll_Py_IsInitialized)(خالی);
ساکن PyObject*(*dll__PyObject_New)(PyTypeObject *, PyObject *);
ساکن PyObject*(*dll__PyObject_GC_New)(PyTypeObject *);
# ifdef Py_underscore_GC
ساکن خالی(*dll__PyObject_GC_Del)(خالی *);
ساکن خالی(*dll__PyObject_GC_UnTrack)(خالی *);
# ورنہ
ساکن خالی(*dll_PyObject_GC_Del)(خالی *);
ساکن خالی(*dll_PyObject_GC_UnTrack)(خالی *);
# endif
ساکن PyObject*(*dll__PyObject_Init)(PyObject *, PyTypeObject *);
ساکن PyObject* (*dll_PyObject_GetIter)(PyObject *);
ساکن صحیح (*dll_PyObject_IsTrue)(PyObject *);
# اگر defined(PY_نسخہ_HEX) && PY_نسخہ_HEX >= 0x02070000
ساکن iternextfunc dll_Py_Object_اگلا_Not_Implemented;
# endif
ساکن PyObject* dll__Py_NoneStruct;
ساکن PyObject* _Py_ZeroStruct;
ساکن PyObject* dll__Py_TrueStruct;
# اگر defined(PY_نسخہ_HEX) && PY_نسخہ_HEX >= 0x02020000
ساکن صحیح (*dll_PyType_IsSubtype)(PyTypeObject *, PyTypeObject *);
#  ifdef Py_DEBUG
ساکن خالی (*dll__Py_NegativeRefcount)(ثابت حرف *fname, صحیح lineno, PyObject *op);
ساکن PyInt* dll_Py_Ref_کل;
ساکن خالی (*dll__Py_Dealloc)(PyObject *obj);
#  endif
# endif
# اگر defined(PY_نسخہ_HEX) && PY_نسخہ_HEX >= 0x02030000
#  اگر defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
ساکن خالی (*dll__PyObject_DebugFree)(خالی*);
ساکن خالی* (*dll__PyObject_DebugMalloc)(size_t);
#  ورنہ
ساکن خالی* (*dll_PyObject_Malloc)(size_t);
ساکن خالی (*dll_PyObject_Free)(خالی*);
#  endif
# endif
# ifdef PY_USE_CAPSULE
ساکن PyObject* (*dll_PyCapsule_New)(خالی *, حرف *, PyCapsule_Destructor);
ساکن خالی* (*dll_Py_Capsule_Get_اشارہ_گر)(PyObject *, حرف *);
# ورنہ
ساکن PyObject* (*dll_PyCObject_FromVoidPtr)(خالی *cobj, خالی (*destr)(خالی *));
ساکن خالی* (*dll_PyCObject_AsVoidPtr)(PyObject *);
# endif
# اگر defined(PY_نسخہ_HEX) && PY_نسخہ_HEX >= 0x02070000
ساکن صحیح* dll_Py_NoSiteFlag;
# endif

ساکن HINSTANCE hinstPython = 0; // Instance of python.dll

// Imported exception objects
ساکن PyObject *imp_PyExc_AttributeError;
ساکن PyObject *imp_Py_Exc_اشاریہ_Error;
ساکن PyObject *imp_Py_Exc_کلید_Error;
ساکن PyObject *imp_Py_Exc_Keyboard_مداخلت;
ساکن PyObject *imp_PyExc_TypeError;
ساکن PyObject *imp_Py_Exc_قدر_Error;
ساکن PyObject *imp_PyExc_SystemExit;
ساکن PyObject *imp_PyExc_RuntimeError;
ساکن PyObject *imp_PyExc_ImportError;
ساکن PyObject *imp_Py_Exc_حد_سے_تجاوز_Error;

# define PyExc_AttributeError imp_PyExc_AttributeError
# define Py_Exc_اشاریہ_Error imp_Py_Exc_اشاریہ_Error
# define Py_Exc_کلید_Error imp_Py_Exc_کلید_Error
# define Py_Exc_Keyboard_مداخلت imp_Py_Exc_Keyboard_مداخلت
# define PyExc_TypeError imp_PyExc_TypeError
# define Py_Exc_قدر_Error imp_Py_Exc_قدر_Error
# define PyExc_SystemExit imp_PyExc_SystemExit
# define PyExc_RuntimeError imp_PyExc_RuntimeError
# define PyExc_ImportError imp_PyExc_ImportError
# define Py_Exc_حد_سے_تجاوز_Error imp_Py_Exc_حد_سے_تجاوز_Error

/*
 * Table of name to function pointer of python.
 */
# define PYTHON_PROC FARPROC
ساکن ساخت
{
    حرف *name;
    PYTHON_PROC *ptr;
} python_funcname_جدول[] =
{
# ifdef PY_SSIZE_T_CLEAN
    {"_PyArg_Parse_SizeT", (PYTHON_PROC*)&dll_PyArg_Parse},
    {"_PyArg_ParseTuple_SizeT", (PYTHON_PROC*)&dll_PyArg_ParseTuple},
    {"_Py_BuildValue_SizeT", (PYTHON_PROC*)&dll_Py_Build_قدر},
# ورنہ
    {"PyArg_Parse", (PYTHON_PROC*)&dll_PyArg_Parse},
    {"PyArg_ParseTuple", (PYTHON_PROC*)&dll_PyArg_ParseTuple},
    {"Py_BuildValue", (PYTHON_PROC*)&dll_Py_Build_قدر},
# endif
    {"PyMem_Free", (PYTHON_PROC*)&dll_PyMem_Free},
    {"PyMem_Malloc", (PYTHON_PROC*)&dll_PyMem_Malloc},
    {"PyDict_SetItemString", (PYTHON_PROC*)&dll_Py_Dict_سیٹ_شے_String},
    {"PyErr_BadArgument", (PYTHON_PROC*)&dll_Py_Err_Bad_دلیل},
    {"PyErr_NewException", (PYTHON_PROC*)&dll_PyErr_NewException},
    {"PyErr_Clear", (PYTHON_PROC*)&dll_PyErr_Clear},
    {"PyErr_Format", (PYTHON_PROC*)&dll_PyErr_Format},
    {"PyErr_PrintEx", (PYTHON_PROC*)&dll_PyErr_PrintEx},
    {"PyErr_NoMemory", (PYTHON_PROC*)&dll_Py_Err_No_حافظہ},
    {"PyErr_Occurred", (PYTHON_PROC*)&dll_PyErr_Occurred},
    {"PyErr_SetNone", (PYTHON_PROC*)&dll_Py_Err_سیٹ_None},
    {"PyErr_SetString", (PYTHON_PROC*)&dll_Py_Err_سیٹ_String},
    {"PyErr_SetObject", (PYTHON_PROC*)&dll_Py_Err_سیٹ_Object},
    {"PyErr_ExceptionMatches", (PYTHON_PROC*)&dll_PyErr_ExceptionMatches},
    {"PyEval_InitThreads", (PYTHON_PROC*)&dll_Py_Eval_Init_دھاگے},
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
    {"PyList_GetItem", (PYTHON_PROC*)&dll_Py_List_Get_شے},
    {"PyList_Append", (PYTHON_PROC*)&dll_PyList_Append},
    {"PyList_Insert", (PYTHON_PROC*)&dll_Py_List_داخل_کرو},
    {"PyList_New", (PYTHON_PROC*)&dll_PyList_New},
    {"PyList_SetItem", (PYTHON_PROC*)&dll_Py_List_سیٹ_شے},
    {"PyList_Size", (PYTHON_PROC*)&dll_Py_List_سائز},
    {"PyList_Type", (PYTHON_PROC*)&dll_PyList_Type},
    {"PyTuple_GetItem", (PYTHON_PROC*)&dll_Py_Tuple_Get_شے},
    {"PyTuple_SetItem", (PYTHON_PROC*)&dll_Py_Tuple_سیٹ_شے},
    {"PyTuple_New", (PYTHON_PROC*)&dll_PyTuple_New},
    {"PyTuple_Size", (PYTHON_PROC*)&dll_Py_Tuple_سائز},
    {"PyTuple_Type", (PYTHON_PROC*)&dll_PyTuple_Type},
    {"PySequence_Size", (PYTHON_PROC*)&dll_Py_Sequence_سائز},
    {"PySequence_Check", (PYTHON_PROC*)&dll_PySequence_Check},
    {"PySequence_GetItem", (PYTHON_PROC*)&dll_Py_Sequence_Get_شے},
    {"PySequence_Fast", (PYTHON_PROC*)&dll_PySequence_Fast},
    {"PySlice_GetIndicesEx", (PYTHON_PROC*)&dll_Py_قاش_Get_Indices_Ex},
    {"PyImport_ImportModule", (PYTHON_PROC*)&dll_PyImport_ImportModule},
    {"PyDict_GetItemString", (PYTHON_PROC*)&dll_Py_Dict_Get_شے_String},
    {"PyDict_Next", (PYTHON_PROC*)&dll_Py_Dict_اگلا},
    {"PyDict_New", (PYTHON_PROC*)&dll_PyDict_New},
    {"PyDict_Type", (PYTHON_PROC*)&dll_PyDict_Type},
# ifndef PY_NO_MAPPING_کلیدیں
    {"PyMapping_Keys", (PYTHON_PROC*)&dll_Py_Mapping_کلیدیں},
# endif
    {"PyObject_GetItem", (PYTHON_PROC*)&dll_Py_Object_Get_شے},
    {"PyObject_CallMethod", (PYTHON_PROC*)&dll_Py_Object_Call_طریقہ},
    {"PyMapping_Check", (PYTHON_PROC*)&dll_PyMapping_Check},
    {"PyIter_Next", (PYTHON_PROC*)&dll_Py_Iter_اگلا},
    {"PyModule_GetDict", (PYTHON_PROC*)&dll_PyModule_GetDict},
    {"PyModule_AddObject", (PYTHON_PROC*)&dll_Py_Module_جوڑو_Object},
    {"PyRun_SimpleString", (PYTHON_PROC*)&dll_Py_تسلسل_Simple_String},
    {"PyRun_String", (PYTHON_PROC*)&dll_Py_تسلسل_String},
    {"PyObject_GetAttrString", (PYTHON_PROC*)&dll_PyObject_GetAttrString},
    {"PyObject_HasAttrString", (PYTHON_PROC*)&dll_PyObject_HasAttrString},
    {"PyObject_SetAttrString", (PYTHON_PROC*)&dll_Py_Object_سیٹ_Attr_String},
    {"PyObject_CallFunctionObjArgs", (PYTHON_PROC*)&dll_Py_Object_Call_Function_Obj_دلائل},
    {"PyObject_CallFunction", (PYTHON_PROC*)&dll_PyObject_CallFunction},
    {"PyObject_Call", (PYTHON_PROC*)&dll_PyObject_Call},
    {"PyObject_Repr", (PYTHON_PROC*)&dll_PyObject_Repr},
    {"PyString_AsString", (PYTHON_PROC*)&dll_PyString_AsString},
    {"PyString_AsStringAndSize", (PYTHON_PROC*)&dll_Py_String_As_String_And_سائز},
    {"PyString_FromString", (PYTHON_PROC*)&dll_PyString_FromString},
    {"PyString_FromFormat", (PYTHON_PROC*)&dll_PyString_FromFormat},
    {"PyString_FromStringAndSize", (PYTHON_PROC*)&dll_Py_String_From_String_And_سائز},
    {"PyString_Size", (PYTHON_PROC*)&dll_Py_String_سائز},
    {"PyString_Type", (PYTHON_PROC*)&dll_PyString_Type},
    {"PyUnicode_Type", (PYTHON_PROC*)&dll_PyUnicode_Type},
    {"PyFloat_Type", (PYTHON_PROC*)&dll_PyFloat_Type},
    {"PyFloat_AsDouble", (PYTHON_PROC*)&dll_PyFloat_AsDouble},
    {"PyFloat_FromDouble", (PYTHON_PROC*)&dll_PyFloat_FromDouble},
    {"PyImport_AddModule", (PYTHON_PROC*)&dll_Py_Import_جوڑو_Module},
    {"PyNumber_Check", (PYTHON_PROC*)&dll_Py_نمبر_Check},
    {"PyNumber_Long", (PYTHON_PROC*)&dll_Py_نمبر_Long},
    {"PySys_SetObject", (PYTHON_PROC*)&dll_Py_Sys_سیٹ_Object},
    {"PySys_GetObject", (PYTHON_PROC*)&dll_PySys_GetObject},
    {"PySys_SetArgv", (PYTHON_PROC*)&dll_Py_Sys_سیٹ_Argv},
    {"PyType_Type", (PYTHON_PROC*)&dll_PyType_Type},
    {"PyFile_Type", (PYTHON_PROC*)&dll_PyFile_Type},
    {"PySlice_Type", (PYTHON_PROC*)&dll_Py_قاش_Type},
    {"PyType_Ready", (PYTHON_PROC*)&dll_PyType_Ready},
    {"PyType_GenericAlloc", (PYTHON_PROC*)&dll_PyType_GenericAlloc},
    {"Py_FindMethod", (PYTHON_PROC*)&dll_Py_ڈھونڈو_طریقہ},
    {"Py_SetPythonHome", (PYTHON_PROC*)&dll_Py_سیٹ_Python_Home},
    {"Py_Initialize", (PYTHON_PROC*)&dll_Py_Initialize},
    {"Py_Finalize", (PYTHON_PROC*)&dll_Py_Finalize},
    {"Py_IsInitialized", (PYTHON_PROC*)&dll_Py_IsInitialized},
    {"_PyObject_New", (PYTHON_PROC*)&dll__PyObject_New},
    {"_PyObject_GC_New", (PYTHON_PROC*)&dll__PyObject_GC_New},
# ifdef Py_underscore_GC
    {"_PyObject_GC_Del", (PYTHON_PROC*)&dll__PyObject_GC_Del},
    {"_PyObject_GC_UnTrack", (PYTHON_PROC*)&dll__PyObject_GC_UnTrack},
# ورنہ
    {"PyObject_GC_Del", (PYTHON_PROC*)&dll_PyObject_GC_Del},
    {"PyObject_GC_UnTrack", (PYTHON_PROC*)&dll_PyObject_GC_UnTrack},
# endif
    {"PyObject_Init", (PYTHON_PROC*)&dll__PyObject_Init},
    {"PyObject_GetIter", (PYTHON_PROC*)&dll_PyObject_GetIter},
    {"PyObject_IsTrue", (PYTHON_PROC*)&dll_PyObject_IsTrue},
# اگر defined(PY_نسخہ_HEX) && PY_نسخہ_HEX >= 0x02070000
    {"_PyObject_NextNotImplemented", (PYTHON_PROC*)&dll_Py_Object_اگلا_Not_Implemented},
# endif
    {"_Py_NoneStruct", (PYTHON_PROC*)&dll__Py_NoneStruct},
    {"_Py_ZeroStruct", (PYTHON_PROC*)&dll__Py_ZeroStruct},
    {"_Py_TrueStruct", (PYTHON_PROC*)&dll__Py_TrueStruct},
# اگر defined(PY_نسخہ_HEX) && PY_نسخہ_HEX >= 0x02020000
#  ifdef Py_DEBUG
    {"_Py_NegativeRefcount", (PYTHON_PROC*)&dll__Py_NegativeRefcount},
    {"_Py_RefTotal", (PYTHON_PROC*)&dll_Py_Ref_کل},
    {"_Py_Dealloc", (PYTHON_PROC*)&dll__Py_Dealloc},
#  endif
    {"PyType_IsSubtype", (PYTHON_PROC*)&dll_PyType_IsSubtype},
# endif
# اگر defined(PY_نسخہ_HEX) && PY_نسخہ_HEX >= 0x02030000
#  اگر defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
    {"_PyObject_DebugFree", (PYTHON_PROC*)&dll__PyObject_DebugFree},
    {"_PyObject_DebugMalloc", (PYTHON_PROC*)&dll__PyObject_DebugMalloc},
#  ورنہ
    {"PyObject_Malloc", (PYTHON_PROC*)&dll_PyObject_Malloc},
    {"PyObject_Free", (PYTHON_PROC*)&dll_PyObject_Free},
#  endif
# endif
# اگر defined(PY_نسخہ_HEX) && PY_نسخہ_HEX >= 0x02050000 \
	&& SIZEOF_سائز_T != VIM_SIZEOF_INT
#  ifdef Py_DEBUG
    {"Py_InitModule4TraceRefs_64", (PYTHON_PROC*)&dll_Py_InitModule4},
#  ورنہ
    {"Py_InitModule4_64", (PYTHON_PROC*)&dll_Py_InitModule4},
#  endif
# ورنہ
#  ifdef Py_DEBUG
    {"Py_InitModule4TraceRefs", (PYTHON_PROC*)&dll_Py_InitModule4},
#  ورنہ
    {"Py_InitModule4", (PYTHON_PROC*)&dll_Py_InitModule4},
#  endif
# endif
# ifdef PY_USE_CAPSULE
    {"PyCapsule_New", (PYTHON_PROC*)&dll_PyCapsule_New},
    {"PyCapsule_GetPointer", (PYTHON_PROC*)&dll_Py_Capsule_Get_اشارہ_گر},
# ورنہ
    {"PyCObject_FromVoidPtr", (PYTHON_PROC*)&dll_PyCObject_FromVoidPtr},
    {"PyCObject_AsVoidPtr", (PYTHON_PROC*)&dll_PyCObject_AsVoidPtr},
# endif
# اگر defined(PY_نسخہ_HEX) && PY_نسخہ_HEX >= 0x02070000
    {"Py_NoSiteFlag", (PYTHON_PROC*)&dll_Py_NoSiteFlag},
# endif
    {"", تہی},
};

/*
 * Load library and get all pointers.
 * Parameter 'libname' provides name of DLL.
 * Return OK or FAIL.
 */
    ساکن صحیح
python_runtime_link_init(حرف *libname, صحیح verbose)
{
    صحیح i;
    PYTHON_PROC *ucs_as_encoded_string =
				   (PYTHON_PROC*)&py_PyUnicode_AsEncodedString;

# اگر !(defined(PY_NO_RTLD_GLOBAL) && defined(PY3_NO_RTLD_GLOBAL)) && defined(UNIX) && defined(FEAT_PYTHON3)
    // Can't have Python and Python3 loaded at the same time.
    // It causes a crash, because RTLD_GLOBAL is needed for
    // standard C extension libraries of one or both python versions.
    اگر (python3_loaded())
    {
	اگر (verbose)
	    emsg(_(e_this_vim_cannot_execute_python_after_using_py3));
	واپس FAIL;
    }
# endif

    اگر (hinstPython)
	واپس OK;
    hinstPython = بھرو_dll(libname);
    اگر (!hinstPython)
    {
	اگر (verbose)
	    semsg(_(e_could_not_بھرو_library_str_str), libname, بھرو_dll_error());
	واپس FAIL;
    }

    برائے (i = 0; python_funcname_جدول[i].ptr; ++i)
    {
	اگر ((*python_funcname_جدول[i].ptr = علامت_from_dll(hinstPython,
			python_funcname_جدول[i].name)) == تہی)
	{
	    close_dll(hinstPython);
	    hinstPython = 0;
	    اگر (verbose)
		semsg(_(e_could_not_بھرو_library_function_str), python_funcname_جدول[i].name);
	    واپس FAIL;
	}
    }

    // Load unicode functions separately as only the ucs2 or the ucs4 functions
    // will be present in the library.
    *ucs_as_encoded_string = علامت_from_dll(hinstPython,
					     "PyUnicodeUCS2_AsEncodedString");
    اگر (*ucs_as_encoded_string == تہی)
	*ucs_as_encoded_string = علامت_from_dll(hinstPython,
					     "PyUnicodeUCS4_AsEncodedString");
    اگر (*ucs_as_encoded_string == تہی)
    {
	close_dll(hinstPython);
	hinstPython = 0;
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
python_enabled(صحیح verbose)
{
    واپس python_runtime_link_init((حرف *)p_pydll, verbose) == OK;
}

/*
 * Load the standard Python exceptions - don't import the symbols from the
 * DLL, as this can cause errors (importing data symbols is not reliable).
 */
    ساکن خالی
get_exceptions(خالی)
{
    PyObject *exmod = PyImport_ImportModule("exceptions");
    PyObject *exdict = PyModule_GetDict(exmod);
    imp_PyExc_AttributeError = Py_Dict_Get_شے_String(exdict, "AttributeError");
    imp_Py_Exc_اشاریہ_Error = Py_Dict_Get_شے_String(exdict, "IndexError");
    imp_Py_Exc_کلید_Error = Py_Dict_Get_شے_String(exdict, "KeyError");
    imp_Py_Exc_Keyboard_مداخلت = Py_Dict_Get_شے_String(exdict, "KeyboardInterrupt");
    imp_PyExc_TypeError = Py_Dict_Get_شے_String(exdict, "TypeError");
    imp_Py_Exc_قدر_Error = Py_Dict_Get_شے_String(exdict, "ValueError");
    imp_PyExc_SystemExit = Py_Dict_Get_شے_String(exdict, "SystemExit");
    imp_PyExc_RuntimeError = Py_Dict_Get_شے_String(exdict, "RuntimeError");
    imp_PyExc_ImportError = Py_Dict_Get_شے_String(exdict, "ImportError");
    imp_Py_Exc_حد_سے_تجاوز_Error = Py_Dict_Get_شے_String(exdict, "OverflowError");
    Py_XINCREF(imp_PyExc_AttributeError);
    Py_XINCREF(imp_Py_Exc_اشاریہ_Error);
    Py_XINCREF(imp_Py_Exc_کلید_Error);
    Py_XINCREF(imp_Py_Exc_Keyboard_مداخلت);
    Py_XINCREF(imp_PyExc_TypeError);
    Py_XINCREF(imp_Py_Exc_قدر_Error);
    Py_XINCREF(imp_PyExc_SystemExit);
    Py_XINCREF(imp_PyExc_RuntimeError);
    Py_XINCREF(imp_PyExc_ImportError);
    Py_XINCREF(imp_Py_Exc_حد_سے_تجاوز_Error);
    Py_XDECREF(exmod);
}
#اگر_ختم // DYNAMIC_PYTHON

ساکن صحیح initialised = 0;
#تعریف_کرو PYINITIALISED initialised
ساکن صحیح python_انتہا_called = FALSE;

#تعریف_کرو DESTRUCTOR_اختتام(self) self->ob_type->tp_free((PyObject*)self);

#تعریف_کرو WIN_PYTHON_REF(win) win->w_python_ref
#تعریف_کرو BUF_PYTHON_REF(buf) buf->b_python_ref
#تعریف_کرو TAB_PYTHON_REF(tab) tab->tp_python_ref

ساکن PyObject *OutputGetattr(PyObject *, حرف *);
ساکن PyObject *بفر_Getattr(PyObject *, حرف *);
ساکن PyObject *کھڑکی_Getattr(PyObject *, حرف *);
ساکن PyObject *Tab_صفحہ_Getattr(PyObject *, حرف *);
ساکن PyObject *RangeGetattr(PyObject *, حرف *);
ساکن PyObject *DictionaryGetattr(PyObject *, حرف*);
ساکن PyObject *ListGetattr(PyObject *, حرف *);
ساکن PyObject *TupleGetattr(PyObject *, حرف *);
ساکن PyObject *FunctionGetattr(PyObject *, حرف *);

#اگر_غیر_تعریف_شدہ Py_دورہ
# define Py_دورہ(obj) دورہ(obj, arg)
#اگر_ختم
#اگر_غیر_تعریف_شدہ Py_CLEAR
# define Py_CLEAR(obj) \
    { \
	Py_XDECREF(obj); \
	obj = تہی; \
    }
#اگر_ختم

#اگر defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    ساکن خالی *
py_memsave(خالی *p, size_t len)
{
    خالی	*r;

    اگر (!(r = PyMem_Malloc(len)))
	واپس تہی;
    mch_memmove(r, p, len);
    واپس r;
}

# define PY_STRSAVE(s) ((char_u *) py_memsave(s, STRLEN(s) + 1))
#اگر_ختم

قسمتعریف Py_قاش_Object Py_قاش_Object_T;

/*
 * Include the code shared with if_python3.c
 */
#شامل "if_py_both.h"


///////////////////////////////////////////////////////
// Internal function prototypes.

ساکن صحیح PythonMod_Init(خالی);


///////////////////////////////////////////////////////
// 1. Python interpreter main program.

#اگر_غیر_تعریف_شدہ PY_CAN_RECURSE
ساکن PyThreadState *saved_python_thread = تہی;

/*
 * Suspend a thread of the Python interpreter, other threads are allowed to
 * run.
 */
    ساکن خالی
Python_SaveThread(خالی)
{
    saved_python_thread = PyEval_SaveThread();
}

/*
 * Restore a thread of the Python interpreter, waits for other threads to
 * block.
 */
    ساکن خالی
Python_RestoreThread(خالی)
{
    PyEval_RestoreThread(saved_python_thread);
    saved_python_thread = تہی;
}
#اگر_ختم

    خالی
python_انتہا(خالی)
{
    ساکن صحیح recurse = 0;

    // If a crash occurs while doing this, don't try again.
    اگر (recurse != 0)
	واپس;

    python_انتہا_called = TRUE;
    ++recurse;

#اگر_تعریف_شدہ DYNAMIC_PYTHON
    اگر (hinstPython && Py_IsInitialized())
    {
# ifdef PY_CAN_RECURSE
	PyGILState_Ensure();
# ورنہ
	Python_RestoreThread();	    // enter python
# endif
	Py_Finalize();
    }
#ورنہ
    اگر (Py_IsInitialized())
    {
# ifdef PY_CAN_RECURSE
	PyGILState_Ensure();
# ورنہ
	Python_RestoreThread();	    // enter python
# endif
	Py_Finalize();
    }
#اگر_ختم

    --recurse;
}

#اگر defined(DYNAMIC_PYTHON) && defined(FEAT_PYTHON3)
    صحیح
python_loaded(خالی)
{
    واپس (hinstPython != 0);
}
#اگر_ختم

ساکن حرف *py_home_buf = تہی;

    ساکن صحیح
Python_Init(خالی)
{
    اگر (!initialised)
    {
#اگر defined(PY_نسخہ_HEX) && PY_نسخہ_HEX >= 0x02070000
	PyObject *site;
#اگر_ختم

#اگر_تعریف_شدہ DYNAMIC_PYTHON
	اگر (!python_enabled(TRUE))
	{
	    emsg(_(e_sorry_this_command_is_disabled_python_library_could_not_be_ملا));
	    جاؤ fail;
	}
#اگر_ختم

	اگر (*p_pyhome != NUL)
	{
	    // The string must not change later, make a copy in static memory.
	    py_home_buf = (حرف *)vim_strsave(p_pyhome);
	    اگر (py_home_buf != تہی)
		Py_سیٹ_Python_Home(py_home_buf);
	}
#اگر_تعریف_شدہ PYTHON_HOME
	ورنہ اگر (mch_getenv((char_u *)"PYTHONHOME") == تہی)
	    Py_سیٹ_Python_Home(PYTHON_HOME);
#اگر_ختم

	init_structs();

#اگر defined(PY_نسخہ_HEX) && PY_نسخہ_HEX >= 0x02070000
	// Disable implicit 'import site', because it may cause Vim to exit
	// when it can't be found.
	Py_NoSiteFlag++;
#اگر_ختم

	Py_Initialize();

#اگر defined(PY_نسخہ_HEX) && PY_نسخہ_HEX >= 0x02070000
	// 'import site' explicitly.
	site = PyImport_ImportModule("site");
	اگر (site == تہی)
	{
	    emsg(_(e_sorry_this_command_is_disabled_python_site_module_could_not_be_loaded));
	    جاؤ fail;
	}
	Py_DECREF(site);
#اگر_ختم

	// Initialise threads, and below save the state using
	// PyEval_SaveThread.  Without the call to PyEval_SaveThread, thread
	// specific state (such as the system trace hook), will be lost
	// between invocations of Python code.
	Py_Eval_Init_دھاگے();
#اگر_تعریف_شدہ DYNAMIC_PYTHON
	get_exceptions();
#اگر_ختم

	اگر (PythonIO_Init_io())
	    جاؤ fail;

	اگر (PythonMod_Init())
	    جاؤ fail;

	globals = PyModule_GetDict(Py_Import_جوڑو_Module("__main__"));

	// Remove the element from sys.path that was added because of our
	// argv[0] value in PythonMod_Init().  Previously we used an empty
	// string, but depending on the OS we then get an empty entry or
	// the current directory in sys.path.
	Py_تسلسل_Simple_String("import sys; sys.path = filter(lambda x: x != '/must>not&exist', sys.path)");

	// lock is created and acquired in PyEval_InitThreads() and thread
	// state is created in Py_Initialize()
	// there _PyGILState_NoteThreadState() also sets gilcounter to 1
	// (python must have threads enabled!)
	// so the following does both: unlock GIL and save thread state in TLS
	// without deleting thread state
#اگر_غیر_تعریف_شدہ PY_CAN_RECURSE
	saved_python_thread =
#اگر_ختم
	    PyEval_SaveThread();

	initialised = 1;
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
DoPyCommand(
    ثابت حرف		*cmd,
    dict_T*		locals,
    rangeinitializer	init_range,
    runner		تسلسل,
    خالی		*arg)
{
#اگر_غیر_تعریف_شدہ PY_CAN_RECURSE
    ساکن صحیح		recursive = 0;
#اگر_ختم
#اگر defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    حرف		*saved_locale;
#اگر_ختم
#اگر_تعریف_شدہ PY_CAN_RECURSE
    PyGILState_STATE	pygilstate;
#اگر_ختم

#اگر_غیر_تعریف_شدہ PY_CAN_RECURSE
    اگر (recursive)
    {
	emsg(_(e_cannot_invoke_python_recursively));
	واپس;
    }
    ++recursive;
#اگر_ختم
    اگر (python_انتہا_called)
	واپس;

    اگر (Python_Init())
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
	saved_locale = (حرف *) PY_STRSAVE(saved_locale);
	(خالی)setlocale(LC_NUMERIC, "C");
    }
#اگر_ختم

#اگر_تعریف_شدہ PY_CAN_RECURSE
    pygilstate = PyGILState_Ensure();
#ورنہ
    Python_RestoreThread();	    // enter python
#اگر_ختم

    تسلسل((حرف *) cmd, locals, arg
#اگر_تعریف_شدہ PY_CAN_RECURSE
	    , &pygilstate
#اگر_ختم
	    );

#اگر_تعریف_شدہ PY_CAN_RECURSE
    PyGILState_Release(pygilstate);
#ورنہ
    Python_SaveThread();	    // leave python
#اگر_ختم

#اگر defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    اگر (saved_locale != تہی)
    {
	(خالی)setlocale(LC_NUMERIC, saved_locale);
	PyMem_Free(saved_locale);
    }
#اگر_ختم

    Python_تالا_Vim();		    // enter vim
    PythonIO_Flush();

theend:
#اگر_غیر_تعریف_شدہ PY_CAN_RECURSE
    --recursive;
#اگر_ختم
    واپس;
}

/*
 * ":python"
 */
    خالی
ex_python(exarg_T *eap)
{
    char_u *script;

    script = script_get(eap, eap->arg);
    اگر (!eap->skip)
    {
	اگر (p_pyx == 0)
	    p_pyx = 2;

	DoPyCommand(script == تہی ? (حرف *) eap->arg : (حرف *) script,
		تہی,
		init_range_cmd,
		(runner) تسلسل_cmd,
		(خالی *) eap);
    }
    vim_free(script);
}

#تعریف_کرو بفر_سائز 1024

/*
 * ":pyfile"
 */
    خالی
ex_pyfile(exarg_T *eap)
{
    ساکن حرف بفر[بفر_سائز];
    ثابت حرف *file = (حرف *)eap->arg;
    حرف *p;

    اگر (p_pyx == 0)
	p_pyx = 2;

    // Have to do it like this. PyRun_SimpleFile requires you to pass a
    // stdio file pointer, but Vim and the Python DLL are compiled with
    // different options under Windows, meaning that stdio pointers aren't
    // compatible between the two. Yuk.
    //
    // Put the string "execfile('file')" into buffer. But, we need to
    // escape any backslashes or single quotes in the file name, so that
    // Python won't mangle the file name.
    strcpy(بفر, "execfile('");
    p = بفر + 10; // size of "execfile('"

    جب (*file && p < بفر + (بفر_سائز - 3))
    {
	اگر (*file == '\\' || *file == '\'')
	    *p++ = '\\';
	*p++ = *file++;
    }

    // If we didn't finish the file name, we hit a buffer overflow
    اگر (*file != '\0')
	واپس;

    // Put in the terminating "')" and a null
    *p++ = '\'';
    *p++ = ')';
    *p++ = '\0';

    // Execute the file
    DoPyCommand(بفر,
	    تہی,
	    init_range_cmd,
	    (runner) تسلسل_cmd,
	    (خالی *) eap);
}

    خالی
ex_pydo(exarg_T *eap)
{
    اگر (p_pyx == 0)
	p_pyx = 2;

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
OutputGetattr(PyObject *self, حرف *name)
{
    اگر (strcmp(name, "softspace") == 0)
	واپس PyInt_FromLong(((OutputObject *)(self))->softspace);
    ورنہ اگر (strcmp(name, "__members__") == 0)
	واپس ObjectDir(تہی, OutputAttrs);
    ورنہ اگر (strcmp(name, "errors") == 0)
	واپس PyString_FromString("strict");
    ورنہ اگر (strcmp(name, "encoding") == 0)
	واپس PyString_FromString(ENC_OPT);
    واپس Py_ڈھونڈو_طریقہ(OutputMethods, self, name);
}

///////////////////////////////////////////////////////
// 3. Implementation of the Vim module for Python

// Window type - Implementation functions
// --------------------------------------

#تعریف_کرو کھڑکی_Type_Check(obj) ((obj)->ob_type == &کھڑکی_Type)

// Buffer type - Implementation functions
// --------------------------------------

#تعریف_کرو بفر_Type_Check(obj) ((obj)->ob_type == &بفر_Type)

ساکن صحیح بفر_Ass_شے(PyObject *, PyInt, PyObject *);
ساکن صحیح بفر_Ass_قاش(PyObject *, PyInt, PyInt, PyObject *);

// Line range type - Implementation functions
// --------------------------------------

#تعریف_کرو RangeType_Check(obj) ((obj)->ob_type == &RangeType)

ساکن صحیح Range_Ass_شے(PyObject *, PyInt, PyObject *);
ساکن صحیح Range_Ass_قاش(PyObject *, PyInt, PyInt, PyObject *);

// Current objects type - Implementation functions
// -----------------------------------------------

ساکن PySequenceMethods بفر_As_Seq = {
    (PyInquiry)		بفر_لمبائی,	    // sq_length,    len(x)
    (binaryfunc)	0,		    // BufferConcat, sq_concat, x+y
    (PyIntArgFunc)	0,		    // BufferRepeat, sq_repeat, x*n
    (PyIntArgFunc)	بفر_شے,	    // sq_item,      x[i]
    (PyIntIntArgFunc)	بفر_قاش,	    // sq_slice,     x[i:j]
    (PyIntObjArgProc)	بفر_Ass_شے,	    // sq_ass_item,  x[i]=v
    (PyIntIntObjArgProc) بفر_Ass_قاش,    // sq_ass_slice, x[i:j]=v
    (objobjproc)	0,
    (binaryfunc)	0,
    0,
};

// Buffer object - Implementation

    ساکن PyObject *
بفر_Getattr(PyObject *self, حرف *name)
{
    PyObject *r;

    اگر ((r = بفر_Attr_Valid((بفر_Object *)(self), name)))
	واپس r;

    اگر (Check_بفر((بفر_Object *)(self)))
	واپس تہی;

    r = بفر_Attr((بفر_Object *)(self), name);
    اگر (r || PyErr_Occurred())
	واپس r;
    ورنہ
	واپس Py_ڈھونڈو_طریقہ(بفر_Methods, self, name);
}

//////////////////

    ساکن صحیح
بفر_Ass_شے(PyObject *self, PyInt n, PyObject *val)
{
    واپس RBAs_شے((بفر_Object *)(self), n, val, 1, -1, تہی);
}

    ساکن صحیح
بفر_Ass_قاش(PyObject *self, PyInt lo, PyInt hi, PyObject *val)
{
    واپس RBAs_قاش((بفر_Object *)(self), lo, hi, val, 1, -1, تہی);
}

ساکن PySequenceMethods RangeAsSeq = {
    (PyInquiry)		Range_لمبائی,	      // sq_length,    len(x)
    (binaryfunc)	0, /* RangeConcat, */ // sq_concat,    x+y
    (PyIntArgFunc)	0, /* RangeRepeat, */ // sq_repeat,    x*n
    (PyIntArgFunc)	Range_شے,	      // sq_item,      x[i]
    (PyIntIntArgFunc)	Range_قاش,	      // sq_slice,     x[i:j]
    (PyIntObjArgProc)	Range_Ass_شے,	      // sq_ass_item,  x[i]=v
    (PyIntIntObjArgProc) Range_Ass_قاش,	      // sq_ass_slice, x[i:j]=v
    (objobjproc)	0,
#اگر PY_MAJOR_نسخہ >= 2
    (binaryfunc)	0,
    0,
#اگر_ختم
};

// Line range object - Implementation

    ساکن PyObject *
RangeGetattr(PyObject *self, حرف *name)
{
    اگر (strcmp(name, "start") == 0)
	واپس Py_Build_قدر(Py_ssize_t_fmt, ((RangeObject *)(self))->آغاز - 1);
    ورنہ اگر (strcmp(name, "end") == 0)
	واپس Py_Build_قدر(Py_ssize_t_fmt, ((RangeObject *)(self))->انتہا - 1);
    ورنہ اگر (strcmp(name, "__members__") == 0)
	واپس ObjectDir(تہی, RangeAttrs);
    ورنہ
	واپس Py_ڈھونڈو_طریقہ(RangeMethods, self, name);
}

////////////////

    ساکن صحیح
Range_Ass_شے(PyObject *self, PyInt n, PyObject *val)
{
    واپس RBAs_شے(((RangeObject *)(self))->buf, n, val,
		     ((RangeObject *)(self))->آغاز,
		     ((RangeObject *)(self))->انتہا,
		     &((RangeObject *)(self))->انتہا);
}

    ساکن صحیح
Range_Ass_قاش(PyObject *self, PyInt lo, PyInt hi, PyObject *val)
{
    واپس RBAs_قاش(((RangeObject *)(self))->buf, lo, hi, val,
		      ((RangeObject *)(self))->آغاز,
		      ((RangeObject *)(self))->انتہا,
		      &((RangeObject *)(self))->انتہا);
}

// TabPage object - Implementation

    ساکن PyObject *
Tab_صفحہ_Getattr(PyObject *self, حرف *name)
{
    PyObject *r;

    اگر ((r = Tab_صفحہ_Attr_Valid((Tab_صفحہ_Object *)(self), name)))
	واپس r;

    اگر (Check_Tab_صفحہ((Tab_صفحہ_Object *)(self)))
	واپس تہی;

    r = Tab_صفحہ_Attr((Tab_صفحہ_Object *)(self), name);
    اگر (r || PyErr_Occurred())
	واپس r;
    ورنہ
	واپس Py_ڈھونڈو_طریقہ(Tab_صفحہ_Methods, self, name);
}

// Window object - Implementation

    ساکن PyObject *
کھڑکی_Getattr(PyObject *self, حرف *name)
{
    PyObject *r;

    اگر ((r = کھڑکی_Attr_Valid((کھڑکی_Object *)(self), name)))
	واپس r;

    اگر (Check_کھڑکی((کھڑکی_Object *)(self)))
	واپس تہی;

    r = کھڑکی_Attr((کھڑکی_Object *)(self), name);
    اگر (r || PyErr_Occurred())
	واپس r;
    ورنہ
	واپس Py_ڈھونڈو_طریقہ(کھڑکی_Methods, self, name);
}

// Tab page list object - Definitions

ساکن PySequenceMethods TabListAsSeq = {
    (PyInquiry)		Tab_List_لمبائی,	    // sq_length,    len(x)
    (binaryfunc)	0,		    // sq_concat,    x+y
    (PyIntArgFunc)	0,		    // sq_repeat,    x*n
    (PyIntArgFunc)	Tab_List_شے,	    // sq_item,      x[i]
    (PyIntIntArgFunc)	0,		    // sq_slice,     x[i:j]
    (PyIntObjArgProc)	0,		    // sq_ass_item,  x[i]=v
    (PyIntIntObjArgProc) 0,		    // sq_ass_slice, x[i:j]=v
    (objobjproc)	0,
#اگر PY_MAJOR_نسخہ >= 2
    (binaryfunc)	0,
    0,
#اگر_ختم
};

// Window list object - Definitions

ساکن PySequenceMethods WinListAsSeq = {
    (PyInquiry)		Win_List_لمبائی,	    // sq_length,    len(x)
    (binaryfunc)	0,		    // sq_concat,    x+y
    (PyIntArgFunc)	0,		    // sq_repeat,    x*n
    (PyIntArgFunc)	Win_List_شے,	    // sq_item,      x[i]
    (PyIntIntArgFunc)	0,		    // sq_slice,     x[i:j]
    (PyIntObjArgProc)	0,		    // sq_ass_item,  x[i]=v
    (PyIntIntObjArgProc) 0,		    // sq_ass_slice, x[i:j]=v
    (objobjproc)	0,
#اگر PY_MAJOR_نسخہ >= 2
    (binaryfunc)	0,
    0,
#اگر_ختم
};

// External interface

    خالی
python_بفر_free(buf_T *buf)
{
    بفر_Object *bp = BUF_PYTHON_REF(buf);
    اگر (bp == تہی)
	واپس;
    bp->buf = INVALID_بفر_قدر;
    BUF_PYTHON_REF(buf) = تہی;
}

    خالی
python_کھڑکی_free(win_T *win)
{
    کھڑکی_Object *wp = WIN_PYTHON_REF(win);
    اگر (wp == تہی)
	واپس;
    wp->win = INVALID_کھڑکی_قدر;
    WIN_PYTHON_REF(win) = تہی;
}

    خالی
python_tabpage_free(tabpage_T *tab)
{
    Tab_صفحہ_Object *tp = TAB_PYTHON_REF(tab);
    اگر (tp == تہی)
	واپس;
    tp->tab = INVALID_TABPAGE_قدر;
    TAB_PYTHON_REF(tab) = تہی;
}

    ساکن صحیح
PythonMod_Init(خالی)
{
    // The special value is removed from sys.path in Python_Init().
    ساکن حرف	*(argv[2]) = {"/must>not&exist/foo", تہی};

    اگر (init_types())
	واپس -1;

    // Set sys.argv[] to avoid a crash in warn().
    Py_Sys_سیٹ_Argv(1, argv);

    vim_module = Py_InitModule4("vim", VimMethods, (حرف *)تہی,
				(PyObject *)تہی, PYTHON_API_نسخہ);

    اگر (populate_module(vim_module))
	واپس -1;

    اگر (init_sys_راستہ())
	واپس -1;

    واپس 0;
}

//////////////////////////////////////////////////////////////////////////
// 4. Utility functions for handling the interface between Vim and Python.

// Convert a Vim line into a Python string.
// All internal newlines are replaced by null characters.
//
// On errors, the Python exception data is set, and NULL is returned.
    ساکن PyObject *
سطر_مقصد_String(ثابت حرف *str)
{
    PyObject *نتیجہ;
    PyInt len = لڑی_لمبائی(str);
    حرف *p;

    // Allocate a Python string object, with uninitialised contents. We
    // must do it this way, so that we can modify the string in place
    // later. See the Python source, Objects/stringobject.c for details.
    نتیجہ = Py_String_From_String_And_سائز(تہی, len);
    اگر (نتیجہ == تہی)
	واپس تہی;

    p = PyString_AsString(نتیجہ);

    جب (*str)
    {
	اگر (*str == '\n')
	    *p = '\0';
	ورنہ
	    *p = *str;

	++p;
	++str;
    }

    واپس نتیجہ;
}

    ساکن PyObject *
DictionaryGetattr(PyObject *self, حرف *name)
{
    DictionaryObject	*this = ((DictionaryObject *) (self));

    اگر (strcmp(name, "locked") == 0)
	واپس PyInt_FromLong(this->dict->dv_تالا);
    ورنہ اگر (strcmp(name, "scope") == 0)
	واپس PyInt_FromLong(this->dict->dv_scope);
    ورنہ اگر (strcmp(name, "__members__") == 0)
	واپس ObjectDir(تہی, DictionaryAttrs);

    واپس Py_ڈھونڈو_طریقہ(DictionaryMethods, self, name);
}

    ساکن PyObject *
ListGetattr(PyObject *self, حرف *name)
{
    اگر (strcmp(name, "locked") == 0)
	واپس PyInt_FromLong(((ListObject *)(self))->list->lv_تالا);
    ورنہ اگر (strcmp(name, "__members__") == 0)
	واپس ObjectDir(تہی, ListAttrs);

    واپس Py_ڈھونڈو_طریقہ(ListMethods, self, name);
}

    ساکن PyObject *
TupleGetattr(PyObject *self, حرف *name)
{
    اگر (strcmp(name, "locked") == 0)
	واپس PyInt_FromLong(((TupleObject *)(self))->tuple->tv_تالا);
    ورنہ اگر (strcmp(name, "__members__") == 0)
	واپس ObjectDir(تہی, TupleAttrs);

    واپس Py_ڈھونڈو_طریقہ(TupleMethods, self, name);
}

    ساکن PyObject *
FunctionGetattr(PyObject *self, حرف *name)
{
    PyObject	*r;

    r = FunctionAttr((FunctionObject *)(self), name);

    اگر (r || PyErr_Occurred())
	واپس r;
    ورنہ
	واپس Py_ڈھونڈو_طریقہ(FunctionMethods, self, name);
}

    خالی
do_pyeval(char_u *str, dict_T *locals, typval_T *rettv)
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
سیٹ_ref_اندر_python(صحیح copyID)
{
    واپس سیٹ_ref_اندر_py(copyID);
}
