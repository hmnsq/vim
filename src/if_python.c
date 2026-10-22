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

#ضمن "vim.h"

#ضمن <limits.h>

// uncomment this if used with the debug version of python.
// Checked on 2.7.4.
// #define Py_DEBUG
// Note: most of time you can add -DPy_DEBUG to CFLAGS in place of uncommenting
// uncomment this if used with the debug version of python, but without its
// allocator
// #define Py_DEBUG_NO_PYMALLOC

// Python.h defines _POSIX_THREADS itself (if needed)
#اگر_تعریف_شده _POSIX_THREADS
# undef _POSIX_THREADS
#پایان_اگر

#إذا defined(MSWIN) && defined(HAVE_FCNTL_H)
# undef HAVE_FCNTL_H
#پایان_اگر

#اگر_تعریف_شده _DEBUG
# undef _DEBUG
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
#اگر_تعریف_شده _POSIX_C_SOURCE
# undef _POSIX_C_SOURCE	// pyconfig.h defines it as well.
#پایان_اگر
#اگر_تعریف_شده XOPEN_منبع
# undef XOPEN_منبع	// pyconfig.h defines it as well.
#پایان_اگر

#تعریف_کن PY_SSIZE_T_CLEAN

#ضمن <Python.h>

#إذا !defined(PY_نسخه_HEX) || PY_نسخه_HEX < 0x02050000
# undef PY_SSIZE_T_CLEAN
#پایان_اگر

// these are NULL for Python 2
#تعریف_کن ERRORS_DECODE_ARG عدم
#تعریف_کن ERRORS_ENCODE_ARG ERRORS_DECODE_ARG

#لغو_تعریف اصلی // Defined in python.h - aargh
#لغو_تعریف HAVE_FCNTL_H // Clash with os_win32.h

// Perhaps leave this out for Python 2.6, which supports bytes?
#تعریف_کن Py_بایتها_From_String      PyString_FromString
#تعریف_کن Py_بایتها_Check		PyString_Check
#تعریف_کن Py_بایتها_As_String_And_اندازه Py_String_As_String_And_اندازه
#تعریف_کن Py_بایتها_From_String_And_اندازه   Py_String_From_String_And_اندازه

#إذا defined(PY_نسخه_HEX) && PY_نسخه_HEX >= 0x02070000
# define PY_USE_CAPSULE
#پایان_اگر

#إذا defined(PY_نسخه_HEX) && PY_نسخه_HEX >= 0x02050000
# define PyInt Py_ssize_t
# define PyInquiry lenfunc
# define PyIntArgFunc ssizeargfunc
# define PyIntIntArgFunc ssizessizeargfunc
# define PyIntObjArgProc ssizeobjargproc
# define PyIntIntObjArgProc ssizessizeobjargproc
# define Py_ssize_t_fmt "n"
#وإلا
# define PyInt صحيح
# define lenfunc inquiry
# define PyInquiry inquiry
# define PyIntArgFunc intargfunc
# define PyIntIntArgFunc intintargfunc
# define PyIntObjArgProc intobjargproc
# define PyIntIntObjArgProc intintobjargproc
# define Py_ssize_t_fmt "i"
#پایان_اگر
#تعریف_کن Py_بایتها_fmt "s"

// Parser flags
#تعریف_کن single_درون_داد	256
#تعریف_کن file_درون_داد	257
#تعریف_کن eval_درون_داد	258

#إذا defined(PY_نسخه_HEX) && PY_نسخه_HEX >= 0x020300F0
  // Python 2.3: can invoke ":python" recursively.
# define PY_CAN_RECURSE
#پایان_اگر

#إذا defined(DYNAMIC_PYTHON)
# ifndef DYNAMIC_PYTHON
#  define HINSTANCE long_u		// for generating prototypes
# endif

# ifdef MSWIN
#  define بارگیری_dll vim_بارگیری_Lib
#  define close_dll FreeLibrary
#  define نماد_from_dll Get_Proc_آدرس
#  define بارگیری_dll_error GetWin32Error
# وإلا
#  include <dlfcn.h>
#  define FARPROC فراغ*
#  define HINSTANCE فراغ*
#  إذا defined(PY_NO_RTLD_GLOBAL) && defined(PY3_NO_RTLD_GLOBAL)
#   define بارگیری_dll(n) dlopen((n), RTLD_LAZY)
#  وإلا
#   define بارگیری_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#  endif
#  define close_dll dlclose
#  define نماد_from_dll dlsym
#  define بارگیری_dll_error dlerror
# endif

// This makes if_python.c compile without warnings against Python 2.5
// on Win32 and Win64.
# undef Py_پیاپی_Simple_String
# undef Py_پیاپی_String
# undef PyArg_Parse
# undef PyArg_ParseTuple
# undef Py_Build_مقدار
# undef Py_InitModule4
# undef Py_InitModule4_64
# undef Py_Object_Call_روش
# undef PyObject_CallFunction

/*
 * Wrapper defines
 */
