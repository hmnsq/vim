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

#නම් PY_VERSION_HEX < 0x02050000
typedef පූර්ණ_සංඛ්‍යා Py_ssize_t;  // Python 2.4 and earlier don't have this type.
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
    නම් (obj == NULL)
	ආපසු PyErr_NoMemory();
    ආපසු PyObject_Init(obj, type);
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
	නම් (qualname == NULL) \
	{ \
	    PyErr_FORMAT(PyExc_TypeError, msg, "(NULL)"); \
	    bha_nga; \
	} \
	PyObject* module = PyObject_GetAttrString((PyObject*)(obj)->ob_type, "__module__"); \
	PyObject* full; \
	නම් (module == NULL || PyUnicode_CompareWithASCIIString(module, "builtins") == 0 \
		|| PyUnicode_CompareWithASCIIString(module, "__main__") == 0) \
	{ \
	    full = qualname; \
	    Py_INCREF(full); \
	} \
	නැතහොත් \
	    full = PyUnicode_FromFormat("%U.%U", module, qualname); \
	PyObject* full_bytes = PyUnicode_AsUTF8String(full); \
	const var_na* full_str = PyBytes_AsString(full_bytes); \
	full_str = full_str == NULL ? "(NULL)" : full_str; \
	PyErr_FORMAT(PyExc_TypeError, msg, full_str); \
	Py_DECREF(qualname); \
	Py_XDECREF(module); \
	Py_XDECREF(full); \
	Py_XDECREF(full_bytes); \
    } අතරතුර (0)

# define PyList_GET_ITEM(list, i) PyList_GetItem(list, i)
# define PyList_GET_SIZE(o) PyList_Size(o)

// PyList_SET_ITEM and PyList_SetItem have slightly different behaviors. The
// former will leave the old item dangling, and the latter will decref on it.
// Since we only use this on new lists, this difference doesn't matter.
# define PyList_SET_ITEM(list, i, item) PyList_SetItem(list, i, item)

# define PyTuple_GET_ITEM(o, pos) PyTuple_GetItem(o, pos)
# define PyTuple_GET_SIZE(o) PyTuple_Size(o)

# නම් Py_LIMITED_API < 0x03080000
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
    නම් (slot_i >= 40) ආපසු NULL; /* this should never happen */ \
    නම් (type_object->slot_name != NULL) \
    { \
	slots[slot_i].slot = Py_##slot_name; \
	slots[slot_i].pfunc = (void*)type_object->slot_name; \
	++slot_i; \
    }
