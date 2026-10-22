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

#అన్తర్భావ "vim.h"

#అన్తర్భావ <limits.h>

// uncomment this if used with the debug version of python.
// Checked on 2.7.4.
// #define Py_DEBUG
// Note: most of time you can add -DPy_DEBUG to CFLAGS in place of uncommenting
// uncomment this if used with the debug version of python, but without its
// allocator
// #define Py_DEBUG_NO_PYMALLOC

// Python.h defines _POSIX_THREADS itself (if needed)
#నిర్వచితమైతే _POSIX_THREADS
# undef _POSIX_THREADS
#అయితే_ముగింపు

#ఒకవేళ defined(MSWIN) && defined(HAVE_FCNTL_H)
# undef HAVE_FCNTL_H
#అయితే_ముగింపు

#నిర్వచితమైతే _DEBUG
# undef _DEBUG
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
#నిర్వచితమైతే _POSIX_C_SOURCE
# undef _POSIX_C_SOURCE	// pyconfig.h defines it as well.
#అయితే_ముగింపు
#నిర్వచితమైతే XOPEN_ఆధారం
# undef XOPEN_ఆధారం	// pyconfig.h defines it as well.
#అయితే_ముగింపు

#నిర్వచించు PY_SSIZE_T_CLEAN

#అన్తర్భావ <Python.h>

#ఒకవేళ !defined(PY_సంస్కరణ_HEX) || PY_సంస్కరణ_HEX < 0x02050000
# undef PY_SSIZE_T_CLEAN
#అయితే_ముగింపు

// these are NULL for Python 2
#నిర్వచించు ERRORS_DECODE_ARG ఖాళీ
#నిర్వచించు ERRORS_ENCODE_ARG ERRORS_DECODE_ARG

#నిర్వచనం_రద్దు ప్రధాన // Defined in python.h - aargh
#నిర్వచనం_రద్దు HAVE_FCNTL_H // Clash with os_win32.h

// Perhaps leave this out for Python 2.6, which supports bytes?
#నిర్వచించు Py_బైట్లు_From_String      PyString_FromString
#నిర్వచించు Py_బైట్లు_Check		PyString_Check
#నిర్వచించు Py_బైట్లు_As_String_And_కొలత Py_String_As_String_And_కొలత
#నిర్వచించు Py_బైట్లు_From_String_And_కొలత   Py_String_From_String_And_కొలత

#ఒకవేళ defined(PY_సంస్కరణ_HEX) && PY_సంస్కరణ_HEX >= 0x02070000
# define PY_USE_CAPSULE
#అయితే_ముగింపు

#ఒకవేళ defined(PY_సంస్కరణ_HEX) && PY_సంస్కరణ_HEX >= 0x02050000
# define PyInt Py_ssize_t
# define PyInquiry lenfunc
# define PyIntArgFunc ssizeargfunc
# define PyIntIntArgFunc ssizessizeargfunc
# define PyIntObjArgProc ssizeobjargproc
# define PyIntIntObjArgProc ssizessizeobjargproc
# define Py_ssize_t_fmt "n"
#లేకపోతే
# define PyInt పూర్ణాంకం
# define lenfunc inquiry
# define PyInquiry inquiry
# define PyIntArgFunc intargfunc
# define PyIntIntArgFunc intintargfunc
# define PyIntObjArgProc intobjargproc
# define PyIntIntObjArgProc intintobjargproc
# define Py_ssize_t_fmt "i"
#అయితే_ముగింపు
#నిర్వచించు Py_బైట్లు_fmt "s"

// Parser flags
#నిర్వచించు single_ఆదానం	256
#నిర్వచించు file_ఆదానం	257
#నిర్వచించు eval_ఆదానం	258

#ఒకవేళ defined(PY_సంస్కరణ_HEX) && PY_సంస్కరణ_HEX >= 0x020300F0
  // Python 2.3: can invoke ":python" recursively.
# define PY_CAN_RECURSE
#అయితే_ముగింపు

#ఒకవేళ defined(DYNAMIC_PYTHON)
# ifndef DYNAMIC_PYTHON
#  define HINSTANCE long_u		// for generating prototypes
# endif

# ifdef MSWIN
#  define ఎక్కించు_dll vim_ఎక్కించు_Lib
#  define close_dll FreeLibrary
#  define చిహ్నం_from_dll Get_Proc_చిరునామా
#  define ఎక్కించు_dll_error GetWin32Error
# లేకపోతే
#  include <dlfcn.h>
#  define FARPROC శూన్య*
#  define HINSTANCE శూన్య*
#  ఒకవేళ defined(PY_NO_RTLD_GLOBAL) && defined(PY3_NO_RTLD_GLOBAL)
#   define ఎక్కించు_dll(n) dlopen((n), RTLD_LAZY)
#  లేకపోతే
#   define ఎక్కించు_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#  endif
#  define close_dll dlclose
#  define చిహ్నం_from_dll dlsym
#  define ఎక్కించు_dll_error dlerror
# endif

// This makes if_python.c compile without warnings against Python 2.5
// on Win32 and Win64.
# undef Py_నిరంతరం_Simple_String
# undef Py_నిరంతరం_String
# undef PyArg_Parse
# undef PyArg_ParseTuple
# undef Py_Build_విలువ
# undef Py_InitModule4
# undef Py_InitModule4_64
# undef Py_Object_Call_పద్ధతి
# undef PyObject_CallFunction

/*
 * Wrapper defines
 */
