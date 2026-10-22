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

#もし PY_VERSION_HEX < 0x02050000
typedef 整数 Py_ssize_t;  // Python 2.4 and earlier don't have this type.
#endif

// Use values that are known to work, others may make Vim crash.
#define ENC_OPT (enc_utf8 ? "utf-8" : enc_dbcs ? "euc-jp" : (char *)p_enc)
#define DOPY_FUNC "_vim_pydo"

static const char *vim_special_path = "_vim_path_";

#define PyErr_SET_STRING(exc, str) PyErr_SetString(exc, _(str))
#define PyErr_SetVim(str) PyErr_SetString(VimError, str)
#define PyErr_SET_VIM(str) PyErr_SET_STRING(VimError, str)
#define PyErr_FORMAT(exc, str, arg) PyErr_Format(exc, _(str), arg)
#define PyErr_FORMAT2(exc, str, arg1, arg2) PyErr_Format(exc, _(str), arg1,arg2)
#define PyErr_VIM_FORMAT(str, arg) PyErr_FORMAT(VimError, str, arg)

#ifdef USE_LIMITED_API
// Limited Python API. Need to call only exposed functions and remap macros.
// PyTypeObject is an opaque struct.

typedef struct {
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

typedef struct {
    lenfunc mp_length;
    binaryfunc mp_subscript;
    objobjargproc mp_ass_subscript;
} PyMappingMethods;

// This struct emulates the concrete _typeobject struct to allow the code to
// work the same way in both limited and full Python APIs.
struct typeobject_wrapper {
    const char *tp_name;
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

    const char *tp_doc;

    traverseproc tp_traverse;

    inquiry tp_clear;

    getiterfunc tp_iter;
    iternextfunc tp_iternext;

    struct PyMethodDef *tp_methods;
    struct _typeobject *tp_base;
    allocfunc tp_alloc;
    newfunc tp_new;
    freefunc tp_free;
};

# define DEFINE_PY_TYPE_OBJECT(type) \
    static struct typeobject_wrapper type; \
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
    もし (obj == NULL)
	戻る PyErr_NoMemory();
    戻る PyObject_Init(obj, type);
}
# undef PyObject_NEW
# define PyObject_NEW(type, typeobj) ((type *)Vim_PyObject_New(typeobj, sizeof(type)))

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
	もし (qualname == NULL) \
	{ \
	    PyErr_FORMAT(PyExc_TypeError, msg, "(NULL)"); \
	    break; \
	} \
	PyObject* module = PyObject_GetAttrString((PyObject*)(obj)->ob_type, "__module__"); \
	PyObject* full; \
	もし (module == NULL || PyUnicode_CompareWithASCIIString(module, "builtins") == 0 \
		|| PyUnicode_CompareWithASCIIString(module, "__main__") == 0) \
	{ \
	    full = qualname; \
	    Py_INCREF(full); \
	} \
	否则 \
	    full = PyUnicode_FromFormat("%U.%U", module, qualname); \
	PyObject* full_bytes = PyUnicode_AsUTF8String(full); \
	const char* full_str = PyBytes_AsString(full_bytes); \
	full_str = full_str == NULL ? "(NULL)" : full_str; \
	PyErr_FORMAT(PyExc_TypeError, msg, full_str); \
	Py_DECREF(qualname); \
	Py_XDECREF(module); \
	Py_XDECREF(full); \
	Py_XDECREF(full_bytes); \
    } ながら (0)

# define PyList_GET_ITEM(list, i) PyList_GetItem(list, i)
# define PyList_GET_SIZE(o) PyList_Size(o)

// PyList_SET_ITEM and PyList_SetItem have slightly different behaviors. The
// former will leave the old item dangling, and the latter will decref on it.
// Since we only use this on new lists, this difference doesn't matter.
# define PyList_SET_ITEM(list, i, item) PyList_SetItem(list, i, item)

# define PyTuple_GET_ITEM(o, pos) PyTuple_GetItem(o, pos)
# define PyTuple_GET_SIZE(o) PyTuple_Size(o)

# もし Py_LIMITED_API < 0x03080000
// PyIter_check only became part of stable ABI in 3.8, and there is no easy way
// to check for it in the API. We simply return false as a compromise. This
// does mean we should avoid compiling with stable ABI < 3.8.
#  undef PyIter_Check
#  define PyIter_Check(obj) (FALSE)
# endif

static PyTypeObject* AddHeapType(struct typeobject_wrapper* type_object)
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
    もし (slot_i >= 40) 戻る NULL; /* this should never happen */ \
    もし (type_object->slot_name != NULL) \
    { \
	slots[slot_i].slot = Py_##slot_name; \
	slots[slot_i].pfunc = (void*)type_object->slot_name; \
	++slot_i; \
    }
