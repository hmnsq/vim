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

#ប្រសិនបើ PY_VERSION_HEX < 0x02050000
typedef ចំនួនគត់ Py_ssize_t;  // Python 2.4 and earlier don't have this type.
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
    ប្រសិនបើ (obj == NULL)
	ត្រឡប់ PyErr_NoMemory();
    ត្រឡប់ PyObject_Init(obj, type);
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
	ប្រសិនបើ (qualname == NULL) \
	{ \
	    PyErr_FORMAT(PyExc_TypeError, msg, "(NULL)"); \
	    bha_nga; \
	} \
	PyObject* module = PyObject_GetAttrString((PyObject*)(obj)->ob_type, "__module__"); \
	PyObject* full; \
	ប្រសិនបើ (module == NULL || PyUnicode_CompareWithASCIIString(module, "builtins") == 0 \
		|| PyUnicode_CompareWithASCIIString(module, "__main__") == 0) \
	{ \
	    full = qualname; \
	    Py_INCREF(full); \
	} \
	បើមិនដូច្នេះ \
	    full = PyUnicode_FromFormat("%U.%U", module, qualname); \
	PyObject* full_bytes = PyUnicode_AsUTF8String(full); \
	const var_na* full_str = PyBytes_AsString(full_bytes); \
	full_str = full_str == NULL ? "(NULL)" : full_str; \
	PyErr_FORMAT(PyExc_TypeError, msg, full_str); \
	Py_DECREF(qualname); \
	Py_XDECREF(module); \
	Py_XDECREF(full); \
	Py_XDECREF(full_bytes); \
    } ខណៈពេល (0)

# define PyList_GET_ITEM(list, i) PyList_GetItem(list, i)
# define PyList_GET_SIZE(o) PyList_Size(o)

// PyList_SET_ITEM and PyList_SetItem have slightly different behaviors. The
// former will leave the old item dangling, and the latter will decref on it.
// Since we only use this on new lists, this difference doesn't matter.
# define PyList_SET_ITEM(list, i, item) PyList_SetItem(list, i, item)

# define PyTuple_GET_ITEM(o, pos) PyTuple_GetItem(o, pos)
# define PyTuple_GET_SIZE(o) PyTuple_Size(o)

# ប្រសិនបើ Py_LIMITED_API < 0x03080000
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
    ប្រសិនបើ (slot_i >= 40) ត្រឡប់ NULL; /* this should never happen */ \
    ប្រសិនបើ (type_object->slot_name != NULL) \
    { \
	slots[slot_i].slot = Py_##slot_name; \
	slots[slot_i].pfunc = (void*)type_object->slot_name; \
	++slot_i; \
    }
