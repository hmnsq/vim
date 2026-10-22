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

#ᬬᬾᬦ᭄ PY_VERSION_HEX < 0x02050000
typedef puur_naa_nka Py_ssize_t;  // Python 2.4 and earlier don't have this type.
#endif

// Use values that are known to work, others may make Vim crash.
#define ENC_OPT (enc_utf8 ? "utf-8" : enc_dbcs ? "euc-jp" : (var_na *)p_enc)
#define DOPY_FUNC "_vim_pydo"

static const var_na *vim_special_path = "_vim_path_";

#define PyErr_SET_STRING(exc, str) PyErr_SetString(exc, _(str))
#define PyErr_SetVim(str) PyErr_SetString(VimError, str)
#define PyErr_SET_VIM(str) PyErr_SET_STRING(VimError, str)
#define PyErr_FORMAT(exc, str, arg) PyErr_Format(exc, _(str), arg)
#define PyErr_FORMAT2(exc, str, arg1, arg2) PyErr_Format(exc, _(str), arg1,arg2)
#define PyErr_VIM_FORMAT(str, arg) PyErr_FORMAT(VimError, str, arg)

#ifdef USE_LIMITED_API
// Limited Python API. Need to call only exposed functions and remap macros.
// PyTypeObject is an opaque struct.

typedef sa_nracanaa {
    lenfunc sq_length;
    binaryfunc sq_concat;
    ssizeargfunc sq_repeat;
    ssizeargfunc sq_item;
    void *was_sq_slice;
    ssizeobjargproc sq_ass_item;
    void *was_sq_ass_slice;
    objobjproc sq_contains;

    binaryfunc sq_inplace_concat;
    ssizeargfunc sq_inplace_repeat;
} PySequenceMethods;

typedef sa_nracanaa {
    lenfunc mp_length;
    binaryfunc mp_subscript;
    objobjargproc mp_ass_subscript;
} PyMappingMethods;

// This struct emulates the concrete _typeobject struct to allow the code to
// work the same way in both limited and full Python APIs.
sa_nracanaa typeobject_wrapper {
    const var_na *tp_name;
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

    const var_na *tp_doc;

    traverseproc tp_traverse;

    inquiry tp_clear;

    getiterfunc tp_iter;
    iternextfunc tp_iternext;

    sa_nracanaa PyMethodDef *tp_methods;
    sa_nracanaa _typeobject *tp_base;
    allocfunc tp_alloc;
    newfunc tp_new;
    freefunc tp_free;
};

# define DEFINE_PY_TYPE_OBJECT(type) \
    static sa_nracanaa typeobject_wrapper type; \
    static PyTypeObject* type##Ptr = NULL

// PyObject_HEAD_INIT_TYPE and PyObject_FINISH_INIT_TYPE need to come in pairs
// We first initialize with NULL because the type is not allocated until
// init_types() is called later. It's in FINISH_INIT_TYPE where we fill the
// type in with the newly allocated type.
# define PyObject_HEAD_INIT_TYPE(type) PyObject_HEAD_INIT(NULL)
# define PyObject_FINISH_INIT_TYPE(obj, type) obj.ob_base.ob_type = type##Ptr

# define Py_TYPE_GET_TP_ALLOC(type) ((allocfunc)PyType_GetSlot(type, Py_tp_alloc))
# define Py_TYPE_GET_TP_METHODS(type) ((PyMethodDef *)PyType_GetSlot(type, Py_tp_methods))

// PyObject_NEW is not part of stable ABI, but PyObject_Malloc/Init are.
static PyObject* Vim_PyObject_New(PyTypeObject *type, size_t objsize)
{
    PyObject *obj = (PyObject *)PyObject_Malloc(objsize);
    ᬬᬾᬦ᭄ (obj == NULL)
	ᬯᬮᬶ PyErr_NoMemory();
    ᬯᬮᬶ PyObject_Init(obj, type);
}
# undef PyObject_NEW
# define PyObject_NEW(type, typeobj) ((type *)Vim_PyObject_New(typeobj, parimaa_na(type)))

// This is a somewhat convoluted because limited API doesn't expose an easy way
// to get the tp_name field, and so we have to manually reconstruct it as
// "__module__.__name__" (with __module__ omitted for builtins to emulate
// Python behavior). Also, some of the more convenient functions like
// PyUnicode_AsUTF8AndSize and PyType_GetQualName() are not available until
// late Python 3 versions, and won't be available if you set Py_LIMITED_API too
// low.
# define PyErr_FORMAT_TYPE(msg, obj) \
    do { \
	PyObject* qualname = PyObject_GetAttrString((PyObject*)(obj)->ob_type, "__qualname__"); \
	ᬬᬾᬦ᭄ (qualname == NULL) \
	{ \
	    PyErr_FORMAT(PyExc_TypeError, msg, "(NULL)"); \
	    bha_nga; \
	} \
	PyObject* module = PyObject_GetAttrString((PyObject*)(obj)->ob_type, "__module__"); \
	PyObject* full; \
	ᬬᬾᬦ᭄ (module == NULL || PyUnicode_CompareWithASCIIString(module, "builtins") == 0 \
		|| PyUnicode_CompareWithASCIIString(module, "__main__") == 0) \
	{ \
	    full = qualname; \
	    Py_INCREF(full); \
	} \
	ᬮᬶᬬ \
	    full = PyUnicode_FromFormat("%U.%U", module, qualname); \
	PyObject* full_bytes = PyUnicode_AsUTF8String(full); \
	const var_na* full_str = PyBytes_AsString(full_bytes); \
	full_str = full_str == NULL ? "(NULL)" : full_str; \
	PyErr_FORMAT(PyExc_TypeError, msg, full_str); \
	Py_DECREF(qualname); \
	Py_XDECREF(module); \
	Py_XDECREF(full); \
	Py_XDECREF(full_bytes); \
    } ᬲᭂᬤᭂᬮ (0)

# define PyList_GET_ITEM(list, i) PyList_GetItem(list, i)
# define PyList_GET_SIZE(o) PyList_Size(o)

// PyList_SET_ITEM and PyList_SetItem have slightly different behaviors. The
// former will leave the old item dangling, and the latter will decref on it.
// Since we only use this on new lists, this difference doesn't matter.
# define PyList_SET_ITEM(list, i, item) PyList_SetItem(list, i, item)

# define PyTuple_GET_ITEM(o, pos) PyTuple_GetItem(o, pos)
# define PyTuple_GET_SIZE(o) PyTuple_Size(o)

# ᬬᬾᬦ᭄ Py_LIMITED_API < 0x03080000
// PyIter_check only became part of stable ABI in 3.8, and there is no easy way
// to check for it in the API. We simply return false as a compromise. This
// does mean we should avoid compiling with stable ABI < 3.8.
#  undef PyIter_Check
#  define PyIter_Check(obj) (FALSE)
# endif