# define ADD_TP_SUB_SLOT(sub_slot, slot_name) \
    もし (slot_i >= 40) 戻る NULL; /* this should never happen */ \
    もし (type_object->sub_slot != NULL && type_object->sub_slot->slot_name != NULL) \
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
    戻る (PyTypeObject*)newtype;
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
    もし (type##Ptr == NULL) \
	戻る -1;
# define PYTYPE_CLEANUP(type) \
    type##Ptr = NULL;

// Limited API does not provide PyRun_* functions. Need to implement manually
// using PyCompile and PyEval.
static PyObject* Vim_PyRun_String(const char *str, 整数 start, PyObject *globals, PyObject *locals)
{
    // Just pass "" for filename for now.
    PyObject* compiled = Py_CompileString(str, "", start);
    もし (compiled == NULL)
	戻る NULL;

    PyObject* eval_result = PyEval_EvalCode(compiled, globals, locals);
    Py_DECREF(compiled);
    戻る eval_result;
}
static 整数 Vim_PyRun_SimpleString(const char *str)
{
    // This function emulates CPython's implementation.
    PyObject* m = PyImport_AddModule("__main__");
    もし (m == NULL)
	戻る -1;
    PyObject* d = PyModule_GetDict(m);
    PyObject* output = Vim_PyRun_String(str, Py_file_input, d, d);
    もし (output == NULL)
    {
	PyErr_PrintEx(TRUE);
	戻る -1;
    }
    Py_DECREF(output);
    戻る 0;
}
# define PyRun_String Vim_PyRun_String
# define PyRun_SimpleString Vim_PyRun_SimpleString

#否则 // !defined(USE_LIMITED_API)

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
    もし (PyType_Ready(type##Ptr)) \
	戻る -1;

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
typedef void (*runner)(const char *, dict_T *, void *
#ifdef PY_CAN_RECURSE
	, PyGILState_STATE *
#endif
	);

static 整数 ConvertFromPyObject(PyObject *, typval_T *);
static 整数 _ConvertFromPyObject(PyObject *, typval_T *, PyObject *);
static 整数 ConvertFromPyMapping(PyObject *, typval_T *);
static 整数 ConvertFromPySequence(PyObject *, typval_T *);
static PyObject *WindowNew(win_T *, tabpage_T *);
static PyObject *BufferNew (buf_T *);
static PyObject *LineToString(const char *);

static PyInt RangeStart;
static PyInt RangeEnd;

static PyObject *globals;

static PyObject *py_chdir;
static PyObject *py_fchdir;
static PyObject *py_getcwd;
static PyObject *vim_module;
static PyObject *vim_special_path_object;

#もし PY_VERSION_HEX >= 0x030700f0
static PyObject *py_find_spec;
#否则
static PyObject *py_load_module;
#endif
#もし PY_VERSION_HEX < 0x30c00a7
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

    もし (PyBytes_Check(obj))
    {

	もし (PyBytes_AsStringAndSize(obj, (char **) &str, NULL) == -1
		|| str == NULL)
	    戻る NULL;

	*todecref = NULL;
    }
    否则 もし (PyUnicode_Check(obj))
    {
	PyObject	*bytes;

	もし (!(bytes = PyUnicode_AsEncodedString(obj, ENC_OPT,
							   ERRORS_ENCODE_ARG)))
	    戻る NULL;

	もし (PyBytes_AsStringAndSize(bytes, (char **) &str, NULL) == -1
		|| str == NULL)
	{
	    Py_DECREF(bytes);
	    戻る NULL;
	}

	*todecref = bytes;
    }
    否则
    {
#もし PY_MAJOR_VERSION < 3
	PyErr_FORMAT_TYPE(N_("expected str() or unicode() instance, but got %s"),
		obj);
#否则
	PyErr_FORMAT_TYPE(N_("expected bytes() or str() instance, but got %s"),
		obj);
#endif
	戻る NULL;
    }

    戻る (char_u *) str;
}

#define NUMBER_LONG     1
#define NUMBER_INT      2
#define NUMBER_NATURAL  4
#define NUMBER_UNSIGNED 8

    static 整数
NumberToLong(PyObject *obj, long *result, 整数 flags)
{
#もし PY_MAJOR_VERSION < 3
    もし (PyInt_Check(obj))
    {
	*result = PyInt_AsLong(obj);
	もし (PyErr_Occurred())
	    戻る -1;
    }
    否则
#endif
    もし (PyLong_Check(obj))
    {
	*result = PyLong_AsLong(obj);
	もし (PyErr_Occurred())
	    戻る -1;
    }
    否则 もし (PyNumber_Check(obj))
    {
	PyObject	*num;

	もし (!(num = PyNumber_Long(obj)))
	    戻る -1;

	*result = PyLong_AsLong(num);

	Py_DECREF(num);

	もし (PyErr_Occurred())
	    戻る -1;
    }
    否则
    {
#もし PY_MAJOR_VERSION < 3
	PyErr_FORMAT_TYPE(
		N_("expected int(), long() or something supporting "
		   "coercing to long(), but got %s"),
		obj);
#否则
	PyErr_FORMAT_TYPE(
		N_("expected int() or something supporting coercing to int(), "
		   "but got %s"),
		obj);
#endif
	戻る -1;
    }

    もし (flags & NUMBER_INT)
    {
	もし (*result > INT_MAX)
	{
	    PyErr_SET_STRING(PyExc_OverflowError,
		    N_("value is too large to fit into C int type"));
	    戻る -1;
	}
	否则 もし (*result < INT_MIN)
	{
	    PyErr_SET_STRING(PyExc_OverflowError,
		    N_("value is too small to fit into C int type"));
	    戻る -1;
	}
    }

    もし (flags & NUMBER_NATURAL)
    {
	もし (*result <= 0)
	{
	    PyErr_SET_STRING(PyExc_ValueError,
		    N_("number must be greater than zero"));
	    戻る -1;
	}
    }
    否则 もし (flags & NUMBER_UNSIGNED)
    {
	もし (*result < 0)
	{
	    PyErr_SET_STRING(PyExc_ValueError,
		    N_("number must be greater or equal to zero"));
	    戻る -1;
	}
    }

    戻る 0;
}

    static 整数
add_string(PyObject *list, char *s)
{
    PyObject	*string;

    もし (!(string = PyString_FromString(s)))
	戻る -1;

    もし (PyList_Append(list, string))
    {
	Py_DECREF(string);
	戻る -1;
    }

    Py_DECREF(string);
    戻る 0;
}

    static PyObject *
ObjectDir(PyObject *self, char **attributes)
{
    PyMethodDef	*method;
    char	**attr;
    PyObject	*ret;

    もし (!(ret = PyList_New(0)))
	戻る NULL;

    もし (self)
	ために (method = Py_TYPE_GET_TP_METHODS(self->ob_type) ; method->ml_name != NULL ; ++method)
	    もし (add_string(ret, (char *)method->ml_name))
	    {
		Py_DECREF(ret);
		戻る NULL;
	    }

    ために (attr = attributes ; *attr ; ++attr)
	もし (add_string(ret, *attr))
	{
	    Py_DECREF(ret);
	    戻る NULL;
	}

#もし PY_MAJOR_VERSION < 3
    もし (add_string(ret, "__members__"))
    {
	Py_DECREF(ret);
	戻る NULL;
    }
#endif

    戻る ret;
}

// Output buffer management

// Function to write a line, points to either msg() or emsg().
typedef 整数 (*writefn)(char *);

DEFINE_PY_TYPE_OBJECT(OutputType);

typedef struct
{
    PyObject_HEAD
    long softspace;
    long error;
} OutputObject;

static char *OutputAttrs[] = {
    "softspace",
    NULL
};

    static PyObject *
OutputDir(PyObject *self, PyObject *args UNUSED)
{
    戻る ObjectDir(self, OutputAttrs);
}

    static 整数
OutputSetattr(PyObject *self_obj, char *name, PyObject *valObject)
{
    OutputObject *self = (OutputObject*)self_obj;
    もし (valObject == NULL)
    {
	PyErr_SET_STRING(PyExc_AttributeError,
		N_("can't delete OutputObject attributes"));
	戻る -1;
    }

    もし (strcmp(name, "softspace") == 0)
    {
	もし (NumberToLong(valObject, &(self->softspace), NUMBER_UNSIGNED))
	    戻る -1;
	戻る 0;
    }

    PyErr_FORMAT(PyExc_AttributeError, N_("invalid attribute: %s"), name);
    戻る -1;
}

// Buffer IO, we write one whole line at a time.
static garray_T io_ga = {0, 0, 1, 80, NULL};
static writefn old_fn = NULL;

    static void
PythonIO_Flush(void)
{
    もし (old_fn != NULL && io_ga.ga_len > 0)
    {
	((char *)io_ga.ga_data)[io_ga.ga_len] = NUL;
	// We don't know what emsg_severe should be here, so ... hope?
	old_fn((char *)io_ga.ga_data);
    }
    io_ga.ga_len = 0;
}

    static void
writer(writefn fn, char_u *str, PyInt n, 整数 severe)
{
    char_u *ptr;

    // Flush when switching output function.
    もし (fn != old_fn)
	PythonIO_Flush();
    old_fn = fn;

    // Write each NL separated line.  Text after the last NL is kept for
    // writing later.
    // For normal messages: Do not output when "got_int" was set.  This avoids
    // a loop gone crazy flooding the terminal with messages.  Also for when
    // "q" is pressed at the more-prompt.
    ながら (n > 0 && (ptr = memchr(str, '\n', n)) != NULL
					  && (fn == (writefn)emsg || !got_int))
    {
	PyInt len = ptr - str;

	もし (ga_grow(&io_ga, (整数)(len + 1)) == FAIL)
	    break;

	mch_memmove(((char *)io_ga.ga_data) + io_ga.ga_len, str, (size_t)len);
	((char *)io_ga.ga_data)[io_ga.ga_len + len] = NUL;
	emsg_severe = severe;
	fn((char *)io_ga.ga_data);
	str = ptr + 1;
	n -= len + 1;
	io_ga.ga_len = 0;
    }

    // Put the remaining text into io_ga for later printing.
    もし (n > 0 && (fn == (writefn)emsg || !got_int)
					&& ga_grow(&io_ga, (整数)(n + 1)) == OK)
    {
	mch_memmove(((char *)io_ga.ga_data) + io_ga.ga_len, str, (size_t)n);
	io_ga.ga_len += (整数)n;
    }
}

    static 整数
write_output(OutputObject *self, PyObject *string)
{
    Py_ssize_t	len = 0;
    char	*str = NULL;
    整数		error = self->error;

    もし (!PyArg_Parse(string, "et#", ENC_OPT, &str, &len))
	戻る -1;

    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    writer((writefn)(error ? emsg : msg), (char_u *)str, len, error);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS
    PyMem_Free(str);

    戻る 0;
}

    static PyObject *
OutputWrite(OutputObject *self, PyObject *string)
{
    もし (write_output(self, string))
	戻る NULL;

    Py_INCREF(Py_None);
    戻る Py_None;
}

    static PyObject *
OutputWritelines(OutputObject *self, PyObject *seq)
{
    PyObject	*iterator;
    PyObject	*item;

    もし (!(iterator = PyObject_GetIter(seq)))
	戻る NULL;

    ながら ((item = PyIter_Next(iterator)))
    {
	もし (write_output(self, item))
	{
	    Py_DECREF(iterator);
	    Py_DECREF(item);
	    戻る NULL;
	}
	Py_DECREF(item);
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    もし (PyErr_Occurred())
	戻る NULL;

    Py_INCREF(Py_None);
    戻る Py_None;
}

    static PyObject *
AlwaysNone(PyObject *self UNUSED, PyObject *args UNUSED)
{
    // do nothing
    Py_INCREF(Py_None);
    戻る Py_None;
}
#define ALWAYS_NONE AlwaysNone(NULL, NULL)

    static PyObject *
AlwaysFalse(PyObject *self UNUSED, PyObject *args UNUSED)
{
    // do nothing
    PyObject	*ret = Py_False;
    Py_INCREF(ret);
    戻る ret;
}
#define ALWAYS_FALSE AlwaysFalse(NULL, NULL)

    static PyObject *
AlwaysTrue(PyObject *self UNUSED, PyObject *args UNUSED)
{
    // do nothing
    PyObject	*ret = Py_True;
    Py_INCREF(ret);
    戻る ret;
}
#define ALWAYS_TRUE AlwaysTrue(NULL, NULL)

/***************/

static struct PyMethodDef OutputMethods[] = {
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

    static 整数
PythonIO_Init_io(void)
{
    もし (PySys_SetObject("stdout", (PyObject *)(void *)&Output))
	戻る -1;
    もし (PySys_SetObject("stderr", (PyObject *)(void *)&Error))
	戻る -1;

    もし (PyErr_Occurred())
    {
	emsg(_(e_python_error_initialising_io_object));
	戻る -1;
    }

    戻る 0;
}

#もし PY_VERSION_HEX < 0x030700f0
static PyObject *call_load_module(char *name, 整数 len, PyObject *find_module_result);

typedef struct
{
    PyObject_HEAD
    char	*fullname;
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
    char	*fullname = self->fullname;
    PyObject	*result = self->result;
    PyObject	*module;

    もし (!fullname)
    {
	module = result ? result : Py_None;
	Py_INCREF(module);
	戻る module;
    }

    module = call_load_module(fullname, (整数)STRLEN(fullname), result);

    self->fullname = NULL;
    self->result = module;

    vim_free(fullname);
    Py_DECREF(result);

    もし (!module)
    {
	もし (PyErr_Occurred())
	    戻る NULL;

	Py_INCREF(Py_None);
	戻る Py_None;
    }

    Py_INCREF(module);
    戻る module;
}

static struct PyMethodDef LoaderMethods[] = {
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

    static 整数
VimTryEnd(void)
{
    --trylevel;
    // Without this it stops processing all subsequent Vim script commands and
    // generates strange error messages if I e.g. try calling Test() in a cycle
    did_emsg = FALSE;
    // Keyboard interrupt should be preferred over anything else
    もし (got_int)
    {
	もし (did_throw)
	    discard_current_exception();
	got_int = FALSE;
	PyErr_SetNone(PyExc_KeyboardInterrupt);
	戻る -1;
    }
    否则 もし (msg_list != NULL && *msg_list != NULL)
    {
	整数	should_free;
	char	*msg;

	msg = get_exception_string(*msg_list, ET_ERROR, NULL, &should_free);

	もし (msg == NULL)
	{
	    PyErr_NoMemory();
	    戻る -1;
	}

	PyErr_SetVim(msg);

	free_global_msglist();

	もし (should_free)
	    vim_free(msg);

	戻る -1;
    }
    否则 もし (!did_throw)
	戻る (PyErr_Occurred() ? -1 : 0);
    // Python exception is preferred over Vim one; unlikely to occur though
    否则 もし (PyErr_Occurred())
    {
	discard_current_exception();
	戻る -1;
    }
    // Finally transform Vim script exception to python one
    否则
    {
	PyErr_SetVim((char *)current_exception->value);
	discard_current_exception();
	戻る -1;
    }
}

    static 整数
VimCheckInterrupt(void)
{
    もし (got_int)
    {
	PyErr_SetNone(PyExc_KeyboardInterrupt);
	戻る 1;
    }
    戻る 0;
}

// Vim module - Implementation

    static PyObject *
VimCommand(PyObject *self UNUSED, PyObject *string)
{
    char_u	*cmd;
    PyObject	*ret;
    PyObject	*todecref;

    もし (!(cmd = StringToChars(string, &todecref)))
	戻る NULL;

    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();

    VimTryStart();
    do_cmdline_cmd(cmd);
    update_screen(UPD_VALID);

    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    もし (VimTryEnd())
	ret = NULL;
    否则
	ret = Py_None;

    Py_XINCREF(ret);
    Py_XDECREF(todecref);
    戻る ret;
}

/*
 * Function to translate a typval_T into a PyObject; this will recursively
 * translate lists/dictionaries into their Python equivalents.
 *
 * The depth parameter is to avoid infinite recursion, set it to 1 when
 * you call VimToPython.
 */
    static PyObject *
VimToPython(typval_T *our_tv, 整数 depth, PyObject *lookup_dict)
{
    PyObject	*ret;
    PyObject	*newObj;
    char	ptrBuf[sizeof(void *) * 2 + 3];

    // Avoid infinite recursion
    もし (depth > 100)
    {
	Py_INCREF(Py_None);
	ret = Py_None;
	戻る ret;
    }

    // Check if we run into a recursive loop.  The item must be in lookup_dict
    // then and we can use it again.
    もし ((our_tv->v_type == VAR_LIST && our_tv->vval.v_list != NULL)
	    || (our_tv->v_type == VAR_TUPLE && our_tv->vval.v_tuple != NULL)
	    || (our_tv->v_type == VAR_DICT && our_tv->vval.v_dict != NULL))
    {
	sprintf(ptrBuf, "%p",
		our_tv->v_type == VAR_LIST ? (void *)our_tv->vval.v_list
		    : our_tv->v_type == VAR_TUPLE ?
			(void *)our_tv->vval.v_tuple
			: (void *)our_tv->vval.v_dict);

	もし ((ret = PyDict_GetItemString(lookup_dict, ptrBuf)))
	{
	    Py_INCREF(ret);
	    戻る ret;
	}
    }

    もし (our_tv->v_type == VAR_STRING)
	ret = PyString_FromString(our_tv->vval.v_string == NULL
					? "" : (char *)our_tv->vval.v_string);
    否则 もし (our_tv->v_type == VAR_NUMBER)
    {
	char buf[NUMBUFLEN];

	// For backwards compatibility numbers are stored as strings.
	sprintf(buf, "%lld", (long long)our_tv->vval.v_number);
	ret = PyString_FromString((char *)buf);
    }
    否则 もし (our_tv->v_type == VAR_FLOAT)
    {
	char buf[NUMBUFLEN];

	sprintf(buf, "%f", our_tv->vval.v_float);
	ret = PyString_FromString((char *)buf);
    }
    否则 もし (our_tv->v_type == VAR_LIST)
    {
	list_T		*list = our_tv->vval.v_list;
	listitem_T	*curr;

	もし (list == NULL)
	    戻る NULL;

	もし (!(ret = PyList_New(0)))
	    戻る NULL;

	もし (PyDict_SetItemString(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    戻る NULL;
	}

	CHECK_LIST_MATERIALIZE(list);
	FOR_ALL_LIST_ITEMS(list, curr)
	{
	    もし (!(newObj = VimToPython(&curr->li_tv, depth + 1, lookup_dict)))
	    {
		Py_DECREF(ret);
		戻る NULL;
	    }
	    もし (PyList_Append(ret, newObj))
	    {
		Py_DECREF(newObj);
		Py_DECREF(ret);
		戻る NULL;
	    }
	    Py_DECREF(newObj);
	}
    }
    否则 もし (our_tv->v_type == VAR_TUPLE)
    {
	tuple_T	*tuple = our_tv->vval.v_tuple;
	整数	len;

	もし (tuple == NULL)
	    戻る NULL;

	len = TUPLE_LEN(tuple);

	ret = PyTuple_New(len);
	もし (ret == NULL)
	    戻る NULL;

	ために (整数 idx = 0; idx < len; idx++)
	{
	    typval_T	*item_tv = TUPLE_ITEM(tuple, idx);

	    newObj = VimToPython(item_tv, depth + 1, lookup_dict);
	    もし (!newObj)
	    {
		Py_DECREF(ret);
		戻る NULL;
	    }
	    PyTuple_SetItem(ret, idx, newObj);
	}

	もし (PyDict_SetItemString(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    戻る NULL;
	}
    }
    否则 もし (our_tv->v_type == VAR_DICT)
    {

	hashtab_T	*ht;
	long_u		todo;
	hashitem_T	*hi;
	dictitem_T	*di;

	もし (our_tv->vval.v_dict == NULL)
	    戻る NULL;
	ht = &our_tv->vval.v_dict->dv_hashtab;

	もし (!(ret = PyDict_New()))
	    戻る NULL;

	もし (PyDict_SetItemString(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    戻る NULL;
	}

	todo = ht->ht_used;
	ために (hi = ht->ht_array; todo > 0; ++hi)
	{
	    もし (!HASHITEM_EMPTY(hi))
	    {
		--todo;

		di = dict_lookup(hi);
		もし (!(newObj = VimToPython(&di->di_tv, depth + 1, lookup_dict)))
		{
		    Py_DECREF(ret);
		    戻る NULL;
		}
		もし (PyDict_SetItemString(ret, (char *)hi->hi_key, newObj))
		{
		    Py_DECREF(ret);
		    Py_DECREF(newObj);
		    戻る NULL;
		}
	    }
	}
    }
    否则 もし (our_tv->v_type == VAR_BOOL)
    {
	もし (our_tv->vval.v_number == VVAL_FALSE)
	{
	    ret = Py_False;
	    Py_INCREF(ret);
	}
	否则
	{
	    ret = Py_True;
	    Py_INCREF(ret);
	}
	戻る ret;
    }
    否则 もし (our_tv->v_type == VAR_SPECIAL)
    {
	Py_INCREF(Py_None);
	ret = Py_None;
	戻る ret;
    }
    否则 もし (our_tv->v_type == VAR_BLOB)
	ret = PyBytes_FromStringAndSize(
		(char*) our_tv->vval.v_blob->bv_ga.ga_data,
		(Py_ssize_t) our_tv->vval.v_blob->bv_ga.ga_len);
    否则
    {
	Py_INCREF(Py_None);
	ret = Py_None;
    }

    戻る ret;
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

    もし (!PyArg_ParseTuple(args, "O", &string))
	戻る NULL;

    もし (!(expr = StringToChars(string, &todecref)))
	戻る NULL;

    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    VimTryStart();
    our_tv = eval_expr(expr, NULL);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    Py_XDECREF(todecref);

    もし (VimTryEnd())
	戻る NULL;

    もし (our_tv == NULL)
    {
	PyErr_SET_VIM(N_("invalid expression"));
	戻る NULL;
    }

    // Convert the Vim type into a Python type.  Create a dictionary that's
    // used to check for recursive loops.
    もし (!(lookup_dict = PyDict_New()))
	ret = NULL;
    否则
    {
	ret = VimToPython(our_tv, 1, lookup_dict);
	Py_DECREF(lookup_dict);
    }


    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    free_tv(our_tv);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    戻る ret;
}

static PyObject *ConvertToPyObject(typval_T *);

    static PyObject *
VimEvalPy(PyObject *self UNUSED, PyObject *string)
{
    typval_T	*our_tv;
    PyObject	*ret;
    char_u	*expr;
    PyObject	*todecref;

    もし (!(expr = StringToChars(string, &todecref)))
	戻る NULL;

    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    VimTryStart();
    our_tv = eval_expr(expr, NULL);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    Py_XDECREF(todecref);

    もし (VimTryEnd())
	戻る NULL;

    もし (our_tv == NULL)
    {
	PyErr_SET_VIM(N_("invalid expression"));
	戻る NULL;
    }

    ret = ConvertToPyObject(our_tv);
    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    free_tv(our_tv);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    戻る ret;
}

    static PyObject *
VimStrwidth(PyObject *self UNUSED, PyObject *string)
{
    char_u	*str;
    PyObject	*todecref;
    整数		len;

    もし (!(str = StringToChars(string, &todecref)))
	戻る NULL;

    len = mb_string2cells(str, (整数)STRLEN(str));

    Py_XDECREF(todecref);

    戻る PyLong_FromLong(len);
}

    static PyObject *
_VimChdir(PyObject *_chdir, PyObject *args, PyObject *kwargs)
{
    PyObject	*ret;
    PyObject	*newwd;
    PyObject	*todecref;
    char_u	*new_dir;

    もし (_chdir == NULL)
	戻る NULL;
    もし (!(ret = PyObject_Call(_chdir, args, kwargs)))
	戻る NULL;

    もし (!(newwd = PyObject_CallFunctionObjArgs(py_getcwd, NULL)))
    {
	Py_DECREF(ret);
	戻る NULL;
    }

    もし (!(new_dir = StringToChars(newwd, &todecref)))
    {
	Py_DECREF(ret);
	Py_DECREF(newwd);
	戻る NULL;
    }

    VimTryStart();

    もし (vim_chdir(new_dir))
    {
	Py_DECREF(ret);
	Py_DECREF(newwd);
	Py_XDECREF(todecref);

	もし (VimTryEnd())
	    戻る NULL;

	PyErr_SET_VIM(N_("failed to change directory"));
	戻る NULL;
    }

    Py_DECREF(newwd);
    Py_XDECREF(todecref);

    post_chdir(CDSCOPE_GLOBAL);

    もし (VimTryEnd())
    {
	Py_DECREF(ret);
	戻る NULL;
    }

    戻る ret;
}

    static PyObject *
VimChdir(PyObject *self UNUSED, PyObject *args, PyObject *kwargs)
{
    戻る _VimChdir(py_chdir, args, kwargs);
}

    static PyObject *
VimFchdir(PyObject *self UNUSED, PyObject *args, PyObject *kwargs)
{
    戻る _VimChdir(py_fchdir, args, kwargs);
}

typedef struct {
    PyObject	*callable;
    PyObject	*result;
} map_rtp_data;

    static void
map_rtp_callback(char_u *path, void *_data)
{
    void	**data = (void **) _data;
    PyObject	*pathObject;
    map_rtp_data	*mr_data = *((map_rtp_data **) data);

    もし (!(pathObject = PyString_FromString((char *)path)))
    {
	*data = NULL;
	戻る;
    }

    mr_data->result = PyObject_CallFunctionObjArgs(mr_data->callable,
						   pathObject, NULL);

    Py_DECREF(pathObject);

    もし (!mr_data->result || mr_data->result != Py_None)
	*data = NULL;
    否则
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

    もし (data.result == NULL)
    {
	もし (PyErr_Occurred())
	    戻る NULL;
	否则
	{
	    Py_INCREF(Py_None);
	    戻る Py_None;
	}
    }
    戻る data.result;
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
    char	*pathbuf;
    size_t	pathlen;

    pathlen = STRLEN(path);

#もし PY_MAJOR_VERSION < 3
# define PY_MAIN_DIR_STRING "python2"
#否则
# define PY_MAIN_DIR_STRING "python3"
#endif
#define PY_ALTERNATE_DIR_STRING "pythonx"

#define PYTHONX_STRING_LENGTH 7 // STRLEN("pythonx")
    もし (!(pathbuf = PyMem_New(char,
		    pathlen + STRLEN(PATHSEPSTR) + PYTHONX_STRING_LENGTH + 1)))
    {
	PyErr_NoMemory();
	*data = NULL;
	戻る;
    }

    mch_memmove(pathbuf, path, pathlen + 1);
    add_pathsep((char_u *) pathbuf);

    pathlen = STRLEN(pathbuf);
    mch_memmove(pathbuf + pathlen, PY_MAIN_DIR_STRING,
	    PYTHONX_STRING_LENGTH + 1);

    もし (!(pathObject1 = PyString_FromString(pathbuf)))
    {
	*data = NULL;
	PyMem_Free(pathbuf);
	戻る;
    }

    mch_memmove(pathbuf + pathlen, PY_ALTERNATE_DIR_STRING,
	    PYTHONX_STRING_LENGTH + 1);

    もし (!(pathObject2 = PyString_FromString(pathbuf)))
    {
	Py_DECREF(pathObject1);
	PyMem_Free(pathbuf);
	*data = NULL;
	戻る;
    }

    PyMem_Free(pathbuf);

    もし (PyList_Append(list, pathObject1)
	    || PyList_Append(list, pathObject2))
	*data = NULL;

    Py_DECREF(pathObject1);
    Py_DECREF(pathObject2);
}

    static PyObject *
Vim_GetPaths(PyObject *self UNUSED, PyObject *args UNUSED)
{
    PyObject	*ret;

    もし (!(ret = PyList_New(0)))
	戻る NULL;

    do_in_runtimepath(NULL, 0, &map_finder_callback, ret);

    もし (PyErr_Occurred())
    {
	Py_DECREF(ret);
	戻る NULL;
    }

    戻る ret;
}

#もし PY_VERSION_HEX >= 0x030700f0
    static PyObject *
FinderFindSpec(PyObject *self, PyObject *args)
{
    char	*fullname;
    PyObject	*paths;
    PyObject	*target = Py_None;
    PyObject	*spec;

    もし (!PyArg_ParseTuple(args, "s|O", &fullname, &target))
	戻る NULL;

    もし (!(paths = Vim_GetPaths(self, NULL)))
	戻る NULL;

    spec = PyObject_CallFunction(py_find_spec, "sOO", fullname, paths, target);

    Py_DECREF(paths);

    もし (!spec)
    {
	もし (PyErr_Occurred())
	    戻る NULL;

	Py_INCREF(Py_None);
	戻る Py_None;
    }

    戻る spec;
}

    static PyObject *
FinderFindModule(PyObject* self UNUSED, PyObject* args UNUSED)
{
    // Apparently returning None works.
    Py_INCREF(Py_None);
    戻る Py_None;
}
#否则
    static PyObject *
call_load_module(char *name, 整数 len, PyObject *find_module_result)
{
    PyObject	*fd, *pathname, *description;

    もし (!PyTuple_Check(find_module_result))
    {
	PyErr_FORMAT_TYPE(
		N_("expected 3-tuple as imp.find_module() result, but got %s"),
		find_module_result);
	戻る NULL;
    }
    もし (PyTuple_GET_SIZE(find_module_result) != 3)
    {
	PyErr_FORMAT(PyExc_TypeError,
		N_("expected 3-tuple as imp.find_module() result, but got "
		   "tuple of size %d"),
		(整数) PyTuple_GET_SIZE(find_module_result));
	戻る NULL;
    }

    もし (!(fd = PyTuple_GET_ITEM(find_module_result, 0))
	    || !(pathname = PyTuple_GET_ITEM(find_module_result, 1))
	    || !(description = PyTuple_GET_ITEM(find_module_result, 2)))
    {
	PyErr_SET_STRING(PyExc_RuntimeError,
		N_("internal error: imp.find_module returned tuple with NULL"));
	戻る NULL;
    }

    戻る PyObject_CallFunction(py_load_module,
	    "s#OOO", name, len, fd, pathname, description);
}

    static PyObject *
find_module(char *fullname, char *tail, PyObject *new_path)
{
    PyObject	*find_module_result;
    PyObject	*module;
    char	*dot;

    もし ((dot = (char *)vim_strchr((char_u *) tail, '.')))
    {
	/*
	 * There is a dot in the name: call find_module recursively without the
	 * first component
	 */
	PyObject	*newest_path;
	整数		partlen = (整数) (dot - 1 - tail);

	もし (!(find_module_result = PyObject_CallFunction(py_find_module,
			"s#O", tail, partlen, new_path)))
	{
	    もし (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_ImportError))
		PyErr_Clear();
	    戻る NULL;
	}

	もし (!(module = call_load_module(
			fullname,
			((整数) (tail - fullname)) + partlen,
			find_module_result)))
	{
	    Py_DECREF(find_module_result);
	    戻る NULL;
	}

	Py_DECREF(find_module_result);

	もし (!(newest_path = PyObject_GetAttrString(module, "__path__")))
	{
	    Py_DECREF(module);
	    戻る NULL;
	}

	Py_DECREF(module);

	find_module_result = find_module(fullname, dot + 1, newest_path);

	Py_DECREF(newest_path);

	戻る find_module_result;
    }
    否则
    {
	もし (!(find_module_result = PyObject_CallFunction(py_find_module,
			"sO", tail, new_path)))
	{
	    もし (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_ImportError))
		PyErr_Clear();
	    戻る NULL;
	}

	戻る find_module_result;
    }
}

    static PyObject *
FinderFindModule(PyObject *self, PyObject *args)
{
    char	*fullname;
    PyObject	*result;
    PyObject	*new_path;
    LoaderObject	*loader;

    もし (!PyArg_ParseTuple(args, "s", &fullname))
	戻る NULL;

    もし (!(new_path = Vim_GetPaths(self, NULL)))
	戻る NULL;

    result = find_module(fullname, fullname, new_path);

    Py_DECREF(new_path);

    もし (!result)
    {
	もし (PyErr_Occurred())
	    戻る NULL;

	Py_INCREF(Py_None);
	戻る Py_None;
    }

    もし (!(fullname = (char *)vim_strsave((char_u *)fullname)))
    {
	Py_DECREF(result);
	PyErr_NoMemory();
	戻る NULL;
    }

    もし (!(loader = PyObject_NEW(LoaderObject, LoaderTypePtr)))
    {
	vim_free(fullname);
	Py_DECREF(result);
	戻る NULL;
    }

    loader->fullname = fullname;
    loader->result = result;

    戻る (PyObject *) loader;
}
#endif

    static PyObject *
VimPathHook(PyObject *self UNUSED, PyObject *args)
{
    char	*path;

    もし (PyArg_ParseTuple(args, "s", &path)
	    && STRCMP(path, vim_special_path) == 0)
    {
	Py_INCREF(vim_module);
	戻る vim_module;
    }

    PyErr_Clear();
    PyErr_SetNone(PyExc_ImportError);
    戻る NULL;
}

/*
 * Vim module - Definitions
 */

static struct PyMethodDef VimMethods[] = {
    // name,	    function,			calling,			documentation
    {"command",	    VimCommand,			METH_O,				"Execute a Vim ex-mode command" },
    {"eval",	    VimEval,			METH_VARARGS,			"Evaluate an expression using Vim evaluator" },
    {"bindeval",    VimEvalPy,			METH_O,				"Like eval(), but returns objects attached to Vim ones"},
    {"strwidth",    VimStrwidth,		METH_O,				"Screen string width, counts <Tab> as having width 1"},
    {"chdir",	    (PyCFunction)(void *)VimChdir,	METH_VARARGS|METH_KEYWORDS,	"Change directory"},
    {"fchdir",	    (PyCFunction)(void *)VimFchdir,	METH_VARARGS|METH_KEYWORDS,	"Change directory"},
    {"foreach_rtp", VimForeachRTP,		METH_O,				"Call given callable for each path in &rtp"},
#もし PY_VERSION_HEX >= 0x030700f0
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
typedef 整数 (*traversefun)(void *, visitproc, void *);
typedef 整数 (*clearfun)(void **);

// Main purpose of this object is removing the need for do python
// initialization (i.e. PyType_Ready and setting type attributes) for a big
// bunch of objects.
typedef struct
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

    もし (iter_object)
	Py_INCREF(iter_object);

    戻る (PyObject *)(self);
}

    static void
IterDestructor(PyObject *self_obj)
{
    IterObject *self = (IterObject*)self_obj;
    もし (self->iter_object)
	Py_DECREF(self->iter_object);
    PyObject_GC_UnTrack((void *)(self));
    self->destruct(self->cur);
    PyObject_GC_Del((void *)(self));
}

    static 整数
IterTraverse(PyObject *self_obj, visitproc visit, void *arg)
{
    IterObject *self = (IterObject*)self_obj;
    もし (self->traverse != NULL)
	戻る self->traverse(self->cur, visit, arg);
    否则
	戻る 0;
}

// Mac OSX defines clear() somewhere.
#ifdef clear
# undef clear
#endif

    static 整数
IterClear(PyObject *self_obj)
{
    IterObject *self = (IterObject*)self_obj;
    もし (self->clear != NULL)
	戻る self->clear(&self->cur);
    否则
	戻る 0;
}

    static PyObject *
IterNext(PyObject *self_obj)
{
    IterObject *self = (IterObject*)self_obj;
    戻る self->next(&self->cur);
}

    static PyObject *
IterIter(PyObject *self)
{
    Py_INCREF(self);
    戻る self;
}

typedef struct pylinkedlist_S {
    struct pylinkedlist_S	*pll_next;
    struct pylinkedlist_S	*pll_prev;
    PyObject			*pll_obj;
} pylinkedlist_T;

static pylinkedlist_T *lastdict = NULL;
static pylinkedlist_T *lastlist = NULL;
static pylinkedlist_T *lasttuple = NULL;
static pylinkedlist_T *lastfunc = NULL;

    static void
pyll_remove(pylinkedlist_T *ref, pylinkedlist_T **last)
{
    もし (ref->pll_prev == NULL)
    {
	もし (ref->pll_next == NULL)
	{
	    *last = NULL;
	    戻る;
	}
    }
    否则
	ref->pll_prev->pll_next = ref->pll_next;

    もし (ref->pll_next == NULL)
	*last = ref->pll_prev;
    否则
	ref->pll_next->pll_prev = ref->pll_prev;
}

    static void
pyll_add(PyObject *self, pylinkedlist_T *ref, pylinkedlist_T **last)
{
    もし (*last == NULL)
	ref->pll_prev = NULL;
    否则
    {
	(*last)->pll_next = ref;
	ref->pll_prev = *last;
    }
    ref->pll_next = NULL;
    ref->pll_obj = self;
    *last = ref;
}

DEFINE_PY_TYPE_OBJECT(DictionaryType);

typedef struct
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
    もし (self == NULL)
	戻る NULL;
    self->dict = dict;
    ++dict->dv_refcount;

    pyll_add((PyObject *)(self), &self->ref, &lastdict);

    戻る (PyObject *)(self);
}

    static dict_T *
py_dict_alloc(void)
{
    dict_T	*ret;

    もし (!(ret = dict_alloc()))
    {
	PyErr_NoMemory();
	戻る NULL;
    }
    ++ret->dv_refcount;

    戻る ret;
}

    static PyObject *
DictionaryConstructor(PyTypeObject *subtype, PyObject *args, PyObject *kwargs)
{
    DictionaryObject	*self;
    dict_T	*dict;

    もし (!(dict = py_dict_alloc()))
	戻る NULL;

    self = (DictionaryObject *) DictionaryNew(subtype, dict);

    --dict->dv_refcount;

    もし (kwargs || PyTuple_Size(args))
    {
	PyObject	*tmp;
	もし (!(tmp = DictionaryUpdate(self, args, kwargs)))
	{
	    Py_DECREF(self);
	    戻る NULL;
	}

	Py_DECREF(tmp);
    }

    戻る (PyObject *)(self);
}

    static void
DictionaryDestructor(PyObject *self_obj)
{
    DictionaryObject *self = (DictionaryObject*)self_obj;
    pyll_remove(&self->ref, &lastdict);
    dict_unref(self->dict);

    DESTRUCTOR_FINISH(self);
}

static char *DictionaryAttrs[] = {
    "locked", "scope",
    NULL
};

    static PyObject *
DictionaryDir(PyObject *self, PyObject *args UNUSED)
{
    戻る ObjectDir(self, DictionaryAttrs);
}

    static 整数
DictionarySetattr(PyObject *self_obj, char *name, PyObject *valObject)
{
    DictionaryObject *self = (DictionaryObject*)self_obj;
    もし (valObject == NULL)
    {
	PyErr_SET_STRING(PyExc_AttributeError,
		N_("cannot delete vim.Dictionary attributes"));
	戻る -1;
    }

    もし (strcmp(name, "locked") == 0)
    {
	もし (self->dict->dv_lock == VAR_FIXED)
	{
	    PyErr_SET_STRING(PyExc_TypeError,
		    N_("cannot modify fixed dictionary"));
	    戻る -1;
	}
	否则
	{
	    整数		istrue = PyObject_IsTrue(valObject);
	    もし (istrue == -1)
		戻る -1;
	    否则 もし (istrue)
		self->dict->dv_lock = VAR_LOCKED;
	    否则
		self->dict->dv_lock = 0;
	}
	戻る 0;
    }
    否则
    {
	PyErr_FORMAT(PyExc_AttributeError, N_("cannot set attribute %s"), name);
	戻る -1;
    }
}

    static PyInt
DictionaryLength(DictionaryObject *self)
{
    戻る ((PyInt) (self->dict->dv_hashtab.ht_used));
}

#define DICT_FLAG_HAS_DEFAULT	0x01
#define DICT_FLAG_POP		0x02
#define DICT_FLAG_NONE_DEFAULT	0x04
#define DICT_FLAG_RETURN_BOOL	0x08 // Incompatible with DICT_FLAG_POP
#define DICT_FLAG_RETURN_PAIR	0x10

    static PyObject *
_DictionaryItem(DictionaryObject *self, PyObject *args, 整数 flags)
{
    PyObject	*keyObject;
    PyObject	*defObject = ((flags & DICT_FLAG_NONE_DEFAULT)? Py_None : NULL);
    PyObject	*ret;
    char_u	*key;
    dictitem_T	*di;
    dict_T	*dict = self->dict;
    hashitem_T	*hi;
    PyObject	*todecref;

    もし (flags & DICT_FLAG_HAS_DEFAULT)
    {
	もし (!PyArg_ParseTuple(args, "O|O", &keyObject, &defObject))
	    戻る NULL;
    }
    否则
	keyObject = args;

    もし (flags & DICT_FLAG_RETURN_BOOL)
	defObject = Py_False;

    もし (!(key = StringToChars(keyObject, &todecref)))
	戻る NULL;

    もし (*key == NUL)
    {
	RAISE_NO_EMPTY_KEYS;
	Py_XDECREF(todecref);
	戻る NULL;
    }

    hi = hash_find(&dict->dv_hashtab, key);

    Py_XDECREF(todecref);

    もし (HASHITEM_EMPTY(hi))
    {
	もし (defObject)
	{
	    Py_INCREF(defObject);
	    戻る defObject;
	}
	否则
	{
	    PyErr_SetObject(PyExc_KeyError, keyObject);
	    戻る NULL;
	}
    }
    否则 もし (flags & DICT_FLAG_RETURN_BOOL)
    {
	ret = Py_True;
	Py_INCREF(ret);
	戻る ret;
    }

    di = dict_lookup(hi);

    もし (!(ret = ConvertToPyObject(&di->di_tv)))
	戻る NULL;

    もし (flags & DICT_FLAG_POP)
    {
	もし (dict->dv_lock)
	{
	    RAISE_LOCKED_DICTIONARY;
	    Py_DECREF(ret);
	    戻る NULL;
	}

	hash_remove(&dict->dv_hashtab, hi, "Python remove variable");
	dictitem_free(di);
    }

    戻る ret;
}

    static PyObject *
DictionaryItem(DictionaryObject *self, PyObject *keyObject)
{
    戻る _DictionaryItem(self, keyObject, 0);
}

    static 整数
DictionaryContains(DictionaryObject *self, PyObject *keyObject)
{
    PyObject	*rObj = _DictionaryItem(self, keyObject, DICT_FLAG_RETURN_BOOL);
    整数		ret;

    もし (rObj == NULL)
	戻る -1;

    ret = (rObj == Py_True);

    Py_DECREF(rObj);

    戻る ret;
}

typedef struct
{
    整数		dii_changed;
    hashtab_T	*dii_ht;
    hashitem_T	*dii_hi;
    long_u	dii_todo;
} dictiterinfo_T;

    static PyObject *
DictionaryIterNext(void **arg)
{
    PyObject	*ret;
    dictiterinfo_T **dii = (dictiterinfo_T**)arg;

    もし (!(*dii)->dii_todo)
	戻る NULL;

    もし ((*dii)->dii_ht->ht_changed != (*dii)->dii_changed)
    {
	PyErr_SET_STRING(PyExc_RuntimeError,
		N_("hashtab changed during iteration"));
	戻る NULL;
    }

    ながら (((*dii)->dii_todo) && HASHITEM_EMPTY((*dii)->dii_hi))
	++((*dii)->dii_hi);

    --((*dii)->dii_todo);

    もし (!(ret = PyBytes_FromString((char *)(*dii)->dii_hi->hi_key)))
	戻る NULL;

    戻る ret;
}

    static PyObject *
DictionaryIter(PyObject *self_obj)
{
    DictionaryObject	*self = (DictionaryObject*)self_obj;
    dictiterinfo_T	*dii;
    hashtab_T		*ht;

    もし (!(dii = PyMem_New(dictiterinfo_T, 1)))
    {
	PyErr_NoMemory();
	戻る NULL;
    }

    ht = &self->dict->dv_hashtab;
    dii->dii_changed = ht->ht_changed;
    dii->dii_ht = ht;
    dii->dii_hi = ht->ht_array;
    dii->dii_todo = ht->ht_used;

    戻る IterNew(dii,
	    PyMem_Free, DictionaryIterNext,
	    NULL, NULL, (PyObject *)self);
}

    static 整数
DictionaryAssItem(
	DictionaryObject *self, PyObject *keyObject, PyObject *valObject)
{
    char_u	*key;
    typval_T	tv;
    dict_T	*dict = self->dict;
    dictitem_T	*di;
    PyObject	*todecref;

    もし (dict->dv_lock)
    {
	RAISE_LOCKED_DICTIONARY;
	戻る -1;
    }

    もし (!(key = StringToChars(keyObject, &todecref)))
	戻る -1;

    もし (*key == NUL)
    {
	RAISE_NO_EMPTY_KEYS;
	Py_XDECREF(todecref);
	戻る -1;
    }

    di = dict_find(dict, key, -1);

    もし (valObject == NULL)
    {
	hashitem_T	*hi;

	もし (di == NULL)
	{
	    Py_XDECREF(todecref);
	    PyErr_SetObject(PyExc_KeyError, keyObject);
	    戻る -1;
	}
	hi = hash_find(&dict->dv_hashtab, di->di_key);
	hash_remove(&dict->dv_hashtab, hi, "Python remove item");
	dictitem_free(di);
	Py_XDECREF(todecref);
	戻る 0;
    }

    もし (ConvertFromPyObject(valObject, &tv) == -1)
    {
	Py_XDECREF(todecref);
	戻る -1;
    }

    もし (di == NULL)
    {
	もし (!(di = dictitem_alloc(key)))
	{
	    Py_XDECREF(todecref);
	    PyErr_NoMemory();
	    戻る -1;
	}
	di->di_tv.v_type = VAR_UNKNOWN;

	もし (dict_add(dict, di) == FAIL)
	{
	    dictitem_free(di);
	    RAISE_KEY_ADD_FAIL(key);
	    Py_XDECREF(todecref);
	    戻る -1;
	}
    }
    否则
	clear_tv(&di->di_tv);

    Py_XDECREF(todecref);

    copy_tv(&tv, &di->di_tv);
    clear_tv(&tv);
    戻る 0;
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
    ために (hi = dict->dv_hashtab.ht_array; todo > 0; ++hi)
    {
	もし (!HASHITEM_EMPTY(hi))
	{
	    もし (!(newObj = hiconvert(hi)))
	    {
		Py_DECREF(ret);
		戻る NULL;
	    }
	    PyList_SET_ITEM(ret, i, newObj);
	    --todo;
	    ++i;
	}
    }
    戻る ret;
}

    static PyObject *
dict_key(hashitem_T *hi)
{
    戻る PyBytes_FromString((char *)(hi->hi_key));
}

    static PyObject *
DictionaryListKeys(DictionaryObject *self, PyObject *args UNUSED)
{
    戻る DictionaryListObjects(self, dict_key);
}

    static PyObject *
dict_val(hashitem_T *hi)
{
    dictitem_T	*di;

    di = dict_lookup(hi);
    戻る ConvertToPyObject(&di->di_tv);
}

    static PyObject *
DictionaryListValues(DictionaryObject *self, PyObject *args UNUSED)
{
    戻る DictionaryListObjects(self, dict_val);
}

    static PyObject *
dict_item(hashitem_T *hi)
{
    PyObject	*keyObject;
    PyObject	*valObject;
    PyObject	*ret;

    もし (!(keyObject = dict_key(hi)))
	戻る NULL;

    もし (!(valObject = dict_val(hi)))
    {
	Py_DECREF(keyObject);
	戻る NULL;
    }

    ret = Py_BuildValue("(OO)", keyObject, valObject);

    Py_DECREF(keyObject);
    Py_DECREF(valObject);

    戻る ret;
}

    static PyObject *
DictionaryListItems(DictionaryObject *self, PyObject *args UNUSED)
{
    戻る DictionaryListObjects(self, dict_item);
}

    static PyObject *
DictionaryUpdate(DictionaryObject *self, PyObject *args, PyObject *kwargs)
{
    dict_T	*dict = self->dict;

    もし (dict->dv_lock)
    {
	RAISE_LOCKED_DICTIONARY;
	戻る NULL;
    }

    もし (kwargs)
    {
	typval_T	tv;

	もし (ConvertFromPyMapping(kwargs, &tv) == -1)
	    戻る NULL;

	VimTryStart();
	dict_extend(self->dict, tv.vval.v_dict, (char_u *) "force", NULL);
	clear_tv(&tv);
	もし (VimTryEnd())
	    戻る NULL;
    }
    否则
    {
	PyObject	*obj = NULL;

	もし (!PyArg_ParseTuple(args, "|O", &obj))
	    戻る NULL;

	もし (obj == NULL)
	{
	    Py_INCREF(Py_None);
	    戻る Py_None;
	}

	もし (PyObject_HasAttrString(obj, "keys"))
	    戻る DictionaryUpdate(self, NULL, obj);
	否则
	{
	    PyObject	*iterator;
	    PyObject	*item;

	    もし (!(iterator = PyObject_GetIter(obj)))
		戻る NULL;

	    ながら ((item = PyIter_Next(iterator)))
	    {
		PyObject	*fast;
		PyObject	*keyObject;
		PyObject	*valObject;
		PyObject	*todecref;
		char_u		*key;
		dictitem_T	*di;
		hashitem_T	*hi;

		もし (!(fast = PySequence_Fast(item, "")))
		{
		    Py_DECREF(iterator);
		    Py_DECREF(item);
		    戻る NULL;
		}

		Py_DECREF(item);

		もし (PySequence_Size(fast) != 2)
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    PyErr_FORMAT(PyExc_ValueError,
			    N_("expected sequence element of size 2, "
			    "but got sequence of size %d"),
			    (整数) PySequence_Size(fast));
		    戻る NULL;
		}

		keyObject = PySequence_GetItem(fast, 0);

		もし (!(key = StringToChars(keyObject, &todecref)))
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    戻る NULL;
		}

		di = dictitem_alloc(key);

		Py_XDECREF(todecref);

		もし (di == NULL)
		{
		    Py_DECREF(fast);
		    Py_DECREF(iterator);
		    PyErr_NoMemory();
		    戻る NULL;
		}
		di->di_tv.v_type = VAR_UNKNOWN;

		valObject = PySequence_GetItem(fast, 1);

		もし (ConvertFromPyObject(valObject, &di->di_tv) == -1)
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    dictitem_free(di);
		    戻る NULL;
		}

		Py_DECREF(fast);

		hi = hash_find(&dict->dv_hashtab, di->di_key);
		もし (!HASHITEM_EMPTY(hi) || dict_add(dict, di) == FAIL)
		{
		    RAISE_KEY_ADD_FAIL(di->di_key);
		    Py_DECREF(iterator);
		    dictitem_free(di);
		    戻る NULL;
		}
	    }

	    Py_DECREF(iterator);

	    // Iterator may have finished due to an exception
	    もし (PyErr_Occurred())
		戻る NULL;
	}
    }
    Py_INCREF(Py_None);
    戻る Py_None;
}

    static PyObject *
DictionaryGet(DictionaryObject *self, PyObject *args)
{
    戻る _DictionaryItem(self, args,
			    DICT_FLAG_HAS_DEFAULT|DICT_FLAG_NONE_DEFAULT);
}

    static PyObject *
DictionaryPop(DictionaryObject *self, PyObject *args)
{
    戻る _DictionaryItem(self, args, DICT_FLAG_HAS_DEFAULT|DICT_FLAG_POP);
}

    static PyObject *
DictionaryPopItem(DictionaryObject *self, PyObject *args UNUSED)
{
    hashitem_T	*hi;
    PyObject	*ret;
    PyObject	*valObject;
    dictitem_T	*di;

    もし (self->dict->dv_hashtab.ht_used == 0)
    {
	PyErr_SetNone(PyExc_KeyError);
	戻る NULL;
    }

    hi = self->dict->dv_hashtab.ht_array;
    ながら (HASHITEM_EMPTY(hi))
	++hi;

    di = dict_lookup(hi);

    もし (!(valObject = ConvertToPyObject(&di->di_tv)))
	戻る NULL;

    もし (!(ret = Py_BuildValue("(" Py_bytes_fmt "O)", hi->hi_key, valObject)))
    {
	Py_DECREF(valObject);
	戻る NULL;
    }

    hash_remove(&self->dict->dv_hashtab, hi, "Python pop item");
    dictitem_free(di);

    戻る ret;
}

    static PyObject *
DictionaryHasKey(DictionaryObject *self, PyObject *keyObject)
{
    戻る _DictionaryItem(self, keyObject, DICT_FLAG_RETURN_BOOL);
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

static struct PyMethodDef DictionaryMethods[] = {
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

typedef struct
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

    もし (list == NULL)
	戻る NULL;

    self = (ListObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    もし (self == NULL)
	戻る NULL;
    self->list = list;
    ++list->lv_refcount;
    CHECK_LIST_MATERIALIZE(list);

    pyll_add((PyObject *)(self), &self->ref, &lastlist);

    戻る (PyObject *)(self);
}

    static list_T *
py_list_alloc(void)
{
    list_T	*ret;

    もし (!(ret = list_alloc()))
    {
	PyErr_NoMemory();
	戻る NULL;
    }
    ++ret->lv_refcount;

    戻る ret;
}

    static 整数
list_py_concat(list_T *l, PyObject *obj, PyObject *lookup_dict)
{
    PyObject	*iterator;
    PyObject	*item;
    listitem_T	*li;

    もし (!(iterator = PyObject_GetIter(obj)))
	戻る -1;

    ながら ((item = PyIter_Next(iterator)))
    {
	もし (!(li = listitem_alloc()))
	{
	    PyErr_NoMemory();
	    Py_DECREF(item);
	    Py_DECREF(iterator);
	    戻る -1;
	}
	li->li_tv.v_lock = 0;
	li->li_tv.v_type = VAR_UNKNOWN;

	もし (_ConvertFromPyObject(item, &li->li_tv, lookup_dict) == -1)
	{
	    Py_DECREF(item);
	    Py_DECREF(iterator);
	    listitem_free(l, li);
	    戻る -1;
	}

	Py_DECREF(item);

	list_append(l, li);
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    もし (PyErr_Occurred())
	戻る -1;

    戻る 0;
}

    static PyObject *
ListConstructor(PyTypeObject *subtype, PyObject *args, PyObject *kwargs)
{
    list_T	*list;
    PyObject	*obj = NULL;

    もし (kwargs)
    {
	PyErr_SET_STRING(PyExc_TypeError,
		N_("list constructor does not accept keyword arguments"));
	戻る NULL;
    }

    もし (!PyArg_ParseTuple(args, "|O", &obj))
	戻る NULL;

    もし (!(list = py_list_alloc()))
	戻る NULL;

    もし (obj)
    {
	PyObject	*lookup_dict;

	もし (!(lookup_dict = PyDict_New()))
	{
	    list_unref(list);
	    戻る NULL;
	}

	もし (list_py_concat(list, obj, lookup_dict) == -1)
	{
	    Py_DECREF(lookup_dict);
	    list_unref(list);
	    戻る NULL;
	}

	Py_DECREF(lookup_dict);
    }

    戻る ListNew(subtype, list);
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
    戻る ((PyInt) (self->list->lv_len));
}

    static PyObject *
ListIndex(ListObject *self, Py_ssize_t index)
{
    listitem_T	*li;

    もし (index >= ListLength(self))
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("list index out of range"));
	戻る NULL;
    }
    li = list_find(self->list, (long) index);
    もし (li == NULL)
    {
	// No more suitable format specifications in python-2.3
	PyErr_VIM_FORMAT(N_("internal error: failed to get Vim list item %d"),
		(整数) index);
	戻る NULL;
    }
    戻る ConvertToPyObject(&li->li_tv);
}

    static PyObject *
ListSlice(ListObject *self, Py_ssize_t first, Py_ssize_t step,
	  Py_ssize_t slicelen)
{
    PyInt	i;
    PyObject	*list;

    もし (step == 0)
    {
	PyErr_SET_STRING(PyExc_ValueError, N_("slice step cannot be zero"));
	戻る NULL;
    }

    list = PyList_New(slicelen);
    もし (list == NULL)
	戻る NULL;

    ために (i = 0; i < slicelen; ++i)
    {
	PyObject	*item;

	item = ListIndex(self, first + i*step);
	もし (item == NULL)
	{
	    Py_DECREF(list);
	    戻る NULL;
	}

	PyList_SET_ITEM(list, i, item);
    }

    戻る list;
}

    static PyObject *
ListItem(ListObject *self, PyObject* idx)
{
#もし PY_MAJOR_VERSION < 3
    もし (PyInt_Check(idx))
    {
	long _idx = PyInt_AsLong(idx);
	戻る ListIndex(self, _idx);
    }
    否则
#endif
    もし (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	戻る ListIndex(self, _idx);
    }
    否则 もし (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	もし (PySlice_GetIndicesEx((PySliceObject_T *)idx, ListLength(self),
				 &start, &stop, &step, &slicelen) < 0)
	    戻る NULL;
	戻る ListSlice(self, start, step, slicelen);
    }
    否则
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	戻る NULL;
    }
}

    static void
list_restore(Py_ssize_t numadded, Py_ssize_t numreplaced, Py_ssize_t slicelen,
	list_T *l, listitem_T **lis, listitem_T *lastaddedli)
{
    ながら (numreplaced--)
    {
	list_insert(l, lis[numreplaced], lis[slicelen + numreplaced]);
	listitem_remove(l, lis[slicelen + numreplaced]);
    }
    ながら (numadded--)
    {
	listitem_T	*next;

	next = lastaddedli->li_prev;
	listitem_remove(l, lastaddedli);
	lastaddedli = next;
    }
}

    static 整数
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

    もし (l->lv_lock)
    {
	RAISE_LOCKED_LIST;
	戻る -1;
    }

    もし (step == 0)
    {
	PyErr_SET_STRING(PyExc_ValueError, N_("slice step cannot be zero"));
	戻る -1;
    }

    もし (step != 1 && slicelen == 0)
    {
	// Nothing to do. Only error out if obj has some items.
	整数		ret = 0;

	もし (obj == NULL)
	    戻る 0;

	もし (!(iterator = PyObject_GetIter(obj)))
	    戻る -1;

	もし ((item = PyIter_Next(iterator)))
	{
	    PyErr_FORMAT(PyExc_ValueError,
		    N_("attempt to assign sequence of size greater than %d "
			"to extended slice"), 0);
	    Py_DECREF(item);
	    ret = -1;
	}
	Py_DECREF(iterator);
	戻る ret;
    }

    もし (obj != NULL)
	// XXX May allocate zero bytes.
	もし (!(lis = PyMem_New(listitem_T *, slicelen * 2)))
	{
	    PyErr_NoMemory();
	    戻る -1;
	}

    もし (first == size)
	li = NULL;
    否则
    {
	li = list_find(l, (long) first);
	もし (li == NULL)
	{
	    PyErr_VIM_FORMAT(N_("internal error: no Vim list item %d"),
		    (整数)first);
	    もし (obj != NULL)
		PyMem_Free(lis);
	    戻る -1;
	}
	i = slicelen;
	ながら (i-- && li != NULL)
	{
	    j = step;
	    next = li;
	    もし (step > 0)
		ながら (next != NULL && ((next = next->li_next) != NULL) && --j);
	    否则
		ながら (next != NULL && ((next = next->li_prev) != NULL) && ++j);

	    もし (obj == NULL)
		listitem_remove(l, li);
	    否则
		lis[slicelen - i - 1] = li;

	    li = next;
	}
	もし (li == NULL && i != -1)
	{
	    PyErr_SET_VIM(N_("internal error: not enough list items"));
	    もし (obj != NULL)
		PyMem_Free(lis);
	    戻る -1;
	}
    }

    もし (obj == NULL)
	戻る 0;

    もし (!(iterator = PyObject_GetIter(obj)))
    {
	PyMem_Free(lis);
	戻る -1;
    }

    i = 0;
    ながら ((item = PyIter_Next(iterator)))
    {
	もし (ConvertFromPyObject(item, &v) == -1)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(item);
	    PyMem_Free(lis);
	    戻る -1;
	}
	Py_DECREF(item);
	もし (list_insert_tv(l, &v, numreplaced < slicelen
				    ? lis[numreplaced]
				    : li) == FAIL)
	{
	    clear_tv(&v);
	    PyErr_SET_VIM(N_("internal error: failed to add item to list"));
	    list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	    PyMem_Free(lis);
	    戻る -1;
	}
	もし (numreplaced < slicelen)
	{
	    lis[slicelen + numreplaced] = lis[numreplaced]->li_prev;
	    vimlist_remove(l, lis[numreplaced], lis[numreplaced]);
	    numreplaced++;
	}
	否则
	{
	    もし (li)
		lastaddedli = li->li_prev;
	    否则
		lastaddedli = l->lv_u.mat.lv_last;
	    numadded++;
	}
	clear_tv(&v);
	もし (step != 1 && i >= slicelen)
	{
	    Py_DECREF(iterator);
	    PyErr_FORMAT(PyExc_ValueError,
		    N_("attempt to assign sequence of size greater than %d "
			"to extended slice"), (整数) slicelen);
	    list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	    PyMem_Free(lis);
	    戻る -1;
	}
	++i;
    }
    Py_DECREF(iterator);

    もし (step != 1 && i != slicelen)
    {
	PyErr_FORMAT2(PyExc_ValueError,
		N_("attempt to assign sequence of size %d to extended slice "
		    "of size %d"), (整数) i, (整数) slicelen);
	list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	PyMem_Free(lis);
	戻る -1;
    }

    もし (PyErr_Occurred())
    {
	list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	PyMem_Free(lis);
	戻る -1;
    }

    ために (i = 0; i < numreplaced; i++)
	listitem_free(l, lis[i]);
    もし (step == 1)
	ために (i = numreplaced; i < slicelen; i++)
	    listitem_remove(l, lis[i]);

    PyMem_Free(lis);

    戻る 0;
}

    static 整数
