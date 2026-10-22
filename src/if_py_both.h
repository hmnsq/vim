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

#اگر PY_VERSION_HEX < 0x02050000
تعریفنوع صحیح Py_ssize_t;  // Python 2.4 and earlier don't have this type.
#endif

// Use values that are known to work, others may make Vim crash.
#define ENC_OPT (enc_utf8 ? "utf-8" : enc_dbcs ? "euc-jp" : (نویسه *)p_enc)
#define DOPY_FUNC "_vim_pydo"

ایستا ثابت نویسه *vim_special_path = "_vim_path_";

#define PyErr_SET_STRING(exc, str) PyErr_SetString(exc, _(str))
#define PyErr_SetVim(str) PyErr_SetString(VimError, str)
#define PyErr_SET_VIM(str) PyErr_SET_STRING(VimError, str)
#define PyErr_FORMAT(exc, str, arg) PyErr_Format(exc, _(str), arg)
#define PyErr_FORMAT2(exc, str, arg1, arg2) PyErr_Format(exc, _(str), arg1,arg2)
#define PyErr_VIM_FORMAT(str, arg) PyErr_FORMAT(VimError, str, arg)

#ifdef USE_LIMITED_API
// Limited Python API. Need to call only exposed functions and remap macros.
// PyTypeObject is an opaque struct.

تعریفنوع ساختار {
    lenfunc sq_length;
    binaryfunc sq_concat;
    ssizeargfunc sq_repeat;
    ssizeargfunc sq_item;
    تهی *was_sq_slice;
    ssizeobjargproc sq_ass_item;
    تهی *was_sq_ass_slice;
    objobjproc sq_contains;

    binaryfunc sq_inplace_concat;
    ssizeargfunc sq_inplace_repeat;
} PySequenceMethods;

تعریفنوع ساختار {
    lenfunc mp_length;
    binaryfunc mp_subscript;
    objobjargproc mp_ass_subscript;
} PyMappingMethods;

// This struct emulates the concrete _typeobject struct to allow the code to
// work the same way in both limited and full Python APIs.
ساختار typeobject_wrapper {
    ثابت نویسه *tp_name;
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

    ثابت نویسه *tp_doc;

    traverseproc tp_traverse;

    inquiry tp_clear;

    getiterfunc tp_iter;
    iternextfunc tp_iternext;

    ساختار PyMethodDef *tp_methods;
    ساختار _typeobject *tp_base;
    allocfunc tp_alloc;
    newfunc tp_new;
    freefunc tp_free;
};

# define DEFINE_PY_TYPE_OBJECT(type) \
    ایستا ساختار typeobject_wrapper type; \
    ایستا PyTypeObject* type##Ptr = پوچ

// PyObject_HEAD_INIT_TYPE and PyObject_FINISH_INIT_TYPE need to come in pairs
// We first initialize with NULL because the type is not allocated until
// init_types() is called later. It's in FINISH_INIT_TYPE where we fill the
// type in with the newly allocated type.
# define PyObject_HEAD_INIT_TYPE(type) PyObject_HEAD_INIT(پوچ)
# define PyObject_FINISH_INIT_TYPE(obj, type) obj.ob_base.ob_type = type##Ptr

# define Py_TYPE_GET_TP_ALLOC(type) ((allocfunc)PyType_GetSlot(type, Py_tp_alloc))
# define Py_TYPE_GET_TP_METHODS(type) ((PyMethodDef *)PyType_GetSlot(type, Py_tp_methods))

// PyObject_NEW is not part of stable ABI, but PyObject_Malloc/Init are.
ایستا PyObject* Vim_PyObject_New(PyTypeObject *type, size_t objsize)
{
    PyObject *obj = (PyObject *)PyObject_Malloc(objsize);
    اگر (obj == پوچ)
	بازگشت PyErr_NoMemory();
    بازگشت PyObject_Init(obj, type);
}
# undef PyObject_NEW
# define PyObject_NEW(type, typeobj) ((type *)Vim_PyObject_New(typeobj, اندازه(type)))

// This is a somewhat convoluted because limited API doesn't expose an easy way
// to get the tp_name field, and so we have to manually reconstruct it as
// "__module__.__name__" (with __module__ omitted for builtins to emulate
// Python behavior). Also, some of the more convenient functions like
// PyUnicode_AsUTF8AndSize and PyType_GetQualName() are not available until
// late Python 3 versions, and won't be available if you set Py_LIMITED_API too
// low.
# define PyErr_FORMAT_TYPE(پیام, obj) \
    انجامبده { \
	PyObject* qualname = PyObject_GetAttrString((PyObject*)(obj)->ob_type, "__qualname__"); \
	اگر (qualname == پوچ) \
	{ \
	    PyErr_FORMAT(PyExc_TypeError, پیام, "(NULL)"); \
	    بشکن; \
	} \
	PyObject* module = PyObject_GetAttrString((PyObject*)(obj)->ob_type, "__module__"); \
	PyObject* full; \
	اگر (module == پوچ || PyUnicode_CompareWithASCIIString(module, "builtins") == 0 \
		|| PyUnicode_CompareWithASCIIString(module, "__main__") == 0) \
	{ \
	    full = qualname; \
	    Py_INCREF(full); \
	} \
	وگرنه \
	    full = PyUnicode_FromFormat("%U.%U", module, qualname); \
	PyObject* full_bytes = PyUnicode_AsUTF8String(full); \
	ثابت نویسه* full_str = PyBytes_AsString(full_bytes); \
	full_str = full_str == پوچ ? "(NULL)" : full_str; \
	PyErr_FORMAT(PyExc_TypeError, پیام, full_str); \
	Py_DECREF(qualname); \
	Py_XDECREF(module); \
	Py_XDECREF(full); \
	Py_XDECREF(full_bytes); \
    } وقتی (0)

# define PyList_GET_ITEM(list, i) PyList_GetItem(list, i)
# define PyList_GET_SIZE(o) PyList_Size(o)

// PyList_SET_ITEM and PyList_SetItem have slightly different behaviors. The
// former will leave the old item dangling, and the latter will decref on it.
// Since we only use this on new lists, this difference doesn't matter.
# define PyList_SET_ITEM(list, i, item) PyList_SetItem(list, i, item)

# define PyTuple_GET_ITEM(o, pos) PyTuple_GetItem(o, pos)
# define PyTuple_GET_SIZE(o) PyTuple_Size(o)

# اگر Py_LIMITED_API < 0x03080000
// PyIter_check only became part of stable ABI in 3.8, and there is no easy way
// to check for it in the API. We simply return false as a compromise. This
// does mean we should avoid compiling with stable ABI < 3.8.
#  undef PyIter_Check
#  define PyIter_Check(obj) (FALSE)
# endif