static PyTypeObject* AddHeapType(sa_nracanaa typeobject_wrapper* type_object)
{
    PyType_Spec type_spec;
    type_spec.name = type_object->tp_name;
    type_spec.basicsize = type_object->tp_basicsize;
    type_spec.itemsize = 0;
    type_spec.flags = type_object->tp_flags;

    // We just need to statically allocate a large enough buffer that can hold
    // all slots. We need to leave a null-terminated slot at the end.
    PyType_Slot slots[40] = { {0, NULL} };
    size_t slot_i = 0;

# define ADD_TP_SLOT(slot_name) \
    ᬬᬾᬦ᭄ (slot_i >= 40) ᬯᬮᬶ NULL; /* this should never happen */ \
    ᬬᬾᬦ᭄ (type_object->slot_name != NULL) \
    { \
	slots[slot_i].slot = Py_##slot_name; \
	slots[slot_i].pfunc = (void*)type_object->slot_name; \
	++slot_i; \
    }
# define ADD_TP_SUB_SLOT(sub_slot, slot_name) \
    ᬬᬾᬦ᭄ (slot_i >= 40) ᬯᬮᬶ NULL; /* this should never happen */ \
    ᬬᬾᬦ᭄ (type_object->sub_slot != NULL && type_object->sub_slot->slot_name != NULL) \
    { \
	slots[slot_i].slot = Py_##slot_name; \
	slots[slot_i].pfunc = (void*)type_object->sub_slot->slot_name; \
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
    ᬯᬮᬶ (PyTypeObject*)newtype;
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
    ᬬᬾᬦ᭄ (type##Ptr == NULL) \
	ᬯᬮᬶ -1;
# define PYTYPE_CLEANUP(type) \
    type##Ptr = NULL;

// Limited API does not provide PyRun_* functions. Need to implement manually
// using PyCompile and PyEval.
static PyObject* Vim_PyRun_String(const var_na *str, puur_naa_nka start, PyObject *globals, PyObject *locals)
{
    // Just pass "" for filename for now.
    PyObject* compiled = Py_CompileString(str, "", start);
    ᬬᬾᬦ᭄ (compiled == NULL)
	ᬯᬮᬶ NULL;

    PyObject* eval_result = PyEval_EvalCode(compiled, globals, locals);
    Py_DECREF(compiled);
    ᬯᬮᬶ eval_result;
}
static puur_naa_nka Vim_PyRun_SimpleString(const var_na *str)
{
    // This function emulates CPython's implementation.
    PyObject* m = PyImport_AddModule("__main__");
    ᬬᬾᬦ᭄ (m == NULL)
	ᬯᬮᬶ -1;
    PyObject* d = PyModule_GetDict(m);
    PyObject* output = Vim_PyRun_String(str, Py_file_input, d, d);
    ᬬᬾᬦ᭄ (output == NULL)
    {
	PyErr_PrintEx(TRUE);
	ᬯᬮᬶ -1;
    }
    Py_DECREF(output);
    ᬯᬮᬶ 0;
}
# define PyRun_String Vim_PyRun_String
# define PyRun_SimpleString Vim_PyRun_SimpleString

#ᬮᬶᬬ // !defined(USE_LIMITED_API)

// Full Python API. Can make use of structs and macros directly.
# define DEFINE_PY_TYPE_OBJECT(type) \
    static PyTypeObject type; \
    static PyTypeObject* type##Ptr = &type
# define PyObject_HEAD_INIT_TYPE(type) PyObject_HEAD_INIT(&type)

# define Py_TYPE_GET_TP_ALLOC(type) type->tp_alloc
# define Py_TYPE_GET_TP_METHODS(type) type->tp_methods

# define Py_TYPE_NAME(obj) ((obj)->ob_type->tp_name == NULL \
	? "(NULL)" \
	: (obj)->ob_type->tp_name)
# define PyErr_FORMAT_TYPE(msg, obj) \
    PyErr_FORMAT(PyExc_TypeError, msg, \
	    Py_TYPE_NAME(obj))

// Add a static type
# define PYTYPE_READY(type) \
    ᬬᬾᬦ᭄ (PyType_Ready(type##Ptr)) \
	ᬯᬮᬶ -1;

#endif


#define RAISE_NO_EMPTY_KEYS PyErr_SET_STRING(PyExc_ValueError, \
					    N_("empty keys are not allowed"))
#define RAISE_LOCKED_DICTIONARY PyErr_SET_VIM(N_("dictionary is locked"))
#define RAISE_LOCKED_LIST PyErr_SET_VIM(N_("list is locked"))
#define RAISE_UNDO_FAIL PyErr_SET_VIM(N_("cannot save undo information"))
#define RAISE_DELETE_LINE_FAIL PyErr_SET_VIM(N_("cannot delete line"))
#define RAISE_INSERT_LINE_FAIL PyErr_SET_VIM(N_("cannot insert line"))
#define RAISE_REPLACE_LINE_FAIL PyErr_SET_VIM(N_("cannot replace line"))
#define RAISE_KEY_ADD_FAIL(key) \
    PyErr_VIM_FORMAT(N_("failed to add key '%s' to dictionary"), key)
#define RAISE_INVALID_INDEX_TYPE(idx) \
    PyErr_FORMAT_TYPE(N_("index must be int or slice, not %s"), idx);

#define INVALID_BUFFER_VALUE ((buf_T *)(-1))
#define INVALID_WINDOW_VALUE ((win_T *)(-1))
#define INVALID_TABPAGE_VALUE ((tabpage_T *)(-1))

typedef void (*rangeinitializer)(void *);
typedef void (*runner)(const var_na *, dict_T *, void *
#ifdef PY_CAN_RECURSE
	, PyGILState_STATE *
#endif
	);

static puur_naa_nka ConvertFromPyObject(PyObject *, typval_T *);
static puur_naa_nka _ConvertFromPyObject(PyObject *, typval_T *, PyObject *);
static puur_naa_nka ConvertFromPyMapping(PyObject *, typval_T *);
static puur_naa_nka ConvertFromPySequence(PyObject *, typval_T *);
static PyObject *WindowNew(win_T *, tabpage_T *);
static PyObject *BufferNew (buf_T *);
static PyObject *LineToString(const var_na *);

static PyInt RangeStart;
static PyInt RangeEnd;

static PyObject *globals;

static PyObject *py_chdir;
static PyObject *py_fchdir;
static PyObject *py_getcwd;
static PyObject *vim_module;
static PyObject *vim_special_path_object;

#ᬬᬾᬦ᭄ PY_VERSION_HEX >= 0x030700f0
static PyObject *py_find_spec;
#ᬮᬶᬬ
static PyObject *py_load_module;
#endif
#ᬬᬾᬦ᭄ PY_VERSION_HEX < 0x30c00a7
static PyObject *py_find_module;
#endif

static PyObject *VimError;

/*
 * obtain a lock on the Vim data structures
 */
    static void
Python_Lock_Vim(void)
{
}

/*
 * release a lock on the Vim data structures
 */
    static void
Python_Release_Vim(void)
{
}

/*
 * The "todecref" argument holds a pointer to PyObject * that must be
 * DECREF'ed after returned char_u * is no longer needed or NULL if all what
 * was needed to generate returned value is object.
 *
 * Use Py_XDECREF to decrement reference count.
 */
    static char_u *
StringToChars(PyObject *obj, PyObject **todecref)
{
    char_u	*str;

    ᬬᬾᬦ᭄ (PyBytes_Check(obj))
    {

	ᬬᬾᬦ᭄ (PyBytes_AsStringAndSize(obj, (var_na **) &str, NULL) == -1
		|| str == NULL)
	    ᬯᬮᬶ NULL;

	*todecref = NULL;
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (PyUnicode_Check(obj))
    {
	PyObject	*bytes;

	ᬬᬾᬦ᭄ (!(bytes = PyUnicode_AsEncodedString(obj, ENC_OPT,
							   ERRORS_ENCODE_ARG)))
	    ᬯᬮᬶ NULL;

	ᬬᬾᬦ᭄ (PyBytes_AsStringAndSize(bytes, (var_na **) &str, NULL) == -1
		|| str == NULL)
	{
	    Py_DECREF(bytes);
	    ᬯᬮᬶ NULL;
	}

	*todecref = bytes;
    }
    ᬮᬶᬬ
    {
#ᬬᬾᬦ᭄ PY_MAJOR_VERSION < 3
	PyErr_FORMAT_TYPE(N_("expected str() or unicode() instance, but got %s"),
		obj);
#ᬮᬶᬬ
	PyErr_FORMAT_TYPE(N_("expected bytes() or str() instance, but got %s"),
		obj);
#endif
	ᬯᬮᬶ NULL;
    }

    ᬯᬮᬶ (char_u *) str;
}

#define NUMBER_LONG     1
#define NUMBER_INT      2
#define NUMBER_NATURAL  4
#define NUMBER_UNSIGNED 8

    static puur_naa_nka
NumberToLong(PyObject *obj, long *result, puur_naa_nka flags)
{
#ᬬᬾᬦ᭄ PY_MAJOR_VERSION < 3
    ᬬᬾᬦ᭄ (PyInt_Check(obj))
    {
	*result = PyInt_AsLong(obj);
	ᬬᬾᬦ᭄ (PyErr_Occurred())
	    ᬯᬮᬶ -1;
    }
    ᬮᬶᬬ
#endif
    ᬬᬾᬦ᭄ (PyLong_Check(obj))
    {
	*result = PyLong_AsLong(obj);
	ᬬᬾᬦ᭄ (PyErr_Occurred())
	    ᬯᬮᬶ -1;
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (PyNumber_Check(obj))
    {
	PyObject	*num;

	ᬬᬾᬦ᭄ (!(num = PyNumber_Long(obj)))
	    ᬯᬮᬶ -1;

	*result = PyLong_AsLong(num);

	Py_DECREF(num);

	ᬬᬾᬦ᭄ (PyErr_Occurred())
	    ᬯᬮᬶ -1;
    }
    ᬮᬶᬬ
    {
#ᬬᬾᬦ᭄ PY_MAJOR_VERSION < 3
	PyErr_FORMAT_TYPE(
		N_("expected int(), long() or something supporting "
		   "coercing to long(), but got %s"),
		obj);
#ᬮᬶᬬ
	PyErr_FORMAT_TYPE(
		N_("expected int() or something supporting coercing to int(), "
		   "but got %s"),
		obj);
#endif
	ᬯᬮᬶ -1;
    }

    ᬬᬾᬦ᭄ (flags & NUMBER_INT)
    {
	ᬬᬾᬦ᭄ (*result > INT_MAX)
	{
	    PyErr_SET_STRING(PyExc_OverflowError,
		    N_("value is too large to fit into C int type"));
	    ᬯᬮᬶ -1;
	}
	ᬮᬶᬬ ᬬᬾᬦ᭄ (*result < INT_MIN)
	{
	    PyErr_SET_STRING(PyExc_OverflowError,
		    N_("value is too small to fit into C int type"));
	    ᬯᬮᬶ -1;
	}
    }

    ᬬᬾᬦ᭄ (flags & NUMBER_NATURAL)
    {
	ᬬᬾᬦ᭄ (*result <= 0)
	{
	    PyErr_SET_STRING(PyExc_ValueError,
		    N_("number must be greater than zero"));
	    ᬯᬮᬶ -1;
	}
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (flags & NUMBER_UNSIGNED)
    {
	ᬬᬾᬦ᭄ (*result < 0)
	{
	    PyErr_SET_STRING(PyExc_ValueError,
		    N_("number must be greater or equal to zero"));
	    ᬯᬮᬶ -1;
	}
    }

    ᬯᬮᬶ 0;
}

    static puur_naa_nka
add_string(PyObject *list, var_na *s)
{
    PyObject	*string;

    ᬬᬾᬦ᭄ (!(string = PyString_FromString(s)))
	ᬯᬮᬶ -1;

    ᬬᬾᬦ᭄ (PyList_Append(list, string))
    {
	Py_DECREF(string);
	ᬯᬮᬶ -1;
    }

    Py_DECREF(string);
    ᬯᬮᬶ 0;
}

    static PyObject *
ObjectDir(PyObject *self, var_na **attributes)
{
    PyMethodDef	*method;
    var_na	**attr;
    PyObject	*ret;

    ᬬᬾᬦ᭄ (!(ret = PyList_New(0)))
	ᬯᬮᬶ NULL;

    ᬬᬾᬦ᭄ (self)
	for (method = Py_TYPE_GET_TP_METHODS(self->ob_type) ; method->ml_name != NULL ; ++method)
	    ᬬᬾᬦ᭄ (add_string(ret, (var_na *)method->ml_name))
	    {
		Py_DECREF(ret);
		ᬯᬮᬶ NULL;
	    }

    for (attr = attributes ; *attr ; ++attr)
	ᬬᬾᬦ᭄ (add_string(ret, *attr))
	{
	    Py_DECREF(ret);
	    ᬯᬮᬶ NULL;
	}

#ᬬᬾᬦ᭄ PY_MAJOR_VERSION < 3
    ᬬᬾᬦ᭄ (add_string(ret, "__members__"))
    {
	Py_DECREF(ret);
	ᬯᬮᬶ NULL;
    }
#endif

    ᬯᬮᬶ ret;
}

// Output buffer management

// Function to write a line, points to either msg() or emsg().
typedef puur_naa_nka (*writefn)(var_na *);

DEFINE_PY_TYPE_OBJECT(OutputType);

typedef sa_nracanaa
{
    PyObject_HEAD
    long softspace;
    long error;
} OutputObject;

static var_na *OutputAttrs[] = {
    "softspace",
    NULL
};

    static PyObject *
OutputDir(PyObject *self, PyObject *args UNUSED)
{
    ᬯᬮᬶ ObjectDir(self, OutputAttrs);
}

    static puur_naa_nka
OutputSetattr(PyObject *self_obj, var_na *name, PyObject *valObject)
{
    OutputObject *self = (OutputObject*)self_obj;
    ᬬᬾᬦ᭄ (valObject == NULL)
    {
	PyErr_SET_STRING(PyExc_AttributeError,
		N_("can't delete OutputObject attributes"));
	ᬯᬮᬶ -1;
    }

    ᬬᬾᬦ᭄ (strcmp(name, "softspace") == 0)
    {
	ᬬᬾᬦ᭄ (NumberToLong(valObject, &(self->softspace), NUMBER_UNSIGNED))
	    ᬯᬮᬶ -1;
	ᬯᬮᬶ 0;
    }

    PyErr_FORMAT(PyExc_AttributeError, N_("invalid attribute: %s"), name);
    ᬯᬮᬶ -1;
}

// Buffer IO, we write one whole line at a time.
static garray_T io_ga = {0, 0, 1, 80, NULL};
static writefn old_fn = NULL;

    static void
PythonIO_Flush(void)
{
    ᬬᬾᬦ᭄ (old_fn != NULL && io_ga.ga_len > 0)
    {
	((var_na *)io_ga.ga_data)[io_ga.ga_len] = NUL;
	// We don't know what emsg_severe should be here, so ... hope?
	old_fn((var_na *)io_ga.ga_data);
    }
    io_ga.ga_len = 0;
}

    static void
writer(writefn fn, char_u *str, PyInt n, puur_naa_nka severe)
{
    char_u *ptr;

    // Flush when switching output function.
    ᬬᬾᬦ᭄ (fn != old_fn)
	PythonIO_Flush();
    old_fn = fn;

    // Write each NL separated line.  Text after the last NL is kept for
    // writing later.
    // For normal messages: Do not output when "got_int" was set.  This avoids
    // a loop gone crazy flooding the terminal with messages.  Also for when
    // "q" is pressed at the more-prompt.
    ᬲᭂᬤᭂᬮ (n > 0 && (ptr = memchr(str, '\n', n)) != NULL
					  && (fn == (writefn)emsg || !got_int))
    {
	PyInt len = ptr - str;

	ᬬᬾᬦ᭄ (ga_grow(&io_ga, (puur_naa_nka)(len + 1)) == FAIL)
	    bha_nga;

	mch_memmove(((var_na *)io_ga.ga_data) + io_ga.ga_len, str, (size_t)len);
	((var_na *)io_ga.ga_data)[io_ga.ga_len + len] = NUL;
	emsg_severe = severe;
	fn((var_na *)io_ga.ga_data);
	str = ptr + 1;
	n -= len + 1;
	io_ga.ga_len = 0;
    }

    // Put the remaining text into io_ga for later printing.
    ᬬᬾᬦ᭄ (n > 0 && (fn == (writefn)emsg || !got_int)
					&& ga_grow(&io_ga, (puur_naa_nka)(n + 1)) == OK)
    {
	mch_memmove(((var_na *)io_ga.ga_data) + io_ga.ga_len, str, (size_t)n);
	io_ga.ga_len += (puur_naa_nka)n;
    }
}

    static puur_naa_nka
write_output(OutputObject *self, PyObject *string)
{
    Py_ssize_t	len = 0;
    var_na	*str = NULL;
    puur_naa_nka		error = self->error;

    ᬬᬾᬦ᭄ (!PyArg_Parse(string, "et#", ENC_OPT, &str, &len))
	ᬯᬮᬶ -1;

    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    writer((writefn)(error ? emsg : msg), (char_u *)str, len, error);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS
    PyMem_Free(str);

    ᬯᬮᬶ 0;
}

    static PyObject *
OutputWrite(OutputObject *self, PyObject *string)
{
    ᬬᬾᬦ᭄ (write_output(self, string))
	ᬯᬮᬶ NULL;

    Py_INCREF(Py_None);
    ᬯᬮᬶ Py_None;
}

    static PyObject *
OutputWritelines(OutputObject *self, PyObject *seq)
{
    PyObject	*iterator;
    PyObject	*item;

    ᬬᬾᬦ᭄ (!(iterator = PyObject_GetIter(seq)))
	ᬯᬮᬶ NULL;

    ᬲᭂᬤᭂᬮ ((item = PyIter_Next(iterator)))
    {
	ᬬᬾᬦ᭄ (write_output(self, item))
	{
	    Py_DECREF(iterator);
	    Py_DECREF(item);
	    ᬯᬮᬶ NULL;
	}
	Py_DECREF(item);
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    ᬬᬾᬦ᭄ (PyErr_Occurred())
	ᬯᬮᬶ NULL;

    Py_INCREF(Py_None);
    ᬯᬮᬶ Py_None;
}

    static PyObject *
AlwaysNone(PyObject *self UNUSED, PyObject *args UNUSED)
{
    // do nothing
    Py_INCREF(Py_None);
    ᬯᬮᬶ Py_None;
}
#define ALWAYS_NONE AlwaysNone(NULL, NULL)

    static PyObject *
AlwaysFalse(PyObject *self UNUSED, PyObject *args UNUSED)
{
    // do nothing
    PyObject	*ret = Py_False;
    Py_INCREF(ret);
    ᬯᬮᬶ ret;
}
#define ALWAYS_FALSE AlwaysFalse(NULL, NULL)

    static PyObject *
AlwaysTrue(PyObject *self UNUSED, PyObject *args UNUSED)
{
    // do nothing
    PyObject	*ret = Py_True;
    Py_INCREF(ret);
    ᬯᬮᬶ ret;
}
#define ALWAYS_TRUE AlwaysTrue(NULL, NULL)

/***************/

static sa_nracanaa PyMethodDef OutputMethods[] = {
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
    { NULL,	    NULL,				0,		NULL}
};

static OutputObject Output =
{
    PyObject_HEAD_INIT_TYPE(OutputType)
    0,
    0
};

static OutputObject Error =
{
    PyObject_HEAD_INIT_TYPE(OutputType)
    0,
    1
};

    static puur_naa_nka
PythonIO_Init_io(void)
{
    ᬬᬾᬦ᭄ (PySys_SetObject("stdout", (PyObject *)(void *)&Output))
	ᬯᬮᬶ -1;
    ᬬᬾᬦ᭄ (PySys_SetObject("stderr", (PyObject *)(void *)&Error))
	ᬯᬮᬶ -1;

    ᬬᬾᬦ᭄ (PyErr_Occurred())
    {
	emsg(_(e_python_error_initialising_io_object));
	ᬯᬮᬶ -1;
    }

    ᬯᬮᬶ 0;
}

#ᬬᬾᬦ᭄ PY_VERSION_HEX < 0x030700f0
static PyObject *call_load_module(var_na *name, puur_naa_nka len, PyObject *find_module_result);

typedef sa_nracanaa
{
    PyObject_HEAD
    var_na	*fullname;
    PyObject	*result;
} LoaderObject;
DEFINE_PY_TYPE_OBJECT(LoaderType);

    static void
LoaderDestructor(PyObject *self_obj)
{
    LoaderObject *self = (LoaderObject*)self_obj;
    vim_free(self->fullname);
    Py_XDECREF(self->result);
    DESTRUCTOR_FINISH(self);
}

    static PyObject *
LoaderLoadModule(LoaderObject *self, PyObject *args UNUSED)
{
    var_na	*fullname = self->fullname;
    PyObject	*result = self->result;
    PyObject	*module;

    ᬬᬾᬦ᭄ (!fullname)
    {
	module = result ? result : Py_None;
	Py_INCREF(module);
	ᬯᬮᬶ module;
    }

    module = call_load_module(fullname, (puur_naa_nka)STRLEN(fullname), result);

    self->fullname = NULL;
    self->result = module;

    vim_free(fullname);
    Py_DECREF(result);

    ᬬᬾᬦ᭄ (!module)
    {
	ᬬᬾᬦ᭄ (PyErr_Occurred())
	    ᬯᬮᬶ NULL;

	Py_INCREF(Py_None);
	ᬯᬮᬶ Py_None;
    }

    Py_INCREF(module);
    ᬯᬮᬶ module;
}

static sa_nracanaa PyMethodDef LoaderMethods[] = {
    // name,	    function,				calling,	doc
    {"load_module", (PyCFunction)LoaderLoadModule,	METH_VARARGS,	""},
    { NULL,	    NULL,				0,		NULL}
};
#endif

/*
 * Check to see whether a Vim error has been reported, or a keyboard
 * interrupt has been detected.
 */
    static void
VimTryStart(void)
{
    ++trylevel;
}

    static puur_naa_nka
VimTryEnd(void)
{
    --trylevel;
    // Without this it stops processing all subsequent Vim script commands and
    // generates strange error messages if I e.g. try calling Test() in a cycle
    did_emsg = FALSE;
    // Keyboard interrupt should be preferred over anything else
    ᬬᬾᬦ᭄ (got_int)
    {
	ᬬᬾᬦ᭄ (did_throw)
	    discard_current_exception();
	got_int = FALSE;
	PyErr_SetNone(PyExc_KeyboardInterrupt);
	ᬯᬮᬶ -1;
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (msg_list != NULL && *msg_list != NULL)
    {
	puur_naa_nka	should_free;
	var_na	*msg;

	msg = get_exception_string(*msg_list, ET_ERROR, NULL, &should_free);

	ᬬᬾᬦ᭄ (msg == NULL)
	{
	    PyErr_NoMemory();
	    ᬯᬮᬶ -1;
	}

	PyErr_SetVim(msg);

	free_global_msglist();

	ᬬᬾᬦ᭄ (should_free)
	    vim_free(msg);

	ᬯᬮᬶ -1;
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (!did_throw)
	ᬯᬮᬶ (PyErr_Occurred() ? -1 : 0);
    // Python exception is preferred over Vim one; unlikely to occur though
    ᬮᬶᬬ ᬬᬾᬦ᭄ (PyErr_Occurred())
    {
	discard_current_exception();
	ᬯᬮᬶ -1;
    }
    // Finally transform Vim script exception to python one
    ᬮᬶᬬ
    {
	PyErr_SetVim((var_na *)current_exception->value);
	discard_current_exception();
	ᬯᬮᬶ -1;
    }
}

    static puur_naa_nka
VimCheckInterrupt(void)
{
    ᬬᬾᬦ᭄ (got_int)
    {
	PyErr_SetNone(PyExc_KeyboardInterrupt);
	ᬯᬮᬶ 1;
    }
    ᬯᬮᬶ 0;
}

// Vim module - Implementation

    static PyObject *
VimCommand(PyObject *self UNUSED, PyObject *string)
{
    char_u	*cmd;
    PyObject	*ret;
    PyObject	*todecref;

    ᬬᬾᬦ᭄ (!(cmd = StringToChars(string, &todecref)))
	ᬯᬮᬶ NULL;

    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();

    VimTryStart();
    do_cmdline_cmd(cmd);
    update_screen(UPD_VALID);

    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    ᬬᬾᬦ᭄ (VimTryEnd())
	ret = NULL;
    ᬮᬶᬬ
	ret = Py_None;

    Py_XINCREF(ret);
    Py_XDECREF(todecref);
    ᬯᬮᬶ ret;
}

/*
 * Function to translate a typval_T into a PyObject; this will recursively
 * translate lists/dictionaries into their Python equivalents.
 *
 * The depth parameter is to avoid infinite recursion, set it to 1 when
 * you call VimToPython.
 */
    static PyObject *
VimToPython(typval_T *our_tv, puur_naa_nka depth, PyObject *lookup_dict)
{
    PyObject	*ret;
    PyObject	*newObj;
    var_na	ptrBuf[parimaa_na(void *) * 2 + 3];

    // Avoid infinite recursion
    ᬬᬾᬦ᭄ (depth > 100)
    {
	Py_INCREF(Py_None);
	ret = Py_None;
	ᬯᬮᬶ ret;
    }

    // Check if we run into a recursive loop.  The item must be in lookup_dict
    // then and we can use it again.
    ᬬᬾᬦ᭄ ((our_tv->v_type == VAR_LIST && our_tv->vval.v_list != NULL)
	    || (our_tv->v_type == VAR_TUPLE && our_tv->vval.v_tuple != NULL)
	    || (our_tv->v_type == VAR_DICT && our_tv->vval.v_dict != NULL))
    {
	sprintf(ptrBuf, "%p",
		our_tv->v_type == VAR_LIST ? (void *)our_tv->vval.v_list
		    : our_tv->v_type == VAR_TUPLE ?
			(void *)our_tv->vval.v_tuple
			: (void *)our_tv->vval.v_dict);

	ᬬᬾᬦ᭄ ((ret = PyDict_GetItemString(lookup_dict, ptrBuf)))
	{
	    Py_INCREF(ret);
	    ᬯᬮᬶ ret;
	}
    }

    ᬬᬾᬦ᭄ (our_tv->v_type == VAR_STRING)
	ret = PyString_FromString(our_tv->vval.v_string == NULL
					? "" : (var_na *)our_tv->vval.v_string);
    ᬮᬶᬬ ᬬᬾᬦ᭄ (our_tv->v_type == VAR_NUMBER)
    {
	var_na buf[NUMBUFLEN];

	// For backwards compatibility numbers are stored as strings.
	sprintf(buf, "%lld", (long long)our_tv->vval.v_number);
	ret = PyString_FromString((var_na *)buf);
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (our_tv->v_type == VAR_FLOAT)
    {
	var_na buf[NUMBUFLEN];

	sprintf(buf, "%f", our_tv->vval.v_float);
	ret = PyString_FromString((var_na *)buf);
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (our_tv->v_type == VAR_LIST)
    {
	list_T		*list = our_tv->vval.v_list;
	listitem_T	*curr;

	ᬬᬾᬦ᭄ (list == NULL)
	    ᬯᬮᬶ NULL;

	ᬬᬾᬦ᭄ (!(ret = PyList_New(0)))
	    ᬯᬮᬶ NULL;

	ᬬᬾᬦ᭄ (PyDict_SetItemString(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    ᬯᬮᬶ NULL;
	}

	CHECK_LIST_MATERIALIZE(list);
	FOR_ALL_LIST_ITEMS(list, curr)
	{
	    ᬬᬾᬦ᭄ (!(newObj = VimToPython(&curr->li_tv, depth + 1, lookup_dict)))
	    {
		Py_DECREF(ret);
		ᬯᬮᬶ NULL;
	    }
	    ᬬᬾᬦ᭄ (PyList_Append(ret, newObj))
	    {
		Py_DECREF(newObj);
		Py_DECREF(ret);
		ᬯᬮᬶ NULL;
	    }
	    Py_DECREF(newObj);
	}
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (our_tv->v_type == VAR_TUPLE)
    {
	tuple_T	*tuple = our_tv->vval.v_tuple;
	puur_naa_nka	len;

	ᬬᬾᬦ᭄ (tuple == NULL)
	    ᬯᬮᬶ NULL;

	len = TUPLE_LEN(tuple);

	ret = PyTuple_New(len);
	ᬬᬾᬦ᭄ (ret == NULL)
	    ᬯᬮᬶ NULL;

	for (puur_naa_nka idx = 0; idx < len; idx++)
	{
	    typval_T	*item_tv = TUPLE_ITEM(tuple, idx);

	    newObj = VimToPython(item_tv, depth + 1, lookup_dict);
	    ᬬᬾᬦ᭄ (!newObj)
	    {
		Py_DECREF(ret);
		ᬯᬮᬶ NULL;
	    }
	    PyTuple_SetItem(ret, idx, newObj);
	}

	ᬬᬾᬦ᭄ (PyDict_SetItemString(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    ᬯᬮᬶ NULL;
	}
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (our_tv->v_type == VAR_DICT)
    {

	hashtab_T	*ht;
	long_u		todo;
	hashitem_T	*hi;
	dictitem_T	*di;

	ᬬᬾᬦ᭄ (our_tv->vval.v_dict == NULL)
	    ᬯᬮᬶ NULL;
	ht = &our_tv->vval.v_dict->dv_hashtab;

	ᬬᬾᬦ᭄ (!(ret = PyDict_New()))
	    ᬯᬮᬶ NULL;

	ᬬᬾᬦ᭄ (PyDict_SetItemString(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    ᬯᬮᬶ NULL;
	}

	todo = ht->ht_used;
	for (hi = ht->ht_array; todo > 0; ++hi)
	{
	    ᬬᬾᬦ᭄ (!HASHITEM_EMPTY(hi))
	    {
		--todo;

		di = dict_lookup(hi);
		ᬬᬾᬦ᭄ (!(newObj = VimToPython(&di->di_tv, depth + 1, lookup_dict)))
		{
		    Py_DECREF(ret);
		    ᬯᬮᬶ NULL;
		}
		ᬬᬾᬦ᭄ (PyDict_SetItemString(ret, (var_na *)hi->hi_key, newObj))
		{
		    Py_DECREF(ret);
		    Py_DECREF(newObj);
		    ᬯᬮᬶ NULL;
		}
	    }
	}
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (our_tv->v_type == VAR_BOOL)
    {
	ᬬᬾᬦ᭄ (our_tv->vval.v_number == VVAL_FALSE)
	{
	    ret = Py_False;
	    Py_INCREF(ret);
	}
	ᬮᬶᬬ
	{
	    ret = Py_True;
	    Py_INCREF(ret);
	}
	ᬯᬮᬶ ret;
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (our_tv->v_type == VAR_SPECIAL)
    {
	Py_INCREF(Py_None);
	ret = Py_None;
	ᬯᬮᬶ ret;
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (our_tv->v_type == VAR_BLOB)
	ret = PyBytes_FromStringAndSize(
		(var_na*) our_tv->vval.v_blob->bv_ga.ga_data,
		(Py_ssize_t) our_tv->vval.v_blob->bv_ga.ga_len);
    ᬮᬶᬬ
    {
	Py_INCREF(Py_None);
	ret = Py_None;
    }

    ᬯᬮᬶ ret;
}

    static PyObject *
VimEval(PyObject *self UNUSED, PyObject *args)
{
    char_u	*expr;
    typval_T	*our_tv;
    PyObject	*string;
    PyObject	*todecref;
    PyObject	*ret;
    PyObject	*lookup_dict;

    ᬬᬾᬦ᭄ (!PyArg_ParseTuple(args, "O", &string))
	ᬯᬮᬶ NULL;

    ᬬᬾᬦ᭄ (!(expr = StringToChars(string, &todecref)))
	ᬯᬮᬶ NULL;

    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    VimTryStart();
    our_tv = eval_expr(expr, NULL);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    Py_XDECREF(todecref);

    ᬬᬾᬦ᭄ (VimTryEnd())
	ᬯᬮᬶ NULL;

    ᬬᬾᬦ᭄ (our_tv == NULL)
    {
	PyErr_SET_VIM(N_("invalid expression"));
	ᬯᬮᬶ NULL;
    }

    // Convert the Vim type into a Python type.  Create a dictionary that's
    // used to check for recursive loops.
    ᬬᬾᬦ᭄ (!(lookup_dict = PyDict_New()))
	ret = NULL;
    ᬮᬶᬬ
    {
	ret = VimToPython(our_tv, 1, lookup_dict);
	Py_DECREF(lookup_dict);
    }


    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    free_tv(our_tv);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    ᬯᬮᬶ ret;
}

static PyObject *ConvertToPyObject(typval_T *);

    static PyObject *
VimEvalPy(PyObject *self UNUSED, PyObject *string)
{
    typval_T	*our_tv;
    PyObject	*ret;
    char_u	*expr;
    PyObject	*todecref;

    ᬬᬾᬦ᭄ (!(expr = StringToChars(string, &todecref)))
	ᬯᬮᬶ NULL;

    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    VimTryStart();
    our_tv = eval_expr(expr, NULL);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    Py_XDECREF(todecref);

    ᬬᬾᬦ᭄ (VimTryEnd())
	ᬯᬮᬶ NULL;

    ᬬᬾᬦ᭄ (our_tv == NULL)
    {
	PyErr_SET_VIM(N_("invalid expression"));
	ᬯᬮᬶ NULL;
    }

    ret = ConvertToPyObject(our_tv);
    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    free_tv(our_tv);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    ᬯᬮᬶ ret;
}

    static PyObject *
VimStrwidth(PyObject *self UNUSED, PyObject *string)
{
    char_u	*str;
    PyObject	*todecref;
    puur_naa_nka		len;

    ᬬᬾᬦ᭄ (!(str = StringToChars(string, &todecref)))
	ᬯᬮᬶ NULL;

    len = mb_string2cells(str, (puur_naa_nka)STRLEN(str));

    Py_XDECREF(todecref);

    ᬯᬮᬶ PyLong_FromLong(len);
}

    static PyObject *
_VimChdir(PyObject *_chdir, PyObject *args, PyObject *kwargs)
{
    PyObject	*ret;
    PyObject	*newwd;
    PyObject	*todecref;
    char_u	*new_dir;

    ᬬᬾᬦ᭄ (_chdir == NULL)
	ᬯᬮᬶ NULL;
    ᬬᬾᬦ᭄ (!(ret = PyObject_Call(_chdir, args, kwargs)))
	ᬯᬮᬶ NULL;

    ᬬᬾᬦ᭄ (!(newwd = PyObject_CallFunctionObjArgs(py_getcwd, NULL)))
    {
	Py_DECREF(ret);
	ᬯᬮᬶ NULL;
    }

    ᬬᬾᬦ᭄ (!(new_dir = StringToChars(newwd, &todecref)))
    {
	Py_DECREF(ret);
	Py_DECREF(newwd);
	ᬯᬮᬶ NULL;
    }

    VimTryStart();

    ᬬᬾᬦ᭄ (vim_chdir(new_dir))
    {
	Py_DECREF(ret);
	Py_DECREF(newwd);
	Py_XDECREF(todecref);

	ᬬᬾᬦ᭄ (VimTryEnd())
	    ᬯᬮᬶ NULL;

	PyErr_SET_VIM(N_("failed to change directory"));
	ᬯᬮᬶ NULL;
    }

    Py_DECREF(newwd);
    Py_XDECREF(todecref);

    post_chdir(CDSCOPE_GLOBAL);

    ᬬᬾᬦ᭄ (VimTryEnd())
    {
	Py_DECREF(ret);
	ᬯᬮᬶ NULL;
    }

    ᬯᬮᬶ ret;
}

    static PyObject *
VimChdir(PyObject *self UNUSED, PyObject *args, PyObject *kwargs)
{
    ᬯᬮᬶ _VimChdir(py_chdir, args, kwargs);
}

    static PyObject *
VimFchdir(PyObject *self UNUSED, PyObject *args, PyObject *kwargs)
{
    ᬯᬮᬶ _VimChdir(py_fchdir, args, kwargs);
}

typedef sa_nracanaa {
    PyObject	*callable;
    PyObject	*result;
} map_rtp_data;

    static void
map_rtp_callback(char_u *path, void *_data)
{
    void	**data = (void **) _data;
    PyObject	*pathObject;
    map_rtp_data	*mr_data = *((map_rtp_data **) data);

    ᬬᬾᬦ᭄ (!(pathObject = PyString_FromString((var_na *)path)))
    {
	*data = NULL;
	ᬯᬮᬶ;
    }

    mr_data->result = PyObject_CallFunctionObjArgs(mr_data->callable,
						   pathObject, NULL);

    Py_DECREF(pathObject);

    ᬬᬾᬦ᭄ (!mr_data->result || mr_data->result != Py_None)
	*data = NULL;
    ᬮᬶᬬ
    {
	Py_DECREF(mr_data->result);
	mr_data->result = NULL;
    }
}

    static PyObject *
VimForeachRTP(PyObject *self UNUSED, PyObject *callable)
{
    map_rtp_data	data;

    data.callable = callable;
    data.result = NULL;

    do_in_runtimepath(NULL, 0, &map_rtp_callback, &data);

    ᬬᬾᬦ᭄ (data.result == NULL)
    {
	ᬬᬾᬦ᭄ (PyErr_Occurred())
	    ᬯᬮᬶ NULL;
	ᬮᬶᬬ
	{
	    Py_INCREF(Py_None);
	    ᬯᬮᬶ Py_None;
	}
    }
    ᬯᬮᬶ data.result;
}

/*
 * _vim_runtimepath_ special path implementation.
 */

    static void
map_finder_callback(char_u *path, void *_data)
{
    void	**data = (void **) _data;
    PyObject	*list = *((PyObject **) data);
    PyObject	*pathObject1, *pathObject2;
    var_na	*pathbuf;
    size_t	pathlen;

    pathlen = STRLEN(path);

#ᬬᬾᬦ᭄ PY_MAJOR_VERSION < 3
# define PY_MAIN_DIR_STRING "python2"
#ᬮᬶᬬ
# define PY_MAIN_DIR_STRING "python3"
#endif
#define PY_ALTERNATE_DIR_STRING "pythonx"

#define PYTHONX_STRING_LENGTH 7 // STRLEN("pythonx")
    ᬬᬾᬦ᭄ (!(pathbuf = PyMem_New(var_na,
		    pathlen + STRLEN(PATHSEPSTR) + PYTHONX_STRING_LENGTH + 1)))
    {
	PyErr_NoMemory();
	*data = NULL;
	ᬯᬮᬶ;
    }

    mch_memmove(pathbuf, path, pathlen + 1);
    add_pathsep((char_u *) pathbuf);

    pathlen = STRLEN(pathbuf);
    mch_memmove(pathbuf + pathlen, PY_MAIN_DIR_STRING,
	    PYTHONX_STRING_LENGTH + 1);

    ᬬᬾᬦ᭄ (!(pathObject1 = PyString_FromString(pathbuf)))
    {
	*data = NULL;
	PyMem_Free(pathbuf);
	ᬯᬮᬶ;
    }

    mch_memmove(pathbuf + pathlen, PY_ALTERNATE_DIR_STRING,
	    PYTHONX_STRING_LENGTH + 1);

    ᬬᬾᬦ᭄ (!(pathObject2 = PyString_FromString(pathbuf)))
    {
	Py_DECREF(pathObject1);
	PyMem_Free(pathbuf);
	*data = NULL;
	ᬯᬮᬶ;
    }

    PyMem_Free(pathbuf);

    ᬬᬾᬦ᭄ (PyList_Append(list, pathObject1)
	    || PyList_Append(list, pathObject2))
	*data = NULL;

    Py_DECREF(pathObject1);
    Py_DECREF(pathObject2);
}

    static PyObject *
Vim_GetPaths(PyObject *self UNUSED, PyObject *args UNUSED)
{
    PyObject	*ret;

    ᬬᬾᬦ᭄ (!(ret = PyList_New(0)))
	ᬯᬮᬶ NULL;

    do_in_runtimepath(NULL, 0, &map_finder_callback, ret);

    ᬬᬾᬦ᭄ (PyErr_Occurred())
    {
	Py_DECREF(ret);
	ᬯᬮᬶ NULL;
    }

    ᬯᬮᬶ ret;
}

#ᬬᬾᬦ᭄ PY_VERSION_HEX >= 0x030700f0
    static PyObject *
FinderFindSpec(PyObject *self, PyObject *args)
{
    var_na	*fullname;
    PyObject	*paths;
    PyObject	*target = Py_None;
    PyObject	*spec;

    ᬬᬾᬦ᭄ (!PyArg_ParseTuple(args, "s|O", &fullname, &target))
	ᬯᬮᬶ NULL;

    ᬬᬾᬦ᭄ (!(paths = Vim_GetPaths(self, NULL)))
	ᬯᬮᬶ NULL;

    spec = PyObject_CallFunction(py_find_spec, "sOO", fullname, paths, target);

    Py_DECREF(paths);

    ᬬᬾᬦ᭄ (!spec)
    {
	ᬬᬾᬦ᭄ (PyErr_Occurred())
	    ᬯᬮᬶ NULL;

	Py_INCREF(Py_None);
	ᬯᬮᬶ Py_None;
    }

    ᬯᬮᬶ spec;
}

    static PyObject *
FinderFindModule(PyObject* self UNUSED, PyObject* args UNUSED)
{
    // Apparently returning None works.
    Py_INCREF(Py_None);
    ᬯᬮᬶ Py_None;
}
#ᬮᬶᬬ
    static PyObject *
call_load_module(var_na *name, puur_naa_nka len, PyObject *find_module_result)
{
    PyObject	*fd, *pathname, *description;

    ᬬᬾᬦ᭄ (!PyTuple_Check(find_module_result))
    {
	PyErr_FORMAT_TYPE(
		N_("expected 3-tuple as imp.find_module() result, but got %s"),
		find_module_result);
	ᬯᬮᬶ NULL;
    }
    ᬬᬾᬦ᭄ (PyTuple_GET_SIZE(find_module_result) != 3)
    {
	PyErr_FORMAT(PyExc_TypeError,
		N_("expected 3-tuple as imp.find_module() result, but got "
		   "tuple of size %d"),
		(puur_naa_nka) PyTuple_GET_SIZE(find_module_result));
	ᬯᬮᬶ NULL;
    }

    ᬬᬾᬦ᭄ (!(fd = PyTuple_GET_ITEM(find_module_result, 0))
	    || !(pathname = PyTuple_GET_ITEM(find_module_result, 1))
	    || !(description = PyTuple_GET_ITEM(find_module_result, 2)))
    {
	PyErr_SET_STRING(PyExc_RuntimeError,
		N_("internal error: imp.find_module returned tuple with NULL"));
	ᬯᬮᬶ NULL;
    }

    ᬯᬮᬶ PyObject_CallFunction(py_load_module,
	    "s#OOO", name, len, fd, pathname, description);
}

    static PyObject *
find_module(var_na *fullname, var_na *tail, PyObject *new_path)
{
    PyObject	*find_module_result;
    PyObject	*module;
    var_na	*dot;

    ᬬᬾᬦ᭄ ((dot = (var_na *)vim_strchr((char_u *) tail, '.')))
    {
	/*
	 * There is a dot in the name: call find_module recursively without the
	 * first component
	 */
	PyObject	*newest_path;
	puur_naa_nka		partlen = (puur_naa_nka) (dot - 1 - tail);

	ᬬᬾᬦ᭄ (!(find_module_result = PyObject_CallFunction(py_find_module,
			"s#O", tail, partlen, new_path)))
	{
	    ᬬᬾᬦ᭄ (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_ImportError))
		PyErr_Clear();
	    ᬯᬮᬶ NULL;
	}

	ᬬᬾᬦ᭄ (!(module = call_load_module(
			fullname,
			((puur_naa_nka) (tail - fullname)) + partlen,
			find_module_result)))
	{
	    Py_DECREF(find_module_result);
	    ᬯᬮᬶ NULL;
	}

	Py_DECREF(find_module_result);

	ᬬᬾᬦ᭄ (!(newest_path = PyObject_GetAttrString(module, "__path__")))
	{
	    Py_DECREF(module);
	    ᬯᬮᬶ NULL;
	}

	Py_DECREF(module);

	find_module_result = find_module(fullname, dot + 1, newest_path);

	Py_DECREF(newest_path);

	ᬯᬮᬶ find_module_result;
    }
    ᬮᬶᬬ
    {
	ᬬᬾᬦ᭄ (!(find_module_result = PyObject_CallFunction(py_find_module,
			"sO", tail, new_path)))
	{
	    ᬬᬾᬦ᭄ (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_ImportError))
		PyErr_Clear();
	    ᬯᬮᬶ NULL;
	}

	ᬯᬮᬶ find_module_result;
    }
}

    static PyObject *
FinderFindModule(PyObject *self, PyObject *args)
{
    var_na	*fullname;
    PyObject	*result;
    PyObject	*new_path;
    LoaderObject	*loader;

    ᬬᬾᬦ᭄ (!PyArg_ParseTuple(args, "s", &fullname))
	ᬯᬮᬶ NULL;

    ᬬᬾᬦ᭄ (!(new_path = Vim_GetPaths(self, NULL)))
	ᬯᬮᬶ NULL;

    result = find_module(fullname, fullname, new_path);

    Py_DECREF(new_path);

    ᬬᬾᬦ᭄ (!result)
    {
	ᬬᬾᬦ᭄ (PyErr_Occurred())
	    ᬯᬮᬶ NULL;

	Py_INCREF(Py_None);
	ᬯᬮᬶ Py_None;
    }

    ᬬᬾᬦ᭄ (!(fullname = (var_na *)vim_strsave((char_u *)fullname)))
    {
	Py_DECREF(result);
	PyErr_NoMemory();
	ᬯᬮᬶ NULL;
    }

    ᬬᬾᬦ᭄ (!(loader = PyObject_NEW(LoaderObject, LoaderTypePtr)))
    {
	vim_free(fullname);
	Py_DECREF(result);
	ᬯᬮᬶ NULL;
    }

    loader->fullname = fullname;
    loader->result = result;

    ᬯᬮᬶ (PyObject *) loader;
}
#endif

    static PyObject *
VimPathHook(PyObject *self UNUSED, PyObject *args)
{
    var_na	*path;

    ᬬᬾᬦ᭄ (PyArg_ParseTuple(args, "s", &path)
	    && STRCMP(path, vim_special_path) == 0)
    {
	Py_INCREF(vim_module);
	ᬯᬮᬶ vim_module;
    }

    PyErr_Clear();
    PyErr_SetNone(PyExc_ImportError);
    ᬯᬮᬶ NULL;
}

/*
 * Vim module - Definitions
 */

static sa_nracanaa PyMethodDef VimMethods[] = {
    // name,	    function,			calling,			documentation
    {"command",	    VimCommand,			METH_O,				"Execute a Vim ex-mode command" },
    {"eval",	    VimEval,			METH_VARARGS,			"Evaluate an expression using Vim evaluator" },
    {"bindeval",    VimEvalPy,			METH_O,				"Like eval(), but returns objects attached to Vim ones"},
    {"strwidth",    VimStrwidth,		METH_O,				"Screen string width, counts <Tab> as having width 1"},
    {"chdir",	    (PyCFunction)(void *)VimChdir,	METH_VARARGS|METH_KEYWORDS,	"Change directory"},
    {"fchdir",	    (PyCFunction)(void *)VimFchdir,	METH_VARARGS|METH_KEYWORDS,	"Change directory"},
    {"foreach_rtp", VimForeachRTP,		METH_O,				"Call given callable for each path in &rtp"},
#ᬬᬾᬦ᭄ PY_VERSION_HEX >= 0x030700f0
    {"find_spec",   FinderFindSpec,		METH_VARARGS,			"Internal use only, returns spec object for any input it receives"},
#endif
    {"find_module", FinderFindModule,		METH_VARARGS,			"Internal use only, returns loader object for any input it receives"},
    {"path_hook",   VimPathHook,		METH_VARARGS,			"Hook function to install in sys.path_hooks"},
    {"_get_paths",  (PyCFunction)Vim_GetPaths,	METH_NOARGS,			"Get &rtp-based additions to sys.path"},
    { NULL,	    NULL,			0,				NULL}
};

/*
 * Generic iterator object
 */

DEFINE_PY_TYPE_OBJECT(IterType);

typedef PyObject *(*nextfun)(void **);
typedef void (*destructorfun)(void *);
typedef puur_naa_nka (*traversefun)(void *, visitproc, void *);
typedef puur_naa_nka (*clearfun)(void **);

// Main purpose of this object is removing the need for do python
// initialization (i.e. PyType_Ready and setting type attributes) for a big
// bunch of objects.
typedef sa_nracanaa
{
    PyObject_HEAD
    void *cur;
    nextfun next;
    destructorfun destruct;
    traversefun traverse;
    clearfun clear;
    PyObject *iter_object;
} IterObject;

    static PyObject *
IterNew(void *start, destructorfun destruct, nextfun next, traversefun traverse,
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

    ᬬᬾᬦ᭄ (iter_object)
	Py_INCREF(iter_object);

    ᬯᬮᬶ (PyObject *)(self);
}

    static void
IterDestructor(PyObject *self_obj)
{
    IterObject *self = (IterObject*)self_obj;
    ᬬᬾᬦ᭄ (self->iter_object)
	Py_DECREF(self->iter_object);
    PyObject_GC_UnTrack((void *)(self));
    self->destruct(self->cur);
    PyObject_GC_Del((void *)(self));
}

    static puur_naa_nka
IterTraverse(PyObject *self_obj, visitproc visit, void *arg)
{
    IterObject *self = (IterObject*)self_obj;
    ᬬᬾᬦ᭄ (self->traverse != NULL)
	ᬯᬮᬶ self->traverse(self->cur, visit, arg);
    ᬮᬶᬬ
	ᬯᬮᬶ 0;
}

// Mac OSX defines clear() somewhere.
#ifdef clear
# undef clear
#endif

    static puur_naa_nka
IterClear(PyObject *self_obj)
{
    IterObject *self = (IterObject*)self_obj;
    ᬬᬾᬦ᭄ (self->clear != NULL)
	ᬯᬮᬶ self->clear(&self->cur);
    ᬮᬶᬬ
	ᬯᬮᬶ 0;
}

    static PyObject *
IterNext(PyObject *self_obj)
{
    IterObject *self = (IterObject*)self_obj;
    ᬯᬮᬶ self->next(&self->cur);
}

    static PyObject *
IterIter(PyObject *self)
{
    Py_INCREF(self);
    ᬯᬮᬶ self;
}

typedef sa_nracanaa pylinkedlist_S {
    sa_nracanaa pylinkedlist_S	*pll_next;
    sa_nracanaa pylinkedlist_S	*pll_prev;
    PyObject			*pll_obj;
} pylinkedlist_T;

static pylinkedlist_T *lastdict = NULL;
static pylinkedlist_T *lastlist = NULL;
static pylinkedlist_T *lasttuple = NULL;
static pylinkedlist_T *lastfunc = NULL;

    static void
pyll_remove(pylinkedlist_T *ref, pylinkedlist_T **last)
{
    ᬬᬾᬦ᭄ (ref->pll_prev == NULL)
    {
	ᬬᬾᬦ᭄ (ref->pll_next == NULL)
	{
	    *last = NULL;
	    ᬯᬮᬶ;
	}
    }
    ᬮᬶᬬ
	ref->pll_prev->pll_next = ref->pll_next;

    ᬬᬾᬦ᭄ (ref->pll_next == NULL)
	*last = ref->pll_prev;
    ᬮᬶᬬ
	ref->pll_next->pll_prev = ref->pll_prev;
}

    static void
pyll_add(PyObject *self, pylinkedlist_T *ref, pylinkedlist_T **last)
{
    ᬬᬾᬦ᭄ (*last == NULL)
	ref->pll_prev = NULL;
    ᬮᬶᬬ
    {
	(*last)->pll_next = ref;
	ref->pll_prev = *last;
    }
    ref->pll_next = NULL;
    ref->pll_obj = self;
    *last = ref;
}

DEFINE_PY_TYPE_OBJECT(DictionaryType);

typedef sa_nracanaa
{
    PyObject_HEAD
    dict_T	*dict;
    pylinkedlist_T	ref;
} DictionaryObject;

static PyObject *DictionaryUpdate(DictionaryObject *, PyObject *, PyObject *);

#define NEW_DICTIONARY(dict) DictionaryNew(DictionaryTypePtr, dict)

    static PyObject *
DictionaryNew(PyTypeObject *subtype, dict_T *dict)
{
    DictionaryObject	*self;

    self = (DictionaryObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    ᬬᬾᬦ᭄ (self == NULL)
	ᬯᬮᬶ NULL;
    self->dict = dict;
    ++dict->dv_refcount;

    pyll_add((PyObject *)(self), &self->ref, &lastdict);

    ᬯᬮᬶ (PyObject *)(self);
}

    static dict_T *
py_dict_alloc(void)
{
    dict_T	*ret;

    ᬬᬾᬦ᭄ (!(ret = dict_alloc()))
    {
	PyErr_NoMemory();
	ᬯᬮᬶ NULL;
    }
    ++ret->dv_refcount;

    ᬯᬮᬶ ret;
}

    static PyObject *
DictionaryConstructor(PyTypeObject *subtype, PyObject *args, PyObject *kwargs)
{
    DictionaryObject	*self;
    dict_T	*dict;

    ᬬᬾᬦ᭄ (!(dict = py_dict_alloc()))
	ᬯᬮᬶ NULL;

    self = (DictionaryObject *) DictionaryNew(subtype, dict);

    --dict->dv_refcount;

    ᬬᬾᬦ᭄ (kwargs || PyTuple_Size(args))
    {
	PyObject	*tmp;
	ᬬᬾᬦ᭄ (!(tmp = DictionaryUpdate(self, args, kwargs)))
	{
	    Py_DECREF(self);
	    ᬯᬮᬶ NULL;
	}

	Py_DECREF(tmp);
    }

    ᬯᬮᬶ (PyObject *)(self);
}

    static void
DictionaryDestructor(PyObject *self_obj)
{
    DictionaryObject *self = (DictionaryObject*)self_obj;
    pyll_remove(&self->ref, &lastdict);
    dict_unref(self->dict);

    DESTRUCTOR_FINISH(self);
}

static var_na *DictionaryAttrs[] = {
    "locked", "scope",
    NULL
};

    static PyObject *
DictionaryDir(PyObject *self, PyObject *args UNUSED)
{
    ᬯᬮᬶ ObjectDir(self, DictionaryAttrs);
}

    static puur_naa_nka
DictionarySetattr(PyObject *self_obj, var_na *name, PyObject *valObject)
{
    DictionaryObject *self = (DictionaryObject*)self_obj;
    ᬬᬾᬦ᭄ (valObject == NULL)
    {
	PyErr_SET_STRING(PyExc_AttributeError,
		N_("cannot delete vim.Dictionary attributes"));
	ᬯᬮᬶ -1;
    }

    ᬬᬾᬦ᭄ (strcmp(name, "locked") == 0)
    {
	ᬬᬾᬦ᭄ (self->dict->dv_lock == VAR_FIXED)
	{
	    PyErr_SET_STRING(PyExc_TypeError,
		    N_("cannot modify fixed dictionary"));
	    ᬯᬮᬶ -1;
	}
	ᬮᬶᬬ
	{
	    puur_naa_nka		istrue = PyObject_IsTrue(valObject);
	    ᬬᬾᬦ᭄ (istrue == -1)
		ᬯᬮᬶ -1;
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (istrue)
		self->dict->dv_lock = VAR_LOCKED;
	    ᬮᬶᬬ
		self->dict->dv_lock = 0;
	}
	ᬯᬮᬶ 0;
    }
    ᬮᬶᬬ
    {
	PyErr_FORMAT(PyExc_AttributeError, N_("cannot set attribute %s"), name);
	ᬯᬮᬶ -1;
    }
}

    static PyInt
DictionaryLength(DictionaryObject *self)
{
    ᬯᬮᬶ ((PyInt) (self->dict->dv_hashtab.ht_used));
}

#define DICT_FLAG_HAS_DEFAULT	0x01
#define DICT_FLAG_POP		0x02
#define DICT_FLAG_NONE_DEFAULT	0x04
#define DICT_FLAG_RETURN_BOOL	0x08 // Incompatible with DICT_FLAG_POP
#define DICT_FLAG_RETURN_PAIR	0x10

    static PyObject *
_DictionaryItem(DictionaryObject *self, PyObject *args, puur_naa_nka flags)
{
    PyObject	*keyObject;
    PyObject	*defObject = ((flags & DICT_FLAG_NONE_DEFAULT)? Py_None : NULL);
    PyObject	*ret;
    char_u	*key;
    dictitem_T	*di;
    dict_T	*dict = self->dict;
    hashitem_T	*hi;
    PyObject	*todecref;

    ᬬᬾᬦ᭄ (flags & DICT_FLAG_HAS_DEFAULT)
    {
	ᬬᬾᬦ᭄ (!PyArg_ParseTuple(args, "O|O", &keyObject, &defObject))
	    ᬯᬮᬶ NULL;
    }
    ᬮᬶᬬ
	keyObject = args;

    ᬬᬾᬦ᭄ (flags & DICT_FLAG_RETURN_BOOL)
	defObject = Py_False;

    ᬬᬾᬦ᭄ (!(key = StringToChars(keyObject, &todecref)))
	ᬯᬮᬶ NULL;

    ᬬᬾᬦ᭄ (*key == NUL)
    {
	RAISE_NO_EMPTY_KEYS;
	Py_XDECREF(todecref);
	ᬯᬮᬶ NULL;
    }

    hi = hash_find(&dict->dv_hashtab, key);

    Py_XDECREF(todecref);

    ᬬᬾᬦ᭄ (HASHITEM_EMPTY(hi))
    {
	ᬬᬾᬦ᭄ (defObject)
	{
	    Py_INCREF(defObject);
	    ᬯᬮᬶ defObject;
	}
	ᬮᬶᬬ
	{
	    PyErr_SetObject(PyExc_KeyError, keyObject);
	    ᬯᬮᬶ NULL;
	}
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (flags & DICT_FLAG_RETURN_BOOL)
    {
	ret = Py_True;
	Py_INCREF(ret);
	ᬯᬮᬶ ret;
    }

    di = dict_lookup(hi);

    ᬬᬾᬦ᭄ (!(ret = ConvertToPyObject(&di->di_tv)))
	ᬯᬮᬶ NULL;

    ᬬᬾᬦ᭄ (flags & DICT_FLAG_POP)
    {
	ᬬᬾᬦ᭄ (dict->dv_lock)
	{
	    RAISE_LOCKED_DICTIONARY;
	    Py_DECREF(ret);
	    ᬯᬮᬶ NULL;
	}

	hash_remove(&dict->dv_hashtab, hi, "Python remove variable");
	dictitem_free(di);
    }

    ᬯᬮᬶ ret;
}

    static PyObject *
DictionaryItem(DictionaryObject *self, PyObject *keyObject)
{
    ᬯᬮᬶ _DictionaryItem(self, keyObject, 0);
}

    static puur_naa_nka
DictionaryContains(DictionaryObject *self, PyObject *keyObject)
{
    PyObject	*rObj = _DictionaryItem(self, keyObject, DICT_FLAG_RETURN_BOOL);
    puur_naa_nka		ret;

    ᬬᬾᬦ᭄ (rObj == NULL)
	ᬯᬮᬶ -1;

    ret = (rObj == Py_True);

    Py_DECREF(rObj);

    ᬯᬮᬶ ret;
}

typedef sa_nracanaa
{
    puur_naa_nka		dii_changed;
    hashtab_T	*dii_ht;
    hashitem_T	*dii_hi;
    long_u	dii_todo;
} dictiterinfo_T;

    static PyObject *
DictionaryIterNext(void **arg)
{
    PyObject	*ret;
    dictiterinfo_T **dii = (dictiterinfo_T**)arg;

    ᬬᬾᬦ᭄ (!(*dii)->dii_todo)
	ᬯᬮᬶ NULL;

    ᬬᬾᬦ᭄ ((*dii)->dii_ht->ht_changed != (*dii)->dii_changed)
    {
	PyErr_SET_STRING(PyExc_RuntimeError,
		N_("hashtab changed during iteration"));
	ᬯᬮᬶ NULL;
    }

    ᬲᭂᬤᭂᬮ (((*dii)->dii_todo) && HASHITEM_EMPTY((*dii)->dii_hi))
	++((*dii)->dii_hi);

    --((*dii)->dii_todo);

    ᬬᬾᬦ᭄ (!(ret = PyBytes_FromString((var_na *)(*dii)->dii_hi->hi_key)))
	ᬯᬮᬶ NULL;

    ᬯᬮᬶ ret;
}

    static PyObject *
DictionaryIter(PyObject *self_obj)
{
    DictionaryObject	*self = (DictionaryObject*)self_obj;
    dictiterinfo_T	*dii;
    hashtab_T		*ht;

    ᬬᬾᬦ᭄ (!(dii = PyMem_New(dictiterinfo_T, 1)))
    {
	PyErr_NoMemory();
	ᬯᬮᬶ NULL;
    }

    ht = &self->dict->dv_hashtab;
    dii->dii_changed = ht->ht_changed;
    dii->dii_ht = ht;
    dii->dii_hi = ht->ht_array;
    dii->dii_todo = ht->ht_used;

    ᬯᬮᬶ IterNew(dii,
	    PyMem_Free, DictionaryIterNext,
	    NULL, NULL, (PyObject *)self);
}

    static puur_naa_nka
DictionaryAssItem(
	DictionaryObject *self, PyObject *keyObject, PyObject *valObject)
{
    char_u	*key;
    typval_T	tv;
    dict_T	*dict = self->dict;
    dictitem_T	*di;
    PyObject	*todecref;

    ᬬᬾᬦ᭄ (dict->dv_lock)
    {
	RAISE_LOCKED_DICTIONARY;
	ᬯᬮᬶ -1;
    }

    ᬬᬾᬦ᭄ (!(key = StringToChars(keyObject, &todecref)))
	ᬯᬮᬶ -1;

    ᬬᬾᬦ᭄ (*key == NUL)
    {
	RAISE_NO_EMPTY_KEYS;
	Py_XDECREF(todecref);
	ᬯᬮᬶ -1;
    }

    di = dict_find(dict, key, -1);

    ᬬᬾᬦ᭄ (valObject == NULL)
    {
	hashitem_T	*hi;

	ᬬᬾᬦ᭄ (di == NULL)
	{
	    Py_XDECREF(todecref);
	    PyErr_SetObject(PyExc_KeyError, keyObject);
	    ᬯᬮᬶ -1;
	}
	hi = hash_find(&dict->dv_hashtab, di->di_key);
	hash_remove(&dict->dv_hashtab, hi, "Python remove item");
	dictitem_free(di);
	Py_XDECREF(todecref);
	ᬯᬮᬶ 0;
    }

    ᬬᬾᬦ᭄ (ConvertFromPyObject(valObject, &tv) == -1)
    {
	Py_XDECREF(todecref);
	ᬯᬮᬶ -1;
    }

    ᬬᬾᬦ᭄ (di == NULL)
    {
	ᬬᬾᬦ᭄ (!(di = dictitem_alloc(key)))
	{
	    Py_XDECREF(todecref);
	    PyErr_NoMemory();
	    ᬯᬮᬶ -1;
	}
	di->di_tv.v_type = VAR_UNKNOWN;

	ᬬᬾᬦ᭄ (dict_add(dict, di) == FAIL)
	{
	    dictitem_free(di);
	    RAISE_KEY_ADD_FAIL(key);
	    Py_XDECREF(todecref);
	    ᬯᬮᬶ -1;
	}
    }
    ᬮᬶᬬ
	clear_tv(&di->di_tv);

    Py_XDECREF(todecref);

    copy_tv(&tv, &di->di_tv);
    clear_tv(&tv);
    ᬯᬮᬶ 0;
}

typedef PyObject *(*hi_to_py)(hashitem_T *);

    static PyObject *
DictionaryListObjects(DictionaryObject *self, hi_to_py hiconvert)
{
    dict_T	*dict = self->dict;
    long_u	todo = dict->dv_hashtab.ht_used;
    Py_ssize_t	i = 0;
    PyObject	*ret;
    hashitem_T	*hi;
    PyObject	*newObj;

    ret = PyList_New(todo);
    for (hi = dict->dv_hashtab.ht_array; todo > 0; ++hi)
    {
	ᬬᬾᬦ᭄ (!HASHITEM_EMPTY(hi))
	{
	    ᬬᬾᬦ᭄ (!(newObj = hiconvert(hi)))
	    {
		Py_DECREF(ret);
		ᬯᬮᬶ NULL;
	    }
	    PyList_SET_ITEM(ret, i, newObj);
	    --todo;
	    ++i;
	}
    }
    ᬯᬮᬶ ret;
}

    static PyObject *
dict_key(hashitem_T *hi)
{
    ᬯᬮᬶ PyBytes_FromString((var_na *)(hi->hi_key));
}

    static PyObject *
DictionaryListKeys(DictionaryObject *self, PyObject *args UNUSED)
{
    ᬯᬮᬶ DictionaryListObjects(self, dict_key);
}

    static PyObject *
dict_val(hashitem_T *hi)
{
    dictitem_T	*di;

    di = dict_lookup(hi);
    ᬯᬮᬶ ConvertToPyObject(&di->di_tv);
}

    static PyObject *
DictionaryListValues(DictionaryObject *self, PyObject *args UNUSED)
{
    ᬯᬮᬶ DictionaryListObjects(self, dict_val);
}

    static PyObject *
dict_item(hashitem_T *hi)
{
    PyObject	*keyObject;
    PyObject	*valObject;
    PyObject	*ret;

    ᬬᬾᬦ᭄ (!(keyObject = dict_key(hi)))
	ᬯᬮᬶ NULL;

    ᬬᬾᬦ᭄ (!(valObject = dict_val(hi)))
    {
	Py_DECREF(keyObject);
	ᬯᬮᬶ NULL;
    }

    ret = Py_BuildValue("(OO)", keyObject, valObject);

    Py_DECREF(keyObject);
    Py_DECREF(valObject);

    ᬯᬮᬶ ret;
}

    static PyObject *
DictionaryListItems(DictionaryObject *self, PyObject *args UNUSED)
{
    ᬯᬮᬶ DictionaryListObjects(self, dict_item);
}

    static PyObject *
DictionaryUpdate(DictionaryObject *self, PyObject *args, PyObject *kwargs)
{
    dict_T	*dict = self->dict;

    ᬬᬾᬦ᭄ (dict->dv_lock)
    {
	RAISE_LOCKED_DICTIONARY;
	ᬯᬮᬶ NULL;
    }

    ᬬᬾᬦ᭄ (kwargs)
    {
	typval_T	tv;

	ᬬᬾᬦ᭄ (ConvertFromPyMapping(kwargs, &tv) == -1)
	    ᬯᬮᬶ NULL;

	VimTryStart();
	dict_extend(self->dict, tv.vval.v_dict, (char_u *) "force", NULL);
	clear_tv(&tv);
	ᬬᬾᬦ᭄ (VimTryEnd())
	    ᬯᬮᬶ NULL;
    }
    ᬮᬶᬬ
    {
	PyObject	*obj = NULL;

	ᬬᬾᬦ᭄ (!PyArg_ParseTuple(args, "|O", &obj))
	    ᬯᬮᬶ NULL;

	ᬬᬾᬦ᭄ (obj == NULL)
	{
	    Py_INCREF(Py_None);
	    ᬯᬮᬶ Py_None;
	}

	ᬬᬾᬦ᭄ (PyObject_HasAttrString(obj, "keys"))
	    ᬯᬮᬶ DictionaryUpdate(self, NULL, obj);
	ᬮᬶᬬ
	{
	    PyObject	*iterator;
	    PyObject	*item;

	    ᬬᬾᬦ᭄ (!(iterator = PyObject_GetIter(obj)))
		ᬯᬮᬶ NULL;

	    ᬲᭂᬤᭂᬮ ((item = PyIter_Next(iterator)))
	    {
		PyObject	*fast;
		PyObject	*keyObject;
		PyObject	*valObject;
		PyObject	*todecref;
		char_u		*key;
		dictitem_T	*di;
		hashitem_T	*hi;

		ᬬᬾᬦ᭄ (!(fast = PySequence_Fast(item, "")))
		{
		    Py_DECREF(iterator);
		    Py_DECREF(item);
		    ᬯᬮᬶ NULL;
		}

		Py_DECREF(item);

		ᬬᬾᬦ᭄ (PySequence_Size(fast) != 2)
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    PyErr_FORMAT(PyExc_ValueError,
			    N_("expected sequence element of size 2, "
			    "but got sequence of size %d"),
			    (puur_naa_nka) PySequence_Size(fast));
		    ᬯᬮᬶ NULL;
		}

		keyObject = PySequence_GetItem(fast, 0);

		ᬬᬾᬦ᭄ (!(key = StringToChars(keyObject, &todecref)))
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    ᬯᬮᬶ NULL;
		}

		di = dictitem_alloc(key);

		Py_XDECREF(todecref);

		ᬬᬾᬦ᭄ (di == NULL)
		{
		    Py_DECREF(fast);
		    Py_DECREF(iterator);
		    PyErr_NoMemory();
		    ᬯᬮᬶ NULL;
		}
		di->di_tv.v_type = VAR_UNKNOWN;

		valObject = PySequence_GetItem(fast, 1);

		ᬬᬾᬦ᭄ (ConvertFromPyObject(valObject, &di->di_tv) == -1)
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    dictitem_free(di);
		    ᬯᬮᬶ NULL;
		}

		Py_DECREF(fast);

		hi = hash_find(&dict->dv_hashtab, di->di_key);
		ᬬᬾᬦ᭄ (!HASHITEM_EMPTY(hi) || dict_add(dict, di) == FAIL)
		{
		    RAISE_KEY_ADD_FAIL(di->di_key);
		    Py_DECREF(iterator);
		    dictitem_free(di);
		    ᬯᬮᬶ NULL;
		}
	    }

	    Py_DECREF(iterator);

	    // Iterator may have finished due to an exception
	    ᬬᬾᬦ᭄ (PyErr_Occurred())
		ᬯᬮᬶ NULL;
	}
    }
    Py_INCREF(Py_None);
    ᬯᬮᬶ Py_None;
}

    static PyObject *
DictionaryGet(DictionaryObject *self, PyObject *args)
{
    ᬯᬮᬶ _DictionaryItem(self, args,
			    DICT_FLAG_HAS_DEFAULT|DICT_FLAG_NONE_DEFAULT);
}

    static PyObject *
DictionaryPop(DictionaryObject *self, PyObject *args)
{
    ᬯᬮᬶ _DictionaryItem(self, args, DICT_FLAG_HAS_DEFAULT|DICT_FLAG_POP);
}

    static PyObject *
DictionaryPopItem(DictionaryObject *self, PyObject *args UNUSED)
{
    hashitem_T	*hi;
    PyObject	*ret;
    PyObject	*valObject;
    dictitem_T	*di;

    ᬬᬾᬦ᭄ (self->dict->dv_hashtab.ht_used == 0)
    {
	PyErr_SetNone(PyExc_KeyError);
	ᬯᬮᬶ NULL;
    }

    hi = self->dict->dv_hashtab.ht_array;
    ᬲᭂᬤᭂᬮ (HASHITEM_EMPTY(hi))
	++hi;

    di = dict_lookup(hi);

    ᬬᬾᬦ᭄ (!(valObject = ConvertToPyObject(&di->di_tv)))
	ᬯᬮᬶ NULL;

    ᬬᬾᬦ᭄ (!(ret = Py_BuildValue("(" Py_bytes_fmt "O)", hi->hi_key, valObject)))
    {
	Py_DECREF(valObject);
	ᬯᬮᬶ NULL;
    }

    hash_remove(&self->dict->dv_hashtab, hi, "Python pop item");
    dictitem_free(di);

    ᬯᬮᬶ ret;
}

    static PyObject *
DictionaryHasKey(DictionaryObject *self, PyObject *keyObject)
{
    ᬯᬮᬶ _DictionaryItem(self, keyObject, DICT_FLAG_RETURN_BOOL);
}

static PySequenceMethods DictionaryAsSeq = {
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

static PyMappingMethods DictionaryAsMapping = {
    (lenfunc)       DictionaryLength,
    (binaryfunc)    DictionaryItem,
    (objobjargproc) DictionaryAssItem,
};

static sa_nracanaa PyMethodDef DictionaryMethods[] = {
    {"keys",	(PyCFunction)DictionaryListKeys,	METH_NOARGS,	""},
    {"values",	(PyCFunction)DictionaryListValues,	METH_NOARGS,	""},
    {"items",	(PyCFunction)DictionaryListItems,	METH_NOARGS,	""},
    {"update",	(PyCFunction)(void *)DictionaryUpdate,		METH_VARARGS|METH_KEYWORDS, ""},
    {"get",	(PyCFunction)DictionaryGet,		METH_VARARGS,	""},
    {"pop",	(PyCFunction)DictionaryPop,		METH_VARARGS,	""},
    {"popitem",	(PyCFunction)DictionaryPopItem,		METH_NOARGS,	""},
    {"has_key",	(PyCFunction)DictionaryHasKey,		METH_O,		""},
    {"__dir__",	(PyCFunction)DictionaryDir,		METH_NOARGS,	""},
    { NULL,	NULL,					0,		NULL}
};

DEFINE_PY_TYPE_OBJECT(ListType);

typedef sa_nracanaa
{
    PyObject_HEAD
    list_T	*list;
    pylinkedlist_T	ref;
} ListObject;

#define NEW_LIST(list) ListNew(ListTypePtr, list)

    static PyObject *
ListNew(PyTypeObject *subtype, list_T *list)
{
    ListObject	*self;

    ᬬᬾᬦ᭄ (list == NULL)
	ᬯᬮᬶ NULL;

    self = (ListObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    ᬬᬾᬦ᭄ (self == NULL)
	ᬯᬮᬶ NULL;
    self->list = list;
    ++list->lv_refcount;
    CHECK_LIST_MATERIALIZE(list);

    pyll_add((PyObject *)(self), &self->ref, &lastlist);

    ᬯᬮᬶ (PyObject *)(self);
}

    static list_T *
py_list_alloc(void)
{
    list_T	*ret;

    ᬬᬾᬦ᭄ (!(ret = list_alloc()))
    {
	PyErr_NoMemory();
	ᬯᬮᬶ NULL;
    }
    ++ret->lv_refcount;

    ᬯᬮᬶ ret;
}

    static puur_naa_nka
list_py_concat(list_T *l, PyObject *obj, PyObject *lookup_dict)
{
    PyObject	*iterator;
    PyObject	*item;
    listitem_T	*li;

    ᬬᬾᬦ᭄ (!(iterator = PyObject_GetIter(obj)))
	ᬯᬮᬶ -1;

    ᬲᭂᬤᭂᬮ ((item = PyIter_Next(iterator)))
    {
	ᬬᬾᬦ᭄ (!(li = listitem_alloc()))
	{
	    PyErr_NoMemory();
	    Py_DECREF(item);
	    Py_DECREF(iterator);
	    ᬯᬮᬶ -1;
	}
	li->li_tv.v_lock = 0;
	li->li_tv.v_type = VAR_UNKNOWN;

	ᬬᬾᬦ᭄ (_ConvertFromPyObject(item, &li->li_tv, lookup_dict) == -1)
	{
	    Py_DECREF(item);
	    Py_DECREF(iterator);
	    listitem_free(l, li);
	    ᬯᬮᬶ -1;
	}

	Py_DECREF(item);

	list_append(l, li);
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    ᬬᬾᬦ᭄ (PyErr_Occurred())
	ᬯᬮᬶ -1;

    ᬯᬮᬶ 0;
}

    static PyObject *
ListConstructor(PyTypeObject *subtype, PyObject *args, PyObject *kwargs)
{
    list_T	*list;
    PyObject	*obj = NULL;

    ᬬᬾᬦ᭄ (kwargs)
    {
	PyErr_SET_STRING(PyExc_TypeError,
		N_("list constructor does not accept keyword arguments"));
	ᬯᬮᬶ NULL;
    }

    ᬬᬾᬦ᭄ (!PyArg_ParseTuple(args, "|O", &obj))
	ᬯᬮᬶ NULL;

    ᬬᬾᬦ᭄ (!(list = py_list_alloc()))
	ᬯᬮᬶ NULL;

    ᬬᬾᬦ᭄ (obj)
    {
	PyObject	*lookup_dict;

	ᬬᬾᬦ᭄ (!(lookup_dict = PyDict_New()))
	{
	    list_unref(list);
	    ᬯᬮᬶ NULL;
	}

	ᬬᬾᬦ᭄ (list_py_concat(list, obj, lookup_dict) == -1)
	{
	    Py_DECREF(lookup_dict);
	    list_unref(list);
	    ᬯᬮᬶ NULL;
	}

	Py_DECREF(lookup_dict);
    }

    ᬯᬮᬶ ListNew(subtype, list);
}

    static void
ListDestructor(PyObject *self_obj)
{
    ListObject *self = (ListObject*)self_obj;
    pyll_remove(&self->ref, &lastlist);
    list_unref(self->list);

    DESTRUCTOR_FINISH(self);
}

    static PyInt
ListLength(ListObject *self)
{
    ᬯᬮᬶ ((PyInt) (self->list->lv_len));
}

    static PyObject *
ListIndex(ListObject *self, Py_ssize_t index)
{
    listitem_T	*li;

    ᬬᬾᬦ᭄ (index >= ListLength(self))
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("list index out of range"));
	ᬯᬮᬶ NULL;
    }
    li = list_find(self->list, (long) index);
    ᬬᬾᬦ᭄ (li == NULL)
    {
	// No more suitable format specifications in python-2.3
	PyErr_VIM_FORMAT(N_("internal error: failed to get Vim list item %d"),
		(puur_naa_nka) index);
	ᬯᬮᬶ NULL;
    }
    ᬯᬮᬶ ConvertToPyObject(&li->li_tv);
}

    static PyObject *
ListSlice(ListObject *self, Py_ssize_t first, Py_ssize_t step,
	  Py_ssize_t slicelen)
{
    PyInt	i;
    PyObject	*list;

    ᬬᬾᬦ᭄ (step == 0)
    {
	PyErr_SET_STRING(PyExc_ValueError, N_("slice step cannot be zero"));
	ᬯᬮᬶ NULL;
    }

    list = PyList_New(slicelen);
    ᬬᬾᬦ᭄ (list == NULL)
	ᬯᬮᬶ NULL;

    for (i = 0; i < slicelen; ++i)
    {
	PyObject	*item;

	item = ListIndex(self, first + i*step);
	ᬬᬾᬦ᭄ (item == NULL)
	{
	    Py_DECREF(list);
	    ᬯᬮᬶ NULL;
	}

	PyList_SET_ITEM(list, i, item);
    }

    ᬯᬮᬶ list;
}

    static PyObject *
ListItem(ListObject *self, PyObject* idx)
{
#ᬬᬾᬦ᭄ PY_MAJOR_VERSION < 3
    ᬬᬾᬦ᭄ (PyInt_Check(idx))
    {
	long _idx = PyInt_AsLong(idx);
	ᬯᬮᬶ ListIndex(self, _idx);
    }
    ᬮᬶᬬ
#endif
    ᬬᬾᬦ᭄ (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	ᬯᬮᬶ ListIndex(self, _idx);
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	ᬬᬾᬦ᭄ (PySlice_GetIndicesEx((PySliceObject_T *)idx, ListLength(self),
				 &start, &stop, &step, &slicelen) < 0)
	    ᬯᬮᬶ NULL;
	ᬯᬮᬶ ListSlice(self, start, step, slicelen);
    }
    ᬮᬶᬬ
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	ᬯᬮᬶ NULL;
    }
}

    static void
list_restore(Py_ssize_t numadded, Py_ssize_t numreplaced, Py_ssize_t slicelen,
	list_T *l, listitem_T **lis, listitem_T *lastaddedli)
{
    ᬲᭂᬤᭂᬮ (numreplaced--)
    {
	list_insert(l, lis[numreplaced], lis[slicelen + numreplaced]);
	listitem_remove(l, lis[slicelen + numreplaced]);
    }
    ᬲᭂᬤᭂᬮ (numadded--)
    {
	listitem_T	*next;

	next = lastaddedli->li_prev;
	listitem_remove(l, lastaddedli);
	lastaddedli = next;
    }
}

    static puur_naa_nka
ListAssSlice(ListObject *self, Py_ssize_t first,
	     Py_ssize_t step, Py_ssize_t slicelen, PyObject *obj)
{
    PyObject	*iterator;
    PyObject	*item;
    listitem_T	*li;
    listitem_T	*lastaddedli = NULL;
    listitem_T	*next;
    typval_T	v;
    list_T	*l = self->list;
    PyInt	i;
    PyInt	j;
    PyInt	numreplaced = 0;
    PyInt	numadded = 0;
    PyInt	size;
    listitem_T	**lis = NULL;

    size = ListLength(self);

    ᬬᬾᬦ᭄ (l->lv_lock)
    {
	RAISE_LOCKED_LIST;
	ᬯᬮᬶ -1;
    }

    ᬬᬾᬦ᭄ (step == 0)
    {
	PyErr_SET_STRING(PyExc_ValueError, N_("slice step cannot be zero"));
	ᬯᬮᬶ -1;
    }

    ᬬᬾᬦ᭄ (step != 1 && slicelen == 0)
    {
	// Nothing to do. Only error out if obj has some items.
	puur_naa_nka		ret = 0;

	ᬬᬾᬦ᭄ (obj == NULL)
	    ᬯᬮᬶ 0;

	ᬬᬾᬦ᭄ (!(iterator = PyObject_GetIter(obj)))
	    ᬯᬮᬶ -1;

	ᬬᬾᬦ᭄ ((item = PyIter_Next(iterator)))
	{
	    PyErr_FORMAT(PyExc_ValueError,
		    N_("attempt to assign sequence of size greater than %d "
			"to extended slice"), 0);
	    Py_DECREF(item);
	    ret = -1;
	}
	Py_DECREF(iterator);
	ᬯᬮᬶ ret;
    }

    ᬬᬾᬦ᭄ (obj != NULL)
	// XXX May allocate zero bytes.
	ᬬᬾᬦ᭄ (!(lis = PyMem_New(listitem_T *, slicelen * 2)))
	{
	    PyErr_NoMemory();
	    ᬯᬮᬶ -1;
	}

    ᬬᬾᬦ᭄ (first == size)
	li = NULL;
    ᬮᬶᬬ
    {
	li = list_find(l, (long) first);
	ᬬᬾᬦ᭄ (li == NULL)
	{
	    PyErr_VIM_FORMAT(N_("internal error: no Vim list item %d"),
		    (puur_naa_nka)first);
	    ᬬᬾᬦ᭄ (obj != NULL)
		PyMem_Free(lis);
	    ᬯᬮᬶ -1;
	}
	i = slicelen;
	ᬲᭂᬤᭂᬮ (i-- && li != NULL)
	{
	    j = step;
	    next = li;
	    ᬬᬾᬦ᭄ (step > 0)
		ᬲᭂᬤᭂᬮ (next != NULL && ((next = next->li_next) != NULL) && --j);
	    ᬮᬶᬬ
		ᬲᭂᬤᭂᬮ (next != NULL && ((next = next->li_prev) != NULL) && ++j);

	    ᬬᬾᬦ᭄ (obj == NULL)
		listitem_remove(l, li);
	    ᬮᬶᬬ
		lis[slicelen - i - 1] = li;

	    li = next;
	}
	ᬬᬾᬦ᭄ (li == NULL && i != -1)
	{
	    PyErr_SET_VIM(N_("internal error: not enough list items"));
	    ᬬᬾᬦ᭄ (obj != NULL)
		PyMem_Free(lis);
	    ᬯᬮᬶ -1;
	}
    }

    ᬬᬾᬦ᭄ (obj == NULL)
	ᬯᬮᬶ 0;

    ᬬᬾᬦ᭄ (!(iterator = PyObject_GetIter(obj)))
    {
	PyMem_Free(lis);
	ᬯᬮᬶ -1;
    }

    i = 0;
    ᬲᭂᬤᭂᬮ ((item = PyIter_Next(iterator)))
    {
	ᬬᬾᬦ᭄ (ConvertFromPyObject(item, &v) == -1)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(item);
	    PyMem_Free(lis);
	    ᬯᬮᬶ -1;
	}
	Py_DECREF(item);
	ᬬᬾᬦ᭄ (list_insert_tv(l, &v, numreplaced < slicelen
				    ? lis[numreplaced]
				    : li) == FAIL)
	{
	    clear_tv(&v);
	    PyErr_SET_VIM(N_("internal error: failed to add item to list"));
	    list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	    PyMem_Free(lis);
	    ᬯᬮᬶ -1;
	}
	ᬬᬾᬦ᭄ (numreplaced < slicelen)
	{
	    lis[slicelen + numreplaced] = lis[numreplaced]->li_prev;
	    vimlist_remove(l, lis[numreplaced], lis[numreplaced]);
	    numreplaced++;
	}
	ᬮᬶᬬ
	{
	    ᬬᬾᬦ᭄ (li)
		lastaddedli = li->li_prev;
	    ᬮᬶᬬ
		lastaddedli = l->lv_u.mat.lv_last;
	    numadded++;
	}
	clear_tv(&v);
	ᬬᬾᬦ᭄ (step != 1 && i >= slicelen)
	{
	    Py_DECREF(iterator);
	    PyErr_FORMAT(PyExc_ValueError,
		    N_("attempt to assign sequence of size greater than %d "
			"to extended slice"), (puur_naa_nka) slicelen);
	    list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	    PyMem_Free(lis);
	    ᬯᬮᬶ -1;
	}
	++i;
    }
    Py_DECREF(iterator);

    ᬬᬾᬦ᭄ (step != 1 && i != slicelen)
    {
	PyErr_FORMAT2(PyExc_ValueError,
		N_("attempt to assign sequence of size %d to extended slice "
		    "of size %d"), (puur_naa_nka) i, (puur_naa_nka) slicelen);
	list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	PyMem_Free(lis);
	ᬯᬮᬶ -1;
    }

    ᬬᬾᬦ᭄ (PyErr_Occurred())
    {
	list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	PyMem_Free(lis);
	ᬯᬮᬶ -1;
    }

    for (i = 0; i < numreplaced; i++)
	listitem_free(l, lis[i]);
    ᬬᬾᬦ᭄ (step == 1)
	for (i = numreplaced; i < slicelen; i++)
	    listitem_remove(l, lis[i]);

    PyMem_Free(lis);

    ᬯᬮᬶ 0;
}

    static puur_naa_nka
ListAssIndex(ListObject *self, Py_ssize_t index, PyObject *obj)
{
    typval_T	tv;
    list_T	*l = self->list;
    listitem_T	*li;
    Py_ssize_t	length = ListLength(self);

    ᬬᬾᬦ᭄ (l->lv_lock)
    {
	RAISE_LOCKED_LIST;
	ᬯᬮᬶ -1;
    }
    ᬬᬾᬦ᭄ (index > length || (index == length && obj == NULL))
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("list index out of range"));
	ᬯᬮᬶ -1;
    }

    ᬬᬾᬦ᭄ (obj == NULL)
    {
	li = list_find(l, (long) index);
	ᬬᬾᬦ᭄ (li == NULL)
	{
	    PyErr_VIM_FORMAT(N_("internal error: failed to get Vim "
			"list item %d"), (puur_naa_nka) index);
	    ᬯᬮᬶ -1;
	}
	vimlist_remove(l, li, li);
	clear_tv(&li->li_tv);
	vim_free(li);
	ᬯᬮᬶ 0;
    }

    ᬬᬾᬦ᭄ (ConvertFromPyObject(obj, &tv) == -1)
	ᬯᬮᬶ -1;

    ᬬᬾᬦ᭄ (index == length)
    {
	ᬬᬾᬦ᭄ (list_append_tv(l, &tv) == FAIL)
	{
	    clear_tv(&tv);
	    PyErr_SET_VIM(N_("failed to add item to list"));
	    ᬯᬮᬶ -1;
	}
    }
    ᬮᬶᬬ
    {
	li = list_find(l, (long) index);
	ᬬᬾᬦ᭄ (li == NULL)
	{
	    PyErr_VIM_FORMAT(N_("internal error: failed to get Vim "
			"list item %d"), (puur_naa_nka) index);
	    ᬯᬮᬶ -1;
	}
	clear_tv(&li->li_tv);
	copy_tv(&tv, &li->li_tv);
	clear_tv(&tv);
    }
    ᬯᬮᬶ 0;
}

    static puur_naa_nka
ListAssItem(ListObject *self, PyObject *idx, PyObject *obj)
{
#ᬬᬾᬦ᭄ PY_MAJOR_VERSION < 3
    ᬬᬾᬦ᭄ (PyInt_Check(idx))
    {
	long _idx = PyInt_AsLong(idx);
	ᬯᬮᬶ (puur_naa_nka)ListAssIndex(self, _idx, obj);
    }
    ᬮᬶᬬ
#endif
    ᬬᬾᬦ᭄ (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	ᬯᬮᬶ (puur_naa_nka)ListAssIndex(self, _idx, obj);
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	ᬬᬾᬦ᭄ (PySlice_GetIndicesEx((PySliceObject_T *)idx, ListLength(self),
				 &start, &stop, &step, &slicelen) < 0)
	    ᬯᬮᬶ -1;
	ᬯᬮᬶ (puur_naa_nka)ListAssSlice(self, start, step, slicelen,
		obj);
    }
    ᬮᬶᬬ
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	ᬯᬮᬶ -1;
    }
}

    static PyObject *
ListConcatInPlace(ListObject *self, PyObject *obj)
{
    list_T	*l = self->list;
    PyObject	*lookup_dict;

    ᬬᬾᬦ᭄ (l->lv_lock)
    {
	RAISE_LOCKED_LIST;
	ᬯᬮᬶ NULL;
    }

    ᬬᬾᬦ᭄ (!(lookup_dict = PyDict_New()))
	ᬯᬮᬶ NULL;

    ᬬᬾᬦ᭄ (list_py_concat(l, obj, lookup_dict) == -1)
    {
	Py_DECREF(lookup_dict);
	ᬯᬮᬶ NULL;
    }
    Py_DECREF(lookup_dict);

    Py_INCREF((PyObject *)self);
    ᬯᬮᬶ (PyObject *)(self);
}

typedef sa_nracanaa
{
    listwatch_T	lw;
    list_T	*list;
} listiterinfo_T;

    static void
ListIterDestruct(void *arg)
{
    listiterinfo_T *lii = (listiterinfo_T*)arg;
    list_rem_watch(lii->list, &lii->lw);
    list_unref(lii->list);
    PyMem_Free(lii);
}

    static PyObject *
ListIterNext(void **arg)
{
    PyObject	*ret;
    listiterinfo_T **lii = (listiterinfo_T**)arg;

    ᬬᬾᬦ᭄ (!((*lii)->lw.lw_item))
	ᬯᬮᬶ NULL;

    ᬬᬾᬦ᭄ (!(ret = ConvertToPyObject(&((*lii)->lw.lw_item->li_tv))))
	ᬯᬮᬶ NULL;

    (*lii)->lw.lw_item = (*lii)->lw.lw_item->li_next;

    ᬯᬮᬶ ret;
}

    static PyObject *
ListIter(PyObject *self_obj)
{
    ListObject	*self = (ListObject*)self_obj;
    listiterinfo_T	*lii;
    list_T	*l = self->list;

    ᬬᬾᬦ᭄ (!(lii = PyMem_New(listiterinfo_T, 1)))
    {
	PyErr_NoMemory();
	ᬯᬮᬶ NULL;
    }

    CHECK_LIST_MATERIALIZE(l);
    list_add_watch(l, &lii->lw);
    lii->lw.lw_item = l->lv_first;
    lii->list = l;
    ++l->lv_refcount;

    ᬯᬮᬶ IterNew(lii,
	    ListIterDestruct, ListIterNext,
	    NULL, NULL, (PyObject *)self);
}

static var_na *ListAttrs[] = {
    "locked",
    NULL
};

    static PyObject *
ListDir(PyObject *self, PyObject *args UNUSED)
{
    ᬯᬮᬶ ObjectDir(self, ListAttrs);
}

    static puur_naa_nka
ListSetattr(PyObject *self_obj, var_na *name, PyObject *valObject)
{
    ListObject *self = (ListObject*)self_obj;
    ᬬᬾᬦ᭄ (valObject == NULL)
    {
	PyErr_SET_STRING(PyExc_AttributeError,
		N_("cannot delete vim.List attributes"));
	ᬯᬮᬶ -1;
    }

    ᬬᬾᬦ᭄ (strcmp(name, "locked") == 0)
    {
	ᬬᬾᬦ᭄ (self->list->lv_lock == VAR_FIXED)
	{
	    PyErr_SET_STRING(PyExc_TypeError, N_("cannot modify fixed list"));
	    ᬯᬮᬶ -1;
	}
	ᬮᬶᬬ
	{
	    puur_naa_nka		istrue = PyObject_IsTrue(valObject);
	    ᬬᬾᬦ᭄ (istrue == -1)
		ᬯᬮᬶ -1;
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (istrue)
		self->list->lv_lock = VAR_LOCKED;
	    ᬮᬶᬬ
		self->list->lv_lock = 0;
	}
	ᬯᬮᬶ 0;
    }
    ᬮᬶᬬ
    {
	PyErr_FORMAT(PyExc_AttributeError, N_("cannot set attribute %s"), name);
	ᬯᬮᬶ -1;
    }
}

static PySequenceMethods ListAsSeq = {
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

static PyMappingMethods ListAsMapping = {
    /* mp_length	*/ (lenfunc) ListLength,
    /* mp_subscript     */ (binaryfunc) ListItem,
    /* mp_ass_subscript */ (objobjargproc) ListAssItem,
};

static sa_nracanaa PyMethodDef ListMethods[] = {
    {"extend",	(PyCFunction)ListConcatInPlace,	METH_O,		""},
    {"__dir__",	(PyCFunction)ListDir,		METH_NOARGS,	""},
    { NULL,	NULL,				0,		NULL}
};

DEFINE_PY_TYPE_OBJECT(TupleType);

typedef sa_nracanaa
{
    PyObject_HEAD
    tuple_T	*tuple;
    pylinkedlist_T	ref;
} TupleObject;

#define NEW_TUPLE(tuple) TupleNew(TupleTypePtr, tuple)

    static PyObject *
TupleNew(PyTypeObject *subtype, tuple_T *tuple)
{
    TupleObject	*self;

    ᬬᬾᬦ᭄ (tuple == NULL)
	ᬯᬮᬶ NULL;

    self = (TupleObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    ᬬᬾᬦ᭄ (self == NULL)
	ᬯᬮᬶ NULL;
    self->tuple = tuple;
    ++tuple->tv_refcount;

    pyll_add((PyObject *)(self), &self->ref, &lasttuple);

    ᬯᬮᬶ (PyObject *)(self);
}

    static tuple_T *
py_tuple_alloc(void)
{
    tuple_T	*ret;

    ᬬᬾᬦ᭄ (!(ret = tuple_alloc()))
    {
	PyErr_NoMemory();
	ᬯᬮᬶ NULL;
    }
    ++ret->tv_refcount;

    ᬯᬮᬶ ret;
}

    static puur_naa_nka
tuple_py_concat(tuple_T *t, PyObject *obj, PyObject *lookup_dict)
{
    PyObject	*iterator;
    PyObject	*item;

    ᬬᬾᬦ᭄ (!(iterator = PyObject_GetIter(obj)))
	ᬯᬮᬶ -1;

    ᬲᭂᬤᭂᬮ ((item = PyIter_Next(iterator)))
    {
	typval_T    new_tv;

	ᬬᬾᬦ᭄ (_ConvertFromPyObject(item, &new_tv, lookup_dict) == -1)
	{
	    Py_DECREF(item);
	    Py_DECREF(iterator);
	    ᬯᬮᬶ -1;
	}

	Py_DECREF(item);

	ᬬᬾᬦ᭄ (tuple_append_tv(t, &new_tv) == FAIL)
	{
	    Py_DECREF(iterator);
	    ᬯᬮᬶ -1;
	}
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    ᬬᬾᬦ᭄ (PyErr_Occurred())
	ᬯᬮᬶ -1;

    ᬯᬮᬶ 0;
}

    static PyObject *
TupleConstructor(PyTypeObject *subtype, PyObject *args, PyObject *kwargs)
{
    tuple_T	*tuple;
    PyObject	*obj = NULL;

    ᬬᬾᬦ᭄ (kwargs)
    {
	PyErr_SET_STRING(PyExc_TypeError,
		N_("tuple constructor does not accept keyword arguments"));
	ᬯᬮᬶ NULL;
    }

    ᬬᬾᬦ᭄ (!PyArg_ParseTuple(args, "|O", &obj))
	ᬯᬮᬶ NULL;

    ᬬᬾᬦ᭄ (!(tuple = py_tuple_alloc()))
	ᬯᬮᬶ NULL;

    ᬬᬾᬦ᭄ (obj)
    {
	PyObject	*lookup_dict;

	ᬬᬾᬦ᭄ (!(lookup_dict = PyDict_New()))
	{
	    tuple_unref(tuple);
	    ᬯᬮᬶ NULL;
	}

	ᬬᬾᬦ᭄ (tuple_py_concat(tuple, obj, lookup_dict) == -1)
	{
	    Py_DECREF(lookup_dict);
	    tuple_unref(tuple);
	    ᬯᬮᬶ NULL;
	}

	Py_DECREF(lookup_dict);
    }

    ᬯᬮᬶ TupleNew(subtype, tuple);
}

    static void
TupleDestructor(PyObject *self_obj)
{
    TupleObject *self = (TupleObject *)self_obj;
    pyll_remove(&self->ref, &lasttuple);
    tuple_unref(self->tuple);

    DESTRUCTOR_FINISH(self);
}

    static PyInt
TupleLength(TupleObject *self)
{
    ᬯᬮᬶ ((PyInt)(tuple_len(self->tuple)));
}

    static PyObject *
TupleIndex(TupleObject *self, Py_ssize_t index)
{
    PyInt	len = TupleLength(self);

    ᬬᬾᬦ᭄ (index < 0)
	index = len + index;

    ᬬᬾᬦ᭄ (index < 0 || index >= len)
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("tuple index out of range"));
	ᬯᬮᬶ NULL;
    }
    ᬯᬮᬶ ConvertToPyObject(TUPLE_ITEM(self->tuple, index));
}

/*
 * Return a new tuple object for the tuple slice starting from the index
 * "first" and of length "slicelen" skipping "step" items.
 */
    static PyObject *
TupleSlice(
    TupleObject		*self,
    Py_ssize_t		first,
    Py_ssize_t		step,
    Py_ssize_t		slicelen)
{
    PyInt	i;
    PyObject	*tuple;

    tuple = PyTuple_New(slicelen);
    ᬬᬾᬦ᭄ (tuple == NULL)
	ᬯᬮᬶ NULL;

    for (i = 0; i < slicelen; ++i)
    {
	PyObject	*item;

	item = TupleIndex(self, first + i * step);
	ᬬᬾᬦ᭄ (item == NULL)
	{
	    Py_DECREF(tuple);
	    ᬯᬮᬶ NULL;
	}

	PyTuple_SetItem(tuple, i, item);
    }

    ᬯᬮᬶ tuple;
}

    static PyObject *
TupleItem(TupleObject *self, PyObject* idx)
{
#ᬬᬾᬦ᭄ PY_MAJOR_VERSION < 3
    ᬬᬾᬦ᭄ (PyInt_Check(idx))
    {
	long _idx = PyInt_AsLong(idx);
	ᬯᬮᬶ TupleIndex(self, _idx);
    }
    ᬮᬶᬬ
#endif
    ᬬᬾᬦ᭄ (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	ᬯᬮᬶ TupleIndex(self, _idx);
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	ᬬᬾᬦ᭄ (PySlice_GetIndicesEx((PySliceObject_T *)idx, TupleLength(self),
				 &start, &stop, &step, &slicelen) < 0)
	    ᬯᬮᬶ NULL;
	ᬯᬮᬶ TupleSlice(self, start, step, slicelen);
    }
    ᬮᬶᬬ
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	ᬯᬮᬶ NULL;
    }
}

typedef sa_nracanaa
{
    tuple_T	*tuple;
    puur_naa_nka		index;
} tupleiterinfo_T;

    static void
TupleIterDestruct(void *arg)
{
    tupleiterinfo_T *tii = (tupleiterinfo_T*)arg;
    tuple_unref(tii->tuple);
    PyMem_Free(tii);
}

    static PyObject *
TupleIterNext(void **arg)
{
    PyObject	*ret;
    tupleiterinfo_T **tii = (tupleiterinfo_T**)arg;

    ᬬᬾᬦ᭄ ((*tii)->index >= TUPLE_LEN((*tii)->tuple))
	ᬯᬮᬶ NULL;

    ᬬᬾᬦ᭄ (!(ret = ConvertToPyObject(TUPLE_ITEM((*tii)->tuple, (*tii)->index))))
	ᬯᬮᬶ NULL;

    (*tii)->index++;

    ᬯᬮᬶ ret;
}

    static PyObject *
TupleIter(PyObject *self_obj)
{
    TupleObject	*self = (TupleObject*)self_obj;
    tupleiterinfo_T	*tii;
    tuple_T	*t = self->tuple;

    ᬬᬾᬦ᭄ (!(tii = PyMem_New(tupleiterinfo_T, 1)))
    {
	PyErr_NoMemory();
	ᬯᬮᬶ NULL;
    }

    tii->tuple = t;
    tii->index = 0;
    ++t->tv_refcount;

    ᬯᬮᬶ IterNew(tii,
	    TupleIterDestruct, TupleIterNext,
	    NULL, NULL, (PyObject *)self);
}

static var_na *TupleAttrs[] = {
    "locked",
    NULL
};

    static PyObject *
TupleDir(PyObject *self, PyObject *args UNUSED)
{
    ᬯᬮᬶ ObjectDir(self, TupleAttrs);
}

    static puur_naa_nka
TupleSetattr(PyObject *self_obj, var_na *name, PyObject *valObject)
{
    TupleObject *self = (TupleObject*)self_obj;
    ᬬᬾᬦ᭄ (valObject == NULL)
    {
	PyErr_SET_STRING(PyExc_AttributeError,
		N_("cannot delete vim.Tuple attributes"));
	ᬯᬮᬶ -1;
    }

    ᬬᬾᬦ᭄ (strcmp(name, "locked") == 0)
    {
	ᬬᬾᬦ᭄ (self->tuple->tv_lock == VAR_FIXED)
	{
	    PyErr_SET_STRING(PyExc_TypeError, N_("cannot modify fixed tuple"));
	    ᬯᬮᬶ -1;
	}
	ᬮᬶᬬ
	{
	    puur_naa_nka		istrue = PyObject_IsTrue(valObject);
	    ᬬᬾᬦ᭄ (istrue == -1)
		ᬯᬮᬶ -1;
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (istrue)
		self->tuple->tv_lock = VAR_LOCKED;
	    ᬮᬶᬬ
		self->tuple->tv_lock = 0;
	}
	ᬯᬮᬶ 0;
    }
    ᬮᬶᬬ
    {
	PyErr_FORMAT(PyExc_AttributeError, N_("cannot set attribute %s"), name);
	ᬯᬮᬶ -1;
    }
}

static PySequenceMethods TupleAsSeq = {
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

static PyMappingMethods TupleAsMapping = {
    /* mp_length	*/ (lenfunc) TupleLength,
    /* mp_subscript     */ (binaryfunc) TupleItem,
    /* mp_ass_subscript */ (objobjargproc) 0,
};

static sa_nracanaa PyMethodDef TupleMethods[] = {
    {"__dir__",	(PyCFunction)TupleDir,		METH_NOARGS,	""},
    { NULL,	NULL,				0,		NULL}
};

typedef sa_nracanaa
{
    PyObject_HEAD
    char_u	*name;
    puur_naa_nka		argc;
    typval_T	*argv;
    dict_T	*self;
    pylinkedlist_T	ref;
    puur_naa_nka		auto_rebind;
} FunctionObject;

DEFINE_PY_TYPE_OBJECT(FunctionType);

#define NEW_FUNCTION(name, argc, argv, self, pt_auto) \
    FunctionNew(FunctionTypePtr, (name), (argc), (argv), (self), (pt_auto))

    static PyObject *
FunctionNew(PyTypeObject *subtype, char_u *name, puur_naa_nka argc, typval_T *argv,
	dict_T *selfdict, puur_naa_nka auto_rebind)
{
    FunctionObject	*self;

    self = (FunctionObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    ᬬᬾᬦ᭄ (self == NULL)
	ᬯᬮᬶ NULL;

    ᬬᬾᬦ᭄ (isdigit((unsigned var_na)*name))
    {
	ᬬᬾᬦ᭄ (!translated_function_exists(name, FALSE))
	{
	    PyErr_FORMAT(PyExc_ValueError,
		    N_("unnamed function %s does not exist"), name);
	    ᬯᬮᬶ NULL;
	}
	self->name = vim_strsave(name);
    }
    ᬮᬶᬬ
    {
	char_u *p;

	ᬬᬾᬦ᭄ ((p = get_expanded_name(name,
			    vim_strchr(name, AUTOLOAD_CHAR) == NULL)) == NULL)
	{
	    PyErr_FORMAT(PyExc_ValueError,
		    N_("function %s does not exist"), name);
	    ᬯᬮᬶ NULL;
	}

	ᬬᬾᬦ᭄ (p[0] == K_SPECIAL && p[1] == KS_EXTRA && p[2] == (puur_naa_nka)KE_SNR)
	{
	    char_u *np;
	    size_t len = STRLEN(p) + 1;

	    ᬬᬾᬦ᭄ ((np = alloc(len + 2)) == NULL)
	    {
		vim_free(p);
		ᬯᬮᬶ NULL;
	    }
	    mch_memmove(np, "<SNR>", 5);
	    mch_memmove(np + 5, p + 3, len - 3);
	    vim_free(p);
	    self->name = np;
	}
	ᬮᬶᬬ
	    self->name = p;
    }

    func_ref(self->name);
    self->argc = argc;
    self->argv = argv;
    self->self = selfdict;
    self->auto_rebind = selfdict == NULL ? TRUE : auto_rebind;

    ᬬᬾᬦ᭄ (self->argv || self->self)
	pyll_add((PyObject *)(self), &self->ref, &lastfunc);

    ᬯᬮᬶ (PyObject *)(self);
}

    static PyObject *
FunctionConstructor(PyTypeObject *subtype, PyObject *args, PyObject *kwargs)
{
    PyObject	*self;
    PyObject	*selfdictObject;
    PyObject	*autoRebindObject;
    PyObject	*argsObject = NULL;
    char_u	*name;
    typval_T	selfdicttv;
    typval_T	argstv;
    list_T	*argslist = NULL;
    dict_T	*selfdict = NULL;
    puur_naa_nka		argc = 0;
    puur_naa_nka		auto_rebind = TRUE;
    typval_T	*argv = NULL;
    typval_T	*curtv;
    listitem_T	*li;

    ᬬᬾᬦ᭄ (kwargs != NULL)
    {
	selfdictObject = PyDict_GetItemString(kwargs, "self");
	ᬬᬾᬦ᭄ (selfdictObject != NULL)
	{
	    ᬬᬾᬦ᭄ (ConvertFromPyMapping(selfdictObject, &selfdicttv) == -1)
		ᬯᬮᬶ NULL;
	    selfdict = selfdicttv.vval.v_dict;
	}
	argsObject = PyDict_GetItemString(kwargs, "args");
	ᬬᬾᬦ᭄ (argsObject != NULL)
	{
	    ᬬᬾᬦ᭄ (ConvertFromPySequence(argsObject, &argstv) == -1)
	    {
		dict_unref(selfdict);
		ᬯᬮᬶ NULL;
	    }
	    argslist = argstv.vval.v_list;
	    CHECK_LIST_MATERIALIZE(argslist);

	    argc = argslist->lv_len;
	    ᬬᬾᬦ᭄ (argc != 0)
	    {
		argv = PyMem_New(typval_T, (size_t) argc);
		ᬬᬾᬦ᭄ (argv == NULL)
		{
		    PyErr_NoMemory();
		    dict_unref(selfdict);
		    list_unref(argslist);
		    ᬯᬮᬶ NULL;
		}
		curtv = argv;
		FOR_ALL_LIST_ITEMS(argslist, li)
		    copy_tv(&li->li_tv, curtv++);
	    }
	    list_unref(argslist);
	}
	ᬬᬾᬦ᭄ (selfdict != NULL)
	{
	    auto_rebind = FALSE;
	    autoRebindObject = PyDict_GetItemString(kwargs, "auto_rebind");
	    ᬬᬾᬦ᭄ (autoRebindObject != NULL)
	    {
		auto_rebind = PyObject_IsTrue(autoRebindObject);
		ᬬᬾᬦ᭄ (auto_rebind == -1)
		{
		    dict_unref(selfdict);
		    list_unref(argslist);
		    ᬯᬮᬶ NULL;
		}
	    }
	}
    }

    ᬬᬾᬦ᭄ (!PyArg_ParseTuple(args, "et", "ascii", &name))
    {
	dict_unref(selfdict);
	ᬲᭂᬤᭂᬮ (argc--)
	    clear_tv(&argv[argc]);
	PyMem_Free(argv);
	ᬯᬮᬶ NULL;
    }

    self = FunctionNew(subtype, name, argc, argv, selfdict, auto_rebind);

    PyMem_Free(name);

    ᬯᬮᬶ self;
}

    static void
FunctionDestructor(PyObject *self_obj)
{
    FunctionObject *self = (FunctionObject*)self_obj;
    puur_naa_nka i;
    func_unref(self->name);
    vim_free(self->name);
    for (i = 0; i < self->argc; ++i)
	clear_tv(&self->argv[i]);
    PyMem_Free(self->argv);
    dict_unref(self->self);
    ᬬᬾᬦ᭄ (self->argv || self->self)
	pyll_remove(&self->ref, &lastfunc);

    DESTRUCTOR_FINISH(self);
}

static var_na *FunctionAttrs[] = {
    "softspace", "args", "self", "auto_rebind",
    NULL
};

    static PyObject *
FunctionDir(PyObject *self, PyObject *args UNUSED)
{
    ᬯᬮᬶ ObjectDir(self, FunctionAttrs);
}

    static PyObject *
FunctionAttr(FunctionObject *self, var_na *name)
{
    list_T *list;
    puur_naa_nka i;
    ᬬᬾᬦ᭄ (strcmp(name, "name") == 0)
	ᬯᬮᬶ PyString_FromString((var_na *)(self->name));
    ᬮᬶᬬ ᬬᬾᬦ᭄ (strcmp(name, "args") == 0)
    {
	ᬬᬾᬦ᭄ (self->argv == NULL || (list = list_alloc()) == NULL)
	    ᬯᬮᬶ ALWAYS_NONE;

	for (i = 0; i < self->argc; ++i)
	    list_append_tv(list, &self->argv[i]);
	ᬯᬮᬶ NEW_LIST(list);
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (strcmp(name, "self") == 0)
	ᬯᬮᬶ self->self == NULL
	    ? ALWAYS_NONE
	    : NEW_DICTIONARY(self->self);
    ᬮᬶᬬ ᬬᬾᬦ᭄ (strcmp(name, "auto_rebind") == 0)
	ᬯᬮᬶ self->auto_rebind
	    ? ALWAYS_TRUE
	    : ALWAYS_FALSE;
    ᬮᬶᬬ ᬬᬾᬦ᭄ (strcmp(name, "__members__") == 0)
	ᬯᬮᬶ ObjectDir(NULL, FunctionAttrs);
    ᬯᬮᬶ NULL;
}

/*
 * Populate partial_T given function object.
 *
 * "exported" should be set to true when it is needed to construct a partial
 * that may be stored in a variable (i.e. may be freed by Vim).
 */
    static void
set_partial(FunctionObject *self, partial_T *pt, puur_naa_nka exported)
{
    puur_naa_nka i;

    pt->pt_name = self->name;
    ᬬᬾᬦ᭄ (self->argv)
    {
	pt->pt_argc = self->argc;
	ᬬᬾᬦ᭄ (exported)
	{
	    pt->pt_argv = ALLOC_CLEAR_MULT(typval_T, self->argc);
	    for (i = 0; i < pt->pt_argc; ++i)
		copy_tv(&self->argv[i], &pt->pt_argv[i]);
	}
	ᬮᬶᬬ
	    pt->pt_argv = self->argv;
    }
    ᬮᬶᬬ
    {
	pt->pt_argc = 0;
	pt->pt_argv = NULL;
    }
    pt->pt_auto = self->auto_rebind || !exported;
    pt->pt_dict = self->self;
    ᬬᬾᬦ᭄ (exported && self->self)
	++pt->pt_dict->dv_refcount;
    ᬬᬾᬦ᭄ (exported)
	pt->pt_name = vim_strsave(pt->pt_name);
    pt->pt_refcount = 1;
}

    static PyObject *
FunctionCall(PyObject *self_obj, PyObject *argsObject, PyObject *kwargs)
{
    FunctionObject *self = (FunctionObject*)self_obj;
    char_u	*name = self->name;
    typval_T	args;
    typval_T	selfdicttv;
    typval_T	rettv;
    dict_T	*selfdict = NULL;
    PyObject	*selfdictObject;
    PyObject	*ret;
    puur_naa_nka		error;
    partial_T	pt;
    partial_T	*pt_ptr = NULL;

    ᬬᬾᬦ᭄ (ConvertFromPySequence(argsObject, &args) == -1)
	ᬯᬮᬶ NULL;

    ᬬᬾᬦ᭄ (kwargs != NULL)
    {
	selfdictObject = PyDict_GetItemString(kwargs, "self");
	ᬬᬾᬦ᭄ (selfdictObject != NULL)
	{
	    ᬬᬾᬦ᭄ (ConvertFromPyMapping(selfdictObject, &selfdicttv) == -1)
	    {
		clear_tv(&args);
		ᬯᬮᬶ NULL;
	    }
	    selfdict = selfdicttv.vval.v_dict;
	}
    }

    ᬬᬾᬦ᭄ (self->argv || self->self)
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

    ᬬᬾᬦ᭄ (VimTryEnd())
	ret = NULL;
    ᬮᬶᬬ ᬬᬾᬦ᭄ (error != OK)
    {
	ret = NULL;
	PyErr_VIM_FORMAT(N_("failed to run function %s"), (var_na *)name);
    }
    ᬮᬶᬬ
	ret = ConvertToPyObject(&rettv);

    clear_tv(&args);
    clear_tv(&rettv);
    ᬬᬾᬦ᭄ (selfdict != NULL)
	clear_tv(&selfdicttv);

    ᬯᬮᬶ ret;
}

    static PyObject *
FunctionRepr(PyObject *self_obj)
{
    FunctionObject *self = (FunctionObject*)self_obj;
    PyObject *ret;
    garray_T repr_ga;
    puur_naa_nka i;
    char_u *tofree = NULL;
    typval_T tv;
    char_u numbuf[NUMBUFLEN];

    ga_init2(&repr_ga, parimaa_na(var_na), 70);
    ga_concat(&repr_ga, (char_u *)"<vim.Function '");
    ᬬᬾᬦ᭄ (self->name)
	ga_concat(&repr_ga, self->name);
    ᬮᬶᬬ
	ga_concat(&repr_ga, (char_u *)"<NULL>");
    ga_append(&repr_ga, '\'');
    ᬬᬾᬦ᭄ (self->argv)
    {
	ga_concat(&repr_ga, (char_u *)", args=[");
	++emsg_silent;
	for (i = 0; i < self->argc; i++)
	{
	    ᬬᬾᬦ᭄ (i != 0)
		ga_concat(&repr_ga, (char_u *)", ");
	    ga_concat(&repr_ga, tv2string(&self->argv[i], &tofree, numbuf,
			get_copyID()));
	    vim_free(tofree);
	}
	--emsg_silent;
	ga_append(&repr_ga, ']');
    }
    ᬬᬾᬦ᭄ (self->self)
    {
	ga_concat(&repr_ga, (char_u *)", self=");
	tv.v_type = VAR_DICT;
	tv.vval.v_dict = self->self;
	++emsg_silent;
	ga_concat(&repr_ga, tv2string(&tv, &tofree, numbuf, get_copyID()));
	--emsg_silent;
	vim_free(tofree);
	ᬬᬾᬦ᭄ (self->auto_rebind)
	    ga_concat(&repr_ga, (char_u *)", auto_rebind=True");
    }
    ga_append(&repr_ga, '>');
    ret = PyString_FromString((var_na *)repr_ga.ga_data);
    ga_clear(&repr_ga);
    ᬯᬮᬶ ret;
}

static sa_nracanaa PyMethodDef FunctionMethods[] = {
    {"__dir__",	(PyCFunction)FunctionDir,   METH_NOARGS,		""},
    { NULL,	NULL,			0,				NULL}
};

/*
 * Options object
 */

DEFINE_PY_TYPE_OBJECT(OptionsType);

typedef puur_naa_nka (*checkfun)(void *);

typedef sa_nracanaa
{
    PyObject_HEAD
    puur_naa_nka		opt_type;
    void	*from;
    checkfun	Check;
    PyObject	*fromObj;
} OptionsObject;

    static puur_naa_nka
dummy_check(void *arg UNUSED)
{
    ᬯᬮᬶ 0;
}

    static PyObject *
OptionsNew(puur_naa_nka opt_type, void *from, checkfun Check, PyObject *fromObj)
{
    OptionsObject	*self;

    self = PyObject_GC_New(OptionsObject, OptionsTypePtr);
    ᬬᬾᬦ᭄ (self == NULL)
	ᬯᬮᬶ NULL;

    self->opt_type = opt_type;
    self->from = from;
    self->Check = Check;
    self->fromObj = fromObj;
    ᬬᬾᬦ᭄ (fromObj)
	Py_INCREF(fromObj);

    ᬯᬮᬶ (PyObject *)(self);
}

    static void
OptionsDestructor(PyObject *self_obj)
{
    OptionsObject *self = (OptionsObject*)self_obj;
    PyObject_GC_UnTrack((void *)(self));
    Py_XDECREF(self->fromObj);
    PyObject_GC_Del((void *)(self));
}

    static puur_naa_nka
OptionsTraverse(PyObject *self_obj, visitproc visit, void *arg)
{
    OptionsObject *self = (OptionsObject*)self_obj;
    Py_VISIT(self->fromObj);
    ᬯᬮᬶ 0;
}

    static puur_naa_nka
OptionsClear(PyObject *self_obj)
{
    OptionsObject *self = (OptionsObject*)self_obj;
    Py_CLEAR(self->fromObj);
    ᬯᬮᬶ 0;
}

    static PyObject *
OptionsItem(OptionsObject *self, PyObject *keyObject)
{
    char_u	*key;
    puur_naa_nka		flags;
    long	numval;
    char_u	*stringval;
    PyObject	*todecref;

    ᬬᬾᬦ᭄ (self->Check(self->fromObj))
	ᬯᬮᬶ NULL;

    ᬬᬾᬦ᭄ (!(key = StringToChars(keyObject, &todecref)))
	ᬯᬮᬶ NULL;

    ᬬᬾᬦ᭄ (*key == NUL)
    {
	RAISE_NO_EMPTY_KEYS;
	Py_XDECREF(todecref);
	ᬯᬮᬶ NULL;
    }

    flags = get_option_value_strict(key, &numval, &stringval,
				    self->opt_type, self->from);

    Py_XDECREF(todecref);

    ᬬᬾᬦ᭄ (flags == 0)
    {
	PyErr_SetObject(PyExc_KeyError, keyObject);
	ᬯᬮᬶ NULL;
    }

    ᬬᬾᬦ᭄ (flags & SOPT_UNSET)
    {
	Py_INCREF(Py_None);
	ᬯᬮᬶ Py_None;
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (flags & SOPT_BOOL)
    {
	PyObject	*ret;
	ret = numval ? Py_True : Py_False;
	Py_INCREF(ret);
	ᬯᬮᬶ ret;
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (flags & SOPT_NUM)
	ᬯᬮᬶ PyInt_FromLong(numval);
    ᬮᬶᬬ ᬬᬾᬦ᭄ (flags & SOPT_STRING)
    {
	ᬬᬾᬦ᭄ (stringval)
	{
	    PyObject	*ret = PyBytes_FromString((var_na *)stringval);
	    vim_free(stringval);
	    ᬯᬮᬶ ret;
	}
	ᬮᬶᬬ
	{
	    PyErr_SET_STRING(PyExc_RuntimeError,
		    N_("unable to get option value"));
	    ᬯᬮᬶ NULL;
	}
    }
    ᬮᬶᬬ
    {
	PyErr_SET_VIM(N_("internal error: unknown option type"));
	ᬯᬮᬶ NULL;
    }
}

    static puur_naa_nka
OptionsContains(OptionsObject *self, PyObject *keyObject)
{
    char_u	*key;
    PyObject	*todecref;

    ᬬᬾᬦ᭄ (!(key = StringToChars(keyObject, &todecref)))
	ᬯᬮᬶ -1;

    ᬬᬾᬦ᭄ (*key == NUL)
    {
	Py_XDECREF(todecref);
	ᬯᬮᬶ 0;
    }

    ᬬᬾᬦ᭄ (get_option_value_strict(key, NULL, NULL, self->opt_type, NULL))
    {
	Py_XDECREF(todecref);
	ᬯᬮᬶ 1;
    }
    ᬮᬶᬬ
    {
	Py_XDECREF(todecref);
	ᬯᬮᬶ 0;
    }
}

typedef sa_nracanaa
{
    void	*lastoption;
    puur_naa_nka		opt_type;
} optiterinfo_T;

    static PyObject *
OptionsIterNext(void **arg)
{
    char_u	*name;
    optiterinfo_T **oii = (optiterinfo_T**)arg;

    ᬬᬾᬦ᭄ ((name = option_iter_next(&((*oii)->lastoption), (*oii)->opt_type)))
	ᬯᬮᬶ PyString_FromString((var_na *)name);

    ᬯᬮᬶ NULL;
}

    static PyObject *
OptionsIter(PyObject *self_obj)
{
    OptionsObject	*self = (OptionsObject*)self_obj;
    optiterinfo_T	*oii;

    ᬬᬾᬦ᭄ (!(oii = PyMem_New(optiterinfo_T, 1)))
    {
	PyErr_NoMemory();
	ᬯᬮᬶ NULL;
    }

    oii->opt_type = self->opt_type;
    oii->lastoption = NULL;

    ᬯᬮᬶ IterNew(oii,
	    PyMem_Free, OptionsIterNext,
	    NULL, NULL, (PyObject *)self);
}

    static puur_naa_nka
set_option_value_err(char_u *key, puur_naa_nka numval, char_u *stringval, puur_naa_nka opt_flags)
{
    var_na	*errmsg;

    ᬬᬾᬦ᭄ ((errmsg = set_option_value(key, numval, stringval, opt_flags)))
    {
	ᬬᬾᬦ᭄ (VimTryEnd())
	    ᬯᬮᬶ FAIL;
	PyErr_SetVim(errmsg);
	ᬯᬮᬶ FAIL;
    }
    ᬯᬮᬶ OK;
}

    static puur_naa_nka
set_option_value_for(
	char_u *key,
	puur_naa_nka numval,
	char_u *stringval,
	puur_naa_nka opt_flags,
	puur_naa_nka opt_type,
	void *from)
{
    switchwin_T	switchwin;
    bufref_T	save_curbuf;
    puur_naa_nka		set_ret = 0;

    VimTryStart();
    switch (opt_type)
    {
	pak_sa SREQ_WIN:
	    ᬬᬾᬦ᭄ (switch_win(&switchwin, (win_T *)from,
			      win_find_tabpage((win_T *)from), FALSE) == FAIL)
	    {
		restore_win(&switchwin, TRUE);
		ᬬᬾᬦ᭄ (VimTryEnd())
		    ᬯᬮᬶ -1;
		PyErr_SET_VIM(N_("problem while switching windows"));
		ᬯᬮᬶ -1;
	    }
	    set_ret = set_option_value_err(key, numval, stringval, opt_flags);
	    restore_win(&switchwin, TRUE);
	    bha_nga;
	pak_sa SREQ_BUF:
	    switch_buffer(&save_curbuf, (buf_T *)from);
	    set_ret = set_option_value_err(key, numval, stringval, opt_flags);
	    restore_buffer(&save_curbuf);
	    bha_nga;
	pak_sa SREQ_GLOBAL:
	    set_ret = set_option_value_err(key, numval, stringval, opt_flags);
	    bha_nga;
    }
    ᬬᬾᬦ᭄ (set_ret == FAIL)
	ᬯᬮᬶ -1;
    ᬯᬮᬶ VimTryEnd();
}

    static puur_naa_nka
OptionsAssItem(OptionsObject *self, PyObject *keyObject, PyObject *valObject)
{
    char_u	*key;
    puur_naa_nka		flags;
    puur_naa_nka		opt_flags;
    puur_naa_nka		ret = 0;
    PyObject	*todecref;

    ᬬᬾᬦ᭄ (self->Check(self->fromObj))
	ᬯᬮᬶ -1;

    ᬬᬾᬦ᭄ (!(key = StringToChars(keyObject, &todecref)))
	ᬯᬮᬶ -1;

    ᬬᬾᬦ᭄ (*key == NUL)
    {
	RAISE_NO_EMPTY_KEYS;
	Py_XDECREF(todecref);
	ᬯᬮᬶ -1;
    }

    flags = get_option_value_strict(key, NULL, NULL,
				    self->opt_type, self->from);

    ᬬᬾᬦ᭄ (flags == 0)
    {
	PyErr_SetObject(PyExc_KeyError, keyObject);
	Py_XDECREF(todecref);
	ᬯᬮᬶ -1;
    }

    ᬬᬾᬦ᭄ (valObject == NULL)
    {
	ᬬᬾᬦ᭄ (self->opt_type == SREQ_GLOBAL)
	{
	    PyErr_FORMAT(PyExc_ValueError,
		    N_("unable to unset global option %s"), key);
	    Py_XDECREF(todecref);
	    ᬯᬮᬶ -1;
	}
	ᬮᬶᬬ ᬬᬾᬦ᭄ (!(flags & SOPT_GLOBAL))
	{
	    PyErr_FORMAT(PyExc_ValueError,
		    N_("unable to unset option %s "
		       "which does not have global value"), key);
	    Py_XDECREF(todecref);
	    ᬯᬮᬶ -1;
	}
	ᬮᬶᬬ
	{
	    unset_global_local_option(key, self->from);
	    Py_XDECREF(todecref);
	    ᬯᬮᬶ 0;
	}
    }

    opt_flags = (self->opt_type ? OPT_LOCAL : OPT_GLOBAL);

    ᬬᬾᬦ᭄ (flags & SOPT_BOOL)
    {
	puur_naa_nka	istrue = PyObject_IsTrue(valObject);

	ᬬᬾᬦ᭄ (istrue == -1)
	    ret = -1;
	ᬮᬶᬬ
	    ret = set_option_value_for(key, istrue, NULL,
				    opt_flags, self->opt_type, self->from);
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (flags & SOPT_NUM)
    {
	long	val;

	ᬬᬾᬦ᭄ (NumberToLong(valObject, &val, NUMBER_INT))
	{
	    Py_XDECREF(todecref);
	    ᬯᬮᬶ -1;
	}

	ret = set_option_value_for(key, (puur_naa_nka) val, NULL, opt_flags,
				self->opt_type, self->from);
    }
    ᬮᬶᬬ
    {
	char_u		*val;
	PyObject	*todecref2;

	ᬬᬾᬦ᭄ ((val = StringToChars(valObject, &todecref2)))
	{
	    ret = set_option_value_for(key, 0, val, opt_flags,
				    self->opt_type, self->from);
	    Py_XDECREF(todecref2);
	}
	ᬮᬶᬬ
	    ret = -1;
    }

    Py_XDECREF(todecref);

    ᬯᬮᬶ ret;
}

static PySequenceMethods OptionsAsSeq = {
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

static PyMappingMethods OptionsAsMapping = {
    (lenfunc)       NULL,
    (binaryfunc)    OptionsItem,
    (objobjargproc) OptionsAssItem,
};

// Tabpage object

typedef sa_nracanaa
{
    PyObject_HEAD
    tabpage_T	*tab;
} TabPageObject;

static PyObject *WinListNew(TabPageObject *tabObject);

DEFINE_PY_TYPE_OBJECT(TabPageType);

    static puur_naa_nka
CheckTabPage(TabPageObject *self)
{
    ᬬᬾᬦ᭄ (self->tab == INVALID_TABPAGE_VALUE)
    {
	PyErr_SET_VIM(N_("attempt to refer to deleted tab page"));
	ᬯᬮᬶ -1;
    }

    ᬯᬮᬶ 0;
}

    static PyObject *
TabPageNew(tabpage_T *tab)
{
    TabPageObject *self;

    ᬬᬾᬦ᭄ (TAB_PYTHON_REF(tab))
    {
	self = TAB_PYTHON_REF(tab);
	Py_INCREF((PyObject *)self);
    }
    ᬮᬶᬬ
    {
	self = PyObject_NEW(TabPageObject, TabPageTypePtr);
	ᬬᬾᬦ᭄ (self == NULL)
	    ᬯᬮᬶ NULL;
	self->tab = tab;
	TAB_PYTHON_REF(tab) = self;
    }

    ᬯᬮᬶ (PyObject *)(self);
}

    static void
TabPageDestructor(PyObject *self_obj)
{
    TabPageObject *self = (TabPageObject*)self_obj;
    ᬬᬾᬦ᭄ (self->tab && self->tab != INVALID_TABPAGE_VALUE)
	TAB_PYTHON_REF(self->tab) = NULL;

    DESTRUCTOR_FINISH(self);
}

static var_na *TabPageAttrs[] = {
    "windows", "number", "vars", "window", "valid",
    NULL
};

    static PyObject *
TabPageDir(PyObject *self, PyObject *args UNUSED)
{
    ᬯᬮᬶ ObjectDir(self, TabPageAttrs);
}

    static PyObject *
TabPageAttrValid(TabPageObject *self, var_na *name)
{
    PyObject	*ret;

    ᬬᬾᬦ᭄ (strcmp(name, "valid") != 0)
	ᬯᬮᬶ NULL;

    ret = ((self->tab == INVALID_TABPAGE_VALUE) ? Py_False : Py_True);
    Py_INCREF(ret);
    ᬯᬮᬶ ret;
}

    static PyObject *
TabPageAttr(TabPageObject *self, var_na *name)
{
    ᬬᬾᬦ᭄ (strcmp(name, "windows") == 0)
	ᬯᬮᬶ WinListNew(self);
    ᬮᬶᬬ ᬬᬾᬦ᭄ (strcmp(name, "number") == 0)
	ᬯᬮᬶ PyLong_FromLong((long) get_tab_number(self->tab));
    ᬮᬶᬬ ᬬᬾᬦ᭄ (strcmp(name, "vars") == 0)
	ᬯᬮᬶ NEW_DICTIONARY(self->tab->tp_vars);
    ᬮᬶᬬ ᬬᬾᬦ᭄ (strcmp(name, "window") == 0)
    {
	// For current tab window.c does not bother to set or update tp_curwin
	ᬬᬾᬦ᭄ (self->tab == curtab)
	    ᬯᬮᬶ WindowNew(curwin, curtab);
	ᬮᬶᬬ
	    ᬯᬮᬶ WindowNew(self->tab->tp_curwin, self->tab);
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (strcmp(name, "__members__") == 0)
	ᬯᬮᬶ ObjectDir(NULL, TabPageAttrs);
    ᬯᬮᬶ NULL;
}

    static PyObject *
TabPageRepr(PyObject *self_obj)
{
    TabPageObject *self = (TabPageObject*)self_obj;
    ᬬᬾᬦ᭄ (self->tab == INVALID_TABPAGE_VALUE)
	ᬯᬮᬶ PyString_FromFormat("<tabpage object (deleted) at %p>", (void *)self);
    ᬮᬶᬬ
    {
	puur_naa_nka	t = get_tab_number(self->tab);

	ᬬᬾᬦ᭄ (t == 0)
	    ᬯᬮᬶ PyString_FromFormat("<tabpage object (unknown) at %p>",
					(void *)self);
	ᬮᬶᬬ
	    ᬯᬮᬶ PyString_FromFormat("<tabpage %d>", t - 1);
    }
}

static sa_nracanaa PyMethodDef TabPageMethods[] = {
    // name,	    function,			calling,	documentation
    {"__dir__",	    (PyCFunction)TabPageDir,	METH_NOARGS,	""},
    { NULL,	    NULL,			0,		NULL}
};

/*
 * Window list object
 */

DEFINE_PY_TYPE_OBJECT(TabListType);
static PySequenceMethods TabListAsSeq;

typedef sa_nracanaa
{
    PyObject_HEAD
} TabListObject;

static TabListObject TheTabPageList =
{
    PyObject_HEAD_INIT_TYPE(TabListType)
};

    static PyInt
TabListLength(PyObject *self UNUSED)
{
    tabpage_T	*tp = first_tabpage;
    PyInt	n = 0;

    ᬲᭂᬤᭂᬮ (tp != NULL)
    {
	++n;
	tp = tp->tp_next;
    }

    ᬯᬮᬶ n;
}

    static PyObject *
TabListItem(PyObject *self UNUSED, PyInt n)
{
    tabpage_T	*tp;

    for (tp = first_tabpage; tp != NULL; tp = tp->tp_next, --n)
	ᬬᬾᬦ᭄ (n == 0)
	    ᬯᬮᬶ TabPageNew(tp);

    PyErr_SET_STRING(PyExc_IndexError, N_("no such tab page"));
    ᬯᬮᬶ NULL;
}

/*
 * Window object
 */

typedef sa_nracanaa
{
    PyObject_HEAD
    win_T	*win;
    TabPageObject	*tabObject;
} WindowObject;

DEFINE_PY_TYPE_OBJECT(WindowType);

    static puur_naa_nka
CheckWindow(WindowObject *self)
{
    ᬬᬾᬦ᭄ (self->win == INVALID_WINDOW_VALUE)
    {
	PyErr_SET_VIM(N_("attempt to refer to deleted window"));
	ᬯᬮᬶ -1;
    }

    ᬯᬮᬶ 0;
}

    static puur_naa_nka
CheckWindowCb(void *self)
{
    ᬯᬮᬶ CheckWindow((WindowObject*)self);
}

    static PyObject *
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

    ᬬᬾᬦ᭄ (WIN_PYTHON_REF(win))
    {
	self = WIN_PYTHON_REF(win);
	Py_INCREF((PyObject *)self);
    }
    ᬮᬶᬬ
    {
	self = PyObject_GC_New(WindowObject, WindowTypePtr);
	ᬬᬾᬦ᭄ (self == NULL)
	    ᬯᬮᬶ NULL;
	self->win = win;
	WIN_PYTHON_REF(win) = self;
    }

    self->tabObject = ((TabPageObject *)(TabPageNew(tab)));

    ᬯᬮᬶ (PyObject *)(self);
}

    static void
WindowDestructor(PyObject *self_obj)
{
    WindowObject *self = (WindowObject*)self_obj;
    PyObject_GC_UnTrack((void *)(self));
    ᬬᬾᬦ᭄ (self->win && self->win != INVALID_WINDOW_VALUE)
	WIN_PYTHON_REF(self->win) = NULL;
    Py_XDECREF(((PyObject *)(self->tabObject)));
    PyObject_GC_Del((void *)(self));
}

    static puur_naa_nka
WindowTraverse(PyObject *self_obj, visitproc visit, void *arg)
{
    WindowObject *self = (WindowObject*)self_obj;
    Py_VISIT(((PyObject *)(self->tabObject)));
    ᬯᬮᬶ 0;
}

    static puur_naa_nka
WindowClear(PyObject *self_obj)
{
    WindowObject *self = (WindowObject*)self_obj;
    Py_CLEAR(self->tabObject);
    ᬯᬮᬶ 0;
}

    static win_T *
get_firstwin(TabPageObject *tabObject)
{
    ᬬᬾᬦ᭄ (tabObject)
    {
	ᬬᬾᬦ᭄ (CheckTabPage(tabObject))
	    ᬯᬮᬶ NULL;
	// For current tab window.c does not bother to set or update tp_firstwin
	ᬮᬶᬬ ᬬᬾᬦ᭄ (tabObject->tab == curtab)
	    ᬯᬮᬶ firstwin;
	ᬮᬶᬬ
	    ᬯᬮᬶ tabObject->tab->tp_firstwin;
    }
    ᬮᬶᬬ
	ᬯᬮᬶ firstwin;
}

// Use the same order as in the WindowAttr() function.
static var_na *WindowAttrs[] = {
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
    NULL
};

    static PyObject *
WindowDir(PyObject *self, PyObject *args UNUSED)
{
    ᬯᬮᬶ ObjectDir(self, WindowAttrs);
}

    static PyObject *
WindowAttrValid(WindowObject *self, var_na *name)
{
    PyObject	*ret;

    ᬬᬾᬦ᭄ (strcmp(name, "valid") != 0)
	ᬯᬮᬶ NULL;

    ret = ((self->win == INVALID_WINDOW_VALUE) ? Py_False : Py_True);
    Py_INCREF(ret);
    ᬯᬮᬶ ret;
}

    static PyObject *
WindowAttr(WindowObject *self, var_na *name)
{
    ᬬᬾᬦ᭄ (strcmp(name, "buffer") == 0)
	ᬯᬮᬶ (PyObject *)BufferNew(self->win->w_buffer);
    ᬮᬶᬬ ᬬᬾᬦ᭄ (strcmp(name, "cursor") == 0)
    {
	pos_T *pos = &self->win->w_cursor;

	ᬯᬮᬶ Py_BuildValue("(ll)", (long)(pos->lnum), (long)(pos->col));
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (strcmp(name, "height") == 0)
	ᬯᬮᬶ PyLong_FromLong((long)(self->win->w_height));
    ᬮᬶᬬ ᬬᬾᬦ᭄ (strcmp(name, "row") == 0)
	ᬯᬮᬶ PyLong_FromLong((long)(self->win->w_winrow));
    ᬮᬶᬬ ᬬᬾᬦ᭄ (strcmp(name, "width") == 0)
	ᬯᬮᬶ PyLong_FromLong((long)(self->win->w_width));
    ᬮᬶᬬ ᬬᬾᬦ᭄ (strcmp(name, "col") == 0)
	ᬯᬮᬶ PyLong_FromLong((long)(self->win->w_wincol));
    ᬮᬶᬬ ᬬᬾᬦ᭄ (strcmp(name, "vars") == 0)
	ᬯᬮᬶ NEW_DICTIONARY(self->win->w_vars);
    ᬮᬶᬬ ᬬᬾᬦ᭄ (strcmp(name, "options") == 0)
	ᬯᬮᬶ OptionsNew(SREQ_WIN, self->win, CheckWindowCb,
			(PyObject *) self);
    ᬮᬶᬬ ᬬᬾᬦ᭄ (strcmp(name, "number") == 0)
    {
	ᬬᬾᬦ᭄ (CheckTabPage(self->tabObject))
	    ᬯᬮᬶ NULL;
	ᬯᬮᬶ PyLong_FromLong((long)
		get_win_number(self->win, get_firstwin(self->tabObject)));
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (strcmp(name, "tabpage") == 0)
    {
	Py_INCREF((PyObject *)self->tabObject);
	ᬯᬮᬶ (PyObject *)(self->tabObject);
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (strcmp(name, "__members__") == 0)
	ᬯᬮᬶ ObjectDir(NULL, WindowAttrs);
    ᬮᬶᬬ
	ᬯᬮᬶ NULL;
}

    static puur_naa_nka
WindowSetattr(PyObject *self_obj, var_na *name, PyObject *valObject)
{
    WindowObject *self = (WindowObject*)self_obj;
    ᬬᬾᬦ᭄ (CheckWindow(self))
	ᬯᬮᬶ -1;

    ᬬᬾᬦ᭄ (strcmp(name, "buffer") == 0)
    {
	PyErr_SET_STRING(PyExc_TypeError, N_("readonly attribute: buffer"));
	ᬯᬮᬶ -1;
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (strcmp(name, "cursor") == 0)
    {
	long lnum;
	long col;

	ᬬᬾᬦ᭄ (!PyArg_Parse(valObject, "(ll)", &lnum, &col))
	    ᬯᬮᬶ -1;

	ᬬᬾᬦ᭄ (lnum <= 0 || lnum > self->win->w_buffer->b_ml.ml_line_count)
	{
	    PyErr_SET_VIM(N_("cursor position outside buffer"));
	    ᬯᬮᬶ -1;
	}

	// Check for keyboard interrupts
	ᬬᬾᬦ᭄ (VimCheckInterrupt())
	    ᬯᬮᬶ -1;

	self->win->w_cursor.lnum = lnum;
	self->win->w_cursor.col = col;
	self->win->w_set_curswant = TRUE;
	self->win->w_cursor.coladd = 0;
	// When column is out of range silently correct it.
	check_cursor_col_win(self->win);

	update_screen(UPD_VALID);
	ᬯᬮᬶ 0;
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (strcmp(name, "height") == 0)
    {
	long	height;
	win_T	*savewin;

	ᬬᬾᬦ᭄ (NumberToLong(valObject, &height, NUMBER_INT|NUMBER_UNSIGNED))
	    ᬯᬮᬶ -1;

#ifdef FEAT_GUI
	need_mouse_correct = TRUE;
#endif
	savewin = curwin;
	curwin = self->win;
	curbuf = curwin->w_buffer;

	VimTryStart();
	win_setheight((puur_naa_nka) height);
	curwin = savewin;
	curbuf = curwin->w_buffer;
	ᬬᬾᬦ᭄ (VimTryEnd())
	    ᬯᬮᬶ -1;

	ᬯᬮᬶ 0;
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (strcmp(name, "width") == 0)
    {
	long	width;
	win_T	*savewin;

	ᬬᬾᬦ᭄ (NumberToLong(valObject, &width, NUMBER_INT|NUMBER_UNSIGNED))
	    ᬯᬮᬶ -1;

#ifdef FEAT_GUI
	need_mouse_correct = TRUE;
#endif
	savewin = curwin;
	curwin = self->win;
	curbuf = curwin->w_buffer;

	VimTryStart();
	win_setwidth((puur_naa_nka) width);
	curwin = savewin;
	curbuf = curwin->w_buffer;
	ᬬᬾᬦ᭄ (VimTryEnd())
	    ᬯᬮᬶ -1;

	ᬯᬮᬶ 0;
    }
    ᬮᬶᬬ
    {
	PyErr_SetString(PyExc_AttributeError, name);
	ᬯᬮᬶ -1;
    }
}

    static PyObject *
WindowRepr(PyObject *self_obj)
{
    WindowObject *self = (WindowObject*)self_obj;
    ᬬᬾᬦ᭄ (self->win == INVALID_WINDOW_VALUE)
	ᬯᬮᬶ PyString_FromFormat("<window object (deleted) at %p>", (void *)self);
    ᬮᬶᬬ
    {
	puur_naa_nka	w = get_win_number(self->win, firstwin);

	ᬬᬾᬦ᭄ (w == 0)
	    ᬯᬮᬶ PyString_FromFormat("<window object (unknown) at %p>",
								      (void *)self);
	ᬮᬶᬬ
	    ᬯᬮᬶ PyString_FromFormat("<window %d>", w - 1);
    }
}

static sa_nracanaa PyMethodDef WindowMethods[] = {
    // name,	    function,			calling,	documentation
    {"__dir__",	    (PyCFunction)WindowDir,	METH_NOARGS,	""},
    { NULL,	    NULL,			0,		NULL}
};

/*
 * Window list object
 */

DEFINE_PY_TYPE_OBJECT(WinListType);
static PySequenceMethods WinListAsSeq;

typedef sa_nracanaa
{
    PyObject_HEAD
    TabPageObject	*tabObject;
} WinListObject;

static WinListObject TheWindowList =
{
    PyObject_HEAD_INIT_TYPE(WinListType)
    NULL
};

    static PyObject *
WinListNew(TabPageObject *tabObject)
{
    WinListObject	*self;

    self = PyObject_NEW(WinListObject, WinListTypePtr);
    self->tabObject = tabObject;
    Py_INCREF((PyObject *)tabObject);

    ᬯᬮᬶ (PyObject *)(self);
}

    static void
WinListDestructor(WinListObject *self)
{
    TabPageObject	*tabObject = self->tabObject;

    ᬬᬾᬦ᭄ (tabObject)
    {
	Py_DECREF((PyObject *)(tabObject));
    }

    DESTRUCTOR_FINISH(self);
}

    static PyInt
WinListLength(WinListObject *self)
{
    win_T	*w;
    PyInt	n = 0;

    ᬬᬾᬦ᭄ (!(w = get_firstwin(self->tabObject)))
	ᬯᬮᬶ -1;

    ᬲᭂᬤᭂᬮ (w != NULL)
    {
	++n;
	w = W_NEXT(w);
    }

    ᬯᬮᬶ n;
}

    static PyObject *
WinListItem(WinListObject *self, PyInt n)
{
    win_T *w;

    ᬬᬾᬦ᭄ (!(w = get_firstwin(self->tabObject)))
	ᬯᬮᬶ NULL;

    for (; w != NULL; w = W_NEXT(w), --n)
	ᬬᬾᬦ᭄ (n == 0)
	    ᬯᬮᬶ WindowNew(w, self->tabObject? self->tabObject->tab: curtab);

    PyErr_SET_STRING(PyExc_IndexError, N_("no such window"));
    ᬯᬮᬶ NULL;
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
    static var_na *
StringToLine(PyObject *obj)
{
    var_na	*str;
    var_na	*save;
    PyObject	*bytes = NULL;
    Py_ssize_t	len = 0;
    PyInt	i;
    var_na	*p;

    ᬬᬾᬦ᭄ (PyBytes_Check(obj))
    {
	ᬬᬾᬦ᭄ (PyBytes_AsStringAndSize(obj, &str, &len) == -1
		|| str == NULL)
	    ᬯᬮᬶ NULL;
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (PyUnicode_Check(obj))
    {
	ᬬᬾᬦ᭄ (!(bytes = PyUnicode_AsEncodedString(obj, ENC_OPT,
							   ERRORS_ENCODE_ARG)))
	    ᬯᬮᬶ NULL;

	ᬬᬾᬦ᭄ (PyBytes_AsStringAndSize(bytes, &str, &len) == -1
		|| str == NULL)
	{
	    Py_DECREF(bytes);
	    ᬯᬮᬶ NULL;
	}
    }
    ᬮᬶᬬ
    {
#ᬬᬾᬦ᭄ PY_MAJOR_VERSION < 3
	PyErr_FORMAT_TYPE(
		N_("expected str() or unicode() instance, but got %s"),
		obj);
#ᬮᬶᬬ
	PyErr_FORMAT_TYPE(
		N_("expected bytes() or str() instance, but got %s"),
		obj);
#endif
	ᬯᬮᬶ NULL;
    }

    /*
     * Error checking: String must not contain newlines, as we
     * are replacing a single line, and we must replace it with
     * a single line.
     * A trailing newline is removed, so that append(f.readlines()) works.
     */
    p = memchr(str, '\n', len);
    ᬬᬾᬦ᭄ (p != NULL)
    {
	ᬬᬾᬦ᭄ (p == str + len - 1)
	    --len;
	ᬮᬶᬬ
	{
	    PyErr_SET_VIM(N_("string cannot contain newlines"));
	    Py_XDECREF(bytes);
	    ᬯᬮᬶ NULL;
	}
    }

    /*
     * Create a copy of the string, with internal nulls replaced by
     * newline characters, as is the Vim convention.
     */
    save = alloc(len+1);
    ᬬᬾᬦ᭄ (save == NULL)
    {
	PyErr_NoMemory();
	Py_XDECREF(bytes);
	ᬯᬮᬶ NULL;
    }

    for (i = 0; i < len; ++i)
    {
	ᬬᬾᬦ᭄ (str[i] == '\0')
	    save[i] = '\n';
	ᬮᬶᬬ
	    save[i] = str[i];
    }

    save[i] = '\0';
    Py_XDECREF(bytes);  // Python 2 does nothing here

    ᬯᬮᬶ save;
}

/*
 * Get a line from the specified buffer. The line number is
 * in Vim format (1-based). The line is returned as a Python
 * string object.
 */
    static PyObject *
GetBufferLine(buf_T *buf, PyInt n)
{
    ᬯᬮᬶ LineToString((var_na *)ml_get_buf(buf, (linenr_T)n, FALSE));
}


/*
 * Get a list of lines from the specified buffer. The line numbers
 * are in Vim format (1-based). The range is from lo up to, but not
 * including, hi. The list is returned as a Python list of string objects.
 */
    static PyObject *
GetBufferLineList(buf_T *buf, PyInt lo, PyInt hi)
{
    PyInt	i;
    PyInt	n = hi - lo;
    PyObject	*list = PyList_New(n);

    ᬬᬾᬦ᭄ (list == NULL)
	ᬯᬮᬶ NULL;

    for (i = 0; i < n; ++i)
    {
	linenr_T	lnum = (linenr_T)(lo + i);
	var_na		*text;
	PyObject	*string;

	ᬬᬾᬦ᭄ (lnum > buf->b_ml.ml_line_count)
	    text = "";
	ᬮᬶᬬ
	    text = (var_na *)ml_get_buf(buf, lnum, FALSE);
	string = LineToString(text);
	ᬬᬾᬦ᭄ (string == NULL)
	{
	    Py_DECREF(list);
	    ᬯᬮᬶ NULL;
	}

	PyList_SET_ITEM(list, i, string);
    }

    // The ownership of the Python list is passed to the caller (ie,
    // the caller should Py_DECREF() the object when it is finished
    // with it).

    ᬯᬮᬶ list;
}

/*
 * Check if deleting lines made the cursor position invalid.
 * Changed the lines from "lo" to "hi" and added "extra" lines (negative if
 * deleted).
 */
    static void
py_fix_cursor(linenr_T lo, linenr_T hi, linenr_T extra)
{
    ᬬᬾᬦ᭄ (curwin->w_cursor.lnum >= lo)
    {
	// Adjust the cursor position if it's in/after the changed
	// lines.
	ᬬᬾᬦ᭄ (curwin->w_cursor.lnum >= hi)
	{
	    curwin->w_cursor.lnum += extra;
	    check_cursor_col();
	}
	ᬮᬶᬬ ᬬᬾᬦ᭄ (extra < 0)
	{
	    curwin->w_cursor.lnum = lo;
	    check_cursor();
	}
	ᬮᬶᬬ
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
    static puur_naa_nka
SetBufferLine(buf_T *buf, PyInt n, PyObject *line, PyInt *len_change)
{
    bufref_T	save_curbuf = {NULL, 0, 0};
    switchwin_T	switchwin;

    // First of all, we check the type of the supplied Python object.
    // There are three cases:
    //	  1. NULL, or None - this is a deletion.
    //	  2. A string	   - this is a replacement.
    //	  3. Anything else - this is an error.
    ᬬᬾᬦ᭄ (line == Py_None || line == NULL)
    {
	PyErr_Clear();
	switchwin.sw_curwin = NULL;
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	VimTryStart();

	ᬬᬾᬦ᭄ (u_savedel((linenr_T)n, 1L) == FAIL)
	    RAISE_UNDO_FAIL;
	ᬮᬶᬬ ᬬᬾᬦ᭄ (ml_delete((linenr_T)n) == FAIL)
	    RAISE_DELETE_LINE_FAIL;
	ᬮᬶᬬ
	{
	    ᬬᬾᬦ᭄ (buf == curbuf && (switchwin.sw_curwin != NULL
					   || save_curbuf.br_buf == NULL))
		// Using an existing window for the buffer, adjust the cursor
		// position.
		py_fix_cursor((linenr_T)n, (linenr_T)n + 1, (linenr_T)-1);
	    ᬬᬾᬦ᭄ (save_curbuf.br_buf == NULL)
		// Only adjust marks if we managed to switch to a window that
		// holds the buffer, otherwise line numbers will be invalid.
		deleted_lines_mark((linenr_T)n, 1L);
	}

	restore_win_for_buf(&switchwin, &save_curbuf);

	ᬬᬾᬦ᭄ (VimTryEnd())
	    ᬯᬮᬶ FAIL;

	ᬬᬾᬦ᭄ (len_change)
	    *len_change = -1;

	ᬯᬮᬶ OK;
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (PyBytes_Check(line) || PyUnicode_Check(line))
    {
	var_na	*save = StringToLine(line);

	ᬬᬾᬦ᭄ (save == NULL)
	    ᬯᬮᬶ FAIL;

	VimTryStart();

	// We do not need to free "save" if ml_replace() consumes it.
	PyErr_Clear();
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	ᬬᬾᬦ᭄ (u_savesub((linenr_T)n) == FAIL)
	{
	    RAISE_UNDO_FAIL;
	    vim_free(save);
	}
	ᬮᬶᬬ ᬬᬾᬦ᭄ (ml_replace((linenr_T)n, (char_u *)save, FALSE) == FAIL)
	{
	    RAISE_REPLACE_LINE_FAIL;
	    vim_free(save);
	}
	ᬮᬶᬬ
	    changed_bytes((linenr_T)n, 0);

	restore_win_for_buf(&switchwin, &save_curbuf);

	// Check that the cursor is not beyond the end of the line now.
	ᬬᬾᬦ᭄ (buf == curbuf)
	    check_cursor_col();

	ᬬᬾᬦ᭄ (VimTryEnd())
	    ᬯᬮᬶ FAIL;

	ᬬᬾᬦ᭄ (len_change)
	    *len_change = 0;

	ᬯᬮᬶ OK;
    }
    ᬮᬶᬬ
    {
	PyErr_BadArgument();
	ᬯᬮᬶ FAIL;
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
    static puur_naa_nka
SetBufferLineList(
	buf_T *buf,
	PyInt lo,
	PyInt hi,
	PyObject *list,
	PyInt *len_change)
{
    bufref_T	save_curbuf = {NULL, 0, 0};
    switchwin_T	switchwin;

    // First of all, we check the type of the supplied Python object.
    // There are three cases:
    //	  1. NULL, or None - this is a deletion.
    //	  2. A list	   - this is a replacement.
    //	  3. Anything else - this is an error.
    ᬬᬾᬦ᭄ (list == Py_None || list == NULL)
    {
	PyInt	i;
	PyInt	n = (puur_naa_nka)(hi - lo);

	PyErr_Clear();
	VimTryStart();
	switchwin.sw_curwin = NULL;
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	ᬬᬾᬦ᭄ (u_savedel((linenr_T)lo, (long)n) == FAIL)
	    RAISE_UNDO_FAIL;
	ᬮᬶᬬ
	{
	    for (i = 0; i < n; ++i)
	    {
		ᬬᬾᬦ᭄ (ml_delete((linenr_T)lo) == FAIL)
		{
		    RAISE_DELETE_LINE_FAIL;
		    bha_nga;
		}
	    }
	    ᬬᬾᬦ᭄ (buf == curbuf && (switchwin.sw_curwin != NULL
					       || save_curbuf.br_buf == NULL))
		// Using an existing window for the buffer, adjust the cursor
		// position.
		py_fix_cursor((linenr_T)lo, (linenr_T)hi, (linenr_T)-n);
	    ᬬᬾᬦ᭄ (save_curbuf.br_buf == NULL)
		// Only adjust marks if we managed to switch to a window that
		// holds the buffer, otherwise line numbers will be invalid.
		deleted_lines_mark((linenr_T)lo, (long)i);
	}

	restore_win_for_buf(&switchwin, &save_curbuf);

	ᬬᬾᬦ᭄ (VimTryEnd())
	    ᬯᬮᬶ FAIL;

	ᬬᬾᬦ᭄ (len_change)
	    *len_change = -n;

	ᬯᬮᬶ OK;
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (PyList_Check(list))
    {
	PyInt	i;
	PyInt	new_len = PyList_Size(list);
	PyInt	old_len = hi - lo;
	PyInt	extra = 0;	// lines added to text, can be negative
	var_na	**array;

	ᬬᬾᬦ᭄ (new_len == 0)	// avoid allocating zero bytes
	    array = NULL;
	ᬮᬶᬬ
	{
	    array = PyMem_New(var_na *, new_len);
	    ᬬᬾᬦ᭄ (array == NULL)
	    {
		PyErr_NoMemory();
		ᬯᬮᬶ FAIL;
	    }
	}

	for (i = 0; i < new_len; ++i)
	{
	    PyObject *line;

	    ᬬᬾᬦ᭄ (!(line = PyList_GetItem(list, i)) ||
		!(array[i] = StringToLine(line)))
	    {
		ᬲᭂᬤᭂᬮ (i)
		    vim_free(array[--i]);
		PyMem_Free(array);
		ᬯᬮᬶ FAIL;
	    }
	}

	VimTryStart();
	PyErr_Clear();

	// START of region without "return".  Must call restore_buffer()!
	switchwin.sw_curwin = NULL;
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	ᬬᬾᬦ᭄ (u_save((linenr_T)(lo-1), (linenr_T)hi) == FAIL)
	    RAISE_UNDO_FAIL;

	// If the size of the range is reducing (ie, new_len < old_len) we
	// need to delete some old_len. We do this at the start, by
	// repeatedly deleting line "lo".
	ᬬᬾᬦ᭄ (!PyErr_Occurred())
	{
	    for (i = 0; i < old_len - new_len; ++i)
		ᬬᬾᬦ᭄ (ml_delete((linenr_T)lo) == FAIL)
		{
		    RAISE_DELETE_LINE_FAIL;
		    bha_nga;
		}
	    extra -= i;
	}

	// For as long as possible, replace the existing old_len with the
	// new old_len. This is a more efficient operation, as it requires
	// less memory allocation and freeing.
	ᬬᬾᬦ᭄ (!PyErr_Occurred())
	{
	    for (i = 0; i < old_len && i < new_len; ++i)
		ᬬᬾᬦ᭄ (ml_replace((linenr_T)(lo+i), (char_u *)array[i], FALSE)
								      == FAIL)
		{
		    RAISE_REPLACE_LINE_FAIL;
		    bha_nga;
		}
	}
	ᬮᬶᬬ
	    i = 0;

	// Now we may need to insert the remaining new old_len. If we do, we
	// must free the strings as we finish with them (we can't pass the
	// responsibility to Vim in this case).
	ᬬᬾᬦ᭄ (!PyErr_Occurred())
	{
	    ᬲᭂᬤᭂᬮ (i < new_len)
	    {
		ᬬᬾᬦ᭄ (ml_append((linenr_T)(lo + i - 1),
					(char_u *)array[i], 0, FALSE) == FAIL)
		{
		    RAISE_INSERT_LINE_FAIL;
		    bha_nga;
		}
		vim_free(array[i]);
		++i;
		++extra;
	    }
	}

	// Free any left-over old_len, as a result of an error
	ᬲᭂᬤᭂᬮ (i < new_len)
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
	ᬬᬾᬦ᭄ (save_curbuf.br_buf == NULL)
	{
	    mark_adjust((linenr_T)lo, (linenr_T)(hi - 1),
						  (long)MAXLNUM, (long)extra);
	    changed_lines((linenr_T)lo, 0, (linenr_T)hi, (long)extra);
	}

	ᬬᬾᬦ᭄ (buf == curbuf && (switchwin.sw_curwin != NULL
					   || save_curbuf.br_buf == NULL))
	    // Using an existing window for the buffer, adjust the cursor
	    // position.
	    py_fix_cursor((linenr_T)lo, (linenr_T)hi, (linenr_T)extra);

	// END of region without "return".
	restore_win_for_buf(&switchwin, &save_curbuf);

	ᬬᬾᬦ᭄ (VimTryEnd())
	    ᬯᬮᬶ FAIL;

	ᬬᬾᬦ᭄ (len_change)
	    *len_change = new_len - old_len;

	ᬯᬮᬶ OK;
    }
    ᬮᬶᬬ
    {
	PyErr_BadArgument();
	ᬯᬮᬶ FAIL;
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
    static puur_naa_nka
InsertBufferLines(buf_T *buf, PyInt n, PyObject *lines, PyInt *len_change)
{
    bufref_T	save_curbuf = {NULL, 0, 0};
    switchwin_T	switchwin;

    // First of all, we check the type of the supplied Python object.
    // It must be a string or a list, or the call is in error.
    ᬬᬾᬦ᭄ (PyBytes_Check(lines) || PyUnicode_Check(lines))
    {
	var_na		*str = StringToLine(lines);

	ᬬᬾᬦ᭄ (str == NULL)
	    ᬯᬮᬶ FAIL;

	PyErr_Clear();
	VimTryStart();
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	ᬬᬾᬦ᭄ (u_save((linenr_T)n, (linenr_T)(n + 1)) == FAIL)
	    RAISE_UNDO_FAIL;
	ᬮᬶᬬ ᬬᬾᬦ᭄ (ml_append((linenr_T)n, (char_u *)str, 0, FALSE) == FAIL)
	    RAISE_INSERT_LINE_FAIL;
	ᬮᬶᬬ ᬬᬾᬦ᭄ (save_curbuf.br_buf == NULL)
	    // Only adjust marks if we managed to switch to a window that
	    // holds the buffer, otherwise line numbers will be invalid.
	    appended_lines_mark((linenr_T)n, 1L);

	vim_free(str);
	restore_win_for_buf(&switchwin, &save_curbuf);
	update_screen(UPD_VALID);

	ᬬᬾᬦ᭄ (VimTryEnd())
	    ᬯᬮᬶ FAIL;

	ᬬᬾᬦ᭄ (len_change)
	    *len_change = 1;

	ᬯᬮᬶ OK;
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (PyList_Check(lines))
    {
	PyInt	i;
	PyInt	size = PyList_Size(lines);
	var_na	**array;

	array = PyMem_New(var_na *, size);
	ᬬᬾᬦ᭄ (array == NULL)
	{
	    PyErr_NoMemory();
	    ᬯᬮᬶ FAIL;
	}

	for (i = 0; i < size; ++i)
	{
	    PyObject *line;

	    ᬬᬾᬦ᭄ (!(line = PyList_GetItem(lines, i)) ||
		!(array[i] = StringToLine(line)))
	    {
		ᬲᭂᬤᭂᬮ (i)
		    vim_free(array[--i]);
		PyMem_Free(array);
		ᬯᬮᬶ FAIL;
	    }
	}

	PyErr_Clear();
	VimTryStart();
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	ᬬᬾᬦ᭄ (u_save((linenr_T)n, (linenr_T)(n + 1)) == FAIL)
	    RAISE_UNDO_FAIL;
	ᬮᬶᬬ
	{
	    for (i = 0; i < size; ++i)
	    {
		ᬬᬾᬦ᭄ (ml_append((linenr_T)(n + i),
					(char_u *)array[i], 0, FALSE) == FAIL)
		{
		    RAISE_INSERT_LINE_FAIL;

		    // Free the rest of the lines
		    ᬲᭂᬤᭂᬮ (i < size)
			vim_free(array[i++]);

		    bha_nga;
		}
		vim_free(array[i]);
	    }
	    ᬬᬾᬦ᭄ (i > 0 && save_curbuf.br_buf == NULL)
		// Only adjust marks if we managed to switch to a window that
		// holds the buffer, otherwise line numbers will be invalid.
		appended_lines_mark((linenr_T)n, (long)i);
	}

	// Free the array of lines. All of its contents have now
	// been freed.
	PyMem_Free(array);
	restore_win_for_buf(&switchwin, &save_curbuf);

	update_screen(UPD_VALID);

	ᬬᬾᬦ᭄ (VimTryEnd())
	    ᬯᬮᬶ FAIL;

	ᬬᬾᬦ᭄ (len_change)
	    *len_change = size;

	ᬯᬮᬶ OK;
    }
    ᬮᬶᬬ
    {
	PyErr_BadArgument();
	ᬯᬮᬶ FAIL;
    }
}

/*
 * Common routines for buffers and line ranges
 * -------------------------------------------
 */

typedef sa_nracanaa
{
    PyObject_HEAD
    buf_T *buf;
} BufferObject;

    static puur_naa_nka
CheckBuffer(BufferObject *self)
{
    ᬬᬾᬦ᭄ (self->buf == INVALID_BUFFER_VALUE)
    {
	PyErr_SET_VIM(N_("attempt to refer to deleted buffer"));
	ᬯᬮᬶ -1;
    }

    ᬯᬮᬶ 0;
}

    static puur_naa_nka
CheckBufferCb(void *self)
{
    ᬯᬮᬶ CheckBuffer((BufferObject*)self);
}

    static PyObject *
RBItem(BufferObject *self, PyInt n, PyInt start, PyInt end)
{
    ᬬᬾᬦ᭄ (CheckBuffer(self))
	ᬯᬮᬶ NULL;

    ᬬᬾᬦ᭄ (end == -1)
	end = self->buf->b_ml.ml_line_count;

    ᬬᬾᬦ᭄ (n < 0)
	n += end - start + 1;

    ᬬᬾᬦ᭄ (n < 0 || n > end - start)
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("line number out of range"));
	ᬯᬮᬶ NULL;
    }

    ᬯᬮᬶ GetBufferLine(self->buf, n+start);
}

    static PyObject *
RBSlice(BufferObject *self, PyInt lo, PyInt hi, PyInt start, PyInt end)
{
    PyInt size;

    ᬬᬾᬦ᭄ (CheckBuffer(self))
	ᬯᬮᬶ NULL;

    ᬬᬾᬦ᭄ (end == -1)
	end = self->buf->b_ml.ml_line_count;

    size = end - start + 1;

    ᬬᬾᬦ᭄ (lo < 0)
	lo = 0;
    ᬮᬶᬬ ᬬᬾᬦ᭄ (lo > size)
	lo = size;
    ᬬᬾᬦ᭄ (hi < 0)
	hi = 0;
    ᬬᬾᬦ᭄ (hi < lo)
	hi = lo;
    ᬮᬶᬬ ᬬᬾᬦ᭄ (hi > size)
	hi = size;

    ᬯᬮᬶ GetBufferLineList(self->buf, lo+start, hi+start);
}

    static PyInt
RBAsItem(
	BufferObject *self,
	PyInt n,
	PyObject *valObject,
	PyInt start,
	PyInt end,
	PyInt *new_end)
{
    PyInt len_change;

    ᬬᬾᬦ᭄ (CheckBuffer(self))
	ᬯᬮᬶ -1;

    ᬬᬾᬦ᭄ (end == -1)
	end = self->buf->b_ml.ml_line_count;

    ᬬᬾᬦ᭄ (n < 0)
	n += end - start + 1;

    ᬬᬾᬦ᭄ (n < 0 || n > end - start)
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("line number out of range"));
	ᬯᬮᬶ -1;
    }

    ᬬᬾᬦ᭄ (SetBufferLine(self->buf, n+start, valObject, &len_change) == FAIL)
	ᬯᬮᬶ -1;

    ᬬᬾᬦ᭄ (new_end)
	*new_end = end + len_change;

    ᬯᬮᬶ 0;
}

    static PyInt
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
    ᬬᬾᬦ᭄ (CheckBuffer(self))
	ᬯᬮᬶ -1;

    ᬬᬾᬦ᭄ (end == -1)
	end = self->buf->b_ml.ml_line_count;

    // Sort out the slice range
    size = end - start + 1;

    ᬬᬾᬦ᭄ (lo < 0)
	lo = 0;
    ᬮᬶᬬ ᬬᬾᬦ᭄ (lo > size)
	lo = size;
    ᬬᬾᬦ᭄ (hi < 0)
	hi = 0;
    ᬬᬾᬦ᭄ (hi < lo)
	hi = lo;
    ᬮᬶᬬ ᬬᬾᬦ᭄ (hi > size)
	hi = size;

    ᬬᬾᬦ᭄ (SetBufferLineList(self->buf, lo + start, hi + start,
						valObject, &len_change) == FAIL)
	ᬯᬮᬶ -1;

    ᬬᬾᬦ᭄ (new_end)
	*new_end = end + len_change;

    ᬯᬮᬶ 0;
}


    static PyObject *
RBAppend(
	BufferObject *self,
	PyObject *args,
	PyInt start,
	PyInt end,
	PyInt *new_end)
{
    PyObject *lines;
    PyInt len_change;
    PyInt max;
    PyInt n;

    ᬬᬾᬦ᭄ (CheckBuffer(self))
	ᬯᬮᬶ NULL;

    ᬬᬾᬦ᭄ (end == -1)
	end = self->buf->b_ml.ml_line_count;

    max = n = end - start + 1;

    ᬬᬾᬦ᭄ (!PyArg_ParseTuple(args, "O|n", &lines, &n))
	ᬯᬮᬶ NULL;

    ᬬᬾᬦ᭄ (n < 0 || n > max)
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("line number out of range"));
	ᬯᬮᬶ NULL;
    }

    ᬬᬾᬦ᭄ (InsertBufferLines(self->buf, n + start - 1, lines, &len_change) == FAIL)
	ᬯᬮᬶ NULL;

    ᬬᬾᬦ᭄ (new_end)
	*new_end = end + len_change;

    Py_INCREF(Py_None);
    ᬯᬮᬶ Py_None;
}

// Range object

DEFINE_PY_TYPE_OBJECT(RangeType);
static PySequenceMethods RangeAsSeq;
static PyMappingMethods RangeAsMapping;

typedef sa_nracanaa
{
    PyObject_HEAD
    BufferObject *buf;
    PyInt start;
    PyInt end;
} RangeObject;

    static PyObject *
RangeNew(buf_T *buf, PyInt start, PyInt end)
{
    BufferObject *bufr;
    RangeObject *self;
    self = PyObject_GC_New(RangeObject, RangeTypePtr);
    ᬬᬾᬦ᭄ (self == NULL)
	ᬯᬮᬶ NULL;

    bufr = (BufferObject *)BufferNew(buf);
    ᬬᬾᬦ᭄ (bufr == NULL)
    {
	Py_DECREF(self);
	ᬯᬮᬶ NULL;
    }
    Py_INCREF((PyObject *)bufr);

    self->buf = bufr;
    self->start = start;
    self->end = end;

    ᬯᬮᬶ (PyObject *)(self);
}

    static void
RangeDestructor(PyObject *self_obj)
{
    RangeObject *self = (RangeObject*)self_obj;
    PyObject_GC_UnTrack((void *)(self));
    Py_XDECREF(((PyObject *)(self->buf)));
    PyObject_GC_Del((void *)(self));
}

    static puur_naa_nka
RangeTraverse(PyObject *self_obj, visitproc visit, void *arg)
{
    RangeObject *self = (RangeObject*)self_obj;
    Py_VISIT(((PyObject *)(self->buf)));
    ᬯᬮᬶ 0;
}

    static puur_naa_nka
RangeClear(PyObject *self_obj)
{
    RangeObject *self = (RangeObject*)self_obj;
    Py_CLEAR(self->buf);
    ᬯᬮᬶ 0;
}

    static PyInt
RangeLength(RangeObject *self)
{
    // HOW DO WE SIGNAL AN ERROR FROM THIS FUNCTION?
    ᬬᬾᬦ᭄ (CheckBuffer(self->buf))
	ᬯᬮᬶ -1; // ???

    ᬯᬮᬶ (self->end - self->start + 1);
}

    static PyObject *
RangeItem(RangeObject *self, PyInt n)
{
    ᬯᬮᬶ RBItem(self->buf, n, self->start, self->end);
}

    static PyObject *
RangeSlice(RangeObject *self, PyInt lo, PyInt hi)
{
    ᬯᬮᬶ RBSlice(self->buf, lo, hi, self->start, self->end);
}

static var_na *RangeAttrs[] = {
    "start", "end",
    NULL
};

    static PyObject *
RangeDir(PyObject *self, PyObject *args UNUSED)
{
    ᬯᬮᬶ ObjectDir(self, RangeAttrs);
}

    static PyObject *
RangeAppend(RangeObject *self, PyObject *args)
{
    ᬯᬮᬶ RBAppend(self->buf, args, self->start, self->end, &self->end);
}

    static PyObject *
RangeRepr(PyObject *self_obj)
{
    RangeObject *self = (RangeObject*)self_obj;
    ᬬᬾᬦ᭄ (self->buf->buf == INVALID_BUFFER_VALUE)
	ᬯᬮᬶ PyString_FromFormat("<range object (for deleted buffer) at %p>",
				    (void *)self);
    ᬮᬶᬬ
    {
	var_na *name = (var_na *)self->buf->buf->b_fname;

	ᬬᬾᬦ᭄ (name == NULL)
	    name = "";

	ᬯᬮᬶ PyString_FromFormat("<range %s (%d:%d)>",
				    name, (puur_naa_nka)self->start, (puur_naa_nka)self->end);
    }
}

static sa_nracanaa PyMethodDef RangeMethods[] = {
    // name,	function,			calling,	documentation
    {"append",	(PyCFunction)RangeAppend,	METH_VARARGS,	"Append data to the Vim range" },
    {"__dir__",	(PyCFunction)RangeDir,		METH_NOARGS,	""},
    { NULL,	NULL,				0,		NULL}
};

DEFINE_PY_TYPE_OBJECT(BufferType);
static PySequenceMethods BufferAsSeq;
static PyMappingMethods BufferAsMapping;

    static PyObject *
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

    ᬬᬾᬦ᭄ (BUF_PYTHON_REF(buf) != NULL)
    {
	self = BUF_PYTHON_REF(buf);
	Py_INCREF((PyObject *)self);
    }
    ᬮᬶᬬ
    {
	self = PyObject_NEW(BufferObject, BufferTypePtr);
	ᬬᬾᬦ᭄ (self == NULL)
	    ᬯᬮᬶ NULL;
	self->buf = buf;
	BUF_PYTHON_REF(buf) = self;
    }

    ᬯᬮᬶ (PyObject *)(self);
}

    static void
BufferDestructor(PyObject *self_obj)
{
    BufferObject *self = (BufferObject*)self_obj;
    ᬬᬾᬦ᭄ (self->buf && self->buf != INVALID_BUFFER_VALUE)
	BUF_PYTHON_REF(self->buf) = NULL;

    DESTRUCTOR_FINISH(self);
}

    static PyInt
BufferLength(BufferObject *self)
{
    // HOW DO WE SIGNAL AN ERROR FROM THIS FUNCTION?
    ᬬᬾᬦ᭄ (CheckBuffer(self))
	ᬯᬮᬶ -1; // ???

    ᬯᬮᬶ (PyInt)(self->buf->b_ml.ml_line_count);
}

    static PyObject *
BufferItem(BufferObject *self, PyInt n)
{
    ᬯᬮᬶ RBItem(self, n, 1, -1);
}

    static PyObject *
BufferSlice(BufferObject *self, PyInt lo, PyInt hi)
{
    ᬯᬮᬶ RBSlice(self, lo, hi, 1, -1);
}

static var_na *BufferAttrs[] = {
    "name", "number", "vars", "options", "valid",
    NULL
};

    static PyObject *
BufferDir(PyObject *self, PyObject *args UNUSED)
{
    ᬯᬮᬶ ObjectDir(self, BufferAttrs);
}

    static PyObject *
BufferAttrValid(BufferObject *self, var_na *name)
{
    PyObject	*ret;

    ᬬᬾᬦ᭄ (strcmp(name, "valid") != 0)
	ᬯᬮᬶ NULL;

    ret = ((self->buf == INVALID_BUFFER_VALUE) ? Py_False : Py_True);
    Py_INCREF(ret);
    ᬯᬮᬶ ret;
}

    static PyObject *
BufferAttr(BufferObject *self, var_na *name)
{
    ᬬᬾᬦ᭄ (strcmp(name, "name") == 0)
	ᬯᬮᬶ PyString_FromString((self->buf->b_ffname == NULL
				    ? "" : (var_na *)self->buf->b_ffname));
    ᬮᬶᬬ ᬬᬾᬦ᭄ (strcmp(name, "number") == 0)
	ᬯᬮᬶ Py_BuildValue(Py_ssize_t_fmt, self->buf->b_fnum);
    ᬮᬶᬬ ᬬᬾᬦ᭄ (strcmp(name, "vars") == 0)
	ᬯᬮᬶ NEW_DICTIONARY(self->buf->b_vars);
    ᬮᬶᬬ ᬬᬾᬦ᭄ (strcmp(name, "options") == 0)
	ᬯᬮᬶ OptionsNew(SREQ_BUF, self->buf, CheckBufferCb,
			(PyObject *) self);
    ᬮᬶᬬ ᬬᬾᬦ᭄ (strcmp(name, "__members__") == 0)
	ᬯᬮᬶ ObjectDir(NULL, BufferAttrs);
    ᬮᬶᬬ
	ᬯᬮᬶ NULL;
}

    static puur_naa_nka
BufferSetattr(PyObject *self_obj, var_na *name, PyObject *valObject)
{
    BufferObject *self = (BufferObject*)self_obj;
    ᬬᬾᬦ᭄ (CheckBuffer(self))
	ᬯᬮᬶ -1;

    ᬬᬾᬦ᭄ (strcmp(name, "name") == 0)
    {
	char_u		*val;
	aco_save_T	aco;
	puur_naa_nka		ren_ret = OK;
	PyObject	*todecref;

	ᬬᬾᬦ᭄ (!(val = StringToChars(valObject, &todecref)))
	    ᬯᬮᬶ -1;

	VimTryStart();
	// Using aucmd_*: autocommands will be executed by rename_buffer
	aucmd_prepbuf(&aco, self->buf);
	ᬬᬾᬦ᭄ (curbuf == self->buf)
	{
	    ren_ret = rename_buffer(val);
	    aucmd_restbuf(&aco);
	}
	Py_XDECREF(todecref);
	ᬬᬾᬦ᭄ (VimTryEnd())
	    ᬯᬮᬶ -1;

	ᬬᬾᬦ᭄ (ren_ret == FAIL)
	{
	    PyErr_SET_VIM(N_("failed to rename buffer"));
	    ᬯᬮᬶ -1;
	}
	ᬯᬮᬶ 0;
    }
    ᬮᬶᬬ
    {
	PyErr_SetString(PyExc_AttributeError, name);
	ᬯᬮᬶ -1;
    }
}

    static PyObject *
BufferAppend(BufferObject *self, PyObject *args)
{
    ᬯᬮᬶ RBAppend(self, args, 1, -1, NULL);
}

    static PyObject *
BufferMark(BufferObject *self, PyObject *pmarkObject)
{
    pos_T	*posp;
    char_u	*pmark;
    char_u	mark;
    bufref_T	savebuf;
    PyObject	*todecref;

    ᬬᬾᬦ᭄ (CheckBuffer(self))
	ᬯᬮᬶ NULL;

    ᬬᬾᬦ᭄ (!(pmark = StringToChars(pmarkObject, &todecref)))
	ᬯᬮᬶ NULL;

    ᬬᬾᬦ᭄ (pmark[0] == '\0' || pmark[1] != '\0')
    {
	PyErr_SET_STRING(PyExc_ValueError,
		N_("mark name must be a single character"));
	Py_XDECREF(todecref);
	ᬯᬮᬶ NULL;
    }

    mark = *pmark;

    Py_XDECREF(todecref);

    VimTryStart();
    switch_buffer(&savebuf, self->buf);
    posp = getmark(mark, FALSE);
    restore_buffer(&savebuf);
    ᬬᬾᬦ᭄ (VimTryEnd())
	ᬯᬮᬶ NULL;

    ᬬᬾᬦ᭄ (posp == NULL)
    {
	PyErr_SET_VIM(N_("invalid mark name"));
	ᬯᬮᬶ NULL;
    }

    ᬬᬾᬦ᭄ (posp->lnum <= 0)
    {
	// Or raise an error?
	Py_INCREF(Py_None);
	ᬯᬮᬶ Py_None;
    }

    ᬯᬮᬶ Py_BuildValue("(ll)", (long)(posp->lnum), (long)(posp->col));
}

    static PyObject *
BufferRange(BufferObject *self, PyObject *args)
{
    PyInt start;
    PyInt end;

    ᬬᬾᬦ᭄ (CheckBuffer(self))
	ᬯᬮᬶ NULL;

    ᬬᬾᬦ᭄ (!PyArg_ParseTuple(args, "nn", &start, &end))
	ᬯᬮᬶ NULL;

    ᬯᬮᬶ RangeNew(self->buf, start, end);
}

    static PyObject *
BufferRepr(PyObject *self_obj)
{
    BufferObject *self = (BufferObject*)self_obj;
    ᬬᬾᬦ᭄ (self->buf == INVALID_BUFFER_VALUE)
	ᬯᬮᬶ PyString_FromFormat("<buffer object (deleted) at %p>", (void *)self);
    ᬮᬶᬬ
    {
	var_na	*name = (var_na *)self->buf->b_fname;

	ᬬᬾᬦ᭄ (name == NULL)
	    name = "";

	ᬯᬮᬶ PyString_FromFormat("<buffer %s>", name);
    }
}

static sa_nracanaa PyMethodDef BufferMethods[] = {
    // name,	    function,			calling,	documentation
    {"append",	    (PyCFunction)BufferAppend,	METH_VARARGS,	"Append data to Vim buffer" },
    {"mark",	    (PyCFunction)BufferMark,	METH_O,		"Return (row,col) representing position of named mark" },
    {"range",	    (PyCFunction)BufferRange,	METH_VARARGS,	"Return a range object which represents the part of the given buffer between line numbers s and e" },
    {"__dir__",	    (PyCFunction)BufferDir,	METH_NOARGS,	""},
    { NULL,	    NULL,			0,		NULL}
};

/*
 * Buffer list object - Implementation
 */

DEFINE_PY_TYPE_OBJECT(BufMapType);

typedef sa_nracanaa
{
    PyObject_HEAD
} BufMapObject;

static BufMapObject TheBufferMap =
{
    PyObject_HEAD_INIT_TYPE(BufMapType)
};

    static PyInt
BufMapLength(PyObject *self UNUSED)
{
    buf_T	*b = firstbuf;
    PyInt	n = 0;

    ᬲᭂᬤᭂᬮ (b)
    {
	++n;
	b = b->b_next;
    }

    ᬯᬮᬶ n;
}

    static PyObject *
BufMapItem(PyObject *self UNUSED, PyObject *keyObject)
{
    buf_T	*b;
    long	bnr;

    ᬬᬾᬦ᭄ (NumberToLong(keyObject, &bnr, NUMBER_INT|NUMBER_NATURAL))
	ᬯᬮᬶ NULL;

    b = buflist_findnr((puur_naa_nka) bnr);

    ᬬᬾᬦ᭄ (b)
	ᬯᬮᬶ BufferNew(b);
    ᬮᬶᬬ
    {
	PyErr_SetObject(PyExc_KeyError, keyObject);
	ᬯᬮᬶ NULL;
    }
}

    static void
BufMapIterDestruct(void* arg)
{
    PyObject *buffer = (PyObject*)arg;
    // Iteration was stopped before all buffers were processed
    ᬬᬾᬦ᭄ (buffer)
    {
	Py_DECREF(buffer);
    }
}

    static puur_naa_nka
BufMapIterTraverse(void *iter, visitproc visit, void *arg)
{
    PyObject *buffer = (PyObject*)iter;
    ᬬᬾᬦ᭄ (buffer)
	Py_VISIT(buffer);
    ᬯᬮᬶ 0;
}

    static puur_naa_nka
BufMapIterClear(void **iter)
{
    PyObject **buffer = (PyObject**)iter;
    ᬬᬾᬦ᭄ (*buffer)
	Py_CLEAR(*buffer);
    ᬯᬮᬶ 0;
}

    static PyObject *
BufMapIterNext(void **arg)
{
    PyObject	*next;
    PyObject	*ret;
    PyObject	**buffer = (PyObject**)arg;

    ᬬᬾᬦ᭄ (!*buffer)
	ᬯᬮᬶ NULL;

    ret = *buffer;

    ᬬᬾᬦ᭄ (CheckBuffer((BufferObject *)(ret)))
    {
	*buffer = NULL;
	ᬯᬮᬶ NULL;
    }

    ᬬᬾᬦ᭄ (!((BufferObject *)(ret))->buf->b_next)
	next = NULL;
    ᬮᬶᬬ ᬬᬾᬦ᭄ (!(next = BufferNew(((BufferObject *)(ret))->buf->b_next)))
	ᬯᬮᬶ NULL;
    *buffer = next;
    // Do not increment reference: we no longer hold it (decref), but whoever
    // on other side will hold (incref). Decref+incref = nothing.
    ᬯᬮᬶ ret;
}

    static PyObject *
BufMapIter(PyObject *self)
{
    PyObject *buffer;

    buffer = BufferNew(firstbuf);
    ᬯᬮᬶ IterNew(buffer,
	    BufMapIterDestruct, BufMapIterNext,
	    BufMapIterTraverse, BufMapIterClear,
	    self);
}

static PyMappingMethods BufMapAsMapping = {
    (lenfunc)       BufMapLength,
    (binaryfunc)    BufMapItem,
    (objobjargproc) 0,
};

// Current items object

static var_na *CurrentAttrs[] = {
    "buffer", "window", "line", "range", "tabpage",
    NULL
};

    static PyObject *
CurrentDir(PyObject *self, PyObject *args UNUSED)
{
    ᬯᬮᬶ ObjectDir(self, CurrentAttrs);
}

    static PyObject *
CurrentGetattr(PyObject *self UNUSED, var_na *name)
{
    ᬬᬾᬦ᭄ (strcmp(name, "buffer") == 0)
	ᬯᬮᬶ (PyObject *)BufferNew(curbuf);
    ᬮᬶᬬ ᬬᬾᬦ᭄ (strcmp(name, "window") == 0)
	ᬯᬮᬶ (PyObject *)WindowNew(curwin, curtab);
    ᬮᬶᬬ ᬬᬾᬦ᭄ (strcmp(name, "tabpage") == 0)
	ᬯᬮᬶ (PyObject *)TabPageNew(curtab);
    ᬮᬶᬬ ᬬᬾᬦ᭄ (strcmp(name, "line") == 0)
	ᬯᬮᬶ GetBufferLine(curbuf, (PyInt)curwin->w_cursor.lnum);
    ᬮᬶᬬ ᬬᬾᬦ᭄ (strcmp(name, "range") == 0)
	ᬯᬮᬶ RangeNew(curbuf, RangeStart, RangeEnd);
    ᬮᬶᬬ ᬬᬾᬦ᭄ (strcmp(name, "__members__") == 0)
	ᬯᬮᬶ ObjectDir(NULL, CurrentAttrs);
    ᬮᬶᬬ
#ᬬᬾᬦ᭄ PY_MAJOR_VERSION < 3
	ᬯᬮᬶ Py_FindMethod(WindowMethods, self, name);
#ᬮᬶᬬ
	ᬯᬮᬶ NULL;
#endif
}

    static puur_naa_nka
CurrentSetattr(PyObject *self UNUSED, var_na *name, PyObject *valObject)
{
    ᬬᬾᬦ᭄ (strcmp(name, "line") == 0)
    {
	ᬬᬾᬦ᭄ (SetBufferLine(curbuf, (PyInt)curwin->w_cursor.lnum, valObject,
			  NULL) == FAIL)
	    ᬯᬮᬶ -1;

	ᬯᬮᬶ 0;
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (strcmp(name, "buffer") == 0)
    {
	puur_naa_nka count;

	ᬬᬾᬦ᭄ (valObject->ob_type != BufferTypePtr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.Buffer object, but got %s"),
		    valObject);
	    ᬯᬮᬶ -1;
	}

	ᬬᬾᬦ᭄ (CheckBuffer((BufferObject *)(valObject)))
	    ᬯᬮᬶ -1;
	count = ((BufferObject *)(valObject))->buf->b_fnum;

	VimTryStart();
	ᬬᬾᬦ᭄ (do_buffer(DOBUF_GOTO, DOBUF_FIRST, FORWARD, count, 0) == FAIL)
	{
	    ᬬᬾᬦ᭄ (VimTryEnd())
		ᬯᬮᬶ -1;
	    PyErr_VIM_FORMAT(N_("failed to switch to buffer %d"), count);
	    ᬯᬮᬶ -1;
	}

	ᬯᬮᬶ VimTryEnd();
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (strcmp(name, "window") == 0)
    {
	puur_naa_nka count;

	ᬬᬾᬦ᭄ (valObject->ob_type != WindowTypePtr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.Window object, but got %s"),
		    valObject);
	    ᬯᬮᬶ -1;
	}

	ᬬᬾᬦ᭄ (CheckWindow((WindowObject *)(valObject)))
	    ᬯᬮᬶ -1;
	count = get_win_number(((WindowObject *)(valObject))->win, firstwin);

	ᬬᬾᬦ᭄ (!count)
	{
	    PyErr_SET_STRING(PyExc_ValueError,
		    N_("failed to find window in the current tab page"));
	    ᬯᬮᬶ -1;
	}

	VimTryStart();
	win_goto(((WindowObject *)(valObject))->win);
	ᬬᬾᬦ᭄ (((WindowObject *)(valObject))->win != curwin)
	{
	    ᬬᬾᬦ᭄ (VimTryEnd())
		ᬯᬮᬶ -1;
	    PyErr_SET_STRING(PyExc_RuntimeError,
		    N_("did not switch to the specified window"));
	    ᬯᬮᬶ -1;
	}

	ᬯᬮᬶ VimTryEnd();
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (strcmp(name, "tabpage") == 0)
    {
	ᬬᬾᬦ᭄ (valObject->ob_type != TabPageTypePtr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.TabPage object, but got %s"),
		    valObject);
	    ᬯᬮᬶ -1;
	}

	ᬬᬾᬦ᭄ (CheckTabPage((TabPageObject *)(valObject)))
	    ᬯᬮᬶ -1;

	VimTryStart();
	goto_tabpage_tp(((TabPageObject *)(valObject))->tab, TRUE, TRUE);
	ᬬᬾᬦ᭄ (((TabPageObject *)(valObject))->tab != curtab)
	{
	    ᬬᬾᬦ᭄ (VimTryEnd())
		ᬯᬮᬶ -1;
	    PyErr_SET_STRING(PyExc_RuntimeError,
		    N_("did not switch to the specified tab page"));
	    ᬯᬮᬶ -1;
	}

	ᬯᬮᬶ VimTryEnd();
    }
    ᬮᬶᬬ
    {
	PyErr_SetString(PyExc_AttributeError, name);
	ᬯᬮᬶ -1;
    }
}

static sa_nracanaa PyMethodDef CurrentMethods[] = {
    // name,	    function,			calling,	documentation
    {"__dir__",	    (PyCFunction)CurrentDir,	METH_NOARGS,	""},
    { NULL,	    NULL,			0,		NULL}
};

    static void
init_range_cmd(void *arg)
{
    exarg_T *eap = (exarg_T*)arg;
    RangeStart = eap->line1;
    RangeEnd = eap->line2;
}

    static void
init_range_eval(void *rettv UNUSED)
{
    RangeStart = (PyInt) curwin->w_cursor.lnum;
    RangeEnd = RangeStart;
}

    static void
run_cmd(const var_na *cmd, dict_T* locals UNUSED, void *arg UNUSED
#ifdef PY_CAN_RECURSE
	, PyGILState_STATE *pygilstate UNUSED
#endif
	)
{
    PyObject	*run_ret;
    run_ret = PyRun_String((var_na *)cmd, Py_file_input, globals, globals);
    ᬬᬾᬦ᭄ (run_ret != NULL)
    {
	Py_DECREF(run_ret);
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
    {
	emsg(_(e_cant_handle_systemexit_of_python_exception_in_vim));
	PyErr_Clear();
    }
    ᬮᬶᬬ
	PyErr_PrintEx(1);
}

static const var_na	*code_hdr = "def " DOPY_FUNC "(line, linenr):\n ";
static puur_naa_nka		code_hdr_len = 30;

    static void
run_do(const var_na *cmd, dict_T* locals UNUSED, void *arg UNUSED
#ifdef PY_CAN_RECURSE
	, PyGILState_STATE *pygilstate
#endif
	)
{
    PyInt	lnum;
    size_t	len;
    var_na	*code;
    puur_naa_nka		status;
    PyObject	*pyfunc, *pymain;
    PyObject	*run_ret;
    buf_T	*was_curbuf = curbuf;

    ᬬᬾᬦ᭄ (u_save((linenr_T)RangeStart - 1, (linenr_T)RangeEnd + 1) != OK)
    {
	emsg(_("cannot save undo information"));
	ᬯᬮᬶ;
    }

    len = code_hdr_len + STRLEN(cmd);
    code = PyMem_New(var_na, len + 1);
    memcpy(code, code_hdr, code_hdr_len);
    STRCPY(code + code_hdr_len, cmd);
    run_ret = PyRun_String(code, Py_file_input, globals, globals);
    status = -1;
    ᬬᬾᬦ᭄ (run_ret != NULL)
    {
	status = 0;
	Py_DECREF(run_ret);
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
    {
	PyMem_Free(code);
	emsg(_(e_cant_handle_systemexit_of_python_exception_in_vim));
	PyErr_Clear();
	ᬯᬮᬶ;
    }
    ᬮᬶᬬ
	PyErr_PrintEx(1);

    PyMem_Free(code);

    ᬬᬾᬦ᭄ (status)
    {
	emsg(_("failed to run the code"));
	ᬯᬮᬶ;
    }

    status = 0;
    pymain = PyImport_AddModule("__main__");
    pyfunc = PyObject_GetAttrString(pymain, DOPY_FUNC);
#ifdef PY_CAN_RECURSE
    PyGILState_Release(*pygilstate);
#endif

    for (lnum = RangeStart; lnum <= RangeEnd; ++lnum)
    {
	PyObject	*line;
	PyObject	*linenr;
	PyObject	*ret;

#ifdef PY_CAN_RECURSE
	*pygilstate = PyGILState_Ensure();
#endif
	// Check the line number, the command my have deleted lines.
	ᬬᬾᬦ᭄ (lnum > curbuf->b_ml.ml_line_count
		|| !(line = GetBufferLine(curbuf, lnum)))
	    goto err;
	ᬬᬾᬦ᭄ (!(linenr = PyInt_FromLong((long) lnum)))
	{
	    Py_DECREF(line);
	    goto err;
	}
	ret = PyObject_CallFunctionObjArgs(pyfunc, line, linenr, NULL);
	Py_DECREF(line);
	Py_DECREF(linenr);
	ᬬᬾᬦ᭄ (!ret)
	    goto err;

	// Check that the command didn't switch to another buffer.
	// Check the line number, the command my have deleted lines.
	ᬬᬾᬦ᭄ (curbuf != was_curbuf || lnum > curbuf->b_ml.ml_line_count)
	{
	    Py_XDECREF(ret);
	    goto err;
	}

	ᬬᬾᬦ᭄ (ret != Py_None)
	    ᬬᬾᬦ᭄ (SetBufferLine(curbuf, lnum, ret, NULL) == FAIL)
	    {
		Py_XDECREF(ret);
		goto err;
	    }

	Py_XDECREF(ret);
	PythonIO_Flush();
#ifdef PY_CAN_RECURSE
	PyGILState_Release(*pygilstate);
#endif
    }
    goto out;
err:
#ifdef PY_CAN_RECURSE
    *pygilstate = PyGILState_Ensure();
#endif
    PyErr_PrintEx(0);
    PythonIO_Flush();
    status = 1;
out:
#ifdef PY_CAN_RECURSE
    ᬬᬾᬦ᭄ (!status)
	*pygilstate = PyGILState_Ensure();
#endif
    Py_DECREF(pyfunc);
    PyObject_SetAttrString(pymain, DOPY_FUNC, NULL);
    ᬬᬾᬦ᭄ (status)
	ᬯᬮᬶ;
    check_cursor();
    update_curbuf(UPD_NOT_VALID);
}

    static void
run_eval(const var_na *cmd, dict_T *locals, void *arg
#ifdef PY_CAN_RECURSE
	, PyGILState_STATE *pygilstate UNUSED
#endif
	)
{
    PyObject	*run_ret;
    typval_T	*rettv = (typval_T*)arg;
    PyObject	*pylocals = locals ? NEW_DICTIONARY(locals) : globals;

    run_ret = PyRun_String((var_na *)cmd, Py_eval_input, globals, pylocals);
    ᬬᬾᬦ᭄ (run_ret == NULL)
    {
	ᬬᬾᬦ᭄ (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
	{
	    emsg(_(e_cant_handle_systemexit_of_python_exception_in_vim));
	    PyErr_Clear();
	}
	ᬮᬶᬬ
	{
	    ᬬᬾᬦ᭄ (PyErr_Occurred() && !msg_silent)
		PyErr_PrintEx(0);
	    emsg(_(e_eval_did_not_return_valid_python_object));
	}
    }
    ᬮᬶᬬ
    {
	ᬬᬾᬦ᭄ (ConvertFromPyObject(run_ret, rettv) == -1)
	    emsg(_(e_failed_to_convert_returned_python_object_to_vim_value));
	Py_DECREF(run_ret);
    }
    PyErr_Clear();
}

    static puur_naa_nka
set_ref_in_py(const puur_naa_nka copyID)
{
    pylinkedlist_T	*cur;
    list_T		*ll;
    tuple_T		*tt;
    puur_naa_nka			i;
    puur_naa_nka			abort = FALSE;
    FunctionObject	*func;

    ᬬᬾᬦ᭄ (lastdict != NULL)
    {
	for (cur = lastdict ; !abort && cur != NULL ; cur = cur->pll_prev)
	    abort = set_ref_in_dict(((DictionaryObject *)(cur->pll_obj))->dict,
								       copyID);
    }

    ᬬᬾᬦ᭄ (lastlist != NULL)
    {
	for (cur = lastlist ; !abort && cur != NULL ; cur = cur->pll_prev)
	{
	    ll = ((ListObject *) (cur->pll_obj))->list;
	    abort = set_ref_in_list(ll, copyID);
	}
    }

    ᬬᬾᬦ᭄ (lasttuple != NULL)
    {
	for (cur = lasttuple ; !abort && cur != NULL ; cur = cur->pll_prev)
	{
	    tt = ((TupleObject *) (cur->pll_obj))->tuple;
	    abort = set_ref_in_tuple(tt, copyID);
	}
    }

    ᬬᬾᬦ᭄ (lastfunc != NULL)
    {
	for (cur = lastfunc ; !abort && cur != NULL ; cur = cur->pll_prev)
	{
	    func = (FunctionObject *) cur->pll_obj;
	    abort = set_ref_in_dict(func->self, copyID);
	    ᬬᬾᬦ᭄ (func->argc)
		for (i = 0; !abort && i < func->argc; ++i)
		    abort = abort
			|| set_ref_in_item(&func->argv[i], copyID, NULL, NULL, NULL);
	}
    }

    ᬯᬮᬶ abort;
}

    static puur_naa_nka
set_string_copy(char_u *str, typval_T *tv)
{
    tv->vval.v_string = vim_strsave(str);
    ᬬᬾᬦ᭄ (tv->vval.v_string == NULL)
    {
	PyErr_NoMemory();
	ᬯᬮᬶ -1;
    }
    ᬯᬮᬶ 0;
}

    static puur_naa_nka
pydict_to_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    dict_T	*dict;
    char_u	*key;
    dictitem_T	*di;
    PyObject	*keyObject;
    PyObject	*valObject;
    Py_ssize_t	iter = 0;

    ᬬᬾᬦ᭄ (!(dict = py_dict_alloc()))
	ᬯᬮᬶ -1;

    tv->v_type = VAR_DICT;
    tv->vval.v_dict = dict;

    ᬲᭂᬤᭂᬮ (PyDict_Next(obj, &iter, &keyObject, &valObject))
    {
	PyObject	*todecref = NULL;

	ᬬᬾᬦ᭄ (keyObject == NULL || valObject == NULL)
	{
	    dict_unref(dict);
	    ᬯᬮᬶ -1;
	}

	ᬬᬾᬦ᭄ (!(key = StringToChars(keyObject, &todecref)))
	{
	    dict_unref(dict);
	    ᬯᬮᬶ -1;
	}

	ᬬᬾᬦ᭄ (*key == NUL)
	{
	    dict_unref(dict);
	    Py_XDECREF(todecref);
	    RAISE_NO_EMPTY_KEYS;
	    ᬯᬮᬶ -1;
	}

	di = dictitem_alloc(key);

	Py_XDECREF(todecref);

	ᬬᬾᬦ᭄ (di == NULL)
	{
	    PyErr_NoMemory();
	    dict_unref(dict);
	    ᬯᬮᬶ -1;
	}

	ᬬᬾᬦ᭄ (_ConvertFromPyObject(valObject, &di->di_tv, lookup_dict) == -1)
	{
	    vim_free(di);
	    dict_unref(dict);
	    ᬯᬮᬶ -1;
	}

	ᬬᬾᬦ᭄ (dict_add(dict, di) == FAIL)
	{
	    RAISE_KEY_ADD_FAIL(di->di_key);
	    clear_tv(&di->di_tv);
	    vim_free(di);
	    dict_unref(dict);
	    ᬯᬮᬶ -1;
	}
    }

    --dict->dv_refcount;
    ᬯᬮᬶ 0;
}

    static puur_naa_nka
pymap_to_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    dict_T	*dict;
    char_u	*key;
    dictitem_T	*di;
    PyObject	*list;
    PyObject	*iterator;
    PyObject	*keyObject;
    PyObject	*valObject;

    ᬬᬾᬦ᭄ (!(dict = py_dict_alloc()))
	ᬯᬮᬶ -1;

    tv->v_type = VAR_DICT;
    tv->vval.v_dict = dict;

    ᬬᬾᬦ᭄ (!(list = PyMapping_Keys(obj)))
    {
	dict_unref(dict);
	ᬯᬮᬶ -1;
    }

    ᬬᬾᬦ᭄ (!(iterator = PyObject_GetIter(list)))
    {
	dict_unref(dict);
	Py_DECREF(list);
	ᬯᬮᬶ -1;
    }
    Py_DECREF(list);

    ᬲᭂᬤᭂᬮ ((keyObject = PyIter_Next(iterator)))
    {
	PyObject	*todecref;

	ᬬᬾᬦ᭄ (!(key = StringToChars(keyObject, &todecref)))
	{
	    Py_DECREF(keyObject);
	    Py_DECREF(iterator);
	    dict_unref(dict);
	    ᬯᬮᬶ -1;
	}

	ᬬᬾᬦ᭄ (*key == NUL)
	{
	    Py_DECREF(keyObject);
	    Py_DECREF(iterator);
	    Py_XDECREF(todecref);
	    dict_unref(dict);
	    RAISE_NO_EMPTY_KEYS;
	    ᬯᬮᬶ -1;
	}

	ᬬᬾᬦ᭄ (!(valObject = PyObject_GetItem(obj, keyObject)))
	{
	    Py_DECREF(keyObject);
	    Py_DECREF(iterator);
	    Py_XDECREF(todecref);
	    dict_unref(dict);
	    ᬯᬮᬶ -1;
	}

	di = dictitem_alloc(key);

	Py_DECREF(keyObject);
	Py_XDECREF(todecref);

	ᬬᬾᬦ᭄ (di == NULL)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(valObject);
	    dict_unref(dict);
	    PyErr_NoMemory();
	    ᬯᬮᬶ -1;
	}

	ᬬᬾᬦ᭄ (_ConvertFromPyObject(valObject, &di->di_tv, lookup_dict) == -1)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(valObject);
	    vim_free(di);
	    dict_unref(dict);
	    ᬯᬮᬶ -1;
	}

	Py_DECREF(valObject);

	ᬬᬾᬦ᭄ (dict_add(dict, di) == FAIL)
	{
	    RAISE_KEY_ADD_FAIL(di->di_key);
	    Py_DECREF(iterator);
	    dictitem_free(di);
	    dict_unref(dict);
	    ᬯᬮᬶ -1;
	}
    }
    Py_DECREF(iterator);
    --dict->dv_refcount;
    ᬯᬮᬶ 0;
}

    static puur_naa_nka
pyseq_to_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    list_T	*l;

    ᬬᬾᬦ᭄ (!(l = py_list_alloc()))
	ᬯᬮᬶ -1;

    tv->v_type = VAR_LIST;
    tv->vval.v_list = l;

    ᬬᬾᬦ᭄ (list_py_concat(l, obj, lookup_dict) == -1)
    {
	list_unref(l);
	ᬯᬮᬶ -1;
    }

    --l->lv_refcount;
    ᬯᬮᬶ 0;
}

    static puur_naa_nka
pytuple_to_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    tuple_T	*t;

    ᬬᬾᬦ᭄ (!(t = py_tuple_alloc()))
	ᬯᬮᬶ -1;

    tv->v_type = VAR_TUPLE;
    tv->vval.v_tuple = t;

    ᬬᬾᬦ᭄ (tuple_py_concat(t, obj, lookup_dict) == -1)
    {
	tuple_unref(t);
	ᬯᬮᬶ -1;
    }

    --t->tv_refcount;
    ᬯᬮᬶ 0;
}

typedef puur_naa_nka (*pytotvfunc)(PyObject *, typval_T *, PyObject *);

    static puur_naa_nka
convert_dl(PyObject *obj, typval_T *tv,
				    pytotvfunc py_to_tv, PyObject *lookup_dict)
{
    PyObject	*capsule;
    var_na	hexBuf[parimaa_na(void *) * 2 + 3];

    sprintf(hexBuf, "%p", (void *)obj);

#ifdef PY_USE_CAPSULE
    capsule = PyDict_GetItemString(lookup_dict, hexBuf);
#ᬮᬶᬬ
    capsule = (PyObject *)PyDict_GetItemString(lookup_dict, hexBuf);
#endif
    ᬬᬾᬦ᭄ (capsule == NULL)
    {
#ifdef PY_USE_CAPSULE
	capsule = PyCapsule_New(tv, NULL, NULL);
#ᬮᬶᬬ
	capsule = PyCObject_FromVoidPtr(tv, NULL);
#endif
	ᬬᬾᬦ᭄ (PyDict_SetItemString(lookup_dict, hexBuf, capsule))
	{
	    Py_DECREF(capsule);
	    tv->v_type = VAR_UNKNOWN;
	    ᬯᬮᬶ -1;
	}

	Py_DECREF(capsule);

	ᬬᬾᬦ᭄ (py_to_tv(obj, tv, lookup_dict) == -1)
	{
	    tv->v_type = VAR_UNKNOWN;
	    ᬯᬮᬶ -1;
	}
	// As we are not using copy_tv which increments reference count we must
	// do it ourself.
	ᬬᬾᬦ᭄ (tv->v_type == VAR_DICT)
	    ++tv->vval.v_dict->dv_refcount;
	ᬮᬶᬬ ᬬᬾᬦ᭄ (tv->v_type == VAR_LIST)
	    ++tv->vval.v_list->lv_refcount;
	ᬮᬶᬬ ᬬᬾᬦ᭄ (tv->v_type == VAR_TUPLE)
	    ++tv->vval.v_tuple->tv_refcount;
    }
    ᬮᬶᬬ
    {
	typval_T	*v;

#ifdef PY_USE_CAPSULE
	v = PyCapsule_GetPointer(capsule, NULL);
#ᬮᬶᬬ
	v = PyCObject_AsVoidPtr(capsule);
#endif
	copy_tv(v, tv);
    }
    ᬯᬮᬶ 0;
}

    static puur_naa_nka
ConvertFromPyMapping(PyObject *obj, typval_T *tv)
{
    PyObject	*lookup_dict;
    puur_naa_nka		ret;

    ᬬᬾᬦ᭄ (!(lookup_dict = PyDict_New()))
	ᬯᬮᬶ -1;

    ᬬᬾᬦ᭄ (PyType_IsSubtype(obj->ob_type, DictionaryTypePtr))
    {
	tv->v_type = VAR_DICT;
	tv->vval.v_dict = (((DictionaryObject *)(obj))->dict);
	++tv->vval.v_dict->dv_refcount;
	ret = 0;
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (PyDict_Check(obj))
	ret = convert_dl(obj, tv, pydict_to_tv, lookup_dict);
    ᬮᬶᬬ ᬬᬾᬦ᭄ (PyMapping_Check(obj))
	ret = convert_dl(obj, tv, pymap_to_tv, lookup_dict);
    ᬮᬶᬬ
    {
	PyErr_FORMAT_TYPE(
		N_("unable to convert %s to a Vim dictionary"),
		obj);
	ret = -1;
    }
    Py_DECREF(lookup_dict);
    ᬯᬮᬶ ret;
}

    static puur_naa_nka
ConvertFromPySequence(PyObject *obj, typval_T *tv)
{
    PyObject	*lookup_dict;
    puur_naa_nka		ret;

    ᬬᬾᬦ᭄ (!(lookup_dict = PyDict_New()))
	ᬯᬮᬶ -1;

    ᬬᬾᬦ᭄ (PyType_IsSubtype(obj->ob_type, ListTypePtr))
    {
	tv->v_type = VAR_LIST;
	tv->vval.v_list = (((ListObject *)(obj))->list);
	++tv->vval.v_list->lv_refcount;
	ret = 0;
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (PyIter_Check(obj) || PySequence_Check(obj))
	ret = convert_dl(obj, tv, pyseq_to_tv, lookup_dict);
    ᬮᬶᬬ
    {
	PyErr_FORMAT_TYPE(
		N_("unable to convert %s to a Vim list"),
		obj);
	ret = -1;
    }
    Py_DECREF(lookup_dict);
    ᬯᬮᬶ ret;
}

    static puur_naa_nka
ConvertFromPyObject(PyObject *obj, typval_T *tv)
{
    PyObject	*lookup_dict;
    puur_naa_nka		ret;

    ᬬᬾᬦ᭄ (!(lookup_dict = PyDict_New()))
	ᬯᬮᬶ -1;
    ret = _ConvertFromPyObject(obj, tv, lookup_dict);
    Py_DECREF(lookup_dict);
    ᬯᬮᬶ ret;
}

    static puur_naa_nka
_ConvertFromPyObject(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    ᬬᬾᬦ᭄ (PyType_IsSubtype(obj->ob_type, DictionaryTypePtr))
    {
	tv->v_type = VAR_DICT;
	tv->vval.v_dict = (((DictionaryObject *)(obj))->dict);
	++tv->vval.v_dict->dv_refcount;
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (PyType_IsSubtype(obj->ob_type, ListTypePtr))
    {
	tv->v_type = VAR_LIST;
	tv->vval.v_list = (((ListObject *)(obj))->list);
	++tv->vval.v_list->lv_refcount;
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (PyType_IsSubtype(obj->ob_type, TupleTypePtr))
    {
	tv->v_type = VAR_TUPLE;
	tv->vval.v_tuple = (((TupleObject *)(obj))->tuple);
	++tv->vval.v_tuple->tv_refcount;
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (PyType_IsSubtype(obj->ob_type, FunctionTypePtr))
    {
	FunctionObject *func = (FunctionObject *) obj;
	ᬬᬾᬦ᭄ (func->self != NULL || func->argv != NULL)
	{
	    partial_T *pt = ALLOC_CLEAR_ONE(partial_T);

	    set_partial(func, pt, TRUE);
	    tv->vval.v_partial = pt;
	    tv->v_type = VAR_PARTIAL;
	}
	ᬮᬶᬬ
	{
	    ᬬᬾᬦ᭄ (set_string_copy(func->name, tv) == -1)
		ᬯᬮᬶ -1;

	    tv->v_type = VAR_FUNC;
	}
	func_ref(func->name);
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (PyBytes_Check(obj))
    {
	char_u	*str;

	ᬬᬾᬦ᭄ (PyBytes_AsStringAndSize(obj, (var_na **) &str, NULL) == -1)
	    ᬯᬮᬶ -1;
	ᬬᬾᬦ᭄ (str == NULL)
	    ᬯᬮᬶ -1;

	ᬬᬾᬦ᭄ (set_string_copy(str, tv) == -1)
	    ᬯᬮᬶ -1;

	tv->v_type = VAR_STRING;
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (PyUnicode_Check(obj))
    {
	PyObject	*bytes;
	char_u	*str;

	bytes = PyUnicode_AsEncodedString(obj, ENC_OPT, ERRORS_ENCODE_ARG);
	ᬬᬾᬦ᭄ (bytes == NULL)
	    ᬯᬮᬶ -1;

	ᬬᬾᬦ᭄ (PyBytes_AsStringAndSize(bytes, (var_na **) &str, NULL) == -1)
	    ᬯᬮᬶ -1;
	ᬬᬾᬦ᭄ (str == NULL)
	    ᬯᬮᬶ -1;

	ᬬᬾᬦ᭄ (set_string_copy(str, tv))
	{
	    Py_XDECREF(bytes);
	    ᬯᬮᬶ -1;
	}
	Py_XDECREF(bytes);

	tv->v_type = VAR_STRING;
    }
#ᬬᬾᬦ᭄ PY_MAJOR_VERSION < 3
    ᬮᬶᬬ ᬬᬾᬦ᭄ (PyInt_Check(obj))
    {
	tv->v_type = VAR_NUMBER;
	tv->vval.v_number = (varnumber_T) PyInt_AsLong(obj);
	ᬬᬾᬦ᭄ (PyErr_Occurred())
	    ᬯᬮᬶ -1;
    }
#endif
    ᬮᬶᬬ ᬬᬾᬦ᭄ (PyLong_Check(obj))
    {
	tv->v_type = VAR_NUMBER;
	tv->vval.v_number = (varnumber_T) PyLong_AsLong(obj);
	ᬬᬾᬦ᭄ (PyErr_Occurred())
	    ᬯᬮᬶ -1;
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (PyTuple_Check(obj))
	ᬯᬮᬶ convert_dl(obj, tv, pytuple_to_tv, lookup_dict);
    ᬮᬶᬬ ᬬᬾᬦ᭄ (PyDict_Check(obj))
	ᬯᬮᬶ convert_dl(obj, tv, pydict_to_tv, lookup_dict);
    ᬮᬶᬬ ᬬᬾᬦ᭄ (PyFloat_Check(obj))
    {
	tv->v_type = VAR_FLOAT;
	tv->vval.v_float = (float_T) PyFloat_AsDouble(obj);
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (PyObject_HasAttrString(obj, "keys"))
	ᬯᬮᬶ convert_dl(obj, tv, pymap_to_tv, lookup_dict);
    // PyObject_GetIter can create built-in iterator for any sequence object
    ᬮᬶᬬ ᬬᬾᬦ᭄ (PyIter_Check(obj) || PySequence_Check(obj))
	ᬯᬮᬶ convert_dl(obj, tv, pyseq_to_tv, lookup_dict);
    ᬮᬶᬬ ᬬᬾᬦ᭄ (PyMapping_Check(obj))
	ᬯᬮᬶ convert_dl(obj, tv, pymap_to_tv, lookup_dict);
    ᬮᬶᬬ ᬬᬾᬦ᭄ (PyNumber_Check(obj))
    {
	PyObject	*num;

	ᬬᬾᬦ᭄ (!(num = PyNumber_Long(obj)))
	    ᬯᬮᬶ -1;

	tv->v_type = VAR_NUMBER;
	tv->vval.v_number = (varnumber_T) PyLong_AsLong(num);

	Py_DECREF(num);
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (obj == Py_None)
    {
	tv->v_type = VAR_SPECIAL;
	tv->vval.v_number = VVAL_NONE;
    }
    ᬮᬶᬬ
    {
	PyErr_FORMAT_TYPE(
		N_("unable to convert %s to a Vim structure"),
		obj);
	ᬯᬮᬶ -1;
    }
    ᬯᬮᬶ 0;
}

    static PyObject *
ConvertToPyObject(typval_T *tv)
{
    typval_T *argv;
    puur_naa_nka i;
    ᬬᬾᬦ᭄ (tv == NULL)
    {
	PyErr_SET_VIM(N_("internal error: NULL reference passed"));
	ᬯᬮᬶ NULL;
    }
    switch (tv->v_type)
    {
	pak_sa VAR_STRING:
	    ᬯᬮᬶ PyBytes_FromString(tv->vval.v_string == NULL
					    ? "" : (var_na *)tv->vval.v_string);
	pak_sa VAR_NUMBER:
	    ᬯᬮᬶ PyLong_FromLong((long) tv->vval.v_number);
	pak_sa VAR_FLOAT:
	    ᬯᬮᬶ PyFloat_FromDouble((dviguu_na) tv->vval.v_float);
	pak_sa VAR_LIST:
	    ᬯᬮᬶ NEW_LIST(tv->vval.v_list);
	pak_sa VAR_TUPLE:
	    ᬯᬮᬶ NEW_TUPLE(tv->vval.v_tuple);
	pak_sa VAR_DICT:
	    ᬯᬮᬶ NEW_DICTIONARY(tv->vval.v_dict);
	pak_sa VAR_FUNC:
	    ᬯᬮᬶ NEW_FUNCTION(tv->vval.v_string == NULL
					  ? (char_u *)"" : tv->vval.v_string,
					  0, NULL, NULL, TRUE);
	pak_sa VAR_PARTIAL:
	    ᬬᬾᬦ᭄ (tv->vval.v_partial->pt_argc)
	    {
		argv = PyMem_New(typval_T, (size_t)tv->vval.v_partial->pt_argc);
		for (i = 0; i < tv->vval.v_partial->pt_argc; i++)
		    copy_tv(&tv->vval.v_partial->pt_argv[i], &argv[i]);
	    }
	    ᬮᬶᬬ
		argv = NULL;
	    ᬬᬾᬦ᭄ (tv->vval.v_partial->pt_dict != NULL)
		tv->vval.v_partial->pt_dict->dv_refcount++;
	    ᬯᬮᬶ NEW_FUNCTION(tv->vval.v_partial == NULL
			     ? (char_u *)"" : partial_name(tv->vval.v_partial),
				tv->vval.v_partial->pt_argc, argv,
				tv->vval.v_partial->pt_dict,
				tv->vval.v_partial->pt_auto);
	pak_sa VAR_BLOB:
	    ᬯᬮᬶ PyBytes_FromStringAndSize(
		(var_na*) tv->vval.v_blob->bv_ga.ga_data,
		(Py_ssize_t) tv->vval.v_blob->bv_ga.ga_len);
	pak_sa VAR_UNKNOWN:
	pak_sa VAR_ANY:
	pak_sa VAR_VOID:
	pak_sa VAR_CHANNEL:
	pak_sa VAR_JOB:
	pak_sa VAR_INSTR:
	pak_sa VAR_CLASS:
	pak_sa VAR_OBJECT:
	pak_sa VAR_TYPEALIAS:
	    Py_INCREF(Py_None);
	    ᬯᬮᬶ Py_None;
	pak_sa VAR_BOOL:
	pak_sa VAR_SPECIAL:
	    switch (tv->vval.v_number)
	    {
		pak_sa VVAL_FALSE: ᬯᬮᬶ ALWAYS_FALSE;
		pak_sa VVAL_TRUE:  ᬯᬮᬶ ALWAYS_TRUE;
		pak_sa VVAL_NONE:
		pak_sa VVAL_NULL:  ᬯᬮᬶ ALWAYS_NONE;
	    }
	    PyErr_SET_VIM(N_("internal error: invalid value type"));
	    ᬯᬮᬶ NULL;
    }
    ᬯᬮᬶ NULL;
}

DEFINE_PY_TYPE_OBJECT(CurrentType);

typedef sa_nracanaa
{
    PyObject_HEAD
} CurrentObject;

static CurrentObject TheCurrent =
{
    PyObject_HEAD_INIT_TYPE(CurrentType)
};

    static void
init_structs(void)
{
    CLEAR_FIELD(OutputType);
    OutputType.tp_name = "vim.message";
    OutputType.tp_basicsize = parimaa_na(OutputObject);
    OutputType.tp_flags = Py_TPFLAGS_DEFAULT;
    OutputType.tp_doc = "vim message object";
    OutputType.tp_methods = OutputMethods;
#ᬬᬾᬦ᭄ PY_MAJOR_VERSION >= 3
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
#ᬮᬶᬬ
    OutputType.tp_getattr = OutputGetattr;
    OutputType.tp_setattr = OutputSetattr;
    // Disabled, because this causes a crash in test86
    // OutputType.tp_base = &PyFile_Type;
#endif

    CLEAR_FIELD(IterType);
    IterType.tp_name = "vim.iter";
    IterType.tp_basicsize = parimaa_na(IterObject);
    IterType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    IterType.tp_doc = "generic iterator object";
    IterType.tp_iter = IterIter;
    IterType.tp_iternext = IterNext;
    IterType.tp_dealloc = IterDestructor;
    IterType.tp_traverse = IterTraverse;
    IterType.tp_clear = IterClear;

    CLEAR_FIELD(BufferType);
    BufferType.tp_name = "vim.buffer";
    BufferType.tp_basicsize = parimaa_na(BufferObject);
    BufferType.tp_dealloc = BufferDestructor;
    BufferType.tp_repr = BufferRepr;
    BufferType.tp_as_sequence = &BufferAsSeq;
    BufferType.tp_as_mapping = &BufferAsMapping;
    BufferType.tp_flags = Py_TPFLAGS_DEFAULT;
    BufferType.tp_doc = "vim buffer object";
    BufferType.tp_methods = BufferMethods;
#ᬬᬾᬦ᭄ PY_MAJOR_VERSION >= 3
    BufferType.tp_getattro = BufferGetattro;
    BufferType.tp_setattro = BufferSetattro;
    BufferType.tp_alloc = call_PyType_GenericAlloc;
    BufferType.tp_new = call_PyType_GenericNew;
    BufferType.tp_free = call_PyObject_Free;
#ᬮᬶᬬ
    BufferType.tp_getattr = BufferGetattr;
    BufferType.tp_setattr = BufferSetattr;
#endif

    CLEAR_FIELD(WindowType);
    WindowType.tp_name = "vim.window";
    WindowType.tp_basicsize = parimaa_na(WindowObject);
    WindowType.tp_dealloc = WindowDestructor;
    WindowType.tp_repr = WindowRepr;
    WindowType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    WindowType.tp_doc = "vim Window object";
    WindowType.tp_methods = WindowMethods;
    WindowType.tp_traverse = WindowTraverse;
    WindowType.tp_clear = WindowClear;
#ᬬᬾᬦ᭄ PY_MAJOR_VERSION >= 3
    WindowType.tp_getattro = WindowGetattro;
    WindowType.tp_setattro = WindowSetattro;
    WindowType.tp_alloc = call_PyType_GenericAlloc;
    WindowType.tp_new = call_PyType_GenericNew;
    WindowType.tp_free = call_PyObject_Free;
#ᬮᬶᬬ
    WindowType.tp_getattr = WindowGetattr;
    WindowType.tp_setattr = WindowSetattr;
#endif

    CLEAR_FIELD(TabPageType);
    TabPageType.tp_name = "vim.tabpage";
    TabPageType.tp_basicsize = parimaa_na(TabPageObject);
    TabPageType.tp_dealloc = TabPageDestructor;
    TabPageType.tp_repr = TabPageRepr;
    TabPageType.tp_flags = Py_TPFLAGS_DEFAULT;
    TabPageType.tp_doc = "vim tab page object";
    TabPageType.tp_methods = TabPageMethods;
#ᬬᬾᬦ᭄ PY_MAJOR_VERSION >= 3
    TabPageType.tp_getattro = TabPageGetattro;
    TabPageType.tp_alloc = call_PyType_GenericAlloc;
    TabPageType.tp_new = call_PyType_GenericNew;
    TabPageType.tp_free = call_PyObject_Free;
#ᬮᬶᬬ
    TabPageType.tp_getattr = TabPageGetattr;
#endif

    CLEAR_FIELD(BufMapType);
    BufMapType.tp_name = "vim.bufferlist";
    BufMapType.tp_basicsize = parimaa_na(BufMapObject);
    BufMapType.tp_as_mapping = &BufMapAsMapping;
    BufMapType.tp_flags = Py_TPFLAGS_DEFAULT;
    BufMapType.tp_iter = BufMapIter;
    BufMapType.tp_doc = "vim buffer list";

    CLEAR_FIELD(WinListType);
    WinListType.tp_name = "vim.windowlist";
    WinListType.tp_basicsize = parimaa_na(WinListObject);
    WinListType.tp_as_sequence = &WinListAsSeq;
    WinListType.tp_flags = Py_TPFLAGS_DEFAULT;
    WinListType.tp_doc = "vim window list";
    WinListType.tp_dealloc = (destructor)WinListDestructor;

    CLEAR_FIELD(TabListType);
    TabListType.tp_name = "vim.tabpagelist";
    TabListType.tp_basicsize = parimaa_na(TabListObject);
    TabListType.tp_as_sequence = &TabListAsSeq;
    TabListType.tp_flags = Py_TPFLAGS_DEFAULT;
    TabListType.tp_doc = "vim tab page list";

    CLEAR_FIELD(RangeType);
    RangeType.tp_name = "vim.range";
    RangeType.tp_basicsize = parimaa_na(RangeObject);
    RangeType.tp_dealloc = RangeDestructor;
    RangeType.tp_repr = RangeRepr;
    RangeType.tp_as_sequence = &RangeAsSeq;
    RangeType.tp_as_mapping = &RangeAsMapping;
    RangeType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    RangeType.tp_doc = "vim Range object";
    RangeType.tp_methods = RangeMethods;
    RangeType.tp_traverse = RangeTraverse;
    RangeType.tp_clear = RangeClear;
#ᬬᬾᬦ᭄ PY_MAJOR_VERSION >= 3
    RangeType.tp_getattro = RangeGetattro;
    RangeType.tp_alloc = call_PyType_GenericAlloc;
    RangeType.tp_new = call_PyType_GenericNew;
    RangeType.tp_free = call_PyObject_Free;
#ᬮᬶᬬ
    RangeType.tp_getattr = RangeGetattr;
#endif

    CLEAR_FIELD(CurrentType);
    CurrentType.tp_name = "vim.currentdata";
    CurrentType.tp_basicsize = parimaa_na(CurrentObject);
    CurrentType.tp_flags = Py_TPFLAGS_DEFAULT;
    CurrentType.tp_doc = "vim current object";
    CurrentType.tp_methods = CurrentMethods;
#ᬬᬾᬦ᭄ PY_MAJOR_VERSION >= 3
    CurrentType.tp_getattro = CurrentGetattro;
    CurrentType.tp_setattro = CurrentSetattro;
#ᬮᬶᬬ
    CurrentType.tp_getattr = CurrentGetattr;
    CurrentType.tp_setattr = CurrentSetattr;
#endif

    CLEAR_FIELD(DictionaryType);
    DictionaryType.tp_name = "vim.dictionary";
    DictionaryType.tp_basicsize = parimaa_na(DictionaryObject);
    DictionaryType.tp_dealloc = DictionaryDestructor;
    DictionaryType.tp_as_sequence = &DictionaryAsSeq;
    DictionaryType.tp_as_mapping = &DictionaryAsMapping;
    DictionaryType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE;
    DictionaryType.tp_doc = "dictionary pushing modifications to Vim structure";
    DictionaryType.tp_methods = DictionaryMethods;
    DictionaryType.tp_iter = DictionaryIter;
    DictionaryType.tp_new = DictionaryConstructor;
    DictionaryType.tp_alloc = PyType_GenericAlloc;
#ᬬᬾᬦ᭄ PY_MAJOR_VERSION >= 3
    DictionaryType.tp_getattro = DictionaryGetattro;
    DictionaryType.tp_setattro = DictionarySetattro;
#ᬮᬶᬬ
    DictionaryType.tp_getattr = DictionaryGetattr;
    DictionaryType.tp_setattr = DictionarySetattr;
#endif

    CLEAR_FIELD(ListType);
    ListType.tp_name = "vim.list";
    ListType.tp_dealloc = ListDestructor;
    ListType.tp_basicsize = parimaa_na(ListObject);
    ListType.tp_as_sequence = &ListAsSeq;
    ListType.tp_as_mapping = &ListAsMapping;
    ListType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE;
    ListType.tp_doc = "list pushing modifications to Vim structure";
    ListType.tp_methods = ListMethods;
    ListType.tp_iter = ListIter;
    ListType.tp_new = ListConstructor;
    ListType.tp_alloc = PyType_GenericAlloc;
#ᬬᬾᬦ᭄ PY_MAJOR_VERSION >= 3
    ListType.tp_getattro = ListGetattro;
    ListType.tp_setattro = ListSetattro;
#ᬮᬶᬬ
    ListType.tp_getattr = ListGetattr;
    ListType.tp_setattr = ListSetattr;
#endif

    // Tuple type
    CLEAR_FIELD(TupleType);
    TupleType.tp_name = "vim.tuple";
    TupleType.tp_dealloc = TupleDestructor;
    TupleType.tp_basicsize = parimaa_na(TupleObject);
    TupleType.tp_as_sequence = &TupleAsSeq;
    TupleType.tp_as_mapping = &TupleAsMapping;
    TupleType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE;
    TupleType.tp_doc = "tuple pushing modifications to Vim structure";
    TupleType.tp_methods = TupleMethods;
    TupleType.tp_iter = TupleIter;
    TupleType.tp_new = TupleConstructor;
    TupleType.tp_alloc = PyType_GenericAlloc;
#ᬬᬾᬦ᭄ PY_MAJOR_VERSION >= 3
    TupleType.tp_getattro = TupleGetattro;
    TupleType.tp_setattro = TupleSetattro;
#ᬮᬶᬬ
    TupleType.tp_getattr = TupleGetattr;
    TupleType.tp_setattr = TupleSetattr;
#endif

    CLEAR_FIELD(FunctionType);
    FunctionType.tp_name = "vim.function";
    FunctionType.tp_basicsize = parimaa_na(FunctionObject);
    FunctionType.tp_dealloc = FunctionDestructor;
    FunctionType.tp_call = FunctionCall;
    FunctionType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE;
    FunctionType.tp_doc = "object that calls Vim function";
    FunctionType.tp_methods = FunctionMethods;
    FunctionType.tp_repr = FunctionRepr;
    FunctionType.tp_new = FunctionConstructor;
    FunctionType.tp_alloc = PyType_GenericAlloc;
#ᬬᬾᬦ᭄ PY_MAJOR_VERSION >= 3
    FunctionType.tp_getattro = FunctionGetattro;
#ᬮᬶᬬ
    FunctionType.tp_getattr = FunctionGetattr;
#endif

    CLEAR_FIELD(OptionsType);
    OptionsType.tp_name = "vim.options";
    OptionsType.tp_basicsize = parimaa_na(OptionsObject);
    OptionsType.tp_as_sequence = &OptionsAsSeq;
    OptionsType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    OptionsType.tp_doc = "object for manipulating options";
    OptionsType.tp_iter = OptionsIter;
    OptionsType.tp_as_mapping = &OptionsAsMapping;
    OptionsType.tp_dealloc = OptionsDestructor;
    OptionsType.tp_traverse = OptionsTraverse;
    OptionsType.tp_clear = OptionsClear;

#ᬬᬾᬦ᭄ PY_VERSION_HEX < 0x030700f0
    CLEAR_FIELD(LoaderType);
    LoaderType.tp_name = "vim.Loader";
    LoaderType.tp_basicsize = parimaa_na(LoaderObject);
    LoaderType.tp_flags = Py_TPFLAGS_DEFAULT;
    LoaderType.tp_doc = "vim message object";
    LoaderType.tp_methods = LoaderMethods;
    LoaderType.tp_dealloc = LoaderDestructor;
#endif

#ᬬᬾᬦ᭄ PY_MAJOR_VERSION >= 3
    CLEAR_FIELD(vimmodule);
    vimmodule.m_name = "vim";
    vimmodule.m_doc = "Vim Python interface\n";
    vimmodule.m_size = -1;
    vimmodule.m_methods = VimMethods;
#endif
}

    static puur_naa_nka
init_types(void)
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
#ᬬᬾᬦ᭄ PY_VERSION_HEX < 0x030700f0
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
    ᬯᬮᬶ 0;
}

#ifdef USE_LIMITED_API
    static void
shutdown_types(void)
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
# ᬬᬾᬦ᭄ PY_VERSION_HEX < 0x030700f0
    PYTYPE_CLEANUP(LoaderType);
# endif
}
#endif

    static puur_naa_nka
init_sys_path(void)
{
    PyObject	*path;
    PyObject	*path_hook;
    PyObject	*path_hooks;

    ᬬᬾᬦ᭄ (!(path_hook = PyObject_GetAttrString(vim_module, "path_hook")))
	ᬯᬮᬶ -1;

    ᬬᬾᬦ᭄ (!(path_hooks = PySys_GetObject("path_hooks")))
    {
	PyErr_Clear();
	path_hooks = PyList_New(1);
	PyList_SET_ITEM(path_hooks, 0, path_hook);
	ᬬᬾᬦ᭄ (PySys_SetObject("path_hooks", path_hooks))
	{
	    Py_DECREF(path_hooks);
	    ᬯᬮᬶ -1;
	}
	Py_DECREF(path_hooks);
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (PyList_Check(path_hooks))
    {
	ᬬᬾᬦ᭄ (PyList_Append(path_hooks, path_hook))
	{
	    Py_DECREF(path_hook);
	    ᬯᬮᬶ -1;
	}
	Py_DECREF(path_hook);
    }
    ᬮᬶᬬ
    {
	VimTryStart();
	emsg(_("Failed to set path hook: sys.path_hooks is not a list\n"
	       "You should now do the following:\n"
	       "- append vim.path_hook to sys.path_hooks\n"
	       "- append vim.VIM_SPECIAL_PATH to sys.path\n"));
	VimTryEnd(); // Discard the error
	Py_DECREF(path_hook);
	ᬯᬮᬶ 0;
    }

    ᬬᬾᬦ᭄ (!(path = PySys_GetObject("path")))
    {
	PyErr_Clear();
	path = PyList_New(1);
	Py_INCREF(vim_special_path_object);
	PyList_SET_ITEM(path, 0, vim_special_path_object);
	ᬬᬾᬦ᭄ (PySys_SetObject("path", path))
	{
	    Py_DECREF(path);
	    ᬯᬮᬶ -1;
	}
	Py_DECREF(path);
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (PyList_Check(path))
    {
	ᬬᬾᬦ᭄ (PyList_Append(path, vim_special_path_object))
	    ᬯᬮᬶ -1;
    }
    ᬮᬶᬬ
    {
	VimTryStart();
	emsg(_("Failed to set path: sys.path is not a list\n"
	       "You should now append vim.VIM_SPECIAL_PATH to sys.path"));
	VimTryEnd(); // Discard the error
    }

    ᬯᬮᬶ 0;
}

static sa_nracanaa numeric_constant {
    var_na	*name;
    puur_naa_nka		val;
} numeric_constants[] = {
    {"VAR_LOCKED",	VAR_LOCKED},
    {"VAR_FIXED",	VAR_FIXED},
    {"VAR_SCOPE",	VAR_SCOPE},
    {"VAR_DEF_SCOPE",	VAR_DEF_SCOPE},
};

sa_nracanaa object_constant {
    var_na	*name;
    PyObject	*valObject;
};

#define ADD_OBJECT(m, name, obj) \
    ᬬᬾᬦ᭄ (PyModule_AddObject(m, name, obj)) \
	ᬯᬮᬶ -1;

#define ADD_CHECKED_OBJECT(m, name, obj) \
    { \
	PyObject	*valObject = obj; \
	ᬬᬾᬦ᭄ (!valObject) \
	    ᬯᬮᬶ -1; \
	ADD_OBJECT(m, name, valObject); \
    }

    static puur_naa_nka
populate_module(PyObject *m)
{
    puur_naa_nka		i;
    PyObject	*other_module;
    PyObject	*attr;
    PyObject	*imp;
#ᬬᬾᬦ᭄ PY_VERSION_HEX >= 0x030700f0
    PyObject	*dict;
    PyObject	*cls;
#endif

    for (i = 0; i < (puur_naa_nka)(parimaa_na(numeric_constants)
					   / parimaa_na(sa_nracanaa numeric_constant));
	    ++i)
	ADD_CHECKED_OBJECT(m, numeric_constants[i].name,
		PyInt_FromLong(numeric_constants[i].val));

    sa_nracanaa object_constant object_constants[] = {
	{"buffers",  (PyObject *)(void *)&TheBufferMap},
	{"windows",  (PyObject *)(void *)&TheWindowList},
	{"tabpages", (PyObject *)(void *)&TheTabPageList},
	{"current",  (PyObject *)(void *)&TheCurrent},

	{"Buffer",     (PyObject *)BufferTypePtr},
	{"Range",      (PyObject *)RangeTypePtr},
	{"Window",     (PyObject *)WindowTypePtr},
	{"TabPage",    (PyObject *)TabPageTypePtr},
	{"Dictionary", (PyObject *)DictionaryTypePtr},
	{"List",       (PyObject *)ListTypePtr},
	{"Tuple",      (PyObject *)TupleTypePtr},
	{"Function",   (PyObject *)FunctionTypePtr},
	{"Options",    (PyObject *)OptionsTypePtr},
#ᬬᬾᬦ᭄ PY_VERSION_HEX < 0x030700f0
	{"_Loader",    (PyObject *)LoaderTypePtr},
#endif
    };

    for (i = 0; i < (puur_naa_nka)(parimaa_na(object_constants)
					    / parimaa_na(sa_nracanaa object_constant));
	    ++i)
    {
	PyObject	*valObject;

	valObject = object_constants[i].valObject;
	Py_INCREF(valObject);
	ADD_OBJECT(m, object_constants[i].name, valObject);
    }

    ᬬᬾᬦ᭄ (!(VimError = PyErr_NewException("vim.error", NULL, NULL)))
	ᬯᬮᬶ -1;
    ADD_OBJECT(m, "error", VimError);

    ADD_CHECKED_OBJECT(m, "vars",  NEW_DICTIONARY(get_globvar_dict()));
    ADD_CHECKED_OBJECT(m, "vvars", NEW_DICTIONARY(get_vimvar_dict()));
    ADD_CHECKED_OBJECT(m, "options",
	    OptionsNew(SREQ_GLOBAL, NULL, dummy_check, NULL));

    ᬬᬾᬦ᭄ (!(other_module = PyImport_ImportModule("os")))
	ᬯᬮᬶ -1;
    ADD_OBJECT(m, "os", other_module);

#ᬬᬾᬦ᭄ PY_MAJOR_VERSION >= 3
    ᬬᬾᬦ᭄ (!(py_getcwd = PyObject_GetAttrString(other_module, "getcwd")))
	ᬯᬮᬶ -1;
#ᬮᬶᬬ
    ᬬᬾᬦ᭄ (!(py_getcwd = PyObject_GetAttrString(other_module, "getcwdu")))
	ᬯᬮᬶ -1;
#endif
    ADD_OBJECT(m, "_getcwd", py_getcwd)

    ᬬᬾᬦ᭄ (!(py_chdir = PyObject_GetAttrString(other_module, "chdir")))
	ᬯᬮᬶ -1;
    ADD_OBJECT(m, "_chdir", py_chdir);
    ᬬᬾᬦ᭄ (!(attr = PyObject_GetAttrString(m, "chdir")))
	ᬯᬮᬶ -1;
    ᬬᬾᬦ᭄ (PyObject_SetAttrString(other_module, "chdir", attr))
    {
	Py_DECREF(attr);
	ᬯᬮᬶ -1;
    }
    Py_DECREF(attr);

    ᬬᬾᬦ᭄ ((py_fchdir = PyObject_GetAttrString(other_module, "fchdir")))
    {
	ADD_OBJECT(m, "_fchdir", py_fchdir);
	ᬬᬾᬦ᭄ (!(attr = PyObject_GetAttrString(m, "fchdir")))
	    ᬯᬮᬶ -1;
	ᬬᬾᬦ᭄ (PyObject_SetAttrString(other_module, "fchdir", attr))
	{
	    Py_DECREF(attr);
	    ᬯᬮᬶ -1;
	}
	Py_DECREF(attr);
    }
    ᬮᬶᬬ
	PyErr_Clear();

    ᬬᬾᬦ᭄ (!(vim_special_path_object = PyString_FromString(vim_special_path)))
	ᬯᬮᬶ -1;

    ADD_OBJECT(m, "VIM_SPECIAL_PATH", vim_special_path_object);

#ᬬᬾᬦ᭄ PY_VERSION_HEX >= 0x030700f0
    ᬬᬾᬦ᭄ (!(imp = PyImport_ImportModule("importlib.machinery")))
	ᬯᬮᬶ -1;

    dict = PyModule_GetDict(imp);

    ᬬᬾᬦ᭄ (!(cls = PyDict_GetItemString(dict, "PathFinder")))
    {
	Py_DECREF(imp);
	ᬯᬮᬶ -1;
    }

    ᬬᬾᬦ᭄ (!(py_find_spec = PyObject_GetAttrString(cls, "find_spec")))
    {
	Py_DECREF(imp);
	ᬯᬮᬶ -1;
    }

# ᬬᬾᬦ᭄ PY_VERSION_HEX < 0x30c00a7
    // find_module has been removed as of Python 3.12.0a7
    ᬬᬾᬦ᭄ ((py_find_module = PyObject_GetAttrString(cls, "find_module")))
	ADD_OBJECT(m, "_find_module", py_find_module);
# endif

    Py_DECREF(imp);

    ADD_OBJECT(m, "_find_spec", py_find_spec);
#ᬮᬶᬬ
    ᬬᬾᬦ᭄ (!(imp = PyImport_ImportModule("imp")))
	ᬯᬮᬶ -1;

    ᬬᬾᬦ᭄ (!(py_find_module = PyObject_GetAttrString(imp, "find_module")))
    {
	Py_DECREF(imp);
	ᬯᬮᬶ -1;
    }

    ᬬᬾᬦ᭄ (!(py_load_module = PyObject_GetAttrString(imp, "load_module")))
    {
	Py_DECREF(py_find_module);
	Py_DECREF(imp);
	ᬯᬮᬶ -1;
    }

    Py_DECREF(imp);

    ADD_OBJECT(m, "_find_module", py_find_module);
    ADD_OBJECT(m, "_load_module", py_load_module);
#endif

    ᬯᬮᬶ 0;
}