ListAssIndex(ListObject *self, Py_ssize_t index, PyObject *obj)
{
    typval_T	tv;
    list_T	*l = self->list;
    listitem_T	*li;
    Py_ssize_t	length = ListLength(self);

    もし (l->lv_lock)
    {
	RAISE_LOCKED_LIST;
	戻る -1;
    }
    もし (index > length || (index == length && obj == NULL))
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("list index out of range"));
	戻る -1;
    }

    もし (obj == NULL)
    {
	li = list_find(l, (long) index);
	もし (li == NULL)
	{
	    PyErr_VIM_FORMAT(N_("internal error: failed to get Vim "
			"list item %d"), (整数) index);
	    戻る -1;
	}
	vimlist_remove(l, li, li);
	clear_tv(&li->li_tv);
	vim_free(li);
	戻る 0;
    }

    もし (ConvertFromPyObject(obj, &tv) == -1)
	戻る -1;

    もし (index == length)
    {
	もし (list_append_tv(l, &tv) == FAIL)
	{
	    clear_tv(&tv);
	    PyErr_SET_VIM(N_("failed to add item to list"));
	    戻る -1;
	}
    }
    否则
    {
	li = list_find(l, (long) index);
	もし (li == NULL)
	{
	    PyErr_VIM_FORMAT(N_("internal error: failed to get Vim "
			"list item %d"), (整数) index);
	    戻る -1;
	}
	clear_tv(&li->li_tv);
	copy_tv(&tv, &li->li_tv);
	clear_tv(&tv);
    }
    戻る 0;
}

    static 整数
ListAssItem(ListObject *self, PyObject *idx, PyObject *obj)
{
#もし PY_MAJOR_VERSION < 3
    もし (PyInt_Check(idx))
    {
	long _idx = PyInt_AsLong(idx);
	戻る (整数)ListAssIndex(self, _idx, obj);
    }
    否则
#endif
    もし (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	戻る (整数)ListAssIndex(self, _idx, obj);
    }
    否则 もし (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	もし (PySlice_GetIndicesEx((PySliceObject_T *)idx, ListLength(self),
				 &start, &stop, &step, &slicelen) < 0)
	    戻る -1;
	戻る (整数)ListAssSlice(self, start, step, slicelen,
		obj);
    }
    否则
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	戻る -1;
    }
}

    static PyObject *
ListConcatInPlace(ListObject *self, PyObject *obj)
{
    list_T	*l = self->list;
    PyObject	*lookup_dict;

    もし (l->lv_lock)
    {
	RAISE_LOCKED_LIST;
	戻る NULL;
    }

    もし (!(lookup_dict = PyDict_New()))
	戻る NULL;

    もし (list_py_concat(l, obj, lookup_dict) == -1)
    {
	Py_DECREF(lookup_dict);
	戻る NULL;
    }
    Py_DECREF(lookup_dict);

    Py_INCREF((PyObject *)self);
    戻る (PyObject *)(self);
}

