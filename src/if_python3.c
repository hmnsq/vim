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

#అన్తర్భావ "vim.h"

#అన్తర్భావ <limits.h>

#ఒకవేళ defined(MSWIN) && defined(HAVE_FCNTL_H)
# undef HAVE_FCNTL_H
#అయితే_ముగింపు

#నిర్వచితమైతే _DEBUG
# undef _DEBUG
#అయితే_ముగింపు

#నిర్వచితమైతే F_BLANK
# undef F_BLANK
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_DUP
# undef HAVE_DUP
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_STRFTIME
# undef HAVE_STRFTIME
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_STRING_H
# undef HAVE_STRING_H
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_PUTENV
# undef HAVE_PUTENV
#అయితే_ముగింపు
#నిర్వచితమైతే _POSIX_C_SOURCE  // defined in feature.h
# undef _POSIX_C_SOURCE
#అయితే_ముగింపు
#నిర్వచితమైతే XOPEN_ఆధారం
# undef XOPEN_ఆధారం	// pyconfig.h defines it as well.
#అయితే_ముగింపు

#నిర్వచించు PY_SSIZE_T_CLEAN

#నిర్వచితమైతే Py_LIMITED_API
# define USE_LIMITED_API // Using Python 3 limited ABI
#అయితే_ముగింపు

#అన్తర్భావ <Python.h>

#నిర్వచనం_రద్దు ప్రధాన // Defined in python.h - aargh
#నిర్వచనం_రద్దు HAVE_FCNTL_H // Clash with os_win32.h

// The "surrogateescape" error handler is new in Python 3.1
#ఒకవేళ PY_సంస్కరణ_HEX >= 0x030100f0
# define CODEC_ERROR_HANDLER "surrogateescape"
#లేకపోతే
# define CODEC_ERROR_HANDLER ఖాళీ
#అయితే_ముగింపు

// Suppress Python 3.11 depreciations to see useful warnings
#నిర్వచితమైతే __GNUC__
# pragma GCC diagnostic నెట్టు
# pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#అయితే_ముగింపు

// Python 3 does not support CObjects, always use Capsules
#నిర్వచించు PY_USE_CAPSULE

#నిర్వచించు ERRORS_DECODE_ARG CODEC_ERROR_HANDLER
#నిర్వచించు ERRORS_ENCODE_ARG ERRORS_DECODE_ARG

#నిర్వచించు PyInt Py_ssize_t
#నిర్వచితం_కాకపోతే PyString_Check
# define PyString_Check(obj) PyUnicode_Check(obj)
#అయితే_ముగింపు
#నిర్వచించు PyString_FromString(repr) \
    PyUnicode_Decode(repr, STRLEN(repr), ENC_OPT, ERRORS_DECODE_ARG)
#నిర్వచించు PyString_FromFormat PyUnicode_FromFormat
#నిర్వచితమైతే PyUnicode_FromFormat
# define Py_UNICODE_USE_UCS_FUNCTIONS
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే PyInt_Check
# define PyInt_Check(obj) PyLong_Check(obj)
#అయితే_ముగింపు
#నిర్వచించు PyInt_FromLong(i) PyLong_FromLong(i)
#నిర్వచించు PyInt_AsLong(obj) PyLong_AsLong(obj)
#నిర్వచించు Py_ssize_t_fmt "n"
#నిర్వచించు Py_బైట్లు_fmt "y"

#నిర్వచించు PyIntArgFunc	ssizeargfunc
#నిర్వచించు PyIntObjArgProc	ssizeobjargproc

/*
 * PySlice_GetIndicesEx(): first argument type changed from PySliceObject
 * to PyObject in Python 3.2 or later.
 */
#ఒకవేళ PY_సంస్కరణ_HEX >= 0x030200f0
రకనిర్వచనం PyObject Py_ముక్క_Object_T;
#లేకపోతే
రకనిర్వచనం Py_ముక్క_Object Py_ముక్క_Object_T;
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే MSWIN
# define HINSTANCE శూన్య *
#అయితే_ముగింపు
#ఒకవేళ defined(DYNAMIC_PYTHON3) || defined(MSWIN)
స్థిర HINSTANCE hinstPy3 = 0; // Instance of python.dll
#అయితే_ముగింపు

#ఒకవేళ defined(DYNAMIC_PYTHON3)

# ifdef MSWIN
#  define ఎక్కించు_dll vim_ఎక్కించు_Lib
#  define close_dll FreeLibrary
#  define చిహ్నం_from_dll Get_Proc_చిరునామా
#  define ఎక్కించు_dll_error GetWin32Error
# లేకపోతే
#  include <dlfcn.h>
#  define FARPROC శూన్య*
#  ఒకవేళ defined(PY_NO_RTLD_GLOBAL) && defined(PY3_NO_RTLD_GLOBAL)
#   define ఎక్కించు_dll(n) dlopen((n), RTLD_LAZY)
#  లేకపోతే
#   define ఎక్కించు_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#  endif
#  define close_dll dlclose
#  define చిహ్నం_from_dll dlsym
#  define ఎక్కించు_dll_error dlerror
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
# define Py_Dict_సమితి_అంశం_String py3_Py_Dict_సమితి_అంశం_String
# define Py_Err_Bad_వాదం py3_Py_Err_Bad_వాదం
# define PyErr_Clear py3_PyErr_Clear
# define PyErr_Format py3_PyErr_Format
# define PyErr_PrintEx py3_PyErr_PrintEx
# define Py_Err_No_జ్ఞాపకం py3_Py_Err_No_జ్ఞాపకం
# define PyErr_Occurred py3_PyErr_Occurred
# define Py_Err_సమితి_None py3_Py_Err_సమితి_None
# define Py_Err_సమితి_String py3_Py_Err_సమితి_String
# define Py_Err_సమితి_Object py3_Py_Err_సమితి_Object
# define PyErr_ExceptionMatches py3_PyErr_ExceptionMatches
# define Py_Eval_Init_దారాలు py3_Py_Eval_Init_దారాలు
# define PyEval_RestoreThread py3_PyEval_RestoreThread
# define PyEval_SaveThread py3_PyEval_SaveThread
# define PyGILState_Ensure py3_PyGILState_Ensure
# define PyGILState_Release py3_PyGILState_Release
# define PyLong_AsLong py3_PyLong_AsLong
# define PyLong_FromLong py3_PyLong_FromLong
# define Py_List_Get_అంశం py3_Py_List_Get_అంశం
# define PyList_Append py3_PyList_Append
# define Py_List_చొప్పించు py3_Py_List_చొప్పించు
# define PyList_New py3_PyList_New
# define Py_List_సమితి_అంశం_2 py3_Py_List_సమితి_అంశం
# define Py_List_కొలత py3_Py_List_కొలత
# define PyTuple_New py3_PyTuple_New
# define Py_Tuple_Get_అంశం py3_Py_Tuple_Get_అంశం
# define Py_Tuple_సమితి_అంశం py3_Py_Tuple_సమితి_అంశం
# define Py_Tuple_కొలత py3_Py_Tuple_కొలత
# define PySequence_Check py3_PySequence_Check
# define Py_Sequence_కొలత py3_Py_Sequence_కొలత
# define Py_Sequence_Get_అంశం py3_Py_Sequence_Get_అంశం
# define PySequence_Fast py3_PySequence_Fast
# ఒకవేళ PY_సంస్కరణ_HEX >= 0x030601f0
#  define Py_ముక్క_Adjust_Indices py3_Py_ముక్క_Adjust_Indices
#  define Py_ముక్క_Unpack py3_Py_ముక్క_Unpack
# endif
# undef Py_ముక్క_Get_Indices_Ex
# define Py_ముక్క_Get_Indices_Ex py3_Py_ముక్క_Get_Indices_Ex
# define PyImport_ImportModule py3_PyImport_ImportModule
# define PyObject_Init py3__PyObject_Init
# define PyDict_New py3_PyDict_New
# define Py_Dict_Get_అంశం_String py3_Py_Dict_Get_అంశం_String
# define Py_Dict_తదుపరి py3_Py_Dict_తదుపరి
# define PyMapping_Check py3_PyMapping_Check
# ifndef Py_Mapping_కీలకాలు
#  define Py_Mapping_కీలకాలు py3_Py_Mapping_కీలకాలు
# endif
# ఒకవేళ (defined(USE_LIMITED_API) && Py_LIMITED_API >= 0x03080000) || \
       (!defined(USE_LIMITED_API) && PY_సంస్కరణ_HEX >= 0x03080000)
#  undef PyIter_Check
#  define PyIter_Check py3_PyIter_Check
# endif
# define Py_Iter_తదుపరి py3_Py_Iter_తదుపరి
# define PyObject_GetIter py3_PyObject_GetIter
# define PyObject_Repr py3_PyObject_Repr
# define Py_Object_Get_అంశం py3_Py_Object_Get_అంశం
# define PyObject_IsTrue py3_PyObject_IsTrue
# define PyModule_GetDict py3_PyModule_GetDict
# ఒకవేళ defined(USE_LIMITED_API) || PY_సంస్కరణ_HEX >= 0x03080000
#  define Py_IncRef py3_Py_IncRef
#  define Py_DecRef py3_Py_DecRef
# endif
# ifdef USE_LIMITED_API
#  define Py_CompileString py3_Py_CompileString
#  define Py_Eval_Eval_సంకేతం py3_Py_Eval_Eval_సంకేతం
# లేకపోతే
#  undef Py_నిరంతరం_Simple_String
#  define Py_నిరంతరం_Simple_String py3_Py_నిరంతరం_Simple_String
#  undef Py_నిరంతరం_String
#  define Py_నిరంతరం_String py3_Py_నిరంతరం_String
# endif
# define PyObject_GetAttrString py3_PyObject_GetAttrString
# define PyObject_HasAttrString py3_PyObject_HasAttrString
# define Py_Object_సమితి_Attr_String py3_Py_Object_సమితి_Attr_String
# define Py_Object_Call_Function_Obj_వాదనలు py3_Py_Object_Call_Function_Obj_వాదనలు
# ఒకవేళ PY_సంస్కరణ_HEX >= 0x030d0000
#  define PyObject_CallFunction py3_PyObject_CallFunction
# లేకపోతే
#  define Py_Object_Call_Function_కొలత_T py3_Py_Object_Call_Function_కొలత_T
# endif
# define PyObject_Call py3_PyObject_Call
# define PyEval_GetLocals py3_PyEval_GetLocals
# define PyEval_GetGlobals py3_PyEval_GetGlobals
# define Py_Sys_సమితి_Object py3_Py_Sys_సమితి_Object
# define PySys_GetObject py3_PySys_GetObject
# define Py_Sys_సమితి_Argv py3_Py_Sys_సమితి_Argv
# define PyType_Ready py3_PyType_Ready
# ఒకవేళ PY_సంస్కరణ_HEX >= 0x03040000
#  define PyType_GetFlags py3_PyType_GetFlags
# endif
# undef Py_Build_విలువ
# define Py_Build_విలువ py3_Py_Build_విలువ
# define Py_సమితి_Python_Home py3_Py_సమితి_Python_Home
# define Py_Initialize py3_Py_Initialize
# define Py_Finalize py3_Py_Finalize
# define Py_IsInitialized py3_Py_IsInitialized
# define _Py_NoneStruct (*py3__Py_NoneStruct)
# define _Py_FalseStruct (*py3__Py_FalseStruct)
# define _Py_TrueStruct (*py3__Py_TrueStruct)
# ఒకవేళ !defined(USE_LIMITED_API) && PY_సంస్కరణ_HEX < 0x030D0000
// Private symbol that used to be required as part of PyIter_Check.
#  define Py_Object_తదుపరి_Not_Implemented (*py3_Py_Object_తదుపరి_Not_Implemented)
# endif
# define Py_Module_కూడు_Object py3_Py_Module_కూడు_Object
# define PyImport_AppendInittab py3_PyImport_AppendInittab
# define Py_Import_కూడు_Module py3_Py_Import_కూడు_Module
# ifdef USE_LIMITED_API
#  ఒకవేళ Py_LIMITED_API >= 0x030a0000
#   define Py_Unicode_As_UTF8_And_కొలత py3_Py_Unicode_As_UTF8_And_కొలత
#  endif
# లేకపోతే
#  ఒకవేళ PY_సంస్కరణ_HEX >= 0x030300f0
#   define Py_Unicode_As_UTF8_And_కొలత py3_Py_Unicode_As_UTF8_And_కొలత
#  లేకపోతే
#   define _PyUnicode_AsString py3__PyUnicode_AsString
#  endif
# endif
# undef PyUnicode_CompareWithASCIIString
# define PyUnicode_CompareWithASCIIString py3_PyUnicode_CompareWithASCIIString
# undef PyUnicode_AsEncodedString
# define PyUnicode_AsEncodedString py3_PyUnicode_AsEncodedString
# undef PyUnicode_AsUTF8String
# define PyUnicode_AsUTF8String py3_PyUnicode_AsUTF8String
# undef Py_బైట్లు_As_String
# define Py_బైట్లు_As_String py3_Py_బైట్లు_As_String
# ifndef Py_బైట్లు_As_String_And_కొలత
#  define Py_బైట్లు_As_String_And_కొలత py3_Py_బైట్లు_As_String_And_కొలత
# endif
# undef Py_బైట్లు_From_String
# define Py_బైట్లు_From_String py3_Py_బైట్లు_From_String
# undef Py_బైట్లు_From_String_And_కొలత
# define Py_బైట్లు_From_String_And_కొలత py3_Py_బైట్లు_From_String_And_కొలత
# define PyFloat_FromDouble py3_PyFloat_FromDouble
# define PyFloat_AsDouble py3_PyFloat_AsDouble
# define PyObject_GenericGetAttr py3_PyObject_GenericGetAttr
# define PyType_Type (*py3_PyType_Type)
# ifndef USE_LIMITED_API
#  define PyStdPrinter_Type (*py3_PyStdPrinter_Type)
# endif
# define Py_ముక్క_Type (*py3_Py_ముక్క_Type)
# define PyFloat_Type (*py3_PyFloat_Type)
# define Py_సంఖ్య_Check (*py3_Py_సంఖ్య_Check)
# define Py_సంఖ్య_Long (*py3_Py_సంఖ్య_Long)
# define PyBool_Type (*py3_PyBool_Type)
# define PyErr_NewException py3_PyErr_NewException
# ifdef Py_DEBUG
#  define _Py_NegativeRefcount py3__Py_NegativeRefcount
#  define Py_Ref_మొత్తం (*py3_Py_Ref_మొత్తం)
#  define PyModule_Create2TraceRefs py3_PyModule_Create2TraceRefs
# లేకపోతే
#  define PyModule_Create2 py3_PyModule_Create2
# endif
# ఒకవేళ defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
#  define _PyObject_DebugMalloc py3__PyObject_DebugMalloc
#  define _PyObject_DebugFree py3__PyObject_DebugFree
# లేకపోతే
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
#  లేకపోతే
#   define PyUnicodeUCS2_FromFormat py3_PyUnicodeUCS2_FromFormat
#  endif
# లేకపోతే
#  define PyUnicode_FromFormat py3_PyUnicode_FromFormat
# endif
# undef PyUnicode_Decode
# define PyUnicode_Decode py3_PyUnicode_Decode
# define PyType_IsSubtype py3_PyType_IsSubtype
# define PyCapsule_New py3_PyCapsule_New
# define Py_Capsule_Get_సూచిక py3_Py_Capsule_Get_సూచిక
# ifdef USE_LIMITED_API
#  define Py_Type_Get_స్థానం py3_Py_Type_Get_స్థానం
#  define PyType_FromSpec py3_PyType_FromSpec
# endif

