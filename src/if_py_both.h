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

#ਜੇ PY_ਸੰਸਕਰਣ_HEX < 0x02050000
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ Py_ssize_t;  // Python 2.4 and earlier don't have this type.
#ਜੇ_ਅੰਤ

// Use values that are known to work, others may make Vim crash.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ENC_OPT (enc_utf8 ? "utf-8" : enc_dbcs ? "euc-jp" : (ਅੱਖਰ *)p_enc)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DOPY_FUNC "_vim_pydo"

ਸਥਿਰ ਅਚਲ ਅੱਖਰ *vim_special_ਰਸਤਾ = "_vim_path_";

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Py_Err_ਸੈੱਟ_STRING(exc, str) Py_Err_ਸੈੱਟ_String(exc, _(str))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Py_Err_ਸੈੱਟ_Vim(str) Py_Err_ਸੈੱਟ_String(VimError, str)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Py_Err_ਸੈੱਟ_VIM(str) Py_Err_ਸੈੱਟ_STRING(VimError, str)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PyErr_FORMAT(exc, str, arg) PyErr_Format(exc, _(str), arg)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PyErr_FORMAT2(exc, str, arg1, arg2) PyErr_Format(exc, _(str), arg1,arg2)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PyErr_VIM_FORMAT(str, arg) PyErr_FORMAT(VimError, str, arg)

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_LIMITED_API
// Limited Python API. Need to call only exposed functions and remap macros.
// PyTypeObject is an opaque struct.

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    lenfunc sq_ਲੰਮਾਈ;
    binaryfunc sq_concat;
    ssizeargfunc sq_repeat;
    ssizeargfunc sq_ਵਸਤੂ;
    ਖਾਲੀ *was_sq_ਟੁਕੜਾ;
    ssizeobjargproc sq_ass_ਵਸਤੂ;
    ਖਾਲੀ *was_sq_ass_ਟੁਕੜਾ;
    objobjproc sq_ਸ਼ਾਮਲ_ਹੈ;

    binaryfunc sq_inplace_concat;
    ssizeargfunc sq_inplace_repeat;
} PySequenceMethods;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    lenfunc mp_ਲੰਮਾਈ;
    binaryfunc mp_subscript;
    objobjargproc mp_ass_subscript;
} PyMappingMethods;

// This struct emulates the concrete _typeobject struct to allow the code to
// work the same way in both limited and full Python APIs.
ਬਣਤਰ typeobject_wrapper {
    ਅਚਲ ਅੱਖਰ *tp_name;
    Py_ssize_t tp_basicsize;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ tp_flags;

    // When adding new slots below, also need to make sure we add ADD_TP_SLOT
    // call in AddHeapType for it.

    destructor tp_dealloc;
    reprfunc tp_repr;

    PySequenceMethods *tp_as_sequence;
    PyMappingMethods *tp_as_mapping;

    ternaryfunc tp_call;
    getattrofunc tp_getattro;
    setattrofunc tp_setattro;

    ਅਚਲ ਅੱਖਰ *tp_doc;

    traverseproc tp_traverse;

    inquiry tp_clear;

    getiterfunc tp_iter;
    iternextfunc tp_iternext;

    ਬਣਤਰ Py_ਵਿਧੀ_Def *tp_methods;
    ਬਣਤਰ _typeobject *tp_ਅਧਾਰ;
    allocfunc tp_alloc;
    newfunc tp_new;
    freefunc tp_free;
};

# define DEFINE_PY_TYPE_OBJECT(type) \
    ਸਥਿਰ ਬਣਤਰ typeobject_wrapper type; \
    ਸਥਿਰ PyTypeObject* type##Ptr = ਨਿਰਾ

// PyObject_HEAD_INIT_TYPE and PyObject_FINISH_INIT_TYPE need to come in pairs
// We first initialize with NULL because the type is not allocated until
// init_types() is called later. It's in FINISH_INIT_TYPE where we fill the
// type in with the newly allocated type.
# define Py_Object_ਸਿਰ_INIT_TYPE(type) Py_Object_ਸਿਰ_INIT(ਨਿਰਾ)
# define Py_Object_ਅੰਤ_INIT_TYPE(obj, type) obj.ob_ਅਧਾਰ.ob_type = type##Ptr

# define Py_TYPE_GET_TP_ALLOC(type) ((allocfunc)Py_Type_Get_ਥਾਂ(type, Py_tp_alloc))
# define Py_TYPE_GET_TP_METHODS(type) ((Py_ਵਿਧੀ_Def *)Py_Type_Get_ਥਾਂ(type, Py_tp_methods))

// PyObject_NEW is not part of stable ABI, but PyObject_Malloc/Init are.
ਸਥਿਰ PyObject* Vim_PyObject_New(PyTypeObject *type, size_t objsize)
{
    PyObject *obj = (PyObject *)PyObject_Malloc(objsize);
    ਜੇ (obj == ਨਿਰਾ)
	ਵਾਪਸ Py_Err_No_ਸਮ੍ਰਿਤੀ();
    ਵਾਪਸ PyObject_Init(obj, type);
}
# undef PyObject_NEW
# define PyObject_NEW(type, typeobj) ((type *)Vim_PyObject_New(typeobj, ਆਕਾਰ(type)))

// This is a somewhat convoluted because limited API doesn't expose an easy way
// to get the tp_name field, and so we have to manually reconstruct it as
// "__module__.__name__" (with __module__ omitted for builtins to emulate
// Python behavior). Also, some of the more convenient functions like
// PyUnicode_AsUTF8AndSize and PyType_GetQualName() are not available until
// late Python 3 versions, and won't be available if you set Py_LIMITED_API too
// low.
# define PyErr_FORMAT_TYPE(ਸੁਨੇਹਾ, obj) \
    ਕਰੋ { \
	PyObject* qualname = PyObject_GetAttrString((PyObject*)(obj)->ob_type, "__qualname__"); \
	ਜੇ (qualname == ਨਿਰਾ) \
	{ \
	    PyErr_FORMAT(PyExc_TypeError, ਸੁਨੇਹਾ, "(NULL)"); \
	    ਰੋਕੋ; \
	} \
	PyObject* module = PyObject_GetAttrString((PyObject*)(obj)->ob_type, "__module__"); \
	PyObject* ਭਰਿਆ; \
	ਜੇ (module == ਨਿਰਾ || PyUnicode_CompareWithASCIIString(module, "builtins") == 0 \
		|| PyUnicode_CompareWithASCIIString(module, "__main__") == 0) \
	{ \
	    ਭਰਿਆ = qualname; \
	    Py_INCREF(ਭਰਿਆ); \
	} \
	ਨਹੀਂਤਾਂ \
	    ਭਰਿਆ = PyUnicode_FromFormat("%U.%U", module, qualname); \
	PyObject* ਭਰਿਆ_ਬਾਈਟਾਂ = PyUnicode_AsUTF8String(ਭਰਿਆ); \
	ਅਚਲ ਅੱਖਰ* ਭਰਿਆ_str = Py_ਬਾਈਟਾਂ_As_String(ਭਰਿਆ_ਬਾਈਟਾਂ); \
	ਭਰਿਆ_str = ਭਰਿਆ_str == ਨਿਰਾ ? "(NULL)" : ਭਰਿਆ_str; \
	PyErr_FORMAT(PyExc_TypeError, ਸੁਨੇਹਾ, ਭਰਿਆ_str); \
	Py_DECREF(qualname); \
	Py_XDECREF(module); \
	Py_XDECREF(ਭਰਿਆ); \
	Py_XDECREF(ਭਰਿਆ_ਬਾਈਟਾਂ); \
    } ਜਦੋਂ (0)

# define Py_List_GET_ਵਸਤੂ(list, i) Py_List_Get_ਵਸਤੂ(list, i)
# define Py_List_GET_ਮਾਪ(o) Py_List_ਮਾਪ(o)

// PyList_SET_ITEM and PyList_SetItem have slightly different behaviors. The
// former will leave the old item dangling, and the latter will decref on it.
// Since we only use this on new lists, this difference doesn't matter.
# define Py_List_ਸੈੱਟ_ਵਸਤੂ(list, i, ਵਸਤੂ) Py_List_ਸੈੱਟ_ਵਸਤੂ_2(list, i, ਵਸਤੂ)

# define Py_Tuple_GET_ਵਸਤੂ(o, pos) Py_Tuple_Get_ਵਸਤੂ(o, pos)
# define Py_Tuple_GET_ਮਾਪ(o) Py_Tuple_ਮਾਪ(o)

# ਜੇ Py_LIMITED_API < 0x03080000
// PyIter_check only became part of stable ABI in 3.8, and there is no easy way
// to check for it in the API. We simply return false as a compromise. This
// does mean we should avoid compiling with stable ABI < 3.8.
#  undef PyIter_Check
#  define PyIter_Check(obj) (FALSE)
# endif