# define PyArg_Parse dll_PyArg_Parse
# define PyArg_ParseTuple dll_PyArg_ParseTuple
# define PyMem_Free dll_PyMem_Free
# define PyMem_Malloc dll_PyMem_Malloc
# define Py_Dict_مجموعه_قلم_String dll_Py_Dict_مجموعه_قلم_String
# define Py_Err_Bad_آرگومان dll_Py_Err_Bad_آرگومان
# define PyErr_NewException dll_PyErr_NewException
# define PyErr_Clear dll_PyErr_Clear
# define PyErr_Format dll_PyErr_Format
# define PyErr_PrintEx dll_PyErr_PrintEx
# define Py_Err_No_حافظه dll_Py_Err_No_حافظه
# define PyErr_Occurred dll_PyErr_Occurred
# define Py_Err_مجموعه_None dll_Py_Err_مجموعه_None
# define Py_Err_مجموعه_String dll_Py_Err_مجموعه_String
# define Py_Err_مجموعه_Object dll_Py_Err_مجموعه_Object
# define PyErr_ExceptionMatches dll_PyErr_ExceptionMatches
# define Py_Eval_Init_نخها dll_Py_Eval_Init_نخها
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
# define Py_List_Get_قلم dll_Py_List_Get_قلم
# define PyList_Append dll_PyList_Append
# define Py_List_درج dll_Py_List_درج
# define PyList_New dll_PyList_New
# define Py_List_مجموعه_قلم_2 dll_Py_List_مجموعه_قلم
# define Py_List_اندازه dll_Py_List_اندازه
# define PyList_Type (*dll_PyList_Type)
# define Py_Tuple_Get_قلم dll_Py_Tuple_Get_قلم
# define Py_Tuple_مجموعه_قلم dll_Py_Tuple_مجموعه_قلم
# define PyTuple_New dll_PyTuple_New
# define Py_Tuple_اندازه dll_Py_Tuple_اندازه
# define PyTuple_Type (*dll_PyTuple_Type)
# define PySequence_Check dll_PySequence_Check
# define Py_Sequence_اندازه dll_Py_Sequence_اندازه
# define Py_Sequence_Get_قلم dll_Py_Sequence_Get_قلم
# define PySequence_Fast dll_PySequence_Fast
# define Py_برش_Get_Indices_Ex dll_Py_برش_Get_Indices_Ex
# define PyImport_ImportModule dll_PyImport_ImportModule
# define PyDict_New dll_PyDict_New
# define Py_Dict_Get_قلم_String dll_Py_Dict_Get_قلم_String
# define Py_Dict_بعدی dll_Py_Dict_بعدی
# define PyDict_Type (*dll_PyDict_Type)
# ifdef Py_Mapping_کلیدها
#  define PY_NO_MAPPING_کلیدها
# وإلا
#  define Py_Mapping_کلیدها dll_Py_Mapping_کلیدها
# endif
# define Py_Object_Get_قلم dll_Py_Object_Get_قلم
# define Py_Object_Call_روش dll_Py_Object_Call_روش
# define PyMapping_Check dll_PyMapping_Check
# define Py_Iter_بعدی dll_Py_Iter_بعدی
# define PyModule_GetDict dll_PyModule_GetDict
# define Py_Module_جمع_کن_Object dll_Py_Module_جمع_کن_Object
# define Py_پیاپی_Simple_String dll_Py_پیاپی_Simple_String
# define Py_پیاپی_String dll_Py_پیاپی_String
# define PyObject_GetAttrString dll_PyObject_GetAttrString
# define PyObject_HasAttrString dll_PyObject_HasAttrString
# define Py_Object_مجموعه_Attr_String dll_Py_Object_مجموعه_Attr_String
# define Py_Object_Call_Function_Obj_آرگومانها dll_Py_Object_Call_Function_Obj_آرگومانها
# define PyObject_CallFunction dll_PyObject_CallFunction
# define PyObject_Call dll_PyObject_Call
# define PyObject_Repr dll_PyObject_Repr
# define PyString_AsString dll_PyString_AsString
# define Py_String_As_String_And_اندازه dll_Py_String_As_String_And_اندازه
# define PyString_FromString dll_PyString_FromString
# define PyString_FromFormat dll_PyString_FromFormat
# define Py_String_From_String_And_اندازه dll_Py_String_From_String_And_اندازه
# define Py_String_اندازه dll_Py_String_اندازه
# define PyString_Type (*dll_PyString_Type)
# define PyUnicode_Type (*dll_PyUnicode_Type)
# undef PyUnicode_AsEncodedString
# define PyUnicode_AsEncodedString py_PyUnicode_AsEncodedString
# define PyFloat_AsDouble dll_PyFloat_AsDouble
# define PyFloat_FromDouble dll_PyFloat_FromDouble
# define PyFloat_Type (*dll_PyFloat_Type)
# define Py_عدد_Check dll_Py_عدد_Check
# define Py_عدد_Long dll_Py_عدد_Long
# define Py_Import_جمع_کن_Module (*dll_Py_Import_جمع_کن_Module)
# define Py_Sys_مجموعه_Object dll_Py_Sys_مجموعه_Object
# define PySys_GetObject dll_PySys_GetObject
# define Py_Sys_مجموعه_Argv dll_Py_Sys_مجموعه_Argv
# define PyType_Type (*dll_PyType_Type)
# define PyFile_Type (*dll_PyFile_Type)
# define Py_برش_Type (*dll_Py_برش_Type)
# define PyType_Ready (*dll_PyType_Ready)
# define PyType_GenericAlloc dll_PyType_GenericAlloc
# define Py_Build_مقدار dll_Py_Build_مقدار
# define Py_پیدا_کن_روش dll_Py_پیدا_کن_روش
# define Py_InitModule4 dll_Py_InitModule4
# define Py_مجموعه_Python_Home dll_Py_مجموعه_Python_Home
# define Py_Initialize dll_Py_Initialize
# define Py_Finalize dll_Py_Finalize
# define Py_IsInitialized dll_Py_IsInitialized
# define _PyObject_New dll__PyObject_New
# define _PyObject_GC_New dll__PyObject_GC_New
# ifdef PyObject_GC_Del
#  define Py_underscore_GC
#  define _PyObject_GC_Del dll__PyObject_GC_Del
#  define _PyObject_GC_UnTrack dll__PyObject_GC_UnTrack
# وإلا
#  define PyObject_GC_Del dll_PyObject_GC_Del
#  define PyObject_GC_UnTrack dll_PyObject_GC_UnTrack
# endif
# إذا defined(PY_نسخه_HEX) && PY_نسخه_HEX >= 0x02070000
#  define Py_Object_بعدی_Not_Implemented (*dll_Py_Object_بعدی_Not_Implemented)
# endif
# define _Py_NoneStruct (*dll__Py_NoneStruct)
# define _Py_ZeroStruct (*dll__Py_ZeroStruct)
# define _Py_TrueStruct (*dll__Py_TrueStruct)
# define PyObject_Init dll__PyObject_Init
# define PyObject_GetIter dll_PyObject_GetIter
# define PyObject_IsTrue dll_PyObject_IsTrue
# إذا defined(PY_نسخه_HEX) && PY_نسخه_HEX >= 0x02020000
#  define PyType_IsSubtype dll_PyType_IsSubtype
#  ifdef Py_DEBUG
#   define _Py_NegativeRefcount dll__Py_NegativeRefcount
#   define Py_Ref_مجموع (*dll_Py_Ref_مجموع)
#   define _Py_Dealloc dll__Py_Dealloc
#  endif
# endif
# إذا defined(PY_نسخه_HEX) && PY_نسخه_HEX >= 0x02030000
#  إذا defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
#   define _PyObject_DebugMalloc dll__PyObject_DebugMalloc
#   define _PyObject_DebugFree dll__PyObject_DebugFree
#  وإلا
#   define PyObject_Malloc dll_PyObject_Malloc
#   define PyObject_Free dll_PyObject_Free
#  endif
# endif
# ifdef PY_USE_CAPSULE
#  define PyCapsule_New dll_PyCapsule_New
#  define Py_Capsule_Get_اشاره_گر dll_Py_Capsule_Get_اشاره_گر
# وإلا
#  define PyCObject_FromVoidPtr dll_PyCObject_FromVoidPtr
#  define PyCObject_AsVoidPtr dll_PyCObject_AsVoidPtr
# endif
# إذا defined(PY_نسخه_HEX) && PY_نسخه_HEX >= 0x02070000
#  define Py_NoSiteFlag (*dll_Py_NoSiteFlag)
# endif

/*
 * Pointers for dynamic link
 */
ساكن صحيح(*dll_PyArg_Parse)(PyObject *, حرف *, ...);
ساكن صحيح(*dll_PyArg_ParseTuple)(PyObject *, حرف *, ...);
ساكن فراغ(*dll_PyMem_Free)(فراغ *);
ساكن فراغ* (*dll_PyMem_Malloc)(size_t);
ساكن صحيح(*dll_Py_Dict_مجموعه_قلم_String)(PyObject *dp, حرف *کلید, PyObject *قلم);
ساكن صحيح(*dll_Py_Err_Bad_آرگومان)(فراغ);
ساكن PyObject *(*dll_PyErr_NewException)(حرف *, PyObject *, PyObject *);
ساكن فراغ(*dll_PyErr_Clear)(فراغ);
ساكن PyObject*(*dll_PyErr_Format)(PyObject *, ثابت حرف *, ...);
ساكن فراغ(*dll_PyErr_PrintEx)(صحيح);
ساكن PyObject*(*dll_Py_Err_No_حافظه)(فراغ);
ساكن PyObject*(*dll_PyErr_Occurred)(فراغ);
ساكن فراغ(*dll_Py_Err_مجموعه_None)(PyObject *);
ساكن فراغ(*dll_Py_Err_مجموعه_String)(PyObject *, ثابت حرف *);
ساكن فراغ(*dll_Py_Err_مجموعه_Object)(PyObject *, PyObject *);
ساكن صحيح(*dll_PyErr_ExceptionMatches)(PyObject *);
ساكن فراغ(*dll_Py_Eval_Init_نخها)(فراغ);
ساكن فراغ(*dll_PyEval_RestoreThread)(PyThreadState *);
ساكن PyThreadState*(*dll_PyEval_SaveThread)(فراغ);
# ifdef PY_CAN_RECURSE
ساكن PyGILState_STATE	(*dll_PyGILState_Ensure)(فراغ);
ساكن فراغ (*dll_PyGILState_Release)(PyGILState_STATE);
# endif
ساكن دراز(*dll_PyInt_AsLong)(PyObject *);
ساكن PyObject*(*dll_PyInt_FromLong)(دراز);
ساكن دراز(*dll_PyLong_AsLong)(PyObject *);
ساكن PyObject*(*dll_PyLong_FromLong)(دراز);
ساكن PyTypeObject* dll_PyBool_Type;
ساكن PyTypeObject* dll_PyInt_Type;
ساكن PyTypeObject* dll_PyLong_Type;
ساكن PyObject*(*dll_Py_List_Get_قلم)(PyObject *, PyInt);
ساكن صحيح(*dll_PyList_Append)(PyObject *, PyObject *);
ساكن صحيح(*dll_Py_List_درج)(PyObject *, PyInt, PyObject *);
ساكن PyObject*(*dll_PyList_New)(PyInt اندازه);
ساكن صحيح(*dll_Py_List_مجموعه_قلم)(PyObject *, PyInt, PyObject *);
ساكن PyInt(*dll_Py_List_اندازه)(PyObject *);
ساكن PyTypeObject* dll_PyList_Type;
ساكن PyObject*(*dll_Py_Tuple_Get_قلم)(PyObject *, PyInt);
ساكن صحيح(*dll_Py_Tuple_مجموعه_قلم)(PyObject *, PyInt, PyObject *);
ساكن PyObject*(*dll_PyTuple_New)(PyInt اندازه);
ساكن PyInt(*dll_Py_Tuple_اندازه)(PyObject *);
ساكن PyTypeObject* dll_PyTuple_Type;
ساكن صحيح (*dll_PySequence_Check)(PyObject *);
ساكن PyInt(*dll_Py_Sequence_اندازه)(PyObject *);
ساكن PyObject*(*dll_Py_Sequence_Get_قلم)(PyObject *, PyInt);
ساكن PyObject*(*dll_PySequence_Fast)(PyObject *, ثابت حرف *);
ساكن صحيح (*dll_Py_برش_Get_Indices_Ex)(Py_برش_Object *r, PyInt درازی,
		     PyInt *شروع, PyInt *stop, PyInt *گام,
		     PyInt *slicelen);