# ఒకవేళ defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
#  undef PyObject_NEW
#  define PyObject_NEW(type, typeobj) \
( (type *) PyObject_Init( \
	(PyObject *) _PyObject_DebugMalloc( Py_Object_కొలత(typeobj) ), (typeobj)) )
# elif PY_సంస్కరణ_HEX >= 0x030900b0
#  undef PyObject_NEW
#  define PyObject_NEW(type, typeobj) \
	((type *)py3__PyObject_New(typeobj))
# endif

/*
 * Pointers for dynamic link
 */
స్థిర పూర్ణాంకం (*py3_Py_Sys_సమితి_Argv)(పూర్ణాంకం, wchar_t **);
స్థిర శూన్య (*py3_Py_సమితి_Python_Home)(wchar_t *home);
స్థిర శూన్య (*py3_Py_Initialize)(శూన్య);
స్థిర PyObject* (*py3_PyList_New)(Py_ssize_t కొలత);
స్థిర PyGILState_STATE (*py3_PyGILState_Ensure)(శూన్య);
స్థిర శూన్య (*py3_PyGILState_Release)(PyGILState_STATE);
స్థిర పూర్ణాంకం (*py3_Py_Sys_సమితి_Object)(అక్షరం *, PyObject *);
స్థిర PyObject* (*py3_PySys_GetObject)(అక్షరం *);
స్థిర పూర్ణాంకం (*py3_PyList_Append)(PyObject *, PyObject *);
స్థిర పూర్ణాంకం (*py3_Py_List_చొప్పించు)(PyObject *, పూర్ణాంకం, PyObject *);
స్థిర Py_ssize_t (*py3_Py_List_కొలత)(PyObject *);
స్థిర PyObject* (*py3_PyTuple_New)(Py_ssize_t కొలత);
స్థిర పూర్ణాంకం (*py3_Py_Tuple_సమితి_అంశం)(PyObject *, Py_ssize_t, PyObject *);
స్థిర Py_ssize_t (*py3_Py_Tuple_కొలత)(PyObject *);
స్థిర పూర్ణాంకం (*py3_PySequence_Check)(PyObject *);
స్థిర Py_ssize_t (*py3_Py_Sequence_కొలత)(PyObject *);
స్థిర PyObject* (*py3_Py_Sequence_Get_అంశం)(PyObject *, Py_ssize_t);
స్థిర PyObject* (*py3_PySequence_Fast)(PyObject *, స్థిరరాశి అక్షరం *);
స్థిర PyObject* (*py3_Py_Tuple_Get_అంశం)(PyObject *, Py_ssize_t);
స్థిర పూర్ణాంకం (*py3_PyMapping_Check)(PyObject *);
స్థిర PyObject* (*py3_Py_Mapping_కీలకాలు)(PyObject *);
# ఒకవేళ PY_సంస్కరణ_HEX >= 0x030601f0
స్థిర పూర్ణాంకం (*py3_Py_ముక్క_Adjust_Indices)(Py_ssize_t నిడివి,
		     Py_ssize_t *ప్రారంభం, Py_ssize_t *stop, Py_ssize_t అడుగు);
స్థిర పూర్ణాంకం (*py3_Py_ముక్క_Unpack)(PyObject *ముక్క,
		     Py_ssize_t *ప్రారంభం, Py_ssize_t *stop, Py_ssize_t *అడుగు);
# endif
స్థిర పూర్ణాంకం (*py3_Py_ముక్క_Get_Indices_Ex)(Py_ముక్క_Object_T *r, Py_ssize_t నిడివి,
		     Py_ssize_t *ప్రారంభం, Py_ssize_t *stop, Py_ssize_t *అడుగు,
		     Py_ssize_t *slicelen);
స్థిర PyObject* (*py3_Py_Err_No_జ్ఞాపకం)(శూన్య);
స్థిర శూన్య (*py3_Py_Finalize)(శూన్య);
స్థిర శూన్య (*py3_Py_Err_సమితి_String)(PyObject *, స్థిరరాశి అక్షరం *);
స్థిర శూన్య (*py3_Py_Err_సమితి_Object)(PyObject *, PyObject *);
స్థిర పూర్ణాంకం (*py3_PyErr_ExceptionMatches)(PyObject *);
# ఒకవేళ defined(USE_LIMITED_API) || PY_సంస్కరణ_HEX >= 0x03080000
స్థిర శూన్య (*py3_Py_IncRef)(PyObject *);
స్థిర శూన్య (*py3_Py_DecRef)(PyObject *);
# endif
# ifdef USE_LIMITED_API
స్థిర PyObject* (*py3_Py_CompileString)(స్థిరరాశి అక్షరం *, స్థిరరాశి అక్షరం *, పూర్ణాంకం);
స్థిర PyObject* (*py3_Py_Eval_Eval_సంకేతం)(PyObject *co, PyObject *globals, PyObject *locals);
# లేకపోతే
స్థిర పూర్ణాంకం (*py3_Py_నిరంతరం_Simple_String)(అక్షరం *);
స్థిర PyObject* (*py3_Py_నిరంతరం_String)(అక్షరం *, పూర్ణాంకం, PyObject *, PyObject *);
# endif
స్థిర PyObject* (*py3_PyObject_GetAttrString)(PyObject *, స్థిరరాశి అక్షరం *);
స్థిర పూర్ణాంకం (*py3_PyObject_HasAttrString)(PyObject *, స్థిరరాశి అక్షరం *);
స్థిర పూర్ణాంకం (*py3_Py_Object_సమితి_Attr_String)(PyObject *, స్థిరరాశి అక్షరం *, PyObject *);
స్థిర PyObject* (*py3_Py_Object_Call_Function_Obj_వాదనలు)(PyObject *, ...);
# ఒకవేళ PY_సంస్కరణ_HEX >= 0x030d0000
స్థిర PyObject* (*py3_PyObject_CallFunction)(PyObject *, అక్షరం *, ...);
# లేకపోతే
స్థిర PyObject* (*py3_Py_Object_Call_Function_కొలత_T)(PyObject *, అక్షరం *, ...);
# endif
స్థిర PyObject* (*py3_PyObject_Call)(PyObject *, PyObject *, PyObject *);
స్థిర PyObject* (*py3_PyEval_GetGlobals)(శూన్య);
స్థిర PyObject* (*py3_PyEval_GetLocals)(శూన్య);
స్థిర PyObject* (*py3_Py_List_Get_అంశం)(PyObject *, Py_ssize_t);
స్థిర PyObject* (*py3_PyImport_ImportModule)(స్థిరరాశి అక్షరం *);
స్థిర PyObject* (*py3_Py_Import_కూడు_Module)(స్థిరరాశి అక్షరం *);
స్థిర పూర్ణాంకం (*py3_Py_Err_Bad_వాదం)(శూన్య);
స్థిర PyObject* (*py3_PyErr_Occurred)(శూన్య);
స్థిర PyObject* (*py3_PyModule_GetDict)(PyObject *);
స్థిర పూర్ణాంకం (*py3_Py_List_సమితి_అంశం)(PyObject *, Py_ssize_t, PyObject *);
స్థిర PyObject* (*py3_Py_Dict_Get_అంశం_String)(PyObject *, స్థిరరాశి అక్షరం *);
స్థిర పూర్ణాంకం (*py3_Py_Dict_తదుపరి)(PyObject *, Py_ssize_t *, PyObject **, PyObject **);
స్థిర PyObject* (*py3_PyLong_FromLong)(దీర్ఘ);
స్థిర PyObject* (*py3_PyDict_New)(శూన్య);
# ఒకవేళ (defined(USE_LIMITED_API) && Py_LIMITED_API >= 0x03080000) || \
       (!defined(USE_LIMITED_API) && PY_సంస్కరణ_HEX >= 0x03080000)
స్థిర పూర్ణాంకం (*py3_PyIter_Check)(PyObject *o);
# endif
స్థిర PyObject* (*py3_Py_Iter_తదుపరి)(PyObject *);
స్థిర PyObject* (*py3_PyObject_GetIter)(PyObject *);
స్థిర PyObject* (*py3_PyObject_Repr)(PyObject *);
స్థిర PyObject* (*py3_Py_Object_Get_అంశం)(PyObject *, PyObject *);
స్థిర పూర్ణాంకం (*py3_PyObject_IsTrue)(PyObject *);
స్థిర PyObject* (*py3_Py_Build_విలువ)(అక్షరం *, ...);
# ఒకవేళ PY_సంస్కరణ_HEX >= 0x03040000
స్థిర పూర్ణాంకం (*py3_PyType_GetFlags)(PyTypeObject *o);
# endif
స్థిర పూర్ణాంకం (*py3_PyType_Ready)(PyTypeObject *type);
స్థిర పూర్ణాంకం (*py3_Py_Dict_సమితి_అంశం_String)(PyObject *dp, అక్షరం *కీలకం, PyObject *అంశం);
స్థిర PyObject* (*py3_PyUnicode_FromString)(స్థిరరాశి అక్షరం *u);
# ifdef Py_UNICODE_USE_UCS_FUNCTIONS
#  ifdef Py_UNICODE_WIDE
స్థిర PyObject* (*py3_PyUnicodeUCS4_FromFormat)(స్థిరరాశి అక్షరం *u, ...);
#  లేకపోతే
స్థిర PyObject* (*py3_PyUnicodeUCS2_FromFormat)(స్థిరరాశి అక్షరం *u, ...);
#  endif
# లేకపోతే
స్థిర PyObject* (*py3_PyUnicode_FromFormat)(స్థిరరాశి అక్షరం *u, ...);
# endif
స్థిర PyObject* (*py3_PyUnicode_Decode)(స్థిరరాశి అక్షరం *u, Py_ssize_t కొలత,
	స్థిరరాశి అక్షరం *encoding, స్థిరరాశి అక్షరం *errors);