typedef struct
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

    もし (!((*lii)->lw.lw_item))
	戻る NULL;

    もし (!(ret = ConvertToPyObject(&((*lii)->lw.lw_item->li_tv))))
	戻る NULL;

    (*lii)->lw.lw_item = (*lii)->lw.lw_item->li_next;

    戻る ret;
}

    static PyObject *
ListIter(PyObject *self_obj)
{
    ListObject	*self = (ListObject*)self_obj;
    listiterinfo_T	*lii;
    list_T	*l = self->list;

    もし (!(lii = PyMem_New(listiterinfo_T, 1)))
    {
	PyErr_NoMemory();
	戻る NULL;
    }

    CHECK_LIST_MATERIALIZE(l);
    list_add_watch(l, &lii->lw);
    lii->lw.lw_item = l->lv_first;
    lii->list = l;
    ++l->lv_refcount;

    戻る IterNew(lii,
	    ListIterDestruct, ListIterNext,
	    NULL, NULL, (PyObject *)self);
}

static char *ListAttrs[] = {
    "locked",
    NULL
};

    static PyObject *
ListDir(PyObject *self, PyObject *args UNUSED)
{
    戻る ObjectDir(self, ListAttrs);
}

    static 整数
ListSetattr(PyObject *self_obj, char *name, PyObject *valObject)
{
    ListObject *self = (ListObject*)self_obj;
    もし (valObject == NULL)
    {
	PyErr_SET_STRING(PyExc_AttributeError,
		N_("cannot delete vim.List attributes"));
	戻る -1;
    }

    もし (strcmp(name, "locked") == 0)
    {
	もし (self->list->lv_lock == VAR_FIXED)
	{
	    PyErr_SET_STRING(PyExc_TypeError, N_("cannot modify fixed list"));
	    戻る -1;
	}
	否则
	{
	    整数		istrue = PyObject_IsTrue(valObject);
	    もし (istrue == -1)
		戻る -1;
	    否则 もし (istrue)
		self->list->lv_lock = VAR_LOCKED;
	    否则
		self->list->lv_lock = 0;
	}
	戻る 0;
    }
    否则
    {
	PyErr_FORMAT(PyExc_AttributeError, N_("cannot set attribute %s"), name);
	戻る -1;
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

static struct PyMethodDef ListMethods[] = {
    {"extend",	(PyCFunction)ListConcatInPlace,	METH_O,		""},
    {"__dir__",	(PyCFunction)ListDir,		METH_NOARGS,	""},
    { NULL,	NULL,				0,		NULL}
};

DEFINE_PY_TYPE_OBJECT(TupleType);

typedef struct
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

    もし (tuple == NULL)
	戻る NULL;

    self = (TupleObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    もし (self == NULL)
	戻る NULL;
    self->tuple = tuple;
    ++tuple->tv_refcount;

    pyll_add((PyObject *)(self), &self->ref, &lasttuple);

    戻る (PyObject *)(self);
}

    static tuple_T *
py_tuple_alloc(void)
{
    tuple_T	*ret;

    もし (!(ret = tuple_alloc()))
    {
	PyErr_NoMemory();
	戻る NULL;
    }
    ++ret->tv_refcount;

    戻る ret;
}

    static 整数
tuple_py_concat(tuple_T *t, PyObject *obj, PyObject *lookup_dict)
{
    PyObject	*iterator;
    PyObject	*item;

    もし (!(iterator = PyObject_GetIter(obj)))
	戻る -1;

    ながら ((item = PyIter_Next(iterator)))
    {
	typval_T    new_tv;

	もし (_ConvertFromPyObject(item, &new_tv, lookup_dict) == -1)
	{
	    Py_DECREF(item);
	    Py_DECREF(iterator);
	    戻る -1;
	}

	Py_DECREF(item);

	もし (tuple_append_tv(t, &new_tv) == FAIL)
	{
	    Py_DECREF(iterator);
	    戻る -1;
	}
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    もし (PyErr_Occurred())
	戻る -1;

    戻る 0;
}

    static PyObject *
TupleConstructor(PyTypeObject *subtype, PyObject *args, PyObject *kwargs)
{
    tuple_T	*tuple;
    PyObject	*obj = NULL;

    もし (kwargs)
    {
	PyErr_SET_STRING(PyExc_TypeError,
		N_("tuple constructor does not accept keyword arguments"));
	戻る NULL;
    }

    もし (!PyArg_ParseTuple(args, "|O", &obj))
	戻る NULL;

    もし (!(tuple = py_tuple_alloc()))
	戻る NULL;

    もし (obj)
    {
	PyObject	*lookup_dict;

	もし (!(lookup_dict = PyDict_New()))
	{
	    tuple_unref(tuple);
	    戻る NULL;
	}

	もし (tuple_py_concat(tuple, obj, lookup_dict) == -1)
	{
	    Py_DECREF(lookup_dict);
	    tuple_unref(tuple);
	    戻る NULL;
	}

	Py_DECREF(lookup_dict);
    }

    戻る TupleNew(subtype, tuple);
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
    戻る ((PyInt)(tuple_len(self->tuple)));
}

    static PyObject *
TupleIndex(TupleObject *self, Py_ssize_t index)
{
    PyInt	len = TupleLength(self);

    もし (index < 0)
	index = len + index;

    もし (index < 0 || index >= len)
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("tuple index out of range"));
	戻る NULL;
    }
    戻る ConvertToPyObject(TUPLE_ITEM(self->tuple, index));
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
    もし (tuple == NULL)
	戻る NULL;

    ために (i = 0; i < slicelen; ++i)
    {
	PyObject	*item;

	item = TupleIndex(self, first + i * step);
	もし (item == NULL)
	{
	    Py_DECREF(tuple);
	    戻る NULL;
	}

	PyTuple_SetItem(tuple, i, item);
    }

    戻る tuple;
}

    static PyObject *
TupleItem(TupleObject *self, PyObject* idx)
{
#もし PY_MAJOR_VERSION < 3
    もし (PyInt_Check(idx))
    {
	long _idx = PyInt_AsLong(idx);
	戻る TupleIndex(self, _idx);
    }
    否则
#endif
    もし (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	戻る TupleIndex(self, _idx);
    }
    否则 もし (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	もし (PySlice_GetIndicesEx((PySliceObject_T *)idx, TupleLength(self),
				 &start, &stop, &step, &slicelen) < 0)
	    戻る NULL;
	戻る TupleSlice(self, start, step, slicelen);
    }
    否则
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	戻る NULL;
    }
}

typedef struct
{
    tuple_T	*tuple;
    整数		index;
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

    もし ((*tii)->index >= TUPLE_LEN((*tii)->tuple))
	戻る NULL;

    もし (!(ret = ConvertToPyObject(TUPLE_ITEM((*tii)->tuple, (*tii)->index))))
	戻る NULL;

    (*tii)->index++;

    戻る ret;
}

    static PyObject *
TupleIter(PyObject *self_obj)
{
    TupleObject	*self = (TupleObject*)self_obj;
    tupleiterinfo_T	*tii;
    tuple_T	*t = self->tuple;

    もし (!(tii = PyMem_New(tupleiterinfo_T, 1)))
    {
	PyErr_NoMemory();
	戻る NULL;
    }

    tii->tuple = t;
    tii->index = 0;
    ++t->tv_refcount;

    戻る IterNew(tii,
	    TupleIterDestruct, TupleIterNext,
	    NULL, NULL, (PyObject *)self);
}

static char *TupleAttrs[] = {
    "locked",
    NULL
};

    static PyObject *
TupleDir(PyObject *self, PyObject *args UNUSED)
{
    戻る ObjectDir(self, TupleAttrs);
}

    static 整数
