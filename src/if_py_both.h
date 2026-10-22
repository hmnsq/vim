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

#ಯದಿ PY_VERSION_HEX < 0x02050000
ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ Py_ssize_t;  // Python 2.4 and earlier don't have this type.
#endif

// Use values that are known to work, others may make Vim crash.
#define ENC_OPT (enc_utf8 ? "utf-8" : enc_dbcs ? "euc-jp" : (ವರ್ಣ *)p_enc)
#define DOPY_FUNC "_vim_pydo"

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *vim_special_path = "_vim_path_";

#define PyErr_SET_STRING(exc, str) PyErr_SetString(exc, _(str))
#define PyErr_SetVim(str) PyErr_SetString(VimError, str)
#define PyErr_SET_VIM(str) PyErr_SET_STRING(VimError, str)
#define PyErr_FORMAT(exc, str, arg) PyErr_Format(exc, _(str), arg)
#define PyErr_FORMAT2(exc, str, arg1, arg2) PyErr_Format(exc, _(str), arg1,arg2)
#define PyErr_VIM_FORMAT(str, arg) PyErr_FORMAT(VimError, str, arg)

#ifdef USE_LIMITED_API
// Limited Python API. Need to call only exposed functions and remap macros.
// PyTypeObject is an opaque struct.

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    lenfunc sq_length;
    binaryfunc sq_concat;
    ssizeargfunc sq_repeat;
    ssizeargfunc sq_item;
    ಶೂನ್ಯ *was_sq_slice;
    ssizeobjargproc sq_ass_item;
    ಶೂನ್ಯ *was_sq_ass_slice;
    objobjproc sq_contains;

    binaryfunc sq_inplace_concat;
    ssizeargfunc sq_inplace_repeat;
} PySequenceMethods;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    lenfunc mp_length;
    binaryfunc mp_subscript;
    objobjargproc mp_ass_subscript;
} PyMappingMethods;

// This struct emulates the concrete _typeobject struct to allow the code to
// work the same way in both limited and full Python APIs.
ಸಂರಚನಾ typeobject_wrapper {
    ಧ್ರುವ ವರ್ಣ *tp_name;
    Py_ssize_t tp_basicsize;
    unsigned long tp_flags;

    // When adding new slots below, also need to make sure we add ADD_TP_SLOT
    // call in AddHeapType for it.

    destructor tp_dealloc;
    reprfunc tp_repr;

    PySequenceMethods *tp_as_sequence;
    PyMappingMethods *tp_as_mapping;

    ternaryfunc tp_call;
    getattrofunc tp_getattro;
    setattrofunc tp_setattro;

    ಧ್ರುವ ವರ್ಣ *tp_doc;

    traverseproc tp_traverse;

    inquiry tp_clear;

    getiterfunc tp_iter;
    iternextfunc tp_iternext;

    ಸಂರಚನಾ PyMethodDef *tp_methods;
    ಸಂರಚನಾ _typeobject *tp_base;
    allocfunc tp_alloc;
    newfunc tp_new;
    freefunc tp_free;
};

# define DEFINE_PY_TYPE_OBJECT(type) \
    ಸ್ಥಿರ ಸಂರಚನಾ typeobject_wrapper type; \
    ಸ್ಥಿರ PyTypeObject* type##Ptr = ರಿಕ್ತ

// PyObject_HEAD_INIT_TYPE and PyObject_FINISH_INIT_TYPE need to come in pairs
// We first initialize with NULL because the type is not allocated until
// init_types() is called later. It's in FINISH_INIT_TYPE where we fill the
// type in with the newly allocated type.
# define PyObject_HEAD_INIT_TYPE(type) PyObject_HEAD_INIT(ರಿಕ್ತ)
# define PyObject_FINISH_INIT_TYPE(obj, type) obj.ob_base.ob_type = type##Ptr

# define Py_TYPE_GET_TP_ALLOC(type) ((allocfunc)PyType_GetSlot(type, Py_tp_alloc))
# define Py_TYPE_GET_TP_METHODS(type) ((PyMethodDef *)PyType_GetSlot(type, Py_tp_methods))