స్థిర దీర్ఘ (*py3_PyLong_AsLong)(PyObject *);
స్థిర శూన్య (*py3_Py_Err_సమితి_None)(PyObject *);
స్థిర శూన్య (*py3_Py_Eval_Init_దారాలు)(శూన్య);
స్థిర శూన్య(*py3_PyEval_RestoreThread)(PyThreadState *);
స్థిర PyThreadState*(*py3_PyEval_SaveThread)(శూన్య);
స్థిర పూర్ణాంకం (*py3_PyArg_Parse)(PyObject *, అక్షరం *, ...);
స్థిర పూర్ణాంకం (*py3_PyArg_ParseTuple)(PyObject *, అక్షరం *, ...);
స్థిర శూన్య (*py3_PyMem_Free)(శూన్య *);
స్థిర శూన్య* (*py3_PyMem_Malloc)(size_t);
స్థిర పూర్ణాంకం (*py3_Py_IsInitialized)(శూన్య);
స్థిర శూన్య (*py3_PyErr_Clear)(శూన్య);
స్థిర PyObject* (*py3_PyErr_Format)(PyObject *, స్థిరరాశి అక్షరం *, ...);
స్థిర శూన్య (*py3_PyErr_PrintEx)(పూర్ణాంకం);
స్థిర PyObject*(*py3__PyObject_Init)(PyObject *, PyTypeObject *);
# ఒకవేళ !defined(USE_LIMITED_API) && PY_సంస్కరణ_HEX < 0x030D0000
స్థిర iternextfunc py3_Py_Object_తదుపరి_Not_Implemented;
# endif
స్థిర PyObject* py3__Py_NoneStruct;
స్థిర PyObject* py3__Py_FalseStruct;
స్థిర PyObject* py3__Py_TrueStruct;
స్థిర పూర్ణాంకం (*py3_Py_Module_కూడు_Object)(PyObject *m, స్థిరరాశి అక్షరం *name, PyObject *o);
స్థిర పూర్ణాంకం (*py3_PyImport_AppendInittab)(స్థిరరాశి అక్షరం *name, PyObject* (*initfunc)(శూన్య));
# ifdef USE_LIMITED_API
#  ఒకవేళ Py_LIMITED_API >= 0x030a0000
స్థిర అక్షరం* (*py3_Py_Unicode_As_UTF8_And_కొలత)(PyObject *unicode, Py_ssize_t *కొలత);
#  endif
# లేకపోతే
#  ఒకవేళ PY_సంస్కరణ_HEX >= 0x030300f0
స్థిర అక్షరం* (*py3_Py_Unicode_As_UTF8_And_కొలత)(PyObject *unicode, Py_ssize_t *కొలత);
#  లేకపోతే
స్థిర అక్షరం* (*py3__PyUnicode_AsString)(PyObject *unicode);
#  endif
# endif
స్థిర పూర్ణాంకం (*py3_PyUnicode_CompareWithASCIIString)(PyObject *unicode, స్థిరరాశి అక్షరం* string);
స్థిర PyObject* (*py3_PyUnicode_AsEncodedString)(PyObject *unicode, స్థిరరాశి అక్షరం* encoding, స్థిరరాశి అక్షరం* errors);
స్థిర PyObject* (*py3_PyUnicode_AsUTF8String)(PyObject *unicode);
స్థిర అక్షరం* (*py3_Py_బైట్లు_As_String)(PyObject *బైట్లు);
స్థిర పూర్ణాంకం (*py3_Py_బైట్లు_As_String_And_కొలత)(PyObject *బైట్లు, అక్షరం **బఫరు, Py_ssize_t *నిడివి);
స్థిర PyObject* (*py3_Py_బైట్లు_From_String)(అక్షరం *str);
స్థిర PyObject* (*py3_Py_బైట్లు_From_String_And_కొలత)(అక్షరం *str, Py_ssize_t నిడివి);
# ఒకవేళ PY_సంస్కరణ_HEX >= 0x030900b0
స్థిర PyObject* (*py3__PyObject_New)(PyTypeObject *);
# endif
స్థిర PyObject* (*py3_PyFloat_FromDouble)(ద్విగుణ అంకె);
స్థిర ద్విగుణ (*py3_PyFloat_AsDouble)(PyObject *);
స్థిర PyObject* (*py3_PyObject_GenericGetAttr)(PyObject *obj, PyObject *name);
స్థిర PyObject* (*py3_PyType_GenericAlloc)(PyTypeObject *type, Py_ssize_t nitems);
స్థిర PyObject* (*py3_PyType_GenericNew)(PyTypeObject *type, PyObject *వాదనలు, PyObject *kwds);
స్థిర PyTypeObject* py3_PyType_Type;
# ifndef USE_LIMITED_API
స్థిర PyTypeObject* py3_PyStdPrinter_Type;
# endif
స్థిర PyTypeObject* py3_Py_ముక్క_Type;
స్థిర PyTypeObject* py3_PyFloat_Type;
స్థిర PyTypeObject* py3_PyBool_Type;
స్థిర పూర్ణాంకం (*py3_Py_సంఖ్య_Check)(PyObject *);
స్థిర PyObject* (*py3_Py_సంఖ్య_Long)(PyObject *);
స్థిర PyObject* (*py3_PyErr_NewException)(అక్షరం *name, PyObject *పునాది, PyObject *dict);
స్థిర PyObject* (*py3_PyCapsule_New)(శూన్య *, అక్షరం *, PyCapsule_Destructor);
స్థిర శూన్య* (*py3_Py_Capsule_Get_సూచిక)(PyObject *, అక్షరం *);
# ifdef Py_DEBUG
స్థిర శూన్య (*py3__Py_NegativeRefcount)(స్థిరరాశి అక్షరం *fname, పూర్ణాంకం lineno, PyObject *op);
స్థిర Py_ssize_t* py3_Py_Ref_మొత్తం;
స్థిర PyObject* (*py3_PyModule_Create2TraceRefs)(నిర్మాణం PyModuleDef* module, పూర్ణాంకం module_api_సంస్కరణ);
# లేకపోతే
స్థిర PyObject* (*py3_PyModule_Create2)(నిర్మాణం PyModuleDef* module, పూర్ణాంకం module_api_సంస్కరణ);
# endif
# ఒకవేళ defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
స్థిర శూన్య (*py3__PyObject_DebugFree)(శూన్య*);
స్థిర శూన్య* (*py3__PyObject_DebugMalloc)(size_t);
# లేకపోతే
స్థిర శూన్య (*py3_PyObject_Free)(శూన్య*);
స్థిర శూన్య* (*py3_PyObject_Malloc)(size_t);
# endif
స్థిర PyObject*(*py3__PyObject_GC_New)(PyTypeObject *);
స్థిర శూన్య(*py3_PyObject_GC_Del)(శూన్య *);
స్థిర శూన్య(*py3_PyObject_GC_UnTrack)(శూన్య *);
స్థిర పూర్ణాంకం (*py3_PyType_IsSubtype)(PyTypeObject *, PyTypeObject *);
# ifdef USE_LIMITED_API
స్థిర శూన్య* (*py3_Py_Type_Get_స్థానం)(PyTypeObject *, పూర్ణాంకం);
స్థిర PyObject* (*py3_PyType_FromSpec)(PyType_Spec *);
# endif

// Imported exception objects
స్థిర PyObject *p3imp_PyExc_AttributeError;
స్థిర PyObject *p3imp_Py_Exc_సూచీ_Error;
స్థిర PyObject *p3imp_Py_Exc_కీలకం_Error;
స్థిర PyObject *p3imp_Py_Exc_Keyboard_అంతరాయం;
స్థిర PyObject *p3imp_PyExc_TypeError;
స్థిర PyObject *p3imp_Py_Exc_విలువ_Error;
స్థిర PyObject *p3imp_PyExc_SystemExit;
స్థిర PyObject *p3imp_PyExc_RuntimeError;
స్థిర PyObject *p3imp_PyExc_ImportError;
స్థిర PyObject *p3imp_Py_Exc_అధికప్రవాహం_Error;

# define PyExc_AttributeError p3imp_PyExc_AttributeError
# define Py_Exc_సూచీ_Error p3imp_Py_Exc_సూచీ_Error
# define Py_Exc_కీలకం_Error p3imp_Py_Exc_కీలకం_Error
# define Py_Exc_Keyboard_అంతరాయం p3imp_Py_Exc_Keyboard_అంతరాయం
# define PyExc_TypeError p3imp_PyExc_TypeError
# define Py_Exc_విలువ_Error p3imp_Py_Exc_విలువ_Error
# define PyExc_SystemExit p3imp_PyExc_SystemExit
# define PyExc_RuntimeError p3imp_PyExc_RuntimeError
# define PyExc_ImportError p3imp_PyExc_ImportError
# define Py_Exc_అధికప్రవాహం_Error p3imp_Py_Exc_అధికప్రవాహం_Error

/*
 * Table of name to function pointer of python.
 */
