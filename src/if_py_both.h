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

#ಆದರೆ PY_ಆವೃತ್ತಿ_HEX < 0x02050000
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ Py_ssize_t;  // Python 2.4 and earlier don't have this type.
#ಆದರೆ_ಅಂತ್ಯ

// Use values that are known to work, others may make Vim crash.
#ವ್ಯಾಖ್ಯಾನಿಸು ENC_OPT (enc_utf8 ? "utf-8" : enc_dbcs ? "euc-jp" : (ಅಕ್ಷರ *)p_enc)
#ವ್ಯಾಖ್ಯಾನಿಸು DOPY_FUNC "_vim_pydo"

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *vim_special_ಮಾರ್ಗ = "_vim_path_";

#ವ್ಯಾಖ್ಯಾನಿಸು Py_Err_ಗಣ_STRING(exc, str) Py_Err_ಗಣ_String(exc, _(str))
#ವ್ಯಾಖ್ಯಾನಿಸು Py_Err_ಗಣ_Vim(str) Py_Err_ಗಣ_String(VimError, str)
#ವ್ಯಾಖ್ಯಾನಿಸು Py_Err_ಗಣ_VIM(str) Py_Err_ಗಣ_STRING(VimError, str)
#ವ್ಯಾಖ್ಯಾನಿಸು PyErr_FORMAT(exc, str, arg) PyErr_Format(exc, _(str), arg)
#ವ್ಯಾಖ್ಯಾನಿಸು PyErr_FORMAT2(exc, str, arg1, arg2) PyErr_Format(exc, _(str), arg1,arg2)
#ವ್ಯಾಖ್ಯಾನಿಸು PyErr_VIM_FORMAT(str, arg) PyErr_FORMAT(VimError, str, arg)

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_LIMITED_API
// Limited Python API. Need to call only exposed functions and remap macros.
// PyTypeObject is an opaque struct.

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    lenfunc sq_ಉದ್ದಳತೆ;
    binaryfunc sq_concat;
    ssizeargfunc sq_repeat;
    ssizeargfunc sq_ಅಂಶ;
    ಶೂನ್ಯ *was_sq_ತುಂಡು;
    ssizeobjargproc sq_ass_ಅಂಶ;
    ಶೂನ್ಯ *was_sq_ass_ತುಂಡು;
    objobjproc sq_ಒಳಗೊಂಡಿದೆ;

    binaryfunc sq_inplace_concat;
    ssizeargfunc sq_inplace_repeat;
} PySequenceMethods;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    lenfunc mp_ಉದ್ದಳತೆ;
    binaryfunc mp_subscript;
    objobjargproc mp_ass_subscript;
} PyMappingMethods;

// This struct emulates the concrete _typeobject struct to allow the code to
// work the same way in both limited and full Python APIs.
ರಚನೆ typeobject_wrapper {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *tp_name;
    Py_ssize_t tp_basicsize;
    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ tp_flags;

    // When adding new slots below, also need to make sure we add ADD_TP_SLOT
    // call in AddHeapType for it.

    destructor tp_dealloc;
    reprfunc tp_repr;

    PySequenceMethods *tp_as_sequence;
    PyMappingMethods *tp_as_mapping;

    ternaryfunc tp_call;
    getattrofunc tp_getattro;
    setattrofunc tp_setattro;

    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *tp_doc;

    traverseproc tp_traverse;

    inquiry tp_clear;

    getiterfunc tp_iter;
    iternextfunc tp_iternext;

    ರಚನೆ Py_ವಿಧಾನ_Def *tp_methods;
    ರಚನೆ _typeobject *tp_ತಳ;
    allocfunc tp_alloc;
    newfunc tp_new;
    freefunc tp_free;
};

# define DEFINE_PY_TYPE_OBJECT(type) \
    ಸ್ಥಿರ ರಚನೆ typeobject_wrapper type; \
    ಸ್ಥಿರ PyTypeObject* type##Ptr = ಖಾಲಿ

// PyObject_HEAD_INIT_TYPE and PyObject_FINISH_INIT_TYPE need to come in pairs
// We first initialize with NULL because the type is not allocated until
// init_types() is called later. It's in FINISH_INIT_TYPE where we fill the
// type in with the newly allocated type.
# define Py_Object_ತಲೆ_INIT_TYPE(type) Py_Object_ತಲೆ_INIT(ಖಾಲಿ)
# define Py_Object_ಮುಕ್ತಾಯ_INIT_TYPE(obj, type) obj.ob_ತಳ.ob_type = type##Ptr

# define Py_TYPE_GET_TP_ALLOC(type) ((allocfunc)Py_Type_Get_ಸ್ಥಾನ(type, Py_tp_alloc))
# define Py_TYPE_GET_TP_METHODS(type) ((Py_ವಿಧಾನ_Def *)Py_Type_Get_ಸ್ಥಾನ(type, Py_tp_methods))