# define PyArg_Parse dll_PyArg_Parse
# define PyArg_ParseTuple dll_PyArg_ParseTuple
# define PyMem_Free dll_PyMem_Free
# define PyMem_Malloc dll_PyMem_Malloc
# define Py_Dict_సమితి_అంశం_String dll_Py_Dict_సమితి_అంశం_String
# define Py_Err_Bad_వాదం dll_Py_Err_Bad_వాదం
# define PyErr_NewException dll_PyErr_NewException
# define PyErr_Clear dll_PyErr_Clear
# define PyErr_Format dll_PyErr_Format
# define PyErr_PrintEx dll_PyErr_PrintEx
# define Py_Err_No_జ్ఞాపకం dll_Py_Err_No_జ్ఞాపకం
# define PyErr_Occurred dll_PyErr_Occurred
# define Py_Err_సమితి_None dll_Py_Err_సమితి_None
# define Py_Err_సమితి_String dll_Py_Err_సమితి_String
# define Py_Err_సమితి_Object dll_Py_Err_సమితి_Object
# define PyErr_ExceptionMatches dll_PyErr_ExceptionMatches
# define Py_Eval_Init_దారాలు dll_Py_Eval_Init_దారాలు
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
# define Py_List_Get_అంశం dll_Py_List_Get_అంశం
# define PyList_Append dll_PyList_Append
# define Py_List_చొప్పించు dll_Py_List_చొప్పించు
# define PyList_New dll_PyList_New
# define Py_List_సమితి_అంశం_2 dll_Py_List_సమితి_అంశం
# define Py_List_కొలత dll_Py_List_కొలత
# define PyList_Type (*dll_PyList_Type)
# define Py_Tuple_Get_అంశం dll_Py_Tuple_Get_అంశం
# define Py_Tuple_సమితి_అంశం dll_Py_Tuple_సమితి_అంశం
# define PyTuple_New dll_PyTuple_New
# define Py_Tuple_కొలత dll_Py_Tuple_కొలత
# define PyTuple_Type (*dll_PyTuple_Type)
# define PySequence_Check dll_PySequence_Check
# define Py_Sequence_కొలత dll_Py_Sequence_కొలత
# define Py_Sequence_Get_అంశం dll_Py_Sequence_Get_అంశం
# define PySequence_Fast dll_PySequence_Fast
# define Py_ముక్క_Get_Indices_Ex dll_Py_ముక్క_Get_Indices_Ex
# define PyImport_ImportModule dll_PyImport_ImportModule
# define PyDict_New dll_PyDict_New
# define Py_Dict_Get_అంశం_String dll_Py_Dict_Get_అంశం_String
# define Py_Dict_తదుపరి dll_Py_Dict_తదుపరి
# define PyDict_Type (*dll_PyDict_Type)
# ifdef Py_Mapping_కీలకాలు
#  define PY_NO_MAPPING_కీలకాలు
# లేకపోతే
#  define Py_Mapping_కీలకాలు dll_Py_Mapping_కీలకాలు
# endif
# define Py_Object_Get_అంశం dll_Py_Object_Get_అంశం
# define Py_Object_Call_పద్ధతి dll_Py_Object_Call_పద్ధతి
# define PyMapping_Check dll_PyMapping_Check
# define Py_Iter_తదుపరి dll_Py_Iter_తదుపరి
# define PyModule_GetDict dll_PyModule_GetDict
# define Py_Module_కూడు_Object dll_Py_Module_కూడు_Object
# define Py_నిరంతరం_Simple_String dll_Py_నిరంతరం_Simple_String
# define Py_నిరంతరం_String dll_Py_నిరంతరం_String
# define PyObject_GetAttrString dll_PyObject_GetAttrString
# define PyObject_HasAttrString dll_PyObject_HasAttrString
# define Py_Object_సమితి_Attr_String dll_Py_Object_సమితి_Attr_String
# define Py_Object_Call_Function_Obj_వాదనలు dll_Py_Object_Call_Function_Obj_వాదనలు
# define PyObject_CallFunction dll_PyObject_CallFunction
# define PyObject_Call dll_PyObject_Call
# define PyObject_Repr dll_PyObject_Repr
# define PyString_AsString dll_PyString_AsString
# define Py_String_As_String_And_కొలత dll_Py_String_As_String_And_కొలత
# define PyString_FromString dll_PyString_FromString
# define PyString_FromFormat dll_PyString_FromFormat
# define Py_String_From_String_And_కొలత dll_Py_String_From_String_And_కొలత
# define Py_String_కొలత dll_Py_String_కొలత
# define PyString_Type (*dll_PyString_Type)
# define PyUnicode_Type (*dll_PyUnicode_Type)
# undef PyUnicode_AsEncodedString
# define PyUnicode_AsEncodedString py_PyUnicode_AsEncodedString
# define PyFloat_AsDouble dll_PyFloat_AsDouble
# define PyFloat_FromDouble dll_PyFloat_FromDouble
# define PyFloat_Type (*dll_PyFloat_Type)
# define Py_సంఖ్య_Check dll_Py_సంఖ్య_Check
# define Py_సంఖ్య_Long dll_Py_సంఖ్య_Long
# define Py_Import_కూడు_Module (*dll_Py_Import_కూడు_Module)
# define Py_Sys_సమితి_Object dll_Py_Sys_సమితి_Object
# define PySys_GetObject dll_PySys_GetObject
# define Py_Sys_సమితి_Argv dll_Py_Sys_సమితి_Argv
# define PyType_Type (*dll_PyType_Type)
# define PyFile_Type (*dll_PyFile_Type)
# define Py_ముక్క_Type (*dll_Py_ముక్క_Type)
# define PyType_Ready (*dll_PyType_Ready)
# define PyType_GenericAlloc dll_PyType_GenericAlloc
# define Py_Build_విలువ dll_Py_Build_విలువ
# define Py_కనుగొను_పద్ధతి dll_Py_కనుగొను_పద్ధతి
# define Py_InitModule4 dll_Py_InitModule4
# define Py_సమితి_Python_Home dll_Py_సమితి_Python_Home
# define Py_Initialize dll_Py_Initialize
# define Py_Finalize dll_Py_Finalize
# define Py_IsInitialized dll_Py_IsInitialized
# define _PyObject_New dll__PyObject_New
# define _PyObject_GC_New dll__PyObject_GC_New
# ifdef PyObject_GC_Del
#  define Py_underscore_GC
#  define _PyObject_GC_Del dll__PyObject_GC_Del
#  define _PyObject_GC_UnTrack dll__PyObject_GC_UnTrack
# లేకపోతే
#  define PyObject_GC_Del dll_PyObject_GC_Del
#  define PyObject_GC_UnTrack dll_PyObject_GC_UnTrack
# endif
# ఒకవేళ defined(PY_సంస్కరణ_HEX) && PY_సంస్కరణ_HEX >= 0x02070000
#  define Py_Object_తదుపరి_Not_Implemented (*dll_Py_Object_తదుపరి_Not_Implemented)
# endif
# define _Py_NoneStruct (*dll__Py_NoneStruct)
# define _Py_ZeroStruct (*dll__Py_ZeroStruct)
# define _Py_TrueStruct (*dll__Py_TrueStruct)
# define PyObject_Init dll__PyObject_Init
# define PyObject_GetIter dll_PyObject_GetIter
# define PyObject_IsTrue dll_PyObject_IsTrue
# ఒకవేళ defined(PY_సంస్కరణ_HEX) && PY_సంస్కరణ_HEX >= 0x02020000
#  define PyType_IsSubtype dll_PyType_IsSubtype
#  ifdef Py_DEBUG
#   define _Py_NegativeRefcount dll__Py_NegativeRefcount
#   define Py_Ref_మొత్తం (*dll_Py_Ref_మొత్తం)
#   define _Py_Dealloc dll__Py_Dealloc
#  endif
# endif
# ఒకవేళ defined(PY_సంస్కరణ_HEX) && PY_సంస్కరణ_HEX >= 0x02030000
#  ఒకవేళ defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
#   define _PyObject_DebugMalloc dll__PyObject_DebugMalloc
#   define _PyObject_DebugFree dll__PyObject_DebugFree
#  లేకపోతే
#   define PyObject_Malloc dll_PyObject_Malloc
#   define PyObject_Free dll_PyObject_Free
#  endif
# endif
# ifdef PY_USE_CAPSULE
#  define PyCapsule_New dll_PyCapsule_New
#  define Py_Capsule_Get_సూచిక dll_Py_Capsule_Get_సూచిక
# లేకపోతే
#  define PyCObject_FromVoidPtr dll_PyCObject_FromVoidPtr
#  define PyCObject_AsVoidPtr dll_PyCObject_AsVoidPtr
# endif
# ఒకవేళ defined(PY_సంస్కరణ_HEX) && PY_సంస్కరణ_HEX >= 0x02070000
#  define Py_NoSiteFlag (*dll_Py_NoSiteFlag)
# endif

/*
 * Pointers for dynamic link
 */
స్థిర పూర్ణాంకం(*dll_PyArg_Parse)(PyObject *, అక్షరం *, ...);
స్థిర పూర్ణాంకం(*dll_PyArg_ParseTuple)(PyObject *, అక్షరం *, ...);
స్థిర శూన్య(*dll_PyMem_Free)(శూన్య *);
స్థిర శూన్య* (*dll_PyMem_Malloc)(size_t);
స్థిర పూర్ణాంకం(*dll_Py_Dict_సమితి_అంశం_String)(PyObject *dp, అక్షరం *కీలకం, PyObject *అంశం);
స్థిర పూర్ణాంకం(*dll_Py_Err_Bad_వాదం)(శూన్య);
స్థిర PyObject *(*dll_PyErr_NewException)(అక్షరం *, PyObject *, PyObject *);
స్థిర శూన్య(*dll_PyErr_Clear)(శూన్య);
స్థిర PyObject*(*dll_PyErr_Format)(PyObject *, స్థిరరాశి అక్షరం *, ...);
స్థిర శూన్య(*dll_PyErr_PrintEx)(పూర్ణాంకం);
స్థిర PyObject*(*dll_Py_Err_No_జ్ఞాపకం)(శూన్య);
స్థిర PyObject*(*dll_PyErr_Occurred)(శూన్య);
స్థిర శూన్య(*dll_Py_Err_సమితి_None)(PyObject *);
స్థిర శూన్య(*dll_Py_Err_సమితి_String)(PyObject *, స్థిరరాశి అక్షరం *);
స్థిర శూన్య(*dll_Py_Err_సమితి_Object)(PyObject *, PyObject *);
స్థిర పూర్ణాంకం(*dll_PyErr_ExceptionMatches)(PyObject *);
స్థిర శూన్య(*dll_Py_Eval_Init_దారాలు)(శూన్య);
స్థిర శూన్య(*dll_PyEval_RestoreThread)(PyThreadState *);
స్థిర PyThreadState*(*dll_PyEval_SaveThread)(శూన్య);
# ifdef PY_CAN_RECURSE
స్థిర PyGILState_STATE	(*dll_PyGILState_Ensure)(శూన్య);
స్థిర శూన్య (*dll_PyGILState_Release)(PyGILState_STATE);
# endif
స్థిర దీర్ఘ(*dll_PyInt_AsLong)(PyObject *);
స్థిర PyObject*(*dll_PyInt_FromLong)(దీర్ఘ);
స్థిర దీర్ఘ(*dll_PyLong_AsLong)(PyObject *);
స్థిర PyObject*(*dll_PyLong_FromLong)(దీర్ఘ);
స్థిర PyTypeObject* dll_PyBool_Type;
స్థిర PyTypeObject* dll_PyInt_Type;
స్థిర PyTypeObject* dll_PyLong_Type;
స్థిర PyObject*(*dll_Py_List_Get_అంశం)(PyObject *, PyInt);
స్థిర పూర్ణాంకం(*dll_PyList_Append)(PyObject *, PyObject *);
స్థిర పూర్ణాంకం(*dll_Py_List_చొప్పించు)(PyObject *, PyInt, PyObject *);
స్థిర PyObject*(*dll_PyList_New)(PyInt కొలత);
స్థిర పూర్ణాంకం(*dll_Py_List_సమితి_అంశం)(PyObject *, PyInt, PyObject *);
స్థిర PyInt(*dll_Py_List_కొలత)(PyObject *);
స్థిర PyTypeObject* dll_PyList_Type;
స్థిర PyObject*(*dll_Py_Tuple_Get_అంశం)(PyObject *, PyInt);
స్థిర పూర్ణాంకం(*dll_Py_Tuple_సమితి_అంశం)(PyObject *, PyInt, PyObject *);
స్థిర PyObject*(*dll_PyTuple_New)(PyInt కొలత);
స్థిర PyInt(*dll_Py_Tuple_కొలత)(PyObject *);
స్థిర PyTypeObject* dll_PyTuple_Type;
స్థిర పూర్ణాంకం (*dll_PySequence_Check)(PyObject *);
స్థిర PyInt(*dll_Py_Sequence_కొలత)(PyObject *);
స్థిర PyObject*(*dll_Py_Sequence_Get_అంశం)(PyObject *, PyInt);
స్థిర PyObject*(*dll_PySequence_Fast)(PyObject *, స్థిరరాశి అక్షరం *);
స్థిర పూర్ణాంకం (*dll_Py_ముక్క_Get_Indices_Ex)(Py_ముక్క_Object *r, PyInt నిడివి,
		     PyInt *ప్రారంభం, PyInt *stop, PyInt *అడుగు,
		     PyInt *slicelen);