# define PYTHON_PROC FARPROC
స్థిర నిర్మాణం
{
    అక్షరం *name;
    PYTHON_PROC *ptr;
} py3_funcname_పట్టిక[] =
{
    {"PySys_SetArgv", (PYTHON_PROC*)&py3_Py_Sys_సమితి_Argv},
    {"Py_SetPythonHome", (PYTHON_PROC*)&py3_Py_సమితి_Python_Home},
    {"Py_Initialize", (PYTHON_PROC*)&py3_Py_Initialize},
    {"_PyArg_ParseTuple_SizeT", (PYTHON_PROC*)&py3_PyArg_ParseTuple},
    {"_Py_BuildValue_SizeT", (PYTHON_PROC*)&py3_Py_Build_విలువ},
    {"PyMem_Free", (PYTHON_PROC*)&py3_PyMem_Free},
    {"PyMem_Malloc", (PYTHON_PROC*)&py3_PyMem_Malloc},
    {"PyList_New", (PYTHON_PROC*)&py3_PyList_New},
    {"PyGILState_Ensure", (PYTHON_PROC*)&py3_PyGILState_Ensure},
    {"PyGILState_Release", (PYTHON_PROC*)&py3_PyGILState_Release},
    {"PySys_SetObject", (PYTHON_PROC*)&py3_Py_Sys_సమితి_Object},
    {"PySys_GetObject", (PYTHON_PROC*)&py3_PySys_GetObject},
    {"PyList_Append", (PYTHON_PROC*)&py3_PyList_Append},
    {"PyList_Insert", (PYTHON_PROC*)&py3_Py_List_చొప్పించు},
    {"PyList_Size", (PYTHON_PROC*)&py3_Py_List_కొలత},
    {"PyTuple_New", (PYTHON_PROC*)&py3_PyTuple_New},
    {"PyTuple_GetItem", (PYTHON_PROC*)&py3_Py_Tuple_Get_అంశం},
    {"PyTuple_SetItem", (PYTHON_PROC*)&py3_Py_Tuple_సమితి_అంశం},
    {"PyTuple_Size", (PYTHON_PROC*)&py3_Py_Tuple_కొలత},
    {"PySequence_Check", (PYTHON_PROC*)&py3_PySequence_Check},
    {"PySequence_Size", (PYTHON_PROC*)&py3_Py_Sequence_కొలత},
    {"PySequence_GetItem", (PYTHON_PROC*)&py3_Py_Sequence_Get_అంశం},
    {"PySequence_Fast", (PYTHON_PROC*)&py3_PySequence_Fast},
# ఒకవేళ PY_సంస్కరణ_HEX >= 0x030601f0
    {"PySlice_AdjustIndices", (PYTHON_PROC*)&py3_Py_ముక్క_Adjust_Indices},
    {"PySlice_Unpack", (PYTHON_PROC*)&py3_Py_ముక్క_Unpack},
# endif
    {"PySlice_GetIndicesEx", (PYTHON_PROC*)&py3_Py_ముక్క_Get_Indices_Ex},
    {"PyErr_NoMemory", (PYTHON_PROC*)&py3_Py_Err_No_జ్ఞాపకం},
    {"Py_Finalize", (PYTHON_PROC*)&py3_Py_Finalize},
    {"PyErr_SetString", (PYTHON_PROC*)&py3_Py_Err_సమితి_String},
    {"PyErr_SetObject", (PYTHON_PROC*)&py3_Py_Err_సమితి_Object},
    {"PyErr_ExceptionMatches", (PYTHON_PROC*)&py3_PyErr_ExceptionMatches},
# ఒకవేళ defined(USE_LIMITED_API) || PY_సంస్కరణ_HEX >= 0x03080000
    {"Py_IncRef", (PYTHON_PROC*)&py3_Py_IncRef},
    {"Py_DecRef", (PYTHON_PROC*)&py3_Py_DecRef},
# endif
# ifdef USE_LIMITED_API
    {"Py_CompileString", (PYTHON_PROC*)&py3_Py_CompileString},
    {"PyEval_EvalCode", (PYTHON_PROC*)&Py_Eval_Eval_సంకేతం},
# లేకపోతే
    {"PyRun_SimpleString", (PYTHON_PROC*)&py3_Py_నిరంతరం_Simple_String},
    {"PyRun_String", (PYTHON_PROC*)&py3_Py_నిరంతరం_String},
# endif
    {"PyObject_GetAttrString", (PYTHON_PROC*)&py3_PyObject_GetAttrString},
    {"PyObject_HasAttrString", (PYTHON_PROC*)&py3_PyObject_HasAttrString},
    {"PyObject_SetAttrString", (PYTHON_PROC*)&py3_Py_Object_సమితి_Attr_String},
    {"PyObject_CallFunctionObjArgs", (PYTHON_PROC*)&py3_Py_Object_Call_Function_Obj_వాదనలు},
# ఒకవేళ PY_సంస్కరణ_HEX >= 0x030d0000
    {"PyObject_CallFunction", (PYTHON_PROC*)&py3_PyObject_CallFunction},
# లేకపోతే
    {"_PyObject_CallFunction_SizeT", (PYTHON_PROC*)&py3_Py_Object_Call_Function_కొలత_T},
# endif
    {"PyObject_Call", (PYTHON_PROC*)&py3_PyObject_Call},
    {"PyEval_GetGlobals", (PYTHON_PROC*)&py3_PyEval_GetGlobals},
    {"PyEval_GetLocals", (PYTHON_PROC*)&py3_PyEval_GetLocals},
    {"PyList_GetItem", (PYTHON_PROC*)&py3_Py_List_Get_అంశం},
    {"PyImport_ImportModule", (PYTHON_PROC*)&py3_PyImport_ImportModule},
    {"PyImport_AddModule", (PYTHON_PROC*)&py3_Py_Import_కూడు_Module},
    {"PyErr_BadArgument", (PYTHON_PROC*)&py3_Py_Err_Bad_వాదం},
    {"PyErr_Occurred", (PYTHON_PROC*)&py3_PyErr_Occurred},
    {"PyModule_GetDict", (PYTHON_PROC*)&py3_PyModule_GetDict},
    {"PyList_SetItem", (PYTHON_PROC*)&py3_Py_List_సమితి_అంశం},
    {"PyDict_GetItemString", (PYTHON_PROC*)&py3_Py_Dict_Get_అంశం_String},
    {"PyDict_Next", (PYTHON_PROC*)&py3_Py_Dict_తదుపరి},
    {"PyMapping_Check", (PYTHON_PROC*)&py3_PyMapping_Check},
    {"PyMapping_Keys", (PYTHON_PROC*)&py3_Py_Mapping_కీలకాలు},
# ఒకవేళ (defined(USE_LIMITED_API) && Py_LIMITED_API >= 0x03080000) || \
       (!defined(USE_LIMITED_API) && PY_సంస్కరణ_HEX >= 0x03080000)
    {"PyIter_Check", (PYTHON_PROC*)&py3_PyIter_Check},
# endif
    {"PyIter_Next", (PYTHON_PROC*)&py3_Py_Iter_తదుపరి},
    {"PyObject_GetIter", (PYTHON_PROC*)&py3_PyObject_GetIter},
    {"PyObject_Repr", (PYTHON_PROC*)&py3_PyObject_Repr},
    {"PyObject_GetItem", (PYTHON_PROC*)&py3_Py_Object_Get_అంశం},
    {"PyObject_IsTrue", (PYTHON_PROC*)&py3_PyObject_IsTrue},
    {"PyLong_FromLong", (PYTHON_PROC*)&py3_PyLong_FromLong},
    {"PyDict_New", (PYTHON_PROC*)&py3_PyDict_New},
# ఒకవేళ PY_సంస్కరణ_HEX >= 0x03040000
    {"PyType_GetFlags", (PYTHON_PROC*)&py3_PyType_GetFlags},
# endif
    {"PyType_Ready", (PYTHON_PROC*)&py3_PyType_Ready},
    {"PyDict_SetItemString", (PYTHON_PROC*)&py3_Py_Dict_సమితి_అంశం_String},
    {"PyLong_AsLong", (PYTHON_PROC*)&py3_PyLong_AsLong},
    {"PyErr_SetNone", (PYTHON_PROC*)&py3_Py_Err_సమితి_None},
    {"PyEval_InitThreads", (PYTHON_PROC*)&py3_Py_Eval_Init_దారాలు},
    {"PyEval_RestoreThread", (PYTHON_PROC*)&py3_PyEval_RestoreThread},
    {"PyEval_SaveThread", (PYTHON_PROC*)&py3_PyEval_SaveThread},
    {"_PyArg_Parse_SizeT", (PYTHON_PROC*)&py3_PyArg_Parse},
    {"Py_IsInitialized", (PYTHON_PROC*)&py3_Py_IsInitialized},
# ఒకవేళ !defined(USE_LIMITED_API) && PY_సంస్కరణ_HEX < 0x030D0000
    {"_PyObject_NextNotImplemented", (PYTHON_PROC*)&py3_Py_Object_తదుపరి_Not_Implemented},
# endif
    {"_Py_NoneStruct", (PYTHON_PROC*)&py3__Py_NoneStruct},
    {"_Py_FalseStruct", (PYTHON_PROC*)&py3__Py_FalseStruct},
    {"_Py_TrueStruct", (PYTHON_PROC*)&py3__Py_TrueStruct},
    {"PyErr_Clear", (PYTHON_PROC*)&py3_PyErr_Clear},
    {"PyErr_Format", (PYTHON_PROC*)&py3_PyErr_Format},
    {"PyErr_PrintEx", (PYTHON_PROC*)&py3_PyErr_PrintEx},
    {"PyObject_Init", (PYTHON_PROC*)&py3__PyObject_Init},
    {"PyModule_AddObject", (PYTHON_PROC*)&py3_Py_Module_కూడు_Object},
    {"PyImport_AppendInittab", (PYTHON_PROC*)&py3_PyImport_AppendInittab},
# ifdef USE_LIMITED_API
#  ఒకవేళ Py_LIMITED_API >= 0x030a0000
    {"PyUnicode_AsUTF8AndSize", (PYTHON_PROC*)&py3_Py_Unicode_As_UTF8_And_కొలత},
#  endif
# లేకపోతే
#  ఒకవేళ PY_సంస్కరణ_HEX >= 0x030300f0
    {"PyUnicode_AsUTF8AndSize", (PYTHON_PROC*)&py3_Py_Unicode_As_UTF8_And_కొలత},
#  లేకపోతే
    {"_PyUnicode_AsString", (PYTHON_PROC*)&py3__PyUnicode_AsString},
#  endif
# endif
    {"PyUnicode_CompareWithASCIIString", (PYTHON_PROC*)&py3_PyUnicode_CompareWithASCIIString},
    {"PyUnicode_AsUTF8String", (PYTHON_PROC*)&py3_PyUnicode_AsUTF8String},
# ifdef Py_UNICODE_USE_UCS_FUNCTIONS
#  ifdef Py_UNICODE_WIDE
    {"PyUnicodeUCS4_FromFormat", (PYTHON_PROC*)&py3_PyUnicodeUCS4_FromFormat},
#  లేకపోతే
    {"PyUnicodeUCS2_FromFormat", (PYTHON_PROC*)&py3_PyUnicodeUCS2_FromFormat},
#  endif
# లేకపోతే
    {"PyUnicode_FromFormat", (PYTHON_PROC*)&py3_PyUnicode_FromFormat},
# endif
    {"PyBytes_AsString", (PYTHON_PROC*)&py3_Py_బైట్లు_As_String},
    {"PyBytes_AsStringAndSize", (PYTHON_PROC*)&py3_Py_బైట్లు_As_String_And_కొలత},
    {"PyBytes_FromString", (PYTHON_PROC*)&py3_Py_బైట్లు_From_String},
    {"PyBytes_FromStringAndSize", (PYTHON_PROC*)&py3_Py_బైట్లు_From_String_And_కొలత},
# ఒకవేళ PY_సంస్కరణ_HEX >= 0x030900b0
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
    {"PySlice_Type", (PYTHON_PROC*)&py3_Py_ముక్క_Type},
    {"PyFloat_Type", (PYTHON_PROC*)&py3_PyFloat_Type},
    {"PyBool_Type", (PYTHON_PROC*)&py3_PyBool_Type},
    {"PyNumber_Check", (PYTHON_PROC*)&py3_Py_సంఖ్య_Check},
    {"PyNumber_Long", (PYTHON_PROC*)&py3_Py_సంఖ్య_Long},
    {"PyErr_NewException", (PYTHON_PROC*)&py3_PyErr_NewException},
# ifdef Py_DEBUG
    {"_Py_NegativeRefcount", (PYTHON_PROC*)&py3__Py_NegativeRefcount},
    {"_Py_RefTotal", (PYTHON_PROC*)&py3_Py_Ref_మొత్తం},
    {"PyModule_Create2TraceRefs", (PYTHON_PROC*)&py3_PyModule_Create2TraceRefs},
# లేకపోతే
    {"PyModule_Create2", (PYTHON_PROC*)&py3_PyModule_Create2},
# endif
# ఒకవేళ defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
    {"_PyObject_DebugFree", (PYTHON_PROC*)&py3__PyObject_DebugFree},
    {"_PyObject_DebugMalloc", (PYTHON_PROC*)&py3__PyObject_DebugMalloc},
# లేకపోతే
    {"PyObject_Malloc", (PYTHON_PROC*)&py3_PyObject_Malloc},
    {"PyObject_Free", (PYTHON_PROC*)&py3_PyObject_Free},
# endif
    {"_PyObject_GC_New", (PYTHON_PROC*)&py3__PyObject_GC_New},
    {"PyObject_GC_Del", (PYTHON_PROC*)&py3_PyObject_GC_Del},
    {"PyObject_GC_UnTrack", (PYTHON_PROC*)&py3_PyObject_GC_UnTrack},
    {"PyType_IsSubtype", (PYTHON_PROC*)&py3_PyType_IsSubtype},
    {"PyCapsule_New", (PYTHON_PROC*)&py3_PyCapsule_New},
    {"PyCapsule_GetPointer", (PYTHON_PROC*)&py3_Py_Capsule_Get_సూచిక},
# ifdef USE_LIMITED_API
#  ఒకవేళ PY_సంస్కరణ_HEX >= 0x03040000
    {"PyType_GetSlot", (PYTHON_PROC*)&py3_Py_Type_Get_స్థానం},
#  endif
    {"PyType_FromSpec", (PYTHON_PROC*)&py3_PyType_FromSpec},
# endif
    {"", ఖాళీ},
};

# ఒకవేళ defined(USE_LIMITED_API) || PY_సంస్కరణ_HEX >= 0x03080000
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

# ఒకవేళ PY_సంస్కరణ_HEX >= 0x030900b0
    స్థిర అంతఃవరుస పూర్ణాంకం
py3_PyType_HasFeature(PyTypeObject *type, గుర్తులేని దీర్ఘ feature)
{
    తిరిగి ((PyType_GetFlags(type) & feature) != 0);
}
#  define PyType_HasFeature(t,f) py3_PyType_HasFeature(t,f)
# endif

# ఒకవేళ PY_సంస్కరణ_HEX >= 0x030a00b2
    స్థిర అంతఃవరుస పూర్ణాంకం
py3__PyObject_TypeCheck(PyObject *ob, PyTypeObject *type)
{
    తిరిగి Py_IS_TYPE(ob, type) || PyType_IsSubtype(Py_TYPE(ob), type);
}
#  ఒకవేళ PY_సంస్కరణ_HEX >= 0x030b00b3
#   undef PyObject_TypeCheck
#   define PyObject_TypeCheck(o,t) py3__PyObject_TypeCheck(o,t)
#  లేకపోతే
#   define _PyObject_TypeCheck(o,t) py3__PyObject_TypeCheck(o,t)
#  endif
# endif

# ఒకవేళ !defined(USE_LIMITED_API) && PY_సంస్కరణ_HEX >= 0x030c00b0
// PyTuple_GET_SIZE/PyList_GET_SIZE are inlined functions that use Py_SIZE(),
// which started to introduce linkage dependency from Python 3.12. When we
// build Python in dynamic mode, we don't link against it in build time, and
// this would fail to build. Just use the non-inlined version instead.
#  undef Py_Tuple_GET_కొలత
#  define Py_Tuple_GET_కొలత(o) Py_Tuple_కొలత(o)
#  undef Py_List_GET_కొలత
#  define Py_List_GET_కొలత(o) Py_List_కొలత(o)
# endif

# ifdef MSWIN
/*
 * Look up the library "libname" using the InstallPath registry key.
 * Return NULL when failed.  Return an allocated string when successful.
 */
    స్థిర WCHAR *