ایستا PyTypeObject* AddHeapType(ساختار typeobject_wrapper* type_object)
{
    PyType_Spec type_spec;
    type_spec.name = type_object->tp_name;
    type_spec.basicsize = type_object->tp_basicsize;
    type_spec.itemsize = 0;
    type_spec.flags = type_object->tp_flags;

    // We just need to statically allocate a large enough buffer that can hold
    // all slots. We need to leave a null-terminated slot at the end.
    PyType_Slot slots[40] = { {0, پوچ} };
    size_t slot_i = 0;

# define ADD_TP_SLOT(slot_name) \
    اگر (slot_i >= 40) بازگشت پوچ; /* this should never happen */ \
    اگر (type_object->slot_name != پوچ) \
    { \
	slots[slot_i].slot = Py_##slot_name; \
	slots[slot_i].pfunc = (تهی*)type_object->slot_name; \
	++slot_i; \
    }
# define ADD_TP_SUB_SLOT(sub_slot, slot_name) \
    اگر (slot_i >= 40) بازگشت پوچ; /* this should never happen */ \
    اگر (type_object->sub_slot != پوچ && type_object->sub_slot->slot_name != پوچ) \
    { \
	slots[slot_i].slot = Py_##slot_name; \
	slots[slot_i].pfunc = (تهی*)type_object->sub_slot->slot_name; \
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
    بازگشت (PyTypeObject*)newtype;
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
    اگر (type##Ptr == پوچ) \
	بازگشت -1;
# define PYTYPE_CLEANUP(type) \
    type##Ptr = پوچ;

// Limited API does not provide PyRun_* functions. Need to implement manually
// using PyCompile and PyEval.
ایستا PyObject* Vim_PyRun_String(ثابت نویسه *str, صحیح start, PyObject *globals, PyObject *locals)
{
    // Just pass "" for filename for now.
    PyObject* compiled = Py_CompileString(str, "", start);
    اگر (compiled == پوچ)
	بازگشت پوچ;

    PyObject* eval_result = PyEval_EvalCode(compiled, globals, locals);
    Py_DECREF(compiled);
    بازگشت eval_result;
}
ایستا صحیح Vim_PyRun_SimpleString(ثابت نویسه *str)
{
    // This function emulates CPython's implementation.
    PyObject* m = PyImport_AddModule("__main__");
    اگر (m == پوچ)
	بازگشت -1;
    PyObject* d = PyModule_GetDict(m);
    PyObject* output = Vim_PyRun_String(str, Py_file_درون_داد, d, d);
    اگر (output == پوچ)
    {
	PyErr_PrintEx(TRUE);
	بازگشت -1;
    }
    Py_DECREF(output);
    بازگشت 0;
}
# define PyRun_String Vim_PyRun_String
# define PyRun_SimpleString Vim_PyRun_SimpleString

#وگرنه // !defined(USE_LIMITED_API)

// Full Python API. Can make use of structs and macros directly.
# define DEFINE_PY_TYPE_OBJECT(type) \
    ایستا PyTypeObject type; \
    ایستا PyTypeObject* type##Ptr = &type
# define PyObject_HEAD_INIT_TYPE(type) PyObject_HEAD_INIT(&type)

# define Py_TYPE_GET_TP_ALLOC(type) type->tp_alloc
# define Py_TYPE_GET_TP_METHODS(type) type->tp_methods

# define Py_TYPE_NAME(obj) ((obj)->ob_type->tp_name == پوچ \
	? "(NULL)" \
	: (obj)->ob_type->tp_name)
# define PyErr_FORMAT_TYPE(پیام, obj) \
    PyErr_FORMAT(PyExc_TypeError, پیام, \
	    Py_TYPE_NAME(obj))

// Add a static type
# define PYTYPE_READY(type) \
    اگر (PyType_Ready(type##Ptr)) \
	بازگشت -1;

#endif


#define RAISE_NO_EMPTY_کلیدها PyErr_SET_STRING(Py_Exc_مقدار_Error, \
					    N_("empty keys are not allowed"))
#define RAISE_LOCKED_DICTIONARY PyErr_SET_VIM(N_("dictionary is locked"))
#define RAISE_LOCKED_LIST PyErr_SET_VIM(N_("list is locked"))
#define RAISE_UNDO_FAIL PyErr_SET_VIM(N_("cannot save undo information"))
#define RAISE_DELETE_سطر_FAIL PyErr_SET_VIM(N_("cannot delete line"))
#define RAISE_درج_سطر_FAIL PyErr_SET_VIM(N_("cannot insert line"))
#define RAISE_REPLACE_سطر_FAIL PyErr_SET_VIM(N_("cannot replace line"))
#define RAISE_کلید_ADD_FAIL(کلید) \
    PyErr_VIM_FORMAT(N_("failed to add key '%s' to dictionary"), کلید)
#define RAISE_INVALID_INDEX_TYPE(idx) \
    PyErr_FORMAT_TYPE(N_("index must be int or slice, not %s"), idx);

#define INVALID_BUFFER_مقدار ((buf_T *)(-1))
#define INVALID_WINDOW_مقدار ((win_T *)(-1))
#define INVALID_TABPAGE_مقدار ((tabpage_T *)(-1))

تعریفنوع تهی (*rangeinitializer)(تهی *);
تعریفنوع تهی (*runner)(ثابت نویسه *, dict_T *, تهی *
#ifdef PY_CAN_RECURSE
	, PyGILState_STATE *
#endif
	);

ایستا صحیح ConvertFromPyObject(PyObject *, typval_T *);
ایستا صحیح _ConvertFromPyObject(PyObject *, typval_T *, PyObject *);
ایستا صحیح ConvertFromPyMapping(PyObject *, typval_T *);
ایستا صحیح ConvertFromPySequence(PyObject *, typval_T *);
ایستا PyObject *WindowNew(win_T *, tabpage_T *);
ایستا PyObject *BufferNew (buf_T *);
ایستا PyObject *سطر_To_String(ثابت نویسه *);

ایستا PyInt RangeStart;
ایستا PyInt RangeEnd;

ایستا PyObject *globals;

ایستا PyObject *py_chdir;
ایستا PyObject *py_fchdir;
ایستا PyObject *py_getcwd;
ایستا PyObject *vim_module;
ایستا PyObject *vim_special_path_object;

#اگر PY_VERSION_HEX >= 0x030700f0
ایستا PyObject *py_find_spec;
#وگرنه
ایستا PyObject *py_load_module;
#endif
#اگر PY_VERSION_HEX < 0x30c00a7
ایستا PyObject *py_find_module;
#endif

ایستا PyObject *VimError;

/*
 * obtain a lock on the Vim data structures
 */
    ایستا تهی
Python_Lock_Vim(تهی)
{
}

/*
 * release a lock on the Vim data structures
 */
    ایستا تهی
Python_Release_Vim(تهی)
{
}

/*
 * The "todecref" argument holds a pointer to PyObject * that must be
 * DECREF'ed after returned char_u * is no longer needed or NULL if all what
 * was needed to generate returned value is object.
 *
 * Use Py_XDECREF to decrement reference count.
 */
    ایستا char_u *
StringToChars(PyObject *obj, PyObject **todecref)
{
    char_u	*str;

    اگر (PyBytes_Check(obj))
    {

	اگر (PyBytes_AsStringAndSize(obj, (نویسه **) &str, پوچ) == -1
		|| str == پوچ)
	    بازگشت پوچ;

	*todecref = پوچ;
    }
    وگرنه اگر (PyUnicode_Check(obj))
    {
	PyObject	*bytes;

	اگر (!(bytes = PyUnicode_AsEncodedString(obj, ENC_OPT,
							   ERRORS_ENCODE_ARG)))
	    بازگشت پوچ;

	اگر (PyBytes_AsStringAndSize(bytes, (نویسه **) &str, پوچ) == -1
		|| str == پوچ)
	{
	    Py_DECREF(bytes);
	    بازگشت پوچ;
	}

	*todecref = bytes;
    }
    وگرنه
    {
#اگر PY_MAJOR_VERSION < 3
	PyErr_FORMAT_TYPE(N_("expected str() or unicode() instance, but got %s"),
		obj);
#وگرنه
	PyErr_FORMAT_TYPE(N_("expected bytes() or str() instance, but got %s"),
		obj);
#endif
	بازگشت پوچ;
    }

    بازگشت (char_u *) str;
}

#define عدد_LONG     1
#define عدد_INT      2
#define عدد_NATURAL  4
#define عدد_UNSIGNED 8

    ایستا صحیح
عدد_To_Long(PyObject *obj, long *result, صحیح flags)
{
#اگر PY_MAJOR_VERSION < 3
    اگر (PyInt_Check(obj))
    {
	*result = PyInt_AsLong(obj);
	اگر (PyErr_Occurred())
	    بازگشت -1;
    }
    وگرنه
#endif
    اگر (PyLong_Check(obj))
    {
	*result = PyLong_AsLong(obj);
	اگر (PyErr_Occurred())
	    بازگشت -1;
    }
    وگرنه اگر (Py_عدد_Check(obj))
    {
	PyObject	*رقم;

	اگر (!(رقم = Py_عدد_Long(obj)))
	    بازگشت -1;

	*result = PyLong_AsLong(رقم);

	Py_DECREF(رقم);

	اگر (PyErr_Occurred())
	    بازگشت -1;
    }
    وگرنه
    {
#اگر PY_MAJOR_VERSION < 3
	PyErr_FORMAT_TYPE(
		N_("expected int(), long() or something supporting "
		   "coercing to long(), but got %s"),
		obj);
#وگرنه
	PyErr_FORMAT_TYPE(
		N_("expected int() or something supporting coercing to int(), "
		   "but got %s"),
		obj);
#endif
	بازگشت -1;
    }

    اگر (flags & عدد_INT)
    {
	اگر (*result > INT_MAX)
	{
	    PyErr_SET_STRING(PyExc_OverflowError,
		    N_("value is too large to fit into C int type"));
	    بازگشت -1;
	}
	وگرنه اگر (*result < INT_MIN)
	{
	    PyErr_SET_STRING(PyExc_OverflowError,
		    N_("value is too small to fit into C int type"));
	    بازگشت -1;
	}
    }

    اگر (flags & عدد_NATURAL)
    {
	اگر (*result <= 0)
	{
	    PyErr_SET_STRING(Py_Exc_مقدار_Error,
		    N_("number must be greater than zero"));
	    بازگشت -1;
	}
    }
    وگرنه اگر (flags & عدد_UNSIGNED)
    {
	اگر (*result < 0)
	{
	    PyErr_SET_STRING(Py_Exc_مقدار_Error,
		    N_("number must be greater or equal to zero"));
	    بازگشت -1;
	}
    }

    بازگشت 0;
}

    ایستا صحیح
add_string(PyObject *list, نویسه *s)
{
    PyObject	*string;

    اگر (!(string = PyString_FromString(s)))
	بازگشت -1;

    اگر (PyList_Append(list, string))
    {
	Py_DECREF(string);
	بازگشت -1;
    }

    Py_DECREF(string);
    بازگشت 0;
}

    ایستا PyObject *
ObjectDir(PyObject *self, نویسه **attributes)
{
    PyMethodDef	*method;
    نویسه	**attr;
    PyObject	*ret;

    اگر (!(ret = PyList_New(0)))
	بازگشت پوچ;

    اگر (self)
	برای (method = Py_TYPE_GET_TP_METHODS(self->ob_type) ; method->ml_name != پوچ ; ++method)
	    اگر (add_string(ret, (نویسه *)method->ml_name))
	    {
		Py_DECREF(ret);
		بازگشت پوچ;
	    }

    برای (attr = attributes ; *attr ; ++attr)
	اگر (add_string(ret, *attr))
	{
	    Py_DECREF(ret);
	    بازگشت پوچ;
	}

#اگر PY_MAJOR_VERSION < 3
    اگر (add_string(ret, "__members__"))
    {
	Py_DECREF(ret);
	بازگشت پوچ;
    }
#endif

    بازگشت ret;
}

// Output buffer management

// Function to write a line, points to either msg() or emsg().
تعریفنوع صحیح (*writefn)(نویسه *);

DEFINE_PY_TYPE_OBJECT(OutputType);

تعریفنوع ساختار
{
    PyObject_HEAD
    long softspace;
    long error;
} OutputObject;

ایستا نویسه *OutputAttrs[] = {
    "softspace",
    پوچ
};

    ایستا PyObject *
OutputDir(PyObject *self, PyObject *args UNUSED)
{
    بازگشت ObjectDir(self, OutputAttrs);
}

    ایستا صحیح
OutputSetattr(PyObject *self_obj, نویسه *name, PyObject *valObject)
{
    OutputObject *self = (OutputObject*)self_obj;
    اگر (valObject == پوچ)
    {
	PyErr_SET_STRING(PyExc_AttributeError,
		N_("can't delete OutputObject attributes"));
	بازگشت -1;
    }

    اگر (strcmp(name, "softspace") == 0)
    {
	اگر (عدد_To_Long(valObject, &(self->softspace), عدد_UNSIGNED))
	    بازگشت -1;
	بازگشت 0;
    }

    PyErr_FORMAT(PyExc_AttributeError, N_("invalid attribute: %s"), name);
    بازگشت -1;
}

// Buffer IO, we write one whole line at a time.
ایستا garray_T io_ga = {0, 0, 1, 80, پوچ};
ایستا writefn old_fn = پوچ;

    ایستا تهی
PythonIO_Flush(تهی)
{
    اگر (old_fn != پوچ && io_ga.ga_len > 0)
    {
	((نویسه *)io_ga.ga_data)[io_ga.ga_len] = NUL;
	// We don't know what emsg_severe should be here, so ... hope?
	old_fn((نویسه *)io_ga.ga_data);
    }
    io_ga.ga_len = 0;
}

    ایستا تهی
writer(writefn fn, char_u *str, PyInt n, صحیح severe)
{
    char_u *ptr;

    // Flush when switching output function.
    اگر (fn != old_fn)
	PythonIO_Flush();
    old_fn = fn;

    // Write each NL separated line.  Text after the last NL is kept for
    // writing later.
    // For normal messages: Do not output when "got_int" was set.  This avoids
    // a loop gone crazy flooding the terminal with messages.  Also for when
    // "q" is pressed at the more-prompt.
    وقتی (n > 0 && (ptr = memchr(str, '\n', n)) != پوچ
					  && (fn == (writefn)emsg || !got_int))
    {
	PyInt len = ptr - str;

	اگر (ga_grow(&io_ga, (صحیح)(len + 1)) == FAIL)
	    بشکن;

	mch_memmove(((نویسه *)io_ga.ga_data) + io_ga.ga_len, str, (size_t)len);
	((نویسه *)io_ga.ga_data)[io_ga.ga_len + len] = NUL;
	emsg_severe = severe;
	fn((نویسه *)io_ga.ga_data);
	str = ptr + 1;
	n -= len + 1;
	io_ga.ga_len = 0;
    }

    // Put the remaining text into io_ga for later printing.
    اگر (n > 0 && (fn == (writefn)emsg || !got_int)
					&& ga_grow(&io_ga, (صحیح)(n + 1)) == OK)
    {
	mch_memmove(((نویسه *)io_ga.ga_data) + io_ga.ga_len, str, (size_t)n);
	io_ga.ga_len += (صحیح)n;
    }
}

    ایستا صحیح
write_output(OutputObject *self, PyObject *string)
{
    Py_ssize_t	len = 0;
    نویسه	*str = پوچ;
    صحیح		error = self->error;

    اگر (!PyArg_Parse(string, "et#", ENC_OPT, &str, &len))
	بازگشت -1;

    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    writer((writefn)(error ? emsg : پیام), (char_u *)str, len, error);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS
    PyMem_Free(str);

    بازگشت 0;
}

    ایستا PyObject *
OutputWrite(OutputObject *self, PyObject *string)
{
    اگر (write_output(self, string))
	بازگشت پوچ;

    Py_INCREF(Py_None);
    بازگشت Py_None;
}

    ایستا PyObject *
OutputWritelines(OutputObject *self, PyObject *seq)
{
    PyObject	*iterator;
    PyObject	*item;

    اگر (!(iterator = PyObject_GetIter(seq)))
	بازگشت پوچ;

    وقتی ((item = PyIter_Next(iterator)))
    {
	اگر (write_output(self, item))
	{
	    Py_DECREF(iterator);
	    Py_DECREF(item);
	    بازگشت پوچ;
	}
	Py_DECREF(item);
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    اگر (PyErr_Occurred())
	بازگشت پوچ;

    Py_INCREF(Py_None);
    بازگشت Py_None;
}

    ایستا PyObject *
AlwaysNone(PyObject *self UNUSED, PyObject *args UNUSED)
{
    // do nothing
    Py_INCREF(Py_None);
    بازگشت Py_None;
}
#define ALWAYS_NONE AlwaysNone(پوچ, پوچ)

    ایستا PyObject *
AlwaysFalse(PyObject *self UNUSED, PyObject *args UNUSED)
{
    // do nothing
    PyObject	*ret = Py_False;
    Py_INCREF(ret);
    بازگشت ret;
}
#define ALWAYS_FALSE AlwaysFalse(پوچ, پوچ)

    ایستا PyObject *
AlwaysTrue(PyObject *self UNUSED, PyObject *args UNUSED)
{
    // do nothing
    PyObject	*ret = Py_True;
    Py_INCREF(ret);
    بازگشت ret;
}
#define ALWAYS_TRUE AlwaysTrue(پوچ, پوچ)

/***************/

ایستا ساختار PyMethodDef OutputMethods[] = {
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
    { پوچ,	    پوچ,				0,		پوچ}
};

ایستا OutputObject Output =
{
    PyObject_HEAD_INIT_TYPE(OutputType)
    0,
    0
};

ایستا OutputObject Error =
{
    PyObject_HEAD_INIT_TYPE(OutputType)
    0,
    1
};

    ایستا صحیح
PythonIO_Init_io(تهی)
{
    اگر (PySys_SetObject("stdout", (PyObject *)(تهی *)&Output))
	بازگشت -1;
    اگر (PySys_SetObject("stderr", (PyObject *)(تهی *)&Error))
	بازگشت -1;

    اگر (PyErr_Occurred())
    {
	emsg(_(e_python_error_initialising_io_object));
	بازگشت -1;
    }

    بازگشت 0;
}

#اگر PY_VERSION_HEX < 0x030700f0
ایستا PyObject *call_load_module(نویسه *name, صحیح len, PyObject *find_module_result);

تعریفنوع ساختار
{
    PyObject_HEAD
    نویسه	*fullname;
    PyObject	*result;
} LoaderObject;
DEFINE_PY_TYPE_OBJECT(LoaderType);

    ایستا تهی
LoaderDestructor(PyObject *self_obj)
{
    LoaderObject *self = (LoaderObject*)self_obj;
    vim_free(self->fullname);
    Py_XDECREF(self->result);
    DESTRUCTOR_FINISH(self);
}

    ایستا PyObject *
LoaderLoadModule(LoaderObject *self, PyObject *args UNUSED)
{
    نویسه	*fullname = self->fullname;
    PyObject	*result = self->result;
    PyObject	*module;

    اگر (!fullname)
    {
	module = result ? result : Py_None;
	Py_INCREF(module);
	بازگشت module;
    }

    module = call_load_module(fullname, (صحیح)STRLEN(fullname), result);

    self->fullname = پوچ;
    self->result = module;

    vim_free(fullname);
    Py_DECREF(result);

    اگر (!module)
    {
	اگر (PyErr_Occurred())
	    بازگشت پوچ;

	Py_INCREF(Py_None);
	بازگشت Py_None;
    }

    Py_INCREF(module);
    بازگشت module;
}

ایستا ساختار PyMethodDef LoaderMethods[] = {
    // name,	    function,				calling,	doc
    {"load_module", (PyCFunction)LoaderLoadModule,	METH_VARARGS,	""},
    { پوچ,	    پوچ,				0,		پوچ}
};
#endif

/*
 * Check to see whether a Vim error has been reported, or a keyboard
 * interrupt has been detected.
 */
    ایستا تهی
VimTryStart(تهی)
{
    ++trylevel;
}

    ایستا صحیح
VimTryEnd(تهی)
{
    --trylevel;
    // Without this it stops processing all subsequent Vim script commands and
    // generates strange error messages if I e.g. try calling Test() in a cycle
    did_emsg = FALSE;
    // Keyboard interrupt should be preferred over anything else
    اگر (got_int)
    {
	اگر (did_throw)
	    discard_current_exception();
	got_int = FALSE;
	PyErr_SetNone(PyExc_KeyboardInterrupt);
	بازگشت -1;
    }
    وگرنه اگر (پیام_list != پوچ && *پیام_list != پوچ)
    {
	صحیح	should_free;
	نویسه	*پیام;

	پیام = get_exception_string(*پیام_list, ET_ERROR, پوچ, &should_free);

	اگر (پیام == پوچ)
	{
	    PyErr_NoMemory();
	    بازگشت -1;
	}

	PyErr_SetVim(پیام);

	free_global_msglist();

	اگر (should_free)
	    vim_free(پیام);

	بازگشت -1;
    }
    وگرنه اگر (!did_throw)
	بازگشت (PyErr_Occurred() ? -1 : 0);
    // Python exception is preferred over Vim one; unlikely to occur though
    وگرنه اگر (PyErr_Occurred())
    {
	discard_current_exception();
	بازگشت -1;
    }
    // Finally transform Vim script exception to python one
    وگرنه
    {
	PyErr_SetVim((نویسه *)current_exception->مقدار);
	discard_current_exception();
	بازگشت -1;
    }
}

    ایستا صحیح
VimCheckInterrupt(تهی)
{
    اگر (got_int)
    {
	PyErr_SetNone(PyExc_KeyboardInterrupt);
	بازگشت 1;
    }
    بازگشت 0;
}

// Vim module - Implementation

    ایستا PyObject *
VimCommand(PyObject *self UNUSED, PyObject *string)
{
    char_u	*cmd;
    PyObject	*ret;
    PyObject	*todecref;

    اگر (!(cmd = StringToChars(string, &todecref)))
	بازگشت پوچ;

    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();

    VimTryStart();
    do_cmdline_cmd(cmd);
    update_screen(UPD_VALID);

    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    اگر (VimTryEnd())
	ret = پوچ;
    وگرنه
	ret = Py_None;

    Py_XINCREF(ret);
    Py_XDECREF(todecref);
    بازگشت ret;
}

/*
 * Function to translate a typval_T into a PyObject; this will recursively
 * translate lists/dictionaries into their Python equivalents.
 *
 * The depth parameter is to avoid infinite recursion, set it to 1 when
 * you call VimToPython.
 */
    ایستا PyObject *
VimToPython(typval_T *our_tv, صحیح depth, PyObject *lookup_dict)
{
    PyObject	*ret;
    PyObject	*newObj;
    نویسه	ptrBuf[اندازه(تهی *) * 2 + 3];

    // Avoid infinite recursion
    اگر (depth > 100)
    {
	Py_INCREF(Py_None);
	ret = Py_None;
	بازگشت ret;
    }

    // Check if we run into a recursive loop.  The item must be in lookup_dict
    // then and we can use it again.
    اگر ((our_tv->v_type == VAR_LIST && our_tv->vval.v_list != پوچ)
	    || (our_tv->v_type == VAR_TUPLE && our_tv->vval.v_tuple != پوچ)
	    || (our_tv->v_type == VAR_DICT && our_tv->vval.v_dict != پوچ))
    {
	sprintf(ptrBuf, "%p",
		our_tv->v_type == VAR_LIST ? (تهی *)our_tv->vval.v_list
		    : our_tv->v_type == VAR_TUPLE ?
			(تهی *)our_tv->vval.v_tuple
			: (تهی *)our_tv->vval.v_dict);

	اگر ((ret = PyDict_GetItemString(lookup_dict, ptrBuf)))
	{
	    Py_INCREF(ret);
	    بازگشت ret;
	}
    }

    اگر (our_tv->v_type == VAR_STRING)
	ret = PyString_FromString(our_tv->vval.v_string == پوچ
					? "" : (نویسه *)our_tv->vval.v_string);
    وگرنه اگر (our_tv->v_type == VAR_عدد)
    {
	نویسه buf[NUMBUFLEN];

	// For backwards compatibility numbers are stored as strings.
	sprintf(buf, "%lld", (long long)our_tv->vval.v_عدد);
	ret = PyString_FromString((نویسه *)buf);
    }
    وگرنه اگر (our_tv->v_type == VAR_FLOAT)
    {
	نویسه buf[NUMBUFLEN];

	sprintf(buf, "%f", our_tv->vval.v_float);
	ret = PyString_FromString((نویسه *)buf);
    }
    وگرنه اگر (our_tv->v_type == VAR_LIST)
    {
	list_T		*list = our_tv->vval.v_list;
	listitem_T	*curr;

	اگر (list == پوچ)
	    بازگشت پوچ;

	اگر (!(ret = PyList_New(0)))
	    بازگشت پوچ;

	اگر (PyDict_SetItemString(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    بازگشت پوچ;
	}

	CHECK_LIST_MATERIALIZE(list);
	FOR_ALL_LIST_اقلام(list, curr)
	{
	    اگر (!(newObj = VimToPython(&curr->li_tv, depth + 1, lookup_dict)))
	    {
		Py_DECREF(ret);
		بازگشت پوچ;
	    }
	    اگر (PyList_Append(ret, newObj))
	    {
		Py_DECREF(newObj);
		Py_DECREF(ret);
		بازگشت پوچ;
	    }
	    Py_DECREF(newObj);
	}
    }
    وگرنه اگر (our_tv->v_type == VAR_TUPLE)
    {
	tuple_T	*tuple = our_tv->vval.v_tuple;
	صحیح	len;

	اگر (tuple == پوچ)
	    بازگشت پوچ;

	len = TUPLE_LEN(tuple);

	ret = PyTuple_New(len);
	اگر (ret == پوچ)
	    بازگشت پوچ;

	برای (صحیح idx = 0; idx < len; idx++)
	{
	    typval_T	*item_tv = TUPLE_ITEM(tuple, idx);

	    newObj = VimToPython(item_tv, depth + 1, lookup_dict);
	    اگر (!newObj)
	    {
		Py_DECREF(ret);
		بازگشت پوچ;
	    }
	    PyTuple_SetItem(ret, idx, newObj);
	}

	اگر (PyDict_SetItemString(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    بازگشت پوچ;
	}
    }
    وگرنه اگر (our_tv->v_type == VAR_DICT)
    {

	hashtab_T	*ht;
	long_u		todo;
	hashitem_T	*hi;
	dictitem_T	*di;

	اگر (our_tv->vval.v_dict == پوچ)
	    بازگشت پوچ;
	ht = &our_tv->vval.v_dict->dv_hashtab;

	اگر (!(ret = PyDict_New()))
	    بازگشت پوچ;

	اگر (PyDict_SetItemString(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    بازگشت پوچ;
	}

	todo = ht->ht_used;
	برای (hi = ht->ht_array; todo > 0; ++hi)
	{
	    اگر (!HASHITEM_EMPTY(hi))
	    {
		--todo;

		di = dict_lookup(hi);
		اگر (!(newObj = VimToPython(&di->di_tv, depth + 1, lookup_dict)))
		{
		    Py_DECREF(ret);
		    بازگشت پوچ;
		}
		اگر (PyDict_SetItemString(ret, (نویسه *)hi->hi_کلید, newObj))
		{
		    Py_DECREF(ret);
		    Py_DECREF(newObj);
		    بازگشت پوچ;
		}
	    }
	}
    }
    وگرنه اگر (our_tv->v_type == VAR_BOOL)
    {
	اگر (our_tv->vval.v_عدد == VVAL_FALSE)
	{
	    ret = Py_False;
	    Py_INCREF(ret);
	}
	وگرنه
	{
	    ret = Py_True;
	    Py_INCREF(ret);
	}
	بازگشت ret;
    }
    وگرنه اگر (our_tv->v_type == VAR_SPECIAL)
    {
	Py_INCREF(Py_None);
	ret = Py_None;
	بازگشت ret;
    }
    وگرنه اگر (our_tv->v_type == VAR_BLOB)
	ret = PyBytes_FromStringAndSize(
		(نویسه*) our_tv->vval.v_blob->bv_ga.ga_data,
		(Py_ssize_t) our_tv->vval.v_blob->bv_ga.ga_len);
    وگرنه
    {
	Py_INCREF(Py_None);
	ret = Py_None;
    }

    بازگشت ret;
}

    ایستا PyObject *
VimEval(PyObject *self UNUSED, PyObject *args)
{
    char_u	*عبارت;
    typval_T	*our_tv;
    PyObject	*string;
    PyObject	*todecref;
    PyObject	*ret;
    PyObject	*lookup_dict;

    اگر (!PyArg_ParseTuple(args, "O", &string))
	بازگشت پوچ;

    اگر (!(عبارت = StringToChars(string, &todecref)))
	بازگشت پوچ;

    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    VimTryStart();
    our_tv = eval_عبارت(عبارت, پوچ);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    Py_XDECREF(todecref);

    اگر (VimTryEnd())
	بازگشت پوچ;

    اگر (our_tv == پوچ)
    {
	PyErr_SET_VIM(N_("invalid expression"));
	بازگشت پوچ;
    }

    // Convert the Vim type into a Python type.  Create a dictionary that's
    // used to check for recursive loops.
    اگر (!(lookup_dict = PyDict_New()))
	ret = پوچ;
    وگرنه
    {
	ret = VimToPython(our_tv, 1, lookup_dict);
	Py_DECREF(lookup_dict);
    }


    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    free_tv(our_tv);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    بازگشت ret;
}

ایستا PyObject *ConvertToPyObject(typval_T *);

    ایستا PyObject *
VimEvalPy(PyObject *self UNUSED, PyObject *string)
{
    typval_T	*our_tv;
    PyObject	*ret;
    char_u	*عبارت;
    PyObject	*todecref;

    اگر (!(عبارت = StringToChars(string, &todecref)))
	بازگشت پوچ;

    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    VimTryStart();
    our_tv = eval_عبارت(عبارت, پوچ);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    Py_XDECREF(todecref);

    اگر (VimTryEnd())
	بازگشت پوچ;

    اگر (our_tv == پوچ)
    {
	PyErr_SET_VIM(N_("invalid expression"));
	بازگشت پوچ;
    }

    ret = ConvertToPyObject(our_tv);
    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    free_tv(our_tv);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    بازگشت ret;
}

    ایستا PyObject *
VimStrwidth(PyObject *self UNUSED, PyObject *string)
{
    char_u	*str;
    PyObject	*todecref;
    صحیح		len;

    اگر (!(str = StringToChars(string, &todecref)))
	بازگشت پوچ;

    len = mb_string2cells(str, (صحیح)STRLEN(str));

    Py_XDECREF(todecref);

    بازگشت PyLong_FromLong(len);
}

    ایستا PyObject *
_VimChdir(PyObject *_chdir, PyObject *args, PyObject *kwargs)
{
    PyObject	*ret;
    PyObject	*newwd;
    PyObject	*todecref;
    char_u	*new_dir;

    اگر (_chdir == پوچ)
	بازگشت پوچ;
    اگر (!(ret = PyObject_Call(_chdir, args, kwargs)))
	بازگشت پوچ;

    اگر (!(newwd = PyObject_CallFunctionObjArgs(py_getcwd, پوچ)))
    {
	Py_DECREF(ret);
	بازگشت پوچ;
    }

    اگر (!(new_dir = StringToChars(newwd, &todecref)))
    {
	Py_DECREF(ret);
	Py_DECREF(newwd);
	بازگشت پوچ;
    }

    VimTryStart();

    اگر (vim_chdir(new_dir))
    {
	Py_DECREF(ret);
	Py_DECREF(newwd);
	Py_XDECREF(todecref);

	اگر (VimTryEnd())
	    بازگشت پوچ;

	PyErr_SET_VIM(N_("failed to change directory"));
	بازگشت پوچ;
    }

    Py_DECREF(newwd);
    Py_XDECREF(todecref);

    post_chdir(CDSCOPE_GLOBAL);

    اگر (VimTryEnd())
    {
	Py_DECREF(ret);
	بازگشت پوچ;
    }

    بازگشت ret;
}

    ایستا PyObject *
VimChdir(PyObject *self UNUSED, PyObject *args, PyObject *kwargs)
{
    بازگشت _VimChdir(py_chdir, args, kwargs);
}

    ایستا PyObject *
VimFchdir(PyObject *self UNUSED, PyObject *args, PyObject *kwargs)
{
    بازگشت _VimChdir(py_fchdir, args, kwargs);
}

تعریفنوع ساختار {
    PyObject	*callable;
    PyObject	*result;
} map_rtp_data;

    ایستا تهی
map_rtp_callback(char_u *path, تهی *_data)
{
    تهی	**data = (تهی **) _data;
    PyObject	*pathObject;
    map_rtp_data	*mr_data = *((map_rtp_data **) data);

    اگر (!(pathObject = PyString_FromString((نویسه *)path)))
    {
	*data = پوچ;
	بازگشت;
    }

    mr_data->result = PyObject_CallFunctionObjArgs(mr_data->callable,
						   pathObject, پوچ);

    Py_DECREF(pathObject);

    اگر (!mr_data->result || mr_data->result != Py_None)
	*data = پوچ;
    وگرنه
    {
	Py_DECREF(mr_data->result);
	mr_data->result = پوچ;
    }
}

    ایستا PyObject *
VimForeachRTP(PyObject *self UNUSED, PyObject *callable)
{
    map_rtp_data	data;

    data.callable = callable;
    data.result = پوچ;

    do_in_runtimepath(پوچ, 0, &map_rtp_callback, &data);

    اگر (data.result == پوچ)
    {
	اگر (PyErr_Occurred())
	    بازگشت پوچ;
	وگرنه
	{
	    Py_INCREF(Py_None);
	    بازگشت Py_None;
	}
    }
    بازگشت data.result;
}

/*
 * _vim_runtimepath_ special path implementation.
 */

    ایستا تهی
map_finder_callback(char_u *path, تهی *_data)
{
    تهی	**data = (تهی **) _data;
    PyObject	*list = *((PyObject **) data);
    PyObject	*pathObject1, *pathObject2;
    نویسه	*pathbuf;
    size_t	pathlen;

    pathlen = STRLEN(path);

#اگر PY_MAJOR_VERSION < 3
# define PY_MAIN_DIR_STRING "python2"
#وگرنه
# define PY_MAIN_DIR_STRING "python3"
#endif
#define PY_ALTERNATE_DIR_STRING "pythonx"

#define PYTHONX_STRING_LENGTH 7 // STRLEN("pythonx")
    اگر (!(pathbuf = PyMem_New(نویسه,
		    pathlen + STRLEN(PATHSEPSTR) + PYTHONX_STRING_LENGTH + 1)))
    {
	PyErr_NoMemory();
	*data = پوچ;
	بازگشت;
    }

    mch_memmove(pathbuf, path, pathlen + 1);
    add_pathsep((char_u *) pathbuf);

    pathlen = STRLEN(pathbuf);
    mch_memmove(pathbuf + pathlen, PY_MAIN_DIR_STRING,
	    PYTHONX_STRING_LENGTH + 1);

    اگر (!(pathObject1 = PyString_FromString(pathbuf)))
    {
	*data = پوچ;
	PyMem_Free(pathbuf);
	بازگشت;
    }

    mch_memmove(pathbuf + pathlen, PY_ALTERNATE_DIR_STRING,
	    PYTHONX_STRING_LENGTH + 1);

    اگر (!(pathObject2 = PyString_FromString(pathbuf)))
    {
	Py_DECREF(pathObject1);
	PyMem_Free(pathbuf);
	*data = پوچ;
	بازگشت;
    }

    PyMem_Free(pathbuf);

    اگر (PyList_Append(list, pathObject1)
	    || PyList_Append(list, pathObject2))
	*data = پوچ;

    Py_DECREF(pathObject1);
    Py_DECREF(pathObject2);
}

    ایستا PyObject *
Vim_GetPaths(PyObject *self UNUSED, PyObject *args UNUSED)
{
    PyObject	*ret;

    اگر (!(ret = PyList_New(0)))
	بازگشت پوچ;

    do_in_runtimepath(پوچ, 0, &map_finder_callback, ret);

    اگر (PyErr_Occurred())
    {
	Py_DECREF(ret);
	بازگشت پوچ;
    }

    بازگشت ret;
}

#اگر PY_VERSION_HEX >= 0x030700f0
    ایستا PyObject *
FinderFindSpec(PyObject *self, PyObject *args)
{
    نویسه	*fullname;
    PyObject	*paths;
    PyObject	*target = Py_None;
    PyObject	*spec;

    اگر (!PyArg_ParseTuple(args, "s|O", &fullname, &target))
	بازگشت پوچ;

    اگر (!(paths = Vim_GetPaths(self, پوچ)))
	بازگشت پوچ;

    spec = PyObject_CallFunction(py_find_spec, "sOO", fullname, paths, target);

    Py_DECREF(paths);

    اگر (!spec)
    {
	اگر (PyErr_Occurred())
	    بازگشت پوچ;

	Py_INCREF(Py_None);
	بازگشت Py_None;
    }

    بازگشت spec;
}

    ایستا PyObject *
FinderFindModule(PyObject* self UNUSED, PyObject* args UNUSED)
{
    // Apparently returning None works.
    Py_INCREF(Py_None);
    بازگشت Py_None;
}
#وگرنه
    ایستا PyObject *
call_load_module(نویسه *name, صحیح len, PyObject *find_module_result)
{
    PyObject	*fd, *pathname, *description;

    اگر (!PyTuple_Check(find_module_result))
    {
	PyErr_FORMAT_TYPE(
		N_("expected 3-tuple as imp.find_module() result, but got %s"),
		find_module_result);
	بازگشت پوچ;
    }
    اگر (PyTuple_GET_SIZE(find_module_result) != 3)
    {
	PyErr_FORMAT(PyExc_TypeError,
		N_("expected 3-tuple as imp.find_module() result, but got "
		   "tuple of size %d"),
		(صحیح) PyTuple_GET_SIZE(find_module_result));
	بازگشت پوچ;
    }

    اگر (!(fd = PyTuple_GET_ITEM(find_module_result, 0))
	    || !(pathname = PyTuple_GET_ITEM(find_module_result, 1))
	    || !(description = PyTuple_GET_ITEM(find_module_result, 2)))
    {
	PyErr_SET_STRING(PyExc_RuntimeError,
		N_("internal error: imp.find_module returned tuple with NULL"));
	بازگشت پوچ;
    }

    بازگشت PyObject_CallFunction(py_load_module,
	    "s#OOO", name, len, fd, pathname, description);
}

    ایستا PyObject *
find_module(نویسه *fullname, نویسه *tail, PyObject *new_path)
{
    PyObject	*find_module_result;
    PyObject	*module;
    نویسه	*dot;

    اگر ((dot = (نویسه *)vim_strchr((char_u *) tail, '.')))
    {
	/*
	 * There is a dot in the name: call find_module recursively without the
	 * first component
	 */
	PyObject	*newest_path;
	صحیح		partlen = (صحیح) (dot - 1 - tail);

	اگر (!(find_module_result = PyObject_CallFunction(py_find_module,
			"s#O", tail, partlen, new_path)))
	{
	    اگر (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_ImportError))
		PyErr_Clear();
	    بازگشت پوچ;
	}

	اگر (!(module = call_load_module(
			fullname,
			((صحیح) (tail - fullname)) + partlen,
			find_module_result)))
	{
	    Py_DECREF(find_module_result);
	    بازگشت پوچ;
	}

	Py_DECREF(find_module_result);

	اگر (!(newest_path = PyObject_GetAttrString(module, "__path__")))
	{
	    Py_DECREF(module);
	    بازگشت پوچ;
	}

	Py_DECREF(module);

	find_module_result = find_module(fullname, dot + 1, newest_path);

	Py_DECREF(newest_path);

	بازگشت find_module_result;
    }
    وگرنه
    {
	اگر (!(find_module_result = PyObject_CallFunction(py_find_module,
			"sO", tail, new_path)))
	{
	    اگر (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_ImportError))
		PyErr_Clear();
	    بازگشت پوچ;
	}

	بازگشت find_module_result;
    }
}

    ایستا PyObject *
FinderFindModule(PyObject *self, PyObject *args)
{
    نویسه	*fullname;
    PyObject	*result;
    PyObject	*new_path;
    LoaderObject	*loader;

    اگر (!PyArg_ParseTuple(args, "s", &fullname))
	بازگشت پوچ;

    اگر (!(new_path = Vim_GetPaths(self, پوچ)))
	بازگشت پوچ;

    result = find_module(fullname, fullname, new_path);

    Py_DECREF(new_path);

    اگر (!result)
    {
	اگر (PyErr_Occurred())
	    بازگشت پوچ;

	Py_INCREF(Py_None);
	بازگشت Py_None;
    }

    اگر (!(fullname = (نویسه *)vim_strsave((char_u *)fullname)))
    {
	Py_DECREF(result);
	PyErr_NoMemory();
	بازگشت پوچ;
    }

    اگر (!(loader = PyObject_NEW(LoaderObject, LoaderTypePtr)))
    {
	vim_free(fullname);
	Py_DECREF(result);
	بازگشت پوچ;
    }

    loader->fullname = fullname;
    loader->result = result;

    بازگشت (PyObject *) loader;
}
#endif

    ایستا PyObject *
VimPathHook(PyObject *self UNUSED, PyObject *args)
{
    نویسه	*path;

    اگر (PyArg_ParseTuple(args, "s", &path)
	    && STRCMP(path, vim_special_path) == 0)
    {
	Py_INCREF(vim_module);
	بازگشت vim_module;
    }

    PyErr_Clear();
    PyErr_SetNone(PyExc_ImportError);
    بازگشت پوچ;
}

/*
 * Vim module - Definitions
 */

ایستا ساختار PyMethodDef VimMethods[] = {
    // name,	    function,			calling,			documentation
    {"command",	    VimCommand,			METH_O,				"Execute a Vim ex-mode command" },
    {"eval",	    VimEval,			METH_VARARGS,			"Evaluate an expression using Vim evaluator" },
    {"bindeval",    VimEvalPy,			METH_O,				"Like eval(), but returns objects attached to Vim ones"},
    {"strwidth",    VimStrwidth,		METH_O,				"Screen string width, counts <Tab> as having width 1"},
    {"chdir",	    (PyCFunction)(تهی *)VimChdir,	METH_VARARGS|METH_KEYWORDS,	"Change directory"},
    {"fchdir",	    (PyCFunction)(تهی *)VimFchdir,	METH_VARARGS|METH_KEYWORDS,	"Change directory"},
    {"foreach_rtp", VimForeachRTP,		METH_O,				"Call given callable for each path in &rtp"},
#اگر PY_VERSION_HEX >= 0x030700f0
    {"find_spec",   FinderFindSpec,		METH_VARARGS,			"Internal use only, returns spec object for any input it receives"},
#endif
    {"find_module", FinderFindModule,		METH_VARARGS,			"Internal use only, returns loader object for any input it receives"},
    {"path_hook",   VimPathHook,		METH_VARARGS,			"Hook function to install in sys.path_hooks"},
    {"_get_paths",  (PyCFunction)Vim_GetPaths,	METH_NOARGS,			"Get &rtp-based additions to sys.path"},
    { پوچ,	    پوچ,			0,				پوچ}
};

/*
 * Generic iterator object
 */

DEFINE_PY_TYPE_OBJECT(IterType);

تعریفنوع PyObject *(*nextfun)(تهی **);
تعریفنوع تهی (*destructorfun)(تهی *);
تعریفنوع صحیح (*traversefun)(تهی *, visitproc, تهی *);
تعریفنوع صحیح (*clearfun)(تهی **);

// Main purpose of this object is removing the need for do python
// initialization (i.e. PyType_Ready and setting type attributes) for a big
// bunch of objects.
تعریفنوع ساختار
{
    PyObject_HEAD
    تهی *cur;
    nextfun next;
    destructorfun destruct;
    traversefun traverse;
    clearfun clear;
    PyObject *iter_object;
} IterObject;

    ایستا PyObject *
IterNew(تهی *start, destructorfun destruct, nextfun next, traversefun traverse,
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

    اگر (iter_object)
	Py_INCREF(iter_object);

    بازگشت (PyObject *)(self);
}

    ایستا تهی
IterDestructor(PyObject *self_obj)
{
    IterObject *self = (IterObject*)self_obj;
    اگر (self->iter_object)
	Py_DECREF(self->iter_object);
    PyObject_GC_UnTrack((تهی *)(self));
    self->destruct(self->cur);
    PyObject_GC_Del((تهی *)(self));
}

    ایستا صحیح
IterTraverse(PyObject *self_obj, visitproc visit, تهی *arg)
{
    IterObject *self = (IterObject*)self_obj;
    اگر (self->traverse != پوچ)
	بازگشت self->traverse(self->cur, visit, arg);
    وگرنه
	بازگشت 0;
}

// Mac OSX defines clear() somewhere.
#ifdef clear
# undef clear
#endif

    ایستا صحیح
IterClear(PyObject *self_obj)
{
    IterObject *self = (IterObject*)self_obj;
    اگر (self->clear != پوچ)
	بازگشت self->clear(&self->cur);
    وگرنه
	بازگشت 0;
}

    ایستا PyObject *
IterNext(PyObject *self_obj)
{
    IterObject *self = (IterObject*)self_obj;
    بازگشت self->next(&self->cur);
}

    ایستا PyObject *
IterIter(PyObject *self)
{
    Py_INCREF(self);
    بازگشت self;
}

تعریفنوع ساختار pylinkedlist_S {
    ساختار pylinkedlist_S	*pll_next;
    ساختار pylinkedlist_S	*pll_prev;
    PyObject			*pll_obj;
} pylinkedlist_T;

ایستا pylinkedlist_T *lastdict = پوچ;
ایستا pylinkedlist_T *lastlist = پوچ;
ایستا pylinkedlist_T *lasttuple = پوچ;
ایستا pylinkedlist_T *lastfunc = پوچ;

    ایستا تهی
pyll_remove(pylinkedlist_T *ref, pylinkedlist_T **last)
{
    اگر (ref->pll_prev == پوچ)
    {
	اگر (ref->pll_next == پوچ)
	{
	    *last = پوچ;
	    بازگشت;
	}
    }
    وگرنه
	ref->pll_prev->pll_next = ref->pll_next;

    اگر (ref->pll_next == پوچ)
	*last = ref->pll_prev;
    وگرنه
	ref->pll_next->pll_prev = ref->pll_prev;
}

    ایستا تهی
pyll_add(PyObject *self, pylinkedlist_T *ref, pylinkedlist_T **last)
{
    اگر (*last == پوچ)
	ref->pll_prev = پوچ;
    وگرنه
    {
	(*last)->pll_next = ref;
	ref->pll_prev = *last;
    }
    ref->pll_next = پوچ;
    ref->pll_obj = self;
    *last = ref;
}

DEFINE_PY_TYPE_OBJECT(DictionaryType);

تعریفنوع ساختار
{
    PyObject_HEAD
    dict_T	*dict;
    pylinkedlist_T	ref;
} DictionaryObject;

ایستا PyObject *DictionaryUpdate(DictionaryObject *, PyObject *, PyObject *);

#define NEW_DICTIONARY(dict) DictionaryNew(DictionaryTypePtr, dict)

    ایستا PyObject *
DictionaryNew(PyTypeObject *subtype, dict_T *dict)
{
    DictionaryObject	*self;

    self = (DictionaryObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    اگر (self == پوچ)
	بازگشت پوچ;
    self->dict = dict;
    ++dict->dv_refcount;

    pyll_add((PyObject *)(self), &self->ref, &lastdict);

    بازگشت (PyObject *)(self);
}

    ایستا dict_T *
py_dict_alloc(تهی)
{
    dict_T	*ret;

    اگر (!(ret = dict_alloc()))
    {
	PyErr_NoMemory();
	بازگشت پوچ;
    }
    ++ret->dv_refcount;

    بازگشت ret;
}

    ایستا PyObject *
DictionaryConstructor(PyTypeObject *subtype, PyObject *args, PyObject *kwargs)
{
    DictionaryObject	*self;
    dict_T	*dict;

    اگر (!(dict = py_dict_alloc()))
	بازگشت پوچ;

    self = (DictionaryObject *) DictionaryNew(subtype, dict);

    --dict->dv_refcount;

    اگر (kwargs || PyTuple_Size(args))
    {
	PyObject	*tmp;
	اگر (!(tmp = DictionaryUpdate(self, args, kwargs)))
	{
	    Py_DECREF(self);
	    بازگشت پوچ;
	}

	Py_DECREF(tmp);
    }

    بازگشت (PyObject *)(self);
}

    ایستا تهی
DictionaryDestructor(PyObject *self_obj)
{
    DictionaryObject *self = (DictionaryObject*)self_obj;
    pyll_remove(&self->ref, &lastdict);
    dict_unref(self->dict);

    DESTRUCTOR_FINISH(self);
}

ایستا نویسه *DictionaryAttrs[] = {
    "locked", "scope",
    پوچ
};

    ایستا PyObject *
DictionaryDir(PyObject *self, PyObject *args UNUSED)
{
    بازگشت ObjectDir(self, DictionaryAttrs);
}

    ایستا صحیح
DictionarySetattr(PyObject *self_obj, نویسه *name, PyObject *valObject)
{
    DictionaryObject *self = (DictionaryObject*)self_obj;
    اگر (valObject == پوچ)
    {
	PyErr_SET_STRING(PyExc_AttributeError,
		N_("cannot delete vim.Dictionary attributes"));
	بازگشت -1;
    }

    اگر (strcmp(name, "locked") == 0)
    {
	اگر (self->dict->dv_lock == VAR_FIXED)
	{
	    PyErr_SET_STRING(PyExc_TypeError,
		    N_("cannot modify fixed dictionary"));
	    بازگشت -1;
	}
	وگرنه
	{
	    صحیح		istrue = PyObject_IsTrue(valObject);
	    اگر (istrue == -1)
		بازگشت -1;
	    وگرنه اگر (istrue)
		self->dict->dv_lock = VAR_LOCKED;
	    وگرنه
		self->dict->dv_lock = 0;
	}
	بازگشت 0;
    }
    وگرنه
    {
	PyErr_FORMAT(PyExc_AttributeError, N_("cannot set attribute %s"), name);
	بازگشت -1;
    }
}

    ایستا PyInt
DictionaryLength(DictionaryObject *self)
{
    بازگشت ((PyInt) (self->dict->dv_hashtab.ht_used));
}

#define DICT_FLAG_HAS_DEFAULT	0x01
#define DICT_FLAG_برداشت		0x02
#define DICT_FLAG_NONE_DEFAULT	0x04
#define DICT_FLAG_RETURN_BOOL	0x08 // Incompatible with DICT_FLAG_POP
#define DICT_FLAG_RETURN_PAIR	0x10

    ایستا PyObject *
_DictionaryItem(DictionaryObject *self, PyObject *args, صحیح flags)
{
    PyObject	*کلید_Object;
    PyObject	*defObject = ((flags & DICT_FLAG_NONE_DEFAULT)? Py_None : پوچ);
    PyObject	*ret;
    char_u	*کلید;
    dictitem_T	*di;
    dict_T	*dict = self->dict;
    hashitem_T	*hi;
    PyObject	*todecref;

    اگر (flags & DICT_FLAG_HAS_DEFAULT)
    {
	اگر (!PyArg_ParseTuple(args, "O|O", &کلید_Object, &defObject))
	    بازگشت پوچ;
    }
    وگرنه
	کلید_Object = args;

    اگر (flags & DICT_FLAG_RETURN_BOOL)
	defObject = Py_False;

    اگر (!(کلید = StringToChars(کلید_Object, &todecref)))
	بازگشت پوچ;

    اگر (*کلید == NUL)
    {
	RAISE_NO_EMPTY_کلیدها;
	Py_XDECREF(todecref);
	بازگشت پوچ;
    }

    hi = hash_find(&dict->dv_hashtab, کلید);

    Py_XDECREF(todecref);

    اگر (HASHITEM_EMPTY(hi))
    {
	اگر (defObject)
	{
	    Py_INCREF(defObject);
	    بازگشت defObject;
	}
	وگرنه
	{
	    PyErr_SetObject(Py_Exc_کلید_Error, کلید_Object);
	    بازگشت پوچ;
	}
    }
    وگرنه اگر (flags & DICT_FLAG_RETURN_BOOL)
    {
	ret = Py_True;
	Py_INCREF(ret);
	بازگشت ret;
    }

    di = dict_lookup(hi);

    اگر (!(ret = ConvertToPyObject(&di->di_tv)))
	بازگشت پوچ;

    اگر (flags & DICT_FLAG_برداشت)
    {
	اگر (dict->dv_lock)
	{
	    RAISE_LOCKED_DICTIONARY;
	    Py_DECREF(ret);
	    بازگشت پوچ;
	}

	hash_remove(&dict->dv_hashtab, hi, "Python remove variable");
	dictitem_free(di);
    }

    بازگشت ret;
}

    ایستا PyObject *
DictionaryItem(DictionaryObject *self, PyObject *کلید_Object)
{
    بازگشت _DictionaryItem(self, کلید_Object, 0);
}

    ایستا صحیح
DictionaryContains(DictionaryObject *self, PyObject *کلید_Object)
{
    PyObject	*rObj = _DictionaryItem(self, کلید_Object, DICT_FLAG_RETURN_BOOL);
    صحیح		ret;

    اگر (rObj == پوچ)
	بازگشت -1;

    ret = (rObj == Py_True);

    Py_DECREF(rObj);

    بازگشت ret;
}

تعریفنوع ساختار
{
    صحیح		dii_changed;
    hashtab_T	*dii_ht;
    hashitem_T	*dii_hi;
    long_u	dii_todo;
} dictiterinfo_T;

    ایستا PyObject *
DictionaryIterNext(تهی **arg)
{
    PyObject	*ret;
    dictiterinfo_T **dii = (dictiterinfo_T**)arg;

    اگر (!(*dii)->dii_todo)
	بازگشت پوچ;

    اگر ((*dii)->dii_ht->ht_changed != (*dii)->dii_changed)
    {
	PyErr_SET_STRING(PyExc_RuntimeError,
		N_("hashtab changed during iteration"));
	بازگشت پوچ;
    }

    وقتی (((*dii)->dii_todo) && HASHITEM_EMPTY((*dii)->dii_hi))
	++((*dii)->dii_hi);

    --((*dii)->dii_todo);

    اگر (!(ret = PyBytes_FromString((نویسه *)(*dii)->dii_hi->hi_کلید)))
	بازگشت پوچ;

    بازگشت ret;
}

    ایستا PyObject *
DictionaryIter(PyObject *self_obj)
{
    DictionaryObject	*self = (DictionaryObject*)self_obj;
    dictiterinfo_T	*dii;
    hashtab_T		*ht;

    اگر (!(dii = PyMem_New(dictiterinfo_T, 1)))
    {
	PyErr_NoMemory();
	بازگشت پوچ;
    }

    ht = &self->dict->dv_hashtab;
    dii->dii_changed = ht->ht_changed;
    dii->dii_ht = ht;
    dii->dii_hi = ht->ht_array;
    dii->dii_todo = ht->ht_used;

    بازگشت IterNew(dii,
	    PyMem_Free, DictionaryIterNext,
	    پوچ, پوچ, (PyObject *)self);
}

    ایستا صحیح
DictionaryAssItem(
	DictionaryObject *self, PyObject *کلید_Object, PyObject *valObject)
{
    char_u	*کلید;
    typval_T	tv;
    dict_T	*dict = self->dict;
    dictitem_T	*di;
    PyObject	*todecref;

    اگر (dict->dv_lock)
    {
	RAISE_LOCKED_DICTIONARY;
	بازگشت -1;
    }

    اگر (!(کلید = StringToChars(کلید_Object, &todecref)))
	بازگشت -1;

    اگر (*کلید == NUL)
    {
	RAISE_NO_EMPTY_کلیدها;
	Py_XDECREF(todecref);
	بازگشت -1;
    }

    di = dict_find(dict, کلید, -1);

    اگر (valObject == پوچ)
    {
	hashitem_T	*hi;

	اگر (di == پوچ)
	{
	    Py_XDECREF(todecref);
	    PyErr_SetObject(Py_Exc_کلید_Error, کلید_Object);
	    بازگشت -1;
	}
	hi = hash_find(&dict->dv_hashtab, di->di_کلید);
	hash_remove(&dict->dv_hashtab, hi, "Python remove item");
	dictitem_free(di);
	Py_XDECREF(todecref);
	بازگشت 0;
    }

    اگر (ConvertFromPyObject(valObject, &tv) == -1)
    {
	Py_XDECREF(todecref);
	بازگشت -1;
    }

    اگر (di == پوچ)
    {
	اگر (!(di = dictitem_alloc(کلید)))
	{
	    Py_XDECREF(todecref);
	    PyErr_NoMemory();
	    بازگشت -1;
	}
	di->di_tv.v_type = VAR_UNKNOWN;

	اگر (dict_add(dict, di) == FAIL)
	{
	    dictitem_free(di);
	    RAISE_کلید_ADD_FAIL(کلید);
	    Py_XDECREF(todecref);
	    بازگشت -1;
	}
    }
    وگرنه
	clear_tv(&di->di_tv);

    Py_XDECREF(todecref);

    copy_tv(&tv, &di->di_tv);
    clear_tv(&tv);
    بازگشت 0;
}

تعریفنوع PyObject *(*hi_to_py)(hashitem_T *);

    ایستا PyObject *
DictionaryListObjects(DictionaryObject *self, hi_to_py hiconvert)
{
    dict_T	*dict = self->dict;
    long_u	todo = dict->dv_hashtab.ht_used;
    Py_ssize_t	i = 0;
    PyObject	*ret;
    hashitem_T	*hi;
    PyObject	*newObj;

    ret = PyList_New(todo);
    برای (hi = dict->dv_hashtab.ht_array; todo > 0; ++hi)
    {
	اگر (!HASHITEM_EMPTY(hi))
	{
	    اگر (!(newObj = hiconvert(hi)))
	    {
		Py_DECREF(ret);
		بازگشت پوچ;
	    }
	    PyList_SET_ITEM(ret, i, newObj);
	    --todo;
	    ++i;
	}
    }
    بازگشت ret;
}

    ایستا PyObject *
dict_کلید(hashitem_T *hi)
{
    بازگشت PyBytes_FromString((نویسه *)(hi->hi_کلید));
}

    ایستا PyObject *
Dictionary_List_کلیدها(DictionaryObject *self, PyObject *args UNUSED)
{
    بازگشت DictionaryListObjects(self, dict_کلید);
}

    ایستا PyObject *
dict_val(hashitem_T *hi)
{
    dictitem_T	*di;

    di = dict_lookup(hi);
    بازگشت ConvertToPyObject(&di->di_tv);
}

    ایستا PyObject *
Dictionary_List_مقادیر(DictionaryObject *self, PyObject *args UNUSED)
{
    بازگشت DictionaryListObjects(self, dict_val);
}

    ایستا PyObject *
dict_item(hashitem_T *hi)
{
    PyObject	*کلید_Object;
    PyObject	*valObject;
    PyObject	*ret;

    اگر (!(کلید_Object = dict_کلید(hi)))
	بازگشت پوچ;

    اگر (!(valObject = dict_val(hi)))
    {
	Py_DECREF(کلید_Object);
	بازگشت پوچ;
    }

    ret = Py_Build_مقدار("(OO)", کلید_Object, valObject);

    Py_DECREF(کلید_Object);
    Py_DECREF(valObject);

    بازگشت ret;
}

    ایستا PyObject *
Dictionary_List_اقلام(DictionaryObject *self, PyObject *args UNUSED)
{
    بازگشت DictionaryListObjects(self, dict_item);
}

    ایستا PyObject *
DictionaryUpdate(DictionaryObject *self, PyObject *args, PyObject *kwargs)
{
    dict_T	*dict = self->dict;

    اگر (dict->dv_lock)
    {
	RAISE_LOCKED_DICTIONARY;
	بازگشت پوچ;
    }

    اگر (kwargs)
    {
	typval_T	tv;

	اگر (ConvertFromPyMapping(kwargs, &tv) == -1)
	    بازگشت پوچ;

	VimTryStart();
	dict_extend(self->dict, tv.vval.v_dict, (char_u *) "force", پوچ);
	clear_tv(&tv);
	اگر (VimTryEnd())
	    بازگشت پوچ;
    }
    وگرنه
    {
	PyObject	*obj = پوچ;

	اگر (!PyArg_ParseTuple(args, "|O", &obj))
	    بازگشت پوچ;

	اگر (obj == پوچ)
	{
	    Py_INCREF(Py_None);
	    بازگشت Py_None;
	}

	اگر (PyObject_HasAttrString(obj, "keys"))
	    بازگشت DictionaryUpdate(self, پوچ, obj);
	وگرنه
	{
	    PyObject	*iterator;
	    PyObject	*item;

	    اگر (!(iterator = PyObject_GetIter(obj)))
		بازگشت پوچ;

	    وقتی ((item = PyIter_Next(iterator)))
	    {
		PyObject	*fast;
		PyObject	*کلید_Object;
		PyObject	*valObject;
		PyObject	*todecref;
		char_u		*کلید;
		dictitem_T	*di;
		hashitem_T	*hi;

		اگر (!(fast = PySequence_Fast(item, "")))
		{
		    Py_DECREF(iterator);
		    Py_DECREF(item);
		    بازگشت پوچ;
		}

		Py_DECREF(item);

		اگر (PySequence_Size(fast) != 2)
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    PyErr_FORMAT(Py_Exc_مقدار_Error,
			    N_("expected sequence element of size 2, "
			    "but got sequence of size %d"),
			    (صحیح) PySequence_Size(fast));
		    بازگشت پوچ;
		}

		کلید_Object = PySequence_GetItem(fast, 0);

		اگر (!(کلید = StringToChars(کلید_Object, &todecref)))
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    بازگشت پوچ;
		}

		di = dictitem_alloc(کلید);

		Py_XDECREF(todecref);

		اگر (di == پوچ)
		{
		    Py_DECREF(fast);
		    Py_DECREF(iterator);
		    PyErr_NoMemory();
		    بازگشت پوچ;
		}
		di->di_tv.v_type = VAR_UNKNOWN;

		valObject = PySequence_GetItem(fast, 1);

		اگر (ConvertFromPyObject(valObject, &di->di_tv) == -1)
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    dictitem_free(di);
		    بازگشت پوچ;
		}

		Py_DECREF(fast);

		hi = hash_find(&dict->dv_hashtab, di->di_کلید);
		اگر (!HASHITEM_EMPTY(hi) || dict_add(dict, di) == FAIL)
		{
		    RAISE_کلید_ADD_FAIL(di->di_کلید);
		    Py_DECREF(iterator);
		    dictitem_free(di);
		    بازگشت پوچ;
		}
	    }

	    Py_DECREF(iterator);

	    // Iterator may have finished due to an exception
	    اگر (PyErr_Occurred())
		بازگشت پوچ;
	}
    }
    Py_INCREF(Py_None);
    بازگشت Py_None;
}

    ایستا PyObject *
DictionaryGet(DictionaryObject *self, PyObject *args)
{
    بازگشت _DictionaryItem(self, args,
			    DICT_FLAG_HAS_DEFAULT|DICT_FLAG_NONE_DEFAULT);
}

    ایستا PyObject *
Dictionary_برداشت(DictionaryObject *self, PyObject *args)
{
    بازگشت _DictionaryItem(self, args, DICT_FLAG_HAS_DEFAULT|DICT_FLAG_برداشت);
}

    ایستا PyObject *
Dictionary_برداشت_Item(DictionaryObject *self, PyObject *args UNUSED)
{
    hashitem_T	*hi;
    PyObject	*ret;
    PyObject	*valObject;
    dictitem_T	*di;

    اگر (self->dict->dv_hashtab.ht_used == 0)
    {
	PyErr_SetNone(Py_Exc_کلید_Error);
	بازگشت پوچ;
    }

    hi = self->dict->dv_hashtab.ht_array;
    وقتی (HASHITEM_EMPTY(hi))
	++hi;

    di = dict_lookup(hi);

    اگر (!(valObject = ConvertToPyObject(&di->di_tv)))
	بازگشت پوچ;

    اگر (!(ret = Py_Build_مقدار("(" Py_bytes_fmt "O)", hi->hi_کلید, valObject)))
    {
	Py_DECREF(valObject);
	بازگشت پوچ;
    }

    hash_remove(&self->dict->dv_hashtab, hi, "Python pop item");
    dictitem_free(di);

    بازگشت ret;
}

    ایستا PyObject *
Dictionary_Has_کلید(DictionaryObject *self, PyObject *کلید_Object)
{
    بازگشت _DictionaryItem(self, کلید_Object, DICT_FLAG_RETURN_BOOL);
}

ایستا PySequenceMethods DictionaryAsSeq = {
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

ایستا PyMappingMethods DictionaryAsMapping = {
    (lenfunc)       DictionaryLength,
    (binaryfunc)    DictionaryItem,
    (objobjargproc) DictionaryAssItem,
};

ایستا ساختار PyMethodDef DictionaryMethods[] = {
    {"keys",	(PyCFunction)Dictionary_List_کلیدها,	METH_NOARGS,	""},
    {"values",	(PyCFunction)Dictionary_List_مقادیر,	METH_NOARGS,	""},
    {"items",	(PyCFunction)Dictionary_List_اقلام,	METH_NOARGS,	""},
    {"update",	(PyCFunction)(تهی *)DictionaryUpdate,		METH_VARARGS|METH_KEYWORDS, ""},
    {"get",	(PyCFunction)DictionaryGet,		METH_VARARGS,	""},
    {"pop",	(PyCFunction)Dictionary_برداشت,		METH_VARARGS,	""},
    {"popitem",	(PyCFunction)Dictionary_برداشت_Item,		METH_NOARGS,	""},
    {"has_key",	(PyCFunction)Dictionary_Has_کلید,		METH_O,		""},
    {"__dir__",	(PyCFunction)DictionaryDir,		METH_NOARGS,	""},
    { پوچ,	پوچ,					0,		پوچ}
};

DEFINE_PY_TYPE_OBJECT(ListType);

تعریفنوع ساختار
{
    PyObject_HEAD
    list_T	*list;
    pylinkedlist_T	ref;
} ListObject;

#define NEW_LIST(list) ListNew(ListTypePtr, list)

    ایستا PyObject *
ListNew(PyTypeObject *subtype, list_T *list)
{
    ListObject	*self;

    اگر (list == پوچ)
	بازگشت پوچ;

    self = (ListObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    اگر (self == پوچ)
	بازگشت پوچ;
    self->list = list;
    ++list->lv_refcount;
    CHECK_LIST_MATERIALIZE(list);

    pyll_add((PyObject *)(self), &self->ref, &lastlist);

    بازگشت (PyObject *)(self);
}

    ایستا list_T *
py_list_alloc(تهی)
{
    list_T	*ret;

    اگر (!(ret = list_alloc()))
    {
	PyErr_NoMemory();
	بازگشت پوچ;
    }
    ++ret->lv_refcount;

    بازگشت ret;
}

    ایستا صحیح
list_py_concat(list_T *l, PyObject *obj, PyObject *lookup_dict)
{
    PyObject	*iterator;
    PyObject	*item;
    listitem_T	*li;

    اگر (!(iterator = PyObject_GetIter(obj)))
	بازگشت -1;

    وقتی ((item = PyIter_Next(iterator)))
    {
	اگر (!(li = listitem_alloc()))
	{
	    PyErr_NoMemory();
	    Py_DECREF(item);
	    Py_DECREF(iterator);
	    بازگشت -1;
	}
	li->li_tv.v_lock = 0;
	li->li_tv.v_type = VAR_UNKNOWN;

	اگر (_ConvertFromPyObject(item, &li->li_tv, lookup_dict) == -1)
	{
	    Py_DECREF(item);
	    Py_DECREF(iterator);
	    listitem_free(l, li);
	    بازگشت -1;
	}

	Py_DECREF(item);

	list_append(l, li);
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    اگر (PyErr_Occurred())
	بازگشت -1;

    بازگشت 0;
}

    ایستا PyObject *
ListConstructor(PyTypeObject *subtype, PyObject *args, PyObject *kwargs)
{
    list_T	*list;
    PyObject	*obj = پوچ;

    اگر (kwargs)
    {
	PyErr_SET_STRING(PyExc_TypeError,
		N_("list constructor does not accept keyword arguments"));
	بازگشت پوچ;
    }

    اگر (!PyArg_ParseTuple(args, "|O", &obj))
	بازگشت پوچ;

    اگر (!(list = py_list_alloc()))
	بازگشت پوچ;

    اگر (obj)
    {
	PyObject	*lookup_dict;

	اگر (!(lookup_dict = PyDict_New()))
	{
	    list_unref(list);
	    بازگشت پوچ;
	}

	اگر (list_py_concat(list, obj, lookup_dict) == -1)
	{
	    Py_DECREF(lookup_dict);
	    list_unref(list);
	    بازگشت پوچ;
	}

	Py_DECREF(lookup_dict);
    }

    بازگشت ListNew(subtype, list);
}

    ایستا تهی
ListDestructor(PyObject *self_obj)
{
    ListObject *self = (ListObject*)self_obj;
    pyll_remove(&self->ref, &lastlist);
    list_unref(self->list);

    DESTRUCTOR_FINISH(self);
}

    ایستا PyInt
ListLength(ListObject *self)
{
    بازگشت ((PyInt) (self->list->lv_len));
}

    ایستا PyObject *
ListIndex(ListObject *self, Py_ssize_t index)
{
    listitem_T	*li;

    اگر (index >= ListLength(self))
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("list index out of range"));
	بازگشت پوچ;
    }
    li = list_find(self->list, (long) index);
    اگر (li == پوچ)
    {
	// No more suitable format specifications in python-2.3
	PyErr_VIM_FORMAT(N_("internal error: failed to get Vim list item %d"),
		(صحیح) index);
	بازگشت پوچ;
    }
    بازگشت ConvertToPyObject(&li->li_tv);
}

    ایستا PyObject *
ListSlice(ListObject *self, Py_ssize_t first, Py_ssize_t step,
	  Py_ssize_t slicelen)
{
    PyInt	i;
    PyObject	*list;

    اگر (step == 0)
    {
	PyErr_SET_STRING(Py_Exc_مقدار_Error, N_("slice step cannot be zero"));
	بازگشت پوچ;
    }

    list = PyList_New(slicelen);
    اگر (list == پوچ)
	بازگشت پوچ;

    برای (i = 0; i < slicelen; ++i)
    {
	PyObject	*item;

	item = ListIndex(self, first + i*step);
	اگر (item == پوچ)
	{
	    Py_DECREF(list);
	    بازگشت پوچ;
	}

	PyList_SET_ITEM(list, i, item);
    }

    بازگشت list;
}

    ایستا PyObject *
ListItem(ListObject *self, PyObject* idx)
{
#اگر PY_MAJOR_VERSION < 3
    اگر (PyInt_Check(idx))
    {
	long _idx = PyInt_AsLong(idx);
	بازگشت ListIndex(self, _idx);
    }
    وگرنه
#endif
    اگر (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	بازگشت ListIndex(self, _idx);
    }
    وگرنه اگر (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	اگر (PySlice_GetIndicesEx((PySliceObject_T *)idx, ListLength(self),
				 &start, &stop, &step, &slicelen) < 0)
	    بازگشت پوچ;
	بازگشت ListSlice(self, start, step, slicelen);
    }
    وگرنه
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	بازگشت پوچ;
    }
}

    ایستا تهی
list_restore(Py_ssize_t numadded, Py_ssize_t numreplaced, Py_ssize_t slicelen,
	list_T *l, listitem_T **lis, listitem_T *lastaddedli)
{
    وقتی (numreplaced--)
    {
	list_درج(l, lis[numreplaced], lis[slicelen + numreplaced]);
	listitem_remove(l, lis[slicelen + numreplaced]);
    }
    وقتی (numadded--)
    {
	listitem_T	*next;

	next = lastaddedli->li_prev;
	listitem_remove(l, lastaddedli);
	lastaddedli = next;
    }
}

    ایستا صحیح
ListAssSlice(ListObject *self, Py_ssize_t first,
	     Py_ssize_t step, Py_ssize_t slicelen, PyObject *obj)
{
    PyObject	*iterator;
    PyObject	*item;
    listitem_T	*li;
    listitem_T	*lastaddedli = پوچ;
    listitem_T	*next;
    typval_T	v;
    list_T	*l = self->list;
    PyInt	i;
    PyInt	j;
    PyInt	numreplaced = 0;
    PyInt	numadded = 0;
    PyInt	size;
    listitem_T	**lis = پوچ;

    size = ListLength(self);

    اگر (l->lv_lock)
    {
	RAISE_LOCKED_LIST;
	بازگشت -1;
    }

    اگر (step == 0)
    {
	PyErr_SET_STRING(Py_Exc_مقدار_Error, N_("slice step cannot be zero"));
	بازگشت -1;
    }

    اگر (step != 1 && slicelen == 0)
    {
	// Nothing to do. Only error out if obj has some items.
	صحیح		ret = 0;

	اگر (obj == پوچ)
	    بازگشت 0;

	اگر (!(iterator = PyObject_GetIter(obj)))
	    بازگشت -1;

	اگر ((item = PyIter_Next(iterator)))
	{
	    PyErr_FORMAT(Py_Exc_مقدار_Error,
		    N_("attempt to assign sequence of size greater than %d "
			"to extended slice"), 0);
	    Py_DECREF(item);
	    ret = -1;
	}
	Py_DECREF(iterator);
	بازگشت ret;
    }

    اگر (obj != پوچ)
	// XXX May allocate zero bytes.
	اگر (!(lis = PyMem_New(listitem_T *, slicelen * 2)))
	{
	    PyErr_NoMemory();
	    بازگشت -1;
	}

    اگر (first == size)
	li = پوچ;
    وگرنه
    {
	li = list_find(l, (long) first);
	اگر (li == پوچ)
	{
	    PyErr_VIM_FORMAT(N_("internal error: no Vim list item %d"),
		    (صحیح)first);
	    اگر (obj != پوچ)
		PyMem_Free(lis);
	    بازگشت -1;
	}
	i = slicelen;
	وقتی (i-- && li != پوچ)
	{
	    j = step;
	    next = li;
	    اگر (step > 0)
		وقتی (next != پوچ && ((next = next->li_next) != پوچ) && --j);
	    وگرنه
		وقتی (next != پوچ && ((next = next->li_prev) != پوچ) && ++j);

	    اگر (obj == پوچ)
		listitem_remove(l, li);
	    وگرنه
		lis[slicelen - i - 1] = li;

	    li = next;
	}
	اگر (li == پوچ && i != -1)
	{
	    PyErr_SET_VIM(N_("internal error: not enough list items"));
	    اگر (obj != پوچ)
		PyMem_Free(lis);
	    بازگشت -1;
	}
    }

    اگر (obj == پوچ)
	بازگشت 0;

    اگر (!(iterator = PyObject_GetIter(obj)))
    {
	PyMem_Free(lis);
	بازگشت -1;
    }

    i = 0;
    وقتی ((item = PyIter_Next(iterator)))
    {
	اگر (ConvertFromPyObject(item, &v) == -1)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(item);
	    PyMem_Free(lis);
	    بازگشت -1;
	}
	Py_DECREF(item);
	اگر (list_درج_tv(l, &v, numreplaced < slicelen
				    ? lis[numreplaced]
				    : li) == FAIL)
	{
	    clear_tv(&v);
	    PyErr_SET_VIM(N_("internal error: failed to add item to list"));
	    list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	    PyMem_Free(lis);
	    بازگشت -1;
	}
	اگر (numreplaced < slicelen)
	{
	    lis[slicelen + numreplaced] = lis[numreplaced]->li_prev;
	    vimlist_remove(l, lis[numreplaced], lis[numreplaced]);
	    numreplaced++;
	}
	وگرنه
	{
	    اگر (li)
		lastaddedli = li->li_prev;
	    وگرنه
		lastaddedli = l->lv_u.mat.lv_last;
	    numadded++;
	}
	clear_tv(&v);
	اگر (step != 1 && i >= slicelen)
	{
	    Py_DECREF(iterator);
	    PyErr_FORMAT(Py_Exc_مقدار_Error,
		    N_("attempt to assign sequence of size greater than %d "
			"to extended slice"), (صحیح) slicelen);
	    list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	    PyMem_Free(lis);
	    بازگشت -1;
	}
	++i;
    }
    Py_DECREF(iterator);

    اگر (step != 1 && i != slicelen)
    {
	PyErr_FORMAT2(Py_Exc_مقدار_Error,
		N_("attempt to assign sequence of size %d to extended slice "
		    "of size %d"), (صحیح) i, (صحیح) slicelen);
	list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	PyMem_Free(lis);
	بازگشت -1;
    }

    اگر (PyErr_Occurred())
    {
	list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	PyMem_Free(lis);
	بازگشت -1;
    }

    برای (i = 0; i < numreplaced; i++)
	listitem_free(l, lis[i]);
    اگر (step == 1)
	برای (i = numreplaced; i < slicelen; i++)
	    listitem_remove(l, lis[i]);

    PyMem_Free(lis);

    بازگشت 0;
}

    ایستا صحیح
ListAssIndex(ListObject *self, Py_ssize_t index, PyObject *obj)
{
    typval_T	tv;
    list_T	*l = self->list;
    listitem_T	*li;
    Py_ssize_t	length = ListLength(self);

    اگر (l->lv_lock)
    {
	RAISE_LOCKED_LIST;
	بازگشت -1;
    }
    اگر (index > length || (index == length && obj == پوچ))
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("list index out of range"));
	بازگشت -1;
    }

    اگر (obj == پوچ)
    {
	li = list_find(l, (long) index);
	اگر (li == پوچ)
	{
	    PyErr_VIM_FORMAT(N_("internal error: failed to get Vim "
			"list item %d"), (صحیح) index);
	    بازگشت -1;
	}
	vimlist_remove(l, li, li);
	clear_tv(&li->li_tv);
	vim_free(li);
	بازگشت 0;
    }

    اگر (ConvertFromPyObject(obj, &tv) == -1)
	بازگشت -1;

    اگر (index == length)
    {
	اگر (list_append_tv(l, &tv) == FAIL)
	{
	    clear_tv(&tv);
	    PyErr_SET_VIM(N_("failed to add item to list"));
	    بازگشت -1;
	}
    }
    وگرنه
    {
	li = list_find(l, (long) index);
	اگر (li == پوچ)
	{
	    PyErr_VIM_FORMAT(N_("internal error: failed to get Vim "
			"list item %d"), (صحیح) index);
	    بازگشت -1;
	}
	clear_tv(&li->li_tv);
	copy_tv(&tv, &li->li_tv);
	clear_tv(&tv);
    }
    بازگشت 0;
}

    ایستا صحیح