TupleSetattr(PyObject *self_obj, char *name, PyObject *valObject)
{
    TupleObject *self = (TupleObject*)self_obj;
    もし (valObject == NULL)
    {
	PyErr_SET_STRING(PyExc_AttributeError,
		N_("cannot delete vim.Tuple attributes"));
	戻る -1;
    }

    もし (strcmp(name, "locked") == 0)
    {
	もし (self->tuple->tv_lock == VAR_FIXED)
	{
	    PyErr_SET_STRING(PyExc_TypeError, N_("cannot modify fixed tuple"));
	    戻る -1;
	}
	否则
	{
	    整数		istrue = PyObject_IsTrue(valObject);
	    もし (istrue == -1)
		戻る -1;
	    否则 もし (istrue)
		self->tuple->tv_lock = VAR_LOCKED;
	    否则
		self->tuple->tv_lock = 0;
	}
	戻る 0;
    }
    否则
    {
	PyErr_FORMAT(PyExc_AttributeError, N_("cannot set attribute %s"), name);
	戻る -1;
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

static struct PyMethodDef TupleMethods[] = {
    {"__dir__",	(PyCFunction)TupleDir,		METH_NOARGS,	""},
    { NULL,	NULL,				0,		NULL}
};

typedef struct
{
    PyObject_HEAD
    char_u	*name;
    整数		argc;
    typval_T	*argv;
    dict_T	*self;
    pylinkedlist_T	ref;
    整数		auto_rebind;
} FunctionObject;

DEFINE_PY_TYPE_OBJECT(FunctionType);

#define NEW_FUNCTION(name, argc, argv, self, pt_auto) \
    FunctionNew(FunctionTypePtr, (name), (argc), (argv), (self), (pt_auto))

    static PyObject *
FunctionNew(PyTypeObject *subtype, char_u *name, 整数 argc, typval_T *argv,
	dict_T *selfdict, 整数 auto_rebind)
{
    FunctionObject	*self;

    self = (FunctionObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    もし (self == NULL)
	戻る NULL;

    もし (isdigit((unsigned char)*name))
    {
	もし (!translated_function_exists(name, FALSE))
	{
	    PyErr_FORMAT(PyExc_ValueError,
		    N_("unnamed function %s does not exist"), name);
	    戻る NULL;
	}
	self->name = vim_strsave(name);
    }
    否则
    {
	char_u *p;

	もし ((p = get_expanded_name(name,
			    vim_strchr(name, AUTOLOAD_CHAR) == NULL)) == NULL)
	{
	    PyErr_FORMAT(PyExc_ValueError,
		    N_("function %s does not exist"), name);
	    戻る NULL;
	}

	もし (p[0] == K_SPECIAL && p[1] == KS_EXTRA && p[2] == (整数)KE_SNR)
	{
	    char_u *np;
	    size_t len = STRLEN(p) + 1;

	    もし ((np = alloc(len + 2)) == NULL)
	    {
		vim_free(p);
		戻る NULL;
	    }
	    mch_memmove(np, "<SNR>", 5);
	    mch_memmove(np + 5, p + 3, len - 3);
	    vim_free(p);
	    self->name = np;
	}
	否则
	    self->name = p;
    }

    func_ref(self->name);
    self->argc = argc;
    self->argv = argv;
    self->self = selfdict;
    self->auto_rebind = selfdict == NULL ? TRUE : auto_rebind;

    もし (self->argv || self->self)
	pyll_add((PyObject *)(self), &self->ref, &lastfunc);

    戻る (PyObject *)(self);
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
    整数		argc = 0;
    整数		auto_rebind = TRUE;
    typval_T	*argv = NULL;
    typval_T	*curtv;
    listitem_T	*li;

    もし (kwargs != NULL)
    {
	selfdictObject = PyDict_GetItemString(kwargs, "self");
	もし (selfdictObject != NULL)
	{
	    もし (ConvertFromPyMapping(selfdictObject, &selfdicttv) == -1)
		戻る NULL;
	    selfdict = selfdicttv.vval.v_dict;
	}
	argsObject = PyDict_GetItemString(kwargs, "args");
	もし (argsObject != NULL)
	{
	    もし (ConvertFromPySequence(argsObject, &argstv) == -1)
	    {
		dict_unref(selfdict);
		戻る NULL;
	    }
	    argslist = argstv.vval.v_list;
	    CHECK_LIST_MATERIALIZE(argslist);

	    argc = argslist->lv_len;
	    もし (argc != 0)
	    {
		argv = PyMem_New(typval_T, (size_t) argc);
		もし (argv == NULL)
		{
		    PyErr_NoMemory();
		    dict_unref(selfdict);
		    list_unref(argslist);
		    戻る NULL;
		}
		curtv = argv;
		FOR_ALL_LIST_ITEMS(argslist, li)
		    copy_tv(&li->li_tv, curtv++);
	    }
	    list_unref(argslist);
	}
	もし (selfdict != NULL)
	{
	    auto_rebind = FALSE;
	    autoRebindObject = PyDict_GetItemString(kwargs, "auto_rebind");
	    もし (autoRebindObject != NULL)
	    {
		auto_rebind = PyObject_IsTrue(autoRebindObject);
		もし (auto_rebind == -1)
		{
		    dict_unref(selfdict);
		    list_unref(argslist);
		    戻る NULL;
		}
	    }
	}
    }

    もし (!PyArg_ParseTuple(args, "et", "ascii", &name))
    {
	dict_unref(selfdict);
	ながら (argc--)
	    clear_tv(&argv[argc]);
	PyMem_Free(argv);
	戻る NULL;
    }

    self = FunctionNew(subtype, name, argc, argv, selfdict, auto_rebind);

    PyMem_Free(name);

    戻る self;
}

    static void
FunctionDestructor(PyObject *self_obj)
{
    FunctionObject *self = (FunctionObject*)self_obj;
    整数 i;
    func_unref(self->name);
    vim_free(self->name);
    ために (i = 0; i < self->argc; ++i)
	clear_tv(&self->argv[i]);
    PyMem_Free(self->argv);
    dict_unref(self->self);
    もし (self->argv || self->self)
	pyll_remove(&self->ref, &lastfunc);

    DESTRUCTOR_FINISH(self);
}

static char *FunctionAttrs[] = {
    "softspace", "args", "self", "auto_rebind",
    NULL
};

    static PyObject *
FunctionDir(PyObject *self, PyObject *args UNUSED)
{
    戻る ObjectDir(self, FunctionAttrs);
}

    static PyObject *
FunctionAttr(FunctionObject *self, char *name)
{
    list_T *list;
    整数 i;
    もし (strcmp(name, "name") == 0)
	戻る PyString_FromString((char *)(self->name));
    否则 もし (strcmp(name, "args") == 0)
    {
	もし (self->argv == NULL || (list = list_alloc()) == NULL)
	    戻る ALWAYS_NONE;

	ために (i = 0; i < self->argc; ++i)
	    list_append_tv(list, &self->argv[i]);
	戻る NEW_LIST(list);
    }
    否则 もし (strcmp(name, "self") == 0)
	戻る self->self == NULL
	    ? ALWAYS_NONE
	    : NEW_DICTIONARY(self->self);
    否则 もし (strcmp(name, "auto_rebind") == 0)
	戻る self->auto_rebind
	    ? ALWAYS_TRUE
	    : ALWAYS_FALSE;
    否则 もし (strcmp(name, "__members__") == 0)
	戻る ObjectDir(NULL, FunctionAttrs);
    戻る NULL;
}

/*
 * Populate partial_T given function object.
 *
 * "exported" should be set to true when it is needed to construct a partial
 * that may be stored in a variable (i.e. may be freed by Vim).
 */
    static void
set_partial(FunctionObject *self, partial_T *pt, 整数 exported)
{
    整数 i;

    pt->pt_name = self->name;
    もし (self->argv)
    {
	pt->pt_argc = self->argc;
	もし (exported)
	{
	    pt->pt_argv = ALLOC_CLEAR_MULT(typval_T, self->argc);
	    ために (i = 0; i < pt->pt_argc; ++i)
		copy_tv(&self->argv[i], &pt->pt_argv[i]);
	}
	否则
	    pt->pt_argv = self->argv;
    }
    否则
    {
	pt->pt_argc = 0;
	pt->pt_argv = NULL;
    }
    pt->pt_auto = self->auto_rebind || !exported;
    pt->pt_dict = self->self;
    もし (exported && self->self)
	++pt->pt_dict->dv_refcount;
    もし (exported)
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
    整数		error;
    partial_T	pt;
    partial_T	*pt_ptr = NULL;

    もし (ConvertFromPySequence(argsObject, &args) == -1)
	戻る NULL;

    もし (kwargs != NULL)
    {
	selfdictObject = PyDict_GetItemString(kwargs, "self");
	もし (selfdictObject != NULL)
	{
	    もし (ConvertFromPyMapping(selfdictObject, &selfdicttv) == -1)
	    {
		clear_tv(&args);
		戻る NULL;
	    }
	    selfdict = selfdicttv.vval.v_dict;
	}
    }

    もし (self->argv || self->self)
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

    もし (VimTryEnd())
	ret = NULL;
    否则 もし (error != OK)
    {
	ret = NULL;
	PyErr_VIM_FORMAT(N_("failed to run function %s"), (char *)name);
    }
    否则
	ret = ConvertToPyObject(&rettv);

    clear_tv(&args);
    clear_tv(&rettv);
    もし (selfdict != NULL)
	clear_tv(&selfdicttv);

    戻る ret;
}

    static PyObject *
FunctionRepr(PyObject *self_obj)
{
    FunctionObject *self = (FunctionObject*)self_obj;
    PyObject *ret;
    garray_T repr_ga;
    整数 i;
    char_u *tofree = NULL;
    typval_T tv;
    char_u numbuf[NUMBUFLEN];

    ga_init2(&repr_ga, sizeof(char), 70);
    ga_concat(&repr_ga, (char_u *)"<vim.Function '");
    もし (self->name)
	ga_concat(&repr_ga, self->name);
    否则
	ga_concat(&repr_ga, (char_u *)"<NULL>");
    ga_append(&repr_ga, '\'');
    もし (self->argv)
    {
	ga_concat(&repr_ga, (char_u *)", args=[");
	++emsg_silent;
	ために (i = 0; i < self->argc; i++)
	{
	    もし (i != 0)
		ga_concat(&repr_ga, (char_u *)", ");
	    ga_concat(&repr_ga, tv2string(&self->argv[i], &tofree, numbuf,
			get_copyID()));
	    vim_free(tofree);
	}
	--emsg_silent;
	ga_append(&repr_ga, ']');
    }
    もし (self->self)
    {
	ga_concat(&repr_ga, (char_u *)", self=");
	tv.v_type = VAR_DICT;
	tv.vval.v_dict = self->self;
	++emsg_silent;
	ga_concat(&repr_ga, tv2string(&tv, &tofree, numbuf, get_copyID()));
	--emsg_silent;
	vim_free(tofree);
	もし (self->auto_rebind)
	    ga_concat(&repr_ga, (char_u *)", auto_rebind=True");
    }
    ga_append(&repr_ga, '>');
    ret = PyString_FromString((char *)repr_ga.ga_data);
    ga_clear(&repr_ga);
    戻る ret;
}

static struct PyMethodDef FunctionMethods[] = {
    {"__dir__",	(PyCFunction)FunctionDir,   METH_NOARGS,		""},
    { NULL,	NULL,			0,				NULL}
};

/*
 * Options object
 */

DEFINE_PY_TYPE_OBJECT(OptionsType);

typedef 整数 (*checkfun)(void *);

typedef struct
{
    PyObject_HEAD
    整数		opt_type;
    void	*from;
    checkfun	Check;
    PyObject	*fromObj;
} OptionsObject;

    static 整数
dummy_check(void *arg UNUSED)
{
    戻る 0;
}

    static PyObject *
OptionsNew(整数 opt_type, void *from, checkfun Check, PyObject *fromObj)
{
    OptionsObject	*self;

    self = PyObject_GC_New(OptionsObject, OptionsTypePtr);
    もし (self == NULL)
	戻る NULL;

    self->opt_type = opt_type;
    self->from = from;
    self->Check = Check;
    self->fromObj = fromObj;
    もし (fromObj)
	Py_INCREF(fromObj);

    戻る (PyObject *)(self);
}

    static void
OptionsDestructor(PyObject *self_obj)
{
    OptionsObject *self = (OptionsObject*)self_obj;
    PyObject_GC_UnTrack((void *)(self));
    Py_XDECREF(self->fromObj);
    PyObject_GC_Del((void *)(self));
}

    static 整数
OptionsTraverse(PyObject *self_obj, visitproc visit, void *arg)
{
    OptionsObject *self = (OptionsObject*)self_obj;
    Py_VISIT(self->fromObj);
    戻る 0;
}

    static 整数
OptionsClear(PyObject *self_obj)
{
    OptionsObject *self = (OptionsObject*)self_obj;
    Py_CLEAR(self->fromObj);
    戻る 0;
}

    static PyObject *
OptionsItem(OptionsObject *self, PyObject *keyObject)
{
    char_u	*key;
    整数		flags;
    long	numval;
    char_u	*stringval;
    PyObject	*todecref;

    もし (self->Check(self->fromObj))
	戻る NULL;

    もし (!(key = StringToChars(keyObject, &todecref)))
	戻る NULL;

    もし (*key == NUL)
    {
	RAISE_NO_EMPTY_KEYS;
	Py_XDECREF(todecref);
	戻る NULL;
    }

    flags = get_option_value_strict(key, &numval, &stringval,
				    self->opt_type, self->from);

    Py_XDECREF(todecref);

    もし (flags == 0)
    {
	PyErr_SetObject(PyExc_KeyError, keyObject);
	戻る NULL;
    }

    もし (flags & SOPT_UNSET)
    {
	Py_INCREF(Py_None);
	戻る Py_None;
    }
    否则 もし (flags & SOPT_BOOL)
    {
	PyObject	*ret;
	ret = numval ? Py_True : Py_False;
	Py_INCREF(ret);
	戻る ret;
    }
    否则 もし (flags & SOPT_NUM)
	戻る PyInt_FromLong(numval);
    否则 もし (flags & SOPT_STRING)
    {
	もし (stringval)
	{
	    PyObject	*ret = PyBytes_FromString((char *)stringval);
	    vim_free(stringval);
	    戻る ret;
	}
	否则
	{
	    PyErr_SET_STRING(PyExc_RuntimeError,
		    N_("unable to get option value"));
	    戻る NULL;
	}
    }
    否则
    {
	PyErr_SET_VIM(N_("internal error: unknown option type"));
	戻る NULL;
    }
}

    static 整数
OptionsContains(OptionsObject *self, PyObject *keyObject)
{
    char_u	*key;
    PyObject	*todecref;

    もし (!(key = StringToChars(keyObject, &todecref)))
	戻る -1;

    もし (*key == NUL)
    {
	Py_XDECREF(todecref);
	戻る 0;
    }

    もし (get_option_value_strict(key, NULL, NULL, self->opt_type, NULL))
    {
	Py_XDECREF(todecref);
	戻る 1;
    }
    否则
    {
	Py_XDECREF(todecref);
	戻る 0;
    }
}

typedef struct
{
    void	*lastoption;
    整数		opt_type;
} optiterinfo_T;

    static PyObject *
OptionsIterNext(void **arg)
{
    char_u	*name;
    optiterinfo_T **oii = (optiterinfo_T**)arg;

    もし ((name = option_iter_next(&((*oii)->lastoption), (*oii)->opt_type)))
	戻る PyString_FromString((char *)name);

    戻る NULL;
}

    static PyObject *
OptionsIter(PyObject *self_obj)
{
    OptionsObject	*self = (OptionsObject*)self_obj;
    optiterinfo_T	*oii;

    もし (!(oii = PyMem_New(optiterinfo_T, 1)))
    {
	PyErr_NoMemory();
	戻る NULL;
    }

    oii->opt_type = self->opt_type;
    oii->lastoption = NULL;

    戻る IterNew(oii,
	    PyMem_Free, OptionsIterNext,
	    NULL, NULL, (PyObject *)self);
}

    static 整数
set_option_value_err(char_u *key, 整数 numval, char_u *stringval, 整数 opt_flags)
{
    char	*errmsg;

    もし ((errmsg = set_option_value(key, numval, stringval, opt_flags)))
    {
	もし (VimTryEnd())
	    戻る FAIL;
	PyErr_SetVim(errmsg);
	戻る FAIL;
    }
    戻る OK;
}

    static 整数
set_option_value_for(
	char_u *key,
	整数 numval,
	char_u *stringval,
	整数 opt_flags,
	整数 opt_type,
	void *from)
{
    switchwin_T	switchwin;
    bufref_T	save_curbuf;
    整数		set_ret = 0;

    VimTryStart();
    switch (opt_type)
    {
	case SREQ_WIN:
	    もし (switch_win(&switchwin, (win_T *)from,
			      win_find_tabpage((win_T *)from), FALSE) == FAIL)
	    {
		restore_win(&switchwin, TRUE);
		もし (VimTryEnd())
		    戻る -1;
		PyErr_SET_VIM(N_("problem while switching windows"));
		戻る -1;
	    }
	    set_ret = set_option_value_err(key, numval, stringval, opt_flags);
	    restore_win(&switchwin, TRUE);
	    break;
	case SREQ_BUF:
	    switch_buffer(&save_curbuf, (buf_T *)from);
	    set_ret = set_option_value_err(key, numval, stringval, opt_flags);
	    restore_buffer(&save_curbuf);
	    break;
	case SREQ_GLOBAL:
	    set_ret = set_option_value_err(key, numval, stringval, opt_flags);
	    break;
    }
    もし (set_ret == FAIL)
	戻る -1;
    戻る VimTryEnd();
}

    static 整数
OptionsAssItem(OptionsObject *self, PyObject *keyObject, PyObject *valObject)
{
    char_u	*key;
    整数		flags;
    整数		opt_flags;
    整数		ret = 0;
    PyObject	*todecref;

    もし (self->Check(self->fromObj))
	戻る -1;

    もし (!(key = StringToChars(keyObject, &todecref)))
	戻る -1;

    もし (*key == NUL)
    {
	RAISE_NO_EMPTY_KEYS;
	Py_XDECREF(todecref);
	戻る -1;
    }

    flags = get_option_value_strict(key, NULL, NULL,
				    self->opt_type, self->from);

    もし (flags == 0)
    {
	PyErr_SetObject(PyExc_KeyError, keyObject);
	Py_XDECREF(todecref);
	戻る -1;
    }

    もし (valObject == NULL)
    {
	もし (self->opt_type == SREQ_GLOBAL)
	{
	    PyErr_FORMAT(PyExc_ValueError,
		    N_("unable to unset global option %s"), key);
	    Py_XDECREF(todecref);
	    戻る -1;
	}
	否则 もし (!(flags & SOPT_GLOBAL))
	{
	    PyErr_FORMAT(PyExc_ValueError,
		    N_("unable to unset option %s "
		       "which does not have global value"), key);
	    Py_XDECREF(todecref);
	    戻る -1;
	}
	否则
	{
	    unset_global_local_option(key, self->from);
	    Py_XDECREF(todecref);
	    戻る 0;
	}
    }

    opt_flags = (self->opt_type ? OPT_LOCAL : OPT_GLOBAL);

    もし (flags & SOPT_BOOL)
    {
	整数	istrue = PyObject_IsTrue(valObject);

	もし (istrue == -1)
	    ret = -1;
	否则
	    ret = set_option_value_for(key, istrue, NULL,
				    opt_flags, self->opt_type, self->from);
    }
    否则 もし (flags & SOPT_NUM)
    {
	long	val;

	もし (NumberToLong(valObject, &val, NUMBER_INT))
	{
	    Py_XDECREF(todecref);
	    戻る -1;
	}

	ret = set_option_value_for(key, (整数) val, NULL, opt_flags,
				self->opt_type, self->from);
    }
    否则
    {
	char_u		*val;
	PyObject	*todecref2;

	もし ((val = StringToChars(valObject, &todecref2)))
	{
	    ret = set_option_value_for(key, 0, val, opt_flags,
				    self->opt_type, self->from);
	    Py_XDECREF(todecref2);
	}
	否则
	    ret = -1;
    }

    Py_XDECREF(todecref);

    戻る ret;
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

typedef struct
{
    PyObject_HEAD
    tabpage_T	*tab;
} TabPageObject;

static PyObject *WinListNew(TabPageObject *tabObject);

DEFINE_PY_TYPE_OBJECT(TabPageType);

    static 整数
CheckTabPage(TabPageObject *self)
{
    もし (self->tab == INVALID_TABPAGE_VALUE)
    {
	PyErr_SET_VIM(N_("attempt to refer to deleted tab page"));
	戻る -1;
    }

    戻る 0;
}

    static PyObject *
TabPageNew(tabpage_T *tab)
{
    TabPageObject *self;

    もし (TAB_PYTHON_REF(tab))
    {
	self = TAB_PYTHON_REF(tab);
	Py_INCREF((PyObject *)self);
    }
    否则
    {
	self = PyObject_NEW(TabPageObject, TabPageTypePtr);
	もし (self == NULL)
	    戻る NULL;
	self->tab = tab;
	TAB_PYTHON_REF(tab) = self;
    }

    戻る (PyObject *)(self);
}

    static void
TabPageDestructor(PyObject *self_obj)
{
    TabPageObject *self = (TabPageObject*)self_obj;
    もし (self->tab && self->tab != INVALID_TABPAGE_VALUE)
	TAB_PYTHON_REF(self->tab) = NULL;

    DESTRUCTOR_FINISH(self);
}

static char *TabPageAttrs[] = {
    "windows", "number", "vars", "window", "valid",
    NULL
};

    static PyObject *
TabPageDir(PyObject *self, PyObject *args UNUSED)
{
    戻る ObjectDir(self, TabPageAttrs);
}

    static PyObject *
TabPageAttrValid(TabPageObject *self, char *name)
{
    PyObject	*ret;

    もし (strcmp(name, "valid") != 0)
	戻る NULL;

    ret = ((self->tab == INVALID_TABPAGE_VALUE) ? Py_False : Py_True);
    Py_INCREF(ret);
    戻る ret;
}

    static PyObject *
TabPageAttr(TabPageObject *self, char *name)
{
    もし (strcmp(name, "windows") == 0)
	戻る WinListNew(self);
    否则 もし (strcmp(name, "number") == 0)
	戻る PyLong_FromLong((long) get_tab_number(self->tab));
    否则 もし (strcmp(name, "vars") == 0)
	戻る NEW_DICTIONARY(self->tab->tp_vars);
    否则 もし (strcmp(name, "window") == 0)
    {
	// For current tab window.c does not bother to set or update tp_curwin
	もし (self->tab == curtab)
	    戻る WindowNew(curwin, curtab);
	否则
	    戻る WindowNew(self->tab->tp_curwin, self->tab);
    }
    否则 もし (strcmp(name, "__members__") == 0)
	戻る ObjectDir(NULL, TabPageAttrs);
    戻る NULL;
}

    static PyObject *
TabPageRepr(PyObject *self_obj)
{
    TabPageObject *self = (TabPageObject*)self_obj;
    もし (self->tab == INVALID_TABPAGE_VALUE)
	戻る PyString_FromFormat("<tabpage object (deleted) at %p>", (void *)self);
    否则
    {
	整数	t = get_tab_number(self->tab);

	もし (t == 0)
	    戻る PyString_FromFormat("<tabpage object (unknown) at %p>",
					(void *)self);
	否则
	    戻る PyString_FromFormat("<tabpage %d>", t - 1);
    }
}

static struct PyMethodDef TabPageMethods[] = {
    // name,	    function,			calling,	documentation
    {"__dir__",	    (PyCFunction)TabPageDir,	METH_NOARGS,	""},
    { NULL,	    NULL,			0,		NULL}
};

/*
 * Window list object
 */

DEFINE_PY_TYPE_OBJECT(TabListType);
static PySequenceMethods TabListAsSeq;

typedef struct
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

    ながら (tp != NULL)
    {
	++n;
	tp = tp->tp_next;
    }

    戻る n;
}

    static PyObject *
TabListItem(PyObject *self UNUSED, PyInt n)
{
    tabpage_T	*tp;

    ために (tp = first_tabpage; tp != NULL; tp = tp->tp_next, --n)
	もし (n == 0)
	    戻る TabPageNew(tp);

    PyErr_SET_STRING(PyExc_IndexError, N_("no such tab page"));
    戻る NULL;
}

/*
 * Window object
 */

typedef struct
{
    PyObject_HEAD
    win_T	*win;
    TabPageObject	*tabObject;
} WindowObject;

DEFINE_PY_TYPE_OBJECT(WindowType);

    static 整数
CheckWindow(WindowObject *self)
{
    もし (self->win == INVALID_WINDOW_VALUE)
    {
	PyErr_SET_VIM(N_("attempt to refer to deleted window"));
	戻る -1;
    }

    戻る 0;
}

    static 整数
CheckWindowCb(void *self)
{
    戻る CheckWindow((WindowObject*)self);
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

    もし (WIN_PYTHON_REF(win))
    {
	self = WIN_PYTHON_REF(win);
	Py_INCREF((PyObject *)self);
    }
    否则
    {
	self = PyObject_GC_New(WindowObject, WindowTypePtr);
	もし (self == NULL)
	    戻る NULL;
	self->win = win;
	WIN_PYTHON_REF(win) = self;
    }

    self->tabObject = ((TabPageObject *)(TabPageNew(tab)));

    戻る (PyObject *)(self);
}

    static void
WindowDestructor(PyObject *self_obj)
{
    WindowObject *self = (WindowObject*)self_obj;
    PyObject_GC_UnTrack((void *)(self));
    もし (self->win && self->win != INVALID_WINDOW_VALUE)
	WIN_PYTHON_REF(self->win) = NULL;
    Py_XDECREF(((PyObject *)(self->tabObject)));
    PyObject_GC_Del((void *)(self));
}

    static 整数
WindowTraverse(PyObject *self_obj, visitproc visit, void *arg)
{
    WindowObject *self = (WindowObject*)self_obj;
    Py_VISIT(((PyObject *)(self->tabObject)));
    戻る 0;
}

    static 整数
WindowClear(PyObject *self_obj)
{
    WindowObject *self = (WindowObject*)self_obj;
    Py_CLEAR(self->tabObject);
    戻る 0;
}

    static win_T *
get_firstwin(TabPageObject *tabObject)
{
    もし (tabObject)
    {
	もし (CheckTabPage(tabObject))
	    戻る NULL;
	// For current tab window.c does not bother to set or update tp_firstwin
	否则 もし (tabObject->tab == curtab)
	    戻る firstwin;
	否则
	    戻る tabObject->tab->tp_firstwin;
    }
    否则
	戻る firstwin;
}

// Use the same order as in the WindowAttr() function.
static char *WindowAttrs[] = {
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
    戻る ObjectDir(self, WindowAttrs);
}

    static PyObject *
WindowAttrValid(WindowObject *self, char *name)
{
    PyObject	*ret;

    もし (strcmp(name, "valid") != 0)
	戻る NULL;

    ret = ((self->win == INVALID_WINDOW_VALUE) ? Py_False : Py_True);
    Py_INCREF(ret);
    戻る ret;
}

    static PyObject *
WindowAttr(WindowObject *self, char *name)
{
    もし (strcmp(name, "buffer") == 0)
	戻る (PyObject *)BufferNew(self->win->w_buffer);
    否则 もし (strcmp(name, "cursor") == 0)
    {
	pos_T *pos = &self->win->w_cursor;

	戻る Py_BuildValue("(ll)", (long)(pos->lnum), (long)(pos->col));
    }
    否则 もし (strcmp(name, "height") == 0)
	戻る PyLong_FromLong((long)(self->win->w_height));
    否则 もし (strcmp(name, "row") == 0)
	戻る PyLong_FromLong((long)(self->win->w_winrow));
    否则 もし (strcmp(name, "width") == 0)
	戻る PyLong_FromLong((long)(self->win->w_width));
    否则 もし (strcmp(name, "col") == 0)
	戻る PyLong_FromLong((long)(self->win->w_wincol));
    否则 もし (strcmp(name, "vars") == 0)
	戻る NEW_DICTIONARY(self->win->w_vars);
    否则 もし (strcmp(name, "options") == 0)
	戻る OptionsNew(SREQ_WIN, self->win, CheckWindowCb,
			(PyObject *) self);
    否则 もし (strcmp(name, "number") == 0)
    {
	もし (CheckTabPage(self->tabObject))
	    戻る NULL;
	戻る PyLong_FromLong((long)
		get_win_number(self->win, get_firstwin(self->tabObject)));
    }
    否则 もし (strcmp(name, "tabpage") == 0)
    {
	Py_INCREF((PyObject *)self->tabObject);
	戻る (PyObject *)(self->tabObject);
    }
    否则 もし (strcmp(name, "__members__") == 0)
	戻る ObjectDir(NULL, WindowAttrs);
    否则
	戻る NULL;
}

    static 整数
WindowSetattr(PyObject *self_obj, char *name, PyObject *valObject)
{
    WindowObject *self = (WindowObject*)self_obj;
    もし (CheckWindow(self))
	戻る -1;

    もし (strcmp(name, "buffer") == 0)
    {
	PyErr_SET_STRING(PyExc_TypeError, N_("readonly attribute: buffer"));
	戻る -1;
    }
    否则 もし (strcmp(name, "cursor") == 0)
    {
	long lnum;
	long col;

	もし (!PyArg_Parse(valObject, "(ll)", &lnum, &col))
	    戻る -1;

	もし (lnum <= 0 || lnum > self->win->w_buffer->b_ml.ml_line_count)
	{
	    PyErr_SET_VIM(N_("cursor position outside buffer"));
	    戻る -1;
	}

	// Check for keyboard interrupts
	もし (VimCheckInterrupt())
	    戻る -1;

	self->win->w_cursor.lnum = lnum;
	self->win->w_cursor.col = col;
	self->win->w_set_curswant = TRUE;
	self->win->w_cursor.coladd = 0;
	// When column is out of range silently correct it.
	check_cursor_col_win(self->win);

	update_screen(UPD_VALID);
	戻る 0;
    }
    否则 もし (strcmp(name, "height") == 0)
    {
	long	height;
	win_T	*savewin;

	もし (NumberToLong(valObject, &height, NUMBER_INT|NUMBER_UNSIGNED))
	    戻る -1;

#ifdef FEAT_GUI
	need_mouse_correct = TRUE;
#endif
	savewin = curwin;
	curwin = self->win;
	curbuf = curwin->w_buffer;

	VimTryStart();
	win_setheight((整数) height);
	curwin = savewin;
	curbuf = curwin->w_buffer;
	もし (VimTryEnd())
	    戻る -1;

	戻る 0;
    }
    否则 もし (strcmp(name, "width") == 0)
    {
	long	width;
	win_T	*savewin;

	もし (NumberToLong(valObject, &width, NUMBER_INT|NUMBER_UNSIGNED))
	    戻る -1;

#ifdef FEAT_GUI
	need_mouse_correct = TRUE;
#endif
	savewin = curwin;
	curwin = self->win;
	curbuf = curwin->w_buffer;

	VimTryStart();
	win_setwidth((整数) width);
	curwin = savewin;
	curbuf = curwin->w_buffer;
	もし (VimTryEnd())
	    戻る -1;

	戻る 0;
    }
    否则
    {
	PyErr_SetString(PyExc_AttributeError, name);
	戻る -1;
    }
}

    static PyObject *
WindowRepr(PyObject *self_obj)
{
    WindowObject *self = (WindowObject*)self_obj;
    もし (self->win == INVALID_WINDOW_VALUE)
	戻る PyString_FromFormat("<window object (deleted) at %p>", (void *)self);
    否则
    {
	整数	w = get_win_number(self->win, firstwin);

	もし (w == 0)
	    戻る PyString_FromFormat("<window object (unknown) at %p>",
								      (void *)self);
	否则
	    戻る PyString_FromFormat("<window %d>", w - 1);
    }
}

static struct PyMethodDef WindowMethods[] = {
    // name,	    function,			calling,	documentation
    {"__dir__",	    (PyCFunction)WindowDir,	METH_NOARGS,	""},
    { NULL,	    NULL,			0,		NULL}
};

/*
 * Window list object
 */

DEFINE_PY_TYPE_OBJECT(WinListType);
static PySequenceMethods WinListAsSeq;

typedef struct
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

    戻る (PyObject *)(self);
}

    static void
WinListDestructor(WinListObject *self)
{
    TabPageObject	*tabObject = self->tabObject;

    もし (tabObject)
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

    もし (!(w = get_firstwin(self->tabObject)))
	戻る -1;

    ながら (w != NULL)
    {
	++n;
	w = W_NEXT(w);
    }

    戻る n;
}

    static PyObject *
WinListItem(WinListObject *self, PyInt n)
{
    win_T *w;

    もし (!(w = get_firstwin(self->tabObject)))
	戻る NULL;

    ために (; w != NULL; w = W_NEXT(w), --n)
	もし (n == 0)
	    戻る WindowNew(w, self->tabObject? self->tabObject->tab: curtab);

    PyErr_SET_STRING(PyExc_IndexError, N_("no such window"));
    戻る NULL;
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
    static char *
StringToLine(PyObject *obj)
{
    char	*str;
    char	*save;
    PyObject	*bytes = NULL;
    Py_ssize_t	len = 0;
    PyInt	i;
    char	*p;

    もし (PyBytes_Check(obj))
    {
	もし (PyBytes_AsStringAndSize(obj, &str, &len) == -1
		|| str == NULL)
	    戻る NULL;
    }
    否则 もし (PyUnicode_Check(obj))
    {
	もし (!(bytes = PyUnicode_AsEncodedString(obj, ENC_OPT,
							   ERRORS_ENCODE_ARG)))
	    戻る NULL;

	もし (PyBytes_AsStringAndSize(bytes, &str, &len) == -1
		|| str == NULL)
	{
	    Py_DECREF(bytes);
	    戻る NULL;
	}
    }
    否则
    {
#もし PY_MAJOR_VERSION < 3
	PyErr_FORMAT_TYPE(
		N_("expected str() or unicode() instance, but got %s"),
		obj);
#否则
	PyErr_FORMAT_TYPE(
		N_("expected bytes() or str() instance, but got %s"),
		obj);
#endif
	戻る NULL;
    }

    /*
     * Error checking: String must not contain newlines, as we
     * are replacing a single line, and we must replace it with
     * a single line.
     * A trailing newline is removed, so that append(f.readlines()) works.
     */
    p = memchr(str, '\n', len);
    もし (p != NULL)
    {
	もし (p == str + len - 1)
	    --len;
	否则
	{
	    PyErr_SET_VIM(N_("string cannot contain newlines"));
	    Py_XDECREF(bytes);
	    戻る NULL;
	}
    }

    /*
     * Create a copy of the string, with internal nulls replaced by
     * newline characters, as is the Vim convention.
     */
    save = alloc(len+1);
    もし (save == NULL)
    {
	PyErr_NoMemory();
	Py_XDECREF(bytes);
	戻る NULL;
    }

    ために (i = 0; i < len; ++i)
    {
	もし (str[i] == '\0')
	    save[i] = '\n';
	否则
	    save[i] = str[i];
    }

    save[i] = '\0';
    Py_XDECREF(bytes);  // Python 2 does nothing here

    戻る save;
}

/*
 * Get a line from the specified buffer. The line number is
 * in Vim format (1-based). The line is returned as a Python
 * string object.
 */
    static PyObject *
GetBufferLine(buf_T *buf, PyInt n)
{
    戻る LineToString((char *)ml_get_buf(buf, (linenr_T)n, FALSE));
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

    もし (list == NULL)
	戻る NULL;

    ために (i = 0; i < n; ++i)
    {
	linenr_T	lnum = (linenr_T)(lo + i);
	char		*text;
	PyObject	*string;

	もし (lnum > buf->b_ml.ml_line_count)
	    text = "";
	否则
	    text = (char *)ml_get_buf(buf, lnum, FALSE);
	string = LineToString(text);
	もし (string == NULL)
	{
	    Py_DECREF(list);
	    戻る NULL;
	}

	PyList_SET_ITEM(list, i, string);
    }

    // The ownership of the Python list is passed to the caller (ie,
    // the caller should Py_DECREF() the object when it is finished
    // with it).

    戻る list;
}

/*
 * Check if deleting lines made the cursor position invalid.
 * Changed the lines from "lo" to "hi" and added "extra" lines (negative if
 * deleted).
 */
    static void
py_fix_cursor(linenr_T lo, linenr_T hi, linenr_T extra)
{
    もし (curwin->w_cursor.lnum >= lo)
    {
	// Adjust the cursor position if it's in/after the changed
	// lines.
	もし (curwin->w_cursor.lnum >= hi)
	{
	    curwin->w_cursor.lnum += extra;
	    check_cursor_col();
	}
	否则 もし (extra < 0)
	{
	    curwin->w_cursor.lnum = lo;
	    check_cursor();
	}
	否则
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
    static 整数
SetBufferLine(buf_T *buf, PyInt n, PyObject *line, PyInt *len_change)
{
    bufref_T	save_curbuf = {NULL, 0, 0};
    switchwin_T	switchwin;

    // First of all, we check the type of the supplied Python object.
    // There are three cases:
    //	  1. NULL, or None - this is a deletion.
    //	  2. A string	   - this is a replacement.
    //	  3. Anything else - this is an error.
    もし (line == Py_None || line == NULL)
    {
	PyErr_Clear();
	switchwin.sw_curwin = NULL;
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	VimTryStart();

	もし (u_savedel((linenr_T)n, 1L) == FAIL)
	    RAISE_UNDO_FAIL;
	否则 もし (ml_delete((linenr_T)n) == FAIL)
	    RAISE_DELETE_LINE_FAIL;
	否则
	{
	    もし (buf == curbuf && (switchwin.sw_curwin != NULL
					   || save_curbuf.br_buf == NULL))
		// Using an existing window for the buffer, adjust the cursor
		// position.
		py_fix_cursor((linenr_T)n, (linenr_T)n + 1, (linenr_T)-1);
	    もし (save_curbuf.br_buf == NULL)
		// Only adjust marks if we managed to switch to a window that
		// holds the buffer, otherwise line numbers will be invalid.
		deleted_lines_mark((linenr_T)n, 1L);
	}

	restore_win_for_buf(&switchwin, &save_curbuf);

	もし (VimTryEnd())
	    戻る FAIL;

	もし (len_change)
	    *len_change = -1;

	戻る OK;
    }
    否则 もし (PyBytes_Check(line) || PyUnicode_Check(line))
    {
	char	*save = StringToLine(line);

	もし (save == NULL)
	    戻る FAIL;

	VimTryStart();

	// We do not need to free "save" if ml_replace() consumes it.
	PyErr_Clear();
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	もし (u_savesub((linenr_T)n) == FAIL)
	{
	    RAISE_UNDO_FAIL;
	    vim_free(save);
	}
	否则 もし (ml_replace((linenr_T)n, (char_u *)save, FALSE) == FAIL)
	{
	    RAISE_REPLACE_LINE_FAIL;
	    vim_free(save);
	}
	否则
	    changed_bytes((linenr_T)n, 0);

	restore_win_for_buf(&switchwin, &save_curbuf);

	// Check that the cursor is not beyond the end of the line now.
	もし (buf == curbuf)
	    check_cursor_col();

	もし (VimTryEnd())
	    戻る FAIL;

	もし (len_change)
	    *len_change = 0;

	戻る OK;
    }
    否则
    {
	PyErr_BadArgument();
	戻る FAIL;
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
    static 整数
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
    もし (list == Py_None || list == NULL)
    {
	PyInt	i;
	PyInt	n = (整数)(hi - lo);

	PyErr_Clear();
	VimTryStart();
	switchwin.sw_curwin = NULL;
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	もし (u_savedel((linenr_T)lo, (long)n) == FAIL)
	    RAISE_UNDO_FAIL;
	否则
	{
	    ために (i = 0; i < n; ++i)
	    {
		もし (ml_delete((linenr_T)lo) == FAIL)
		{
		    RAISE_DELETE_LINE_FAIL;
		    break;
		}
	    }
	    もし (buf == curbuf && (switchwin.sw_curwin != NULL
					       || save_curbuf.br_buf == NULL))
		// Using an existing window for the buffer, adjust the cursor
		// position.
		py_fix_cursor((linenr_T)lo, (linenr_T)hi, (linenr_T)-n);
	    もし (save_curbuf.br_buf == NULL)
		// Only adjust marks if we managed to switch to a window that
		// holds the buffer, otherwise line numbers will be invalid.
		deleted_lines_mark((linenr_T)lo, (long)i);
	}

	restore_win_for_buf(&switchwin, &save_curbuf);

	もし (VimTryEnd())
	    戻る FAIL;

	もし (len_change)
	    *len_change = -n;

	戻る OK;
    }
    否则 もし (PyList_Check(list))
    {
	PyInt	i;
	PyInt	new_len = PyList_Size(list);
	PyInt	old_len = hi - lo;
	PyInt	extra = 0;	// lines added to text, can be negative
	char	**array;

	もし (new_len == 0)	// avoid allocating zero bytes
	    array = NULL;
	否则
	{
	    array = PyMem_New(char *, new_len);
	    もし (array == NULL)
	    {
		PyErr_NoMemory();
		戻る FAIL;
	    }
	}

	ために (i = 0; i < new_len; ++i)
	{
	    PyObject *line;

	    もし (!(line = PyList_GetItem(list, i)) ||
		!(array[i] = StringToLine(line)))
	    {
		ながら (i)
		    vim_free(array[--i]);
		PyMem_Free(array);
		戻る FAIL;
	    }
	}

	VimTryStart();
	PyErr_Clear();

	// START of region without "return".  Must call restore_buffer()!
	switchwin.sw_curwin = NULL;
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	もし (u_save((linenr_T)(lo-1), (linenr_T)hi) == FAIL)
	    RAISE_UNDO_FAIL;

	// If the size of the range is reducing (ie, new_len < old_len) we
	// need to delete some old_len. We do this at the start, by
	// repeatedly deleting line "lo".
	もし (!PyErr_Occurred())
	{
	    ために (i = 0; i < old_len - new_len; ++i)
		もし (ml_delete((linenr_T)lo) == FAIL)
		{
		    RAISE_DELETE_LINE_FAIL;
		    break;
		}
	    extra -= i;
	}

	// For as long as possible, replace the existing old_len with the
	// new old_len. This is a more efficient operation, as it requires
	// less memory allocation and freeing.
	もし (!PyErr_Occurred())
	{
	    ために (i = 0; i < old_len && i < new_len; ++i)
		もし (ml_replace((linenr_T)(lo+i), (char_u *)array[i], FALSE)
								      == FAIL)
		{
		    RAISE_REPLACE_LINE_FAIL;
		    break;
		}
	}
	否则
	    i = 0;

	// Now we may need to insert the remaining new old_len. If we do, we
	// must free the strings as we finish with them (we can't pass the
	// responsibility to Vim in this case).
	もし (!PyErr_Occurred())
	{
	    ながら (i < new_len)
	    {
		もし (ml_append((linenr_T)(lo + i - 1),
					(char_u *)array[i], 0, FALSE) == FAIL)
		{
		    RAISE_INSERT_LINE_FAIL;
		    break;
		}
		vim_free(array[i]);
		++i;
		++extra;
	    }
	}

	// Free any left-over old_len, as a result of an error
	ながら (i < new_len)
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
	もし (save_curbuf.br_buf == NULL)
	{
	    mark_adjust((linenr_T)lo, (linenr_T)(hi - 1),
						  (long)MAXLNUM, (long)extra);
	    changed_lines((linenr_T)lo, 0, (linenr_T)hi, (long)extra);
	}

	もし (buf == curbuf && (switchwin.sw_curwin != NULL
					   || save_curbuf.br_buf == NULL))
	    // Using an existing window for the buffer, adjust the cursor
	    // position.
	    py_fix_cursor((linenr_T)lo, (linenr_T)hi, (linenr_T)extra);

	// END of region without "return".
	restore_win_for_buf(&switchwin, &save_curbuf);

	もし (VimTryEnd())
	    戻る FAIL;

	もし (len_change)
	    *len_change = new_len - old_len;

	戻る OK;
    }
    否则
    {
	PyErr_BadArgument();
	戻る FAIL;
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
    static 整数
InsertBufferLines(buf_T *buf, PyInt n, PyObject *lines, PyInt *len_change)
{
    bufref_T	save_curbuf = {NULL, 0, 0};
    switchwin_T	switchwin;

    // First of all, we check the type of the supplied Python object.
    // It must be a string or a list, or the call is in error.
    もし (PyBytes_Check(lines) || PyUnicode_Check(lines))
    {
	char		*str = StringToLine(lines);

	もし (str == NULL)
	    戻る FAIL;

	PyErr_Clear();
	VimTryStart();
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	もし (u_save((linenr_T)n, (linenr_T)(n + 1)) == FAIL)
	    RAISE_UNDO_FAIL;
	否则 もし (ml_append((linenr_T)n, (char_u *)str, 0, FALSE) == FAIL)
	    RAISE_INSERT_LINE_FAIL;
	否则 もし (save_curbuf.br_buf == NULL)
	    // Only adjust marks if we managed to switch to a window that
	    // holds the buffer, otherwise line numbers will be invalid.
	    appended_lines_mark((linenr_T)n, 1L);

	vim_free(str);
	restore_win_for_buf(&switchwin, &save_curbuf);
	update_screen(UPD_VALID);

	もし (VimTryEnd())
	    戻る FAIL;

	もし (len_change)
	    *len_change = 1;

	戻る OK;
    }
    否则 もし (PyList_Check(lines))
    {
	PyInt	i;
	PyInt	size = PyList_Size(lines);
	char	**array;

	array = PyMem_New(char *, size);
	もし (array == NULL)
	{
	    PyErr_NoMemory();
	    戻る FAIL;
	}

	ために (i = 0; i < size; ++i)
	{
	    PyObject *line;

	    もし (!(line = PyList_GetItem(lines, i)) ||
		!(array[i] = StringToLine(line)))
	    {
		ながら (i)
		    vim_free(array[--i]);
		PyMem_Free(array);
		戻る FAIL;
	    }
	}

	PyErr_Clear();
	VimTryStart();
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	もし (u_save((linenr_T)n, (linenr_T)(n + 1)) == FAIL)
	    RAISE_UNDO_FAIL;
	否则
	{
	    ために (i = 0; i < size; ++i)
	    {
		もし (ml_append((linenr_T)(n + i),
					(char_u *)array[i], 0, FALSE) == FAIL)
		{
		    RAISE_INSERT_LINE_FAIL;

		    // Free the rest of the lines
		    ながら (i < size)
			vim_free(array[i++]);

		    break;
		}
		vim_free(array[i]);
	    }
	    もし (i > 0 && save_curbuf.br_buf == NULL)
		// Only adjust marks if we managed to switch to a window that
		// holds the buffer, otherwise line numbers will be invalid.
		appended_lines_mark((linenr_T)n, (long)i);
	}

	// Free the array of lines. All of its contents have now
	// been freed.
	PyMem_Free(array);
	restore_win_for_buf(&switchwin, &save_curbuf);

	update_screen(UPD_VALID);

	もし (VimTryEnd())
	    戻る FAIL;

	もし (len_change)
	    *len_change = size;

	戻る OK;
    }
    否则
    {
	PyErr_BadArgument();
	戻る FAIL;
    }
}

/*
 * Common routines for buffers and line ranges
 * -------------------------------------------
 */

typedef struct
{
    PyObject_HEAD
    buf_T *buf;
} BufferObject;

    static 整数
CheckBuffer(BufferObject *self)
{
    もし (self->buf == INVALID_BUFFER_VALUE)
    {
	PyErr_SET_VIM(N_("attempt to refer to deleted buffer"));
	戻る -1;
    }

    戻る 0;
}

    static 整数
CheckBufferCb(void *self)
{
    戻る CheckBuffer((BufferObject*)self);
}

    static PyObject *
RBItem(BufferObject *self, PyInt n, PyInt start, PyInt end)
{
    もし (CheckBuffer(self))
	戻る NULL;

    もし (end == -1)
	end = self->buf->b_ml.ml_line_count;

    もし (n < 0)
	n += end - start + 1;

    もし (n < 0 || n > end - start)
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("line number out of range"));
	戻る NULL;
    }

    戻る GetBufferLine(self->buf, n+start);
}

    static PyObject *
RBSlice(BufferObject *self, PyInt lo, PyInt hi, PyInt start, PyInt end)
{
    PyInt size;

    もし (CheckBuffer(self))
	戻る NULL;

    もし (end == -1)
	end = self->buf->b_ml.ml_line_count;

    size = end - start + 1;

    もし (lo < 0)
	lo = 0;
    否则 もし (lo > size)
	lo = size;
    もし (hi < 0)
	hi = 0;
    もし (hi < lo)
	hi = lo;
    否则 もし (hi > size)
	hi = size;

    戻る GetBufferLineList(self->buf, lo+start, hi+start);
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

    もし (CheckBuffer(self))
	戻る -1;

    もし (end == -1)
	end = self->buf->b_ml.ml_line_count;

    もし (n < 0)
	n += end - start + 1;

    もし (n < 0 || n > end - start)
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("line number out of range"));
	戻る -1;
    }

    もし (SetBufferLine(self->buf, n+start, valObject, &len_change) == FAIL)
	戻る -1;

    もし (new_end)
	*new_end = end + len_change;

    戻る 0;
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
    もし (CheckBuffer(self))
	戻る -1;

    もし (end == -1)
	end = self->buf->b_ml.ml_line_count;

    // Sort out the slice range
    size = end - start + 1;

    もし (lo < 0)
	lo = 0;
    否则 もし (lo > size)
	lo = size;
    もし (hi < 0)
	hi = 0;
    もし (hi < lo)
	hi = lo;
    否则 もし (hi > size)
	hi = size;

    もし (SetBufferLineList(self->buf, lo + start, hi + start,
						valObject, &len_change) == FAIL)
	戻る -1;

    もし (new_end)
	*new_end = end + len_change;

    戻る 0;
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

    もし (CheckBuffer(self))
	戻る NULL;

    もし (end == -1)
	end = self->buf->b_ml.ml_line_count;

    max = n = end - start + 1;

    もし (!PyArg_ParseTuple(args, "O|n", &lines, &n))
	戻る NULL;

    もし (n < 0 || n > max)
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("line number out of range"));
	戻る NULL;
    }

    もし (InsertBufferLines(self->buf, n + start - 1, lines, &len_change) == FAIL)
	戻る NULL;

    もし (new_end)
	*new_end = end + len_change;

    Py_INCREF(Py_None);
    戻る Py_None;
}

// Range object

DEFINE_PY_TYPE_OBJECT(RangeType);
static PySequenceMethods RangeAsSeq;
static PyMappingMethods RangeAsMapping;

typedef struct
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
    もし (self == NULL)
	戻る NULL;

    bufr = (BufferObject *)BufferNew(buf);
    もし (bufr == NULL)
    {
	Py_DECREF(self);
	戻る NULL;
    }
    Py_INCREF((PyObject *)bufr);

    self->buf = bufr;
    self->start = start;
    self->end = end;

    戻る (PyObject *)(self);
}

    static void
RangeDestructor(PyObject *self_obj)
{
    RangeObject *self = (RangeObject*)self_obj;
    PyObject_GC_UnTrack((void *)(self));
    Py_XDECREF(((PyObject *)(self->buf)));
    PyObject_GC_Del((void *)(self));
}

    static 整数
RangeTraverse(PyObject *self_obj, visitproc visit, void *arg)
{
    RangeObject *self = (RangeObject*)self_obj;
    Py_VISIT(((PyObject *)(self->buf)));
    戻る 0;
}

    static 整数
RangeClear(PyObject *self_obj)
{
    RangeObject *self = (RangeObject*)self_obj;
    Py_CLEAR(self->buf);
    戻る 0;
}

    static PyInt
RangeLength(RangeObject *self)
{
    // HOW DO WE SIGNAL AN ERROR FROM THIS FUNCTION?
    もし (CheckBuffer(self->buf))
	戻る -1; // ???

    戻る (self->end - self->start + 1);
}

    static PyObject *
RangeItem(RangeObject *self, PyInt n)
{
    戻る RBItem(self->buf, n, self->start, self->end);
}

    static PyObject *
RangeSlice(RangeObject *self, PyInt lo, PyInt hi)
{
    戻る RBSlice(self->buf, lo, hi, self->start, self->end);
}

static char *RangeAttrs[] = {
    "start", "end",
    NULL
};

    static PyObject *
RangeDir(PyObject *self, PyObject *args UNUSED)
{
    戻る ObjectDir(self, RangeAttrs);
}

    static PyObject *
RangeAppend(RangeObject *self, PyObject *args)
{
    戻る RBAppend(self->buf, args, self->start, self->end, &self->end);
}

    static PyObject *
RangeRepr(PyObject *self_obj)
{
    RangeObject *self = (RangeObject*)self_obj;
    もし (self->buf->buf == INVALID_BUFFER_VALUE)
	戻る PyString_FromFormat("<range object (for deleted buffer) at %p>",
				    (void *)self);
    否则
    {
	char *name = (char *)self->buf->buf->b_fname;

	もし (name == NULL)
	    name = "";

	戻る PyString_FromFormat("<range %s (%d:%d)>",
				    name, (整数)self->start, (整数)self->end);
    }
}

static struct PyMethodDef RangeMethods[] = {
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

    もし (BUF_PYTHON_REF(buf) != NULL)
    {
	self = BUF_PYTHON_REF(buf);
	Py_INCREF((PyObject *)self);
    }
    否则
    {
	self = PyObject_NEW(BufferObject, BufferTypePtr);
	もし (self == NULL)
	    戻る NULL;
	self->buf = buf;
	BUF_PYTHON_REF(buf) = self;
    }

    戻る (PyObject *)(self);
}

    static void
BufferDestructor(PyObject *self_obj)
{
    BufferObject *self = (BufferObject*)self_obj;
    もし (self->buf && self->buf != INVALID_BUFFER_VALUE)
	BUF_PYTHON_REF(self->buf) = NULL;

    DESTRUCTOR_FINISH(self);
}

    static PyInt
BufferLength(BufferObject *self)
{
    // HOW DO WE SIGNAL AN ERROR FROM THIS FUNCTION?
    もし (CheckBuffer(self))
	戻る -1; // ???

    戻る (PyInt)(self->buf->b_ml.ml_line_count);
}

    static PyObject *
BufferItem(BufferObject *self, PyInt n)
{
    戻る RBItem(self, n, 1, -1);
}

    static PyObject *
BufferSlice(BufferObject *self, PyInt lo, PyInt hi)
{
    戻る RBSlice(self, lo, hi, 1, -1);
}

static char *BufferAttrs[] = {
    "name", "number", "vars", "options", "valid",
    NULL
};

    static PyObject *
BufferDir(PyObject *self, PyObject *args UNUSED)
{
    戻る ObjectDir(self, BufferAttrs);
}

    static PyObject *
BufferAttrValid(BufferObject *self, char *name)
{
    PyObject	*ret;

    もし (strcmp(name, "valid") != 0)
	戻る NULL;

    ret = ((self->buf == INVALID_BUFFER_VALUE) ? Py_False : Py_True);
    Py_INCREF(ret);
    戻る ret;
}

    static PyObject *
BufferAttr(BufferObject *self, char *name)
{
    もし (strcmp(name, "name") == 0)
	戻る PyString_FromString((self->buf->b_ffname == NULL
				    ? "" : (char *)self->buf->b_ffname));
    否则 もし (strcmp(name, "number") == 0)
	戻る Py_BuildValue(Py_ssize_t_fmt, self->buf->b_fnum);
    否则 もし (strcmp(name, "vars") == 0)
	戻る NEW_DICTIONARY(self->buf->b_vars);
    否则 もし (strcmp(name, "options") == 0)
	戻る OptionsNew(SREQ_BUF, self->buf, CheckBufferCb,
			(PyObject *) self);
    否则 もし (strcmp(name, "__members__") == 0)
	戻る ObjectDir(NULL, BufferAttrs);
    否则
	戻る NULL;
}

    static 整数
BufferSetattr(PyObject *self_obj, char *name, PyObject *valObject)
{
    BufferObject *self = (BufferObject*)self_obj;
    もし (CheckBuffer(self))
	戻る -1;

    もし (strcmp(name, "name") == 0)
    {
	char_u		*val;
	aco_save_T	aco;
	整数		ren_ret = OK;
	PyObject	*todecref;

	もし (!(val = StringToChars(valObject, &todecref)))
	    戻る -1;

	VimTryStart();
	// Using aucmd_*: autocommands will be executed by rename_buffer
	aucmd_prepbuf(&aco, self->buf);
	もし (curbuf == self->buf)
	{
	    ren_ret = rename_buffer(val);
	    aucmd_restbuf(&aco);
	}
	Py_XDECREF(todecref);
	もし (VimTryEnd())
	    戻る -1;

	もし (ren_ret == FAIL)
	{
	    PyErr_SET_VIM(N_("failed to rename buffer"));
	    戻る -1;
	}
	戻る 0;
    }
    否则
    {
	PyErr_SetString(PyExc_AttributeError, name);
	戻る -1;
    }
}

    static PyObject *
BufferAppend(BufferObject *self, PyObject *args)
{
    戻る RBAppend(self, args, 1, -1, NULL);
}

    static PyObject *
BufferMark(BufferObject *self, PyObject *pmarkObject)
{
    pos_T	*posp;
    char_u	*pmark;
    char_u	mark;
    bufref_T	savebuf;
    PyObject	*todecref;

    もし (CheckBuffer(self))
	戻る NULL;

    もし (!(pmark = StringToChars(pmarkObject, &todecref)))
	戻る NULL;

    もし (pmark[0] == '\0' || pmark[1] != '\0')
    {
	PyErr_SET_STRING(PyExc_ValueError,
		N_("mark name must be a single character"));
	Py_XDECREF(todecref);
	戻る NULL;
    }

    mark = *pmark;

    Py_XDECREF(todecref);

    VimTryStart();
    switch_buffer(&savebuf, self->buf);
    posp = getmark(mark, FALSE);
    restore_buffer(&savebuf);
    もし (VimTryEnd())
	戻る NULL;

    もし (posp == NULL)
    {
	PyErr_SET_VIM(N_("invalid mark name"));
	戻る NULL;
    }

    もし (posp->lnum <= 0)
    {
	// Or raise an error?
	Py_INCREF(Py_None);
	戻る Py_None;
    }

    戻る Py_BuildValue("(ll)", (long)(posp->lnum), (long)(posp->col));
}

    static PyObject *
BufferRange(BufferObject *self, PyObject *args)
{
    PyInt start;
    PyInt end;

    もし (CheckBuffer(self))
	戻る NULL;

    もし (!PyArg_ParseTuple(args, "nn", &start, &end))
	戻る NULL;

    戻る RangeNew(self->buf, start, end);
}

    static PyObject *
BufferRepr(PyObject *self_obj)
{
    BufferObject *self = (BufferObject*)self_obj;
    もし (self->buf == INVALID_BUFFER_VALUE)
	戻る PyString_FromFormat("<buffer object (deleted) at %p>", (void *)self);
    否则
    {
	char	*name = (char *)self->buf->b_fname;

	もし (name == NULL)
	    name = "";

	戻る PyString_FromFormat("<buffer %s>", name);
    }
}

static struct PyMethodDef BufferMethods[] = {
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

typedef struct
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

    ながら (b)
    {
	++n;
	b = b->b_next;
    }

    戻る n;
}

    static PyObject *
BufMapItem(PyObject *self UNUSED, PyObject *keyObject)
{
    buf_T	*b;
    long	bnr;

    もし (NumberToLong(keyObject, &bnr, NUMBER_INT|NUMBER_NATURAL))
	戻る NULL;

    b = buflist_findnr((整数) bnr);

    もし (b)
	戻る BufferNew(b);
    否则
    {
	PyErr_SetObject(PyExc_KeyError, keyObject);
	戻る NULL;
    }
}

    static void
BufMapIterDestruct(void* arg)
{
    PyObject *buffer = (PyObject*)arg;
    // Iteration was stopped before all buffers were processed
    もし (buffer)
    {
	Py_DECREF(buffer);
    }
}

    static 整数
BufMapIterTraverse(void *iter, visitproc visit, void *arg)
{
    PyObject *buffer = (PyObject*)iter;
    もし (buffer)
	Py_VISIT(buffer);
    戻る 0;
}

    static 整数
BufMapIterClear(void **iter)
{
    PyObject **buffer = (PyObject**)iter;
    もし (*buffer)
	Py_CLEAR(*buffer);
    戻る 0;
}

    static PyObject *
BufMapIterNext(void **arg)
{
    PyObject	*next;
    PyObject	*ret;
    PyObject	**buffer = (PyObject**)arg;

    もし (!*buffer)
	戻る NULL;

    ret = *buffer;

    もし (CheckBuffer((BufferObject *)(ret)))
    {
	*buffer = NULL;
	戻る NULL;
    }

    もし (!((BufferObject *)(ret))->buf->b_next)
	next = NULL;
    否则 もし (!(next = BufferNew(((BufferObject *)(ret))->buf->b_next)))
	戻る NULL;
    *buffer = next;
    // Do not increment reference: we no longer hold it (decref), but whoever
    // on other side will hold (incref). Decref+incref = nothing.
    戻る ret;
}

    static PyObject *
BufMapIter(PyObject *self)
{
    PyObject *buffer;

    buffer = BufferNew(firstbuf);
    戻る IterNew(buffer,
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

static char *CurrentAttrs[] = {
    "buffer", "window", "line", "range", "tabpage",
    NULL
};

    static PyObject *
CurrentDir(PyObject *self, PyObject *args UNUSED)
{
    戻る ObjectDir(self, CurrentAttrs);
}

    static PyObject *
CurrentGetattr(PyObject *self UNUSED, char *name)
{
    もし (strcmp(name, "buffer") == 0)
	戻る (PyObject *)BufferNew(curbuf);
    否则 もし (strcmp(name, "window") == 0)
	戻る (PyObject *)WindowNew(curwin, curtab);
    否则 もし (strcmp(name, "tabpage") == 0)
	戻る (PyObject *)TabPageNew(curtab);
    否则 もし (strcmp(name, "line") == 0)
	戻る GetBufferLine(curbuf, (PyInt)curwin->w_cursor.lnum);
    否则 もし (strcmp(name, "range") == 0)
	戻る RangeNew(curbuf, RangeStart, RangeEnd);
    否则 もし (strcmp(name, "__members__") == 0)
	戻る ObjectDir(NULL, CurrentAttrs);
    否则
#もし PY_MAJOR_VERSION < 3
	戻る Py_FindMethod(WindowMethods, self, name);
#否则
	戻る NULL;
#endif
}

    static 整数
CurrentSetattr(PyObject *self UNUSED, char *name, PyObject *valObject)
{
    もし (strcmp(name, "line") == 0)
    {
	もし (SetBufferLine(curbuf, (PyInt)curwin->w_cursor.lnum, valObject,
			  NULL) == FAIL)
	    戻る -1;

	戻る 0;
    }
    否则 もし (strcmp(name, "buffer") == 0)
    {
	整数 count;

	もし (valObject->ob_type != BufferTypePtr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.Buffer object, but got %s"),
		    valObject);
	    戻る -1;
	}

	もし (CheckBuffer((BufferObject *)(valObject)))
	    戻る -1;
	count = ((BufferObject *)(valObject))->buf->b_fnum;

	VimTryStart();
	もし (do_buffer(DOBUF_GOTO, DOBUF_FIRST, FORWARD, count, 0) == FAIL)
	{
	    もし (VimTryEnd())
		戻る -1;
	    PyErr_VIM_FORMAT(N_("failed to switch to buffer %d"), count);
	    戻る -1;
	}

	戻る VimTryEnd();
    }
    否则 もし (strcmp(name, "window") == 0)
    {
	整数 count;

	もし (valObject->ob_type != WindowTypePtr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.Window object, but got %s"),
		    valObject);
	    戻る -1;
	}

	もし (CheckWindow((WindowObject *)(valObject)))
	    戻る -1;
	count = get_win_number(((WindowObject *)(valObject))->win, firstwin);

	もし (!count)
	{
	    PyErr_SET_STRING(PyExc_ValueError,
		    N_("failed to find window in the current tab page"));
	    戻る -1;
	}

	VimTryStart();
	win_goto(((WindowObject *)(valObject))->win);
	もし (((WindowObject *)(valObject))->win != curwin)
	{
	    もし (VimTryEnd())
		戻る -1;
	    PyErr_SET_STRING(PyExc_RuntimeError,
		    N_("did not switch to the specified window"));
	    戻る -1;
	}

	戻る VimTryEnd();
    }
    否则 もし (strcmp(name, "tabpage") == 0)
    {
	もし (valObject->ob_type != TabPageTypePtr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.TabPage object, but got %s"),
		    valObject);
	    戻る -1;
	}

	もし (CheckTabPage((TabPageObject *)(valObject)))
	    戻る -1;

	VimTryStart();
	goto_tabpage_tp(((TabPageObject *)(valObject))->tab, TRUE, TRUE);
	もし (((TabPageObject *)(valObject))->tab != curtab)
	{
	    もし (VimTryEnd())
		戻る -1;
	    PyErr_SET_STRING(PyExc_RuntimeError,
		    N_("did not switch to the specified tab page"));
	    戻る -1;
	}

	戻る VimTryEnd();
    }
    否则
    {
	PyErr_SetString(PyExc_AttributeError, name);
	戻る -1;
    }
}

