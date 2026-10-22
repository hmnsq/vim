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

#ଯଦି PY_VERSION_HEX < 0x02050000
ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା Py_ssize_t;  // Python 2.4 and earlier don't have this type.
#endif

// Use values that are known to work, others may make Vim crash.
#define ENC_OPT (enc_utf8 ? "utf-8" : enc_dbcs ? "euc-jp" : (ଅକ୍ଷର *)p_enc)
#define DOPY_FUNC "_vim_pydo"

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *vim_special_path = "_vim_path_";

#define PyErr_SET_STRING(exc, str) PyErr_SetString(exc, _(str))
#define PyErr_SetVim(str) PyErr_SetString(VimError, str)
#define PyErr_SET_VIM(str) PyErr_SET_STRING(VimError, str)
#define PyErr_FORMAT(exc, str, arg) PyErr_Format(exc, _(str), arg)
#define PyErr_FORMAT2(exc, str, arg1, arg2) PyErr_Format(exc, _(str), arg1,arg2)
#define PyErr_VIM_FORMAT(str, arg) PyErr_FORMAT(VimError, str, arg)

#ifdef USE_LIMITED_API
// Limited Python API. Need to call only exposed functions and remap macros.
// PyTypeObject is an opaque struct.

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    lenfunc sq_length;
    binaryfunc sq_concat;
    ssizeargfunc sq_repeat;
    ssizeargfunc sq_item;
    ଶୂନ୍ୟ *was_sq_slice;
    ssizeobjargproc sq_ass_item;
    ଶୂନ୍ୟ *was_sq_ass_slice;
    objobjproc sq_contains;

    binaryfunc sq_inplace_concat;
    ssizeargfunc sq_inplace_repeat;
} PySequenceMethods;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    lenfunc mp_length;
    binaryfunc mp_subscript;
    objobjargproc mp_ass_subscript;
} PyMappingMethods;

// This struct emulates the concrete _typeobject struct to allow the code to
// work the same way in both limited and full Python APIs.
ସଂରଚନା typeobject_wrapper {
    ଅଚଳ ଅକ୍ଷର *tp_name;
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

    ଅଚଳ ଅକ୍ଷର *tp_doc;

    traverseproc tp_traverse;

    inquiry tp_clear;

    getiterfunc tp_iter;
    iternextfunc tp_iternext;

    ସଂରଚନା PyMethodDef *tp_methods;
    ସଂରଚନା _typeobject *tp_base;
    allocfunc tp_alloc;
    newfunc tp_new;
    freefunc tp_free;
};

# define DEFINE_PY_TYPE_OBJECT(type) \
    ସ୍ଥିର ସଂରଚନା typeobject_wrapper type; \
    ସ୍ଥିର PyTypeObject* type##Ptr = ଖାଲି

// PyObject_HEAD_INIT_TYPE and PyObject_FINISH_INIT_TYPE need to come in pairs
// We first initialize with NULL because the type is not allocated until
// init_types() is called later. It's in FINISH_INIT_TYPE where we fill the
// type in with the newly allocated type.
# define PyObject_HEAD_INIT_TYPE(type) PyObject_HEAD_INIT(ଖାଲି)
# define PyObject_FINISH_INIT_TYPE(obj, type) obj.ob_base.ob_type = type##Ptr

# define Py_TYPE_GET_TP_ALLOC(type) ((allocfunc)PyType_GetSlot(type, Py_tp_alloc))
# define Py_TYPE_GET_TP_METHODS(type) ((PyMethodDef *)PyType_GetSlot(type, Py_tp_methods))

// PyObject_NEW is not part of stable ABI, but PyObject_Malloc/Init are.
ସ୍ଥିର PyObject* Vim_PyObject_New(PyTypeObject *type, size_t objsize)
{
    PyObject *obj = (PyObject *)PyObject_Malloc(objsize);
    ଯଦି (obj == ଖାଲି)
	ଫେରନ୍ତୁ PyErr_NoMemory();
    ଫେରନ୍ତୁ PyObject_Init(obj, type);
}
# undef PyObject_NEW
# define PyObject_NEW(type, typeobj) ((type *)Vim_PyObject_New(typeobj, ଆକାର(type)))

// This is a somewhat convoluted because limited API doesn't expose an easy way
// to get the tp_name field, and so we have to manually reconstruct it as
// "__module__.__name__" (with __module__ omitted for builtins to emulate
// Python behavior). Also, some of the more convenient functions like
// PyUnicode_AsUTF8AndSize and PyType_GetQualName() are not available until
// late Python 3 versions, and won't be available if you set Py_LIMITED_API too
// low.
# define PyErr_FORMAT_TYPE(ସନ୍ଦେଶ, obj) \
    କର { \
	PyObject* qualname = PyObject_GetAttrString((PyObject*)(obj)->ob_type, "__qualname__"); \
	ଯଦି (qualname == ଖାଲି) \
	{ \
	    PyErr_FORMAT(PyExc_TypeError, ସନ୍ଦେଶ, "(NULL)"); \
	    ଭାଙ୍ଗ; \
	} \
	PyObject* module = PyObject_GetAttrString((PyObject*)(obj)->ob_type, "__module__"); \
	PyObject* full; \
	ଯଦି (module == ଖାଲି || PyUnicode_CompareWithASCIIString(module, "builtins") == 0 \
		|| PyUnicode_CompareWithASCIIString(module, "__main__") == 0) \
	{ \
	    full = qualname; \
	    Py_INCREF(full); \
	} \
	ଅନ୍ୟଥା \
	    full = PyUnicode_FromFormat("%U.%U", module, qualname); \
	PyObject* full_bytes = PyUnicode_AsUTF8String(full); \
	ଅଚଳ ଅକ୍ଷର* full_str = PyBytes_AsString(full_bytes); \
	full_str = full_str == ଖାଲି ? "(NULL)" : full_str; \
	PyErr_FORMAT(PyExc_TypeError, ସନ୍ଦେଶ, full_str); \
	Py_DECREF(qualname); \
	Py_XDECREF(module); \
	Py_XDECREF(full); \
	Py_XDECREF(full_bytes); \
    } ଯେତେବେଳେ (0)

# define PyList_GET_ITEM(list, i) PyList_GetItem(list, i)
# define PyList_GET_SIZE(o) PyList_Size(o)

// PyList_SET_ITEM and PyList_SetItem have slightly different behaviors. The
// former will leave the old item dangling, and the latter will decref on it.
// Since we only use this on new lists, this difference doesn't matter.
# define PyList_SET_ITEM(list, i, item) PyList_SetItem(list, i, item)

# define PyTuple_GET_ITEM(o, pos) PyTuple_GetItem(o, pos)
# define PyTuple_GET_SIZE(o) PyTuple_Size(o)

# ଯଦି Py_LIMITED_API < 0x03080000
// PyIter_check only became part of stable ABI in 3.8, and there is no easy way
// to check for it in the API. We simply return false as a compromise. This
// does mean we should avoid compiling with stable ABI < 3.8.
#  undef PyIter_Check
#  define PyIter_Check(obj) (FALSE)
# endif