ਸਥਿਰ PyTypeObject* ਜੋੜੋ_ਢੇਰ_Type(ਬਣਤਰ typeobject_wrapper* type_object)
{
    PyType_Spec type_spec;
    type_spec.name = type_object->tp_name;
    type_spec.basicsize = type_object->tp_basicsize;
    type_spec.itemsize = 0;
    type_spec.flags = type_object->tp_flags;

    // We just need to statically allocate a large enough buffer that can hold
    // all slots. We need to leave a null-terminated slot at the end.
    Py_Type_ਥਾਂ slots[40] = { {0, ਨਿਰਾ} };
    size_t ਥਾਂ_i = 0;

# define ਜੋੜੋ_TP_ਥਾਂ(ਥਾਂ_name) \
    ਜੇ (ਥਾਂ_i >= 40) ਵਾਪਸ ਨਿਰਾ; /* this should never happen */ \
    ਜੇ (type_object->ਥਾਂ_name != ਨਿਰਾ) \
    { \
	slots[ਥਾਂ_i].ਥਾਂ = Py_##slot_name; \
	slots[ਥਾਂ_i].pfunc = (ਖਾਲੀ*)type_object->ਥਾਂ_name; \
	++ਥਾਂ_i; \
    }
# define ਜੋੜੋ_TP_SUB_ਥਾਂ(sub_ਥਾਂ, ਥਾਂ_name) \
    ਜੇ (ਥਾਂ_i >= 40) ਵਾਪਸ ਨਿਰਾ; /* this should never happen */ \
    ਜੇ (type_object->sub_ਥਾਂ != ਨਿਰਾ && type_object->sub_ਥਾਂ->ਥਾਂ_name != ਨਿਰਾ) \
    { \
	slots[ਥਾਂ_i].ਥਾਂ = Py_##slot_name; \
	slots[ਥਾਂ_i].pfunc = (ਖਾਲੀ*)type_object->sub_ਥਾਂ->ਥਾਂ_name; \
	++ਥਾਂ_i; \
    }

    ਜੋੜੋ_TP_ਥਾਂ(tp_dealloc)
    ਜੋੜੋ_TP_ਥਾਂ(tp_repr)
    ਜੋੜੋ_TP_ਥਾਂ(tp_call)
    ਜੋੜੋ_TP_ਥਾਂ(tp_getattro)
    ਜੋੜੋ_TP_ਥਾਂ(tp_setattro)
    ਜੋੜੋ_TP_ਥਾਂ(tp_doc)
    ਜੋੜੋ_TP_ਥਾਂ(tp_traverse)
    ਜੋੜੋ_TP_ਥਾਂ(tp_clear)
    ਜੋੜੋ_TP_ਥਾਂ(tp_iter)
    ਜੋੜੋ_TP_ਥਾਂ(tp_iternext)
    ਜੋੜੋ_TP_ਥਾਂ(tp_methods)
    ਜੋੜੋ_TP_ਥਾਂ(tp_ਅਧਾਰ)
    ਜੋੜੋ_TP_ਥਾਂ(tp_alloc)
    ਜੋੜੋ_TP_ਥਾਂ(tp_new)
    ਜੋੜੋ_TP_ਥਾਂ(tp_free)

    ਜੋੜੋ_TP_SUB_ਥਾਂ(tp_as_sequence, sq_ਲੰਮਾਈ)
    ਜੋੜੋ_TP_SUB_ਥਾਂ(tp_as_sequence, sq_concat)
    ਜੋੜੋ_TP_SUB_ਥਾਂ(tp_as_sequence, sq_repeat)
    ਜੋੜੋ_TP_SUB_ਥਾਂ(tp_as_sequence, sq_ਵਸਤੂ)
    ਜੋੜੋ_TP_SUB_ਥਾਂ(tp_as_sequence, sq_ass_ਵਸਤੂ)
    ਜੋੜੋ_TP_SUB_ਥਾਂ(tp_as_sequence, sq_ਸ਼ਾਮਲ_ਹੈ)
    ਜੋੜੋ_TP_SUB_ਥਾਂ(tp_as_sequence, sq_inplace_concat)
    ਜੋੜੋ_TP_SUB_ਥਾਂ(tp_as_sequence, sq_inplace_repeat)

    ਜੋੜੋ_TP_SUB_ਥਾਂ(tp_as_mapping, mp_ਲੰਮਾਈ)
    ਜੋੜੋ_TP_SUB_ਥਾਂ(tp_as_mapping, mp_subscript)
    ਜੋੜੋ_TP_SUB_ਥਾਂ(tp_as_mapping, mp_ass_subscript)
# undef ਜੋੜੋ_TP_ਥਾਂ
# undef ਜੋੜੋ_TP_SUB_ਥਾਂ

    type_spec.slots = slots;

    PyObject* newtype = PyType_FromSpec(&type_spec);
    ਵਾਪਸ (PyTypeObject*)newtype;
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
    type##Ptr = ਜੋੜੋ_ਢੇਰ_Type(&(type)); \
    ਜੇ (type##Ptr == ਨਿਰਾ) \
	ਵਾਪਸ -1;
# define PYTYPE_CLEANUP(type) \
    type##Ptr = ਨਿਰਾ;

// Limited API does not provide PyRun_* functions. Need to implement manually
// using PyCompile and PyEval.
ਸਥਿਰ PyObject* Vim_Py_ਲੜੀਵਾਰ_String(ਅਚਲ ਅੱਖਰ *str, ਪੂਰਨ ਸ਼ੁਰੂ, PyObject *globals, PyObject *locals)
{
    // Just pass "" for filename for now.
    PyObject* compiled = Py_CompileString(str, "", ਸ਼ੁਰੂ);
    ਜੇ (compiled == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    PyObject* eval_ਨਤੀਜਾ = Py_Eval_Eval_ਕੋਡ(compiled, globals, locals);
    Py_DECREF(compiled);
    ਵਾਪਸ eval_ਨਤੀਜਾ;
}
ਸਥਿਰ ਪੂਰਨ Vim_Py_ਲੜੀਵਾਰ_Simple_String(ਅਚਲ ਅੱਖਰ *str)
{
    // This function emulates CPython's implementation.
    PyObject* m = Py_Import_ਜੋੜੋ_Module("__main__");
    ਜੇ (m == ਨਿਰਾ)
	ਵਾਪਸ -1;
    PyObject* d = PyModule_GetDict(m);
    PyObject* output = Vim_Py_ਲੜੀਵਾਰ_String(str, Py_file_ਨਿਵੇਸ਼, d, d);
    ਜੇ (output == ਨਿਰਾ)
    {
	PyErr_PrintEx(TRUE);
	ਵਾਪਸ -1;
    }
    Py_DECREF(output);
    ਵਾਪਸ 0;
}
# define Py_ਲੜੀਵਾਰ_String Vim_Py_ਲੜੀਵਾਰ_String
# define Py_ਲੜੀਵਾਰ_Simple_String Vim_Py_ਲੜੀਵਾਰ_Simple_String

#ਨਹੀਂਤਾਂ // !defined(USE_LIMITED_API)

// Full Python API. Can make use of structs and macros directly.
# define DEFINE_PY_TYPE_OBJECT(type) \
    ਸਥਿਰ PyTypeObject type; \
    ਸਥਿਰ PyTypeObject* type##Ptr = &type
# define Py_Object_ਸਿਰ_INIT_TYPE(type) Py_Object_ਸਿਰ_INIT(&type)

# define Py_TYPE_GET_TP_ALLOC(type) type->tp_alloc
# define Py_TYPE_GET_TP_METHODS(type) type->tp_methods

# define Py_TYPE_NAME(obj) ((obj)->ob_type->tp_name == ਨਿਰਾ \
	? "(NULL)" \
	: (obj)->ob_type->tp_name)
# define PyErr_FORMAT_TYPE(ਸੁਨੇਹਾ, obj) \
    PyErr_FORMAT(PyExc_TypeError, ਸੁਨੇਹਾ, \
	    Py_TYPE_NAME(obj))

// Add a static type
# define PYTYPE_READY(type) \
    ਜੇ (PyType_Ready(type##Ptr)) \
	ਵਾਪਸ -1;

#ਜੇ_ਅੰਤ


#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RAISE_NO_ਖਾਲੀ_ਥਾਵਾਂ_ਕੁੰਜੀਆਂ Py_Err_ਸੈੱਟ_STRING(Py_Exc_ਮੁੱਲ_Error, \
					    N_("empty keys are not allowed"))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RAISE_LOCKED_DICTIONARY Py_Err_ਸੈੱਟ_VIM(N_("dictionary is locked"))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RAISE_LOCKED_LIST Py_Err_ਸੈੱਟ_VIM(N_("list is locked"))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RAISE_UNDO_FAIL Py_Err_ਸੈੱਟ_VIM(N_("cannot save undo information"))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RAISE_DELETE_ਸਤਰ_FAIL Py_Err_ਸੈੱਟ_VIM(N_("cannot delete line"))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RAISE_ਪਾਓ_ਸਤਰ_FAIL Py_Err_ਸੈੱਟ_VIM(N_("cannot insert line"))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RAISE_REPLACE_ਸਤਰ_FAIL Py_Err_ਸੈੱਟ_VIM(N_("cannot replace line"))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RAISE_ਕੁੰਜੀ_ਜੋੜੋ_FAIL(ਕੁੰਜੀ) \
    PyErr_VIM_FORMAT(N_("failed to add key '%s' to dictionary"), ਕੁੰਜੀ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RAISE_INVALID_ਸੂਚਕਾਂਕ_TYPE(idx) \
    PyErr_FORMAT_TYPE(N_("index must be int or slice, not %s"), idx);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INVALID_ਬਫਰ_ਮੁੱਲ ((buf_T *)(-1))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INVALID_ਖਿੜਕੀ_ਮੁੱਲ ((win_T *)(-1))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INVALID_TABPAGE_ਮੁੱਲ ((tabpage_T *)(-1))

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ (*rangeinitializer)(ਖਾਲੀ *);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ (*runner)(ਅਚਲ ਅੱਖਰ *, dict_T *, ਖਾਲੀ *
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ PY_CAN_RECURSE
	, PyGILState_STATE *
#ਜੇ_ਅੰਤ
	);

ਸਥਿਰ ਪੂਰਨ ConvertFromPyObject(PyObject *, typval_T *);
ਸਥਿਰ ਪੂਰਨ _ConvertFromPyObject(PyObject *, typval_T *, PyObject *);
ਸਥਿਰ ਪੂਰਨ ConvertFromPyMapping(PyObject *, typval_T *);
ਸਥਿਰ ਪੂਰਨ ConvertFromPySequence(PyObject *, typval_T *);
ਸਥਿਰ PyObject *ਖਿੜਕੀ_New(win_T *, tabpage_T *);
ਸਥਿਰ PyObject *ਬਫਰ_New (buf_T *);
ਸਥਿਰ PyObject *ਸਤਰ_ਮੰਜ਼ਿਲ_String(ਅਚਲ ਅੱਖਰ *);

ਸਥਿਰ PyInt Range_ਸ਼ੁਰੂ;
ਸਥਿਰ PyInt Range_ਸਿਰਾ;

ਸਥਿਰ PyObject *globals;

ਸਥਿਰ PyObject *py_chdir;
ਸਥਿਰ PyObject *py_fchdir;
ਸਥਿਰ PyObject *py_getcwd;
ਸਥਿਰ PyObject *vim_module;
ਸਥਿਰ PyObject *vim_special_ਰਸਤਾ_object;

#ਜੇ PY_ਸੰਸਕਰਣ_HEX >= 0x030700f0
ਸਥਿਰ PyObject *py_ਲੱਭੋ_spec;
#ਨਹੀਂਤਾਂ
ਸਥਿਰ PyObject *py_ਲੱਦੋ_module;
#ਜੇ_ਅੰਤ
#ਜੇ PY_ਸੰਸਕਰਣ_HEX < 0x30c00a7
ਸਥਿਰ PyObject *py_ਲੱਭੋ_module;
#ਜੇ_ਅੰਤ

ਸਥਿਰ PyObject *VimError;

/*
 * obtain a lock on the Vim data structures
 */
    ਸਥਿਰ ਖਾਲੀ
Python_ਤਾਲਾ_Vim(ਖਾਲੀ)
{
}

/*
 * release a lock on the Vim data structures
 */
    ਸਥਿਰ ਖਾਲੀ
Python_Release_Vim(ਖਾਲੀ)
{
}

/*
 * The "todecref" argument holds a pointer to PyObject * that must be
 * DECREF'ed after returned char_u * is no longer needed or NULL if all what
 * was needed to generate returned value is object.
 *
 * Use Py_XDECREF to decrement reference count.
 */
    ਸਥਿਰ char_u *
String_ਮੰਜ਼ਿਲ_Chars(PyObject *obj, PyObject **todecref)
{
    char_u	*str;

    ਜੇ (Py_ਬਾਈਟਾਂ_Check(obj))
    {

	ਜੇ (Py_ਬਾਈਟਾਂ_As_String_And_ਮਾਪ(obj, (ਅੱਖਰ **) &str, ਨਿਰਾ) == -1
		|| str == ਨਿਰਾ)
	    ਵਾਪਸ ਨਿਰਾ;

	*todecref = ਨਿਰਾ;
    }
    ਨਹੀਂਤਾਂ ਜੇ (PyUnicode_Check(obj))
    {
	PyObject	*ਬਾਈਟਾਂ;

	ਜੇ (!(ਬਾਈਟਾਂ = PyUnicode_AsEncodedString(obj, ENC_OPT,
							   ERRORS_ENCODE_ARG)))
	    ਵਾਪਸ ਨਿਰਾ;

	ਜੇ (Py_ਬਾਈਟਾਂ_As_String_And_ਮਾਪ(ਬਾਈਟਾਂ, (ਅੱਖਰ **) &str, ਨਿਰਾ) == -1
		|| str == ਨਿਰਾ)
	{
	    Py_DECREF(ਬਾਈਟਾਂ);
	    ਵਾਪਸ ਨਿਰਾ;
	}

	*todecref = ਬਾਈਟਾਂ;
    }
    ਨਹੀਂਤਾਂ
    {
#ਜੇ PY_MAJOR_ਸੰਸਕਰਣ < 3
	PyErr_FORMAT_TYPE(N_("expected str() or unicode() instance, but got %s"),
		obj);
#ਨਹੀਂਤਾਂ
	PyErr_FORMAT_TYPE(N_("expected bytes() or str() instance, but got %s"),
		obj);
#ਜੇ_ਅੰਤ
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਵਾਪਸ (char_u *) str;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੰਖਿਆ_LONG     1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੰਖਿਆ_INT      2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੰਖਿਆ_NATURAL  4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੰਖਿਆ_UNSIGNED 8

    ਸਥਿਰ ਪੂਰਨ
ਸੰਖਿਆ_ਮੰਜ਼ਿਲ_Long(PyObject *obj, ਲੰਮਾ *ਨਤੀਜਾ, ਪੂਰਨ flags)
{
#ਜੇ PY_MAJOR_ਸੰਸਕਰਣ < 3
    ਜੇ (PyInt_Check(obj))
    {
	*ਨਤੀਜਾ = PyInt_AsLong(obj);
	ਜੇ (PyErr_Occurred())
	    ਵਾਪਸ -1;
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    ਜੇ (PyLong_Check(obj))
    {
	*ਨਤੀਜਾ = PyLong_AsLong(obj);
	ਜੇ (PyErr_Occurred())
	    ਵਾਪਸ -1;
    }
    ਨਹੀਂਤਾਂ ਜੇ (Py_ਸੰਖਿਆ_Check(obj))
    {
	PyObject	*ਅੰਕ;

	ਜੇ (!(ਅੰਕ = Py_ਸੰਖਿਆ_Long(obj)))
	    ਵਾਪਸ -1;

	*ਨਤੀਜਾ = PyLong_AsLong(ਅੰਕ);

	Py_DECREF(ਅੰਕ);

	ਜੇ (PyErr_Occurred())
	    ਵਾਪਸ -1;
    }
    ਨਹੀਂਤਾਂ
    {
#ਜੇ PY_MAJOR_ਸੰਸਕਰਣ < 3
	PyErr_FORMAT_TYPE(
		N_("expected int(), long() or something supporting "
		   "coercing to long(), but got %s"),
		obj);
#ਨਹੀਂਤਾਂ
	PyErr_FORMAT_TYPE(
		N_("expected int() or something supporting coercing to int(), "
		   "but got %s"),
		obj);
#ਜੇ_ਅੰਤ
	ਵਾਪਸ -1;
    }

    ਜੇ (flags & ਸੰਖਿਆ_INT)
    {
	ਜੇ (*ਨਤੀਜਾ > ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ)
	{
	    Py_Err_ਸੈੱਟ_STRING(Py_Exc_ਉਛਾਲ_Error,
		    N_("value is too large to fit into C int type"));
	    ਵਾਪਸ -1;
	}
	ਨਹੀਂਤਾਂ ਜੇ (*ਨਤੀਜਾ < INT_MIN)
	{
	    Py_Err_ਸੈੱਟ_STRING(Py_Exc_ਉਛਾਲ_Error,
		    N_("value is too small to fit into C int type"));
	    ਵਾਪਸ -1;
	}
    }

    ਜੇ (flags & ਸੰਖਿਆ_NATURAL)
    {
	ਜੇ (*ਨਤੀਜਾ <= 0)
	{
	    Py_Err_ਸੈੱਟ_STRING(Py_Exc_ਮੁੱਲ_Error,
		    N_("number must be greater than zero"));
	    ਵਾਪਸ -1;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (flags & ਸੰਖਿਆ_UNSIGNED)
    {
	ਜੇ (*ਨਤੀਜਾ < 0)
	{
	    Py_Err_ਸੈੱਟ_STRING(Py_Exc_ਮੁੱਲ_Error,
		    N_("number must be greater or equal to zero"));
	    ਵਾਪਸ -1;
	}
    }

    ਵਾਪਸ 0;
}

    ਸਥਿਰ ਪੂਰਨ
ਜੋੜੋ_string(PyObject *list, ਅੱਖਰ *s)
{
    PyObject	*string;

    ਜੇ (!(string = PyString_FromString(s)))
	ਵਾਪਸ -1;

    ਜੇ (PyList_Append(list, string))
    {
	Py_DECREF(string);
	ਵਾਪਸ -1;
    }

    Py_DECREF(string);
    ਵਾਪਸ 0;
}

    ਸਥਿਰ PyObject *
ObjectDir(PyObject *self, ਅੱਖਰ **ਗੁਣ)
{
    Py_ਵਿਧੀ_Def	*ਵਿਧੀ;
    ਅੱਖਰ	**attr;
    PyObject	*ret;

    ਜੇ (!(ret = PyList_New(0)))
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (self)
	ਲਈ (ਵਿਧੀ = Py_TYPE_GET_TP_METHODS(self->ob_type) ; ਵਿਧੀ->ml_name != ਨਿਰਾ ; ++ਵਿਧੀ)
	    ਜੇ (ਜੋੜੋ_string(ret, (ਅੱਖਰ *)ਵਿਧੀ->ml_name))
	    {
		Py_DECREF(ret);
		ਵਾਪਸ ਨਿਰਾ;
	    }

    ਲਈ (attr = ਗੁਣ ; *attr ; ++attr)
	ਜੇ (ਜੋੜੋ_string(ret, *attr))
	{
	    Py_DECREF(ret);
	    ਵਾਪਸ ਨਿਰਾ;
	}

#ਜੇ PY_MAJOR_ਸੰਸਕਰਣ < 3
    ਜੇ (ਜੋੜੋ_string(ret, "__members__"))
    {
	Py_DECREF(ret);
	ਵਾਪਸ ਨਿਰਾ;
    }
#ਜੇ_ਅੰਤ

    ਵਾਪਸ ret;
}

// Output buffer management

// Function to write a line, points to either msg() or emsg().
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*writefn)(ਅੱਖਰ *);

DEFINE_PY_TYPE_OBJECT(OutputType);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    Py_Object_ਸਿਰ
    ਲੰਮਾ softspace;
    ਲੰਮਾ error;
} OutputObject;

ਸਥਿਰ ਅੱਖਰ *OutputAttrs[] = {
    "softspace",
    ਨਿਰਾ
};

    ਸਥਿਰ PyObject *
OutputDir(PyObject *self, PyObject *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ ObjectDir(self, OutputAttrs);
}

    ਸਥਿਰ ਪੂਰਨ
OutputSetattr(PyObject *self_obj, ਅੱਖਰ *name, PyObject *valObject)
{
    OutputObject *self = (OutputObject*)self_obj;
    ਜੇ (valObject == ਨਿਰਾ)
    {
	Py_Err_ਸੈੱਟ_STRING(PyExc_AttributeError,
		N_("can't delete OutputObject attributes"));
	ਵਾਪਸ -1;
    }

    ਜੇ (strcmp(name, "softspace") == 0)
    {
	ਜੇ (ਸੰਖਿਆ_ਮੰਜ਼ਿਲ_Long(valObject, &(self->softspace), ਸੰਖਿਆ_UNSIGNED))
	    ਵਾਪਸ -1;
	ਵਾਪਸ 0;
    }

    PyErr_FORMAT(PyExc_AttributeError, N_("invalid attribute: %s"), name);
    ਵਾਪਸ -1;
}

// Buffer IO, we write one whole line at a time.
ਸਥਿਰ garray_T io_ga = {0, 0, 1, 80, ਨਿਰਾ};
ਸਥਿਰ writefn old_fn = ਨਿਰਾ;

    ਸਥਿਰ ਖਾਲੀ
PythonIO_Flush(ਖਾਲੀ)
{
    ਜੇ (old_fn != ਨਿਰਾ && io_ga.ga_len > 0)
    {
	((ਅੱਖਰ *)io_ga.ga_ਅੰਕੜੇ)[io_ga.ga_len] = NUL;
	// We don't know what emsg_severe should be here, so ... hope?
	old_fn((ਅੱਖਰ *)io_ga.ga_ਅੰਕੜੇ);
    }
    io_ga.ga_len = 0;
}

    ਸਥਿਰ ਖਾਲੀ
writer(writefn fn, char_u *str, PyInt n, ਪੂਰਨ severe)
{
    char_u *ptr;

    // Flush when switching output function.
    ਜੇ (fn != old_fn)
	PythonIO_Flush();
    old_fn = fn;

    // Write each NL separated line.  Text after the last NL is kept for
    // writing later.
    // For normal messages: Do not output when "got_int" was set.  This avoids
    // a loop gone crazy flooding the terminal with messages.  Also for when
    // "q" is pressed at the more-prompt.
    ਜਦੋਂ (n > 0 && (ptr = memchr(str, '\n', n)) != ਨਿਰਾ
					  && (fn == (writefn)emsg || !got_int))
    {
	PyInt len = ptr - str;

	ਜੇ (ga_grow(&io_ga, (ਪੂਰਨ)(len + 1)) == FAIL)
	    ਰੋਕੋ;

	mch_memmove(((ਅੱਖਰ *)io_ga.ga_ਅੰਕੜੇ) + io_ga.ga_len, str, (size_t)len);
	((ਅੱਖਰ *)io_ga.ga_ਅੰਕੜੇ)[io_ga.ga_len + len] = NUL;
	emsg_severe = severe;
	fn((ਅੱਖਰ *)io_ga.ga_ਅੰਕੜੇ);
	str = ptr + 1;
	n -= len + 1;
	io_ga.ga_len = 0;
    }

    // Put the remaining text into io_ga for later printing.
    ਜੇ (n > 0 && (fn == (writefn)emsg || !got_int)
					&& ga_grow(&io_ga, (ਪੂਰਨ)(n + 1)) == OK)
    {
	mch_memmove(((ਅੱਖਰ *)io_ga.ga_ਅੰਕੜੇ) + io_ga.ga_len, str, (size_t)n);
	io_ga.ga_len += (ਪੂਰਨ)n;
    }
}

    ਸਥਿਰ ਪੂਰਨ
write_output(OutputObject *self, PyObject *string)
{
    Py_ssize_t	len = 0;
    ਅੱਖਰ	*str = ਨਿਰਾ;
    ਪੂਰਨ		error = self->error;

    ਜੇ (!PyArg_Parse(string, "et#", ENC_OPT, &str, &len))
	ਵਾਪਸ -1;

    Py_BEGIN_ALLOW_ਧਾਗੇ
    Python_ਤਾਲਾ_Vim();
    writer((writefn)(error ? emsg : ਸੁਨੇਹਾ), (char_u *)str, len, error);
    Python_Release_Vim();
    Py_ਸਿਰਾ_ALLOW_ਧਾਗੇ
    PyMem_Free(str);

    ਵਾਪਸ 0;
}

    ਸਥਿਰ PyObject *
OutputWrite(OutputObject *self, PyObject *string)
{
    ਜੇ (write_output(self, string))
	ਵਾਪਸ ਨਿਰਾ;

    Py_INCREF(Py_None);
    ਵਾਪਸ Py_None;
}

    ਸਥਿਰ PyObject *
OutputWritelines(OutputObject *self, PyObject *seq)
{
    PyObject	*iterator;
    PyObject	*ਵਸਤੂ;

    ਜੇ (!(iterator = PyObject_GetIter(seq)))
	ਵਾਪਸ ਨਿਰਾ;

    ਜਦੋਂ ((ਵਸਤੂ = Py_Iter_ਅਗਲਾ(iterator)))
    {
	ਜੇ (write_output(self, ਵਸਤੂ))
	{
	    Py_DECREF(iterator);
	    Py_DECREF(ਵਸਤੂ);
	    ਵਾਪਸ ਨਿਰਾ;
	}
	Py_DECREF(ਵਸਤੂ);
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    ਜੇ (PyErr_Occurred())
	ਵਾਪਸ ਨਿਰਾ;

    Py_INCREF(Py_None);
    ਵਾਪਸ Py_None;
}

    ਸਥਿਰ PyObject *
AlwaysNone(PyObject *self ਅਣਵਰਤਿਆ, PyObject *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    // do nothing
    Py_INCREF(Py_None);
    ਵਾਪਸ Py_None;
}
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ALWAYS_NONE AlwaysNone(ਨਿਰਾ, ਨਿਰਾ)

    ਸਥਿਰ PyObject *
AlwaysFalse(PyObject *self ਅਣਵਰਤਿਆ, PyObject *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    // do nothing
    PyObject	*ret = Py_False;
    Py_INCREF(ret);
    ਵਾਪਸ ret;
}
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ALWAYS_FALSE AlwaysFalse(ਨਿਰਾ, ਨਿਰਾ)

    ਸਥਿਰ PyObject *
AlwaysTrue(PyObject *self ਅਣਵਰਤਿਆ, PyObject *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    // do nothing
    PyObject	*ret = Py_True;
    Py_INCREF(ret);
    ਵਾਪਸ ret;
}
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ALWAYS_TRUE AlwaysTrue(ਨਿਰਾ, ਨਿਰਾ)

/***************/

ਸਥਿਰ ਬਣਤਰ Py_ਵਿਧੀ_Def OutputMethods[] = {
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
    { ਨਿਰਾ,	    ਨਿਰਾ,				0,		ਨਿਰਾ}
};

ਸਥਿਰ OutputObject Output =
{
    Py_Object_ਸਿਰ_INIT_TYPE(OutputType)
    0,
    0
};

ਸਥਿਰ OutputObject Error =
{
    Py_Object_ਸਿਰ_INIT_TYPE(OutputType)
    0,
    1
};

    ਸਥਿਰ ਪੂਰਨ
PythonIO_Init_io(ਖਾਲੀ)
{
    ਜੇ (Py_Sys_ਸੈੱਟ_Object("stdout", (PyObject *)(ਖਾਲੀ *)&Output))
	ਵਾਪਸ -1;
    ਜੇ (Py_Sys_ਸੈੱਟ_Object("stderr", (PyObject *)(ਖਾਲੀ *)&Error))
	ਵਾਪਸ -1;

    ਜੇ (PyErr_Occurred())
    {
	emsg(_(e_python_error_initialising_io_object));
	ਵਾਪਸ -1;
    }

    ਵਾਪਸ 0;
}

#ਜੇ PY_ਸੰਸਕਰਣ_HEX < 0x030700f0
ਸਥਿਰ PyObject *call_ਲੱਦੋ_module(ਅੱਖਰ *name, ਪੂਰਨ len, PyObject *ਲੱਭੋ_module_ਨਤੀਜਾ);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    Py_Object_ਸਿਰ
    ਅੱਖਰ	*fullname;
    PyObject	*ਨਤੀਜਾ;
} LoaderObject;
DEFINE_PY_TYPE_OBJECT(LoaderType);

    ਸਥਿਰ ਖਾਲੀ
LoaderDestructor(PyObject *self_obj)
{
    LoaderObject *self = (LoaderObject*)self_obj;
    vim_free(self->fullname);
    Py_XDECREF(self->ਨਤੀਜਾ);
    DESTRUCTOR_ਅੰਤ(self);
}

    ਸਥਿਰ PyObject *
Loader_ਲੱਦੋ_Module(LoaderObject *self, PyObject *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਅੱਖਰ	*fullname = self->fullname;
    PyObject	*ਨਤੀਜਾ = self->ਨਤੀਜਾ;
    PyObject	*module;

    ਜੇ (!fullname)
    {
	module = ਨਤੀਜਾ ? ਨਤੀਜਾ : Py_None;
	Py_INCREF(module);
	ਵਾਪਸ module;
    }

    module = call_ਲੱਦੋ_module(fullname, (ਪੂਰਨ)STRLEN(fullname), ਨਤੀਜਾ);

    self->fullname = ਨਿਰਾ;
    self->ਨਤੀਜਾ = module;

    vim_free(fullname);
    Py_DECREF(ਨਤੀਜਾ);

    ਜੇ (!module)
    {
	ਜੇ (PyErr_Occurred())
	    ਵਾਪਸ ਨਿਰਾ;

	Py_INCREF(Py_None);
	ਵਾਪਸ Py_None;
    }

    Py_INCREF(module);
    ਵਾਪਸ module;
}

ਸਥਿਰ ਬਣਤਰ Py_ਵਿਧੀ_Def LoaderMethods[] = {
    // name,	    function,				calling,	doc
    {"load_module", (PyCFunction)Loader_ਲੱਦੋ_Module,	METH_VARARGS,	""},
    { ਨਿਰਾ,	    ਨਿਰਾ,				0,		ਨਿਰਾ}
};
#ਜੇ_ਅੰਤ

/*
 * Check to see whether a Vim error has been reported, or a keyboard
 * interrupt has been detected.
 */
    ਸਥਿਰ ਖਾਲੀ
Vim_Try_ਸ਼ੁਰੂ(ਖਾਲੀ)
{
    ++trylevel;
}

    ਸਥਿਰ ਪੂਰਨ
Vim_Try_ਸਿਰਾ(ਖਾਲੀ)
{
    --trylevel;
    // Without this it stops processing all subsequent Vim script commands and
    // generates strange error messages if I e.g. try calling Test() in a cycle
    did_emsg = FALSE;
    // Keyboard interrupt should be preferred over anything else
    ਜੇ (got_int)
    {
	ਜੇ (did_throw)
	    discard_current_exception();
	got_int = FALSE;
	Py_Err_ਸੈੱਟ_None(Py_Exc_Keyboard_ਰੁਕਾਵਟ);
	ਵਾਪਸ -1;
    }
    ਨਹੀਂਤਾਂ ਜੇ (ਸੁਨੇਹਾ_list != ਨਿਰਾ && *ਸੁਨੇਹਾ_list != ਨਿਰਾ)
    {
	ਪੂਰਨ	should_free;
	ਅੱਖਰ	*ਸੁਨੇਹਾ;

	ਸੁਨੇਹਾ = get_exception_string(*ਸੁਨੇਹਾ_list, ET_ERROR, ਨਿਰਾ, &should_free);

	ਜੇ (ਸੁਨੇਹਾ == ਨਿਰਾ)
	{
	    Py_Err_No_ਸਮ੍ਰਿਤੀ();
	    ਵਾਪਸ -1;
	}

	Py_Err_ਸੈੱਟ_Vim(ਸੁਨੇਹਾ);

	free_global_msglist();

	ਜੇ (should_free)
	    vim_free(ਸੁਨੇਹਾ);

	ਵਾਪਸ -1;
    }
    ਨਹੀਂਤਾਂ ਜੇ (!did_throw)
	ਵਾਪਸ (PyErr_Occurred() ? -1 : 0);
    // Python exception is preferred over Vim one; unlikely to occur though
    ਨਹੀਂਤਾਂ ਜੇ (PyErr_Occurred())
    {
	discard_current_exception();
	ਵਾਪਸ -1;
    }
    // Finally transform Vim script exception to python one
    ਨਹੀਂਤਾਂ
    {
	Py_Err_ਸੈੱਟ_Vim((ਅੱਖਰ *)current_exception->ਮੁੱਲ);
	discard_current_exception();
	ਵਾਪਸ -1;
    }
}

    ਸਥਿਰ ਪੂਰਨ
Vim_Check_ਰੁਕਾਵਟ(ਖਾਲੀ)
{
    ਜੇ (got_int)
    {
	Py_Err_ਸੈੱਟ_None(Py_Exc_Keyboard_ਰੁਕਾਵਟ);
	ਵਾਪਸ 1;
    }
    ਵਾਪਸ 0;
}

// Vim module - Implementation

    ਸਥਿਰ PyObject *
VimCommand(PyObject *self ਅਣਵਰਤਿਆ, PyObject *string)
{
    char_u	*cmd;
    PyObject	*ret;
    PyObject	*todecref;

    ਜੇ (!(cmd = String_ਮੰਜ਼ਿਲ_Chars(string, &todecref)))
	ਵਾਪਸ ਨਿਰਾ;

    Py_BEGIN_ALLOW_ਧਾਗੇ
    Python_ਤਾਲਾ_Vim();

    Vim_Try_ਸ਼ੁਰੂ();
    do_cmdline_cmd(cmd);
    update_screen(UPD_VALID);

    Python_Release_Vim();
    Py_ਸਿਰਾ_ALLOW_ਧਾਗੇ

    ਜੇ (Vim_Try_ਸਿਰਾ())
	ret = ਨਿਰਾ;
    ਨਹੀਂਤਾਂ
	ret = Py_None;

    Py_XINCREF(ret);
    Py_XDECREF(todecref);
    ਵਾਪਸ ret;
}

/*
 * Function to translate a typval_T into a PyObject; this will recursively
 * translate lists/dictionaries into their Python equivalents.
 *
 * The depth parameter is to avoid infinite recursion, set it to 1 when
 * you call VimToPython.
 */
    ਸਥਿਰ PyObject *
Vim_ਮੰਜ਼ਿਲ_Python(typval_T *our_tv, ਪੂਰਨ ਡੂੰਘਾਈ, PyObject *lookup_dict)
{
    PyObject	*ret;
    PyObject	*newObj;
    ਅੱਖਰ	ptrBuf[ਆਕਾਰ(ਖਾਲੀ *) * 2 + 3];

    // Avoid infinite recursion
    ਜੇ (ਡੂੰਘਾਈ > 100)
    {
	Py_INCREF(Py_None);
	ret = Py_None;
	ਵਾਪਸ ret;
    }

    // Check if we run into a recursive loop.  The item must be in lookup_dict
    // then and we can use it again.
    ਜੇ ((our_tv->v_type == VAR_LIST && our_tv->vval.v_list != ਨਿਰਾ)
	    || (our_tv->v_type == VAR_TUPLE && our_tv->vval.v_tuple != ਨਿਰਾ)
	    || (our_tv->v_type == VAR_DICT && our_tv->vval.v_dict != ਨਿਰਾ))
    {
	sprintf(ptrBuf, "%p",
		our_tv->v_type == VAR_LIST ? (ਖਾਲੀ *)our_tv->vval.v_list
		    : our_tv->v_type == VAR_TUPLE ?
			(ਖਾਲੀ *)our_tv->vval.v_tuple
			: (ਖਾਲੀ *)our_tv->vval.v_dict);

	ਜੇ ((ret = Py_Dict_Get_ਵਸਤੂ_String(lookup_dict, ptrBuf)))
	{
	    Py_INCREF(ret);
	    ਵਾਪਸ ret;
	}
    }

    ਜੇ (our_tv->v_type == VAR_STRING)
	ret = PyString_FromString(our_tv->vval.v_string == ਨਿਰਾ
					? "" : (ਅੱਖਰ *)our_tv->vval.v_string);
    ਨਹੀਂਤਾਂ ਜੇ (our_tv->v_type == VAR_ਸੰਖਿਆ)
    {
	ਅੱਖਰ buf[NUMBUFLEN];

	// For backwards compatibility numbers are stored as strings.
	sprintf(buf, "%lld", (ਲੰਮਾ ਲੰਮਾ)our_tv->vval.v_ਸੰਖਿਆ);
	ret = PyString_FromString((ਅੱਖਰ *)buf);
    }
    ਨਹੀਂਤਾਂ ਜੇ (our_tv->v_type == VAR_FLOAT)
    {
	ਅੱਖਰ buf[NUMBUFLEN];

	sprintf(buf, "%f", our_tv->vval.v_float);
	ret = PyString_FromString((ਅੱਖਰ *)buf);
    }
    ਨਹੀਂਤਾਂ ਜੇ (our_tv->v_type == VAR_LIST)
    {
	list_T		*list = our_tv->vval.v_list;
	listitem_T	*curr;

	ਜੇ (list == ਨਿਰਾ)
	    ਵਾਪਸ ਨਿਰਾ;

	ਜੇ (!(ret = PyList_New(0)))
	    ਵਾਪਸ ਨਿਰਾ;

	ਜੇ (Py_Dict_ਸੈੱਟ_ਵਸਤੂ_String(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    ਵਾਪਸ ਨਿਰਾ;
	}

	CHECK_LIST_MATERIALIZE(list);
	FOR_ALL_LIST_ਚੀਜ਼ਾਂ(list, curr)
	{
	    ਜੇ (!(newObj = Vim_ਮੰਜ਼ਿਲ_Python(&curr->li_tv, ਡੂੰਘਾਈ + 1, lookup_dict)))
	    {
		Py_DECREF(ret);
		ਵਾਪਸ ਨਿਰਾ;
	    }
	    ਜੇ (PyList_Append(ret, newObj))
	    {
		Py_DECREF(newObj);
		Py_DECREF(ret);
		ਵਾਪਸ ਨਿਰਾ;
	    }
	    Py_DECREF(newObj);
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (our_tv->v_type == VAR_TUPLE)
    {
	tuple_T	*tuple = our_tv->vval.v_tuple;
	ਪੂਰਨ	len;

	ਜੇ (tuple == ਨਿਰਾ)
	    ਵਾਪਸ ਨਿਰਾ;

	len = TUPLE_LEN(tuple);

	ret = PyTuple_New(len);
	ਜੇ (ret == ਨਿਰਾ)
	    ਵਾਪਸ ਨਿਰਾ;

	ਲਈ (ਪੂਰਨ idx = 0; idx < len; idx++)
	{
	    typval_T	*ਵਸਤੂ_tv = TUPLE_ਵਸਤੂ(tuple, idx);

	    newObj = Vim_ਮੰਜ਼ਿਲ_Python(ਵਸਤੂ_tv, ਡੂੰਘਾਈ + 1, lookup_dict);
	    ਜੇ (!newObj)
	    {
		Py_DECREF(ret);
		ਵਾਪਸ ਨਿਰਾ;
	    }
	    Py_Tuple_ਸੈੱਟ_ਵਸਤੂ(ret, idx, newObj);
	}

	ਜੇ (Py_Dict_ਸੈੱਟ_ਵਸਤੂ_String(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    ਵਾਪਸ ਨਿਰਾ;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (our_tv->v_type == VAR_DICT)
    {

	hashtab_T	*ht;
	long_u		todo;
	hashitem_T	*hi;
	dictitem_T	*di;

	ਜੇ (our_tv->vval.v_dict == ਨਿਰਾ)
	    ਵਾਪਸ ਨਿਰਾ;
	ht = &our_tv->vval.v_dict->dv_hashtab;

	ਜੇ (!(ret = PyDict_New()))
	    ਵਾਪਸ ਨਿਰਾ;

	ਜੇ (Py_Dict_ਸੈੱਟ_ਵਸਤੂ_String(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    ਵਾਪਸ ਨਿਰਾ;
	}

	todo = ht->ht_ਵਰਤਿਆ;
	ਲਈ (hi = ht->ht_array; todo > 0; ++hi)
	{
	    ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	    {
		--todo;

		di = dict_lookup(hi);
		ਜੇ (!(newObj = Vim_ਮੰਜ਼ਿਲ_Python(&di->di_tv, ਡੂੰਘਾਈ + 1, lookup_dict)))
		{
		    Py_DECREF(ret);
		    ਵਾਪਸ ਨਿਰਾ;
		}
		ਜੇ (Py_Dict_ਸੈੱਟ_ਵਸਤੂ_String(ret, (ਅੱਖਰ *)hi->hi_ਕੁੰਜੀ, newObj))
		{
		    Py_DECREF(ret);
		    Py_DECREF(newObj);
		    ਵਾਪਸ ਨਿਰਾ;
		}
	    }
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (our_tv->v_type == VAR_BOOL)
    {
	ਜੇ (our_tv->vval.v_ਸੰਖਿਆ == VVAL_FALSE)
	{
	    ret = Py_False;
	    Py_INCREF(ret);
	}
	ਨਹੀਂਤਾਂ
	{
	    ret = Py_True;
	    Py_INCREF(ret);
	}
	ਵਾਪਸ ret;
    }
    ਨਹੀਂਤਾਂ ਜੇ (our_tv->v_type == VAR_SPECIAL)
    {
	Py_INCREF(Py_None);
	ret = Py_None;
	ਵਾਪਸ ret;
    }
    ਨਹੀਂਤਾਂ ਜੇ (our_tv->v_type == VAR_BLOB)
	ret = Py_ਬਾਈਟਾਂ_From_String_And_ਮਾਪ(
		(ਅੱਖਰ*) our_tv->vval.v_blob->bv_ga.ga_ਅੰਕੜੇ,
		(Py_ssize_t) our_tv->vval.v_blob->bv_ga.ga_len);
    ਨਹੀਂਤਾਂ
    {
	Py_INCREF(Py_None);
	ret = Py_None;
    }

    ਵਾਪਸ ret;
}

    ਸਥਿਰ PyObject *
VimEval(PyObject *self ਅਣਵਰਤਿਆ, PyObject *ਦਲੀਲਾਂ)
{
    char_u	*ਵਿਅੰਜਕ;
    typval_T	*our_tv;
    PyObject	*string;
    PyObject	*todecref;
    PyObject	*ret;
    PyObject	*lookup_dict;

    ਜੇ (!PyArg_ParseTuple(ਦਲੀਲਾਂ, "O", &string))
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (!(ਵਿਅੰਜਕ = String_ਮੰਜ਼ਿਲ_Chars(string, &todecref)))
	ਵਾਪਸ ਨਿਰਾ;

    Py_BEGIN_ALLOW_ਧਾਗੇ
    Python_ਤਾਲਾ_Vim();
    Vim_Try_ਸ਼ੁਰੂ();
    our_tv = eval_ਵਿਅੰਜਕ(ਵਿਅੰਜਕ, ਨਿਰਾ);
    Python_Release_Vim();
    Py_ਸਿਰਾ_ALLOW_ਧਾਗੇ

    Py_XDECREF(todecref);

    ਜੇ (Vim_Try_ਸਿਰਾ())
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (our_tv == ਨਿਰਾ)
    {
	Py_Err_ਸੈੱਟ_VIM(N_("invalid expression"));
	ਵਾਪਸ ਨਿਰਾ;
    }

    // Convert the Vim type into a Python type.  Create a dictionary that's
    // used to check for recursive loops.
    ਜੇ (!(lookup_dict = PyDict_New()))
	ret = ਨਿਰਾ;
    ਨਹੀਂਤਾਂ
    {
	ret = Vim_ਮੰਜ਼ਿਲ_Python(our_tv, 1, lookup_dict);
	Py_DECREF(lookup_dict);
    }


    Py_BEGIN_ALLOW_ਧਾਗੇ
    Python_ਤਾਲਾ_Vim();
    free_tv(our_tv);
    Python_Release_Vim();
    Py_ਸਿਰਾ_ALLOW_ਧਾਗੇ

    ਵਾਪਸ ret;
}

ਸਥਿਰ PyObject *Convert_ਮੰਜ਼ਿਲ_Py_Object(typval_T *);

    ਸਥਿਰ PyObject *
VimEvalPy(PyObject *self ਅਣਵਰਤਿਆ, PyObject *string)
{
    typval_T	*our_tv;
    PyObject	*ret;
    char_u	*ਵਿਅੰਜਕ;
    PyObject	*todecref;

    ਜੇ (!(ਵਿਅੰਜਕ = String_ਮੰਜ਼ਿਲ_Chars(string, &todecref)))
	ਵਾਪਸ ਨਿਰਾ;

    Py_BEGIN_ALLOW_ਧਾਗੇ
    Python_ਤਾਲਾ_Vim();
    Vim_Try_ਸ਼ੁਰੂ();
    our_tv = eval_ਵਿਅੰਜਕ(ਵਿਅੰਜਕ, ਨਿਰਾ);
    Python_Release_Vim();
    Py_ਸਿਰਾ_ALLOW_ਧਾਗੇ

    Py_XDECREF(todecref);

    ਜੇ (Vim_Try_ਸਿਰਾ())
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (our_tv == ਨਿਰਾ)
    {
	Py_Err_ਸੈੱਟ_VIM(N_("invalid expression"));
	ਵਾਪਸ ਨਿਰਾ;
    }

    ret = Convert_ਮੰਜ਼ਿਲ_Py_Object(our_tv);
    Py_BEGIN_ALLOW_ਧਾਗੇ
    Python_ਤਾਲਾ_Vim();
    free_tv(our_tv);
    Python_Release_Vim();
    Py_ਸਿਰਾ_ALLOW_ਧਾਗੇ

    ਵਾਪਸ ret;
}

    ਸਥਿਰ PyObject *
VimStrwidth(PyObject *self ਅਣਵਰਤਿਆ, PyObject *string)
{
    char_u	*str;
    PyObject	*todecref;
    ਪੂਰਨ		len;

    ਜੇ (!(str = String_ਮੰਜ਼ਿਲ_Chars(string, &todecref)))
	ਵਾਪਸ ਨਿਰਾ;

    len = mb_string2cells(str, (ਪੂਰਨ)STRLEN(str));

    Py_XDECREF(todecref);

    ਵਾਪਸ PyLong_FromLong(len);
}

    ਸਥਿਰ PyObject *
_VimChdir(PyObject *_chdir, PyObject *ਦਲੀਲਾਂ, PyObject *kwargs)
{
    PyObject	*ret;
    PyObject	*newwd;
    PyObject	*todecref;
    char_u	*new_dir;

    ਜੇ (_chdir == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    ਜੇ (!(ret = PyObject_Call(_chdir, ਦਲੀਲਾਂ, kwargs)))
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (!(newwd = Py_Object_Call_Function_Obj_ਦਲੀਲਾਂ(py_getcwd, ਨਿਰਾ)))
    {
	Py_DECREF(ret);
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਜੇ (!(new_dir = String_ਮੰਜ਼ਿਲ_Chars(newwd, &todecref)))
    {
	Py_DECREF(ret);
	Py_DECREF(newwd);
	ਵਾਪਸ ਨਿਰਾ;
    }

    Vim_Try_ਸ਼ੁਰੂ();

    ਜੇ (vim_chdir(new_dir))
    {
	Py_DECREF(ret);
	Py_DECREF(newwd);
	Py_XDECREF(todecref);

	ਜੇ (Vim_Try_ਸਿਰਾ())
	    ਵਾਪਸ ਨਿਰਾ;

	Py_Err_ਸੈੱਟ_VIM(N_("failed to change directory"));
	ਵਾਪਸ ਨਿਰਾ;
    }

    Py_DECREF(newwd);
    Py_XDECREF(todecref);

    post_chdir(CDSCOPE_GLOBAL);

    ਜੇ (Vim_Try_ਸਿਰਾ())
    {
	Py_DECREF(ret);
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਵਾਪਸ ret;
}

    ਸਥਿਰ PyObject *
VimChdir(PyObject *self ਅਣਵਰਤਿਆ, PyObject *ਦਲੀਲਾਂ, PyObject *kwargs)
{
    ਵਾਪਸ _VimChdir(py_chdir, ਦਲੀਲਾਂ, kwargs);
}

    ਸਥਿਰ PyObject *
VimFchdir(PyObject *self ਅਣਵਰਤਿਆ, PyObject *ਦਲੀਲਾਂ, PyObject *kwargs)
{
    ਵਾਪਸ _VimChdir(py_fchdir, ਦਲੀਲਾਂ, kwargs);
}

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    PyObject	*callable;
    PyObject	*ਨਤੀਜਾ;
} map_rtp_ਅੰਕੜੇ;

    ਸਥਿਰ ਖਾਲੀ
map_rtp_callback(char_u *ਰਸਤਾ, ਖਾਲੀ *ਅੰਕੜੇ_3)
{
    ਖਾਲੀ	**ਅੰਕੜੇ = (ਖਾਲੀ **) ਅੰਕੜੇ_3;
    PyObject	*ਰਸਤਾ_Object;
    map_rtp_ਅੰਕੜੇ	*mr_ਅੰਕੜੇ = *((map_rtp_ਅੰਕੜੇ **) ਅੰਕੜੇ);

    ਜੇ (!(ਰਸਤਾ_Object = PyString_FromString((ਅੱਖਰ *)ਰਸਤਾ)))
    {
	*ਅੰਕੜੇ = ਨਿਰਾ;
	ਵਾਪਸ;
    }

    mr_ਅੰਕੜੇ->ਨਤੀਜਾ = Py_Object_Call_Function_Obj_ਦਲੀਲਾਂ(mr_ਅੰਕੜੇ->callable,
						   ਰਸਤਾ_Object, ਨਿਰਾ);

    Py_DECREF(ਰਸਤਾ_Object);

    ਜੇ (!mr_ਅੰਕੜੇ->ਨਤੀਜਾ || mr_ਅੰਕੜੇ->ਨਤੀਜਾ != Py_None)
	*ਅੰਕੜੇ = ਨਿਰਾ;
    ਨਹੀਂਤਾਂ
    {
	Py_DECREF(mr_ਅੰਕੜੇ->ਨਤੀਜਾ);
	mr_ਅੰਕੜੇ->ਨਤੀਜਾ = ਨਿਰਾ;
    }
}

    ਸਥਿਰ PyObject *
VimForeachRTP(PyObject *self ਅਣਵਰਤਿਆ, PyObject *callable)
{
    map_rtp_ਅੰਕੜੇ	ਅੰਕੜੇ;

    ਅੰਕੜੇ.callable = callable;
    ਅੰਕੜੇ.ਨਤੀਜਾ = ਨਿਰਾ;

    do_ਵਿੱਚ_runtimepath(ਨਿਰਾ, 0, &map_rtp_callback, &ਅੰਕੜੇ);

    ਜੇ (ਅੰਕੜੇ.ਨਤੀਜਾ == ਨਿਰਾ)
    {
	ਜੇ (PyErr_Occurred())
	    ਵਾਪਸ ਨਿਰਾ;
	ਨਹੀਂਤਾਂ
	{
	    Py_INCREF(Py_None);
	    ਵਾਪਸ Py_None;
	}
    }
    ਵਾਪਸ ਅੰਕੜੇ.ਨਤੀਜਾ;
}

/*
 * _vim_runtimepath_ special path implementation.
 */

    ਸਥਿਰ ਖਾਲੀ
map_finder_callback(char_u *ਰਸਤਾ, ਖਾਲੀ *ਅੰਕੜੇ_3)
{
    ਖਾਲੀ	**ਅੰਕੜੇ = (ਖਾਲੀ **) ਅੰਕੜੇ_3;
    PyObject	*list = *((PyObject **) ਅੰਕੜੇ);
    PyObject	*ਰਸਤਾ_Object1, *ਰਸਤਾ_Object2;
    ਅੱਖਰ	*pathbuf;
    size_t	pathlen;

    pathlen = STRLEN(ਰਸਤਾ);

#ਜੇ PY_MAJOR_ਸੰਸਕਰਣ < 3
# define PY_MAIN_DIR_STRING "python2"
#ਨਹੀਂਤਾਂ
# define PY_MAIN_DIR_STRING "python3"
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PY_ALTERNATE_DIR_STRING "pythonx"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PYTHONX_STRING_ਲੰਮਾਈ 7 // STRLEN("pythonx")
    ਜੇ (!(pathbuf = PyMem_New(ਅੱਖਰ,
		    pathlen + STRLEN(PATHSEPSTR) + PYTHONX_STRING_ਲੰਮਾਈ + 1)))
    {
	Py_Err_No_ਸਮ੍ਰਿਤੀ();
	*ਅੰਕੜੇ = ਨਿਰਾ;
	ਵਾਪਸ;
    }

    mch_memmove(pathbuf, ਰਸਤਾ, pathlen + 1);
    ਜੋੜੋ_pathsep((char_u *) pathbuf);

    pathlen = STRLEN(pathbuf);
    mch_memmove(pathbuf + pathlen, PY_MAIN_DIR_STRING,
	    PYTHONX_STRING_ਲੰਮਾਈ + 1);

    ਜੇ (!(ਰਸਤਾ_Object1 = PyString_FromString(pathbuf)))
    {
	*ਅੰਕੜੇ = ਨਿਰਾ;
	PyMem_Free(pathbuf);
	ਵਾਪਸ;
    }

    mch_memmove(pathbuf + pathlen, PY_ALTERNATE_DIR_STRING,
	    PYTHONX_STRING_ਲੰਮਾਈ + 1);

    ਜੇ (!(ਰਸਤਾ_Object2 = PyString_FromString(pathbuf)))
    {
	Py_DECREF(ਰਸਤਾ_Object1);
	PyMem_Free(pathbuf);
	*ਅੰਕੜੇ = ਨਿਰਾ;
	ਵਾਪਸ;
    }

    PyMem_Free(pathbuf);

    ਜੇ (PyList_Append(list, ਰਸਤਾ_Object1)
	    || PyList_Append(list, ਰਸਤਾ_Object2))
	*ਅੰਕੜੇ = ਨਿਰਾ;

    Py_DECREF(ਰਸਤਾ_Object1);
    Py_DECREF(ਰਸਤਾ_Object2);
}

    ਸਥਿਰ PyObject *
Vim_GetPaths(PyObject *self ਅਣਵਰਤਿਆ, PyObject *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    PyObject	*ret;

    ਜੇ (!(ret = PyList_New(0)))
	ਵਾਪਸ ਨਿਰਾ;

    do_ਵਿੱਚ_runtimepath(ਨਿਰਾ, 0, &map_finder_callback, ret);

    ਜੇ (PyErr_Occurred())
    {
	Py_DECREF(ret);
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਵਾਪਸ ret;
}

#ਜੇ PY_ਸੰਸਕਰਣ_HEX >= 0x030700f0
    ਸਥਿਰ PyObject *
Finder_ਲੱਭੋ_Spec(PyObject *self, PyObject *ਦਲੀਲਾਂ)
{
    ਅੱਖਰ	*fullname;
    PyObject	*paths;
    PyObject	*ਨਿਸ਼ਾਨਾ = Py_None;
    PyObject	*spec;

    ਜੇ (!PyArg_ParseTuple(ਦਲੀਲਾਂ, "s|O", &fullname, &ਨਿਸ਼ਾਨਾ))
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (!(paths = Vim_GetPaths(self, ਨਿਰਾ)))
	ਵਾਪਸ ਨਿਰਾ;

    spec = PyObject_CallFunction(py_ਲੱਭੋ_spec, "sOO", fullname, paths, ਨਿਸ਼ਾਨਾ);

    Py_DECREF(paths);

    ਜੇ (!spec)
    {
	ਜੇ (PyErr_Occurred())
	    ਵਾਪਸ ਨਿਰਾ;

	Py_INCREF(Py_None);
	ਵਾਪਸ Py_None;
    }

    ਵਾਪਸ spec;
}

    ਸਥਿਰ PyObject *
Finder_ਲੱਭੋ_Module(PyObject* self ਅਣਵਰਤਿਆ, PyObject* ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    // Apparently returning None works.
    Py_INCREF(Py_None);
    ਵਾਪਸ Py_None;
}
#ਨਹੀਂਤਾਂ
    ਸਥਿਰ PyObject *
call_ਲੱਦੋ_module(ਅੱਖਰ *name, ਪੂਰਨ len, PyObject *ਲੱਭੋ_module_ਨਤੀਜਾ)
{
    PyObject	*fd, *pathname, *description;

    ਜੇ (!PyTuple_Check(ਲੱਭੋ_module_ਨਤੀਜਾ))
    {
	PyErr_FORMAT_TYPE(
		N_("expected 3-tuple as imp.find_module() result, but got %s"),
		ਲੱਭੋ_module_ਨਤੀਜਾ);
	ਵਾਪਸ ਨਿਰਾ;
    }
    ਜੇ (Py_Tuple_GET_ਮਾਪ(ਲੱਭੋ_module_ਨਤੀਜਾ) != 3)
    {
	PyErr_FORMAT(PyExc_TypeError,
		N_("expected 3-tuple as imp.find_module() result, but got "
		   "tuple of size %d"),
		(ਪੂਰਨ) Py_Tuple_GET_ਮਾਪ(ਲੱਭੋ_module_ਨਤੀਜਾ));
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਜੇ (!(fd = Py_Tuple_GET_ਵਸਤੂ(ਲੱਭੋ_module_ਨਤੀਜਾ, 0))
	    || !(pathname = Py_Tuple_GET_ਵਸਤੂ(ਲੱਭੋ_module_ਨਤੀਜਾ, 1))
	    || !(description = Py_Tuple_GET_ਵਸਤੂ(ਲੱਭੋ_module_ਨਤੀਜਾ, 2)))
    {
	Py_Err_ਸੈੱਟ_STRING(PyExc_RuntimeError,
		N_("internal error: imp.find_module returned tuple with NULL"));
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਵਾਪਸ PyObject_CallFunction(py_ਲੱਦੋ_module,
	    "s#OOO", name, len, fd, pathname, description);
}

    ਸਥਿਰ PyObject *
ਲੱਭੋ_module(ਅੱਖਰ *fullname, ਅੱਖਰ *ਪੂਛ, PyObject *new_ਰਸਤਾ)
{
    PyObject	*ਲੱਭੋ_module_ਨਤੀਜਾ;
    PyObject	*module;
    ਅੱਖਰ	*dot;

    ਜੇ ((dot = (ਅੱਖਰ *)vim_strchr((char_u *) ਪੂਛ, '.')))
    {
	/*
	 * There is a dot in the name: call find_module recursively without the
	 * first component
	 */
	PyObject	*newest_ਰਸਤਾ;
	ਪੂਰਨ		partlen = (ਪੂਰਨ) (dot - 1 - ਪੂਛ);

	ਜੇ (!(ਲੱਭੋ_module_ਨਤੀਜਾ = PyObject_CallFunction(py_ਲੱਭੋ_module,
			"s#O", ਪੂਛ, partlen, new_ਰਸਤਾ)))
	{
	    ਜੇ (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_ImportError))
		PyErr_Clear();
	    ਵਾਪਸ ਨਿਰਾ;
	}

	ਜੇ (!(module = call_ਲੱਦੋ_module(
			fullname,
			((ਪੂਰਨ) (ਪੂਛ - fullname)) + partlen,
			ਲੱਭੋ_module_ਨਤੀਜਾ)))
	{
	    Py_DECREF(ਲੱਭੋ_module_ਨਤੀਜਾ);
	    ਵਾਪਸ ਨਿਰਾ;
	}

	Py_DECREF(ਲੱਭੋ_module_ਨਤੀਜਾ);

	ਜੇ (!(newest_ਰਸਤਾ = PyObject_GetAttrString(module, "__path__")))
	{
	    Py_DECREF(module);
	    ਵਾਪਸ ਨਿਰਾ;
	}

	Py_DECREF(module);

	ਲੱਭੋ_module_ਨਤੀਜਾ = ਲੱਭੋ_module(fullname, dot + 1, newest_ਰਸਤਾ);

	Py_DECREF(newest_ਰਸਤਾ);

	ਵਾਪਸ ਲੱਭੋ_module_ਨਤੀਜਾ;
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (!(ਲੱਭੋ_module_ਨਤੀਜਾ = PyObject_CallFunction(py_ਲੱਭੋ_module,
			"sO", ਪੂਛ, new_ਰਸਤਾ)))
	{
	    ਜੇ (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_ImportError))
		PyErr_Clear();
	    ਵਾਪਸ ਨਿਰਾ;
	}

	ਵਾਪਸ ਲੱਭੋ_module_ਨਤੀਜਾ;
    }
}

    ਸਥਿਰ PyObject *
Finder_ਲੱਭੋ_Module(PyObject *self, PyObject *ਦਲੀਲਾਂ)
{
    ਅੱਖਰ	*fullname;
    PyObject	*ਨਤੀਜਾ;
    PyObject	*new_ਰਸਤਾ;
    LoaderObject	*loader;

    ਜੇ (!PyArg_ParseTuple(ਦਲੀਲਾਂ, "s", &fullname))
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (!(new_ਰਸਤਾ = Vim_GetPaths(self, ਨਿਰਾ)))
	ਵਾਪਸ ਨਿਰਾ;

    ਨਤੀਜਾ = ਲੱਭੋ_module(fullname, fullname, new_ਰਸਤਾ);

    Py_DECREF(new_ਰਸਤਾ);

    ਜੇ (!ਨਤੀਜਾ)
    {
	ਜੇ (PyErr_Occurred())
	    ਵਾਪਸ ਨਿਰਾ;

	Py_INCREF(Py_None);
	ਵਾਪਸ Py_None;
    }

    ਜੇ (!(fullname = (ਅੱਖਰ *)vim_strsave((char_u *)fullname)))
    {
	Py_DECREF(ਨਤੀਜਾ);
	Py_Err_No_ਸਮ੍ਰਿਤੀ();
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਜੇ (!(loader = PyObject_NEW(LoaderObject, LoaderTypePtr)))
    {
	vim_free(fullname);
	Py_DECREF(ਨਤੀਜਾ);
	ਵਾਪਸ ਨਿਰਾ;
    }

    loader->fullname = fullname;
    loader->ਨਤੀਜਾ = ਨਤੀਜਾ;

    ਵਾਪਸ (PyObject *) loader;
}
#ਜੇ_ਅੰਤ

    ਸਥਿਰ PyObject *
Vim_ਰਸਤਾ_Hook(PyObject *self ਅਣਵਰਤਿਆ, PyObject *ਦਲੀਲਾਂ)
{
    ਅੱਖਰ	*ਰਸਤਾ;

    ਜੇ (PyArg_ParseTuple(ਦਲੀਲਾਂ, "s", &ਰਸਤਾ)
	    && STRCMP(ਰਸਤਾ, vim_special_ਰਸਤਾ) == 0)
    {
	Py_INCREF(vim_module);
	ਵਾਪਸ vim_module;
    }

    PyErr_Clear();
    Py_Err_ਸੈੱਟ_None(PyExc_ImportError);
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Vim module - Definitions
 */

ਸਥਿਰ ਬਣਤਰ Py_ਵਿਧੀ_Def VimMethods[] = {
    // name,	    function,			calling,			documentation
    {"command",	    VimCommand,			METH_O,				"Execute a Vim ex-mode command" },
    {"eval",	    VimEval,			METH_VARARGS,			"Evaluate an expression using Vim evaluator" },
    {"bindeval",    VimEvalPy,			METH_O,				"Like eval(), but returns objects attached to Vim ones"},
    {"strwidth",    VimStrwidth,		METH_O,				"Screen string width, counts <Tab> as having width 1"},
    {"chdir",	    (PyCFunction)(ਖਾਲੀ *)VimChdir,	METH_VARARGS|METH_KEYWORDS,	"Change directory"},
    {"fchdir",	    (PyCFunction)(ਖਾਲੀ *)VimFchdir,	METH_VARARGS|METH_KEYWORDS,	"Change directory"},
    {"foreach_rtp", VimForeachRTP,		METH_O,				"Call given callable for each path in &rtp"},
#ਜੇ PY_ਸੰਸਕਰਣ_HEX >= 0x030700f0
    {"find_spec",   Finder_ਲੱਭੋ_Spec,		METH_VARARGS,			"Internal use only, returns spec object for any input it receives"},
#ਜੇ_ਅੰਤ
    {"find_module", Finder_ਲੱਭੋ_Module,		METH_VARARGS,			"Internal use only, returns loader object for any input it receives"},
    {"path_hook",   Vim_ਰਸਤਾ_Hook,		METH_VARARGS,			"Hook function to install in sys.path_hooks"},
    {"_get_paths",  (PyCFunction)Vim_GetPaths,	METH_NOARGS,			"Get &rtp-based additions to sys.path"},
    { ਨਿਰਾ,	    ਨਿਰਾ,			0,				ਨਿਰਾ}
};

/*
 * Generic iterator object
 */

DEFINE_PY_TYPE_OBJECT(IterType);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ PyObject *(*nextfun)(ਖਾਲੀ **);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ (*destructorfun)(ਖਾਲੀ *);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*traversefun)(ਖਾਲੀ *, visitproc, ਖਾਲੀ *);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*clearfun)(ਖਾਲੀ **);

// Main purpose of this object is removing the need for do python
// initialization (i.e. PyType_Ready and setting type attributes) for a big
// bunch of objects.
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    Py_Object_ਸਿਰ
    ਖਾਲੀ *cur;
    nextfun ਅਗਲਾ;
    destructorfun destruct;
    traversefun traverse;
    clearfun clear;
    PyObject *iter_object;
} IterObject;

    ਸਥਿਰ PyObject *
IterNew(ਖਾਲੀ *ਸ਼ੁਰੂ, destructorfun destruct, nextfun ਅਗਲਾ, traversefun traverse,
	clearfun clear, PyObject *iter_object)
{
    IterObject *self;

    self = PyObject_GC_New(IterObject, IterTypePtr);
    self->cur = ਸ਼ੁਰੂ;
    self->ਅਗਲਾ = ਅਗਲਾ;
    self->destruct = destruct;
    self->traverse = traverse;
    self->clear = clear;
    self->iter_object = iter_object;

    ਜੇ (iter_object)
	Py_INCREF(iter_object);

    ਵਾਪਸ (PyObject *)(self);
}

    ਸਥਿਰ ਖਾਲੀ
IterDestructor(PyObject *self_obj)
{
    IterObject *self = (IterObject*)self_obj;
    ਜੇ (self->iter_object)
	Py_DECREF(self->iter_object);
    PyObject_GC_UnTrack((ਖਾਲੀ *)(self));
    self->destruct(self->cur);
    PyObject_GC_Del((ਖਾਲੀ *)(self));
}

    ਸਥਿਰ ਪੂਰਨ
IterTraverse(PyObject *self_obj, visitproc ਦੇਖੋ, ਖਾਲੀ *arg)
{
    IterObject *self = (IterObject*)self_obj;
    ਜੇ (self->traverse != ਨਿਰਾ)
	ਵਾਪਸ self->traverse(self->cur, ਦੇਖੋ, arg);
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ 0;
}

// Mac OSX defines clear() somewhere.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ clear
# undef clear
#ਜੇ_ਅੰਤ

    ਸਥਿਰ ਪੂਰਨ
IterClear(PyObject *self_obj)
{
    IterObject *self = (IterObject*)self_obj;
    ਜੇ (self->clear != ਨਿਰਾ)
	ਵਾਪਸ self->clear(&self->cur);
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ 0;
}

    ਸਥਿਰ PyObject *
Iter_ਅਗਲਾ(PyObject *self_obj)
{
    IterObject *self = (IterObject*)self_obj;
    ਵਾਪਸ self->ਅਗਲਾ(&self->cur);
}

    ਸਥਿਰ PyObject *
IterIter(PyObject *self)
{
    Py_INCREF(self);
    ਵਾਪਸ self;
}

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ pylinkedlist_S {
    ਬਣਤਰ pylinkedlist_S	*pll_ਅਗਲਾ;
    ਬਣਤਰ pylinkedlist_S	*pll_prev;
    PyObject			*pll_obj;
} pylinkedlist_T;

ਸਥਿਰ pylinkedlist_T *lastdict = ਨਿਰਾ;
ਸਥਿਰ pylinkedlist_T *lastlist = ਨਿਰਾ;
ਸਥਿਰ pylinkedlist_T *lasttuple = ਨਿਰਾ;
ਸਥਿਰ pylinkedlist_T *lastfunc = ਨਿਰਾ;

    ਸਥਿਰ ਖਾਲੀ
pyll_remove(pylinkedlist_T *ref, pylinkedlist_T **ਆਖਰੀ)
{
    ਜੇ (ref->pll_prev == ਨਿਰਾ)
    {
	ਜੇ (ref->pll_ਅਗਲਾ == ਨਿਰਾ)
	{
	    *ਆਖਰੀ = ਨਿਰਾ;
	    ਵਾਪਸ;
	}
    }
    ਨਹੀਂਤਾਂ
	ref->pll_prev->pll_ਅਗਲਾ = ref->pll_ਅਗਲਾ;

    ਜੇ (ref->pll_ਅਗਲਾ == ਨਿਰਾ)
	*ਆਖਰੀ = ref->pll_prev;
    ਨਹੀਂਤਾਂ
	ref->pll_ਅਗਲਾ->pll_prev = ref->pll_prev;
}

    ਸਥਿਰ ਖਾਲੀ
pyll_ਜੋੜੋ(PyObject *self, pylinkedlist_T *ref, pylinkedlist_T **ਆਖਰੀ)
{
    ਜੇ (*ਆਖਰੀ == ਨਿਰਾ)
	ref->pll_prev = ਨਿਰਾ;
    ਨਹੀਂਤਾਂ
    {
	(*ਆਖਰੀ)->pll_ਅਗਲਾ = ref;
	ref->pll_prev = *ਆਖਰੀ;
    }
    ref->pll_ਅਗਲਾ = ਨਿਰਾ;
    ref->pll_obj = self;
    *ਆਖਰੀ = ref;
}

DEFINE_PY_TYPE_OBJECT(DictionaryType);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    Py_Object_ਸਿਰ
    dict_T	*dict;
    pylinkedlist_T	ref;
} DictionaryObject;

ਸਥਿਰ PyObject *DictionaryUpdate(DictionaryObject *, PyObject *, PyObject *);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NEW_DICTIONARY(dict) DictionaryNew(DictionaryTypePtr, dict)

    ਸਥਿਰ PyObject *
DictionaryNew(PyTypeObject *subtype, dict_T *dict)
{
    DictionaryObject	*self;

    self = (DictionaryObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    ਜੇ (self == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    self->dict = dict;
    ++dict->dv_refcount;

    pyll_ਜੋੜੋ((PyObject *)(self), &self->ref, &lastdict);

    ਵਾਪਸ (PyObject *)(self);
}

    ਸਥਿਰ dict_T *
py_dict_alloc(ਖਾਲੀ)
{
    dict_T	*ret;

    ਜੇ (!(ret = dict_alloc()))
    {
	Py_Err_No_ਸਮ੍ਰਿਤੀ();
	ਵਾਪਸ ਨਿਰਾ;
    }
    ++ret->dv_refcount;

    ਵਾਪਸ ret;
}

    ਸਥਿਰ PyObject *
DictionaryConstructor(PyTypeObject *subtype, PyObject *ਦਲੀਲਾਂ, PyObject *kwargs)
{
    DictionaryObject	*self;
    dict_T	*dict;

    ਜੇ (!(dict = py_dict_alloc()))
	ਵਾਪਸ ਨਿਰਾ;

    self = (DictionaryObject *) DictionaryNew(subtype, dict);

    --dict->dv_refcount;

    ਜੇ (kwargs || Py_Tuple_ਮਾਪ(ਦਲੀਲਾਂ))
    {
	PyObject	*tmp;
	ਜੇ (!(tmp = DictionaryUpdate(self, ਦਲੀਲਾਂ, kwargs)))
	{
	    Py_DECREF(self);
	    ਵਾਪਸ ਨਿਰਾ;
	}

	Py_DECREF(tmp);
    }

    ਵਾਪਸ (PyObject *)(self);
}

    ਸਥਿਰ ਖਾਲੀ
DictionaryDestructor(PyObject *self_obj)
{
    DictionaryObject *self = (DictionaryObject*)self_obj;
    pyll_remove(&self->ref, &lastdict);
    dict_unref(self->dict);

    DESTRUCTOR_ਅੰਤ(self);
}

ਸਥਿਰ ਅੱਖਰ *DictionaryAttrs[] = {
    "locked", "scope",
    ਨਿਰਾ
};

    ਸਥਿਰ PyObject *
DictionaryDir(PyObject *self, PyObject *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ ObjectDir(self, DictionaryAttrs);
}

    ਸਥਿਰ ਪੂਰਨ
DictionarySetattr(PyObject *self_obj, ਅੱਖਰ *name, PyObject *valObject)
{
    DictionaryObject *self = (DictionaryObject*)self_obj;
    ਜੇ (valObject == ਨਿਰਾ)
    {
	Py_Err_ਸੈੱਟ_STRING(PyExc_AttributeError,
		N_("cannot delete vim.Dictionary attributes"));
	ਵਾਪਸ -1;
    }

    ਜੇ (strcmp(name, "locked") == 0)
    {
	ਜੇ (self->dict->dv_ਤਾਲਾ == VAR_FIXED)
	{
	    Py_Err_ਸੈੱਟ_STRING(PyExc_TypeError,
		    N_("cannot modify fixed dictionary"));
	    ਵਾਪਸ -1;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਪੂਰਨ		istrue = PyObject_IsTrue(valObject);
	    ਜੇ (istrue == -1)
		ਵਾਪਸ -1;
	    ਨਹੀਂਤਾਂ ਜੇ (istrue)
		self->dict->dv_ਤਾਲਾ = VAR_LOCKED;
	    ਨਹੀਂਤਾਂ
		self->dict->dv_ਤਾਲਾ = 0;
	}
	ਵਾਪਸ 0;
    }
    ਨਹੀਂਤਾਂ
    {
	PyErr_FORMAT(PyExc_AttributeError, N_("cannot set attribute %s"), name);
	ਵਾਪਸ -1;
    }
}

    ਸਥਿਰ PyInt
Dictionary_ਲੰਮਾਈ(DictionaryObject *self)
{
    ਵਾਪਸ ((PyInt) (self->dict->dv_hashtab.ht_ਵਰਤਿਆ));
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DICT_FLAG_HAS_DEFAULT	0x01
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DICT_FLAG_ਕੱਢੋ		0x02
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DICT_FLAG_NONE_DEFAULT	0x04
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DICT_FLAG_RETURN_BOOL	0x08 // Incompatible with DICT_FLAG_POP
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DICT_FLAG_RETURN_PAIR	0x10

    ਸਥਿਰ PyObject *
Dictionary_ਵਸਤੂ(DictionaryObject *self, PyObject *ਦਲੀਲਾਂ, ਪੂਰਨ flags)
{
    PyObject	*ਕੁੰਜੀ_Object;
    PyObject	*defObject = ((flags & DICT_FLAG_NONE_DEFAULT)? Py_None : ਨਿਰਾ);
    PyObject	*ret;
    char_u	*ਕੁੰਜੀ;
    dictitem_T	*di;
    dict_T	*dict = self->dict;
    hashitem_T	*hi;
    PyObject	*todecref;

    ਜੇ (flags & DICT_FLAG_HAS_DEFAULT)
    {
	ਜੇ (!PyArg_ParseTuple(ਦਲੀਲਾਂ, "O|O", &ਕੁੰਜੀ_Object, &defObject))
	    ਵਾਪਸ ਨਿਰਾ;
    }
    ਨਹੀਂਤਾਂ
	ਕੁੰਜੀ_Object = ਦਲੀਲਾਂ;

    ਜੇ (flags & DICT_FLAG_RETURN_BOOL)
	defObject = Py_False;

    ਜੇ (!(ਕੁੰਜੀ = String_ਮੰਜ਼ਿਲ_Chars(ਕੁੰਜੀ_Object, &todecref)))
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (*ਕੁੰਜੀ == NUL)
    {
	RAISE_NO_ਖਾਲੀ_ਥਾਵਾਂ_ਕੁੰਜੀਆਂ;
	Py_XDECREF(todecref);
	ਵਾਪਸ ਨਿਰਾ;
    }

    hi = hash_ਲੱਭੋ(&dict->dv_hashtab, ਕੁੰਜੀ);

    Py_XDECREF(todecref);

    ਜੇ (HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
    {
	ਜੇ (defObject)
	{
	    Py_INCREF(defObject);
	    ਵਾਪਸ defObject;
	}
	ਨਹੀਂਤਾਂ
	{
	    Py_Err_ਸੈੱਟ_Object(Py_Exc_ਕੁੰਜੀ_Error, ਕੁੰਜੀ_Object);
	    ਵਾਪਸ ਨਿਰਾ;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (flags & DICT_FLAG_RETURN_BOOL)
    {
	ret = Py_True;
	Py_INCREF(ret);
	ਵਾਪਸ ret;
    }

    di = dict_lookup(hi);

    ਜੇ (!(ret = Convert_ਮੰਜ਼ਿਲ_Py_Object(&di->di_tv)))
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (flags & DICT_FLAG_ਕੱਢੋ)
    {
	ਜੇ (dict->dv_ਤਾਲਾ)
	{
	    RAISE_LOCKED_DICTIONARY;
	    Py_DECREF(ret);
	    ਵਾਪਸ ਨਿਰਾ;
	}

	hash_remove(&dict->dv_hashtab, hi, "Python remove variable");
	dictitem_free(di);
    }

    ਵਾਪਸ ret;
}

    ਸਥਿਰ PyObject *
Dictionary_ਵਸਤੂ_2(DictionaryObject *self, PyObject *ਕੁੰਜੀ_Object)
{
    ਵਾਪਸ Dictionary_ਵਸਤੂ(self, ਕੁੰਜੀ_Object, 0);
}

    ਸਥਿਰ ਪੂਰਨ
Dictionary_ਸ਼ਾਮਲ_ਹੈ(DictionaryObject *self, PyObject *ਕੁੰਜੀ_Object)
{
    PyObject	*rObj = Dictionary_ਵਸਤੂ(self, ਕੁੰਜੀ_Object, DICT_FLAG_RETURN_BOOL);
    ਪੂਰਨ		ret;

    ਜੇ (rObj == ਨਿਰਾ)
	ਵਾਪਸ -1;

    ret = (rObj == Py_True);

    Py_DECREF(rObj);

    ਵਾਪਸ ret;
}

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਪੂਰਨ		dii_ਬਦਲਿਆ;
    hashtab_T	*dii_ht;
    hashitem_T	*dii_hi;
    long_u	dii_todo;
} dictiterinfo_T;

    ਸਥਿਰ PyObject *
Dictionary_Iter_ਅਗਲਾ(ਖਾਲੀ **arg)
{
    PyObject	*ret;
    dictiterinfo_T **dii = (dictiterinfo_T**)arg;

    ਜੇ (!(*dii)->dii_todo)
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ ((*dii)->dii_ht->ht_ਬਦਲਿਆ != (*dii)->dii_ਬਦਲਿਆ)
    {
	Py_Err_ਸੈੱਟ_STRING(PyExc_RuntimeError,
		N_("hashtab changed during iteration"));
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਜਦੋਂ (((*dii)->dii_todo) && HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ((*dii)->dii_hi))
	++((*dii)->dii_hi);

    --((*dii)->dii_todo);

    ਜੇ (!(ret = Py_ਬਾਈਟਾਂ_From_String((ਅੱਖਰ *)(*dii)->dii_hi->hi_ਕੁੰਜੀ)))
	ਵਾਪਸ ਨਿਰਾ;

    ਵਾਪਸ ret;
}

    ਸਥਿਰ PyObject *
DictionaryIter(PyObject *self_obj)
{
    DictionaryObject	*self = (DictionaryObject*)self_obj;
    dictiterinfo_T	*dii;
    hashtab_T		*ht;

    ਜੇ (!(dii = PyMem_New(dictiterinfo_T, 1)))
    {
	Py_Err_No_ਸਮ੍ਰਿਤੀ();
	ਵਾਪਸ ਨਿਰਾ;
    }

    ht = &self->dict->dv_hashtab;
    dii->dii_ਬਦਲਿਆ = ht->ht_ਬਦਲਿਆ;
    dii->dii_ht = ht;
    dii->dii_hi = ht->ht_array;
    dii->dii_todo = ht->ht_ਵਰਤਿਆ;

    ਵਾਪਸ IterNew(dii,
	    PyMem_Free, Dictionary_Iter_ਅਗਲਾ,
	    ਨਿਰਾ, ਨਿਰਾ, (PyObject *)self);
}

    ਸਥਿਰ ਪੂਰਨ
Dictionary_Ass_ਵਸਤੂ(
	DictionaryObject *self, PyObject *ਕੁੰਜੀ_Object, PyObject *valObject)
{
    char_u	*ਕੁੰਜੀ;
    typval_T	tv;
    dict_T	*dict = self->dict;
    dictitem_T	*di;
    PyObject	*todecref;

    ਜੇ (dict->dv_ਤਾਲਾ)
    {
	RAISE_LOCKED_DICTIONARY;
	ਵਾਪਸ -1;
    }

    ਜੇ (!(ਕੁੰਜੀ = String_ਮੰਜ਼ਿਲ_Chars(ਕੁੰਜੀ_Object, &todecref)))
	ਵਾਪਸ -1;

    ਜੇ (*ਕੁੰਜੀ == NUL)
    {
	RAISE_NO_ਖਾਲੀ_ਥਾਵਾਂ_ਕੁੰਜੀਆਂ;
	Py_XDECREF(todecref);
	ਵਾਪਸ -1;
    }

    di = dict_ਲੱਭੋ(dict, ਕੁੰਜੀ, -1);

    ਜੇ (valObject == ਨਿਰਾ)
    {
	hashitem_T	*hi;

	ਜੇ (di == ਨਿਰਾ)
	{
	    Py_XDECREF(todecref);
	    Py_Err_ਸੈੱਟ_Object(Py_Exc_ਕੁੰਜੀ_Error, ਕੁੰਜੀ_Object);
	    ਵਾਪਸ -1;
	}
	hi = hash_ਲੱਭੋ(&dict->dv_hashtab, di->di_ਕੁੰਜੀ);
	hash_remove(&dict->dv_hashtab, hi, "Python remove item");
	dictitem_free(di);
	Py_XDECREF(todecref);
	ਵਾਪਸ 0;
    }

    ਜੇ (ConvertFromPyObject(valObject, &tv) == -1)
    {
	Py_XDECREF(todecref);
	ਵਾਪਸ -1;
    }

    ਜੇ (di == ਨਿਰਾ)
    {
	ਜੇ (!(di = dictitem_alloc(ਕੁੰਜੀ)))
	{
	    Py_XDECREF(todecref);
	    Py_Err_No_ਸਮ੍ਰਿਤੀ();
	    ਵਾਪਸ -1;
	}
	di->di_tv.v_type = VAR_UNKNOWN;

	ਜੇ (dict_ਜੋੜੋ(dict, di) == FAIL)
	{
	    dictitem_free(di);
	    RAISE_ਕੁੰਜੀ_ਜੋੜੋ_FAIL(ਕੁੰਜੀ);
	    Py_XDECREF(todecref);
	    ਵਾਪਸ -1;
	}
    }
    ਨਹੀਂਤਾਂ
	clear_tv(&di->di_tv);

    Py_XDECREF(todecref);

    copy_tv(&tv, &di->di_tv);
    clear_tv(&tv);
    ਵਾਪਸ 0;
}

ਕਿਸਮਪਰਿਭਾਸ਼ਾ PyObject *(*hi_ਮੰਜ਼ਿਲ_py)(hashitem_T *);

    ਸਥਿਰ PyObject *
DictionaryListObjects(DictionaryObject *self, hi_ਮੰਜ਼ਿਲ_py hiconvert)
{
    dict_T	*dict = self->dict;
    long_u	todo = dict->dv_hashtab.ht_ਵਰਤਿਆ;
    Py_ssize_t	i = 0;
    PyObject	*ret;
    hashitem_T	*hi;
    PyObject	*newObj;

    ret = PyList_New(todo);
    ਲਈ (hi = dict->dv_hashtab.ht_array; todo > 0; ++hi)
    {
	ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	{
	    ਜੇ (!(newObj = hiconvert(hi)))
	    {
		Py_DECREF(ret);
		ਵਾਪਸ ਨਿਰਾ;
	    }
	    Py_List_ਸੈੱਟ_ਵਸਤੂ(ret, i, newObj);
	    --todo;
	    ++i;
	}
    }
    ਵਾਪਸ ret;
}

    ਸਥਿਰ PyObject *
dict_ਕੁੰਜੀ(hashitem_T *hi)
{
    ਵਾਪਸ Py_ਬਾਈਟਾਂ_From_String((ਅੱਖਰ *)(hi->hi_ਕੁੰਜੀ));
}

    ਸਥਿਰ PyObject *
Dictionary_List_ਕੁੰਜੀਆਂ(DictionaryObject *self, PyObject *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ DictionaryListObjects(self, dict_ਕੁੰਜੀ);
}

    ਸਥਿਰ PyObject *
dict_val(hashitem_T *hi)
{
    dictitem_T	*di;

    di = dict_lookup(hi);
    ਵਾਪਸ Convert_ਮੰਜ਼ਿਲ_Py_Object(&di->di_tv);
}

    ਸਥਿਰ PyObject *
Dictionary_List_ਮੁੱਲਾਂ(DictionaryObject *self, PyObject *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ DictionaryListObjects(self, dict_val);
}

    ਸਥਿਰ PyObject *
dict_ਵਸਤੂ(hashitem_T *hi)
{
    PyObject	*ਕੁੰਜੀ_Object;
    PyObject	*valObject;
    PyObject	*ret;

    ਜੇ (!(ਕੁੰਜੀ_Object = dict_ਕੁੰਜੀ(hi)))
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (!(valObject = dict_val(hi)))
    {
	Py_DECREF(ਕੁੰਜੀ_Object);
	ਵਾਪਸ ਨਿਰਾ;
    }

    ret = Py_Build_ਮੁੱਲ("(OO)", ਕੁੰਜੀ_Object, valObject);

    Py_DECREF(ਕੁੰਜੀ_Object);
    Py_DECREF(valObject);

    ਵਾਪਸ ret;
}

    ਸਥਿਰ PyObject *
Dictionary_List_ਚੀਜ਼ਾਂ(DictionaryObject *self, PyObject *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ DictionaryListObjects(self, dict_ਵਸਤੂ);
}

    ਸਥਿਰ PyObject *
DictionaryUpdate(DictionaryObject *self, PyObject *ਦਲੀਲਾਂ, PyObject *kwargs)
{
    dict_T	*dict = self->dict;

    ਜੇ (dict->dv_ਤਾਲਾ)
    {
	RAISE_LOCKED_DICTIONARY;
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਜੇ (kwargs)
    {
	typval_T	tv;

	ਜੇ (ConvertFromPyMapping(kwargs, &tv) == -1)
	    ਵਾਪਸ ਨਿਰਾ;

	Vim_Try_ਸ਼ੁਰੂ();
	dict_extend(self->dict, tv.vval.v_dict, (char_u *) "force", ਨਿਰਾ);
	clear_tv(&tv);
	ਜੇ (Vim_Try_ਸਿਰਾ())
	    ਵਾਪਸ ਨਿਰਾ;
    }
    ਨਹੀਂਤਾਂ
    {
	PyObject	*obj = ਨਿਰਾ;

	ਜੇ (!PyArg_ParseTuple(ਦਲੀਲਾਂ, "|O", &obj))
	    ਵਾਪਸ ਨਿਰਾ;

	ਜੇ (obj == ਨਿਰਾ)
	{
	    Py_INCREF(Py_None);
	    ਵਾਪਸ Py_None;
	}

	ਜੇ (PyObject_HasAttrString(obj, "keys"))
	    ਵਾਪਸ DictionaryUpdate(self, ਨਿਰਾ, obj);
	ਨਹੀਂਤਾਂ
	{
	    PyObject	*iterator;
	    PyObject	*ਵਸਤੂ;

	    ਜੇ (!(iterator = PyObject_GetIter(obj)))
		ਵਾਪਸ ਨਿਰਾ;

	    ਜਦੋਂ ((ਵਸਤੂ = Py_Iter_ਅਗਲਾ(iterator)))
	    {
		PyObject	*fast;
		PyObject	*ਕੁੰਜੀ_Object;
		PyObject	*valObject;
		PyObject	*todecref;
		char_u		*ਕੁੰਜੀ;
		dictitem_T	*di;
		hashitem_T	*hi;

		ਜੇ (!(fast = PySequence_Fast(ਵਸਤੂ, "")))
		{
		    Py_DECREF(iterator);
		    Py_DECREF(ਵਸਤੂ);
		    ਵਾਪਸ ਨਿਰਾ;
		}

		Py_DECREF(ਵਸਤੂ);

		ਜੇ (Py_Sequence_ਮਾਪ(fast) != 2)
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    PyErr_FORMAT(Py_Exc_ਮੁੱਲ_Error,
			    N_("expected sequence element of size 2, "
			    "but got sequence of size %d"),
			    (ਪੂਰਨ) Py_Sequence_ਮਾਪ(fast));
		    ਵਾਪਸ ਨਿਰਾ;
		}

		ਕੁੰਜੀ_Object = Py_Sequence_Get_ਵਸਤੂ(fast, 0);

		ਜੇ (!(ਕੁੰਜੀ = String_ਮੰਜ਼ਿਲ_Chars(ਕੁੰਜੀ_Object, &todecref)))
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    ਵਾਪਸ ਨਿਰਾ;
		}

		di = dictitem_alloc(ਕੁੰਜੀ);

		Py_XDECREF(todecref);

		ਜੇ (di == ਨਿਰਾ)
		{
		    Py_DECREF(fast);
		    Py_DECREF(iterator);
		    Py_Err_No_ਸਮ੍ਰਿਤੀ();
		    ਵਾਪਸ ਨਿਰਾ;
		}
		di->di_tv.v_type = VAR_UNKNOWN;

		valObject = Py_Sequence_Get_ਵਸਤੂ(fast, 1);

		ਜੇ (ConvertFromPyObject(valObject, &di->di_tv) == -1)
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    dictitem_free(di);
		    ਵਾਪਸ ਨਿਰਾ;
		}

		Py_DECREF(fast);

		hi = hash_ਲੱਭੋ(&dict->dv_hashtab, di->di_ਕੁੰਜੀ);
		ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi) || dict_ਜੋੜੋ(dict, di) == FAIL)
		{
		    RAISE_ਕੁੰਜੀ_ਜੋੜੋ_FAIL(di->di_ਕੁੰਜੀ);
		    Py_DECREF(iterator);
		    dictitem_free(di);
		    ਵਾਪਸ ਨਿਰਾ;
		}
	    }

	    Py_DECREF(iterator);

	    // Iterator may have finished due to an exception
	    ਜੇ (PyErr_Occurred())
		ਵਾਪਸ ਨਿਰਾ;
	}
    }
    Py_INCREF(Py_None);
    ਵਾਪਸ Py_None;
}

    ਸਥਿਰ PyObject *
DictionaryGet(DictionaryObject *self, PyObject *ਦਲੀਲਾਂ)
{
    ਵਾਪਸ Dictionary_ਵਸਤੂ(self, ਦਲੀਲਾਂ,
			    DICT_FLAG_HAS_DEFAULT|DICT_FLAG_NONE_DEFAULT);
}

    ਸਥਿਰ PyObject *
Dictionary_ਕੱਢੋ(DictionaryObject *self, PyObject *ਦਲੀਲਾਂ)
{
    ਵਾਪਸ Dictionary_ਵਸਤੂ(self, ਦਲੀਲਾਂ, DICT_FLAG_HAS_DEFAULT|DICT_FLAG_ਕੱਢੋ);
}

    ਸਥਿਰ PyObject *
Dictionary_ਕੱਢੋ_ਵਸਤੂ(DictionaryObject *self, PyObject *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    hashitem_T	*hi;
    PyObject	*ret;
    PyObject	*valObject;
    dictitem_T	*di;

    ਜੇ (self->dict->dv_hashtab.ht_ਵਰਤਿਆ == 0)
    {
	Py_Err_ਸੈੱਟ_None(Py_Exc_ਕੁੰਜੀ_Error);
	ਵਾਪਸ ਨਿਰਾ;
    }

    hi = self->dict->dv_hashtab.ht_array;
    ਜਦੋਂ (HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	++hi;

    di = dict_lookup(hi);

    ਜੇ (!(valObject = Convert_ਮੰਜ਼ਿਲ_Py_Object(&di->di_tv)))
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (!(ret = Py_Build_ਮੁੱਲ("(" Py_ਬਾਈਟਾਂ_fmt "O)", hi->hi_ਕੁੰਜੀ, valObject)))
    {
	Py_DECREF(valObject);
	ਵਾਪਸ ਨਿਰਾ;
    }

    hash_remove(&self->dict->dv_hashtab, hi, "Python pop item");
    dictitem_free(di);

    ਵਾਪਸ ret;
}

    ਸਥਿਰ PyObject *
Dictionary_Has_ਕੁੰਜੀ(DictionaryObject *self, PyObject *ਕੁੰਜੀ_Object)
{
    ਵਾਪਸ Dictionary_ਵਸਤੂ(self, ਕੁੰਜੀ_Object, DICT_FLAG_RETURN_BOOL);
}

ਸਥਿਰ PySequenceMethods DictionaryAsSeq = {
    0,					// sq_length
    0,					// sq_concat
    0,					// sq_repeat
    0,					// sq_item
    0,					// sq_slice
    0,					// sq_ass_item
    0,					// sq_ass_slice
    (objobjproc) Dictionary_ਸ਼ਾਮਲ_ਹੈ,	// sq_contains
    0,					// sq_inplace_concat
    0,					// sq_inplace_repeat
};

ਸਥਿਰ PyMappingMethods DictionaryAsMapping = {
    (lenfunc)       Dictionary_ਲੰਮਾਈ,
    (binaryfunc)    Dictionary_ਵਸਤੂ_2,
    (objobjargproc) Dictionary_Ass_ਵਸਤੂ,
};

ਸਥਿਰ ਬਣਤਰ Py_ਵਿਧੀ_Def DictionaryMethods[] = {
    {"keys",	(PyCFunction)Dictionary_List_ਕੁੰਜੀਆਂ,	METH_NOARGS,	""},
    {"values",	(PyCFunction)Dictionary_List_ਮੁੱਲਾਂ,	METH_NOARGS,	""},
    {"items",	(PyCFunction)Dictionary_List_ਚੀਜ਼ਾਂ,	METH_NOARGS,	""},
    {"update",	(PyCFunction)(ਖਾਲੀ *)DictionaryUpdate,		METH_VARARGS|METH_KEYWORDS, ""},
    {"get",	(PyCFunction)DictionaryGet,		METH_VARARGS,	""},
    {"pop",	(PyCFunction)Dictionary_ਕੱਢੋ,		METH_VARARGS,	""},
    {"popitem",	(PyCFunction)Dictionary_ਕੱਢੋ_ਵਸਤੂ,		METH_NOARGS,	""},
    {"has_key",	(PyCFunction)Dictionary_Has_ਕੁੰਜੀ,		METH_O,		""},
    {"__dir__",	(PyCFunction)DictionaryDir,		METH_NOARGS,	""},
    { ਨਿਰਾ,	ਨਿਰਾ,					0,		ਨਿਰਾ}
};

DEFINE_PY_TYPE_OBJECT(ListType);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    Py_Object_ਸਿਰ
    list_T	*list;
    pylinkedlist_T	ref;
} ListObject;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NEW_LIST(list) ListNew(ListTypePtr, list)

    ਸਥਿਰ PyObject *
ListNew(PyTypeObject *subtype, list_T *list)
{
    ListObject	*self;

    ਜੇ (list == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    self = (ListObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    ਜੇ (self == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    self->list = list;
    ++list->lv_refcount;
    CHECK_LIST_MATERIALIZE(list);

    pyll_ਜੋੜੋ((PyObject *)(self), &self->ref, &lastlist);

    ਵਾਪਸ (PyObject *)(self);
}

    ਸਥਿਰ list_T *
py_list_alloc(ਖਾਲੀ)
{
    list_T	*ret;

    ਜੇ (!(ret = list_alloc()))
    {
	Py_Err_No_ਸਮ੍ਰਿਤੀ();
	ਵਾਪਸ ਨਿਰਾ;
    }
    ++ret->lv_refcount;

    ਵਾਪਸ ret;
}

    ਸਥਿਰ ਪੂਰਨ
list_py_concat(list_T *l, PyObject *obj, PyObject *lookup_dict)
{
    PyObject	*iterator;
    PyObject	*ਵਸਤੂ;
    listitem_T	*li;

    ਜੇ (!(iterator = PyObject_GetIter(obj)))
	ਵਾਪਸ -1;

    ਜਦੋਂ ((ਵਸਤੂ = Py_Iter_ਅਗਲਾ(iterator)))
    {
	ਜੇ (!(li = listitem_alloc()))
	{
	    Py_Err_No_ਸਮ੍ਰਿਤੀ();
	    Py_DECREF(ਵਸਤੂ);
	    Py_DECREF(iterator);
	    ਵਾਪਸ -1;
	}
	li->li_tv.v_ਤਾਲਾ = 0;
	li->li_tv.v_type = VAR_UNKNOWN;

	ਜੇ (_ConvertFromPyObject(ਵਸਤੂ, &li->li_tv, lookup_dict) == -1)
	{
	    Py_DECREF(ਵਸਤੂ);
	    Py_DECREF(iterator);
	    listitem_free(l, li);
	    ਵਾਪਸ -1;
	}

	Py_DECREF(ਵਸਤੂ);

	list_append(l, li);
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    ਜੇ (PyErr_Occurred())
	ਵਾਪਸ -1;

    ਵਾਪਸ 0;
}

    ਸਥਿਰ PyObject *
ListConstructor(PyTypeObject *subtype, PyObject *ਦਲੀਲਾਂ, PyObject *kwargs)
{
    list_T	*list;
    PyObject	*obj = ਨਿਰਾ;

    ਜੇ (kwargs)
    {
	Py_Err_ਸੈੱਟ_STRING(PyExc_TypeError,
		N_("list constructor does not accept keyword arguments"));
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਜੇ (!PyArg_ParseTuple(ਦਲੀਲਾਂ, "|O", &obj))
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (!(list = py_list_alloc()))
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (obj)
    {
	PyObject	*lookup_dict;

	ਜੇ (!(lookup_dict = PyDict_New()))
	{
	    list_unref(list);
	    ਵਾਪਸ ਨਿਰਾ;
	}

	ਜੇ (list_py_concat(list, obj, lookup_dict) == -1)
	{
	    Py_DECREF(lookup_dict);
	    list_unref(list);
	    ਵਾਪਸ ਨਿਰਾ;
	}

	Py_DECREF(lookup_dict);
    }

    ਵਾਪਸ ListNew(subtype, list);
}

    ਸਥਿਰ ਖਾਲੀ
ListDestructor(PyObject *self_obj)
{
    ListObject *self = (ListObject*)self_obj;
    pyll_remove(&self->ref, &lastlist);
    list_unref(self->list);

    DESTRUCTOR_ਅੰਤ(self);
}

    ਸਥਿਰ PyInt
List_ਲੰਮਾਈ(ListObject *self)
{
    ਵਾਪਸ ((PyInt) (self->list->lv_len));
}

    ਸਥਿਰ PyObject *
List_ਸੂਚਕਾਂਕ(ListObject *self, Py_ssize_t ਸੂਚਕਾਂਕ)
{
    listitem_T	*li;

    ਜੇ (ਸੂਚਕਾਂਕ >= List_ਲੰਮਾਈ(self))
    {
	Py_Err_ਸੈੱਟ_STRING(Py_Exc_ਸੂਚਕਾਂਕ_Error, N_("list index out of range"));
	ਵਾਪਸ ਨਿਰਾ;
    }
    li = list_ਲੱਭੋ(self->list, (ਲੰਮਾ) ਸੂਚਕਾਂਕ);
    ਜੇ (li == ਨਿਰਾ)
    {
	// No more suitable format specifications in python-2.3
	PyErr_VIM_FORMAT(N_("internal error: failed to get Vim list item %d"),
		(ਪੂਰਨ) ਸੂਚਕਾਂਕ);
	ਵਾਪਸ ਨਿਰਾ;
    }
    ਵਾਪਸ Convert_ਮੰਜ਼ਿਲ_Py_Object(&li->li_tv);
}

    ਸਥਿਰ PyObject *
List_ਟੁਕੜਾ(ListObject *self, Py_ssize_t ਪਹਿਲਾ, Py_ssize_t ਕਦਮ,
	  Py_ssize_t slicelen)
{
    PyInt	i;
    PyObject	*list;

    ਜੇ (ਕਦਮ == 0)
    {
	Py_Err_ਸੈੱਟ_STRING(Py_Exc_ਮੁੱਲ_Error, N_("slice step cannot be zero"));
	ਵਾਪਸ ਨਿਰਾ;
    }

    list = PyList_New(slicelen);
    ਜੇ (list == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ਲਈ (i = 0; i < slicelen; ++i)
    {
	PyObject	*ਵਸਤੂ;

	ਵਸਤੂ = List_ਸੂਚਕਾਂਕ(self, ਪਹਿਲਾ + i*ਕਦਮ);
	ਜੇ (ਵਸਤੂ == ਨਿਰਾ)
	{
	    Py_DECREF(list);
	    ਵਾਪਸ ਨਿਰਾ;
	}

	Py_List_ਸੈੱਟ_ਵਸਤੂ(list, i, ਵਸਤੂ);
    }

    ਵਾਪਸ list;
}

    ਸਥਿਰ PyObject *
List_ਵਸਤੂ(ListObject *self, PyObject* idx)
{
#ਜੇ PY_MAJOR_ਸੰਸਕਰਣ < 3
    ਜੇ (PyInt_Check(idx))
    {
	ਲੰਮਾ _idx = PyInt_AsLong(idx);
	ਵਾਪਸ List_ਸੂਚਕਾਂਕ(self, _idx);
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    ਜੇ (PyLong_Check(idx))
    {
	ਲੰਮਾ _idx = PyLong_AsLong(idx);
	ਵਾਪਸ List_ਸੂਚਕਾਂਕ(self, _idx);
    }
    ਨਹੀਂਤਾਂ ਜੇ (Py_ਟੁਕੜਾ_Check(idx))
    {
	Py_ssize_t ਸ਼ੁਰੂ, stop, ਕਦਮ, slicelen;

	ਜੇ (Py_ਟੁਕੜਾ_Get_Indices_Ex((Py_ਟੁਕੜਾ_Object_T *)idx, List_ਲੰਮਾਈ(self),
				 &ਸ਼ੁਰੂ, &stop, &ਕਦਮ, &slicelen) < 0)
	    ਵਾਪਸ ਨਿਰਾ;
	ਵਾਪਸ List_ਟੁਕੜਾ(self, ਸ਼ੁਰੂ, ਕਦਮ, slicelen);
    }
    ਨਹੀਂਤਾਂ
    {
	RAISE_INVALID_ਸੂਚਕਾਂਕ_TYPE(idx);
	ਵਾਪਸ ਨਿਰਾ;
    }
}

    ਸਥਿਰ ਖਾਲੀ
list_restore(Py_ssize_t numadded, Py_ssize_t numreplaced, Py_ssize_t slicelen,
	list_T *l, listitem_T **lis, listitem_T *lastaddedli)
{
    ਜਦੋਂ (numreplaced--)
    {
	list_ਪਾਓ(l, lis[numreplaced], lis[slicelen + numreplaced]);
	listitem_remove(l, lis[slicelen + numreplaced]);
    }
    ਜਦੋਂ (numadded--)
    {
	listitem_T	*ਅਗਲਾ;

	ਅਗਲਾ = lastaddedli->li_prev;
	listitem_remove(l, lastaddedli);
	lastaddedli = ਅਗਲਾ;
    }
}

    ਸਥਿਰ ਪੂਰਨ
List_Ass_ਟੁਕੜਾ(ListObject *self, Py_ssize_t ਪਹਿਲਾ,
	     Py_ssize_t ਕਦਮ, Py_ssize_t slicelen, PyObject *obj)
{
    PyObject	*iterator;
    PyObject	*ਵਸਤੂ;
    listitem_T	*li;
    listitem_T	*lastaddedli = ਨਿਰਾ;
    listitem_T	*ਅਗਲਾ;
    typval_T	v;
    list_T	*l = self->list;
    PyInt	i;
    PyInt	j;
    PyInt	numreplaced = 0;
    PyInt	numadded = 0;
    PyInt	ਮਾਪ;
    listitem_T	**lis = ਨਿਰਾ;

    ਮਾਪ = List_ਲੰਮਾਈ(self);

    ਜੇ (l->lv_ਤਾਲਾ)
    {
	RAISE_LOCKED_LIST;
	ਵਾਪਸ -1;
    }

    ਜੇ (ਕਦਮ == 0)
    {
	Py_Err_ਸੈੱਟ_STRING(Py_Exc_ਮੁੱਲ_Error, N_("slice step cannot be zero"));
	ਵਾਪਸ -1;
    }

    ਜੇ (ਕਦਮ != 1 && slicelen == 0)
    {
	// Nothing to do. Only error out if obj has some items.
	ਪੂਰਨ		ret = 0;

	ਜੇ (obj == ਨਿਰਾ)
	    ਵਾਪਸ 0;

	ਜੇ (!(iterator = PyObject_GetIter(obj)))
	    ਵਾਪਸ -1;

	ਜੇ ((ਵਸਤੂ = Py_Iter_ਅਗਲਾ(iterator)))
	{
	    PyErr_FORMAT(Py_Exc_ਮੁੱਲ_Error,
		    N_("attempt to assign sequence of size greater than %d "
			"to extended slice"), 0);
	    Py_DECREF(ਵਸਤੂ);
	    ret = -1;
	}
	Py_DECREF(iterator);
	ਵਾਪਸ ret;
    }

    ਜੇ (obj != ਨਿਰਾ)
	// XXX May allocate zero bytes.
	ਜੇ (!(lis = PyMem_New(listitem_T *, slicelen * 2)))
	{
	    Py_Err_No_ਸਮ੍ਰਿਤੀ();
	    ਵਾਪਸ -1;
	}

    ਜੇ (ਪਹਿਲਾ == ਮਾਪ)
	li = ਨਿਰਾ;
    ਨਹੀਂਤਾਂ
    {
	li = list_ਲੱਭੋ(l, (ਲੰਮਾ) ਪਹਿਲਾ);
	ਜੇ (li == ਨਿਰਾ)
	{
	    PyErr_VIM_FORMAT(N_("internal error: no Vim list item %d"),
		    (ਪੂਰਨ)ਪਹਿਲਾ);
	    ਜੇ (obj != ਨਿਰਾ)
		PyMem_Free(lis);
	    ਵਾਪਸ -1;
	}
	i = slicelen;
	ਜਦੋਂ (i-- && li != ਨਿਰਾ)
	{
	    j = ਕਦਮ;
	    ਅਗਲਾ = li;
	    ਜੇ (ਕਦਮ > 0)
		ਜਦੋਂ (ਅਗਲਾ != ਨਿਰਾ && ((ਅਗਲਾ = ਅਗਲਾ->li_ਅਗਲਾ) != ਨਿਰਾ) && --j);
	    ਨਹੀਂਤਾਂ
		ਜਦੋਂ (ਅਗਲਾ != ਨਿਰਾ && ((ਅਗਲਾ = ਅਗਲਾ->li_prev) != ਨਿਰਾ) && ++j);

	    ਜੇ (obj == ਨਿਰਾ)
		listitem_remove(l, li);
	    ਨਹੀਂਤਾਂ
		lis[slicelen - i - 1] = li;

	    li = ਅਗਲਾ;
	}
	ਜੇ (li == ਨਿਰਾ && i != -1)
	{
	    Py_Err_ਸੈੱਟ_VIM(N_("internal error: not enough list items"));
	    ਜੇ (obj != ਨਿਰਾ)
		PyMem_Free(lis);
	    ਵਾਪਸ -1;
	}
    }

    ਜੇ (obj == ਨਿਰਾ)
	ਵਾਪਸ 0;

    ਜੇ (!(iterator = PyObject_GetIter(obj)))
    {
	PyMem_Free(lis);
	ਵਾਪਸ -1;
    }

    i = 0;
    ਜਦੋਂ ((ਵਸਤੂ = Py_Iter_ਅਗਲਾ(iterator)))
    {
	ਜੇ (ConvertFromPyObject(ਵਸਤੂ, &v) == -1)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(ਵਸਤੂ);
	    PyMem_Free(lis);
	    ਵਾਪਸ -1;
	}
	Py_DECREF(ਵਸਤੂ);
	ਜੇ (list_ਪਾਓ_tv(l, &v, numreplaced < slicelen
				    ? lis[numreplaced]
				    : li) == FAIL)
	{
	    clear_tv(&v);
	    Py_Err_ਸੈੱਟ_VIM(N_("internal error: failed to add item to list"));
	    list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	    PyMem_Free(lis);
	    ਵਾਪਸ -1;
	}
	ਜੇ (numreplaced < slicelen)
	{
	    lis[slicelen + numreplaced] = lis[numreplaced]->li_prev;
	    vimlist_remove(l, lis[numreplaced], lis[numreplaced]);
	    numreplaced++;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (li)
		lastaddedli = li->li_prev;
	    ਨਹੀਂਤਾਂ
		lastaddedli = l->lv_u.mat.lv_ਆਖਰੀ;
	    numadded++;
	}
	clear_tv(&v);
	ਜੇ (ਕਦਮ != 1 && i >= slicelen)
	{
	    Py_DECREF(iterator);
	    PyErr_FORMAT(Py_Exc_ਮੁੱਲ_Error,
		    N_("attempt to assign sequence of size greater than %d "
			"to extended slice"), (ਪੂਰਨ) slicelen);
	    list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	    PyMem_Free(lis);
	    ਵਾਪਸ -1;
	}
	++i;
    }
    Py_DECREF(iterator);

    ਜੇ (ਕਦਮ != 1 && i != slicelen)
    {
	PyErr_FORMAT2(Py_Exc_ਮੁੱਲ_Error,
		N_("attempt to assign sequence of size %d to extended slice "
		    "of size %d"), (ਪੂਰਨ) i, (ਪੂਰਨ) slicelen);
	list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	PyMem_Free(lis);
	ਵਾਪਸ -1;
    }

    ਜੇ (PyErr_Occurred())
    {
	list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	PyMem_Free(lis);
	ਵਾਪਸ -1;
    }

    ਲਈ (i = 0; i < numreplaced; i++)
	listitem_free(l, lis[i]);
    ਜੇ (ਕਦਮ == 1)
	ਲਈ (i = numreplaced; i < slicelen; i++)
	    listitem_remove(l, lis[i]);

    PyMem_Free(lis);

    ਵਾਪਸ 0;
}

    ਸਥਿਰ ਪੂਰਨ
List_Ass_ਸੂਚਕਾਂਕ(ListObject *self, Py_ssize_t ਸੂਚਕਾਂਕ, PyObject *obj)
{
    typval_T	tv;
    list_T	*l = self->list;
    listitem_T	*li;
    Py_ssize_t	ਲੰਮਾਈ = List_ਲੰਮਾਈ(self);

    ਜੇ (l->lv_ਤਾਲਾ)
    {
	RAISE_LOCKED_LIST;
	ਵਾਪਸ -1;
    }
    ਜੇ (ਸੂਚਕਾਂਕ > ਲੰਮਾਈ || (ਸੂਚਕਾਂਕ == ਲੰਮਾਈ && obj == ਨਿਰਾ))
    {
	Py_Err_ਸੈੱਟ_STRING(Py_Exc_ਸੂਚਕਾਂਕ_Error, N_("list index out of range"));
	ਵਾਪਸ -1;
    }

    ਜੇ (obj == ਨਿਰਾ)
    {
	li = list_ਲੱਭੋ(l, (ਲੰਮਾ) ਸੂਚਕਾਂਕ);
	ਜੇ (li == ਨਿਰਾ)
	{
	    PyErr_VIM_FORMAT(N_("internal error: failed to get Vim "
			"list item %d"), (ਪੂਰਨ) ਸੂਚਕਾਂਕ);
	    ਵਾਪਸ -1;
	}
	vimlist_remove(l, li, li);
	clear_tv(&li->li_tv);
	vim_free(li);
	ਵਾਪਸ 0;
    }

    ਜੇ (ConvertFromPyObject(obj, &tv) == -1)
	ਵਾਪਸ -1;

    ਜੇ (ਸੂਚਕਾਂਕ == ਲੰਮਾਈ)
    {
	ਜੇ (list_append_tv(l, &tv) == FAIL)
	{
	    clear_tv(&tv);
	    Py_Err_ਸੈੱਟ_VIM(N_("failed to add item to list"));
	    ਵਾਪਸ -1;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	li = list_ਲੱਭੋ(l, (ਲੰਮਾ) ਸੂਚਕਾਂਕ);
	ਜੇ (li == ਨਿਰਾ)
	{
	    PyErr_VIM_FORMAT(N_("internal error: failed to get Vim "
			"list item %d"), (ਪੂਰਨ) ਸੂਚਕਾਂਕ);
	    ਵਾਪਸ -1;
	}
	clear_tv(&li->li_tv);
	copy_tv(&tv, &li->li_tv);
	clear_tv(&tv);
    }
    ਵਾਪਸ 0;
}

    ਸਥਿਰ ਪੂਰਨ
List_Ass_ਵਸਤੂ(ListObject *self, PyObject *idx, PyObject *obj)
{
#ਜੇ PY_MAJOR_ਸੰਸਕਰਣ < 3
    ਜੇ (PyInt_Check(idx))
    {
	ਲੰਮਾ _idx = PyInt_AsLong(idx);
	ਵਾਪਸ (ਪੂਰਨ)List_Ass_ਸੂਚਕਾਂਕ(self, _idx, obj);
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    ਜੇ (PyLong_Check(idx))
    {
	ਲੰਮਾ _idx = PyLong_AsLong(idx);
	ਵਾਪਸ (ਪੂਰਨ)List_Ass_ਸੂਚਕਾਂਕ(self, _idx, obj);
    }
    ਨਹੀਂਤਾਂ ਜੇ (Py_ਟੁਕੜਾ_Check(idx))
    {
	Py_ssize_t ਸ਼ੁਰੂ, stop, ਕਦਮ, slicelen;

	ਜੇ (Py_ਟੁਕੜਾ_Get_Indices_Ex((Py_ਟੁਕੜਾ_Object_T *)idx, List_ਲੰਮਾਈ(self),
				 &ਸ਼ੁਰੂ, &stop, &ਕਦਮ, &slicelen) < 0)
	    ਵਾਪਸ -1;
	ਵਾਪਸ (ਪੂਰਨ)List_Ass_ਟੁਕੜਾ(self, ਸ਼ੁਰੂ, ਕਦਮ, slicelen,
		obj);
    }
    ਨਹੀਂਤਾਂ
    {
	RAISE_INVALID_ਸੂਚਕਾਂਕ_TYPE(idx);
	ਵਾਪਸ -1;
    }
}

    ਸਥਿਰ PyObject *
List_Concat_ਵਿੱਚ_ਰੱਖੋ(ListObject *self, PyObject *obj)
{
    list_T	*l = self->list;
    PyObject	*lookup_dict;

    ਜੇ (l->lv_ਤਾਲਾ)
    {
	RAISE_LOCKED_LIST;
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਜੇ (!(lookup_dict = PyDict_New()))
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (list_py_concat(l, obj, lookup_dict) == -1)
    {
	Py_DECREF(lookup_dict);
	ਵਾਪਸ ਨਿਰਾ;
    }
    Py_DECREF(lookup_dict);

    Py_INCREF((PyObject *)self);
    ਵਾਪਸ (PyObject *)(self);
}

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    listwatch_T	lw;
    list_T	*list;
} listiterinfo_T;

    ਸਥਿਰ ਖਾਲੀ
ListIterDestruct(ਖਾਲੀ *arg)
{
    listiterinfo_T *lii = (listiterinfo_T*)arg;
    list_rem_watch(lii->list, &lii->lw);
    list_unref(lii->list);
    PyMem_Free(lii);
}

    ਸਥਿਰ PyObject *
List_Iter_ਅਗਲਾ(ਖਾਲੀ **arg)
{
    PyObject	*ret;
    listiterinfo_T **lii = (listiterinfo_T**)arg;

    ਜੇ (!((*lii)->lw.lw_ਵਸਤੂ))
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (!(ret = Convert_ਮੰਜ਼ਿਲ_Py_Object(&((*lii)->lw.lw_ਵਸਤੂ->li_tv))))
	ਵਾਪਸ ਨਿਰਾ;

    (*lii)->lw.lw_ਵਸਤੂ = (*lii)->lw.lw_ਵਸਤੂ->li_ਅਗਲਾ;

    ਵਾਪਸ ret;
}

    ਸਥਿਰ PyObject *
ListIter(PyObject *self_obj)
{
    ListObject	*self = (ListObject*)self_obj;
    listiterinfo_T	*lii;
    list_T	*l = self->list;

    ਜੇ (!(lii = PyMem_New(listiterinfo_T, 1)))
    {
	Py_Err_No_ਸਮ੍ਰਿਤੀ();
	ਵਾਪਸ ਨਿਰਾ;
    }

    CHECK_LIST_MATERIALIZE(l);
    list_ਜੋੜੋ_watch(l, &lii->lw);
    lii->lw.lw_ਵਸਤੂ = l->lv_ਪਹਿਲਾ;
    lii->list = l;
    ++l->lv_refcount;

    ਵਾਪਸ IterNew(lii,
	    ListIterDestruct, List_Iter_ਅਗਲਾ,
	    ਨਿਰਾ, ਨਿਰਾ, (PyObject *)self);
}

ਸਥਿਰ ਅੱਖਰ *ListAttrs[] = {
    "locked",
    ਨਿਰਾ
};

    ਸਥਿਰ PyObject *
ListDir(PyObject *self, PyObject *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ ObjectDir(self, ListAttrs);
}

    ਸਥਿਰ ਪੂਰਨ
ListSetattr(PyObject *self_obj, ਅੱਖਰ *name, PyObject *valObject)
{
    ListObject *self = (ListObject*)self_obj;
    ਜੇ (valObject == ਨਿਰਾ)
    {
	Py_Err_ਸੈੱਟ_STRING(PyExc_AttributeError,
		N_("cannot delete vim.List attributes"));
	ਵਾਪਸ -1;
    }

    ਜੇ (strcmp(name, "locked") == 0)
    {
	ਜੇ (self->list->lv_ਤਾਲਾ == VAR_FIXED)
	{
	    Py_Err_ਸੈੱਟ_STRING(PyExc_TypeError, N_("cannot modify fixed list"));
	    ਵਾਪਸ -1;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਪੂਰਨ		istrue = PyObject_IsTrue(valObject);
	    ਜੇ (istrue == -1)
		ਵਾਪਸ -1;
	    ਨਹੀਂਤਾਂ ਜੇ (istrue)
		self->list->lv_ਤਾਲਾ = VAR_LOCKED;
	    ਨਹੀਂਤਾਂ
		self->list->lv_ਤਾਲਾ = 0;
	}
	ਵਾਪਸ 0;
    }
    ਨਹੀਂਤਾਂ
    {
	PyErr_FORMAT(PyExc_AttributeError, N_("cannot set attribute %s"), name);
	ਵਾਪਸ -1;
    }
}

ਸਥਿਰ PySequenceMethods ListAsSeq = {
    (lenfunc)		List_ਲੰਮਾਈ,	 // sq_length,	  len(x)
    (binaryfunc)	0,		 // RangeConcat, sq_concat,  x+y
    0,					 // RangeRepeat, sq_repeat,  x*n
    (PyIntArgFunc)	List_ਸੂਚਕਾਂਕ,	 // sq_item,	  x[i]
    0,					 // was_sq_slice,     x[i:j]
    (PyIntObjArgProc)	List_Ass_ਸੂਚਕਾਂਕ,	 // sq_as_item,  x[i]=v
    0,					 // was_sq_ass_slice, x[i:j]=v
    0,					 // sq_contains
    (binaryfunc)	List_Concat_ਵਿੱਚ_ਰੱਖੋ,// sq_inplace_concat
    0,					 // sq_inplace_repeat
};

ਸਥਿਰ PyMappingMethods ListAsMapping = {
    /* mp_length	*/ (lenfunc) List_ਲੰਮਾਈ,
    /* mp_subscript     */ (binaryfunc) List_ਵਸਤੂ,
    /* mp_ass_subscript */ (objobjargproc) List_Ass_ਵਸਤੂ,
};

ਸਥਿਰ ਬਣਤਰ Py_ਵਿਧੀ_Def ListMethods[] = {
    {"extend",	(PyCFunction)List_Concat_ਵਿੱਚ_ਰੱਖੋ,	METH_O,		""},
    {"__dir__",	(PyCFunction)ListDir,		METH_NOARGS,	""},
    { ਨਿਰਾ,	ਨਿਰਾ,				0,		ਨਿਰਾ}
};

DEFINE_PY_TYPE_OBJECT(TupleType);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    Py_Object_ਸਿਰ
    tuple_T	*tuple;
    pylinkedlist_T	ref;
} TupleObject;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NEW_TUPLE(tuple) TupleNew(TupleTypePtr, tuple)

    ਸਥਿਰ PyObject *
TupleNew(PyTypeObject *subtype, tuple_T *tuple)
{
    TupleObject	*self;

    ਜੇ (tuple == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    self = (TupleObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    ਜੇ (self == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    self->tuple = tuple;
    ++tuple->tv_refcount;

    pyll_ਜੋੜੋ((PyObject *)(self), &self->ref, &lasttuple);

    ਵਾਪਸ (PyObject *)(self);
}

    ਸਥਿਰ tuple_T *
py_tuple_alloc(ਖਾਲੀ)
{
    tuple_T	*ret;

    ਜੇ (!(ret = tuple_alloc()))
    {
	Py_Err_No_ਸਮ੍ਰਿਤੀ();
	ਵਾਪਸ ਨਿਰਾ;
    }
    ++ret->tv_refcount;

    ਵਾਪਸ ret;
}

    ਸਥਿਰ ਪੂਰਨ
tuple_py_concat(tuple_T *t, PyObject *obj, PyObject *lookup_dict)
{
    PyObject	*iterator;
    PyObject	*ਵਸਤੂ;

    ਜੇ (!(iterator = PyObject_GetIter(obj)))
	ਵਾਪਸ -1;

    ਜਦੋਂ ((ਵਸਤੂ = Py_Iter_ਅਗਲਾ(iterator)))
    {
	typval_T    new_tv;

	ਜੇ (_ConvertFromPyObject(ਵਸਤੂ, &new_tv, lookup_dict) == -1)
	{
	    Py_DECREF(ਵਸਤੂ);
	    Py_DECREF(iterator);
	    ਵਾਪਸ -1;
	}

	Py_DECREF(ਵਸਤੂ);

	ਜੇ (tuple_append_tv(t, &new_tv) == FAIL)
	{
	    Py_DECREF(iterator);
	    ਵਾਪਸ -1;
	}
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    ਜੇ (PyErr_Occurred())
	ਵਾਪਸ -1;

    ਵਾਪਸ 0;
}

    ਸਥਿਰ PyObject *
TupleConstructor(PyTypeObject *subtype, PyObject *ਦਲੀਲਾਂ, PyObject *kwargs)
{
    tuple_T	*tuple;
    PyObject	*obj = ਨਿਰਾ;

    ਜੇ (kwargs)
    {
	Py_Err_ਸੈੱਟ_STRING(PyExc_TypeError,
		N_("tuple constructor does not accept keyword arguments"));
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਜੇ (!PyArg_ParseTuple(ਦਲੀਲਾਂ, "|O", &obj))
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (!(tuple = py_tuple_alloc()))
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (obj)
    {
	PyObject	*lookup_dict;

	ਜੇ (!(lookup_dict = PyDict_New()))
	{
	    tuple_unref(tuple);
	    ਵਾਪਸ ਨਿਰਾ;
	}

	ਜੇ (tuple_py_concat(tuple, obj, lookup_dict) == -1)
	{
	    Py_DECREF(lookup_dict);
	    tuple_unref(tuple);
	    ਵਾਪਸ ਨਿਰਾ;
	}

	Py_DECREF(lookup_dict);
    }

    ਵਾਪਸ TupleNew(subtype, tuple);
}

    ਸਥਿਰ ਖਾਲੀ
TupleDestructor(PyObject *self_obj)
{
    TupleObject *self = (TupleObject *)self_obj;
    pyll_remove(&self->ref, &lasttuple);
    tuple_unref(self->tuple);

    DESTRUCTOR_ਅੰਤ(self);
}

    ਸਥਿਰ PyInt
Tuple_ਲੰਮਾਈ(TupleObject *self)
{
    ਵਾਪਸ ((PyInt)(tuple_len(self->tuple)));
}

    ਸਥਿਰ PyObject *
Tuple_ਸੂਚਕਾਂਕ(TupleObject *self, Py_ssize_t ਸੂਚਕਾਂਕ)
{
    PyInt	len = Tuple_ਲੰਮਾਈ(self);

    ਜੇ (ਸੂਚਕਾਂਕ < 0)
	ਸੂਚਕਾਂਕ = len + ਸੂਚਕਾਂਕ;

    ਜੇ (ਸੂਚਕਾਂਕ < 0 || ਸੂਚਕਾਂਕ >= len)
    {
	Py_Err_ਸੈੱਟ_STRING(Py_Exc_ਸੂਚਕਾਂਕ_Error, N_("tuple index out of range"));
	ਵਾਪਸ ਨਿਰਾ;
    }
    ਵਾਪਸ Convert_ਮੰਜ਼ਿਲ_Py_Object(TUPLE_ਵਸਤੂ(self->tuple, ਸੂਚਕਾਂਕ));
}

/*
 * Return a new tuple object for the tuple slice starting from the index
 * "first" and of length "slicelen" skipping "step" items.
 */
    ਸਥਿਰ PyObject *
Tuple_ਟੁਕੜਾ(
    TupleObject		*self,
    Py_ssize_t		ਪਹਿਲਾ,
    Py_ssize_t		ਕਦਮ,
    Py_ssize_t		slicelen)
{
    PyInt	i;
    PyObject	*tuple;

    tuple = PyTuple_New(slicelen);
    ਜੇ (tuple == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ਲਈ (i = 0; i < slicelen; ++i)
    {
	PyObject	*ਵਸਤੂ;

	ਵਸਤੂ = Tuple_ਸੂਚਕਾਂਕ(self, ਪਹਿਲਾ + i * ਕਦਮ);
	ਜੇ (ਵਸਤੂ == ਨਿਰਾ)
	{
	    Py_DECREF(tuple);
	    ਵਾਪਸ ਨਿਰਾ;
	}

	Py_Tuple_ਸੈੱਟ_ਵਸਤੂ(tuple, i, ਵਸਤੂ);
    }

    ਵਾਪਸ tuple;
}

    ਸਥਿਰ PyObject *
Tuple_ਵਸਤੂ(TupleObject *self, PyObject* idx)
{
#ਜੇ PY_MAJOR_ਸੰਸਕਰਣ < 3
    ਜੇ (PyInt_Check(idx))
    {
	ਲੰਮਾ _idx = PyInt_AsLong(idx);
	ਵਾਪਸ Tuple_ਸੂਚਕਾਂਕ(self, _idx);
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    ਜੇ (PyLong_Check(idx))
    {
	ਲੰਮਾ _idx = PyLong_AsLong(idx);
	ਵਾਪਸ Tuple_ਸੂਚਕਾਂਕ(self, _idx);
    }
    ਨਹੀਂਤਾਂ ਜੇ (Py_ਟੁਕੜਾ_Check(idx))
    {
	Py_ssize_t ਸ਼ੁਰੂ, stop, ਕਦਮ, slicelen;

	ਜੇ (Py_ਟੁਕੜਾ_Get_Indices_Ex((Py_ਟੁਕੜਾ_Object_T *)idx, Tuple_ਲੰਮਾਈ(self),
				 &ਸ਼ੁਰੂ, &stop, &ਕਦਮ, &slicelen) < 0)
	    ਵਾਪਸ ਨਿਰਾ;
	ਵਾਪਸ Tuple_ਟੁਕੜਾ(self, ਸ਼ੁਰੂ, ਕਦਮ, slicelen);
    }
    ਨਹੀਂਤਾਂ
    {
	RAISE_INVALID_ਸੂਚਕਾਂਕ_TYPE(idx);
	ਵਾਪਸ ਨਿਰਾ;
    }
}

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    tuple_T	*tuple;
    ਪੂਰਨ		ਸੂਚਕਾਂਕ;
} tupleiterinfo_T;

    ਸਥਿਰ ਖਾਲੀ
TupleIterDestruct(ਖਾਲੀ *arg)
{
    tupleiterinfo_T *tii = (tupleiterinfo_T*)arg;
    tuple_unref(tii->tuple);
    PyMem_Free(tii);
}

    ਸਥਿਰ PyObject *
Tuple_Iter_ਅਗਲਾ(ਖਾਲੀ **arg)
{
    PyObject	*ret;
    tupleiterinfo_T **tii = (tupleiterinfo_T**)arg;

    ਜੇ ((*tii)->ਸੂਚਕਾਂਕ >= TUPLE_LEN((*tii)->tuple))
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (!(ret = Convert_ਮੰਜ਼ਿਲ_Py_Object(TUPLE_ਵਸਤੂ((*tii)->tuple, (*tii)->ਸੂਚਕਾਂਕ))))
	ਵਾਪਸ ਨਿਰਾ;

    (*tii)->ਸੂਚਕਾਂਕ++;

    ਵਾਪਸ ret;
}

    ਸਥਿਰ PyObject *
TupleIter(PyObject *self_obj)
{
    TupleObject	*self = (TupleObject*)self_obj;
    tupleiterinfo_T	*tii;
    tuple_T	*t = self->tuple;

    ਜੇ (!(tii = PyMem_New(tupleiterinfo_T, 1)))
    {
	Py_Err_No_ਸਮ੍ਰਿਤੀ();
	ਵਾਪਸ ਨਿਰਾ;
    }

    tii->tuple = t;
    tii->ਸੂਚਕਾਂਕ = 0;
    ++t->tv_refcount;

    ਵਾਪਸ IterNew(tii,
	    TupleIterDestruct, Tuple_Iter_ਅਗਲਾ,
	    ਨਿਰਾ, ਨਿਰਾ, (PyObject *)self);
}

ਸਥਿਰ ਅੱਖਰ *TupleAttrs[] = {
    "locked",
    ਨਿਰਾ
};

    ਸਥਿਰ PyObject *
TupleDir(PyObject *self, PyObject *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ ObjectDir(self, TupleAttrs);
}

    ਸਥਿਰ ਪੂਰਨ
TupleSetattr(PyObject *self_obj, ਅੱਖਰ *name, PyObject *valObject)
{
    TupleObject *self = (TupleObject*)self_obj;
    ਜੇ (valObject == ਨਿਰਾ)
    {
	Py_Err_ਸੈੱਟ_STRING(PyExc_AttributeError,
		N_("cannot delete vim.Tuple attributes"));
	ਵਾਪਸ -1;
    }

    ਜੇ (strcmp(name, "locked") == 0)
    {
	ਜੇ (self->tuple->tv_ਤਾਲਾ == VAR_FIXED)
	{
	    Py_Err_ਸੈੱਟ_STRING(PyExc_TypeError, N_("cannot modify fixed tuple"));
	    ਵਾਪਸ -1;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਪੂਰਨ		istrue = PyObject_IsTrue(valObject);
	    ਜੇ (istrue == -1)
		ਵਾਪਸ -1;
	    ਨਹੀਂਤਾਂ ਜੇ (istrue)
		self->tuple->tv_ਤਾਲਾ = VAR_LOCKED;
	    ਨਹੀਂਤਾਂ
		self->tuple->tv_ਤਾਲਾ = 0;
	}
	ਵਾਪਸ 0;
    }
    ਨਹੀਂਤਾਂ
    {
	PyErr_FORMAT(PyExc_AttributeError, N_("cannot set attribute %s"), name);
	ਵਾਪਸ -1;
    }
}

ਸਥਿਰ PySequenceMethods TupleAsSeq = {
    (lenfunc)		Tuple_ਲੰਮਾਈ,	 // sq_length,	  len(x)
    (binaryfunc)	0,		 // RangeConcat, sq_concat,  x+y
    0,					 // RangeRepeat, sq_repeat,  x*n
    (PyIntArgFunc)	Tuple_ਸੂਚਕਾਂਕ,	 // sq_item,	  x[i]
    0,					 // was_sq_slice,     x[i:j]
    (PyIntObjArgProc)	0,		 // sq_as_item,  x[i]=v
    0,					 // was_sq_ass_slice, x[i:j]=v
    0,					 // sq_contains
    (binaryfunc)	0,		 // sq_inplace_concat
    0,					 // sq_inplace_repeat
};

ਸਥਿਰ PyMappingMethods TupleAsMapping = {
    /* mp_length	*/ (lenfunc) Tuple_ਲੰਮਾਈ,
    /* mp_subscript     */ (binaryfunc) Tuple_ਵਸਤੂ,
    /* mp_ass_subscript */ (objobjargproc) 0,
};

ਸਥਿਰ ਬਣਤਰ Py_ਵਿਧੀ_Def TupleMethods[] = {
    {"__dir__",	(PyCFunction)TupleDir,		METH_NOARGS,	""},
    { ਨਿਰਾ,	ਨਿਰਾ,				0,		ਨਿਰਾ}
};

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    Py_Object_ਸਿਰ
    char_u	*name;
    ਪੂਰਨ		argc;
    typval_T	*argv;
    dict_T	*self;
    pylinkedlist_T	ref;
    ਪੂਰਨ		auto_rebind;
} FunctionObject;

DEFINE_PY_TYPE_OBJECT(FunctionType);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NEW_FUNCTION(name, argc, argv, self, pt_auto) \
    FunctionNew(FunctionTypePtr, (name), (argc), (argv), (self), (pt_auto))

    ਸਥਿਰ PyObject *
FunctionNew(PyTypeObject *subtype, char_u *name, ਪੂਰਨ argc, typval_T *argv,
	dict_T *selfdict, ਪੂਰਨ auto_rebind)
{
    FunctionObject	*self;

    self = (FunctionObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    ਜੇ (self == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (ਅੰਕ_ਹੈ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)*name))
    {
	ਜੇ (!translated_function_exists(name, FALSE))
	{
	    PyErr_FORMAT(Py_Exc_ਮੁੱਲ_Error,
		    N_("unnamed function %s does not exist"), name);
	    ਵਾਪਸ ਨਿਰਾ;
	}
	self->name = vim_strsave(name);
    }
    ਨਹੀਂਤਾਂ
    {
	char_u *p;

	ਜੇ ((p = get_expanded_name(name,
			    vim_strchr(name, AUTOLOAD_CHAR) == ਨਿਰਾ)) == ਨਿਰਾ)
	{
	    PyErr_FORMAT(Py_Exc_ਮੁੱਲ_Error,
		    N_("function %s does not exist"), name);
	    ਵਾਪਸ ਨਿਰਾ;
	}

	ਜੇ (p[0] == K_SPECIAL && p[1] == KS_EXTRA && p[2] == (ਪੂਰਨ)KE_SNR)
	{
	    char_u *np;
	    size_t len = STRLEN(p) + 1;

	    ਜੇ ((np = alloc(len + 2)) == ਨਿਰਾ)
	    {
		vim_free(p);
		ਵਾਪਸ ਨਿਰਾ;
	    }
	    mch_memmove(np, "<SNR>", 5);
	    mch_memmove(np + 5, p + 3, len - 3);
	    vim_free(p);
	    self->name = np;
	}
	ਨਹੀਂਤਾਂ
	    self->name = p;
    }

    func_ref(self->name);
    self->argc = argc;
    self->argv = argv;
    self->self = selfdict;
    self->auto_rebind = selfdict == ਨਿਰਾ ? TRUE : auto_rebind;

    ਜੇ (self->argv || self->self)
	pyll_ਜੋੜੋ((PyObject *)(self), &self->ref, &lastfunc);

    ਵਾਪਸ (PyObject *)(self);
}

    ਸਥਿਰ PyObject *
FunctionConstructor(PyTypeObject *subtype, PyObject *ਦਲੀਲਾਂ, PyObject *kwargs)
{
    PyObject	*self;
    PyObject	*selfdictObject;
    PyObject	*autoRebindObject;
    PyObject	*ਦਲੀਲਾਂ_Object = ਨਿਰਾ;
    char_u	*name;
    typval_T	selfdicttv;
    typval_T	argstv;
    list_T	*argslist = ਨਿਰਾ;
    dict_T	*selfdict = ਨਿਰਾ;
    ਪੂਰਨ		argc = 0;
    ਪੂਰਨ		auto_rebind = TRUE;
    typval_T	*argv = ਨਿਰਾ;
    typval_T	*curtv;
    listitem_T	*li;

    ਜੇ (kwargs != ਨਿਰਾ)
    {
	selfdictObject = Py_Dict_Get_ਵਸਤੂ_String(kwargs, "self");
	ਜੇ (selfdictObject != ਨਿਰਾ)
	{
	    ਜੇ (ConvertFromPyMapping(selfdictObject, &selfdicttv) == -1)
		ਵਾਪਸ ਨਿਰਾ;
	    selfdict = selfdicttv.vval.v_dict;
	}
	ਦਲੀਲਾਂ_Object = Py_Dict_Get_ਵਸਤੂ_String(kwargs, "args");
	ਜੇ (ਦਲੀਲਾਂ_Object != ਨਿਰਾ)
	{
	    ਜੇ (ConvertFromPySequence(ਦਲੀਲਾਂ_Object, &argstv) == -1)
	    {
		dict_unref(selfdict);
		ਵਾਪਸ ਨਿਰਾ;
	    }
	    argslist = argstv.vval.v_list;
	    CHECK_LIST_MATERIALIZE(argslist);

	    argc = argslist->lv_len;
	    ਜੇ (argc != 0)
	    {
		argv = PyMem_New(typval_T, (size_t) argc);
		ਜੇ (argv == ਨਿਰਾ)
		{
		    Py_Err_No_ਸਮ੍ਰਿਤੀ();
		    dict_unref(selfdict);
		    list_unref(argslist);
		    ਵਾਪਸ ਨਿਰਾ;
		}
		curtv = argv;
		FOR_ALL_LIST_ਚੀਜ਼ਾਂ(argslist, li)
		    copy_tv(&li->li_tv, curtv++);
	    }
	    list_unref(argslist);
	}
	ਜੇ (selfdict != ਨਿਰਾ)
	{
	    auto_rebind = FALSE;
	    autoRebindObject = Py_Dict_Get_ਵਸਤੂ_String(kwargs, "auto_rebind");
	    ਜੇ (autoRebindObject != ਨਿਰਾ)
	    {
		auto_rebind = PyObject_IsTrue(autoRebindObject);
		ਜੇ (auto_rebind == -1)
		{
		    dict_unref(selfdict);
		    list_unref(argslist);
		    ਵਾਪਸ ਨਿਰਾ;
		}
	    }
	}
    }

    ਜੇ (!PyArg_ParseTuple(ਦਲੀਲਾਂ, "et", "ascii", &name))
    {
	dict_unref(selfdict);
	ਜਦੋਂ (argc--)
	    clear_tv(&argv[argc]);
	PyMem_Free(argv);
	ਵਾਪਸ ਨਿਰਾ;
    }

    self = FunctionNew(subtype, name, argc, argv, selfdict, auto_rebind);

    PyMem_Free(name);

    ਵਾਪਸ self;
}

    ਸਥਿਰ ਖਾਲੀ
FunctionDestructor(PyObject *self_obj)
{
    FunctionObject *self = (FunctionObject*)self_obj;
    ਪੂਰਨ i;
    func_unref(self->name);
    vim_free(self->name);
    ਲਈ (i = 0; i < self->argc; ++i)
	clear_tv(&self->argv[i]);
    PyMem_Free(self->argv);
    dict_unref(self->self);
    ਜੇ (self->argv || self->self)
	pyll_remove(&self->ref, &lastfunc);

    DESTRUCTOR_ਅੰਤ(self);
}

ਸਥਿਰ ਅੱਖਰ *FunctionAttrs[] = {
    "softspace", "args", "self", "auto_rebind",
    ਨਿਰਾ
};

    ਸਥਿਰ PyObject *
FunctionDir(PyObject *self, PyObject *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ ObjectDir(self, FunctionAttrs);
}

    ਸਥਿਰ PyObject *
FunctionAttr(FunctionObject *self, ਅੱਖਰ *name)
{
    list_T *list;
    ਪੂਰਨ i;
    ਜੇ (strcmp(name, "name") == 0)
	ਵਾਪਸ PyString_FromString((ਅੱਖਰ *)(self->name));
    ਨਹੀਂਤਾਂ ਜੇ (strcmp(name, "args") == 0)
    {
	ਜੇ (self->argv == ਨਿਰਾ || (list = list_alloc()) == ਨਿਰਾ)
	    ਵਾਪਸ ALWAYS_NONE;

	ਲਈ (i = 0; i < self->argc; ++i)
	    list_append_tv(list, &self->argv[i]);
	ਵਾਪਸ NEW_LIST(list);
    }
    ਨਹੀਂਤਾਂ ਜੇ (strcmp(name, "self") == 0)
	ਵਾਪਸ self->self == ਨਿਰਾ
	    ? ALWAYS_NONE
	    : NEW_DICTIONARY(self->self);
    ਨਹੀਂਤਾਂ ਜੇ (strcmp(name, "auto_rebind") == 0)
	ਵਾਪਸ self->auto_rebind
	    ? ALWAYS_TRUE
	    : ALWAYS_FALSE;
    ਨਹੀਂਤਾਂ ਜੇ (strcmp(name, "__members__") == 0)
	ਵਾਪਸ ObjectDir(ਨਿਰਾ, FunctionAttrs);
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Populate partial_T given function object.
 *
 * "exported" should be set to true when it is needed to construct a partial
 * that may be stored in a variable (i.e. may be freed by Vim).
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_partial(FunctionObject *self, partial_T *pt, ਪੂਰਨ exported)
{
    ਪੂਰਨ i;

    pt->pt_name = self->name;
    ਜੇ (self->argv)
    {
	pt->pt_argc = self->argc;
	ਜੇ (exported)
	{
	    pt->pt_argv = ALLOC_CLEAR_MULT(typval_T, self->argc);
	    ਲਈ (i = 0; i < pt->pt_argc; ++i)
		copy_tv(&self->argv[i], &pt->pt_argv[i]);
	}
	ਨਹੀਂਤਾਂ
	    pt->pt_argv = self->argv;
    }
    ਨਹੀਂਤਾਂ
    {
	pt->pt_argc = 0;
	pt->pt_argv = ਨਿਰਾ;
    }
    pt->pt_auto = self->auto_rebind || !exported;
    pt->pt_dict = self->self;
    ਜੇ (exported && self->self)
	++pt->pt_dict->dv_refcount;
    ਜੇ (exported)
	pt->pt_name = vim_strsave(pt->pt_name);
    pt->pt_refcount = 1;
}

    ਸਥਿਰ PyObject *
FunctionCall(PyObject *self_obj, PyObject *ਦਲੀਲਾਂ_Object, PyObject *kwargs)
{
    FunctionObject *self = (FunctionObject*)self_obj;
    char_u	*name = self->name;
    typval_T	ਦਲੀਲਾਂ;
    typval_T	selfdicttv;
    typval_T	rettv;
    dict_T	*selfdict = ਨਿਰਾ;
    PyObject	*selfdictObject;
    PyObject	*ret;
    ਪੂਰਨ		error;
    partial_T	pt;
    partial_T	*pt_ptr = ਨਿਰਾ;

    ਜੇ (ConvertFromPySequence(ਦਲੀਲਾਂ_Object, &ਦਲੀਲਾਂ) == -1)
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (kwargs != ਨਿਰਾ)
    {
	selfdictObject = Py_Dict_Get_ਵਸਤੂ_String(kwargs, "self");
	ਜੇ (selfdictObject != ਨਿਰਾ)
	{
	    ਜੇ (ConvertFromPyMapping(selfdictObject, &selfdicttv) == -1)
	    {
		clear_tv(&ਦਲੀਲਾਂ);
		ਵਾਪਸ ਨਿਰਾ;
	    }
	    selfdict = selfdicttv.vval.v_dict;
	}
    }

    ਜੇ (self->argv || self->self)
    {
	CLEAR_ਖੇਤਰ(pt);
	ਸੈੱਟ_partial(self, &pt, FALSE);
	pt_ptr = &pt;
    }

    Py_BEGIN_ALLOW_ਧਾਗੇ
    Python_ਤਾਲਾ_Vim();

    Vim_Try_ਸ਼ੁਰੂ();
    error = func_call(name, &ਦਲੀਲਾਂ, pt_ptr, selfdict, &rettv);

    Python_Release_Vim();
    Py_ਸਿਰਾ_ALLOW_ਧਾਗੇ

    ਜੇ (Vim_Try_ਸਿਰਾ())
	ret = ਨਿਰਾ;
    ਨਹੀਂਤਾਂ ਜੇ (error != OK)
    {
	ret = ਨਿਰਾ;
	PyErr_VIM_FORMAT(N_("failed to run function %s"), (ਅੱਖਰ *)name);
    }
    ਨਹੀਂਤਾਂ
	ret = Convert_ਮੰਜ਼ਿਲ_Py_Object(&rettv);

    clear_tv(&ਦਲੀਲਾਂ);
    clear_tv(&rettv);
    ਜੇ (selfdict != ਨਿਰਾ)
	clear_tv(&selfdicttv);

    ਵਾਪਸ ret;
}

    ਸਥਿਰ PyObject *
FunctionRepr(PyObject *self_obj)
{
    FunctionObject *self = (FunctionObject*)self_obj;
    PyObject *ret;
    garray_T repr_ga;
    ਪੂਰਨ i;
    char_u *tofree = ਨਿਰਾ;
    typval_T tv;
    char_u numbuf[NUMBUFLEN];

    ga_init2(&repr_ga, ਆਕਾਰ(ਅੱਖਰ), 70);
    ga_concat(&repr_ga, (char_u *)"<vim.Function '");
    ਜੇ (self->name)
	ga_concat(&repr_ga, self->name);
    ਨਹੀਂਤਾਂ
	ga_concat(&repr_ga, (char_u *)"<NULL>");
    ga_append(&repr_ga, '\'');
    ਜੇ (self->argv)
    {
	ga_concat(&repr_ga, (char_u *)", args=[");
	++emsg_silent;
	ਲਈ (i = 0; i < self->argc; i++)
	{
	    ਜੇ (i != 0)
		ga_concat(&repr_ga, (char_u *)", ");
	    ga_concat(&repr_ga, tv2string(&self->argv[i], &tofree, numbuf,
			get_copyID()));
	    vim_free(tofree);
	}
	--emsg_silent;
	ga_append(&repr_ga, ']');
    }
    ਜੇ (self->self)
    {
	ga_concat(&repr_ga, (char_u *)", self=");
	tv.v_type = VAR_DICT;
	tv.vval.v_dict = self->self;
	++emsg_silent;
	ga_concat(&repr_ga, tv2string(&tv, &tofree, numbuf, get_copyID()));
	--emsg_silent;
	vim_free(tofree);
	ਜੇ (self->auto_rebind)
	    ga_concat(&repr_ga, (char_u *)", auto_rebind=True");
    }
    ga_append(&repr_ga, '>');
    ret = PyString_FromString((ਅੱਖਰ *)repr_ga.ga_ਅੰਕੜੇ);
    ga_clear(&repr_ga);
    ਵਾਪਸ ret;
}

ਸਥਿਰ ਬਣਤਰ Py_ਵਿਧੀ_Def FunctionMethods[] = {
    {"__dir__",	(PyCFunction)FunctionDir,   METH_NOARGS,		""},
    { ਨਿਰਾ,	ਨਿਰਾ,			0,				ਨਿਰਾ}
};

/*
 * Options object
 */

DEFINE_PY_TYPE_OBJECT(OptionsType);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*checkfun)(ਖਾਲੀ *);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    Py_Object_ਸਿਰ
    ਪੂਰਨ		opt_type;
    ਖਾਲੀ	*from;
    checkfun	Check;
    PyObject	*fromObj;
} OptionsObject;

    ਸਥਿਰ ਪੂਰਨ
dummy_check(ਖਾਲੀ *arg ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ 0;
}

    ਸਥਿਰ PyObject *
OptionsNew(ਪੂਰਨ opt_type, ਖਾਲੀ *from, checkfun Check, PyObject *fromObj)
{
    OptionsObject	*self;

    self = PyObject_GC_New(OptionsObject, OptionsTypePtr);
    ਜੇ (self == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    self->opt_type = opt_type;
    self->from = from;
    self->Check = Check;
    self->fromObj = fromObj;
    ਜੇ (fromObj)
	Py_INCREF(fromObj);

    ਵਾਪਸ (PyObject *)(self);
}

    ਸਥਿਰ ਖਾਲੀ
OptionsDestructor(PyObject *self_obj)
{
    OptionsObject *self = (OptionsObject*)self_obj;
    PyObject_GC_UnTrack((ਖਾਲੀ *)(self));
    Py_XDECREF(self->fromObj);
    PyObject_GC_Del((ਖਾਲੀ *)(self));
}

    ਸਥਿਰ ਪੂਰਨ
OptionsTraverse(PyObject *self_obj, visitproc ਦੇਖੋ, ਖਾਲੀ *arg)
{
    OptionsObject *self = (OptionsObject*)self_obj;
    Py_ਦੇਖੋ(self->fromObj);
    ਵਾਪਸ 0;
}

    ਸਥਿਰ ਪੂਰਨ
OptionsClear(PyObject *self_obj)
{
    OptionsObject *self = (OptionsObject*)self_obj;
    Py_CLEAR(self->fromObj);
    ਵਾਪਸ 0;
}

    ਸਥਿਰ PyObject *
Options_ਵਸਤੂ(OptionsObject *self, PyObject *ਕੁੰਜੀ_Object)
{
    char_u	*ਕੁੰਜੀ;
    ਪੂਰਨ		flags;
    ਲੰਮਾ	numval;
    char_u	*stringval;
    PyObject	*todecref;

    ਜੇ (self->Check(self->fromObj))
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (!(ਕੁੰਜੀ = String_ਮੰਜ਼ਿਲ_Chars(ਕੁੰਜੀ_Object, &todecref)))
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (*ਕੁੰਜੀ == NUL)
    {
	RAISE_NO_ਖਾਲੀ_ਥਾਵਾਂ_ਕੁੰਜੀਆਂ;
	Py_XDECREF(todecref);
	ਵਾਪਸ ਨਿਰਾ;
    }

    flags = get_option_ਮੁੱਲ_strict(ਕੁੰਜੀ, &numval, &stringval,
				    self->opt_type, self->from);

    Py_XDECREF(todecref);

    ਜੇ (flags == 0)
    {
	Py_Err_ਸੈੱਟ_Object(Py_Exc_ਕੁੰਜੀ_Error, ਕੁੰਜੀ_Object);
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਜੇ (flags & SOPT_UNSET)
    {
	Py_INCREF(Py_None);
	ਵਾਪਸ Py_None;
    }
    ਨਹੀਂਤਾਂ ਜੇ (flags & SOPT_BOOL)
    {
	PyObject	*ret;
	ret = numval ? Py_True : Py_False;
	Py_INCREF(ret);
	ਵਾਪਸ ret;
    }
    ਨਹੀਂਤਾਂ ਜੇ (flags & SOPT_ਅੰਕ)
	ਵਾਪਸ PyInt_FromLong(numval);
    ਨਹੀਂਤਾਂ ਜੇ (flags & SOPT_STRING)
    {
	ਜੇ (stringval)
	{
	    PyObject	*ret = Py_ਬਾਈਟਾਂ_From_String((ਅੱਖਰ *)stringval);
	    vim_free(stringval);
	    ਵਾਪਸ ret;
	}
	ਨਹੀਂਤਾਂ
	{
	    Py_Err_ਸੈੱਟ_STRING(PyExc_RuntimeError,
		    N_("unable to get option value"));
	    ਵਾਪਸ ਨਿਰਾ;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	Py_Err_ਸੈੱਟ_VIM(N_("internal error: unknown option type"));
	ਵਾਪਸ ਨਿਰਾ;
    }
}

    ਸਥਿਰ ਪੂਰਨ
Options_ਸ਼ਾਮਲ_ਹੈ(OptionsObject *self, PyObject *ਕੁੰਜੀ_Object)
{
    char_u	*ਕੁੰਜੀ;
    PyObject	*todecref;

    ਜੇ (!(ਕੁੰਜੀ = String_ਮੰਜ਼ਿਲ_Chars(ਕੁੰਜੀ_Object, &todecref)))
	ਵਾਪਸ -1;

    ਜੇ (*ਕੁੰਜੀ == NUL)
    {
	Py_XDECREF(todecref);
	ਵਾਪਸ 0;
    }

    ਜੇ (get_option_ਮੁੱਲ_strict(ਕੁੰਜੀ, ਨਿਰਾ, ਨਿਰਾ, self->opt_type, ਨਿਰਾ))
    {
	Py_XDECREF(todecref);
	ਵਾਪਸ 1;
    }
    ਨਹੀਂਤਾਂ
    {
	Py_XDECREF(todecref);
	ਵਾਪਸ 0;
    }
}

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਖਾਲੀ	*lastoption;
    ਪੂਰਨ		opt_type;
} optiterinfo_T;

    ਸਥਿਰ PyObject *
Options_Iter_ਅਗਲਾ(ਖਾਲੀ **arg)
{
    char_u	*name;
    optiterinfo_T **oii = (optiterinfo_T**)arg;

    ਜੇ ((name = option_iter_ਅਗਲਾ(&((*oii)->lastoption), (*oii)->opt_type)))
	ਵਾਪਸ PyString_FromString((ਅੱਖਰ *)name);

    ਵਾਪਸ ਨਿਰਾ;
}

    ਸਥਿਰ PyObject *
OptionsIter(PyObject *self_obj)
{
    OptionsObject	*self = (OptionsObject*)self_obj;
    optiterinfo_T	*oii;

    ਜੇ (!(oii = PyMem_New(optiterinfo_T, 1)))
    {
	Py_Err_No_ਸਮ੍ਰਿਤੀ();
	ਵਾਪਸ ਨਿਰਾ;
    }

    oii->opt_type = self->opt_type;
    oii->lastoption = ਨਿਰਾ;

    ਵਾਪਸ IterNew(oii,
	    PyMem_Free, Options_Iter_ਅਗਲਾ,
	    ਨਿਰਾ, ਨਿਰਾ, (PyObject *)self);
}

    ਸਥਿਰ ਪੂਰਨ
ਸੈੱਟ_option_ਮੁੱਲ_err(char_u *ਕੁੰਜੀ, ਪੂਰਨ numval, char_u *stringval, ਪੂਰਨ opt_flags)
{
    ਅੱਖਰ	*errmsg;

    ਜੇ ((errmsg = ਸੈੱਟ_option_ਮੁੱਲ(ਕੁੰਜੀ, numval, stringval, opt_flags)))
    {
	ਜੇ (Vim_Try_ਸਿਰਾ())
	    ਵਾਪਸ FAIL;
	Py_Err_ਸੈੱਟ_Vim(errmsg);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

    ਸਥਿਰ ਪੂਰਨ
ਸੈੱਟ_option_ਮੁੱਲ_for(
	char_u *ਕੁੰਜੀ,
	ਪੂਰਨ numval,
	char_u *stringval,
	ਪੂਰਨ opt_flags,
	ਪੂਰਨ opt_type,
	ਖਾਲੀ *from)
{
    switchwin_T	switchwin;
    bufref_T	save_curbuf;
    ਪੂਰਨ		ਸੈੱਟ_ret = 0;

    Vim_Try_ਸ਼ੁਰੂ();
    ਚੋਣ (opt_type)
    {
	ਹਾਲਤ SREQ_WIN:
	    ਜੇ (switch_win(&switchwin, (win_T *)from,
			      win_ਲੱਭੋ_tabpage((win_T *)from), FALSE) == FAIL)
	    {
		restore_win(&switchwin, TRUE);
		ਜੇ (Vim_Try_ਸਿਰਾ())
		    ਵਾਪਸ -1;
		Py_Err_ਸੈੱਟ_VIM(N_("problem while switching windows"));
		ਵਾਪਸ -1;
	    }
	    ਸੈੱਟ_ret = ਸੈੱਟ_option_ਮੁੱਲ_err(ਕੁੰਜੀ, numval, stringval, opt_flags);
	    restore_win(&switchwin, TRUE);
	    ਰੋਕੋ;
	ਹਾਲਤ SREQ_BUF:
	    switch_ਬਫਰ(&save_curbuf, (buf_T *)from);
	    ਸੈੱਟ_ret = ਸੈੱਟ_option_ਮੁੱਲ_err(ਕੁੰਜੀ, numval, stringval, opt_flags);
	    restore_ਬਫਰ(&save_curbuf);
	    ਰੋਕੋ;
	ਹਾਲਤ SREQ_GLOBAL:
	    ਸੈੱਟ_ret = ਸੈੱਟ_option_ਮੁੱਲ_err(ਕੁੰਜੀ, numval, stringval, opt_flags);
	    ਰੋਕੋ;
    }
    ਜੇ (ਸੈੱਟ_ret == FAIL)
	ਵਾਪਸ -1;
    ਵਾਪਸ Vim_Try_ਸਿਰਾ();
}

    ਸਥਿਰ ਪੂਰਨ
Options_Ass_ਵਸਤੂ(OptionsObject *self, PyObject *ਕੁੰਜੀ_Object, PyObject *valObject)
{
    char_u	*ਕੁੰਜੀ;
    ਪੂਰਨ		flags;
    ਪੂਰਨ		opt_flags;
    ਪੂਰਨ		ret = 0;
    PyObject	*todecref;

    ਜੇ (self->Check(self->fromObj))
	ਵਾਪਸ -1;

    ਜੇ (!(ਕੁੰਜੀ = String_ਮੰਜ਼ਿਲ_Chars(ਕੁੰਜੀ_Object, &todecref)))
	ਵਾਪਸ -1;

    ਜੇ (*ਕੁੰਜੀ == NUL)
    {
	RAISE_NO_ਖਾਲੀ_ਥਾਵਾਂ_ਕੁੰਜੀਆਂ;
	Py_XDECREF(todecref);
	ਵਾਪਸ -1;
    }

    flags = get_option_ਮੁੱਲ_strict(ਕੁੰਜੀ, ਨਿਰਾ, ਨਿਰਾ,
				    self->opt_type, self->from);

    ਜੇ (flags == 0)
    {
	Py_Err_ਸੈੱਟ_Object(Py_Exc_ਕੁੰਜੀ_Error, ਕੁੰਜੀ_Object);
	Py_XDECREF(todecref);
	ਵਾਪਸ -1;
    }

    ਜੇ (valObject == ਨਿਰਾ)
    {
	ਜੇ (self->opt_type == SREQ_GLOBAL)
	{
	    PyErr_FORMAT(Py_Exc_ਮੁੱਲ_Error,
		    N_("unable to unset global option %s"), ਕੁੰਜੀ);
	    Py_XDECREF(todecref);
	    ਵਾਪਸ -1;
	}
	ਨਹੀਂਤਾਂ ਜੇ (!(flags & SOPT_GLOBAL))
	{
	    PyErr_FORMAT(Py_Exc_ਮੁੱਲ_Error,
		    N_("unable to unset option %s "
		       "which does not have global value"), ਕੁੰਜੀ);
	    Py_XDECREF(todecref);
	    ਵਾਪਸ -1;
	}
	ਨਹੀਂਤਾਂ
	{
	    unset_global_local_option(ਕੁੰਜੀ, self->from);
	    Py_XDECREF(todecref);
	    ਵਾਪਸ 0;
	}
    }

    opt_flags = (self->opt_type ? OPT_LOCAL : OPT_GLOBAL);

    ਜੇ (flags & SOPT_BOOL)
    {
	ਪੂਰਨ	istrue = PyObject_IsTrue(valObject);

	ਜੇ (istrue == -1)
	    ret = -1;
	ਨਹੀਂਤਾਂ
	    ret = ਸੈੱਟ_option_ਮੁੱਲ_for(ਕੁੰਜੀ, istrue, ਨਿਰਾ,
				    opt_flags, self->opt_type, self->from);
    }
    ਨਹੀਂਤਾਂ ਜੇ (flags & SOPT_ਅੰਕ)
    {
	ਲੰਮਾ	val;

	ਜੇ (ਸੰਖਿਆ_ਮੰਜ਼ਿਲ_Long(valObject, &val, ਸੰਖਿਆ_INT))
	{
	    Py_XDECREF(todecref);
	    ਵਾਪਸ -1;
	}

	ret = ਸੈੱਟ_option_ਮੁੱਲ_for(ਕੁੰਜੀ, (ਪੂਰਨ) val, ਨਿਰਾ, opt_flags,
				self->opt_type, self->from);
    }
    ਨਹੀਂਤਾਂ
    {
	char_u		*val;
	PyObject	*todecref2;

	ਜੇ ((val = String_ਮੰਜ਼ਿਲ_Chars(valObject, &todecref2)))
	{
	    ret = ਸੈੱਟ_option_ਮੁੱਲ_for(ਕੁੰਜੀ, 0, val, opt_flags,
				    self->opt_type, self->from);
	    Py_XDECREF(todecref2);
	}
	ਨਹੀਂਤਾਂ
	    ret = -1;
    }

    Py_XDECREF(todecref);

    ਵਾਪਸ ret;
}

ਸਥਿਰ PySequenceMethods OptionsAsSeq = {
    0,					// sq_length
    0,					// sq_concat
    0,					// sq_repeat
    0,					// sq_item
    0,					// sq_slice
    0,					// sq_ass_item
    0,					// sq_ass_slice
    (objobjproc) Options_ਸ਼ਾਮਲ_ਹੈ,	// sq_contains
    0,					// sq_inplace_concat
    0,					// sq_inplace_repeat
};

ਸਥਿਰ PyMappingMethods OptionsAsMapping = {
    (lenfunc)       ਨਿਰਾ,
    (binaryfunc)    Options_ਵਸਤੂ,
    (objobjargproc) Options_Ass_ਵਸਤੂ,
};

// Tabpage object

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    Py_Object_ਸਿਰ
    tabpage_T	*tab;
} Tab_ਪੰਨਾ_Object;

ਸਥਿਰ PyObject *WinListNew(Tab_ਪੰਨਾ_Object *tabObject);

DEFINE_PY_TYPE_OBJECT(Tab_ਪੰਨਾ_Type);

    ਸਥਿਰ ਪੂਰਨ
Check_Tab_ਪੰਨਾ(Tab_ਪੰਨਾ_Object *self)
{
    ਜੇ (self->tab == INVALID_TABPAGE_ਮੁੱਲ)
    {
	Py_Err_ਸੈੱਟ_VIM(N_("attempt to refer to deleted tab page"));
	ਵਾਪਸ -1;
    }

    ਵਾਪਸ 0;
}

    ਸਥਿਰ PyObject *
Tab_ਪੰਨਾ_New(tabpage_T *tab)
{
    Tab_ਪੰਨਾ_Object *self;

    ਜੇ (TAB_PYTHON_REF(tab))
    {
	self = TAB_PYTHON_REF(tab);
	Py_INCREF((PyObject *)self);
    }
    ਨਹੀਂਤਾਂ
    {
	self = PyObject_NEW(Tab_ਪੰਨਾ_Object, Tab_ਪੰਨਾ_Type_Ptr);
	ਜੇ (self == ਨਿਰਾ)
	    ਵਾਪਸ ਨਿਰਾ;
	self->tab = tab;
	TAB_PYTHON_REF(tab) = self;
    }

    ਵਾਪਸ (PyObject *)(self);
}

    ਸਥਿਰ ਖਾਲੀ
Tab_ਪੰਨਾ_Destructor(PyObject *self_obj)
{
    Tab_ਪੰਨਾ_Object *self = (Tab_ਪੰਨਾ_Object*)self_obj;
    ਜੇ (self->tab && self->tab != INVALID_TABPAGE_ਮੁੱਲ)
	TAB_PYTHON_REF(self->tab) = ਨਿਰਾ;

    DESTRUCTOR_ਅੰਤ(self);
}

ਸਥਿਰ ਅੱਖਰ *Tab_ਪੰਨਾ_Attrs[] = {
    "windows", "number", "vars", "window", "valid",
    ਨਿਰਾ
};

    ਸਥਿਰ PyObject *
Tab_ਪੰਨਾ_Dir(PyObject *self, PyObject *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ ObjectDir(self, Tab_ਪੰਨਾ_Attrs);
}

    ਸਥਿਰ PyObject *
Tab_ਪੰਨਾ_Attr_Valid(Tab_ਪੰਨਾ_Object *self, ਅੱਖਰ *name)
{
    PyObject	*ret;

    ਜੇ (strcmp(name, "valid") != 0)
	ਵਾਪਸ ਨਿਰਾ;

    ret = ((self->tab == INVALID_TABPAGE_ਮੁੱਲ) ? Py_False : Py_True);
    Py_INCREF(ret);
    ਵਾਪਸ ret;
}

    ਸਥਿਰ PyObject *
Tab_ਪੰਨਾ_Attr(Tab_ਪੰਨਾ_Object *self, ਅੱਖਰ *name)
{
    ਜੇ (strcmp(name, "windows") == 0)
	ਵਾਪਸ WinListNew(self);
    ਨਹੀਂਤਾਂ ਜੇ (strcmp(name, "number") == 0)
	ਵਾਪਸ PyLong_FromLong((ਲੰਮਾ) get_tab_ਸੰਖਿਆ(self->tab));
    ਨਹੀਂਤਾਂ ਜੇ (strcmp(name, "vars") == 0)
	ਵਾਪਸ NEW_DICTIONARY(self->tab->tp_vars);
    ਨਹੀਂਤਾਂ ਜੇ (strcmp(name, "window") == 0)
    {
	// For current tab window.c does not bother to set or update tp_curwin
	ਜੇ (self->tab == curtab)
	    ਵਾਪਸ ਖਿੜਕੀ_New(curwin, curtab);
	ਨਹੀਂਤਾਂ
	    ਵਾਪਸ ਖਿੜਕੀ_New(self->tab->tp_curwin, self->tab);
    }
    ਨਹੀਂਤਾਂ ਜੇ (strcmp(name, "__members__") == 0)
	ਵਾਪਸ ObjectDir(ਨਿਰਾ, Tab_ਪੰਨਾ_Attrs);
    ਵਾਪਸ ਨਿਰਾ;
}

    ਸਥਿਰ PyObject *
Tab_ਪੰਨਾ_Repr(PyObject *self_obj)
{
    Tab_ਪੰਨਾ_Object *self = (Tab_ਪੰਨਾ_Object*)self_obj;
    ਜੇ (self->tab == INVALID_TABPAGE_ਮੁੱਲ)
	ਵਾਪਸ PyString_FromFormat("<tabpage object (deleted) at %p>", (ਖਾਲੀ *)self);
    ਨਹੀਂਤਾਂ
    {
	ਪੂਰਨ	t = get_tab_ਸੰਖਿਆ(self->tab);

	ਜੇ (t == 0)
	    ਵਾਪਸ PyString_FromFormat("<tabpage object (unknown) at %p>",
					(ਖਾਲੀ *)self);
	ਨਹੀਂਤਾਂ
	    ਵਾਪਸ PyString_FromFormat("<tabpage %d>", t - 1);
    }
}

ਸਥਿਰ ਬਣਤਰ Py_ਵਿਧੀ_Def Tab_ਪੰਨਾ_Methods[] = {
    // name,	    function,			calling,	documentation
    {"__dir__",	    (PyCFunction)Tab_ਪੰਨਾ_Dir,	METH_NOARGS,	""},
    { ਨਿਰਾ,	    ਨਿਰਾ,			0,		ਨਿਰਾ}
};

/*
 * Window list object
 */

DEFINE_PY_TYPE_OBJECT(TabListType);
ਸਥਿਰ PySequenceMethods TabListAsSeq;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    Py_Object_ਸਿਰ
} TabListObject;

ਸਥਿਰ TabListObject The_Tab_ਪੰਨਾ_List =
{
    Py_Object_ਸਿਰ_INIT_TYPE(TabListType)
};

    ਸਥਿਰ PyInt
Tab_List_ਲੰਮਾਈ(PyObject *self ਅਣਵਰਤਿਆ)
{
    tabpage_T	*tp = ਪਹਿਲਾ_tabpage;
    PyInt	n = 0;

    ਜਦੋਂ (tp != ਨਿਰਾ)
    {
	++n;
	tp = tp->tp_ਅਗਲਾ;
    }

    ਵਾਪਸ n;
}

    ਸਥਿਰ PyObject *
Tab_List_ਵਸਤੂ(PyObject *self ਅਣਵਰਤਿਆ, PyInt n)
{
    tabpage_T	*tp;

    ਲਈ (tp = ਪਹਿਲਾ_tabpage; tp != ਨਿਰਾ; tp = tp->tp_ਅਗਲਾ, --n)
	ਜੇ (n == 0)
	    ਵਾਪਸ Tab_ਪੰਨਾ_New(tp);

    Py_Err_ਸੈੱਟ_STRING(Py_Exc_ਸੂਚਕਾਂਕ_Error, N_("no such tab page"));
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Window object
 */

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    Py_Object_ਸਿਰ
    win_T	*win;
    Tab_ਪੰਨਾ_Object	*tabObject;
} ਖਿੜਕੀ_Object;

DEFINE_PY_TYPE_OBJECT(ਖਿੜਕੀ_Type);

    ਸਥਿਰ ਪੂਰਨ
Check_ਖਿੜਕੀ(ਖਿੜਕੀ_Object *self)
{
    ਜੇ (self->win == INVALID_ਖਿੜਕੀ_ਮੁੱਲ)
    {
	Py_Err_ਸੈੱਟ_VIM(N_("attempt to refer to deleted window"));
	ਵਾਪਸ -1;
    }

    ਵਾਪਸ 0;
}

    ਸਥਿਰ ਪੂਰਨ
Check_ਖਿੜਕੀ_Cb(ਖਾਲੀ *self)
{
    ਵਾਪਸ Check_ਖਿੜਕੀ((ਖਿੜਕੀ_Object*)self);
}

    ਸਥਿਰ PyObject *
ਖਿੜਕੀ_New(win_T *win, tabpage_T *tab)
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

    ਖਿੜਕੀ_Object *self;

    ਜੇ (WIN_PYTHON_REF(win))
    {
	self = WIN_PYTHON_REF(win);
	Py_INCREF((PyObject *)self);
    }
    ਨਹੀਂਤਾਂ
    {
	self = PyObject_GC_New(ਖਿੜਕੀ_Object, ਖਿੜਕੀ_Type_Ptr);
	ਜੇ (self == ਨਿਰਾ)
	    ਵਾਪਸ ਨਿਰਾ;
	self->win = win;
	WIN_PYTHON_REF(win) = self;
    }

    self->tabObject = ((Tab_ਪੰਨਾ_Object *)(Tab_ਪੰਨਾ_New(tab)));

    ਵਾਪਸ (PyObject *)(self);
}

    ਸਥਿਰ ਖਾਲੀ
ਖਿੜਕੀ_Destructor(PyObject *self_obj)
{
    ਖਿੜਕੀ_Object *self = (ਖਿੜਕੀ_Object*)self_obj;
    PyObject_GC_UnTrack((ਖਾਲੀ *)(self));
    ਜੇ (self->win && self->win != INVALID_ਖਿੜਕੀ_ਮੁੱਲ)
	WIN_PYTHON_REF(self->win) = ਨਿਰਾ;
    Py_XDECREF(((PyObject *)(self->tabObject)));
    PyObject_GC_Del((ਖਾਲੀ *)(self));
}

    ਸਥਿਰ ਪੂਰਨ
ਖਿੜਕੀ_Traverse(PyObject *self_obj, visitproc ਦੇਖੋ, ਖਾਲੀ *arg)
{
    ਖਿੜਕੀ_Object *self = (ਖਿੜਕੀ_Object*)self_obj;
    Py_ਦੇਖੋ(((PyObject *)(self->tabObject)));
    ਵਾਪਸ 0;
}

    ਸਥਿਰ ਪੂਰਨ
ਖਿੜਕੀ_Clear(PyObject *self_obj)
{
    ਖਿੜਕੀ_Object *self = (ਖਿੜਕੀ_Object*)self_obj;
    Py_CLEAR(self->tabObject);
    ਵਾਪਸ 0;
}

    ਸਥਿਰ win_T *
get_firstwin(Tab_ਪੰਨਾ_Object *tabObject)
{
    ਜੇ (tabObject)
    {
	ਜੇ (Check_Tab_ਪੰਨਾ(tabObject))
	    ਵਾਪਸ ਨਿਰਾ;
	// For current tab window.c does not bother to set or update tp_firstwin
	ਨਹੀਂਤਾਂ ਜੇ (tabObject->tab == curtab)
	    ਵਾਪਸ firstwin;
	ਨਹੀਂਤਾਂ
	    ਵਾਪਸ tabObject->tab->tp_firstwin;
    }
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ firstwin;
}

// Use the same order as in the WindowAttr() function.
ਸਥਿਰ ਅੱਖਰ *ਖਿੜਕੀ_Attrs[] = {
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
    ਨਿਰਾ
};

    ਸਥਿਰ PyObject *
ਖਿੜਕੀ_Dir(PyObject *self, PyObject *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ ObjectDir(self, ਖਿੜਕੀ_Attrs);
}

    ਸਥਿਰ PyObject *
ਖਿੜਕੀ_Attr_Valid(ਖਿੜਕੀ_Object *self, ਅੱਖਰ *name)
{
    PyObject	*ret;

    ਜੇ (strcmp(name, "valid") != 0)
	ਵਾਪਸ ਨਿਰਾ;

    ret = ((self->win == INVALID_ਖਿੜਕੀ_ਮੁੱਲ) ? Py_False : Py_True);
    Py_INCREF(ret);
    ਵਾਪਸ ret;
}

    ਸਥਿਰ PyObject *
ਖਿੜਕੀ_Attr(ਖਿੜਕੀ_Object *self, ਅੱਖਰ *name)
{
    ਜੇ (strcmp(name, "buffer") == 0)
	ਵਾਪਸ (PyObject *)ਬਫਰ_New(self->win->w_ਬਫਰ);
    ਨਹੀਂਤਾਂ ਜੇ (strcmp(name, "cursor") == 0)
    {
	pos_T *pos = &self->win->w_cursor;

	ਵਾਪਸ Py_Build_ਮੁੱਲ("(ll)", (ਲੰਮਾ)(pos->lnum), (ਲੰਮਾ)(pos->ਖਾਨਾ));
    }
    ਨਹੀਂਤਾਂ ਜੇ (strcmp(name, "height") == 0)
	ਵਾਪਸ PyLong_FromLong((ਲੰਮਾ)(self->win->w_ਉਚਾਈ));
    ਨਹੀਂਤਾਂ ਜੇ (strcmp(name, "row") == 0)
	ਵਾਪਸ PyLong_FromLong((ਲੰਮਾ)(self->win->w_winrow));
    ਨਹੀਂਤਾਂ ਜੇ (strcmp(name, "width") == 0)
	ਵਾਪਸ PyLong_FromLong((ਲੰਮਾ)(self->win->w_ਚੌੜਾਈ));
    ਨਹੀਂਤਾਂ ਜੇ (strcmp(name, "col") == 0)
	ਵਾਪਸ PyLong_FromLong((ਲੰਮਾ)(self->win->w_wincol));
    ਨਹੀਂਤਾਂ ਜੇ (strcmp(name, "vars") == 0)
	ਵਾਪਸ NEW_DICTIONARY(self->win->w_vars);
    ਨਹੀਂਤਾਂ ਜੇ (strcmp(name, "options") == 0)
	ਵਾਪਸ OptionsNew(SREQ_WIN, self->win, Check_ਖਿੜਕੀ_Cb,
			(PyObject *) self);
    ਨਹੀਂਤਾਂ ਜੇ (strcmp(name, "number") == 0)
    {
	ਜੇ (Check_Tab_ਪੰਨਾ(self->tabObject))
	    ਵਾਪਸ ਨਿਰਾ;
	ਵਾਪਸ PyLong_FromLong((ਲੰਮਾ)
		get_win_ਸੰਖਿਆ(self->win, get_firstwin(self->tabObject)));
    }
    ਨਹੀਂਤਾਂ ਜੇ (strcmp(name, "tabpage") == 0)
    {
	Py_INCREF((PyObject *)self->tabObject);
	ਵਾਪਸ (PyObject *)(self->tabObject);
    }
    ਨਹੀਂਤਾਂ ਜੇ (strcmp(name, "__members__") == 0)
	ਵਾਪਸ ObjectDir(ਨਿਰਾ, ਖਿੜਕੀ_Attrs);
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ ਨਿਰਾ;
}

    ਸਥਿਰ ਪੂਰਨ
ਖਿੜਕੀ_Setattr(PyObject *self_obj, ਅੱਖਰ *name, PyObject *valObject)
{
    ਖਿੜਕੀ_Object *self = (ਖਿੜਕੀ_Object*)self_obj;
    ਜੇ (Check_ਖਿੜਕੀ(self))
	ਵਾਪਸ -1;

    ਜੇ (strcmp(name, "buffer") == 0)
    {
	Py_Err_ਸੈੱਟ_STRING(PyExc_TypeError, N_("readonly attribute: buffer"));
	ਵਾਪਸ -1;
    }
    ਨਹੀਂਤਾਂ ਜੇ (strcmp(name, "cursor") == 0)
    {
	ਲੰਮਾ lnum;
	ਲੰਮਾ ਖਾਨਾ;

	ਜੇ (!PyArg_Parse(valObject, "(ll)", &lnum, &ਖਾਨਾ))
	    ਵਾਪਸ -1;

	ਜੇ (lnum <= 0 || lnum > self->win->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	{
	    Py_Err_ਸੈੱਟ_VIM(N_("cursor position outside buffer"));
	    ਵਾਪਸ -1;
	}

	// Check for keyboard interrupts
	ਜੇ (Vim_Check_ਰੁਕਾਵਟ())
	    ਵਾਪਸ -1;

	self->win->w_cursor.lnum = lnum;
	self->win->w_cursor.ਖਾਨਾ = ਖਾਨਾ;
	self->win->w_ਸੈੱਟ_curswant = TRUE;
	self->win->w_cursor.coladd = 0;
	// When column is out of range silently correct it.
	check_cursor_ਖਾਨਾ_win(self->win);

	update_screen(UPD_VALID);
	ਵਾਪਸ 0;
    }
    ਨਹੀਂਤਾਂ ਜੇ (strcmp(name, "height") == 0)
    {
	ਲੰਮਾ	ਉਚਾਈ;
	win_T	*savewin;

	ਜੇ (ਸੰਖਿਆ_ਮੰਜ਼ਿਲ_Long(valObject, &ਉਚਾਈ, ਸੰਖਿਆ_INT|ਸੰਖਿਆ_UNSIGNED))
	    ਵਾਪਸ -1;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	need_mouse_correct = TRUE;
#ਜੇ_ਅੰਤ
	savewin = curwin;
	curwin = self->win;
	curbuf = curwin->w_ਬਫਰ;

	Vim_Try_ਸ਼ੁਰੂ();
	win_setheight((ਪੂਰਨ) ਉਚਾਈ);
	curwin = savewin;
	curbuf = curwin->w_ਬਫਰ;
	ਜੇ (Vim_Try_ਸਿਰਾ())
	    ਵਾਪਸ -1;

	ਵਾਪਸ 0;
    }
    ਨਹੀਂਤਾਂ ਜੇ (strcmp(name, "width") == 0)
    {
	ਲੰਮਾ	ਚੌੜਾਈ;
	win_T	*savewin;

	ਜੇ (ਸੰਖਿਆ_ਮੰਜ਼ਿਲ_Long(valObject, &ਚੌੜਾਈ, ਸੰਖਿਆ_INT|ਸੰਖਿਆ_UNSIGNED))
	    ਵਾਪਸ -1;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	need_mouse_correct = TRUE;
#ਜੇ_ਅੰਤ
	savewin = curwin;
	curwin = self->win;
	curbuf = curwin->w_ਬਫਰ;

	Vim_Try_ਸ਼ੁਰੂ();
	win_setwidth((ਪੂਰਨ) ਚੌੜਾਈ);
	curwin = savewin;
	curbuf = curwin->w_ਬਫਰ;
	ਜੇ (Vim_Try_ਸਿਰਾ())
	    ਵਾਪਸ -1;

	ਵਾਪਸ 0;
    }
    ਨਹੀਂਤਾਂ
    {
	Py_Err_ਸੈੱਟ_String(PyExc_AttributeError, name);
	ਵਾਪਸ -1;
    }
}

    ਸਥਿਰ PyObject *
ਖਿੜਕੀ_Repr(PyObject *self_obj)
{
    ਖਿੜਕੀ_Object *self = (ਖਿੜਕੀ_Object*)self_obj;
    ਜੇ (self->win == INVALID_ਖਿੜਕੀ_ਮੁੱਲ)
	ਵਾਪਸ PyString_FromFormat("<window object (deleted) at %p>", (ਖਾਲੀ *)self);
    ਨਹੀਂਤਾਂ
    {
	ਪੂਰਨ	w = get_win_ਸੰਖਿਆ(self->win, firstwin);

	ਜੇ (w == 0)
	    ਵਾਪਸ PyString_FromFormat("<window object (unknown) at %p>",
								      (ਖਾਲੀ *)self);
	ਨਹੀਂਤਾਂ
	    ਵਾਪਸ PyString_FromFormat("<window %d>", w - 1);
    }
}

ਸਥਿਰ ਬਣਤਰ Py_ਵਿਧੀ_Def ਖਿੜਕੀ_Methods[] = {
    // name,	    function,			calling,	documentation
    {"__dir__",	    (PyCFunction)ਖਿੜਕੀ_Dir,	METH_NOARGS,	""},
    { ਨਿਰਾ,	    ਨਿਰਾ,			0,		ਨਿਰਾ}
};

/*
 * Window list object
 */

DEFINE_PY_TYPE_OBJECT(WinListType);
ਸਥਿਰ PySequenceMethods WinListAsSeq;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    Py_Object_ਸਿਰ
    Tab_ਪੰਨਾ_Object	*tabObject;
} WinListObject;

ਸਥਿਰ WinListObject The_ਖਿੜਕੀ_List =
{
    Py_Object_ਸਿਰ_INIT_TYPE(WinListType)
    ਨਿਰਾ
};

    ਸਥਿਰ PyObject *
WinListNew(Tab_ਪੰਨਾ_Object *tabObject)
{
    WinListObject	*self;

    self = PyObject_NEW(WinListObject, WinListTypePtr);
    self->tabObject = tabObject;
    Py_INCREF((PyObject *)tabObject);

    ਵਾਪਸ (PyObject *)(self);
}

    ਸਥਿਰ ਖਾਲੀ
WinListDestructor(WinListObject *self)
{
    Tab_ਪੰਨਾ_Object	*tabObject = self->tabObject;

    ਜੇ (tabObject)
    {
	Py_DECREF((PyObject *)(tabObject));
    }

    DESTRUCTOR_ਅੰਤ(self);
}

    ਸਥਿਰ PyInt
Win_List_ਲੰਮਾਈ(WinListObject *self)
{
    win_T	*w;
    PyInt	n = 0;

    ਜੇ (!(w = get_firstwin(self->tabObject)))
	ਵਾਪਸ -1;

    ਜਦੋਂ (w != ਨਿਰਾ)
    {
	++n;
	w = W_ਅਗਲਾ(w);
    }

    ਵਾਪਸ n;
}

    ਸਥਿਰ PyObject *
Win_List_ਵਸਤੂ(WinListObject *self, PyInt n)
{
    win_T *w;

    ਜੇ (!(w = get_firstwin(self->tabObject)))
	ਵਾਪਸ ਨਿਰਾ;

    ਲਈ (; w != ਨਿਰਾ; w = W_ਅਗਲਾ(w), --n)
	ਜੇ (n == 0)
	    ਵਾਪਸ ਖਿੜਕੀ_New(w, self->tabObject? self->tabObject->tab: curtab);

    Py_Err_ਸੈੱਟ_STRING(Py_Exc_ਸੂਚਕਾਂਕ_Error, N_("no such window"));
    ਵਾਪਸ ਨਿਰਾ;
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
    ਸਥਿਰ ਅੱਖਰ *
String_ਮੰਜ਼ਿਲ_ਸਤਰ(PyObject *obj)
{
    ਅੱਖਰ	*str;
    ਅੱਖਰ	*save;
    PyObject	*ਬਾਈਟਾਂ = ਨਿਰਾ;
    Py_ssize_t	len = 0;
    PyInt	i;
    ਅੱਖਰ	*p;

    ਜੇ (Py_ਬਾਈਟਾਂ_Check(obj))
    {
	ਜੇ (Py_ਬਾਈਟਾਂ_As_String_And_ਮਾਪ(obj, &str, &len) == -1
		|| str == ਨਿਰਾ)
	    ਵਾਪਸ ਨਿਰਾ;
    }
    ਨਹੀਂਤਾਂ ਜੇ (PyUnicode_Check(obj))
    {
	ਜੇ (!(ਬਾਈਟਾਂ = PyUnicode_AsEncodedString(obj, ENC_OPT,
							   ERRORS_ENCODE_ARG)))
	    ਵਾਪਸ ਨਿਰਾ;

	ਜੇ (Py_ਬਾਈਟਾਂ_As_String_And_ਮਾਪ(ਬਾਈਟਾਂ, &str, &len) == -1
		|| str == ਨਿਰਾ)
	{
	    Py_DECREF(ਬਾਈਟਾਂ);
	    ਵਾਪਸ ਨਿਰਾ;
	}
    }
    ਨਹੀਂਤਾਂ
    {
#ਜੇ PY_MAJOR_ਸੰਸਕਰਣ < 3
	PyErr_FORMAT_TYPE(
		N_("expected str() or unicode() instance, but got %s"),
		obj);
#ਨਹੀਂਤਾਂ
	PyErr_FORMAT_TYPE(
		N_("expected bytes() or str() instance, but got %s"),
		obj);
#ਜੇ_ਅੰਤ
	ਵਾਪਸ ਨਿਰਾ;
    }

    /*
     * Error checking: String must not contain newlines, as we
     * are replacing a single line, and we must replace it with
     * a single line.
     * A trailing newline is removed, so that append(f.readlines()) works.
     */
    p = memchr(str, '\n', len);
    ਜੇ (p != ਨਿਰਾ)
    {
	ਜੇ (p == str + len - 1)
	    --len;
	ਨਹੀਂਤਾਂ
	{
	    Py_Err_ਸੈੱਟ_VIM(N_("string cannot contain newlines"));
	    Py_XDECREF(ਬਾਈਟਾਂ);
	    ਵਾਪਸ ਨਿਰਾ;
	}
    }

    /*
     * Create a copy of the string, with internal nulls replaced by
     * newline characters, as is the Vim convention.
     */
    save = alloc(len+1);
    ਜੇ (save == ਨਿਰਾ)
    {
	Py_Err_No_ਸਮ੍ਰਿਤੀ();
	Py_XDECREF(ਬਾਈਟਾਂ);
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਲਈ (i = 0; i < len; ++i)
    {
	ਜੇ (str[i] == '\0')
	    save[i] = '\n';
	ਨਹੀਂਤਾਂ
	    save[i] = str[i];
    }

    save[i] = '\0';
    Py_XDECREF(ਬਾਈਟਾਂ);  // Python 2 does nothing here

    ਵਾਪਸ save;
}

/*
 * Get a line from the specified buffer. The line number is
 * in Vim format (1-based). The line is returned as a Python
 * string object.
 */
    ਸਥਿਰ PyObject *
Get_ਬਫਰ_ਸਤਰ(buf_T *buf, PyInt n)
{
    ਵਾਪਸ ਸਤਰ_ਮੰਜ਼ਿਲ_String((ਅੱਖਰ *)ml_get_buf(buf, (linenr_T)n, FALSE));
}


/*
 * Get a list of lines from the specified buffer. The line numbers
 * are in Vim format (1-based). The range is from lo up to, but not
 * including, hi. The list is returned as a Python list of string objects.
 */
    ਸਥਿਰ PyObject *
Get_ਬਫਰ_ਸਤਰ_List(buf_T *buf, PyInt lo, PyInt hi)
{
    PyInt	i;
    PyInt	n = hi - lo;
    PyObject	*list = PyList_New(n);

    ਜੇ (list == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ਲਈ (i = 0; i < n; ++i)
    {
	linenr_T	lnum = (linenr_T)(lo + i);
	ਅੱਖਰ		*ਪਾਠ;
	PyObject	*string;

	ਜੇ (lnum > buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	    ਪਾਠ = "";
	ਨਹੀਂਤਾਂ
	    ਪਾਠ = (ਅੱਖਰ *)ml_get_buf(buf, lnum, FALSE);
	string = ਸਤਰ_ਮੰਜ਼ਿਲ_String(ਪਾਠ);
	ਜੇ (string == ਨਿਰਾ)
	{
	    Py_DECREF(list);
	    ਵਾਪਸ ਨਿਰਾ;
	}

	Py_List_ਸੈੱਟ_ਵਸਤੂ(list, i, string);
    }

    // The ownership of the Python list is passed to the caller (ie,
    // the caller should Py_DECREF() the object when it is finished
    // with it).

    ਵਾਪਸ list;
}

/*
 * Check if deleting lines made the cursor position invalid.
 * Changed the lines from "lo" to "hi" and added "extra" lines (negative if
 * deleted).
 */
    ਸਥਿਰ ਖਾਲੀ
py_fix_cursor(linenr_T lo, linenr_T hi, linenr_T extra)
{
    ਜੇ (curwin->w_cursor.lnum >= lo)
    {
	// Adjust the cursor position if it's in/after the changed
	// lines.
	ਜੇ (curwin->w_cursor.lnum >= hi)
	{
	    curwin->w_cursor.lnum += extra;
	    check_cursor_ਖਾਨਾ();
	}
	ਨਹੀਂਤਾਂ ਜੇ (extra < 0)
	{
	    curwin->w_cursor.lnum = lo;
	    check_cursor();
	}
	ਨਹੀਂਤਾਂ
	    check_cursor_ਖਾਨਾ();
	ਬਦਲਿਆ_cline_bef_curs();
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
    ਸਥਿਰ ਪੂਰਨ
ਸੈੱਟ_ਬਫਰ_ਸਤਰ_2(buf_T *buf, PyInt n, PyObject *ਸਤਰ, PyInt *len_change)
{
    bufref_T	save_curbuf = {ਨਿਰਾ, 0, 0};
    switchwin_T	switchwin;

    // First of all, we check the type of the supplied Python object.
    // There are three cases:
    //	  1. NULL, or None - this is a deletion.
    //	  2. A string	   - this is a replacement.
    //	  3. Anything else - this is an error.
    ਜੇ (ਸਤਰ == Py_None || ਸਤਰ == ਨਿਰਾ)
    {
	PyErr_Clear();
	switchwin.sw_curwin = ਨਿਰਾ;
	switch_ਮੰਜ਼ਿਲ_win_for_buf(buf, &switchwin, &save_curbuf);

	Vim_Try_ਸ਼ੁਰੂ();

	ਜੇ (u_savedel((linenr_T)n, 1L) == FAIL)
	    RAISE_UNDO_FAIL;
	ਨਹੀਂਤਾਂ ਜੇ (ml_delete((linenr_T)n) == FAIL)
	    RAISE_DELETE_ਸਤਰ_FAIL;
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (buf == curbuf && (switchwin.sw_curwin != ਨਿਰਾ
					   || save_curbuf.br_buf == ਨਿਰਾ))
		// Using an existing window for the buffer, adjust the cursor
		// position.
		py_fix_cursor((linenr_T)n, (linenr_T)n + 1, (linenr_T)-1);
	    ਜੇ (save_curbuf.br_buf == ਨਿਰਾ)
		// Only adjust marks if we managed to switch to a window that
		// holds the buffer, otherwise line numbers will be invalid.
		deleted_ਸਤਰਾਂ_mark((linenr_T)n, 1L);
	}

	restore_win_for_buf(&switchwin, &save_curbuf);

	ਜੇ (Vim_Try_ਸਿਰਾ())
	    ਵਾਪਸ FAIL;

	ਜੇ (len_change)
	    *len_change = -1;

	ਵਾਪਸ OK;
    }
    ਨਹੀਂਤਾਂ ਜੇ (Py_ਬਾਈਟਾਂ_Check(ਸਤਰ) || PyUnicode_Check(ਸਤਰ))
    {
	ਅੱਖਰ	*save = String_ਮੰਜ਼ਿਲ_ਸਤਰ(ਸਤਰ);

	ਜੇ (save == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;

	Vim_Try_ਸ਼ੁਰੂ();

	// We do not need to free "save" if ml_replace() consumes it.
	PyErr_Clear();
	switch_ਮੰਜ਼ਿਲ_win_for_buf(buf, &switchwin, &save_curbuf);

	ਜੇ (u_savesub((linenr_T)n) == FAIL)
	{
	    RAISE_UNDO_FAIL;
	    vim_free(save);
	}
	ਨਹੀਂਤਾਂ ਜੇ (ml_replace((linenr_T)n, (char_u *)save, FALSE) == FAIL)
	{
	    RAISE_REPLACE_ਸਤਰ_FAIL;
	    vim_free(save);
	}
	ਨਹੀਂਤਾਂ
	    ਬਦਲਿਆ_ਬਾਈਟਾਂ((linenr_T)n, 0);

	restore_win_for_buf(&switchwin, &save_curbuf);

	// Check that the cursor is not beyond the end of the line now.
	ਜੇ (buf == curbuf)
	    check_cursor_ਖਾਨਾ();

	ਜੇ (Vim_Try_ਸਿਰਾ())
	    ਵਾਪਸ FAIL;

	ਜੇ (len_change)
	    *len_change = 0;

	ਵਾਪਸ OK;
    }
    ਨਹੀਂਤਾਂ
    {
	Py_Err_Bad_ਦਲੀਲ();
	ਵਾਪਸ FAIL;
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
    ਸਥਿਰ ਪੂਰਨ
ਸੈੱਟ_ਬਫਰ_ਸਤਰ_List(
	buf_T *buf,
	PyInt lo,
	PyInt hi,
	PyObject *list,
	PyInt *len_change)
{
    bufref_T	save_curbuf = {ਨਿਰਾ, 0, 0};
    switchwin_T	switchwin;

    // First of all, we check the type of the supplied Python object.
    // There are three cases:
    //	  1. NULL, or None - this is a deletion.
    //	  2. A list	   - this is a replacement.
    //	  3. Anything else - this is an error.
    ਜੇ (list == Py_None || list == ਨਿਰਾ)
    {
	PyInt	i;
	PyInt	n = (ਪੂਰਨ)(hi - lo);

	PyErr_Clear();
	Vim_Try_ਸ਼ੁਰੂ();
	switchwin.sw_curwin = ਨਿਰਾ;
	switch_ਮੰਜ਼ਿਲ_win_for_buf(buf, &switchwin, &save_curbuf);

	ਜੇ (u_savedel((linenr_T)lo, (ਲੰਮਾ)n) == FAIL)
	    RAISE_UNDO_FAIL;
	ਨਹੀਂਤਾਂ
	{
	    ਲਈ (i = 0; i < n; ++i)
	    {
		ਜੇ (ml_delete((linenr_T)lo) == FAIL)
		{
		    RAISE_DELETE_ਸਤਰ_FAIL;
		    ਰੋਕੋ;
		}
	    }
	    ਜੇ (buf == curbuf && (switchwin.sw_curwin != ਨਿਰਾ
					       || save_curbuf.br_buf == ਨਿਰਾ))
		// Using an existing window for the buffer, adjust the cursor
		// position.
		py_fix_cursor((linenr_T)lo, (linenr_T)hi, (linenr_T)-n);
	    ਜੇ (save_curbuf.br_buf == ਨਿਰਾ)
		// Only adjust marks if we managed to switch to a window that
		// holds the buffer, otherwise line numbers will be invalid.
		deleted_ਸਤਰਾਂ_mark((linenr_T)lo, (ਲੰਮਾ)i);
	}

	restore_win_for_buf(&switchwin, &save_curbuf);

	ਜੇ (Vim_Try_ਸਿਰਾ())
	    ਵਾਪਸ FAIL;

	ਜੇ (len_change)
	    *len_change = -n;

	ਵਾਪਸ OK;
    }
    ਨਹੀਂਤਾਂ ਜੇ (PyList_Check(list))
    {
	PyInt	i;
	PyInt	new_len = Py_List_ਮਾਪ(list);
	PyInt	old_len = hi - lo;
	PyInt	extra = 0;	// lines added to text, can be negative
	ਅੱਖਰ	**array;

	ਜੇ (new_len == 0)	// avoid allocating zero bytes
	    array = ਨਿਰਾ;
	ਨਹੀਂਤਾਂ
	{
	    array = PyMem_New(ਅੱਖਰ *, new_len);
	    ਜੇ (array == ਨਿਰਾ)
	    {
		Py_Err_No_ਸਮ੍ਰਿਤੀ();
		ਵਾਪਸ FAIL;
	    }
	}

	ਲਈ (i = 0; i < new_len; ++i)
	{
	    PyObject *ਸਤਰ;

	    ਜੇ (!(ਸਤਰ = Py_List_Get_ਵਸਤੂ(list, i)) ||
		!(array[i] = String_ਮੰਜ਼ਿਲ_ਸਤਰ(ਸਤਰ)))
	    {
		ਜਦੋਂ (i)
		    vim_free(array[--i]);
		PyMem_Free(array);
		ਵਾਪਸ FAIL;
	    }
	}

	Vim_Try_ਸ਼ੁਰੂ();
	PyErr_Clear();

	// START of region without "return".  Must call restore_buffer()!
	switchwin.sw_curwin = ਨਿਰਾ;
	switch_ਮੰਜ਼ਿਲ_win_for_buf(buf, &switchwin, &save_curbuf);

	ਜੇ (u_save((linenr_T)(lo-1), (linenr_T)hi) == FAIL)
	    RAISE_UNDO_FAIL;

	// If the size of the range is reducing (ie, new_len < old_len) we
	// need to delete some old_len. We do this at the start, by
	// repeatedly deleting line "lo".
	ਜੇ (!PyErr_Occurred())
	{
	    ਲਈ (i = 0; i < old_len - new_len; ++i)
		ਜੇ (ml_delete((linenr_T)lo) == FAIL)
		{
		    RAISE_DELETE_ਸਤਰ_FAIL;
		    ਰੋਕੋ;
		}
	    extra -= i;
	}

	// For as long as possible, replace the existing old_len with the
	// new old_len. This is a more efficient operation, as it requires
	// less memory allocation and freeing.
	ਜੇ (!PyErr_Occurred())
	{
	    ਲਈ (i = 0; i < old_len && i < new_len; ++i)
		ਜੇ (ml_replace((linenr_T)(lo+i), (char_u *)array[i], FALSE)
								      == FAIL)
		{
		    RAISE_REPLACE_ਸਤਰ_FAIL;
		    ਰੋਕੋ;
		}
	}
	ਨਹੀਂਤਾਂ
	    i = 0;

	// Now we may need to insert the remaining new old_len. If we do, we
	// must free the strings as we finish with them (we can't pass the
	// responsibility to Vim in this case).
	ਜੇ (!PyErr_Occurred())
	{
	    ਜਦੋਂ (i < new_len)
	    {
		ਜੇ (ml_append((linenr_T)(lo + i - 1),
					(char_u *)array[i], 0, FALSE) == FAIL)
		{
		    RAISE_ਪਾਓ_ਸਤਰ_FAIL;
		    ਰੋਕੋ;
		}
		vim_free(array[i]);
		++i;
		++extra;
	    }
	}

	// Free any left-over old_len, as a result of an error
	ਜਦੋਂ (i < new_len)
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
	ਜੇ (save_curbuf.br_buf == ਨਿਰਾ)
	{
	    mark_adjust((linenr_T)lo, (linenr_T)(hi - 1),
						  (ਲੰਮਾ)MAXLNUM, (ਲੰਮਾ)extra);
	    ਬਦਲਿਆ_ਸਤਰਾਂ((linenr_T)lo, 0, (linenr_T)hi, (ਲੰਮਾ)extra);
	}

	ਜੇ (buf == curbuf && (switchwin.sw_curwin != ਨਿਰਾ
					   || save_curbuf.br_buf == ਨਿਰਾ))
	    // Using an existing window for the buffer, adjust the cursor
	    // position.
	    py_fix_cursor((linenr_T)lo, (linenr_T)hi, (linenr_T)extra);

	// END of region without "return".
	restore_win_for_buf(&switchwin, &save_curbuf);

	ਜੇ (Vim_Try_ਸਿਰਾ())
	    ਵਾਪਸ FAIL;

	ਜੇ (len_change)
	    *len_change = new_len - old_len;

	ਵਾਪਸ OK;
    }
    ਨਹੀਂਤਾਂ
    {
	Py_Err_Bad_ਦਲੀਲ();
	ਵਾਪਸ FAIL;
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
    ਸਥਿਰ ਪੂਰਨ
ਪਾਓ_ਬਫਰ_ਸਤਰਾਂ(buf_T *buf, PyInt n, PyObject *ਸਤਰਾਂ, PyInt *len_change)
{
    bufref_T	save_curbuf = {ਨਿਰਾ, 0, 0};
    switchwin_T	switchwin;

    // First of all, we check the type of the supplied Python object.
    // It must be a string or a list, or the call is in error.
    ਜੇ (Py_ਬਾਈਟਾਂ_Check(ਸਤਰਾਂ) || PyUnicode_Check(ਸਤਰਾਂ))
    {
	ਅੱਖਰ		*str = String_ਮੰਜ਼ਿਲ_ਸਤਰ(ਸਤਰਾਂ);

	ਜੇ (str == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;

	PyErr_Clear();
	Vim_Try_ਸ਼ੁਰੂ();
	switch_ਮੰਜ਼ਿਲ_win_for_buf(buf, &switchwin, &save_curbuf);

	ਜੇ (u_save((linenr_T)n, (linenr_T)(n + 1)) == FAIL)
	    RAISE_UNDO_FAIL;
	ਨਹੀਂਤਾਂ ਜੇ (ml_append((linenr_T)n, (char_u *)str, 0, FALSE) == FAIL)
	    RAISE_ਪਾਓ_ਸਤਰ_FAIL;
	ਨਹੀਂਤਾਂ ਜੇ (save_curbuf.br_buf == ਨਿਰਾ)
	    // Only adjust marks if we managed to switch to a window that
	    // holds the buffer, otherwise line numbers will be invalid.
	    appended_ਸਤਰਾਂ_mark((linenr_T)n, 1L);

	vim_free(str);
	restore_win_for_buf(&switchwin, &save_curbuf);
	update_screen(UPD_VALID);

	ਜੇ (Vim_Try_ਸਿਰਾ())
	    ਵਾਪਸ FAIL;

	ਜੇ (len_change)
	    *len_change = 1;

	ਵਾਪਸ OK;
    }
    ਨਹੀਂਤਾਂ ਜੇ (PyList_Check(ਸਤਰਾਂ))
    {
	PyInt	i;
	PyInt	ਮਾਪ = Py_List_ਮਾਪ(ਸਤਰਾਂ);
	ਅੱਖਰ	**array;

	array = PyMem_New(ਅੱਖਰ *, ਮਾਪ);
	ਜੇ (array == ਨਿਰਾ)
	{
	    Py_Err_No_ਸਮ੍ਰਿਤੀ();
	    ਵਾਪਸ FAIL;
	}

	ਲਈ (i = 0; i < ਮਾਪ; ++i)
	{
	    PyObject *ਸਤਰ;

	    ਜੇ (!(ਸਤਰ = Py_List_Get_ਵਸਤੂ(ਸਤਰਾਂ, i)) ||
		!(array[i] = String_ਮੰਜ਼ਿਲ_ਸਤਰ(ਸਤਰ)))
	    {
		ਜਦੋਂ (i)
		    vim_free(array[--i]);
		PyMem_Free(array);
		ਵਾਪਸ FAIL;
	    }
	}

	PyErr_Clear();
	Vim_Try_ਸ਼ੁਰੂ();
	switch_ਮੰਜ਼ਿਲ_win_for_buf(buf, &switchwin, &save_curbuf);

	ਜੇ (u_save((linenr_T)n, (linenr_T)(n + 1)) == FAIL)
	    RAISE_UNDO_FAIL;
	ਨਹੀਂਤਾਂ
	{
	    ਲਈ (i = 0; i < ਮਾਪ; ++i)
	    {
		ਜੇ (ml_append((linenr_T)(n + i),
					(char_u *)array[i], 0, FALSE) == FAIL)
		{
		    RAISE_ਪਾਓ_ਸਤਰ_FAIL;

		    // Free the rest of the lines
		    ਜਦੋਂ (i < ਮਾਪ)
			vim_free(array[i++]);

		    ਰੋਕੋ;
		}
		vim_free(array[i]);
	    }
	    ਜੇ (i > 0 && save_curbuf.br_buf == ਨਿਰਾ)
		// Only adjust marks if we managed to switch to a window that
		// holds the buffer, otherwise line numbers will be invalid.
		appended_ਸਤਰਾਂ_mark((linenr_T)n, (ਲੰਮਾ)i);
	}

	// Free the array of lines. All of its contents have now
	// been freed.
	PyMem_Free(array);
	restore_win_for_buf(&switchwin, &save_curbuf);

	update_screen(UPD_VALID);

	ਜੇ (Vim_Try_ਸਿਰਾ())
	    ਵਾਪਸ FAIL;

	ਜੇ (len_change)
	    *len_change = ਮਾਪ;

	ਵਾਪਸ OK;
    }
    ਨਹੀਂਤਾਂ
    {
	Py_Err_Bad_ਦਲੀਲ();
	ਵਾਪਸ FAIL;
    }
}

/*
 * Common routines for buffers and line ranges
 * -------------------------------------------
 */

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    Py_Object_ਸਿਰ
    buf_T *buf;
} ਬਫਰ_Object;

    ਸਥਿਰ ਪੂਰਨ
Check_ਬਫਰ(ਬਫਰ_Object *self)
{
    ਜੇ (self->buf == INVALID_ਬਫਰ_ਮੁੱਲ)
    {
	Py_Err_ਸੈੱਟ_VIM(N_("attempt to refer to deleted buffer"));
	ਵਾਪਸ -1;
    }

    ਵਾਪਸ 0;
}

    ਸਥਿਰ ਪੂਰਨ
Check_ਬਫਰ_Cb(ਖਾਲੀ *self)
{
    ਵਾਪਸ Check_ਬਫਰ((ਬਫਰ_Object*)self);
}

    ਸਥਿਰ PyObject *
RBItem(ਬਫਰ_Object *self, PyInt n, PyInt ਸ਼ੁਰੂ, PyInt ਸਿਰਾ)
{
    ਜੇ (Check_ਬਫਰ(self))
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (ਸਿਰਾ == -1)
	ਸਿਰਾ = self->buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;

    ਜੇ (n < 0)
	n += ਸਿਰਾ - ਸ਼ੁਰੂ + 1;

    ਜੇ (n < 0 || n > ਸਿਰਾ - ਸ਼ੁਰੂ)
    {
	Py_Err_ਸੈੱਟ_STRING(Py_Exc_ਸੂਚਕਾਂਕ_Error, N_("line number out of range"));
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਵਾਪਸ Get_ਬਫਰ_ਸਤਰ(self->buf, n+ਸ਼ੁਰੂ);
}

    ਸਥਿਰ PyObject *
RBSlice(ਬਫਰ_Object *self, PyInt lo, PyInt hi, PyInt ਸ਼ੁਰੂ, PyInt ਸਿਰਾ)
{
    PyInt ਮਾਪ;

    ਜੇ (Check_ਬਫਰ(self))
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (ਸਿਰਾ == -1)
	ਸਿਰਾ = self->buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;

    ਮਾਪ = ਸਿਰਾ - ਸ਼ੁਰੂ + 1;

    ਜੇ (lo < 0)
	lo = 0;
    ਨਹੀਂਤਾਂ ਜੇ (lo > ਮਾਪ)
	lo = ਮਾਪ;
    ਜੇ (hi < 0)
	hi = 0;
    ਜੇ (hi < lo)
	hi = lo;
    ਨਹੀਂਤਾਂ ਜੇ (hi > ਮਾਪ)
	hi = ਮਾਪ;

    ਵਾਪਸ Get_ਬਫਰ_ਸਤਰ_List(self->buf, lo+ਸ਼ੁਰੂ, hi+ਸ਼ੁਰੂ);
}

    ਸਥਿਰ PyInt
RBAs_ਵਸਤੂ(
	ਬਫਰ_Object *self,
	PyInt n,
	PyObject *valObject,
	PyInt ਸ਼ੁਰੂ,
	PyInt ਸਿਰਾ,
	PyInt *new_ਸਿਰਾ)
{
    PyInt len_change;

    ਜੇ (Check_ਬਫਰ(self))
	ਵਾਪਸ -1;

    ਜੇ (ਸਿਰਾ == -1)
	ਸਿਰਾ = self->buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;

    ਜੇ (n < 0)
	n += ਸਿਰਾ - ਸ਼ੁਰੂ + 1;

    ਜੇ (n < 0 || n > ਸਿਰਾ - ਸ਼ੁਰੂ)
    {
	Py_Err_ਸੈੱਟ_STRING(Py_Exc_ਸੂਚਕਾਂਕ_Error, N_("line number out of range"));
	ਵਾਪਸ -1;
    }

    ਜੇ (ਸੈੱਟ_ਬਫਰ_ਸਤਰ_2(self->buf, n+ਸ਼ੁਰੂ, valObject, &len_change) == FAIL)
	ਵਾਪਸ -1;

    ਜੇ (new_ਸਿਰਾ)
	*new_ਸਿਰਾ = ਸਿਰਾ + len_change;

    ਵਾਪਸ 0;
}

    ਸਥਿਰ PyInt
RBAs_ਟੁਕੜਾ(
	ਬਫਰ_Object *self,
	PyInt lo,
	PyInt hi,
	PyObject *valObject,
	PyInt ਸ਼ੁਰੂ,
	PyInt ਸਿਰਾ,
	PyInt *new_ਸਿਰਾ)
{
    PyInt ਮਾਪ;
    PyInt len_change;

    // Self must be a valid buffer
    ਜੇ (Check_ਬਫਰ(self))
	ਵਾਪਸ -1;

    ਜੇ (ਸਿਰਾ == -1)
	ਸਿਰਾ = self->buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;

    // Sort out the slice range
    ਮਾਪ = ਸਿਰਾ - ਸ਼ੁਰੂ + 1;

    ਜੇ (lo < 0)
	lo = 0;
    ਨਹੀਂਤਾਂ ਜੇ (lo > ਮਾਪ)
	lo = ਮਾਪ;
    ਜੇ (hi < 0)
	hi = 0;
    ਜੇ (hi < lo)
	hi = lo;
    ਨਹੀਂਤਾਂ ਜੇ (hi > ਮਾਪ)
	hi = ਮਾਪ;

    ਜੇ (ਸੈੱਟ_ਬਫਰ_ਸਤਰ_List(self->buf, lo + ਸ਼ੁਰੂ, hi + ਸ਼ੁਰੂ,
						valObject, &len_change) == FAIL)
	ਵਾਪਸ -1;

    ਜੇ (new_ਸਿਰਾ)
	*new_ਸਿਰਾ = ਸਿਰਾ + len_change;

    ਵਾਪਸ 0;
}


    ਸਥਿਰ PyObject *
RBAppend(
	ਬਫਰ_Object *self,
	PyObject *ਦਲੀਲਾਂ,
	PyInt ਸ਼ੁਰੂ,
	PyInt ਸਿਰਾ,
	PyInt *new_ਸਿਰਾ)
{
    PyObject *ਸਤਰਾਂ;
    PyInt len_change;
    PyInt max;
    PyInt n;

    ਜੇ (Check_ਬਫਰ(self))
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (ਸਿਰਾ == -1)
	ਸਿਰਾ = self->buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;

    max = n = ਸਿਰਾ - ਸ਼ੁਰੂ + 1;

    ਜੇ (!PyArg_ParseTuple(ਦਲੀਲਾਂ, "O|n", &ਸਤਰਾਂ, &n))
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (n < 0 || n > max)
    {
	Py_Err_ਸੈੱਟ_STRING(Py_Exc_ਸੂਚਕਾਂਕ_Error, N_("line number out of range"));
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਜੇ (ਪਾਓ_ਬਫਰ_ਸਤਰਾਂ(self->buf, n + ਸ਼ੁਰੂ - 1, ਸਤਰਾਂ, &len_change) == FAIL)
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (new_ਸਿਰਾ)
	*new_ਸਿਰਾ = ਸਿਰਾ + len_change;

    Py_INCREF(Py_None);
    ਵਾਪਸ Py_None;
}

// Range object

DEFINE_PY_TYPE_OBJECT(RangeType);
ਸਥਿਰ PySequenceMethods RangeAsSeq;
ਸਥਿਰ PyMappingMethods RangeAsMapping;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    Py_Object_ਸਿਰ
    ਬਫਰ_Object *buf;
    PyInt ਸ਼ੁਰੂ;
    PyInt ਸਿਰਾ;
} RangeObject;

    ਸਥਿਰ PyObject *
RangeNew(buf_T *buf, PyInt ਸ਼ੁਰੂ, PyInt ਸਿਰਾ)
{
    ਬਫਰ_Object *bufr;
    RangeObject *self;
    self = PyObject_GC_New(RangeObject, RangeTypePtr);
    ਜੇ (self == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    bufr = (ਬਫਰ_Object *)ਬਫਰ_New(buf);
    ਜੇ (bufr == ਨਿਰਾ)
    {
	Py_DECREF(self);
	ਵਾਪਸ ਨਿਰਾ;
    }
    Py_INCREF((PyObject *)bufr);

    self->buf = bufr;
    self->ਸ਼ੁਰੂ = ਸ਼ੁਰੂ;
    self->ਸਿਰਾ = ਸਿਰਾ;

    ਵਾਪਸ (PyObject *)(self);
}

    ਸਥਿਰ ਖਾਲੀ
RangeDestructor(PyObject *self_obj)
{
    RangeObject *self = (RangeObject*)self_obj;
    PyObject_GC_UnTrack((ਖਾਲੀ *)(self));
    Py_XDECREF(((PyObject *)(self->buf)));
    PyObject_GC_Del((ਖਾਲੀ *)(self));
}

    ਸਥਿਰ ਪੂਰਨ
RangeTraverse(PyObject *self_obj, visitproc ਦੇਖੋ, ਖਾਲੀ *arg)
{
    RangeObject *self = (RangeObject*)self_obj;
    Py_ਦੇਖੋ(((PyObject *)(self->buf)));
    ਵਾਪਸ 0;
}

    ਸਥਿਰ ਪੂਰਨ
RangeClear(PyObject *self_obj)
{
    RangeObject *self = (RangeObject*)self_obj;
    Py_CLEAR(self->buf);
    ਵਾਪਸ 0;
}

    ਸਥਿਰ PyInt
Range_ਲੰਮਾਈ(RangeObject *self)
{
    // HOW DO WE SIGNAL AN ERROR FROM THIS FUNCTION?
    ਜੇ (Check_ਬਫਰ(self->buf))
	ਵਾਪਸ -1; // ???

    ਵਾਪਸ (self->ਸਿਰਾ - self->ਸ਼ੁਰੂ + 1);
}

    ਸਥਿਰ PyObject *
Range_ਵਸਤੂ(RangeObject *self, PyInt n)
{
    ਵਾਪਸ RBItem(self->buf, n, self->ਸ਼ੁਰੂ, self->ਸਿਰਾ);
}

    ਸਥਿਰ PyObject *
Range_ਟੁਕੜਾ(RangeObject *self, PyInt lo, PyInt hi)
{
    ਵਾਪਸ RBSlice(self->buf, lo, hi, self->ਸ਼ੁਰੂ, self->ਸਿਰਾ);
}

ਸਥਿਰ ਅੱਖਰ *RangeAttrs[] = {
    "start", "end",
    ਨਿਰਾ
};

    ਸਥਿਰ PyObject *
RangeDir(PyObject *self, PyObject *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ ObjectDir(self, RangeAttrs);
}

    ਸਥਿਰ PyObject *
RangeAppend(RangeObject *self, PyObject *ਦਲੀਲਾਂ)
{
    ਵਾਪਸ RBAppend(self->buf, ਦਲੀਲਾਂ, self->ਸ਼ੁਰੂ, self->ਸਿਰਾ, &self->ਸਿਰਾ);
}

    ਸਥਿਰ PyObject *
RangeRepr(PyObject *self_obj)
{
    RangeObject *self = (RangeObject*)self_obj;
    ਜੇ (self->buf->buf == INVALID_ਬਫਰ_ਮੁੱਲ)
	ਵਾਪਸ PyString_FromFormat("<range object (for deleted buffer) at %p>",
				    (ਖਾਲੀ *)self);
    ਨਹੀਂਤਾਂ
    {
	ਅੱਖਰ *name = (ਅੱਖਰ *)self->buf->buf->b_fname;

	ਜੇ (name == ਨਿਰਾ)
	    name = "";

	ਵਾਪਸ PyString_FromFormat("<range %s (%d:%d)>",
				    name, (ਪੂਰਨ)self->ਸ਼ੁਰੂ, (ਪੂਰਨ)self->ਸਿਰਾ);
    }
}

ਸਥਿਰ ਬਣਤਰ Py_ਵਿਧੀ_Def RangeMethods[] = {
    // name,	function,			calling,	documentation
    {"append",	(PyCFunction)RangeAppend,	METH_VARARGS,	"Append data to the Vim range" },
    {"__dir__",	(PyCFunction)RangeDir,		METH_NOARGS,	""},
    { ਨਿਰਾ,	ਨਿਰਾ,				0,		ਨਿਰਾ}
};

DEFINE_PY_TYPE_OBJECT(ਬਫਰ_Type);
ਸਥਿਰ PySequenceMethods ਬਫਰ_As_Seq;
ਸਥਿਰ PyMappingMethods ਬਫਰ_As_Mapping;

    ਸਥਿਰ PyObject *
ਬਫਰ_New(buf_T *buf)
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

    ਬਫਰ_Object *self;

    ਜੇ (BUF_PYTHON_REF(buf) != ਨਿਰਾ)
    {
	self = BUF_PYTHON_REF(buf);
	Py_INCREF((PyObject *)self);
    }
    ਨਹੀਂਤਾਂ
    {
	self = PyObject_NEW(ਬਫਰ_Object, ਬਫਰ_Type_Ptr);
	ਜੇ (self == ਨਿਰਾ)
	    ਵਾਪਸ ਨਿਰਾ;
	self->buf = buf;
	BUF_PYTHON_REF(buf) = self;
    }

    ਵਾਪਸ (PyObject *)(self);
}

    ਸਥਿਰ ਖਾਲੀ
ਬਫਰ_Destructor(PyObject *self_obj)
{
    ਬਫਰ_Object *self = (ਬਫਰ_Object*)self_obj;
    ਜੇ (self->buf && self->buf != INVALID_ਬਫਰ_ਮੁੱਲ)
	BUF_PYTHON_REF(self->buf) = ਨਿਰਾ;

    DESTRUCTOR_ਅੰਤ(self);
}

    ਸਥਿਰ PyInt
ਬਫਰ_ਲੰਮਾਈ(ਬਫਰ_Object *self)
{
    // HOW DO WE SIGNAL AN ERROR FROM THIS FUNCTION?
    ਜੇ (Check_ਬਫਰ(self))
	ਵਾਪਸ -1; // ???

    ਵਾਪਸ (PyInt)(self->buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ);
}

    ਸਥਿਰ PyObject *
ਬਫਰ_ਵਸਤੂ(ਬਫਰ_Object *self, PyInt n)
{
    ਵਾਪਸ RBItem(self, n, 1, -1);
}

    ਸਥਿਰ PyObject *
ਬਫਰ_ਟੁਕੜਾ(ਬਫਰ_Object *self, PyInt lo, PyInt hi)
{
    ਵਾਪਸ RBSlice(self, lo, hi, 1, -1);
}

ਸਥਿਰ ਅੱਖਰ *ਬਫਰ_Attrs[] = {
    "name", "number", "vars", "options", "valid",
    ਨਿਰਾ
};

    ਸਥਿਰ PyObject *
ਬਫਰ_Dir(PyObject *self, PyObject *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ ObjectDir(self, ਬਫਰ_Attrs);
}

    ਸਥਿਰ PyObject *
ਬਫਰ_Attr_Valid(ਬਫਰ_Object *self, ਅੱਖਰ *name)
{
    PyObject	*ret;

    ਜੇ (strcmp(name, "valid") != 0)
	ਵਾਪਸ ਨਿਰਾ;

    ret = ((self->buf == INVALID_ਬਫਰ_ਮੁੱਲ) ? Py_False : Py_True);
    Py_INCREF(ret);
    ਵਾਪਸ ret;
}

    ਸਥਿਰ PyObject *
ਬਫਰ_Attr(ਬਫਰ_Object *self, ਅੱਖਰ *name)
{
    ਜੇ (strcmp(name, "name") == 0)
	ਵਾਪਸ PyString_FromString((self->buf->b_ffname == ਨਿਰਾ
				    ? "" : (ਅੱਖਰ *)self->buf->b_ffname));
    ਨਹੀਂਤਾਂ ਜੇ (strcmp(name, "number") == 0)
	ਵਾਪਸ Py_Build_ਮੁੱਲ(Py_ssize_t_fmt, self->buf->b_fnum);
    ਨਹੀਂਤਾਂ ਜੇ (strcmp(name, "vars") == 0)
	ਵਾਪਸ NEW_DICTIONARY(self->buf->b_vars);
    ਨਹੀਂਤਾਂ ਜੇ (strcmp(name, "options") == 0)
	ਵਾਪਸ OptionsNew(SREQ_BUF, self->buf, Check_ਬਫਰ_Cb,
			(PyObject *) self);
    ਨਹੀਂਤਾਂ ਜੇ (strcmp(name, "__members__") == 0)
	ਵਾਪਸ ObjectDir(ਨਿਰਾ, ਬਫਰ_Attrs);
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ ਨਿਰਾ;
}

    ਸਥਿਰ ਪੂਰਨ
ਬਫਰ_Setattr(PyObject *self_obj, ਅੱਖਰ *name, PyObject *valObject)
{
    ਬਫਰ_Object *self = (ਬਫਰ_Object*)self_obj;
    ਜੇ (Check_ਬਫਰ(self))
	ਵਾਪਸ -1;

    ਜੇ (strcmp(name, "name") == 0)
    {
	char_u		*val;
	aco_save_T	aco;
	ਪੂਰਨ		ren_ret = OK;
	PyObject	*todecref;

	ਜੇ (!(val = String_ਮੰਜ਼ਿਲ_Chars(valObject, &todecref)))
	    ਵਾਪਸ -1;

	Vim_Try_ਸ਼ੁਰੂ();
	// Using aucmd_*: autocommands will be executed by rename_buffer
	aucmd_prepbuf(&aco, self->buf);
	ਜੇ (curbuf == self->buf)
	{
	    ren_ret = rename_ਬਫਰ(val);
	    aucmd_restbuf(&aco);
	}
	Py_XDECREF(todecref);
	ਜੇ (Vim_Try_ਸਿਰਾ())
	    ਵਾਪਸ -1;

	ਜੇ (ren_ret == FAIL)
	{
	    Py_Err_ਸੈੱਟ_VIM(N_("failed to rename buffer"));
	    ਵਾਪਸ -1;
	}
	ਵਾਪਸ 0;
    }
    ਨਹੀਂਤਾਂ
    {
	Py_Err_ਸੈੱਟ_String(PyExc_AttributeError, name);
	ਵਾਪਸ -1;
    }
}

    ਸਥਿਰ PyObject *
ਬਫਰ_Append(ਬਫਰ_Object *self, PyObject *ਦਲੀਲਾਂ)
{
    ਵਾਪਸ RBAppend(self, ਦਲੀਲਾਂ, 1, -1, ਨਿਰਾ);
}

    ਸਥਿਰ PyObject *
ਬਫਰ_Mark(ਬਫਰ_Object *self, PyObject *pmarkObject)
{
    pos_T	*posp;
    char_u	*pmark;
    char_u	mark;
    bufref_T	savebuf;
    PyObject	*todecref;

    ਜੇ (Check_ਬਫਰ(self))
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (!(pmark = String_ਮੰਜ਼ਿਲ_Chars(pmarkObject, &todecref)))
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (pmark[0] == '\0' || pmark[1] != '\0')
    {
	Py_Err_ਸੈੱਟ_STRING(Py_Exc_ਮੁੱਲ_Error,
		N_("mark name must be a single character"));
	Py_XDECREF(todecref);
	ਵਾਪਸ ਨਿਰਾ;
    }

    mark = *pmark;

    Py_XDECREF(todecref);

    Vim_Try_ਸ਼ੁਰੂ();
    switch_ਬਫਰ(&savebuf, self->buf);
    posp = getmark(mark, FALSE);
    restore_ਬਫਰ(&savebuf);
    ਜੇ (Vim_Try_ਸਿਰਾ())
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (posp == ਨਿਰਾ)
    {
	Py_Err_ਸੈੱਟ_VIM(N_("invalid mark name"));
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਜੇ (posp->lnum <= 0)
    {
	// Or raise an error?
	Py_INCREF(Py_None);
	ਵਾਪਸ Py_None;
    }

    ਵਾਪਸ Py_Build_ਮੁੱਲ("(ll)", (ਲੰਮਾ)(posp->lnum), (ਲੰਮਾ)(posp->ਖਾਨਾ));
}

    ਸਥਿਰ PyObject *
ਬਫਰ_Range(ਬਫਰ_Object *self, PyObject *ਦਲੀਲਾਂ)
{
    PyInt ਸ਼ੁਰੂ;
    PyInt ਸਿਰਾ;

    ਜੇ (Check_ਬਫਰ(self))
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (!PyArg_ParseTuple(ਦਲੀਲਾਂ, "nn", &ਸ਼ੁਰੂ, &ਸਿਰਾ))
	ਵਾਪਸ ਨਿਰਾ;

    ਵਾਪਸ RangeNew(self->buf, ਸ਼ੁਰੂ, ਸਿਰਾ);
}

    ਸਥਿਰ PyObject *
ਬਫਰ_Repr(PyObject *self_obj)
{
    ਬਫਰ_Object *self = (ਬਫਰ_Object*)self_obj;
    ਜੇ (self->buf == INVALID_ਬਫਰ_ਮੁੱਲ)
	ਵਾਪਸ PyString_FromFormat("<buffer object (deleted) at %p>", (ਖਾਲੀ *)self);
    ਨਹੀਂਤਾਂ
    {
	ਅੱਖਰ	*name = (ਅੱਖਰ *)self->buf->b_fname;

	ਜੇ (name == ਨਿਰਾ)
	    name = "";

	ਵਾਪਸ PyString_FromFormat("<buffer %s>", name);
    }
}

ਸਥਿਰ ਬਣਤਰ Py_ਵਿਧੀ_Def ਬਫਰ_Methods[] = {
    // name,	    function,			calling,	documentation
    {"append",	    (PyCFunction)ਬਫਰ_Append,	METH_VARARGS,	"Append data to Vim buffer" },
    {"mark",	    (PyCFunction)ਬਫਰ_Mark,	METH_O,		"Return (row,col) representing position of named mark" },
    {"range",	    (PyCFunction)ਬਫਰ_Range,	METH_VARARGS,	"Return a range object which represents the part of the given buffer between line numbers s and e" },
    {"__dir__",	    (PyCFunction)ਬਫਰ_Dir,	METH_NOARGS,	""},
    { ਨਿਰਾ,	    ਨਿਰਾ,			0,		ਨਿਰਾ}
};

/*
 * Buffer list object - Implementation
 */

DEFINE_PY_TYPE_OBJECT(BufMapType);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    Py_Object_ਸਿਰ
} BufMapObject;

ਸਥਿਰ BufMapObject The_ਬਫਰ_Map =
{
    Py_Object_ਸਿਰ_INIT_TYPE(BufMapType)
};

    ਸਥਿਰ PyInt
Buf_Map_ਲੰਮਾਈ(PyObject *self ਅਣਵਰਤਿਆ)
{
    buf_T	*b = firstbuf;
    PyInt	n = 0;

    ਜਦੋਂ (b)
    {
	++n;
	b = b->b_ਅਗਲਾ;
    }

    ਵਾਪਸ n;
}

    ਸਥਿਰ PyObject *
Buf_Map_ਵਸਤੂ(PyObject *self ਅਣਵਰਤਿਆ, PyObject *ਕੁੰਜੀ_Object)
{
    buf_T	*b;
    ਲੰਮਾ	bnr;

    ਜੇ (ਸੰਖਿਆ_ਮੰਜ਼ਿਲ_Long(ਕੁੰਜੀ_Object, &bnr, ਸੰਖਿਆ_INT|ਸੰਖਿਆ_NATURAL))
	ਵਾਪਸ ਨਿਰਾ;

    b = buflist_findnr((ਪੂਰਨ) bnr);

    ਜੇ (b)
	ਵਾਪਸ ਬਫਰ_New(b);
    ਨਹੀਂਤਾਂ
    {
	Py_Err_ਸੈੱਟ_Object(Py_Exc_ਕੁੰਜੀ_Error, ਕੁੰਜੀ_Object);
	ਵਾਪਸ ਨਿਰਾ;
    }
}

    ਸਥਿਰ ਖਾਲੀ
BufMapIterDestruct(ਖਾਲੀ* arg)
{
    PyObject *ਬਫਰ = (PyObject*)arg;
    // Iteration was stopped before all buffers were processed
    ਜੇ (ਬਫਰ)
    {
	Py_DECREF(ਬਫਰ);
    }
}

    ਸਥਿਰ ਪੂਰਨ
BufMapIterTraverse(ਖਾਲੀ *iter, visitproc ਦੇਖੋ, ਖਾਲੀ *arg)
{
    PyObject *ਬਫਰ = (PyObject*)iter;
    ਜੇ (ਬਫਰ)
	Py_ਦੇਖੋ(ਬਫਰ);
    ਵਾਪਸ 0;
}

    ਸਥਿਰ ਪੂਰਨ
BufMapIterClear(ਖਾਲੀ **iter)
{
    PyObject **ਬਫਰ = (PyObject**)iter;
    ਜੇ (*ਬਫਰ)
	Py_CLEAR(*ਬਫਰ);
    ਵਾਪਸ 0;
}

    ਸਥਿਰ PyObject *
Buf_Map_Iter_ਅਗਲਾ(ਖਾਲੀ **arg)
{
    PyObject	*ਅਗਲਾ;
    PyObject	*ret;
    PyObject	**ਬਫਰ = (PyObject**)arg;

    ਜੇ (!*ਬਫਰ)
	ਵਾਪਸ ਨਿਰਾ;

    ret = *ਬਫਰ;

    ਜੇ (Check_ਬਫਰ((ਬਫਰ_Object *)(ret)))
    {
	*ਬਫਰ = ਨਿਰਾ;
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਜੇ (!((ਬਫਰ_Object *)(ret))->buf->b_ਅਗਲਾ)
	ਅਗਲਾ = ਨਿਰਾ;
    ਨਹੀਂਤਾਂ ਜੇ (!(ਅਗਲਾ = ਬਫਰ_New(((ਬਫਰ_Object *)(ret))->buf->b_ਅਗਲਾ)))
	ਵਾਪਸ ਨਿਰਾ;
    *ਬਫਰ = ਅਗਲਾ;
    // Do not increment reference: we no longer hold it (decref), but whoever
    // on other side will hold (incref). Decref+incref = nothing.
    ਵਾਪਸ ret;
}

    ਸਥਿਰ PyObject *
BufMapIter(PyObject *self)
{
    PyObject *ਬਫਰ;

    ਬਫਰ = ਬਫਰ_New(firstbuf);
    ਵਾਪਸ IterNew(ਬਫਰ,
	    BufMapIterDestruct, Buf_Map_Iter_ਅਗਲਾ,
	    BufMapIterTraverse, BufMapIterClear,
	    self);
}

ਸਥਿਰ PyMappingMethods BufMapAsMapping = {
    (lenfunc)       Buf_Map_ਲੰਮਾਈ,
    (binaryfunc)    Buf_Map_ਵਸਤੂ,
    (objobjargproc) 0,
};

// Current items object

ਸਥਿਰ ਅੱਖਰ *CurrentAttrs[] = {
    "buffer", "window", "line", "range", "tabpage",
    ਨਿਰਾ
};

    ਸਥਿਰ PyObject *
CurrentDir(PyObject *self, PyObject *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ ObjectDir(self, CurrentAttrs);
}

    ਸਥਿਰ PyObject *
CurrentGetattr(PyObject *self ਅਣਵਰਤਿਆ, ਅੱਖਰ *name)
{
    ਜੇ (strcmp(name, "buffer") == 0)
	ਵਾਪਸ (PyObject *)ਬਫਰ_New(curbuf);
    ਨਹੀਂਤਾਂ ਜੇ (strcmp(name, "window") == 0)
	ਵਾਪਸ (PyObject *)ਖਿੜਕੀ_New(curwin, curtab);
    ਨਹੀਂਤਾਂ ਜੇ (strcmp(name, "tabpage") == 0)
	ਵਾਪਸ (PyObject *)Tab_ਪੰਨਾ_New(curtab);
    ਨਹੀਂਤਾਂ ਜੇ (strcmp(name, "line") == 0)
	ਵਾਪਸ Get_ਬਫਰ_ਸਤਰ(curbuf, (PyInt)curwin->w_cursor.lnum);
    ਨਹੀਂਤਾਂ ਜੇ (strcmp(name, "range") == 0)
	ਵਾਪਸ RangeNew(curbuf, Range_ਸ਼ੁਰੂ, Range_ਸਿਰਾ);
    ਨਹੀਂਤਾਂ ਜੇ (strcmp(name, "__members__") == 0)
	ਵਾਪਸ ObjectDir(ਨਿਰਾ, CurrentAttrs);
    ਨਹੀਂਤਾਂ
#ਜੇ PY_MAJOR_ਸੰਸਕਰਣ < 3
	ਵਾਪਸ Py_ਲੱਭੋ_ਵਿਧੀ(ਖਿੜਕੀ_Methods, self, name);
#ਨਹੀਂਤਾਂ
	ਵਾਪਸ ਨਿਰਾ;
#ਜੇ_ਅੰਤ
}

    ਸਥਿਰ ਪੂਰਨ
CurrentSetattr(PyObject *self ਅਣਵਰਤਿਆ, ਅੱਖਰ *name, PyObject *valObject)
{
    ਜੇ (strcmp(name, "line") == 0)
    {
	ਜੇ (ਸੈੱਟ_ਬਫਰ_ਸਤਰ_2(curbuf, (PyInt)curwin->w_cursor.lnum, valObject,
			  ਨਿਰਾ) == FAIL)
	    ਵਾਪਸ -1;

	ਵਾਪਸ 0;
    }
    ਨਹੀਂਤਾਂ ਜੇ (strcmp(name, "buffer") == 0)
    {
	ਪੂਰਨ ਗਿਣਤੀ_2;

	ਜੇ (valObject->ob_type != ਬਫਰ_Type_Ptr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.Buffer object, but got %s"),
		    valObject);
	    ਵਾਪਸ -1;
	}

	ਜੇ (Check_ਬਫਰ((ਬਫਰ_Object *)(valObject)))
	    ਵਾਪਸ -1;
	ਗਿਣਤੀ_2 = ((ਬਫਰ_Object *)(valObject))->buf->b_fnum;

	Vim_Try_ਸ਼ੁਰੂ();
	ਜੇ (do_ਬਫਰ(DOBUF_GOTO, DOBUF_ਪਹਿਲਾ, FORWARD, ਗਿਣਤੀ_2, 0) == FAIL)
	{
	    ਜੇ (Vim_Try_ਸਿਰਾ())
		ਵਾਪਸ -1;
	    PyErr_VIM_FORMAT(N_("failed to switch to buffer %d"), ਗਿਣਤੀ_2);
	    ਵਾਪਸ -1;
	}

	ਵਾਪਸ Vim_Try_ਸਿਰਾ();
    }
    ਨਹੀਂਤਾਂ ਜੇ (strcmp(name, "window") == 0)
    {
	ਪੂਰਨ ਗਿਣਤੀ_2;

	ਜੇ (valObject->ob_type != ਖਿੜਕੀ_Type_Ptr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.Window object, but got %s"),
		    valObject);
	    ਵਾਪਸ -1;
	}

	ਜੇ (Check_ਖਿੜਕੀ((ਖਿੜਕੀ_Object *)(valObject)))
	    ਵਾਪਸ -1;
	ਗਿਣਤੀ_2 = get_win_ਸੰਖਿਆ(((ਖਿੜਕੀ_Object *)(valObject))->win, firstwin);

	ਜੇ (!ਗਿਣਤੀ_2)
	{
	    Py_Err_ਸੈੱਟ_STRING(Py_Exc_ਮੁੱਲ_Error,
		    N_("failed to find window in the current tab page"));
	    ਵਾਪਸ -1;
	}

	Vim_Try_ਸ਼ੁਰੂ();
	win_goto(((ਖਿੜਕੀ_Object *)(valObject))->win);
	ਜੇ (((ਖਿੜਕੀ_Object *)(valObject))->win != curwin)
	{
	    ਜੇ (Vim_Try_ਸਿਰਾ())
		ਵਾਪਸ -1;
	    Py_Err_ਸੈੱਟ_STRING(PyExc_RuntimeError,
		    N_("did not switch to the specified window"));
	    ਵਾਪਸ -1;
	}

	ਵਾਪਸ Vim_Try_ਸਿਰਾ();
    }
    ਨਹੀਂਤਾਂ ਜੇ (strcmp(name, "tabpage") == 0)
    {
	ਜੇ (valObject->ob_type != Tab_ਪੰਨਾ_Type_Ptr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.TabPage object, but got %s"),
		    valObject);
	    ਵਾਪਸ -1;
	}

	ਜੇ (Check_Tab_ਪੰਨਾ((Tab_ਪੰਨਾ_Object *)(valObject)))
	    ਵਾਪਸ -1;

	Vim_Try_ਸ਼ੁਰੂ();
	goto_tabpage_tp(((Tab_ਪੰਨਾ_Object *)(valObject))->tab, TRUE, TRUE);
	ਜੇ (((Tab_ਪੰਨਾ_Object *)(valObject))->tab != curtab)
	{
	    ਜੇ (Vim_Try_ਸਿਰਾ())
		ਵਾਪਸ -1;
	    Py_Err_ਸੈੱਟ_STRING(PyExc_RuntimeError,
		    N_("did not switch to the specified tab page"));
	    ਵਾਪਸ -1;
	}

	ਵਾਪਸ Vim_Try_ਸਿਰਾ();
    }
    ਨਹੀਂਤਾਂ
    {
	Py_Err_ਸੈੱਟ_String(PyExc_AttributeError, name);
	ਵਾਪਸ -1;
    }
}

ਸਥਿਰ ਬਣਤਰ Py_ਵਿਧੀ_Def CurrentMethods[] = {
    // name,	    function,			calling,	documentation
    {"__dir__",	    (PyCFunction)CurrentDir,	METH_NOARGS,	""},
    { ਨਿਰਾ,	    ਨਿਰਾ,			0,		ਨਿਰਾ}
};

    ਸਥਿਰ ਖਾਲੀ
init_range_cmd(ਖਾਲੀ *arg)
{
    exarg_T *eap = (exarg_T*)arg;
    Range_ਸ਼ੁਰੂ = eap->line1;
    Range_ਸਿਰਾ = eap->line2;
}

    ਸਥਿਰ ਖਾਲੀ
init_range_eval(ਖਾਲੀ *rettv ਅਣਵਰਤਿਆ)
{
    Range_ਸ਼ੁਰੂ = (PyInt) curwin->w_cursor.lnum;
    Range_ਸਿਰਾ = Range_ਸ਼ੁਰੂ;
}

    ਸਥਿਰ ਖਾਲੀ
ਲੜੀਵਾਰ_cmd(ਅਚਲ ਅੱਖਰ *cmd, dict_T* locals ਅਣਵਰਤਿਆ, ਖਾਲੀ *arg ਅਣਵਰਤਿਆ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ PY_CAN_RECURSE
	, PyGILState_STATE *pygilstate ਅਣਵਰਤਿਆ
#ਜੇ_ਅੰਤ
	)
{
    PyObject	*ਲੜੀਵਾਰ_ret;
    ਲੜੀਵਾਰ_ret = Py_ਲੜੀਵਾਰ_String((ਅੱਖਰ *)cmd, Py_file_ਨਿਵੇਸ਼, globals, globals);
    ਜੇ (ਲੜੀਵਾਰ_ret != ਨਿਰਾ)
    {
	Py_DECREF(ਲੜੀਵਾਰ_ret);
    }
    ਨਹੀਂਤਾਂ ਜੇ (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
    {
	emsg(_(e_cant_handle_systemexit_ਦਾ_python_exception_ਵਿੱਚ_vim));
	PyErr_Clear();
    }
    ਨਹੀਂਤਾਂ
	PyErr_PrintEx(1);
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ	*ਕੋਡ_hdr = "def " DOPY_FUNC "(line, linenr):\n ";
ਸਥਿਰ ਪੂਰਨ		ਕੋਡ_hdr_len = 30;

    ਸਥਿਰ ਖਾਲੀ
ਲੜੀਵਾਰ_do(ਅਚਲ ਅੱਖਰ *cmd, dict_T* locals ਅਣਵਰਤਿਆ, ਖਾਲੀ *arg ਅਣਵਰਤਿਆ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ PY_CAN_RECURSE
	, PyGILState_STATE *pygilstate
#ਜੇ_ਅੰਤ
	)
{
    PyInt	lnum;
    size_t	len;
    ਅੱਖਰ	*ਕੋਡ;
    ਪੂਰਨ		ਸਥਿਤੀ;
    PyObject	*pyfunc, *pymain;
    PyObject	*ਲੜੀਵਾਰ_ret;
    buf_T	*was_curbuf = curbuf;

    ਜੇ (u_save((linenr_T)Range_ਸ਼ੁਰੂ - 1, (linenr_T)Range_ਸਿਰਾ + 1) != OK)
    {
	emsg(_("cannot save undo information"));
	ਵਾਪਸ;
    }

    len = ਕੋਡ_hdr_len + STRLEN(cmd);
    ਕੋਡ = PyMem_New(ਅੱਖਰ, len + 1);
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(ਕੋਡ, ਕੋਡ_hdr, ਕੋਡ_hdr_len);
    STRCPY(ਕੋਡ + ਕੋਡ_hdr_len, cmd);
    ਲੜੀਵਾਰ_ret = Py_ਲੜੀਵਾਰ_String(ਕੋਡ, Py_file_ਨਿਵੇਸ਼, globals, globals);
    ਸਥਿਤੀ = -1;
    ਜੇ (ਲੜੀਵਾਰ_ret != ਨਿਰਾ)
    {
	ਸਥਿਤੀ = 0;
	Py_DECREF(ਲੜੀਵਾਰ_ret);
    }
    ਨਹੀਂਤਾਂ ਜੇ (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
    {
	PyMem_Free(ਕੋਡ);
	emsg(_(e_cant_handle_systemexit_ਦਾ_python_exception_ਵਿੱਚ_vim));
	PyErr_Clear();
	ਵਾਪਸ;
    }
    ਨਹੀਂਤਾਂ
	PyErr_PrintEx(1);

    PyMem_Free(ਕੋਡ);

    ਜੇ (ਸਥਿਤੀ)
    {
	emsg(_("failed to run the code"));
	ਵਾਪਸ;
    }

    ਸਥਿਤੀ = 0;
    pymain = Py_Import_ਜੋੜੋ_Module("__main__");
    pyfunc = PyObject_GetAttrString(pymain, DOPY_FUNC);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ PY_CAN_RECURSE
    PyGILState_Release(*pygilstate);
#ਜੇ_ਅੰਤ

    ਲਈ (lnum = Range_ਸ਼ੁਰੂ; lnum <= Range_ਸਿਰਾ; ++lnum)
    {
	PyObject	*ਸਤਰ;
	PyObject	*linenr;
	PyObject	*ret;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ PY_CAN_RECURSE
	*pygilstate = PyGILState_Ensure();
#ਜੇ_ਅੰਤ
	// Check the line number, the command my have deleted lines.
	ਜੇ (lnum > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ
		|| !(ਸਤਰ = Get_ਬਫਰ_ਸਤਰ(curbuf, lnum)))
	    ਜਾਓ err;
	ਜੇ (!(linenr = PyInt_FromLong((ਲੰਮਾ) lnum)))
	{
	    Py_DECREF(ਸਤਰ);
	    ਜਾਓ err;
	}
	ret = Py_Object_Call_Function_Obj_ਦਲੀਲਾਂ(pyfunc, ਸਤਰ, linenr, ਨਿਰਾ);
	Py_DECREF(ਸਤਰ);
	Py_DECREF(linenr);
	ਜੇ (!ret)
	    ਜਾਓ err;

	// Check that the command didn't switch to another buffer.
	// Check the line number, the command my have deleted lines.
	ਜੇ (curbuf != was_curbuf || lnum > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	{
	    Py_XDECREF(ret);
	    ਜਾਓ err;
	}

	ਜੇ (ret != Py_None)
	    ਜੇ (ਸੈੱਟ_ਬਫਰ_ਸਤਰ_2(curbuf, lnum, ret, ਨਿਰਾ) == FAIL)
	    {
		Py_XDECREF(ret);
		ਜਾਓ err;
	    }

	Py_XDECREF(ret);
	PythonIO_Flush();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ PY_CAN_RECURSE
	PyGILState_Release(*pygilstate);
#ਜੇ_ਅੰਤ
    }
    ਜਾਓ ਬਾਹਰਲਾ;
err:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ PY_CAN_RECURSE
    *pygilstate = PyGILState_Ensure();
#ਜੇ_ਅੰਤ
    PyErr_PrintEx(0);
    PythonIO_Flush();
    ਸਥਿਤੀ = 1;
ਬਾਹਰਲਾ:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ PY_CAN_RECURSE
    ਜੇ (!ਸਥਿਤੀ)
	*pygilstate = PyGILState_Ensure();
#ਜੇ_ਅੰਤ
    Py_DECREF(pyfunc);
    Py_Object_ਸੈੱਟ_Attr_String(pymain, DOPY_FUNC, ਨਿਰਾ);
    ਜੇ (ਸਥਿਤੀ)
	ਵਾਪਸ;
    check_cursor();
    update_curbuf(UPD_NOT_VALID);
}

    ਸਥਿਰ ਖਾਲੀ
ਲੜੀਵਾਰ_eval(ਅਚਲ ਅੱਖਰ *cmd, dict_T *locals, ਖਾਲੀ *arg
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ PY_CAN_RECURSE
	, PyGILState_STATE *pygilstate ਅਣਵਰਤਿਆ
#ਜੇ_ਅੰਤ
	)
{
    PyObject	*ਲੜੀਵਾਰ_ret;
    typval_T	*rettv = (typval_T*)arg;
    PyObject	*pylocals = locals ? NEW_DICTIONARY(locals) : globals;

    ਲੜੀਵਾਰ_ret = Py_ਲੜੀਵਾਰ_String((ਅੱਖਰ *)cmd, Py_eval_ਨਿਵੇਸ਼, globals, pylocals);
    ਜੇ (ਲੜੀਵਾਰ_ret == ਨਿਰਾ)
    {
	ਜੇ (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
	{
	    emsg(_(e_cant_handle_systemexit_ਦਾ_python_exception_ਵਿੱਚ_vim));
	    PyErr_Clear();
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (PyErr_Occurred() && !ਸੁਨੇਹਾ_silent)
		PyErr_PrintEx(0);
	    emsg(_(e_eval_did_not_return_valid_python_object));
	}
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (ConvertFromPyObject(ਲੜੀਵਾਰ_ret, rettv) == -1)
	    emsg(_(e_failed_ਮੰਜ਼ਿਲ_convert_returned_python_object_ਮੰਜ਼ਿਲ_vim_ਮੁੱਲ));
	Py_DECREF(ਲੜੀਵਾਰ_ret);
    }
    PyErr_Clear();
}

    ਸਥਿਰ ਪੂਰਨ
ਸੈੱਟ_ref_ਵਿੱਚ_py(ਅਚਲ ਪੂਰਨ copyID)
{
    pylinkedlist_T	*cur;
    list_T		*ll;
    tuple_T		*tt;
    ਪੂਰਨ			i;
    ਪੂਰਨ			abort = FALSE;
    FunctionObject	*func;

    ਜੇ (lastdict != ਨਿਰਾ)
    {
	ਲਈ (cur = lastdict ; !abort && cur != ਨਿਰਾ ; cur = cur->pll_prev)
	    abort = ਸੈੱਟ_ref_ਵਿੱਚ_dict(((DictionaryObject *)(cur->pll_obj))->dict,
								       copyID);
    }

    ਜੇ (lastlist != ਨਿਰਾ)
    {
	ਲਈ (cur = lastlist ; !abort && cur != ਨਿਰਾ ; cur = cur->pll_prev)
	{
	    ll = ((ListObject *) (cur->pll_obj))->list;
	    abort = ਸੈੱਟ_ref_ਵਿੱਚ_list(ll, copyID);
	}
    }

    ਜੇ (lasttuple != ਨਿਰਾ)
    {
	ਲਈ (cur = lasttuple ; !abort && cur != ਨਿਰਾ ; cur = cur->pll_prev)
	{
	    tt = ((TupleObject *) (cur->pll_obj))->tuple;
	    abort = ਸੈੱਟ_ref_ਵਿੱਚ_tuple(tt, copyID);
	}
    }

    ਜੇ (lastfunc != ਨਿਰਾ)
    {
	ਲਈ (cur = lastfunc ; !abort && cur != ਨਿਰਾ ; cur = cur->pll_prev)
	{
	    func = (FunctionObject *) cur->pll_obj;
	    abort = ਸੈੱਟ_ref_ਵਿੱਚ_dict(func->self, copyID);
	    ਜੇ (func->argc)
		ਲਈ (i = 0; !abort && i < func->argc; ++i)
		    abort = abort
			|| ਸੈੱਟ_ref_ਵਿੱਚ_ਵਸਤੂ(&func->argv[i], copyID, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);
	}
    }

    ਵਾਪਸ abort;
}

    ਸਥਿਰ ਪੂਰਨ
ਸੈੱਟ_string_copy(char_u *str, typval_T *tv)
{
    tv->vval.v_string = vim_strsave(str);
    ਜੇ (tv->vval.v_string == ਨਿਰਾ)
    {
	Py_Err_No_ਸਮ੍ਰਿਤੀ();
	ਵਾਪਸ -1;
    }
    ਵਾਪਸ 0;
}

    ਸਥਿਰ ਪੂਰਨ
pydict_ਮੰਜ਼ਿਲ_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    dict_T	*dict;
    char_u	*ਕੁੰਜੀ;
    dictitem_T	*di;
    PyObject	*ਕੁੰਜੀ_Object;
    PyObject	*valObject;
    Py_ssize_t	iter = 0;

    ਜੇ (!(dict = py_dict_alloc()))
	ਵਾਪਸ -1;

    tv->v_type = VAR_DICT;
    tv->vval.v_dict = dict;

    ਜਦੋਂ (Py_Dict_ਅਗਲਾ(obj, &iter, &ਕੁੰਜੀ_Object, &valObject))
    {
	PyObject	*todecref = ਨਿਰਾ;

	ਜੇ (ਕੁੰਜੀ_Object == ਨਿਰਾ || valObject == ਨਿਰਾ)
	{
	    dict_unref(dict);
	    ਵਾਪਸ -1;
	}

	ਜੇ (!(ਕੁੰਜੀ = String_ਮੰਜ਼ਿਲ_Chars(ਕੁੰਜੀ_Object, &todecref)))
	{
	    dict_unref(dict);
	    ਵਾਪਸ -1;
	}

	ਜੇ (*ਕੁੰਜੀ == NUL)
	{
	    dict_unref(dict);
	    Py_XDECREF(todecref);
	    RAISE_NO_ਖਾਲੀ_ਥਾਵਾਂ_ਕੁੰਜੀਆਂ;
	    ਵਾਪਸ -1;
	}

	di = dictitem_alloc(ਕੁੰਜੀ);

	Py_XDECREF(todecref);

	ਜੇ (di == ਨਿਰਾ)
	{
	    Py_Err_No_ਸਮ੍ਰਿਤੀ();
	    dict_unref(dict);
	    ਵਾਪਸ -1;
	}

	ਜੇ (_ConvertFromPyObject(valObject, &di->di_tv, lookup_dict) == -1)
	{
	    vim_free(di);
	    dict_unref(dict);
	    ਵਾਪਸ -1;
	}

	ਜੇ (dict_ਜੋੜੋ(dict, di) == FAIL)
	{
	    RAISE_ਕੁੰਜੀ_ਜੋੜੋ_FAIL(di->di_ਕੁੰਜੀ);
	    clear_tv(&di->di_tv);
	    vim_free(di);
	    dict_unref(dict);
	    ਵਾਪਸ -1;
	}
    }

    --dict->dv_refcount;
    ਵਾਪਸ 0;
}

    ਸਥਿਰ ਪੂਰਨ
pymap_ਮੰਜ਼ਿਲ_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    dict_T	*dict;
    char_u	*ਕੁੰਜੀ;
    dictitem_T	*di;
    PyObject	*list;
    PyObject	*iterator;
    PyObject	*ਕੁੰਜੀ_Object;
    PyObject	*valObject;

    ਜੇ (!(dict = py_dict_alloc()))
	ਵਾਪਸ -1;

    tv->v_type = VAR_DICT;
    tv->vval.v_dict = dict;

    ਜੇ (!(list = Py_Mapping_ਕੁੰਜੀਆਂ(obj)))
    {
	dict_unref(dict);
	ਵਾਪਸ -1;
    }

    ਜੇ (!(iterator = PyObject_GetIter(list)))
    {
	dict_unref(dict);
	Py_DECREF(list);
	ਵਾਪਸ -1;
    }
    Py_DECREF(list);

    ਜਦੋਂ ((ਕੁੰਜੀ_Object = Py_Iter_ਅਗਲਾ(iterator)))
    {
	PyObject	*todecref;

	ਜੇ (!(ਕੁੰਜੀ = String_ਮੰਜ਼ਿਲ_Chars(ਕੁੰਜੀ_Object, &todecref)))
	{
	    Py_DECREF(ਕੁੰਜੀ_Object);
	    Py_DECREF(iterator);
	    dict_unref(dict);
	    ਵਾਪਸ -1;
	}

	ਜੇ (*ਕੁੰਜੀ == NUL)
	{
	    Py_DECREF(ਕੁੰਜੀ_Object);
	    Py_DECREF(iterator);
	    Py_XDECREF(todecref);
	    dict_unref(dict);
	    RAISE_NO_ਖਾਲੀ_ਥਾਵਾਂ_ਕੁੰਜੀਆਂ;
	    ਵਾਪਸ -1;
	}

	ਜੇ (!(valObject = Py_Object_Get_ਵਸਤੂ(obj, ਕੁੰਜੀ_Object)))
	{
	    Py_DECREF(ਕੁੰਜੀ_Object);
	    Py_DECREF(iterator);
	    Py_XDECREF(todecref);
	    dict_unref(dict);
	    ਵਾਪਸ -1;
	}

	di = dictitem_alloc(ਕੁੰਜੀ);

	Py_DECREF(ਕੁੰਜੀ_Object);
	Py_XDECREF(todecref);

	ਜੇ (di == ਨਿਰਾ)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(valObject);
	    dict_unref(dict);
	    Py_Err_No_ਸਮ੍ਰਿਤੀ();
	    ਵਾਪਸ -1;
	}

	ਜੇ (_ConvertFromPyObject(valObject, &di->di_tv, lookup_dict) == -1)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(valObject);
	    vim_free(di);
	    dict_unref(dict);
	    ਵਾਪਸ -1;
	}

	Py_DECREF(valObject);

	ਜੇ (dict_ਜੋੜੋ(dict, di) == FAIL)
	{
	    RAISE_ਕੁੰਜੀ_ਜੋੜੋ_FAIL(di->di_ਕੁੰਜੀ);
	    Py_DECREF(iterator);
	    dictitem_free(di);
	    dict_unref(dict);
	    ਵਾਪਸ -1;
	}
    }
    Py_DECREF(iterator);
    --dict->dv_refcount;
    ਵਾਪਸ 0;
}

    ਸਥਿਰ ਪੂਰਨ
pyseq_ਮੰਜ਼ਿਲ_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    list_T	*l;

    ਜੇ (!(l = py_list_alloc()))
	ਵਾਪਸ -1;

    tv->v_type = VAR_LIST;
    tv->vval.v_list = l;

    ਜੇ (list_py_concat(l, obj, lookup_dict) == -1)
    {
	list_unref(l);
	ਵਾਪਸ -1;
    }

    --l->lv_refcount;
    ਵਾਪਸ 0;
}

    ਸਥਿਰ ਪੂਰਨ
pytuple_ਮੰਜ਼ਿਲ_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    tuple_T	*t;

    ਜੇ (!(t = py_tuple_alloc()))
	ਵਾਪਸ -1;

    tv->v_type = VAR_TUPLE;
    tv->vval.v_tuple = t;

    ਜੇ (tuple_py_concat(t, obj, lookup_dict) == -1)
    {
	tuple_unref(t);
	ਵਾਪਸ -1;
    }

    --t->tv_refcount;
    ਵਾਪਸ 0;
}

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*pytotvfunc)(PyObject *, typval_T *, PyObject *);

    ਸਥਿਰ ਪੂਰਨ
convert_dl(PyObject *obj, typval_T *tv,
				    pytotvfunc py_ਮੰਜ਼ਿਲ_tv, PyObject *lookup_dict)
{
    PyObject	*capsule;
    ਅੱਖਰ	hexBuf[ਆਕਾਰ(ਖਾਲੀ *) * 2 + 3];

    sprintf(hexBuf, "%p", (ਖਾਲੀ *)obj);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ PY_USE_CAPSULE
    capsule = Py_Dict_Get_ਵਸਤੂ_String(lookup_dict, hexBuf);
#ਨਹੀਂਤਾਂ
    capsule = (PyObject *)Py_Dict_Get_ਵਸਤੂ_String(lookup_dict, hexBuf);
#ਜੇ_ਅੰਤ
    ਜੇ (capsule == ਨਿਰਾ)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ PY_USE_CAPSULE
	capsule = PyCapsule_New(tv, ਨਿਰਾ, ਨਿਰਾ);
#ਨਹੀਂਤਾਂ
	capsule = PyCObject_FromVoidPtr(tv, ਨਿਰਾ);
#ਜੇ_ਅੰਤ
	ਜੇ (Py_Dict_ਸੈੱਟ_ਵਸਤੂ_String(lookup_dict, hexBuf, capsule))
	{
	    Py_DECREF(capsule);
	    tv->v_type = VAR_UNKNOWN;
	    ਵਾਪਸ -1;
	}

	Py_DECREF(capsule);

	ਜੇ (py_ਮੰਜ਼ਿਲ_tv(obj, tv, lookup_dict) == -1)
	{
	    tv->v_type = VAR_UNKNOWN;
	    ਵਾਪਸ -1;
	}
	// As we are not using copy_tv which increments reference count we must
	// do it ourself.
	ਜੇ (tv->v_type == VAR_DICT)
	    ++tv->vval.v_dict->dv_refcount;
	ਨਹੀਂਤਾਂ ਜੇ (tv->v_type == VAR_LIST)
	    ++tv->vval.v_list->lv_refcount;
	ਨਹੀਂਤਾਂ ਜੇ (tv->v_type == VAR_TUPLE)
	    ++tv->vval.v_tuple->tv_refcount;
    }
    ਨਹੀਂਤਾਂ
    {
	typval_T	*v;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ PY_USE_CAPSULE
	v = Py_Capsule_Get_ਸੂਚਕ(capsule, ਨਿਰਾ);
#ਨਹੀਂਤਾਂ
	v = PyCObject_AsVoidPtr(capsule);
#ਜੇ_ਅੰਤ
	copy_tv(v, tv);
    }
    ਵਾਪਸ 0;
}

    ਸਥਿਰ ਪੂਰਨ
ConvertFromPyMapping(PyObject *obj, typval_T *tv)
{
    PyObject	*lookup_dict;
    ਪੂਰਨ		ret;

    ਜੇ (!(lookup_dict = PyDict_New()))
	ਵਾਪਸ -1;

    ਜੇ (PyType_IsSubtype(obj->ob_type, DictionaryTypePtr))
    {
	tv->v_type = VAR_DICT;
	tv->vval.v_dict = (((DictionaryObject *)(obj))->dict);
	++tv->vval.v_dict->dv_refcount;
	ret = 0;
    }
    ਨਹੀਂਤਾਂ ਜੇ (PyDict_Check(obj))
	ret = convert_dl(obj, tv, pydict_ਮੰਜ਼ਿਲ_tv, lookup_dict);
    ਨਹੀਂਤਾਂ ਜੇ (PyMapping_Check(obj))
	ret = convert_dl(obj, tv, pymap_ਮੰਜ਼ਿਲ_tv, lookup_dict);
    ਨਹੀਂਤਾਂ
    {
	PyErr_FORMAT_TYPE(
		N_("unable to convert %s to a Vim dictionary"),
		obj);
	ret = -1;
    }
    Py_DECREF(lookup_dict);
    ਵਾਪਸ ret;
}

    ਸਥਿਰ ਪੂਰਨ
ConvertFromPySequence(PyObject *obj, typval_T *tv)
{
    PyObject	*lookup_dict;
    ਪੂਰਨ		ret;

    ਜੇ (!(lookup_dict = PyDict_New()))
	ਵਾਪਸ -1;

    ਜੇ (PyType_IsSubtype(obj->ob_type, ListTypePtr))
    {
	tv->v_type = VAR_LIST;
	tv->vval.v_list = (((ListObject *)(obj))->list);
	++tv->vval.v_list->lv_refcount;
	ret = 0;
    }
    ਨਹੀਂਤਾਂ ਜੇ (PyIter_Check(obj) || PySequence_Check(obj))
	ret = convert_dl(obj, tv, pyseq_ਮੰਜ਼ਿਲ_tv, lookup_dict);
    ਨਹੀਂਤਾਂ
    {
	PyErr_FORMAT_TYPE(
		N_("unable to convert %s to a Vim list"),
		obj);
	ret = -1;
    }
    Py_DECREF(lookup_dict);
    ਵਾਪਸ ret;
}

    ਸਥਿਰ ਪੂਰਨ
ConvertFromPyObject(PyObject *obj, typval_T *tv)
{
    PyObject	*lookup_dict;
    ਪੂਰਨ		ret;

    ਜੇ (!(lookup_dict = PyDict_New()))
	ਵਾਪਸ -1;
    ret = _ConvertFromPyObject(obj, tv, lookup_dict);
    Py_DECREF(lookup_dict);
    ਵਾਪਸ ret;
}

    ਸਥਿਰ ਪੂਰਨ
_ConvertFromPyObject(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    ਜੇ (PyType_IsSubtype(obj->ob_type, DictionaryTypePtr))
    {
	tv->v_type = VAR_DICT;
	tv->vval.v_dict = (((DictionaryObject *)(obj))->dict);
	++tv->vval.v_dict->dv_refcount;
    }
    ਨਹੀਂਤਾਂ ਜੇ (PyType_IsSubtype(obj->ob_type, ListTypePtr))
    {
	tv->v_type = VAR_LIST;
	tv->vval.v_list = (((ListObject *)(obj))->list);
	++tv->vval.v_list->lv_refcount;
    }
    ਨਹੀਂਤਾਂ ਜੇ (PyType_IsSubtype(obj->ob_type, TupleTypePtr))
    {
	tv->v_type = VAR_TUPLE;
	tv->vval.v_tuple = (((TupleObject *)(obj))->tuple);
	++tv->vval.v_tuple->tv_refcount;
    }
    ਨਹੀਂਤਾਂ ਜੇ (PyType_IsSubtype(obj->ob_type, FunctionTypePtr))
    {
	FunctionObject *func = (FunctionObject *) obj;
	ਜੇ (func->self != ਨਿਰਾ || func->argv != ਨਿਰਾ)
	{
	    partial_T *pt = ALLOC_CLEAR_ONE(partial_T);

	    ਸੈੱਟ_partial(func, pt, TRUE);
	    tv->vval.v_partial = pt;
	    tv->v_type = VAR_PARTIAL;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (ਸੈੱਟ_string_copy(func->name, tv) == -1)
		ਵਾਪਸ -1;

	    tv->v_type = VAR_FUNC;
	}
	func_ref(func->name);
    }
    ਨਹੀਂਤਾਂ ਜੇ (Py_ਬਾਈਟਾਂ_Check(obj))
    {
	char_u	*str;

	ਜੇ (Py_ਬਾਈਟਾਂ_As_String_And_ਮਾਪ(obj, (ਅੱਖਰ **) &str, ਨਿਰਾ) == -1)
	    ਵਾਪਸ -1;
	ਜੇ (str == ਨਿਰਾ)
	    ਵਾਪਸ -1;

	ਜੇ (ਸੈੱਟ_string_copy(str, tv) == -1)
	    ਵਾਪਸ -1;

	tv->v_type = VAR_STRING;
    }
    ਨਹੀਂਤਾਂ ਜੇ (PyUnicode_Check(obj))
    {
	PyObject	*ਬਾਈਟਾਂ;
	char_u	*str;

	ਬਾਈਟਾਂ = PyUnicode_AsEncodedString(obj, ENC_OPT, ERRORS_ENCODE_ARG);
	ਜੇ (ਬਾਈਟਾਂ == ਨਿਰਾ)
	    ਵਾਪਸ -1;

	ਜੇ (Py_ਬਾਈਟਾਂ_As_String_And_ਮਾਪ(ਬਾਈਟਾਂ, (ਅੱਖਰ **) &str, ਨਿਰਾ) == -1)
	    ਵਾਪਸ -1;
	ਜੇ (str == ਨਿਰਾ)
	    ਵਾਪਸ -1;

	ਜੇ (ਸੈੱਟ_string_copy(str, tv))
	{
	    Py_XDECREF(ਬਾਈਟਾਂ);
	    ਵਾਪਸ -1;
	}
	Py_XDECREF(ਬਾਈਟਾਂ);

	tv->v_type = VAR_STRING;
    }
#ਜੇ PY_MAJOR_ਸੰਸਕਰਣ < 3
    ਨਹੀਂਤਾਂ ਜੇ (PyInt_Check(obj))
    {
	tv->v_type = VAR_ਸੰਖਿਆ;
	tv->vval.v_ਸੰਖਿਆ = (varnumber_T) PyInt_AsLong(obj);
	ਜੇ (PyErr_Occurred())
	    ਵਾਪਸ -1;
    }
#ਜੇ_ਅੰਤ
    ਨਹੀਂਤਾਂ ਜੇ (PyLong_Check(obj))
    {
	tv->v_type = VAR_ਸੰਖਿਆ;
	tv->vval.v_ਸੰਖਿਆ = (varnumber_T) PyLong_AsLong(obj);
	ਜੇ (PyErr_Occurred())
	    ਵਾਪਸ -1;
    }
    ਨਹੀਂਤਾਂ ਜੇ (PyTuple_Check(obj))
	ਵਾਪਸ convert_dl(obj, tv, pytuple_ਮੰਜ਼ਿਲ_tv, lookup_dict);
    ਨਹੀਂਤਾਂ ਜੇ (PyDict_Check(obj))
	ਵਾਪਸ convert_dl(obj, tv, pydict_ਮੰਜ਼ਿਲ_tv, lookup_dict);
    ਨਹੀਂਤਾਂ ਜੇ (PyFloat_Check(obj))
    {
	tv->v_type = VAR_FLOAT;
	tv->vval.v_float = (float_T) PyFloat_AsDouble(obj);
    }
    ਨਹੀਂਤਾਂ ਜੇ (PyObject_HasAttrString(obj, "keys"))
	ਵਾਪਸ convert_dl(obj, tv, pymap_ਮੰਜ਼ਿਲ_tv, lookup_dict);
    // PyObject_GetIter can create built-in iterator for any sequence object
    ਨਹੀਂਤਾਂ ਜੇ (PyIter_Check(obj) || PySequence_Check(obj))
	ਵਾਪਸ convert_dl(obj, tv, pyseq_ਮੰਜ਼ਿਲ_tv, lookup_dict);
    ਨਹੀਂਤਾਂ ਜੇ (PyMapping_Check(obj))
	ਵਾਪਸ convert_dl(obj, tv, pymap_ਮੰਜ਼ਿਲ_tv, lookup_dict);
    ਨਹੀਂਤਾਂ ਜੇ (Py_ਸੰਖਿਆ_Check(obj))
    {
	PyObject	*ਅੰਕ;

	ਜੇ (!(ਅੰਕ = Py_ਸੰਖਿਆ_Long(obj)))
	    ਵਾਪਸ -1;

	tv->v_type = VAR_ਸੰਖਿਆ;
	tv->vval.v_ਸੰਖਿਆ = (varnumber_T) PyLong_AsLong(ਅੰਕ);

	Py_DECREF(ਅੰਕ);
    }
    ਨਹੀਂਤਾਂ ਜੇ (obj == Py_None)
    {
	tv->v_type = VAR_SPECIAL;
	tv->vval.v_ਸੰਖਿਆ = VVAL_NONE;
    }
    ਨਹੀਂਤਾਂ
    {
	PyErr_FORMAT_TYPE(
		N_("unable to convert %s to a Vim structure"),
		obj);
	ਵਾਪਸ -1;
    }
    ਵਾਪਸ 0;
}

    ਸਥਿਰ PyObject *
Convert_ਮੰਜ਼ਿਲ_Py_Object(typval_T *tv)
{
    typval_T *argv;
    ਪੂਰਨ i;
    ਜੇ (tv == ਨਿਰਾ)
    {
	Py_Err_ਸੈੱਟ_VIM(N_("internal error: NULL reference passed"));
	ਵਾਪਸ ਨਿਰਾ;
    }
    ਚੋਣ (tv->v_type)
    {
	ਹਾਲਤ VAR_STRING:
	    ਵਾਪਸ Py_ਬਾਈਟਾਂ_From_String(tv->vval.v_string == ਨਿਰਾ
					    ? "" : (ਅੱਖਰ *)tv->vval.v_string);
	ਹਾਲਤ VAR_ਸੰਖਿਆ:
	    ਵਾਪਸ PyLong_FromLong((ਲੰਮਾ) tv->vval.v_ਸੰਖਿਆ);
	ਹਾਲਤ VAR_FLOAT:
	    ਵਾਪਸ PyFloat_FromDouble((ਦੁੱਗਣਾ) tv->vval.v_float);
	ਹਾਲਤ VAR_LIST:
	    ਵਾਪਸ NEW_LIST(tv->vval.v_list);
	ਹਾਲਤ VAR_TUPLE:
	    ਵਾਪਸ NEW_TUPLE(tv->vval.v_tuple);
	ਹਾਲਤ VAR_DICT:
	    ਵਾਪਸ NEW_DICTIONARY(tv->vval.v_dict);
	ਹਾਲਤ VAR_FUNC:
	    ਵਾਪਸ NEW_FUNCTION(tv->vval.v_string == ਨਿਰਾ
					  ? (char_u *)"" : tv->vval.v_string,
					  0, ਨਿਰਾ, ਨਿਰਾ, TRUE);
	ਹਾਲਤ VAR_PARTIAL:
	    ਜੇ (tv->vval.v_partial->pt_argc)
	    {
		argv = PyMem_New(typval_T, (size_t)tv->vval.v_partial->pt_argc);
		ਲਈ (i = 0; i < tv->vval.v_partial->pt_argc; i++)
		    copy_tv(&tv->vval.v_partial->pt_argv[i], &argv[i]);
	    }
	    ਨਹੀਂਤਾਂ
		argv = ਨਿਰਾ;
	    ਜੇ (tv->vval.v_partial->pt_dict != ਨਿਰਾ)
		tv->vval.v_partial->pt_dict->dv_refcount++;
	    ਵਾਪਸ NEW_FUNCTION(tv->vval.v_partial == ਨਿਰਾ
			     ? (char_u *)"" : partial_name(tv->vval.v_partial),
				tv->vval.v_partial->pt_argc, argv,
				tv->vval.v_partial->pt_dict,
				tv->vval.v_partial->pt_auto);
	ਹਾਲਤ VAR_BLOB:
	    ਵਾਪਸ Py_ਬਾਈਟਾਂ_From_String_And_ਮਾਪ(
		(ਅੱਖਰ*) tv->vval.v_blob->bv_ga.ga_ਅੰਕੜੇ,
		(Py_ssize_t) tv->vval.v_blob->bv_ga.ga_len);
	ਹਾਲਤ VAR_UNKNOWN:
	ਹਾਲਤ VAR_ANY:
	ਹਾਲਤ VAR_VOID:
	ਹਾਲਤ VAR_CHANNEL:
	ਹਾਲਤ VAR_ਕੰਮ:
	ਹਾਲਤ VAR_INSTR:
	ਹਾਲਤ VAR_CLASS:
	ਹਾਲਤ VAR_OBJECT:
	ਹਾਲਤ VAR_TYPEALIAS:
	    Py_INCREF(Py_None);
	    ਵਾਪਸ Py_None;
	ਹਾਲਤ VAR_BOOL:
	ਹਾਲਤ VAR_SPECIAL:
	    ਚੋਣ (tv->vval.v_ਸੰਖਿਆ)
	    {
		ਹਾਲਤ VVAL_FALSE: ਵਾਪਸ ALWAYS_FALSE;
		ਹਾਲਤ VVAL_TRUE:  ਵਾਪਸ ALWAYS_TRUE;
		ਹਾਲਤ VVAL_NONE:
		ਹਾਲਤ VVAL_NULL:  ਵਾਪਸ ALWAYS_NONE;
	    }
	    Py_Err_ਸੈੱਟ_VIM(N_("internal error: invalid value type"));
	    ਵਾਪਸ ਨਿਰਾ;
    }
    ਵਾਪਸ ਨਿਰਾ;
}

DEFINE_PY_TYPE_OBJECT(CurrentType);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    Py_Object_ਸਿਰ
} CurrentObject;

ਸਥਿਰ CurrentObject TheCurrent =
{
    Py_Object_ਸਿਰ_INIT_TYPE(CurrentType)
};

    ਸਥਿਰ ਖਾਲੀ
init_structs(ਖਾਲੀ)
{
    CLEAR_ਖੇਤਰ(OutputType);
    OutputType.tp_name = "vim.message";
    OutputType.tp_basicsize = ਆਕਾਰ(OutputObject);
    OutputType.tp_flags = Py_TPFLAGS_DEFAULT;
    OutputType.tp_doc = "vim message object";
    OutputType.tp_methods = OutputMethods;
#ਜੇ PY_MAJOR_ਸੰਸਕਰਣ >= 3
    OutputType.tp_getattro = OutputGetattro;
    OutputType.tp_setattro = OutputSetattro;
    OutputType.tp_alloc = call_PyType_GenericAlloc;
    OutputType.tp_new = call_PyType_GenericNew;
    OutputType.tp_free = call_PyObject_Free;
# ifndef USE_LIMITED_API
    // The std printer type is only exposed in full API. It is not essential
    // anyway and so in limited API we don't set it.
    OutputType.tp_ਅਧਾਰ = &PyStdPrinter_Type;
# endif
#ਨਹੀਂਤਾਂ
    OutputType.tp_getattr = OutputGetattr;
    OutputType.tp_setattr = OutputSetattr;
    // Disabled, because this causes a crash in test86
    // OutputType.tp_base = &PyFile_Type;
#ਜੇ_ਅੰਤ

    CLEAR_ਖੇਤਰ(IterType);
    IterType.tp_name = "vim.iter";
    IterType.tp_basicsize = ਆਕਾਰ(IterObject);
    IterType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    IterType.tp_doc = "generic iterator object";
    IterType.tp_iter = IterIter;
    IterType.tp_iternext = Iter_ਅਗਲਾ;
    IterType.tp_dealloc = IterDestructor;
    IterType.tp_traverse = IterTraverse;
    IterType.tp_clear = IterClear;

    CLEAR_ਖੇਤਰ(ਬਫਰ_Type);
    ਬਫਰ_Type.tp_name = "vim.buffer";
    ਬਫਰ_Type.tp_basicsize = ਆਕਾਰ(ਬਫਰ_Object);
    ਬਫਰ_Type.tp_dealloc = ਬਫਰ_Destructor;
    ਬਫਰ_Type.tp_repr = ਬਫਰ_Repr;
    ਬਫਰ_Type.tp_as_sequence = &ਬਫਰ_As_Seq;
    ਬਫਰ_Type.tp_as_mapping = &ਬਫਰ_As_Mapping;
    ਬਫਰ_Type.tp_flags = Py_TPFLAGS_DEFAULT;
    ਬਫਰ_Type.tp_doc = "vim buffer object";
    ਬਫਰ_Type.tp_methods = ਬਫਰ_Methods;
#ਜੇ PY_MAJOR_ਸੰਸਕਰਣ >= 3
    ਬਫਰ_Type.tp_getattro = ਬਫਰ_Getattro;
    ਬਫਰ_Type.tp_setattro = ਬਫਰ_Setattro;
    ਬਫਰ_Type.tp_alloc = call_PyType_GenericAlloc;
    ਬਫਰ_Type.tp_new = call_PyType_GenericNew;
    ਬਫਰ_Type.tp_free = call_PyObject_Free;
#ਨਹੀਂਤਾਂ
    ਬਫਰ_Type.tp_getattr = ਬਫਰ_Getattr;
    ਬਫਰ_Type.tp_setattr = ਬਫਰ_Setattr;
#ਜੇ_ਅੰਤ

    CLEAR_ਖੇਤਰ(ਖਿੜਕੀ_Type);
    ਖਿੜਕੀ_Type.tp_name = "vim.window";
    ਖਿੜਕੀ_Type.tp_basicsize = ਆਕਾਰ(ਖਿੜਕੀ_Object);
    ਖਿੜਕੀ_Type.tp_dealloc = ਖਿੜਕੀ_Destructor;
    ਖਿੜਕੀ_Type.tp_repr = ਖਿੜਕੀ_Repr;
    ਖਿੜਕੀ_Type.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    ਖਿੜਕੀ_Type.tp_doc = "vim Window object";
    ਖਿੜਕੀ_Type.tp_methods = ਖਿੜਕੀ_Methods;
    ਖਿੜਕੀ_Type.tp_traverse = ਖਿੜਕੀ_Traverse;
    ਖਿੜਕੀ_Type.tp_clear = ਖਿੜਕੀ_Clear;
#ਜੇ PY_MAJOR_ਸੰਸਕਰਣ >= 3
    ਖਿੜਕੀ_Type.tp_getattro = ਖਿੜਕੀ_Getattro;
    ਖਿੜਕੀ_Type.tp_setattro = ਖਿੜਕੀ_Setattro;
    ਖਿੜਕੀ_Type.tp_alloc = call_PyType_GenericAlloc;
    ਖਿੜਕੀ_Type.tp_new = call_PyType_GenericNew;
    ਖਿੜਕੀ_Type.tp_free = call_PyObject_Free;
#ਨਹੀਂਤਾਂ
    ਖਿੜਕੀ_Type.tp_getattr = ਖਿੜਕੀ_Getattr;
    ਖਿੜਕੀ_Type.tp_setattr = ਖਿੜਕੀ_Setattr;
#ਜੇ_ਅੰਤ

    CLEAR_ਖੇਤਰ(Tab_ਪੰਨਾ_Type);
    Tab_ਪੰਨਾ_Type.tp_name = "vim.tabpage";
    Tab_ਪੰਨਾ_Type.tp_basicsize = ਆਕਾਰ(Tab_ਪੰਨਾ_Object);
    Tab_ਪੰਨਾ_Type.tp_dealloc = Tab_ਪੰਨਾ_Destructor;
    Tab_ਪੰਨਾ_Type.tp_repr = Tab_ਪੰਨਾ_Repr;
    Tab_ਪੰਨਾ_Type.tp_flags = Py_TPFLAGS_DEFAULT;
    Tab_ਪੰਨਾ_Type.tp_doc = "vim tab page object";
    Tab_ਪੰਨਾ_Type.tp_methods = Tab_ਪੰਨਾ_Methods;
#ਜੇ PY_MAJOR_ਸੰਸਕਰਣ >= 3
    Tab_ਪੰਨਾ_Type.tp_getattro = Tab_ਪੰਨਾ_Getattro;
    Tab_ਪੰਨਾ_Type.tp_alloc = call_PyType_GenericAlloc;
    Tab_ਪੰਨਾ_Type.tp_new = call_PyType_GenericNew;
    Tab_ਪੰਨਾ_Type.tp_free = call_PyObject_Free;
#ਨਹੀਂਤਾਂ
    Tab_ਪੰਨਾ_Type.tp_getattr = Tab_ਪੰਨਾ_Getattr;
#ਜੇ_ਅੰਤ

    CLEAR_ਖੇਤਰ(BufMapType);
    BufMapType.tp_name = "vim.bufferlist";
    BufMapType.tp_basicsize = ਆਕਾਰ(BufMapObject);
    BufMapType.tp_as_mapping = &BufMapAsMapping;
    BufMapType.tp_flags = Py_TPFLAGS_DEFAULT;
    BufMapType.tp_iter = BufMapIter;
    BufMapType.tp_doc = "vim buffer list";

    CLEAR_ਖੇਤਰ(WinListType);
    WinListType.tp_name = "vim.windowlist";
    WinListType.tp_basicsize = ਆਕਾਰ(WinListObject);
    WinListType.tp_as_sequence = &WinListAsSeq;
    WinListType.tp_flags = Py_TPFLAGS_DEFAULT;
    WinListType.tp_doc = "vim window list";
    WinListType.tp_dealloc = (destructor)WinListDestructor;

    CLEAR_ਖੇਤਰ(TabListType);
    TabListType.tp_name = "vim.tabpagelist";
    TabListType.tp_basicsize = ਆਕਾਰ(TabListObject);
    TabListType.tp_as_sequence = &TabListAsSeq;
    TabListType.tp_flags = Py_TPFLAGS_DEFAULT;
    TabListType.tp_doc = "vim tab page list";

    CLEAR_ਖੇਤਰ(RangeType);
    RangeType.tp_name = "vim.range";
    RangeType.tp_basicsize = ਆਕਾਰ(RangeObject);
    RangeType.tp_dealloc = RangeDestructor;
    RangeType.tp_repr = RangeRepr;
    RangeType.tp_as_sequence = &RangeAsSeq;
    RangeType.tp_as_mapping = &RangeAsMapping;
    RangeType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    RangeType.tp_doc = "vim Range object";
    RangeType.tp_methods = RangeMethods;
    RangeType.tp_traverse = RangeTraverse;
    RangeType.tp_clear = RangeClear;
#ਜੇ PY_MAJOR_ਸੰਸਕਰਣ >= 3
    RangeType.tp_getattro = RangeGetattro;
    RangeType.tp_alloc = call_PyType_GenericAlloc;
    RangeType.tp_new = call_PyType_GenericNew;
    RangeType.tp_free = call_PyObject_Free;
#ਨਹੀਂਤਾਂ
    RangeType.tp_getattr = RangeGetattr;
#ਜੇ_ਅੰਤ

    CLEAR_ਖੇਤਰ(CurrentType);
    CurrentType.tp_name = "vim.currentdata";
    CurrentType.tp_basicsize = ਆਕਾਰ(CurrentObject);
    CurrentType.tp_flags = Py_TPFLAGS_DEFAULT;
    CurrentType.tp_doc = "vim current object";
    CurrentType.tp_methods = CurrentMethods;
#ਜੇ PY_MAJOR_ਸੰਸਕਰਣ >= 3
    CurrentType.tp_getattro = CurrentGetattro;
    CurrentType.tp_setattro = CurrentSetattro;
#ਨਹੀਂਤਾਂ
    CurrentType.tp_getattr = CurrentGetattr;
    CurrentType.tp_setattr = CurrentSetattr;
#ਜੇ_ਅੰਤ

    CLEAR_ਖੇਤਰ(DictionaryType);
    DictionaryType.tp_name = "vim.dictionary";
    DictionaryType.tp_basicsize = ਆਕਾਰ(DictionaryObject);
    DictionaryType.tp_dealloc = DictionaryDestructor;
    DictionaryType.tp_as_sequence = &DictionaryAsSeq;
    DictionaryType.tp_as_mapping = &DictionaryAsMapping;
    DictionaryType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE;
    DictionaryType.tp_doc = "dictionary pushing modifications to Vim structure";
    DictionaryType.tp_methods = DictionaryMethods;
    DictionaryType.tp_iter = DictionaryIter;
    DictionaryType.tp_new = DictionaryConstructor;
    DictionaryType.tp_alloc = PyType_GenericAlloc;
#ਜੇ PY_MAJOR_ਸੰਸਕਰਣ >= 3
    DictionaryType.tp_getattro = DictionaryGetattro;
    DictionaryType.tp_setattro = DictionarySetattro;
#ਨਹੀਂਤਾਂ
    DictionaryType.tp_getattr = DictionaryGetattr;
    DictionaryType.tp_setattr = DictionarySetattr;
#ਜੇ_ਅੰਤ

    CLEAR_ਖੇਤਰ(ListType);
    ListType.tp_name = "vim.list";
    ListType.tp_dealloc = ListDestructor;
    ListType.tp_basicsize = ਆਕਾਰ(ListObject);
    ListType.tp_as_sequence = &ListAsSeq;
    ListType.tp_as_mapping = &ListAsMapping;
    ListType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE;
    ListType.tp_doc = "list pushing modifications to Vim structure";
    ListType.tp_methods = ListMethods;
    ListType.tp_iter = ListIter;
    ListType.tp_new = ListConstructor;
    ListType.tp_alloc = PyType_GenericAlloc;
#ਜੇ PY_MAJOR_ਸੰਸਕਰਣ >= 3
    ListType.tp_getattro = ListGetattro;
    ListType.tp_setattro = ListSetattro;
#ਨਹੀਂਤਾਂ
    ListType.tp_getattr = ListGetattr;
    ListType.tp_setattr = ListSetattr;
#ਜੇ_ਅੰਤ

    // Tuple type
    CLEAR_ਖੇਤਰ(TupleType);
    TupleType.tp_name = "vim.tuple";
    TupleType.tp_dealloc = TupleDestructor;
    TupleType.tp_basicsize = ਆਕਾਰ(TupleObject);
    TupleType.tp_as_sequence = &TupleAsSeq;
    TupleType.tp_as_mapping = &TupleAsMapping;
    TupleType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE;
    TupleType.tp_doc = "tuple pushing modifications to Vim structure";
    TupleType.tp_methods = TupleMethods;
    TupleType.tp_iter = TupleIter;
    TupleType.tp_new = TupleConstructor;
    TupleType.tp_alloc = PyType_GenericAlloc;
#ਜੇ PY_MAJOR_ਸੰਸਕਰਣ >= 3
    TupleType.tp_getattro = TupleGetattro;
    TupleType.tp_setattro = TupleSetattro;
#ਨਹੀਂਤਾਂ
    TupleType.tp_getattr = TupleGetattr;
    TupleType.tp_setattr = TupleSetattr;
#ਜੇ_ਅੰਤ

    CLEAR_ਖੇਤਰ(FunctionType);
    FunctionType.tp_name = "vim.function";
    FunctionType.tp_basicsize = ਆਕਾਰ(FunctionObject);
    FunctionType.tp_dealloc = FunctionDestructor;
    FunctionType.tp_call = FunctionCall;
    FunctionType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE;
    FunctionType.tp_doc = "object that calls Vim function";
    FunctionType.tp_methods = FunctionMethods;
    FunctionType.tp_repr = FunctionRepr;
    FunctionType.tp_new = FunctionConstructor;
    FunctionType.tp_alloc = PyType_GenericAlloc;
#ਜੇ PY_MAJOR_ਸੰਸਕਰਣ >= 3
    FunctionType.tp_getattro = FunctionGetattro;
#ਨਹੀਂਤਾਂ
    FunctionType.tp_getattr = FunctionGetattr;
#ਜੇ_ਅੰਤ

    CLEAR_ਖੇਤਰ(OptionsType);
    OptionsType.tp_name = "vim.options";
    OptionsType.tp_basicsize = ਆਕਾਰ(OptionsObject);
    OptionsType.tp_as_sequence = &OptionsAsSeq;
    OptionsType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    OptionsType.tp_doc = "object for manipulating options";
    OptionsType.tp_iter = OptionsIter;
    OptionsType.tp_as_mapping = &OptionsAsMapping;
    OptionsType.tp_dealloc = OptionsDestructor;
    OptionsType.tp_traverse = OptionsTraverse;
    OptionsType.tp_clear = OptionsClear;

#ਜੇ PY_ਸੰਸਕਰਣ_HEX < 0x030700f0
    CLEAR_ਖੇਤਰ(LoaderType);
    LoaderType.tp_name = "vim.Loader";
    LoaderType.tp_basicsize = ਆਕਾਰ(LoaderObject);
    LoaderType.tp_flags = Py_TPFLAGS_DEFAULT;
    LoaderType.tp_doc = "vim message object";
    LoaderType.tp_methods = LoaderMethods;
    LoaderType.tp_dealloc = LoaderDestructor;
#ਜੇ_ਅੰਤ

#ਜੇ PY_MAJOR_ਸੰਸਕਰਣ >= 3
    CLEAR_ਖੇਤਰ(vimmodule);
    vimmodule.m_name = "vim";
    vimmodule.m_doc = "Vim Python interface\n";
    vimmodule.m_ਮਾਪ_2 = -1;
    vimmodule.m_methods = VimMethods;
#ਜੇ_ਅੰਤ
}

    ਸਥਿਰ ਪੂਰਨ
init_types(ਖਾਲੀ)
{
    PYTYPE_READY(IterType);
    PYTYPE_READY(ਬਫਰ_Type);
    PYTYPE_READY(RangeType);
    PYTYPE_READY(ਖਿੜਕੀ_Type);
    PYTYPE_READY(Tab_ਪੰਨਾ_Type);
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
#ਜੇ PY_ਸੰਸਕਰਣ_HEX < 0x030700f0
    PYTYPE_READY(LoaderType);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_LIMITED_API
    // We need to finish initializing all the static objects because the types
    // are only just allocated on the heap now.
    // Each PyObject_HEAD_INIT_TYPE should correspond to a
    // PyObject_FINISH_INIT_TYPE below.
    Py_Object_ਅੰਤ_INIT_TYPE(Output, OutputType);
    Py_Object_ਅੰਤ_INIT_TYPE(Error, OutputType);
    Py_Object_ਅੰਤ_INIT_TYPE(The_ਬਫਰ_Map, BufMapType);
    Py_Object_ਅੰਤ_INIT_TYPE(The_ਖਿੜਕੀ_List, WinListType);
    Py_Object_ਅੰਤ_INIT_TYPE(TheCurrent, CurrentType);
    Py_Object_ਅੰਤ_INIT_TYPE(The_Tab_ਪੰਨਾ_List, TabListType);
#ਜੇ_ਅੰਤ
    ਵਾਪਸ 0;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_LIMITED_API
    ਸਥਿਰ ਖਾਲੀ
shutdown_types(ਖਾਲੀ)
{
    PYTYPE_CLEANUP(IterType);
    PYTYPE_CLEANUP(ਬਫਰ_Type);
    PYTYPE_CLEANUP(RangeType);
    PYTYPE_CLEANUP(ਖਿੜਕੀ_Type);
    PYTYPE_CLEANUP(Tab_ਪੰਨਾ_Type);
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
# ਜੇ PY_ਸੰਸਕਰਣ_HEX < 0x030700f0
    PYTYPE_CLEANUP(LoaderType);
# endif
}
#ਜੇ_ਅੰਤ

    ਸਥਿਰ ਪੂਰਨ
init_sys_ਰਸਤਾ(ਖਾਲੀ)
{
    PyObject	*ਰਸਤਾ;
    PyObject	*ਰਸਤਾ_hook;
    PyObject	*ਰਸਤਾ_hooks;

    ਜੇ (!(ਰਸਤਾ_hook = PyObject_GetAttrString(vim_module, "path_hook")))
	ਵਾਪਸ -1;

    ਜੇ (!(ਰਸਤਾ_hooks = PySys_GetObject("path_hooks")))
    {
	PyErr_Clear();
	ਰਸਤਾ_hooks = PyList_New(1);
	Py_List_ਸੈੱਟ_ਵਸਤੂ(ਰਸਤਾ_hooks, 0, ਰਸਤਾ_hook);
	ਜੇ (Py_Sys_ਸੈੱਟ_Object("path_hooks", ਰਸਤਾ_hooks))
	{
	    Py_DECREF(ਰਸਤਾ_hooks);
	    ਵਾਪਸ -1;
	}
	Py_DECREF(ਰਸਤਾ_hooks);
    }
    ਨਹੀਂਤਾਂ ਜੇ (PyList_Check(ਰਸਤਾ_hooks))
    {
	ਜੇ (PyList_Append(ਰਸਤਾ_hooks, ਰਸਤਾ_hook))
	{
	    Py_DECREF(ਰਸਤਾ_hook);
	    ਵਾਪਸ -1;
	}
	Py_DECREF(ਰਸਤਾ_hook);
    }
    ਨਹੀਂਤਾਂ
    {
	Vim_Try_ਸ਼ੁਰੂ();
	emsg(_("Failed to set path hook: sys.path_hooks is not a list\n"
	       "You should now do the following:\n"
	       "- append vim.path_hook to sys.path_hooks\n"
	       "- append vim.VIM_SPECIAL_PATH to sys.path\n"));
	Vim_Try_ਸਿਰਾ(); // Discard the error
	Py_DECREF(ਰਸਤਾ_hook);
	ਵਾਪਸ 0;
    }

    ਜੇ (!(ਰਸਤਾ = PySys_GetObject("path")))
    {
	PyErr_Clear();
	ਰਸਤਾ = PyList_New(1);
	Py_INCREF(vim_special_ਰਸਤਾ_object);
	Py_List_ਸੈੱਟ_ਵਸਤੂ(ਰਸਤਾ, 0, vim_special_ਰਸਤਾ_object);
	ਜੇ (Py_Sys_ਸੈੱਟ_Object("path", ਰਸਤਾ))
	{
	    Py_DECREF(ਰਸਤਾ);
	    ਵਾਪਸ -1;
	}
	Py_DECREF(ਰਸਤਾ);
    }
    ਨਹੀਂਤਾਂ ਜੇ (PyList_Check(ਰਸਤਾ))
    {
	ਜੇ (PyList_Append(ਰਸਤਾ, vim_special_ਰਸਤਾ_object))
	    ਵਾਪਸ -1;
    }
    ਨਹੀਂਤਾਂ
    {
	Vim_Try_ਸ਼ੁਰੂ();
	emsg(_("Failed to set path: sys.path is not a list\n"
	       "You should now append vim.VIM_SPECIAL_PATH to sys.path"));
	Vim_Try_ਸਿਰਾ(); // Discard the error
    }

    ਵਾਪਸ 0;
}

ਸਥਿਰ ਬਣਤਰ numeric_constant {
    ਅੱਖਰ	*name;
    ਪੂਰਨ		val;
} numeric_constants[] = {
    {"VAR_LOCKED",	VAR_LOCKED},
    {"VAR_FIXED",	VAR_FIXED},
    {"VAR_SCOPE",	VAR_SCOPE},
    {"VAR_DEF_SCOPE",	VAR_DEF_SCOPE},
};

ਬਣਤਰ object_constant {
    ਅੱਖਰ	*name;
    PyObject	*valObject;
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਜੋੜੋ_OBJECT(m, name, obj) \
    ਜੇ (Py_Module_ਜੋੜੋ_Object(m, name, obj)) \
	ਵਾਪਸ -1;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਜੋੜੋ_CHECKED_OBJECT(m, name, obj) \
    { \
	PyObject	*valObject = obj; \
	ਜੇ (!valObject) \
	    ਵਾਪਸ -1; \
	ਜੋੜੋ_OBJECT(m, name, valObject); \
    }

    ਸਥਿਰ ਪੂਰਨ
populate_module(PyObject *m)
{
    ਪੂਰਨ		i;
    PyObject	*other_module;
    PyObject	*attr;
    PyObject	*imp;
#ਜੇ PY_ਸੰਸਕਰਣ_HEX >= 0x030700f0
    PyObject	*dict;
    PyObject	*cls;
#ਜੇ_ਅੰਤ

    ਲਈ (i = 0; i < (ਪੂਰਨ)(ਆਕਾਰ(numeric_constants)
					   / ਆਕਾਰ(ਬਣਤਰ numeric_constant));
	    ++i)
	ਜੋੜੋ_CHECKED_OBJECT(m, numeric_constants[i].name,
		PyInt_FromLong(numeric_constants[i].val));

    ਬਣਤਰ object_constant object_constants[] = {
	{"buffers",  (PyObject *)(ਖਾਲੀ *)&The_ਬਫਰ_Map},
	{"windows",  (PyObject *)(ਖਾਲੀ *)&The_ਖਿੜਕੀ_List},
	{"tabpages", (PyObject *)(ਖਾਲੀ *)&The_Tab_ਪੰਨਾ_List},
	{"current",  (PyObject *)(ਖਾਲੀ *)&TheCurrent},

	{"Buffer",     (PyObject *)ਬਫਰ_Type_Ptr},
	{"Range",      (PyObject *)RangeTypePtr},
	{"Window",     (PyObject *)ਖਿੜਕੀ_Type_Ptr},
	{"TabPage",    (PyObject *)Tab_ਪੰਨਾ_Type_Ptr},
	{"Dictionary", (PyObject *)DictionaryTypePtr},
	{"List",       (PyObject *)ListTypePtr},
	{"Tuple",      (PyObject *)TupleTypePtr},
	{"Function",   (PyObject *)FunctionTypePtr},
	{"Options",    (PyObject *)OptionsTypePtr},
#ਜੇ PY_ਸੰਸਕਰਣ_HEX < 0x030700f0
	{"_Loader",    (PyObject *)LoaderTypePtr},
#ਜੇ_ਅੰਤ
    };

    ਲਈ (i = 0; i < (ਪੂਰਨ)(ਆਕਾਰ(object_constants)
					    / ਆਕਾਰ(ਬਣਤਰ object_constant));
	    ++i)
    {
	PyObject	*valObject;

	valObject = object_constants[i].valObject;
	Py_INCREF(valObject);
	ਜੋੜੋ_OBJECT(m, object_constants[i].name, valObject);
    }

    ਜੇ (!(VimError = PyErr_NewException("vim.error", ਨਿਰਾ, ਨਿਰਾ)))
	ਵਾਪਸ -1;
    ਜੋੜੋ_OBJECT(m, "error", VimError);

    ਜੋੜੋ_CHECKED_OBJECT(m, "vars",  NEW_DICTIONARY(get_globvar_dict()));
    ਜੋੜੋ_CHECKED_OBJECT(m, "vvars", NEW_DICTIONARY(get_vimvar_dict()));
    ਜੋੜੋ_CHECKED_OBJECT(m, "options",
	    OptionsNew(SREQ_GLOBAL, ਨਿਰਾ, dummy_check, ਨਿਰਾ));

    ਜੇ (!(other_module = PyImport_ImportModule("os")))
	ਵਾਪਸ -1;
    ਜੋੜੋ_OBJECT(m, "os", other_module);

#ਜੇ PY_MAJOR_ਸੰਸਕਰਣ >= 3
    ਜੇ (!(py_getcwd = PyObject_GetAttrString(other_module, "getcwd")))
	ਵਾਪਸ -1;
#ਨਹੀਂਤਾਂ
    ਜੇ (!(py_getcwd = PyObject_GetAttrString(other_module, "getcwdu")))
	ਵਾਪਸ -1;
#ਜੇ_ਅੰਤ
    ਜੋੜੋ_OBJECT(m, "_getcwd", py_getcwd)

    ਜੇ (!(py_chdir = PyObject_GetAttrString(other_module, "chdir")))
	ਵਾਪਸ -1;
    ਜੋੜੋ_OBJECT(m, "_chdir", py_chdir);
    ਜੇ (!(attr = PyObject_GetAttrString(m, "chdir")))
	ਵਾਪਸ -1;
    ਜੇ (Py_Object_ਸੈੱਟ_Attr_String(other_module, "chdir", attr))
    {
	Py_DECREF(attr);
	ਵਾਪਸ -1;
    }
    Py_DECREF(attr);

    ਜੇ ((py_fchdir = PyObject_GetAttrString(other_module, "fchdir")))
    {
	ਜੋੜੋ_OBJECT(m, "_fchdir", py_fchdir);
	ਜੇ (!(attr = PyObject_GetAttrString(m, "fchdir")))
	    ਵਾਪਸ -1;
	ਜੇ (Py_Object_ਸੈੱਟ_Attr_String(other_module, "fchdir", attr))
	{
	    Py_DECREF(attr);
	    ਵਾਪਸ -1;
	}
	Py_DECREF(attr);
    }
    ਨਹੀਂਤਾਂ
	PyErr_Clear();

    ਜੇ (!(vim_special_ਰਸਤਾ_object = PyString_FromString(vim_special_ਰਸਤਾ)))
	ਵਾਪਸ -1;

    ਜੋੜੋ_OBJECT(m, "VIM_SPECIAL_PATH", vim_special_ਰਸਤਾ_object);

#ਜੇ PY_ਸੰਸਕਰਣ_HEX >= 0x030700f0
    ਜੇ (!(imp = PyImport_ImportModule("importlib.machinery")))
	ਵਾਪਸ -1;

    dict = PyModule_GetDict(imp);

    ਜੇ (!(cls = Py_Dict_Get_ਵਸਤੂ_String(dict, "PathFinder")))
    {
	Py_DECREF(imp);
	ਵਾਪਸ -1;
    }

    ਜੇ (!(py_ਲੱਭੋ_spec = PyObject_GetAttrString(cls, "find_spec")))
    {
	Py_DECREF(imp);
	ਵਾਪਸ -1;
    }

# ਜੇ PY_ਸੰਸਕਰਣ_HEX < 0x30c00a7
    // find_module has been removed as of Python 3.12.0a7
    ਜੇ ((py_ਲੱਭੋ_module = PyObject_GetAttrString(cls, "find_module")))
	ਜੋੜੋ_OBJECT(m, "_find_module", py_ਲੱਭੋ_module);
# endif

    Py_DECREF(imp);

    ਜੋੜੋ_OBJECT(m, "_find_spec", py_ਲੱਭੋ_spec);
#ਨਹੀਂਤਾਂ
    ਜੇ (!(imp = PyImport_ImportModule("imp")))
	ਵਾਪਸ -1;

    ਜੇ (!(py_ਲੱਭੋ_module = PyObject_GetAttrString(imp, "find_module")))
    {
	Py_DECREF(imp);
	ਵਾਪਸ -1;
    }

    ਜੇ (!(py_ਲੱਦੋ_module = PyObject_GetAttrString(imp, "load_module")))
    {
	Py_DECREF(py_ਲੱਭੋ_module);
	Py_DECREF(imp);
	ਵਾਪਸ -1;
    }

    Py_DECREF(imp);

    ਜੋੜੋ_OBJECT(m, "_find_module", py_ਲੱਭੋ_module);
    ਜੋੜੋ_OBJECT(m, "_load_module", py_ਲੱਦੋ_module);
#ਜੇ_ਅੰਤ

    ਵਾਪਸ 0;
}