static struct PyMethodDef CurrentMethods[] = {
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
run_cmd(const char *cmd, dict_T* locals UNUSED, void *arg UNUSED
#ifdef PY_CAN_RECURSE
	, PyGILState_STATE *pygilstate UNUSED
#endif
	)
{
    PyObject	*run_ret;
    run_ret = PyRun_String((char *)cmd, Py_file_input, globals, globals);
    もし (run_ret != NULL)
    {
	Py_DECREF(run_ret);
    }
    否则 もし (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
    {
	emsg(_(e_cant_handle_systemexit_of_python_exception_in_vim));
	PyErr_Clear();
    }
    否则
	PyErr_PrintEx(1);
}

static const char	*code_hdr = "def " DOPY_FUNC "(line, linenr):\n ";
static 整数		code_hdr_len = 30;

    static void
run_do(const char *cmd, dict_T* locals UNUSED, void *arg UNUSED
#ifdef PY_CAN_RECURSE
	, PyGILState_STATE *pygilstate
#endif
	)
{
    PyInt	lnum;
    size_t	len;
    char	*code;
    整数		status;
    PyObject	*pyfunc, *pymain;
    PyObject	*run_ret;
    buf_T	*was_curbuf = curbuf;

    もし (u_save((linenr_T)RangeStart - 1, (linenr_T)RangeEnd + 1) != OK)
    {
	emsg(_("cannot save undo information"));
	戻る;
    }

    len = code_hdr_len + STRLEN(cmd);
    code = PyMem_New(char, len + 1);
    memcpy(code, code_hdr, code_hdr_len);
    STRCPY(code + code_hdr_len, cmd);
    run_ret = PyRun_String(code, Py_file_input, globals, globals);
    status = -1;
    もし (run_ret != NULL)
    {
	status = 0;
	Py_DECREF(run_ret);
    }
    否则 もし (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
    {
	PyMem_Free(code);
	emsg(_(e_cant_handle_systemexit_of_python_exception_in_vim));
	PyErr_Clear();
	戻る;
    }
    否则
	PyErr_PrintEx(1);

    PyMem_Free(code);

    もし (status)
    {
	emsg(_("failed to run the code"));
	戻る;
    }

    status = 0;
    pymain = PyImport_AddModule("__main__");
    pyfunc = PyObject_GetAttrString(pymain, DOPY_FUNC);
#ifdef PY_CAN_RECURSE
    PyGILState_Release(*pygilstate);
#endif

    ために (lnum = RangeStart; lnum <= RangeEnd; ++lnum)
    {
	PyObject	*line;
	PyObject	*linenr;
	PyObject	*ret;

#ifdef PY_CAN_RECURSE
	*pygilstate = PyGILState_Ensure();
#endif
	// Check the line number, the command my have deleted lines.
	もし (lnum > curbuf->b_ml.ml_line_count
		|| !(line = GetBufferLine(curbuf, lnum)))
	    goto err;
	もし (!(linenr = PyInt_FromLong((long) lnum)))
	{
	    Py_DECREF(line);
	    goto err;
	}
	ret = PyObject_CallFunctionObjArgs(pyfunc, line, linenr, NULL);
	Py_DECREF(line);
	Py_DECREF(linenr);
	もし (!ret)
	    goto err;

	// Check that the command didn't switch to another buffer.
	// Check the line number, the command my have deleted lines.
	もし (curbuf != was_curbuf || lnum > curbuf->b_ml.ml_line_count)
	{
	    Py_XDECREF(ret);
	    goto err;
	}

	もし (ret != Py_None)
	    もし (SetBufferLine(curbuf, lnum, ret, NULL) == FAIL)
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
    もし (!status)
	*pygilstate = PyGILState_Ensure();
#endif
    Py_DECREF(pyfunc);
    PyObject_SetAttrString(pymain, DOPY_FUNC, NULL);
    もし (status)
	戻る;
    check_cursor();
    update_curbuf(UPD_NOT_VALID);
}

    static void
run_eval(const char *cmd, dict_T *locals, void *arg
#ifdef PY_CAN_RECURSE
	, PyGILState_STATE *pygilstate UNUSED
#endif
	)
{
    PyObject	*run_ret;
    typval_T	*rettv = (typval_T*)arg;
    PyObject	*pylocals = locals ? NEW_DICTIONARY(locals) : globals;

    run_ret = PyRun_String((char *)cmd, Py_eval_input, globals, pylocals);
    もし (run_ret == NULL)
    {
	もし (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
	{
	    emsg(_(e_cant_handle_systemexit_of_python_exception_in_vim));
	    PyErr_Clear();
	}
	否则
	{
	    もし (PyErr_Occurred() && !msg_silent)
		PyErr_PrintEx(0);
	    emsg(_(e_eval_did_not_return_valid_python_object));
	}
    }
    否则
    {
	もし (ConvertFromPyObject(run_ret, rettv) == -1)
	    emsg(_(e_failed_to_convert_returned_python_object_to_vim_value));
	Py_DECREF(run_ret);
    }
    PyErr_Clear();
}

    static 整数
set_ref_in_py(const 整数 copyID)
{
    pylinkedlist_T	*cur;
    list_T		*ll;
    tuple_T		*tt;
    整数			i;
    整数			abort = FALSE;
    FunctionObject	*func;

    もし (lastdict != NULL)
    {
	ために (cur = lastdict ; !abort && cur != NULL ; cur = cur->pll_prev)
	    abort = set_ref_in_dict(((DictionaryObject *)(cur->pll_obj))->dict,
								       copyID);
    }

    もし (lastlist != NULL)
    {
	ために (cur = lastlist ; !abort && cur != NULL ; cur = cur->pll_prev)
	{
	    ll = ((ListObject *) (cur->pll_obj))->list;
	    abort = set_ref_in_list(ll, copyID);
	}
    }

    もし (lasttuple != NULL)
    {
	ために (cur = lasttuple ; !abort && cur != NULL ; cur = cur->pll_prev)
	{
	    tt = ((TupleObject *) (cur->pll_obj))->tuple;
	    abort = set_ref_in_tuple(tt, copyID);
	}
    }

    もし (lastfunc != NULL)
    {
	ために (cur = lastfunc ; !abort && cur != NULL ; cur = cur->pll_prev)
	{
	    func = (FunctionObject *) cur->pll_obj;
	    abort = set_ref_in_dict(func->self, copyID);
	    もし (func->argc)
		ために (i = 0; !abort && i < func->argc; ++i)
		    abort = abort
			|| set_ref_in_item(&func->argv[i], copyID, NULL, NULL, NULL);
	}
    }

    戻る abort;
}

    static 整数
set_string_copy(char_u *str, typval_T *tv)
{
    tv->vval.v_string = vim_strsave(str);
    もし (tv->vval.v_string == NULL)
    {
	PyErr_NoMemory();
	戻る -1;
    }
    戻る 0;
}

    static 整数
pydict_to_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    dict_T	*dict;
    char_u	*key;
    dictitem_T	*di;
    PyObject	*keyObject;
    PyObject	*valObject;
    Py_ssize_t	iter = 0;

    もし (!(dict = py_dict_alloc()))
	戻る -1;

    tv->v_type = VAR_DICT;
    tv->vval.v_dict = dict;

    ながら (PyDict_Next(obj, &iter, &keyObject, &valObject))
    {
	PyObject	*todecref = NULL;

	もし (keyObject == NULL || valObject == NULL)
	{
	    dict_unref(dict);
	    戻る -1;
	}

	もし (!(key = StringToChars(keyObject, &todecref)))
	{
	    dict_unref(dict);
	    戻る -1;
	}

	もし (*key == NUL)
	{
	    dict_unref(dict);
	    Py_XDECREF(todecref);
	    RAISE_NO_EMPTY_KEYS;
	    戻る -1;
	}

	di = dictitem_alloc(key);

	Py_XDECREF(todecref);

	もし (di == NULL)
	{
	    PyErr_NoMemory();
	    dict_unref(dict);
	    戻る -1;
	}

	もし (_ConvertFromPyObject(valObject, &di->di_tv, lookup_dict) == -1)
	{
	    vim_free(di);
	    dict_unref(dict);
	    戻る -1;
	}

	もし (dict_add(dict, di) == FAIL)
	{
	    RAISE_KEY_ADD_FAIL(di->di_key);
	    clear_tv(&di->di_tv);
	    vim_free(di);
	    dict_unref(dict);
	    戻る -1;
	}
    }

    --dict->dv_refcount;
    戻る 0;
}

    static 整数
pymap_to_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    dict_T	*dict;
    char_u	*key;
    dictitem_T	*di;
    PyObject	*list;
    PyObject	*iterator;
    PyObject	*keyObject;
    PyObject	*valObject;

    もし (!(dict = py_dict_alloc()))
	戻る -1;

    tv->v_type = VAR_DICT;
    tv->vval.v_dict = dict;

    もし (!(list = PyMapping_Keys(obj)))
    {
	dict_unref(dict);
	戻る -1;
    }

    もし (!(iterator = PyObject_GetIter(list)))
    {
	dict_unref(dict);
	Py_DECREF(list);
	戻る -1;
    }
    Py_DECREF(list);

    ながら ((keyObject = PyIter_Next(iterator)))
    {
	PyObject	*todecref;

	もし (!(key = StringToChars(keyObject, &todecref)))
	{
	    Py_DECREF(keyObject);
	    Py_DECREF(iterator);
	    dict_unref(dict);
	    戻る -1;
	}

	もし (*key == NUL)
	{
	    Py_DECREF(keyObject);
	    Py_DECREF(iterator);
	    Py_XDECREF(todecref);
	    dict_unref(dict);
	    RAISE_NO_EMPTY_KEYS;
	    戻る -1;
	}

	もし (!(valObject = PyObject_GetItem(obj, keyObject)))
	{
	    Py_DECREF(keyObject);
	    Py_DECREF(iterator);
	    Py_XDECREF(todecref);
	    dict_unref(dict);
	    戻る -1;
	}

	di = dictitem_alloc(key);

	Py_DECREF(keyObject);
	Py_XDECREF(todecref);

	もし (di == NULL)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(valObject);
	    dict_unref(dict);
	    PyErr_NoMemory();
	    戻る -1;
	}

	もし (_ConvertFromPyObject(valObject, &di->di_tv, lookup_dict) == -1)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(valObject);
	    vim_free(di);
	    dict_unref(dict);
	    戻る -1;
	}

	Py_DECREF(valObject);

	もし (dict_add(dict, di) == FAIL)
	{
	    RAISE_KEY_ADD_FAIL(di->di_key);
	    Py_DECREF(iterator);
	    dictitem_free(di);
	    dict_unref(dict);
	    戻る -1;
	}
    }
    Py_DECREF(iterator);
    --dict->dv_refcount;
    戻る 0;
}