ساكن PyObject*(*dll_PyImport_ImportModule)(ثابت حرف *);
ساكن PyObject*(*dll_PyDict_New)(فراغ);
ساكن PyObject*(*dll_Py_Dict_Get_قلم_String)(PyObject *, ثابت حرف *);
ساكن صحيح (*dll_Py_Dict_بعدی)(PyObject *, PyInt *, PyObject **, PyObject **);
ساكن PyTypeObject* dll_PyDict_Type;
# ifndef PY_NO_MAPPING_کلیدها
ساكن PyObject* (*dll_Py_Mapping_کلیدها)(PyObject *);
# endif
ساكن PyObject* (*dll_Py_Object_Get_قلم)(PyObject *, PyObject *);
ساكن PyObject* (*dll_Py_Object_Call_روش)(PyObject *, حرف *, PyObject *);
ساكن صحيح (*dll_PyMapping_Check)(PyObject *);
ساكن PyObject* (*dll_Py_Iter_بعدی)(PyObject *);
ساكن PyObject*(*dll_PyModule_GetDict)(PyObject *);
ساكن صحيح(*dll_Py_Module_جمع_کن_Object)(PyObject *, ثابت حرف *, PyObject *);
ساكن صحيح(*dll_Py_پیاپی_Simple_String)(حرف *);
ساكن PyObject *(*dll_Py_پیاپی_String)(حرف *, صحيح, PyObject *, PyObject *);
ساكن PyObject* (*dll_PyObject_GetAttrString)(PyObject *, ثابت حرف *);
ساكن صحيح (*dll_PyObject_HasAttrString)(PyObject *, ثابت حرف *);
ساكن صحيح (*dll_Py_Object_مجموعه_Attr_String)(PyObject *, ثابت حرف *, PyObject *);
ساكن PyObject* (*dll_Py_Object_Call_Function_Obj_آرگومانها)(PyObject *, ...);
ساكن PyObject* (*dll_PyObject_CallFunction)(PyObject *, حرف *, ...);
ساكن PyObject* (*dll_PyObject_Call)(PyObject *, PyObject *, PyObject *);
ساكن PyObject* (*dll_PyObject_Repr)(PyObject *);
ساكن حرف*(*dll_PyString_AsString)(PyObject *);
ساكن صحيح(*dll_Py_String_As_String_And_اندازه)(PyObject *, حرف **, PyInt *);
ساكن PyObject*(*dll_PyString_FromString)(ثابت حرف *);
ساكن PyObject*(*dll_PyString_FromFormat)(ثابت حرف *, ...);
ساكن PyObject*(*dll_Py_String_From_String_And_اندازه)(ثابت حرف *, PyInt);
ساكن PyInt(*dll_Py_String_اندازه)(PyObject *);
ساكن PyTypeObject* dll_PyString_Type;
ساكن PyTypeObject* dll_PyUnicode_Type;
ساكن PyObject *(*py_PyUnicode_AsEncodedString)(PyObject *, حرف *, حرف *);
ساكن مزدوج(*dll_PyFloat_AsDouble)(PyObject *);
ساكن PyObject*(*dll_PyFloat_FromDouble)(مزدوج);
ساكن PyTypeObject* dll_PyFloat_Type;
ساكن صحيح(*dll_Py_عدد_Check)(PyObject *);
ساكن PyObject*(*dll_Py_عدد_Long)(PyObject *);
ساكن صحيح(*dll_Py_Sys_مجموعه_Object)(حرف *, PyObject *);
ساكن PyObject *(*dll_PySys_GetObject)(حرف *);
ساكن صحيح(*dll_Py_Sys_مجموعه_Argv)(صحيح, حرف **);
ساكن PyTypeObject* dll_PyType_Type;
ساكن PyTypeObject* dll_PyFile_Type;
ساكن PyTypeObject* dll_Py_برش_Type;
ساكن صحيح (*dll_PyType_Ready)(PyTypeObject *type);
ساكن PyObject* (*dll_PyType_GenericAlloc)(PyTypeObject *type, PyInt nitems);
ساكن PyObject*(*dll_Py_Build_مقدار)(حرف *, ...);
ساكن PyObject*(*dll_Py_پیدا_کن_روش)(بنية Py_روش_Def[], PyObject *, حرف *);
ساكن PyObject*(*dll_Py_InitModule4)(حرف *, بنية Py_روش_Def *, حرف *, PyObject *, صحيح);
ساكن PyObject*(*dll_Py_Import_جمع_کن_Module)(حرف *);
ساكن فراغ(*dll_Py_مجموعه_Python_Home)(حرف *home);
ساكن فراغ(*dll_Py_Initialize)(فراغ);
ساكن فراغ(*dll_Py_Finalize)(فراغ);
ساكن صحيح(*dll_Py_IsInitialized)(فراغ);
ساكن PyObject*(*dll__PyObject_New)(PyTypeObject *, PyObject *);
ساكن PyObject*(*dll__PyObject_GC_New)(PyTypeObject *);
# ifdef Py_underscore_GC
ساكن فراغ(*dll__PyObject_GC_Del)(فراغ *);
ساكن فراغ(*dll__PyObject_GC_UnTrack)(فراغ *);
# وإلا
ساكن فراغ(*dll_PyObject_GC_Del)(فراغ *);
ساكن فراغ(*dll_PyObject_GC_UnTrack)(فراغ *);
# endif
ساكن PyObject*(*dll__PyObject_Init)(PyObject *, PyTypeObject *);
ساكن PyObject* (*dll_PyObject_GetIter)(PyObject *);
ساكن صحيح (*dll_PyObject_IsTrue)(PyObject *);
# إذا defined(PY_نسخه_HEX) && PY_نسخه_HEX >= 0x02070000
ساكن iternextfunc dll_Py_Object_بعدی_Not_Implemented;
# endif
ساكن PyObject* dll__Py_NoneStruct;
ساكن PyObject* _Py_ZeroStruct;
ساكن PyObject* dll__Py_TrueStruct;
# إذا defined(PY_نسخه_HEX) && PY_نسخه_HEX >= 0x02020000
ساكن صحيح (*dll_PyType_IsSubtype)(PyTypeObject *, PyTypeObject *);
#  ifdef Py_DEBUG
ساكن فراغ (*dll__Py_NegativeRefcount)(ثابت حرف *fname, صحيح lineno, PyObject *op);
ساكن PyInt* dll_Py_Ref_مجموع;
ساكن فراغ (*dll__Py_Dealloc)(PyObject *obj);
#  endif
# endif
# إذا defined(PY_نسخه_HEX) && PY_نسخه_HEX >= 0x02030000
#  إذا defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
ساكن فراغ (*dll__PyObject_DebugFree)(فراغ*);
ساكن فراغ* (*dll__PyObject_DebugMalloc)(size_t);
#  وإلا
ساكن فراغ* (*dll_PyObject_Malloc)(size_t);
ساكن فراغ (*dll_PyObject_Free)(فراغ*);
#  endif
# endif
# ifdef PY_USE_CAPSULE
ساكن PyObject* (*dll_PyCapsule_New)(فراغ *, حرف *, PyCapsule_Destructor);
ساكن فراغ* (*dll_Py_Capsule_Get_اشاره_گر)(PyObject *, حرف *);
# وإلا
ساكن PyObject* (*dll_PyCObject_FromVoidPtr)(فراغ *cobj, فراغ (*destr)(فراغ *));
ساكن فراغ* (*dll_PyCObject_AsVoidPtr)(PyObject *);
# endif
# إذا defined(PY_نسخه_HEX) && PY_نسخه_HEX >= 0x02070000
ساكن صحيح* dll_Py_NoSiteFlag;
# endif

ساكن HINSTANCE hinstPython = 0; // Instance of python.dll

// Imported exception objects
ساكن PyObject *imp_PyExc_AttributeError;
ساكن PyObject *imp_Py_Exc_شاخص_Error;
ساكن PyObject *imp_Py_Exc_کلید_Error;
ساكن PyObject *imp_Py_Exc_Keyboard_وقفه;
ساكن PyObject *imp_PyExc_TypeError;
ساكن PyObject *imp_Py_Exc_مقدار_Error;
ساكن PyObject *imp_PyExc_SystemExit;
ساكن PyObject *imp_PyExc_RuntimeError;
ساكن PyObject *imp_PyExc_ImportError;
ساكن PyObject *imp_Py_Exc_سرریز_Error;