py3_get_system_libname(స్థిరరాశి అక్షరం *libname)
{
    స్థిరరాశి WCHAR	*pythoncore = L"Software\\Python\\PythonCore";
    స్థిరరాశి అక్షరం	*cp = libname;
    WCHAR	subkey[128];
    HKEY	h_కీలకం;
    పూర్ణాంకం		i;
    DWORD	j, len;
    LSTATUS	ret;

    అయితే (*cp != '\0')
    {
	ఒకవేళ (*cp == ':' || *cp == '\\' || *cp == '/')
	{
	    // Bail out if "libname" contains path separator, assume it is
	    // an absolute path.
	    తిరిగి ఖాళీ;
	}
	++cp;
    }

    WCHAR   keyfound[32];
    HKEY    h_కీలకం_శిఖరం[] = {HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE};
    HKEY    h_కీలకం_దొరికింది = ఖాళీ;
#  ifdef USE_LIMITED_API
    దీర్ఘ    maxminor = -1;
#  endif
    కొరకు (i = 0; i < ARRAY_నిడివి(h_కీలకం_శిఖరం); i++)
    {
	దీర్ఘ	major, minor;

	ret = Reg_Open_కీలకం_Ex_W(h_కీలకం_శిఖరం[i], pythoncore, 0, కీలకం_READ, &h_కీలకం);
	ఒకవేళ (ret != ERROR_SUCCESS)
	    కొనసాగు;
	కొరకు (j = 0;; j++)
	{
	    WCHAR   keyname[32];
	    WCHAR   *wp;

	    len = ARRAY_నిడివి(keyname);
	    ret = Reg_Enum_కీలకం_Ex_W(h_కీలకం, j, keyname, &len,
						    ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ);
	    ఒకవేళ (ret == ERROR_NO_మరిన్ని_వస్తువులు)
		ఆపు;

	    major = wcstol(keyname, &wp, 10);
	    ఒకవేళ (*wp != L'.')
		కొనసాగు;
	    minor = wcstol(wp + 1, &wp, 10);
#  ifdef _WIN64
	    ఒకవేళ (*wp != L'\0')
		కొనసాగు;
#  లేకపోతే
	    ఒకవేళ (wcscmp(wp, L"-32") != 0)
		కొనసాగు;
#  endif

	    ఒకవేళ (major != PY_MAJOR_సంస్కరణ)
		కొనసాగు;
#  ifdef USE_LIMITED_API
	    // Search the latest version.
	    ఒకవేళ ((minor > maxminor)
		    && (minor >= ((Py_LIMITED_API >> 16) & 0xff)))
	    {
		maxminor = minor;
		wcscpy(keyfound, keyname);
		h_కీలకం_దొరికింది = h_కీలకం_శిఖరం[i];
	    }
#  లేకపోతే
	    // Check if it matches with the compiled version.
	    ఒకవేళ (minor == PY_MINOR_సంస్కరణ)
	    {
		wcscpy(keyfound, keyname);
		h_కీలకం_దొరికింది = h_కీలకం_శిఖరం[i];
		ఆపు;
	    }
#  endif
	}
	Reg_Close_కీలకం(h_కీలకం);
#  ifdef USE_LIMITED_API
	ఒకవేళ (h_కీలకం_దొరికింది != ఖాళీ)
	    ఆపు;
#  endif
    }
    ఒకవేళ (h_కీలకం_దొరికింది == ఖాళీ)
	తిరిగి ఖాళీ;

    swprintf(subkey, ARRAY_నిడివి(subkey), L"%ls\\%ls\\InstallPath",
							pythoncore, keyfound);
    ret = Reg_Get_విలువ_W(h_కీలకం_దొరికింది, subkey, ఖాళీ, RRF_RT_REG_SZ,
							    ఖాళీ, ఖాళీ, &len);
    ఒకవేళ (ret != ERROR_మరిన్ని_దత్తాంశం && ret != ERROR_SUCCESS)
	తిరిగి ఖాళీ;
    size_t len2 = len / పరిమాణం(WCHAR) + 1 + అక్షరశ్రేణి_పొడవు(libname);
    WCHAR *మార్గం = alloc(len2 * పరిమాణం(WCHAR));
    ఒకవేళ (మార్గం == ఖాళీ)
	తిరిగి ఖాళీ;
    ret = Reg_Get_విలువ_W(h_కీలకం_దొరికింది, subkey, ఖాళీ, RRF_RT_REG_SZ,
							    ఖాళీ, మార్గం, &len);
    ఒకవేళ (ret != ERROR_SUCCESS)
    {
	vim_free(మార్గం);
	తిరిగి ఖాళీ;
    }
    // Remove trailing path separators.
    size_t len3 = wcslen(మార్గం);
    ఒకవేళ ((len3 > 0) && (మార్గం[len3 - 1] == L'/' || మార్గం[len3 - 1] == L'\\'))
	--len3;
    swprintf(మార్గం + len3, len2 - len3, L"\\%hs", libname);
    తిరిగి మార్గం;
}
# endif

/*
 * Load library and get all pointers.
 * Parameter 'libname' provides name of DLL.
 * Return OK or FAIL.
 */
    స్థిర పూర్ణాంకం
py3_runtime_link_init(అక్షరం *libname, పూర్ణాంకం verbose)
{
    పూర్ణాంకం i;
    PYTHON_PROC *ucs_from_string = (PYTHON_PROC *)&py3_PyUnicode_FromString;
    PYTHON_PROC *ucs_decode = (PYTHON_PROC *)&py3_PyUnicode_Decode;
    PYTHON_PROC *ucs_as_encoded_string =
				 (PYTHON_PROC *)&py3_PyUnicode_AsEncodedString;

# ఒకవేళ !(defined(PY_NO_RTLD_GLOBAL) && defined(PY3_NO_RTLD_GLOBAL)) && defined(UNIX) && defined(FEAT_PYTHON)
    // Can't have Python and Python3 loaded at the same time.
    // It causes a crash, because RTLD_GLOBAL is needed for
    // standard C extension libraries of one or both python versions.
    ఒకవేళ (python_loaded())
    {
	ఒకవేళ (verbose)
	    emsg(_(e_this_vim_cannot_execute_py3_after_using_python));
	తిరిగి FAIL;
    }
# endif

    ఒకవేళ (hinstPy3 != 0)
	తిరిగి OK;
    hinstPy3 = ఎక్కించు_dll(libname);

# ifdef MSWIN
    ఒకవేళ (!hinstPy3)
    {
	// Attempt to use the path from InstallPath as stored in the registry.
	WCHAR *syslibname = py3_get_system_libname(libname);

	ఒకవేళ (syslibname != ఖాళీ)
	{
	    hinstPy3 = ఎక్కించు_Library_Ex_W(syslibname, ఖాళీ,
		    ఎక్కించు_LIBRARY_శోధన_DLL_ఎక్కించు_DIR |
		    ఎక్కించు_LIBRARY_శోధన_SYSTEM32);
	    vim_free(syslibname);
	}
    }
# endif

    ఒకవేళ (!hinstPy3)
    {
	ఒకవేళ (verbose)
	    semsg(_(e_could_not_ఎక్కించు_library_str_str), libname, ఎక్కించు_dll_error());
	తిరిగి FAIL;
    }

    కొరకు (i = 0; py3_funcname_పట్టిక[i].ptr; ++i)
    {
	ఒకవేళ ((*py3_funcname_పట్టిక[i].ptr = చిహ్నం_from_dll(hinstPy3,
			py3_funcname_పట్టిక[i].name)) == ఖాళీ)
	{
	    close_dll(hinstPy3);
	    hinstPy3 = 0;
	    ఒకవేళ (verbose)
		semsg(_(e_could_not_ఎక్కించు_library_function_str), py3_funcname_పట్టిక[i].name);
	    తిరిగి FAIL;
	}
    }

    // Load unicode functions separately as only the ucs2 or the ucs4 functions
    // will be present in the library.
# ఒకవేళ PY_సంస్కరణ_HEX >= 0x030300f0
    *ucs_from_string = చిహ్నం_from_dll(hinstPy3, "PyUnicode_FromString");
    *ucs_decode = చిహ్నం_from_dll(hinstPy3, "PyUnicode_Decode");
    *ucs_as_encoded_string = చిహ్నం_from_dll(hinstPy3,
	    "PyUnicode_AsEncodedString");
# లేకపోతే
    *ucs_from_string = చిహ్నం_from_dll(hinstPy3, "PyUnicodeUCS2_FromString");
    *ucs_decode = చిహ్నం_from_dll(hinstPy3,
	    "PyUnicodeUCS2_Decode");
    *ucs_as_encoded_string = చిహ్నం_from_dll(hinstPy3,
	    "PyUnicodeUCS2_AsEncodedString");
    ఒకవేళ (*ucs_from_string == ఖాళీ || *ucs_decode == ఖాళీ
					     || *ucs_as_encoded_string == ఖాళీ)
    {
	*ucs_from_string = చిహ్నం_from_dll(hinstPy3,
		"PyUnicodeUCS4_FromString");
	*ucs_decode = చిహ్నం_from_dll(hinstPy3,
		"PyUnicodeUCS4_Decode");
	*ucs_as_encoded_string = చిహ్నం_from_dll(hinstPy3,
		"PyUnicodeUCS4_AsEncodedString");
    }
# endif
    ఒకవేళ (*ucs_from_string == ఖాళీ || *ucs_decode == ఖాళీ
					     || *ucs_as_encoded_string == ఖాళీ)
    {
	close_dll(hinstPy3);
	hinstPy3 = 0;
	ఒకవేళ (verbose)
	    semsg(_(e_could_not_ఎక్కించు_library_function_str), "PyUnicode_UCSX_*");
	తిరిగి FAIL;
    }

    తిరిగి OK;
}

/*
 * If python is enabled (there is installed python on Windows system) return
 * TRUE, else FALSE.
 */
    పూర్ణాంకం
python3_enabled(పూర్ణాంకం verbose)
{
    తిరిగి py3_runtime_link_init((అక్షరం *)p_py3dll, verbose) == OK;
}

/*
 * Load the standard Python exceptions - don't import the symbols from the
 * DLL, as this can cause errors (importing data symbols is not reliable).
 */
    స్థిర శూన్య
get_py3_exceptions(శూన్య)
{
    PyObject *exmod = PyImport_ImportModule("builtins");
    PyObject *exdict = PyModule_GetDict(exmod);
    p3imp_PyExc_AttributeError = Py_Dict_Get_అంశం_String(exdict, "AttributeError");
    p3imp_Py_Exc_సూచీ_Error = Py_Dict_Get_అంశం_String(exdict, "IndexError");
    p3imp_Py_Exc_కీలకం_Error = Py_Dict_Get_అంశం_String(exdict, "KeyError");
    p3imp_Py_Exc_Keyboard_అంతరాయం = Py_Dict_Get_అంశం_String(exdict, "KeyboardInterrupt");
    p3imp_PyExc_TypeError = Py_Dict_Get_అంశం_String(exdict, "TypeError");
    p3imp_Py_Exc_విలువ_Error = Py_Dict_Get_అంశం_String(exdict, "ValueError");
    p3imp_PyExc_SystemExit = Py_Dict_Get_అంశం_String(exdict, "SystemExit");
    p3imp_PyExc_RuntimeError = Py_Dict_Get_అంశం_String(exdict, "RuntimeError");
    p3imp_PyExc_ImportError = Py_Dict_Get_అంశం_String(exdict, "ImportError");
    p3imp_Py_Exc_అధికప్రవాహం_Error = Py_Dict_Get_అంశం_String(exdict, "OverflowError");
    Py_XINCREF(p3imp_PyExc_AttributeError);
    Py_XINCREF(p3imp_Py_Exc_సూచీ_Error);
    Py_XINCREF(p3imp_Py_Exc_కీలకం_Error);
    Py_XINCREF(p3imp_Py_Exc_Keyboard_అంతరాయం);
    Py_XINCREF(p3imp_PyExc_TypeError);
    Py_XINCREF(p3imp_Py_Exc_విలువ_Error);
    Py_XINCREF(p3imp_PyExc_SystemExit);
    Py_XINCREF(p3imp_PyExc_RuntimeError);
    Py_XINCREF(p3imp_PyExc_ImportError);
    Py_XINCREF(p3imp_Py_Exc_అధికప్రవాహం_Error);
    Py_XDECREF(exmod);
}
#అయితే_ముగింపు // DYNAMIC_PYTHON3

స్థిర పూర్ణాంకం py3initialised = 0;
#నిర్వచించు PYINITIALISED py3initialised
స్థిర పూర్ణాంకం python_చివర_called = FALSE;

#నిర్వచితమైతే USE_LIMITED_API
# define DESTRUCTOR_ముగింపు(self) \
    ((freefunc)Py_Type_Get_స్థానం(Py_TYPE((PyObject*)self), Py_tp_free))((PyObject*)self)
#లేకపోతే
# define DESTRUCTOR_ముగింపు(self) Py_TYPE(self)->tp_free((PyObject*)self)
#అయితే_ముగింపు

#నిర్వచించు WIN_PYTHON_REF(win) win->w_python3_ref
#నిర్వచించు BUF_PYTHON_REF(buf) buf->b_python3_ref
#నిర్వచించు TAB_PYTHON_REF(tab) tab->tp_python3_ref

    స్థిర శూన్య
call_PyObject_Free(శూన్య *p)
{
#ఒకవేళ defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
    _PyObject_DebugFree(p);
#లేకపోతే
    PyObject_Free(p);
#అయితే_ముగింపు
}

    స్థిర PyObject *
call_PyType_GenericNew(PyTypeObject *type, PyObject *వాదనలు, PyObject *kwds)
{
    తిరిగి PyType_GenericNew(type,వాదనలు,kwds);
}

    స్థిర PyObject *
call_PyType_GenericAlloc(PyTypeObject *type, Py_ssize_t nitems)
{
    తిరిగి PyType_GenericAlloc(type,nitems);
}