# define ADD_TP_SUB_SLOT(sub_slot, slot_name) \
    නම් (slot_i >= 40) ආපසු NULL; /* this should never happen */ \
    නම් (type_object->sub_slot != NULL && type_object->sub_slot->slot_name != NULL) \
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
    ආපසු (PyTypeObject*)newtype;
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
    නම් (type##Ptr == NULL) \
	ආපසු -1;
# define PYTYPE_CLEANUP(type) \
    type##Ptr = NULL;

// Limited API does not provide PyRun_* functions. Need to implement manually
// using PyCompile and PyEval.
static PyObject* Vim_PyRun_String(const var_na *str, පූර්ණ_සංඛ්‍යා start, PyObject *globals, PyObject *locals)
{
    // Just pass "" for filename for now.
    PyObject* compiled = Py_CompileString(str, "", start);
    නම් (compiled == NULL)
	ආපසු NULL;

    PyObject* eval_result = PyEval_EvalCode(compiled, globals, locals);
    Py_DECREF(compiled);
    ආපසු eval_result;
}
static පූර්ණ_සංඛ්‍යා Vim_PyRun_SimpleString(const var_na *str)
{
    // This function emulates CPython's implementation.
    PyObject* m = PyImport_AddModule("__main__");
    නම් (m == NULL)
	ආපසු -1;
    PyObject* d = PyModule_GetDict(m);
    PyObject* output = Vim_PyRun_String(str, Py_file_input, d, d);
    නම් (output == NULL)
    {
	PyErr_PrintEx(TRUE);
	ආපසු -1;
    }
    Py_DECREF(output);
    ආපසු 0;
}
# define PyRun_String Vim_PyRun_String
# define PyRun_SimpleString Vim_PyRun_SimpleString

#නැතහොත් // !defined(USE_LIMITED_API)

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
    නම් (PyType_Ready(type##Ptr)) \
	ආපසු -1;

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

static පූර්ණ_සංඛ්‍යා ConvertFromPyObject(PyObject *, typval_T *);
static පූර්ණ_සංඛ්‍යා _ConvertFromPyObject(PyObject *, typval_T *, PyObject *);
static පූර්ණ_සංඛ්‍යා ConvertFromPyMapping(PyObject *, typval_T *);
static පූර්ණ_සංඛ්‍යා ConvertFromPySequence(PyObject *, typval_T *);
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

#නම් PY_VERSION_HEX >= 0x030700f0
static PyObject *py_find_spec;
#නැතහොත්
static PyObject *py_load_module;
#endif
#නම් PY_VERSION_HEX < 0x30c00a7
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

    නම් (PyBytes_Check(obj))
    {

	නම් (PyBytes_AsStringAndSize(obj, (var_na **) &str, NULL) == -1
		|| str == NULL)
	    ආපසු NULL;

	*todecref = NULL;
    }
    නැතහොත් නම් (PyUnicode_Check(obj))
    {
	PyObject	*bytes;

	නම් (!(bytes = PyUnicode_AsEncodedString(obj, ENC_OPT,
							   ERRORS_ENCODE_ARG)))
	    ආපසු NULL;

	නම් (PyBytes_AsStringAndSize(bytes, (var_na **) &str, NULL) == -1
		|| str == NULL)
	{
	    Py_DECREF(bytes);
	    ආපසු NULL;
	}

	*todecref = bytes;
    }
    නැතහොත්
    {
#නම් PY_MAJOR_VERSION < 3
	PyErr_FORMAT_TYPE(N_("expected str() or unicode() instance, but got %s"),
		obj);
#නැතහොත්
	PyErr_FORMAT_TYPE(N_("expected bytes() or str() instance, but got %s"),
		obj);
#endif
	ආපසු NULL;
    }

    ආපසු (char_u *) str;
}

#define NUMBER_LONG     1
#define NUMBER_INT      2
#define NUMBER_NATURAL  4
#define NUMBER_UNSIGNED 8

    static පූර්ණ_සංඛ්‍යා
NumberToLong(PyObject *obj, long *result, පූර්ණ_සංඛ්‍යා flags)
{
#නම් PY_MAJOR_VERSION < 3
    නම් (PyInt_Check(obj))
    {
	*result = PyInt_AsLong(obj);
	නම් (PyErr_Occurred())
	    ආපසු -1;
    }
    නැතහොත්
#endif
    නම් (PyLong_Check(obj))
    {
	*result = PyLong_AsLong(obj);
	නම් (PyErr_Occurred())
	    ආපසු -1;
    }
    නැතහොත් නම් (PyNumber_Check(obj))
    {
	PyObject	*num;

	නම් (!(num = PyNumber_Long(obj)))
	    ආපසු -1;

	*result = PyLong_AsLong(num);

	Py_DECREF(num);

	නම් (PyErr_Occurred())
	    ආපසු -1;
    }
    නැතහොත්
    {
#නම් PY_MAJOR_VERSION < 3
	PyErr_FORMAT_TYPE(
		N_("expected int(), long() or something supporting "
		   "coercing to long(), but got %s"),
		obj);
#නැතහොත්
	PyErr_FORMAT_TYPE(
		N_("expected int() or something supporting coercing to int(), "
		   "but got %s"),
		obj);
#endif
	ආපසු -1;
    }

    නම් (flags & NUMBER_INT)
    {
	නම් (*result > INT_MAX)
	{
	    PyErr_SET_STRING(PyExc_OverflowError,
		    N_("value is too large to fit into C int type"));
	    ආපසු -1;
	}
	නැතහොත් නම් (*result < INT_MIN)
	{
	    PyErr_SET_STRING(PyExc_OverflowError,
		    N_("value is too small to fit into C int type"));
	    ආපසු -1;
	}
    }

    නම් (flags & NUMBER_NATURAL)
    {
	නම් (*result <= 0)
	{
	    PyErr_SET_STRING(PyExc_ValueError,
		    N_("number must be greater than zero"));
	    ආපසු -1;
	}
    }
    නැතහොත් නම් (flags & NUMBER_UNSIGNED)
    {
	නම් (*result < 0)
	{
	    PyErr_SET_STRING(PyExc_ValueError,
		    N_("number must be greater or equal to zero"));
	    ආපසු -1;
	}
    }

    ආපසු 0;
}

    static පූර්ණ_සංඛ්‍යා
add_string(PyObject *list, var_na *s)
{
    PyObject	*string;

    නම් (!(string = PyString_FromString(s)))
	ආපසු -1;

    නම් (PyList_Append(list, string))
    {
	Py_DECREF(string);
	ආපසු -1;
    }

    Py_DECREF(string);
    ආපසු 0;
}

    static PyObject *
ObjectDir(PyObject *self, var_na **attributes)
{
    PyMethodDef	*method;
    var_na	**attr;
    PyObject	*ret;

    නම් (!(ret = PyList_New(0)))
	ආපසු NULL;

    නම් (self)
	සඳහා (method = Py_TYPE_GET_TP_METHODS(self->ob_type) ; method->ml_name != NULL ; ++method)
	    නම් (add_string(ret, (var_na *)method->ml_name))
	    {
		Py_DECREF(ret);
		ආපසු NULL;
	    }

    සඳහා (attr = attributes ; *attr ; ++attr)
	නම් (add_string(ret, *attr))
	{
	    Py_DECREF(ret);
	    ආපසු NULL;
	}

#නම් PY_MAJOR_VERSION < 3
    නම් (add_string(ret, "__members__"))
    {
	Py_DECREF(ret);
	ආපසු NULL;
    }
#endif

    ආපසු ret;
}

// Output buffer management

// Function to write a line, points to either msg() or emsg().
typedef පූර්ණ_සංඛ්‍යා (*writefn)(var_na *);

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
    ආපසු ObjectDir(self, OutputAttrs);
}

    static පූර්ණ_සංඛ්‍යා
OutputSetattr(PyObject *self_obj, var_na *name, PyObject *valObject)
{
    OutputObject *self = (OutputObject*)self_obj;
    නම් (valObject == NULL)
    {
	PyErr_SET_STRING(PyExc_AttributeError,
		N_("can't delete OutputObject attributes"));
	ආපසු -1;
    }

    නම් (strcmp(name, "softspace") == 0)
    {
	නම් (NumberToLong(valObject, &(self->softspace), NUMBER_UNSIGNED))
	    ආපසු -1;
	ආපසු 0;
    }

    PyErr_FORMAT(PyExc_AttributeError, N_("invalid attribute: %s"), name);
    ආපසු -1;
}

// Buffer IO, we write one whole line at a time.
static garray_T io_ga = {0, 0, 1, 80, NULL};
static writefn old_fn = NULL;

    static void
PythonIO_Flush(void)
{
    නම් (old_fn != NULL && io_ga.ga_len > 0)
    {
	((var_na *)io_ga.ga_data)[io_ga.ga_len] = NUL;
	// We don't know what emsg_severe should be here, so ... hope?
	old_fn((var_na *)io_ga.ga_data);
    }
    io_ga.ga_len = 0;
}

    static void
writer(writefn fn, char_u *str, PyInt n, පූර්ණ_සංඛ්‍යා severe)
{
    char_u *ptr;

    // Flush when switching output function.
    නම් (fn != old_fn)
	PythonIO_Flush();
    old_fn = fn;

    // Write each NL separated line.  Text after the last NL is kept for
    // writing later.
    // For normal messages: Do not output when "got_int" was set.  This avoids
    // a loop gone crazy flooding the terminal with messages.  Also for when
    // "q" is pressed at the more-prompt.
    අතරතුර (n > 0 && (ptr = memchr(str, '\n', n)) != NULL
					  && (fn == (writefn)emsg || !got_int))
    {
	PyInt len = ptr - str;

	නම් (ga_grow(&io_ga, (පූර්ණ_සංඛ්‍යා)(len + 1)) == FAIL)
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
    නම් (n > 0 && (fn == (writefn)emsg || !got_int)
					&& ga_grow(&io_ga, (පූර්ණ_සංඛ්‍යා)(n + 1)) == OK)
    {
	mch_memmove(((var_na *)io_ga.ga_data) + io_ga.ga_len, str, (size_t)n);
	io_ga.ga_len += (පූර්ණ_සංඛ්‍යා)n;
    }
}

    static පූර්ණ_සංඛ්‍යා
write_output(OutputObject *self, PyObject *string)
{
    Py_ssize_t	len = 0;
    var_na	*str = NULL;
    පූර්ණ_සංඛ්‍යා		error = self->error;

    නම් (!PyArg_Parse(string, "et#", ENC_OPT, &str, &len))
	ආපසු -1;

    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    writer((writefn)(error ? emsg : msg), (char_u *)str, len, error);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS
    PyMem_Free(str);

    ආපසු 0;
}

    static PyObject *
OutputWrite(OutputObject *self, PyObject *string)
{
    නම් (write_output(self, string))
	ආපසු NULL;

    Py_INCREF(Py_None);
    ආපසු Py_None;
}

    static PyObject *
OutputWritelines(OutputObject *self, PyObject *seq)
{
    PyObject	*iterator;
    PyObject	*item;

    නම් (!(iterator = PyObject_GetIter(seq)))
	ආපසු NULL;

    අතරතුර ((item = PyIter_Next(iterator)))
    {
	නම් (write_output(self, item))
	{
	    Py_DECREF(iterator);
	    Py_DECREF(item);
	    ආපසු NULL;
	}
	Py_DECREF(item);
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    නම් (PyErr_Occurred())
	ආපසු NULL;

    Py_INCREF(Py_None);
    ආපසු Py_None;
}

    static PyObject *
AlwaysNone(PyObject *self UNUSED, PyObject *args UNUSED)
{
    // do nothing
    Py_INCREF(Py_None);
    ආපසු Py_None;
}
#define ALWAYS_NONE AlwaysNone(NULL, NULL)

    static PyObject *
AlwaysFalse(PyObject *self UNUSED, PyObject *args UNUSED)
{
    // do nothing
    PyObject	*ret = Py_False;
    Py_INCREF(ret);
    ආපසු ret;
}
#define ALWAYS_FALSE AlwaysFalse(NULL, NULL)

    static PyObject *
AlwaysTrue(PyObject *self UNUSED, PyObject *args UNUSED)
{
    // do nothing
    PyObject	*ret = Py_True;
    Py_INCREF(ret);
    ආපසු ret;
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

    static පූර්ණ_සංඛ්‍යා
PythonIO_Init_io(void)
{
    නම් (PySys_SetObject("stdout", (PyObject *)(void *)&Output))
	ආපසු -1;
    නම් (PySys_SetObject("stderr", (PyObject *)(void *)&Error))
	ආපසු -1;

    නම් (PyErr_Occurred())
    {
	emsg(_(e_python_error_initialising_io_object));
	ආපසු -1;
    }

    ආපසු 0;
}

#නම් PY_VERSION_HEX < 0x030700f0
static PyObject *call_load_module(var_na *name, පූර්ණ_සංඛ්‍යා len, PyObject *find_module_result);

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

    නම් (!fullname)
    {
	module = result ? result : Py_None;
	Py_INCREF(module);
	ආපසු module;
    }

    module = call_load_module(fullname, (පූර්ණ_සංඛ්‍යා)STRLEN(fullname), result);

    self->fullname = NULL;
    self->result = module;

    vim_free(fullname);
    Py_DECREF(result);

    නම් (!module)
    {
	නම් (PyErr_Occurred())
	    ආපසු NULL;

	Py_INCREF(Py_None);
	ආපසු Py_None;
    }

    Py_INCREF(module);
    ආපසු module;
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

    static පූර්ණ_සංඛ්‍යා
VimTryEnd(void)
{
    --trylevel;
    // Without this it stops processing all subsequent Vim script commands and
    // generates strange error messages if I e.g. try calling Test() in a cycle
    did_emsg = FALSE;
    // Keyboard interrupt should be preferred over anything else
    නම් (got_int)
    {
	නම් (did_throw)
	    discard_current_exception();
	got_int = FALSE;
	PyErr_SetNone(PyExc_KeyboardInterrupt);
	ආපසු -1;
    }
    නැතහොත් නම් (msg_list != NULL && *msg_list != NULL)
    {
	පූර්ණ_සංඛ්‍යා	should_free;
	var_na	*msg;

	msg = get_exception_string(*msg_list, ET_ERROR, NULL, &should_free);

	නම් (msg == NULL)
	{
	    PyErr_NoMemory();
	    ආපසු -1;
	}

	PyErr_SetVim(msg);

	free_global_msglist();

	නම් (should_free)
	    vim_free(msg);

	ආපසු -1;
    }
    නැතහොත් නම් (!did_throw)
	ආපසු (PyErr_Occurred() ? -1 : 0);
    // Python exception is preferred over Vim one; unlikely to occur though
    නැතහොත් නම් (PyErr_Occurred())
    {
	discard_current_exception();
	ආපසු -1;
    }
    // Finally transform Vim script exception to python one
    නැතහොත්
    {
	PyErr_SetVim((var_na *)current_exception->value);
	discard_current_exception();
	ආපසු -1;
    }
}

    static පූර්ණ_සංඛ්‍යා
VimCheckInterrupt(void)
{
    නම් (got_int)
    {
	PyErr_SetNone(PyExc_KeyboardInterrupt);
	ආපසු 1;
    }
    ආපසු 0;
}

// Vim module - Implementation

    static PyObject *
VimCommand(PyObject *self UNUSED, PyObject *string)
{
    char_u	*cmd;
    PyObject	*ret;
    PyObject	*todecref;

    නම් (!(cmd = StringToChars(string, &todecref)))
	ආපසු NULL;

    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();

    VimTryStart();
    do_cmdline_cmd(cmd);
    update_screen(UPD_VALID);

    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    නම් (VimTryEnd())
	ret = NULL;
    නැතහොත්
	ret = Py_None;

    Py_XINCREF(ret);
    Py_XDECREF(todecref);
    ආපසු ret;
}

/*
 * Function to translate a typval_T into a PyObject; this will recursively
 * translate lists/dictionaries into their Python equivalents.
 *
 * The depth parameter is to avoid infinite recursion, set it to 1 when
 * you call VimToPython.
 */
    static PyObject *
VimToPython(typval_T *our_tv, පූර්ණ_සංඛ්‍යා depth, PyObject *lookup_dict)
{
    PyObject	*ret;
    PyObject	*newObj;
    var_na	ptrBuf[parimaa_na(void *) * 2 + 3];

    // Avoid infinite recursion
    නම් (depth > 100)
    {
	Py_INCREF(Py_None);
	ret = Py_None;
	ආපසු ret;
    }

    // Check if we run into a recursive loop.  The item must be in lookup_dict
    // then and we can use it again.
    නම් ((our_tv->v_type == VAR_LIST && our_tv->vval.v_list != NULL)
	    || (our_tv->v_type == VAR_TUPLE && our_tv->vval.v_tuple != NULL)
	    || (our_tv->v_type == VAR_DICT && our_tv->vval.v_dict != NULL))
    {
	sprintf(ptrBuf, "%p",
		our_tv->v_type == VAR_LIST ? (void *)our_tv->vval.v_list
		    : our_tv->v_type == VAR_TUPLE ?
			(void *)our_tv->vval.v_tuple
			: (void *)our_tv->vval.v_dict);

	නම් ((ret = PyDict_GetItemString(lookup_dict, ptrBuf)))
	{
	    Py_INCREF(ret);
	    ආපසු ret;
	}
    }

    නම් (our_tv->v_type == VAR_STRING)
	ret = PyString_FromString(our_tv->vval.v_string == NULL
					? "" : (var_na *)our_tv->vval.v_string);
    නැතහොත් නම් (our_tv->v_type == VAR_NUMBER)
    {
	var_na buf[NUMBUFLEN];

	// For backwards compatibility numbers are stored as strings.
	sprintf(buf, "%lld", (long long)our_tv->vval.v_number);
	ret = PyString_FromString((var_na *)buf);
    }
    නැතහොත් නම් (our_tv->v_type == VAR_FLOAT)
    {
	var_na buf[NUMBUFLEN];

	sprintf(buf, "%f", our_tv->vval.v_float);
	ret = PyString_FromString((var_na *)buf);
    }
    නැතහොත් නම් (our_tv->v_type == VAR_LIST)
    {
	list_T		*list = our_tv->vval.v_list;
	listitem_T	*curr;

	නම් (list == NULL)
	    ආපසු NULL;

	නම් (!(ret = PyList_New(0)))
	    ආපසු NULL;

	නම් (PyDict_SetItemString(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    ආපසු NULL;
	}

	CHECK_LIST_MATERIALIZE(list);
	FOR_ALL_LIST_ITEMS(list, curr)
	{
	    නම් (!(newObj = VimToPython(&curr->li_tv, depth + 1, lookup_dict)))
	    {
		Py_DECREF(ret);
		ආපසු NULL;
	    }
	    නම් (PyList_Append(ret, newObj))
	    {
		Py_DECREF(newObj);
		Py_DECREF(ret);
		ආපසු NULL;
	    }
	    Py_DECREF(newObj);
	}
    }
    නැතහොත් නම් (our_tv->v_type == VAR_TUPLE)
    {
	tuple_T	*tuple = our_tv->vval.v_tuple;
	පූර්ණ_සංඛ්‍යා	len;

	නම් (tuple == NULL)
	    ආපසු NULL;

	len = TUPLE_LEN(tuple);

	ret = PyTuple_New(len);
	නම් (ret == NULL)
	    ආපසු NULL;

	සඳහා (පූර්ණ_සංඛ්‍යා idx = 0; idx < len; idx++)
	{
	    typval_T	*item_tv = TUPLE_ITEM(tuple, idx);

	    newObj = VimToPython(item_tv, depth + 1, lookup_dict);
	    නම් (!newObj)
	    {
		Py_DECREF(ret);
		ආපසු NULL;
	    }
	    PyTuple_SetItem(ret, idx, newObj);
	}

	නම් (PyDict_SetItemString(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    ආපසු NULL;
	}
    }
    නැතහොත් නම් (our_tv->v_type == VAR_DICT)
    {

	hashtab_T	*ht;
	long_u		todo;
	hashitem_T	*hi;
	dictitem_T	*di;

	නම් (our_tv->vval.v_dict == NULL)
	    ආපසු NULL;
	ht = &our_tv->vval.v_dict->dv_hashtab;

	නම් (!(ret = PyDict_New()))
	    ආපසු NULL;

	නම් (PyDict_SetItemString(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    ආපසු NULL;
	}

	todo = ht->ht_used;
	සඳහා (hi = ht->ht_array; todo > 0; ++hi)
	{
	    නම් (!HASHITEM_EMPTY(hi))
	    {
		--todo;

		di = dict_lookup(hi);
		නම් (!(newObj = VimToPython(&di->di_tv, depth + 1, lookup_dict)))
		{
		    Py_DECREF(ret);
		    ආපසු NULL;
		}
		නම් (PyDict_SetItemString(ret, (var_na *)hi->hi_key, newObj))
		{
		    Py_DECREF(ret);
		    Py_DECREF(newObj);
		    ආපසු NULL;
		}
	    }
	}
    }
    නැතහොත් නම් (our_tv->v_type == VAR_BOOL)
    {
	නම් (our_tv->vval.v_number == VVAL_FALSE)
	{
	    ret = Py_False;
	    Py_INCREF(ret);
	}
	නැතහොත්
	{
	    ret = Py_True;
	    Py_INCREF(ret);
	}
	ආපසු ret;
    }
    නැතහොත් නම් (our_tv->v_type == VAR_SPECIAL)
    {
	Py_INCREF(Py_None);
	ret = Py_None;
	ආපසු ret;
    }
    නැතහොත් නම් (our_tv->v_type == VAR_BLOB)
	ret = PyBytes_FromStringAndSize(
		(var_na*) our_tv->vval.v_blob->bv_ga.ga_data,
		(Py_ssize_t) our_tv->vval.v_blob->bv_ga.ga_len);
    නැතහොත්
    {
	Py_INCREF(Py_None);
	ret = Py_None;
    }

    ආපසු ret;
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

    නම් (!PyArg_ParseTuple(args, "O", &string))
	ආපසු NULL;

    නම් (!(expr = StringToChars(string, &todecref)))
	ආපසු NULL;

    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    VimTryStart();
    our_tv = eval_expr(expr, NULL);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    Py_XDECREF(todecref);

    නම් (VimTryEnd())
	ආපසු NULL;

    නම් (our_tv == NULL)
    {
	PyErr_SET_VIM(N_("invalid expression"));
	ආපසු NULL;
    }

    // Convert the Vim type into a Python type.  Create a dictionary that's
    // used to check for recursive loops.
    නම් (!(lookup_dict = PyDict_New()))
	ret = NULL;
    නැතහොත්
    {
	ret = VimToPython(our_tv, 1, lookup_dict);
	Py_DECREF(lookup_dict);
    }


    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    free_tv(our_tv);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    ආපසු ret;
}

static PyObject *ConvertToPyObject(typval_T *);

    static PyObject *
VimEvalPy(PyObject *self UNUSED, PyObject *string)
{
    typval_T	*our_tv;
    PyObject	*ret;
    char_u	*expr;
    PyObject	*todecref;

    නම් (!(expr = StringToChars(string, &todecref)))
	ආපසු NULL;

    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    VimTryStart();
    our_tv = eval_expr(expr, NULL);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    Py_XDECREF(todecref);

    නම් (VimTryEnd())
	ආපසු NULL;

    නම් (our_tv == NULL)
    {
	PyErr_SET_VIM(N_("invalid expression"));
	ආපසු NULL;
    }

    ret = ConvertToPyObject(our_tv);
    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    free_tv(our_tv);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    ආපසු ret;
}

    static PyObject *
VimStrwidth(PyObject *self UNUSED, PyObject *string)
{
    char_u	*str;
    PyObject	*todecref;
    පූර්ණ_සංඛ්‍යා		len;

    නම් (!(str = StringToChars(string, &todecref)))
	ආපසු NULL;

    len = mb_string2cells(str, (පූර්ණ_සංඛ්‍යා)STRLEN(str));

    Py_XDECREF(todecref);

    ආපසු PyLong_FromLong(len);
}

    static PyObject *
_VimChdir(PyObject *_chdir, PyObject *args, PyObject *kwargs)
{
    PyObject	*ret;
    PyObject	*newwd;
    PyObject	*todecref;
    char_u	*new_dir;

    නම් (_chdir == NULL)
	ආපසු NULL;
    නම් (!(ret = PyObject_Call(_chdir, args, kwargs)))
	ආපසු NULL;

    නම් (!(newwd = PyObject_CallFunctionObjArgs(py_getcwd, NULL)))
    {
	Py_DECREF(ret);
	ආපසු NULL;
    }

    නම් (!(new_dir = StringToChars(newwd, &todecref)))
    {
	Py_DECREF(ret);
	Py_DECREF(newwd);
	ආපසු NULL;
    }

    VimTryStart();

    නම් (vim_chdir(new_dir))
    {
	Py_DECREF(ret);
	Py_DECREF(newwd);
	Py_XDECREF(todecref);

	නම් (VimTryEnd())
	    ආපසු NULL;

	PyErr_SET_VIM(N_("failed to change directory"));
	ආපසු NULL;
    }

    Py_DECREF(newwd);
    Py_XDECREF(todecref);

    post_chdir(CDSCOPE_GLOBAL);

    නම් (VimTryEnd())
    {
	Py_DECREF(ret);
	ආපසු NULL;
    }

    ආපසු ret;
}

    static PyObject *
VimChdir(PyObject *self UNUSED, PyObject *args, PyObject *kwargs)
{
    ආපසු _VimChdir(py_chdir, args, kwargs);
}

    static PyObject *
VimFchdir(PyObject *self UNUSED, PyObject *args, PyObject *kwargs)
{
    ආපසු _VimChdir(py_fchdir, args, kwargs);
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

    නම් (!(pathObject = PyString_FromString((var_na *)path)))
    {
	*data = NULL;
	ආපසු;
    }

    mr_data->result = PyObject_CallFunctionObjArgs(mr_data->callable,
						   pathObject, NULL);

    Py_DECREF(pathObject);

    නම් (!mr_data->result || mr_data->result != Py_None)
	*data = NULL;
    නැතහොත්
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

    නම් (data.result == NULL)
    {
	නම් (PyErr_Occurred())
	    ආපසු NULL;
	නැතහොත්
	{
	    Py_INCREF(Py_None);
	    ආපසු Py_None;
	}
    }
    ආපසු data.result;
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

#නම් PY_MAJOR_VERSION < 3
# define PY_MAIN_DIR_STRING "python2"
#නැතහොත්
# define PY_MAIN_DIR_STRING "python3"
#endif
#define PY_ALTERNATE_DIR_STRING "pythonx"

#define PYTHONX_STRING_LENGTH 7 // STRLEN("pythonx")
    නම් (!(pathbuf = PyMem_New(var_na,
		    pathlen + STRLEN(PATHSEPSTR) + PYTHONX_STRING_LENGTH + 1)))
    {
	PyErr_NoMemory();
	*data = NULL;
	ආපසු;
    }

    mch_memmove(pathbuf, path, pathlen + 1);
    add_pathsep((char_u *) pathbuf);

    pathlen = STRLEN(pathbuf);
    mch_memmove(pathbuf + pathlen, PY_MAIN_DIR_STRING,
	    PYTHONX_STRING_LENGTH + 1);

    නම් (!(pathObject1 = PyString_FromString(pathbuf)))
    {
	*data = NULL;
	PyMem_Free(pathbuf);
	ආපසු;
    }

    mch_memmove(pathbuf + pathlen, PY_ALTERNATE_DIR_STRING,
	    PYTHONX_STRING_LENGTH + 1);

    නම් (!(pathObject2 = PyString_FromString(pathbuf)))
    {
	Py_DECREF(pathObject1);
	PyMem_Free(pathbuf);
	*data = NULL;
	ආපසු;
    }

    PyMem_Free(pathbuf);

    නම් (PyList_Append(list, pathObject1)
	    || PyList_Append(list, pathObject2))
	*data = NULL;

    Py_DECREF(pathObject1);
    Py_DECREF(pathObject2);
}

    static PyObject *
Vim_GetPaths(PyObject *self UNUSED, PyObject *args UNUSED)
{
    PyObject	*ret;

    නම් (!(ret = PyList_New(0)))
	ආපසු NULL;

    do_in_runtimepath(NULL, 0, &map_finder_callback, ret);

    නම් (PyErr_Occurred())
    {
	Py_DECREF(ret);
	ආපසු NULL;
    }

    ආපසු ret;
}

#නම් PY_VERSION_HEX >= 0x030700f0
    static PyObject *
FinderFindSpec(PyObject *self, PyObject *args)
{
    var_na	*fullname;
    PyObject	*paths;
    PyObject	*target = Py_None;
    PyObject	*spec;

    නම් (!PyArg_ParseTuple(args, "s|O", &fullname, &target))
	ආපසු NULL;

    නම් (!(paths = Vim_GetPaths(self, NULL)))
	ආපසු NULL;

    spec = PyObject_CallFunction(py_find_spec, "sOO", fullname, paths, target);

    Py_DECREF(paths);

    නම් (!spec)
    {
	නම් (PyErr_Occurred())
	    ආපසු NULL;

	Py_INCREF(Py_None);
	ආපසු Py_None;
    }

    ආපසු spec;
}

    static PyObject *
FinderFindModule(PyObject* self UNUSED, PyObject* args UNUSED)
{
    // Apparently returning None works.
    Py_INCREF(Py_None);
    ආපසු Py_None;
}
#නැතහොත්
    static PyObject *
call_load_module(var_na *name, පූර්ණ_සංඛ්‍යා len, PyObject *find_module_result)
{
    PyObject	*fd, *pathname, *description;

    නම් (!PyTuple_Check(find_module_result))
    {
	PyErr_FORMAT_TYPE(
		N_("expected 3-tuple as imp.find_module() result, but got %s"),
		find_module_result);
	ආපසු NULL;
    }
    නම් (PyTuple_GET_SIZE(find_module_result) != 3)
    {
	PyErr_FORMAT(PyExc_TypeError,
		N_("expected 3-tuple as imp.find_module() result, but got "
		   "tuple of size %d"),
		(පූර්ණ_සංඛ්‍යා) PyTuple_GET_SIZE(find_module_result));
	ආපසු NULL;
    }

    නම් (!(fd = PyTuple_GET_ITEM(find_module_result, 0))
	    || !(pathname = PyTuple_GET_ITEM(find_module_result, 1))
	    || !(description = PyTuple_GET_ITEM(find_module_result, 2)))
    {
	PyErr_SET_STRING(PyExc_RuntimeError,
		N_("internal error: imp.find_module returned tuple with NULL"));
	ආපසු NULL;
    }

    ආපසු PyObject_CallFunction(py_load_module,
	    "s#OOO", name, len, fd, pathname, description);
}

    static PyObject *
find_module(var_na *fullname, var_na *tail, PyObject *new_path)
{
    PyObject	*find_module_result;
    PyObject	*module;
    var_na	*dot;

    නම් ((dot = (var_na *)vim_strchr((char_u *) tail, '.')))
    {
	/*
	 * There is a dot in the name: call find_module recursively without the
	 * first component
	 */
	PyObject	*newest_path;
	පූර්ණ_සංඛ්‍යා		partlen = (පූර්ණ_සංඛ්‍යා) (dot - 1 - tail);

	නම් (!(find_module_result = PyObject_CallFunction(py_find_module,
			"s#O", tail, partlen, new_path)))
	{
	    නම් (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_ImportError))
		PyErr_Clear();
	    ආපසු NULL;
	}

	නම් (!(module = call_load_module(
			fullname,
			((පූර්ණ_සංඛ්‍යා) (tail - fullname)) + partlen,
			find_module_result)))
	{
	    Py_DECREF(find_module_result);
	    ආපසු NULL;
	}

	Py_DECREF(find_module_result);

	නම් (!(newest_path = PyObject_GetAttrString(module, "__path__")))
	{
	    Py_DECREF(module);
	    ආපසු NULL;
	}

	Py_DECREF(module);

	find_module_result = find_module(fullname, dot + 1, newest_path);

	Py_DECREF(newest_path);

	ආපසු find_module_result;
    }
    නැතහොත්
    {
	නම් (!(find_module_result = PyObject_CallFunction(py_find_module,
			"sO", tail, new_path)))
	{
	    නම් (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_ImportError))
		PyErr_Clear();
	    ආපසු NULL;
	}

	ආපසු find_module_result;
    }
}

    static PyObject *
FinderFindModule(PyObject *self, PyObject *args)
{
    var_na	*fullname;
    PyObject	*result;
    PyObject	*new_path;
    LoaderObject	*loader;

    නම් (!PyArg_ParseTuple(args, "s", &fullname))
	ආපසු NULL;

    නම් (!(new_path = Vim_GetPaths(self, NULL)))
	ආපසු NULL;

    result = find_module(fullname, fullname, new_path);

    Py_DECREF(new_path);

    නම් (!result)
    {
	නම් (PyErr_Occurred())
	    ආපසු NULL;

	Py_INCREF(Py_None);
	ආපසු Py_None;
    }

    නම් (!(fullname = (var_na *)vim_strsave((char_u *)fullname)))
    {
	Py_DECREF(result);
	PyErr_NoMemory();
	ආපසු NULL;
    }

    නම් (!(loader = PyObject_NEW(LoaderObject, LoaderTypePtr)))
    {
	vim_free(fullname);
	Py_DECREF(result);
	ආපසු NULL;
    }

    loader->fullname = fullname;
    loader->result = result;

    ආපසු (PyObject *) loader;
}
#endif

    static PyObject *
VimPathHook(PyObject *self UNUSED, PyObject *args)
{
    var_na	*path;

    නම් (PyArg_ParseTuple(args, "s", &path)
	    && STRCMP(path, vim_special_path) == 0)
    {
	Py_INCREF(vim_module);
	ආපසු vim_module;
    }

    PyErr_Clear();
    PyErr_SetNone(PyExc_ImportError);
    ආපසු NULL;
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
#නම් PY_VERSION_HEX >= 0x030700f0
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
typedef පූර්ණ_සංඛ්‍යා (*traversefun)(void *, visitproc, void *);
typedef පූර්ණ_සංඛ්‍යා (*clearfun)(void **);

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

    නම් (iter_object)
	Py_INCREF(iter_object);

    ආපසු (PyObject *)(self);
}

    static void
IterDestructor(PyObject *self_obj)
{
    IterObject *self = (IterObject*)self_obj;
    නම් (self->iter_object)
	Py_DECREF(self->iter_object);
    PyObject_GC_UnTrack((void *)(self));
    self->destruct(self->cur);
    PyObject_GC_Del((void *)(self));
}

    static පූර්ණ_සංඛ්‍යා
IterTraverse(PyObject *self_obj, visitproc visit, void *arg)
{
    IterObject *self = (IterObject*)self_obj;
    නම් (self->traverse != NULL)
	ආපසු self->traverse(self->cur, visit, arg);
    නැතහොත්
	ආපසු 0;
}

// Mac OSX defines clear() somewhere.
#ifdef clear
# undef clear
#endif

    static පූර්ණ_සංඛ්‍යා
IterClear(PyObject *self_obj)
{
    IterObject *self = (IterObject*)self_obj;
    නම් (self->clear != NULL)
	ආපසු self->clear(&self->cur);
    නැතහොත්
	ආපසු 0;
}

    static PyObject *
IterNext(PyObject *self_obj)
{
    IterObject *self = (IterObject*)self_obj;
    ආපසු self->next(&self->cur);
}

    static PyObject *
IterIter(PyObject *self)
{
    Py_INCREF(self);
    ආපසු self;
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
    නම් (ref->pll_prev == NULL)
    {
	නම් (ref->pll_next == NULL)
	{
	    *last = NULL;
	    ආපසු;
	}
    }
    නැතහොත්
	ref->pll_prev->pll_next = ref->pll_next;

    නම් (ref->pll_next == NULL)
	*last = ref->pll_prev;
    නැතහොත්
	ref->pll_next->pll_prev = ref->pll_prev;
}

    static void
pyll_add(PyObject *self, pylinkedlist_T *ref, pylinkedlist_T **last)
{
    නම් (*last == NULL)
	ref->pll_prev = NULL;
    නැතහොත්
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
    නම් (self == NULL)
	ආපසු NULL;
    self->dict = dict;
    ++dict->dv_refcount;

    pyll_add((PyObject *)(self), &self->ref, &lastdict);

    ආපසු (PyObject *)(self);
}

    static dict_T *
py_dict_alloc(void)
{
    dict_T	*ret;

    නම් (!(ret = dict_alloc()))
    {
	PyErr_NoMemory();
	ආපසු NULL;
    }
    ++ret->dv_refcount;

    ආපසු ret;
}

    static PyObject *
DictionaryConstructor(PyTypeObject *subtype, PyObject *args, PyObject *kwargs)
{
    DictionaryObject	*self;
    dict_T	*dict;

    නම් (!(dict = py_dict_alloc()))
	ආපසු NULL;

    self = (DictionaryObject *) DictionaryNew(subtype, dict);

    --dict->dv_refcount;

    නම් (kwargs || PyTuple_Size(args))
    {
	PyObject	*tmp;
	නම් (!(tmp = DictionaryUpdate(self, args, kwargs)))
	{
	    Py_DECREF(self);
	    ආපසු NULL;
	}

	Py_DECREF(tmp);
    }

    ආපසු (PyObject *)(self);
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
    ආපසු ObjectDir(self, DictionaryAttrs);
}

    static පූර්ණ_සංඛ්‍යා
DictionarySetattr(PyObject *self_obj, var_na *name, PyObject *valObject)
{
    DictionaryObject *self = (DictionaryObject*)self_obj;
    නම් (valObject == NULL)
    {
	PyErr_SET_STRING(PyExc_AttributeError,
		N_("cannot delete vim.Dictionary attributes"));
	ආපසු -1;
    }

    නම් (strcmp(name, "locked") == 0)
    {
	නම් (self->dict->dv_lock == VAR_FIXED)
	{
	    PyErr_SET_STRING(PyExc_TypeError,
		    N_("cannot modify fixed dictionary"));
	    ආපසු -1;
	}
	නැතහොත්
	{
	    පූර්ණ_සංඛ්‍යා		istrue = PyObject_IsTrue(valObject);
	    නම් (istrue == -1)
		ආපසු -1;
	    නැතහොත් නම් (istrue)
		self->dict->dv_lock = VAR_LOCKED;
	    නැතහොත්
		self->dict->dv_lock = 0;
	}
	ආපසු 0;
    }
    නැතහොත්
    {
	PyErr_FORMAT(PyExc_AttributeError, N_("cannot set attribute %s"), name);
	ආපසු -1;
    }
}

    static PyInt
DictionaryLength(DictionaryObject *self)
{
    ආපසු ((PyInt) (self->dict->dv_hashtab.ht_used));
}

#define DICT_FLAG_HAS_DEFAULT	0x01
#define DICT_FLAG_POP		0x02
#define DICT_FLAG_NONE_DEFAULT	0x04
#define DICT_FLAG_RETURN_BOOL	0x08 // Incompatible with DICT_FLAG_POP
#define DICT_FLAG_RETURN_PAIR	0x10

    static PyObject *
_DictionaryItem(DictionaryObject *self, PyObject *args, පූර්ණ_සංඛ්‍යා flags)
{
    PyObject	*keyObject;
    PyObject	*defObject = ((flags & DICT_FLAG_NONE_DEFAULT)? Py_None : NULL);
    PyObject	*ret;
    char_u	*key;
    dictitem_T	*di;
    dict_T	*dict = self->dict;
    hashitem_T	*hi;
    PyObject	*todecref;

    නම් (flags & DICT_FLAG_HAS_DEFAULT)
    {
	නම් (!PyArg_ParseTuple(args, "O|O", &keyObject, &defObject))
	    ආපසු NULL;
    }
    නැතහොත්
	keyObject = args;

    නම් (flags & DICT_FLAG_RETURN_BOOL)
	defObject = Py_False;

    නම් (!(key = StringToChars(keyObject, &todecref)))
	ආපසු NULL;

    නම් (*key == NUL)
    {
	RAISE_NO_EMPTY_KEYS;
	Py_XDECREF(todecref);
	ආපසු NULL;
    }

    hi = hash_find(&dict->dv_hashtab, key);

    Py_XDECREF(todecref);

    නම් (HASHITEM_EMPTY(hi))
    {
	නම් (defObject)
	{
	    Py_INCREF(defObject);
	    ආපසු defObject;
	}
	නැතහොත්
	{
	    PyErr_SetObject(PyExc_KeyError, keyObject);
	    ආපසු NULL;
	}
    }
    නැතහොත් නම් (flags & DICT_FLAG_RETURN_BOOL)
    {
	ret = Py_True;
	Py_INCREF(ret);
	ආපසු ret;
    }

    di = dict_lookup(hi);

    නම් (!(ret = ConvertToPyObject(&di->di_tv)))
	ආපසු NULL;

    නම් (flags & DICT_FLAG_POP)
    {
	නම් (dict->dv_lock)
	{
	    RAISE_LOCKED_DICTIONARY;
	    Py_DECREF(ret);
	    ආපසු NULL;
	}

	hash_remove(&dict->dv_hashtab, hi, "Python remove variable");
	dictitem_free(di);
    }

    ආපසු ret;
}

    static PyObject *
DictionaryItem(DictionaryObject *self, PyObject *keyObject)
{
    ආපසු _DictionaryItem(self, keyObject, 0);
}

    static පූර්ණ_සංඛ්‍යා
DictionaryContains(DictionaryObject *self, PyObject *keyObject)
{
    PyObject	*rObj = _DictionaryItem(self, keyObject, DICT_FLAG_RETURN_BOOL);
    පූර්ණ_සංඛ්‍යා		ret;

    නම් (rObj == NULL)
	ආපසු -1;

    ret = (rObj == Py_True);

    Py_DECREF(rObj);

    ආපසු ret;
}

typedef sa_nracanaa
{
    පූර්ණ_සංඛ්‍යා		dii_changed;
    hashtab_T	*dii_ht;
    hashitem_T	*dii_hi;
    long_u	dii_todo;
} dictiterinfo_T;

    static PyObject *
DictionaryIterNext(void **arg)
{
    PyObject	*ret;
    dictiterinfo_T **dii = (dictiterinfo_T**)arg;

    නම් (!(*dii)->dii_todo)
	ආපසු NULL;

    නම් ((*dii)->dii_ht->ht_changed != (*dii)->dii_changed)
    {
	PyErr_SET_STRING(PyExc_RuntimeError,
		N_("hashtab changed during iteration"));
	ආපසු NULL;
    }

    අතරතුර (((*dii)->dii_todo) && HASHITEM_EMPTY((*dii)->dii_hi))
	++((*dii)->dii_hi);

    --((*dii)->dii_todo);

    නම් (!(ret = PyBytes_FromString((var_na *)(*dii)->dii_hi->hi_key)))
	ආපසු NULL;

    ආපසු ret;
}

    static PyObject *
DictionaryIter(PyObject *self_obj)
{
    DictionaryObject	*self = (DictionaryObject*)self_obj;
    dictiterinfo_T	*dii;
    hashtab_T		*ht;

    නම් (!(dii = PyMem_New(dictiterinfo_T, 1)))
    {
	PyErr_NoMemory();
	ආපසු NULL;
    }

    ht = &self->dict->dv_hashtab;
    dii->dii_changed = ht->ht_changed;
    dii->dii_ht = ht;
    dii->dii_hi = ht->ht_array;
    dii->dii_todo = ht->ht_used;

    ආපසු IterNew(dii,
	    PyMem_Free, DictionaryIterNext,
	    NULL, NULL, (PyObject *)self);
}

    static පූර්ණ_සංඛ්‍යා
DictionaryAssItem(
	DictionaryObject *self, PyObject *keyObject, PyObject *valObject)
{
    char_u	*key;
    typval_T	tv;
    dict_T	*dict = self->dict;
    dictitem_T	*di;
    PyObject	*todecref;

    නම් (dict->dv_lock)
    {
	RAISE_LOCKED_DICTIONARY;
	ආපසු -1;
    }

    නම් (!(key = StringToChars(keyObject, &todecref)))
	ආපසු -1;

    නම් (*key == NUL)
    {
	RAISE_NO_EMPTY_KEYS;
	Py_XDECREF(todecref);
	ආපසු -1;
    }

    di = dict_find(dict, key, -1);

    නම් (valObject == NULL)
    {
	hashitem_T	*hi;

	නම් (di == NULL)
	{
	    Py_XDECREF(todecref);
	    PyErr_SetObject(PyExc_KeyError, keyObject);
	    ආපසු -1;
	}
	hi = hash_find(&dict->dv_hashtab, di->di_key);
	hash_remove(&dict->dv_hashtab, hi, "Python remove item");
	dictitem_free(di);
	Py_XDECREF(todecref);
	ආපසු 0;
    }

    නම් (ConvertFromPyObject(valObject, &tv) == -1)
    {
	Py_XDECREF(todecref);
	ආපසු -1;
    }

    නම් (di == NULL)
    {
	නම් (!(di = dictitem_alloc(key)))
	{
	    Py_XDECREF(todecref);
	    PyErr_NoMemory();
	    ආපසු -1;
	}
	di->di_tv.v_type = VAR_UNKNOWN;

	නම් (dict_add(dict, di) == FAIL)
	{
	    dictitem_free(di);
	    RAISE_KEY_ADD_FAIL(key);
	    Py_XDECREF(todecref);
	    ආපසු -1;
	}
    }
    නැතහොත්
	clear_tv(&di->di_tv);

    Py_XDECREF(todecref);

    copy_tv(&tv, &di->di_tv);
    clear_tv(&tv);
    ආපසු 0;
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
    සඳහා (hi = dict->dv_hashtab.ht_array; todo > 0; ++hi)
    {
	නම් (!HASHITEM_EMPTY(hi))
	{
	    නම් (!(newObj = hiconvert(hi)))
	    {
		Py_DECREF(ret);
		ආපසු NULL;
	    }
	    PyList_SET_ITEM(ret, i, newObj);
	    --todo;
	    ++i;
	}
    }
    ආපසු ret;
}

    static PyObject *
dict_key(hashitem_T *hi)
{
    ආපසු PyBytes_FromString((var_na *)(hi->hi_key));
}

    static PyObject *
DictionaryListKeys(DictionaryObject *self, PyObject *args UNUSED)
{
    ආපසු DictionaryListObjects(self, dict_key);
}

    static PyObject *
dict_val(hashitem_T *hi)
{
    dictitem_T	*di;

    di = dict_lookup(hi);
    ආපසු ConvertToPyObject(&di->di_tv);
}

    static PyObject *
DictionaryListValues(DictionaryObject *self, PyObject *args UNUSED)
{
    ආපසු DictionaryListObjects(self, dict_val);
}

    static PyObject *
dict_item(hashitem_T *hi)
{
    PyObject	*keyObject;
    PyObject	*valObject;
    PyObject	*ret;

    නම් (!(keyObject = dict_key(hi)))
	ආපසු NULL;

    නම් (!(valObject = dict_val(hi)))
    {
	Py_DECREF(keyObject);
	ආපසු NULL;
    }

    ret = Py_BuildValue("(OO)", keyObject, valObject);

    Py_DECREF(keyObject);
    Py_DECREF(valObject);

    ආපසු ret;
}

    static PyObject *
DictionaryListItems(DictionaryObject *self, PyObject *args UNUSED)
{
    ආපසු DictionaryListObjects(self, dict_item);
}

    static PyObject *
DictionaryUpdate(DictionaryObject *self, PyObject *args, PyObject *kwargs)
{
    dict_T	*dict = self->dict;

    නම් (dict->dv_lock)
    {
	RAISE_LOCKED_DICTIONARY;
	ආපසු NULL;
    }

    නම් (kwargs)
    {
	typval_T	tv;

	නම් (ConvertFromPyMapping(kwargs, &tv) == -1)
	    ආපසු NULL;

	VimTryStart();
	dict_extend(self->dict, tv.vval.v_dict, (char_u *) "force", NULL);
	clear_tv(&tv);
	නම් (VimTryEnd())
	    ආපසු NULL;
    }
    නැතහොත්
    {
	PyObject	*obj = NULL;

	නම් (!PyArg_ParseTuple(args, "|O", &obj))
	    ආපසු NULL;

	නම් (obj == NULL)
	{
	    Py_INCREF(Py_None);
	    ආපසු Py_None;
	}

	නම් (PyObject_HasAttrString(obj, "keys"))
	    ආපසු DictionaryUpdate(self, NULL, obj);
	නැතහොත්
	{
	    PyObject	*iterator;
	    PyObject	*item;

	    නම් (!(iterator = PyObject_GetIter(obj)))
		ආපසු NULL;

	    අතරතුර ((item = PyIter_Next(iterator)))
	    {
		PyObject	*fast;
		PyObject	*keyObject;
		PyObject	*valObject;
		PyObject	*todecref;
		char_u		*key;
		dictitem_T	*di;
		hashitem_T	*hi;

		නම් (!(fast = PySequence_Fast(item, "")))
		{
		    Py_DECREF(iterator);
		    Py_DECREF(item);
		    ආපසු NULL;
		}

		Py_DECREF(item);

		නම් (PySequence_Size(fast) != 2)
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    PyErr_FORMAT(PyExc_ValueError,
			    N_("expected sequence element of size 2, "
			    "but got sequence of size %d"),
			    (පූර්ණ_සංඛ්‍යා) PySequence_Size(fast));
		    ආපසු NULL;
		}

		keyObject = PySequence_GetItem(fast, 0);

		නම් (!(key = StringToChars(keyObject, &todecref)))
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    ආපසු NULL;
		}

		di = dictitem_alloc(key);

		Py_XDECREF(todecref);

		නම් (di == NULL)
		{
		    Py_DECREF(fast);
		    Py_DECREF(iterator);
		    PyErr_NoMemory();
		    ආපසු NULL;
		}
		di->di_tv.v_type = VAR_UNKNOWN;

		valObject = PySequence_GetItem(fast, 1);

		නම් (ConvertFromPyObject(valObject, &di->di_tv) == -1)
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    dictitem_free(di);
		    ආපසු NULL;
		}

		Py_DECREF(fast);

		hi = hash_find(&dict->dv_hashtab, di->di_key);
		නම් (!HASHITEM_EMPTY(hi) || dict_add(dict, di) == FAIL)
		{
		    RAISE_KEY_ADD_FAIL(di->di_key);
		    Py_DECREF(iterator);
		    dictitem_free(di);
		    ආපසු NULL;
		}
	    }

	    Py_DECREF(iterator);

	    // Iterator may have finished due to an exception
	    නම් (PyErr_Occurred())
		ආපසු NULL;
	}
    }
    Py_INCREF(Py_None);
    ආපසු Py_None;
}

    static PyObject *
DictionaryGet(DictionaryObject *self, PyObject *args)
{
    ආපසු _DictionaryItem(self, args,
			    DICT_FLAG_HAS_DEFAULT|DICT_FLAG_NONE_DEFAULT);
}

    static PyObject *
DictionaryPop(DictionaryObject *self, PyObject *args)
{
    ආපසු _DictionaryItem(self, args, DICT_FLAG_HAS_DEFAULT|DICT_FLAG_POP);
}

    static PyObject *
DictionaryPopItem(DictionaryObject *self, PyObject *args UNUSED)
{
    hashitem_T	*hi;
    PyObject	*ret;
    PyObject	*valObject;
    dictitem_T	*di;

    නම් (self->dict->dv_hashtab.ht_used == 0)
    {
	PyErr_SetNone(PyExc_KeyError);
	ආපසු NULL;
    }

    hi = self->dict->dv_hashtab.ht_array;
    අතරතුර (HASHITEM_EMPTY(hi))
	++hi;

    di = dict_lookup(hi);

    නම් (!(valObject = ConvertToPyObject(&di->di_tv)))
	ආපසු NULL;

    නම් (!(ret = Py_BuildValue("(" Py_bytes_fmt "O)", hi->hi_key, valObject)))
    {
	Py_DECREF(valObject);
	ආපසු NULL;
    }

    hash_remove(&self->dict->dv_hashtab, hi, "Python pop item");
    dictitem_free(di);

    ආපසු ret;
}

    static PyObject *
DictionaryHasKey(DictionaryObject *self, PyObject *keyObject)
{
    ආපසු _DictionaryItem(self, keyObject, DICT_FLAG_RETURN_BOOL);
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

    නම් (list == NULL)
	ආපසු NULL;

    self = (ListObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    නම් (self == NULL)
	ආපසු NULL;
    self->list = list;
    ++list->lv_refcount;
    CHECK_LIST_MATERIALIZE(list);

    pyll_add((PyObject *)(self), &self->ref, &lastlist);

    ආපසු (PyObject *)(self);
}

    static list_T *
py_list_alloc(void)
{
    list_T	*ret;

    නම් (!(ret = list_alloc()))
    {
	PyErr_NoMemory();
	ආපසු NULL;
    }
    ++ret->lv_refcount;

    ආපසු ret;
}

    static පූර්ණ_සංඛ්‍යා
list_py_concat(list_T *l, PyObject *obj, PyObject *lookup_dict)
{
    PyObject	*iterator;
    PyObject	*item;
    listitem_T	*li;

    නම් (!(iterator = PyObject_GetIter(obj)))
	ආපසු -1;

    අතරතුර ((item = PyIter_Next(iterator)))
    {
	නම් (!(li = listitem_alloc()))
	{
	    PyErr_NoMemory();
	    Py_DECREF(item);
	    Py_DECREF(iterator);
	    ආපසු -1;
	}
	li->li_tv.v_lock = 0;
	li->li_tv.v_type = VAR_UNKNOWN;

	නම් (_ConvertFromPyObject(item, &li->li_tv, lookup_dict) == -1)
	{
	    Py_DECREF(item);
	    Py_DECREF(iterator);
	    listitem_free(l, li);
	    ආපසු -1;
	}

	Py_DECREF(item);

	list_append(l, li);
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    නම් (PyErr_Occurred())
	ආපසු -1;

    ආපසු 0;
}

    static PyObject *
ListConstructor(PyTypeObject *subtype, PyObject *args, PyObject *kwargs)
{
    list_T	*list;
    PyObject	*obj = NULL;

    නම් (kwargs)
    {
	PyErr_SET_STRING(PyExc_TypeError,
		N_("list constructor does not accept keyword arguments"));
	ආපසු NULL;
    }

    නම් (!PyArg_ParseTuple(args, "|O", &obj))
	ආපසු NULL;

    නම් (!(list = py_list_alloc()))
	ආපසු NULL;

    නම් (obj)
    {
	PyObject	*lookup_dict;

	නම් (!(lookup_dict = PyDict_New()))
	{
	    list_unref(list);
	    ආපසු NULL;
	}

	නම් (list_py_concat(list, obj, lookup_dict) == -1)
	{
	    Py_DECREF(lookup_dict);
	    list_unref(list);
	    ආපසු NULL;
	}

	Py_DECREF(lookup_dict);
    }

    ආපසු ListNew(subtype, list);
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
    ආපසු ((PyInt) (self->list->lv_len));
}

    static PyObject *
ListIndex(ListObject *self, Py_ssize_t index)
{
    listitem_T	*li;

    නම් (index >= ListLength(self))
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("list index out of range"));
	ආපසු NULL;
    }
    li = list_find(self->list, (long) index);
    නම් (li == NULL)
    {
	// No more suitable format specifications in python-2.3
	PyErr_VIM_FORMAT(N_("internal error: failed to get Vim list item %d"),
		(පූර්ණ_සංඛ්‍යා) index);
	ආපසු NULL;
    }
    ආපසු ConvertToPyObject(&li->li_tv);
}

    static PyObject *
ListSlice(ListObject *self, Py_ssize_t first, Py_ssize_t step,
	  Py_ssize_t slicelen)
{
    PyInt	i;
    PyObject	*list;

    නම් (step == 0)
    {
	PyErr_SET_STRING(PyExc_ValueError, N_("slice step cannot be zero"));
	ආපසු NULL;
    }

    list = PyList_New(slicelen);
    නම් (list == NULL)
	ආපසු NULL;

    සඳහා (i = 0; i < slicelen; ++i)
    {
	PyObject	*item;

	item = ListIndex(self, first + i*step);
	නම් (item == NULL)
	{
	    Py_DECREF(list);
	    ආපසු NULL;
	}

	PyList_SET_ITEM(list, i, item);
    }

    ආපසු list;
}

    static PyObject *
ListItem(ListObject *self, PyObject* idx)
{
#නම් PY_MAJOR_VERSION < 3
    නම් (PyInt_Check(idx))
    {
	long _idx = PyInt_AsLong(idx);
	ආපසු ListIndex(self, _idx);
    }
    නැතහොත්
#endif
    නම් (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	ආපසු ListIndex(self, _idx);
    }
    නැතහොත් නම් (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	නම් (PySlice_GetIndicesEx((PySliceObject_T *)idx, ListLength(self),
				 &start, &stop, &step, &slicelen) < 0)
	    ආපසු NULL;
	ආපසු ListSlice(self, start, step, slicelen);
    }
    නැතහොත්
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	ආපසු NULL;
    }
}

    static void
list_restore(Py_ssize_t numadded, Py_ssize_t numreplaced, Py_ssize_t slicelen,
	list_T *l, listitem_T **lis, listitem_T *lastaddedli)
{
    අතරතුර (numreplaced--)
    {
	list_insert(l, lis[numreplaced], lis[slicelen + numreplaced]);
	listitem_remove(l, lis[slicelen + numreplaced]);
    }
    අතරතුර (numadded--)
    {
	listitem_T	*next;

	next = lastaddedli->li_prev;
	listitem_remove(l, lastaddedli);
	lastaddedli = next;
    }
}

    static පූර්ණ_සංඛ්‍යා
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

    නම් (l->lv_lock)
    {
	RAISE_LOCKED_LIST;
	ආපසු -1;
    }

    නම් (step == 0)
    {
	PyErr_SET_STRING(PyExc_ValueError, N_("slice step cannot be zero"));
	ආපසු -1;
    }

    නම් (step != 1 && slicelen == 0)
    {
	// Nothing to do. Only error out if obj has some items.
	පූර්ණ_සංඛ්‍යා		ret = 0;

	නම් (obj == NULL)
	    ආපසු 0;

	නම් (!(iterator = PyObject_GetIter(obj)))
	    ආපසු -1;

	නම් ((item = PyIter_Next(iterator)))
	{
	    PyErr_FORMAT(PyExc_ValueError,
		    N_("attempt to assign sequence of size greater than %d "
			"to extended slice"), 0);
	    Py_DECREF(item);
	    ret = -1;
	}
	Py_DECREF(iterator);
	ආපසු ret;
    }

    නම් (obj != NULL)
	// XXX May allocate zero bytes.
	නම් (!(lis = PyMem_New(listitem_T *, slicelen * 2)))
	{
	    PyErr_NoMemory();
	    ආපසු -1;
	}

    නම් (first == size)
	li = NULL;
    නැතහොත්
    {
	li = list_find(l, (long) first);
	නම් (li == NULL)
	{
	    PyErr_VIM_FORMAT(N_("internal error: no Vim list item %d"),
		    (පූර්ණ_සංඛ්‍යා)first);
	    නම් (obj != NULL)
		PyMem_Free(lis);
	    ආපසු -1;
	}
	i = slicelen;
	අතරතුර (i-- && li != NULL)
	{
	    j = step;
	    next = li;
	    නම් (step > 0)
		අතරතුර (next != NULL && ((next = next->li_next) != NULL) && --j);
	    නැතහොත්
		අතරතුර (next != NULL && ((next = next->li_prev) != NULL) && ++j);

	    නම් (obj == NULL)
		listitem_remove(l, li);
	    නැතහොත්
		lis[slicelen - i - 1] = li;

	    li = next;
	}
	නම් (li == NULL && i != -1)
	{
	    PyErr_SET_VIM(N_("internal error: not enough list items"));
	    නම් (obj != NULL)
		PyMem_Free(lis);
	    ආපසු -1;
	}
    }

    නම් (obj == NULL)
	ආපසු 0;

    නම් (!(iterator = PyObject_GetIter(obj)))
    {
	PyMem_Free(lis);
	ආපසු -1;
    }

    i = 0;
    අතරතුර ((item = PyIter_Next(iterator)))
    {
	නම් (ConvertFromPyObject(item, &v) == -1)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(item);
	    PyMem_Free(lis);
	    ආපසු -1;
	}
	Py_DECREF(item);
	නම් (list_insert_tv(l, &v, numreplaced < slicelen
				    ? lis[numreplaced]
				    : li) == FAIL)
	{
	    clear_tv(&v);
	    PyErr_SET_VIM(N_("internal error: failed to add item to list"));
	    list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	    PyMem_Free(lis);
	    ආපසු -1;
	}
	නම් (numreplaced < slicelen)
	{
	    lis[slicelen + numreplaced] = lis[numreplaced]->li_prev;
	    vimlist_remove(l, lis[numreplaced], lis[numreplaced]);
	    numreplaced++;
	}
	නැතහොත්
	{
	    නම් (li)
		lastaddedli = li->li_prev;
	    නැතහොත්
		lastaddedli = l->lv_u.mat.lv_last;
	    numadded++;
	}
	clear_tv(&v);
	නම් (step != 1 && i >= slicelen)
	{
	    Py_DECREF(iterator);
	    PyErr_FORMAT(PyExc_ValueError,
		    N_("attempt to assign sequence of size greater than %d "
			"to extended slice"), (පූර්ණ_සංඛ්‍යා) slicelen);
	    list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	    PyMem_Free(lis);
	    ආපසු -1;
	}
	++i;
    }
    Py_DECREF(iterator);

    නම් (step != 1 && i != slicelen)
    {
	PyErr_FORMAT2(PyExc_ValueError,
		N_("attempt to assign sequence of size %d to extended slice "
		    "of size %d"), (පූර්ණ_සංඛ්‍යා) i, (පූර්ණ_සංඛ්‍යා) slicelen);
	list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	PyMem_Free(lis);
	ආපසු -1;
    }

    නම් (PyErr_Occurred())
    {
	list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	PyMem_Free(lis);
	ආපසු -1;
    }

    සඳහා (i = 0; i < numreplaced; i++)
	listitem_free(l, lis[i]);
    නම් (step == 1)
	සඳහා (i = numreplaced; i < slicelen; i++)
	    listitem_remove(l, lis[i]);

    PyMem_Free(lis);

    ආපසු 0;
}

    static පූර්ණ_සංඛ්‍යා