    static 整数
pyseq_to_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    list_T	*l;

    もし (!(l = py_list_alloc()))
	戻る -1;

    tv->v_type = VAR_LIST;
    tv->vval.v_list = l;

    もし (list_py_concat(l, obj, lookup_dict) == -1)
    {
	list_unref(l);
	戻る -1;
    }

    --l->lv_refcount;
    戻る 0;
}

    static 整数
pytuple_to_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    tuple_T	*t;

    もし (!(t = py_tuple_alloc()))
	戻る -1;

    tv->v_type = VAR_TUPLE;
    tv->vval.v_tuple = t;

    もし (tuple_py_concat(t, obj, lookup_dict) == -1)
    {
	tuple_unref(t);
	戻る -1;
    }

    --t->tv_refcount;
    戻る 0;
}

typedef 整数 (*pytotvfunc)(PyObject *, typval_T *, PyObject *);

    static 整数
convert_dl(PyObject *obj, typval_T *tv,
				    pytotvfunc py_to_tv, PyObject *lookup_dict)
{
    PyObject	*capsule;
    char	hexBuf[sizeof(void *) * 2 + 3];

    sprintf(hexBuf, "%p", (void *)obj);

#ifdef PY_USE_CAPSULE
    capsule = PyDict_GetItemString(lookup_dict, hexBuf);
#否则
    capsule = (PyObject *)PyDict_GetItemString(lookup_dict, hexBuf);
#endif
    もし (capsule == NULL)
    {
#ifdef PY_USE_CAPSULE
	capsule = PyCapsule_New(tv, NULL, NULL);
#否则
	capsule = PyCObject_FromVoidPtr(tv, NULL);
#endif
	もし (PyDict_SetItemString(lookup_dict, hexBuf, capsule))
	{
	    Py_DECREF(capsule);
	    tv->v_type = VAR_UNKNOWN;
	    戻る -1;
	}

	Py_DECREF(capsule);

	もし (py_to_tv(obj, tv, lookup_dict) == -1)
	{
	    tv->v_type = VAR_UNKNOWN;
	    戻る -1;
	}
	// As we are not using copy_tv which increments reference count we must
	// do it ourself.
	もし (tv->v_type == VAR_DICT)
	    ++tv->vval.v_dict->dv_refcount;
	否则 もし (tv->v_type == VAR_LIST)
	    ++tv->vval.v_list->lv_refcount;
	否则 もし (tv->v_type == VAR_TUPLE)
	    ++tv->vval.v_tuple->tv_refcount;
    }
    否则
    {
	typval_T	*v;

#ifdef PY_USE_CAPSULE
	v = PyCapsule_GetPointer(capsule, NULL);
#否则
	v = PyCObject_AsVoidPtr(capsule);
#endif
	copy_tv(v, tv);
    }
    戻る 0;
}

    static 整数
ConvertFromPyMapping(PyObject *obj, typval_T *tv)
{
    PyObject	*lookup_dict;
    整数		ret;

    もし (!(lookup_dict = PyDict_New()))
	戻る -1;

    もし (PyType_IsSubtype(obj->ob_type, DictionaryTypePtr))
    {
	tv->v_type = VAR_DICT;
	tv->vval.v_dict = (((DictionaryObject *)(obj))->dict);
	++tv->vval.v_dict->dv_refcount;
	ret = 0;
    }
    否则 もし (PyDict_Check(obj))
	ret = convert_dl(obj, tv, pydict_to_tv, lookup_dict);
    否则 もし (PyMapping_Check(obj))
	ret = convert_dl(obj, tv, pymap_to_tv, lookup_dict);
    否则
    {
	PyErr_FORMAT_TYPE(
		N_("unable to convert %s to a Vim dictionary"),
		obj);
	ret = -1;
    }
    Py_DECREF(lookup_dict);
    戻る ret;
}

    static 整数
ConvertFromPySequence(PyObject *obj, typval_T *tv)
{
    PyObject	*lookup_dict;
    整数		ret;

    もし (!(lookup_dict = PyDict_New()))
	戻る -1;

    もし (PyType_IsSubtype(obj->ob_type, ListTypePtr))
    {
	tv->v_type = VAR_LIST;
	tv->vval.v_list = (((ListObject *)(obj))->list);
	++tv->vval.v_list->lv_refcount;
	ret = 0;
    }
    否则 もし (PyIter_Check(obj) || PySequence_Check(obj))
	ret = convert_dl(obj, tv, pyseq_to_tv, lookup_dict);
    否则
    {
	PyErr_FORMAT_TYPE(
		N_("unable to convert %s to a Vim list"),
		obj);
	ret = -1;
    }
    Py_DECREF(lookup_dict);
    戻る ret;
}

    static 整数
ConvertFromPyObject(PyObject *obj, typval_T *tv)
{
    PyObject	*lookup_dict;
    整数		ret;

    もし (!(lookup_dict = PyDict_New()))
	戻る -1;
    ret = _ConvertFromPyObject(obj, tv, lookup_dict);
    Py_DECREF(lookup_dict);
    戻る ret;
}

    static 整数
_ConvertFromPyObject(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    もし (PyType_IsSubtype(obj->ob_type, DictionaryTypePtr))
    {
	tv->v_type = VAR_DICT;
	tv->vval.v_dict = (((DictionaryObject *)(obj))->dict);
	++tv->vval.v_dict->dv_refcount;
    }
    否则 もし (PyType_IsSubtype(obj->ob_type, ListTypePtr))
    {
	tv->v_type = VAR_LIST;
	tv->vval.v_list = (((ListObject *)(obj))->list);
	++tv->vval.v_list->lv_refcount;
    }
    否则 もし (PyType_IsSubtype(obj->ob_type, TupleTypePtr))
    {
	tv->v_type = VAR_TUPLE;
	tv->vval.v_tuple = (((TupleObject *)(obj))->tuple);
	++tv->vval.v_tuple->tv_refcount;
    }
    否则 もし (PyType_IsSubtype(obj->ob_type, FunctionTypePtr))
    {
	FunctionObject *func = (FunctionObject *) obj;
	もし (func->self != NULL || func->argv != NULL)
	{
	    partial_T *pt = ALLOC_CLEAR_ONE(partial_T);

	    set_partial(func, pt, TRUE);
	    tv->vval.v_partial = pt;
	    tv->v_type = VAR_PARTIAL;
	}
	否则
	{
	    もし (set_string_copy(func->name, tv) == -1)
		戻る -1;

	    tv->v_type = VAR_FUNC;
	}
	func_ref(func->name);
    }
    否则 もし (PyBytes_Check(obj))
    {
	char_u	*str;

	もし (PyBytes_AsStringAndSize(obj, (char **) &str, NULL) == -1)
	    戻る -1;
	もし (str == NULL)
	    戻る -1;

	もし (set_string_copy(str, tv) == -1)
	    戻る -1;

	tv->v_type = VAR_STRING;
    }
    否则 もし (PyUnicode_Check(obj))
    {
	PyObject	*bytes;
	char_u	*str;

	bytes = PyUnicode_AsEncodedString(obj, ENC_OPT, ERRORS_ENCODE_ARG);
	もし (bytes == NULL)
	    戻る -1;

	もし (PyBytes_AsStringAndSize(bytes, (char **) &str, NULL) == -1)
	    戻る -1;
	もし (str == NULL)
	    戻る -1;

	もし (set_string_copy(str, tv))
	{
	    Py_XDECREF(bytes);
	    戻る -1;
	}
	Py_XDECREF(bytes);

	tv->v_type = VAR_STRING;
    }
#もし PY_MAJOR_VERSION < 3
    否则 もし (PyInt_Check(obj))
    {
	tv->v_type = VAR_NUMBER;
	tv->vval.v_number = (varnumber_T) PyInt_AsLong(obj);
	もし (PyErr_Occurred())
	    戻る -1;
    }
#endif
    否则 もし (PyLong_Check(obj))
    {
	tv->v_type = VAR_NUMBER;
	tv->vval.v_number = (varnumber_T) PyLong_AsLong(obj);
	もし (PyErr_Occurred())
	    戻る -1;
    }
    否则 もし (PyTuple_Check(obj))
	戻る convert_dl(obj, tv, pytuple_to_tv, lookup_dict);
    否则 もし (PyDict_Check(obj))
	戻る convert_dl(obj, tv, pydict_to_tv, lookup_dict);
    否则 もし (PyFloat_Check(obj))
    {
	tv->v_type = VAR_FLOAT;
	tv->vval.v_float = (float_T) PyFloat_AsDouble(obj);
    }
    否则 もし (PyObject_HasAttrString(obj, "keys"))
	戻る convert_dl(obj, tv, pymap_to_tv, lookup_dict);
    // PyObject_GetIter can create built-in iterator for any sequence object
    否则 もし (PyIter_Check(obj) || PySequence_Check(obj))
	戻る convert_dl(obj, tv, pyseq_to_tv, lookup_dict);
    否则 もし (PyMapping_Check(obj))
	戻る convert_dl(obj, tv, pymap_to_tv, lookup_dict);
    否则 もし (PyNumber_Check(obj))
    {
	PyObject	*num;

	もし (!(num = PyNumber_Long(obj)))
	    戻る -1;

	tv->v_type = VAR_NUMBER;
	tv->vval.v_number = (varnumber_T) PyLong_AsLong(num);

	Py_DECREF(num);
    }
    否则 もし (obj == Py_None)
    {
	tv->v_type = VAR_SPECIAL;
	tv->vval.v_number = VVAL_NONE;
    }
    否则
    {
	PyErr_FORMAT_TYPE(
		N_("unable to convert %s to a Vim structure"),
		obj);
	戻る -1;
    }
    戻る 0;
}

    static PyObject *