ListAssItem(ListObject *self, PyObject *idx, PyObject *obj)
{
#اگر PY_MAJOR_VERSION < 3
    اگر (PyInt_Check(idx))
    {
	long _idx = PyInt_AsLong(idx);
	بازگشت (صحیح)ListAssIndex(self, _idx, obj);
    }
    وگرنه
#endif
    اگر (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	بازگشت (صحیح)ListAssIndex(self, _idx, obj);
    }
    وگرنه اگر (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	اگر (PySlice_GetIndicesEx((PySliceObject_T *)idx, ListLength(self),
				 &start, &stop, &step, &slicelen) < 0)
	    بازگشت -1;
	بازگشت (صحیح)ListAssSlice(self, start, step, slicelen,
		obj);
    }
    وگرنه
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	بازگشت -1;
    }
}

    ایستا PyObject *
ListConcatInPlace(ListObject *self, PyObject *obj)
{
    list_T	*l = self->list;
    PyObject	*lookup_dict;

    اگر (l->lv_lock)
    {
	RAISE_LOCKED_LIST;
	بازگشت پوچ;
    }

    اگر (!(lookup_dict = PyDict_New()))
	بازگشت پوچ;

    اگر (list_py_concat(l, obj, lookup_dict) == -1)
    {
	Py_DECREF(lookup_dict);
	بازگشت پوچ;
    }
    Py_DECREF(lookup_dict);

    Py_INCREF((PyObject *)self);
    بازگشت (PyObject *)(self);
}