ListAssIndex(ListObject *self, Py_ssize_t index, PyObject *obj)
{
    typval_T	tv;
    list_T	*l = self->list;
    listitem_T	*li;
    Py_ssize_t	length = ListLength(self);

    නම් (l->lv_lock)
    {
	RAISE_LOCKED_LIST;
	ආපසු -1;
    }
    නම් (index > length || (index == length && obj == NULL))
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("list index out of range"));
	ආපසු -1;
    }

    නම් (obj == NULL)
    {
	li = list_find(l, (long) index);
	නම් (li == NULL)
	{
	    PyErr_VIM_FORMAT(N_("internal error: failed to get Vim "
			"list item %d"), (පූර්ණ_සංඛ්‍යා) index);
	    ආපසු -1;
	}
	vimlist_remove(l, li, li);
	clear_tv(&li->li_tv);
	vim_free(li);
	ආපසු 0;
    }

    නම් (ConvertFromPyObject(obj, &tv) == -1)
	ආපසු -1;

    නම් (index == length)
    {
	නම් (list_append_tv(l, &tv) == FAIL)
	{
	    clear_tv(&tv);
	    PyErr_SET_VIM(N_("failed to add item to list"));
	    ආපසු -1;
	}
    }
    නැතහොත්
    {
	li = list_find(l, (long) index);
	නම් (li == NULL)
	{
	    PyErr_VIM_FORMAT(N_("internal error: failed to get Vim "
			"list item %d"), (පූර්ණ_සංඛ්‍යා) index);
	    ආපසු -1;
	}
	clear_tv(&li->li_tv);
	copy_tv(&tv, &li->li_tv);
	clear_tv(&tv);
    }
    ආපසු 0;
}

    static පූර්ණ_සංඛ්‍යා