ConvertToPyObject(typval_T *tv)
{
    typval_T *argv;
    整数 i;
    もし (tv == NULL)
    {
	PyErr_SET_VIM(N_("internal error: NULL reference passed"));
	戻る NULL;
    }
    switch (tv->v_type)
    {
	case VAR_STRING:
	    戻る PyBytes_FromString(tv->vval.v_string == NULL
					    ? "" : (char *)tv->vval.v_string);
	case VAR_NUMBER:
	    戻る PyLong_FromLong((long) tv->vval.v_number);
	case VAR_FLOAT:
	    戻る PyFloat_FromDouble((double) tv->vval.v_float);
	case VAR_LIST:
	    戻る NEW_LIST(tv->vval.v_list);
	case VAR_TUPLE:
	    戻る NEW_TUPLE(tv->vval.v_tuple);
	case VAR_DICT:
	    戻る NEW_DICTIONARY(tv->vval.v_dict);
	case VAR_FUNC:
	    戻る NEW_FUNCTION(tv->vval.v_string == NULL
					  ? (char_u *)"" : tv->vval.v_string,
					  0, NULL, NULL, TRUE);
	case VAR_PARTIAL:
	    もし (tv->vval.v_partial->pt_argc)
	    {
		argv = PyMem_New(typval_T, (size_t)tv->vval.v_partial->pt_argc);
		ために (i = 0; i < tv->vval.v_partial->pt_argc; i++)
		    copy_tv(&tv->vval.v_partial->pt_argv[i], &argv[i]);
	    }
	    否则
		argv = NULL;
	    もし (tv->vval.v_partial->pt_dict != NULL)
		tv->vval.v_partial->pt_dict->dv_refcount++;
	    戻る NEW_FUNCTION(tv->vval.v_partial == NULL
			     ? (char_u *)"" : partial_name(tv->vval.v_partial),
				tv->vval.v_partial->pt_argc, argv,
				tv->vval.v_partial->pt_dict,
				tv->vval.v_partial->pt_auto);
	case VAR_BLOB:
	    戻る PyBytes_FromStringAndSize(
		(char*) tv->vval.v_blob->bv_ga.ga_data,
		(Py_ssize_t) tv->vval.v_blob->bv_ga.ga_len);
	case VAR_UNKNOWN:
	case VAR_ANY:
	case VAR_VOID:
	case VAR_CHANNEL:
	case VAR_JOB:
	case VAR_INSTR:
	case VAR_CLASS:
	case VAR_OBJECT:
	case VAR_TYPEALIAS:
	    Py_INCREF(Py_None);
	    戻る Py_None;
	case VAR_BOOL:
	case VAR_SPECIAL:
	    switch (tv->vval.v_number)
	    {
		case VVAL_FALSE: 戻る ALWAYS_FALSE;
		case VVAL_TRUE:  戻る ALWAYS_TRUE;
		case VVAL_NONE:
		case VVAL_NULL:  戻る ALWAYS_NONE;
	    }
	    PyErr_SET_VIM(N_("internal error: invalid value type"));
	    戻る NULL;
    }
    戻る NULL;
}

DEFINE_PY_TYPE_OBJECT(CurrentType);

typedef struct
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
    OutputType.tp_basicsize = sizeof(OutputObject);
    OutputType.tp_flags = Py_TPFLAGS_DEFAULT;
    OutputType.tp_doc = "vim message object";
    OutputType.tp_methods = OutputMethods;
#もし PY_MAJOR_VERSION >= 3
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
#否则
    OutputType.tp_getattr = OutputGetattr;
    OutputType.tp_setattr = OutputSetattr;
    // Disabled, because this causes a crash in test86
    // OutputType.tp_base = &PyFile_Type;
#endif

    CLEAR_FIELD(IterType);
    IterType.tp_name = "vim.iter";
    IterType.tp_basicsize = sizeof(IterObject);
    IterType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    IterType.tp_doc = "generic iterator object";
    IterType.tp_iter = IterIter;
    IterType.tp_iternext = IterNext;
    IterType.tp_dealloc = IterDestructor;
    IterType.tp_traverse = IterTraverse;
    IterType.tp_clear = IterClear;

    CLEAR_FIELD(BufferType);
    BufferType.tp_name = "vim.buffer";
    BufferType.tp_basicsize = sizeof(BufferObject);
    BufferType.tp_dealloc = BufferDestructor;
    BufferType.tp_repr = BufferRepr;
    BufferType.tp_as_sequence = &BufferAsSeq;
    BufferType.tp_as_mapping = &BufferAsMapping;
    BufferType.tp_flags = Py_TPFLAGS_DEFAULT;
    BufferType.tp_doc = "vim buffer object";
    BufferType.tp_methods = BufferMethods;
#もし PY_MAJOR_VERSION >= 3
    BufferType.tp_getattro = BufferGetattro;
    BufferType.tp_setattro = BufferSetattro;
    BufferType.tp_alloc = call_PyType_GenericAlloc;
    BufferType.tp_new = call_PyType_GenericNew;
    BufferType.tp_free = call_PyObject_Free;
#否则
    BufferType.tp_getattr = BufferGetattr;
    BufferType.tp_setattr = BufferSetattr;
#endif

    CLEAR_FIELD(WindowType);
    WindowType.tp_name = "vim.window";
    WindowType.tp_basicsize = sizeof(WindowObject);
    WindowType.tp_dealloc = WindowDestructor;
    WindowType.tp_repr = WindowRepr;
    WindowType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    WindowType.tp_doc = "vim Window object";
    WindowType.tp_methods = WindowMethods;
    WindowType.tp_traverse = WindowTraverse;
    WindowType.tp_clear = WindowClear;
#もし PY_MAJOR_VERSION >= 3
    WindowType.tp_getattro = WindowGetattro;
    WindowType.tp_setattro = WindowSetattro;
    WindowType.tp_alloc = call_PyType_GenericAlloc;
    WindowType.tp_new = call_PyType_GenericNew;
    WindowType.tp_free = call_PyObject_Free;
#否则
    WindowType.tp_getattr = WindowGetattr;
    WindowType.tp_setattr = WindowSetattr;
#endif

    CLEAR_FIELD(TabPageType);
    TabPageType.tp_name = "vim.tabpage";
    TabPageType.tp_basicsize = sizeof(TabPageObject);
    TabPageType.tp_dealloc = TabPageDestructor;
    TabPageType.tp_repr = TabPageRepr;
    TabPageType.tp_flags = Py_TPFLAGS_DEFAULT;
    TabPageType.tp_doc = "vim tab page object";
    TabPageType.tp_methods = TabPageMethods;
#もし PY_MAJOR_VERSION >= 3
    TabPageType.tp_getattro = TabPageGetattro;
    TabPageType.tp_alloc = call_PyType_GenericAlloc;
    TabPageType.tp_new = call_PyType_GenericNew;
    TabPageType.tp_free = call_PyObject_Free;
#否则
    TabPageType.tp_getattr = TabPageGetattr;
#endif

    CLEAR_FIELD(BufMapType);
    BufMapType.tp_name = "vim.bufferlist";
    BufMapType.tp_basicsize = sizeof(BufMapObject);
    BufMapType.tp_as_mapping = &BufMapAsMapping;
    BufMapType.tp_flags = Py_TPFLAGS_DEFAULT;
    BufMapType.tp_iter = BufMapIter;
    BufMapType.tp_doc = "vim buffer list";

    CLEAR_FIELD(WinListType);
    WinListType.tp_name = "vim.windowlist";
    WinListType.tp_basicsize = sizeof(WinListObject);
    WinListType.tp_as_sequence = &WinListAsSeq;
    WinListType.tp_flags = Py_TPFLAGS_DEFAULT;
    WinListType.tp_doc = "vim window list";
    WinListType.tp_dealloc = (destructor)WinListDestructor;

    CLEAR_FIELD(TabListType);
    TabListType.tp_name = "vim.tabpagelist";
    TabListType.tp_basicsize = sizeof(TabListObject);
    TabListType.tp_as_sequence = &TabListAsSeq;
    TabListType.tp_flags = Py_TPFLAGS_DEFAULT;
    TabListType.tp_doc = "vim tab page list";

    CLEAR_FIELD(RangeType);
    RangeType.tp_name = "vim.range";
    RangeType.tp_basicsize = sizeof(RangeObject);
    RangeType.tp_dealloc = RangeDestructor;
    RangeType.tp_repr = RangeRepr;
    RangeType.tp_as_sequence = &RangeAsSeq;
    RangeType.tp_as_mapping = &RangeAsMapping;
    RangeType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    RangeType.tp_doc = "vim Range object";
    RangeType.tp_methods = RangeMethods;
    RangeType.tp_traverse = RangeTraverse;
    RangeType.tp_clear = RangeClear;
#もし PY_MAJOR_VERSION >= 3
    RangeType.tp_getattro = RangeGetattro;
    RangeType.tp_alloc = call_PyType_GenericAlloc;
    RangeType.tp_new = call_PyType_GenericNew;
    RangeType.tp_free = call_PyObject_Free;
#否则
    RangeType.tp_getattr = RangeGetattr;
#endif

    CLEAR_FIELD(CurrentType);
    CurrentType.tp_name = "vim.currentdata";
    CurrentType.tp_basicsize = sizeof(CurrentObject);
    CurrentType.tp_flags = Py_TPFLAGS_DEFAULT;
    CurrentType.tp_doc = "vim current object";
    CurrentType.tp_methods = CurrentMethods;
#もし PY_MAJOR_VERSION >= 3
    CurrentType.tp_getattro = CurrentGetattro;
    CurrentType.tp_setattro = CurrentSetattro;
#否则
    CurrentType.tp_getattr = CurrentGetattr;
    CurrentType.tp_setattr = CurrentSetattr;
#endif

    CLEAR_FIELD(DictionaryType);
    DictionaryType.tp_name = "vim.dictionary";
    DictionaryType.tp_basicsize = sizeof(DictionaryObject);
    DictionaryType.tp_dealloc = DictionaryDestructor;
    DictionaryType.tp_as_sequence = &DictionaryAsSeq;
    DictionaryType.tp_as_mapping = &DictionaryAsMapping;
    DictionaryType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE;
    DictionaryType.tp_doc = "dictionary pushing modifications to Vim structure";
    DictionaryType.tp_methods = DictionaryMethods;
    DictionaryType.tp_iter = DictionaryIter;
    DictionaryType.tp_new = DictionaryConstructor;
    DictionaryType.tp_alloc = PyType_GenericAlloc;
#もし PY_MAJOR_VERSION >= 3
    DictionaryType.tp_getattro = DictionaryGetattro;
    DictionaryType.tp_setattro = DictionarySetattro;
#否则
    DictionaryType.tp_getattr = DictionaryGetattr;
    DictionaryType.tp_setattr = DictionarySetattr;
#endif

    CLEAR_FIELD(ListType);
    ListType.tp_name = "vim.list";
    ListType.tp_dealloc = ListDestructor;
    ListType.tp_basicsize = sizeof(ListObject);
    ListType.tp_as_sequence = &ListAsSeq;
    ListType.tp_as_mapping = &ListAsMapping;
    ListType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE;
    ListType.tp_doc = "list pushing modifications to Vim structure";
    ListType.tp_methods = ListMethods;
    ListType.tp_iter = ListIter;
    ListType.tp_new = ListConstructor;
    ListType.tp_alloc = PyType_GenericAlloc;
#もし PY_MAJOR_VERSION >= 3
    ListType.tp_getattro = ListGetattro;
    ListType.tp_setattro = ListSetattro;
#否则
    ListType.tp_getattr = ListGetattr;
    ListType.tp_setattr = ListSetattr;
#endif

    // Tuple type
    CLEAR_FIELD(TupleType);
    TupleType.tp_name = "vim.tuple";
    TupleType.tp_dealloc = TupleDestructor;
    TupleType.tp_basicsize = sizeof(TupleObject);
    TupleType.tp_as_sequence = &TupleAsSeq;
    TupleType.tp_as_mapping = &TupleAsMapping;
    TupleType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE;
    TupleType.tp_doc = "tuple pushing modifications to Vim structure";
    TupleType.tp_methods = TupleMethods;
    TupleType.tp_iter = TupleIter;
    TupleType.tp_new = TupleConstructor;
    TupleType.tp_alloc = PyType_GenericAlloc;
#もし PY_MAJOR_VERSION >= 3
    TupleType.tp_getattro = TupleGetattro;
    TupleType.tp_setattro = TupleSetattro;
#否则
    TupleType.tp_getattr = TupleGetattr;
    TupleType.tp_setattr = TupleSetattr;
#endif

    CLEAR_FIELD(FunctionType);
    FunctionType.tp_name = "vim.function";
    FunctionType.tp_basicsize = sizeof(FunctionObject);
    FunctionType.tp_dealloc = FunctionDestructor;
    FunctionType.tp_call = FunctionCall;
    FunctionType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE;
    FunctionType.tp_doc = "object that calls Vim function";
    FunctionType.tp_methods = FunctionMethods;
    FunctionType.tp_repr = FunctionRepr;
    FunctionType.tp_new = FunctionConstructor;
    FunctionType.tp_alloc = PyType_GenericAlloc;
#もし PY_MAJOR_VERSION >= 3
    FunctionType.tp_getattro = FunctionGetattro;
#否则
    FunctionType.tp_getattr = FunctionGetattr;
#endif

    CLEAR_FIELD(OptionsType);
    OptionsType.tp_name = "vim.options";
    OptionsType.tp_basicsize = sizeof(OptionsObject);
    OptionsType.tp_as_sequence = &OptionsAsSeq;
    OptionsType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    OptionsType.tp_doc = "object for manipulating options";
    OptionsType.tp_iter = OptionsIter;
    OptionsType.tp_as_mapping = &OptionsAsMapping;
    OptionsType.tp_dealloc = OptionsDestructor;
    OptionsType.tp_traverse = OptionsTraverse;
    OptionsType.tp_clear = OptionsClear;

#もし PY_VERSION_HEX < 0x030700f0
    CLEAR_FIELD(LoaderType);
    LoaderType.tp_name = "vim.Loader";
    LoaderType.tp_basicsize = sizeof(LoaderObject);
    LoaderType.tp_flags = Py_TPFLAGS_DEFAULT;
    LoaderType.tp_doc = "vim message object";
    LoaderType.tp_methods = LoaderMethods;
    LoaderType.tp_dealloc = LoaderDestructor;
#endif

#もし PY_MAJOR_VERSION >= 3
    CLEAR_FIELD(vimmodule);
    vimmodule.m_name = "vim";
    vimmodule.m_doc = "Vim Python interface\n";
    vimmodule.m_size = -1;
    vimmodule.m_methods = VimMethods;
#endif
}

    static 整数
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
#もし PY_VERSION_HEX < 0x030700f0
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
    戻る 0;
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
# もし PY_VERSION_HEX < 0x030700f0
    PYTYPE_CLEANUP(LoaderType);
# endif
}
#endif

    static 整数
init_sys_path(void)
{
    PyObject	*path;
    PyObject	*path_hook;
    PyObject	*path_hooks;

    もし (!(path_hook = PyObject_GetAttrString(vim_module, "path_hook")))
	戻る -1;

    もし (!(path_hooks = PySys_GetObject("path_hooks")))
    {
	PyErr_Clear();
	path_hooks = PyList_New(1);
	PyList_SET_ITEM(path_hooks, 0, path_hook);
	もし (PySys_SetObject("path_hooks", path_hooks))
	{
	    Py_DECREF(path_hooks);
	    戻る -1;
	}
	Py_DECREF(path_hooks);
    }
    否则 もし (PyList_Check(path_hooks))
    {
	もし (PyList_Append(path_hooks, path_hook))
	{
	    Py_DECREF(path_hook);
	    戻る -1;
	}
	Py_DECREF(path_hook);
    }
    否则
    {
	VimTryStart();
	emsg(_("Failed to set path hook: sys.path_hooks is not a list\n"
	       "You should now do the following:\n"
	       "- append vim.path_hook to sys.path_hooks\n"
	       "- append vim.VIM_SPECIAL_PATH to sys.path\n"));
	VimTryEnd(); // Discard the error
	Py_DECREF(path_hook);
	戻る 0;
    }

    もし (!(path = PySys_GetObject("path")))
    {
	PyErr_Clear();
	path = PyList_New(1);
	Py_INCREF(vim_special_path_object);
	PyList_SET_ITEM(path, 0, vim_special_path_object);
	もし (PySys_SetObject("path", path))
	{
	    Py_DECREF(path);
	    戻る -1;
	}
	Py_DECREF(path);
    }
    否则 もし (PyList_Check(path))
    {
	もし (PyList_Append(path, vim_special_path_object))
	    戻る -1;
    }
    否则
    {
	VimTryStart();
	emsg(_("Failed to set path: sys.path is not a list\n"
	       "You should now append vim.VIM_SPECIAL_PATH to sys.path"));
	VimTryEnd(); // Discard the error
    }

    戻る 0;
}

static struct numeric_constant {
    char	*name;
    整数		val;
} numeric_constants[] = {
    {"VAR_LOCKED",	VAR_LOCKED},
    {"VAR_FIXED",	VAR_FIXED},
    {"VAR_SCOPE",	VAR_SCOPE},
    {"VAR_DEF_SCOPE",	VAR_DEF_SCOPE},
};

struct object_constant {
    char	*name;
    PyObject	*valObject;
};

#define ADD_OBJECT(m, name, obj) \
    もし (PyModule_AddObject(m, name, obj)) \
	戻る -1;

#define ADD_CHECKED_OBJECT(m, name, obj) \
    { \
	PyObject	*valObject = obj; \
	もし (!valObject) \
	    戻る -1; \
	ADD_OBJECT(m, name, valObject); \
    }

    static 整数
populate_module(PyObject *m)
{
    整数		i;
    PyObject	*other_module;
    PyObject	*attr;
    PyObject	*imp;
#もし PY_VERSION_HEX >= 0x030700f0
    PyObject	*dict;
    PyObject	*cls;
#endif

    ために (i = 0; i < (整数)(sizeof(numeric_constants)
					   / sizeof(struct numeric_constant));
	    ++i)
	ADD_CHECKED_OBJECT(m, numeric_constants[i].name,
		PyInt_FromLong(numeric_constants[i].val));

    struct object_constant object_constants[] = {
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
#もし PY_VERSION_HEX < 0x030700f0
	{"_Loader",    (PyObject *)LoaderTypePtr},
#endif
    };

    ために (i = 0; i < (整数)(sizeof(object_constants)
					    / sizeof(struct object_constant));
	    ++i)
    {
	PyObject	*valObject;

	valObject = object_constants[i].valObject;
	Py_INCREF(valObject);
	ADD_OBJECT(m, object_constants[i].name, valObject);
    }

    もし (!(VimError = PyErr_NewException("vim.error", NULL, NULL)))
	戻る -1;
    ADD_OBJECT(m, "error", VimError);

    ADD_CHECKED_OBJECT(m, "vars",  NEW_DICTIONARY(get_globvar_dict()));
    ADD_CHECKED_OBJECT(m, "vvars", NEW_DICTIONARY(get_vimvar_dict()));
    ADD_CHECKED_OBJECT(m, "options",
	    OptionsNew(SREQ_GLOBAL, NULL, dummy_check, NULL));

    もし (!(other_module = PyImport_ImportModule("os")))
	戻る -1;
    ADD_OBJECT(m, "os", other_module);

#もし PY_MAJOR_VERSION >= 3
    もし (!(py_getcwd = PyObject_GetAttrString(other_module, "getcwd")))
	戻る -1;
#否则
    もし (!(py_getcwd = PyObject_GetAttrString(other_module, "getcwdu")))
	戻る -1;
#endif
    ADD_OBJECT(m, "_getcwd", py_getcwd)

    もし (!(py_chdir = PyObject_GetAttrString(other_module, "chdir")))
	戻る -1;
    ADD_OBJECT(m, "_chdir", py_chdir);
    もし (!(attr = PyObject_GetAttrString(m, "chdir")))
	戻る -1;
    もし (PyObject_SetAttrString(other_module, "chdir", attr))
    {
	Py_DECREF(attr);
	戻る -1;
    }
    Py_DECREF(attr);

    もし ((py_fchdir = PyObject_GetAttrString(other_module, "fchdir")))
    {
	ADD_OBJECT(m, "_fchdir", py_fchdir);
	もし (!(attr = PyObject_GetAttrString(m, "fchdir")))
	    戻る -1;
	もし (PyObject_SetAttrString(other_module, "fchdir", attr))
	{
	    Py_DECREF(attr);
	    戻る -1;
	}
	Py_DECREF(attr);
    }
    否则
	PyErr_Clear();

    もし (!(vim_special_path_object = PyString_FromString(vim_special_path)))
	戻る -1;

    ADD_OBJECT(m, "VIM_SPECIAL_PATH", vim_special_path_object);

#もし PY_VERSION_HEX >= 0x030700f0
    もし (!(imp = PyImport_ImportModule("importlib.machinery")))
	戻る -1;

    dict = PyModule_GetDict(imp);

    もし (!(cls = PyDict_GetItemString(dict, "PathFinder")))
    {
	Py_DECREF(imp);
	戻る -1;
    }

    もし (!(py_find_spec = PyObject_GetAttrString(cls, "find_spec")))
    {
	Py_DECREF(imp);
	戻る -1;
    }

# もし PY_VERSION_HEX < 0x30c00a7
    // find_module has been removed as of Python 3.12.0a7
    もし ((py_find_module = PyObject_GetAttrString(cls, "find_module")))
	ADD_OBJECT(m, "_find_module", py_find_module);
# endif

    Py_DECREF(imp);

    ADD_OBJECT(m, "_find_spec", py_find_spec);
#否则
    もし (!(imp = PyImport_ImportModule("imp")))
	戻る -1;

    もし (!(py_find_module = PyObject_GetAttrString(imp, "find_module")))
    {
	Py_DECREF(imp);
	戻る -1;
    }

    もし (!(py_load_module = PyObject_GetAttrString(imp, "load_module")))
    {
	Py_DECREF(py_find_module);
	Py_DECREF(imp);
	戻る -1;
    }

    Py_DECREF(imp);

    ADD_OBJECT(m, "_find_module", py_find_module);
    ADD_OBJECT(m, "_load_module", py_load_module);
#endif

    戻る 0;
}