تعریفنوع ساختار
{
    listwatch_T	lw;
    list_T	*list;
} listiterinfo_T;

    ایستا تهی
ListIterDestruct(تهی *arg)
{
    listiterinfo_T *lii = (listiterinfo_T*)arg;
    list_rem_watch(lii->list, &lii->lw);
    list_unref(lii->list);
    PyMem_Free(lii);
}

    ایستا PyObject *
ListIterNext(تهی **arg)
{
    PyObject	*ret;
    listiterinfo_T **lii = (listiterinfo_T**)arg;

    اگر (!((*lii)->lw.lw_item))
	بازگشت پوچ;

    اگر (!(ret = ConvertToPyObject(&((*lii)->lw.lw_item->li_tv))))
	بازگشت پوچ;

    (*lii)->lw.lw_item = (*lii)->lw.lw_item->li_next;

    بازگشت ret;
}

    ایستا PyObject *
ListIter(PyObject *self_obj)
{
    ListObject	*self = (ListObject*)self_obj;
    listiterinfo_T	*lii;
    list_T	*l = self->list;

    اگر (!(lii = PyMem_New(listiterinfo_T, 1)))
    {
	PyErr_NoMemory();
	بازگشت پوچ;
    }

    CHECK_LIST_MATERIALIZE(l);
    list_add_watch(l, &lii->lw);
    lii->lw.lw_item = l->lv_first;
    lii->list = l;
    ++l->lv_refcount;

    بازگشت IterNew(lii,
	    ListIterDestruct, ListIterNext,
	    پوچ, پوچ, (PyObject *)self);
}

ایستا نویسه *ListAttrs[] = {
    "locked",
    پوچ
};

    ایستا PyObject *
ListDir(PyObject *self, PyObject *args UNUSED)
{
    بازگشت ObjectDir(self, ListAttrs);
}

    ایستا صحیح
ListSetattr(PyObject *self_obj, نویسه *name, PyObject *valObject)
{
    ListObject *self = (ListObject*)self_obj;
    اگر (valObject == پوچ)
    {
	PyErr_SET_STRING(PyExc_AttributeError,
		N_("cannot delete vim.List attributes"));
	بازگشت -1;
    }

    اگر (strcmp(name, "locked") == 0)
    {
	اگر (self->list->lv_lock == VAR_FIXED)
	{
	    PyErr_SET_STRING(PyExc_TypeError, N_("cannot modify fixed list"));
	    بازگشت -1;
	}
	وگرنه
	{
	    صحیح		istrue = PyObject_IsTrue(valObject);
	    اگر (istrue == -1)
		بازگشت -1;
	    وگرنه اگر (istrue)
		self->list->lv_lock = VAR_LOCKED;
	    وگرنه
		self->list->lv_lock = 0;
	}
	بازگشت 0;
    }
    وگرنه
    {
	PyErr_FORMAT(PyExc_AttributeError, N_("cannot set attribute %s"), name);
	بازگشت -1;
    }
}

ایستا PySequenceMethods ListAsSeq = {
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

ایستا PyMappingMethods ListAsMapping = {
    /* mp_length	*/ (lenfunc) ListLength,
    /* mp_subscript     */ (binaryfunc) ListItem,
    /* mp_ass_subscript */ (objobjargproc) ListAssItem,
};

ایستا ساختار PyMethodDef ListMethods[] = {
    {"extend",	(PyCFunction)ListConcatInPlace,	METH_O,		""},
    {"__dir__",	(PyCFunction)ListDir,		METH_NOARGS,	""},
    { پوچ,	پوچ,				0,		پوچ}
};

DEFINE_PY_TYPE_OBJECT(TupleType);

تعریفنوع ساختار
{
    PyObject_HEAD
    tuple_T	*tuple;
    pylinkedlist_T	ref;
} TupleObject;

#define NEW_TUPLE(tuple) TupleNew(TupleTypePtr, tuple)

    ایستا PyObject *
TupleNew(PyTypeObject *subtype, tuple_T *tuple)
{
    TupleObject	*self;

    اگر (tuple == پوچ)
	بازگشت پوچ;

    self = (TupleObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    اگر (self == پوچ)
	بازگشت پوچ;
    self->tuple = tuple;
    ++tuple->tv_refcount;

    pyll_add((PyObject *)(self), &self->ref, &lasttuple);

    بازگشت (PyObject *)(self);
}

    ایستا tuple_T *
py_tuple_alloc(تهی)
{
    tuple_T	*ret;

    اگر (!(ret = tuple_alloc()))
    {
	PyErr_NoMemory();
	بازگشت پوچ;
    }
    ++ret->tv_refcount;

    بازگشت ret;
}

    ایستا صحیح
tuple_py_concat(tuple_T *t, PyObject *obj, PyObject *lookup_dict)
{
    PyObject	*iterator;
    PyObject	*item;

    اگر (!(iterator = PyObject_GetIter(obj)))
	بازگشت -1;

    وقتی ((item = PyIter_Next(iterator)))
    {
	typval_T    new_tv;

	اگر (_ConvertFromPyObject(item, &new_tv, lookup_dict) == -1)
	{
	    Py_DECREF(item);
	    Py_DECREF(iterator);
	    بازگشت -1;
	}

	Py_DECREF(item);

	اگر (tuple_append_tv(t, &new_tv) == FAIL)
	{
	    Py_DECREF(iterator);
	    بازگشت -1;
	}
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    اگر (PyErr_Occurred())
	بازگشت -1;

    بازگشت 0;
}

    ایستا PyObject *
TupleConstructor(PyTypeObject *subtype, PyObject *args, PyObject *kwargs)
{
    tuple_T	*tuple;
    PyObject	*obj = پوچ;

    اگر (kwargs)
    {
	PyErr_SET_STRING(PyExc_TypeError,
		N_("tuple constructor does not accept keyword arguments"));
	بازگشت پوچ;
    }

    اگر (!PyArg_ParseTuple(args, "|O", &obj))
	بازگشت پوچ;

    اگر (!(tuple = py_tuple_alloc()))
	بازگشت پوچ;

    اگر (obj)
    {
	PyObject	*lookup_dict;

	اگر (!(lookup_dict = PyDict_New()))
	{
	    tuple_unref(tuple);
	    بازگشت پوچ;
	}

	اگر (tuple_py_concat(tuple, obj, lookup_dict) == -1)
	{
	    Py_DECREF(lookup_dict);
	    tuple_unref(tuple);
	    بازگشت پوچ;
	}

	Py_DECREF(lookup_dict);
    }

    بازگشت TupleNew(subtype, tuple);
}

    ایستا تهی
TupleDestructor(PyObject *self_obj)
{
    TupleObject *self = (TupleObject *)self_obj;
    pyll_remove(&self->ref, &lasttuple);
    tuple_unref(self->tuple);

    DESTRUCTOR_FINISH(self);
}

    ایستا PyInt
TupleLength(TupleObject *self)
{
    بازگشت ((PyInt)(tuple_len(self->tuple)));
}

    ایستا PyObject *
TupleIndex(TupleObject *self, Py_ssize_t index)
{
    PyInt	len = TupleLength(self);

    اگر (index < 0)
	index = len + index;

    اگر (index < 0 || index >= len)
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("tuple index out of range"));
	بازگشت پوچ;
    }
    بازگشت ConvertToPyObject(TUPLE_ITEM(self->tuple, index));
}

/*
 * Return a new tuple object for the tuple slice starting from the index
 * "first" and of length "slicelen" skipping "step" items.
 */
    ایستا PyObject *
TupleSlice(
    TupleObject		*self,
    Py_ssize_t		first,
    Py_ssize_t		step,
    Py_ssize_t		slicelen)
{
    PyInt	i;
    PyObject	*tuple;

    tuple = PyTuple_New(slicelen);
    اگر (tuple == پوچ)
	بازگشت پوچ;

    برای (i = 0; i < slicelen; ++i)
    {
	PyObject	*item;

	item = TupleIndex(self, first + i * step);
	اگر (item == پوچ)
	{
	    Py_DECREF(tuple);
	    بازگشت پوچ;
	}

	PyTuple_SetItem(tuple, i, item);
    }

    بازگشت tuple;
}

    ایستا PyObject *
TupleItem(TupleObject *self, PyObject* idx)
{
#اگر PY_MAJOR_VERSION < 3
    اگر (PyInt_Check(idx))
    {
	long _idx = PyInt_AsLong(idx);
	بازگشت TupleIndex(self, _idx);
    }
    وگرنه
#endif
    اگر (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	بازگشت TupleIndex(self, _idx);
    }
    وگرنه اگر (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	اگر (PySlice_GetIndicesEx((PySliceObject_T *)idx, TupleLength(self),
				 &start, &stop, &step, &slicelen) < 0)
	    بازگشت پوچ;
	بازگشت TupleSlice(self, start, step, slicelen);
    }
    وگرنه
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	بازگشت پوچ;
    }
}

تعریفنوع ساختار
{
    tuple_T	*tuple;
    صحیح		index;
} tupleiterinfo_T;

    ایستا تهی
TupleIterDestruct(تهی *arg)
{
    tupleiterinfo_T *tii = (tupleiterinfo_T*)arg;
    tuple_unref(tii->tuple);
    PyMem_Free(tii);
}

    ایستا PyObject *
TupleIterNext(تهی **arg)
{
    PyObject	*ret;
    tupleiterinfo_T **tii = (tupleiterinfo_T**)arg;

    اگر ((*tii)->index >= TUPLE_LEN((*tii)->tuple))
	بازگشت پوچ;

    اگر (!(ret = ConvertToPyObject(TUPLE_ITEM((*tii)->tuple, (*tii)->index))))
	بازگشت پوچ;

    (*tii)->index++;

    بازگشت ret;
}

    ایستا PyObject *
TupleIter(PyObject *self_obj)
{
    TupleObject	*self = (TupleObject*)self_obj;
    tupleiterinfo_T	*tii;
    tuple_T	*t = self->tuple;

    اگر (!(tii = PyMem_New(tupleiterinfo_T, 1)))
    {
	PyErr_NoMemory();
	بازگشت پوچ;
    }

    tii->tuple = t;
    tii->index = 0;
    ++t->tv_refcount;

    بازگشت IterNew(tii,
	    TupleIterDestruct, TupleIterNext,
	    پوچ, پوچ, (PyObject *)self);
}

ایستا نویسه *TupleAttrs[] = {
    "locked",
    پوچ
};

    ایستا PyObject *
TupleDir(PyObject *self, PyObject *args UNUSED)
{
    بازگشت ObjectDir(self, TupleAttrs);
}

    ایستا صحیح
TupleSetattr(PyObject *self_obj, نویسه *name, PyObject *valObject)
{
    TupleObject *self = (TupleObject*)self_obj;
    اگر (valObject == پوچ)
    {
	PyErr_SET_STRING(PyExc_AttributeError,
		N_("cannot delete vim.Tuple attributes"));
	بازگشت -1;
    }

    اگر (strcmp(name, "locked") == 0)
    {
	اگر (self->tuple->tv_lock == VAR_FIXED)
	{
	    PyErr_SET_STRING(PyExc_TypeError, N_("cannot modify fixed tuple"));
	    بازگشت -1;
	}
	وگرنه
	{
	    صحیح		istrue = PyObject_IsTrue(valObject);
	    اگر (istrue == -1)
		بازگشت -1;
	    وگرنه اگر (istrue)
		self->tuple->tv_lock = VAR_LOCKED;
	    وگرنه
		self->tuple->tv_lock = 0;
	}
	بازگشت 0;
    }
    وگرنه
    {
	PyErr_FORMAT(PyExc_AttributeError, N_("cannot set attribute %s"), name);
	بازگشت -1;
    }
}