ListAssItem(ListObject *self, PyObject *idx, PyObject *obj)
{
#නම් PY_MAJOR_VERSION < 3
    නම් (PyInt_Check(idx))
    {
	long _idx = PyInt_AsLong(idx);
	ආපසු (පූර්ණ_සංඛ්‍යා)ListAssIndex(self, _idx, obj);
    }
    නැතහොත්
#endif
    නම් (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	ආපසු (පූර්ණ_සංඛ්‍යා)ListAssIndex(self, _idx, obj);
    }
    නැතහොත් නම් (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	නම් (PySlice_GetIndicesEx((PySliceObject_T *)idx, ListLength(self),
				 &start, &stop, &step, &slicelen) < 0)
	    ආපසු -1;
	ආපසු (පූර්ණ_සංඛ්‍යා)ListAssSlice(self, start, step, slicelen,
		obj);
    }
    නැතහොත්
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	ආපසු -1;
    }
}

    static PyObject *
ListConcatInPlace(ListObject *self, PyObject *obj)
{
    list_T	*l = self->list;
    PyObject	*lookup_dict;

    නම් (l->lv_lock)
    {
	RAISE_LOCKED_LIST;
	ආපසු NULL;
    }

    නම් (!(lookup_dict = PyDict_New()))
	ආපසු NULL;

    නම් (list_py_concat(l, obj, lookup_dict) == -1)
    {
	Py_DECREF(lookup_dict);
	ආපසු NULL;
    }
    Py_DECREF(lookup_dict);

    Py_INCREF((PyObject *)self);
    ආපසු (PyObject *)(self);
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

    නම් (!((*lii)->lw.lw_item))
	ආපසු NULL;

    නම් (!(ret = ConvertToPyObject(&((*lii)->lw.lw_item->li_tv))))
	ආපසු NULL;

    (*lii)->lw.lw_item = (*lii)->lw.lw_item->li_next;

    ආපසු ret;
}

    static PyObject *
