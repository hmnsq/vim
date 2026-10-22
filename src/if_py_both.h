/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */
/*
 * Python extensions by Paul Moore, David Leonard, Roland Puntaier, Nikolay
 * Pavlov.
 *
 * Common code for if_python.c and if_python3.c.
 */

#ఒకవేళ PY_సంస్కరణ_HEX < 0x02050000
రకనిర్వచనం పూర్ణాంకం Py_ssize_t;  // Python 2.4 and earlier don't have this type.
#అయితే_ముగింపు

// Use values that are known to work, others may make Vim crash.
#నిర్వచించు ENC_OPT (enc_utf8 ? "utf-8" : enc_dbcs ? "euc-jp" : (అక్షరం *)p_enc)
#నిర్వచించు DOPY_FUNC "_vim_pydo"

స్థిర స్థిరరాశి అక్షరం *vim_special_మార్గం = "_vim_path_";

#నిర్వచించు Py_Err_సమితి_STRING(exc, str) Py_Err_సమితి_String(exc, _(str))
#నిర్వచించు Py_Err_సమితి_Vim(str) Py_Err_సమితి_String(VimError, str)
#నిర్వచించు Py_Err_సమితి_VIM(str) Py_Err_సమితి_STRING(VimError, str)
#నిర్వచించు PyErr_FORMAT(exc, str, arg) PyErr_Format(exc, _(str), arg)
#నిర్వచించు PyErr_FORMAT2(exc, str, arg1, arg2) PyErr_Format(exc, _(str), arg1,arg2)
#నిర్వచించు PyErr_VIM_FORMAT(str, arg) PyErr_FORMAT(VimError, str, arg)

#నిర్వచితమైతే USE_LIMITED_API
// Limited Python API. Need to call only exposed functions and remap macros.
// PyTypeObject is an opaque struct.

రకనిర్వచనం నిర్మాణం {
    lenfunc sq_నిడివి;
    binaryfunc sq_concat;
    ssizeargfunc sq_repeat;
    ssizeargfunc sq_అంశం;
    శూన్య *was_sq_ముక్క;
    ssizeobjargproc sq_ass_అంశం;
    శూన్య *was_sq_ass_ముక్క;
    objobjproc sq_కలిగియుంది;

    binaryfunc sq_inplace_concat;
    ssizeargfunc sq_inplace_repeat;
} PySequenceMethods;

రకనిర్వచనం నిర్మాణం {
    lenfunc mp_నిడివి;
    binaryfunc mp_subscript;
    objobjargproc mp_ass_subscript;
} PyMappingMethods;

// This struct emulates the concrete _typeobject struct to allow the code to
// work the same way in both limited and full Python APIs.
నిర్మాణం typeobject_wrapper {
    స్థిరరాశి అక్షరం *tp_name;
    Py_ssize_t tp_basicsize;
    గుర్తులేని దీర్ఘ tp_flags;

    // When adding new slots below, also need to make sure we add ADD_TP_SLOT
    // call in AddHeapType for it.

    destructor tp_dealloc;
    reprfunc tp_repr;

    PySequenceMethods *tp_as_sequence;
    PyMappingMethods *tp_as_mapping;

    ternaryfunc tp_call;
    getattrofunc tp_getattro;
    setattrofunc tp_setattro;

    స్థిరరాశి అక్షరం *tp_doc;

    traverseproc tp_traverse;

    inquiry tp_clear;

    getiterfunc tp_iter;
    iternextfunc tp_iternext;

    నిర్మాణం Py_పద్ధతి_Def *tp_methods;
    నిర్మాణం _typeobject *tp_పునాది;
    allocfunc tp_alloc;
    newfunc tp_new;
    freefunc tp_free;
};

# define DEFINE_PY_TYPE_OBJECT(type) \
    స్థిర నిర్మాణం typeobject_wrapper type; \
    స్థిర PyTypeObject* type##Ptr = ఖాళీ

// PyObject_HEAD_INIT_TYPE and PyObject_FINISH_INIT_TYPE need to come in pairs
// We first initialize with NULL because the type is not allocated until
// init_types() is called later. It's in FINISH_INIT_TYPE where we fill the
// type in with the newly allocated type.
# define Py_Object_తల_INIT_TYPE(type) Py_Object_తల_INIT(ఖాళీ)
# define Py_Object_ముగింపు_INIT_TYPE(obj, type) obj.ob_పునాది.ob_type = type##Ptr

# define Py_TYPE_GET_TP_ALLOC(type) ((allocfunc)Py_Type_Get_స్థానం(type, Py_tp_alloc))
# define Py_TYPE_GET_TP_METHODS(type) ((Py_పద్ధతి_Def *)Py_Type_Get_స్థానం(type, Py_tp_methods))

// PyObject_NEW is not part of stable ABI, but PyObject_Malloc/Init are.
స్థిర PyObject* Vim_PyObject_New(PyTypeObject *type, size_t objsize)
{
    PyObject *obj = (PyObject *)PyObject_Malloc(objsize);
    ఒకవేళ (obj == ఖాళీ)
	తిరిగి Py_Err_No_జ్ఞాపకం();
    తిరిగి PyObject_Init(obj, type);
}
# undef PyObject_NEW
# define PyObject_NEW(type, typeobj) ((type *)Vim_PyObject_New(typeobj, పరిమాణం(type)))

// This is a somewhat convoluted because limited API doesn't expose an easy way
// to get the tp_name field, and so we have to manually reconstruct it as
// "__module__.__name__" (with __module__ omitted for builtins to emulate
// Python behavior). Also, some of the more convenient functions like
// PyUnicode_AsUTF8AndSize and PyType_GetQualName() are not available until
// late Python 3 versions, and won't be available if you set Py_LIMITED_API too
// low.
# define PyErr_FORMAT_TYPE(సందేశం, obj) \
    చేయి { \
	PyObject* qualname = PyObject_GetAttrString((PyObject*)(obj)->ob_type, "__qualname__"); \
	ఒకవేళ (qualname == ఖాళీ) \
	{ \
	    PyErr_FORMAT(PyExc_TypeError, సందేశం, "(NULL)"); \
	    ఆపు; \
	} \
	PyObject* module = PyObject_GetAttrString((PyObject*)(obj)->ob_type, "__module__"); \
	PyObject* నిండిన; \
	ఒకవేళ (module == ఖాళీ || PyUnicode_CompareWithASCIIString(module, "builtins") == 0 \
		|| PyUnicode_CompareWithASCIIString(module, "__main__") == 0) \
	{ \
	    నిండిన = qualname; \
	    Py_INCREF(నిండిన); \
	} \
	లేకపోతే \
	    నిండిన = PyUnicode_FromFormat("%U.%U", module, qualname); \
	PyObject* నిండిన_బైట్లు = PyUnicode_AsUTF8String(నిండిన); \
	స్థిరరాశి అక్షరం* నిండిన_str = Py_బైట్లు_As_String(నిండిన_బైట్లు); \
	నిండిన_str = నిండిన_str == ఖాళీ ? "(NULL)" : నిండిన_str; \
	PyErr_FORMAT(PyExc_TypeError, సందేశం, నిండిన_str); \
	Py_DECREF(qualname); \
	Py_XDECREF(module); \
	Py_XDECREF(నిండిన); \
	Py_XDECREF(నిండిన_బైట్లు); \
    } అయితే (0)

# define Py_List_GET_అంశం(list, i) Py_List_Get_అంశం(list, i)
# define Py_List_GET_కొలత(o) Py_List_కొలత(o)

// PyList_SET_ITEM and PyList_SetItem have slightly different behaviors. The
// former will leave the old item dangling, and the latter will decref on it.
// Since we only use this on new lists, this difference doesn't matter.
# define Py_List_సమితి_అంశం(list, i, అంశం) Py_List_సమితి_అంశం_2(list, i, అంశం)

# define Py_Tuple_GET_అంశం(o, pos) Py_Tuple_Get_అంశం(o, pos)
# define Py_Tuple_GET_కొలత(o) Py_Tuple_కొలత(o)

# ఒకవేళ Py_LIMITED_API < 0x03080000
// PyIter_check only became part of stable ABI in 3.8, and there is no easy way
// to check for it in the API. We simply return false as a compromise. This
// does mean we should avoid compiling with stable ABI < 3.8.
#  undef PyIter_Check
#  define PyIter_Check(obj) (FALSE)
# endif

స్థిర PyTypeObject* కూడు_కుప్ప_Type(నిర్మాణం typeobject_wrapper* type_object)
{
    PyType_Spec type_spec;
    type_spec.name = type_object->tp_name;
    type_spec.basicsize = type_object->tp_basicsize;
    type_spec.itemsize = 0;
    type_spec.flags = type_object->tp_flags;

    // We just need to statically allocate a large enough buffer that can hold
    // all slots. We need to leave a null-terminated slot at the end.
    Py_Type_స్థానం slots[40] = { {0, ఖాళీ} };
    size_t స్థానం_i = 0;

# define కూడు_TP_స్థానం(స్థానం_name) \
    ఒకవేళ (స్థానం_i >= 40) తిరిగి ఖాళీ; /* this should never happen */ \
    ఒకవేళ (type_object->స్థానం_name != ఖాళీ) \
    { \
	slots[స్థానం_i].స్థానం = Py_##slot_name; \
	slots[స్థానం_i].pfunc = (శూన్య*)type_object->స్థానం_name; \
	++స్థానం_i; \
    }
# define కూడు_TP_SUB_స్థానం(sub_స్థానం, స్థానం_name) \
    ఒకవేళ (స్థానం_i >= 40) తిరిగి ఖాళీ; /* this should never happen */ \
    ఒకవేళ (type_object->sub_స్థానం != ఖాళీ && type_object->sub_స్థానం->స్థానం_name != ఖాళీ) \
    { \
	slots[స్థానం_i].స్థానం = Py_##slot_name; \
	slots[స్థానం_i].pfunc = (శూన్య*)type_object->sub_స్థానం->స్థానం_name; \
	++స్థానం_i; \
    }

    కూడు_TP_స్థానం(tp_dealloc)
    కూడు_TP_స్థానం(tp_repr)
    కూడు_TP_స్థానం(tp_call)
    కూడు_TP_స్థానం(tp_getattro)
    కూడు_TP_స్థానం(tp_setattro)
    కూడు_TP_స్థానం(tp_doc)
    కూడు_TP_స్థానం(tp_traverse)
    కూడు_TP_స్థానం(tp_clear)
    కూడు_TP_స్థానం(tp_iter)
    కూడు_TP_స్థానం(tp_iternext)
    కూడు_TP_స్థానం(tp_methods)
    కూడు_TP_స్థానం(tp_పునాది)
    కూడు_TP_స్థానం(tp_alloc)
    కూడు_TP_స్థానం(tp_new)
    కూడు_TP_స్థానం(tp_free)

    కూడు_TP_SUB_స్థానం(tp_as_sequence, sq_నిడివి)
    కూడు_TP_SUB_స్థానం(tp_as_sequence, sq_concat)
    కూడు_TP_SUB_స్థానం(tp_as_sequence, sq_repeat)
    కూడు_TP_SUB_స్థానం(tp_as_sequence, sq_అంశం)
    కూడు_TP_SUB_స్థానం(tp_as_sequence, sq_ass_అంశం)
    కూడు_TP_SUB_స్థానం(tp_as_sequence, sq_కలిగియుంది)
    కూడు_TP_SUB_స్థానం(tp_as_sequence, sq_inplace_concat)
    కూడు_TP_SUB_స్థానం(tp_as_sequence, sq_inplace_repeat)

    కూడు_TP_SUB_స్థానం(tp_as_mapping, mp_నిడివి)
    కూడు_TP_SUB_స్థానం(tp_as_mapping, mp_subscript)
    కూడు_TP_SUB_స్థానం(tp_as_mapping, mp_ass_subscript)
# undef కూడు_TP_స్థానం
# undef కూడు_TP_SUB_స్థానం

    type_spec.slots = slots;

    PyObject* newtype = PyType_FromSpec(&type_spec);
    తిరిగి (PyTypeObject*)newtype;
}

// Add a heap type, since static types do not work in limited API
// Each PYTYPE_READY is paired with PYTYPE_CLEANUP.
//
// Note that we don't call Py_DECREF(type##Ptr) in clean up. The reason for
// that in 3.7, it's possible to de-allocate a heap type before all instances
// are cleared, leading to a crash, whereas in 3.8 the semantics were changed
// and instances hold strong references to types. Since these types are
// designed to be static, just keep them around to avoid having to write
// version-specific handling. Vim does not re-start the Python runtime so there
// will be no long-term leak.
# define PYTYPE_READY(type) \
    type##Ptr = కూడు_కుప్ప_Type(&(type)); \
    ఒకవేళ (type##Ptr == ఖాళీ) \
	తిరిగి -1;
# define PYTYPE_CLEANUP(type) \
    type##Ptr = ఖాళీ;

// Limited API does not provide PyRun_* functions. Need to implement manually
// using PyCompile and PyEval.
స్థిర PyObject* Vim_Py_నిరంతరం_String(స్థిరరాశి అక్షరం *str, పూర్ణాంకం ప్రారంభం, PyObject *globals, PyObject *locals)
{
    // Just pass "" for filename for now.
    PyObject* compiled = Py_CompileString(str, "", ప్రారంభం);
    ఒకవేళ (compiled == ఖాళీ)
	తిరిగి ఖాళీ;

    PyObject* eval_ఫలితం = Py_Eval_Eval_సంకేతం(compiled, globals, locals);
    Py_DECREF(compiled);
    తిరిగి eval_ఫలితం;
}
స్థిర పూర్ణాంకం Vim_Py_నిరంతరం_Simple_String(స్థిరరాశి అక్షరం *str)
{
    // This function emulates CPython's implementation.
    PyObject* m = Py_Import_కూడు_Module("__main__");
    ఒకవేళ (m == ఖాళీ)
	తిరిగి -1;
    PyObject* d = PyModule_GetDict(m);
    PyObject* output = Vim_Py_నిరంతరం_String(str, Py_file_ఆదానం, d, d);
    ఒకవేళ (output == ఖాళీ)
    {
	PyErr_PrintEx(TRUE);
	తిరిగి -1;
    }
    Py_DECREF(output);
    తిరిగి 0;
}
# define Py_నిరంతరం_String Vim_Py_నిరంతరం_String
# define Py_నిరంతరం_Simple_String Vim_Py_నిరంతరం_Simple_String

#లేకపోతే // !defined(USE_LIMITED_API)

// Full Python API. Can make use of structs and macros directly.
# define DEFINE_PY_TYPE_OBJECT(type) \
    స్థిర PyTypeObject type; \
    స్థిర PyTypeObject* type##Ptr = &type
# define Py_Object_తల_INIT_TYPE(type) Py_Object_తల_INIT(&type)

# define Py_TYPE_GET_TP_ALLOC(type) type->tp_alloc
# define Py_TYPE_GET_TP_METHODS(type) type->tp_methods

# define Py_TYPE_NAME(obj) ((obj)->ob_type->tp_name == ఖాళీ \
	? "(NULL)" \
	: (obj)->ob_type->tp_name)
# define PyErr_FORMAT_TYPE(సందేశం, obj) \
    PyErr_FORMAT(PyExc_TypeError, సందేశం, \
	    Py_TYPE_NAME(obj))