ایستا PySequenceMethods TupleAsSeq = {
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

ایستا PyMappingMethods TupleAsMapping = {
    /* mp_length	*/ (lenfunc) TupleLength,
    /* mp_subscript     */ (binaryfunc) TupleItem,
    /* mp_ass_subscript */ (objobjargproc) 0,
};

ایستا ساختار PyMethodDef TupleMethods[] = {
    {"__dir__",	(PyCFunction)TupleDir,		METH_NOARGS,	""},
    { پوچ,	پوچ,				0,		پوچ}
};

تعریفنوع ساختار
{
    PyObject_HEAD
    char_u	*name;
    صحیح		argc;
    typval_T	*argv;
    dict_T	*self;
    pylinkedlist_T	ref;
    صحیح		auto_rebind;
} FunctionObject;

DEFINE_PY_TYPE_OBJECT(FunctionType);

#define NEW_FUNCTION(name, argc, argv, self, pt_auto) \
    FunctionNew(FunctionTypePtr, (name), (argc), (argv), (self), (pt_auto))

    ایستا PyObject *
FunctionNew(PyTypeObject *subtype, char_u *name, صحیح argc, typval_T *argv,
	dict_T *selfdict, صحیح auto_rebind)
{
    FunctionObject	*self;

    self = (FunctionObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    اگر (self == پوچ)
	بازگشت پوچ;

    اگر (isdigit((unsigned نویسه)*name))
    {
	اگر (!translated_function_exists(name, FALSE))
	{
	    PyErr_FORMAT(Py_Exc_مقدار_Error,
		    N_("unnamed function %s does not exist"), name);
	    بازگشت پوچ;
	}
	self->name = vim_strsave(name);
    }
    وگرنه
    {
	char_u *p;

	اگر ((p = get_expanded_name(name,
			    vim_strchr(name, AUTOLOAD_CHAR) == پوچ)) == پوچ)
	{
	    PyErr_FORMAT(Py_Exc_مقدار_Error,
		    N_("function %s does not exist"), name);
	    بازگشت پوچ;
	}

	اگر (p[0] == K_SPECIAL && p[1] == KS_EXTRA && p[2] == (صحیح)KE_SNR)
	{
	    char_u *np;
	    size_t len = STRLEN(p) + 1;

	    اگر ((np = alloc(len + 2)) == پوچ)
	    {
		vim_free(p);
		بازگشت پوچ;
	    }
	    mch_memmove(np, "<SNR>", 5);
	    mch_memmove(np + 5, p + 3, len - 3);
	    vim_free(p);
	    self->name = np;
	}
	وگرنه
	    self->name = p;
    }

    func_ref(self->name);
    self->argc = argc;
    self->argv = argv;
    self->self = selfdict;
    self->auto_rebind = selfdict == پوچ ? TRUE : auto_rebind;

    اگر (self->argv || self->self)
	pyll_add((PyObject *)(self), &self->ref, &lastfunc);

    بازگشت (PyObject *)(self);
}

    ایستا PyObject *
FunctionConstructor(PyTypeObject *subtype, PyObject *args, PyObject *kwargs)
{
    PyObject	*self;
    PyObject	*selfdictObject;
    PyObject	*autoRebindObject;
    PyObject	*argsObject = پوچ;
    char_u	*name;
    typval_T	selfdicttv;
    typval_T	argstv;
    list_T	*argslist = پوچ;
    dict_T	*selfdict = پوچ;
    صحیح		argc = 0;
    صحیح		auto_rebind = TRUE;
    typval_T	*argv = پوچ;
    typval_T	*curtv;
    listitem_T	*li;

    اگر (kwargs != پوچ)
    {
	selfdictObject = PyDict_GetItemString(kwargs, "self");
	اگر (selfdictObject != پوچ)
	{
	    اگر (ConvertFromPyMapping(selfdictObject, &selfdicttv) == -1)
		بازگشت پوچ;
	    selfdict = selfdicttv.vval.v_dict;
	}
	argsObject = PyDict_GetItemString(kwargs, "args");
	اگر (argsObject != پوچ)
	{
	    اگر (ConvertFromPySequence(argsObject, &argstv) == -1)
	    {
		dict_unref(selfdict);
		بازگشت پوچ;
	    }
	    argslist = argstv.vval.v_list;
	    CHECK_LIST_MATERIALIZE(argslist);

	    argc = argslist->lv_len;
	    اگر (argc != 0)
	    {
		argv = PyMem_New(typval_T, (size_t) argc);
		اگر (argv == پوچ)
		{
		    PyErr_NoMemory();
		    dict_unref(selfdict);
		    list_unref(argslist);
		    بازگشت پوچ;
		}
		curtv = argv;
		FOR_ALL_LIST_اقلام(argslist, li)
		    copy_tv(&li->li_tv, curtv++);
	    }
	    list_unref(argslist);
	}
	اگر (selfdict != پوچ)
	{
	    auto_rebind = FALSE;
	    autoRebindObject = PyDict_GetItemString(kwargs, "auto_rebind");
	    اگر (autoRebindObject != پوچ)
	    {
		auto_rebind = PyObject_IsTrue(autoRebindObject);
		اگر (auto_rebind == -1)
		{
		    dict_unref(selfdict);
		    list_unref(argslist);
		    بازگشت پوچ;
		}
	    }
	}
    }

    اگر (!PyArg_ParseTuple(args, "et", "ascii", &name))
    {
	dict_unref(selfdict);
	وقتی (argc--)
	    clear_tv(&argv[argc]);
	PyMem_Free(argv);
	بازگشت پوچ;
    }

    self = FunctionNew(subtype, name, argc, argv, selfdict, auto_rebind);

    PyMem_Free(name);

    بازگشت self;
}

    ایستا تهی
FunctionDestructor(PyObject *self_obj)
{
    FunctionObject *self = (FunctionObject*)self_obj;
    صحیح i;
    func_unref(self->name);
    vim_free(self->name);
    برای (i = 0; i < self->argc; ++i)
	clear_tv(&self->argv[i]);
    PyMem_Free(self->argv);
    dict_unref(self->self);
    اگر (self->argv || self->self)
	pyll_remove(&self->ref, &lastfunc);

    DESTRUCTOR_FINISH(self);
}

ایستا نویسه *FunctionAttrs[] = {
    "softspace", "args", "self", "auto_rebind",
    پوچ
};

    ایستا PyObject *
FunctionDir(PyObject *self, PyObject *args UNUSED)
{
    بازگشت ObjectDir(self, FunctionAttrs);
}

    ایستا PyObject *
FunctionAttr(FunctionObject *self, نویسه *name)
{
    list_T *list;
    صحیح i;
    اگر (strcmp(name, "name") == 0)
	بازگشت PyString_FromString((نویسه *)(self->name));
    وگرنه اگر (strcmp(name, "args") == 0)
    {
	اگر (self->argv == پوچ || (list = list_alloc()) == پوچ)
	    بازگشت ALWAYS_NONE;

	برای (i = 0; i < self->argc; ++i)
	    list_append_tv(list, &self->argv[i]);
	بازگشت NEW_LIST(list);
    }
    وگرنه اگر (strcmp(name, "self") == 0)
	بازگشت self->self == پوچ
	    ? ALWAYS_NONE
	    : NEW_DICTIONARY(self->self);
    وگرنه اگر (strcmp(name, "auto_rebind") == 0)
	بازگشت self->auto_rebind
	    ? ALWAYS_TRUE
	    : ALWAYS_FALSE;
    وگرنه اگر (strcmp(name, "__members__") == 0)
	بازگشت ObjectDir(پوچ, FunctionAttrs);
    بازگشت پوچ;
}

/*
 * Populate partial_T given function object.
 *
 * "exported" should be set to true when it is needed to construct a partial
 * that may be stored in a variable (i.e. may be freed by Vim).
 */
    ایستا تهی
set_partial(FunctionObject *self, partial_T *pt, صحیح exported)
{
    صحیح i;

    pt->pt_name = self->name;
    اگر (self->argv)
    {
	pt->pt_argc = self->argc;
	اگر (exported)
	{
	    pt->pt_argv = ALLOC_CLEAR_MULT(typval_T, self->argc);
	    برای (i = 0; i < pt->pt_argc; ++i)
		copy_tv(&self->argv[i], &pt->pt_argv[i]);
	}
	وگرنه
	    pt->pt_argv = self->argv;
    }
    وگرنه
    {
	pt->pt_argc = 0;
	pt->pt_argv = پوچ;
    }
    pt->pt_auto = self->auto_rebind || !exported;
    pt->pt_dict = self->self;
    اگر (exported && self->self)
	++pt->pt_dict->dv_refcount;
    اگر (exported)
	pt->pt_name = vim_strsave(pt->pt_name);
    pt->pt_refcount = 1;
}

    ایستا PyObject *
FunctionCall(PyObject *self_obj, PyObject *argsObject, PyObject *kwargs)
{
    FunctionObject *self = (FunctionObject*)self_obj;
    char_u	*name = self->name;
    typval_T	args;
    typval_T	selfdicttv;
    typval_T	rettv;
    dict_T	*selfdict = پوچ;
    PyObject	*selfdictObject;
    PyObject	*ret;
    صحیح		error;
    partial_T	pt;
    partial_T	*pt_ptr = پوچ;

    اگر (ConvertFromPySequence(argsObject, &args) == -1)
	بازگشت پوچ;

    اگر (kwargs != پوچ)
    {
	selfdictObject = PyDict_GetItemString(kwargs, "self");
	اگر (selfdictObject != پوچ)
	{
	    اگر (ConvertFromPyMapping(selfdictObject, &selfdicttv) == -1)
	    {
		clear_tv(&args);
		بازگشت پوچ;
	    }
	    selfdict = selfdicttv.vval.v_dict;
	}
    }

    اگر (self->argv || self->self)
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

    اگر (VimTryEnd())
	ret = پوچ;
    وگرنه اگر (error != OK)
    {
	ret = پوچ;
	PyErr_VIM_FORMAT(N_("failed to run function %s"), (نویسه *)name);
    }
    وگرنه
	ret = ConvertToPyObject(&rettv);

    clear_tv(&args);
    clear_tv(&rettv);
    اگر (selfdict != پوچ)
	clear_tv(&selfdicttv);

    بازگشت ret;
}

    ایستا PyObject *
FunctionRepr(PyObject *self_obj)
{
    FunctionObject *self = (FunctionObject*)self_obj;
    PyObject *ret;
    garray_T repr_ga;
    صحیح i;
    char_u *tofree = پوچ;
    typval_T tv;
    char_u numbuf[NUMBUFLEN];

    ga_init2(&repr_ga, اندازه(نویسه), 70);
    ga_concat(&repr_ga, (char_u *)"<vim.Function '");
    اگر (self->name)
	ga_concat(&repr_ga, self->name);
    وگرنه
	ga_concat(&repr_ga, (char_u *)"<NULL>");
    ga_append(&repr_ga, '\'');
    اگر (self->argv)
    {
	ga_concat(&repr_ga, (char_u *)", args=[");
	++emsg_silent;
	برای (i = 0; i < self->argc; i++)
	{
	    اگر (i != 0)
		ga_concat(&repr_ga, (char_u *)", ");
	    ga_concat(&repr_ga, tv2string(&self->argv[i], &tofree, numbuf,
			get_copyID()));
	    vim_free(tofree);
	}
	--emsg_silent;
	ga_append(&repr_ga, ']');
    }
    اگر (self->self)
    {
	ga_concat(&repr_ga, (char_u *)", self=");
	tv.v_type = VAR_DICT;
	tv.vval.v_dict = self->self;
	++emsg_silent;
	ga_concat(&repr_ga, tv2string(&tv, &tofree, numbuf, get_copyID()));
	--emsg_silent;
	vim_free(tofree);
	اگر (self->auto_rebind)
	    ga_concat(&repr_ga, (char_u *)", auto_rebind=True");
    }
    ga_append(&repr_ga, '>');
    ret = PyString_FromString((نویسه *)repr_ga.ga_data);
    ga_clear(&repr_ga);
    بازگشت ret;
}

ایستا ساختار PyMethodDef FunctionMethods[] = {
    {"__dir__",	(PyCFunction)FunctionDir,   METH_NOARGS,		""},
    { پوچ,	پوچ,			0,				پوچ}
};

/*
 * Options object
 */

DEFINE_PY_TYPE_OBJECT(OptionsType);

تعریفنوع صحیح (*checkfun)(تهی *);

تعریفنوع ساختار
{
    PyObject_HEAD
    صحیح		opt_type;
    تهی	*from;
    checkfun	Check;
    PyObject	*fromObj;
} OptionsObject;

    ایستا صحیح
dummy_check(تهی *arg UNUSED)
{
    بازگشت 0;
}

    ایستا PyObject *
OptionsNew(صحیح opt_type, تهی *from, checkfun Check, PyObject *fromObj)
{
    OptionsObject	*self;

    self = PyObject_GC_New(OptionsObject, OptionsTypePtr);
    اگر (self == پوچ)
	بازگشت پوچ;

    self->opt_type = opt_type;
    self->from = from;
    self->Check = Check;
    self->fromObj = fromObj;
    اگر (fromObj)
	Py_INCREF(fromObj);

    بازگشت (PyObject *)(self);
}

    ایستا تهی
OptionsDestructor(PyObject *self_obj)
{
    OptionsObject *self = (OptionsObject*)self_obj;
    PyObject_GC_UnTrack((تهی *)(self));
    Py_XDECREF(self->fromObj);
    PyObject_GC_Del((تهی *)(self));
}

    ایستا صحیح
OptionsTraverse(PyObject *self_obj, visitproc visit, تهی *arg)
{
    OptionsObject *self = (OptionsObject*)self_obj;
    Py_VISIT(self->fromObj);
    بازگشت 0;
}

    ایستا صحیح
OptionsClear(PyObject *self_obj)
{
    OptionsObject *self = (OptionsObject*)self_obj;
    Py_CLEAR(self->fromObj);
    بازگشت 0;
}

    ایستا PyObject *
OptionsItem(OptionsObject *self, PyObject *کلید_Object)
{
    char_u	*کلید;
    صحیح		flags;
    long	numval;
    char_u	*stringval;
    PyObject	*todecref;

    اگر (self->Check(self->fromObj))
	بازگشت پوچ;

    اگر (!(کلید = StringToChars(کلید_Object, &todecref)))
	بازگشت پوچ;

    اگر (*کلید == NUL)
    {
	RAISE_NO_EMPTY_کلیدها;
	Py_XDECREF(todecref);
	بازگشت پوچ;
    }

    flags = get_option_مقدار_strict(کلید, &numval, &stringval,
				    self->opt_type, self->from);

    Py_XDECREF(todecref);

    اگر (flags == 0)
    {
	PyErr_SetObject(Py_Exc_کلید_Error, کلید_Object);
	بازگشت پوچ;
    }

    اگر (flags & SOPT_UNSET)
    {
	Py_INCREF(Py_None);
	بازگشت Py_None;
    }
    وگرنه اگر (flags & SOPT_BOOL)
    {
	PyObject	*ret;
	ret = numval ? Py_True : Py_False;
	Py_INCREF(ret);
	بازگشت ret;
    }
    وگرنه اگر (flags & SOPT_رقم)
	بازگشت PyInt_FromLong(numval);
    وگرنه اگر (flags & SOPT_STRING)
    {
	اگر (stringval)
	{
	    PyObject	*ret = PyBytes_FromString((نویسه *)stringval);
	    vim_free(stringval);
	    بازگشت ret;
	}
	وگرنه
	{
	    PyErr_SET_STRING(PyExc_RuntimeError,
		    N_("unable to get option value"));
	    بازگشت پوچ;
	}
    }
    وگرنه
    {
	PyErr_SET_VIM(N_("internal error: unknown option type"));
	بازگشت پوچ;
    }
}

    ایستا صحیح
OptionsContains(OptionsObject *self, PyObject *کلید_Object)
{
    char_u	*کلید;
    PyObject	*todecref;

    اگر (!(کلید = StringToChars(کلید_Object, &todecref)))
	بازگشت -1;

    اگر (*کلید == NUL)
    {
	Py_XDECREF(todecref);
	بازگشت 0;
    }

    اگر (get_option_مقدار_strict(کلید, پوچ, پوچ, self->opt_type, پوچ))
    {
	Py_XDECREF(todecref);
	بازگشت 1;
    }
    وگرنه
    {
	Py_XDECREF(todecref);
	بازگشت 0;
    }
}

تعریفنوع ساختار
{
    تهی	*lastoption;
    صحیح		opt_type;
} optiterinfo_T;

    ایستا PyObject *
OptionsIterNext(تهی **arg)
{
    char_u	*name;
    optiterinfo_T **oii = (optiterinfo_T**)arg;

    اگر ((name = option_iter_next(&((*oii)->lastoption), (*oii)->opt_type)))
	بازگشت PyString_FromString((نویسه *)name);

    بازگشت پوچ;
}

    ایستا PyObject *
OptionsIter(PyObject *self_obj)
{
    OptionsObject	*self = (OptionsObject*)self_obj;
    optiterinfo_T	*oii;

    اگر (!(oii = PyMem_New(optiterinfo_T, 1)))
    {
	PyErr_NoMemory();
	بازگشت پوچ;
    }

    oii->opt_type = self->opt_type;
    oii->lastoption = پوچ;

    بازگشت IterNew(oii,
	    PyMem_Free, OptionsIterNext,
	    پوچ, پوچ, (PyObject *)self);
}

    ایستا صحیح
set_option_مقدار_err(char_u *کلید, صحیح numval, char_u *stringval, صحیح opt_flags)
{
    نویسه	*errmsg;

    اگر ((errmsg = set_option_مقدار(کلید, numval, stringval, opt_flags)))
    {
	اگر (VimTryEnd())
	    بازگشت FAIL;
	PyErr_SetVim(errmsg);
	بازگشت FAIL;
    }
    بازگشت OK;
}

    ایستا صحیح
set_option_مقدار_for(
	char_u *کلید,
	صحیح numval,
	char_u *stringval,
	صحیح opt_flags,
	صحیح opt_type,
	تهی *from)
{
    switchwin_T	switchwin;
    bufref_T	save_curbuf;
    صحیح		set_ret = 0;

    VimTryStart();
    انتخاب (opt_type)
    {
	حالت SREQ_WIN:
	    اگر (switch_win(&switchwin, (win_T *)from,
			      win_find_tabpage((win_T *)from), FALSE) == FAIL)
	    {
		restore_win(&switchwin, TRUE);
		اگر (VimTryEnd())
		    بازگشت -1;
		PyErr_SET_VIM(N_("problem while switching windows"));
		بازگشت -1;
	    }
	    set_ret = set_option_مقدار_err(کلید, numval, stringval, opt_flags);
	    restore_win(&switchwin, TRUE);
	    بشکن;
	حالت SREQ_BUF:
	    switch_buffer(&save_curbuf, (buf_T *)from);
	    set_ret = set_option_مقدار_err(کلید, numval, stringval, opt_flags);
	    restore_buffer(&save_curbuf);
	    بشکن;
	حالت SREQ_GLOBAL:
	    set_ret = set_option_مقدار_err(کلید, numval, stringval, opt_flags);
	    بشکن;
    }
    اگر (set_ret == FAIL)
	بازگشت -1;
    بازگشت VimTryEnd();
}

    ایستا صحیح
OptionsAssItem(OptionsObject *self, PyObject *کلید_Object, PyObject *valObject)
{
    char_u	*کلید;
    صحیح		flags;
    صحیح		opt_flags;
    صحیح		ret = 0;
    PyObject	*todecref;

    اگر (self->Check(self->fromObj))
	بازگشت -1;

    اگر (!(کلید = StringToChars(کلید_Object, &todecref)))
	بازگشت -1;

    اگر (*کلید == NUL)
    {
	RAISE_NO_EMPTY_کلیدها;
	Py_XDECREF(todecref);
	بازگشت -1;
    }

    flags = get_option_مقدار_strict(کلید, پوچ, پوچ,
				    self->opt_type, self->from);

    اگر (flags == 0)
    {
	PyErr_SetObject(Py_Exc_کلید_Error, کلید_Object);
	Py_XDECREF(todecref);
	بازگشت -1;
    }

    اگر (valObject == پوچ)
    {
	اگر (self->opt_type == SREQ_GLOBAL)
	{
	    PyErr_FORMAT(Py_Exc_مقدار_Error,
		    N_("unable to unset global option %s"), کلید);
	    Py_XDECREF(todecref);
	    بازگشت -1;
	}
	وگرنه اگر (!(flags & SOPT_GLOBAL))
	{
	    PyErr_FORMAT(Py_Exc_مقدار_Error,
		    N_("unable to unset option %s "
		       "which does not have global value"), کلید);
	    Py_XDECREF(todecref);
	    بازگشت -1;
	}
	وگرنه
	{
	    unset_global_local_option(کلید, self->from);
	    Py_XDECREF(todecref);
	    بازگشت 0;
	}
    }

    opt_flags = (self->opt_type ? OPT_LOCAL : OPT_GLOBAL);

    اگر (flags & SOPT_BOOL)
    {
	صحیح	istrue = PyObject_IsTrue(valObject);

	اگر (istrue == -1)
	    ret = -1;
	وگرنه
	    ret = set_option_مقدار_for(کلید, istrue, پوچ,
				    opt_flags, self->opt_type, self->from);
    }
    وگرنه اگر (flags & SOPT_رقم)
    {
	long	val;

	اگر (عدد_To_Long(valObject, &val, عدد_INT))
	{
	    Py_XDECREF(todecref);
	    بازگشت -1;
	}

	ret = set_option_مقدار_for(کلید, (صحیح) val, پوچ, opt_flags,
				self->opt_type, self->from);
    }
    وگرنه
    {
	char_u		*val;
	PyObject	*todecref2;

	اگر ((val = StringToChars(valObject, &todecref2)))
	{
	    ret = set_option_مقدار_for(کلید, 0, val, opt_flags,
				    self->opt_type, self->from);
	    Py_XDECREF(todecref2);
	}
	وگرنه
	    ret = -1;
    }

    Py_XDECREF(todecref);

    بازگشت ret;
}

ایستا PySequenceMethods OptionsAsSeq = {
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

ایستا PyMappingMethods OptionsAsMapping = {
    (lenfunc)       پوچ,
    (binaryfunc)    OptionsItem,
    (objobjargproc) OptionsAssItem,
};

// Tabpage object

تعریفنوع ساختار
{
    PyObject_HEAD
    tabpage_T	*tab;
} TabPageObject;

ایستا PyObject *WinListNew(TabPageObject *tabObject);

DEFINE_PY_TYPE_OBJECT(TabPageType);

    ایستا صحیح
CheckTabPage(TabPageObject *self)
{
    اگر (self->tab == INVALID_TABPAGE_مقدار)
    {
	PyErr_SET_VIM(N_("attempt to refer to deleted tab page"));
	بازگشت -1;
    }

    بازگشت 0;
}

    ایستا PyObject *
TabPageNew(tabpage_T *tab)
{
    TabPageObject *self;

    اگر (TAB_PYTHON_REF(tab))
    {
	self = TAB_PYTHON_REF(tab);
	Py_INCREF((PyObject *)self);
    }
    وگرنه
    {
	self = PyObject_NEW(TabPageObject, TabPageTypePtr);
	اگر (self == پوچ)
	    بازگشت پوچ;
	self->tab = tab;
	TAB_PYTHON_REF(tab) = self;
    }

    بازگشت (PyObject *)(self);
}

    ایستا تهی
TabPageDestructor(PyObject *self_obj)
{
    TabPageObject *self = (TabPageObject*)self_obj;
    اگر (self->tab && self->tab != INVALID_TABPAGE_مقدار)
	TAB_PYTHON_REF(self->tab) = پوچ;

    DESTRUCTOR_FINISH(self);
}

ایستا نویسه *TabPageAttrs[] = {
    "windows", "number", "vars", "window", "valid",
    پوچ
};

    ایستا PyObject *
TabPageDir(PyObject *self, PyObject *args UNUSED)
{
    بازگشت ObjectDir(self, TabPageAttrs);
}

    ایستا PyObject *
TabPageAttrValid(TabPageObject *self, نویسه *name)
{
    PyObject	*ret;

    اگر (strcmp(name, "valid") != 0)
	بازگشت پوچ;

    ret = ((self->tab == INVALID_TABPAGE_مقدار) ? Py_False : Py_True);
    Py_INCREF(ret);
    بازگشت ret;
}

    ایستا PyObject *
TabPageAttr(TabPageObject *self, نویسه *name)
{
    اگر (strcmp(name, "windows") == 0)
	بازگشت WinListNew(self);
    وگرنه اگر (strcmp(name, "number") == 0)
	بازگشت PyLong_FromLong((long) get_tab_عدد(self->tab));
    وگرنه اگر (strcmp(name, "vars") == 0)
	بازگشت NEW_DICTIONARY(self->tab->tp_vars);
    وگرنه اگر (strcmp(name, "window") == 0)
    {
	// For current tab window.c does not bother to set or update tp_curwin
	اگر (self->tab == curtab)
	    بازگشت WindowNew(curwin, curtab);
	وگرنه
	    بازگشت WindowNew(self->tab->tp_curwin, self->tab);
    }
    وگرنه اگر (strcmp(name, "__members__") == 0)
	بازگشت ObjectDir(پوچ, TabPageAttrs);
    بازگشت پوچ;
}

    ایستا PyObject *
TabPageRepr(PyObject *self_obj)
{
    TabPageObject *self = (TabPageObject*)self_obj;
    اگر (self->tab == INVALID_TABPAGE_مقدار)
	بازگشت PyString_FromFormat("<tabpage object (deleted) at %p>", (تهی *)self);
    وگرنه
    {
	صحیح	t = get_tab_عدد(self->tab);

	اگر (t == 0)
	    بازگشت PyString_FromFormat("<tabpage object (unknown) at %p>",
					(تهی *)self);
	وگرنه
	    بازگشت PyString_FromFormat("<tabpage %d>", t - 1);
    }
}

ایستا ساختار PyMethodDef TabPageMethods[] = {
    // name,	    function,			calling,	documentation
    {"__dir__",	    (PyCFunction)TabPageDir,	METH_NOARGS,	""},
    { پوچ,	    پوچ,			0,		پوچ}
};

/*
 * Window list object
 */

DEFINE_PY_TYPE_OBJECT(TabListType);
ایستا PySequenceMethods TabListAsSeq;

تعریفنوع ساختار
{
    PyObject_HEAD
} TabListObject;

ایستا TabListObject TheTabPageList =
{
    PyObject_HEAD_INIT_TYPE(TabListType)
};

    ایستا PyInt
TabListLength(PyObject *self UNUSED)
{
    tabpage_T	*tp = first_tabpage;
    PyInt	n = 0;

    وقتی (tp != پوچ)
    {
	++n;
	tp = tp->tp_next;
    }

    بازگشت n;
}

    ایستا PyObject *
TabListItem(PyObject *self UNUSED, PyInt n)
{
    tabpage_T	*tp;

    برای (tp = first_tabpage; tp != پوچ; tp = tp->tp_next, --n)
	اگر (n == 0)
	    بازگشت TabPageNew(tp);

    PyErr_SET_STRING(PyExc_IndexError, N_("no such tab page"));
    بازگشت پوچ;
}

/*
 * Window object
 */

تعریفنوع ساختار
{
    PyObject_HEAD
    win_T	*win;
    TabPageObject	*tabObject;
} WindowObject;

DEFINE_PY_TYPE_OBJECT(WindowType);

    ایستا صحیح
CheckWindow(WindowObject *self)
{
    اگر (self->win == INVALID_WINDOW_مقدار)
    {
	PyErr_SET_VIM(N_("attempt to refer to deleted window"));
	بازگشت -1;
    }

    بازگشت 0;
}

    ایستا صحیح
CheckWindowCb(تهی *self)
{
    بازگشت CheckWindow((WindowObject*)self);
}

    ایستا PyObject *
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

    اگر (WIN_PYTHON_REF(win))
    {
	self = WIN_PYTHON_REF(win);
	Py_INCREF((PyObject *)self);
    }
    وگرنه
    {
	self = PyObject_GC_New(WindowObject, WindowTypePtr);
	اگر (self == پوچ)
	    بازگشت پوچ;
	self->win = win;
	WIN_PYTHON_REF(win) = self;
    }

    self->tabObject = ((TabPageObject *)(TabPageNew(tab)));

    بازگشت (PyObject *)(self);
}

    ایستا تهی
WindowDestructor(PyObject *self_obj)
{
    WindowObject *self = (WindowObject*)self_obj;
    PyObject_GC_UnTrack((تهی *)(self));
    اگر (self->win && self->win != INVALID_WINDOW_مقدار)
	WIN_PYTHON_REF(self->win) = پوچ;
    Py_XDECREF(((PyObject *)(self->tabObject)));
    PyObject_GC_Del((تهی *)(self));
}

    ایستا صحیح
WindowTraverse(PyObject *self_obj, visitproc visit, تهی *arg)
{
    WindowObject *self = (WindowObject*)self_obj;
    Py_VISIT(((PyObject *)(self->tabObject)));
    بازگشت 0;
}

    ایستا صحیح
WindowClear(PyObject *self_obj)
{
    WindowObject *self = (WindowObject*)self_obj;
    Py_CLEAR(self->tabObject);
    بازگشت 0;
}

    ایستا win_T *
get_firstwin(TabPageObject *tabObject)
{
    اگر (tabObject)
    {
	اگر (CheckTabPage(tabObject))
	    بازگشت پوچ;
	// For current tab window.c does not bother to set or update tp_firstwin
	وگرنه اگر (tabObject->tab == curtab)
	    بازگشت firstwin;
	وگرنه
	    بازگشت tabObject->tab->tp_firstwin;
    }
    وگرنه
	بازگشت firstwin;
}

// Use the same order as in the WindowAttr() function.
ایستا نویسه *WindowAttrs[] = {
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
    پوچ
};

    ایستا PyObject *
WindowDir(PyObject *self, PyObject *args UNUSED)
{
    بازگشت ObjectDir(self, WindowAttrs);
}

    ایستا PyObject *
WindowAttrValid(WindowObject *self, نویسه *name)
{
    PyObject	*ret;

    اگر (strcmp(name, "valid") != 0)
	بازگشت پوچ;

    ret = ((self->win == INVALID_WINDOW_مقدار) ? Py_False : Py_True);
    Py_INCREF(ret);
    بازگشت ret;
}

    ایستا PyObject *
WindowAttr(WindowObject *self, نویسه *name)
{
    اگر (strcmp(name, "buffer") == 0)
	بازگشت (PyObject *)BufferNew(self->win->w_buffer);
    وگرنه اگر (strcmp(name, "cursor") == 0)
    {
	pos_T *pos = &self->win->w_cursor;

	بازگشت Py_Build_مقدار("(ll)", (long)(pos->lnum), (long)(pos->col));
    }
    وگرنه اگر (strcmp(name, "height") == 0)
	بازگشت PyLong_FromLong((long)(self->win->w_height));
    وگرنه اگر (strcmp(name, "row") == 0)
	بازگشت PyLong_FromLong((long)(self->win->w_winrow));
    وگرنه اگر (strcmp(name, "width") == 0)
	بازگشت PyLong_FromLong((long)(self->win->w_width));
    وگرنه اگر (strcmp(name, "col") == 0)
	بازگشت PyLong_FromLong((long)(self->win->w_wincol));
    وگرنه اگر (strcmp(name, "vars") == 0)
	بازگشت NEW_DICTIONARY(self->win->w_vars);
    وگرنه اگر (strcmp(name, "options") == 0)
	بازگشت OptionsNew(SREQ_WIN, self->win, CheckWindowCb,
			(PyObject *) self);
    وگرنه اگر (strcmp(name, "number") == 0)
    {
	اگر (CheckTabPage(self->tabObject))
	    بازگشت پوچ;
	بازگشت PyLong_FromLong((long)
		get_win_عدد(self->win, get_firstwin(self->tabObject)));
    }
    وگرنه اگر (strcmp(name, "tabpage") == 0)
    {
	Py_INCREF((PyObject *)self->tabObject);
	بازگشت (PyObject *)(self->tabObject);
    }
    وگرنه اگر (strcmp(name, "__members__") == 0)
	بازگشت ObjectDir(پوچ, WindowAttrs);
    وگرنه
	بازگشت پوچ;
}

    ایستا صحیح
WindowSetattr(PyObject *self_obj, نویسه *name, PyObject *valObject)
{
    WindowObject *self = (WindowObject*)self_obj;
    اگر (CheckWindow(self))
	بازگشت -1;

    اگر (strcmp(name, "buffer") == 0)
    {
	PyErr_SET_STRING(PyExc_TypeError, N_("readonly attribute: buffer"));
	بازگشت -1;
    }
    وگرنه اگر (strcmp(name, "cursor") == 0)
    {
	long lnum;
	long col;

	اگر (!PyArg_Parse(valObject, "(ll)", &lnum, &col))
	    بازگشت -1;

	اگر (lnum <= 0 || lnum > self->win->w_buffer->b_ml.ml_سطر_شمار)
	{
	    PyErr_SET_VIM(N_("cursor position outside buffer"));
	    بازگشت -1;
	}

	// Check for keyboard interrupts
	اگر (VimCheckInterrupt())
	    بازگشت -1;

	self->win->w_cursor.lnum = lnum;
	self->win->w_cursor.col = col;
	self->win->w_set_curswant = TRUE;
	self->win->w_cursor.coladd = 0;
	// When column is out of range silently correct it.
	check_cursor_col_win(self->win);

	update_screen(UPD_VALID);
	بازگشت 0;
    }
    وگرنه اگر (strcmp(name, "height") == 0)
    {
	long	height;
	win_T	*savewin;

	اگر (عدد_To_Long(valObject, &height, عدد_INT|عدد_UNSIGNED))
	    بازگشت -1;

#ifdef FEAT_GUI
	need_mouse_correct = TRUE;
#endif
	savewin = curwin;
	curwin = self->win;
	curbuf = curwin->w_buffer;

	VimTryStart();
	win_setheight((صحیح) height);
	curwin = savewin;
	curbuf = curwin->w_buffer;
	اگر (VimTryEnd())
	    بازگشت -1;

	بازگشت 0;
    }
    وگرنه اگر (strcmp(name, "width") == 0)
    {
	long	width;
	win_T	*savewin;

	اگر (عدد_To_Long(valObject, &width, عدد_INT|عدد_UNSIGNED))
	    بازگشت -1;

#ifdef FEAT_GUI
	need_mouse_correct = TRUE;
#endif
	savewin = curwin;
	curwin = self->win;
	curbuf = curwin->w_buffer;

	VimTryStart();
	win_setwidth((صحیح) width);
	curwin = savewin;
	curbuf = curwin->w_buffer;
	اگر (VimTryEnd())
	    بازگشت -1;

	بازگشت 0;
    }
    وگرنه
    {
	PyErr_SetString(PyExc_AttributeError, name);
	بازگشت -1;
    }
}

    ایستا PyObject *
WindowRepr(PyObject *self_obj)
{
    WindowObject *self = (WindowObject*)self_obj;
    اگر (self->win == INVALID_WINDOW_مقدار)
	بازگشت PyString_FromFormat("<window object (deleted) at %p>", (تهی *)self);
    وگرنه
    {
	صحیح	w = get_win_عدد(self->win, firstwin);

	اگر (w == 0)
	    بازگشت PyString_FromFormat("<window object (unknown) at %p>",
								      (تهی *)self);
	وگرنه
	    بازگشت PyString_FromFormat("<window %d>", w - 1);
    }
}

ایستا ساختار PyMethodDef WindowMethods[] = {
    // name,	    function,			calling,	documentation
    {"__dir__",	    (PyCFunction)WindowDir,	METH_NOARGS,	""},
    { پوچ,	    پوچ,			0,		پوچ}
};

/*
 * Window list object
 */

DEFINE_PY_TYPE_OBJECT(WinListType);
ایستا PySequenceMethods WinListAsSeq;

تعریفنوع ساختار
{
    PyObject_HEAD
    TabPageObject	*tabObject;
} WinListObject;

ایستا WinListObject TheWindowList =
{
    PyObject_HEAD_INIT_TYPE(WinListType)
    پوچ
};

    ایستا PyObject *
WinListNew(TabPageObject *tabObject)
{
    WinListObject	*self;

    self = PyObject_NEW(WinListObject, WinListTypePtr);
    self->tabObject = tabObject;
    Py_INCREF((PyObject *)tabObject);

    بازگشت (PyObject *)(self);
}

    ایستا تهی
WinListDestructor(WinListObject *self)
{
    TabPageObject	*tabObject = self->tabObject;

    اگر (tabObject)
    {
	Py_DECREF((PyObject *)(tabObject));
    }

    DESTRUCTOR_FINISH(self);
}

    ایستا PyInt
WinListLength(WinListObject *self)
{
    win_T	*w;
    PyInt	n = 0;

    اگر (!(w = get_firstwin(self->tabObject)))
	بازگشت -1;

    وقتی (w != پوچ)
    {
	++n;
	w = W_NEXT(w);
    }

    بازگشت n;
}

    ایستا PyObject *
WinListItem(WinListObject *self, PyInt n)
{
    win_T *w;

    اگر (!(w = get_firstwin(self->tabObject)))
	بازگشت پوچ;

    برای (; w != پوچ; w = W_NEXT(w), --n)
	اگر (n == 0)
	    بازگشت WindowNew(w, self->tabObject? self->tabObject->tab: curtab);

    PyErr_SET_STRING(PyExc_IndexError, N_("no such window"));
    بازگشت پوچ;
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
    ایستا نویسه *
String_To_سطر(PyObject *obj)
{
    نویسه	*str;
    نویسه	*save;
    PyObject	*bytes = پوچ;
    Py_ssize_t	len = 0;
    PyInt	i;
    نویسه	*p;

    اگر (PyBytes_Check(obj))
    {
	اگر (PyBytes_AsStringAndSize(obj, &str, &len) == -1
		|| str == پوچ)
	    بازگشت پوچ;
    }
    وگرنه اگر (PyUnicode_Check(obj))
    {
	اگر (!(bytes = PyUnicode_AsEncodedString(obj, ENC_OPT,
							   ERRORS_ENCODE_ARG)))
	    بازگشت پوچ;

	اگر (PyBytes_AsStringAndSize(bytes, &str, &len) == -1
		|| str == پوچ)
	{
	    Py_DECREF(bytes);
	    بازگشت پوچ;
	}
    }
    وگرنه
    {
#اگر PY_MAJOR_VERSION < 3
	PyErr_FORMAT_TYPE(
		N_("expected str() or unicode() instance, but got %s"),
		obj);
#وگرنه
	PyErr_FORMAT_TYPE(
		N_("expected bytes() or str() instance, but got %s"),
		obj);
#endif
	بازگشت پوچ;
    }

    /*
     * Error checking: String must not contain newlines, as we
     * are replacing a single line, and we must replace it with
     * a single line.
     * A trailing newline is removed, so that append(f.readlines()) works.
     */
    p = memchr(str, '\n', len);
    اگر (p != پوچ)
    {
	اگر (p == str + len - 1)
	    --len;
	وگرنه
	{
	    PyErr_SET_VIM(N_("string cannot contain newlines"));
	    Py_XDECREF(bytes);
	    بازگشت پوچ;
	}
    }

    /*
     * Create a copy of the string, with internal nulls replaced by
     * newline characters, as is the Vim convention.
     */
    save = alloc(len+1);
    اگر (save == پوچ)
    {
	PyErr_NoMemory();
	Py_XDECREF(bytes);
	بازگشت پوچ;
    }

    برای (i = 0; i < len; ++i)
    {
	اگر (str[i] == '\0')
	    save[i] = '\n';
	وگرنه
	    save[i] = str[i];
    }

    save[i] = '\0';
    Py_XDECREF(bytes);  // Python 2 does nothing here

    بازگشت save;
}

/*
 * Get a line from the specified buffer. The line number is
 * in Vim format (1-based). The line is returned as a Python
 * string object.
 */
    ایستا PyObject *
Get_Buffer_سطر(buf_T *buf, PyInt n)
{
    بازگشت سطر_To_String((نویسه *)ml_get_buf(buf, (linenr_T)n, FALSE));
}


/*
 * Get a list of lines from the specified buffer. The line numbers
 * are in Vim format (1-based). The range is from lo up to, but not
 * including, hi. The list is returned as a Python list of string objects.
 */
    ایستا PyObject *
Get_Buffer_سطر_List(buf_T *buf, PyInt lo, PyInt hi)
{
    PyInt	i;
    PyInt	n = hi - lo;
    PyObject	*list = PyList_New(n);

    اگر (list == پوچ)
	بازگشت پوچ;

    برای (i = 0; i < n; ++i)
    {
	linenr_T	lnum = (linenr_T)(lo + i);
	نویسه		*text;
	PyObject	*string;

	اگر (lnum > buf->b_ml.ml_سطر_شمار)
	    text = "";
	وگرنه
	    text = (نویسه *)ml_get_buf(buf, lnum, FALSE);
	string = سطر_To_String(text);
	اگر (string == پوچ)
	{
	    Py_DECREF(list);
	    بازگشت پوچ;
	}

	PyList_SET_ITEM(list, i, string);
    }

    // The ownership of the Python list is passed to the caller (ie,
    // the caller should Py_DECREF() the object when it is finished
    // with it).

    بازگشت list;
}

/*
 * Check if deleting lines made the cursor position invalid.
 * Changed the lines from "lo" to "hi" and added "extra" lines (negative if
 * deleted).
 */
    ایستا تهی
py_fix_cursor(linenr_T lo, linenr_T hi, linenr_T extra)
{
    اگر (curwin->w_cursor.lnum >= lo)
    {
	// Adjust the cursor position if it's in/after the changed
	// lines.
	اگر (curwin->w_cursor.lnum >= hi)
	{
	    curwin->w_cursor.lnum += extra;
	    check_cursor_col();
	}
	وگرنه اگر (extra < 0)
	{
	    curwin->w_cursor.lnum = lo;
	    check_cursor();
	}
	وگرنه
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
    ایستا صحیح
Set_Buffer_سطر(buf_T *buf, PyInt n, PyObject *سطر, PyInt *len_change)
{
    bufref_T	save_curbuf = {پوچ, 0, 0};
    switchwin_T	switchwin;

    // First of all, we check the type of the supplied Python object.
    // There are three cases:
    //	  1. NULL, or None - this is a deletion.
    //	  2. A string	   - this is a replacement.
    //	  3. Anything else - this is an error.
    اگر (سطر == Py_None || سطر == پوچ)
    {
	PyErr_Clear();
	switchwin.sw_curwin = پوچ;
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	VimTryStart();

	اگر (u_savedel((linenr_T)n, 1L) == FAIL)
	    RAISE_UNDO_FAIL;
	وگرنه اگر (ml_delete((linenr_T)n) == FAIL)
	    RAISE_DELETE_سطر_FAIL;
	وگرنه
	{
	    اگر (buf == curbuf && (switchwin.sw_curwin != پوچ
					   || save_curbuf.br_buf == پوچ))
		// Using an existing window for the buffer, adjust the cursor
		// position.
		py_fix_cursor((linenr_T)n, (linenr_T)n + 1, (linenr_T)-1);
	    اگر (save_curbuf.br_buf == پوچ)
		// Only adjust marks if we managed to switch to a window that
		// holds the buffer, otherwise line numbers will be invalid.
		deleted_سطرها_mark((linenr_T)n, 1L);
	}

	restore_win_for_buf(&switchwin, &save_curbuf);

	اگر (VimTryEnd())
	    بازگشت FAIL;

	اگر (len_change)
	    *len_change = -1;

	بازگشت OK;
    }
    وگرنه اگر (PyBytes_Check(سطر) || PyUnicode_Check(سطر))
    {
	نویسه	*save = String_To_سطر(سطر);

	اگر (save == پوچ)
	    بازگشت FAIL;

	VimTryStart();

	// We do not need to free "save" if ml_replace() consumes it.
	PyErr_Clear();
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	اگر (u_savesub((linenr_T)n) == FAIL)
	{
	    RAISE_UNDO_FAIL;
	    vim_free(save);
	}
	وگرنه اگر (ml_replace((linenr_T)n, (char_u *)save, FALSE) == FAIL)
	{
	    RAISE_REPLACE_سطر_FAIL;
	    vim_free(save);
	}
	وگرنه
	    changed_bytes((linenr_T)n, 0);

	restore_win_for_buf(&switchwin, &save_curbuf);

	// Check that the cursor is not beyond the end of the line now.
	اگر (buf == curbuf)
	    check_cursor_col();

	اگر (VimTryEnd())
	    بازگشت FAIL;

	اگر (len_change)
	    *len_change = 0;

	بازگشت OK;
    }
    وگرنه
    {
	PyErr_BadArgument();
	بازگشت FAIL;
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
    ایستا صحیح
Set_Buffer_سطر_List(
	buf_T *buf,
	PyInt lo,
	PyInt hi,
	PyObject *list,
	PyInt *len_change)
{
    bufref_T	save_curbuf = {پوچ, 0, 0};
    switchwin_T	switchwin;

    // First of all, we check the type of the supplied Python object.
    // There are three cases:
    //	  1. NULL, or None - this is a deletion.
    //	  2. A list	   - this is a replacement.
    //	  3. Anything else - this is an error.
    اگر (list == Py_None || list == پوچ)
    {
	PyInt	i;
	PyInt	n = (صحیح)(hi - lo);

	PyErr_Clear();
	VimTryStart();
	switchwin.sw_curwin = پوچ;
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	اگر (u_savedel((linenr_T)lo, (long)n) == FAIL)
	    RAISE_UNDO_FAIL;
	وگرنه
	{
	    برای (i = 0; i < n; ++i)
	    {
		اگر (ml_delete((linenr_T)lo) == FAIL)
		{
		    RAISE_DELETE_سطر_FAIL;
		    بشکن;
		}
	    }
	    اگر (buf == curbuf && (switchwin.sw_curwin != پوچ
					       || save_curbuf.br_buf == پوچ))
		// Using an existing window for the buffer, adjust the cursor
		// position.
		py_fix_cursor((linenr_T)lo, (linenr_T)hi, (linenr_T)-n);
	    اگر (save_curbuf.br_buf == پوچ)
		// Only adjust marks if we managed to switch to a window that
		// holds the buffer, otherwise line numbers will be invalid.
		deleted_سطرها_mark((linenr_T)lo, (long)i);
	}

	restore_win_for_buf(&switchwin, &save_curbuf);

	اگر (VimTryEnd())
	    بازگشت FAIL;

	اگر (len_change)
	    *len_change = -n;

	بازگشت OK;
    }
    وگرنه اگر (PyList_Check(list))
    {
	PyInt	i;
	PyInt	new_len = PyList_Size(list);
	PyInt	old_len = hi - lo;
	PyInt	extra = 0;	// lines added to text, can be negative
	نویسه	**array;

	اگر (new_len == 0)	// avoid allocating zero bytes
	    array = پوچ;
	وگرنه
	{
	    array = PyMem_New(نویسه *, new_len);
	    اگر (array == پوچ)
	    {
		PyErr_NoMemory();
		بازگشت FAIL;
	    }
	}

	برای (i = 0; i < new_len; ++i)
	{
	    PyObject *سطر;

	    اگر (!(سطر = PyList_GetItem(list, i)) ||
		!(array[i] = String_To_سطر(سطر)))
	    {
		وقتی (i)
		    vim_free(array[--i]);
		PyMem_Free(array);
		بازگشت FAIL;
	    }
	}

	VimTryStart();
	PyErr_Clear();

	// START of region without "return".  Must call restore_buffer()!
	switchwin.sw_curwin = پوچ;
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	اگر (u_save((linenr_T)(lo-1), (linenr_T)hi) == FAIL)
	    RAISE_UNDO_FAIL;

	// If the size of the range is reducing (ie, new_len < old_len) we
	// need to delete some old_len. We do this at the start, by
	// repeatedly deleting line "lo".
	اگر (!PyErr_Occurred())
	{
	    برای (i = 0; i < old_len - new_len; ++i)
		اگر (ml_delete((linenr_T)lo) == FAIL)
		{
		    RAISE_DELETE_سطر_FAIL;
		    بشکن;
		}
	    extra -= i;
	}

	// For as long as possible, replace the existing old_len with the
	// new old_len. This is a more efficient operation, as it requires
	// less memory allocation and freeing.
	اگر (!PyErr_Occurred())
	{
	    برای (i = 0; i < old_len && i < new_len; ++i)
		اگر (ml_replace((linenr_T)(lo+i), (char_u *)array[i], FALSE)
								      == FAIL)
		{
		    RAISE_REPLACE_سطر_FAIL;
		    بشکن;
		}
	}
	وگرنه
	    i = 0;

	// Now we may need to insert the remaining new old_len. If we do, we
	// must free the strings as we finish with them (we can't pass the
	// responsibility to Vim in this case).
	اگر (!PyErr_Occurred())
	{
	    وقتی (i < new_len)
	    {
		اگر (ml_append((linenr_T)(lo + i - 1),
					(char_u *)array[i], 0, FALSE) == FAIL)
		{
		    RAISE_درج_سطر_FAIL;
		    بشکن;
		}
		vim_free(array[i]);
		++i;
		++extra;
	    }
	}

	// Free any left-over old_len, as a result of an error
	وقتی (i < new_len)
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
	اگر (save_curbuf.br_buf == پوچ)
	{
	    mark_adjust((linenr_T)lo, (linenr_T)(hi - 1),
						  (long)MAXLNUM, (long)extra);
	    changed_سطرها((linenr_T)lo, 0, (linenr_T)hi, (long)extra);
	}

	اگر (buf == curbuf && (switchwin.sw_curwin != پوچ
					   || save_curbuf.br_buf == پوچ))
	    // Using an existing window for the buffer, adjust the cursor
	    // position.
	    py_fix_cursor((linenr_T)lo, (linenr_T)hi, (linenr_T)extra);

	// END of region without "return".
	restore_win_for_buf(&switchwin, &save_curbuf);

	اگر (VimTryEnd())
	    بازگشت FAIL;

	اگر (len_change)
	    *len_change = new_len - old_len;

	بازگشت OK;
    }
    وگرنه
    {
	PyErr_BadArgument();
	بازگشت FAIL;
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
    ایستا صحیح
درج_Buffer_سطرها(buf_T *buf, PyInt n, PyObject *سطرها, PyInt *len_change)
{
    bufref_T	save_curbuf = {پوچ, 0, 0};
    switchwin_T	switchwin;

    // First of all, we check the type of the supplied Python object.
    // It must be a string or a list, or the call is in error.
    اگر (PyBytes_Check(سطرها) || PyUnicode_Check(سطرها))
    {
	نویسه		*str = String_To_سطر(سطرها);

	اگر (str == پوچ)
	    بازگشت FAIL;

	PyErr_Clear();
	VimTryStart();
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	اگر (u_save((linenr_T)n, (linenr_T)(n + 1)) == FAIL)
	    RAISE_UNDO_FAIL;
	وگرنه اگر (ml_append((linenr_T)n, (char_u *)str, 0, FALSE) == FAIL)
	    RAISE_درج_سطر_FAIL;
	وگرنه اگر (save_curbuf.br_buf == پوچ)
	    // Only adjust marks if we managed to switch to a window that
	    // holds the buffer, otherwise line numbers will be invalid.
	    appended_سطرها_mark((linenr_T)n, 1L);

	vim_free(str);
	restore_win_for_buf(&switchwin, &save_curbuf);
	update_screen(UPD_VALID);

	اگر (VimTryEnd())
	    بازگشت FAIL;

	اگر (len_change)
	    *len_change = 1;

	بازگشت OK;
    }
    وگرنه اگر (PyList_Check(سطرها))
    {
	PyInt	i;
	PyInt	size = PyList_Size(سطرها);
	نویسه	**array;

	array = PyMem_New(نویسه *, size);
	اگر (array == پوچ)
	{
	    PyErr_NoMemory();
	    بازگشت FAIL;
	}

	برای (i = 0; i < size; ++i)
	{
	    PyObject *سطر;

	    اگر (!(سطر = PyList_GetItem(سطرها, i)) ||
		!(array[i] = String_To_سطر(سطر)))
	    {
		وقتی (i)
		    vim_free(array[--i]);
		PyMem_Free(array);
		بازگشت FAIL;
	    }
	}

	PyErr_Clear();
	VimTryStart();
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	اگر (u_save((linenr_T)n, (linenr_T)(n + 1)) == FAIL)
	    RAISE_UNDO_FAIL;
	وگرنه
	{
	    برای (i = 0; i < size; ++i)
	    {
		اگر (ml_append((linenr_T)(n + i),
					(char_u *)array[i], 0, FALSE) == FAIL)
		{
		    RAISE_درج_سطر_FAIL;

		    // Free the rest of the lines
		    وقتی (i < size)
			vim_free(array[i++]);

		    بشکن;
		}
		vim_free(array[i]);
	    }
	    اگر (i > 0 && save_curbuf.br_buf == پوچ)
		// Only adjust marks if we managed to switch to a window that
		// holds the buffer, otherwise line numbers will be invalid.
		appended_سطرها_mark((linenr_T)n, (long)i);
	}

	// Free the array of lines. All of its contents have now
	// been freed.
	PyMem_Free(array);
	restore_win_for_buf(&switchwin, &save_curbuf);

	update_screen(UPD_VALID);

	اگر (VimTryEnd())
	    بازگشت FAIL;

	اگر (len_change)
	    *len_change = size;

	بازگشت OK;
    }
    وگرنه
    {
	PyErr_BadArgument();
	بازگشت FAIL;
    }
}

/*
 * Common routines for buffers and line ranges
 * -------------------------------------------
 */

تعریفنوع ساختار
{
    PyObject_HEAD
    buf_T *buf;
} BufferObject;

    ایستا صحیح
CheckBuffer(BufferObject *self)
{
    اگر (self->buf == INVALID_BUFFER_مقدار)
    {
	PyErr_SET_VIM(N_("attempt to refer to deleted buffer"));
	بازگشت -1;
    }

    بازگشت 0;
}

    ایستا صحیح
CheckBufferCb(تهی *self)
{
    بازگشت CheckBuffer((BufferObject*)self);
}

    ایستا PyObject *
RBItem(BufferObject *self, PyInt n, PyInt start, PyInt end)
{
    اگر (CheckBuffer(self))
	بازگشت پوچ;

    اگر (end == -1)
	end = self->buf->b_ml.ml_سطر_شمار;

    اگر (n < 0)
	n += end - start + 1;

    اگر (n < 0 || n > end - start)
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("line number out of range"));
	بازگشت پوچ;
    }

    بازگشت Get_Buffer_سطر(self->buf, n+start);
}

    ایستا PyObject *
RBSlice(BufferObject *self, PyInt lo, PyInt hi, PyInt start, PyInt end)
{
    PyInt size;

    اگر (CheckBuffer(self))
	بازگشت پوچ;

    اگر (end == -1)
	end = self->buf->b_ml.ml_سطر_شمار;

    size = end - start + 1;

    اگر (lo < 0)
	lo = 0;
    وگرنه اگر (lo > size)
	lo = size;
    اگر (hi < 0)
	hi = 0;
    اگر (hi < lo)
	hi = lo;
    وگرنه اگر (hi > size)
	hi = size;

    بازگشت Get_Buffer_سطر_List(self->buf, lo+start, hi+start);
}

    ایستا PyInt
RBAsItem(
	BufferObject *self,
	PyInt n,
	PyObject *valObject,
	PyInt start,
	PyInt end,
	PyInt *new_end)
{
    PyInt len_change;

    اگر (CheckBuffer(self))
	بازگشت -1;

    اگر (end == -1)
	end = self->buf->b_ml.ml_سطر_شمار;

    اگر (n < 0)
	n += end - start + 1;

    اگر (n < 0 || n > end - start)
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("line number out of range"));
	بازگشت -1;
    }

    اگر (Set_Buffer_سطر(self->buf, n+start, valObject, &len_change) == FAIL)
	بازگشت -1;

    اگر (new_end)
	*new_end = end + len_change;

    بازگشت 0;
}

    ایستا PyInt
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
    اگر (CheckBuffer(self))
	بازگشت -1;

    اگر (end == -1)
	end = self->buf->b_ml.ml_سطر_شمار;

    // Sort out the slice range
    size = end - start + 1;

    اگر (lo < 0)
	lo = 0;
    وگرنه اگر (lo > size)
	lo = size;
    اگر (hi < 0)
	hi = 0;
    اگر (hi < lo)
	hi = lo;
    وگرنه اگر (hi > size)
	hi = size;

    اگر (Set_Buffer_سطر_List(self->buf, lo + start, hi + start,
						valObject, &len_change) == FAIL)
	بازگشت -1;

    اگر (new_end)
	*new_end = end + len_change;

    بازگشت 0;
}


    ایستا PyObject *
RBAppend(
	BufferObject *self,
	PyObject *args,
	PyInt start,
	PyInt end,
	PyInt *new_end)
{
    PyObject *سطرها;
    PyInt len_change;
    PyInt max;
    PyInt n;

    اگر (CheckBuffer(self))
	بازگشت پوچ;

    اگر (end == -1)
	end = self->buf->b_ml.ml_سطر_شمار;

    max = n = end - start + 1;

    اگر (!PyArg_ParseTuple(args, "O|n", &سطرها, &n))
	بازگشت پوچ;

    اگر (n < 0 || n > max)
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("line number out of range"));
	بازگشت پوچ;
    }

    اگر (درج_Buffer_سطرها(self->buf, n + start - 1, سطرها, &len_change) == FAIL)
	بازگشت پوچ;

    اگر (new_end)
	*new_end = end + len_change;

    Py_INCREF(Py_None);
    بازگشت Py_None;
}

// Range object

DEFINE_PY_TYPE_OBJECT(RangeType);
ایستا PySequenceMethods RangeAsSeq;
ایستا PyMappingMethods RangeAsMapping;

تعریفنوع ساختار
{
    PyObject_HEAD
    BufferObject *buf;
    PyInt start;
    PyInt end;
} RangeObject;

    ایستا PyObject *
RangeNew(buf_T *buf, PyInt start, PyInt end)
{
    BufferObject *bufr;
    RangeObject *self;
    self = PyObject_GC_New(RangeObject, RangeTypePtr);
    اگر (self == پوچ)
	بازگشت پوچ;

    bufr = (BufferObject *)BufferNew(buf);
    اگر (bufr == پوچ)
    {
	Py_DECREF(self);
	بازگشت پوچ;
    }
    Py_INCREF((PyObject *)bufr);

    self->buf = bufr;
    self->start = start;
    self->end = end;

    بازگشت (PyObject *)(self);
}

    ایستا تهی
RangeDestructor(PyObject *self_obj)
{
    RangeObject *self = (RangeObject*)self_obj;
    PyObject_GC_UnTrack((تهی *)(self));
    Py_XDECREF(((PyObject *)(self->buf)));
    PyObject_GC_Del((تهی *)(self));
}

    ایستا صحیح
RangeTraverse(PyObject *self_obj, visitproc visit, تهی *arg)
{
    RangeObject *self = (RangeObject*)self_obj;
    Py_VISIT(((PyObject *)(self->buf)));
    بازگشت 0;
}

    ایستا صحیح
RangeClear(PyObject *self_obj)
{
    RangeObject *self = (RangeObject*)self_obj;
    Py_CLEAR(self->buf);
    بازگشت 0;
}

    ایستا PyInt
RangeLength(RangeObject *self)
{
    // HOW DO WE SIGNAL AN ERROR FROM THIS FUNCTION?
    اگر (CheckBuffer(self->buf))
	بازگشت -1; // ???

    بازگشت (self->end - self->start + 1);
}

    ایستا PyObject *
RangeItem(RangeObject *self, PyInt n)
{
    بازگشت RBItem(self->buf, n, self->start, self->end);
}

    ایستا PyObject *
RangeSlice(RangeObject *self, PyInt lo, PyInt hi)
{
    بازگشت RBSlice(self->buf, lo, hi, self->start, self->end);
}

ایستا نویسه *RangeAttrs[] = {
    "start", "end",
    پوچ
};

    ایستا PyObject *
RangeDir(PyObject *self, PyObject *args UNUSED)
{
    بازگشت ObjectDir(self, RangeAttrs);
}

    ایستا PyObject *
RangeAppend(RangeObject *self, PyObject *args)
{
    بازگشت RBAppend(self->buf, args, self->start, self->end, &self->end);
}

    ایستا PyObject *
RangeRepr(PyObject *self_obj)
{
    RangeObject *self = (RangeObject*)self_obj;
    اگر (self->buf->buf == INVALID_BUFFER_مقدار)
	بازگشت PyString_FromFormat("<range object (for deleted buffer) at %p>",
				    (تهی *)self);
    وگرنه
    {
	نویسه *name = (نویسه *)self->buf->buf->b_fname;

	اگر (name == پوچ)
	    name = "";

	بازگشت PyString_FromFormat("<range %s (%d:%d)>",
				    name, (صحیح)self->start, (صحیح)self->end);
    }
}

ایستا ساختار PyMethodDef RangeMethods[] = {
    // name,	function,			calling,	documentation
    {"append",	(PyCFunction)RangeAppend,	METH_VARARGS,	"Append data to the Vim range" },
    {"__dir__",	(PyCFunction)RangeDir,		METH_NOARGS,	""},
    { پوچ,	پوچ,				0,		پوچ}
};

DEFINE_PY_TYPE_OBJECT(BufferType);
ایستا PySequenceMethods BufferAsSeq;
ایستا PyMappingMethods BufferAsMapping;

    ایستا PyObject *
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

    اگر (BUF_PYTHON_REF(buf) != پوچ)
    {
	self = BUF_PYTHON_REF(buf);
	Py_INCREF((PyObject *)self);
    }
    وگرنه
    {
	self = PyObject_NEW(BufferObject, BufferTypePtr);
	اگر (self == پوچ)
	    بازگشت پوچ;
	self->buf = buf;
	BUF_PYTHON_REF(buf) = self;
    }

    بازگشت (PyObject *)(self);
}

    ایستا تهی
BufferDestructor(PyObject *self_obj)
{
    BufferObject *self = (BufferObject*)self_obj;
    اگر (self->buf && self->buf != INVALID_BUFFER_مقدار)
	BUF_PYTHON_REF(self->buf) = پوچ;

    DESTRUCTOR_FINISH(self);
}

    ایستا PyInt
BufferLength(BufferObject *self)
{
    // HOW DO WE SIGNAL AN ERROR FROM THIS FUNCTION?
    اگر (CheckBuffer(self))
	بازگشت -1; // ???

    بازگشت (PyInt)(self->buf->b_ml.ml_سطر_شمار);
}

    ایستا PyObject *
BufferItem(BufferObject *self, PyInt n)
{
    بازگشت RBItem(self, n, 1, -1);
}

    ایستا PyObject *
BufferSlice(BufferObject *self, PyInt lo, PyInt hi)
{
    بازگشت RBSlice(self, lo, hi, 1, -1);
}

ایستا نویسه *BufferAttrs[] = {
    "name", "number", "vars", "options", "valid",
    پوچ
};

    ایستا PyObject *
BufferDir(PyObject *self, PyObject *args UNUSED)
{
    بازگشت ObjectDir(self, BufferAttrs);
}

    ایستا PyObject *
BufferAttrValid(BufferObject *self, نویسه *name)
{
    PyObject	*ret;

    اگر (strcmp(name, "valid") != 0)
	بازگشت پوچ;

    ret = ((self->buf == INVALID_BUFFER_مقدار) ? Py_False : Py_True);
    Py_INCREF(ret);
    بازگشت ret;
}

    ایستا PyObject *
BufferAttr(BufferObject *self, نویسه *name)
{
    اگر (strcmp(name, "name") == 0)
	بازگشت PyString_FromString((self->buf->b_ffname == پوچ
				    ? "" : (نویسه *)self->buf->b_ffname));
    وگرنه اگر (strcmp(name, "number") == 0)
	بازگشت Py_Build_مقدار(Py_ssize_t_fmt, self->buf->b_fnum);
    وگرنه اگر (strcmp(name, "vars") == 0)
	بازگشت NEW_DICTIONARY(self->buf->b_vars);
    وگرنه اگر (strcmp(name, "options") == 0)
	بازگشت OptionsNew(SREQ_BUF, self->buf, CheckBufferCb,
			(PyObject *) self);
    وگرنه اگر (strcmp(name, "__members__") == 0)
	بازگشت ObjectDir(پوچ, BufferAttrs);
    وگرنه
	بازگشت پوچ;
}

    ایستا صحیح
BufferSetattr(PyObject *self_obj, نویسه *name, PyObject *valObject)
{
    BufferObject *self = (BufferObject*)self_obj;
    اگر (CheckBuffer(self))
	بازگشت -1;

    اگر (strcmp(name, "name") == 0)
    {
	char_u		*val;
	aco_save_T	aco;
	صحیح		ren_ret = OK;
	PyObject	*todecref;

	اگر (!(val = StringToChars(valObject, &todecref)))
	    بازگشت -1;

	VimTryStart();
	// Using aucmd_*: autocommands will be executed by rename_buffer
	aucmd_prepbuf(&aco, self->buf);
	اگر (curbuf == self->buf)
	{
	    ren_ret = rename_buffer(val);
	    aucmd_restbuf(&aco);
	}
	Py_XDECREF(todecref);
	اگر (VimTryEnd())
	    بازگشت -1;

	اگر (ren_ret == FAIL)
	{
	    PyErr_SET_VIM(N_("failed to rename buffer"));
	    بازگشت -1;
	}
	بازگشت 0;
    }
    وگرنه
    {
	PyErr_SetString(PyExc_AttributeError, name);
	بازگشت -1;
    }
}

    ایستا PyObject *
BufferAppend(BufferObject *self, PyObject *args)
{
    بازگشت RBAppend(self, args, 1, -1, پوچ);
}

    ایستا PyObject *
BufferMark(BufferObject *self, PyObject *pmarkObject)
{
    pos_T	*posp;
    char_u	*pmark;
    char_u	mark;
    bufref_T	savebuf;
    PyObject	*todecref;

    اگر (CheckBuffer(self))
	بازگشت پوچ;

    اگر (!(pmark = StringToChars(pmarkObject, &todecref)))
	بازگشت پوچ;

    اگر (pmark[0] == '\0' || pmark[1] != '\0')
    {
	PyErr_SET_STRING(Py_Exc_مقدار_Error,
		N_("mark name must be a single character"));
	Py_XDECREF(todecref);
	بازگشت پوچ;
    }

    mark = *pmark;

    Py_XDECREF(todecref);

    VimTryStart();
    switch_buffer(&savebuf, self->buf);
    posp = getmark(mark, FALSE);
    restore_buffer(&savebuf);
    اگر (VimTryEnd())
	بازگشت پوچ;

    اگر (posp == پوچ)
    {
	PyErr_SET_VIM(N_("invalid mark name"));
	بازگشت پوچ;
    }

    اگر (posp->lnum <= 0)
    {
	// Or raise an error?
	Py_INCREF(Py_None);
	بازگشت Py_None;
    }

    بازگشت Py_Build_مقدار("(ll)", (long)(posp->lnum), (long)(posp->col));
}

    ایستا PyObject *
BufferRange(BufferObject *self, PyObject *args)
{
    PyInt start;
    PyInt end;

    اگر (CheckBuffer(self))
	بازگشت پوچ;

    اگر (!PyArg_ParseTuple(args, "nn", &start, &end))
	بازگشت پوچ;

    بازگشت RangeNew(self->buf, start, end);
}

    ایستا PyObject *
BufferRepr(PyObject *self_obj)
{
    BufferObject *self = (BufferObject*)self_obj;
    اگر (self->buf == INVALID_BUFFER_مقدار)
	بازگشت PyString_FromFormat("<buffer object (deleted) at %p>", (تهی *)self);
    وگرنه
    {
	نویسه	*name = (نویسه *)self->buf->b_fname;

	اگر (name == پوچ)
	    name = "";

	بازگشت PyString_FromFormat("<buffer %s>", name);
    }
}

ایستا ساختار PyMethodDef BufferMethods[] = {
    // name,	    function,			calling,	documentation
    {"append",	    (PyCFunction)BufferAppend,	METH_VARARGS,	"Append data to Vim buffer" },
    {"mark",	    (PyCFunction)BufferMark,	METH_O,		"Return (row,col) representing position of named mark" },
    {"range",	    (PyCFunction)BufferRange,	METH_VARARGS,	"Return a range object which represents the part of the given buffer between line numbers s and e" },
    {"__dir__",	    (PyCFunction)BufferDir,	METH_NOARGS,	""},
    { پوچ,	    پوچ,			0,		پوچ}
};

/*
 * Buffer list object - Implementation
 */

DEFINE_PY_TYPE_OBJECT(BufMapType);

تعریفنوع ساختار
{
    PyObject_HEAD
} BufMapObject;

ایستا BufMapObject TheBufferMap =
{
    PyObject_HEAD_INIT_TYPE(BufMapType)
};

    ایستا PyInt
BufMapLength(PyObject *self UNUSED)
{
    buf_T	*b = firstbuf;
    PyInt	n = 0;

    وقتی (b)
    {
	++n;
	b = b->b_next;
    }

    بازگشت n;
}

    ایستا PyObject *
BufMapItem(PyObject *self UNUSED, PyObject *کلید_Object)
{
    buf_T	*b;
    long	bnr;

    اگر (عدد_To_Long(کلید_Object, &bnr, عدد_INT|عدد_NATURAL))
	بازگشت پوچ;

    b = buflist_findnr((صحیح) bnr);

    اگر (b)
	بازگشت BufferNew(b);
    وگرنه
    {
	PyErr_SetObject(Py_Exc_کلید_Error, کلید_Object);
	بازگشت پوچ;
    }
}

    ایستا تهی
BufMapIterDestruct(تهی* arg)
{
    PyObject *buffer = (PyObject*)arg;
    // Iteration was stopped before all buffers were processed
    اگر (buffer)
    {
	Py_DECREF(buffer);
    }
}

    ایستا صحیح
BufMapIterTraverse(تهی *iter, visitproc visit, تهی *arg)
{
    PyObject *buffer = (PyObject*)iter;
    اگر (buffer)
	Py_VISIT(buffer);
    بازگشت 0;
}

    ایستا صحیح
BufMapIterClear(تهی **iter)
{
    PyObject **buffer = (PyObject**)iter;
    اگر (*buffer)
	Py_CLEAR(*buffer);
    بازگشت 0;
}

    ایستا PyObject *
BufMapIterNext(تهی **arg)
{
    PyObject	*next;
    PyObject	*ret;
    PyObject	**buffer = (PyObject**)arg;

    اگر (!*buffer)
	بازگشت پوچ;

    ret = *buffer;

    اگر (CheckBuffer((BufferObject *)(ret)))
    {
	*buffer = پوچ;
	بازگشت پوچ;
    }

    اگر (!((BufferObject *)(ret))->buf->b_next)
	next = پوچ;
    وگرنه اگر (!(next = BufferNew(((BufferObject *)(ret))->buf->b_next)))
	بازگشت پوچ;
    *buffer = next;
    // Do not increment reference: we no longer hold it (decref), but whoever
    // on other side will hold (incref). Decref+incref = nothing.
    بازگشت ret;
}

    ایستا PyObject *
BufMapIter(PyObject *self)
{
    PyObject *buffer;

    buffer = BufferNew(firstbuf);
    بازگشت IterNew(buffer,
	    BufMapIterDestruct, BufMapIterNext,
	    BufMapIterTraverse, BufMapIterClear,
	    self);
}

ایستا PyMappingMethods BufMapAsMapping = {
    (lenfunc)       BufMapLength,
    (binaryfunc)    BufMapItem,
    (objobjargproc) 0,
};

// Current items object

ایستا نویسه *CurrentAttrs[] = {
    "buffer", "window", "line", "range", "tabpage",
    پوچ
};

    ایستا PyObject *
CurrentDir(PyObject *self, PyObject *args UNUSED)
{
    بازگشت ObjectDir(self, CurrentAttrs);
}

    ایستا PyObject *
CurrentGetattr(PyObject *self UNUSED, نویسه *name)
{
    اگر (strcmp(name, "buffer") == 0)
	بازگشت (PyObject *)BufferNew(curbuf);
    وگرنه اگر (strcmp(name, "window") == 0)
	بازگشت (PyObject *)WindowNew(curwin, curtab);
    وگرنه اگر (strcmp(name, "tabpage") == 0)
	بازگشت (PyObject *)TabPageNew(curtab);
    وگرنه اگر (strcmp(name, "line") == 0)
	بازگشت Get_Buffer_سطر(curbuf, (PyInt)curwin->w_cursor.lnum);
    وگرنه اگر (strcmp(name, "range") == 0)
	بازگشت RangeNew(curbuf, RangeStart, RangeEnd);
    وگرنه اگر (strcmp(name, "__members__") == 0)
	بازگشت ObjectDir(پوچ, CurrentAttrs);
    وگرنه
#اگر PY_MAJOR_VERSION < 3
	بازگشت Py_FindMethod(WindowMethods, self, name);
#وگرنه
	بازگشت پوچ;
#endif
}

    ایستا صحیح
CurrentSetattr(PyObject *self UNUSED, نویسه *name, PyObject *valObject)
{
    اگر (strcmp(name, "line") == 0)
    {
	اگر (Set_Buffer_سطر(curbuf, (PyInt)curwin->w_cursor.lnum, valObject,
			  پوچ) == FAIL)
	    بازگشت -1;

	بازگشت 0;
    }
    وگرنه اگر (strcmp(name, "buffer") == 0)
    {
	صحیح شمار;

	اگر (valObject->ob_type != BufferTypePtr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.Buffer object, but got %s"),
		    valObject);
	    بازگشت -1;
	}

	اگر (CheckBuffer((BufferObject *)(valObject)))
	    بازگشت -1;
	شمار = ((BufferObject *)(valObject))->buf->b_fnum;

	VimTryStart();
	اگر (do_buffer(DOBUF_GOTO, DOBUF_FIRST, FORWARD, شمار, 0) == FAIL)
	{
	    اگر (VimTryEnd())
		بازگشت -1;
	    PyErr_VIM_FORMAT(N_("failed to switch to buffer %d"), شمار);
	    بازگشت -1;
	}

	بازگشت VimTryEnd();
    }
    وگرنه اگر (strcmp(name, "window") == 0)
    {
	صحیح شمار;

	اگر (valObject->ob_type != WindowTypePtr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.Window object, but got %s"),
		    valObject);
	    بازگشت -1;
	}

	اگر (CheckWindow((WindowObject *)(valObject)))
	    بازگشت -1;
	شمار = get_win_عدد(((WindowObject *)(valObject))->win, firstwin);

	اگر (!شمار)
	{
	    PyErr_SET_STRING(Py_Exc_مقدار_Error,
		    N_("failed to find window in the current tab page"));
	    بازگشت -1;
	}

	VimTryStart();
	win_goto(((WindowObject *)(valObject))->win);
	اگر (((WindowObject *)(valObject))->win != curwin)
	{
	    اگر (VimTryEnd())
		بازگشت -1;
	    PyErr_SET_STRING(PyExc_RuntimeError,
		    N_("did not switch to the specified window"));
	    بازگشت -1;
	}

	بازگشت VimTryEnd();
    }
    وگرنه اگر (strcmp(name, "tabpage") == 0)
    {
	اگر (valObject->ob_type != TabPageTypePtr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.TabPage object, but got %s"),
		    valObject);
	    بازگشت -1;
	}

	اگر (CheckTabPage((TabPageObject *)(valObject)))
	    بازگشت -1;

	VimTryStart();
	goto_tabpage_tp(((TabPageObject *)(valObject))->tab, TRUE, TRUE);
	اگر (((TabPageObject *)(valObject))->tab != curtab)
	{
	    اگر (VimTryEnd())
		بازگشت -1;
	    PyErr_SET_STRING(PyExc_RuntimeError,
		    N_("did not switch to the specified tab page"));
	    بازگشت -1;
	}

	بازگشت VimTryEnd();
    }
    وگرنه
    {
	PyErr_SetString(PyExc_AttributeError, name);
	بازگشت -1;
    }
}