# define PyExc_AttributeError imp_PyExc_AttributeError
# define Py_Exc_شاخص_Error imp_Py_Exc_شاخص_Error
# define Py_Exc_کلید_Error imp_Py_Exc_کلید_Error
# define Py_Exc_Keyboard_وقفه imp_Py_Exc_Keyboard_وقفه
# define PyExc_TypeError imp_PyExc_TypeError
# define Py_Exc_مقدار_Error imp_Py_Exc_مقدار_Error
# define PyExc_SystemExit imp_PyExc_SystemExit
# define PyExc_RuntimeError imp_PyExc_RuntimeError
# define PyExc_ImportError imp_PyExc_ImportError
# define Py_Exc_سرریز_Error imp_Py_Exc_سرریز_Error

/*
 * Table of name to function pointer of python.
 */
# define PYTHON_PROC FARPROC
ساكن بنية
{
    حرف *name;
    PYTHON_PROC *ptr;
} python_funcname_جدول[] =
{
# ifdef PY_SSIZE_T_CLEAN
    {"_PyArg_Parse_SizeT", (PYTHON_PROC*)&dll_PyArg_Parse},
    {"_PyArg_ParseTuple_SizeT", (PYTHON_PROC*)&dll_PyArg_ParseTuple},
    {"_Py_BuildValue_SizeT", (PYTHON_PROC*)&dll_Py_Build_مقدار},
# وإلا
    {"PyArg_Parse", (PYTHON_PROC*)&dll_PyArg_Parse},
    {"PyArg_ParseTuple", (PYTHON_PROC*)&dll_PyArg_ParseTuple},
    {"Py_BuildValue", (PYTHON_PROC*)&dll_Py_Build_مقدار},
# endif
    {"PyMem_Free", (PYTHON_PROC*)&dll_PyMem_Free},
    {"PyMem_Malloc", (PYTHON_PROC*)&dll_PyMem_Malloc},
    {"PyDict_SetItemString", (PYTHON_PROC*)&dll_Py_Dict_مجموعه_قلم_String},
    {"PyErr_BadArgument", (PYTHON_PROC*)&dll_Py_Err_Bad_آرگومان},
    {"PyErr_NewException", (PYTHON_PROC*)&dll_PyErr_NewException},
    {"PyErr_Clear", (PYTHON_PROC*)&dll_PyErr_Clear},
    {"PyErr_Format", (PYTHON_PROC*)&dll_PyErr_Format},
    {"PyErr_PrintEx", (PYTHON_PROC*)&dll_PyErr_PrintEx},
    {"PyErr_NoMemory", (PYTHON_PROC*)&dll_Py_Err_No_حافظه},
    {"PyErr_Occurred", (PYTHON_PROC*)&dll_PyErr_Occurred},
    {"PyErr_SetNone", (PYTHON_PROC*)&dll_Py_Err_مجموعه_None},
    {"PyErr_SetString", (PYTHON_PROC*)&dll_Py_Err_مجموعه_String},
    {"PyErr_SetObject", (PYTHON_PROC*)&dll_Py_Err_مجموعه_Object},
    {"PyErr_ExceptionMatches", (PYTHON_PROC*)&dll_PyErr_ExceptionMatches},
    {"PyEval_InitThreads", (PYTHON_PROC*)&dll_Py_Eval_Init_نخها},
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
    {"PyList_GetItem", (PYTHON_PROC*)&dll_Py_List_Get_قلم},
    {"PyList_Append", (PYTHON_PROC*)&dll_PyList_Append},
    {"PyList_Insert", (PYTHON_PROC*)&dll_Py_List_درج},
    {"PyList_New", (PYTHON_PROC*)&dll_PyList_New},
    {"PyList_SetItem", (PYTHON_PROC*)&dll_Py_List_مجموعه_قلم},
    {"PyList_Size", (PYTHON_PROC*)&dll_Py_List_اندازه},
    {"PyList_Type", (PYTHON_PROC*)&dll_PyList_Type},
    {"PyTuple_GetItem", (PYTHON_PROC*)&dll_Py_Tuple_Get_قلم},
    {"PyTuple_SetItem", (PYTHON_PROC*)&dll_Py_Tuple_مجموعه_قلم},
    {"PyTuple_New", (PYTHON_PROC*)&dll_PyTuple_New},
    {"PyTuple_Size", (PYTHON_PROC*)&dll_Py_Tuple_اندازه},
    {"PyTuple_Type", (PYTHON_PROC*)&dll_PyTuple_Type},
    {"PySequence_Size", (PYTHON_PROC*)&dll_Py_Sequence_اندازه},
    {"PySequence_Check", (PYTHON_PROC*)&dll_PySequence_Check},
    {"PySequence_GetItem", (PYTHON_PROC*)&dll_Py_Sequence_Get_قلم},
    {"PySequence_Fast", (PYTHON_PROC*)&dll_PySequence_Fast},
    {"PySlice_GetIndicesEx", (PYTHON_PROC*)&dll_Py_برش_Get_Indices_Ex},
    {"PyImport_ImportModule", (PYTHON_PROC*)&dll_PyImport_ImportModule},
    {"PyDict_GetItemString", (PYTHON_PROC*)&dll_Py_Dict_Get_قلم_String},
    {"PyDict_Next", (PYTHON_PROC*)&dll_Py_Dict_بعدی},
    {"PyDict_New", (PYTHON_PROC*)&dll_PyDict_New},
    {"PyDict_Type", (PYTHON_PROC*)&dll_PyDict_Type},
# ifndef PY_NO_MAPPING_کلیدها
    {"PyMapping_Keys", (PYTHON_PROC*)&dll_Py_Mapping_کلیدها},
# endif
    {"PyObject_GetItem", (PYTHON_PROC*)&dll_Py_Object_Get_قلم},
    {"PyObject_CallMethod", (PYTHON_PROC*)&dll_Py_Object_Call_روش},
    {"PyMapping_Check", (PYTHON_PROC*)&dll_PyMapping_Check},
    {"PyIter_Next", (PYTHON_PROC*)&dll_Py_Iter_بعدی},
    {"PyModule_GetDict", (PYTHON_PROC*)&dll_PyModule_GetDict},
    {"PyModule_AddObject", (PYTHON_PROC*)&dll_Py_Module_جمع_کن_Object},
    {"PyRun_SimpleString", (PYTHON_PROC*)&dll_Py_پیاپی_Simple_String},
    {"PyRun_String", (PYTHON_PROC*)&dll_Py_پیاپی_String},
    {"PyObject_GetAttrString", (PYTHON_PROC*)&dll_PyObject_GetAttrString},
    {"PyObject_HasAttrString", (PYTHON_PROC*)&dll_PyObject_HasAttrString},
    {"PyObject_SetAttrString", (PYTHON_PROC*)&dll_Py_Object_مجموعه_Attr_String},
    {"PyObject_CallFunctionObjArgs", (PYTHON_PROC*)&dll_Py_Object_Call_Function_Obj_آرگومانها},
    {"PyObject_CallFunction", (PYTHON_PROC*)&dll_PyObject_CallFunction},
    {"PyObject_Call", (PYTHON_PROC*)&dll_PyObject_Call},
    {"PyObject_Repr", (PYTHON_PROC*)&dll_PyObject_Repr},
    {"PyString_AsString", (PYTHON_PROC*)&dll_PyString_AsString},
    {"PyString_AsStringAndSize", (PYTHON_PROC*)&dll_Py_String_As_String_And_اندازه},
    {"PyString_FromString", (PYTHON_PROC*)&dll_PyString_FromString},
    {"PyString_FromFormat", (PYTHON_PROC*)&dll_PyString_FromFormat},
    {"PyString_FromStringAndSize", (PYTHON_PROC*)&dll_Py_String_From_String_And_اندازه},
    {"PyString_Size", (PYTHON_PROC*)&dll_Py_String_اندازه},
    {"PyString_Type", (PYTHON_PROC*)&dll_PyString_Type},
    {"PyUnicode_Type", (PYTHON_PROC*)&dll_PyUnicode_Type},
    {"PyFloat_Type", (PYTHON_PROC*)&dll_PyFloat_Type},
    {"PyFloat_AsDouble", (PYTHON_PROC*)&dll_PyFloat_AsDouble},
    {"PyFloat_FromDouble", (PYTHON_PROC*)&dll_PyFloat_FromDouble},
    {"PyImport_AddModule", (PYTHON_PROC*)&dll_Py_Import_جمع_کن_Module},
    {"PyNumber_Check", (PYTHON_PROC*)&dll_Py_عدد_Check},
    {"PyNumber_Long", (PYTHON_PROC*)&dll_Py_عدد_Long},
    {"PySys_SetObject", (PYTHON_PROC*)&dll_Py_Sys_مجموعه_Object},
    {"PySys_GetObject", (PYTHON_PROC*)&dll_PySys_GetObject},
    {"PySys_SetArgv", (PYTHON_PROC*)&dll_Py_Sys_مجموعه_Argv},
    {"PyType_Type", (PYTHON_PROC*)&dll_PyType_Type},
    {"PyFile_Type", (PYTHON_PROC*)&dll_PyFile_Type},
    {"PySlice_Type", (PYTHON_PROC*)&dll_Py_برش_Type},
    {"PyType_Ready", (PYTHON_PROC*)&dll_PyType_Ready},
    {"PyType_GenericAlloc", (PYTHON_PROC*)&dll_PyType_GenericAlloc},
    {"Py_FindMethod", (PYTHON_PROC*)&dll_Py_پیدا_کن_روش},
    {"Py_SetPythonHome", (PYTHON_PROC*)&dll_Py_مجموعه_Python_Home},
    {"Py_Initialize", (PYTHON_PROC*)&dll_Py_Initialize},
    {"Py_Finalize", (PYTHON_PROC*)&dll_Py_Finalize},
    {"Py_IsInitialized", (PYTHON_PROC*)&dll_Py_IsInitialized},
    {"_PyObject_New", (PYTHON_PROC*)&dll__PyObject_New},
    {"_PyObject_GC_New", (PYTHON_PROC*)&dll__PyObject_GC_New},
# ifdef Py_underscore_GC
    {"_PyObject_GC_Del", (PYTHON_PROC*)&dll__PyObject_GC_Del},
    {"_PyObject_GC_UnTrack", (PYTHON_PROC*)&dll__PyObject_GC_UnTrack},
# وإلا
    {"PyObject_GC_Del", (PYTHON_PROC*)&dll_PyObject_GC_Del},
    {"PyObject_GC_UnTrack", (PYTHON_PROC*)&dll_PyObject_GC_UnTrack},
# endif
    {"PyObject_Init", (PYTHON_PROC*)&dll__PyObject_Init},
    {"PyObject_GetIter", (PYTHON_PROC*)&dll_PyObject_GetIter},
    {"PyObject_IsTrue", (PYTHON_PROC*)&dll_PyObject_IsTrue},
# إذا defined(PY_نسخه_HEX) && PY_نسخه_HEX >= 0x02070000
    {"_PyObject_NextNotImplemented", (PYTHON_PROC*)&dll_Py_Object_بعدی_Not_Implemented},
# endif
    {"_Py_NoneStruct", (PYTHON_PROC*)&dll__Py_NoneStruct},
    {"_Py_ZeroStruct", (PYTHON_PROC*)&dll__Py_ZeroStruct},
    {"_Py_TrueStruct", (PYTHON_PROC*)&dll__Py_TrueStruct},
# إذا defined(PY_نسخه_HEX) && PY_نسخه_HEX >= 0x02020000
#  ifdef Py_DEBUG
    {"_Py_NegativeRefcount", (PYTHON_PROC*)&dll__Py_NegativeRefcount},
    {"_Py_RefTotal", (PYTHON_PROC*)&dll_Py_Ref_مجموع},
    {"_Py_Dealloc", (PYTHON_PROC*)&dll__Py_Dealloc},
#  endif
    {"PyType_IsSubtype", (PYTHON_PROC*)&dll_PyType_IsSubtype},
# endif
# إذا defined(PY_نسخه_HEX) && PY_نسخه_HEX >= 0x02030000
#  إذا defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
    {"_PyObject_DebugFree", (PYTHON_PROC*)&dll__PyObject_DebugFree},
    {"_PyObject_DebugMalloc", (PYTHON_PROC*)&dll__PyObject_DebugMalloc},
#  وإلا
    {"PyObject_Malloc", (PYTHON_PROC*)&dll_PyObject_Malloc},
    {"PyObject_Free", (PYTHON_PROC*)&dll_PyObject_Free},
#  endif
# endif
# إذا defined(PY_نسخه_HEX) && PY_نسخه_HEX >= 0x02050000 \
	&& SIZEOF_اندازه_T != VIM_SIZEOF_INT
#  ifdef Py_DEBUG
    {"Py_InitModule4TraceRefs_64", (PYTHON_PROC*)&dll_Py_InitModule4},
#  وإلا
    {"Py_InitModule4_64", (PYTHON_PROC*)&dll_Py_InitModule4},
#  endif
# وإلا
#  ifdef Py_DEBUG
    {"Py_InitModule4TraceRefs", (PYTHON_PROC*)&dll_Py_InitModule4},
#  وإلا
    {"Py_InitModule4", (PYTHON_PROC*)&dll_Py_InitModule4},
#  endif
# endif
# ifdef PY_USE_CAPSULE
    {"PyCapsule_New", (PYTHON_PROC*)&dll_PyCapsule_New},
    {"PyCapsule_GetPointer", (PYTHON_PROC*)&dll_Py_Capsule_Get_اشاره_گر},
# وإلا
    {"PyCObject_FromVoidPtr", (PYTHON_PROC*)&dll_PyCObject_FromVoidPtr},
    {"PyCObject_AsVoidPtr", (PYTHON_PROC*)&dll_PyCObject_AsVoidPtr},
# endif
# إذا defined(PY_نسخه_HEX) && PY_نسخه_HEX >= 0x02070000
    {"Py_NoSiteFlag", (PYTHON_PROC*)&dll_Py_NoSiteFlag},
# endif
    {"", عدم},
};