స్థిర PyObject *OutputGetattro(PyObject *, PyObject *);
స్థిర పూర్ణాంకం OutputSetattro(PyObject *, PyObject *, PyObject *);
స్థిర PyObject *బఫరు_Getattro(PyObject *, PyObject *);
స్థిర పూర్ణాంకం బఫరు_Setattro(PyObject *, PyObject *, PyObject *);
స్థిర PyObject *Tab_పుట_Getattro(PyObject *, PyObject *);
స్థిర PyObject *కిటికీ_Getattro(PyObject *, PyObject *);
స్థిర పూర్ణాంకం కిటికీ_Setattro(PyObject *, PyObject *, PyObject *);
స్థిర PyObject *RangeGetattro(PyObject *, PyObject *);
స్థిర PyObject *CurrentGetattro(PyObject *, PyObject *);
స్థిర పూర్ణాంకం CurrentSetattro(PyObject *, PyObject *, PyObject *);
స్థిర PyObject *DictionaryGetattro(PyObject *, PyObject *);
స్థిర పూర్ణాంకం DictionarySetattro(PyObject *, PyObject *, PyObject *);
స్థిర PyObject *ListGetattro(PyObject *, PyObject *);
స్థిర పూర్ణాంకం ListSetattro(PyObject *, PyObject *, PyObject *);
స్థిర PyObject *TupleGetattro(PyObject *, PyObject *);
స్థిర పూర్ణాంకం TupleSetattro(PyObject *, PyObject *, PyObject *);
స్థిర PyObject *FunctionGetattro(PyObject *, PyObject *);

స్థిర నిర్మాణం PyModuleDef vimmodule;

#నిర్వచించు PY_CAN_RECURSE

/*
 * Include the code shared with if_python.c
 */
#అన్తర్భావ "if_py_both.h"

#నిర్వచితమైతే USE_LIMITED_API
# ఒకవేళ Py_LIMITED_API >= 0x030A0000
#  define PY_UNICODE_GET_UTF8_CHARS(obj) Py_Unicode_As_UTF8_And_కొలత(obj, ఖాళీ)
# లేకపోతే
// Python limited API before 3.10 lack easy ways to query the raw UTF-8 chars.
// We need to first convert the string to bytes, and then extract the chars.
// This function is only used for attribute string comparisons, which have
// known short length. As such, just allocate a short static buffer to hold
// the characters instead of having to allocate/deallcoate it.
//
// An alternative would be to convert all attribute string comparisons to use
// PyUnicode_CompareWithASCIIString to skip having to extract the chars.
స్థిర అక్షరం py3_unicode_utf8_chars[20];
స్థిర అక్షరం* PY_UNICODE_GET_UTF8_CHARS(PyObject* str)
{
    py3_unicode_utf8_chars[0] = '\0';
    PyObject* బైట్లు = PyUnicode_AsUTF8String(str);
    ఒకవేళ (బైట్లు)
    {
	అక్షరం *chars;
	Py_ssize_t len;
	ఒకవేళ (Py_బైట్లు_As_String_And_కొలత(బైట్లు, &chars, &len) != -1)
	{
	    ఒకవేళ (len < (Py_ssize_t)పరిమాణం(py3_unicode_utf8_chars))
		// PyBytes_AsStringAndSize guarantees null-termination
		జ్ఞాపకం_నకలు(py3_unicode_utf8_chars, chars, len + 1);
	}
	Py_DECREF(బైట్లు);
    }
    తిరిగి py3_unicode_utf8_chars;
}
# endif
#లేకపోతే	// !USE_LIMITED_API
# ఒకవేళ PY_సంస్కరణ_HEX >= 0x030300f0
#  define PY_UNICODE_GET_UTF8_CHARS(obj) Py_Unicode_As_UTF8_And_కొలత(obj, ఖాళీ)
# లేకపోతే
#  define PY_UNICODE_GET_UTF8_CHARS _PyUnicode_AsString
# endif
#అయితే_ముగింపు

// NOTE: Must always be used at the start of a block, since it declares "name".
#నిర్వచించు GET_ATTR_STRING(name, nameobj) \
    అక్షరం	*name = ""; \
    ఒకవేళ (PyUnicode_Check(nameobj)) \
	name = (అక్షరం *)PY_UNICODE_GET_UTF8_CHARS(nameobj)

#నిర్వచించు PY3OBJ_DELETED(obj) (obj->ob_పునాది.ob_refcnt<=0)

///////////////////////////////////////////////////////
// Internal function prototypes.

స్థిర PyObject *Py3Init_vim(శూన్య);

///////////////////////////////////////////////////////
// 1. Python interpreter main program.

    శూన్య
python3_చివర(శూన్య)
{
    స్థిర పూర్ణాంకం recurse = 0;

    // If a crash occurs while doing this, don't try again.
    ఒకవేళ (recurse != 0)
	తిరిగి;

    python_చివర_called = TRUE;
    ++recurse;

#నిర్వచితమైతే DYNAMIC_PYTHON3
    ఒకవేళ (hinstPy3)
#అయితే_ముగింపు
    ఒకవేళ (Py_IsInitialized())
    {
#నిర్వచితమైతే USE_LIMITED_API
	shutdown_types();
#అయితే_ముగింపు

	// acquire lock before finalizing
	PyGILState_Ensure();

	Py_Finalize();
    }

    --recurse;
}

#ఒకవేళ defined(DYNAMIC_PYTHON3) && defined(DYNAMIC_PYTHON) && defined(FEAT_PYTHON) && defined(UNIX)
    పూర్ణాంకం
python3_loaded(శూన్య)
{
    తిరిగి (hinstPy3 != 0);
}
#అయితే_ముగింపు

స్థిర wchar_t *py_home_buf = ఖాళీ;

#ఒకవేళ defined(MSWIN) && (PY_సంస్కరణ_HEX >= 0x030500f0)
/*
 * Return TRUE if stdin is readable from Python 3.
 */
    స్థిర BOOL
is_stdin_readable(శూన్య)
{
    DWORD	    బహుళకం, eventnum;
    నిర్మాణం _stat    st;
    పూర్ణాంకం		    fd = fileno(stdin);
    HANDLE	    hstdin = (HANDLE)_get_osfhandle(fd);

    // Check if stdin is connected to the console.
    ఒకవేళ (Get_Console_బహుళకం(hstdin, &బహుళకం))
	// Check if it is opened as input.
	తిరిగి Get_సంఖ్య_యొక్క_Console_ఆదానం_Events(hstdin, &eventnum);

    తిరిగి _fstat(fd, &st) == 0;
}

// Python 3.5 or later will abort inside Py_Initialize() when stdin has
// been closed (i.e. executed by "vim -").  Reconnect stdin to CONIN$.
// Note that the python DLL is linked to its own stdio DLL which can be
// differ from Vim's stdio.
    స్థిర శూన్య
reset_stdin(శూన్య)
{
    FILE *(*py__acrt_iob_func)(గుర్తులేని) = ఖాళీ;
    FILE *(*pyfreopen)(స్థిరరాశి అక్షరం *, స్థిరరాశి అక్షరం *, FILE *) = ఖాళీ;
    HINSTANCE hinst = get_forwarded_dll(hinstPy3);

    ఒకవేళ (hinst == ఖాళీ || is_stdin_readable())
	తిరిగి;

    // Get "freopen" and "stdin" which are used in the python DLL.
    // "stdin" is defined as "__acrt_iob_func(0)" in VC++ 2015 or later.
    py__acrt_iob_func = get_dll_import_func(hinst, "__acrt_iob_func");
    ఒకవేళ (py__acrt_iob_func)
    {
	HINSTANCE hpystdiodll = కనుగొను_imported_module_by_funcname(hinst,
							    "__acrt_iob_func");
	ఒకవేళ (hpystdiodll)
	    pyfreopen = (శూన్య *)Get_Proc_చిరునామా(hpystdiodll, "freopen");
    }

    // Reconnect stdin to CONIN$.
    ఒకవేళ (pyfreopen != ఖాళీ)
	pyfreopen("CONIN$", "r", py__acrt_iob_func(0));
    లేకపోతే
	freopen("CONIN$", "r", stdin);
}
#లేకపోతే
# define reset_stdin()
#అయితే_ముగింపు

// Python 3.2 or later will abort inside Py_Initialize() when mandatory
// modules cannot be loaded (e.g. 'pythonthreehome' is wrongly set.).
// Install a hook to python dll's exit() and recover from it.
#ఒకవేళ defined(MSWIN) && (PY_సంస్కరణ_HEX >= 0x030200f0)
# define HOOK_EXIT
# include <setjmp.h>

స్థిర jmp_buf exit_hook_jump_buf;
స్థిర శూన్య *orig_exit = ఖాళీ;

/*
 * Function that replaces exit() while calling Py_Initialize().
 */
    స్థిర శూన్య
hooked_exit(పూర్ణాంకం ret)
{
    // Recover from exit.
    longjmp(exit_hook_jump_buf, 1);
}

/*
 * Install a hook to python dll's exit().
 */
    స్థిర శూన్య
hook_py_exit(శూన్య)
{
    HINSTANCE hinst = get_forwarded_dll(hinstPy3);

    ఒకవేళ (hinst == ఖాళీ || orig_exit != ఖాళీ)
	తిరిగి;

    orig_exit = hook_dll_import_func(hinst, "exit", (శూన్య *)hooked_exit);
}

/*
 * Remove the hook installed by hook_py_exit().
 */
    స్థిర శూన్య
restore_py_exit(శూన్య)
{
    HINSTANCE hinst = hinstPy3;

    ఒకవేళ (hinst == ఖాళీ)
	తిరిగి;

    ఒకవేళ (orig_exit != ఖాళీ)
	hook_dll_import_func(hinst, "exit", orig_exit);
    orig_exit = ఖాళీ;
}
#అయితే_ముగింపు

    స్థిర పూర్ణాంకం