ایستا ساختار PyMethodDef CurrentMethods[] = {
    // name,	    function,			calling,	documentation
    {"__dir__",	    (PyCFunction)CurrentDir,	METH_NOARGS,	""},
    { پوچ,	    پوچ,			0,		پوچ}
};

    ایستا تهی
init_range_cmd(تهی *arg)
{
    exarg_T *eap = (exarg_T*)arg;
    RangeStart = eap->line1;
    RangeEnd = eap->line2;
}

    ایستا تهی
init_range_eval(تهی *rettv UNUSED)
{
    RangeStart = (PyInt) curwin->w_cursor.lnum;
    RangeEnd = RangeStart;
}

    ایستا تهی
run_cmd(ثابت نویسه *cmd, dict_T* locals UNUSED, تهی *arg UNUSED
#ifdef PY_CAN_RECURSE
	, PyGILState_STATE *pygilstate UNUSED
#endif
	)
{
    PyObject	*run_ret;
    run_ret = PyRun_String((نویسه *)cmd, Py_file_درون_داد, globals, globals);
    اگر (run_ret != پوچ)
    {
	Py_DECREF(run_ret);
    }
    وگرنه اگر (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
    {
	emsg(_(e_cant_handle_systemexit_of_python_exception_in_vim));
	PyErr_Clear();
    }
    وگرنه
	PyErr_PrintEx(1);
}

ایستا ثابت نویسه	*code_hdr = "def " DOPY_FUNC "(line, linenr):\n ";
ایستا صحیح		code_hdr_len = 30;

    ایستا تهی
run_do(ثابت نویسه *cmd, dict_T* locals UNUSED, تهی *arg UNUSED
#ifdef PY_CAN_RECURSE
	, PyGILState_STATE *pygilstate
#endif
	)
{
    PyInt	lnum;
    size_t	len;
    نویسه	*code;
    صحیح		status;
    PyObject	*pyfunc, *pymain;
    PyObject	*run_ret;
    buf_T	*was_curbuf = curbuf;

    اگر (u_save((linenr_T)RangeStart - 1, (linenr_T)RangeEnd + 1) != OK)
    {
	emsg(_("cannot save undo information"));
	بازگشت;
    }

    len = code_hdr_len + STRLEN(cmd);
    code = PyMem_New(نویسه, len + 1);
    memcpy(code, code_hdr, code_hdr_len);
    STRCPY(code + code_hdr_len, cmd);
    run_ret = PyRun_String(code, Py_file_درون_داد, globals, globals);
    status = -1;
    اگر (run_ret != پوچ)
    {
	status = 0;
	Py_DECREF(run_ret);
    }
    وگرنه اگر (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
    {
	PyMem_Free(code);
	emsg(_(e_cant_handle_systemexit_of_python_exception_in_vim));
	PyErr_Clear();
	بازگشت;
    }
    وگرنه
	PyErr_PrintEx(1);

    PyMem_Free(code);

    اگر (status)
    {
	emsg(_("failed to run the code"));
	بازگشت;
    }

    status = 0;
    pymain = PyImport_AddModule("__main__");
    pyfunc = PyObject_GetAttrString(pymain, DOPY_FUNC);
#ifdef PY_CAN_RECURSE
    PyGILState_Release(*pygilstate);
#endif

    برای (lnum = RangeStart; lnum <= RangeEnd; ++lnum)
    {
	PyObject	*سطر;
	PyObject	*linenr;
	PyObject	*ret;

#ifdef PY_CAN_RECURSE
	*pygilstate = PyGILState_Ensure();
#endif
	// Check the line number, the command my have deleted lines.
	اگر (lnum > curbuf->b_ml.ml_سطر_شمار
		|| !(سطر = Get_Buffer_سطر(curbuf, lnum)))
	    برو err;
	اگر (!(linenr = PyInt_FromLong((long) lnum)))
	{
	    Py_DECREF(سطر);
	    برو err;
	}
	ret = PyObject_CallFunctionObjArgs(pyfunc, سطر, linenr, پوچ);
	Py_DECREF(سطر);
	Py_DECREF(linenr);
	اگر (!ret)
	    برو err;

	// Check that the command didn't switch to another buffer.
	// Check the line number, the command my have deleted lines.
	اگر (curbuf != was_curbuf || lnum > curbuf->b_ml.ml_سطر_شمار)
	{
	    Py_XDECREF(ret);
	    برو err;
	}

	اگر (ret != Py_None)
	    اگر (Set_Buffer_سطر(curbuf, lnum, ret, پوچ) == FAIL)
	    {
		Py_XDECREF(ret);
		برو err;
	    }

	Py_XDECREF(ret);
	PythonIO_Flush();
#ifdef PY_CAN_RECURSE
	PyGILState_Release(*pygilstate);
#endif
    }
    برو out;
err:
#ifdef PY_CAN_RECURSE
    *pygilstate = PyGILState_Ensure();
#endif
    PyErr_PrintEx(0);
    PythonIO_Flush();
    status = 1;
out:
#ifdef PY_CAN_RECURSE
    اگر (!status)
	*pygilstate = PyGILState_Ensure();
#endif
    Py_DECREF(pyfunc);
    PyObject_SetAttrString(pymain, DOPY_FUNC, پوچ);
    اگر (status)
	بازگشت;
    check_cursor();
    update_curbuf(UPD_NOT_VALID);
}

    ایستا تهی
run_eval(ثابت نویسه *cmd, dict_T *locals, تهی *arg
#ifdef PY_CAN_RECURSE
	, PyGILState_STATE *pygilstate UNUSED
#endif
	)
{
    PyObject	*run_ret;
    typval_T	*rettv = (typval_T*)arg;
    PyObject	*pylocals = locals ? NEW_DICTIONARY(locals) : globals;

    run_ret = PyRun_String((نویسه *)cmd, Py_eval_درون_داد, globals, pylocals);
    اگر (run_ret == پوچ)
    {
	اگر (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
	{
	    emsg(_(e_cant_handle_systemexit_of_python_exception_in_vim));
	    PyErr_Clear();
	}
	وگرنه
	{
	    اگر (PyErr_Occurred() && !پیام_silent)
		PyErr_PrintEx(0);
	    emsg(_(e_eval_did_not_return_valid_python_object));
	}
    }
    وگرنه
    {
	اگر (ConvertFromPyObject(run_ret, rettv) == -1)
	    emsg(_(e_failed_to_convert_returned_python_object_to_vim_مقدار));
	Py_DECREF(run_ret);
    }
    PyErr_Clear();
}

    ایستا صحیح
set_ref_in_py(ثابت صحیح copyID)
{
    pylinkedlist_T	*cur;
    list_T		*ll;
    tuple_T		*tt;
    صحیح			i;
    صحیح			abort = FALSE;
    FunctionObject	*func;

    اگر (lastdict != پوچ)
    {
	برای (cur = lastdict ; !abort && cur != پوچ ; cur = cur->pll_prev)
	    abort = set_ref_in_dict(((DictionaryObject *)(cur->pll_obj))->dict,
								       copyID);
    }

    اگر (lastlist != پوچ)
    {
	برای (cur = lastlist ; !abort && cur != پوچ ; cur = cur->pll_prev)
	{
	    ll = ((ListObject *) (cur->pll_obj))->list;
	    abort = set_ref_in_list(ll, copyID);
	}
    }

    اگر (lasttuple != پوچ)
    {
	برای (cur = lasttuple ; !abort && cur != پوچ ; cur = cur->pll_prev)
	{
	    tt = ((TupleObject *) (cur->pll_obj))->tuple;
	    abort = set_ref_in_tuple(tt, copyID);
	}
    }

    اگر (lastfunc != پوچ)
    {
	برای (cur = lastfunc ; !abort && cur != پوچ ; cur = cur->pll_prev)
	{
	    func = (FunctionObject *) cur->pll_obj;
	    abort = set_ref_in_dict(func->self, copyID);
	    اگر (func->argc)
		برای (i = 0; !abort && i < func->argc; ++i)
		    abort = abort
			|| set_ref_in_item(&func->argv[i], copyID, پوچ, پوچ, پوچ);
	}
    }

    بازگشت abort;
}

    ایستا صحیح
set_string_copy(char_u *str, typval_T *tv)
{
    tv->vval.v_string = vim_strsave(str);
    اگر (tv->vval.v_string == پوچ)
    {
	PyErr_NoMemory();
	بازگشت -1;
    }
    بازگشت 0;
}

    ایستا صحیح
pydict_to_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    dict_T	*dict;
    char_u	*کلید;
    dictitem_T	*di;
    PyObject	*کلید_Object;
    PyObject	*valObject;
    Py_ssize_t	iter = 0;

    اگر (!(dict = py_dict_alloc()))
	بازگشت -1;

    tv->v_type = VAR_DICT;
    tv->vval.v_dict = dict;

    وقتی (PyDict_Next(obj, &iter, &کلید_Object, &valObject))
    {
	PyObject	*todecref = پوچ;

	اگر (کلید_Object == پوچ || valObject == پوچ)
	{
	    dict_unref(dict);
	    بازگشت -1;
	}

	اگر (!(کلید = StringToChars(کلید_Object, &todecref)))
	{
	    dict_unref(dict);
	    بازگشت -1;
	}

	اگر (*کلید == NUL)
	{
	    dict_unref(dict);
	    Py_XDECREF(todecref);
	    RAISE_NO_EMPTY_کلیدها;
	    بازگشت -1;
	}

	di = dictitem_alloc(کلید);

	Py_XDECREF(todecref);

	اگر (di == پوچ)
	{
	    PyErr_NoMemory();
	    dict_unref(dict);
	    بازگشت -1;
	}

	اگر (_ConvertFromPyObject(valObject, &di->di_tv, lookup_dict) == -1)
	{
	    vim_free(di);
	    dict_unref(dict);
	    بازگشت -1;
	}

	اگر (dict_add(dict, di) == FAIL)
	{
	    RAISE_کلید_ADD_FAIL(di->di_کلید);
	    clear_tv(&di->di_tv);
	    vim_free(di);
	    dict_unref(dict);
	    بازگشت -1;
	}
    }

    --dict->dv_refcount;
    بازگشت 0;
}

    ایستا صحیح
pymap_to_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    dict_T	*dict;
    char_u	*کلید;
    dictitem_T	*di;
    PyObject	*list;
    PyObject	*iterator;
    PyObject	*کلید_Object;
    PyObject	*valObject;

    اگر (!(dict = py_dict_alloc()))
	بازگشت -1;

    tv->v_type = VAR_DICT;
    tv->vval.v_dict = dict;

    اگر (!(list = Py_Mapping_کلیدها(obj)))
    {
	dict_unref(dict);
	بازگشت -1;
    }

    اگر (!(iterator = PyObject_GetIter(list)))
    {
	dict_unref(dict);
	Py_DECREF(list);
	بازگشت -1;
    }
    Py_DECREF(list);

    وقتی ((کلید_Object = PyIter_Next(iterator)))
    {
	PyObject	*todecref;

	اگر (!(کلید = StringToChars(کلید_Object, &todecref)))
	{
	    Py_DECREF(کلید_Object);
	    Py_DECREF(iterator);
	    dict_unref(dict);
	    بازگشت -1;
	}

	اگر (*کلید == NUL)
	{
	    Py_DECREF(کلید_Object);
	    Py_DECREF(iterator);
	    Py_XDECREF(todecref);
	    dict_unref(dict);
	    RAISE_NO_EMPTY_کلیدها;
	    بازگشت -1;
	}

	اگر (!(valObject = PyObject_GetItem(obj, کلید_Object)))
	{
	    Py_DECREF(کلید_Object);
	    Py_DECREF(iterator);
	    Py_XDECREF(todecref);
	    dict_unref(dict);
	    بازگشت -1;
	}

	di = dictitem_alloc(کلید);

	Py_DECREF(کلید_Object);
	Py_XDECREF(todecref);

	اگر (di == پوچ)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(valObject);
	    dict_unref(dict);
	    PyErr_NoMemory();
	    بازگشت -1;
	}

	اگر (_ConvertFromPyObject(valObject, &di->di_tv, lookup_dict) == -1)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(valObject);
	    vim_free(di);
	    dict_unref(dict);
	    بازگشت -1;
	}

	Py_DECREF(valObject);

	اگر (dict_add(dict, di) == FAIL)
	{
	    RAISE_کلید_ADD_FAIL(di->di_کلید);
	    Py_DECREF(iterator);
	    dictitem_free(di);
	    dict_unref(dict);
	    بازگشت -1;
	}
    }
    Py_DECREF(iterator);
    --dict->dv_refcount;
    بازگشت 0;
}

    ایستا صحیح