// PyObject_NEW is not part of stable ABI, but PyObject_Malloc/Init are.
ಸ್ಥಿರ PyObject* Vim_PyObject_New(PyTypeObject *type, size_t objsize)
{
    PyObject *obj = (PyObject *)PyObject_Malloc(objsize);
    ಆದರೆ (obj == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ Py_Err_No_ಸ್ಮರಣೆ();
    ಹಿಂತಿರುಗಿ PyObject_Init(obj, type);
}
# undef PyObject_NEW
# define PyObject_NEW(type, typeobj) ((type *)Vim_PyObject_New(typeobj, ಗಾತ್ರ(type)))

// This is a somewhat convoluted because limited API doesn't expose an easy way
// to get the tp_name field, and so we have to manually reconstruct it as
// "__module__.__name__" (with __module__ omitted for builtins to emulate
// Python behavior). Also, some of the more convenient functions like
// PyUnicode_AsUTF8AndSize and PyType_GetQualName() are not available until
// late Python 3 versions, and won't be available if you set Py_LIMITED_API too
// low.
# define PyErr_FORMAT_TYPE(ಸಂದೇಶ, obj) \
    ಮಾಡು { \
	PyObject* qualname = PyObject_GetAttrString((PyObject*)(obj)->ob_type, "__qualname__"); \
	ಆದರೆ (qualname == ಖಾಲಿ) \
	{ \
	    PyErr_FORMAT(PyExc_TypeError, ಸಂದೇಶ, "(NULL)"); \
	    ನಿಲ್ಲಿಸು; \
	} \
	PyObject* module = PyObject_GetAttrString((PyObject*)(obj)->ob_type, "__module__"); \
	PyObject* ತುಂಬಿದ; \
	ಆದರೆ (module == ಖಾಲಿ || PyUnicode_CompareWithASCIIString(module, "builtins") == 0 \
		|| PyUnicode_CompareWithASCIIString(module, "__main__") == 0) \
	{ \
	    ತುಂಬಿದ = qualname; \
	    Py_INCREF(ತುಂಬಿದ); \
	} \
	ಇಲ್ಲದಿದ್ದರೆ \
	    ತುಂಬಿದ = PyUnicode_FromFormat("%U.%U", module, qualname); \
	PyObject* ತುಂಬಿದ_ಬೈಟುಗಳು = PyUnicode_AsUTF8String(ತುಂಬಿದ); \
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ತುಂಬಿದ_str = Py_ಬೈಟುಗಳು_As_String(ತುಂಬಿದ_ಬೈಟುಗಳು); \
	ತುಂಬಿದ_str = ತುಂಬಿದ_str == ಖಾಲಿ ? "(NULL)" : ತುಂಬಿದ_str; \
	PyErr_FORMAT(PyExc_TypeError, ಸಂದೇಶ, ತುಂಬಿದ_str); \
	Py_DECREF(qualname); \
	Py_XDECREF(module); \
	Py_XDECREF(ತುಂಬಿದ); \
	Py_XDECREF(ತುಂಬಿದ_ಬೈಟುಗಳು); \
    } ಆಗ (0)

# define Py_List_GET_ಅಂಶ(list, i) Py_List_Get_ಅಂಶ(list, i)
# define Py_List_GET_ಗಾತ್ರ(o) Py_List_ಗಾತ್ರ(o)

// PyList_SET_ITEM and PyList_SetItem have slightly different behaviors. The
// former will leave the old item dangling, and the latter will decref on it.
// Since we only use this on new lists, this difference doesn't matter.
# define Py_List_ಗಣ_ಅಂಶ(list, i, ಅಂಶ) Py_List_ಗಣ_ಅಂಶ_2(list, i, ಅಂಶ)

# define Py_Tuple_GET_ಅಂಶ(o, pos) Py_Tuple_Get_ಅಂಶ(o, pos)
# define Py_Tuple_GET_ಗಾತ್ರ(o) Py_Tuple_ಗಾತ್ರ(o)

# ಆದರೆ Py_LIMITED_API < 0x03080000
// PyIter_check only became part of stable ABI in 3.8, and there is no easy way
// to check for it in the API. We simply return false as a compromise. This
// does mean we should avoid compiling with stable ABI < 3.8.
#  undef PyIter_Check
#  define PyIter_Check(obj) (FALSE)
# endif

ಸ್ಥಿರ PyTypeObject* ಕೂಡಿಸು_ರಾಶಿ_Type(ರಚನೆ typeobject_wrapper* type_object)
{
    PyType_Spec type_spec;
    type_spec.name = type_object->tp_name;
    type_spec.basicsize = type_object->tp_basicsize;
    type_spec.itemsize = 0;
    type_spec.flags = type_object->tp_flags;

    // We just need to statically allocate a large enough buffer that can hold
    // all slots. We need to leave a null-terminated slot at the end.
    Py_Type_ಸ್ಥಾನ slots[40] = { {0, ಖಾಲಿ} };
    size_t ಸ್ಥಾನ_i = 0;

# define ಕೂಡಿಸು_TP_ಸ್ಥಾನ(ಸ್ಥಾನ_name) \
    ಆದರೆ (ಸ್ಥಾನ_i >= 40) ಹಿಂತಿರುಗಿ ಖಾಲಿ; /* this should never happen */ \
    ಆದರೆ (type_object->ಸ್ಥಾನ_name != ಖಾಲಿ) \
    { \
	slots[ಸ್ಥಾನ_i].ಸ್ಥಾನ = Py_##slot_name; \
	slots[ಸ್ಥಾನ_i].pfunc = (ಶೂನ್ಯ*)type_object->ಸ್ಥಾನ_name; \
	++ಸ್ಥಾನ_i; \
    }
# define ಕೂಡಿಸು_TP_SUB_ಸ್ಥಾನ(sub_ಸ್ಥಾನ, ಸ್ಥಾನ_name) \
    ಆದರೆ (ಸ್ಥಾನ_i >= 40) ಹಿಂತಿರುಗಿ ಖಾಲಿ; /* this should never happen */ \
    ಆದರೆ (type_object->sub_ಸ್ಥಾನ != ಖಾಲಿ && type_object->sub_ಸ್ಥಾನ->ಸ್ಥಾನ_name != ಖಾಲಿ) \
    { \
	slots[ಸ್ಥಾನ_i].ಸ್ಥಾನ = Py_##slot_name; \
	slots[ಸ್ಥಾನ_i].pfunc = (ಶೂನ್ಯ*)type_object->sub_ಸ್ಥಾನ->ಸ್ಥಾನ_name; \
	++ಸ್ಥಾನ_i; \
    }

    ಕೂಡಿಸು_TP_ಸ್ಥಾನ(tp_dealloc)
    ಕೂಡಿಸು_TP_ಸ್ಥಾನ(tp_repr)
    ಕೂಡಿಸು_TP_ಸ್ಥಾನ(tp_call)
    ಕೂಡಿಸು_TP_ಸ್ಥಾನ(tp_getattro)
    ಕೂಡಿಸು_TP_ಸ್ಥಾನ(tp_setattro)
    ಕೂಡಿಸು_TP_ಸ್ಥಾನ(tp_doc)
    ಕೂಡಿಸು_TP_ಸ್ಥಾನ(tp_traverse)
    ಕೂಡಿಸು_TP_ಸ್ಥಾನ(tp_clear)
    ಕೂಡಿಸು_TP_ಸ್ಥಾನ(tp_iter)
    ಕೂಡಿಸು_TP_ಸ್ಥಾನ(tp_iternext)
    ಕೂಡಿಸು_TP_ಸ್ಥಾನ(tp_methods)
    ಕೂಡಿಸು_TP_ಸ್ಥಾನ(tp_ತಳ)
    ಕೂಡಿಸು_TP_ಸ್ಥಾನ(tp_alloc)
    ಕೂಡಿಸು_TP_ಸ್ಥಾನ(tp_new)
    ಕೂಡಿಸು_TP_ಸ್ಥಾನ(tp_free)

    ಕೂಡಿಸು_TP_SUB_ಸ್ಥಾನ(tp_as_sequence, sq_ಉದ್ದಳತೆ)
    ಕೂಡಿಸು_TP_SUB_ಸ್ಥಾನ(tp_as_sequence, sq_concat)
    ಕೂಡಿಸು_TP_SUB_ಸ್ಥಾನ(tp_as_sequence, sq_repeat)
    ಕೂಡಿಸು_TP_SUB_ಸ್ಥಾನ(tp_as_sequence, sq_ಅಂಶ)
    ಕೂಡಿಸು_TP_SUB_ಸ್ಥಾನ(tp_as_sequence, sq_ass_ಅಂಶ)
    ಕೂಡಿಸು_TP_SUB_ಸ್ಥಾನ(tp_as_sequence, sq_ಒಳಗೊಂಡಿದೆ)
    ಕೂಡಿಸು_TP_SUB_ಸ್ಥಾನ(tp_as_sequence, sq_inplace_concat)
    ಕೂಡಿಸು_TP_SUB_ಸ್ಥಾನ(tp_as_sequence, sq_inplace_repeat)

    ಕೂಡಿಸು_TP_SUB_ಸ್ಥಾನ(tp_as_mapping, mp_ಉದ್ದಳತೆ)
    ಕೂಡಿಸು_TP_SUB_ಸ್ಥಾನ(tp_as_mapping, mp_subscript)
    ಕೂಡಿಸು_TP_SUB_ಸ್ಥಾನ(tp_as_mapping, mp_ass_subscript)
# undef ಕೂಡಿಸು_TP_ಸ್ಥಾನ
# undef ಕೂಡಿಸು_TP_SUB_ಸ್ಥಾನ

    type_spec.slots = slots;

    PyObject* newtype = PyType_FromSpec(&type_spec);
    ಹಿಂತಿರುಗಿ (PyTypeObject*)newtype;
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
    type##Ptr = ಕೂಡಿಸು_ರಾಶಿ_Type(&(type)); \
    ಆದರೆ (type##Ptr == ಖಾಲಿ) \
	ಹಿಂತಿರುಗಿ -1;
# define PYTYPE_CLEANUP(type) \
    type##Ptr = ಖಾಲಿ;

// Limited API does not provide PyRun_* functions. Need to implement manually
// using PyCompile and PyEval.
ಸ್ಥಿರ PyObject* Vim_Py_ಸತತ_String(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str, ಪೂರ್ಣಾಂಕ ಆರಂಭ, PyObject *globals, PyObject *locals)
{
    // Just pass "" for filename for now.
    PyObject* compiled = Py_CompileString(str, "", ಆರಂಭ);
    ಆದರೆ (compiled == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    PyObject* eval_ಫಲಿತಾಂಶ = Py_Eval_Eval_ಸಂಕೇತ(compiled, globals, locals);
    Py_DECREF(compiled);
    ಹಿಂತಿರುಗಿ eval_ಫಲಿತಾಂಶ;
}
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ Vim_Py_ಸತತ_Simple_String(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str)
{
    // This function emulates CPython's implementation.
    PyObject* m = Py_Import_ಕೂಡಿಸು_Module("__main__");
    ಆದರೆ (m == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ -1;
    PyObject* d = PyModule_GetDict(m);
    PyObject* output = Vim_Py_ಸತತ_String(str, Py_file_ಒಳಹರಿವು, d, d);
    ಆದರೆ (output == ಖಾಲಿ)
    {
	PyErr_PrintEx(TRUE);
	ಹಿಂತಿರುಗಿ -1;
    }
    Py_DECREF(output);
    ಹಿಂತಿರುಗಿ 0;
}
# define Py_ಸತತ_String Vim_Py_ಸತತ_String
# define Py_ಸತತ_Simple_String Vim_Py_ಸತತ_Simple_String

#ಇಲ್ಲದಿದ್ದರೆ // !defined(USE_LIMITED_API)

// Full Python API. Can make use of structs and macros directly.
# define DEFINE_PY_TYPE_OBJECT(type) \
    ಸ್ಥಿರ PyTypeObject type; \
    ಸ್ಥಿರ PyTypeObject* type##Ptr = &type
# define Py_Object_ತಲೆ_INIT_TYPE(type) Py_Object_ತಲೆ_INIT(&type)

# define Py_TYPE_GET_TP_ALLOC(type) type->tp_alloc
# define Py_TYPE_GET_TP_METHODS(type) type->tp_methods

# define Py_TYPE_NAME(obj) ((obj)->ob_type->tp_name == ಖಾಲಿ \
	? "(NULL)" \
	: (obj)->ob_type->tp_name)
# define PyErr_FORMAT_TYPE(ಸಂದೇಶ, obj) \
    PyErr_FORMAT(PyExc_TypeError, ಸಂದೇಶ, \
	    Py_TYPE_NAME(obj))

// Add a static type
# define PYTYPE_READY(type) \
    ಆದರೆ (PyType_Ready(type##Ptr)) \
	ಹಿಂತಿರುಗಿ -1;

#ಆದರೆ_ಅಂತ್ಯ


#ವ್ಯಾಖ್ಯಾನಿಸು RAISE_NO_ಖಾಲಿಗಳು_ಕೀಲಿಗಳು Py_Err_ಗಣ_STRING(Py_Exc_ಮೌಲ್ಯ_Error, \
					    N_("empty keys are not allowed"))
#ವ್ಯಾಖ್ಯಾನಿಸು RAISE_LOCKED_DICTIONARY Py_Err_ಗಣ_VIM(N_("dictionary is locked"))
#ವ್ಯಾಖ್ಯಾನಿಸು RAISE_LOCKED_LIST Py_Err_ಗಣ_VIM(N_("list is locked"))
#ವ್ಯಾಖ್ಯಾನಿಸು RAISE_UNDO_FAIL Py_Err_ಗಣ_VIM(N_("cannot save undo information"))
#ವ್ಯಾಖ್ಯಾನಿಸು RAISE_DELETE_ಸಾಲು_FAIL Py_Err_ಗಣ_VIM(N_("cannot delete line"))
#ವ್ಯಾಖ್ಯಾನಿಸು RAISE_ಸೇರಿಸು_ಸಾಲು_FAIL Py_Err_ಗಣ_VIM(N_("cannot insert line"))
#ವ್ಯಾಖ್ಯಾನಿಸು RAISE_REPLACE_ಸಾಲು_FAIL Py_Err_ಗಣ_VIM(N_("cannot replace line"))
#ವ್ಯಾಖ್ಯಾನಿಸು RAISE_ಕೀಲಿ_ಕೂಡಿಸು_FAIL(ಕೀಲಿ) \
    PyErr_VIM_FORMAT(N_("failed to add key '%s' to dictionary"), ಕೀಲಿ)
#ವ್ಯಾಖ್ಯಾನಿಸು RAISE_INVALID_ಸೂಚ್ಯಂಕ_TYPE(idx) \
    PyErr_FORMAT_TYPE(N_("index must be int or slice, not %s"), idx);

#ವ್ಯಾಖ್ಯಾನಿಸು INVALID_ಬಫರ್_ಮೌಲ್ಯ ((buf_T *)(-1))
#ವ್ಯಾಖ್ಯಾನಿಸು INVALID_ಕಿಟಕಿ_ಮೌಲ್ಯ ((win_T *)(-1))
#ವ್ಯಾಖ್ಯಾನಿಸು INVALID_TABPAGE_ಮೌಲ್ಯ ((tabpage_T *)(-1))

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ (*rangeinitializer)(ಶೂನ್ಯ *);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ (*runner)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *, dict_T *, ಶೂನ್ಯ *
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ PY_CAN_RECURSE
	, PyGILState_STATE *
#ಆದರೆ_ಅಂತ್ಯ
	);

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ConvertFromPyObject(PyObject *, typval_T *);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ _ConvertFromPyObject(PyObject *, typval_T *, PyObject *);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ConvertFromPyMapping(PyObject *, typval_T *);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ConvertFromPySequence(PyObject *, typval_T *);
ಸ್ಥಿರ PyObject *ಕಿಟಕಿ_New(win_T *, tabpage_T *);
ಸ್ಥಿರ PyObject *ಬಫರ್_New (buf_T *);
ಸ್ಥಿರ PyObject *ಸಾಲು_ಗಮ್ಯ_String(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *);

ಸ್ಥಿರ PyInt Range_ಆರಂಭ;
ಸ್ಥಿರ PyInt Range_ಕೊನೆ;

ಸ್ಥಿರ PyObject *globals;

ಸ್ಥಿರ PyObject *py_chdir;
ಸ್ಥಿರ PyObject *py_fchdir;
ಸ್ಥಿರ PyObject *py_getcwd;
ಸ್ಥಿರ PyObject *vim_module;
ಸ್ಥಿರ PyObject *vim_special_ಮಾರ್ಗ_object;

#ಆದರೆ PY_ಆವೃತ್ತಿ_HEX >= 0x030700f0
ಸ್ಥಿರ PyObject *py_ಕಂಡುಹಿಡಿ_spec;
#ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ PyObject *py_ತುಂಬು_module;
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ PY_ಆವೃತ್ತಿ_HEX < 0x30c00a7
ಸ್ಥಿರ PyObject *py_ಕಂಡುಹಿಡಿ_module;
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ PyObject *VimError;

/*
 * obtain a lock on the Vim data structures
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
Python_ಬೀಗ_Vim(ಶೂನ್ಯ)
{
}

/*
 * release a lock on the Vim data structures
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
Python_Release_Vim(ಶೂನ್ಯ)
{
}

/*
 * The "todecref" argument holds a pointer to PyObject * that must be
 * DECREF'ed after returned char_u * is no longer needed or NULL if all what
 * was needed to generate returned value is object.
 *
 * Use Py_XDECREF to decrement reference count.
 */
    ಸ್ಥಿರ char_u *
String_ಗಮ್ಯ_Chars(PyObject *obj, PyObject **todecref)
{
    char_u	*str;

    ಆದರೆ (Py_ಬೈಟುಗಳು_Check(obj))
    {

	ಆದರೆ (Py_ಬೈಟುಗಳು_As_String_And_ಗಾತ್ರ(obj, (ಅಕ್ಷರ **) &str, ಖಾಲಿ) == -1
		|| str == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	*todecref = ಖಾಲಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (PyUnicode_Check(obj))
    {
	PyObject	*ಬೈಟುಗಳು;

	ಆದರೆ (!(ಬೈಟುಗಳು = PyUnicode_AsEncodedString(obj, ENC_OPT,
							   ERRORS_ENCODE_ARG)))
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	ಆದರೆ (Py_ಬೈಟುಗಳು_As_String_And_ಗಾತ್ರ(ಬೈಟುಗಳು, (ಅಕ್ಷರ **) &str, ಖಾಲಿ) == -1
		|| str == ಖಾಲಿ)
	{
	    Py_DECREF(ಬೈಟುಗಳು);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

	*todecref = ಬೈಟುಗಳು;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
#ಆದರೆ PY_MAJOR_ಆವೃತ್ತಿ < 3
	PyErr_FORMAT_TYPE(N_("expected str() or unicode() instance, but got %s"),
		obj);
#ಇಲ್ಲದಿದ್ದರೆ
	PyErr_FORMAT_TYPE(N_("expected bytes() or str() instance, but got %s"),
		obj);
#ಆದರೆ_ಅಂತ್ಯ
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಹಿಂತಿರುಗಿ (char_u *) str;
}

#ವ್ಯಾಖ್ಯಾನಿಸು ಸಂಖ್ಯೆ_LONG     1
#ವ್ಯಾಖ್ಯಾನಿಸು ಸಂಖ್ಯೆ_INT      2
#ವ್ಯಾಖ್ಯಾನಿಸು ಸಂಖ್ಯೆ_NATURAL  4
#ವ್ಯಾಖ್ಯಾನಿಸು ಸಂಖ್ಯೆ_UNSIGNED 8

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಸಂಖ್ಯೆ_ಗಮ್ಯ_Long(PyObject *obj, ದೀರ್ಘ *ಫಲಿತಾಂಶ, ಪೂರ್ಣಾಂಕ flags)
{
#ಆದರೆ PY_MAJOR_ಆವೃತ್ತಿ < 3
    ಆದರೆ (PyInt_Check(obj))
    {
	*ಫಲಿತಾಂಶ = PyInt_AsLong(obj);
	ಆದರೆ (PyErr_Occurred())
	    ಹಿಂತಿರುಗಿ -1;
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (PyLong_Check(obj))
    {
	*ಫಲಿತಾಂಶ = PyLong_AsLong(obj);
	ಆದರೆ (PyErr_Occurred())
	    ಹಿಂತಿರುಗಿ -1;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (Py_ಸಂಖ್ಯೆ_Check(obj))
    {
	PyObject	*ಅಂಕೆ;

	ಆದರೆ (!(ಅಂಕೆ = Py_ಸಂಖ್ಯೆ_Long(obj)))
	    ಹಿಂತಿರುಗಿ -1;

	*ಫಲಿತಾಂಶ = PyLong_AsLong(ಅಂಕೆ);

	Py_DECREF(ಅಂಕೆ);

	ಆದರೆ (PyErr_Occurred())
	    ಹಿಂತಿರುಗಿ -1;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
#ಆದರೆ PY_MAJOR_ಆವೃತ್ತಿ < 3
	PyErr_FORMAT_TYPE(
		N_("expected int(), long() or something supporting "
		   "coercing to long(), but got %s"),
		obj);
#ಇಲ್ಲದಿದ್ದರೆ
	PyErr_FORMAT_TYPE(
		N_("expected int() or something supporting coercing to int(), "
		   "but got %s"),
		obj);
#ಆದರೆ_ಅಂತ್ಯ
	ಹಿಂತಿರುಗಿ -1;
    }

    ಆದರೆ (flags & ಸಂಖ್ಯೆ_INT)
    {
	ಆದರೆ (*ಫಲಿತಾಂಶ > ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ)
	{
	    Py_Err_ಗಣ_STRING(Py_Exc_ಅತಿಪ್ರವಾಹ_Error,
		    N_("value is too large to fit into C int type"));
	    ಹಿಂತಿರುಗಿ -1;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*ಫಲಿತಾಂಶ < INT_MIN)
	{
	    Py_Err_ಗಣ_STRING(Py_Exc_ಅತಿಪ್ರವಾಹ_Error,
		    N_("value is too small to fit into C int type"));
	    ಹಿಂತಿರುಗಿ -1;
	}
    }

    ಆದರೆ (flags & ಸಂಖ್ಯೆ_NATURAL)
    {
	ಆದರೆ (*ಫಲಿತಾಂಶ <= 0)
	{
	    Py_Err_ಗಣ_STRING(Py_Exc_ಮೌಲ್ಯ_Error,
		    N_("number must be greater than zero"));
	    ಹಿಂತಿರುಗಿ -1;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (flags & ಸಂಖ್ಯೆ_UNSIGNED)
    {
	ಆದರೆ (*ಫಲಿತಾಂಶ < 0)
	{
	    Py_Err_ಗಣ_STRING(Py_Exc_ಮೌಲ್ಯ_Error,
		    N_("number must be greater or equal to zero"));
	    ಹಿಂತಿರುಗಿ -1;
	}
    }

    ಹಿಂತಿರುಗಿ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಕೂಡಿಸು_string(PyObject *list, ಅಕ್ಷರ *s)
{
    PyObject	*string;

    ಆದರೆ (!(string = PyString_FromString(s)))
	ಹಿಂತಿರುಗಿ -1;

    ಆದರೆ (PyList_Append(list, string))
    {
	Py_DECREF(string);
	ಹಿಂತಿರುಗಿ -1;
    }

    Py_DECREF(string);
    ಹಿಂತಿರುಗಿ 0;
}

    ಸ್ಥಿರ PyObject *
ObjectDir(PyObject *self, ಅಕ್ಷರ **ಗುಣಲಕ್ಷಣಗಳು)
{
    Py_ವಿಧಾನ_Def	*ವಿಧಾನ;
    ಅಕ್ಷರ	**attr;
    PyObject	*ret;

    ಆದರೆ (!(ret = PyList_New(0)))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (self)
	ಗಾಗಿ (ವಿಧಾನ = Py_TYPE_GET_TP_METHODS(self->ob_type) ; ವಿಧಾನ->ml_name != ಖಾಲಿ ; ++ವಿಧಾನ)
	    ಆದರೆ (ಕೂಡಿಸು_string(ret, (ಅಕ್ಷರ *)ವಿಧಾನ->ml_name))
	    {
		Py_DECREF(ret);
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    }

    ಗಾಗಿ (attr = ಗುಣಲಕ್ಷಣಗಳು ; *attr ; ++attr)
	ಆದರೆ (ಕೂಡಿಸು_string(ret, *attr))
	{
	    Py_DECREF(ret);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

#ಆದರೆ PY_MAJOR_ಆವೃತ್ತಿ < 3
    ಆದರೆ (ಕೂಡಿಸು_string(ret, "__members__"))
    {
	Py_DECREF(ret);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ ret;
}

// Output buffer management

// Function to write a line, points to either msg() or emsg().
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*writefn)(ಅಕ್ಷರ *);

DEFINE_PY_TYPE_OBJECT(OutputType);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    Py_Object_ತಲೆ
    ದೀರ್ಘ softspace;
    ದೀರ್ಘ error;
} OutputObject;

ಸ್ಥಿರ ಅಕ್ಷರ *OutputAttrs[] = {
    "softspace",
    ಖಾಲಿ
};

    ಸ್ಥಿರ PyObject *
OutputDir(PyObject *self, PyObject *ವಾದಗಳು ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ ObjectDir(self, OutputAttrs);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
OutputSetattr(PyObject *self_obj, ಅಕ್ಷರ *name, PyObject *valObject)
{
    OutputObject *self = (OutputObject*)self_obj;
    ಆದರೆ (valObject == ಖಾಲಿ)
    {
	Py_Err_ಗಣ_STRING(PyExc_AttributeError,
		N_("can't delete OutputObject attributes"));
	ಹಿಂತಿರುಗಿ -1;
    }

    ಆದರೆ (strcmp(name, "softspace") == 0)
    {
	ಆದರೆ (ಸಂಖ್ಯೆ_ಗಮ್ಯ_Long(valObject, &(self->softspace), ಸಂಖ್ಯೆ_UNSIGNED))
	    ಹಿಂತಿರುಗಿ -1;
	ಹಿಂತಿರುಗಿ 0;
    }

    PyErr_FORMAT(PyExc_AttributeError, N_("invalid attribute: %s"), name);
    ಹಿಂತಿರುಗಿ -1;
}

// Buffer IO, we write one whole line at a time.
ಸ್ಥಿರ garray_T io_ga = {0, 0, 1, 80, ಖಾಲಿ};
ಸ್ಥಿರ writefn old_fn = ಖಾಲಿ;

    ಸ್ಥಿರ ಶೂನ್ಯ
PythonIO_Flush(ಶೂನ್ಯ)
{
    ಆದರೆ (old_fn != ಖಾಲಿ && io_ga.ga_len > 0)
    {
	((ಅಕ್ಷರ *)io_ga.ga_ದತ್ತಾಂಶ)[io_ga.ga_len] = NUL;
	// We don't know what emsg_severe should be here, so ... hope?
	old_fn((ಅಕ್ಷರ *)io_ga.ga_ದತ್ತಾಂಶ);
    }
    io_ga.ga_len = 0;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
writer(writefn fn, char_u *str, PyInt n, ಪೂರ್ಣಾಂಕ severe)
{
    char_u *ptr;

    // Flush when switching output function.
    ಆದರೆ (fn != old_fn)
	PythonIO_Flush();
    old_fn = fn;

    // Write each NL separated line.  Text after the last NL is kept for
    // writing later.
    // For normal messages: Do not output when "got_int" was set.  This avoids
    // a loop gone crazy flooding the terminal with messages.  Also for when
    // "q" is pressed at the more-prompt.
    ಆಗ (n > 0 && (ptr = memchr(str, '\n', n)) != ಖಾಲಿ
					  && (fn == (writefn)emsg || !got_int))
    {
	PyInt len = ptr - str;

	ಆದರೆ (ga_grow(&io_ga, (ಪೂರ್ಣಾಂಕ)(len + 1)) == FAIL)
	    ನಿಲ್ಲಿಸು;

	mch_memmove(((ಅಕ್ಷರ *)io_ga.ga_ದತ್ತಾಂಶ) + io_ga.ga_len, str, (size_t)len);
	((ಅಕ್ಷರ *)io_ga.ga_ದತ್ತಾಂಶ)[io_ga.ga_len + len] = NUL;
	emsg_severe = severe;
	fn((ಅಕ್ಷರ *)io_ga.ga_ದತ್ತಾಂಶ);
	str = ptr + 1;
	n -= len + 1;
	io_ga.ga_len = 0;
    }

    // Put the remaining text into io_ga for later printing.
    ಆದರೆ (n > 0 && (fn == (writefn)emsg || !got_int)
					&& ga_grow(&io_ga, (ಪೂರ್ಣಾಂಕ)(n + 1)) == OK)
    {
	mch_memmove(((ಅಕ್ಷರ *)io_ga.ga_ದತ್ತಾಂಶ) + io_ga.ga_len, str, (size_t)n);
	io_ga.ga_len += (ಪೂರ್ಣಾಂಕ)n;
    }
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
write_output(OutputObject *self, PyObject *string)
{
    Py_ssize_t	len = 0;
    ಅಕ್ಷರ	*str = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		error = self->error;

    ಆದರೆ (!PyArg_Parse(string, "et#", ENC_OPT, &str, &len))
	ಹಿಂತಿರುಗಿ -1;

    Py_BEGIN_ALLOW_ಎಳೆಗಳು
    Python_ಬೀಗ_Vim();
    writer((writefn)(error ? emsg : ಸಂದೇಶ), (char_u *)str, len, error);
    Python_Release_Vim();
    Py_ಕೊನೆ_ALLOW_ಎಳೆಗಳು
    PyMem_Free(str);

    ಹಿಂತಿರುಗಿ 0;
}

    ಸ್ಥಿರ PyObject *
OutputWrite(OutputObject *self, PyObject *string)
{
    ಆದರೆ (write_output(self, string))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    Py_INCREF(Py_None);
    ಹಿಂತಿರುಗಿ Py_None;
}

    ಸ್ಥಿರ PyObject *
OutputWritelines(OutputObject *self, PyObject *seq)
{
    PyObject	*iterator;
    PyObject	*ಅಂಶ;

    ಆದರೆ (!(iterator = PyObject_GetIter(seq)))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆಗ ((ಅಂಶ = Py_Iter_ಮುಂದಿನ(iterator)))
    {
	ಆದರೆ (write_output(self, ಅಂಶ))
	{
	    Py_DECREF(iterator);
	    Py_DECREF(ಅಂಶ);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
	Py_DECREF(ಅಂಶ);
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    ಆದರೆ (PyErr_Occurred())
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    Py_INCREF(Py_None);
    ಹಿಂತಿರುಗಿ Py_None;
}

    ಸ್ಥಿರ PyObject *
AlwaysNone(PyObject *self ಬಳಸದ, PyObject *ವಾದಗಳು ಬಳಸದ)
{
    // do nothing
    Py_INCREF(Py_None);
    ಹಿಂತಿರುಗಿ Py_None;
}
#ವ್ಯಾಖ್ಯಾನಿಸು ALWAYS_NONE AlwaysNone(ಖಾಲಿ, ಖಾಲಿ)

    ಸ್ಥಿರ PyObject *
AlwaysFalse(PyObject *self ಬಳಸದ, PyObject *ವಾದಗಳು ಬಳಸದ)
{
    // do nothing
    PyObject	*ret = Py_False;
    Py_INCREF(ret);
    ಹಿಂತಿರುಗಿ ret;
}
#ವ್ಯಾಖ್ಯಾನಿಸು ALWAYS_FALSE AlwaysFalse(ಖಾಲಿ, ಖಾಲಿ)

    ಸ್ಥಿರ PyObject *
AlwaysTrue(PyObject *self ಬಳಸದ, PyObject *ವಾದಗಳು ಬಳಸದ)
{
    // do nothing
    PyObject	*ret = Py_True;
    Py_INCREF(ret);
    ಹಿಂತಿರುಗಿ ret;
}
#ವ್ಯಾಖ್ಯಾನಿಸು ALWAYS_TRUE AlwaysTrue(ಖಾಲಿ, ಖಾಲಿ)

/***************/

ಸ್ಥಿರ ರಚನೆ Py_ವಿಧಾನ_Def OutputMethods[] = {
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
    { ಖಾಲಿ,	    ಖಾಲಿ,				0,		ಖಾಲಿ}
};

ಸ್ಥಿರ OutputObject Output =
{
    Py_Object_ತಲೆ_INIT_TYPE(OutputType)
    0,
    0
};

ಸ್ಥಿರ OutputObject Error =
{
    Py_Object_ತಲೆ_INIT_TYPE(OutputType)
    0,
    1
};

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
PythonIO_Init_io(ಶೂನ್ಯ)
{
    ಆದರೆ (Py_Sys_ಗಣ_Object("stdout", (PyObject *)(ಶೂನ್ಯ *)&Output))
	ಹಿಂತಿರುಗಿ -1;
    ಆದರೆ (Py_Sys_ಗಣ_Object("stderr", (PyObject *)(ಶೂನ್ಯ *)&Error))
	ಹಿಂತಿರುಗಿ -1;

    ಆದರೆ (PyErr_Occurred())
    {
	emsg(_(e_python_error_initialising_io_object));
	ಹಿಂತಿರುಗಿ -1;
    }

    ಹಿಂತಿರುಗಿ 0;
}

#ಆದರೆ PY_ಆವೃತ್ತಿ_HEX < 0x030700f0
ಸ್ಥಿರ PyObject *call_ತುಂಬು_module(ಅಕ್ಷರ *name, ಪೂರ್ಣಾಂಕ len, PyObject *ಕಂಡುಹಿಡಿ_module_ಫಲಿತಾಂಶ);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    Py_Object_ತಲೆ
    ಅಕ್ಷರ	*fullname;
    PyObject	*ಫಲಿತಾಂಶ;
} LoaderObject;
DEFINE_PY_TYPE_OBJECT(LoaderType);

    ಸ್ಥಿರ ಶೂನ್ಯ
LoaderDestructor(PyObject *self_obj)
{
    LoaderObject *self = (LoaderObject*)self_obj;
    vim_free(self->fullname);
    Py_XDECREF(self->ಫಲಿತಾಂಶ);
    DESTRUCTOR_ಮುಕ್ತಾಯ(self);
}

    ಸ್ಥಿರ PyObject *
Loader_ತುಂಬು_Module(LoaderObject *self, PyObject *ವಾದಗಳು ಬಳಸದ)
{
    ಅಕ್ಷರ	*fullname = self->fullname;
    PyObject	*ಫಲಿತಾಂಶ = self->ಫಲಿತಾಂಶ;
    PyObject	*module;

    ಆದರೆ (!fullname)
    {
	module = ಫಲಿತಾಂಶ ? ಫಲಿತಾಂಶ : Py_None;
	Py_INCREF(module);
	ಹಿಂತಿರುಗಿ module;
    }

    module = call_ತುಂಬು_module(fullname, (ಪೂರ್ಣಾಂಕ)STRLEN(fullname), ಫಲಿತಾಂಶ);

    self->fullname = ಖಾಲಿ;
    self->ಫಲಿತಾಂಶ = module;

    vim_free(fullname);
    Py_DECREF(ಫಲಿತಾಂಶ);

    ಆದರೆ (!module)
    {
	ಆದರೆ (PyErr_Occurred())
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	Py_INCREF(Py_None);
	ಹಿಂತಿರುಗಿ Py_None;
    }

    Py_INCREF(module);
    ಹಿಂತಿರುಗಿ module;
}

ಸ್ಥಿರ ರಚನೆ Py_ವಿಧಾನ_Def LoaderMethods[] = {
    // name,	    function,				calling,	doc
    {"load_module", (PyCFunction)Loader_ತುಂಬು_Module,	METH_VARARGS,	""},
    { ಖಾಲಿ,	    ಖಾಲಿ,				0,		ಖಾಲಿ}
};
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Check to see whether a Vim error has been reported, or a keyboard
 * interrupt has been detected.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
Vim_Try_ಆರಂಭ(ಶೂನ್ಯ)
{
    ++trylevel;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
Vim_Try_ಕೊನೆ(ಶೂನ್ಯ)
{
    --trylevel;
    // Without this it stops processing all subsequent Vim script commands and
    // generates strange error messages if I e.g. try calling Test() in a cycle
    did_emsg = FALSE;
    // Keyboard interrupt should be preferred over anything else
    ಆದರೆ (got_int)
    {
	ಆದರೆ (did_throw)
	    discard_current_exception();
	got_int = FALSE;
	Py_Err_ಗಣ_None(Py_Exc_Keyboard_ಅಡಚಣೆ);
	ಹಿಂತಿರುಗಿ -1;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಸಂದೇಶ_list != ಖಾಲಿ && *ಸಂದೇಶ_list != ಖಾಲಿ)
    {
	ಪೂರ್ಣಾಂಕ	should_free;
	ಅಕ್ಷರ	*ಸಂದೇಶ;

	ಸಂದೇಶ = get_exception_string(*ಸಂದೇಶ_list, ET_ERROR, ಖಾಲಿ, &should_free);

	ಆದರೆ (ಸಂದೇಶ == ಖಾಲಿ)
	{
	    Py_Err_No_ಸ್ಮರಣೆ();
	    ಹಿಂತಿರುಗಿ -1;
	}

	Py_Err_ಗಣ_Vim(ಸಂದೇಶ);

	free_global_msglist();

	ಆದರೆ (should_free)
	    vim_free(ಸಂದೇಶ);

	ಹಿಂತಿರುಗಿ -1;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!did_throw)
	ಹಿಂತಿರುಗಿ (PyErr_Occurred() ? -1 : 0);
    // Python exception is preferred over Vim one; unlikely to occur though
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (PyErr_Occurred())
    {
	discard_current_exception();
	ಹಿಂತಿರುಗಿ -1;
    }
    // Finally transform Vim script exception to python one
    ಇಲ್ಲದಿದ್ದರೆ
    {
	Py_Err_ಗಣ_Vim((ಅಕ್ಷರ *)current_exception->ಮೌಲ್ಯ);
	discard_current_exception();
	ಹಿಂತಿರುಗಿ -1;
    }
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
Vim_Check_ಅಡಚಣೆ(ಶೂನ್ಯ)
{
    ಆದರೆ (got_int)
    {
	Py_Err_ಗಣ_None(Py_Exc_Keyboard_ಅಡಚಣೆ);
	ಹಿಂತಿರುಗಿ 1;
    }
    ಹಿಂತಿರುಗಿ 0;
}

// Vim module - Implementation

    ಸ್ಥಿರ PyObject *
VimCommand(PyObject *self ಬಳಸದ, PyObject *string)
{
    char_u	*cmd;
    PyObject	*ret;
    PyObject	*todecref;

    ಆದರೆ (!(cmd = String_ಗಮ್ಯ_Chars(string, &todecref)))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    Py_BEGIN_ALLOW_ಎಳೆಗಳು
    Python_ಬೀಗ_Vim();

    Vim_Try_ಆರಂಭ();
    do_cmdline_cmd(cmd);
    update_screen(UPD_VALID);

    Python_Release_Vim();
    Py_ಕೊನೆ_ALLOW_ಎಳೆಗಳು

    ಆದರೆ (Vim_Try_ಕೊನೆ())
	ret = ಖಾಲಿ;
    ಇಲ್ಲದಿದ್ದರೆ
	ret = Py_None;

    Py_XINCREF(ret);
    Py_XDECREF(todecref);
    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Function to translate a typval_T into a PyObject; this will recursively
 * translate lists/dictionaries into their Python equivalents.
 *
 * The depth parameter is to avoid infinite recursion, set it to 1 when
 * you call VimToPython.
 */
    ಸ್ಥಿರ PyObject *
Vim_ಗಮ್ಯ_Python(typval_T *our_tv, ಪೂರ್ಣಾಂಕ ಆಳ, PyObject *lookup_dict)
{
    PyObject	*ret;
    PyObject	*newObj;
    ಅಕ್ಷರ	ptrBuf[ಗಾತ್ರ(ಶೂನ್ಯ *) * 2 + 3];

    // Avoid infinite recursion
    ಆದರೆ (ಆಳ > 100)
    {
	Py_INCREF(Py_None);
	ret = Py_None;
	ಹಿಂತಿರುಗಿ ret;
    }

    // Check if we run into a recursive loop.  The item must be in lookup_dict
    // then and we can use it again.
    ಆದರೆ ((our_tv->v_type == VAR_LIST && our_tv->vval.v_list != ಖಾಲಿ)
	    || (our_tv->v_type == VAR_TUPLE && our_tv->vval.v_tuple != ಖಾಲಿ)
	    || (our_tv->v_type == VAR_DICT && our_tv->vval.v_dict != ಖಾಲಿ))
    {
	sprintf(ptrBuf, "%p",
		our_tv->v_type == VAR_LIST ? (ಶೂನ್ಯ *)our_tv->vval.v_list
		    : our_tv->v_type == VAR_TUPLE ?
			(ಶೂನ್ಯ *)our_tv->vval.v_tuple
			: (ಶೂನ್ಯ *)our_tv->vval.v_dict);

	ಆದರೆ ((ret = Py_Dict_Get_ಅಂಶ_String(lookup_dict, ptrBuf)))
	{
	    Py_INCREF(ret);
	    ಹಿಂತಿರುಗಿ ret;
	}
    }

    ಆದರೆ (our_tv->v_type == VAR_STRING)
	ret = PyString_FromString(our_tv->vval.v_string == ಖಾಲಿ
					? "" : (ಅಕ್ಷರ *)our_tv->vval.v_string);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (our_tv->v_type == VAR_ಸಂಖ್ಯೆ)
    {
	ಅಕ್ಷರ buf[NUMBUFLEN];

	// For backwards compatibility numbers are stored as strings.
	sprintf(buf, "%lld", (ದೀರ್ಘ ದೀರ್ಘ)our_tv->vval.v_ಸಂಖ್ಯೆ);
	ret = PyString_FromString((ಅಕ್ಷರ *)buf);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (our_tv->v_type == VAR_FLOAT)
    {
	ಅಕ್ಷರ buf[NUMBUFLEN];

	sprintf(buf, "%f", our_tv->vval.v_float);
	ret = PyString_FromString((ಅಕ್ಷರ *)buf);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (our_tv->v_type == VAR_LIST)
    {
	list_T		*list = our_tv->vval.v_list;
	listitem_T	*curr;

	ಆದರೆ (list == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	ಆದರೆ (!(ret = PyList_New(0)))
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	ಆದರೆ (Py_Dict_ಗಣ_ಅಂಶ_String(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

	CHECK_LIST_MATERIALIZE(list);
	FOR_ALL_LIST_ವಸ್ತುಗಳು(list, curr)
	{
	    ಆದರೆ (!(newObj = Vim_ಗಮ್ಯ_Python(&curr->li_tv, ಆಳ + 1, lookup_dict)))
	    {
		Py_DECREF(ret);
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    }
	    ಆದರೆ (PyList_Append(ret, newObj))
	    {
		Py_DECREF(newObj);
		Py_DECREF(ret);
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    }
	    Py_DECREF(newObj);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (our_tv->v_type == VAR_TUPLE)
    {
	tuple_T	*tuple = our_tv->vval.v_tuple;
	ಪೂರ್ಣಾಂಕ	len;

	ಆದರೆ (tuple == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	len = TUPLE_LEN(tuple);

	ret = PyTuple_New(len);
	ಆದರೆ (ret == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ idx = 0; idx < len; idx++)
	{
	    typval_T	*ಅಂಶ_tv = TUPLE_ಅಂಶ(tuple, idx);

	    newObj = Vim_ಗಮ್ಯ_Python(ಅಂಶ_tv, ಆಳ + 1, lookup_dict);
	    ಆದರೆ (!newObj)
	    {
		Py_DECREF(ret);
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    }
	    Py_Tuple_ಗಣ_ಅಂಶ(ret, idx, newObj);
	}

	ಆದರೆ (Py_Dict_ಗಣ_ಅಂಶ_String(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (our_tv->v_type == VAR_DICT)
    {

	hashtab_T	*ht;
	long_u		todo;
	hashitem_T	*hi;
	dictitem_T	*di;

	ಆದರೆ (our_tv->vval.v_dict == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	ht = &our_tv->vval.v_dict->dv_hashtab;

	ಆದರೆ (!(ret = PyDict_New()))
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	ಆದರೆ (Py_Dict_ಗಣ_ಅಂಶ_String(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

	todo = ht->ht_ಬಳಸಿದ;
	ಗಾಗಿ (hi = ht->ht_array; todo > 0; ++hi)
	{
	    ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
	    {
		--todo;

		di = dict_lookup(hi);
		ಆದರೆ (!(newObj = Vim_ಗಮ್ಯ_Python(&di->di_tv, ಆಳ + 1, lookup_dict)))
		{
		    Py_DECREF(ret);
		    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		}
		ಆದರೆ (Py_Dict_ಗಣ_ಅಂಶ_String(ret, (ಅಕ್ಷರ *)hi->hi_ಕೀಲಿ, newObj))
		{
		    Py_DECREF(ret);
		    Py_DECREF(newObj);
		    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		}
	    }
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (our_tv->v_type == VAR_BOOL)
    {
	ಆದರೆ (our_tv->vval.v_ಸಂಖ್ಯೆ == VVAL_FALSE)
	{
	    ret = Py_False;
	    Py_INCREF(ret);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ret = Py_True;
	    Py_INCREF(ret);
	}
	ಹಿಂತಿರುಗಿ ret;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (our_tv->v_type == VAR_SPECIAL)
    {
	Py_INCREF(Py_None);
	ret = Py_None;
	ಹಿಂತಿರುಗಿ ret;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (our_tv->v_type == VAR_BLOB)
	ret = Py_ಬೈಟುಗಳು_From_String_And_ಗಾತ್ರ(
		(ಅಕ್ಷರ*) our_tv->vval.v_blob->bv_ga.ga_ದತ್ತಾಂಶ,
		(Py_ssize_t) our_tv->vval.v_blob->bv_ga.ga_len);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	Py_INCREF(Py_None);
	ret = Py_None;
    }

    ಹಿಂತಿರುಗಿ ret;
}

    ಸ್ಥಿರ PyObject *
VimEval(PyObject *self ಬಳಸದ, PyObject *ವಾದಗಳು)
{
    char_u	*ಅಭಿವ್ಯಕ್ತಿ;
    typval_T	*our_tv;
    PyObject	*string;
    PyObject	*todecref;
    PyObject	*ret;
    PyObject	*lookup_dict;

    ಆದರೆ (!PyArg_ParseTuple(ವಾದಗಳು, "O", &string))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (!(ಅಭಿವ್ಯಕ್ತಿ = String_ಗಮ್ಯ_Chars(string, &todecref)))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    Py_BEGIN_ALLOW_ಎಳೆಗಳು
    Python_ಬೀಗ_Vim();
    Vim_Try_ಆರಂಭ();
    our_tv = eval_ಅಭಿವ್ಯಕ್ತಿ(ಅಭಿವ್ಯಕ್ತಿ, ಖಾಲಿ);
    Python_Release_Vim();
    Py_ಕೊನೆ_ALLOW_ಎಳೆಗಳು

    Py_XDECREF(todecref);

    ಆದರೆ (Vim_Try_ಕೊನೆ())
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (our_tv == ಖಾಲಿ)
    {
	Py_Err_ಗಣ_VIM(N_("invalid expression"));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    // Convert the Vim type into a Python type.  Create a dictionary that's
    // used to check for recursive loops.
    ಆದರೆ (!(lookup_dict = PyDict_New()))
	ret = ಖಾಲಿ;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ret = Vim_ಗಮ್ಯ_Python(our_tv, 1, lookup_dict);
	Py_DECREF(lookup_dict);
    }


    Py_BEGIN_ALLOW_ಎಳೆಗಳು
    Python_ಬೀಗ_Vim();
    free_tv(our_tv);
    Python_Release_Vim();
    Py_ಕೊನೆ_ALLOW_ಎಳೆಗಳು

    ಹಿಂತಿರುಗಿ ret;
}

ಸ್ಥಿರ PyObject *Convert_ಗಮ್ಯ_Py_Object(typval_T *);

    ಸ್ಥಿರ PyObject *
VimEvalPy(PyObject *self ಬಳಸದ, PyObject *string)
{
    typval_T	*our_tv;
    PyObject	*ret;
    char_u	*ಅಭಿವ್ಯಕ್ತಿ;
    PyObject	*todecref;

    ಆದರೆ (!(ಅಭಿವ್ಯಕ್ತಿ = String_ಗಮ್ಯ_Chars(string, &todecref)))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    Py_BEGIN_ALLOW_ಎಳೆಗಳು
    Python_ಬೀಗ_Vim();
    Vim_Try_ಆರಂಭ();
    our_tv = eval_ಅಭಿವ್ಯಕ್ತಿ(ಅಭಿವ್ಯಕ್ತಿ, ಖಾಲಿ);
    Python_Release_Vim();
    Py_ಕೊನೆ_ALLOW_ಎಳೆಗಳು

    Py_XDECREF(todecref);

    ಆದರೆ (Vim_Try_ಕೊನೆ())
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (our_tv == ಖಾಲಿ)
    {
	Py_Err_ಗಣ_VIM(N_("invalid expression"));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ret = Convert_ಗಮ್ಯ_Py_Object(our_tv);
    Py_BEGIN_ALLOW_ಎಳೆಗಳು
    Python_ಬೀಗ_Vim();
    free_tv(our_tv);
    Python_Release_Vim();
    Py_ಕೊನೆ_ALLOW_ಎಳೆಗಳು

    ಹಿಂತಿರುಗಿ ret;
}

    ಸ್ಥಿರ PyObject *
VimStrwidth(PyObject *self ಬಳಸದ, PyObject *string)
{
    char_u	*str;
    PyObject	*todecref;
    ಪೂರ್ಣಾಂಕ		len;

    ಆದರೆ (!(str = String_ಗಮ್ಯ_Chars(string, &todecref)))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    len = mb_string2cells(str, (ಪೂರ್ಣಾಂಕ)STRLEN(str));

    Py_XDECREF(todecref);

    ಹಿಂತಿರುಗಿ PyLong_FromLong(len);
}

    ಸ್ಥಿರ PyObject *
_VimChdir(PyObject *_chdir, PyObject *ವಾದಗಳು, PyObject *kwargs)
{
    PyObject	*ret;
    PyObject	*newwd;
    PyObject	*todecref;
    char_u	*new_dir;

    ಆದರೆ (_chdir == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಆದರೆ (!(ret = PyObject_Call(_chdir, ವಾದಗಳು, kwargs)))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (!(newwd = Py_Object_Call_Function_Obj_ವಾದಗಳು(py_getcwd, ಖಾಲಿ)))
    {
	Py_DECREF(ret);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಆದರೆ (!(new_dir = String_ಗಮ್ಯ_Chars(newwd, &todecref)))
    {
	Py_DECREF(ret);
	Py_DECREF(newwd);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    Vim_Try_ಆರಂಭ();

    ಆದರೆ (vim_chdir(new_dir))
    {
	Py_DECREF(ret);
	Py_DECREF(newwd);
	Py_XDECREF(todecref);

	ಆದರೆ (Vim_Try_ಕೊನೆ())
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	Py_Err_ಗಣ_VIM(N_("failed to change directory"));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    Py_DECREF(newwd);
    Py_XDECREF(todecref);

    post_chdir(CDSCOPE_GLOBAL);

    ಆದರೆ (Vim_Try_ಕೊನೆ())
    {
	Py_DECREF(ret);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಹಿಂತಿರುಗಿ ret;
}

    ಸ್ಥಿರ PyObject *
VimChdir(PyObject *self ಬಳಸದ, PyObject *ವಾದಗಳು, PyObject *kwargs)
{
    ಹಿಂತಿರುಗಿ _VimChdir(py_chdir, ವಾದಗಳು, kwargs);
}

    ಸ್ಥಿರ PyObject *
VimFchdir(PyObject *self ಬಳಸದ, PyObject *ವಾದಗಳು, PyObject *kwargs)
{
    ಹಿಂತಿರುಗಿ _VimChdir(py_fchdir, ವಾದಗಳು, kwargs);
}

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    PyObject	*callable;
    PyObject	*ಫಲಿತಾಂಶ;
} map_rtp_ದತ್ತಾಂಶ;

    ಸ್ಥಿರ ಶೂನ್ಯ
map_rtp_callback(char_u *ಮಾರ್ಗ, ಶೂನ್ಯ *ದತ್ತಾಂಶ_3)
{
    ಶೂನ್ಯ	**ದತ್ತಾಂಶ = (ಶೂನ್ಯ **) ದತ್ತಾಂಶ_3;
    PyObject	*ಮಾರ್ಗ_Object;
    map_rtp_ದತ್ತಾಂಶ	*mr_ದತ್ತಾಂಶ = *((map_rtp_ದತ್ತಾಂಶ **) ದತ್ತಾಂಶ);

    ಆದರೆ (!(ಮಾರ್ಗ_Object = PyString_FromString((ಅಕ್ಷರ *)ಮಾರ್ಗ)))
    {
	*ದತ್ತಾಂಶ = ಖಾಲಿ;
	ಹಿಂತಿರುಗಿ;
    }

    mr_ದತ್ತಾಂಶ->ಫಲಿತಾಂಶ = Py_Object_Call_Function_Obj_ವಾದಗಳು(mr_ದತ್ತಾಂಶ->callable,
						   ಮಾರ್ಗ_Object, ಖಾಲಿ);

    Py_DECREF(ಮಾರ್ಗ_Object);

    ಆದರೆ (!mr_ದತ್ತಾಂಶ->ಫಲಿತಾಂಶ || mr_ದತ್ತಾಂಶ->ಫಲಿತಾಂಶ != Py_None)
	*ದತ್ತಾಂಶ = ಖಾಲಿ;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	Py_DECREF(mr_ದತ್ತಾಂಶ->ಫಲಿತಾಂಶ);
	mr_ದತ್ತಾಂಶ->ಫಲಿತಾಂಶ = ಖಾಲಿ;
    }
}

    ಸ್ಥಿರ PyObject *
VimForeachRTP(PyObject *self ಬಳಸದ, PyObject *callable)
{
    map_rtp_ದತ್ತಾಂಶ	ದತ್ತಾಂಶ;

    ದತ್ತಾಂಶ.callable = callable;
    ದತ್ತಾಂಶ.ಫಲಿತಾಂಶ = ಖಾಲಿ;

    do_ಒಳ_runtimepath(ಖಾಲಿ, 0, &map_rtp_callback, &ದತ್ತಾಂಶ);

    ಆದರೆ (ದತ್ತಾಂಶ.ಫಲಿತಾಂಶ == ಖಾಲಿ)
    {
	ಆದರೆ (PyErr_Occurred())
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    Py_INCREF(Py_None);
	    ಹಿಂತಿರುಗಿ Py_None;
	}
    }
    ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ.ಫಲಿತಾಂಶ;
}

/*
 * _vim_runtimepath_ special path implementation.
 */

    ಸ್ಥಿರ ಶೂನ್ಯ
map_finder_callback(char_u *ಮಾರ್ಗ, ಶೂನ್ಯ *ದತ್ತಾಂಶ_3)
{
    ಶೂನ್ಯ	**ದತ್ತಾಂಶ = (ಶೂನ್ಯ **) ದತ್ತಾಂಶ_3;
    PyObject	*list = *((PyObject **) ದತ್ತಾಂಶ);
    PyObject	*ಮಾರ್ಗ_Object1, *ಮಾರ್ಗ_Object2;
    ಅಕ್ಷರ	*pathbuf;
    size_t	pathlen;

    pathlen = STRLEN(ಮಾರ್ಗ);

#ಆದರೆ PY_MAJOR_ಆವೃತ್ತಿ < 3
# define PY_MAIN_DIR_STRING "python2"
#ಇಲ್ಲದಿದ್ದರೆ
# define PY_MAIN_DIR_STRING "python3"
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು PY_ALTERNATE_DIR_STRING "pythonx"

#ವ್ಯಾಖ್ಯಾನಿಸು PYTHONX_STRING_ಉದ್ದಳತೆ 7 // STRLEN("pythonx")
    ಆದರೆ (!(pathbuf = PyMem_New(ಅಕ್ಷರ,
		    pathlen + STRLEN(PATHSEPSTR) + PYTHONX_STRING_ಉದ್ದಳತೆ + 1)))
    {
	Py_Err_No_ಸ್ಮರಣೆ();
	*ದತ್ತಾಂಶ = ಖಾಲಿ;
	ಹಿಂತಿರುಗಿ;
    }

    mch_memmove(pathbuf, ಮಾರ್ಗ, pathlen + 1);
    ಕೂಡಿಸು_pathsep((char_u *) pathbuf);

    pathlen = STRLEN(pathbuf);
    mch_memmove(pathbuf + pathlen, PY_MAIN_DIR_STRING,
	    PYTHONX_STRING_ಉದ್ದಳತೆ + 1);

    ಆದರೆ (!(ಮಾರ್ಗ_Object1 = PyString_FromString(pathbuf)))
    {
	*ದತ್ತಾಂಶ = ಖಾಲಿ;
	PyMem_Free(pathbuf);
	ಹಿಂತಿರುಗಿ;
    }

    mch_memmove(pathbuf + pathlen, PY_ALTERNATE_DIR_STRING,
	    PYTHONX_STRING_ಉದ್ದಳತೆ + 1);

    ಆದರೆ (!(ಮಾರ್ಗ_Object2 = PyString_FromString(pathbuf)))
    {
	Py_DECREF(ಮಾರ್ಗ_Object1);
	PyMem_Free(pathbuf);
	*ದತ್ತಾಂಶ = ಖಾಲಿ;
	ಹಿಂತಿರುಗಿ;
    }

    PyMem_Free(pathbuf);

    ಆದರೆ (PyList_Append(list, ಮಾರ್ಗ_Object1)
	    || PyList_Append(list, ಮಾರ್ಗ_Object2))
	*ದತ್ತಾಂಶ = ಖಾಲಿ;

    Py_DECREF(ಮಾರ್ಗ_Object1);
    Py_DECREF(ಮಾರ್ಗ_Object2);
}

    ಸ್ಥಿರ PyObject *
Vim_GetPaths(PyObject *self ಬಳಸದ, PyObject *ವಾದಗಳು ಬಳಸದ)
{
    PyObject	*ret;

    ಆದರೆ (!(ret = PyList_New(0)))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    do_ಒಳ_runtimepath(ಖಾಲಿ, 0, &map_finder_callback, ret);

    ಆದರೆ (PyErr_Occurred())
    {
	Py_DECREF(ret);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಹಿಂತಿರುಗಿ ret;
}

#ಆದರೆ PY_ಆವೃತ್ತಿ_HEX >= 0x030700f0
    ಸ್ಥಿರ PyObject *
Finder_ಕಂಡುಹಿಡಿ_Spec(PyObject *self, PyObject *ವಾದಗಳು)
{
    ಅಕ್ಷರ	*fullname;
    PyObject	*paths;
    PyObject	*ಗುರಿ = Py_None;
    PyObject	*spec;

    ಆದರೆ (!PyArg_ParseTuple(ವಾದಗಳು, "s|O", &fullname, &ಗುರಿ))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (!(paths = Vim_GetPaths(self, ಖಾಲಿ)))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    spec = PyObject_CallFunction(py_ಕಂಡುಹಿಡಿ_spec, "sOO", fullname, paths, ಗುರಿ);

    Py_DECREF(paths);

    ಆದರೆ (!spec)
    {
	ಆದರೆ (PyErr_Occurred())
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	Py_INCREF(Py_None);
	ಹಿಂತಿರುಗಿ Py_None;
    }

    ಹಿಂತಿರುಗಿ spec;
}

    ಸ್ಥಿರ PyObject *
Finder_ಕಂಡುಹಿಡಿ_Module(PyObject* self ಬಳಸದ, PyObject* ವಾದಗಳು ಬಳಸದ)
{
    // Apparently returning None works.
    Py_INCREF(Py_None);
    ಹಿಂತಿರುಗಿ Py_None;
}
#ಇಲ್ಲದಿದ್ದರೆ
    ಸ್ಥಿರ PyObject *
call_ತುಂಬು_module(ಅಕ್ಷರ *name, ಪೂರ್ಣಾಂಕ len, PyObject *ಕಂಡುಹಿಡಿ_module_ಫಲಿತಾಂಶ)
{
    PyObject	*fd, *pathname, *description;

    ಆದರೆ (!PyTuple_Check(ಕಂಡುಹಿಡಿ_module_ಫಲಿತಾಂಶ))
    {
	PyErr_FORMAT_TYPE(
		N_("expected 3-tuple as imp.find_module() result, but got %s"),
		ಕಂಡುಹಿಡಿ_module_ಫಲಿತಾಂಶ);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಆದರೆ (Py_Tuple_GET_ಗಾತ್ರ(ಕಂಡುಹಿಡಿ_module_ಫಲಿತಾಂಶ) != 3)
    {
	PyErr_FORMAT(PyExc_TypeError,
		N_("expected 3-tuple as imp.find_module() result, but got "
		   "tuple of size %d"),
		(ಪೂರ್ಣಾಂಕ) Py_Tuple_GET_ಗಾತ್ರ(ಕಂಡುಹಿಡಿ_module_ಫಲಿತಾಂಶ));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಆದರೆ (!(fd = Py_Tuple_GET_ಅಂಶ(ಕಂಡುಹಿಡಿ_module_ಫಲಿತಾಂಶ, 0))
	    || !(pathname = Py_Tuple_GET_ಅಂಶ(ಕಂಡುಹಿಡಿ_module_ಫಲಿತಾಂಶ, 1))
	    || !(description = Py_Tuple_GET_ಅಂಶ(ಕಂಡುಹಿಡಿ_module_ಫಲಿತಾಂಶ, 2)))
    {
	Py_Err_ಗಣ_STRING(PyExc_RuntimeError,
		N_("internal error: imp.find_module returned tuple with NULL"));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಹಿಂತಿರುಗಿ PyObject_CallFunction(py_ತುಂಬು_module,
	    "s#OOO", name, len, fd, pathname, description);
}

    ಸ್ಥಿರ PyObject *
ಕಂಡುಹಿಡಿ_module(ಅಕ್ಷರ *fullname, ಅಕ್ಷರ *ಬಾಲ, PyObject *new_ಮಾರ್ಗ)
{
    PyObject	*ಕಂಡುಹಿಡಿ_module_ಫಲಿತಾಂಶ;
    PyObject	*module;
    ಅಕ್ಷರ	*dot;

    ಆದರೆ ((dot = (ಅಕ್ಷರ *)vim_strchr((char_u *) ಬಾಲ, '.')))
    {
	/*
	 * There is a dot in the name: call find_module recursively without the
	 * first component
	 */
	PyObject	*newest_ಮಾರ್ಗ;
	ಪೂರ್ಣಾಂಕ		partlen = (ಪೂರ್ಣಾಂಕ) (dot - 1 - ಬಾಲ);

	ಆದರೆ (!(ಕಂಡುಹಿಡಿ_module_ಫಲಿತಾಂಶ = PyObject_CallFunction(py_ಕಂಡುಹಿಡಿ_module,
			"s#O", ಬಾಲ, partlen, new_ಮಾರ್ಗ)))
	{
	    ಆದರೆ (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_ImportError))
		PyErr_Clear();
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

	ಆದರೆ (!(module = call_ತುಂಬು_module(
			fullname,
			((ಪೂರ್ಣಾಂಕ) (ಬಾಲ - fullname)) + partlen,
			ಕಂಡುಹಿಡಿ_module_ಫಲಿತಾಂಶ)))
	{
	    Py_DECREF(ಕಂಡುಹಿಡಿ_module_ಫಲಿತಾಂಶ);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

	Py_DECREF(ಕಂಡುಹಿಡಿ_module_ಫಲಿತಾಂಶ);

	ಆದರೆ (!(newest_ಮಾರ್ಗ = PyObject_GetAttrString(module, "__path__")))
	{
	    Py_DECREF(module);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

	Py_DECREF(module);

	ಕಂಡುಹಿಡಿ_module_ಫಲಿತಾಂಶ = ಕಂಡುಹಿಡಿ_module(fullname, dot + 1, newest_ಮಾರ್ಗ);

	Py_DECREF(newest_ಮಾರ್ಗ);

	ಹಿಂತಿರುಗಿ ಕಂಡುಹಿಡಿ_module_ಫಲಿತಾಂಶ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (!(ಕಂಡುಹಿಡಿ_module_ಫಲಿತಾಂಶ = PyObject_CallFunction(py_ಕಂಡುಹಿಡಿ_module,
			"sO", ಬಾಲ, new_ಮಾರ್ಗ)))
	{
	    ಆದರೆ (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_ImportError))
		PyErr_Clear();
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

	ಹಿಂತಿರುಗಿ ಕಂಡುಹಿಡಿ_module_ಫಲಿತಾಂಶ;
    }
}

    ಸ್ಥಿರ PyObject *
Finder_ಕಂಡುಹಿಡಿ_Module(PyObject *self, PyObject *ವಾದಗಳು)
{
    ಅಕ್ಷರ	*fullname;
    PyObject	*ಫಲಿತಾಂಶ;
    PyObject	*new_ಮಾರ್ಗ;
    LoaderObject	*loader;

    ಆದರೆ (!PyArg_ParseTuple(ವಾದಗಳು, "s", &fullname))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (!(new_ಮಾರ್ಗ = Vim_GetPaths(self, ಖಾಲಿ)))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಫಲಿತಾಂಶ = ಕಂಡುಹಿಡಿ_module(fullname, fullname, new_ಮಾರ್ಗ);

    Py_DECREF(new_ಮಾರ್ಗ);

    ಆದರೆ (!ಫಲಿತಾಂಶ)
    {
	ಆದರೆ (PyErr_Occurred())
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	Py_INCREF(Py_None);
	ಹಿಂತಿರುಗಿ Py_None;
    }

    ಆದರೆ (!(fullname = (ಅಕ್ಷರ *)vim_strsave((char_u *)fullname)))
    {
	Py_DECREF(ಫಲಿತಾಂಶ);
	Py_Err_No_ಸ್ಮರಣೆ();
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಆದರೆ (!(loader = PyObject_NEW(LoaderObject, LoaderTypePtr)))
    {
	vim_free(fullname);
	Py_DECREF(ಫಲಿತಾಂಶ);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    loader->fullname = fullname;
    loader->ಫಲಿತಾಂಶ = ಫಲಿತಾಂಶ;

    ಹಿಂತಿರುಗಿ (PyObject *) loader;
}
#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿರ PyObject *
Vim_ಮಾರ್ಗ_Hook(PyObject *self ಬಳಸದ, PyObject *ವಾದಗಳು)
{
    ಅಕ್ಷರ	*ಮಾರ್ಗ;

    ಆದರೆ (PyArg_ParseTuple(ವಾದಗಳು, "s", &ಮಾರ್ಗ)
	    && STRCMP(ಮಾರ್ಗ, vim_special_ಮಾರ್ಗ) == 0)
    {
	Py_INCREF(vim_module);
	ಹಿಂತಿರುಗಿ vim_module;
    }

    PyErr_Clear();
    Py_Err_ಗಣ_None(PyExc_ImportError);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Vim module - Definitions
 */

ಸ್ಥಿರ ರಚನೆ Py_ವಿಧಾನ_Def VimMethods[] = {
    // name,	    function,			calling,			documentation
    {"command",	    VimCommand,			METH_O,				"Execute a Vim ex-mode command" },
    {"eval",	    VimEval,			METH_VARARGS,			"Evaluate an expression using Vim evaluator" },
    {"bindeval",    VimEvalPy,			METH_O,				"Like eval(), but returns objects attached to Vim ones"},
    {"strwidth",    VimStrwidth,		METH_O,				"Screen string width, counts <Tab> as having width 1"},
    {"chdir",	    (PyCFunction)(ಶೂನ್ಯ *)VimChdir,	METH_VARARGS|METH_KEYWORDS,	"Change directory"},
    {"fchdir",	    (PyCFunction)(ಶೂನ್ಯ *)VimFchdir,	METH_VARARGS|METH_KEYWORDS,	"Change directory"},
    {"foreach_rtp", VimForeachRTP,		METH_O,				"Call given callable for each path in &rtp"},
#ಆದರೆ PY_ಆವೃತ್ತಿ_HEX >= 0x030700f0
    {"find_spec",   Finder_ಕಂಡುಹಿಡಿ_Spec,		METH_VARARGS,			"Internal use only, returns spec object for any input it receives"},
#ಆದರೆ_ಅಂತ್ಯ
    {"find_module", Finder_ಕಂಡುಹಿಡಿ_Module,		METH_VARARGS,			"Internal use only, returns loader object for any input it receives"},
    {"path_hook",   Vim_ಮಾರ್ಗ_Hook,		METH_VARARGS,			"Hook function to install in sys.path_hooks"},
    {"_get_paths",  (PyCFunction)Vim_GetPaths,	METH_NOARGS,			"Get &rtp-based additions to sys.path"},
    { ಖಾಲಿ,	    ಖಾಲಿ,			0,				ಖಾಲಿ}
};

/*
 * Generic iterator object
 */

DEFINE_PY_TYPE_OBJECT(IterType);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ PyObject *(*nextfun)(ಶೂನ್ಯ **);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ (*destructorfun)(ಶೂನ್ಯ *);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*traversefun)(ಶೂನ್ಯ *, visitproc, ಶೂನ್ಯ *);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*clearfun)(ಶೂನ್ಯ **);

// Main purpose of this object is removing the need for do python
// initialization (i.e. PyType_Ready and setting type attributes) for a big
// bunch of objects.
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    Py_Object_ತಲೆ
    ಶೂನ್ಯ *cur;
    nextfun ಮುಂದಿನ;
    destructorfun destruct;
    traversefun traverse;
    clearfun clear;
    PyObject *iter_object;
} IterObject;

    ಸ್ಥಿರ PyObject *
IterNew(ಶೂನ್ಯ *ಆರಂಭ, destructorfun destruct, nextfun ಮುಂದಿನ, traversefun traverse,
	clearfun clear, PyObject *iter_object)
{
    IterObject *self;

    self = PyObject_GC_New(IterObject, IterTypePtr);
    self->cur = ಆರಂಭ;
    self->ಮುಂದಿನ = ಮುಂದಿನ;
    self->destruct = destruct;
    self->traverse = traverse;
    self->clear = clear;
    self->iter_object = iter_object;

    ಆದರೆ (iter_object)
	Py_INCREF(iter_object);

    ಹಿಂತಿರುಗಿ (PyObject *)(self);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
IterDestructor(PyObject *self_obj)
{
    IterObject *self = (IterObject*)self_obj;
    ಆದರೆ (self->iter_object)
	Py_DECREF(self->iter_object);
    PyObject_GC_UnTrack((ಶೂನ್ಯ *)(self));
    self->destruct(self->cur);
    PyObject_GC_Del((ಶೂನ್ಯ *)(self));
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
IterTraverse(PyObject *self_obj, visitproc ಭೇಟಿ, ಶೂನ್ಯ *arg)
{
    IterObject *self = (IterObject*)self_obj;
    ಆದರೆ (self->traverse != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ self->traverse(self->cur, ಭೇಟಿ, arg);
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ 0;
}

// Mac OSX defines clear() somewhere.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ clear
# undef clear
#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
IterClear(PyObject *self_obj)
{
    IterObject *self = (IterObject*)self_obj;
    ಆದರೆ (self->clear != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ self->clear(&self->cur);
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ 0;
}

    ಸ್ಥಿರ PyObject *
Iter_ಮುಂದಿನ(PyObject *self_obj)
{
    IterObject *self = (IterObject*)self_obj;
    ಹಿಂತಿರುಗಿ self->ಮುಂದಿನ(&self->cur);
}

    ಸ್ಥಿರ PyObject *
IterIter(PyObject *self)
{
    Py_INCREF(self);
    ಹಿಂತಿರುಗಿ self;
}

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ pylinkedlist_S {
    ರಚನೆ pylinkedlist_S	*pll_ಮುಂದಿನ;
    ರಚನೆ pylinkedlist_S	*pll_prev;
    PyObject			*pll_obj;
} pylinkedlist_T;

ಸ್ಥಿರ pylinkedlist_T *lastdict = ಖಾಲಿ;
ಸ್ಥಿರ pylinkedlist_T *lastlist = ಖಾಲಿ;
ಸ್ಥಿರ pylinkedlist_T *lasttuple = ಖಾಲಿ;
ಸ್ಥಿರ pylinkedlist_T *lastfunc = ಖಾಲಿ;

    ಸ್ಥಿರ ಶೂನ್ಯ
pyll_remove(pylinkedlist_T *ref, pylinkedlist_T **ಕೊನೆಯ)
{
    ಆದರೆ (ref->pll_prev == ಖಾಲಿ)
    {
	ಆದರೆ (ref->pll_ಮುಂದಿನ == ಖಾಲಿ)
	{
	    *ಕೊನೆಯ = ಖಾಲಿ;
	    ಹಿಂತಿರುಗಿ;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ref->pll_prev->pll_ಮುಂದಿನ = ref->pll_ಮುಂದಿನ;

    ಆದರೆ (ref->pll_ಮುಂದಿನ == ಖಾಲಿ)
	*ಕೊನೆಯ = ref->pll_prev;
    ಇಲ್ಲದಿದ್ದರೆ
	ref->pll_ಮುಂದಿನ->pll_prev = ref->pll_prev;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
pyll_ಕೂಡಿಸು(PyObject *self, pylinkedlist_T *ref, pylinkedlist_T **ಕೊನೆಯ)
{
    ಆದರೆ (*ಕೊನೆಯ == ಖಾಲಿ)
	ref->pll_prev = ಖಾಲಿ;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	(*ಕೊನೆಯ)->pll_ಮುಂದಿನ = ref;
	ref->pll_prev = *ಕೊನೆಯ;
    }
    ref->pll_ಮುಂದಿನ = ಖಾಲಿ;
    ref->pll_obj = self;
    *ಕೊನೆಯ = ref;
}

DEFINE_PY_TYPE_OBJECT(DictionaryType);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    Py_Object_ತಲೆ
    dict_T	*dict;
    pylinkedlist_T	ref;
} DictionaryObject;

ಸ್ಥಿರ PyObject *DictionaryUpdate(DictionaryObject *, PyObject *, PyObject *);

#ವ್ಯಾಖ್ಯಾನಿಸು NEW_DICTIONARY(dict) DictionaryNew(DictionaryTypePtr, dict)

    ಸ್ಥಿರ PyObject *
DictionaryNew(PyTypeObject *subtype, dict_T *dict)
{
    DictionaryObject	*self;

    self = (DictionaryObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    ಆದರೆ (self == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    self->dict = dict;
    ++dict->dv_refcount;

    pyll_ಕೂಡಿಸು((PyObject *)(self), &self->ref, &lastdict);

    ಹಿಂತಿರುಗಿ (PyObject *)(self);
}

    ಸ್ಥಿರ dict_T *
py_dict_alloc(ಶೂನ್ಯ)
{
    dict_T	*ret;

    ಆದರೆ (!(ret = dict_alloc()))
    {
	Py_Err_No_ಸ್ಮರಣೆ();
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ++ret->dv_refcount;

    ಹಿಂತಿರುಗಿ ret;
}

    ಸ್ಥಿರ PyObject *
DictionaryConstructor(PyTypeObject *subtype, PyObject *ವಾದಗಳು, PyObject *kwargs)
{
    DictionaryObject	*self;
    dict_T	*dict;

    ಆದರೆ (!(dict = py_dict_alloc()))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    self = (DictionaryObject *) DictionaryNew(subtype, dict);

    --dict->dv_refcount;

    ಆದರೆ (kwargs || Py_Tuple_ಗಾತ್ರ(ವಾದಗಳು))
    {
	PyObject	*tmp;
	ಆದರೆ (!(tmp = DictionaryUpdate(self, ವಾದಗಳು, kwargs)))
	{
	    Py_DECREF(self);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

	Py_DECREF(tmp);
    }

    ಹಿಂತಿರುಗಿ (PyObject *)(self);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
DictionaryDestructor(PyObject *self_obj)
{
    DictionaryObject *self = (DictionaryObject*)self_obj;
    pyll_remove(&self->ref, &lastdict);
    dict_unref(self->dict);

    DESTRUCTOR_ಮುಕ್ತಾಯ(self);
}

ಸ್ಥಿರ ಅಕ್ಷರ *DictionaryAttrs[] = {
    "locked", "scope",
    ಖಾಲಿ
};

    ಸ್ಥಿರ PyObject *
DictionaryDir(PyObject *self, PyObject *ವಾದಗಳು ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ ObjectDir(self, DictionaryAttrs);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
DictionarySetattr(PyObject *self_obj, ಅಕ್ಷರ *name, PyObject *valObject)
{
    DictionaryObject *self = (DictionaryObject*)self_obj;
    ಆದರೆ (valObject == ಖಾಲಿ)
    {
	Py_Err_ಗಣ_STRING(PyExc_AttributeError,
		N_("cannot delete vim.Dictionary attributes"));
	ಹಿಂತಿರುಗಿ -1;
    }

    ಆದರೆ (strcmp(name, "locked") == 0)
    {
	ಆದರೆ (self->dict->dv_ಬೀಗ == VAR_FIXED)
	{
	    Py_Err_ಗಣ_STRING(PyExc_TypeError,
		    N_("cannot modify fixed dictionary"));
	    ಹಿಂತಿರುಗಿ -1;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಪೂರ್ಣಾಂಕ		istrue = PyObject_IsTrue(valObject);
	    ಆದರೆ (istrue == -1)
		ಹಿಂತಿರುಗಿ -1;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (istrue)
		self->dict->dv_ಬೀಗ = VAR_LOCKED;
	    ಇಲ್ಲದಿದ್ದರೆ
		self->dict->dv_ಬೀಗ = 0;
	}
	ಹಿಂತಿರುಗಿ 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	PyErr_FORMAT(PyExc_AttributeError, N_("cannot set attribute %s"), name);
	ಹಿಂತಿರುಗಿ -1;
    }
}

    ಸ್ಥಿರ PyInt
Dictionary_ಉದ್ದಳತೆ(DictionaryObject *self)
{
    ಹಿಂತಿರುಗಿ ((PyInt) (self->dict->dv_hashtab.ht_ಬಳಸಿದ));
}

#ವ್ಯಾಖ್ಯಾನಿಸು DICT_FLAG_HAS_DEFAULT	0x01
#ವ್ಯಾಖ್ಯಾನಿಸು DICT_FLAG_ತೆಗೆ		0x02
#ವ್ಯಾಖ್ಯಾನಿಸು DICT_FLAG_NONE_DEFAULT	0x04
#ವ್ಯಾಖ್ಯಾನಿಸು DICT_FLAG_RETURN_BOOL	0x08 // Incompatible with DICT_FLAG_POP
#ವ್ಯಾಖ್ಯಾನಿಸು DICT_FLAG_RETURN_PAIR	0x10

    ಸ್ಥಿರ PyObject *
Dictionary_ಅಂಶ(DictionaryObject *self, PyObject *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ flags)
{
    PyObject	*ಕೀಲಿ_Object;
    PyObject	*defObject = ((flags & DICT_FLAG_NONE_DEFAULT)? Py_None : ಖಾಲಿ);
    PyObject	*ret;
    char_u	*ಕೀಲಿ;
    dictitem_T	*di;
    dict_T	*dict = self->dict;
    hashitem_T	*hi;
    PyObject	*todecref;

    ಆದರೆ (flags & DICT_FLAG_HAS_DEFAULT)
    {
	ಆದರೆ (!PyArg_ParseTuple(ವಾದಗಳು, "O|O", &ಕೀಲಿ_Object, &defObject))
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಕೀಲಿ_Object = ವಾದಗಳು;

    ಆದರೆ (flags & DICT_FLAG_RETURN_BOOL)
	defObject = Py_False;

    ಆದರೆ (!(ಕೀಲಿ = String_ಗಮ್ಯ_Chars(ಕೀಲಿ_Object, &todecref)))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (*ಕೀಲಿ == NUL)
    {
	RAISE_NO_ಖಾಲಿಗಳು_ಕೀಲಿಗಳು;
	Py_XDECREF(todecref);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    hi = hash_ಕಂಡುಹಿಡಿ(&dict->dv_hashtab, ಕೀಲಿ);

    Py_XDECREF(todecref);

    ಆದರೆ (HASHITEM_ಖಾಲಿಗಳು(hi))
    {
	ಆದರೆ (defObject)
	{
	    Py_INCREF(defObject);
	    ಹಿಂತಿರುಗಿ defObject;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    Py_Err_ಗಣ_Object(Py_Exc_ಕೀಲಿ_Error, ಕೀಲಿ_Object);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (flags & DICT_FLAG_RETURN_BOOL)
    {
	ret = Py_True;
	Py_INCREF(ret);
	ಹಿಂತಿರುಗಿ ret;
    }

    di = dict_lookup(hi);

    ಆದರೆ (!(ret = Convert_ಗಮ್ಯ_Py_Object(&di->di_tv)))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (flags & DICT_FLAG_ತೆಗೆ)
    {
	ಆದರೆ (dict->dv_ಬೀಗ)
	{
	    RAISE_LOCKED_DICTIONARY;
	    Py_DECREF(ret);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

	hash_remove(&dict->dv_hashtab, hi, "Python remove variable");
	dictitem_free(di);
    }

    ಹಿಂತಿರುಗಿ ret;
}

    ಸ್ಥಿರ PyObject *
Dictionary_ಅಂಶ_2(DictionaryObject *self, PyObject *ಕೀಲಿ_Object)
{
    ಹಿಂತಿರುಗಿ Dictionary_ಅಂಶ(self, ಕೀಲಿ_Object, 0);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
Dictionary_ಒಳಗೊಂಡಿದೆ(DictionaryObject *self, PyObject *ಕೀಲಿ_Object)
{
    PyObject	*rObj = Dictionary_ಅಂಶ(self, ಕೀಲಿ_Object, DICT_FLAG_RETURN_BOOL);
    ಪೂರ್ಣಾಂಕ		ret;

    ಆದರೆ (rObj == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ -1;

    ret = (rObj == Py_True);

    Py_DECREF(rObj);

    ಹಿಂತಿರುಗಿ ret;
}

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ಪೂರ್ಣಾಂಕ		dii_ಬದಲಾಯಿತು;
    hashtab_T	*dii_ht;
    hashitem_T	*dii_hi;
    long_u	dii_todo;
} dictiterinfo_T;

    ಸ್ಥಿರ PyObject *
Dictionary_Iter_ಮುಂದಿನ(ಶೂನ್ಯ **arg)
{
    PyObject	*ret;
    dictiterinfo_T **dii = (dictiterinfo_T**)arg;

    ಆದರೆ (!(*dii)->dii_todo)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ ((*dii)->dii_ht->ht_ಬದಲಾಯಿತು != (*dii)->dii_ಬದಲಾಯಿತು)
    {
	Py_Err_ಗಣ_STRING(PyExc_RuntimeError,
		N_("hashtab changed during iteration"));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಆಗ (((*dii)->dii_todo) && HASHITEM_ಖಾಲಿಗಳು((*dii)->dii_hi))
	++((*dii)->dii_hi);

    --((*dii)->dii_todo);

    ಆದರೆ (!(ret = Py_ಬೈಟುಗಳು_From_String((ಅಕ್ಷರ *)(*dii)->dii_hi->hi_ಕೀಲಿ)))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಹಿಂತಿರುಗಿ ret;
}

    ಸ್ಥಿರ PyObject *
DictionaryIter(PyObject *self_obj)
{
    DictionaryObject	*self = (DictionaryObject*)self_obj;
    dictiterinfo_T	*dii;
    hashtab_T		*ht;

    ಆದರೆ (!(dii = PyMem_New(dictiterinfo_T, 1)))
    {
	Py_Err_No_ಸ್ಮರಣೆ();
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ht = &self->dict->dv_hashtab;
    dii->dii_ಬದಲಾಯಿತು = ht->ht_ಬದಲಾಯಿತು;
    dii->dii_ht = ht;
    dii->dii_hi = ht->ht_array;
    dii->dii_todo = ht->ht_ಬಳಸಿದ;

    ಹಿಂತಿರುಗಿ IterNew(dii,
	    PyMem_Free, Dictionary_Iter_ಮುಂದಿನ,
	    ಖಾಲಿ, ಖಾಲಿ, (PyObject *)self);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
Dictionary_Ass_ಅಂಶ(
	DictionaryObject *self, PyObject *ಕೀಲಿ_Object, PyObject *valObject)
{
    char_u	*ಕೀಲಿ;
    typval_T	tv;
    dict_T	*dict = self->dict;
    dictitem_T	*di;
    PyObject	*todecref;

    ಆದರೆ (dict->dv_ಬೀಗ)
    {
	RAISE_LOCKED_DICTIONARY;
	ಹಿಂತಿರುಗಿ -1;
    }

    ಆದರೆ (!(ಕೀಲಿ = String_ಗಮ್ಯ_Chars(ಕೀಲಿ_Object, &todecref)))
	ಹಿಂತಿರುಗಿ -1;

    ಆದರೆ (*ಕೀಲಿ == NUL)
    {
	RAISE_NO_ಖಾಲಿಗಳು_ಕೀಲಿಗಳು;
	Py_XDECREF(todecref);
	ಹಿಂತಿರುಗಿ -1;
    }

    di = dict_ಕಂಡುಹಿಡಿ(dict, ಕೀಲಿ, -1);

    ಆದರೆ (valObject == ಖಾಲಿ)
    {
	hashitem_T	*hi;

	ಆದರೆ (di == ಖಾಲಿ)
	{
	    Py_XDECREF(todecref);
	    Py_Err_ಗಣ_Object(Py_Exc_ಕೀಲಿ_Error, ಕೀಲಿ_Object);
	    ಹಿಂತಿರುಗಿ -1;
	}
	hi = hash_ಕಂಡುಹಿಡಿ(&dict->dv_hashtab, di->di_ಕೀಲಿ);
	hash_remove(&dict->dv_hashtab, hi, "Python remove item");
	dictitem_free(di);
	Py_XDECREF(todecref);
	ಹಿಂತಿರುಗಿ 0;
    }

    ಆದರೆ (ConvertFromPyObject(valObject, &tv) == -1)
    {
	Py_XDECREF(todecref);
	ಹಿಂತಿರುಗಿ -1;
    }

    ಆದರೆ (di == ಖಾಲಿ)
    {
	ಆದರೆ (!(di = dictitem_alloc(ಕೀಲಿ)))
	{
	    Py_XDECREF(todecref);
	    Py_Err_No_ಸ್ಮರಣೆ();
	    ಹಿಂತಿರುಗಿ -1;
	}
	di->di_tv.v_type = VAR_UNKNOWN;

	ಆದರೆ (dict_ಕೂಡಿಸು(dict, di) == FAIL)
	{
	    dictitem_free(di);
	    RAISE_ಕೀಲಿ_ಕೂಡಿಸು_FAIL(ಕೀಲಿ);
	    Py_XDECREF(todecref);
	    ಹಿಂತಿರುಗಿ -1;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	clear_tv(&di->di_tv);

    Py_XDECREF(todecref);

    copy_tv(&tv, &di->di_tv);
    clear_tv(&tv);
    ಹಿಂತಿರುಗಿ 0;
}

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ PyObject *(*hi_ಗಮ್ಯ_py)(hashitem_T *);

    ಸ್ಥಿರ PyObject *
DictionaryListObjects(DictionaryObject *self, hi_ಗಮ್ಯ_py hiconvert)
{
    dict_T	*dict = self->dict;
    long_u	todo = dict->dv_hashtab.ht_ಬಳಸಿದ;
    Py_ssize_t	i = 0;
    PyObject	*ret;
    hashitem_T	*hi;
    PyObject	*newObj;

    ret = PyList_New(todo);
    ಗಾಗಿ (hi = dict->dv_hashtab.ht_array; todo > 0; ++hi)
    {
	ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
	{
	    ಆದರೆ (!(newObj = hiconvert(hi)))
	    {
		Py_DECREF(ret);
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    }
	    Py_List_ಗಣ_ಅಂಶ(ret, i, newObj);
	    --todo;
	    ++i;
	}
    }
    ಹಿಂತಿರುಗಿ ret;
}

    ಸ್ಥಿರ PyObject *
dict_ಕೀಲಿ(hashitem_T *hi)
{
    ಹಿಂತಿರುಗಿ Py_ಬೈಟುಗಳು_From_String((ಅಕ್ಷರ *)(hi->hi_ಕೀಲಿ));
}

    ಸ್ಥಿರ PyObject *
Dictionary_List_ಕೀಲಿಗಳು(DictionaryObject *self, PyObject *ವಾದಗಳು ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ DictionaryListObjects(self, dict_ಕೀಲಿ);
}

    ಸ್ಥಿರ PyObject *
dict_val(hashitem_T *hi)
{
    dictitem_T	*di;

    di = dict_lookup(hi);
    ಹಿಂತಿರುಗಿ Convert_ಗಮ್ಯ_Py_Object(&di->di_tv);
}

    ಸ್ಥಿರ PyObject *
Dictionary_List_ಮೌಲ್ಯಗಳು(DictionaryObject *self, PyObject *ವಾದಗಳು ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ DictionaryListObjects(self, dict_val);
}

    ಸ್ಥಿರ PyObject *
dict_ಅಂಶ(hashitem_T *hi)
{
    PyObject	*ಕೀಲಿ_Object;
    PyObject	*valObject;
    PyObject	*ret;

    ಆದರೆ (!(ಕೀಲಿ_Object = dict_ಕೀಲಿ(hi)))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (!(valObject = dict_val(hi)))
    {
	Py_DECREF(ಕೀಲಿ_Object);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ret = Py_Build_ಮೌಲ್ಯ("(OO)", ಕೀಲಿ_Object, valObject);

    Py_DECREF(ಕೀಲಿ_Object);
    Py_DECREF(valObject);

    ಹಿಂತಿರುಗಿ ret;
}

    ಸ್ಥಿರ PyObject *
Dictionary_List_ವಸ್ತುಗಳು(DictionaryObject *self, PyObject *ವಾದಗಳು ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ DictionaryListObjects(self, dict_ಅಂಶ);
}

    ಸ್ಥಿರ PyObject *
DictionaryUpdate(DictionaryObject *self, PyObject *ವಾದಗಳು, PyObject *kwargs)
{
    dict_T	*dict = self->dict;

    ಆದರೆ (dict->dv_ಬೀಗ)
    {
	RAISE_LOCKED_DICTIONARY;
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಆದರೆ (kwargs)
    {
	typval_T	tv;

	ಆದರೆ (ConvertFromPyMapping(kwargs, &tv) == -1)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	Vim_Try_ಆರಂಭ();
	dict_extend(self->dict, tv.vval.v_dict, (char_u *) "force", ಖಾಲಿ);
	clear_tv(&tv);
	ಆದರೆ (Vim_Try_ಕೊನೆ())
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	PyObject	*obj = ಖಾಲಿ;

	ಆದರೆ (!PyArg_ParseTuple(ವಾದಗಳು, "|O", &obj))
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	ಆದರೆ (obj == ಖಾಲಿ)
	{
	    Py_INCREF(Py_None);
	    ಹಿಂತಿರುಗಿ Py_None;
	}

	ಆದರೆ (PyObject_HasAttrString(obj, "keys"))
	    ಹಿಂತಿರುಗಿ DictionaryUpdate(self, ಖಾಲಿ, obj);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    PyObject	*iterator;
	    PyObject	*ಅಂಶ;

	    ಆದರೆ (!(iterator = PyObject_GetIter(obj)))
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	    ಆಗ ((ಅಂಶ = Py_Iter_ಮುಂದಿನ(iterator)))
	    {
		PyObject	*fast;
		PyObject	*ಕೀಲಿ_Object;
		PyObject	*valObject;
		PyObject	*todecref;
		char_u		*ಕೀಲಿ;
		dictitem_T	*di;
		hashitem_T	*hi;

		ಆದರೆ (!(fast = PySequence_Fast(ಅಂಶ, "")))
		{
		    Py_DECREF(iterator);
		    Py_DECREF(ಅಂಶ);
		    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		}

		Py_DECREF(ಅಂಶ);

		ಆದರೆ (Py_Sequence_ಗಾತ್ರ(fast) != 2)
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    PyErr_FORMAT(Py_Exc_ಮೌಲ್ಯ_Error,
			    N_("expected sequence element of size 2, "
			    "but got sequence of size %d"),
			    (ಪೂರ್ಣಾಂಕ) Py_Sequence_ಗಾತ್ರ(fast));
		    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		}

		ಕೀಲಿ_Object = Py_Sequence_Get_ಅಂಶ(fast, 0);

		ಆದರೆ (!(ಕೀಲಿ = String_ಗಮ್ಯ_Chars(ಕೀಲಿ_Object, &todecref)))
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		}

		di = dictitem_alloc(ಕೀಲಿ);

		Py_XDECREF(todecref);

		ಆದರೆ (di == ಖಾಲಿ)
		{
		    Py_DECREF(fast);
		    Py_DECREF(iterator);
		    Py_Err_No_ಸ್ಮರಣೆ();
		    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		}
		di->di_tv.v_type = VAR_UNKNOWN;

		valObject = Py_Sequence_Get_ಅಂಶ(fast, 1);

		ಆದರೆ (ConvertFromPyObject(valObject, &di->di_tv) == -1)
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    dictitem_free(di);
		    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		}

		Py_DECREF(fast);

		hi = hash_ಕಂಡುಹಿಡಿ(&dict->dv_hashtab, di->di_ಕೀಲಿ);
		ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi) || dict_ಕೂಡಿಸು(dict, di) == FAIL)
		{
		    RAISE_ಕೀಲಿ_ಕೂಡಿಸು_FAIL(di->di_ಕೀಲಿ);
		    Py_DECREF(iterator);
		    dictitem_free(di);
		    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		}
	    }

	    Py_DECREF(iterator);

	    // Iterator may have finished due to an exception
	    ಆದರೆ (PyErr_Occurred())
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
    }
    Py_INCREF(Py_None);
    ಹಿಂತಿರುಗಿ Py_None;
}

    ಸ್ಥಿರ PyObject *
DictionaryGet(DictionaryObject *self, PyObject *ವಾದಗಳು)
{
    ಹಿಂತಿರುಗಿ Dictionary_ಅಂಶ(self, ವಾದಗಳು,
			    DICT_FLAG_HAS_DEFAULT|DICT_FLAG_NONE_DEFAULT);
}

    ಸ್ಥಿರ PyObject *
Dictionary_ತೆಗೆ(DictionaryObject *self, PyObject *ವಾದಗಳು)
{
    ಹಿಂತಿರುಗಿ Dictionary_ಅಂಶ(self, ವಾದಗಳು, DICT_FLAG_HAS_DEFAULT|DICT_FLAG_ತೆಗೆ);
}

    ಸ್ಥಿರ PyObject *
Dictionary_ತೆಗೆ_ಅಂಶ(DictionaryObject *self, PyObject *ವಾದಗಳು ಬಳಸದ)
{
    hashitem_T	*hi;
    PyObject	*ret;
    PyObject	*valObject;
    dictitem_T	*di;

    ಆದರೆ (self->dict->dv_hashtab.ht_ಬಳಸಿದ == 0)
    {
	Py_Err_ಗಣ_None(Py_Exc_ಕೀಲಿ_Error);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    hi = self->dict->dv_hashtab.ht_array;
    ಆಗ (HASHITEM_ಖಾಲಿಗಳು(hi))
	++hi;

    di = dict_lookup(hi);

    ಆದರೆ (!(valObject = Convert_ಗಮ್ಯ_Py_Object(&di->di_tv)))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (!(ret = Py_Build_ಮೌಲ್ಯ("(" Py_ಬೈಟುಗಳು_fmt "O)", hi->hi_ಕೀಲಿ, valObject)))
    {
	Py_DECREF(valObject);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    hash_remove(&self->dict->dv_hashtab, hi, "Python pop item");
    dictitem_free(di);

    ಹಿಂತಿರುಗಿ ret;
}

    ಸ್ಥಿರ PyObject *
Dictionary_Has_ಕೀಲಿ(DictionaryObject *self, PyObject *ಕೀಲಿ_Object)
{
    ಹಿಂತಿರುಗಿ Dictionary_ಅಂಶ(self, ಕೀಲಿ_Object, DICT_FLAG_RETURN_BOOL);
}

ಸ್ಥಿರ PySequenceMethods DictionaryAsSeq = {
    0,					// sq_length
    0,					// sq_concat
    0,					// sq_repeat
    0,					// sq_item
    0,					// sq_slice
    0,					// sq_ass_item
    0,					// sq_ass_slice
    (objobjproc) Dictionary_ಒಳಗೊಂಡಿದೆ,	// sq_contains
    0,					// sq_inplace_concat
    0,					// sq_inplace_repeat
};

ಸ್ಥಿರ PyMappingMethods DictionaryAsMapping = {
    (lenfunc)       Dictionary_ಉದ್ದಳತೆ,
    (binaryfunc)    Dictionary_ಅಂಶ_2,
    (objobjargproc) Dictionary_Ass_ಅಂಶ,
};

ಸ್ಥಿರ ರಚನೆ Py_ವಿಧಾನ_Def DictionaryMethods[] = {
    {"keys",	(PyCFunction)Dictionary_List_ಕೀಲಿಗಳು,	METH_NOARGS,	""},
    {"values",	(PyCFunction)Dictionary_List_ಮೌಲ್ಯಗಳು,	METH_NOARGS,	""},
    {"items",	(PyCFunction)Dictionary_List_ವಸ್ತುಗಳು,	METH_NOARGS,	""},
    {"update",	(PyCFunction)(ಶೂನ್ಯ *)DictionaryUpdate,		METH_VARARGS|METH_KEYWORDS, ""},
    {"get",	(PyCFunction)DictionaryGet,		METH_VARARGS,	""},
    {"pop",	(PyCFunction)Dictionary_ತೆಗೆ,		METH_VARARGS,	""},
    {"popitem",	(PyCFunction)Dictionary_ತೆಗೆ_ಅಂಶ,		METH_NOARGS,	""},
    {"has_key",	(PyCFunction)Dictionary_Has_ಕೀಲಿ,		METH_O,		""},
    {"__dir__",	(PyCFunction)DictionaryDir,		METH_NOARGS,	""},
    { ಖಾಲಿ,	ಖಾಲಿ,					0,		ಖಾಲಿ}
};

DEFINE_PY_TYPE_OBJECT(ListType);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    Py_Object_ತಲೆ
    list_T	*list;
    pylinkedlist_T	ref;
} ListObject;

#ವ್ಯಾಖ್ಯಾನಿಸು NEW_LIST(list) ListNew(ListTypePtr, list)

    ಸ್ಥಿರ PyObject *
ListNew(PyTypeObject *subtype, list_T *list)
{
    ListObject	*self;

    ಆದರೆ (list == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    self = (ListObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    ಆದರೆ (self == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    self->list = list;
    ++list->lv_refcount;
    CHECK_LIST_MATERIALIZE(list);

    pyll_ಕೂಡಿಸು((PyObject *)(self), &self->ref, &lastlist);

    ಹಿಂತಿರುಗಿ (PyObject *)(self);
}

    ಸ್ಥಿರ list_T *
py_list_alloc(ಶೂನ್ಯ)
{
    list_T	*ret;

    ಆದರೆ (!(ret = list_alloc()))
    {
	Py_Err_No_ಸ್ಮರಣೆ();
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ++ret->lv_refcount;

    ಹಿಂತಿರುಗಿ ret;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
list_py_concat(list_T *l, PyObject *obj, PyObject *lookup_dict)
{
    PyObject	*iterator;
    PyObject	*ಅಂಶ;
    listitem_T	*li;

    ಆದರೆ (!(iterator = PyObject_GetIter(obj)))
	ಹಿಂತಿರುಗಿ -1;

    ಆಗ ((ಅಂಶ = Py_Iter_ಮುಂದಿನ(iterator)))
    {
	ಆದರೆ (!(li = listitem_alloc()))
	{
	    Py_Err_No_ಸ್ಮರಣೆ();
	    Py_DECREF(ಅಂಶ);
	    Py_DECREF(iterator);
	    ಹಿಂತಿರುಗಿ -1;
	}
	li->li_tv.v_ಬೀಗ = 0;
	li->li_tv.v_type = VAR_UNKNOWN;

	ಆದರೆ (_ConvertFromPyObject(ಅಂಶ, &li->li_tv, lookup_dict) == -1)
	{
	    Py_DECREF(ಅಂಶ);
	    Py_DECREF(iterator);
	    listitem_free(l, li);
	    ಹಿಂತಿರುಗಿ -1;
	}

	Py_DECREF(ಅಂಶ);

	list_append(l, li);
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    ಆದರೆ (PyErr_Occurred())
	ಹಿಂತಿರುಗಿ -1;

    ಹಿಂತಿರುಗಿ 0;
}

    ಸ್ಥಿರ PyObject *
ListConstructor(PyTypeObject *subtype, PyObject *ವಾದಗಳು, PyObject *kwargs)
{
    list_T	*list;
    PyObject	*obj = ಖಾಲಿ;

    ಆದರೆ (kwargs)
    {
	Py_Err_ಗಣ_STRING(PyExc_TypeError,
		N_("list constructor does not accept keyword arguments"));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಆದರೆ (!PyArg_ParseTuple(ವಾದಗಳು, "|O", &obj))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (!(list = py_list_alloc()))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (obj)
    {
	PyObject	*lookup_dict;

	ಆದರೆ (!(lookup_dict = PyDict_New()))
	{
	    list_unref(list);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

	ಆದರೆ (list_py_concat(list, obj, lookup_dict) == -1)
	{
	    Py_DECREF(lookup_dict);
	    list_unref(list);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

	Py_DECREF(lookup_dict);
    }

    ಹಿಂತಿರುಗಿ ListNew(subtype, list);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ListDestructor(PyObject *self_obj)
{
    ListObject *self = (ListObject*)self_obj;
    pyll_remove(&self->ref, &lastlist);
    list_unref(self->list);

    DESTRUCTOR_ಮುಕ್ತಾಯ(self);
}

    ಸ್ಥಿರ PyInt
List_ಉದ್ದಳತೆ(ListObject *self)
{
    ಹಿಂತಿರುಗಿ ((PyInt) (self->list->lv_len));
}

    ಸ್ಥಿರ PyObject *
List_ಸೂಚ್ಯಂಕ(ListObject *self, Py_ssize_t ಸೂಚ್ಯಂಕ)
{
    listitem_T	*li;

    ಆದರೆ (ಸೂಚ್ಯಂಕ >= List_ಉದ್ದಳತೆ(self))
    {
	Py_Err_ಗಣ_STRING(Py_Exc_ಸೂಚ್ಯಂಕ_Error, N_("list index out of range"));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    li = list_ಕಂಡುಹಿಡಿ(self->list, (ದೀರ್ಘ) ಸೂಚ್ಯಂಕ);
    ಆದರೆ (li == ಖಾಲಿ)
    {
	// No more suitable format specifications in python-2.3
	PyErr_VIM_FORMAT(N_("internal error: failed to get Vim list item %d"),
		(ಪೂರ್ಣಾಂಕ) ಸೂಚ್ಯಂಕ);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಹಿಂತಿರುಗಿ Convert_ಗಮ್ಯ_Py_Object(&li->li_tv);
}

    ಸ್ಥಿರ PyObject *
List_ತುಂಡು(ListObject *self, Py_ssize_t ಮೊದಲ, Py_ssize_t ಹೆಜ್ಜೆ,
	  Py_ssize_t slicelen)
{
    PyInt	i;
    PyObject	*list;

    ಆದರೆ (ಹೆಜ್ಜೆ == 0)
    {
	Py_Err_ಗಣ_STRING(Py_Exc_ಮೌಲ್ಯ_Error, N_("slice step cannot be zero"));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    list = PyList_New(slicelen);
    ಆದರೆ (list == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಗಾಗಿ (i = 0; i < slicelen; ++i)
    {
	PyObject	*ಅಂಶ;

	ಅಂಶ = List_ಸೂಚ್ಯಂಕ(self, ಮೊದಲ + i*ಹೆಜ್ಜೆ);
	ಆದರೆ (ಅಂಶ == ಖಾಲಿ)
	{
	    Py_DECREF(list);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

	Py_List_ಗಣ_ಅಂಶ(list, i, ಅಂಶ);
    }

    ಹಿಂತಿರುಗಿ list;
}

    ಸ್ಥಿರ PyObject *
List_ಅಂಶ(ListObject *self, PyObject* idx)
{
#ಆದರೆ PY_MAJOR_ಆವೃತ್ತಿ < 3
    ಆದರೆ (PyInt_Check(idx))
    {
	ದೀರ್ಘ _idx = PyInt_AsLong(idx);
	ಹಿಂತಿರುಗಿ List_ಸೂಚ್ಯಂಕ(self, _idx);
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (PyLong_Check(idx))
    {
	ದೀರ್ಘ _idx = PyLong_AsLong(idx);
	ಹಿಂತಿರುಗಿ List_ಸೂಚ್ಯಂಕ(self, _idx);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (Py_ತುಂಡು_Check(idx))
    {
	Py_ssize_t ಆರಂಭ, stop, ಹೆಜ್ಜೆ, slicelen;

	ಆದರೆ (Py_ತುಂಡು_Get_Indices_Ex((Py_ತುಂಡು_Object_T *)idx, List_ಉದ್ದಳತೆ(self),
				 &ಆರಂಭ, &stop, &ಹೆಜ್ಜೆ, &slicelen) < 0)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	ಹಿಂತಿರುಗಿ List_ತುಂಡು(self, ಆರಂಭ, ಹೆಜ್ಜೆ, slicelen);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	RAISE_INVALID_ಸೂಚ್ಯಂಕ_TYPE(idx);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
list_restore(Py_ssize_t numadded, Py_ssize_t numreplaced, Py_ssize_t slicelen,
	list_T *l, listitem_T **lis, listitem_T *lastaddedli)
{
    ಆಗ (numreplaced--)
    {
	list_ಸೇರಿಸು(l, lis[numreplaced], lis[slicelen + numreplaced]);
	listitem_remove(l, lis[slicelen + numreplaced]);
    }
    ಆಗ (numadded--)
    {
	listitem_T	*ಮುಂದಿನ;

	ಮುಂದಿನ = lastaddedli->li_prev;
	listitem_remove(l, lastaddedli);
	lastaddedli = ಮುಂದಿನ;
    }
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
List_Ass_ತುಂಡು(ListObject *self, Py_ssize_t ಮೊದಲ,
	     Py_ssize_t ಹೆಜ್ಜೆ, Py_ssize_t slicelen, PyObject *obj)
{
    PyObject	*iterator;
    PyObject	*ಅಂಶ;
    listitem_T	*li;
    listitem_T	*lastaddedli = ಖಾಲಿ;
    listitem_T	*ಮುಂದಿನ;
    typval_T	v;
    list_T	*l = self->list;
    PyInt	i;
    PyInt	j;
    PyInt	numreplaced = 0;
    PyInt	numadded = 0;
    PyInt	ಗಾತ್ರ_2;
    listitem_T	**lis = ಖಾಲಿ;

    ಗಾತ್ರ_2 = List_ಉದ್ದಳತೆ(self);

    ಆದರೆ (l->lv_ಬೀಗ)
    {
	RAISE_LOCKED_LIST;
	ಹಿಂತಿರುಗಿ -1;
    }

    ಆದರೆ (ಹೆಜ್ಜೆ == 0)
    {
	Py_Err_ಗಣ_STRING(Py_Exc_ಮೌಲ್ಯ_Error, N_("slice step cannot be zero"));
	ಹಿಂತಿರುಗಿ -1;
    }

    ಆದರೆ (ಹೆಜ್ಜೆ != 1 && slicelen == 0)
    {
	// Nothing to do. Only error out if obj has some items.
	ಪೂರ್ಣಾಂಕ		ret = 0;

	ಆದರೆ (obj == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ 0;

	ಆದರೆ (!(iterator = PyObject_GetIter(obj)))
	    ಹಿಂತಿರುಗಿ -1;

	ಆದರೆ ((ಅಂಶ = Py_Iter_ಮುಂದಿನ(iterator)))
	{
	    PyErr_FORMAT(Py_Exc_ಮೌಲ್ಯ_Error,
		    N_("attempt to assign sequence of size greater than %d "
			"to extended slice"), 0);
	    Py_DECREF(ಅಂಶ);
	    ret = -1;
	}
	Py_DECREF(iterator);
	ಹಿಂತಿರುಗಿ ret;
    }

    ಆದರೆ (obj != ಖಾಲಿ)
	// XXX May allocate zero bytes.
	ಆದರೆ (!(lis = PyMem_New(listitem_T *, slicelen * 2)))
	{
	    Py_Err_No_ಸ್ಮರಣೆ();
	    ಹಿಂತಿರುಗಿ -1;
	}

    ಆದರೆ (ಮೊದಲ == ಗಾತ್ರ_2)
	li = ಖಾಲಿ;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	li = list_ಕಂಡುಹಿಡಿ(l, (ದೀರ್ಘ) ಮೊದಲ);
	ಆದರೆ (li == ಖಾಲಿ)
	{
	    PyErr_VIM_FORMAT(N_("internal error: no Vim list item %d"),
		    (ಪೂರ್ಣಾಂಕ)ಮೊದಲ);
	    ಆದರೆ (obj != ಖಾಲಿ)
		PyMem_Free(lis);
	    ಹಿಂತಿರುಗಿ -1;
	}
	i = slicelen;
	ಆಗ (i-- && li != ಖಾಲಿ)
	{
	    j = ಹೆಜ್ಜೆ;
	    ಮುಂದಿನ = li;
	    ಆದರೆ (ಹೆಜ್ಜೆ > 0)
		ಆಗ (ಮುಂದಿನ != ಖಾಲಿ && ((ಮುಂದಿನ = ಮುಂದಿನ->li_ಮುಂದಿನ) != ಖಾಲಿ) && --j);
	    ಇಲ್ಲದಿದ್ದರೆ
		ಆಗ (ಮುಂದಿನ != ಖಾಲಿ && ((ಮುಂದಿನ = ಮುಂದಿನ->li_prev) != ಖಾಲಿ) && ++j);

	    ಆದರೆ (obj == ಖಾಲಿ)
		listitem_remove(l, li);
	    ಇಲ್ಲದಿದ್ದರೆ
		lis[slicelen - i - 1] = li;

	    li = ಮುಂದಿನ;
	}
	ಆದರೆ (li == ಖಾಲಿ && i != -1)
	{
	    Py_Err_ಗಣ_VIM(N_("internal error: not enough list items"));
	    ಆದರೆ (obj != ಖಾಲಿ)
		PyMem_Free(lis);
	    ಹಿಂತಿರುಗಿ -1;
	}
    }

    ಆದರೆ (obj == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 0;

    ಆದರೆ (!(iterator = PyObject_GetIter(obj)))
    {
	PyMem_Free(lis);
	ಹಿಂತಿರುಗಿ -1;
    }

    i = 0;
    ಆಗ ((ಅಂಶ = Py_Iter_ಮುಂದಿನ(iterator)))
    {
	ಆದರೆ (ConvertFromPyObject(ಅಂಶ, &v) == -1)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(ಅಂಶ);
	    PyMem_Free(lis);
	    ಹಿಂತಿರುಗಿ -1;
	}
	Py_DECREF(ಅಂಶ);
	ಆದರೆ (list_ಸೇರಿಸು_tv(l, &v, numreplaced < slicelen
				    ? lis[numreplaced]
				    : li) == FAIL)
	{
	    clear_tv(&v);
	    Py_Err_ಗಣ_VIM(N_("internal error: failed to add item to list"));
	    list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	    PyMem_Free(lis);
	    ಹಿಂತಿರುಗಿ -1;
	}
	ಆದರೆ (numreplaced < slicelen)
	{
	    lis[slicelen + numreplaced] = lis[numreplaced]->li_prev;
	    vimlist_remove(l, lis[numreplaced], lis[numreplaced]);
	    numreplaced++;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (li)
		lastaddedli = li->li_prev;
	    ಇಲ್ಲದಿದ್ದರೆ
		lastaddedli = l->lv_u.mat.lv_ಕೊನೆಯ;
	    numadded++;
	}
	clear_tv(&v);
	ಆದರೆ (ಹೆಜ್ಜೆ != 1 && i >= slicelen)
	{
	    Py_DECREF(iterator);
	    PyErr_FORMAT(Py_Exc_ಮೌಲ್ಯ_Error,
		    N_("attempt to assign sequence of size greater than %d "
			"to extended slice"), (ಪೂರ್ಣಾಂಕ) slicelen);
	    list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	    PyMem_Free(lis);
	    ಹಿಂತಿರುಗಿ -1;
	}
	++i;
    }
    Py_DECREF(iterator);

    ಆದರೆ (ಹೆಜ್ಜೆ != 1 && i != slicelen)
    {
	PyErr_FORMAT2(Py_Exc_ಮೌಲ್ಯ_Error,
		N_("attempt to assign sequence of size %d to extended slice "
		    "of size %d"), (ಪೂರ್ಣಾಂಕ) i, (ಪೂರ್ಣಾಂಕ) slicelen);
	list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	PyMem_Free(lis);
	ಹಿಂತಿರುಗಿ -1;
    }

    ಆದರೆ (PyErr_Occurred())
    {
	list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	PyMem_Free(lis);
	ಹಿಂತಿರುಗಿ -1;
    }

    ಗಾಗಿ (i = 0; i < numreplaced; i++)
	listitem_free(l, lis[i]);
    ಆದರೆ (ಹೆಜ್ಜೆ == 1)
	ಗಾಗಿ (i = numreplaced; i < slicelen; i++)
	    listitem_remove(l, lis[i]);

    PyMem_Free(lis);

    ಹಿಂತಿರುಗಿ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
List_Ass_ಸೂಚ್ಯಂಕ(ListObject *self, Py_ssize_t ಸೂಚ್ಯಂಕ, PyObject *obj)
{
    typval_T	tv;
    list_T	*l = self->list;
    listitem_T	*li;
    Py_ssize_t	ಉದ್ದಳತೆ = List_ಉದ್ದಳತೆ(self);

    ಆದರೆ (l->lv_ಬೀಗ)
    {
	RAISE_LOCKED_LIST;
	ಹಿಂತಿರುಗಿ -1;
    }
    ಆದರೆ (ಸೂಚ್ಯಂಕ > ಉದ್ದಳತೆ || (ಸೂಚ್ಯಂಕ == ಉದ್ದಳತೆ && obj == ಖಾಲಿ))
    {
	Py_Err_ಗಣ_STRING(Py_Exc_ಸೂಚ್ಯಂಕ_Error, N_("list index out of range"));
	ಹಿಂತಿರುಗಿ -1;
    }

    ಆದರೆ (obj == ಖಾಲಿ)
    {
	li = list_ಕಂಡುಹಿಡಿ(l, (ದೀರ್ಘ) ಸೂಚ್ಯಂಕ);
	ಆದರೆ (li == ಖಾಲಿ)
	{
	    PyErr_VIM_FORMAT(N_("internal error: failed to get Vim "
			"list item %d"), (ಪೂರ್ಣಾಂಕ) ಸೂಚ್ಯಂಕ);
	    ಹಿಂತಿರುಗಿ -1;
	}
	vimlist_remove(l, li, li);
	clear_tv(&li->li_tv);
	vim_free(li);
	ಹಿಂತಿರುಗಿ 0;
    }

    ಆದರೆ (ConvertFromPyObject(obj, &tv) == -1)
	ಹಿಂತಿರುಗಿ -1;

    ಆದರೆ (ಸೂಚ್ಯಂಕ == ಉದ್ದಳತೆ)
    {
	ಆದರೆ (list_append_tv(l, &tv) == FAIL)
	{
	    clear_tv(&tv);
	    Py_Err_ಗಣ_VIM(N_("failed to add item to list"));
	    ಹಿಂತಿರುಗಿ -1;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	li = list_ಕಂಡುಹಿಡಿ(l, (ದೀರ್ಘ) ಸೂಚ್ಯಂಕ);
	ಆದರೆ (li == ಖಾಲಿ)
	{
	    PyErr_VIM_FORMAT(N_("internal error: failed to get Vim "
			"list item %d"), (ಪೂರ್ಣಾಂಕ) ಸೂಚ್ಯಂಕ);
	    ಹಿಂತಿರುಗಿ -1;
	}
	clear_tv(&li->li_tv);
	copy_tv(&tv, &li->li_tv);
	clear_tv(&tv);
    }
    ಹಿಂತಿರುಗಿ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
List_Ass_ಅಂಶ(ListObject *self, PyObject *idx, PyObject *obj)
{
#ಆದರೆ PY_MAJOR_ಆವೃತ್ತಿ < 3
    ಆದರೆ (PyInt_Check(idx))
    {
	ದೀರ್ಘ _idx = PyInt_AsLong(idx);
	ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)List_Ass_ಸೂಚ್ಯಂಕ(self, _idx, obj);
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (PyLong_Check(idx))
    {
	ದೀರ್ಘ _idx = PyLong_AsLong(idx);
	ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)List_Ass_ಸೂಚ್ಯಂಕ(self, _idx, obj);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (Py_ತುಂಡು_Check(idx))
    {
	Py_ssize_t ಆರಂಭ, stop, ಹೆಜ್ಜೆ, slicelen;

	ಆದರೆ (Py_ತುಂಡು_Get_Indices_Ex((Py_ತುಂಡು_Object_T *)idx, List_ಉದ್ದಳತೆ(self),
				 &ಆರಂಭ, &stop, &ಹೆಜ್ಜೆ, &slicelen) < 0)
	    ಹಿಂತಿರುಗಿ -1;
	ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)List_Ass_ತುಂಡು(self, ಆರಂಭ, ಹೆಜ್ಜೆ, slicelen,
		obj);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	RAISE_INVALID_ಸೂಚ್ಯಂಕ_TYPE(idx);
	ಹಿಂತಿರುಗಿ -1;
    }
}

    ಸ್ಥಿರ PyObject *
List_Concat_ಒಳ_ಇರಿಸು(ListObject *self, PyObject *obj)
{
    list_T	*l = self->list;
    PyObject	*lookup_dict;

    ಆದರೆ (l->lv_ಬೀಗ)
    {
	RAISE_LOCKED_LIST;
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಆದರೆ (!(lookup_dict = PyDict_New()))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (list_py_concat(l, obj, lookup_dict) == -1)
    {
	Py_DECREF(lookup_dict);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    Py_DECREF(lookup_dict);

    Py_INCREF((PyObject *)self);
    ಹಿಂತಿರುಗಿ (PyObject *)(self);
}

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    listwatch_T	lw;
    list_T	*list;
} listiterinfo_T;

    ಸ್ಥಿರ ಶೂನ್ಯ
ListIterDestruct(ಶೂನ್ಯ *arg)
{
    listiterinfo_T *lii = (listiterinfo_T*)arg;
    list_rem_watch(lii->list, &lii->lw);
    list_unref(lii->list);
    PyMem_Free(lii);
}

    ಸ್ಥಿರ PyObject *
List_Iter_ಮುಂದಿನ(ಶೂನ್ಯ **arg)
{
    PyObject	*ret;
    listiterinfo_T **lii = (listiterinfo_T**)arg;

    ಆದರೆ (!((*lii)->lw.lw_ಅಂಶ))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (!(ret = Convert_ಗಮ್ಯ_Py_Object(&((*lii)->lw.lw_ಅಂಶ->li_tv))))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    (*lii)->lw.lw_ಅಂಶ = (*lii)->lw.lw_ಅಂಶ->li_ಮುಂದಿನ;

    ಹಿಂತಿರುಗಿ ret;
}

    ಸ್ಥಿರ PyObject *
ListIter(PyObject *self_obj)
{
    ListObject	*self = (ListObject*)self_obj;
    listiterinfo_T	*lii;
    list_T	*l = self->list;

    ಆದರೆ (!(lii = PyMem_New(listiterinfo_T, 1)))
    {
	Py_Err_No_ಸ್ಮರಣೆ();
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    CHECK_LIST_MATERIALIZE(l);
    list_ಕೂಡಿಸು_watch(l, &lii->lw);
    lii->lw.lw_ಅಂಶ = l->lv_ಮೊದಲ;
    lii->list = l;
    ++l->lv_refcount;

    ಹಿಂತಿರುಗಿ IterNew(lii,
	    ListIterDestruct, List_Iter_ಮುಂದಿನ,
	    ಖಾಲಿ, ಖಾಲಿ, (PyObject *)self);
}

ಸ್ಥಿರ ಅಕ್ಷರ *ListAttrs[] = {
    "locked",
    ಖಾಲಿ
};

    ಸ್ಥಿರ PyObject *
ListDir(PyObject *self, PyObject *ವಾದಗಳು ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ ObjectDir(self, ListAttrs);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ListSetattr(PyObject *self_obj, ಅಕ್ಷರ *name, PyObject *valObject)
{
    ListObject *self = (ListObject*)self_obj;
    ಆದರೆ (valObject == ಖಾಲಿ)
    {
	Py_Err_ಗಣ_STRING(PyExc_AttributeError,
		N_("cannot delete vim.List attributes"));
	ಹಿಂತಿರುಗಿ -1;
    }

    ಆದರೆ (strcmp(name, "locked") == 0)
    {
	ಆದರೆ (self->list->lv_ಬೀಗ == VAR_FIXED)
	{
	    Py_Err_ಗಣ_STRING(PyExc_TypeError, N_("cannot modify fixed list"));
	    ಹಿಂತಿರುಗಿ -1;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಪೂರ್ಣಾಂಕ		istrue = PyObject_IsTrue(valObject);
	    ಆದರೆ (istrue == -1)
		ಹಿಂತಿರುಗಿ -1;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (istrue)
		self->list->lv_ಬೀಗ = VAR_LOCKED;
	    ಇಲ್ಲದಿದ್ದರೆ
		self->list->lv_ಬೀಗ = 0;
	}
	ಹಿಂತಿರುಗಿ 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	PyErr_FORMAT(PyExc_AttributeError, N_("cannot set attribute %s"), name);
	ಹಿಂತಿರುಗಿ -1;
    }
}

ಸ್ಥಿರ PySequenceMethods ListAsSeq = {
    (lenfunc)		List_ಉದ್ದಳತೆ,	 // sq_length,	  len(x)
    (binaryfunc)	0,		 // RangeConcat, sq_concat,  x+y
    0,					 // RangeRepeat, sq_repeat,  x*n
    (PyIntArgFunc)	List_ಸೂಚ್ಯಂಕ,	 // sq_item,	  x[i]
    0,					 // was_sq_slice,     x[i:j]
    (PyIntObjArgProc)	List_Ass_ಸೂಚ್ಯಂಕ,	 // sq_as_item,  x[i]=v
    0,					 // was_sq_ass_slice, x[i:j]=v
    0,					 // sq_contains
    (binaryfunc)	List_Concat_ಒಳ_ಇರಿಸು,// sq_inplace_concat
    0,					 // sq_inplace_repeat
};

ಸ್ಥಿರ PyMappingMethods ListAsMapping = {
    /* mp_length	*/ (lenfunc) List_ಉದ್ದಳತೆ,
    /* mp_subscript     */ (binaryfunc) List_ಅಂಶ,
    /* mp_ass_subscript */ (objobjargproc) List_Ass_ಅಂಶ,
};

ಸ್ಥಿರ ರಚನೆ Py_ವಿಧಾನ_Def ListMethods[] = {
    {"extend",	(PyCFunction)List_Concat_ಒಳ_ಇರಿಸು,	METH_O,		""},
    {"__dir__",	(PyCFunction)ListDir,		METH_NOARGS,	""},
    { ಖಾಲಿ,	ಖಾಲಿ,				0,		ಖಾಲಿ}
};

DEFINE_PY_TYPE_OBJECT(TupleType);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    Py_Object_ತಲೆ
    tuple_T	*tuple;
    pylinkedlist_T	ref;
} TupleObject;

#ವ್ಯಾಖ್ಯಾನಿಸು NEW_TUPLE(tuple) TupleNew(TupleTypePtr, tuple)

    ಸ್ಥಿರ PyObject *
TupleNew(PyTypeObject *subtype, tuple_T *tuple)
{
    TupleObject	*self;

    ಆದರೆ (tuple == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    self = (TupleObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    ಆದರೆ (self == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    self->tuple = tuple;
    ++tuple->tv_refcount;

    pyll_ಕೂಡಿಸು((PyObject *)(self), &self->ref, &lasttuple);

    ಹಿಂತಿರುಗಿ (PyObject *)(self);
}

    ಸ್ಥಿರ tuple_T *
py_tuple_alloc(ಶೂನ್ಯ)
{
    tuple_T	*ret;

    ಆದರೆ (!(ret = tuple_alloc()))
    {
	Py_Err_No_ಸ್ಮರಣೆ();
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ++ret->tv_refcount;

    ಹಿಂತಿರುಗಿ ret;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
tuple_py_concat(tuple_T *t, PyObject *obj, PyObject *lookup_dict)
{
    PyObject	*iterator;
    PyObject	*ಅಂಶ;

    ಆದರೆ (!(iterator = PyObject_GetIter(obj)))
	ಹಿಂತಿರುಗಿ -1;

    ಆಗ ((ಅಂಶ = Py_Iter_ಮುಂದಿನ(iterator)))
    {
	typval_T    new_tv;

	ಆದರೆ (_ConvertFromPyObject(ಅಂಶ, &new_tv, lookup_dict) == -1)
	{
	    Py_DECREF(ಅಂಶ);
	    Py_DECREF(iterator);
	    ಹಿಂತಿರುಗಿ -1;
	}

	Py_DECREF(ಅಂಶ);

	ಆದರೆ (tuple_append_tv(t, &new_tv) == FAIL)
	{
	    Py_DECREF(iterator);
	    ಹಿಂತಿರುಗಿ -1;
	}
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    ಆದರೆ (PyErr_Occurred())
	ಹಿಂತಿರುಗಿ -1;

    ಹಿಂತಿರುಗಿ 0;
}

    ಸ್ಥಿರ PyObject *
TupleConstructor(PyTypeObject *subtype, PyObject *ವಾದಗಳು, PyObject *kwargs)
{
    tuple_T	*tuple;
    PyObject	*obj = ಖಾಲಿ;

    ಆದರೆ (kwargs)
    {
	Py_Err_ಗಣ_STRING(PyExc_TypeError,
		N_("tuple constructor does not accept keyword arguments"));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಆದರೆ (!PyArg_ParseTuple(ವಾದಗಳು, "|O", &obj))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (!(tuple = py_tuple_alloc()))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (obj)
    {
	PyObject	*lookup_dict;

	ಆದರೆ (!(lookup_dict = PyDict_New()))
	{
	    tuple_unref(tuple);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

	ಆದರೆ (tuple_py_concat(tuple, obj, lookup_dict) == -1)
	{
	    Py_DECREF(lookup_dict);
	    tuple_unref(tuple);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

	Py_DECREF(lookup_dict);
    }

    ಹಿಂತಿರುಗಿ TupleNew(subtype, tuple);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
TupleDestructor(PyObject *self_obj)
{
    TupleObject *self = (TupleObject *)self_obj;
    pyll_remove(&self->ref, &lasttuple);
    tuple_unref(self->tuple);

    DESTRUCTOR_ಮುಕ್ತಾಯ(self);
}

    ಸ್ಥಿರ PyInt
Tuple_ಉದ್ದಳತೆ(TupleObject *self)
{
    ಹಿಂತಿರುಗಿ ((PyInt)(tuple_len(self->tuple)));
}

    ಸ್ಥಿರ PyObject *
Tuple_ಸೂಚ್ಯಂಕ(TupleObject *self, Py_ssize_t ಸೂಚ್ಯಂಕ)
{
    PyInt	len = Tuple_ಉದ್ದಳತೆ(self);

    ಆದರೆ (ಸೂಚ್ಯಂಕ < 0)
	ಸೂಚ್ಯಂಕ = len + ಸೂಚ್ಯಂಕ;

    ಆದರೆ (ಸೂಚ್ಯಂಕ < 0 || ಸೂಚ್ಯಂಕ >= len)
    {
	Py_Err_ಗಣ_STRING(Py_Exc_ಸೂಚ್ಯಂಕ_Error, N_("tuple index out of range"));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಹಿಂತಿರುಗಿ Convert_ಗಮ್ಯ_Py_Object(TUPLE_ಅಂಶ(self->tuple, ಸೂಚ್ಯಂಕ));
}

/*
 * Return a new tuple object for the tuple slice starting from the index
 * "first" and of length "slicelen" skipping "step" items.
 */
    ಸ್ಥಿರ PyObject *
Tuple_ತುಂಡು(
    TupleObject		*self,
    Py_ssize_t		ಮೊದಲ,
    Py_ssize_t		ಹೆಜ್ಜೆ,
    Py_ssize_t		slicelen)
{
    PyInt	i;
    PyObject	*tuple;

    tuple = PyTuple_New(slicelen);
    ಆದರೆ (tuple == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಗಾಗಿ (i = 0; i < slicelen; ++i)
    {
	PyObject	*ಅಂಶ;

	ಅಂಶ = Tuple_ಸೂಚ್ಯಂಕ(self, ಮೊದಲ + i * ಹೆಜ್ಜೆ);
	ಆದರೆ (ಅಂಶ == ಖಾಲಿ)
	{
	    Py_DECREF(tuple);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

	Py_Tuple_ಗಣ_ಅಂಶ(tuple, i, ಅಂಶ);
    }

    ಹಿಂತಿರುಗಿ tuple;
}

    ಸ್ಥಿರ PyObject *
Tuple_ಅಂಶ(TupleObject *self, PyObject* idx)
{
#ಆದರೆ PY_MAJOR_ಆವೃತ್ತಿ < 3
    ಆದರೆ (PyInt_Check(idx))
    {
	ದೀರ್ಘ _idx = PyInt_AsLong(idx);
	ಹಿಂತಿರುಗಿ Tuple_ಸೂಚ್ಯಂಕ(self, _idx);
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (PyLong_Check(idx))
    {
	ದೀರ್ಘ _idx = PyLong_AsLong(idx);
	ಹಿಂತಿರುಗಿ Tuple_ಸೂಚ್ಯಂಕ(self, _idx);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (Py_ತುಂಡು_Check(idx))
    {
	Py_ssize_t ಆರಂಭ, stop, ಹೆಜ್ಜೆ, slicelen;

	ಆದರೆ (Py_ತುಂಡು_Get_Indices_Ex((Py_ತುಂಡು_Object_T *)idx, Tuple_ಉದ್ದಳತೆ(self),
				 &ಆರಂಭ, &stop, &ಹೆಜ್ಜೆ, &slicelen) < 0)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	ಹಿಂತಿರುಗಿ Tuple_ತುಂಡು(self, ಆರಂಭ, ಹೆಜ್ಜೆ, slicelen);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	RAISE_INVALID_ಸೂಚ್ಯಂಕ_TYPE(idx);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
}

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    tuple_T	*tuple;
    ಪೂರ್ಣಾಂಕ		ಸೂಚ್ಯಂಕ;
} tupleiterinfo_T;

    ಸ್ಥಿರ ಶೂನ್ಯ
TupleIterDestruct(ಶೂನ್ಯ *arg)
{
    tupleiterinfo_T *tii = (tupleiterinfo_T*)arg;
    tuple_unref(tii->tuple);
    PyMem_Free(tii);
}

    ಸ್ಥಿರ PyObject *
Tuple_Iter_ಮುಂದಿನ(ಶೂನ್ಯ **arg)
{
    PyObject	*ret;
    tupleiterinfo_T **tii = (tupleiterinfo_T**)arg;

    ಆದರೆ ((*tii)->ಸೂಚ್ಯಂಕ >= TUPLE_LEN((*tii)->tuple))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (!(ret = Convert_ಗಮ್ಯ_Py_Object(TUPLE_ಅಂಶ((*tii)->tuple, (*tii)->ಸೂಚ್ಯಂಕ))))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    (*tii)->ಸೂಚ್ಯಂಕ++;

    ಹಿಂತಿರುಗಿ ret;
}

    ಸ್ಥಿರ PyObject *
TupleIter(PyObject *self_obj)
{
    TupleObject	*self = (TupleObject*)self_obj;
    tupleiterinfo_T	*tii;
    tuple_T	*t = self->tuple;

    ಆದರೆ (!(tii = PyMem_New(tupleiterinfo_T, 1)))
    {
	Py_Err_No_ಸ್ಮರಣೆ();
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    tii->tuple = t;
    tii->ಸೂಚ್ಯಂಕ = 0;
    ++t->tv_refcount;

    ಹಿಂತಿರುಗಿ IterNew(tii,
	    TupleIterDestruct, Tuple_Iter_ಮುಂದಿನ,
	    ಖಾಲಿ, ಖಾಲಿ, (PyObject *)self);
}

ಸ್ಥಿರ ಅಕ್ಷರ *TupleAttrs[] = {
    "locked",
    ಖಾಲಿ
};

    ಸ್ಥಿರ PyObject *
TupleDir(PyObject *self, PyObject *ವಾದಗಳು ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ ObjectDir(self, TupleAttrs);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
TupleSetattr(PyObject *self_obj, ಅಕ್ಷರ *name, PyObject *valObject)
{
    TupleObject *self = (TupleObject*)self_obj;
    ಆದರೆ (valObject == ಖಾಲಿ)
    {
	Py_Err_ಗಣ_STRING(PyExc_AttributeError,
		N_("cannot delete vim.Tuple attributes"));
	ಹಿಂತಿರುಗಿ -1;
    }

    ಆದರೆ (strcmp(name, "locked") == 0)
    {
	ಆದರೆ (self->tuple->tv_ಬೀಗ == VAR_FIXED)
	{
	    Py_Err_ಗಣ_STRING(PyExc_TypeError, N_("cannot modify fixed tuple"));
	    ಹಿಂತಿರುಗಿ -1;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಪೂರ್ಣಾಂಕ		istrue = PyObject_IsTrue(valObject);
	    ಆದರೆ (istrue == -1)
		ಹಿಂತಿರುಗಿ -1;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (istrue)
		self->tuple->tv_ಬೀಗ = VAR_LOCKED;
	    ಇಲ್ಲದಿದ್ದರೆ
		self->tuple->tv_ಬೀಗ = 0;
	}
	ಹಿಂತಿರುಗಿ 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	PyErr_FORMAT(PyExc_AttributeError, N_("cannot set attribute %s"), name);
	ಹಿಂತಿರುಗಿ -1;
    }
}

ಸ್ಥಿರ PySequenceMethods TupleAsSeq = {
    (lenfunc)		Tuple_ಉದ್ದಳತೆ,	 // sq_length,	  len(x)
    (binaryfunc)	0,		 // RangeConcat, sq_concat,  x+y
    0,					 // RangeRepeat, sq_repeat,  x*n
    (PyIntArgFunc)	Tuple_ಸೂಚ್ಯಂಕ,	 // sq_item,	  x[i]
    0,					 // was_sq_slice,     x[i:j]
    (PyIntObjArgProc)	0,		 // sq_as_item,  x[i]=v
    0,					 // was_sq_ass_slice, x[i:j]=v
    0,					 // sq_contains
    (binaryfunc)	0,		 // sq_inplace_concat
    0,					 // sq_inplace_repeat
};

ಸ್ಥಿರ PyMappingMethods TupleAsMapping = {
    /* mp_length	*/ (lenfunc) Tuple_ಉದ್ದಳತೆ,
    /* mp_subscript     */ (binaryfunc) Tuple_ಅಂಶ,
    /* mp_ass_subscript */ (objobjargproc) 0,
};

ಸ್ಥಿರ ರಚನೆ Py_ವಿಧಾನ_Def TupleMethods[] = {
    {"__dir__",	(PyCFunction)TupleDir,		METH_NOARGS,	""},
    { ಖಾಲಿ,	ಖಾಲಿ,				0,		ಖಾಲಿ}
};

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    Py_Object_ತಲೆ
    char_u	*name;
    ಪೂರ್ಣಾಂಕ		argc;
    typval_T	*argv;
    dict_T	*self;
    pylinkedlist_T	ref;
    ಪೂರ್ಣಾಂಕ		auto_rebind;
} FunctionObject;

DEFINE_PY_TYPE_OBJECT(FunctionType);

#ವ್ಯಾಖ್ಯಾನಿಸು NEW_FUNCTION(name, argc, argv, self, pt_auto) \
    FunctionNew(FunctionTypePtr, (name), (argc), (argv), (self), (pt_auto))

    ಸ್ಥಿರ PyObject *
FunctionNew(PyTypeObject *subtype, char_u *name, ಪೂರ್ಣಾಂಕ argc, typval_T *argv,
	dict_T *selfdict, ಪೂರ್ಣಾಂಕ auto_rebind)
{
    FunctionObject	*self;

    self = (FunctionObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    ಆದರೆ (self == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (ಅಂಕಿಯೇ((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)*name))
    {
	ಆದರೆ (!translated_function_exists(name, FALSE))
	{
	    PyErr_FORMAT(Py_Exc_ಮೌಲ್ಯ_Error,
		    N_("unnamed function %s does not exist"), name);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
	self->name = vim_strsave(name);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	char_u *p;

	ಆದರೆ ((p = get_expanded_name(name,
			    vim_strchr(name, AUTOLOAD_CHAR) == ಖಾಲಿ)) == ಖಾಲಿ)
	{
	    PyErr_FORMAT(Py_Exc_ಮೌಲ್ಯ_Error,
		    N_("function %s does not exist"), name);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

	ಆದರೆ (p[0] == K_SPECIAL && p[1] == KS_EXTRA && p[2] == (ಪೂರ್ಣಾಂಕ)KE_SNR)
	{
	    char_u *np;
	    size_t len = STRLEN(p) + 1;

	    ಆದರೆ ((np = alloc(len + 2)) == ಖಾಲಿ)
	    {
		vim_free(p);
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    }
	    mch_memmove(np, "<SNR>", 5);
	    mch_memmove(np + 5, p + 3, len - 3);
	    vim_free(p);
	    self->name = np;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    self->name = p;
    }

    func_ref(self->name);
    self->argc = argc;
    self->argv = argv;
    self->self = selfdict;
    self->auto_rebind = selfdict == ಖಾಲಿ ? TRUE : auto_rebind;

    ಆದರೆ (self->argv || self->self)
	pyll_ಕೂಡಿಸು((PyObject *)(self), &self->ref, &lastfunc);

    ಹಿಂತಿರುಗಿ (PyObject *)(self);
}

    ಸ್ಥಿರ PyObject *
FunctionConstructor(PyTypeObject *subtype, PyObject *ವಾದಗಳು, PyObject *kwargs)
{
    PyObject	*self;
    PyObject	*selfdictObject;
    PyObject	*autoRebindObject;
    PyObject	*ವಾದಗಳು_Object = ಖಾಲಿ;
    char_u	*name;
    typval_T	selfdicttv;
    typval_T	argstv;
    list_T	*argslist = ಖಾಲಿ;
    dict_T	*selfdict = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		argc = 0;
    ಪೂರ್ಣಾಂಕ		auto_rebind = TRUE;
    typval_T	*argv = ಖಾಲಿ;
    typval_T	*curtv;
    listitem_T	*li;

    ಆದರೆ (kwargs != ಖಾಲಿ)
    {
	selfdictObject = Py_Dict_Get_ಅಂಶ_String(kwargs, "self");
	ಆದರೆ (selfdictObject != ಖಾಲಿ)
	{
	    ಆದರೆ (ConvertFromPyMapping(selfdictObject, &selfdicttv) == -1)
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    selfdict = selfdicttv.vval.v_dict;
	}
	ವಾದಗಳು_Object = Py_Dict_Get_ಅಂಶ_String(kwargs, "args");
	ಆದರೆ (ವಾದಗಳು_Object != ಖಾಲಿ)
	{
	    ಆದರೆ (ConvertFromPySequence(ವಾದಗಳು_Object, &argstv) == -1)
	    {
		dict_unref(selfdict);
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    }
	    argslist = argstv.vval.v_list;
	    CHECK_LIST_MATERIALIZE(argslist);

	    argc = argslist->lv_len;
	    ಆದರೆ (argc != 0)
	    {
		argv = PyMem_New(typval_T, (size_t) argc);
		ಆದರೆ (argv == ಖಾಲಿ)
		{
		    Py_Err_No_ಸ್ಮರಣೆ();
		    dict_unref(selfdict);
		    list_unref(argslist);
		    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		}
		curtv = argv;
		FOR_ALL_LIST_ವಸ್ತುಗಳು(argslist, li)
		    copy_tv(&li->li_tv, curtv++);
	    }
	    list_unref(argslist);
	}
	ಆದರೆ (selfdict != ಖಾಲಿ)
	{
	    auto_rebind = FALSE;
	    autoRebindObject = Py_Dict_Get_ಅಂಶ_String(kwargs, "auto_rebind");
	    ಆದರೆ (autoRebindObject != ಖಾಲಿ)
	    {
		auto_rebind = PyObject_IsTrue(autoRebindObject);
		ಆದರೆ (auto_rebind == -1)
		{
		    dict_unref(selfdict);
		    list_unref(argslist);
		    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		}
	    }
	}
    }

    ಆದರೆ (!PyArg_ParseTuple(ವಾದಗಳು, "et", "ascii", &name))
    {
	dict_unref(selfdict);
	ಆಗ (argc--)
	    clear_tv(&argv[argc]);
	PyMem_Free(argv);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    self = FunctionNew(subtype, name, argc, argv, selfdict, auto_rebind);

    PyMem_Free(name);

    ಹಿಂತಿರುಗಿ self;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
FunctionDestructor(PyObject *self_obj)
{
    FunctionObject *self = (FunctionObject*)self_obj;
    ಪೂರ್ಣಾಂಕ i;
    func_unref(self->name);
    vim_free(self->name);
    ಗಾಗಿ (i = 0; i < self->argc; ++i)
	clear_tv(&self->argv[i]);
    PyMem_Free(self->argv);
    dict_unref(self->self);
    ಆದರೆ (self->argv || self->self)
	pyll_remove(&self->ref, &lastfunc);

    DESTRUCTOR_ಮುಕ್ತಾಯ(self);
}

ಸ್ಥಿರ ಅಕ್ಷರ *FunctionAttrs[] = {
    "softspace", "args", "self", "auto_rebind",
    ಖಾಲಿ
};

    ಸ್ಥಿರ PyObject *
FunctionDir(PyObject *self, PyObject *ವಾದಗಳು ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ ObjectDir(self, FunctionAttrs);
}

    ಸ್ಥಿರ PyObject *
FunctionAttr(FunctionObject *self, ಅಕ್ಷರ *name)
{
    list_T *list;
    ಪೂರ್ಣಾಂಕ i;
    ಆದರೆ (strcmp(name, "name") == 0)
	ಹಿಂತಿರುಗಿ PyString_FromString((ಅಕ್ಷರ *)(self->name));
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(name, "args") == 0)
    {
	ಆದರೆ (self->argv == ಖಾಲಿ || (list = list_alloc()) == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ALWAYS_NONE;

	ಗಾಗಿ (i = 0; i < self->argc; ++i)
	    list_append_tv(list, &self->argv[i]);
	ಹಿಂತಿರುಗಿ NEW_LIST(list);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(name, "self") == 0)
	ಹಿಂತಿರುಗಿ self->self == ಖಾಲಿ
	    ? ALWAYS_NONE
	    : NEW_DICTIONARY(self->self);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(name, "auto_rebind") == 0)
	ಹಿಂತಿರುಗಿ self->auto_rebind
	    ? ALWAYS_TRUE
	    : ALWAYS_FALSE;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(name, "__members__") == 0)
	ಹಿಂತಿರುಗಿ ObjectDir(ಖಾಲಿ, FunctionAttrs);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Populate partial_T given function object.
 *
 * "exported" should be set to true when it is needed to construct a partial
 * that may be stored in a variable (i.e. may be freed by Vim).
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_partial(FunctionObject *self, partial_T *pt, ಪೂರ್ಣಾಂಕ exported)
{
    ಪೂರ್ಣಾಂಕ i;

    pt->pt_name = self->name;
    ಆದರೆ (self->argv)
    {
	pt->pt_argc = self->argc;
	ಆದರೆ (exported)
	{
	    pt->pt_argv = ALLOC_CLEAR_MULT(typval_T, self->argc);
	    ಗಾಗಿ (i = 0; i < pt->pt_argc; ++i)
		copy_tv(&self->argv[i], &pt->pt_argv[i]);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    pt->pt_argv = self->argv;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	pt->pt_argc = 0;
	pt->pt_argv = ಖಾಲಿ;
    }
    pt->pt_auto = self->auto_rebind || !exported;
    pt->pt_dict = self->self;
    ಆದರೆ (exported && self->self)
	++pt->pt_dict->dv_refcount;
    ಆದರೆ (exported)
	pt->pt_name = vim_strsave(pt->pt_name);
    pt->pt_refcount = 1;
}

    ಸ್ಥಿರ PyObject *
FunctionCall(PyObject *self_obj, PyObject *ವಾದಗಳು_Object, PyObject *kwargs)
{
    FunctionObject *self = (FunctionObject*)self_obj;
    char_u	*name = self->name;
    typval_T	ವಾದಗಳು;
    typval_T	selfdicttv;
    typval_T	rettv;
    dict_T	*selfdict = ಖಾಲಿ;
    PyObject	*selfdictObject;
    PyObject	*ret;
    ಪೂರ್ಣಾಂಕ		error;
    partial_T	pt;
    partial_T	*pt_ptr = ಖಾಲಿ;

    ಆದರೆ (ConvertFromPySequence(ವಾದಗಳು_Object, &ವಾದಗಳು) == -1)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (kwargs != ಖಾಲಿ)
    {
	selfdictObject = Py_Dict_Get_ಅಂಶ_String(kwargs, "self");
	ಆದರೆ (selfdictObject != ಖಾಲಿ)
	{
	    ಆದರೆ (ConvertFromPyMapping(selfdictObject, &selfdicttv) == -1)
	    {
		clear_tv(&ವಾದಗಳು);
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    }
	    selfdict = selfdicttv.vval.v_dict;
	}
    }

    ಆದರೆ (self->argv || self->self)
    {
	CLEAR_ಕ್ಷೇತ್ರ(pt);
	ಗಣ_partial(self, &pt, FALSE);
	pt_ptr = &pt;
    }

    Py_BEGIN_ALLOW_ಎಳೆಗಳು
    Python_ಬೀಗ_Vim();

    Vim_Try_ಆರಂಭ();
    error = func_call(name, &ವಾದಗಳು, pt_ptr, selfdict, &rettv);

    Python_Release_Vim();
    Py_ಕೊನೆ_ALLOW_ಎಳೆಗಳು

    ಆದರೆ (Vim_Try_ಕೊನೆ())
	ret = ಖಾಲಿ;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (error != OK)
    {
	ret = ಖಾಲಿ;
	PyErr_VIM_FORMAT(N_("failed to run function %s"), (ಅಕ್ಷರ *)name);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ret = Convert_ಗಮ್ಯ_Py_Object(&rettv);

    clear_tv(&ವಾದಗಳು);
    clear_tv(&rettv);
    ಆದರೆ (selfdict != ಖಾಲಿ)
	clear_tv(&selfdicttv);

    ಹಿಂತಿರುಗಿ ret;
}

    ಸ್ಥಿರ PyObject *
FunctionRepr(PyObject *self_obj)
{
    FunctionObject *self = (FunctionObject*)self_obj;
    PyObject *ret;
    garray_T repr_ga;
    ಪೂರ್ಣಾಂಕ i;
    char_u *tofree = ಖಾಲಿ;
    typval_T tv;
    char_u numbuf[NUMBUFLEN];

    ga_init2(&repr_ga, ಗಾತ್ರ(ಅಕ್ಷರ), 70);
    ga_concat(&repr_ga, (char_u *)"<vim.Function '");
    ಆದರೆ (self->name)
	ga_concat(&repr_ga, self->name);
    ಇಲ್ಲದಿದ್ದರೆ
	ga_concat(&repr_ga, (char_u *)"<NULL>");
    ga_append(&repr_ga, '\'');
    ಆದರೆ (self->argv)
    {
	ga_concat(&repr_ga, (char_u *)", args=[");
	++emsg_silent;
	ಗಾಗಿ (i = 0; i < self->argc; i++)
	{
	    ಆದರೆ (i != 0)
		ga_concat(&repr_ga, (char_u *)", ");
	    ga_concat(&repr_ga, tv2string(&self->argv[i], &tofree, numbuf,
			get_copyID()));
	    vim_free(tofree);
	}
	--emsg_silent;
	ga_append(&repr_ga, ']');
    }
    ಆದರೆ (self->self)
    {
	ga_concat(&repr_ga, (char_u *)", self=");
	tv.v_type = VAR_DICT;
	tv.vval.v_dict = self->self;
	++emsg_silent;
	ga_concat(&repr_ga, tv2string(&tv, &tofree, numbuf, get_copyID()));
	--emsg_silent;
	vim_free(tofree);
	ಆದರೆ (self->auto_rebind)
	    ga_concat(&repr_ga, (char_u *)", auto_rebind=True");
    }
    ga_append(&repr_ga, '>');
    ret = PyString_FromString((ಅಕ್ಷರ *)repr_ga.ga_ದತ್ತಾಂಶ);
    ga_clear(&repr_ga);
    ಹಿಂತಿರುಗಿ ret;
}

ಸ್ಥಿರ ರಚನೆ Py_ವಿಧಾನ_Def FunctionMethods[] = {
    {"__dir__",	(PyCFunction)FunctionDir,   METH_NOARGS,		""},
    { ಖಾಲಿ,	ಖಾಲಿ,			0,				ಖಾಲಿ}
};

/*
 * Options object
 */

DEFINE_PY_TYPE_OBJECT(OptionsType);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*checkfun)(ಶೂನ್ಯ *);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    Py_Object_ತಲೆ
    ಪೂರ್ಣಾಂಕ		opt_type;
    ಶೂನ್ಯ	*from;
    checkfun	Check;
    PyObject	*fromObj;
} OptionsObject;

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
dummy_check(ಶೂನ್ಯ *arg ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ 0;
}

    ಸ್ಥಿರ PyObject *
OptionsNew(ಪೂರ್ಣಾಂಕ opt_type, ಶೂನ್ಯ *from, checkfun Check, PyObject *fromObj)
{
    OptionsObject	*self;

    self = PyObject_GC_New(OptionsObject, OptionsTypePtr);
    ಆದರೆ (self == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    self->opt_type = opt_type;
    self->from = from;
    self->Check = Check;
    self->fromObj = fromObj;
    ಆದರೆ (fromObj)
	Py_INCREF(fromObj);

    ಹಿಂತಿರುಗಿ (PyObject *)(self);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
OptionsDestructor(PyObject *self_obj)
{
    OptionsObject *self = (OptionsObject*)self_obj;
    PyObject_GC_UnTrack((ಶೂನ್ಯ *)(self));
    Py_XDECREF(self->fromObj);
    PyObject_GC_Del((ಶೂನ್ಯ *)(self));
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
OptionsTraverse(PyObject *self_obj, visitproc ಭೇಟಿ, ಶೂನ್ಯ *arg)
{
    OptionsObject *self = (OptionsObject*)self_obj;
    Py_ಭೇಟಿ(self->fromObj);
    ಹಿಂತಿರುಗಿ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
OptionsClear(PyObject *self_obj)
{
    OptionsObject *self = (OptionsObject*)self_obj;
    Py_CLEAR(self->fromObj);
    ಹಿಂತಿರುಗಿ 0;
}

    ಸ್ಥಿರ PyObject *
Options_ಅಂಶ(OptionsObject *self, PyObject *ಕೀಲಿ_Object)
{
    char_u	*ಕೀಲಿ;
    ಪೂರ್ಣಾಂಕ		flags;
    ದೀರ್ಘ	numval;
    char_u	*stringval;
    PyObject	*todecref;

    ಆದರೆ (self->Check(self->fromObj))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (!(ಕೀಲಿ = String_ಗಮ್ಯ_Chars(ಕೀಲಿ_Object, &todecref)))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (*ಕೀಲಿ == NUL)
    {
	RAISE_NO_ಖಾಲಿಗಳು_ಕೀಲಿಗಳು;
	Py_XDECREF(todecref);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    flags = get_option_ಮೌಲ್ಯ_strict(ಕೀಲಿ, &numval, &stringval,
				    self->opt_type, self->from);

    Py_XDECREF(todecref);

    ಆದರೆ (flags == 0)
    {
	Py_Err_ಗಣ_Object(Py_Exc_ಕೀಲಿ_Error, ಕೀಲಿ_Object);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಆದರೆ (flags & SOPT_UNSET)
    {
	Py_INCREF(Py_None);
	ಹಿಂತಿರುಗಿ Py_None;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (flags & SOPT_BOOL)
    {
	PyObject	*ret;
	ret = numval ? Py_True : Py_False;
	Py_INCREF(ret);
	ಹಿಂತಿರುಗಿ ret;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (flags & SOPT_ಅಂಕೆ)
	ಹಿಂತಿರುಗಿ PyInt_FromLong(numval);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (flags & SOPT_STRING)
    {
	ಆದರೆ (stringval)
	{
	    PyObject	*ret = Py_ಬೈಟುಗಳು_From_String((ಅಕ್ಷರ *)stringval);
	    vim_free(stringval);
	    ಹಿಂತಿರುಗಿ ret;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    Py_Err_ಗಣ_STRING(PyExc_RuntimeError,
		    N_("unable to get option value"));
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	Py_Err_ಗಣ_VIM(N_("internal error: unknown option type"));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
Options_ಒಳಗೊಂಡಿದೆ(OptionsObject *self, PyObject *ಕೀಲಿ_Object)
{
    char_u	*ಕೀಲಿ;
    PyObject	*todecref;

    ಆದರೆ (!(ಕೀಲಿ = String_ಗಮ್ಯ_Chars(ಕೀಲಿ_Object, &todecref)))
	ಹಿಂತಿರುಗಿ -1;

    ಆದರೆ (*ಕೀಲಿ == NUL)
    {
	Py_XDECREF(todecref);
	ಹಿಂತಿರುಗಿ 0;
    }

    ಆದರೆ (get_option_ಮೌಲ್ಯ_strict(ಕೀಲಿ, ಖಾಲಿ, ಖಾಲಿ, self->opt_type, ಖಾಲಿ))
    {
	Py_XDECREF(todecref);
	ಹಿಂತಿರುಗಿ 1;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	Py_XDECREF(todecref);
	ಹಿಂತಿರುಗಿ 0;
    }
}

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ಶೂನ್ಯ	*lastoption;
    ಪೂರ್ಣಾಂಕ		opt_type;
} optiterinfo_T;

    ಸ್ಥಿರ PyObject *
Options_Iter_ಮುಂದಿನ(ಶೂನ್ಯ **arg)
{
    char_u	*name;
    optiterinfo_T **oii = (optiterinfo_T**)arg;

    ಆದರೆ ((name = option_iter_ಮುಂದಿನ(&((*oii)->lastoption), (*oii)->opt_type)))
	ಹಿಂತಿರುಗಿ PyString_FromString((ಅಕ್ಷರ *)name);

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಸ್ಥಿರ PyObject *
OptionsIter(PyObject *self_obj)
{
    OptionsObject	*self = (OptionsObject*)self_obj;
    optiterinfo_T	*oii;

    ಆದರೆ (!(oii = PyMem_New(optiterinfo_T, 1)))
    {
	Py_Err_No_ಸ್ಮರಣೆ();
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    oii->opt_type = self->opt_type;
    oii->lastoption = ಖಾಲಿ;

    ಹಿಂತಿರುಗಿ IterNew(oii,
	    PyMem_Free, Options_Iter_ಮುಂದಿನ,
	    ಖಾಲಿ, ಖಾಲಿ, (PyObject *)self);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಗಣ_option_ಮೌಲ್ಯ_err(char_u *ಕೀಲಿ, ಪೂರ್ಣಾಂಕ numval, char_u *stringval, ಪೂರ್ಣಾಂಕ opt_flags)
{
    ಅಕ್ಷರ	*errmsg;

    ಆದರೆ ((errmsg = ಗಣ_option_ಮೌಲ್ಯ(ಕೀಲಿ, numval, stringval, opt_flags)))
    {
	ಆದರೆ (Vim_Try_ಕೊನೆ())
	    ಹಿಂತಿರುಗಿ FAIL;
	Py_Err_ಗಣ_Vim(errmsg);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಗಣ_option_ಮೌಲ್ಯ_for(
	char_u *ಕೀಲಿ,
	ಪೂರ್ಣಾಂಕ numval,
	char_u *stringval,
	ಪೂರ್ಣಾಂಕ opt_flags,
	ಪೂರ್ಣಾಂಕ opt_type,
	ಶೂನ್ಯ *from)
{
    switchwin_T	switchwin;
    bufref_T	save_curbuf;
    ಪೂರ್ಣಾಂಕ		ಗಣ_ret = 0;

    Vim_Try_ಆರಂಭ();
    ಆಯ್ಕೆ (opt_type)
    {
	ಸ್ಥಿತಿ SREQ_WIN:
	    ಆದರೆ (switch_win(&switchwin, (win_T *)from,
			      win_ಕಂಡುಹಿಡಿ_tabpage((win_T *)from), FALSE) == FAIL)
	    {
		restore_win(&switchwin, TRUE);
		ಆದರೆ (Vim_Try_ಕೊನೆ())
		    ಹಿಂತಿರುಗಿ -1;
		Py_Err_ಗಣ_VIM(N_("problem while switching windows"));
		ಹಿಂತಿರುಗಿ -1;
	    }
	    ಗಣ_ret = ಗಣ_option_ಮೌಲ್ಯ_err(ಕೀಲಿ, numval, stringval, opt_flags);
	    restore_win(&switchwin, TRUE);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SREQ_BUF:
	    switch_ಬಫರ್(&save_curbuf, (buf_T *)from);
	    ಗಣ_ret = ಗಣ_option_ಮೌಲ್ಯ_err(ಕೀಲಿ, numval, stringval, opt_flags);
	    restore_ಬಫರ್(&save_curbuf);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SREQ_GLOBAL:
	    ಗಣ_ret = ಗಣ_option_ಮೌಲ್ಯ_err(ಕೀಲಿ, numval, stringval, opt_flags);
	    ನಿಲ್ಲಿಸು;
    }
    ಆದರೆ (ಗಣ_ret == FAIL)
	ಹಿಂತಿರುಗಿ -1;
    ಹಿಂತಿರುಗಿ Vim_Try_ಕೊನೆ();
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
Options_Ass_ಅಂಶ(OptionsObject *self, PyObject *ಕೀಲಿ_Object, PyObject *valObject)
{
    char_u	*ಕೀಲಿ;
    ಪೂರ್ಣಾಂಕ		flags;
    ಪೂರ್ಣಾಂಕ		opt_flags;
    ಪೂರ್ಣಾಂಕ		ret = 0;
    PyObject	*todecref;

    ಆದರೆ (self->Check(self->fromObj))
	ಹಿಂತಿರುಗಿ -1;

    ಆದರೆ (!(ಕೀಲಿ = String_ಗಮ್ಯ_Chars(ಕೀಲಿ_Object, &todecref)))
	ಹಿಂತಿರುಗಿ -1;

    ಆದರೆ (*ಕೀಲಿ == NUL)
    {
	RAISE_NO_ಖಾಲಿಗಳು_ಕೀಲಿಗಳು;
	Py_XDECREF(todecref);
	ಹಿಂತಿರುಗಿ -1;
    }

    flags = get_option_ಮೌಲ್ಯ_strict(ಕೀಲಿ, ಖಾಲಿ, ಖಾಲಿ,
				    self->opt_type, self->from);

    ಆದರೆ (flags == 0)
    {
	Py_Err_ಗಣ_Object(Py_Exc_ಕೀಲಿ_Error, ಕೀಲಿ_Object);
	Py_XDECREF(todecref);
	ಹಿಂತಿರುಗಿ -1;
    }

    ಆದರೆ (valObject == ಖಾಲಿ)
    {
	ಆದರೆ (self->opt_type == SREQ_GLOBAL)
	{
	    PyErr_FORMAT(Py_Exc_ಮೌಲ್ಯ_Error,
		    N_("unable to unset global option %s"), ಕೀಲಿ);
	    Py_XDECREF(todecref);
	    ಹಿಂತಿರುಗಿ -1;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!(flags & SOPT_GLOBAL))
	{
	    PyErr_FORMAT(Py_Exc_ಮೌಲ್ಯ_Error,
		    N_("unable to unset option %s "
		       "which does not have global value"), ಕೀಲಿ);
	    Py_XDECREF(todecref);
	    ಹಿಂತಿರುಗಿ -1;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    unset_global_local_option(ಕೀಲಿ, self->from);
	    Py_XDECREF(todecref);
	    ಹಿಂತಿರುಗಿ 0;
	}
    }

    opt_flags = (self->opt_type ? OPT_LOCAL : OPT_GLOBAL);

    ಆದರೆ (flags & SOPT_BOOL)
    {
	ಪೂರ್ಣಾಂಕ	istrue = PyObject_IsTrue(valObject);

	ಆದರೆ (istrue == -1)
	    ret = -1;
	ಇಲ್ಲದಿದ್ದರೆ
	    ret = ಗಣ_option_ಮೌಲ್ಯ_for(ಕೀಲಿ, istrue, ಖಾಲಿ,
				    opt_flags, self->opt_type, self->from);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (flags & SOPT_ಅಂಕೆ)
    {
	ದೀರ್ಘ	val;

	ಆದರೆ (ಸಂಖ್ಯೆ_ಗಮ್ಯ_Long(valObject, &val, ಸಂಖ್ಯೆ_INT))
	{
	    Py_XDECREF(todecref);
	    ಹಿಂತಿರುಗಿ -1;
	}

	ret = ಗಣ_option_ಮೌಲ್ಯ_for(ಕೀಲಿ, (ಪೂರ್ಣಾಂಕ) val, ಖಾಲಿ, opt_flags,
				self->opt_type, self->from);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	char_u		*val;
	PyObject	*todecref2;

	ಆದರೆ ((val = String_ಗಮ್ಯ_Chars(valObject, &todecref2)))
	{
	    ret = ಗಣ_option_ಮೌಲ್ಯ_for(ಕೀಲಿ, 0, val, opt_flags,
				    self->opt_type, self->from);
	    Py_XDECREF(todecref2);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ret = -1;
    }

    Py_XDECREF(todecref);

    ಹಿಂತಿರುಗಿ ret;
}

ಸ್ಥಿರ PySequenceMethods OptionsAsSeq = {
    0,					// sq_length
    0,					// sq_concat
    0,					// sq_repeat
    0,					// sq_item
    0,					// sq_slice
    0,					// sq_ass_item
    0,					// sq_ass_slice
    (objobjproc) Options_ಒಳಗೊಂಡಿದೆ,	// sq_contains
    0,					// sq_inplace_concat
    0,					// sq_inplace_repeat
};

ಸ್ಥಿರ PyMappingMethods OptionsAsMapping = {
    (lenfunc)       ಖಾಲಿ,
    (binaryfunc)    Options_ಅಂಶ,
    (objobjargproc) Options_Ass_ಅಂಶ,
};

// Tabpage object

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    Py_Object_ತಲೆ
    tabpage_T	*tab;
} Tab_ಪುಟ_Object;

ಸ್ಥಿರ PyObject *WinListNew(Tab_ಪುಟ_Object *tabObject);

DEFINE_PY_TYPE_OBJECT(Tab_ಪುಟ_Type);

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
Check_Tab_ಪುಟ(Tab_ಪುಟ_Object *self)
{
    ಆದರೆ (self->tab == INVALID_TABPAGE_ಮೌಲ್ಯ)
    {
	Py_Err_ಗಣ_VIM(N_("attempt to refer to deleted tab page"));
	ಹಿಂತಿರುಗಿ -1;
    }

    ಹಿಂತಿರುಗಿ 0;
}

    ಸ್ಥಿರ PyObject *
Tab_ಪುಟ_New(tabpage_T *tab)
{
    Tab_ಪುಟ_Object *self;

    ಆದರೆ (TAB_PYTHON_REF(tab))
    {
	self = TAB_PYTHON_REF(tab);
	Py_INCREF((PyObject *)self);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	self = PyObject_NEW(Tab_ಪುಟ_Object, Tab_ಪುಟ_Type_Ptr);
	ಆದರೆ (self == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	self->tab = tab;
	TAB_PYTHON_REF(tab) = self;
    }

    ಹಿಂತಿರುಗಿ (PyObject *)(self);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
Tab_ಪುಟ_Destructor(PyObject *self_obj)
{
    Tab_ಪುಟ_Object *self = (Tab_ಪುಟ_Object*)self_obj;
    ಆದರೆ (self->tab && self->tab != INVALID_TABPAGE_ಮೌಲ್ಯ)
	TAB_PYTHON_REF(self->tab) = ಖಾಲಿ;

    DESTRUCTOR_ಮುಕ್ತಾಯ(self);
}

ಸ್ಥಿರ ಅಕ್ಷರ *Tab_ಪುಟ_Attrs[] = {
    "windows", "number", "vars", "window", "valid",
    ಖಾಲಿ
};

    ಸ್ಥಿರ PyObject *
Tab_ಪುಟ_Dir(PyObject *self, PyObject *ವಾದಗಳು ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ ObjectDir(self, Tab_ಪುಟ_Attrs);
}

    ಸ್ಥಿರ PyObject *
Tab_ಪುಟ_Attr_Valid(Tab_ಪುಟ_Object *self, ಅಕ್ಷರ *name)
{
    PyObject	*ret;

    ಆದರೆ (strcmp(name, "valid") != 0)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ret = ((self->tab == INVALID_TABPAGE_ಮೌಲ್ಯ) ? Py_False : Py_True);
    Py_INCREF(ret);
    ಹಿಂತಿರುಗಿ ret;
}

    ಸ್ಥಿರ PyObject *
Tab_ಪುಟ_Attr(Tab_ಪುಟ_Object *self, ಅಕ್ಷರ *name)
{
    ಆದರೆ (strcmp(name, "windows") == 0)
	ಹಿಂತಿರುಗಿ WinListNew(self);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(name, "number") == 0)
	ಹಿಂತಿರುಗಿ PyLong_FromLong((ದೀರ್ಘ) get_tab_ಸಂಖ್ಯೆ(self->tab));
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(name, "vars") == 0)
	ಹಿಂತಿರುಗಿ NEW_DICTIONARY(self->tab->tp_vars);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(name, "window") == 0)
    {
	// For current tab window.c does not bother to set or update tp_curwin
	ಆದರೆ (self->tab == curtab)
	    ಹಿಂತಿರುಗಿ ಕಿಟಕಿ_New(curwin, curtab);
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ ಕಿಟಕಿ_New(self->tab->tp_curwin, self->tab);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(name, "__members__") == 0)
	ಹಿಂತಿರುಗಿ ObjectDir(ಖಾಲಿ, Tab_ಪುಟ_Attrs);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಸ್ಥಿರ PyObject *
Tab_ಪುಟ_Repr(PyObject *self_obj)
{
    Tab_ಪುಟ_Object *self = (Tab_ಪುಟ_Object*)self_obj;
    ಆದರೆ (self->tab == INVALID_TABPAGE_ಮೌಲ್ಯ)
	ಹಿಂತಿರುಗಿ PyString_FromFormat("<tabpage object (deleted) at %p>", (ಶೂನ್ಯ *)self);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪೂರ್ಣಾಂಕ	t = get_tab_ಸಂಖ್ಯೆ(self->tab);

	ಆದರೆ (t == 0)
	    ಹಿಂತಿರುಗಿ PyString_FromFormat("<tabpage object (unknown) at %p>",
					(ಶೂನ್ಯ *)self);
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ PyString_FromFormat("<tabpage %d>", t - 1);
    }
}

ಸ್ಥಿರ ರಚನೆ Py_ವಿಧಾನ_Def Tab_ಪುಟ_Methods[] = {
    // name,	    function,			calling,	documentation
    {"__dir__",	    (PyCFunction)Tab_ಪುಟ_Dir,	METH_NOARGS,	""},
    { ಖಾಲಿ,	    ಖಾಲಿ,			0,		ಖಾಲಿ}
};

/*
 * Window list object
 */

DEFINE_PY_TYPE_OBJECT(TabListType);
ಸ್ಥಿರ PySequenceMethods TabListAsSeq;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    Py_Object_ತಲೆ
} TabListObject;

ಸ್ಥಿರ TabListObject The_Tab_ಪುಟ_List =
{
    Py_Object_ತಲೆ_INIT_TYPE(TabListType)
};

    ಸ್ಥಿರ PyInt
Tab_List_ಉದ್ದಳತೆ(PyObject *self ಬಳಸದ)
{
    tabpage_T	*tp = ಮೊದಲ_tabpage;
    PyInt	n = 0;

    ಆಗ (tp != ಖಾಲಿ)
    {
	++n;
	tp = tp->tp_ಮುಂದಿನ;
    }

    ಹಿಂತಿರುಗಿ n;
}

    ಸ್ಥಿರ PyObject *
Tab_List_ಅಂಶ(PyObject *self ಬಳಸದ, PyInt n)
{
    tabpage_T	*tp;

    ಗಾಗಿ (tp = ಮೊದಲ_tabpage; tp != ಖಾಲಿ; tp = tp->tp_ಮುಂದಿನ, --n)
	ಆದರೆ (n == 0)
	    ಹಿಂತಿರುಗಿ Tab_ಪುಟ_New(tp);

    Py_Err_ಗಣ_STRING(Py_Exc_ಸೂಚ್ಯಂಕ_Error, N_("no such tab page"));
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Window object
 */

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    Py_Object_ತಲೆ
    win_T	*win;
    Tab_ಪುಟ_Object	*tabObject;
} ಕಿಟಕಿ_Object;

DEFINE_PY_TYPE_OBJECT(ಕಿಟಕಿ_Type);

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
Check_ಕಿಟಕಿ(ಕಿಟಕಿ_Object *self)
{
    ಆದರೆ (self->win == INVALID_ಕಿಟಕಿ_ಮೌಲ್ಯ)
    {
	Py_Err_ಗಣ_VIM(N_("attempt to refer to deleted window"));
	ಹಿಂತಿರುಗಿ -1;
    }

    ಹಿಂತಿರುಗಿ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
Check_ಕಿಟಕಿ_Cb(ಶೂನ್ಯ *self)
{
    ಹಿಂತಿರುಗಿ Check_ಕಿಟಕಿ((ಕಿಟಕಿ_Object*)self);
}

    ಸ್ಥಿರ PyObject *
ಕಿಟಕಿ_New(win_T *win, tabpage_T *tab)
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

    ಕಿಟಕಿ_Object *self;

    ಆದರೆ (WIN_PYTHON_REF(win))
    {
	self = WIN_PYTHON_REF(win);
	Py_INCREF((PyObject *)self);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	self = PyObject_GC_New(ಕಿಟಕಿ_Object, ಕಿಟಕಿ_Type_Ptr);
	ಆದರೆ (self == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	self->win = win;
	WIN_PYTHON_REF(win) = self;
    }

    self->tabObject = ((Tab_ಪುಟ_Object *)(Tab_ಪುಟ_New(tab)));

    ಹಿಂತಿರುಗಿ (PyObject *)(self);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಕಿಟಕಿ_Destructor(PyObject *self_obj)
{
    ಕಿಟಕಿ_Object *self = (ಕಿಟಕಿ_Object*)self_obj;
    PyObject_GC_UnTrack((ಶೂನ್ಯ *)(self));
    ಆದರೆ (self->win && self->win != INVALID_ಕಿಟಕಿ_ಮೌಲ್ಯ)
	WIN_PYTHON_REF(self->win) = ಖಾಲಿ;
    Py_XDECREF(((PyObject *)(self->tabObject)));
    PyObject_GC_Del((ಶೂನ್ಯ *)(self));
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಕಿಟಕಿ_Traverse(PyObject *self_obj, visitproc ಭೇಟಿ, ಶೂನ್ಯ *arg)
{
    ಕಿಟಕಿ_Object *self = (ಕಿಟಕಿ_Object*)self_obj;
    Py_ಭೇಟಿ(((PyObject *)(self->tabObject)));
    ಹಿಂತಿರುಗಿ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಕಿಟಕಿ_Clear(PyObject *self_obj)
{
    ಕಿಟಕಿ_Object *self = (ಕಿಟಕಿ_Object*)self_obj;
    Py_CLEAR(self->tabObject);
    ಹಿಂತಿರುಗಿ 0;
}

    ಸ್ಥಿರ win_T *
get_firstwin(Tab_ಪುಟ_Object *tabObject)
{
    ಆದರೆ (tabObject)
    {
	ಆದರೆ (Check_Tab_ಪುಟ(tabObject))
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	// For current tab window.c does not bother to set or update tp_firstwin
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tabObject->tab == curtab)
	    ಹಿಂತಿರುಗಿ firstwin;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ tabObject->tab->tp_firstwin;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ firstwin;
}

// Use the same order as in the WindowAttr() function.
ಸ್ಥಿರ ಅಕ್ಷರ *ಕಿಟಕಿ_Attrs[] = {
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
    ಖಾಲಿ
};

    ಸ್ಥಿರ PyObject *
ಕಿಟಕಿ_Dir(PyObject *self, PyObject *ವಾದಗಳು ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ ObjectDir(self, ಕಿಟಕಿ_Attrs);
}

    ಸ್ಥಿರ PyObject *
ಕಿಟಕಿ_Attr_Valid(ಕಿಟಕಿ_Object *self, ಅಕ್ಷರ *name)
{
    PyObject	*ret;

    ಆದರೆ (strcmp(name, "valid") != 0)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ret = ((self->win == INVALID_ಕಿಟಕಿ_ಮೌಲ್ಯ) ? Py_False : Py_True);
    Py_INCREF(ret);
    ಹಿಂತಿರುಗಿ ret;
}

    ಸ್ಥಿರ PyObject *
ಕಿಟಕಿ_Attr(ಕಿಟಕಿ_Object *self, ಅಕ್ಷರ *name)
{
    ಆದರೆ (strcmp(name, "buffer") == 0)
	ಹಿಂತಿರುಗಿ (PyObject *)ಬಫರ್_New(self->win->w_ಬಫರ್);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(name, "cursor") == 0)
    {
	pos_T *pos = &self->win->w_cursor;

	ಹಿಂತಿರುಗಿ Py_Build_ಮೌಲ್ಯ("(ll)", (ದೀರ್ಘ)(pos->lnum), (ದೀರ್ಘ)(pos->ಲಂಬಸಾಲು));
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(name, "height") == 0)
	ಹಿಂತಿರುಗಿ PyLong_FromLong((ದೀರ್ಘ)(self->win->w_ಎತ್ತರ));
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(name, "row") == 0)
	ಹಿಂತಿರುಗಿ PyLong_FromLong((ದೀರ್ಘ)(self->win->w_winrow));
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(name, "width") == 0)
	ಹಿಂತಿರುಗಿ PyLong_FromLong((ದೀರ್ಘ)(self->win->w_ಅಗಲ));
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(name, "col") == 0)
	ಹಿಂತಿರುಗಿ PyLong_FromLong((ದೀರ್ಘ)(self->win->w_wincol));
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(name, "vars") == 0)
	ಹಿಂತಿರುಗಿ NEW_DICTIONARY(self->win->w_vars);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(name, "options") == 0)
	ಹಿಂತಿರುಗಿ OptionsNew(SREQ_WIN, self->win, Check_ಕಿಟಕಿ_Cb,
			(PyObject *) self);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(name, "number") == 0)
    {
	ಆದರೆ (Check_Tab_ಪುಟ(self->tabObject))
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	ಹಿಂತಿರುಗಿ PyLong_FromLong((ದೀರ್ಘ)
		get_win_ಸಂಖ್ಯೆ(self->win, get_firstwin(self->tabObject)));
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(name, "tabpage") == 0)
    {
	Py_INCREF((PyObject *)self->tabObject);
	ಹಿಂತಿರುಗಿ (PyObject *)(self->tabObject);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(name, "__members__") == 0)
	ಹಿಂತಿರುಗಿ ObjectDir(ಖಾಲಿ, ಕಿಟಕಿ_Attrs);
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಕಿಟಕಿ_Setattr(PyObject *self_obj, ಅಕ್ಷರ *name, PyObject *valObject)
{
    ಕಿಟಕಿ_Object *self = (ಕಿಟಕಿ_Object*)self_obj;
    ಆದರೆ (Check_ಕಿಟಕಿ(self))
	ಹಿಂತಿರುಗಿ -1;

    ಆದರೆ (strcmp(name, "buffer") == 0)
    {
	Py_Err_ಗಣ_STRING(PyExc_TypeError, N_("readonly attribute: buffer"));
	ಹಿಂತಿರುಗಿ -1;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(name, "cursor") == 0)
    {
	ದೀರ್ಘ lnum;
	ದೀರ್ಘ ಲಂಬಸಾಲು;

	ಆದರೆ (!PyArg_Parse(valObject, "(ll)", &lnum, &ಲಂಬಸಾಲು))
	    ಹಿಂತಿರುಗಿ -1;

	ಆದರೆ (lnum <= 0 || lnum > self->win->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	{
	    Py_Err_ಗಣ_VIM(N_("cursor position outside buffer"));
	    ಹಿಂತಿರುಗಿ -1;
	}

	// Check for keyboard interrupts
	ಆದರೆ (Vim_Check_ಅಡಚಣೆ())
	    ಹಿಂತಿರುಗಿ -1;

	self->win->w_cursor.lnum = lnum;
	self->win->w_cursor.ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;
	self->win->w_ಗಣ_curswant = TRUE;
	self->win->w_cursor.coladd = 0;
	// When column is out of range silently correct it.
	check_cursor_ಲಂಬಸಾಲು_win(self->win);

	update_screen(UPD_VALID);
	ಹಿಂತಿರುಗಿ 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(name, "height") == 0)
    {
	ದೀರ್ಘ	ಎತ್ತರ;
	win_T	*savewin;

	ಆದರೆ (ಸಂಖ್ಯೆ_ಗಮ್ಯ_Long(valObject, &ಎತ್ತರ, ಸಂಖ್ಯೆ_INT|ಸಂಖ್ಯೆ_UNSIGNED))
	    ಹಿಂತಿರುಗಿ -1;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	need_mouse_correct = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
	savewin = curwin;
	curwin = self->win;
	curbuf = curwin->w_ಬಫರ್;

	Vim_Try_ಆರಂಭ();
	win_setheight((ಪೂರ್ಣಾಂಕ) ಎತ್ತರ);
	curwin = savewin;
	curbuf = curwin->w_ಬಫರ್;
	ಆದರೆ (Vim_Try_ಕೊನೆ())
	    ಹಿಂತಿರುಗಿ -1;

	ಹಿಂತಿರುಗಿ 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(name, "width") == 0)
    {
	ದೀರ್ಘ	ಅಗಲ;
	win_T	*savewin;

	ಆದರೆ (ಸಂಖ್ಯೆ_ಗಮ್ಯ_Long(valObject, &ಅಗಲ, ಸಂಖ್ಯೆ_INT|ಸಂಖ್ಯೆ_UNSIGNED))
	    ಹಿಂತಿರುಗಿ -1;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	need_mouse_correct = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
	savewin = curwin;
	curwin = self->win;
	curbuf = curwin->w_ಬಫರ್;

	Vim_Try_ಆರಂಭ();
	win_setwidth((ಪೂರ್ಣಾಂಕ) ಅಗಲ);
	curwin = savewin;
	curbuf = curwin->w_ಬಫರ್;
	ಆದರೆ (Vim_Try_ಕೊನೆ())
	    ಹಿಂತಿರುಗಿ -1;

	ಹಿಂತಿರುಗಿ 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	Py_Err_ಗಣ_String(PyExc_AttributeError, name);
	ಹಿಂತಿರುಗಿ -1;
    }
}

    ಸ್ಥಿರ PyObject *
ಕಿಟಕಿ_Repr(PyObject *self_obj)
{
    ಕಿಟಕಿ_Object *self = (ಕಿಟಕಿ_Object*)self_obj;
    ಆದರೆ (self->win == INVALID_ಕಿಟಕಿ_ಮೌಲ್ಯ)
	ಹಿಂತಿರುಗಿ PyString_FromFormat("<window object (deleted) at %p>", (ಶೂನ್ಯ *)self);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪೂರ್ಣಾಂಕ	w = get_win_ಸಂಖ್ಯೆ(self->win, firstwin);

	ಆದರೆ (w == 0)
	    ಹಿಂತಿರುಗಿ PyString_FromFormat("<window object (unknown) at %p>",
								      (ಶೂನ್ಯ *)self);
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ PyString_FromFormat("<window %d>", w - 1);
    }
}

ಸ್ಥಿರ ರಚನೆ Py_ವಿಧಾನ_Def ಕಿಟಕಿ_Methods[] = {
    // name,	    function,			calling,	documentation
    {"__dir__",	    (PyCFunction)ಕಿಟಕಿ_Dir,	METH_NOARGS,	""},
    { ಖಾಲಿ,	    ಖಾಲಿ,			0,		ಖಾಲಿ}
};

/*
 * Window list object
 */

DEFINE_PY_TYPE_OBJECT(WinListType);
ಸ್ಥಿರ PySequenceMethods WinListAsSeq;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    Py_Object_ತಲೆ
    Tab_ಪುಟ_Object	*tabObject;
} WinListObject;

ಸ್ಥಿರ WinListObject The_ಕಿಟಕಿ_List =
{
    Py_Object_ತಲೆ_INIT_TYPE(WinListType)
    ಖಾಲಿ
};

    ಸ್ಥಿರ PyObject *
WinListNew(Tab_ಪುಟ_Object *tabObject)
{
    WinListObject	*self;

    self = PyObject_NEW(WinListObject, WinListTypePtr);
    self->tabObject = tabObject;
    Py_INCREF((PyObject *)tabObject);

    ಹಿಂತಿರುಗಿ (PyObject *)(self);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
WinListDestructor(WinListObject *self)
{
    Tab_ಪುಟ_Object	*tabObject = self->tabObject;

    ಆದರೆ (tabObject)
    {
	Py_DECREF((PyObject *)(tabObject));
    }

    DESTRUCTOR_ಮುಕ್ತಾಯ(self);
}

    ಸ್ಥಿರ PyInt
Win_List_ಉದ್ದಳತೆ(WinListObject *self)
{
    win_T	*w;
    PyInt	n = 0;

    ಆದರೆ (!(w = get_firstwin(self->tabObject)))
	ಹಿಂತಿರುಗಿ -1;

    ಆಗ (w != ಖಾಲಿ)
    {
	++n;
	w = W_ಮುಂದಿನ(w);
    }

    ಹಿಂತಿರುಗಿ n;
}

    ಸ್ಥಿರ PyObject *
Win_List_ಅಂಶ(WinListObject *self, PyInt n)
{
    win_T *w;

    ಆದರೆ (!(w = get_firstwin(self->tabObject)))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಗಾಗಿ (; w != ಖಾಲಿ; w = W_ಮುಂದಿನ(w), --n)
	ಆದರೆ (n == 0)
	    ಹಿಂತಿರುಗಿ ಕಿಟಕಿ_New(w, self->tabObject? self->tabObject->tab: curtab);

    Py_Err_ಗಣ_STRING(Py_Exc_ಸೂಚ್ಯಂಕ_Error, N_("no such window"));
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
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
    ಸ್ಥಿರ ಅಕ್ಷರ *
String_ಗಮ್ಯ_ಸಾಲು(PyObject *obj)
{
    ಅಕ್ಷರ	*str;
    ಅಕ್ಷರ	*save;
    PyObject	*ಬೈಟುಗಳು = ಖಾಲಿ;
    Py_ssize_t	len = 0;
    PyInt	i;
    ಅಕ್ಷರ	*p;

    ಆದರೆ (Py_ಬೈಟುಗಳು_Check(obj))
    {
	ಆದರೆ (Py_ಬೈಟುಗಳು_As_String_And_ಗಾತ್ರ(obj, &str, &len) == -1
		|| str == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (PyUnicode_Check(obj))
    {
	ಆದರೆ (!(ಬೈಟುಗಳು = PyUnicode_AsEncodedString(obj, ENC_OPT,
							   ERRORS_ENCODE_ARG)))
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	ಆದರೆ (Py_ಬೈಟುಗಳು_As_String_And_ಗಾತ್ರ(ಬೈಟುಗಳು, &str, &len) == -1
		|| str == ಖಾಲಿ)
	{
	    Py_DECREF(ಬೈಟುಗಳು);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
#ಆದರೆ PY_MAJOR_ಆವೃತ್ತಿ < 3
	PyErr_FORMAT_TYPE(
		N_("expected str() or unicode() instance, but got %s"),
		obj);
#ಇಲ್ಲದಿದ್ದರೆ
	PyErr_FORMAT_TYPE(
		N_("expected bytes() or str() instance, but got %s"),
		obj);
#ಆದರೆ_ಅಂತ್ಯ
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    /*
     * Error checking: String must not contain newlines, as we
     * are replacing a single line, and we must replace it with
     * a single line.
     * A trailing newline is removed, so that append(f.readlines()) works.
     */
    p = memchr(str, '\n', len);
    ಆದರೆ (p != ಖಾಲಿ)
    {
	ಆದರೆ (p == str + len - 1)
	    --len;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    Py_Err_ಗಣ_VIM(N_("string cannot contain newlines"));
	    Py_XDECREF(ಬೈಟುಗಳು);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
    }

    /*
     * Create a copy of the string, with internal nulls replaced by
     * newline characters, as is the Vim convention.
     */
    save = alloc(len+1);
    ಆದರೆ (save == ಖಾಲಿ)
    {
	Py_Err_No_ಸ್ಮರಣೆ();
	Py_XDECREF(ಬೈಟುಗಳು);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಗಾಗಿ (i = 0; i < len; ++i)
    {
	ಆದರೆ (str[i] == '\0')
	    save[i] = '\n';
	ಇಲ್ಲದಿದ್ದರೆ
	    save[i] = str[i];
    }

    save[i] = '\0';
    Py_XDECREF(ಬೈಟುಗಳು);  // Python 2 does nothing here

    ಹಿಂತಿರುಗಿ save;
}

/*
 * Get a line from the specified buffer. The line number is
 * in Vim format (1-based). The line is returned as a Python
 * string object.
 */
    ಸ್ಥಿರ PyObject *
Get_ಬಫರ್_ಸಾಲು(buf_T *buf, PyInt n)
{
    ಹಿಂತಿರುಗಿ ಸಾಲು_ಗಮ್ಯ_String((ಅಕ್ಷರ *)ml_get_buf(buf, (linenr_T)n, FALSE));
}


/*
 * Get a list of lines from the specified buffer. The line numbers
 * are in Vim format (1-based). The range is from lo up to, but not
 * including, hi. The list is returned as a Python list of string objects.
 */
    ಸ್ಥಿರ PyObject *
Get_ಬಫರ್_ಸಾಲು_List(buf_T *buf, PyInt lo, PyInt hi)
{
    PyInt	i;
    PyInt	n = hi - lo;
    PyObject	*list = PyList_New(n);

    ಆದರೆ (list == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಗಾಗಿ (i = 0; i < n; ++i)
    {
	linenr_T	lnum = (linenr_T)(lo + i);
	ಅಕ್ಷರ		*ಪಠ್ಯ;
	PyObject	*string;

	ಆದರೆ (lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    ಪಠ್ಯ = "";
	ಇಲ್ಲದಿದ್ದರೆ
	    ಪಠ್ಯ = (ಅಕ್ಷರ *)ml_get_buf(buf, lnum, FALSE);
	string = ಸಾಲು_ಗಮ್ಯ_String(ಪಠ್ಯ);
	ಆದರೆ (string == ಖಾಲಿ)
	{
	    Py_DECREF(list);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

	Py_List_ಗಣ_ಅಂಶ(list, i, string);
    }

    // The ownership of the Python list is passed to the caller (ie,
    // the caller should Py_DECREF() the object when it is finished
    // with it).

    ಹಿಂತಿರುಗಿ list;
}

/*
 * Check if deleting lines made the cursor position invalid.
 * Changed the lines from "lo" to "hi" and added "extra" lines (negative if
 * deleted).
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
py_fix_cursor(linenr_T lo, linenr_T hi, linenr_T extra)
{
    ಆದರೆ (curwin->w_cursor.lnum >= lo)
    {
	// Adjust the cursor position if it's in/after the changed
	// lines.
	ಆದರೆ (curwin->w_cursor.lnum >= hi)
	{
	    curwin->w_cursor.lnum += extra;
	    check_cursor_ಲಂಬಸಾಲು();
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (extra < 0)
	{
	    curwin->w_cursor.lnum = lo;
	    check_cursor();
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    check_cursor_ಲಂಬಸಾಲು();
	ಬದಲಾಯಿತು_cline_bef_curs();
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಗಣ_ಬಫರ್_ಸಾಲು_2(buf_T *buf, PyInt n, PyObject *ಸಾಲು, PyInt *len_change)
{
    bufref_T	save_curbuf = {ಖಾಲಿ, 0, 0};
    switchwin_T	switchwin;

    // First of all, we check the type of the supplied Python object.
    // There are three cases:
    //	  1. NULL, or None - this is a deletion.
    //	  2. A string	   - this is a replacement.
    //	  3. Anything else - this is an error.
    ಆದರೆ (ಸಾಲು == Py_None || ಸಾಲು == ಖಾಲಿ)
    {
	PyErr_Clear();
	switchwin.sw_curwin = ಖಾಲಿ;
	switch_ಗಮ್ಯ_win_for_buf(buf, &switchwin, &save_curbuf);

	Vim_Try_ಆರಂಭ();

	ಆದರೆ (u_savedel((linenr_T)n, 1L) == FAIL)
	    RAISE_UNDO_FAIL;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ml_delete((linenr_T)n) == FAIL)
	    RAISE_DELETE_ಸಾಲು_FAIL;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (buf == curbuf && (switchwin.sw_curwin != ಖಾಲಿ
					   || save_curbuf.br_buf == ಖಾಲಿ))
		// Using an existing window for the buffer, adjust the cursor
		// position.
		py_fix_cursor((linenr_T)n, (linenr_T)n + 1, (linenr_T)-1);
	    ಆದರೆ (save_curbuf.br_buf == ಖಾಲಿ)
		// Only adjust marks if we managed to switch to a window that
		// holds the buffer, otherwise line numbers will be invalid.
		deleted_ಸಾಲುಗಳು_mark((linenr_T)n, 1L);
	}

	restore_win_for_buf(&switchwin, &save_curbuf);

	ಆದರೆ (Vim_Try_ಕೊನೆ())
	    ಹಿಂತಿರುಗಿ FAIL;

	ಆದರೆ (len_change)
	    *len_change = -1;

	ಹಿಂತಿರುಗಿ OK;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (Py_ಬೈಟುಗಳು_Check(ಸಾಲು) || PyUnicode_Check(ಸಾಲು))
    {
	ಅಕ್ಷರ	*save = String_ಗಮ್ಯ_ಸಾಲು(ಸಾಲು);

	ಆದರೆ (save == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;

	Vim_Try_ಆರಂಭ();

	// We do not need to free "save" if ml_replace() consumes it.
	PyErr_Clear();
	switch_ಗಮ್ಯ_win_for_buf(buf, &switchwin, &save_curbuf);

	ಆದರೆ (u_savesub((linenr_T)n) == FAIL)
	{
	    RAISE_UNDO_FAIL;
	    vim_free(save);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ml_replace((linenr_T)n, (char_u *)save, FALSE) == FAIL)
	{
	    RAISE_REPLACE_ಸಾಲು_FAIL;
	    vim_free(save);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಬದಲಾಯಿತು_ಬೈಟುಗಳು((linenr_T)n, 0);

	restore_win_for_buf(&switchwin, &save_curbuf);

	// Check that the cursor is not beyond the end of the line now.
	ಆದರೆ (buf == curbuf)
	    check_cursor_ಲಂಬಸಾಲು();

	ಆದರೆ (Vim_Try_ಕೊನೆ())
	    ಹಿಂತಿರುಗಿ FAIL;

	ಆದರೆ (len_change)
	    *len_change = 0;

	ಹಿಂತಿರುಗಿ OK;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	Py_Err_Bad_ವಾದ();
	ಹಿಂತಿರುಗಿ FAIL;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಗಣ_ಬಫರ್_ಸಾಲು_List(
	buf_T *buf,
	PyInt lo,
	PyInt hi,
	PyObject *list,
	PyInt *len_change)
{
    bufref_T	save_curbuf = {ಖಾಲಿ, 0, 0};
    switchwin_T	switchwin;

    // First of all, we check the type of the supplied Python object.
    // There are three cases:
    //	  1. NULL, or None - this is a deletion.
    //	  2. A list	   - this is a replacement.
    //	  3. Anything else - this is an error.
    ಆದರೆ (list == Py_None || list == ಖಾಲಿ)
    {
	PyInt	i;
	PyInt	n = (ಪೂರ್ಣಾಂಕ)(hi - lo);

	PyErr_Clear();
	Vim_Try_ಆರಂಭ();
	switchwin.sw_curwin = ಖಾಲಿ;
	switch_ಗಮ್ಯ_win_for_buf(buf, &switchwin, &save_curbuf);

	ಆದರೆ (u_savedel((linenr_T)lo, (ದೀರ್ಘ)n) == FAIL)
	    RAISE_UNDO_FAIL;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಗಾಗಿ (i = 0; i < n; ++i)
	    {
		ಆದರೆ (ml_delete((linenr_T)lo) == FAIL)
		{
		    RAISE_DELETE_ಸಾಲು_FAIL;
		    ನಿಲ್ಲಿಸು;
		}
	    }
	    ಆದರೆ (buf == curbuf && (switchwin.sw_curwin != ಖಾಲಿ
					       || save_curbuf.br_buf == ಖಾಲಿ))
		// Using an existing window for the buffer, adjust the cursor
		// position.
		py_fix_cursor((linenr_T)lo, (linenr_T)hi, (linenr_T)-n);
	    ಆದರೆ (save_curbuf.br_buf == ಖಾಲಿ)
		// Only adjust marks if we managed to switch to a window that
		// holds the buffer, otherwise line numbers will be invalid.
		deleted_ಸಾಲುಗಳು_mark((linenr_T)lo, (ದೀರ್ಘ)i);
	}

	restore_win_for_buf(&switchwin, &save_curbuf);

	ಆದರೆ (Vim_Try_ಕೊನೆ())
	    ಹಿಂತಿರುಗಿ FAIL;

	ಆದರೆ (len_change)
	    *len_change = -n;

	ಹಿಂತಿರುಗಿ OK;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (PyList_Check(list))
    {
	PyInt	i;
	PyInt	new_len = Py_List_ಗಾತ್ರ(list);
	PyInt	old_len = hi - lo;
	PyInt	extra = 0;	// lines added to text, can be negative
	ಅಕ್ಷರ	**array;

	ಆದರೆ (new_len == 0)	// avoid allocating zero bytes
	    array = ಖಾಲಿ;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    array = PyMem_New(ಅಕ್ಷರ *, new_len);
	    ಆದರೆ (array == ಖಾಲಿ)
	    {
		Py_Err_No_ಸ್ಮರಣೆ();
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	}

	ಗಾಗಿ (i = 0; i < new_len; ++i)
	{
	    PyObject *ಸಾಲು;

	    ಆದರೆ (!(ಸಾಲು = Py_List_Get_ಅಂಶ(list, i)) ||
		!(array[i] = String_ಗಮ್ಯ_ಸಾಲು(ಸಾಲು)))
	    {
		ಆಗ (i)
		    vim_free(array[--i]);
		PyMem_Free(array);
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	}

	Vim_Try_ಆರಂಭ();
	PyErr_Clear();

	// START of region without "return".  Must call restore_buffer()!
	switchwin.sw_curwin = ಖಾಲಿ;
	switch_ಗಮ್ಯ_win_for_buf(buf, &switchwin, &save_curbuf);

	ಆದರೆ (u_save((linenr_T)(lo-1), (linenr_T)hi) == FAIL)
	    RAISE_UNDO_FAIL;

	// If the size of the range is reducing (ie, new_len < old_len) we
	// need to delete some old_len. We do this at the start, by
	// repeatedly deleting line "lo".
	ಆದರೆ (!PyErr_Occurred())
	{
	    ಗಾಗಿ (i = 0; i < old_len - new_len; ++i)
		ಆದರೆ (ml_delete((linenr_T)lo) == FAIL)
		{
		    RAISE_DELETE_ಸಾಲು_FAIL;
		    ನಿಲ್ಲಿಸು;
		}
	    extra -= i;
	}

	// For as long as possible, replace the existing old_len with the
	// new old_len. This is a more efficient operation, as it requires
	// less memory allocation and freeing.
	ಆದರೆ (!PyErr_Occurred())
	{
	    ಗಾಗಿ (i = 0; i < old_len && i < new_len; ++i)
		ಆದರೆ (ml_replace((linenr_T)(lo+i), (char_u *)array[i], FALSE)
								      == FAIL)
		{
		    RAISE_REPLACE_ಸಾಲು_FAIL;
		    ನಿಲ್ಲಿಸು;
		}
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    i = 0;

	// Now we may need to insert the remaining new old_len. If we do, we
	// must free the strings as we finish with them (we can't pass the
	// responsibility to Vim in this case).
	ಆದರೆ (!PyErr_Occurred())
	{
	    ಆಗ (i < new_len)
	    {
		ಆದರೆ (ml_append((linenr_T)(lo + i - 1),
					(char_u *)array[i], 0, FALSE) == FAIL)
		{
		    RAISE_ಸೇರಿಸು_ಸಾಲು_FAIL;
		    ನಿಲ್ಲಿಸು;
		}
		vim_free(array[i]);
		++i;
		++extra;
	    }
	}

	// Free any left-over old_len, as a result of an error
	ಆಗ (i < new_len)
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
	ಆದರೆ (save_curbuf.br_buf == ಖಾಲಿ)
	{
	    mark_adjust((linenr_T)lo, (linenr_T)(hi - 1),
						  (ದೀರ್ಘ)MAXLNUM, (ದೀರ್ಘ)extra);
	    ಬದಲಾಯಿತು_ಸಾಲುಗಳು((linenr_T)lo, 0, (linenr_T)hi, (ದೀರ್ಘ)extra);
	}

	ಆದರೆ (buf == curbuf && (switchwin.sw_curwin != ಖಾಲಿ
					   || save_curbuf.br_buf == ಖಾಲಿ))
	    // Using an existing window for the buffer, adjust the cursor
	    // position.
	    py_fix_cursor((linenr_T)lo, (linenr_T)hi, (linenr_T)extra);

	// END of region without "return".
	restore_win_for_buf(&switchwin, &save_curbuf);

	ಆದರೆ (Vim_Try_ಕೊನೆ())
	    ಹಿಂತಿರುಗಿ FAIL;

	ಆದರೆ (len_change)
	    *len_change = new_len - old_len;

	ಹಿಂತಿರುಗಿ OK;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	Py_Err_Bad_ವಾದ();
	ಹಿಂತಿರುಗಿ FAIL;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಸೇರಿಸು_ಬಫರ್_ಸಾಲುಗಳು(buf_T *buf, PyInt n, PyObject *ಸಾಲುಗಳು, PyInt *len_change)
{
    bufref_T	save_curbuf = {ಖಾಲಿ, 0, 0};
    switchwin_T	switchwin;

    // First of all, we check the type of the supplied Python object.
    // It must be a string or a list, or the call is in error.
    ಆದರೆ (Py_ಬೈಟುಗಳು_Check(ಸಾಲುಗಳು) || PyUnicode_Check(ಸಾಲುಗಳು))
    {
	ಅಕ್ಷರ		*str = String_ಗಮ್ಯ_ಸಾಲು(ಸಾಲುಗಳು);

	ಆದರೆ (str == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;

	PyErr_Clear();
	Vim_Try_ಆರಂಭ();
	switch_ಗಮ್ಯ_win_for_buf(buf, &switchwin, &save_curbuf);

	ಆದರೆ (u_save((linenr_T)n, (linenr_T)(n + 1)) == FAIL)
	    RAISE_UNDO_FAIL;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ml_append((linenr_T)n, (char_u *)str, 0, FALSE) == FAIL)
	    RAISE_ಸೇರಿಸು_ಸಾಲು_FAIL;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (save_curbuf.br_buf == ಖಾಲಿ)
	    // Only adjust marks if we managed to switch to a window that
	    // holds the buffer, otherwise line numbers will be invalid.
	    appended_ಸಾಲುಗಳು_mark((linenr_T)n, 1L);

	vim_free(str);
	restore_win_for_buf(&switchwin, &save_curbuf);
	update_screen(UPD_VALID);

	ಆದರೆ (Vim_Try_ಕೊನೆ())
	    ಹಿಂತಿರುಗಿ FAIL;

	ಆದರೆ (len_change)
	    *len_change = 1;

	ಹಿಂತಿರುಗಿ OK;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (PyList_Check(ಸಾಲುಗಳು))
    {
	PyInt	i;
	PyInt	ಗಾತ್ರ_2 = Py_List_ಗಾತ್ರ(ಸಾಲುಗಳು);
	ಅಕ್ಷರ	**array;

	array = PyMem_New(ಅಕ್ಷರ *, ಗಾತ್ರ_2);
	ಆದರೆ (array == ಖಾಲಿ)
	{
	    Py_Err_No_ಸ್ಮರಣೆ();
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	ಗಾಗಿ (i = 0; i < ಗಾತ್ರ_2; ++i)
	{
	    PyObject *ಸಾಲು;

	    ಆದರೆ (!(ಸಾಲು = Py_List_Get_ಅಂಶ(ಸಾಲುಗಳು, i)) ||
		!(array[i] = String_ಗಮ್ಯ_ಸಾಲು(ಸಾಲು)))
	    {
		ಆಗ (i)
		    vim_free(array[--i]);
		PyMem_Free(array);
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	}

	PyErr_Clear();
	Vim_Try_ಆರಂಭ();
	switch_ಗಮ್ಯ_win_for_buf(buf, &switchwin, &save_curbuf);

	ಆದರೆ (u_save((linenr_T)n, (linenr_T)(n + 1)) == FAIL)
	    RAISE_UNDO_FAIL;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಗಾಗಿ (i = 0; i < ಗಾತ್ರ_2; ++i)
	    {
		ಆದರೆ (ml_append((linenr_T)(n + i),
					(char_u *)array[i], 0, FALSE) == FAIL)
		{
		    RAISE_ಸೇರಿಸು_ಸಾಲು_FAIL;

		    // Free the rest of the lines
		    ಆಗ (i < ಗಾತ್ರ_2)
			vim_free(array[i++]);

		    ನಿಲ್ಲಿಸು;
		}
		vim_free(array[i]);
	    }
	    ಆದರೆ (i > 0 && save_curbuf.br_buf == ಖಾಲಿ)
		// Only adjust marks if we managed to switch to a window that
		// holds the buffer, otherwise line numbers will be invalid.
		appended_ಸಾಲುಗಳು_mark((linenr_T)n, (ದೀರ್ಘ)i);
	}

	// Free the array of lines. All of its contents have now
	// been freed.
	PyMem_Free(array);
	restore_win_for_buf(&switchwin, &save_curbuf);

	update_screen(UPD_VALID);

	ಆದರೆ (Vim_Try_ಕೊನೆ())
	    ಹಿಂತಿರುಗಿ FAIL;

	ಆದರೆ (len_change)
	    *len_change = ಗಾತ್ರ_2;

	ಹಿಂತಿರುಗಿ OK;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	Py_Err_Bad_ವಾದ();
	ಹಿಂತಿರುಗಿ FAIL;
    }
}

/*
 * Common routines for buffers and line ranges
 * -------------------------------------------
 */

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    Py_Object_ತಲೆ
    buf_T *buf;
} ಬಫರ್_Object;

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
Check_ಬಫರ್(ಬಫರ್_Object *self)
{
    ಆದರೆ (self->buf == INVALID_ಬಫರ್_ಮೌಲ್ಯ)
    {
	Py_Err_ಗಣ_VIM(N_("attempt to refer to deleted buffer"));
	ಹಿಂತಿರುಗಿ -1;
    }

    ಹಿಂತಿರುಗಿ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
Check_ಬಫರ್_Cb(ಶೂನ್ಯ *self)
{
    ಹಿಂತಿರುಗಿ Check_ಬಫರ್((ಬಫರ್_Object*)self);
}

    ಸ್ಥಿರ PyObject *
RBItem(ಬಫರ್_Object *self, PyInt n, PyInt ಆರಂಭ, PyInt ಕೊನೆ)
{
    ಆದರೆ (Check_ಬಫರ್(self))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (ಕೊನೆ == -1)
	ಕೊನೆ = self->buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

    ಆದರೆ (n < 0)
	n += ಕೊನೆ - ಆರಂಭ + 1;

    ಆದರೆ (n < 0 || n > ಕೊನೆ - ಆರಂಭ)
    {
	Py_Err_ಗಣ_STRING(Py_Exc_ಸೂಚ್ಯಂಕ_Error, N_("line number out of range"));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಹಿಂತಿರುಗಿ Get_ಬಫರ್_ಸಾಲು(self->buf, n+ಆರಂಭ);
}

    ಸ್ಥಿರ PyObject *
RBSlice(ಬಫರ್_Object *self, PyInt lo, PyInt hi, PyInt ಆರಂಭ, PyInt ಕೊನೆ)
{
    PyInt ಗಾತ್ರ_2;

    ಆದರೆ (Check_ಬಫರ್(self))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (ಕೊನೆ == -1)
	ಕೊನೆ = self->buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

    ಗಾತ್ರ_2 = ಕೊನೆ - ಆರಂಭ + 1;

    ಆದರೆ (lo < 0)
	lo = 0;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lo > ಗಾತ್ರ_2)
	lo = ಗಾತ್ರ_2;
    ಆದರೆ (hi < 0)
	hi = 0;
    ಆದರೆ (hi < lo)
	hi = lo;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (hi > ಗಾತ್ರ_2)
	hi = ಗಾತ್ರ_2;

    ಹಿಂತಿರುಗಿ Get_ಬಫರ್_ಸಾಲು_List(self->buf, lo+ಆರಂಭ, hi+ಆರಂಭ);
}

    ಸ್ಥಿರ PyInt
RBAs_ಅಂಶ(
	ಬಫರ್_Object *self,
	PyInt n,
	PyObject *valObject,
	PyInt ಆರಂಭ,
	PyInt ಕೊನೆ,
	PyInt *new_ಕೊನೆ)
{
    PyInt len_change;

    ಆದರೆ (Check_ಬಫರ್(self))
	ಹಿಂತಿರುಗಿ -1;

    ಆದರೆ (ಕೊನೆ == -1)
	ಕೊನೆ = self->buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

    ಆದರೆ (n < 0)
	n += ಕೊನೆ - ಆರಂಭ + 1;

    ಆದರೆ (n < 0 || n > ಕೊನೆ - ಆರಂಭ)
    {
	Py_Err_ಗಣ_STRING(Py_Exc_ಸೂಚ್ಯಂಕ_Error, N_("line number out of range"));
	ಹಿಂತಿರುಗಿ -1;
    }

    ಆದರೆ (ಗಣ_ಬಫರ್_ಸಾಲು_2(self->buf, n+ಆರಂಭ, valObject, &len_change) == FAIL)
	ಹಿಂತಿರುಗಿ -1;

    ಆದರೆ (new_ಕೊನೆ)
	*new_ಕೊನೆ = ಕೊನೆ + len_change;

    ಹಿಂತಿರುಗಿ 0;
}

    ಸ್ಥಿರ PyInt
RBAs_ತುಂಡು(
	ಬಫರ್_Object *self,
	PyInt lo,
	PyInt hi,
	PyObject *valObject,
	PyInt ಆರಂಭ,
	PyInt ಕೊನೆ,
	PyInt *new_ಕೊನೆ)
{
    PyInt ಗಾತ್ರ_2;
    PyInt len_change;

    // Self must be a valid buffer
    ಆದರೆ (Check_ಬಫರ್(self))
	ಹಿಂತಿರುಗಿ -1;

    ಆದರೆ (ಕೊನೆ == -1)
	ಕೊನೆ = self->buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

    // Sort out the slice range
    ಗಾತ್ರ_2 = ಕೊನೆ - ಆರಂಭ + 1;

    ಆದರೆ (lo < 0)
	lo = 0;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lo > ಗಾತ್ರ_2)
	lo = ಗಾತ್ರ_2;
    ಆದರೆ (hi < 0)
	hi = 0;
    ಆದರೆ (hi < lo)
	hi = lo;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (hi > ಗಾತ್ರ_2)
	hi = ಗಾತ್ರ_2;

    ಆದರೆ (ಗಣ_ಬಫರ್_ಸಾಲು_List(self->buf, lo + ಆರಂಭ, hi + ಆರಂಭ,
						valObject, &len_change) == FAIL)
	ಹಿಂತಿರುಗಿ -1;

    ಆದರೆ (new_ಕೊನೆ)
	*new_ಕೊನೆ = ಕೊನೆ + len_change;

    ಹಿಂತಿರುಗಿ 0;
}


    ಸ್ಥಿರ PyObject *
RBAppend(
	ಬಫರ್_Object *self,
	PyObject *ವಾದಗಳು,
	PyInt ಆರಂಭ,
	PyInt ಕೊನೆ,
	PyInt *new_ಕೊನೆ)
{
    PyObject *ಸಾಲುಗಳು;
    PyInt len_change;
    PyInt max;
    PyInt n;

    ಆದರೆ (Check_ಬಫರ್(self))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (ಕೊನೆ == -1)
	ಕೊನೆ = self->buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

    max = n = ಕೊನೆ - ಆರಂಭ + 1;

    ಆದರೆ (!PyArg_ParseTuple(ವಾದಗಳು, "O|n", &ಸಾಲುಗಳು, &n))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (n < 0 || n > max)
    {
	Py_Err_ಗಣ_STRING(Py_Exc_ಸೂಚ್ಯಂಕ_Error, N_("line number out of range"));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಆದರೆ (ಸೇರಿಸು_ಬಫರ್_ಸಾಲುಗಳು(self->buf, n + ಆರಂಭ - 1, ಸಾಲುಗಳು, &len_change) == FAIL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (new_ಕೊನೆ)
	*new_ಕೊನೆ = ಕೊನೆ + len_change;

    Py_INCREF(Py_None);
    ಹಿಂತಿರುಗಿ Py_None;
}

// Range object

DEFINE_PY_TYPE_OBJECT(RangeType);
ಸ್ಥಿರ PySequenceMethods RangeAsSeq;
ಸ್ಥಿರ PyMappingMethods RangeAsMapping;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    Py_Object_ತಲೆ
    ಬಫರ್_Object *buf;
    PyInt ಆರಂಭ;
    PyInt ಕೊನೆ;
} RangeObject;

    ಸ್ಥಿರ PyObject *
RangeNew(buf_T *buf, PyInt ಆರಂಭ, PyInt ಕೊನೆ)
{
    ಬಫರ್_Object *bufr;
    RangeObject *self;
    self = PyObject_GC_New(RangeObject, RangeTypePtr);
    ಆದರೆ (self == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    bufr = (ಬಫರ್_Object *)ಬಫರ್_New(buf);
    ಆದರೆ (bufr == ಖಾಲಿ)
    {
	Py_DECREF(self);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    Py_INCREF((PyObject *)bufr);

    self->buf = bufr;
    self->ಆರಂಭ = ಆರಂಭ;
    self->ಕೊನೆ = ಕೊನೆ;

    ಹಿಂತಿರುಗಿ (PyObject *)(self);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
RangeDestructor(PyObject *self_obj)
{
    RangeObject *self = (RangeObject*)self_obj;
    PyObject_GC_UnTrack((ಶೂನ್ಯ *)(self));
    Py_XDECREF(((PyObject *)(self->buf)));
    PyObject_GC_Del((ಶೂನ್ಯ *)(self));
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
RangeTraverse(PyObject *self_obj, visitproc ಭೇಟಿ, ಶೂನ್ಯ *arg)
{
    RangeObject *self = (RangeObject*)self_obj;
    Py_ಭೇಟಿ(((PyObject *)(self->buf)));
    ಹಿಂತಿರುಗಿ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
RangeClear(PyObject *self_obj)
{
    RangeObject *self = (RangeObject*)self_obj;
    Py_CLEAR(self->buf);
    ಹಿಂತಿರುಗಿ 0;
}

    ಸ್ಥಿರ PyInt
Range_ಉದ್ದಳತೆ(RangeObject *self)
{
    // HOW DO WE SIGNAL AN ERROR FROM THIS FUNCTION?
    ಆದರೆ (Check_ಬಫರ್(self->buf))
	ಹಿಂತಿರುಗಿ -1; // ???

    ಹಿಂತಿರುಗಿ (self->ಕೊನೆ - self->ಆರಂಭ + 1);
}

    ಸ್ಥಿರ PyObject *
Range_ಅಂಶ(RangeObject *self, PyInt n)
{
    ಹಿಂತಿರುಗಿ RBItem(self->buf, n, self->ಆರಂಭ, self->ಕೊನೆ);
}

    ಸ್ಥಿರ PyObject *
Range_ತುಂಡು(RangeObject *self, PyInt lo, PyInt hi)
{
    ಹಿಂತಿರುಗಿ RBSlice(self->buf, lo, hi, self->ಆರಂಭ, self->ಕೊನೆ);
}

ಸ್ಥಿರ ಅಕ್ಷರ *RangeAttrs[] = {
    "start", "end",
    ಖಾಲಿ
};

    ಸ್ಥಿರ PyObject *
RangeDir(PyObject *self, PyObject *ವಾದಗಳು ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ ObjectDir(self, RangeAttrs);
}

    ಸ್ಥಿರ PyObject *
RangeAppend(RangeObject *self, PyObject *ವಾದಗಳು)
{
    ಹಿಂತಿರುಗಿ RBAppend(self->buf, ವಾದಗಳು, self->ಆರಂಭ, self->ಕೊನೆ, &self->ಕೊನೆ);
}

    ಸ್ಥಿರ PyObject *
RangeRepr(PyObject *self_obj)
{
    RangeObject *self = (RangeObject*)self_obj;
    ಆದರೆ (self->buf->buf == INVALID_ಬಫರ್_ಮೌಲ್ಯ)
	ಹಿಂತಿರುಗಿ PyString_FromFormat("<range object (for deleted buffer) at %p>",
				    (ಶೂನ್ಯ *)self);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಅಕ್ಷರ *name = (ಅಕ್ಷರ *)self->buf->buf->b_fname;

	ಆದರೆ (name == ಖಾಲಿ)
	    name = "";

	ಹಿಂತಿರುಗಿ PyString_FromFormat("<range %s (%d:%d)>",
				    name, (ಪೂರ್ಣಾಂಕ)self->ಆರಂಭ, (ಪೂರ್ಣಾಂಕ)self->ಕೊನೆ);
    }
}

ಸ್ಥಿರ ರಚನೆ Py_ವಿಧಾನ_Def RangeMethods[] = {
    // name,	function,			calling,	documentation
    {"append",	(PyCFunction)RangeAppend,	METH_VARARGS,	"Append data to the Vim range" },
    {"__dir__",	(PyCFunction)RangeDir,		METH_NOARGS,	""},
    { ಖಾಲಿ,	ಖಾಲಿ,				0,		ಖಾಲಿ}
};

DEFINE_PY_TYPE_OBJECT(ಬಫರ್_Type);
ಸ್ಥಿರ PySequenceMethods ಬಫರ್_As_Seq;
ಸ್ಥಿರ PyMappingMethods ಬಫರ್_As_Mapping;

    ಸ್ಥಿರ PyObject *
ಬಫರ್_New(buf_T *buf)
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

    ಬಫರ್_Object *self;

    ಆದರೆ (BUF_PYTHON_REF(buf) != ಖಾಲಿ)
    {
	self = BUF_PYTHON_REF(buf);
	Py_INCREF((PyObject *)self);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	self = PyObject_NEW(ಬಫರ್_Object, ಬಫರ್_Type_Ptr);
	ಆದರೆ (self == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	self->buf = buf;
	BUF_PYTHON_REF(buf) = self;
    }

    ಹಿಂತಿರುಗಿ (PyObject *)(self);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಬಫರ್_Destructor(PyObject *self_obj)
{
    ಬಫರ್_Object *self = (ಬಫರ್_Object*)self_obj;
    ಆದರೆ (self->buf && self->buf != INVALID_ಬಫರ್_ಮೌಲ್ಯ)
	BUF_PYTHON_REF(self->buf) = ಖಾಲಿ;

    DESTRUCTOR_ಮುಕ್ತಾಯ(self);
}

    ಸ್ಥಿರ PyInt
ಬಫರ್_ಉದ್ದಳತೆ(ಬಫರ್_Object *self)
{
    // HOW DO WE SIGNAL AN ERROR FROM THIS FUNCTION?
    ಆದರೆ (Check_ಬಫರ್(self))
	ಹಿಂತಿರುಗಿ -1; // ???

    ಹಿಂತಿರುಗಿ (PyInt)(self->buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ);
}

    ಸ್ಥಿರ PyObject *
ಬಫರ್_ಅಂಶ(ಬಫರ್_Object *self, PyInt n)
{
    ಹಿಂತಿರುಗಿ RBItem(self, n, 1, -1);
}

    ಸ್ಥಿರ PyObject *
ಬಫರ್_ತುಂಡು(ಬಫರ್_Object *self, PyInt lo, PyInt hi)
{
    ಹಿಂತಿರುಗಿ RBSlice(self, lo, hi, 1, -1);
}

ಸ್ಥಿರ ಅಕ್ಷರ *ಬಫರ್_Attrs[] = {
    "name", "number", "vars", "options", "valid",
    ಖಾಲಿ
};

    ಸ್ಥಿರ PyObject *
ಬಫರ್_Dir(PyObject *self, PyObject *ವಾದಗಳು ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ ObjectDir(self, ಬಫರ್_Attrs);
}

    ಸ್ಥಿರ PyObject *
ಬಫರ್_Attr_Valid(ಬಫರ್_Object *self, ಅಕ್ಷರ *name)
{
    PyObject	*ret;

    ಆದರೆ (strcmp(name, "valid") != 0)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ret = ((self->buf == INVALID_ಬಫರ್_ಮೌಲ್ಯ) ? Py_False : Py_True);
    Py_INCREF(ret);
    ಹಿಂತಿರುಗಿ ret;
}

    ಸ್ಥಿರ PyObject *
ಬಫರ್_Attr(ಬಫರ್_Object *self, ಅಕ್ಷರ *name)
{
    ಆದರೆ (strcmp(name, "name") == 0)
	ಹಿಂತಿರುಗಿ PyString_FromString((self->buf->b_ffname == ಖಾಲಿ
				    ? "" : (ಅಕ್ಷರ *)self->buf->b_ffname));
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(name, "number") == 0)
	ಹಿಂತಿರುಗಿ Py_Build_ಮೌಲ್ಯ(Py_ssize_t_fmt, self->buf->b_fnum);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(name, "vars") == 0)
	ಹಿಂತಿರುಗಿ NEW_DICTIONARY(self->buf->b_vars);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(name, "options") == 0)
	ಹಿಂತಿರುಗಿ OptionsNew(SREQ_BUF, self->buf, Check_ಬಫರ್_Cb,
			(PyObject *) self);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(name, "__members__") == 0)
	ಹಿಂತಿರುಗಿ ObjectDir(ಖಾಲಿ, ಬಫರ್_Attrs);
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಬಫರ್_Setattr(PyObject *self_obj, ಅಕ್ಷರ *name, PyObject *valObject)
{
    ಬಫರ್_Object *self = (ಬಫರ್_Object*)self_obj;
    ಆದರೆ (Check_ಬಫರ್(self))
	ಹಿಂತಿರುಗಿ -1;

    ಆದರೆ (strcmp(name, "name") == 0)
    {
	char_u		*val;
	aco_save_T	aco;
	ಪೂರ್ಣಾಂಕ		ren_ret = OK;
	PyObject	*todecref;

	ಆದರೆ (!(val = String_ಗಮ್ಯ_Chars(valObject, &todecref)))
	    ಹಿಂತಿರುಗಿ -1;

	Vim_Try_ಆರಂಭ();
	// Using aucmd_*: autocommands will be executed by rename_buffer
	aucmd_prepbuf(&aco, self->buf);
	ಆದರೆ (curbuf == self->buf)
	{
	    ren_ret = rename_ಬಫರ್(val);
	    aucmd_restbuf(&aco);
	}
	Py_XDECREF(todecref);
	ಆದರೆ (Vim_Try_ಕೊನೆ())
	    ಹಿಂತಿರುಗಿ -1;

	ಆದರೆ (ren_ret == FAIL)
	{
	    Py_Err_ಗಣ_VIM(N_("failed to rename buffer"));
	    ಹಿಂತಿರುಗಿ -1;
	}
	ಹಿಂತಿರುಗಿ 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	Py_Err_ಗಣ_String(PyExc_AttributeError, name);
	ಹಿಂತಿರುಗಿ -1;
    }
}

    ಸ್ಥಿರ PyObject *
ಬಫರ್_Append(ಬಫರ್_Object *self, PyObject *ವಾದಗಳು)
{
    ಹಿಂತಿರುಗಿ RBAppend(self, ವಾದಗಳು, 1, -1, ಖಾಲಿ);
}

    ಸ್ಥಿರ PyObject *
ಬಫರ್_Mark(ಬಫರ್_Object *self, PyObject *pmarkObject)
{
    pos_T	*posp;
    char_u	*pmark;
    char_u	mark;
    bufref_T	savebuf;
    PyObject	*todecref;

    ಆದರೆ (Check_ಬಫರ್(self))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (!(pmark = String_ಗಮ್ಯ_Chars(pmarkObject, &todecref)))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (pmark[0] == '\0' || pmark[1] != '\0')
    {
	Py_Err_ಗಣ_STRING(Py_Exc_ಮೌಲ್ಯ_Error,
		N_("mark name must be a single character"));
	Py_XDECREF(todecref);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    mark = *pmark;

    Py_XDECREF(todecref);

    Vim_Try_ಆರಂಭ();
    switch_ಬಫರ್(&savebuf, self->buf);
    posp = getmark(mark, FALSE);
    restore_ಬಫರ್(&savebuf);
    ಆದರೆ (Vim_Try_ಕೊನೆ())
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (posp == ಖಾಲಿ)
    {
	Py_Err_ಗಣ_VIM(N_("invalid mark name"));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಆದರೆ (posp->lnum <= 0)
    {
	// Or raise an error?
	Py_INCREF(Py_None);
	ಹಿಂತಿರುಗಿ Py_None;
    }

    ಹಿಂತಿರುಗಿ Py_Build_ಮೌಲ್ಯ("(ll)", (ದೀರ್ಘ)(posp->lnum), (ದೀರ್ಘ)(posp->ಲಂಬಸಾಲು));
}

    ಸ್ಥಿರ PyObject *
ಬಫರ್_Range(ಬಫರ್_Object *self, PyObject *ವಾದಗಳು)
{
    PyInt ಆರಂಭ;
    PyInt ಕೊನೆ;

    ಆದರೆ (Check_ಬಫರ್(self))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (!PyArg_ParseTuple(ವಾದಗಳು, "nn", &ಆರಂಭ, &ಕೊನೆ))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಹಿಂತಿರುಗಿ RangeNew(self->buf, ಆರಂಭ, ಕೊನೆ);
}

    ಸ್ಥಿರ PyObject *
ಬಫರ್_Repr(PyObject *self_obj)
{
    ಬಫರ್_Object *self = (ಬಫರ್_Object*)self_obj;
    ಆದರೆ (self->buf == INVALID_ಬಫರ್_ಮೌಲ್ಯ)
	ಹಿಂತಿರುಗಿ PyString_FromFormat("<buffer object (deleted) at %p>", (ಶೂನ್ಯ *)self);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಅಕ್ಷರ	*name = (ಅಕ್ಷರ *)self->buf->b_fname;

	ಆದರೆ (name == ಖಾಲಿ)
	    name = "";

	ಹಿಂತಿರುಗಿ PyString_FromFormat("<buffer %s>", name);
    }
}

ಸ್ಥಿರ ರಚನೆ Py_ವಿಧಾನ_Def ಬಫರ್_Methods[] = {
    // name,	    function,			calling,	documentation
    {"append",	    (PyCFunction)ಬಫರ್_Append,	METH_VARARGS,	"Append data to Vim buffer" },
    {"mark",	    (PyCFunction)ಬಫರ್_Mark,	METH_O,		"Return (row,col) representing position of named mark" },
    {"range",	    (PyCFunction)ಬಫರ್_Range,	METH_VARARGS,	"Return a range object which represents the part of the given buffer between line numbers s and e" },
    {"__dir__",	    (PyCFunction)ಬಫರ್_Dir,	METH_NOARGS,	""},
    { ಖಾಲಿ,	    ಖಾಲಿ,			0,		ಖಾಲಿ}
};

/*
 * Buffer list object - Implementation
 */

DEFINE_PY_TYPE_OBJECT(BufMapType);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    Py_Object_ತಲೆ
} BufMapObject;

ಸ್ಥಿರ BufMapObject The_ಬಫರ್_Map =
{
    Py_Object_ತಲೆ_INIT_TYPE(BufMapType)
};

    ಸ್ಥಿರ PyInt
Buf_Map_ಉದ್ದಳತೆ(PyObject *self ಬಳಸದ)
{
    buf_T	*b = firstbuf;
    PyInt	n = 0;

    ಆಗ (b)
    {
	++n;
	b = b->b_ಮುಂದಿನ;
    }

    ಹಿಂತಿರುಗಿ n;
}

    ಸ್ಥಿರ PyObject *
Buf_Map_ಅಂಶ(PyObject *self ಬಳಸದ, PyObject *ಕೀಲಿ_Object)
{
    buf_T	*b;
    ದೀರ್ಘ	bnr;

    ಆದರೆ (ಸಂಖ್ಯೆ_ಗಮ್ಯ_Long(ಕೀಲಿ_Object, &bnr, ಸಂಖ್ಯೆ_INT|ಸಂಖ್ಯೆ_NATURAL))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    b = buflist_findnr((ಪೂರ್ಣಾಂಕ) bnr);

    ಆದರೆ (b)
	ಹಿಂತಿರುಗಿ ಬಫರ್_New(b);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	Py_Err_ಗಣ_Object(Py_Exc_ಕೀಲಿ_Error, ಕೀಲಿ_Object);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
BufMapIterDestruct(ಶೂನ್ಯ* arg)
{
    PyObject *ಬಫರ್ = (PyObject*)arg;
    // Iteration was stopped before all buffers were processed
    ಆದರೆ (ಬಫರ್)
    {
	Py_DECREF(ಬಫರ್);
    }
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
BufMapIterTraverse(ಶೂನ್ಯ *iter, visitproc ಭೇಟಿ, ಶೂನ್ಯ *arg)
{
    PyObject *ಬಫರ್ = (PyObject*)iter;
    ಆದರೆ (ಬಫರ್)
	Py_ಭೇಟಿ(ಬಫರ್);
    ಹಿಂತಿರುಗಿ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
BufMapIterClear(ಶೂನ್ಯ **iter)
{
    PyObject **ಬಫರ್ = (PyObject**)iter;
    ಆದರೆ (*ಬಫರ್)
	Py_CLEAR(*ಬಫರ್);
    ಹಿಂತಿರುಗಿ 0;
}

    ಸ್ಥಿರ PyObject *
Buf_Map_Iter_ಮುಂದಿನ(ಶೂನ್ಯ **arg)
{
    PyObject	*ಮುಂದಿನ;
    PyObject	*ret;
    PyObject	**ಬಫರ್ = (PyObject**)arg;

    ಆದರೆ (!*ಬಫರ್)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ret = *ಬಫರ್;

    ಆದರೆ (Check_ಬಫರ್((ಬಫರ್_Object *)(ret)))
    {
	*ಬಫರ್ = ಖಾಲಿ;
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಆದರೆ (!((ಬಫರ್_Object *)(ret))->buf->b_ಮುಂದಿನ)
	ಮುಂದಿನ = ಖಾಲಿ;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!(ಮುಂದಿನ = ಬಫರ್_New(((ಬಫರ್_Object *)(ret))->buf->b_ಮುಂದಿನ)))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    *ಬಫರ್ = ಮುಂದಿನ;
    // Do not increment reference: we no longer hold it (decref), but whoever
    // on other side will hold (incref). Decref+incref = nothing.
    ಹಿಂತಿರುಗಿ ret;
}

    ಸ್ಥಿರ PyObject *
BufMapIter(PyObject *self)
{
    PyObject *ಬಫರ್;

    ಬಫರ್ = ಬಫರ್_New(firstbuf);
    ಹಿಂತಿರುಗಿ IterNew(ಬಫರ್,
	    BufMapIterDestruct, Buf_Map_Iter_ಮುಂದಿನ,
	    BufMapIterTraverse, BufMapIterClear,
	    self);
}

ಸ್ಥಿರ PyMappingMethods BufMapAsMapping = {
    (lenfunc)       Buf_Map_ಉದ್ದಳತೆ,
    (binaryfunc)    Buf_Map_ಅಂಶ,
    (objobjargproc) 0,
};

// Current items object

ಸ್ಥಿರ ಅಕ್ಷರ *CurrentAttrs[] = {
    "buffer", "window", "line", "range", "tabpage",
    ಖಾಲಿ
};

    ಸ್ಥಿರ PyObject *
CurrentDir(PyObject *self, PyObject *ವಾದಗಳು ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ ObjectDir(self, CurrentAttrs);
}

    ಸ್ಥಿರ PyObject *
CurrentGetattr(PyObject *self ಬಳಸದ, ಅಕ್ಷರ *name)
{
    ಆದರೆ (strcmp(name, "buffer") == 0)
	ಹಿಂತಿರುಗಿ (PyObject *)ಬಫರ್_New(curbuf);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(name, "window") == 0)
	ಹಿಂತಿರುಗಿ (PyObject *)ಕಿಟಕಿ_New(curwin, curtab);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(name, "tabpage") == 0)
	ಹಿಂತಿರುಗಿ (PyObject *)Tab_ಪುಟ_New(curtab);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(name, "line") == 0)
	ಹಿಂತಿರುಗಿ Get_ಬಫರ್_ಸಾಲು(curbuf, (PyInt)curwin->w_cursor.lnum);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(name, "range") == 0)
	ಹಿಂತಿರುಗಿ RangeNew(curbuf, Range_ಆರಂಭ, Range_ಕೊನೆ);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(name, "__members__") == 0)
	ಹಿಂತಿರುಗಿ ObjectDir(ಖಾಲಿ, CurrentAttrs);
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ PY_MAJOR_ಆವೃತ್ತಿ < 3
	ಹಿಂತಿರುಗಿ Py_ಕಂಡುಹಿಡಿ_ವಿಧಾನ(ಕಿಟಕಿ_Methods, self, name);
#ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
CurrentSetattr(PyObject *self ಬಳಸದ, ಅಕ್ಷರ *name, PyObject *valObject)
{
    ಆದರೆ (strcmp(name, "line") == 0)
    {
	ಆದರೆ (ಗಣ_ಬಫರ್_ಸಾಲು_2(curbuf, (PyInt)curwin->w_cursor.lnum, valObject,
			  ಖಾಲಿ) == FAIL)
	    ಹಿಂತಿರುಗಿ -1;

	ಹಿಂತಿರುಗಿ 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(name, "buffer") == 0)
    {
	ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2;

	ಆದರೆ (valObject->ob_type != ಬಫರ್_Type_Ptr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.Buffer object, but got %s"),
		    valObject);
	    ಹಿಂತಿರುಗಿ -1;
	}

	ಆದರೆ (Check_ಬಫರ್((ಬಫರ್_Object *)(valObject)))
	    ಹಿಂತಿರುಗಿ -1;
	ಎಣಿಕೆ_2 = ((ಬಫರ್_Object *)(valObject))->buf->b_fnum;

	Vim_Try_ಆರಂಭ();
	ಆದರೆ (do_ಬಫರ್(DOBUF_GOTO, DOBUF_ಮೊದಲ, FORWARD, ಎಣಿಕೆ_2, 0) == FAIL)
	{
	    ಆದರೆ (Vim_Try_ಕೊನೆ())
		ಹಿಂತಿರುಗಿ -1;
	    PyErr_VIM_FORMAT(N_("failed to switch to buffer %d"), ಎಣಿಕೆ_2);
	    ಹಿಂತಿರುಗಿ -1;
	}

	ಹಿಂತಿರುಗಿ Vim_Try_ಕೊನೆ();
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(name, "window") == 0)
    {
	ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2;

	ಆದರೆ (valObject->ob_type != ಕಿಟಕಿ_Type_Ptr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.Window object, but got %s"),
		    valObject);
	    ಹಿಂತಿರುಗಿ -1;
	}

	ಆದರೆ (Check_ಕಿಟಕಿ((ಕಿಟಕಿ_Object *)(valObject)))
	    ಹಿಂತಿರುಗಿ -1;
	ಎಣಿಕೆ_2 = get_win_ಸಂಖ್ಯೆ(((ಕಿಟಕಿ_Object *)(valObject))->win, firstwin);

	ಆದರೆ (!ಎಣಿಕೆ_2)
	{
	    Py_Err_ಗಣ_STRING(Py_Exc_ಮೌಲ್ಯ_Error,
		    N_("failed to find window in the current tab page"));
	    ಹಿಂತಿರುಗಿ -1;
	}

	Vim_Try_ಆರಂಭ();
	win_goto(((ಕಿಟಕಿ_Object *)(valObject))->win);
	ಆದರೆ (((ಕಿಟಕಿ_Object *)(valObject))->win != curwin)
	{
	    ಆದರೆ (Vim_Try_ಕೊನೆ())
		ಹಿಂತಿರುಗಿ -1;
	    Py_Err_ಗಣ_STRING(PyExc_RuntimeError,
		    N_("did not switch to the specified window"));
	    ಹಿಂತಿರುಗಿ -1;
	}

	ಹಿಂತಿರುಗಿ Vim_Try_ಕೊನೆ();
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(name, "tabpage") == 0)
    {
	ಆದರೆ (valObject->ob_type != Tab_ಪುಟ_Type_Ptr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.TabPage object, but got %s"),
		    valObject);
	    ಹಿಂತಿರುಗಿ -1;
	}

	ಆದರೆ (Check_Tab_ಪುಟ((Tab_ಪುಟ_Object *)(valObject)))
	    ಹಿಂತಿರುಗಿ -1;

	Vim_Try_ಆರಂಭ();
	goto_tabpage_tp(((Tab_ಪುಟ_Object *)(valObject))->tab, TRUE, TRUE);
	ಆದರೆ (((Tab_ಪುಟ_Object *)(valObject))->tab != curtab)
	{
	    ಆದರೆ (Vim_Try_ಕೊನೆ())
		ಹಿಂತಿರುಗಿ -1;
	    Py_Err_ಗಣ_STRING(PyExc_RuntimeError,
		    N_("did not switch to the specified tab page"));
	    ಹಿಂತಿರುಗಿ -1;
	}

	ಹಿಂತಿರುಗಿ Vim_Try_ಕೊನೆ();
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	Py_Err_ಗಣ_String(PyExc_AttributeError, name);
	ಹಿಂತಿರುಗಿ -1;
    }
}

ಸ್ಥಿರ ರಚನೆ Py_ವಿಧಾನ_Def CurrentMethods[] = {
    // name,	    function,			calling,	documentation
    {"__dir__",	    (PyCFunction)CurrentDir,	METH_NOARGS,	""},
    { ಖಾಲಿ,	    ಖಾಲಿ,			0,		ಖಾಲಿ}
};

    ಸ್ಥಿರ ಶೂನ್ಯ
init_range_cmd(ಶೂನ್ಯ *arg)
{
    exarg_T *eap = (exarg_T*)arg;
    Range_ಆರಂಭ = eap->line1;
    Range_ಕೊನೆ = eap->line2;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
init_range_eval(ಶೂನ್ಯ *rettv ಬಳಸದ)
{
    Range_ಆರಂಭ = (PyInt) curwin->w_cursor.lnum;
    Range_ಕೊನೆ = Range_ಆರಂಭ;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಸತತ_cmd(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *cmd, dict_T* locals ಬಳಸದ, ಶೂನ್ಯ *arg ಬಳಸದ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ PY_CAN_RECURSE
	, PyGILState_STATE *pygilstate ಬಳಸದ
#ಆದರೆ_ಅಂತ್ಯ
	)
{
    PyObject	*ಸತತ_ret;
    ಸತತ_ret = Py_ಸತತ_String((ಅಕ್ಷರ *)cmd, Py_file_ಒಳಹರಿವು, globals, globals);
    ಆದರೆ (ಸತತ_ret != ಖಾಲಿ)
    {
	Py_DECREF(ಸತತ_ret);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
    {
	emsg(_(e_cant_handle_systemexit_ದ_python_exception_ಒಳ_vim));
	PyErr_Clear();
    }
    ಇಲ್ಲದಿದ್ದರೆ
	PyErr_PrintEx(1);
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ	*ಸಂಕೇತ_hdr = "def " DOPY_FUNC "(line, linenr):\n ";
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ		ಸಂಕೇತ_hdr_len = 30;

    ಸ್ಥಿರ ಶೂನ್ಯ
ಸತತ_do(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *cmd, dict_T* locals ಬಳಸದ, ಶೂನ್ಯ *arg ಬಳಸದ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ PY_CAN_RECURSE
	, PyGILState_STATE *pygilstate
#ಆದರೆ_ಅಂತ್ಯ
	)
{
    PyInt	lnum;
    size_t	len;
    ಅಕ್ಷರ	*ಸಂಕೇತ_2;
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಿತಿ_2;
    PyObject	*pyfunc, *pymain;
    PyObject	*ಸತತ_ret;
    buf_T	*was_curbuf = curbuf;

    ಆದರೆ (u_save((linenr_T)Range_ಆರಂಭ - 1, (linenr_T)Range_ಕೊನೆ + 1) != OK)
    {
	emsg(_("cannot save undo information"));
	ಹಿಂತಿರುಗಿ;
    }

    len = ಸಂಕೇತ_hdr_len + STRLEN(cmd);
    ಸಂಕೇತ_2 = PyMem_New(ಅಕ್ಷರ, len + 1);
    ಸ್ಮರಣೆ_ನಕಲು(ಸಂಕೇತ_2, ಸಂಕೇತ_hdr, ಸಂಕೇತ_hdr_len);
    STRCPY(ಸಂಕೇತ_2 + ಸಂಕೇತ_hdr_len, cmd);
    ಸತತ_ret = Py_ಸತತ_String(ಸಂಕೇತ_2, Py_file_ಒಳಹರಿವು, globals, globals);
    ಸ್ಥಿತಿ_2 = -1;
    ಆದರೆ (ಸತತ_ret != ಖಾಲಿ)
    {
	ಸ್ಥಿತಿ_2 = 0;
	Py_DECREF(ಸತತ_ret);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
    {
	PyMem_Free(ಸಂಕೇತ_2);
	emsg(_(e_cant_handle_systemexit_ದ_python_exception_ಒಳ_vim));
	PyErr_Clear();
	ಹಿಂತಿರುಗಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	PyErr_PrintEx(1);

    PyMem_Free(ಸಂಕೇತ_2);

    ಆದರೆ (ಸ್ಥಿತಿ_2)
    {
	emsg(_("failed to run the code"));
	ಹಿಂತಿರುಗಿ;
    }

    ಸ್ಥಿತಿ_2 = 0;
    pymain = Py_Import_ಕೂಡಿಸು_Module("__main__");
    pyfunc = PyObject_GetAttrString(pymain, DOPY_FUNC);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ PY_CAN_RECURSE
    PyGILState_Release(*pygilstate);
#ಆದರೆ_ಅಂತ್ಯ

    ಗಾಗಿ (lnum = Range_ಆರಂಭ; lnum <= Range_ಕೊನೆ; ++lnum)
    {
	PyObject	*ಸಾಲು;
	PyObject	*linenr;
	PyObject	*ret;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ PY_CAN_RECURSE
	*pygilstate = PyGILState_Ensure();
#ಆದರೆ_ಅಂತ್ಯ
	// Check the line number, the command my have deleted lines.
	ಆದರೆ (lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
		|| !(ಸಾಲು = Get_ಬಫರ್_ಸಾಲು(curbuf, lnum)))
	    ಹೋಗು err;
	ಆದರೆ (!(linenr = PyInt_FromLong((ದೀರ್ಘ) lnum)))
	{
	    Py_DECREF(ಸಾಲು);
	    ಹೋಗು err;
	}
	ret = Py_Object_Call_Function_Obj_ವಾದಗಳು(pyfunc, ಸಾಲು, linenr, ಖಾಲಿ);
	Py_DECREF(ಸಾಲು);
	Py_DECREF(linenr);
	ಆದರೆ (!ret)
	    ಹೋಗು err;

	// Check that the command didn't switch to another buffer.
	// Check the line number, the command my have deleted lines.
	ಆದರೆ (curbuf != was_curbuf || lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	{
	    Py_XDECREF(ret);
	    ಹೋಗು err;
	}

	ಆದರೆ (ret != Py_None)
	    ಆದರೆ (ಗಣ_ಬಫರ್_ಸಾಲು_2(curbuf, lnum, ret, ಖಾಲಿ) == FAIL)
	    {
		Py_XDECREF(ret);
		ಹೋಗು err;
	    }

	Py_XDECREF(ret);
	PythonIO_Flush();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ PY_CAN_RECURSE
	PyGILState_Release(*pygilstate);
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಹೋಗು ಹೊರಹೋಗು;
err:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ PY_CAN_RECURSE
    *pygilstate = PyGILState_Ensure();
#ಆದರೆ_ಅಂತ್ಯ
    PyErr_PrintEx(0);
    PythonIO_Flush();
    ಸ್ಥಿತಿ_2 = 1;
ಹೊರಹೋಗು:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ PY_CAN_RECURSE
    ಆದರೆ (!ಸ್ಥಿತಿ_2)
	*pygilstate = PyGILState_Ensure();
#ಆದರೆ_ಅಂತ್ಯ
    Py_DECREF(pyfunc);
    Py_Object_ಗಣ_Attr_String(pymain, DOPY_FUNC, ಖಾಲಿ);
    ಆದರೆ (ಸ್ಥಿತಿ_2)
	ಹಿಂತಿರುಗಿ;
    check_cursor();
    update_curbuf(UPD_NOT_VALID);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಸತತ_eval(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *cmd, dict_T *locals, ಶೂನ್ಯ *arg
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ PY_CAN_RECURSE
	, PyGILState_STATE *pygilstate ಬಳಸದ
#ಆದರೆ_ಅಂತ್ಯ
	)
{
    PyObject	*ಸತತ_ret;
    typval_T	*rettv = (typval_T*)arg;
    PyObject	*pylocals = locals ? NEW_DICTIONARY(locals) : globals;

    ಸತತ_ret = Py_ಸತತ_String((ಅಕ್ಷರ *)cmd, Py_eval_ಒಳಹರಿವು, globals, pylocals);
    ಆದರೆ (ಸತತ_ret == ಖಾಲಿ)
    {
	ಆದರೆ (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
	{
	    emsg(_(e_cant_handle_systemexit_ದ_python_exception_ಒಳ_vim));
	    PyErr_Clear();
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (PyErr_Occurred() && !ಸಂದೇಶ_silent)
		PyErr_PrintEx(0);
	    emsg(_(e_eval_did_not_return_valid_python_object));
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (ConvertFromPyObject(ಸತತ_ret, rettv) == -1)
	    emsg(_(e_failed_ಗಮ್ಯ_convert_returned_python_object_ಗಮ್ಯ_vim_ಮೌಲ್ಯ));
	Py_DECREF(ಸತತ_ret);
    }
    PyErr_Clear();
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಗಣ_ref_ಒಳ_py(ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ copyID)
{
    pylinkedlist_T	*cur;
    list_T		*ll;
    tuple_T		*tt;
    ಪೂರ್ಣಾಂಕ			i;
    ಪೂರ್ಣಾಂಕ			abort = FALSE;
    FunctionObject	*func;

    ಆದರೆ (lastdict != ಖಾಲಿ)
    {
	ಗಾಗಿ (cur = lastdict ; !abort && cur != ಖಾಲಿ ; cur = cur->pll_prev)
	    abort = ಗಣ_ref_ಒಳ_dict(((DictionaryObject *)(cur->pll_obj))->dict,
								       copyID);
    }

    ಆದರೆ (lastlist != ಖಾಲಿ)
    {
	ಗಾಗಿ (cur = lastlist ; !abort && cur != ಖಾಲಿ ; cur = cur->pll_prev)
	{
	    ll = ((ListObject *) (cur->pll_obj))->list;
	    abort = ಗಣ_ref_ಒಳ_list(ll, copyID);
	}
    }

    ಆದರೆ (lasttuple != ಖಾಲಿ)
    {
	ಗಾಗಿ (cur = lasttuple ; !abort && cur != ಖಾಲಿ ; cur = cur->pll_prev)
	{
	    tt = ((TupleObject *) (cur->pll_obj))->tuple;
	    abort = ಗಣ_ref_ಒಳ_tuple(tt, copyID);
	}
    }

    ಆದರೆ (lastfunc != ಖಾಲಿ)
    {
	ಗಾಗಿ (cur = lastfunc ; !abort && cur != ಖಾಲಿ ; cur = cur->pll_prev)
	{
	    func = (FunctionObject *) cur->pll_obj;
	    abort = ಗಣ_ref_ಒಳ_dict(func->self, copyID);
	    ಆದರೆ (func->argc)
		ಗಾಗಿ (i = 0; !abort && i < func->argc; ++i)
		    abort = abort
			|| ಗಣ_ref_ಒಳ_ಅಂಶ(&func->argv[i], copyID, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
	}
    }

    ಹಿಂತಿರುಗಿ abort;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಗಣ_string_copy(char_u *str, typval_T *tv)
{
    tv->vval.v_string = vim_strsave(str);
    ಆದರೆ (tv->vval.v_string == ಖಾಲಿ)
    {
	Py_Err_No_ಸ್ಮರಣೆ();
	ಹಿಂತಿರುಗಿ -1;
    }
    ಹಿಂತಿರುಗಿ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
pydict_ಗಮ್ಯ_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    dict_T	*dict;
    char_u	*ಕೀಲಿ;
    dictitem_T	*di;
    PyObject	*ಕೀಲಿ_Object;
    PyObject	*valObject;
    Py_ssize_t	iter = 0;

    ಆದರೆ (!(dict = py_dict_alloc()))
	ಹಿಂತಿರುಗಿ -1;

    tv->v_type = VAR_DICT;
    tv->vval.v_dict = dict;

    ಆಗ (Py_Dict_ಮುಂದಿನ(obj, &iter, &ಕೀಲಿ_Object, &valObject))
    {
	PyObject	*todecref = ಖಾಲಿ;

	ಆದರೆ (ಕೀಲಿ_Object == ಖಾಲಿ || valObject == ಖಾಲಿ)
	{
	    dict_unref(dict);
	    ಹಿಂತಿರುಗಿ -1;
	}

	ಆದರೆ (!(ಕೀಲಿ = String_ಗಮ್ಯ_Chars(ಕೀಲಿ_Object, &todecref)))
	{
	    dict_unref(dict);
	    ಹಿಂತಿರುಗಿ -1;
	}

	ಆದರೆ (*ಕೀಲಿ == NUL)
	{
	    dict_unref(dict);
	    Py_XDECREF(todecref);
	    RAISE_NO_ಖಾಲಿಗಳು_ಕೀಲಿಗಳು;
	    ಹಿಂತಿರುಗಿ -1;
	}

	di = dictitem_alloc(ಕೀಲಿ);

	Py_XDECREF(todecref);

	ಆದರೆ (di == ಖಾಲಿ)
	{
	    Py_Err_No_ಸ್ಮರಣೆ();
	    dict_unref(dict);
	    ಹಿಂತಿರುಗಿ -1;
	}

	ಆದರೆ (_ConvertFromPyObject(valObject, &di->di_tv, lookup_dict) == -1)
	{
	    vim_free(di);
	    dict_unref(dict);
	    ಹಿಂತಿರುಗಿ -1;
	}

	ಆದರೆ (dict_ಕೂಡಿಸು(dict, di) == FAIL)
	{
	    RAISE_ಕೀಲಿ_ಕೂಡಿಸು_FAIL(di->di_ಕೀಲಿ);
	    clear_tv(&di->di_tv);
	    vim_free(di);
	    dict_unref(dict);
	    ಹಿಂತಿರುಗಿ -1;
	}
    }

    --dict->dv_refcount;
    ಹಿಂತಿರುಗಿ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
pymap_ಗಮ್ಯ_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    dict_T	*dict;
    char_u	*ಕೀಲಿ;
    dictitem_T	*di;
    PyObject	*list;
    PyObject	*iterator;
    PyObject	*ಕೀಲಿ_Object;
    PyObject	*valObject;

    ಆದರೆ (!(dict = py_dict_alloc()))
	ಹಿಂತಿರುಗಿ -1;

    tv->v_type = VAR_DICT;
    tv->vval.v_dict = dict;

    ಆದರೆ (!(list = Py_Mapping_ಕೀಲಿಗಳು(obj)))
    {
	dict_unref(dict);
	ಹಿಂತಿರುಗಿ -1;
    }

    ಆದರೆ (!(iterator = PyObject_GetIter(list)))
    {
	dict_unref(dict);
	Py_DECREF(list);
	ಹಿಂತಿರುಗಿ -1;
    }
    Py_DECREF(list);

    ಆಗ ((ಕೀಲಿ_Object = Py_Iter_ಮುಂದಿನ(iterator)))
    {
	PyObject	*todecref;

	ಆದರೆ (!(ಕೀಲಿ = String_ಗಮ್ಯ_Chars(ಕೀಲಿ_Object, &todecref)))
	{
	    Py_DECREF(ಕೀಲಿ_Object);
	    Py_DECREF(iterator);
	    dict_unref(dict);
	    ಹಿಂತಿರುಗಿ -1;
	}

	ಆದರೆ (*ಕೀಲಿ == NUL)
	{
	    Py_DECREF(ಕೀಲಿ_Object);
	    Py_DECREF(iterator);
	    Py_XDECREF(todecref);
	    dict_unref(dict);
	    RAISE_NO_ಖಾಲಿಗಳು_ಕೀಲಿಗಳು;
	    ಹಿಂತಿರುಗಿ -1;
	}

	ಆದರೆ (!(valObject = Py_Object_Get_ಅಂಶ(obj, ಕೀಲಿ_Object)))
	{
	    Py_DECREF(ಕೀಲಿ_Object);
	    Py_DECREF(iterator);
	    Py_XDECREF(todecref);
	    dict_unref(dict);
	    ಹಿಂತಿರುಗಿ -1;
	}

	di = dictitem_alloc(ಕೀಲಿ);

	Py_DECREF(ಕೀಲಿ_Object);
	Py_XDECREF(todecref);

	ಆದರೆ (di == ಖಾಲಿ)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(valObject);
	    dict_unref(dict);
	    Py_Err_No_ಸ್ಮರಣೆ();
	    ಹಿಂತಿರುಗಿ -1;
	}

	ಆದರೆ (_ConvertFromPyObject(valObject, &di->di_tv, lookup_dict) == -1)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(valObject);
	    vim_free(di);
	    dict_unref(dict);
	    ಹಿಂತಿರುಗಿ -1;
	}

	Py_DECREF(valObject);

	ಆದರೆ (dict_ಕೂಡಿಸು(dict, di) == FAIL)
	{
	    RAISE_ಕೀಲಿ_ಕೂಡಿಸು_FAIL(di->di_ಕೀಲಿ);
	    Py_DECREF(iterator);
	    dictitem_free(di);
	    dict_unref(dict);
	    ಹಿಂತಿರುಗಿ -1;
	}
    }
    Py_DECREF(iterator);
    --dict->dv_refcount;
    ಹಿಂತಿರುಗಿ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
pyseq_ಗಮ್ಯ_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    list_T	*l;

    ಆದರೆ (!(l = py_list_alloc()))
	ಹಿಂತಿರುಗಿ -1;

    tv->v_type = VAR_LIST;
    tv->vval.v_list = l;

    ಆದರೆ (list_py_concat(l, obj, lookup_dict) == -1)
    {
	list_unref(l);
	ಹಿಂತಿರುಗಿ -1;
    }

    --l->lv_refcount;
    ಹಿಂತಿರುಗಿ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
pytuple_ಗಮ್ಯ_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    tuple_T	*t;

    ಆದರೆ (!(t = py_tuple_alloc()))
	ಹಿಂತಿರುಗಿ -1;

    tv->v_type = VAR_TUPLE;
    tv->vval.v_tuple = t;

    ಆದರೆ (tuple_py_concat(t, obj, lookup_dict) == -1)
    {
	tuple_unref(t);
	ಹಿಂತಿರುಗಿ -1;
    }

    --t->tv_refcount;
    ಹಿಂತಿರುಗಿ 0;
}

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*pytotvfunc)(PyObject *, typval_T *, PyObject *);

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
convert_dl(PyObject *obj, typval_T *tv,
				    pytotvfunc py_ಗಮ್ಯ_tv, PyObject *lookup_dict)
{
    PyObject	*capsule;
    ಅಕ್ಷರ	hexBuf[ಗಾತ್ರ(ಶೂನ್ಯ *) * 2 + 3];

    sprintf(hexBuf, "%p", (ಶೂನ್ಯ *)obj);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ PY_USE_CAPSULE
    capsule = Py_Dict_Get_ಅಂಶ_String(lookup_dict, hexBuf);
#ಇಲ್ಲದಿದ್ದರೆ
    capsule = (PyObject *)Py_Dict_Get_ಅಂಶ_String(lookup_dict, hexBuf);
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (capsule == ಖಾಲಿ)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ PY_USE_CAPSULE
	capsule = PyCapsule_New(tv, ಖಾಲಿ, ಖಾಲಿ);
#ಇಲ್ಲದಿದ್ದರೆ
	capsule = PyCObject_FromVoidPtr(tv, ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (Py_Dict_ಗಣ_ಅಂಶ_String(lookup_dict, hexBuf, capsule))
	{
	    Py_DECREF(capsule);
	    tv->v_type = VAR_UNKNOWN;
	    ಹಿಂತಿರುಗಿ -1;
	}

	Py_DECREF(capsule);

	ಆದರೆ (py_ಗಮ್ಯ_tv(obj, tv, lookup_dict) == -1)
	{
	    tv->v_type = VAR_UNKNOWN;
	    ಹಿಂತಿರುಗಿ -1;
	}
	// As we are not using copy_tv which increments reference count we must
	// do it ourself.
	ಆದರೆ (tv->v_type == VAR_DICT)
	    ++tv->vval.v_dict->dv_refcount;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tv->v_type == VAR_LIST)
	    ++tv->vval.v_list->lv_refcount;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tv->v_type == VAR_TUPLE)
	    ++tv->vval.v_tuple->tv_refcount;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	typval_T	*v;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ PY_USE_CAPSULE
	v = Py_Capsule_Get_ಸೂಚಕ(capsule, ಖಾಲಿ);
#ಇಲ್ಲದಿದ್ದರೆ
	v = PyCObject_AsVoidPtr(capsule);
#ಆದರೆ_ಅಂತ್ಯ
	copy_tv(v, tv);
    }
    ಹಿಂತಿರುಗಿ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ConvertFromPyMapping(PyObject *obj, typval_T *tv)
{
    PyObject	*lookup_dict;
    ಪೂರ್ಣಾಂಕ		ret;

    ಆದರೆ (!(lookup_dict = PyDict_New()))
	ಹಿಂತಿರುಗಿ -1;

    ಆದರೆ (PyType_IsSubtype(obj->ob_type, DictionaryTypePtr))
    {
	tv->v_type = VAR_DICT;
	tv->vval.v_dict = (((DictionaryObject *)(obj))->dict);
	++tv->vval.v_dict->dv_refcount;
	ret = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (PyDict_Check(obj))
	ret = convert_dl(obj, tv, pydict_ಗಮ್ಯ_tv, lookup_dict);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (PyMapping_Check(obj))
	ret = convert_dl(obj, tv, pymap_ಗಮ್ಯ_tv, lookup_dict);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	PyErr_FORMAT_TYPE(
		N_("unable to convert %s to a Vim dictionary"),
		obj);
	ret = -1;
    }
    Py_DECREF(lookup_dict);
    ಹಿಂತಿರುಗಿ ret;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ConvertFromPySequence(PyObject *obj, typval_T *tv)
{
    PyObject	*lookup_dict;
    ಪೂರ್ಣಾಂಕ		ret;

    ಆದರೆ (!(lookup_dict = PyDict_New()))
	ಹಿಂತಿರುಗಿ -1;

    ಆದರೆ (PyType_IsSubtype(obj->ob_type, ListTypePtr))
    {
	tv->v_type = VAR_LIST;
	tv->vval.v_list = (((ListObject *)(obj))->list);
	++tv->vval.v_list->lv_refcount;
	ret = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (PyIter_Check(obj) || PySequence_Check(obj))
	ret = convert_dl(obj, tv, pyseq_ಗಮ್ಯ_tv, lookup_dict);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	PyErr_FORMAT_TYPE(
		N_("unable to convert %s to a Vim list"),
		obj);
	ret = -1;
    }
    Py_DECREF(lookup_dict);
    ಹಿಂತಿರುಗಿ ret;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ConvertFromPyObject(PyObject *obj, typval_T *tv)
{
    PyObject	*lookup_dict;
    ಪೂರ್ಣಾಂಕ		ret;

    ಆದರೆ (!(lookup_dict = PyDict_New()))
	ಹಿಂತಿರುಗಿ -1;
    ret = _ConvertFromPyObject(obj, tv, lookup_dict);
    Py_DECREF(lookup_dict);
    ಹಿಂತಿರುಗಿ ret;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
_ConvertFromPyObject(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    ಆದರೆ (PyType_IsSubtype(obj->ob_type, DictionaryTypePtr))
    {
	tv->v_type = VAR_DICT;
	tv->vval.v_dict = (((DictionaryObject *)(obj))->dict);
	++tv->vval.v_dict->dv_refcount;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (PyType_IsSubtype(obj->ob_type, ListTypePtr))
    {
	tv->v_type = VAR_LIST;
	tv->vval.v_list = (((ListObject *)(obj))->list);
	++tv->vval.v_list->lv_refcount;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (PyType_IsSubtype(obj->ob_type, TupleTypePtr))
    {
	tv->v_type = VAR_TUPLE;
	tv->vval.v_tuple = (((TupleObject *)(obj))->tuple);
	++tv->vval.v_tuple->tv_refcount;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (PyType_IsSubtype(obj->ob_type, FunctionTypePtr))
    {
	FunctionObject *func = (FunctionObject *) obj;
	ಆದರೆ (func->self != ಖಾಲಿ || func->argv != ಖಾಲಿ)
	{
	    partial_T *pt = ALLOC_CLEAR_ONE(partial_T);

	    ಗಣ_partial(func, pt, TRUE);
	    tv->vval.v_partial = pt;
	    tv->v_type = VAR_PARTIAL;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (ಗಣ_string_copy(func->name, tv) == -1)
		ಹಿಂತಿರುಗಿ -1;

	    tv->v_type = VAR_FUNC;
	}
	func_ref(func->name);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (Py_ಬೈಟುಗಳು_Check(obj))
    {
	char_u	*str;

	ಆದರೆ (Py_ಬೈಟುಗಳು_As_String_And_ಗಾತ್ರ(obj, (ಅಕ್ಷರ **) &str, ಖಾಲಿ) == -1)
	    ಹಿಂತಿರುಗಿ -1;
	ಆದರೆ (str == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ -1;

	ಆದರೆ (ಗಣ_string_copy(str, tv) == -1)
	    ಹಿಂತಿರುಗಿ -1;

	tv->v_type = VAR_STRING;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (PyUnicode_Check(obj))
    {
	PyObject	*ಬೈಟುಗಳು;
	char_u	*str;

	ಬೈಟುಗಳು = PyUnicode_AsEncodedString(obj, ENC_OPT, ERRORS_ENCODE_ARG);
	ಆದರೆ (ಬೈಟುಗಳು == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ -1;

	ಆದರೆ (Py_ಬೈಟುಗಳು_As_String_And_ಗಾತ್ರ(ಬೈಟುಗಳು, (ಅಕ್ಷರ **) &str, ಖಾಲಿ) == -1)
	    ಹಿಂತಿರುಗಿ -1;
	ಆದರೆ (str == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ -1;

	ಆದರೆ (ಗಣ_string_copy(str, tv))
	{
	    Py_XDECREF(ಬೈಟುಗಳು);
	    ಹಿಂತಿರುಗಿ -1;
	}
	Py_XDECREF(ಬೈಟುಗಳು);

	tv->v_type = VAR_STRING;
    }
#ಆದರೆ PY_MAJOR_ಆವೃತ್ತಿ < 3
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (PyInt_Check(obj))
    {
	tv->v_type = VAR_ಸಂಖ್ಯೆ;
	tv->vval.v_ಸಂಖ್ಯೆ = (varnumber_T) PyInt_AsLong(obj);
	ಆದರೆ (PyErr_Occurred())
	    ಹಿಂತಿರುಗಿ -1;
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (PyLong_Check(obj))
    {
	tv->v_type = VAR_ಸಂಖ್ಯೆ;
	tv->vval.v_ಸಂಖ್ಯೆ = (varnumber_T) PyLong_AsLong(obj);
	ಆದರೆ (PyErr_Occurred())
	    ಹಿಂತಿರುಗಿ -1;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (PyTuple_Check(obj))
	ಹಿಂತಿರುಗಿ convert_dl(obj, tv, pytuple_ಗಮ್ಯ_tv, lookup_dict);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (PyDict_Check(obj))
	ಹಿಂತಿರುಗಿ convert_dl(obj, tv, pydict_ಗಮ್ಯ_tv, lookup_dict);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (PyFloat_Check(obj))
    {
	tv->v_type = VAR_FLOAT;
	tv->vval.v_float = (float_T) PyFloat_AsDouble(obj);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (PyObject_HasAttrString(obj, "keys"))
	ಹಿಂತಿರುಗಿ convert_dl(obj, tv, pymap_ಗಮ್ಯ_tv, lookup_dict);
    // PyObject_GetIter can create built-in iterator for any sequence object
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (PyIter_Check(obj) || PySequence_Check(obj))
	ಹಿಂತಿರುಗಿ convert_dl(obj, tv, pyseq_ಗಮ್ಯ_tv, lookup_dict);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (PyMapping_Check(obj))
	ಹಿಂತಿರುಗಿ convert_dl(obj, tv, pymap_ಗಮ್ಯ_tv, lookup_dict);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (Py_ಸಂಖ್ಯೆ_Check(obj))
    {
	PyObject	*ಅಂಕೆ;

	ಆದರೆ (!(ಅಂಕೆ = Py_ಸಂಖ್ಯೆ_Long(obj)))
	    ಹಿಂತಿರುಗಿ -1;

	tv->v_type = VAR_ಸಂಖ್ಯೆ;
	tv->vval.v_ಸಂಖ್ಯೆ = (varnumber_T) PyLong_AsLong(ಅಂಕೆ);

	Py_DECREF(ಅಂಕೆ);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (obj == Py_None)
    {
	tv->v_type = VAR_SPECIAL;
	tv->vval.v_ಸಂಖ್ಯೆ = VVAL_NONE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	PyErr_FORMAT_TYPE(
		N_("unable to convert %s to a Vim structure"),
		obj);
	ಹಿಂತಿರುಗಿ -1;
    }
    ಹಿಂತಿರುಗಿ 0;
}

    ಸ್ಥಿರ PyObject *
Convert_ಗಮ್ಯ_Py_Object(typval_T *tv)
{
    typval_T *argv;
    ಪೂರ್ಣಾಂಕ i;
    ಆದರೆ (tv == ಖಾಲಿ)
    {
	Py_Err_ಗಣ_VIM(N_("internal error: NULL reference passed"));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಆಯ್ಕೆ (tv->v_type)
    {
	ಸ್ಥಿತಿ VAR_STRING:
	    ಹಿಂತಿರುಗಿ Py_ಬೈಟುಗಳು_From_String(tv->vval.v_string == ಖಾಲಿ
					    ? "" : (ಅಕ್ಷರ *)tv->vval.v_string);
	ಸ್ಥಿತಿ VAR_ಸಂಖ್ಯೆ:
	    ಹಿಂತಿರುಗಿ PyLong_FromLong((ದೀರ್ಘ) tv->vval.v_ಸಂಖ್ಯೆ);
	ಸ್ಥಿತಿ VAR_FLOAT:
	    ಹಿಂತಿರುಗಿ PyFloat_FromDouble((ದ್ವಿಗುಣ) tv->vval.v_float);
	ಸ್ಥಿತಿ VAR_LIST:
	    ಹಿಂತಿರುಗಿ NEW_LIST(tv->vval.v_list);
	ಸ್ಥಿತಿ VAR_TUPLE:
	    ಹಿಂತಿರುಗಿ NEW_TUPLE(tv->vval.v_tuple);
	ಸ್ಥಿತಿ VAR_DICT:
	    ಹಿಂತಿರುಗಿ NEW_DICTIONARY(tv->vval.v_dict);
	ಸ್ಥಿತಿ VAR_FUNC:
	    ಹಿಂತಿರುಗಿ NEW_FUNCTION(tv->vval.v_string == ಖಾಲಿ
					  ? (char_u *)"" : tv->vval.v_string,
					  0, ಖಾಲಿ, ಖಾಲಿ, TRUE);
	ಸ್ಥಿತಿ VAR_PARTIAL:
	    ಆದರೆ (tv->vval.v_partial->pt_argc)
	    {
		argv = PyMem_New(typval_T, (size_t)tv->vval.v_partial->pt_argc);
		ಗಾಗಿ (i = 0; i < tv->vval.v_partial->pt_argc; i++)
		    copy_tv(&tv->vval.v_partial->pt_argv[i], &argv[i]);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		argv = ಖಾಲಿ;
	    ಆದರೆ (tv->vval.v_partial->pt_dict != ಖಾಲಿ)
		tv->vval.v_partial->pt_dict->dv_refcount++;
	    ಹಿಂತಿರುಗಿ NEW_FUNCTION(tv->vval.v_partial == ಖಾಲಿ
			     ? (char_u *)"" : partial_name(tv->vval.v_partial),
				tv->vval.v_partial->pt_argc, argv,
				tv->vval.v_partial->pt_dict,
				tv->vval.v_partial->pt_auto);
	ಸ್ಥಿತಿ VAR_BLOB:
	    ಹಿಂತಿರುಗಿ Py_ಬೈಟುಗಳು_From_String_And_ಗಾತ್ರ(
		(ಅಕ್ಷರ*) tv->vval.v_blob->bv_ga.ga_ದತ್ತಾಂಶ,
		(Py_ssize_t) tv->vval.v_blob->bv_ga.ga_len);
	ಸ್ಥಿತಿ VAR_UNKNOWN:
	ಸ್ಥಿತಿ VAR_ANY:
	ಸ್ಥಿತಿ VAR_VOID:
	ಸ್ಥಿತಿ VAR_CHANNEL:
	ಸ್ಥಿತಿ VAR_ಕೆಲಸ:
	ಸ್ಥಿತಿ VAR_INSTR:
	ಸ್ಥಿತಿ VAR_CLASS:
	ಸ್ಥಿತಿ VAR_OBJECT:
	ಸ್ಥಿತಿ VAR_TYPEALIAS:
	    Py_INCREF(Py_None);
	    ಹಿಂತಿರುಗಿ Py_None;
	ಸ್ಥಿತಿ VAR_BOOL:
	ಸ್ಥಿತಿ VAR_SPECIAL:
	    ಆಯ್ಕೆ (tv->vval.v_ಸಂಖ್ಯೆ)
	    {
		ಸ್ಥಿತಿ VVAL_FALSE: ಹಿಂತಿರುಗಿ ALWAYS_FALSE;
		ಸ್ಥಿತಿ VVAL_TRUE:  ಹಿಂತಿರುಗಿ ALWAYS_TRUE;
		ಸ್ಥಿತಿ VVAL_NONE:
		ಸ್ಥಿತಿ VVAL_NULL:  ಹಿಂತಿರುಗಿ ALWAYS_NONE;
	    }
	    Py_Err_ಗಣ_VIM(N_("internal error: invalid value type"));
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

DEFINE_PY_TYPE_OBJECT(CurrentType);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    Py_Object_ತಲೆ
} CurrentObject;

ಸ್ಥಿರ CurrentObject TheCurrent =
{
    Py_Object_ತಲೆ_INIT_TYPE(CurrentType)
};

    ಸ್ಥಿರ ಶೂನ್ಯ
init_structs(ಶೂನ್ಯ)
{
    CLEAR_ಕ್ಷೇತ್ರ(OutputType);
    OutputType.tp_name = "vim.message";
    OutputType.tp_basicsize = ಗಾತ್ರ(OutputObject);
    OutputType.tp_flags = Py_TPFLAGS_DEFAULT;
    OutputType.tp_doc = "vim message object";
    OutputType.tp_methods = OutputMethods;
#ಆದರೆ PY_MAJOR_ಆವೃತ್ತಿ >= 3
    OutputType.tp_getattro = OutputGetattro;
    OutputType.tp_setattro = OutputSetattro;
    OutputType.tp_alloc = call_PyType_GenericAlloc;
    OutputType.tp_new = call_PyType_GenericNew;
    OutputType.tp_free = call_PyObject_Free;
# ifndef USE_LIMITED_API
    // The std printer type is only exposed in full API. It is not essential
    // anyway and so in limited API we don't set it.
    OutputType.tp_ತಳ = &PyStdPrinter_Type;
# endif
#ಇಲ್ಲದಿದ್ದರೆ
    OutputType.tp_getattr = OutputGetattr;
    OutputType.tp_setattr = OutputSetattr;
    // Disabled, because this causes a crash in test86
    // OutputType.tp_base = &PyFile_Type;
#ಆದರೆ_ಅಂತ್ಯ

    CLEAR_ಕ್ಷೇತ್ರ(IterType);
    IterType.tp_name = "vim.iter";
    IterType.tp_basicsize = ಗಾತ್ರ(IterObject);
    IterType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    IterType.tp_doc = "generic iterator object";
    IterType.tp_iter = IterIter;
    IterType.tp_iternext = Iter_ಮುಂದಿನ;
    IterType.tp_dealloc = IterDestructor;
    IterType.tp_traverse = IterTraverse;
    IterType.tp_clear = IterClear;

    CLEAR_ಕ್ಷೇತ್ರ(ಬಫರ್_Type);
    ಬಫರ್_Type.tp_name = "vim.buffer";
    ಬಫರ್_Type.tp_basicsize = ಗಾತ್ರ(ಬಫರ್_Object);
    ಬಫರ್_Type.tp_dealloc = ಬಫರ್_Destructor;
    ಬಫರ್_Type.tp_repr = ಬಫರ್_Repr;
    ಬಫರ್_Type.tp_as_sequence = &ಬಫರ್_As_Seq;
    ಬಫರ್_Type.tp_as_mapping = &ಬಫರ್_As_Mapping;
    ಬಫರ್_Type.tp_flags = Py_TPFLAGS_DEFAULT;
    ಬಫರ್_Type.tp_doc = "vim buffer object";
    ಬಫರ್_Type.tp_methods = ಬಫರ್_Methods;
#ಆದರೆ PY_MAJOR_ಆವೃತ್ತಿ >= 3
    ಬಫರ್_Type.tp_getattro = ಬಫರ್_Getattro;
    ಬಫರ್_Type.tp_setattro = ಬಫರ್_Setattro;
    ಬಫರ್_Type.tp_alloc = call_PyType_GenericAlloc;
    ಬಫರ್_Type.tp_new = call_PyType_GenericNew;
    ಬಫರ್_Type.tp_free = call_PyObject_Free;
#ಇಲ್ಲದಿದ್ದರೆ
    ಬಫರ್_Type.tp_getattr = ಬಫರ್_Getattr;
    ಬಫರ್_Type.tp_setattr = ಬಫರ್_Setattr;
#ಆದರೆ_ಅಂತ್ಯ

    CLEAR_ಕ್ಷೇತ್ರ(ಕಿಟಕಿ_Type);
    ಕಿಟಕಿ_Type.tp_name = "vim.window";
    ಕಿಟಕಿ_Type.tp_basicsize = ಗಾತ್ರ(ಕಿಟಕಿ_Object);
    ಕಿಟಕಿ_Type.tp_dealloc = ಕಿಟಕಿ_Destructor;
    ಕಿಟಕಿ_Type.tp_repr = ಕಿಟಕಿ_Repr;
    ಕಿಟಕಿ_Type.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    ಕಿಟಕಿ_Type.tp_doc = "vim Window object";
    ಕಿಟಕಿ_Type.tp_methods = ಕಿಟಕಿ_Methods;
    ಕಿಟಕಿ_Type.tp_traverse = ಕಿಟಕಿ_Traverse;
    ಕಿಟಕಿ_Type.tp_clear = ಕಿಟಕಿ_Clear;
#ಆದರೆ PY_MAJOR_ಆವೃತ್ತಿ >= 3
    ಕಿಟಕಿ_Type.tp_getattro = ಕಿಟಕಿ_Getattro;
    ಕಿಟಕಿ_Type.tp_setattro = ಕಿಟಕಿ_Setattro;
    ಕಿಟಕಿ_Type.tp_alloc = call_PyType_GenericAlloc;
    ಕಿಟಕಿ_Type.tp_new = call_PyType_GenericNew;
    ಕಿಟಕಿ_Type.tp_free = call_PyObject_Free;
#ಇಲ್ಲದಿದ್ದರೆ
    ಕಿಟಕಿ_Type.tp_getattr = ಕಿಟಕಿ_Getattr;
    ಕಿಟಕಿ_Type.tp_setattr = ಕಿಟಕಿ_Setattr;
#ಆದರೆ_ಅಂತ್ಯ

    CLEAR_ಕ್ಷೇತ್ರ(Tab_ಪುಟ_Type);
    Tab_ಪುಟ_Type.tp_name = "vim.tabpage";
    Tab_ಪುಟ_Type.tp_basicsize = ಗಾತ್ರ(Tab_ಪುಟ_Object);
    Tab_ಪುಟ_Type.tp_dealloc = Tab_ಪುಟ_Destructor;
    Tab_ಪುಟ_Type.tp_repr = Tab_ಪುಟ_Repr;
    Tab_ಪುಟ_Type.tp_flags = Py_TPFLAGS_DEFAULT;
    Tab_ಪುಟ_Type.tp_doc = "vim tab page object";
    Tab_ಪುಟ_Type.tp_methods = Tab_ಪುಟ_Methods;
#ಆದರೆ PY_MAJOR_ಆವೃತ್ತಿ >= 3
    Tab_ಪುಟ_Type.tp_getattro = Tab_ಪುಟ_Getattro;
    Tab_ಪುಟ_Type.tp_alloc = call_PyType_GenericAlloc;
    Tab_ಪುಟ_Type.tp_new = call_PyType_GenericNew;
    Tab_ಪುಟ_Type.tp_free = call_PyObject_Free;
#ಇಲ್ಲದಿದ್ದರೆ
    Tab_ಪುಟ_Type.tp_getattr = Tab_ಪುಟ_Getattr;
#ಆದರೆ_ಅಂತ್ಯ

    CLEAR_ಕ್ಷೇತ್ರ(BufMapType);
    BufMapType.tp_name = "vim.bufferlist";
    BufMapType.tp_basicsize = ಗಾತ್ರ(BufMapObject);
    BufMapType.tp_as_mapping = &BufMapAsMapping;
    BufMapType.tp_flags = Py_TPFLAGS_DEFAULT;
    BufMapType.tp_iter = BufMapIter;
    BufMapType.tp_doc = "vim buffer list";

    CLEAR_ಕ್ಷೇತ್ರ(WinListType);
    WinListType.tp_name = "vim.windowlist";
    WinListType.tp_basicsize = ಗಾತ್ರ(WinListObject);
    WinListType.tp_as_sequence = &WinListAsSeq;
    WinListType.tp_flags = Py_TPFLAGS_DEFAULT;
    WinListType.tp_doc = "vim window list";
    WinListType.tp_dealloc = (destructor)WinListDestructor;

    CLEAR_ಕ್ಷೇತ್ರ(TabListType);
    TabListType.tp_name = "vim.tabpagelist";
    TabListType.tp_basicsize = ಗಾತ್ರ(TabListObject);
    TabListType.tp_as_sequence = &TabListAsSeq;
    TabListType.tp_flags = Py_TPFLAGS_DEFAULT;
    TabListType.tp_doc = "vim tab page list";

    CLEAR_ಕ್ಷೇತ್ರ(RangeType);
    RangeType.tp_name = "vim.range";
    RangeType.tp_basicsize = ಗಾತ್ರ(RangeObject);
    RangeType.tp_dealloc = RangeDestructor;
    RangeType.tp_repr = RangeRepr;
    RangeType.tp_as_sequence = &RangeAsSeq;
    RangeType.tp_as_mapping = &RangeAsMapping;
    RangeType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    RangeType.tp_doc = "vim Range object";
    RangeType.tp_methods = RangeMethods;
    RangeType.tp_traverse = RangeTraverse;
    RangeType.tp_clear = RangeClear;
#ಆದರೆ PY_MAJOR_ಆವೃತ್ತಿ >= 3
    RangeType.tp_getattro = RangeGetattro;
    RangeType.tp_alloc = call_PyType_GenericAlloc;
    RangeType.tp_new = call_PyType_GenericNew;
    RangeType.tp_free = call_PyObject_Free;
#ಇಲ್ಲದಿದ್ದರೆ
    RangeType.tp_getattr = RangeGetattr;
#ಆದರೆ_ಅಂತ್ಯ

    CLEAR_ಕ್ಷೇತ್ರ(CurrentType);
    CurrentType.tp_name = "vim.currentdata";
    CurrentType.tp_basicsize = ಗಾತ್ರ(CurrentObject);
    CurrentType.tp_flags = Py_TPFLAGS_DEFAULT;
    CurrentType.tp_doc = "vim current object";
    CurrentType.tp_methods = CurrentMethods;
#ಆದರೆ PY_MAJOR_ಆವೃತ್ತಿ >= 3
    CurrentType.tp_getattro = CurrentGetattro;
    CurrentType.tp_setattro = CurrentSetattro;
#ಇಲ್ಲದಿದ್ದರೆ
    CurrentType.tp_getattr = CurrentGetattr;
    CurrentType.tp_setattr = CurrentSetattr;
#ಆದರೆ_ಅಂತ್ಯ

    CLEAR_ಕ್ಷೇತ್ರ(DictionaryType);
    DictionaryType.tp_name = "vim.dictionary";
    DictionaryType.tp_basicsize = ಗಾತ್ರ(DictionaryObject);
    DictionaryType.tp_dealloc = DictionaryDestructor;
    DictionaryType.tp_as_sequence = &DictionaryAsSeq;
    DictionaryType.tp_as_mapping = &DictionaryAsMapping;
    DictionaryType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE;
    DictionaryType.tp_doc = "dictionary pushing modifications to Vim structure";
    DictionaryType.tp_methods = DictionaryMethods;
    DictionaryType.tp_iter = DictionaryIter;
    DictionaryType.tp_new = DictionaryConstructor;
    DictionaryType.tp_alloc = PyType_GenericAlloc;
#ಆದರೆ PY_MAJOR_ಆವೃತ್ತಿ >= 3
    DictionaryType.tp_getattro = DictionaryGetattro;
    DictionaryType.tp_setattro = DictionarySetattro;
#ಇಲ್ಲದಿದ್ದರೆ
    DictionaryType.tp_getattr = DictionaryGetattr;
    DictionaryType.tp_setattr = DictionarySetattr;
#ಆದರೆ_ಅಂತ್ಯ

    CLEAR_ಕ್ಷೇತ್ರ(ListType);
    ListType.tp_name = "vim.list";
    ListType.tp_dealloc = ListDestructor;
    ListType.tp_basicsize = ಗಾತ್ರ(ListObject);
    ListType.tp_as_sequence = &ListAsSeq;
    ListType.tp_as_mapping = &ListAsMapping;
    ListType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE;
    ListType.tp_doc = "list pushing modifications to Vim structure";
    ListType.tp_methods = ListMethods;
    ListType.tp_iter = ListIter;
    ListType.tp_new = ListConstructor;
    ListType.tp_alloc = PyType_GenericAlloc;
#ಆದರೆ PY_MAJOR_ಆವೃತ್ತಿ >= 3
    ListType.tp_getattro = ListGetattro;
    ListType.tp_setattro = ListSetattro;
#ಇಲ್ಲದಿದ್ದರೆ
    ListType.tp_getattr = ListGetattr;
    ListType.tp_setattr = ListSetattr;
#ಆದರೆ_ಅಂತ್ಯ

    // Tuple type
    CLEAR_ಕ್ಷೇತ್ರ(TupleType);
    TupleType.tp_name = "vim.tuple";
    TupleType.tp_dealloc = TupleDestructor;
    TupleType.tp_basicsize = ಗಾತ್ರ(TupleObject);
    TupleType.tp_as_sequence = &TupleAsSeq;
    TupleType.tp_as_mapping = &TupleAsMapping;
    TupleType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE;
    TupleType.tp_doc = "tuple pushing modifications to Vim structure";
    TupleType.tp_methods = TupleMethods;
    TupleType.tp_iter = TupleIter;
    TupleType.tp_new = TupleConstructor;
    TupleType.tp_alloc = PyType_GenericAlloc;
#ಆದರೆ PY_MAJOR_ಆವೃತ್ತಿ >= 3
    TupleType.tp_getattro = TupleGetattro;
    TupleType.tp_setattro = TupleSetattro;
#ಇಲ್ಲದಿದ್ದರೆ
    TupleType.tp_getattr = TupleGetattr;
    TupleType.tp_setattr = TupleSetattr;
#ಆದರೆ_ಅಂತ್ಯ

    CLEAR_ಕ್ಷೇತ್ರ(FunctionType);
    FunctionType.tp_name = "vim.function";
    FunctionType.tp_basicsize = ಗಾತ್ರ(FunctionObject);
    FunctionType.tp_dealloc = FunctionDestructor;
    FunctionType.tp_call = FunctionCall;
    FunctionType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE;
    FunctionType.tp_doc = "object that calls Vim function";
    FunctionType.tp_methods = FunctionMethods;
    FunctionType.tp_repr = FunctionRepr;
    FunctionType.tp_new = FunctionConstructor;
    FunctionType.tp_alloc = PyType_GenericAlloc;
#ಆದರೆ PY_MAJOR_ಆವೃತ್ತಿ >= 3
    FunctionType.tp_getattro = FunctionGetattro;
#ಇಲ್ಲದಿದ್ದರೆ
    FunctionType.tp_getattr = FunctionGetattr;
#ಆದರೆ_ಅಂತ್ಯ

    CLEAR_ಕ್ಷೇತ್ರ(OptionsType);
    OptionsType.tp_name = "vim.options";
    OptionsType.tp_basicsize = ಗಾತ್ರ(OptionsObject);
    OptionsType.tp_as_sequence = &OptionsAsSeq;
    OptionsType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    OptionsType.tp_doc = "object for manipulating options";
    OptionsType.tp_iter = OptionsIter;
    OptionsType.tp_as_mapping = &OptionsAsMapping;
    OptionsType.tp_dealloc = OptionsDestructor;
    OptionsType.tp_traverse = OptionsTraverse;
    OptionsType.tp_clear = OptionsClear;

#ಆದರೆ PY_ಆವೃತ್ತಿ_HEX < 0x030700f0
    CLEAR_ಕ್ಷೇತ್ರ(LoaderType);
    LoaderType.tp_name = "vim.Loader";
    LoaderType.tp_basicsize = ಗಾತ್ರ(LoaderObject);
    LoaderType.tp_flags = Py_TPFLAGS_DEFAULT;
    LoaderType.tp_doc = "vim message object";
    LoaderType.tp_methods = LoaderMethods;
    LoaderType.tp_dealloc = LoaderDestructor;
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ PY_MAJOR_ಆವೃತ್ತಿ >= 3
    CLEAR_ಕ್ಷೇತ್ರ(vimmodule);
    vimmodule.m_name = "vim";
    vimmodule.m_doc = "Vim Python interface\n";
    vimmodule.m_ಗಾತ್ರ_2 = -1;
    vimmodule.m_methods = VimMethods;
#ಆದರೆ_ಅಂತ್ಯ
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
init_types(ಶೂನ್ಯ)
{
    PYTYPE_READY(IterType);
    PYTYPE_READY(ಬಫರ್_Type);
    PYTYPE_READY(RangeType);
    PYTYPE_READY(ಕಿಟಕಿ_Type);
    PYTYPE_READY(Tab_ಪುಟ_Type);
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
#ಆದರೆ PY_ಆವೃತ್ತಿ_HEX < 0x030700f0
    PYTYPE_READY(LoaderType);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_LIMITED_API
    // We need to finish initializing all the static objects because the types
    // are only just allocated on the heap now.
    // Each PyObject_HEAD_INIT_TYPE should correspond to a
    // PyObject_FINISH_INIT_TYPE below.
    Py_Object_ಮುಕ್ತಾಯ_INIT_TYPE(Output, OutputType);
    Py_Object_ಮುಕ್ತಾಯ_INIT_TYPE(Error, OutputType);
    Py_Object_ಮುಕ್ತಾಯ_INIT_TYPE(The_ಬಫರ್_Map, BufMapType);
    Py_Object_ಮುಕ್ತಾಯ_INIT_TYPE(The_ಕಿಟಕಿ_List, WinListType);
    Py_Object_ಮುಕ್ತಾಯ_INIT_TYPE(TheCurrent, CurrentType);
    Py_Object_ಮುಕ್ತಾಯ_INIT_TYPE(The_Tab_ಪುಟ_List, TabListType);
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ 0;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_LIMITED_API
    ಸ್ಥಿರ ಶೂನ್ಯ
shutdown_types(ಶೂನ್ಯ)
{
    PYTYPE_CLEANUP(IterType);
    PYTYPE_CLEANUP(ಬಫರ್_Type);
    PYTYPE_CLEANUP(RangeType);
    PYTYPE_CLEANUP(ಕಿಟಕಿ_Type);
    PYTYPE_CLEANUP(Tab_ಪುಟ_Type);
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
# ಆದರೆ PY_ಆವೃತ್ತಿ_HEX < 0x030700f0
    PYTYPE_CLEANUP(LoaderType);
# endif
}
#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
init_sys_ಮಾರ್ಗ(ಶೂನ್ಯ)
{
    PyObject	*ಮಾರ್ಗ;
    PyObject	*ಮಾರ್ಗ_hook;
    PyObject	*ಮಾರ್ಗ_hooks;

    ಆದರೆ (!(ಮಾರ್ಗ_hook = PyObject_GetAttrString(vim_module, "path_hook")))
	ಹಿಂತಿರುಗಿ -1;

    ಆದರೆ (!(ಮಾರ್ಗ_hooks = PySys_GetObject("path_hooks")))
    {
	PyErr_Clear();
	ಮಾರ್ಗ_hooks = PyList_New(1);
	Py_List_ಗಣ_ಅಂಶ(ಮಾರ್ಗ_hooks, 0, ಮಾರ್ಗ_hook);
	ಆದರೆ (Py_Sys_ಗಣ_Object("path_hooks", ಮಾರ್ಗ_hooks))
	{
	    Py_DECREF(ಮಾರ್ಗ_hooks);
	    ಹಿಂತಿರುಗಿ -1;
	}
	Py_DECREF(ಮಾರ್ಗ_hooks);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (PyList_Check(ಮಾರ್ಗ_hooks))
    {
	ಆದರೆ (PyList_Append(ಮಾರ್ಗ_hooks, ಮಾರ್ಗ_hook))
	{
	    Py_DECREF(ಮಾರ್ಗ_hook);
	    ಹಿಂತಿರುಗಿ -1;
	}
	Py_DECREF(ಮಾರ್ಗ_hook);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	Vim_Try_ಆರಂಭ();
	emsg(_("Failed to set path hook: sys.path_hooks is not a list\n"
	       "You should now do the following:\n"
	       "- append vim.path_hook to sys.path_hooks\n"
	       "- append vim.VIM_SPECIAL_PATH to sys.path\n"));
	Vim_Try_ಕೊನೆ(); // Discard the error
	Py_DECREF(ಮಾರ್ಗ_hook);
	ಹಿಂತಿರುಗಿ 0;
    }

    ಆದರೆ (!(ಮಾರ್ಗ = PySys_GetObject("path")))
    {
	PyErr_Clear();
	ಮಾರ್ಗ = PyList_New(1);
	Py_INCREF(vim_special_ಮಾರ್ಗ_object);
	Py_List_ಗಣ_ಅಂಶ(ಮಾರ್ಗ, 0, vim_special_ಮಾರ್ಗ_object);
	ಆದರೆ (Py_Sys_ಗಣ_Object("path", ಮಾರ್ಗ))
	{
	    Py_DECREF(ಮಾರ್ಗ);
	    ಹಿಂತಿರುಗಿ -1;
	}
	Py_DECREF(ಮಾರ್ಗ);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (PyList_Check(ಮಾರ್ಗ))
    {
	ಆದರೆ (PyList_Append(ಮಾರ್ಗ, vim_special_ಮಾರ್ಗ_object))
	    ಹಿಂತಿರುಗಿ -1;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	Vim_Try_ಆರಂಭ();
	emsg(_("Failed to set path: sys.path is not a list\n"
	       "You should now append vim.VIM_SPECIAL_PATH to sys.path"));
	Vim_Try_ಕೊನೆ(); // Discard the error
    }

    ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ರಚನೆ numeric_constant {
    ಅಕ್ಷರ	*name;
    ಪೂರ್ಣಾಂಕ		val;
} numeric_constants[] = {
    {"VAR_LOCKED",	VAR_LOCKED},
    {"VAR_FIXED",	VAR_FIXED},
    {"VAR_SCOPE",	VAR_SCOPE},
    {"VAR_DEF_SCOPE",	VAR_DEF_SCOPE},
};

ರಚನೆ object_constant {
    ಅಕ್ಷರ	*name;
    PyObject	*valObject;
};

#ವ್ಯಾಖ್ಯಾನಿಸು ಕೂಡಿಸು_OBJECT(m, name, obj) \
    ಆದರೆ (Py_Module_ಕೂಡಿಸು_Object(m, name, obj)) \
	ಹಿಂತಿರುಗಿ -1;

#ವ್ಯಾಖ್ಯಾನಿಸು ಕೂಡಿಸು_CHECKED_OBJECT(m, name, obj) \
    { \
	PyObject	*valObject = obj; \
	ಆದರೆ (!valObject) \
	    ಹಿಂತಿರುಗಿ -1; \
	ಕೂಡಿಸು_OBJECT(m, name, valObject); \
    }

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
populate_module(PyObject *m)
{
    ಪೂರ್ಣಾಂಕ		i;
    PyObject	*other_module;
    PyObject	*attr;
    PyObject	*imp;
#ಆದರೆ PY_ಆವೃತ್ತಿ_HEX >= 0x030700f0
    PyObject	*dict;
    PyObject	*cls;
#ಆದರೆ_ಅಂತ್ಯ

    ಗಾಗಿ (i = 0; i < (ಪೂರ್ಣಾಂಕ)(ಗಾತ್ರ(numeric_constants)
					   / ಗಾತ್ರ(ರಚನೆ numeric_constant));
	    ++i)
	ಕೂಡಿಸು_CHECKED_OBJECT(m, numeric_constants[i].name,
		PyInt_FromLong(numeric_constants[i].val));

    ರಚನೆ object_constant object_constants[] = {
	{"buffers",  (PyObject *)(ಶೂನ್ಯ *)&The_ಬಫರ್_Map},
	{"windows",  (PyObject *)(ಶೂನ್ಯ *)&The_ಕಿಟಕಿ_List},
	{"tabpages", (PyObject *)(ಶೂನ್ಯ *)&The_Tab_ಪುಟ_List},
	{"current",  (PyObject *)(ಶೂನ್ಯ *)&TheCurrent},

	{"Buffer",     (PyObject *)ಬಫರ್_Type_Ptr},
	{"Range",      (PyObject *)RangeTypePtr},
	{"Window",     (PyObject *)ಕಿಟಕಿ_Type_Ptr},
	{"TabPage",    (PyObject *)Tab_ಪುಟ_Type_Ptr},
	{"Dictionary", (PyObject *)DictionaryTypePtr},
	{"List",       (PyObject *)ListTypePtr},
	{"Tuple",      (PyObject *)TupleTypePtr},
	{"Function",   (PyObject *)FunctionTypePtr},
	{"Options",    (PyObject *)OptionsTypePtr},
#ಆದರೆ PY_ಆವೃತ್ತಿ_HEX < 0x030700f0
	{"_Loader",    (PyObject *)LoaderTypePtr},
#ಆದರೆ_ಅಂತ್ಯ
    };

    ಗಾಗಿ (i = 0; i < (ಪೂರ್ಣಾಂಕ)(ಗಾತ್ರ(object_constants)
					    / ಗಾತ್ರ(ರಚನೆ object_constant));
	    ++i)
    {
	PyObject	*valObject;

	valObject = object_constants[i].valObject;
	Py_INCREF(valObject);
	ಕೂಡಿಸು_OBJECT(m, object_constants[i].name, valObject);
    }

    ಆದರೆ (!(VimError = PyErr_NewException("vim.error", ಖಾಲಿ, ಖಾಲಿ)))
	ಹಿಂತಿರುಗಿ -1;
    ಕೂಡಿಸು_OBJECT(m, "error", VimError);

    ಕೂಡಿಸು_CHECKED_OBJECT(m, "vars",  NEW_DICTIONARY(get_globvar_dict()));
    ಕೂಡಿಸು_CHECKED_OBJECT(m, "vvars", NEW_DICTIONARY(get_vimvar_dict()));
    ಕೂಡಿಸು_CHECKED_OBJECT(m, "options",
	    OptionsNew(SREQ_GLOBAL, ಖಾಲಿ, dummy_check, ಖಾಲಿ));

    ಆದರೆ (!(other_module = PyImport_ImportModule("os")))
	ಹಿಂತಿರುಗಿ -1;
    ಕೂಡಿಸು_OBJECT(m, "os", other_module);

#ಆದರೆ PY_MAJOR_ಆವೃತ್ತಿ >= 3
    ಆದರೆ (!(py_getcwd = PyObject_GetAttrString(other_module, "getcwd")))
	ಹಿಂತಿರುಗಿ -1;
#ಇಲ್ಲದಿದ್ದರೆ
    ಆದರೆ (!(py_getcwd = PyObject_GetAttrString(other_module, "getcwdu")))
	ಹಿಂತಿರುಗಿ -1;
#ಆದರೆ_ಅಂತ್ಯ
    ಕೂಡಿಸು_OBJECT(m, "_getcwd", py_getcwd)

    ಆದರೆ (!(py_chdir = PyObject_GetAttrString(other_module, "chdir")))
	ಹಿಂತಿರುಗಿ -1;
    ಕೂಡಿಸು_OBJECT(m, "_chdir", py_chdir);
    ಆದರೆ (!(attr = PyObject_GetAttrString(m, "chdir")))
	ಹಿಂತಿರುಗಿ -1;
    ಆದರೆ (Py_Object_ಗಣ_Attr_String(other_module, "chdir", attr))
    {
	Py_DECREF(attr);
	ಹಿಂತಿರುಗಿ -1;
    }
    Py_DECREF(attr);

    ಆದರೆ ((py_fchdir = PyObject_GetAttrString(other_module, "fchdir")))
    {
	ಕೂಡಿಸು_OBJECT(m, "_fchdir", py_fchdir);
	ಆದರೆ (!(attr = PyObject_GetAttrString(m, "fchdir")))
	    ಹಿಂತಿರುಗಿ -1;
	ಆದರೆ (Py_Object_ಗಣ_Attr_String(other_module, "fchdir", attr))
	{
	    Py_DECREF(attr);
	    ಹಿಂತಿರುಗಿ -1;
	}
	Py_DECREF(attr);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	PyErr_Clear();

    ಆದರೆ (!(vim_special_ಮಾರ್ಗ_object = PyString_FromString(vim_special_ಮಾರ್ಗ)))
	ಹಿಂತಿರುಗಿ -1;

    ಕೂಡಿಸು_OBJECT(m, "VIM_SPECIAL_PATH", vim_special_ಮಾರ್ಗ_object);

#ಆದರೆ PY_ಆವೃತ್ತಿ_HEX >= 0x030700f0
    ಆದರೆ (!(imp = PyImport_ImportModule("importlib.machinery")))
	ಹಿಂತಿರುಗಿ -1;

    dict = PyModule_GetDict(imp);

    ಆದರೆ (!(cls = Py_Dict_Get_ಅಂಶ_String(dict, "PathFinder")))
    {
	Py_DECREF(imp);
	ಹಿಂತಿರುಗಿ -1;
    }

    ಆದರೆ (!(py_ಕಂಡುಹಿಡಿ_spec = PyObject_GetAttrString(cls, "find_spec")))
    {
	Py_DECREF(imp);
	ಹಿಂತಿರುಗಿ -1;
    }

# ಆದರೆ PY_ಆವೃತ್ತಿ_HEX < 0x30c00a7
    // find_module has been removed as of Python 3.12.0a7
    ಆದರೆ ((py_ಕಂಡುಹಿಡಿ_module = PyObject_GetAttrString(cls, "find_module")))
	ಕೂಡಿಸು_OBJECT(m, "_find_module", py_ಕಂಡುಹಿಡಿ_module);
# endif

    Py_DECREF(imp);

    ಕೂಡಿಸು_OBJECT(m, "_find_spec", py_ಕಂಡುಹಿಡಿ_spec);
#ಇಲ್ಲದಿದ್ದರೆ
    ಆದರೆ (!(imp = PyImport_ImportModule("imp")))
	ಹಿಂತಿರುಗಿ -1;

    ಆದರೆ (!(py_ಕಂಡುಹಿಡಿ_module = PyObject_GetAttrString(imp, "find_module")))
    {
	Py_DECREF(imp);
	ಹಿಂತಿರುಗಿ -1;
    }

    ಆದರೆ (!(py_ತುಂಬು_module = PyObject_GetAttrString(imp, "load_module")))
    {
	Py_DECREF(py_ಕಂಡುಹಿಡಿ_module);
	Py_DECREF(imp);
	ಹಿಂತಿರುಗಿ -1;
    }

    Py_DECREF(imp);

    ಕೂಡಿಸು_OBJECT(m, "_find_module", py_ಕಂಡುಹಿಡಿ_module);
    ಕೂಡಿಸು_OBJECT(m, "_load_module", py_ತುಂಬು_module);
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ 0;
}