స్థిర PyObject*(*dll_PyImport_ImportModule)(స్థిరరాశి అక్షరం *);
స్థిర PyObject*(*dll_PyDict_New)(శూన్య);
స్థిర PyObject*(*dll_Py_Dict_Get_అంశం_String)(PyObject *, స్థిరరాశి అక్షరం *);
స్థిర పూర్ణాంకం (*dll_Py_Dict_తదుపరి)(PyObject *, PyInt *, PyObject **, PyObject **);
స్థిర PyTypeObject* dll_PyDict_Type;
# ifndef PY_NO_MAPPING_కీలకాలు
స్థిర PyObject* (*dll_Py_Mapping_కీలకాలు)(PyObject *);
# endif
స్థిర PyObject* (*dll_Py_Object_Get_అంశం)(PyObject *, PyObject *);
స్థిర PyObject* (*dll_Py_Object_Call_పద్ధతి)(PyObject *, అక్షరం *, PyObject *);
స్థిర పూర్ణాంకం (*dll_PyMapping_Check)(PyObject *);
స్థిర PyObject* (*dll_Py_Iter_తదుపరి)(PyObject *);
స్థిర PyObject*(*dll_PyModule_GetDict)(PyObject *);
స్థిర పూర్ణాంకం(*dll_Py_Module_కూడు_Object)(PyObject *, స్థిరరాశి అక్షరం *, PyObject *);
స్థిర పూర్ణాంకం(*dll_Py_నిరంతరం_Simple_String)(అక్షరం *);
స్థిర PyObject *(*dll_Py_నిరంతరం_String)(అక్షరం *, పూర్ణాంకం, PyObject *, PyObject *);
స్థిర PyObject* (*dll_PyObject_GetAttrString)(PyObject *, స్థిరరాశి అక్షరం *);
స్థిర పూర్ణాంకం (*dll_PyObject_HasAttrString)(PyObject *, స్థిరరాశి అక్షరం *);
స్థిర పూర్ణాంకం (*dll_Py_Object_సమితి_Attr_String)(PyObject *, స్థిరరాశి అక్షరం *, PyObject *);
స్థిర PyObject* (*dll_Py_Object_Call_Function_Obj_వాదనలు)(PyObject *, ...);
స్థిర PyObject* (*dll_PyObject_CallFunction)(PyObject *, అక్షరం *, ...);
స్థిర PyObject* (*dll_PyObject_Call)(PyObject *, PyObject *, PyObject *);
స్థిర PyObject* (*dll_PyObject_Repr)(PyObject *);
స్థిర అక్షరం*(*dll_PyString_AsString)(PyObject *);
స్థిర పూర్ణాంకం(*dll_Py_String_As_String_And_కొలత)(PyObject *, అక్షరం **, PyInt *);
స్థిర PyObject*(*dll_PyString_FromString)(స్థిరరాశి అక్షరం *);
స్థిర PyObject*(*dll_PyString_FromFormat)(స్థిరరాశి అక్షరం *, ...);
స్థిర PyObject*(*dll_Py_String_From_String_And_కొలత)(స్థిరరాశి అక్షరం *, PyInt);
స్థిర PyInt(*dll_Py_String_కొలత)(PyObject *);
స్థిర PyTypeObject* dll_PyString_Type;
స్థిర PyTypeObject* dll_PyUnicode_Type;
స్థిర PyObject *(*py_PyUnicode_AsEncodedString)(PyObject *, అక్షరం *, అక్షరం *);
స్థిర ద్విగుణ(*dll_PyFloat_AsDouble)(PyObject *);
స్థిర PyObject*(*dll_PyFloat_FromDouble)(ద్విగుణ);
స్థిర PyTypeObject* dll_PyFloat_Type;
స్థిర పూర్ణాంకం(*dll_Py_సంఖ్య_Check)(PyObject *);
స్థిర PyObject*(*dll_Py_సంఖ్య_Long)(PyObject *);
స్థిర పూర్ణాంకం(*dll_Py_Sys_సమితి_Object)(అక్షరం *, PyObject *);
స్థిర PyObject *(*dll_PySys_GetObject)(అక్షరం *);
స్థిర పూర్ణాంకం(*dll_Py_Sys_సమితి_Argv)(పూర్ణాంకం, అక్షరం **);
స్థిర PyTypeObject* dll_PyType_Type;
స్థిర PyTypeObject* dll_PyFile_Type;
స్థిర PyTypeObject* dll_Py_ముక్క_Type;
స్థిర పూర్ణాంకం (*dll_PyType_Ready)(PyTypeObject *type);
స్థిర PyObject* (*dll_PyType_GenericAlloc)(PyTypeObject *type, PyInt nitems);
స్థిర PyObject*(*dll_Py_Build_విలువ)(అక్షరం *, ...);
స్థిర PyObject*(*dll_Py_కనుగొను_పద్ధతి)(నిర్మాణం Py_పద్ధతి_Def[], PyObject *, అక్షరం *);
స్థిర PyObject*(*dll_Py_InitModule4)(అక్షరం *, నిర్మాణం Py_పద్ధతి_Def *, అక్షరం *, PyObject *, పూర్ణాంకం);
స్థిర PyObject*(*dll_Py_Import_కూడు_Module)(అక్షరం *);
స్థిర శూన్య(*dll_Py_సమితి_Python_Home)(అక్షరం *home);
స్థిర శూన్య(*dll_Py_Initialize)(శూన్య);
స్థిర శూన్య(*dll_Py_Finalize)(శూన్య);
స్థిర పూర్ణాంకం(*dll_Py_IsInitialized)(శూన్య);
స్థిర PyObject*(*dll__PyObject_New)(PyTypeObject *, PyObject *);
స్థిర PyObject*(*dll__PyObject_GC_New)(PyTypeObject *);
# ifdef Py_underscore_GC
స్థిర శూన్య(*dll__PyObject_GC_Del)(శూన్య *);
స్థిర శూన్య(*dll__PyObject_GC_UnTrack)(శూన్య *);
# లేకపోతే
స్థిర శూన్య(*dll_PyObject_GC_Del)(శూన్య *);
స్థిర శూన్య(*dll_PyObject_GC_UnTrack)(శూన్య *);
# endif
స్థిర PyObject*(*dll__PyObject_Init)(PyObject *, PyTypeObject *);
స్థిర PyObject* (*dll_PyObject_GetIter)(PyObject *);
స్థిర పూర్ణాంకం (*dll_PyObject_IsTrue)(PyObject *);
# ఒకవేళ defined(PY_సంస్కరణ_HEX) && PY_సంస్కరణ_HEX >= 0x02070000
స్థిర iternextfunc dll_Py_Object_తదుపరి_Not_Implemented;
# endif
స్థిర PyObject* dll__Py_NoneStruct;
స్థిర PyObject* _Py_ZeroStruct;
స్థిర PyObject* dll__Py_TrueStruct;
# ఒకవేళ defined(PY_సంస్కరణ_HEX) && PY_సంస్కరణ_HEX >= 0x02020000
స్థిర పూర్ణాంకం (*dll_PyType_IsSubtype)(PyTypeObject *, PyTypeObject *);
#  ifdef Py_DEBUG
స్థిర శూన్య (*dll__Py_NegativeRefcount)(స్థిరరాశి అక్షరం *fname, పూర్ణాంకం lineno, PyObject *op);
స్థిర PyInt* dll_Py_Ref_మొత్తం;
స్థిర శూన్య (*dll__Py_Dealloc)(PyObject *obj);
#  endif
# endif
# ఒకవేళ defined(PY_సంస్కరణ_HEX) && PY_సంస్కరణ_HEX >= 0x02030000
#  ఒకవేళ defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
స్థిర శూన్య (*dll__PyObject_DebugFree)(శూన్య*);
స్థిర శూన్య* (*dll__PyObject_DebugMalloc)(size_t);
#  లేకపోతే
స్థిర శూన్య* (*dll_PyObject_Malloc)(size_t);
స్థిర శూన్య (*dll_PyObject_Free)(శూన్య*);
#  endif
# endif
# ifdef PY_USE_CAPSULE
స్థిర PyObject* (*dll_PyCapsule_New)(శూన్య *, అక్షరం *, PyCapsule_Destructor);
స్థిర శూన్య* (*dll_Py_Capsule_Get_సూచిక)(PyObject *, అక్షరం *);
# లేకపోతే
స్థిర PyObject* (*dll_PyCObject_FromVoidPtr)(శూన్య *cobj, శూన్య (*destr)(శూన్య *));
స్థిర శూన్య* (*dll_PyCObject_AsVoidPtr)(PyObject *);
# endif
# ఒకవేళ defined(PY_సంస్కరణ_HEX) && PY_సంస్కరణ_HEX >= 0x02070000
స్థిర పూర్ణాంకం* dll_Py_NoSiteFlag;
# endif

స్థిర HINSTANCE hinstPython = 0; // Instance of python.dll

// Imported exception objects
స్థిర PyObject *imp_PyExc_AttributeError;
స్థిర PyObject *imp_Py_Exc_సూచీ_Error;
స్థిర PyObject *imp_Py_Exc_కీలకం_Error;
స్థిర PyObject *imp_Py_Exc_Keyboard_అంతరాయం;
స్థిర PyObject *imp_PyExc_TypeError;
స్థిర PyObject *imp_Py_Exc_విలువ_Error;
స్థిర PyObject *imp_PyExc_SystemExit;
స్థిర PyObject *imp_PyExc_RuntimeError;
స్థిర PyObject *imp_PyExc_ImportError;
స్థిర PyObject *imp_Py_Exc_అధికప్రవాహం_Error;