pyseq_to_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    list_T	*l;

    اگر (!(l = py_list_alloc()))
	بازگشت -1;

    tv->v_type = VAR_LIST;
    tv->vval.v_list = l;

    اگر (list_py_concat(l, obj, lookup_dict) == -1)
    {
	list_unref(l);
	بازگشت -1;
    }

    --l->lv_refcount;
    بازگشت 0;
}

    ایستا صحیح
pytuple_to_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    tuple_T	*t;

    اگر (!(t = py_tuple_alloc()))
	بازگشت -1;

    tv->v_type = VAR_TUPLE;
    tv->vval.v_tuple = t;

    اگر (tuple_py_concat(t, obj, lookup_dict) == -1)
    {
	tuple_unref(t);
	بازگشت -1;
    }

    --t->tv_refcount;
    بازگشت 0;
}

تعریفنوع صحیح (*pytotvfunc)(PyObject *, typval_T *, PyObject *);

    ایستا صحیح
convert_dl(PyObject *obj, typval_T *tv,
				    pytotvfunc py_to_tv, PyObject *lookup_dict)
{
    PyObject	*capsule;
    نویسه	hexBuf[اندازه(تهی *) * 2 + 3];

    sprintf(hexBuf, "%p", (تهی *)obj);

#ifdef PY_USE_CAPSULE
    capsule = PyDict_GetItemString(lookup_dict, hexBuf);
#وگرنه
    capsule = (PyObject *)PyDict_GetItemString(lookup_dict, hexBuf);
#endif
    اگر (capsule == پوچ)
    {
#ifdef PY_USE_CAPSULE
	capsule = PyCapsule_New(tv, پوچ, پوچ);
#وگرنه
	capsule = PyCObject_FromVoidPtr(tv, پوچ);
#endif
	اگر (PyDict_SetItemString(lookup_dict, hexBuf, capsule))
	{
	    Py_DECREF(capsule);
	    tv->v_type = VAR_UNKNOWN;
	    بازگشت -1;
	}

	Py_DECREF(capsule);

	اگر (py_to_tv(obj, tv, lookup_dict) == -1)
	{
	    tv->v_type = VAR_UNKNOWN;
	    بازگشت -1;
	}
	// As we are not using copy_tv which increments reference count we must
	// do it ourself.
	اگر (tv->v_type == VAR_DICT)
	    ++tv->vval.v_dict->dv_refcount;
	وگرنه اگر (tv->v_type == VAR_LIST)
	    ++tv->vval.v_list->lv_refcount;
	وگرنه اگر (tv->v_type == VAR_TUPLE)
	    ++tv->vval.v_tuple->tv_refcount;
    }
    وگرنه
    {
	typval_T	*v;

#ifdef PY_USE_CAPSULE
	v = PyCapsule_GetPointer(capsule, پوچ);
#وگرنه
	v = PyCObject_AsVoidPtr(capsule);
#endif
	copy_tv(v, tv);
    }
    بازگشت 0;
}

    ایستا صحیح
ConvertFromPyMapping(PyObject *obj, typval_T *tv)
{
    PyObject	*lookup_dict;
    صحیح		ret;

    اگر (!(lookup_dict = PyDict_New()))
	بازگشت -1;

    اگر (PyType_IsSubtype(obj->ob_type, DictionaryTypePtr))
    {
	tv->v_type = VAR_DICT;
	tv->vval.v_dict = (((DictionaryObject *)(obj))->dict);
	++tv->vval.v_dict->dv_refcount;
	ret = 0;
    }
    وگرنه اگر (PyDict_Check(obj))
	ret = convert_dl(obj, tv, pydict_to_tv, lookup_dict);
    وگرنه اگر (PyMapping_Check(obj))
	ret = convert_dl(obj, tv, pymap_to_tv, lookup_dict);
    وگرنه
    {
	PyErr_FORMAT_TYPE(
		N_("unable to convert %s to a Vim dictionary"),
		obj);
	ret = -1;
    }
    Py_DECREF(lookup_dict);
    بازگشت ret;
}

    ایستا صحیح
ConvertFromPySequence(PyObject *obj, typval_T *tv)
{
    PyObject	*lookup_dict;
    صحیح		ret;

    اگر (!(lookup_dict = PyDict_New()))
	بازگشت -1;

    اگر (PyType_IsSubtype(obj->ob_type, ListTypePtr))
    {
	tv->v_type = VAR_LIST;
	tv->vval.v_list = (((ListObject *)(obj))->list);
	++tv->vval.v_list->lv_refcount;
	ret = 0;
    }
    وگرنه اگر (PyIter_Check(obj) || PySequence_Check(obj))
	ret = convert_dl(obj, tv, pyseq_to_tv, lookup_dict);
    وگرنه
    {
	PyErr_FORMAT_TYPE(
		N_("unable to convert %s to a Vim list"),
		obj);
	ret = -1;
    }
    Py_DECREF(lookup_dict);
    بازگشت ret;
}

    ایستا صحیح
ConvertFromPyObject(PyObject *obj, typval_T *tv)
{
    PyObject	*lookup_dict;
    صحیح		ret;

    اگر (!(lookup_dict = PyDict_New()))
	بازگشت -1;
    ret = _ConvertFromPyObject(obj, tv, lookup_dict);
    Py_DECREF(lookup_dict);
    بازگشت ret;
}

    ایستا صحیح