ସ୍ଥିର PyTypeObject* AddHeapType(ସଂରଚନା typeobject_wrapper* type_object)
{
    PyType_Spec type_spec;
    type_spec.name = type_object->tp_name;
    type_spec.basicsize = type_object->tp_basicsize;
    type_spec.itemsize = 0;
    type_spec.flags = type_object->tp_flags;

    // We just need to statically allocate a large enough buffer that can hold
    // all slots. We need to leave a null-terminated slot at the end.
    PyType_Slot slots[40] = { {0, ଖାଲି} };
    size_t slot_i = 0;

# define ADD_TP_SLOT(slot_name) \
    ଯଦି (slot_i >= 40) ଫେରନ୍ତୁ ଖାଲି; /* this should never happen */ \
    ଯଦି (type_object->slot_name != ଖାଲି) \
    { \
	slots[slot_i].slot = Py_##slot_name; \
	slots[slot_i].pfunc = (ଶୂନ୍ୟ*)type_object->slot_name; \
	++slot_i; \
    }
# define ADD_TP_SUB_SLOT(sub_slot, slot_name) \
    ଯଦି (slot_i >= 40) ଫେରନ୍ତୁ ଖାଲି; /* this should never happen */ \
    ଯଦି (type_object->sub_slot != ଖାଲି && type_object->sub_slot->slot_name != ଖାଲି) \
    { \
	slots[slot_i].slot = Py_##slot_name; \
	slots[slot_i].pfunc = (ଶୂନ୍ୟ*)type_object->sub_slot->slot_name; \
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
    ଫେରନ୍ତୁ (PyTypeObject*)newtype;
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
    ଯଦି (type##Ptr == ଖାଲି) \
	ଫେରନ୍ତୁ -1;
# define PYTYPE_CLEANUP(type) \
    type##Ptr = ଖାଲି;

// Limited API does not provide PyRun_* functions. Need to implement manually
// using PyCompile and PyEval.
ସ୍ଥିର PyObject* Vim_PyRun_String(ଅଚଳ ଅକ୍ଷର *str, ପୂର୍ଣ୍ଣସଂଖ୍ୟା start, PyObject *globals, PyObject *locals)
{
    // Just pass "" for filename for now.
    PyObject* compiled = Py_CompileString(str, "", start);
    ଯଦି (compiled == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    PyObject* eval_result = PyEval_EvalCode(compiled, globals, locals);
    Py_DECREF(compiled);
    ଫେରନ୍ତୁ eval_result;
}
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା Vim_PyRun_SimpleString(ଅଚଳ ଅକ୍ଷର *str)
{
    // This function emulates CPython's implementation.
    PyObject* m = PyImport_AddModule("__main__");
    ଯଦି (m == ଖାଲି)
	ଫେରନ୍ତୁ -1;
    PyObject* d = PyModule_GetDict(m);
    PyObject* output = Vim_PyRun_String(str, Py_file_ନିବେଶ, d, d);
    ଯଦି (output == ଖାଲି)
    {
	PyErr_PrintEx(TRUE);
	ଫେରନ୍ତୁ -1;
    }
    Py_DECREF(output);
    ଫେରନ୍ତୁ 0;
}
# define PyRun_String Vim_PyRun_String
# define PyRun_SimpleString Vim_PyRun_SimpleString

#ଅନ୍ୟଥା // !defined(USE_LIMITED_API)

// Full Python API. Can make use of structs and macros directly.
# define DEFINE_PY_TYPE_OBJECT(type) \
    ସ୍ଥିର PyTypeObject type; \
    ସ୍ଥିର PyTypeObject* type##Ptr = &type
# define PyObject_HEAD_INIT_TYPE(type) PyObject_HEAD_INIT(&type)

# define Py_TYPE_GET_TP_ALLOC(type) type->tp_alloc
# define Py_TYPE_GET_TP_METHODS(type) type->tp_methods

# define Py_TYPE_NAME(obj) ((obj)->ob_type->tp_name == ଖାଲି \
	? "(NULL)" \
	: (obj)->ob_type->tp_name)
# define PyErr_FORMAT_TYPE(ସନ୍ଦେଶ, obj) \
    PyErr_FORMAT(PyExc_TypeError, ସନ୍ଦେଶ, \
	    Py_TYPE_NAME(obj))

// Add a static type
# define PYTYPE_READY(type) \
    ଯଦି (PyType_Ready(type##Ptr)) \
	ଫେରନ୍ତୁ -1;

#endif


#define RAISE_NO_EMPTY_ଚାବିଗୁଡ଼ିକ PyErr_SET_STRING(Py_Exc_ମୂଲ୍ୟ_Error, \
					    N_("empty keys are not allowed"))
#define RAISE_LOCKED_DICTIONARY PyErr_SET_VIM(N_("dictionary is locked"))
#define RAISE_LOCKED_LIST PyErr_SET_VIM(N_("list is locked"))
#define RAISE_UNDO_FAIL PyErr_SET_VIM(N_("cannot save undo information"))
#define RAISE_DELETE_ଧାଡ଼ି_FAIL PyErr_SET_VIM(N_("cannot delete line"))
#define RAISE_ଭର୍ତ୍ତି_ଧାଡ଼ି_FAIL PyErr_SET_VIM(N_("cannot insert line"))
#define RAISE_REPLACE_ଧାଡ଼ି_FAIL PyErr_SET_VIM(N_("cannot replace line"))
#define RAISE_ଚାବି_ADD_FAIL(ଚାବି) \
    PyErr_VIM_FORMAT(N_("failed to add key '%s' to dictionary"), ଚାବି)
#define RAISE_INVALID_INDEX_TYPE(idx) \
    PyErr_FORMAT_TYPE(N_("index must be int or slice, not %s"), idx);

#define INVALID_BUFFER_ମୂଲ୍ୟ ((buf_T *)(-1))
#define INVALID_WINDOW_ମୂଲ୍ୟ ((win_T *)(-1))
#define INVALID_TABPAGE_ମୂଲ୍ୟ ((tabpage_T *)(-1))

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଶୂନ୍ୟ (*rangeinitializer)(ଶୂନ୍ୟ *);
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଶୂନ୍ୟ (*runner)(ଅଚଳ ଅକ୍ଷର *, dict_T *, ଶୂନ୍ୟ *
#ifdef PY_CAN_RECURSE
	, PyGILState_STATE *
#endif
	);

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ConvertFromPyObject(PyObject *, typval_T *);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା _ConvertFromPyObject(PyObject *, typval_T *, PyObject *);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ConvertFromPyMapping(PyObject *, typval_T *);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ConvertFromPySequence(PyObject *, typval_T *);
ସ୍ଥିର PyObject *WindowNew(win_T *, tabpage_T *);
ସ୍ଥିର PyObject *BufferNew (buf_T *);
ସ୍ଥିର PyObject *ଧାଡ଼ି_To_String(ଅଚଳ ଅକ୍ଷର *);

ସ୍ଥିର PyInt RangeStart;
ସ୍ଥିର PyInt RangeEnd;

ସ୍ଥିର PyObject *globals;

ସ୍ଥିର PyObject *py_chdir;
ସ୍ଥିର PyObject *py_fchdir;
ସ୍ଥିର PyObject *py_getcwd;
ସ୍ଥିର PyObject *vim_module;
ସ୍ଥିର PyObject *vim_special_path_object;

#ଯଦି PY_VERSION_HEX >= 0x030700f0
ସ୍ଥିର PyObject *py_find_spec;
#ଅନ୍ୟଥା
ସ୍ଥିର PyObject *py_load_module;
#endif
#ଯଦି PY_VERSION_HEX < 0x30c00a7
ସ୍ଥିର PyObject *py_find_module;
#endif

ସ୍ଥିର PyObject *VimError;

/*
 * obtain a lock on the Vim data structures
 */
    ସ୍ଥିର ଶୂନ୍ୟ
Python_Lock_Vim(ଶୂନ୍ୟ)
{
}

/*
 * release a lock on the Vim data structures
 */
    ସ୍ଥିର ଶୂନ୍ୟ
Python_Release_Vim(ଶୂନ୍ୟ)
{
}

/*
 * The "todecref" argument holds a pointer to PyObject * that must be
 * DECREF'ed after returned char_u * is no longer needed or NULL if all what
 * was needed to generate returned value is object.
 *
 * Use Py_XDECREF to decrement reference count.
 */
    ସ୍ଥିର char_u *
StringToChars(PyObject *obj, PyObject **todecref)
{
    char_u	*str;

    ଯଦି (PyBytes_Check(obj))
    {

	ଯଦି (PyBytes_AsStringAndSize(obj, (ଅକ୍ଷର **) &str, ଖାଲି) == -1
		|| str == ଖାଲି)
	    ଫେରନ୍ତୁ ଖାଲି;

	*todecref = ଖାଲି;
    }
    ଅନ୍ୟଥା ଯଦି (PyUnicode_Check(obj))
    {
	PyObject	*bytes;

	ଯଦି (!(bytes = PyUnicode_AsEncodedString(obj, ENC_OPT,
							   ERRORS_ENCODE_ARG)))
	    ଫେରନ୍ତୁ ଖାଲି;

	ଯଦି (PyBytes_AsStringAndSize(bytes, (ଅକ୍ଷର **) &str, ଖାଲି) == -1
		|| str == ଖାଲି)
	{
	    Py_DECREF(bytes);
	    ଫେରନ୍ତୁ ଖାଲି;
	}

	*todecref = bytes;
    }
    ଅନ୍ୟଥା
    {
#ଯଦି PY_MAJOR_VERSION < 3
	PyErr_FORMAT_TYPE(N_("expected str() or unicode() instance, but got %s"),
		obj);
#ଅନ୍ୟଥା
	PyErr_FORMAT_TYPE(N_("expected bytes() or str() instance, but got %s"),
		obj);
#endif
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ଫେରନ୍ତୁ (char_u *) str;
}

#define ସଂଖ୍ୟା_LONG     1
#define ସଂଖ୍ୟା_INT      2
#define ସଂଖ୍ୟା_NATURAL  4
#define ସଂଖ୍ୟା_UNSIGNED 8

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ସଂଖ୍ୟା_To_Long(PyObject *obj, long *result, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
#ଯଦି PY_MAJOR_VERSION < 3
    ଯଦି (PyInt_Check(obj))
    {
	*result = PyInt_AsLong(obj);
	ଯଦି (PyErr_Occurred())
	    ଫେରନ୍ତୁ -1;
    }
    ଅନ୍ୟଥା
#endif
    ଯଦି (PyLong_Check(obj))
    {
	*result = PyLong_AsLong(obj);
	ଯଦି (PyErr_Occurred())
	    ଫେରନ୍ତୁ -1;
    }
    ଅନ୍ୟଥା ଯଦି (Py_ସଂଖ୍ୟା_Check(obj))
    {
	PyObject	*ଅଙ୍କ;

	ଯଦି (!(ଅଙ୍କ = Py_ସଂଖ୍ୟା_Long(obj)))
	    ଫେରନ୍ତୁ -1;

	*result = PyLong_AsLong(ଅଙ୍କ);

	Py_DECREF(ଅଙ୍କ);

	ଯଦି (PyErr_Occurred())
	    ଫେରନ୍ତୁ -1;
    }
    ଅନ୍ୟଥା
    {
#ଯଦି PY_MAJOR_VERSION < 3
	PyErr_FORMAT_TYPE(
		N_("expected int(), long() or something supporting "
		   "coercing to long(), but got %s"),
		obj);
#ଅନ୍ୟଥା
	PyErr_FORMAT_TYPE(
		N_("expected int() or something supporting coercing to int(), "
		   "but got %s"),
		obj);
#endif
	ଫେରନ୍ତୁ -1;
    }

    ଯଦି (flags & ସଂଖ୍ୟା_INT)
    {
	ଯଦି (*result > INT_MAX)
	{
	    PyErr_SET_STRING(PyExc_OverflowError,
		    N_("value is too large to fit into C int type"));
	    ଫେରନ୍ତୁ -1;
	}
	ଅନ୍ୟଥା ଯଦି (*result < INT_MIN)
	{
	    PyErr_SET_STRING(PyExc_OverflowError,
		    N_("value is too small to fit into C int type"));
	    ଫେରନ୍ତୁ -1;
	}
    }

    ଯଦି (flags & ସଂଖ୍ୟା_NATURAL)
    {
	ଯଦି (*result <= 0)
	{
	    PyErr_SET_STRING(Py_Exc_ମୂଲ୍ୟ_Error,
		    N_("number must be greater than zero"));
	    ଫେରନ୍ତୁ -1;
	}
    }
    ଅନ୍ୟଥା ଯଦି (flags & ସଂଖ୍ୟା_UNSIGNED)
    {
	ଯଦି (*result < 0)
	{
	    PyErr_SET_STRING(Py_Exc_ମୂଲ୍ୟ_Error,
		    N_("number must be greater or equal to zero"));
	    ଫେରନ୍ତୁ -1;
	}
    }

    ଫେରନ୍ତୁ 0;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
add_string(PyObject *list, ଅକ୍ଷର *s)
{
    PyObject	*string;

    ଯଦି (!(string = PyString_FromString(s)))
	ଫେରନ୍ତୁ -1;

    ଯଦି (PyList_Append(list, string))
    {
	Py_DECREF(string);
	ଫେରନ୍ତୁ -1;
    }

    Py_DECREF(string);
    ଫେରନ୍ତୁ 0;
}

    ସ୍ଥିର PyObject *
ObjectDir(PyObject *self, ଅକ୍ଷର **attributes)
{
    PyMethodDef	*method;
    ଅକ୍ଷର	**attr;
    PyObject	*ret;

    ଯଦି (!(ret = PyList_New(0)))
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (self)
	ପାଇଁ (method = Py_TYPE_GET_TP_METHODS(self->ob_type) ; method->ml_name != ଖାଲି ; ++method)
	    ଯଦି (add_string(ret, (ଅକ୍ଷର *)method->ml_name))
	    {
		Py_DECREF(ret);
		ଫେରନ୍ତୁ ଖାଲି;
	    }

    ପାଇଁ (attr = attributes ; *attr ; ++attr)
	ଯଦି (add_string(ret, *attr))
	{
	    Py_DECREF(ret);
	    ଫେରନ୍ତୁ ଖାଲି;
	}

#ଯଦି PY_MAJOR_VERSION < 3
    ଯଦି (add_string(ret, "__members__"))
    {
	Py_DECREF(ret);
	ଫେରନ୍ତୁ ଖାଲି;
    }
#endif

    ଫେରନ୍ତୁ ret;
}

// Output buffer management

// Function to write a line, points to either msg() or emsg().
ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*writefn)(ଅକ୍ଷର *);

DEFINE_PY_TYPE_OBJECT(OutputType);

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    PyObject_HEAD
    long softspace;
    long error;
} OutputObject;

ସ୍ଥିର ଅକ୍ଷର *OutputAttrs[] = {
    "softspace",
    ଖାଲି
};

    ସ୍ଥିର PyObject *
OutputDir(PyObject *self, PyObject *args UNUSED)
{
    ଫେରନ୍ତୁ ObjectDir(self, OutputAttrs);
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
OutputSetattr(PyObject *self_obj, ଅକ୍ଷର *name, PyObject *valObject)
{
    OutputObject *self = (OutputObject*)self_obj;
    ଯଦି (valObject == ଖାଲି)
    {
	PyErr_SET_STRING(PyExc_AttributeError,
		N_("can't delete OutputObject attributes"));
	ଫେରନ୍ତୁ -1;
    }

    ଯଦି (strcmp(name, "softspace") == 0)
    {
	ଯଦି (ସଂଖ୍ୟା_To_Long(valObject, &(self->softspace), ସଂଖ୍ୟା_UNSIGNED))
	    ଫେରନ୍ତୁ -1;
	ଫେରନ୍ତୁ 0;
    }

    PyErr_FORMAT(PyExc_AttributeError, N_("invalid attribute: %s"), name);
    ଫେରନ୍ତୁ -1;
}

// Buffer IO, we write one whole line at a time.
ସ୍ଥିର garray_T io_ga = {0, 0, 1, 80, ଖାଲି};
ସ୍ଥିର writefn old_fn = ଖାଲି;

    ସ୍ଥିର ଶୂନ୍ୟ
PythonIO_Flush(ଶୂନ୍ୟ)
{
    ଯଦି (old_fn != ଖାଲି && io_ga.ga_len > 0)
    {
	((ଅକ୍ଷର *)io_ga.ga_data)[io_ga.ga_len] = NUL;
	// We don't know what emsg_severe should be here, so ... hope?
	old_fn((ଅକ୍ଷର *)io_ga.ga_data);
    }
    io_ga.ga_len = 0;
}

    ସ୍ଥିର ଶୂନ୍ୟ
writer(writefn fn, char_u *str, PyInt n, ପୂର୍ଣ୍ଣସଂଖ୍ୟା severe)
{
    char_u *ptr;

    // Flush when switching output function.
    ଯଦି (fn != old_fn)
	PythonIO_Flush();
    old_fn = fn;

    // Write each NL separated line.  Text after the last NL is kept for
    // writing later.
    // For normal messages: Do not output when "got_int" was set.  This avoids
    // a loop gone crazy flooding the terminal with messages.  Also for when
    // "q" is pressed at the more-prompt.
    ଯେତେବେଳେ (n > 0 && (ptr = memchr(str, '\n', n)) != ଖାଲି
					  && (fn == (writefn)emsg || !got_int))
    {
	PyInt len = ptr - str;

	ଯଦି (ga_grow(&io_ga, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(len + 1)) == FAIL)
	    ଭାଙ୍ଗ;

	mch_memmove(((ଅକ୍ଷର *)io_ga.ga_data) + io_ga.ga_len, str, (size_t)len);
	((ଅକ୍ଷର *)io_ga.ga_data)[io_ga.ga_len + len] = NUL;
	emsg_severe = severe;
	fn((ଅକ୍ଷର *)io_ga.ga_data);
	str = ptr + 1;
	n -= len + 1;
	io_ga.ga_len = 0;
    }

    // Put the remaining text into io_ga for later printing.
    ଯଦି (n > 0 && (fn == (writefn)emsg || !got_int)
					&& ga_grow(&io_ga, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(n + 1)) == OK)
    {
	mch_memmove(((ଅକ୍ଷର *)io_ga.ga_data) + io_ga.ga_len, str, (size_t)n);
	io_ga.ga_len += (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n;
    }
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
write_output(OutputObject *self, PyObject *string)
{
    Py_ssize_t	len = 0;
    ଅକ୍ଷର	*str = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = self->error;

    ଯଦି (!PyArg_Parse(string, "et#", ENC_OPT, &str, &len))
	ଫେରନ୍ତୁ -1;

    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    writer((writefn)(error ? emsg : ସନ୍ଦେଶ), (char_u *)str, len, error);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS
    PyMem_Free(str);

    ଫେରନ୍ତୁ 0;
}

    ସ୍ଥିର PyObject *
OutputWrite(OutputObject *self, PyObject *string)
{
    ଯଦି (write_output(self, string))
	ଫେରନ୍ତୁ ଖାଲି;

    Py_INCREF(Py_None);
    ଫେରନ୍ତୁ Py_None;
}

    ସ୍ଥିର PyObject *
OutputWritelines(OutputObject *self, PyObject *seq)
{
    PyObject	*iterator;
    PyObject	*item;

    ଯଦି (!(iterator = PyObject_GetIter(seq)))
	ଫେରନ୍ତୁ ଖାଲି;

    ଯେତେବେଳେ ((item = PyIter_Next(iterator)))
    {
	ଯଦି (write_output(self, item))
	{
	    Py_DECREF(iterator);
	    Py_DECREF(item);
	    ଫେରନ୍ତୁ ଖାଲି;
	}
	Py_DECREF(item);
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    ଯଦି (PyErr_Occurred())
	ଫେରନ୍ତୁ ଖାଲି;

    Py_INCREF(Py_None);
    ଫେରନ୍ତୁ Py_None;
}

    ସ୍ଥିର PyObject *
AlwaysNone(PyObject *self UNUSED, PyObject *args UNUSED)
{
    // do nothing
    Py_INCREF(Py_None);
    ଫେରନ୍ତୁ Py_None;
}
#define ALWAYS_NONE AlwaysNone(ଖାଲି, ଖାଲି)

    ସ୍ଥିର PyObject *
AlwaysFalse(PyObject *self UNUSED, PyObject *args UNUSED)
{
    // do nothing
    PyObject	*ret = Py_False;
    Py_INCREF(ret);
    ଫେରନ୍ତୁ ret;
}
#define ALWAYS_FALSE AlwaysFalse(ଖାଲି, ଖାଲି)

    ସ୍ଥିର PyObject *
AlwaysTrue(PyObject *self UNUSED, PyObject *args UNUSED)
{
    // do nothing
    PyObject	*ret = Py_True;
    Py_INCREF(ret);
    ଫେରନ୍ତୁ ret;
}
#define ALWAYS_TRUE AlwaysTrue(ଖାଲି, ଖାଲି)

/***************/

ସ୍ଥିର ସଂରଚନା PyMethodDef OutputMethods[] = {
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
    { ଖାଲି,	    ଖାଲି,				0,		ଖାଲି}
};

ସ୍ଥିର OutputObject Output =
{
    PyObject_HEAD_INIT_TYPE(OutputType)
    0,
    0
};

ସ୍ଥିର OutputObject Error =
{
    PyObject_HEAD_INIT_TYPE(OutputType)
    0,
    1
};

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
PythonIO_Init_io(ଶୂନ୍ୟ)
{
    ଯଦି (PySys_SetObject("stdout", (PyObject *)(ଶୂନ୍ୟ *)&Output))
	ଫେରନ୍ତୁ -1;
    ଯଦି (PySys_SetObject("stderr", (PyObject *)(ଶୂନ୍ୟ *)&Error))
	ଫେରନ୍ତୁ -1;

    ଯଦି (PyErr_Occurred())
    {
	emsg(_(e_python_error_initialising_io_object));
	ଫେରନ୍ତୁ -1;
    }

    ଫେରନ୍ତୁ 0;
}

#ଯଦି PY_VERSION_HEX < 0x030700f0
ସ୍ଥିର PyObject *call_load_module(ଅକ୍ଷର *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len, PyObject *find_module_result);

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    PyObject_HEAD
    ଅକ୍ଷର	*fullname;
    PyObject	*result;
} LoaderObject;
DEFINE_PY_TYPE_OBJECT(LoaderType);

    ସ୍ଥିର ଶୂନ୍ୟ
LoaderDestructor(PyObject *self_obj)
{
    LoaderObject *self = (LoaderObject*)self_obj;
    vim_free(self->fullname);
    Py_XDECREF(self->result);
    DESTRUCTOR_FINISH(self);
}

    ସ୍ଥିର PyObject *
LoaderLoadModule(LoaderObject *self, PyObject *args UNUSED)
{
    ଅକ୍ଷର	*fullname = self->fullname;
    PyObject	*result = self->result;
    PyObject	*module;

    ଯଦି (!fullname)
    {
	module = result ? result : Py_None;
	Py_INCREF(module);
	ଫେରନ୍ତୁ module;
    }

    module = call_load_module(fullname, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(fullname), result);

    self->fullname = ଖାଲି;
    self->result = module;

    vim_free(fullname);
    Py_DECREF(result);

    ଯଦି (!module)
    {
	ଯଦି (PyErr_Occurred())
	    ଫେରନ୍ତୁ ଖାଲି;

	Py_INCREF(Py_None);
	ଫେରନ୍ତୁ Py_None;
    }

    Py_INCREF(module);
    ଫେରନ୍ତୁ module;
}

ସ୍ଥିର ସଂରଚନା PyMethodDef LoaderMethods[] = {
    // name,	    function,				calling,	doc
    {"load_module", (PyCFunction)LoaderLoadModule,	METH_VARARGS,	""},
    { ଖାଲି,	    ଖାଲି,				0,		ଖାଲି}
};
#endif

/*
 * Check to see whether a Vim error has been reported, or a keyboard
 * interrupt has been detected.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
VimTryStart(ଶୂନ୍ୟ)
{
    ++trylevel;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
VimTryEnd(ଶୂନ୍ୟ)
{
    --trylevel;
    // Without this it stops processing all subsequent Vim script commands and
    // generates strange error messages if I e.g. try calling Test() in a cycle
    did_emsg = FALSE;
    // Keyboard interrupt should be preferred over anything else
    ଯଦି (got_int)
    {
	ଯଦି (did_throw)
	    discard_current_exception();
	got_int = FALSE;
	PyErr_SetNone(PyExc_KeyboardInterrupt);
	ଫେରନ୍ତୁ -1;
    }
    ଅନ୍ୟଥା ଯଦି (ସନ୍ଦେଶ_list != ଖାଲି && *ସନ୍ଦେଶ_list != ଖାଲି)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	should_free;
	ଅକ୍ଷର	*ସନ୍ଦେଶ;

	ସନ୍ଦେଶ = get_exception_string(*ସନ୍ଦେଶ_list, ET_ERROR, ଖାଲି, &should_free);

	ଯଦି (ସନ୍ଦେଶ == ଖାଲି)
	{
	    PyErr_NoMemory();
	    ଫେରନ୍ତୁ -1;
	}

	PyErr_SetVim(ସନ୍ଦେଶ);

	free_global_msglist();

	ଯଦି (should_free)
	    vim_free(ସନ୍ଦେଶ);

	ଫେରନ୍ତୁ -1;
    }
    ଅନ୍ୟଥା ଯଦି (!did_throw)
	ଫେରନ୍ତୁ (PyErr_Occurred() ? -1 : 0);
    // Python exception is preferred over Vim one; unlikely to occur though
    ଅନ୍ୟଥା ଯଦି (PyErr_Occurred())
    {
	discard_current_exception();
	ଫେରନ୍ତୁ -1;
    }
    // Finally transform Vim script exception to python one
    ଅନ୍ୟଥା
    {
	PyErr_SetVim((ଅକ୍ଷର *)current_exception->ମୂଲ୍ୟ);
	discard_current_exception();
	ଫେରନ୍ତୁ -1;
    }
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
VimCheckInterrupt(ଶୂନ୍ୟ)
{
    ଯଦି (got_int)
    {
	PyErr_SetNone(PyExc_KeyboardInterrupt);
	ଫେରନ୍ତୁ 1;
    }
    ଫେରନ୍ତୁ 0;
}

// Vim module - Implementation

    ସ୍ଥିର PyObject *
VimCommand(PyObject *self UNUSED, PyObject *string)
{
    char_u	*cmd;
    PyObject	*ret;
    PyObject	*todecref;

    ଯଦି (!(cmd = StringToChars(string, &todecref)))
	ଫେରନ୍ତୁ ଖାଲି;

    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();

    VimTryStart();
    do_cmdline_cmd(cmd);
    update_screen(UPD_VALID);

    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    ଯଦି (VimTryEnd())
	ret = ଖାଲି;
    ଅନ୍ୟଥା
	ret = Py_None;

    Py_XINCREF(ret);
    Py_XDECREF(todecref);
    ଫେରନ୍ତୁ ret;
}

/*
 * Function to translate a typval_T into a PyObject; this will recursively
 * translate lists/dictionaries into their Python equivalents.
 *
 * The depth parameter is to avoid infinite recursion, set it to 1 when
 * you call VimToPython.
 */
    ସ୍ଥିର PyObject *
VimToPython(typval_T *our_tv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା depth, PyObject *lookup_dict)
{
    PyObject	*ret;
    PyObject	*newObj;
    ଅକ୍ଷର	ptrBuf[ଆକାର(ଶୂନ୍ୟ *) * 2 + 3];

    // Avoid infinite recursion
    ଯଦି (depth > 100)
    {
	Py_INCREF(Py_None);
	ret = Py_None;
	ଫେରନ୍ତୁ ret;
    }

    // Check if we run into a recursive loop.  The item must be in lookup_dict
    // then and we can use it again.
    ଯଦି ((our_tv->v_type == VAR_LIST && our_tv->vval.v_list != ଖାଲି)
	    || (our_tv->v_type == VAR_TUPLE && our_tv->vval.v_tuple != ଖାଲି)
	    || (our_tv->v_type == VAR_DICT && our_tv->vval.v_dict != ଖାଲି))
    {
	sprintf(ptrBuf, "%p",
		our_tv->v_type == VAR_LIST ? (ଶୂନ୍ୟ *)our_tv->vval.v_list
		    : our_tv->v_type == VAR_TUPLE ?
			(ଶୂନ୍ୟ *)our_tv->vval.v_tuple
			: (ଶୂନ୍ୟ *)our_tv->vval.v_dict);

	ଯଦି ((ret = PyDict_GetItemString(lookup_dict, ptrBuf)))
	{
	    Py_INCREF(ret);
	    ଫେରନ୍ତୁ ret;
	}
    }

    ଯଦି (our_tv->v_type == VAR_STRING)
	ret = PyString_FromString(our_tv->vval.v_string == ଖାଲି
					? "" : (ଅକ୍ଷର *)our_tv->vval.v_string);
    ଅନ୍ୟଥା ଯଦି (our_tv->v_type == VAR_ସଂଖ୍ୟା)
    {
	ଅକ୍ଷର buf[NUMBUFLEN];

	// For backwards compatibility numbers are stored as strings.
	sprintf(buf, "%lld", (long long)our_tv->vval.v_ସଂଖ୍ୟା);
	ret = PyString_FromString((ଅକ୍ଷର *)buf);
    }
    ଅନ୍ୟଥା ଯଦି (our_tv->v_type == VAR_FLOAT)
    {
	ଅକ୍ଷର buf[NUMBUFLEN];

	sprintf(buf, "%f", our_tv->vval.v_float);
	ret = PyString_FromString((ଅକ୍ଷର *)buf);
    }
    ଅନ୍ୟଥା ଯଦି (our_tv->v_type == VAR_LIST)
    {
	list_T		*list = our_tv->vval.v_list;
	listitem_T	*curr;

	ଯଦି (list == ଖାଲି)
	    ଫେରନ୍ତୁ ଖାଲି;

	ଯଦି (!(ret = PyList_New(0)))
	    ଫେରନ୍ତୁ ଖାଲି;

	ଯଦି (PyDict_SetItemString(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    ଫେରନ୍ତୁ ଖାଲି;
	}

	CHECK_LIST_MATERIALIZE(list);
	FOR_ALL_LIST_ବସ୍ତୁଗୁଡ଼ିକ(list, curr)
	{
	    ଯଦି (!(newObj = VimToPython(&curr->li_tv, depth + 1, lookup_dict)))
	    {
		Py_DECREF(ret);
		ଫେରନ୍ତୁ ଖାଲି;
	    }
	    ଯଦି (PyList_Append(ret, newObj))
	    {
		Py_DECREF(newObj);
		Py_DECREF(ret);
		ଫେରନ୍ତୁ ଖାଲି;
	    }
	    Py_DECREF(newObj);
	}
    }
    ଅନ୍ୟଥା ଯଦି (our_tv->v_type == VAR_TUPLE)
    {
	tuple_T	*tuple = our_tv->vval.v_tuple;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	len;

	ଯଦି (tuple == ଖାଲି)
	    ଫେରନ୍ତୁ ଖାଲି;

	len = TUPLE_LEN(tuple);

	ret = PyTuple_New(len);
	ଯଦି (ret == ଖାଲି)
	    ଫେରନ୍ତୁ ଖାଲି;

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = 0; idx < len; idx++)
	{
	    typval_T	*item_tv = TUPLE_ITEM(tuple, idx);

	    newObj = VimToPython(item_tv, depth + 1, lookup_dict);
	    ଯଦି (!newObj)
	    {
		Py_DECREF(ret);
		ଫେରନ୍ତୁ ଖାଲି;
	    }
	    PyTuple_SetItem(ret, idx, newObj);
	}

	ଯଦି (PyDict_SetItemString(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    ଫେରନ୍ତୁ ଖାଲି;
	}
    }
    ଅନ୍ୟଥା ଯଦି (our_tv->v_type == VAR_DICT)
    {

	hashtab_T	*ht;
	long_u		todo;
	hashitem_T	*hi;
	dictitem_T	*di;

	ଯଦି (our_tv->vval.v_dict == ଖାଲି)
	    ଫେରନ୍ତୁ ଖାଲି;
	ht = &our_tv->vval.v_dict->dv_hashtab;

	ଯଦି (!(ret = PyDict_New()))
	    ଫେରନ୍ତୁ ଖାଲି;

	ଯଦି (PyDict_SetItemString(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    ଫେରନ୍ତୁ ଖାଲି;
	}

	todo = ht->ht_used;
	ପାଇଁ (hi = ht->ht_array; todo > 0; ++hi)
	{
	    ଯଦି (!HASHITEM_EMPTY(hi))
	    {
		--todo;

		di = dict_lookup(hi);
		ଯଦି (!(newObj = VimToPython(&di->di_tv, depth + 1, lookup_dict)))
		{
		    Py_DECREF(ret);
		    ଫେରନ୍ତୁ ଖାଲି;
		}
		ଯଦି (PyDict_SetItemString(ret, (ଅକ୍ଷର *)hi->hi_ଚାବି, newObj))
		{
		    Py_DECREF(ret);
		    Py_DECREF(newObj);
		    ଫେରନ୍ତୁ ଖାଲି;
		}
	    }
	}
    }
    ଅନ୍ୟଥା ଯଦି (our_tv->v_type == VAR_BOOL)
    {
	ଯଦି (our_tv->vval.v_ସଂଖ୍ୟା == VVAL_FALSE)
	{
	    ret = Py_False;
	    Py_INCREF(ret);
	}
	ଅନ୍ୟଥା
	{
	    ret = Py_True;
	    Py_INCREF(ret);
	}
	ଫେରନ୍ତୁ ret;
    }
    ଅନ୍ୟଥା ଯଦି (our_tv->v_type == VAR_SPECIAL)
    {
	Py_INCREF(Py_None);
	ret = Py_None;
	ଫେରନ୍ତୁ ret;
    }
    ଅନ୍ୟଥା ଯଦି (our_tv->v_type == VAR_BLOB)
	ret = PyBytes_FromStringAndSize(
		(ଅକ୍ଷର*) our_tv->vval.v_blob->bv_ga.ga_data,
		(Py_ssize_t) our_tv->vval.v_blob->bv_ga.ga_len);
    ଅନ୍ୟଥା
    {
	Py_INCREF(Py_None);
	ret = Py_None;
    }

    ଫେରନ୍ତୁ ret;
}

    ସ୍ଥିର PyObject *
VimEval(PyObject *self UNUSED, PyObject *args)
{
    char_u	*ପଦସମଷ୍ଟି;
    typval_T	*our_tv;
    PyObject	*string;
    PyObject	*todecref;
    PyObject	*ret;
    PyObject	*lookup_dict;

    ଯଦି (!PyArg_ParseTuple(args, "O", &string))
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (!(ପଦସମଷ୍ଟି = StringToChars(string, &todecref)))
	ଫେରନ୍ତୁ ଖାଲି;

    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    VimTryStart();
    our_tv = eval_ପଦସମଷ୍ଟି(ପଦସମଷ୍ଟି, ଖାଲି);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    Py_XDECREF(todecref);

    ଯଦି (VimTryEnd())
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (our_tv == ଖାଲି)
    {
	PyErr_SET_VIM(N_("invalid expression"));
	ଫେରନ୍ତୁ ଖାଲି;
    }

    // Convert the Vim type into a Python type.  Create a dictionary that's
    // used to check for recursive loops.
    ଯଦି (!(lookup_dict = PyDict_New()))
	ret = ଖାଲି;
    ଅନ୍ୟଥା
    {
	ret = VimToPython(our_tv, 1, lookup_dict);
	Py_DECREF(lookup_dict);
    }


    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    free_tv(our_tv);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    ଫେରନ୍ତୁ ret;
}

ସ୍ଥିର PyObject *ConvertToPyObject(typval_T *);

    ସ୍ଥିର PyObject *
VimEvalPy(PyObject *self UNUSED, PyObject *string)
{
    typval_T	*our_tv;
    PyObject	*ret;
    char_u	*ପଦସମଷ୍ଟି;
    PyObject	*todecref;

    ଯଦି (!(ପଦସମଷ୍ଟି = StringToChars(string, &todecref)))
	ଫେରନ୍ତୁ ଖାଲି;

    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    VimTryStart();
    our_tv = eval_ପଦସମଷ୍ଟି(ପଦସମଷ୍ଟି, ଖାଲି);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    Py_XDECREF(todecref);

    ଯଦି (VimTryEnd())
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (our_tv == ଖାଲି)
    {
	PyErr_SET_VIM(N_("invalid expression"));
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ret = ConvertToPyObject(our_tv);
    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    free_tv(our_tv);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    ଫେରନ୍ତୁ ret;
}

    ସ୍ଥିର PyObject *
VimStrwidth(PyObject *self UNUSED, PyObject *string)
{
    char_u	*str;
    PyObject	*todecref;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;

    ଯଦି (!(str = StringToChars(string, &todecref)))
	ଫେରନ୍ତୁ ଖାଲି;

    len = mb_string2cells(str, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(str));

    Py_XDECREF(todecref);

    ଫେରନ୍ତୁ PyLong_FromLong(len);
}

    ସ୍ଥିର PyObject *
_VimChdir(PyObject *_chdir, PyObject *args, PyObject *kwargs)
{
    PyObject	*ret;
    PyObject	*newwd;
    PyObject	*todecref;
    char_u	*new_dir;

    ଯଦି (_chdir == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    ଯଦି (!(ret = PyObject_Call(_chdir, args, kwargs)))
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (!(newwd = PyObject_CallFunctionObjArgs(py_getcwd, ଖାଲି)))
    {
	Py_DECREF(ret);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ଯଦି (!(new_dir = StringToChars(newwd, &todecref)))
    {
	Py_DECREF(ret);
	Py_DECREF(newwd);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    VimTryStart();

    ଯଦି (vim_chdir(new_dir))
    {
	Py_DECREF(ret);
	Py_DECREF(newwd);
	Py_XDECREF(todecref);

	ଯଦି (VimTryEnd())
	    ଫେରନ୍ତୁ ଖାଲି;

	PyErr_SET_VIM(N_("failed to change directory"));
	ଫେରନ୍ତୁ ଖାଲି;
    }

    Py_DECREF(newwd);
    Py_XDECREF(todecref);

    post_chdir(CDSCOPE_GLOBAL);

    ଯଦି (VimTryEnd())
    {
	Py_DECREF(ret);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ଫେରନ୍ତୁ ret;
}

    ସ୍ଥିର PyObject *
VimChdir(PyObject *self UNUSED, PyObject *args, PyObject *kwargs)
{
    ଫେରନ୍ତୁ _VimChdir(py_chdir, args, kwargs);
}

    ସ୍ଥିର PyObject *
VimFchdir(PyObject *self UNUSED, PyObject *args, PyObject *kwargs)
{
    ଫେରନ୍ତୁ _VimChdir(py_fchdir, args, kwargs);
}

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    PyObject	*callable;
    PyObject	*result;
} map_rtp_data;

    ସ୍ଥିର ଶୂନ୍ୟ
map_rtp_callback(char_u *path, ଶୂନ୍ୟ *_data)
{
    ଶୂନ୍ୟ	**data = (ଶୂନ୍ୟ **) _data;
    PyObject	*pathObject;
    map_rtp_data	*mr_data = *((map_rtp_data **) data);

    ଯଦି (!(pathObject = PyString_FromString((ଅକ୍ଷର *)path)))
    {
	*data = ଖାଲି;
	ଫେରନ୍ତୁ;
    }

    mr_data->result = PyObject_CallFunctionObjArgs(mr_data->callable,
						   pathObject, ଖାଲି);

    Py_DECREF(pathObject);

    ଯଦି (!mr_data->result || mr_data->result != Py_None)
	*data = ଖାଲି;
    ଅନ୍ୟଥା
    {
	Py_DECREF(mr_data->result);
	mr_data->result = ଖାଲି;
    }
}

    ସ୍ଥିର PyObject *
VimForeachRTP(PyObject *self UNUSED, PyObject *callable)
{
    map_rtp_data	data;

    data.callable = callable;
    data.result = ଖାଲି;

    do_in_runtimepath(ଖାଲି, 0, &map_rtp_callback, &data);

    ଯଦି (data.result == ଖାଲି)
    {
	ଯଦି (PyErr_Occurred())
	    ଫେରନ୍ତୁ ଖାଲି;
	ଅନ୍ୟଥା
	{
	    Py_INCREF(Py_None);
	    ଫେରନ୍ତୁ Py_None;
	}
    }
    ଫେରନ୍ତୁ data.result;
}

/*
 * _vim_runtimepath_ special path implementation.
 */

    ସ୍ଥିର ଶୂନ୍ୟ
map_finder_callback(char_u *path, ଶୂନ୍ୟ *_data)
{
    ଶୂନ୍ୟ	**data = (ଶୂନ୍ୟ **) _data;
    PyObject	*list = *((PyObject **) data);
    PyObject	*pathObject1, *pathObject2;
    ଅକ୍ଷର	*pathbuf;
    size_t	pathlen;

    pathlen = STRLEN(path);

#ଯଦି PY_MAJOR_VERSION < 3
# define PY_MAIN_DIR_STRING "python2"
#ଅନ୍ୟଥା
# define PY_MAIN_DIR_STRING "python3"
#endif
#define PY_ALTERNATE_DIR_STRING "pythonx"

#define PYTHONX_STRING_LENGTH 7 // STRLEN("pythonx")
    ଯଦି (!(pathbuf = PyMem_New(ଅକ୍ଷର,
		    pathlen + STRLEN(PATHSEPSTR) + PYTHONX_STRING_LENGTH + 1)))
    {
	PyErr_NoMemory();
	*data = ଖାଲି;
	ଫେରନ୍ତୁ;
    }

    mch_memmove(pathbuf, path, pathlen + 1);
    add_pathsep((char_u *) pathbuf);

    pathlen = STRLEN(pathbuf);
    mch_memmove(pathbuf + pathlen, PY_MAIN_DIR_STRING,
	    PYTHONX_STRING_LENGTH + 1);

    ଯଦି (!(pathObject1 = PyString_FromString(pathbuf)))
    {
	*data = ଖାଲି;
	PyMem_Free(pathbuf);
	ଫେରନ୍ତୁ;
    }

    mch_memmove(pathbuf + pathlen, PY_ALTERNATE_DIR_STRING,
	    PYTHONX_STRING_LENGTH + 1);

    ଯଦି (!(pathObject2 = PyString_FromString(pathbuf)))
    {
	Py_DECREF(pathObject1);
	PyMem_Free(pathbuf);
	*data = ଖାଲି;
	ଫେରନ୍ତୁ;
    }

    PyMem_Free(pathbuf);

    ଯଦି (PyList_Append(list, pathObject1)
	    || PyList_Append(list, pathObject2))
	*data = ଖାଲି;

    Py_DECREF(pathObject1);
    Py_DECREF(pathObject2);
}

    ସ୍ଥିର PyObject *
Vim_GetPaths(PyObject *self UNUSED, PyObject *args UNUSED)
{
    PyObject	*ret;

    ଯଦି (!(ret = PyList_New(0)))
	ଫେରନ୍ତୁ ଖାଲି;

    do_in_runtimepath(ଖାଲି, 0, &map_finder_callback, ret);

    ଯଦି (PyErr_Occurred())
    {
	Py_DECREF(ret);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ଫେରନ୍ତୁ ret;
}

#ଯଦି PY_VERSION_HEX >= 0x030700f0
    ସ୍ଥିର PyObject *
FinderFindSpec(PyObject *self, PyObject *args)
{
    ଅକ୍ଷର	*fullname;
    PyObject	*paths;
    PyObject	*target = Py_None;
    PyObject	*spec;

    ଯଦି (!PyArg_ParseTuple(args, "s|O", &fullname, &target))
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (!(paths = Vim_GetPaths(self, ଖାଲି)))
	ଫେରନ୍ତୁ ଖାଲି;

    spec = PyObject_CallFunction(py_find_spec, "sOO", fullname, paths, target);

    Py_DECREF(paths);

    ଯଦି (!spec)
    {
	ଯଦି (PyErr_Occurred())
	    ଫେରନ୍ତୁ ଖାଲି;

	Py_INCREF(Py_None);
	ଫେରନ୍ତୁ Py_None;
    }

    ଫେରନ୍ତୁ spec;
}

    ସ୍ଥିର PyObject *
FinderFindModule(PyObject* self UNUSED, PyObject* args UNUSED)
{
    // Apparently returning None works.
    Py_INCREF(Py_None);
    ଫେରନ୍ତୁ Py_None;
}
#ଅନ୍ୟଥା
    ସ୍ଥିର PyObject *
call_load_module(ଅକ୍ଷର *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len, PyObject *find_module_result)
{
    PyObject	*fd, *pathname, *description;

    ଯଦି (!PyTuple_Check(find_module_result))
    {
	PyErr_FORMAT_TYPE(
		N_("expected 3-tuple as imp.find_module() result, but got %s"),
		find_module_result);
	ଫେରନ୍ତୁ ଖାଲି;
    }
    ଯଦି (PyTuple_GET_SIZE(find_module_result) != 3)
    {
	PyErr_FORMAT(PyExc_TypeError,
		N_("expected 3-tuple as imp.find_module() result, but got "
		   "tuple of size %d"),
		(ପୂର୍ଣ୍ଣସଂଖ୍ୟା) PyTuple_GET_SIZE(find_module_result));
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ଯଦି (!(fd = PyTuple_GET_ITEM(find_module_result, 0))
	    || !(pathname = PyTuple_GET_ITEM(find_module_result, 1))
	    || !(description = PyTuple_GET_ITEM(find_module_result, 2)))
    {
	PyErr_SET_STRING(PyExc_RuntimeError,
		N_("internal error: imp.find_module returned tuple with NULL"));
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ଫେରନ୍ତୁ PyObject_CallFunction(py_load_module,
	    "s#OOO", name, len, fd, pathname, description);
}

    ସ୍ଥିର PyObject *
find_module(ଅକ୍ଷର *fullname, ଅକ୍ଷର *tail, PyObject *new_path)
{
    PyObject	*find_module_result;
    PyObject	*module;
    ଅକ୍ଷର	*dot;

    ଯଦି ((dot = (ଅକ୍ଷର *)vim_strchr((char_u *) tail, '.')))
    {
	/*
	 * There is a dot in the name: call find_module recursively without the
	 * first component
	 */
	PyObject	*newest_path;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		partlen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) (dot - 1 - tail);

	ଯଦି (!(find_module_result = PyObject_CallFunction(py_find_module,
			"s#O", tail, partlen, new_path)))
	{
	    ଯଦି (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_ImportError))
		PyErr_Clear();
	    ଫେରନ୍ତୁ ଖାଲି;
	}

	ଯଦି (!(module = call_load_module(
			fullname,
			((ପୂର୍ଣ୍ଣସଂଖ୍ୟା) (tail - fullname)) + partlen,
			find_module_result)))
	{
	    Py_DECREF(find_module_result);
	    ଫେରନ୍ତୁ ଖାଲି;
	}

	Py_DECREF(find_module_result);

	ଯଦି (!(newest_path = PyObject_GetAttrString(module, "__path__")))
	{
	    Py_DECREF(module);
	    ଫେରନ୍ତୁ ଖାଲି;
	}

	Py_DECREF(module);

	find_module_result = find_module(fullname, dot + 1, newest_path);

	Py_DECREF(newest_path);

	ଫେରନ୍ତୁ find_module_result;
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (!(find_module_result = PyObject_CallFunction(py_find_module,
			"sO", tail, new_path)))
	{
	    ଯଦି (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_ImportError))
		PyErr_Clear();
	    ଫେରନ୍ତୁ ଖାଲି;
	}

	ଫେରନ୍ତୁ find_module_result;
    }
}

    ସ୍ଥିର PyObject *
FinderFindModule(PyObject *self, PyObject *args)
{
    ଅକ୍ଷର	*fullname;
    PyObject	*result;
    PyObject	*new_path;
    LoaderObject	*loader;

    ଯଦି (!PyArg_ParseTuple(args, "s", &fullname))
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (!(new_path = Vim_GetPaths(self, ଖାଲି)))
	ଫେରନ୍ତୁ ଖାଲି;

    result = find_module(fullname, fullname, new_path);

    Py_DECREF(new_path);

    ଯଦି (!result)
    {
	ଯଦି (PyErr_Occurred())
	    ଫେରନ୍ତୁ ଖାଲି;

	Py_INCREF(Py_None);
	ଫେରନ୍ତୁ Py_None;
    }

    ଯଦି (!(fullname = (ଅକ୍ଷର *)vim_strsave((char_u *)fullname)))
    {
	Py_DECREF(result);
	PyErr_NoMemory();
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ଯଦି (!(loader = PyObject_NEW(LoaderObject, LoaderTypePtr)))
    {
	vim_free(fullname);
	Py_DECREF(result);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    loader->fullname = fullname;
    loader->result = result;

    ଫେରନ୍ତୁ (PyObject *) loader;
}
#endif

    ସ୍ଥିର PyObject *
VimPathHook(PyObject *self UNUSED, PyObject *args)
{
    ଅକ୍ଷର	*path;

    ଯଦି (PyArg_ParseTuple(args, "s", &path)
	    && STRCMP(path, vim_special_path) == 0)
    {
	Py_INCREF(vim_module);
	ଫେରନ୍ତୁ vim_module;
    }

    PyErr_Clear();
    PyErr_SetNone(PyExc_ImportError);
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Vim module - Definitions
 */

ସ୍ଥିର ସଂରଚନା PyMethodDef VimMethods[] = {
    // name,	    function,			calling,			documentation
    {"command",	    VimCommand,			METH_O,				"Execute a Vim ex-mode command" },
    {"eval",	    VimEval,			METH_VARARGS,			"Evaluate an expression using Vim evaluator" },
    {"bindeval",    VimEvalPy,			METH_O,				"Like eval(), but returns objects attached to Vim ones"},
    {"strwidth",    VimStrwidth,		METH_O,				"Screen string width, counts <Tab> as having width 1"},
    {"chdir",	    (PyCFunction)(ଶୂନ୍ୟ *)VimChdir,	METH_VARARGS|METH_KEYWORDS,	"Change directory"},
    {"fchdir",	    (PyCFunction)(ଶୂନ୍ୟ *)VimFchdir,	METH_VARARGS|METH_KEYWORDS,	"Change directory"},
    {"foreach_rtp", VimForeachRTP,		METH_O,				"Call given callable for each path in &rtp"},
#ଯଦି PY_VERSION_HEX >= 0x030700f0
    {"find_spec",   FinderFindSpec,		METH_VARARGS,			"Internal use only, returns spec object for any input it receives"},
#endif
    {"find_module", FinderFindModule,		METH_VARARGS,			"Internal use only, returns loader object for any input it receives"},
    {"path_hook",   VimPathHook,		METH_VARARGS,			"Hook function to install in sys.path_hooks"},
    {"_get_paths",  (PyCFunction)Vim_GetPaths,	METH_NOARGS,			"Get &rtp-based additions to sys.path"},
    { ଖାଲି,	    ଖାଲି,			0,				ଖାଲି}
};

/*
 * Generic iterator object
 */

DEFINE_PY_TYPE_OBJECT(IterType);

ପ୍ରକାରବ୍ୟାଖ୍ୟା PyObject *(*nextfun)(ଶୂନ୍ୟ **);
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଶୂନ୍ୟ (*destructorfun)(ଶୂନ୍ୟ *);
ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*traversefun)(ଶୂନ୍ୟ *, visitproc, ଶୂନ୍ୟ *);
ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*clearfun)(ଶୂନ୍ୟ **);

// Main purpose of this object is removing the need for do python
// initialization (i.e. PyType_Ready and setting type attributes) for a big
// bunch of objects.
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    PyObject_HEAD
    ଶୂନ୍ୟ *cur;
    nextfun next;
    destructorfun destruct;
    traversefun traverse;
    clearfun clear;
    PyObject *iter_object;
} IterObject;

    ସ୍ଥିର PyObject *
IterNew(ଶୂନ୍ୟ *start, destructorfun destruct, nextfun next, traversefun traverse,
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

    ଯଦି (iter_object)
	Py_INCREF(iter_object);

    ଫେରନ୍ତୁ (PyObject *)(self);
}

    ସ୍ଥିର ଶୂନ୍ୟ
IterDestructor(PyObject *self_obj)
{
    IterObject *self = (IterObject*)self_obj;
    ଯଦି (self->iter_object)
	Py_DECREF(self->iter_object);
    PyObject_GC_UnTrack((ଶୂନ୍ୟ *)(self));
    self->destruct(self->cur);
    PyObject_GC_Del((ଶୂନ୍ୟ *)(self));
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
IterTraverse(PyObject *self_obj, visitproc visit, ଶୂନ୍ୟ *arg)
{
    IterObject *self = (IterObject*)self_obj;
    ଯଦି (self->traverse != ଖାଲି)
	ଫେରନ୍ତୁ self->traverse(self->cur, visit, arg);
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ 0;
}

// Mac OSX defines clear() somewhere.
#ifdef clear
# undef clear
#endif

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
IterClear(PyObject *self_obj)
{
    IterObject *self = (IterObject*)self_obj;
    ଯଦି (self->clear != ଖାଲି)
	ଫେରନ୍ତୁ self->clear(&self->cur);
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ 0;
}

    ସ୍ଥିର PyObject *
IterNext(PyObject *self_obj)
{
    IterObject *self = (IterObject*)self_obj;
    ଫେରନ୍ତୁ self->next(&self->cur);
}

    ସ୍ଥିର PyObject *
IterIter(PyObject *self)
{
    Py_INCREF(self);
    ଫେରନ୍ତୁ self;
}

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା pylinkedlist_S {
    ସଂରଚନା pylinkedlist_S	*pll_next;
    ସଂରଚନା pylinkedlist_S	*pll_prev;
    PyObject			*pll_obj;
} pylinkedlist_T;

ସ୍ଥିର pylinkedlist_T *lastdict = ଖାଲି;
ସ୍ଥିର pylinkedlist_T *lastlist = ଖାଲି;
ସ୍ଥିର pylinkedlist_T *lasttuple = ଖାଲି;
ସ୍ଥିର pylinkedlist_T *lastfunc = ଖାଲି;

    ସ୍ଥିର ଶୂନ୍ୟ
pyll_remove(pylinkedlist_T *ref, pylinkedlist_T **last)
{
    ଯଦି (ref->pll_prev == ଖାଲି)
    {
	ଯଦି (ref->pll_next == ଖାଲି)
	{
	    *last = ଖାଲି;
	    ଫେରନ୍ତୁ;
	}
    }
    ଅନ୍ୟଥା
	ref->pll_prev->pll_next = ref->pll_next;

    ଯଦି (ref->pll_next == ଖାଲି)
	*last = ref->pll_prev;
    ଅନ୍ୟଥା
	ref->pll_next->pll_prev = ref->pll_prev;
}

    ସ୍ଥିର ଶୂନ୍ୟ
pyll_add(PyObject *self, pylinkedlist_T *ref, pylinkedlist_T **last)
{
    ଯଦି (*last == ଖାଲି)
	ref->pll_prev = ଖାଲି;
    ଅନ୍ୟଥା
    {
	(*last)->pll_next = ref;
	ref->pll_prev = *last;
    }
    ref->pll_next = ଖାଲି;
    ref->pll_obj = self;
    *last = ref;
}

DEFINE_PY_TYPE_OBJECT(DictionaryType);

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    PyObject_HEAD
    dict_T	*dict;
    pylinkedlist_T	ref;
} DictionaryObject;

ସ୍ଥିର PyObject *DictionaryUpdate(DictionaryObject *, PyObject *, PyObject *);

#define NEW_DICTIONARY(dict) DictionaryNew(DictionaryTypePtr, dict)

    ସ୍ଥିର PyObject *
DictionaryNew(PyTypeObject *subtype, dict_T *dict)
{
    DictionaryObject	*self;

    self = (DictionaryObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    ଯଦି (self == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    self->dict = dict;
    ++dict->dv_refcount;

    pyll_add((PyObject *)(self), &self->ref, &lastdict);

    ଫେରନ୍ତୁ (PyObject *)(self);
}

    ସ୍ଥିର dict_T *
py_dict_alloc(ଶୂନ୍ୟ)
{
    dict_T	*ret;

    ଯଦି (!(ret = dict_alloc()))
    {
	PyErr_NoMemory();
	ଫେରନ୍ତୁ ଖାଲି;
    }
    ++ret->dv_refcount;

    ଫେରନ୍ତୁ ret;
}

    ସ୍ଥିର PyObject *
DictionaryConstructor(PyTypeObject *subtype, PyObject *args, PyObject *kwargs)
{
    DictionaryObject	*self;
    dict_T	*dict;

    ଯଦି (!(dict = py_dict_alloc()))
	ଫେରନ୍ତୁ ଖାଲି;

    self = (DictionaryObject *) DictionaryNew(subtype, dict);

    --dict->dv_refcount;

    ଯଦି (kwargs || PyTuple_Size(args))
    {
	PyObject	*tmp;
	ଯଦି (!(tmp = DictionaryUpdate(self, args, kwargs)))
	{
	    Py_DECREF(self);
	    ଫେରନ୍ତୁ ଖାଲି;
	}

	Py_DECREF(tmp);
    }

    ଫେରନ୍ତୁ (PyObject *)(self);
}

    ସ୍ଥିର ଶୂନ୍ୟ
DictionaryDestructor(PyObject *self_obj)
{
    DictionaryObject *self = (DictionaryObject*)self_obj;
    pyll_remove(&self->ref, &lastdict);
    dict_unref(self->dict);

    DESTRUCTOR_FINISH(self);
}

ସ୍ଥିର ଅକ୍ଷର *DictionaryAttrs[] = {
    "locked", "scope",
    ଖାଲି
};

    ସ୍ଥିର PyObject *
DictionaryDir(PyObject *self, PyObject *args UNUSED)
{
    ଫେରନ୍ତୁ ObjectDir(self, DictionaryAttrs);
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
DictionarySetattr(PyObject *self_obj, ଅକ୍ଷର *name, PyObject *valObject)
{
    DictionaryObject *self = (DictionaryObject*)self_obj;
    ଯଦି (valObject == ଖାଲି)
    {
	PyErr_SET_STRING(PyExc_AttributeError,
		N_("cannot delete vim.Dictionary attributes"));
	ଫେରନ୍ତୁ -1;
    }

    ଯଦି (strcmp(name, "locked") == 0)
    {
	ଯଦି (self->dict->dv_lock == VAR_FIXED)
	{
	    PyErr_SET_STRING(PyExc_TypeError,
		    N_("cannot modify fixed dictionary"));
	    ଫେରନ୍ତୁ -1;
	}
	ଅନ୍ୟଥା
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		istrue = PyObject_IsTrue(valObject);
	    ଯଦି (istrue == -1)
		ଫେରନ୍ତୁ -1;
	    ଅନ୍ୟଥା ଯଦି (istrue)
		self->dict->dv_lock = VAR_LOCKED;
	    ଅନ୍ୟଥା
		self->dict->dv_lock = 0;
	}
	ଫେରନ୍ତୁ 0;
    }
    ଅନ୍ୟଥା
    {
	PyErr_FORMAT(PyExc_AttributeError, N_("cannot set attribute %s"), name);
	ଫେରନ୍ତୁ -1;
    }
}

    ସ୍ଥିର PyInt
DictionaryLength(DictionaryObject *self)
{
    ଫେରନ୍ତୁ ((PyInt) (self->dict->dv_hashtab.ht_used));
}

#define DICT_FLAG_HAS_DEFAULT	0x01
#define DICT_FLAG_କାଢ଼		0x02
#define DICT_FLAG_NONE_DEFAULT	0x04
#define DICT_FLAG_RETURN_BOOL	0x08 // Incompatible with DICT_FLAG_POP
#define DICT_FLAG_RETURN_PAIR	0x10

    ସ୍ଥିର PyObject *
_DictionaryItem(DictionaryObject *self, PyObject *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
    PyObject	*ଚାବି_Object;
    PyObject	*defObject = ((flags & DICT_FLAG_NONE_DEFAULT)? Py_None : ଖାଲି);
    PyObject	*ret;
    char_u	*ଚାବି;
    dictitem_T	*di;
    dict_T	*dict = self->dict;
    hashitem_T	*hi;
    PyObject	*todecref;

    ଯଦି (flags & DICT_FLAG_HAS_DEFAULT)
    {
	ଯଦି (!PyArg_ParseTuple(args, "O|O", &ଚାବି_Object, &defObject))
	    ଫେରନ୍ତୁ ଖାଲି;
    }
    ଅନ୍ୟଥା
	ଚାବି_Object = args;

    ଯଦି (flags & DICT_FLAG_RETURN_BOOL)
	defObject = Py_False;

    ଯଦି (!(ଚାବି = StringToChars(ଚାବି_Object, &todecref)))
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (*ଚାବି == NUL)
    {
	RAISE_NO_EMPTY_ଚାବିଗୁଡ଼ିକ;
	Py_XDECREF(todecref);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    hi = hash_find(&dict->dv_hashtab, ଚାବି);

    Py_XDECREF(todecref);

    ଯଦି (HASHITEM_EMPTY(hi))
    {
	ଯଦି (defObject)
	{
	    Py_INCREF(defObject);
	    ଫେରନ୍ତୁ defObject;
	}
	ଅନ୍ୟଥା
	{
	    PyErr_SetObject(Py_Exc_ଚାବି_Error, ଚାବି_Object);
	    ଫେରନ୍ତୁ ଖାଲି;
	}
    }
    ଅନ୍ୟଥା ଯଦି (flags & DICT_FLAG_RETURN_BOOL)
    {
	ret = Py_True;
	Py_INCREF(ret);
	ଫେରନ୍ତୁ ret;
    }

    di = dict_lookup(hi);

    ଯଦି (!(ret = ConvertToPyObject(&di->di_tv)))
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (flags & DICT_FLAG_କାଢ଼)
    {
	ଯଦି (dict->dv_lock)
	{
	    RAISE_LOCKED_DICTIONARY;
	    Py_DECREF(ret);
	    ଫେରନ୍ତୁ ଖାଲି;
	}

	hash_remove(&dict->dv_hashtab, hi, "Python remove variable");
	dictitem_free(di);
    }

    ଫେରନ୍ତୁ ret;
}

    ସ୍ଥିର PyObject *
DictionaryItem(DictionaryObject *self, PyObject *ଚାବି_Object)
{
    ଫେରନ୍ତୁ _DictionaryItem(self, ଚାବି_Object, 0);
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
DictionaryContains(DictionaryObject *self, PyObject *ଚାବି_Object)
{
    PyObject	*rObj = _DictionaryItem(self, ଚାବି_Object, DICT_FLAG_RETURN_BOOL);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret;

    ଯଦି (rObj == ଖାଲି)
	ଫେରନ୍ତୁ -1;

    ret = (rObj == Py_True);

    Py_DECREF(rObj);

    ଫେରନ୍ତୁ ret;
}

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dii_changed;
    hashtab_T	*dii_ht;
    hashitem_T	*dii_hi;
    long_u	dii_todo;
} dictiterinfo_T;

    ସ୍ଥିର PyObject *
DictionaryIterNext(ଶୂନ୍ୟ **arg)
{
    PyObject	*ret;
    dictiterinfo_T **dii = (dictiterinfo_T**)arg;

    ଯଦି (!(*dii)->dii_todo)
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି ((*dii)->dii_ht->ht_changed != (*dii)->dii_changed)
    {
	PyErr_SET_STRING(PyExc_RuntimeError,
		N_("hashtab changed during iteration"));
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ଯେତେବେଳେ (((*dii)->dii_todo) && HASHITEM_EMPTY((*dii)->dii_hi))
	++((*dii)->dii_hi);

    --((*dii)->dii_todo);

    ଯଦି (!(ret = PyBytes_FromString((ଅକ୍ଷର *)(*dii)->dii_hi->hi_ଚାବି)))
	ଫେରନ୍ତୁ ଖାଲି;

    ଫେରନ୍ତୁ ret;
}

    ସ୍ଥିର PyObject *
DictionaryIter(PyObject *self_obj)
{
    DictionaryObject	*self = (DictionaryObject*)self_obj;
    dictiterinfo_T	*dii;
    hashtab_T		*ht;

    ଯଦି (!(dii = PyMem_New(dictiterinfo_T, 1)))
    {
	PyErr_NoMemory();
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ht = &self->dict->dv_hashtab;
    dii->dii_changed = ht->ht_changed;
    dii->dii_ht = ht;
    dii->dii_hi = ht->ht_array;
    dii->dii_todo = ht->ht_used;

    ଫେରନ୍ତୁ IterNew(dii,
	    PyMem_Free, DictionaryIterNext,
	    ଖାଲି, ଖାଲି, (PyObject *)self);
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
DictionaryAssItem(
	DictionaryObject *self, PyObject *ଚାବି_Object, PyObject *valObject)
{
    char_u	*ଚାବି;
    typval_T	tv;
    dict_T	*dict = self->dict;
    dictitem_T	*di;
    PyObject	*todecref;

    ଯଦି (dict->dv_lock)
    {
	RAISE_LOCKED_DICTIONARY;
	ଫେରନ୍ତୁ -1;
    }

    ଯଦି (!(ଚାବି = StringToChars(ଚାବି_Object, &todecref)))
	ଫେରନ୍ତୁ -1;

    ଯଦି (*ଚାବି == NUL)
    {
	RAISE_NO_EMPTY_ଚାବିଗୁଡ଼ିକ;
	Py_XDECREF(todecref);
	ଫେରନ୍ତୁ -1;
    }

    di = dict_find(dict, ଚାବି, -1);

    ଯଦି (valObject == ଖାଲି)
    {
	hashitem_T	*hi;

	ଯଦି (di == ଖାଲି)
	{
	    Py_XDECREF(todecref);
	    PyErr_SetObject(Py_Exc_ଚାବି_Error, ଚାବି_Object);
	    ଫେରନ୍ତୁ -1;
	}
	hi = hash_find(&dict->dv_hashtab, di->di_ଚାବି);
	hash_remove(&dict->dv_hashtab, hi, "Python remove item");
	dictitem_free(di);
	Py_XDECREF(todecref);
	ଫେରନ୍ତୁ 0;
    }

    ଯଦି (ConvertFromPyObject(valObject, &tv) == -1)
    {
	Py_XDECREF(todecref);
	ଫେରନ୍ତୁ -1;
    }

    ଯଦି (di == ଖାଲି)
    {
	ଯଦି (!(di = dictitem_alloc(ଚାବି)))
	{
	    Py_XDECREF(todecref);
	    PyErr_NoMemory();
	    ଫେରନ୍ତୁ -1;
	}
	di->di_tv.v_type = VAR_UNKNOWN;

	ଯଦି (dict_add(dict, di) == FAIL)
	{
	    dictitem_free(di);
	    RAISE_ଚାବି_ADD_FAIL(ଚାବି);
	    Py_XDECREF(todecref);
	    ଫେରନ୍ତୁ -1;
	}
    }
    ଅନ୍ୟଥା
	clear_tv(&di->di_tv);

    Py_XDECREF(todecref);

    copy_tv(&tv, &di->di_tv);
    clear_tv(&tv);
    ଫେରନ୍ତୁ 0;
}

ପ୍ରକାରବ୍ୟାଖ୍ୟା PyObject *(*hi_to_py)(hashitem_T *);

    ସ୍ଥିର PyObject *
DictionaryListObjects(DictionaryObject *self, hi_to_py hiconvert)
{
    dict_T	*dict = self->dict;
    long_u	todo = dict->dv_hashtab.ht_used;
    Py_ssize_t	i = 0;
    PyObject	*ret;
    hashitem_T	*hi;
    PyObject	*newObj;

    ret = PyList_New(todo);
    ପାଇଁ (hi = dict->dv_hashtab.ht_array; todo > 0; ++hi)
    {
	ଯଦି (!HASHITEM_EMPTY(hi))
	{
	    ଯଦି (!(newObj = hiconvert(hi)))
	    {
		Py_DECREF(ret);
		ଫେରନ୍ତୁ ଖାଲି;
	    }
	    PyList_SET_ITEM(ret, i, newObj);
	    --todo;
	    ++i;
	}
    }
    ଫେରନ୍ତୁ ret;
}

    ସ୍ଥିର PyObject *
dict_ଚାବି(hashitem_T *hi)
{
    ଫେରନ୍ତୁ PyBytes_FromString((ଅକ୍ଷର *)(hi->hi_ଚାବି));
}

    ସ୍ଥିର PyObject *
Dictionary_List_ଚାବିଗୁଡ଼ିକ(DictionaryObject *self, PyObject *args UNUSED)
{
    ଫେରନ୍ତୁ DictionaryListObjects(self, dict_ଚାବି);
}

    ସ୍ଥିର PyObject *
dict_val(hashitem_T *hi)
{
    dictitem_T	*di;

    di = dict_lookup(hi);
    ଫେରନ୍ତୁ ConvertToPyObject(&di->di_tv);
}

    ସ୍ଥିର PyObject *
Dictionary_List_ମୂଲ୍ୟଗୁଡ଼ିକ(DictionaryObject *self, PyObject *args UNUSED)
{
    ଫେରନ୍ତୁ DictionaryListObjects(self, dict_val);
}

    ସ୍ଥିର PyObject *
dict_item(hashitem_T *hi)
{
    PyObject	*ଚାବି_Object;
    PyObject	*valObject;
    PyObject	*ret;

    ଯଦି (!(ଚାବି_Object = dict_ଚାବି(hi)))
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (!(valObject = dict_val(hi)))
    {
	Py_DECREF(ଚାବି_Object);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ret = Py_Build_ମୂଲ୍ୟ("(OO)", ଚାବି_Object, valObject);

    Py_DECREF(ଚାବି_Object);
    Py_DECREF(valObject);

    ଫେରନ୍ତୁ ret;
}

    ସ୍ଥିର PyObject *
Dictionary_List_ବସ୍ତୁଗୁଡ଼ିକ(DictionaryObject *self, PyObject *args UNUSED)
{
    ଫେରନ୍ତୁ DictionaryListObjects(self, dict_item);
}

    ସ୍ଥିର PyObject *
DictionaryUpdate(DictionaryObject *self, PyObject *args, PyObject *kwargs)
{
    dict_T	*dict = self->dict;

    ଯଦି (dict->dv_lock)
    {
	RAISE_LOCKED_DICTIONARY;
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ଯଦି (kwargs)
    {
	typval_T	tv;

	ଯଦି (ConvertFromPyMapping(kwargs, &tv) == -1)
	    ଫେରନ୍ତୁ ଖାଲି;

	VimTryStart();
	dict_extend(self->dict, tv.vval.v_dict, (char_u *) "force", ଖାଲି);
	clear_tv(&tv);
	ଯଦି (VimTryEnd())
	    ଫେରନ୍ତୁ ଖାଲି;
    }
    ଅନ୍ୟଥା
    {
	PyObject	*obj = ଖାଲି;

	ଯଦି (!PyArg_ParseTuple(args, "|O", &obj))
	    ଫେରନ୍ତୁ ଖାଲି;

	ଯଦି (obj == ଖାଲି)
	{
	    Py_INCREF(Py_None);
	    ଫେରନ୍ତୁ Py_None;
	}

	ଯଦି (PyObject_HasAttrString(obj, "keys"))
	    ଫେରନ୍ତୁ DictionaryUpdate(self, ଖାଲି, obj);
	ଅନ୍ୟଥା
	{
	    PyObject	*iterator;
	    PyObject	*item;

	    ଯଦି (!(iterator = PyObject_GetIter(obj)))
		ଫେରନ୍ତୁ ଖାଲି;

	    ଯେତେବେଳେ ((item = PyIter_Next(iterator)))
	    {
		PyObject	*fast;
		PyObject	*ଚାବି_Object;
		PyObject	*valObject;
		PyObject	*todecref;
		char_u		*ଚାବି;
		dictitem_T	*di;
		hashitem_T	*hi;

		ଯଦି (!(fast = PySequence_Fast(item, "")))
		{
		    Py_DECREF(iterator);
		    Py_DECREF(item);
		    ଫେରନ୍ତୁ ଖାଲି;
		}

		Py_DECREF(item);

		ଯଦି (PySequence_Size(fast) != 2)
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    PyErr_FORMAT(Py_Exc_ମୂଲ୍ୟ_Error,
			    N_("expected sequence element of size 2, "
			    "but got sequence of size %d"),
			    (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) PySequence_Size(fast));
		    ଫେରନ୍ତୁ ଖାଲି;
		}

		ଚାବି_Object = PySequence_GetItem(fast, 0);

		ଯଦି (!(ଚାବି = StringToChars(ଚାବି_Object, &todecref)))
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    ଫେରନ୍ତୁ ଖାଲି;
		}

		di = dictitem_alloc(ଚାବି);

		Py_XDECREF(todecref);

		ଯଦି (di == ଖାଲି)
		{
		    Py_DECREF(fast);
		    Py_DECREF(iterator);
		    PyErr_NoMemory();
		    ଫେରନ୍ତୁ ଖାଲି;
		}
		di->di_tv.v_type = VAR_UNKNOWN;

		valObject = PySequence_GetItem(fast, 1);

		ଯଦି (ConvertFromPyObject(valObject, &di->di_tv) == -1)
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    dictitem_free(di);
		    ଫେରନ୍ତୁ ଖାଲି;
		}

		Py_DECREF(fast);

		hi = hash_find(&dict->dv_hashtab, di->di_ଚାବି);
		ଯଦି (!HASHITEM_EMPTY(hi) || dict_add(dict, di) == FAIL)
		{
		    RAISE_ଚାବି_ADD_FAIL(di->di_ଚାବି);
		    Py_DECREF(iterator);
		    dictitem_free(di);
		    ଫେରନ୍ତୁ ଖାଲି;
		}
	    }

	    Py_DECREF(iterator);

	    // Iterator may have finished due to an exception
	    ଯଦି (PyErr_Occurred())
		ଫେରନ୍ତୁ ଖାଲି;
	}
    }
    Py_INCREF(Py_None);
    ଫେରନ୍ତୁ Py_None;
}

    ସ୍ଥିର PyObject *
DictionaryGet(DictionaryObject *self, PyObject *args)
{
    ଫେରନ୍ତୁ _DictionaryItem(self, args,
			    DICT_FLAG_HAS_DEFAULT|DICT_FLAG_NONE_DEFAULT);
}

    ସ୍ଥିର PyObject *
Dictionary_କାଢ଼(DictionaryObject *self, PyObject *args)
{
    ଫେରନ୍ତୁ _DictionaryItem(self, args, DICT_FLAG_HAS_DEFAULT|DICT_FLAG_କାଢ଼);
}

    ସ୍ଥିର PyObject *
Dictionary_କାଢ଼_Item(DictionaryObject *self, PyObject *args UNUSED)
{
    hashitem_T	*hi;
    PyObject	*ret;
    PyObject	*valObject;
    dictitem_T	*di;

    ଯଦି (self->dict->dv_hashtab.ht_used == 0)
    {
	PyErr_SetNone(Py_Exc_ଚାବି_Error);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    hi = self->dict->dv_hashtab.ht_array;
    ଯେତେବେଳେ (HASHITEM_EMPTY(hi))
	++hi;

    di = dict_lookup(hi);

    ଯଦି (!(valObject = ConvertToPyObject(&di->di_tv)))
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (!(ret = Py_Build_ମୂଲ୍ୟ("(" Py_bytes_fmt "O)", hi->hi_ଚାବି, valObject)))
    {
	Py_DECREF(valObject);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    hash_remove(&self->dict->dv_hashtab, hi, "Python pop item");
    dictitem_free(di);

    ଫେରନ୍ତୁ ret;
}

    ସ୍ଥିର PyObject *
Dictionary_Has_ଚାବି(DictionaryObject *self, PyObject *ଚାବି_Object)
{
    ଫେରନ୍ତୁ _DictionaryItem(self, ଚାବି_Object, DICT_FLAG_RETURN_BOOL);
}

ସ୍ଥିର PySequenceMethods DictionaryAsSeq = {
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

ସ୍ଥିର PyMappingMethods DictionaryAsMapping = {
    (lenfunc)       DictionaryLength,
    (binaryfunc)    DictionaryItem,
    (objobjargproc) DictionaryAssItem,
};

ସ୍ଥିର ସଂରଚନା PyMethodDef DictionaryMethods[] = {
    {"keys",	(PyCFunction)Dictionary_List_ଚାବିଗୁଡ଼ିକ,	METH_NOARGS,	""},
    {"values",	(PyCFunction)Dictionary_List_ମୂଲ୍ୟଗୁଡ଼ିକ,	METH_NOARGS,	""},
    {"items",	(PyCFunction)Dictionary_List_ବସ୍ତୁଗୁଡ଼ିକ,	METH_NOARGS,	""},
    {"update",	(PyCFunction)(ଶୂନ୍ୟ *)DictionaryUpdate,		METH_VARARGS|METH_KEYWORDS, ""},
    {"get",	(PyCFunction)DictionaryGet,		METH_VARARGS,	""},
    {"pop",	(PyCFunction)Dictionary_କାଢ଼,		METH_VARARGS,	""},
    {"popitem",	(PyCFunction)Dictionary_କାଢ଼_Item,		METH_NOARGS,	""},
    {"has_key",	(PyCFunction)Dictionary_Has_ଚାବି,		METH_O,		""},
    {"__dir__",	(PyCFunction)DictionaryDir,		METH_NOARGS,	""},
    { ଖାଲି,	ଖାଲି,					0,		ଖାଲି}
};

DEFINE_PY_TYPE_OBJECT(ListType);

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    PyObject_HEAD
    list_T	*list;
    pylinkedlist_T	ref;
} ListObject;

#define NEW_LIST(list) ListNew(ListTypePtr, list)

    ସ୍ଥିର PyObject *
ListNew(PyTypeObject *subtype, list_T *list)
{
    ListObject	*self;

    ଯଦି (list == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    self = (ListObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    ଯଦି (self == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    self->list = list;
    ++list->lv_refcount;
    CHECK_LIST_MATERIALIZE(list);

    pyll_add((PyObject *)(self), &self->ref, &lastlist);

    ଫେରନ୍ତୁ (PyObject *)(self);
}

    ସ୍ଥିର list_T *
py_list_alloc(ଶୂନ୍ୟ)
{
    list_T	*ret;

    ଯଦି (!(ret = list_alloc()))
    {
	PyErr_NoMemory();
	ଫେରନ୍ତୁ ଖାଲି;
    }
    ++ret->lv_refcount;

    ଫେରନ୍ତୁ ret;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
list_py_concat(list_T *l, PyObject *obj, PyObject *lookup_dict)
{
    PyObject	*iterator;
    PyObject	*item;
    listitem_T	*li;

    ଯଦି (!(iterator = PyObject_GetIter(obj)))
	ଫେରନ୍ତୁ -1;

    ଯେତେବେଳେ ((item = PyIter_Next(iterator)))
    {
	ଯଦି (!(li = listitem_alloc()))
	{
	    PyErr_NoMemory();
	    Py_DECREF(item);
	    Py_DECREF(iterator);
	    ଫେରନ୍ତୁ -1;
	}
	li->li_tv.v_lock = 0;
	li->li_tv.v_type = VAR_UNKNOWN;

	ଯଦି (_ConvertFromPyObject(item, &li->li_tv, lookup_dict) == -1)
	{
	    Py_DECREF(item);
	    Py_DECREF(iterator);
	    listitem_free(l, li);
	    ଫେରନ୍ତୁ -1;
	}

	Py_DECREF(item);

	list_append(l, li);
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    ଯଦି (PyErr_Occurred())
	ଫେରନ୍ତୁ -1;

    ଫେରନ୍ତୁ 0;
}

    ସ୍ଥିର PyObject *
ListConstructor(PyTypeObject *subtype, PyObject *args, PyObject *kwargs)
{
    list_T	*list;
    PyObject	*obj = ଖାଲି;

    ଯଦି (kwargs)
    {
	PyErr_SET_STRING(PyExc_TypeError,
		N_("list constructor does not accept keyword arguments"));
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ଯଦି (!PyArg_ParseTuple(args, "|O", &obj))
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (!(list = py_list_alloc()))
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (obj)
    {
	PyObject	*lookup_dict;

	ଯଦି (!(lookup_dict = PyDict_New()))
	{
	    list_unref(list);
	    ଫେରନ୍ତୁ ଖାଲି;
	}

	ଯଦି (list_py_concat(list, obj, lookup_dict) == -1)
	{
	    Py_DECREF(lookup_dict);
	    list_unref(list);
	    ଫେରନ୍ତୁ ଖାଲି;
	}

	Py_DECREF(lookup_dict);
    }

    ଫେରନ୍ତୁ ListNew(subtype, list);
}

    ସ୍ଥିର ଶୂନ୍ୟ
ListDestructor(PyObject *self_obj)
{
    ListObject *self = (ListObject*)self_obj;
    pyll_remove(&self->ref, &lastlist);
    list_unref(self->list);

    DESTRUCTOR_FINISH(self);
}

    ସ୍ଥିର PyInt
ListLength(ListObject *self)
{
    ଫେରନ୍ତୁ ((PyInt) (self->list->lv_len));
}

    ସ୍ଥିର PyObject *
ListIndex(ListObject *self, Py_ssize_t index)
{
    listitem_T	*li;

    ଯଦି (index >= ListLength(self))
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("list index out of range"));
	ଫେରନ୍ତୁ ଖାଲି;
    }
    li = list_find(self->list, (long) index);
    ଯଦି (li == ଖାଲି)
    {
	// No more suitable format specifications in python-2.3
	PyErr_VIM_FORMAT(N_("internal error: failed to get Vim list item %d"),
		(ପୂର୍ଣ୍ଣସଂଖ୍ୟା) index);
	ଫେରନ୍ତୁ ଖାଲି;
    }
    ଫେରନ୍ତୁ ConvertToPyObject(&li->li_tv);
}

    ସ୍ଥିର PyObject *
ListSlice(ListObject *self, Py_ssize_t first, Py_ssize_t step,
	  Py_ssize_t slicelen)
{
    PyInt	i;
    PyObject	*list;

    ଯଦି (step == 0)
    {
	PyErr_SET_STRING(Py_Exc_ମୂଲ୍ୟ_Error, N_("slice step cannot be zero"));
	ଫେରନ୍ତୁ ଖାଲି;
    }

    list = PyList_New(slicelen);
    ଯଦି (list == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ପାଇଁ (i = 0; i < slicelen; ++i)
    {
	PyObject	*item;

	item = ListIndex(self, first + i*step);
	ଯଦି (item == ଖାଲି)
	{
	    Py_DECREF(list);
	    ଫେରନ୍ତୁ ଖାଲି;
	}

	PyList_SET_ITEM(list, i, item);
    }

    ଫେରନ୍ତୁ list;
}

    ସ୍ଥିର PyObject *
ListItem(ListObject *self, PyObject* idx)
{
#ଯଦି PY_MAJOR_VERSION < 3
    ଯଦି (PyInt_Check(idx))
    {
	long _idx = PyInt_AsLong(idx);
	ଫେରନ୍ତୁ ListIndex(self, _idx);
    }
    ଅନ୍ୟଥା
#endif
    ଯଦି (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	ଫେରନ୍ତୁ ListIndex(self, _idx);
    }
    ଅନ୍ୟଥା ଯଦି (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	ଯଦି (PySlice_GetIndicesEx((PySliceObject_T *)idx, ListLength(self),
				 &start, &stop, &step, &slicelen) < 0)
	    ଫେରନ୍ତୁ ଖାଲି;
	ଫେରନ୍ତୁ ListSlice(self, start, step, slicelen);
    }
    ଅନ୍ୟଥା
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	ଫେରନ୍ତୁ ଖାଲି;
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
list_restore(Py_ssize_t numadded, Py_ssize_t numreplaced, Py_ssize_t slicelen,
	list_T *l, listitem_T **lis, listitem_T *lastaddedli)
{
    ଯେତେବେଳେ (numreplaced--)
    {
	list_ଭର୍ତ୍ତି(l, lis[numreplaced], lis[slicelen + numreplaced]);
	listitem_remove(l, lis[slicelen + numreplaced]);
    }
    ଯେତେବେଳେ (numadded--)
    {
	listitem_T	*next;

	next = lastaddedli->li_prev;
	listitem_remove(l, lastaddedli);
	lastaddedli = next;
    }
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ListAssSlice(ListObject *self, Py_ssize_t first,
	     Py_ssize_t step, Py_ssize_t slicelen, PyObject *obj)
{
    PyObject	*iterator;
    PyObject	*item;
    listitem_T	*li;
    listitem_T	*lastaddedli = ଖାଲି;
    listitem_T	*next;
    typval_T	v;
    list_T	*l = self->list;
    PyInt	i;
    PyInt	j;
    PyInt	numreplaced = 0;
    PyInt	numadded = 0;
    PyInt	size;
    listitem_T	**lis = ଖାଲି;

    size = ListLength(self);

    ଯଦି (l->lv_lock)
    {
	RAISE_LOCKED_LIST;
	ଫେରନ୍ତୁ -1;
    }

    ଯଦି (step == 0)
    {
	PyErr_SET_STRING(Py_Exc_ମୂଲ୍ୟ_Error, N_("slice step cannot be zero"));
	ଫେରନ୍ତୁ -1;
    }

    ଯଦି (step != 1 && slicelen == 0)
    {
	// Nothing to do. Only error out if obj has some items.
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = 0;

	ଯଦି (obj == ଖାଲି)
	    ଫେରନ୍ତୁ 0;

	ଯଦି (!(iterator = PyObject_GetIter(obj)))
	    ଫେରନ୍ତୁ -1;

	ଯଦି ((item = PyIter_Next(iterator)))
	{
	    PyErr_FORMAT(Py_Exc_ମୂଲ୍ୟ_Error,
		    N_("attempt to assign sequence of size greater than %d "
			"to extended slice"), 0);
	    Py_DECREF(item);
	    ret = -1;
	}
	Py_DECREF(iterator);
	ଫେରନ୍ତୁ ret;
    }

    ଯଦି (obj != ଖାଲି)
	// XXX May allocate zero bytes.
	ଯଦି (!(lis = PyMem_New(listitem_T *, slicelen * 2)))
	{
	    PyErr_NoMemory();
	    ଫେରନ୍ତୁ -1;
	}

    ଯଦି (first == size)
	li = ଖାଲି;
    ଅନ୍ୟଥା
    {
	li = list_find(l, (long) first);
	ଯଦି (li == ଖାଲି)
	{
	    PyErr_VIM_FORMAT(N_("internal error: no Vim list item %d"),
		    (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)first);
	    ଯଦି (obj != ଖାଲି)
		PyMem_Free(lis);
	    ଫେରନ୍ତୁ -1;
	}
	i = slicelen;
	ଯେତେବେଳେ (i-- && li != ଖାଲି)
	{
	    j = step;
	    next = li;
	    ଯଦି (step > 0)
		ଯେତେବେଳେ (next != ଖାଲି && ((next = next->li_next) != ଖାଲି) && --j);
	    ଅନ୍ୟଥା
		ଯେତେବେଳେ (next != ଖାଲି && ((next = next->li_prev) != ଖାଲି) && ++j);

	    ଯଦି (obj == ଖାଲି)
		listitem_remove(l, li);
	    ଅନ୍ୟଥା
		lis[slicelen - i - 1] = li;

	    li = next;
	}
	ଯଦି (li == ଖାଲି && i != -1)
	{
	    PyErr_SET_VIM(N_("internal error: not enough list items"));
	    ଯଦି (obj != ଖାଲି)
		PyMem_Free(lis);
	    ଫେରନ୍ତୁ -1;
	}
    }

    ଯଦି (obj == ଖାଲି)
	ଫେରନ୍ତୁ 0;

    ଯଦି (!(iterator = PyObject_GetIter(obj)))
    {
	PyMem_Free(lis);
	ଫେରନ୍ତୁ -1;
    }

    i = 0;
    ଯେତେବେଳେ ((item = PyIter_Next(iterator)))
    {
	ଯଦି (ConvertFromPyObject(item, &v) == -1)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(item);
	    PyMem_Free(lis);
	    ଫେରନ୍ତୁ -1;
	}
	Py_DECREF(item);
	ଯଦି (list_ଭର୍ତ୍ତି_tv(l, &v, numreplaced < slicelen
				    ? lis[numreplaced]
				    : li) == FAIL)
	{
	    clear_tv(&v);
	    PyErr_SET_VIM(N_("internal error: failed to add item to list"));
	    list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	    PyMem_Free(lis);
	    ଫେରନ୍ତୁ -1;
	}
	ଯଦି (numreplaced < slicelen)
	{
	    lis[slicelen + numreplaced] = lis[numreplaced]->li_prev;
	    vimlist_remove(l, lis[numreplaced], lis[numreplaced]);
	    numreplaced++;
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (li)
		lastaddedli = li->li_prev;
	    ଅନ୍ୟଥା
		lastaddedli = l->lv_u.mat.lv_last;
	    numadded++;
	}
	clear_tv(&v);
	ଯଦି (step != 1 && i >= slicelen)
	{
	    Py_DECREF(iterator);
	    PyErr_FORMAT(Py_Exc_ମୂଲ୍ୟ_Error,
		    N_("attempt to assign sequence of size greater than %d "
			"to extended slice"), (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) slicelen);
	    list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	    PyMem_Free(lis);
	    ଫେରନ୍ତୁ -1;
	}
	++i;
    }
    Py_DECREF(iterator);

    ଯଦି (step != 1 && i != slicelen)
    {
	PyErr_FORMAT2(Py_Exc_ମୂଲ୍ୟ_Error,
		N_("attempt to assign sequence of size %d to extended slice "
		    "of size %d"), (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) i, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) slicelen);
	list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	PyMem_Free(lis);
	ଫେରନ୍ତୁ -1;
    }

    ଯଦି (PyErr_Occurred())
    {
	list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	PyMem_Free(lis);
	ଫେରନ୍ତୁ -1;
    }

    ପାଇଁ (i = 0; i < numreplaced; i++)
	listitem_free(l, lis[i]);
    ଯଦି (step == 1)
	ପାଇଁ (i = numreplaced; i < slicelen; i++)
	    listitem_remove(l, lis[i]);

    PyMem_Free(lis);

    ଫେରନ୍ତୁ 0;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ListAssIndex(ListObject *self, Py_ssize_t index, PyObject *obj)
{
    typval_T	tv;
    list_T	*l = self->list;
    listitem_T	*li;
    Py_ssize_t	length = ListLength(self);

    ଯଦି (l->lv_lock)
    {
	RAISE_LOCKED_LIST;
	ଫେରନ୍ତୁ -1;
    }
    ଯଦି (index > length || (index == length && obj == ଖାଲି))
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("list index out of range"));
	ଫେରନ୍ତୁ -1;
    }

    ଯଦି (obj == ଖାଲି)
    {
	li = list_find(l, (long) index);
	ଯଦି (li == ଖାଲି)
	{
	    PyErr_VIM_FORMAT(N_("internal error: failed to get Vim "
			"list item %d"), (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) index);
	    ଫେରନ୍ତୁ -1;
	}
	vimlist_remove(l, li, li);
	clear_tv(&li->li_tv);
	vim_free(li);
	ଫେରନ୍ତୁ 0;
    }

    ଯଦି (ConvertFromPyObject(obj, &tv) == -1)
	ଫେରନ୍ତୁ -1;

    ଯଦି (index == length)
    {
	ଯଦି (list_append_tv(l, &tv) == FAIL)
	{
	    clear_tv(&tv);
	    PyErr_SET_VIM(N_("failed to add item to list"));
	    ଫେରନ୍ତୁ -1;
	}
    }
    ଅନ୍ୟଥା
    {
	li = list_find(l, (long) index);
	ଯଦି (li == ଖାଲି)
	{
	    PyErr_VIM_FORMAT(N_("internal error: failed to get Vim "
			"list item %d"), (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) index);
	    ଫେରନ୍ତୁ -1;
	}
	clear_tv(&li->li_tv);
	copy_tv(&tv, &li->li_tv);
	clear_tv(&tv);
    }
    ଫେରନ୍ତୁ 0;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ListAssItem(ListObject *self, PyObject *idx, PyObject *obj)
{
#ଯଦି PY_MAJOR_VERSION < 3
    ଯଦି (PyInt_Check(idx))
    {
	long _idx = PyInt_AsLong(idx);
	ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ListAssIndex(self, _idx, obj);
    }
    ଅନ୍ୟଥା
#endif
    ଯଦି (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ListAssIndex(self, _idx, obj);
    }
    ଅନ୍ୟଥା ଯଦି (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	ଯଦି (PySlice_GetIndicesEx((PySliceObject_T *)idx, ListLength(self),
				 &start, &stop, &step, &slicelen) < 0)
	    ଫେରନ୍ତୁ -1;
	ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ListAssSlice(self, start, step, slicelen,
		obj);
    }
    ଅନ୍ୟଥା
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	ଫେରନ୍ତୁ -1;
    }
}

    ସ୍ଥିର PyObject *
ListConcatInPlace(ListObject *self, PyObject *obj)
{
    list_T	*l = self->list;
    PyObject	*lookup_dict;

    ଯଦି (l->lv_lock)
    {
	RAISE_LOCKED_LIST;
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ଯଦି (!(lookup_dict = PyDict_New()))
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (list_py_concat(l, obj, lookup_dict) == -1)
    {
	Py_DECREF(lookup_dict);
	ଫେରନ୍ତୁ ଖାଲି;
    }
    Py_DECREF(lookup_dict);

    Py_INCREF((PyObject *)self);
    ଫେରନ୍ତୁ (PyObject *)(self);
}

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    listwatch_T	lw;
    list_T	*list;
} listiterinfo_T;

    ସ୍ଥିର ଶୂନ୍ୟ
ListIterDestruct(ଶୂନ୍ୟ *arg)
{
    listiterinfo_T *lii = (listiterinfo_T*)arg;
    list_rem_watch(lii->list, &lii->lw);
    list_unref(lii->list);
    PyMem_Free(lii);
}

    ସ୍ଥିର PyObject *
ListIterNext(ଶୂନ୍ୟ **arg)
{
    PyObject	*ret;
    listiterinfo_T **lii = (listiterinfo_T**)arg;

    ଯଦି (!((*lii)->lw.lw_item))
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (!(ret = ConvertToPyObject(&((*lii)->lw.lw_item->li_tv))))
	ଫେରନ୍ତୁ ଖାଲି;

    (*lii)->lw.lw_item = (*lii)->lw.lw_item->li_next;

    ଫେରନ୍ତୁ ret;
}

    ସ୍ଥିର PyObject *
ListIter(PyObject *self_obj)
{
    ListObject	*self = (ListObject*)self_obj;
    listiterinfo_T	*lii;
    list_T	*l = self->list;

    ଯଦି (!(lii = PyMem_New(listiterinfo_T, 1)))
    {
	PyErr_NoMemory();
	ଫେରନ୍ତୁ ଖାଲି;
    }

    CHECK_LIST_MATERIALIZE(l);
    list_add_watch(l, &lii->lw);
    lii->lw.lw_item = l->lv_first;
    lii->list = l;
    ++l->lv_refcount;

    ଫେରନ୍ତୁ IterNew(lii,
	    ListIterDestruct, ListIterNext,
	    ଖାଲି, ଖାଲି, (PyObject *)self);
}

ସ୍ଥିର ଅକ୍ଷର *ListAttrs[] = {
    "locked",
    ଖାଲି
};

    ସ୍ଥିର PyObject *
ListDir(PyObject *self, PyObject *args UNUSED)
{
    ଫେରନ୍ତୁ ObjectDir(self, ListAttrs);
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ListSetattr(PyObject *self_obj, ଅକ୍ଷର *name, PyObject *valObject)
{
    ListObject *self = (ListObject*)self_obj;
    ଯଦି (valObject == ଖାଲି)
    {
	PyErr_SET_STRING(PyExc_AttributeError,
		N_("cannot delete vim.List attributes"));
	ଫେରନ୍ତୁ -1;
    }

    ଯଦି (strcmp(name, "locked") == 0)
    {
	ଯଦି (self->list->lv_lock == VAR_FIXED)
	{
	    PyErr_SET_STRING(PyExc_TypeError, N_("cannot modify fixed list"));
	    ଫେରନ୍ତୁ -1;
	}
	ଅନ୍ୟଥା
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		istrue = PyObject_IsTrue(valObject);
	    ଯଦି (istrue == -1)
		ଫେରନ୍ତୁ -1;
	    ଅନ୍ୟଥା ଯଦି (istrue)
		self->list->lv_lock = VAR_LOCKED;
	    ଅନ୍ୟଥା
		self->list->lv_lock = 0;
	}
	ଫେରନ୍ତୁ 0;
    }
    ଅନ୍ୟଥା
    {
	PyErr_FORMAT(PyExc_AttributeError, N_("cannot set attribute %s"), name);
	ଫେରନ୍ତୁ -1;
    }
}

ସ୍ଥିର PySequenceMethods ListAsSeq = {
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

ସ୍ଥିର PyMappingMethods ListAsMapping = {
    /* mp_length	*/ (lenfunc) ListLength,
    /* mp_subscript     */ (binaryfunc) ListItem,
    /* mp_ass_subscript */ (objobjargproc) ListAssItem,
};

ସ୍ଥିର ସଂରଚନା PyMethodDef ListMethods[] = {
    {"extend",	(PyCFunction)ListConcatInPlace,	METH_O,		""},
    {"__dir__",	(PyCFunction)ListDir,		METH_NOARGS,	""},
    { ଖାଲି,	ଖାଲି,				0,		ଖାଲି}
};

DEFINE_PY_TYPE_OBJECT(TupleType);

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    PyObject_HEAD
    tuple_T	*tuple;
    pylinkedlist_T	ref;
} TupleObject;

#define NEW_TUPLE(tuple) TupleNew(TupleTypePtr, tuple)

    ସ୍ଥିର PyObject *
TupleNew(PyTypeObject *subtype, tuple_T *tuple)
{
    TupleObject	*self;

    ଯଦି (tuple == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    self = (TupleObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    ଯଦି (self == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    self->tuple = tuple;
    ++tuple->tv_refcount;

    pyll_add((PyObject *)(self), &self->ref, &lasttuple);

    ଫେରନ୍ତୁ (PyObject *)(self);
}

    ସ୍ଥିର tuple_T *
py_tuple_alloc(ଶୂନ୍ୟ)
{
    tuple_T	*ret;

    ଯଦି (!(ret = tuple_alloc()))
    {
	PyErr_NoMemory();
	ଫେରନ୍ତୁ ଖାଲି;
    }
    ++ret->tv_refcount;

    ଫେରନ୍ତୁ ret;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
tuple_py_concat(tuple_T *t, PyObject *obj, PyObject *lookup_dict)
{
    PyObject	*iterator;
    PyObject	*item;

    ଯଦି (!(iterator = PyObject_GetIter(obj)))
	ଫେରନ୍ତୁ -1;

    ଯେତେବେଳେ ((item = PyIter_Next(iterator)))
    {
	typval_T    new_tv;

	ଯଦି (_ConvertFromPyObject(item, &new_tv, lookup_dict) == -1)
	{
	    Py_DECREF(item);
	    Py_DECREF(iterator);
	    ଫେରନ୍ତୁ -1;
	}

	Py_DECREF(item);

	ଯଦି (tuple_append_tv(t, &new_tv) == FAIL)
	{
	    Py_DECREF(iterator);
	    ଫେରନ୍ତୁ -1;
	}
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    ଯଦି (PyErr_Occurred())
	ଫେରନ୍ତୁ -1;

    ଫେରନ୍ତୁ 0;
}

    ସ୍ଥିର PyObject *
TupleConstructor(PyTypeObject *subtype, PyObject *args, PyObject *kwargs)
{
    tuple_T	*tuple;
    PyObject	*obj = ଖାଲି;

    ଯଦି (kwargs)
    {
	PyErr_SET_STRING(PyExc_TypeError,
		N_("tuple constructor does not accept keyword arguments"));
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ଯଦି (!PyArg_ParseTuple(args, "|O", &obj))
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (!(tuple = py_tuple_alloc()))
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (obj)
    {
	PyObject	*lookup_dict;

	ଯଦି (!(lookup_dict = PyDict_New()))
	{
	    tuple_unref(tuple);
	    ଫେରନ୍ତୁ ଖାଲି;
	}

	ଯଦି (tuple_py_concat(tuple, obj, lookup_dict) == -1)
	{
	    Py_DECREF(lookup_dict);
	    tuple_unref(tuple);
	    ଫେରନ୍ତୁ ଖାଲି;
	}

	Py_DECREF(lookup_dict);
    }

    ଫେରନ୍ତୁ TupleNew(subtype, tuple);
}

    ସ୍ଥିର ଶୂନ୍ୟ
TupleDestructor(PyObject *self_obj)
{
    TupleObject *self = (TupleObject *)self_obj;
    pyll_remove(&self->ref, &lasttuple);
    tuple_unref(self->tuple);

    DESTRUCTOR_FINISH(self);
}

    ସ୍ଥିର PyInt
TupleLength(TupleObject *self)
{
    ଫେରନ୍ତୁ ((PyInt)(tuple_len(self->tuple)));
}

    ସ୍ଥିର PyObject *
TupleIndex(TupleObject *self, Py_ssize_t index)
{
    PyInt	len = TupleLength(self);

    ଯଦି (index < 0)
	index = len + index;

    ଯଦି (index < 0 || index >= len)
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("tuple index out of range"));
	ଫେରନ୍ତୁ ଖାଲି;
    }
    ଫେରନ୍ତୁ ConvertToPyObject(TUPLE_ITEM(self->tuple, index));
}

/*
 * Return a new tuple object for the tuple slice starting from the index
 * "first" and of length "slicelen" skipping "step" items.
 */
    ସ୍ଥିର PyObject *
TupleSlice(
    TupleObject		*self,
    Py_ssize_t		first,
    Py_ssize_t		step,
    Py_ssize_t		slicelen)
{
    PyInt	i;
    PyObject	*tuple;

    tuple = PyTuple_New(slicelen);
    ଯଦି (tuple == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ପାଇଁ (i = 0; i < slicelen; ++i)
    {
	PyObject	*item;

	item = TupleIndex(self, first + i * step);
	ଯଦି (item == ଖାଲି)
	{
	    Py_DECREF(tuple);
	    ଫେରନ୍ତୁ ଖାଲି;
	}

	PyTuple_SetItem(tuple, i, item);
    }

    ଫେରନ୍ତୁ tuple;
}

    ସ୍ଥିର PyObject *
TupleItem(TupleObject *self, PyObject* idx)
{
#ଯଦି PY_MAJOR_VERSION < 3
    ଯଦି (PyInt_Check(idx))
    {
	long _idx = PyInt_AsLong(idx);
	ଫେରନ୍ତୁ TupleIndex(self, _idx);
    }
    ଅନ୍ୟଥା
#endif
    ଯଦି (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	ଫେରନ୍ତୁ TupleIndex(self, _idx);
    }
    ଅନ୍ୟଥା ଯଦି (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	ଯଦି (PySlice_GetIndicesEx((PySliceObject_T *)idx, TupleLength(self),
				 &start, &stop, &step, &slicelen) < 0)
	    ଫେରନ୍ତୁ ଖାଲି;
	ଫେରନ୍ତୁ TupleSlice(self, start, step, slicelen);
    }
    ଅନ୍ୟଥା
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	ଫେରନ୍ତୁ ଖାଲି;
    }
}

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    tuple_T	*tuple;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		index;
} tupleiterinfo_T;

    ସ୍ଥିର ଶୂନ୍ୟ
TupleIterDestruct(ଶୂନ୍ୟ *arg)
{
    tupleiterinfo_T *tii = (tupleiterinfo_T*)arg;
    tuple_unref(tii->tuple);
    PyMem_Free(tii);
}

    ସ୍ଥିର PyObject *
TupleIterNext(ଶୂନ୍ୟ **arg)
{
    PyObject	*ret;
    tupleiterinfo_T **tii = (tupleiterinfo_T**)arg;

    ଯଦି ((*tii)->index >= TUPLE_LEN((*tii)->tuple))
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (!(ret = ConvertToPyObject(TUPLE_ITEM((*tii)->tuple, (*tii)->index))))
	ଫେରନ୍ତୁ ଖାଲି;

    (*tii)->index++;

    ଫେରନ୍ତୁ ret;
}

    ସ୍ଥିର PyObject *
TupleIter(PyObject *self_obj)
{
    TupleObject	*self = (TupleObject*)self_obj;
    tupleiterinfo_T	*tii;
    tuple_T	*t = self->tuple;

    ଯଦି (!(tii = PyMem_New(tupleiterinfo_T, 1)))
    {
	PyErr_NoMemory();
	ଫେରନ୍ତୁ ଖାଲି;
    }

    tii->tuple = t;
    tii->index = 0;
    ++t->tv_refcount;

    ଫେରନ୍ତୁ IterNew(tii,
	    TupleIterDestruct, TupleIterNext,
	    ଖାଲି, ଖାଲି, (PyObject *)self);
}

ସ୍ଥିର ଅକ୍ଷର *TupleAttrs[] = {
    "locked",
    ଖାଲି
};

    ସ୍ଥିର PyObject *
TupleDir(PyObject *self, PyObject *args UNUSED)
{
    ଫେରନ୍ତୁ ObjectDir(self, TupleAttrs);
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
TupleSetattr(PyObject *self_obj, ଅକ୍ଷର *name, PyObject *valObject)
{
    TupleObject *self = (TupleObject*)self_obj;
    ଯଦି (valObject == ଖାଲି)
    {
	PyErr_SET_STRING(PyExc_AttributeError,
		N_("cannot delete vim.Tuple attributes"));
	ଫେରନ୍ତୁ -1;
    }

    ଯଦି (strcmp(name, "locked") == 0)
    {
	ଯଦି (self->tuple->tv_lock == VAR_FIXED)
	{
	    PyErr_SET_STRING(PyExc_TypeError, N_("cannot modify fixed tuple"));
	    ଫେରନ୍ତୁ -1;
	}
	ଅନ୍ୟଥା
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		istrue = PyObject_IsTrue(valObject);
	    ଯଦି (istrue == -1)
		ଫେରନ୍ତୁ -1;
	    ଅନ୍ୟଥା ଯଦି (istrue)
		self->tuple->tv_lock = VAR_LOCKED;
	    ଅନ୍ୟଥା
		self->tuple->tv_lock = 0;
	}
	ଫେରନ୍ତୁ 0;
    }
    ଅନ୍ୟଥା
    {
	PyErr_FORMAT(PyExc_AttributeError, N_("cannot set attribute %s"), name);
	ଫେରନ୍ତୁ -1;
    }
}

ସ୍ଥିର PySequenceMethods TupleAsSeq = {
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

ସ୍ଥିର PyMappingMethods TupleAsMapping = {
    /* mp_length	*/ (lenfunc) TupleLength,
    /* mp_subscript     */ (binaryfunc) TupleItem,
    /* mp_ass_subscript */ (objobjargproc) 0,
};

ସ୍ଥିର ସଂରଚନା PyMethodDef TupleMethods[] = {
    {"__dir__",	(PyCFunction)TupleDir,		METH_NOARGS,	""},
    { ଖାଲି,	ଖାଲି,				0,		ଖାଲି}
};

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    PyObject_HEAD
    char_u	*name;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		argc;
    typval_T	*argv;
    dict_T	*self;
    pylinkedlist_T	ref;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		auto_rebind;
} FunctionObject;

DEFINE_PY_TYPE_OBJECT(FunctionType);

#define NEW_FUNCTION(name, argc, argv, self, pt_auto) \
    FunctionNew(FunctionTypePtr, (name), (argc), (argv), (self), (pt_auto))

    ସ୍ଥିର PyObject *
FunctionNew(PyTypeObject *subtype, char_u *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, typval_T *argv,
	dict_T *selfdict, ପୂର୍ଣ୍ଣସଂଖ୍ୟା auto_rebind)
{
    FunctionObject	*self;

    self = (FunctionObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    ଯଦି (self == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (isdigit((unsigned ଅକ୍ଷର)*name))
    {
	ଯଦି (!translated_function_exists(name, FALSE))
	{
	    PyErr_FORMAT(Py_Exc_ମୂଲ୍ୟ_Error,
		    N_("unnamed function %s does not exist"), name);
	    ଫେରନ୍ତୁ ଖାଲି;
	}
	self->name = vim_strsave(name);
    }
    ଅନ୍ୟଥା
    {
	char_u *p;

	ଯଦି ((p = get_expanded_name(name,
			    vim_strchr(name, AUTOLOAD_CHAR) == ଖାଲି)) == ଖାଲି)
	{
	    PyErr_FORMAT(Py_Exc_ମୂଲ୍ୟ_Error,
		    N_("function %s does not exist"), name);
	    ଫେରନ୍ତୁ ଖାଲି;
	}

	ଯଦି (p[0] == K_SPECIAL && p[1] == KS_EXTRA && p[2] == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_SNR)
	{
	    char_u *np;
	    size_t len = STRLEN(p) + 1;

	    ଯଦି ((np = alloc(len + 2)) == ଖାଲି)
	    {
		vim_free(p);
		ଫେରନ୍ତୁ ଖାଲି;
	    }
	    mch_memmove(np, "<SNR>", 5);
	    mch_memmove(np + 5, p + 3, len - 3);
	    vim_free(p);
	    self->name = np;
	}
	ଅନ୍ୟଥା
	    self->name = p;
    }

    func_ref(self->name);
    self->argc = argc;
    self->argv = argv;
    self->self = selfdict;
    self->auto_rebind = selfdict == ଖାଲି ? TRUE : auto_rebind;

    ଯଦି (self->argv || self->self)
	pyll_add((PyObject *)(self), &self->ref, &lastfunc);

    ଫେରନ୍ତୁ (PyObject *)(self);
}

    ସ୍ଥିର PyObject *
FunctionConstructor(PyTypeObject *subtype, PyObject *args, PyObject *kwargs)
{
    PyObject	*self;
    PyObject	*selfdictObject;
    PyObject	*autoRebindObject;
    PyObject	*argsObject = ଖାଲି;
    char_u	*name;
    typval_T	selfdicttv;
    typval_T	argstv;
    list_T	*argslist = ଖାଲି;
    dict_T	*selfdict = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		argc = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		auto_rebind = TRUE;
    typval_T	*argv = ଖାଲି;
    typval_T	*curtv;
    listitem_T	*li;

    ଯଦି (kwargs != ଖାଲି)
    {
	selfdictObject = PyDict_GetItemString(kwargs, "self");
	ଯଦି (selfdictObject != ଖାଲି)
	{
	    ଯଦି (ConvertFromPyMapping(selfdictObject, &selfdicttv) == -1)
		ଫେରନ୍ତୁ ଖାଲି;
	    selfdict = selfdicttv.vval.v_dict;
	}
	argsObject = PyDict_GetItemString(kwargs, "args");
	ଯଦି (argsObject != ଖାଲି)
	{
	    ଯଦି (ConvertFromPySequence(argsObject, &argstv) == -1)
	    {
		dict_unref(selfdict);
		ଫେରନ୍ତୁ ଖାଲି;
	    }
	    argslist = argstv.vval.v_list;
	    CHECK_LIST_MATERIALIZE(argslist);

	    argc = argslist->lv_len;
	    ଯଦି (argc != 0)
	    {
		argv = PyMem_New(typval_T, (size_t) argc);
		ଯଦି (argv == ଖାଲି)
		{
		    PyErr_NoMemory();
		    dict_unref(selfdict);
		    list_unref(argslist);
		    ଫେରନ୍ତୁ ଖାଲି;
		}
		curtv = argv;
		FOR_ALL_LIST_ବସ୍ତୁଗୁଡ଼ିକ(argslist, li)
		    copy_tv(&li->li_tv, curtv++);
	    }
	    list_unref(argslist);
	}
	ଯଦି (selfdict != ଖାଲି)
	{
	    auto_rebind = FALSE;
	    autoRebindObject = PyDict_GetItemString(kwargs, "auto_rebind");
	    ଯଦି (autoRebindObject != ଖାଲି)
	    {
		auto_rebind = PyObject_IsTrue(autoRebindObject);
		ଯଦି (auto_rebind == -1)
		{
		    dict_unref(selfdict);
		    list_unref(argslist);
		    ଫେରନ୍ତୁ ଖାଲି;
		}
	    }
	}
    }

    ଯଦି (!PyArg_ParseTuple(args, "et", "ascii", &name))
    {
	dict_unref(selfdict);
	ଯେତେବେଳେ (argc--)
	    clear_tv(&argv[argc]);
	PyMem_Free(argv);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    self = FunctionNew(subtype, name, argc, argv, selfdict, auto_rebind);

    PyMem_Free(name);

    ଫେରନ୍ତୁ self;
}

    ସ୍ଥିର ଶୂନ୍ୟ
FunctionDestructor(PyObject *self_obj)
{
    FunctionObject *self = (FunctionObject*)self_obj;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
    func_unref(self->name);
    vim_free(self->name);
    ପାଇଁ (i = 0; i < self->argc; ++i)
	clear_tv(&self->argv[i]);
    PyMem_Free(self->argv);
    dict_unref(self->self);
    ଯଦି (self->argv || self->self)
	pyll_remove(&self->ref, &lastfunc);

    DESTRUCTOR_FINISH(self);
}

ସ୍ଥିର ଅକ୍ଷର *FunctionAttrs[] = {
    "softspace", "args", "self", "auto_rebind",
    ଖାଲି
};

    ସ୍ଥିର PyObject *
FunctionDir(PyObject *self, PyObject *args UNUSED)
{
    ଫେରନ୍ତୁ ObjectDir(self, FunctionAttrs);
}

    ସ୍ଥିର PyObject *
FunctionAttr(FunctionObject *self, ଅକ୍ଷର *name)
{
    list_T *list;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
    ଯଦି (strcmp(name, "name") == 0)
	ଫେରନ୍ତୁ PyString_FromString((ଅକ୍ଷର *)(self->name));
    ଅନ୍ୟଥା ଯଦି (strcmp(name, "args") == 0)
    {
	ଯଦି (self->argv == ଖାଲି || (list = list_alloc()) == ଖାଲି)
	    ଫେରନ୍ତୁ ALWAYS_NONE;

	ପାଇଁ (i = 0; i < self->argc; ++i)
	    list_append_tv(list, &self->argv[i]);
	ଫେରନ୍ତୁ NEW_LIST(list);
    }
    ଅନ୍ୟଥା ଯଦି (strcmp(name, "self") == 0)
	ଫେରନ୍ତୁ self->self == ଖାଲି
	    ? ALWAYS_NONE
	    : NEW_DICTIONARY(self->self);
    ଅନ୍ୟଥା ଯଦି (strcmp(name, "auto_rebind") == 0)
	ଫେରନ୍ତୁ self->auto_rebind
	    ? ALWAYS_TRUE
	    : ALWAYS_FALSE;
    ଅନ୍ୟଥା ଯଦି (strcmp(name, "__members__") == 0)
	ଫେରନ୍ତୁ ObjectDir(ଖାଲି, FunctionAttrs);
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Populate partial_T given function object.
 *
 * "exported" should be set to true when it is needed to construct a partial
 * that may be stored in a variable (i.e. may be freed by Vim).
 */
    ସ୍ଥିର ଶୂନ୍ୟ
set_partial(FunctionObject *self, partial_T *pt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା exported)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    pt->pt_name = self->name;
    ଯଦି (self->argv)
    {
	pt->pt_argc = self->argc;
	ଯଦି (exported)
	{
	    pt->pt_argv = ALLOC_CLEAR_MULT(typval_T, self->argc);
	    ପାଇଁ (i = 0; i < pt->pt_argc; ++i)
		copy_tv(&self->argv[i], &pt->pt_argv[i]);
	}
	ଅନ୍ୟଥା
	    pt->pt_argv = self->argv;
    }
    ଅନ୍ୟଥା
    {
	pt->pt_argc = 0;
	pt->pt_argv = ଖାଲି;
    }
    pt->pt_auto = self->auto_rebind || !exported;
    pt->pt_dict = self->self;
    ଯଦି (exported && self->self)
	++pt->pt_dict->dv_refcount;
    ଯଦି (exported)
	pt->pt_name = vim_strsave(pt->pt_name);
    pt->pt_refcount = 1;
}

    ସ୍ଥିର PyObject *
FunctionCall(PyObject *self_obj, PyObject *argsObject, PyObject *kwargs)
{
    FunctionObject *self = (FunctionObject*)self_obj;
    char_u	*name = self->name;
    typval_T	args;
    typval_T	selfdicttv;
    typval_T	rettv;
    dict_T	*selfdict = ଖାଲି;
    PyObject	*selfdictObject;
    PyObject	*ret;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error;
    partial_T	pt;
    partial_T	*pt_ptr = ଖାଲି;

    ଯଦି (ConvertFromPySequence(argsObject, &args) == -1)
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (kwargs != ଖାଲି)
    {
	selfdictObject = PyDict_GetItemString(kwargs, "self");
	ଯଦି (selfdictObject != ଖାଲି)
	{
	    ଯଦି (ConvertFromPyMapping(selfdictObject, &selfdicttv) == -1)
	    {
		clear_tv(&args);
		ଫେରନ୍ତୁ ଖାଲି;
	    }
	    selfdict = selfdicttv.vval.v_dict;
	}
    }

    ଯଦି (self->argv || self->self)
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

    ଯଦି (VimTryEnd())
	ret = ଖାଲି;
    ଅନ୍ୟଥା ଯଦି (error != OK)
    {
	ret = ଖାଲି;
	PyErr_VIM_FORMAT(N_("failed to run function %s"), (ଅକ୍ଷର *)name);
    }
    ଅନ୍ୟଥା
	ret = ConvertToPyObject(&rettv);

    clear_tv(&args);
    clear_tv(&rettv);
    ଯଦି (selfdict != ଖାଲି)
	clear_tv(&selfdicttv);

    ଫେରନ୍ତୁ ret;
}

    ସ୍ଥିର PyObject *
FunctionRepr(PyObject *self_obj)
{
    FunctionObject *self = (FunctionObject*)self_obj;
    PyObject *ret;
    garray_T repr_ga;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
    char_u *tofree = ଖାଲି;
    typval_T tv;
    char_u numbuf[NUMBUFLEN];

    ga_init2(&repr_ga, ଆକାର(ଅକ୍ଷର), 70);
    ga_concat(&repr_ga, (char_u *)"<vim.Function '");
    ଯଦି (self->name)
	ga_concat(&repr_ga, self->name);
    ଅନ୍ୟଥା
	ga_concat(&repr_ga, (char_u *)"<NULL>");
    ga_append(&repr_ga, '\'');
    ଯଦି (self->argv)
    {
	ga_concat(&repr_ga, (char_u *)", args=[");
	++emsg_silent;
	ପାଇଁ (i = 0; i < self->argc; i++)
	{
	    ଯଦି (i != 0)
		ga_concat(&repr_ga, (char_u *)", ");
	    ga_concat(&repr_ga, tv2string(&self->argv[i], &tofree, numbuf,
			get_copyID()));
	    vim_free(tofree);
	}
	--emsg_silent;
	ga_append(&repr_ga, ']');
    }
    ଯଦି (self->self)
    {
	ga_concat(&repr_ga, (char_u *)", self=");
	tv.v_type = VAR_DICT;
	tv.vval.v_dict = self->self;
	++emsg_silent;
	ga_concat(&repr_ga, tv2string(&tv, &tofree, numbuf, get_copyID()));
	--emsg_silent;
	vim_free(tofree);
	ଯଦି (self->auto_rebind)
	    ga_concat(&repr_ga, (char_u *)", auto_rebind=True");
    }
    ga_append(&repr_ga, '>');
    ret = PyString_FromString((ଅକ୍ଷର *)repr_ga.ga_data);
    ga_clear(&repr_ga);
    ଫେରନ୍ତୁ ret;
}

ସ୍ଥିର ସଂରଚନା PyMethodDef FunctionMethods[] = {
    {"__dir__",	(PyCFunction)FunctionDir,   METH_NOARGS,		""},
    { ଖାଲି,	ଖାଲି,			0,				ଖାଲି}
};

/*
 * Options object
 */

DEFINE_PY_TYPE_OBJECT(OptionsType);

ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*checkfun)(ଶୂନ୍ୟ *);

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    PyObject_HEAD
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_type;
    ଶୂନ୍ୟ	*from;
    checkfun	Check;
    PyObject	*fromObj;
} OptionsObject;

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
dummy_check(ଶୂନ୍ୟ *arg UNUSED)
{
    ଫେରନ୍ତୁ 0;
}

    ସ୍ଥିର PyObject *
OptionsNew(ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt_type, ଶୂନ୍ୟ *from, checkfun Check, PyObject *fromObj)
{
    OptionsObject	*self;

    self = PyObject_GC_New(OptionsObject, OptionsTypePtr);
    ଯଦି (self == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    self->opt_type = opt_type;
    self->from = from;
    self->Check = Check;
    self->fromObj = fromObj;
    ଯଦି (fromObj)
	Py_INCREF(fromObj);

    ଫେରନ୍ତୁ (PyObject *)(self);
}

    ସ୍ଥିର ଶୂନ୍ୟ
OptionsDestructor(PyObject *self_obj)
{
    OptionsObject *self = (OptionsObject*)self_obj;
    PyObject_GC_UnTrack((ଶୂନ୍ୟ *)(self));
    Py_XDECREF(self->fromObj);
    PyObject_GC_Del((ଶୂନ୍ୟ *)(self));
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
OptionsTraverse(PyObject *self_obj, visitproc visit, ଶୂନ୍ୟ *arg)
{
    OptionsObject *self = (OptionsObject*)self_obj;
    Py_VISIT(self->fromObj);
    ଫେରନ୍ତୁ 0;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
OptionsClear(PyObject *self_obj)
{
    OptionsObject *self = (OptionsObject*)self_obj;
    Py_CLEAR(self->fromObj);
    ଫେରନ୍ତୁ 0;
}

    ସ୍ଥିର PyObject *
OptionsItem(OptionsObject *self, PyObject *ଚାବି_Object)
{
    char_u	*ଚାବି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags;
    long	numval;
    char_u	*stringval;
    PyObject	*todecref;

    ଯଦି (self->Check(self->fromObj))
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (!(ଚାବି = StringToChars(ଚାବି_Object, &todecref)))
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (*ଚାବି == NUL)
    {
	RAISE_NO_EMPTY_ଚାବିଗୁଡ଼ିକ;
	Py_XDECREF(todecref);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    flags = get_option_ମୂଲ୍ୟ_strict(ଚାବି, &numval, &stringval,
				    self->opt_type, self->from);

    Py_XDECREF(todecref);

    ଯଦି (flags == 0)
    {
	PyErr_SetObject(Py_Exc_ଚାବି_Error, ଚାବି_Object);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ଯଦି (flags & SOPT_UNSET)
    {
	Py_INCREF(Py_None);
	ଫେରନ୍ତୁ Py_None;
    }
    ଅନ୍ୟଥା ଯଦି (flags & SOPT_BOOL)
    {
	PyObject	*ret;
	ret = numval ? Py_True : Py_False;
	Py_INCREF(ret);
	ଫେରନ୍ତୁ ret;
    }
    ଅନ୍ୟଥା ଯଦି (flags & SOPT_ଅଙ୍କ)
	ଫେରନ୍ତୁ PyInt_FromLong(numval);
    ଅନ୍ୟଥା ଯଦି (flags & SOPT_STRING)
    {
	ଯଦି (stringval)
	{
	    PyObject	*ret = PyBytes_FromString((ଅକ୍ଷର *)stringval);
	    vim_free(stringval);
	    ଫେରନ୍ତୁ ret;
	}
	ଅନ୍ୟଥା
	{
	    PyErr_SET_STRING(PyExc_RuntimeError,
		    N_("unable to get option value"));
	    ଫେରନ୍ତୁ ଖାଲି;
	}
    }
    ଅନ୍ୟଥା
    {
	PyErr_SET_VIM(N_("internal error: unknown option type"));
	ଫେରନ୍ତୁ ଖାଲି;
    }
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
OptionsContains(OptionsObject *self, PyObject *ଚାବି_Object)
{
    char_u	*ଚାବି;
    PyObject	*todecref;

    ଯଦି (!(ଚାବି = StringToChars(ଚାବି_Object, &todecref)))
	ଫେରନ୍ତୁ -1;

    ଯଦି (*ଚାବି == NUL)
    {
	Py_XDECREF(todecref);
	ଫେରନ୍ତୁ 0;
    }

    ଯଦି (get_option_ମୂଲ୍ୟ_strict(ଚାବି, ଖାଲି, ଖାଲି, self->opt_type, ଖାଲି))
    {
	Py_XDECREF(todecref);
	ଫେରନ୍ତୁ 1;
    }
    ଅନ୍ୟଥା
    {
	Py_XDECREF(todecref);
	ଫେରନ୍ତୁ 0;
    }
}

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    ଶୂନ୍ୟ	*lastoption;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_type;
} optiterinfo_T;

    ସ୍ଥିର PyObject *
OptionsIterNext(ଶୂନ୍ୟ **arg)
{
    char_u	*name;
    optiterinfo_T **oii = (optiterinfo_T**)arg;

    ଯଦି ((name = option_iter_next(&((*oii)->lastoption), (*oii)->opt_type)))
	ଫେରନ୍ତୁ PyString_FromString((ଅକ୍ଷର *)name);

    ଫେରନ୍ତୁ ଖାଲି;
}

    ସ୍ଥିର PyObject *
OptionsIter(PyObject *self_obj)
{
    OptionsObject	*self = (OptionsObject*)self_obj;
    optiterinfo_T	*oii;

    ଯଦି (!(oii = PyMem_New(optiterinfo_T, 1)))
    {
	PyErr_NoMemory();
	ଫେରନ୍ତୁ ଖାଲି;
    }

    oii->opt_type = self->opt_type;
    oii->lastoption = ଖାଲି;

    ଫେରନ୍ତୁ IterNew(oii,
	    PyMem_Free, OptionsIterNext,
	    ଖାଲି, ଖାଲି, (PyObject *)self);
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
set_option_ମୂଲ୍ୟ_err(char_u *ଚାବି, ପୂର୍ଣ୍ଣସଂଖ୍ୟା numval, char_u *stringval, ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt_flags)
{
    ଅକ୍ଷର	*errmsg;

    ଯଦି ((errmsg = set_option_ମୂଲ୍ୟ(ଚାବି, numval, stringval, opt_flags)))
    {
	ଯଦି (VimTryEnd())
	    ଫେରନ୍ତୁ FAIL;
	PyErr_SetVim(errmsg);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
set_option_ମୂଲ୍ୟ_for(
	char_u *ଚାବି,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା numval,
	char_u *stringval,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt_flags,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt_type,
	ଶୂନ୍ୟ *from)
{
    switchwin_T	switchwin;
    bufref_T	save_curbuf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		set_ret = 0;

    VimTryStart();
    ବାଛ (opt_type)
    {
	ସ୍ଥିତି SREQ_WIN:
	    ଯଦି (switch_win(&switchwin, (win_T *)from,
			      win_find_tabpage((win_T *)from), FALSE) == FAIL)
	    {
		restore_win(&switchwin, TRUE);
		ଯଦି (VimTryEnd())
		    ଫେରନ୍ତୁ -1;
		PyErr_SET_VIM(N_("problem while switching windows"));
		ଫେରନ୍ତୁ -1;
	    }
	    set_ret = set_option_ମୂଲ୍ୟ_err(ଚାବି, numval, stringval, opt_flags);
	    restore_win(&switchwin, TRUE);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି SREQ_BUF:
	    switch_buffer(&save_curbuf, (buf_T *)from);
	    set_ret = set_option_ମୂଲ୍ୟ_err(ଚାବି, numval, stringval, opt_flags);
	    restore_buffer(&save_curbuf);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି SREQ_GLOBAL:
	    set_ret = set_option_ମୂଲ୍ୟ_err(ଚାବି, numval, stringval, opt_flags);
	    ଭାଙ୍ଗ;
    }
    ଯଦି (set_ret == FAIL)
	ଫେରନ୍ତୁ -1;
    ଫେରନ୍ତୁ VimTryEnd();
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
OptionsAssItem(OptionsObject *self, PyObject *ଚାବି_Object, PyObject *valObject)
{
    char_u	*ଚାବି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_flags;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = 0;
    PyObject	*todecref;

    ଯଦି (self->Check(self->fromObj))
	ଫେରନ୍ତୁ -1;

    ଯଦି (!(ଚାବି = StringToChars(ଚାବି_Object, &todecref)))
	ଫେରନ୍ତୁ -1;

    ଯଦି (*ଚାବି == NUL)
    {
	RAISE_NO_EMPTY_ଚାବିଗୁଡ଼ିକ;
	Py_XDECREF(todecref);
	ଫେରନ୍ତୁ -1;
    }

    flags = get_option_ମୂଲ୍ୟ_strict(ଚାବି, ଖାଲି, ଖାଲି,
				    self->opt_type, self->from);

    ଯଦି (flags == 0)
    {
	PyErr_SetObject(Py_Exc_ଚାବି_Error, ଚାବି_Object);
	Py_XDECREF(todecref);
	ଫେରନ୍ତୁ -1;
    }

    ଯଦି (valObject == ଖାଲି)
    {
	ଯଦି (self->opt_type == SREQ_GLOBAL)
	{
	    PyErr_FORMAT(Py_Exc_ମୂଲ୍ୟ_Error,
		    N_("unable to unset global option %s"), ଚାବି);
	    Py_XDECREF(todecref);
	    ଫେରନ୍ତୁ -1;
	}
	ଅନ୍ୟଥା ଯଦି (!(flags & SOPT_GLOBAL))
	{
	    PyErr_FORMAT(Py_Exc_ମୂଲ୍ୟ_Error,
		    N_("unable to unset option %s "
		       "which does not have global value"), ଚାବି);
	    Py_XDECREF(todecref);
	    ଫେରନ୍ତୁ -1;
	}
	ଅନ୍ୟଥା
	{
	    unset_global_local_option(ଚାବି, self->from);
	    Py_XDECREF(todecref);
	    ଫେରନ୍ତୁ 0;
	}
    }

    opt_flags = (self->opt_type ? OPT_LOCAL : OPT_GLOBAL);

    ଯଦି (flags & SOPT_BOOL)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	istrue = PyObject_IsTrue(valObject);

	ଯଦି (istrue == -1)
	    ret = -1;
	ଅନ୍ୟଥା
	    ret = set_option_ମୂଲ୍ୟ_for(ଚାବି, istrue, ଖାଲି,
				    opt_flags, self->opt_type, self->from);
    }
    ଅନ୍ୟଥା ଯଦି (flags & SOPT_ଅଙ୍କ)
    {
	long	val;

	ଯଦି (ସଂଖ୍ୟା_To_Long(valObject, &val, ସଂଖ୍ୟା_INT))
	{
	    Py_XDECREF(todecref);
	    ଫେରନ୍ତୁ -1;
	}

	ret = set_option_ମୂଲ୍ୟ_for(ଚାବି, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) val, ଖାଲି, opt_flags,
				self->opt_type, self->from);
    }
    ଅନ୍ୟଥା
    {
	char_u		*val;
	PyObject	*todecref2;

	ଯଦି ((val = StringToChars(valObject, &todecref2)))
	{
	    ret = set_option_ମୂଲ୍ୟ_for(ଚାବି, 0, val, opt_flags,
				    self->opt_type, self->from);
	    Py_XDECREF(todecref2);
	}
	ଅନ୍ୟଥା
	    ret = -1;
    }

    Py_XDECREF(todecref);

    ଫେରନ୍ତୁ ret;
}

ସ୍ଥିର PySequenceMethods OptionsAsSeq = {
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

ସ୍ଥିର PyMappingMethods OptionsAsMapping = {
    (lenfunc)       ଖାଲି,
    (binaryfunc)    OptionsItem,
    (objobjargproc) OptionsAssItem,
};

// Tabpage object

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    PyObject_HEAD
    tabpage_T	*tab;
} TabPageObject;

ସ୍ଥିର PyObject *WinListNew(TabPageObject *tabObject);

DEFINE_PY_TYPE_OBJECT(TabPageType);

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
CheckTabPage(TabPageObject *self)
{
    ଯଦି (self->tab == INVALID_TABPAGE_ମୂଲ୍ୟ)
    {
	PyErr_SET_VIM(N_("attempt to refer to deleted tab page"));
	ଫେରନ୍ତୁ -1;
    }

    ଫେରନ୍ତୁ 0;
}

    ସ୍ଥିର PyObject *
TabPageNew(tabpage_T *tab)
{
    TabPageObject *self;

    ଯଦି (TAB_PYTHON_REF(tab))
    {
	self = TAB_PYTHON_REF(tab);
	Py_INCREF((PyObject *)self);
    }
    ଅନ୍ୟଥା
    {
	self = PyObject_NEW(TabPageObject, TabPageTypePtr);
	ଯଦି (self == ଖାଲି)
	    ଫେରନ୍ତୁ ଖାଲି;
	self->tab = tab;
	TAB_PYTHON_REF(tab) = self;
    }

    ଫେରନ୍ତୁ (PyObject *)(self);
}

    ସ୍ଥିର ଶୂନ୍ୟ
TabPageDestructor(PyObject *self_obj)
{
    TabPageObject *self = (TabPageObject*)self_obj;
    ଯଦି (self->tab && self->tab != INVALID_TABPAGE_ମୂଲ୍ୟ)
	TAB_PYTHON_REF(self->tab) = ଖାଲି;

    DESTRUCTOR_FINISH(self);
}

ସ୍ଥିର ଅକ୍ଷର *TabPageAttrs[] = {
    "windows", "number", "vars", "window", "valid",
    ଖାଲି
};

    ସ୍ଥିର PyObject *
TabPageDir(PyObject *self, PyObject *args UNUSED)
{
    ଫେରନ୍ତୁ ObjectDir(self, TabPageAttrs);
}

    ସ୍ଥିର PyObject *
TabPageAttrValid(TabPageObject *self, ଅକ୍ଷର *name)
{
    PyObject	*ret;

    ଯଦି (strcmp(name, "valid") != 0)
	ଫେରନ୍ତୁ ଖାଲି;

    ret = ((self->tab == INVALID_TABPAGE_ମୂଲ୍ୟ) ? Py_False : Py_True);
    Py_INCREF(ret);
    ଫେରନ୍ତୁ ret;
}

    ସ୍ଥିର PyObject *
TabPageAttr(TabPageObject *self, ଅକ୍ଷର *name)
{
    ଯଦି (strcmp(name, "windows") == 0)
	ଫେରନ୍ତୁ WinListNew(self);
    ଅନ୍ୟଥା ଯଦି (strcmp(name, "number") == 0)
	ଫେରନ୍ତୁ PyLong_FromLong((long) get_tab_ସଂଖ୍ୟା(self->tab));
    ଅନ୍ୟଥା ଯଦି (strcmp(name, "vars") == 0)
	ଫେରନ୍ତୁ NEW_DICTIONARY(self->tab->tp_vars);
    ଅନ୍ୟଥା ଯଦି (strcmp(name, "window") == 0)
    {
	// For current tab window.c does not bother to set or update tp_curwin
	ଯଦି (self->tab == curtab)
	    ଫେରନ୍ତୁ WindowNew(curwin, curtab);
	ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ WindowNew(self->tab->tp_curwin, self->tab);
    }
    ଅନ୍ୟଥା ଯଦି (strcmp(name, "__members__") == 0)
	ଫେରନ୍ତୁ ObjectDir(ଖାଲି, TabPageAttrs);
    ଫେରନ୍ତୁ ଖାଲି;
}

    ସ୍ଥିର PyObject *
TabPageRepr(PyObject *self_obj)
{
    TabPageObject *self = (TabPageObject*)self_obj;
    ଯଦି (self->tab == INVALID_TABPAGE_ମୂଲ୍ୟ)
	ଫେରନ୍ତୁ PyString_FromFormat("<tabpage object (deleted) at %p>", (ଶୂନ୍ୟ *)self);
    ଅନ୍ୟଥା
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	t = get_tab_ସଂଖ୍ୟା(self->tab);

	ଯଦି (t == 0)
	    ଫେରନ୍ତୁ PyString_FromFormat("<tabpage object (unknown) at %p>",
					(ଶୂନ୍ୟ *)self);
	ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ PyString_FromFormat("<tabpage %d>", t - 1);
    }
}

ସ୍ଥିର ସଂରଚନା PyMethodDef TabPageMethods[] = {
    // name,	    function,			calling,	documentation
    {"__dir__",	    (PyCFunction)TabPageDir,	METH_NOARGS,	""},
    { ଖାଲି,	    ଖାଲି,			0,		ଖାଲି}
};

/*
 * Window list object
 */

DEFINE_PY_TYPE_OBJECT(TabListType);
ସ୍ଥିର PySequenceMethods TabListAsSeq;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    PyObject_HEAD
} TabListObject;

ସ୍ଥିର TabListObject TheTabPageList =
{
    PyObject_HEAD_INIT_TYPE(TabListType)
};

    ସ୍ଥିର PyInt
TabListLength(PyObject *self UNUSED)
{
    tabpage_T	*tp = first_tabpage;
    PyInt	n = 0;

    ଯେତେବେଳେ (tp != ଖାଲି)
    {
	++n;
	tp = tp->tp_next;
    }

    ଫେରନ୍ତୁ n;
}

    ସ୍ଥିର PyObject *
TabListItem(PyObject *self UNUSED, PyInt n)
{
    tabpage_T	*tp;

    ପାଇଁ (tp = first_tabpage; tp != ଖାଲି; tp = tp->tp_next, --n)
	ଯଦି (n == 0)
	    ଫେରନ୍ତୁ TabPageNew(tp);

    PyErr_SET_STRING(PyExc_IndexError, N_("no such tab page"));
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Window object
 */

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    PyObject_HEAD
    win_T	*win;
    TabPageObject	*tabObject;
} WindowObject;

DEFINE_PY_TYPE_OBJECT(WindowType);

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
CheckWindow(WindowObject *self)
{
    ଯଦି (self->win == INVALID_WINDOW_ମୂଲ୍ୟ)
    {
	PyErr_SET_VIM(N_("attempt to refer to deleted window"));
	ଫେରନ୍ତୁ -1;
    }

    ଫେରନ୍ତୁ 0;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
CheckWindowCb(ଶୂନ୍ୟ *self)
{
    ଫେରନ୍ତୁ CheckWindow((WindowObject*)self);
}

    ସ୍ଥିର PyObject *
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

    ଯଦି (WIN_PYTHON_REF(win))
    {
	self = WIN_PYTHON_REF(win);
	Py_INCREF((PyObject *)self);
    }
    ଅନ୍ୟଥା
    {
	self = PyObject_GC_New(WindowObject, WindowTypePtr);
	ଯଦି (self == ଖାଲି)
	    ଫେରନ୍ତୁ ଖାଲି;
	self->win = win;
	WIN_PYTHON_REF(win) = self;
    }

    self->tabObject = ((TabPageObject *)(TabPageNew(tab)));

    ଫେରନ୍ତୁ (PyObject *)(self);
}

    ସ୍ଥିର ଶୂନ୍ୟ
WindowDestructor(PyObject *self_obj)
{
    WindowObject *self = (WindowObject*)self_obj;
    PyObject_GC_UnTrack((ଶୂନ୍ୟ *)(self));
    ଯଦି (self->win && self->win != INVALID_WINDOW_ମୂଲ୍ୟ)
	WIN_PYTHON_REF(self->win) = ଖାଲି;
    Py_XDECREF(((PyObject *)(self->tabObject)));
    PyObject_GC_Del((ଶୂନ୍ୟ *)(self));
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
WindowTraverse(PyObject *self_obj, visitproc visit, ଶୂନ୍ୟ *arg)
{
    WindowObject *self = (WindowObject*)self_obj;
    Py_VISIT(((PyObject *)(self->tabObject)));
    ଫେରନ୍ତୁ 0;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
WindowClear(PyObject *self_obj)
{
    WindowObject *self = (WindowObject*)self_obj;
    Py_CLEAR(self->tabObject);
    ଫେରନ୍ତୁ 0;
}

    ସ୍ଥିର win_T *
get_firstwin(TabPageObject *tabObject)
{
    ଯଦି (tabObject)
    {
	ଯଦି (CheckTabPage(tabObject))
	    ଫେରନ୍ତୁ ଖାଲି;
	// For current tab window.c does not bother to set or update tp_firstwin
	ଅନ୍ୟଥା ଯଦି (tabObject->tab == curtab)
	    ଫେରନ୍ତୁ firstwin;
	ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ tabObject->tab->tp_firstwin;
    }
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ firstwin;
}

// Use the same order as in the WindowAttr() function.
ସ୍ଥିର ଅକ୍ଷର *WindowAttrs[] = {
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
    ଖାଲି
};

    ସ୍ଥିର PyObject *
WindowDir(PyObject *self, PyObject *args UNUSED)
{
    ଫେରନ୍ତୁ ObjectDir(self, WindowAttrs);
}

    ସ୍ଥିର PyObject *
WindowAttrValid(WindowObject *self, ଅକ୍ଷର *name)
{
    PyObject	*ret;

    ଯଦି (strcmp(name, "valid") != 0)
	ଫେରନ୍ତୁ ଖାଲି;

    ret = ((self->win == INVALID_WINDOW_ମୂଲ୍ୟ) ? Py_False : Py_True);
    Py_INCREF(ret);
    ଫେରନ୍ତୁ ret;
}

    ସ୍ଥିର PyObject *
WindowAttr(WindowObject *self, ଅକ୍ଷର *name)
{
    ଯଦି (strcmp(name, "buffer") == 0)
	ଫେରନ୍ତୁ (PyObject *)BufferNew(self->win->w_buffer);
    ଅନ୍ୟଥା ଯଦି (strcmp(name, "cursor") == 0)
    {
	pos_T *pos = &self->win->w_cursor;

	ଫେରନ୍ତୁ Py_Build_ମୂଲ୍ୟ("(ll)", (long)(pos->lnum), (long)(pos->col));
    }
    ଅନ୍ୟଥା ଯଦି (strcmp(name, "height") == 0)
	ଫେରନ୍ତୁ PyLong_FromLong((long)(self->win->w_height));
    ଅନ୍ୟଥା ଯଦି (strcmp(name, "row") == 0)
	ଫେରନ୍ତୁ PyLong_FromLong((long)(self->win->w_winrow));
    ଅନ୍ୟଥା ଯଦି (strcmp(name, "width") == 0)
	ଫେରନ୍ତୁ PyLong_FromLong((long)(self->win->w_width));
    ଅନ୍ୟଥା ଯଦି (strcmp(name, "col") == 0)
	ଫେରନ୍ତୁ PyLong_FromLong((long)(self->win->w_wincol));
    ଅନ୍ୟଥା ଯଦି (strcmp(name, "vars") == 0)
	ଫେରନ୍ତୁ NEW_DICTIONARY(self->win->w_vars);
    ଅନ୍ୟଥା ଯଦି (strcmp(name, "options") == 0)
	ଫେରନ୍ତୁ OptionsNew(SREQ_WIN, self->win, CheckWindowCb,
			(PyObject *) self);
    ଅନ୍ୟଥା ଯଦି (strcmp(name, "number") == 0)
    {
	ଯଦି (CheckTabPage(self->tabObject))
	    ଫେରନ୍ତୁ ଖାଲି;
	ଫେରନ୍ତୁ PyLong_FromLong((long)
		get_win_ସଂଖ୍ୟା(self->win, get_firstwin(self->tabObject)));
    }
    ଅନ୍ୟଥା ଯଦି (strcmp(name, "tabpage") == 0)
    {
	Py_INCREF((PyObject *)self->tabObject);
	ଫେରନ୍ତୁ (PyObject *)(self->tabObject);
    }
    ଅନ୍ୟଥା ଯଦି (strcmp(name, "__members__") == 0)
	ଫେରନ୍ତୁ ObjectDir(ଖାଲି, WindowAttrs);
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ ଖାଲି;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
WindowSetattr(PyObject *self_obj, ଅକ୍ଷର *name, PyObject *valObject)
{
    WindowObject *self = (WindowObject*)self_obj;
    ଯଦି (CheckWindow(self))
	ଫେରନ୍ତୁ -1;

    ଯଦି (strcmp(name, "buffer") == 0)
    {
	PyErr_SET_STRING(PyExc_TypeError, N_("readonly attribute: buffer"));
	ଫେରନ୍ତୁ -1;
    }
    ଅନ୍ୟଥା ଯଦି (strcmp(name, "cursor") == 0)
    {
	long lnum;
	long col;

	ଯଦି (!PyArg_Parse(valObject, "(ll)", &lnum, &col))
	    ଫେରନ୍ତୁ -1;

	ଯଦି (lnum <= 0 || lnum > self->win->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	{
	    PyErr_SET_VIM(N_("cursor position outside buffer"));
	    ଫେରନ୍ତୁ -1;
	}

	// Check for keyboard interrupts
	ଯଦି (VimCheckInterrupt())
	    ଫେରନ୍ତୁ -1;

	self->win->w_cursor.lnum = lnum;
	self->win->w_cursor.col = col;
	self->win->w_set_curswant = TRUE;
	self->win->w_cursor.coladd = 0;
	// When column is out of range silently correct it.
	check_cursor_col_win(self->win);

	update_screen(UPD_VALID);
	ଫେରନ୍ତୁ 0;
    }
    ଅନ୍ୟଥା ଯଦି (strcmp(name, "height") == 0)
    {
	long	height;
	win_T	*savewin;

	ଯଦି (ସଂଖ୍ୟା_To_Long(valObject, &height, ସଂଖ୍ୟା_INT|ସଂଖ୍ୟା_UNSIGNED))
	    ଫେରନ୍ତୁ -1;

#ifdef FEAT_GUI
	need_mouse_correct = TRUE;
#endif
	savewin = curwin;
	curwin = self->win;
	curbuf = curwin->w_buffer;

	VimTryStart();
	win_setheight((ପୂର୍ଣ୍ଣସଂଖ୍ୟା) height);
	curwin = savewin;
	curbuf = curwin->w_buffer;
	ଯଦି (VimTryEnd())
	    ଫେରନ୍ତୁ -1;

	ଫେରନ୍ତୁ 0;
    }
    ଅନ୍ୟଥା ଯଦି (strcmp(name, "width") == 0)
    {
	long	width;
	win_T	*savewin;

	ଯଦି (ସଂଖ୍ୟା_To_Long(valObject, &width, ସଂଖ୍ୟା_INT|ସଂଖ୍ୟା_UNSIGNED))
	    ଫେରନ୍ତୁ -1;

#ifdef FEAT_GUI
	need_mouse_correct = TRUE;
#endif
	savewin = curwin;
	curwin = self->win;
	curbuf = curwin->w_buffer;

	VimTryStart();
	win_setwidth((ପୂର୍ଣ୍ଣସଂଖ୍ୟା) width);
	curwin = savewin;
	curbuf = curwin->w_buffer;
	ଯଦି (VimTryEnd())
	    ଫେରନ୍ତୁ -1;

	ଫେରନ୍ତୁ 0;
    }
    ଅନ୍ୟଥା
    {
	PyErr_SetString(PyExc_AttributeError, name);
	ଫେରନ୍ତୁ -1;
    }
}

    ସ୍ଥିର PyObject *
WindowRepr(PyObject *self_obj)
{
    WindowObject *self = (WindowObject*)self_obj;
    ଯଦି (self->win == INVALID_WINDOW_ମୂଲ୍ୟ)
	ଫେରନ୍ତୁ PyString_FromFormat("<window object (deleted) at %p>", (ଶୂନ୍ୟ *)self);
    ଅନ୍ୟଥା
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	w = get_win_ସଂଖ୍ୟା(self->win, firstwin);

	ଯଦି (w == 0)
	    ଫେରନ୍ତୁ PyString_FromFormat("<window object (unknown) at %p>",
								      (ଶୂନ୍ୟ *)self);
	ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ PyString_FromFormat("<window %d>", w - 1);
    }
}

ସ୍ଥିର ସଂରଚନା PyMethodDef WindowMethods[] = {
    // name,	    function,			calling,	documentation
    {"__dir__",	    (PyCFunction)WindowDir,	METH_NOARGS,	""},
    { ଖାଲି,	    ଖାଲି,			0,		ଖାଲି}
};

/*
 * Window list object
 */

DEFINE_PY_TYPE_OBJECT(WinListType);
ସ୍ଥିର PySequenceMethods WinListAsSeq;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    PyObject_HEAD
    TabPageObject	*tabObject;
} WinListObject;

ସ୍ଥିର WinListObject TheWindowList =
{
    PyObject_HEAD_INIT_TYPE(WinListType)
    ଖାଲି
};

    ସ୍ଥିର PyObject *
WinListNew(TabPageObject *tabObject)
{
    WinListObject	*self;

    self = PyObject_NEW(WinListObject, WinListTypePtr);
    self->tabObject = tabObject;
    Py_INCREF((PyObject *)tabObject);

    ଫେରନ୍ତୁ (PyObject *)(self);
}

    ସ୍ଥିର ଶୂନ୍ୟ
WinListDestructor(WinListObject *self)
{
    TabPageObject	*tabObject = self->tabObject;

    ଯଦି (tabObject)
    {
	Py_DECREF((PyObject *)(tabObject));
    }

    DESTRUCTOR_FINISH(self);
}

    ସ୍ଥିର PyInt
WinListLength(WinListObject *self)
{
    win_T	*w;
    PyInt	n = 0;

    ଯଦି (!(w = get_firstwin(self->tabObject)))
	ଫେରନ୍ତୁ -1;

    ଯେତେବେଳେ (w != ଖାଲି)
    {
	++n;
	w = W_NEXT(w);
    }

    ଫେରନ୍ତୁ n;
}

    ସ୍ଥିର PyObject *
WinListItem(WinListObject *self, PyInt n)
{
    win_T *w;

    ଯଦି (!(w = get_firstwin(self->tabObject)))
	ଫେରନ୍ତୁ ଖାଲି;

    ପାଇଁ (; w != ଖାଲି; w = W_NEXT(w), --n)
	ଯଦି (n == 0)
	    ଫେରନ୍ତୁ WindowNew(w, self->tabObject? self->tabObject->tab: curtab);

    PyErr_SET_STRING(PyExc_IndexError, N_("no such window"));
    ଫେରନ୍ତୁ ଖାଲି;
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
    ସ୍ଥିର ଅକ୍ଷର *
String_To_ଧାଡ଼ି(PyObject *obj)
{
    ଅକ୍ଷର	*str;
    ଅକ୍ଷର	*save;
    PyObject	*bytes = ଖାଲି;
    Py_ssize_t	len = 0;
    PyInt	i;
    ଅକ୍ଷର	*p;

    ଯଦି (PyBytes_Check(obj))
    {
	ଯଦି (PyBytes_AsStringAndSize(obj, &str, &len) == -1
		|| str == ଖାଲି)
	    ଫେରନ୍ତୁ ଖାଲି;
    }
    ଅନ୍ୟଥା ଯଦି (PyUnicode_Check(obj))
    {
	ଯଦି (!(bytes = PyUnicode_AsEncodedString(obj, ENC_OPT,
							   ERRORS_ENCODE_ARG)))
	    ଫେରନ୍ତୁ ଖାଲି;

	ଯଦି (PyBytes_AsStringAndSize(bytes, &str, &len) == -1
		|| str == ଖାଲି)
	{
	    Py_DECREF(bytes);
	    ଫେରନ୍ତୁ ଖାଲି;
	}
    }
    ଅନ୍ୟଥା
    {
#ଯଦି PY_MAJOR_VERSION < 3
	PyErr_FORMAT_TYPE(
		N_("expected str() or unicode() instance, but got %s"),
		obj);
#ଅନ୍ୟଥା
	PyErr_FORMAT_TYPE(
		N_("expected bytes() or str() instance, but got %s"),
		obj);
#endif
	ଫେରନ୍ତୁ ଖାଲି;
    }

    /*
     * Error checking: String must not contain newlines, as we
     * are replacing a single line, and we must replace it with
     * a single line.
     * A trailing newline is removed, so that append(f.readlines()) works.
     */
    p = memchr(str, '\n', len);
    ଯଦି (p != ଖାଲି)
    {
	ଯଦି (p == str + len - 1)
	    --len;
	ଅନ୍ୟଥା
	{
	    PyErr_SET_VIM(N_("string cannot contain newlines"));
	    Py_XDECREF(bytes);
	    ଫେରନ୍ତୁ ଖାଲି;
	}
    }

    /*
     * Create a copy of the string, with internal nulls replaced by
     * newline characters, as is the Vim convention.
     */
    save = alloc(len+1);
    ଯଦି (save == ଖାଲି)
    {
	PyErr_NoMemory();
	Py_XDECREF(bytes);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ପାଇଁ (i = 0; i < len; ++i)
    {
	ଯଦି (str[i] == '\0')
	    save[i] = '\n';
	ଅନ୍ୟଥା
	    save[i] = str[i];
    }

    save[i] = '\0';
    Py_XDECREF(bytes);  // Python 2 does nothing here

    ଫେରନ୍ତୁ save;
}

/*
 * Get a line from the specified buffer. The line number is
 * in Vim format (1-based). The line is returned as a Python
 * string object.
 */
    ସ୍ଥିର PyObject *
Get_Buffer_ଧାଡ଼ି(buf_T *buf, PyInt n)
{
    ଫେରନ୍ତୁ ଧାଡ଼ି_To_String((ଅକ୍ଷର *)ml_get_buf(buf, (linenr_T)n, FALSE));
}


/*
 * Get a list of lines from the specified buffer. The line numbers
 * are in Vim format (1-based). The range is from lo up to, but not
 * including, hi. The list is returned as a Python list of string objects.
 */
    ସ୍ଥିର PyObject *
Get_Buffer_ଧାଡ଼ି_List(buf_T *buf, PyInt lo, PyInt hi)
{
    PyInt	i;
    PyInt	n = hi - lo;
    PyObject	*list = PyList_New(n);

    ଯଦି (list == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ପାଇଁ (i = 0; i < n; ++i)
    {
	linenr_T	lnum = (linenr_T)(lo + i);
	ଅକ୍ଷର		*text;
	PyObject	*string;

	ଯଦି (lnum > buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	    text = "";
	ଅନ୍ୟଥା
	    text = (ଅକ୍ଷର *)ml_get_buf(buf, lnum, FALSE);
	string = ଧାଡ଼ି_To_String(text);
	ଯଦି (string == ଖାଲି)
	{
	    Py_DECREF(list);
	    ଫେରନ୍ତୁ ଖାଲି;
	}

	PyList_SET_ITEM(list, i, string);
    }

    // The ownership of the Python list is passed to the caller (ie,
    // the caller should Py_DECREF() the object when it is finished
    // with it).

    ଫେରନ୍ତୁ list;
}

/*
 * Check if deleting lines made the cursor position invalid.
 * Changed the lines from "lo" to "hi" and added "extra" lines (negative if
 * deleted).
 */
    ସ୍ଥିର ଶୂନ୍ୟ
py_fix_cursor(linenr_T lo, linenr_T hi, linenr_T extra)
{
    ଯଦି (curwin->w_cursor.lnum >= lo)
    {
	// Adjust the cursor position if it's in/after the changed
	// lines.
	ଯଦି (curwin->w_cursor.lnum >= hi)
	{
	    curwin->w_cursor.lnum += extra;
	    check_cursor_col();
	}
	ଅନ୍ୟଥା ଯଦି (extra < 0)
	{
	    curwin->w_cursor.lnum = lo;
	    check_cursor();
	}
	ଅନ୍ୟଥା
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
Set_Buffer_ଧାଡ଼ି(buf_T *buf, PyInt n, PyObject *ଧାଡ଼ି, PyInt *len_change)
{
    bufref_T	save_curbuf = {ଖାଲି, 0, 0};
    switchwin_T	switchwin;

    // First of all, we check the type of the supplied Python object.
    // There are three cases:
    //	  1. NULL, or None - this is a deletion.
    //	  2. A string	   - this is a replacement.
    //	  3. Anything else - this is an error.
    ଯଦି (ଧାଡ଼ି == Py_None || ଧାଡ଼ି == ଖାଲି)
    {
	PyErr_Clear();
	switchwin.sw_curwin = ଖାଲି;
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	VimTryStart();

	ଯଦି (u_savedel((linenr_T)n, 1L) == FAIL)
	    RAISE_UNDO_FAIL;
	ଅନ୍ୟଥା ଯଦି (ml_delete((linenr_T)n) == FAIL)
	    RAISE_DELETE_ଧାଡ଼ି_FAIL;
	ଅନ୍ୟଥା
	{
	    ଯଦି (buf == curbuf && (switchwin.sw_curwin != ଖାଲି
					   || save_curbuf.br_buf == ଖାଲି))
		// Using an existing window for the buffer, adjust the cursor
		// position.
		py_fix_cursor((linenr_T)n, (linenr_T)n + 1, (linenr_T)-1);
	    ଯଦି (save_curbuf.br_buf == ଖାଲି)
		// Only adjust marks if we managed to switch to a window that
		// holds the buffer, otherwise line numbers will be invalid.
		deleted_ଧାଡ଼ିଗୁଡ଼ିକ_mark((linenr_T)n, 1L);
	}

	restore_win_for_buf(&switchwin, &save_curbuf);

	ଯଦି (VimTryEnd())
	    ଫେରନ୍ତୁ FAIL;

	ଯଦି (len_change)
	    *len_change = -1;

	ଫେରନ୍ତୁ OK;
    }
    ଅନ୍ୟଥା ଯଦି (PyBytes_Check(ଧାଡ଼ି) || PyUnicode_Check(ଧାଡ଼ି))
    {
	ଅକ୍ଷର	*save = String_To_ଧାଡ଼ି(ଧାଡ଼ି);

	ଯଦି (save == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;

	VimTryStart();

	// We do not need to free "save" if ml_replace() consumes it.
	PyErr_Clear();
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	ଯଦି (u_savesub((linenr_T)n) == FAIL)
	{
	    RAISE_UNDO_FAIL;
	    vim_free(save);
	}
	ଅନ୍ୟଥା ଯଦି (ml_replace((linenr_T)n, (char_u *)save, FALSE) == FAIL)
	{
	    RAISE_REPLACE_ଧାଡ଼ି_FAIL;
	    vim_free(save);
	}
	ଅନ୍ୟଥା
	    changed_bytes((linenr_T)n, 0);

	restore_win_for_buf(&switchwin, &save_curbuf);

	// Check that the cursor is not beyond the end of the line now.
	ଯଦି (buf == curbuf)
	    check_cursor_col();

	ଯଦି (VimTryEnd())
	    ଫେରନ୍ତୁ FAIL;

	ଯଦି (len_change)
	    *len_change = 0;

	ଫେରନ୍ତୁ OK;
    }
    ଅନ୍ୟଥା
    {
	PyErr_BadArgument();
	ଫେରନ୍ତୁ FAIL;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
Set_Buffer_ଧାଡ଼ି_List(
	buf_T *buf,
	PyInt lo,
	PyInt hi,
	PyObject *list,
	PyInt *len_change)
{
    bufref_T	save_curbuf = {ଖାଲି, 0, 0};
    switchwin_T	switchwin;

    // First of all, we check the type of the supplied Python object.
    // There are three cases:
    //	  1. NULL, or None - this is a deletion.
    //	  2. A list	   - this is a replacement.
    //	  3. Anything else - this is an error.
    ଯଦି (list == Py_None || list == ଖାଲି)
    {
	PyInt	i;
	PyInt	n = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(hi - lo);

	PyErr_Clear();
	VimTryStart();
	switchwin.sw_curwin = ଖାଲି;
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	ଯଦି (u_savedel((linenr_T)lo, (long)n) == FAIL)
	    RAISE_UNDO_FAIL;
	ଅନ୍ୟଥା
	{
	    ପାଇଁ (i = 0; i < n; ++i)
	    {
		ଯଦି (ml_delete((linenr_T)lo) == FAIL)
		{
		    RAISE_DELETE_ଧାଡ଼ି_FAIL;
		    ଭାଙ୍ଗ;
		}
	    }
	    ଯଦି (buf == curbuf && (switchwin.sw_curwin != ଖାଲି
					       || save_curbuf.br_buf == ଖାଲି))
		// Using an existing window for the buffer, adjust the cursor
		// position.
		py_fix_cursor((linenr_T)lo, (linenr_T)hi, (linenr_T)-n);
	    ଯଦି (save_curbuf.br_buf == ଖାଲି)
		// Only adjust marks if we managed to switch to a window that
		// holds the buffer, otherwise line numbers will be invalid.
		deleted_ଧାଡ଼ିଗୁଡ଼ିକ_mark((linenr_T)lo, (long)i);
	}

	restore_win_for_buf(&switchwin, &save_curbuf);

	ଯଦି (VimTryEnd())
	    ଫେରନ୍ତୁ FAIL;

	ଯଦି (len_change)
	    *len_change = -n;

	ଫେରନ୍ତୁ OK;
    }
    ଅନ୍ୟଥା ଯଦି (PyList_Check(list))
    {
	PyInt	i;
	PyInt	new_len = PyList_Size(list);
	PyInt	old_len = hi - lo;
	PyInt	extra = 0;	// lines added to text, can be negative
	ଅକ୍ଷର	**array;

	ଯଦି (new_len == 0)	// avoid allocating zero bytes
	    array = ଖାଲି;
	ଅନ୍ୟଥା
	{
	    array = PyMem_New(ଅକ୍ଷର *, new_len);
	    ଯଦି (array == ଖାଲି)
	    {
		PyErr_NoMemory();
		ଫେରନ୍ତୁ FAIL;
	    }
	}

	ପାଇଁ (i = 0; i < new_len; ++i)
	{
	    PyObject *ଧାଡ଼ି;

	    ଯଦି (!(ଧାଡ଼ି = PyList_GetItem(list, i)) ||
		!(array[i] = String_To_ଧାଡ଼ି(ଧାଡ଼ି)))
	    {
		ଯେତେବେଳେ (i)
		    vim_free(array[--i]);
		PyMem_Free(array);
		ଫେରନ୍ତୁ FAIL;
	    }
	}

	VimTryStart();
	PyErr_Clear();

	// START of region without "return".  Must call restore_buffer()!
	switchwin.sw_curwin = ଖାଲି;
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	ଯଦି (u_save((linenr_T)(lo-1), (linenr_T)hi) == FAIL)
	    RAISE_UNDO_FAIL;

	// If the size of the range is reducing (ie, new_len < old_len) we
	// need to delete some old_len. We do this at the start, by
	// repeatedly deleting line "lo".
	ଯଦି (!PyErr_Occurred())
	{
	    ପାଇଁ (i = 0; i < old_len - new_len; ++i)
		ଯଦି (ml_delete((linenr_T)lo) == FAIL)
		{
		    RAISE_DELETE_ଧାଡ଼ି_FAIL;
		    ଭାଙ୍ଗ;
		}
	    extra -= i;
	}

	// For as long as possible, replace the existing old_len with the
	// new old_len. This is a more efficient operation, as it requires
	// less memory allocation and freeing.
	ଯଦି (!PyErr_Occurred())
	{
	    ପାଇଁ (i = 0; i < old_len && i < new_len; ++i)
		ଯଦି (ml_replace((linenr_T)(lo+i), (char_u *)array[i], FALSE)
								      == FAIL)
		{
		    RAISE_REPLACE_ଧାଡ଼ି_FAIL;
		    ଭାଙ୍ଗ;
		}
	}
	ଅନ୍ୟଥା
	    i = 0;

	// Now we may need to insert the remaining new old_len. If we do, we
	// must free the strings as we finish with them (we can't pass the
	// responsibility to Vim in this case).
	ଯଦି (!PyErr_Occurred())
	{
	    ଯେତେବେଳେ (i < new_len)
	    {
		ଯଦି (ml_append((linenr_T)(lo + i - 1),
					(char_u *)array[i], 0, FALSE) == FAIL)
		{
		    RAISE_ଭର୍ତ୍ତି_ଧାଡ଼ି_FAIL;
		    ଭାଙ୍ଗ;
		}
		vim_free(array[i]);
		++i;
		++extra;
	    }
	}

	// Free any left-over old_len, as a result of an error
	ଯେତେବେଳେ (i < new_len)
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
	ଯଦି (save_curbuf.br_buf == ଖାଲି)
	{
	    mark_adjust((linenr_T)lo, (linenr_T)(hi - 1),
						  (long)MAXLNUM, (long)extra);
	    changed_ଧାଡ଼ିଗୁଡ଼ିକ((linenr_T)lo, 0, (linenr_T)hi, (long)extra);
	}

	ଯଦି (buf == curbuf && (switchwin.sw_curwin != ଖାଲି
					   || save_curbuf.br_buf == ଖାଲି))
	    // Using an existing window for the buffer, adjust the cursor
	    // position.
	    py_fix_cursor((linenr_T)lo, (linenr_T)hi, (linenr_T)extra);

	// END of region without "return".
	restore_win_for_buf(&switchwin, &save_curbuf);

	ଯଦି (VimTryEnd())
	    ଫେରନ୍ତୁ FAIL;

	ଯଦି (len_change)
	    *len_change = new_len - old_len;

	ଫେରନ୍ତୁ OK;
    }
    ଅନ୍ୟଥା
    {
	PyErr_BadArgument();
	ଫେରନ୍ତୁ FAIL;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ଭର୍ତ୍ତି_Buffer_ଧାଡ଼ିଗୁଡ଼ିକ(buf_T *buf, PyInt n, PyObject *ଧାଡ଼ିଗୁଡ଼ିକ, PyInt *len_change)
{
    bufref_T	save_curbuf = {ଖାଲି, 0, 0};
    switchwin_T	switchwin;

    // First of all, we check the type of the supplied Python object.
    // It must be a string or a list, or the call is in error.
    ଯଦି (PyBytes_Check(ଧାଡ଼ିଗୁଡ଼ିକ) || PyUnicode_Check(ଧାଡ଼ିଗୁଡ଼ିକ))
    {
	ଅକ୍ଷର		*str = String_To_ଧାଡ଼ି(ଧାଡ଼ିଗୁଡ଼ିକ);

	ଯଦି (str == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;

	PyErr_Clear();
	VimTryStart();
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	ଯଦି (u_save((linenr_T)n, (linenr_T)(n + 1)) == FAIL)
	    RAISE_UNDO_FAIL;
	ଅନ୍ୟଥା ଯଦି (ml_append((linenr_T)n, (char_u *)str, 0, FALSE) == FAIL)
	    RAISE_ଭର୍ତ୍ତି_ଧାଡ଼ି_FAIL;
	ଅନ୍ୟଥା ଯଦି (save_curbuf.br_buf == ଖାଲି)
	    // Only adjust marks if we managed to switch to a window that
	    // holds the buffer, otherwise line numbers will be invalid.
	    appended_ଧାଡ଼ିଗୁଡ଼ିକ_mark((linenr_T)n, 1L);

	vim_free(str);
	restore_win_for_buf(&switchwin, &save_curbuf);
	update_screen(UPD_VALID);

	ଯଦି (VimTryEnd())
	    ଫେରନ୍ତୁ FAIL;

	ଯଦି (len_change)
	    *len_change = 1;

	ଫେରନ୍ତୁ OK;
    }
    ଅନ୍ୟଥା ଯଦି (PyList_Check(ଧାଡ଼ିଗୁଡ଼ିକ))
    {
	PyInt	i;
	PyInt	size = PyList_Size(ଧାଡ଼ିଗୁଡ଼ିକ);
	ଅକ୍ଷର	**array;

	array = PyMem_New(ଅକ୍ଷର *, size);
	ଯଦି (array == ଖାଲି)
	{
	    PyErr_NoMemory();
	    ଫେରନ୍ତୁ FAIL;
	}

	ପାଇଁ (i = 0; i < size; ++i)
	{
	    PyObject *ଧାଡ଼ି;

	    ଯଦି (!(ଧାଡ଼ି = PyList_GetItem(ଧାଡ଼ିଗୁଡ଼ିକ, i)) ||
		!(array[i] = String_To_ଧାଡ଼ି(ଧାଡ଼ି)))
	    {
		ଯେତେବେଳେ (i)
		    vim_free(array[--i]);
		PyMem_Free(array);
		ଫେରନ୍ତୁ FAIL;
	    }
	}

	PyErr_Clear();
	VimTryStart();
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	ଯଦି (u_save((linenr_T)n, (linenr_T)(n + 1)) == FAIL)
	    RAISE_UNDO_FAIL;
	ଅନ୍ୟଥା
	{
	    ପାଇଁ (i = 0; i < size; ++i)
	    {
		ଯଦି (ml_append((linenr_T)(n + i),
					(char_u *)array[i], 0, FALSE) == FAIL)
		{
		    RAISE_ଭର୍ତ୍ତି_ଧାଡ଼ି_FAIL;

		    // Free the rest of the lines
		    ଯେତେବେଳେ (i < size)
			vim_free(array[i++]);

		    ଭାଙ୍ଗ;
		}
		vim_free(array[i]);
	    }
	    ଯଦି (i > 0 && save_curbuf.br_buf == ଖାଲି)
		// Only adjust marks if we managed to switch to a window that
		// holds the buffer, otherwise line numbers will be invalid.
		appended_ଧାଡ଼ିଗୁଡ଼ିକ_mark((linenr_T)n, (long)i);
	}

	// Free the array of lines. All of its contents have now
	// been freed.
	PyMem_Free(array);
	restore_win_for_buf(&switchwin, &save_curbuf);

	update_screen(UPD_VALID);

	ଯଦି (VimTryEnd())
	    ଫେରନ୍ତୁ FAIL;

	ଯଦି (len_change)
	    *len_change = size;

	ଫେରନ୍ତୁ OK;
    }
    ଅନ୍ୟଥା
    {
	PyErr_BadArgument();
	ଫେରନ୍ତୁ FAIL;
    }
}

/*
 * Common routines for buffers and line ranges
 * -------------------------------------------
 */

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    PyObject_HEAD
    buf_T *buf;
} BufferObject;

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
CheckBuffer(BufferObject *self)
{
    ଯଦି (self->buf == INVALID_BUFFER_ମୂଲ୍ୟ)
    {
	PyErr_SET_VIM(N_("attempt to refer to deleted buffer"));
	ଫେରନ୍ତୁ -1;
    }

    ଫେରନ୍ତୁ 0;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
CheckBufferCb(ଶୂନ୍ୟ *self)
{
    ଫେରନ୍ତୁ CheckBuffer((BufferObject*)self);
}

    ସ୍ଥିର PyObject *
RBItem(BufferObject *self, PyInt n, PyInt start, PyInt end)
{
    ଯଦି (CheckBuffer(self))
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (end == -1)
	end = self->buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;

    ଯଦି (n < 0)
	n += end - start + 1;

    ଯଦି (n < 0 || n > end - start)
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("line number out of range"));
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ଫେରନ୍ତୁ Get_Buffer_ଧାଡ଼ି(self->buf, n+start);
}

    ସ୍ଥିର PyObject *
RBSlice(BufferObject *self, PyInt lo, PyInt hi, PyInt start, PyInt end)
{
    PyInt size;

    ଯଦି (CheckBuffer(self))
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (end == -1)
	end = self->buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;

    size = end - start + 1;

    ଯଦି (lo < 0)
	lo = 0;
    ଅନ୍ୟଥା ଯଦି (lo > size)
	lo = size;
    ଯଦି (hi < 0)
	hi = 0;
    ଯଦି (hi < lo)
	hi = lo;
    ଅନ୍ୟଥା ଯଦି (hi > size)
	hi = size;

    ଫେରନ୍ତୁ Get_Buffer_ଧାଡ଼ି_List(self->buf, lo+start, hi+start);
}

    ସ୍ଥିର PyInt
RBAsItem(
	BufferObject *self,
	PyInt n,
	PyObject *valObject,
	PyInt start,
	PyInt end,
	PyInt *new_end)
{
    PyInt len_change;

    ଯଦି (CheckBuffer(self))
	ଫେରନ୍ତୁ -1;

    ଯଦି (end == -1)
	end = self->buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;

    ଯଦି (n < 0)
	n += end - start + 1;

    ଯଦି (n < 0 || n > end - start)
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("line number out of range"));
	ଫେରନ୍ତୁ -1;
    }

    ଯଦି (Set_Buffer_ଧାଡ଼ି(self->buf, n+start, valObject, &len_change) == FAIL)
	ଫେରନ୍ତୁ -1;

    ଯଦି (new_end)
	*new_end = end + len_change;

    ଫେରନ୍ତୁ 0;
}

    ସ୍ଥିର PyInt
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
    ଯଦି (CheckBuffer(self))
	ଫେରନ୍ତୁ -1;

    ଯଦି (end == -1)
	end = self->buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;

    // Sort out the slice range
    size = end - start + 1;

    ଯଦି (lo < 0)
	lo = 0;
    ଅନ୍ୟଥା ଯଦି (lo > size)
	lo = size;
    ଯଦି (hi < 0)
	hi = 0;
    ଯଦି (hi < lo)
	hi = lo;
    ଅନ୍ୟଥା ଯଦି (hi > size)
	hi = size;

    ଯଦି (Set_Buffer_ଧାଡ଼ି_List(self->buf, lo + start, hi + start,
						valObject, &len_change) == FAIL)
	ଫେରନ୍ତୁ -1;

    ଯଦି (new_end)
	*new_end = end + len_change;

    ଫେରନ୍ତୁ 0;
}


    ସ୍ଥିର PyObject *
RBAppend(
	BufferObject *self,
	PyObject *args,
	PyInt start,
	PyInt end,
	PyInt *new_end)
{
    PyObject *ଧାଡ଼ିଗୁଡ଼ିକ;
    PyInt len_change;
    PyInt max;
    PyInt n;

    ଯଦି (CheckBuffer(self))
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (end == -1)
	end = self->buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;

    max = n = end - start + 1;

    ଯଦି (!PyArg_ParseTuple(args, "O|n", &ଧାଡ଼ିଗୁଡ଼ିକ, &n))
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (n < 0 || n > max)
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("line number out of range"));
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ଯଦି (ଭର୍ତ୍ତି_Buffer_ଧାଡ଼ିଗୁଡ଼ିକ(self->buf, n + start - 1, ଧାଡ଼ିଗୁଡ଼ିକ, &len_change) == FAIL)
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (new_end)
	*new_end = end + len_change;

    Py_INCREF(Py_None);
    ଫେରନ୍ତୁ Py_None;
}

// Range object

DEFINE_PY_TYPE_OBJECT(RangeType);
ସ୍ଥିର PySequenceMethods RangeAsSeq;
ସ୍ଥିର PyMappingMethods RangeAsMapping;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    PyObject_HEAD
    BufferObject *buf;
    PyInt start;
    PyInt end;
} RangeObject;

    ସ୍ଥିର PyObject *
RangeNew(buf_T *buf, PyInt start, PyInt end)
{
    BufferObject *bufr;
    RangeObject *self;
    self = PyObject_GC_New(RangeObject, RangeTypePtr);
    ଯଦି (self == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    bufr = (BufferObject *)BufferNew(buf);
    ଯଦି (bufr == ଖାଲି)
    {
	Py_DECREF(self);
	ଫେରନ୍ତୁ ଖାଲି;
    }
    Py_INCREF((PyObject *)bufr);

    self->buf = bufr;
    self->start = start;
    self->end = end;

    ଫେରନ୍ତୁ (PyObject *)(self);
}

    ସ୍ଥିର ଶୂନ୍ୟ
RangeDestructor(PyObject *self_obj)
{
    RangeObject *self = (RangeObject*)self_obj;
    PyObject_GC_UnTrack((ଶୂନ୍ୟ *)(self));
    Py_XDECREF(((PyObject *)(self->buf)));
    PyObject_GC_Del((ଶୂନ୍ୟ *)(self));
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
RangeTraverse(PyObject *self_obj, visitproc visit, ଶୂନ୍ୟ *arg)
{
    RangeObject *self = (RangeObject*)self_obj;
    Py_VISIT(((PyObject *)(self->buf)));
    ଫେରନ୍ତୁ 0;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
RangeClear(PyObject *self_obj)
{
    RangeObject *self = (RangeObject*)self_obj;
    Py_CLEAR(self->buf);
    ଫେରନ୍ତୁ 0;
}

    ସ୍ଥିର PyInt
RangeLength(RangeObject *self)
{
    // HOW DO WE SIGNAL AN ERROR FROM THIS FUNCTION?
    ଯଦି (CheckBuffer(self->buf))
	ଫେରନ୍ତୁ -1; // ???

    ଫେରନ୍ତୁ (self->end - self->start + 1);
}

    ସ୍ଥିର PyObject *
RangeItem(RangeObject *self, PyInt n)
{
    ଫେରନ୍ତୁ RBItem(self->buf, n, self->start, self->end);
}

    ସ୍ଥିର PyObject *
RangeSlice(RangeObject *self, PyInt lo, PyInt hi)
{
    ଫେରନ୍ତୁ RBSlice(self->buf, lo, hi, self->start, self->end);
}

ସ୍ଥିର ଅକ୍ଷର *RangeAttrs[] = {
    "start", "end",
    ଖାଲି
};

    ସ୍ଥିର PyObject *
RangeDir(PyObject *self, PyObject *args UNUSED)
{
    ଫେରନ୍ତୁ ObjectDir(self, RangeAttrs);
}

    ସ୍ଥିର PyObject *
RangeAppend(RangeObject *self, PyObject *args)
{
    ଫେରନ୍ତୁ RBAppend(self->buf, args, self->start, self->end, &self->end);
}

    ସ୍ଥିର PyObject *
RangeRepr(PyObject *self_obj)
{
    RangeObject *self = (RangeObject*)self_obj;
    ଯଦି (self->buf->buf == INVALID_BUFFER_ମୂଲ୍ୟ)
	ଫେରନ୍ତୁ PyString_FromFormat("<range object (for deleted buffer) at %p>",
				    (ଶୂନ୍ୟ *)self);
    ଅନ୍ୟଥା
    {
	ଅକ୍ଷର *name = (ଅକ୍ଷର *)self->buf->buf->b_fname;

	ଯଦି (name == ଖାଲି)
	    name = "";

	ଫେରନ୍ତୁ PyString_FromFormat("<range %s (%d:%d)>",
				    name, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)self->start, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)self->end);
    }
}

ସ୍ଥିର ସଂରଚନା PyMethodDef RangeMethods[] = {
    // name,	function,			calling,	documentation
    {"append",	(PyCFunction)RangeAppend,	METH_VARARGS,	"Append data to the Vim range" },
    {"__dir__",	(PyCFunction)RangeDir,		METH_NOARGS,	""},
    { ଖାଲି,	ଖାଲି,				0,		ଖାଲି}
};

DEFINE_PY_TYPE_OBJECT(BufferType);
ସ୍ଥିର PySequenceMethods BufferAsSeq;
ସ୍ଥିର PyMappingMethods BufferAsMapping;

    ସ୍ଥିର PyObject *
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

    ଯଦି (BUF_PYTHON_REF(buf) != ଖାଲି)
    {
	self = BUF_PYTHON_REF(buf);
	Py_INCREF((PyObject *)self);
    }
    ଅନ୍ୟଥା
    {
	self = PyObject_NEW(BufferObject, BufferTypePtr);
	ଯଦି (self == ଖାଲି)
	    ଫେରନ୍ତୁ ଖାଲି;
	self->buf = buf;
	BUF_PYTHON_REF(buf) = self;
    }

    ଫେରନ୍ତୁ (PyObject *)(self);
}

    ସ୍ଥିର ଶୂନ୍ୟ
BufferDestructor(PyObject *self_obj)
{
    BufferObject *self = (BufferObject*)self_obj;
    ଯଦି (self->buf && self->buf != INVALID_BUFFER_ମୂଲ୍ୟ)
	BUF_PYTHON_REF(self->buf) = ଖାଲି;

    DESTRUCTOR_FINISH(self);
}

    ସ୍ଥିର PyInt
BufferLength(BufferObject *self)
{
    // HOW DO WE SIGNAL AN ERROR FROM THIS FUNCTION?
    ଯଦି (CheckBuffer(self))
	ଫେରନ୍ତୁ -1; // ???

    ଫେରନ୍ତୁ (PyInt)(self->buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା);
}

    ସ୍ଥିର PyObject *
BufferItem(BufferObject *self, PyInt n)
{
    ଫେରନ୍ତୁ RBItem(self, n, 1, -1);
}

    ସ୍ଥିର PyObject *
BufferSlice(BufferObject *self, PyInt lo, PyInt hi)
{
    ଫେରନ୍ତୁ RBSlice(self, lo, hi, 1, -1);
}

ସ୍ଥିର ଅକ୍ଷର *BufferAttrs[] = {
    "name", "number", "vars", "options", "valid",
    ଖାଲି
};

    ସ୍ଥିର PyObject *
BufferDir(PyObject *self, PyObject *args UNUSED)
{
    ଫେରନ୍ତୁ ObjectDir(self, BufferAttrs);
}

    ସ୍ଥିର PyObject *
BufferAttrValid(BufferObject *self, ଅକ୍ଷର *name)
{
    PyObject	*ret;

    ଯଦି (strcmp(name, "valid") != 0)
	ଫେରନ୍ତୁ ଖାଲି;

    ret = ((self->buf == INVALID_BUFFER_ମୂଲ୍ୟ) ? Py_False : Py_True);
    Py_INCREF(ret);
    ଫେରନ୍ତୁ ret;
}

    ସ୍ଥିର PyObject *
BufferAttr(BufferObject *self, ଅକ୍ଷର *name)
{
    ଯଦି (strcmp(name, "name") == 0)
	ଫେରନ୍ତୁ PyString_FromString((self->buf->b_ffname == ଖାଲି
				    ? "" : (ଅକ୍ଷର *)self->buf->b_ffname));
    ଅନ୍ୟଥା ଯଦି (strcmp(name, "number") == 0)
	ଫେରନ୍ତୁ Py_Build_ମୂଲ୍ୟ(Py_ssize_t_fmt, self->buf->b_fnum);
    ଅନ୍ୟଥା ଯଦି (strcmp(name, "vars") == 0)
	ଫେରନ୍ତୁ NEW_DICTIONARY(self->buf->b_vars);
    ଅନ୍ୟଥା ଯଦି (strcmp(name, "options") == 0)
	ଫେରନ୍ତୁ OptionsNew(SREQ_BUF, self->buf, CheckBufferCb,
			(PyObject *) self);
    ଅନ୍ୟଥା ଯଦି (strcmp(name, "__members__") == 0)
	ଫେରନ୍ତୁ ObjectDir(ଖାଲି, BufferAttrs);
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ ଖାଲି;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
BufferSetattr(PyObject *self_obj, ଅକ୍ଷର *name, PyObject *valObject)
{
    BufferObject *self = (BufferObject*)self_obj;
    ଯଦି (CheckBuffer(self))
	ଫେରନ୍ତୁ -1;

    ଯଦି (strcmp(name, "name") == 0)
    {
	char_u		*val;
	aco_save_T	aco;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ren_ret = OK;
	PyObject	*todecref;

	ଯଦି (!(val = StringToChars(valObject, &todecref)))
	    ଫେରନ୍ତୁ -1;

	VimTryStart();
	// Using aucmd_*: autocommands will be executed by rename_buffer
	aucmd_prepbuf(&aco, self->buf);
	ଯଦି (curbuf == self->buf)
	{
	    ren_ret = rename_buffer(val);
	    aucmd_restbuf(&aco);
	}
	Py_XDECREF(todecref);
	ଯଦି (VimTryEnd())
	    ଫେରନ୍ତୁ -1;

	ଯଦି (ren_ret == FAIL)
	{
	    PyErr_SET_VIM(N_("failed to rename buffer"));
	    ଫେରନ୍ତୁ -1;
	}
	ଫେରନ୍ତୁ 0;
    }
    ଅନ୍ୟଥା
    {
	PyErr_SetString(PyExc_AttributeError, name);
	ଫେରନ୍ତୁ -1;
    }
}

    ସ୍ଥିର PyObject *
BufferAppend(BufferObject *self, PyObject *args)
{
    ଫେରନ୍ତୁ RBAppend(self, args, 1, -1, ଖାଲି);
}

    ସ୍ଥିର PyObject *
BufferMark(BufferObject *self, PyObject *pmarkObject)
{
    pos_T	*posp;
    char_u	*pmark;
    char_u	mark;
    bufref_T	savebuf;
    PyObject	*todecref;

    ଯଦି (CheckBuffer(self))
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (!(pmark = StringToChars(pmarkObject, &todecref)))
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (pmark[0] == '\0' || pmark[1] != '\0')
    {
	PyErr_SET_STRING(Py_Exc_ମୂଲ୍ୟ_Error,
		N_("mark name must be a single character"));
	Py_XDECREF(todecref);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    mark = *pmark;

    Py_XDECREF(todecref);

    VimTryStart();
    switch_buffer(&savebuf, self->buf);
    posp = getmark(mark, FALSE);
    restore_buffer(&savebuf);
    ଯଦି (VimTryEnd())
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (posp == ଖାଲି)
    {
	PyErr_SET_VIM(N_("invalid mark name"));
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ଯଦି (posp->lnum <= 0)
    {
	// Or raise an error?
	Py_INCREF(Py_None);
	ଫେରନ୍ତୁ Py_None;
    }

    ଫେରନ୍ତୁ Py_Build_ମୂଲ୍ୟ("(ll)", (long)(posp->lnum), (long)(posp->col));
}

    ସ୍ଥିର PyObject *
BufferRange(BufferObject *self, PyObject *args)
{
    PyInt start;
    PyInt end;

    ଯଦି (CheckBuffer(self))
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (!PyArg_ParseTuple(args, "nn", &start, &end))
	ଫେରନ୍ତୁ ଖାଲି;

    ଫେରନ୍ତୁ RangeNew(self->buf, start, end);
}

    ସ୍ଥିର PyObject *
BufferRepr(PyObject *self_obj)
{
    BufferObject *self = (BufferObject*)self_obj;
    ଯଦି (self->buf == INVALID_BUFFER_ମୂଲ୍ୟ)
	ଫେରନ୍ତୁ PyString_FromFormat("<buffer object (deleted) at %p>", (ଶୂନ୍ୟ *)self);
    ଅନ୍ୟଥା
    {
	ଅକ୍ଷର	*name = (ଅକ୍ଷର *)self->buf->b_fname;

	ଯଦି (name == ଖାଲି)
	    name = "";

	ଫେରନ୍ତୁ PyString_FromFormat("<buffer %s>", name);
    }
}

ସ୍ଥିର ସଂରଚନା PyMethodDef BufferMethods[] = {
    // name,	    function,			calling,	documentation
    {"append",	    (PyCFunction)BufferAppend,	METH_VARARGS,	"Append data to Vim buffer" },
    {"mark",	    (PyCFunction)BufferMark,	METH_O,		"Return (row,col) representing position of named mark" },
    {"range",	    (PyCFunction)BufferRange,	METH_VARARGS,	"Return a range object which represents the part of the given buffer between line numbers s and e" },
    {"__dir__",	    (PyCFunction)BufferDir,	METH_NOARGS,	""},
    { ଖାଲି,	    ଖାଲି,			0,		ଖାଲି}
};

/*
 * Buffer list object - Implementation
 */

DEFINE_PY_TYPE_OBJECT(BufMapType);

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    PyObject_HEAD
} BufMapObject;

ସ୍ଥିର BufMapObject TheBufferMap =
{
    PyObject_HEAD_INIT_TYPE(BufMapType)
};

    ସ୍ଥିର PyInt
BufMapLength(PyObject *self UNUSED)
{
    buf_T	*b = firstbuf;
    PyInt	n = 0;

    ଯେତେବେଳେ (b)
    {
	++n;
	b = b->b_next;
    }

    ଫେରନ୍ତୁ n;
}

    ସ୍ଥିର PyObject *
BufMapItem(PyObject *self UNUSED, PyObject *ଚାବି_Object)
{
    buf_T	*b;
    long	bnr;

    ଯଦି (ସଂଖ୍ୟା_To_Long(ଚାବି_Object, &bnr, ସଂଖ୍ୟା_INT|ସଂଖ୍ୟା_NATURAL))
	ଫେରନ୍ତୁ ଖାଲି;

    b = buflist_findnr((ପୂର୍ଣ୍ଣସଂଖ୍ୟା) bnr);

    ଯଦି (b)
	ଫେରନ୍ତୁ BufferNew(b);
    ଅନ୍ୟଥା
    {
	PyErr_SetObject(Py_Exc_ଚାବି_Error, ଚାବି_Object);
	ଫେରନ୍ତୁ ଖାଲି;
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
BufMapIterDestruct(ଶୂନ୍ୟ* arg)
{
    PyObject *buffer = (PyObject*)arg;
    // Iteration was stopped before all buffers were processed
    ଯଦି (buffer)
    {
	Py_DECREF(buffer);
    }
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
BufMapIterTraverse(ଶୂନ୍ୟ *iter, visitproc visit, ଶୂନ୍ୟ *arg)
{
    PyObject *buffer = (PyObject*)iter;
    ଯଦି (buffer)
	Py_VISIT(buffer);
    ଫେରନ୍ତୁ 0;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
BufMapIterClear(ଶୂନ୍ୟ **iter)
{
    PyObject **buffer = (PyObject**)iter;
    ଯଦି (*buffer)
	Py_CLEAR(*buffer);
    ଫେରନ୍ତୁ 0;
}

    ସ୍ଥିର PyObject *
BufMapIterNext(ଶୂନ୍ୟ **arg)
{
    PyObject	*next;
    PyObject	*ret;
    PyObject	**buffer = (PyObject**)arg;

    ଯଦି (!*buffer)
	ଫେରନ୍ତୁ ଖାଲି;

    ret = *buffer;

    ଯଦି (CheckBuffer((BufferObject *)(ret)))
    {
	*buffer = ଖାଲି;
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ଯଦି (!((BufferObject *)(ret))->buf->b_next)
	next = ଖାଲି;
    ଅନ୍ୟଥା ଯଦି (!(next = BufferNew(((BufferObject *)(ret))->buf->b_next)))
	ଫେରନ୍ତୁ ଖାଲି;
    *buffer = next;
    // Do not increment reference: we no longer hold it (decref), but whoever
    // on other side will hold (incref). Decref+incref = nothing.
    ଫେରନ୍ତୁ ret;
}

    ସ୍ଥିର PyObject *
BufMapIter(PyObject *self)
{
    PyObject *buffer;

    buffer = BufferNew(firstbuf);
    ଫେରନ୍ତୁ IterNew(buffer,
	    BufMapIterDestruct, BufMapIterNext,
	    BufMapIterTraverse, BufMapIterClear,
	    self);
}

ସ୍ଥିର PyMappingMethods BufMapAsMapping = {
    (lenfunc)       BufMapLength,
    (binaryfunc)    BufMapItem,
    (objobjargproc) 0,
};

// Current items object

ସ୍ଥିର ଅକ୍ଷର *CurrentAttrs[] = {
    "buffer", "window", "line", "range", "tabpage",
    ଖାଲି
};

    ସ୍ଥିର PyObject *
CurrentDir(PyObject *self, PyObject *args UNUSED)
{
    ଫେରନ୍ତୁ ObjectDir(self, CurrentAttrs);
}

    ସ୍ଥିର PyObject *
CurrentGetattr(PyObject *self UNUSED, ଅକ୍ଷର *name)
{
    ଯଦି (strcmp(name, "buffer") == 0)
	ଫେରନ୍ତୁ (PyObject *)BufferNew(curbuf);
    ଅନ୍ୟଥା ଯଦି (strcmp(name, "window") == 0)
	ଫେରନ୍ତୁ (PyObject *)WindowNew(curwin, curtab);
    ଅନ୍ୟଥା ଯଦି (strcmp(name, "tabpage") == 0)
	ଫେରନ୍ତୁ (PyObject *)TabPageNew(curtab);
    ଅନ୍ୟଥା ଯଦି (strcmp(name, "line") == 0)
	ଫେରନ୍ତୁ Get_Buffer_ଧାଡ଼ି(curbuf, (PyInt)curwin->w_cursor.lnum);
    ଅନ୍ୟଥା ଯଦି (strcmp(name, "range") == 0)
	ଫେରନ୍ତୁ RangeNew(curbuf, RangeStart, RangeEnd);
    ଅନ୍ୟଥା ଯଦି (strcmp(name, "__members__") == 0)
	ଫେରନ୍ତୁ ObjectDir(ଖାଲି, CurrentAttrs);
    ଅନ୍ୟଥା
#ଯଦି PY_MAJOR_VERSION < 3
	ଫେରନ୍ତୁ Py_FindMethod(WindowMethods, self, name);
#ଅନ୍ୟଥା
	ଫେରନ୍ତୁ ଖାଲି;
#endif
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
CurrentSetattr(PyObject *self UNUSED, ଅକ୍ଷର *name, PyObject *valObject)
{
    ଯଦି (strcmp(name, "line") == 0)
    {
	ଯଦି (Set_Buffer_ଧାଡ଼ି(curbuf, (PyInt)curwin->w_cursor.lnum, valObject,
			  ଖାଲି) == FAIL)
	    ଫେରନ୍ତୁ -1;

	ଫେରନ୍ତୁ 0;
    }
    ଅନ୍ୟଥା ଯଦି (strcmp(name, "buffer") == 0)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2;

	ଯଦି (valObject->ob_type != BufferTypePtr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.Buffer object, but got %s"),
		    valObject);
	    ଫେରନ୍ତୁ -1;
	}

	ଯଦି (CheckBuffer((BufferObject *)(valObject)))
	    ଫେରନ୍ତୁ -1;
	ଗଣନା_2 = ((BufferObject *)(valObject))->buf->b_fnum;

	VimTryStart();
	ଯଦି (do_buffer(DOBUF_GOTO, DOBUF_FIRST, FORWARD, ଗଣନା_2, 0) == FAIL)
	{
	    ଯଦି (VimTryEnd())
		ଫେରନ୍ତୁ -1;
	    PyErr_VIM_FORMAT(N_("failed to switch to buffer %d"), ଗଣନା_2);
	    ଫେରନ୍ତୁ -1;
	}

	ଫେରନ୍ତୁ VimTryEnd();
    }
    ଅନ୍ୟଥା ଯଦି (strcmp(name, "window") == 0)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2;

	ଯଦି (valObject->ob_type != WindowTypePtr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.Window object, but got %s"),
		    valObject);
	    ଫେରନ୍ତୁ -1;
	}

	ଯଦି (CheckWindow((WindowObject *)(valObject)))
	    ଫେରନ୍ତୁ -1;
	ଗଣନା_2 = get_win_ସଂଖ୍ୟା(((WindowObject *)(valObject))->win, firstwin);

	ଯଦି (!ଗଣନା_2)
	{
	    PyErr_SET_STRING(Py_Exc_ମୂଲ୍ୟ_Error,
		    N_("failed to find window in the current tab page"));
	    ଫେରନ୍ତୁ -1;
	}

	VimTryStart();
	win_goto(((WindowObject *)(valObject))->win);
	ଯଦି (((WindowObject *)(valObject))->win != curwin)
	{
	    ଯଦି (VimTryEnd())
		ଫେରନ୍ତୁ -1;
	    PyErr_SET_STRING(PyExc_RuntimeError,
		    N_("did not switch to the specified window"));
	    ଫେରନ୍ତୁ -1;
	}

	ଫେରନ୍ତୁ VimTryEnd();
    }
    ଅନ୍ୟଥା ଯଦି (strcmp(name, "tabpage") == 0)
    {
	ଯଦି (valObject->ob_type != TabPageTypePtr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.TabPage object, but got %s"),
		    valObject);
	    ଫେରନ୍ତୁ -1;
	}

	ଯଦି (CheckTabPage((TabPageObject *)(valObject)))
	    ଫେରନ୍ତୁ -1;

	VimTryStart();
	goto_tabpage_tp(((TabPageObject *)(valObject))->tab, TRUE, TRUE);
	ଯଦି (((TabPageObject *)(valObject))->tab != curtab)
	{
	    ଯଦି (VimTryEnd())
		ଫେରନ୍ତୁ -1;
	    PyErr_SET_STRING(PyExc_RuntimeError,
		    N_("did not switch to the specified tab page"));
	    ଫେରନ୍ତୁ -1;
	}

	ଫେରନ୍ତୁ VimTryEnd();
    }
    ଅନ୍ୟଥା
    {
	PyErr_SetString(PyExc_AttributeError, name);
	ଫେରନ୍ତୁ -1;
    }
}

ସ୍ଥିର ସଂରଚନା PyMethodDef CurrentMethods[] = {
    // name,	    function,			calling,	documentation
    {"__dir__",	    (PyCFunction)CurrentDir,	METH_NOARGS,	""},
    { ଖାଲି,	    ଖାଲି,			0,		ଖାଲି}
};

    ସ୍ଥିର ଶୂନ୍ୟ
init_range_cmd(ଶୂନ୍ୟ *arg)
{
    exarg_T *eap = (exarg_T*)arg;
    RangeStart = eap->line1;
    RangeEnd = eap->line2;
}

    ସ୍ଥିର ଶୂନ୍ୟ
init_range_eval(ଶୂନ୍ୟ *rettv UNUSED)
{
    RangeStart = (PyInt) curwin->w_cursor.lnum;
    RangeEnd = RangeStart;
}

    ସ୍ଥିର ଶୂନ୍ୟ
run_cmd(ଅଚଳ ଅକ୍ଷର *cmd, dict_T* locals UNUSED, ଶୂନ୍ୟ *arg UNUSED
#ifdef PY_CAN_RECURSE
	, PyGILState_STATE *pygilstate UNUSED
#endif
	)
{
    PyObject	*run_ret;
    run_ret = PyRun_String((ଅକ୍ଷର *)cmd, Py_file_ନିବେଶ, globals, globals);
    ଯଦି (run_ret != ଖାଲି)
    {
	Py_DECREF(run_ret);
    }
    ଅନ୍ୟଥା ଯଦି (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
    {
	emsg(_(e_cant_handle_systemexit_of_python_exception_in_vim));
	PyErr_Clear();
    }
    ଅନ୍ୟଥା
	PyErr_PrintEx(1);
}

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର	*code_hdr = "def " DOPY_FUNC "(line, linenr):\n ";
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା		code_hdr_len = 30;

    ସ୍ଥିର ଶୂନ୍ୟ
run_do(ଅଚଳ ଅକ୍ଷର *cmd, dict_T* locals UNUSED, ଶୂନ୍ୟ *arg UNUSED
#ifdef PY_CAN_RECURSE
	, PyGILState_STATE *pygilstate
#endif
	)
{
    PyInt	lnum;
    size_t	len;
    ଅକ୍ଷର	*code;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		status;
    PyObject	*pyfunc, *pymain;
    PyObject	*run_ret;
    buf_T	*was_curbuf = curbuf;

    ଯଦି (u_save((linenr_T)RangeStart - 1, (linenr_T)RangeEnd + 1) != OK)
    {
	emsg(_("cannot save undo information"));
	ଫେରନ୍ତୁ;
    }

    len = code_hdr_len + STRLEN(cmd);
    code = PyMem_New(ଅକ୍ଷର, len + 1);
    memcpy(code, code_hdr, code_hdr_len);
    STRCPY(code + code_hdr_len, cmd);
    run_ret = PyRun_String(code, Py_file_ନିବେଶ, globals, globals);
    status = -1;
    ଯଦି (run_ret != ଖାଲି)
    {
	status = 0;
	Py_DECREF(run_ret);
    }
    ଅନ୍ୟଥା ଯଦି (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
    {
	PyMem_Free(code);
	emsg(_(e_cant_handle_systemexit_of_python_exception_in_vim));
	PyErr_Clear();
	ଫେରନ୍ତୁ;
    }
    ଅନ୍ୟଥା
	PyErr_PrintEx(1);

    PyMem_Free(code);

    ଯଦି (status)
    {
	emsg(_("failed to run the code"));
	ଫେରନ୍ତୁ;
    }

    status = 0;
    pymain = PyImport_AddModule("__main__");
    pyfunc = PyObject_GetAttrString(pymain, DOPY_FUNC);
#ifdef PY_CAN_RECURSE
    PyGILState_Release(*pygilstate);
#endif

    ପାଇଁ (lnum = RangeStart; lnum <= RangeEnd; ++lnum)
    {
	PyObject	*ଧାଡ଼ି;
	PyObject	*linenr;
	PyObject	*ret;

#ifdef PY_CAN_RECURSE
	*pygilstate = PyGILState_Ensure();
#endif
	// Check the line number, the command my have deleted lines.
	ଯଦି (lnum > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା
		|| !(ଧାଡ଼ି = Get_Buffer_ଧାଡ଼ି(curbuf, lnum)))
	    ଯାଅ err;
	ଯଦି (!(linenr = PyInt_FromLong((long) lnum)))
	{
	    Py_DECREF(ଧାଡ଼ି);
	    ଯାଅ err;
	}
	ret = PyObject_CallFunctionObjArgs(pyfunc, ଧାଡ଼ି, linenr, ଖାଲି);
	Py_DECREF(ଧାଡ଼ି);
	Py_DECREF(linenr);
	ଯଦି (!ret)
	    ଯାଅ err;

	// Check that the command didn't switch to another buffer.
	// Check the line number, the command my have deleted lines.
	ଯଦି (curbuf != was_curbuf || lnum > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	{
	    Py_XDECREF(ret);
	    ଯାଅ err;
	}

	ଯଦି (ret != Py_None)
	    ଯଦି (Set_Buffer_ଧାଡ଼ି(curbuf, lnum, ret, ଖାଲି) == FAIL)
	    {
		Py_XDECREF(ret);
		ଯାଅ err;
	    }

	Py_XDECREF(ret);
	PythonIO_Flush();
#ifdef PY_CAN_RECURSE
	PyGILState_Release(*pygilstate);
#endif
    }
    ଯାଅ out;
err:
#ifdef PY_CAN_RECURSE
    *pygilstate = PyGILState_Ensure();
#endif
    PyErr_PrintEx(0);
    PythonIO_Flush();
    status = 1;
out:
#ifdef PY_CAN_RECURSE
    ଯଦି (!status)
	*pygilstate = PyGILState_Ensure();
#endif
    Py_DECREF(pyfunc);
    PyObject_SetAttrString(pymain, DOPY_FUNC, ଖାଲି);
    ଯଦି (status)
	ଫେରନ୍ତୁ;
    check_cursor();
    update_curbuf(UPD_NOT_VALID);
}

    ସ୍ଥିର ଶୂନ୍ୟ
run_eval(ଅଚଳ ଅକ୍ଷର *cmd, dict_T *locals, ଶୂନ୍ୟ *arg
#ifdef PY_CAN_RECURSE
	, PyGILState_STATE *pygilstate UNUSED
#endif
	)
{
    PyObject	*run_ret;
    typval_T	*rettv = (typval_T*)arg;
    PyObject	*pylocals = locals ? NEW_DICTIONARY(locals) : globals;

    run_ret = PyRun_String((ଅକ୍ଷର *)cmd, Py_eval_ନିବେଶ, globals, pylocals);
    ଯଦି (run_ret == ଖାଲି)
    {
	ଯଦି (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
	{
	    emsg(_(e_cant_handle_systemexit_of_python_exception_in_vim));
	    PyErr_Clear();
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (PyErr_Occurred() && !ସନ୍ଦେଶ_silent)
		PyErr_PrintEx(0);
	    emsg(_(e_eval_did_not_return_valid_python_object));
	}
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (ConvertFromPyObject(run_ret, rettv) == -1)
	    emsg(_(e_failed_to_convert_returned_python_object_to_vim_ମୂଲ୍ୟ));
	Py_DECREF(run_ret);
    }
    PyErr_Clear();
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
set_ref_in_py(ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID)
{
    pylinkedlist_T	*cur;
    list_T		*ll;
    tuple_T		*tt;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			abort = FALSE;
    FunctionObject	*func;

    ଯଦି (lastdict != ଖାଲି)
    {
	ପାଇଁ (cur = lastdict ; !abort && cur != ଖାଲି ; cur = cur->pll_prev)
	    abort = set_ref_in_dict(((DictionaryObject *)(cur->pll_obj))->dict,
								       copyID);
    }

    ଯଦି (lastlist != ଖାଲି)
    {
	ପାଇଁ (cur = lastlist ; !abort && cur != ଖାଲି ; cur = cur->pll_prev)
	{
	    ll = ((ListObject *) (cur->pll_obj))->list;
	    abort = set_ref_in_list(ll, copyID);
	}
    }

    ଯଦି (lasttuple != ଖାଲି)
    {
	ପାଇଁ (cur = lasttuple ; !abort && cur != ଖାଲି ; cur = cur->pll_prev)
	{
	    tt = ((TupleObject *) (cur->pll_obj))->tuple;
	    abort = set_ref_in_tuple(tt, copyID);
	}
    }

    ଯଦି (lastfunc != ଖାଲି)
    {
	ପାଇଁ (cur = lastfunc ; !abort && cur != ଖାଲି ; cur = cur->pll_prev)
	{
	    func = (FunctionObject *) cur->pll_obj;
	    abort = set_ref_in_dict(func->self, copyID);
	    ଯଦି (func->argc)
		ପାଇଁ (i = 0; !abort && i < func->argc; ++i)
		    abort = abort
			|| set_ref_in_item(&func->argv[i], copyID, ଖାଲି, ଖାଲି, ଖାଲି);
	}
    }

    ଫେରନ୍ତୁ abort;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
set_string_copy(char_u *str, typval_T *tv)
{
    tv->vval.v_string = vim_strsave(str);
    ଯଦି (tv->vval.v_string == ଖାଲି)
    {
	PyErr_NoMemory();
	ଫେରନ୍ତୁ -1;
    }
    ଫେରନ୍ତୁ 0;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
pydict_to_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    dict_T	*dict;
    char_u	*ଚାବି;
    dictitem_T	*di;
    PyObject	*ଚାବି_Object;
    PyObject	*valObject;
    Py_ssize_t	iter = 0;

    ଯଦି (!(dict = py_dict_alloc()))
	ଫେରନ୍ତୁ -1;

    tv->v_type = VAR_DICT;
    tv->vval.v_dict = dict;

    ଯେତେବେଳେ (PyDict_Next(obj, &iter, &ଚାବି_Object, &valObject))
    {
	PyObject	*todecref = ଖାଲି;

	ଯଦି (ଚାବି_Object == ଖାଲି || valObject == ଖାଲି)
	{
	    dict_unref(dict);
	    ଫେରନ୍ତୁ -1;
	}

	ଯଦି (!(ଚାବି = StringToChars(ଚାବି_Object, &todecref)))
	{
	    dict_unref(dict);
	    ଫେରନ୍ତୁ -1;
	}

	ଯଦି (*ଚାବି == NUL)
	{
	    dict_unref(dict);
	    Py_XDECREF(todecref);
	    RAISE_NO_EMPTY_ଚାବିଗୁଡ଼ିକ;
	    ଫେରନ୍ତୁ -1;
	}

	di = dictitem_alloc(ଚାବି);

	Py_XDECREF(todecref);

	ଯଦି (di == ଖାଲି)
	{
	    PyErr_NoMemory();
	    dict_unref(dict);
	    ଫେରନ୍ତୁ -1;
	}

	ଯଦି (_ConvertFromPyObject(valObject, &di->di_tv, lookup_dict) == -1)
	{
	    vim_free(di);
	    dict_unref(dict);
	    ଫେରନ୍ତୁ -1;
	}

	ଯଦି (dict_add(dict, di) == FAIL)
	{
	    RAISE_ଚାବି_ADD_FAIL(di->di_ଚାବି);
	    clear_tv(&di->di_tv);
	    vim_free(di);
	    dict_unref(dict);
	    ଫେରନ୍ତୁ -1;
	}
    }

    --dict->dv_refcount;
    ଫେରନ୍ତୁ 0;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
pymap_to_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    dict_T	*dict;
    char_u	*ଚାବି;
    dictitem_T	*di;
    PyObject	*list;
    PyObject	*iterator;
    PyObject	*ଚାବି_Object;
    PyObject	*valObject;

    ଯଦି (!(dict = py_dict_alloc()))
	ଫେରନ୍ତୁ -1;

    tv->v_type = VAR_DICT;
    tv->vval.v_dict = dict;

    ଯଦି (!(list = Py_Mapping_ଚାବିଗୁଡ଼ିକ(obj)))
    {
	dict_unref(dict);
	ଫେରନ୍ତୁ -1;
    }

    ଯଦି (!(iterator = PyObject_GetIter(list)))
    {
	dict_unref(dict);
	Py_DECREF(list);
	ଫେରନ୍ତୁ -1;
    }
    Py_DECREF(list);

    ଯେତେବେଳେ ((ଚାବି_Object = PyIter_Next(iterator)))
    {
	PyObject	*todecref;

	ଯଦି (!(ଚାବି = StringToChars(ଚାବି_Object, &todecref)))
	{
	    Py_DECREF(ଚାବି_Object);
	    Py_DECREF(iterator);
	    dict_unref(dict);
	    ଫେରନ୍ତୁ -1;
	}

	ଯଦି (*ଚାବି == NUL)
	{
	    Py_DECREF(ଚାବି_Object);
	    Py_DECREF(iterator);
	    Py_XDECREF(todecref);
	    dict_unref(dict);
	    RAISE_NO_EMPTY_ଚାବିଗୁଡ଼ିକ;
	    ଫେରନ୍ତୁ -1;
	}

	ଯଦି (!(valObject = PyObject_GetItem(obj, ଚାବି_Object)))
	{
	    Py_DECREF(ଚାବି_Object);
	    Py_DECREF(iterator);
	    Py_XDECREF(todecref);
	    dict_unref(dict);
	    ଫେରନ୍ତୁ -1;
	}

	di = dictitem_alloc(ଚାବି);

	Py_DECREF(ଚାବି_Object);
	Py_XDECREF(todecref);

	ଯଦି (di == ଖାଲି)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(valObject);
	    dict_unref(dict);
	    PyErr_NoMemory();
	    ଫେରନ୍ତୁ -1;
	}

	ଯଦି (_ConvertFromPyObject(valObject, &di->di_tv, lookup_dict) == -1)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(valObject);
	    vim_free(di);
	    dict_unref(dict);
	    ଫେରନ୍ତୁ -1;
	}

	Py_DECREF(valObject);

	ଯଦି (dict_add(dict, di) == FAIL)
	{
	    RAISE_ଚାବି_ADD_FAIL(di->di_ଚାବି);
	    Py_DECREF(iterator);
	    dictitem_free(di);
	    dict_unref(dict);
	    ଫେରନ୍ତୁ -1;
	}
    }
    Py_DECREF(iterator);
    --dict->dv_refcount;
    ଫେରନ୍ତୁ 0;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
pyseq_to_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    list_T	*l;

    ଯଦି (!(l = py_list_alloc()))
	ଫେରନ୍ତୁ -1;

    tv->v_type = VAR_LIST;
    tv->vval.v_list = l;

    ଯଦି (list_py_concat(l, obj, lookup_dict) == -1)
    {
	list_unref(l);
	ଫେରନ୍ତୁ -1;
    }

    --l->lv_refcount;
    ଫେରନ୍ତୁ 0;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
pytuple_to_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    tuple_T	*t;

    ଯଦି (!(t = py_tuple_alloc()))
	ଫେରନ୍ତୁ -1;

    tv->v_type = VAR_TUPLE;
    tv->vval.v_tuple = t;

    ଯଦି (tuple_py_concat(t, obj, lookup_dict) == -1)
    {
	tuple_unref(t);
	ଫେରନ୍ତୁ -1;
    }

    --t->tv_refcount;
    ଫେରନ୍ତୁ 0;
}

ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*pytotvfunc)(PyObject *, typval_T *, PyObject *);

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
convert_dl(PyObject *obj, typval_T *tv,
				    pytotvfunc py_to_tv, PyObject *lookup_dict)
{
    PyObject	*capsule;
    ଅକ୍ଷର	hexBuf[ଆକାର(ଶୂନ୍ୟ *) * 2 + 3];

    sprintf(hexBuf, "%p", (ଶୂନ୍ୟ *)obj);

#ifdef PY_USE_CAPSULE
    capsule = PyDict_GetItemString(lookup_dict, hexBuf);
#ଅନ୍ୟଥା
    capsule = (PyObject *)PyDict_GetItemString(lookup_dict, hexBuf);
#endif
    ଯଦି (capsule == ଖାଲି)
    {
#ifdef PY_USE_CAPSULE
	capsule = PyCapsule_New(tv, ଖାଲି, ଖାଲି);
#ଅନ୍ୟଥା
	capsule = PyCObject_FromVoidPtr(tv, ଖାଲି);
#endif
	ଯଦି (PyDict_SetItemString(lookup_dict, hexBuf, capsule))
	{
	    Py_DECREF(capsule);
	    tv->v_type = VAR_UNKNOWN;
	    ଫେରନ୍ତୁ -1;
	}

	Py_DECREF(capsule);

	ଯଦି (py_to_tv(obj, tv, lookup_dict) == -1)
	{
	    tv->v_type = VAR_UNKNOWN;
	    ଫେରନ୍ତୁ -1;
	}
	// As we are not using copy_tv which increments reference count we must
	// do it ourself.
	ଯଦି (tv->v_type == VAR_DICT)
	    ++tv->vval.v_dict->dv_refcount;
	ଅନ୍ୟଥା ଯଦି (tv->v_type == VAR_LIST)
	    ++tv->vval.v_list->lv_refcount;
	ଅନ୍ୟଥା ଯଦି (tv->v_type == VAR_TUPLE)
	    ++tv->vval.v_tuple->tv_refcount;
    }
    ଅନ୍ୟଥା
    {
	typval_T	*v;

#ifdef PY_USE_CAPSULE
	v = PyCapsule_GetPointer(capsule, ଖାଲି);
#ଅନ୍ୟଥା
	v = PyCObject_AsVoidPtr(capsule);
#endif
	copy_tv(v, tv);
    }
    ଫେରନ୍ତୁ 0;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ConvertFromPyMapping(PyObject *obj, typval_T *tv)
{
    PyObject	*lookup_dict;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret;

    ଯଦି (!(lookup_dict = PyDict_New()))
	ଫେରନ୍ତୁ -1;

    ଯଦି (PyType_IsSubtype(obj->ob_type, DictionaryTypePtr))
    {
	tv->v_type = VAR_DICT;
	tv->vval.v_dict = (((DictionaryObject *)(obj))->dict);
	++tv->vval.v_dict->dv_refcount;
	ret = 0;
    }
    ଅନ୍ୟଥା ଯଦି (PyDict_Check(obj))
	ret = convert_dl(obj, tv, pydict_to_tv, lookup_dict);
    ଅନ୍ୟଥା ଯଦି (PyMapping_Check(obj))
	ret = convert_dl(obj, tv, pymap_to_tv, lookup_dict);
    ଅନ୍ୟଥା
    {
	PyErr_FORMAT_TYPE(
		N_("unable to convert %s to a Vim dictionary"),
		obj);
	ret = -1;
    }
    Py_DECREF(lookup_dict);
    ଫେରନ୍ତୁ ret;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ConvertFromPySequence(PyObject *obj, typval_T *tv)
{
    PyObject	*lookup_dict;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret;

    ଯଦି (!(lookup_dict = PyDict_New()))
	ଫେରନ୍ତୁ -1;

    ଯଦି (PyType_IsSubtype(obj->ob_type, ListTypePtr))
    {
	tv->v_type = VAR_LIST;
	tv->vval.v_list = (((ListObject *)(obj))->list);
	++tv->vval.v_list->lv_refcount;
	ret = 0;
    }
    ଅନ୍ୟଥା ଯଦି (PyIter_Check(obj) || PySequence_Check(obj))
	ret = convert_dl(obj, tv, pyseq_to_tv, lookup_dict);
    ଅନ୍ୟଥା
    {
	PyErr_FORMAT_TYPE(
		N_("unable to convert %s to a Vim list"),
		obj);
	ret = -1;
    }
    Py_DECREF(lookup_dict);
    ଫେରନ୍ତୁ ret;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ConvertFromPyObject(PyObject *obj, typval_T *tv)
{
    PyObject	*lookup_dict;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret;

    ଯଦି (!(lookup_dict = PyDict_New()))
	ଫେରନ୍ତୁ -1;
    ret = _ConvertFromPyObject(obj, tv, lookup_dict);
    Py_DECREF(lookup_dict);
    ଫେରନ୍ତୁ ret;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
_ConvertFromPyObject(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    ଯଦି (PyType_IsSubtype(obj->ob_type, DictionaryTypePtr))
    {
	tv->v_type = VAR_DICT;
	tv->vval.v_dict = (((DictionaryObject *)(obj))->dict);
	++tv->vval.v_dict->dv_refcount;
    }
    ଅନ୍ୟଥା ଯଦି (PyType_IsSubtype(obj->ob_type, ListTypePtr))
    {
	tv->v_type = VAR_LIST;
	tv->vval.v_list = (((ListObject *)(obj))->list);
	++tv->vval.v_list->lv_refcount;
    }
    ଅନ୍ୟଥା ଯଦି (PyType_IsSubtype(obj->ob_type, TupleTypePtr))
    {
	tv->v_type = VAR_TUPLE;
	tv->vval.v_tuple = (((TupleObject *)(obj))->tuple);
	++tv->vval.v_tuple->tv_refcount;
    }
    ଅନ୍ୟଥା ଯଦି (PyType_IsSubtype(obj->ob_type, FunctionTypePtr))
    {
	FunctionObject *func = (FunctionObject *) obj;
	ଯଦି (func->self != ଖାଲି || func->argv != ଖାଲି)
	{
	    partial_T *pt = ALLOC_CLEAR_ONE(partial_T);

	    set_partial(func, pt, TRUE);
	    tv->vval.v_partial = pt;
	    tv->v_type = VAR_PARTIAL;
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (set_string_copy(func->name, tv) == -1)
		ଫେରନ୍ତୁ -1;

	    tv->v_type = VAR_FUNC;
	}
	func_ref(func->name);
    }
    ଅନ୍ୟଥା ଯଦି (PyBytes_Check(obj))
    {
	char_u	*str;

	ଯଦି (PyBytes_AsStringAndSize(obj, (ଅକ୍ଷର **) &str, ଖାଲି) == -1)
	    ଫେରନ୍ତୁ -1;
	ଯଦି (str == ଖାଲି)
	    ଫେରନ୍ତୁ -1;

	ଯଦି (set_string_copy(str, tv) == -1)
	    ଫେରନ୍ତୁ -1;

	tv->v_type = VAR_STRING;
    }
    ଅନ୍ୟଥା ଯଦି (PyUnicode_Check(obj))
    {
	PyObject	*bytes;
	char_u	*str;

	bytes = PyUnicode_AsEncodedString(obj, ENC_OPT, ERRORS_ENCODE_ARG);
	ଯଦି (bytes == ଖାଲି)
	    ଫେରନ୍ତୁ -1;

	ଯଦି (PyBytes_AsStringAndSize(bytes, (ଅକ୍ଷର **) &str, ଖାଲି) == -1)
	    ଫେରନ୍ତୁ -1;
	ଯଦି (str == ଖାଲି)
	    ଫେରନ୍ତୁ -1;

	ଯଦି (set_string_copy(str, tv))
	{
	    Py_XDECREF(bytes);
	    ଫେରନ୍ତୁ -1;
	}
	Py_XDECREF(bytes);

	tv->v_type = VAR_STRING;
    }
#ଯଦି PY_MAJOR_VERSION < 3
    ଅନ୍ୟଥା ଯଦି (PyInt_Check(obj))
    {
	tv->v_type = VAR_ସଂଖ୍ୟା;
	tv->vval.v_ସଂଖ୍ୟା = (varnumber_T) PyInt_AsLong(obj);
	ଯଦି (PyErr_Occurred())
	    ଫେରନ୍ତୁ -1;
    }
#endif
    ଅନ୍ୟଥା ଯଦି (PyLong_Check(obj))
    {
	tv->v_type = VAR_ସଂଖ୍ୟା;
	tv->vval.v_ସଂଖ୍ୟା = (varnumber_T) PyLong_AsLong(obj);
	ଯଦି (PyErr_Occurred())
	    ଫେରନ୍ତୁ -1;
    }
    ଅନ୍ୟଥା ଯଦି (PyTuple_Check(obj))
	ଫେରନ୍ତୁ convert_dl(obj, tv, pytuple_to_tv, lookup_dict);
    ଅନ୍ୟଥା ଯଦି (PyDict_Check(obj))
	ଫେରନ୍ତୁ convert_dl(obj, tv, pydict_to_tv, lookup_dict);
    ଅନ୍ୟଥା ଯଦି (PyFloat_Check(obj))
    {
	tv->v_type = VAR_FLOAT;
	tv->vval.v_float = (float_T) PyFloat_AsDouble(obj);
    }
    ଅନ୍ୟଥା ଯଦି (PyObject_HasAttrString(obj, "keys"))
	ଫେରନ୍ତୁ convert_dl(obj, tv, pymap_to_tv, lookup_dict);
    // PyObject_GetIter can create built-in iterator for any sequence object
    ଅନ୍ୟଥା ଯଦି (PyIter_Check(obj) || PySequence_Check(obj))
	ଫେରନ୍ତୁ convert_dl(obj, tv, pyseq_to_tv, lookup_dict);
    ଅନ୍ୟଥା ଯଦି (PyMapping_Check(obj))
	ଫେରନ୍ତୁ convert_dl(obj, tv, pymap_to_tv, lookup_dict);
    ଅନ୍ୟଥା ଯଦି (Py_ସଂଖ୍ୟା_Check(obj))
    {
	PyObject	*ଅଙ୍କ;

	ଯଦି (!(ଅଙ୍କ = Py_ସଂଖ୍ୟା_Long(obj)))
	    ଫେରନ୍ତୁ -1;

	tv->v_type = VAR_ସଂଖ୍ୟା;
	tv->vval.v_ସଂଖ୍ୟା = (varnumber_T) PyLong_AsLong(ଅଙ୍କ);

	Py_DECREF(ଅଙ୍କ);
    }
    ଅନ୍ୟଥା ଯଦି (obj == Py_None)
    {
	tv->v_type = VAR_SPECIAL;
	tv->vval.v_ସଂଖ୍ୟା = VVAL_NONE;
    }
    ଅନ୍ୟଥା
    {
	PyErr_FORMAT_TYPE(
		N_("unable to convert %s to a Vim structure"),
		obj);
	ଫେରନ୍ତୁ -1;
    }
    ଫେରନ୍ତୁ 0;
}

    ସ୍ଥିର PyObject *
ConvertToPyObject(typval_T *tv)
{
    typval_T *argv;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
    ଯଦି (tv == ଖାଲି)
    {
	PyErr_SET_VIM(N_("internal error: NULL reference passed"));
	ଫେରନ୍ତୁ ଖାଲି;
    }
    ବାଛ (tv->v_type)
    {
	ସ୍ଥିତି VAR_STRING:
	    ଫେରନ୍ତୁ PyBytes_FromString(tv->vval.v_string == ଖାଲି
					    ? "" : (ଅକ୍ଷର *)tv->vval.v_string);
	ସ୍ଥିତି VAR_ସଂଖ୍ୟା:
	    ଫେରନ୍ତୁ PyLong_FromLong((long) tv->vval.v_ସଂଖ୍ୟା);
	ସ୍ଥିତି VAR_FLOAT:
	    ଫେରନ୍ତୁ PyFloat_FromDouble((ଦ୍ୱିଗୁଣ) tv->vval.v_float);
	ସ୍ଥିତି VAR_LIST:
	    ଫେରନ୍ତୁ NEW_LIST(tv->vval.v_list);
	ସ୍ଥିତି VAR_TUPLE:
	    ଫେରନ୍ତୁ NEW_TUPLE(tv->vval.v_tuple);
	ସ୍ଥିତି VAR_DICT:
	    ଫେରନ୍ତୁ NEW_DICTIONARY(tv->vval.v_dict);
	ସ୍ଥିତି VAR_FUNC:
	    ଫେରନ୍ତୁ NEW_FUNCTION(tv->vval.v_string == ଖାଲି
					  ? (char_u *)"" : tv->vval.v_string,
					  0, ଖାଲି, ଖାଲି, TRUE);
	ସ୍ଥିତି VAR_PARTIAL:
	    ଯଦି (tv->vval.v_partial->pt_argc)
	    {
		argv = PyMem_New(typval_T, (size_t)tv->vval.v_partial->pt_argc);
		ପାଇଁ (i = 0; i < tv->vval.v_partial->pt_argc; i++)
		    copy_tv(&tv->vval.v_partial->pt_argv[i], &argv[i]);
	    }
	    ଅନ୍ୟଥା
		argv = ଖାଲି;
	    ଯଦି (tv->vval.v_partial->pt_dict != ଖାଲି)
		tv->vval.v_partial->pt_dict->dv_refcount++;
	    ଫେରନ୍ତୁ NEW_FUNCTION(tv->vval.v_partial == ଖାଲି
			     ? (char_u *)"" : partial_name(tv->vval.v_partial),
				tv->vval.v_partial->pt_argc, argv,
				tv->vval.v_partial->pt_dict,
				tv->vval.v_partial->pt_auto);
	ସ୍ଥିତି VAR_BLOB:
	    ଫେରନ୍ତୁ PyBytes_FromStringAndSize(
		(ଅକ୍ଷର*) tv->vval.v_blob->bv_ga.ga_data,
		(Py_ssize_t) tv->vval.v_blob->bv_ga.ga_len);
	ସ୍ଥିତି VAR_UNKNOWN:
	ସ୍ଥିତି VAR_ANY:
	ସ୍ଥିତି VAR_VOID:
	ସ୍ଥିତି VAR_CHANNEL:
	ସ୍ଥିତି VAR_JOB:
	ସ୍ଥିତି VAR_INSTR:
	ସ୍ଥିତି VAR_CLASS:
	ସ୍ଥିତି VAR_OBJECT:
	ସ୍ଥିତି VAR_TYPEALIAS:
	    Py_INCREF(Py_None);
	    ଫେରନ୍ତୁ Py_None;
	ସ୍ଥିତି VAR_BOOL:
	ସ୍ଥିତି VAR_SPECIAL:
	    ବାଛ (tv->vval.v_ସଂଖ୍ୟା)
	    {
		ସ୍ଥିତି VVAL_FALSE: ଫେରନ୍ତୁ ALWAYS_FALSE;
		ସ୍ଥିତି VVAL_TRUE:  ଫେରନ୍ତୁ ALWAYS_TRUE;
		ସ୍ଥିତି VVAL_NONE:
		ସ୍ଥିତି VVAL_NULL:  ଫେରନ୍ତୁ ALWAYS_NONE;
	    }
	    PyErr_SET_VIM(N_("internal error: invalid value type"));
	    ଫେରନ୍ତୁ ଖାଲି;
    }
    ଫେରନ୍ତୁ ଖାଲି;
}

DEFINE_PY_TYPE_OBJECT(CurrentType);

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    PyObject_HEAD
} CurrentObject;

ସ୍ଥିର CurrentObject TheCurrent =
{
    PyObject_HEAD_INIT_TYPE(CurrentType)
};

    ସ୍ଥିର ଶୂନ୍ୟ
init_structs(ଶୂନ୍ୟ)
{
    CLEAR_FIELD(OutputType);
    OutputType.tp_name = "vim.message";
    OutputType.tp_basicsize = ଆକାର(OutputObject);
    OutputType.tp_flags = Py_TPFLAGS_DEFAULT;
    OutputType.tp_doc = "vim message object";
    OutputType.tp_methods = OutputMethods;
#ଯଦି PY_MAJOR_VERSION >= 3
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
#ଅନ୍ୟଥା
    OutputType.tp_getattr = OutputGetattr;
    OutputType.tp_setattr = OutputSetattr;
    // Disabled, because this causes a crash in test86
    // OutputType.tp_base = &PyFile_Type;
#endif

    CLEAR_FIELD(IterType);
    IterType.tp_name = "vim.iter";
    IterType.tp_basicsize = ଆକାର(IterObject);
    IterType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    IterType.tp_doc = "generic iterator object";
    IterType.tp_iter = IterIter;
    IterType.tp_iternext = IterNext;
    IterType.tp_dealloc = IterDestructor;
    IterType.tp_traverse = IterTraverse;
    IterType.tp_clear = IterClear;

    CLEAR_FIELD(BufferType);
    BufferType.tp_name = "vim.buffer";
    BufferType.tp_basicsize = ଆକାର(BufferObject);
    BufferType.tp_dealloc = BufferDestructor;
    BufferType.tp_repr = BufferRepr;
    BufferType.tp_as_sequence = &BufferAsSeq;
    BufferType.tp_as_mapping = &BufferAsMapping;
    BufferType.tp_flags = Py_TPFLAGS_DEFAULT;
    BufferType.tp_doc = "vim buffer object";
    BufferType.tp_methods = BufferMethods;
#ଯଦି PY_MAJOR_VERSION >= 3
    BufferType.tp_getattro = BufferGetattro;
    BufferType.tp_setattro = BufferSetattro;
    BufferType.tp_alloc = call_PyType_GenericAlloc;
    BufferType.tp_new = call_PyType_GenericNew;
    BufferType.tp_free = call_PyObject_Free;
#ଅନ୍ୟଥା
    BufferType.tp_getattr = BufferGetattr;
    BufferType.tp_setattr = BufferSetattr;
#endif

    CLEAR_FIELD(WindowType);
    WindowType.tp_name = "vim.window";
    WindowType.tp_basicsize = ଆକାର(WindowObject);
    WindowType.tp_dealloc = WindowDestructor;
    WindowType.tp_repr = WindowRepr;
    WindowType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    WindowType.tp_doc = "vim Window object";
    WindowType.tp_methods = WindowMethods;
    WindowType.tp_traverse = WindowTraverse;
    WindowType.tp_clear = WindowClear;
#ଯଦି PY_MAJOR_VERSION >= 3
    WindowType.tp_getattro = WindowGetattro;
    WindowType.tp_setattro = WindowSetattro;
    WindowType.tp_alloc = call_PyType_GenericAlloc;
    WindowType.tp_new = call_PyType_GenericNew;
    WindowType.tp_free = call_PyObject_Free;
#ଅନ୍ୟଥା
    WindowType.tp_getattr = WindowGetattr;
    WindowType.tp_setattr = WindowSetattr;
#endif

    CLEAR_FIELD(TabPageType);
    TabPageType.tp_name = "vim.tabpage";
    TabPageType.tp_basicsize = ଆକାର(TabPageObject);
    TabPageType.tp_dealloc = TabPageDestructor;
    TabPageType.tp_repr = TabPageRepr;
    TabPageType.tp_flags = Py_TPFLAGS_DEFAULT;
    TabPageType.tp_doc = "vim tab page object";
    TabPageType.tp_methods = TabPageMethods;
#ଯଦି PY_MAJOR_VERSION >= 3
    TabPageType.tp_getattro = TabPageGetattro;
    TabPageType.tp_alloc = call_PyType_GenericAlloc;
    TabPageType.tp_new = call_PyType_GenericNew;
    TabPageType.tp_free = call_PyObject_Free;
#ଅନ୍ୟଥା
    TabPageType.tp_getattr = TabPageGetattr;
#endif

    CLEAR_FIELD(BufMapType);
    BufMapType.tp_name = "vim.bufferlist";
    BufMapType.tp_basicsize = ଆକାର(BufMapObject);
    BufMapType.tp_as_mapping = &BufMapAsMapping;
    BufMapType.tp_flags = Py_TPFLAGS_DEFAULT;
    BufMapType.tp_iter = BufMapIter;
    BufMapType.tp_doc = "vim buffer list";

    CLEAR_FIELD(WinListType);
    WinListType.tp_name = "vim.windowlist";
    WinListType.tp_basicsize = ଆକାର(WinListObject);
    WinListType.tp_as_sequence = &WinListAsSeq;
    WinListType.tp_flags = Py_TPFLAGS_DEFAULT;
    WinListType.tp_doc = "vim window list";
    WinListType.tp_dealloc = (destructor)WinListDestructor;

    CLEAR_FIELD(TabListType);
    TabListType.tp_name = "vim.tabpagelist";
    TabListType.tp_basicsize = ଆକାର(TabListObject);
    TabListType.tp_as_sequence = &TabListAsSeq;
    TabListType.tp_flags = Py_TPFLAGS_DEFAULT;
    TabListType.tp_doc = "vim tab page list";

    CLEAR_FIELD(RangeType);
    RangeType.tp_name = "vim.range";
    RangeType.tp_basicsize = ଆକାର(RangeObject);
    RangeType.tp_dealloc = RangeDestructor;
    RangeType.tp_repr = RangeRepr;
    RangeType.tp_as_sequence = &RangeAsSeq;
    RangeType.tp_as_mapping = &RangeAsMapping;
    RangeType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    RangeType.tp_doc = "vim Range object";
    RangeType.tp_methods = RangeMethods;
    RangeType.tp_traverse = RangeTraverse;
    RangeType.tp_clear = RangeClear;
#ଯଦି PY_MAJOR_VERSION >= 3
    RangeType.tp_getattro = RangeGetattro;
    RangeType.tp_alloc = call_PyType_GenericAlloc;
    RangeType.tp_new = call_PyType_GenericNew;
    RangeType.tp_free = call_PyObject_Free;
#ଅନ୍ୟଥା
    RangeType.tp_getattr = RangeGetattr;
#endif

    CLEAR_FIELD(CurrentType);
    CurrentType.tp_name = "vim.currentdata";
    CurrentType.tp_basicsize = ଆକାର(CurrentObject);
    CurrentType.tp_flags = Py_TPFLAGS_DEFAULT;
    CurrentType.tp_doc = "vim current object";
    CurrentType.tp_methods = CurrentMethods;
#ଯଦି PY_MAJOR_VERSION >= 3
    CurrentType.tp_getattro = CurrentGetattro;
    CurrentType.tp_setattro = CurrentSetattro;
#ଅନ୍ୟଥା
    CurrentType.tp_getattr = CurrentGetattr;
    CurrentType.tp_setattr = CurrentSetattr;
#endif

    CLEAR_FIELD(DictionaryType);
    DictionaryType.tp_name = "vim.dictionary";
    DictionaryType.tp_basicsize = ଆକାର(DictionaryObject);
    DictionaryType.tp_dealloc = DictionaryDestructor;
    DictionaryType.tp_as_sequence = &DictionaryAsSeq;
    DictionaryType.tp_as_mapping = &DictionaryAsMapping;
    DictionaryType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE;
    DictionaryType.tp_doc = "dictionary pushing modifications to Vim structure";
    DictionaryType.tp_methods = DictionaryMethods;
    DictionaryType.tp_iter = DictionaryIter;
    DictionaryType.tp_new = DictionaryConstructor;
    DictionaryType.tp_alloc = PyType_GenericAlloc;
#ଯଦି PY_MAJOR_VERSION >= 3
    DictionaryType.tp_getattro = DictionaryGetattro;
    DictionaryType.tp_setattro = DictionarySetattro;
#ଅନ୍ୟଥା
    DictionaryType.tp_getattr = DictionaryGetattr;
    DictionaryType.tp_setattr = DictionarySetattr;
#endif

    CLEAR_FIELD(ListType);
    ListType.tp_name = "vim.list";
    ListType.tp_dealloc = ListDestructor;
    ListType.tp_basicsize = ଆକାର(ListObject);
    ListType.tp_as_sequence = &ListAsSeq;
    ListType.tp_as_mapping = &ListAsMapping;
    ListType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE;
    ListType.tp_doc = "list pushing modifications to Vim structure";
    ListType.tp_methods = ListMethods;
    ListType.tp_iter = ListIter;
    ListType.tp_new = ListConstructor;
    ListType.tp_alloc = PyType_GenericAlloc;
#ଯଦି PY_MAJOR_VERSION >= 3
    ListType.tp_getattro = ListGetattro;
    ListType.tp_setattro = ListSetattro;
#ଅନ୍ୟଥା
    ListType.tp_getattr = ListGetattr;
    ListType.tp_setattr = ListSetattr;
#endif

    // Tuple type
    CLEAR_FIELD(TupleType);
    TupleType.tp_name = "vim.tuple";
    TupleType.tp_dealloc = TupleDestructor;
    TupleType.tp_basicsize = ଆକାର(TupleObject);
    TupleType.tp_as_sequence = &TupleAsSeq;
    TupleType.tp_as_mapping = &TupleAsMapping;
    TupleType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE;
    TupleType.tp_doc = "tuple pushing modifications to Vim structure";
    TupleType.tp_methods = TupleMethods;
    TupleType.tp_iter = TupleIter;
    TupleType.tp_new = TupleConstructor;
    TupleType.tp_alloc = PyType_GenericAlloc;
#ଯଦି PY_MAJOR_VERSION >= 3
    TupleType.tp_getattro = TupleGetattro;
    TupleType.tp_setattro = TupleSetattro;
#ଅନ୍ୟଥା
    TupleType.tp_getattr = TupleGetattr;
    TupleType.tp_setattr = TupleSetattr;
#endif

    CLEAR_FIELD(FunctionType);
    FunctionType.tp_name = "vim.function";
    FunctionType.tp_basicsize = ଆକାର(FunctionObject);
    FunctionType.tp_dealloc = FunctionDestructor;
    FunctionType.tp_call = FunctionCall;
    FunctionType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE;
    FunctionType.tp_doc = "object that calls Vim function";
    FunctionType.tp_methods = FunctionMethods;
    FunctionType.tp_repr = FunctionRepr;
    FunctionType.tp_new = FunctionConstructor;
    FunctionType.tp_alloc = PyType_GenericAlloc;
#ଯଦି PY_MAJOR_VERSION >= 3
    FunctionType.tp_getattro = FunctionGetattro;
#ଅନ୍ୟଥା
    FunctionType.tp_getattr = FunctionGetattr;
#endif

    CLEAR_FIELD(OptionsType);
    OptionsType.tp_name = "vim.options";
    OptionsType.tp_basicsize = ଆକାର(OptionsObject);
    OptionsType.tp_as_sequence = &OptionsAsSeq;
    OptionsType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    OptionsType.tp_doc = "object for manipulating options";
    OptionsType.tp_iter = OptionsIter;
    OptionsType.tp_as_mapping = &OptionsAsMapping;
    OptionsType.tp_dealloc = OptionsDestructor;
    OptionsType.tp_traverse = OptionsTraverse;
    OptionsType.tp_clear = OptionsClear;

#ଯଦି PY_VERSION_HEX < 0x030700f0
    CLEAR_FIELD(LoaderType);
    LoaderType.tp_name = "vim.Loader";
    LoaderType.tp_basicsize = ଆକାର(LoaderObject);
    LoaderType.tp_flags = Py_TPFLAGS_DEFAULT;
    LoaderType.tp_doc = "vim message object";
    LoaderType.tp_methods = LoaderMethods;
    LoaderType.tp_dealloc = LoaderDestructor;
#endif

#ଯଦି PY_MAJOR_VERSION >= 3
    CLEAR_FIELD(vimmodule);
    vimmodule.m_name = "vim";
    vimmodule.m_doc = "Vim Python interface\n";
    vimmodule.m_size = -1;
    vimmodule.m_methods = VimMethods;
#endif
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
init_types(ଶୂନ୍ୟ)
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
#ଯଦି PY_VERSION_HEX < 0x030700f0
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
    ଫେରନ୍ତୁ 0;
}

#ifdef USE_LIMITED_API
    ସ୍ଥିର ଶୂନ୍ୟ
shutdown_types(ଶୂନ୍ୟ)
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
# ଯଦି PY_VERSION_HEX < 0x030700f0
    PYTYPE_CLEANUP(LoaderType);
# endif
}
#endif

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
init_sys_path(ଶୂନ୍ୟ)
{
    PyObject	*path;
    PyObject	*path_hook;
    PyObject	*path_hooks;

    ଯଦି (!(path_hook = PyObject_GetAttrString(vim_module, "path_hook")))
	ଫେରନ୍ତୁ -1;

    ଯଦି (!(path_hooks = PySys_GetObject("path_hooks")))
    {
	PyErr_Clear();
	path_hooks = PyList_New(1);
	PyList_SET_ITEM(path_hooks, 0, path_hook);
	ଯଦି (PySys_SetObject("path_hooks", path_hooks))
	{
	    Py_DECREF(path_hooks);
	    ଫେରନ୍ତୁ -1;
	}
	Py_DECREF(path_hooks);
    }
    ଅନ୍ୟଥା ଯଦି (PyList_Check(path_hooks))
    {
	ଯଦି (PyList_Append(path_hooks, path_hook))
	{
	    Py_DECREF(path_hook);
	    ଫେରନ୍ତୁ -1;
	}
	Py_DECREF(path_hook);
    }
    ଅନ୍ୟଥା
    {
	VimTryStart();
	emsg(_("Failed to set path hook: sys.path_hooks is not a list\n"
	       "You should now do the following:\n"
	       "- append vim.path_hook to sys.path_hooks\n"
	       "- append vim.VIM_SPECIAL_PATH to sys.path\n"));
	VimTryEnd(); // Discard the error
	Py_DECREF(path_hook);
	ଫେରନ୍ତୁ 0;
    }

    ଯଦି (!(path = PySys_GetObject("path")))
    {
	PyErr_Clear();
	path = PyList_New(1);
	Py_INCREF(vim_special_path_object);
	PyList_SET_ITEM(path, 0, vim_special_path_object);
	ଯଦି (PySys_SetObject("path", path))
	{
	    Py_DECREF(path);
	    ଫେରନ୍ତୁ -1;
	}
	Py_DECREF(path);
    }
    ଅନ୍ୟଥା ଯଦି (PyList_Check(path))
    {
	ଯଦି (PyList_Append(path, vim_special_path_object))
	    ଫେରନ୍ତୁ -1;
    }
    ଅନ୍ୟଥା
    {
	VimTryStart();
	emsg(_("Failed to set path: sys.path is not a list\n"
	       "You should now append vim.VIM_SPECIAL_PATH to sys.path"));
	VimTryEnd(); // Discard the error
    }

    ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ସଂରଚନା numeric_constant {
    ଅକ୍ଷର	*name;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		val;
} numeric_constants[] = {
    {"VAR_LOCKED",	VAR_LOCKED},
    {"VAR_FIXED",	VAR_FIXED},
    {"VAR_SCOPE",	VAR_SCOPE},
    {"VAR_DEF_SCOPE",	VAR_DEF_SCOPE},
};

ସଂରଚନା object_constant {
    ଅକ୍ଷର	*name;
    PyObject	*valObject;
};

#define ADD_OBJECT(m, name, obj) \
    ଯଦି (PyModule_AddObject(m, name, obj)) \
	ଫେରନ୍ତୁ -1;

#define ADD_CHECKED_OBJECT(m, name, obj) \
    { \
	PyObject	*valObject = obj; \
	ଯଦି (!valObject) \
	    ଫେରନ୍ତୁ -1; \
	ADD_OBJECT(m, name, valObject); \
    }

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
populate_module(PyObject *m)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    PyObject	*other_module;
    PyObject	*attr;
    PyObject	*imp;
#ଯଦି PY_VERSION_HEX >= 0x030700f0
    PyObject	*dict;
    PyObject	*cls;
#endif

    ପାଇଁ (i = 0; i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(ଆକାର(numeric_constants)
					   / ଆକାର(ସଂରଚନା numeric_constant));
	    ++i)
	ADD_CHECKED_OBJECT(m, numeric_constants[i].name,
		PyInt_FromLong(numeric_constants[i].val));

    ସଂରଚନା object_constant object_constants[] = {
	{"buffers",  (PyObject *)(ଶୂନ୍ୟ *)&TheBufferMap},
	{"windows",  (PyObject *)(ଶୂନ୍ୟ *)&TheWindowList},
	{"tabpages", (PyObject *)(ଶୂନ୍ୟ *)&TheTabPageList},
	{"current",  (PyObject *)(ଶୂନ୍ୟ *)&TheCurrent},

	{"Buffer",     (PyObject *)BufferTypePtr},
	{"Range",      (PyObject *)RangeTypePtr},
	{"Window",     (PyObject *)WindowTypePtr},
	{"TabPage",    (PyObject *)TabPageTypePtr},
	{"Dictionary", (PyObject *)DictionaryTypePtr},
	{"List",       (PyObject *)ListTypePtr},
	{"Tuple",      (PyObject *)TupleTypePtr},
	{"Function",   (PyObject *)FunctionTypePtr},
	{"Options",    (PyObject *)OptionsTypePtr},
#ଯଦି PY_VERSION_HEX < 0x030700f0
	{"_Loader",    (PyObject *)LoaderTypePtr},
#endif
    };

    ପାଇଁ (i = 0; i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(ଆକାର(object_constants)
					    / ଆକାର(ସଂରଚନା object_constant));
	    ++i)
    {
	PyObject	*valObject;

	valObject = object_constants[i].valObject;
	Py_INCREF(valObject);
	ADD_OBJECT(m, object_constants[i].name, valObject);
    }

    ଯଦି (!(VimError = PyErr_NewException("vim.error", ଖାଲି, ଖାଲି)))
	ଫେରନ୍ତୁ -1;
    ADD_OBJECT(m, "error", VimError);

    ADD_CHECKED_OBJECT(m, "vars",  NEW_DICTIONARY(get_globvar_dict()));
    ADD_CHECKED_OBJECT(m, "vvars", NEW_DICTIONARY(get_vimvar_dict()));
    ADD_CHECKED_OBJECT(m, "options",
	    OptionsNew(SREQ_GLOBAL, ଖାଲି, dummy_check, ଖାଲି));

    ଯଦି (!(other_module = PyImport_ImportModule("os")))
	ଫେରନ୍ତୁ -1;
    ADD_OBJECT(m, "os", other_module);

#ଯଦି PY_MAJOR_VERSION >= 3
    ଯଦି (!(py_getcwd = PyObject_GetAttrString(other_module, "getcwd")))
	ଫେରନ୍ତୁ -1;
#ଅନ୍ୟଥା
    ଯଦି (!(py_getcwd = PyObject_GetAttrString(other_module, "getcwdu")))
	ଫେରନ୍ତୁ -1;
#endif
    ADD_OBJECT(m, "_getcwd", py_getcwd)

    ଯଦି (!(py_chdir = PyObject_GetAttrString(other_module, "chdir")))
	ଫେରନ୍ତୁ -1;
    ADD_OBJECT(m, "_chdir", py_chdir);
    ଯଦି (!(attr = PyObject_GetAttrString(m, "chdir")))
	ଫେରନ୍ତୁ -1;
    ଯଦି (PyObject_SetAttrString(other_module, "chdir", attr))
    {
	Py_DECREF(attr);
	ଫେରନ୍ତୁ -1;
    }
    Py_DECREF(attr);

    ଯଦି ((py_fchdir = PyObject_GetAttrString(other_module, "fchdir")))
    {
	ADD_OBJECT(m, "_fchdir", py_fchdir);
	ଯଦି (!(attr = PyObject_GetAttrString(m, "fchdir")))
	    ଫେରନ୍ତୁ -1;
	ଯଦି (PyObject_SetAttrString(other_module, "fchdir", attr))
	{
	    Py_DECREF(attr);
	    ଫେରନ୍ତୁ -1;
	}
	Py_DECREF(attr);
    }
    ଅନ୍ୟଥା
	PyErr_Clear();

    ଯଦି (!(vim_special_path_object = PyString_FromString(vim_special_path)))
	ଫେରନ୍ତୁ -1;

    ADD_OBJECT(m, "VIM_SPECIAL_PATH", vim_special_path_object);

#ଯଦି PY_VERSION_HEX >= 0x030700f0
    ଯଦି (!(imp = PyImport_ImportModule("importlib.machinery")))
	ଫେରନ୍ତୁ -1;

    dict = PyModule_GetDict(imp);

    ଯଦି (!(cls = PyDict_GetItemString(dict, "PathFinder")))
    {
	Py_DECREF(imp);
	ଫେରନ୍ତୁ -1;
    }

    ଯଦି (!(py_find_spec = PyObject_GetAttrString(cls, "find_spec")))
    {
	Py_DECREF(imp);
	ଫେରନ୍ତୁ -1;
    }

# ଯଦି PY_VERSION_HEX < 0x30c00a7
    // find_module has been removed as of Python 3.12.0a7
    ଯଦି ((py_find_module = PyObject_GetAttrString(cls, "find_module")))
	ADD_OBJECT(m, "_find_module", py_find_module);
# endif

    Py_DECREF(imp);

    ADD_OBJECT(m, "_find_spec", py_find_spec);
#ଅନ୍ୟଥା
    ଯଦି (!(imp = PyImport_ImportModule("imp")))
	ଫେରନ୍ତୁ -1;

    ଯଦି (!(py_find_module = PyObject_GetAttrString(imp, "find_module")))
    {
	Py_DECREF(imp);
	ଫେରନ୍ତୁ -1;
    }

    ଯଦି (!(py_load_module = PyObject_GetAttrString(imp, "load_module")))
    {
	Py_DECREF(py_find_module);
	Py_DECREF(imp);
	ଫେରନ୍ତୁ -1;
    }

    Py_DECREF(imp);

    ADD_OBJECT(m, "_find_module", py_find_module);
    ADD_OBJECT(m, "_load_module", py_load_module);
#endif

    ଫେରନ୍ତୁ 0;
}