// Add a static type
# define PYTYPE_READY(type) \
    ఒకవేళ (PyType_Ready(type##Ptr)) \
	తిరిగి -1;

#అయితే_ముగింపు


#నిర్వచించు RAISE_NO_ఖాళీలు_కీలకాలు Py_Err_సమితి_STRING(Py_Exc_విలువ_Error, \
					    N_("empty keys are not allowed"))
#నిర్వచించు RAISE_LOCKED_DICTIONARY Py_Err_సమితి_VIM(N_("dictionary is locked"))
#నిర్వచించు RAISE_LOCKED_LIST Py_Err_సమితి_VIM(N_("list is locked"))
#నిర్వచించు RAISE_UNDO_FAIL Py_Err_సమితి_VIM(N_("cannot save undo information"))
#నిర్వచించు RAISE_DELETE_పంక్తి_FAIL Py_Err_సమితి_VIM(N_("cannot delete line"))
#నిర్వచించు RAISE_చొప్పించు_పంక్తి_FAIL Py_Err_సమితి_VIM(N_("cannot insert line"))
#నిర్వచించు RAISE_REPLACE_పంక్తి_FAIL Py_Err_సమితి_VIM(N_("cannot replace line"))
#నిర్వచించు RAISE_కీలకం_కూడు_FAIL(కీలకం) \
    PyErr_VIM_FORMAT(N_("failed to add key '%s' to dictionary"), కీలకం)
#నిర్వచించు RAISE_INVALID_సూచీ_TYPE(idx) \
    PyErr_FORMAT_TYPE(N_("index must be int or slice, not %s"), idx);

#నిర్వచించు INVALID_బఫరు_విలువ ((buf_T *)(-1))
#నిర్వచించు INVALID_కిటికీ_విలువ ((win_T *)(-1))
#నిర్వచించు INVALID_TABPAGE_విలువ ((tabpage_T *)(-1))

రకనిర్వచనం శూన్య (*rangeinitializer)(శూన్య *);
రకనిర్వచనం శూన్య (*runner)(స్థిరరాశి అక్షరం *, dict_T *, శూన్య *
#నిర్వచితమైతే PY_CAN_RECURSE
	, PyGILState_STATE *
#అయితే_ముగింపు
	);

స్థిర పూర్ణాంకం ConvertFromPyObject(PyObject *, typval_T *);
స్థిర పూర్ణాంకం _ConvertFromPyObject(PyObject *, typval_T *, PyObject *);
స్థిర పూర్ణాంకం ConvertFromPyMapping(PyObject *, typval_T *);
స్థిర పూర్ణాంకం ConvertFromPySequence(PyObject *, typval_T *);
స్థిర PyObject *కిటికీ_New(win_T *, tabpage_T *);
స్థిర PyObject *బఫరు_New (buf_T *);
స్థిర PyObject *పంక్తి_గమ్యం_String(స్థిరరాశి అక్షరం *);

స్థిర PyInt Range_ప్రారంభం;
స్థిర PyInt Range_చివర;

స్థిర PyObject *globals;

స్థిర PyObject *py_chdir;
స్థిర PyObject *py_fchdir;
స్థిర PyObject *py_getcwd;
స్థిర PyObject *vim_module;
స్థిర PyObject *vim_special_మార్గం_object;

#ఒకవేళ PY_సంస్కరణ_HEX >= 0x030700f0
స్థిర PyObject *py_కనుగొను_spec;
#లేకపోతే
స్థిర PyObject *py_ఎక్కించు_module;
#అయితే_ముగింపు
#ఒకవేళ PY_సంస్కరణ_HEX < 0x30c00a7
స్థిర PyObject *py_కనుగొను_module;
#అయితే_ముగింపు

స్థిర PyObject *VimError;

/*
 * obtain a lock on the Vim data structures
 */
    స్థిర శూన్య
Python_తాళం_Vim(శూన్య)
{
}

/*
 * release a lock on the Vim data structures
 */
    స్థిర శూన్య
Python_Release_Vim(శూన్య)
{
}

/*
 * The "todecref" argument holds a pointer to PyObject * that must be
 * DECREF'ed after returned char_u * is no longer needed or NULL if all what
 * was needed to generate returned value is object.
 *
 * Use Py_XDECREF to decrement reference count.
 */
    స్థిర char_u *
String_గమ్యం_Chars(PyObject *obj, PyObject **todecref)
{
    char_u	*str;

    ఒకవేళ (Py_బైట్లు_Check(obj))
    {

	ఒకవేళ (Py_బైట్లు_As_String_And_కొలత(obj, (అక్షరం **) &str, ఖాళీ) == -1
		|| str == ఖాళీ)
	    తిరిగి ఖాళీ;

	*todecref = ఖాళీ;
    }
    లేకపోతే ఒకవేళ (PyUnicode_Check(obj))
    {
	PyObject	*బైట్లు;

	ఒకవేళ (!(బైట్లు = PyUnicode_AsEncodedString(obj, ENC_OPT,
							   ERRORS_ENCODE_ARG)))
	    తిరిగి ఖాళీ;

	ఒకవేళ (Py_బైట్లు_As_String_And_కొలత(బైట్లు, (అక్షరం **) &str, ఖాళీ) == -1
		|| str == ఖాళీ)
	{
	    Py_DECREF(బైట్లు);
	    తిరిగి ఖాళీ;
	}

	*todecref = బైట్లు;
    }
    లేకపోతే
    {
#ఒకవేళ PY_MAJOR_సంస్కరణ < 3
	PyErr_FORMAT_TYPE(N_("expected str() or unicode() instance, but got %s"),
		obj);
#లేకపోతే
	PyErr_FORMAT_TYPE(N_("expected bytes() or str() instance, but got %s"),
		obj);
#అయితే_ముగింపు
	తిరిగి ఖాళీ;
    }

    తిరిగి (char_u *) str;
}

#నిర్వచించు సంఖ్య_LONG     1
#నిర్వచించు సంఖ్య_INT      2
#నిర్వచించు సంఖ్య_NATURAL  4
#నిర్వచించు సంఖ్య_UNSIGNED 8

    స్థిర పూర్ణాంకం
సంఖ్య_గమ్యం_Long(PyObject *obj, దీర్ఘ *ఫలితం, పూర్ణాంకం flags)
{
#ఒకవేళ PY_MAJOR_సంస్కరణ < 3
    ఒకవేళ (PyInt_Check(obj))
    {
	*ఫలితం = PyInt_AsLong(obj);
	ఒకవేళ (PyErr_Occurred())
	    తిరిగి -1;
    }
    లేకపోతే
#అయితే_ముగింపు
    ఒకవేళ (PyLong_Check(obj))
    {
	*ఫలితం = PyLong_AsLong(obj);
	ఒకవేళ (PyErr_Occurred())
	    తిరిగి -1;
    }
    లేకపోతే ఒకవేళ (Py_సంఖ్య_Check(obj))
    {
	PyObject	*అంకె;

	ఒకవేళ (!(అంకె = Py_సంఖ్య_Long(obj)))
	    తిరిగి -1;

	*ఫలితం = PyLong_AsLong(అంకె);

	Py_DECREF(అంకె);

	ఒకవేళ (PyErr_Occurred())
	    తిరిగి -1;
    }
    లేకపోతే
    {
#ఒకవేళ PY_MAJOR_సంస్కరణ < 3
	PyErr_FORMAT_TYPE(
		N_("expected int(), long() or something supporting "
		   "coercing to long(), but got %s"),
		obj);
#లేకపోతే
	PyErr_FORMAT_TYPE(
		N_("expected int() or something supporting coercing to int(), "
		   "but got %s"),
		obj);
#అయితే_ముగింపు
	తిరిగి -1;
    }

    ఒకవేళ (flags & సంఖ్య_INT)
    {
	ఒకవేళ (*ఫలితం > గరిష్ఠ_పూర్ణాంకం)
	{
	    Py_Err_సమితి_STRING(Py_Exc_అధికప్రవాహం_Error,
		    N_("value is too large to fit into C int type"));
	    తిరిగి -1;
	}
	లేకపోతే ఒకవేళ (*ఫలితం < INT_MIN)
	{
	    Py_Err_సమితి_STRING(Py_Exc_అధికప్రవాహం_Error,
		    N_("value is too small to fit into C int type"));
	    తిరిగి -1;
	}
    }

    ఒకవేళ (flags & సంఖ్య_NATURAL)
    {
	ఒకవేళ (*ఫలితం <= 0)
	{
	    Py_Err_సమితి_STRING(Py_Exc_విలువ_Error,
		    N_("number must be greater than zero"));
	    తిరిగి -1;
	}
    }
    లేకపోతే ఒకవేళ (flags & సంఖ్య_UNSIGNED)
    {
	ఒకవేళ (*ఫలితం < 0)
	{
	    Py_Err_సమితి_STRING(Py_Exc_విలువ_Error,
		    N_("number must be greater or equal to zero"));
	    తిరిగి -1;
	}
    }

    తిరిగి 0;
}

    స్థిర పూర్ణాంకం
కూడు_string(PyObject *list, అక్షరం *s)
{
    PyObject	*string;

    ఒకవేళ (!(string = PyString_FromString(s)))
	తిరిగి -1;

    ఒకవేళ (PyList_Append(list, string))
    {
	Py_DECREF(string);
	తిరిగి -1;
    }

    Py_DECREF(string);
    తిరిగి 0;
}

    స్థిర PyObject *
ObjectDir(PyObject *self, అక్షరం **లక్షణాలు)
{
    Py_పద్ధతి_Def	*పద్ధతి;
    అక్షరం	**attr;
    PyObject	*ret;

    ఒకవేళ (!(ret = PyList_New(0)))
	తిరిగి ఖాళీ;

    ఒకవేళ (self)
	కొరకు (పద్ధతి = Py_TYPE_GET_TP_METHODS(self->ob_type) ; పద్ధతి->ml_name != ఖాళీ ; ++పద్ధతి)
	    ఒకవేళ (కూడు_string(ret, (అక్షరం *)పద్ధతి->ml_name))
	    {
		Py_DECREF(ret);
		తిరిగి ఖాళీ;
	    }

    కొరకు (attr = లక్షణాలు ; *attr ; ++attr)
	ఒకవేళ (కూడు_string(ret, *attr))
	{
	    Py_DECREF(ret);
	    తిరిగి ఖాళీ;
	}

#ఒకవేళ PY_MAJOR_సంస్కరణ < 3
    ఒకవేళ (కూడు_string(ret, "__members__"))
    {
	Py_DECREF(ret);
	తిరిగి ఖాళీ;
    }
#అయితే_ముగింపు

    తిరిగి ret;
}

// Output buffer management

// Function to write a line, points to either msg() or emsg().
రకనిర్వచనం పూర్ణాంకం (*writefn)(అక్షరం *);

DEFINE_PY_TYPE_OBJECT(OutputType);

రకనిర్వచనం నిర్మాణం
{
    Py_Object_తల
    దీర్ఘ softspace;
    దీర్ఘ error;
} OutputObject;

స్థిర అక్షరం *OutputAttrs[] = {
    "softspace",
    ఖాళీ
};

    స్థిర PyObject *
OutputDir(PyObject *self, PyObject *వాదనలు వాడని)
{
    తిరిగి ObjectDir(self, OutputAttrs);
}

    స్థిర పూర్ణాంకం
OutputSetattr(PyObject *self_obj, అక్షరం *name, PyObject *valObject)
{
    OutputObject *self = (OutputObject*)self_obj;
    ఒకవేళ (valObject == ఖాళీ)
    {
	Py_Err_సమితి_STRING(PyExc_AttributeError,
		N_("can't delete OutputObject attributes"));
	తిరిగి -1;
    }

    ఒకవేళ (strcmp(name, "softspace") == 0)
    {
	ఒకవేళ (సంఖ్య_గమ్యం_Long(valObject, &(self->softspace), సంఖ్య_UNSIGNED))
	    తిరిగి -1;
	తిరిగి 0;
    }

    PyErr_FORMAT(PyExc_AttributeError, N_("invalid attribute: %s"), name);
    తిరిగి -1;
}

// Buffer IO, we write one whole line at a time.
స్థిర garray_T io_ga = {0, 0, 1, 80, ఖాళీ};
స్థిర writefn old_fn = ఖాళీ;

    స్థిర శూన్య
PythonIO_Flush(శూన్య)
{
    ఒకవేళ (old_fn != ఖాళీ && io_ga.ga_len > 0)
    {
	((అక్షరం *)io_ga.ga_దత్తాంశం)[io_ga.ga_len] = NUL;
	// We don't know what emsg_severe should be here, so ... hope?
	old_fn((అక్షరం *)io_ga.ga_దత్తాంశం);
    }
    io_ga.ga_len = 0;
}

    స్థిర శూన్య
writer(writefn fn, char_u *str, PyInt n, పూర్ణాంకం severe)
{
    char_u *ptr;

    // Flush when switching output function.
    ఒకవేళ (fn != old_fn)
	PythonIO_Flush();
    old_fn = fn;

    // Write each NL separated line.  Text after the last NL is kept for
    // writing later.
    // For normal messages: Do not output when "got_int" was set.  This avoids
    // a loop gone crazy flooding the terminal with messages.  Also for when
    // "q" is pressed at the more-prompt.
    అయితే (n > 0 && (ptr = memchr(str, '\n', n)) != ఖాళీ
					  && (fn == (writefn)emsg || !got_int))
    {
	PyInt len = ptr - str;

	ఒకవేళ (ga_grow(&io_ga, (పూర్ణాంకం)(len + 1)) == FAIL)
	    ఆపు;

	mch_memmove(((అక్షరం *)io_ga.ga_దత్తాంశం) + io_ga.ga_len, str, (size_t)len);
	((అక్షరం *)io_ga.ga_దత్తాంశం)[io_ga.ga_len + len] = NUL;
	emsg_severe = severe;
	fn((అక్షరం *)io_ga.ga_దత్తాంశం);
	str = ptr + 1;
	n -= len + 1;
	io_ga.ga_len = 0;
    }

    // Put the remaining text into io_ga for later printing.
    ఒకవేళ (n > 0 && (fn == (writefn)emsg || !got_int)
					&& ga_grow(&io_ga, (పూర్ణాంకం)(n + 1)) == OK)
    {
	mch_memmove(((అక్షరం *)io_ga.ga_దత్తాంశం) + io_ga.ga_len, str, (size_t)n);
	io_ga.ga_len += (పూర్ణాంకం)n;
    }
}

    స్థిర పూర్ణాంకం
write_output(OutputObject *self, PyObject *string)
{
    Py_ssize_t	len = 0;
    అక్షరం	*str = ఖాళీ;
    పూర్ణాంకం		error = self->error;

    ఒకవేళ (!PyArg_Parse(string, "et#", ENC_OPT, &str, &len))
	తిరిగి -1;

    Py_BEGIN_ALLOW_దారాలు
    Python_తాళం_Vim();
    writer((writefn)(error ? emsg : సందేశం), (char_u *)str, len, error);
    Python_Release_Vim();
    Py_చివర_ALLOW_దారాలు
    PyMem_Free(str);

    తిరిగి 0;
}

    స్థిర PyObject *
OutputWrite(OutputObject *self, PyObject *string)
{
    ఒకవేళ (write_output(self, string))
	తిరిగి ఖాళీ;

    Py_INCREF(Py_None);
    తిరిగి Py_None;
}

    స్థిర PyObject *
OutputWritelines(OutputObject *self, PyObject *seq)
{
    PyObject	*iterator;
    PyObject	*అంశం;

    ఒకవేళ (!(iterator = PyObject_GetIter(seq)))
	తిరిగి ఖాళీ;

    అయితే ((అంశం = Py_Iter_తదుపరి(iterator)))
    {
	ఒకవేళ (write_output(self, అంశం))
	{
	    Py_DECREF(iterator);
	    Py_DECREF(అంశం);
	    తిరిగి ఖాళీ;
	}
	Py_DECREF(అంశం);
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    ఒకవేళ (PyErr_Occurred())
	తిరిగి ఖాళీ;

    Py_INCREF(Py_None);
    తిరిగి Py_None;
}

    స్థిర PyObject *
AlwaysNone(PyObject *self వాడని, PyObject *వాదనలు వాడని)
{
    // do nothing
    Py_INCREF(Py_None);
    తిరిగి Py_None;
}
#నిర్వచించు ALWAYS_NONE AlwaysNone(ఖాళీ, ఖాళీ)

    స్థిర PyObject *
AlwaysFalse(PyObject *self వాడని, PyObject *వాదనలు వాడని)
{
    // do nothing
    PyObject	*ret = Py_False;
    Py_INCREF(ret);
    తిరిగి ret;
}
#నిర్వచించు ALWAYS_FALSE AlwaysFalse(ఖాళీ, ఖాళీ)

    స్థిర PyObject *
AlwaysTrue(PyObject *self వాడని, PyObject *వాదనలు వాడని)
{
    // do nothing
    PyObject	*ret = Py_True;
    Py_INCREF(ret);
    తిరిగి ret;
}
#నిర్వచించు ALWAYS_TRUE AlwaysTrue(ఖాళీ, ఖాళీ)

/***************/

స్థిర నిర్మాణం Py_పద్ధతి_Def OutputMethods[] = {
    // name,	    function,				calling,	doc
    {"write",	    (PyCFunction)OutputWrite,		METH_O,		""},
    {"writelines",  (PyCFunction)OutputWritelines,	METH_O,		""},
    {"flush",	    (PyCFunction)AlwaysNone,		METH_NOARGS,	""},
    {"close",	    (PyCFunction)AlwaysNone,		METH_NOARGS,	""},
    {"isatty",	    (PyCFunction)AlwaysFalse,		METH_NOARGS,	""},
    {"readable",    (PyCFunction)AlwaysFalse,		METH_NOARGS,	""},
    {"seekable",    (PyCFunction)AlwaysFalse,		METH_NOARGS,	""},
    {"writable",    (PyCFunction)AlwaysTrue,		METH_NOARGS,	""},
    {"closed",      (PyCFunction)AlwaysFalse,		METH_NOARGS,	""},
    {"__dir__",	    (PyCFunction)OutputDir,		METH_NOARGS,	""},
    { ఖాళీ,	    ఖాళీ,				0,		ఖాళీ}
};

స్థిర OutputObject Output =
{
    Py_Object_తల_INIT_TYPE(OutputType)
    0,
    0
};

స్థిర OutputObject Error =
{
    Py_Object_తల_INIT_TYPE(OutputType)
    0,
    1
};

    స్థిర పూర్ణాంకం
PythonIO_Init_io(శూన్య)
{
    ఒకవేళ (Py_Sys_సమితి_Object("stdout", (PyObject *)(శూన్య *)&Output))
	తిరిగి -1;
    ఒకవేళ (Py_Sys_సమితి_Object("stderr", (PyObject *)(శూన్య *)&Error))
	తిరిగి -1;

    ఒకవేళ (PyErr_Occurred())
    {
	emsg(_(e_python_error_initialising_io_object));
	తిరిగి -1;
    }

    తిరిగి 0;
}

#ఒకవేళ PY_సంస్కరణ_HEX < 0x030700f0
స్థిర PyObject *call_ఎక్కించు_module(అక్షరం *name, పూర్ణాంకం len, PyObject *కనుగొను_module_ఫలితం);

రకనిర్వచనం నిర్మాణం
{
    Py_Object_తల
    అక్షరం	*fullname;
    PyObject	*ఫలితం;
} LoaderObject;
DEFINE_PY_TYPE_OBJECT(LoaderType);

    స్థిర శూన్య
LoaderDestructor(PyObject *self_obj)
{
    LoaderObject *self = (LoaderObject*)self_obj;
    vim_free(self->fullname);
    Py_XDECREF(self->ఫలితం);
    DESTRUCTOR_ముగింపు(self);
}

    స్థిర PyObject *
Loader_ఎక్కించు_Module(LoaderObject *self, PyObject *వాదనలు వాడని)
{
    అక్షరం	*fullname = self->fullname;
    PyObject	*ఫలితం = self->ఫలితం;
    PyObject	*module;

    ఒకవేళ (!fullname)
    {
	module = ఫలితం ? ఫలితం : Py_None;
	Py_INCREF(module);
	తిరిగి module;
    }

    module = call_ఎక్కించు_module(fullname, (పూర్ణాంకం)STRLEN(fullname), ఫలితం);

    self->fullname = ఖాళీ;
    self->ఫలితం = module;

    vim_free(fullname);
    Py_DECREF(ఫలితం);

    ఒకవేళ (!module)
    {
	ఒకవేళ (PyErr_Occurred())
	    తిరిగి ఖాళీ;

	Py_INCREF(Py_None);
	తిరిగి Py_None;
    }

    Py_INCREF(module);
    తిరిగి module;
}

స్థిర నిర్మాణం Py_పద్ధతి_Def LoaderMethods[] = {
    // name,	    function,				calling,	doc
    {"load_module", (PyCFunction)Loader_ఎక్కించు_Module,	METH_VARARGS,	""},
    { ఖాళీ,	    ఖాళీ,				0,		ఖాళీ}
};
#అయితే_ముగింపు

/*
 * Check to see whether a Vim error has been reported, or a keyboard
 * interrupt has been detected.
 */
    స్థిర శూన్య
Vim_Try_ప్రారంభం(శూన్య)
{
    ++trylevel;
}

    స్థిర పూర్ణాంకం
Vim_Try_చివర(శూన్య)
{
    --trylevel;
    // Without this it stops processing all subsequent Vim script commands and
    // generates strange error messages if I e.g. try calling Test() in a cycle
    did_emsg = FALSE;
    // Keyboard interrupt should be preferred over anything else
    ఒకవేళ (got_int)
    {
	ఒకవేళ (did_throw)
	    discard_current_exception();
	got_int = FALSE;
	Py_Err_సమితి_None(Py_Exc_Keyboard_అంతరాయం);
	తిరిగి -1;
    }
    లేకపోతే ఒకవేళ (సందేశం_list != ఖాళీ && *సందేశం_list != ఖాళీ)
    {
	పూర్ణాంకం	should_free;
	అక్షరం	*సందేశం;

	సందేశం = get_exception_string(*సందేశం_list, ET_ERROR, ఖాళీ, &should_free);

	ఒకవేళ (సందేశం == ఖాళీ)
	{
	    Py_Err_No_జ్ఞాపకం();
	    తిరిగి -1;
	}

	Py_Err_సమితి_Vim(సందేశం);

	free_global_msglist();

	ఒకవేళ (should_free)
	    vim_free(సందేశం);

	తిరిగి -1;
    }
    లేకపోతే ఒకవేళ (!did_throw)
	తిరిగి (PyErr_Occurred() ? -1 : 0);
    // Python exception is preferred over Vim one; unlikely to occur though
    లేకపోతే ఒకవేళ (PyErr_Occurred())
    {
	discard_current_exception();
	తిరిగి -1;
    }
    // Finally transform Vim script exception to python one
    లేకపోతే
    {
	Py_Err_సమితి_Vim((అక్షరం *)current_exception->విలువ);
	discard_current_exception();
	తిరిగి -1;
    }
}

    స్థిర పూర్ణాంకం
Vim_Check_అంతరాయం(శూన్య)
{
    ఒకవేళ (got_int)
    {
	Py_Err_సమితి_None(Py_Exc_Keyboard_అంతరాయం);
	తిరిగి 1;
    }
    తిరిగి 0;
}

// Vim module - Implementation

    స్థిర PyObject *
VimCommand(PyObject *self వాడని, PyObject *string)
{
    char_u	*cmd;
    PyObject	*ret;
    PyObject	*todecref;

    ఒకవేళ (!(cmd = String_గమ్యం_Chars(string, &todecref)))
	తిరిగి ఖాళీ;

    Py_BEGIN_ALLOW_దారాలు
    Python_తాళం_Vim();

    Vim_Try_ప్రారంభం();
    do_cmdline_cmd(cmd);
    update_screen(UPD_VALID);

    Python_Release_Vim();
    Py_చివర_ALLOW_దారాలు

    ఒకవేళ (Vim_Try_చివర())
	ret = ఖాళీ;
    లేకపోతే
	ret = Py_None;

    Py_XINCREF(ret);
    Py_XDECREF(todecref);
    తిరిగి ret;
}

/*
 * Function to translate a typval_T into a PyObject; this will recursively
 * translate lists/dictionaries into their Python equivalents.
 *
 * The depth parameter is to avoid infinite recursion, set it to 1 when
 * you call VimToPython.
 */
    స్థిర PyObject *
Vim_గమ్యం_Python(typval_T *our_tv, పూర్ణాంకం లోతు, PyObject *lookup_dict)
{
    PyObject	*ret;
    PyObject	*newObj;
    అక్షరం	ptrBuf[పరిమాణం(శూన్య *) * 2 + 3];

    // Avoid infinite recursion
    ఒకవేళ (లోతు > 100)
    {
	Py_INCREF(Py_None);
	ret = Py_None;
	తిరిగి ret;
    }

    // Check if we run into a recursive loop.  The item must be in lookup_dict
    // then and we can use it again.
    ఒకవేళ ((our_tv->v_type == VAR_LIST && our_tv->vval.v_list != ఖాళీ)
	    || (our_tv->v_type == VAR_TUPLE && our_tv->vval.v_tuple != ఖాళీ)
	    || (our_tv->v_type == VAR_DICT && our_tv->vval.v_dict != ఖాళీ))
    {
	sprintf(ptrBuf, "%p",
		our_tv->v_type == VAR_LIST ? (శూన్య *)our_tv->vval.v_list
		    : our_tv->v_type == VAR_TUPLE ?
			(శూన్య *)our_tv->vval.v_tuple
			: (శూన్య *)our_tv->vval.v_dict);

	ఒకవేళ ((ret = Py_Dict_Get_అంశం_String(lookup_dict, ptrBuf)))
	{
	    Py_INCREF(ret);
	    తిరిగి ret;
	}
    }

    ఒకవేళ (our_tv->v_type == VAR_STRING)
	ret = PyString_FromString(our_tv->vval.v_string == ఖాళీ
					? "" : (అక్షరం *)our_tv->vval.v_string);
    లేకపోతే ఒకవేళ (our_tv->v_type == VAR_సంఖ్య)
    {
	అక్షరం buf[NUMBUFLEN];

	// For backwards compatibility numbers are stored as strings.
	sprintf(buf, "%lld", (దీర్ఘ దీర్ఘ)our_tv->vval.v_సంఖ్య);
	ret = PyString_FromString((అక్షరం *)buf);
    }
    లేకపోతే ఒకవేళ (our_tv->v_type == VAR_FLOAT)
    {
	అక్షరం buf[NUMBUFLEN];

	sprintf(buf, "%f", our_tv->vval.v_float);
	ret = PyString_FromString((అక్షరం *)buf);
    }
    లేకపోతే ఒకవేళ (our_tv->v_type == VAR_LIST)
    {
	list_T		*list = our_tv->vval.v_list;
	listitem_T	*curr;

	ఒకవేళ (list == ఖాళీ)
	    తిరిగి ఖాళీ;

	ఒకవేళ (!(ret = PyList_New(0)))
	    తిరిగి ఖాళీ;

	ఒకవేళ (Py_Dict_సమితి_అంశం_String(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    తిరిగి ఖాళీ;
	}

	CHECK_LIST_MATERIALIZE(list);
	FOR_ALL_LIST_వస్తువులు(list, curr)
	{
	    ఒకవేళ (!(newObj = Vim_గమ్యం_Python(&curr->li_tv, లోతు + 1, lookup_dict)))
	    {
		Py_DECREF(ret);
		తిరిగి ఖాళీ;
	    }
	    ఒకవేళ (PyList_Append(ret, newObj))
	    {
		Py_DECREF(newObj);
		Py_DECREF(ret);
		తిరిగి ఖాళీ;
	    }
	    Py_DECREF(newObj);
	}
    }
    లేకపోతే ఒకవేళ (our_tv->v_type == VAR_TUPLE)
    {
	tuple_T	*tuple = our_tv->vval.v_tuple;
	పూర్ణాంకం	len;

	ఒకవేళ (tuple == ఖాళీ)
	    తిరిగి ఖాళీ;

	len = TUPLE_LEN(tuple);

	ret = PyTuple_New(len);
	ఒకవేళ (ret == ఖాళీ)
	    తిరిగి ఖాళీ;

	కొరకు (పూర్ణాంకం idx = 0; idx < len; idx++)
	{
	    typval_T	*అంశం_tv = TUPLE_అంశం(tuple, idx);

	    newObj = Vim_గమ్యం_Python(అంశం_tv, లోతు + 1, lookup_dict);
	    ఒకవేళ (!newObj)
	    {
		Py_DECREF(ret);
		తిరిగి ఖాళీ;
	    }
	    Py_Tuple_సమితి_అంశం(ret, idx, newObj);
	}

	ఒకవేళ (Py_Dict_సమితి_అంశం_String(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    తిరిగి ఖాళీ;
	}
    }
    లేకపోతే ఒకవేళ (our_tv->v_type == VAR_DICT)
    {

	hashtab_T	*ht;
	long_u		todo;
	hashitem_T	*hi;
	dictitem_T	*di;

	ఒకవేళ (our_tv->vval.v_dict == ఖాళీ)
	    తిరిగి ఖాళీ;
	ht = &our_tv->vval.v_dict->dv_hashtab;

	ఒకవేళ (!(ret = PyDict_New()))
	    తిరిగి ఖాళీ;

	ఒకవేళ (Py_Dict_సమితి_అంశం_String(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    తిరిగి ఖాళీ;
	}

	todo = ht->ht_వాడిన;
	కొరకు (hi = ht->ht_array; todo > 0; ++hi)
	{
	    ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
	    {
		--todo;

		di = dict_lookup(hi);
		ఒకవేళ (!(newObj = Vim_గమ్యం_Python(&di->di_tv, లోతు + 1, lookup_dict)))
		{
		    Py_DECREF(ret);
		    తిరిగి ఖాళీ;
		}
		ఒకవేళ (Py_Dict_సమితి_అంశం_String(ret, (అక్షరం *)hi->hi_కీలకం, newObj))
		{
		    Py_DECREF(ret);
		    Py_DECREF(newObj);
		    తిరిగి ఖాళీ;
		}
	    }
	}
    }
    లేకపోతే ఒకవేళ (our_tv->v_type == VAR_BOOL)
    {
	ఒకవేళ (our_tv->vval.v_సంఖ్య == VVAL_FALSE)
	{
	    ret = Py_False;
	    Py_INCREF(ret);
	}
	లేకపోతే
	{
	    ret = Py_True;
	    Py_INCREF(ret);
	}
	తిరిగి ret;
    }
    లేకపోతే ఒకవేళ (our_tv->v_type == VAR_SPECIAL)
    {
	Py_INCREF(Py_None);
	ret = Py_None;
	తిరిగి ret;
    }
    లేకపోతే ఒకవేళ (our_tv->v_type == VAR_BLOB)
	ret = Py_బైట్లు_From_String_And_కొలత(
		(అక్షరం*) our_tv->vval.v_blob->bv_ga.ga_దత్తాంశం,
		(Py_ssize_t) our_tv->vval.v_blob->bv_ga.ga_len);
    లేకపోతే
    {
	Py_INCREF(Py_None);
	ret = Py_None;
    }

    తిరిగి ret;
}

    స్థిర PyObject *
VimEval(PyObject *self వాడని, PyObject *వాదనలు)
{
    char_u	*సమాసం;
    typval_T	*our_tv;
    PyObject	*string;
    PyObject	*todecref;
    PyObject	*ret;
    PyObject	*lookup_dict;

    ఒకవేళ (!PyArg_ParseTuple(వాదనలు, "O", &string))
	తిరిగి ఖాళీ;

    ఒకవేళ (!(సమాసం = String_గమ్యం_Chars(string, &todecref)))
	తిరిగి ఖాళీ;

    Py_BEGIN_ALLOW_దారాలు
    Python_తాళం_Vim();
    Vim_Try_ప్రారంభం();
    our_tv = eval_సమాసం(సమాసం, ఖాళీ);
    Python_Release_Vim();
    Py_చివర_ALLOW_దారాలు

    Py_XDECREF(todecref);

    ఒకవేళ (Vim_Try_చివర())
	తిరిగి ఖాళీ;

    ఒకవేళ (our_tv == ఖాళీ)
    {
	Py_Err_సమితి_VIM(N_("invalid expression"));
	తిరిగి ఖాళీ;
    }

    // Convert the Vim type into a Python type.  Create a dictionary that's
    // used to check for recursive loops.
    ఒకవేళ (!(lookup_dict = PyDict_New()))
	ret = ఖాళీ;
    లేకపోతే
    {
	ret = Vim_గమ్యం_Python(our_tv, 1, lookup_dict);
	Py_DECREF(lookup_dict);
    }


    Py_BEGIN_ALLOW_దారాలు
    Python_తాళం_Vim();
    free_tv(our_tv);
    Python_Release_Vim();
    Py_చివర_ALLOW_దారాలు

    తిరిగి ret;
}

స్థిర PyObject *Convert_గమ్యం_Py_Object(typval_T *);

    స్థిర PyObject *
VimEvalPy(PyObject *self వాడని, PyObject *string)
{
    typval_T	*our_tv;
    PyObject	*ret;
    char_u	*సమాసం;
    PyObject	*todecref;

    ఒకవేళ (!(సమాసం = String_గమ్యం_Chars(string, &todecref)))
	తిరిగి ఖాళీ;

    Py_BEGIN_ALLOW_దారాలు
    Python_తాళం_Vim();
    Vim_Try_ప్రారంభం();
    our_tv = eval_సమాసం(సమాసం, ఖాళీ);
    Python_Release_Vim();
    Py_చివర_ALLOW_దారాలు

    Py_XDECREF(todecref);

    ఒకవేళ (Vim_Try_చివర())
	తిరిగి ఖాళీ;

    ఒకవేళ (our_tv == ఖాళీ)
    {
	Py_Err_సమితి_VIM(N_("invalid expression"));
	తిరిగి ఖాళీ;
    }

    ret = Convert_గమ్యం_Py_Object(our_tv);
    Py_BEGIN_ALLOW_దారాలు
    Python_తాళం_Vim();
    free_tv(our_tv);
    Python_Release_Vim();
    Py_చివర_ALLOW_దారాలు

    తిరిగి ret;
}

    స్థిర PyObject *
VimStrwidth(PyObject *self వాడని, PyObject *string)
{
    char_u	*str;
    PyObject	*todecref;
    పూర్ణాంకం		len;

    ఒకవేళ (!(str = String_గమ్యం_Chars(string, &todecref)))
	తిరిగి ఖాళీ;

    len = mb_string2cells(str, (పూర్ణాంకం)STRLEN(str));

    Py_XDECREF(todecref);

    తిరిగి PyLong_FromLong(len);
}

    స్థిర PyObject *
_VimChdir(PyObject *_chdir, PyObject *వాదనలు, PyObject *kwargs)
{
    PyObject	*ret;
    PyObject	*newwd;
    PyObject	*todecref;
    char_u	*new_dir;

    ఒకవేళ (_chdir == ఖాళీ)
	తిరిగి ఖాళీ;
    ఒకవేళ (!(ret = PyObject_Call(_chdir, వాదనలు, kwargs)))
	తిరిగి ఖాళీ;

    ఒకవేళ (!(newwd = Py_Object_Call_Function_Obj_వాదనలు(py_getcwd, ఖాళీ)))
    {
	Py_DECREF(ret);
	తిరిగి ఖాళీ;
    }

    ఒకవేళ (!(new_dir = String_గమ్యం_Chars(newwd, &todecref)))
    {
	Py_DECREF(ret);
	Py_DECREF(newwd);
	తిరిగి ఖాళీ;
    }

    Vim_Try_ప్రారంభం();

    ఒకవేళ (vim_chdir(new_dir))
    {
	Py_DECREF(ret);
	Py_DECREF(newwd);
	Py_XDECREF(todecref);

	ఒకవేళ (Vim_Try_చివర())
	    తిరిగి ఖాళీ;

	Py_Err_సమితి_VIM(N_("failed to change directory"));
	తిరిగి ఖాళీ;
    }

    Py_DECREF(newwd);
    Py_XDECREF(todecref);

    post_chdir(CDSCOPE_GLOBAL);

    ఒకవేళ (Vim_Try_చివర())
    {
	Py_DECREF(ret);
	తిరిగి ఖాళీ;
    }

    తిరిగి ret;
}

    స్థిర PyObject *
VimChdir(PyObject *self వాడని, PyObject *వాదనలు, PyObject *kwargs)
{
    తిరిగి _VimChdir(py_chdir, వాదనలు, kwargs);
}

    స్థిర PyObject *
VimFchdir(PyObject *self వాడని, PyObject *వాదనలు, PyObject *kwargs)
{
    తిరిగి _VimChdir(py_fchdir, వాదనలు, kwargs);
}

రకనిర్వచనం నిర్మాణం {
    PyObject	*callable;
    PyObject	*ఫలితం;
} map_rtp_దత్తాంశం;

    స్థిర శూన్య
map_rtp_callback(char_u *మార్గం, శూన్య *దత్తాంశం_3)
{
    శూన్య	**దత్తాంశం = (శూన్య **) దత్తాంశం_3;
    PyObject	*మార్గం_Object;
    map_rtp_దత్తాంశం	*mr_దత్తాంశం = *((map_rtp_దత్తాంశం **) దత్తాంశం);

    ఒకవేళ (!(మార్గం_Object = PyString_FromString((అక్షరం *)మార్గం)))
    {
	*దత్తాంశం = ఖాళీ;
	తిరిగి;
    }

    mr_దత్తాంశం->ఫలితం = Py_Object_Call_Function_Obj_వాదనలు(mr_దత్తాంశం->callable,
						   మార్గం_Object, ఖాళీ);

    Py_DECREF(మార్గం_Object);

    ఒకవేళ (!mr_దత్తాంశం->ఫలితం || mr_దత్తాంశం->ఫలితం != Py_None)
	*దత్తాంశం = ఖాళీ;
    లేకపోతే
    {
	Py_DECREF(mr_దత్తాంశం->ఫలితం);
	mr_దత్తాంశం->ఫలితం = ఖాళీ;
    }
}

    స్థిర PyObject *
VimForeachRTP(PyObject *self వాడని, PyObject *callable)
{
    map_rtp_దత్తాంశం	దత్తాంశం;

    దత్తాంశం.callable = callable;
    దత్తాంశం.ఫలితం = ఖాళీ;

    do_లోపల_runtimepath(ఖాళీ, 0, &map_rtp_callback, &దత్తాంశం);

    ఒకవేళ (దత్తాంశం.ఫలితం == ఖాళీ)
    {
	ఒకవేళ (PyErr_Occurred())
	    తిరిగి ఖాళీ;
	లేకపోతే
	{
	    Py_INCREF(Py_None);
	    తిరిగి Py_None;
	}
    }
    తిరిగి దత్తాంశం.ఫలితం;
}

/*
 * _vim_runtimepath_ special path implementation.
 */

    స్థిర శూన్య
map_finder_callback(char_u *మార్గం, శూన్య *దత్తాంశం_3)
{
    శూన్య	**దత్తాంశం = (శూన్య **) దత్తాంశం_3;
    PyObject	*list = *((PyObject **) దత్తాంశం);
    PyObject	*మార్గం_Object1, *మార్గం_Object2;
    అక్షరం	*pathbuf;
    size_t	pathlen;

    pathlen = STRLEN(మార్గం);

#ఒకవేళ PY_MAJOR_సంస్కరణ < 3
# define PY_MAIN_DIR_STRING "python2"
#లేకపోతే
# define PY_MAIN_DIR_STRING "python3"
#అయితే_ముగింపు
#నిర్వచించు PY_ALTERNATE_DIR_STRING "pythonx"

#నిర్వచించు PYTHONX_STRING_నిడివి 7 // STRLEN("pythonx")
    ఒకవేళ (!(pathbuf = PyMem_New(అక్షరం,
		    pathlen + STRLEN(PATHSEPSTR) + PYTHONX_STRING_నిడివి + 1)))
    {
	Py_Err_No_జ్ఞాపకం();
	*దత్తాంశం = ఖాళీ;
	తిరిగి;
    }

    mch_memmove(pathbuf, మార్గం, pathlen + 1);
    కూడు_pathsep((char_u *) pathbuf);

    pathlen = STRLEN(pathbuf);
    mch_memmove(pathbuf + pathlen, PY_MAIN_DIR_STRING,
	    PYTHONX_STRING_నిడివి + 1);

    ఒకవేళ (!(మార్గం_Object1 = PyString_FromString(pathbuf)))
    {
	*దత్తాంశం = ఖాళీ;
	PyMem_Free(pathbuf);
	తిరిగి;
    }

    mch_memmove(pathbuf + pathlen, PY_ALTERNATE_DIR_STRING,
	    PYTHONX_STRING_నిడివి + 1);

    ఒకవేళ (!(మార్గం_Object2 = PyString_FromString(pathbuf)))
    {
	Py_DECREF(మార్గం_Object1);
	PyMem_Free(pathbuf);
	*దత్తాంశం = ఖాళీ;
	తిరిగి;
    }

    PyMem_Free(pathbuf);

    ఒకవేళ (PyList_Append(list, మార్గం_Object1)
	    || PyList_Append(list, మార్గం_Object2))
	*దత్తాంశం = ఖాళీ;

    Py_DECREF(మార్గం_Object1);
    Py_DECREF(మార్గం_Object2);
}

    స్థిర PyObject *
Vim_GetPaths(PyObject *self వాడని, PyObject *వాదనలు వాడని)
{
    PyObject	*ret;

    ఒకవేళ (!(ret = PyList_New(0)))
	తిరిగి ఖాళీ;

    do_లోపల_runtimepath(ఖాళీ, 0, &map_finder_callback, ret);

    ఒకవేళ (PyErr_Occurred())
    {
	Py_DECREF(ret);
	తిరిగి ఖాళీ;
    }

    తిరిగి ret;
}

#ఒకవేళ PY_సంస్కరణ_HEX >= 0x030700f0
    స్థిర PyObject *
Finder_కనుగొను_Spec(PyObject *self, PyObject *వాదనలు)
{
    అక్షరం	*fullname;
    PyObject	*paths;
    PyObject	*లక్ష్యం = Py_None;
    PyObject	*spec;

    ఒకవేళ (!PyArg_ParseTuple(వాదనలు, "s|O", &fullname, &లక్ష్యం))
	తిరిగి ఖాళీ;

    ఒకవేళ (!(paths = Vim_GetPaths(self, ఖాళీ)))
	తిరిగి ఖాళీ;

    spec = PyObject_CallFunction(py_కనుగొను_spec, "sOO", fullname, paths, లక్ష్యం);

    Py_DECREF(paths);

    ఒకవేళ (!spec)
    {
	ఒకవేళ (PyErr_Occurred())
	    తిరిగి ఖాళీ;

	Py_INCREF(Py_None);
	తిరిగి Py_None;
    }

    తిరిగి spec;
}

    స్థిర PyObject *
Finder_కనుగొను_Module(PyObject* self వాడని, PyObject* వాదనలు వాడని)
{
    // Apparently returning None works.
    Py_INCREF(Py_None);
    తిరిగి Py_None;
}
#లేకపోతే
    స్థిర PyObject *
call_ఎక్కించు_module(అక్షరం *name, పూర్ణాంకం len, PyObject *కనుగొను_module_ఫలితం)
{
    PyObject	*fd, *pathname, *description;

    ఒకవేళ (!PyTuple_Check(కనుగొను_module_ఫలితం))
    {
	PyErr_FORMAT_TYPE(
		N_("expected 3-tuple as imp.find_module() result, but got %s"),
		కనుగొను_module_ఫలితం);
	తిరిగి ఖాళీ;
    }
    ఒకవేళ (Py_Tuple_GET_కొలత(కనుగొను_module_ఫలితం) != 3)
    {
	PyErr_FORMAT(PyExc_TypeError,
		N_("expected 3-tuple as imp.find_module() result, but got "
		   "tuple of size %d"),
		(పూర్ణాంకం) Py_Tuple_GET_కొలత(కనుగొను_module_ఫలితం));
	తిరిగి ఖాళీ;
    }

    ఒకవేళ (!(fd = Py_Tuple_GET_అంశం(కనుగొను_module_ఫలితం, 0))
	    || !(pathname = Py_Tuple_GET_అంశం(కనుగొను_module_ఫలితం, 1))
	    || !(description = Py_Tuple_GET_అంశం(కనుగొను_module_ఫలితం, 2)))
    {
	Py_Err_సమితి_STRING(PyExc_RuntimeError,
		N_("internal error: imp.find_module returned tuple with NULL"));
	తిరిగి ఖాళీ;
    }

    తిరిగి PyObject_CallFunction(py_ఎక్కించు_module,
	    "s#OOO", name, len, fd, pathname, description);
}

    స్థిర PyObject *
కనుగొను_module(అక్షరం *fullname, అక్షరం *తోక, PyObject *new_మార్గం)
{
    PyObject	*కనుగొను_module_ఫలితం;
    PyObject	*module;
    అక్షరం	*dot;

    ఒకవేళ ((dot = (అక్షరం *)vim_strchr((char_u *) తోక, '.')))
    {
	/*
	 * There is a dot in the name: call find_module recursively without the
	 * first component
	 */
	PyObject	*newest_మార్గం;
	పూర్ణాంకం		partlen = (పూర్ణాంకం) (dot - 1 - తోక);

	ఒకవేళ (!(కనుగొను_module_ఫలితం = PyObject_CallFunction(py_కనుగొను_module,
			"s#O", తోక, partlen, new_మార్గం)))
	{
	    ఒకవేళ (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_ImportError))
		PyErr_Clear();
	    తిరిగి ఖాళీ;
	}

	ఒకవేళ (!(module = call_ఎక్కించు_module(
			fullname,
			((పూర్ణాంకం) (తోక - fullname)) + partlen,
			కనుగొను_module_ఫలితం)))
	{
	    Py_DECREF(కనుగొను_module_ఫలితం);
	    తిరిగి ఖాళీ;
	}

	Py_DECREF(కనుగొను_module_ఫలితం);

	ఒకవేళ (!(newest_మార్గం = PyObject_GetAttrString(module, "__path__")))
	{
	    Py_DECREF(module);
	    తిరిగి ఖాళీ;
	}

	Py_DECREF(module);

	కనుగొను_module_ఫలితం = కనుగొను_module(fullname, dot + 1, newest_మార్గం);

	Py_DECREF(newest_మార్గం);

	తిరిగి కనుగొను_module_ఫలితం;
    }
    లేకపోతే
    {
	ఒకవేళ (!(కనుగొను_module_ఫలితం = PyObject_CallFunction(py_కనుగొను_module,
			"sO", తోక, new_మార్గం)))
	{
	    ఒకవేళ (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_ImportError))
		PyErr_Clear();
	    తిరిగి ఖాళీ;
	}

	తిరిగి కనుగొను_module_ఫలితం;
    }
}

    స్థిర PyObject *
Finder_కనుగొను_Module(PyObject *self, PyObject *వాదనలు)
{
    అక్షరం	*fullname;
    PyObject	*ఫలితం;
    PyObject	*new_మార్గం;
    LoaderObject	*loader;

    ఒకవేళ (!PyArg_ParseTuple(వాదనలు, "s", &fullname))
	తిరిగి ఖాళీ;

    ఒకవేళ (!(new_మార్గం = Vim_GetPaths(self, ఖాళీ)))
	తిరిగి ఖాళీ;

    ఫలితం = కనుగొను_module(fullname, fullname, new_మార్గం);

    Py_DECREF(new_మార్గం);

    ఒకవేళ (!ఫలితం)
    {
	ఒకవేళ (PyErr_Occurred())
	    తిరిగి ఖాళీ;

	Py_INCREF(Py_None);
	తిరిగి Py_None;
    }

    ఒకవేళ (!(fullname = (అక్షరం *)vim_strsave((char_u *)fullname)))
    {
	Py_DECREF(ఫలితం);
	Py_Err_No_జ్ఞాపకం();
	తిరిగి ఖాళీ;
    }

    ఒకవేళ (!(loader = PyObject_NEW(LoaderObject, LoaderTypePtr)))
    {
	vim_free(fullname);
	Py_DECREF(ఫలితం);
	తిరిగి ఖాళీ;
    }

    loader->fullname = fullname;
    loader->ఫలితం = ఫలితం;

    తిరిగి (PyObject *) loader;
}
#అయితే_ముగింపు

    స్థిర PyObject *
Vim_మార్గం_Hook(PyObject *self వాడని, PyObject *వాదనలు)
{
    అక్షరం	*మార్గం;

    ఒకవేళ (PyArg_ParseTuple(వాదనలు, "s", &మార్గం)
	    && STRCMP(మార్గం, vim_special_మార్గం) == 0)
    {
	Py_INCREF(vim_module);
	తిరిగి vim_module;
    }

    PyErr_Clear();
    Py_Err_సమితి_None(PyExc_ImportError);
    తిరిగి ఖాళీ;
}

/*
 * Vim module - Definitions
 */

స్థిర నిర్మాణం Py_పద్ధతి_Def VimMethods[] = {
    // name,	    function,			calling,			documentation
    {"command",	    VimCommand,			METH_O,				"Execute a Vim ex-mode command" },
    {"eval",	    VimEval,			METH_VARARGS,			"Evaluate an expression using Vim evaluator" },
    {"bindeval",    VimEvalPy,			METH_O,				"Like eval(), but returns objects attached to Vim ones"},
    {"strwidth",    VimStrwidth,		METH_O,				"Screen string width, counts <Tab> as having width 1"},
    {"chdir",	    (PyCFunction)(శూన్య *)VimChdir,	METH_VARARGS|METH_KEYWORDS,	"Change directory"},
    {"fchdir",	    (PyCFunction)(శూన్య *)VimFchdir,	METH_VARARGS|METH_KEYWORDS,	"Change directory"},
    {"foreach_rtp", VimForeachRTP,		METH_O,				"Call given callable for each path in &rtp"},
#ఒకవేళ PY_సంస్కరణ_HEX >= 0x030700f0
    {"find_spec",   Finder_కనుగొను_Spec,		METH_VARARGS,			"Internal use only, returns spec object for any input it receives"},
#అయితే_ముగింపు
    {"find_module", Finder_కనుగొను_Module,		METH_VARARGS,			"Internal use only, returns loader object for any input it receives"},
    {"path_hook",   Vim_మార్గం_Hook,		METH_VARARGS,			"Hook function to install in sys.path_hooks"},
    {"_get_paths",  (PyCFunction)Vim_GetPaths,	METH_NOARGS,			"Get &rtp-based additions to sys.path"},
    { ఖాళీ,	    ఖాళీ,			0,				ఖాళీ}
};

/*
 * Generic iterator object
 */

DEFINE_PY_TYPE_OBJECT(IterType);

రకనిర్వచనం PyObject *(*nextfun)(శూన్య **);
రకనిర్వచనం శూన్య (*destructorfun)(శూన్య *);
రకనిర్వచనం పూర్ణాంకం (*traversefun)(శూన్య *, visitproc, శూన్య *);
రకనిర్వచనం పూర్ణాంకం (*clearfun)(శూన్య **);

// Main purpose of this object is removing the need for do python
// initialization (i.e. PyType_Ready and setting type attributes) for a big
// bunch of objects.
రకనిర్వచనం నిర్మాణం
{
    Py_Object_తల
    శూన్య *cur;
    nextfun తదుపరి;
    destructorfun destruct;
    traversefun traverse;
    clearfun clear;
    PyObject *iter_object;
} IterObject;

    స్థిర PyObject *
IterNew(శూన్య *ప్రారంభం, destructorfun destruct, nextfun తదుపరి, traversefun traverse,
	clearfun clear, PyObject *iter_object)
{
    IterObject *self;

    self = PyObject_GC_New(IterObject, IterTypePtr);
    self->cur = ప్రారంభం;
    self->తదుపరి = తదుపరి;
    self->destruct = destruct;
    self->traverse = traverse;
    self->clear = clear;
    self->iter_object = iter_object;

    ఒకవేళ (iter_object)
	Py_INCREF(iter_object);

    తిరిగి (PyObject *)(self);
}

    స్థిర శూన్య
IterDestructor(PyObject *self_obj)
{
    IterObject *self = (IterObject*)self_obj;
    ఒకవేళ (self->iter_object)
	Py_DECREF(self->iter_object);
    PyObject_GC_UnTrack((శూన్య *)(self));
    self->destruct(self->cur);
    PyObject_GC_Del((శూన్య *)(self));
}

    స్థిర పూర్ణాంకం
IterTraverse(PyObject *self_obj, visitproc సందర్శించు, శూన్య *arg)
{
    IterObject *self = (IterObject*)self_obj;
    ఒకవేళ (self->traverse != ఖాళీ)
	తిరిగి self->traverse(self->cur, సందర్శించు, arg);
    లేకపోతే
	తిరిగి 0;
}

// Mac OSX defines clear() somewhere.
#నిర్వచితమైతే clear
# undef clear
#అయితే_ముగింపు

    స్థిర పూర్ణాంకం
IterClear(PyObject *self_obj)
{
    IterObject *self = (IterObject*)self_obj;
    ఒకవేళ (self->clear != ఖాళీ)
	తిరిగి self->clear(&self->cur);
    లేకపోతే
	తిరిగి 0;
}

    స్థిర PyObject *
Iter_తదుపరి(PyObject *self_obj)
{
    IterObject *self = (IterObject*)self_obj;
    తిరిగి self->తదుపరి(&self->cur);
}

    స్థిర PyObject *
IterIter(PyObject *self)
{
    Py_INCREF(self);
    తిరిగి self;
}

రకనిర్వచనం నిర్మాణం pylinkedlist_S {
    నిర్మాణం pylinkedlist_S	*pll_తదుపరి;
    నిర్మాణం pylinkedlist_S	*pll_prev;
    PyObject			*pll_obj;
} pylinkedlist_T;

స్థిర pylinkedlist_T *lastdict = ఖాళీ;
స్థిర pylinkedlist_T *lastlist = ఖాళీ;
స్థిర pylinkedlist_T *lasttuple = ఖాళీ;
స్థిర pylinkedlist_T *lastfunc = ఖాళీ;

    స్థిర శూన్య
pyll_remove(pylinkedlist_T *ref, pylinkedlist_T **చివరి)
{
    ఒకవేళ (ref->pll_prev == ఖాళీ)
    {
	ఒకవేళ (ref->pll_తదుపరి == ఖాళీ)
	{
	    *చివరి = ఖాళీ;
	    తిరిగి;
	}
    }
    లేకపోతే
	ref->pll_prev->pll_తదుపరి = ref->pll_తదుపరి;

    ఒకవేళ (ref->pll_తదుపరి == ఖాళీ)
	*చివరి = ref->pll_prev;
    లేకపోతే
	ref->pll_తదుపరి->pll_prev = ref->pll_prev;
}

    స్థిర శూన్య
pyll_కూడు(PyObject *self, pylinkedlist_T *ref, pylinkedlist_T **చివరి)
{
    ఒకవేళ (*చివరి == ఖాళీ)
	ref->pll_prev = ఖాళీ;
    లేకపోతే
    {
	(*చివరి)->pll_తదుపరి = ref;
	ref->pll_prev = *చివరి;
    }
    ref->pll_తదుపరి = ఖాళీ;
    ref->pll_obj = self;
    *చివరి = ref;
}

DEFINE_PY_TYPE_OBJECT(DictionaryType);

రకనిర్వచనం నిర్మాణం
{
    Py_Object_తల
    dict_T	*dict;
    pylinkedlist_T	ref;
} DictionaryObject;

స్థిర PyObject *DictionaryUpdate(DictionaryObject *, PyObject *, PyObject *);

#నిర్వచించు NEW_DICTIONARY(dict) DictionaryNew(DictionaryTypePtr, dict)

    స్థిర PyObject *
DictionaryNew(PyTypeObject *subtype, dict_T *dict)
{
    DictionaryObject	*self;

    self = (DictionaryObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    ఒకవేళ (self == ఖాళీ)
	తిరిగి ఖాళీ;
    self->dict = dict;
    ++dict->dv_refcount;

    pyll_కూడు((PyObject *)(self), &self->ref, &lastdict);

    తిరిగి (PyObject *)(self);
}

    స్థిర dict_T *
py_dict_alloc(శూన్య)
{
    dict_T	*ret;

    ఒకవేళ (!(ret = dict_alloc()))
    {
	Py_Err_No_జ్ఞాపకం();
	తిరిగి ఖాళీ;
    }
    ++ret->dv_refcount;

    తిరిగి ret;
}

    స్థిర PyObject *
DictionaryConstructor(PyTypeObject *subtype, PyObject *వాదనలు, PyObject *kwargs)
{
    DictionaryObject	*self;
    dict_T	*dict;

    ఒకవేళ (!(dict = py_dict_alloc()))
	తిరిగి ఖాళీ;

    self = (DictionaryObject *) DictionaryNew(subtype, dict);

    --dict->dv_refcount;

    ఒకవేళ (kwargs || Py_Tuple_కొలత(వాదనలు))
    {
	PyObject	*tmp;
	ఒకవేళ (!(tmp = DictionaryUpdate(self, వాదనలు, kwargs)))
	{
	    Py_DECREF(self);
	    తిరిగి ఖాళీ;
	}

	Py_DECREF(tmp);
    }

    తిరిగి (PyObject *)(self);
}

    స్థిర శూన్య
DictionaryDestructor(PyObject *self_obj)
{
    DictionaryObject *self = (DictionaryObject*)self_obj;
    pyll_remove(&self->ref, &lastdict);
    dict_unref(self->dict);

    DESTRUCTOR_ముగింపు(self);
}

స్థిర అక్షరం *DictionaryAttrs[] = {
    "locked", "scope",
    ఖాళీ
};

    స్థిర PyObject *
DictionaryDir(PyObject *self, PyObject *వాదనలు వాడని)
{
    తిరిగి ObjectDir(self, DictionaryAttrs);
}

    స్థిర పూర్ణాంకం
DictionarySetattr(PyObject *self_obj, అక్షరం *name, PyObject *valObject)
{
    DictionaryObject *self = (DictionaryObject*)self_obj;
    ఒకవేళ (valObject == ఖాళీ)
    {
	Py_Err_సమితి_STRING(PyExc_AttributeError,
		N_("cannot delete vim.Dictionary attributes"));
	తిరిగి -1;
    }

    ఒకవేళ (strcmp(name, "locked") == 0)
    {
	ఒకవేళ (self->dict->dv_తాళం == VAR_FIXED)
	{
	    Py_Err_సమితి_STRING(PyExc_TypeError,
		    N_("cannot modify fixed dictionary"));
	    తిరిగి -1;
	}
	లేకపోతే
	{
	    పూర్ణాంకం		istrue = PyObject_IsTrue(valObject);
	    ఒకవేళ (istrue == -1)
		తిరిగి -1;
	    లేకపోతే ఒకవేళ (istrue)
		self->dict->dv_తాళం = VAR_LOCKED;
	    లేకపోతే
		self->dict->dv_తాళం = 0;
	}
	తిరిగి 0;
    }
    లేకపోతే
    {
	PyErr_FORMAT(PyExc_AttributeError, N_("cannot set attribute %s"), name);
	తిరిగి -1;
    }
}

    స్థిర PyInt
Dictionary_నిడివి(DictionaryObject *self)
{
    తిరిగి ((PyInt) (self->dict->dv_hashtab.ht_వాడిన));
}

#నిర్వచించు DICT_FLAG_HAS_DEFAULT	0x01
#నిర్వచించు DICT_FLAG_తీయి		0x02
#నిర్వచించు DICT_FLAG_NONE_DEFAULT	0x04
#నిర్వచించు DICT_FLAG_RETURN_BOOL	0x08 // Incompatible with DICT_FLAG_POP
#నిర్వచించు DICT_FLAG_RETURN_PAIR	0x10

    స్థిర PyObject *
Dictionary_అంశం(DictionaryObject *self, PyObject *వాదనలు, పూర్ణాంకం flags)
{
    PyObject	*కీలకం_Object;
    PyObject	*defObject = ((flags & DICT_FLAG_NONE_DEFAULT)? Py_None : ఖాళీ);
    PyObject	*ret;
    char_u	*కీలకం;
    dictitem_T	*di;
    dict_T	*dict = self->dict;
    hashitem_T	*hi;
    PyObject	*todecref;

    ఒకవేళ (flags & DICT_FLAG_HAS_DEFAULT)
    {
	ఒకవేళ (!PyArg_ParseTuple(వాదనలు, "O|O", &కీలకం_Object, &defObject))
	    తిరిగి ఖాళీ;
    }
    లేకపోతే
	కీలకం_Object = వాదనలు;

    ఒకవేళ (flags & DICT_FLAG_RETURN_BOOL)
	defObject = Py_False;

    ఒకవేళ (!(కీలకం = String_గమ్యం_Chars(కీలకం_Object, &todecref)))
	తిరిగి ఖాళీ;

    ఒకవేళ (*కీలకం == NUL)
    {
	RAISE_NO_ఖాళీలు_కీలకాలు;
	Py_XDECREF(todecref);
	తిరిగి ఖాళీ;
    }

    hi = hash_కనుగొను(&dict->dv_hashtab, కీలకం);

    Py_XDECREF(todecref);

    ఒకవేళ (HASHITEM_ఖాళీలు(hi))
    {
	ఒకవేళ (defObject)
	{
	    Py_INCREF(defObject);
	    తిరిగి defObject;
	}
	లేకపోతే
	{
	    Py_Err_సమితి_Object(Py_Exc_కీలకం_Error, కీలకం_Object);
	    తిరిగి ఖాళీ;
	}
    }
    లేకపోతే ఒకవేళ (flags & DICT_FLAG_RETURN_BOOL)
    {
	ret = Py_True;
	Py_INCREF(ret);
	తిరిగి ret;
    }

    di = dict_lookup(hi);

    ఒకవేళ (!(ret = Convert_గమ్యం_Py_Object(&di->di_tv)))
	తిరిగి ఖాళీ;

    ఒకవేళ (flags & DICT_FLAG_తీయి)
    {
	ఒకవేళ (dict->dv_తాళం)
	{
	    RAISE_LOCKED_DICTIONARY;
	    Py_DECREF(ret);
	    తిరిగి ఖాళీ;
	}

	hash_remove(&dict->dv_hashtab, hi, "Python remove variable");
	dictitem_free(di);
    }

    తిరిగి ret;
}

    స్థిర PyObject *
Dictionary_అంశం_2(DictionaryObject *self, PyObject *కీలకం_Object)
{
    తిరిగి Dictionary_అంశం(self, కీలకం_Object, 0);
}

    స్థిర పూర్ణాంకం
Dictionary_కలిగియుంది(DictionaryObject *self, PyObject *కీలకం_Object)
{
    PyObject	*rObj = Dictionary_అంశం(self, కీలకం_Object, DICT_FLAG_RETURN_BOOL);
    పూర్ణాంకం		ret;

    ఒకవేళ (rObj == ఖాళీ)
	తిరిగి -1;

    ret = (rObj == Py_True);

    Py_DECREF(rObj);

    తిరిగి ret;
}

రకనిర్వచనం నిర్మాణం
{
    పూర్ణాంకం		dii_మారింది;
    hashtab_T	*dii_ht;
    hashitem_T	*dii_hi;
    long_u	dii_todo;
} dictiterinfo_T;

    స్థిర PyObject *
Dictionary_Iter_తదుపరి(శూన్య **arg)
{
    PyObject	*ret;
    dictiterinfo_T **dii = (dictiterinfo_T**)arg;

    ఒకవేళ (!(*dii)->dii_todo)
	తిరిగి ఖాళీ;

    ఒకవేళ ((*dii)->dii_ht->ht_మారింది != (*dii)->dii_మారింది)
    {
	Py_Err_సమితి_STRING(PyExc_RuntimeError,
		N_("hashtab changed during iteration"));
	తిరిగి ఖాళీ;
    }

    అయితే (((*dii)->dii_todo) && HASHITEM_ఖాళీలు((*dii)->dii_hi))
	++((*dii)->dii_hi);

    --((*dii)->dii_todo);

    ఒకవేళ (!(ret = Py_బైట్లు_From_String((అక్షరం *)(*dii)->dii_hi->hi_కీలకం)))
	తిరిగి ఖాళీ;

    తిరిగి ret;
}

    స్థిర PyObject *
DictionaryIter(PyObject *self_obj)
{
    DictionaryObject	*self = (DictionaryObject*)self_obj;
    dictiterinfo_T	*dii;
    hashtab_T		*ht;

    ఒకవేళ (!(dii = PyMem_New(dictiterinfo_T, 1)))
    {
	Py_Err_No_జ్ఞాపకం();
	తిరిగి ఖాళీ;
    }

    ht = &self->dict->dv_hashtab;
    dii->dii_మారింది = ht->ht_మారింది;
    dii->dii_ht = ht;
    dii->dii_hi = ht->ht_array;
    dii->dii_todo = ht->ht_వాడిన;

    తిరిగి IterNew(dii,
	    PyMem_Free, Dictionary_Iter_తదుపరి,
	    ఖాళీ, ఖాళీ, (PyObject *)self);
}

    స్థిర పూర్ణాంకం
Dictionary_Ass_అంశం(
	DictionaryObject *self, PyObject *కీలకం_Object, PyObject *valObject)
{
    char_u	*కీలకం;
    typval_T	tv;
    dict_T	*dict = self->dict;
    dictitem_T	*di;
    PyObject	*todecref;

    ఒకవేళ (dict->dv_తాళం)
    {
	RAISE_LOCKED_DICTIONARY;
	తిరిగి -1;
    }

    ఒకవేళ (!(కీలకం = String_గమ్యం_Chars(కీలకం_Object, &todecref)))
	తిరిగి -1;

    ఒకవేళ (*కీలకం == NUL)
    {
	RAISE_NO_ఖాళీలు_కీలకాలు;
	Py_XDECREF(todecref);
	తిరిగి -1;
    }

    di = dict_కనుగొను(dict, కీలకం, -1);

    ఒకవేళ (valObject == ఖాళీ)
    {
	hashitem_T	*hi;

	ఒకవేళ (di == ఖాళీ)
	{
	    Py_XDECREF(todecref);
	    Py_Err_సమితి_Object(Py_Exc_కీలకం_Error, కీలకం_Object);
	    తిరిగి -1;
	}
	hi = hash_కనుగొను(&dict->dv_hashtab, di->di_కీలకం);
	hash_remove(&dict->dv_hashtab, hi, "Python remove item");
	dictitem_free(di);
	Py_XDECREF(todecref);
	తిరిగి 0;
    }

    ఒకవేళ (ConvertFromPyObject(valObject, &tv) == -1)
    {
	Py_XDECREF(todecref);
	తిరిగి -1;
    }

    ఒకవేళ (di == ఖాళీ)
    {
	ఒకవేళ (!(di = dictitem_alloc(కీలకం)))
	{
	    Py_XDECREF(todecref);
	    Py_Err_No_జ్ఞాపకం();
	    తిరిగి -1;
	}
	di->di_tv.v_type = VAR_UNKNOWN;

	ఒకవేళ (dict_కూడు(dict, di) == FAIL)
	{
	    dictitem_free(di);
	    RAISE_కీలకం_కూడు_FAIL(కీలకం);
	    Py_XDECREF(todecref);
	    తిరిగి -1;
	}
    }
    లేకపోతే
	clear_tv(&di->di_tv);

    Py_XDECREF(todecref);

    copy_tv(&tv, &di->di_tv);
    clear_tv(&tv);
    తిరిగి 0;
}

రకనిర్వచనం PyObject *(*hi_గమ్యం_py)(hashitem_T *);

    స్థిర PyObject *
DictionaryListObjects(DictionaryObject *self, hi_గమ్యం_py hiconvert)
{
    dict_T	*dict = self->dict;
    long_u	todo = dict->dv_hashtab.ht_వాడిన;
    Py_ssize_t	i = 0;
    PyObject	*ret;
    hashitem_T	*hi;
    PyObject	*newObj;

    ret = PyList_New(todo);
    కొరకు (hi = dict->dv_hashtab.ht_array; todo > 0; ++hi)
    {
	ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
	{
	    ఒకవేళ (!(newObj = hiconvert(hi)))
	    {
		Py_DECREF(ret);
		తిరిగి ఖాళీ;
	    }
	    Py_List_సమితి_అంశం(ret, i, newObj);
	    --todo;
	    ++i;
	}
    }
    తిరిగి ret;
}

    స్థిర PyObject *
dict_కీలకం(hashitem_T *hi)
{
    తిరిగి Py_బైట్లు_From_String((అక్షరం *)(hi->hi_కీలకం));
}

    స్థిర PyObject *
Dictionary_List_కీలకాలు(DictionaryObject *self, PyObject *వాదనలు వాడని)
{
    తిరిగి DictionaryListObjects(self, dict_కీలకం);
}

    స్థిర PyObject *
dict_val(hashitem_T *hi)
{
    dictitem_T	*di;

    di = dict_lookup(hi);
    తిరిగి Convert_గమ్యం_Py_Object(&di->di_tv);
}

    స్థిర PyObject *
Dictionary_List_విలువలు(DictionaryObject *self, PyObject *వాదనలు వాడని)
{
    తిరిగి DictionaryListObjects(self, dict_val);
}

    స్థిర PyObject *
dict_అంశం(hashitem_T *hi)
{
    PyObject	*కీలకం_Object;
    PyObject	*valObject;
    PyObject	*ret;

    ఒకవేళ (!(కీలకం_Object = dict_కీలకం(hi)))
	తిరిగి ఖాళీ;

    ఒకవేళ (!(valObject = dict_val(hi)))
    {
	Py_DECREF(కీలకం_Object);
	తిరిగి ఖాళీ;
    }

    ret = Py_Build_విలువ("(OO)", కీలకం_Object, valObject);

    Py_DECREF(కీలకం_Object);
    Py_DECREF(valObject);

    తిరిగి ret;
}

    స్థిర PyObject *
Dictionary_List_వస్తువులు(DictionaryObject *self, PyObject *వాదనలు వాడని)
{
    తిరిగి DictionaryListObjects(self, dict_అంశం);
}

    స్థిర PyObject *
DictionaryUpdate(DictionaryObject *self, PyObject *వాదనలు, PyObject *kwargs)
{
    dict_T	*dict = self->dict;

    ఒకవేళ (dict->dv_తాళం)
    {
	RAISE_LOCKED_DICTIONARY;
	తిరిగి ఖాళీ;
    }

    ఒకవేళ (kwargs)
    {
	typval_T	tv;

	ఒకవేళ (ConvertFromPyMapping(kwargs, &tv) == -1)
	    తిరిగి ఖాళీ;

	Vim_Try_ప్రారంభం();
	dict_extend(self->dict, tv.vval.v_dict, (char_u *) "force", ఖాళీ);
	clear_tv(&tv);
	ఒకవేళ (Vim_Try_చివర())
	    తిరిగి ఖాళీ;
    }
    లేకపోతే
    {
	PyObject	*obj = ఖాళీ;

	ఒకవేళ (!PyArg_ParseTuple(వాదనలు, "|O", &obj))
	    తిరిగి ఖాళీ;

	ఒకవేళ (obj == ఖాళీ)
	{
	    Py_INCREF(Py_None);
	    తిరిగి Py_None;
	}

	ఒకవేళ (PyObject_HasAttrString(obj, "keys"))
	    తిరిగి DictionaryUpdate(self, ఖాళీ, obj);
	లేకపోతే
	{
	    PyObject	*iterator;
	    PyObject	*అంశం;

	    ఒకవేళ (!(iterator = PyObject_GetIter(obj)))
		తిరిగి ఖాళీ;

	    అయితే ((అంశం = Py_Iter_తదుపరి(iterator)))
	    {
		PyObject	*fast;
		PyObject	*కీలకం_Object;
		PyObject	*valObject;
		PyObject	*todecref;
		char_u		*కీలకం;
		dictitem_T	*di;
		hashitem_T	*hi;

		ఒకవేళ (!(fast = PySequence_Fast(అంశం, "")))
		{
		    Py_DECREF(iterator);
		    Py_DECREF(అంశం);
		    తిరిగి ఖాళీ;
		}

		Py_DECREF(అంశం);

		ఒకవేళ (Py_Sequence_కొలత(fast) != 2)
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    PyErr_FORMAT(Py_Exc_విలువ_Error,
			    N_("expected sequence element of size 2, "
			    "but got sequence of size %d"),
			    (పూర్ణాంకం) Py_Sequence_కొలత(fast));
		    తిరిగి ఖాళీ;
		}

		కీలకం_Object = Py_Sequence_Get_అంశం(fast, 0);

		ఒకవేళ (!(కీలకం = String_గమ్యం_Chars(కీలకం_Object, &todecref)))
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    తిరిగి ఖాళీ;
		}

		di = dictitem_alloc(కీలకం);

		Py_XDECREF(todecref);

		ఒకవేళ (di == ఖాళీ)
		{
		    Py_DECREF(fast);
		    Py_DECREF(iterator);
		    Py_Err_No_జ్ఞాపకం();
		    తిరిగి ఖాళీ;
		}
		di->di_tv.v_type = VAR_UNKNOWN;

		valObject = Py_Sequence_Get_అంశం(fast, 1);

		ఒకవేళ (ConvertFromPyObject(valObject, &di->di_tv) == -1)
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    dictitem_free(di);
		    తిరిగి ఖాళీ;
		}

		Py_DECREF(fast);

		hi = hash_కనుగొను(&dict->dv_hashtab, di->di_కీలకం);
		ఒకవేళ (!HASHITEM_ఖాళీలు(hi) || dict_కూడు(dict, di) == FAIL)
		{
		    RAISE_కీలకం_కూడు_FAIL(di->di_కీలకం);
		    Py_DECREF(iterator);
		    dictitem_free(di);
		    తిరిగి ఖాళీ;
		}
	    }

	    Py_DECREF(iterator);

	    // Iterator may have finished due to an exception
	    ఒకవేళ (PyErr_Occurred())
		తిరిగి ఖాళీ;
	}
    }
    Py_INCREF(Py_None);
    తిరిగి Py_None;
}

    స్థిర PyObject *
DictionaryGet(DictionaryObject *self, PyObject *వాదనలు)
{
    తిరిగి Dictionary_అంశం(self, వాదనలు,
			    DICT_FLAG_HAS_DEFAULT|DICT_FLAG_NONE_DEFAULT);
}

    స్థిర PyObject *
Dictionary_తీయి(DictionaryObject *self, PyObject *వాదనలు)
{
    తిరిగి Dictionary_అంశం(self, వాదనలు, DICT_FLAG_HAS_DEFAULT|DICT_FLAG_తీయి);
}

    స్థిర PyObject *
Dictionary_తీయి_అంశం(DictionaryObject *self, PyObject *వాదనలు వాడని)
{
    hashitem_T	*hi;
    PyObject	*ret;
    PyObject	*valObject;
    dictitem_T	*di;

    ఒకవేళ (self->dict->dv_hashtab.ht_వాడిన == 0)
    {
	Py_Err_సమితి_None(Py_Exc_కీలకం_Error);
	తిరిగి ఖాళీ;
    }

    hi = self->dict->dv_hashtab.ht_array;
    అయితే (HASHITEM_ఖాళీలు(hi))
	++hi;

    di = dict_lookup(hi);

    ఒకవేళ (!(valObject = Convert_గమ్యం_Py_Object(&di->di_tv)))
	తిరిగి ఖాళీ;

    ఒకవేళ (!(ret = Py_Build_విలువ("(" Py_బైట్లు_fmt "O)", hi->hi_కీలకం, valObject)))
    {
	Py_DECREF(valObject);
	తిరిగి ఖాళీ;
    }

    hash_remove(&self->dict->dv_hashtab, hi, "Python pop item");
    dictitem_free(di);

    తిరిగి ret;
}

    స్థిర PyObject *
Dictionary_Has_కీలకం(DictionaryObject *self, PyObject *కీలకం_Object)
{
    తిరిగి Dictionary_అంశం(self, కీలకం_Object, DICT_FLAG_RETURN_BOOL);
}

స్థిర PySequenceMethods DictionaryAsSeq = {
    0,					// sq_length
    0,					// sq_concat
    0,					// sq_repeat
    0,					// sq_item
    0,					// sq_slice
    0,					// sq_ass_item
    0,					// sq_ass_slice
    (objobjproc) Dictionary_కలిగియుంది,	// sq_contains
    0,					// sq_inplace_concat
    0,					// sq_inplace_repeat
};

స్థిర PyMappingMethods DictionaryAsMapping = {
    (lenfunc)       Dictionary_నిడివి,
    (binaryfunc)    Dictionary_అంశం_2,
    (objobjargproc) Dictionary_Ass_అంశం,
};

స్థిర నిర్మాణం Py_పద్ధతి_Def DictionaryMethods[] = {
    {"keys",	(PyCFunction)Dictionary_List_కీలకాలు,	METH_NOARGS,	""},
    {"values",	(PyCFunction)Dictionary_List_విలువలు,	METH_NOARGS,	""},
    {"items",	(PyCFunction)Dictionary_List_వస్తువులు,	METH_NOARGS,	""},
    {"update",	(PyCFunction)(శూన్య *)DictionaryUpdate,		METH_VARARGS|METH_KEYWORDS, ""},
    {"get",	(PyCFunction)DictionaryGet,		METH_VARARGS,	""},
    {"pop",	(PyCFunction)Dictionary_తీయి,		METH_VARARGS,	""},
    {"popitem",	(PyCFunction)Dictionary_తీయి_అంశం,		METH_NOARGS,	""},
    {"has_key",	(PyCFunction)Dictionary_Has_కీలకం,		METH_O,		""},
    {"__dir__",	(PyCFunction)DictionaryDir,		METH_NOARGS,	""},
    { ఖాళీ,	ఖాళీ,					0,		ఖాళీ}
};

DEFINE_PY_TYPE_OBJECT(ListType);

రకనిర్వచనం నిర్మాణం
{
    Py_Object_తల
    list_T	*list;
    pylinkedlist_T	ref;
} ListObject;

#నిర్వచించు NEW_LIST(list) ListNew(ListTypePtr, list)

    స్థిర PyObject *
ListNew(PyTypeObject *subtype, list_T *list)
{
    ListObject	*self;

    ఒకవేళ (list == ఖాళీ)
	తిరిగి ఖాళీ;

    self = (ListObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    ఒకవేళ (self == ఖాళీ)
	తిరిగి ఖాళీ;
    self->list = list;
    ++list->lv_refcount;
    CHECK_LIST_MATERIALIZE(list);

    pyll_కూడు((PyObject *)(self), &self->ref, &lastlist);

    తిరిగి (PyObject *)(self);
}

    స్థిర list_T *
py_list_alloc(శూన్య)
{
    list_T	*ret;

    ఒకవేళ (!(ret = list_alloc()))
    {
	Py_Err_No_జ్ఞాపకం();
	తిరిగి ఖాళీ;
    }
    ++ret->lv_refcount;

    తిరిగి ret;
}

    స్థిర పూర్ణాంకం
list_py_concat(list_T *l, PyObject *obj, PyObject *lookup_dict)
{
    PyObject	*iterator;
    PyObject	*అంశం;
    listitem_T	*li;

    ఒకవేళ (!(iterator = PyObject_GetIter(obj)))
	తిరిగి -1;

    అయితే ((అంశం = Py_Iter_తదుపరి(iterator)))
    {
	ఒకవేళ (!(li = listitem_alloc()))
	{
	    Py_Err_No_జ్ఞాపకం();
	    Py_DECREF(అంశం);
	    Py_DECREF(iterator);
	    తిరిగి -1;
	}
	li->li_tv.v_తాళం = 0;
	li->li_tv.v_type = VAR_UNKNOWN;

	ఒకవేళ (_ConvertFromPyObject(అంశం, &li->li_tv, lookup_dict) == -1)
	{
	    Py_DECREF(అంశం);
	    Py_DECREF(iterator);
	    listitem_free(l, li);
	    తిరిగి -1;
	}

	Py_DECREF(అంశం);

	list_append(l, li);
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    ఒకవేళ (PyErr_Occurred())
	తిరిగి -1;

    తిరిగి 0;
}

    స్థిర PyObject *
ListConstructor(PyTypeObject *subtype, PyObject *వాదనలు, PyObject *kwargs)
{
    list_T	*list;
    PyObject	*obj = ఖాళీ;

    ఒకవేళ (kwargs)
    {
	Py_Err_సమితి_STRING(PyExc_TypeError,
		N_("list constructor does not accept keyword arguments"));
	తిరిగి ఖాళీ;
    }

    ఒకవేళ (!PyArg_ParseTuple(వాదనలు, "|O", &obj))
	తిరిగి ఖాళీ;

    ఒకవేళ (!(list = py_list_alloc()))
	తిరిగి ఖాళీ;

    ఒకవేళ (obj)
    {
	PyObject	*lookup_dict;

	ఒకవేళ (!(lookup_dict = PyDict_New()))
	{
	    list_unref(list);
	    తిరిగి ఖాళీ;
	}

	ఒకవేళ (list_py_concat(list, obj, lookup_dict) == -1)
	{
	    Py_DECREF(lookup_dict);
	    list_unref(list);
	    తిరిగి ఖాళీ;
	}

	Py_DECREF(lookup_dict);
    }

    తిరిగి ListNew(subtype, list);
}

    స్థిర శూన్య
ListDestructor(PyObject *self_obj)
{
    ListObject *self = (ListObject*)self_obj;
    pyll_remove(&self->ref, &lastlist);
    list_unref(self->list);

    DESTRUCTOR_ముగింపు(self);
}

    స్థిర PyInt
List_నిడివి(ListObject *self)
{
    తిరిగి ((PyInt) (self->list->lv_len));
}

    స్థిర PyObject *
List_సూచీ(ListObject *self, Py_ssize_t సూచీ)
{
    listitem_T	*li;

    ఒకవేళ (సూచీ >= List_నిడివి(self))
    {
	Py_Err_సమితి_STRING(Py_Exc_సూచీ_Error, N_("list index out of range"));
	తిరిగి ఖాళీ;
    }
    li = list_కనుగొను(self->list, (దీర్ఘ) సూచీ);
    ఒకవేళ (li == ఖాళీ)
    {
	// No more suitable format specifications in python-2.3
	PyErr_VIM_FORMAT(N_("internal error: failed to get Vim list item %d"),
		(పూర్ణాంకం) సూచీ);
	తిరిగి ఖాళీ;
    }
    తిరిగి Convert_గమ్యం_Py_Object(&li->li_tv);
}

    స్థిర PyObject *
List_ముక్క(ListObject *self, Py_ssize_t మొదటి, Py_ssize_t అడుగు,
	  Py_ssize_t slicelen)
{
    PyInt	i;
    PyObject	*list;

    ఒకవేళ (అడుగు == 0)
    {
	Py_Err_సమితి_STRING(Py_Exc_విలువ_Error, N_("slice step cannot be zero"));
	తిరిగి ఖాళీ;
    }

    list = PyList_New(slicelen);
    ఒకవేళ (list == ఖాళీ)
	తిరిగి ఖాళీ;

    కొరకు (i = 0; i < slicelen; ++i)
    {
	PyObject	*అంశం;

	అంశం = List_సూచీ(self, మొదటి + i*అడుగు);
	ఒకవేళ (అంశం == ఖాళీ)
	{
	    Py_DECREF(list);
	    తిరిగి ఖాళీ;
	}

	Py_List_సమితి_అంశం(list, i, అంశం);
    }

    తిరిగి list;
}

    స్థిర PyObject *
List_అంశం(ListObject *self, PyObject* idx)
{
#ఒకవేళ PY_MAJOR_సంస్కరణ < 3
    ఒకవేళ (PyInt_Check(idx))
    {
	దీర్ఘ _idx = PyInt_AsLong(idx);
	తిరిగి List_సూచీ(self, _idx);
    }
    లేకపోతే
#అయితే_ముగింపు
    ఒకవేళ (PyLong_Check(idx))
    {
	దీర్ఘ _idx = PyLong_AsLong(idx);
	తిరిగి List_సూచీ(self, _idx);
    }
    లేకపోతే ఒకవేళ (Py_ముక్క_Check(idx))
    {
	Py_ssize_t ప్రారంభం, stop, అడుగు, slicelen;

	ఒకవేళ (Py_ముక్క_Get_Indices_Ex((Py_ముక్క_Object_T *)idx, List_నిడివి(self),
				 &ప్రారంభం, &stop, &అడుగు, &slicelen) < 0)
	    తిరిగి ఖాళీ;
	తిరిగి List_ముక్క(self, ప్రారంభం, అడుగు, slicelen);
    }
    లేకపోతే
    {
	RAISE_INVALID_సూచీ_TYPE(idx);
	తిరిగి ఖాళీ;
    }
}

    స్థిర శూన్య
list_restore(Py_ssize_t numadded, Py_ssize_t numreplaced, Py_ssize_t slicelen,
	list_T *l, listitem_T **lis, listitem_T *lastaddedli)
{
    అయితే (numreplaced--)
    {
	list_చొప్పించు(l, lis[numreplaced], lis[slicelen + numreplaced]);
	listitem_remove(l, lis[slicelen + numreplaced]);
    }
    అయితే (numadded--)
    {
	listitem_T	*తదుపరి;

	తదుపరి = lastaddedli->li_prev;
	listitem_remove(l, lastaddedli);
	lastaddedli = తదుపరి;
    }
}

    స్థిర పూర్ణాంకం
List_Ass_ముక్క(ListObject *self, Py_ssize_t మొదటి,
	     Py_ssize_t అడుగు, Py_ssize_t slicelen, PyObject *obj)
{
    PyObject	*iterator;
    PyObject	*అంశం;
    listitem_T	*li;
    listitem_T	*lastaddedli = ఖాళీ;
    listitem_T	*తదుపరి;
    typval_T	v;
    list_T	*l = self->list;
    PyInt	i;
    PyInt	j;
    PyInt	numreplaced = 0;
    PyInt	numadded = 0;
    PyInt	కొలత;
    listitem_T	**lis = ఖాళీ;

    కొలత = List_నిడివి(self);

    ఒకవేళ (l->lv_తాళం)
    {
	RAISE_LOCKED_LIST;
	తిరిగి -1;
    }

    ఒకవేళ (అడుగు == 0)
    {
	Py_Err_సమితి_STRING(Py_Exc_విలువ_Error, N_("slice step cannot be zero"));
	తిరిగి -1;
    }

    ఒకవేళ (అడుగు != 1 && slicelen == 0)
    {
	// Nothing to do. Only error out if obj has some items.
	పూర్ణాంకం		ret = 0;

	ఒకవేళ (obj == ఖాళీ)
	    తిరిగి 0;

	ఒకవేళ (!(iterator = PyObject_GetIter(obj)))
	    తిరిగి -1;

	ఒకవేళ ((అంశం = Py_Iter_తదుపరి(iterator)))
	{
	    PyErr_FORMAT(Py_Exc_విలువ_Error,
		    N_("attempt to assign sequence of size greater than %d "
			"to extended slice"), 0);
	    Py_DECREF(అంశం);
	    ret = -1;
	}
	Py_DECREF(iterator);
	తిరిగి ret;
    }

    ఒకవేళ (obj != ఖాళీ)
	// XXX May allocate zero bytes.
	ఒకవేళ (!(lis = PyMem_New(listitem_T *, slicelen * 2)))
	{
	    Py_Err_No_జ్ఞాపకం();
	    తిరిగి -1;
	}

    ఒకవేళ (మొదటి == కొలత)
	li = ఖాళీ;
    లేకపోతే
    {
	li = list_కనుగొను(l, (దీర్ఘ) మొదటి);
	ఒకవేళ (li == ఖాళీ)
	{
	    PyErr_VIM_FORMAT(N_("internal error: no Vim list item %d"),
		    (పూర్ణాంకం)మొదటి);
	    ఒకవేళ (obj != ఖాళీ)
		PyMem_Free(lis);
	    తిరిగి -1;
	}
	i = slicelen;
	అయితే (i-- && li != ఖాళీ)
	{
	    j = అడుగు;
	    తదుపరి = li;
	    ఒకవేళ (అడుగు > 0)
		అయితే (తదుపరి != ఖాళీ && ((తదుపరి = తదుపరి->li_తదుపరి) != ఖాళీ) && --j);
	    లేకపోతే
		అయితే (తదుపరి != ఖాళీ && ((తదుపరి = తదుపరి->li_prev) != ఖాళీ) && ++j);

	    ఒకవేళ (obj == ఖాళీ)
		listitem_remove(l, li);
	    లేకపోతే
		lis[slicelen - i - 1] = li;

	    li = తదుపరి;
	}
	ఒకవేళ (li == ఖాళీ && i != -1)
	{
	    Py_Err_సమితి_VIM(N_("internal error: not enough list items"));
	    ఒకవేళ (obj != ఖాళీ)
		PyMem_Free(lis);
	    తిరిగి -1;
	}
    }

    ఒకవేళ (obj == ఖాళీ)
	తిరిగి 0;

    ఒకవేళ (!(iterator = PyObject_GetIter(obj)))
    {
	PyMem_Free(lis);
	తిరిగి -1;
    }

    i = 0;
    అయితే ((అంశం = Py_Iter_తదుపరి(iterator)))
    {
	ఒకవేళ (ConvertFromPyObject(అంశం, &v) == -1)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(అంశం);
	    PyMem_Free(lis);
	    తిరిగి -1;
	}
	Py_DECREF(అంశం);
	ఒకవేళ (list_చొప్పించు_tv(l, &v, numreplaced < slicelen
				    ? lis[numreplaced]
				    : li) == FAIL)
	{
	    clear_tv(&v);
	    Py_Err_సమితి_VIM(N_("internal error: failed to add item to list"));
	    list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	    PyMem_Free(lis);
	    తిరిగి -1;
	}
	ఒకవేళ (numreplaced < slicelen)
	{
	    lis[slicelen + numreplaced] = lis[numreplaced]->li_prev;
	    vimlist_remove(l, lis[numreplaced], lis[numreplaced]);
	    numreplaced++;
	}
	లేకపోతే
	{
	    ఒకవేళ (li)
		lastaddedli = li->li_prev;
	    లేకపోతే
		lastaddedli = l->lv_u.mat.lv_చివరి;
	    numadded++;
	}
	clear_tv(&v);
	ఒకవేళ (అడుగు != 1 && i >= slicelen)
	{
	    Py_DECREF(iterator);
	    PyErr_FORMAT(Py_Exc_విలువ_Error,
		    N_("attempt to assign sequence of size greater than %d "
			"to extended slice"), (పూర్ణాంకం) slicelen);
	    list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	    PyMem_Free(lis);
	    తిరిగి -1;
	}
	++i;
    }
    Py_DECREF(iterator);

    ఒకవేళ (అడుగు != 1 && i != slicelen)
    {
	PyErr_FORMAT2(Py_Exc_విలువ_Error,
		N_("attempt to assign sequence of size %d to extended slice "
		    "of size %d"), (పూర్ణాంకం) i, (పూర్ణాంకం) slicelen);
	list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	PyMem_Free(lis);
	తిరిగి -1;
    }

    ఒకవేళ (PyErr_Occurred())
    {
	list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	PyMem_Free(lis);
	తిరిగి -1;
    }

    కొరకు (i = 0; i < numreplaced; i++)
	listitem_free(l, lis[i]);
    ఒకవేళ (అడుగు == 1)
	కొరకు (i = numreplaced; i < slicelen; i++)
	    listitem_remove(l, lis[i]);

    PyMem_Free(lis);

    తిరిగి 0;
}

    స్థిర పూర్ణాంకం
List_Ass_సూచీ(ListObject *self, Py_ssize_t సూచీ, PyObject *obj)
{
    typval_T	tv;
    list_T	*l = self->list;
    listitem_T	*li;
    Py_ssize_t	నిడివి = List_నిడివి(self);

    ఒకవేళ (l->lv_తాళం)
    {
	RAISE_LOCKED_LIST;
	తిరిగి -1;
    }
    ఒకవేళ (సూచీ > నిడివి || (సూచీ == నిడివి && obj == ఖాళీ))
    {
	Py_Err_సమితి_STRING(Py_Exc_సూచీ_Error, N_("list index out of range"));
	తిరిగి -1;
    }

    ఒకవేళ (obj == ఖాళీ)
    {
	li = list_కనుగొను(l, (దీర్ఘ) సూచీ);
	ఒకవేళ (li == ఖాళీ)
	{
	    PyErr_VIM_FORMAT(N_("internal error: failed to get Vim "
			"list item %d"), (పూర్ణాంకం) సూచీ);
	    తిరిగి -1;
	}
	vimlist_remove(l, li, li);
	clear_tv(&li->li_tv);
	vim_free(li);
	తిరిగి 0;
    }

    ఒకవేళ (ConvertFromPyObject(obj, &tv) == -1)
	తిరిగి -1;

    ఒకవేళ (సూచీ == నిడివి)
    {
	ఒకవేళ (list_append_tv(l, &tv) == FAIL)
	{
	    clear_tv(&tv);
	    Py_Err_సమితి_VIM(N_("failed to add item to list"));
	    తిరిగి -1;
	}
    }
    లేకపోతే
    {
	li = list_కనుగొను(l, (దీర్ఘ) సూచీ);
	ఒకవేళ (li == ఖాళీ)
	{
	    PyErr_VIM_FORMAT(N_("internal error: failed to get Vim "
			"list item %d"), (పూర్ణాంకం) సూచీ);
	    తిరిగి -1;
	}
	clear_tv(&li->li_tv);
	copy_tv(&tv, &li->li_tv);
	clear_tv(&tv);
    }
    తిరిగి 0;
}

    స్థిర పూర్ణాంకం
List_Ass_అంశం(ListObject *self, PyObject *idx, PyObject *obj)
{
#ఒకవేళ PY_MAJOR_సంస్కరణ < 3
    ఒకవేళ (PyInt_Check(idx))
    {
	దీర్ఘ _idx = PyInt_AsLong(idx);
	తిరిగి (పూర్ణాంకం)List_Ass_సూచీ(self, _idx, obj);
    }
    లేకపోతే
#అయితే_ముగింపు
    ఒకవేళ (PyLong_Check(idx))
    {
	దీర్ఘ _idx = PyLong_AsLong(idx);
	తిరిగి (పూర్ణాంకం)List_Ass_సూచీ(self, _idx, obj);
    }
    లేకపోతే ఒకవేళ (Py_ముక్క_Check(idx))
    {
	Py_ssize_t ప్రారంభం, stop, అడుగు, slicelen;

	ఒకవేళ (Py_ముక్క_Get_Indices_Ex((Py_ముక్క_Object_T *)idx, List_నిడివి(self),
				 &ప్రారంభం, &stop, &అడుగు, &slicelen) < 0)
	    తిరిగి -1;
	తిరిగి (పూర్ణాంకం)List_Ass_ముక్క(self, ప్రారంభం, అడుగు, slicelen,
		obj);
    }
    లేకపోతే
    {
	RAISE_INVALID_సూచీ_TYPE(idx);
	తిరిగి -1;
    }
}

    స్థిర PyObject *
List_Concat_లోపల_ఉంచు(ListObject *self, PyObject *obj)
{
    list_T	*l = self->list;
    PyObject	*lookup_dict;

    ఒకవేళ (l->lv_తాళం)
    {
	RAISE_LOCKED_LIST;
	తిరిగి ఖాళీ;
    }

    ఒకవేళ (!(lookup_dict = PyDict_New()))
	తిరిగి ఖాళీ;

    ఒకవేళ (list_py_concat(l, obj, lookup_dict) == -1)
    {
	Py_DECREF(lookup_dict);
	తిరిగి ఖాళీ;
    }
    Py_DECREF(lookup_dict);

    Py_INCREF((PyObject *)self);
    తిరిగి (PyObject *)(self);
}

రకనిర్వచనం నిర్మాణం
{
    listwatch_T	lw;
    list_T	*list;
} listiterinfo_T;

    స్థిర శూన్య
ListIterDestruct(శూన్య *arg)
{
    listiterinfo_T *lii = (listiterinfo_T*)arg;
    list_rem_watch(lii->list, &lii->lw);
    list_unref(lii->list);
    PyMem_Free(lii);
}

    స్థిర PyObject *
List_Iter_తదుపరి(శూన్య **arg)
{
    PyObject	*ret;
    listiterinfo_T **lii = (listiterinfo_T**)arg;

    ఒకవేళ (!((*lii)->lw.lw_అంశం))
	తిరిగి ఖాళీ;

    ఒకవేళ (!(ret = Convert_గమ్యం_Py_Object(&((*lii)->lw.lw_అంశం->li_tv))))
	తిరిగి ఖాళీ;

    (*lii)->lw.lw_అంశం = (*lii)->lw.lw_అంశం->li_తదుపరి;

    తిరిగి ret;
}

    స్థిర PyObject *
ListIter(PyObject *self_obj)
{
    ListObject	*self = (ListObject*)self_obj;
    listiterinfo_T	*lii;
    list_T	*l = self->list;

    ఒకవేళ (!(lii = PyMem_New(listiterinfo_T, 1)))
    {
	Py_Err_No_జ్ఞాపకం();
	తిరిగి ఖాళీ;
    }

    CHECK_LIST_MATERIALIZE(l);
    list_కూడు_watch(l, &lii->lw);
    lii->lw.lw_అంశం = l->lv_మొదటి;
    lii->list = l;
    ++l->lv_refcount;

    తిరిగి IterNew(lii,
	    ListIterDestruct, List_Iter_తదుపరి,
	    ఖాళీ, ఖాళీ, (PyObject *)self);
}

స్థిర అక్షరం *ListAttrs[] = {
    "locked",
    ఖాళీ
};

    స్థిర PyObject *
ListDir(PyObject *self, PyObject *వాదనలు వాడని)
{
    తిరిగి ObjectDir(self, ListAttrs);
}

    స్థిర పూర్ణాంకం
ListSetattr(PyObject *self_obj, అక్షరం *name, PyObject *valObject)
{
    ListObject *self = (ListObject*)self_obj;
    ఒకవేళ (valObject == ఖాళీ)
    {
	Py_Err_సమితి_STRING(PyExc_AttributeError,
		N_("cannot delete vim.List attributes"));
	తిరిగి -1;
    }

    ఒకవేళ (strcmp(name, "locked") == 0)
    {
	ఒకవేళ (self->list->lv_తాళం == VAR_FIXED)
	{
	    Py_Err_సమితి_STRING(PyExc_TypeError, N_("cannot modify fixed list"));
	    తిరిగి -1;
	}
	లేకపోతే
	{
	    పూర్ణాంకం		istrue = PyObject_IsTrue(valObject);
	    ఒకవేళ (istrue == -1)
		తిరిగి -1;
	    లేకపోతే ఒకవేళ (istrue)
		self->list->lv_తాళం = VAR_LOCKED;
	    లేకపోతే
		self->list->lv_తాళం = 0;
	}
	తిరిగి 0;
    }
    లేకపోతే
    {
	PyErr_FORMAT(PyExc_AttributeError, N_("cannot set attribute %s"), name);
	తిరిగి -1;
    }
}

స్థిర PySequenceMethods ListAsSeq = {
    (lenfunc)		List_నిడివి,	 // sq_length,	  len(x)
    (binaryfunc)	0,		 // RangeConcat, sq_concat,  x+y
    0,					 // RangeRepeat, sq_repeat,  x*n
    (PyIntArgFunc)	List_సూచీ,	 // sq_item,	  x[i]
    0,					 // was_sq_slice,     x[i:j]
    (PyIntObjArgProc)	List_Ass_సూచీ,	 // sq_as_item,  x[i]=v
    0,					 // was_sq_ass_slice, x[i:j]=v
    0,					 // sq_contains
    (binaryfunc)	List_Concat_లోపల_ఉంచు,// sq_inplace_concat
    0,					 // sq_inplace_repeat
};

స్థిర PyMappingMethods ListAsMapping = {
    /* mp_length	*/ (lenfunc) List_నిడివి,
    /* mp_subscript     */ (binaryfunc) List_అంశం,
    /* mp_ass_subscript */ (objobjargproc) List_Ass_అంశం,
};

స్థిర నిర్మాణం Py_పద్ధతి_Def ListMethods[] = {
    {"extend",	(PyCFunction)List_Concat_లోపల_ఉంచు,	METH_O,		""},
    {"__dir__",	(PyCFunction)ListDir,		METH_NOARGS,	""},
    { ఖాళీ,	ఖాళీ,				0,		ఖాళీ}
};

DEFINE_PY_TYPE_OBJECT(TupleType);

రకనిర్వచనం నిర్మాణం
{
    Py_Object_తల
    tuple_T	*tuple;
    pylinkedlist_T	ref;
} TupleObject;

#నిర్వచించు NEW_TUPLE(tuple) TupleNew(TupleTypePtr, tuple)

    స్థిర PyObject *
TupleNew(PyTypeObject *subtype, tuple_T *tuple)
{
    TupleObject	*self;

    ఒకవేళ (tuple == ఖాళీ)
	తిరిగి ఖాళీ;

    self = (TupleObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    ఒకవేళ (self == ఖాళీ)
	తిరిగి ఖాళీ;
    self->tuple = tuple;
    ++tuple->tv_refcount;

    pyll_కూడు((PyObject *)(self), &self->ref, &lasttuple);

    తిరిగి (PyObject *)(self);
}

    స్థిర tuple_T *
py_tuple_alloc(శూన్య)
{
    tuple_T	*ret;

    ఒకవేళ (!(ret = tuple_alloc()))
    {
	Py_Err_No_జ్ఞాపకం();
	తిరిగి ఖాళీ;
    }
    ++ret->tv_refcount;

    తిరిగి ret;
}

    స్థిర పూర్ణాంకం
tuple_py_concat(tuple_T *t, PyObject *obj, PyObject *lookup_dict)
{
    PyObject	*iterator;
    PyObject	*అంశం;

    ఒకవేళ (!(iterator = PyObject_GetIter(obj)))
	తిరిగి -1;

    అయితే ((అంశం = Py_Iter_తదుపరి(iterator)))
    {
	typval_T    new_tv;

	ఒకవేళ (_ConvertFromPyObject(అంశం, &new_tv, lookup_dict) == -1)
	{
	    Py_DECREF(అంశం);
	    Py_DECREF(iterator);
	    తిరిగి -1;
	}

	Py_DECREF(అంశం);

	ఒకవేళ (tuple_append_tv(t, &new_tv) == FAIL)
	{
	    Py_DECREF(iterator);
	    తిరిగి -1;
	}
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    ఒకవేళ (PyErr_Occurred())
	తిరిగి -1;

    తిరిగి 0;
}

    స్థిర PyObject *
TupleConstructor(PyTypeObject *subtype, PyObject *వాదనలు, PyObject *kwargs)
{
    tuple_T	*tuple;
    PyObject	*obj = ఖాళీ;

    ఒకవేళ (kwargs)
    {
	Py_Err_సమితి_STRING(PyExc_TypeError,
		N_("tuple constructor does not accept keyword arguments"));
	తిరిగి ఖాళీ;
    }

    ఒకవేళ (!PyArg_ParseTuple(వాదనలు, "|O", &obj))
	తిరిగి ఖాళీ;

    ఒకవేళ (!(tuple = py_tuple_alloc()))
	తిరిగి ఖాళీ;

    ఒకవేళ (obj)
    {
	PyObject	*lookup_dict;

	ఒకవేళ (!(lookup_dict = PyDict_New()))
	{
	    tuple_unref(tuple);
	    తిరిగి ఖాళీ;
	}

	ఒకవేళ (tuple_py_concat(tuple, obj, lookup_dict) == -1)
	{
	    Py_DECREF(lookup_dict);
	    tuple_unref(tuple);
	    తిరిగి ఖాళీ;
	}

	Py_DECREF(lookup_dict);
    }

    తిరిగి TupleNew(subtype, tuple);
}

    స్థిర శూన్య
TupleDestructor(PyObject *self_obj)
{
    TupleObject *self = (TupleObject *)self_obj;
    pyll_remove(&self->ref, &lasttuple);
    tuple_unref(self->tuple);

    DESTRUCTOR_ముగింపు(self);
}

    స్థిర PyInt
Tuple_నిడివి(TupleObject *self)
{
    తిరిగి ((PyInt)(tuple_len(self->tuple)));
}

    స్థిర PyObject *
Tuple_సూచీ(TupleObject *self, Py_ssize_t సూచీ)
{
    PyInt	len = Tuple_నిడివి(self);

    ఒకవేళ (సూచీ < 0)
	సూచీ = len + సూచీ;

    ఒకవేళ (సూచీ < 0 || సూచీ >= len)
    {
	Py_Err_సమితి_STRING(Py_Exc_సూచీ_Error, N_("tuple index out of range"));
	తిరిగి ఖాళీ;
    }
    తిరిగి Convert_గమ్యం_Py_Object(TUPLE_అంశం(self->tuple, సూచీ));
}

/*
 * Return a new tuple object for the tuple slice starting from the index
 * "first" and of length "slicelen" skipping "step" items.
 */
    స్థిర PyObject *
Tuple_ముక్క(
    TupleObject		*self,
    Py_ssize_t		మొదటి,
    Py_ssize_t		అడుగు,
    Py_ssize_t		slicelen)
{
    PyInt	i;
    PyObject	*tuple;

    tuple = PyTuple_New(slicelen);
    ఒకవేళ (tuple == ఖాళీ)
	తిరిగి ఖాళీ;

    కొరకు (i = 0; i < slicelen; ++i)
    {
	PyObject	*అంశం;

	అంశం = Tuple_సూచీ(self, మొదటి + i * అడుగు);
	ఒకవేళ (అంశం == ఖాళీ)
	{
	    Py_DECREF(tuple);
	    తిరిగి ఖాళీ;
	}

	Py_Tuple_సమితి_అంశం(tuple, i, అంశం);
    }

    తిరిగి tuple;
}

    స్థిర PyObject *
Tuple_అంశం(TupleObject *self, PyObject* idx)
{
#ఒకవేళ PY_MAJOR_సంస్కరణ < 3
    ఒకవేళ (PyInt_Check(idx))
    {
	దీర్ఘ _idx = PyInt_AsLong(idx);
	తిరిగి Tuple_సూచీ(self, _idx);
    }
    లేకపోతే
#అయితే_ముగింపు
    ఒకవేళ (PyLong_Check(idx))
    {
	దీర్ఘ _idx = PyLong_AsLong(idx);
	తిరిగి Tuple_సూచీ(self, _idx);
    }
    లేకపోతే ఒకవేళ (Py_ముక్క_Check(idx))
    {
	Py_ssize_t ప్రారంభం, stop, అడుగు, slicelen;

	ఒకవేళ (Py_ముక్క_Get_Indices_Ex((Py_ముక్క_Object_T *)idx, Tuple_నిడివి(self),
				 &ప్రారంభం, &stop, &అడుగు, &slicelen) < 0)
	    తిరిగి ఖాళీ;
	తిరిగి Tuple_ముక్క(self, ప్రారంభం, అడుగు, slicelen);
    }
    లేకపోతే
    {
	RAISE_INVALID_సూచీ_TYPE(idx);
	తిరిగి ఖాళీ;
    }
}

రకనిర్వచనం నిర్మాణం
{
    tuple_T	*tuple;
    పూర్ణాంకం		సూచీ;
} tupleiterinfo_T;

    స్థిర శూన్య
TupleIterDestruct(శూన్య *arg)
{
    tupleiterinfo_T *tii = (tupleiterinfo_T*)arg;
    tuple_unref(tii->tuple);
    PyMem_Free(tii);
}

    స్థిర PyObject *
Tuple_Iter_తదుపరి(శూన్య **arg)
{
    PyObject	*ret;
    tupleiterinfo_T **tii = (tupleiterinfo_T**)arg;

    ఒకవేళ ((*tii)->సూచీ >= TUPLE_LEN((*tii)->tuple))
	తిరిగి ఖాళీ;

    ఒకవేళ (!(ret = Convert_గమ్యం_Py_Object(TUPLE_అంశం((*tii)->tuple, (*tii)->సూచీ))))
	తిరిగి ఖాళీ;

    (*tii)->సూచీ++;

    తిరిగి ret;
}

    స్థిర PyObject *
TupleIter(PyObject *self_obj)
{
    TupleObject	*self = (TupleObject*)self_obj;
    tupleiterinfo_T	*tii;
    tuple_T	*t = self->tuple;

    ఒకవేళ (!(tii = PyMem_New(tupleiterinfo_T, 1)))
    {
	Py_Err_No_జ్ఞాపకం();
	తిరిగి ఖాళీ;
    }

    tii->tuple = t;
    tii->సూచీ = 0;
    ++t->tv_refcount;

    తిరిగి IterNew(tii,
	    TupleIterDestruct, Tuple_Iter_తదుపరి,
	    ఖాళీ, ఖాళీ, (PyObject *)self);
}

స్థిర అక్షరం *TupleAttrs[] = {
    "locked",
    ఖాళీ
};

    స్థిర PyObject *
TupleDir(PyObject *self, PyObject *వాదనలు వాడని)
{
    తిరిగి ObjectDir(self, TupleAttrs);
}

    స్థిర పూర్ణాంకం
TupleSetattr(PyObject *self_obj, అక్షరం *name, PyObject *valObject)
{
    TupleObject *self = (TupleObject*)self_obj;
    ఒకవేళ (valObject == ఖాళీ)
    {
	Py_Err_సమితి_STRING(PyExc_AttributeError,
		N_("cannot delete vim.Tuple attributes"));
	తిరిగి -1;
    }

    ఒకవేళ (strcmp(name, "locked") == 0)
    {
	ఒకవేళ (self->tuple->tv_తాళం == VAR_FIXED)
	{
	    Py_Err_సమితి_STRING(PyExc_TypeError, N_("cannot modify fixed tuple"));
	    తిరిగి -1;
	}
	లేకపోతే
	{
	    పూర్ణాంకం		istrue = PyObject_IsTrue(valObject);
	    ఒకవేళ (istrue == -1)
		తిరిగి -1;
	    లేకపోతే ఒకవేళ (istrue)
		self->tuple->tv_తాళం = VAR_LOCKED;
	    లేకపోతే
		self->tuple->tv_తాళం = 0;
	}
	తిరిగి 0;
    }
    లేకపోతే
    {
	PyErr_FORMAT(PyExc_AttributeError, N_("cannot set attribute %s"), name);
	తిరిగి -1;
    }
}

స్థిర PySequenceMethods TupleAsSeq = {
    (lenfunc)		Tuple_నిడివి,	 // sq_length,	  len(x)
    (binaryfunc)	0,		 // RangeConcat, sq_concat,  x+y
    0,					 // RangeRepeat, sq_repeat,  x*n
    (PyIntArgFunc)	Tuple_సూచీ,	 // sq_item,	  x[i]
    0,					 // was_sq_slice,     x[i:j]
    (PyIntObjArgProc)	0,		 // sq_as_item,  x[i]=v
    0,					 // was_sq_ass_slice, x[i:j]=v
    0,					 // sq_contains
    (binaryfunc)	0,		 // sq_inplace_concat
    0,					 // sq_inplace_repeat
};

స్థిర PyMappingMethods TupleAsMapping = {
    /* mp_length	*/ (lenfunc) Tuple_నిడివి,
    /* mp_subscript     */ (binaryfunc) Tuple_అంశం,
    /* mp_ass_subscript */ (objobjargproc) 0,
};

స్థిర నిర్మాణం Py_పద్ధతి_Def TupleMethods[] = {
    {"__dir__",	(PyCFunction)TupleDir,		METH_NOARGS,	""},
    { ఖాళీ,	ఖాళీ,				0,		ఖాళీ}
};

రకనిర్వచనం నిర్మాణం
{
    Py_Object_తల
    char_u	*name;
    పూర్ణాంకం		argc;
    typval_T	*argv;
    dict_T	*self;
    pylinkedlist_T	ref;
    పూర్ణాంకం		auto_rebind;
} FunctionObject;

DEFINE_PY_TYPE_OBJECT(FunctionType);

#నిర్వచించు NEW_FUNCTION(name, argc, argv, self, pt_auto) \
    FunctionNew(FunctionTypePtr, (name), (argc), (argv), (self), (pt_auto))

    స్థిర PyObject *
FunctionNew(PyTypeObject *subtype, char_u *name, పూర్ణాంకం argc, typval_T *argv,
	dict_T *selfdict, పూర్ణాంకం auto_rebind)
{
    FunctionObject	*self;

    self = (FunctionObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    ఒకవేళ (self == ఖాళీ)
	తిరిగి ఖాళీ;

    ఒకవేళ (అంకెనా((గుర్తులేని అక్షరం)*name))
    {
	ఒకవేళ (!translated_function_exists(name, FALSE))
	{
	    PyErr_FORMAT(Py_Exc_విలువ_Error,
		    N_("unnamed function %s does not exist"), name);
	    తిరిగి ఖాళీ;
	}
	self->name = vim_strsave(name);
    }
    లేకపోతే
    {
	char_u *p;

	ఒకవేళ ((p = get_expanded_name(name,
			    vim_strchr(name, AUTOLOAD_CHAR) == ఖాళీ)) == ఖాళీ)
	{
	    PyErr_FORMAT(Py_Exc_విలువ_Error,
		    N_("function %s does not exist"), name);
	    తిరిగి ఖాళీ;
	}

	ఒకవేళ (p[0] == K_SPECIAL && p[1] == KS_EXTRA && p[2] == (పూర్ణాంకం)KE_SNR)
	{
	    char_u *np;
	    size_t len = STRLEN(p) + 1;

	    ఒకవేళ ((np = alloc(len + 2)) == ఖాళీ)
	    {
		vim_free(p);
		తిరిగి ఖాళీ;
	    }
	    mch_memmove(np, "<SNR>", 5);
	    mch_memmove(np + 5, p + 3, len - 3);
	    vim_free(p);
	    self->name = np;
	}
	లేకపోతే
	    self->name = p;
    }

    func_ref(self->name);
    self->argc = argc;
    self->argv = argv;
    self->self = selfdict;
    self->auto_rebind = selfdict == ఖాళీ ? TRUE : auto_rebind;

    ఒకవేళ (self->argv || self->self)
	pyll_కూడు((PyObject *)(self), &self->ref, &lastfunc);

    తిరిగి (PyObject *)(self);
}

    స్థిర PyObject *
FunctionConstructor(PyTypeObject *subtype, PyObject *వాదనలు, PyObject *kwargs)
{
    PyObject	*self;
    PyObject	*selfdictObject;
    PyObject	*autoRebindObject;
    PyObject	*వాదనలు_Object = ఖాళీ;
    char_u	*name;
    typval_T	selfdicttv;
    typval_T	argstv;
    list_T	*argslist = ఖాళీ;
    dict_T	*selfdict = ఖాళీ;
    పూర్ణాంకం		argc = 0;
    పూర్ణాంకం		auto_rebind = TRUE;
    typval_T	*argv = ఖాళీ;
    typval_T	*curtv;
    listitem_T	*li;

    ఒకవేళ (kwargs != ఖాళీ)
    {
	selfdictObject = Py_Dict_Get_అంశం_String(kwargs, "self");
	ఒకవేళ (selfdictObject != ఖాళీ)
	{
	    ఒకవేళ (ConvertFromPyMapping(selfdictObject, &selfdicttv) == -1)
		తిరిగి ఖాళీ;
	    selfdict = selfdicttv.vval.v_dict;
	}
	వాదనలు_Object = Py_Dict_Get_అంశం_String(kwargs, "args");
	ఒకవేళ (వాదనలు_Object != ఖాళీ)
	{
	    ఒకవేళ (ConvertFromPySequence(వాదనలు_Object, &argstv) == -1)
	    {
		dict_unref(selfdict);
		తిరిగి ఖాళీ;
	    }
	    argslist = argstv.vval.v_list;
	    CHECK_LIST_MATERIALIZE(argslist);

	    argc = argslist->lv_len;
	    ఒకవేళ (argc != 0)
	    {
		argv = PyMem_New(typval_T, (size_t) argc);
		ఒకవేళ (argv == ఖాళీ)
		{
		    Py_Err_No_జ్ఞాపకం();
		    dict_unref(selfdict);
		    list_unref(argslist);
		    తిరిగి ఖాళీ;
		}
		curtv = argv;
		FOR_ALL_LIST_వస్తువులు(argslist, li)
		    copy_tv(&li->li_tv, curtv++);
	    }
	    list_unref(argslist);
	}
	ఒకవేళ (selfdict != ఖాళీ)
	{
	    auto_rebind = FALSE;
	    autoRebindObject = Py_Dict_Get_అంశం_String(kwargs, "auto_rebind");
	    ఒకవేళ (autoRebindObject != ఖాళీ)
	    {
		auto_rebind = PyObject_IsTrue(autoRebindObject);
		ఒకవేళ (auto_rebind == -1)
		{
		    dict_unref(selfdict);
		    list_unref(argslist);
		    తిరిగి ఖాళీ;
		}
	    }
	}
    }

    ఒకవేళ (!PyArg_ParseTuple(వాదనలు, "et", "ascii", &name))
    {
	dict_unref(selfdict);
	అయితే (argc--)
	    clear_tv(&argv[argc]);
	PyMem_Free(argv);
	తిరిగి ఖాళీ;
    }

    self = FunctionNew(subtype, name, argc, argv, selfdict, auto_rebind);

    PyMem_Free(name);

    తిరిగి self;
}

    స్థిర శూన్య
FunctionDestructor(PyObject *self_obj)
{
    FunctionObject *self = (FunctionObject*)self_obj;
    పూర్ణాంకం i;
    func_unref(self->name);
    vim_free(self->name);
    కొరకు (i = 0; i < self->argc; ++i)
	clear_tv(&self->argv[i]);
    PyMem_Free(self->argv);
    dict_unref(self->self);
    ఒకవేళ (self->argv || self->self)
	pyll_remove(&self->ref, &lastfunc);

    DESTRUCTOR_ముగింపు(self);
}

స్థిర అక్షరం *FunctionAttrs[] = {
    "softspace", "args", "self", "auto_rebind",
    ఖాళీ
};

    స్థిర PyObject *
FunctionDir(PyObject *self, PyObject *వాదనలు వాడని)
{
    తిరిగి ObjectDir(self, FunctionAttrs);
}

    స్థిర PyObject *
FunctionAttr(FunctionObject *self, అక్షరం *name)
{
    list_T *list;
    పూర్ణాంకం i;
    ఒకవేళ (strcmp(name, "name") == 0)
	తిరిగి PyString_FromString((అక్షరం *)(self->name));
    లేకపోతే ఒకవేళ (strcmp(name, "args") == 0)
    {
	ఒకవేళ (self->argv == ఖాళీ || (list = list_alloc()) == ఖాళీ)
	    తిరిగి ALWAYS_NONE;

	కొరకు (i = 0; i < self->argc; ++i)
	    list_append_tv(list, &self->argv[i]);
	తిరిగి NEW_LIST(list);
    }
    లేకపోతే ఒకవేళ (strcmp(name, "self") == 0)
	తిరిగి self->self == ఖాళీ
	    ? ALWAYS_NONE
	    : NEW_DICTIONARY(self->self);
    లేకపోతే ఒకవేళ (strcmp(name, "auto_rebind") == 0)
	తిరిగి self->auto_rebind
	    ? ALWAYS_TRUE
	    : ALWAYS_FALSE;
    లేకపోతే ఒకవేళ (strcmp(name, "__members__") == 0)
	తిరిగి ObjectDir(ఖాళీ, FunctionAttrs);
    తిరిగి ఖాళీ;
}

/*
 * Populate partial_T given function object.
 *
 * "exported" should be set to true when it is needed to construct a partial
 * that may be stored in a variable (i.e. may be freed by Vim).
 */
    స్థిర శూన్య
సమితి_partial(FunctionObject *self, partial_T *pt, పూర్ణాంకం exported)
{
    పూర్ణాంకం i;

    pt->pt_name = self->name;
    ఒకవేళ (self->argv)
    {
	pt->pt_argc = self->argc;
	ఒకవేళ (exported)
	{
	    pt->pt_argv = ALLOC_CLEAR_MULT(typval_T, self->argc);
	    కొరకు (i = 0; i < pt->pt_argc; ++i)
		copy_tv(&self->argv[i], &pt->pt_argv[i]);
	}
	లేకపోతే
	    pt->pt_argv = self->argv;
    }
    లేకపోతే
    {
	pt->pt_argc = 0;
	pt->pt_argv = ఖాళీ;
    }
    pt->pt_auto = self->auto_rebind || !exported;
    pt->pt_dict = self->self;
    ఒకవేళ (exported && self->self)
	++pt->pt_dict->dv_refcount;
    ఒకవేళ (exported)
	pt->pt_name = vim_strsave(pt->pt_name);
    pt->pt_refcount = 1;
}

    స్థిర PyObject *
FunctionCall(PyObject *self_obj, PyObject *వాదనలు_Object, PyObject *kwargs)
{
    FunctionObject *self = (FunctionObject*)self_obj;
    char_u	*name = self->name;
    typval_T	వాదనలు;
    typval_T	selfdicttv;
    typval_T	rettv;
    dict_T	*selfdict = ఖాళీ;
    PyObject	*selfdictObject;
    PyObject	*ret;
    పూర్ణాంకం		error;
    partial_T	pt;
    partial_T	*pt_ptr = ఖాళీ;

    ఒకవేళ (ConvertFromPySequence(వాదనలు_Object, &వాదనలు) == -1)
	తిరిగి ఖాళీ;

    ఒకవేళ (kwargs != ఖాళీ)
    {
	selfdictObject = Py_Dict_Get_అంశం_String(kwargs, "self");
	ఒకవేళ (selfdictObject != ఖాళీ)
	{
	    ఒకవేళ (ConvertFromPyMapping(selfdictObject, &selfdicttv) == -1)
	    {
		clear_tv(&వాదనలు);
		తిరిగి ఖాళీ;
	    }
	    selfdict = selfdicttv.vval.v_dict;
	}
    }

    ఒకవేళ (self->argv || self->self)
    {
	CLEAR_క్షేత్రం(pt);
	సమితి_partial(self, &pt, FALSE);
	pt_ptr = &pt;
    }

    Py_BEGIN_ALLOW_దారాలు
    Python_తాళం_Vim();

    Vim_Try_ప్రారంభం();
    error = func_call(name, &వాదనలు, pt_ptr, selfdict, &rettv);

    Python_Release_Vim();
    Py_చివర_ALLOW_దారాలు

    ఒకవేళ (Vim_Try_చివర())
	ret = ఖాళీ;
    లేకపోతే ఒకవేళ (error != OK)
    {
	ret = ఖాళీ;
	PyErr_VIM_FORMAT(N_("failed to run function %s"), (అక్షరం *)name);
    }
    లేకపోతే
	ret = Convert_గమ్యం_Py_Object(&rettv);

    clear_tv(&వాదనలు);
    clear_tv(&rettv);
    ఒకవేళ (selfdict != ఖాళీ)
	clear_tv(&selfdicttv);

    తిరిగి ret;
}

    స్థిర PyObject *
FunctionRepr(PyObject *self_obj)
{
    FunctionObject *self = (FunctionObject*)self_obj;
    PyObject *ret;
    garray_T repr_ga;
    పూర్ణాంకం i;
    char_u *tofree = ఖాళీ;
    typval_T tv;
    char_u numbuf[NUMBUFLEN];

    ga_init2(&repr_ga, పరిమాణం(అక్షరం), 70);
    ga_concat(&repr_ga, (char_u *)"<vim.Function '");
    ఒకవేళ (self->name)
	ga_concat(&repr_ga, self->name);
    లేకపోతే
	ga_concat(&repr_ga, (char_u *)"<NULL>");
    ga_append(&repr_ga, '\'');
    ఒకవేళ (self->argv)
    {
	ga_concat(&repr_ga, (char_u *)", args=[");
	++emsg_silent;
	కొరకు (i = 0; i < self->argc; i++)
	{
	    ఒకవేళ (i != 0)
		ga_concat(&repr_ga, (char_u *)", ");
	    ga_concat(&repr_ga, tv2string(&self->argv[i], &tofree, numbuf,
			get_copyID()));
	    vim_free(tofree);
	}
	--emsg_silent;
	ga_append(&repr_ga, ']');
    }
    ఒకవేళ (self->self)
    {
	ga_concat(&repr_ga, (char_u *)", self=");
	tv.v_type = VAR_DICT;
	tv.vval.v_dict = self->self;
	++emsg_silent;
	ga_concat(&repr_ga, tv2string(&tv, &tofree, numbuf, get_copyID()));
	--emsg_silent;
	vim_free(tofree);
	ఒకవేళ (self->auto_rebind)
	    ga_concat(&repr_ga, (char_u *)", auto_rebind=True");
    }
    ga_append(&repr_ga, '>');
    ret = PyString_FromString((అక్షరం *)repr_ga.ga_దత్తాంశం);
    ga_clear(&repr_ga);
    తిరిగి ret;
}

స్థిర నిర్మాణం Py_పద్ధతి_Def FunctionMethods[] = {
    {"__dir__",	(PyCFunction)FunctionDir,   METH_NOARGS,		""},
    { ఖాళీ,	ఖాళీ,			0,				ఖాళీ}
};

/*
 * Options object
 */

DEFINE_PY_TYPE_OBJECT(OptionsType);

రకనిర్వచనం పూర్ణాంకం (*checkfun)(శూన్య *);

రకనిర్వచనం నిర్మాణం
{
    Py_Object_తల
    పూర్ణాంకం		opt_type;
    శూన్య	*from;
    checkfun	Check;
    PyObject	*fromObj;
} OptionsObject;

    స్థిర పూర్ణాంకం
dummy_check(శూన్య *arg వాడని)
{
    తిరిగి 0;
}

    స్థిర PyObject *
OptionsNew(పూర్ణాంకం opt_type, శూన్య *from, checkfun Check, PyObject *fromObj)
{
    OptionsObject	*self;

    self = PyObject_GC_New(OptionsObject, OptionsTypePtr);
    ఒకవేళ (self == ఖాళీ)
	తిరిగి ఖాళీ;

    self->opt_type = opt_type;
    self->from = from;
    self->Check = Check;
    self->fromObj = fromObj;
    ఒకవేళ (fromObj)
	Py_INCREF(fromObj);

    తిరిగి (PyObject *)(self);
}

    స్థిర శూన్య
OptionsDestructor(PyObject *self_obj)
{
    OptionsObject *self = (OptionsObject*)self_obj;
    PyObject_GC_UnTrack((శూన్య *)(self));
    Py_XDECREF(self->fromObj);
    PyObject_GC_Del((శూన్య *)(self));
}

    స్థిర పూర్ణాంకం
OptionsTraverse(PyObject *self_obj, visitproc సందర్శించు, శూన్య *arg)
{
    OptionsObject *self = (OptionsObject*)self_obj;
    Py_సందర్శించు(self->fromObj);
    తిరిగి 0;
}

    స్థిర పూర్ణాంకం
OptionsClear(PyObject *self_obj)
{
    OptionsObject *self = (OptionsObject*)self_obj;
    Py_CLEAR(self->fromObj);
    తిరిగి 0;
}

    స్థిర PyObject *
Options_అంశం(OptionsObject *self, PyObject *కీలకం_Object)
{
    char_u	*కీలకం;
    పూర్ణాంకం		flags;
    దీర్ఘ	numval;
    char_u	*stringval;
    PyObject	*todecref;

    ఒకవేళ (self->Check(self->fromObj))
	తిరిగి ఖాళీ;

    ఒకవేళ (!(కీలకం = String_గమ్యం_Chars(కీలకం_Object, &todecref)))
	తిరిగి ఖాళీ;

    ఒకవేళ (*కీలకం == NUL)
    {
	RAISE_NO_ఖాళీలు_కీలకాలు;
	Py_XDECREF(todecref);
	తిరిగి ఖాళీ;
    }

    flags = get_option_విలువ_strict(కీలకం, &numval, &stringval,
				    self->opt_type, self->from);

    Py_XDECREF(todecref);

    ఒకవేళ (flags == 0)
    {
	Py_Err_సమితి_Object(Py_Exc_కీలకం_Error, కీలకం_Object);
	తిరిగి ఖాళీ;
    }

    ఒకవేళ (flags & SOPT_UNSET)
    {
	Py_INCREF(Py_None);
	తిరిగి Py_None;
    }
    లేకపోతే ఒకవేళ (flags & SOPT_BOOL)
    {
	PyObject	*ret;
	ret = numval ? Py_True : Py_False;
	Py_INCREF(ret);
	తిరిగి ret;
    }
    లేకపోతే ఒకవేళ (flags & SOPT_అంకె)
	తిరిగి PyInt_FromLong(numval);
    లేకపోతే ఒకవేళ (flags & SOPT_STRING)
    {
	ఒకవేళ (stringval)
	{
	    PyObject	*ret = Py_బైట్లు_From_String((అక్షరం *)stringval);
	    vim_free(stringval);
	    తిరిగి ret;
	}
	లేకపోతే
	{
	    Py_Err_సమితి_STRING(PyExc_RuntimeError,
		    N_("unable to get option value"));
	    తిరిగి ఖాళీ;
	}
    }
    లేకపోతే
    {
	Py_Err_సమితి_VIM(N_("internal error: unknown option type"));
	తిరిగి ఖాళీ;
    }
}

    స్థిర పూర్ణాంకం
Options_కలిగియుంది(OptionsObject *self, PyObject *కీలకం_Object)
{
    char_u	*కీలకం;
    PyObject	*todecref;

    ఒకవేళ (!(కీలకం = String_గమ్యం_Chars(కీలకం_Object, &todecref)))
	తిరిగి -1;

    ఒకవేళ (*కీలకం == NUL)
    {
	Py_XDECREF(todecref);
	తిరిగి 0;
    }

    ఒకవేళ (get_option_విలువ_strict(కీలకం, ఖాళీ, ఖాళీ, self->opt_type, ఖాళీ))
    {
	Py_XDECREF(todecref);
	తిరిగి 1;
    }
    లేకపోతే
    {
	Py_XDECREF(todecref);
	తిరిగి 0;
    }
}

రకనిర్వచనం నిర్మాణం
{
    శూన్య	*lastoption;
    పూర్ణాంకం		opt_type;
} optiterinfo_T;

    స్థిర PyObject *
Options_Iter_తదుపరి(శూన్య **arg)
{
    char_u	*name;
    optiterinfo_T **oii = (optiterinfo_T**)arg;

    ఒకవేళ ((name = option_iter_తదుపరి(&((*oii)->lastoption), (*oii)->opt_type)))
	తిరిగి PyString_FromString((అక్షరం *)name);

    తిరిగి ఖాళీ;
}

    స్థిర PyObject *
OptionsIter(PyObject *self_obj)
{
    OptionsObject	*self = (OptionsObject*)self_obj;
    optiterinfo_T	*oii;

    ఒకవేళ (!(oii = PyMem_New(optiterinfo_T, 1)))
    {
	Py_Err_No_జ్ఞాపకం();
	తిరిగి ఖాళీ;
    }

    oii->opt_type = self->opt_type;
    oii->lastoption = ఖాళీ;

    తిరిగి IterNew(oii,
	    PyMem_Free, Options_Iter_తదుపరి,
	    ఖాళీ, ఖాళీ, (PyObject *)self);
}

    స్థిర పూర్ణాంకం
సమితి_option_విలువ_err(char_u *కీలకం, పూర్ణాంకం numval, char_u *stringval, పూర్ణాంకం opt_flags)
{
    అక్షరం	*errmsg;

    ఒకవేళ ((errmsg = సమితి_option_విలువ(కీలకం, numval, stringval, opt_flags)))
    {
	ఒకవేళ (Vim_Try_చివర())
	    తిరిగి FAIL;
	Py_Err_సమితి_Vim(errmsg);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

    స్థిర పూర్ణాంకం
సమితి_option_విలువ_for(
	char_u *కీలకం,
	పూర్ణాంకం numval,
	char_u *stringval,
	పూర్ణాంకం opt_flags,
	పూర్ణాంకం opt_type,
	శూన్య *from)
{
    switchwin_T	switchwin;
    bufref_T	save_curbuf;
    పూర్ణాంకం		సమితి_ret = 0;

    Vim_Try_ప్రారంభం();
    ఎంపిక (opt_type)
    {
	స్థితి SREQ_WIN:
	    ఒకవేళ (switch_win(&switchwin, (win_T *)from,
			      win_కనుగొను_tabpage((win_T *)from), FALSE) == FAIL)
	    {
		restore_win(&switchwin, TRUE);
		ఒకవేళ (Vim_Try_చివర())
		    తిరిగి -1;
		Py_Err_సమితి_VIM(N_("problem while switching windows"));
		తిరిగి -1;
	    }
	    సమితి_ret = సమితి_option_విలువ_err(కీలకం, numval, stringval, opt_flags);
	    restore_win(&switchwin, TRUE);
	    ఆపు;
	స్థితి SREQ_BUF:
	    switch_బఫరు(&save_curbuf, (buf_T *)from);
	    సమితి_ret = సమితి_option_విలువ_err(కీలకం, numval, stringval, opt_flags);
	    restore_బఫరు(&save_curbuf);
	    ఆపు;
	స్థితి SREQ_GLOBAL:
	    సమితి_ret = సమితి_option_విలువ_err(కీలకం, numval, stringval, opt_flags);
	    ఆపు;
    }
    ఒకవేళ (సమితి_ret == FAIL)
	తిరిగి -1;
    తిరిగి Vim_Try_చివర();
}

    స్థిర పూర్ణాంకం
Options_Ass_అంశం(OptionsObject *self, PyObject *కీలకం_Object, PyObject *valObject)
{
    char_u	*కీలకం;
    పూర్ణాంకం		flags;
    పూర్ణాంకం		opt_flags;
    పూర్ణాంకం		ret = 0;
    PyObject	*todecref;

    ఒకవేళ (self->Check(self->fromObj))
	తిరిగి -1;

    ఒకవేళ (!(కీలకం = String_గమ్యం_Chars(కీలకం_Object, &todecref)))
	తిరిగి -1;

    ఒకవేళ (*కీలకం == NUL)
    {
	RAISE_NO_ఖాళీలు_కీలకాలు;
	Py_XDECREF(todecref);
	తిరిగి -1;
    }

    flags = get_option_విలువ_strict(కీలకం, ఖాళీ, ఖాళీ,
				    self->opt_type, self->from);

    ఒకవేళ (flags == 0)
    {
	Py_Err_సమితి_Object(Py_Exc_కీలకం_Error, కీలకం_Object);
	Py_XDECREF(todecref);
	తిరిగి -1;
    }

    ఒకవేళ (valObject == ఖాళీ)
    {
	ఒకవేళ (self->opt_type == SREQ_GLOBAL)
	{
	    PyErr_FORMAT(Py_Exc_విలువ_Error,
		    N_("unable to unset global option %s"), కీలకం);
	    Py_XDECREF(todecref);
	    తిరిగి -1;
	}
	లేకపోతే ఒకవేళ (!(flags & SOPT_GLOBAL))
	{
	    PyErr_FORMAT(Py_Exc_విలువ_Error,
		    N_("unable to unset option %s "
		       "which does not have global value"), కీలకం);
	    Py_XDECREF(todecref);
	    తిరిగి -1;
	}
	లేకపోతే
	{
	    unset_global_local_option(కీలకం, self->from);
	    Py_XDECREF(todecref);
	    తిరిగి 0;
	}
    }

    opt_flags = (self->opt_type ? OPT_LOCAL : OPT_GLOBAL);

    ఒకవేళ (flags & SOPT_BOOL)
    {
	పూర్ణాంకం	istrue = PyObject_IsTrue(valObject);

	ఒకవేళ (istrue == -1)
	    ret = -1;
	లేకపోతే
	    ret = సమితి_option_విలువ_for(కీలకం, istrue, ఖాళీ,
				    opt_flags, self->opt_type, self->from);
    }
    లేకపోతే ఒకవేళ (flags & SOPT_అంకె)
    {
	దీర్ఘ	val;

	ఒకవేళ (సంఖ్య_గమ్యం_Long(valObject, &val, సంఖ్య_INT))
	{
	    Py_XDECREF(todecref);
	    తిరిగి -1;
	}

	ret = సమితి_option_విలువ_for(కీలకం, (పూర్ణాంకం) val, ఖాళీ, opt_flags,
				self->opt_type, self->from);
    }
    లేకపోతే
    {
	char_u		*val;
	PyObject	*todecref2;

	ఒకవేళ ((val = String_గమ్యం_Chars(valObject, &todecref2)))
	{
	    ret = సమితి_option_విలువ_for(కీలకం, 0, val, opt_flags,
				    self->opt_type, self->from);
	    Py_XDECREF(todecref2);
	}
	లేకపోతే
	    ret = -1;
    }

    Py_XDECREF(todecref);

    తిరిగి ret;
}

స్థిర PySequenceMethods OptionsAsSeq = {
    0,					// sq_length
    0,					// sq_concat
    0,					// sq_repeat
    0,					// sq_item
    0,					// sq_slice
    0,					// sq_ass_item
    0,					// sq_ass_slice
    (objobjproc) Options_కలిగియుంది,	// sq_contains
    0,					// sq_inplace_concat
    0,					// sq_inplace_repeat
};

స్థిర PyMappingMethods OptionsAsMapping = {
    (lenfunc)       ఖాళీ,
    (binaryfunc)    Options_అంశం,
    (objobjargproc) Options_Ass_అంశం,
};

// Tabpage object

రకనిర్వచనం నిర్మాణం
{
    Py_Object_తల
    tabpage_T	*tab;
} Tab_పుట_Object;

స్థిర PyObject *WinListNew(Tab_పుట_Object *tabObject);

DEFINE_PY_TYPE_OBJECT(Tab_పుట_Type);

    స్థిర పూర్ణాంకం
Check_Tab_పుట(Tab_పుట_Object *self)
{
    ఒకవేళ (self->tab == INVALID_TABPAGE_విలువ)
    {
	Py_Err_సమితి_VIM(N_("attempt to refer to deleted tab page"));
	తిరిగి -1;
    }

    తిరిగి 0;
}

    స్థిర PyObject *
Tab_పుట_New(tabpage_T *tab)
{
    Tab_పుట_Object *self;

    ఒకవేళ (TAB_PYTHON_REF(tab))
    {
	self = TAB_PYTHON_REF(tab);
	Py_INCREF((PyObject *)self);
    }
    లేకపోతే
    {
	self = PyObject_NEW(Tab_పుట_Object, Tab_పుట_Type_Ptr);
	ఒకవేళ (self == ఖాళీ)
	    తిరిగి ఖాళీ;
	self->tab = tab;
	TAB_PYTHON_REF(tab) = self;
    }

    తిరిగి (PyObject *)(self);
}

    స్థిర శూన్య
Tab_పుట_Destructor(PyObject *self_obj)
{
    Tab_పుట_Object *self = (Tab_పుట_Object*)self_obj;
    ఒకవేళ (self->tab && self->tab != INVALID_TABPAGE_విలువ)
	TAB_PYTHON_REF(self->tab) = ఖాళీ;

    DESTRUCTOR_ముగింపు(self);
}

స్థిర అక్షరం *Tab_పుట_Attrs[] = {
    "windows", "number", "vars", "window", "valid",
    ఖాళీ
};

    స్థిర PyObject *
Tab_పుట_Dir(PyObject *self, PyObject *వాదనలు వాడని)
{
    తిరిగి ObjectDir(self, Tab_పుట_Attrs);
}

    స్థిర PyObject *
Tab_పుట_Attr_Valid(Tab_పుట_Object *self, అక్షరం *name)
{
    PyObject	*ret;

    ఒకవేళ (strcmp(name, "valid") != 0)
	తిరిగి ఖాళీ;

    ret = ((self->tab == INVALID_TABPAGE_విలువ) ? Py_False : Py_True);
    Py_INCREF(ret);
    తిరిగి ret;
}

    స్థిర PyObject *
Tab_పుట_Attr(Tab_పుట_Object *self, అక్షరం *name)
{
    ఒకవేళ (strcmp(name, "windows") == 0)
	తిరిగి WinListNew(self);
    లేకపోతే ఒకవేళ (strcmp(name, "number") == 0)
	తిరిగి PyLong_FromLong((దీర్ఘ) get_tab_సంఖ్య(self->tab));
    లేకపోతే ఒకవేళ (strcmp(name, "vars") == 0)
	తిరిగి NEW_DICTIONARY(self->tab->tp_vars);
    లేకపోతే ఒకవేళ (strcmp(name, "window") == 0)
    {
	// For current tab window.c does not bother to set or update tp_curwin
	ఒకవేళ (self->tab == curtab)
	    తిరిగి కిటికీ_New(curwin, curtab);
	లేకపోతే
	    తిరిగి కిటికీ_New(self->tab->tp_curwin, self->tab);
    }
    లేకపోతే ఒకవేళ (strcmp(name, "__members__") == 0)
	తిరిగి ObjectDir(ఖాళీ, Tab_పుట_Attrs);
    తిరిగి ఖాళీ;
}

    స్థిర PyObject *
Tab_పుట_Repr(PyObject *self_obj)
{
    Tab_పుట_Object *self = (Tab_పుట_Object*)self_obj;
    ఒకవేళ (self->tab == INVALID_TABPAGE_విలువ)
	తిరిగి PyString_FromFormat("<tabpage object (deleted) at %p>", (శూన్య *)self);
    లేకపోతే
    {
	పూర్ణాంకం	t = get_tab_సంఖ్య(self->tab);

	ఒకవేళ (t == 0)
	    తిరిగి PyString_FromFormat("<tabpage object (unknown) at %p>",
					(శూన్య *)self);
	లేకపోతే
	    తిరిగి PyString_FromFormat("<tabpage %d>", t - 1);
    }
}

స్థిర నిర్మాణం Py_పద్ధతి_Def Tab_పుట_Methods[] = {
    // name,	    function,			calling,	documentation
    {"__dir__",	    (PyCFunction)Tab_పుట_Dir,	METH_NOARGS,	""},
    { ఖాళీ,	    ఖాళీ,			0,		ఖాళీ}
};

/*
 * Window list object
 */

DEFINE_PY_TYPE_OBJECT(TabListType);
స్థిర PySequenceMethods TabListAsSeq;

రకనిర్వచనం నిర్మాణం
{
    Py_Object_తల
} TabListObject;

స్థిర TabListObject The_Tab_పుట_List =
{
    Py_Object_తల_INIT_TYPE(TabListType)
};

    స్థిర PyInt
Tab_List_నిడివి(PyObject *self వాడని)
{
    tabpage_T	*tp = మొదటి_tabpage;
    PyInt	n = 0;

    అయితే (tp != ఖాళీ)
    {
	++n;
	tp = tp->tp_తదుపరి;
    }

    తిరిగి n;
}

    స్థిర PyObject *
Tab_List_అంశం(PyObject *self వాడని, PyInt n)
{
    tabpage_T	*tp;

    కొరకు (tp = మొదటి_tabpage; tp != ఖాళీ; tp = tp->tp_తదుపరి, --n)
	ఒకవేళ (n == 0)
	    తిరిగి Tab_పుట_New(tp);

    Py_Err_సమితి_STRING(Py_Exc_సూచీ_Error, N_("no such tab page"));
    తిరిగి ఖాళీ;
}

/*
 * Window object
 */

రకనిర్వచనం నిర్మాణం
{
    Py_Object_తల
    win_T	*win;
    Tab_పుట_Object	*tabObject;
} కిటికీ_Object;

DEFINE_PY_TYPE_OBJECT(కిటికీ_Type);

    స్థిర పూర్ణాంకం
Check_కిటికీ(కిటికీ_Object *self)
{
    ఒకవేళ (self->win == INVALID_కిటికీ_విలువ)
    {
	Py_Err_సమితి_VIM(N_("attempt to refer to deleted window"));
	తిరిగి -1;
    }

    తిరిగి 0;
}

    స్థిర పూర్ణాంకం
Check_కిటికీ_Cb(శూన్య *self)
{
    తిరిగి Check_కిటికీ((కిటికీ_Object*)self);
}

    స్థిర PyObject *
కిటికీ_New(win_T *win, tabpage_T *tab)
{
    /*
     * We need to handle deletion of windows underneath us.
     * If we add a "w_python*_ref" field to the win_T structure,
     * then we can get at it in win_free() in vim. We then
     * need to create only ONE Python object per window - if
     * we try to create a second, just INCREF the existing one
     * and return it. The (single) Python object referring to
     * the window is stored in "w_python*_ref".
     * On a win_free() we set the Python object's win_T* field
     * to an invalid value. We trap all uses of a window
     * object, and reject them if the win_T* field is invalid.
     *
     * Python2 and Python3 get different fields and different objects:
     * w_python_ref and w_python3_ref fields respectively.
     */

    కిటికీ_Object *self;

    ఒకవేళ (WIN_PYTHON_REF(win))
    {
	self = WIN_PYTHON_REF(win);
	Py_INCREF((PyObject *)self);
    }
    లేకపోతే
    {
	self = PyObject_GC_New(కిటికీ_Object, కిటికీ_Type_Ptr);
	ఒకవేళ (self == ఖాళీ)
	    తిరిగి ఖాళీ;
	self->win = win;
	WIN_PYTHON_REF(win) = self;
    }

    self->tabObject = ((Tab_పుట_Object *)(Tab_పుట_New(tab)));

    తిరిగి (PyObject *)(self);
}

    స్థిర శూన్య
కిటికీ_Destructor(PyObject *self_obj)
{
    కిటికీ_Object *self = (కిటికీ_Object*)self_obj;
    PyObject_GC_UnTrack((శూన్య *)(self));
    ఒకవేళ (self->win && self->win != INVALID_కిటికీ_విలువ)
	WIN_PYTHON_REF(self->win) = ఖాళీ;
    Py_XDECREF(((PyObject *)(self->tabObject)));
    PyObject_GC_Del((శూన్య *)(self));
}

    స్థిర పూర్ణాంకం
కిటికీ_Traverse(PyObject *self_obj, visitproc సందర్శించు, శూన్య *arg)
{
    కిటికీ_Object *self = (కిటికీ_Object*)self_obj;
    Py_సందర్శించు(((PyObject *)(self->tabObject)));
    తిరిగి 0;
}

    స్థిర పూర్ణాంకం
కిటికీ_Clear(PyObject *self_obj)
{
    కిటికీ_Object *self = (కిటికీ_Object*)self_obj;
    Py_CLEAR(self->tabObject);
    తిరిగి 0;
}

    స్థిర win_T *
get_firstwin(Tab_పుట_Object *tabObject)
{
    ఒకవేళ (tabObject)
    {
	ఒకవేళ (Check_Tab_పుట(tabObject))
	    తిరిగి ఖాళీ;
	// For current tab window.c does not bother to set or update tp_firstwin
	లేకపోతే ఒకవేళ (tabObject->tab == curtab)
	    తిరిగి firstwin;
	లేకపోతే
	    తిరిగి tabObject->tab->tp_firstwin;
    }
    లేకపోతే
	తిరిగి firstwin;
}

// Use the same order as in the WindowAttr() function.
స్థిర అక్షరం *కిటికీ_Attrs[] = {
    "buffer",
    "cursor",
    "height",
    "row",
    "width",
    "col",
    "vars",
    "options",
    "number",
    "tabpage",
    "valid",
    ఖాళీ
};

    స్థిర PyObject *
కిటికీ_Dir(PyObject *self, PyObject *వాదనలు వాడని)
{
    తిరిగి ObjectDir(self, కిటికీ_Attrs);
}

    స్థిర PyObject *
కిటికీ_Attr_Valid(కిటికీ_Object *self, అక్షరం *name)
{
    PyObject	*ret;

    ఒకవేళ (strcmp(name, "valid") != 0)
	తిరిగి ఖాళీ;

    ret = ((self->win == INVALID_కిటికీ_విలువ) ? Py_False : Py_True);
    Py_INCREF(ret);
    తిరిగి ret;
}

    స్థిర PyObject *
కిటికీ_Attr(కిటికీ_Object *self, అక్షరం *name)
{
    ఒకవేళ (strcmp(name, "buffer") == 0)
	తిరిగి (PyObject *)బఫరు_New(self->win->w_బఫరు);
    లేకపోతే ఒకవేళ (strcmp(name, "cursor") == 0)
    {
	pos_T *pos = &self->win->w_cursor;

	తిరిగి Py_Build_విలువ("(ll)", (దీర్ఘ)(pos->lnum), (దీర్ఘ)(pos->నిలువు));
    }
    లేకపోతే ఒకవేళ (strcmp(name, "height") == 0)
	తిరిగి PyLong_FromLong((దీర్ఘ)(self->win->w_ఎత్తు));
    లేకపోతే ఒకవేళ (strcmp(name, "row") == 0)
	తిరిగి PyLong_FromLong((దీర్ఘ)(self->win->w_winrow));
    లేకపోతే ఒకవేళ (strcmp(name, "width") == 0)
	తిరిగి PyLong_FromLong((దీర్ఘ)(self->win->w_వెడల్పు));
    లేకపోతే ఒకవేళ (strcmp(name, "col") == 0)
	తిరిగి PyLong_FromLong((దీర్ఘ)(self->win->w_wincol));
    లేకపోతే ఒకవేళ (strcmp(name, "vars") == 0)
	తిరిగి NEW_DICTIONARY(self->win->w_vars);
    లేకపోతే ఒకవేళ (strcmp(name, "options") == 0)
	తిరిగి OptionsNew(SREQ_WIN, self->win, Check_కిటికీ_Cb,
			(PyObject *) self);
    లేకపోతే ఒకవేళ (strcmp(name, "number") == 0)
    {
	ఒకవేళ (Check_Tab_పుట(self->tabObject))
	    తిరిగి ఖాళీ;
	తిరిగి PyLong_FromLong((దీర్ఘ)
		get_win_సంఖ్య(self->win, get_firstwin(self->tabObject)));
    }
    లేకపోతే ఒకవేళ (strcmp(name, "tabpage") == 0)
    {
	Py_INCREF((PyObject *)self->tabObject);
	తిరిగి (PyObject *)(self->tabObject);
    }
    లేకపోతే ఒకవేళ (strcmp(name, "__members__") == 0)
	తిరిగి ObjectDir(ఖాళీ, కిటికీ_Attrs);
    లేకపోతే
	తిరిగి ఖాళీ;
}

    స్థిర పూర్ణాంకం
కిటికీ_Setattr(PyObject *self_obj, అక్షరం *name, PyObject *valObject)
{
    కిటికీ_Object *self = (కిటికీ_Object*)self_obj;
    ఒకవేళ (Check_కిటికీ(self))
	తిరిగి -1;

    ఒకవేళ (strcmp(name, "buffer") == 0)
    {
	Py_Err_సమితి_STRING(PyExc_TypeError, N_("readonly attribute: buffer"));
	తిరిగి -1;
    }
    లేకపోతే ఒకవేళ (strcmp(name, "cursor") == 0)
    {
	దీర్ఘ lnum;
	దీర్ఘ నిలువు;

	ఒకవేళ (!PyArg_Parse(valObject, "(ll)", &lnum, &నిలువు))
	    తిరిగి -1;

	ఒకవేళ (lnum <= 0 || lnum > self->win->w_బఫరు->b_ml.ml_పంక్తి_లెక్క)
	{
	    Py_Err_సమితి_VIM(N_("cursor position outside buffer"));
	    తిరిగి -1;
	}

	// Check for keyboard interrupts
	ఒకవేళ (Vim_Check_అంతరాయం())
	    తిరిగి -1;

	self->win->w_cursor.lnum = lnum;
	self->win->w_cursor.నిలువు = నిలువు;
	self->win->w_సమితి_curswant = TRUE;
	self->win->w_cursor.coladd = 0;
	// When column is out of range silently correct it.
	check_cursor_నిలువు_win(self->win);

	update_screen(UPD_VALID);
	తిరిగి 0;
    }
    లేకపోతే ఒకవేళ (strcmp(name, "height") == 0)
    {
	దీర్ఘ	ఎత్తు;
	win_T	*savewin;

	ఒకవేళ (సంఖ్య_గమ్యం_Long(valObject, &ఎత్తు, సంఖ్య_INT|సంఖ్య_UNSIGNED))
	    తిరిగి -1;

#నిర్వచితమైతే FEAT_GUI
	need_mouse_correct = TRUE;
#అయితే_ముగింపు
	savewin = curwin;
	curwin = self->win;
	curbuf = curwin->w_బఫరు;

	Vim_Try_ప్రారంభం();
	win_setheight((పూర్ణాంకం) ఎత్తు);
	curwin = savewin;
	curbuf = curwin->w_బఫరు;
	ఒకవేళ (Vim_Try_చివర())
	    తిరిగి -1;

	తిరిగి 0;
    }
    లేకపోతే ఒకవేళ (strcmp(name, "width") == 0)
    {
	దీర్ఘ	వెడల్పు;
	win_T	*savewin;

	ఒకవేళ (సంఖ్య_గమ్యం_Long(valObject, &వెడల్పు, సంఖ్య_INT|సంఖ్య_UNSIGNED))
	    తిరిగి -1;

#నిర్వచితమైతే FEAT_GUI
	need_mouse_correct = TRUE;
#అయితే_ముగింపు
	savewin = curwin;
	curwin = self->win;
	curbuf = curwin->w_బఫరు;

	Vim_Try_ప్రారంభం();
	win_setwidth((పూర్ణాంకం) వెడల్పు);
	curwin = savewin;
	curbuf = curwin->w_బఫరు;
	ఒకవేళ (Vim_Try_చివర())
	    తిరిగి -1;

	తిరిగి 0;
    }
    లేకపోతే
    {
	Py_Err_సమితి_String(PyExc_AttributeError, name);
	తిరిగి -1;
    }
}

    స్థిర PyObject *
కిటికీ_Repr(PyObject *self_obj)
{
    కిటికీ_Object *self = (కిటికీ_Object*)self_obj;
    ఒకవేళ (self->win == INVALID_కిటికీ_విలువ)
	తిరిగి PyString_FromFormat("<window object (deleted) at %p>", (శూన్య *)self);
    లేకపోతే
    {
	పూర్ణాంకం	w = get_win_సంఖ్య(self->win, firstwin);

	ఒకవేళ (w == 0)
	    తిరిగి PyString_FromFormat("<window object (unknown) at %p>",
								      (శూన్య *)self);
	లేకపోతే
	    తిరిగి PyString_FromFormat("<window %d>", w - 1);
    }
}

స్థిర నిర్మాణం Py_పద్ధతి_Def కిటికీ_Methods[] = {
    // name,	    function,			calling,	documentation
    {"__dir__",	    (PyCFunction)కిటికీ_Dir,	METH_NOARGS,	""},
    { ఖాళీ,	    ఖాళీ,			0,		ఖాళీ}
};

/*
 * Window list object
 */

DEFINE_PY_TYPE_OBJECT(WinListType);
స్థిర PySequenceMethods WinListAsSeq;

రకనిర్వచనం నిర్మాణం
{
    Py_Object_తల
    Tab_పుట_Object	*tabObject;
} WinListObject;

స్థిర WinListObject The_కిటికీ_List =
{
    Py_Object_తల_INIT_TYPE(WinListType)
    ఖాళీ
};

    స్థిర PyObject *
WinListNew(Tab_పుట_Object *tabObject)
{
    WinListObject	*self;

    self = PyObject_NEW(WinListObject, WinListTypePtr);
    self->tabObject = tabObject;
    Py_INCREF((PyObject *)tabObject);

    తిరిగి (PyObject *)(self);
}

    స్థిర శూన్య
WinListDestructor(WinListObject *self)
{
    Tab_పుట_Object	*tabObject = self->tabObject;

    ఒకవేళ (tabObject)
    {
	Py_DECREF((PyObject *)(tabObject));
    }

    DESTRUCTOR_ముగింపు(self);
}

    స్థిర PyInt
Win_List_నిడివి(WinListObject *self)
{
    win_T	*w;
    PyInt	n = 0;

    ఒకవేళ (!(w = get_firstwin(self->tabObject)))
	తిరిగి -1;

    అయితే (w != ఖాళీ)
    {
	++n;
	w = W_తదుపరి(w);
    }

    తిరిగి n;
}

    స్థిర PyObject *
Win_List_అంశం(WinListObject *self, PyInt n)
{
    win_T *w;

    ఒకవేళ (!(w = get_firstwin(self->tabObject)))
	తిరిగి ఖాళీ;

    కొరకు (; w != ఖాళీ; w = W_తదుపరి(w), --n)
	ఒకవేళ (n == 0)
	    తిరిగి కిటికీ_New(w, self->tabObject? self->tabObject->tab: curtab);

    Py_Err_సమితి_STRING(Py_Exc_సూచీ_Error, N_("no such window"));
    తిరిగి ఖాళీ;
}

/*
 * Convert a Python string into a Vim line.
 *
 * The result is in allocated memory. All internal nulls are replaced by
 * newline characters. It is an error for the string to contain newline
 * characters.
 *
 * On errors, the Python exception data is set, and NULL is returned.
 */
    స్థిర అక్షరం *
String_గమ్యం_పంక్తి(PyObject *obj)
{
    అక్షరం	*str;
    అక్షరం	*save;
    PyObject	*బైట్లు = ఖాళీ;
    Py_ssize_t	len = 0;
    PyInt	i;
    అక్షరం	*p;

    ఒకవేళ (Py_బైట్లు_Check(obj))
    {
	ఒకవేళ (Py_బైట్లు_As_String_And_కొలత(obj, &str, &len) == -1
		|| str == ఖాళీ)
	    తిరిగి ఖాళీ;
    }
    లేకపోతే ఒకవేళ (PyUnicode_Check(obj))
    {
	ఒకవేళ (!(బైట్లు = PyUnicode_AsEncodedString(obj, ENC_OPT,
							   ERRORS_ENCODE_ARG)))
	    తిరిగి ఖాళీ;

	ఒకవేళ (Py_బైట్లు_As_String_And_కొలత(బైట్లు, &str, &len) == -1
		|| str == ఖాళీ)
	{
	    Py_DECREF(బైట్లు);
	    తిరిగి ఖాళీ;
	}
    }
    లేకపోతే
    {
#ఒకవేళ PY_MAJOR_సంస్కరణ < 3
	PyErr_FORMAT_TYPE(
		N_("expected str() or unicode() instance, but got %s"),
		obj);
#లేకపోతే
	PyErr_FORMAT_TYPE(
		N_("expected bytes() or str() instance, but got %s"),
		obj);
#అయితే_ముగింపు
	తిరిగి ఖాళీ;
    }

    /*
     * Error checking: String must not contain newlines, as we
     * are replacing a single line, and we must replace it with
     * a single line.
     * A trailing newline is removed, so that append(f.readlines()) works.
     */
    p = memchr(str, '\n', len);
    ఒకవేళ (p != ఖాళీ)
    {
	ఒకవేళ (p == str + len - 1)
	    --len;
	లేకపోతే
	{
	    Py_Err_సమితి_VIM(N_("string cannot contain newlines"));
	    Py_XDECREF(బైట్లు);
	    తిరిగి ఖాళీ;
	}
    }

    /*
     * Create a copy of the string, with internal nulls replaced by
     * newline characters, as is the Vim convention.
     */
    save = alloc(len+1);
    ఒకవేళ (save == ఖాళీ)
    {
	Py_Err_No_జ్ఞాపకం();
	Py_XDECREF(బైట్లు);
	తిరిగి ఖాళీ;
    }

    కొరకు (i = 0; i < len; ++i)
    {
	ఒకవేళ (str[i] == '\0')
	    save[i] = '\n';
	లేకపోతే
	    save[i] = str[i];
    }

    save[i] = '\0';
    Py_XDECREF(బైట్లు);  // Python 2 does nothing here

    తిరిగి save;
}

/*
 * Get a line from the specified buffer. The line number is
 * in Vim format (1-based). The line is returned as a Python
 * string object.
 */
    స్థిర PyObject *
Get_బఫరు_పంక్తి(buf_T *buf, PyInt n)
{
    తిరిగి పంక్తి_గమ్యం_String((అక్షరం *)ml_get_buf(buf, (linenr_T)n, FALSE));
}


/*
 * Get a list of lines from the specified buffer. The line numbers
 * are in Vim format (1-based). The range is from lo up to, but not
 * including, hi. The list is returned as a Python list of string objects.
 */
    స్థిర PyObject *
Get_బఫరు_పంక్తి_List(buf_T *buf, PyInt lo, PyInt hi)
{
    PyInt	i;
    PyInt	n = hi - lo;
    PyObject	*list = PyList_New(n);

    ఒకవేళ (list == ఖాళీ)
	తిరిగి ఖాళీ;

    కొరకు (i = 0; i < n; ++i)
    {
	linenr_T	lnum = (linenr_T)(lo + i);
	అక్షరం		*పాఠ్యం;
	PyObject	*string;

	ఒకవేళ (lnum > buf->b_ml.ml_పంక్తి_లెక్క)
	    పాఠ్యం = "";
	లేకపోతే
	    పాఠ్యం = (అక్షరం *)ml_get_buf(buf, lnum, FALSE);
	string = పంక్తి_గమ్యం_String(పాఠ్యం);
	ఒకవేళ (string == ఖాళీ)
	{
	    Py_DECREF(list);
	    తిరిగి ఖాళీ;
	}

	Py_List_సమితి_అంశం(list, i, string);
    }

    // The ownership of the Python list is passed to the caller (ie,
    // the caller should Py_DECREF() the object when it is finished
    // with it).

    తిరిగి list;
}

/*
 * Check if deleting lines made the cursor position invalid.
 * Changed the lines from "lo" to "hi" and added "extra" lines (negative if
 * deleted).
 */
    స్థిర శూన్య
py_fix_cursor(linenr_T lo, linenr_T hi, linenr_T extra)
{
    ఒకవేళ (curwin->w_cursor.lnum >= lo)
    {
	// Adjust the cursor position if it's in/after the changed
	// lines.
	ఒకవేళ (curwin->w_cursor.lnum >= hi)
	{
	    curwin->w_cursor.lnum += extra;
	    check_cursor_నిలువు();
	}
	లేకపోతే ఒకవేళ (extra < 0)
	{
	    curwin->w_cursor.lnum = lo;
	    check_cursor();
	}
	లేకపోతే
	    check_cursor_నిలువు();
	మారింది_cline_bef_curs();
    }
    invalidate_botline();
}

/*
 * Replace a line in the specified buffer. The line number is
 * in Vim format (1-based). The replacement line is given as
 * a Python string object. The object is checked for validity
 * and correct format. Errors are returned as a value of FAIL.
 * The return value is OK on success.
 * If OK is returned and len_change is not NULL, *len_change
 * is set to the change in the buffer length.
 */
    స్థిర పూర్ణాంకం
సమితి_బఫరు_పంక్తి_2(buf_T *buf, PyInt n, PyObject *పంక్తి, PyInt *len_change)
{
    bufref_T	save_curbuf = {ఖాళీ, 0, 0};
    switchwin_T	switchwin;

    // First of all, we check the type of the supplied Python object.
    // There are three cases:
    //	  1. NULL, or None - this is a deletion.
    //	  2. A string	   - this is a replacement.
    //	  3. Anything else - this is an error.
    ఒకవేళ (పంక్తి == Py_None || పంక్తి == ఖాళీ)
    {
	PyErr_Clear();
	switchwin.sw_curwin = ఖాళీ;
	switch_గమ్యం_win_for_buf(buf, &switchwin, &save_curbuf);

	Vim_Try_ప్రారంభం();

	ఒకవేళ (u_savedel((linenr_T)n, 1L) == FAIL)
	    RAISE_UNDO_FAIL;
	లేకపోతే ఒకవేళ (ml_delete((linenr_T)n) == FAIL)
	    RAISE_DELETE_పంక్తి_FAIL;
	లేకపోతే
	{
	    ఒకవేళ (buf == curbuf && (switchwin.sw_curwin != ఖాళీ
					   || save_curbuf.br_buf == ఖాళీ))
		// Using an existing window for the buffer, adjust the cursor
		// position.
		py_fix_cursor((linenr_T)n, (linenr_T)n + 1, (linenr_T)-1);
	    ఒకవేళ (save_curbuf.br_buf == ఖాళీ)
		// Only adjust marks if we managed to switch to a window that
		// holds the buffer, otherwise line numbers will be invalid.
		deleted_పంక్తులు_mark((linenr_T)n, 1L);
	}

	restore_win_for_buf(&switchwin, &save_curbuf);

	ఒకవేళ (Vim_Try_చివర())
	    తిరిగి FAIL;

	ఒకవేళ (len_change)
	    *len_change = -1;

	తిరిగి OK;
    }
    లేకపోతే ఒకవేళ (Py_బైట్లు_Check(పంక్తి) || PyUnicode_Check(పంక్తి))
    {
	అక్షరం	*save = String_గమ్యం_పంక్తి(పంక్తి);

	ఒకవేళ (save == ఖాళీ)
	    తిరిగి FAIL;

	Vim_Try_ప్రారంభం();

	// We do not need to free "save" if ml_replace() consumes it.
	PyErr_Clear();
	switch_గమ్యం_win_for_buf(buf, &switchwin, &save_curbuf);

	ఒకవేళ (u_savesub((linenr_T)n) == FAIL)
	{
	    RAISE_UNDO_FAIL;
	    vim_free(save);
	}
	లేకపోతే ఒకవేళ (ml_replace((linenr_T)n, (char_u *)save, FALSE) == FAIL)
	{
	    RAISE_REPLACE_పంక్తి_FAIL;
	    vim_free(save);
	}
	లేకపోతే
	    మారింది_బైట్లు((linenr_T)n, 0);

	restore_win_for_buf(&switchwin, &save_curbuf);

	// Check that the cursor is not beyond the end of the line now.
	ఒకవేళ (buf == curbuf)
	    check_cursor_నిలువు();

	ఒకవేళ (Vim_Try_చివర())
	    తిరిగి FAIL;

	ఒకవేళ (len_change)
	    *len_change = 0;

	తిరిగి OK;
    }
    లేకపోతే
    {
	Py_Err_Bad_వాదం();
	తిరిగి FAIL;
    }
}

/*
 * Replace a range of lines in the specified buffer. The line numbers are in
 * Vim format (1-based). The range is from lo up to, but not including, hi.
 * The replacement lines are given as a Python list of string objects. The
 * list is checked for validity and correct format. Errors are returned as a
 * value of FAIL.  The return value is OK on success.
 * If OK is returned and len_change is not NULL, *len_change
 * is set to the change in the buffer length.
 */
    స్థిర పూర్ణాంకం
సమితి_బఫరు_పంక్తి_List(
	buf_T *buf,
	PyInt lo,
	PyInt hi,
	PyObject *list,
	PyInt *len_change)
{
    bufref_T	save_curbuf = {ఖాళీ, 0, 0};
    switchwin_T	switchwin;

    // First of all, we check the type of the supplied Python object.
    // There are three cases:
    //	  1. NULL, or None - this is a deletion.
    //	  2. A list	   - this is a replacement.
    //	  3. Anything else - this is an error.
    ఒకవేళ (list == Py_None || list == ఖాళీ)
    {
	PyInt	i;
	PyInt	n = (పూర్ణాంకం)(hi - lo);

	PyErr_Clear();
	Vim_Try_ప్రారంభం();
	switchwin.sw_curwin = ఖాళీ;
	switch_గమ్యం_win_for_buf(buf, &switchwin, &save_curbuf);

	ఒకవేళ (u_savedel((linenr_T)lo, (దీర్ఘ)n) == FAIL)
	    RAISE_UNDO_FAIL;
	లేకపోతే
	{
	    కొరకు (i = 0; i < n; ++i)
	    {
		ఒకవేళ (ml_delete((linenr_T)lo) == FAIL)
		{
		    RAISE_DELETE_పంక్తి_FAIL;
		    ఆపు;
		}
	    }
	    ఒకవేళ (buf == curbuf && (switchwin.sw_curwin != ఖాళీ
					       || save_curbuf.br_buf == ఖాళీ))
		// Using an existing window for the buffer, adjust the cursor
		// position.
		py_fix_cursor((linenr_T)lo, (linenr_T)hi, (linenr_T)-n);
	    ఒకవేళ (save_curbuf.br_buf == ఖాళీ)
		// Only adjust marks if we managed to switch to a window that
		// holds the buffer, otherwise line numbers will be invalid.
		deleted_పంక్తులు_mark((linenr_T)lo, (దీర్ఘ)i);
	}

	restore_win_for_buf(&switchwin, &save_curbuf);

	ఒకవేళ (Vim_Try_చివర())
	    తిరిగి FAIL;

	ఒకవేళ (len_change)
	    *len_change = -n;

	తిరిగి OK;
    }
    లేకపోతే ఒకవేళ (PyList_Check(list))
    {
	PyInt	i;
	PyInt	new_len = Py_List_కొలత(list);
	PyInt	old_len = hi - lo;
	PyInt	extra = 0;	// lines added to text, can be negative
	అక్షరం	**array;

	ఒకవేళ (new_len == 0)	// avoid allocating zero bytes
	    array = ఖాళీ;
	లేకపోతే
	{
	    array = PyMem_New(అక్షరం *, new_len);
	    ఒకవేళ (array == ఖాళీ)
	    {
		Py_Err_No_జ్ఞాపకం();
		తిరిగి FAIL;
	    }
	}

	కొరకు (i = 0; i < new_len; ++i)
	{
	    PyObject *పంక్తి;

	    ఒకవేళ (!(పంక్తి = Py_List_Get_అంశం(list, i)) ||
		!(array[i] = String_గమ్యం_పంక్తి(పంక్తి)))
	    {
		అయితే (i)
		    vim_free(array[--i]);
		PyMem_Free(array);
		తిరిగి FAIL;
	    }
	}

	Vim_Try_ప్రారంభం();
	PyErr_Clear();

	// START of region without "return".  Must call restore_buffer()!
	switchwin.sw_curwin = ఖాళీ;
	switch_గమ్యం_win_for_buf(buf, &switchwin, &save_curbuf);

	ఒకవేళ (u_save((linenr_T)(lo-1), (linenr_T)hi) == FAIL)
	    RAISE_UNDO_FAIL;

	// If the size of the range is reducing (ie, new_len < old_len) we
	// need to delete some old_len. We do this at the start, by
	// repeatedly deleting line "lo".
	ఒకవేళ (!PyErr_Occurred())
	{
	    కొరకు (i = 0; i < old_len - new_len; ++i)
		ఒకవేళ (ml_delete((linenr_T)lo) == FAIL)
		{
		    RAISE_DELETE_పంక్తి_FAIL;
		    ఆపు;
		}
	    extra -= i;
	}

	// For as long as possible, replace the existing old_len with the
	// new old_len. This is a more efficient operation, as it requires
	// less memory allocation and freeing.
	ఒకవేళ (!PyErr_Occurred())
	{
	    కొరకు (i = 0; i < old_len && i < new_len; ++i)
		ఒకవేళ (ml_replace((linenr_T)(lo+i), (char_u *)array[i], FALSE)
								      == FAIL)
		{
		    RAISE_REPLACE_పంక్తి_FAIL;
		    ఆపు;
		}
	}
	లేకపోతే
	    i = 0;

	// Now we may need to insert the remaining new old_len. If we do, we
	// must free the strings as we finish with them (we can't pass the
	// responsibility to Vim in this case).
	ఒకవేళ (!PyErr_Occurred())
	{
	    అయితే (i < new_len)
	    {
		ఒకవేళ (ml_append((linenr_T)(lo + i - 1),
					(char_u *)array[i], 0, FALSE) == FAIL)
		{
		    RAISE_చొప్పించు_పంక్తి_FAIL;
		    ఆపు;
		}
		vim_free(array[i]);
		++i;
		++extra;
	    }
	}

	// Free any left-over old_len, as a result of an error
	అయితే (i < new_len)
	{
	    vim_free(array[i]);
	    ++i;
	}

	// Free the array of old_len. All of its contents have now
	// been dealt with (either freed, or the responsibility passed
	// to vim.
	PyMem_Free(array);

	// Adjust marks. Invalidate any which lie in the
	// changed range, and move any in the remainder of the buffer.
	// Only adjust marks if we managed to switch to a window that holds
	// the buffer, otherwise line numbers will be invalid.
	ఒకవేళ (save_curbuf.br_buf == ఖాళీ)
	{
	    mark_adjust((linenr_T)lo, (linenr_T)(hi - 1),
						  (దీర్ఘ)MAXLNUM, (దీర్ఘ)extra);
	    మారింది_పంక్తులు((linenr_T)lo, 0, (linenr_T)hi, (దీర్ఘ)extra);
	}

	ఒకవేళ (buf == curbuf && (switchwin.sw_curwin != ఖాళీ
					   || save_curbuf.br_buf == ఖాళీ))
	    // Using an existing window for the buffer, adjust the cursor
	    // position.
	    py_fix_cursor((linenr_T)lo, (linenr_T)hi, (linenr_T)extra);

	// END of region without "return".
	restore_win_for_buf(&switchwin, &save_curbuf);

	ఒకవేళ (Vim_Try_చివర())
	    తిరిగి FAIL;

	ఒకవేళ (len_change)
	    *len_change = new_len - old_len;

	తిరిగి OK;
    }
    లేకపోతే
    {
	Py_Err_Bad_వాదం();
	తిరిగి FAIL;
    }
}

/*
 * Insert a number of lines into the specified buffer after the specified line.
 * The line number is in Vim format (1-based). The lines to be inserted are
 * given as a Python list of string objects or as a single string. The lines
 * to be added are checked for validity and correct format. Errors are
 * returned as a value of FAIL.  The return value is OK on success.
 * If OK is returned and len_change is not NULL, *len_change
 * is set to the change in the buffer length.
 */
    స్థిర పూర్ణాంకం
చొప్పించు_బఫరు_పంక్తులు(buf_T *buf, PyInt n, PyObject *పంక్తులు, PyInt *len_change)
{
    bufref_T	save_curbuf = {ఖాళీ, 0, 0};
    switchwin_T	switchwin;

    // First of all, we check the type of the supplied Python object.
    // It must be a string or a list, or the call is in error.
    ఒకవేళ (Py_బైట్లు_Check(పంక్తులు) || PyUnicode_Check(పంక్తులు))
    {
	అక్షరం		*str = String_గమ్యం_పంక్తి(పంక్తులు);

	ఒకవేళ (str == ఖాళీ)
	    తిరిగి FAIL;

	PyErr_Clear();
	Vim_Try_ప్రారంభం();
	switch_గమ్యం_win_for_buf(buf, &switchwin, &save_curbuf);

	ఒకవేళ (u_save((linenr_T)n, (linenr_T)(n + 1)) == FAIL)
	    RAISE_UNDO_FAIL;
	లేకపోతే ఒకవేళ (ml_append((linenr_T)n, (char_u *)str, 0, FALSE) == FAIL)
	    RAISE_చొప్పించు_పంక్తి_FAIL;
	లేకపోతే ఒకవేళ (save_curbuf.br_buf == ఖాళీ)
	    // Only adjust marks if we managed to switch to a window that
	    // holds the buffer, otherwise line numbers will be invalid.
	    appended_పంక్తులు_mark((linenr_T)n, 1L);

	vim_free(str);
	restore_win_for_buf(&switchwin, &save_curbuf);
	update_screen(UPD_VALID);

	ఒకవేళ (Vim_Try_చివర())
	    తిరిగి FAIL;

	ఒకవేళ (len_change)
	    *len_change = 1;

	తిరిగి OK;
    }
    లేకపోతే ఒకవేళ (PyList_Check(పంక్తులు))
    {
	PyInt	i;
	PyInt	కొలత = Py_List_కొలత(పంక్తులు);
	అక్షరం	**array;

	array = PyMem_New(అక్షరం *, కొలత);
	ఒకవేళ (array == ఖాళీ)
	{
	    Py_Err_No_జ్ఞాపకం();
	    తిరిగి FAIL;
	}

	కొరకు (i = 0; i < కొలత; ++i)
	{
	    PyObject *పంక్తి;

	    ఒకవేళ (!(పంక్తి = Py_List_Get_అంశం(పంక్తులు, i)) ||
		!(array[i] = String_గమ్యం_పంక్తి(పంక్తి)))
	    {
		అయితే (i)
		    vim_free(array[--i]);
		PyMem_Free(array);
		తిరిగి FAIL;
	    }
	}

	PyErr_Clear();
	Vim_Try_ప్రారంభం();
	switch_గమ్యం_win_for_buf(buf, &switchwin, &save_curbuf);

	ఒకవేళ (u_save((linenr_T)n, (linenr_T)(n + 1)) == FAIL)
	    RAISE_UNDO_FAIL;
	లేకపోతే
	{
	    కొరకు (i = 0; i < కొలత; ++i)
	    {
		ఒకవేళ (ml_append((linenr_T)(n + i),
					(char_u *)array[i], 0, FALSE) == FAIL)
		{
		    RAISE_చొప్పించు_పంక్తి_FAIL;

		    // Free the rest of the lines
		    అయితే (i < కొలత)
			vim_free(array[i++]);

		    ఆపు;
		}
		vim_free(array[i]);
	    }
	    ఒకవేళ (i > 0 && save_curbuf.br_buf == ఖాళీ)
		// Only adjust marks if we managed to switch to a window that
		// holds the buffer, otherwise line numbers will be invalid.
		appended_పంక్తులు_mark((linenr_T)n, (దీర్ఘ)i);
	}

	// Free the array of lines. All of its contents have now
	// been freed.
	PyMem_Free(array);
	restore_win_for_buf(&switchwin, &save_curbuf);

	update_screen(UPD_VALID);

	ఒకవేళ (Vim_Try_చివర())
	    తిరిగి FAIL;

	ఒకవేళ (len_change)
	    *len_change = కొలత;

	తిరిగి OK;
    }
    లేకపోతే
    {
	Py_Err_Bad_వాదం();
	తిరిగి FAIL;
    }
}

/*
 * Common routines for buffers and line ranges
 * -------------------------------------------
 */

రకనిర్వచనం నిర్మాణం
{
    Py_Object_తల
    buf_T *buf;
} బఫరు_Object;

    స్థిర పూర్ణాంకం
Check_బఫరు(బఫరు_Object *self)
{
    ఒకవేళ (self->buf == INVALID_బఫరు_విలువ)
    {
	Py_Err_సమితి_VIM(N_("attempt to refer to deleted buffer"));
	తిరిగి -1;
    }

    తిరిగి 0;
}

    స్థిర పూర్ణాంకం
Check_బఫరు_Cb(శూన్య *self)
{
    తిరిగి Check_బఫరు((బఫరు_Object*)self);
}

    స్థిర PyObject *
RBItem(బఫరు_Object *self, PyInt n, PyInt ప్రారంభం, PyInt చివర)
{
    ఒకవేళ (Check_బఫరు(self))
	తిరిగి ఖాళీ;

    ఒకవేళ (చివర == -1)
	చివర = self->buf->b_ml.ml_పంక్తి_లెక్క;

    ఒకవేళ (n < 0)
	n += చివర - ప్రారంభం + 1;

    ఒకవేళ (n < 0 || n > చివర - ప్రారంభం)
    {
	Py_Err_సమితి_STRING(Py_Exc_సూచీ_Error, N_("line number out of range"));
	తిరిగి ఖాళీ;
    }

    తిరిగి Get_బఫరు_పంక్తి(self->buf, n+ప్రారంభం);
}

    స్థిర PyObject *
RBSlice(బఫరు_Object *self, PyInt lo, PyInt hi, PyInt ప్రారంభం, PyInt చివర)
{
    PyInt కొలత;

    ఒకవేళ (Check_బఫరు(self))
	తిరిగి ఖాళీ;

    ఒకవేళ (చివర == -1)
	చివర = self->buf->b_ml.ml_పంక్తి_లెక్క;

    కొలత = చివర - ప్రారంభం + 1;

    ఒకవేళ (lo < 0)
	lo = 0;
    లేకపోతే ఒకవేళ (lo > కొలత)
	lo = కొలత;
    ఒకవేళ (hi < 0)
	hi = 0;
    ఒకవేళ (hi < lo)
	hi = lo;
    లేకపోతే ఒకవేళ (hi > కొలత)
	hi = కొలత;

    తిరిగి Get_బఫరు_పంక్తి_List(self->buf, lo+ప్రారంభం, hi+ప్రారంభం);
}

    స్థిర PyInt
RBAs_అంశం(
	బఫరు_Object *self,
	PyInt n,
	PyObject *valObject,
	PyInt ప్రారంభం,
	PyInt చివర,
	PyInt *new_చివర)
{
    PyInt len_change;

    ఒకవేళ (Check_బఫరు(self))
	తిరిగి -1;

    ఒకవేళ (చివర == -1)
	చివర = self->buf->b_ml.ml_పంక్తి_లెక్క;

    ఒకవేళ (n < 0)
	n += చివర - ప్రారంభం + 1;

    ఒకవేళ (n < 0 || n > చివర - ప్రారంభం)
    {
	Py_Err_సమితి_STRING(Py_Exc_సూచీ_Error, N_("line number out of range"));
	తిరిగి -1;
    }

    ఒకవేళ (సమితి_బఫరు_పంక్తి_2(self->buf, n+ప్రారంభం, valObject, &len_change) == FAIL)
	తిరిగి -1;

    ఒకవేళ (new_చివర)
	*new_చివర = చివర + len_change;

    తిరిగి 0;
}

    స్థిర PyInt
RBAs_ముక్క(
	బఫరు_Object *self,
	PyInt lo,
	PyInt hi,
	PyObject *valObject,
	PyInt ప్రారంభం,
	PyInt చివర,
	PyInt *new_చివర)
{
    PyInt కొలత;
    PyInt len_change;

    // Self must be a valid buffer
    ఒకవేళ (Check_బఫరు(self))
	తిరిగి -1;

    ఒకవేళ (చివర == -1)
	చివర = self->buf->b_ml.ml_పంక్తి_లెక్క;

    // Sort out the slice range
    కొలత = చివర - ప్రారంభం + 1;

    ఒకవేళ (lo < 0)
	lo = 0;
    లేకపోతే ఒకవేళ (lo > కొలత)
	lo = కొలత;
    ఒకవేళ (hi < 0)
	hi = 0;
    ఒకవేళ (hi < lo)
	hi = lo;
    లేకపోతే ఒకవేళ (hi > కొలత)
	hi = కొలత;

    ఒకవేళ (సమితి_బఫరు_పంక్తి_List(self->buf, lo + ప్రారంభం, hi + ప్రారంభం,
						valObject, &len_change) == FAIL)
	తిరిగి -1;

    ఒకవేళ (new_చివర)
	*new_చివర = చివర + len_change;

    తిరిగి 0;
}


    స్థిర PyObject *
RBAppend(
	బఫరు_Object *self,
	PyObject *వాదనలు,
	PyInt ప్రారంభం,
	PyInt చివర,
	PyInt *new_చివర)
{
    PyObject *పంక్తులు;
    PyInt len_change;
    PyInt max;
    PyInt n;

    ఒకవేళ (Check_బఫరు(self))
	తిరిగి ఖాళీ;

    ఒకవేళ (చివర == -1)
	చివర = self->buf->b_ml.ml_పంక్తి_లెక్క;

    max = n = చివర - ప్రారంభం + 1;

    ఒకవేళ (!PyArg_ParseTuple(వాదనలు, "O|n", &పంక్తులు, &n))
	తిరిగి ఖాళీ;

    ఒకవేళ (n < 0 || n > max)
    {
	Py_Err_సమితి_STRING(Py_Exc_సూచీ_Error, N_("line number out of range"));
	తిరిగి ఖాళీ;
    }

    ఒకవేళ (చొప్పించు_బఫరు_పంక్తులు(self->buf, n + ప్రారంభం - 1, పంక్తులు, &len_change) == FAIL)
	తిరిగి ఖాళీ;

    ఒకవేళ (new_చివర)
	*new_చివర = చివర + len_change;

    Py_INCREF(Py_None);
    తిరిగి Py_None;
}

// Range object

DEFINE_PY_TYPE_OBJECT(RangeType);
స్థిర PySequenceMethods RangeAsSeq;
స్థిర PyMappingMethods RangeAsMapping;

రకనిర్వచనం నిర్మాణం
{
    Py_Object_తల
    బఫరు_Object *buf;
    PyInt ప్రారంభం;
    PyInt చివర;
} RangeObject;

    స్థిర PyObject *
RangeNew(buf_T *buf, PyInt ప్రారంభం, PyInt చివర)
{
    బఫరు_Object *bufr;
    RangeObject *self;
    self = PyObject_GC_New(RangeObject, RangeTypePtr);
    ఒకవేళ (self == ఖాళీ)
	తిరిగి ఖాళీ;

    bufr = (బఫరు_Object *)బఫరు_New(buf);
    ఒకవేళ (bufr == ఖాళీ)
    {
	Py_DECREF(self);
	తిరిగి ఖాళీ;
    }
    Py_INCREF((PyObject *)bufr);

    self->buf = bufr;
    self->ప్రారంభం = ప్రారంభం;
    self->చివర = చివర;

    తిరిగి (PyObject *)(self);
}

    స్థిర శూన్య
RangeDestructor(PyObject *self_obj)
{
    RangeObject *self = (RangeObject*)self_obj;
    PyObject_GC_UnTrack((శూన్య *)(self));
    Py_XDECREF(((PyObject *)(self->buf)));
    PyObject_GC_Del((శూన్య *)(self));
}

    స్థిర పూర్ణాంకం
RangeTraverse(PyObject *self_obj, visitproc సందర్శించు, శూన్య *arg)
{
    RangeObject *self = (RangeObject*)self_obj;
    Py_సందర్శించు(((PyObject *)(self->buf)));
    తిరిగి 0;
}

    స్థిర పూర్ణాంకం
RangeClear(PyObject *self_obj)
{
    RangeObject *self = (RangeObject*)self_obj;
    Py_CLEAR(self->buf);
    తిరిగి 0;
}

    స్థిర PyInt
Range_నిడివి(RangeObject *self)
{
    // HOW DO WE SIGNAL AN ERROR FROM THIS FUNCTION?
    ఒకవేళ (Check_బఫరు(self->buf))
	తిరిగి -1; // ???

    తిరిగి (self->చివర - self->ప్రారంభం + 1);
}

    స్థిర PyObject *
Range_అంశం(RangeObject *self, PyInt n)
{
    తిరిగి RBItem(self->buf, n, self->ప్రారంభం, self->చివర);
}

    స్థిర PyObject *
Range_ముక్క(RangeObject *self, PyInt lo, PyInt hi)
{
    తిరిగి RBSlice(self->buf, lo, hi, self->ప్రారంభం, self->చివర);
}

స్థిర అక్షరం *RangeAttrs[] = {
    "start", "end",
    ఖాళీ
};

    స్థిర PyObject *
RangeDir(PyObject *self, PyObject *వాదనలు వాడని)
{
    తిరిగి ObjectDir(self, RangeAttrs);
}

    స్థిర PyObject *
RangeAppend(RangeObject *self, PyObject *వాదనలు)
{
    తిరిగి RBAppend(self->buf, వాదనలు, self->ప్రారంభం, self->చివర, &self->చివర);
}

    స్థిర PyObject *
RangeRepr(PyObject *self_obj)
{
    RangeObject *self = (RangeObject*)self_obj;
    ఒకవేళ (self->buf->buf == INVALID_బఫరు_విలువ)
	తిరిగి PyString_FromFormat("<range object (for deleted buffer) at %p>",
				    (శూన్య *)self);
    లేకపోతే
    {
	అక్షరం *name = (అక్షరం *)self->buf->buf->b_fname;

	ఒకవేళ (name == ఖాళీ)
	    name = "";

	తిరిగి PyString_FromFormat("<range %s (%d:%d)>",
				    name, (పూర్ణాంకం)self->ప్రారంభం, (పూర్ణాంకం)self->చివర);
    }
}

స్థిర నిర్మాణం Py_పద్ధతి_Def RangeMethods[] = {
    // name,	function,			calling,	documentation
    {"append",	(PyCFunction)RangeAppend,	METH_VARARGS,	"Append data to the Vim range" },
    {"__dir__",	(PyCFunction)RangeDir,		METH_NOARGS,	""},
    { ఖాళీ,	ఖాళీ,				0,		ఖాళీ}
};

DEFINE_PY_TYPE_OBJECT(బఫరు_Type);
స్థిర PySequenceMethods బఫరు_As_Seq;
స్థిర PyMappingMethods బఫరు_As_Mapping;

    స్థిర PyObject *
బఫరు_New(buf_T *buf)
{
    /*
     * We need to handle deletion of buffers underneath us.
     * If we add a "b_python*_ref" field to the buf_T structure,
     * then we can get at it in buf_freeall() in vim. We then
     * need to create only ONE Python object per buffer - if
     * we try to create a second, just INCREF the existing one
     * and return it. The (single) Python object referring to
     * the buffer is stored in "b_python*_ref".
     * Question: what to do on a buf_freeall(). We'll probably
     * have to either delete the Python object (DECREF it to
     * zero - a bad idea, as it leaves dangling refs!) or
     * set the buf_T * value to an invalid value (-1?), which
     * means we need checks in all access functions... Bah.
     *
     * Python2 and Python3 get different fields and different objects:
     * b_python_ref and b_python3_ref fields respectively.
     */

    బఫరు_Object *self;

    ఒకవేళ (BUF_PYTHON_REF(buf) != ఖాళీ)
    {
	self = BUF_PYTHON_REF(buf);
	Py_INCREF((PyObject *)self);
    }
    లేకపోతే
    {
	self = PyObject_NEW(బఫరు_Object, బఫరు_Type_Ptr);
	ఒకవేళ (self == ఖాళీ)
	    తిరిగి ఖాళీ;
	self->buf = buf;
	BUF_PYTHON_REF(buf) = self;
    }

    తిరిగి (PyObject *)(self);
}

    స్థిర శూన్య
బఫరు_Destructor(PyObject *self_obj)
{
    బఫరు_Object *self = (బఫరు_Object*)self_obj;
    ఒకవేళ (self->buf && self->buf != INVALID_బఫరు_విలువ)
	BUF_PYTHON_REF(self->buf) = ఖాళీ;

    DESTRUCTOR_ముగింపు(self);
}

    స్థిర PyInt
బఫరు_నిడివి(బఫరు_Object *self)
{
    // HOW DO WE SIGNAL AN ERROR FROM THIS FUNCTION?
    ఒకవేళ (Check_బఫరు(self))
	తిరిగి -1; // ???

    తిరిగి (PyInt)(self->buf->b_ml.ml_పంక్తి_లెక్క);
}

    స్థిర PyObject *
బఫరు_అంశం(బఫరు_Object *self, PyInt n)
{
    తిరిగి RBItem(self, n, 1, -1);
}

    స్థిర PyObject *
బఫరు_ముక్క(బఫరు_Object *self, PyInt lo, PyInt hi)
{
    తిరిగి RBSlice(self, lo, hi, 1, -1);
}

స్థిర అక్షరం *బఫరు_Attrs[] = {
    "name", "number", "vars", "options", "valid",
    ఖాళీ
};

    స్థిర PyObject *
బఫరు_Dir(PyObject *self, PyObject *వాదనలు వాడని)
{
    తిరిగి ObjectDir(self, బఫరు_Attrs);
}

    స్థిర PyObject *
బఫరు_Attr_Valid(బఫరు_Object *self, అక్షరం *name)
{
    PyObject	*ret;

    ఒకవేళ (strcmp(name, "valid") != 0)
	తిరిగి ఖాళీ;

    ret = ((self->buf == INVALID_బఫరు_విలువ) ? Py_False : Py_True);
    Py_INCREF(ret);
    తిరిగి ret;
}

    స్థిర PyObject *
బఫరు_Attr(బఫరు_Object *self, అక్షరం *name)
{
    ఒకవేళ (strcmp(name, "name") == 0)
	తిరిగి PyString_FromString((self->buf->b_ffname == ఖాళీ
				    ? "" : (అక్షరం *)self->buf->b_ffname));
    లేకపోతే ఒకవేళ (strcmp(name, "number") == 0)
	తిరిగి Py_Build_విలువ(Py_ssize_t_fmt, self->buf->b_fnum);
    లేకపోతే ఒకవేళ (strcmp(name, "vars") == 0)
	తిరిగి NEW_DICTIONARY(self->buf->b_vars);
    లేకపోతే ఒకవేళ (strcmp(name, "options") == 0)
	తిరిగి OptionsNew(SREQ_BUF, self->buf, Check_బఫరు_Cb,
			(PyObject *) self);
    లేకపోతే ఒకవేళ (strcmp(name, "__members__") == 0)
	తిరిగి ObjectDir(ఖాళీ, బఫరు_Attrs);
    లేకపోతే
	తిరిగి ఖాళీ;
}

    స్థిర పూర్ణాంకం
బఫరు_Setattr(PyObject *self_obj, అక్షరం *name, PyObject *valObject)
{
    బఫరు_Object *self = (బఫరు_Object*)self_obj;
    ఒకవేళ (Check_బఫరు(self))
	తిరిగి -1;

    ఒకవేళ (strcmp(name, "name") == 0)
    {
	char_u		*val;
	aco_save_T	aco;
	పూర్ణాంకం		ren_ret = OK;
	PyObject	*todecref;

	ఒకవేళ (!(val = String_గమ్యం_Chars(valObject, &todecref)))
	    తిరిగి -1;

	Vim_Try_ప్రారంభం();
	// Using aucmd_*: autocommands will be executed by rename_buffer
	aucmd_prepbuf(&aco, self->buf);
	ఒకవేళ (curbuf == self->buf)
	{
	    ren_ret = rename_బఫరు(val);
	    aucmd_restbuf(&aco);
	}
	Py_XDECREF(todecref);
	ఒకవేళ (Vim_Try_చివర())
	    తిరిగి -1;

	ఒకవేళ (ren_ret == FAIL)
	{
	    Py_Err_సమితి_VIM(N_("failed to rename buffer"));
	    తిరిగి -1;
	}
	తిరిగి 0;
    }
    లేకపోతే
    {
	Py_Err_సమితి_String(PyExc_AttributeError, name);
	తిరిగి -1;
    }
}

    స్థిర PyObject *
బఫరు_Append(బఫరు_Object *self, PyObject *వాదనలు)
{
    తిరిగి RBAppend(self, వాదనలు, 1, -1, ఖాళీ);
}

    స్థిర PyObject *
బఫరు_Mark(బఫరు_Object *self, PyObject *pmarkObject)
{
    pos_T	*posp;
    char_u	*pmark;
    char_u	mark;
    bufref_T	savebuf;
    PyObject	*todecref;

    ఒకవేళ (Check_బఫరు(self))
	తిరిగి ఖాళీ;

    ఒకవేళ (!(pmark = String_గమ్యం_Chars(pmarkObject, &todecref)))
	తిరిగి ఖాళీ;

    ఒకవేళ (pmark[0] == '\0' || pmark[1] != '\0')
    {
	Py_Err_సమితి_STRING(Py_Exc_విలువ_Error,
		N_("mark name must be a single character"));
	Py_XDECREF(todecref);
	తిరిగి ఖాళీ;
    }

    mark = *pmark;

    Py_XDECREF(todecref);

    Vim_Try_ప్రారంభం();
    switch_బఫరు(&savebuf, self->buf);
    posp = getmark(mark, FALSE);
    restore_బఫరు(&savebuf);
    ఒకవేళ (Vim_Try_చివర())
	తిరిగి ఖాళీ;

    ఒకవేళ (posp == ఖాళీ)
    {
	Py_Err_సమితి_VIM(N_("invalid mark name"));
	తిరిగి ఖాళీ;
    }

    ఒకవేళ (posp->lnum <= 0)
    {
	// Or raise an error?
	Py_INCREF(Py_None);
	తిరిగి Py_None;
    }

    తిరిగి Py_Build_విలువ("(ll)", (దీర్ఘ)(posp->lnum), (దీర్ఘ)(posp->నిలువు));
}

    స్థిర PyObject *
బఫరు_Range(బఫరు_Object *self, PyObject *వాదనలు)
{
    PyInt ప్రారంభం;
    PyInt చివర;

    ఒకవేళ (Check_బఫరు(self))
	తిరిగి ఖాళీ;

    ఒకవేళ (!PyArg_ParseTuple(వాదనలు, "nn", &ప్రారంభం, &చివర))
	తిరిగి ఖాళీ;

    తిరిగి RangeNew(self->buf, ప్రారంభం, చివర);
}

    స్థిర PyObject *
బఫరు_Repr(PyObject *self_obj)
{
    బఫరు_Object *self = (బఫరు_Object*)self_obj;
    ఒకవేళ (self->buf == INVALID_బఫరు_విలువ)
	తిరిగి PyString_FromFormat("<buffer object (deleted) at %p>", (శూన్య *)self);
    లేకపోతే
    {
	అక్షరం	*name = (అక్షరం *)self->buf->b_fname;

	ఒకవేళ (name == ఖాళీ)
	    name = "";

	తిరిగి PyString_FromFormat("<buffer %s>", name);
    }
}

స్థిర నిర్మాణం Py_పద్ధతి_Def బఫరు_Methods[] = {
    // name,	    function,			calling,	documentation
    {"append",	    (PyCFunction)బఫరు_Append,	METH_VARARGS,	"Append data to Vim buffer" },
    {"mark",	    (PyCFunction)బఫరు_Mark,	METH_O,		"Return (row,col) representing position of named mark" },
    {"range",	    (PyCFunction)బఫరు_Range,	METH_VARARGS,	"Return a range object which represents the part of the given buffer between line numbers s and e" },
    {"__dir__",	    (PyCFunction)బఫరు_Dir,	METH_NOARGS,	""},
    { ఖాళీ,	    ఖాళీ,			0,		ఖాళీ}
};

/*
 * Buffer list object - Implementation
 */

DEFINE_PY_TYPE_OBJECT(BufMapType);

రకనిర్వచనం నిర్మాణం
{
    Py_Object_తల
} BufMapObject;

స్థిర BufMapObject The_బఫరు_Map =
{
    Py_Object_తల_INIT_TYPE(BufMapType)
};

    స్థిర PyInt
Buf_Map_నిడివి(PyObject *self వాడని)
{
    buf_T	*b = firstbuf;
    PyInt	n = 0;

    అయితే (b)
    {
	++n;
	b = b->b_తదుపరి;
    }

    తిరిగి n;
}

    స్థిర PyObject *
Buf_Map_అంశం(PyObject *self వాడని, PyObject *కీలకం_Object)
{
    buf_T	*b;
    దీర్ఘ	bnr;

    ఒకవేళ (సంఖ్య_గమ్యం_Long(కీలకం_Object, &bnr, సంఖ్య_INT|సంఖ్య_NATURAL))
	తిరిగి ఖాళీ;

    b = buflist_findnr((పూర్ణాంకం) bnr);

    ఒకవేళ (b)
	తిరిగి బఫరు_New(b);
    లేకపోతే
    {
	Py_Err_సమితి_Object(Py_Exc_కీలకం_Error, కీలకం_Object);
	తిరిగి ఖాళీ;
    }
}

    స్థిర శూన్య
BufMapIterDestruct(శూన్య* arg)
{
    PyObject *బఫరు = (PyObject*)arg;
    // Iteration was stopped before all buffers were processed
    ఒకవేళ (బఫరు)
    {
	Py_DECREF(బఫరు);
    }
}

    స్థిర పూర్ణాంకం
BufMapIterTraverse(శూన్య *iter, visitproc సందర్శించు, శూన్య *arg)
{
    PyObject *బఫరు = (PyObject*)iter;
    ఒకవేళ (బఫరు)
	Py_సందర్శించు(బఫరు);
    తిరిగి 0;
}

    స్థిర పూర్ణాంకం
BufMapIterClear(శూన్య **iter)
{
    PyObject **బఫరు = (PyObject**)iter;
    ఒకవేళ (*బఫరు)
	Py_CLEAR(*బఫరు);
    తిరిగి 0;
}

    స్థిర PyObject *
Buf_Map_Iter_తదుపరి(శూన్య **arg)
{
    PyObject	*తదుపరి;
    PyObject	*ret;
    PyObject	**బఫరు = (PyObject**)arg;

    ఒకవేళ (!*బఫరు)
	తిరిగి ఖాళీ;

    ret = *బఫరు;

    ఒకవేళ (Check_బఫరు((బఫరు_Object *)(ret)))
    {
	*బఫరు = ఖాళీ;
	తిరిగి ఖాళీ;
    }

    ఒకవేళ (!((బఫరు_Object *)(ret))->buf->b_తదుపరి)
	తదుపరి = ఖాళీ;
    లేకపోతే ఒకవేళ (!(తదుపరి = బఫరు_New(((బఫరు_Object *)(ret))->buf->b_తదుపరి)))
	తిరిగి ఖాళీ;
    *బఫరు = తదుపరి;
    // Do not increment reference: we no longer hold it (decref), but whoever
    // on other side will hold (incref). Decref+incref = nothing.
    తిరిగి ret;
}

    స్థిర PyObject *
BufMapIter(PyObject *self)
{
    PyObject *బఫరు;

    బఫరు = బఫరు_New(firstbuf);
    తిరిగి IterNew(బఫరు,
	    BufMapIterDestruct, Buf_Map_Iter_తదుపరి,
	    BufMapIterTraverse, BufMapIterClear,
	    self);
}

స్థిర PyMappingMethods BufMapAsMapping = {
    (lenfunc)       Buf_Map_నిడివి,
    (binaryfunc)    Buf_Map_అంశం,
    (objobjargproc) 0,
};

// Current items object

స్థిర అక్షరం *CurrentAttrs[] = {
    "buffer", "window", "line", "range", "tabpage",
    ఖాళీ
};

    స్థిర PyObject *
CurrentDir(PyObject *self, PyObject *వాదనలు వాడని)
{
    తిరిగి ObjectDir(self, CurrentAttrs);
}

    స్థిర PyObject *
CurrentGetattr(PyObject *self వాడని, అక్షరం *name)
{
    ఒకవేళ (strcmp(name, "buffer") == 0)
	తిరిగి (PyObject *)బఫరు_New(curbuf);
    లేకపోతే ఒకవేళ (strcmp(name, "window") == 0)
	తిరిగి (PyObject *)కిటికీ_New(curwin, curtab);
    లేకపోతే ఒకవేళ (strcmp(name, "tabpage") == 0)
	తిరిగి (PyObject *)Tab_పుట_New(curtab);
    లేకపోతే ఒకవేళ (strcmp(name, "line") == 0)
	తిరిగి Get_బఫరు_పంక్తి(curbuf, (PyInt)curwin->w_cursor.lnum);
    లేకపోతే ఒకవేళ (strcmp(name, "range") == 0)
	తిరిగి RangeNew(curbuf, Range_ప్రారంభం, Range_చివర);
    లేకపోతే ఒకవేళ (strcmp(name, "__members__") == 0)
	తిరిగి ObjectDir(ఖాళీ, CurrentAttrs);
    లేకపోతే
#ఒకవేళ PY_MAJOR_సంస్కరణ < 3
	తిరిగి Py_కనుగొను_పద్ధతి(కిటికీ_Methods, self, name);
#లేకపోతే
	తిరిగి ఖాళీ;
#అయితే_ముగింపు
}

    స్థిర పూర్ణాంకం
CurrentSetattr(PyObject *self వాడని, అక్షరం *name, PyObject *valObject)
{
    ఒకవేళ (strcmp(name, "line") == 0)
    {
	ఒకవేళ (సమితి_బఫరు_పంక్తి_2(curbuf, (PyInt)curwin->w_cursor.lnum, valObject,
			  ఖాళీ) == FAIL)
	    తిరిగి -1;

	తిరిగి 0;
    }
    లేకపోతే ఒకవేళ (strcmp(name, "buffer") == 0)
    {
	పూర్ణాంకం లెక్క;

	ఒకవేళ (valObject->ob_type != బఫరు_Type_Ptr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.Buffer object, but got %s"),
		    valObject);
	    తిరిగి -1;
	}

	ఒకవేళ (Check_బఫరు((బఫరు_Object *)(valObject)))
	    తిరిగి -1;
	లెక్క = ((బఫరు_Object *)(valObject))->buf->b_fnum;

	Vim_Try_ప్రారంభం();
	ఒకవేళ (do_బఫరు(DOBUF_GOTO, DOBUF_మొదటి, FORWARD, లెక్క, 0) == FAIL)
	{
	    ఒకవేళ (Vim_Try_చివర())
		తిరిగి -1;
	    PyErr_VIM_FORMAT(N_("failed to switch to buffer %d"), లెక్క);
	    తిరిగి -1;
	}

	తిరిగి Vim_Try_చివర();
    }
    లేకపోతే ఒకవేళ (strcmp(name, "window") == 0)
    {
	పూర్ణాంకం లెక్క;

	ఒకవేళ (valObject->ob_type != కిటికీ_Type_Ptr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.Window object, but got %s"),
		    valObject);
	    తిరిగి -1;
	}

	ఒకవేళ (Check_కిటికీ((కిటికీ_Object *)(valObject)))
	    తిరిగి -1;
	లెక్క = get_win_సంఖ్య(((కిటికీ_Object *)(valObject))->win, firstwin);

	ఒకవేళ (!లెక్క)
	{
	    Py_Err_సమితి_STRING(Py_Exc_విలువ_Error,
		    N_("failed to find window in the current tab page"));
	    తిరిగి -1;
	}

	Vim_Try_ప్రారంభం();
	win_goto(((కిటికీ_Object *)(valObject))->win);
	ఒకవేళ (((కిటికీ_Object *)(valObject))->win != curwin)
	{
	    ఒకవేళ (Vim_Try_చివర())
		తిరిగి -1;
	    Py_Err_సమితి_STRING(PyExc_RuntimeError,
		    N_("did not switch to the specified window"));
	    తిరిగి -1;
	}

	తిరిగి Vim_Try_చివర();
    }
    లేకపోతే ఒకవేళ (strcmp(name, "tabpage") == 0)
    {
	ఒకవేళ (valObject->ob_type != Tab_పుట_Type_Ptr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.TabPage object, but got %s"),
		    valObject);
	    తిరిగి -1;
	}

	ఒకవేళ (Check_Tab_పుట((Tab_పుట_Object *)(valObject)))
	    తిరిగి -1;

	Vim_Try_ప్రారంభం();
	goto_tabpage_tp(((Tab_పుట_Object *)(valObject))->tab, TRUE, TRUE);
	ఒకవేళ (((Tab_పుట_Object *)(valObject))->tab != curtab)
	{
	    ఒకవేళ (Vim_Try_చివర())
		తిరిగి -1;
	    Py_Err_సమితి_STRING(PyExc_RuntimeError,
		    N_("did not switch to the specified tab page"));
	    తిరిగి -1;
	}

	తిరిగి Vim_Try_చివర();
    }
    లేకపోతే
    {
	Py_Err_సమితి_String(PyExc_AttributeError, name);
	తిరిగి -1;
    }
}

స్థిర నిర్మాణం Py_పద్ధతి_Def CurrentMethods[] = {
    // name,	    function,			calling,	documentation
    {"__dir__",	    (PyCFunction)CurrentDir,	METH_NOARGS,	""},
    { ఖాళీ,	    ఖాళీ,			0,		ఖాళీ}
};

    స్థిర శూన్య
init_range_cmd(శూన్య *arg)
{
    exarg_T *eap = (exarg_T*)arg;
    Range_ప్రారంభం = eap->line1;
    Range_చివర = eap->line2;
}

    స్థిర శూన్య
init_range_eval(శూన్య *rettv వాడని)
{
    Range_ప్రారంభం = (PyInt) curwin->w_cursor.lnum;
    Range_చివర = Range_ప్రారంభం;
}

    స్థిర శూన్య
నిరంతరం_cmd(స్థిరరాశి అక్షరం *cmd, dict_T* locals వాడని, శూన్య *arg వాడని
#నిర్వచితమైతే PY_CAN_RECURSE
	, PyGILState_STATE *pygilstate వాడని
#అయితే_ముగింపు
	)
{
    PyObject	*నిరంతరం_ret;
    నిరంతరం_ret = Py_నిరంతరం_String((అక్షరం *)cmd, Py_file_ఆదానం, globals, globals);
    ఒకవేళ (నిరంతరం_ret != ఖాళీ)
    {
	Py_DECREF(నిరంతరం_ret);
    }
    లేకపోతే ఒకవేళ (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
    {
	emsg(_(e_cant_handle_systemexit_యొక్క_python_exception_లోపల_vim));
	PyErr_Clear();
    }
    లేకపోతే
	PyErr_PrintEx(1);
}

స్థిర స్థిరరాశి అక్షరం	*సంకేతం_hdr = "def " DOPY_FUNC "(line, linenr):\n ";
స్థిర పూర్ణాంకం		సంకేతం_hdr_len = 30;

    స్థిర శూన్య
నిరంతరం_do(స్థిరరాశి అక్షరం *cmd, dict_T* locals వాడని, శూన్య *arg వాడని
#నిర్వచితమైతే PY_CAN_RECURSE
	, PyGILState_STATE *pygilstate
#అయితే_ముగింపు
	)
{
    PyInt	lnum;
    size_t	len;
    అక్షరం	*సంకేతం_2;
    పూర్ణాంకం		స్థితి_2;
    PyObject	*pyfunc, *pymain;
    PyObject	*నిరంతరం_ret;
    buf_T	*was_curbuf = curbuf;

    ఒకవేళ (u_save((linenr_T)Range_ప్రారంభం - 1, (linenr_T)Range_చివర + 1) != OK)
    {
	emsg(_("cannot save undo information"));
	తిరిగి;
    }

    len = సంకేతం_hdr_len + STRLEN(cmd);
    సంకేతం_2 = PyMem_New(అక్షరం, len + 1);
    జ్ఞాపకం_నకలు(సంకేతం_2, సంకేతం_hdr, సంకేతం_hdr_len);
    STRCPY(సంకేతం_2 + సంకేతం_hdr_len, cmd);
    నిరంతరం_ret = Py_నిరంతరం_String(సంకేతం_2, Py_file_ఆదానం, globals, globals);
    స్థితి_2 = -1;
    ఒకవేళ (నిరంతరం_ret != ఖాళీ)
    {
	స్థితి_2 = 0;
	Py_DECREF(నిరంతరం_ret);
    }
    లేకపోతే ఒకవేళ (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
    {
	PyMem_Free(సంకేతం_2);
	emsg(_(e_cant_handle_systemexit_యొక్క_python_exception_లోపల_vim));
	PyErr_Clear();
	తిరిగి;
    }
    లేకపోతే
	PyErr_PrintEx(1);

    PyMem_Free(సంకేతం_2);

    ఒకవేళ (స్థితి_2)
    {
	emsg(_("failed to run the code"));
	తిరిగి;
    }

    స్థితి_2 = 0;
    pymain = Py_Import_కూడు_Module("__main__");
    pyfunc = PyObject_GetAttrString(pymain, DOPY_FUNC);
#నిర్వచితమైతే PY_CAN_RECURSE
    PyGILState_Release(*pygilstate);
#అయితే_ముగింపు

    కొరకు (lnum = Range_ప్రారంభం; lnum <= Range_చివర; ++lnum)
    {
	PyObject	*పంక్తి;
	PyObject	*linenr;
	PyObject	*ret;

#నిర్వచితమైతే PY_CAN_RECURSE
	*pygilstate = PyGILState_Ensure();
#అయితే_ముగింపు
	// Check the line number, the command my have deleted lines.
	ఒకవేళ (lnum > curbuf->b_ml.ml_పంక్తి_లెక్క
		|| !(పంక్తి = Get_బఫరు_పంక్తి(curbuf, lnum)))
	    వెళ్ళు err;
	ఒకవేళ (!(linenr = PyInt_FromLong((దీర్ఘ) lnum)))
	{
	    Py_DECREF(పంక్తి);
	    వెళ్ళు err;
	}
	ret = Py_Object_Call_Function_Obj_వాదనలు(pyfunc, పంక్తి, linenr, ఖాళీ);
	Py_DECREF(పంక్తి);
	Py_DECREF(linenr);
	ఒకవేళ (!ret)
	    వెళ్ళు err;

	// Check that the command didn't switch to another buffer.
	// Check the line number, the command my have deleted lines.
	ఒకవేళ (curbuf != was_curbuf || lnum > curbuf->b_ml.ml_పంక్తి_లెక్క)
	{
	    Py_XDECREF(ret);
	    వెళ్ళు err;
	}

	ఒకవేళ (ret != Py_None)
	    ఒకవేళ (సమితి_బఫరు_పంక్తి_2(curbuf, lnum, ret, ఖాళీ) == FAIL)
	    {
		Py_XDECREF(ret);
		వెళ్ళు err;
	    }

	Py_XDECREF(ret);
	PythonIO_Flush();
#నిర్వచితమైతే PY_CAN_RECURSE
	PyGILState_Release(*pygilstate);
#అయితే_ముగింపు
    }
    వెళ్ళు బహిర్గతం;
err:
#నిర్వచితమైతే PY_CAN_RECURSE
    *pygilstate = PyGILState_Ensure();
#అయితే_ముగింపు
    PyErr_PrintEx(0);
    PythonIO_Flush();
    స్థితి_2 = 1;
బహిర్గతం:
#నిర్వచితమైతే PY_CAN_RECURSE
    ఒకవేళ (!స్థితి_2)
	*pygilstate = PyGILState_Ensure();
#అయితే_ముగింపు
    Py_DECREF(pyfunc);
    Py_Object_సమితి_Attr_String(pymain, DOPY_FUNC, ఖాళీ);
    ఒకవేళ (స్థితి_2)
	తిరిగి;
    check_cursor();
    update_curbuf(UPD_NOT_VALID);
}

    స్థిర శూన్య
నిరంతరం_eval(స్థిరరాశి అక్షరం *cmd, dict_T *locals, శూన్య *arg
#నిర్వచితమైతే PY_CAN_RECURSE
	, PyGILState_STATE *pygilstate వాడని
#అయితే_ముగింపు
	)
{
    PyObject	*నిరంతరం_ret;
    typval_T	*rettv = (typval_T*)arg;
    PyObject	*pylocals = locals ? NEW_DICTIONARY(locals) : globals;

    నిరంతరం_ret = Py_నిరంతరం_String((అక్షరం *)cmd, Py_eval_ఆదానం, globals, pylocals);
    ఒకవేళ (నిరంతరం_ret == ఖాళీ)
    {
	ఒకవేళ (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
	{
	    emsg(_(e_cant_handle_systemexit_యొక్క_python_exception_లోపల_vim));
	    PyErr_Clear();
	}
	లేకపోతే
	{
	    ఒకవేళ (PyErr_Occurred() && !సందేశం_silent)
		PyErr_PrintEx(0);
	    emsg(_(e_eval_did_not_return_valid_python_object));
	}
    }
    లేకపోతే
    {
	ఒకవేళ (ConvertFromPyObject(నిరంతరం_ret, rettv) == -1)
	    emsg(_(e_failed_గమ్యం_convert_returned_python_object_గమ్యం_vim_విలువ));
	Py_DECREF(నిరంతరం_ret);
    }
    PyErr_Clear();
}

    స్థిర పూర్ణాంకం
సమితి_ref_లోపల_py(స్థిరరాశి పూర్ణాంకం copyID)
{
    pylinkedlist_T	*cur;
    list_T		*ll;
    tuple_T		*tt;
    పూర్ణాంకం			i;
    పూర్ణాంకం			abort = FALSE;
    FunctionObject	*func;

    ఒకవేళ (lastdict != ఖాళీ)
    {
	కొరకు (cur = lastdict ; !abort && cur != ఖాళీ ; cur = cur->pll_prev)
	    abort = సమితి_ref_లోపల_dict(((DictionaryObject *)(cur->pll_obj))->dict,
								       copyID);
    }

    ఒకవేళ (lastlist != ఖాళీ)
    {
	కొరకు (cur = lastlist ; !abort && cur != ఖాళీ ; cur = cur->pll_prev)
	{
	    ll = ((ListObject *) (cur->pll_obj))->list;
	    abort = సమితి_ref_లోపల_list(ll, copyID);
	}
    }

    ఒకవేళ (lasttuple != ఖాళీ)
    {
	కొరకు (cur = lasttuple ; !abort && cur != ఖాళీ ; cur = cur->pll_prev)
	{
	    tt = ((TupleObject *) (cur->pll_obj))->tuple;
	    abort = సమితి_ref_లోపల_tuple(tt, copyID);
	}
    }

    ఒకవేళ (lastfunc != ఖాళీ)
    {
	కొరకు (cur = lastfunc ; !abort && cur != ఖాళీ ; cur = cur->pll_prev)
	{
	    func = (FunctionObject *) cur->pll_obj;
	    abort = సమితి_ref_లోపల_dict(func->self, copyID);
	    ఒకవేళ (func->argc)
		కొరకు (i = 0; !abort && i < func->argc; ++i)
		    abort = abort
			|| సమితి_ref_లోపల_అంశం(&func->argv[i], copyID, ఖాళీ, ఖాళీ, ఖాళీ);
	}
    }

    తిరిగి abort;
}

    స్థిర పూర్ణాంకం
సమితి_string_copy(char_u *str, typval_T *tv)
{
    tv->vval.v_string = vim_strsave(str);
    ఒకవేళ (tv->vval.v_string == ఖాళీ)
    {
	Py_Err_No_జ్ఞాపకం();
	తిరిగి -1;
    }
    తిరిగి 0;
}

    స్థిర పూర్ణాంకం
pydict_గమ్యం_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    dict_T	*dict;
    char_u	*కీలకం;
    dictitem_T	*di;
    PyObject	*కీలకం_Object;
    PyObject	*valObject;
    Py_ssize_t	iter = 0;

    ఒకవేళ (!(dict = py_dict_alloc()))
	తిరిగి -1;

    tv->v_type = VAR_DICT;
    tv->vval.v_dict = dict;

    అయితే (Py_Dict_తదుపరి(obj, &iter, &కీలకం_Object, &valObject))
    {
	PyObject	*todecref = ఖాళీ;

	ఒకవేళ (కీలకం_Object == ఖాళీ || valObject == ఖాళీ)
	{
	    dict_unref(dict);
	    తిరిగి -1;
	}

	ఒకవేళ (!(కీలకం = String_గమ్యం_Chars(కీలకం_Object, &todecref)))
	{
	    dict_unref(dict);
	    తిరిగి -1;
	}

	ఒకవేళ (*కీలకం == NUL)
	{
	    dict_unref(dict);
	    Py_XDECREF(todecref);
	    RAISE_NO_ఖాళీలు_కీలకాలు;
	    తిరిగి -1;
	}

	di = dictitem_alloc(కీలకం);

	Py_XDECREF(todecref);

	ఒకవేళ (di == ఖాళీ)
	{
	    Py_Err_No_జ్ఞాపకం();
	    dict_unref(dict);
	    తిరిగి -1;
	}

	ఒకవేళ (_ConvertFromPyObject(valObject, &di->di_tv, lookup_dict) == -1)
	{
	    vim_free(di);
	    dict_unref(dict);
	    తిరిగి -1;
	}

	ఒకవేళ (dict_కూడు(dict, di) == FAIL)
	{
	    RAISE_కీలకం_కూడు_FAIL(di->di_కీలకం);
	    clear_tv(&di->di_tv);
	    vim_free(di);
	    dict_unref(dict);
	    తిరిగి -1;
	}
    }

    --dict->dv_refcount;
    తిరిగి 0;
}

    స్థిర పూర్ణాంకం
pymap_గమ్యం_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    dict_T	*dict;
    char_u	*కీలకం;
    dictitem_T	*di;
    PyObject	*list;
    PyObject	*iterator;
    PyObject	*కీలకం_Object;
    PyObject	*valObject;

    ఒకవేళ (!(dict = py_dict_alloc()))
	తిరిగి -1;

    tv->v_type = VAR_DICT;
    tv->vval.v_dict = dict;

    ఒకవేళ (!(list = Py_Mapping_కీలకాలు(obj)))
    {
	dict_unref(dict);
	తిరిగి -1;
    }

    ఒకవేళ (!(iterator = PyObject_GetIter(list)))
    {
	dict_unref(dict);
	Py_DECREF(list);
	తిరిగి -1;
    }
    Py_DECREF(list);

    అయితే ((కీలకం_Object = Py_Iter_తదుపరి(iterator)))
    {
	PyObject	*todecref;

	ఒకవేళ (!(కీలకం = String_గమ్యం_Chars(కీలకం_Object, &todecref)))
	{
	    Py_DECREF(కీలకం_Object);
	    Py_DECREF(iterator);
	    dict_unref(dict);
	    తిరిగి -1;
	}

	ఒకవేళ (*కీలకం == NUL)
	{
	    Py_DECREF(కీలకం_Object);
	    Py_DECREF(iterator);
	    Py_XDECREF(todecref);
	    dict_unref(dict);
	    RAISE_NO_ఖాళీలు_కీలకాలు;
	    తిరిగి -1;
	}

	ఒకవేళ (!(valObject = Py_Object_Get_అంశం(obj, కీలకం_Object)))
	{
	    Py_DECREF(కీలకం_Object);
	    Py_DECREF(iterator);
	    Py_XDECREF(todecref);
	    dict_unref(dict);
	    తిరిగి -1;
	}

	di = dictitem_alloc(కీలకం);

	Py_DECREF(కీలకం_Object);
	Py_XDECREF(todecref);

	ఒకవేళ (di == ఖాళీ)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(valObject);
	    dict_unref(dict);
	    Py_Err_No_జ్ఞాపకం();
	    తిరిగి -1;
	}

	ఒకవేళ (_ConvertFromPyObject(valObject, &di->di_tv, lookup_dict) == -1)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(valObject);
	    vim_free(di);
	    dict_unref(dict);
	    తిరిగి -1;
	}

	Py_DECREF(valObject);

	ఒకవేళ (dict_కూడు(dict, di) == FAIL)
	{
	    RAISE_కీలకం_కూడు_FAIL(di->di_కీలకం);
	    Py_DECREF(iterator);
	    dictitem_free(di);
	    dict_unref(dict);
	    తిరిగి -1;
	}
    }
    Py_DECREF(iterator);
    --dict->dv_refcount;
    తిరిగి 0;
}

    స్థిర పూర్ణాంకం
pyseq_గమ్యం_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    list_T	*l;

    ఒకవేళ (!(l = py_list_alloc()))
	తిరిగి -1;

    tv->v_type = VAR_LIST;
    tv->vval.v_list = l;

    ఒకవేళ (list_py_concat(l, obj, lookup_dict) == -1)
    {
	list_unref(l);
	తిరిగి -1;
    }

    --l->lv_refcount;
    తిరిగి 0;
}

    స్థిర పూర్ణాంకం
pytuple_గమ్యం_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    tuple_T	*t;

    ఒకవేళ (!(t = py_tuple_alloc()))
	తిరిగి -1;

    tv->v_type = VAR_TUPLE;
    tv->vval.v_tuple = t;

    ఒకవేళ (tuple_py_concat(t, obj, lookup_dict) == -1)
    {
	tuple_unref(t);
	తిరిగి -1;
    }

    --t->tv_refcount;
    తిరిగి 0;
}

రకనిర్వచనం పూర్ణాంకం (*pytotvfunc)(PyObject *, typval_T *, PyObject *);

    స్థిర పూర్ణాంకం
convert_dl(PyObject *obj, typval_T *tv,
				    pytotvfunc py_గమ్యం_tv, PyObject *lookup_dict)
{
    PyObject	*capsule;
    అక్షరం	hexBuf[పరిమాణం(శూన్య *) * 2 + 3];

    sprintf(hexBuf, "%p", (శూన్య *)obj);

#నిర్వచితమైతే PY_USE_CAPSULE
    capsule = Py_Dict_Get_అంశం_String(lookup_dict, hexBuf);
#లేకపోతే
    capsule = (PyObject *)Py_Dict_Get_అంశం_String(lookup_dict, hexBuf);
#అయితే_ముగింపు
    ఒకవేళ (capsule == ఖాళీ)
    {
#నిర్వచితమైతే PY_USE_CAPSULE
	capsule = PyCapsule_New(tv, ఖాళీ, ఖాళీ);
#లేకపోతే
	capsule = PyCObject_FromVoidPtr(tv, ఖాళీ);
#అయితే_ముగింపు
	ఒకవేళ (Py_Dict_సమితి_అంశం_String(lookup_dict, hexBuf, capsule))
	{
	    Py_DECREF(capsule);
	    tv->v_type = VAR_UNKNOWN;
	    తిరిగి -1;
	}

	Py_DECREF(capsule);

	ఒకవేళ (py_గమ్యం_tv(obj, tv, lookup_dict) == -1)
	{
	    tv->v_type = VAR_UNKNOWN;
	    తిరిగి -1;
	}
	// As we are not using copy_tv which increments reference count we must
	// do it ourself.
	ఒకవేళ (tv->v_type == VAR_DICT)
	    ++tv->vval.v_dict->dv_refcount;
	లేకపోతే ఒకవేళ (tv->v_type == VAR_LIST)
	    ++tv->vval.v_list->lv_refcount;
	లేకపోతే ఒకవేళ (tv->v_type == VAR_TUPLE)
	    ++tv->vval.v_tuple->tv_refcount;
    }
    లేకపోతే
    {
	typval_T	*v;

#నిర్వచితమైతే PY_USE_CAPSULE
	v = Py_Capsule_Get_సూచిక(capsule, ఖాళీ);
#లేకపోతే
	v = PyCObject_AsVoidPtr(capsule);
#అయితే_ముగింపు
	copy_tv(v, tv);
    }
    తిరిగి 0;
}

    స్థిర పూర్ణాంకం
ConvertFromPyMapping(PyObject *obj, typval_T *tv)
{
    PyObject	*lookup_dict;
    పూర్ణాంకం		ret;

    ఒకవేళ (!(lookup_dict = PyDict_New()))
	తిరిగి -1;

    ఒకవేళ (PyType_IsSubtype(obj->ob_type, DictionaryTypePtr))
    {
	tv->v_type = VAR_DICT;
	tv->vval.v_dict = (((DictionaryObject *)(obj))->dict);
	++tv->vval.v_dict->dv_refcount;
	ret = 0;
    }
    లేకపోతే ఒకవేళ (PyDict_Check(obj))
	ret = convert_dl(obj, tv, pydict_గమ్యం_tv, lookup_dict);
    లేకపోతే ఒకవేళ (PyMapping_Check(obj))
	ret = convert_dl(obj, tv, pymap_గమ్యం_tv, lookup_dict);
    లేకపోతే
    {
	PyErr_FORMAT_TYPE(
		N_("unable to convert %s to a Vim dictionary"),
		obj);
	ret = -1;
    }
    Py_DECREF(lookup_dict);
    తిరిగి ret;
}

    స్థిర పూర్ణాంకం
ConvertFromPySequence(PyObject *obj, typval_T *tv)
{
    PyObject	*lookup_dict;
    పూర్ణాంకం		ret;

    ఒకవేళ (!(lookup_dict = PyDict_New()))
	తిరిగి -1;

    ఒకవేళ (PyType_IsSubtype(obj->ob_type, ListTypePtr))
    {
	tv->v_type = VAR_LIST;
	tv->vval.v_list = (((ListObject *)(obj))->list);
	++tv->vval.v_list->lv_refcount;
	ret = 0;
    }
    లేకపోతే ఒకవేళ (PyIter_Check(obj) || PySequence_Check(obj))
	ret = convert_dl(obj, tv, pyseq_గమ్యం_tv, lookup_dict);
    లేకపోతే
    {
	PyErr_FORMAT_TYPE(
		N_("unable to convert %s to a Vim list"),
		obj);
	ret = -1;
    }
    Py_DECREF(lookup_dict);
    తిరిగి ret;
}

    స్థిర పూర్ణాంకం
ConvertFromPyObject(PyObject *obj, typval_T *tv)
{
    PyObject	*lookup_dict;
    పూర్ణాంకం		ret;

    ఒకవేళ (!(lookup_dict = PyDict_New()))
	తిరిగి -1;
    ret = _ConvertFromPyObject(obj, tv, lookup_dict);
    Py_DECREF(lookup_dict);
    తిరిగి ret;
}

    స్థిర పూర్ణాంకం
_ConvertFromPyObject(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    ఒకవేళ (PyType_IsSubtype(obj->ob_type, DictionaryTypePtr))
    {
	tv->v_type = VAR_DICT;
	tv->vval.v_dict = (((DictionaryObject *)(obj))->dict);
	++tv->vval.v_dict->dv_refcount;
    }
    లేకపోతే ఒకవేళ (PyType_IsSubtype(obj->ob_type, ListTypePtr))
    {
	tv->v_type = VAR_LIST;
	tv->vval.v_list = (((ListObject *)(obj))->list);
	++tv->vval.v_list->lv_refcount;
    }
    లేకపోతే ఒకవేళ (PyType_IsSubtype(obj->ob_type, TupleTypePtr))
    {
	tv->v_type = VAR_TUPLE;
	tv->vval.v_tuple = (((TupleObject *)(obj))->tuple);
	++tv->vval.v_tuple->tv_refcount;
    }
    లేకపోతే ఒకవేళ (PyType_IsSubtype(obj->ob_type, FunctionTypePtr))
    {
	FunctionObject *func = (FunctionObject *) obj;
	ఒకవేళ (func->self != ఖాళీ || func->argv != ఖాళీ)
	{
	    partial_T *pt = ALLOC_CLEAR_ONE(partial_T);

	    సమితి_partial(func, pt, TRUE);
	    tv->vval.v_partial = pt;
	    tv->v_type = VAR_PARTIAL;
	}
	లేకపోతే
	{
	    ఒకవేళ (సమితి_string_copy(func->name, tv) == -1)
		తిరిగి -1;

	    tv->v_type = VAR_FUNC;
	}
	func_ref(func->name);
    }
    లేకపోతే ఒకవేళ (Py_బైట్లు_Check(obj))
    {
	char_u	*str;

	ఒకవేళ (Py_బైట్లు_As_String_And_కొలత(obj, (అక్షరం **) &str, ఖాళీ) == -1)
	    తిరిగి -1;
	ఒకవేళ (str == ఖాళీ)
	    తిరిగి -1;

	ఒకవేళ (సమితి_string_copy(str, tv) == -1)
	    తిరిగి -1;

	tv->v_type = VAR_STRING;
    }
    లేకపోతే ఒకవేళ (PyUnicode_Check(obj))
    {
	PyObject	*బైట్లు;
	char_u	*str;

	బైట్లు = PyUnicode_AsEncodedString(obj, ENC_OPT, ERRORS_ENCODE_ARG);
	ఒకవేళ (బైట్లు == ఖాళీ)
	    తిరిగి -1;

	ఒకవేళ (Py_బైట్లు_As_String_And_కొలత(బైట్లు, (అక్షరం **) &str, ఖాళీ) == -1)
	    తిరిగి -1;
	ఒకవేళ (str == ఖాళీ)
	    తిరిగి -1;

	ఒకవేళ (సమితి_string_copy(str, tv))
	{
	    Py_XDECREF(బైట్లు);
	    తిరిగి -1;
	}
	Py_XDECREF(బైట్లు);

	tv->v_type = VAR_STRING;
    }
#ఒకవేళ PY_MAJOR_సంస్కరణ < 3
    లేకపోతే ఒకవేళ (PyInt_Check(obj))
    {
	tv->v_type = VAR_సంఖ్య;
	tv->vval.v_సంఖ్య = (varnumber_T) PyInt_AsLong(obj);
	ఒకవేళ (PyErr_Occurred())
	    తిరిగి -1;
    }
#అయితే_ముగింపు
    లేకపోతే ఒకవేళ (PyLong_Check(obj))
    {
	tv->v_type = VAR_సంఖ్య;
	tv->vval.v_సంఖ్య = (varnumber_T) PyLong_AsLong(obj);
	ఒకవేళ (PyErr_Occurred())
	    తిరిగి -1;
    }
    లేకపోతే ఒకవేళ (PyTuple_Check(obj))
	తిరిగి convert_dl(obj, tv, pytuple_గమ్యం_tv, lookup_dict);
    లేకపోతే ఒకవేళ (PyDict_Check(obj))
	తిరిగి convert_dl(obj, tv, pydict_గమ్యం_tv, lookup_dict);
    లేకపోతే ఒకవేళ (PyFloat_Check(obj))
    {
	tv->v_type = VAR_FLOAT;
	tv->vval.v_float = (float_T) PyFloat_AsDouble(obj);
    }
    లేకపోతే ఒకవేళ (PyObject_HasAttrString(obj, "keys"))
	తిరిగి convert_dl(obj, tv, pymap_గమ్యం_tv, lookup_dict);
    // PyObject_GetIter can create built-in iterator for any sequence object
    లేకపోతే ఒకవేళ (PyIter_Check(obj) || PySequence_Check(obj))
	తిరిగి convert_dl(obj, tv, pyseq_గమ్యం_tv, lookup_dict);
    లేకపోతే ఒకవేళ (PyMapping_Check(obj))
	తిరిగి convert_dl(obj, tv, pymap_గమ్యం_tv, lookup_dict);
    లేకపోతే ఒకవేళ (Py_సంఖ్య_Check(obj))
    {
	PyObject	*అంకె;

	ఒకవేళ (!(అంకె = Py_సంఖ్య_Long(obj)))
	    తిరిగి -1;

	tv->v_type = VAR_సంఖ్య;
	tv->vval.v_సంఖ్య = (varnumber_T) PyLong_AsLong(అంకె);

	Py_DECREF(అంకె);
    }
    లేకపోతే ఒకవేళ (obj == Py_None)
    {
	tv->v_type = VAR_SPECIAL;
	tv->vval.v_సంఖ్య = VVAL_NONE;
    }
    లేకపోతే
    {
	PyErr_FORMAT_TYPE(
		N_("unable to convert %s to a Vim structure"),
		obj);
	తిరిగి -1;
    }
    తిరిగి 0;
}

    స్థిర PyObject *
Convert_గమ్యం_Py_Object(typval_T *tv)
{
    typval_T *argv;
    పూర్ణాంకం i;
    ఒకవేళ (tv == ఖాళీ)
    {
	Py_Err_సమితి_VIM(N_("internal error: NULL reference passed"));
	తిరిగి ఖాళీ;
    }
    ఎంపిక (tv->v_type)
    {
	స్థితి VAR_STRING:
	    తిరిగి Py_బైట్లు_From_String(tv->vval.v_string == ఖాళీ
					    ? "" : (అక్షరం *)tv->vval.v_string);
	స్థితి VAR_సంఖ్య:
	    తిరిగి PyLong_FromLong((దీర్ఘ) tv->vval.v_సంఖ్య);
	స్థితి VAR_FLOAT:
	    తిరిగి PyFloat_FromDouble((ద్విగుణ) tv->vval.v_float);
	స్థితి VAR_LIST:
	    తిరిగి NEW_LIST(tv->vval.v_list);
	స్థితి VAR_TUPLE:
	    తిరిగి NEW_TUPLE(tv->vval.v_tuple);
	స్థితి VAR_DICT:
	    తిరిగి NEW_DICTIONARY(tv->vval.v_dict);
	స్థితి VAR_FUNC:
	    తిరిగి NEW_FUNCTION(tv->vval.v_string == ఖాళీ
					  ? (char_u *)"" : tv->vval.v_string,
					  0, ఖాళీ, ఖాళీ, TRUE);
	స్థితి VAR_PARTIAL:
	    ఒకవేళ (tv->vval.v_partial->pt_argc)
	    {
		argv = PyMem_New(typval_T, (size_t)tv->vval.v_partial->pt_argc);
		కొరకు (i = 0; i < tv->vval.v_partial->pt_argc; i++)
		    copy_tv(&tv->vval.v_partial->pt_argv[i], &argv[i]);
	    }
	    లేకపోతే
		argv = ఖాళీ;
	    ఒకవేళ (tv->vval.v_partial->pt_dict != ఖాళీ)
		tv->vval.v_partial->pt_dict->dv_refcount++;
	    తిరిగి NEW_FUNCTION(tv->vval.v_partial == ఖాళీ
			     ? (char_u *)"" : partial_name(tv->vval.v_partial),
				tv->vval.v_partial->pt_argc, argv,
				tv->vval.v_partial->pt_dict,
				tv->vval.v_partial->pt_auto);
	స్థితి VAR_BLOB:
	    తిరిగి Py_బైట్లు_From_String_And_కొలత(
		(అక్షరం*) tv->vval.v_blob->bv_ga.ga_దత్తాంశం,
		(Py_ssize_t) tv->vval.v_blob->bv_ga.ga_len);
	స్థితి VAR_UNKNOWN:
	స్థితి VAR_ANY:
	స్థితి VAR_VOID:
	స్థితి VAR_CHANNEL:
	స్థితి VAR_పని:
	స్థితి VAR_INSTR:
	స్థితి VAR_CLASS:
	స్థితి VAR_OBJECT:
	స్థితి VAR_TYPEALIAS:
	    Py_INCREF(Py_None);
	    తిరిగి Py_None;
	స్థితి VAR_BOOL:
	స్థితి VAR_SPECIAL:
	    ఎంపిక (tv->vval.v_సంఖ్య)
	    {
		స్థితి VVAL_FALSE: తిరిగి ALWAYS_FALSE;
		స్థితి VVAL_TRUE:  తిరిగి ALWAYS_TRUE;
		స్థితి VVAL_NONE:
		స్థితి VVAL_NULL:  తిరిగి ALWAYS_NONE;
	    }
	    Py_Err_సమితి_VIM(N_("internal error: invalid value type"));
	    తిరిగి ఖాళీ;
    }
    తిరిగి ఖాళీ;
}

DEFINE_PY_TYPE_OBJECT(CurrentType);

రకనిర్వచనం నిర్మాణం
{
    Py_Object_తల
} CurrentObject;

స్థిర CurrentObject TheCurrent =
{
    Py_Object_తల_INIT_TYPE(CurrentType)
};

    స్థిర శూన్య
init_structs(శూన్య)
{
    CLEAR_క్షేత్రం(OutputType);
    OutputType.tp_name = "vim.message";
    OutputType.tp_basicsize = పరిమాణం(OutputObject);
    OutputType.tp_flags = Py_TPFLAGS_DEFAULT;
    OutputType.tp_doc = "vim message object";
    OutputType.tp_methods = OutputMethods;
#ఒకవేళ PY_MAJOR_సంస్కరణ >= 3
    OutputType.tp_getattro = OutputGetattro;
    OutputType.tp_setattro = OutputSetattro;
    OutputType.tp_alloc = call_PyType_GenericAlloc;
    OutputType.tp_new = call_PyType_GenericNew;
    OutputType.tp_free = call_PyObject_Free;
# ifndef USE_LIMITED_API
    // The std printer type is only exposed in full API. It is not essential
    // anyway and so in limited API we don't set it.
    OutputType.tp_పునాది = &PyStdPrinter_Type;
# endif
#లేకపోతే
    OutputType.tp_getattr = OutputGetattr;
    OutputType.tp_setattr = OutputSetattr;
    // Disabled, because this causes a crash in test86
    // OutputType.tp_base = &PyFile_Type;
#అయితే_ముగింపు

    CLEAR_క్షేత్రం(IterType);
    IterType.tp_name = "vim.iter";
    IterType.tp_basicsize = పరిమాణం(IterObject);
    IterType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    IterType.tp_doc = "generic iterator object";
    IterType.tp_iter = IterIter;
    IterType.tp_iternext = Iter_తదుపరి;
    IterType.tp_dealloc = IterDestructor;
    IterType.tp_traverse = IterTraverse;
    IterType.tp_clear = IterClear;

    CLEAR_క్షేత్రం(బఫరు_Type);
    బఫరు_Type.tp_name = "vim.buffer";
    బఫరు_Type.tp_basicsize = పరిమాణం(బఫరు_Object);
    బఫరు_Type.tp_dealloc = బఫరు_Destructor;
    బఫరు_Type.tp_repr = బఫరు_Repr;
    బఫరు_Type.tp_as_sequence = &బఫరు_As_Seq;
    బఫరు_Type.tp_as_mapping = &బఫరు_As_Mapping;
    బఫరు_Type.tp_flags = Py_TPFLAGS_DEFAULT;
    బఫరు_Type.tp_doc = "vim buffer object";
    బఫరు_Type.tp_methods = బఫరు_Methods;
#ఒకవేళ PY_MAJOR_సంస్కరణ >= 3
    బఫరు_Type.tp_getattro = బఫరు_Getattro;
    బఫరు_Type.tp_setattro = బఫరు_Setattro;
    బఫరు_Type.tp_alloc = call_PyType_GenericAlloc;
    బఫరు_Type.tp_new = call_PyType_GenericNew;
    బఫరు_Type.tp_free = call_PyObject_Free;
#లేకపోతే
    బఫరు_Type.tp_getattr = బఫరు_Getattr;
    బఫరు_Type.tp_setattr = బఫరు_Setattr;
#అయితే_ముగింపు

    CLEAR_క్షేత్రం(కిటికీ_Type);
    కిటికీ_Type.tp_name = "vim.window";
    కిటికీ_Type.tp_basicsize = పరిమాణం(కిటికీ_Object);
    కిటికీ_Type.tp_dealloc = కిటికీ_Destructor;
    కిటికీ_Type.tp_repr = కిటికీ_Repr;
    కిటికీ_Type.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    కిటికీ_Type.tp_doc = "vim Window object";
    కిటికీ_Type.tp_methods = కిటికీ_Methods;
    కిటికీ_Type.tp_traverse = కిటికీ_Traverse;
    కిటికీ_Type.tp_clear = కిటికీ_Clear;
#ఒకవేళ PY_MAJOR_సంస్కరణ >= 3
    కిటికీ_Type.tp_getattro = కిటికీ_Getattro;
    కిటికీ_Type.tp_setattro = కిటికీ_Setattro;
    కిటికీ_Type.tp_alloc = call_PyType_GenericAlloc;
    కిటికీ_Type.tp_new = call_PyType_GenericNew;
    కిటికీ_Type.tp_free = call_PyObject_Free;
#లేకపోతే
    కిటికీ_Type.tp_getattr = కిటికీ_Getattr;
    కిటికీ_Type.tp_setattr = కిటికీ_Setattr;
#అయితే_ముగింపు

    CLEAR_క్షేత్రం(Tab_పుట_Type);
    Tab_పుట_Type.tp_name = "vim.tabpage";
    Tab_పుట_Type.tp_basicsize = పరిమాణం(Tab_పుట_Object);
    Tab_పుట_Type.tp_dealloc = Tab_పుట_Destructor;
    Tab_పుట_Type.tp_repr = Tab_పుట_Repr;
    Tab_పుట_Type.tp_flags = Py_TPFLAGS_DEFAULT;
    Tab_పుట_Type.tp_doc = "vim tab page object";
    Tab_పుట_Type.tp_methods = Tab_పుట_Methods;
#ఒకవేళ PY_MAJOR_సంస్కరణ >= 3
    Tab_పుట_Type.tp_getattro = Tab_పుట_Getattro;
    Tab_పుట_Type.tp_alloc = call_PyType_GenericAlloc;
    Tab_పుట_Type.tp_new = call_PyType_GenericNew;
    Tab_పుట_Type.tp_free = call_PyObject_Free;
#లేకపోతే
    Tab_పుట_Type.tp_getattr = Tab_పుట_Getattr;
#అయితే_ముగింపు

    CLEAR_క్షేత్రం(BufMapType);
    BufMapType.tp_name = "vim.bufferlist";
    BufMapType.tp_basicsize = పరిమాణం(BufMapObject);
    BufMapType.tp_as_mapping = &BufMapAsMapping;
    BufMapType.tp_flags = Py_TPFLAGS_DEFAULT;
    BufMapType.tp_iter = BufMapIter;
    BufMapType.tp_doc = "vim buffer list";

    CLEAR_క్షేత్రం(WinListType);
    WinListType.tp_name = "vim.windowlist";
    WinListType.tp_basicsize = పరిమాణం(WinListObject);
    WinListType.tp_as_sequence = &WinListAsSeq;
    WinListType.tp_flags = Py_TPFLAGS_DEFAULT;
    WinListType.tp_doc = "vim window list";
    WinListType.tp_dealloc = (destructor)WinListDestructor;

    CLEAR_క్షేత్రం(TabListType);
    TabListType.tp_name = "vim.tabpagelist";
    TabListType.tp_basicsize = పరిమాణం(TabListObject);
    TabListType.tp_as_sequence = &TabListAsSeq;
    TabListType.tp_flags = Py_TPFLAGS_DEFAULT;
    TabListType.tp_doc = "vim tab page list";

    CLEAR_క్షేత్రం(RangeType);
    RangeType.tp_name = "vim.range";
    RangeType.tp_basicsize = పరిమాణం(RangeObject);
    RangeType.tp_dealloc = RangeDestructor;
    RangeType.tp_repr = RangeRepr;
    RangeType.tp_as_sequence = &RangeAsSeq;
    RangeType.tp_as_mapping = &RangeAsMapping;
    RangeType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    RangeType.tp_doc = "vim Range object";
    RangeType.tp_methods = RangeMethods;
    RangeType.tp_traverse = RangeTraverse;
    RangeType.tp_clear = RangeClear;
#ఒకవేళ PY_MAJOR_సంస్కరణ >= 3
    RangeType.tp_getattro = RangeGetattro;
    RangeType.tp_alloc = call_PyType_GenericAlloc;
    RangeType.tp_new = call_PyType_GenericNew;
    RangeType.tp_free = call_PyObject_Free;
#లేకపోతే
    RangeType.tp_getattr = RangeGetattr;
#అయితే_ముగింపు

    CLEAR_క్షేత్రం(CurrentType);
    CurrentType.tp_name = "vim.currentdata";
    CurrentType.tp_basicsize = పరిమాణం(CurrentObject);
    CurrentType.tp_flags = Py_TPFLAGS_DEFAULT;
    CurrentType.tp_doc = "vim current object";
    CurrentType.tp_methods = CurrentMethods;
#ఒకవేళ PY_MAJOR_సంస్కరణ >= 3
    CurrentType.tp_getattro = CurrentGetattro;
    CurrentType.tp_setattro = CurrentSetattro;
#లేకపోతే
    CurrentType.tp_getattr = CurrentGetattr;
    CurrentType.tp_setattr = CurrentSetattr;
#అయితే_ముగింపు

    CLEAR_క్షేత్రం(DictionaryType);
    DictionaryType.tp_name = "vim.dictionary";
    DictionaryType.tp_basicsize = పరిమాణం(DictionaryObject);
    DictionaryType.tp_dealloc = DictionaryDestructor;
    DictionaryType.tp_as_sequence = &DictionaryAsSeq;
    DictionaryType.tp_as_mapping = &DictionaryAsMapping;
    DictionaryType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE;
    DictionaryType.tp_doc = "dictionary pushing modifications to Vim structure";
    DictionaryType.tp_methods = DictionaryMethods;
    DictionaryType.tp_iter = DictionaryIter;
    DictionaryType.tp_new = DictionaryConstructor;
    DictionaryType.tp_alloc = PyType_GenericAlloc;
#ఒకవేళ PY_MAJOR_సంస్కరణ >= 3
    DictionaryType.tp_getattro = DictionaryGetattro;
    DictionaryType.tp_setattro = DictionarySetattro;
#లేకపోతే
    DictionaryType.tp_getattr = DictionaryGetattr;
    DictionaryType.tp_setattr = DictionarySetattr;
#అయితే_ముగింపు

    CLEAR_క్షేత్రం(ListType);
    ListType.tp_name = "vim.list";
    ListType.tp_dealloc = ListDestructor;
    ListType.tp_basicsize = పరిమాణం(ListObject);
    ListType.tp_as_sequence = &ListAsSeq;
    ListType.tp_as_mapping = &ListAsMapping;
    ListType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE;
    ListType.tp_doc = "list pushing modifications to Vim structure";
    ListType.tp_methods = ListMethods;
    ListType.tp_iter = ListIter;
    ListType.tp_new = ListConstructor;
    ListType.tp_alloc = PyType_GenericAlloc;
#ఒకవేళ PY_MAJOR_సంస్కరణ >= 3
    ListType.tp_getattro = ListGetattro;
    ListType.tp_setattro = ListSetattro;
#లేకపోతే
    ListType.tp_getattr = ListGetattr;
    ListType.tp_setattr = ListSetattr;
#అయితే_ముగింపు

    // Tuple type
    CLEAR_క్షేత్రం(TupleType);
    TupleType.tp_name = "vim.tuple";
    TupleType.tp_dealloc = TupleDestructor;
    TupleType.tp_basicsize = పరిమాణం(TupleObject);
    TupleType.tp_as_sequence = &TupleAsSeq;
    TupleType.tp_as_mapping = &TupleAsMapping;
    TupleType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE;
    TupleType.tp_doc = "tuple pushing modifications to Vim structure";
    TupleType.tp_methods = TupleMethods;
    TupleType.tp_iter = TupleIter;
    TupleType.tp_new = TupleConstructor;
    TupleType.tp_alloc = PyType_GenericAlloc;
#ఒకవేళ PY_MAJOR_సంస్కరణ >= 3
    TupleType.tp_getattro = TupleGetattro;
    TupleType.tp_setattro = TupleSetattro;
#లేకపోతే
    TupleType.tp_getattr = TupleGetattr;
    TupleType.tp_setattr = TupleSetattr;
#అయితే_ముగింపు

    CLEAR_క్షేత్రం(FunctionType);
    FunctionType.tp_name = "vim.function";
    FunctionType.tp_basicsize = పరిమాణం(FunctionObject);
    FunctionType.tp_dealloc = FunctionDestructor;
    FunctionType.tp_call = FunctionCall;
    FunctionType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE;
    FunctionType.tp_doc = "object that calls Vim function";
    FunctionType.tp_methods = FunctionMethods;
    FunctionType.tp_repr = FunctionRepr;
    FunctionType.tp_new = FunctionConstructor;
    FunctionType.tp_alloc = PyType_GenericAlloc;
#ఒకవేళ PY_MAJOR_సంస్కరణ >= 3
    FunctionType.tp_getattro = FunctionGetattro;
#లేకపోతే
    FunctionType.tp_getattr = FunctionGetattr;
#అయితే_ముగింపు

    CLEAR_క్షేత్రం(OptionsType);
    OptionsType.tp_name = "vim.options";
    OptionsType.tp_basicsize = పరిమాణం(OptionsObject);
    OptionsType.tp_as_sequence = &OptionsAsSeq;
    OptionsType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    OptionsType.tp_doc = "object for manipulating options";
    OptionsType.tp_iter = OptionsIter;
    OptionsType.tp_as_mapping = &OptionsAsMapping;
    OptionsType.tp_dealloc = OptionsDestructor;
    OptionsType.tp_traverse = OptionsTraverse;
    OptionsType.tp_clear = OptionsClear;

#ఒకవేళ PY_సంస్కరణ_HEX < 0x030700f0
    CLEAR_క్షేత్రం(LoaderType);
    LoaderType.tp_name = "vim.Loader";
    LoaderType.tp_basicsize = పరిమాణం(LoaderObject);
    LoaderType.tp_flags = Py_TPFLAGS_DEFAULT;
    LoaderType.tp_doc = "vim message object";
    LoaderType.tp_methods = LoaderMethods;
    LoaderType.tp_dealloc = LoaderDestructor;
#అయితే_ముగింపు

#ఒకవేళ PY_MAJOR_సంస్కరణ >= 3
    CLEAR_క్షేత్రం(vimmodule);
    vimmodule.m_name = "vim";
    vimmodule.m_doc = "Vim Python interface\n";
    vimmodule.m_కొలత_2 = -1;
    vimmodule.m_methods = VimMethods;
#అయితే_ముగింపు
}

    స్థిర పూర్ణాంకం
init_types(శూన్య)
{
    PYTYPE_READY(IterType);
    PYTYPE_READY(బఫరు_Type);
    PYTYPE_READY(RangeType);
    PYTYPE_READY(కిటికీ_Type);
    PYTYPE_READY(Tab_పుట_Type);
    PYTYPE_READY(BufMapType);
    PYTYPE_READY(WinListType);
    PYTYPE_READY(TabListType);
    PYTYPE_READY(CurrentType);
    PYTYPE_READY(DictionaryType);
    PYTYPE_READY(ListType);
    PYTYPE_READY(TupleType);
    PYTYPE_READY(FunctionType);
    PYTYPE_READY(OptionsType);
    PYTYPE_READY(OutputType);
#ఒకవేళ PY_సంస్కరణ_HEX < 0x030700f0
    PYTYPE_READY(LoaderType);
#అయితే_ముగింపు

#నిర్వచితమైతే USE_LIMITED_API
    // We need to finish initializing all the static objects because the types
    // are only just allocated on the heap now.
    // Each PyObject_HEAD_INIT_TYPE should correspond to a
    // PyObject_FINISH_INIT_TYPE below.
    Py_Object_ముగింపు_INIT_TYPE(Output, OutputType);
    Py_Object_ముగింపు_INIT_TYPE(Error, OutputType);
    Py_Object_ముగింపు_INIT_TYPE(The_బఫరు_Map, BufMapType);
    Py_Object_ముగింపు_INIT_TYPE(The_కిటికీ_List, WinListType);
    Py_Object_ముగింపు_INIT_TYPE(TheCurrent, CurrentType);
    Py_Object_ముగింపు_INIT_TYPE(The_Tab_పుట_List, TabListType);
#అయితే_ముగింపు
    తిరిగి 0;
}

#నిర్వచితమైతే USE_LIMITED_API
    స్థిర శూన్య
shutdown_types(శూన్య)
{
    PYTYPE_CLEANUP(IterType);
    PYTYPE_CLEANUP(బఫరు_Type);
    PYTYPE_CLEANUP(RangeType);
    PYTYPE_CLEANUP(కిటికీ_Type);
    PYTYPE_CLEANUP(Tab_పుట_Type);
    PYTYPE_CLEANUP(BufMapType);
    PYTYPE_CLEANUP(WinListType);
    PYTYPE_CLEANUP(TabListType);
    PYTYPE_CLEANUP(CurrentType);
    PYTYPE_CLEANUP(DictionaryType);
    PYTYPE_CLEANUP(ListType);
    PYTYPE_CLEANUP(TupleType);
    PYTYPE_CLEANUP(FunctionType);
    PYTYPE_CLEANUP(OptionsType);
    PYTYPE_CLEANUP(OutputType);
# ఒకవేళ PY_సంస్కరణ_HEX < 0x030700f0
    PYTYPE_CLEANUP(LoaderType);
# endif
}
#అయితే_ముగింపు

    స్థిర పూర్ణాంకం
init_sys_మార్గం(శూన్య)
{
    PyObject	*మార్గం;
    PyObject	*మార్గం_hook;
    PyObject	*మార్గం_hooks;

    ఒకవేళ (!(మార్గం_hook = PyObject_GetAttrString(vim_module, "path_hook")))
	తిరిగి -1;

    ఒకవేళ (!(మార్గం_hooks = PySys_GetObject("path_hooks")))
    {
	PyErr_Clear();
	మార్గం_hooks = PyList_New(1);
	Py_List_సమితి_అంశం(మార్గం_hooks, 0, మార్గం_hook);
	ఒకవేళ (Py_Sys_సమితి_Object("path_hooks", మార్గం_hooks))
	{
	    Py_DECREF(మార్గం_hooks);
	    తిరిగి -1;
	}
	Py_DECREF(మార్గం_hooks);
    }
    లేకపోతే ఒకవేళ (PyList_Check(మార్గం_hooks))
    {
	ఒకవేళ (PyList_Append(మార్గం_hooks, మార్గం_hook))
	{
	    Py_DECREF(మార్గం_hook);
	    తిరిగి -1;
	}
	Py_DECREF(మార్గం_hook);
    }
    లేకపోతే
    {
	Vim_Try_ప్రారంభం();
	emsg(_("Failed to set path hook: sys.path_hooks is not a list\n"
	       "You should now do the following:\n"
	       "- append vim.path_hook to sys.path_hooks\n"
	       "- append vim.VIM_SPECIAL_PATH to sys.path\n"));
	Vim_Try_చివర(); // Discard the error
	Py_DECREF(మార్గం_hook);
	తిరిగి 0;
    }

    ఒకవేళ (!(మార్గం = PySys_GetObject("path")))
    {
	PyErr_Clear();
	మార్గం = PyList_New(1);
	Py_INCREF(vim_special_మార్గం_object);
	Py_List_సమితి_అంశం(మార్గం, 0, vim_special_మార్గం_object);
	ఒకవేళ (Py_Sys_సమితి_Object("path", మార్గం))
	{
	    Py_DECREF(మార్గం);
	    తిరిగి -1;
	}
	Py_DECREF(మార్గం);
    }
    లేకపోతే ఒకవేళ (PyList_Check(మార్గం))
    {
	ఒకవేళ (PyList_Append(మార్గం, vim_special_మార్గం_object))
	    తిరిగి -1;
    }
    లేకపోతే
    {
	Vim_Try_ప్రారంభం();
	emsg(_("Failed to set path: sys.path is not a list\n"
	       "You should now append vim.VIM_SPECIAL_PATH to sys.path"));
	Vim_Try_చివర(); // Discard the error
    }

    తిరిగి 0;
}

స్థిర నిర్మాణం numeric_constant {
    అక్షరం	*name;
    పూర్ణాంకం		val;
} numeric_constants[] = {
    {"VAR_LOCKED",	VAR_LOCKED},
    {"VAR_FIXED",	VAR_FIXED},
    {"VAR_SCOPE",	VAR_SCOPE},
    {"VAR_DEF_SCOPE",	VAR_DEF_SCOPE},
};

నిర్మాణం object_constant {
    అక్షరం	*name;
    PyObject	*valObject;
};

#నిర్వచించు కూడు_OBJECT(m, name, obj) \
    ఒకవేళ (Py_Module_కూడు_Object(m, name, obj)) \
	తిరిగి -1;

#నిర్వచించు కూడు_CHECKED_OBJECT(m, name, obj) \
    { \
	PyObject	*valObject = obj; \
	ఒకవేళ (!valObject) \
	    తిరిగి -1; \
	కూడు_OBJECT(m, name, valObject); \
    }

    స్థిర పూర్ణాంకం
populate_module(PyObject *m)
{
    పూర్ణాంకం		i;
    PyObject	*other_module;
    PyObject	*attr;
    PyObject	*imp;
#ఒకవేళ PY_సంస్కరణ_HEX >= 0x030700f0
    PyObject	*dict;
    PyObject	*cls;
#అయితే_ముగింపు

    కొరకు (i = 0; i < (పూర్ణాంకం)(పరిమాణం(numeric_constants)
					   / పరిమాణం(నిర్మాణం numeric_constant));
	    ++i)
	కూడు_CHECKED_OBJECT(m, numeric_constants[i].name,
		PyInt_FromLong(numeric_constants[i].val));

    నిర్మాణం object_constant object_constants[] = {
	{"buffers",  (PyObject *)(శూన్య *)&The_బఫరు_Map},
	{"windows",  (PyObject *)(శూన్య *)&The_కిటికీ_List},
	{"tabpages", (PyObject *)(శూన్య *)&The_Tab_పుట_List},
	{"current",  (PyObject *)(శూన్య *)&TheCurrent},

	{"Buffer",     (PyObject *)బఫరు_Type_Ptr},
	{"Range",      (PyObject *)RangeTypePtr},
	{"Window",     (PyObject *)కిటికీ_Type_Ptr},
	{"TabPage",    (PyObject *)Tab_పుట_Type_Ptr},
	{"Dictionary", (PyObject *)DictionaryTypePtr},
	{"List",       (PyObject *)ListTypePtr},
	{"Tuple",      (PyObject *)TupleTypePtr},
	{"Function",   (PyObject *)FunctionTypePtr},
	{"Options",    (PyObject *)OptionsTypePtr},
#ఒకవేళ PY_సంస్కరణ_HEX < 0x030700f0
	{"_Loader",    (PyObject *)LoaderTypePtr},
#అయితే_ముగింపు
    };

    కొరకు (i = 0; i < (పూర్ణాంకం)(పరిమాణం(object_constants)
					    / పరిమాణం(నిర్మాణం object_constant));
	    ++i)
    {
	PyObject	*valObject;

	valObject = object_constants[i].valObject;
	Py_INCREF(valObject);
	కూడు_OBJECT(m, object_constants[i].name, valObject);
    }

    ఒకవేళ (!(VimError = PyErr_NewException("vim.error", ఖాళీ, ఖాళీ)))
	తిరిగి -1;
    కూడు_OBJECT(m, "error", VimError);

    కూడు_CHECKED_OBJECT(m, "vars",  NEW_DICTIONARY(get_globvar_dict()));
    కూడు_CHECKED_OBJECT(m, "vvars", NEW_DICTIONARY(get_vimvar_dict()));
    కూడు_CHECKED_OBJECT(m, "options",
	    OptionsNew(SREQ_GLOBAL, ఖాళీ, dummy_check, ఖాళీ));

    ఒకవేళ (!(other_module = PyImport_ImportModule("os")))
	తిరిగి -1;
    కూడు_OBJECT(m, "os", other_module);

#ఒకవేళ PY_MAJOR_సంస్కరణ >= 3
    ఒకవేళ (!(py_getcwd = PyObject_GetAttrString(other_module, "getcwd")))
	తిరిగి -1;
#లేకపోతే
    ఒకవేళ (!(py_getcwd = PyObject_GetAttrString(other_module, "getcwdu")))
	తిరిగి -1;
#అయితే_ముగింపు
    కూడు_OBJECT(m, "_getcwd", py_getcwd)

    ఒకవేళ (!(py_chdir = PyObject_GetAttrString(other_module, "chdir")))
	తిరిగి -1;
    కూడు_OBJECT(m, "_chdir", py_chdir);
    ఒకవేళ (!(attr = PyObject_GetAttrString(m, "chdir")))
	తిరిగి -1;
    ఒకవేళ (Py_Object_సమితి_Attr_String(other_module, "chdir", attr))
    {
	Py_DECREF(attr);
	తిరిగి -1;
    }
    Py_DECREF(attr);

    ఒకవేళ ((py_fchdir = PyObject_GetAttrString(other_module, "fchdir")))
    {
	కూడు_OBJECT(m, "_fchdir", py_fchdir);
	ఒకవేళ (!(attr = PyObject_GetAttrString(m, "fchdir")))
	    తిరిగి -1;
	ఒకవేళ (Py_Object_సమితి_Attr_String(other_module, "fchdir", attr))
	{
	    Py_DECREF(attr);
	    తిరిగి -1;
	}
	Py_DECREF(attr);
    }
    లేకపోతే
	PyErr_Clear();

    ఒకవేళ (!(vim_special_మార్గం_object = PyString_FromString(vim_special_మార్గం)))
	తిరిగి -1;

    కూడు_OBJECT(m, "VIM_SPECIAL_PATH", vim_special_మార్గం_object);

#ఒకవేళ PY_సంస్కరణ_HEX >= 0x030700f0
    ఒకవేళ (!(imp = PyImport_ImportModule("importlib.machinery")))
	తిరిగి -1;

    dict = PyModule_GetDict(imp);

    ఒకవేళ (!(cls = Py_Dict_Get_అంశం_String(dict, "PathFinder")))
    {
	Py_DECREF(imp);
	తిరిగి -1;
    }

    ఒకవేళ (!(py_కనుగొను_spec = PyObject_GetAttrString(cls, "find_spec")))
    {
	Py_DECREF(imp);
	తిరిగి -1;
    }

# ఒకవేళ PY_సంస్కరణ_HEX < 0x30c00a7
    // find_module has been removed as of Python 3.12.0a7
    ఒకవేళ ((py_కనుగొను_module = PyObject_GetAttrString(cls, "find_module")))
	కూడు_OBJECT(m, "_find_module", py_కనుగొను_module);
# endif

    Py_DECREF(imp);

    కూడు_OBJECT(m, "_find_spec", py_కనుగొను_spec);
#లేకపోతే
    ఒకవేళ (!(imp = PyImport_ImportModule("imp")))
	తిరిగి -1;

    ఒకవేళ (!(py_కనుగొను_module = PyObject_GetAttrString(imp, "find_module")))
    {
	Py_DECREF(imp);
	తిరిగి -1;
    }

    ఒకవేళ (!(py_ఎక్కించు_module = PyObject_GetAttrString(imp, "load_module")))
    {
	Py_DECREF(py_కనుగొను_module);
	Py_DECREF(imp);
	తిరిగి -1;
    }

    Py_DECREF(imp);

    కూడు_OBJECT(m, "_find_module", py_కనుగొను_module);
    కూడు_OBJECT(m, "_load_module", py_ఎక్కించు_module);
#అయితే_ముగింపు

    తిరిగి 0;
}