ListIter(PyObject *self_obj)
{
    ListObject	*self = (ListObject*)self_obj;
    listiterinfo_T	*lii;
    list_T	*l = self->list;

    නම් (!(lii = PyMem_New(listiterinfo_T, 1)))
    {
	PyErr_NoMemory();
	ආපසු NULL;
    }

    CHECK_LIST_MATERIALIZE(l);
    list_add_watch(l, &lii->lw);
    lii->lw.lw_item = l->lv_first;
    lii->list = l;
    ++l->lv_refcount;

    ආපසු IterNew(lii,
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
    ආපසු ObjectDir(self, ListAttrs);
}

    static පූර්ණ_සංඛ්‍යා
ListSetattr(PyObject *self_obj, var_na *name, PyObject *valObject)
{
    ListObject *self = (ListObject*)self_obj;
    නම් (valObject == NULL)
    {
	PyErr_SET_STRING(PyExc_AttributeError,
		N_("cannot delete vim.List attributes"));
	ආපසු -1;
    }

    නම් (strcmp(name, "locked") == 0)
    {
	නම් (self->list->lv_lock == VAR_FIXED)
	{
	    PyErr_SET_STRING(PyExc_TypeError, N_("cannot modify fixed list"));
	    ආපසු -1;
	}
	නැතහොත්
	{
	    පූර්ණ_සංඛ්‍යා		istrue = PyObject_IsTrue(valObject);
	    නම් (istrue == -1)
		ආපසු -1;
	    නැතහොත් නම් (istrue)
		self->list->lv_lock = VAR_LOCKED;
	    නැතහොත්
		self->list->lv_lock = 0;
	}
	ආපසු 0;
    }
    නැතහොත්
    {
	PyErr_FORMAT(PyExc_AttributeError, N_("cannot set attribute %s"), name);
	ආපසු -1;
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

    නම් (tuple == NULL)
	ආපසු NULL;

    self = (TupleObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    නම් (self == NULL)
	ආපසු NULL;
    self->tuple = tuple;
    ++tuple->tv_refcount;

    pyll_add((PyObject *)(self), &self->ref, &lasttuple);

    ආපසු (PyObject *)(self);
}

    static tuple_T *
py_tuple_alloc(void)
{
    tuple_T	*ret;

    නම් (!(ret = tuple_alloc()))
    {
	PyErr_NoMemory();
	ආපසු NULL;
    }
    ++ret->tv_refcount;

    ආපසු ret;
}

    static පූර්ණ_සංඛ්‍යා
tuple_py_concat(tuple_T *t, PyObject *obj, PyObject *lookup_dict)
{
    PyObject	*iterator;
    PyObject	*item;

    නම් (!(iterator = PyObject_GetIter(obj)))
	ආපසු -1;

    අතරතුර ((item = PyIter_Next(iterator)))
    {
	typval_T    new_tv;

	නම් (_ConvertFromPyObject(item, &new_tv, lookup_dict) == -1)
	{
	    Py_DECREF(item);
	    Py_DECREF(iterator);
	    ආපසු -1;
	}

	Py_DECREF(item);

	නම් (tuple_append_tv(t, &new_tv) == FAIL)
	{
	    Py_DECREF(iterator);
	    ආපසු -1;
	}
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    නම් (PyErr_Occurred())
	ආපසු -1;

    ආපසු 0;
}

    static PyObject *
TupleConstructor(PyTypeObject *subtype, PyObject *args, PyObject *kwargs)
{
    tuple_T	*tuple;
    PyObject	*obj = NULL;

    නම් (kwargs)
    {
	PyErr_SET_STRING(PyExc_TypeError,
		N_("tuple constructor does not accept keyword arguments"));
	ආපසු NULL;
    }

    නම් (!PyArg_ParseTuple(args, "|O", &obj))
	ආපසු NULL;

    නම් (!(tuple = py_tuple_alloc()))
	ආපසු NULL;

    නම් (obj)
    {
	PyObject	*lookup_dict;

	නම් (!(lookup_dict = PyDict_New()))
	{
	    tuple_unref(tuple);
	    ආපසු NULL;
	}

	නම් (tuple_py_concat(tuple, obj, lookup_dict) == -1)
	{
	    Py_DECREF(lookup_dict);
	    tuple_unref(tuple);
	    ආපසු NULL;
	}

	Py_DECREF(lookup_dict);
    }

    ආපසු TupleNew(subtype, tuple);
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
    ආපසු ((PyInt)(tuple_len(self->tuple)));
}

    static PyObject *
TupleIndex(TupleObject *self, Py_ssize_t index)
{
    PyInt	len = TupleLength(self);

    නම් (index < 0)
	index = len + index;

    නම් (index < 0 || index >= len)
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("tuple index out of range"));
	ආපසු NULL;
    }
    ආපසු ConvertToPyObject(TUPLE_ITEM(self->tuple, index));
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
    නම් (tuple == NULL)
	ආපසු NULL;

    සඳහා (i = 0; i < slicelen; ++i)
    {
	PyObject	*item;

	item = TupleIndex(self, first + i * step);
	නම් (item == NULL)
	{
	    Py_DECREF(tuple);
	    ආපසු NULL;
	}

	PyTuple_SetItem(tuple, i, item);
    }

    ආපසු tuple;
}

    static PyObject *
TupleItem(TupleObject *self, PyObject* idx)
{
#නම් PY_MAJOR_VERSION < 3
    නම් (PyInt_Check(idx))
    {
	long _idx = PyInt_AsLong(idx);
	ආපසු TupleIndex(self, _idx);
    }
    නැතහොත්
#endif
    නම් (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	ආපසු TupleIndex(self, _idx);
    }
    නැතහොත් නම් (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	නම් (PySlice_GetIndicesEx((PySliceObject_T *)idx, TupleLength(self),
				 &start, &stop, &step, &slicelen) < 0)
	    ආපසු NULL;
	ආපසු TupleSlice(self, start, step, slicelen);
    }
    නැතහොත්
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	ආපසු NULL;
    }
}

typedef sa_nracanaa
{
    tuple_T	*tuple;
    පූර්ණ_සංඛ්‍යා		index;
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

    නම් ((*tii)->index >= TUPLE_LEN((*tii)->tuple))
	ආපසු NULL;

    නම් (!(ret = ConvertToPyObject(TUPLE_ITEM((*tii)->tuple, (*tii)->index))))
	ආපසු NULL;

    (*tii)->index++;

    ආපසු ret;
}

    static PyObject *
TupleIter(PyObject *self_obj)
{
    TupleObject	*self = (TupleObject*)self_obj;
    tupleiterinfo_T	*tii;
    tuple_T	*t = self->tuple;

    නම් (!(tii = PyMem_New(tupleiterinfo_T, 1)))
    {
	PyErr_NoMemory();
	ආපසු NULL;
    }

    tii->tuple = t;
    tii->index = 0;
    ++t->tv_refcount;

    ආපසු IterNew(tii,
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
    ආපසු ObjectDir(self, TupleAttrs);
}

    static පූර්ණ_සංඛ්‍යා
TupleSetattr(PyObject *self_obj, var_na *name, PyObject *valObject)
{
    TupleObject *self = (TupleObject*)self_obj;
    නම් (valObject == NULL)
    {
	PyErr_SET_STRING(PyExc_AttributeError,
		N_("cannot delete vim.Tuple attributes"));
	ආපසු -1;
    }

    නම් (strcmp(name, "locked") == 0)
    {
	නම් (self->tuple->tv_lock == VAR_FIXED)
	{
	    PyErr_SET_STRING(PyExc_TypeError, N_("cannot modify fixed tuple"));
	    ආපසු -1;
	}
	නැතහොත්
	{
	    පූර්ණ_සංඛ්‍යා		istrue = PyObject_IsTrue(valObject);
	    නම් (istrue == -1)
		ආපසු -1;
	    නැතහොත් නම් (istrue)
		self->tuple->tv_lock = VAR_LOCKED;
	    නැතහොත්
		self->tuple->tv_lock = 0;
	}
	ආපසු 0;
    }
    නැතහොත්
    {
	PyErr_FORMAT(PyExc_AttributeError, N_("cannot set attribute %s"), name);
	ආපසු -1;
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
    පූර්ණ_සංඛ්‍යා		argc;
    typval_T	*argv;
    dict_T	*self;
    pylinkedlist_T	ref;
    පූර්ණ_සංඛ්‍යා		auto_rebind;
} FunctionObject;

DEFINE_PY_TYPE_OBJECT(FunctionType);

#define NEW_FUNCTION(name, argc, argv, self, pt_auto) \
    FunctionNew(FunctionTypePtr, (name), (argc), (argv), (self), (pt_auto))

    static PyObject *
FunctionNew(PyTypeObject *subtype, char_u *name, පූර්ණ_සංඛ්‍යා argc, typval_T *argv,
	dict_T *selfdict, පූර්ණ_සංඛ්‍යා auto_rebind)
{
    FunctionObject	*self;

    self = (FunctionObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    නම් (self == NULL)
	ආපසු NULL;

    නම් (isdigit((unsigned var_na)*name))
    {
	නම් (!translated_function_exists(name, FALSE))
	{
	    PyErr_FORMAT(PyExc_ValueError,
		    N_("unnamed function %s does not exist"), name);
	    ආපසු NULL;
	}
	self->name = vim_strsave(name);
    }
    නැතහොත්
    {
	char_u *p;

	නම් ((p = get_expanded_name(name,
			    vim_strchr(name, AUTOLOAD_CHAR) == NULL)) == NULL)
	{
	    PyErr_FORMAT(PyExc_ValueError,
		    N_("function %s does not exist"), name);
	    ආපසු NULL;
	}

	නම් (p[0] == K_SPECIAL && p[1] == KS_EXTRA && p[2] == (පූර්ණ_සංඛ්‍යා)KE_SNR)
	{
	    char_u *np;
	    size_t len = STRLEN(p) + 1;

	    නම් ((np = alloc(len + 2)) == NULL)
	    {
		vim_free(p);
		ආපසු NULL;
	    }
	    mch_memmove(np, "<SNR>", 5);
	    mch_memmove(np + 5, p + 3, len - 3);
	    vim_free(p);
	    self->name = np;
	}
	නැතහොත්
	    self->name = p;
    }

    func_ref(self->name);
    self->argc = argc;
    self->argv = argv;
    self->self = selfdict;
    self->auto_rebind = selfdict == NULL ? TRUE : auto_rebind;

    නම් (self->argv || self->self)
	pyll_add((PyObject *)(self), &self->ref, &lastfunc);

    ආපසු (PyObject *)(self);
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
    පූර්ණ_සංඛ්‍යා		argc = 0;
    පූර්ණ_සංඛ්‍යා		auto_rebind = TRUE;
    typval_T	*argv = NULL;
    typval_T	*curtv;
    listitem_T	*li;

    නම් (kwargs != NULL)
    {
	selfdictObject = PyDict_GetItemString(kwargs, "self");
	නම් (selfdictObject != NULL)
	{
	    නම් (ConvertFromPyMapping(selfdictObject, &selfdicttv) == -1)
		ආපසු NULL;
	    selfdict = selfdicttv.vval.v_dict;
	}
	argsObject = PyDict_GetItemString(kwargs, "args");
	නම් (argsObject != NULL)
	{
	    නම් (ConvertFromPySequence(argsObject, &argstv) == -1)
	    {
		dict_unref(selfdict);
		ආපසු NULL;
	    }
	    argslist = argstv.vval.v_list;
	    CHECK_LIST_MATERIALIZE(argslist);

	    argc = argslist->lv_len;
	    නම් (argc != 0)
	    {
		argv = PyMem_New(typval_T, (size_t) argc);
		නම් (argv == NULL)
		{
		    PyErr_NoMemory();
		    dict_unref(selfdict);
		    list_unref(argslist);
		    ආපසු NULL;
		}
		curtv = argv;
		FOR_ALL_LIST_ITEMS(argslist, li)
		    copy_tv(&li->li_tv, curtv++);
	    }
	    list_unref(argslist);
	}
	නම් (selfdict != NULL)
	{
	    auto_rebind = FALSE;
	    autoRebindObject = PyDict_GetItemString(kwargs, "auto_rebind");
	    නම් (autoRebindObject != NULL)
	    {
		auto_rebind = PyObject_IsTrue(autoRebindObject);
		නම් (auto_rebind == -1)
		{
		    dict_unref(selfdict);
		    list_unref(argslist);
		    ආපසු NULL;
		}
	    }
	}
    }

    නම් (!PyArg_ParseTuple(args, "et", "ascii", &name))
    {
	dict_unref(selfdict);
	අතරතුර (argc--)
	    clear_tv(&argv[argc]);
	PyMem_Free(argv);
	ආපසු NULL;
    }

    self = FunctionNew(subtype, name, argc, argv, selfdict, auto_rebind);

    PyMem_Free(name);

    ආපසු self;
}

    static void
FunctionDestructor(PyObject *self_obj)
{
    FunctionObject *self = (FunctionObject*)self_obj;
    පූර්ණ_සංඛ්‍යා i;
    func_unref(self->name);
    vim_free(self->name);
    සඳහා (i = 0; i < self->argc; ++i)
	clear_tv(&self->argv[i]);
    PyMem_Free(self->argv);
    dict_unref(self->self);
    නම් (self->argv || self->self)
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
    ආපසු ObjectDir(self, FunctionAttrs);
}

    static PyObject *
FunctionAttr(FunctionObject *self, var_na *name)
{
    list_T *list;
    පූර්ණ_සංඛ්‍යා i;
    නම් (strcmp(name, "name") == 0)
	ආපසු PyString_FromString((var_na *)(self->name));
    නැතහොත් නම් (strcmp(name, "args") == 0)
    {
	නම් (self->argv == NULL || (list = list_alloc()) == NULL)
	    ආපසු ALWAYS_NONE;

	සඳහා (i = 0; i < self->argc; ++i)
	    list_append_tv(list, &self->argv[i]);
	ආපසු NEW_LIST(list);
    }
    නැතහොත් නම් (strcmp(name, "self") == 0)
	ආපසු self->self == NULL
	    ? ALWAYS_NONE
	    : NEW_DICTIONARY(self->self);
    නැතහොත් නම් (strcmp(name, "auto_rebind") == 0)
	ආපසු self->auto_rebind
	    ? ALWAYS_TRUE
	    : ALWAYS_FALSE;
    නැතහොත් නම් (strcmp(name, "__members__") == 0)
	ආපසු ObjectDir(NULL, FunctionAttrs);
    ආපසු NULL;
}

/*
 * Populate partial_T given function object.
 *
 * "exported" should be set to true when it is needed to construct a partial
 * that may be stored in a variable (i.e. may be freed by Vim).
 */
    static void
set_partial(FunctionObject *self, partial_T *pt, පූර්ණ_සංඛ්‍යා exported)
{
    පූර්ණ_සංඛ්‍යා i;

    pt->pt_name = self->name;
    නම් (self->argv)
    {
	pt->pt_argc = self->argc;
	නම් (exported)
	{
	    pt->pt_argv = ALLOC_CLEAR_MULT(typval_T, self->argc);
	    සඳහා (i = 0; i < pt->pt_argc; ++i)
		copy_tv(&self->argv[i], &pt->pt_argv[i]);
	}
	නැතහොත්
	    pt->pt_argv = self->argv;
    }
    නැතහොත්
    {
	pt->pt_argc = 0;
	pt->pt_argv = NULL;
    }
    pt->pt_auto = self->auto_rebind || !exported;
    pt->pt_dict = self->self;
    නම් (exported && self->self)
	++pt->pt_dict->dv_refcount;
    නම් (exported)
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
    පූර්ණ_සංඛ්‍යා		error;
    partial_T	pt;
    partial_T	*pt_ptr = NULL;

    නම් (ConvertFromPySequence(argsObject, &args) == -1)
	ආපසු NULL;

    නම් (kwargs != NULL)
    {
	selfdictObject = PyDict_GetItemString(kwargs, "self");
	නම් (selfdictObject != NULL)
	{
	    නම් (ConvertFromPyMapping(selfdictObject, &selfdicttv) == -1)
	    {
		clear_tv(&args);
		ආපසු NULL;
	    }
	    selfdict = selfdicttv.vval.v_dict;
	}
    }

    නම් (self->argv || self->self)
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

    නම් (VimTryEnd())
	ret = NULL;
    නැතහොත් නම් (error != OK)
    {
	ret = NULL;
	PyErr_VIM_FORMAT(N_("failed to run function %s"), (var_na *)name);
    }
    නැතහොත්
	ret = ConvertToPyObject(&rettv);

    clear_tv(&args);
    clear_tv(&rettv);
    නම් (selfdict != NULL)
	clear_tv(&selfdicttv);

    ආපසු ret;
}

    static PyObject *
FunctionRepr(PyObject *self_obj)
{
    FunctionObject *self = (FunctionObject*)self_obj;
    PyObject *ret;
    garray_T repr_ga;
    පූර්ණ_සංඛ්‍යා i;
    char_u *tofree = NULL;
    typval_T tv;
    char_u numbuf[NUMBUFLEN];

    ga_init2(&repr_ga, parimaa_na(var_na), 70);
    ga_concat(&repr_ga, (char_u *)"<vim.Function '");
    නම් (self->name)
	ga_concat(&repr_ga, self->name);
    නැතහොත්
	ga_concat(&repr_ga, (char_u *)"<NULL>");
    ga_append(&repr_ga, '\'');
    නම් (self->argv)
    {
	ga_concat(&repr_ga, (char_u *)", args=[");
	++emsg_silent;
	සඳහා (i = 0; i < self->argc; i++)
	{
	    නම් (i != 0)
		ga_concat(&repr_ga, (char_u *)", ");
	    ga_concat(&repr_ga, tv2string(&self->argv[i], &tofree, numbuf,
			get_copyID()));
	    vim_free(tofree);
	}
	--emsg_silent;
	ga_append(&repr_ga, ']');
    }
    නම් (self->self)
    {
	ga_concat(&repr_ga, (char_u *)", self=");
	tv.v_type = VAR_DICT;
	tv.vval.v_dict = self->self;
	++emsg_silent;
	ga_concat(&repr_ga, tv2string(&tv, &tofree, numbuf, get_copyID()));
	--emsg_silent;
	vim_free(tofree);
	නම් (self->auto_rebind)
	    ga_concat(&repr_ga, (char_u *)", auto_rebind=True");
    }
    ga_append(&repr_ga, '>');
    ret = PyString_FromString((var_na *)repr_ga.ga_data);
    ga_clear(&repr_ga);
    ආපසු ret;
}

static sa_nracanaa PyMethodDef FunctionMethods[] = {
    {"__dir__",	(PyCFunction)FunctionDir,   METH_NOARGS,		""},
    { NULL,	NULL,			0,				NULL}
};

/*
 * Options object
 */

DEFINE_PY_TYPE_OBJECT(OptionsType);

typedef පූර්ණ_සංඛ්‍යා (*checkfun)(void *);

typedef sa_nracanaa
{
    PyObject_HEAD
    පූර්ණ_සංඛ්‍යා		opt_type;
    void	*from;
    checkfun	Check;
    PyObject	*fromObj;
} OptionsObject;

    static පූර්ණ_සංඛ්‍යා
dummy_check(void *arg UNUSED)
{
    ආපසු 0;
}

    static PyObject *
OptionsNew(පූර්ණ_සංඛ්‍යා opt_type, void *from, checkfun Check, PyObject *fromObj)
{
    OptionsObject	*self;

    self = PyObject_GC_New(OptionsObject, OptionsTypePtr);
    නම් (self == NULL)
	ආපසු NULL;

    self->opt_type = opt_type;
    self->from = from;
    self->Check = Check;
    self->fromObj = fromObj;
    නම් (fromObj)
	Py_INCREF(fromObj);

    ආපසු (PyObject *)(self);
}

    static void
OptionsDestructor(PyObject *self_obj)
{
    OptionsObject *self = (OptionsObject*)self_obj;
    PyObject_GC_UnTrack((void *)(self));
    Py_XDECREF(self->fromObj);
    PyObject_GC_Del((void *)(self));
}

    static පූර්ණ_සංඛ්‍යා
OptionsTraverse(PyObject *self_obj, visitproc visit, void *arg)
{
    OptionsObject *self = (OptionsObject*)self_obj;
    Py_VISIT(self->fromObj);
    ආපසු 0;
}

    static පූර්ණ_සංඛ්‍යා
OptionsClear(PyObject *self_obj)
{
    OptionsObject *self = (OptionsObject*)self_obj;
    Py_CLEAR(self->fromObj);
    ආපසු 0;
}

    static PyObject *
OptionsItem(OptionsObject *self, PyObject *keyObject)
{
    char_u	*key;
    පූර්ණ_සංඛ්‍යා		flags;
    long	numval;
    char_u	*stringval;
    PyObject	*todecref;

    නම් (self->Check(self->fromObj))
	ආපසු NULL;

    නම් (!(key = StringToChars(keyObject, &todecref)))
	ආපසු NULL;

    නම් (*key == NUL)
    {
	RAISE_NO_EMPTY_KEYS;
	Py_XDECREF(todecref);
	ආපසු NULL;
    }

    flags = get_option_value_strict(key, &numval, &stringval,
				    self->opt_type, self->from);

    Py_XDECREF(todecref);

    නම් (flags == 0)
    {
	PyErr_SetObject(PyExc_KeyError, keyObject);
	ආපසු NULL;
    }

    නම් (flags & SOPT_UNSET)
    {
	Py_INCREF(Py_None);
	ආපසු Py_None;
    }
    නැතහොත් නම් (flags & SOPT_BOOL)
    {
	PyObject	*ret;
	ret = numval ? Py_True : Py_False;
	Py_INCREF(ret);
	ආපසු ret;
    }
    නැතහොත් නම් (flags & SOPT_NUM)
	ආපසු PyInt_FromLong(numval);
    නැතහොත් නම් (flags & SOPT_STRING)
    {
	නම් (stringval)
	{
	    PyObject	*ret = PyBytes_FromString((var_na *)stringval);
	    vim_free(stringval);
	    ආපසු ret;
	}
	නැතහොත්
	{
	    PyErr_SET_STRING(PyExc_RuntimeError,
		    N_("unable to get option value"));
	    ආපසු NULL;
	}
    }
    නැතහොත්
    {
	PyErr_SET_VIM(N_("internal error: unknown option type"));
	ආපසු NULL;
    }
}

    static පූර්ණ_සංඛ්‍යා
OptionsContains(OptionsObject *self, PyObject *keyObject)
{
    char_u	*key;
    PyObject	*todecref;

    නම් (!(key = StringToChars(keyObject, &todecref)))
	ආපසු -1;

    නම් (*key == NUL)
    {
	Py_XDECREF(todecref);
	ආපසු 0;
    }

    නම් (get_option_value_strict(key, NULL, NULL, self->opt_type, NULL))
    {
	Py_XDECREF(todecref);
	ආපසු 1;
    }
    නැතහොත්
    {
	Py_XDECREF(todecref);
	ආපසු 0;
    }
}

typedef sa_nracanaa
{
    void	*lastoption;
    පූර්ණ_සංඛ්‍යා		opt_type;
} optiterinfo_T;

    static PyObject *
OptionsIterNext(void **arg)
{
    char_u	*name;
    optiterinfo_T **oii = (optiterinfo_T**)arg;

    නම් ((name = option_iter_next(&((*oii)->lastoption), (*oii)->opt_type)))
	ආපසු PyString_FromString((var_na *)name);

    ආපසු NULL;
}

    static PyObject *
OptionsIter(PyObject *self_obj)
{
    OptionsObject	*self = (OptionsObject*)self_obj;
    optiterinfo_T	*oii;

    නම් (!(oii = PyMem_New(optiterinfo_T, 1)))
    {
	PyErr_NoMemory();
	ආපසු NULL;
    }

    oii->opt_type = self->opt_type;
    oii->lastoption = NULL;

    ආපසු IterNew(oii,
	    PyMem_Free, OptionsIterNext,
	    NULL, NULL, (PyObject *)self);
}

    static පූර්ණ_සංඛ්‍යා
set_option_value_err(char_u *key, පූර්ණ_සංඛ්‍යා numval, char_u *stringval, පූර්ණ_සංඛ්‍යා opt_flags)
{
    var_na	*errmsg;

    නම් ((errmsg = set_option_value(key, numval, stringval, opt_flags)))
    {
	නම් (VimTryEnd())
	    ආපසු FAIL;
	PyErr_SetVim(errmsg);
	ආපසු FAIL;
    }
    ආපසු OK;
}

    static පූර්ණ_සංඛ්‍යා
set_option_value_for(
	char_u *key,
	පූර්ණ_සංඛ්‍යා numval,
	char_u *stringval,
	පූර්ණ_සංඛ්‍යා opt_flags,
	පූර්ණ_සංඛ්‍යා opt_type,
	void *from)
{
    switchwin_T	switchwin;
    bufref_T	save_curbuf;
    පූර්ණ_සංඛ්‍යා		set_ret = 0;

    VimTryStart();
    switch (opt_type)
    {
	pak_sa SREQ_WIN:
	    නම් (switch_win(&switchwin, (win_T *)from,
			      win_find_tabpage((win_T *)from), FALSE) == FAIL)
	    {
		restore_win(&switchwin, TRUE);
		නම් (VimTryEnd())
		    ආපසු -1;
		PyErr_SET_VIM(N_("problem while switching windows"));
		ආපසු -1;
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
    නම් (set_ret == FAIL)
	ආපසු -1;
    ආපසු VimTryEnd();
}

    static පූර්ණ_සංඛ්‍යා
OptionsAssItem(OptionsObject *self, PyObject *keyObject, PyObject *valObject)
{
    char_u	*key;
    පූර්ණ_සංඛ්‍යා		flags;
    පූර්ණ_සංඛ්‍යා		opt_flags;
    පූර්ණ_සංඛ්‍යා		ret = 0;
    PyObject	*todecref;

    නම් (self->Check(self->fromObj))
	ආපසු -1;

    නම් (!(key = StringToChars(keyObject, &todecref)))
	ආපසු -1;

    නම් (*key == NUL)
    {
	RAISE_NO_EMPTY_KEYS;
	Py_XDECREF(todecref);
	ආපසු -1;
    }

    flags = get_option_value_strict(key, NULL, NULL,
				    self->opt_type, self->from);

    නම් (flags == 0)
    {
	PyErr_SetObject(PyExc_KeyError, keyObject);
	Py_XDECREF(todecref);
	ආපසු -1;
    }

    නම් (valObject == NULL)
    {
	නම් (self->opt_type == SREQ_GLOBAL)
	{
	    PyErr_FORMAT(PyExc_ValueError,
		    N_("unable to unset global option %s"), key);
	    Py_XDECREF(todecref);
	    ආපසු -1;
	}
	නැතහොත් නම් (!(flags & SOPT_GLOBAL))
	{
	    PyErr_FORMAT(PyExc_ValueError,
		    N_("unable to unset option %s "
		       "which does not have global value"), key);
	    Py_XDECREF(todecref);
	    ආපසු -1;
	}
	නැතහොත්
	{
	    unset_global_local_option(key, self->from);
	    Py_XDECREF(todecref);
	    ආපසු 0;
	}
    }

    opt_flags = (self->opt_type ? OPT_LOCAL : OPT_GLOBAL);

    නම් (flags & SOPT_BOOL)
    {
	පූර්ණ_සංඛ්‍යා	istrue = PyObject_IsTrue(valObject);

	නම් (istrue == -1)
	    ret = -1;
	නැතහොත්
	    ret = set_option_value_for(key, istrue, NULL,
				    opt_flags, self->opt_type, self->from);
    }
    නැතහොත් නම් (flags & SOPT_NUM)
    {
	long	val;

	නම් (NumberToLong(valObject, &val, NUMBER_INT))
	{
	    Py_XDECREF(todecref);
	    ආපසු -1;
	}

	ret = set_option_value_for(key, (පූර්ණ_සංඛ්‍යා) val, NULL, opt_flags,
				self->opt_type, self->from);
    }
    නැතහොත්
    {
	char_u		*val;
	PyObject	*todecref2;

	නම් ((val = StringToChars(valObject, &todecref2)))
	{
	    ret = set_option_value_for(key, 0, val, opt_flags,
				    self->opt_type, self->from);
	    Py_XDECREF(todecref2);
	}
	නැතහොත්
	    ret = -1;
    }

    Py_XDECREF(todecref);

    ආපසු ret;
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

    static පූර්ණ_සංඛ්‍යා
CheckTabPage(TabPageObject *self)
{
    නම් (self->tab == INVALID_TABPAGE_VALUE)
    {
	PyErr_SET_VIM(N_("attempt to refer to deleted tab page"));
	ආපසු -1;
    }

    ආපසු 0;
}

    static PyObject *
TabPageNew(tabpage_T *tab)
{
    TabPageObject *self;

    නම් (TAB_PYTHON_REF(tab))
    {
	self = TAB_PYTHON_REF(tab);
	Py_INCREF((PyObject *)self);
    }
    නැතහොත්
    {
	self = PyObject_NEW(TabPageObject, TabPageTypePtr);
	නම් (self == NULL)
	    ආපසු NULL;
	self->tab = tab;
	TAB_PYTHON_REF(tab) = self;
    }

    ආපසු (PyObject *)(self);
}

    static void
TabPageDestructor(PyObject *self_obj)
{
    TabPageObject *self = (TabPageObject*)self_obj;
    නම් (self->tab && self->tab != INVALID_TABPAGE_VALUE)
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
    ආපසු ObjectDir(self, TabPageAttrs);
}

    static PyObject *
TabPageAttrValid(TabPageObject *self, var_na *name)
{
    PyObject	*ret;

    නම් (strcmp(name, "valid") != 0)
	ආපසු NULL;

    ret = ((self->tab == INVALID_TABPAGE_VALUE) ? Py_False : Py_True);
    Py_INCREF(ret);
    ආපසු ret;
}

    static PyObject *
TabPageAttr(TabPageObject *self, var_na *name)
{
    නම් (strcmp(name, "windows") == 0)
	ආපසු WinListNew(self);
    නැතහොත් නම් (strcmp(name, "number") == 0)
	ආපසු PyLong_FromLong((long) get_tab_number(self->tab));
    නැතහොත් නම් (strcmp(name, "vars") == 0)
	ආපසු NEW_DICTIONARY(self->tab->tp_vars);
    නැතහොත් නම් (strcmp(name, "window") == 0)
    {
	// For current tab window.c does not bother to set or update tp_curwin
	නම් (self->tab == curtab)
	    ආපසු WindowNew(curwin, curtab);
	නැතහොත්
	    ආපසු WindowNew(self->tab->tp_curwin, self->tab);
    }
    නැතහොත් නම් (strcmp(name, "__members__") == 0)
	ආපසු ObjectDir(NULL, TabPageAttrs);
    ආපසු NULL;
}

    static PyObject *
TabPageRepr(PyObject *self_obj)
{
    TabPageObject *self = (TabPageObject*)self_obj;
    නම් (self->tab == INVALID_TABPAGE_VALUE)
	ආපසු PyString_FromFormat("<tabpage object (deleted) at %p>", (void *)self);
    නැතහොත්
    {
	පූර්ණ_සංඛ්‍යා	t = get_tab_number(self->tab);

	නම් (t == 0)
	    ආපසු PyString_FromFormat("<tabpage object (unknown) at %p>",
					(void *)self);
	නැතහොත්
	    ආපසු PyString_FromFormat("<tabpage %d>", t - 1);
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

    අතරතුර (tp != NULL)
    {
	++n;
	tp = tp->tp_next;
    }

    ආපසු n;
}

    static PyObject *
TabListItem(PyObject *self UNUSED, PyInt n)
{
    tabpage_T	*tp;

    සඳහා (tp = first_tabpage; tp != NULL; tp = tp->tp_next, --n)
	නම් (n == 0)
	    ආපසු TabPageNew(tp);

    PyErr_SET_STRING(PyExc_IndexError, N_("no such tab page"));
    ආපසු NULL;
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

    static පූර්ණ_සංඛ්‍යා
CheckWindow(WindowObject *self)
{
    නම් (self->win == INVALID_WINDOW_VALUE)
    {
	PyErr_SET_VIM(N_("attempt to refer to deleted window"));
	ආපසු -1;
    }

    ආපසු 0;
}

    static පූර්ණ_සංඛ්‍යා
CheckWindowCb(void *self)
{
    ආපසු CheckWindow((WindowObject*)self);
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

    නම් (WIN_PYTHON_REF(win))
    {
	self = WIN_PYTHON_REF(win);
	Py_INCREF((PyObject *)self);
    }
    නැතහොත්
    {
	self = PyObject_GC_New(WindowObject, WindowTypePtr);
	නම් (self == NULL)
	    ආපසු NULL;
	self->win = win;
	WIN_PYTHON_REF(win) = self;
    }

    self->tabObject = ((TabPageObject *)(TabPageNew(tab)));

    ආපසු (PyObject *)(self);
}

    static void
WindowDestructor(PyObject *self_obj)
{
    WindowObject *self = (WindowObject*)self_obj;
    PyObject_GC_UnTrack((void *)(self));
    නම් (self->win && self->win != INVALID_WINDOW_VALUE)
	WIN_PYTHON_REF(self->win) = NULL;
    Py_XDECREF(((PyObject *)(self->tabObject)));
    PyObject_GC_Del((void *)(self));
}

    static පූර්ණ_සංඛ්‍යා
WindowTraverse(PyObject *self_obj, visitproc visit, void *arg)
{
    WindowObject *self = (WindowObject*)self_obj;
    Py_VISIT(((PyObject *)(self->tabObject)));
    ආපසු 0;
}

    static පූර්ණ_සංඛ්‍යා
WindowClear(PyObject *self_obj)
{
    WindowObject *self = (WindowObject*)self_obj;
    Py_CLEAR(self->tabObject);
    ආපසු 0;
}

    static win_T *
get_firstwin(TabPageObject *tabObject)
{
    නම් (tabObject)
    {
	නම් (CheckTabPage(tabObject))
	    ආපසු NULL;
	// For current tab window.c does not bother to set or update tp_firstwin
	නැතහොත් නම් (tabObject->tab == curtab)
	    ආපසු firstwin;
	නැතහොත්
	    ආපසු tabObject->tab->tp_firstwin;
    }
    නැතහොත්
	ආපසු firstwin;
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
    ආපසු ObjectDir(self, WindowAttrs);
}

    static PyObject *
WindowAttrValid(WindowObject *self, var_na *name)
{
    PyObject	*ret;

    නම් (strcmp(name, "valid") != 0)
	ආපසු NULL;

    ret = ((self->win == INVALID_WINDOW_VALUE) ? Py_False : Py_True);
    Py_INCREF(ret);
    ආපසු ret;
}

    static PyObject *
WindowAttr(WindowObject *self, var_na *name)
{
    නම් (strcmp(name, "buffer") == 0)
	ආපසු (PyObject *)BufferNew(self->win->w_buffer);
    නැතහොත් නම් (strcmp(name, "cursor") == 0)
    {
	pos_T *pos = &self->win->w_cursor;

	ආපසු Py_BuildValue("(ll)", (long)(pos->lnum), (long)(pos->col));
    }
    නැතහොත් නම් (strcmp(name, "height") == 0)
	ආපසු PyLong_FromLong((long)(self->win->w_height));
    නැතහොත් නම් (strcmp(name, "row") == 0)
	ආපසු PyLong_FromLong((long)(self->win->w_winrow));
    නැතහොත් නම් (strcmp(name, "width") == 0)
	ආපසු PyLong_FromLong((long)(self->win->w_width));
    නැතහොත් නම් (strcmp(name, "col") == 0)
	ආපසු PyLong_FromLong((long)(self->win->w_wincol));
    නැතහොත් නම් (strcmp(name, "vars") == 0)
	ආපසු NEW_DICTIONARY(self->win->w_vars);
    නැතහොත් නම් (strcmp(name, "options") == 0)
	ආපසු OptionsNew(SREQ_WIN, self->win, CheckWindowCb,
			(PyObject *) self);
    නැතහොත් නම් (strcmp(name, "number") == 0)
    {
	නම් (CheckTabPage(self->tabObject))
	    ආපසු NULL;
	ආපසු PyLong_FromLong((long)
		get_win_number(self->win, get_firstwin(self->tabObject)));
    }
    නැතහොත් නම් (strcmp(name, "tabpage") == 0)
    {
	Py_INCREF((PyObject *)self->tabObject);
	ආපසු (PyObject *)(self->tabObject);
    }
    නැතහොත් නම් (strcmp(name, "__members__") == 0)
	ආපසු ObjectDir(NULL, WindowAttrs);
    නැතහොත්
	ආපසු NULL;
}

    static පූර්ණ_සංඛ්‍යා
WindowSetattr(PyObject *self_obj, var_na *name, PyObject *valObject)
{
    WindowObject *self = (WindowObject*)self_obj;
    නම් (CheckWindow(self))
	ආපසු -1;

    නම් (strcmp(name, "buffer") == 0)
    {
	PyErr_SET_STRING(PyExc_TypeError, N_("readonly attribute: buffer"));
	ආපසු -1;
    }
    නැතහොත් නම් (strcmp(name, "cursor") == 0)
    {
	long lnum;
	long col;

	නම් (!PyArg_Parse(valObject, "(ll)", &lnum, &col))
	    ආපසු -1;

	නම් (lnum <= 0 || lnum > self->win->w_buffer->b_ml.ml_line_count)
	{
	    PyErr_SET_VIM(N_("cursor position outside buffer"));
	    ආපසු -1;
	}

	// Check for keyboard interrupts
	නම් (VimCheckInterrupt())
	    ආපසු -1;

	self->win->w_cursor.lnum = lnum;
	self->win->w_cursor.col = col;
	self->win->w_set_curswant = TRUE;
	self->win->w_cursor.coladd = 0;
	// When column is out of range silently correct it.
	check_cursor_col_win(self->win);

	update_screen(UPD_VALID);
	ආපසු 0;
    }
    නැතහොත් නම් (strcmp(name, "height") == 0)
    {
	long	height;
	win_T	*savewin;

	නම් (NumberToLong(valObject, &height, NUMBER_INT|NUMBER_UNSIGNED))
	    ආපසු -1;

#ifdef FEAT_GUI
	need_mouse_correct = TRUE;
#endif
	savewin = curwin;
	curwin = self->win;
	curbuf = curwin->w_buffer;

	VimTryStart();
	win_setheight((පූර්ණ_සංඛ්‍යා) height);
	curwin = savewin;
	curbuf = curwin->w_buffer;
	නම් (VimTryEnd())
	    ආපසු -1;

	ආපසු 0;
    }
    නැතහොත් නම් (strcmp(name, "width") == 0)
    {
	long	width;
	win_T	*savewin;

	නම් (NumberToLong(valObject, &width, NUMBER_INT|NUMBER_UNSIGNED))
	    ආපසු -1;

#ifdef FEAT_GUI
	need_mouse_correct = TRUE;
#endif
	savewin = curwin;
	curwin = self->win;
	curbuf = curwin->w_buffer;

	VimTryStart();
	win_setwidth((පූර්ණ_සංඛ්‍යා) width);
	curwin = savewin;
	curbuf = curwin->w_buffer;
	නම් (VimTryEnd())
	    ආපසු -1;

	ආපසු 0;
    }
    නැතහොත්
    {
	PyErr_SetString(PyExc_AttributeError, name);
	ආපසු -1;
    }
}

    static PyObject *
WindowRepr(PyObject *self_obj)
{
    WindowObject *self = (WindowObject*)self_obj;
    නම් (self->win == INVALID_WINDOW_VALUE)
	ආපසු PyString_FromFormat("<window object (deleted) at %p>", (void *)self);
    නැතහොත්
    {
	පූර්ණ_සංඛ්‍යා	w = get_win_number(self->win, firstwin);

	නම් (w == 0)
	    ආපසු PyString_FromFormat("<window object (unknown) at %p>",
								      (void *)self);
	නැතහොත්
	    ආපසු PyString_FromFormat("<window %d>", w - 1);
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

    ආපසු (PyObject *)(self);
}

    static void
WinListDestructor(WinListObject *self)
{
    TabPageObject	*tabObject = self->tabObject;

    නම් (tabObject)
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

    නම් (!(w = get_firstwin(self->tabObject)))
	ආපසු -1;

    අතරතුර (w != NULL)
    {
	++n;
	w = W_NEXT(w);
    }

    ආපසු n;
}

    static PyObject *
WinListItem(WinListObject *self, PyInt n)
{
    win_T *w;

    නම් (!(w = get_firstwin(self->tabObject)))
	ආපසු NULL;

    සඳහා (; w != NULL; w = W_NEXT(w), --n)
	නම් (n == 0)
	    ආපසු WindowNew(w, self->tabObject? self->tabObject->tab: curtab);

    PyErr_SET_STRING(PyExc_IndexError, N_("no such window"));
    ආපසු NULL;
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

    නම් (PyBytes_Check(obj))
    {
	නම් (PyBytes_AsStringAndSize(obj, &str, &len) == -1
		|| str == NULL)
	    ආපසු NULL;
    }
    නැතහොත් නම් (PyUnicode_Check(obj))
    {
	නම් (!(bytes = PyUnicode_AsEncodedString(obj, ENC_OPT,
							   ERRORS_ENCODE_ARG)))
	    ආපසු NULL;

	නම් (PyBytes_AsStringAndSize(bytes, &str, &len) == -1
		|| str == NULL)
	{
	    Py_DECREF(bytes);
	    ආපසු NULL;
	}
    }
    නැතහොත්
    {
#නම් PY_MAJOR_VERSION < 3
	PyErr_FORMAT_TYPE(
		N_("expected str() or unicode() instance, but got %s"),
		obj);
#නැතහොත්
	PyErr_FORMAT_TYPE(
		N_("expected bytes() or str() instance, but got %s"),
		obj);
#endif
	ආපසු NULL;
    }

    /*
     * Error checking: String must not contain newlines, as we
     * are replacing a single line, and we must replace it with
     * a single line.
     * A trailing newline is removed, so that append(f.readlines()) works.
     */
    p = memchr(str, '\n', len);
    නම් (p != NULL)
    {
	නම් (p == str + len - 1)
	    --len;
	නැතහොත්
	{
	    PyErr_SET_VIM(N_("string cannot contain newlines"));
	    Py_XDECREF(bytes);
	    ආපසු NULL;
	}
    }

    /*
     * Create a copy of the string, with internal nulls replaced by
     * newline characters, as is the Vim convention.
     */
    save = alloc(len+1);
    නම් (save == NULL)
    {
	PyErr_NoMemory();
	Py_XDECREF(bytes);
	ආපසු NULL;
    }

    සඳහා (i = 0; i < len; ++i)
    {
	නම් (str[i] == '\0')
	    save[i] = '\n';
	නැතහොත්
	    save[i] = str[i];
    }

    save[i] = '\0';
    Py_XDECREF(bytes);  // Python 2 does nothing here

    ආපසු save;
}

/*
 * Get a line from the specified buffer. The line number is
 * in Vim format (1-based). The line is returned as a Python
 * string object.
 */
    static PyObject *
GetBufferLine(buf_T *buf, PyInt n)
{
    ආපසු LineToString((var_na *)ml_get_buf(buf, (linenr_T)n, FALSE));
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

    නම් (list == NULL)
	ආපසු NULL;

    සඳහා (i = 0; i < n; ++i)
    {
	linenr_T	lnum = (linenr_T)(lo + i);
	var_na		*text;
	PyObject	*string;

	නම් (lnum > buf->b_ml.ml_line_count)
	    text = "";
	නැතහොත්
	    text = (var_na *)ml_get_buf(buf, lnum, FALSE);
	string = LineToString(text);
	නම් (string == NULL)
	{
	    Py_DECREF(list);
	    ආපසු NULL;
	}

	PyList_SET_ITEM(list, i, string);
    }

    // The ownership of the Python list is passed to the caller (ie,
    // the caller should Py_DECREF() the object when it is finished
    // with it).

    ආපසු list;
}

/*
 * Check if deleting lines made the cursor position invalid.
 * Changed the lines from "lo" to "hi" and added "extra" lines (negative if
 * deleted).
 */
    static void
py_fix_cursor(linenr_T lo, linenr_T hi, linenr_T extra)
{
    නම් (curwin->w_cursor.lnum >= lo)
    {
	// Adjust the cursor position if it's in/after the changed
	// lines.
	නම් (curwin->w_cursor.lnum >= hi)
	{
	    curwin->w_cursor.lnum += extra;
	    check_cursor_col();
	}
	නැතහොත් නම් (extra < 0)
	{
	    curwin->w_cursor.lnum = lo;
	    check_cursor();
	}
	නැතහොත්
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
    static පූර්ණ_සංඛ්‍යා
SetBufferLine(buf_T *buf, PyInt n, PyObject *line, PyInt *len_change)
{
    bufref_T	save_curbuf = {NULL, 0, 0};
    switchwin_T	switchwin;

    // First of all, we check the type of the supplied Python object.
    // There are three cases:
    //	  1. NULL, or None - this is a deletion.
    //	  2. A string	   - this is a replacement.
    //	  3. Anything else - this is an error.
    නම් (line == Py_None || line == NULL)
    {
	PyErr_Clear();
	switchwin.sw_curwin = NULL;
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	VimTryStart();

	නම් (u_savedel((linenr_T)n, 1L) == FAIL)
	    RAISE_UNDO_FAIL;
	නැතහොත් නම් (ml_delete((linenr_T)n) == FAIL)
	    RAISE_DELETE_LINE_FAIL;
	නැතහොත්
	{
	    නම් (buf == curbuf && (switchwin.sw_curwin != NULL
					   || save_curbuf.br_buf == NULL))
		// Using an existing window for the buffer, adjust the cursor
		// position.
		py_fix_cursor((linenr_T)n, (linenr_T)n + 1, (linenr_T)-1);
	    නම් (save_curbuf.br_buf == NULL)
		// Only adjust marks if we managed to switch to a window that
		// holds the buffer, otherwise line numbers will be invalid.
		deleted_lines_mark((linenr_T)n, 1L);
	}

	restore_win_for_buf(&switchwin, &save_curbuf);

	නම් (VimTryEnd())
	    ආපසු FAIL;

	නම් (len_change)
	    *len_change = -1;

	ආපසු OK;
    }
    නැතහොත් නම් (PyBytes_Check(line) || PyUnicode_Check(line))
    {
	var_na	*save = StringToLine(line);

	නම් (save == NULL)
	    ආපසු FAIL;

	VimTryStart();

	// We do not need to free "save" if ml_replace() consumes it.
	PyErr_Clear();
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	නම් (u_savesub((linenr_T)n) == FAIL)
	{
	    RAISE_UNDO_FAIL;
	    vim_free(save);
	}
	නැතහොත් නම් (ml_replace((linenr_T)n, (char_u *)save, FALSE) == FAIL)
	{
	    RAISE_REPLACE_LINE_FAIL;
	    vim_free(save);
	}
	නැතහොත්
	    changed_bytes((linenr_T)n, 0);

	restore_win_for_buf(&switchwin, &save_curbuf);

	// Check that the cursor is not beyond the end of the line now.
	නම් (buf == curbuf)
	    check_cursor_col();

	නම් (VimTryEnd())
	    ආපසු FAIL;

	නම් (len_change)
	    *len_change = 0;

	ආපසු OK;
    }
    නැතහොත්
    {
	PyErr_BadArgument();
	ආපසු FAIL;
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
    static පූර්ණ_සංඛ්‍යා
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
    නම් (list == Py_None || list == NULL)
    {
	PyInt	i;
	PyInt	n = (පූර්ණ_සංඛ්‍යා)(hi - lo);

	PyErr_Clear();
	VimTryStart();
	switchwin.sw_curwin = NULL;
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	නම් (u_savedel((linenr_T)lo, (long)n) == FAIL)
	    RAISE_UNDO_FAIL;
	නැතහොත්
	{
	    සඳහා (i = 0; i < n; ++i)
	    {
		නම් (ml_delete((linenr_T)lo) == FAIL)
		{
		    RAISE_DELETE_LINE_FAIL;
		    bha_nga;
		}
	    }
	    නම් (buf == curbuf && (switchwin.sw_curwin != NULL
					       || save_curbuf.br_buf == NULL))
		// Using an existing window for the buffer, adjust the cursor
		// position.
		py_fix_cursor((linenr_T)lo, (linenr_T)hi, (linenr_T)-n);
	    නම් (save_curbuf.br_buf == NULL)
		// Only adjust marks if we managed to switch to a window that
		// holds the buffer, otherwise line numbers will be invalid.
		deleted_lines_mark((linenr_T)lo, (long)i);
	}

	restore_win_for_buf(&switchwin, &save_curbuf);

	නම් (VimTryEnd())
	    ආපසු FAIL;

	නම් (len_change)
	    *len_change = -n;

	ආපසු OK;
    }
    නැතහොත් නම් (PyList_Check(list))
    {
	PyInt	i;
	PyInt	new_len = PyList_Size(list);
	PyInt	old_len = hi - lo;
	PyInt	extra = 0;	// lines added to text, can be negative
	var_na	**array;

	නම් (new_len == 0)	// avoid allocating zero bytes
	    array = NULL;
	නැතහොත්
	{
	    array = PyMem_New(var_na *, new_len);
	    නම් (array == NULL)
	    {
		PyErr_NoMemory();
		ආපසු FAIL;
	    }
	}

	සඳහා (i = 0; i < new_len; ++i)
	{
	    PyObject *line;

	    නම් (!(line = PyList_GetItem(list, i)) ||
		!(array[i] = StringToLine(line)))
	    {
		අතරතුර (i)
		    vim_free(array[--i]);
		PyMem_Free(array);
		ආපසු FAIL;
	    }
	}

	VimTryStart();
	PyErr_Clear();

	// START of region without "return".  Must call restore_buffer()!
	switchwin.sw_curwin = NULL;
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	නම් (u_save((linenr_T)(lo-1), (linenr_T)hi) == FAIL)
	    RAISE_UNDO_FAIL;

	// If the size of the range is reducing (ie, new_len < old_len) we
	// need to delete some old_len. We do this at the start, by
	// repeatedly deleting line "lo".
	නම් (!PyErr_Occurred())
	{
	    සඳහා (i = 0; i < old_len - new_len; ++i)
		නම් (ml_delete((linenr_T)lo) == FAIL)
		{
		    RAISE_DELETE_LINE_FAIL;
		    bha_nga;
		}
	    extra -= i;
	}

	// For as long as possible, replace the existing old_len with the
	// new old_len. This is a more efficient operation, as it requires
	// less memory allocation and freeing.
	නම් (!PyErr_Occurred())
	{
	    සඳහා (i = 0; i < old_len && i < new_len; ++i)
		නම් (ml_replace((linenr_T)(lo+i), (char_u *)array[i], FALSE)
								      == FAIL)
		{
		    RAISE_REPLACE_LINE_FAIL;
		    bha_nga;
		}
	}
	නැතහොත්
	    i = 0;

	// Now we may need to insert the remaining new old_len. If we do, we
	// must free the strings as we finish with them (we can't pass the
	// responsibility to Vim in this case).
	නම් (!PyErr_Occurred())
	{
	    අතරතුර (i < new_len)
	    {
		නම් (ml_append((linenr_T)(lo + i - 1),
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
	අතරතුර (i < new_len)
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
	නම් (save_curbuf.br_buf == NULL)
	{
	    mark_adjust((linenr_T)lo, (linenr_T)(hi - 1),
						  (long)MAXLNUM, (long)extra);
	    changed_lines((linenr_T)lo, 0, (linenr_T)hi, (long)extra);
	}

	නම් (buf == curbuf && (switchwin.sw_curwin != NULL
					   || save_curbuf.br_buf == NULL))
	    // Using an existing window for the buffer, adjust the cursor
	    // position.
	    py_fix_cursor((linenr_T)lo, (linenr_T)hi, (linenr_T)extra);

	// END of region without "return".
	restore_win_for_buf(&switchwin, &save_curbuf);

	නම් (VimTryEnd())
	    ආපසු FAIL;

	නම් (len_change)
	    *len_change = new_len - old_len;

	ආපසු OK;
    }
    නැතහොත්
    {
	PyErr_BadArgument();
	ආපසු FAIL;
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
    static පූර්ණ_සංඛ්‍යා
InsertBufferLines(buf_T *buf, PyInt n, PyObject *lines, PyInt *len_change)
{
    bufref_T	save_curbuf = {NULL, 0, 0};
    switchwin_T	switchwin;

    // First of all, we check the type of the supplied Python object.
    // It must be a string or a list, or the call is in error.
    නම් (PyBytes_Check(lines) || PyUnicode_Check(lines))
    {
	var_na		*str = StringToLine(lines);

	නම් (str == NULL)
	    ආපසු FAIL;

	PyErr_Clear();
	VimTryStart();
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	නම් (u_save((linenr_T)n, (linenr_T)(n + 1)) == FAIL)
	    RAISE_UNDO_FAIL;
	නැතහොත් නම් (ml_append((linenr_T)n, (char_u *)str, 0, FALSE) == FAIL)
	    RAISE_INSERT_LINE_FAIL;
	නැතහොත් නම් (save_curbuf.br_buf == NULL)
	    // Only adjust marks if we managed to switch to a window that
	    // holds the buffer, otherwise line numbers will be invalid.
	    appended_lines_mark((linenr_T)n, 1L);

	vim_free(str);
	restore_win_for_buf(&switchwin, &save_curbuf);
	update_screen(UPD_VALID);

	නම් (VimTryEnd())
	    ආපසු FAIL;

	නම් (len_change)
	    *len_change = 1;

	ආපසු OK;
    }
    නැතහොත් නම් (PyList_Check(lines))
    {
	PyInt	i;
	PyInt	size = PyList_Size(lines);
	var_na	**array;

	array = PyMem_New(var_na *, size);
	නම් (array == NULL)
	{
	    PyErr_NoMemory();
	    ආපසු FAIL;
	}

	සඳහා (i = 0; i < size; ++i)
	{
	    PyObject *line;

	    නම් (!(line = PyList_GetItem(lines, i)) ||
		!(array[i] = StringToLine(line)))
	    {
		අතරතුර (i)
		    vim_free(array[--i]);
		PyMem_Free(array);
		ආපසු FAIL;
	    }
	}

	PyErr_Clear();
	VimTryStart();
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	නම් (u_save((linenr_T)n, (linenr_T)(n + 1)) == FAIL)
	    RAISE_UNDO_FAIL;
	නැතහොත්
	{
	    සඳහා (i = 0; i < size; ++i)
	    {
		නම් (ml_append((linenr_T)(n + i),
					(char_u *)array[i], 0, FALSE) == FAIL)
		{
		    RAISE_INSERT_LINE_FAIL;

		    // Free the rest of the lines
		    අතරතුර (i < size)
			vim_free(array[i++]);

		    bha_nga;
		}
		vim_free(array[i]);
	    }
	    නම් (i > 0 && save_curbuf.br_buf == NULL)
		// Only adjust marks if we managed to switch to a window that
		// holds the buffer, otherwise line numbers will be invalid.
		appended_lines_mark((linenr_T)n, (long)i);
	}

	// Free the array of lines. All of its contents have now
	// been freed.
	PyMem_Free(array);
	restore_win_for_buf(&switchwin, &save_curbuf);

	update_screen(UPD_VALID);

	නම් (VimTryEnd())
	    ආපසු FAIL;

	නම් (len_change)
	    *len_change = size;

	ආපසු OK;
    }
    නැතහොත්
    {
	PyErr_BadArgument();
	ආපසු FAIL;
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

    static පූර්ණ_සංඛ්‍යා
CheckBuffer(BufferObject *self)
{
    නම් (self->buf == INVALID_BUFFER_VALUE)
    {
	PyErr_SET_VIM(N_("attempt to refer to deleted buffer"));
	ආපසු -1;
    }

    ආපසු 0;
}

    static පූර්ණ_සංඛ්‍යා
CheckBufferCb(void *self)
{
    ආපසු CheckBuffer((BufferObject*)self);
}

    static PyObject *
RBItem(BufferObject *self, PyInt n, PyInt start, PyInt end)
{
    නම් (CheckBuffer(self))
	ආපසු NULL;

    නම් (end == -1)
	end = self->buf->b_ml.ml_line_count;

    නම් (n < 0)
	n += end - start + 1;

    නම් (n < 0 || n > end - start)
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("line number out of range"));
	ආපසු NULL;
    }

    ආපසු GetBufferLine(self->buf, n+start);
}

    static PyObject *
RBSlice(BufferObject *self, PyInt lo, PyInt hi, PyInt start, PyInt end)
{
    PyInt size;

    නම් (CheckBuffer(self))
	ආපසු NULL;

    නම් (end == -1)
	end = self->buf->b_ml.ml_line_count;

    size = end - start + 1;

    නම් (lo < 0)
	lo = 0;
    නැතහොත් නම් (lo > size)
	lo = size;
    නම් (hi < 0)
	hi = 0;
    නම් (hi < lo)
	hi = lo;
    නැතහොත් නම් (hi > size)
	hi = size;

    ආපසු GetBufferLineList(self->buf, lo+start, hi+start);
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

    නම් (CheckBuffer(self))
	ආපසු -1;

    නම් (end == -1)
	end = self->buf->b_ml.ml_line_count;

    නම් (n < 0)
	n += end - start + 1;

    නම් (n < 0 || n > end - start)
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("line number out of range"));
	ආපසු -1;
    }

    නම් (SetBufferLine(self->buf, n+start, valObject, &len_change) == FAIL)
	ආපසු -1;

    නම් (new_end)
	*new_end = end + len_change;

    ආපසු 0;
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
    නම් (CheckBuffer(self))
	ආපසු -1;

    නම් (end == -1)
	end = self->buf->b_ml.ml_line_count;

    // Sort out the slice range
    size = end - start + 1;

    නම් (lo < 0)
	lo = 0;
    නැතහොත් නම් (lo > size)
	lo = size;
    නම් (hi < 0)
	hi = 0;
    නම් (hi < lo)
	hi = lo;
    නැතහොත් නම් (hi > size)
	hi = size;

    නම් (SetBufferLineList(self->buf, lo + start, hi + start,
						valObject, &len_change) == FAIL)
	ආපසු -1;

    නම් (new_end)
	*new_end = end + len_change;

    ආපසු 0;
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

    නම් (CheckBuffer(self))
	ආපසු NULL;

    නම් (end == -1)
	end = self->buf->b_ml.ml_line_count;

    max = n = end - start + 1;

    නම් (!PyArg_ParseTuple(args, "O|n", &lines, &n))
	ආපසු NULL;

    නම් (n < 0 || n > max)
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("line number out of range"));
	ආපසු NULL;
    }

    නම් (InsertBufferLines(self->buf, n + start - 1, lines, &len_change) == FAIL)
	ආපසු NULL;

    නම් (new_end)
	*new_end = end + len_change;

    Py_INCREF(Py_None);
    ආපසු Py_None;
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
    නම් (self == NULL)
	ආපසු NULL;

    bufr = (BufferObject *)BufferNew(buf);
    නම් (bufr == NULL)
    {
	Py_DECREF(self);
	ආපසු NULL;
    }
    Py_INCREF((PyObject *)bufr);

    self->buf = bufr;
    self->start = start;
    self->end = end;

    ආපසු (PyObject *)(self);
}

    static void
RangeDestructor(PyObject *self_obj)
{
    RangeObject *self = (RangeObject*)self_obj;
    PyObject_GC_UnTrack((void *)(self));
    Py_XDECREF(((PyObject *)(self->buf)));
    PyObject_GC_Del((void *)(self));
}

    static පූර්ණ_සංඛ්‍යා
RangeTraverse(PyObject *self_obj, visitproc visit, void *arg)
{
    RangeObject *self = (RangeObject*)self_obj;
    Py_VISIT(((PyObject *)(self->buf)));
    ආපසු 0;
}

    static පූර්ණ_සංඛ්‍යා
RangeClear(PyObject *self_obj)
{
    RangeObject *self = (RangeObject*)self_obj;
    Py_CLEAR(self->buf);
    ආපසු 0;
}

    static PyInt
RangeLength(RangeObject *self)
{
    // HOW DO WE SIGNAL AN ERROR FROM THIS FUNCTION?
    නම් (CheckBuffer(self->buf))
	ආපසු -1; // ???

    ආපසු (self->end - self->start + 1);
}

    static PyObject *
RangeItem(RangeObject *self, PyInt n)
{
    ආපසු RBItem(self->buf, n, self->start, self->end);
}

    static PyObject *
RangeSlice(RangeObject *self, PyInt lo, PyInt hi)
{
    ආපසු RBSlice(self->buf, lo, hi, self->start, self->end);
}

static var_na *RangeAttrs[] = {
    "start", "end",
    NULL
};

    static PyObject *
RangeDir(PyObject *self, PyObject *args UNUSED)
{
    ආපසු ObjectDir(self, RangeAttrs);
}

    static PyObject *
RangeAppend(RangeObject *self, PyObject *args)
{
    ආපසු RBAppend(self->buf, args, self->start, self->end, &self->end);
}

    static PyObject *
RangeRepr(PyObject *self_obj)
{
    RangeObject *self = (RangeObject*)self_obj;
    නම් (self->buf->buf == INVALID_BUFFER_VALUE)
	ආපසු PyString_FromFormat("<range object (for deleted buffer) at %p>",
				    (void *)self);
    නැතහොත්
    {
	var_na *name = (var_na *)self->buf->buf->b_fname;

	නම් (name == NULL)
	    name = "";

	ආපසු PyString_FromFormat("<range %s (%d:%d)>",
				    name, (පූර්ණ_සංඛ්‍යා)self->start, (පූර්ණ_සංඛ්‍යා)self->end);
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

    නම් (BUF_PYTHON_REF(buf) != NULL)
    {
	self = BUF_PYTHON_REF(buf);
	Py_INCREF((PyObject *)self);
    }
    නැතහොත්
    {
	self = PyObject_NEW(BufferObject, BufferTypePtr);
	නම් (self == NULL)
	    ආපසු NULL;
	self->buf = buf;
	BUF_PYTHON_REF(buf) = self;
    }

    ආපසු (PyObject *)(self);
}

    static void
BufferDestructor(PyObject *self_obj)
{
    BufferObject *self = (BufferObject*)self_obj;
    නම් (self->buf && self->buf != INVALID_BUFFER_VALUE)
	BUF_PYTHON_REF(self->buf) = NULL;

    DESTRUCTOR_FINISH(self);
}

    static PyInt
BufferLength(BufferObject *self)
{
    // HOW DO WE SIGNAL AN ERROR FROM THIS FUNCTION?
    නම් (CheckBuffer(self))
	ආපසු -1; // ???

    ආපසු (PyInt)(self->buf->b_ml.ml_line_count);
}

    static PyObject *
BufferItem(BufferObject *self, PyInt n)
{
    ආපසු RBItem(self, n, 1, -1);
}

    static PyObject *
BufferSlice(BufferObject *self, PyInt lo, PyInt hi)
{
    ආපසු RBSlice(self, lo, hi, 1, -1);
}

static var_na *BufferAttrs[] = {
    "name", "number", "vars", "options", "valid",
    NULL
};

    static PyObject *
BufferDir(PyObject *self, PyObject *args UNUSED)
{
    ආපසු ObjectDir(self, BufferAttrs);
}

    static PyObject *
BufferAttrValid(BufferObject *self, var_na *name)
{
    PyObject	*ret;

    නම් (strcmp(name, "valid") != 0)
	ආපසු NULL;

    ret = ((self->buf == INVALID_BUFFER_VALUE) ? Py_False : Py_True);
    Py_INCREF(ret);
    ආපසු ret;
}

    static PyObject *
BufferAttr(BufferObject *self, var_na *name)
{
    නම් (strcmp(name, "name") == 0)
	ආපසු PyString_FromString((self->buf->b_ffname == NULL
				    ? "" : (var_na *)self->buf->b_ffname));
    නැතහොත් නම් (strcmp(name, "number") == 0)
	ආපසු Py_BuildValue(Py_ssize_t_fmt, self->buf->b_fnum);
    නැතහොත් නම් (strcmp(name, "vars") == 0)
	ආපසු NEW_DICTIONARY(self->buf->b_vars);
    නැතහොත් නම් (strcmp(name, "options") == 0)
	ආපසු OptionsNew(SREQ_BUF, self->buf, CheckBufferCb,
			(PyObject *) self);
    නැතහොත් නම් (strcmp(name, "__members__") == 0)
	ආපසු ObjectDir(NULL, BufferAttrs);
    නැතහොත්
	ආපසු NULL;
}

    static පූර්ණ_සංඛ්‍යා
BufferSetattr(PyObject *self_obj, var_na *name, PyObject *valObject)
{
    BufferObject *self = (BufferObject*)self_obj;
    නම් (CheckBuffer(self))
	ආපසු -1;

    නම් (strcmp(name, "name") == 0)
    {
	char_u		*val;
	aco_save_T	aco;
	පූර්ණ_සංඛ්‍යා		ren_ret = OK;
	PyObject	*todecref;

	නම් (!(val = StringToChars(valObject, &todecref)))
	    ආපසු -1;

	VimTryStart();
	// Using aucmd_*: autocommands will be executed by rename_buffer
	aucmd_prepbuf(&aco, self->buf);
	නම් (curbuf == self->buf)
	{
	    ren_ret = rename_buffer(val);
	    aucmd_restbuf(&aco);
	}
	Py_XDECREF(todecref);
	නම් (VimTryEnd())
	    ආපසු -1;

	නම් (ren_ret == FAIL)
	{
	    PyErr_SET_VIM(N_("failed to rename buffer"));
	    ආපසු -1;
	}
	ආපසු 0;
    }
    නැතහොත්
    {
	PyErr_SetString(PyExc_AttributeError, name);
	ආපසු -1;
    }
}

    static PyObject *
BufferAppend(BufferObject *self, PyObject *args)
{
    ආපසු RBAppend(self, args, 1, -1, NULL);
}

    static PyObject *
BufferMark(BufferObject *self, PyObject *pmarkObject)
{
    pos_T	*posp;
    char_u	*pmark;
    char_u	mark;
    bufref_T	savebuf;
    PyObject	*todecref;

    නම් (CheckBuffer(self))
	ආපසු NULL;

    නම් (!(pmark = StringToChars(pmarkObject, &todecref)))
	ආපසු NULL;

    නම් (pmark[0] == '\0' || pmark[1] != '\0')
    {
	PyErr_SET_STRING(PyExc_ValueError,
		N_("mark name must be a single character"));
	Py_XDECREF(todecref);
	ආපසු NULL;
    }

    mark = *pmark;

    Py_XDECREF(todecref);

    VimTryStart();
    switch_buffer(&savebuf, self->buf);
    posp = getmark(mark, FALSE);
    restore_buffer(&savebuf);
    නම් (VimTryEnd())
	ආපසු NULL;

    නම් (posp == NULL)
    {
	PyErr_SET_VIM(N_("invalid mark name"));
	ආපසු NULL;
    }

    නම් (posp->lnum <= 0)
    {
	// Or raise an error?
	Py_INCREF(Py_None);
	ආපසු Py_None;
    }

    ආපසු Py_BuildValue("(ll)", (long)(posp->lnum), (long)(posp->col));
}

    static PyObject *
BufferRange(BufferObject *self, PyObject *args)
{
    PyInt start;
    PyInt end;

    නම් (CheckBuffer(self))
	ආපසු NULL;

    නම් (!PyArg_ParseTuple(args, "nn", &start, &end))
	ආපසු NULL;

    ආපසු RangeNew(self->buf, start, end);
}

    static PyObject *
BufferRepr(PyObject *self_obj)
{
    BufferObject *self = (BufferObject*)self_obj;
    නම් (self->buf == INVALID_BUFFER_VALUE)
	ආපසු PyString_FromFormat("<buffer object (deleted) at %p>", (void *)self);
    නැතහොත්
    {
	var_na	*name = (var_na *)self->buf->b_fname;

	නම් (name == NULL)
	    name = "";

	ආපසු PyString_FromFormat("<buffer %s>", name);
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

    අතරතුර (b)
    {
	++n;
	b = b->b_next;
    }

    ආපසු n;
}

    static PyObject *
BufMapItem(PyObject *self UNUSED, PyObject *keyObject)
{
    buf_T	*b;
    long	bnr;

    නම් (NumberToLong(keyObject, &bnr, NUMBER_INT|NUMBER_NATURAL))
	ආපසු NULL;

    b = buflist_findnr((පූර්ණ_සංඛ්‍යා) bnr);

    නම් (b)
	ආපසු BufferNew(b);
    නැතහොත්
    {
	PyErr_SetObject(PyExc_KeyError, keyObject);
	ආපසු NULL;
    }
}

    static void
BufMapIterDestruct(void* arg)
{
    PyObject *buffer = (PyObject*)arg;
    // Iteration was stopped before all buffers were processed
    නම් (buffer)
    {
	Py_DECREF(buffer);
    }
}

    static පූර්ණ_සංඛ්‍යා
BufMapIterTraverse(void *iter, visitproc visit, void *arg)
{
    PyObject *buffer = (PyObject*)iter;
    නම් (buffer)
	Py_VISIT(buffer);
    ආපසු 0;
}

    static පූර්ණ_සංඛ්‍යා
BufMapIterClear(void **iter)
{
    PyObject **buffer = (PyObject**)iter;
    නම් (*buffer)
	Py_CLEAR(*buffer);
    ආපසු 0;
}

    static PyObject *
BufMapIterNext(void **arg)
{
    PyObject	*next;
    PyObject	*ret;
    PyObject	**buffer = (PyObject**)arg;

    නම් (!*buffer)
	ආපසු NULL;

    ret = *buffer;

    නම් (CheckBuffer((BufferObject *)(ret)))
    {
	*buffer = NULL;
	ආපසු NULL;
    }

    නම් (!((BufferObject *)(ret))->buf->b_next)
	next = NULL;
    නැතහොත් නම් (!(next = BufferNew(((BufferObject *)(ret))->buf->b_next)))
	ආපසු NULL;
    *buffer = next;
    // Do not increment reference: we no longer hold it (decref), but whoever
    // on other side will hold (incref). Decref+incref = nothing.
    ආපසු ret;
}

    static PyObject *
BufMapIter(PyObject *self)
{
    PyObject *buffer;

    buffer = BufferNew(firstbuf);
    ආපසු IterNew(buffer,
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
    ආපසු ObjectDir(self, CurrentAttrs);
}

    static PyObject *
CurrentGetattr(PyObject *self UNUSED, var_na *name)
{
    නම් (strcmp(name, "buffer") == 0)
	ආපසු (PyObject *)BufferNew(curbuf);
    නැතහොත් නම් (strcmp(name, "window") == 0)
	ආපසු (PyObject *)WindowNew(curwin, curtab);
    නැතහොත් නම් (strcmp(name, "tabpage") == 0)
	ආපසු (PyObject *)TabPageNew(curtab);
    නැතහොත් නම් (strcmp(name, "line") == 0)
	ආපසු GetBufferLine(curbuf, (PyInt)curwin->w_cursor.lnum);
    නැතහොත් නම් (strcmp(name, "range") == 0)
	ආපසු RangeNew(curbuf, RangeStart, RangeEnd);
    නැතහොත් නම් (strcmp(name, "__members__") == 0)
	ආපසු ObjectDir(NULL, CurrentAttrs);
    නැතහොත්
#නම් PY_MAJOR_VERSION < 3
	ආපසු Py_FindMethod(WindowMethods, self, name);
#නැතහොත්
	ආපසු NULL;
#endif
}

    static පූර්ණ_සංඛ්‍යා
CurrentSetattr(PyObject *self UNUSED, var_na *name, PyObject *valObject)
{
    නම් (strcmp(name, "line") == 0)
    {
	නම් (SetBufferLine(curbuf, (PyInt)curwin->w_cursor.lnum, valObject,
			  NULL) == FAIL)
	    ආපසු -1;

	ආපසු 0;
    }
    නැතහොත් නම් (strcmp(name, "buffer") == 0)
    {
	පූර්ණ_සංඛ්‍යා count;

	නම් (valObject->ob_type != BufferTypePtr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.Buffer object, but got %s"),
		    valObject);
	    ආපසු -1;
	}

	නම් (CheckBuffer((BufferObject *)(valObject)))
	    ආපසු -1;
	count = ((BufferObject *)(valObject))->buf->b_fnum;

	VimTryStart();
	නම් (do_buffer(DOBUF_GOTO, DOBUF_FIRST, FORWARD, count, 0) == FAIL)
	{
	    නම් (VimTryEnd())
		ආපසු -1;
	    PyErr_VIM_FORMAT(N_("failed to switch to buffer %d"), count);
	    ආපසු -1;
	}

	ආපසු VimTryEnd();
    }
    නැතහොත් නම් (strcmp(name, "window") == 0)
    {
	පූර්ණ_සංඛ්‍යා count;

	නම් (valObject->ob_type != WindowTypePtr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.Window object, but got %s"),
		    valObject);
	    ආපසු -1;
	}

	නම් (CheckWindow((WindowObject *)(valObject)))
	    ආපසු -1;
	count = get_win_number(((WindowObject *)(valObject))->win, firstwin);

	නම් (!count)
	{
	    PyErr_SET_STRING(PyExc_ValueError,
		    N_("failed to find window in the current tab page"));
	    ආපසු -1;
	}

	VimTryStart();
	win_goto(((WindowObject *)(valObject))->win);
	නම් (((WindowObject *)(valObject))->win != curwin)
	{
	    නම් (VimTryEnd())
		ආපසු -1;
	    PyErr_SET_STRING(PyExc_RuntimeError,
		    N_("did not switch to the specified window"));
	    ආපසු -1;
	}

	ආපසු VimTryEnd();
    }
    නැතහොත් නම් (strcmp(name, "tabpage") == 0)
    {
	නම් (valObject->ob_type != TabPageTypePtr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.TabPage object, but got %s"),
		    valObject);
	    ආපසු -1;
	}

	නම් (CheckTabPage((TabPageObject *)(valObject)))
	    ආපසු -1;

	VimTryStart();
	goto_tabpage_tp(((TabPageObject *)(valObject))->tab, TRUE, TRUE);
	නම් (((TabPageObject *)(valObject))->tab != curtab)
	{
	    නම් (VimTryEnd())
		ආපසු -1;
	    PyErr_SET_STRING(PyExc_RuntimeError,
		    N_("did not switch to the specified tab page"));
	    ආපසු -1;
	}

	ආපසු VimTryEnd();
    }
    නැතහොත්
    {
	PyErr_SetString(PyExc_AttributeError, name);
	ආපසු -1;
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
    නම් (run_ret != NULL)
    {
	Py_DECREF(run_ret);
    }
    නැතහොත් නම් (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
    {
	emsg(_(e_cant_handle_systemexit_of_python_exception_in_vim));
	PyErr_Clear();
    }
    නැතහොත්
	PyErr_PrintEx(1);
}

static const var_na	*code_hdr = "def " DOPY_FUNC "(line, linenr):\n ";
static පූර්ණ_සංඛ්‍යා		code_hdr_len = 30;

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
    පූර්ණ_සංඛ්‍යා		status;
    PyObject	*pyfunc, *pymain;
    PyObject	*run_ret;
    buf_T	*was_curbuf = curbuf;

    නම් (u_save((linenr_T)RangeStart - 1, (linenr_T)RangeEnd + 1) != OK)
    {
	emsg(_("cannot save undo information"));
	ආපසු;
    }

    len = code_hdr_len + STRLEN(cmd);
    code = PyMem_New(var_na, len + 1);
    memcpy(code, code_hdr, code_hdr_len);
    STRCPY(code + code_hdr_len, cmd);
    run_ret = PyRun_String(code, Py_file_input, globals, globals);
    status = -1;
    නම් (run_ret != NULL)
    {
	status = 0;
	Py_DECREF(run_ret);
    }
    නැතහොත් නම් (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
    {
	PyMem_Free(code);
	emsg(_(e_cant_handle_systemexit_of_python_exception_in_vim));
	PyErr_Clear();
	ආපසු;
    }
    නැතහොත්
	PyErr_PrintEx(1);

    PyMem_Free(code);

    නම් (status)
    {
	emsg(_("failed to run the code"));
	ආපසු;
    }

    status = 0;
    pymain = PyImport_AddModule("__main__");
    pyfunc = PyObject_GetAttrString(pymain, DOPY_FUNC);
#ifdef PY_CAN_RECURSE
    PyGILState_Release(*pygilstate);
#endif

    සඳහා (lnum = RangeStart; lnum <= RangeEnd; ++lnum)
    {
	PyObject	*line;
	PyObject	*linenr;
	PyObject	*ret;

#ifdef PY_CAN_RECURSE
	*pygilstate = PyGILState_Ensure();
#endif
	// Check the line number, the command my have deleted lines.
	නම් (lnum > curbuf->b_ml.ml_line_count
		|| !(line = GetBufferLine(curbuf, lnum)))
	    goto err;
	නම් (!(linenr = PyInt_FromLong((long) lnum)))
	{
	    Py_DECREF(line);
	    goto err;
	}
	ret = PyObject_CallFunctionObjArgs(pyfunc, line, linenr, NULL);
	Py_DECREF(line);
	Py_DECREF(linenr);
	නම් (!ret)
	    goto err;

	// Check that the command didn't switch to another buffer.
	// Check the line number, the command my have deleted lines.
	නම් (curbuf != was_curbuf || lnum > curbuf->b_ml.ml_line_count)
	{
	    Py_XDECREF(ret);
	    goto err;
	}

	නම් (ret != Py_None)
	    නම් (SetBufferLine(curbuf, lnum, ret, NULL) == FAIL)
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
    නම් (!status)
	*pygilstate = PyGILState_Ensure();
#endif
    Py_DECREF(pyfunc);
    PyObject_SetAttrString(pymain, DOPY_FUNC, NULL);
    නම් (status)
	ආපසු;
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
    නම් (run_ret == NULL)
    {
	නම් (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
	{
	    emsg(_(e_cant_handle_systemexit_of_python_exception_in_vim));
	    PyErr_Clear();
	}
	නැතහොත්
	{
	    නම් (PyErr_Occurred() && !msg_silent)
		PyErr_PrintEx(0);
	    emsg(_(e_eval_did_not_return_valid_python_object));
	}
    }
    නැතහොත්
    {
	නම් (ConvertFromPyObject(run_ret, rettv) == -1)
	    emsg(_(e_failed_to_convert_returned_python_object_to_vim_value));
	Py_DECREF(run_ret);
    }
    PyErr_Clear();
}

    static පූර්ණ_සංඛ්‍යා
set_ref_in_py(const පූර්ණ_සංඛ්‍යා copyID)
{
    pylinkedlist_T	*cur;
    list_T		*ll;
    tuple_T		*tt;
    පූර්ණ_සංඛ්‍යා			i;
    පූර්ණ_සංඛ්‍යා			abort = FALSE;
    FunctionObject	*func;

    නම් (lastdict != NULL)
    {
	සඳහා (cur = lastdict ; !abort && cur != NULL ; cur = cur->pll_prev)
	    abort = set_ref_in_dict(((DictionaryObject *)(cur->pll_obj))->dict,
								       copyID);
    }

    නම් (lastlist != NULL)
    {
	සඳහා (cur = lastlist ; !abort && cur != NULL ; cur = cur->pll_prev)
	{
	    ll = ((ListObject *) (cur->pll_obj))->list;
	    abort = set_ref_in_list(ll, copyID);
	}
    }

    නම් (lasttuple != NULL)
    {
	සඳහා (cur = lasttuple ; !abort && cur != NULL ; cur = cur->pll_prev)
	{
	    tt = ((TupleObject *) (cur->pll_obj))->tuple;
	    abort = set_ref_in_tuple(tt, copyID);
	}
    }

    නම් (lastfunc != NULL)
    {
	සඳහා (cur = lastfunc ; !abort && cur != NULL ; cur = cur->pll_prev)
	{
	    func = (FunctionObject *) cur->pll_obj;
	    abort = set_ref_in_dict(func->self, copyID);
	    නම් (func->argc)
		සඳහා (i = 0; !abort && i < func->argc; ++i)
		    abort = abort
			|| set_ref_in_item(&func->argv[i], copyID, NULL, NULL, NULL);
	}
    }

    ආපසු abort;
}

    static පූර්ණ_සංඛ්‍යා
set_string_copy(char_u *str, typval_T *tv)
{
    tv->vval.v_string = vim_strsave(str);
    නම් (tv->vval.v_string == NULL)
    {
	PyErr_NoMemory();
	ආපසු -1;
    }
    ආපසු 0;
}

    static පූර්ණ_සංඛ්‍යා
pydict_to_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    dict_T	*dict;
    char_u	*key;
    dictitem_T	*di;
    PyObject	*keyObject;
    PyObject	*valObject;
    Py_ssize_t	iter = 0;

    නම් (!(dict = py_dict_alloc()))
	ආපසු -1;

    tv->v_type = VAR_DICT;
    tv->vval.v_dict = dict;

    අතරතුර (PyDict_Next(obj, &iter, &keyObject, &valObject))
    {
	PyObject	*todecref = NULL;

	නම් (keyObject == NULL || valObject == NULL)
	{
	    dict_unref(dict);
	    ආපසු -1;
	}

	නම් (!(key = StringToChars(keyObject, &todecref)))
	{
	    dict_unref(dict);
	    ආපසු -1;
	}

	නම් (*key == NUL)
	{
	    dict_unref(dict);
	    Py_XDECREF(todecref);
	    RAISE_NO_EMPTY_KEYS;
	    ආපසු -1;
	}

	di = dictitem_alloc(key);

	Py_XDECREF(todecref);

	නම් (di == NULL)
	{
	    PyErr_NoMemory();
	    dict_unref(dict);
	    ආපසු -1;
	}

	නම් (_ConvertFromPyObject(valObject, &di->di_tv, lookup_dict) == -1)
	{
	    vim_free(di);
	    dict_unref(dict);
	    ආපසු -1;
	}

	නම් (dict_add(dict, di) == FAIL)
	{
	    RAISE_KEY_ADD_FAIL(di->di_key);
	    clear_tv(&di->di_tv);
	    vim_free(di);
	    dict_unref(dict);
	    ආපසු -1;
	}
    }

    --dict->dv_refcount;
    ආපසු 0;
}

    static පූර්ණ_සංඛ්‍යා
pymap_to_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    dict_T	*dict;
    char_u	*key;
    dictitem_T	*di;
    PyObject	*list;
    PyObject	*iterator;
    PyObject	*keyObject;
    PyObject	*valObject;

    නම් (!(dict = py_dict_alloc()))
	ආපසු -1;

    tv->v_type = VAR_DICT;
    tv->vval.v_dict = dict;

    නම් (!(list = PyMapping_Keys(obj)))
    {
	dict_unref(dict);
	ආපසු -1;
    }

    නම් (!(iterator = PyObject_GetIter(list)))
    {
	dict_unref(dict);
	Py_DECREF(list);
	ආපසු -1;
    }
    Py_DECREF(list);

    අතරතුර ((keyObject = PyIter_Next(iterator)))
    {
	PyObject	*todecref;

	නම් (!(key = StringToChars(keyObject, &todecref)))
	{
	    Py_DECREF(keyObject);
	    Py_DECREF(iterator);
	    dict_unref(dict);
	    ආපසු -1;
	}

	නම් (*key == NUL)
	{
	    Py_DECREF(keyObject);
	    Py_DECREF(iterator);
	    Py_XDECREF(todecref);
	    dict_unref(dict);
	    RAISE_NO_EMPTY_KEYS;
	    ආපසු -1;
	}

	නම් (!(valObject = PyObject_GetItem(obj, keyObject)))
	{
	    Py_DECREF(keyObject);
	    Py_DECREF(iterator);
	    Py_XDECREF(todecref);
	    dict_unref(dict);
	    ආපසු -1;
	}

	di = dictitem_alloc(key);

	Py_DECREF(keyObject);
	Py_XDECREF(todecref);

	නම් (di == NULL)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(valObject);
	    dict_unref(dict);
	    PyErr_NoMemory();
	    ආපසු -1;
	}

	නම් (_ConvertFromPyObject(valObject, &di->di_tv, lookup_dict) == -1)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(valObject);
	    vim_free(di);
	    dict_unref(dict);
	    ආපසු -1;
	}

	Py_DECREF(valObject);

	නම් (dict_add(dict, di) == FAIL)
	{
	    RAISE_KEY_ADD_FAIL(di->di_key);
	    Py_DECREF(iterator);
	    dictitem_free(di);
	    dict_unref(dict);
	    ආපසු -1;
	}
    }
    Py_DECREF(iterator);
    --dict->dv_refcount;
    ආපසු 0;
}

    static පූර්ණ_සංඛ්‍යා
pyseq_to_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    list_T	*l;

    නම් (!(l = py_list_alloc()))
	ආපසු -1;

    tv->v_type = VAR_LIST;
    tv->vval.v_list = l;

    නම් (list_py_concat(l, obj, lookup_dict) == -1)
    {
	list_unref(l);
	ආපසු -1;
    }

    --l->lv_refcount;
    ආපසු 0;
}

    static පූර්ණ_සංඛ්‍යා
pytuple_to_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    tuple_T	*t;

    නම් (!(t = py_tuple_alloc()))
	ආපසු -1;

    tv->v_type = VAR_TUPLE;
    tv->vval.v_tuple = t;

    නම් (tuple_py_concat(t, obj, lookup_dict) == -1)
    {
	tuple_unref(t);
	ආපසු -1;
    }

    --t->tv_refcount;
    ආපසු 0;
}

typedef පූර්ණ_සංඛ්‍යා (*pytotvfunc)(PyObject *, typval_T *, PyObject *);

    static පූර්ණ_සංඛ්‍යා
convert_dl(PyObject *obj, typval_T *tv,
				    pytotvfunc py_to_tv, PyObject *lookup_dict)
{
    PyObject	*capsule;
    var_na	hexBuf[parimaa_na(void *) * 2 + 3];

    sprintf(hexBuf, "%p", (void *)obj);

#ifdef PY_USE_CAPSULE
    capsule = PyDict_GetItemString(lookup_dict, hexBuf);
#නැතහොත්
    capsule = (PyObject *)PyDict_GetItemString(lookup_dict, hexBuf);
#endif
    නම් (capsule == NULL)
    {
#ifdef PY_USE_CAPSULE
	capsule = PyCapsule_New(tv, NULL, NULL);
#නැතහොත්
	capsule = PyCObject_FromVoidPtr(tv, NULL);
#endif
	නම් (PyDict_SetItemString(lookup_dict, hexBuf, capsule))
	{
	    Py_DECREF(capsule);
	    tv->v_type = VAR_UNKNOWN;
	    ආපසු -1;
	}

	Py_DECREF(capsule);

	නම් (py_to_tv(obj, tv, lookup_dict) == -1)
	{
	    tv->v_type = VAR_UNKNOWN;
	    ආපසු -1;
	}
	// As we are not using copy_tv which increments reference count we must
	// do it ourself.
	නම් (tv->v_type == VAR_DICT)
	    ++tv->vval.v_dict->dv_refcount;
	නැතහොත් නම් (tv->v_type == VAR_LIST)
	    ++tv->vval.v_list->lv_refcount;
	නැතහොත් නම් (tv->v_type == VAR_TUPLE)
	    ++tv->vval.v_tuple->tv_refcount;
    }
    නැතහොත්
    {
	typval_T	*v;

#ifdef PY_USE_CAPSULE
	v = PyCapsule_GetPointer(capsule, NULL);
#නැතහොත්
	v = PyCObject_AsVoidPtr(capsule);
#endif
	copy_tv(v, tv);
    }
    ආපසු 0;
}

    static පූර්ණ_සංඛ්‍යා
ConvertFromPyMapping(PyObject *obj, typval_T *tv)
{
    PyObject	*lookup_dict;
    පූර්ණ_සංඛ්‍යා		ret;

    නම් (!(lookup_dict = PyDict_New()))
	ආපසු -1;

    නම් (PyType_IsSubtype(obj->ob_type, DictionaryTypePtr))
    {
	tv->v_type = VAR_DICT;
	tv->vval.v_dict = (((DictionaryObject *)(obj))->dict);
	++tv->vval.v_dict->dv_refcount;
	ret = 0;
    }
    නැතහොත් නම් (PyDict_Check(obj))
	ret = convert_dl(obj, tv, pydict_to_tv, lookup_dict);
    නැතහොත් නම් (PyMapping_Check(obj))
	ret = convert_dl(obj, tv, pymap_to_tv, lookup_dict);
    නැතහොත්
    {
	PyErr_FORMAT_TYPE(
		N_("unable to convert %s to a Vim dictionary"),
		obj);
	ret = -1;
    }
    Py_DECREF(lookup_dict);
    ආපසු ret;
}

    static පූර්ණ_සංඛ්‍යා
ConvertFromPySequence(PyObject *obj, typval_T *tv)
{
    PyObject	*lookup_dict;
    පූර්ණ_සංඛ්‍යා		ret;

    නම් (!(lookup_dict = PyDict_New()))
	ආපසු -1;

    නම් (PyType_IsSubtype(obj->ob_type, ListTypePtr))
    {
	tv->v_type = VAR_LIST;
	tv->vval.v_list = (((ListObject *)(obj))->list);
	++tv->vval.v_list->lv_refcount;
	ret = 0;
    }
    නැතහොත් නම් (PyIter_Check(obj) || PySequence_Check(obj))
	ret = convert_dl(obj, tv, pyseq_to_tv, lookup_dict);
    නැතහොත්
    {
	PyErr_FORMAT_TYPE(
		N_("unable to convert %s to a Vim list"),
		obj);
	ret = -1;
    }
    Py_DECREF(lookup_dict);
    ආපසු ret;
}

    static පූර්ණ_සංඛ්‍යා
ConvertFromPyObject(PyObject *obj, typval_T *tv)
{
    PyObject	*lookup_dict;
    පූර්ණ_සංඛ්‍යා		ret;

    නම් (!(lookup_dict = PyDict_New()))
	ආපසු -1;
    ret = _ConvertFromPyObject(obj, tv, lookup_dict);
    Py_DECREF(lookup_dict);
    ආපසු ret;
}

    static පූර්ණ_සංඛ්‍යා
_ConvertFromPyObject(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    නම් (PyType_IsSubtype(obj->ob_type, DictionaryTypePtr))
    {
	tv->v_type = VAR_DICT;
	tv->vval.v_dict = (((DictionaryObject *)(obj))->dict);
	++tv->vval.v_dict->dv_refcount;
    }
    නැතහොත් නම් (PyType_IsSubtype(obj->ob_type, ListTypePtr))
    {
	tv->v_type = VAR_LIST;
	tv->vval.v_list = (((ListObject *)(obj))->list);
	++tv->vval.v_list->lv_refcount;
    }
    නැතහොත් නම් (PyType_IsSubtype(obj->ob_type, TupleTypePtr))
    {
	tv->v_type = VAR_TUPLE;
	tv->vval.v_tuple = (((TupleObject *)(obj))->tuple);
	++tv->vval.v_tuple->tv_refcount;
    }
    නැතහොත් නම් (PyType_IsSubtype(obj->ob_type, FunctionTypePtr))
    {
	FunctionObject *func = (FunctionObject *) obj;
	නම් (func->self != NULL || func->argv != NULL)
	{
	    partial_T *pt = ALLOC_CLEAR_ONE(partial_T);

	    set_partial(func, pt, TRUE);
	    tv->vval.v_partial = pt;
	    tv->v_type = VAR_PARTIAL;
	}
	නැතහොත්
	{
	    නම් (set_string_copy(func->name, tv) == -1)
		ආපසු -1;

	    tv->v_type = VAR_FUNC;
	}
	func_ref(func->name);
    }
    නැතහොත් නම් (PyBytes_Check(obj))
    {
	char_u	*str;

	නම් (PyBytes_AsStringAndSize(obj, (var_na **) &str, NULL) == -1)
	    ආපසු -1;
	නම් (str == NULL)
	    ආපසු -1;

	නම් (set_string_copy(str, tv) == -1)
	    ආපසු -1;

	tv->v_type = VAR_STRING;
    }
    නැතහොත් නම් (PyUnicode_Check(obj))
    {
	PyObject	*bytes;
	char_u	*str;

	bytes = PyUnicode_AsEncodedString(obj, ENC_OPT, ERRORS_ENCODE_ARG);
	නම් (bytes == NULL)
	    ආපසු -1;

	නම් (PyBytes_AsStringAndSize(bytes, (var_na **) &str, NULL) == -1)
	    ආපසු -1;
	නම් (str == NULL)
	    ආපසු -1;

	නම් (set_string_copy(str, tv))
	{
	    Py_XDECREF(bytes);
	    ආපසු -1;
	}
	Py_XDECREF(bytes);

	tv->v_type = VAR_STRING;
    }
#නම් PY_MAJOR_VERSION < 3
    නැතහොත් නම් (PyInt_Check(obj))
    {
	tv->v_type = VAR_NUMBER;
	tv->vval.v_number = (varnumber_T) PyInt_AsLong(obj);
	නම් (PyErr_Occurred())
	    ආපසු -1;
    }
#endif
    නැතහොත් නම් (PyLong_Check(obj))
    {
	tv->v_type = VAR_NUMBER;
	tv->vval.v_number = (varnumber_T) PyLong_AsLong(obj);
	නම් (PyErr_Occurred())
	    ආපසු -1;
    }
    නැතහොත් නම් (PyTuple_Check(obj))
	ආපසු convert_dl(obj, tv, pytuple_to_tv, lookup_dict);
    නැතහොත් නම් (PyDict_Check(obj))
	ආපසු convert_dl(obj, tv, pydict_to_tv, lookup_dict);
    නැතහොත් නම් (PyFloat_Check(obj))
    {
	tv->v_type = VAR_FLOAT;
	tv->vval.v_float = (float_T) PyFloat_AsDouble(obj);
    }
    නැතහොත් නම් (PyObject_HasAttrString(obj, "keys"))
	ආපසු convert_dl(obj, tv, pymap_to_tv, lookup_dict);
    // PyObject_GetIter can create built-in iterator for any sequence object
    නැතහොත් නම් (PyIter_Check(obj) || PySequence_Check(obj))
	ආපසු convert_dl(obj, tv, pyseq_to_tv, lookup_dict);
    නැතහොත් නම් (PyMapping_Check(obj))
	ආපසු convert_dl(obj, tv, pymap_to_tv, lookup_dict);
    නැතහොත් නම් (PyNumber_Check(obj))
    {
	PyObject	*num;

	නම් (!(num = PyNumber_Long(obj)))
	    ආපසු -1;

	tv->v_type = VAR_NUMBER;
	tv->vval.v_number = (varnumber_T) PyLong_AsLong(num);

	Py_DECREF(num);
    }
    නැතහොත් නම් (obj == Py_None)
    {
	tv->v_type = VAR_SPECIAL;
	tv->vval.v_number = VVAL_NONE;
    }
    නැතහොත්
    {
	PyErr_FORMAT_TYPE(
		N_("unable to convert %s to a Vim structure"),
		obj);
	ආපසු -1;
    }
    ආපසු 0;
}

    static PyObject *
ConvertToPyObject(typval_T *tv)
{
    typval_T *argv;
    පූර්ණ_සංඛ්‍යා i;
    නම් (tv == NULL)
    {
	PyErr_SET_VIM(N_("internal error: NULL reference passed"));
	ආපසු NULL;
    }
    switch (tv->v_type)
    {
	pak_sa VAR_STRING:
	    ආපසු PyBytes_FromString(tv->vval.v_string == NULL
					    ? "" : (var_na *)tv->vval.v_string);
	pak_sa VAR_NUMBER:
	    ආපසු PyLong_FromLong((long) tv->vval.v_number);
	pak_sa VAR_FLOAT:
	    ආපසු PyFloat_FromDouble((dviguu_na) tv->vval.v_float);
	pak_sa VAR_LIST:
	    ආපසු NEW_LIST(tv->vval.v_list);
	pak_sa VAR_TUPLE:
	    ආපසු NEW_TUPLE(tv->vval.v_tuple);
	pak_sa VAR_DICT:
	    ආපසු NEW_DICTIONARY(tv->vval.v_dict);
	pak_sa VAR_FUNC:
	    ආපසු NEW_FUNCTION(tv->vval.v_string == NULL
					  ? (char_u *)"" : tv->vval.v_string,
					  0, NULL, NULL, TRUE);
	pak_sa VAR_PARTIAL:
	    නම් (tv->vval.v_partial->pt_argc)
	    {
		argv = PyMem_New(typval_T, (size_t)tv->vval.v_partial->pt_argc);
		සඳහා (i = 0; i < tv->vval.v_partial->pt_argc; i++)
		    copy_tv(&tv->vval.v_partial->pt_argv[i], &argv[i]);
	    }
	    නැතහොත්
		argv = NULL;
	    නම් (tv->vval.v_partial->pt_dict != NULL)
		tv->vval.v_partial->pt_dict->dv_refcount++;
	    ආපසු NEW_FUNCTION(tv->vval.v_partial == NULL
			     ? (char_u *)"" : partial_name(tv->vval.v_partial),
				tv->vval.v_partial->pt_argc, argv,
				tv->vval.v_partial->pt_dict,
				tv->vval.v_partial->pt_auto);
	pak_sa VAR_BLOB:
	    ආපසු PyBytes_FromStringAndSize(
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
	    ආපසු Py_None;
	pak_sa VAR_BOOL:
	pak_sa VAR_SPECIAL:
	    switch (tv->vval.v_number)
	    {
		pak_sa VVAL_FALSE: ආපසු ALWAYS_FALSE;
		pak_sa VVAL_TRUE:  ආපසු ALWAYS_TRUE;
		pak_sa VVAL_NONE:
		pak_sa VVAL_NULL:  ආපසු ALWAYS_NONE;
	    }
	    PyErr_SET_VIM(N_("internal error: invalid value type"));
	    ආපසු NULL;
    }
    ආපසු NULL;
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
#නම් PY_MAJOR_VERSION >= 3
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
#නැතහොත්
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
#නම් PY_MAJOR_VERSION >= 3
    BufferType.tp_getattro = BufferGetattro;
    BufferType.tp_setattro = BufferSetattro;
    BufferType.tp_alloc = call_PyType_GenericAlloc;
    BufferType.tp_new = call_PyType_GenericNew;
    BufferType.tp_free = call_PyObject_Free;
#නැතහොත්
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
#නම් PY_MAJOR_VERSION >= 3
    WindowType.tp_getattro = WindowGetattro;
    WindowType.tp_setattro = WindowSetattro;
    WindowType.tp_alloc = call_PyType_GenericAlloc;
    WindowType.tp_new = call_PyType_GenericNew;
    WindowType.tp_free = call_PyObject_Free;
#නැතහොත්
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
#නම් PY_MAJOR_VERSION >= 3
    TabPageType.tp_getattro = TabPageGetattro;
    TabPageType.tp_alloc = call_PyType_GenericAlloc;
    TabPageType.tp_new = call_PyType_GenericNew;
    TabPageType.tp_free = call_PyObject_Free;
#නැතහොත්
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
#නම් PY_MAJOR_VERSION >= 3
    RangeType.tp_getattro = RangeGetattro;
    RangeType.tp_alloc = call_PyType_GenericAlloc;
    RangeType.tp_new = call_PyType_GenericNew;
    RangeType.tp_free = call_PyObject_Free;
#නැතහොත්
    RangeType.tp_getattr = RangeGetattr;
#endif

    CLEAR_FIELD(CurrentType);
    CurrentType.tp_name = "vim.currentdata";
    CurrentType.tp_basicsize = parimaa_na(CurrentObject);
    CurrentType.tp_flags = Py_TPFLAGS_DEFAULT;
    CurrentType.tp_doc = "vim current object";
    CurrentType.tp_methods = CurrentMethods;
#නම් PY_MAJOR_VERSION >= 3
    CurrentType.tp_getattro = CurrentGetattro;
    CurrentType.tp_setattro = CurrentSetattro;
#නැතහොත්
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
#නම් PY_MAJOR_VERSION >= 3
    DictionaryType.tp_getattro = DictionaryGetattro;
    DictionaryType.tp_setattro = DictionarySetattro;
#නැතහොත්
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
#නම් PY_MAJOR_VERSION >= 3
    ListType.tp_getattro = ListGetattro;
    ListType.tp_setattro = ListSetattro;
#නැතහොත්
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
#නම් PY_MAJOR_VERSION >= 3
    TupleType.tp_getattro = TupleGetattro;
    TupleType.tp_setattro = TupleSetattro;
#නැතහොත්
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
#නම් PY_MAJOR_VERSION >= 3
    FunctionType.tp_getattro = FunctionGetattro;
#නැතහොත්
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

#නම් PY_VERSION_HEX < 0x030700f0
    CLEAR_FIELD(LoaderType);
    LoaderType.tp_name = "vim.Loader";
    LoaderType.tp_basicsize = parimaa_na(LoaderObject);
    LoaderType.tp_flags = Py_TPFLAGS_DEFAULT;
    LoaderType.tp_doc = "vim message object";
    LoaderType.tp_methods = LoaderMethods;
    LoaderType.tp_dealloc = LoaderDestructor;
#endif

#නම් PY_MAJOR_VERSION >= 3
    CLEAR_FIELD(vimmodule);
    vimmodule.m_name = "vim";
    vimmodule.m_doc = "Vim Python interface\n";
    vimmodule.m_size = -1;
    vimmodule.m_methods = VimMethods;
#endif
}

    static පූර්ණ_සංඛ්‍යා
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
#නම් PY_VERSION_HEX < 0x030700f0
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
    ආපසු 0;
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
# නම් PY_VERSION_HEX < 0x030700f0
    PYTYPE_CLEANUP(LoaderType);
# endif
}
#endif

    static පූර්ණ_සංඛ්‍යා
init_sys_path(void)
{
    PyObject	*path;
    PyObject	*path_hook;
    PyObject	*path_hooks;

    නම් (!(path_hook = PyObject_GetAttrString(vim_module, "path_hook")))
	ආපසු -1;

    නම් (!(path_hooks = PySys_GetObject("path_hooks")))
    {
	PyErr_Clear();
	path_hooks = PyList_New(1);
	PyList_SET_ITEM(path_hooks, 0, path_hook);
	නම් (PySys_SetObject("path_hooks", path_hooks))
	{
	    Py_DECREF(path_hooks);
	    ආපසු -1;
	}
	Py_DECREF(path_hooks);
    }
    නැතහොත් නම් (PyList_Check(path_hooks))
    {
	නම් (PyList_Append(path_hooks, path_hook))
	{
	    Py_DECREF(path_hook);
	    ආපසු -1;
	}
	Py_DECREF(path_hook);
    }
    නැතහොත්
    {
	VimTryStart();
	emsg(_("Failed to set path hook: sys.path_hooks is not a list\n"
	       "You should now do the following:\n"
	       "- append vim.path_hook to sys.path_hooks\n"
	       "- append vim.VIM_SPECIAL_PATH to sys.path\n"));
	VimTryEnd(); // Discard the error
	Py_DECREF(path_hook);
	ආපසු 0;
    }

    නම් (!(path = PySys_GetObject("path")))
    {
	PyErr_Clear();
	path = PyList_New(1);
	Py_INCREF(vim_special_path_object);
	PyList_SET_ITEM(path, 0, vim_special_path_object);
	නම් (PySys_SetObject("path", path))
	{
	    Py_DECREF(path);
	    ආපසු -1;
	}
	Py_DECREF(path);
    }
    නැතහොත් නම් (PyList_Check(path))
    {
	නම් (PyList_Append(path, vim_special_path_object))
	    ආපසු -1;
    }
    නැතහොත්
    {
	VimTryStart();
	emsg(_("Failed to set path: sys.path is not a list\n"
	       "You should now append vim.VIM_SPECIAL_PATH to sys.path"));
	VimTryEnd(); // Discard the error
    }

    ආපසු 0;
}

static sa_nracanaa numeric_constant {
    var_na	*name;
    පූර්ණ_සංඛ්‍යා		val;
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
    නම් (PyModule_AddObject(m, name, obj)) \
	ආපසු -1;

#define ADD_CHECKED_OBJECT(m, name, obj) \
    { \
	PyObject	*valObject = obj; \
	නම් (!valObject) \
	    ආපසු -1; \
	ADD_OBJECT(m, name, valObject); \
    }

    static පූර්ණ_සංඛ්‍යා
populate_module(PyObject *m)
{
    පූර්ණ_සංඛ්‍යා		i;
    PyObject	*other_module;
    PyObject	*attr;
    PyObject	*imp;
#නම් PY_VERSION_HEX >= 0x030700f0
    PyObject	*dict;
    PyObject	*cls;
#endif

    සඳහා (i = 0; i < (පූර්ණ_සංඛ්‍යා)(parimaa_na(numeric_constants)
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
#නම් PY_VERSION_HEX < 0x030700f0
	{"_Loader",    (PyObject *)LoaderTypePtr},
#endif
    };

    සඳහා (i = 0; i < (පූර්ණ_සංඛ්‍යා)(parimaa_na(object_constants)
					    / parimaa_na(sa_nracanaa object_constant));
	    ++i)
    {
	PyObject	*valObject;

	valObject = object_constants[i].valObject;
	Py_INCREF(valObject);
	ADD_OBJECT(m, object_constants[i].name, valObject);
    }

    නම් (!(VimError = PyErr_NewException("vim.error", NULL, NULL)))
	ආපසු -1;
    ADD_OBJECT(m, "error", VimError);

    ADD_CHECKED_OBJECT(m, "vars",  NEW_DICTIONARY(get_globvar_dict()));
    ADD_CHECKED_OBJECT(m, "vvars", NEW_DICTIONARY(get_vimvar_dict()));
    ADD_CHECKED_OBJECT(m, "options",
	    OptionsNew(SREQ_GLOBAL, NULL, dummy_check, NULL));

    නම් (!(other_module = PyImport_ImportModule("os")))
	ආපසු -1;
    ADD_OBJECT(m, "os", other_module);

#නම් PY_MAJOR_VERSION >= 3
    නම් (!(py_getcwd = PyObject_GetAttrString(other_module, "getcwd")))
	ආපසු -1;
#නැතහොත්
    නම් (!(py_getcwd = PyObject_GetAttrString(other_module, "getcwdu")))
	ආපසු -1;
#endif
    ADD_OBJECT(m, "_getcwd", py_getcwd)

    නම් (!(py_chdir = PyObject_GetAttrString(other_module, "chdir")))
	ආපසු -1;
    ADD_OBJECT(m, "_chdir", py_chdir);
    නම් (!(attr = PyObject_GetAttrString(m, "chdir")))
	ආපසු -1;
    නම් (PyObject_SetAttrString(other_module, "chdir", attr))
    {
	Py_DECREF(attr);
	ආපසු -1;
    }
    Py_DECREF(attr);

    නම් ((py_fchdir = PyObject_GetAttrString(other_module, "fchdir")))
    {
	ADD_OBJECT(m, "_fchdir", py_fchdir);
	නම් (!(attr = PyObject_GetAttrString(m, "fchdir")))
	    ආපසු -1;
	නම් (PyObject_SetAttrString(other_module, "fchdir", attr))
	{
	    Py_DECREF(attr);
	    ආපසු -1;
	}
	Py_DECREF(attr);
    }
    නැතහොත්
	PyErr_Clear();

    නම් (!(vim_special_path_object = PyString_FromString(vim_special_path)))
	ආපසු -1;

    ADD_OBJECT(m, "VIM_SPECIAL_PATH", vim_special_path_object);

#නම් PY_VERSION_HEX >= 0x030700f0
    නම් (!(imp = PyImport_ImportModule("importlib.machinery")))
	ආපසු -1;

    dict = PyModule_GetDict(imp);

    නම් (!(cls = PyDict_GetItemString(dict, "PathFinder")))
    {
	Py_DECREF(imp);
	ආපසු -1;
    }

    නම් (!(py_find_spec = PyObject_GetAttrString(cls, "find_spec")))
    {
	Py_DECREF(imp);
	ආපසු -1;
    }

# නම් PY_VERSION_HEX < 0x30c00a7
    // find_module has been removed as of Python 3.12.0a7
    නම් ((py_find_module = PyObject_GetAttrString(cls, "find_module")))
	ADD_OBJECT(m, "_find_module", py_find_module);
# endif

    Py_DECREF(imp);

    ADD_OBJECT(m, "_find_spec", py_find_spec);
#නැතහොත්
    නම් (!(imp = PyImport_ImportModule("imp")))
	ආපසු -1;

    නම් (!(py_find_module = PyObject_GetAttrString(imp, "find_module")))
    {
	Py_DECREF(imp);
	ආපසු -1;
    }

    නම් (!(py_load_module = PyObject_GetAttrString(imp, "load_module")))
    {
	Py_DECREF(py_find_module);
	Py_DECREF(imp);
	ආපසු -1;
    }

    Py_DECREF(imp);

    ADD_OBJECT(m, "_find_module", py_find_module);
    ADD_OBJECT(m, "_load_module", py_load_module);
#endif

    ආපසු 0;
}