# define ADD_TP_SUB_SLOT(sub_slot, slot_name) \
    ប្រសិនបើ (slot_i >= 40) ត្រឡប់ NULL; /* this should never happen */ \
    ប្រសិនបើ (type_object->sub_slot != NULL && type_object->sub_slot->slot_name != NULL) \
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
    ត្រឡប់ (PyTypeObject*)newtype;
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
    ប្រសិនបើ (type##Ptr == NULL) \
	ត្រឡប់ -1;
# define PYTYPE_CLEANUP(type) \
    type##Ptr = NULL;

// Limited API does not provide PyRun_* functions. Need to implement manually
// using PyCompile and PyEval.
static PyObject* Vim_PyRun_String(const var_na *str, ចំនួនគត់ start, PyObject *globals, PyObject *locals)
{
    // Just pass "" for filename for now.
    PyObject* compiled = Py_CompileString(str, "", start);
    ប្រសិនបើ (compiled == NULL)
	ត្រឡប់ NULL;

    PyObject* eval_result = PyEval_EvalCode(compiled, globals, locals);
    Py_DECREF(compiled);
    ត្រឡប់ eval_result;
}
static ចំនួនគត់ Vim_PyRun_SimpleString(const var_na *str)
{
    // This function emulates CPython's implementation.
    PyObject* m = PyImport_AddModule("__main__");
    ប្រសិនបើ (m == NULL)
	ត្រឡប់ -1;
    PyObject* d = PyModule_GetDict(m);
    PyObject* output = Vim_PyRun_String(str, Py_file_input, d, d);
    ប្រសិនបើ (output == NULL)
    {
	PyErr_PrintEx(TRUE);
	ត្រឡប់ -1;
    }
    Py_DECREF(output);
    ត្រឡប់ 0;
}
# define PyRun_String Vim_PyRun_String
# define PyRun_SimpleString Vim_PyRun_SimpleString

#បើមិនដូច្នេះ // !defined(USE_LIMITED_API)

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
    ប្រសិនបើ (PyType_Ready(type##Ptr)) \
	ត្រឡប់ -1;

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

static ចំនួនគត់ ConvertFromPyObject(PyObject *, typval_T *);
static ចំនួនគត់ _ConvertFromPyObject(PyObject *, typval_T *, PyObject *);
static ចំនួនគត់ ConvertFromPyMapping(PyObject *, typval_T *);
static ចំនួនគត់ ConvertFromPySequence(PyObject *, typval_T *);
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

#ប្រសិនបើ PY_VERSION_HEX >= 0x030700f0
static PyObject *py_find_spec;
#បើមិនដូច្នេះ
static PyObject *py_load_module;
#endif
#ប្រសិនបើ PY_VERSION_HEX < 0x30c00a7
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

    ប្រសិនបើ (PyBytes_Check(obj))
    {

	ប្រសិនបើ (PyBytes_AsStringAndSize(obj, (var_na **) &str, NULL) == -1
		|| str == NULL)
	    ត្រឡប់ NULL;

	*todecref = NULL;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (PyUnicode_Check(obj))
    {
	PyObject	*bytes;

	ប្រសិនបើ (!(bytes = PyUnicode_AsEncodedString(obj, ENC_OPT,
							   ERRORS_ENCODE_ARG)))
	    ត្រឡប់ NULL;

	ប្រសិនបើ (PyBytes_AsStringAndSize(bytes, (var_na **) &str, NULL) == -1
		|| str == NULL)
	{
	    Py_DECREF(bytes);
	    ត្រឡប់ NULL;
	}

	*todecref = bytes;
    }
    បើមិនដូច្នេះ
    {
#ប្រសិនបើ PY_MAJOR_VERSION < 3
	PyErr_FORMAT_TYPE(N_("expected str() or unicode() instance, but got %s"),
		obj);
#បើមិនដូច្នេះ
	PyErr_FORMAT_TYPE(N_("expected bytes() or str() instance, but got %s"),
		obj);
#endif
	ត្រឡប់ NULL;
    }

    ត្រឡប់ (char_u *) str;
}

#define NUMBER_LONG     1
#define NUMBER_INT      2
#define NUMBER_NATURAL  4
#define NUMBER_UNSIGNED 8

    static ចំនួនគត់
NumberToLong(PyObject *obj, long *result, ចំនួនគត់ flags)
{
#ប្រសិនបើ PY_MAJOR_VERSION < 3
    ប្រសិនបើ (PyInt_Check(obj))
    {
	*result = PyInt_AsLong(obj);
	ប្រសិនបើ (PyErr_Occurred())
	    ត្រឡប់ -1;
    }
    បើមិនដូច្នេះ
#endif
    ប្រសិនបើ (PyLong_Check(obj))
    {
	*result = PyLong_AsLong(obj);
	ប្រសិនបើ (PyErr_Occurred())
	    ត្រឡប់ -1;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (PyNumber_Check(obj))
    {
	PyObject	*num;

	ប្រសិនបើ (!(num = PyNumber_Long(obj)))
	    ត្រឡប់ -1;

	*result = PyLong_AsLong(num);

	Py_DECREF(num);

	ប្រសិនបើ (PyErr_Occurred())
	    ត្រឡប់ -1;
    }
    បើមិនដូច្នេះ
    {
#ប្រសិនបើ PY_MAJOR_VERSION < 3
	PyErr_FORMAT_TYPE(
		N_("expected int(), long() or something supporting "
		   "coercing to long(), but got %s"),
		obj);
#បើមិនដូច្នេះ
	PyErr_FORMAT_TYPE(
		N_("expected int() or something supporting coercing to int(), "
		   "but got %s"),
		obj);
#endif
	ត្រឡប់ -1;
    }

    ប្រសិនបើ (flags & NUMBER_INT)
    {
	ប្រសិនបើ (*result > INT_MAX)
	{
	    PyErr_SET_STRING(PyExc_OverflowError,
		    N_("value is too large to fit into C int type"));
	    ត្រឡប់ -1;
	}
	បើមិនដូច្នេះ ប្រសិនបើ (*result < INT_MIN)
	{
	    PyErr_SET_STRING(PyExc_OverflowError,
		    N_("value is too small to fit into C int type"));
	    ត្រឡប់ -1;
	}
    }

    ប្រសិនបើ (flags & NUMBER_NATURAL)
    {
	ប្រសិនបើ (*result <= 0)
	{
	    PyErr_SET_STRING(PyExc_ValueError,
		    N_("number must be greater than zero"));
	    ត្រឡប់ -1;
	}
    }
    បើមិនដូច្នេះ ប្រសិនបើ (flags & NUMBER_UNSIGNED)
    {
	ប្រសិនបើ (*result < 0)
	{
	    PyErr_SET_STRING(PyExc_ValueError,
		    N_("number must be greater or equal to zero"));
	    ត្រឡប់ -1;
	}
    }

    ត្រឡប់ 0;
}

    static ចំនួនគត់
add_string(PyObject *list, var_na *s)
{
    PyObject	*string;

    ប្រសិនបើ (!(string = PyString_FromString(s)))
	ត្រឡប់ -1;

    ប្រសិនបើ (PyList_Append(list, string))
    {
	Py_DECREF(string);
	ត្រឡប់ -1;
    }

    Py_DECREF(string);
    ត្រឡប់ 0;
}

    static PyObject *
ObjectDir(PyObject *self, var_na **attributes)
{
    PyMethodDef	*method;
    var_na	**attr;
    PyObject	*ret;

    ប្រសិនបើ (!(ret = PyList_New(0)))
	ត្រឡប់ NULL;

    ប្រសិនបើ (self)
	សម្រាប់ (method = Py_TYPE_GET_TP_METHODS(self->ob_type) ; method->ml_name != NULL ; ++method)
	    ប្រសិនបើ (add_string(ret, (var_na *)method->ml_name))
	    {
		Py_DECREF(ret);
		ត្រឡប់ NULL;
	    }

    សម្រាប់ (attr = attributes ; *attr ; ++attr)
	ប្រសិនបើ (add_string(ret, *attr))
	{
	    Py_DECREF(ret);
	    ត្រឡប់ NULL;
	}

#ប្រសិនបើ PY_MAJOR_VERSION < 3
    ប្រសិនបើ (add_string(ret, "__members__"))
    {
	Py_DECREF(ret);
	ត្រឡប់ NULL;
    }
#endif

    ត្រឡប់ ret;
}

// Output buffer management

// Function to write a line, points to either msg() or emsg().
typedef ចំនួនគត់ (*writefn)(var_na *);

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
    ត្រឡប់ ObjectDir(self, OutputAttrs);
}

    static ចំនួនគត់
OutputSetattr(PyObject *self_obj, var_na *name, PyObject *valObject)
{
    OutputObject *self = (OutputObject*)self_obj;
    ប្រសិនបើ (valObject == NULL)
    {
	PyErr_SET_STRING(PyExc_AttributeError,
		N_("can't delete OutputObject attributes"));
	ត្រឡប់ -1;
    }

    ប្រសិនបើ (strcmp(name, "softspace") == 0)
    {
	ប្រសិនបើ (NumberToLong(valObject, &(self->softspace), NUMBER_UNSIGNED))
	    ត្រឡប់ -1;
	ត្រឡប់ 0;
    }

    PyErr_FORMAT(PyExc_AttributeError, N_("invalid attribute: %s"), name);
    ត្រឡប់ -1;
}

// Buffer IO, we write one whole line at a time.
static garray_T io_ga = {0, 0, 1, 80, NULL};
static writefn old_fn = NULL;

    static void
PythonIO_Flush(void)
{
    ប្រសិនបើ (old_fn != NULL && io_ga.ga_len > 0)
    {
	((var_na *)io_ga.ga_data)[io_ga.ga_len] = NUL;
	// We don't know what emsg_severe should be here, so ... hope?
	old_fn((var_na *)io_ga.ga_data);
    }
    io_ga.ga_len = 0;
}

    static void
writer(writefn fn, char_u *str, PyInt n, ចំនួនគត់ severe)
{
    char_u *ptr;

    // Flush when switching output function.
    ប្រសិនបើ (fn != old_fn)
	PythonIO_Flush();
    old_fn = fn;

    // Write each NL separated line.  Text after the last NL is kept for
    // writing later.
    // For normal messages: Do not output when "got_int" was set.  This avoids
    // a loop gone crazy flooding the terminal with messages.  Also for when
    // "q" is pressed at the more-prompt.
    ខណៈពេល (n > 0 && (ptr = memchr(str, '\n', n)) != NULL
					  && (fn == (writefn)emsg || !got_int))
    {
	PyInt len = ptr - str;

	ប្រសិនបើ (ga_grow(&io_ga, (ចំនួនគត់)(len + 1)) == FAIL)
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
    ប្រសិនបើ (n > 0 && (fn == (writefn)emsg || !got_int)
					&& ga_grow(&io_ga, (ចំនួនគត់)(n + 1)) == OK)
    {
	mch_memmove(((var_na *)io_ga.ga_data) + io_ga.ga_len, str, (size_t)n);
	io_ga.ga_len += (ចំនួនគត់)n;
    }
}

    static ចំនួនគត់
write_output(OutputObject *self, PyObject *string)
{
    Py_ssize_t	len = 0;
    var_na	*str = NULL;
    ចំនួនគត់		error = self->error;

    ប្រសិនបើ (!PyArg_Parse(string, "et#", ENC_OPT, &str, &len))
	ត្រឡប់ -1;

    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    writer((writefn)(error ? emsg : msg), (char_u *)str, len, error);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS
    PyMem_Free(str);

    ត្រឡប់ 0;
}

    static PyObject *
OutputWrite(OutputObject *self, PyObject *string)
{
    ប្រសិនបើ (write_output(self, string))
	ត្រឡប់ NULL;

    Py_INCREF(Py_None);
    ត្រឡប់ Py_None;
}

    static PyObject *
OutputWritelines(OutputObject *self, PyObject *seq)
{
    PyObject	*iterator;
    PyObject	*item;

    ប្រសិនបើ (!(iterator = PyObject_GetIter(seq)))
	ត្រឡប់ NULL;

    ខណៈពេល ((item = PyIter_Next(iterator)))
    {
	ប្រសិនបើ (write_output(self, item))
	{
	    Py_DECREF(iterator);
	    Py_DECREF(item);
	    ត្រឡប់ NULL;
	}
	Py_DECREF(item);
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    ប្រសិនបើ (PyErr_Occurred())
	ត្រឡប់ NULL;

    Py_INCREF(Py_None);
    ត្រឡប់ Py_None;
}

    static PyObject *
AlwaysNone(PyObject *self UNUSED, PyObject *args UNUSED)
{
    // do nothing
    Py_INCREF(Py_None);
    ត្រឡប់ Py_None;
}
#define ALWAYS_NONE AlwaysNone(NULL, NULL)

    static PyObject *
AlwaysFalse(PyObject *self UNUSED, PyObject *args UNUSED)
{
    // do nothing
    PyObject	*ret = Py_False;
    Py_INCREF(ret);
    ត្រឡប់ ret;
}
#define ALWAYS_FALSE AlwaysFalse(NULL, NULL)

    static PyObject *
AlwaysTrue(PyObject *self UNUSED, PyObject *args UNUSED)
{
    // do nothing
    PyObject	*ret = Py_True;
    Py_INCREF(ret);
    ត្រឡប់ ret;
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

    static ចំនួនគត់
PythonIO_Init_io(void)
{
    ប្រសិនបើ (PySys_SetObject("stdout", (PyObject *)(void *)&Output))
	ត្រឡប់ -1;
    ប្រសិនបើ (PySys_SetObject("stderr", (PyObject *)(void *)&Error))
	ត្រឡប់ -1;

    ប្រសិនបើ (PyErr_Occurred())
    {
	emsg(_(e_python_error_initialising_io_object));
	ត្រឡប់ -1;
    }

    ត្រឡប់ 0;
}

#ប្រសិនបើ PY_VERSION_HEX < 0x030700f0
static PyObject *call_load_module(var_na *name, ចំនួនគត់ len, PyObject *find_module_result);

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

    ប្រសិនបើ (!fullname)
    {
	module = result ? result : Py_None;
	Py_INCREF(module);
	ត្រឡប់ module;
    }

    module = call_load_module(fullname, (ចំនួនគត់)STRLEN(fullname), result);

    self->fullname = NULL;
    self->result = module;

    vim_free(fullname);
    Py_DECREF(result);

    ប្រសិនបើ (!module)
    {
	ប្រសិនបើ (PyErr_Occurred())
	    ត្រឡប់ NULL;

	Py_INCREF(Py_None);
	ត្រឡប់ Py_None;
    }

    Py_INCREF(module);
    ត្រឡប់ module;
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

    static ចំនួនគត់
VimTryEnd(void)
{
    --trylevel;
    // Without this it stops processing all subsequent Vim script commands and
    // generates strange error messages if I e.g. try calling Test() in a cycle
    did_emsg = FALSE;
    // Keyboard interrupt should be preferred over anything else
    ប្រសិនបើ (got_int)
    {
	ប្រសិនបើ (did_throw)
	    discard_current_exception();
	got_int = FALSE;
	PyErr_SetNone(PyExc_KeyboardInterrupt);
	ត្រឡប់ -1;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (msg_list != NULL && *msg_list != NULL)
    {
	ចំនួនគត់	should_free;
	var_na	*msg;

	msg = get_exception_string(*msg_list, ET_ERROR, NULL, &should_free);

	ប្រសិនបើ (msg == NULL)
	{
	    PyErr_NoMemory();
	    ត្រឡប់ -1;
	}

	PyErr_SetVim(msg);

	free_global_msglist();

	ប្រសិនបើ (should_free)
	    vim_free(msg);

	ត្រឡប់ -1;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (!did_throw)
	ត្រឡប់ (PyErr_Occurred() ? -1 : 0);
    // Python exception is preferred over Vim one; unlikely to occur though
    បើមិនដូច្នេះ ប្រសិនបើ (PyErr_Occurred())
    {
	discard_current_exception();
	ត្រឡប់ -1;
    }
    // Finally transform Vim script exception to python one
    បើមិនដូច្នេះ
    {
	PyErr_SetVim((var_na *)current_exception->value);
	discard_current_exception();
	ត្រឡប់ -1;
    }
}

    static ចំនួនគត់
VimCheckInterrupt(void)
{
    ប្រសិនបើ (got_int)
    {
	PyErr_SetNone(PyExc_KeyboardInterrupt);
	ត្រឡប់ 1;
    }
    ត្រឡប់ 0;
}

// Vim module - Implementation

    static PyObject *
VimCommand(PyObject *self UNUSED, PyObject *string)
{
    char_u	*cmd;
    PyObject	*ret;
    PyObject	*todecref;

    ប្រសិនបើ (!(cmd = StringToChars(string, &todecref)))
	ត្រឡប់ NULL;

    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();

    VimTryStart();
    do_cmdline_cmd(cmd);
    update_screen(UPD_VALID);

    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    ប្រសិនបើ (VimTryEnd())
	ret = NULL;
    បើមិនដូច្នេះ
	ret = Py_None;

    Py_XINCREF(ret);
    Py_XDECREF(todecref);
    ត្រឡប់ ret;
}

/*
 * Function to translate a typval_T into a PyObject; this will recursively
 * translate lists/dictionaries into their Python equivalents.
 *
 * The depth parameter is to avoid infinite recursion, set it to 1 when
 * you call VimToPython.
 */
    static PyObject *
VimToPython(typval_T *our_tv, ចំនួនគត់ depth, PyObject *lookup_dict)
{
    PyObject	*ret;
    PyObject	*newObj;
    var_na	ptrBuf[parimaa_na(void *) * 2 + 3];

    // Avoid infinite recursion
    ប្រសិនបើ (depth > 100)
    {
	Py_INCREF(Py_None);
	ret = Py_None;
	ត្រឡប់ ret;
    }

    // Check if we run into a recursive loop.  The item must be in lookup_dict
    // then and we can use it again.
    ប្រសិនបើ ((our_tv->v_type == VAR_LIST && our_tv->vval.v_list != NULL)
	    || (our_tv->v_type == VAR_TUPLE && our_tv->vval.v_tuple != NULL)
	    || (our_tv->v_type == VAR_DICT && our_tv->vval.v_dict != NULL))
    {
	sprintf(ptrBuf, "%p",
		our_tv->v_type == VAR_LIST ? (void *)our_tv->vval.v_list
		    : our_tv->v_type == VAR_TUPLE ?
			(void *)our_tv->vval.v_tuple
			: (void *)our_tv->vval.v_dict);

	ប្រសិនបើ ((ret = PyDict_GetItemString(lookup_dict, ptrBuf)))
	{
	    Py_INCREF(ret);
	    ត្រឡប់ ret;
	}
    }

    ប្រសិនបើ (our_tv->v_type == VAR_STRING)
	ret = PyString_FromString(our_tv->vval.v_string == NULL
					? "" : (var_na *)our_tv->vval.v_string);
    បើមិនដូច្នេះ ប្រសិនបើ (our_tv->v_type == VAR_NUMBER)
    {
	var_na buf[NUMBUFLEN];

	// For backwards compatibility numbers are stored as strings.
	sprintf(buf, "%lld", (long long)our_tv->vval.v_number);
	ret = PyString_FromString((var_na *)buf);
    }
    បើមិនដូច្នេះ ប្រសិនបើ (our_tv->v_type == VAR_FLOAT)
    {
	var_na buf[NUMBUFLEN];

	sprintf(buf, "%f", our_tv->vval.v_float);
	ret = PyString_FromString((var_na *)buf);
    }
    បើមិនដូច្នេះ ប្រសិនបើ (our_tv->v_type == VAR_LIST)
    {
	list_T		*list = our_tv->vval.v_list;
	listitem_T	*curr;

	ប្រសិនបើ (list == NULL)
	    ត្រឡប់ NULL;

	ប្រសិនបើ (!(ret = PyList_New(0)))
	    ត្រឡប់ NULL;

	ប្រសិនបើ (PyDict_SetItemString(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    ត្រឡប់ NULL;
	}

	CHECK_LIST_MATERIALIZE(list);
	FOR_ALL_LIST_ITEMS(list, curr)
	{
	    ប្រសិនបើ (!(newObj = VimToPython(&curr->li_tv, depth + 1, lookup_dict)))
	    {
		Py_DECREF(ret);
		ត្រឡប់ NULL;
	    }
	    ប្រសិនបើ (PyList_Append(ret, newObj))
	    {
		Py_DECREF(newObj);
		Py_DECREF(ret);
		ត្រឡប់ NULL;
	    }
	    Py_DECREF(newObj);
	}
    }
    បើមិនដូច្នេះ ប្រសិនបើ (our_tv->v_type == VAR_TUPLE)
    {
	tuple_T	*tuple = our_tv->vval.v_tuple;
	ចំនួនគត់	len;

	ប្រសិនបើ (tuple == NULL)
	    ត្រឡប់ NULL;

	len = TUPLE_LEN(tuple);

	ret = PyTuple_New(len);
	ប្រសិនបើ (ret == NULL)
	    ត្រឡប់ NULL;

	សម្រាប់ (ចំនួនគត់ idx = 0; idx < len; idx++)
	{
	    typval_T	*item_tv = TUPLE_ITEM(tuple, idx);

	    newObj = VimToPython(item_tv, depth + 1, lookup_dict);
	    ប្រសិនបើ (!newObj)
	    {
		Py_DECREF(ret);
		ត្រឡប់ NULL;
	    }
	    PyTuple_SetItem(ret, idx, newObj);
	}

	ប្រសិនបើ (PyDict_SetItemString(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    ត្រឡប់ NULL;
	}
    }
    បើមិនដូច្នេះ ប្រសិនបើ (our_tv->v_type == VAR_DICT)
    {

	hashtab_T	*ht;
	long_u		todo;
	hashitem_T	*hi;
	dictitem_T	*di;

	ប្រសិនបើ (our_tv->vval.v_dict == NULL)
	    ត្រឡប់ NULL;
	ht = &our_tv->vval.v_dict->dv_hashtab;

	ប្រសិនបើ (!(ret = PyDict_New()))
	    ត្រឡប់ NULL;

	ប្រសិនបើ (PyDict_SetItemString(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    ត្រឡប់ NULL;
	}

	todo = ht->ht_used;
	សម្រាប់ (hi = ht->ht_array; todo > 0; ++hi)
	{
	    ប្រសិនបើ (!HASHITEM_EMPTY(hi))
	    {
		--todo;

		di = dict_lookup(hi);
		ប្រសិនបើ (!(newObj = VimToPython(&di->di_tv, depth + 1, lookup_dict)))
		{
		    Py_DECREF(ret);
		    ត្រឡប់ NULL;
		}
		ប្រសិនបើ (PyDict_SetItemString(ret, (var_na *)hi->hi_key, newObj))
		{
		    Py_DECREF(ret);
		    Py_DECREF(newObj);
		    ត្រឡប់ NULL;
		}
	    }
	}
    }
    បើមិនដូច្នេះ ប្រសិនបើ (our_tv->v_type == VAR_BOOL)
    {
	ប្រសិនបើ (our_tv->vval.v_number == VVAL_FALSE)
	{
	    ret = Py_False;
	    Py_INCREF(ret);
	}
	បើមិនដូច្នេះ
	{
	    ret = Py_True;
	    Py_INCREF(ret);
	}
	ត្រឡប់ ret;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (our_tv->v_type == VAR_SPECIAL)
    {
	Py_INCREF(Py_None);
	ret = Py_None;
	ត្រឡប់ ret;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (our_tv->v_type == VAR_BLOB)
	ret = PyBytes_FromStringAndSize(
		(var_na*) our_tv->vval.v_blob->bv_ga.ga_data,
		(Py_ssize_t) our_tv->vval.v_blob->bv_ga.ga_len);
    បើមិនដូច្នេះ
    {
	Py_INCREF(Py_None);
	ret = Py_None;
    }

    ត្រឡប់ ret;
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

    ប្រសិនបើ (!PyArg_ParseTuple(args, "O", &string))
	ត្រឡប់ NULL;

    ប្រសិនបើ (!(expr = StringToChars(string, &todecref)))
	ត្រឡប់ NULL;

    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    VimTryStart();
    our_tv = eval_expr(expr, NULL);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    Py_XDECREF(todecref);

    ប្រសិនបើ (VimTryEnd())
	ត្រឡប់ NULL;

    ប្រសិនបើ (our_tv == NULL)
    {
	PyErr_SET_VIM(N_("invalid expression"));
	ត្រឡប់ NULL;
    }

    // Convert the Vim type into a Python type.  Create a dictionary that's
    // used to check for recursive loops.
    ប្រសិនបើ (!(lookup_dict = PyDict_New()))
	ret = NULL;
    បើមិនដូច្នេះ
    {
	ret = VimToPython(our_tv, 1, lookup_dict);
	Py_DECREF(lookup_dict);
    }


    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    free_tv(our_tv);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    ត្រឡប់ ret;
}

static PyObject *ConvertToPyObject(typval_T *);

    static PyObject *
VimEvalPy(PyObject *self UNUSED, PyObject *string)
{
    typval_T	*our_tv;
    PyObject	*ret;
    char_u	*expr;
    PyObject	*todecref;

    ប្រសិនបើ (!(expr = StringToChars(string, &todecref)))
	ត្រឡប់ NULL;

    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    VimTryStart();
    our_tv = eval_expr(expr, NULL);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    Py_XDECREF(todecref);

    ប្រសិនបើ (VimTryEnd())
	ត្រឡប់ NULL;

    ប្រសិនបើ (our_tv == NULL)
    {
	PyErr_SET_VIM(N_("invalid expression"));
	ត្រឡប់ NULL;
    }

    ret = ConvertToPyObject(our_tv);
    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    free_tv(our_tv);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    ត្រឡប់ ret;
}

    static PyObject *
VimStrwidth(PyObject *self UNUSED, PyObject *string)
{
    char_u	*str;
    PyObject	*todecref;
    ចំនួនគត់		len;

    ប្រសិនបើ (!(str = StringToChars(string, &todecref)))
	ត្រឡប់ NULL;

    len = mb_string2cells(str, (ចំនួនគត់)STRLEN(str));

    Py_XDECREF(todecref);

    ត្រឡប់ PyLong_FromLong(len);
}

    static PyObject *
_VimChdir(PyObject *_chdir, PyObject *args, PyObject *kwargs)
{
    PyObject	*ret;
    PyObject	*newwd;
    PyObject	*todecref;
    char_u	*new_dir;

    ប្រសិនបើ (_chdir == NULL)
	ត្រឡប់ NULL;
    ប្រសិនបើ (!(ret = PyObject_Call(_chdir, args, kwargs)))
	ត្រឡប់ NULL;

    ប្រសិនបើ (!(newwd = PyObject_CallFunctionObjArgs(py_getcwd, NULL)))
    {
	Py_DECREF(ret);
	ត្រឡប់ NULL;
    }

    ប្រសិនបើ (!(new_dir = StringToChars(newwd, &todecref)))
    {
	Py_DECREF(ret);
	Py_DECREF(newwd);
	ត្រឡប់ NULL;
    }

    VimTryStart();

    ប្រសិនបើ (vim_chdir(new_dir))
    {
	Py_DECREF(ret);
	Py_DECREF(newwd);
	Py_XDECREF(todecref);

	ប្រសិនបើ (VimTryEnd())
	    ត្រឡប់ NULL;

	PyErr_SET_VIM(N_("failed to change directory"));
	ត្រឡប់ NULL;
    }

    Py_DECREF(newwd);
    Py_XDECREF(todecref);

    post_chdir(CDSCOPE_GLOBAL);

    ប្រសិនបើ (VimTryEnd())
    {
	Py_DECREF(ret);
	ត្រឡប់ NULL;
    }

    ត្រឡប់ ret;
}

    static PyObject *
VimChdir(PyObject *self UNUSED, PyObject *args, PyObject *kwargs)
{
    ត្រឡប់ _VimChdir(py_chdir, args, kwargs);
}

    static PyObject *
VimFchdir(PyObject *self UNUSED, PyObject *args, PyObject *kwargs)
{
    ត្រឡប់ _VimChdir(py_fchdir, args, kwargs);
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

    ប្រសិនបើ (!(pathObject = PyString_FromString((var_na *)path)))
    {
	*data = NULL;
	ត្រឡប់;
    }

    mr_data->result = PyObject_CallFunctionObjArgs(mr_data->callable,
						   pathObject, NULL);

    Py_DECREF(pathObject);

    ប្រសិនបើ (!mr_data->result || mr_data->result != Py_None)
	*data = NULL;
    បើមិនដូច្នេះ
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

    ប្រសិនបើ (data.result == NULL)
    {
	ប្រសិនបើ (PyErr_Occurred())
	    ត្រឡប់ NULL;
	បើមិនដូច្នេះ
	{
	    Py_INCREF(Py_None);
	    ត្រឡប់ Py_None;
	}
    }
    ត្រឡប់ data.result;
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

#ប្រសិនបើ PY_MAJOR_VERSION < 3
# define PY_MAIN_DIR_STRING "python2"
#បើមិនដូច្នេះ
# define PY_MAIN_DIR_STRING "python3"
#endif
#define PY_ALTERNATE_DIR_STRING "pythonx"

#define PYTHONX_STRING_LENGTH 7 // STRLEN("pythonx")
    ប្រសិនបើ (!(pathbuf = PyMem_New(var_na,
		    pathlen + STRLEN(PATHSEPSTR) + PYTHONX_STRING_LENGTH + 1)))
    {
	PyErr_NoMemory();
	*data = NULL;
	ត្រឡប់;
    }

    mch_memmove(pathbuf, path, pathlen + 1);
    add_pathsep((char_u *) pathbuf);

    pathlen = STRLEN(pathbuf);
    mch_memmove(pathbuf + pathlen, PY_MAIN_DIR_STRING,
	    PYTHONX_STRING_LENGTH + 1);

    ប្រសិនបើ (!(pathObject1 = PyString_FromString(pathbuf)))
    {
	*data = NULL;
	PyMem_Free(pathbuf);
	ត្រឡប់;
    }

    mch_memmove(pathbuf + pathlen, PY_ALTERNATE_DIR_STRING,
	    PYTHONX_STRING_LENGTH + 1);

    ប្រសិនបើ (!(pathObject2 = PyString_FromString(pathbuf)))
    {
	Py_DECREF(pathObject1);
	PyMem_Free(pathbuf);
	*data = NULL;
	ត្រឡប់;
    }

    PyMem_Free(pathbuf);

    ប្រសិនបើ (PyList_Append(list, pathObject1)
	    || PyList_Append(list, pathObject2))
	*data = NULL;

    Py_DECREF(pathObject1);
    Py_DECREF(pathObject2);
}

    static PyObject *
Vim_GetPaths(PyObject *self UNUSED, PyObject *args UNUSED)
{
    PyObject	*ret;

    ប្រសិនបើ (!(ret = PyList_New(0)))
	ត្រឡប់ NULL;

    do_in_runtimepath(NULL, 0, &map_finder_callback, ret);

    ប្រសិនបើ (PyErr_Occurred())
    {
	Py_DECREF(ret);
	ត្រឡប់ NULL;
    }

    ត្រឡប់ ret;
}

#ប្រសិនបើ PY_VERSION_HEX >= 0x030700f0
    static PyObject *
FinderFindSpec(PyObject *self, PyObject *args)
{
    var_na	*fullname;
    PyObject	*paths;
    PyObject	*target = Py_None;
    PyObject	*spec;

    ប្រសិនបើ (!PyArg_ParseTuple(args, "s|O", &fullname, &target))
	ត្រឡប់ NULL;

    ប្រសិនបើ (!(paths = Vim_GetPaths(self, NULL)))
	ត្រឡប់ NULL;

    spec = PyObject_CallFunction(py_find_spec, "sOO", fullname, paths, target);

    Py_DECREF(paths);

    ប្រសិនបើ (!spec)
    {
	ប្រសិនបើ (PyErr_Occurred())
	    ត្រឡប់ NULL;

	Py_INCREF(Py_None);
	ត្រឡប់ Py_None;
    }

    ត្រឡប់ spec;
}

    static PyObject *
FinderFindModule(PyObject* self UNUSED, PyObject* args UNUSED)
{
    // Apparently returning None works.
    Py_INCREF(Py_None);
    ត្រឡប់ Py_None;
}
#បើមិនដូច្នេះ
    static PyObject *
call_load_module(var_na *name, ចំនួនគត់ len, PyObject *find_module_result)
{
    PyObject	*fd, *pathname, *description;

    ប្រសិនបើ (!PyTuple_Check(find_module_result))
    {
	PyErr_FORMAT_TYPE(
		N_("expected 3-tuple as imp.find_module() result, but got %s"),
		find_module_result);
	ត្រឡប់ NULL;
    }
    ប្រសិនបើ (PyTuple_GET_SIZE(find_module_result) != 3)
    {
	PyErr_FORMAT(PyExc_TypeError,
		N_("expected 3-tuple as imp.find_module() result, but got "
		   "tuple of size %d"),
		(ចំនួនគត់) PyTuple_GET_SIZE(find_module_result));
	ត្រឡប់ NULL;
    }

    ប្រសិនបើ (!(fd = PyTuple_GET_ITEM(find_module_result, 0))
	    || !(pathname = PyTuple_GET_ITEM(find_module_result, 1))
	    || !(description = PyTuple_GET_ITEM(find_module_result, 2)))
    {
	PyErr_SET_STRING(PyExc_RuntimeError,
		N_("internal error: imp.find_module returned tuple with NULL"));
	ត្រឡប់ NULL;
    }

    ត្រឡប់ PyObject_CallFunction(py_load_module,
	    "s#OOO", name, len, fd, pathname, description);
}

    static PyObject *
find_module(var_na *fullname, var_na *tail, PyObject *new_path)
{
    PyObject	*find_module_result;
    PyObject	*module;
    var_na	*dot;

    ប្រសិនបើ ((dot = (var_na *)vim_strchr((char_u *) tail, '.')))
    {
	/*
	 * There is a dot in the name: call find_module recursively without the
	 * first component
	 */
	PyObject	*newest_path;
	ចំនួនគត់		partlen = (ចំនួនគត់) (dot - 1 - tail);

	ប្រសិនបើ (!(find_module_result = PyObject_CallFunction(py_find_module,
			"s#O", tail, partlen, new_path)))
	{
	    ប្រសិនបើ (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_ImportError))
		PyErr_Clear();
	    ត្រឡប់ NULL;
	}

	ប្រសិនបើ (!(module = call_load_module(
			fullname,
			((ចំនួនគត់) (tail - fullname)) + partlen,
			find_module_result)))
	{
	    Py_DECREF(find_module_result);
	    ត្រឡប់ NULL;
	}

	Py_DECREF(find_module_result);

	ប្រសិនបើ (!(newest_path = PyObject_GetAttrString(module, "__path__")))
	{
	    Py_DECREF(module);
	    ត្រឡប់ NULL;
	}

	Py_DECREF(module);

	find_module_result = find_module(fullname, dot + 1, newest_path);

	Py_DECREF(newest_path);

	ត្រឡប់ find_module_result;
    }
    បើមិនដូច្នេះ
    {
	ប្រសិនបើ (!(find_module_result = PyObject_CallFunction(py_find_module,
			"sO", tail, new_path)))
	{
	    ប្រសិនបើ (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_ImportError))
		PyErr_Clear();
	    ត្រឡប់ NULL;
	}

	ត្រឡប់ find_module_result;
    }
}

    static PyObject *
FinderFindModule(PyObject *self, PyObject *args)
{
    var_na	*fullname;
    PyObject	*result;
    PyObject	*new_path;
    LoaderObject	*loader;

    ប្រសិនបើ (!PyArg_ParseTuple(args, "s", &fullname))
	ត្រឡប់ NULL;

    ប្រសិនបើ (!(new_path = Vim_GetPaths(self, NULL)))
	ត្រឡប់ NULL;

    result = find_module(fullname, fullname, new_path);

    Py_DECREF(new_path);

    ប្រសិនបើ (!result)
    {
	ប្រសិនបើ (PyErr_Occurred())
	    ត្រឡប់ NULL;

	Py_INCREF(Py_None);
	ត្រឡប់ Py_None;
    }

    ប្រសិនបើ (!(fullname = (var_na *)vim_strsave((char_u *)fullname)))
    {
	Py_DECREF(result);
	PyErr_NoMemory();
	ត្រឡប់ NULL;
    }

    ប្រសិនបើ (!(loader = PyObject_NEW(LoaderObject, LoaderTypePtr)))
    {
	vim_free(fullname);
	Py_DECREF(result);
	ត្រឡប់ NULL;
    }

    loader->fullname = fullname;
    loader->result = result;

    ត្រឡប់ (PyObject *) loader;
}
#endif

    static PyObject *
VimPathHook(PyObject *self UNUSED, PyObject *args)
{
    var_na	*path;

    ប្រសិនបើ (PyArg_ParseTuple(args, "s", &path)
	    && STRCMP(path, vim_special_path) == 0)
    {
	Py_INCREF(vim_module);
	ត្រឡប់ vim_module;
    }

    PyErr_Clear();
    PyErr_SetNone(PyExc_ImportError);
    ត្រឡប់ NULL;
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
#ប្រសិនបើ PY_VERSION_HEX >= 0x030700f0
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
typedef ចំនួនគត់ (*traversefun)(void *, visitproc, void *);
typedef ចំនួនគត់ (*clearfun)(void **);

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

    ប្រសិនបើ (iter_object)
	Py_INCREF(iter_object);

    ត្រឡប់ (PyObject *)(self);
}

    static void
IterDestructor(PyObject *self_obj)
{
    IterObject *self = (IterObject*)self_obj;
    ប្រសិនបើ (self->iter_object)
	Py_DECREF(self->iter_object);
    PyObject_GC_UnTrack((void *)(self));
    self->destruct(self->cur);
    PyObject_GC_Del((void *)(self));
}

    static ចំនួនគត់
IterTraverse(PyObject *self_obj, visitproc visit, void *arg)
{
    IterObject *self = (IterObject*)self_obj;
    ប្រសិនបើ (self->traverse != NULL)
	ត្រឡប់ self->traverse(self->cur, visit, arg);
    បើមិនដូច្នេះ
	ត្រឡប់ 0;
}

// Mac OSX defines clear() somewhere.
#ifdef clear
# undef clear
#endif

    static ចំនួនគត់
IterClear(PyObject *self_obj)
{
    IterObject *self = (IterObject*)self_obj;
    ប្រសិនបើ (self->clear != NULL)
	ត្រឡប់ self->clear(&self->cur);
    បើមិនដូច្នេះ
	ត្រឡប់ 0;
}

    static PyObject *
IterNext(PyObject *self_obj)
{
    IterObject *self = (IterObject*)self_obj;
    ត្រឡប់ self->next(&self->cur);
}

    static PyObject *
IterIter(PyObject *self)
{
    Py_INCREF(self);
    ត្រឡប់ self;
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
    ប្រសិនបើ (ref->pll_prev == NULL)
    {
	ប្រសិនបើ (ref->pll_next == NULL)
	{
	    *last = NULL;
	    ត្រឡប់;
	}
    }
    បើមិនដូច្នេះ
	ref->pll_prev->pll_next = ref->pll_next;

    ប្រសិនបើ (ref->pll_next == NULL)
	*last = ref->pll_prev;
    បើមិនដូច្នេះ
	ref->pll_next->pll_prev = ref->pll_prev;
}

    static void
pyll_add(PyObject *self, pylinkedlist_T *ref, pylinkedlist_T **last)
{
    ប្រសិនបើ (*last == NULL)
	ref->pll_prev = NULL;
    បើមិនដូច្នេះ
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
    ប្រសិនបើ (self == NULL)
	ត្រឡប់ NULL;
    self->dict = dict;
    ++dict->dv_refcount;

    pyll_add((PyObject *)(self), &self->ref, &lastdict);

    ត្រឡប់ (PyObject *)(self);
}

    static dict_T *
py_dict_alloc(void)
{
    dict_T	*ret;

    ប្រសិនបើ (!(ret = dict_alloc()))
    {
	PyErr_NoMemory();
	ត្រឡប់ NULL;
    }
    ++ret->dv_refcount;

    ត្រឡប់ ret;
}

    static PyObject *
DictionaryConstructor(PyTypeObject *subtype, PyObject *args, PyObject *kwargs)
{
    DictionaryObject	*self;
    dict_T	*dict;

    ប្រសិនបើ (!(dict = py_dict_alloc()))
	ត្រឡប់ NULL;

    self = (DictionaryObject *) DictionaryNew(subtype, dict);

    --dict->dv_refcount;

    ប្រសិនបើ (kwargs || PyTuple_Size(args))
    {
	PyObject	*tmp;
	ប្រសិនបើ (!(tmp = DictionaryUpdate(self, args, kwargs)))
	{
	    Py_DECREF(self);
	    ត្រឡប់ NULL;
	}

	Py_DECREF(tmp);
    }

    ត្រឡប់ (PyObject *)(self);
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
    ត្រឡប់ ObjectDir(self, DictionaryAttrs);
}

    static ចំនួនគត់
DictionarySetattr(PyObject *self_obj, var_na *name, PyObject *valObject)
{
    DictionaryObject *self = (DictionaryObject*)self_obj;
    ប្រសិនបើ (valObject == NULL)
    {
	PyErr_SET_STRING(PyExc_AttributeError,
		N_("cannot delete vim.Dictionary attributes"));
	ត្រឡប់ -1;
    }

    ប្រសិនបើ (strcmp(name, "locked") == 0)
    {
	ប្រសិនបើ (self->dict->dv_lock == VAR_FIXED)
	{
	    PyErr_SET_STRING(PyExc_TypeError,
		    N_("cannot modify fixed dictionary"));
	    ត្រឡប់ -1;
	}
	បើមិនដូច្នេះ
	{
	    ចំនួនគត់		istrue = PyObject_IsTrue(valObject);
	    ប្រសិនបើ (istrue == -1)
		ត្រឡប់ -1;
	    បើមិនដូច្នេះ ប្រសិនបើ (istrue)
		self->dict->dv_lock = VAR_LOCKED;
	    បើមិនដូច្នេះ
		self->dict->dv_lock = 0;
	}
	ត្រឡប់ 0;
    }
    បើមិនដូច្នេះ
    {
	PyErr_FORMAT(PyExc_AttributeError, N_("cannot set attribute %s"), name);
	ត្រឡប់ -1;
    }
}

    static PyInt
DictionaryLength(DictionaryObject *self)
{
    ត្រឡប់ ((PyInt) (self->dict->dv_hashtab.ht_used));
}

#define DICT_FLAG_HAS_DEFAULT	0x01
#define DICT_FLAG_POP		0x02
#define DICT_FLAG_NONE_DEFAULT	0x04
#define DICT_FLAG_RETURN_BOOL	0x08 // Incompatible with DICT_FLAG_POP
#define DICT_FLAG_RETURN_PAIR	0x10

    static PyObject *
_DictionaryItem(DictionaryObject *self, PyObject *args, ចំនួនគត់ flags)
{
    PyObject	*keyObject;
    PyObject	*defObject = ((flags & DICT_FLAG_NONE_DEFAULT)? Py_None : NULL);
    PyObject	*ret;
    char_u	*key;
    dictitem_T	*di;
    dict_T	*dict = self->dict;
    hashitem_T	*hi;
    PyObject	*todecref;

    ប្រសិនបើ (flags & DICT_FLAG_HAS_DEFAULT)
    {
	ប្រសិនបើ (!PyArg_ParseTuple(args, "O|O", &keyObject, &defObject))
	    ត្រឡប់ NULL;
    }
    បើមិនដូច្នេះ
	keyObject = args;

    ប្រសិនបើ (flags & DICT_FLAG_RETURN_BOOL)
	defObject = Py_False;

    ប្រសិនបើ (!(key = StringToChars(keyObject, &todecref)))
	ត្រឡប់ NULL;

    ប្រសិនបើ (*key == NUL)
    {
	RAISE_NO_EMPTY_KEYS;
	Py_XDECREF(todecref);
	ត្រឡប់ NULL;
    }

    hi = hash_find(&dict->dv_hashtab, key);

    Py_XDECREF(todecref);

    ប្រសិនបើ (HASHITEM_EMPTY(hi))
    {
	ប្រសិនបើ (defObject)
	{
	    Py_INCREF(defObject);
	    ត្រឡប់ defObject;
	}
	បើមិនដូច្នេះ
	{
	    PyErr_SetObject(PyExc_KeyError, keyObject);
	    ត្រឡប់ NULL;
	}
    }
    បើមិនដូច្នេះ ប្រសិនបើ (flags & DICT_FLAG_RETURN_BOOL)
    {
	ret = Py_True;
	Py_INCREF(ret);
	ត្រឡប់ ret;
    }

    di = dict_lookup(hi);

    ប្រសិនបើ (!(ret = ConvertToPyObject(&di->di_tv)))
	ត្រឡប់ NULL;

    ប្រសិនបើ (flags & DICT_FLAG_POP)
    {
	ប្រសិនបើ (dict->dv_lock)
	{
	    RAISE_LOCKED_DICTIONARY;
	    Py_DECREF(ret);
	    ត្រឡប់ NULL;
	}

	hash_remove(&dict->dv_hashtab, hi, "Python remove variable");
	dictitem_free(di);
    }

    ត្រឡប់ ret;
}

    static PyObject *
DictionaryItem(DictionaryObject *self, PyObject *keyObject)
{
    ត្រឡប់ _DictionaryItem(self, keyObject, 0);
}

    static ចំនួនគត់
DictionaryContains(DictionaryObject *self, PyObject *keyObject)
{
    PyObject	*rObj = _DictionaryItem(self, keyObject, DICT_FLAG_RETURN_BOOL);
    ចំនួនគត់		ret;

    ប្រសិនបើ (rObj == NULL)
	ត្រឡប់ -1;

    ret = (rObj == Py_True);

    Py_DECREF(rObj);

    ត្រឡប់ ret;
}

typedef sa_nracanaa
{
    ចំនួនគត់		dii_changed;
    hashtab_T	*dii_ht;
    hashitem_T	*dii_hi;
    long_u	dii_todo;
} dictiterinfo_T;

    static PyObject *
DictionaryIterNext(void **arg)
{
    PyObject	*ret;
    dictiterinfo_T **dii = (dictiterinfo_T**)arg;

    ប្រសិនបើ (!(*dii)->dii_todo)
	ត្រឡប់ NULL;

    ប្រសិនបើ ((*dii)->dii_ht->ht_changed != (*dii)->dii_changed)
    {
	PyErr_SET_STRING(PyExc_RuntimeError,
		N_("hashtab changed during iteration"));
	ត្រឡប់ NULL;
    }

    ខណៈពេល (((*dii)->dii_todo) && HASHITEM_EMPTY((*dii)->dii_hi))
	++((*dii)->dii_hi);

    --((*dii)->dii_todo);

    ប្រសិនបើ (!(ret = PyBytes_FromString((var_na *)(*dii)->dii_hi->hi_key)))
	ត្រឡប់ NULL;

    ត្រឡប់ ret;
}

    static PyObject *
DictionaryIter(PyObject *self_obj)
{
    DictionaryObject	*self = (DictionaryObject*)self_obj;
    dictiterinfo_T	*dii;
    hashtab_T		*ht;

    ប្រសិនបើ (!(dii = PyMem_New(dictiterinfo_T, 1)))
    {
	PyErr_NoMemory();
	ត្រឡប់ NULL;
    }

    ht = &self->dict->dv_hashtab;
    dii->dii_changed = ht->ht_changed;
    dii->dii_ht = ht;
    dii->dii_hi = ht->ht_array;
    dii->dii_todo = ht->ht_used;

    ត្រឡប់ IterNew(dii,
	    PyMem_Free, DictionaryIterNext,
	    NULL, NULL, (PyObject *)self);
}

    static ចំនួនគត់
DictionaryAssItem(
	DictionaryObject *self, PyObject *keyObject, PyObject *valObject)
{
    char_u	*key;
    typval_T	tv;
    dict_T	*dict = self->dict;
    dictitem_T	*di;
    PyObject	*todecref;

    ប្រសិនបើ (dict->dv_lock)
    {
	RAISE_LOCKED_DICTIONARY;
	ត្រឡប់ -1;
    }

    ប្រសិនបើ (!(key = StringToChars(keyObject, &todecref)))
	ត្រឡប់ -1;

    ប្រសិនបើ (*key == NUL)
    {
	RAISE_NO_EMPTY_KEYS;
	Py_XDECREF(todecref);
	ត្រឡប់ -1;
    }

    di = dict_find(dict, key, -1);

    ប្រសិនបើ (valObject == NULL)
    {
	hashitem_T	*hi;

	ប្រសិនបើ (di == NULL)
	{
	    Py_XDECREF(todecref);
	    PyErr_SetObject(PyExc_KeyError, keyObject);
	    ត្រឡប់ -1;
	}
	hi = hash_find(&dict->dv_hashtab, di->di_key);
	hash_remove(&dict->dv_hashtab, hi, "Python remove item");
	dictitem_free(di);
	Py_XDECREF(todecref);
	ត្រឡប់ 0;
    }

    ប្រសិនបើ (ConvertFromPyObject(valObject, &tv) == -1)
    {
	Py_XDECREF(todecref);
	ត្រឡប់ -1;
    }

    ប្រសិនបើ (di == NULL)
    {
	ប្រសិនបើ (!(di = dictitem_alloc(key)))
	{
	    Py_XDECREF(todecref);
	    PyErr_NoMemory();
	    ត្រឡប់ -1;
	}
	di->di_tv.v_type = VAR_UNKNOWN;

	ប្រសិនបើ (dict_add(dict, di) == FAIL)
	{
	    dictitem_free(di);
	    RAISE_KEY_ADD_FAIL(key);
	    Py_XDECREF(todecref);
	    ត្រឡប់ -1;
	}
    }
    បើមិនដូច្នេះ
	clear_tv(&di->di_tv);

    Py_XDECREF(todecref);

    copy_tv(&tv, &di->di_tv);
    clear_tv(&tv);
    ត្រឡប់ 0;
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
    សម្រាប់ (hi = dict->dv_hashtab.ht_array; todo > 0; ++hi)
    {
	ប្រសិនបើ (!HASHITEM_EMPTY(hi))
	{
	    ប្រសិនបើ (!(newObj = hiconvert(hi)))
	    {
		Py_DECREF(ret);
		ត្រឡប់ NULL;
	    }
	    PyList_SET_ITEM(ret, i, newObj);
	    --todo;
	    ++i;
	}
    }
    ត្រឡប់ ret;
}

    static PyObject *
dict_key(hashitem_T *hi)
{
    ត្រឡប់ PyBytes_FromString((var_na *)(hi->hi_key));
}

    static PyObject *
DictionaryListKeys(DictionaryObject *self, PyObject *args UNUSED)
{
    ត្រឡប់ DictionaryListObjects(self, dict_key);
}

    static PyObject *
dict_val(hashitem_T *hi)
{
    dictitem_T	*di;

    di = dict_lookup(hi);
    ត្រឡប់ ConvertToPyObject(&di->di_tv);
}

    static PyObject *
DictionaryListValues(DictionaryObject *self, PyObject *args UNUSED)
{
    ត្រឡប់ DictionaryListObjects(self, dict_val);
}

    static PyObject *
dict_item(hashitem_T *hi)
{
    PyObject	*keyObject;
    PyObject	*valObject;
    PyObject	*ret;

    ប្រសិនបើ (!(keyObject = dict_key(hi)))
	ត្រឡប់ NULL;

    ប្រសិនបើ (!(valObject = dict_val(hi)))
    {
	Py_DECREF(keyObject);
	ត្រឡប់ NULL;
    }

    ret = Py_BuildValue("(OO)", keyObject, valObject);

    Py_DECREF(keyObject);
    Py_DECREF(valObject);

    ត្រឡប់ ret;
}

    static PyObject *
DictionaryListItems(DictionaryObject *self, PyObject *args UNUSED)
{
    ត្រឡប់ DictionaryListObjects(self, dict_item);
}

    static PyObject *
DictionaryUpdate(DictionaryObject *self, PyObject *args, PyObject *kwargs)
{
    dict_T	*dict = self->dict;

    ប្រសិនបើ (dict->dv_lock)
    {
	RAISE_LOCKED_DICTIONARY;
	ត្រឡប់ NULL;
    }

    ប្រសិនបើ (kwargs)
    {
	typval_T	tv;

	ប្រសិនបើ (ConvertFromPyMapping(kwargs, &tv) == -1)
	    ត្រឡប់ NULL;

	VimTryStart();
	dict_extend(self->dict, tv.vval.v_dict, (char_u *) "force", NULL);
	clear_tv(&tv);
	ប្រសិនបើ (VimTryEnd())
	    ត្រឡប់ NULL;
    }
    បើមិនដូច្នេះ
    {
	PyObject	*obj = NULL;

	ប្រសិនបើ (!PyArg_ParseTuple(args, "|O", &obj))
	    ត្រឡប់ NULL;

	ប្រសិនបើ (obj == NULL)
	{
	    Py_INCREF(Py_None);
	    ត្រឡប់ Py_None;
	}

	ប្រសិនបើ (PyObject_HasAttrString(obj, "keys"))
	    ត្រឡប់ DictionaryUpdate(self, NULL, obj);
	បើមិនដូច្នេះ
	{
	    PyObject	*iterator;
	    PyObject	*item;

	    ប្រសិនបើ (!(iterator = PyObject_GetIter(obj)))
		ត្រឡប់ NULL;

	    ខណៈពេល ((item = PyIter_Next(iterator)))
	    {
		PyObject	*fast;
		PyObject	*keyObject;
		PyObject	*valObject;
		PyObject	*todecref;
		char_u		*key;
		dictitem_T	*di;
		hashitem_T	*hi;

		ប្រសិនបើ (!(fast = PySequence_Fast(item, "")))
		{
		    Py_DECREF(iterator);
		    Py_DECREF(item);
		    ត្រឡប់ NULL;
		}

		Py_DECREF(item);

		ប្រសិនបើ (PySequence_Size(fast) != 2)
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    PyErr_FORMAT(PyExc_ValueError,
			    N_("expected sequence element of size 2, "
			    "but got sequence of size %d"),
			    (ចំនួនគត់) PySequence_Size(fast));
		    ត្រឡប់ NULL;
		}

		keyObject = PySequence_GetItem(fast, 0);

		ប្រសិនបើ (!(key = StringToChars(keyObject, &todecref)))
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    ត្រឡប់ NULL;
		}

		di = dictitem_alloc(key);

		Py_XDECREF(todecref);

		ប្រសិនបើ (di == NULL)
		{
		    Py_DECREF(fast);
		    Py_DECREF(iterator);
		    PyErr_NoMemory();
		    ត្រឡប់ NULL;
		}
		di->di_tv.v_type = VAR_UNKNOWN;

		valObject = PySequence_GetItem(fast, 1);

		ប្រសិនបើ (ConvertFromPyObject(valObject, &di->di_tv) == -1)
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    dictitem_free(di);
		    ត្រឡប់ NULL;
		}

		Py_DECREF(fast);

		hi = hash_find(&dict->dv_hashtab, di->di_key);
		ប្រសិនបើ (!HASHITEM_EMPTY(hi) || dict_add(dict, di) == FAIL)
		{
		    RAISE_KEY_ADD_FAIL(di->di_key);
		    Py_DECREF(iterator);
		    dictitem_free(di);
		    ត្រឡប់ NULL;
		}
	    }

	    Py_DECREF(iterator);

	    // Iterator may have finished due to an exception
	    ប្រសិនបើ (PyErr_Occurred())
		ត្រឡប់ NULL;
	}
    }
    Py_INCREF(Py_None);
    ត្រឡប់ Py_None;
}

    static PyObject *
DictionaryGet(DictionaryObject *self, PyObject *args)
{
    ត្រឡប់ _DictionaryItem(self, args,
			    DICT_FLAG_HAS_DEFAULT|DICT_FLAG_NONE_DEFAULT);
}

    static PyObject *
DictionaryPop(DictionaryObject *self, PyObject *args)
{
    ត្រឡប់ _DictionaryItem(self, args, DICT_FLAG_HAS_DEFAULT|DICT_FLAG_POP);
}

    static PyObject *
DictionaryPopItem(DictionaryObject *self, PyObject *args UNUSED)
{
    hashitem_T	*hi;
    PyObject	*ret;
    PyObject	*valObject;
    dictitem_T	*di;

    ប្រសិនបើ (self->dict->dv_hashtab.ht_used == 0)
    {
	PyErr_SetNone(PyExc_KeyError);
	ត្រឡប់ NULL;
    }

    hi = self->dict->dv_hashtab.ht_array;
    ខណៈពេល (HASHITEM_EMPTY(hi))
	++hi;

    di = dict_lookup(hi);

    ប្រសិនបើ (!(valObject = ConvertToPyObject(&di->di_tv)))
	ត្រឡប់ NULL;

    ប្រសិនបើ (!(ret = Py_BuildValue("(" Py_bytes_fmt "O)", hi->hi_key, valObject)))
    {
	Py_DECREF(valObject);
	ត្រឡប់ NULL;
    }

    hash_remove(&self->dict->dv_hashtab, hi, "Python pop item");
    dictitem_free(di);

    ត្រឡប់ ret;
}

    static PyObject *
DictionaryHasKey(DictionaryObject *self, PyObject *keyObject)
{
    ត្រឡប់ _DictionaryItem(self, keyObject, DICT_FLAG_RETURN_BOOL);
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

    ប្រសិនបើ (list == NULL)
	ត្រឡប់ NULL;

    self = (ListObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    ប្រសិនបើ (self == NULL)
	ត្រឡប់ NULL;
    self->list = list;
    ++list->lv_refcount;
    CHECK_LIST_MATERIALIZE(list);

    pyll_add((PyObject *)(self), &self->ref, &lastlist);

    ត្រឡប់ (PyObject *)(self);
}

    static list_T *
py_list_alloc(void)
{
    list_T	*ret;

    ប្រសិនបើ (!(ret = list_alloc()))
    {
	PyErr_NoMemory();
	ត្រឡប់ NULL;
    }
    ++ret->lv_refcount;

    ត្រឡប់ ret;
}

    static ចំនួនគត់
list_py_concat(list_T *l, PyObject *obj, PyObject *lookup_dict)
{
    PyObject	*iterator;
    PyObject	*item;
    listitem_T	*li;

    ប្រសិនបើ (!(iterator = PyObject_GetIter(obj)))
	ត្រឡប់ -1;

    ខណៈពេល ((item = PyIter_Next(iterator)))
    {
	ប្រសិនបើ (!(li = listitem_alloc()))
	{
	    PyErr_NoMemory();
	    Py_DECREF(item);
	    Py_DECREF(iterator);
	    ត្រឡប់ -1;
	}
	li->li_tv.v_lock = 0;
	li->li_tv.v_type = VAR_UNKNOWN;

	ប្រសិនបើ (_ConvertFromPyObject(item, &li->li_tv, lookup_dict) == -1)
	{
	    Py_DECREF(item);
	    Py_DECREF(iterator);
	    listitem_free(l, li);
	    ត្រឡប់ -1;
	}

	Py_DECREF(item);

	list_append(l, li);
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    ប្រសិនបើ (PyErr_Occurred())
	ត្រឡប់ -1;

    ត្រឡប់ 0;
}

    static PyObject *
ListConstructor(PyTypeObject *subtype, PyObject *args, PyObject *kwargs)
{
    list_T	*list;
    PyObject	*obj = NULL;

    ប្រសិនបើ (kwargs)
    {
	PyErr_SET_STRING(PyExc_TypeError,
		N_("list constructor does not accept keyword arguments"));
	ត្រឡប់ NULL;
    }

    ប្រសិនបើ (!PyArg_ParseTuple(args, "|O", &obj))
	ត្រឡប់ NULL;

    ប្រសិនបើ (!(list = py_list_alloc()))
	ត្រឡប់ NULL;

    ប្រសិនបើ (obj)
    {
	PyObject	*lookup_dict;

	ប្រសិនបើ (!(lookup_dict = PyDict_New()))
	{
	    list_unref(list);
	    ត្រឡប់ NULL;
	}

	ប្រសិនបើ (list_py_concat(list, obj, lookup_dict) == -1)
	{
	    Py_DECREF(lookup_dict);
	    list_unref(list);
	    ត្រឡប់ NULL;
	}

	Py_DECREF(lookup_dict);
    }

    ត្រឡប់ ListNew(subtype, list);
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
    ត្រឡប់ ((PyInt) (self->list->lv_len));
}

    static PyObject *
ListIndex(ListObject *self, Py_ssize_t index)
{
    listitem_T	*li;

    ប្រសិនបើ (index >= ListLength(self))
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("list index out of range"));
	ត្រឡប់ NULL;
    }
    li = list_find(self->list, (long) index);
    ប្រសិនបើ (li == NULL)
    {
	// No more suitable format specifications in python-2.3
	PyErr_VIM_FORMAT(N_("internal error: failed to get Vim list item %d"),
		(ចំនួនគត់) index);
	ត្រឡប់ NULL;
    }
    ត្រឡប់ ConvertToPyObject(&li->li_tv);
}

    static PyObject *
ListSlice(ListObject *self, Py_ssize_t first, Py_ssize_t step,
	  Py_ssize_t slicelen)
{
    PyInt	i;
    PyObject	*list;

    ប្រសិនបើ (step == 0)
    {
	PyErr_SET_STRING(PyExc_ValueError, N_("slice step cannot be zero"));
	ត្រឡប់ NULL;
    }

    list = PyList_New(slicelen);
    ប្រសិនបើ (list == NULL)
	ត្រឡប់ NULL;

    សម្រាប់ (i = 0; i < slicelen; ++i)
    {
	PyObject	*item;

	item = ListIndex(self, first + i*step);
	ប្រសិនបើ (item == NULL)
	{
	    Py_DECREF(list);
	    ត្រឡប់ NULL;
	}

	PyList_SET_ITEM(list, i, item);
    }

    ត្រឡប់ list;
}

    static PyObject *
ListItem(ListObject *self, PyObject* idx)
{
#ប្រសិនបើ PY_MAJOR_VERSION < 3
    ប្រសិនបើ (PyInt_Check(idx))
    {
	long _idx = PyInt_AsLong(idx);
	ត្រឡប់ ListIndex(self, _idx);
    }
    បើមិនដូច្នេះ
#endif
    ប្រសិនបើ (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	ត្រឡប់ ListIndex(self, _idx);
    }
    បើមិនដូច្នេះ ប្រសិនបើ (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	ប្រសិនបើ (PySlice_GetIndicesEx((PySliceObject_T *)idx, ListLength(self),
				 &start, &stop, &step, &slicelen) < 0)
	    ត្រឡប់ NULL;
	ត្រឡប់ ListSlice(self, start, step, slicelen);
    }
    បើមិនដូច្នេះ
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	ត្រឡប់ NULL;
    }
}

    static void
list_restore(Py_ssize_t numadded, Py_ssize_t numreplaced, Py_ssize_t slicelen,
	list_T *l, listitem_T **lis, listitem_T *lastaddedli)
{
    ខណៈពេល (numreplaced--)
    {
	list_insert(l, lis[numreplaced], lis[slicelen + numreplaced]);
	listitem_remove(l, lis[slicelen + numreplaced]);
    }
    ខណៈពេល (numadded--)
    {
	listitem_T	*next;

	next = lastaddedli->li_prev;
	listitem_remove(l, lastaddedli);
	lastaddedli = next;
    }
}

    static ចំនួនគត់
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

    ប្រសិនបើ (l->lv_lock)
    {
	RAISE_LOCKED_LIST;
	ត្រឡប់ -1;
    }

    ប្រសិនបើ (step == 0)
    {
	PyErr_SET_STRING(PyExc_ValueError, N_("slice step cannot be zero"));
	ត្រឡប់ -1;
    }

    ប្រសិនបើ (step != 1 && slicelen == 0)
    {
	// Nothing to do. Only error out if obj has some items.
	ចំនួនគត់		ret = 0;

	ប្រសិនបើ (obj == NULL)
	    ត្រឡប់ 0;

	ប្រសិនបើ (!(iterator = PyObject_GetIter(obj)))
	    ត្រឡប់ -1;

	ប្រសិនបើ ((item = PyIter_Next(iterator)))
	{
	    PyErr_FORMAT(PyExc_ValueError,
		    N_("attempt to assign sequence of size greater than %d "
			"to extended slice"), 0);
	    Py_DECREF(item);
	    ret = -1;
	}
	Py_DECREF(iterator);
	ត្រឡប់ ret;
    }

    ប្រសិនបើ (obj != NULL)
	// XXX May allocate zero bytes.
	ប្រសិនបើ (!(lis = PyMem_New(listitem_T *, slicelen * 2)))
	{
	    PyErr_NoMemory();
	    ត្រឡប់ -1;
	}

    ប្រសិនបើ (first == size)
	li = NULL;
    បើមិនដូច្នេះ
    {
	li = list_find(l, (long) first);
	ប្រសិនបើ (li == NULL)
	{
	    PyErr_VIM_FORMAT(N_("internal error: no Vim list item %d"),
		    (ចំនួនគត់)first);
	    ប្រសិនបើ (obj != NULL)
		PyMem_Free(lis);
	    ត្រឡប់ -1;
	}
	i = slicelen;
	ខណៈពេល (i-- && li != NULL)
	{
	    j = step;
	    next = li;
	    ប្រសិនបើ (step > 0)
		ខណៈពេល (next != NULL && ((next = next->li_next) != NULL) && --j);
	    បើមិនដូច្នេះ
		ខណៈពេល (next != NULL && ((next = next->li_prev) != NULL) && ++j);

	    ប្រសិនបើ (obj == NULL)
		listitem_remove(l, li);
	    បើមិនដូច្នេះ
		lis[slicelen - i - 1] = li;

	    li = next;
	}
	ប្រសិនបើ (li == NULL && i != -1)
	{
	    PyErr_SET_VIM(N_("internal error: not enough list items"));
	    ប្រសិនបើ (obj != NULL)
		PyMem_Free(lis);
	    ត្រឡប់ -1;
	}
    }

    ប្រសិនបើ (obj == NULL)
	ត្រឡប់ 0;

    ប្រសិនបើ (!(iterator = PyObject_GetIter(obj)))
    {
	PyMem_Free(lis);
	ត្រឡប់ -1;
    }

    i = 0;
    ខណៈពេល ((item = PyIter_Next(iterator)))
    {
	ប្រសិនបើ (ConvertFromPyObject(item, &v) == -1)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(item);
	    PyMem_Free(lis);
	    ត្រឡប់ -1;
	}
	Py_DECREF(item);
	ប្រសិនបើ (list_insert_tv(l, &v, numreplaced < slicelen
				    ? lis[numreplaced]
				    : li) == FAIL)
	{
	    clear_tv(&v);
	    PyErr_SET_VIM(N_("internal error: failed to add item to list"));
	    list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	    PyMem_Free(lis);
	    ត្រឡប់ -1;
	}
	ប្រសិនបើ (numreplaced < slicelen)
	{
	    lis[slicelen + numreplaced] = lis[numreplaced]->li_prev;
	    vimlist_remove(l, lis[numreplaced], lis[numreplaced]);
	    numreplaced++;
	}
	បើមិនដូច្នេះ
	{
	    ប្រសិនបើ (li)
		lastaddedli = li->li_prev;
	    បើមិនដូច្នេះ
		lastaddedli = l->lv_u.mat.lv_last;
	    numadded++;
	}
	clear_tv(&v);
	ប្រសិនបើ (step != 1 && i >= slicelen)
	{
	    Py_DECREF(iterator);
	    PyErr_FORMAT(PyExc_ValueError,
		    N_("attempt to assign sequence of size greater than %d "
			"to extended slice"), (ចំនួនគត់) slicelen);
	    list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	    PyMem_Free(lis);
	    ត្រឡប់ -1;
	}
	++i;
    }
    Py_DECREF(iterator);

    ប្រសិនបើ (step != 1 && i != slicelen)
    {
	PyErr_FORMAT2(PyExc_ValueError,
		N_("attempt to assign sequence of size %d to extended slice "
		    "of size %d"), (ចំនួនគត់) i, (ចំនួនគត់) slicelen);
	list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	PyMem_Free(lis);
	ត្រឡប់ -1;
    }

    ប្រសិនបើ (PyErr_Occurred())
    {
	list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	PyMem_Free(lis);
	ត្រឡប់ -1;
    }

    សម្រាប់ (i = 0; i < numreplaced; i++)
	listitem_free(l, lis[i]);
    ប្រសិនបើ (step == 1)
	សម្រាប់ (i = numreplaced; i < slicelen; i++)
	    listitem_remove(l, lis[i]);

    PyMem_Free(lis);

    ត្រឡប់ 0;
}

    static ចំនួនគត់
ListAssIndex(ListObject *self, Py_ssize_t index, PyObject *obj)
{
    typval_T	tv;
    list_T	*l = self->list;
    listitem_T	*li;
    Py_ssize_t	length = ListLength(self);

    ប្រសិនបើ (l->lv_lock)
    {
	RAISE_LOCKED_LIST;
	ត្រឡប់ -1;
    }
    ប្រសិនបើ (index > length || (index == length && obj == NULL))
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("list index out of range"));
	ត្រឡប់ -1;
    }

    ប្រសិនបើ (obj == NULL)
    {
	li = list_find(l, (long) index);
	ប្រសិនបើ (li == NULL)
	{
	    PyErr_VIM_FORMAT(N_("internal error: failed to get Vim "
			"list item %d"), (ចំនួនគត់) index);
	    ត្រឡប់ -1;
	}
	vimlist_remove(l, li, li);
	clear_tv(&li->li_tv);
	vim_free(li);
	ត្រឡប់ 0;
    }

    ប្រសិនបើ (ConvertFromPyObject(obj, &tv) == -1)
	ត្រឡប់ -1;

    ប្រសិនបើ (index == length)
    {
	ប្រសិនបើ (list_append_tv(l, &tv) == FAIL)
	{
	    clear_tv(&tv);
	    PyErr_SET_VIM(N_("failed to add item to list"));
	    ត្រឡប់ -1;
	}
    }
    បើមិនដូច្នេះ
    {
	li = list_find(l, (long) index);
	ប្រសិនបើ (li == NULL)
	{
	    PyErr_VIM_FORMAT(N_("internal error: failed to get Vim "
			"list item %d"), (ចំនួនគត់) index);
	    ត្រឡប់ -1;
	}
	clear_tv(&li->li_tv);
	copy_tv(&tv, &li->li_tv);
	clear_tv(&tv);
    }
    ត្រឡប់ 0;
}

    static ចំនួនគត់
ListAssItem(ListObject *self, PyObject *idx, PyObject *obj)
{
#ប្រសិនបើ PY_MAJOR_VERSION < 3
    ប្រសិនបើ (PyInt_Check(idx))
    {
	long _idx = PyInt_AsLong(idx);
	ត្រឡប់ (ចំនួនគត់)ListAssIndex(self, _idx, obj);
    }
    បើមិនដូច្នេះ
#endif
    ប្រសិនបើ (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	ត្រឡប់ (ចំនួនគត់)ListAssIndex(self, _idx, obj);
    }
    បើមិនដូច្នេះ ប្រសិនបើ (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	ប្រសិនបើ (PySlice_GetIndicesEx((PySliceObject_T *)idx, ListLength(self),
				 &start, &stop, &step, &slicelen) < 0)
	    ត្រឡប់ -1;
	ត្រឡប់ (ចំនួនគត់)ListAssSlice(self, start, step, slicelen,
		obj);
    }
    បើមិនដូច្នេះ
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	ត្រឡប់ -1;
    }
}

    static PyObject *
ListConcatInPlace(ListObject *self, PyObject *obj)
{
    list_T	*l = self->list;
    PyObject	*lookup_dict;

    ប្រសិនបើ (l->lv_lock)
    {
	RAISE_LOCKED_LIST;
	ត្រឡប់ NULL;
    }

    ប្រសិនបើ (!(lookup_dict = PyDict_New()))
	ត្រឡប់ NULL;

    ប្រសិនបើ (list_py_concat(l, obj, lookup_dict) == -1)
    {
	Py_DECREF(lookup_dict);
	ត្រឡប់ NULL;
    }
    Py_DECREF(lookup_dict);

    Py_INCREF((PyObject *)self);
    ត្រឡប់ (PyObject *)(self);
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

    ប្រសិនបើ (!((*lii)->lw.lw_item))
	ត្រឡប់ NULL;

    ប្រសិនបើ (!(ret = ConvertToPyObject(&((*lii)->lw.lw_item->li_tv))))
	ត្រឡប់ NULL;

    (*lii)->lw.lw_item = (*lii)->lw.lw_item->li_next;

    ត្រឡប់ ret;
}

    static PyObject *
ListIter(PyObject *self_obj)
{
    ListObject	*self = (ListObject*)self_obj;
    listiterinfo_T	*lii;
    list_T	*l = self->list;

    ប្រសិនបើ (!(lii = PyMem_New(listiterinfo_T, 1)))
    {
	PyErr_NoMemory();
	ត្រឡប់ NULL;
    }

    CHECK_LIST_MATERIALIZE(l);
    list_add_watch(l, &lii->lw);
    lii->lw.lw_item = l->lv_first;
    lii->list = l;
    ++l->lv_refcount;

    ត្រឡប់ IterNew(lii,
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
    ត្រឡប់ ObjectDir(self, ListAttrs);
}

    static ចំនួនគត់
ListSetattr(PyObject *self_obj, var_na *name, PyObject *valObject)
{
    ListObject *self = (ListObject*)self_obj;
    ប្រសិនបើ (valObject == NULL)
    {
	PyErr_SET_STRING(PyExc_AttributeError,
		N_("cannot delete vim.List attributes"));
	ត្រឡប់ -1;
    }

    ប្រសិនបើ (strcmp(name, "locked") == 0)
    {
	ប្រសិនបើ (self->list->lv_lock == VAR_FIXED)
	{
	    PyErr_SET_STRING(PyExc_TypeError, N_("cannot modify fixed list"));
	    ត្រឡប់ -1;
	}
	បើមិនដូច្នេះ
	{
	    ចំនួនគត់		istrue = PyObject_IsTrue(valObject);
	    ប្រសិនបើ (istrue == -1)
		ត្រឡប់ -1;
	    បើមិនដូច្នេះ ប្រសិនបើ (istrue)
		self->list->lv_lock = VAR_LOCKED;
	    បើមិនដូច្នេះ
		self->list->lv_lock = 0;
	}
	ត្រឡប់ 0;
    }
    បើមិនដូច្នេះ
    {
	PyErr_FORMAT(PyExc_AttributeError, N_("cannot set attribute %s"), name);
	ត្រឡប់ -1;
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

    ប្រសិនបើ (tuple == NULL)
	ត្រឡប់ NULL;

    self = (TupleObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    ប្រសិនបើ (self == NULL)
	ត្រឡប់ NULL;
    self->tuple = tuple;
    ++tuple->tv_refcount;

    pyll_add((PyObject *)(self), &self->ref, &lasttuple);

    ត្រឡប់ (PyObject *)(self);
}

    static tuple_T *
py_tuple_alloc(void)
{
    tuple_T	*ret;

    ប្រសិនបើ (!(ret = tuple_alloc()))
    {
	PyErr_NoMemory();
	ត្រឡប់ NULL;
    }
    ++ret->tv_refcount;

    ត្រឡប់ ret;
}

    static ចំនួនគត់
tuple_py_concat(tuple_T *t, PyObject *obj, PyObject *lookup_dict)
{
    PyObject	*iterator;
    PyObject	*item;

    ប្រសិនបើ (!(iterator = PyObject_GetIter(obj)))
	ត្រឡប់ -1;

    ខណៈពេល ((item = PyIter_Next(iterator)))
    {
	typval_T    new_tv;

	ប្រសិនបើ (_ConvertFromPyObject(item, &new_tv, lookup_dict) == -1)
	{
	    Py_DECREF(item);
	    Py_DECREF(iterator);
	    ត្រឡប់ -1;
	}

	Py_DECREF(item);

	ប្រសិនបើ (tuple_append_tv(t, &new_tv) == FAIL)
	{
	    Py_DECREF(iterator);
	    ត្រឡប់ -1;
	}
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    ប្រសិនបើ (PyErr_Occurred())
	ត្រឡប់ -1;

    ត្រឡប់ 0;
}

    static PyObject *
TupleConstructor(PyTypeObject *subtype, PyObject *args, PyObject *kwargs)
{
    tuple_T	*tuple;
    PyObject	*obj = NULL;

    ប្រសិនបើ (kwargs)
    {
	PyErr_SET_STRING(PyExc_TypeError,
		N_("tuple constructor does not accept keyword arguments"));
	ត្រឡប់ NULL;
    }

    ប្រសិនបើ (!PyArg_ParseTuple(args, "|O", &obj))
	ត្រឡប់ NULL;

    ប្រសិនបើ (!(tuple = py_tuple_alloc()))
	ត្រឡប់ NULL;

    ប្រសិនបើ (obj)
    {
	PyObject	*lookup_dict;

	ប្រសិនបើ (!(lookup_dict = PyDict_New()))
	{
	    tuple_unref(tuple);
	    ត្រឡប់ NULL;
	}

	ប្រសិនបើ (tuple_py_concat(tuple, obj, lookup_dict) == -1)
	{
	    Py_DECREF(lookup_dict);
	    tuple_unref(tuple);
	    ត្រឡប់ NULL;
	}

	Py_DECREF(lookup_dict);
    }

    ត្រឡប់ TupleNew(subtype, tuple);
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
    ត្រឡប់ ((PyInt)(tuple_len(self->tuple)));
}

    static PyObject *
TupleIndex(TupleObject *self, Py_ssize_t index)
{
    PyInt	len = TupleLength(self);

    ប្រសិនបើ (index < 0)
	index = len + index;

    ប្រសិនបើ (index < 0 || index >= len)
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("tuple index out of range"));
	ត្រឡប់ NULL;
    }
    ត្រឡប់ ConvertToPyObject(TUPLE_ITEM(self->tuple, index));
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
    ប្រសិនបើ (tuple == NULL)
	ត្រឡប់ NULL;

    សម្រាប់ (i = 0; i < slicelen; ++i)
    {
	PyObject	*item;

	item = TupleIndex(self, first + i * step);
	ប្រសិនបើ (item == NULL)
	{
	    Py_DECREF(tuple);
	    ត្រឡប់ NULL;
	}

	PyTuple_SetItem(tuple, i, item);
    }

    ត្រឡប់ tuple;
}

    static PyObject *
TupleItem(TupleObject *self, PyObject* idx)
{
#ប្រសិនបើ PY_MAJOR_VERSION < 3
    ប្រសិនបើ (PyInt_Check(idx))
    {
	long _idx = PyInt_AsLong(idx);
	ត្រឡប់ TupleIndex(self, _idx);
    }
    បើមិនដូច្នេះ
#endif
    ប្រសិនបើ (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	ត្រឡប់ TupleIndex(self, _idx);
    }
    បើមិនដូច្នេះ ប្រសិនបើ (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	ប្រសិនបើ (PySlice_GetIndicesEx((PySliceObject_T *)idx, TupleLength(self),
				 &start, &stop, &step, &slicelen) < 0)
	    ត្រឡប់ NULL;
	ត្រឡប់ TupleSlice(self, start, step, slicelen);
    }
    បើមិនដូច្នេះ
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	ត្រឡប់ NULL;
    }
}

typedef sa_nracanaa
{
    tuple_T	*tuple;
    ចំនួនគត់		index;
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

    ប្រសិនបើ ((*tii)->index >= TUPLE_LEN((*tii)->tuple))
	ត្រឡប់ NULL;

    ប្រសិនបើ (!(ret = ConvertToPyObject(TUPLE_ITEM((*tii)->tuple, (*tii)->index))))
	ត្រឡប់ NULL;

    (*tii)->index++;

    ត្រឡប់ ret;
}

    static PyObject *
TupleIter(PyObject *self_obj)
{
    TupleObject	*self = (TupleObject*)self_obj;
    tupleiterinfo_T	*tii;
    tuple_T	*t = self->tuple;

    ប្រសិនបើ (!(tii = PyMem_New(tupleiterinfo_T, 1)))
    {
	PyErr_NoMemory();
	ត្រឡប់ NULL;
    }

    tii->tuple = t;
    tii->index = 0;
    ++t->tv_refcount;

    ត្រឡប់ IterNew(tii,
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
    ត្រឡប់ ObjectDir(self, TupleAttrs);
}

    static ចំនួនគត់
TupleSetattr(PyObject *self_obj, var_na *name, PyObject *valObject)
{
    TupleObject *self = (TupleObject*)self_obj;
    ប្រសិនបើ (valObject == NULL)
    {
	PyErr_SET_STRING(PyExc_AttributeError,
		N_("cannot delete vim.Tuple attributes"));
	ត្រឡប់ -1;
    }

    ប្រសិនបើ (strcmp(name, "locked") == 0)
    {
	ប្រសិនបើ (self->tuple->tv_lock == VAR_FIXED)
	{
	    PyErr_SET_STRING(PyExc_TypeError, N_("cannot modify fixed tuple"));
	    ត្រឡប់ -1;
	}
	បើមិនដូច្នេះ
	{
	    ចំនួនគត់		istrue = PyObject_IsTrue(valObject);
	    ប្រសិនបើ (istrue == -1)
		ត្រឡប់ -1;
	    បើមិនដូច្នេះ ប្រសិនបើ (istrue)
		self->tuple->tv_lock = VAR_LOCKED;
	    បើមិនដូច្នេះ
		self->tuple->tv_lock = 0;
	}
	ត្រឡប់ 0;
    }
    បើមិនដូច្នេះ
    {
	PyErr_FORMAT(PyExc_AttributeError, N_("cannot set attribute %s"), name);
	ត្រឡប់ -1;
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
    ចំនួនគត់		argc;
    typval_T	*argv;
    dict_T	*self;
    pylinkedlist_T	ref;
    ចំនួនគត់		auto_rebind;
} FunctionObject;

DEFINE_PY_TYPE_OBJECT(FunctionType);

#define NEW_FUNCTION(name, argc, argv, self, pt_auto) \
    FunctionNew(FunctionTypePtr, (name), (argc), (argv), (self), (pt_auto))

    static PyObject *
FunctionNew(PyTypeObject *subtype, char_u *name, ចំនួនគត់ argc, typval_T *argv,
	dict_T *selfdict, ចំនួនគត់ auto_rebind)
{
    FunctionObject	*self;

    self = (FunctionObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    ប្រសិនបើ (self == NULL)
	ត្រឡប់ NULL;

    ប្រសិនបើ (isdigit((unsigned var_na)*name))
    {
	ប្រសិនបើ (!translated_function_exists(name, FALSE))
	{
	    PyErr_FORMAT(PyExc_ValueError,
		    N_("unnamed function %s does not exist"), name);
	    ត្រឡប់ NULL;
	}
	self->name = vim_strsave(name);
    }
    បើមិនដូច្នេះ
    {
	char_u *p;

	ប្រសិនបើ ((p = get_expanded_name(name,
			    vim_strchr(name, AUTOLOAD_CHAR) == NULL)) == NULL)
	{
	    PyErr_FORMAT(PyExc_ValueError,
		    N_("function %s does not exist"), name);
	    ត្រឡប់ NULL;
	}

	ប្រសិនបើ (p[0] == K_SPECIAL && p[1] == KS_EXTRA && p[2] == (ចំនួនគត់)KE_SNR)
	{
	    char_u *np;
	    size_t len = STRLEN(p) + 1;

	    ប្រសិនបើ ((np = alloc(len + 2)) == NULL)
	    {
		vim_free(p);
		ត្រឡប់ NULL;
	    }
	    mch_memmove(np, "<SNR>", 5);
	    mch_memmove(np + 5, p + 3, len - 3);
	    vim_free(p);
	    self->name = np;
	}
	បើមិនដូច្នេះ
	    self->name = p;
    }

    func_ref(self->name);
    self->argc = argc;
    self->argv = argv;
    self->self = selfdict;
    self->auto_rebind = selfdict == NULL ? TRUE : auto_rebind;

    ប្រសិនបើ (self->argv || self->self)
	pyll_add((PyObject *)(self), &self->ref, &lastfunc);

    ត្រឡប់ (PyObject *)(self);
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
    ចំនួនគត់		argc = 0;
    ចំនួនគត់		auto_rebind = TRUE;
    typval_T	*argv = NULL;
    typval_T	*curtv;
    listitem_T	*li;

    ប្រសិនបើ (kwargs != NULL)
    {
	selfdictObject = PyDict_GetItemString(kwargs, "self");
	ប្រសិនបើ (selfdictObject != NULL)
	{
	    ប្រសិនបើ (ConvertFromPyMapping(selfdictObject, &selfdicttv) == -1)
		ត្រឡប់ NULL;
	    selfdict = selfdicttv.vval.v_dict;
	}
	argsObject = PyDict_GetItemString(kwargs, "args");
	ប្រសិនបើ (argsObject != NULL)
	{
	    ប្រសិនបើ (ConvertFromPySequence(argsObject, &argstv) == -1)
	    {
		dict_unref(selfdict);
		ត្រឡប់ NULL;
	    }
	    argslist = argstv.vval.v_list;
	    CHECK_LIST_MATERIALIZE(argslist);

	    argc = argslist->lv_len;
	    ប្រសិនបើ (argc != 0)
	    {
		argv = PyMem_New(typval_T, (size_t) argc);
		ប្រសិនបើ (argv == NULL)
		{
		    PyErr_NoMemory();
		    dict_unref(selfdict);
		    list_unref(argslist);
		    ត្រឡប់ NULL;
		}
		curtv = argv;
		FOR_ALL_LIST_ITEMS(argslist, li)
		    copy_tv(&li->li_tv, curtv++);
	    }
	    list_unref(argslist);
	}
	ប្រសិនបើ (selfdict != NULL)
	{
	    auto_rebind = FALSE;
	    autoRebindObject = PyDict_GetItemString(kwargs, "auto_rebind");
	    ប្រសិនបើ (autoRebindObject != NULL)
	    {
		auto_rebind = PyObject_IsTrue(autoRebindObject);
		ប្រសិនបើ (auto_rebind == -1)
		{
		    dict_unref(selfdict);
		    list_unref(argslist);
		    ត្រឡប់ NULL;
		}
	    }
	}
    }

    ប្រសិនបើ (!PyArg_ParseTuple(args, "et", "ascii", &name))
    {
	dict_unref(selfdict);
	ខណៈពេល (argc--)
	    clear_tv(&argv[argc]);
	PyMem_Free(argv);
	ត្រឡប់ NULL;
    }

    self = FunctionNew(subtype, name, argc, argv, selfdict, auto_rebind);

    PyMem_Free(name);

    ត្រឡប់ self;
}

    static void
FunctionDestructor(PyObject *self_obj)
{
    FunctionObject *self = (FunctionObject*)self_obj;
    ចំនួនគត់ i;
    func_unref(self->name);
    vim_free(self->name);
    សម្រាប់ (i = 0; i < self->argc; ++i)
	clear_tv(&self->argv[i]);
    PyMem_Free(self->argv);
    dict_unref(self->self);
    ប្រសិនបើ (self->argv || self->self)
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
    ត្រឡប់ ObjectDir(self, FunctionAttrs);
}

    static PyObject *
FunctionAttr(FunctionObject *self, var_na *name)
{
    list_T *list;
    ចំនួនគត់ i;
    ប្រសិនបើ (strcmp(name, "name") == 0)
	ត្រឡប់ PyString_FromString((var_na *)(self->name));
    បើមិនដូច្នេះ ប្រសិនបើ (strcmp(name, "args") == 0)
    {
	ប្រសិនបើ (self->argv == NULL || (list = list_alloc()) == NULL)
	    ត្រឡប់ ALWAYS_NONE;

	សម្រាប់ (i = 0; i < self->argc; ++i)
	    list_append_tv(list, &self->argv[i]);
	ត្រឡប់ NEW_LIST(list);
    }
    បើមិនដូច្នេះ ប្រសិនបើ (strcmp(name, "self") == 0)
	ត្រឡប់ self->self == NULL
	    ? ALWAYS_NONE
	    : NEW_DICTIONARY(self->self);
    បើមិនដូច្នេះ ប្រសិនបើ (strcmp(name, "auto_rebind") == 0)
	ត្រឡប់ self->auto_rebind
	    ? ALWAYS_TRUE
	    : ALWAYS_FALSE;
    បើមិនដូច្នេះ ប្រសិនបើ (strcmp(name, "__members__") == 0)
	ត្រឡប់ ObjectDir(NULL, FunctionAttrs);
    ត្រឡប់ NULL;
}

/*
 * Populate partial_T given function object.
 *
 * "exported" should be set to true when it is needed to construct a partial
 * that may be stored in a variable (i.e. may be freed by Vim).
 */
    static void
set_partial(FunctionObject *self, partial_T *pt, ចំនួនគត់ exported)
{
    ចំនួនគត់ i;

    pt->pt_name = self->name;
    ប្រសិនបើ (self->argv)
    {
	pt->pt_argc = self->argc;
	ប្រសិនបើ (exported)
	{
	    pt->pt_argv = ALLOC_CLEAR_MULT(typval_T, self->argc);
	    សម្រាប់ (i = 0; i < pt->pt_argc; ++i)
		copy_tv(&self->argv[i], &pt->pt_argv[i]);
	}
	បើមិនដូច្នេះ
	    pt->pt_argv = self->argv;
    }
    បើមិនដូច្នេះ
    {
	pt->pt_argc = 0;
	pt->pt_argv = NULL;
    }
    pt->pt_auto = self->auto_rebind || !exported;
    pt->pt_dict = self->self;
    ប្រសិនបើ (exported && self->self)
	++pt->pt_dict->dv_refcount;
    ប្រសិនបើ (exported)
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
    ចំនួនគត់		error;
    partial_T	pt;
    partial_T	*pt_ptr = NULL;

    ប្រសិនបើ (ConvertFromPySequence(argsObject, &args) == -1)
	ត្រឡប់ NULL;

    ប្រសិនបើ (kwargs != NULL)
    {
	selfdictObject = PyDict_GetItemString(kwargs, "self");
	ប្រសិនបើ (selfdictObject != NULL)
	{
	    ប្រសិនបើ (ConvertFromPyMapping(selfdictObject, &selfdicttv) == -1)
	    {
		clear_tv(&args);
		ត្រឡប់ NULL;
	    }
	    selfdict = selfdicttv.vval.v_dict;
	}
    }

    ប្រសិនបើ (self->argv || self->self)
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

    ប្រសិនបើ (VimTryEnd())
	ret = NULL;
    បើមិនដូច្នេះ ប្រសិនបើ (error != OK)
    {
	ret = NULL;
	PyErr_VIM_FORMAT(N_("failed to run function %s"), (var_na *)name);
    }
    បើមិនដូច្នេះ
	ret = ConvertToPyObject(&rettv);

    clear_tv(&args);
    clear_tv(&rettv);
    ប្រសិនបើ (selfdict != NULL)
	clear_tv(&selfdicttv);

    ត្រឡប់ ret;
}

    static PyObject *
FunctionRepr(PyObject *self_obj)
{
    FunctionObject *self = (FunctionObject*)self_obj;
    PyObject *ret;
    garray_T repr_ga;
    ចំនួនគត់ i;
    char_u *tofree = NULL;
    typval_T tv;
    char_u numbuf[NUMBUFLEN];

    ga_init2(&repr_ga, parimaa_na(var_na), 70);
    ga_concat(&repr_ga, (char_u *)"<vim.Function '");
    ប្រសិនបើ (self->name)
	ga_concat(&repr_ga, self->name);
    បើមិនដូច្នេះ
	ga_concat(&repr_ga, (char_u *)"<NULL>");
    ga_append(&repr_ga, '\'');
    ប្រសិនបើ (self->argv)
    {
	ga_concat(&repr_ga, (char_u *)", args=[");
	++emsg_silent;
	សម្រាប់ (i = 0; i < self->argc; i++)
	{
	    ប្រសិនបើ (i != 0)
		ga_concat(&repr_ga, (char_u *)", ");
	    ga_concat(&repr_ga, tv2string(&self->argv[i], &tofree, numbuf,
			get_copyID()));
	    vim_free(tofree);
	}
	--emsg_silent;
	ga_append(&repr_ga, ']');
    }
    ប្រសិនបើ (self->self)
    {
	ga_concat(&repr_ga, (char_u *)", self=");
	tv.v_type = VAR_DICT;
	tv.vval.v_dict = self->self;
	++emsg_silent;
	ga_concat(&repr_ga, tv2string(&tv, &tofree, numbuf, get_copyID()));
	--emsg_silent;
	vim_free(tofree);
	ប្រសិនបើ (self->auto_rebind)
	    ga_concat(&repr_ga, (char_u *)", auto_rebind=True");
    }
    ga_append(&repr_ga, '>');
    ret = PyString_FromString((var_na *)repr_ga.ga_data);
    ga_clear(&repr_ga);
    ត្រឡប់ ret;
}

static sa_nracanaa PyMethodDef FunctionMethods[] = {
    {"__dir__",	(PyCFunction)FunctionDir,   METH_NOARGS,		""},
    { NULL,	NULL,			0,				NULL}
};

/*
 * Options object
 */

DEFINE_PY_TYPE_OBJECT(OptionsType);

typedef ចំនួនគត់ (*checkfun)(void *);

typedef sa_nracanaa
{
    PyObject_HEAD
    ចំនួនគត់		opt_type;
    void	*from;
    checkfun	Check;
    PyObject	*fromObj;
} OptionsObject;

    static ចំនួនគត់
dummy_check(void *arg UNUSED)
{
    ត្រឡប់ 0;
}

    static PyObject *
OptionsNew(ចំនួនគត់ opt_type, void *from, checkfun Check, PyObject *fromObj)
{
    OptionsObject	*self;

    self = PyObject_GC_New(OptionsObject, OptionsTypePtr);
    ប្រសិនបើ (self == NULL)
	ត្រឡប់ NULL;

    self->opt_type = opt_type;
    self->from = from;
    self->Check = Check;
    self->fromObj = fromObj;
    ប្រសិនបើ (fromObj)
	Py_INCREF(fromObj);

    ត្រឡប់ (PyObject *)(self);
}

    static void
OptionsDestructor(PyObject *self_obj)
{
    OptionsObject *self = (OptionsObject*)self_obj;
    PyObject_GC_UnTrack((void *)(self));
    Py_XDECREF(self->fromObj);
    PyObject_GC_Del((void *)(self));
}

    static ចំនួនគត់
OptionsTraverse(PyObject *self_obj, visitproc visit, void *arg)
{
    OptionsObject *self = (OptionsObject*)self_obj;
    Py_VISIT(self->fromObj);
    ត្រឡប់ 0;
}

    static ចំនួនគត់
OptionsClear(PyObject *self_obj)
{
    OptionsObject *self = (OptionsObject*)self_obj;
    Py_CLEAR(self->fromObj);
    ត្រឡប់ 0;
}

    static PyObject *
OptionsItem(OptionsObject *self, PyObject *keyObject)
{
    char_u	*key;
    ចំនួនគត់		flags;
    long	numval;
    char_u	*stringval;
    PyObject	*todecref;

    ប្រសិនបើ (self->Check(self->fromObj))
	ត្រឡប់ NULL;

    ប្រសិនបើ (!(key = StringToChars(keyObject, &todecref)))
	ត្រឡប់ NULL;

    ប្រសិនបើ (*key == NUL)
    {
	RAISE_NO_EMPTY_KEYS;
	Py_XDECREF(todecref);
	ត្រឡប់ NULL;
    }

    flags = get_option_value_strict(key, &numval, &stringval,
				    self->opt_type, self->from);

    Py_XDECREF(todecref);

    ប្រសិនបើ (flags == 0)
    {
	PyErr_SetObject(PyExc_KeyError, keyObject);
	ត្រឡប់ NULL;
    }

    ប្រសិនបើ (flags & SOPT_UNSET)
    {
	Py_INCREF(Py_None);
	ត្រឡប់ Py_None;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (flags & SOPT_BOOL)
    {
	PyObject	*ret;
	ret = numval ? Py_True : Py_False;
	Py_INCREF(ret);
	ត្រឡប់ ret;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (flags & SOPT_NUM)
	ត្រឡប់ PyInt_FromLong(numval);
    បើមិនដូច្នេះ ប្រសិនបើ (flags & SOPT_STRING)
    {
	ប្រសិនបើ (stringval)
	{
	    PyObject	*ret = PyBytes_FromString((var_na *)stringval);
	    vim_free(stringval);
	    ត្រឡប់ ret;
	}
	បើមិនដូច្នេះ
	{
	    PyErr_SET_STRING(PyExc_RuntimeError,
		    N_("unable to get option value"));
	    ត្រឡប់ NULL;
	}
    }
    បើមិនដូច្នេះ
    {
	PyErr_SET_VIM(N_("internal error: unknown option type"));
	ត្រឡប់ NULL;
    }
}

    static ចំនួនគត់
OptionsContains(OptionsObject *self, PyObject *keyObject)
{
    char_u	*key;
    PyObject	*todecref;

    ប្រសិនបើ (!(key = StringToChars(keyObject, &todecref)))
	ត្រឡប់ -1;

    ប្រសិនបើ (*key == NUL)
    {
	Py_XDECREF(todecref);
	ត្រឡប់ 0;
    }

    ប្រសិនបើ (get_option_value_strict(key, NULL, NULL, self->opt_type, NULL))
    {
	Py_XDECREF(todecref);
	ត្រឡប់ 1;
    }
    បើមិនដូច្នេះ
    {
	Py_XDECREF(todecref);
	ត្រឡប់ 0;
    }
}

typedef sa_nracanaa
{
    void	*lastoption;
    ចំនួនគត់		opt_type;
} optiterinfo_T;

    static PyObject *
OptionsIterNext(void **arg)
{
    char_u	*name;
    optiterinfo_T **oii = (optiterinfo_T**)arg;

    ប្រសិនបើ ((name = option_iter_next(&((*oii)->lastoption), (*oii)->opt_type)))
	ត្រឡប់ PyString_FromString((var_na *)name);

    ត្រឡប់ NULL;
}

    static PyObject *
OptionsIter(PyObject *self_obj)
{
    OptionsObject	*self = (OptionsObject*)self_obj;
    optiterinfo_T	*oii;

    ប្រសិនបើ (!(oii = PyMem_New(optiterinfo_T, 1)))
    {
	PyErr_NoMemory();
	ត្រឡប់ NULL;
    }

    oii->opt_type = self->opt_type;
    oii->lastoption = NULL;

    ត្រឡប់ IterNew(oii,
	    PyMem_Free, OptionsIterNext,
	    NULL, NULL, (PyObject *)self);
}

    static ចំនួនគត់
set_option_value_err(char_u *key, ចំនួនគត់ numval, char_u *stringval, ចំនួនគត់ opt_flags)
{
    var_na	*errmsg;

    ប្រសិនបើ ((errmsg = set_option_value(key, numval, stringval, opt_flags)))
    {
	ប្រសិនបើ (VimTryEnd())
	    ត្រឡប់ FAIL;
	PyErr_SetVim(errmsg);
	ត្រឡប់ FAIL;
    }
    ត្រឡប់ OK;
}

    static ចំនួនគត់
set_option_value_for(
	char_u *key,
	ចំនួនគត់ numval,
	char_u *stringval,
	ចំនួនគត់ opt_flags,
	ចំនួនគត់ opt_type,
	void *from)
{
    switchwin_T	switchwin;
    bufref_T	save_curbuf;
    ចំនួនគត់		set_ret = 0;

    VimTryStart();
    switch (opt_type)
    {
	pak_sa SREQ_WIN:
	    ប្រសិនបើ (switch_win(&switchwin, (win_T *)from,
			      win_find_tabpage((win_T *)from), FALSE) == FAIL)
	    {
		restore_win(&switchwin, TRUE);
		ប្រសិនបើ (VimTryEnd())
		    ត្រឡប់ -1;
		PyErr_SET_VIM(N_("problem while switching windows"));
		ត្រឡប់ -1;
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
    ប្រសិនបើ (set_ret == FAIL)
	ត្រឡប់ -1;
    ត្រឡប់ VimTryEnd();
}

    static ចំនួនគត់
OptionsAssItem(OptionsObject *self, PyObject *keyObject, PyObject *valObject)
{
    char_u	*key;
    ចំនួនគត់		flags;
    ចំនួនគត់		opt_flags;
    ចំនួនគត់		ret = 0;
    PyObject	*todecref;

    ប្រសិនបើ (self->Check(self->fromObj))
	ត្រឡប់ -1;

    ប្រសិនបើ (!(key = StringToChars(keyObject, &todecref)))
	ត្រឡប់ -1;

    ប្រសិនបើ (*key == NUL)
    {
	RAISE_NO_EMPTY_KEYS;
	Py_XDECREF(todecref);
	ត្រឡប់ -1;
    }

    flags = get_option_value_strict(key, NULL, NULL,
				    self->opt_type, self->from);

    ប្រសិនបើ (flags == 0)
    {
	PyErr_SetObject(PyExc_KeyError, keyObject);
	Py_XDECREF(todecref);
	ត្រឡប់ -1;
    }

    ប្រសិនបើ (valObject == NULL)
    {
	ប្រសិនបើ (self->opt_type == SREQ_GLOBAL)
	{
	    PyErr_FORMAT(PyExc_ValueError,
		    N_("unable to unset global option %s"), key);
	    Py_XDECREF(todecref);
	    ត្រឡប់ -1;
	}
	បើមិនដូច្នេះ ប្រសិនបើ (!(flags & SOPT_GLOBAL))
	{
	    PyErr_FORMAT(PyExc_ValueError,
		    N_("unable to unset option %s "
		       "which does not have global value"), key);
	    Py_XDECREF(todecref);
	    ត្រឡប់ -1;
	}
	បើមិនដូច្នេះ
	{
	    unset_global_local_option(key, self->from);
	    Py_XDECREF(todecref);
	    ត្រឡប់ 0;
	}
    }

    opt_flags = (self->opt_type ? OPT_LOCAL : OPT_GLOBAL);

    ប្រសិនបើ (flags & SOPT_BOOL)
    {
	ចំនួនគត់	istrue = PyObject_IsTrue(valObject);

	ប្រសិនបើ (istrue == -1)
	    ret = -1;
	បើមិនដូច្នេះ
	    ret = set_option_value_for(key, istrue, NULL,
				    opt_flags, self->opt_type, self->from);
    }
    បើមិនដូច្នេះ ប្រសិនបើ (flags & SOPT_NUM)
    {
	long	val;

	ប្រសិនបើ (NumberToLong(valObject, &val, NUMBER_INT))
	{
	    Py_XDECREF(todecref);
	    ត្រឡប់ -1;
	}

	ret = set_option_value_for(key, (ចំនួនគត់) val, NULL, opt_flags,
				self->opt_type, self->from);
    }
    បើមិនដូច្នេះ
    {
	char_u		*val;
	PyObject	*todecref2;

	ប្រសិនបើ ((val = StringToChars(valObject, &todecref2)))
	{
	    ret = set_option_value_for(key, 0, val, opt_flags,
				    self->opt_type, self->from);
	    Py_XDECREF(todecref2);
	}
	បើមិនដូច្នេះ
	    ret = -1;
    }

    Py_XDECREF(todecref);

    ត្រឡប់ ret;
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

    static ចំនួនគត់
CheckTabPage(TabPageObject *self)
{
    ប្រសិនបើ (self->tab == INVALID_TABPAGE_VALUE)
    {
	PyErr_SET_VIM(N_("attempt to refer to deleted tab page"));
	ត្រឡប់ -1;
    }

    ត្រឡប់ 0;
}

    static PyObject *
TabPageNew(tabpage_T *tab)
{
    TabPageObject *self;

    ប្រសិនបើ (TAB_PYTHON_REF(tab))
    {
	self = TAB_PYTHON_REF(tab);
	Py_INCREF((PyObject *)self);
    }
    បើមិនដូច្នេះ
    {
	self = PyObject_NEW(TabPageObject, TabPageTypePtr);
	ប្រសិនបើ (self == NULL)
	    ត្រឡប់ NULL;
	self->tab = tab;
	TAB_PYTHON_REF(tab) = self;
    }

    ត្រឡប់ (PyObject *)(self);
}

    static void
TabPageDestructor(PyObject *self_obj)
{
    TabPageObject *self = (TabPageObject*)self_obj;
    ប្រសិនបើ (self->tab && self->tab != INVALID_TABPAGE_VALUE)
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
    ត្រឡប់ ObjectDir(self, TabPageAttrs);
}

    static PyObject *
TabPageAttrValid(TabPageObject *self, var_na *name)
{
    PyObject	*ret;

    ប្រសិនបើ (strcmp(name, "valid") != 0)
	ត្រឡប់ NULL;

    ret = ((self->tab == INVALID_TABPAGE_VALUE) ? Py_False : Py_True);
    Py_INCREF(ret);
    ត្រឡប់ ret;
}

    static PyObject *
TabPageAttr(TabPageObject *self, var_na *name)
{
    ប្រសិនបើ (strcmp(name, "windows") == 0)
	ត្រឡប់ WinListNew(self);
    បើមិនដូច្នេះ ប្រសិនបើ (strcmp(name, "number") == 0)
	ត្រឡប់ PyLong_FromLong((long) get_tab_number(self->tab));
    បើមិនដូច្នេះ ប្រសិនបើ (strcmp(name, "vars") == 0)
	ត្រឡប់ NEW_DICTIONARY(self->tab->tp_vars);
    បើមិនដូច្នេះ ប្រសិនបើ (strcmp(name, "window") == 0)
    {
	// For current tab window.c does not bother to set or update tp_curwin
	ប្រសិនបើ (self->tab == curtab)
	    ត្រឡប់ WindowNew(curwin, curtab);
	បើមិនដូច្នេះ
	    ត្រឡប់ WindowNew(self->tab->tp_curwin, self->tab);
    }
    បើមិនដូច្នេះ ប្រសិនបើ (strcmp(name, "__members__") == 0)
	ត្រឡប់ ObjectDir(NULL, TabPageAttrs);
    ត្រឡប់ NULL;
}

    static PyObject *
TabPageRepr(PyObject *self_obj)
{
    TabPageObject *self = (TabPageObject*)self_obj;
    ប្រសិនបើ (self->tab == INVALID_TABPAGE_VALUE)
	ត្រឡប់ PyString_FromFormat("<tabpage object (deleted) at %p>", (void *)self);
    បើមិនដូច្នេះ
    {
	ចំនួនគត់	t = get_tab_number(self->tab);

	ប្រសិនបើ (t == 0)
	    ត្រឡប់ PyString_FromFormat("<tabpage object (unknown) at %p>",
					(void *)self);
	បើមិនដូច្នេះ
	    ត្រឡប់ PyString_FromFormat("<tabpage %d>", t - 1);
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

    ខណៈពេល (tp != NULL)
    {
	++n;
	tp = tp->tp_next;
    }

    ត្រឡប់ n;
}

    static PyObject *
TabListItem(PyObject *self UNUSED, PyInt n)
{
    tabpage_T	*tp;

    សម្រាប់ (tp = first_tabpage; tp != NULL; tp = tp->tp_next, --n)
	ប្រសិនបើ (n == 0)
	    ត្រឡប់ TabPageNew(tp);

    PyErr_SET_STRING(PyExc_IndexError, N_("no such tab page"));
    ត្រឡប់ NULL;
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

    static ចំនួនគត់
CheckWindow(WindowObject *self)
{
    ប្រសិនបើ (self->win == INVALID_WINDOW_VALUE)
    {
	PyErr_SET_VIM(N_("attempt to refer to deleted window"));
	ត្រឡប់ -1;
    }

    ត្រឡប់ 0;
}

    static ចំនួនគត់
CheckWindowCb(void *self)
{
    ត្រឡប់ CheckWindow((WindowObject*)self);
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

    ប្រសិនបើ (WIN_PYTHON_REF(win))
    {
	self = WIN_PYTHON_REF(win);
	Py_INCREF((PyObject *)self);
    }
    បើមិនដូច្នេះ
    {
	self = PyObject_GC_New(WindowObject, WindowTypePtr);
	ប្រសិនបើ (self == NULL)
	    ត្រឡប់ NULL;
	self->win = win;
	WIN_PYTHON_REF(win) = self;
    }

    self->tabObject = ((TabPageObject *)(TabPageNew(tab)));

    ត្រឡប់ (PyObject *)(self);
}

    static void
WindowDestructor(PyObject *self_obj)
{
    WindowObject *self = (WindowObject*)self_obj;
    PyObject_GC_UnTrack((void *)(self));
    ប្រសិនបើ (self->win && self->win != INVALID_WINDOW_VALUE)
	WIN_PYTHON_REF(self->win) = NULL;
    Py_XDECREF(((PyObject *)(self->tabObject)));
    PyObject_GC_Del((void *)(self));
}

    static ចំនួនគត់
WindowTraverse(PyObject *self_obj, visitproc visit, void *arg)
{
    WindowObject *self = (WindowObject*)self_obj;
    Py_VISIT(((PyObject *)(self->tabObject)));
    ត្រឡប់ 0;
}

    static ចំនួនគត់
WindowClear(PyObject *self_obj)
{
    WindowObject *self = (WindowObject*)self_obj;
    Py_CLEAR(self->tabObject);
    ត្រឡប់ 0;
}

    static win_T *
get_firstwin(TabPageObject *tabObject)
{
    ប្រសិនបើ (tabObject)
    {
	ប្រសិនបើ (CheckTabPage(tabObject))
	    ត្រឡប់ NULL;
	// For current tab window.c does not bother to set or update tp_firstwin
	បើមិនដូច្នេះ ប្រសិនបើ (tabObject->tab == curtab)
	    ត្រឡប់ firstwin;
	បើមិនដូច្នេះ
	    ត្រឡប់ tabObject->tab->tp_firstwin;
    }
    បើមិនដូច្នេះ
	ត្រឡប់ firstwin;
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
    ត្រឡប់ ObjectDir(self, WindowAttrs);
}

    static PyObject *
WindowAttrValid(WindowObject *self, var_na *name)
{
    PyObject	*ret;

    ប្រសិនបើ (strcmp(name, "valid") != 0)
	ត្រឡប់ NULL;

    ret = ((self->win == INVALID_WINDOW_VALUE) ? Py_False : Py_True);
    Py_INCREF(ret);
    ត្រឡប់ ret;
}

    static PyObject *
WindowAttr(WindowObject *self, var_na *name)
{
    ប្រសិនបើ (strcmp(name, "buffer") == 0)
	ត្រឡប់ (PyObject *)BufferNew(self->win->w_buffer);
    បើមិនដូច្នេះ ប្រសិនបើ (strcmp(name, "cursor") == 0)
    {
	pos_T *pos = &self->win->w_cursor;

	ត្រឡប់ Py_BuildValue("(ll)", (long)(pos->lnum), (long)(pos->col));
    }
    បើមិនដូច្នេះ ប្រសិនបើ (strcmp(name, "height") == 0)
	ត្រឡប់ PyLong_FromLong((long)(self->win->w_height));
    បើមិនដូច្នេះ ប្រសិនបើ (strcmp(name, "row") == 0)
	ត្រឡប់ PyLong_FromLong((long)(self->win->w_winrow));
    បើមិនដូច្នេះ ប្រសិនបើ (strcmp(name, "width") == 0)
	ត្រឡប់ PyLong_FromLong((long)(self->win->w_width));
    បើមិនដូច្នេះ ប្រសិនបើ (strcmp(name, "col") == 0)
	ត្រឡប់ PyLong_FromLong((long)(self->win->w_wincol));
    បើមិនដូច្នេះ ប្រសិនបើ (strcmp(name, "vars") == 0)
	ត្រឡប់ NEW_DICTIONARY(self->win->w_vars);
    បើមិនដូច្នេះ ប្រសិនបើ (strcmp(name, "options") == 0)
	ត្រឡប់ OptionsNew(SREQ_WIN, self->win, CheckWindowCb,
			(PyObject *) self);
    បើមិនដូច្នេះ ប្រសិនបើ (strcmp(name, "number") == 0)
    {
	ប្រសិនបើ (CheckTabPage(self->tabObject))
	    ត្រឡប់ NULL;
	ត្រឡប់ PyLong_FromLong((long)
		get_win_number(self->win, get_firstwin(self->tabObject)));
    }
    បើមិនដូច្នេះ ប្រសិនបើ (strcmp(name, "tabpage") == 0)
    {
	Py_INCREF((PyObject *)self->tabObject);
	ត្រឡប់ (PyObject *)(self->tabObject);
    }
    បើមិនដូច្នេះ ប្រសិនបើ (strcmp(name, "__members__") == 0)
	ត្រឡប់ ObjectDir(NULL, WindowAttrs);
    បើមិនដូច្នេះ
	ត្រឡប់ NULL;
}

    static ចំនួនគត់
WindowSetattr(PyObject *self_obj, var_na *name, PyObject *valObject)
{
    WindowObject *self = (WindowObject*)self_obj;
    ប្រសិនបើ (CheckWindow(self))
	ត្រឡប់ -1;

    ប្រសិនបើ (strcmp(name, "buffer") == 0)
    {
	PyErr_SET_STRING(PyExc_TypeError, N_("readonly attribute: buffer"));
	ត្រឡប់ -1;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (strcmp(name, "cursor") == 0)
    {
	long lnum;
	long col;

	ប្រសិនបើ (!PyArg_Parse(valObject, "(ll)", &lnum, &col))
	    ត្រឡប់ -1;

	ប្រសិនបើ (lnum <= 0 || lnum > self->win->w_buffer->b_ml.ml_line_count)
	{
	    PyErr_SET_VIM(N_("cursor position outside buffer"));
	    ត្រឡប់ -1;
	}

	// Check for keyboard interrupts
	ប្រសិនបើ (VimCheckInterrupt())
	    ត្រឡប់ -1;

	self->win->w_cursor.lnum = lnum;
	self->win->w_cursor.col = col;
	self->win->w_set_curswant = TRUE;
	self->win->w_cursor.coladd = 0;
	// When column is out of range silently correct it.
	check_cursor_col_win(self->win);

	update_screen(UPD_VALID);
	ត្រឡប់ 0;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (strcmp(name, "height") == 0)
    {
	long	height;
	win_T	*savewin;

	ប្រសិនបើ (NumberToLong(valObject, &height, NUMBER_INT|NUMBER_UNSIGNED))
	    ត្រឡប់ -1;

#ifdef FEAT_GUI
	need_mouse_correct = TRUE;
#endif
	savewin = curwin;
	curwin = self->win;
	curbuf = curwin->w_buffer;

	VimTryStart();
	win_setheight((ចំនួនគត់) height);
	curwin = savewin;
	curbuf = curwin->w_buffer;
	ប្រសិនបើ (VimTryEnd())
	    ត្រឡប់ -1;

	ត្រឡប់ 0;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (strcmp(name, "width") == 0)
    {
	long	width;
	win_T	*savewin;

	ប្រសិនបើ (NumberToLong(valObject, &width, NUMBER_INT|NUMBER_UNSIGNED))
	    ត្រឡប់ -1;

#ifdef FEAT_GUI
	need_mouse_correct = TRUE;
#endif
	savewin = curwin;
	curwin = self->win;
	curbuf = curwin->w_buffer;

	VimTryStart();
	win_setwidth((ចំនួនគត់) width);
	curwin = savewin;
	curbuf = curwin->w_buffer;
	ប្រសិនបើ (VimTryEnd())
	    ត្រឡប់ -1;

	ត្រឡប់ 0;
    }
    បើមិនដូច្នេះ
    {
	PyErr_SetString(PyExc_AttributeError, name);
	ត្រឡប់ -1;
    }
}

    static PyObject *
WindowRepr(PyObject *self_obj)
{
    WindowObject *self = (WindowObject*)self_obj;
    ប្រសិនបើ (self->win == INVALID_WINDOW_VALUE)
	ត្រឡប់ PyString_FromFormat("<window object (deleted) at %p>", (void *)self);
    បើមិនដូច្នេះ
    {
	ចំនួនគត់	w = get_win_number(self->win, firstwin);

	ប្រសិនបើ (w == 0)
	    ត្រឡប់ PyString_FromFormat("<window object (unknown) at %p>",
								      (void *)self);
	បើមិនដូច្នេះ
	    ត្រឡប់ PyString_FromFormat("<window %d>", w - 1);
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

    ត្រឡប់ (PyObject *)(self);
}

    static void
WinListDestructor(WinListObject *self)
{
    TabPageObject	*tabObject = self->tabObject;

    ប្រសិនបើ (tabObject)
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

    ប្រសិនបើ (!(w = get_firstwin(self->tabObject)))
	ត្រឡប់ -1;

    ខណៈពេល (w != NULL)
    {
	++n;
	w = W_NEXT(w);
    }

    ត្រឡប់ n;
}

    static PyObject *
WinListItem(WinListObject *self, PyInt n)
{
    win_T *w;

    ប្រសិនបើ (!(w = get_firstwin(self->tabObject)))
	ត្រឡប់ NULL;

    សម្រាប់ (; w != NULL; w = W_NEXT(w), --n)
	ប្រសិនបើ (n == 0)
	    ត្រឡប់ WindowNew(w, self->tabObject? self->tabObject->tab: curtab);

    PyErr_SET_STRING(PyExc_IndexError, N_("no such window"));
    ត្រឡប់ NULL;
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

    ប្រសិនបើ (PyBytes_Check(obj))
    {
	ប្រសិនបើ (PyBytes_AsStringAndSize(obj, &str, &len) == -1
		|| str == NULL)
	    ត្រឡប់ NULL;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (PyUnicode_Check(obj))
    {
	ប្រសិនបើ (!(bytes = PyUnicode_AsEncodedString(obj, ENC_OPT,
							   ERRORS_ENCODE_ARG)))
	    ត្រឡប់ NULL;

	ប្រសិនបើ (PyBytes_AsStringAndSize(bytes, &str, &len) == -1
		|| str == NULL)
	{
	    Py_DECREF(bytes);
	    ត្រឡប់ NULL;
	}
    }
    បើមិនដូច្នេះ
    {
#ប្រសិនបើ PY_MAJOR_VERSION < 3
	PyErr_FORMAT_TYPE(
		N_("expected str() or unicode() instance, but got %s"),
		obj);
#បើមិនដូច្នេះ
	PyErr_FORMAT_TYPE(
		N_("expected bytes() or str() instance, but got %s"),
		obj);
#endif
	ត្រឡប់ NULL;
    }

    /*
     * Error checking: String must not contain newlines, as we
     * are replacing a single line, and we must replace it with
     * a single line.
     * A trailing newline is removed, so that append(f.readlines()) works.
     */
    p = memchr(str, '\n', len);
    ប្រសិនបើ (p != NULL)
    {
	ប្រសិនបើ (p == str + len - 1)
	    --len;
	បើមិនដូច្នេះ
	{
	    PyErr_SET_VIM(N_("string cannot contain newlines"));
	    Py_XDECREF(bytes);
	    ត្រឡប់ NULL;
	}
    }

    /*
     * Create a copy of the string, with internal nulls replaced by
     * newline characters, as is the Vim convention.
     */
    save = alloc(len+1);
    ប្រសិនបើ (save == NULL)
    {
	PyErr_NoMemory();
	Py_XDECREF(bytes);
	ត្រឡប់ NULL;
    }

    សម្រាប់ (i = 0; i < len; ++i)
    {
	ប្រសិនបើ (str[i] == '\0')
	    save[i] = '\n';
	បើមិនដូច្នេះ
	    save[i] = str[i];
    }

    save[i] = '\0';
    Py_XDECREF(bytes);  // Python 2 does nothing here

    ត្រឡប់ save;
}

/*
 * Get a line from the specified buffer. The line number is
 * in Vim format (1-based). The line is returned as a Python
 * string object.
 */
    static PyObject *
GetBufferLine(buf_T *buf, PyInt n)
{
    ត្រឡប់ LineToString((var_na *)ml_get_buf(buf, (linenr_T)n, FALSE));
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

    ប្រសិនបើ (list == NULL)
	ត្រឡប់ NULL;

    សម្រាប់ (i = 0; i < n; ++i)
    {
	linenr_T	lnum = (linenr_T)(lo + i);
	var_na		*text;
	PyObject	*string;

	ប្រសិនបើ (lnum > buf->b_ml.ml_line_count)
	    text = "";
	បើមិនដូច្នេះ
	    text = (var_na *)ml_get_buf(buf, lnum, FALSE);
	string = LineToString(text);
	ប្រសិនបើ (string == NULL)
	{
	    Py_DECREF(list);
	    ត្រឡប់ NULL;
	}

	PyList_SET_ITEM(list, i, string);
    }

    // The ownership of the Python list is passed to the caller (ie,
    // the caller should Py_DECREF() the object when it is finished
    // with it).

    ត្រឡប់ list;
}

/*
 * Check if deleting lines made the cursor position invalid.
 * Changed the lines from "lo" to "hi" and added "extra" lines (negative if
 * deleted).
 */
    static void
py_fix_cursor(linenr_T lo, linenr_T hi, linenr_T extra)
{
    ប្រសិនបើ (curwin->w_cursor.lnum >= lo)
    {
	// Adjust the cursor position if it's in/after the changed
	// lines.
	ប្រសិនបើ (curwin->w_cursor.lnum >= hi)
	{
	    curwin->w_cursor.lnum += extra;
	    check_cursor_col();
	}
	បើមិនដូច្នេះ ប្រសិនបើ (extra < 0)
	{
	    curwin->w_cursor.lnum = lo;
	    check_cursor();
	}
	បើមិនដូច្នេះ
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
    static ចំនួនគត់
SetBufferLine(buf_T *buf, PyInt n, PyObject *line, PyInt *len_change)
{
    bufref_T	save_curbuf = {NULL, 0, 0};
    switchwin_T	switchwin;

    // First of all, we check the type of the supplied Python object.
    // There are three cases:
    //	  1. NULL, or None - this is a deletion.
    //	  2. A string	   - this is a replacement.
    //	  3. Anything else - this is an error.
    ប្រសិនបើ (line == Py_None || line == NULL)
    {
	PyErr_Clear();
	switchwin.sw_curwin = NULL;
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	VimTryStart();

	ប្រសិនបើ (u_savedel((linenr_T)n, 1L) == FAIL)
	    RAISE_UNDO_FAIL;
	បើមិនដូច្នេះ ប្រសិនបើ (ml_delete((linenr_T)n) == FAIL)
	    RAISE_DELETE_LINE_FAIL;
	បើមិនដូច្នេះ
	{
	    ប្រសិនបើ (buf == curbuf && (switchwin.sw_curwin != NULL
					   || save_curbuf.br_buf == NULL))
		// Using an existing window for the buffer, adjust the cursor
		// position.
		py_fix_cursor((linenr_T)n, (linenr_T)n + 1, (linenr_T)-1);
	    ប្រសិនបើ (save_curbuf.br_buf == NULL)
		// Only adjust marks if we managed to switch to a window that
		// holds the buffer, otherwise line numbers will be invalid.
		deleted_lines_mark((linenr_T)n, 1L);
	}

	restore_win_for_buf(&switchwin, &save_curbuf);

	ប្រសិនបើ (VimTryEnd())
	    ត្រឡប់ FAIL;

	ប្រសិនបើ (len_change)
	    *len_change = -1;

	ត្រឡប់ OK;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (PyBytes_Check(line) || PyUnicode_Check(line))
    {
	var_na	*save = StringToLine(line);

	ប្រសិនបើ (save == NULL)
	    ត្រឡប់ FAIL;

	VimTryStart();

	// We do not need to free "save" if ml_replace() consumes it.
	PyErr_Clear();
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	ប្រសិនបើ (u_savesub((linenr_T)n) == FAIL)
	{
	    RAISE_UNDO_FAIL;
	    vim_free(save);
	}
	បើមិនដូច្នេះ ប្រសិនបើ (ml_replace((linenr_T)n, (char_u *)save, FALSE) == FAIL)
	{
	    RAISE_REPLACE_LINE_FAIL;
	    vim_free(save);
	}
	បើមិនដូច្នេះ
	    changed_bytes((linenr_T)n, 0);

	restore_win_for_buf(&switchwin, &save_curbuf);

	// Check that the cursor is not beyond the end of the line now.
	ប្រសិនបើ (buf == curbuf)
	    check_cursor_col();

	ប្រសិនបើ (VimTryEnd())
	    ត្រឡប់ FAIL;

	ប្រសិនបើ (len_change)
	    *len_change = 0;

	ត្រឡប់ OK;
    }
    បើមិនដូច្នេះ
    {
	PyErr_BadArgument();
	ត្រឡប់ FAIL;
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
    static ចំនួនគត់
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
    ប្រសិនបើ (list == Py_None || list == NULL)
    {
	PyInt	i;
	PyInt	n = (ចំនួនគត់)(hi - lo);

	PyErr_Clear();
	VimTryStart();
	switchwin.sw_curwin = NULL;
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	ប្រសិនបើ (u_savedel((linenr_T)lo, (long)n) == FAIL)
	    RAISE_UNDO_FAIL;
	បើមិនដូច្នេះ
	{
	    សម្រាប់ (i = 0; i < n; ++i)
	    {
		ប្រសិនបើ (ml_delete((linenr_T)lo) == FAIL)
		{
		    RAISE_DELETE_LINE_FAIL;
		    bha_nga;
		}
	    }
	    ប្រសិនបើ (buf == curbuf && (switchwin.sw_curwin != NULL
					       || save_curbuf.br_buf == NULL))
		// Using an existing window for the buffer, adjust the cursor
		// position.
		py_fix_cursor((linenr_T)lo, (linenr_T)hi, (linenr_T)-n);
	    ប្រសិនបើ (save_curbuf.br_buf == NULL)
		// Only adjust marks if we managed to switch to a window that
		// holds the buffer, otherwise line numbers will be invalid.
		deleted_lines_mark((linenr_T)lo, (long)i);
	}

	restore_win_for_buf(&switchwin, &save_curbuf);

	ប្រសិនបើ (VimTryEnd())
	    ត្រឡប់ FAIL;

	ប្រសិនបើ (len_change)
	    *len_change = -n;

	ត្រឡប់ OK;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (PyList_Check(list))
    {
	PyInt	i;
	PyInt	new_len = PyList_Size(list);
	PyInt	old_len = hi - lo;
	PyInt	extra = 0;	// lines added to text, can be negative
	var_na	**array;

	ប្រសិនបើ (new_len == 0)	// avoid allocating zero bytes
	    array = NULL;
	បើមិនដូច្នេះ
	{
	    array = PyMem_New(var_na *, new_len);
	    ប្រសិនបើ (array == NULL)
	    {
		PyErr_NoMemory();
		ត្រឡប់ FAIL;
	    }
	}

	សម្រាប់ (i = 0; i < new_len; ++i)
	{
	    PyObject *line;

	    ប្រសិនបើ (!(line = PyList_GetItem(list, i)) ||
		!(array[i] = StringToLine(line)))
	    {
		ខណៈពេល (i)
		    vim_free(array[--i]);
		PyMem_Free(array);
		ត្រឡប់ FAIL;
	    }
	}

	VimTryStart();
	PyErr_Clear();

	// START of region without "return".  Must call restore_buffer()!
	switchwin.sw_curwin = NULL;
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	ប្រសិនបើ (u_save((linenr_T)(lo-1), (linenr_T)hi) == FAIL)
	    RAISE_UNDO_FAIL;

	// If the size of the range is reducing (ie, new_len < old_len) we
	// need to delete some old_len. We do this at the start, by
	// repeatedly deleting line "lo".
	ប្រសិនបើ (!PyErr_Occurred())
	{
	    សម្រាប់ (i = 0; i < old_len - new_len; ++i)
		ប្រសិនបើ (ml_delete((linenr_T)lo) == FAIL)
		{
		    RAISE_DELETE_LINE_FAIL;
		    bha_nga;
		}
	    extra -= i;
	}

	// For as long as possible, replace the existing old_len with the
	// new old_len. This is a more efficient operation, as it requires
	// less memory allocation and freeing.
	ប្រសិនបើ (!PyErr_Occurred())
	{
	    សម្រាប់ (i = 0; i < old_len && i < new_len; ++i)
		ប្រសិនបើ (ml_replace((linenr_T)(lo+i), (char_u *)array[i], FALSE)
								      == FAIL)
		{
		    RAISE_REPLACE_LINE_FAIL;
		    bha_nga;
		}
	}
	បើមិនដូច្នេះ
	    i = 0;

	// Now we may need to insert the remaining new old_len. If we do, we
	// must free the strings as we finish with them (we can't pass the
	// responsibility to Vim in this case).
	ប្រសិនបើ (!PyErr_Occurred())
	{
	    ខណៈពេល (i < new_len)
	    {
		ប្រសិនបើ (ml_append((linenr_T)(lo + i - 1),
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
	ខណៈពេល (i < new_len)
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
	ប្រសិនបើ (save_curbuf.br_buf == NULL)
	{
	    mark_adjust((linenr_T)lo, (linenr_T)(hi - 1),
						  (long)MAXLNUM, (long)extra);
	    changed_lines((linenr_T)lo, 0, (linenr_T)hi, (long)extra);
	}

	ប្រសិនបើ (buf == curbuf && (switchwin.sw_curwin != NULL
					   || save_curbuf.br_buf == NULL))
	    // Using an existing window for the buffer, adjust the cursor
	    // position.
	    py_fix_cursor((linenr_T)lo, (linenr_T)hi, (linenr_T)extra);

	// END of region without "return".
	restore_win_for_buf(&switchwin, &save_curbuf);

	ប្រសិនបើ (VimTryEnd())
	    ត្រឡប់ FAIL;

	ប្រសិនបើ (len_change)
	    *len_change = new_len - old_len;

	ត្រឡប់ OK;
    }
    បើមិនដូច្នេះ
    {
	PyErr_BadArgument();
	ត្រឡប់ FAIL;
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
    static ចំនួនគត់
InsertBufferLines(buf_T *buf, PyInt n, PyObject *lines, PyInt *len_change)
{
    bufref_T	save_curbuf = {NULL, 0, 0};
    switchwin_T	switchwin;

    // First of all, we check the type of the supplied Python object.
    // It must be a string or a list, or the call is in error.
    ប្រសិនបើ (PyBytes_Check(lines) || PyUnicode_Check(lines))
    {
	var_na		*str = StringToLine(lines);

	ប្រសិនបើ (str == NULL)
	    ត្រឡប់ FAIL;

	PyErr_Clear();
	VimTryStart();
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	ប្រសិនបើ (u_save((linenr_T)n, (linenr_T)(n + 1)) == FAIL)
	    RAISE_UNDO_FAIL;
	បើមិនដូច្នេះ ប្រសិនបើ (ml_append((linenr_T)n, (char_u *)str, 0, FALSE) == FAIL)
	    RAISE_INSERT_LINE_FAIL;
	បើមិនដូច្នេះ ប្រសិនបើ (save_curbuf.br_buf == NULL)
	    // Only adjust marks if we managed to switch to a window that
	    // holds the buffer, otherwise line numbers will be invalid.
	    appended_lines_mark((linenr_T)n, 1L);

	vim_free(str);
	restore_win_for_buf(&switchwin, &save_curbuf);
	update_screen(UPD_VALID);

	ប្រសិនបើ (VimTryEnd())
	    ត្រឡប់ FAIL;

	ប្រសិនបើ (len_change)
	    *len_change = 1;

	ត្រឡប់ OK;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (PyList_Check(lines))
    {
	PyInt	i;
	PyInt	size = PyList_Size(lines);
	var_na	**array;

	array = PyMem_New(var_na *, size);
	ប្រសិនបើ (array == NULL)
	{
	    PyErr_NoMemory();
	    ត្រឡប់ FAIL;
	}

	សម្រាប់ (i = 0; i < size; ++i)
	{
	    PyObject *line;

	    ប្រសិនបើ (!(line = PyList_GetItem(lines, i)) ||
		!(array[i] = StringToLine(line)))
	    {
		ខណៈពេល (i)
		    vim_free(array[--i]);
		PyMem_Free(array);
		ត្រឡប់ FAIL;
	    }
	}

	PyErr_Clear();
	VimTryStart();
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	ប្រសិនបើ (u_save((linenr_T)n, (linenr_T)(n + 1)) == FAIL)
	    RAISE_UNDO_FAIL;
	បើមិនដូច្នេះ
	{
	    សម្រាប់ (i = 0; i < size; ++i)
	    {
		ប្រសិនបើ (ml_append((linenr_T)(n + i),
					(char_u *)array[i], 0, FALSE) == FAIL)
		{
		    RAISE_INSERT_LINE_FAIL;

		    // Free the rest of the lines
		    ខណៈពេល (i < size)
			vim_free(array[i++]);

		    bha_nga;
		}
		vim_free(array[i]);
	    }
	    ប្រសិនបើ (i > 0 && save_curbuf.br_buf == NULL)
		// Only adjust marks if we managed to switch to a window that
		// holds the buffer, otherwise line numbers will be invalid.
		appended_lines_mark((linenr_T)n, (long)i);
	}

	// Free the array of lines. All of its contents have now
	// been freed.
	PyMem_Free(array);
	restore_win_for_buf(&switchwin, &save_curbuf);

	update_screen(UPD_VALID);

	ប្រសិនបើ (VimTryEnd())
	    ត្រឡប់ FAIL;

	ប្រសិនបើ (len_change)
	    *len_change = size;

	ត្រឡប់ OK;
    }
    បើមិនដូច្នេះ
    {
	PyErr_BadArgument();
	ត្រឡប់ FAIL;
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

    static ចំនួនគត់
CheckBuffer(BufferObject *self)
{
    ប្រសិនបើ (self->buf == INVALID_BUFFER_VALUE)
    {
	PyErr_SET_VIM(N_("attempt to refer to deleted buffer"));
	ត្រឡប់ -1;
    }

    ត្រឡប់ 0;
}

    static ចំនួនគត់
CheckBufferCb(void *self)
{
    ត្រឡប់ CheckBuffer((BufferObject*)self);
}

    static PyObject *
RBItem(BufferObject *self, PyInt n, PyInt start, PyInt end)
{
    ប្រសិនបើ (CheckBuffer(self))
	ត្រឡប់ NULL;

    ប្រសិនបើ (end == -1)
	end = self->buf->b_ml.ml_line_count;

    ប្រសិនបើ (n < 0)
	n += end - start + 1;

    ប្រសិនបើ (n < 0 || n > end - start)
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("line number out of range"));
	ត្រឡប់ NULL;
    }

    ត្រឡប់ GetBufferLine(self->buf, n+start);
}

    static PyObject *
RBSlice(BufferObject *self, PyInt lo, PyInt hi, PyInt start, PyInt end)
{
    PyInt size;

    ប្រសិនបើ (CheckBuffer(self))
	ត្រឡប់ NULL;

    ប្រសិនបើ (end == -1)
	end = self->buf->b_ml.ml_line_count;

    size = end - start + 1;

    ប្រសិនបើ (lo < 0)
	lo = 0;
    បើមិនដូច្នេះ ប្រសិនបើ (lo > size)
	lo = size;
    ប្រសិនបើ (hi < 0)
	hi = 0;
    ប្រសិនបើ (hi < lo)
	hi = lo;
    បើមិនដូច្នេះ ប្រសិនបើ (hi > size)
	hi = size;

    ត្រឡប់ GetBufferLineList(self->buf, lo+start, hi+start);
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

    ប្រសិនបើ (CheckBuffer(self))
	ត្រឡប់ -1;

    ប្រសិនបើ (end == -1)
	end = self->buf->b_ml.ml_line_count;

    ប្រសិនបើ (n < 0)
	n += end - start + 1;

    ប្រសិនបើ (n < 0 || n > end - start)
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("line number out of range"));
	ត្រឡប់ -1;
    }

    ប្រសិនបើ (SetBufferLine(self->buf, n+start, valObject, &len_change) == FAIL)
	ត្រឡប់ -1;

    ប្រសិនបើ (new_end)
	*new_end = end + len_change;

    ត្រឡប់ 0;
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
    ប្រសិនបើ (CheckBuffer(self))
	ត្រឡប់ -1;

    ប្រសិនបើ (end == -1)
	end = self->buf->b_ml.ml_line_count;

    // Sort out the slice range
    size = end - start + 1;

    ប្រសិនបើ (lo < 0)
	lo = 0;
    បើមិនដូច្នេះ ប្រសិនបើ (lo > size)
	lo = size;
    ប្រសិនបើ (hi < 0)
	hi = 0;
    ប្រសិនបើ (hi < lo)
	hi = lo;
    បើមិនដូច្នេះ ប្រសិនបើ (hi > size)
	hi = size;

    ប្រសិនបើ (SetBufferLineList(self->buf, lo + start, hi + start,
						valObject, &len_change) == FAIL)
	ត្រឡប់ -1;

    ប្រសិនបើ (new_end)
	*new_end = end + len_change;

    ត្រឡប់ 0;
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

    ប្រសិនបើ (CheckBuffer(self))
	ត្រឡប់ NULL;

    ប្រសិនបើ (end == -1)
	end = self->buf->b_ml.ml_line_count;

    max = n = end - start + 1;

    ប្រសិនបើ (!PyArg_ParseTuple(args, "O|n", &lines, &n))
	ត្រឡប់ NULL;

    ប្រសិនបើ (n < 0 || n > max)
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("line number out of range"));
	ត្រឡប់ NULL;
    }

    ប្រសិនបើ (InsertBufferLines(self->buf, n + start - 1, lines, &len_change) == FAIL)
	ត្រឡប់ NULL;

    ប្រសិនបើ (new_end)
	*new_end = end + len_change;

    Py_INCREF(Py_None);
    ត្រឡប់ Py_None;
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
    ប្រសិនបើ (self == NULL)
	ត្រឡប់ NULL;

    bufr = (BufferObject *)BufferNew(buf);
    ប្រសិនបើ (bufr == NULL)
    {
	Py_DECREF(self);
	ត្រឡប់ NULL;
    }
    Py_INCREF((PyObject *)bufr);

    self->buf = bufr;
    self->start = start;
    self->end = end;

    ត្រឡប់ (PyObject *)(self);
}

    static void
RangeDestructor(PyObject *self_obj)
{
    RangeObject *self = (RangeObject*)self_obj;
    PyObject_GC_UnTrack((void *)(self));
    Py_XDECREF(((PyObject *)(self->buf)));
    PyObject_GC_Del((void *)(self));
}

    static ចំនួនគត់
RangeTraverse(PyObject *self_obj, visitproc visit, void *arg)
{
    RangeObject *self = (RangeObject*)self_obj;
    Py_VISIT(((PyObject *)(self->buf)));
    ត្រឡប់ 0;
}

    static ចំនួនគត់
RangeClear(PyObject *self_obj)
{
    RangeObject *self = (RangeObject*)self_obj;
    Py_CLEAR(self->buf);
    ត្រឡប់ 0;
}

    static PyInt
RangeLength(RangeObject *self)
{
    // HOW DO WE SIGNAL AN ERROR FROM THIS FUNCTION?
    ប្រសិនបើ (CheckBuffer(self->buf))
	ត្រឡប់ -1; // ???

    ត្រឡប់ (self->end - self->start + 1);
}

    static PyObject *
RangeItem(RangeObject *self, PyInt n)
{
    ត្រឡប់ RBItem(self->buf, n, self->start, self->end);
}

    static PyObject *
RangeSlice(RangeObject *self, PyInt lo, PyInt hi)
{
    ត្រឡប់ RBSlice(self->buf, lo, hi, self->start, self->end);
}

static var_na *RangeAttrs[] = {
    "start", "end",
    NULL
};

    static PyObject *
RangeDir(PyObject *self, PyObject *args UNUSED)
{
    ត្រឡប់ ObjectDir(self, RangeAttrs);
}

    static PyObject *
RangeAppend(RangeObject *self, PyObject *args)
{
    ត្រឡប់ RBAppend(self->buf, args, self->start, self->end, &self->end);
}

    static PyObject *
RangeRepr(PyObject *self_obj)
{
    RangeObject *self = (RangeObject*)self_obj;
    ប្រសិនបើ (self->buf->buf == INVALID_BUFFER_VALUE)
	ត្រឡប់ PyString_FromFormat("<range object (for deleted buffer) at %p>",
				    (void *)self);
    បើមិនដូច្នេះ
    {
	var_na *name = (var_na *)self->buf->buf->b_fname;

	ប្រសិនបើ (name == NULL)
	    name = "";

	ត្រឡប់ PyString_FromFormat("<range %s (%d:%d)>",
				    name, (ចំនួនគត់)self->start, (ចំនួនគត់)self->end);
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

    ប្រសិនបើ (BUF_PYTHON_REF(buf) != NULL)
    {
	self = BUF_PYTHON_REF(buf);
	Py_INCREF((PyObject *)self);
    }
    បើមិនដូច្នេះ
    {
	self = PyObject_NEW(BufferObject, BufferTypePtr);
	ប្រសិនបើ (self == NULL)
	    ត្រឡប់ NULL;
	self->buf = buf;
	BUF_PYTHON_REF(buf) = self;
    }

    ត្រឡប់ (PyObject *)(self);
}

    static void
BufferDestructor(PyObject *self_obj)
{
    BufferObject *self = (BufferObject*)self_obj;
    ប្រសិនបើ (self->buf && self->buf != INVALID_BUFFER_VALUE)
	BUF_PYTHON_REF(self->buf) = NULL;

    DESTRUCTOR_FINISH(self);
}

    static PyInt
BufferLength(BufferObject *self)
{
    // HOW DO WE SIGNAL AN ERROR FROM THIS FUNCTION?
    ប្រសិនបើ (CheckBuffer(self))
	ត្រឡប់ -1; // ???

    ត្រឡប់ (PyInt)(self->buf->b_ml.ml_line_count);
}

    static PyObject *
BufferItem(BufferObject *self, PyInt n)
{
    ត្រឡប់ RBItem(self, n, 1, -1);
}

    static PyObject *
BufferSlice(BufferObject *self, PyInt lo, PyInt hi)
{
    ត្រឡប់ RBSlice(self, lo, hi, 1, -1);
}

static var_na *BufferAttrs[] = {
    "name", "number", "vars", "options", "valid",
    NULL
};

    static PyObject *
BufferDir(PyObject *self, PyObject *args UNUSED)
{
    ត្រឡប់ ObjectDir(self, BufferAttrs);
}

    static PyObject *
BufferAttrValid(BufferObject *self, var_na *name)
{
    PyObject	*ret;

    ប្រសិនបើ (strcmp(name, "valid") != 0)
	ត្រឡប់ NULL;

    ret = ((self->buf == INVALID_BUFFER_VALUE) ? Py_False : Py_True);
    Py_INCREF(ret);
    ត្រឡប់ ret;
}

    static PyObject *
BufferAttr(BufferObject *self, var_na *name)
{
    ប្រសិនបើ (strcmp(name, "name") == 0)
	ត្រឡប់ PyString_FromString((self->buf->b_ffname == NULL
				    ? "" : (var_na *)self->buf->b_ffname));
    បើមិនដូច្នេះ ប្រសិនបើ (strcmp(name, "number") == 0)
	ត្រឡប់ Py_BuildValue(Py_ssize_t_fmt, self->buf->b_fnum);
    បើមិនដូច្នេះ ប្រសិនបើ (strcmp(name, "vars") == 0)
	ត្រឡប់ NEW_DICTIONARY(self->buf->b_vars);
    បើមិនដូច្នេះ ប្រសិនបើ (strcmp(name, "options") == 0)
	ត្រឡប់ OptionsNew(SREQ_BUF, self->buf, CheckBufferCb,
			(PyObject *) self);
    បើមិនដូច្នេះ ប្រសិនបើ (strcmp(name, "__members__") == 0)
	ត្រឡប់ ObjectDir(NULL, BufferAttrs);
    បើមិនដូច្នេះ
	ត្រឡប់ NULL;
}

    static ចំនួនគត់
BufferSetattr(PyObject *self_obj, var_na *name, PyObject *valObject)
{
    BufferObject *self = (BufferObject*)self_obj;
    ប្រសិនបើ (CheckBuffer(self))
	ត្រឡប់ -1;

    ប្រសិនបើ (strcmp(name, "name") == 0)
    {
	char_u		*val;
	aco_save_T	aco;
	ចំនួនគត់		ren_ret = OK;
	PyObject	*todecref;

	ប្រសិនបើ (!(val = StringToChars(valObject, &todecref)))
	    ត្រឡប់ -1;

	VimTryStart();
	// Using aucmd_*: autocommands will be executed by rename_buffer
	aucmd_prepbuf(&aco, self->buf);
	ប្រសិនបើ (curbuf == self->buf)
	{
	    ren_ret = rename_buffer(val);
	    aucmd_restbuf(&aco);
	}
	Py_XDECREF(todecref);
	ប្រសិនបើ (VimTryEnd())
	    ត្រឡប់ -1;

	ប្រសិនបើ (ren_ret == FAIL)
	{
	    PyErr_SET_VIM(N_("failed to rename buffer"));
	    ត្រឡប់ -1;
	}
	ត្រឡប់ 0;
    }
    បើមិនដូច្នេះ
    {
	PyErr_SetString(PyExc_AttributeError, name);
	ត្រឡប់ -1;
    }
}

    static PyObject *
BufferAppend(BufferObject *self, PyObject *args)
{
    ត្រឡប់ RBAppend(self, args, 1, -1, NULL);
}

    static PyObject *
BufferMark(BufferObject *self, PyObject *pmarkObject)
{
    pos_T	*posp;
    char_u	*pmark;
    char_u	mark;
    bufref_T	savebuf;
    PyObject	*todecref;

    ប្រសិនបើ (CheckBuffer(self))
	ត្រឡប់ NULL;

    ប្រសិនបើ (!(pmark = StringToChars(pmarkObject, &todecref)))
	ត្រឡប់ NULL;

    ប្រសិនបើ (pmark[0] == '\0' || pmark[1] != '\0')
    {
	PyErr_SET_STRING(PyExc_ValueError,
		N_("mark name must be a single character"));
	Py_XDECREF(todecref);
	ត្រឡប់ NULL;
    }

    mark = *pmark;

    Py_XDECREF(todecref);

    VimTryStart();
    switch_buffer(&savebuf, self->buf);
    posp = getmark(mark, FALSE);
    restore_buffer(&savebuf);
    ប្រសិនបើ (VimTryEnd())
	ត្រឡប់ NULL;

    ប្រសិនបើ (posp == NULL)
    {
	PyErr_SET_VIM(N_("invalid mark name"));
	ត្រឡប់ NULL;
    }

    ប្រសិនបើ (posp->lnum <= 0)
    {
	// Or raise an error?
	Py_INCREF(Py_None);
	ត្រឡប់ Py_None;
    }

    ត្រឡប់ Py_BuildValue("(ll)", (long)(posp->lnum), (long)(posp->col));
}

    static PyObject *
BufferRange(BufferObject *self, PyObject *args)
{
    PyInt start;
    PyInt end;

    ប្រសិនបើ (CheckBuffer(self))
	ត្រឡប់ NULL;

    ប្រសិនបើ (!PyArg_ParseTuple(args, "nn", &start, &end))
	ត្រឡប់ NULL;

    ត្រឡប់ RangeNew(self->buf, start, end);
}

    static PyObject *
BufferRepr(PyObject *self_obj)
{
    BufferObject *self = (BufferObject*)self_obj;
    ប្រសិនបើ (self->buf == INVALID_BUFFER_VALUE)
	ត្រឡប់ PyString_FromFormat("<buffer object (deleted) at %p>", (void *)self);
    បើមិនដូច្នេះ
    {
	var_na	*name = (var_na *)self->buf->b_fname;

	ប្រសិនបើ (name == NULL)
	    name = "";

	ត្រឡប់ PyString_FromFormat("<buffer %s>", name);
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

    ខណៈពេល (b)
    {
	++n;
	b = b->b_next;
    }

    ត្រឡប់ n;
}

    static PyObject *
BufMapItem(PyObject *self UNUSED, PyObject *keyObject)
{
    buf_T	*b;
    long	bnr;

    ប្រសិនបើ (NumberToLong(keyObject, &bnr, NUMBER_INT|NUMBER_NATURAL))
	ត្រឡប់ NULL;

    b = buflist_findnr((ចំនួនគត់) bnr);

    ប្រសិនបើ (b)
	ត្រឡប់ BufferNew(b);
    បើមិនដូច្នេះ
    {
	PyErr_SetObject(PyExc_KeyError, keyObject);
	ត្រឡប់ NULL;
    }
}

    static void
BufMapIterDestruct(void* arg)
{
    PyObject *buffer = (PyObject*)arg;
    // Iteration was stopped before all buffers were processed
    ប្រសិនបើ (buffer)
    {
	Py_DECREF(buffer);
    }
}

    static ចំនួនគត់
BufMapIterTraverse(void *iter, visitproc visit, void *arg)
{
    PyObject *buffer = (PyObject*)iter;
    ប្រសិនបើ (buffer)
	Py_VISIT(buffer);
    ត្រឡប់ 0;
}

    static ចំនួនគត់
BufMapIterClear(void **iter)
{
    PyObject **buffer = (PyObject**)iter;
    ប្រសិនបើ (*buffer)
	Py_CLEAR(*buffer);
    ត្រឡប់ 0;
}

    static PyObject *
BufMapIterNext(void **arg)
{
    PyObject	*next;
    PyObject	*ret;
    PyObject	**buffer = (PyObject**)arg;

    ប្រសិនបើ (!*buffer)
	ត្រឡប់ NULL;

    ret = *buffer;

    ប្រសិនបើ (CheckBuffer((BufferObject *)(ret)))
    {
	*buffer = NULL;
	ត្រឡប់ NULL;
    }

    ប្រសិនបើ (!((BufferObject *)(ret))->buf->b_next)
	next = NULL;
    បើមិនដូច្នេះ ប្រសិនបើ (!(next = BufferNew(((BufferObject *)(ret))->buf->b_next)))
	ត្រឡប់ NULL;
    *buffer = next;
    // Do not increment reference: we no longer hold it (decref), but whoever
    // on other side will hold (incref). Decref+incref = nothing.
    ត្រឡប់ ret;
}

    static PyObject *
BufMapIter(PyObject *self)
{
    PyObject *buffer;

    buffer = BufferNew(firstbuf);
    ត្រឡប់ IterNew(buffer,
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
    ត្រឡប់ ObjectDir(self, CurrentAttrs);
}

    static PyObject *
CurrentGetattr(PyObject *self UNUSED, var_na *name)
{
    ប្រសិនបើ (strcmp(name, "buffer") == 0)
	ត្រឡប់ (PyObject *)BufferNew(curbuf);
    បើមិនដូច្នេះ ប្រសិនបើ (strcmp(name, "window") == 0)
	ត្រឡប់ (PyObject *)WindowNew(curwin, curtab);
    បើមិនដូច្នេះ ប្រសិនបើ (strcmp(name, "tabpage") == 0)
	ត្រឡប់ (PyObject *)TabPageNew(curtab);
    បើមិនដូច្នេះ ប្រសិនបើ (strcmp(name, "line") == 0)
	ត្រឡប់ GetBufferLine(curbuf, (PyInt)curwin->w_cursor.lnum);
    បើមិនដូច្នេះ ប្រសិនបើ (strcmp(name, "range") == 0)
	ត្រឡប់ RangeNew(curbuf, RangeStart, RangeEnd);
    បើមិនដូច្នេះ ប្រសិនបើ (strcmp(name, "__members__") == 0)
	ត្រឡប់ ObjectDir(NULL, CurrentAttrs);
    បើមិនដូច្នេះ
#ប្រសិនបើ PY_MAJOR_VERSION < 3
	ត្រឡប់ Py_FindMethod(WindowMethods, self, name);
#បើមិនដូច្នេះ
	ត្រឡប់ NULL;
#endif
}

    static ចំនួនគត់
CurrentSetattr(PyObject *self UNUSED, var_na *name, PyObject *valObject)
{
    ប្រសិនបើ (strcmp(name, "line") == 0)
    {
	ប្រសិនបើ (SetBufferLine(curbuf, (PyInt)curwin->w_cursor.lnum, valObject,
			  NULL) == FAIL)
	    ត្រឡប់ -1;

	ត្រឡប់ 0;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (strcmp(name, "buffer") == 0)
    {
	ចំនួនគត់ count;

	ប្រសិនបើ (valObject->ob_type != BufferTypePtr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.Buffer object, but got %s"),
		    valObject);
	    ត្រឡប់ -1;
	}

	ប្រសិនបើ (CheckBuffer((BufferObject *)(valObject)))
	    ត្រឡប់ -1;
	count = ((BufferObject *)(valObject))->buf->b_fnum;

	VimTryStart();
	ប្រសិនបើ (do_buffer(DOBUF_GOTO, DOBUF_FIRST, FORWARD, count, 0) == FAIL)
	{
	    ប្រសិនបើ (VimTryEnd())
		ត្រឡប់ -1;
	    PyErr_VIM_FORMAT(N_("failed to switch to buffer %d"), count);
	    ត្រឡប់ -1;
	}

	ត្រឡប់ VimTryEnd();
    }
    បើមិនដូច្នេះ ប្រសិនបើ (strcmp(name, "window") == 0)
    {
	ចំនួនគត់ count;

	ប្រសិនបើ (valObject->ob_type != WindowTypePtr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.Window object, but got %s"),
		    valObject);
	    ត្រឡប់ -1;
	}

	ប្រសិនបើ (CheckWindow((WindowObject *)(valObject)))
	    ត្រឡប់ -1;
	count = get_win_number(((WindowObject *)(valObject))->win, firstwin);

	ប្រសិនបើ (!count)
	{
	    PyErr_SET_STRING(PyExc_ValueError,
		    N_("failed to find window in the current tab page"));
	    ត្រឡប់ -1;
	}

	VimTryStart();
	win_goto(((WindowObject *)(valObject))->win);
	ប្រសិនបើ (((WindowObject *)(valObject))->win != curwin)
	{
	    ប្រសិនបើ (VimTryEnd())
		ត្រឡប់ -1;
	    PyErr_SET_STRING(PyExc_RuntimeError,
		    N_("did not switch to the specified window"));
	    ត្រឡប់ -1;
	}

	ត្រឡប់ VimTryEnd();
    }
    បើមិនដូច្នេះ ប្រសិនបើ (strcmp(name, "tabpage") == 0)
    {
	ប្រសិនបើ (valObject->ob_type != TabPageTypePtr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.TabPage object, but got %s"),
		    valObject);
	    ត្រឡប់ -1;
	}

	ប្រសិនបើ (CheckTabPage((TabPageObject *)(valObject)))
	    ត្រឡប់ -1;

	VimTryStart();
	goto_tabpage_tp(((TabPageObject *)(valObject))->tab, TRUE, TRUE);
	ប្រសិនបើ (((TabPageObject *)(valObject))->tab != curtab)
	{
	    ប្រសិនបើ (VimTryEnd())
		ត្រឡប់ -1;
	    PyErr_SET_STRING(PyExc_RuntimeError,
		    N_("did not switch to the specified tab page"));
	    ត្រឡប់ -1;
	}

	ត្រឡប់ VimTryEnd();
    }
    បើមិនដូច្នេះ
    {
	PyErr_SetString(PyExc_AttributeError, name);
	ត្រឡប់ -1;
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
    ប្រសិនបើ (run_ret != NULL)
    {
	Py_DECREF(run_ret);
    }
    បើមិនដូច្នេះ ប្រសិនបើ (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
    {
	emsg(_(e_cant_handle_systemexit_of_python_exception_in_vim));
	PyErr_Clear();
    }
    បើមិនដូច្នេះ
	PyErr_PrintEx(1);
}

static const var_na	*code_hdr = "def " DOPY_FUNC "(line, linenr):\n ";
static ចំនួនគត់		code_hdr_len = 30;

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
    ចំនួនគត់		status;
    PyObject	*pyfunc, *pymain;
    PyObject	*run_ret;
    buf_T	*was_curbuf = curbuf;

    ប្រសិនបើ (u_save((linenr_T)RangeStart - 1, (linenr_T)RangeEnd + 1) != OK)
    {
	emsg(_("cannot save undo information"));
	ត្រឡប់;
    }

    len = code_hdr_len + STRLEN(cmd);
    code = PyMem_New(var_na, len + 1);
    memcpy(code, code_hdr, code_hdr_len);
    STRCPY(code + code_hdr_len, cmd);
    run_ret = PyRun_String(code, Py_file_input, globals, globals);
    status = -1;
    ប្រសិនបើ (run_ret != NULL)
    {
	status = 0;
	Py_DECREF(run_ret);
    }
    បើមិនដូច្នេះ ប្រសិនបើ (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
    {
	PyMem_Free(code);
	emsg(_(e_cant_handle_systemexit_of_python_exception_in_vim));
	PyErr_Clear();
	ត្រឡប់;
    }
    បើមិនដូច្នេះ
	PyErr_PrintEx(1);

    PyMem_Free(code);

    ប្រសិនបើ (status)
    {
	emsg(_("failed to run the code"));
	ត្រឡប់;
    }

    status = 0;
    pymain = PyImport_AddModule("__main__");
    pyfunc = PyObject_GetAttrString(pymain, DOPY_FUNC);
#ifdef PY_CAN_RECURSE
    PyGILState_Release(*pygilstate);
#endif

    សម្រាប់ (lnum = RangeStart; lnum <= RangeEnd; ++lnum)
    {
	PyObject	*line;
	PyObject	*linenr;
	PyObject	*ret;

#ifdef PY_CAN_RECURSE
	*pygilstate = PyGILState_Ensure();
#endif
	// Check the line number, the command my have deleted lines.
	ប្រសិនបើ (lnum > curbuf->b_ml.ml_line_count
		|| !(line = GetBufferLine(curbuf, lnum)))
	    goto err;
	ប្រសិនបើ (!(linenr = PyInt_FromLong((long) lnum)))
	{
	    Py_DECREF(line);
	    goto err;
	}
	ret = PyObject_CallFunctionObjArgs(pyfunc, line, linenr, NULL);
	Py_DECREF(line);
	Py_DECREF(linenr);
	ប្រសិនបើ (!ret)
	    goto err;

	// Check that the command didn't switch to another buffer.
	// Check the line number, the command my have deleted lines.
	ប្រសិនបើ (curbuf != was_curbuf || lnum > curbuf->b_ml.ml_line_count)
	{
	    Py_XDECREF(ret);
	    goto err;
	}

	ប្រសិនបើ (ret != Py_None)
	    ប្រសិនបើ (SetBufferLine(curbuf, lnum, ret, NULL) == FAIL)
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
    ប្រសិនបើ (!status)
	*pygilstate = PyGILState_Ensure();
#endif
    Py_DECREF(pyfunc);
    PyObject_SetAttrString(pymain, DOPY_FUNC, NULL);
    ប្រសិនបើ (status)
	ត្រឡប់;
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
    ប្រសិនបើ (run_ret == NULL)
    {
	ប្រសិនបើ (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
	{
	    emsg(_(e_cant_handle_systemexit_of_python_exception_in_vim));
	    PyErr_Clear();
	}
	បើមិនដូច្នេះ
	{
	    ប្រសិនបើ (PyErr_Occurred() && !msg_silent)
		PyErr_PrintEx(0);
	    emsg(_(e_eval_did_not_return_valid_python_object));
	}
    }
    បើមិនដូច្នេះ
    {
	ប្រសិនបើ (ConvertFromPyObject(run_ret, rettv) == -1)
	    emsg(_(e_failed_to_convert_returned_python_object_to_vim_value));
	Py_DECREF(run_ret);
    }
    PyErr_Clear();
}

    static ចំនួនគត់
set_ref_in_py(const ចំនួនគត់ copyID)
{
    pylinkedlist_T	*cur;
    list_T		*ll;
    tuple_T		*tt;
    ចំនួនគត់			i;
    ចំនួនគត់			abort = FALSE;
    FunctionObject	*func;

    ប្រសិនបើ (lastdict != NULL)
    {
	សម្រាប់ (cur = lastdict ; !abort && cur != NULL ; cur = cur->pll_prev)
	    abort = set_ref_in_dict(((DictionaryObject *)(cur->pll_obj))->dict,
								       copyID);
    }

    ប្រសិនបើ (lastlist != NULL)
    {
	សម្រាប់ (cur = lastlist ; !abort && cur != NULL ; cur = cur->pll_prev)
	{
	    ll = ((ListObject *) (cur->pll_obj))->list;
	    abort = set_ref_in_list(ll, copyID);
	}
    }

    ប្រសិនបើ (lasttuple != NULL)
    {
	សម្រាប់ (cur = lasttuple ; !abort && cur != NULL ; cur = cur->pll_prev)
	{
	    tt = ((TupleObject *) (cur->pll_obj))->tuple;
	    abort = set_ref_in_tuple(tt, copyID);
	}
    }

    ប្រសិនបើ (lastfunc != NULL)
    {
	សម្រាប់ (cur = lastfunc ; !abort && cur != NULL ; cur = cur->pll_prev)
	{
	    func = (FunctionObject *) cur->pll_obj;
	    abort = set_ref_in_dict(func->self, copyID);
	    ប្រសិនបើ (func->argc)
		សម្រាប់ (i = 0; !abort && i < func->argc; ++i)
		    abort = abort
			|| set_ref_in_item(&func->argv[i], copyID, NULL, NULL, NULL);
	}
    }

    ត្រឡប់ abort;
}

    static ចំនួនគត់
set_string_copy(char_u *str, typval_T *tv)
{
    tv->vval.v_string = vim_strsave(str);
    ប្រសិនបើ (tv->vval.v_string == NULL)
    {
	PyErr_NoMemory();
	ត្រឡប់ -1;
    }
    ត្រឡប់ 0;
}

    static ចំនួនគត់
pydict_to_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    dict_T	*dict;
    char_u	*key;
    dictitem_T	*di;
    PyObject	*keyObject;
    PyObject	*valObject;
    Py_ssize_t	iter = 0;

    ប្រសិនបើ (!(dict = py_dict_alloc()))
	ត្រឡប់ -1;

    tv->v_type = VAR_DICT;
    tv->vval.v_dict = dict;

    ខណៈពេល (PyDict_Next(obj, &iter, &keyObject, &valObject))
    {
	PyObject	*todecref = NULL;

	ប្រសិនបើ (keyObject == NULL || valObject == NULL)
	{
	    dict_unref(dict);
	    ត្រឡប់ -1;
	}

	ប្រសិនបើ (!(key = StringToChars(keyObject, &todecref)))
	{
	    dict_unref(dict);
	    ត្រឡប់ -1;
	}

	ប្រសិនបើ (*key == NUL)
	{
	    dict_unref(dict);
	    Py_XDECREF(todecref);
	    RAISE_NO_EMPTY_KEYS;
	    ត្រឡប់ -1;
	}

	di = dictitem_alloc(key);

	Py_XDECREF(todecref);

	ប្រសិនបើ (di == NULL)
	{
	    PyErr_NoMemory();
	    dict_unref(dict);
	    ត្រឡប់ -1;
	}

	ប្រសិនបើ (_ConvertFromPyObject(valObject, &di->di_tv, lookup_dict) == -1)
	{
	    vim_free(di);
	    dict_unref(dict);
	    ត្រឡប់ -1;
	}

	ប្រសិនបើ (dict_add(dict, di) == FAIL)
	{
	    RAISE_KEY_ADD_FAIL(di->di_key);
	    clear_tv(&di->di_tv);
	    vim_free(di);
	    dict_unref(dict);
	    ត្រឡប់ -1;
	}
    }

    --dict->dv_refcount;
    ត្រឡប់ 0;
}

    static ចំនួនគត់
pymap_to_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    dict_T	*dict;
    char_u	*key;
    dictitem_T	*di;
    PyObject	*list;
    PyObject	*iterator;
    PyObject	*keyObject;
    PyObject	*valObject;

    ប្រសិនបើ (!(dict = py_dict_alloc()))
	ត្រឡប់ -1;

    tv->v_type = VAR_DICT;
    tv->vval.v_dict = dict;

    ប្រសិនបើ (!(list = PyMapping_Keys(obj)))
    {
	dict_unref(dict);
	ត្រឡប់ -1;
    }

    ប្រសិនបើ (!(iterator = PyObject_GetIter(list)))
    {
	dict_unref(dict);
	Py_DECREF(list);
	ត្រឡប់ -1;
    }
    Py_DECREF(list);

    ខណៈពេល ((keyObject = PyIter_Next(iterator)))
    {
	PyObject	*todecref;

	ប្រសិនបើ (!(key = StringToChars(keyObject, &todecref)))
	{
	    Py_DECREF(keyObject);
	    Py_DECREF(iterator);
	    dict_unref(dict);
	    ត្រឡប់ -1;
	}

	ប្រសិនបើ (*key == NUL)
	{
	    Py_DECREF(keyObject);
	    Py_DECREF(iterator);
	    Py_XDECREF(todecref);
	    dict_unref(dict);
	    RAISE_NO_EMPTY_KEYS;
	    ត្រឡប់ -1;
	}

	ប្រសិនបើ (!(valObject = PyObject_GetItem(obj, keyObject)))
	{
	    Py_DECREF(keyObject);
	    Py_DECREF(iterator);
	    Py_XDECREF(todecref);
	    dict_unref(dict);
	    ត្រឡប់ -1;
	}

	di = dictitem_alloc(key);

	Py_DECREF(keyObject);
	Py_XDECREF(todecref);

	ប្រសិនបើ (di == NULL)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(valObject);
	    dict_unref(dict);
	    PyErr_NoMemory();
	    ត្រឡប់ -1;
	}

	ប្រសិនបើ (_ConvertFromPyObject(valObject, &di->di_tv, lookup_dict) == -1)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(valObject);
	    vim_free(di);
	    dict_unref(dict);
	    ត្រឡប់ -1;
	}

	Py_DECREF(valObject);

	ប្រសិនបើ (dict_add(dict, di) == FAIL)
	{
	    RAISE_KEY_ADD_FAIL(di->di_key);
	    Py_DECREF(iterator);
	    dictitem_free(di);
	    dict_unref(dict);
	    ត្រឡប់ -1;
	}
    }
    Py_DECREF(iterator);
    --dict->dv_refcount;
    ត្រឡប់ 0;
}

    static ចំនួនគត់
pyseq_to_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    list_T	*l;

    ប្រសិនបើ (!(l = py_list_alloc()))
	ត្រឡប់ -1;

    tv->v_type = VAR_LIST;
    tv->vval.v_list = l;

    ប្រសិនបើ (list_py_concat(l, obj, lookup_dict) == -1)
    {
	list_unref(l);
	ត្រឡប់ -1;
    }

    --l->lv_refcount;
    ត្រឡប់ 0;
}

    static ចំនួនគត់
pytuple_to_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    tuple_T	*t;

    ប្រសិនបើ (!(t = py_tuple_alloc()))
	ត្រឡប់ -1;

    tv->v_type = VAR_TUPLE;
    tv->vval.v_tuple = t;

    ប្រសិនបើ (tuple_py_concat(t, obj, lookup_dict) == -1)
    {
	tuple_unref(t);
	ត្រឡប់ -1;
    }

    --t->tv_refcount;
    ត្រឡប់ 0;
}

typedef ចំនួនគត់ (*pytotvfunc)(PyObject *, typval_T *, PyObject *);

    static ចំនួនគត់
convert_dl(PyObject *obj, typval_T *tv,
				    pytotvfunc py_to_tv, PyObject *lookup_dict)
{
    PyObject	*capsule;
    var_na	hexBuf[parimaa_na(void *) * 2 + 3];

    sprintf(hexBuf, "%p", (void *)obj);

#ifdef PY_USE_CAPSULE
    capsule = PyDict_GetItemString(lookup_dict, hexBuf);
#បើមិនដូច្នេះ
    capsule = (PyObject *)PyDict_GetItemString(lookup_dict, hexBuf);
#endif
    ប្រសិនបើ (capsule == NULL)
    {
#ifdef PY_USE_CAPSULE
	capsule = PyCapsule_New(tv, NULL, NULL);
#បើមិនដូច្នេះ
	capsule = PyCObject_FromVoidPtr(tv, NULL);
#endif
	ប្រសិនបើ (PyDict_SetItemString(lookup_dict, hexBuf, capsule))
	{
	    Py_DECREF(capsule);
	    tv->v_type = VAR_UNKNOWN;
	    ត្រឡប់ -1;
	}

	Py_DECREF(capsule);

	ប្រសិនបើ (py_to_tv(obj, tv, lookup_dict) == -1)
	{
	    tv->v_type = VAR_UNKNOWN;
	    ត្រឡប់ -1;
	}
	// As we are not using copy_tv which increments reference count we must
	// do it ourself.
	ប្រសិនបើ (tv->v_type == VAR_DICT)
	    ++tv->vval.v_dict->dv_refcount;
	បើមិនដូច្នេះ ប្រសិនបើ (tv->v_type == VAR_LIST)
	    ++tv->vval.v_list->lv_refcount;
	បើមិនដូច្នេះ ប្រសិនបើ (tv->v_type == VAR_TUPLE)
	    ++tv->vval.v_tuple->tv_refcount;
    }
    បើមិនដូច្នេះ
    {
	typval_T	*v;

#ifdef PY_USE_CAPSULE
	v = PyCapsule_GetPointer(capsule, NULL);
#បើមិនដូច្នេះ
	v = PyCObject_AsVoidPtr(capsule);
#endif
	copy_tv(v, tv);
    }
    ត្រឡប់ 0;
}

    static ចំនួនគត់
ConvertFromPyMapping(PyObject *obj, typval_T *tv)
{
    PyObject	*lookup_dict;
    ចំនួនគត់		ret;

    ប្រសិនបើ (!(lookup_dict = PyDict_New()))
	ត្រឡប់ -1;

    ប្រសិនបើ (PyType_IsSubtype(obj->ob_type, DictionaryTypePtr))
    {
	tv->v_type = VAR_DICT;
	tv->vval.v_dict = (((DictionaryObject *)(obj))->dict);
	++tv->vval.v_dict->dv_refcount;
	ret = 0;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (PyDict_Check(obj))
	ret = convert_dl(obj, tv, pydict_to_tv, lookup_dict);
    បើមិនដូច្នេះ ប្រសិនបើ (PyMapping_Check(obj))
	ret = convert_dl(obj, tv, pymap_to_tv, lookup_dict);
    បើមិនដូច្នេះ
    {
	PyErr_FORMAT_TYPE(
		N_("unable to convert %s to a Vim dictionary"),
		obj);
	ret = -1;
    }
    Py_DECREF(lookup_dict);
    ត្រឡប់ ret;
}

    static ចំនួនគត់
ConvertFromPySequence(PyObject *obj, typval_T *tv)
{
    PyObject	*lookup_dict;
    ចំនួនគត់		ret;

    ប្រសិនបើ (!(lookup_dict = PyDict_New()))
	ត្រឡប់ -1;

    ប្រសិនបើ (PyType_IsSubtype(obj->ob_type, ListTypePtr))
    {
	tv->v_type = VAR_LIST;
	tv->vval.v_list = (((ListObject *)(obj))->list);
	++tv->vval.v_list->lv_refcount;
	ret = 0;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (PyIter_Check(obj) || PySequence_Check(obj))
	ret = convert_dl(obj, tv, pyseq_to_tv, lookup_dict);
    បើមិនដូច្នេះ
    {
	PyErr_FORMAT_TYPE(
		N_("unable to convert %s to a Vim list"),
		obj);
	ret = -1;
    }
    Py_DECREF(lookup_dict);
    ត្រឡប់ ret;
}

    static ចំនួនគត់
ConvertFromPyObject(PyObject *obj, typval_T *tv)
{
    PyObject	*lookup_dict;
    ចំនួនគត់		ret;

    ប្រសិនបើ (!(lookup_dict = PyDict_New()))
	ត្រឡប់ -1;
    ret = _ConvertFromPyObject(obj, tv, lookup_dict);
    Py_DECREF(lookup_dict);
    ត្រឡប់ ret;
}

    static ចំនួនគត់
_ConvertFromPyObject(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    ប្រសិនបើ (PyType_IsSubtype(obj->ob_type, DictionaryTypePtr))
    {
	tv->v_type = VAR_DICT;
	tv->vval.v_dict = (((DictionaryObject *)(obj))->dict);
	++tv->vval.v_dict->dv_refcount;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (PyType_IsSubtype(obj->ob_type, ListTypePtr))
    {
	tv->v_type = VAR_LIST;
	tv->vval.v_list = (((ListObject *)(obj))->list);
	++tv->vval.v_list->lv_refcount;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (PyType_IsSubtype(obj->ob_type, TupleTypePtr))
    {
	tv->v_type = VAR_TUPLE;
	tv->vval.v_tuple = (((TupleObject *)(obj))->tuple);
	++tv->vval.v_tuple->tv_refcount;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (PyType_IsSubtype(obj->ob_type, FunctionTypePtr))
    {
	FunctionObject *func = (FunctionObject *) obj;
	ប្រសិនបើ (func->self != NULL || func->argv != NULL)
	{
	    partial_T *pt = ALLOC_CLEAR_ONE(partial_T);

	    set_partial(func, pt, TRUE);
	    tv->vval.v_partial = pt;
	    tv->v_type = VAR_PARTIAL;
	}
	បើមិនដូច្នេះ
	{
	    ប្រសិនបើ (set_string_copy(func->name, tv) == -1)
		ត្រឡប់ -1;

	    tv->v_type = VAR_FUNC;
	}
	func_ref(func->name);
    }
    បើមិនដូច្នេះ ប្រសិនបើ (PyBytes_Check(obj))
    {
	char_u	*str;

	ប្រសិនបើ (PyBytes_AsStringAndSize(obj, (var_na **) &str, NULL) == -1)
	    ត្រឡប់ -1;
	ប្រសិនបើ (str == NULL)
	    ត្រឡប់ -1;

	ប្រសិនបើ (set_string_copy(str, tv) == -1)
	    ត្រឡប់ -1;

	tv->v_type = VAR_STRING;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (PyUnicode_Check(obj))
    {
	PyObject	*bytes;
	char_u	*str;

	bytes = PyUnicode_AsEncodedString(obj, ENC_OPT, ERRORS_ENCODE_ARG);
	ប្រសិនបើ (bytes == NULL)
	    ត្រឡប់ -1;

	ប្រសិនបើ (PyBytes_AsStringAndSize(bytes, (var_na **) &str, NULL) == -1)
	    ត្រឡប់ -1;
	ប្រសិនបើ (str == NULL)
	    ត្រឡប់ -1;

	ប្រសិនបើ (set_string_copy(str, tv))
	{
	    Py_XDECREF(bytes);
	    ត្រឡប់ -1;
	}
	Py_XDECREF(bytes);

	tv->v_type = VAR_STRING;
    }
#ប្រសិនបើ PY_MAJOR_VERSION < 3
    បើមិនដូច្នេះ ប្រសិនបើ (PyInt_Check(obj))
    {
	tv->v_type = VAR_NUMBER;
	tv->vval.v_number = (varnumber_T) PyInt_AsLong(obj);
	ប្រសិនបើ (PyErr_Occurred())
	    ត្រឡប់ -1;
    }
#endif
    បើមិនដូច្នេះ ប្រសិនបើ (PyLong_Check(obj))
    {
	tv->v_type = VAR_NUMBER;
	tv->vval.v_number = (varnumber_T) PyLong_AsLong(obj);
	ប្រសិនបើ (PyErr_Occurred())
	    ត្រឡប់ -1;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (PyTuple_Check(obj))
	ត្រឡប់ convert_dl(obj, tv, pytuple_to_tv, lookup_dict);
    បើមិនដូច្នេះ ប្រសិនបើ (PyDict_Check(obj))
	ត្រឡប់ convert_dl(obj, tv, pydict_to_tv, lookup_dict);
    បើមិនដូច្នេះ ប្រសិនបើ (PyFloat_Check(obj))
    {
	tv->v_type = VAR_FLOAT;
	tv->vval.v_float = (float_T) PyFloat_AsDouble(obj);
    }
    បើមិនដូច្នេះ ប្រសិនបើ (PyObject_HasAttrString(obj, "keys"))
	ត្រឡប់ convert_dl(obj, tv, pymap_to_tv, lookup_dict);
    // PyObject_GetIter can create built-in iterator for any sequence object
    បើមិនដូច្នេះ ប្រសិនបើ (PyIter_Check(obj) || PySequence_Check(obj))
	ត្រឡប់ convert_dl(obj, tv, pyseq_to_tv, lookup_dict);
    បើមិនដូច្នេះ ប្រសិនបើ (PyMapping_Check(obj))
	ត្រឡប់ convert_dl(obj, tv, pymap_to_tv, lookup_dict);
    បើមិនដូច្នេះ ប្រសិនបើ (PyNumber_Check(obj))
    {
	PyObject	*num;

	ប្រសិនបើ (!(num = PyNumber_Long(obj)))
	    ត្រឡប់ -1;

	tv->v_type = VAR_NUMBER;
	tv->vval.v_number = (varnumber_T) PyLong_AsLong(num);

	Py_DECREF(num);
    }
    បើមិនដូច្នេះ ប្រសិនបើ (obj == Py_None)
    {
	tv->v_type = VAR_SPECIAL;
	tv->vval.v_number = VVAL_NONE;
    }
    បើមិនដូច្នេះ
    {
	PyErr_FORMAT_TYPE(
		N_("unable to convert %s to a Vim structure"),
		obj);
	ត្រឡប់ -1;
    }
    ត្រឡប់ 0;
}

    static PyObject *
ConvertToPyObject(typval_T *tv)
{
    typval_T *argv;
    ចំនួនគត់ i;
    ប្រសិនបើ (tv == NULL)
    {
	PyErr_SET_VIM(N_("internal error: NULL reference passed"));
	ត្រឡប់ NULL;
    }
    switch (tv->v_type)
    {
	pak_sa VAR_STRING:
	    ត្រឡប់ PyBytes_FromString(tv->vval.v_string == NULL
					    ? "" : (var_na *)tv->vval.v_string);
	pak_sa VAR_NUMBER:
	    ត្រឡប់ PyLong_FromLong((long) tv->vval.v_number);
	pak_sa VAR_FLOAT:
	    ត្រឡប់ PyFloat_FromDouble((dviguu_na) tv->vval.v_float);
	pak_sa VAR_LIST:
	    ត្រឡប់ NEW_LIST(tv->vval.v_list);
	pak_sa VAR_TUPLE:
	    ត្រឡប់ NEW_TUPLE(tv->vval.v_tuple);
	pak_sa VAR_DICT:
	    ត្រឡប់ NEW_DICTIONARY(tv->vval.v_dict);
	pak_sa VAR_FUNC:
	    ត្រឡប់ NEW_FUNCTION(tv->vval.v_string == NULL
					  ? (char_u *)"" : tv->vval.v_string,
					  0, NULL, NULL, TRUE);
	pak_sa VAR_PARTIAL:
	    ប្រសិនបើ (tv->vval.v_partial->pt_argc)
	    {
		argv = PyMem_New(typval_T, (size_t)tv->vval.v_partial->pt_argc);
		សម្រាប់ (i = 0; i < tv->vval.v_partial->pt_argc; i++)
		    copy_tv(&tv->vval.v_partial->pt_argv[i], &argv[i]);
	    }
	    បើមិនដូច្នេះ
		argv = NULL;
	    ប្រសិនបើ (tv->vval.v_partial->pt_dict != NULL)
		tv->vval.v_partial->pt_dict->dv_refcount++;
	    ត្រឡប់ NEW_FUNCTION(tv->vval.v_partial == NULL
			     ? (char_u *)"" : partial_name(tv->vval.v_partial),
				tv->vval.v_partial->pt_argc, argv,
				tv->vval.v_partial->pt_dict,
				tv->vval.v_partial->pt_auto);
	pak_sa VAR_BLOB:
	    ត្រឡប់ PyBytes_FromStringAndSize(
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
	    ត្រឡប់ Py_None;
	pak_sa VAR_BOOL:
	pak_sa VAR_SPECIAL:
	    switch (tv->vval.v_number)
	    {
		pak_sa VVAL_FALSE: ត្រឡប់ ALWAYS_FALSE;
		pak_sa VVAL_TRUE:  ត្រឡប់ ALWAYS_TRUE;
		pak_sa VVAL_NONE:
		pak_sa VVAL_NULL:  ត្រឡប់ ALWAYS_NONE;
	    }
	    PyErr_SET_VIM(N_("internal error: invalid value type"));
	    ត្រឡប់ NULL;
    }
    ត្រឡប់ NULL;
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
#ប្រសិនបើ PY_MAJOR_VERSION >= 3
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
#បើមិនដូច្នេះ
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
#ប្រសិនបើ PY_MAJOR_VERSION >= 3
    BufferType.tp_getattro = BufferGetattro;
    BufferType.tp_setattro = BufferSetattro;
    BufferType.tp_alloc = call_PyType_GenericAlloc;
    BufferType.tp_new = call_PyType_GenericNew;
    BufferType.tp_free = call_PyObject_Free;
#បើមិនដូច្នេះ
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
#ប្រសិនបើ PY_MAJOR_VERSION >= 3
    WindowType.tp_getattro = WindowGetattro;
    WindowType.tp_setattro = WindowSetattro;
    WindowType.tp_alloc = call_PyType_GenericAlloc;
    WindowType.tp_new = call_PyType_GenericNew;
    WindowType.tp_free = call_PyObject_Free;
#បើមិនដូច្នេះ
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
#ប្រសិនបើ PY_MAJOR_VERSION >= 3
    TabPageType.tp_getattro = TabPageGetattro;
    TabPageType.tp_alloc = call_PyType_GenericAlloc;
    TabPageType.tp_new = call_PyType_GenericNew;
    TabPageType.tp_free = call_PyObject_Free;
#បើមិនដូច្នេះ
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
#ប្រសិនបើ PY_MAJOR_VERSION >= 3
    RangeType.tp_getattro = RangeGetattro;
    RangeType.tp_alloc = call_PyType_GenericAlloc;
    RangeType.tp_new = call_PyType_GenericNew;
    RangeType.tp_free = call_PyObject_Free;
#បើមិនដូច្នេះ
    RangeType.tp_getattr = RangeGetattr;
#endif

    CLEAR_FIELD(CurrentType);
    CurrentType.tp_name = "vim.currentdata";
    CurrentType.tp_basicsize = parimaa_na(CurrentObject);
    CurrentType.tp_flags = Py_TPFLAGS_DEFAULT;
    CurrentType.tp_doc = "vim current object";
    CurrentType.tp_methods = CurrentMethods;
#ប្រសិនបើ PY_MAJOR_VERSION >= 3
    CurrentType.tp_getattro = CurrentGetattro;
    CurrentType.tp_setattro = CurrentSetattro;
#បើមិនដូច្នេះ
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
#ប្រសិនបើ PY_MAJOR_VERSION >= 3
    DictionaryType.tp_getattro = DictionaryGetattro;
    DictionaryType.tp_setattro = DictionarySetattro;
#បើមិនដូច្នេះ
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
#ប្រសិនបើ PY_MAJOR_VERSION >= 3
    ListType.tp_getattro = ListGetattro;
    ListType.tp_setattro = ListSetattro;
#បើមិនដូច្នេះ
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
#ប្រសិនបើ PY_MAJOR_VERSION >= 3
    TupleType.tp_getattro = TupleGetattro;
    TupleType.tp_setattro = TupleSetattro;
#បើមិនដូច្នេះ
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
#ប្រសិនបើ PY_MAJOR_VERSION >= 3
    FunctionType.tp_getattro = FunctionGetattro;
#បើមិនដូច្នេះ
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

#ប្រសិនបើ PY_VERSION_HEX < 0x030700f0
    CLEAR_FIELD(LoaderType);
    LoaderType.tp_name = "vim.Loader";
    LoaderType.tp_basicsize = parimaa_na(LoaderObject);
    LoaderType.tp_flags = Py_TPFLAGS_DEFAULT;
    LoaderType.tp_doc = "vim message object";
    LoaderType.tp_methods = LoaderMethods;
    LoaderType.tp_dealloc = LoaderDestructor;
#endif

#ប្រសិនបើ PY_MAJOR_VERSION >= 3
    CLEAR_FIELD(vimmodule);
    vimmodule.m_name = "vim";
    vimmodule.m_doc = "Vim Python interface\n";
    vimmodule.m_size = -1;
    vimmodule.m_methods = VimMethods;
#endif
}

    static ចំនួនគត់
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
#ប្រសិនបើ PY_VERSION_HEX < 0x030700f0
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
    ត្រឡប់ 0;
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
# ប្រសិនបើ PY_VERSION_HEX < 0x030700f0
    PYTYPE_CLEANUP(LoaderType);
# endif
}
#endif

    static ចំនួនគត់
init_sys_path(void)
{
    PyObject	*path;
    PyObject	*path_hook;
    PyObject	*path_hooks;

    ប្រសិនបើ (!(path_hook = PyObject_GetAttrString(vim_module, "path_hook")))
	ត្រឡប់ -1;

    ប្រសិនបើ (!(path_hooks = PySys_GetObject("path_hooks")))
    {
	PyErr_Clear();
	path_hooks = PyList_New(1);
	PyList_SET_ITEM(path_hooks, 0, path_hook);
	ប្រសិនបើ (PySys_SetObject("path_hooks", path_hooks))
	{
	    Py_DECREF(path_hooks);
	    ត្រឡប់ -1;
	}
	Py_DECREF(path_hooks);
    }
    បើមិនដូច្នេះ ប្រសិនបើ (PyList_Check(path_hooks))
    {
	ប្រសិនបើ (PyList_Append(path_hooks, path_hook))
	{
	    Py_DECREF(path_hook);
	    ត្រឡប់ -1;
	}
	Py_DECREF(path_hook);
    }
    បើមិនដូច្នេះ
    {
	VimTryStart();
	emsg(_("Failed to set path hook: sys.path_hooks is not a list\n"
	       "You should now do the following:\n"
	       "- append vim.path_hook to sys.path_hooks\n"
	       "- append vim.VIM_SPECIAL_PATH to sys.path\n"));
	VimTryEnd(); // Discard the error
	Py_DECREF(path_hook);
	ត្រឡប់ 0;
    }

    ប្រសិនបើ (!(path = PySys_GetObject("path")))
    {
	PyErr_Clear();
	path = PyList_New(1);
	Py_INCREF(vim_special_path_object);
	PyList_SET_ITEM(path, 0, vim_special_path_object);
	ប្រសិនបើ (PySys_SetObject("path", path))
	{
	    Py_DECREF(path);
	    ត្រឡប់ -1;
	}
	Py_DECREF(path);
    }
    បើមិនដូច្នេះ ប្រសិនបើ (PyList_Check(path))
    {
	ប្រសិនបើ (PyList_Append(path, vim_special_path_object))
	    ត្រឡប់ -1;
    }
    បើមិនដូច្នេះ
    {
	VimTryStart();
	emsg(_("Failed to set path: sys.path is not a list\n"
	       "You should now append vim.VIM_SPECIAL_PATH to sys.path"));
	VimTryEnd(); // Discard the error
    }

    ត្រឡប់ 0;
}

static sa_nracanaa numeric_constant {
    var_na	*name;
    ចំនួនគត់		val;
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
    ប្រសិនបើ (PyModule_AddObject(m, name, obj)) \
	ត្រឡប់ -1;

#define ADD_CHECKED_OBJECT(m, name, obj) \
    { \
	PyObject	*valObject = obj; \
	ប្រសិនបើ (!valObject) \
	    ត្រឡប់ -1; \
	ADD_OBJECT(m, name, valObject); \
    }

    static ចំនួនគត់
populate_module(PyObject *m)
{
    ចំនួនគត់		i;
    PyObject	*other_module;
    PyObject	*attr;
    PyObject	*imp;
#ប្រសិនបើ PY_VERSION_HEX >= 0x030700f0
    PyObject	*dict;
    PyObject	*cls;
#endif

    សម្រាប់ (i = 0; i < (ចំនួនគត់)(parimaa_na(numeric_constants)
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
#ប្រសិនបើ PY_VERSION_HEX < 0x030700f0
	{"_Loader",    (PyObject *)LoaderTypePtr},
#endif
    };

    សម្រាប់ (i = 0; i < (ចំនួនគត់)(parimaa_na(object_constants)
					    / parimaa_na(sa_nracanaa object_constant));
	    ++i)
    {
	PyObject	*valObject;

	valObject = object_constants[i].valObject;
	Py_INCREF(valObject);
	ADD_OBJECT(m, object_constants[i].name, valObject);
    }

    ប្រសិនបើ (!(VimError = PyErr_NewException("vim.error", NULL, NULL)))
	ត្រឡប់ -1;
    ADD_OBJECT(m, "error", VimError);

    ADD_CHECKED_OBJECT(m, "vars",  NEW_DICTIONARY(get_globvar_dict()));
    ADD_CHECKED_OBJECT(m, "vvars", NEW_DICTIONARY(get_vimvar_dict()));
    ADD_CHECKED_OBJECT(m, "options",
	    OptionsNew(SREQ_GLOBAL, NULL, dummy_check, NULL));

    ប្រសិនបើ (!(other_module = PyImport_ImportModule("os")))
	ត្រឡប់ -1;
    ADD_OBJECT(m, "os", other_module);

#ប្រសិនបើ PY_MAJOR_VERSION >= 3
    ប្រសិនបើ (!(py_getcwd = PyObject_GetAttrString(other_module, "getcwd")))
	ត្រឡប់ -1;
#បើមិនដូច្នេះ
    ប្រសិនបើ (!(py_getcwd = PyObject_GetAttrString(other_module, "getcwdu")))
	ត្រឡប់ -1;
#endif
    ADD_OBJECT(m, "_getcwd", py_getcwd)

    ប្រសិនបើ (!(py_chdir = PyObject_GetAttrString(other_module, "chdir")))
	ត្រឡប់ -1;
    ADD_OBJECT(m, "_chdir", py_chdir);
    ប្រសិនបើ (!(attr = PyObject_GetAttrString(m, "chdir")))
	ត្រឡប់ -1;
    ប្រសិនបើ (PyObject_SetAttrString(other_module, "chdir", attr))
    {
	Py_DECREF(attr);
	ត្រឡប់ -1;
    }
    Py_DECREF(attr);

    ប្រសិនបើ ((py_fchdir = PyObject_GetAttrString(other_module, "fchdir")))
    {
	ADD_OBJECT(m, "_fchdir", py_fchdir);
	ប្រសិនបើ (!(attr = PyObject_GetAttrString(m, "fchdir")))
	    ត្រឡប់ -1;
	ប្រសិនបើ (PyObject_SetAttrString(other_module, "fchdir", attr))
	{
	    Py_DECREF(attr);
	    ត្រឡប់ -1;
	}
	Py_DECREF(attr);
    }
    បើមិនដូច្នេះ
	PyErr_Clear();

    ប្រសិនបើ (!(vim_special_path_object = PyString_FromString(vim_special_path)))
	ត្រឡប់ -1;

    ADD_OBJECT(m, "VIM_SPECIAL_PATH", vim_special_path_object);

#ប្រសិនបើ PY_VERSION_HEX >= 0x030700f0
    ប្រសិនបើ (!(imp = PyImport_ImportModule("importlib.machinery")))
	ត្រឡប់ -1;

    dict = PyModule_GetDict(imp);

    ប្រសិនបើ (!(cls = PyDict_GetItemString(dict, "PathFinder")))
    {
	Py_DECREF(imp);
	ត្រឡប់ -1;
    }

    ប្រសិនបើ (!(py_find_spec = PyObject_GetAttrString(cls, "find_spec")))
    {
	Py_DECREF(imp);
	ត្រឡប់ -1;
    }

# ប្រសិនបើ PY_VERSION_HEX < 0x30c00a7
    // find_module has been removed as of Python 3.12.0a7
    ប្រសិនបើ ((py_find_module = PyObject_GetAttrString(cls, "find_module")))
	ADD_OBJECT(m, "_find_module", py_find_module);
# endif

    Py_DECREF(imp);

    ADD_OBJECT(m, "_find_spec", py_find_spec);
#បើមិនដូច្នេះ
    ប្រសិនបើ (!(imp = PyImport_ImportModule("imp")))
	ត្រឡប់ -1;

    ប្រសិនបើ (!(py_find_module = PyObject_GetAttrString(imp, "find_module")))
    {
	Py_DECREF(imp);
	ត្រឡប់ -1;
    }

    ប្រសិនបើ (!(py_load_module = PyObject_GetAttrString(imp, "load_module")))
    {
	Py_DECREF(py_find_module);
	Py_DECREF(imp);
	ត្រឡប់ -1;
    }

    Py_DECREF(imp);

    ADD_OBJECT(m, "_find_module", py_find_module);
    ADD_OBJECT(m, "_load_module", py_load_module);
#endif

    ត្រឡប់ 0;
}