/*
 * Load library and get all pointers.
 * Parameter 'libname' provides name of DLL.
 * Return OK or FAIL.
 */
    ساكن صحيح
python_runtime_link_init(حرف *libname, صحيح verbose)
{
    صحيح i;
    PYTHON_PROC *ucs_as_encoded_string =
				   (PYTHON_PROC*)&py_PyUnicode_AsEncodedString;

# إذا !(defined(PY_NO_RTLD_GLOBAL) && defined(PY3_NO_RTLD_GLOBAL)) && defined(UNIX) && defined(FEAT_PYTHON3)
    // Can't have Python and Python3 loaded at the same time.
    // It causes a crash, because RTLD_GLOBAL is needed for
    // standard C extension libraries of one or both python versions.
    إذا (python3_loaded())
    {
	إذا (verbose)
	    emsg(_(e_this_vim_cannot_execute_python_after_using_py3));
	أرجع FAIL;
    }
# endif

    إذا (hinstPython)
	أرجع OK;
    hinstPython = بارگیری_dll(libname);
    إذا (!hinstPython)
    {
	إذا (verbose)
	    semsg(_(e_could_not_بارگیری_library_str_str), libname, بارگیری_dll_error());
	أرجع FAIL;
    }

    لكل (i = 0; python_funcname_جدول[i].ptr; ++i)
    {
	إذا ((*python_funcname_جدول[i].ptr = نماد_from_dll(hinstPython,
			python_funcname_جدول[i].name)) == عدم)
	{
	    close_dll(hinstPython);
	    hinstPython = 0;
	    إذا (verbose)
		semsg(_(e_could_not_بارگیری_library_function_str), python_funcname_جدول[i].name);
	    أرجع FAIL;
	}
    }

    // Load unicode functions separately as only the ucs2 or the ucs4 functions
    // will be present in the library.
    *ucs_as_encoded_string = نماد_from_dll(hinstPython,
					     "PyUnicodeUCS2_AsEncodedString");
    إذا (*ucs_as_encoded_string == عدم)
	*ucs_as_encoded_string = نماد_from_dll(hinstPython,
					     "PyUnicodeUCS4_AsEncodedString");
    إذا (*ucs_as_encoded_string == عدم)
    {
	close_dll(hinstPython);
	hinstPython = 0;
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
python_enabled(صحيح verbose)
{
    أرجع python_runtime_link_init((حرف *)p_pydll, verbose) == OK;
}

/*
 * Load the standard Python exceptions - don't import the symbols from the
 * DLL, as this can cause errors (importing data symbols is not reliable).
 */
    ساكن فراغ
get_exceptions(فراغ)
{
    PyObject *exmod = PyImport_ImportModule("exceptions");
    PyObject *exdict = PyModule_GetDict(exmod);
    imp_PyExc_AttributeError = Py_Dict_Get_قلم_String(exdict, "AttributeError");
    imp_Py_Exc_شاخص_Error = Py_Dict_Get_قلم_String(exdict, "IndexError");
    imp_Py_Exc_کلید_Error = Py_Dict_Get_قلم_String(exdict, "KeyError");
    imp_Py_Exc_Keyboard_وقفه = Py_Dict_Get_قلم_String(exdict, "KeyboardInterrupt");
    imp_PyExc_TypeError = Py_Dict_Get_قلم_String(exdict, "TypeError");
    imp_Py_Exc_مقدار_Error = Py_Dict_Get_قلم_String(exdict, "ValueError");
    imp_PyExc_SystemExit = Py_Dict_Get_قلم_String(exdict, "SystemExit");
    imp_PyExc_RuntimeError = Py_Dict_Get_قلم_String(exdict, "RuntimeError");
    imp_PyExc_ImportError = Py_Dict_Get_قلم_String(exdict, "ImportError");
    imp_Py_Exc_سرریز_Error = Py_Dict_Get_قلم_String(exdict, "OverflowError");
    Py_XINCREF(imp_PyExc_AttributeError);
    Py_XINCREF(imp_Py_Exc_شاخص_Error);
    Py_XINCREF(imp_Py_Exc_کلید_Error);
    Py_XINCREF(imp_Py_Exc_Keyboard_وقفه);
    Py_XINCREF(imp_PyExc_TypeError);
    Py_XINCREF(imp_Py_Exc_مقدار_Error);
    Py_XINCREF(imp_PyExc_SystemExit);
    Py_XINCREF(imp_PyExc_RuntimeError);
    Py_XINCREF(imp_PyExc_ImportError);
    Py_XINCREF(imp_Py_Exc_سرریز_Error);
    Py_XDECREF(exmod);
}
#پایان_اگر // DYNAMIC_PYTHON

ساكن صحيح initialised = 0;
#تعریف_کن PYINITIALISED initialised
ساكن صحيح python_انتها_called = FALSE;

#تعریف_کن DESTRUCTOR_پایان(self) self->ob_type->tp_free((PyObject*)self);

#تعریف_کن WIN_PYTHON_REF(win) win->w_python_ref
#تعریف_کن BUF_PYTHON_REF(buf) buf->b_python_ref
#تعریف_کن TAB_PYTHON_REF(tab) tab->tp_python_ref

ساكن PyObject *OutputGetattr(PyObject *, حرف *);
ساكن PyObject *میانگیر_Getattr(PyObject *, حرف *);
ساكن PyObject *پنجره_Getattr(PyObject *, حرف *);
ساكن PyObject *Tab_صفحه_Getattr(PyObject *, حرف *);
ساكن PyObject *RangeGetattr(PyObject *, حرف *);
ساكن PyObject *DictionaryGetattr(PyObject *, حرف*);
ساكن PyObject *ListGetattr(PyObject *, حرف *);
ساكن PyObject *TupleGetattr(PyObject *, حرف *);
ساكن PyObject *FunctionGetattr(PyObject *, حرف *);

#اگر_تعریف_نشده Py_بازدید
# define Py_بازدید(obj) بازدید(obj, arg)
#پایان_اگر
#اگر_تعریف_نشده Py_CLEAR
# define Py_CLEAR(obj) \
    { \
	Py_XDECREF(obj); \
	obj = عدم; \
    }
#پایان_اگر

#إذا defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    ساكن فراغ *
py_memsave(فراغ *p, size_t len)
{
    فراغ	*r;

    إذا (!(r = PyMem_Malloc(len)))
	أرجع عدم;
    mch_memmove(r, p, len);
    أرجع r;
}

# define PY_STRSAVE(s) ((char_u *) py_memsave(s, STRLEN(s) + 1))
#پایان_اگر

تعريفنوع Py_برش_Object Py_برش_Object_T;

/*
 * Include the code shared with if_python3.c
 */
#ضمن "if_py_both.h"


///////////////////////////////////////////////////////
// Internal function prototypes.

ساكن صحيح PythonMod_Init(فراغ);


///////////////////////////////////////////////////////
// 1. Python interpreter main program.

#اگر_تعریف_نشده PY_CAN_RECURSE
ساكن PyThreadState *saved_python_thread = عدم;

/*
 * Suspend a thread of the Python interpreter, other threads are allowed to
 * run.
 */
    ساكن فراغ
Python_SaveThread(فراغ)
{
    saved_python_thread = PyEval_SaveThread();
}

/*
 * Restore a thread of the Python interpreter, waits for other threads to
 * block.
 */
    ساكن فراغ
Python_RestoreThread(فراغ)
{
    PyEval_RestoreThread(saved_python_thread);
    saved_python_thread = عدم;
}
#پایان_اگر

    فراغ
python_انتها(فراغ)
{
    ساكن صحيح recurse = 0;

    // If a crash occurs while doing this, don't try again.
    إذا (recurse != 0)
	أرجع;

    python_انتها_called = TRUE;
    ++recurse;

#اگر_تعریف_شده DYNAMIC_PYTHON
    إذا (hinstPython && Py_IsInitialized())
    {
# ifdef PY_CAN_RECURSE
	PyGILState_Ensure();
# وإلا
	Python_RestoreThread();	    // enter python
# endif
	Py_Finalize();
    }
#وإلا
    إذا (Py_IsInitialized())
    {
# ifdef PY_CAN_RECURSE
	PyGILState_Ensure();
# وإلا
	Python_RestoreThread();	    // enter python
# endif
	Py_Finalize();
    }
#پایان_اگر

    --recurse;
}

#إذا defined(DYNAMIC_PYTHON) && defined(FEAT_PYTHON3)
    صحيح
python_loaded(فراغ)
{
    أرجع (hinstPython != 0);
}
#پایان_اگر

ساكن حرف *py_home_buf = عدم;

    ساكن صحيح
Python_Init(فراغ)
{
    إذا (!initialised)
    {
#إذا defined(PY_نسخه_HEX) && PY_نسخه_HEX >= 0x02070000
	PyObject *site;
#پایان_اگر

#اگر_تعریف_شده DYNAMIC_PYTHON
	إذا (!python_enabled(TRUE))
	{
	    emsg(_(e_sorry_this_command_is_disabled_python_library_could_not_be_یافت));
	    اذهب fail;
	}
#پایان_اگر

	إذا (*p_pyhome != NUL)
	{
	    // The string must not change later, make a copy in static memory.
	    py_home_buf = (حرف *)vim_strsave(p_pyhome);
	    إذا (py_home_buf != عدم)
		Py_مجموعه_Python_Home(py_home_buf);
	}
#اگر_تعریف_شده PYTHON_HOME
	وإلا إذا (mch_getenv((char_u *)"PYTHONHOME") == عدم)
	    Py_مجموعه_Python_Home(PYTHON_HOME);
#پایان_اگر

	init_structs();

#إذا defined(PY_نسخه_HEX) && PY_نسخه_HEX >= 0x02070000
	// Disable implicit 'import site', because it may cause Vim to exit
	// when it can't be found.
	Py_NoSiteFlag++;
#پایان_اگر

	Py_Initialize();

#إذا defined(PY_نسخه_HEX) && PY_نسخه_HEX >= 0x02070000
	// 'import site' explicitly.
	site = PyImport_ImportModule("site");
	إذا (site == عدم)
	{
	    emsg(_(e_sorry_this_command_is_disabled_python_site_module_could_not_be_loaded));
	    اذهب fail;
	}
	Py_DECREF(site);
#پایان_اگر

	// Initialise threads, and below save the state using
	// PyEval_SaveThread.  Without the call to PyEval_SaveThread, thread
	// specific state (such as the system trace hook), will be lost
	// between invocations of Python code.
	Py_Eval_Init_نخها();
#اگر_تعریف_شده DYNAMIC_PYTHON
	get_exceptions();
#پایان_اگر

	إذا (PythonIO_Init_io())
	    اذهب fail;

	إذا (PythonMod_Init())
	    اذهب fail;

	globals = PyModule_GetDict(Py_Import_جمع_کن_Module("__main__"));

	// Remove the element from sys.path that was added because of our
	// argv[0] value in PythonMod_Init().  Previously we used an empty
	// string, but depending on the OS we then get an empty entry or
	// the current directory in sys.path.
	Py_پیاپی_Simple_String("import sys; sys.path = filter(lambda x: x != '/must>not&exist', sys.path)");

	// lock is created and acquired in PyEval_InitThreads() and thread
	// state is created in Py_Initialize()
	// there _PyGILState_NoteThreadState() also sets gilcounter to 1
	// (python must have threads enabled!)
	// so the following does both: unlock GIL and save thread state in TLS
	// without deleting thread state
#اگر_تعریف_نشده PY_CAN_RECURSE
	saved_python_thread =
#پایان_اگر
	    PyEval_SaveThread();

	initialised = 1;
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
DoPyCommand(
    ثابت حرف		*cmd,
    dict_T*		locals,
    rangeinitializer	init_range,
    runner		پیاپی,
    فراغ		*arg)
{
#اگر_تعریف_نشده PY_CAN_RECURSE
    ساكن صحيح		recursive = 0;
#پایان_اگر
#إذا defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    حرف		*saved_locale;
#پایان_اگر
#اگر_تعریف_شده PY_CAN_RECURSE
    PyGILState_STATE	pygilstate;
#پایان_اگر

#اگر_تعریف_نشده PY_CAN_RECURSE
    إذا (recursive)
    {
	emsg(_(e_cannot_invoke_python_recursively));
	أرجع;
    }
    ++recursive;
#پایان_اگر
    إذا (python_انتها_called)
	أرجع;

    إذا (Python_Init())
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
	saved_locale = (حرف *) PY_STRSAVE(saved_locale);
	(فراغ)setlocale(LC_NUMERIC, "C");
    }
#پایان_اگر

#اگر_تعریف_شده PY_CAN_RECURSE
    pygilstate = PyGILState_Ensure();
#وإلا
    Python_RestoreThread();	    // enter python
#پایان_اگر

    پیاپی((حرف *) cmd, locals, arg
#اگر_تعریف_شده PY_CAN_RECURSE
	    , &pygilstate
#پایان_اگر
	    );

#اگر_تعریف_شده PY_CAN_RECURSE
    PyGILState_Release(pygilstate);
#وإلا
    Python_SaveThread();	    // leave python
#پایان_اگر

#إذا defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    إذا (saved_locale != عدم)
    {
	(فراغ)setlocale(LC_NUMERIC, saved_locale);
	PyMem_Free(saved_locale);
    }
#پایان_اگر

    Python_قفل_Vim();		    // enter vim
    PythonIO_Flush();

theend:
#اگر_تعریف_نشده PY_CAN_RECURSE
    --recursive;
#پایان_اگر
    أرجع;
}

/*
 * ":python"
 */
    فراغ
ex_python(exarg_T *eap)
{
    char_u *script;

    script = script_get(eap, eap->arg);
    إذا (!eap->skip)
    {
	إذا (p_pyx == 0)
	    p_pyx = 2;

	DoPyCommand(script == عدم ? (حرف *) eap->arg : (حرف *) script,
		عدم,
		init_range_cmd,
		(runner) پیاپی_cmd,
		(فراغ *) eap);
    }
    vim_free(script);
}

#تعریف_کن میانگیر_اندازه 1024

/*
 * ":pyfile"
 */
    فراغ
ex_pyfile(exarg_T *eap)
{
    ساكن حرف میانگیر[میانگیر_اندازه];
    ثابت حرف *file = (حرف *)eap->arg;
    حرف *p;

    إذا (p_pyx == 0)
	p_pyx = 2;

    // Have to do it like this. PyRun_SimpleFile requires you to pass a
    // stdio file pointer, but Vim and the Python DLL are compiled with
    // different options under Windows, meaning that stdio pointers aren't
    // compatible between the two. Yuk.
    //
    // Put the string "execfile('file')" into buffer. But, we need to
    // escape any backslashes or single quotes in the file name, so that
    // Python won't mangle the file name.
    strcpy(میانگیر, "execfile('");
    p = میانگیر + 10; // size of "execfile('"

    طالما (*file && p < میانگیر + (میانگیر_اندازه - 3))
    {
	إذا (*file == '\\' || *file == '\'')
	    *p++ = '\\';
	*p++ = *file++;
    }

    // If we didn't finish the file name, we hit a buffer overflow
    إذا (*file != '\0')
	أرجع;

    // Put in the terminating "')" and a null
    *p++ = '\'';
    *p++ = ')';
    *p++ = '\0';

    // Execute the file
    DoPyCommand(میانگیر,
	    عدم,
	    init_range_cmd,
	    (runner) پیاپی_cmd,
	    (فراغ *) eap);
}

    فراغ
ex_pydo(exarg_T *eap)
{
    إذا (p_pyx == 0)
	p_pyx = 2;

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
OutputGetattr(PyObject *self, حرف *name)
{
    إذا (strcmp(name, "softspace") == 0)
	أرجع PyInt_FromLong(((OutputObject *)(self))->softspace);
    وإلا إذا (strcmp(name, "__members__") == 0)
	أرجع ObjectDir(عدم, OutputAttrs);
    وإلا إذا (strcmp(name, "errors") == 0)
	أرجع PyString_FromString("strict");
    وإلا إذا (strcmp(name, "encoding") == 0)
	أرجع PyString_FromString(ENC_OPT);
    أرجع Py_پیدا_کن_روش(OutputMethods, self, name);
}

///////////////////////////////////////////////////////
// 3. Implementation of the Vim module for Python

// Window type - Implementation functions
// --------------------------------------

#تعریف_کن پنجره_Type_Check(obj) ((obj)->ob_type == &پنجره_Type)

// Buffer type - Implementation functions
// --------------------------------------

#تعریف_کن میانگیر_Type_Check(obj) ((obj)->ob_type == &میانگیر_Type)

ساكن صحيح میانگیر_Ass_قلم(PyObject *, PyInt, PyObject *);
ساكن صحيح میانگیر_Ass_برش(PyObject *, PyInt, PyInt, PyObject *);

// Line range type - Implementation functions
// --------------------------------------

#تعریف_کن RangeType_Check(obj) ((obj)->ob_type == &RangeType)

ساكن صحيح Range_Ass_قلم(PyObject *, PyInt, PyObject *);
ساكن صحيح Range_Ass_برش(PyObject *, PyInt, PyInt, PyObject *);

// Current objects type - Implementation functions
// -----------------------------------------------

ساكن PySequenceMethods میانگیر_As_Seq = {
    (PyInquiry)		میانگیر_درازی,	    // sq_length,    len(x)
    (binaryfunc)	0,		    // BufferConcat, sq_concat, x+y
    (PyIntArgFunc)	0,		    // BufferRepeat, sq_repeat, x*n
    (PyIntArgFunc)	میانگیر_قلم,	    // sq_item,      x[i]
    (PyIntIntArgFunc)	میانگیر_برش,	    // sq_slice,     x[i:j]
    (PyIntObjArgProc)	میانگیر_Ass_قلم,	    // sq_ass_item,  x[i]=v
    (PyIntIntObjArgProc) میانگیر_Ass_برش,    // sq_ass_slice, x[i:j]=v
    (objobjproc)	0,
    (binaryfunc)	0,
    0,
};

// Buffer object - Implementation

    ساكن PyObject *
میانگیر_Getattr(PyObject *self, حرف *name)
{
    PyObject *r;

    إذا ((r = میانگیر_Attr_Valid((میانگیر_Object *)(self), name)))
	أرجع r;

    إذا (Check_میانگیر((میانگیر_Object *)(self)))
	أرجع عدم;

    r = میانگیر_Attr((میانگیر_Object *)(self), name);
    إذا (r || PyErr_Occurred())
	أرجع r;
    وإلا
	أرجع Py_پیدا_کن_روش(میانگیر_Methods, self, name);
}

//////////////////

    ساكن صحيح
میانگیر_Ass_قلم(PyObject *self, PyInt n, PyObject *val)
{
    أرجع RBAs_قلم((میانگیر_Object *)(self), n, val, 1, -1, عدم);
}

    ساكن صحيح
میانگیر_Ass_برش(PyObject *self, PyInt lo, PyInt hi, PyObject *val)
{
    أرجع RBAs_برش((میانگیر_Object *)(self), lo, hi, val, 1, -1, عدم);
}

ساكن PySequenceMethods RangeAsSeq = {
    (PyInquiry)		Range_درازی,	      // sq_length,    len(x)
    (binaryfunc)	0, /* RangeConcat, */ // sq_concat,    x+y
    (PyIntArgFunc)	0, /* RangeRepeat, */ // sq_repeat,    x*n
    (PyIntArgFunc)	Range_قلم,	      // sq_item,      x[i]
    (PyIntIntArgFunc)	Range_برش,	      // sq_slice,     x[i:j]
    (PyIntObjArgProc)	Range_Ass_قلم,	      // sq_ass_item,  x[i]=v
    (PyIntIntObjArgProc) Range_Ass_برش,	      // sq_ass_slice, x[i:j]=v
    (objobjproc)	0,
#إذا PY_MAJOR_نسخه >= 2
    (binaryfunc)	0,
    0,
#پایان_اگر
};

// Line range object - Implementation

    ساكن PyObject *
RangeGetattr(PyObject *self, حرف *name)
{
    إذا (strcmp(name, "start") == 0)
	أرجع Py_Build_مقدار(Py_ssize_t_fmt, ((RangeObject *)(self))->شروع - 1);
    وإلا إذا (strcmp(name, "end") == 0)
	أرجع Py_Build_مقدار(Py_ssize_t_fmt, ((RangeObject *)(self))->انتها - 1);
    وإلا إذا (strcmp(name, "__members__") == 0)
	أرجع ObjectDir(عدم, RangeAttrs);
    وإلا
	أرجع Py_پیدا_کن_روش(RangeMethods, self, name);
}

////////////////

    ساكن صحيح
Range_Ass_قلم(PyObject *self, PyInt n, PyObject *val)
{
    أرجع RBAs_قلم(((RangeObject *)(self))->buf, n, val,
		     ((RangeObject *)(self))->شروع,
		     ((RangeObject *)(self))->انتها,
		     &((RangeObject *)(self))->انتها);
}

    ساكن صحيح
Range_Ass_برش(PyObject *self, PyInt lo, PyInt hi, PyObject *val)
{
    أرجع RBAs_برش(((RangeObject *)(self))->buf, lo, hi, val,
		      ((RangeObject *)(self))->شروع,
		      ((RangeObject *)(self))->انتها,
		      &((RangeObject *)(self))->انتها);
}

// TabPage object - Implementation

    ساكن PyObject *
Tab_صفحه_Getattr(PyObject *self, حرف *name)
{
    PyObject *r;

    إذا ((r = Tab_صفحه_Attr_Valid((Tab_صفحه_Object *)(self), name)))
	أرجع r;

    إذا (Check_Tab_صفحه((Tab_صفحه_Object *)(self)))
	أرجع عدم;

    r = Tab_صفحه_Attr((Tab_صفحه_Object *)(self), name);
    إذا (r || PyErr_Occurred())
	أرجع r;
    وإلا
	أرجع Py_پیدا_کن_روش(Tab_صفحه_Methods, self, name);
}

// Window object - Implementation

    ساكن PyObject *
پنجره_Getattr(PyObject *self, حرف *name)
{
    PyObject *r;

    إذا ((r = پنجره_Attr_Valid((پنجره_Object *)(self), name)))
	أرجع r;

    إذا (Check_پنجره((پنجره_Object *)(self)))
	أرجع عدم;

    r = پنجره_Attr((پنجره_Object *)(self), name);
    إذا (r || PyErr_Occurred())
	أرجع r;
    وإلا
	أرجع Py_پیدا_کن_روش(پنجره_Methods, self, name);
}

// Tab page list object - Definitions

ساكن PySequenceMethods TabListAsSeq = {
    (PyInquiry)		Tab_List_درازی,	    // sq_length,    len(x)
    (binaryfunc)	0,		    // sq_concat,    x+y
    (PyIntArgFunc)	0,		    // sq_repeat,    x*n
    (PyIntArgFunc)	Tab_List_قلم,	    // sq_item,      x[i]
    (PyIntIntArgFunc)	0,		    // sq_slice,     x[i:j]
    (PyIntObjArgProc)	0,		    // sq_ass_item,  x[i]=v
    (PyIntIntObjArgProc) 0,		    // sq_ass_slice, x[i:j]=v
    (objobjproc)	0,
#إذا PY_MAJOR_نسخه >= 2
    (binaryfunc)	0,
    0,
#پایان_اگر
};

// Window list object - Definitions

ساكن PySequenceMethods WinListAsSeq = {
    (PyInquiry)		Win_List_درازی,	    // sq_length,    len(x)
    (binaryfunc)	0,		    // sq_concat,    x+y
    (PyIntArgFunc)	0,		    // sq_repeat,    x*n
    (PyIntArgFunc)	Win_List_قلم,	    // sq_item,      x[i]
    (PyIntIntArgFunc)	0,		    // sq_slice,     x[i:j]
    (PyIntObjArgProc)	0,		    // sq_ass_item,  x[i]=v
    (PyIntIntObjArgProc) 0,		    // sq_ass_slice, x[i:j]=v
    (objobjproc)	0,
#إذا PY_MAJOR_نسخه >= 2
    (binaryfunc)	0,
    0,
#پایان_اگر
};

// External interface

    فراغ
python_میانگیر_free(buf_T *buf)
{
    میانگیر_Object *bp = BUF_PYTHON_REF(buf);
    إذا (bp == عدم)
	أرجع;
    bp->buf = INVALID_میانگیر_مقدار;
    BUF_PYTHON_REF(buf) = عدم;
}

    فراغ
python_پنجره_free(win_T *win)
{
    پنجره_Object *wp = WIN_PYTHON_REF(win);
    إذا (wp == عدم)
	أرجع;
    wp->win = INVALID_پنجره_مقدار;
    WIN_PYTHON_REF(win) = عدم;
}

    فراغ
python_tabpage_free(tabpage_T *tab)
{
    Tab_صفحه_Object *tp = TAB_PYTHON_REF(tab);
    إذا (tp == عدم)
	أرجع;
    tp->tab = INVALID_TABPAGE_مقدار;
    TAB_PYTHON_REF(tab) = عدم;
}

    ساكن صحيح
PythonMod_Init(فراغ)
{
    // The special value is removed from sys.path in Python_Init().
    ساكن حرف	*(argv[2]) = {"/must>not&exist/foo", عدم};

    إذا (init_types())
	أرجع -1;

    // Set sys.argv[] to avoid a crash in warn().
    Py_Sys_مجموعه_Argv(1, argv);

    vim_module = Py_InitModule4("vim", VimMethods, (حرف *)عدم,
				(PyObject *)عدم, PYTHON_API_نسخه);

    إذا (populate_module(vim_module))
	أرجع -1;

    إذا (init_sys_مسیر())
	أرجع -1;

    أرجع 0;
}

//////////////////////////////////////////////////////////////////////////
// 4. Utility functions for handling the interface between Vim and Python.

// Convert a Vim line into a Python string.
// All internal newlines are replaced by null characters.
//
// On errors, the Python exception data is set, and NULL is returned.
    ساكن PyObject *
سطر_مقصد_String(ثابت حرف *str)
{
    PyObject *نتیجه;
    PyInt len = طول_رشته(str);
    حرف *p;

    // Allocate a Python string object, with uninitialised contents. We
    // must do it this way, so that we can modify the string in place
    // later. See the Python source, Objects/stringobject.c for details.
    نتیجه = Py_String_From_String_And_اندازه(عدم, len);
    إذا (نتیجه == عدم)
	أرجع عدم;

    p = PyString_AsString(نتیجه);

    طالما (*str)
    {
	إذا (*str == '\n')
	    *p = '\0';
	وإلا
	    *p = *str;

	++p;
	++str;
    }

    أرجع نتیجه;
}

    ساكن PyObject *
DictionaryGetattr(PyObject *self, حرف *name)
{
    DictionaryObject	*this = ((DictionaryObject *) (self));

    إذا (strcmp(name, "locked") == 0)
	أرجع PyInt_FromLong(this->dict->dv_قفل);
    وإلا إذا (strcmp(name, "scope") == 0)
	أرجع PyInt_FromLong(this->dict->dv_scope);
    وإلا إذا (strcmp(name, "__members__") == 0)
	أرجع ObjectDir(عدم, DictionaryAttrs);

    أرجع Py_پیدا_کن_روش(DictionaryMethods, self, name);
}

    ساكن PyObject *
ListGetattr(PyObject *self, حرف *name)
{
    إذا (strcmp(name, "locked") == 0)
	أرجع PyInt_FromLong(((ListObject *)(self))->list->lv_قفل);
    وإلا إذا (strcmp(name, "__members__") == 0)
	أرجع ObjectDir(عدم, ListAttrs);

    أرجع Py_پیدا_کن_روش(ListMethods, self, name);
}

    ساكن PyObject *
TupleGetattr(PyObject *self, حرف *name)
{
    إذا (strcmp(name, "locked") == 0)
	أرجع PyInt_FromLong(((TupleObject *)(self))->tuple->tv_قفل);
    وإلا إذا (strcmp(name, "__members__") == 0)
	أرجع ObjectDir(عدم, TupleAttrs);

    أرجع Py_پیدا_کن_روش(TupleMethods, self, name);
}

    ساكن PyObject *
FunctionGetattr(PyObject *self, حرف *name)
{
    PyObject	*r;

    r = FunctionAttr((FunctionObject *)(self), name);

    إذا (r || PyErr_Occurred())
	أرجع r;
    وإلا
	أرجع Py_پیدا_کن_روش(FunctionMethods, self, name);
}

    فراغ
do_pyeval(char_u *str, dict_T *locals, typval_T *rettv)
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
مجموعه_ref_درون_python(صحيح copyID)
{
    أرجع مجموعه_ref_درون_py(copyID);
}