// PyObject_NEW is not part of stable ABI, but PyObject_Malloc/Init are.
ಸ್ಥಿರ PyObject* Vim_PyObject_New(PyTypeObject *type, size_t objsize)
{
    PyObject *obj = (PyObject *)PyObject_Malloc(objsize);
    ಯದಿ (obj == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ PyErr_NoMemory();
    ಪ್ರತ್ಯಾಗಮ PyObject_Init(obj, type);
}
# undef PyObject_NEW
# define PyObject_NEW(type, typeobj) ((type *)Vim_PyObject_New(typeobj, ಪರಿಮಾಣ(type)))

// This is a somewhat convoluted because limited API doesn't expose an easy way
// to get the tp_name field, and so we have to manually reconstruct it as
// "__module__.__name__" (with __module__ omitted for builtins to emulate
// Python behavior). Also, some of the more convenient functions like
// PyUnicode_AsUTF8AndSize and PyType_GetQualName() are not available until
// late Python 3 versions, and won't be available if you set Py_LIMITED_API too
// low.
# define PyErr_FORMAT_TYPE(ಸಂದೇಶ, obj) \
    ಕುರು { \
	PyObject* qualname = PyObject_GetAttrString((PyObject*)(obj)->ob_type, "__qualname__"); \
	ಯದಿ (qualname == ರಿಕ್ತ) \
	{ \
	    PyErr_FORMAT(PyExc_TypeError, ಸಂದೇಶ, "(NULL)"); \
	    ಭಙ್ಗ; \
	} \
	PyObject* module = PyObject_GetAttrString((PyObject*)(obj)->ob_type, "__module__"); \
	PyObject* full; \
	ಯದಿ (module == ರಿಕ್ತ || PyUnicode_CompareWithASCIIString(module, "builtins") == 0 \
		|| PyUnicode_CompareWithASCIIString(module, "__main__") == 0) \
	{ \
	    full = qualname; \
	    Py_INCREF(full); \
	} \
	ಅನ್ಯಥಾ \
	    full = PyUnicode_FromFormat("%U.%U", module, qualname); \
	PyObject* full_bytes = PyUnicode_AsUTF8String(full); \
	ಧ್ರುವ ವರ್ಣ* full_str = PyBytes_AsString(full_bytes); \
	full_str = full_str == ರಿಕ್ತ ? "(NULL)" : full_str; \
	PyErr_FORMAT(PyExc_TypeError, ಸಂದೇಶ, full_str); \
	Py_DECREF(qualname); \
	Py_XDECREF(module); \
	Py_XDECREF(full); \
	Py_XDECREF(full_bytes); \
    } ಯಾವತ್ (0)

# define PyList_GET_ITEM(list, i) PyList_GetItem(list, i)
# define PyList_GET_SIZE(o) PyList_Size(o)

// PyList_SET_ITEM and PyList_SetItem have slightly different behaviors. The
// former will leave the old item dangling, and the latter will decref on it.
// Since we only use this on new lists, this difference doesn't matter.
# define PyList_SET_ITEM(list, i, item) PyList_SetItem(list, i, item)

# define PyTuple_GET_ITEM(o, pos) PyTuple_GetItem(o, pos)
# define PyTuple_GET_SIZE(o) PyTuple_Size(o)

# ಯದಿ Py_LIMITED_API < 0x03080000
// PyIter_check only became part of stable ABI in 3.8, and there is no easy way
// to check for it in the API. We simply return false as a compromise. This
// does mean we should avoid compiling with stable ABI < 3.8.
#  undef PyIter_Check
#  define PyIter_Check(obj) (FALSE)
# endif

ಸ್ಥಿರ PyTypeObject* AddHeapType(ಸಂರಚನಾ typeobject_wrapper* type_object)
{
    PyType_Spec type_spec;
    type_spec.name = type_object->tp_name;
    type_spec.basicsize = type_object->tp_basicsize;
    type_spec.itemsize = 0;
    type_spec.flags = type_object->tp_flags;

    // We just need to statically allocate a large enough buffer that can hold
    // all slots. We need to leave a null-terminated slot at the end.
    PyType_Slot slots[40] = { {0, ರಿಕ್ತ} };
    size_t slot_i = 0;

# define ADD_TP_SLOT(slot_name) \
    ಯದಿ (slot_i >= 40) ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ; /* this should never happen */ \
    ಯದಿ (type_object->slot_name != ರಿಕ್ತ) \
    { \
	slots[slot_i].slot = Py_##slot_name; \
	slots[slot_i].pfunc = (ಶೂನ್ಯ*)type_object->slot_name; \
	++slot_i; \
    }
# define ADD_TP_SUB_SLOT(sub_slot, slot_name) \
    ಯದಿ (slot_i >= 40) ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ; /* this should never happen */ \
    ಯದಿ (type_object->sub_slot != ರಿಕ್ತ && type_object->sub_slot->slot_name != ರಿಕ್ತ) \
    { \
	slots[slot_i].slot = Py_##slot_name; \
	slots[slot_i].pfunc = (ಶೂನ್ಯ*)type_object->sub_slot->slot_name; \
	++slot_i; \
    }

    ADD_TP_SLOT(tp_dealloc)
    ADD_TP_SLOT(tp_repr)
    ADD_TP_SLOT(tp_call)
    ADD_TP_SLOT(tp_getattro)
    ADD_TP_SLOT(tp_setattro)
    ADD_TP_SLOT(tp_doc)
    ADD_TP_SLOT(tp_traverse)
    ADD_TP_SLOT(tp_clear)
    ADD_TP_SLOT(tp_iter)
    ADD_TP_SLOT(tp_iternext)
    ADD_TP_SLOT(tp_methods)
    ADD_TP_SLOT(tp_base)
    ADD_TP_SLOT(tp_alloc)
    ADD_TP_SLOT(tp_new)
    ADD_TP_SLOT(tp_free)

    ADD_TP_SUB_SLOT(tp_as_sequence, sq_length)
    ADD_TP_SUB_SLOT(tp_as_sequence, sq_concat)
    ADD_TP_SUB_SLOT(tp_as_sequence, sq_repeat)
    ADD_TP_SUB_SLOT(tp_as_sequence, sq_item)
    ADD_TP_SUB_SLOT(tp_as_sequence, sq_ass_item)
    ADD_TP_SUB_SLOT(tp_as_sequence, sq_contains)
    ADD_TP_SUB_SLOT(tp_as_sequence, sq_inplace_concat)
    ADD_TP_SUB_SLOT(tp_as_sequence, sq_inplace_repeat)

    ADD_TP_SUB_SLOT(tp_as_mapping, mp_length)
    ADD_TP_SUB_SLOT(tp_as_mapping, mp_subscript)
    ADD_TP_SUB_SLOT(tp_as_mapping, mp_ass_subscript)
# undef ADD_TP_SLOT
# undef ADD_TP_SUB_SLOT

    type_spec.slots = slots;

    PyObject* newtype = PyType_FromSpec(&type_spec);
    ಪ್ರತ್ಯಾಗಮ (PyTypeObject*)newtype;
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
    type##Ptr = AddHeapType(&(type)); \
    ಯದಿ (type##Ptr == ರಿಕ್ತ) \
	ಪ್ರತ್ಯಾಗಮ -1;
# define PYTYPE_CLEANUP(type) \
    type##Ptr = ರಿಕ್ತ;

// Limited API does not provide PyRun_* functions. Need to implement manually
// using PyCompile and PyEval.
ಸ್ಥಿರ PyObject* Vim_PyRun_String(ಧ್ರುವ ವರ್ಣ *str, ಪೂರ್ಣಾಙ್ಕ start, PyObject *globals, PyObject *locals)
{
    // Just pass "" for filename for now.
    PyObject* compiled = Py_CompileString(str, "", start);
    ಯದಿ (compiled == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    PyObject* eval_result = PyEval_EvalCode(compiled, globals, locals);
    Py_DECREF(compiled);
    ಪ್ರತ್ಯಾಗಮ eval_result;
}
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ Vim_PyRun_SimpleString(ಧ್ರುವ ವರ್ಣ *str)
{
    // This function emulates CPython's implementation.
    PyObject* m = PyImport_AddModule("__main__");
    ಯದಿ (m == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ -1;
    PyObject* d = PyModule_GetDict(m);
    PyObject* output = Vim_PyRun_String(str, Py_file_ಒಳಹರಿವು, d, d);
    ಯದಿ (output == ರಿಕ್ತ)
    {
	PyErr_PrintEx(TRUE);
	ಪ್ರತ್ಯಾಗಮ -1;
    }
    Py_DECREF(output);
    ಪ್ರತ್ಯಾಗಮ 0;
}
# define PyRun_String Vim_PyRun_String
# define PyRun_SimpleString Vim_PyRun_SimpleString

#ಅನ್ಯಥಾ // !defined(USE_LIMITED_API)

// Full Python API. Can make use of structs and macros directly.
# define DEFINE_PY_TYPE_OBJECT(type) \
    ಸ್ಥಿರ PyTypeObject type; \
    ಸ್ಥಿರ PyTypeObject* type##Ptr = &type
# define PyObject_HEAD_INIT_TYPE(type) PyObject_HEAD_INIT(&type)

# define Py_TYPE_GET_TP_ALLOC(type) type->tp_alloc
# define Py_TYPE_GET_TP_METHODS(type) type->tp_methods

# define Py_TYPE_NAME(obj) ((obj)->ob_type->tp_name == ರಿಕ್ತ \
	? "(NULL)" \
	: (obj)->ob_type->tp_name)
# define PyErr_FORMAT_TYPE(ಸಂದೇಶ, obj) \
    PyErr_FORMAT(PyExc_TypeError, ಸಂದೇಶ, \
	    Py_TYPE_NAME(obj))

// Add a static type
# define PYTYPE_READY(type) \
    ಯದಿ (PyType_Ready(type##Ptr)) \
	ಪ್ರತ್ಯಾಗಮ -1;

#endif


#define RAISE_NO_EMPTY_ಕೀಲಿಗಳು PyErr_SET_STRING(Py_Exc_ಮೌಲ್ಯ_Error, \
					    N_("empty keys are not allowed"))
#define RAISE_LOCKED_DICTIONARY PyErr_SET_VIM(N_("dictionary is locked"))
#define RAISE_LOCKED_LIST PyErr_SET_VIM(N_("list is locked"))
#define RAISE_UNDO_FAIL PyErr_SET_VIM(N_("cannot save undo information"))
#define RAISE_DELETE_ಸಾಲು_FAIL PyErr_SET_VIM(N_("cannot delete line"))
#define RAISE_ಸೇರಿಸು_ಸಾಲು_FAIL PyErr_SET_VIM(N_("cannot insert line"))
#define RAISE_REPLACE_ಸಾಲು_FAIL PyErr_SET_VIM(N_("cannot replace line"))
#define RAISE_ಕೀಲಿ_ADD_FAIL(ಕೀಲಿ) \
    PyErr_VIM_FORMAT(N_("failed to add key '%s' to dictionary"), ಕೀಲಿ)
#define RAISE_INVALID_INDEX_TYPE(idx) \
    PyErr_FORMAT_TYPE(N_("index must be int or slice, not %s"), idx);

#define INVALID_BUFFER_ಮೌಲ್ಯ ((buf_T *)(-1))
#define INVALID_WINDOW_ಮೌಲ್ಯ ((win_T *)(-1))
#define INVALID_TABPAGE_ಮೌಲ್ಯ ((tabpage_T *)(-1))

ಪ್ರಕಾರನಿರ್ದೇಶ ಶೂನ್ಯ (*rangeinitializer)(ಶೂನ್ಯ *);
ಪ್ರಕಾರನಿರ್ದೇಶ ಶೂನ್ಯ (*runner)(ಧ್ರುವ ವರ್ಣ *, dict_T *, ಶೂನ್ಯ *
#ifdef PY_CAN_RECURSE
	, PyGILState_STATE *
#endif
	);

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ConvertFromPyObject(PyObject *, typval_T *);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ _ConvertFromPyObject(PyObject *, typval_T *, PyObject *);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ConvertFromPyMapping(PyObject *, typval_T *);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ConvertFromPySequence(PyObject *, typval_T *);
ಸ್ಥಿರ PyObject *WindowNew(win_T *, tabpage_T *);
ಸ್ಥಿರ PyObject *BufferNew (buf_T *);
ಸ್ಥಿರ PyObject *ಸಾಲು_To_String(ಧ್ರುವ ವರ್ಣ *);

ಸ್ಥಿರ PyInt RangeStart;
ಸ್ಥಿರ PyInt RangeEnd;

ಸ್ಥಿರ PyObject *globals;

ಸ್ಥಿರ PyObject *py_chdir;
ಸ್ಥಿರ PyObject *py_fchdir;
ಸ್ಥಿರ PyObject *py_getcwd;
ಸ್ಥಿರ PyObject *vim_module;
ಸ್ಥಿರ PyObject *vim_special_path_object;

#ಯದಿ PY_VERSION_HEX >= 0x030700f0
ಸ್ಥಿರ PyObject *py_find_spec;
#ಅನ್ಯಥಾ
ಸ್ಥಿರ PyObject *py_load_module;
#endif
#ಯದಿ PY_VERSION_HEX < 0x30c00a7
ಸ್ಥಿರ PyObject *py_find_module;
#endif

ಸ್ಥಿರ PyObject *VimError;

/*
 * obtain a lock on the Vim data structures
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
Python_Lock_Vim(ಶೂನ್ಯ)
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
StringToChars(PyObject *obj, PyObject **todecref)
{
    char_u	*str;

    ಯದಿ (PyBytes_Check(obj))
    {

	ಯದಿ (PyBytes_AsStringAndSize(obj, (ವರ್ಣ **) &str, ರಿಕ್ತ) == -1
		|| str == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	*todecref = ರಿಕ್ತ;
    }
    ಅನ್ಯಥಾ ಯದಿ (PyUnicode_Check(obj))
    {
	PyObject	*bytes;

	ಯದಿ (!(bytes = PyUnicode_AsEncodedString(obj, ENC_OPT,
							   ERRORS_ENCODE_ARG)))
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	ಯದಿ (PyBytes_AsStringAndSize(bytes, (ವರ್ಣ **) &str, ರಿಕ್ತ) == -1
		|| str == ರಿಕ್ತ)
	{
	    Py_DECREF(bytes);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}

	*todecref = bytes;
    }
    ಅನ್ಯಥಾ
    {
#ಯದಿ PY_MAJOR_VERSION < 3
	PyErr_FORMAT_TYPE(N_("expected str() or unicode() instance, but got %s"),
		obj);
#ಅನ್ಯಥಾ
	PyErr_FORMAT_TYPE(N_("expected bytes() or str() instance, but got %s"),
		obj);
#endif
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಪ್ರತ್ಯಾಗಮ (char_u *) str;
}

#define ಸಂಖ್ಯೆ_LONG     1
#define ಸಂಖ್ಯೆ_INT      2
#define ಸಂಖ್ಯೆ_NATURAL  4
#define ಸಂಖ್ಯೆ_UNSIGNED 8

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ಸಂಖ್ಯೆ_To_Long(PyObject *obj, long *result, ಪೂರ್ಣಾಙ್ಕ flags)
{
#ಯದಿ PY_MAJOR_VERSION < 3
    ಯದಿ (PyInt_Check(obj))
    {
	*result = PyInt_AsLong(obj);
	ಯದಿ (PyErr_Occurred())
	    ಪ್ರತ್ಯಾಗಮ -1;
    }
    ಅನ್ಯಥಾ
#endif
    ಯದಿ (PyLong_Check(obj))
    {
	*result = PyLong_AsLong(obj);
	ಯದಿ (PyErr_Occurred())
	    ಪ್ರತ್ಯಾಗಮ -1;
    }
    ಅನ್ಯಥಾ ಯದಿ (Py_ಸಂಖ್ಯೆ_Check(obj))
    {
	PyObject	*ಅಂಕೆ;

	ಯದಿ (!(ಅಂಕೆ = Py_ಸಂಖ್ಯೆ_Long(obj)))
	    ಪ್ರತ್ಯಾಗಮ -1;

	*result = PyLong_AsLong(ಅಂಕೆ);

	Py_DECREF(ಅಂಕೆ);

	ಯದಿ (PyErr_Occurred())
	    ಪ್ರತ್ಯಾಗಮ -1;
    }
    ಅನ್ಯಥಾ
    {
#ಯದಿ PY_MAJOR_VERSION < 3
	PyErr_FORMAT_TYPE(
		N_("expected int(), long() or something supporting "
		   "coercing to long(), but got %s"),
		obj);
#ಅನ್ಯಥಾ
	PyErr_FORMAT_TYPE(
		N_("expected int() or something supporting coercing to int(), "
		   "but got %s"),
		obj);
#endif
	ಪ್ರತ್ಯಾಗಮ -1;
    }

    ಯದಿ (flags & ಸಂಖ್ಯೆ_INT)
    {
	ಯದಿ (*result > INT_MAX)
	{
	    PyErr_SET_STRING(PyExc_OverflowError,
		    N_("value is too large to fit into C int type"));
	    ಪ್ರತ್ಯಾಗಮ -1;
	}
	ಅನ್ಯಥಾ ಯದಿ (*result < INT_MIN)
	{
	    PyErr_SET_STRING(PyExc_OverflowError,
		    N_("value is too small to fit into C int type"));
	    ಪ್ರತ್ಯಾಗಮ -1;
	}
    }

    ಯದಿ (flags & ಸಂಖ್ಯೆ_NATURAL)
    {
	ಯದಿ (*result <= 0)
	{
	    PyErr_SET_STRING(Py_Exc_ಮೌಲ್ಯ_Error,
		    N_("number must be greater than zero"));
	    ಪ್ರತ್ಯಾಗಮ -1;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (flags & ಸಂಖ್ಯೆ_UNSIGNED)
    {
	ಯದಿ (*result < 0)
	{
	    PyErr_SET_STRING(Py_Exc_ಮೌಲ್ಯ_Error,
		    N_("number must be greater or equal to zero"));
	    ಪ್ರತ್ಯಾಗಮ -1;
	}
    }

    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
add_string(PyObject *list, ವರ್ಣ *s)
{
    PyObject	*string;

    ಯದಿ (!(string = PyString_FromString(s)))
	ಪ್ರತ್ಯಾಗಮ -1;

    ಯದಿ (PyList_Append(list, string))
    {
	Py_DECREF(string);
	ಪ್ರತ್ಯಾಗಮ -1;
    }

    Py_DECREF(string);
    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಸ್ಥಿರ PyObject *
ObjectDir(PyObject *self, ವರ್ಣ **attributes)
{
    PyMethodDef	*method;
    ವರ್ಣ	**attr;
    PyObject	*ret;

    ಯದಿ (!(ret = PyList_New(0)))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (self)
	ಪ್ರತಿ (method = Py_TYPE_GET_TP_METHODS(self->ob_type) ; method->ml_name != ರಿಕ್ತ ; ++method)
	    ಯದಿ (add_string(ret, (ವರ್ಣ *)method->ml_name))
	    {
		Py_DECREF(ret);
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    }

    ಪ್ರತಿ (attr = attributes ; *attr ; ++attr)
	ಯದಿ (add_string(ret, *attr))
	{
	    Py_DECREF(ret);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}

#ಯದಿ PY_MAJOR_VERSION < 3
    ಯದಿ (add_string(ret, "__members__"))
    {
	Py_DECREF(ret);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
#endif

    ಪ್ರತ್ಯಾಗಮ ret;
}

// Output buffer management

// Function to write a line, points to either msg() or emsg().
ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*writefn)(ವರ್ಣ *);

DEFINE_PY_TYPE_OBJECT(OutputType);

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    PyObject_HEAD
    long softspace;
    long error;
} OutputObject;

ಸ್ಥಿರ ವರ್ಣ *OutputAttrs[] = {
    "softspace",
    ರಿಕ್ತ
};

    ಸ್ಥಿರ PyObject *
OutputDir(PyObject *self, PyObject *args UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ ObjectDir(self, OutputAttrs);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
OutputSetattr(PyObject *self_obj, ವರ್ಣ *name, PyObject *valObject)
{
    OutputObject *self = (OutputObject*)self_obj;
    ಯದಿ (valObject == ರಿಕ್ತ)
    {
	PyErr_SET_STRING(PyExc_AttributeError,
		N_("can't delete OutputObject attributes"));
	ಪ್ರತ್ಯಾಗಮ -1;
    }

    ಯದಿ (strcmp(name, "softspace") == 0)
    {
	ಯದಿ (ಸಂಖ್ಯೆ_To_Long(valObject, &(self->softspace), ಸಂಖ್ಯೆ_UNSIGNED))
	    ಪ್ರತ್ಯಾಗಮ -1;
	ಪ್ರತ್ಯಾಗಮ 0;
    }

    PyErr_FORMAT(PyExc_AttributeError, N_("invalid attribute: %s"), name);
    ಪ್ರತ್ಯಾಗಮ -1;
}

// Buffer IO, we write one whole line at a time.
ಸ್ಥಿರ garray_T io_ga = {0, 0, 1, 80, ರಿಕ್ತ};
ಸ್ಥಿರ writefn old_fn = ರಿಕ್ತ;

    ಸ್ಥಿರ ಶೂನ್ಯ
PythonIO_Flush(ಶೂನ್ಯ)
{
    ಯದಿ (old_fn != ರಿಕ್ತ && io_ga.ga_len > 0)
    {
	((ವರ್ಣ *)io_ga.ga_data)[io_ga.ga_len] = NUL;
	// We don't know what emsg_severe should be here, so ... hope?
	old_fn((ವರ್ಣ *)io_ga.ga_data);
    }
    io_ga.ga_len = 0;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
writer(writefn fn, char_u *str, PyInt n, ಪೂರ್ಣಾಙ್ಕ severe)
{
    char_u *ptr;

    // Flush when switching output function.
    ಯದಿ (fn != old_fn)
	PythonIO_Flush();
    old_fn = fn;

    // Write each NL separated line.  Text after the last NL is kept for
    // writing later.
    // For normal messages: Do not output when "got_int" was set.  This avoids
    // a loop gone crazy flooding the terminal with messages.  Also for when
    // "q" is pressed at the more-prompt.
    ಯಾವತ್ (n > 0 && (ptr = memchr(str, '\n', n)) != ರಿಕ್ತ
					  && (fn == (writefn)emsg || !got_int))
    {
	PyInt len = ptr - str;

	ಯದಿ (ga_grow(&io_ga, (ಪೂರ್ಣಾಙ್ಕ)(len + 1)) == FAIL)
	    ಭಙ್ಗ;

	mch_memmove(((ವರ್ಣ *)io_ga.ga_data) + io_ga.ga_len, str, (size_t)len);
	((ವರ್ಣ *)io_ga.ga_data)[io_ga.ga_len + len] = NUL;
	emsg_severe = severe;
	fn((ವರ್ಣ *)io_ga.ga_data);
	str = ptr + 1;
	n -= len + 1;
	io_ga.ga_len = 0;
    }

    // Put the remaining text into io_ga for later printing.
    ಯದಿ (n > 0 && (fn == (writefn)emsg || !got_int)
					&& ga_grow(&io_ga, (ಪೂರ್ಣಾಙ್ಕ)(n + 1)) == OK)
    {
	mch_memmove(((ವರ್ಣ *)io_ga.ga_data) + io_ga.ga_len, str, (size_t)n);
	io_ga.ga_len += (ಪೂರ್ಣಾಙ್ಕ)n;
    }
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
write_output(OutputObject *self, PyObject *string)
{
    Py_ssize_t	len = 0;
    ವರ್ಣ	*str = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		error = self->error;

    ಯದಿ (!PyArg_Parse(string, "et#", ENC_OPT, &str, &len))
	ಪ್ರತ್ಯಾಗಮ -1;

    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    writer((writefn)(error ? emsg : ಸಂದೇಶ), (char_u *)str, len, error);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS
    PyMem_Free(str);

    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಸ್ಥಿರ PyObject *
OutputWrite(OutputObject *self, PyObject *string)
{
    ಯದಿ (write_output(self, string))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    Py_INCREF(Py_None);
    ಪ್ರತ್ಯಾಗಮ Py_None;
}

    ಸ್ಥಿರ PyObject *
OutputWritelines(OutputObject *self, PyObject *seq)
{
    PyObject	*iterator;
    PyObject	*item;

    ಯದಿ (!(iterator = PyObject_GetIter(seq)))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯಾವತ್ ((item = PyIter_Next(iterator)))
    {
	ಯದಿ (write_output(self, item))
	{
	    Py_DECREF(iterator);
	    Py_DECREF(item);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
	Py_DECREF(item);
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    ಯದಿ (PyErr_Occurred())
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    Py_INCREF(Py_None);
    ಪ್ರತ್ಯಾಗಮ Py_None;
}

    ಸ್ಥಿರ PyObject *
AlwaysNone(PyObject *self UNUSED, PyObject *args UNUSED)
{
    // do nothing
    Py_INCREF(Py_None);
    ಪ್ರತ್ಯಾಗಮ Py_None;
}
#define ALWAYS_NONE AlwaysNone(ರಿಕ್ತ, ರಿಕ್ತ)

    ಸ್ಥಿರ PyObject *
AlwaysFalse(PyObject *self UNUSED, PyObject *args UNUSED)
{
    // do nothing
    PyObject	*ret = Py_False;
    Py_INCREF(ret);
    ಪ್ರತ್ಯಾಗಮ ret;
}
#define ALWAYS_FALSE AlwaysFalse(ರಿಕ್ತ, ರಿಕ್ತ)

    ಸ್ಥಿರ PyObject *
AlwaysTrue(PyObject *self UNUSED, PyObject *args UNUSED)
{
    // do nothing
    PyObject	*ret = Py_True;
    Py_INCREF(ret);
    ಪ್ರತ್ಯಾಗಮ ret;
}
#define ALWAYS_TRUE AlwaysTrue(ರಿಕ್ತ, ರಿಕ್ತ)

/***************/

ಸ್ಥಿರ ಸಂರಚನಾ PyMethodDef OutputMethods[] = {
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
    { ರಿಕ್ತ,	    ರಿಕ್ತ,				0,		ರಿಕ್ತ}
};

ಸ್ಥಿರ OutputObject Output =
{
    PyObject_HEAD_INIT_TYPE(OutputType)
    0,
    0
};

ಸ್ಥಿರ OutputObject Error =
{
    PyObject_HEAD_INIT_TYPE(OutputType)
    0,
    1
};

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
PythonIO_Init_io(ಶೂನ್ಯ)
{
    ಯದಿ (PySys_SetObject("stdout", (PyObject *)(ಶೂನ್ಯ *)&Output))
	ಪ್ರತ್ಯಾಗಮ -1;
    ಯದಿ (PySys_SetObject("stderr", (PyObject *)(ಶೂನ್ಯ *)&Error))
	ಪ್ರತ್ಯಾಗಮ -1;

    ಯದಿ (PyErr_Occurred())
    {
	emsg(_(e_python_error_initialising_io_object));
	ಪ್ರತ್ಯಾಗಮ -1;
    }

    ಪ್ರತ್ಯಾಗಮ 0;
}

#ಯದಿ PY_VERSION_HEX < 0x030700f0
ಸ್ಥಿರ PyObject *call_load_module(ವರ್ಣ *name, ಪೂರ್ಣಾಙ್ಕ len, PyObject *find_module_result);

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    PyObject_HEAD
    ವರ್ಣ	*fullname;
    PyObject	*result;
} LoaderObject;
DEFINE_PY_TYPE_OBJECT(LoaderType);

    ಸ್ಥಿರ ಶೂನ್ಯ
LoaderDestructor(PyObject *self_obj)
{
    LoaderObject *self = (LoaderObject*)self_obj;
    vim_free(self->fullname);
    Py_XDECREF(self->result);
    DESTRUCTOR_FINISH(self);
}

    ಸ್ಥಿರ PyObject *
LoaderLoadModule(LoaderObject *self, PyObject *args UNUSED)
{
    ವರ್ಣ	*fullname = self->fullname;
    PyObject	*result = self->result;
    PyObject	*module;

    ಯದಿ (!fullname)
    {
	module = result ? result : Py_None;
	Py_INCREF(module);
	ಪ್ರತ್ಯಾಗಮ module;
    }

    module = call_load_module(fullname, (ಪೂರ್ಣಾಙ್ಕ)STRLEN(fullname), result);

    self->fullname = ರಿಕ್ತ;
    self->result = module;

    vim_free(fullname);
    Py_DECREF(result);

    ಯದಿ (!module)
    {
	ಯದಿ (PyErr_Occurred())
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	Py_INCREF(Py_None);
	ಪ್ರತ್ಯಾಗಮ Py_None;
    }

    Py_INCREF(module);
    ಪ್ರತ್ಯಾಗಮ module;
}

ಸ್ಥಿರ ಸಂರಚನಾ PyMethodDef LoaderMethods[] = {
    // name,	    function,				calling,	doc
    {"load_module", (PyCFunction)LoaderLoadModule,	METH_VARARGS,	""},
    { ರಿಕ್ತ,	    ರಿಕ್ತ,				0,		ರಿಕ್ತ}
};
#endif

/*
 * Check to see whether a Vim error has been reported, or a keyboard
 * interrupt has been detected.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
VimTryStart(ಶೂನ್ಯ)
{
    ++trylevel;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
VimTryEnd(ಶೂನ್ಯ)
{
    --trylevel;
    // Without this it stops processing all subsequent Vim script commands and
    // generates strange error messages if I e.g. try calling Test() in a cycle
    did_emsg = FALSE;
    // Keyboard interrupt should be preferred over anything else
    ಯದಿ (got_int)
    {
	ಯದಿ (did_throw)
	    discard_current_exception();
	got_int = FALSE;
	PyErr_SetNone(PyExc_KeyboardInterrupt);
	ಪ್ರತ್ಯಾಗಮ -1;
    }
    ಅನ್ಯಥಾ ಯದಿ (ಸಂದೇಶ_list != ರಿಕ್ತ && *ಸಂದೇಶ_list != ರಿಕ್ತ)
    {
	ಪೂರ್ಣಾಙ್ಕ	should_free;
	ವರ್ಣ	*ಸಂದೇಶ;

	ಸಂದೇಶ = get_exception_string(*ಸಂದೇಶ_list, ET_ERROR, ರಿಕ್ತ, &should_free);

	ಯದಿ (ಸಂದೇಶ == ರಿಕ್ತ)
	{
	    PyErr_NoMemory();
	    ಪ್ರತ್ಯಾಗಮ -1;
	}

	PyErr_SetVim(ಸಂದೇಶ);

	free_global_msglist();

	ಯದಿ (should_free)
	    vim_free(ಸಂದೇಶ);

	ಪ್ರತ್ಯಾಗಮ -1;
    }
    ಅನ್ಯಥಾ ಯದಿ (!did_throw)
	ಪ್ರತ್ಯಾಗಮ (PyErr_Occurred() ? -1 : 0);
    // Python exception is preferred over Vim one; unlikely to occur though
    ಅನ್ಯಥಾ ಯದಿ (PyErr_Occurred())
    {
	discard_current_exception();
	ಪ್ರತ್ಯಾಗಮ -1;
    }
    // Finally transform Vim script exception to python one
    ಅನ್ಯಥಾ
    {
	PyErr_SetVim((ವರ್ಣ *)current_exception->ಮೌಲ್ಯ);
	discard_current_exception();
	ಪ್ರತ್ಯಾಗಮ -1;
    }
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
VimCheckInterrupt(ಶೂನ್ಯ)
{
    ಯದಿ (got_int)
    {
	PyErr_SetNone(PyExc_KeyboardInterrupt);
	ಪ್ರತ್ಯಾಗಮ 1;
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

// Vim module - Implementation

    ಸ್ಥಿರ PyObject *
VimCommand(PyObject *self UNUSED, PyObject *string)
{
    char_u	*cmd;
    PyObject	*ret;
    PyObject	*todecref;

    ಯದಿ (!(cmd = StringToChars(string, &todecref)))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();

    VimTryStart();
    do_cmdline_cmd(cmd);
    update_screen(UPD_VALID);

    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    ಯದಿ (VimTryEnd())
	ret = ರಿಕ್ತ;
    ಅನ್ಯಥಾ
	ret = Py_None;

    Py_XINCREF(ret);
    Py_XDECREF(todecref);
    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Function to translate a typval_T into a PyObject; this will recursively
 * translate lists/dictionaries into their Python equivalents.
 *
 * The depth parameter is to avoid infinite recursion, set it to 1 when
 * you call VimToPython.
 */
    ಸ್ಥಿರ PyObject *
VimToPython(typval_T *our_tv, ಪೂರ್ಣಾಙ್ಕ depth, PyObject *lookup_dict)
{
    PyObject	*ret;
    PyObject	*newObj;
    ವರ್ಣ	ptrBuf[ಪರಿಮಾಣ(ಶೂನ್ಯ *) * 2 + 3];

    // Avoid infinite recursion
    ಯದಿ (depth > 100)
    {
	Py_INCREF(Py_None);
	ret = Py_None;
	ಪ್ರತ್ಯಾಗಮ ret;
    }

    // Check if we run into a recursive loop.  The item must be in lookup_dict
    // then and we can use it again.
    ಯದಿ ((our_tv->v_type == VAR_LIST && our_tv->vval.v_list != ರಿಕ್ತ)
	    || (our_tv->v_type == VAR_TUPLE && our_tv->vval.v_tuple != ರಿಕ್ತ)
	    || (our_tv->v_type == VAR_DICT && our_tv->vval.v_dict != ರಿಕ್ತ))
    {
	sprintf(ptrBuf, "%p",
		our_tv->v_type == VAR_LIST ? (ಶೂನ್ಯ *)our_tv->vval.v_list
		    : our_tv->v_type == VAR_TUPLE ?
			(ಶೂನ್ಯ *)our_tv->vval.v_tuple
			: (ಶೂನ್ಯ *)our_tv->vval.v_dict);

	ಯದಿ ((ret = PyDict_GetItemString(lookup_dict, ptrBuf)))
	{
	    Py_INCREF(ret);
	    ಪ್ರತ್ಯಾಗಮ ret;
	}
    }

    ಯದಿ (our_tv->v_type == VAR_STRING)
	ret = PyString_FromString(our_tv->vval.v_string == ರಿಕ್ತ
					? "" : (ವರ್ಣ *)our_tv->vval.v_string);
    ಅನ್ಯಥಾ ಯದಿ (our_tv->v_type == VAR_ಸಂಖ್ಯೆ)
    {
	ವರ್ಣ buf[NUMBUFLEN];

	// For backwards compatibility numbers are stored as strings.
	sprintf(buf, "%lld", (long long)our_tv->vval.v_ಸಂಖ್ಯೆ);
	ret = PyString_FromString((ವರ್ಣ *)buf);
    }
    ಅನ್ಯಥಾ ಯದಿ (our_tv->v_type == VAR_FLOAT)
    {
	ವರ್ಣ buf[NUMBUFLEN];

	sprintf(buf, "%f", our_tv->vval.v_float);
	ret = PyString_FromString((ವರ್ಣ *)buf);
    }
    ಅನ್ಯಥಾ ಯದಿ (our_tv->v_type == VAR_LIST)
    {
	list_T		*list = our_tv->vval.v_list;
	listitem_T	*curr;

	ಯದಿ (list == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	ಯದಿ (!(ret = PyList_New(0)))
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	ಯದಿ (PyDict_SetItemString(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}

	CHECK_LIST_MATERIALIZE(list);
	FOR_ALL_LIST_ವಸ್ತುಗಳು(list, curr)
	{
	    ಯದಿ (!(newObj = VimToPython(&curr->li_tv, depth + 1, lookup_dict)))
	    {
		Py_DECREF(ret);
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    }
	    ಯದಿ (PyList_Append(ret, newObj))
	    {
		Py_DECREF(newObj);
		Py_DECREF(ret);
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    }
	    Py_DECREF(newObj);
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (our_tv->v_type == VAR_TUPLE)
    {
	tuple_T	*tuple = our_tv->vval.v_tuple;
	ಪೂರ್ಣಾಙ್ಕ	len;

	ಯದಿ (tuple == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	len = TUPLE_LEN(tuple);

	ret = PyTuple_New(len);
	ಯದಿ (ret == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ idx = 0; idx < len; idx++)
	{
	    typval_T	*item_tv = TUPLE_ITEM(tuple, idx);

	    newObj = VimToPython(item_tv, depth + 1, lookup_dict);
	    ಯದಿ (!newObj)
	    {
		Py_DECREF(ret);
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    }
	    PyTuple_SetItem(ret, idx, newObj);
	}

	ಯದಿ (PyDict_SetItemString(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (our_tv->v_type == VAR_DICT)
    {

	hashtab_T	*ht;
	long_u		todo;
	hashitem_T	*hi;
	dictitem_T	*di;

	ಯದಿ (our_tv->vval.v_dict == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	ht = &our_tv->vval.v_dict->dv_hashtab;

	ಯದಿ (!(ret = PyDict_New()))
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	ಯದಿ (PyDict_SetItemString(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}

	todo = ht->ht_used;
	ಪ್ರತಿ (hi = ht->ht_array; todo > 0; ++hi)
	{
	    ಯದಿ (!HASHITEM_EMPTY(hi))
	    {
		--todo;

		di = dict_lookup(hi);
		ಯದಿ (!(newObj = VimToPython(&di->di_tv, depth + 1, lookup_dict)))
		{
		    Py_DECREF(ret);
		    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		}
		ಯದಿ (PyDict_SetItemString(ret, (ವರ್ಣ *)hi->hi_ಕೀಲಿ, newObj))
		{
		    Py_DECREF(ret);
		    Py_DECREF(newObj);
		    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		}
	    }
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (our_tv->v_type == VAR_BOOL)
    {
	ಯದಿ (our_tv->vval.v_ಸಂಖ್ಯೆ == VVAL_FALSE)
	{
	    ret = Py_False;
	    Py_INCREF(ret);
	}
	ಅನ್ಯಥಾ
	{
	    ret = Py_True;
	    Py_INCREF(ret);
	}
	ಪ್ರತ್ಯಾಗಮ ret;
    }
    ಅನ್ಯಥಾ ಯದಿ (our_tv->v_type == VAR_SPECIAL)
    {
	Py_INCREF(Py_None);
	ret = Py_None;
	ಪ್ರತ್ಯಾಗಮ ret;
    }
    ಅನ್ಯಥಾ ಯದಿ (our_tv->v_type == VAR_BLOB)
	ret = PyBytes_FromStringAndSize(
		(ವರ್ಣ*) our_tv->vval.v_blob->bv_ga.ga_data,
		(Py_ssize_t) our_tv->vval.v_blob->bv_ga.ga_len);
    ಅನ್ಯಥಾ
    {
	Py_INCREF(Py_None);
	ret = Py_None;
    }

    ಪ್ರತ್ಯಾಗಮ ret;
}

    ಸ್ಥಿರ PyObject *
VimEval(PyObject *self UNUSED, PyObject *args)
{
    char_u	*ಅಭಿವ್ಯಕ್ತಿ;
    typval_T	*our_tv;
    PyObject	*string;
    PyObject	*todecref;
    PyObject	*ret;
    PyObject	*lookup_dict;

    ಯದಿ (!PyArg_ParseTuple(args, "O", &string))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (!(ಅಭಿವ್ಯಕ್ತಿ = StringToChars(string, &todecref)))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    VimTryStart();
    our_tv = eval_ಅಭಿವ್ಯಕ್ತಿ(ಅಭಿವ್ಯಕ್ತಿ, ರಿಕ್ತ);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    Py_XDECREF(todecref);

    ಯದಿ (VimTryEnd())
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (our_tv == ರಿಕ್ತ)
    {
	PyErr_SET_VIM(N_("invalid expression"));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    // Convert the Vim type into a Python type.  Create a dictionary that's
    // used to check for recursive loops.
    ಯದಿ (!(lookup_dict = PyDict_New()))
	ret = ರಿಕ್ತ;
    ಅನ್ಯಥಾ
    {
	ret = VimToPython(our_tv, 1, lookup_dict);
	Py_DECREF(lookup_dict);
    }


    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    free_tv(our_tv);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    ಪ್ರತ್ಯಾಗಮ ret;
}

ಸ್ಥಿರ PyObject *ConvertToPyObject(typval_T *);

    ಸ್ಥಿರ PyObject *
VimEvalPy(PyObject *self UNUSED, PyObject *string)
{
    typval_T	*our_tv;
    PyObject	*ret;
    char_u	*ಅಭಿವ್ಯಕ್ತಿ;
    PyObject	*todecref;

    ಯದಿ (!(ಅಭಿವ್ಯಕ್ತಿ = StringToChars(string, &todecref)))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    VimTryStart();
    our_tv = eval_ಅಭಿವ್ಯಕ್ತಿ(ಅಭಿವ್ಯಕ್ತಿ, ರಿಕ್ತ);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    Py_XDECREF(todecref);

    ಯದಿ (VimTryEnd())
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (our_tv == ರಿಕ್ತ)
    {
	PyErr_SET_VIM(N_("invalid expression"));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ret = ConvertToPyObject(our_tv);
    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    free_tv(our_tv);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    ಪ್ರತ್ಯಾಗಮ ret;
}

    ಸ್ಥಿರ PyObject *
VimStrwidth(PyObject *self UNUSED, PyObject *string)
{
    char_u	*str;
    PyObject	*todecref;
    ಪೂರ್ಣಾಙ್ಕ		len;

    ಯದಿ (!(str = StringToChars(string, &todecref)))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    len = mb_string2cells(str, (ಪೂರ್ಣಾಙ್ಕ)STRLEN(str));

    Py_XDECREF(todecref);

    ಪ್ರತ್ಯಾಗಮ PyLong_FromLong(len);
}

    ಸ್ಥಿರ PyObject *
_VimChdir(PyObject *_chdir, PyObject *args, PyObject *kwargs)
{
    PyObject	*ret;
    PyObject	*newwd;
    PyObject	*todecref;
    char_u	*new_dir;

    ಯದಿ (_chdir == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಯದಿ (!(ret = PyObject_Call(_chdir, args, kwargs)))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (!(newwd = PyObject_CallFunctionObjArgs(py_getcwd, ರಿಕ್ತ)))
    {
	Py_DECREF(ret);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಯದಿ (!(new_dir = StringToChars(newwd, &todecref)))
    {
	Py_DECREF(ret);
	Py_DECREF(newwd);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    VimTryStart();

    ಯದಿ (vim_chdir(new_dir))
    {
	Py_DECREF(ret);
	Py_DECREF(newwd);
	Py_XDECREF(todecref);

	ಯದಿ (VimTryEnd())
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	PyErr_SET_VIM(N_("failed to change directory"));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    Py_DECREF(newwd);
    Py_XDECREF(todecref);

    post_chdir(CDSCOPE_GLOBAL);

    ಯದಿ (VimTryEnd())
    {
	Py_DECREF(ret);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಪ್ರತ್ಯಾಗಮ ret;
}

    ಸ್ಥಿರ PyObject *
VimChdir(PyObject *self UNUSED, PyObject *args, PyObject *kwargs)
{
    ಪ್ರತ್ಯಾಗಮ _VimChdir(py_chdir, args, kwargs);
}

    ಸ್ಥಿರ PyObject *
VimFchdir(PyObject *self UNUSED, PyObject *args, PyObject *kwargs)
{
    ಪ್ರತ್ಯಾಗಮ _VimChdir(py_fchdir, args, kwargs);
}

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    PyObject	*callable;
    PyObject	*result;
} map_rtp_data;

    ಸ್ಥಿರ ಶೂನ್ಯ
map_rtp_callback(char_u *path, ಶೂನ್ಯ *_data)
{
    ಶೂನ್ಯ	**data = (ಶೂನ್ಯ **) _data;
    PyObject	*pathObject;
    map_rtp_data	*mr_data = *((map_rtp_data **) data);

    ಯದಿ (!(pathObject = PyString_FromString((ವರ್ಣ *)path)))
    {
	*data = ರಿಕ್ತ;
	ಪ್ರತ್ಯಾಗಮ;
    }

    mr_data->result = PyObject_CallFunctionObjArgs(mr_data->callable,
						   pathObject, ರಿಕ್ತ);

    Py_DECREF(pathObject);

    ಯದಿ (!mr_data->result || mr_data->result != Py_None)
	*data = ರಿಕ್ತ;
    ಅನ್ಯಥಾ
    {
	Py_DECREF(mr_data->result);
	mr_data->result = ರಿಕ್ತ;
    }
}

    ಸ್ಥಿರ PyObject *
VimForeachRTP(PyObject *self UNUSED, PyObject *callable)
{
    map_rtp_data	data;

    data.callable = callable;
    data.result = ರಿಕ್ತ;

    do_in_runtimepath(ರಿಕ್ತ, 0, &map_rtp_callback, &data);

    ಯದಿ (data.result == ರಿಕ್ತ)
    {
	ಯದಿ (PyErr_Occurred())
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	ಅನ್ಯಥಾ
	{
	    Py_INCREF(Py_None);
	    ಪ್ರತ್ಯಾಗಮ Py_None;
	}
    }
    ಪ್ರತ್ಯಾಗಮ data.result;
}

/*
 * _vim_runtimepath_ special path implementation.
 */

    ಸ್ಥಿರ ಶೂನ್ಯ
map_finder_callback(char_u *path, ಶೂನ್ಯ *_data)
{
    ಶೂನ್ಯ	**data = (ಶೂನ್ಯ **) _data;
    PyObject	*list = *((PyObject **) data);
    PyObject	*pathObject1, *pathObject2;
    ವರ್ಣ	*pathbuf;
    size_t	pathlen;

    pathlen = STRLEN(path);

#ಯದಿ PY_MAJOR_VERSION < 3
# define PY_MAIN_DIR_STRING "python2"
#ಅನ್ಯಥಾ
# define PY_MAIN_DIR_STRING "python3"
#endif
#define PY_ALTERNATE_DIR_STRING "pythonx"

#define PYTHONX_STRING_LENGTH 7 // STRLEN("pythonx")
    ಯದಿ (!(pathbuf = PyMem_New(ವರ್ಣ,
		    pathlen + STRLEN(PATHSEPSTR) + PYTHONX_STRING_LENGTH + 1)))
    {
	PyErr_NoMemory();
	*data = ರಿಕ್ತ;
	ಪ್ರತ್ಯಾಗಮ;
    }

    mch_memmove(pathbuf, path, pathlen + 1);
    add_pathsep((char_u *) pathbuf);

    pathlen = STRLEN(pathbuf);
    mch_memmove(pathbuf + pathlen, PY_MAIN_DIR_STRING,
	    PYTHONX_STRING_LENGTH + 1);

    ಯದಿ (!(pathObject1 = PyString_FromString(pathbuf)))
    {
	*data = ರಿಕ್ತ;
	PyMem_Free(pathbuf);
	ಪ್ರತ್ಯಾಗಮ;
    }

    mch_memmove(pathbuf + pathlen, PY_ALTERNATE_DIR_STRING,
	    PYTHONX_STRING_LENGTH + 1);

    ಯದಿ (!(pathObject2 = PyString_FromString(pathbuf)))
    {
	Py_DECREF(pathObject1);
	PyMem_Free(pathbuf);
	*data = ರಿಕ್ತ;
	ಪ್ರತ್ಯಾಗಮ;
    }

    PyMem_Free(pathbuf);

    ಯದಿ (PyList_Append(list, pathObject1)
	    || PyList_Append(list, pathObject2))
	*data = ರಿಕ್ತ;

    Py_DECREF(pathObject1);
    Py_DECREF(pathObject2);
}

    ಸ್ಥಿರ PyObject *
Vim_GetPaths(PyObject *self UNUSED, PyObject *args UNUSED)
{
    PyObject	*ret;

    ಯದಿ (!(ret = PyList_New(0)))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    do_in_runtimepath(ರಿಕ್ತ, 0, &map_finder_callback, ret);

    ಯದಿ (PyErr_Occurred())
    {
	Py_DECREF(ret);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಪ್ರತ್ಯಾಗಮ ret;
}

#ಯದಿ PY_VERSION_HEX >= 0x030700f0
    ಸ್ಥಿರ PyObject *
FinderFindSpec(PyObject *self, PyObject *args)
{
    ವರ್ಣ	*fullname;
    PyObject	*paths;
    PyObject	*target = Py_None;
    PyObject	*spec;

    ಯದಿ (!PyArg_ParseTuple(args, "s|O", &fullname, &target))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (!(paths = Vim_GetPaths(self, ರಿಕ್ತ)))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    spec = PyObject_CallFunction(py_find_spec, "sOO", fullname, paths, target);

    Py_DECREF(paths);

    ಯದಿ (!spec)
    {
	ಯದಿ (PyErr_Occurred())
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	Py_INCREF(Py_None);
	ಪ್ರತ್ಯಾಗಮ Py_None;
    }

    ಪ್ರತ್ಯಾಗಮ spec;
}

    ಸ್ಥಿರ PyObject *
FinderFindModule(PyObject* self UNUSED, PyObject* args UNUSED)
{
    // Apparently returning None works.
    Py_INCREF(Py_None);
    ಪ್ರತ್ಯಾಗಮ Py_None;
}
#ಅನ್ಯಥಾ
    ಸ್ಥಿರ PyObject *
call_load_module(ವರ್ಣ *name, ಪೂರ್ಣಾಙ್ಕ len, PyObject *find_module_result)
{
    PyObject	*fd, *pathname, *description;

    ಯದಿ (!PyTuple_Check(find_module_result))
    {
	PyErr_FORMAT_TYPE(
		N_("expected 3-tuple as imp.find_module() result, but got %s"),
		find_module_result);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಯದಿ (PyTuple_GET_SIZE(find_module_result) != 3)
    {
	PyErr_FORMAT(PyExc_TypeError,
		N_("expected 3-tuple as imp.find_module() result, but got "
		   "tuple of size %d"),
		(ಪೂರ್ಣಾಙ್ಕ) PyTuple_GET_SIZE(find_module_result));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಯದಿ (!(fd = PyTuple_GET_ITEM(find_module_result, 0))
	    || !(pathname = PyTuple_GET_ITEM(find_module_result, 1))
	    || !(description = PyTuple_GET_ITEM(find_module_result, 2)))
    {
	PyErr_SET_STRING(PyExc_RuntimeError,
		N_("internal error: imp.find_module returned tuple with NULL"));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಪ್ರತ್ಯಾಗಮ PyObject_CallFunction(py_load_module,
	    "s#OOO", name, len, fd, pathname, description);
}

    ಸ್ಥಿರ PyObject *
find_module(ವರ್ಣ *fullname, ವರ್ಣ *tail, PyObject *new_path)
{
    PyObject	*find_module_result;
    PyObject	*module;
    ವರ್ಣ	*dot;

    ಯದಿ ((dot = (ವರ್ಣ *)vim_strchr((char_u *) tail, '.')))
    {
	/*
	 * There is a dot in the name: call find_module recursively without the
	 * first component
	 */
	PyObject	*newest_path;
	ಪೂರ್ಣಾಙ್ಕ		partlen = (ಪೂರ್ಣಾಙ್ಕ) (dot - 1 - tail);

	ಯದಿ (!(find_module_result = PyObject_CallFunction(py_find_module,
			"s#O", tail, partlen, new_path)))
	{
	    ಯದಿ (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_ImportError))
		PyErr_Clear();
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}

	ಯದಿ (!(module = call_load_module(
			fullname,
			((ಪೂರ್ಣಾಙ್ಕ) (tail - fullname)) + partlen,
			find_module_result)))
	{
	    Py_DECREF(find_module_result);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}

	Py_DECREF(find_module_result);

	ಯದಿ (!(newest_path = PyObject_GetAttrString(module, "__path__")))
	{
	    Py_DECREF(module);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}

	Py_DECREF(module);

	find_module_result = find_module(fullname, dot + 1, newest_path);

	Py_DECREF(newest_path);

	ಪ್ರತ್ಯಾಗಮ find_module_result;
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (!(find_module_result = PyObject_CallFunction(py_find_module,
			"sO", tail, new_path)))
	{
	    ಯದಿ (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_ImportError))
		PyErr_Clear();
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}

	ಪ್ರತ್ಯಾಗಮ find_module_result;
    }
}

    ಸ್ಥಿರ PyObject *
FinderFindModule(PyObject *self, PyObject *args)
{
    ವರ್ಣ	*fullname;
    PyObject	*result;
    PyObject	*new_path;
    LoaderObject	*loader;

    ಯದಿ (!PyArg_ParseTuple(args, "s", &fullname))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (!(new_path = Vim_GetPaths(self, ರಿಕ್ತ)))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    result = find_module(fullname, fullname, new_path);

    Py_DECREF(new_path);

    ಯದಿ (!result)
    {
	ಯದಿ (PyErr_Occurred())
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	Py_INCREF(Py_None);
	ಪ್ರತ್ಯಾಗಮ Py_None;
    }

    ಯದಿ (!(fullname = (ವರ್ಣ *)vim_strsave((char_u *)fullname)))
    {
	Py_DECREF(result);
	PyErr_NoMemory();
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಯದಿ (!(loader = PyObject_NEW(LoaderObject, LoaderTypePtr)))
    {
	vim_free(fullname);
	Py_DECREF(result);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    loader->fullname = fullname;
    loader->result = result;

    ಪ್ರತ್ಯಾಗಮ (PyObject *) loader;
}
#endif

    ಸ್ಥಿರ PyObject *
VimPathHook(PyObject *self UNUSED, PyObject *args)
{
    ವರ್ಣ	*path;

    ಯದಿ (PyArg_ParseTuple(args, "s", &path)
	    && STRCMP(path, vim_special_path) == 0)
    {
	Py_INCREF(vim_module);
	ಪ್ರತ್ಯಾಗಮ vim_module;
    }

    PyErr_Clear();
    PyErr_SetNone(PyExc_ImportError);
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Vim module - Definitions
 */

ಸ್ಥಿರ ಸಂರಚನಾ PyMethodDef VimMethods[] = {
    // name,	    function,			calling,			documentation
    {"command",	    VimCommand,			METH_O,				"Execute a Vim ex-mode command" },
    {"eval",	    VimEval,			METH_VARARGS,			"Evaluate an expression using Vim evaluator" },
    {"bindeval",    VimEvalPy,			METH_O,				"Like eval(), but returns objects attached to Vim ones"},
    {"strwidth",    VimStrwidth,		METH_O,				"Screen string width, counts <Tab> as having width 1"},
    {"chdir",	    (PyCFunction)(ಶೂನ್ಯ *)VimChdir,	METH_VARARGS|METH_KEYWORDS,	"Change directory"},
    {"fchdir",	    (PyCFunction)(ಶೂನ್ಯ *)VimFchdir,	METH_VARARGS|METH_KEYWORDS,	"Change directory"},
    {"foreach_rtp", VimForeachRTP,		METH_O,				"Call given callable for each path in &rtp"},
#ಯದಿ PY_VERSION_HEX >= 0x030700f0
    {"find_spec",   FinderFindSpec,		METH_VARARGS,			"Internal use only, returns spec object for any input it receives"},
#endif
    {"find_module", FinderFindModule,		METH_VARARGS,			"Internal use only, returns loader object for any input it receives"},
    {"path_hook",   VimPathHook,		METH_VARARGS,			"Hook function to install in sys.path_hooks"},
    {"_get_paths",  (PyCFunction)Vim_GetPaths,	METH_NOARGS,			"Get &rtp-based additions to sys.path"},
    { ರಿಕ್ತ,	    ರಿಕ್ತ,			0,				ರಿಕ್ತ}
};

/*
 * Generic iterator object
 */

DEFINE_PY_TYPE_OBJECT(IterType);

ಪ್ರಕಾರನಿರ್ದೇಶ PyObject *(*nextfun)(ಶೂನ್ಯ **);
ಪ್ರಕಾರನಿರ್ದೇಶ ಶೂನ್ಯ (*destructorfun)(ಶೂನ್ಯ *);
ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*traversefun)(ಶೂನ್ಯ *, visitproc, ಶೂನ್ಯ *);
ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*clearfun)(ಶೂನ್ಯ **);

// Main purpose of this object is removing the need for do python
// initialization (i.e. PyType_Ready and setting type attributes) for a big
// bunch of objects.
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    PyObject_HEAD
    ಶೂನ್ಯ *cur;
    nextfun next;
    destructorfun destruct;
    traversefun traverse;
    clearfun clear;
    PyObject *iter_object;
} IterObject;

    ಸ್ಥಿರ PyObject *
IterNew(ಶೂನ್ಯ *start, destructorfun destruct, nextfun next, traversefun traverse,
	clearfun clear, PyObject *iter_object)
{
    IterObject *self;

    self = PyObject_GC_New(IterObject, IterTypePtr);
    self->cur = start;
    self->next = next;
    self->destruct = destruct;
    self->traverse = traverse;
    self->clear = clear;
    self->iter_object = iter_object;

    ಯದಿ (iter_object)
	Py_INCREF(iter_object);

    ಪ್ರತ್ಯಾಗಮ (PyObject *)(self);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
IterDestructor(PyObject *self_obj)
{
    IterObject *self = (IterObject*)self_obj;
    ಯದಿ (self->iter_object)
	Py_DECREF(self->iter_object);
    PyObject_GC_UnTrack((ಶೂನ್ಯ *)(self));
    self->destruct(self->cur);
    PyObject_GC_Del((ಶೂನ್ಯ *)(self));
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
IterTraverse(PyObject *self_obj, visitproc visit, ಶೂನ್ಯ *arg)
{
    IterObject *self = (IterObject*)self_obj;
    ಯದಿ (self->traverse != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ self->traverse(self->cur, visit, arg);
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ 0;
}

// Mac OSX defines clear() somewhere.
#ifdef clear
# undef clear
#endif

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
IterClear(PyObject *self_obj)
{
    IterObject *self = (IterObject*)self_obj;
    ಯದಿ (self->clear != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ self->clear(&self->cur);
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ 0;
}

    ಸ್ಥಿರ PyObject *
IterNext(PyObject *self_obj)
{
    IterObject *self = (IterObject*)self_obj;
    ಪ್ರತ್ಯಾಗಮ self->next(&self->cur);
}

    ಸ್ಥಿರ PyObject *
IterIter(PyObject *self)
{
    Py_INCREF(self);
    ಪ್ರತ್ಯಾಗಮ self;
}

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ pylinkedlist_S {
    ಸಂರಚನಾ pylinkedlist_S	*pll_next;
    ಸಂರಚನಾ pylinkedlist_S	*pll_prev;
    PyObject			*pll_obj;
} pylinkedlist_T;

ಸ್ಥಿರ pylinkedlist_T *lastdict = ರಿಕ್ತ;
ಸ್ಥಿರ pylinkedlist_T *lastlist = ರಿಕ್ತ;
ಸ್ಥಿರ pylinkedlist_T *lasttuple = ರಿಕ್ತ;
ಸ್ಥಿರ pylinkedlist_T *lastfunc = ರಿಕ್ತ;

    ಸ್ಥಿರ ಶೂನ್ಯ
pyll_remove(pylinkedlist_T *ref, pylinkedlist_T **last)
{
    ಯದಿ (ref->pll_prev == ರಿಕ್ತ)
    {
	ಯದಿ (ref->pll_next == ರಿಕ್ತ)
	{
	    *last = ರಿಕ್ತ;
	    ಪ್ರತ್ಯಾಗಮ;
	}
    }
    ಅನ್ಯಥಾ
	ref->pll_prev->pll_next = ref->pll_next;

    ಯದಿ (ref->pll_next == ರಿಕ್ತ)
	*last = ref->pll_prev;
    ಅನ್ಯಥಾ
	ref->pll_next->pll_prev = ref->pll_prev;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
pyll_add(PyObject *self, pylinkedlist_T *ref, pylinkedlist_T **last)
{
    ಯದಿ (*last == ರಿಕ್ತ)
	ref->pll_prev = ರಿಕ್ತ;
    ಅನ್ಯಥಾ
    {
	(*last)->pll_next = ref;
	ref->pll_prev = *last;
    }
    ref->pll_next = ರಿಕ್ತ;
    ref->pll_obj = self;
    *last = ref;
}

DEFINE_PY_TYPE_OBJECT(DictionaryType);

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    PyObject_HEAD
    dict_T	*dict;
    pylinkedlist_T	ref;
} DictionaryObject;

ಸ್ಥಿರ PyObject *DictionaryUpdate(DictionaryObject *, PyObject *, PyObject *);

#define NEW_DICTIONARY(dict) DictionaryNew(DictionaryTypePtr, dict)

    ಸ್ಥಿರ PyObject *
DictionaryNew(PyTypeObject *subtype, dict_T *dict)
{
    DictionaryObject	*self;

    self = (DictionaryObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    ಯದಿ (self == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    self->dict = dict;
    ++dict->dv_refcount;

    pyll_add((PyObject *)(self), &self->ref, &lastdict);

    ಪ್ರತ್ಯಾಗಮ (PyObject *)(self);
}

    ಸ್ಥಿರ dict_T *
py_dict_alloc(ಶೂನ್ಯ)
{
    dict_T	*ret;

    ಯದಿ (!(ret = dict_alloc()))
    {
	PyErr_NoMemory();
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ++ret->dv_refcount;

    ಪ್ರತ್ಯಾಗಮ ret;
}

    ಸ್ಥಿರ PyObject *
DictionaryConstructor(PyTypeObject *subtype, PyObject *args, PyObject *kwargs)
{
    DictionaryObject	*self;
    dict_T	*dict;

    ಯದಿ (!(dict = py_dict_alloc()))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    self = (DictionaryObject *) DictionaryNew(subtype, dict);

    --dict->dv_refcount;

    ಯದಿ (kwargs || PyTuple_Size(args))
    {
	PyObject	*tmp;
	ಯದಿ (!(tmp = DictionaryUpdate(self, args, kwargs)))
	{
	    Py_DECREF(self);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}

	Py_DECREF(tmp);
    }

    ಪ್ರತ್ಯಾಗಮ (PyObject *)(self);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
DictionaryDestructor(PyObject *self_obj)
{
    DictionaryObject *self = (DictionaryObject*)self_obj;
    pyll_remove(&self->ref, &lastdict);
    dict_unref(self->dict);

    DESTRUCTOR_FINISH(self);
}

ಸ್ಥಿರ ವರ್ಣ *DictionaryAttrs[] = {
    "locked", "scope",
    ರಿಕ್ತ
};

    ಸ್ಥಿರ PyObject *
DictionaryDir(PyObject *self, PyObject *args UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ ObjectDir(self, DictionaryAttrs);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
DictionarySetattr(PyObject *self_obj, ವರ್ಣ *name, PyObject *valObject)
{
    DictionaryObject *self = (DictionaryObject*)self_obj;
    ಯದಿ (valObject == ರಿಕ್ತ)
    {
	PyErr_SET_STRING(PyExc_AttributeError,
		N_("cannot delete vim.Dictionary attributes"));
	ಪ್ರತ್ಯಾಗಮ -1;
    }

    ಯದಿ (strcmp(name, "locked") == 0)
    {
	ಯದಿ (self->dict->dv_lock == VAR_FIXED)
	{
	    PyErr_SET_STRING(PyExc_TypeError,
		    N_("cannot modify fixed dictionary"));
	    ಪ್ರತ್ಯಾಗಮ -1;
	}
	ಅನ್ಯಥಾ
	{
	    ಪೂರ್ಣಾಙ್ಕ		istrue = PyObject_IsTrue(valObject);
	    ಯದಿ (istrue == -1)
		ಪ್ರತ್ಯಾಗಮ -1;
	    ಅನ್ಯಥಾ ಯದಿ (istrue)
		self->dict->dv_lock = VAR_LOCKED;
	    ಅನ್ಯಥಾ
		self->dict->dv_lock = 0;
	}
	ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಅನ್ಯಥಾ
    {
	PyErr_FORMAT(PyExc_AttributeError, N_("cannot set attribute %s"), name);
	ಪ್ರತ್ಯಾಗಮ -1;
    }
}

    ಸ್ಥಿರ PyInt
DictionaryLength(DictionaryObject *self)
{
    ಪ್ರತ್ಯಾಗಮ ((PyInt) (self->dict->dv_hashtab.ht_used));
}

#define DICT_FLAG_HAS_DEFAULT	0x01
#define DICT_FLAG_ತೆಗೆ		0x02
#define DICT_FLAG_NONE_DEFAULT	0x04
#define DICT_FLAG_RETURN_BOOL	0x08 // Incompatible with DICT_FLAG_POP
#define DICT_FLAG_RETURN_PAIR	0x10

    ಸ್ಥಿರ PyObject *
_DictionaryItem(DictionaryObject *self, PyObject *args, ಪೂರ್ಣಾಙ್ಕ flags)
{
    PyObject	*ಕೀಲಿ_Object;
    PyObject	*defObject = ((flags & DICT_FLAG_NONE_DEFAULT)? Py_None : ರಿಕ್ತ);
    PyObject	*ret;
    char_u	*ಕೀಲಿ;
    dictitem_T	*di;
    dict_T	*dict = self->dict;
    hashitem_T	*hi;
    PyObject	*todecref;

    ಯದಿ (flags & DICT_FLAG_HAS_DEFAULT)
    {
	ಯದಿ (!PyArg_ParseTuple(args, "O|O", &ಕೀಲಿ_Object, &defObject))
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಅನ್ಯಥಾ
	ಕೀಲಿ_Object = args;

    ಯದಿ (flags & DICT_FLAG_RETURN_BOOL)
	defObject = Py_False;

    ಯದಿ (!(ಕೀಲಿ = StringToChars(ಕೀಲಿ_Object, &todecref)))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (*ಕೀಲಿ == NUL)
    {
	RAISE_NO_EMPTY_ಕೀಲಿಗಳು;
	Py_XDECREF(todecref);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    hi = hash_find(&dict->dv_hashtab, ಕೀಲಿ);

    Py_XDECREF(todecref);

    ಯದಿ (HASHITEM_EMPTY(hi))
    {
	ಯದಿ (defObject)
	{
	    Py_INCREF(defObject);
	    ಪ್ರತ್ಯಾಗಮ defObject;
	}
	ಅನ್ಯಥಾ
	{
	    PyErr_SetObject(Py_Exc_ಕೀಲಿ_Error, ಕೀಲಿ_Object);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (flags & DICT_FLAG_RETURN_BOOL)
    {
	ret = Py_True;
	Py_INCREF(ret);
	ಪ್ರತ್ಯಾಗಮ ret;
    }

    di = dict_lookup(hi);

    ಯದಿ (!(ret = ConvertToPyObject(&di->di_tv)))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (flags & DICT_FLAG_ತೆಗೆ)
    {
	ಯದಿ (dict->dv_lock)
	{
	    RAISE_LOCKED_DICTIONARY;
	    Py_DECREF(ret);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}

	hash_remove(&dict->dv_hashtab, hi, "Python remove variable");
	dictitem_free(di);
    }

    ಪ್ರತ್ಯಾಗಮ ret;
}

    ಸ್ಥಿರ PyObject *
DictionaryItem(DictionaryObject *self, PyObject *ಕೀಲಿ_Object)
{
    ಪ್ರತ್ಯಾಗಮ _DictionaryItem(self, ಕೀಲಿ_Object, 0);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
DictionaryContains(DictionaryObject *self, PyObject *ಕೀಲಿ_Object)
{
    PyObject	*rObj = _DictionaryItem(self, ಕೀಲಿ_Object, DICT_FLAG_RETURN_BOOL);
    ಪೂರ್ಣಾಙ್ಕ		ret;

    ಯದಿ (rObj == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ -1;

    ret = (rObj == Py_True);

    Py_DECREF(rObj);

    ಪ್ರತ್ಯಾಗಮ ret;
}

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    ಪೂರ್ಣಾಙ್ಕ		dii_changed;
    hashtab_T	*dii_ht;
    hashitem_T	*dii_hi;
    long_u	dii_todo;
} dictiterinfo_T;

    ಸ್ಥಿರ PyObject *
DictionaryIterNext(ಶೂನ್ಯ **arg)
{
    PyObject	*ret;
    dictiterinfo_T **dii = (dictiterinfo_T**)arg;

    ಯದಿ (!(*dii)->dii_todo)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ ((*dii)->dii_ht->ht_changed != (*dii)->dii_changed)
    {
	PyErr_SET_STRING(PyExc_RuntimeError,
		N_("hashtab changed during iteration"));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಯಾವತ್ (((*dii)->dii_todo) && HASHITEM_EMPTY((*dii)->dii_hi))
	++((*dii)->dii_hi);

    --((*dii)->dii_todo);

    ಯದಿ (!(ret = PyBytes_FromString((ವರ್ಣ *)(*dii)->dii_hi->hi_ಕೀಲಿ)))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಪ್ರತ್ಯಾಗಮ ret;
}

    ಸ್ಥಿರ PyObject *
DictionaryIter(PyObject *self_obj)
{
    DictionaryObject	*self = (DictionaryObject*)self_obj;
    dictiterinfo_T	*dii;
    hashtab_T		*ht;

    ಯದಿ (!(dii = PyMem_New(dictiterinfo_T, 1)))
    {
	PyErr_NoMemory();
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ht = &self->dict->dv_hashtab;
    dii->dii_changed = ht->ht_changed;
    dii->dii_ht = ht;
    dii->dii_hi = ht->ht_array;
    dii->dii_todo = ht->ht_used;

    ಪ್ರತ್ಯಾಗಮ IterNew(dii,
	    PyMem_Free, DictionaryIterNext,
	    ರಿಕ್ತ, ರಿಕ್ತ, (PyObject *)self);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
DictionaryAssItem(
	DictionaryObject *self, PyObject *ಕೀಲಿ_Object, PyObject *valObject)
{
    char_u	*ಕೀಲಿ;
    typval_T	tv;
    dict_T	*dict = self->dict;
    dictitem_T	*di;
    PyObject	*todecref;

    ಯದಿ (dict->dv_lock)
    {
	RAISE_LOCKED_DICTIONARY;
	ಪ್ರತ್ಯಾಗಮ -1;
    }

    ಯದಿ (!(ಕೀಲಿ = StringToChars(ಕೀಲಿ_Object, &todecref)))
	ಪ್ರತ್ಯಾಗಮ -1;

    ಯದಿ (*ಕೀಲಿ == NUL)
    {
	RAISE_NO_EMPTY_ಕೀಲಿಗಳು;
	Py_XDECREF(todecref);
	ಪ್ರತ್ಯಾಗಮ -1;
    }

    di = dict_find(dict, ಕೀಲಿ, -1);

    ಯದಿ (valObject == ರಿಕ್ತ)
    {
	hashitem_T	*hi;

	ಯದಿ (di == ರಿಕ್ತ)
	{
	    Py_XDECREF(todecref);
	    PyErr_SetObject(Py_Exc_ಕೀಲಿ_Error, ಕೀಲಿ_Object);
	    ಪ್ರತ್ಯಾಗಮ -1;
	}
	hi = hash_find(&dict->dv_hashtab, di->di_ಕೀಲಿ);
	hash_remove(&dict->dv_hashtab, hi, "Python remove item");
	dictitem_free(di);
	Py_XDECREF(todecref);
	ಪ್ರತ್ಯಾಗಮ 0;
    }

    ಯದಿ (ConvertFromPyObject(valObject, &tv) == -1)
    {
	Py_XDECREF(todecref);
	ಪ್ರತ್ಯಾಗಮ -1;
    }

    ಯದಿ (di == ರಿಕ್ತ)
    {
	ಯದಿ (!(di = dictitem_alloc(ಕೀಲಿ)))
	{
	    Py_XDECREF(todecref);
	    PyErr_NoMemory();
	    ಪ್ರತ್ಯಾಗಮ -1;
	}
	di->di_tv.v_type = VAR_UNKNOWN;

	ಯದಿ (dict_add(dict, di) == FAIL)
	{
	    dictitem_free(di);
	    RAISE_ಕೀಲಿ_ADD_FAIL(ಕೀಲಿ);
	    Py_XDECREF(todecref);
	    ಪ್ರತ್ಯಾಗಮ -1;
	}
    }
    ಅನ್ಯಥಾ
	clear_tv(&di->di_tv);

    Py_XDECREF(todecref);

    copy_tv(&tv, &di->di_tv);
    clear_tv(&tv);
    ಪ್ರತ್ಯಾಗಮ 0;
}

ಪ್ರಕಾರನಿರ್ದೇಶ PyObject *(*hi_to_py)(hashitem_T *);

    ಸ್ಥಿರ PyObject *
DictionaryListObjects(DictionaryObject *self, hi_to_py hiconvert)
{
    dict_T	*dict = self->dict;
    long_u	todo = dict->dv_hashtab.ht_used;
    Py_ssize_t	i = 0;
    PyObject	*ret;
    hashitem_T	*hi;
    PyObject	*newObj;

    ret = PyList_New(todo);
    ಪ್ರತಿ (hi = dict->dv_hashtab.ht_array; todo > 0; ++hi)
    {
	ಯದಿ (!HASHITEM_EMPTY(hi))
	{
	    ಯದಿ (!(newObj = hiconvert(hi)))
	    {
		Py_DECREF(ret);
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    }
	    PyList_SET_ITEM(ret, i, newObj);
	    --todo;
	    ++i;
	}
    }
    ಪ್ರತ್ಯಾಗಮ ret;
}

    ಸ್ಥಿರ PyObject *
dict_ಕೀಲಿ(hashitem_T *hi)
{
    ಪ್ರತ್ಯಾಗಮ PyBytes_FromString((ವರ್ಣ *)(hi->hi_ಕೀಲಿ));
}

    ಸ್ಥಿರ PyObject *
Dictionary_List_ಕೀಲಿಗಳು(DictionaryObject *self, PyObject *args UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ DictionaryListObjects(self, dict_ಕೀಲಿ);
}

    ಸ್ಥಿರ PyObject *
dict_val(hashitem_T *hi)
{
    dictitem_T	*di;

    di = dict_lookup(hi);
    ಪ್ರತ್ಯಾಗಮ ConvertToPyObject(&di->di_tv);
}

    ಸ್ಥಿರ PyObject *
Dictionary_List_ಮೌಲ್ಯಗಳು(DictionaryObject *self, PyObject *args UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ DictionaryListObjects(self, dict_val);
}

    ಸ್ಥಿರ PyObject *
dict_item(hashitem_T *hi)
{
    PyObject	*ಕೀಲಿ_Object;
    PyObject	*valObject;
    PyObject	*ret;

    ಯದಿ (!(ಕೀಲಿ_Object = dict_ಕೀಲಿ(hi)))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (!(valObject = dict_val(hi)))
    {
	Py_DECREF(ಕೀಲಿ_Object);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ret = Py_Build_ಮೌಲ್ಯ("(OO)", ಕೀಲಿ_Object, valObject);

    Py_DECREF(ಕೀಲಿ_Object);
    Py_DECREF(valObject);

    ಪ್ರತ್ಯಾಗಮ ret;
}

    ಸ್ಥಿರ PyObject *
Dictionary_List_ವಸ್ತುಗಳು(DictionaryObject *self, PyObject *args UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ DictionaryListObjects(self, dict_item);
}

    ಸ್ಥಿರ PyObject *
DictionaryUpdate(DictionaryObject *self, PyObject *args, PyObject *kwargs)
{
    dict_T	*dict = self->dict;

    ಯದಿ (dict->dv_lock)
    {
	RAISE_LOCKED_DICTIONARY;
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಯದಿ (kwargs)
    {
	typval_T	tv;

	ಯದಿ (ConvertFromPyMapping(kwargs, &tv) == -1)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	VimTryStart();
	dict_extend(self->dict, tv.vval.v_dict, (char_u *) "force", ರಿಕ್ತ);
	clear_tv(&tv);
	ಯದಿ (VimTryEnd())
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಅನ್ಯಥಾ
    {
	PyObject	*obj = ರಿಕ್ತ;

	ಯದಿ (!PyArg_ParseTuple(args, "|O", &obj))
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	ಯದಿ (obj == ರಿಕ್ತ)
	{
	    Py_INCREF(Py_None);
	    ಪ್ರತ್ಯಾಗಮ Py_None;
	}

	ಯದಿ (PyObject_HasAttrString(obj, "keys"))
	    ಪ್ರತ್ಯಾಗಮ DictionaryUpdate(self, ರಿಕ್ತ, obj);
	ಅನ್ಯಥಾ
	{
	    PyObject	*iterator;
	    PyObject	*item;

	    ಯದಿ (!(iterator = PyObject_GetIter(obj)))
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	    ಯಾವತ್ ((item = PyIter_Next(iterator)))
	    {
		PyObject	*fast;
		PyObject	*ಕೀಲಿ_Object;
		PyObject	*valObject;
		PyObject	*todecref;
		char_u		*ಕೀಲಿ;
		dictitem_T	*di;
		hashitem_T	*hi;

		ಯದಿ (!(fast = PySequence_Fast(item, "")))
		{
		    Py_DECREF(iterator);
		    Py_DECREF(item);
		    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		}

		Py_DECREF(item);

		ಯದಿ (PySequence_Size(fast) != 2)
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    PyErr_FORMAT(Py_Exc_ಮೌಲ್ಯ_Error,
			    N_("expected sequence element of size 2, "
			    "but got sequence of size %d"),
			    (ಪೂರ್ಣಾಙ್ಕ) PySequence_Size(fast));
		    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		}

		ಕೀಲಿ_Object = PySequence_GetItem(fast, 0);

		ಯದಿ (!(ಕೀಲಿ = StringToChars(ಕೀಲಿ_Object, &todecref)))
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		}

		di = dictitem_alloc(ಕೀಲಿ);

		Py_XDECREF(todecref);

		ಯದಿ (di == ರಿಕ್ತ)
		{
		    Py_DECREF(fast);
		    Py_DECREF(iterator);
		    PyErr_NoMemory();
		    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		}
		di->di_tv.v_type = VAR_UNKNOWN;

		valObject = PySequence_GetItem(fast, 1);

		ಯದಿ (ConvertFromPyObject(valObject, &di->di_tv) == -1)
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    dictitem_free(di);
		    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		}

		Py_DECREF(fast);

		hi = hash_find(&dict->dv_hashtab, di->di_ಕೀಲಿ);
		ಯದಿ (!HASHITEM_EMPTY(hi) || dict_add(dict, di) == FAIL)
		{
		    RAISE_ಕೀಲಿ_ADD_FAIL(di->di_ಕೀಲಿ);
		    Py_DECREF(iterator);
		    dictitem_free(di);
		    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		}
	    }

	    Py_DECREF(iterator);

	    // Iterator may have finished due to an exception
	    ಯದಿ (PyErr_Occurred())
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
    }
    Py_INCREF(Py_None);
    ಪ್ರತ್ಯಾಗಮ Py_None;
}

    ಸ್ಥಿರ PyObject *
DictionaryGet(DictionaryObject *self, PyObject *args)
{
    ಪ್ರತ್ಯಾಗಮ _DictionaryItem(self, args,
			    DICT_FLAG_HAS_DEFAULT|DICT_FLAG_NONE_DEFAULT);
}

    ಸ್ಥಿರ PyObject *
Dictionary_ತೆಗೆ(DictionaryObject *self, PyObject *args)
{
    ಪ್ರತ್ಯಾಗಮ _DictionaryItem(self, args, DICT_FLAG_HAS_DEFAULT|DICT_FLAG_ತೆಗೆ);
}

    ಸ್ಥಿರ PyObject *
Dictionary_ತೆಗೆ_Item(DictionaryObject *self, PyObject *args UNUSED)
{
    hashitem_T	*hi;
    PyObject	*ret;
    PyObject	*valObject;
    dictitem_T	*di;

    ಯದಿ (self->dict->dv_hashtab.ht_used == 0)
    {
	PyErr_SetNone(Py_Exc_ಕೀಲಿ_Error);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    hi = self->dict->dv_hashtab.ht_array;
    ಯಾವತ್ (HASHITEM_EMPTY(hi))
	++hi;

    di = dict_lookup(hi);

    ಯದಿ (!(valObject = ConvertToPyObject(&di->di_tv)))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (!(ret = Py_Build_ಮೌಲ್ಯ("(" Py_bytes_fmt "O)", hi->hi_ಕೀಲಿ, valObject)))
    {
	Py_DECREF(valObject);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    hash_remove(&self->dict->dv_hashtab, hi, "Python pop item");
    dictitem_free(di);

    ಪ್ರತ್ಯಾಗಮ ret;
}

    ಸ್ಥಿರ PyObject *
Dictionary_Has_ಕೀಲಿ(DictionaryObject *self, PyObject *ಕೀಲಿ_Object)
{
    ಪ್ರತ್ಯಾಗಮ _DictionaryItem(self, ಕೀಲಿ_Object, DICT_FLAG_RETURN_BOOL);
}

ಸ್ಥಿರ PySequenceMethods DictionaryAsSeq = {
    0,					// sq_length
    0,					// sq_concat
    0,					// sq_repeat
    0,					// sq_item
    0,					// sq_slice
    0,					// sq_ass_item
    0,					// sq_ass_slice
    (objobjproc) DictionaryContains,	// sq_contains
    0,					// sq_inplace_concat
    0,					// sq_inplace_repeat
};

ಸ್ಥಿರ PyMappingMethods DictionaryAsMapping = {
    (lenfunc)       DictionaryLength,
    (binaryfunc)    DictionaryItem,
    (objobjargproc) DictionaryAssItem,
};

ಸ್ಥಿರ ಸಂರಚನಾ PyMethodDef DictionaryMethods[] = {
    {"keys",	(PyCFunction)Dictionary_List_ಕೀಲಿಗಳು,	METH_NOARGS,	""},
    {"values",	(PyCFunction)Dictionary_List_ಮೌಲ್ಯಗಳು,	METH_NOARGS,	""},
    {"items",	(PyCFunction)Dictionary_List_ವಸ್ತುಗಳು,	METH_NOARGS,	""},
    {"update",	(PyCFunction)(ಶೂನ್ಯ *)DictionaryUpdate,		METH_VARARGS|METH_KEYWORDS, ""},
    {"get",	(PyCFunction)DictionaryGet,		METH_VARARGS,	""},
    {"pop",	(PyCFunction)Dictionary_ತೆಗೆ,		METH_VARARGS,	""},
    {"popitem",	(PyCFunction)Dictionary_ತೆಗೆ_Item,		METH_NOARGS,	""},
    {"has_key",	(PyCFunction)Dictionary_Has_ಕೀಲಿ,		METH_O,		""},
    {"__dir__",	(PyCFunction)DictionaryDir,		METH_NOARGS,	""},
    { ರಿಕ್ತ,	ರಿಕ್ತ,					0,		ರಿಕ್ತ}
};

DEFINE_PY_TYPE_OBJECT(ListType);

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    PyObject_HEAD
    list_T	*list;
    pylinkedlist_T	ref;
} ListObject;

#define NEW_LIST(list) ListNew(ListTypePtr, list)

    ಸ್ಥಿರ PyObject *
ListNew(PyTypeObject *subtype, list_T *list)
{
    ListObject	*self;

    ಯದಿ (list == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    self = (ListObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    ಯದಿ (self == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    self->list = list;
    ++list->lv_refcount;
    CHECK_LIST_MATERIALIZE(list);

    pyll_add((PyObject *)(self), &self->ref, &lastlist);

    ಪ್ರತ್ಯಾಗಮ (PyObject *)(self);
}

    ಸ್ಥಿರ list_T *
py_list_alloc(ಶೂನ್ಯ)
{
    list_T	*ret;

    ಯದಿ (!(ret = list_alloc()))
    {
	PyErr_NoMemory();
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ++ret->lv_refcount;

    ಪ್ರತ್ಯಾಗಮ ret;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
list_py_concat(list_T *l, PyObject *obj, PyObject *lookup_dict)
{
    PyObject	*iterator;
    PyObject	*item;
    listitem_T	*li;

    ಯದಿ (!(iterator = PyObject_GetIter(obj)))
	ಪ್ರತ್ಯಾಗಮ -1;

    ಯಾವತ್ ((item = PyIter_Next(iterator)))
    {
	ಯದಿ (!(li = listitem_alloc()))
	{
	    PyErr_NoMemory();
	    Py_DECREF(item);
	    Py_DECREF(iterator);
	    ಪ್ರತ್ಯಾಗಮ -1;
	}
	li->li_tv.v_lock = 0;
	li->li_tv.v_type = VAR_UNKNOWN;

	ಯದಿ (_ConvertFromPyObject(item, &li->li_tv, lookup_dict) == -1)
	{
	    Py_DECREF(item);
	    Py_DECREF(iterator);
	    listitem_free(l, li);
	    ಪ್ರತ್ಯಾಗಮ -1;
	}

	Py_DECREF(item);

	list_append(l, li);
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    ಯದಿ (PyErr_Occurred())
	ಪ್ರತ್ಯಾಗಮ -1;

    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಸ್ಥಿರ PyObject *
ListConstructor(PyTypeObject *subtype, PyObject *args, PyObject *kwargs)
{
    list_T	*list;
    PyObject	*obj = ರಿಕ್ತ;

    ಯದಿ (kwargs)
    {
	PyErr_SET_STRING(PyExc_TypeError,
		N_("list constructor does not accept keyword arguments"));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಯದಿ (!PyArg_ParseTuple(args, "|O", &obj))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (!(list = py_list_alloc()))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (obj)
    {
	PyObject	*lookup_dict;

	ಯದಿ (!(lookup_dict = PyDict_New()))
	{
	    list_unref(list);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}

	ಯದಿ (list_py_concat(list, obj, lookup_dict) == -1)
	{
	    Py_DECREF(lookup_dict);
	    list_unref(list);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}

	Py_DECREF(lookup_dict);
    }

    ಪ್ರತ್ಯಾಗಮ ListNew(subtype, list);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ListDestructor(PyObject *self_obj)
{
    ListObject *self = (ListObject*)self_obj;
    pyll_remove(&self->ref, &lastlist);
    list_unref(self->list);

    DESTRUCTOR_FINISH(self);
}

    ಸ್ಥಿರ PyInt
ListLength(ListObject *self)
{
    ಪ್ರತ್ಯಾಗಮ ((PyInt) (self->list->lv_len));
}

    ಸ್ಥಿರ PyObject *
ListIndex(ListObject *self, Py_ssize_t index)
{
    listitem_T	*li;

    ಯದಿ (index >= ListLength(self))
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("list index out of range"));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    li = list_find(self->list, (long) index);
    ಯದಿ (li == ರಿಕ್ತ)
    {
	// No more suitable format specifications in python-2.3
	PyErr_VIM_FORMAT(N_("internal error: failed to get Vim list item %d"),
		(ಪೂರ್ಣಾಙ್ಕ) index);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತ್ಯಾಗಮ ConvertToPyObject(&li->li_tv);
}

    ಸ್ಥಿರ PyObject *
ListSlice(ListObject *self, Py_ssize_t first, Py_ssize_t step,
	  Py_ssize_t slicelen)
{
    PyInt	i;
    PyObject	*list;

    ಯದಿ (step == 0)
    {
	PyErr_SET_STRING(Py_Exc_ಮೌಲ್ಯ_Error, N_("slice step cannot be zero"));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    list = PyList_New(slicelen);
    ಯದಿ (list == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಪ್ರತಿ (i = 0; i < slicelen; ++i)
    {
	PyObject	*item;

	item = ListIndex(self, first + i*step);
	ಯದಿ (item == ರಿಕ್ತ)
	{
	    Py_DECREF(list);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}

	PyList_SET_ITEM(list, i, item);
    }

    ಪ್ರತ್ಯಾಗಮ list;
}

    ಸ್ಥಿರ PyObject *
ListItem(ListObject *self, PyObject* idx)
{
#ಯದಿ PY_MAJOR_VERSION < 3
    ಯದಿ (PyInt_Check(idx))
    {
	long _idx = PyInt_AsLong(idx);
	ಪ್ರತ್ಯಾಗಮ ListIndex(self, _idx);
    }
    ಅನ್ಯಥಾ
#endif
    ಯದಿ (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	ಪ್ರತ್ಯಾಗಮ ListIndex(self, _idx);
    }
    ಅನ್ಯಥಾ ಯದಿ (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	ಯದಿ (PySlice_GetIndicesEx((PySliceObject_T *)idx, ListLength(self),
				 &start, &stop, &step, &slicelen) < 0)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	ಪ್ರತ್ಯಾಗಮ ListSlice(self, start, step, slicelen);
    }
    ಅನ್ಯಥಾ
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
list_restore(Py_ssize_t numadded, Py_ssize_t numreplaced, Py_ssize_t slicelen,
	list_T *l, listitem_T **lis, listitem_T *lastaddedli)
{
    ಯಾವತ್ (numreplaced--)
    {
	list_ಸೇರಿಸು(l, lis[numreplaced], lis[slicelen + numreplaced]);
	listitem_remove(l, lis[slicelen + numreplaced]);
    }
    ಯಾವತ್ (numadded--)
    {
	listitem_T	*next;

	next = lastaddedli->li_prev;
	listitem_remove(l, lastaddedli);
	lastaddedli = next;
    }
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ListAssSlice(ListObject *self, Py_ssize_t first,
	     Py_ssize_t step, Py_ssize_t slicelen, PyObject *obj)
{
    PyObject	*iterator;
    PyObject	*item;
    listitem_T	*li;
    listitem_T	*lastaddedli = ರಿಕ್ತ;
    listitem_T	*next;
    typval_T	v;
    list_T	*l = self->list;
    PyInt	i;
    PyInt	j;
    PyInt	numreplaced = 0;
    PyInt	numadded = 0;
    PyInt	size;
    listitem_T	**lis = ರಿಕ್ತ;

    size = ListLength(self);

    ಯದಿ (l->lv_lock)
    {
	RAISE_LOCKED_LIST;
	ಪ್ರತ್ಯಾಗಮ -1;
    }

    ಯದಿ (step == 0)
    {
	PyErr_SET_STRING(Py_Exc_ಮೌಲ್ಯ_Error, N_("slice step cannot be zero"));
	ಪ್ರತ್ಯಾಗಮ -1;
    }

    ಯದಿ (step != 1 && slicelen == 0)
    {
	// Nothing to do. Only error out if obj has some items.
	ಪೂರ್ಣಾಙ್ಕ		ret = 0;

	ಯದಿ (obj == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ 0;

	ಯದಿ (!(iterator = PyObject_GetIter(obj)))
	    ಪ್ರತ್ಯಾಗಮ -1;

	ಯದಿ ((item = PyIter_Next(iterator)))
	{
	    PyErr_FORMAT(Py_Exc_ಮೌಲ್ಯ_Error,
		    N_("attempt to assign sequence of size greater than %d "
			"to extended slice"), 0);
	    Py_DECREF(item);
	    ret = -1;
	}
	Py_DECREF(iterator);
	ಪ್ರತ್ಯಾಗಮ ret;
    }

    ಯದಿ (obj != ರಿಕ್ತ)
	// XXX May allocate zero bytes.
	ಯದಿ (!(lis = PyMem_New(listitem_T *, slicelen * 2)))
	{
	    PyErr_NoMemory();
	    ಪ್ರತ್ಯಾಗಮ -1;
	}

    ಯದಿ (first == size)
	li = ರಿಕ್ತ;
    ಅನ್ಯಥಾ
    {
	li = list_find(l, (long) first);
	ಯದಿ (li == ರಿಕ್ತ)
	{
	    PyErr_VIM_FORMAT(N_("internal error: no Vim list item %d"),
		    (ಪೂರ್ಣಾಙ್ಕ)first);
	    ಯದಿ (obj != ರಿಕ್ತ)
		PyMem_Free(lis);
	    ಪ್ರತ್ಯಾಗಮ -1;
	}
	i = slicelen;
	ಯಾವತ್ (i-- && li != ರಿಕ್ತ)
	{
	    j = step;
	    next = li;
	    ಯದಿ (step > 0)
		ಯಾವತ್ (next != ರಿಕ್ತ && ((next = next->li_next) != ರಿಕ್ತ) && --j);
	    ಅನ್ಯಥಾ
		ಯಾವತ್ (next != ರಿಕ್ತ && ((next = next->li_prev) != ರಿಕ್ತ) && ++j);

	    ಯದಿ (obj == ರಿಕ್ತ)
		listitem_remove(l, li);
	    ಅನ್ಯಥಾ
		lis[slicelen - i - 1] = li;

	    li = next;
	}
	ಯದಿ (li == ರಿಕ್ತ && i != -1)
	{
	    PyErr_SET_VIM(N_("internal error: not enough list items"));
	    ಯದಿ (obj != ರಿಕ್ತ)
		PyMem_Free(lis);
	    ಪ್ರತ್ಯಾಗಮ -1;
	}
    }

    ಯದಿ (obj == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ 0;

    ಯದಿ (!(iterator = PyObject_GetIter(obj)))
    {
	PyMem_Free(lis);
	ಪ್ರತ್ಯಾಗಮ -1;
    }

    i = 0;
    ಯಾವತ್ ((item = PyIter_Next(iterator)))
    {
	ಯದಿ (ConvertFromPyObject(item, &v) == -1)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(item);
	    PyMem_Free(lis);
	    ಪ್ರತ್ಯಾಗಮ -1;
	}
	Py_DECREF(item);
	ಯದಿ (list_ಸೇರಿಸು_tv(l, &v, numreplaced < slicelen
				    ? lis[numreplaced]
				    : li) == FAIL)
	{
	    clear_tv(&v);
	    PyErr_SET_VIM(N_("internal error: failed to add item to list"));
	    list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	    PyMem_Free(lis);
	    ಪ್ರತ್ಯಾಗಮ -1;
	}
	ಯದಿ (numreplaced < slicelen)
	{
	    lis[slicelen + numreplaced] = lis[numreplaced]->li_prev;
	    vimlist_remove(l, lis[numreplaced], lis[numreplaced]);
	    numreplaced++;
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (li)
		lastaddedli = li->li_prev;
	    ಅನ್ಯಥಾ
		lastaddedli = l->lv_u.mat.lv_last;
	    numadded++;
	}
	clear_tv(&v);
	ಯದಿ (step != 1 && i >= slicelen)
	{
	    Py_DECREF(iterator);
	    PyErr_FORMAT(Py_Exc_ಮೌಲ್ಯ_Error,
		    N_("attempt to assign sequence of size greater than %d "
			"to extended slice"), (ಪೂರ್ಣಾಙ್ಕ) slicelen);
	    list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	    PyMem_Free(lis);
	    ಪ್ರತ್ಯಾಗಮ -1;
	}
	++i;
    }
    Py_DECREF(iterator);

    ಯದಿ (step != 1 && i != slicelen)
    {
	PyErr_FORMAT2(Py_Exc_ಮೌಲ್ಯ_Error,
		N_("attempt to assign sequence of size %d to extended slice "
		    "of size %d"), (ಪೂರ್ಣಾಙ್ಕ) i, (ಪೂರ್ಣಾಙ್ಕ) slicelen);
	list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	PyMem_Free(lis);
	ಪ್ರತ್ಯಾಗಮ -1;
    }

    ಯದಿ (PyErr_Occurred())
    {
	list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	PyMem_Free(lis);
	ಪ್ರತ್ಯಾಗಮ -1;
    }

    ಪ್ರತಿ (i = 0; i < numreplaced; i++)
	listitem_free(l, lis[i]);
    ಯದಿ (step == 1)
	ಪ್ರತಿ (i = numreplaced; i < slicelen; i++)
	    listitem_remove(l, lis[i]);

    PyMem_Free(lis);

    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ListAssIndex(ListObject *self, Py_ssize_t index, PyObject *obj)
{
    typval_T	tv;
    list_T	*l = self->list;
    listitem_T	*li;
    Py_ssize_t	length = ListLength(self);

    ಯದಿ (l->lv_lock)
    {
	RAISE_LOCKED_LIST;
	ಪ್ರತ್ಯಾಗಮ -1;
    }
    ಯದಿ (index > length || (index == length && obj == ರಿಕ್ತ))
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("list index out of range"));
	ಪ್ರತ್ಯಾಗಮ -1;
    }

    ಯದಿ (obj == ರಿಕ್ತ)
    {
	li = list_find(l, (long) index);
	ಯದಿ (li == ರಿಕ್ತ)
	{
	    PyErr_VIM_FORMAT(N_("internal error: failed to get Vim "
			"list item %d"), (ಪೂರ್ಣಾಙ್ಕ) index);
	    ಪ್ರತ್ಯಾಗಮ -1;
	}
	vimlist_remove(l, li, li);
	clear_tv(&li->li_tv);
	vim_free(li);
	ಪ್ರತ್ಯಾಗಮ 0;
    }

    ಯದಿ (ConvertFromPyObject(obj, &tv) == -1)
	ಪ್ರತ್ಯಾಗಮ -1;

    ಯದಿ (index == length)
    {
	ಯದಿ (list_append_tv(l, &tv) == FAIL)
	{
	    clear_tv(&tv);
	    PyErr_SET_VIM(N_("failed to add item to list"));
	    ಪ್ರತ್ಯಾಗಮ -1;
	}
    }
    ಅನ್ಯಥಾ
    {
	li = list_find(l, (long) index);
	ಯದಿ (li == ರಿಕ್ತ)
	{
	    PyErr_VIM_FORMAT(N_("internal error: failed to get Vim "
			"list item %d"), (ಪೂರ್ಣಾಙ್ಕ) index);
	    ಪ್ರತ್ಯಾಗಮ -1;
	}
	clear_tv(&li->li_tv);
	copy_tv(&tv, &li->li_tv);
	clear_tv(&tv);
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ListAssItem(ListObject *self, PyObject *idx, PyObject *obj)
{
#ಯದಿ PY_MAJOR_VERSION < 3
    ಯದಿ (PyInt_Check(idx))
    {
	long _idx = PyInt_AsLong(idx);
	ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)ListAssIndex(self, _idx, obj);
    }
    ಅನ್ಯಥಾ
#endif
    ಯದಿ (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)ListAssIndex(self, _idx, obj);
    }
    ಅನ್ಯಥಾ ಯದಿ (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	ಯದಿ (PySlice_GetIndicesEx((PySliceObject_T *)idx, ListLength(self),
				 &start, &stop, &step, &slicelen) < 0)
	    ಪ್ರತ್ಯಾಗಮ -1;
	ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)ListAssSlice(self, start, step, slicelen,
		obj);
    }
    ಅನ್ಯಥಾ
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	ಪ್ರತ್ಯಾಗಮ -1;
    }
}

    ಸ್ಥಿರ PyObject *
ListConcatInPlace(ListObject *self, PyObject *obj)
{
    list_T	*l = self->list;
    PyObject	*lookup_dict;

    ಯದಿ (l->lv_lock)
    {
	RAISE_LOCKED_LIST;
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಯದಿ (!(lookup_dict = PyDict_New()))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (list_py_concat(l, obj, lookup_dict) == -1)
    {
	Py_DECREF(lookup_dict);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    Py_DECREF(lookup_dict);

    Py_INCREF((PyObject *)self);
    ಪ್ರತ್ಯಾಗಮ (PyObject *)(self);
}

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
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
ListIterNext(ಶೂನ್ಯ **arg)
{
    PyObject	*ret;
    listiterinfo_T **lii = (listiterinfo_T**)arg;

    ಯದಿ (!((*lii)->lw.lw_item))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (!(ret = ConvertToPyObject(&((*lii)->lw.lw_item->li_tv))))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    (*lii)->lw.lw_item = (*lii)->lw.lw_item->li_next;

    ಪ್ರತ್ಯಾಗಮ ret;
}

    ಸ್ಥಿರ PyObject *
ListIter(PyObject *self_obj)
{
    ListObject	*self = (ListObject*)self_obj;
    listiterinfo_T	*lii;
    list_T	*l = self->list;

    ಯದಿ (!(lii = PyMem_New(listiterinfo_T, 1)))
    {
	PyErr_NoMemory();
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    CHECK_LIST_MATERIALIZE(l);
    list_add_watch(l, &lii->lw);
    lii->lw.lw_item = l->lv_first;
    lii->list = l;
    ++l->lv_refcount;

    ಪ್ರತ್ಯಾಗಮ IterNew(lii,
	    ListIterDestruct, ListIterNext,
	    ರಿಕ್ತ, ರಿಕ್ತ, (PyObject *)self);
}

ಸ್ಥಿರ ವರ್ಣ *ListAttrs[] = {
    "locked",
    ರಿಕ್ತ
};

    ಸ್ಥಿರ PyObject *
ListDir(PyObject *self, PyObject *args UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ ObjectDir(self, ListAttrs);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ListSetattr(PyObject *self_obj, ವರ್ಣ *name, PyObject *valObject)
{
    ListObject *self = (ListObject*)self_obj;
    ಯದಿ (valObject == ರಿಕ್ತ)
    {
	PyErr_SET_STRING(PyExc_AttributeError,
		N_("cannot delete vim.List attributes"));
	ಪ್ರತ್ಯಾಗಮ -1;
    }

    ಯದಿ (strcmp(name, "locked") == 0)
    {
	ಯದಿ (self->list->lv_lock == VAR_FIXED)
	{
	    PyErr_SET_STRING(PyExc_TypeError, N_("cannot modify fixed list"));
	    ಪ್ರತ್ಯಾಗಮ -1;
	}
	ಅನ್ಯಥಾ
	{
	    ಪೂರ್ಣಾಙ್ಕ		istrue = PyObject_IsTrue(valObject);
	    ಯದಿ (istrue == -1)
		ಪ್ರತ್ಯಾಗಮ -1;
	    ಅನ್ಯಥಾ ಯದಿ (istrue)
		self->list->lv_lock = VAR_LOCKED;
	    ಅನ್ಯಥಾ
		self->list->lv_lock = 0;
	}
	ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಅನ್ಯಥಾ
    {
	PyErr_FORMAT(PyExc_AttributeError, N_("cannot set attribute %s"), name);
	ಪ್ರತ್ಯಾಗಮ -1;
    }
}

ಸ್ಥಿರ PySequenceMethods ListAsSeq = {
    (lenfunc)		ListLength,	 // sq_length,	  len(x)
    (binaryfunc)	0,		 // RangeConcat, sq_concat,  x+y
    0,					 // RangeRepeat, sq_repeat,  x*n
    (PyIntArgFunc)	ListIndex,	 // sq_item,	  x[i]
    0,					 // was_sq_slice,     x[i:j]
    (PyIntObjArgProc)	ListAssIndex,	 // sq_as_item,  x[i]=v
    0,					 // was_sq_ass_slice, x[i:j]=v
    0,					 // sq_contains
    (binaryfunc)	ListConcatInPlace,// sq_inplace_concat
    0,					 // sq_inplace_repeat
};

ಸ್ಥಿರ PyMappingMethods ListAsMapping = {
    /* mp_length	*/ (lenfunc) ListLength,
    /* mp_subscript     */ (binaryfunc) ListItem,
    /* mp_ass_subscript */ (objobjargproc) ListAssItem,
};

ಸ್ಥಿರ ಸಂರಚನಾ PyMethodDef ListMethods[] = {
    {"extend",	(PyCFunction)ListConcatInPlace,	METH_O,		""},
    {"__dir__",	(PyCFunction)ListDir,		METH_NOARGS,	""},
    { ರಿಕ್ತ,	ರಿಕ್ತ,				0,		ರಿಕ್ತ}
};

DEFINE_PY_TYPE_OBJECT(TupleType);

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    PyObject_HEAD
    tuple_T	*tuple;
    pylinkedlist_T	ref;
} TupleObject;

#define NEW_TUPLE(tuple) TupleNew(TupleTypePtr, tuple)

    ಸ್ಥಿರ PyObject *
TupleNew(PyTypeObject *subtype, tuple_T *tuple)
{
    TupleObject	*self;

    ಯದಿ (tuple == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    self = (TupleObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    ಯದಿ (self == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    self->tuple = tuple;
    ++tuple->tv_refcount;

    pyll_add((PyObject *)(self), &self->ref, &lasttuple);

    ಪ್ರತ್ಯಾಗಮ (PyObject *)(self);
}

    ಸ್ಥಿರ tuple_T *
py_tuple_alloc(ಶೂನ್ಯ)
{
    tuple_T	*ret;

    ಯದಿ (!(ret = tuple_alloc()))
    {
	PyErr_NoMemory();
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ++ret->tv_refcount;

    ಪ್ರತ್ಯಾಗಮ ret;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
tuple_py_concat(tuple_T *t, PyObject *obj, PyObject *lookup_dict)
{
    PyObject	*iterator;
    PyObject	*item;

    ಯದಿ (!(iterator = PyObject_GetIter(obj)))
	ಪ್ರತ್ಯಾಗಮ -1;

    ಯಾವತ್ ((item = PyIter_Next(iterator)))
    {
	typval_T    new_tv;

	ಯದಿ (_ConvertFromPyObject(item, &new_tv, lookup_dict) == -1)
	{
	    Py_DECREF(item);
	    Py_DECREF(iterator);
	    ಪ್ರತ್ಯಾಗಮ -1;
	}

	Py_DECREF(item);

	ಯದಿ (tuple_append_tv(t, &new_tv) == FAIL)
	{
	    Py_DECREF(iterator);
	    ಪ್ರತ್ಯಾಗಮ -1;
	}
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    ಯದಿ (PyErr_Occurred())
	ಪ್ರತ್ಯಾಗಮ -1;

    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಸ್ಥಿರ PyObject *
TupleConstructor(PyTypeObject *subtype, PyObject *args, PyObject *kwargs)
{
    tuple_T	*tuple;
    PyObject	*obj = ರಿಕ್ತ;

    ಯದಿ (kwargs)
    {
	PyErr_SET_STRING(PyExc_TypeError,
		N_("tuple constructor does not accept keyword arguments"));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಯದಿ (!PyArg_ParseTuple(args, "|O", &obj))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (!(tuple = py_tuple_alloc()))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (obj)
    {
	PyObject	*lookup_dict;

	ಯದಿ (!(lookup_dict = PyDict_New()))
	{
	    tuple_unref(tuple);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}

	ಯದಿ (tuple_py_concat(tuple, obj, lookup_dict) == -1)
	{
	    Py_DECREF(lookup_dict);
	    tuple_unref(tuple);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}

	Py_DECREF(lookup_dict);
    }

    ಪ್ರತ್ಯಾಗಮ TupleNew(subtype, tuple);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
TupleDestructor(PyObject *self_obj)
{
    TupleObject *self = (TupleObject *)self_obj;
    pyll_remove(&self->ref, &lasttuple);
    tuple_unref(self->tuple);

    DESTRUCTOR_FINISH(self);
}

    ಸ್ಥಿರ PyInt
TupleLength(TupleObject *self)
{
    ಪ್ರತ್ಯಾಗಮ ((PyInt)(tuple_len(self->tuple)));
}

    ಸ್ಥಿರ PyObject *
TupleIndex(TupleObject *self, Py_ssize_t index)
{
    PyInt	len = TupleLength(self);

    ಯದಿ (index < 0)
	index = len + index;

    ಯದಿ (index < 0 || index >= len)
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("tuple index out of range"));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತ್ಯಾಗಮ ConvertToPyObject(TUPLE_ITEM(self->tuple, index));
}

/*
 * Return a new tuple object for the tuple slice starting from the index
 * "first" and of length "slicelen" skipping "step" items.
 */
    ಸ್ಥಿರ PyObject *
TupleSlice(
    TupleObject		*self,
    Py_ssize_t		first,
    Py_ssize_t		step,
    Py_ssize_t		slicelen)
{
    PyInt	i;
    PyObject	*tuple;

    tuple = PyTuple_New(slicelen);
    ಯದಿ (tuple == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಪ್ರತಿ (i = 0; i < slicelen; ++i)
    {
	PyObject	*item;

	item = TupleIndex(self, first + i * step);
	ಯದಿ (item == ರಿಕ್ತ)
	{
	    Py_DECREF(tuple);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}

	PyTuple_SetItem(tuple, i, item);
    }

    ಪ್ರತ್ಯಾಗಮ tuple;
}

    ಸ್ಥಿರ PyObject *
TupleItem(TupleObject *self, PyObject* idx)
{
#ಯದಿ PY_MAJOR_VERSION < 3
    ಯದಿ (PyInt_Check(idx))
    {
	long _idx = PyInt_AsLong(idx);
	ಪ್ರತ್ಯಾಗಮ TupleIndex(self, _idx);
    }
    ಅನ್ಯಥಾ
#endif
    ಯದಿ (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	ಪ್ರತ್ಯಾಗಮ TupleIndex(self, _idx);
    }
    ಅನ್ಯಥಾ ಯದಿ (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	ಯದಿ (PySlice_GetIndicesEx((PySliceObject_T *)idx, TupleLength(self),
				 &start, &stop, &step, &slicelen) < 0)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	ಪ್ರತ್ಯಾಗಮ TupleSlice(self, start, step, slicelen);
    }
    ಅನ್ಯಥಾ
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
}

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    tuple_T	*tuple;
    ಪೂರ್ಣಾಙ್ಕ		index;
} tupleiterinfo_T;

    ಸ್ಥಿರ ಶೂನ್ಯ
TupleIterDestruct(ಶೂನ್ಯ *arg)
{
    tupleiterinfo_T *tii = (tupleiterinfo_T*)arg;
    tuple_unref(tii->tuple);
    PyMem_Free(tii);
}

    ಸ್ಥಿರ PyObject *
TupleIterNext(ಶೂನ್ಯ **arg)
{
    PyObject	*ret;
    tupleiterinfo_T **tii = (tupleiterinfo_T**)arg;

    ಯದಿ ((*tii)->index >= TUPLE_LEN((*tii)->tuple))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (!(ret = ConvertToPyObject(TUPLE_ITEM((*tii)->tuple, (*tii)->index))))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    (*tii)->index++;

    ಪ್ರತ್ಯಾಗಮ ret;
}

    ಸ್ಥಿರ PyObject *
TupleIter(PyObject *self_obj)
{
    TupleObject	*self = (TupleObject*)self_obj;
    tupleiterinfo_T	*tii;
    tuple_T	*t = self->tuple;

    ಯದಿ (!(tii = PyMem_New(tupleiterinfo_T, 1)))
    {
	PyErr_NoMemory();
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    tii->tuple = t;
    tii->index = 0;
    ++t->tv_refcount;

    ಪ್ರತ್ಯಾಗಮ IterNew(tii,
	    TupleIterDestruct, TupleIterNext,
	    ರಿಕ್ತ, ರಿಕ್ತ, (PyObject *)self);
}

ಸ್ಥಿರ ವರ್ಣ *TupleAttrs[] = {
    "locked",
    ರಿಕ್ತ
};

    ಸ್ಥಿರ PyObject *
TupleDir(PyObject *self, PyObject *args UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ ObjectDir(self, TupleAttrs);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
TupleSetattr(PyObject *self_obj, ವರ್ಣ *name, PyObject *valObject)
{
    TupleObject *self = (TupleObject*)self_obj;
    ಯದಿ (valObject == ರಿಕ್ತ)
    {
	PyErr_SET_STRING(PyExc_AttributeError,
		N_("cannot delete vim.Tuple attributes"));
	ಪ್ರತ್ಯಾಗಮ -1;
    }

    ಯದಿ (strcmp(name, "locked") == 0)
    {
	ಯದಿ (self->tuple->tv_lock == VAR_FIXED)
	{
	    PyErr_SET_STRING(PyExc_TypeError, N_("cannot modify fixed tuple"));
	    ಪ್ರತ್ಯಾಗಮ -1;
	}
	ಅನ್ಯಥಾ
	{
	    ಪೂರ್ಣಾಙ್ಕ		istrue = PyObject_IsTrue(valObject);
	    ಯದಿ (istrue == -1)
		ಪ್ರತ್ಯಾಗಮ -1;
	    ಅನ್ಯಥಾ ಯದಿ (istrue)
		self->tuple->tv_lock = VAR_LOCKED;
	    ಅನ್ಯಥಾ
		self->tuple->tv_lock = 0;
	}
	ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಅನ್ಯಥಾ
    {
	PyErr_FORMAT(PyExc_AttributeError, N_("cannot set attribute %s"), name);
	ಪ್ರತ್ಯಾಗಮ -1;
    }
}

ಸ್ಥಿರ PySequenceMethods TupleAsSeq = {
    (lenfunc)		TupleLength,	 // sq_length,	  len(x)
    (binaryfunc)	0,		 // RangeConcat, sq_concat,  x+y
    0,					 // RangeRepeat, sq_repeat,  x*n
    (PyIntArgFunc)	TupleIndex,	 // sq_item,	  x[i]
    0,					 // was_sq_slice,     x[i:j]
    (PyIntObjArgProc)	0,		 // sq_as_item,  x[i]=v
    0,					 // was_sq_ass_slice, x[i:j]=v
    0,					 // sq_contains
    (binaryfunc)	0,		 // sq_inplace_concat
    0,					 // sq_inplace_repeat
};

ಸ್ಥಿರ PyMappingMethods TupleAsMapping = {
    /* mp_length	*/ (lenfunc) TupleLength,
    /* mp_subscript     */ (binaryfunc) TupleItem,
    /* mp_ass_subscript */ (objobjargproc) 0,
};

ಸ್ಥಿರ ಸಂರಚನಾ PyMethodDef TupleMethods[] = {
    {"__dir__",	(PyCFunction)TupleDir,		METH_NOARGS,	""},
    { ರಿಕ್ತ,	ರಿಕ್ತ,				0,		ರಿಕ್ತ}
};

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    PyObject_HEAD
    char_u	*name;
    ಪೂರ್ಣಾಙ್ಕ		argc;
    typval_T	*argv;
    dict_T	*self;
    pylinkedlist_T	ref;
    ಪೂರ್ಣಾಙ್ಕ		auto_rebind;
} FunctionObject;

DEFINE_PY_TYPE_OBJECT(FunctionType);

#define NEW_FUNCTION(name, argc, argv, self, pt_auto) \
    FunctionNew(FunctionTypePtr, (name), (argc), (argv), (self), (pt_auto))

    ಸ್ಥಿರ PyObject *
FunctionNew(PyTypeObject *subtype, char_u *name, ಪೂರ್ಣಾಙ್ಕ argc, typval_T *argv,
	dict_T *selfdict, ಪೂರ್ಣಾಙ್ಕ auto_rebind)
{
    FunctionObject	*self;

    self = (FunctionObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    ಯದಿ (self == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (isdigit((unsigned ವರ್ಣ)*name))
    {
	ಯದಿ (!translated_function_exists(name, FALSE))
	{
	    PyErr_FORMAT(Py_Exc_ಮೌಲ್ಯ_Error,
		    N_("unnamed function %s does not exist"), name);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
	self->name = vim_strsave(name);
    }
    ಅನ್ಯಥಾ
    {
	char_u *p;

	ಯದಿ ((p = get_expanded_name(name,
			    vim_strchr(name, AUTOLOAD_CHAR) == ರಿಕ್ತ)) == ರಿಕ್ತ)
	{
	    PyErr_FORMAT(Py_Exc_ಮೌಲ್ಯ_Error,
		    N_("function %s does not exist"), name);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}

	ಯದಿ (p[0] == K_SPECIAL && p[1] == KS_EXTRA && p[2] == (ಪೂರ್ಣಾಙ್ಕ)KE_SNR)
	{
	    char_u *np;
	    size_t len = STRLEN(p) + 1;

	    ಯದಿ ((np = alloc(len + 2)) == ರಿಕ್ತ)
	    {
		vim_free(p);
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    }
	    mch_memmove(np, "<SNR>", 5);
	    mch_memmove(np + 5, p + 3, len - 3);
	    vim_free(p);
	    self->name = np;
	}
	ಅನ್ಯಥಾ
	    self->name = p;
    }

    func_ref(self->name);
    self->argc = argc;
    self->argv = argv;
    self->self = selfdict;
    self->auto_rebind = selfdict == ರಿಕ್ತ ? TRUE : auto_rebind;

    ಯದಿ (self->argv || self->self)
	pyll_add((PyObject *)(self), &self->ref, &lastfunc);

    ಪ್ರತ್ಯಾಗಮ (PyObject *)(self);
}

    ಸ್ಥಿರ PyObject *
FunctionConstructor(PyTypeObject *subtype, PyObject *args, PyObject *kwargs)
{
    PyObject	*self;
    PyObject	*selfdictObject;
    PyObject	*autoRebindObject;
    PyObject	*argsObject = ರಿಕ್ತ;
    char_u	*name;
    typval_T	selfdicttv;
    typval_T	argstv;
    list_T	*argslist = ರಿಕ್ತ;
    dict_T	*selfdict = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		argc = 0;
    ಪೂರ್ಣಾಙ್ಕ		auto_rebind = TRUE;
    typval_T	*argv = ರಿಕ್ತ;
    typval_T	*curtv;
    listitem_T	*li;

    ಯದಿ (kwargs != ರಿಕ್ತ)
    {
	selfdictObject = PyDict_GetItemString(kwargs, "self");
	ಯದಿ (selfdictObject != ರಿಕ್ತ)
	{
	    ಯದಿ (ConvertFromPyMapping(selfdictObject, &selfdicttv) == -1)
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    selfdict = selfdicttv.vval.v_dict;
	}
	argsObject = PyDict_GetItemString(kwargs, "args");
	ಯದಿ (argsObject != ರಿಕ್ತ)
	{
	    ಯದಿ (ConvertFromPySequence(argsObject, &argstv) == -1)
	    {
		dict_unref(selfdict);
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    }
	    argslist = argstv.vval.v_list;
	    CHECK_LIST_MATERIALIZE(argslist);

	    argc = argslist->lv_len;
	    ಯದಿ (argc != 0)
	    {
		argv = PyMem_New(typval_T, (size_t) argc);
		ಯದಿ (argv == ರಿಕ್ತ)
		{
		    PyErr_NoMemory();
		    dict_unref(selfdict);
		    list_unref(argslist);
		    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		}
		curtv = argv;
		FOR_ALL_LIST_ವಸ್ತುಗಳು(argslist, li)
		    copy_tv(&li->li_tv, curtv++);
	    }
	    list_unref(argslist);
	}
	ಯದಿ (selfdict != ರಿಕ್ತ)
	{
	    auto_rebind = FALSE;
	    autoRebindObject = PyDict_GetItemString(kwargs, "auto_rebind");
	    ಯದಿ (autoRebindObject != ರಿಕ್ತ)
	    {
		auto_rebind = PyObject_IsTrue(autoRebindObject);
		ಯದಿ (auto_rebind == -1)
		{
		    dict_unref(selfdict);
		    list_unref(argslist);
		    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		}
	    }
	}
    }

    ಯದಿ (!PyArg_ParseTuple(args, "et", "ascii", &name))
    {
	dict_unref(selfdict);
	ಯಾವತ್ (argc--)
	    clear_tv(&argv[argc]);
	PyMem_Free(argv);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    self = FunctionNew(subtype, name, argc, argv, selfdict, auto_rebind);

    PyMem_Free(name);

    ಪ್ರತ್ಯಾಗಮ self;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
FunctionDestructor(PyObject *self_obj)
{
    FunctionObject *self = (FunctionObject*)self_obj;
    ಪೂರ್ಣಾಙ್ಕ i;
    func_unref(self->name);
    vim_free(self->name);
    ಪ್ರತಿ (i = 0; i < self->argc; ++i)
	clear_tv(&self->argv[i]);
    PyMem_Free(self->argv);
    dict_unref(self->self);
    ಯದಿ (self->argv || self->self)
	pyll_remove(&self->ref, &lastfunc);

    DESTRUCTOR_FINISH(self);
}

ಸ್ಥಿರ ವರ್ಣ *FunctionAttrs[] = {
    "softspace", "args", "self", "auto_rebind",
    ರಿಕ್ತ
};

    ಸ್ಥಿರ PyObject *
FunctionDir(PyObject *self, PyObject *args UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ ObjectDir(self, FunctionAttrs);
}

    ಸ್ಥಿರ PyObject *
FunctionAttr(FunctionObject *self, ವರ್ಣ *name)
{
    list_T *list;
    ಪೂರ್ಣಾಙ್ಕ i;
    ಯದಿ (strcmp(name, "name") == 0)
	ಪ್ರತ್ಯಾಗಮ PyString_FromString((ವರ್ಣ *)(self->name));
    ಅನ್ಯಥಾ ಯದಿ (strcmp(name, "args") == 0)
    {
	ಯದಿ (self->argv == ರಿಕ್ತ || (list = list_alloc()) == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ ALWAYS_NONE;

	ಪ್ರತಿ (i = 0; i < self->argc; ++i)
	    list_append_tv(list, &self->argv[i]);
	ಪ್ರತ್ಯಾಗಮ NEW_LIST(list);
    }
    ಅನ್ಯಥಾ ಯದಿ (strcmp(name, "self") == 0)
	ಪ್ರತ್ಯಾಗಮ self->self == ರಿಕ್ತ
	    ? ALWAYS_NONE
	    : NEW_DICTIONARY(self->self);
    ಅನ್ಯಥಾ ಯದಿ (strcmp(name, "auto_rebind") == 0)
	ಪ್ರತ್ಯಾಗಮ self->auto_rebind
	    ? ALWAYS_TRUE
	    : ALWAYS_FALSE;
    ಅನ್ಯಥಾ ಯದಿ (strcmp(name, "__members__") == 0)
	ಪ್ರತ್ಯಾಗಮ ObjectDir(ರಿಕ್ತ, FunctionAttrs);
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Populate partial_T given function object.
 *
 * "exported" should be set to true when it is needed to construct a partial
 * that may be stored in a variable (i.e. may be freed by Vim).
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
set_partial(FunctionObject *self, partial_T *pt, ಪೂರ್ಣಾಙ್ಕ exported)
{
    ಪೂರ್ಣಾಙ್ಕ i;

    pt->pt_name = self->name;
    ಯದಿ (self->argv)
    {
	pt->pt_argc = self->argc;
	ಯದಿ (exported)
	{
	    pt->pt_argv = ALLOC_CLEAR_MULT(typval_T, self->argc);
	    ಪ್ರತಿ (i = 0; i < pt->pt_argc; ++i)
		copy_tv(&self->argv[i], &pt->pt_argv[i]);
	}
	ಅನ್ಯಥಾ
	    pt->pt_argv = self->argv;
    }
    ಅನ್ಯಥಾ
    {
	pt->pt_argc = 0;
	pt->pt_argv = ರಿಕ್ತ;
    }
    pt->pt_auto = self->auto_rebind || !exported;
    pt->pt_dict = self->self;
    ಯದಿ (exported && self->self)
	++pt->pt_dict->dv_refcount;
    ಯದಿ (exported)
	pt->pt_name = vim_strsave(pt->pt_name);
    pt->pt_refcount = 1;
}

    ಸ್ಥಿರ PyObject *
FunctionCall(PyObject *self_obj, PyObject *argsObject, PyObject *kwargs)
{
    FunctionObject *self = (FunctionObject*)self_obj;
    char_u	*name = self->name;
    typval_T	args;
    typval_T	selfdicttv;
    typval_T	rettv;
    dict_T	*selfdict = ರಿಕ್ತ;
    PyObject	*selfdictObject;
    PyObject	*ret;
    ಪೂರ್ಣಾಙ್ಕ		error;
    partial_T	pt;
    partial_T	*pt_ptr = ರಿಕ್ತ;

    ಯದಿ (ConvertFromPySequence(argsObject, &args) == -1)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (kwargs != ರಿಕ್ತ)
    {
	selfdictObject = PyDict_GetItemString(kwargs, "self");
	ಯದಿ (selfdictObject != ರಿಕ್ತ)
	{
	    ಯದಿ (ConvertFromPyMapping(selfdictObject, &selfdicttv) == -1)
	    {
		clear_tv(&args);
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    }
	    selfdict = selfdicttv.vval.v_dict;
	}
    }

    ಯದಿ (self->argv || self->self)
    {
	CLEAR_FIELD(pt);
	set_partial(self, &pt, FALSE);
	pt_ptr = &pt;
    }

    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();

    VimTryStart();
    error = func_call(name, &args, pt_ptr, selfdict, &rettv);

    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    ಯದಿ (VimTryEnd())
	ret = ರಿಕ್ತ;
    ಅನ್ಯಥಾ ಯದಿ (error != OK)
    {
	ret = ರಿಕ್ತ;
	PyErr_VIM_FORMAT(N_("failed to run function %s"), (ವರ್ಣ *)name);
    }
    ಅನ್ಯಥಾ
	ret = ConvertToPyObject(&rettv);

    clear_tv(&args);
    clear_tv(&rettv);
    ಯದಿ (selfdict != ರಿಕ್ತ)
	clear_tv(&selfdicttv);

    ಪ್ರತ್ಯಾಗಮ ret;
}

    ಸ್ಥಿರ PyObject *
FunctionRepr(PyObject *self_obj)
{
    FunctionObject *self = (FunctionObject*)self_obj;
    PyObject *ret;
    garray_T repr_ga;
    ಪೂರ್ಣಾಙ್ಕ i;
    char_u *tofree = ರಿಕ್ತ;
    typval_T tv;
    char_u numbuf[NUMBUFLEN];

    ga_init2(&repr_ga, ಪರಿಮಾಣ(ವರ್ಣ), 70);
    ga_concat(&repr_ga, (char_u *)"<vim.Function '");
    ಯದಿ (self->name)
	ga_concat(&repr_ga, self->name);
    ಅನ್ಯಥಾ
	ga_concat(&repr_ga, (char_u *)"<NULL>");
    ga_append(&repr_ga, '\'');
    ಯದಿ (self->argv)
    {
	ga_concat(&repr_ga, (char_u *)", args=[");
	++emsg_silent;
	ಪ್ರತಿ (i = 0; i < self->argc; i++)
	{
	    ಯದಿ (i != 0)
		ga_concat(&repr_ga, (char_u *)", ");
	    ga_concat(&repr_ga, tv2string(&self->argv[i], &tofree, numbuf,
			get_copyID()));
	    vim_free(tofree);
	}
	--emsg_silent;
	ga_append(&repr_ga, ']');
    }
    ಯದಿ (self->self)
    {
	ga_concat(&repr_ga, (char_u *)", self=");
	tv.v_type = VAR_DICT;
	tv.vval.v_dict = self->self;
	++emsg_silent;
	ga_concat(&repr_ga, tv2string(&tv, &tofree, numbuf, get_copyID()));
	--emsg_silent;
	vim_free(tofree);
	ಯದಿ (self->auto_rebind)
	    ga_concat(&repr_ga, (char_u *)", auto_rebind=True");
    }
    ga_append(&repr_ga, '>');
    ret = PyString_FromString((ವರ್ಣ *)repr_ga.ga_data);
    ga_clear(&repr_ga);
    ಪ್ರತ್ಯಾಗಮ ret;
}

ಸ್ಥಿರ ಸಂರಚನಾ PyMethodDef FunctionMethods[] = {
    {"__dir__",	(PyCFunction)FunctionDir,   METH_NOARGS,		""},
    { ರಿಕ್ತ,	ರಿಕ್ತ,			0,				ರಿಕ್ತ}
};

/*
 * Options object
 */

DEFINE_PY_TYPE_OBJECT(OptionsType);

ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*checkfun)(ಶೂನ್ಯ *);

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    PyObject_HEAD
    ಪೂರ್ಣಾಙ್ಕ		opt_type;
    ಶೂನ್ಯ	*from;
    checkfun	Check;
    PyObject	*fromObj;
} OptionsObject;

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
dummy_check(ಶೂನ್ಯ *arg UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಸ್ಥಿರ PyObject *
OptionsNew(ಪೂರ್ಣಾಙ್ಕ opt_type, ಶೂನ್ಯ *from, checkfun Check, PyObject *fromObj)
{
    OptionsObject	*self;

    self = PyObject_GC_New(OptionsObject, OptionsTypePtr);
    ಯದಿ (self == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    self->opt_type = opt_type;
    self->from = from;
    self->Check = Check;
    self->fromObj = fromObj;
    ಯದಿ (fromObj)
	Py_INCREF(fromObj);

    ಪ್ರತ್ಯಾಗಮ (PyObject *)(self);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
OptionsDestructor(PyObject *self_obj)
{
    OptionsObject *self = (OptionsObject*)self_obj;
    PyObject_GC_UnTrack((ಶೂನ್ಯ *)(self));
    Py_XDECREF(self->fromObj);
    PyObject_GC_Del((ಶೂನ್ಯ *)(self));
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
OptionsTraverse(PyObject *self_obj, visitproc visit, ಶೂನ್ಯ *arg)
{
    OptionsObject *self = (OptionsObject*)self_obj;
    Py_VISIT(self->fromObj);
    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
OptionsClear(PyObject *self_obj)
{
    OptionsObject *self = (OptionsObject*)self_obj;
    Py_CLEAR(self->fromObj);
    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಸ್ಥಿರ PyObject *
OptionsItem(OptionsObject *self, PyObject *ಕೀಲಿ_Object)
{
    char_u	*ಕೀಲಿ;
    ಪೂರ್ಣಾಙ್ಕ		flags;
    long	numval;
    char_u	*stringval;
    PyObject	*todecref;

    ಯದಿ (self->Check(self->fromObj))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (!(ಕೀಲಿ = StringToChars(ಕೀಲಿ_Object, &todecref)))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (*ಕೀಲಿ == NUL)
    {
	RAISE_NO_EMPTY_ಕೀಲಿಗಳು;
	Py_XDECREF(todecref);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    flags = get_option_ಮೌಲ್ಯ_strict(ಕೀಲಿ, &numval, &stringval,
				    self->opt_type, self->from);

    Py_XDECREF(todecref);

    ಯದಿ (flags == 0)
    {
	PyErr_SetObject(Py_Exc_ಕೀಲಿ_Error, ಕೀಲಿ_Object);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಯದಿ (flags & SOPT_UNSET)
    {
	Py_INCREF(Py_None);
	ಪ್ರತ್ಯಾಗಮ Py_None;
    }
    ಅನ್ಯಥಾ ಯದಿ (flags & SOPT_BOOL)
    {
	PyObject	*ret;
	ret = numval ? Py_True : Py_False;
	Py_INCREF(ret);
	ಪ್ರತ್ಯಾಗಮ ret;
    }
    ಅನ್ಯಥಾ ಯದಿ (flags & SOPT_ಅಂಕೆ)
	ಪ್ರತ್ಯಾಗಮ PyInt_FromLong(numval);
    ಅನ್ಯಥಾ ಯದಿ (flags & SOPT_STRING)
    {
	ಯದಿ (stringval)
	{
	    PyObject	*ret = PyBytes_FromString((ವರ್ಣ *)stringval);
	    vim_free(stringval);
	    ಪ್ರತ್ಯಾಗಮ ret;
	}
	ಅನ್ಯಥಾ
	{
	    PyErr_SET_STRING(PyExc_RuntimeError,
		    N_("unable to get option value"));
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
    }
    ಅನ್ಯಥಾ
    {
	PyErr_SET_VIM(N_("internal error: unknown option type"));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
OptionsContains(OptionsObject *self, PyObject *ಕೀಲಿ_Object)
{
    char_u	*ಕೀಲಿ;
    PyObject	*todecref;

    ಯದಿ (!(ಕೀಲಿ = StringToChars(ಕೀಲಿ_Object, &todecref)))
	ಪ್ರತ್ಯಾಗಮ -1;

    ಯದಿ (*ಕೀಲಿ == NUL)
    {
	Py_XDECREF(todecref);
	ಪ್ರತ್ಯಾಗಮ 0;
    }

    ಯದಿ (get_option_ಮೌಲ್ಯ_strict(ಕೀಲಿ, ರಿಕ್ತ, ರಿಕ್ತ, self->opt_type, ರಿಕ್ತ))
    {
	Py_XDECREF(todecref);
	ಪ್ರತ್ಯಾಗಮ 1;
    }
    ಅನ್ಯಥಾ
    {
	Py_XDECREF(todecref);
	ಪ್ರತ್ಯಾಗಮ 0;
    }
}

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    ಶೂನ್ಯ	*lastoption;
    ಪೂರ್ಣಾಙ್ಕ		opt_type;
} optiterinfo_T;

    ಸ್ಥಿರ PyObject *
OptionsIterNext(ಶೂನ್ಯ **arg)
{
    char_u	*name;
    optiterinfo_T **oii = (optiterinfo_T**)arg;

    ಯದಿ ((name = option_iter_next(&((*oii)->lastoption), (*oii)->opt_type)))
	ಪ್ರತ್ಯಾಗಮ PyString_FromString((ವರ್ಣ *)name);

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಸ್ಥಿರ PyObject *
OptionsIter(PyObject *self_obj)
{
    OptionsObject	*self = (OptionsObject*)self_obj;
    optiterinfo_T	*oii;

    ಯದಿ (!(oii = PyMem_New(optiterinfo_T, 1)))
    {
	PyErr_NoMemory();
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    oii->opt_type = self->opt_type;
    oii->lastoption = ರಿಕ್ತ;

    ಪ್ರತ್ಯಾಗಮ IterNew(oii,
	    PyMem_Free, OptionsIterNext,
	    ರಿಕ್ತ, ರಿಕ್ತ, (PyObject *)self);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
set_option_ಮೌಲ್ಯ_err(char_u *ಕೀಲಿ, ಪೂರ್ಣಾಙ್ಕ numval, char_u *stringval, ಪೂರ್ಣಾಙ್ಕ opt_flags)
{
    ವರ್ಣ	*errmsg;

    ಯದಿ ((errmsg = set_option_ಮೌಲ್ಯ(ಕೀಲಿ, numval, stringval, opt_flags)))
    {
	ಯದಿ (VimTryEnd())
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	PyErr_SetVim(errmsg);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
set_option_ಮೌಲ್ಯ_for(
	char_u *ಕೀಲಿ,
	ಪೂರ್ಣಾಙ್ಕ numval,
	char_u *stringval,
	ಪೂರ್ಣಾಙ್ಕ opt_flags,
	ಪೂರ್ಣಾಙ್ಕ opt_type,
	ಶೂನ್ಯ *from)
{
    switchwin_T	switchwin;
    bufref_T	save_curbuf;
    ಪೂರ್ಣಾಙ್ಕ		set_ret = 0;

    VimTryStart();
    ವಿಕಲ್ಪ (opt_type)
    {
	ಪಕ್ಷ SREQ_WIN:
	    ಯದಿ (switch_win(&switchwin, (win_T *)from,
			      win_find_tabpage((win_T *)from), FALSE) == FAIL)
	    {
		restore_win(&switchwin, TRUE);
		ಯದಿ (VimTryEnd())
		    ಪ್ರತ್ಯಾಗಮ -1;
		PyErr_SET_VIM(N_("problem while switching windows"));
		ಪ್ರತ್ಯಾಗಮ -1;
	    }
	    set_ret = set_option_ಮೌಲ್ಯ_err(ಕೀಲಿ, numval, stringval, opt_flags);
	    restore_win(&switchwin, TRUE);
	    ಭಙ್ಗ;
	ಪಕ್ಷ SREQ_BUF:
	    switch_buffer(&save_curbuf, (buf_T *)from);
	    set_ret = set_option_ಮೌಲ್ಯ_err(ಕೀಲಿ, numval, stringval, opt_flags);
	    restore_buffer(&save_curbuf);
	    ಭಙ್ಗ;
	ಪಕ್ಷ SREQ_GLOBAL:
	    set_ret = set_option_ಮೌಲ್ಯ_err(ಕೀಲಿ, numval, stringval, opt_flags);
	    ಭಙ್ಗ;
    }
    ಯದಿ (set_ret == FAIL)
	ಪ್ರತ್ಯಾಗಮ -1;
    ಪ್ರತ್ಯಾಗಮ VimTryEnd();
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
OptionsAssItem(OptionsObject *self, PyObject *ಕೀಲಿ_Object, PyObject *valObject)
{
    char_u	*ಕೀಲಿ;
    ಪೂರ್ಣಾಙ್ಕ		flags;
    ಪೂರ್ಣಾಙ್ಕ		opt_flags;
    ಪೂರ್ಣಾಙ್ಕ		ret = 0;
    PyObject	*todecref;

    ಯದಿ (self->Check(self->fromObj))
	ಪ್ರತ್ಯಾಗಮ -1;

    ಯದಿ (!(ಕೀಲಿ = StringToChars(ಕೀಲಿ_Object, &todecref)))
	ಪ್ರತ್ಯಾಗಮ -1;

    ಯದಿ (*ಕೀಲಿ == NUL)
    {
	RAISE_NO_EMPTY_ಕೀಲಿಗಳು;
	Py_XDECREF(todecref);
	ಪ್ರತ್ಯಾಗಮ -1;
    }

    flags = get_option_ಮೌಲ್ಯ_strict(ಕೀಲಿ, ರಿಕ್ತ, ರಿಕ್ತ,
				    self->opt_type, self->from);

    ಯದಿ (flags == 0)
    {
	PyErr_SetObject(Py_Exc_ಕೀಲಿ_Error, ಕೀಲಿ_Object);
	Py_XDECREF(todecref);
	ಪ್ರತ್ಯಾಗಮ -1;
    }

    ಯದಿ (valObject == ರಿಕ್ತ)
    {
	ಯದಿ (self->opt_type == SREQ_GLOBAL)
	{
	    PyErr_FORMAT(Py_Exc_ಮೌಲ್ಯ_Error,
		    N_("unable to unset global option %s"), ಕೀಲಿ);
	    Py_XDECREF(todecref);
	    ಪ್ರತ್ಯಾಗಮ -1;
	}
	ಅನ್ಯಥಾ ಯದಿ (!(flags & SOPT_GLOBAL))
	{
	    PyErr_FORMAT(Py_Exc_ಮೌಲ್ಯ_Error,
		    N_("unable to unset option %s "
		       "which does not have global value"), ಕೀಲಿ);
	    Py_XDECREF(todecref);
	    ಪ್ರತ್ಯಾಗಮ -1;
	}
	ಅನ್ಯಥಾ
	{
	    unset_global_local_option(ಕೀಲಿ, self->from);
	    Py_XDECREF(todecref);
	    ಪ್ರತ್ಯಾಗಮ 0;
	}
    }

    opt_flags = (self->opt_type ? OPT_LOCAL : OPT_GLOBAL);

    ಯದಿ (flags & SOPT_BOOL)
    {
	ಪೂರ್ಣಾಙ್ಕ	istrue = PyObject_IsTrue(valObject);

	ಯದಿ (istrue == -1)
	    ret = -1;
	ಅನ್ಯಥಾ
	    ret = set_option_ಮೌಲ್ಯ_for(ಕೀಲಿ, istrue, ರಿಕ್ತ,
				    opt_flags, self->opt_type, self->from);
    }
    ಅನ್ಯಥಾ ಯದಿ (flags & SOPT_ಅಂಕೆ)
    {
	long	val;

	ಯದಿ (ಸಂಖ್ಯೆ_To_Long(valObject, &val, ಸಂಖ್ಯೆ_INT))
	{
	    Py_XDECREF(todecref);
	    ಪ್ರತ್ಯಾಗಮ -1;
	}

	ret = set_option_ಮೌಲ್ಯ_for(ಕೀಲಿ, (ಪೂರ್ಣಾಙ್ಕ) val, ರಿಕ್ತ, opt_flags,
				self->opt_type, self->from);
    }
    ಅನ್ಯಥಾ
    {
	char_u		*val;
	PyObject	*todecref2;

	ಯದಿ ((val = StringToChars(valObject, &todecref2)))
	{
	    ret = set_option_ಮೌಲ್ಯ_for(ಕೀಲಿ, 0, val, opt_flags,
				    self->opt_type, self->from);
	    Py_XDECREF(todecref2);
	}
	ಅನ್ಯಥಾ
	    ret = -1;
    }

    Py_XDECREF(todecref);

    ಪ್ರತ್ಯಾಗಮ ret;
}

ಸ್ಥಿರ PySequenceMethods OptionsAsSeq = {
    0,					// sq_length
    0,					// sq_concat
    0,					// sq_repeat
    0,					// sq_item
    0,					// sq_slice
    0,					// sq_ass_item
    0,					// sq_ass_slice
    (objobjproc) OptionsContains,	// sq_contains
    0,					// sq_inplace_concat
    0,					// sq_inplace_repeat
};

ಸ್ಥಿರ PyMappingMethods OptionsAsMapping = {
    (lenfunc)       ರಿಕ್ತ,
    (binaryfunc)    OptionsItem,
    (objobjargproc) OptionsAssItem,
};

// Tabpage object

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    PyObject_HEAD
    tabpage_T	*tab;
} TabPageObject;

ಸ್ಥಿರ PyObject *WinListNew(TabPageObject *tabObject);

DEFINE_PY_TYPE_OBJECT(TabPageType);

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
CheckTabPage(TabPageObject *self)
{
    ಯದಿ (self->tab == INVALID_TABPAGE_ಮೌಲ್ಯ)
    {
	PyErr_SET_VIM(N_("attempt to refer to deleted tab page"));
	ಪ್ರತ್ಯಾಗಮ -1;
    }

    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಸ್ಥಿರ PyObject *
TabPageNew(tabpage_T *tab)
{
    TabPageObject *self;

    ಯದಿ (TAB_PYTHON_REF(tab))
    {
	self = TAB_PYTHON_REF(tab);
	Py_INCREF((PyObject *)self);
    }
    ಅನ್ಯಥಾ
    {
	self = PyObject_NEW(TabPageObject, TabPageTypePtr);
	ಯದಿ (self == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	self->tab = tab;
	TAB_PYTHON_REF(tab) = self;
    }

    ಪ್ರತ್ಯಾಗಮ (PyObject *)(self);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
TabPageDestructor(PyObject *self_obj)
{
    TabPageObject *self = (TabPageObject*)self_obj;
    ಯದಿ (self->tab && self->tab != INVALID_TABPAGE_ಮೌಲ್ಯ)
	TAB_PYTHON_REF(self->tab) = ರಿಕ್ತ;

    DESTRUCTOR_FINISH(self);
}

ಸ್ಥಿರ ವರ್ಣ *TabPageAttrs[] = {
    "windows", "number", "vars", "window", "valid",
    ರಿಕ್ತ
};

    ಸ್ಥಿರ PyObject *
TabPageDir(PyObject *self, PyObject *args UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ ObjectDir(self, TabPageAttrs);
}

    ಸ್ಥಿರ PyObject *
TabPageAttrValid(TabPageObject *self, ವರ್ಣ *name)
{
    PyObject	*ret;

    ಯದಿ (strcmp(name, "valid") != 0)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ret = ((self->tab == INVALID_TABPAGE_ಮೌಲ್ಯ) ? Py_False : Py_True);
    Py_INCREF(ret);
    ಪ್ರತ್ಯಾಗಮ ret;
}

    ಸ್ಥಿರ PyObject *
TabPageAttr(TabPageObject *self, ವರ್ಣ *name)
{
    ಯದಿ (strcmp(name, "windows") == 0)
	ಪ್ರತ್ಯಾಗಮ WinListNew(self);
    ಅನ್ಯಥಾ ಯದಿ (strcmp(name, "number") == 0)
	ಪ್ರತ್ಯಾಗಮ PyLong_FromLong((long) get_tab_ಸಂಖ್ಯೆ(self->tab));
    ಅನ್ಯಥಾ ಯದಿ (strcmp(name, "vars") == 0)
	ಪ್ರತ್ಯಾಗಮ NEW_DICTIONARY(self->tab->tp_vars);
    ಅನ್ಯಥಾ ಯದಿ (strcmp(name, "window") == 0)
    {
	// For current tab window.c does not bother to set or update tp_curwin
	ಯದಿ (self->tab == curtab)
	    ಪ್ರತ್ಯಾಗಮ WindowNew(curwin, curtab);
	ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ WindowNew(self->tab->tp_curwin, self->tab);
    }
    ಅನ್ಯಥಾ ಯದಿ (strcmp(name, "__members__") == 0)
	ಪ್ರತ್ಯಾಗಮ ObjectDir(ರಿಕ್ತ, TabPageAttrs);
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಸ್ಥಿರ PyObject *
TabPageRepr(PyObject *self_obj)
{
    TabPageObject *self = (TabPageObject*)self_obj;
    ಯದಿ (self->tab == INVALID_TABPAGE_ಮೌಲ್ಯ)
	ಪ್ರತ್ಯಾಗಮ PyString_FromFormat("<tabpage object (deleted) at %p>", (ಶೂನ್ಯ *)self);
    ಅನ್ಯಥಾ
    {
	ಪೂರ್ಣಾಙ್ಕ	t = get_tab_ಸಂಖ್ಯೆ(self->tab);

	ಯದಿ (t == 0)
	    ಪ್ರತ್ಯಾಗಮ PyString_FromFormat("<tabpage object (unknown) at %p>",
					(ಶೂನ್ಯ *)self);
	ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ PyString_FromFormat("<tabpage %d>", t - 1);
    }
}

ಸ್ಥಿರ ಸಂರಚನಾ PyMethodDef TabPageMethods[] = {
    // name,	    function,			calling,	documentation
    {"__dir__",	    (PyCFunction)TabPageDir,	METH_NOARGS,	""},
    { ರಿಕ್ತ,	    ರಿಕ್ತ,			0,		ರಿಕ್ತ}
};

/*
 * Window list object
 */

DEFINE_PY_TYPE_OBJECT(TabListType);
ಸ್ಥಿರ PySequenceMethods TabListAsSeq;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    PyObject_HEAD
} TabListObject;

ಸ್ಥಿರ TabListObject TheTabPageList =
{
    PyObject_HEAD_INIT_TYPE(TabListType)
};

    ಸ್ಥಿರ PyInt
TabListLength(PyObject *self UNUSED)
{
    tabpage_T	*tp = first_tabpage;
    PyInt	n = 0;

    ಯಾವತ್ (tp != ರಿಕ್ತ)
    {
	++n;
	tp = tp->tp_next;
    }

    ಪ್ರತ್ಯಾಗಮ n;
}

    ಸ್ಥಿರ PyObject *
TabListItem(PyObject *self UNUSED, PyInt n)
{
    tabpage_T	*tp;

    ಪ್ರತಿ (tp = first_tabpage; tp != ರಿಕ್ತ; tp = tp->tp_next, --n)
	ಯದಿ (n == 0)
	    ಪ್ರತ್ಯಾಗಮ TabPageNew(tp);

    PyErr_SET_STRING(PyExc_IndexError, N_("no such tab page"));
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Window object
 */

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    PyObject_HEAD
    win_T	*win;
    TabPageObject	*tabObject;
} WindowObject;

DEFINE_PY_TYPE_OBJECT(WindowType);

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
CheckWindow(WindowObject *self)
{
    ಯದಿ (self->win == INVALID_WINDOW_ಮೌಲ್ಯ)
    {
	PyErr_SET_VIM(N_("attempt to refer to deleted window"));
	ಪ್ರತ್ಯಾಗಮ -1;
    }

    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
CheckWindowCb(ಶೂನ್ಯ *self)
{
    ಪ್ರತ್ಯಾಗಮ CheckWindow((WindowObject*)self);
}

    ಸ್ಥಿರ PyObject *
WindowNew(win_T *win, tabpage_T *tab)
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

    WindowObject *self;

    ಯದಿ (WIN_PYTHON_REF(win))
    {
	self = WIN_PYTHON_REF(win);
	Py_INCREF((PyObject *)self);
    }
    ಅನ್ಯಥಾ
    {
	self = PyObject_GC_New(WindowObject, WindowTypePtr);
	ಯದಿ (self == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	self->win = win;
	WIN_PYTHON_REF(win) = self;
    }

    self->tabObject = ((TabPageObject *)(TabPageNew(tab)));

    ಪ್ರತ್ಯಾಗಮ (PyObject *)(self);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
WindowDestructor(PyObject *self_obj)
{
    WindowObject *self = (WindowObject*)self_obj;
    PyObject_GC_UnTrack((ಶೂನ್ಯ *)(self));
    ಯದಿ (self->win && self->win != INVALID_WINDOW_ಮೌಲ್ಯ)
	WIN_PYTHON_REF(self->win) = ರಿಕ್ತ;
    Py_XDECREF(((PyObject *)(self->tabObject)));
    PyObject_GC_Del((ಶೂನ್ಯ *)(self));
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
WindowTraverse(PyObject *self_obj, visitproc visit, ಶೂನ್ಯ *arg)
{
    WindowObject *self = (WindowObject*)self_obj;
    Py_VISIT(((PyObject *)(self->tabObject)));
    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
WindowClear(PyObject *self_obj)
{
    WindowObject *self = (WindowObject*)self_obj;
    Py_CLEAR(self->tabObject);
    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಸ್ಥಿರ win_T *
get_firstwin(TabPageObject *tabObject)
{
    ಯದಿ (tabObject)
    {
	ಯದಿ (CheckTabPage(tabObject))
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	// For current tab window.c does not bother to set or update tp_firstwin
	ಅನ್ಯಥಾ ಯದಿ (tabObject->tab == curtab)
	    ಪ್ರತ್ಯಾಗಮ firstwin;
	ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ tabObject->tab->tp_firstwin;
    }
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ firstwin;
}

// Use the same order as in the WindowAttr() function.
ಸ್ಥಿರ ವರ್ಣ *WindowAttrs[] = {
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
    ರಿಕ್ತ
};

    ಸ್ಥಿರ PyObject *
WindowDir(PyObject *self, PyObject *args UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ ObjectDir(self, WindowAttrs);
}

    ಸ್ಥಿರ PyObject *
WindowAttrValid(WindowObject *self, ವರ್ಣ *name)
{
    PyObject	*ret;

    ಯದಿ (strcmp(name, "valid") != 0)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ret = ((self->win == INVALID_WINDOW_ಮೌಲ್ಯ) ? Py_False : Py_True);
    Py_INCREF(ret);
    ಪ್ರತ್ಯಾಗಮ ret;
}

    ಸ್ಥಿರ PyObject *
WindowAttr(WindowObject *self, ವರ್ಣ *name)
{
    ಯದಿ (strcmp(name, "buffer") == 0)
	ಪ್ರತ್ಯಾಗಮ (PyObject *)BufferNew(self->win->w_buffer);
    ಅನ್ಯಥಾ ಯದಿ (strcmp(name, "cursor") == 0)
    {
	pos_T *pos = &self->win->w_cursor;

	ಪ್ರತ್ಯಾಗಮ Py_Build_ಮೌಲ್ಯ("(ll)", (long)(pos->lnum), (long)(pos->col));
    }
    ಅನ್ಯಥಾ ಯದಿ (strcmp(name, "height") == 0)
	ಪ್ರತ್ಯಾಗಮ PyLong_FromLong((long)(self->win->w_height));
    ಅನ್ಯಥಾ ಯದಿ (strcmp(name, "row") == 0)
	ಪ್ರತ್ಯಾಗಮ PyLong_FromLong((long)(self->win->w_winrow));
    ಅನ್ಯಥಾ ಯದಿ (strcmp(name, "width") == 0)
	ಪ್ರತ್ಯಾಗಮ PyLong_FromLong((long)(self->win->w_width));
    ಅನ್ಯಥಾ ಯದಿ (strcmp(name, "col") == 0)
	ಪ್ರತ್ಯಾಗಮ PyLong_FromLong((long)(self->win->w_wincol));
    ಅನ್ಯಥಾ ಯದಿ (strcmp(name, "vars") == 0)
	ಪ್ರತ್ಯಾಗಮ NEW_DICTIONARY(self->win->w_vars);
    ಅನ್ಯಥಾ ಯದಿ (strcmp(name, "options") == 0)
	ಪ್ರತ್ಯಾಗಮ OptionsNew(SREQ_WIN, self->win, CheckWindowCb,
			(PyObject *) self);
    ಅನ್ಯಥಾ ಯದಿ (strcmp(name, "number") == 0)
    {
	ಯದಿ (CheckTabPage(self->tabObject))
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	ಪ್ರತ್ಯಾಗಮ PyLong_FromLong((long)
		get_win_ಸಂಖ್ಯೆ(self->win, get_firstwin(self->tabObject)));
    }
    ಅನ್ಯಥಾ ಯದಿ (strcmp(name, "tabpage") == 0)
    {
	Py_INCREF((PyObject *)self->tabObject);
	ಪ್ರತ್ಯಾಗಮ (PyObject *)(self->tabObject);
    }
    ಅನ್ಯಥಾ ಯದಿ (strcmp(name, "__members__") == 0)
	ಪ್ರತ್ಯಾಗಮ ObjectDir(ರಿಕ್ತ, WindowAttrs);
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
WindowSetattr(PyObject *self_obj, ವರ್ಣ *name, PyObject *valObject)
{
    WindowObject *self = (WindowObject*)self_obj;
    ಯದಿ (CheckWindow(self))
	ಪ್ರತ್ಯಾಗಮ -1;

    ಯದಿ (strcmp(name, "buffer") == 0)
    {
	PyErr_SET_STRING(PyExc_TypeError, N_("readonly attribute: buffer"));
	ಪ್ರತ್ಯಾಗಮ -1;
    }
    ಅನ್ಯಥಾ ಯದಿ (strcmp(name, "cursor") == 0)
    {
	long lnum;
	long col;

	ಯದಿ (!PyArg_Parse(valObject, "(ll)", &lnum, &col))
	    ಪ್ರತ್ಯಾಗಮ -1;

	ಯದಿ (lnum <= 0 || lnum > self->win->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	{
	    PyErr_SET_VIM(N_("cursor position outside buffer"));
	    ಪ್ರತ್ಯಾಗಮ -1;
	}

	// Check for keyboard interrupts
	ಯದಿ (VimCheckInterrupt())
	    ಪ್ರತ್ಯಾಗಮ -1;

	self->win->w_cursor.lnum = lnum;
	self->win->w_cursor.col = col;
	self->win->w_set_curswant = TRUE;
	self->win->w_cursor.coladd = 0;
	// When column is out of range silently correct it.
	check_cursor_col_win(self->win);

	update_screen(UPD_VALID);
	ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಅನ್ಯಥಾ ಯದಿ (strcmp(name, "height") == 0)
    {
	long	height;
	win_T	*savewin;

	ಯದಿ (ಸಂಖ್ಯೆ_To_Long(valObject, &height, ಸಂಖ್ಯೆ_INT|ಸಂಖ್ಯೆ_UNSIGNED))
	    ಪ್ರತ್ಯಾಗಮ -1;

#ifdef FEAT_GUI
	need_mouse_correct = TRUE;
#endif
	savewin = curwin;
	curwin = self->win;
	curbuf = curwin->w_buffer;

	VimTryStart();
	win_setheight((ಪೂರ್ಣಾಙ್ಕ) height);
	curwin = savewin;
	curbuf = curwin->w_buffer;
	ಯದಿ (VimTryEnd())
	    ಪ್ರತ್ಯಾಗಮ -1;

	ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಅನ್ಯಥಾ ಯದಿ (strcmp(name, "width") == 0)
    {
	long	width;
	win_T	*savewin;

	ಯದಿ (ಸಂಖ್ಯೆ_To_Long(valObject, &width, ಸಂಖ್ಯೆ_INT|ಸಂಖ್ಯೆ_UNSIGNED))
	    ಪ್ರತ್ಯಾಗಮ -1;

#ifdef FEAT_GUI
	need_mouse_correct = TRUE;
#endif
	savewin = curwin;
	curwin = self->win;
	curbuf = curwin->w_buffer;

	VimTryStart();
	win_setwidth((ಪೂರ್ಣಾಙ್ಕ) width);
	curwin = savewin;
	curbuf = curwin->w_buffer;
	ಯದಿ (VimTryEnd())
	    ಪ್ರತ್ಯಾಗಮ -1;

	ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಅನ್ಯಥಾ
    {
	PyErr_SetString(PyExc_AttributeError, name);
	ಪ್ರತ್ಯಾಗಮ -1;
    }
}

    ಸ್ಥಿರ PyObject *
WindowRepr(PyObject *self_obj)
{
    WindowObject *self = (WindowObject*)self_obj;
    ಯದಿ (self->win == INVALID_WINDOW_ಮೌಲ್ಯ)
	ಪ್ರತ್ಯಾಗಮ PyString_FromFormat("<window object (deleted) at %p>", (ಶೂನ್ಯ *)self);
    ಅನ್ಯಥಾ
    {
	ಪೂರ್ಣಾಙ್ಕ	w = get_win_ಸಂಖ್ಯೆ(self->win, firstwin);

	ಯದಿ (w == 0)
	    ಪ್ರತ್ಯಾಗಮ PyString_FromFormat("<window object (unknown) at %p>",
								      (ಶೂನ್ಯ *)self);
	ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ PyString_FromFormat("<window %d>", w - 1);
    }
}

ಸ್ಥಿರ ಸಂರಚನಾ PyMethodDef WindowMethods[] = {
    // name,	    function,			calling,	documentation
    {"__dir__",	    (PyCFunction)WindowDir,	METH_NOARGS,	""},
    { ರಿಕ್ತ,	    ರಿಕ್ತ,			0,		ರಿಕ್ತ}
};

/*
 * Window list object
 */

DEFINE_PY_TYPE_OBJECT(WinListType);
ಸ್ಥಿರ PySequenceMethods WinListAsSeq;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    PyObject_HEAD
    TabPageObject	*tabObject;
} WinListObject;

ಸ್ಥಿರ WinListObject TheWindowList =
{
    PyObject_HEAD_INIT_TYPE(WinListType)
    ರಿಕ್ತ
};

    ಸ್ಥಿರ PyObject *
WinListNew(TabPageObject *tabObject)
{
    WinListObject	*self;

    self = PyObject_NEW(WinListObject, WinListTypePtr);
    self->tabObject = tabObject;
    Py_INCREF((PyObject *)tabObject);

    ಪ್ರತ್ಯಾಗಮ (PyObject *)(self);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
WinListDestructor(WinListObject *self)
{
    TabPageObject	*tabObject = self->tabObject;

    ಯದಿ (tabObject)
    {
	Py_DECREF((PyObject *)(tabObject));
    }

    DESTRUCTOR_FINISH(self);
}

    ಸ್ಥಿರ PyInt
WinListLength(WinListObject *self)
{
    win_T	*w;
    PyInt	n = 0;

    ಯದಿ (!(w = get_firstwin(self->tabObject)))
	ಪ್ರತ್ಯಾಗಮ -1;

    ಯಾವತ್ (w != ರಿಕ್ತ)
    {
	++n;
	w = W_NEXT(w);
    }

    ಪ್ರತ್ಯಾಗಮ n;
}

    ಸ್ಥಿರ PyObject *
WinListItem(WinListObject *self, PyInt n)
{
    win_T *w;

    ಯದಿ (!(w = get_firstwin(self->tabObject)))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಪ್ರತಿ (; w != ರಿಕ್ತ; w = W_NEXT(w), --n)
	ಯದಿ (n == 0)
	    ಪ್ರತ್ಯಾಗಮ WindowNew(w, self->tabObject? self->tabObject->tab: curtab);

    PyErr_SET_STRING(PyExc_IndexError, N_("no such window"));
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
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
    ಸ್ಥಿರ ವರ್ಣ *
String_To_ಸಾಲು(PyObject *obj)
{
    ವರ್ಣ	*str;
    ವರ್ಣ	*save;
    PyObject	*bytes = ರಿಕ್ತ;
    Py_ssize_t	len = 0;
    PyInt	i;
    ವರ್ಣ	*p;

    ಯದಿ (PyBytes_Check(obj))
    {
	ಯದಿ (PyBytes_AsStringAndSize(obj, &str, &len) == -1
		|| str == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಅನ್ಯಥಾ ಯದಿ (PyUnicode_Check(obj))
    {
	ಯದಿ (!(bytes = PyUnicode_AsEncodedString(obj, ENC_OPT,
							   ERRORS_ENCODE_ARG)))
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	ಯದಿ (PyBytes_AsStringAndSize(bytes, &str, &len) == -1
		|| str == ರಿಕ್ತ)
	{
	    Py_DECREF(bytes);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
    }
    ಅನ್ಯಥಾ
    {
#ಯದಿ PY_MAJOR_VERSION < 3
	PyErr_FORMAT_TYPE(
		N_("expected str() or unicode() instance, but got %s"),
		obj);
#ಅನ್ಯಥಾ
	PyErr_FORMAT_TYPE(
		N_("expected bytes() or str() instance, but got %s"),
		obj);
#endif
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    /*
     * Error checking: String must not contain newlines, as we
     * are replacing a single line, and we must replace it with
     * a single line.
     * A trailing newline is removed, so that append(f.readlines()) works.
     */
    p = memchr(str, '\n', len);
    ಯದಿ (p != ರಿಕ್ತ)
    {
	ಯದಿ (p == str + len - 1)
	    --len;
	ಅನ್ಯಥಾ
	{
	    PyErr_SET_VIM(N_("string cannot contain newlines"));
	    Py_XDECREF(bytes);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
    }

    /*
     * Create a copy of the string, with internal nulls replaced by
     * newline characters, as is the Vim convention.
     */
    save = alloc(len+1);
    ಯದಿ (save == ರಿಕ್ತ)
    {
	PyErr_NoMemory();
	Py_XDECREF(bytes);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಪ್ರತಿ (i = 0; i < len; ++i)
    {
	ಯದಿ (str[i] == '\0')
	    save[i] = '\n';
	ಅನ್ಯಥಾ
	    save[i] = str[i];
    }

    save[i] = '\0';
    Py_XDECREF(bytes);  // Python 2 does nothing here

    ಪ್ರತ್ಯಾಗಮ save;
}

/*
 * Get a line from the specified buffer. The line number is
 * in Vim format (1-based). The line is returned as a Python
 * string object.
 */
    ಸ್ಥಿರ PyObject *
Get_Buffer_ಸಾಲು(buf_T *buf, PyInt n)
{
    ಪ್ರತ್ಯಾಗಮ ಸಾಲು_To_String((ವರ್ಣ *)ml_get_buf(buf, (linenr_T)n, FALSE));
}


/*
 * Get a list of lines from the specified buffer. The line numbers
 * are in Vim format (1-based). The range is from lo up to, but not
 * including, hi. The list is returned as a Python list of string objects.
 */
    ಸ್ಥಿರ PyObject *
Get_Buffer_ಸಾಲು_List(buf_T *buf, PyInt lo, PyInt hi)
{
    PyInt	i;
    PyInt	n = hi - lo;
    PyObject	*list = PyList_New(n);

    ಯದಿ (list == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಪ್ರತಿ (i = 0; i < n; ++i)
    {
	linenr_T	lnum = (linenr_T)(lo + i);
	ವರ್ಣ		*text;
	PyObject	*string;

	ಯದಿ (lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    text = "";
	ಅನ್ಯಥಾ
	    text = (ವರ್ಣ *)ml_get_buf(buf, lnum, FALSE);
	string = ಸಾಲು_To_String(text);
	ಯದಿ (string == ರಿಕ್ತ)
	{
	    Py_DECREF(list);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}

	PyList_SET_ITEM(list, i, string);
    }

    // The ownership of the Python list is passed to the caller (ie,
    // the caller should Py_DECREF() the object when it is finished
    // with it).

    ಪ್ರತ್ಯಾಗಮ list;
}

/*
 * Check if deleting lines made the cursor position invalid.
 * Changed the lines from "lo" to "hi" and added "extra" lines (negative if
 * deleted).
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
py_fix_cursor(linenr_T lo, linenr_T hi, linenr_T extra)
{
    ಯದಿ (curwin->w_cursor.lnum >= lo)
    {
	// Adjust the cursor position if it's in/after the changed
	// lines.
	ಯದಿ (curwin->w_cursor.lnum >= hi)
	{
	    curwin->w_cursor.lnum += extra;
	    check_cursor_col();
	}
	ಅನ್ಯಥಾ ಯದಿ (extra < 0)
	{
	    curwin->w_cursor.lnum = lo;
	    check_cursor();
	}
	ಅನ್ಯಥಾ
	    check_cursor_col();
	changed_cline_bef_curs();
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
Set_Buffer_ಸಾಲು(buf_T *buf, PyInt n, PyObject *ಸಾಲು, PyInt *len_change)
{
    bufref_T	save_curbuf = {ರಿಕ್ತ, 0, 0};
    switchwin_T	switchwin;

    // First of all, we check the type of the supplied Python object.
    // There are three cases:
    //	  1. NULL, or None - this is a deletion.
    //	  2. A string	   - this is a replacement.
    //	  3. Anything else - this is an error.
    ಯದಿ (ಸಾಲು == Py_None || ಸಾಲು == ರಿಕ್ತ)
    {
	PyErr_Clear();
	switchwin.sw_curwin = ರಿಕ್ತ;
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	VimTryStart();

	ಯದಿ (u_savedel((linenr_T)n, 1L) == FAIL)
	    RAISE_UNDO_FAIL;
	ಅನ್ಯಥಾ ಯದಿ (ml_delete((linenr_T)n) == FAIL)
	    RAISE_DELETE_ಸಾಲು_FAIL;
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (buf == curbuf && (switchwin.sw_curwin != ರಿಕ್ತ
					   || save_curbuf.br_buf == ರಿಕ್ತ))
		// Using an existing window for the buffer, adjust the cursor
		// position.
		py_fix_cursor((linenr_T)n, (linenr_T)n + 1, (linenr_T)-1);
	    ಯದಿ (save_curbuf.br_buf == ರಿಕ್ತ)
		// Only adjust marks if we managed to switch to a window that
		// holds the buffer, otherwise line numbers will be invalid.
		deleted_ಸಾಲುಗಳು_mark((linenr_T)n, 1L);
	}

	restore_win_for_buf(&switchwin, &save_curbuf);

	ಯದಿ (VimTryEnd())
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	ಯದಿ (len_change)
	    *len_change = -1;

	ಪ್ರತ್ಯಾಗಮ OK;
    }
    ಅನ್ಯಥಾ ಯದಿ (PyBytes_Check(ಸಾಲು) || PyUnicode_Check(ಸಾಲು))
    {
	ವರ್ಣ	*save = String_To_ಸಾಲು(ಸಾಲು);

	ಯದಿ (save == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	VimTryStart();

	// We do not need to free "save" if ml_replace() consumes it.
	PyErr_Clear();
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	ಯದಿ (u_savesub((linenr_T)n) == FAIL)
	{
	    RAISE_UNDO_FAIL;
	    vim_free(save);
	}
	ಅನ್ಯಥಾ ಯದಿ (ml_replace((linenr_T)n, (char_u *)save, FALSE) == FAIL)
	{
	    RAISE_REPLACE_ಸಾಲು_FAIL;
	    vim_free(save);
	}
	ಅನ್ಯಥಾ
	    changed_bytes((linenr_T)n, 0);

	restore_win_for_buf(&switchwin, &save_curbuf);

	// Check that the cursor is not beyond the end of the line now.
	ಯದಿ (buf == curbuf)
	    check_cursor_col();

	ಯದಿ (VimTryEnd())
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	ಯದಿ (len_change)
	    *len_change = 0;

	ಪ್ರತ್ಯಾಗಮ OK;
    }
    ಅನ್ಯಥಾ
    {
	PyErr_BadArgument();
	ಪ್ರತ್ಯಾಗಮ FAIL;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
Set_Buffer_ಸಾಲು_List(
	buf_T *buf,
	PyInt lo,
	PyInt hi,
	PyObject *list,
	PyInt *len_change)
{
    bufref_T	save_curbuf = {ರಿಕ್ತ, 0, 0};
    switchwin_T	switchwin;

    // First of all, we check the type of the supplied Python object.
    // There are three cases:
    //	  1. NULL, or None - this is a deletion.
    //	  2. A list	   - this is a replacement.
    //	  3. Anything else - this is an error.
    ಯದಿ (list == Py_None || list == ರಿಕ್ತ)
    {
	PyInt	i;
	PyInt	n = (ಪೂರ್ಣಾಙ್ಕ)(hi - lo);

	PyErr_Clear();
	VimTryStart();
	switchwin.sw_curwin = ರಿಕ್ತ;
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	ಯದಿ (u_savedel((linenr_T)lo, (long)n) == FAIL)
	    RAISE_UNDO_FAIL;
	ಅನ್ಯಥಾ
	{
	    ಪ್ರತಿ (i = 0; i < n; ++i)
	    {
		ಯದಿ (ml_delete((linenr_T)lo) == FAIL)
		{
		    RAISE_DELETE_ಸಾಲು_FAIL;
		    ಭಙ್ಗ;
		}
	    }
	    ಯದಿ (buf == curbuf && (switchwin.sw_curwin != ರಿಕ್ತ
					       || save_curbuf.br_buf == ರಿಕ್ತ))
		// Using an existing window for the buffer, adjust the cursor
		// position.
		py_fix_cursor((linenr_T)lo, (linenr_T)hi, (linenr_T)-n);
	    ಯದಿ (save_curbuf.br_buf == ರಿಕ್ತ)
		// Only adjust marks if we managed to switch to a window that
		// holds the buffer, otherwise line numbers will be invalid.
		deleted_ಸಾಲುಗಳು_mark((linenr_T)lo, (long)i);
	}

	restore_win_for_buf(&switchwin, &save_curbuf);

	ಯದಿ (VimTryEnd())
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	ಯದಿ (len_change)
	    *len_change = -n;

	ಪ್ರತ್ಯಾಗಮ OK;
    }
    ಅನ್ಯಥಾ ಯದಿ (PyList_Check(list))
    {
	PyInt	i;
	PyInt	new_len = PyList_Size(list);
	PyInt	old_len = hi - lo;
	PyInt	extra = 0;	// lines added to text, can be negative
	ವರ್ಣ	**array;

	ಯದಿ (new_len == 0)	// avoid allocating zero bytes
	    array = ರಿಕ್ತ;
	ಅನ್ಯಥಾ
	{
	    array = PyMem_New(ವರ್ಣ *, new_len);
	    ಯದಿ (array == ರಿಕ್ತ)
	    {
		PyErr_NoMemory();
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	}

	ಪ್ರತಿ (i = 0; i < new_len; ++i)
	{
	    PyObject *ಸಾಲು;

	    ಯದಿ (!(ಸಾಲು = PyList_GetItem(list, i)) ||
		!(array[i] = String_To_ಸಾಲು(ಸಾಲು)))
	    {
		ಯಾವತ್ (i)
		    vim_free(array[--i]);
		PyMem_Free(array);
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	}

	VimTryStart();
	PyErr_Clear();

	// START of region without "return".  Must call restore_buffer()!
	switchwin.sw_curwin = ರಿಕ್ತ;
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	ಯದಿ (u_save((linenr_T)(lo-1), (linenr_T)hi) == FAIL)
	    RAISE_UNDO_FAIL;

	// If the size of the range is reducing (ie, new_len < old_len) we
	// need to delete some old_len. We do this at the start, by
	// repeatedly deleting line "lo".
	ಯದಿ (!PyErr_Occurred())
	{
	    ಪ್ರತಿ (i = 0; i < old_len - new_len; ++i)
		ಯದಿ (ml_delete((linenr_T)lo) == FAIL)
		{
		    RAISE_DELETE_ಸಾಲು_FAIL;
		    ಭಙ್ಗ;
		}
	    extra -= i;
	}

	// For as long as possible, replace the existing old_len with the
	// new old_len. This is a more efficient operation, as it requires
	// less memory allocation and freeing.
	ಯದಿ (!PyErr_Occurred())
	{
	    ಪ್ರತಿ (i = 0; i < old_len && i < new_len; ++i)
		ಯದಿ (ml_replace((linenr_T)(lo+i), (char_u *)array[i], FALSE)
								      == FAIL)
		{
		    RAISE_REPLACE_ಸಾಲು_FAIL;
		    ಭಙ್ಗ;
		}
	}
	ಅನ್ಯಥಾ
	    i = 0;

	// Now we may need to insert the remaining new old_len. If we do, we
	// must free the strings as we finish with them (we can't pass the
	// responsibility to Vim in this case).
	ಯದಿ (!PyErr_Occurred())
	{
	    ಯಾವತ್ (i < new_len)
	    {
		ಯದಿ (ml_append((linenr_T)(lo + i - 1),
					(char_u *)array[i], 0, FALSE) == FAIL)
		{
		    RAISE_ಸೇರಿಸು_ಸಾಲು_FAIL;
		    ಭಙ್ಗ;
		}
		vim_free(array[i]);
		++i;
		++extra;
	    }
	}

	// Free any left-over old_len, as a result of an error
	ಯಾವತ್ (i < new_len)
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
	ಯದಿ (save_curbuf.br_buf == ರಿಕ್ತ)
	{
	    mark_adjust((linenr_T)lo, (linenr_T)(hi - 1),
						  (long)MAXLNUM, (long)extra);
	    changed_ಸಾಲುಗಳು((linenr_T)lo, 0, (linenr_T)hi, (long)extra);
	}

	ಯದಿ (buf == curbuf && (switchwin.sw_curwin != ರಿಕ್ತ
					   || save_curbuf.br_buf == ರಿಕ್ತ))
	    // Using an existing window for the buffer, adjust the cursor
	    // position.
	    py_fix_cursor((linenr_T)lo, (linenr_T)hi, (linenr_T)extra);

	// END of region without "return".
	restore_win_for_buf(&switchwin, &save_curbuf);

	ಯದಿ (VimTryEnd())
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	ಯದಿ (len_change)
	    *len_change = new_len - old_len;

	ಪ್ರತ್ಯಾಗಮ OK;
    }
    ಅನ್ಯಥಾ
    {
	PyErr_BadArgument();
	ಪ್ರತ್ಯಾಗಮ FAIL;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ಸೇರಿಸು_Buffer_ಸಾಲುಗಳು(buf_T *buf, PyInt n, PyObject *ಸಾಲುಗಳು, PyInt *len_change)
{
    bufref_T	save_curbuf = {ರಿಕ್ತ, 0, 0};
    switchwin_T	switchwin;

    // First of all, we check the type of the supplied Python object.
    // It must be a string or a list, or the call is in error.
    ಯದಿ (PyBytes_Check(ಸಾಲುಗಳು) || PyUnicode_Check(ಸಾಲುಗಳು))
    {
	ವರ್ಣ		*str = String_To_ಸಾಲು(ಸಾಲುಗಳು);

	ಯದಿ (str == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	PyErr_Clear();
	VimTryStart();
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	ಯದಿ (u_save((linenr_T)n, (linenr_T)(n + 1)) == FAIL)
	    RAISE_UNDO_FAIL;
	ಅನ್ಯಥಾ ಯದಿ (ml_append((linenr_T)n, (char_u *)str, 0, FALSE) == FAIL)
	    RAISE_ಸೇರಿಸು_ಸಾಲು_FAIL;
	ಅನ್ಯಥಾ ಯದಿ (save_curbuf.br_buf == ರಿಕ್ತ)
	    // Only adjust marks if we managed to switch to a window that
	    // holds the buffer, otherwise line numbers will be invalid.
	    appended_ಸಾಲುಗಳು_mark((linenr_T)n, 1L);

	vim_free(str);
	restore_win_for_buf(&switchwin, &save_curbuf);
	update_screen(UPD_VALID);

	ಯದಿ (VimTryEnd())
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	ಯದಿ (len_change)
	    *len_change = 1;

	ಪ್ರತ್ಯಾಗಮ OK;
    }
    ಅನ್ಯಥಾ ಯದಿ (PyList_Check(ಸಾಲುಗಳು))
    {
	PyInt	i;
	PyInt	size = PyList_Size(ಸಾಲುಗಳು);
	ವರ್ಣ	**array;

	array = PyMem_New(ವರ್ಣ *, size);
	ಯದಿ (array == ರಿಕ್ತ)
	{
	    PyErr_NoMemory();
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}

	ಪ್ರತಿ (i = 0; i < size; ++i)
	{
	    PyObject *ಸಾಲು;

	    ಯದಿ (!(ಸಾಲು = PyList_GetItem(ಸಾಲುಗಳು, i)) ||
		!(array[i] = String_To_ಸಾಲು(ಸಾಲು)))
	    {
		ಯಾವತ್ (i)
		    vim_free(array[--i]);
		PyMem_Free(array);
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	}

	PyErr_Clear();
	VimTryStart();
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	ಯದಿ (u_save((linenr_T)n, (linenr_T)(n + 1)) == FAIL)
	    RAISE_UNDO_FAIL;
	ಅನ್ಯಥಾ
	{
	    ಪ್ರತಿ (i = 0; i < size; ++i)
	    {
		ಯದಿ (ml_append((linenr_T)(n + i),
					(char_u *)array[i], 0, FALSE) == FAIL)
		{
		    RAISE_ಸೇರಿಸು_ಸಾಲು_FAIL;

		    // Free the rest of the lines
		    ಯಾವತ್ (i < size)
			vim_free(array[i++]);

		    ಭಙ್ಗ;
		}
		vim_free(array[i]);
	    }
	    ಯದಿ (i > 0 && save_curbuf.br_buf == ರಿಕ್ತ)
		// Only adjust marks if we managed to switch to a window that
		// holds the buffer, otherwise line numbers will be invalid.
		appended_ಸಾಲುಗಳು_mark((linenr_T)n, (long)i);
	}

	// Free the array of lines. All of its contents have now
	// been freed.
	PyMem_Free(array);
	restore_win_for_buf(&switchwin, &save_curbuf);

	update_screen(UPD_VALID);

	ಯದಿ (VimTryEnd())
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	ಯದಿ (len_change)
	    *len_change = size;

	ಪ್ರತ್ಯಾಗಮ OK;
    }
    ಅನ್ಯಥಾ
    {
	PyErr_BadArgument();
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
}

/*
 * Common routines for buffers and line ranges
 * -------------------------------------------
 */

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    PyObject_HEAD
    buf_T *buf;
} BufferObject;

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
CheckBuffer(BufferObject *self)
{
    ಯದಿ (self->buf == INVALID_BUFFER_ಮೌಲ್ಯ)
    {
	PyErr_SET_VIM(N_("attempt to refer to deleted buffer"));
	ಪ್ರತ್ಯಾಗಮ -1;
    }

    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
CheckBufferCb(ಶೂನ್ಯ *self)
{
    ಪ್ರತ್ಯಾಗಮ CheckBuffer((BufferObject*)self);
}

    ಸ್ಥಿರ PyObject *
RBItem(BufferObject *self, PyInt n, PyInt start, PyInt end)
{
    ಯದಿ (CheckBuffer(self))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (end == -1)
	end = self->buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

    ಯದಿ (n < 0)
	n += end - start + 1;

    ಯದಿ (n < 0 || n > end - start)
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("line number out of range"));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಪ್ರತ್ಯಾಗಮ Get_Buffer_ಸಾಲು(self->buf, n+start);
}

    ಸ್ಥಿರ PyObject *
RBSlice(BufferObject *self, PyInt lo, PyInt hi, PyInt start, PyInt end)
{
    PyInt size;

    ಯದಿ (CheckBuffer(self))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (end == -1)
	end = self->buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

    size = end - start + 1;

    ಯದಿ (lo < 0)
	lo = 0;
    ಅನ್ಯಥಾ ಯದಿ (lo > size)
	lo = size;
    ಯದಿ (hi < 0)
	hi = 0;
    ಯದಿ (hi < lo)
	hi = lo;
    ಅನ್ಯಥಾ ಯದಿ (hi > size)
	hi = size;

    ಪ್ರತ್ಯಾಗಮ Get_Buffer_ಸಾಲು_List(self->buf, lo+start, hi+start);
}

    ಸ್ಥಿರ PyInt
RBAsItem(
	BufferObject *self,
	PyInt n,
	PyObject *valObject,
	PyInt start,
	PyInt end,
	PyInt *new_end)
{
    PyInt len_change;

    ಯದಿ (CheckBuffer(self))
	ಪ್ರತ್ಯಾಗಮ -1;

    ಯದಿ (end == -1)
	end = self->buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

    ಯದಿ (n < 0)
	n += end - start + 1;

    ಯದಿ (n < 0 || n > end - start)
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("line number out of range"));
	ಪ್ರತ್ಯಾಗಮ -1;
    }

    ಯದಿ (Set_Buffer_ಸಾಲು(self->buf, n+start, valObject, &len_change) == FAIL)
	ಪ್ರತ್ಯಾಗಮ -1;

    ಯದಿ (new_end)
	*new_end = end + len_change;

    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಸ್ಥಿರ PyInt
RBAsSlice(
	BufferObject *self,
	PyInt lo,
	PyInt hi,
	PyObject *valObject,
	PyInt start,
	PyInt end,
	PyInt *new_end)
{
    PyInt size;
    PyInt len_change;

    // Self must be a valid buffer
    ಯದಿ (CheckBuffer(self))
	ಪ್ರತ್ಯಾಗಮ -1;

    ಯದಿ (end == -1)
	end = self->buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

    // Sort out the slice range
    size = end - start + 1;

    ಯದಿ (lo < 0)
	lo = 0;
    ಅನ್ಯಥಾ ಯದಿ (lo > size)
	lo = size;
    ಯದಿ (hi < 0)
	hi = 0;
    ಯದಿ (hi < lo)
	hi = lo;
    ಅನ್ಯಥಾ ಯದಿ (hi > size)
	hi = size;

    ಯದಿ (Set_Buffer_ಸಾಲು_List(self->buf, lo + start, hi + start,
						valObject, &len_change) == FAIL)
	ಪ್ರತ್ಯಾಗಮ -1;

    ಯದಿ (new_end)
	*new_end = end + len_change;

    ಪ್ರತ್ಯಾಗಮ 0;
}


    ಸ್ಥಿರ PyObject *
RBAppend(
	BufferObject *self,
	PyObject *args,
	PyInt start,
	PyInt end,
	PyInt *new_end)
{
    PyObject *ಸಾಲುಗಳು;
    PyInt len_change;
    PyInt max;
    PyInt n;

    ಯದಿ (CheckBuffer(self))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (end == -1)
	end = self->buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

    max = n = end - start + 1;

    ಯದಿ (!PyArg_ParseTuple(args, "O|n", &ಸಾಲುಗಳು, &n))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (n < 0 || n > max)
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("line number out of range"));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಯದಿ (ಸೇರಿಸು_Buffer_ಸಾಲುಗಳು(self->buf, n + start - 1, ಸಾಲುಗಳು, &len_change) == FAIL)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (new_end)
	*new_end = end + len_change;

    Py_INCREF(Py_None);
    ಪ್ರತ್ಯಾಗಮ Py_None;
}

// Range object

DEFINE_PY_TYPE_OBJECT(RangeType);
ಸ್ಥಿರ PySequenceMethods RangeAsSeq;
ಸ್ಥಿರ PyMappingMethods RangeAsMapping;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    PyObject_HEAD
    BufferObject *buf;
    PyInt start;
    PyInt end;
} RangeObject;

    ಸ್ಥಿರ PyObject *
RangeNew(buf_T *buf, PyInt start, PyInt end)
{
    BufferObject *bufr;
    RangeObject *self;
    self = PyObject_GC_New(RangeObject, RangeTypePtr);
    ಯದಿ (self == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    bufr = (BufferObject *)BufferNew(buf);
    ಯದಿ (bufr == ರಿಕ್ತ)
    {
	Py_DECREF(self);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    Py_INCREF((PyObject *)bufr);

    self->buf = bufr;
    self->start = start;
    self->end = end;

    ಪ್ರತ್ಯಾಗಮ (PyObject *)(self);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
RangeDestructor(PyObject *self_obj)
{
    RangeObject *self = (RangeObject*)self_obj;
    PyObject_GC_UnTrack((ಶೂನ್ಯ *)(self));
    Py_XDECREF(((PyObject *)(self->buf)));
    PyObject_GC_Del((ಶೂನ್ಯ *)(self));
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
RangeTraverse(PyObject *self_obj, visitproc visit, ಶೂನ್ಯ *arg)
{
    RangeObject *self = (RangeObject*)self_obj;
    Py_VISIT(((PyObject *)(self->buf)));
    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
RangeClear(PyObject *self_obj)
{
    RangeObject *self = (RangeObject*)self_obj;
    Py_CLEAR(self->buf);
    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಸ್ಥಿರ PyInt
RangeLength(RangeObject *self)
{
    // HOW DO WE SIGNAL AN ERROR FROM THIS FUNCTION?
    ಯದಿ (CheckBuffer(self->buf))
	ಪ್ರತ್ಯಾಗಮ -1; // ???

    ಪ್ರತ್ಯಾಗಮ (self->end - self->start + 1);
}

    ಸ್ಥಿರ PyObject *
RangeItem(RangeObject *self, PyInt n)
{
    ಪ್ರತ್ಯಾಗಮ RBItem(self->buf, n, self->start, self->end);
}

    ಸ್ಥಿರ PyObject *
RangeSlice(RangeObject *self, PyInt lo, PyInt hi)
{
    ಪ್ರತ್ಯಾಗಮ RBSlice(self->buf, lo, hi, self->start, self->end);
}

ಸ್ಥಿರ ವರ್ಣ *RangeAttrs[] = {
    "start", "end",
    ರಿಕ್ತ
};

    ಸ್ಥಿರ PyObject *
RangeDir(PyObject *self, PyObject *args UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ ObjectDir(self, RangeAttrs);
}

    ಸ್ಥಿರ PyObject *
RangeAppend(RangeObject *self, PyObject *args)
{
    ಪ್ರತ್ಯಾಗಮ RBAppend(self->buf, args, self->start, self->end, &self->end);
}

    ಸ್ಥಿರ PyObject *
RangeRepr(PyObject *self_obj)
{
    RangeObject *self = (RangeObject*)self_obj;
    ಯದಿ (self->buf->buf == INVALID_BUFFER_ಮೌಲ್ಯ)
	ಪ್ರತ್ಯಾಗಮ PyString_FromFormat("<range object (for deleted buffer) at %p>",
				    (ಶೂನ್ಯ *)self);
    ಅನ್ಯಥಾ
    {
	ವರ್ಣ *name = (ವರ್ಣ *)self->buf->buf->b_fname;

	ಯದಿ (name == ರಿಕ್ತ)
	    name = "";

	ಪ್ರತ್ಯಾಗಮ PyString_FromFormat("<range %s (%d:%d)>",
				    name, (ಪೂರ್ಣಾಙ್ಕ)self->start, (ಪೂರ್ಣಾಙ್ಕ)self->end);
    }
}

ಸ್ಥಿರ ಸಂರಚನಾ PyMethodDef RangeMethods[] = {
    // name,	function,			calling,	documentation
    {"append",	(PyCFunction)RangeAppend,	METH_VARARGS,	"Append data to the Vim range" },
    {"__dir__",	(PyCFunction)RangeDir,		METH_NOARGS,	""},
    { ರಿಕ್ತ,	ರಿಕ್ತ,				0,		ರಿಕ್ತ}
};

DEFINE_PY_TYPE_OBJECT(BufferType);
ಸ್ಥಿರ PySequenceMethods BufferAsSeq;
ಸ್ಥಿರ PyMappingMethods BufferAsMapping;

    ಸ್ಥಿರ PyObject *
BufferNew(buf_T *buf)
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

    BufferObject *self;

    ಯದಿ (BUF_PYTHON_REF(buf) != ರಿಕ್ತ)
    {
	self = BUF_PYTHON_REF(buf);
	Py_INCREF((PyObject *)self);
    }
    ಅನ್ಯಥಾ
    {
	self = PyObject_NEW(BufferObject, BufferTypePtr);
	ಯದಿ (self == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	self->buf = buf;
	BUF_PYTHON_REF(buf) = self;
    }

    ಪ್ರತ್ಯಾಗಮ (PyObject *)(self);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
BufferDestructor(PyObject *self_obj)
{
    BufferObject *self = (BufferObject*)self_obj;
    ಯದಿ (self->buf && self->buf != INVALID_BUFFER_ಮೌಲ್ಯ)
	BUF_PYTHON_REF(self->buf) = ರಿಕ್ತ;

    DESTRUCTOR_FINISH(self);
}

    ಸ್ಥಿರ PyInt
BufferLength(BufferObject *self)
{
    // HOW DO WE SIGNAL AN ERROR FROM THIS FUNCTION?
    ಯದಿ (CheckBuffer(self))
	ಪ್ರತ್ಯಾಗಮ -1; // ???

    ಪ್ರತ್ಯಾಗಮ (PyInt)(self->buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ);
}

    ಸ್ಥಿರ PyObject *
BufferItem(BufferObject *self, PyInt n)
{
    ಪ್ರತ್ಯಾಗಮ RBItem(self, n, 1, -1);
}

    ಸ್ಥಿರ PyObject *
BufferSlice(BufferObject *self, PyInt lo, PyInt hi)
{
    ಪ್ರತ್ಯಾಗಮ RBSlice(self, lo, hi, 1, -1);
}

ಸ್ಥಿರ ವರ್ಣ *BufferAttrs[] = {
    "name", "number", "vars", "options", "valid",
    ರಿಕ್ತ
};

    ಸ್ಥಿರ PyObject *
BufferDir(PyObject *self, PyObject *args UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ ObjectDir(self, BufferAttrs);
}

    ಸ್ಥಿರ PyObject *
BufferAttrValid(BufferObject *self, ವರ್ಣ *name)
{
    PyObject	*ret;

    ಯದಿ (strcmp(name, "valid") != 0)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ret = ((self->buf == INVALID_BUFFER_ಮೌಲ್ಯ) ? Py_False : Py_True);
    Py_INCREF(ret);
    ಪ್ರತ್ಯಾಗಮ ret;
}

    ಸ್ಥಿರ PyObject *
BufferAttr(BufferObject *self, ವರ್ಣ *name)
{
    ಯದಿ (strcmp(name, "name") == 0)
	ಪ್ರತ್ಯಾಗಮ PyString_FromString((self->buf->b_ffname == ರಿಕ್ತ
				    ? "" : (ವರ್ಣ *)self->buf->b_ffname));
    ಅನ್ಯಥಾ ಯದಿ (strcmp(name, "number") == 0)
	ಪ್ರತ್ಯಾಗಮ Py_Build_ಮೌಲ್ಯ(Py_ssize_t_fmt, self->buf->b_fnum);
    ಅನ್ಯಥಾ ಯದಿ (strcmp(name, "vars") == 0)
	ಪ್ರತ್ಯಾಗಮ NEW_DICTIONARY(self->buf->b_vars);
    ಅನ್ಯಥಾ ಯದಿ (strcmp(name, "options") == 0)
	ಪ್ರತ್ಯಾಗಮ OptionsNew(SREQ_BUF, self->buf, CheckBufferCb,
			(PyObject *) self);
    ಅನ್ಯಥಾ ಯದಿ (strcmp(name, "__members__") == 0)
	ಪ್ರತ್ಯಾಗಮ ObjectDir(ರಿಕ್ತ, BufferAttrs);
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
BufferSetattr(PyObject *self_obj, ವರ್ಣ *name, PyObject *valObject)
{
    BufferObject *self = (BufferObject*)self_obj;
    ಯದಿ (CheckBuffer(self))
	ಪ್ರತ್ಯಾಗಮ -1;

    ಯದಿ (strcmp(name, "name") == 0)
    {
	char_u		*val;
	aco_save_T	aco;
	ಪೂರ್ಣಾಙ್ಕ		ren_ret = OK;
	PyObject	*todecref;

	ಯದಿ (!(val = StringToChars(valObject, &todecref)))
	    ಪ್ರತ್ಯಾಗಮ -1;

	VimTryStart();
	// Using aucmd_*: autocommands will be executed by rename_buffer
	aucmd_prepbuf(&aco, self->buf);
	ಯದಿ (curbuf == self->buf)
	{
	    ren_ret = rename_buffer(val);
	    aucmd_restbuf(&aco);
	}
	Py_XDECREF(todecref);
	ಯದಿ (VimTryEnd())
	    ಪ್ರತ್ಯಾಗಮ -1;

	ಯದಿ (ren_ret == FAIL)
	{
	    PyErr_SET_VIM(N_("failed to rename buffer"));
	    ಪ್ರತ್ಯಾಗಮ -1;
	}
	ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಅನ್ಯಥಾ
    {
	PyErr_SetString(PyExc_AttributeError, name);
	ಪ್ರತ್ಯಾಗಮ -1;
    }
}

    ಸ್ಥಿರ PyObject *
BufferAppend(BufferObject *self, PyObject *args)
{
    ಪ್ರತ್ಯಾಗಮ RBAppend(self, args, 1, -1, ರಿಕ್ತ);
}

    ಸ್ಥಿರ PyObject *
BufferMark(BufferObject *self, PyObject *pmarkObject)
{
    pos_T	*posp;
    char_u	*pmark;
    char_u	mark;
    bufref_T	savebuf;
    PyObject	*todecref;

    ಯದಿ (CheckBuffer(self))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (!(pmark = StringToChars(pmarkObject, &todecref)))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (pmark[0] == '\0' || pmark[1] != '\0')
    {
	PyErr_SET_STRING(Py_Exc_ಮೌಲ್ಯ_Error,
		N_("mark name must be a single character"));
	Py_XDECREF(todecref);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    mark = *pmark;

    Py_XDECREF(todecref);

    VimTryStart();
    switch_buffer(&savebuf, self->buf);
    posp = getmark(mark, FALSE);
    restore_buffer(&savebuf);
    ಯದಿ (VimTryEnd())
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (posp == ರಿಕ್ತ)
    {
	PyErr_SET_VIM(N_("invalid mark name"));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಯದಿ (posp->lnum <= 0)
    {
	// Or raise an error?
	Py_INCREF(Py_None);
	ಪ್ರತ್ಯಾಗಮ Py_None;
    }

    ಪ್ರತ್ಯಾಗಮ Py_Build_ಮೌಲ್ಯ("(ll)", (long)(posp->lnum), (long)(posp->col));
}

    ಸ್ಥಿರ PyObject *
BufferRange(BufferObject *self, PyObject *args)
{
    PyInt start;
    PyInt end;

    ಯದಿ (CheckBuffer(self))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (!PyArg_ParseTuple(args, "nn", &start, &end))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಪ್ರತ್ಯಾಗಮ RangeNew(self->buf, start, end);
}

    ಸ್ಥಿರ PyObject *
BufferRepr(PyObject *self_obj)
{
    BufferObject *self = (BufferObject*)self_obj;
    ಯದಿ (self->buf == INVALID_BUFFER_ಮೌಲ್ಯ)
	ಪ್ರತ್ಯಾಗಮ PyString_FromFormat("<buffer object (deleted) at %p>", (ಶೂನ್ಯ *)self);
    ಅನ್ಯಥಾ
    {
	ವರ್ಣ	*name = (ವರ್ಣ *)self->buf->b_fname;

	ಯದಿ (name == ರಿಕ್ತ)
	    name = "";

	ಪ್ರತ್ಯಾಗಮ PyString_FromFormat("<buffer %s>", name);
    }
}

ಸ್ಥಿರ ಸಂರಚನಾ PyMethodDef BufferMethods[] = {
    // name,	    function,			calling,	documentation
    {"append",	    (PyCFunction)BufferAppend,	METH_VARARGS,	"Append data to Vim buffer" },
    {"mark",	    (PyCFunction)BufferMark,	METH_O,		"Return (row,col) representing position of named mark" },
    {"range",	    (PyCFunction)BufferRange,	METH_VARARGS,	"Return a range object which represents the part of the given buffer between line numbers s and e" },
    {"__dir__",	    (PyCFunction)BufferDir,	METH_NOARGS,	""},
    { ರಿಕ್ತ,	    ರಿಕ್ತ,			0,		ರಿಕ್ತ}
};

/*
 * Buffer list object - Implementation
 */

DEFINE_PY_TYPE_OBJECT(BufMapType);

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    PyObject_HEAD
} BufMapObject;

ಸ್ಥಿರ BufMapObject TheBufferMap =
{
    PyObject_HEAD_INIT_TYPE(BufMapType)
};

    ಸ್ಥಿರ PyInt
BufMapLength(PyObject *self UNUSED)
{
    buf_T	*b = firstbuf;
    PyInt	n = 0;

    ಯಾವತ್ (b)
    {
	++n;
	b = b->b_next;
    }

    ಪ್ರತ್ಯಾಗಮ n;
}

    ಸ್ಥಿರ PyObject *
BufMapItem(PyObject *self UNUSED, PyObject *ಕೀಲಿ_Object)
{
    buf_T	*b;
    long	bnr;

    ಯದಿ (ಸಂಖ್ಯೆ_To_Long(ಕೀಲಿ_Object, &bnr, ಸಂಖ್ಯೆ_INT|ಸಂಖ್ಯೆ_NATURAL))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    b = buflist_findnr((ಪೂರ್ಣಾಙ್ಕ) bnr);

    ಯದಿ (b)
	ಪ್ರತ್ಯಾಗಮ BufferNew(b);
    ಅನ್ಯಥಾ
    {
	PyErr_SetObject(Py_Exc_ಕೀಲಿ_Error, ಕೀಲಿ_Object);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
BufMapIterDestruct(ಶೂನ್ಯ* arg)
{
    PyObject *buffer = (PyObject*)arg;
    // Iteration was stopped before all buffers were processed
    ಯದಿ (buffer)
    {
	Py_DECREF(buffer);
    }
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
BufMapIterTraverse(ಶೂನ್ಯ *iter, visitproc visit, ಶೂನ್ಯ *arg)
{
    PyObject *buffer = (PyObject*)iter;
    ಯದಿ (buffer)
	Py_VISIT(buffer);
    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
BufMapIterClear(ಶೂನ್ಯ **iter)
{
    PyObject **buffer = (PyObject**)iter;
    ಯದಿ (*buffer)
	Py_CLEAR(*buffer);
    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಸ್ಥಿರ PyObject *
BufMapIterNext(ಶೂನ್ಯ **arg)
{
    PyObject	*next;
    PyObject	*ret;
    PyObject	**buffer = (PyObject**)arg;

    ಯದಿ (!*buffer)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ret = *buffer;

    ಯದಿ (CheckBuffer((BufferObject *)(ret)))
    {
	*buffer = ರಿಕ್ತ;
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಯದಿ (!((BufferObject *)(ret))->buf->b_next)
	next = ರಿಕ್ತ;
    ಅನ್ಯಥಾ ಯದಿ (!(next = BufferNew(((BufferObject *)(ret))->buf->b_next)))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    *buffer = next;
    // Do not increment reference: we no longer hold it (decref), but whoever
    // on other side will hold (incref). Decref+incref = nothing.
    ಪ್ರತ್ಯಾಗಮ ret;
}

    ಸ್ಥಿರ PyObject *
BufMapIter(PyObject *self)
{
    PyObject *buffer;

    buffer = BufferNew(firstbuf);
    ಪ್ರತ್ಯಾಗಮ IterNew(buffer,
	    BufMapIterDestruct, BufMapIterNext,
	    BufMapIterTraverse, BufMapIterClear,
	    self);
}

ಸ್ಥಿರ PyMappingMethods BufMapAsMapping = {
    (lenfunc)       BufMapLength,
    (binaryfunc)    BufMapItem,
    (objobjargproc) 0,
};

// Current items object

ಸ್ಥಿರ ವರ್ಣ *CurrentAttrs[] = {
    "buffer", "window", "line", "range", "tabpage",
    ರಿಕ್ತ
};

    ಸ್ಥಿರ PyObject *
CurrentDir(PyObject *self, PyObject *args UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ ObjectDir(self, CurrentAttrs);
}

    ಸ್ಥಿರ PyObject *
CurrentGetattr(PyObject *self UNUSED, ವರ್ಣ *name)
{
    ಯದಿ (strcmp(name, "buffer") == 0)
	ಪ್ರತ್ಯಾಗಮ (PyObject *)BufferNew(curbuf);
    ಅನ್ಯಥಾ ಯದಿ (strcmp(name, "window") == 0)
	ಪ್ರತ್ಯಾಗಮ (PyObject *)WindowNew(curwin, curtab);
    ಅನ್ಯಥಾ ಯದಿ (strcmp(name, "tabpage") == 0)
	ಪ್ರತ್ಯಾಗಮ (PyObject *)TabPageNew(curtab);
    ಅನ್ಯಥಾ ಯದಿ (strcmp(name, "line") == 0)
	ಪ್ರತ್ಯಾಗಮ Get_Buffer_ಸಾಲು(curbuf, (PyInt)curwin->w_cursor.lnum);
    ಅನ್ಯಥಾ ಯದಿ (strcmp(name, "range") == 0)
	ಪ್ರತ್ಯಾಗಮ RangeNew(curbuf, RangeStart, RangeEnd);
    ಅನ್ಯಥಾ ಯದಿ (strcmp(name, "__members__") == 0)
	ಪ್ರತ್ಯಾಗಮ ObjectDir(ರಿಕ್ತ, CurrentAttrs);
    ಅನ್ಯಥಾ
#ಯದಿ PY_MAJOR_VERSION < 3
	ಪ್ರತ್ಯಾಗಮ Py_FindMethod(WindowMethods, self, name);
#ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
#endif
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
CurrentSetattr(PyObject *self UNUSED, ವರ್ಣ *name, PyObject *valObject)
{
    ಯದಿ (strcmp(name, "line") == 0)
    {
	ಯದಿ (Set_Buffer_ಸಾಲು(curbuf, (PyInt)curwin->w_cursor.lnum, valObject,
			  ರಿಕ್ತ) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ -1;

	ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಅನ್ಯಥಾ ಯದಿ (strcmp(name, "buffer") == 0)
    {
	ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ;

	ಯದಿ (valObject->ob_type != BufferTypePtr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.Buffer object, but got %s"),
		    valObject);
	    ಪ್ರತ್ಯಾಗಮ -1;
	}

	ಯದಿ (CheckBuffer((BufferObject *)(valObject)))
	    ಪ್ರತ್ಯಾಗಮ -1;
	ಎಣಿಕೆ = ((BufferObject *)(valObject))->buf->b_fnum;

	VimTryStart();
	ಯದಿ (do_buffer(DOBUF_GOTO, DOBUF_FIRST, FORWARD, ಎಣಿಕೆ, 0) == FAIL)
	{
	    ಯದಿ (VimTryEnd())
		ಪ್ರತ್ಯಾಗಮ -1;
	    PyErr_VIM_FORMAT(N_("failed to switch to buffer %d"), ಎಣಿಕೆ);
	    ಪ್ರತ್ಯಾಗಮ -1;
	}

	ಪ್ರತ್ಯಾಗಮ VimTryEnd();
    }
    ಅನ್ಯಥಾ ಯದಿ (strcmp(name, "window") == 0)
    {
	ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ;

	ಯದಿ (valObject->ob_type != WindowTypePtr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.Window object, but got %s"),
		    valObject);
	    ಪ್ರತ್ಯಾಗಮ -1;
	}

	ಯದಿ (CheckWindow((WindowObject *)(valObject)))
	    ಪ್ರತ್ಯಾಗಮ -1;
	ಎಣಿಕೆ = get_win_ಸಂಖ್ಯೆ(((WindowObject *)(valObject))->win, firstwin);

	ಯದಿ (!ಎಣಿಕೆ)
	{
	    PyErr_SET_STRING(Py_Exc_ಮೌಲ್ಯ_Error,
		    N_("failed to find window in the current tab page"));
	    ಪ್ರತ್ಯಾಗಮ -1;
	}

	VimTryStart();
	win_goto(((WindowObject *)(valObject))->win);
	ಯದಿ (((WindowObject *)(valObject))->win != curwin)
	{
	    ಯದಿ (VimTryEnd())
		ಪ್ರತ್ಯಾಗಮ -1;
	    PyErr_SET_STRING(PyExc_RuntimeError,
		    N_("did not switch to the specified window"));
	    ಪ್ರತ್ಯಾಗಮ -1;
	}

	ಪ್ರತ್ಯಾಗಮ VimTryEnd();
    }
    ಅನ್ಯಥಾ ಯದಿ (strcmp(name, "tabpage") == 0)
    {
	ಯದಿ (valObject->ob_type != TabPageTypePtr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.TabPage object, but got %s"),
		    valObject);
	    ಪ್ರತ್ಯಾಗಮ -1;
	}

	ಯದಿ (CheckTabPage((TabPageObject *)(valObject)))
	    ಪ್ರತ್ಯಾಗಮ -1;

	VimTryStart();
	goto_tabpage_tp(((TabPageObject *)(valObject))->tab, TRUE, TRUE);
	ಯದಿ (((TabPageObject *)(valObject))->tab != curtab)
	{
	    ಯದಿ (VimTryEnd())
		ಪ್ರತ್ಯಾಗಮ -1;
	    PyErr_SET_STRING(PyExc_RuntimeError,
		    N_("did not switch to the specified tab page"));
	    ಪ್ರತ್ಯಾಗಮ -1;
	}

	ಪ್ರತ್ಯಾಗಮ VimTryEnd();
    }
    ಅನ್ಯಥಾ
    {
	PyErr_SetString(PyExc_AttributeError, name);
	ಪ್ರತ್ಯಾಗಮ -1;
    }
}

ಸ್ಥಿರ ಸಂರಚನಾ PyMethodDef CurrentMethods[] = {
    // name,	    function,			calling,	documentation
    {"__dir__",	    (PyCFunction)CurrentDir,	METH_NOARGS,	""},
    { ರಿಕ್ತ,	    ರಿಕ್ತ,			0,		ರಿಕ್ತ}
};

    ಸ್ಥಿರ ಶೂನ್ಯ
init_range_cmd(ಶೂನ್ಯ *arg)
{
    exarg_T *eap = (exarg_T*)arg;
    RangeStart = eap->line1;
    RangeEnd = eap->line2;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
init_range_eval(ಶೂನ್ಯ *rettv UNUSED)
{
    RangeStart = (PyInt) curwin->w_cursor.lnum;
    RangeEnd = RangeStart;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
run_cmd(ಧ್ರುವ ವರ್ಣ *cmd, dict_T* locals UNUSED, ಶೂನ್ಯ *arg UNUSED
#ifdef PY_CAN_RECURSE
	, PyGILState_STATE *pygilstate UNUSED
#endif
	)
{
    PyObject	*run_ret;
    run_ret = PyRun_String((ವರ್ಣ *)cmd, Py_file_ಒಳಹರಿವು, globals, globals);
    ಯದಿ (run_ret != ರಿಕ್ತ)
    {
	Py_DECREF(run_ret);
    }
    ಅನ್ಯಥಾ ಯದಿ (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
    {
	emsg(_(e_cant_handle_systemexit_of_python_exception_in_vim));
	PyErr_Clear();
    }
    ಅನ್ಯಥಾ
	PyErr_PrintEx(1);
}

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ	*code_hdr = "def " DOPY_FUNC "(line, linenr):\n ";
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ		code_hdr_len = 30;

    ಸ್ಥಿರ ಶೂನ್ಯ
run_do(ಧ್ರುವ ವರ್ಣ *cmd, dict_T* locals UNUSED, ಶೂನ್ಯ *arg UNUSED
#ifdef PY_CAN_RECURSE
	, PyGILState_STATE *pygilstate
#endif
	)
{
    PyInt	lnum;
    size_t	len;
    ವರ್ಣ	*code;
    ಪೂರ್ಣಾಙ್ಕ		status;
    PyObject	*pyfunc, *pymain;
    PyObject	*run_ret;
    buf_T	*was_curbuf = curbuf;

    ಯದಿ (u_save((linenr_T)RangeStart - 1, (linenr_T)RangeEnd + 1) != OK)
    {
	emsg(_("cannot save undo information"));
	ಪ್ರತ್ಯಾಗಮ;
    }

    len = code_hdr_len + STRLEN(cmd);
    code = PyMem_New(ವರ್ಣ, len + 1);
    memcpy(code, code_hdr, code_hdr_len);
    STRCPY(code + code_hdr_len, cmd);
    run_ret = PyRun_String(code, Py_file_ಒಳಹರಿವು, globals, globals);
    status = -1;
    ಯದಿ (run_ret != ರಿಕ್ತ)
    {
	status = 0;
	Py_DECREF(run_ret);
    }
    ಅನ್ಯಥಾ ಯದಿ (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
    {
	PyMem_Free(code);
	emsg(_(e_cant_handle_systemexit_of_python_exception_in_vim));
	PyErr_Clear();
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಅನ್ಯಥಾ
	PyErr_PrintEx(1);

    PyMem_Free(code);

    ಯದಿ (status)
    {
	emsg(_("failed to run the code"));
	ಪ್ರತ್ಯಾಗಮ;
    }

    status = 0;
    pymain = PyImport_AddModule("__main__");
    pyfunc = PyObject_GetAttrString(pymain, DOPY_FUNC);
#ifdef PY_CAN_RECURSE
    PyGILState_Release(*pygilstate);
#endif

    ಪ್ರತಿ (lnum = RangeStart; lnum <= RangeEnd; ++lnum)
    {
	PyObject	*ಸಾಲು;
	PyObject	*linenr;
	PyObject	*ret;

#ifdef PY_CAN_RECURSE
	*pygilstate = PyGILState_Ensure();
#endif
	// Check the line number, the command my have deleted lines.
	ಯದಿ (lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
		|| !(ಸಾಲು = Get_Buffer_ಸಾಲು(curbuf, lnum)))
	    ಗಚ್ಛ err;
	ಯದಿ (!(linenr = PyInt_FromLong((long) lnum)))
	{
	    Py_DECREF(ಸಾಲು);
	    ಗಚ್ಛ err;
	}
	ret = PyObject_CallFunctionObjArgs(pyfunc, ಸಾಲು, linenr, ರಿಕ್ತ);
	Py_DECREF(ಸಾಲು);
	Py_DECREF(linenr);
	ಯದಿ (!ret)
	    ಗಚ್ಛ err;

	// Check that the command didn't switch to another buffer.
	// Check the line number, the command my have deleted lines.
	ಯದಿ (curbuf != was_curbuf || lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	{
	    Py_XDECREF(ret);
	    ಗಚ್ಛ err;
	}

	ಯದಿ (ret != Py_None)
	    ಯದಿ (Set_Buffer_ಸಾಲು(curbuf, lnum, ret, ರಿಕ್ತ) == FAIL)
	    {
		Py_XDECREF(ret);
		ಗಚ್ಛ err;
	    }

	Py_XDECREF(ret);
	PythonIO_Flush();
#ifdef PY_CAN_RECURSE
	PyGILState_Release(*pygilstate);
#endif
    }
    ಗಚ್ಛ out;
err:
#ifdef PY_CAN_RECURSE
    *pygilstate = PyGILState_Ensure();
#endif
    PyErr_PrintEx(0);
    PythonIO_Flush();
    status = 1;
out:
#ifdef PY_CAN_RECURSE
    ಯದಿ (!status)
	*pygilstate = PyGILState_Ensure();
#endif
    Py_DECREF(pyfunc);
    PyObject_SetAttrString(pymain, DOPY_FUNC, ರಿಕ್ತ);
    ಯದಿ (status)
	ಪ್ರತ್ಯಾಗಮ;
    check_cursor();
    update_curbuf(UPD_NOT_VALID);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
run_eval(ಧ್ರುವ ವರ್ಣ *cmd, dict_T *locals, ಶೂನ್ಯ *arg
#ifdef PY_CAN_RECURSE
	, PyGILState_STATE *pygilstate UNUSED
#endif
	)
{
    PyObject	*run_ret;
    typval_T	*rettv = (typval_T*)arg;
    PyObject	*pylocals = locals ? NEW_DICTIONARY(locals) : globals;

    run_ret = PyRun_String((ವರ್ಣ *)cmd, Py_eval_ಒಳಹರಿವು, globals, pylocals);
    ಯದಿ (run_ret == ರಿಕ್ತ)
    {
	ಯದಿ (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
	{
	    emsg(_(e_cant_handle_systemexit_of_python_exception_in_vim));
	    PyErr_Clear();
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (PyErr_Occurred() && !ಸಂದೇಶ_silent)
		PyErr_PrintEx(0);
	    emsg(_(e_eval_did_not_return_valid_python_object));
	}
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (ConvertFromPyObject(run_ret, rettv) == -1)
	    emsg(_(e_failed_to_convert_returned_python_object_to_vim_ಮೌಲ್ಯ));
	Py_DECREF(run_ret);
    }
    PyErr_Clear();
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
set_ref_in_py(ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ copyID)
{
    pylinkedlist_T	*cur;
    list_T		*ll;
    tuple_T		*tt;
    ಪೂರ್ಣಾಙ್ಕ			i;
    ಪೂರ್ಣಾಙ್ಕ			abort = FALSE;
    FunctionObject	*func;

    ಯದಿ (lastdict != ರಿಕ್ತ)
    {
	ಪ್ರತಿ (cur = lastdict ; !abort && cur != ರಿಕ್ತ ; cur = cur->pll_prev)
	    abort = set_ref_in_dict(((DictionaryObject *)(cur->pll_obj))->dict,
								       copyID);
    }

    ಯದಿ (lastlist != ರಿಕ್ತ)
    {
	ಪ್ರತಿ (cur = lastlist ; !abort && cur != ರಿಕ್ತ ; cur = cur->pll_prev)
	{
	    ll = ((ListObject *) (cur->pll_obj))->list;
	    abort = set_ref_in_list(ll, copyID);
	}
    }

    ಯದಿ (lasttuple != ರಿಕ್ತ)
    {
	ಪ್ರತಿ (cur = lasttuple ; !abort && cur != ರಿಕ್ತ ; cur = cur->pll_prev)
	{
	    tt = ((TupleObject *) (cur->pll_obj))->tuple;
	    abort = set_ref_in_tuple(tt, copyID);
	}
    }

    ಯದಿ (lastfunc != ರಿಕ್ತ)
    {
	ಪ್ರತಿ (cur = lastfunc ; !abort && cur != ರಿಕ್ತ ; cur = cur->pll_prev)
	{
	    func = (FunctionObject *) cur->pll_obj;
	    abort = set_ref_in_dict(func->self, copyID);
	    ಯದಿ (func->argc)
		ಪ್ರತಿ (i = 0; !abort && i < func->argc; ++i)
		    abort = abort
			|| set_ref_in_item(&func->argv[i], copyID, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);
	}
    }

    ಪ್ರತ್ಯಾಗಮ abort;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
set_string_copy(char_u *str, typval_T *tv)
{
    tv->vval.v_string = vim_strsave(str);
    ಯದಿ (tv->vval.v_string == ರಿಕ್ತ)
    {
	PyErr_NoMemory();
	ಪ್ರತ್ಯಾಗಮ -1;
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
pydict_to_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    dict_T	*dict;
    char_u	*ಕೀಲಿ;
    dictitem_T	*di;
    PyObject	*ಕೀಲಿ_Object;
    PyObject	*valObject;
    Py_ssize_t	iter = 0;

    ಯದಿ (!(dict = py_dict_alloc()))
	ಪ್ರತ್ಯಾಗಮ -1;

    tv->v_type = VAR_DICT;
    tv->vval.v_dict = dict;

    ಯಾವತ್ (PyDict_Next(obj, &iter, &ಕೀಲಿ_Object, &valObject))
    {
	PyObject	*todecref = ರಿಕ್ತ;

	ಯದಿ (ಕೀಲಿ_Object == ರಿಕ್ತ || valObject == ರಿಕ್ತ)
	{
	    dict_unref(dict);
	    ಪ್ರತ್ಯಾಗಮ -1;
	}

	ಯದಿ (!(ಕೀಲಿ = StringToChars(ಕೀಲಿ_Object, &todecref)))
	{
	    dict_unref(dict);
	    ಪ್ರತ್ಯಾಗಮ -1;
	}

	ಯದಿ (*ಕೀಲಿ == NUL)
	{
	    dict_unref(dict);
	    Py_XDECREF(todecref);
	    RAISE_NO_EMPTY_ಕೀಲಿಗಳು;
	    ಪ್ರತ್ಯಾಗಮ -1;
	}

	di = dictitem_alloc(ಕೀಲಿ);

	Py_XDECREF(todecref);

	ಯದಿ (di == ರಿಕ್ತ)
	{
	    PyErr_NoMemory();
	    dict_unref(dict);
	    ಪ್ರತ್ಯಾಗಮ -1;
	}

	ಯದಿ (_ConvertFromPyObject(valObject, &di->di_tv, lookup_dict) == -1)
	{
	    vim_free(di);
	    dict_unref(dict);
	    ಪ್ರತ್ಯಾಗಮ -1;
	}

	ಯದಿ (dict_add(dict, di) == FAIL)
	{
	    RAISE_ಕೀಲಿ_ADD_FAIL(di->di_ಕೀಲಿ);
	    clear_tv(&di->di_tv);
	    vim_free(di);
	    dict_unref(dict);
	    ಪ್ರತ್ಯಾಗಮ -1;
	}
    }

    --dict->dv_refcount;
    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
pymap_to_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    dict_T	*dict;
    char_u	*ಕೀಲಿ;
    dictitem_T	*di;
    PyObject	*list;
    PyObject	*iterator;
    PyObject	*ಕೀಲಿ_Object;
    PyObject	*valObject;

    ಯದಿ (!(dict = py_dict_alloc()))
	ಪ್ರತ್ಯಾಗಮ -1;

    tv->v_type = VAR_DICT;
    tv->vval.v_dict = dict;

    ಯದಿ (!(list = Py_Mapping_ಕೀಲಿಗಳು(obj)))
    {
	dict_unref(dict);
	ಪ್ರತ್ಯಾಗಮ -1;
    }

    ಯದಿ (!(iterator = PyObject_GetIter(list)))
    {
	dict_unref(dict);
	Py_DECREF(list);
	ಪ್ರತ್ಯಾಗಮ -1;
    }
    Py_DECREF(list);

    ಯಾವತ್ ((ಕೀಲಿ_Object = PyIter_Next(iterator)))
    {
	PyObject	*todecref;

	ಯದಿ (!(ಕೀಲಿ = StringToChars(ಕೀಲಿ_Object, &todecref)))
	{
	    Py_DECREF(ಕೀಲಿ_Object);
	    Py_DECREF(iterator);
	    dict_unref(dict);
	    ಪ್ರತ್ಯಾಗಮ -1;
	}

	ಯದಿ (*ಕೀಲಿ == NUL)
	{
	    Py_DECREF(ಕೀಲಿ_Object);
	    Py_DECREF(iterator);
	    Py_XDECREF(todecref);
	    dict_unref(dict);
	    RAISE_NO_EMPTY_ಕೀಲಿಗಳು;
	    ಪ್ರತ್ಯಾಗಮ -1;
	}

	ಯದಿ (!(valObject = PyObject_GetItem(obj, ಕೀಲಿ_Object)))
	{
	    Py_DECREF(ಕೀಲಿ_Object);
	    Py_DECREF(iterator);
	    Py_XDECREF(todecref);
	    dict_unref(dict);
	    ಪ್ರತ್ಯಾಗಮ -1;
	}

	di = dictitem_alloc(ಕೀಲಿ);

	Py_DECREF(ಕೀಲಿ_Object);
	Py_XDECREF(todecref);

	ಯದಿ (di == ರಿಕ್ತ)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(valObject);
	    dict_unref(dict);
	    PyErr_NoMemory();
	    ಪ್ರತ್ಯಾಗಮ -1;
	}

	ಯದಿ (_ConvertFromPyObject(valObject, &di->di_tv, lookup_dict) == -1)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(valObject);
	    vim_free(di);
	    dict_unref(dict);
	    ಪ್ರತ್ಯಾಗಮ -1;
	}

	Py_DECREF(valObject);

	ಯದಿ (dict_add(dict, di) == FAIL)
	{
	    RAISE_ಕೀಲಿ_ADD_FAIL(di->di_ಕೀಲಿ);
	    Py_DECREF(iterator);
	    dictitem_free(di);
	    dict_unref(dict);
	    ಪ್ರತ್ಯಾಗಮ -1;
	}
    }
    Py_DECREF(iterator);
    --dict->dv_refcount;
    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
pyseq_to_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    list_T	*l;

    ಯದಿ (!(l = py_list_alloc()))
	ಪ್ರತ್ಯಾಗಮ -1;

    tv->v_type = VAR_LIST;
    tv->vval.v_list = l;

    ಯದಿ (list_py_concat(l, obj, lookup_dict) == -1)
    {
	list_unref(l);
	ಪ್ರತ್ಯಾಗಮ -1;
    }

    --l->lv_refcount;
    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
pytuple_to_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    tuple_T	*t;

    ಯದಿ (!(t = py_tuple_alloc()))
	ಪ್ರತ್ಯಾಗಮ -1;

    tv->v_type = VAR_TUPLE;
    tv->vval.v_tuple = t;

    ಯದಿ (tuple_py_concat(t, obj, lookup_dict) == -1)
    {
	tuple_unref(t);
	ಪ್ರತ್ಯಾಗಮ -1;
    }

    --t->tv_refcount;
    ಪ್ರತ್ಯಾಗಮ 0;
}

ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*pytotvfunc)(PyObject *, typval_T *, PyObject *);

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
convert_dl(PyObject *obj, typval_T *tv,
				    pytotvfunc py_to_tv, PyObject *lookup_dict)
{
    PyObject	*capsule;
    ವರ್ಣ	hexBuf[ಪರಿಮಾಣ(ಶೂನ್ಯ *) * 2 + 3];

    sprintf(hexBuf, "%p", (ಶೂನ್ಯ *)obj);

#ifdef PY_USE_CAPSULE
    capsule = PyDict_GetItemString(lookup_dict, hexBuf);
#ಅನ್ಯಥಾ
    capsule = (PyObject *)PyDict_GetItemString(lookup_dict, hexBuf);
#endif
    ಯದಿ (capsule == ರಿಕ್ತ)
    {
#ifdef PY_USE_CAPSULE
	capsule = PyCapsule_New(tv, ರಿಕ್ತ, ರಿಕ್ತ);
#ಅನ್ಯಥಾ
	capsule = PyCObject_FromVoidPtr(tv, ರಿಕ್ತ);
#endif
	ಯದಿ (PyDict_SetItemString(lookup_dict, hexBuf, capsule))
	{
	    Py_DECREF(capsule);
	    tv->v_type = VAR_UNKNOWN;
	    ಪ್ರತ್ಯಾಗಮ -1;
	}

	Py_DECREF(capsule);

	ಯದಿ (py_to_tv(obj, tv, lookup_dict) == -1)
	{
	    tv->v_type = VAR_UNKNOWN;
	    ಪ್ರತ್ಯಾಗಮ -1;
	}
	// As we are not using copy_tv which increments reference count we must
	// do it ourself.
	ಯದಿ (tv->v_type == VAR_DICT)
	    ++tv->vval.v_dict->dv_refcount;
	ಅನ್ಯಥಾ ಯದಿ (tv->v_type == VAR_LIST)
	    ++tv->vval.v_list->lv_refcount;
	ಅನ್ಯಥಾ ಯದಿ (tv->v_type == VAR_TUPLE)
	    ++tv->vval.v_tuple->tv_refcount;
    }
    ಅನ್ಯಥಾ
    {
	typval_T	*v;

#ifdef PY_USE_CAPSULE
	v = PyCapsule_GetPointer(capsule, ರಿಕ್ತ);
#ಅನ್ಯಥಾ
	v = PyCObject_AsVoidPtr(capsule);
#endif
	copy_tv(v, tv);
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ConvertFromPyMapping(PyObject *obj, typval_T *tv)
{
    PyObject	*lookup_dict;
    ಪೂರ್ಣಾಙ್ಕ		ret;

    ಯದಿ (!(lookup_dict = PyDict_New()))
	ಪ್ರತ್ಯಾಗಮ -1;

    ಯದಿ (PyType_IsSubtype(obj->ob_type, DictionaryTypePtr))
    {
	tv->v_type = VAR_DICT;
	tv->vval.v_dict = (((DictionaryObject *)(obj))->dict);
	++tv->vval.v_dict->dv_refcount;
	ret = 0;
    }
    ಅನ್ಯಥಾ ಯದಿ (PyDict_Check(obj))
	ret = convert_dl(obj, tv, pydict_to_tv, lookup_dict);
    ಅನ್ಯಥಾ ಯದಿ (PyMapping_Check(obj))
	ret = convert_dl(obj, tv, pymap_to_tv, lookup_dict);
    ಅನ್ಯಥಾ
    {
	PyErr_FORMAT_TYPE(
		N_("unable to convert %s to a Vim dictionary"),
		obj);
	ret = -1;
    }
    Py_DECREF(lookup_dict);
    ಪ್ರತ್ಯಾಗಮ ret;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ConvertFromPySequence(PyObject *obj, typval_T *tv)
{
    PyObject	*lookup_dict;
    ಪೂರ್ಣಾಙ್ಕ		ret;

    ಯದಿ (!(lookup_dict = PyDict_New()))
	ಪ್ರತ್ಯಾಗಮ -1;

    ಯದಿ (PyType_IsSubtype(obj->ob_type, ListTypePtr))
    {
	tv->v_type = VAR_LIST;
	tv->vval.v_list = (((ListObject *)(obj))->list);
	++tv->vval.v_list->lv_refcount;
	ret = 0;
    }
    ಅನ್ಯಥಾ ಯದಿ (PyIter_Check(obj) || PySequence_Check(obj))
	ret = convert_dl(obj, tv, pyseq_to_tv, lookup_dict);
    ಅನ್ಯಥಾ
    {
	PyErr_FORMAT_TYPE(
		N_("unable to convert %s to a Vim list"),
		obj);
	ret = -1;
    }
    Py_DECREF(lookup_dict);
    ಪ್ರತ್ಯಾಗಮ ret;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ConvertFromPyObject(PyObject *obj, typval_T *tv)
{
    PyObject	*lookup_dict;
    ಪೂರ್ಣಾಙ್ಕ		ret;

    ಯದಿ (!(lookup_dict = PyDict_New()))
	ಪ್ರತ್ಯಾಗಮ -1;
    ret = _ConvertFromPyObject(obj, tv, lookup_dict);
    Py_DECREF(lookup_dict);
    ಪ್ರತ್ಯಾಗಮ ret;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
_ConvertFromPyObject(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    ಯದಿ (PyType_IsSubtype(obj->ob_type, DictionaryTypePtr))
    {
	tv->v_type = VAR_DICT;
	tv->vval.v_dict = (((DictionaryObject *)(obj))->dict);
	++tv->vval.v_dict->dv_refcount;
    }
    ಅನ್ಯಥಾ ಯದಿ (PyType_IsSubtype(obj->ob_type, ListTypePtr))
    {
	tv->v_type = VAR_LIST;
	tv->vval.v_list = (((ListObject *)(obj))->list);
	++tv->vval.v_list->lv_refcount;
    }
    ಅನ್ಯಥಾ ಯದಿ (PyType_IsSubtype(obj->ob_type, TupleTypePtr))
    {
	tv->v_type = VAR_TUPLE;
	tv->vval.v_tuple = (((TupleObject *)(obj))->tuple);
	++tv->vval.v_tuple->tv_refcount;
    }
    ಅನ್ಯಥಾ ಯದಿ (PyType_IsSubtype(obj->ob_type, FunctionTypePtr))
    {
	FunctionObject *func = (FunctionObject *) obj;
	ಯದಿ (func->self != ರಿಕ್ತ || func->argv != ರಿಕ್ತ)
	{
	    partial_T *pt = ALLOC_CLEAR_ONE(partial_T);

	    set_partial(func, pt, TRUE);
	    tv->vval.v_partial = pt;
	    tv->v_type = VAR_PARTIAL;
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (set_string_copy(func->name, tv) == -1)
		ಪ್ರತ್ಯಾಗಮ -1;

	    tv->v_type = VAR_FUNC;
	}
	func_ref(func->name);
    }
    ಅನ್ಯಥಾ ಯದಿ (PyBytes_Check(obj))
    {
	char_u	*str;

	ಯದಿ (PyBytes_AsStringAndSize(obj, (ವರ್ಣ **) &str, ರಿಕ್ತ) == -1)
	    ಪ್ರತ್ಯಾಗಮ -1;
	ಯದಿ (str == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ -1;

	ಯದಿ (set_string_copy(str, tv) == -1)
	    ಪ್ರತ್ಯಾಗಮ -1;

	tv->v_type = VAR_STRING;
    }
    ಅನ್ಯಥಾ ಯದಿ (PyUnicode_Check(obj))
    {
	PyObject	*bytes;
	char_u	*str;

	bytes = PyUnicode_AsEncodedString(obj, ENC_OPT, ERRORS_ENCODE_ARG);
	ಯದಿ (bytes == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ -1;

	ಯದಿ (PyBytes_AsStringAndSize(bytes, (ವರ್ಣ **) &str, ರಿಕ್ತ) == -1)
	    ಪ್ರತ್ಯಾಗಮ -1;
	ಯದಿ (str == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ -1;

	ಯದಿ (set_string_copy(str, tv))
	{
	    Py_XDECREF(bytes);
	    ಪ್ರತ್ಯಾಗಮ -1;
	}
	Py_XDECREF(bytes);

	tv->v_type = VAR_STRING;
    }
#ಯದಿ PY_MAJOR_VERSION < 3
    ಅನ್ಯಥಾ ಯದಿ (PyInt_Check(obj))
    {
	tv->v_type = VAR_ಸಂಖ್ಯೆ;
	tv->vval.v_ಸಂಖ್ಯೆ = (varnumber_T) PyInt_AsLong(obj);
	ಯದಿ (PyErr_Occurred())
	    ಪ್ರತ್ಯಾಗಮ -1;
    }
#endif
    ಅನ್ಯಥಾ ಯದಿ (PyLong_Check(obj))
    {
	tv->v_type = VAR_ಸಂಖ್ಯೆ;
	tv->vval.v_ಸಂಖ್ಯೆ = (varnumber_T) PyLong_AsLong(obj);
	ಯದಿ (PyErr_Occurred())
	    ಪ್ರತ್ಯಾಗಮ -1;
    }
    ಅನ್ಯಥಾ ಯದಿ (PyTuple_Check(obj))
	ಪ್ರತ್ಯಾಗಮ convert_dl(obj, tv, pytuple_to_tv, lookup_dict);
    ಅನ್ಯಥಾ ಯದಿ (PyDict_Check(obj))
	ಪ್ರತ್ಯಾಗಮ convert_dl(obj, tv, pydict_to_tv, lookup_dict);
    ಅನ್ಯಥಾ ಯದಿ (PyFloat_Check(obj))
    {
	tv->v_type = VAR_FLOAT;
	tv->vval.v_float = (float_T) PyFloat_AsDouble(obj);
    }
    ಅನ್ಯಥಾ ಯದಿ (PyObject_HasAttrString(obj, "keys"))
	ಪ್ರತ್ಯಾಗಮ convert_dl(obj, tv, pymap_to_tv, lookup_dict);
    // PyObject_GetIter can create built-in iterator for any sequence object
    ಅನ್ಯಥಾ ಯದಿ (PyIter_Check(obj) || PySequence_Check(obj))
	ಪ್ರತ್ಯಾಗಮ convert_dl(obj, tv, pyseq_to_tv, lookup_dict);
    ಅನ್ಯಥಾ ಯದಿ (PyMapping_Check(obj))
	ಪ್ರತ್ಯಾಗಮ convert_dl(obj, tv, pymap_to_tv, lookup_dict);
    ಅನ್ಯಥಾ ಯದಿ (Py_ಸಂಖ್ಯೆ_Check(obj))
    {
	PyObject	*ಅಂಕೆ;

	ಯದಿ (!(ಅಂಕೆ = Py_ಸಂಖ್ಯೆ_Long(obj)))
	    ಪ್ರತ್ಯಾಗಮ -1;

	tv->v_type = VAR_ಸಂಖ್ಯೆ;
	tv->vval.v_ಸಂಖ್ಯೆ = (varnumber_T) PyLong_AsLong(ಅಂಕೆ);

	Py_DECREF(ಅಂಕೆ);
    }
    ಅನ್ಯಥಾ ಯದಿ (obj == Py_None)
    {
	tv->v_type = VAR_SPECIAL;
	tv->vval.v_ಸಂಖ್ಯೆ = VVAL_NONE;
    }
    ಅನ್ಯಥಾ
    {
	PyErr_FORMAT_TYPE(
		N_("unable to convert %s to a Vim structure"),
		obj);
	ಪ್ರತ್ಯಾಗಮ -1;
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಸ್ಥಿರ PyObject *
ConvertToPyObject(typval_T *tv)
{
    typval_T *argv;
    ಪೂರ್ಣಾಙ್ಕ i;
    ಯದಿ (tv == ರಿಕ್ತ)
    {
	PyErr_SET_VIM(N_("internal error: NULL reference passed"));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ವಿಕಲ್ಪ (tv->v_type)
    {
	ಪಕ್ಷ VAR_STRING:
	    ಪ್ರತ್ಯಾಗಮ PyBytes_FromString(tv->vval.v_string == ರಿಕ್ತ
					    ? "" : (ವರ್ಣ *)tv->vval.v_string);
	ಪಕ್ಷ VAR_ಸಂಖ್ಯೆ:
	    ಪ್ರತ್ಯಾಗಮ PyLong_FromLong((long) tv->vval.v_ಸಂಖ್ಯೆ);
	ಪಕ್ಷ VAR_FLOAT:
	    ಪ್ರತ್ಯಾಗಮ PyFloat_FromDouble((ದ್ವಿಗುಣ) tv->vval.v_float);
	ಪಕ್ಷ VAR_LIST:
	    ಪ್ರತ್ಯಾಗಮ NEW_LIST(tv->vval.v_list);
	ಪಕ್ಷ VAR_TUPLE:
	    ಪ್ರತ್ಯಾಗಮ NEW_TUPLE(tv->vval.v_tuple);
	ಪಕ್ಷ VAR_DICT:
	    ಪ್ರತ್ಯಾಗಮ NEW_DICTIONARY(tv->vval.v_dict);
	ಪಕ್ಷ VAR_FUNC:
	    ಪ್ರತ್ಯಾಗಮ NEW_FUNCTION(tv->vval.v_string == ರಿಕ್ತ
					  ? (char_u *)"" : tv->vval.v_string,
					  0, ರಿಕ್ತ, ರಿಕ್ತ, TRUE);
	ಪಕ್ಷ VAR_PARTIAL:
	    ಯದಿ (tv->vval.v_partial->pt_argc)
	    {
		argv = PyMem_New(typval_T, (size_t)tv->vval.v_partial->pt_argc);
		ಪ್ರತಿ (i = 0; i < tv->vval.v_partial->pt_argc; i++)
		    copy_tv(&tv->vval.v_partial->pt_argv[i], &argv[i]);
	    }
	    ಅನ್ಯಥಾ
		argv = ರಿಕ್ತ;
	    ಯದಿ (tv->vval.v_partial->pt_dict != ರಿಕ್ತ)
		tv->vval.v_partial->pt_dict->dv_refcount++;
	    ಪ್ರತ್ಯಾಗಮ NEW_FUNCTION(tv->vval.v_partial == ರಿಕ್ತ
			     ? (char_u *)"" : partial_name(tv->vval.v_partial),
				tv->vval.v_partial->pt_argc, argv,
				tv->vval.v_partial->pt_dict,
				tv->vval.v_partial->pt_auto);
	ಪಕ್ಷ VAR_BLOB:
	    ಪ್ರತ್ಯಾಗಮ PyBytes_FromStringAndSize(
		(ವರ್ಣ*) tv->vval.v_blob->bv_ga.ga_data,
		(Py_ssize_t) tv->vval.v_blob->bv_ga.ga_len);
	ಪಕ್ಷ VAR_UNKNOWN:
	ಪಕ್ಷ VAR_ANY:
	ಪಕ್ಷ VAR_VOID:
	ಪಕ್ಷ VAR_CHANNEL:
	ಪಕ್ಷ VAR_JOB:
	ಪಕ್ಷ VAR_INSTR:
	ಪಕ್ಷ VAR_CLASS:
	ಪಕ್ಷ VAR_OBJECT:
	ಪಕ್ಷ VAR_TYPEALIAS:
	    Py_INCREF(Py_None);
	    ಪ್ರತ್ಯಾಗಮ Py_None;
	ಪಕ್ಷ VAR_BOOL:
	ಪಕ್ಷ VAR_SPECIAL:
	    ವಿಕಲ್ಪ (tv->vval.v_ಸಂಖ್ಯೆ)
	    {
		ಪಕ್ಷ VVAL_FALSE: ಪ್ರತ್ಯಾಗಮ ALWAYS_FALSE;
		ಪಕ್ಷ VVAL_TRUE:  ಪ್ರತ್ಯಾಗಮ ALWAYS_TRUE;
		ಪಕ್ಷ VVAL_NONE:
		ಪಕ್ಷ VVAL_NULL:  ಪ್ರತ್ಯಾಗಮ ALWAYS_NONE;
	    }
	    PyErr_SET_VIM(N_("internal error: invalid value type"));
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

DEFINE_PY_TYPE_OBJECT(CurrentType);

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    PyObject_HEAD
} CurrentObject;

ಸ್ಥಿರ CurrentObject TheCurrent =
{
    PyObject_HEAD_INIT_TYPE(CurrentType)
};

    ಸ್ಥಿರ ಶೂನ್ಯ
init_structs(ಶೂನ್ಯ)
{
    CLEAR_FIELD(OutputType);
    OutputType.tp_name = "vim.message";
    OutputType.tp_basicsize = ಪರಿಮಾಣ(OutputObject);
    OutputType.tp_flags = Py_TPFLAGS_DEFAULT;
    OutputType.tp_doc = "vim message object";
    OutputType.tp_methods = OutputMethods;
#ಯದಿ PY_MAJOR_VERSION >= 3
    OutputType.tp_getattro = OutputGetattro;
    OutputType.tp_setattro = OutputSetattro;
    OutputType.tp_alloc = call_PyType_GenericAlloc;
    OutputType.tp_new = call_PyType_GenericNew;
    OutputType.tp_free = call_PyObject_Free;
# ifndef USE_LIMITED_API
    // The std printer type is only exposed in full API. It is not essential
    // anyway and so in limited API we don't set it.
    OutputType.tp_base = &PyStdPrinter_Type;
# endif
#ಅನ್ಯಥಾ
    OutputType.tp_getattr = OutputGetattr;
    OutputType.tp_setattr = OutputSetattr;
    // Disabled, because this causes a crash in test86
    // OutputType.tp_base = &PyFile_Type;
#endif

    CLEAR_FIELD(IterType);
    IterType.tp_name = "vim.iter";
    IterType.tp_basicsize = ಪರಿಮಾಣ(IterObject);
    IterType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    IterType.tp_doc = "generic iterator object";
    IterType.tp_iter = IterIter;
    IterType.tp_iternext = IterNext;
    IterType.tp_dealloc = IterDestructor;
    IterType.tp_traverse = IterTraverse;
    IterType.tp_clear = IterClear;

    CLEAR_FIELD(BufferType);
    BufferType.tp_name = "vim.buffer";
    BufferType.tp_basicsize = ಪರಿಮಾಣ(BufferObject);
    BufferType.tp_dealloc = BufferDestructor;
    BufferType.tp_repr = BufferRepr;
    BufferType.tp_as_sequence = &BufferAsSeq;
    BufferType.tp_as_mapping = &BufferAsMapping;
    BufferType.tp_flags = Py_TPFLAGS_DEFAULT;
    BufferType.tp_doc = "vim buffer object";
    BufferType.tp_methods = BufferMethods;
#ಯದಿ PY_MAJOR_VERSION >= 3
    BufferType.tp_getattro = BufferGetattro;
    BufferType.tp_setattro = BufferSetattro;
    BufferType.tp_alloc = call_PyType_GenericAlloc;
    BufferType.tp_new = call_PyType_GenericNew;
    BufferType.tp_free = call_PyObject_Free;
#ಅನ್ಯಥಾ
    BufferType.tp_getattr = BufferGetattr;
    BufferType.tp_setattr = BufferSetattr;
#endif

    CLEAR_FIELD(WindowType);
    WindowType.tp_name = "vim.window";
    WindowType.tp_basicsize = ಪರಿಮಾಣ(WindowObject);
    WindowType.tp_dealloc = WindowDestructor;
    WindowType.tp_repr = WindowRepr;
    WindowType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    WindowType.tp_doc = "vim Window object";
    WindowType.tp_methods = WindowMethods;
    WindowType.tp_traverse = WindowTraverse;
    WindowType.tp_clear = WindowClear;
#ಯದಿ PY_MAJOR_VERSION >= 3
    WindowType.tp_getattro = WindowGetattro;
    WindowType.tp_setattro = WindowSetattro;
    WindowType.tp_alloc = call_PyType_GenericAlloc;
    WindowType.tp_new = call_PyType_GenericNew;
    WindowType.tp_free = call_PyObject_Free;
#ಅನ್ಯಥಾ
    WindowType.tp_getattr = WindowGetattr;
    WindowType.tp_setattr = WindowSetattr;
#endif

    CLEAR_FIELD(TabPageType);
    TabPageType.tp_name = "vim.tabpage";
    TabPageType.tp_basicsize = ಪರಿಮಾಣ(TabPageObject);
    TabPageType.tp_dealloc = TabPageDestructor;
    TabPageType.tp_repr = TabPageRepr;
    TabPageType.tp_flags = Py_TPFLAGS_DEFAULT;
    TabPageType.tp_doc = "vim tab page object";
    TabPageType.tp_methods = TabPageMethods;
#ಯದಿ PY_MAJOR_VERSION >= 3
    TabPageType.tp_getattro = TabPageGetattro;
    TabPageType.tp_alloc = call_PyType_GenericAlloc;
    TabPageType.tp_new = call_PyType_GenericNew;
    TabPageType.tp_free = call_PyObject_Free;
#ಅನ್ಯಥಾ
    TabPageType.tp_getattr = TabPageGetattr;
#endif

    CLEAR_FIELD(BufMapType);
    BufMapType.tp_name = "vim.bufferlist";
    BufMapType.tp_basicsize = ಪರಿಮಾಣ(BufMapObject);
    BufMapType.tp_as_mapping = &BufMapAsMapping;
    BufMapType.tp_flags = Py_TPFLAGS_DEFAULT;
    BufMapType.tp_iter = BufMapIter;
    BufMapType.tp_doc = "vim buffer list";

    CLEAR_FIELD(WinListType);
    WinListType.tp_name = "vim.windowlist";
    WinListType.tp_basicsize = ಪರಿಮಾಣ(WinListObject);
    WinListType.tp_as_sequence = &WinListAsSeq;
    WinListType.tp_flags = Py_TPFLAGS_DEFAULT;
    WinListType.tp_doc = "vim window list";
    WinListType.tp_dealloc = (destructor)WinListDestructor;

    CLEAR_FIELD(TabListType);
    TabListType.tp_name = "vim.tabpagelist";
    TabListType.tp_basicsize = ಪರಿಮಾಣ(TabListObject);
    TabListType.tp_as_sequence = &TabListAsSeq;
    TabListType.tp_flags = Py_TPFLAGS_DEFAULT;
    TabListType.tp_doc = "vim tab page list";

    CLEAR_FIELD(RangeType);
    RangeType.tp_name = "vim.range";
    RangeType.tp_basicsize = ಪರಿಮಾಣ(RangeObject);
    RangeType.tp_dealloc = RangeDestructor;
    RangeType.tp_repr = RangeRepr;
    RangeType.tp_as_sequence = &RangeAsSeq;
    RangeType.tp_as_mapping = &RangeAsMapping;
    RangeType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    RangeType.tp_doc = "vim Range object";
    RangeType.tp_methods = RangeMethods;
    RangeType.tp_traverse = RangeTraverse;
    RangeType.tp_clear = RangeClear;
#ಯದಿ PY_MAJOR_VERSION >= 3
    RangeType.tp_getattro = RangeGetattro;
    RangeType.tp_alloc = call_PyType_GenericAlloc;
    RangeType.tp_new = call_PyType_GenericNew;
    RangeType.tp_free = call_PyObject_Free;
#ಅನ್ಯಥಾ
    RangeType.tp_getattr = RangeGetattr;
#endif

    CLEAR_FIELD(CurrentType);
    CurrentType.tp_name = "vim.currentdata";
    CurrentType.tp_basicsize = ಪರಿಮಾಣ(CurrentObject);
    CurrentType.tp_flags = Py_TPFLAGS_DEFAULT;
    CurrentType.tp_doc = "vim current object";
    CurrentType.tp_methods = CurrentMethods;
#ಯದಿ PY_MAJOR_VERSION >= 3
    CurrentType.tp_getattro = CurrentGetattro;
    CurrentType.tp_setattro = CurrentSetattro;
#ಅನ್ಯಥಾ
    CurrentType.tp_getattr = CurrentGetattr;
    CurrentType.tp_setattr = CurrentSetattr;
#endif

    CLEAR_FIELD(DictionaryType);
    DictionaryType.tp_name = "vim.dictionary";
    DictionaryType.tp_basicsize = ಪರಿಮಾಣ(DictionaryObject);
    DictionaryType.tp_dealloc = DictionaryDestructor;
    DictionaryType.tp_as_sequence = &DictionaryAsSeq;
    DictionaryType.tp_as_mapping = &DictionaryAsMapping;
    DictionaryType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE;
    DictionaryType.tp_doc = "dictionary pushing modifications to Vim structure";
    DictionaryType.tp_methods = DictionaryMethods;
    DictionaryType.tp_iter = DictionaryIter;
    DictionaryType.tp_new = DictionaryConstructor;
    DictionaryType.tp_alloc = PyType_GenericAlloc;
#ಯದಿ PY_MAJOR_VERSION >= 3
    DictionaryType.tp_getattro = DictionaryGetattro;
    DictionaryType.tp_setattro = DictionarySetattro;
#ಅನ್ಯಥಾ
    DictionaryType.tp_getattr = DictionaryGetattr;
    DictionaryType.tp_setattr = DictionarySetattr;
#endif

    CLEAR_FIELD(ListType);
    ListType.tp_name = "vim.list";
    ListType.tp_dealloc = ListDestructor;
    ListType.tp_basicsize = ಪರಿಮಾಣ(ListObject);
    ListType.tp_as_sequence = &ListAsSeq;
    ListType.tp_as_mapping = &ListAsMapping;
    ListType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE;
    ListType.tp_doc = "list pushing modifications to Vim structure";
    ListType.tp_methods = ListMethods;
    ListType.tp_iter = ListIter;
    ListType.tp_new = ListConstructor;
    ListType.tp_alloc = PyType_GenericAlloc;
#ಯದಿ PY_MAJOR_VERSION >= 3
    ListType.tp_getattro = ListGetattro;
    ListType.tp_setattro = ListSetattro;
#ಅನ್ಯಥಾ
    ListType.tp_getattr = ListGetattr;
    ListType.tp_setattr = ListSetattr;
#endif

    // Tuple type
    CLEAR_FIELD(TupleType);
    TupleType.tp_name = "vim.tuple";
    TupleType.tp_dealloc = TupleDestructor;
    TupleType.tp_basicsize = ಪರಿಮಾಣ(TupleObject);
    TupleType.tp_as_sequence = &TupleAsSeq;
    TupleType.tp_as_mapping = &TupleAsMapping;
    TupleType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE;
    TupleType.tp_doc = "tuple pushing modifications to Vim structure";
    TupleType.tp_methods = TupleMethods;
    TupleType.tp_iter = TupleIter;
    TupleType.tp_new = TupleConstructor;
    TupleType.tp_alloc = PyType_GenericAlloc;
#ಯದಿ PY_MAJOR_VERSION >= 3
    TupleType.tp_getattro = TupleGetattro;
    TupleType.tp_setattro = TupleSetattro;
#ಅನ್ಯಥಾ
    TupleType.tp_getattr = TupleGetattr;
    TupleType.tp_setattr = TupleSetattr;
#endif

    CLEAR_FIELD(FunctionType);
    FunctionType.tp_name = "vim.function";
    FunctionType.tp_basicsize = ಪರಿಮಾಣ(FunctionObject);
    FunctionType.tp_dealloc = FunctionDestructor;
    FunctionType.tp_call = FunctionCall;
    FunctionType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE;
    FunctionType.tp_doc = "object that calls Vim function";
    FunctionType.tp_methods = FunctionMethods;
    FunctionType.tp_repr = FunctionRepr;
    FunctionType.tp_new = FunctionConstructor;
    FunctionType.tp_alloc = PyType_GenericAlloc;
#ಯದಿ PY_MAJOR_VERSION >= 3
    FunctionType.tp_getattro = FunctionGetattro;
#ಅನ್ಯಥಾ
    FunctionType.tp_getattr = FunctionGetattr;
#endif

    CLEAR_FIELD(OptionsType);
    OptionsType.tp_name = "vim.options";
    OptionsType.tp_basicsize = ಪರಿಮಾಣ(OptionsObject);
    OptionsType.tp_as_sequence = &OptionsAsSeq;
    OptionsType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    OptionsType.tp_doc = "object for manipulating options";
    OptionsType.tp_iter = OptionsIter;
    OptionsType.tp_as_mapping = &OptionsAsMapping;
    OptionsType.tp_dealloc = OptionsDestructor;
    OptionsType.tp_traverse = OptionsTraverse;
    OptionsType.tp_clear = OptionsClear;

#ಯದಿ PY_VERSION_HEX < 0x030700f0
    CLEAR_FIELD(LoaderType);
    LoaderType.tp_name = "vim.Loader";
    LoaderType.tp_basicsize = ಪರಿಮಾಣ(LoaderObject);
    LoaderType.tp_flags = Py_TPFLAGS_DEFAULT;
    LoaderType.tp_doc = "vim message object";
    LoaderType.tp_methods = LoaderMethods;
    LoaderType.tp_dealloc = LoaderDestructor;
#endif

#ಯದಿ PY_MAJOR_VERSION >= 3
    CLEAR_FIELD(vimmodule);
    vimmodule.m_name = "vim";
    vimmodule.m_doc = "Vim Python interface\n";
    vimmodule.m_size = -1;
    vimmodule.m_methods = VimMethods;
#endif
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
init_types(ಶೂನ್ಯ)
{
    PYTYPE_READY(IterType);
    PYTYPE_READY(BufferType);
    PYTYPE_READY(RangeType);
    PYTYPE_READY(WindowType);
    PYTYPE_READY(TabPageType);
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
#ಯದಿ PY_VERSION_HEX < 0x030700f0
    PYTYPE_READY(LoaderType);
#endif

#ifdef USE_LIMITED_API
    // We need to finish initializing all the static objects because the types
    // are only just allocated on the heap now.
    // Each PyObject_HEAD_INIT_TYPE should correspond to a
    // PyObject_FINISH_INIT_TYPE below.
    PyObject_FINISH_INIT_TYPE(Output, OutputType);
    PyObject_FINISH_INIT_TYPE(Error, OutputType);
    PyObject_FINISH_INIT_TYPE(TheBufferMap, BufMapType);
    PyObject_FINISH_INIT_TYPE(TheWindowList, WinListType);
    PyObject_FINISH_INIT_TYPE(TheCurrent, CurrentType);
    PyObject_FINISH_INIT_TYPE(TheTabPageList, TabListType);
#endif
    ಪ್ರತ್ಯಾಗಮ 0;
}

#ifdef USE_LIMITED_API
    ಸ್ಥಿರ ಶೂನ್ಯ
shutdown_types(ಶೂನ್ಯ)
{
    PYTYPE_CLEANUP(IterType);
    PYTYPE_CLEANUP(BufferType);
    PYTYPE_CLEANUP(RangeType);
    PYTYPE_CLEANUP(WindowType);
    PYTYPE_CLEANUP(TabPageType);
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
# ಯದಿ PY_VERSION_HEX < 0x030700f0
    PYTYPE_CLEANUP(LoaderType);
# endif
}
#endif

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
init_sys_path(ಶೂನ್ಯ)
{
    PyObject	*path;
    PyObject	*path_hook;
    PyObject	*path_hooks;

    ಯದಿ (!(path_hook = PyObject_GetAttrString(vim_module, "path_hook")))
	ಪ್ರತ್ಯಾಗಮ -1;

    ಯದಿ (!(path_hooks = PySys_GetObject("path_hooks")))
    {
	PyErr_Clear();
	path_hooks = PyList_New(1);
	PyList_SET_ITEM(path_hooks, 0, path_hook);
	ಯದಿ (PySys_SetObject("path_hooks", path_hooks))
	{
	    Py_DECREF(path_hooks);
	    ಪ್ರತ್ಯಾಗಮ -1;
	}
	Py_DECREF(path_hooks);
    }
    ಅನ್ಯಥಾ ಯದಿ (PyList_Check(path_hooks))
    {
	ಯದಿ (PyList_Append(path_hooks, path_hook))
	{
	    Py_DECREF(path_hook);
	    ಪ್ರತ್ಯಾಗಮ -1;
	}
	Py_DECREF(path_hook);
    }
    ಅನ್ಯಥಾ
    {
	VimTryStart();
	emsg(_("Failed to set path hook: sys.path_hooks is not a list\n"
	       "You should now do the following:\n"
	       "- append vim.path_hook to sys.path_hooks\n"
	       "- append vim.VIM_SPECIAL_PATH to sys.path\n"));
	VimTryEnd(); // Discard the error
	Py_DECREF(path_hook);
	ಪ್ರತ್ಯಾಗಮ 0;
    }

    ಯದಿ (!(path = PySys_GetObject("path")))
    {
	PyErr_Clear();
	path = PyList_New(1);
	Py_INCREF(vim_special_path_object);
	PyList_SET_ITEM(path, 0, vim_special_path_object);
	ಯದಿ (PySys_SetObject("path", path))
	{
	    Py_DECREF(path);
	    ಪ್ರತ್ಯಾಗಮ -1;
	}
	Py_DECREF(path);
    }
    ಅನ್ಯಥಾ ಯದಿ (PyList_Check(path))
    {
	ಯದಿ (PyList_Append(path, vim_special_path_object))
	    ಪ್ರತ್ಯಾಗಮ -1;
    }
    ಅನ್ಯಥಾ
    {
	VimTryStart();
	emsg(_("Failed to set path: sys.path is not a list\n"
	       "You should now append vim.VIM_SPECIAL_PATH to sys.path"));
	VimTryEnd(); // Discard the error
    }

    ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಸಂರಚನಾ numeric_constant {
    ವರ್ಣ	*name;
    ಪೂರ್ಣಾಙ್ಕ		val;
} numeric_constants[] = {
    {"VAR_LOCKED",	VAR_LOCKED},
    {"VAR_FIXED",	VAR_FIXED},
    {"VAR_SCOPE",	VAR_SCOPE},
    {"VAR_DEF_SCOPE",	VAR_DEF_SCOPE},
};

ಸಂರಚನಾ object_constant {
    ವರ್ಣ	*name;
    PyObject	*valObject;
};

#define ADD_OBJECT(m, name, obj) \
    ಯದಿ (PyModule_AddObject(m, name, obj)) \
	ಪ್ರತ್ಯಾಗಮ -1;

#define ADD_CHECKED_OBJECT(m, name, obj) \
    { \
	PyObject	*valObject = obj; \
	ಯದಿ (!valObject) \
	    ಪ್ರತ್ಯಾಗಮ -1; \
	ADD_OBJECT(m, name, valObject); \
    }

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
populate_module(PyObject *m)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    PyObject	*other_module;
    PyObject	*attr;
    PyObject	*imp;
#ಯದಿ PY_VERSION_HEX >= 0x030700f0
    PyObject	*dict;
    PyObject	*cls;
#endif

    ಪ್ರತಿ (i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)(ಪರಿಮಾಣ(numeric_constants)
					   / ಪರಿಮಾಣ(ಸಂರಚನಾ numeric_constant));
	    ++i)
	ADD_CHECKED_OBJECT(m, numeric_constants[i].name,
		PyInt_FromLong(numeric_constants[i].val));

    ಸಂರಚನಾ object_constant object_constants[] = {
	{"buffers",  (PyObject *)(ಶೂನ್ಯ *)&TheBufferMap},
	{"windows",  (PyObject *)(ಶೂನ್ಯ *)&TheWindowList},
	{"tabpages", (PyObject *)(ಶೂನ್ಯ *)&TheTabPageList},
	{"current",  (PyObject *)(ಶೂನ್ಯ *)&TheCurrent},

	{"Buffer",     (PyObject *)BufferTypePtr},
	{"Range",      (PyObject *)RangeTypePtr},
	{"Window",     (PyObject *)WindowTypePtr},
	{"TabPage",    (PyObject *)TabPageTypePtr},
	{"Dictionary", (PyObject *)DictionaryTypePtr},
	{"List",       (PyObject *)ListTypePtr},
	{"Tuple",      (PyObject *)TupleTypePtr},
	{"Function",   (PyObject *)FunctionTypePtr},
	{"Options",    (PyObject *)OptionsTypePtr},
#ಯದಿ PY_VERSION_HEX < 0x030700f0
	{"_Loader",    (PyObject *)LoaderTypePtr},
#endif
    };

    ಪ್ರತಿ (i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)(ಪರಿಮಾಣ(object_constants)
					    / ಪರಿಮಾಣ(ಸಂರಚನಾ object_constant));
	    ++i)
    {
	PyObject	*valObject;

	valObject = object_constants[i].valObject;
	Py_INCREF(valObject);
	ADD_OBJECT(m, object_constants[i].name, valObject);
    }

    ಯದಿ (!(VimError = PyErr_NewException("vim.error", ರಿಕ್ತ, ರಿಕ್ತ)))
	ಪ್ರತ್ಯಾಗಮ -1;
    ADD_OBJECT(m, "error", VimError);

    ADD_CHECKED_OBJECT(m, "vars",  NEW_DICTIONARY(get_globvar_dict()));
    ADD_CHECKED_OBJECT(m, "vvars", NEW_DICTIONARY(get_vimvar_dict()));
    ADD_CHECKED_OBJECT(m, "options",
	    OptionsNew(SREQ_GLOBAL, ರಿಕ್ತ, dummy_check, ರಿಕ್ತ));

    ಯದಿ (!(other_module = PyImport_ImportModule("os")))
	ಪ್ರತ್ಯಾಗಮ -1;
    ADD_OBJECT(m, "os", other_module);

#ಯದಿ PY_MAJOR_VERSION >= 3
    ಯದಿ (!(py_getcwd = PyObject_GetAttrString(other_module, "getcwd")))
	ಪ್ರತ್ಯಾಗಮ -1;
#ಅನ್ಯಥಾ
    ಯದಿ (!(py_getcwd = PyObject_GetAttrString(other_module, "getcwdu")))
	ಪ್ರತ್ಯಾಗಮ -1;
#endif
    ADD_OBJECT(m, "_getcwd", py_getcwd)

    ಯದಿ (!(py_chdir = PyObject_GetAttrString(other_module, "chdir")))
	ಪ್ರತ್ಯಾಗಮ -1;
    ADD_OBJECT(m, "_chdir", py_chdir);
    ಯದಿ (!(attr = PyObject_GetAttrString(m, "chdir")))
	ಪ್ರತ್ಯಾಗಮ -1;
    ಯದಿ (PyObject_SetAttrString(other_module, "chdir", attr))
    {
	Py_DECREF(attr);
	ಪ್ರತ್ಯಾಗಮ -1;
    }
    Py_DECREF(attr);

    ಯದಿ ((py_fchdir = PyObject_GetAttrString(other_module, "fchdir")))
    {
	ADD_OBJECT(m, "_fchdir", py_fchdir);
	ಯದಿ (!(attr = PyObject_GetAttrString(m, "fchdir")))
	    ಪ್ರತ್ಯಾಗಮ -1;
	ಯದಿ (PyObject_SetAttrString(other_module, "fchdir", attr))
	{
	    Py_DECREF(attr);
	    ಪ್ರತ್ಯಾಗಮ -1;
	}
	Py_DECREF(attr);
    }
    ಅನ್ಯಥಾ
	PyErr_Clear();

    ಯದಿ (!(vim_special_path_object = PyString_FromString(vim_special_path)))
	ಪ್ರತ್ಯಾಗಮ -1;

    ADD_OBJECT(m, "VIM_SPECIAL_PATH", vim_special_path_object);

#ಯದಿ PY_VERSION_HEX >= 0x030700f0
    ಯದಿ (!(imp = PyImport_ImportModule("importlib.machinery")))
	ಪ್ರತ್ಯಾಗಮ -1;

    dict = PyModule_GetDict(imp);

    ಯದಿ (!(cls = PyDict_GetItemString(dict, "PathFinder")))
    {
	Py_DECREF(imp);
	ಪ್ರತ್ಯಾಗಮ -1;
    }

    ಯದಿ (!(py_find_spec = PyObject_GetAttrString(cls, "find_spec")))
    {
	Py_DECREF(imp);
	ಪ್ರತ್ಯಾಗಮ -1;
    }

# ಯದಿ PY_VERSION_HEX < 0x30c00a7
    // find_module has been removed as of Python 3.12.0a7
    ಯದಿ ((py_find_module = PyObject_GetAttrString(cls, "find_module")))
	ADD_OBJECT(m, "_find_module", py_find_module);
# endif

    Py_DECREF(imp);

    ADD_OBJECT(m, "_find_spec", py_find_spec);
#ಅನ್ಯಥಾ
    ಯದಿ (!(imp = PyImport_ImportModule("imp")))
	ಪ್ರತ್ಯಾಗಮ -1;

    ಯದಿ (!(py_find_module = PyObject_GetAttrString(imp, "find_module")))
    {
	Py_DECREF(imp);
	ಪ್ರತ್ಯಾಗಮ -1;
    }

    ಯದಿ (!(py_load_module = PyObject_GetAttrString(imp, "load_module")))
    {
	Py_DECREF(py_find_module);
	Py_DECREF(imp);
	ಪ್ರತ್ಯಾಗಮ -1;
    }

    Py_DECREF(imp);

    ADD_OBJECT(m, "_find_module", py_find_module);
    ADD_OBJECT(m, "_load_module", py_load_module);
#endif

    ಪ್ರತ್ಯಾಗಮ 0;
}