# define PyExc_AttributeError imp_PyExc_AttributeError
# define Py_Exc_సూచీ_Error imp_Py_Exc_సూచీ_Error
# define Py_Exc_కీలకం_Error imp_Py_Exc_కీలకం_Error
# define Py_Exc_Keyboard_అంతరాయం imp_Py_Exc_Keyboard_అంతరాయం
# define PyExc_TypeError imp_PyExc_TypeError
# define Py_Exc_విలువ_Error imp_Py_Exc_విలువ_Error
# define PyExc_SystemExit imp_PyExc_SystemExit
# define PyExc_RuntimeError imp_PyExc_RuntimeError
# define PyExc_ImportError imp_PyExc_ImportError
# define Py_Exc_అధికప్రవాహం_Error imp_Py_Exc_అధికప్రవాహం_Error

/*
 * Table of name to function pointer of python.
 */
# define PYTHON_PROC FARPROC
స్థిర నిర్మాణం
{
    అక్షరం *name;
    PYTHON_PROC *ptr;
} python_funcname_పట్టిక[] =
{
# ifdef PY_SSIZE_T_CLEAN
    {"_PyArg_Parse_SizeT", (PYTHON_PROC*)&dll_PyArg_Parse},
    {"_PyArg_ParseTuple_SizeT", (PYTHON_PROC*)&dll_PyArg_ParseTuple},
    {"_Py_BuildValue_SizeT", (PYTHON_PROC*)&dll_Py_Build_విలువ},
# లేకపోతే
    {"PyArg_Parse", (PYTHON_PROC*)&dll_PyArg_Parse},
    {"PyArg_ParseTuple", (PYTHON_PROC*)&dll_PyArg_ParseTuple},
    {"Py_BuildValue", (PYTHON_PROC*)&dll_Py_Build_విలువ},
# endif
    {"PyMem_Free", (PYTHON_PROC*)&dll_PyMem_Free},
    {"PyMem_Malloc", (PYTHON_PROC*)&dll_PyMem_Malloc},
    {"PyDict_SetItemString", (PYTHON_PROC*)&dll_Py_Dict_సమితి_అంశం_String},
    {"PyErr_BadArgument", (PYTHON_PROC*)&dll_Py_Err_Bad_వాదం},
    {"PyErr_NewException", (PYTHON_PROC*)&dll_PyErr_NewException},
    {"PyErr_Clear", (PYTHON_PROC*)&dll_PyErr_Clear},
    {"PyErr_Format", (PYTHON_PROC*)&dll_PyErr_Format},
    {"PyErr_PrintEx", (PYTHON_PROC*)&dll_PyErr_PrintEx},
    {"PyErr_NoMemory", (PYTHON_PROC*)&dll_Py_Err_No_జ్ఞాపకం},
    {"PyErr_Occurred", (PYTHON_PROC*)&dll_PyErr_Occurred},
    {"PyErr_SetNone", (PYTHON_PROC*)&dll_Py_Err_సమితి_None},
    {"PyErr_SetString", (PYTHON_PROC*)&dll_Py_Err_సమితి_String},
    {"PyErr_SetObject", (PYTHON_PROC*)&dll_Py_Err_సమితి_Object},
    {"PyErr_ExceptionMatches", (PYTHON_PROC*)&dll_PyErr_ExceptionMatches},
    {"PyEval_InitThreads", (PYTHON_PROC*)&dll_Py_Eval_Init_దారాలు},
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
    {"PyList_GetItem", (PYTHON_PROC*)&dll_Py_List_Get_అంశం},
    {"PyList_Append", (PYTHON_PROC*)&dll_PyList_Append},
    {"PyList_Insert", (PYTHON_PROC*)&dll_Py_List_చొప్పించు},
    {"PyList_New", (PYTHON_PROC*)&dll_PyList_New},
    {"PyList_SetItem", (PYTHON_PROC*)&dll_Py_List_సమితి_అంశం},
    {"PyList_Size", (PYTHON_PROC*)&dll_Py_List_కొలత},
    {"PyList_Type", (PYTHON_PROC*)&dll_PyList_Type},
    {"PyTuple_GetItem", (PYTHON_PROC*)&dll_Py_Tuple_Get_అంశం},
    {"PyTuple_SetItem", (PYTHON_PROC*)&dll_Py_Tuple_సమితి_అంశం},
    {"PyTuple_New", (PYTHON_PROC*)&dll_PyTuple_New},
    {"PyTuple_Size", (PYTHON_PROC*)&dll_Py_Tuple_కొలత},
    {"PyTuple_Type", (PYTHON_PROC*)&dll_PyTuple_Type},
    {"PySequence_Size", (PYTHON_PROC*)&dll_Py_Sequence_కొలత},
    {"PySequence_Check", (PYTHON_PROC*)&dll_PySequence_Check},
    {"PySequence_GetItem", (PYTHON_PROC*)&dll_Py_Sequence_Get_అంశం},
    {"PySequence_Fast", (PYTHON_PROC*)&dll_PySequence_Fast},
    {"PySlice_GetIndicesEx", (PYTHON_PROC*)&dll_Py_ముక్క_Get_Indices_Ex},
    {"PyImport_ImportModule", (PYTHON_PROC*)&dll_PyImport_ImportModule},
    {"PyDict_GetItemString", (PYTHON_PROC*)&dll_Py_Dict_Get_అంశం_String},
    {"PyDict_Next", (PYTHON_PROC*)&dll_Py_Dict_తదుపరి},
    {"PyDict_New", (PYTHON_PROC*)&dll_PyDict_New},
    {"PyDict_Type", (PYTHON_PROC*)&dll_PyDict_Type},
# ifndef PY_NO_MAPPING_కీలకాలు
    {"PyMapping_Keys", (PYTHON_PROC*)&dll_Py_Mapping_కీలకాలు},
# endif
    {"PyObject_GetItem", (PYTHON_PROC*)&dll_Py_Object_Get_అంశం},
    {"PyObject_CallMethod", (PYTHON_PROC*)&dll_Py_Object_Call_పద్ధతి},
    {"PyMapping_Check", (PYTHON_PROC*)&dll_PyMapping_Check},
    {"PyIter_Next", (PYTHON_PROC*)&dll_Py_Iter_తదుపరి},
    {"PyModule_GetDict", (PYTHON_PROC*)&dll_PyModule_GetDict},
    {"PyModule_AddObject", (PYTHON_PROC*)&dll_Py_Module_కూడు_Object},
    {"PyRun_SimpleString", (PYTHON_PROC*)&dll_Py_నిరంతరం_Simple_String},
    {"PyRun_String", (PYTHON_PROC*)&dll_Py_నిరంతరం_String},
    {"PyObject_GetAttrString", (PYTHON_PROC*)&dll_PyObject_GetAttrString},
    {"PyObject_HasAttrString", (PYTHON_PROC*)&dll_PyObject_HasAttrString},
    {"PyObject_SetAttrString", (PYTHON_PROC*)&dll_Py_Object_సమితి_Attr_String},
    {"PyObject_CallFunctionObjArgs", (PYTHON_PROC*)&dll_Py_Object_Call_Function_Obj_వాదనలు},
    {"PyObject_CallFunction", (PYTHON_PROC*)&dll_PyObject_CallFunction},
    {"PyObject_Call", (PYTHON_PROC*)&dll_PyObject_Call},
    {"PyObject_Repr", (PYTHON_PROC*)&dll_PyObject_Repr},
    {"PyString_AsString", (PYTHON_PROC*)&dll_PyString_AsString},
    {"PyString_AsStringAndSize", (PYTHON_PROC*)&dll_Py_String_As_String_And_కొలత},
    {"PyString_FromString", (PYTHON_PROC*)&dll_PyString_FromString},
    {"PyString_FromFormat", (PYTHON_PROC*)&dll_PyString_FromFormat},
    {"PyString_FromStringAndSize", (PYTHON_PROC*)&dll_Py_String_From_String_And_కొలత},
    {"PyString_Size", (PYTHON_PROC*)&dll_Py_String_కొలత},
    {"PyString_Type", (PYTHON_PROC*)&dll_PyString_Type},
    {"PyUnicode_Type", (PYTHON_PROC*)&dll_PyUnicode_Type},
    {"PyFloat_Type", (PYTHON_PROC*)&dll_PyFloat_Type},
    {"PyFloat_AsDouble", (PYTHON_PROC*)&dll_PyFloat_AsDouble},
    {"PyFloat_FromDouble", (PYTHON_PROC*)&dll_PyFloat_FromDouble},
    {"PyImport_AddModule", (PYTHON_PROC*)&dll_Py_Import_కూడు_Module},
    {"PyNumber_Check", (PYTHON_PROC*)&dll_Py_సంఖ్య_Check},
    {"PyNumber_Long", (PYTHON_PROC*)&dll_Py_సంఖ్య_Long},
    {"PySys_SetObject", (PYTHON_PROC*)&dll_Py_Sys_సమితి_Object},
    {"PySys_GetObject", (PYTHON_PROC*)&dll_PySys_GetObject},
    {"PySys_SetArgv", (PYTHON_PROC*)&dll_Py_Sys_సమితి_Argv},
    {"PyType_Type", (PYTHON_PROC*)&dll_PyType_Type},
    {"PyFile_Type", (PYTHON_PROC*)&dll_PyFile_Type},
    {"PySlice_Type", (PYTHON_PROC*)&dll_Py_ముక్క_Type},
    {"PyType_Ready", (PYTHON_PROC*)&dll_PyType_Ready},
    {"PyType_GenericAlloc", (PYTHON_PROC*)&dll_PyType_GenericAlloc},
    {"Py_FindMethod", (PYTHON_PROC*)&dll_Py_కనుగొను_పద్ధతి},
    {"Py_SetPythonHome", (PYTHON_PROC*)&dll_Py_సమితి_Python_Home},
    {"Py_Initialize", (PYTHON_PROC*)&dll_Py_Initialize},
    {"Py_Finalize", (PYTHON_PROC*)&dll_Py_Finalize},
    {"Py_IsInitialized", (PYTHON_PROC*)&dll_Py_IsInitialized},
    {"_PyObject_New", (PYTHON_PROC*)&dll__PyObject_New},
    {"_PyObject_GC_New", (PYTHON_PROC*)&dll__PyObject_GC_New},
# ifdef Py_underscore_GC
    {"_PyObject_GC_Del", (PYTHON_PROC*)&dll__PyObject_GC_Del},
    {"_PyObject_GC_UnTrack", (PYTHON_PROC*)&dll__PyObject_GC_UnTrack},
# లేకపోతే
    {"PyObject_GC_Del", (PYTHON_PROC*)&dll_PyObject_GC_Del},
    {"PyObject_GC_UnTrack", (PYTHON_PROC*)&dll_PyObject_GC_UnTrack},
# endif
    {"PyObject_Init", (PYTHON_PROC*)&dll__PyObject_Init},
    {"PyObject_GetIter", (PYTHON_PROC*)&dll_PyObject_GetIter},
    {"PyObject_IsTrue", (PYTHON_PROC*)&dll_PyObject_IsTrue},
# ఒకవేళ defined(PY_సంస్కరణ_HEX) && PY_సంస్కరణ_HEX >= 0x02070000
    {"_PyObject_NextNotImplemented", (PYTHON_PROC*)&dll_Py_Object_తదుపరి_Not_Implemented},
# endif
    {"_Py_NoneStruct", (PYTHON_PROC*)&dll__Py_NoneStruct},
    {"_Py_ZeroStruct", (PYTHON_PROC*)&dll__Py_ZeroStruct},
    {"_Py_TrueStruct", (PYTHON_PROC*)&dll__Py_TrueStruct},
# ఒకవేళ defined(PY_సంస్కరణ_HEX) && PY_సంస్కరణ_HEX >= 0x02020000
#  ifdef Py_DEBUG
    {"_Py_NegativeRefcount", (PYTHON_PROC*)&dll__Py_NegativeRefcount},
    {"_Py_RefTotal", (PYTHON_PROC*)&dll_Py_Ref_మొత్తం},
    {"_Py_Dealloc", (PYTHON_PROC*)&dll__Py_Dealloc},
#  endif
    {"PyType_IsSubtype", (PYTHON_PROC*)&dll_PyType_IsSubtype},
# endif
# ఒకవేళ defined(PY_సంస్కరణ_HEX) && PY_సంస్కరణ_HEX >= 0x02030000
#  ఒకవేళ defined(Py_DEBUG) && !defined(Py_DEBUG_NO_PYMALLOC)
    {"_PyObject_DebugFree", (PYTHON_PROC*)&dll__PyObject_DebugFree},
    {"_PyObject_DebugMalloc", (PYTHON_PROC*)&dll__PyObject_DebugMalloc},
#  లేకపోతే
    {"PyObject_Malloc", (PYTHON_PROC*)&dll_PyObject_Malloc},
    {"PyObject_Free", (PYTHON_PROC*)&dll_PyObject_Free},
#  endif
# endif
# ఒకవేళ defined(PY_సంస్కరణ_HEX) && PY_సంస్కరణ_HEX >= 0x02050000 \
	&& SIZEOF_కొలత_T != VIM_SIZEOF_INT
#  ifdef Py_DEBUG
    {"Py_InitModule4TraceRefs_64", (PYTHON_PROC*)&dll_Py_InitModule4},
#  లేకపోతే
    {"Py_InitModule4_64", (PYTHON_PROC*)&dll_Py_InitModule4},
#  endif
# లేకపోతే
#  ifdef Py_DEBUG
    {"Py_InitModule4TraceRefs", (PYTHON_PROC*)&dll_Py_InitModule4},
#  లేకపోతే
    {"Py_InitModule4", (PYTHON_PROC*)&dll_Py_InitModule4},
#  endif
# endif
# ifdef PY_USE_CAPSULE
    {"PyCapsule_New", (PYTHON_PROC*)&dll_PyCapsule_New},
    {"PyCapsule_GetPointer", (PYTHON_PROC*)&dll_Py_Capsule_Get_సూచిక},
# లేకపోతే
    {"PyCObject_FromVoidPtr", (PYTHON_PROC*)&dll_PyCObject_FromVoidPtr},
    {"PyCObject_AsVoidPtr", (PYTHON_PROC*)&dll_PyCObject_AsVoidPtr},
# endif
# ఒకవేళ defined(PY_సంస్కరణ_HEX) && PY_సంస్కరణ_HEX >= 0x02070000
    {"Py_NoSiteFlag", (PYTHON_PROC*)&dll_Py_NoSiteFlag},
# endif
    {"", ఖాళీ},
};