_ConvertFromPyObject(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    اگر (PyType_IsSubtype(obj->ob_type, DictionaryTypePtr))
    {
	tv->v_type = VAR_DICT;
	tv->vval.v_dict = (((DictionaryObject *)(obj))->dict);
	++tv->vval.v_dict->dv_refcount;
    }
    وگرنه اگر (PyType_IsSubtype(obj->ob_type, ListTypePtr))
    {
	tv->v_type = VAR_LIST;
	tv->vval.v_list = (((ListObject *)(obj))->list);
	++tv->vval.v_list->lv_refcount;
    }
    وگرنه اگر (PyType_IsSubtype(obj->ob_type, TupleTypePtr))
    {
	tv->v_type = VAR_TUPLE;
	tv->vval.v_tuple = (((TupleObject *)(obj))->tuple);
	++tv->vval.v_tuple->tv_refcount;
    }
    وگرنه اگر (PyType_IsSubtype(obj->ob_type, FunctionTypePtr))
    {
	FunctionObject *func = (FunctionObject *) obj;
	اگر (func->self != پوچ || func->argv != پوچ)
	{
	    partial_T *pt = ALLOC_CLEAR_ONE(partial_T);

	    set_partial(func, pt, TRUE);
	    tv->vval.v_partial = pt;
	    tv->v_type = VAR_PARTIAL;
	}
	وگرنه
	{
	    اگر (set_string_copy(func->name, tv) == -1)
		بازگشت -1;

	    tv->v_type = VAR_FUNC;
	}
	func_ref(func->name);
    }
    وگرنه اگر (PyBytes_Check(obj))
    {
	char_u	*str;

	اگر (PyBytes_AsStringAndSize(obj, (نویسه **) &str, پوچ) == -1)
	    بازگشت -1;
	اگر (str == پوچ)
	    بازگشت -1;

	اگر (set_string_copy(str, tv) == -1)
	    بازگشت -1;

	tv->v_type = VAR_STRING;
    }
    وگرنه اگر (PyUnicode_Check(obj))
    {
	PyObject	*bytes;
	char_u	*str;

	bytes = PyUnicode_AsEncodedString(obj, ENC_OPT, ERRORS_ENCODE_ARG);
	اگر (bytes == پوچ)
	    بازگشت -1;

	اگر (PyBytes_AsStringAndSize(bytes, (نویسه **) &str, پوچ) == -1)
	    بازگشت -1;
	اگر (str == پوچ)
	    بازگشت -1;

	اگر (set_string_copy(str, tv))
	{
	    Py_XDECREF(bytes);
	    بازگشت -1;
	}
	Py_XDECREF(bytes);

	tv->v_type = VAR_STRING;
    }
#اگر PY_MAJOR_VERSION < 3
    وگرنه اگر (PyInt_Check(obj))
    {
	tv->v_type = VAR_عدد;
	tv->vval.v_عدد = (varnumber_T) PyInt_AsLong(obj);
	اگر (PyErr_Occurred())
	    بازگشت -1;
    }
#endif
    وگرنه اگر (PyLong_Check(obj))
    {
	tv->v_type = VAR_عدد;
	tv->vval.v_عدد = (varnumber_T) PyLong_AsLong(obj);
	اگر (PyErr_Occurred())
	    بازگشت -1;
    }
    وگرنه اگر (PyTuple_Check(obj))
	بازگشت convert_dl(obj, tv, pytuple_to_tv, lookup_dict);
    وگرنه اگر (PyDict_Check(obj))
	بازگشت convert_dl(obj, tv, pydict_to_tv, lookup_dict);
    وگرنه اگر (PyFloat_Check(obj))
    {
	tv->v_type = VAR_FLOAT;
	tv->vval.v_float = (float_T) PyFloat_AsDouble(obj);
    }
    وگرنه اگر (PyObject_HasAttrString(obj, "keys"))
	بازگشت convert_dl(obj, tv, pymap_to_tv, lookup_dict);
    // PyObject_GetIter can create built-in iterator for any sequence object
    وگرنه اگر (PyIter_Check(obj) || PySequence_Check(obj))
	بازگشت convert_dl(obj, tv, pyseq_to_tv, lookup_dict);
    وگرنه اگر (PyMapping_Check(obj))
	بازگشت convert_dl(obj, tv, pymap_to_tv, lookup_dict);
    وگرنه اگر (Py_عدد_Check(obj))
    {
	PyObject	*رقم;

	اگر (!(رقم = Py_عدد_Long(obj)))
	    بازگشت -1;

	tv->v_type = VAR_عدد;
	tv->vval.v_عدد = (varnumber_T) PyLong_AsLong(رقم);

	Py_DECREF(رقم);
    }
    وگرنه اگر (obj == Py_None)
    {
	tv->v_type = VAR_SPECIAL;
	tv->vval.v_عدد = VVAL_NONE;
    }
    وگرنه
    {
	PyErr_FORMAT_TYPE(
		N_("unable to convert %s to a Vim structure"),
		obj);
	بازگشت -1;
    }
    بازگشت 0;
}

    ایستا PyObject *
ConvertToPyObject(typval_T *tv)
{
    typval_T *argv;
    صحیح i;
    اگر (tv == پوچ)
    {
	PyErr_SET_VIM(N_("internal error: NULL reference passed"));
	بازگشت پوچ;
    }
    انتخاب (tv->v_type)
    {
	حالت VAR_STRING:
	    بازگشت PyBytes_FromString(tv->vval.v_string == پوچ
					    ? "" : (نویسه *)tv->vval.v_string);
	حالت VAR_عدد:
	    بازگشت PyLong_FromLong((long) tv->vval.v_عدد);
	حالت VAR_FLOAT:
	    بازگشت PyFloat_FromDouble((دوگانه) tv->vval.v_float);
	حالت VAR_LIST:
	    بازگشت NEW_LIST(tv->vval.v_list);
	حالت VAR_TUPLE:
	    بازگشت NEW_TUPLE(tv->vval.v_tuple);
	حالت VAR_DICT:
	    بازگشت NEW_DICTIONARY(tv->vval.v_dict);
	حالت VAR_FUNC:
	    بازگشت NEW_FUNCTION(tv->vval.v_string == پوچ
					  ? (char_u *)"" : tv->vval.v_string,
					  0, پوچ, پوچ, TRUE);
	حالت VAR_PARTIAL:
	    اگر (tv->vval.v_partial->pt_argc)
	    {
		argv = PyMem_New(typval_T, (size_t)tv->vval.v_partial->pt_argc);
		برای (i = 0; i < tv->vval.v_partial->pt_argc; i++)
		    copy_tv(&tv->vval.v_partial->pt_argv[i], &argv[i]);
	    }
	    وگرنه
		argv = پوچ;
	    اگر (tv->vval.v_partial->pt_dict != پوچ)
		tv->vval.v_partial->pt_dict->dv_refcount++;
	    بازگشت NEW_FUNCTION(tv->vval.v_partial == پوچ
			     ? (char_u *)"" : partial_name(tv->vval.v_partial),
				tv->vval.v_partial->pt_argc, argv,
				tv->vval.v_partial->pt_dict,
				tv->vval.v_partial->pt_auto);
	حالت VAR_BLOB:
	    بازگشت PyBytes_FromStringAndSize(
		(نویسه*) tv->vval.v_blob->bv_ga.ga_data,
		(Py_ssize_t) tv->vval.v_blob->bv_ga.ga_len);
	حالت VAR_UNKNOWN:
	حالت VAR_ANY:
	حالت VAR_VOID:
	حالت VAR_CHANNEL:
	حالت VAR_JOB:
	حالت VAR_INSTR:
	حالت VAR_CLASS:
	حالت VAR_OBJECT:
	حالت VAR_TYPEALIAS:
	    Py_INCREF(Py_None);
	    بازگشت Py_None;
	حالت VAR_BOOL:
	حالت VAR_SPECIAL:
	    انتخاب (tv->vval.v_عدد)
	    {
		حالت VVAL_FALSE: بازگشت ALWAYS_FALSE;
		حالت VVAL_TRUE:  بازگشت ALWAYS_TRUE;
		حالت VVAL_NONE:
		حالت VVAL_NULL:  بازگشت ALWAYS_NONE;
	    }
	    PyErr_SET_VIM(N_("internal error: invalid value type"));
	    بازگشت پوچ;
    }
    بازگشت پوچ;
}

DEFINE_PY_TYPE_OBJECT(CurrentType);

تعریفنوع ساختار
{
    PyObject_HEAD
} CurrentObject;

ایستا CurrentObject TheCurrent =
{
    PyObject_HEAD_INIT_TYPE(CurrentType)
};

    ایستا تهی
init_structs(تهی)
{
    CLEAR_FIELD(OutputType);
    OutputType.tp_name = "vim.message";
    OutputType.tp_basicsize = اندازه(OutputObject);
    OutputType.tp_flags = Py_TPFLAGS_DEFAULT;
    OutputType.tp_doc = "vim message object";
    OutputType.tp_methods = OutputMethods;
#اگر PY_MAJOR_VERSION >= 3
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
#وگرنه
    OutputType.tp_getattr = OutputGetattr;
    OutputType.tp_setattr = OutputSetattr;
    // Disabled, because this causes a crash in test86
    // OutputType.tp_base = &PyFile_Type;
#endif

    CLEAR_FIELD(IterType);
    IterType.tp_name = "vim.iter";
    IterType.tp_basicsize = اندازه(IterObject);
    IterType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    IterType.tp_doc = "generic iterator object";
    IterType.tp_iter = IterIter;
    IterType.tp_iternext = IterNext;
    IterType.tp_dealloc = IterDestructor;
    IterType.tp_traverse = IterTraverse;
    IterType.tp_clear = IterClear;

    CLEAR_FIELD(BufferType);
    BufferType.tp_name = "vim.buffer";
    BufferType.tp_basicsize = اندازه(BufferObject);
    BufferType.tp_dealloc = BufferDestructor;
    BufferType.tp_repr = BufferRepr;
    BufferType.tp_as_sequence = &BufferAsSeq;
    BufferType.tp_as_mapping = &BufferAsMapping;
    BufferType.tp_flags = Py_TPFLAGS_DEFAULT;
    BufferType.tp_doc = "vim buffer object";
    BufferType.tp_methods = BufferMethods;
#اگر PY_MAJOR_VERSION >= 3
    BufferType.tp_getattro = BufferGetattro;
    BufferType.tp_setattro = BufferSetattro;
    BufferType.tp_alloc = call_PyType_GenericAlloc;
    BufferType.tp_new = call_PyType_GenericNew;
    BufferType.tp_free = call_PyObject_Free;
#وگرنه
    BufferType.tp_getattr = BufferGetattr;
    BufferType.tp_setattr = BufferSetattr;
#endif

    CLEAR_FIELD(WindowType);
    WindowType.tp_name = "vim.window";
    WindowType.tp_basicsize = اندازه(WindowObject);
    WindowType.tp_dealloc = WindowDestructor;
    WindowType.tp_repr = WindowRepr;
    WindowType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    WindowType.tp_doc = "vim Window object";
    WindowType.tp_methods = WindowMethods;
    WindowType.tp_traverse = WindowTraverse;
    WindowType.tp_clear = WindowClear;
#اگر PY_MAJOR_VERSION >= 3
    WindowType.tp_getattro = WindowGetattro;
    WindowType.tp_setattro = WindowSetattro;
    WindowType.tp_alloc = call_PyType_GenericAlloc;
    WindowType.tp_new = call_PyType_GenericNew;
    WindowType.tp_free = call_PyObject_Free;
#وگرنه
    WindowType.tp_getattr = WindowGetattr;
    WindowType.tp_setattr = WindowSetattr;
#endif

    CLEAR_FIELD(TabPageType);
    TabPageType.tp_name = "vim.tabpage";
    TabPageType.tp_basicsize = اندازه(TabPageObject);
    TabPageType.tp_dealloc = TabPageDestructor;
    TabPageType.tp_repr = TabPageRepr;
    TabPageType.tp_flags = Py_TPFLAGS_DEFAULT;
    TabPageType.tp_doc = "vim tab page object";
    TabPageType.tp_methods = TabPageMethods;
#اگر PY_MAJOR_VERSION >= 3
    TabPageType.tp_getattro = TabPageGetattro;
    TabPageType.tp_alloc = call_PyType_GenericAlloc;
    TabPageType.tp_new = call_PyType_GenericNew;
    TabPageType.tp_free = call_PyObject_Free;
#وگرنه
    TabPageType.tp_getattr = TabPageGetattr;
#endif

    CLEAR_FIELD(BufMapType);
    BufMapType.tp_name = "vim.bufferlist";
    BufMapType.tp_basicsize = اندازه(BufMapObject);
    BufMapType.tp_as_mapping = &BufMapAsMapping;
    BufMapType.tp_flags = Py_TPFLAGS_DEFAULT;
    BufMapType.tp_iter = BufMapIter;
    BufMapType.tp_doc = "vim buffer list";

    CLEAR_FIELD(WinListType);
    WinListType.tp_name = "vim.windowlist";
    WinListType.tp_basicsize = اندازه(WinListObject);
    WinListType.tp_as_sequence = &WinListAsSeq;
    WinListType.tp_flags = Py_TPFLAGS_DEFAULT;
    WinListType.tp_doc = "vim window list";
    WinListType.tp_dealloc = (destructor)WinListDestructor;

    CLEAR_FIELD(TabListType);
    TabListType.tp_name = "vim.tabpagelist";
    TabListType.tp_basicsize = اندازه(TabListObject);
    TabListType.tp_as_sequence = &TabListAsSeq;
    TabListType.tp_flags = Py_TPFLAGS_DEFAULT;
    TabListType.tp_doc = "vim tab page list";

    CLEAR_FIELD(RangeType);
    RangeType.tp_name = "vim.range";
    RangeType.tp_basicsize = اندازه(RangeObject);
    RangeType.tp_dealloc = RangeDestructor;
    RangeType.tp_repr = RangeRepr;
    RangeType.tp_as_sequence = &RangeAsSeq;
    RangeType.tp_as_mapping = &RangeAsMapping;
    RangeType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    RangeType.tp_doc = "vim Range object";
    RangeType.tp_methods = RangeMethods;
    RangeType.tp_traverse = RangeTraverse;
    RangeType.tp_clear = RangeClear;
#اگر PY_MAJOR_VERSION >= 3
    RangeType.tp_getattro = RangeGetattro;
    RangeType.tp_alloc = call_PyType_GenericAlloc;
    RangeType.tp_new = call_PyType_GenericNew;
    RangeType.tp_free = call_PyObject_Free;
#وگرنه
    RangeType.tp_getattr = RangeGetattr;
#endif

    CLEAR_FIELD(CurrentType);
    CurrentType.tp_name = "vim.currentdata";
    CurrentType.tp_basicsize = اندازه(CurrentObject);
    CurrentType.tp_flags = Py_TPFLAGS_DEFAULT;
    CurrentType.tp_doc = "vim current object";
    CurrentType.tp_methods = CurrentMethods;
#اگر PY_MAJOR_VERSION >= 3
    CurrentType.tp_getattro = CurrentGetattro;
    CurrentType.tp_setattro = CurrentSetattro;
#وگرنه
    CurrentType.tp_getattr = CurrentGetattr;
    CurrentType.tp_setattr = CurrentSetattr;
#endif

    CLEAR_FIELD(DictionaryType);
    DictionaryType.tp_name = "vim.dictionary";
    DictionaryType.tp_basicsize = اندازه(DictionaryObject);
    DictionaryType.tp_dealloc = DictionaryDestructor;
    DictionaryType.tp_as_sequence = &DictionaryAsSeq;
    DictionaryType.tp_as_mapping = &DictionaryAsMapping;
    DictionaryType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE;
    DictionaryType.tp_doc = "dictionary pushing modifications to Vim structure";
    DictionaryType.tp_methods = DictionaryMethods;
    DictionaryType.tp_iter = DictionaryIter;
    DictionaryType.tp_new = DictionaryConstructor;
    DictionaryType.tp_alloc = PyType_GenericAlloc;
#اگر PY_MAJOR_VERSION >= 3
    DictionaryType.tp_getattro = DictionaryGetattro;
    DictionaryType.tp_setattro = DictionarySetattro;
#وگرنه
    DictionaryType.tp_getattr = DictionaryGetattr;
    DictionaryType.tp_setattr = DictionarySetattr;
#endif

    CLEAR_FIELD(ListType);
    ListType.tp_name = "vim.list";
    ListType.tp_dealloc = ListDestructor;
    ListType.tp_basicsize = اندازه(ListObject);
    ListType.tp_as_sequence = &ListAsSeq;
    ListType.tp_as_mapping = &ListAsMapping;
    ListType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE;
    ListType.tp_doc = "list pushing modifications to Vim structure";
    ListType.tp_methods = ListMethods;
    ListType.tp_iter = ListIter;
    ListType.tp_new = ListConstructor;
    ListType.tp_alloc = PyType_GenericAlloc;
#اگر PY_MAJOR_VERSION >= 3
    ListType.tp_getattro = ListGetattro;
    ListType.tp_setattro = ListSetattro;
#وگرنه
    ListType.tp_getattr = ListGetattr;
    ListType.tp_setattr = ListSetattr;
#endif

    // Tuple type
    CLEAR_FIELD(TupleType);
    TupleType.tp_name = "vim.tuple";
    TupleType.tp_dealloc = TupleDestructor;
    TupleType.tp_basicsize = اندازه(TupleObject);
    TupleType.tp_as_sequence = &TupleAsSeq;
    TupleType.tp_as_mapping = &TupleAsMapping;
    TupleType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE;
    TupleType.tp_doc = "tuple pushing modifications to Vim structure";
    TupleType.tp_methods = TupleMethods;
    TupleType.tp_iter = TupleIter;
    TupleType.tp_new = TupleConstructor;
    TupleType.tp_alloc = PyType_GenericAlloc;
#اگر PY_MAJOR_VERSION >= 3
    TupleType.tp_getattro = TupleGetattro;
    TupleType.tp_setattro = TupleSetattro;
#وگرنه
    TupleType.tp_getattr = TupleGetattr;
    TupleType.tp_setattr = TupleSetattr;
#endif

    CLEAR_FIELD(FunctionType);
    FunctionType.tp_name = "vim.function";
    FunctionType.tp_basicsize = اندازه(FunctionObject);
    FunctionType.tp_dealloc = FunctionDestructor;
    FunctionType.tp_call = FunctionCall;
    FunctionType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE;
    FunctionType.tp_doc = "object that calls Vim function";
    FunctionType.tp_methods = FunctionMethods;
    FunctionType.tp_repr = FunctionRepr;
    FunctionType.tp_new = FunctionConstructor;
    FunctionType.tp_alloc = PyType_GenericAlloc;
#اگر PY_MAJOR_VERSION >= 3
    FunctionType.tp_getattro = FunctionGetattro;
#وگرنه
    FunctionType.tp_getattr = FunctionGetattr;
#endif

    CLEAR_FIELD(OptionsType);
    OptionsType.tp_name = "vim.options";
    OptionsType.tp_basicsize = اندازه(OptionsObject);
    OptionsType.tp_as_sequence = &OptionsAsSeq;
    OptionsType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    OptionsType.tp_doc = "object for manipulating options";
    OptionsType.tp_iter = OptionsIter;
    OptionsType.tp_as_mapping = &OptionsAsMapping;
    OptionsType.tp_dealloc = OptionsDestructor;
    OptionsType.tp_traverse = OptionsTraverse;
    OptionsType.tp_clear = OptionsClear;

#اگر PY_VERSION_HEX < 0x030700f0
    CLEAR_FIELD(LoaderType);
    LoaderType.tp_name = "vim.Loader";
    LoaderType.tp_basicsize = اندازه(LoaderObject);
    LoaderType.tp_flags = Py_TPFLAGS_DEFAULT;
    LoaderType.tp_doc = "vim message object";
    LoaderType.tp_methods = LoaderMethods;
    LoaderType.tp_dealloc = LoaderDestructor;
#endif

#اگر PY_MAJOR_VERSION >= 3
    CLEAR_FIELD(vimmodule);
    vimmodule.m_name = "vim";
    vimmodule.m_doc = "Vim Python interface\n";
    vimmodule.m_size = -1;
    vimmodule.m_methods = VimMethods;
#endif
}

    ایستا صحیح
init_types(تهی)
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
#اگر PY_VERSION_HEX < 0x030700f0
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
    بازگشت 0;
}

#ifdef USE_LIMITED_API
    ایستا تهی
shutdown_types(تهی)
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
# اگر PY_VERSION_HEX < 0x030700f0
    PYTYPE_CLEANUP(LoaderType);
# endif
}
#endif

    ایستا صحیح
init_sys_path(تهی)
{
    PyObject	*path;
    PyObject	*path_hook;
    PyObject	*path_hooks;

    اگر (!(path_hook = PyObject_GetAttrString(vim_module, "path_hook")))
	بازگشت -1;

    اگر (!(path_hooks = PySys_GetObject("path_hooks")))
    {
	PyErr_Clear();
	path_hooks = PyList_New(1);
	PyList_SET_ITEM(path_hooks, 0, path_hook);
	اگر (PySys_SetObject("path_hooks", path_hooks))
	{
	    Py_DECREF(path_hooks);
	    بازگشت -1;
	}
	Py_DECREF(path_hooks);
    }
    وگرنه اگر (PyList_Check(path_hooks))
    {
	اگر (PyList_Append(path_hooks, path_hook))
	{
	    Py_DECREF(path_hook);
	    بازگشت -1;
	}
	Py_DECREF(path_hook);
    }
    وگرنه
    {
	VimTryStart();
	emsg(_("Failed to set path hook: sys.path_hooks is not a list\n"
	       "You should now do the following:\n"
	       "- append vim.path_hook to sys.path_hooks\n"
	       "- append vim.VIM_SPECIAL_PATH to sys.path\n"));
	VimTryEnd(); // Discard the error
	Py_DECREF(path_hook);
	بازگشت 0;
    }

    اگر (!(path = PySys_GetObject("path")))
    {
	PyErr_Clear();
	path = PyList_New(1);
	Py_INCREF(vim_special_path_object);
	PyList_SET_ITEM(path, 0, vim_special_path_object);
	اگر (PySys_SetObject("path", path))
	{
	    Py_DECREF(path);
	    بازگشت -1;
	}
	Py_DECREF(path);
    }
    وگرنه اگر (PyList_Check(path))
    {
	اگر (PyList_Append(path, vim_special_path_object))
	    بازگشت -1;
    }
    وگرنه
    {
	VimTryStart();
	emsg(_("Failed to set path: sys.path is not a list\n"
	       "You should now append vim.VIM_SPECIAL_PATH to sys.path"));
	VimTryEnd(); // Discard the error
    }

    بازگشت 0;
}

ایستا ساختار numeric_constant {
    نویسه	*name;
    صحیح		val;
} numeric_constants[] = {
    {"VAR_LOCKED",	VAR_LOCKED},
    {"VAR_FIXED",	VAR_FIXED},
    {"VAR_SCOPE",	VAR_SCOPE},
    {"VAR_DEF_SCOPE",	VAR_DEF_SCOPE},
};

ساختار object_constant {
    نویسه	*name;
    PyObject	*valObject;
};

#define ADD_OBJECT(m, name, obj) \
    اگر (PyModule_AddObject(m, name, obj)) \
	بازگشت -1;

#define ADD_CHECKED_OBJECT(m, name, obj) \
    { \
	PyObject	*valObject = obj; \
	اگر (!valObject) \
	    بازگشت -1; \
	ADD_OBJECT(m, name, valObject); \
    }

    ایستا صحیح
populate_module(PyObject *m)
{
    صحیح		i;
    PyObject	*other_module;
    PyObject	*attr;
    PyObject	*imp;
#اگر PY_VERSION_HEX >= 0x030700f0
    PyObject	*dict;
    PyObject	*cls;
#endif

    برای (i = 0; i < (صحیح)(اندازه(numeric_constants)
					   / اندازه(ساختار numeric_constant));
	    ++i)
	ADD_CHECKED_OBJECT(m, numeric_constants[i].name,
		PyInt_FromLong(numeric_constants[i].val));

    ساختار object_constant object_constants[] = {
	{"buffers",  (PyObject *)(تهی *)&TheBufferMap},
	{"windows",  (PyObject *)(تهی *)&TheWindowList},
	{"tabpages", (PyObject *)(تهی *)&TheTabPageList},
	{"current",  (PyObject *)(تهی *)&TheCurrent},

	{"Buffer",     (PyObject *)BufferTypePtr},
	{"Range",      (PyObject *)RangeTypePtr},
	{"Window",     (PyObject *)WindowTypePtr},
	{"TabPage",    (PyObject *)TabPageTypePtr},
	{"Dictionary", (PyObject *)DictionaryTypePtr},
	{"List",       (PyObject *)ListTypePtr},
	{"Tuple",      (PyObject *)TupleTypePtr},
	{"Function",   (PyObject *)FunctionTypePtr},
	{"Options",    (PyObject *)OptionsTypePtr},
#اگر PY_VERSION_HEX < 0x030700f0
	{"_Loader",    (PyObject *)LoaderTypePtr},
#endif
    };

    برای (i = 0; i < (صحیح)(اندازه(object_constants)
					    / اندازه(ساختار object_constant));
	    ++i)
    {
	PyObject	*valObject;

	valObject = object_constants[i].valObject;
	Py_INCREF(valObject);
	ADD_OBJECT(m, object_constants[i].name, valObject);
    }

    اگر (!(VimError = PyErr_NewException("vim.error", پوچ, پوچ)))
	بازگشت -1;
    ADD_OBJECT(m, "error", VimError);

    ADD_CHECKED_OBJECT(m, "vars",  NEW_DICTIONARY(get_globvar_dict()));
    ADD_CHECKED_OBJECT(m, "vvars", NEW_DICTIONARY(get_vimvar_dict()));
    ADD_CHECKED_OBJECT(m, "options",
	    OptionsNew(SREQ_GLOBAL, پوچ, dummy_check, پوچ));

    اگر (!(other_module = PyImport_ImportModule("os")))
	بازگشت -1;
    ADD_OBJECT(m, "os", other_module);

#اگر PY_MAJOR_VERSION >= 3
    اگر (!(py_getcwd = PyObject_GetAttrString(other_module, "getcwd")))
	بازگشت -1;
#وگرنه
    اگر (!(py_getcwd = PyObject_GetAttrString(other_module, "getcwdu")))
	بازگشت -1;
#endif
    ADD_OBJECT(m, "_getcwd", py_getcwd)

    اگر (!(py_chdir = PyObject_GetAttrString(other_module, "chdir")))
	بازگشت -1;
    ADD_OBJECT(m, "_chdir", py_chdir);
    اگر (!(attr = PyObject_GetAttrString(m, "chdir")))
	بازگشت -1;
    اگر (PyObject_SetAttrString(other_module, "chdir", attr))
    {
	Py_DECREF(attr);
	بازگشت -1;
    }
    Py_DECREF(attr);

    اگر ((py_fchdir = PyObject_GetAttrString(other_module, "fchdir")))
    {
	ADD_OBJECT(m, "_fchdir", py_fchdir);
	اگر (!(attr = PyObject_GetAttrString(m, "fchdir")))
	    بازگشت -1;
	اگر (PyObject_SetAttrString(other_module, "fchdir", attr))
	{
	    Py_DECREF(attr);
	    بازگشت -1;
	}
	Py_DECREF(attr);
    }
    وگرنه
	PyErr_Clear();

    اگر (!(vim_special_path_object = PyString_FromString(vim_special_path)))
	بازگشت -1;

    ADD_OBJECT(m, "VIM_SPECIAL_PATH", vim_special_path_object);

#اگر PY_VERSION_HEX >= 0x030700f0
    اگر (!(imp = PyImport_ImportModule("importlib.machinery")))
	بازگشت -1;

    dict = PyModule_GetDict(imp);

    اگر (!(cls = PyDict_GetItemString(dict, "PathFinder")))
    {
	Py_DECREF(imp);
	بازگشت -1;
    }

    اگر (!(py_find_spec = PyObject_GetAttrString(cls, "find_spec")))
    {
	Py_DECREF(imp);
	بازگشت -1;
    }

# اگر PY_VERSION_HEX < 0x30c00a7
    // find_module has been removed as of Python 3.12.0a7
    اگر ((py_find_module = PyObject_GetAttrString(cls, "find_module")))
	ADD_OBJECT(m, "_find_module", py_find_module);
# endif

    Py_DECREF(imp);

    ADD_OBJECT(m, "_find_spec", py_find_spec);
#وگرنه
    اگر (!(imp = PyImport_ImportModule("imp")))
	بازگشت -1;

    اگر (!(py_find_module = PyObject_GetAttrString(imp, "find_module")))
    {
	Py_DECREF(imp);
	بازگشت -1;
    }

    اگر (!(py_load_module = PyObject_GetAttrString(imp, "load_module")))
    {
	Py_DECREF(py_find_module);
	Py_DECREF(imp);
	بازگشت -1;
    }

    Py_DECREF(imp);

    ADD_OBJECT(m, "_find_module", py_find_module);
    ADD_OBJECT(m, "_load_module", py_load_module);
#endif

    بازگشت 0;
}