Python3_Init(శూన్య)
{
    ఒకవేళ (!py3initialised)
    {
#నిర్వచితమైతే DYNAMIC_PYTHON3
	ఒకవేళ (!python3_enabled(TRUE))
	{
	    emsg(_(e_sorry_this_command_is_disabled_python_library_could_not_be_దొరికింది));
	    వెళ్ళు fail;
	}
#అయితే_ముగింపు
	// Python 3.13 introduced a really useful feature: colorized exceptions.
	// This is great if you're reading them from the terminal, but useless
	// and broken everywhere else (such as in log files, or text editors).
	// Opt out, forcefully.
	vim_setenv((char_u*)"PYTHON_COLORS", (char_u*)"0");

	init_structs();

	ఒకవేళ (*p_py3home != NUL)
	{
	    size_t len = mbstowcs(ఖాళీ, (అక్షరం *)p_py3home, 0) + 1;

	    // The string must not change later, make a copy in static memory.
	    py_home_buf = ALLOC_MULT(wchar_t, len);
	    ఒకవేళ (py_home_buf != ఖాళీ && mbstowcs(
			    py_home_buf, (అక్షరం *)p_py3home, len) != (size_t)-1)
		Py_సమితి_Python_Home(py_home_buf);
	}
#నిర్వచితమైతే PYTHON3_HOME
	లేకపోతే ఒకవేళ (mch_getenv((char_u *)"PYTHONHOME") == ఖాళీ)
	    Py_సమితి_Python_Home(PYTHON3_HOME);
#అయితే_ముగింపు

	PyImport_AppendInittab("vim", Py3Init_vim);

#ఒకవేళ !defined(DYNAMIC_PYTHON3) && defined(MSWIN)
	hinstPy3 = GetModuleHandle(PYTHON3_DLL);
#అయితే_ముగింపు
	reset_stdin();

#నిర్వచితమైతే HOOK_EXIT
	// Catch exit() called in Py_Initialize().
	hook_py_exit();
	ఒకవేళ (setjmp(exit_hook_jump_buf) == 0)
	{
	    Py_Initialize();
	    restore_py_exit();
	}
	లేకపోతే
	{
	    // exit() was called in Py_Initialize().
	    restore_py_exit();
	    emsg(_(e_critical_error_లోపల_python3_initialization_check_your_installation));
	    వెళ్ళు fail;
	}
#లేకపోతే
	Py_Initialize();
#అయితే_ముగింపు

#ఒకవేళ PY_సంస్కరణ_HEX < 0x03090000
	// Initialise threads.  This is deprecated since Python 3.9.
	Py_Eval_Init_దారాలు();
#అయితే_ముగింపు
#నిర్వచితమైతే DYNAMIC_PYTHON3
	get_py3_exceptions();
#అయితే_ముగింపు

	ఒకవేళ (PythonIO_Init_io())
	    వెళ్ళు fail;

	globals = PyModule_GetDict(Py_Import_కూడు_Module("__main__"));

	// Remove the element from sys.path that was added because of our
	// argv[0] value in Py3Init_vim().  Previously we used an empty
	// string, but depending on the OS we then get an empty entry or
	// the current directory in sys.path.
	// Only after vim has been imported, the element does exist in
	// sys.path.
	Py_నిరంతరం_Simple_String("import vim; import sys; sys.path = list(filter(lambda x: not x.endswith('must>not&exist'), sys.path))");

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

    తిరిగి 0;

fail:
    // We call PythonIO_Flush() here to print any Python errors.
    // This is OK, as it is possible to call this function even
    // if PythonIO_Init_io() has not completed successfully (it will
    // not do anything in this case).
    PythonIO_Flush();
    తిరిగి -1;
}

/*
 * External interface
 */
    స్థిర శూన్య
DoPyCommand(స్థిరరాశి అక్షరం *cmd,
	    dict_T* locals,
	    rangeinitializer init_range,
	    runner నిరంతరం,
	    శూన్య *arg)
{
#ఒకవేళ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    అక్షరం		*saved_locale;
#అయితే_ముగింపు
    PyObject		*cmdstr;
    PyObject		*cmdbytes;
    PyGILState_STATE	pygilstate;

    ఒకవేళ (python_చివర_called)
	వెళ్ళు theend;

    ఒకవేళ (Python3_Init())
	వెళ్ళు theend;

    init_range(arg);

    Python_Release_Vim();	    // leave Vim

#ఒకవేళ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    // Python only works properly when the LC_NUMERIC locale is "C".
    saved_locale = setlocale(LC_NUMERIC, ఖాళీ);
    ఒకవేళ (saved_locale == ఖాళీ || STRCMP(saved_locale, "C") == 0)
	saved_locale = ఖాళీ;
    లేకపోతే
    {
	// Need to make a copy, value may change when setting new locale.
	saved_locale = (అక్షరం *)vim_strsave((char_u *)saved_locale);
	(శూన్య)setlocale(LC_NUMERIC, "C");
    }
#అయితే_ముగింపు

    pygilstate = PyGILState_Ensure();

    // PyRun_SimpleString expects a UTF-8 string. Wrong encoding may cause
    // SyntaxError (unicode error).
    cmdstr = PyUnicode_Decode(cmd, అక్షరశ్రేణి_పొడవు(cmd),
					(అక్షరం *)ENC_OPT, ERRORS_DECODE_ARG);
    cmdbytes = PyUnicode_AsEncodedString(cmdstr, "utf-8", ERRORS_ENCODE_ARG);
    Py_XDECREF(cmdstr);

    నిరంతరం(Py_బైట్లు_As_String(cmdbytes), locals, arg, &pygilstate);
    Py_XDECREF(cmdbytes);

    PyGILState_Release(pygilstate);

#ఒకవేళ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    ఒకవేళ (saved_locale != ఖాళీ)
    {
	(శూన్య)setlocale(LC_NUMERIC, saved_locale);
	vim_free(saved_locale);
    }
#అయితే_ముగింపు

    Python_తాళం_Vim();		    // enter Vim
    PythonIO_Flush();

theend:
    తిరిగి;	    // keeps lint happy
}

/*
 * ":py3"
 */
    శూన్య
ex_py3(exarg_T *eap)
{
    char_u *script;

    script = script_get(eap, eap->arg);
    ఒకవేళ (!eap->skip)
    {
	ఒకవేళ (p_pyx == 0)
	    p_pyx = 3;

	DoPyCommand(script == ఖాళీ ? (అక్షరం *) eap->arg : (అక్షరం *) script,
		ఖాళీ,
		init_range_cmd,
		(runner) నిరంతరం_cmd,
		(శూన్య *) eap);
    }
    vim_free(script);
}

#నిర్వచించు బఫరు_కొలత 2048

/*
 * ":py3file"
 */
    శూన్య
ex_py3file(exarg_T *eap)
{
    స్థిర అక్షరం బఫరు[బఫరు_కొలత];
    స్థిరరాశి అక్షరం *file;
    అక్షరం *p;
    పూర్ణాంకం i;

    ఒకవేళ (p_pyx == 0)
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

    strcpy(బఫరు, "exec(compile(open('");
    p = బఫరు + 19; // size of "exec(compile(open('"

    కొరకు (i=0; i<2; ++i)
    {
	file = (అక్షరం *)eap->arg;
	అయితే (*file && p < బఫరు + (బఫరు_కొలత - 3))
	{
	    ఒకవేళ (*file == '\\' || *file == '\'')
		*p++ = '\\';
	    *p++ = *file++;
	}
	// If we didn't finish the file name, we hit a buffer overflow
	ఒకవేళ (*file != '\0')
	    తిరిగి;
	ఒకవేళ (i==0)
	{
	    strcpy(p,"','rb').read(),'");
	    p += 16;
	}
	లేకపోతే
	{
	    strcpy(p,"','exec'))");
	    p += 10;
	}
    }


    // Execute the file
    DoPyCommand(బఫరు,
	    ఖాళీ,
	    init_range_cmd,
	    (runner) నిరంతరం_cmd,
	    (శూన్య *) eap);
}

    శూన్య
ex_py3do(exarg_T *eap)
{
    ఒకవేళ (p_pyx == 0)
	p_pyx = 3;

    DoPyCommand((అక్షరం *)eap->arg,
	    ఖాళీ,
	    init_range_cmd,
	    (runner)నిరంతరం_do,
	    (శూన్య *)eap);
}

///////////////////////////////////////////////////////
// 2. Python output stream: writes output via [e]msg().

// Implementation functions

    స్థిర PyObject *
OutputGetattro(PyObject *self, PyObject *nameobj)
{
    GET_ATTR_STRING(name, nameobj);

    ఒకవేళ (strcmp(name, "softspace") == 0)
	తిరిగి PyLong_FromLong(((OutputObject *)(self))->softspace);
    లేకపోతే ఒకవేళ (strcmp(name, "errors") == 0)
	తిరిగి PyString_FromString("strict");
    లేకపోతే ఒకవేళ (strcmp(name, "encoding") == 0)
	తిరిగి PyString_FromString(ENC_OPT);

    తిరిగి PyObject_GenericGetAttr(self, nameobj);
}

    స్థిర పూర్ణాంకం
OutputSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);

    తిరిగి OutputSetattr(self, name, val);
}

///////////////////////////////////////////////////////
// 3. Implementation of the Vim module for Python

// Window type - Implementation functions
// --------------------------------------

#నిర్వచించు కిటికీ_Type_Check(obj) ((obj)->ob_పునాది.ob_type == &కిటికీ_Type)

// Buffer type - Implementation functions
// --------------------------------------

#నిర్వచించు బఫరు_Type_Check(obj) ((obj)->ob_పునాది.ob_type == &బఫరు_Type)

స్థిర PyObject* బఫరు_Subscript(PyObject *self, PyObject *idx);
స్థిర పూర్ణాంకం బఫరు_As_Subscript(PyObject *self, PyObject *idx, PyObject *val);

// Line range type - Implementation functions
// --------------------------------------

#నిర్వచించు RangeType_Check(obj) ((obj)->ob_పునాది.ob_type == &RangeType)

స్థిర PyObject* RangeSubscript(PyObject *self, PyObject *idx);
స్థిర పూర్ణాంకం Range_As_అంశం(PyObject *, Py_ssize_t, PyObject *);
స్థిర పూర్ణాంకం RangeAsSubscript(PyObject *self, PyObject *idx, PyObject *val);

// Current objects type - Implementation functions
// -----------------------------------------------

స్థిర PySequenceMethods బఫరు_As_Seq = {
    (lenfunc)		బఫరు_నిడివి,	    // sq_length,    len(x)
    (binaryfunc)	0,		    // sq_concat,    x+y
    (ssizeargfunc)	0,		    // sq_repeat,    x*n
    (ssizeargfunc)	బఫరు_అంశం,	    // sq_item,      x[i]
    0,					    // was_sq_slice,	 x[i:j]
    0,					    // sq_ass_item,  x[i]=v
    0,					    // sq_ass_slice, x[i:j]=v
    0,					    // sq_contains
    0,					    // sq_inplace_concat
    0,					    // sq_inplace_repeat
};

స్థిర PyMappingMethods బఫరు_As_Mapping = {
    /* mp_length	*/ (lenfunc)బఫరు_నిడివి,
    /* mp_subscript     */ (binaryfunc)బఫరు_Subscript,
    /* mp_ass_subscript */ (objobjargproc)బఫరు_As_Subscript,
};


// Buffer object

    స్థిర PyObject *
బఫరు_Getattro(PyObject *self, PyObject *nameobj)
{
    PyObject *r;

    GET_ATTR_STRING(name, nameobj);

    ఒకవేళ ((r = బఫరు_Attr_Valid((బఫరు_Object *)(self), name)))
	తిరిగి r;

    ఒకవేళ (Check_బఫరు((బఫరు_Object *)(self)))
	తిరిగి ఖాళీ;

    r = బఫరు_Attr((బఫరు_Object *)(self), name);
    ఒకవేళ (r || PyErr_Occurred())
	తిరిగి r;
    లేకపోతే
	తిరిగి PyObject_GenericGetAttr(self, nameobj);
}

    స్థిర పూర్ణాంకం
బఫరు_Setattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);

    తిరిగి బఫరు_Setattr(self, name, val);
}

//////////////////

    స్థిర PyObject *
బఫరు_Subscript(PyObject *self, PyObject* idx)
{
    ఒకవేళ (PyLong_Check(idx))
    {
	దీర్ఘ _idx = PyLong_AsLong(idx);
	తిరిగి బఫరు_అంశం((బఫరు_Object *)(self), _idx);
    }
    లేకపోతే ఒకవేళ (Py_ముక్క_Check(idx))
    {
	Py_ssize_t ప్రారంభం, stop, అడుగు, slicelen;

	ఒకవేళ (Check_బఫరు((బఫరు_Object *) self))
	    తిరిగి ఖాళీ;

	ఒకవేళ (Py_ముక్క_Get_Indices_Ex((Py_ముక్క_Object_T *)idx,
	      (Py_ssize_t)((బఫరు_Object *)(self))->buf->b_ml.ml_పంక్తి_లెక్క,
	      &ప్రారంభం, &stop,
	      &అడుగు, &slicelen) < 0)
	    తిరిగి ఖాళీ;
	తిరిగి బఫరు_ముక్క((బఫరు_Object *)(self), ప్రారంభం, stop);
    }
    లేకపోతే
    {
	RAISE_INVALID_సూచీ_TYPE(idx);
	తిరిగి ఖాళీ;
    }
}

    స్థిర పూర్ణాంకం
బఫరు_As_Subscript(PyObject *self, PyObject* idx, PyObject* val)
{
    ఒకవేళ (PyLong_Check(idx))
    {
	దీర్ఘ n = PyLong_AsLong(idx);

	ఒకవేళ (Check_బఫరు((బఫరు_Object *) self))
	    తిరిగి -1;

	తిరిగి RBAs_అంశం((బఫరు_Object *)(self), n, val, 1,
		    (Py_ssize_t)((బఫరు_Object *)(self))->buf->b_ml.ml_పంక్తి_లెక్క,
		    ఖాళీ);
    }
    లేకపోతే ఒకవేళ (Py_ముక్క_Check(idx))
    {
	Py_ssize_t ప్రారంభం, stop, అడుగు, slicelen;

	ఒకవేళ (Check_బఫరు((బఫరు_Object *) self))
	    తిరిగి -1;

	ఒకవేళ (Py_ముక్క_Get_Indices_Ex((Py_ముక్క_Object_T *)idx,
	      (Py_ssize_t)((బఫరు_Object *)(self))->buf->b_ml.ml_పంక్తి_లెక్క,
	      &ప్రారంభం, &stop,
	      &అడుగు, &slicelen) < 0)
	    తిరిగి -1;
	తిరిగి RBAs_ముక్క((బఫరు_Object *)(self), ప్రారంభం, stop, val, 1,
			  (PyInt)((బఫరు_Object *)(self))->buf->b_ml.ml_పంక్తి_లెక్క,
			  ఖాళీ);
    }
    లేకపోతే
    {
	RAISE_INVALID_సూచీ_TYPE(idx);
	తిరిగి -1;
    }
}

స్థిర PySequenceMethods RangeAsSeq = {
    (lenfunc)		Range_నిడివి,	 // sq_length,	  len(x)
    (binaryfunc)	0,		 // RangeConcat, sq_concat,  x+y
    (ssizeargfunc)	0,		 // RangeRepeat, sq_repeat,  x*n
    (ssizeargfunc)	Range_అంశం,	 // sq_item,	  x[i]
    0,					 // was_sq_slice,     x[i:j]
    (ssizeobjargproc)	Range_As_అంశం,	 // sq_as_item,  x[i]=v
    0,					 // sq_ass_slice, x[i:j]=v
    0,					 // sq_contains
    0,					 // sq_inplace_concat
    0,					 // sq_inplace_repeat
};

స్థిర PyMappingMethods RangeAsMapping = {
    /* mp_length	*/ (lenfunc)Range_నిడివి,
    /* mp_subscript     */ (binaryfunc)RangeSubscript,
    /* mp_ass_subscript */ (objobjargproc)RangeAsSubscript,
};

// Line range object - Implementation

    స్థిర PyObject *