/*
 * Load library and get all pointers.
 * Parameter 'libname' provides name of DLL.
 * Return OK or FAIL.
 */
    స్థిర పూర్ణాంకం
python_runtime_link_init(అక్షరం *libname, పూర్ణాంకం verbose)
{
    పూర్ణాంకం i;
    PYTHON_PROC *ucs_as_encoded_string =
				   (PYTHON_PROC*)&py_PyUnicode_AsEncodedString;

# ఒకవేళ !(defined(PY_NO_RTLD_GLOBAL) && defined(PY3_NO_RTLD_GLOBAL)) && defined(UNIX) && defined(FEAT_PYTHON3)
    // Can't have Python and Python3 loaded at the same time.
    // It causes a crash, because RTLD_GLOBAL is needed for
    // standard C extension libraries of one or both python versions.
    ఒకవేళ (python3_loaded())
    {
	ఒకవేళ (verbose)
	    emsg(_(e_this_vim_cannot_execute_python_after_using_py3));
	తిరిగి FAIL;
    }
# endif

    ఒకవేళ (hinstPython)
	తిరిగి OK;
    hinstPython = ఎక్కించు_dll(libname);
    ఒకవేళ (!hinstPython)
    {
	ఒకవేళ (verbose)
	    semsg(_(e_could_not_ఎక్కించు_library_str_str), libname, ఎక్కించు_dll_error());
	తిరిగి FAIL;
    }

    కొరకు (i = 0; python_funcname_పట్టిక[i].ptr; ++i)
    {
	ఒకవేళ ((*python_funcname_పట్టిక[i].ptr = చిహ్నం_from_dll(hinstPython,
			python_funcname_పట్టిక[i].name)) == ఖాళీ)
	{
	    close_dll(hinstPython);
	    hinstPython = 0;
	    ఒకవేళ (verbose)
		semsg(_(e_could_not_ఎక్కించు_library_function_str), python_funcname_పట్టిక[i].name);
	    తిరిగి FAIL;
	}
    }

    // Load unicode functions separately as only the ucs2 or the ucs4 functions
    // will be present in the library.
    *ucs_as_encoded_string = చిహ్నం_from_dll(hinstPython,
					     "PyUnicodeUCS2_AsEncodedString");
    ఒకవేళ (*ucs_as_encoded_string == ఖాళీ)
	*ucs_as_encoded_string = చిహ్నం_from_dll(hinstPython,
					     "PyUnicodeUCS4_AsEncodedString");
    ఒకవేళ (*ucs_as_encoded_string == ఖాళీ)
    {
	close_dll(hinstPython);
	hinstPython = 0;
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
python_enabled(పూర్ణాంకం verbose)
{
    తిరిగి python_runtime_link_init((అక్షరం *)p_pydll, verbose) == OK;
}

/*
 * Load the standard Python exceptions - don't import the symbols from the
 * DLL, as this can cause errors (importing data symbols is not reliable).
 */
    స్థిర శూన్య
get_exceptions(శూన్య)
{
    PyObject *exmod = PyImport_ImportModule("exceptions");
    PyObject *exdict = PyModule_GetDict(exmod);
    imp_PyExc_AttributeError = Py_Dict_Get_అంశం_String(exdict, "AttributeError");
    imp_Py_Exc_సూచీ_Error = Py_Dict_Get_అంశం_String(exdict, "IndexError");
    imp_Py_Exc_కీలకం_Error = Py_Dict_Get_అంశం_String(exdict, "KeyError");
    imp_Py_Exc_Keyboard_అంతరాయం = Py_Dict_Get_అంశం_String(exdict, "KeyboardInterrupt");
    imp_PyExc_TypeError = Py_Dict_Get_అంశం_String(exdict, "TypeError");
    imp_Py_Exc_విలువ_Error = Py_Dict_Get_అంశం_String(exdict, "ValueError");
    imp_PyExc_SystemExit = Py_Dict_Get_అంశం_String(exdict, "SystemExit");
    imp_PyExc_RuntimeError = Py_Dict_Get_అంశం_String(exdict, "RuntimeError");
    imp_PyExc_ImportError = Py_Dict_Get_అంశం_String(exdict, "ImportError");
    imp_Py_Exc_అధికప్రవాహం_Error = Py_Dict_Get_అంశం_String(exdict, "OverflowError");
    Py_XINCREF(imp_PyExc_AttributeError);
    Py_XINCREF(imp_Py_Exc_సూచీ_Error);
    Py_XINCREF(imp_Py_Exc_కీలకం_Error);
    Py_XINCREF(imp_Py_Exc_Keyboard_అంతరాయం);
    Py_XINCREF(imp_PyExc_TypeError);
    Py_XINCREF(imp_Py_Exc_విలువ_Error);
    Py_XINCREF(imp_PyExc_SystemExit);
    Py_XINCREF(imp_PyExc_RuntimeError);
    Py_XINCREF(imp_PyExc_ImportError);
    Py_XINCREF(imp_Py_Exc_అధికప్రవాహం_Error);
    Py_XDECREF(exmod);
}
#అయితే_ముగింపు // DYNAMIC_PYTHON

స్థిర పూర్ణాంకం initialised = 0;
#నిర్వచించు PYINITIALISED initialised
స్థిర పూర్ణాంకం python_చివర_called = FALSE;

#నిర్వచించు DESTRUCTOR_ముగింపు(self) self->ob_type->tp_free((PyObject*)self);

#నిర్వచించు WIN_PYTHON_REF(win) win->w_python_ref
#నిర్వచించు BUF_PYTHON_REF(buf) buf->b_python_ref
#నిర్వచించు TAB_PYTHON_REF(tab) tab->tp_python_ref

స్థిర PyObject *OutputGetattr(PyObject *, అక్షరం *);
స్థిర PyObject *బఫరు_Getattr(PyObject *, అక్షరం *);
స్థిర PyObject *కిటికీ_Getattr(PyObject *, అక్షరం *);
స్థిర PyObject *Tab_పుట_Getattr(PyObject *, అక్షరం *);
స్థిర PyObject *RangeGetattr(PyObject *, అక్షరం *);
స్థిర PyObject *DictionaryGetattr(PyObject *, అక్షరం*);
స్థిర PyObject *ListGetattr(PyObject *, అక్షరం *);
స్థిర PyObject *TupleGetattr(PyObject *, అక్షరం *);
స్థిర PyObject *FunctionGetattr(PyObject *, అక్షరం *);

#నిర్వచితం_కాకపోతే Py_సందర్శించు
# define Py_సందర్శించు(obj) సందర్శించు(obj, arg)
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే Py_CLEAR
# define Py_CLEAR(obj) \
    { \
	Py_XDECREF(obj); \
	obj = ఖాళీ; \
    }
#అయితే_ముగింపు

#ఒకవేళ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    స్థిర శూన్య *
py_memsave(శూన్య *p, size_t len)
{
    శూన్య	*r;

    ఒకవేళ (!(r = PyMem_Malloc(len)))
	తిరిగి ఖాళీ;
    mch_memmove(r, p, len);
    తిరిగి r;
}

# define PY_STRSAVE(s) ((char_u *) py_memsave(s, STRLEN(s) + 1))
#అయితే_ముగింపు

రకనిర్వచనం Py_ముక్క_Object Py_ముక్క_Object_T;

/*
 * Include the code shared with if_python3.c
 */
#అన్తర్భావ "if_py_both.h"


///////////////////////////////////////////////////////
// Internal function prototypes.

స్థిర పూర్ణాంకం PythonMod_Init(శూన్య);


///////////////////////////////////////////////////////
// 1. Python interpreter main program.

#నిర్వచితం_కాకపోతే PY_CAN_RECURSE
స్థిర PyThreadState *saved_python_thread = ఖాళీ;

/*
 * Suspend a thread of the Python interpreter, other threads are allowed to
 * run.
 */
    స్థిర శూన్య
Python_SaveThread(శూన్య)
{
    saved_python_thread = PyEval_SaveThread();
}

/*
 * Restore a thread of the Python interpreter, waits for other threads to
 * block.
 */
    స్థిర శూన్య
Python_RestoreThread(శూన్య)
{
    PyEval_RestoreThread(saved_python_thread);
    saved_python_thread = ఖాళీ;
}
#అయితే_ముగింపు

    శూన్య
python_చివర(శూన్య)
{
    స్థిర పూర్ణాంకం recurse = 0;

    // If a crash occurs while doing this, don't try again.
    ఒకవేళ (recurse != 0)
	తిరిగి;

    python_చివర_called = TRUE;
    ++recurse;

#నిర్వచితమైతే DYNAMIC_PYTHON
    ఒకవేళ (hinstPython && Py_IsInitialized())
    {
# ifdef PY_CAN_RECURSE
	PyGILState_Ensure();
# లేకపోతే
	Python_RestoreThread();	    // enter python
# endif
	Py_Finalize();
    }
#లేకపోతే
    ఒకవేళ (Py_IsInitialized())
    {
# ifdef PY_CAN_RECURSE
	PyGILState_Ensure();
# లేకపోతే
	Python_RestoreThread();	    // enter python
# endif
	Py_Finalize();
    }
#అయితే_ముగింపు

    --recurse;
}

#ఒకవేళ defined(DYNAMIC_PYTHON) && defined(FEAT_PYTHON3)
    పూర్ణాంకం
python_loaded(శూన్య)
{
    తిరిగి (hinstPython != 0);
}
#అయితే_ముగింపు

స్థిర అక్షరం *py_home_buf = ఖాళీ;

    స్థిర పూర్ణాంకం
Python_Init(శూన్య)
{
    ఒకవేళ (!initialised)
    {
#ఒకవేళ defined(PY_సంస్కరణ_HEX) && PY_సంస్కరణ_HEX >= 0x02070000
	PyObject *site;
#అయితే_ముగింపు

#నిర్వచితమైతే DYNAMIC_PYTHON
	ఒకవేళ (!python_enabled(TRUE))
	{
	    emsg(_(e_sorry_this_command_is_disabled_python_library_could_not_be_దొరికింది));
	    వెళ్ళు fail;
	}
#అయితే_ముగింపు

	ఒకవేళ (*p_pyhome != NUL)
	{
	    // The string must not change later, make a copy in static memory.
	    py_home_buf = (అక్షరం *)vim_strsave(p_pyhome);
	    ఒకవేళ (py_home_buf != ఖాళీ)
		Py_సమితి_Python_Home(py_home_buf);
	}
#నిర్వచితమైతే PYTHON_HOME
	లేకపోతే ఒకవేళ (mch_getenv((char_u *)"PYTHONHOME") == ఖాళీ)
	    Py_సమితి_Python_Home(PYTHON_HOME);
#అయితే_ముగింపు

	init_structs();

#ఒకవేళ defined(PY_సంస్కరణ_HEX) && PY_సంస్కరణ_HEX >= 0x02070000
	// Disable implicit 'import site', because it may cause Vim to exit
	// when it can't be found.
	Py_NoSiteFlag++;
#అయితే_ముగింపు

	Py_Initialize();

#ఒకవేళ defined(PY_సంస్కరణ_HEX) && PY_సంస్కరణ_HEX >= 0x02070000
	// 'import site' explicitly.
	site = PyImport_ImportModule("site");
	ఒకవేళ (site == ఖాళీ)
	{
	    emsg(_(e_sorry_this_command_is_disabled_python_site_module_could_not_be_loaded));
	    వెళ్ళు fail;
	}
	Py_DECREF(site);
#అయితే_ముగింపు

	// Initialise threads, and below save the state using
	// PyEval_SaveThread.  Without the call to PyEval_SaveThread, thread
	// specific state (such as the system trace hook), will be lost
	// between invocations of Python code.
	Py_Eval_Init_దారాలు();
#నిర్వచితమైతే DYNAMIC_PYTHON
	get_exceptions();
#అయితే_ముగింపు

	ఒకవేళ (PythonIO_Init_io())
	    వెళ్ళు fail;

	ఒకవేళ (PythonMod_Init())
	    వెళ్ళు fail;

	globals = PyModule_GetDict(Py_Import_కూడు_Module("__main__"));

	// Remove the element from sys.path that was added because of our
	// argv[0] value in PythonMod_Init().  Previously we used an empty
	// string, but depending on the OS we then get an empty entry or
	// the current directory in sys.path.
	Py_నిరంతరం_Simple_String("import sys; sys.path = filter(lambda x: x != '/must>not&exist', sys.path)");

	// lock is created and acquired in PyEval_InitThreads() and thread
	// state is created in Py_Initialize()
	// there _PyGILState_NoteThreadState() also sets gilcounter to 1
	// (python must have threads enabled!)
	// so the following does both: unlock GIL and save thread state in TLS
	// without deleting thread state
#నిర్వచితం_కాకపోతే PY_CAN_RECURSE
	saved_python_thread =
#అయితే_ముగింపు
	    PyEval_SaveThread();

	initialised = 1;
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
DoPyCommand(
    స్థిరరాశి అక్షరం		*cmd,
    dict_T*		locals,
    rangeinitializer	init_range,
    runner		నిరంతరం,
    శూన్య		*arg)
{
#నిర్వచితం_కాకపోతే PY_CAN_RECURSE
    స్థిర పూర్ణాంకం		recursive = 0;
#అయితే_ముగింపు
#ఒకవేళ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    అక్షరం		*saved_locale;
#అయితే_ముగింపు
#నిర్వచితమైతే PY_CAN_RECURSE
    PyGILState_STATE	pygilstate;
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే PY_CAN_RECURSE
    ఒకవేళ (recursive)
    {
	emsg(_(e_cannot_invoke_python_recursively));
	తిరిగి;
    }
    ++recursive;
#అయితే_ముగింపు
    ఒకవేళ (python_చివర_called)
	తిరిగి;

    ఒకవేళ (Python_Init())
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
	saved_locale = (అక్షరం *) PY_STRSAVE(saved_locale);
	(శూన్య)setlocale(LC_NUMERIC, "C");
    }
#అయితే_ముగింపు

#నిర్వచితమైతే PY_CAN_RECURSE
    pygilstate = PyGILState_Ensure();
#లేకపోతే
    Python_RestoreThread();	    // enter python
#అయితే_ముగింపు

    నిరంతరం((అక్షరం *) cmd, locals, arg
#నిర్వచితమైతే PY_CAN_RECURSE
	    , &pygilstate
#అయితే_ముగింపు
	    );

#నిర్వచితమైతే PY_CAN_RECURSE
    PyGILState_Release(pygilstate);
#లేకపోతే
    Python_SaveThread();	    // leave python
#అయితే_ముగింపు

#ఒకవేళ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    ఒకవేళ (saved_locale != ఖాళీ)
    {
	(శూన్య)setlocale(LC_NUMERIC, saved_locale);
	PyMem_Free(saved_locale);
    }
#అయితే_ముగింపు

    Python_తాళం_Vim();		    // enter vim
    PythonIO_Flush();

theend:
#నిర్వచితం_కాకపోతే PY_CAN_RECURSE
    --recursive;
#అయితే_ముగింపు
    తిరిగి;
}

/*
 * ":python"
 */
    శూన్య
ex_python(exarg_T *eap)
{
    char_u *script;

    script = script_get(eap, eap->arg);
    ఒకవేళ (!eap->skip)
    {
	ఒకవేళ (p_pyx == 0)
	    p_pyx = 2;

	DoPyCommand(script == ఖాళీ ? (అక్షరం *) eap->arg : (అక్షరం *) script,
		ఖాళీ,
		init_range_cmd,
		(runner) నిరంతరం_cmd,
		(శూన్య *) eap);
    }
    vim_free(script);
}

#నిర్వచించు బఫరు_కొలత 1024

/*
 * ":pyfile"
 */
    శూన్య
ex_pyfile(exarg_T *eap)
{
    స్థిర అక్షరం బఫరు[బఫరు_కొలత];
    స్థిరరాశి అక్షరం *file = (అక్షరం *)eap->arg;
    అక్షరం *p;

    ఒకవేళ (p_pyx == 0)
	p_pyx = 2;

    // Have to do it like this. PyRun_SimpleFile requires you to pass a
    // stdio file pointer, but Vim and the Python DLL are compiled with
    // different options under Windows, meaning that stdio pointers aren't
    // compatible between the two. Yuk.
    //
    // Put the string "execfile('file')" into buffer. But, we need to
    // escape any backslashes or single quotes in the file name, so that
    // Python won't mangle the file name.
    strcpy(బఫరు, "execfile('");
    p = బఫరు + 10; // size of "execfile('"

    అయితే (*file && p < బఫరు + (బఫరు_కొలత - 3))
    {
	ఒకవేళ (*file == '\\' || *file == '\'')
	    *p++ = '\\';
	*p++ = *file++;
    }

    // If we didn't finish the file name, we hit a buffer overflow
    ఒకవేళ (*file != '\0')
	తిరిగి;

    // Put in the terminating "')" and a null
    *p++ = '\'';
    *p++ = ')';
    *p++ = '\0';

    // Execute the file
    DoPyCommand(బఫరు,
	    ఖాళీ,
	    init_range_cmd,
	    (runner) నిరంతరం_cmd,
	    (శూన్య *) eap);
}

    శూన్య
ex_pydo(exarg_T *eap)
{
    ఒకవేళ (p_pyx == 0)
	p_pyx = 2;

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
OutputGetattr(PyObject *self, అక్షరం *name)
{
    ఒకవేళ (strcmp(name, "softspace") == 0)
	తిరిగి PyInt_FromLong(((OutputObject *)(self))->softspace);
    లేకపోతే ఒకవేళ (strcmp(name, "__members__") == 0)
	తిరిగి ObjectDir(ఖాళీ, OutputAttrs);
    లేకపోతే ఒకవేళ (strcmp(name, "errors") == 0)
	తిరిగి PyString_FromString("strict");
    లేకపోతే ఒకవేళ (strcmp(name, "encoding") == 0)
	తిరిగి PyString_FromString(ENC_OPT);
    తిరిగి Py_కనుగొను_పద్ధతి(OutputMethods, self, name);
}

///////////////////////////////////////////////////////
// 3. Implementation of the Vim module for Python

// Window type - Implementation functions
// --------------------------------------

#నిర్వచించు కిటికీ_Type_Check(obj) ((obj)->ob_type == &కిటికీ_Type)

// Buffer type - Implementation functions
// --------------------------------------

#నిర్వచించు బఫరు_Type_Check(obj) ((obj)->ob_type == &బఫరు_Type)

స్థిర పూర్ణాంకం బఫరు_Ass_అంశం(PyObject *, PyInt, PyObject *);
స్థిర పూర్ణాంకం బఫరు_Ass_ముక్క(PyObject *, PyInt, PyInt, PyObject *);

// Line range type - Implementation functions
// --------------------------------------

#నిర్వచించు RangeType_Check(obj) ((obj)->ob_type == &RangeType)

స్థిర పూర్ణాంకం Range_Ass_అంశం(PyObject *, PyInt, PyObject *);
స్థిర పూర్ణాంకం Range_Ass_ముక్క(PyObject *, PyInt, PyInt, PyObject *);

// Current objects type - Implementation functions
// -----------------------------------------------

స్థిర PySequenceMethods బఫరు_As_Seq = {
    (PyInquiry)		బఫరు_నిడివి,	    // sq_length,    len(x)
    (binaryfunc)	0,		    // BufferConcat, sq_concat, x+y
    (PyIntArgFunc)	0,		    // BufferRepeat, sq_repeat, x*n
    (PyIntArgFunc)	బఫరు_అంశం,	    // sq_item,      x[i]
    (PyIntIntArgFunc)	బఫరు_ముక్క,	    // sq_slice,     x[i:j]
    (PyIntObjArgProc)	బఫరు_Ass_అంశం,	    // sq_ass_item,  x[i]=v
    (PyIntIntObjArgProc) బఫరు_Ass_ముక్క,    // sq_ass_slice, x[i:j]=v
    (objobjproc)	0,
    (binaryfunc)	0,
    0,
};

// Buffer object - Implementation

    స్థిర PyObject *
బఫరు_Getattr(PyObject *self, అక్షరం *name)
{
    PyObject *r;

    ఒకవేళ ((r = బఫరు_Attr_Valid((బఫరు_Object *)(self), name)))
	తిరిగి r;

    ఒకవేళ (Check_బఫరు((బఫరు_Object *)(self)))
	తిరిగి ఖాళీ;

    r = బఫరు_Attr((బఫరు_Object *)(self), name);
    ఒకవేళ (r || PyErr_Occurred())
	తిరిగి r;
    లేకపోతే
	తిరిగి Py_కనుగొను_పద్ధతి(బఫరు_Methods, self, name);
}

//////////////////

    స్థిర పూర్ణాంకం
బఫరు_Ass_అంశం(PyObject *self, PyInt n, PyObject *val)
{
    తిరిగి RBAs_అంశం((బఫరు_Object *)(self), n, val, 1, -1, ఖాళీ);
}

    స్థిర పూర్ణాంకం
బఫరు_Ass_ముక్క(PyObject *self, PyInt lo, PyInt hi, PyObject *val)
{
    తిరిగి RBAs_ముక్క((బఫరు_Object *)(self), lo, hi, val, 1, -1, ఖాళీ);
}

స్థిర PySequenceMethods RangeAsSeq = {
    (PyInquiry)		Range_నిడివి,	      // sq_length,    len(x)
    (binaryfunc)	0, /* RangeConcat, */ // sq_concat,    x+y
    (PyIntArgFunc)	0, /* RangeRepeat, */ // sq_repeat,    x*n
    (PyIntArgFunc)	Range_అంశం,	      // sq_item,      x[i]
    (PyIntIntArgFunc)	Range_ముక్క,	      // sq_slice,     x[i:j]
    (PyIntObjArgProc)	Range_Ass_అంశం,	      // sq_ass_item,  x[i]=v
    (PyIntIntObjArgProc) Range_Ass_ముక్క,	      // sq_ass_slice, x[i:j]=v
    (objobjproc)	0,
#ఒకవేళ PY_MAJOR_సంస్కరణ >= 2
    (binaryfunc)	0,
    0,
#అయితే_ముగింపు
};

// Line range object - Implementation

    స్థిర PyObject *
RangeGetattr(PyObject *self, అక్షరం *name)
{
    ఒకవేళ (strcmp(name, "start") == 0)
	తిరిగి Py_Build_విలువ(Py_ssize_t_fmt, ((RangeObject *)(self))->ప్రారంభం - 1);
    లేకపోతే ఒకవేళ (strcmp(name, "end") == 0)
	తిరిగి Py_Build_విలువ(Py_ssize_t_fmt, ((RangeObject *)(self))->చివర - 1);
    లేకపోతే ఒకవేళ (strcmp(name, "__members__") == 0)
	తిరిగి ObjectDir(ఖాళీ, RangeAttrs);
    లేకపోతే
	తిరిగి Py_కనుగొను_పద్ధతి(RangeMethods, self, name);
}

////////////////

    స్థిర పూర్ణాంకం
Range_Ass_అంశం(PyObject *self, PyInt n, PyObject *val)
{
    తిరిగి RBAs_అంశం(((RangeObject *)(self))->buf, n, val,
		     ((RangeObject *)(self))->ప్రారంభం,
		     ((RangeObject *)(self))->చివర,
		     &((RangeObject *)(self))->చివర);
}

    స్థిర పూర్ణాంకం
Range_Ass_ముక్క(PyObject *self, PyInt lo, PyInt hi, PyObject *val)
{
    తిరిగి RBAs_ముక్క(((RangeObject *)(self))->buf, lo, hi, val,
		      ((RangeObject *)(self))->ప్రారంభం,
		      ((RangeObject *)(self))->చివర,
		      &((RangeObject *)(self))->చివర);
}

// TabPage object - Implementation

    స్థిర PyObject *
Tab_పుట_Getattr(PyObject *self, అక్షరం *name)
{
    PyObject *r;

    ఒకవేళ ((r = Tab_పుట_Attr_Valid((Tab_పుట_Object *)(self), name)))
	తిరిగి r;

    ఒకవేళ (Check_Tab_పుట((Tab_పుట_Object *)(self)))
	తిరిగి ఖాళీ;

    r = Tab_పుట_Attr((Tab_పుట_Object *)(self), name);
    ఒకవేళ (r || PyErr_Occurred())
	తిరిగి r;
    లేకపోతే
	తిరిగి Py_కనుగొను_పద్ధతి(Tab_పుట_Methods, self, name);
}

// Window object - Implementation

    స్థిర PyObject *
కిటికీ_Getattr(PyObject *self, అక్షరం *name)
{
    PyObject *r;

    ఒకవేళ ((r = కిటికీ_Attr_Valid((కిటికీ_Object *)(self), name)))
	తిరిగి r;

    ఒకవేళ (Check_కిటికీ((కిటికీ_Object *)(self)))
	తిరిగి ఖాళీ;

    r = కిటికీ_Attr((కిటికీ_Object *)(self), name);
    ఒకవేళ (r || PyErr_Occurred())
	తిరిగి r;
    లేకపోతే
	తిరిగి Py_కనుగొను_పద్ధతి(కిటికీ_Methods, self, name);
}

// Tab page list object - Definitions

స్థిర PySequenceMethods TabListAsSeq = {
    (PyInquiry)		Tab_List_నిడివి,	    // sq_length,    len(x)
    (binaryfunc)	0,		    // sq_concat,    x+y
    (PyIntArgFunc)	0,		    // sq_repeat,    x*n
    (PyIntArgFunc)	Tab_List_అంశం,	    // sq_item,      x[i]
    (PyIntIntArgFunc)	0,		    // sq_slice,     x[i:j]
    (PyIntObjArgProc)	0,		    // sq_ass_item,  x[i]=v
    (PyIntIntObjArgProc) 0,		    // sq_ass_slice, x[i:j]=v
    (objobjproc)	0,
#ఒకవేళ PY_MAJOR_సంస్కరణ >= 2
    (binaryfunc)	0,
    0,
#అయితే_ముగింపు
};

// Window list object - Definitions

స్థిర PySequenceMethods WinListAsSeq = {
    (PyInquiry)		Win_List_నిడివి,	    // sq_length,    len(x)
    (binaryfunc)	0,		    // sq_concat,    x+y
    (PyIntArgFunc)	0,		    // sq_repeat,    x*n
    (PyIntArgFunc)	Win_List_అంశం,	    // sq_item,      x[i]
    (PyIntIntArgFunc)	0,		    // sq_slice,     x[i:j]
    (PyIntObjArgProc)	0,		    // sq_ass_item,  x[i]=v
    (PyIntIntObjArgProc) 0,		    // sq_ass_slice, x[i:j]=v
    (objobjproc)	0,
#ఒకవేళ PY_MAJOR_సంస్కరణ >= 2
    (binaryfunc)	0,
    0,
#అయితే_ముగింపు
};

// External interface

    శూన్య
python_బఫరు_free(buf_T *buf)
{
    బఫరు_Object *bp = BUF_PYTHON_REF(buf);
    ఒకవేళ (bp == ఖాళీ)
	తిరిగి;
    bp->buf = INVALID_బఫరు_విలువ;
    BUF_PYTHON_REF(buf) = ఖాళీ;
}

    శూన్య
python_కిటికీ_free(win_T *win)
{
    కిటికీ_Object *wp = WIN_PYTHON_REF(win);
    ఒకవేళ (wp == ఖాళీ)
	తిరిగి;
    wp->win = INVALID_కిటికీ_విలువ;
    WIN_PYTHON_REF(win) = ఖాళీ;
}

    శూన్య
python_tabpage_free(tabpage_T *tab)
{
    Tab_పుట_Object *tp = TAB_PYTHON_REF(tab);
    ఒకవేళ (tp == ఖాళీ)
	తిరిగి;
    tp->tab = INVALID_TABPAGE_విలువ;
    TAB_PYTHON_REF(tab) = ఖాళీ;
}

    స్థిర పూర్ణాంకం
PythonMod_Init(శూన్య)
{
    // The special value is removed from sys.path in Python_Init().
    స్థిర అక్షరం	*(argv[2]) = {"/must>not&exist/foo", ఖాళీ};

    ఒకవేళ (init_types())
	తిరిగి -1;

    // Set sys.argv[] to avoid a crash in warn().
    Py_Sys_సమితి_Argv(1, argv);

    vim_module = Py_InitModule4("vim", VimMethods, (అక్షరం *)ఖాళీ,
				(PyObject *)ఖాళీ, PYTHON_API_సంస్కరణ);

    ఒకవేళ (populate_module(vim_module))
	తిరిగి -1;

    ఒకవేళ (init_sys_మార్గం())
	తిరిగి -1;

    తిరిగి 0;
}

//////////////////////////////////////////////////////////////////////////
// 4. Utility functions for handling the interface between Vim and Python.

// Convert a Vim line into a Python string.
// All internal newlines are replaced by null characters.
//
// On errors, the Python exception data is set, and NULL is returned.
    స్థిర PyObject *
పంక్తి_గమ్యం_String(స్థిరరాశి అక్షరం *str)
{
    PyObject *ఫలితం;
    PyInt len = అక్షరశ్రేణి_పొడవు(str);
    అక్షరం *p;

    // Allocate a Python string object, with uninitialised contents. We
    // must do it this way, so that we can modify the string in place
    // later. See the Python source, Objects/stringobject.c for details.
    ఫలితం = Py_String_From_String_And_కొలత(ఖాళీ, len);
    ఒకవేళ (ఫలితం == ఖాళీ)
	తిరిగి ఖాళీ;

    p = PyString_AsString(ఫలితం);

    అయితే (*str)
    {
	ఒకవేళ (*str == '\n')
	    *p = '\0';
	లేకపోతే
	    *p = *str;

	++p;
	++str;
    }

    తిరిగి ఫలితం;
}

    స్థిర PyObject *
DictionaryGetattr(PyObject *self, అక్షరం *name)
{
    DictionaryObject	*this = ((DictionaryObject *) (self));

    ఒకవేళ (strcmp(name, "locked") == 0)
	తిరిగి PyInt_FromLong(this->dict->dv_తాళం);
    లేకపోతే ఒకవేళ (strcmp(name, "scope") == 0)
	తిరిగి PyInt_FromLong(this->dict->dv_scope);
    లేకపోతే ఒకవేళ (strcmp(name, "__members__") == 0)
	తిరిగి ObjectDir(ఖాళీ, DictionaryAttrs);

    తిరిగి Py_కనుగొను_పద్ధతి(DictionaryMethods, self, name);
}

    స్థిర PyObject *
ListGetattr(PyObject *self, అక్షరం *name)
{
    ఒకవేళ (strcmp(name, "locked") == 0)
	తిరిగి PyInt_FromLong(((ListObject *)(self))->list->lv_తాళం);
    లేకపోతే ఒకవేళ (strcmp(name, "__members__") == 0)
	తిరిగి ObjectDir(ఖాళీ, ListAttrs);

    తిరిగి Py_కనుగొను_పద్ధతి(ListMethods, self, name);
}

    స్థిర PyObject *
TupleGetattr(PyObject *self, అక్షరం *name)
{
    ఒకవేళ (strcmp(name, "locked") == 0)
	తిరిగి PyInt_FromLong(((TupleObject *)(self))->tuple->tv_తాళం);
    లేకపోతే ఒకవేళ (strcmp(name, "__members__") == 0)
	తిరిగి ObjectDir(ఖాళీ, TupleAttrs);

    తిరిగి Py_కనుగొను_పద్ధతి(TupleMethods, self, name);
}

    స్థిర PyObject *
FunctionGetattr(PyObject *self, అక్షరం *name)
{
    PyObject	*r;

    r = FunctionAttr((FunctionObject *)(self), name);

    ఒకవేళ (r || PyErr_Occurred())
	తిరిగి r;
    లేకపోతే
	తిరిగి Py_కనుగొను_పద్ధతి(FunctionMethods, self, name);
}

    శూన్య
do_pyeval(char_u *str, dict_T *locals, typval_T *rettv)
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
సమితి_ref_లోపల_python(పూర్ణాంకం copyID)
{
    తిరిగి సమితి_ref_లోపల_py(copyID);
}