RangeGetattro(PyObject *self, PyObject *nameobj)
{
    GET_ATTR_STRING(name, nameobj);

    ఒకవేళ (strcmp(name, "start") == 0)
	తిరిగి Py_Build_విలువ("n", ((RangeObject *)(self))->ప్రారంభం - 1);
    లేకపోతే ఒకవేళ (strcmp(name, "end") == 0)
	తిరిగి Py_Build_విలువ("n", ((RangeObject *)(self))->చివర - 1);
    లేకపోతే
	తిరిగి PyObject_GenericGetAttr(self, nameobj);
}

////////////////

    స్థిర పూర్ణాంకం
Range_As_అంశం(PyObject *self, Py_ssize_t n, PyObject *val)
{
    తిరిగి RBAs_అంశం(((RangeObject *)(self))->buf, n, val,
		    ((RangeObject *)(self))->ప్రారంభం,
		    ((RangeObject *)(self))->చివర,
		    &((RangeObject *)(self))->చివర);
}

    స్థిర Py_ssize_t
Range_As_ముక్క(PyObject *self, Py_ssize_t lo, Py_ssize_t hi, PyObject *val)
{
    తిరిగి RBAs_ముక్క(((RangeObject *)(self))->buf, lo, hi, val,
		    ((RangeObject *)(self))->ప్రారంభం,
		    ((RangeObject *)(self))->చివర,
		    &((RangeObject *)(self))->చివర);
}

    స్థిర PyObject *
RangeSubscript(PyObject *self, PyObject* idx)
{
    ఒకవేళ (PyLong_Check(idx))
    {
	దీర్ఘ _idx = PyLong_AsLong(idx);
	తిరిగి Range_అంశం((RangeObject *)(self), _idx);
    }
    లేకపోతే ఒకవేళ (Py_ముక్క_Check(idx))
    {
	Py_ssize_t ప్రారంభం, stop, అడుగు, slicelen;

	ఒకవేళ (Py_ముక్క_Get_Indices_Ex((Py_ముక్క_Object_T *)idx,
		((RangeObject *)(self))->చివర-((RangeObject *)(self))->ప్రారంభం+1,
		&ప్రారంభం, &stop,
		&అడుగు, &slicelen) < 0)
	    తిరిగి ఖాళీ;
	తిరిగి Range_ముక్క((RangeObject *)(self), ప్రారంభం, stop);
    }
    లేకపోతే
    {
	RAISE_INVALID_సూచీ_TYPE(idx);
	తిరిగి ఖాళీ;
    }
}

    స్థిర పూర్ణాంకం
RangeAsSubscript(PyObject *self, PyObject *idx, PyObject *val)
{
    ఒకవేళ (PyLong_Check(idx))
    {
	దీర్ఘ n = PyLong_AsLong(idx);
	తిరిగి Range_As_అంశం(self, n, val);
    }
    లేకపోతే ఒకవేళ (Py_ముక్క_Check(idx))
    {
	Py_ssize_t ప్రారంభం, stop, అడుగు, slicelen;

	ఒకవేళ (Py_ముక్క_Get_Indices_Ex((Py_ముక్క_Object_T *)idx,
		((RangeObject *)(self))->చివర-((RangeObject *)(self))->ప్రారంభం+1,
		&ప్రారంభం, &stop,
		&అడుగు, &slicelen) < 0)
	    తిరిగి -1;
	తిరిగి Range_As_ముక్క(self, ప్రారంభం, stop, val);
    }
    లేకపోతే
    {
	RAISE_INVALID_సూచీ_TYPE(idx);
	తిరిగి -1;
    }
}

// TabPage object - Implementation

    స్థిర PyObject *
Tab_పుట_Getattro(PyObject *self, PyObject *nameobj)
{
    PyObject *r;

    GET_ATTR_STRING(name, nameobj);

    ఒకవేళ ((r = Tab_పుట_Attr_Valid((Tab_పుట_Object *)(self), name)))
	తిరిగి r;

    ఒకవేళ (Check_Tab_పుట((Tab_పుట_Object *)(self)))
	తిరిగి ఖాళీ;

    r = Tab_పుట_Attr((Tab_పుట_Object *)(self), name);
    ఒకవేళ (r || PyErr_Occurred())
	తిరిగి r;
    లేకపోతే
	తిరిగి PyObject_GenericGetAttr(self, nameobj);
}

// Window object - Implementation

    స్థిర PyObject *
కిటికీ_Getattro(PyObject *self, PyObject *nameobj)
{
    PyObject *r;

    GET_ATTR_STRING(name, nameobj);

    ఒకవేళ ((r = కిటికీ_Attr_Valid((కిటికీ_Object *)(self), name)))
	తిరిగి r;

    ఒకవేళ (Check_కిటికీ((కిటికీ_Object *)(self)))
	తిరిగి ఖాళీ;

    r = కిటికీ_Attr((కిటికీ_Object *)(self), name);
    ఒకవేళ (r || PyErr_Occurred())
	తిరిగి r;
    లేకపోతే
	తిరిగి PyObject_GenericGetAttr(self, nameobj);
}

    స్థిర పూర్ణాంకం
కిటికీ_Setattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);

    తిరిగి కిటికీ_Setattr(self, name, val);
}

// Tab page list object - Definitions

స్థిర PySequenceMethods TabListAsSeq = {
    (lenfunc)	     Tab_List_నిడివి,	    // sq_length,    len(x)
    (binaryfunc)     0,			    // sq_concat,    x+y
    (ssizeargfunc)   0,			    // sq_repeat,    x*n
    (ssizeargfunc)   Tab_List_అంశం,	    // sq_item,      x[i]
    0,					    // sq_slice,     x[i:j]
    (ssizeobjargproc)0,			    // sq_as_item,  x[i]=v
    0,					    // sq_ass_slice, x[i:j]=v
    0,					    // sq_contains
    0,					    // sq_inplace_concat
    0,					    // sq_inplace_repeat
};

// Window list object - Definitions

స్థిర PySequenceMethods WinListAsSeq = {
    (lenfunc)	     Win_List_నిడివి,	    // sq_length,    len(x)
    (binaryfunc)     0,			    // sq_concat,    x+y
    (ssizeargfunc)   0,			    // sq_repeat,    x*n
    (ssizeargfunc)   Win_List_అంశం,	    // sq_item,      x[i]
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
    స్థిర PyObject *
CurrentGetattro(PyObject *self, PyObject *nameobj)
{
    PyObject	*r;
    GET_ATTR_STRING(name, nameobj);
    ఒకవేళ (!(r = CurrentGetattr(self, name)))
	తిరిగి PyObject_GenericGetAttr(self, nameobj);
    తిరిగి r;
}

    స్థిర పూర్ణాంకం
CurrentSetattro(PyObject *self, PyObject *nameobj, PyObject *విలువ)
{
    GET_ATTR_STRING(name, nameobj);
    తిరిగి CurrentSetattr(self, name, విలువ);
}

// Dictionary object - Definitions

    స్థిర PyObject *
DictionaryGetattro(PyObject *self, PyObject *nameobj)
{
    DictionaryObject	*this = ((DictionaryObject *) (self));

    GET_ATTR_STRING(name, nameobj);

    ఒకవేళ (strcmp(name, "locked") == 0)
	తిరిగి PyLong_FromLong(this->dict->dv_తాళం);
    లేకపోతే ఒకవేళ (strcmp(name, "scope") == 0)
	తిరిగి PyLong_FromLong(this->dict->dv_scope);

    తిరిగి PyObject_GenericGetAttr(self, nameobj);
}

    స్థిర పూర్ణాంకం
DictionarySetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);
    తిరిగి DictionarySetattr(self, name, val);
}

// List object - Definitions

    స్థిర PyObject *
ListGetattro(PyObject *self, PyObject *nameobj)
{
    GET_ATTR_STRING(name, nameobj);

    ఒకవేళ (strcmp(name, "locked") == 0)
	తిరిగి PyLong_FromLong(((ListObject *) (self))->list->lv_తాళం);

    తిరిగి PyObject_GenericGetAttr(self, nameobj);
}

    స్థిర పూర్ణాంకం
ListSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);
    తిరిగి ListSetattr(self, name, val);
}

// Tuple object - Definitions

    స్థిర PyObject *
TupleGetattro(PyObject *self, PyObject *nameobj)
{
    GET_ATTR_STRING(name, nameobj);

    ఒకవేళ (strcmp(name, "locked") == 0)
	తిరిగి PyLong_FromLong(((TupleObject *) (self))->tuple->tv_తాళం);

    తిరిగి PyObject_GenericGetAttr(self, nameobj);
}

    స్థిర పూర్ణాంకం
TupleSetattro(PyObject *self, PyObject *nameobj, PyObject *val)
{
    GET_ATTR_STRING(name, nameobj);
    తిరిగి TupleSetattr(self, name, val);
}

// Function object - Definitions

    స్థిర PyObject *
FunctionGetattro(PyObject *self, PyObject *nameobj)
{
    PyObject		*r;
    FunctionObject	*this = (FunctionObject *)(self);

    GET_ATTR_STRING(name, nameobj);

    r = FunctionAttr(this, name);
    ఒకవేళ (r || PyErr_Occurred())
	తిరిగి r;
    లేకపోతే
	తిరిగి PyObject_GenericGetAttr(self, nameobj);
}

// External interface

    శూన్య
python3_బఫరు_free(buf_T *buf)
{
    బఫరు_Object *bp = BUF_PYTHON_REF(buf);
    ఒకవేళ (bp == ఖాళీ)
	తిరిగి;
    bp->buf = INVALID_బఫరు_విలువ;
    BUF_PYTHON_REF(buf) = ఖాళీ;
}

    శూన్య
python3_కిటికీ_free(win_T *win)
{
    కిటికీ_Object *wp = WIN_PYTHON_REF(win);
    ఒకవేళ (wp == ఖాళీ)
	తిరిగి;
    wp->win = INVALID_కిటికీ_విలువ;
    WIN_PYTHON_REF(win) = ఖాళీ;
}

    శూన్య
python3_tabpage_free(tabpage_T *tab)
{
    Tab_పుట_Object *tp = TAB_PYTHON_REF(tab);
    ఒకవేళ (tp == ఖాళీ)
	తిరిగి;
    tp->tab = INVALID_TABPAGE_విలువ;
    TAB_PYTHON_REF(tab) = ఖాళీ;
}

    స్థిర PyObject *
Py3Init_vim(శూన్య)
{
    // The special value is removed from sys.path in Python3_Init().
    స్థిర wchar_t *(argv[2]) = {L"/must>not&exist/foo", ఖాళీ};

    ఒకవేళ (init_types())
	తిరిగి ఖాళీ;

    // Set sys.argv[] to avoid a crash in warn().
    Py_Sys_సమితి_Argv(1, argv);

    ఒకవేళ ((vim_module = PyModule_Create(&vimmodule)) == ఖాళీ)
	తిరిగి ఖాళీ;

    ఒకవేళ (populate_module(vim_module))
	తిరిగి ఖాళీ;

    ఒకవేళ (init_sys_మార్గం())
	తిరిగి ఖాళీ;

    తిరిగి vim_module;
}

//////////////////////////////////////////////////////////////////////////
// 4. Utility functions for handling the interface between Vim and Python.

/*
 * Convert a Vim line into a Python string.
 * All internal newlines are replaced by null characters.
 *
 * On errors, the Python exception data is set, and NULL is returned.
 */
    స్థిర PyObject *
పంక్తి_గమ్యం_String(స్థిరరాశి అక్షరం *str)
{
    PyObject *ఫలితం;
    Py_ssize_t len = అక్షరశ్రేణి_పొడవు(str);
    అక్షరం *tmp, *p;

    tmp = alloc(len + 1);
    p = tmp;
    ఒకవేళ (p == ఖాళీ)
    {
	Py_Err_No_జ్ఞాపకం();
	తిరిగి ఖాళీ;
    }

    అయితే (*str)
    {
	ఒకవేళ (*str == '\n')
	    *p = '\0';
	లేకపోతే
	    *p = *str;

	++p;
	++str;
    }
    *p = '\0';

    ఫలితం = PyUnicode_Decode(tmp, len, (అక్షరం *)ENC_OPT, ERRORS_DECODE_ARG);

    vim_free(tmp);
    తిరిగి ఫలితం;
}

    శూన్య
do_py3eval(char_u *str, dict_T *locals, typval_T *rettv)
{
    DoPyCommand((అక్షరం *) str,
	    locals,
	    init_range_eval,
	    (runner) నిరంతరం_eval,
	    (శూన్య *) rettv);
    ఒకవేళ (rettv->v_type == VAR_UNKNOWN)
    {
	rettv->v_type = VAR_సంఖ్య;
	rettv->vval.v_సంఖ్య = 0;
    }
}

    పూర్ణాంకం
సమితి_ref_లోపల_python3(పూర్ణాంకం copyID)
{
    తిరిగి సమితి_ref_లోపల_py(copyID);
}

    పూర్ణాంకం
python3_సంస్కరణ(శూన్య)
{
#నిర్వచితమైతే USE_LIMITED_API
    తిరిగి Py_LIMITED_API;
#లేకపోతే
    తిరిగి PY_సంస్కరణ_HEX;
#అయితే_ముగింపు
}
