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

#ܐܢ PY_VERSION_HEX < 0x02050000
تعريفنوع صحيح Py_ssize_t;  // Python 2.4 and earlier don't have this type.
#endif

// Use values that are known to work, others may make Vim crash.
#define ENC_OPT (enc_utf8 ? "utf-8" : enc_dbcs ? "euc-jp" : (حرف *)p_enc)
#define DOPY_FUNC "_vim_pydo"

ساكن ثابت حرف *vim_special_path = "_vim_path_";

#define PyErr_SET_STRING(exc, str) PyErr_SetString(exc, _(str))
#define PyErr_SetVim(str) PyErr_SetString(VimError, str)
#define PyErr_SET_VIM(str) PyErr_SET_STRING(VimError, str)
#define PyErr_FORMAT(exc, str, arg) PyErr_Format(exc, _(str), arg)
#define PyErr_FORMAT2(exc, str, arg1, arg2) PyErr_Format(exc, _(str), arg1,arg2)
#define PyErr_VIM_FORMAT(str, arg) PyErr_FORMAT(VimError, str, arg)

#ifdef USE_LIMITED_API
// Limited Python API. Need to call only exposed functions and remap macros.
// PyTypeObject is an opaque struct.

تعريفنوع بنية {
    lenfunc sq_length;
    binaryfunc sq_concat;
    ssizeargfunc sq_repeat;
    ssizeargfunc sq_item;
    فراغ *was_sq_slice;
    ssizeobjargproc sq_ass_item;
    فراغ *was_sq_ass_slice;
    objobjproc sq_contains;

    binaryfunc sq_inplace_concat;
    ssizeargfunc sq_inplace_repeat;
} PySequenceMethods;

تعريفنوع بنية {
    lenfunc mp_length;
    binaryfunc mp_subscript;
    objobjargproc mp_ass_subscript;
} PyMappingMethods;

// This struct emulates the concrete _typeobject struct to allow the code to
// work the same way in both limited and full Python APIs.
بنية typeobject_wrapper {
    ثابت حرف *tp_name;
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

    ثابت حرف *tp_doc;

    traverseproc tp_traverse;

    inquiry tp_clear;

    getiterfunc tp_iter;
    iternextfunc tp_iternext;

    بنية PyMethodDef *tp_methods;
    بنية _typeobject *tp_base;
    allocfunc tp_alloc;
    newfunc tp_new;
    freefunc tp_free;
};

# define DEFINE_PY_TYPE_OBJECT(type) \
    ساكن بنية typeobject_wrapper type; \
    ساكن PyTypeObject* type##Ptr = عدم

// PyObject_HEAD_INIT_TYPE and PyObject_FINISH_INIT_TYPE need to come in pairs
// We first initialize with NULL because the type is not allocated until
// init_types() is called later. It's in FINISH_INIT_TYPE where we fill the
// type in with the newly allocated type.
# define PyObject_HEAD_INIT_TYPE(type) PyObject_HEAD_INIT(عدم)
# define PyObject_FINISH_INIT_TYPE(obj, type) obj.ob_base.ob_type = type##Ptr

# define Py_TYPE_GET_TP_ALLOC(type) ((allocfunc)PyType_GetSlot(type, Py_tp_alloc))
# define Py_TYPE_GET_TP_METHODS(type) ((PyMethodDef *)PyType_GetSlot(type, Py_tp_methods))

// PyObject_NEW is not part of stable ABI, but PyObject_Malloc/Init are.
ساكن PyObject* Vim_PyObject_New(PyTypeObject *type, size_t objsize)
{
    PyObject *obj = (PyObject *)PyObject_Malloc(objsize);
    ܐܢ (obj == عدم)
	ܦܢܐ PyErr_NoMemory();
    ܦܢܐ PyObject_Init(obj, type);
}
# undef PyObject_NEW
# define PyObject_NEW(type, typeobj) ((type *)Vim_PyObject_New(typeobj, حجم(type)))

// This is a somewhat convoluted because limited API doesn't expose an easy way
// to get the tp_name field, and so we have to manually reconstruct it as
// "__module__.__name__" (with __module__ omitted for builtins to emulate
// Python behavior). Also, some of the more convenient functions like
// PyUnicode_AsUTF8AndSize and PyType_GetQualName() are not available until
// late Python 3 versions, and won't be available if you set Py_LIMITED_API too
// low.
# define PyErr_FORMAT_TYPE(msg, obj) \
    ܥܒܕ { \
	PyObject* qualname = PyObject_GetAttrString((PyObject*)(obj)->ob_type, "__qualname__"); \
	ܐܢ (qualname == عدم) \
	{ \
	    PyErr_FORMAT(PyExc_TypeError, msg, "(NULL)"); \
	    اكسر; \
	} \
	PyObject* module = PyObject_GetAttrString((PyObject*)(obj)->ob_type, "__module__"); \
	PyObject* full; \
	ܐܢ (module == عدم || PyUnicode_CompareWithASCIIString(module, "builtins") == 0 \
		|| PyUnicode_CompareWithASCIIString(module, "__main__") == 0) \
	{ \
	    full = qualname; \
	    Py_INCREF(full); \
	} \
	ܐܚܪܢܐ \
	    full = PyUnicode_FromFormat("%U.%U", module, qualname); \
	PyObject* full_bytes = PyUnicode_AsUTF8String(full); \
	ثابت حرف* full_str = PyBytes_AsString(full_bytes); \
	full_str = full_str == عدم ? "(NULL)" : full_str; \
	PyErr_FORMAT(PyExc_TypeError, msg, full_str); \
	Py_DECREF(qualname); \
	Py_XDECREF(module); \
	Py_XDECREF(full); \
	Py_XDECREF(full_bytes); \
    } ܟܡܐ (0)

# define PyList_GET_ITEM(list, i) PyList_GetItem(list, i)
# define PyList_GET_SIZE(o) PyList_Size(o)

// PyList_SET_ITEM and PyList_SetItem have slightly different behaviors. The
// former will leave the old item dangling, and the latter will decref on it.
// Since we only use this on new lists, this difference doesn't matter.
# define PyList_SET_ITEM(list, i, item) PyList_SetItem(list, i, item)

# define PyTuple_GET_ITEM(o, pos) PyTuple_GetItem(o, pos)
# define PyTuple_GET_SIZE(o) PyTuple_Size(o)

# ܐܢ Py_LIMITED_API < 0x03080000
// PyIter_check only became part of stable ABI in 3.8, and there is no easy way
// to check for it in the API. We simply return false as a compromise. This
// does mean we should avoid compiling with stable ABI < 3.8.
#  undef PyIter_Check
#  define PyIter_Check(obj) (FALSE)
# endif

ساكن PyTypeObject* AddHeapType(بنية typeobject_wrapper* type_object)
{
    PyType_Spec type_spec;
    type_spec.name = type_object->tp_name;
    type_spec.basicsize = type_object->tp_basicsize;
    type_spec.itemsize = 0;
    type_spec.flags = type_object->tp_flags;

    // We just need to statically allocate a large enough buffer that can hold
    // all slots. We need to leave a null-terminated slot at the end.
    PyType_Slot slots[40] = { {0, عدم} };
    size_t slot_i = 0;

# define ADD_TP_SLOT(slot_name) \
    ܐܢ (slot_i >= 40) ܦܢܐ عدم; /* this should never happen */ \
    ܐܢ (type_object->slot_name != عدم) \
    { \
	slots[slot_i].slot = Py_##slot_name; \
	slots[slot_i].pfunc = (فراغ*)type_object->slot_name; \
	++slot_i; \
    }
# define ADD_TP_SUB_SLOT(sub_slot, slot_name) \
    ܐܢ (slot_i >= 40) ܦܢܐ عدم; /* this should never happen */ \
    ܐܢ (type_object->sub_slot != عدم && type_object->sub_slot->slot_name != عدم) \
    { \
	slots[slot_i].slot = Py_##slot_name; \
	slots[slot_i].pfunc = (فراغ*)type_object->sub_slot->slot_name; \
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
    ܦܢܐ (PyTypeObject*)newtype;
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
    ܐܢ (type##Ptr == عدم) \
	ܦܢܐ -1;
# define PYTYPE_CLEANUP(type) \
    type##Ptr = عدم;

// Limited API does not provide PyRun_* functions. Need to implement manually
// using PyCompile and PyEval.
ساكن PyObject* Vim_PyRun_String(ثابت حرف *str, صحيح start, PyObject *globals, PyObject *locals)
{
    // Just pass "" for filename for now.
    PyObject* compiled = Py_CompileString(str, "", start);
    ܐܢ (compiled == عدم)
	ܦܢܐ عدم;

    PyObject* eval_result = PyEval_EvalCode(compiled, globals, locals);
    Py_DECREF(compiled);
    ܦܢܐ eval_result;
}
ساكن صحيح Vim_PyRun_SimpleString(ثابت حرف *str)
{
    // This function emulates CPython's implementation.
    PyObject* m = PyImport_AddModule("__main__");
    ܐܢ (m == عدم)
	ܦܢܐ -1;
    PyObject* d = PyModule_GetDict(m);
    PyObject* output = Vim_PyRun_String(str, Py_file_input, d, d);
    ܐܢ (output == عدم)
    {
	PyErr_PrintEx(TRUE);
	ܦܢܐ -1;
    }
    Py_DECREF(output);
    ܦܢܐ 0;
}
# define PyRun_String Vim_PyRun_String
# define PyRun_SimpleString Vim_PyRun_SimpleString

#ܐܚܪܢܐ // !defined(USE_LIMITED_API)

// Full Python API. Can make use of structs and macros directly.
# define DEFINE_PY_TYPE_OBJECT(type) \
    ساكن PyTypeObject type; \
    ساكن PyTypeObject* type##Ptr = &type
# define PyObject_HEAD_INIT_TYPE(type) PyObject_HEAD_INIT(&type)

# define Py_TYPE_GET_TP_ALLOC(type) type->tp_alloc
# define Py_TYPE_GET_TP_METHODS(type) type->tp_methods

# define Py_TYPE_NAME(obj) ((obj)->ob_type->tp_name == عدم \
	? "(NULL)" \
	: (obj)->ob_type->tp_name)
# define PyErr_FORMAT_TYPE(msg, obj) \
    PyErr_FORMAT(PyExc_TypeError, msg, \
	    Py_TYPE_NAME(obj))

// Add a static type
# define PYTYPE_READY(type) \
    ܐܢ (PyType_Ready(type##Ptr)) \
	ܦܢܐ -1;

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

تعريفنوع فراغ (*rangeinitializer)(فراغ *);
تعريفنوع فراغ (*runner)(ثابت حرف *, dict_T *, فراغ *
#ifdef PY_CAN_RECURSE
	, PyGILState_STATE *
#endif
	);

ساكن صحيح ConvertFromPyObject(PyObject *, typval_T *);
ساكن صحيح _ConvertFromPyObject(PyObject *, typval_T *, PyObject *);
ساكن صحيح ConvertFromPyMapping(PyObject *, typval_T *);
ساكن صحيح ConvertFromPySequence(PyObject *, typval_T *);
ساكن PyObject *WindowNew(win_T *, tabpage_T *);
ساكن PyObject *BufferNew (buf_T *);
ساكن PyObject *LineToString(ثابت حرف *);

ساكن PyInt RangeStart;
ساكن PyInt RangeEnd;

ساكن PyObject *globals;

ساكن PyObject *py_chdir;
ساكن PyObject *py_fchdir;
ساكن PyObject *py_getcwd;
ساكن PyObject *vim_module;
ساكن PyObject *vim_special_path_object;

#ܐܢ PY_VERSION_HEX >= 0x030700f0
ساكن PyObject *py_find_spec;
#ܐܚܪܢܐ
ساكن PyObject *py_load_module;
#endif
#ܐܢ PY_VERSION_HEX < 0x30c00a7
ساكن PyObject *py_find_module;
#endif

ساكن PyObject *VimError;

/*
 * obtain a lock on the Vim data structures
 */
    ساكن فراغ
Python_Lock_Vim(فراغ)
{
}

/*
 * release a lock on the Vim data structures
 */
    ساكن فراغ
Python_Release_Vim(فراغ)
{
}

/*
 * The "todecref" argument holds a pointer to PyObject * that must be
 * DECREF'ed after returned char_u * is no longer needed or NULL if all what
 * was needed to generate returned value is object.
 *
 * Use Py_XDECREF to decrement reference count.
 */
    ساكن char_u *
StringToChars(PyObject *obj, PyObject **todecref)
{
    char_u	*str;

    ܐܢ (PyBytes_Check(obj))
    {

	ܐܢ (PyBytes_AsStringAndSize(obj, (حرف **) &str, عدم) == -1
		|| str == عدم)
	    ܦܢܐ عدم;

	*todecref = عدم;
    }
    ܐܚܪܢܐ ܐܢ (PyUnicode_Check(obj))
    {
	PyObject	*bytes;

	ܐܢ (!(bytes = PyUnicode_AsEncodedString(obj, ENC_OPT,
							   ERRORS_ENCODE_ARG)))
	    ܦܢܐ عدم;

	ܐܢ (PyBytes_AsStringAndSize(bytes, (حرف **) &str, عدم) == -1
		|| str == عدم)
	{
	    Py_DECREF(bytes);
	    ܦܢܐ عدم;
	}

	*todecref = bytes;
    }
    ܐܚܪܢܐ
    {
#ܐܢ PY_MAJOR_VERSION < 3
	PyErr_FORMAT_TYPE(N_("expected str() or unicode() instance, but got %s"),
		obj);
#ܐܚܪܢܐ
	PyErr_FORMAT_TYPE(N_("expected bytes() or str() instance, but got %s"),
		obj);
#endif
	ܦܢܐ عدم;
    }

    ܦܢܐ (char_u *) str;
}

#define NUMBER_LONG     1
#define NUMBER_INT      2
#define NUMBER_NATURAL  4
#define NUMBER_UNSIGNED 8

    ساكن صحيح
NumberToLong(PyObject *obj, long *result, صحيح flags)
{
#ܐܢ PY_MAJOR_VERSION < 3
    ܐܢ (PyInt_Check(obj))
    {
	*result = PyInt_AsLong(obj);
	ܐܢ (PyErr_Occurred())
	    ܦܢܐ -1;
    }
    ܐܚܪܢܐ
#endif
    ܐܢ (PyLong_Check(obj))
    {
	*result = PyLong_AsLong(obj);
	ܐܢ (PyErr_Occurred())
	    ܦܢܐ -1;
    }
    ܐܚܪܢܐ ܐܢ (PyNumber_Check(obj))
    {
	PyObject	*num;

	ܐܢ (!(num = PyNumber_Long(obj)))
	    ܦܢܐ -1;

	*result = PyLong_AsLong(num);

	Py_DECREF(num);

	ܐܢ (PyErr_Occurred())
	    ܦܢܐ -1;
    }
    ܐܚܪܢܐ
    {
#ܐܢ PY_MAJOR_VERSION < 3
	PyErr_FORMAT_TYPE(
		N_("expected int(), long() or something supporting "
		   "coercing to long(), but got %s"),
		obj);
#ܐܚܪܢܐ
	PyErr_FORMAT_TYPE(
		N_("expected int() or something supporting coercing to int(), "
		   "but got %s"),
		obj);
#endif
	ܦܢܐ -1;
    }

    ܐܢ (flags & NUMBER_INT)
    {
	ܐܢ (*result > INT_MAX)
	{
	    PyErr_SET_STRING(PyExc_OverflowError,
		    N_("value is too large to fit into C int type"));
	    ܦܢܐ -1;
	}
	ܐܚܪܢܐ ܐܢ (*result < INT_MIN)
	{
	    PyErr_SET_STRING(PyExc_OverflowError,
		    N_("value is too small to fit into C int type"));
	    ܦܢܐ -1;
	}
    }

    ܐܢ (flags & NUMBER_NATURAL)
    {
	ܐܢ (*result <= 0)
	{
	    PyErr_SET_STRING(PyExc_ValueError,
		    N_("number must be greater than zero"));
	    ܦܢܐ -1;
	}
    }
    ܐܚܪܢܐ ܐܢ (flags & NUMBER_UNSIGNED)
    {
	ܐܢ (*result < 0)
	{
	    PyErr_SET_STRING(PyExc_ValueError,
		    N_("number must be greater or equal to zero"));
	    ܦܢܐ -1;
	}
    }

    ܦܢܐ 0;
}

    ساكن صحيح
add_string(PyObject *list, حرف *s)
{
    PyObject	*string;

    ܐܢ (!(string = PyString_FromString(s)))
	ܦܢܐ -1;

    ܐܢ (PyList_Append(list, string))
    {
	Py_DECREF(string);
	ܦܢܐ -1;
    }

    Py_DECREF(string);
    ܦܢܐ 0;
}

    ساكن PyObject *
ObjectDir(PyObject *self, حرف **attributes)
{
    PyMethodDef	*method;
    حرف	**attr;
    PyObject	*ret;

    ܐܢ (!(ret = PyList_New(0)))
	ܦܢܐ عدم;

    ܐܢ (self)
	لكل (method = Py_TYPE_GET_TP_METHODS(self->ob_type) ; method->ml_name != عدم ; ++method)
	    ܐܢ (add_string(ret, (حرف *)method->ml_name))
	    {
		Py_DECREF(ret);
		ܦܢܐ عدم;
	    }

    لكل (attr = attributes ; *attr ; ++attr)
	ܐܢ (add_string(ret, *attr))
	{
	    Py_DECREF(ret);
	    ܦܢܐ عدم;
	}

#ܐܢ PY_MAJOR_VERSION < 3
    ܐܢ (add_string(ret, "__members__"))
    {
	Py_DECREF(ret);
	ܦܢܐ عدم;
    }
#endif

    ܦܢܐ ret;
}

// Output buffer management

// Function to write a line, points to either msg() or emsg().
تعريفنوع صحيح (*writefn)(حرف *);

DEFINE_PY_TYPE_OBJECT(OutputType);

تعريفنوع بنية
{
    PyObject_HEAD
    long softspace;
    long error;
} OutputObject;

ساكن حرف *OutputAttrs[] = {
    "softspace",
    عدم
};

    ساكن PyObject *
OutputDir(PyObject *self, PyObject *args UNUSED)
{
    ܦܢܐ ObjectDir(self, OutputAttrs);
}

    ساكن صحيح
OutputSetattr(PyObject *self_obj, حرف *name, PyObject *valObject)
{
    OutputObject *self = (OutputObject*)self_obj;
    ܐܢ (valObject == عدم)
    {
	PyErr_SET_STRING(PyExc_AttributeError,
		N_("can't delete OutputObject attributes"));
	ܦܢܐ -1;
    }

    ܐܢ (strcmp(name, "softspace") == 0)
    {
	ܐܢ (NumberToLong(valObject, &(self->softspace), NUMBER_UNSIGNED))
	    ܦܢܐ -1;
	ܦܢܐ 0;
    }

    PyErr_FORMAT(PyExc_AttributeError, N_("invalid attribute: %s"), name);
    ܦܢܐ -1;
}

// Buffer IO, we write one whole line at a time.
ساكن garray_T io_ga = {0, 0, 1, 80, عدم};
ساكن writefn old_fn = عدم;

    ساكن فراغ
PythonIO_Flush(فراغ)
{
    ܐܢ (old_fn != عدم && io_ga.ga_len > 0)
    {
	((حرف *)io_ga.ga_data)[io_ga.ga_len] = NUL;
	// We don't know what emsg_severe should be here, so ... hope?
	old_fn((حرف *)io_ga.ga_data);
    }
    io_ga.ga_len = 0;
}

    ساكن فراغ
writer(writefn fn, char_u *str, PyInt n, صحيح severe)
{
    char_u *ptr;

    // Flush when switching output function.
    ܐܢ (fn != old_fn)
	PythonIO_Flush();
    old_fn = fn;

    // Write each NL separated line.  Text after the last NL is kept for
    // writing later.
    // For normal messages: Do not output when "got_int" was set.  This avoids
    // a loop gone crazy flooding the terminal with messages.  Also for when
    // "q" is pressed at the more-prompt.
    ܟܡܐ (n > 0 && (ptr = memchr(str, '\n', n)) != عدم
					  && (fn == (writefn)emsg || !got_int))
    {
	PyInt len = ptr - str;

	ܐܢ (ga_grow(&io_ga, (صحيح)(len + 1)) == FAIL)
	    اكسر;

	mch_memmove(((حرف *)io_ga.ga_data) + io_ga.ga_len, str, (size_t)len);
	((حرف *)io_ga.ga_data)[io_ga.ga_len + len] = NUL;
	emsg_severe = severe;
	fn((حرف *)io_ga.ga_data);
	str = ptr + 1;
	n -= len + 1;
	io_ga.ga_len = 0;
    }

    // Put the remaining text into io_ga for later printing.
    ܐܢ (n > 0 && (fn == (writefn)emsg || !got_int)
					&& ga_grow(&io_ga, (صحيح)(n + 1)) == OK)
    {
	mch_memmove(((حرف *)io_ga.ga_data) + io_ga.ga_len, str, (size_t)n);
	io_ga.ga_len += (صحيح)n;
    }
}

    ساكن صحيح
write_output(OutputObject *self, PyObject *string)
{
    Py_ssize_t	len = 0;
    حرف	*str = عدم;
    صحيح		error = self->error;

    ܐܢ (!PyArg_Parse(string, "et#", ENC_OPT, &str, &len))
	ܦܢܐ -1;

    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    writer((writefn)(error ? emsg : msg), (char_u *)str, len, error);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS
    PyMem_Free(str);

    ܦܢܐ 0;
}

    ساكن PyObject *
OutputWrite(OutputObject *self, PyObject *string)
{
    ܐܢ (write_output(self, string))
	ܦܢܐ عدم;

    Py_INCREF(Py_None);
    ܦܢܐ Py_None;
}

    ساكن PyObject *
OutputWritelines(OutputObject *self, PyObject *seq)
{
    PyObject	*iterator;
    PyObject	*item;

    ܐܢ (!(iterator = PyObject_GetIter(seq)))
	ܦܢܐ عدم;

    ܟܡܐ ((item = PyIter_Next(iterator)))
    {
	ܐܢ (write_output(self, item))
	{
	    Py_DECREF(iterator);
	    Py_DECREF(item);
	    ܦܢܐ عدم;
	}
	Py_DECREF(item);
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    ܐܢ (PyErr_Occurred())
	ܦܢܐ عدم;

    Py_INCREF(Py_None);
    ܦܢܐ Py_None;
}

    ساكن PyObject *
AlwaysNone(PyObject *self UNUSED, PyObject *args UNUSED)
{
    // do nothing
    Py_INCREF(Py_None);
    ܦܢܐ Py_None;
}
#define ALWAYS_NONE AlwaysNone(عدم, عدم)

    ساكن PyObject *
AlwaysFalse(PyObject *self UNUSED, PyObject *args UNUSED)
{
    // do nothing
    PyObject	*ret = Py_False;
    Py_INCREF(ret);
    ܦܢܐ ret;
}
#define ALWAYS_FALSE AlwaysFalse(عدم, عدم)

    ساكن PyObject *
AlwaysTrue(PyObject *self UNUSED, PyObject *args UNUSED)
{
    // do nothing
    PyObject	*ret = Py_True;
    Py_INCREF(ret);
    ܦܢܐ ret;
}
#define ALWAYS_TRUE AlwaysTrue(عدم, عدم)

/***************/

ساكن بنية PyMethodDef OutputMethods[] = {
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
    { عدم,	    عدم,				0,		عدم}
};

ساكن OutputObject Output =
{
    PyObject_HEAD_INIT_TYPE(OutputType)
    0,
    0
};

ساكن OutputObject Error =
{
    PyObject_HEAD_INIT_TYPE(OutputType)
    0,
    1
};

    ساكن صحيح
PythonIO_Init_io(فراغ)
{
    ܐܢ (PySys_SetObject("stdout", (PyObject *)(فراغ *)&Output))
	ܦܢܐ -1;
    ܐܢ (PySys_SetObject("stderr", (PyObject *)(فراغ *)&Error))
	ܦܢܐ -1;

    ܐܢ (PyErr_Occurred())
    {
	emsg(_(e_python_error_initialising_io_object));
	ܦܢܐ -1;
    }

    ܦܢܐ 0;
}

#ܐܢ PY_VERSION_HEX < 0x030700f0
ساكن PyObject *call_load_module(حرف *name, صحيح len, PyObject *find_module_result);

تعريفنوع بنية
{
    PyObject_HEAD
    حرف	*fullname;
    PyObject	*result;
} LoaderObject;
DEFINE_PY_TYPE_OBJECT(LoaderType);

    ساكن فراغ
LoaderDestructor(PyObject *self_obj)
{
    LoaderObject *self = (LoaderObject*)self_obj;
    vim_free(self->fullname);
    Py_XDECREF(self->result);
    DESTRUCTOR_FINISH(self);
}

    ساكن PyObject *
LoaderLoadModule(LoaderObject *self, PyObject *args UNUSED)
{
    حرف	*fullname = self->fullname;
    PyObject	*result = self->result;
    PyObject	*module;

    ܐܢ (!fullname)
    {
	module = result ? result : Py_None;
	Py_INCREF(module);
	ܦܢܐ module;
    }

    module = call_load_module(fullname, (صحيح)STRLEN(fullname), result);

    self->fullname = عدم;
    self->result = module;

    vim_free(fullname);
    Py_DECREF(result);

    ܐܢ (!module)
    {
	ܐܢ (PyErr_Occurred())
	    ܦܢܐ عدم;

	Py_INCREF(Py_None);
	ܦܢܐ Py_None;
    }

    Py_INCREF(module);
    ܦܢܐ module;
}

ساكن بنية PyMethodDef LoaderMethods[] = {
    // name,	    function,				calling,	doc
    {"load_module", (PyCFunction)LoaderLoadModule,	METH_VARARGS,	""},
    { عدم,	    عدم,				0,		عدم}
};
#endif

/*
 * Check to see whether a Vim error has been reported, or a keyboard
 * interrupt has been detected.
 */
    ساكن فراغ
VimTryStart(فراغ)
{
    ++trylevel;
}

    ساكن صحيح
VimTryEnd(فراغ)
{
    --trylevel;
    // Without this it stops processing all subsequent Vim script commands and
    // generates strange error messages if I e.g. try calling Test() in a cycle
    did_emsg = FALSE;
    // Keyboard interrupt should be preferred over anything else
    ܐܢ (got_int)
    {
	ܐܢ (did_throw)
	    discard_current_exception();
	got_int = FALSE;
	PyErr_SetNone(PyExc_KeyboardInterrupt);
	ܦܢܐ -1;
    }
    ܐܚܪܢܐ ܐܢ (msg_list != عدم && *msg_list != عدم)
    {
	صحيح	should_free;
	حرف	*msg;

	msg = get_exception_string(*msg_list, ET_ERROR, عدم, &should_free);

	ܐܢ (msg == عدم)
	{
	    PyErr_NoMemory();
	    ܦܢܐ -1;
	}

	PyErr_SetVim(msg);

	free_global_msglist();

	ܐܢ (should_free)
	    vim_free(msg);

	ܦܢܐ -1;
    }
    ܐܚܪܢܐ ܐܢ (!did_throw)
	ܦܢܐ (PyErr_Occurred() ? -1 : 0);
    // Python exception is preferred over Vim one; unlikely to occur though
    ܐܚܪܢܐ ܐܢ (PyErr_Occurred())
    {
	discard_current_exception();
	ܦܢܐ -1;
    }
    // Finally transform Vim script exception to python one
    ܐܚܪܢܐ
    {
	PyErr_SetVim((حرف *)current_exception->value);
	discard_current_exception();
	ܦܢܐ -1;
    }
}

    ساكن صحيح
VimCheckInterrupt(فراغ)
{
    ܐܢ (got_int)
    {
	PyErr_SetNone(PyExc_KeyboardInterrupt);
	ܦܢܐ 1;
    }
    ܦܢܐ 0;
}

// Vim module - Implementation

    ساكن PyObject *
VimCommand(PyObject *self UNUSED, PyObject *string)
{
    char_u	*cmd;
    PyObject	*ret;
    PyObject	*todecref;

    ܐܢ (!(cmd = StringToChars(string, &todecref)))
	ܦܢܐ عدم;

    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();

    VimTryStart();
    do_cmdline_cmd(cmd);
    update_screen(UPD_VALID);

    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    ܐܢ (VimTryEnd())
	ret = عدم;
    ܐܚܪܢܐ
	ret = Py_None;

    Py_XINCREF(ret);
    Py_XDECREF(todecref);
    ܦܢܐ ret;
}

/*
 * Function to translate a typval_T into a PyObject; this will recursively
 * translate lists/dictionaries into their Python equivalents.
 *
 * The depth parameter is to avoid infinite recursion, set it to 1 when
 * you call VimToPython.
 */
    ساكن PyObject *
VimToPython(typval_T *our_tv, صحيح depth, PyObject *lookup_dict)
{
    PyObject	*ret;
    PyObject	*newObj;
    حرف	ptrBuf[حجم(فراغ *) * 2 + 3];

    // Avoid infinite recursion
    ܐܢ (depth > 100)
    {
	Py_INCREF(Py_None);
	ret = Py_None;
	ܦܢܐ ret;
    }

    // Check if we run into a recursive loop.  The item must be in lookup_dict
    // then and we can use it again.
    ܐܢ ((our_tv->v_type == VAR_LIST && our_tv->vval.v_list != عدم)
	    || (our_tv->v_type == VAR_TUPLE && our_tv->vval.v_tuple != عدم)
	    || (our_tv->v_type == VAR_DICT && our_tv->vval.v_dict != عدم))
    {
	sprintf(ptrBuf, "%p",
		our_tv->v_type == VAR_LIST ? (فراغ *)our_tv->vval.v_list
		    : our_tv->v_type == VAR_TUPLE ?
			(فراغ *)our_tv->vval.v_tuple
			: (فراغ *)our_tv->vval.v_dict);

	ܐܢ ((ret = PyDict_GetItemString(lookup_dict, ptrBuf)))
	{
	    Py_INCREF(ret);
	    ܦܢܐ ret;
	}
    }

    ܐܢ (our_tv->v_type == VAR_STRING)
	ret = PyString_FromString(our_tv->vval.v_string == عدم
					? "" : (حرف *)our_tv->vval.v_string);
    ܐܚܪܢܐ ܐܢ (our_tv->v_type == VAR_NUMBER)
    {
	حرف buf[NUMBUFLEN];

	// For backwards compatibility numbers are stored as strings.
	sprintf(buf, "%lld", (long long)our_tv->vval.v_number);
	ret = PyString_FromString((حرف *)buf);
    }
    ܐܚܪܢܐ ܐܢ (our_tv->v_type == VAR_FLOAT)
    {
	حرف buf[NUMBUFLEN];

	sprintf(buf, "%f", our_tv->vval.v_float);
	ret = PyString_FromString((حرف *)buf);
    }
    ܐܚܪܢܐ ܐܢ (our_tv->v_type == VAR_LIST)
    {
	list_T		*list = our_tv->vval.v_list;
	listitem_T	*curr;

	ܐܢ (list == عدم)
	    ܦܢܐ عدم;

	ܐܢ (!(ret = PyList_New(0)))
	    ܦܢܐ عدم;

	ܐܢ (PyDict_SetItemString(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    ܦܢܐ عدم;
	}

	CHECK_LIST_MATERIALIZE(list);
	FOR_ALL_LIST_ITEMS(list, curr)
	{
	    ܐܢ (!(newObj = VimToPython(&curr->li_tv, depth + 1, lookup_dict)))
	    {
		Py_DECREF(ret);
		ܦܢܐ عدم;
	    }
	    ܐܢ (PyList_Append(ret, newObj))
	    {
		Py_DECREF(newObj);
		Py_DECREF(ret);
		ܦܢܐ عدم;
	    }
	    Py_DECREF(newObj);
	}
    }
    ܐܚܪܢܐ ܐܢ (our_tv->v_type == VAR_TUPLE)
    {
	tuple_T	*tuple = our_tv->vval.v_tuple;
	صحيح	len;

	ܐܢ (tuple == عدم)
	    ܦܢܐ عدم;

	len = TUPLE_LEN(tuple);

	ret = PyTuple_New(len);
	ܐܢ (ret == عدم)
	    ܦܢܐ عدم;

	لكل (صحيح idx = 0; idx < len; idx++)
	{
	    typval_T	*item_tv = TUPLE_ITEM(tuple, idx);

	    newObj = VimToPython(item_tv, depth + 1, lookup_dict);
	    ܐܢ (!newObj)
	    {
		Py_DECREF(ret);
		ܦܢܐ عدم;
	    }
	    PyTuple_SetItem(ret, idx, newObj);
	}

	ܐܢ (PyDict_SetItemString(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    ܦܢܐ عدم;
	}
    }
    ܐܚܪܢܐ ܐܢ (our_tv->v_type == VAR_DICT)
    {

	hashtab_T	*ht;
	long_u		todo;
	hashitem_T	*hi;
	dictitem_T	*di;

	ܐܢ (our_tv->vval.v_dict == عدم)
	    ܦܢܐ عدم;
	ht = &our_tv->vval.v_dict->dv_hashtab;

	ܐܢ (!(ret = PyDict_New()))
	    ܦܢܐ عدم;

	ܐܢ (PyDict_SetItemString(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    ܦܢܐ عدم;
	}

	todo = ht->ht_used;
	لكل (hi = ht->ht_array; todo > 0; ++hi)
	{
	    ܐܢ (!HASHITEM_EMPTY(hi))
	    {
		--todo;

		di = dict_lookup(hi);
		ܐܢ (!(newObj = VimToPython(&di->di_tv, depth + 1, lookup_dict)))
		{
		    Py_DECREF(ret);
		    ܦܢܐ عدم;
		}
		ܐܢ (PyDict_SetItemString(ret, (حرف *)hi->hi_key, newObj))
		{
		    Py_DECREF(ret);
		    Py_DECREF(newObj);
		    ܦܢܐ عدم;
		}
	    }
	}
    }
    ܐܚܪܢܐ ܐܢ (our_tv->v_type == VAR_BOOL)
    {
	ܐܢ (our_tv->vval.v_number == VVAL_FALSE)
	{
	    ret = Py_False;
	    Py_INCREF(ret);
	}
	ܐܚܪܢܐ
	{
	    ret = Py_True;
	    Py_INCREF(ret);
	}
	ܦܢܐ ret;
    }
    ܐܚܪܢܐ ܐܢ (our_tv->v_type == VAR_SPECIAL)
    {
	Py_INCREF(Py_None);
	ret = Py_None;
	ܦܢܐ ret;
    }
    ܐܚܪܢܐ ܐܢ (our_tv->v_type == VAR_BLOB)
	ret = PyBytes_FromStringAndSize(
		(حرف*) our_tv->vval.v_blob->bv_ga.ga_data,
		(Py_ssize_t) our_tv->vval.v_blob->bv_ga.ga_len);
    ܐܚܪܢܐ
    {
	Py_INCREF(Py_None);
	ret = Py_None;
    }

    ܦܢܐ ret;
}

    ساكن PyObject *
VimEval(PyObject *self UNUSED, PyObject *args)
{
    char_u	*expr;
    typval_T	*our_tv;
    PyObject	*string;
    PyObject	*todecref;
    PyObject	*ret;
    PyObject	*lookup_dict;

    ܐܢ (!PyArg_ParseTuple(args, "O", &string))
	ܦܢܐ عدم;

    ܐܢ (!(expr = StringToChars(string, &todecref)))
	ܦܢܐ عدم;

    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    VimTryStart();
    our_tv = eval_expr(expr, عدم);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    Py_XDECREF(todecref);

    ܐܢ (VimTryEnd())
	ܦܢܐ عدم;

    ܐܢ (our_tv == عدم)
    {
	PyErr_SET_VIM(N_("invalid expression"));
	ܦܢܐ عدم;
    }

    // Convert the Vim type into a Python type.  Create a dictionary that's
    // used to check for recursive loops.
    ܐܢ (!(lookup_dict = PyDict_New()))
	ret = عدم;
    ܐܚܪܢܐ
    {
	ret = VimToPython(our_tv, 1, lookup_dict);
	Py_DECREF(lookup_dict);
    }


    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    free_tv(our_tv);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    ܦܢܐ ret;
}

ساكن PyObject *ConvertToPyObject(typval_T *);

    ساكن PyObject *
VimEvalPy(PyObject *self UNUSED, PyObject *string)
{
    typval_T	*our_tv;
    PyObject	*ret;
    char_u	*expr;
    PyObject	*todecref;

    ܐܢ (!(expr = StringToChars(string, &todecref)))
	ܦܢܐ عدم;

    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    VimTryStart();
    our_tv = eval_expr(expr, عدم);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    Py_XDECREF(todecref);

    ܐܢ (VimTryEnd())
	ܦܢܐ عدم;

    ܐܢ (our_tv == عدم)
    {
	PyErr_SET_VIM(N_("invalid expression"));
	ܦܢܐ عدم;
    }

    ret = ConvertToPyObject(our_tv);
    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    free_tv(our_tv);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    ܦܢܐ ret;
}

    ساكن PyObject *
VimStrwidth(PyObject *self UNUSED, PyObject *string)
{
    char_u	*str;
    PyObject	*todecref;
    صحيح		len;

    ܐܢ (!(str = StringToChars(string, &todecref)))
	ܦܢܐ عدم;

    len = mb_string2cells(str, (صحيح)STRLEN(str));

    Py_XDECREF(todecref);

    ܦܢܐ PyLong_FromLong(len);
}

    ساكن PyObject *
_VimChdir(PyObject *_chdir, PyObject *args, PyObject *kwargs)
{
    PyObject	*ret;
    PyObject	*newwd;
    PyObject	*todecref;
    char_u	*new_dir;

    ܐܢ (_chdir == عدم)
	ܦܢܐ عدم;
    ܐܢ (!(ret = PyObject_Call(_chdir, args, kwargs)))
	ܦܢܐ عدم;

    ܐܢ (!(newwd = PyObject_CallFunctionObjArgs(py_getcwd, عدم)))
    {
	Py_DECREF(ret);
	ܦܢܐ عدم;
    }

    ܐܢ (!(new_dir = StringToChars(newwd, &todecref)))
    {
	Py_DECREF(ret);
	Py_DECREF(newwd);
	ܦܢܐ عدم;
    }

    VimTryStart();

    ܐܢ (vim_chdir(new_dir))
    {
	Py_DECREF(ret);
	Py_DECREF(newwd);
	Py_XDECREF(todecref);

	ܐܢ (VimTryEnd())
	    ܦܢܐ عدم;

	PyErr_SET_VIM(N_("failed to change directory"));
	ܦܢܐ عدم;
    }

    Py_DECREF(newwd);
    Py_XDECREF(todecref);

    post_chdir(CDSCOPE_GLOBAL);

    ܐܢ (VimTryEnd())
    {
	Py_DECREF(ret);
	ܦܢܐ عدم;
    }

    ܦܢܐ ret;
}

    ساكن PyObject *
VimChdir(PyObject *self UNUSED, PyObject *args, PyObject *kwargs)
{
    ܦܢܐ _VimChdir(py_chdir, args, kwargs);
}

    ساكن PyObject *
VimFchdir(PyObject *self UNUSED, PyObject *args, PyObject *kwargs)
{
    ܦܢܐ _VimChdir(py_fchdir, args, kwargs);
}

تعريفنوع بنية {
    PyObject	*callable;
    PyObject	*result;
} map_rtp_data;

    ساكن فراغ
map_rtp_callback(char_u *path, فراغ *_data)
{
    فراغ	**data = (فراغ **) _data;
    PyObject	*pathObject;
    map_rtp_data	*mr_data = *((map_rtp_data **) data);

    ܐܢ (!(pathObject = PyString_FromString((حرف *)path)))
    {
	*data = عدم;
	ܦܢܐ;
    }

    mr_data->result = PyObject_CallFunctionObjArgs(mr_data->callable,
						   pathObject, عدم);

    Py_DECREF(pathObject);

    ܐܢ (!mr_data->result || mr_data->result != Py_None)
	*data = عدم;
    ܐܚܪܢܐ
    {
	Py_DECREF(mr_data->result);
	mr_data->result = عدم;
    }
}

    ساكن PyObject *
VimForeachRTP(PyObject *self UNUSED, PyObject *callable)
{
    map_rtp_data	data;

    data.callable = callable;
    data.result = عدم;

    do_in_runtimepath(عدم, 0, &map_rtp_callback, &data);

    ܐܢ (data.result == عدم)
    {
	ܐܢ (PyErr_Occurred())
	    ܦܢܐ عدم;
	ܐܚܪܢܐ
	{
	    Py_INCREF(Py_None);
	    ܦܢܐ Py_None;
	}
    }
    ܦܢܐ data.result;
}

/*
 * _vim_runtimepath_ special path implementation.
 */

    ساكن فراغ
map_finder_callback(char_u *path, فراغ *_data)
{
    فراغ	**data = (فراغ **) _data;
    PyObject	*list = *((PyObject **) data);
    PyObject	*pathObject1, *pathObject2;
    حرف	*pathbuf;
    size_t	pathlen;

    pathlen = STRLEN(path);

#ܐܢ PY_MAJOR_VERSION < 3
# define PY_MAIN_DIR_STRING "python2"
#ܐܚܪܢܐ
# define PY_MAIN_DIR_STRING "python3"
#endif
#define PY_ALTERNATE_DIR_STRING "pythonx"

#define PYTHONX_STRING_LENGTH 7 // STRLEN("pythonx")
    ܐܢ (!(pathbuf = PyMem_New(حرف,
		    pathlen + STRLEN(PATHSEPSTR) + PYTHONX_STRING_LENGTH + 1)))
    {
	PyErr_NoMemory();
	*data = عدم;
	ܦܢܐ;
    }

    mch_memmove(pathbuf, path, pathlen + 1);
    add_pathsep((char_u *) pathbuf);

    pathlen = STRLEN(pathbuf);
    mch_memmove(pathbuf + pathlen, PY_MAIN_DIR_STRING,
	    PYTHONX_STRING_LENGTH + 1);

    ܐܢ (!(pathObject1 = PyString_FromString(pathbuf)))
    {
	*data = عدم;
	PyMem_Free(pathbuf);
	ܦܢܐ;
    }

    mch_memmove(pathbuf + pathlen, PY_ALTERNATE_DIR_STRING,
	    PYTHONX_STRING_LENGTH + 1);

    ܐܢ (!(pathObject2 = PyString_FromString(pathbuf)))
    {
	Py_DECREF(pathObject1);
	PyMem_Free(pathbuf);
	*data = عدم;
	ܦܢܐ;
    }

    PyMem_Free(pathbuf);

    ܐܢ (PyList_Append(list, pathObject1)
	    || PyList_Append(list, pathObject2))
	*data = عدم;

    Py_DECREF(pathObject1);
    Py_DECREF(pathObject2);
}

    ساكن PyObject *
Vim_GetPaths(PyObject *self UNUSED, PyObject *args UNUSED)
{
    PyObject	*ret;

    ܐܢ (!(ret = PyList_New(0)))
	ܦܢܐ عدم;

    do_in_runtimepath(عدم, 0, &map_finder_callback, ret);

    ܐܢ (PyErr_Occurred())
    {
	Py_DECREF(ret);
	ܦܢܐ عدم;
    }

    ܦܢܐ ret;
}

#ܐܢ PY_VERSION_HEX >= 0x030700f0
    ساكن PyObject *
FinderFindSpec(PyObject *self, PyObject *args)
{
    حرف	*fullname;
    PyObject	*paths;
    PyObject	*target = Py_None;
    PyObject	*spec;

    ܐܢ (!PyArg_ParseTuple(args, "s|O", &fullname, &target))
	ܦܢܐ عدم;

    ܐܢ (!(paths = Vim_GetPaths(self, عدم)))
	ܦܢܐ عدم;

    spec = PyObject_CallFunction(py_find_spec, "sOO", fullname, paths, target);

    Py_DECREF(paths);

    ܐܢ (!spec)
    {
	ܐܢ (PyErr_Occurred())
	    ܦܢܐ عدم;

	Py_INCREF(Py_None);
	ܦܢܐ Py_None;
    }

    ܦܢܐ spec;
}

    ساكن PyObject *
FinderFindModule(PyObject* self UNUSED, PyObject* args UNUSED)
{
    // Apparently returning None works.
    Py_INCREF(Py_None);
    ܦܢܐ Py_None;
}
#ܐܚܪܢܐ
    ساكن PyObject *
call_load_module(حرف *name, صحيح len, PyObject *find_module_result)
{
    PyObject	*fd, *pathname, *description;

    ܐܢ (!PyTuple_Check(find_module_result))
    {
	PyErr_FORMAT_TYPE(
		N_("expected 3-tuple as imp.find_module() result, but got %s"),
		find_module_result);
	ܦܢܐ عدم;
    }
    ܐܢ (PyTuple_GET_SIZE(find_module_result) != 3)
    {
	PyErr_FORMAT(PyExc_TypeError,
		N_("expected 3-tuple as imp.find_module() result, but got "
		   "tuple of size %d"),
		(صحيح) PyTuple_GET_SIZE(find_module_result));
	ܦܢܐ عدم;
    }

    ܐܢ (!(fd = PyTuple_GET_ITEM(find_module_result, 0))
	    || !(pathname = PyTuple_GET_ITEM(find_module_result, 1))
	    || !(description = PyTuple_GET_ITEM(find_module_result, 2)))
    {
	PyErr_SET_STRING(PyExc_RuntimeError,
		N_("internal error: imp.find_module returned tuple with NULL"));
	ܦܢܐ عدم;
    }

    ܦܢܐ PyObject_CallFunction(py_load_module,
	    "s#OOO", name, len, fd, pathname, description);
}

    ساكن PyObject *
find_module(حرف *fullname, حرف *tail, PyObject *new_path)
{
    PyObject	*find_module_result;
    PyObject	*module;
    حرف	*dot;

    ܐܢ ((dot = (حرف *)vim_strchr((char_u *) tail, '.')))
    {
	/*
	 * There is a dot in the name: call find_module recursively without the
	 * first component
	 */
	PyObject	*newest_path;
	صحيح		partlen = (صحيح) (dot - 1 - tail);

	ܐܢ (!(find_module_result = PyObject_CallFunction(py_find_module,
			"s#O", tail, partlen, new_path)))
	{
	    ܐܢ (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_ImportError))
		PyErr_Clear();
	    ܦܢܐ عدم;
	}

	ܐܢ (!(module = call_load_module(
			fullname,
			((صحيح) (tail - fullname)) + partlen,
			find_module_result)))
	{
	    Py_DECREF(find_module_result);
	    ܦܢܐ عدم;
	}

	Py_DECREF(find_module_result);

	ܐܢ (!(newest_path = PyObject_GetAttrString(module, "__path__")))
	{
	    Py_DECREF(module);
	    ܦܢܐ عدم;
	}

	Py_DECREF(module);

	find_module_result = find_module(fullname, dot + 1, newest_path);

	Py_DECREF(newest_path);

	ܦܢܐ find_module_result;
    }
    ܐܚܪܢܐ
    {
	ܐܢ (!(find_module_result = PyObject_CallFunction(py_find_module,
			"sO", tail, new_path)))
	{
	    ܐܢ (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_ImportError))
		PyErr_Clear();
	    ܦܢܐ عدم;
	}

	ܦܢܐ find_module_result;
    }
}

    ساكن PyObject *
FinderFindModule(PyObject *self, PyObject *args)
{
    حرف	*fullname;
    PyObject	*result;
    PyObject	*new_path;
    LoaderObject	*loader;

    ܐܢ (!PyArg_ParseTuple(args, "s", &fullname))
	ܦܢܐ عدم;

    ܐܢ (!(new_path = Vim_GetPaths(self, عدم)))
	ܦܢܐ عدم;

    result = find_module(fullname, fullname, new_path);

    Py_DECREF(new_path);

    ܐܢ (!result)
    {
	ܐܢ (PyErr_Occurred())
	    ܦܢܐ عدم;

	Py_INCREF(Py_None);
	ܦܢܐ Py_None;
    }

    ܐܢ (!(fullname = (حرف *)vim_strsave((char_u *)fullname)))
    {
	Py_DECREF(result);
	PyErr_NoMemory();
	ܦܢܐ عدم;
    }

    ܐܢ (!(loader = PyObject_NEW(LoaderObject, LoaderTypePtr)))
    {
	vim_free(fullname);
	Py_DECREF(result);
	ܦܢܐ عدم;
    }

    loader->fullname = fullname;
    loader->result = result;

    ܦܢܐ (PyObject *) loader;
}
#endif

    ساكن PyObject *
VimPathHook(PyObject *self UNUSED, PyObject *args)
{
    حرف	*path;

    ܐܢ (PyArg_ParseTuple(args, "s", &path)
	    && STRCMP(path, vim_special_path) == 0)
    {
	Py_INCREF(vim_module);
	ܦܢܐ vim_module;
    }

    PyErr_Clear();
    PyErr_SetNone(PyExc_ImportError);
    ܦܢܐ عدم;
}

/*
 * Vim module - Definitions
 */

ساكن بنية PyMethodDef VimMethods[] = {
    // name,	    function,			calling,			documentation
    {"command",	    VimCommand,			METH_O,				"Execute a Vim ex-mode command" },
    {"eval",	    VimEval,			METH_VARARGS,			"Evaluate an expression using Vim evaluator" },
    {"bindeval",    VimEvalPy,			METH_O,				"Like eval(), but returns objects attached to Vim ones"},
    {"strwidth",    VimStrwidth,		METH_O,				"Screen string width, counts <Tab> as having width 1"},
    {"chdir",	    (PyCFunction)(فراغ *)VimChdir,	METH_VARARGS|METH_KEYWORDS,	"Change directory"},
    {"fchdir",	    (PyCFunction)(فراغ *)VimFchdir,	METH_VARARGS|METH_KEYWORDS,	"Change directory"},
    {"foreach_rtp", VimForeachRTP,		METH_O,				"Call given callable for each path in &rtp"},
#ܐܢ PY_VERSION_HEX >= 0x030700f0
    {"find_spec",   FinderFindSpec,		METH_VARARGS,			"Internal use only, returns spec object for any input it receives"},
#endif
    {"find_module", FinderFindModule,		METH_VARARGS,			"Internal use only, returns loader object for any input it receives"},
    {"path_hook",   VimPathHook,		METH_VARARGS,			"Hook function to install in sys.path_hooks"},
    {"_get_paths",  (PyCFunction)Vim_GetPaths,	METH_NOARGS,			"Get &rtp-based additions to sys.path"},
    { عدم,	    عدم,			0,				عدم}
};

/*
 * Generic iterator object
 */

DEFINE_PY_TYPE_OBJECT(IterType);

تعريفنوع PyObject *(*nextfun)(فراغ **);
تعريفنوع فراغ (*destructorfun)(فراغ *);
تعريفنوع صحيح (*traversefun)(فراغ *, visitproc, فراغ *);
تعريفنوع صحيح (*clearfun)(فراغ **);

// Main purpose of this object is removing the need for do python
// initialization (i.e. PyType_Ready and setting type attributes) for a big
// bunch of objects.
تعريفنوع بنية
{
    PyObject_HEAD
    فراغ *cur;
    nextfun next;
    destructorfun destruct;
    traversefun traverse;
    clearfun clear;
    PyObject *iter_object;
} IterObject;

    ساكن PyObject *
IterNew(فراغ *start, destructorfun destruct, nextfun next, traversefun traverse,
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

    ܐܢ (iter_object)
	Py_INCREF(iter_object);

    ܦܢܐ (PyObject *)(self);
}

    ساكن فراغ
IterDestructor(PyObject *self_obj)
{
    IterObject *self = (IterObject*)self_obj;
    ܐܢ (self->iter_object)
	Py_DECREF(self->iter_object);
    PyObject_GC_UnTrack((فراغ *)(self));
    self->destruct(self->cur);
    PyObject_GC_Del((فراغ *)(self));
}

    ساكن صحيح
IterTraverse(PyObject *self_obj, visitproc visit, فراغ *arg)
{
    IterObject *self = (IterObject*)self_obj;
    ܐܢ (self->traverse != عدم)
	ܦܢܐ self->traverse(self->cur, visit, arg);
    ܐܚܪܢܐ
	ܦܢܐ 0;
}

// Mac OSX defines clear() somewhere.
#ifdef clear
# undef clear
#endif

    ساكن صحيح
IterClear(PyObject *self_obj)
{
    IterObject *self = (IterObject*)self_obj;
    ܐܢ (self->clear != عدم)
	ܦܢܐ self->clear(&self->cur);
    ܐܚܪܢܐ
	ܦܢܐ 0;
}

    ساكن PyObject *
IterNext(PyObject *self_obj)
{
    IterObject *self = (IterObject*)self_obj;
    ܦܢܐ self->next(&self->cur);
}

    ساكن PyObject *
IterIter(PyObject *self)
{
    Py_INCREF(self);
    ܦܢܐ self;
}

تعريفنوع بنية pylinkedlist_S {
    بنية pylinkedlist_S	*pll_next;
    بنية pylinkedlist_S	*pll_prev;
    PyObject			*pll_obj;
} pylinkedlist_T;

ساكن pylinkedlist_T *lastdict = عدم;
ساكن pylinkedlist_T *lastlist = عدم;
ساكن pylinkedlist_T *lasttuple = عدم;
ساكن pylinkedlist_T *lastfunc = عدم;

    ساكن فراغ
pyll_remove(pylinkedlist_T *ref, pylinkedlist_T **last)
{
    ܐܢ (ref->pll_prev == عدم)
    {
	ܐܢ (ref->pll_next == عدم)
	{
	    *last = عدم;
	    ܦܢܐ;
	}
    }
    ܐܚܪܢܐ
	ref->pll_prev->pll_next = ref->pll_next;

    ܐܢ (ref->pll_next == عدم)
	*last = ref->pll_prev;
    ܐܚܪܢܐ
	ref->pll_next->pll_prev = ref->pll_prev;
}

    ساكن فراغ
pyll_add(PyObject *self, pylinkedlist_T *ref, pylinkedlist_T **last)
{
    ܐܢ (*last == عدم)
	ref->pll_prev = عدم;
    ܐܚܪܢܐ
    {
	(*last)->pll_next = ref;
	ref->pll_prev = *last;
    }
    ref->pll_next = عدم;
    ref->pll_obj = self;
    *last = ref;
}

DEFINE_PY_TYPE_OBJECT(DictionaryType);

تعريفنوع بنية
{
    PyObject_HEAD
    dict_T	*dict;
    pylinkedlist_T	ref;
} DictionaryObject;

ساكن PyObject *DictionaryUpdate(DictionaryObject *, PyObject *, PyObject *);

#define NEW_DICTIONARY(dict) DictionaryNew(DictionaryTypePtr, dict)

    ساكن PyObject *
DictionaryNew(PyTypeObject *subtype, dict_T *dict)
{
    DictionaryObject	*self;

    self = (DictionaryObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    ܐܢ (self == عدم)
	ܦܢܐ عدم;
    self->dict = dict;
    ++dict->dv_refcount;

    pyll_add((PyObject *)(self), &self->ref, &lastdict);

    ܦܢܐ (PyObject *)(self);
}

    ساكن dict_T *
py_dict_alloc(فراغ)
{
    dict_T	*ret;

    ܐܢ (!(ret = dict_alloc()))
    {
	PyErr_NoMemory();
	ܦܢܐ عدم;
    }
    ++ret->dv_refcount;

    ܦܢܐ ret;
}

    ساكن PyObject *
DictionaryConstructor(PyTypeObject *subtype, PyObject *args, PyObject *kwargs)
{
    DictionaryObject	*self;
    dict_T	*dict;

    ܐܢ (!(dict = py_dict_alloc()))
	ܦܢܐ عدم;

    self = (DictionaryObject *) DictionaryNew(subtype, dict);

    --dict->dv_refcount;

    ܐܢ (kwargs || PyTuple_Size(args))
    {
	PyObject	*tmp;
	ܐܢ (!(tmp = DictionaryUpdate(self, args, kwargs)))
	{
	    Py_DECREF(self);
	    ܦܢܐ عدم;
	}

	Py_DECREF(tmp);
    }

    ܦܢܐ (PyObject *)(self);
}

    ساكن فراغ
DictionaryDestructor(PyObject *self_obj)
{
    DictionaryObject *self = (DictionaryObject*)self_obj;
    pyll_remove(&self->ref, &lastdict);
    dict_unref(self->dict);

    DESTRUCTOR_FINISH(self);
}

ساكن حرف *DictionaryAttrs[] = {
    "locked", "scope",
    عدم
};

    ساكن PyObject *
DictionaryDir(PyObject *self, PyObject *args UNUSED)
{
    ܦܢܐ ObjectDir(self, DictionaryAttrs);
}

    ساكن صحيح
DictionarySetattr(PyObject *self_obj, حرف *name, PyObject *valObject)
{
    DictionaryObject *self = (DictionaryObject*)self_obj;
    ܐܢ (valObject == عدم)
    {
	PyErr_SET_STRING(PyExc_AttributeError,
		N_("cannot delete vim.Dictionary attributes"));
	ܦܢܐ -1;
    }

    ܐܢ (strcmp(name, "locked") == 0)
    {
	ܐܢ (self->dict->dv_lock == VAR_FIXED)
	{
	    PyErr_SET_STRING(PyExc_TypeError,
		    N_("cannot modify fixed dictionary"));
	    ܦܢܐ -1;
	}
	ܐܚܪܢܐ
	{
	    صحيح		istrue = PyObject_IsTrue(valObject);
	    ܐܢ (istrue == -1)
		ܦܢܐ -1;
	    ܐܚܪܢܐ ܐܢ (istrue)
		self->dict->dv_lock = VAR_LOCKED;
	    ܐܚܪܢܐ
		self->dict->dv_lock = 0;
	}
	ܦܢܐ 0;
    }
    ܐܚܪܢܐ
    {
	PyErr_FORMAT(PyExc_AttributeError, N_("cannot set attribute %s"), name);
	ܦܢܐ -1;
    }
}

    ساكن PyInt
DictionaryLength(DictionaryObject *self)
{
    ܦܢܐ ((PyInt) (self->dict->dv_hashtab.ht_used));
}

#define DICT_FLAG_HAS_DEFAULT	0x01
#define DICT_FLAG_POP		0x02
#define DICT_FLAG_NONE_DEFAULT	0x04
#define DICT_FLAG_RETURN_BOOL	0x08 // Incompatible with DICT_FLAG_POP
#define DICT_FLAG_RETURN_PAIR	0x10

    ساكن PyObject *
_DictionaryItem(DictionaryObject *self, PyObject *args, صحيح flags)
{
    PyObject	*keyObject;
    PyObject	*defObject = ((flags & DICT_FLAG_NONE_DEFAULT)? Py_None : عدم);
    PyObject	*ret;
    char_u	*key;
    dictitem_T	*di;
    dict_T	*dict = self->dict;
    hashitem_T	*hi;
    PyObject	*todecref;

    ܐܢ (flags & DICT_FLAG_HAS_DEFAULT)
    {
	ܐܢ (!PyArg_ParseTuple(args, "O|O", &keyObject, &defObject))
	    ܦܢܐ عدم;
    }
    ܐܚܪܢܐ
	keyObject = args;

    ܐܢ (flags & DICT_FLAG_RETURN_BOOL)
	defObject = Py_False;

    ܐܢ (!(key = StringToChars(keyObject, &todecref)))
	ܦܢܐ عدم;

    ܐܢ (*key == NUL)
    {
	RAISE_NO_EMPTY_KEYS;
	Py_XDECREF(todecref);
	ܦܢܐ عدم;
    }

    hi = hash_find(&dict->dv_hashtab, key);

    Py_XDECREF(todecref);

    ܐܢ (HASHITEM_EMPTY(hi))
    {
	ܐܢ (defObject)
	{
	    Py_INCREF(defObject);
	    ܦܢܐ defObject;
	}
	ܐܚܪܢܐ
	{
	    PyErr_SetObject(PyExc_KeyError, keyObject);
	    ܦܢܐ عدم;
	}
    }
    ܐܚܪܢܐ ܐܢ (flags & DICT_FLAG_RETURN_BOOL)
    {
	ret = Py_True;
	Py_INCREF(ret);
	ܦܢܐ ret;
    }

    di = dict_lookup(hi);

    ܐܢ (!(ret = ConvertToPyObject(&di->di_tv)))
	ܦܢܐ عدم;

    ܐܢ (flags & DICT_FLAG_POP)
    {
	ܐܢ (dict->dv_lock)
	{
	    RAISE_LOCKED_DICTIONARY;
	    Py_DECREF(ret);
	    ܦܢܐ عدم;
	}

	hash_remove(&dict->dv_hashtab, hi, "Python remove variable");
	dictitem_free(di);
    }

    ܦܢܐ ret;
}

    ساكن PyObject *
DictionaryItem(DictionaryObject *self, PyObject *keyObject)
{
    ܦܢܐ _DictionaryItem(self, keyObject, 0);
}

    ساكن صحيح
DictionaryContains(DictionaryObject *self, PyObject *keyObject)
{
    PyObject	*rObj = _DictionaryItem(self, keyObject, DICT_FLAG_RETURN_BOOL);
    صحيح		ret;

    ܐܢ (rObj == عدم)
	ܦܢܐ -1;

    ret = (rObj == Py_True);

    Py_DECREF(rObj);

    ܦܢܐ ret;
}

تعريفنوع بنية
{
    صحيح		dii_changed;
    hashtab_T	*dii_ht;
    hashitem_T	*dii_hi;
    long_u	dii_todo;
} dictiterinfo_T;

    ساكن PyObject *
DictionaryIterNext(فراغ **arg)
{
    PyObject	*ret;
    dictiterinfo_T **dii = (dictiterinfo_T**)arg;

    ܐܢ (!(*dii)->dii_todo)
	ܦܢܐ عدم;

    ܐܢ ((*dii)->dii_ht->ht_changed != (*dii)->dii_changed)
    {
	PyErr_SET_STRING(PyExc_RuntimeError,
		N_("hashtab changed during iteration"));
	ܦܢܐ عدم;
    }

    ܟܡܐ (((*dii)->dii_todo) && HASHITEM_EMPTY((*dii)->dii_hi))
	++((*dii)->dii_hi);

    --((*dii)->dii_todo);

    ܐܢ (!(ret = PyBytes_FromString((حرف *)(*dii)->dii_hi->hi_key)))
	ܦܢܐ عدم;

    ܦܢܐ ret;
}

    ساكن PyObject *
DictionaryIter(PyObject *self_obj)
{
    DictionaryObject	*self = (DictionaryObject*)self_obj;
    dictiterinfo_T	*dii;
    hashtab_T		*ht;

    ܐܢ (!(dii = PyMem_New(dictiterinfo_T, 1)))
    {
	PyErr_NoMemory();
	ܦܢܐ عدم;
    }

    ht = &self->dict->dv_hashtab;
    dii->dii_changed = ht->ht_changed;
    dii->dii_ht = ht;
    dii->dii_hi = ht->ht_array;
    dii->dii_todo = ht->ht_used;

    ܦܢܐ IterNew(dii,
	    PyMem_Free, DictionaryIterNext,
	    عدم, عدم, (PyObject *)self);
}

    ساكن صحيح
DictionaryAssItem(
	DictionaryObject *self, PyObject *keyObject, PyObject *valObject)
{
    char_u	*key;
    typval_T	tv;
    dict_T	*dict = self->dict;
    dictitem_T	*di;
    PyObject	*todecref;

    ܐܢ (dict->dv_lock)
    {
	RAISE_LOCKED_DICTIONARY;
	ܦܢܐ -1;
    }

    ܐܢ (!(key = StringToChars(keyObject, &todecref)))
	ܦܢܐ -1;

    ܐܢ (*key == NUL)
    {
	RAISE_NO_EMPTY_KEYS;
	Py_XDECREF(todecref);
	ܦܢܐ -1;
    }

    di = dict_find(dict, key, -1);

    ܐܢ (valObject == عدم)
    {
	hashitem_T	*hi;

	ܐܢ (di == عدم)
	{
	    Py_XDECREF(todecref);
	    PyErr_SetObject(PyExc_KeyError, keyObject);
	    ܦܢܐ -1;
	}
	hi = hash_find(&dict->dv_hashtab, di->di_key);
	hash_remove(&dict->dv_hashtab, hi, "Python remove item");
	dictitem_free(di);
	Py_XDECREF(todecref);
	ܦܢܐ 0;
    }

    ܐܢ (ConvertFromPyObject(valObject, &tv) == -1)
    {
	Py_XDECREF(todecref);
	ܦܢܐ -1;
    }

    ܐܢ (di == عدم)
    {
	ܐܢ (!(di = dictitem_alloc(key)))
	{
	    Py_XDECREF(todecref);
	    PyErr_NoMemory();
	    ܦܢܐ -1;
	}
	di->di_tv.v_type = VAR_UNKNOWN;

	ܐܢ (dict_add(dict, di) == FAIL)
	{
	    dictitem_free(di);
	    RAISE_KEY_ADD_FAIL(key);
	    Py_XDECREF(todecref);
	    ܦܢܐ -1;
	}
    }
    ܐܚܪܢܐ
	clear_tv(&di->di_tv);

    Py_XDECREF(todecref);

    copy_tv(&tv, &di->di_tv);
    clear_tv(&tv);
    ܦܢܐ 0;
}

تعريفنوع PyObject *(*hi_to_py)(hashitem_T *);

    ساكن PyObject *
DictionaryListObjects(DictionaryObject *self, hi_to_py hiconvert)
{
    dict_T	*dict = self->dict;
    long_u	todo = dict->dv_hashtab.ht_used;
    Py_ssize_t	i = 0;
    PyObject	*ret;
    hashitem_T	*hi;
    PyObject	*newObj;

    ret = PyList_New(todo);
    لكل (hi = dict->dv_hashtab.ht_array; todo > 0; ++hi)
    {
	ܐܢ (!HASHITEM_EMPTY(hi))
	{
	    ܐܢ (!(newObj = hiconvert(hi)))
	    {
		Py_DECREF(ret);
		ܦܢܐ عدم;
	    }
	    PyList_SET_ITEM(ret, i, newObj);
	    --todo;
	    ++i;
	}
    }
    ܦܢܐ ret;
}

    ساكن PyObject *
dict_key(hashitem_T *hi)
{
    ܦܢܐ PyBytes_FromString((حرف *)(hi->hi_key));
}

    ساكن PyObject *
DictionaryListKeys(DictionaryObject *self, PyObject *args UNUSED)
{
    ܦܢܐ DictionaryListObjects(self, dict_key);
}

    ساكن PyObject *
dict_val(hashitem_T *hi)
{
    dictitem_T	*di;

    di = dict_lookup(hi);
    ܦܢܐ ConvertToPyObject(&di->di_tv);
}

    ساكن PyObject *
DictionaryListValues(DictionaryObject *self, PyObject *args UNUSED)
{
    ܦܢܐ DictionaryListObjects(self, dict_val);
}

    ساكن PyObject *
dict_item(hashitem_T *hi)
{
    PyObject	*keyObject;
    PyObject	*valObject;
    PyObject	*ret;

    ܐܢ (!(keyObject = dict_key(hi)))
	ܦܢܐ عدم;

    ܐܢ (!(valObject = dict_val(hi)))
    {
	Py_DECREF(keyObject);
	ܦܢܐ عدم;
    }

    ret = Py_BuildValue("(OO)", keyObject, valObject);

    Py_DECREF(keyObject);
    Py_DECREF(valObject);

    ܦܢܐ ret;
}

    ساكن PyObject *
DictionaryListItems(DictionaryObject *self, PyObject *args UNUSED)
{
    ܦܢܐ DictionaryListObjects(self, dict_item);
}

    ساكن PyObject *
DictionaryUpdate(DictionaryObject *self, PyObject *args, PyObject *kwargs)
{
    dict_T	*dict = self->dict;

    ܐܢ (dict->dv_lock)
    {
	RAISE_LOCKED_DICTIONARY;
	ܦܢܐ عدم;
    }

    ܐܢ (kwargs)
    {
	typval_T	tv;

	ܐܢ (ConvertFromPyMapping(kwargs, &tv) == -1)
	    ܦܢܐ عدم;

	VimTryStart();
	dict_extend(self->dict, tv.vval.v_dict, (char_u *) "force", عدم);
	clear_tv(&tv);
	ܐܢ (VimTryEnd())
	    ܦܢܐ عدم;
    }
    ܐܚܪܢܐ
    {
	PyObject	*obj = عدم;

	ܐܢ (!PyArg_ParseTuple(args, "|O", &obj))
	    ܦܢܐ عدم;

	ܐܢ (obj == عدم)
	{
	    Py_INCREF(Py_None);
	    ܦܢܐ Py_None;
	}

	ܐܢ (PyObject_HasAttrString(obj, "keys"))
	    ܦܢܐ DictionaryUpdate(self, عدم, obj);
	ܐܚܪܢܐ
	{
	    PyObject	*iterator;
	    PyObject	*item;

	    ܐܢ (!(iterator = PyObject_GetIter(obj)))
		ܦܢܐ عدم;

	    ܟܡܐ ((item = PyIter_Next(iterator)))
	    {
		PyObject	*fast;
		PyObject	*keyObject;
		PyObject	*valObject;
		PyObject	*todecref;
		char_u		*key;
		dictitem_T	*di;
		hashitem_T	*hi;

		ܐܢ (!(fast = PySequence_Fast(item, "")))
		{
		    Py_DECREF(iterator);
		    Py_DECREF(item);
		    ܦܢܐ عدم;
		}

		Py_DECREF(item);

		ܐܢ (PySequence_Size(fast) != 2)
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    PyErr_FORMAT(PyExc_ValueError,
			    N_("expected sequence element of size 2, "
			    "but got sequence of size %d"),
			    (صحيح) PySequence_Size(fast));
		    ܦܢܐ عدم;
		}

		keyObject = PySequence_GetItem(fast, 0);

		ܐܢ (!(key = StringToChars(keyObject, &todecref)))
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    ܦܢܐ عدم;
		}

		di = dictitem_alloc(key);

		Py_XDECREF(todecref);

		ܐܢ (di == عدم)
		{
		    Py_DECREF(fast);
		    Py_DECREF(iterator);
		    PyErr_NoMemory();
		    ܦܢܐ عدم;
		}
		di->di_tv.v_type = VAR_UNKNOWN;

		valObject = PySequence_GetItem(fast, 1);

		ܐܢ (ConvertFromPyObject(valObject, &di->di_tv) == -1)
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    dictitem_free(di);
		    ܦܢܐ عدم;
		}

		Py_DECREF(fast);

		hi = hash_find(&dict->dv_hashtab, di->di_key);
		ܐܢ (!HASHITEM_EMPTY(hi) || dict_add(dict, di) == FAIL)
		{
		    RAISE_KEY_ADD_FAIL(di->di_key);
		    Py_DECREF(iterator);
		    dictitem_free(di);
		    ܦܢܐ عدم;
		}
	    }

	    Py_DECREF(iterator);

	    // Iterator may have finished due to an exception
	    ܐܢ (PyErr_Occurred())
		ܦܢܐ عدم;
	}
    }
    Py_INCREF(Py_None);
    ܦܢܐ Py_None;
}

    ساكن PyObject *
DictionaryGet(DictionaryObject *self, PyObject *args)
{
    ܦܢܐ _DictionaryItem(self, args,
			    DICT_FLAG_HAS_DEFAULT|DICT_FLAG_NONE_DEFAULT);
}

    ساكن PyObject *
DictionaryPop(DictionaryObject *self, PyObject *args)
{
    ܦܢܐ _DictionaryItem(self, args, DICT_FLAG_HAS_DEFAULT|DICT_FLAG_POP);
}

    ساكن PyObject *
DictionaryPopItem(DictionaryObject *self, PyObject *args UNUSED)
{
    hashitem_T	*hi;
    PyObject	*ret;
    PyObject	*valObject;
    dictitem_T	*di;

    ܐܢ (self->dict->dv_hashtab.ht_used == 0)
    {
	PyErr_SetNone(PyExc_KeyError);
	ܦܢܐ عدم;
    }

    hi = self->dict->dv_hashtab.ht_array;
    ܟܡܐ (HASHITEM_EMPTY(hi))
	++hi;

    di = dict_lookup(hi);

    ܐܢ (!(valObject = ConvertToPyObject(&di->di_tv)))
	ܦܢܐ عدم;

    ܐܢ (!(ret = Py_BuildValue("(" Py_bytes_fmt "O)", hi->hi_key, valObject)))
    {
	Py_DECREF(valObject);
	ܦܢܐ عدم;
    }

    hash_remove(&self->dict->dv_hashtab, hi, "Python pop item");
    dictitem_free(di);

    ܦܢܐ ret;
}

    ساكن PyObject *
DictionaryHasKey(DictionaryObject *self, PyObject *keyObject)
{
    ܦܢܐ _DictionaryItem(self, keyObject, DICT_FLAG_RETURN_BOOL);
}

ساكن PySequenceMethods DictionaryAsSeq = {
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

ساكن PyMappingMethods DictionaryAsMapping = {
    (lenfunc)       DictionaryLength,
    (binaryfunc)    DictionaryItem,
    (objobjargproc) DictionaryAssItem,
};

ساكن بنية PyMethodDef DictionaryMethods[] = {
    {"keys",	(PyCFunction)DictionaryListKeys,	METH_NOARGS,	""},
    {"values",	(PyCFunction)DictionaryListValues,	METH_NOARGS,	""},
    {"items",	(PyCFunction)DictionaryListItems,	METH_NOARGS,	""},
    {"update",	(PyCFunction)(فراغ *)DictionaryUpdate,		METH_VARARGS|METH_KEYWORDS, ""},
    {"get",	(PyCFunction)DictionaryGet,		METH_VARARGS,	""},
    {"pop",	(PyCFunction)DictionaryPop,		METH_VARARGS,	""},
    {"popitem",	(PyCFunction)DictionaryPopItem,		METH_NOARGS,	""},
    {"has_key",	(PyCFunction)DictionaryHasKey,		METH_O,		""},
    {"__dir__",	(PyCFunction)DictionaryDir,		METH_NOARGS,	""},
    { عدم,	عدم,					0,		عدم}
};

DEFINE_PY_TYPE_OBJECT(ListType);

تعريفنوع بنية
{
    PyObject_HEAD
    list_T	*list;
    pylinkedlist_T	ref;
} ListObject;

#define NEW_LIST(list) ListNew(ListTypePtr, list)

    ساكن PyObject *
ListNew(PyTypeObject *subtype, list_T *list)
{
    ListObject	*self;

    ܐܢ (list == عدم)
	ܦܢܐ عدم;

    self = (ListObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    ܐܢ (self == عدم)
	ܦܢܐ عدم;
    self->list = list;
    ++list->lv_refcount;
    CHECK_LIST_MATERIALIZE(list);

    pyll_add((PyObject *)(self), &self->ref, &lastlist);

    ܦܢܐ (PyObject *)(self);
}

    ساكن list_T *
py_list_alloc(فراغ)
{
    list_T	*ret;

    ܐܢ (!(ret = list_alloc()))
    {
	PyErr_NoMemory();
	ܦܢܐ عدم;
    }
    ++ret->lv_refcount;

    ܦܢܐ ret;
}

    ساكن صحيح
list_py_concat(list_T *l, PyObject *obj, PyObject *lookup_dict)
{
    PyObject	*iterator;
    PyObject	*item;
    listitem_T	*li;

    ܐܢ (!(iterator = PyObject_GetIter(obj)))
	ܦܢܐ -1;

    ܟܡܐ ((item = PyIter_Next(iterator)))
    {
	ܐܢ (!(li = listitem_alloc()))
	{
	    PyErr_NoMemory();
	    Py_DECREF(item);
	    Py_DECREF(iterator);
	    ܦܢܐ -1;
	}
	li->li_tv.v_lock = 0;
	li->li_tv.v_type = VAR_UNKNOWN;

	ܐܢ (_ConvertFromPyObject(item, &li->li_tv, lookup_dict) == -1)
	{
	    Py_DECREF(item);
	    Py_DECREF(iterator);
	    listitem_free(l, li);
	    ܦܢܐ -1;
	}

	Py_DECREF(item);

	list_append(l, li);
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    ܐܢ (PyErr_Occurred())
	ܦܢܐ -1;

    ܦܢܐ 0;
}

    ساكن PyObject *
ListConstructor(PyTypeObject *subtype, PyObject *args, PyObject *kwargs)
{
    list_T	*list;
    PyObject	*obj = عدم;

    ܐܢ (kwargs)
    {
	PyErr_SET_STRING(PyExc_TypeError,
		N_("list constructor does not accept keyword arguments"));
	ܦܢܐ عدم;
    }

    ܐܢ (!PyArg_ParseTuple(args, "|O", &obj))
	ܦܢܐ عدم;

    ܐܢ (!(list = py_list_alloc()))
	ܦܢܐ عدم;

    ܐܢ (obj)
    {
	PyObject	*lookup_dict;

	ܐܢ (!(lookup_dict = PyDict_New()))
	{
	    list_unref(list);
	    ܦܢܐ عدم;
	}

	ܐܢ (list_py_concat(list, obj, lookup_dict) == -1)
	{
	    Py_DECREF(lookup_dict);
	    list_unref(list);
	    ܦܢܐ عدم;
	}

	Py_DECREF(lookup_dict);
    }

    ܦܢܐ ListNew(subtype, list);
}

    ساكن فراغ
ListDestructor(PyObject *self_obj)
{
    ListObject *self = (ListObject*)self_obj;
    pyll_remove(&self->ref, &lastlist);
    list_unref(self->list);

    DESTRUCTOR_FINISH(self);
}

    ساكن PyInt
ListLength(ListObject *self)
{
    ܦܢܐ ((PyInt) (self->list->lv_len));
}

    ساكن PyObject *
ListIndex(ListObject *self, Py_ssize_t index)
{
    listitem_T	*li;

    ܐܢ (index >= ListLength(self))
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("list index out of range"));
	ܦܢܐ عدم;
    }
    li = list_find(self->list, (long) index);
    ܐܢ (li == عدم)
    {
	// No more suitable format specifications in python-2.3
	PyErr_VIM_FORMAT(N_("internal error: failed to get Vim list item %d"),
		(صحيح) index);
	ܦܢܐ عدم;
    }
    ܦܢܐ ConvertToPyObject(&li->li_tv);
}

    ساكن PyObject *
ListSlice(ListObject *self, Py_ssize_t first, Py_ssize_t step,
	  Py_ssize_t slicelen)
{
    PyInt	i;
    PyObject	*list;

    ܐܢ (step == 0)
    {
	PyErr_SET_STRING(PyExc_ValueError, N_("slice step cannot be zero"));
	ܦܢܐ عدم;
    }

    list = PyList_New(slicelen);
    ܐܢ (list == عدم)
	ܦܢܐ عدم;

    لكل (i = 0; i < slicelen; ++i)
    {
	PyObject	*item;

	item = ListIndex(self, first + i*step);
	ܐܢ (item == عدم)
	{
	    Py_DECREF(list);
	    ܦܢܐ عدم;
	}

	PyList_SET_ITEM(list, i, item);
    }

    ܦܢܐ list;
}

    ساكن PyObject *
ListItem(ListObject *self, PyObject* idx)
{
#ܐܢ PY_MAJOR_VERSION < 3
    ܐܢ (PyInt_Check(idx))
    {
	long _idx = PyInt_AsLong(idx);
	ܦܢܐ ListIndex(self, _idx);
    }
    ܐܚܪܢܐ
#endif
    ܐܢ (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	ܦܢܐ ListIndex(self, _idx);
    }
    ܐܚܪܢܐ ܐܢ (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	ܐܢ (PySlice_GetIndicesEx((PySliceObject_T *)idx, ListLength(self),
				 &start, &stop, &step, &slicelen) < 0)
	    ܦܢܐ عدم;
	ܦܢܐ ListSlice(self, start, step, slicelen);
    }
    ܐܚܪܢܐ
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	ܦܢܐ عدم;
    }
}

    ساكن فراغ
list_restore(Py_ssize_t numadded, Py_ssize_t numreplaced, Py_ssize_t slicelen,
	list_T *l, listitem_T **lis, listitem_T *lastaddedli)
{
    ܟܡܐ (numreplaced--)
    {
	list_insert(l, lis[numreplaced], lis[slicelen + numreplaced]);
	listitem_remove(l, lis[slicelen + numreplaced]);
    }
    ܟܡܐ (numadded--)
    {
	listitem_T	*next;

	next = lastaddedli->li_prev;
	listitem_remove(l, lastaddedli);
	lastaddedli = next;
    }
}

    ساكن صحيح
ListAssSlice(ListObject *self, Py_ssize_t first,
	     Py_ssize_t step, Py_ssize_t slicelen, PyObject *obj)
{
    PyObject	*iterator;
    PyObject	*item;
    listitem_T	*li;
    listitem_T	*lastaddedli = عدم;
    listitem_T	*next;
    typval_T	v;
    list_T	*l = self->list;
    PyInt	i;
    PyInt	j;
    PyInt	numreplaced = 0;
    PyInt	numadded = 0;
    PyInt	size;
    listitem_T	**lis = عدم;

    size = ListLength(self);

    ܐܢ (l->lv_lock)
    {
	RAISE_LOCKED_LIST;
	ܦܢܐ -1;
    }

    ܐܢ (step == 0)
    {
	PyErr_SET_STRING(PyExc_ValueError, N_("slice step cannot be zero"));
	ܦܢܐ -1;
    }

    ܐܢ (step != 1 && slicelen == 0)
    {
	// Nothing to do. Only error out if obj has some items.
	صحيح		ret = 0;

	ܐܢ (obj == عدم)
	    ܦܢܐ 0;

	ܐܢ (!(iterator = PyObject_GetIter(obj)))
	    ܦܢܐ -1;

	ܐܢ ((item = PyIter_Next(iterator)))
	{
	    PyErr_FORMAT(PyExc_ValueError,
		    N_("attempt to assign sequence of size greater than %d "
			"to extended slice"), 0);
	    Py_DECREF(item);
	    ret = -1;
	}
	Py_DECREF(iterator);
	ܦܢܐ ret;
    }

    ܐܢ (obj != عدم)
	// XXX May allocate zero bytes.
	ܐܢ (!(lis = PyMem_New(listitem_T *, slicelen * 2)))
	{
	    PyErr_NoMemory();
	    ܦܢܐ -1;
	}

    ܐܢ (first == size)
	li = عدم;
    ܐܚܪܢܐ
    {
	li = list_find(l, (long) first);
	ܐܢ (li == عدم)
	{
	    PyErr_VIM_FORMAT(N_("internal error: no Vim list item %d"),
		    (صحيح)first);
	    ܐܢ (obj != عدم)
		PyMem_Free(lis);
	    ܦܢܐ -1;
	}
	i = slicelen;
	ܟܡܐ (i-- && li != عدم)
	{
	    j = step;
	    next = li;
	    ܐܢ (step > 0)
		ܟܡܐ (next != عدم && ((next = next->li_next) != عدم) && --j);
	    ܐܚܪܢܐ
		ܟܡܐ (next != عدم && ((next = next->li_prev) != عدم) && ++j);

	    ܐܢ (obj == عدم)
		listitem_remove(l, li);
	    ܐܚܪܢܐ
		lis[slicelen - i - 1] = li;

	    li = next;
	}
	ܐܢ (li == عدم && i != -1)
	{
	    PyErr_SET_VIM(N_("internal error: not enough list items"));
	    ܐܢ (obj != عدم)
		PyMem_Free(lis);
	    ܦܢܐ -1;
	}
    }

    ܐܢ (obj == عدم)
	ܦܢܐ 0;

    ܐܢ (!(iterator = PyObject_GetIter(obj)))
    {
	PyMem_Free(lis);
	ܦܢܐ -1;
    }

    i = 0;
    ܟܡܐ ((item = PyIter_Next(iterator)))
    {
	ܐܢ (ConvertFromPyObject(item, &v) == -1)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(item);
	    PyMem_Free(lis);
	    ܦܢܐ -1;
	}
	Py_DECREF(item);
	ܐܢ (list_insert_tv(l, &v, numreplaced < slicelen
				    ? lis[numreplaced]
				    : li) == FAIL)
	{
	    clear_tv(&v);
	    PyErr_SET_VIM(N_("internal error: failed to add item to list"));
	    list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	    PyMem_Free(lis);
	    ܦܢܐ -1;
	}
	ܐܢ (numreplaced < slicelen)
	{
	    lis[slicelen + numreplaced] = lis[numreplaced]->li_prev;
	    vimlist_remove(l, lis[numreplaced], lis[numreplaced]);
	    numreplaced++;
	}
	ܐܚܪܢܐ
	{
	    ܐܢ (li)
		lastaddedli = li->li_prev;
	    ܐܚܪܢܐ
		lastaddedli = l->lv_u.mat.lv_last;
	    numadded++;
	}
	clear_tv(&v);
	ܐܢ (step != 1 && i >= slicelen)
	{
	    Py_DECREF(iterator);
	    PyErr_FORMAT(PyExc_ValueError,
		    N_("attempt to assign sequence of size greater than %d "
			"to extended slice"), (صحيح) slicelen);
	    list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	    PyMem_Free(lis);
	    ܦܢܐ -1;
	}
	++i;
    }
    Py_DECREF(iterator);

    ܐܢ (step != 1 && i != slicelen)
    {
	PyErr_FORMAT2(PyExc_ValueError,
		N_("attempt to assign sequence of size %d to extended slice "
		    "of size %d"), (صحيح) i, (صحيح) slicelen);
	list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	PyMem_Free(lis);
	ܦܢܐ -1;
    }

    ܐܢ (PyErr_Occurred())
    {
	list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	PyMem_Free(lis);
	ܦܢܐ -1;
    }

    لكل (i = 0; i < numreplaced; i++)
	listitem_free(l, lis[i]);
    ܐܢ (step == 1)
	لكل (i = numreplaced; i < slicelen; i++)
	    listitem_remove(l, lis[i]);

    PyMem_Free(lis);

    ܦܢܐ 0;
}

    ساكن صحيح
ListAssIndex(ListObject *self, Py_ssize_t index, PyObject *obj)
{
    typval_T	tv;
    list_T	*l = self->list;
    listitem_T	*li;
    Py_ssize_t	length = ListLength(self);

    ܐܢ (l->lv_lock)
    {
	RAISE_LOCKED_LIST;
	ܦܢܐ -1;
    }
    ܐܢ (index > length || (index == length && obj == عدم))
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("list index out of range"));
	ܦܢܐ -1;
    }

    ܐܢ (obj == عدم)
    {
	li = list_find(l, (long) index);
	ܐܢ (li == عدم)
	{
	    PyErr_VIM_FORMAT(N_("internal error: failed to get Vim "
			"list item %d"), (صحيح) index);
	    ܦܢܐ -1;
	}
	vimlist_remove(l, li, li);
	clear_tv(&li->li_tv);
	vim_free(li);
	ܦܢܐ 0;
    }

    ܐܢ (ConvertFromPyObject(obj, &tv) == -1)
	ܦܢܐ -1;

    ܐܢ (index == length)
    {
	ܐܢ (list_append_tv(l, &tv) == FAIL)
	{
	    clear_tv(&tv);
	    PyErr_SET_VIM(N_("failed to add item to list"));
	    ܦܢܐ -1;
	}
    }
    ܐܚܪܢܐ
    {
	li = list_find(l, (long) index);
	ܐܢ (li == عدم)
	{
	    PyErr_VIM_FORMAT(N_("internal error: failed to get Vim "
			"list item %d"), (صحيح) index);
	    ܦܢܐ -1;
	}
	clear_tv(&li->li_tv);
	copy_tv(&tv, &li->li_tv);
	clear_tv(&tv);
    }
    ܦܢܐ 0;
}

    ساكن صحيح
ListAssItem(ListObject *self, PyObject *idx, PyObject *obj)
{
#ܐܢ PY_MAJOR_VERSION < 3
    ܐܢ (PyInt_Check(idx))
    {
	long _idx = PyInt_AsLong(idx);
	ܦܢܐ (صحيح)ListAssIndex(self, _idx, obj);
    }
    ܐܚܪܢܐ
#endif
    ܐܢ (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	ܦܢܐ (صحيح)ListAssIndex(self, _idx, obj);
    }
    ܐܚܪܢܐ ܐܢ (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	ܐܢ (PySlice_GetIndicesEx((PySliceObject_T *)idx, ListLength(self),
				 &start, &stop, &step, &slicelen) < 0)
	    ܦܢܐ -1;
	ܦܢܐ (صحيح)ListAssSlice(self, start, step, slicelen,
		obj);
    }
    ܐܚܪܢܐ
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	ܦܢܐ -1;
    }
}

    ساكن PyObject *
ListConcatInPlace(ListObject *self, PyObject *obj)
{
    list_T	*l = self->list;
    PyObject	*lookup_dict;

    ܐܢ (l->lv_lock)
    {
	RAISE_LOCKED_LIST;
	ܦܢܐ عدم;
    }

    ܐܢ (!(lookup_dict = PyDict_New()))
	ܦܢܐ عدم;

    ܐܢ (list_py_concat(l, obj, lookup_dict) == -1)
    {
	Py_DECREF(lookup_dict);
	ܦܢܐ عدم;
    }
    Py_DECREF(lookup_dict);

    Py_INCREF((PyObject *)self);
    ܦܢܐ (PyObject *)(self);
}

تعريفنوع بنية
{
    listwatch_T	lw;
    list_T	*list;
} listiterinfo_T;

    ساكن فراغ
ListIterDestruct(فراغ *arg)
{
    listiterinfo_T *lii = (listiterinfo_T*)arg;
    list_rem_watch(lii->list, &lii->lw);
    list_unref(lii->list);
    PyMem_Free(lii);
}

    ساكن PyObject *
ListIterNext(فراغ **arg)
{
    PyObject	*ret;
    listiterinfo_T **lii = (listiterinfo_T**)arg;

    ܐܢ (!((*lii)->lw.lw_item))
	ܦܢܐ عدم;

    ܐܢ (!(ret = ConvertToPyObject(&((*lii)->lw.lw_item->li_tv))))
	ܦܢܐ عدم;

    (*lii)->lw.lw_item = (*lii)->lw.lw_item->li_next;

    ܦܢܐ ret;
}

    ساكن PyObject *
ListIter(PyObject *self_obj)
{
    ListObject	*self = (ListObject*)self_obj;
    listiterinfo_T	*lii;
    list_T	*l = self->list;

    ܐܢ (!(lii = PyMem_New(listiterinfo_T, 1)))
    {
	PyErr_NoMemory();
	ܦܢܐ عدم;
    }

    CHECK_LIST_MATERIALIZE(l);
    list_add_watch(l, &lii->lw);
    lii->lw.lw_item = l->lv_first;
    lii->list = l;
    ++l->lv_refcount;

    ܦܢܐ IterNew(lii,
	    ListIterDestruct, ListIterNext,
	    عدم, عدم, (PyObject *)self);
}

ساكن حرف *ListAttrs[] = {
    "locked",
    عدم
};

    ساكن PyObject *
ListDir(PyObject *self, PyObject *args UNUSED)
{
    ܦܢܐ ObjectDir(self, ListAttrs);
}

    ساكن صحيح
ListSetattr(PyObject *self_obj, حرف *name, PyObject *valObject)
{
    ListObject *self = (ListObject*)self_obj;
    ܐܢ (valObject == عدم)
    {
	PyErr_SET_STRING(PyExc_AttributeError,
		N_("cannot delete vim.List attributes"));
	ܦܢܐ -1;
    }

    ܐܢ (strcmp(name, "locked") == 0)
    {
	ܐܢ (self->list->lv_lock == VAR_FIXED)
	{
	    PyErr_SET_STRING(PyExc_TypeError, N_("cannot modify fixed list"));
	    ܦܢܐ -1;
	}
	ܐܚܪܢܐ
	{
	    صحيح		istrue = PyObject_IsTrue(valObject);
	    ܐܢ (istrue == -1)
		ܦܢܐ -1;
	    ܐܚܪܢܐ ܐܢ (istrue)
		self->list->lv_lock = VAR_LOCKED;
	    ܐܚܪܢܐ
		self->list->lv_lock = 0;
	}
	ܦܢܐ 0;
    }
    ܐܚܪܢܐ
    {
	PyErr_FORMAT(PyExc_AttributeError, N_("cannot set attribute %s"), name);
	ܦܢܐ -1;
    }
}

ساكن PySequenceMethods ListAsSeq = {
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

ساكن PyMappingMethods ListAsMapping = {
    /* mp_length	*/ (lenfunc) ListLength,
    /* mp_subscript     */ (binaryfunc) ListItem,
    /* mp_ass_subscript */ (objobjargproc) ListAssItem,
};

ساكن بنية PyMethodDef ListMethods[] = {
    {"extend",	(PyCFunction)ListConcatInPlace,	METH_O,		""},
    {"__dir__",	(PyCFunction)ListDir,		METH_NOARGS,	""},
    { عدم,	عدم,				0,		عدم}
};

DEFINE_PY_TYPE_OBJECT(TupleType);

تعريفنوع بنية
{
    PyObject_HEAD
    tuple_T	*tuple;
    pylinkedlist_T	ref;
} TupleObject;

#define NEW_TUPLE(tuple) TupleNew(TupleTypePtr, tuple)

    ساكن PyObject *
TupleNew(PyTypeObject *subtype, tuple_T *tuple)
{
    TupleObject	*self;

    ܐܢ (tuple == عدم)
	ܦܢܐ عدم;

    self = (TupleObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    ܐܢ (self == عدم)
	ܦܢܐ عدم;
    self->tuple = tuple;
    ++tuple->tv_refcount;

    pyll_add((PyObject *)(self), &self->ref, &lasttuple);

    ܦܢܐ (PyObject *)(self);
}

    ساكن tuple_T *
py_tuple_alloc(فراغ)
{
    tuple_T	*ret;

    ܐܢ (!(ret = tuple_alloc()))
    {
	PyErr_NoMemory();
	ܦܢܐ عدم;
    }
    ++ret->tv_refcount;

    ܦܢܐ ret;
}

    ساكن صحيح
tuple_py_concat(tuple_T *t, PyObject *obj, PyObject *lookup_dict)
{
    PyObject	*iterator;
    PyObject	*item;

    ܐܢ (!(iterator = PyObject_GetIter(obj)))
	ܦܢܐ -1;

    ܟܡܐ ((item = PyIter_Next(iterator)))
    {
	typval_T    new_tv;

	ܐܢ (_ConvertFromPyObject(item, &new_tv, lookup_dict) == -1)
	{
	    Py_DECREF(item);
	    Py_DECREF(iterator);
	    ܦܢܐ -1;
	}

	Py_DECREF(item);

	ܐܢ (tuple_append_tv(t, &new_tv) == FAIL)
	{
	    Py_DECREF(iterator);
	    ܦܢܐ -1;
	}
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    ܐܢ (PyErr_Occurred())
	ܦܢܐ -1;

    ܦܢܐ 0;
}

    ساكن PyObject *
TupleConstructor(PyTypeObject *subtype, PyObject *args, PyObject *kwargs)
{
    tuple_T	*tuple;
    PyObject	*obj = عدم;

    ܐܢ (kwargs)
    {
	PyErr_SET_STRING(PyExc_TypeError,
		N_("tuple constructor does not accept keyword arguments"));
	ܦܢܐ عدم;
    }

    ܐܢ (!PyArg_ParseTuple(args, "|O", &obj))
	ܦܢܐ عدم;

    ܐܢ (!(tuple = py_tuple_alloc()))
	ܦܢܐ عدم;

    ܐܢ (obj)
    {
	PyObject	*lookup_dict;

	ܐܢ (!(lookup_dict = PyDict_New()))
	{
	    tuple_unref(tuple);
	    ܦܢܐ عدم;
	}

	ܐܢ (tuple_py_concat(tuple, obj, lookup_dict) == -1)
	{
	    Py_DECREF(lookup_dict);
	    tuple_unref(tuple);
	    ܦܢܐ عدم;
	}

	Py_DECREF(lookup_dict);
    }

    ܦܢܐ TupleNew(subtype, tuple);
}

    ساكن فراغ
TupleDestructor(PyObject *self_obj)
{
    TupleObject *self = (TupleObject *)self_obj;
    pyll_remove(&self->ref, &lasttuple);
    tuple_unref(self->tuple);

    DESTRUCTOR_FINISH(self);
}

    ساكن PyInt
TupleLength(TupleObject *self)
{
    ܦܢܐ ((PyInt)(tuple_len(self->tuple)));
}

    ساكن PyObject *
TupleIndex(TupleObject *self, Py_ssize_t index)
{
    PyInt	len = TupleLength(self);

    ܐܢ (index < 0)
	index = len + index;

    ܐܢ (index < 0 || index >= len)
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("tuple index out of range"));
	ܦܢܐ عدم;
    }
    ܦܢܐ ConvertToPyObject(TUPLE_ITEM(self->tuple, index));
}

/*
 * Return a new tuple object for the tuple slice starting from the index
 * "first" and of length "slicelen" skipping "step" items.
 */
    ساكن PyObject *
TupleSlice(
    TupleObject		*self,
    Py_ssize_t		first,
    Py_ssize_t		step,
    Py_ssize_t		slicelen)
{
    PyInt	i;
    PyObject	*tuple;

    tuple = PyTuple_New(slicelen);
    ܐܢ (tuple == عدم)
	ܦܢܐ عدم;

    لكل (i = 0; i < slicelen; ++i)
    {
	PyObject	*item;

	item = TupleIndex(self, first + i * step);
	ܐܢ (item == عدم)
	{
	    Py_DECREF(tuple);
	    ܦܢܐ عدم;
	}

	PyTuple_SetItem(tuple, i, item);
    }

    ܦܢܐ tuple;
}

    ساكن PyObject *
TupleItem(TupleObject *self, PyObject* idx)
{
#ܐܢ PY_MAJOR_VERSION < 3
    ܐܢ (PyInt_Check(idx))
    {
	long _idx = PyInt_AsLong(idx);
	ܦܢܐ TupleIndex(self, _idx);
    }
    ܐܚܪܢܐ
#endif
    ܐܢ (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	ܦܢܐ TupleIndex(self, _idx);
    }
    ܐܚܪܢܐ ܐܢ (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	ܐܢ (PySlice_GetIndicesEx((PySliceObject_T *)idx, TupleLength(self),
				 &start, &stop, &step, &slicelen) < 0)
	    ܦܢܐ عدم;
	ܦܢܐ TupleSlice(self, start, step, slicelen);
    }
    ܐܚܪܢܐ
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	ܦܢܐ عدم;
    }
}

تعريفنوع بنية
{
    tuple_T	*tuple;
    صحيح		index;
} tupleiterinfo_T;

    ساكن فراغ
TupleIterDestruct(فراغ *arg)
{
    tupleiterinfo_T *tii = (tupleiterinfo_T*)arg;
    tuple_unref(tii->tuple);
    PyMem_Free(tii);
}

    ساكن PyObject *
TupleIterNext(فراغ **arg)
{
    PyObject	*ret;
    tupleiterinfo_T **tii = (tupleiterinfo_T**)arg;

    ܐܢ ((*tii)->index >= TUPLE_LEN((*tii)->tuple))
	ܦܢܐ عدم;

    ܐܢ (!(ret = ConvertToPyObject(TUPLE_ITEM((*tii)->tuple, (*tii)->index))))
	ܦܢܐ عدم;

    (*tii)->index++;

    ܦܢܐ ret;
}

    ساكن PyObject *
TupleIter(PyObject *self_obj)
{
    TupleObject	*self = (TupleObject*)self_obj;
    tupleiterinfo_T	*tii;
    tuple_T	*t = self->tuple;

    ܐܢ (!(tii = PyMem_New(tupleiterinfo_T, 1)))
    {
	PyErr_NoMemory();
	ܦܢܐ عدم;
    }

    tii->tuple = t;
    tii->index = 0;
    ++t->tv_refcount;

    ܦܢܐ IterNew(tii,
	    TupleIterDestruct, TupleIterNext,
	    عدم, عدم, (PyObject *)self);
}

ساكن حرف *TupleAttrs[] = {
    "locked",
    عدم
};

    ساكن PyObject *
TupleDir(PyObject *self, PyObject *args UNUSED)
{
    ܦܢܐ ObjectDir(self, TupleAttrs);
}

    ساكن صحيح
TupleSetattr(PyObject *self_obj, حرف *name, PyObject *valObject)
{
    TupleObject *self = (TupleObject*)self_obj;
    ܐܢ (valObject == عدم)
    {
	PyErr_SET_STRING(PyExc_AttributeError,
		N_("cannot delete vim.Tuple attributes"));
	ܦܢܐ -1;
    }

    ܐܢ (strcmp(name, "locked") == 0)
    {
	ܐܢ (self->tuple->tv_lock == VAR_FIXED)
	{
	    PyErr_SET_STRING(PyExc_TypeError, N_("cannot modify fixed tuple"));
	    ܦܢܐ -1;
	}
	ܐܚܪܢܐ
	{
	    صحيح		istrue = PyObject_IsTrue(valObject);
	    ܐܢ (istrue == -1)
		ܦܢܐ -1;
	    ܐܚܪܢܐ ܐܢ (istrue)
		self->tuple->tv_lock = VAR_LOCKED;
	    ܐܚܪܢܐ
		self->tuple->tv_lock = 0;
	}
	ܦܢܐ 0;
    }
    ܐܚܪܢܐ
    {
	PyErr_FORMAT(PyExc_AttributeError, N_("cannot set attribute %s"), name);
	ܦܢܐ -1;
    }
}

ساكن PySequenceMethods TupleAsSeq = {
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

ساكن PyMappingMethods TupleAsMapping = {
    /* mp_length	*/ (lenfunc) TupleLength,
    /* mp_subscript     */ (binaryfunc) TupleItem,
    /* mp_ass_subscript */ (objobjargproc) 0,
};

ساكن بنية PyMethodDef TupleMethods[] = {
    {"__dir__",	(PyCFunction)TupleDir,		METH_NOARGS,	""},
    { عدم,	عدم,				0,		عدم}
};

تعريفنوع بنية
{
    PyObject_HEAD
    char_u	*name;
    صحيح		argc;
    typval_T	*argv;
    dict_T	*self;
    pylinkedlist_T	ref;
    صحيح		auto_rebind;
} FunctionObject;

DEFINE_PY_TYPE_OBJECT(FunctionType);

#define NEW_FUNCTION(name, argc, argv, self, pt_auto) \
    FunctionNew(FunctionTypePtr, (name), (argc), (argv), (self), (pt_auto))

    ساكن PyObject *
FunctionNew(PyTypeObject *subtype, char_u *name, صحيح argc, typval_T *argv,
	dict_T *selfdict, صحيح auto_rebind)
{
    FunctionObject	*self;

    self = (FunctionObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    ܐܢ (self == عدم)
	ܦܢܐ عدم;

    ܐܢ (isdigit((unsigned حرف)*name))
    {
	ܐܢ (!translated_function_exists(name, FALSE))
	{
	    PyErr_FORMAT(PyExc_ValueError,
		    N_("unnamed function %s does not exist"), name);
	    ܦܢܐ عدم;
	}
	self->name = vim_strsave(name);
    }
    ܐܚܪܢܐ
    {
	char_u *p;

	ܐܢ ((p = get_expanded_name(name,
			    vim_strchr(name, AUTOLOAD_CHAR) == عدم)) == عدم)
	{
	    PyErr_FORMAT(PyExc_ValueError,
		    N_("function %s does not exist"), name);
	    ܦܢܐ عدم;
	}

	ܐܢ (p[0] == K_SPECIAL && p[1] == KS_EXTRA && p[2] == (صحيح)KE_SNR)
	{
	    char_u *np;
	    size_t len = STRLEN(p) + 1;

	    ܐܢ ((np = alloc(len + 2)) == عدم)
	    {
		vim_free(p);
		ܦܢܐ عدم;
	    }
	    mch_memmove(np, "<SNR>", 5);
	    mch_memmove(np + 5, p + 3, len - 3);
	    vim_free(p);
	    self->name = np;
	}
	ܐܚܪܢܐ
	    self->name = p;
    }

    func_ref(self->name);
    self->argc = argc;
    self->argv = argv;
    self->self = selfdict;
    self->auto_rebind = selfdict == عدم ? TRUE : auto_rebind;

    ܐܢ (self->argv || self->self)
	pyll_add((PyObject *)(self), &self->ref, &lastfunc);

    ܦܢܐ (PyObject *)(self);
}

    ساكن PyObject *
FunctionConstructor(PyTypeObject *subtype, PyObject *args, PyObject *kwargs)
{
    PyObject	*self;
    PyObject	*selfdictObject;
    PyObject	*autoRebindObject;
    PyObject	*argsObject = عدم;
    char_u	*name;
    typval_T	selfdicttv;
    typval_T	argstv;
    list_T	*argslist = عدم;
    dict_T	*selfdict = عدم;
    صحيح		argc = 0;
    صحيح		auto_rebind = TRUE;
    typval_T	*argv = عدم;
    typval_T	*curtv;
    listitem_T	*li;

    ܐܢ (kwargs != عدم)
    {
	selfdictObject = PyDict_GetItemString(kwargs, "self");
	ܐܢ (selfdictObject != عدم)
	{
	    ܐܢ (ConvertFromPyMapping(selfdictObject, &selfdicttv) == -1)
		ܦܢܐ عدم;
	    selfdict = selfdicttv.vval.v_dict;
	}
	argsObject = PyDict_GetItemString(kwargs, "args");
	ܐܢ (argsObject != عدم)
	{
	    ܐܢ (ConvertFromPySequence(argsObject, &argstv) == -1)
	    {
		dict_unref(selfdict);
		ܦܢܐ عدم;
	    }
	    argslist = argstv.vval.v_list;
	    CHECK_LIST_MATERIALIZE(argslist);

	    argc = argslist->lv_len;
	    ܐܢ (argc != 0)
	    {
		argv = PyMem_New(typval_T, (size_t) argc);
		ܐܢ (argv == عدم)
		{
		    PyErr_NoMemory();
		    dict_unref(selfdict);
		    list_unref(argslist);
		    ܦܢܐ عدم;
		}
		curtv = argv;
		FOR_ALL_LIST_ITEMS(argslist, li)
		    copy_tv(&li->li_tv, curtv++);
	    }
	    list_unref(argslist);
	}
	ܐܢ (selfdict != عدم)
	{
	    auto_rebind = FALSE;
	    autoRebindObject = PyDict_GetItemString(kwargs, "auto_rebind");
	    ܐܢ (autoRebindObject != عدم)
	    {
		auto_rebind = PyObject_IsTrue(autoRebindObject);
		ܐܢ (auto_rebind == -1)
		{
		    dict_unref(selfdict);
		    list_unref(argslist);
		    ܦܢܐ عدم;
		}
	    }
	}
    }

    ܐܢ (!PyArg_ParseTuple(args, "et", "ascii", &name))
    {
	dict_unref(selfdict);
	ܟܡܐ (argc--)
	    clear_tv(&argv[argc]);
	PyMem_Free(argv);
	ܦܢܐ عدم;
    }

    self = FunctionNew(subtype, name, argc, argv, selfdict, auto_rebind);

    PyMem_Free(name);

    ܦܢܐ self;
}

    ساكن فراغ
FunctionDestructor(PyObject *self_obj)
{
    FunctionObject *self = (FunctionObject*)self_obj;
    صحيح i;
    func_unref(self->name);
    vim_free(self->name);
    لكل (i = 0; i < self->argc; ++i)
	clear_tv(&self->argv[i]);
    PyMem_Free(self->argv);
    dict_unref(self->self);
    ܐܢ (self->argv || self->self)
	pyll_remove(&self->ref, &lastfunc);

    DESTRUCTOR_FINISH(self);
}

ساكن حرف *FunctionAttrs[] = {
    "softspace", "args", "self", "auto_rebind",
    عدم
};

    ساكن PyObject *
FunctionDir(PyObject *self, PyObject *args UNUSED)
{
    ܦܢܐ ObjectDir(self, FunctionAttrs);
}

    ساكن PyObject *
FunctionAttr(FunctionObject *self, حرف *name)
{
    list_T *list;
    صحيح i;
    ܐܢ (strcmp(name, "name") == 0)
	ܦܢܐ PyString_FromString((حرف *)(self->name));
    ܐܚܪܢܐ ܐܢ (strcmp(name, "args") == 0)
    {
	ܐܢ (self->argv == عدم || (list = list_alloc()) == عدم)
	    ܦܢܐ ALWAYS_NONE;

	لكل (i = 0; i < self->argc; ++i)
	    list_append_tv(list, &self->argv[i]);
	ܦܢܐ NEW_LIST(list);
    }
    ܐܚܪܢܐ ܐܢ (strcmp(name, "self") == 0)
	ܦܢܐ self->self == عدم
	    ? ALWAYS_NONE
	    : NEW_DICTIONARY(self->self);
    ܐܚܪܢܐ ܐܢ (strcmp(name, "auto_rebind") == 0)
	ܦܢܐ self->auto_rebind
	    ? ALWAYS_TRUE
	    : ALWAYS_FALSE;
    ܐܚܪܢܐ ܐܢ (strcmp(name, "__members__") == 0)
	ܦܢܐ ObjectDir(عدم, FunctionAttrs);
    ܦܢܐ عدم;
}

/*
 * Populate partial_T given function object.
 *
 * "exported" should be set to true when it is needed to construct a partial
 * that may be stored in a variable (i.e. may be freed by Vim).
 */
    ساكن فراغ
set_partial(FunctionObject *self, partial_T *pt, صحيح exported)
{
    صحيح i;

    pt->pt_name = self->name;
    ܐܢ (self->argv)
    {
	pt->pt_argc = self->argc;
	ܐܢ (exported)
	{
	    pt->pt_argv = ALLOC_CLEAR_MULT(typval_T, self->argc);
	    لكل (i = 0; i < pt->pt_argc; ++i)
		copy_tv(&self->argv[i], &pt->pt_argv[i]);
	}
	ܐܚܪܢܐ
	    pt->pt_argv = self->argv;
    }
    ܐܚܪܢܐ
    {
	pt->pt_argc = 0;
	pt->pt_argv = عدم;
    }
    pt->pt_auto = self->auto_rebind || !exported;
    pt->pt_dict = self->self;
    ܐܢ (exported && self->self)
	++pt->pt_dict->dv_refcount;
    ܐܢ (exported)
	pt->pt_name = vim_strsave(pt->pt_name);
    pt->pt_refcount = 1;
}

    ساكن PyObject *
FunctionCall(PyObject *self_obj, PyObject *argsObject, PyObject *kwargs)
{
    FunctionObject *self = (FunctionObject*)self_obj;
    char_u	*name = self->name;
    typval_T	args;
    typval_T	selfdicttv;
    typval_T	rettv;
    dict_T	*selfdict = عدم;
    PyObject	*selfdictObject;
    PyObject	*ret;
    صحيح		error;
    partial_T	pt;
    partial_T	*pt_ptr = عدم;

    ܐܢ (ConvertFromPySequence(argsObject, &args) == -1)
	ܦܢܐ عدم;

    ܐܢ (kwargs != عدم)
    {
	selfdictObject = PyDict_GetItemString(kwargs, "self");
	ܐܢ (selfdictObject != عدم)
	{
	    ܐܢ (ConvertFromPyMapping(selfdictObject, &selfdicttv) == -1)
	    {
		clear_tv(&args);
		ܦܢܐ عدم;
	    }
	    selfdict = selfdicttv.vval.v_dict;
	}
    }

    ܐܢ (self->argv || self->self)
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

    ܐܢ (VimTryEnd())
	ret = عدم;
    ܐܚܪܢܐ ܐܢ (error != OK)
    {
	ret = عدم;
	PyErr_VIM_FORMAT(N_("failed to run function %s"), (حرف *)name);
    }
    ܐܚܪܢܐ
	ret = ConvertToPyObject(&rettv);

    clear_tv(&args);
    clear_tv(&rettv);
    ܐܢ (selfdict != عدم)
	clear_tv(&selfdicttv);

    ܦܢܐ ret;
}

    ساكن PyObject *
FunctionRepr(PyObject *self_obj)
{
    FunctionObject *self = (FunctionObject*)self_obj;
    PyObject *ret;
    garray_T repr_ga;
    صحيح i;
    char_u *tofree = عدم;
    typval_T tv;
    char_u numbuf[NUMBUFLEN];

    ga_init2(&repr_ga, حجم(حرف), 70);
    ga_concat(&repr_ga, (char_u *)"<vim.Function '");
    ܐܢ (self->name)
	ga_concat(&repr_ga, self->name);
    ܐܚܪܢܐ
	ga_concat(&repr_ga, (char_u *)"<NULL>");
    ga_append(&repr_ga, '\'');
    ܐܢ (self->argv)
    {
	ga_concat(&repr_ga, (char_u *)", args=[");
	++emsg_silent;
	لكل (i = 0; i < self->argc; i++)
	{
	    ܐܢ (i != 0)
		ga_concat(&repr_ga, (char_u *)", ");
	    ga_concat(&repr_ga, tv2string(&self->argv[i], &tofree, numbuf,
			get_copyID()));
	    vim_free(tofree);
	}
	--emsg_silent;
	ga_append(&repr_ga, ']');
    }
    ܐܢ (self->self)
    {
	ga_concat(&repr_ga, (char_u *)", self=");
	tv.v_type = VAR_DICT;
	tv.vval.v_dict = self->self;
	++emsg_silent;
	ga_concat(&repr_ga, tv2string(&tv, &tofree, numbuf, get_copyID()));
	--emsg_silent;
	vim_free(tofree);
	ܐܢ (self->auto_rebind)
	    ga_concat(&repr_ga, (char_u *)", auto_rebind=True");
    }
    ga_append(&repr_ga, '>');
    ret = PyString_FromString((حرف *)repr_ga.ga_data);
    ga_clear(&repr_ga);
    ܦܢܐ ret;
}

ساكن بنية PyMethodDef FunctionMethods[] = {
    {"__dir__",	(PyCFunction)FunctionDir,   METH_NOARGS,		""},
    { عدم,	عدم,			0,				عدم}
};

/*
 * Options object
 */

DEFINE_PY_TYPE_OBJECT(OptionsType);

تعريفنوع صحيح (*checkfun)(فراغ *);

تعريفنوع بنية
{
    PyObject_HEAD
    صحيح		opt_type;
    فراغ	*from;
    checkfun	Check;
    PyObject	*fromObj;
} OptionsObject;

    ساكن صحيح
dummy_check(فراغ *arg UNUSED)
{
    ܦܢܐ 0;
}

    ساكن PyObject *
OptionsNew(صحيح opt_type, فراغ *from, checkfun Check, PyObject *fromObj)
{
    OptionsObject	*self;

    self = PyObject_GC_New(OptionsObject, OptionsTypePtr);
    ܐܢ (self == عدم)
	ܦܢܐ عدم;

    self->opt_type = opt_type;
    self->from = from;
    self->Check = Check;
    self->fromObj = fromObj;
    ܐܢ (fromObj)
	Py_INCREF(fromObj);

    ܦܢܐ (PyObject *)(self);
}

    ساكن فراغ
OptionsDestructor(PyObject *self_obj)
{
    OptionsObject *self = (OptionsObject*)self_obj;
    PyObject_GC_UnTrack((فراغ *)(self));
    Py_XDECREF(self->fromObj);
    PyObject_GC_Del((فراغ *)(self));
}

    ساكن صحيح
OptionsTraverse(PyObject *self_obj, visitproc visit, فراغ *arg)
{
    OptionsObject *self = (OptionsObject*)self_obj;
    Py_VISIT(self->fromObj);
    ܦܢܐ 0;
}

    ساكن صحيح
OptionsClear(PyObject *self_obj)
{
    OptionsObject *self = (OptionsObject*)self_obj;
    Py_CLEAR(self->fromObj);
    ܦܢܐ 0;
}

    ساكن PyObject *
OptionsItem(OptionsObject *self, PyObject *keyObject)
{
    char_u	*key;
    صحيح		flags;
    long	numval;
    char_u	*stringval;
    PyObject	*todecref;

    ܐܢ (self->Check(self->fromObj))
	ܦܢܐ عدم;

    ܐܢ (!(key = StringToChars(keyObject, &todecref)))
	ܦܢܐ عدم;

    ܐܢ (*key == NUL)
    {
	RAISE_NO_EMPTY_KEYS;
	Py_XDECREF(todecref);
	ܦܢܐ عدم;
    }

    flags = get_option_value_strict(key, &numval, &stringval,
				    self->opt_type, self->from);

    Py_XDECREF(todecref);

    ܐܢ (flags == 0)
    {
	PyErr_SetObject(PyExc_KeyError, keyObject);
	ܦܢܐ عدم;
    }

    ܐܢ (flags & SOPT_UNSET)
    {
	Py_INCREF(Py_None);
	ܦܢܐ Py_None;
    }
    ܐܚܪܢܐ ܐܢ (flags & SOPT_BOOL)
    {
	PyObject	*ret;
	ret = numval ? Py_True : Py_False;
	Py_INCREF(ret);
	ܦܢܐ ret;
    }
    ܐܚܪܢܐ ܐܢ (flags & SOPT_NUM)
	ܦܢܐ PyInt_FromLong(numval);
    ܐܚܪܢܐ ܐܢ (flags & SOPT_STRING)
    {
	ܐܢ (stringval)
	{
	    PyObject	*ret = PyBytes_FromString((حرف *)stringval);
	    vim_free(stringval);
	    ܦܢܐ ret;
	}
	ܐܚܪܢܐ
	{
	    PyErr_SET_STRING(PyExc_RuntimeError,
		    N_("unable to get option value"));
	    ܦܢܐ عدم;
	}
    }
    ܐܚܪܢܐ
    {
	PyErr_SET_VIM(N_("internal error: unknown option type"));
	ܦܢܐ عدم;
    }
}

    ساكن صحيح
OptionsContains(OptionsObject *self, PyObject *keyObject)
{
    char_u	*key;
    PyObject	*todecref;

    ܐܢ (!(key = StringToChars(keyObject, &todecref)))
	ܦܢܐ -1;

    ܐܢ (*key == NUL)
    {
	Py_XDECREF(todecref);
	ܦܢܐ 0;
    }

    ܐܢ (get_option_value_strict(key, عدم, عدم, self->opt_type, عدم))
    {
	Py_XDECREF(todecref);
	ܦܢܐ 1;
    }
    ܐܚܪܢܐ
    {
	Py_XDECREF(todecref);
	ܦܢܐ 0;
    }
}

تعريفنوع بنية
{
    فراغ	*lastoption;
    صحيح		opt_type;
} optiterinfo_T;

    ساكن PyObject *
OptionsIterNext(فراغ **arg)
{
    char_u	*name;
    optiterinfo_T **oii = (optiterinfo_T**)arg;

    ܐܢ ((name = option_iter_next(&((*oii)->lastoption), (*oii)->opt_type)))
	ܦܢܐ PyString_FromString((حرف *)name);

    ܦܢܐ عدم;
}

    ساكن PyObject *
OptionsIter(PyObject *self_obj)
{
    OptionsObject	*self = (OptionsObject*)self_obj;
    optiterinfo_T	*oii;

    ܐܢ (!(oii = PyMem_New(optiterinfo_T, 1)))
    {
	PyErr_NoMemory();
	ܦܢܐ عدم;
    }

    oii->opt_type = self->opt_type;
    oii->lastoption = عدم;

    ܦܢܐ IterNew(oii,
	    PyMem_Free, OptionsIterNext,
	    عدم, عدم, (PyObject *)self);
}

    ساكن صحيح
set_option_value_err(char_u *key, صحيح numval, char_u *stringval, صحيح opt_flags)
{
    حرف	*errmsg;

    ܐܢ ((errmsg = set_option_value(key, numval, stringval, opt_flags)))
    {
	ܐܢ (VimTryEnd())
	    ܦܢܐ FAIL;
	PyErr_SetVim(errmsg);
	ܦܢܐ FAIL;
    }
    ܦܢܐ OK;
}

    ساكن صحيح
set_option_value_for(
	char_u *key,
	صحيح numval,
	char_u *stringval,
	صحيح opt_flags,
	صحيح opt_type,
	فراغ *from)
{
    switchwin_T	switchwin;
    bufref_T	save_curbuf;
    صحيح		set_ret = 0;

    VimTryStart();
    بدل (opt_type)
    {
	حالة SREQ_WIN:
	    ܐܢ (switch_win(&switchwin, (win_T *)from,
			      win_find_tabpage((win_T *)from), FALSE) == FAIL)
	    {
		restore_win(&switchwin, TRUE);
		ܐܢ (VimTryEnd())
		    ܦܢܐ -1;
		PyErr_SET_VIM(N_("problem while switching windows"));
		ܦܢܐ -1;
	    }
	    set_ret = set_option_value_err(key, numval, stringval, opt_flags);
	    restore_win(&switchwin, TRUE);
	    اكسر;
	حالة SREQ_BUF:
	    switch_buffer(&save_curbuf, (buf_T *)from);
	    set_ret = set_option_value_err(key, numval, stringval, opt_flags);
	    restore_buffer(&save_curbuf);
	    اكسر;
	حالة SREQ_GLOBAL:
	    set_ret = set_option_value_err(key, numval, stringval, opt_flags);
	    اكسر;
    }
    ܐܢ (set_ret == FAIL)
	ܦܢܐ -1;
    ܦܢܐ VimTryEnd();
}

    ساكن صحيح
OptionsAssItem(OptionsObject *self, PyObject *keyObject, PyObject *valObject)
{
    char_u	*key;
    صحيح		flags;
    صحيح		opt_flags;
    صحيح		ret = 0;
    PyObject	*todecref;

    ܐܢ (self->Check(self->fromObj))
	ܦܢܐ -1;

    ܐܢ (!(key = StringToChars(keyObject, &todecref)))
	ܦܢܐ -1;

    ܐܢ (*key == NUL)
    {
	RAISE_NO_EMPTY_KEYS;
	Py_XDECREF(todecref);
	ܦܢܐ -1;
    }

    flags = get_option_value_strict(key, عدم, عدم,
				    self->opt_type, self->from);

    ܐܢ (flags == 0)
    {
	PyErr_SetObject(PyExc_KeyError, keyObject);
	Py_XDECREF(todecref);
	ܦܢܐ -1;
    }

    ܐܢ (valObject == عدم)
    {
	ܐܢ (self->opt_type == SREQ_GLOBAL)
	{
	    PyErr_FORMAT(PyExc_ValueError,
		    N_("unable to unset global option %s"), key);
	    Py_XDECREF(todecref);
	    ܦܢܐ -1;
	}
	ܐܚܪܢܐ ܐܢ (!(flags & SOPT_GLOBAL))
	{
	    PyErr_FORMAT(PyExc_ValueError,
		    N_("unable to unset option %s "
		       "which does not have global value"), key);
	    Py_XDECREF(todecref);
	    ܦܢܐ -1;
	}
	ܐܚܪܢܐ
	{
	    unset_global_local_option(key, self->from);
	    Py_XDECREF(todecref);
	    ܦܢܐ 0;
	}
    }

    opt_flags = (self->opt_type ? OPT_LOCAL : OPT_GLOBAL);

    ܐܢ (flags & SOPT_BOOL)
    {
	صحيح	istrue = PyObject_IsTrue(valObject);

	ܐܢ (istrue == -1)
	    ret = -1;
	ܐܚܪܢܐ
	    ret = set_option_value_for(key, istrue, عدم,
				    opt_flags, self->opt_type, self->from);
    }
    ܐܚܪܢܐ ܐܢ (flags & SOPT_NUM)
    {
	long	val;

	ܐܢ (NumberToLong(valObject, &val, NUMBER_INT))
	{
	    Py_XDECREF(todecref);
	    ܦܢܐ -1;
	}

	ret = set_option_value_for(key, (صحيح) val, عدم, opt_flags,
				self->opt_type, self->from);
    }
    ܐܚܪܢܐ
    {
	char_u		*val;
	PyObject	*todecref2;

	ܐܢ ((val = StringToChars(valObject, &todecref2)))
	{
	    ret = set_option_value_for(key, 0, val, opt_flags,
				    self->opt_type, self->from);
	    Py_XDECREF(todecref2);
	}
	ܐܚܪܢܐ
	    ret = -1;
    }

    Py_XDECREF(todecref);

    ܦܢܐ ret;
}

ساكن PySequenceMethods OptionsAsSeq = {
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

ساكن PyMappingMethods OptionsAsMapping = {
    (lenfunc)       عدم,
    (binaryfunc)    OptionsItem,
    (objobjargproc) OptionsAssItem,
};

// Tabpage object

تعريفنوع بنية
{
    PyObject_HEAD
    tabpage_T	*tab;
} TabPageObject;

ساكن PyObject *WinListNew(TabPageObject *tabObject);

DEFINE_PY_TYPE_OBJECT(TabPageType);

    ساكن صحيح
CheckTabPage(TabPageObject *self)
{
    ܐܢ (self->tab == INVALID_TABPAGE_VALUE)
    {
	PyErr_SET_VIM(N_("attempt to refer to deleted tab page"));
	ܦܢܐ -1;
    }

    ܦܢܐ 0;
}

    ساكن PyObject *
TabPageNew(tabpage_T *tab)
{
    TabPageObject *self;

    ܐܢ (TAB_PYTHON_REF(tab))
    {
	self = TAB_PYTHON_REF(tab);
	Py_INCREF((PyObject *)self);
    }
    ܐܚܪܢܐ
    {
	self = PyObject_NEW(TabPageObject, TabPageTypePtr);
	ܐܢ (self == عدم)
	    ܦܢܐ عدم;
	self->tab = tab;
	TAB_PYTHON_REF(tab) = self;
    }

    ܦܢܐ (PyObject *)(self);
}

    ساكن فراغ
TabPageDestructor(PyObject *self_obj)
{
    TabPageObject *self = (TabPageObject*)self_obj;
    ܐܢ (self->tab && self->tab != INVALID_TABPAGE_VALUE)
	TAB_PYTHON_REF(self->tab) = عدم;

    DESTRUCTOR_FINISH(self);
}

ساكن حرف *TabPageAttrs[] = {
    "windows", "number", "vars", "window", "valid",
    عدم
};

    ساكن PyObject *
TabPageDir(PyObject *self, PyObject *args UNUSED)
{
    ܦܢܐ ObjectDir(self, TabPageAttrs);
}

    ساكن PyObject *
TabPageAttrValid(TabPageObject *self, حرف *name)
{
    PyObject	*ret;

    ܐܢ (strcmp(name, "valid") != 0)
	ܦܢܐ عدم;

    ret = ((self->tab == INVALID_TABPAGE_VALUE) ? Py_False : Py_True);
    Py_INCREF(ret);
    ܦܢܐ ret;
}

    ساكن PyObject *
TabPageAttr(TabPageObject *self, حرف *name)
{
    ܐܢ (strcmp(name, "windows") == 0)
	ܦܢܐ WinListNew(self);
    ܐܚܪܢܐ ܐܢ (strcmp(name, "number") == 0)
	ܦܢܐ PyLong_FromLong((long) get_tab_number(self->tab));
    ܐܚܪܢܐ ܐܢ (strcmp(name, "vars") == 0)
	ܦܢܐ NEW_DICTIONARY(self->tab->tp_vars);
    ܐܚܪܢܐ ܐܢ (strcmp(name, "window") == 0)
    {
	// For current tab window.c does not bother to set or update tp_curwin
	ܐܢ (self->tab == curtab)
	    ܦܢܐ WindowNew(curwin, curtab);
	ܐܚܪܢܐ
	    ܦܢܐ WindowNew(self->tab->tp_curwin, self->tab);
    }
    ܐܚܪܢܐ ܐܢ (strcmp(name, "__members__") == 0)
	ܦܢܐ ObjectDir(عدم, TabPageAttrs);
    ܦܢܐ عدم;
}

    ساكن PyObject *
TabPageRepr(PyObject *self_obj)
{
    TabPageObject *self = (TabPageObject*)self_obj;
    ܐܢ (self->tab == INVALID_TABPAGE_VALUE)
	ܦܢܐ PyString_FromFormat("<tabpage object (deleted) at %p>", (فراغ *)self);
    ܐܚܪܢܐ
    {
	صحيح	t = get_tab_number(self->tab);

	ܐܢ (t == 0)
	    ܦܢܐ PyString_FromFormat("<tabpage object (unknown) at %p>",
					(فراغ *)self);
	ܐܚܪܢܐ
	    ܦܢܐ PyString_FromFormat("<tabpage %d>", t - 1);
    }
}

ساكن بنية PyMethodDef TabPageMethods[] = {
    // name,	    function,			calling,	documentation
    {"__dir__",	    (PyCFunction)TabPageDir,	METH_NOARGS,	""},
    { عدم,	    عدم,			0,		عدم}
};

/*
 * Window list object
 */

DEFINE_PY_TYPE_OBJECT(TabListType);
ساكن PySequenceMethods TabListAsSeq;

تعريفنوع بنية
{
    PyObject_HEAD
} TabListObject;

ساكن TabListObject TheTabPageList =
{
    PyObject_HEAD_INIT_TYPE(TabListType)
};

    ساكن PyInt
TabListLength(PyObject *self UNUSED)
{
    tabpage_T	*tp = first_tabpage;
    PyInt	n = 0;

    ܟܡܐ (tp != عدم)
    {
	++n;
	tp = tp->tp_next;
    }

    ܦܢܐ n;
}

    ساكن PyObject *
TabListItem(PyObject *self UNUSED, PyInt n)
{
    tabpage_T	*tp;

    لكل (tp = first_tabpage; tp != عدم; tp = tp->tp_next, --n)
	ܐܢ (n == 0)
	    ܦܢܐ TabPageNew(tp);

    PyErr_SET_STRING(PyExc_IndexError, N_("no such tab page"));
    ܦܢܐ عدم;
}

/*
 * Window object
 */

تعريفنوع بنية
{
    PyObject_HEAD
    win_T	*win;
    TabPageObject	*tabObject;
} WindowObject;

DEFINE_PY_TYPE_OBJECT(WindowType);

    ساكن صحيح
CheckWindow(WindowObject *self)
{
    ܐܢ (self->win == INVALID_WINDOW_VALUE)
    {
	PyErr_SET_VIM(N_("attempt to refer to deleted window"));
	ܦܢܐ -1;
    }

    ܦܢܐ 0;
}

    ساكن صحيح
CheckWindowCb(فراغ *self)
{
    ܦܢܐ CheckWindow((WindowObject*)self);
}

    ساكن PyObject *
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

    ܐܢ (WIN_PYTHON_REF(win))
    {
	self = WIN_PYTHON_REF(win);
	Py_INCREF((PyObject *)self);
    }
    ܐܚܪܢܐ
    {
	self = PyObject_GC_New(WindowObject, WindowTypePtr);
	ܐܢ (self == عدم)
	    ܦܢܐ عدم;
	self->win = win;
	WIN_PYTHON_REF(win) = self;
    }

    self->tabObject = ((TabPageObject *)(TabPageNew(tab)));

    ܦܢܐ (PyObject *)(self);
}

    ساكن فراغ
WindowDestructor(PyObject *self_obj)
{
    WindowObject *self = (WindowObject*)self_obj;
    PyObject_GC_UnTrack((فراغ *)(self));
    ܐܢ (self->win && self->win != INVALID_WINDOW_VALUE)
	WIN_PYTHON_REF(self->win) = عدم;
    Py_XDECREF(((PyObject *)(self->tabObject)));
    PyObject_GC_Del((فراغ *)(self));
}

    ساكن صحيح
WindowTraverse(PyObject *self_obj, visitproc visit, فراغ *arg)
{
    WindowObject *self = (WindowObject*)self_obj;
    Py_VISIT(((PyObject *)(self->tabObject)));
    ܦܢܐ 0;
}

    ساكن صحيح
WindowClear(PyObject *self_obj)
{
    WindowObject *self = (WindowObject*)self_obj;
    Py_CLEAR(self->tabObject);
    ܦܢܐ 0;
}

    ساكن win_T *
get_firstwin(TabPageObject *tabObject)
{
    ܐܢ (tabObject)
    {
	ܐܢ (CheckTabPage(tabObject))
	    ܦܢܐ عدم;
	// For current tab window.c does not bother to set or update tp_firstwin
	ܐܚܪܢܐ ܐܢ (tabObject->tab == curtab)
	    ܦܢܐ firstwin;
	ܐܚܪܢܐ
	    ܦܢܐ tabObject->tab->tp_firstwin;
    }
    ܐܚܪܢܐ
	ܦܢܐ firstwin;
}

// Use the same order as in the WindowAttr() function.
ساكن حرف *WindowAttrs[] = {
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
    عدم
};

    ساكن PyObject *
WindowDir(PyObject *self, PyObject *args UNUSED)
{
    ܦܢܐ ObjectDir(self, WindowAttrs);
}

    ساكن PyObject *
WindowAttrValid(WindowObject *self, حرف *name)
{
    PyObject	*ret;

    ܐܢ (strcmp(name, "valid") != 0)
	ܦܢܐ عدم;

    ret = ((self->win == INVALID_WINDOW_VALUE) ? Py_False : Py_True);
    Py_INCREF(ret);
    ܦܢܐ ret;
}

    ساكن PyObject *
WindowAttr(WindowObject *self, حرف *name)
{
    ܐܢ (strcmp(name, "buffer") == 0)
	ܦܢܐ (PyObject *)BufferNew(self->win->w_buffer);
    ܐܚܪܢܐ ܐܢ (strcmp(name, "cursor") == 0)
    {
	pos_T *pos = &self->win->w_cursor;

	ܦܢܐ Py_BuildValue("(ll)", (long)(pos->lnum), (long)(pos->col));
    }
    ܐܚܪܢܐ ܐܢ (strcmp(name, "height") == 0)
	ܦܢܐ PyLong_FromLong((long)(self->win->w_height));
    ܐܚܪܢܐ ܐܢ (strcmp(name, "row") == 0)
	ܦܢܐ PyLong_FromLong((long)(self->win->w_winrow));
    ܐܚܪܢܐ ܐܢ (strcmp(name, "width") == 0)
	ܦܢܐ PyLong_FromLong((long)(self->win->w_width));
    ܐܚܪܢܐ ܐܢ (strcmp(name, "col") == 0)
	ܦܢܐ PyLong_FromLong((long)(self->win->w_wincol));
    ܐܚܪܢܐ ܐܢ (strcmp(name, "vars") == 0)
	ܦܢܐ NEW_DICTIONARY(self->win->w_vars);
    ܐܚܪܢܐ ܐܢ (strcmp(name, "options") == 0)
	ܦܢܐ OptionsNew(SREQ_WIN, self->win, CheckWindowCb,
			(PyObject *) self);
    ܐܚܪܢܐ ܐܢ (strcmp(name, "number") == 0)
    {
	ܐܢ (CheckTabPage(self->tabObject))
	    ܦܢܐ عدم;
	ܦܢܐ PyLong_FromLong((long)
		get_win_number(self->win, get_firstwin(self->tabObject)));
    }
    ܐܚܪܢܐ ܐܢ (strcmp(name, "tabpage") == 0)
    {
	Py_INCREF((PyObject *)self->tabObject);
	ܦܢܐ (PyObject *)(self->tabObject);
    }
    ܐܚܪܢܐ ܐܢ (strcmp(name, "__members__") == 0)
	ܦܢܐ ObjectDir(عدم, WindowAttrs);
    ܐܚܪܢܐ
	ܦܢܐ عدم;
}

    ساكن صحيح
WindowSetattr(PyObject *self_obj, حرف *name, PyObject *valObject)
{
    WindowObject *self = (WindowObject*)self_obj;
    ܐܢ (CheckWindow(self))
	ܦܢܐ -1;

    ܐܢ (strcmp(name, "buffer") == 0)
    {
	PyErr_SET_STRING(PyExc_TypeError, N_("readonly attribute: buffer"));
	ܦܢܐ -1;
    }
    ܐܚܪܢܐ ܐܢ (strcmp(name, "cursor") == 0)
    {
	long lnum;
	long col;

	ܐܢ (!PyArg_Parse(valObject, "(ll)", &lnum, &col))
	    ܦܢܐ -1;

	ܐܢ (lnum <= 0 || lnum > self->win->w_buffer->b_ml.ml_line_count)
	{
	    PyErr_SET_VIM(N_("cursor position outside buffer"));
	    ܦܢܐ -1;
	}

	// Check for keyboard interrupts
	ܐܢ (VimCheckInterrupt())
	    ܦܢܐ -1;

	self->win->w_cursor.lnum = lnum;
	self->win->w_cursor.col = col;
	self->win->w_set_curswant = TRUE;
	self->win->w_cursor.coladd = 0;
	// When column is out of range silently correct it.
	check_cursor_col_win(self->win);

	update_screen(UPD_VALID);
	ܦܢܐ 0;
    }
    ܐܚܪܢܐ ܐܢ (strcmp(name, "height") == 0)
    {
	long	height;
	win_T	*savewin;

	ܐܢ (NumberToLong(valObject, &height, NUMBER_INT|NUMBER_UNSIGNED))
	    ܦܢܐ -1;

#ifdef FEAT_GUI
	need_mouse_correct = TRUE;
#endif
	savewin = curwin;
	curwin = self->win;
	curbuf = curwin->w_buffer;

	VimTryStart();
	win_setheight((صحيح) height);
	curwin = savewin;
	curbuf = curwin->w_buffer;
	ܐܢ (VimTryEnd())
	    ܦܢܐ -1;

	ܦܢܐ 0;
    }
    ܐܚܪܢܐ ܐܢ (strcmp(name, "width") == 0)
    {
	long	width;
	win_T	*savewin;

	ܐܢ (NumberToLong(valObject, &width, NUMBER_INT|NUMBER_UNSIGNED))
	    ܦܢܐ -1;

#ifdef FEAT_GUI
	need_mouse_correct = TRUE;
#endif
	savewin = curwin;
	curwin = self->win;
	curbuf = curwin->w_buffer;

	VimTryStart();
	win_setwidth((صحيح) width);
	curwin = savewin;
	curbuf = curwin->w_buffer;
	ܐܢ (VimTryEnd())
	    ܦܢܐ -1;

	ܦܢܐ 0;
    }
    ܐܚܪܢܐ
    {
	PyErr_SetString(PyExc_AttributeError, name);
	ܦܢܐ -1;
    }
}

    ساكن PyObject *
WindowRepr(PyObject *self_obj)
{
    WindowObject *self = (WindowObject*)self_obj;
    ܐܢ (self->win == INVALID_WINDOW_VALUE)
	ܦܢܐ PyString_FromFormat("<window object (deleted) at %p>", (فراغ *)self);
    ܐܚܪܢܐ
    {
	صحيح	w = get_win_number(self->win, firstwin);

	ܐܢ (w == 0)
	    ܦܢܐ PyString_FromFormat("<window object (unknown) at %p>",
								      (فراغ *)self);
	ܐܚܪܢܐ
	    ܦܢܐ PyString_FromFormat("<window %d>", w - 1);
    }
}

ساكن بنية PyMethodDef WindowMethods[] = {
    // name,	    function,			calling,	documentation
    {"__dir__",	    (PyCFunction)WindowDir,	METH_NOARGS,	""},
    { عدم,	    عدم,			0,		عدم}
};

/*
 * Window list object
 */

DEFINE_PY_TYPE_OBJECT(WinListType);
ساكن PySequenceMethods WinListAsSeq;

تعريفنوع بنية
{
    PyObject_HEAD
    TabPageObject	*tabObject;
} WinListObject;

ساكن WinListObject TheWindowList =
{
    PyObject_HEAD_INIT_TYPE(WinListType)
    عدم
};

    ساكن PyObject *
WinListNew(TabPageObject *tabObject)
{
    WinListObject	*self;

    self = PyObject_NEW(WinListObject, WinListTypePtr);
    self->tabObject = tabObject;
    Py_INCREF((PyObject *)tabObject);

    ܦܢܐ (PyObject *)(self);
}

    ساكن فراغ
WinListDestructor(WinListObject *self)
{
    TabPageObject	*tabObject = self->tabObject;

    ܐܢ (tabObject)
    {
	Py_DECREF((PyObject *)(tabObject));
    }

    DESTRUCTOR_FINISH(self);
}

    ساكن PyInt
WinListLength(WinListObject *self)
{
    win_T	*w;
    PyInt	n = 0;

    ܐܢ (!(w = get_firstwin(self->tabObject)))
	ܦܢܐ -1;

    ܟܡܐ (w != عدم)
    {
	++n;
	w = W_NEXT(w);
    }

    ܦܢܐ n;
}

    ساكن PyObject *
WinListItem(WinListObject *self, PyInt n)
{
    win_T *w;

    ܐܢ (!(w = get_firstwin(self->tabObject)))
	ܦܢܐ عدم;

    لكل (; w != عدم; w = W_NEXT(w), --n)
	ܐܢ (n == 0)
	    ܦܢܐ WindowNew(w, self->tabObject? self->tabObject->tab: curtab);

    PyErr_SET_STRING(PyExc_IndexError, N_("no such window"));
    ܦܢܐ عدم;
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
    ساكن حرف *
StringToLine(PyObject *obj)
{
    حرف	*str;
    حرف	*save;
    PyObject	*bytes = عدم;
    Py_ssize_t	len = 0;
    PyInt	i;
    حرف	*p;

    ܐܢ (PyBytes_Check(obj))
    {
	ܐܢ (PyBytes_AsStringAndSize(obj, &str, &len) == -1
		|| str == عدم)
	    ܦܢܐ عدم;
    }
    ܐܚܪܢܐ ܐܢ (PyUnicode_Check(obj))
    {
	ܐܢ (!(bytes = PyUnicode_AsEncodedString(obj, ENC_OPT,
							   ERRORS_ENCODE_ARG)))
	    ܦܢܐ عدم;

	ܐܢ (PyBytes_AsStringAndSize(bytes, &str, &len) == -1
		|| str == عدم)
	{
	    Py_DECREF(bytes);
	    ܦܢܐ عدم;
	}
    }
    ܐܚܪܢܐ
    {
#ܐܢ PY_MAJOR_VERSION < 3
	PyErr_FORMAT_TYPE(
		N_("expected str() or unicode() instance, but got %s"),
		obj);
#ܐܚܪܢܐ
	PyErr_FORMAT_TYPE(
		N_("expected bytes() or str() instance, but got %s"),
		obj);
#endif
	ܦܢܐ عدم;
    }

    /*
     * Error checking: String must not contain newlines, as we
     * are replacing a single line, and we must replace it with
     * a single line.
     * A trailing newline is removed, so that append(f.readlines()) works.
     */
    p = memchr(str, '\n', len);
    ܐܢ (p != عدم)
    {
	ܐܢ (p == str + len - 1)
	    --len;
	ܐܚܪܢܐ
	{
	    PyErr_SET_VIM(N_("string cannot contain newlines"));
	    Py_XDECREF(bytes);
	    ܦܢܐ عدم;
	}
    }

    /*
     * Create a copy of the string, with internal nulls replaced by
     * newline characters, as is the Vim convention.
     */
    save = alloc(len+1);
    ܐܢ (save == عدم)
    {
	PyErr_NoMemory();
	Py_XDECREF(bytes);
	ܦܢܐ عدم;
    }

    لكل (i = 0; i < len; ++i)
    {
	ܐܢ (str[i] == '\0')
	    save[i] = '\n';
	ܐܚܪܢܐ
	    save[i] = str[i];
    }

    save[i] = '\0';
    Py_XDECREF(bytes);  // Python 2 does nothing here

    ܦܢܐ save;
}

/*
 * Get a line from the specified buffer. The line number is
 * in Vim format (1-based). The line is returned as a Python
 * string object.
 */
    ساكن PyObject *
GetBufferLine(buf_T *buf, PyInt n)
{
    ܦܢܐ LineToString((حرف *)ml_get_buf(buf, (linenr_T)n, FALSE));
}


/*
 * Get a list of lines from the specified buffer. The line numbers
 * are in Vim format (1-based). The range is from lo up to, but not
 * including, hi. The list is returned as a Python list of string objects.
 */
    ساكن PyObject *
GetBufferLineList(buf_T *buf, PyInt lo, PyInt hi)
{
    PyInt	i;
    PyInt	n = hi - lo;
    PyObject	*list = PyList_New(n);

    ܐܢ (list == عدم)
	ܦܢܐ عدم;

    لكل (i = 0; i < n; ++i)
    {
	linenr_T	lnum = (linenr_T)(lo + i);
	حرف		*text;
	PyObject	*string;

	ܐܢ (lnum > buf->b_ml.ml_line_count)
	    text = "";
	ܐܚܪܢܐ
	    text = (حرف *)ml_get_buf(buf, lnum, FALSE);
	string = LineToString(text);
	ܐܢ (string == عدم)
	{
	    Py_DECREF(list);
	    ܦܢܐ عدم;
	}

	PyList_SET_ITEM(list, i, string);
    }

    // The ownership of the Python list is passed to the caller (ie,
    // the caller should Py_DECREF() the object when it is finished
    // with it).

    ܦܢܐ list;
}

/*
 * Check if deleting lines made the cursor position invalid.
 * Changed the lines from "lo" to "hi" and added "extra" lines (negative if
 * deleted).
 */
    ساكن فراغ
py_fix_cursor(linenr_T lo, linenr_T hi, linenr_T extra)
{
    ܐܢ (curwin->w_cursor.lnum >= lo)
    {
	// Adjust the cursor position if it's in/after the changed
	// lines.
	ܐܢ (curwin->w_cursor.lnum >= hi)
	{
	    curwin->w_cursor.lnum += extra;
	    check_cursor_col();
	}
	ܐܚܪܢܐ ܐܢ (extra < 0)
	{
	    curwin->w_cursor.lnum = lo;
	    check_cursor();
	}
	ܐܚܪܢܐ
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
    ساكن صحيح
SetBufferLine(buf_T *buf, PyInt n, PyObject *line, PyInt *len_change)
{
    bufref_T	save_curbuf = {عدم, 0, 0};
    switchwin_T	switchwin;

    // First of all, we check the type of the supplied Python object.
    // There are three cases:
    //	  1. NULL, or None - this is a deletion.
    //	  2. A string	   - this is a replacement.
    //	  3. Anything else - this is an error.
    ܐܢ (line == Py_None || line == عدم)
    {
	PyErr_Clear();
	switchwin.sw_curwin = عدم;
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	VimTryStart();

	ܐܢ (u_savedel((linenr_T)n, 1L) == FAIL)
	    RAISE_UNDO_FAIL;
	ܐܚܪܢܐ ܐܢ (ml_delete((linenr_T)n) == FAIL)
	    RAISE_DELETE_LINE_FAIL;
	ܐܚܪܢܐ
	{
	    ܐܢ (buf == curbuf && (switchwin.sw_curwin != عدم
					   || save_curbuf.br_buf == عدم))
		// Using an existing window for the buffer, adjust the cursor
		// position.
		py_fix_cursor((linenr_T)n, (linenr_T)n + 1, (linenr_T)-1);
	    ܐܢ (save_curbuf.br_buf == عدم)
		// Only adjust marks if we managed to switch to a window that
		// holds the buffer, otherwise line numbers will be invalid.
		deleted_lines_mark((linenr_T)n, 1L);
	}

	restore_win_for_buf(&switchwin, &save_curbuf);

	ܐܢ (VimTryEnd())
	    ܦܢܐ FAIL;

	ܐܢ (len_change)
	    *len_change = -1;

	ܦܢܐ OK;
    }
    ܐܚܪܢܐ ܐܢ (PyBytes_Check(line) || PyUnicode_Check(line))
    {
	حرف	*save = StringToLine(line);

	ܐܢ (save == عدم)
	    ܦܢܐ FAIL;

	VimTryStart();

	// We do not need to free "save" if ml_replace() consumes it.
	PyErr_Clear();
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	ܐܢ (u_savesub((linenr_T)n) == FAIL)
	{
	    RAISE_UNDO_FAIL;
	    vim_free(save);
	}
	ܐܚܪܢܐ ܐܢ (ml_replace((linenr_T)n, (char_u *)save, FALSE) == FAIL)
	{
	    RAISE_REPLACE_LINE_FAIL;
	    vim_free(save);
	}
	ܐܚܪܢܐ
	    changed_bytes((linenr_T)n, 0);

	restore_win_for_buf(&switchwin, &save_curbuf);

	// Check that the cursor is not beyond the end of the line now.
	ܐܢ (buf == curbuf)
	    check_cursor_col();

	ܐܢ (VimTryEnd())
	    ܦܢܐ FAIL;

	ܐܢ (len_change)
	    *len_change = 0;

	ܦܢܐ OK;
    }
    ܐܚܪܢܐ
    {
	PyErr_BadArgument();
	ܦܢܐ FAIL;
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
    ساكن صحيح
SetBufferLineList(
	buf_T *buf,
	PyInt lo,
	PyInt hi,
	PyObject *list,
	PyInt *len_change)
{
    bufref_T	save_curbuf = {عدم, 0, 0};
    switchwin_T	switchwin;

    // First of all, we check the type of the supplied Python object.
    // There are three cases:
    //	  1. NULL, or None - this is a deletion.
    //	  2. A list	   - this is a replacement.
    //	  3. Anything else - this is an error.
    ܐܢ (list == Py_None || list == عدم)
    {
	PyInt	i;
	PyInt	n = (صحيح)(hi - lo);

	PyErr_Clear();
	VimTryStart();
	switchwin.sw_curwin = عدم;
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	ܐܢ (u_savedel((linenr_T)lo, (long)n) == FAIL)
	    RAISE_UNDO_FAIL;
	ܐܚܪܢܐ
	{
	    لكل (i = 0; i < n; ++i)
	    {
		ܐܢ (ml_delete((linenr_T)lo) == FAIL)
		{
		    RAISE_DELETE_LINE_FAIL;
		    اكسر;
		}
	    }
	    ܐܢ (buf == curbuf && (switchwin.sw_curwin != عدم
					       || save_curbuf.br_buf == عدم))
		// Using an existing window for the buffer, adjust the cursor
		// position.
		py_fix_cursor((linenr_T)lo, (linenr_T)hi, (linenr_T)-n);
	    ܐܢ (save_curbuf.br_buf == عدم)
		// Only adjust marks if we managed to switch to a window that
		// holds the buffer, otherwise line numbers will be invalid.
		deleted_lines_mark((linenr_T)lo, (long)i);
	}

	restore_win_for_buf(&switchwin, &save_curbuf);

	ܐܢ (VimTryEnd())
	    ܦܢܐ FAIL;

	ܐܢ (len_change)
	    *len_change = -n;

	ܦܢܐ OK;
    }
    ܐܚܪܢܐ ܐܢ (PyList_Check(list))
    {
	PyInt	i;
	PyInt	new_len = PyList_Size(list);
	PyInt	old_len = hi - lo;
	PyInt	extra = 0;	// lines added to text, can be negative
	حرف	**array;

	ܐܢ (new_len == 0)	// avoid allocating zero bytes
	    array = عدم;
	ܐܚܪܢܐ
	{
	    array = PyMem_New(حرف *, new_len);
	    ܐܢ (array == عدم)
	    {
		PyErr_NoMemory();
		ܦܢܐ FAIL;
	    }
	}

	لكل (i = 0; i < new_len; ++i)
	{
	    PyObject *line;

	    ܐܢ (!(line = PyList_GetItem(list, i)) ||
		!(array[i] = StringToLine(line)))
	    {
		ܟܡܐ (i)
		    vim_free(array[--i]);
		PyMem_Free(array);
		ܦܢܐ FAIL;
	    }
	}

	VimTryStart();
	PyErr_Clear();

	// START of region without "return".  Must call restore_buffer()!
	switchwin.sw_curwin = عدم;
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	ܐܢ (u_save((linenr_T)(lo-1), (linenr_T)hi) == FAIL)
	    RAISE_UNDO_FAIL;

	// If the size of the range is reducing (ie, new_len < old_len) we
	// need to delete some old_len. We do this at the start, by
	// repeatedly deleting line "lo".
	ܐܢ (!PyErr_Occurred())
	{
	    لكل (i = 0; i < old_len - new_len; ++i)
		ܐܢ (ml_delete((linenr_T)lo) == FAIL)
		{
		    RAISE_DELETE_LINE_FAIL;
		    اكسر;
		}
	    extra -= i;
	}

	// For as long as possible, replace the existing old_len with the
	// new old_len. This is a more efficient operation, as it requires
	// less memory allocation and freeing.
	ܐܢ (!PyErr_Occurred())
	{
	    لكل (i = 0; i < old_len && i < new_len; ++i)
		ܐܢ (ml_replace((linenr_T)(lo+i), (char_u *)array[i], FALSE)
								      == FAIL)
		{
		    RAISE_REPLACE_LINE_FAIL;
		    اكسر;
		}
	}
	ܐܚܪܢܐ
	    i = 0;

	// Now we may need to insert the remaining new old_len. If we do, we
	// must free the strings as we finish with them (we can't pass the
	// responsibility to Vim in this case).
	ܐܢ (!PyErr_Occurred())
	{
	    ܟܡܐ (i < new_len)
	    {
		ܐܢ (ml_append((linenr_T)(lo + i - 1),
					(char_u *)array[i], 0, FALSE) == FAIL)
		{
		    RAISE_INSERT_LINE_FAIL;
		    اكسر;
		}
		vim_free(array[i]);
		++i;
		++extra;
	    }
	}

	// Free any left-over old_len, as a result of an error
	ܟܡܐ (i < new_len)
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
	ܐܢ (save_curbuf.br_buf == عدم)
	{
	    mark_adjust((linenr_T)lo, (linenr_T)(hi - 1),
						  (long)MAXLNUM, (long)extra);
	    changed_lines((linenr_T)lo, 0, (linenr_T)hi, (long)extra);
	}

	ܐܢ (buf == curbuf && (switchwin.sw_curwin != عدم
					   || save_curbuf.br_buf == عدم))
	    // Using an existing window for the buffer, adjust the cursor
	    // position.
	    py_fix_cursor((linenr_T)lo, (linenr_T)hi, (linenr_T)extra);

	// END of region without "return".
	restore_win_for_buf(&switchwin, &save_curbuf);

	ܐܢ (VimTryEnd())
	    ܦܢܐ FAIL;

	ܐܢ (len_change)
	    *len_change = new_len - old_len;

	ܦܢܐ OK;
    }
    ܐܚܪܢܐ
    {
	PyErr_BadArgument();
	ܦܢܐ FAIL;
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
    ساكن صحيح
InsertBufferLines(buf_T *buf, PyInt n, PyObject *lines, PyInt *len_change)
{
    bufref_T	save_curbuf = {عدم, 0, 0};
    switchwin_T	switchwin;

    // First of all, we check the type of the supplied Python object.
    // It must be a string or a list, or the call is in error.
    ܐܢ (PyBytes_Check(lines) || PyUnicode_Check(lines))
    {
	حرف		*str = StringToLine(lines);

	ܐܢ (str == عدم)
	    ܦܢܐ FAIL;

	PyErr_Clear();
	VimTryStart();
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	ܐܢ (u_save((linenr_T)n, (linenr_T)(n + 1)) == FAIL)
	    RAISE_UNDO_FAIL;
	ܐܚܪܢܐ ܐܢ (ml_append((linenr_T)n, (char_u *)str, 0, FALSE) == FAIL)
	    RAISE_INSERT_LINE_FAIL;
	ܐܚܪܢܐ ܐܢ (save_curbuf.br_buf == عدم)
	    // Only adjust marks if we managed to switch to a window that
	    // holds the buffer, otherwise line numbers will be invalid.
	    appended_lines_mark((linenr_T)n, 1L);

	vim_free(str);
	restore_win_for_buf(&switchwin, &save_curbuf);
	update_screen(UPD_VALID);

	ܐܢ (VimTryEnd())
	    ܦܢܐ FAIL;

	ܐܢ (len_change)
	    *len_change = 1;

	ܦܢܐ OK;
    }
    ܐܚܪܢܐ ܐܢ (PyList_Check(lines))
    {
	PyInt	i;
	PyInt	size = PyList_Size(lines);
	حرف	**array;

	array = PyMem_New(حرف *, size);
	ܐܢ (array == عدم)
	{
	    PyErr_NoMemory();
	    ܦܢܐ FAIL;
	}

	لكل (i = 0; i < size; ++i)
	{
	    PyObject *line;

	    ܐܢ (!(line = PyList_GetItem(lines, i)) ||
		!(array[i] = StringToLine(line)))
	    {
		ܟܡܐ (i)
		    vim_free(array[--i]);
		PyMem_Free(array);
		ܦܢܐ FAIL;
	    }
	}

	PyErr_Clear();
	VimTryStart();
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	ܐܢ (u_save((linenr_T)n, (linenr_T)(n + 1)) == FAIL)
	    RAISE_UNDO_FAIL;
	ܐܚܪܢܐ
	{
	    لكل (i = 0; i < size; ++i)
	    {
		ܐܢ (ml_append((linenr_T)(n + i),
					(char_u *)array[i], 0, FALSE) == FAIL)
		{
		    RAISE_INSERT_LINE_FAIL;

		    // Free the rest of the lines
		    ܟܡܐ (i < size)
			vim_free(array[i++]);

		    اكسر;
		}
		vim_free(array[i]);
	    }
	    ܐܢ (i > 0 && save_curbuf.br_buf == عدم)
		// Only adjust marks if we managed to switch to a window that
		// holds the buffer, otherwise line numbers will be invalid.
		appended_lines_mark((linenr_T)n, (long)i);
	}

	// Free the array of lines. All of its contents have now
	// been freed.
	PyMem_Free(array);
	restore_win_for_buf(&switchwin, &save_curbuf);

	update_screen(UPD_VALID);

	ܐܢ (VimTryEnd())
	    ܦܢܐ FAIL;

	ܐܢ (len_change)
	    *len_change = size;

	ܦܢܐ OK;
    }
    ܐܚܪܢܐ
    {
	PyErr_BadArgument();
	ܦܢܐ FAIL;
    }
}

/*
 * Common routines for buffers and line ranges
 * -------------------------------------------
 */

تعريفنوع بنية
{
    PyObject_HEAD
    buf_T *buf;
} BufferObject;

    ساكن صحيح
CheckBuffer(BufferObject *self)
{
    ܐܢ (self->buf == INVALID_BUFFER_VALUE)
    {
	PyErr_SET_VIM(N_("attempt to refer to deleted buffer"));
	ܦܢܐ -1;
    }

    ܦܢܐ 0;
}

    ساكن صحيح
CheckBufferCb(فراغ *self)
{
    ܦܢܐ CheckBuffer((BufferObject*)self);
}

    ساكن PyObject *
RBItem(BufferObject *self, PyInt n, PyInt start, PyInt end)
{
    ܐܢ (CheckBuffer(self))
	ܦܢܐ عدم;

    ܐܢ (end == -1)
	end = self->buf->b_ml.ml_line_count;

    ܐܢ (n < 0)
	n += end - start + 1;

    ܐܢ (n < 0 || n > end - start)
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("line number out of range"));
	ܦܢܐ عدم;
    }

    ܦܢܐ GetBufferLine(self->buf, n+start);
}

    ساكن PyObject *
RBSlice(BufferObject *self, PyInt lo, PyInt hi, PyInt start, PyInt end)
{
    PyInt size;

    ܐܢ (CheckBuffer(self))
	ܦܢܐ عدم;

    ܐܢ (end == -1)
	end = self->buf->b_ml.ml_line_count;

    size = end - start + 1;

    ܐܢ (lo < 0)
	lo = 0;
    ܐܚܪܢܐ ܐܢ (lo > size)
	lo = size;
    ܐܢ (hi < 0)
	hi = 0;
    ܐܢ (hi < lo)
	hi = lo;
    ܐܚܪܢܐ ܐܢ (hi > size)
	hi = size;

    ܦܢܐ GetBufferLineList(self->buf, lo+start, hi+start);
}

    ساكن PyInt
RBAsItem(
	BufferObject *self,
	PyInt n,
	PyObject *valObject,
	PyInt start,
	PyInt end,
	PyInt *new_end)
{
    PyInt len_change;

    ܐܢ (CheckBuffer(self))
	ܦܢܐ -1;

    ܐܢ (end == -1)
	end = self->buf->b_ml.ml_line_count;

    ܐܢ (n < 0)
	n += end - start + 1;

    ܐܢ (n < 0 || n > end - start)
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("line number out of range"));
	ܦܢܐ -1;
    }

    ܐܢ (SetBufferLine(self->buf, n+start, valObject, &len_change) == FAIL)
	ܦܢܐ -1;

    ܐܢ (new_end)
	*new_end = end + len_change;

    ܦܢܐ 0;
}

    ساكن PyInt
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
    ܐܢ (CheckBuffer(self))
	ܦܢܐ -1;

    ܐܢ (end == -1)
	end = self->buf->b_ml.ml_line_count;

    // Sort out the slice range
    size = end - start + 1;

    ܐܢ (lo < 0)
	lo = 0;
    ܐܚܪܢܐ ܐܢ (lo > size)
	lo = size;
    ܐܢ (hi < 0)
	hi = 0;
    ܐܢ (hi < lo)
	hi = lo;
    ܐܚܪܢܐ ܐܢ (hi > size)
	hi = size;

    ܐܢ (SetBufferLineList(self->buf, lo + start, hi + start,
						valObject, &len_change) == FAIL)
	ܦܢܐ -1;

    ܐܢ (new_end)
	*new_end = end + len_change;

    ܦܢܐ 0;
}


    ساكن PyObject *
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

    ܐܢ (CheckBuffer(self))
	ܦܢܐ عدم;

    ܐܢ (end == -1)
	end = self->buf->b_ml.ml_line_count;

    max = n = end - start + 1;

    ܐܢ (!PyArg_ParseTuple(args, "O|n", &lines, &n))
	ܦܢܐ عدم;

    ܐܢ (n < 0 || n > max)
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("line number out of range"));
	ܦܢܐ عدم;
    }

    ܐܢ (InsertBufferLines(self->buf, n + start - 1, lines, &len_change) == FAIL)
	ܦܢܐ عدم;

    ܐܢ (new_end)
	*new_end = end + len_change;

    Py_INCREF(Py_None);
    ܦܢܐ Py_None;
}

// Range object

DEFINE_PY_TYPE_OBJECT(RangeType);
ساكن PySequenceMethods RangeAsSeq;
ساكن PyMappingMethods RangeAsMapping;

تعريفنوع بنية
{
    PyObject_HEAD
    BufferObject *buf;
    PyInt start;
    PyInt end;
} RangeObject;

    ساكن PyObject *
RangeNew(buf_T *buf, PyInt start, PyInt end)
{
    BufferObject *bufr;
    RangeObject *self;
    self = PyObject_GC_New(RangeObject, RangeTypePtr);
    ܐܢ (self == عدم)
	ܦܢܐ عدم;

    bufr = (BufferObject *)BufferNew(buf);
    ܐܢ (bufr == عدم)
    {
	Py_DECREF(self);
	ܦܢܐ عدم;
    }
    Py_INCREF((PyObject *)bufr);

    self->buf = bufr;
    self->start = start;
    self->end = end;

    ܦܢܐ (PyObject *)(self);
}

    ساكن فراغ
RangeDestructor(PyObject *self_obj)
{
    RangeObject *self = (RangeObject*)self_obj;
    PyObject_GC_UnTrack((فراغ *)(self));
    Py_XDECREF(((PyObject *)(self->buf)));
    PyObject_GC_Del((فراغ *)(self));
}

    ساكن صحيح
RangeTraverse(PyObject *self_obj, visitproc visit, فراغ *arg)
{
    RangeObject *self = (RangeObject*)self_obj;
    Py_VISIT(((PyObject *)(self->buf)));
    ܦܢܐ 0;
}

    ساكن صحيح
RangeClear(PyObject *self_obj)
{
    RangeObject *self = (RangeObject*)self_obj;
    Py_CLEAR(self->buf);
    ܦܢܐ 0;
}

    ساكن PyInt
RangeLength(RangeObject *self)
{
    // HOW DO WE SIGNAL AN ERROR FROM THIS FUNCTION?
    ܐܢ (CheckBuffer(self->buf))
	ܦܢܐ -1; // ???

    ܦܢܐ (self->end - self->start + 1);
}

    ساكن PyObject *
RangeItem(RangeObject *self, PyInt n)
{
    ܦܢܐ RBItem(self->buf, n, self->start, self->end);
}

    ساكن PyObject *
RangeSlice(RangeObject *self, PyInt lo, PyInt hi)
{
    ܦܢܐ RBSlice(self->buf, lo, hi, self->start, self->end);
}

ساكن حرف *RangeAttrs[] = {
    "start", "end",
    عدم
};

    ساكن PyObject *
RangeDir(PyObject *self, PyObject *args UNUSED)
{
    ܦܢܐ ObjectDir(self, RangeAttrs);
}

    ساكن PyObject *
RangeAppend(RangeObject *self, PyObject *args)
{
    ܦܢܐ RBAppend(self->buf, args, self->start, self->end, &self->end);
}

    ساكن PyObject *
RangeRepr(PyObject *self_obj)
{
    RangeObject *self = (RangeObject*)self_obj;
    ܐܢ (self->buf->buf == INVALID_BUFFER_VALUE)
	ܦܢܐ PyString_FromFormat("<range object (for deleted buffer) at %p>",
				    (فراغ *)self);
    ܐܚܪܢܐ
    {
	حرف *name = (حرف *)self->buf->buf->b_fname;

	ܐܢ (name == عدم)
	    name = "";

	ܦܢܐ PyString_FromFormat("<range %s (%d:%d)>",
				    name, (صحيح)self->start, (صحيح)self->end);
    }
}

ساكن بنية PyMethodDef RangeMethods[] = {
    // name,	function,			calling,	documentation
    {"append",	(PyCFunction)RangeAppend,	METH_VARARGS,	"Append data to the Vim range" },
    {"__dir__",	(PyCFunction)RangeDir,		METH_NOARGS,	""},
    { عدم,	عدم,				0,		عدم}
};

DEFINE_PY_TYPE_OBJECT(BufferType);
ساكن PySequenceMethods BufferAsSeq;
ساكن PyMappingMethods BufferAsMapping;

    ساكن PyObject *
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

    ܐܢ (BUF_PYTHON_REF(buf) != عدم)
    {
	self = BUF_PYTHON_REF(buf);
	Py_INCREF((PyObject *)self);
    }
    ܐܚܪܢܐ
    {
	self = PyObject_NEW(BufferObject, BufferTypePtr);
	ܐܢ (self == عدم)
	    ܦܢܐ عدم;
	self->buf = buf;
	BUF_PYTHON_REF(buf) = self;
    }

    ܦܢܐ (PyObject *)(self);
}

    ساكن فراغ
BufferDestructor(PyObject *self_obj)
{
    BufferObject *self = (BufferObject*)self_obj;
    ܐܢ (self->buf && self->buf != INVALID_BUFFER_VALUE)
	BUF_PYTHON_REF(self->buf) = عدم;

    DESTRUCTOR_FINISH(self);
}

    ساكن PyInt
BufferLength(BufferObject *self)
{
    // HOW DO WE SIGNAL AN ERROR FROM THIS FUNCTION?
    ܐܢ (CheckBuffer(self))
	ܦܢܐ -1; // ???

    ܦܢܐ (PyInt)(self->buf->b_ml.ml_line_count);
}

    ساكن PyObject *
BufferItem(BufferObject *self, PyInt n)
{
    ܦܢܐ RBItem(self, n, 1, -1);
}

    ساكن PyObject *
BufferSlice(BufferObject *self, PyInt lo, PyInt hi)
{
    ܦܢܐ RBSlice(self, lo, hi, 1, -1);
}

ساكن حرف *BufferAttrs[] = {
    "name", "number", "vars", "options", "valid",
    عدم
};

    ساكن PyObject *
BufferDir(PyObject *self, PyObject *args UNUSED)
{
    ܦܢܐ ObjectDir(self, BufferAttrs);
}

    ساكن PyObject *
BufferAttrValid(BufferObject *self, حرف *name)
{
    PyObject	*ret;

    ܐܢ (strcmp(name, "valid") != 0)
	ܦܢܐ عدم;

    ret = ((self->buf == INVALID_BUFFER_VALUE) ? Py_False : Py_True);
    Py_INCREF(ret);
    ܦܢܐ ret;
}

    ساكن PyObject *
BufferAttr(BufferObject *self, حرف *name)
{
    ܐܢ (strcmp(name, "name") == 0)
	ܦܢܐ PyString_FromString((self->buf->b_ffname == عدم
				    ? "" : (حرف *)self->buf->b_ffname));
    ܐܚܪܢܐ ܐܢ (strcmp(name, "number") == 0)
	ܦܢܐ Py_BuildValue(Py_ssize_t_fmt, self->buf->b_fnum);
    ܐܚܪܢܐ ܐܢ (strcmp(name, "vars") == 0)
	ܦܢܐ NEW_DICTIONARY(self->buf->b_vars);
    ܐܚܪܢܐ ܐܢ (strcmp(name, "options") == 0)
	ܦܢܐ OptionsNew(SREQ_BUF, self->buf, CheckBufferCb,
			(PyObject *) self);
    ܐܚܪܢܐ ܐܢ (strcmp(name, "__members__") == 0)
	ܦܢܐ ObjectDir(عدم, BufferAttrs);
    ܐܚܪܢܐ
	ܦܢܐ عدم;
}

    ساكن صحيح
BufferSetattr(PyObject *self_obj, حرف *name, PyObject *valObject)
{
    BufferObject *self = (BufferObject*)self_obj;
    ܐܢ (CheckBuffer(self))
	ܦܢܐ -1;

    ܐܢ (strcmp(name, "name") == 0)
    {
	char_u		*val;
	aco_save_T	aco;
	صحيح		ren_ret = OK;
	PyObject	*todecref;

	ܐܢ (!(val = StringToChars(valObject, &todecref)))
	    ܦܢܐ -1;

	VimTryStart();
	// Using aucmd_*: autocommands will be executed by rename_buffer
	aucmd_prepbuf(&aco, self->buf);
	ܐܢ (curbuf == self->buf)
	{
	    ren_ret = rename_buffer(val);
	    aucmd_restbuf(&aco);
	}
	Py_XDECREF(todecref);
	ܐܢ (VimTryEnd())
	    ܦܢܐ -1;

	ܐܢ (ren_ret == FAIL)
	{
	    PyErr_SET_VIM(N_("failed to rename buffer"));
	    ܦܢܐ -1;
	}
	ܦܢܐ 0;
    }
    ܐܚܪܢܐ
    {
	PyErr_SetString(PyExc_AttributeError, name);
	ܦܢܐ -1;
    }
}

    ساكن PyObject *
BufferAppend(BufferObject *self, PyObject *args)
{
    ܦܢܐ RBAppend(self, args, 1, -1, عدم);
}

    ساكن PyObject *
BufferMark(BufferObject *self, PyObject *pmarkObject)
{
    pos_T	*posp;
    char_u	*pmark;
    char_u	mark;
    bufref_T	savebuf;
    PyObject	*todecref;

    ܐܢ (CheckBuffer(self))
	ܦܢܐ عدم;

    ܐܢ (!(pmark = StringToChars(pmarkObject, &todecref)))
	ܦܢܐ عدم;

    ܐܢ (pmark[0] == '\0' || pmark[1] != '\0')
    {
	PyErr_SET_STRING(PyExc_ValueError,
		N_("mark name must be a single character"));
	Py_XDECREF(todecref);
	ܦܢܐ عدم;
    }

    mark = *pmark;

    Py_XDECREF(todecref);

    VimTryStart();
    switch_buffer(&savebuf, self->buf);
    posp = getmark(mark, FALSE);
    restore_buffer(&savebuf);
    ܐܢ (VimTryEnd())
	ܦܢܐ عدم;

    ܐܢ (posp == عدم)
    {
	PyErr_SET_VIM(N_("invalid mark name"));
	ܦܢܐ عدم;
    }

    ܐܢ (posp->lnum <= 0)
    {
	// Or raise an error?
	Py_INCREF(Py_None);
	ܦܢܐ Py_None;
    }

    ܦܢܐ Py_BuildValue("(ll)", (long)(posp->lnum), (long)(posp->col));
}

    ساكن PyObject *
BufferRange(BufferObject *self, PyObject *args)
{
    PyInt start;
    PyInt end;

    ܐܢ (CheckBuffer(self))
	ܦܢܐ عدم;

    ܐܢ (!PyArg_ParseTuple(args, "nn", &start, &end))
	ܦܢܐ عدم;

    ܦܢܐ RangeNew(self->buf, start, end);
}

    ساكن PyObject *
BufferRepr(PyObject *self_obj)
{
    BufferObject *self = (BufferObject*)self_obj;
    ܐܢ (self->buf == INVALID_BUFFER_VALUE)
	ܦܢܐ PyString_FromFormat("<buffer object (deleted) at %p>", (فراغ *)self);
    ܐܚܪܢܐ
    {
	حرف	*name = (حرف *)self->buf->b_fname;

	ܐܢ (name == عدم)
	    name = "";

	ܦܢܐ PyString_FromFormat("<buffer %s>", name);
    }
}

ساكن بنية PyMethodDef BufferMethods[] = {
    // name,	    function,			calling,	documentation
    {"append",	    (PyCFunction)BufferAppend,	METH_VARARGS,	"Append data to Vim buffer" },
    {"mark",	    (PyCFunction)BufferMark,	METH_O,		"Return (row,col) representing position of named mark" },
    {"range",	    (PyCFunction)BufferRange,	METH_VARARGS,	"Return a range object which represents the part of the given buffer between line numbers s and e" },
    {"__dir__",	    (PyCFunction)BufferDir,	METH_NOARGS,	""},
    { عدم,	    عدم,			0,		عدم}
};

/*
 * Buffer list object - Implementation
 */

DEFINE_PY_TYPE_OBJECT(BufMapType);

تعريفنوع بنية
{
    PyObject_HEAD
} BufMapObject;

ساكن BufMapObject TheBufferMap =
{
    PyObject_HEAD_INIT_TYPE(BufMapType)
};

    ساكن PyInt
BufMapLength(PyObject *self UNUSED)
{
    buf_T	*b = firstbuf;
    PyInt	n = 0;

    ܟܡܐ (b)
    {
	++n;
	b = b->b_next;
    }

    ܦܢܐ n;
}

    ساكن PyObject *
BufMapItem(PyObject *self UNUSED, PyObject *keyObject)
{
    buf_T	*b;
    long	bnr;

    ܐܢ (NumberToLong(keyObject, &bnr, NUMBER_INT|NUMBER_NATURAL))
	ܦܢܐ عدم;

    b = buflist_findnr((صحيح) bnr);

    ܐܢ (b)
	ܦܢܐ BufferNew(b);
    ܐܚܪܢܐ
    {
	PyErr_SetObject(PyExc_KeyError, keyObject);
	ܦܢܐ عدم;
    }
}

    ساكن فراغ
BufMapIterDestruct(فراغ* arg)
{
    PyObject *buffer = (PyObject*)arg;
    // Iteration was stopped before all buffers were processed
    ܐܢ (buffer)
    {
	Py_DECREF(buffer);
    }
}

    ساكن صحيح
BufMapIterTraverse(فراغ *iter, visitproc visit, فراغ *arg)
{
    PyObject *buffer = (PyObject*)iter;
    ܐܢ (buffer)
	Py_VISIT(buffer);
    ܦܢܐ 0;
}

    ساكن صحيح
BufMapIterClear(فراغ **iter)
{
    PyObject **buffer = (PyObject**)iter;
    ܐܢ (*buffer)
	Py_CLEAR(*buffer);
    ܦܢܐ 0;
}

    ساكن PyObject *
BufMapIterNext(فراغ **arg)
{
    PyObject	*next;
    PyObject	*ret;
    PyObject	**buffer = (PyObject**)arg;

    ܐܢ (!*buffer)
	ܦܢܐ عدم;

    ret = *buffer;

    ܐܢ (CheckBuffer((BufferObject *)(ret)))
    {
	*buffer = عدم;
	ܦܢܐ عدم;
    }

    ܐܢ (!((BufferObject *)(ret))->buf->b_next)
	next = عدم;
    ܐܚܪܢܐ ܐܢ (!(next = BufferNew(((BufferObject *)(ret))->buf->b_next)))
	ܦܢܐ عدم;
    *buffer = next;
    // Do not increment reference: we no longer hold it (decref), but whoever
    // on other side will hold (incref). Decref+incref = nothing.
    ܦܢܐ ret;
}

    ساكن PyObject *
BufMapIter(PyObject *self)
{
    PyObject *buffer;

    buffer = BufferNew(firstbuf);
    ܦܢܐ IterNew(buffer,
	    BufMapIterDestruct, BufMapIterNext,
	    BufMapIterTraverse, BufMapIterClear,
	    self);
}

ساكن PyMappingMethods BufMapAsMapping = {
    (lenfunc)       BufMapLength,
    (binaryfunc)    BufMapItem,
    (objobjargproc) 0,
};

// Current items object

ساكن حرف *CurrentAttrs[] = {
    "buffer", "window", "line", "range", "tabpage",
    عدم
};

    ساكن PyObject *
CurrentDir(PyObject *self, PyObject *args UNUSED)
{
    ܦܢܐ ObjectDir(self, CurrentAttrs);
}

    ساكن PyObject *
CurrentGetattr(PyObject *self UNUSED, حرف *name)
{
    ܐܢ (strcmp(name, "buffer") == 0)
	ܦܢܐ (PyObject *)BufferNew(curbuf);
    ܐܚܪܢܐ ܐܢ (strcmp(name, "window") == 0)
	ܦܢܐ (PyObject *)WindowNew(curwin, curtab);
    ܐܚܪܢܐ ܐܢ (strcmp(name, "tabpage") == 0)
	ܦܢܐ (PyObject *)TabPageNew(curtab);
    ܐܚܪܢܐ ܐܢ (strcmp(name, "line") == 0)
	ܦܢܐ GetBufferLine(curbuf, (PyInt)curwin->w_cursor.lnum);
    ܐܚܪܢܐ ܐܢ (strcmp(name, "range") == 0)
	ܦܢܐ RangeNew(curbuf, RangeStart, RangeEnd);
    ܐܚܪܢܐ ܐܢ (strcmp(name, "__members__") == 0)
	ܦܢܐ ObjectDir(عدم, CurrentAttrs);
    ܐܚܪܢܐ
#ܐܢ PY_MAJOR_VERSION < 3
	ܦܢܐ Py_FindMethod(WindowMethods, self, name);
#ܐܚܪܢܐ
	ܦܢܐ عدم;
#endif
}

    ساكن صحيح
CurrentSetattr(PyObject *self UNUSED, حرف *name, PyObject *valObject)
{
    ܐܢ (strcmp(name, "line") == 0)
    {
	ܐܢ (SetBufferLine(curbuf, (PyInt)curwin->w_cursor.lnum, valObject,
			  عدم) == FAIL)
	    ܦܢܐ -1;

	ܦܢܐ 0;
    }
    ܐܚܪܢܐ ܐܢ (strcmp(name, "buffer") == 0)
    {
	صحيح count;

	ܐܢ (valObject->ob_type != BufferTypePtr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.Buffer object, but got %s"),
		    valObject);
	    ܦܢܐ -1;
	}

	ܐܢ (CheckBuffer((BufferObject *)(valObject)))
	    ܦܢܐ -1;
	count = ((BufferObject *)(valObject))->buf->b_fnum;

	VimTryStart();
	ܐܢ (do_buffer(DOBUF_GOTO, DOBUF_FIRST, FORWARD, count, 0) == FAIL)
	{
	    ܐܢ (VimTryEnd())
		ܦܢܐ -1;
	    PyErr_VIM_FORMAT(N_("failed to switch to buffer %d"), count);
	    ܦܢܐ -1;
	}

	ܦܢܐ VimTryEnd();
    }
    ܐܚܪܢܐ ܐܢ (strcmp(name, "window") == 0)
    {
	صحيح count;

	ܐܢ (valObject->ob_type != WindowTypePtr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.Window object, but got %s"),
		    valObject);
	    ܦܢܐ -1;
	}

	ܐܢ (CheckWindow((WindowObject *)(valObject)))
	    ܦܢܐ -1;
	count = get_win_number(((WindowObject *)(valObject))->win, firstwin);

	ܐܢ (!count)
	{
	    PyErr_SET_STRING(PyExc_ValueError,
		    N_("failed to find window in the current tab page"));
	    ܦܢܐ -1;
	}

	VimTryStart();
	win_goto(((WindowObject *)(valObject))->win);
	ܐܢ (((WindowObject *)(valObject))->win != curwin)
	{
	    ܐܢ (VimTryEnd())
		ܦܢܐ -1;
	    PyErr_SET_STRING(PyExc_RuntimeError,
		    N_("did not switch to the specified window"));
	    ܦܢܐ -1;
	}

	ܦܢܐ VimTryEnd();
    }
    ܐܚܪܢܐ ܐܢ (strcmp(name, "tabpage") == 0)
    {
	ܐܢ (valObject->ob_type != TabPageTypePtr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.TabPage object, but got %s"),
		    valObject);
	    ܦܢܐ -1;
	}

	ܐܢ (CheckTabPage((TabPageObject *)(valObject)))
	    ܦܢܐ -1;

	VimTryStart();
	goto_tabpage_tp(((TabPageObject *)(valObject))->tab, TRUE, TRUE);
	ܐܢ (((TabPageObject *)(valObject))->tab != curtab)
	{
	    ܐܢ (VimTryEnd())
		ܦܢܐ -1;
	    PyErr_SET_STRING(PyExc_RuntimeError,
		    N_("did not switch to the specified tab page"));
	    ܦܢܐ -1;
	}

	ܦܢܐ VimTryEnd();
    }
    ܐܚܪܢܐ
    {
	PyErr_SetString(PyExc_AttributeError, name);
	ܦܢܐ -1;
    }
}

ساكن بنية PyMethodDef CurrentMethods[] = {
    // name,	    function,			calling,	documentation
    {"__dir__",	    (PyCFunction)CurrentDir,	METH_NOARGS,	""},
    { عدم,	    عدم,			0,		عدم}
};

    ساكن فراغ
init_range_cmd(فراغ *arg)
{
    exarg_T *eap = (exarg_T*)arg;
    RangeStart = eap->line1;
    RangeEnd = eap->line2;
}

    ساكن فراغ
init_range_eval(فراغ *rettv UNUSED)
{
    RangeStart = (PyInt) curwin->w_cursor.lnum;
    RangeEnd = RangeStart;
}

    ساكن فراغ
run_cmd(ثابت حرف *cmd, dict_T* locals UNUSED, فراغ *arg UNUSED
#ifdef PY_CAN_RECURSE
	, PyGILState_STATE *pygilstate UNUSED
#endif
	)
{
    PyObject	*run_ret;
    run_ret = PyRun_String((حرف *)cmd, Py_file_input, globals, globals);
    ܐܢ (run_ret != عدم)
    {
	Py_DECREF(run_ret);
    }
    ܐܚܪܢܐ ܐܢ (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
    {
	emsg(_(e_cant_handle_systemexit_of_python_exception_in_vim));
	PyErr_Clear();
    }
    ܐܚܪܢܐ
	PyErr_PrintEx(1);
}

ساكن ثابت حرف	*code_hdr = "def " DOPY_FUNC "(line, linenr):\n ";
ساكن صحيح		code_hdr_len = 30;

    ساكن فراغ
run_do(ثابت حرف *cmd, dict_T* locals UNUSED, فراغ *arg UNUSED
#ifdef PY_CAN_RECURSE
	, PyGILState_STATE *pygilstate
#endif
	)
{
    PyInt	lnum;
    size_t	len;
    حرف	*code;
    صحيح		status;
    PyObject	*pyfunc, *pymain;
    PyObject	*run_ret;
    buf_T	*was_curbuf = curbuf;

    ܐܢ (u_save((linenr_T)RangeStart - 1, (linenr_T)RangeEnd + 1) != OK)
    {
	emsg(_("cannot save undo information"));
	ܦܢܐ;
    }

    len = code_hdr_len + STRLEN(cmd);
    code = PyMem_New(حرف, len + 1);
    memcpy(code, code_hdr, code_hdr_len);
    STRCPY(code + code_hdr_len, cmd);
    run_ret = PyRun_String(code, Py_file_input, globals, globals);
    status = -1;
    ܐܢ (run_ret != عدم)
    {
	status = 0;
	Py_DECREF(run_ret);
    }
    ܐܚܪܢܐ ܐܢ (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
    {
	PyMem_Free(code);
	emsg(_(e_cant_handle_systemexit_of_python_exception_in_vim));
	PyErr_Clear();
	ܦܢܐ;
    }
    ܐܚܪܢܐ
	PyErr_PrintEx(1);

    PyMem_Free(code);

    ܐܢ (status)
    {
	emsg(_("failed to run the code"));
	ܦܢܐ;
    }

    status = 0;
    pymain = PyImport_AddModule("__main__");
    pyfunc = PyObject_GetAttrString(pymain, DOPY_FUNC);
#ifdef PY_CAN_RECURSE
    PyGILState_Release(*pygilstate);
#endif

    لكل (lnum = RangeStart; lnum <= RangeEnd; ++lnum)
    {
	PyObject	*line;
	PyObject	*linenr;
	PyObject	*ret;

#ifdef PY_CAN_RECURSE
	*pygilstate = PyGILState_Ensure();
#endif
	// Check the line number, the command my have deleted lines.
	ܐܢ (lnum > curbuf->b_ml.ml_line_count
		|| !(line = GetBufferLine(curbuf, lnum)))
	    اذهب err;
	ܐܢ (!(linenr = PyInt_FromLong((long) lnum)))
	{
	    Py_DECREF(line);
	    اذهب err;
	}
	ret = PyObject_CallFunctionObjArgs(pyfunc, line, linenr, عدم);
	Py_DECREF(line);
	Py_DECREF(linenr);
	ܐܢ (!ret)
	    اذهب err;

	// Check that the command didn't switch to another buffer.
	// Check the line number, the command my have deleted lines.
	ܐܢ (curbuf != was_curbuf || lnum > curbuf->b_ml.ml_line_count)
	{
	    Py_XDECREF(ret);
	    اذهب err;
	}

	ܐܢ (ret != Py_None)
	    ܐܢ (SetBufferLine(curbuf, lnum, ret, عدم) == FAIL)
	    {
		Py_XDECREF(ret);
		اذهب err;
	    }

	Py_XDECREF(ret);
	PythonIO_Flush();
#ifdef PY_CAN_RECURSE
	PyGILState_Release(*pygilstate);
#endif
    }
    اذهب out;
err:
#ifdef PY_CAN_RECURSE
    *pygilstate = PyGILState_Ensure();
#endif
    PyErr_PrintEx(0);
    PythonIO_Flush();
    status = 1;
out:
#ifdef PY_CAN_RECURSE
    ܐܢ (!status)
	*pygilstate = PyGILState_Ensure();
#endif
    Py_DECREF(pyfunc);
    PyObject_SetAttrString(pymain, DOPY_FUNC, عدم);
    ܐܢ (status)
	ܦܢܐ;
    check_cursor();
    update_curbuf(UPD_NOT_VALID);
}

    ساكن فراغ
run_eval(ثابت حرف *cmd, dict_T *locals, فراغ *arg
#ifdef PY_CAN_RECURSE
	, PyGILState_STATE *pygilstate UNUSED
#endif
	)
{
    PyObject	*run_ret;
    typval_T	*rettv = (typval_T*)arg;
    PyObject	*pylocals = locals ? NEW_DICTIONARY(locals) : globals;

    run_ret = PyRun_String((حرف *)cmd, Py_eval_input, globals, pylocals);
    ܐܢ (run_ret == عدم)
    {
	ܐܢ (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
	{
	    emsg(_(e_cant_handle_systemexit_of_python_exception_in_vim));
	    PyErr_Clear();
	}
	ܐܚܪܢܐ
	{
	    ܐܢ (PyErr_Occurred() && !msg_silent)
		PyErr_PrintEx(0);
	    emsg(_(e_eval_did_not_return_valid_python_object));
	}
    }
    ܐܚܪܢܐ
    {
	ܐܢ (ConvertFromPyObject(run_ret, rettv) == -1)
	    emsg(_(e_failed_to_convert_returned_python_object_to_vim_value));
	Py_DECREF(run_ret);
    }
    PyErr_Clear();
}

    ساكن صحيح
set_ref_in_py(ثابت صحيح copyID)
{
    pylinkedlist_T	*cur;
    list_T		*ll;
    tuple_T		*tt;
    صحيح			i;
    صحيح			abort = FALSE;
    FunctionObject	*func;

    ܐܢ (lastdict != عدم)
    {
	لكل (cur = lastdict ; !abort && cur != عدم ; cur = cur->pll_prev)
	    abort = set_ref_in_dict(((DictionaryObject *)(cur->pll_obj))->dict,
								       copyID);
    }

    ܐܢ (lastlist != عدم)
    {
	لكل (cur = lastlist ; !abort && cur != عدم ; cur = cur->pll_prev)
	{
	    ll = ((ListObject *) (cur->pll_obj))->list;
	    abort = set_ref_in_list(ll, copyID);
	}
    }

    ܐܢ (lasttuple != عدم)
    {
	لكل (cur = lasttuple ; !abort && cur != عدم ; cur = cur->pll_prev)
	{
	    tt = ((TupleObject *) (cur->pll_obj))->tuple;
	    abort = set_ref_in_tuple(tt, copyID);
	}
    }

    ܐܢ (lastfunc != عدم)
    {
	لكل (cur = lastfunc ; !abort && cur != عدم ; cur = cur->pll_prev)
	{
	    func = (FunctionObject *) cur->pll_obj;
	    abort = set_ref_in_dict(func->self, copyID);
	    ܐܢ (func->argc)
		لكل (i = 0; !abort && i < func->argc; ++i)
		    abort = abort
			|| set_ref_in_item(&func->argv[i], copyID, عدم, عدم, عدم);
	}
    }

    ܦܢܐ abort;
}

    ساكن صحيح
set_string_copy(char_u *str, typval_T *tv)
{
    tv->vval.v_string = vim_strsave(str);
    ܐܢ (tv->vval.v_string == عدم)
    {
	PyErr_NoMemory();
	ܦܢܐ -1;
    }
    ܦܢܐ 0;
}

    ساكن صحيح
pydict_to_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    dict_T	*dict;
    char_u	*key;
    dictitem_T	*di;
    PyObject	*keyObject;
    PyObject	*valObject;
    Py_ssize_t	iter = 0;

    ܐܢ (!(dict = py_dict_alloc()))
	ܦܢܐ -1;

    tv->v_type = VAR_DICT;
    tv->vval.v_dict = dict;

    ܟܡܐ (PyDict_Next(obj, &iter, &keyObject, &valObject))
    {
	PyObject	*todecref = عدم;

	ܐܢ (keyObject == عدم || valObject == عدم)
	{
	    dict_unref(dict);
	    ܦܢܐ -1;
	}

	ܐܢ (!(key = StringToChars(keyObject, &todecref)))
	{
	    dict_unref(dict);
	    ܦܢܐ -1;
	}

	ܐܢ (*key == NUL)
	{
	    dict_unref(dict);
	    Py_XDECREF(todecref);
	    RAISE_NO_EMPTY_KEYS;
	    ܦܢܐ -1;
	}

	di = dictitem_alloc(key);

	Py_XDECREF(todecref);

	ܐܢ (di == عدم)
	{
	    PyErr_NoMemory();
	    dict_unref(dict);
	    ܦܢܐ -1;
	}

	ܐܢ (_ConvertFromPyObject(valObject, &di->di_tv, lookup_dict) == -1)
	{
	    vim_free(di);
	    dict_unref(dict);
	    ܦܢܐ -1;
	}

	ܐܢ (dict_add(dict, di) == FAIL)
	{
	    RAISE_KEY_ADD_FAIL(di->di_key);
	    clear_tv(&di->di_tv);
	    vim_free(di);
	    dict_unref(dict);
	    ܦܢܐ -1;
	}
    }

    --dict->dv_refcount;
    ܦܢܐ 0;
}

    ساكن صحيح
pymap_to_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    dict_T	*dict;
    char_u	*key;
    dictitem_T	*di;
    PyObject	*list;
    PyObject	*iterator;
    PyObject	*keyObject;
    PyObject	*valObject;

    ܐܢ (!(dict = py_dict_alloc()))
	ܦܢܐ -1;

    tv->v_type = VAR_DICT;
    tv->vval.v_dict = dict;

    ܐܢ (!(list = PyMapping_Keys(obj)))
    {
	dict_unref(dict);
	ܦܢܐ -1;
    }

    ܐܢ (!(iterator = PyObject_GetIter(list)))
    {
	dict_unref(dict);
	Py_DECREF(list);
	ܦܢܐ -1;
    }
    Py_DECREF(list);

    ܟܡܐ ((keyObject = PyIter_Next(iterator)))
    {
	PyObject	*todecref;

	ܐܢ (!(key = StringToChars(keyObject, &todecref)))
	{
	    Py_DECREF(keyObject);
	    Py_DECREF(iterator);
	    dict_unref(dict);
	    ܦܢܐ -1;
	}

	ܐܢ (*key == NUL)
	{
	    Py_DECREF(keyObject);
	    Py_DECREF(iterator);
	    Py_XDECREF(todecref);
	    dict_unref(dict);
	    RAISE_NO_EMPTY_KEYS;
	    ܦܢܐ -1;
	}

	ܐܢ (!(valObject = PyObject_GetItem(obj, keyObject)))
	{
	    Py_DECREF(keyObject);
	    Py_DECREF(iterator);
	    Py_XDECREF(todecref);
	    dict_unref(dict);
	    ܦܢܐ -1;
	}

	di = dictitem_alloc(key);

	Py_DECREF(keyObject);
	Py_XDECREF(todecref);

	ܐܢ (di == عدم)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(valObject);
	    dict_unref(dict);
	    PyErr_NoMemory();
	    ܦܢܐ -1;
	}

	ܐܢ (_ConvertFromPyObject(valObject, &di->di_tv, lookup_dict) == -1)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(valObject);
	    vim_free(di);
	    dict_unref(dict);
	    ܦܢܐ -1;
	}

	Py_DECREF(valObject);

	ܐܢ (dict_add(dict, di) == FAIL)
	{
	    RAISE_KEY_ADD_FAIL(di->di_key);
	    Py_DECREF(iterator);
	    dictitem_free(di);
	    dict_unref(dict);
	    ܦܢܐ -1;
	}
    }
    Py_DECREF(iterator);
    --dict->dv_refcount;
    ܦܢܐ 0;
}

    ساكن صحيح
pyseq_to_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    list_T	*l;

    ܐܢ (!(l = py_list_alloc()))
	ܦܢܐ -1;

    tv->v_type = VAR_LIST;
    tv->vval.v_list = l;

    ܐܢ (list_py_concat(l, obj, lookup_dict) == -1)
    {
	list_unref(l);
	ܦܢܐ -1;
    }

    --l->lv_refcount;
    ܦܢܐ 0;
}

    ساكن صحيح
pytuple_to_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    tuple_T	*t;

    ܐܢ (!(t = py_tuple_alloc()))
	ܦܢܐ -1;

    tv->v_type = VAR_TUPLE;
    tv->vval.v_tuple = t;

    ܐܢ (tuple_py_concat(t, obj, lookup_dict) == -1)
    {
	tuple_unref(t);
	ܦܢܐ -1;
    }

    --t->tv_refcount;
    ܦܢܐ 0;
}

تعريفنوع صحيح (*pytotvfunc)(PyObject *, typval_T *, PyObject *);

    ساكن صحيح
convert_dl(PyObject *obj, typval_T *tv,
				    pytotvfunc py_to_tv, PyObject *lookup_dict)
{
    PyObject	*capsule;
    حرف	hexBuf[حجم(فراغ *) * 2 + 3];

    sprintf(hexBuf, "%p", (فراغ *)obj);

#ifdef PY_USE_CAPSULE
    capsule = PyDict_GetItemString(lookup_dict, hexBuf);
#ܐܚܪܢܐ
    capsule = (PyObject *)PyDict_GetItemString(lookup_dict, hexBuf);
#endif
    ܐܢ (capsule == عدم)
    {
#ifdef PY_USE_CAPSULE
	capsule = PyCapsule_New(tv, عدم, عدم);
#ܐܚܪܢܐ
	capsule = PyCObject_FromVoidPtr(tv, عدم);
#endif
	ܐܢ (PyDict_SetItemString(lookup_dict, hexBuf, capsule))
	{
	    Py_DECREF(capsule);
	    tv->v_type = VAR_UNKNOWN;
	    ܦܢܐ -1;
	}

	Py_DECREF(capsule);

	ܐܢ (py_to_tv(obj, tv, lookup_dict) == -1)
	{
	    tv->v_type = VAR_UNKNOWN;
	    ܦܢܐ -1;
	}
	// As we are not using copy_tv which increments reference count we must
	// do it ourself.
	ܐܢ (tv->v_type == VAR_DICT)
	    ++tv->vval.v_dict->dv_refcount;
	ܐܚܪܢܐ ܐܢ (tv->v_type == VAR_LIST)
	    ++tv->vval.v_list->lv_refcount;
	ܐܚܪܢܐ ܐܢ (tv->v_type == VAR_TUPLE)
	    ++tv->vval.v_tuple->tv_refcount;
    }
    ܐܚܪܢܐ
    {
	typval_T	*v;

#ifdef PY_USE_CAPSULE
	v = PyCapsule_GetPointer(capsule, عدم);
#ܐܚܪܢܐ
	v = PyCObject_AsVoidPtr(capsule);
#endif
	copy_tv(v, tv);
    }
    ܦܢܐ 0;
}

    ساكن صحيح
ConvertFromPyMapping(PyObject *obj, typval_T *tv)
{
    PyObject	*lookup_dict;
    صحيح		ret;

    ܐܢ (!(lookup_dict = PyDict_New()))
	ܦܢܐ -1;

    ܐܢ (PyType_IsSubtype(obj->ob_type, DictionaryTypePtr))
    {
	tv->v_type = VAR_DICT;
	tv->vval.v_dict = (((DictionaryObject *)(obj))->dict);
	++tv->vval.v_dict->dv_refcount;
	ret = 0;
    }
    ܐܚܪܢܐ ܐܢ (PyDict_Check(obj))
	ret = convert_dl(obj, tv, pydict_to_tv, lookup_dict);
    ܐܚܪܢܐ ܐܢ (PyMapping_Check(obj))
	ret = convert_dl(obj, tv, pymap_to_tv, lookup_dict);
    ܐܚܪܢܐ
    {
	PyErr_FORMAT_TYPE(
		N_("unable to convert %s to a Vim dictionary"),
		obj);
	ret = -1;
    }
    Py_DECREF(lookup_dict);
    ܦܢܐ ret;
}

    ساكن صحيح
ConvertFromPySequence(PyObject *obj, typval_T *tv)
{
    PyObject	*lookup_dict;
    صحيح		ret;

    ܐܢ (!(lookup_dict = PyDict_New()))
	ܦܢܐ -1;

    ܐܢ (PyType_IsSubtype(obj->ob_type, ListTypePtr))
    {
	tv->v_type = VAR_LIST;
	tv->vval.v_list = (((ListObject *)(obj))->list);
	++tv->vval.v_list->lv_refcount;
	ret = 0;
    }
    ܐܚܪܢܐ ܐܢ (PyIter_Check(obj) || PySequence_Check(obj))
	ret = convert_dl(obj, tv, pyseq_to_tv, lookup_dict);
    ܐܚܪܢܐ
    {
	PyErr_FORMAT_TYPE(
		N_("unable to convert %s to a Vim list"),
		obj);
	ret = -1;
    }
    Py_DECREF(lookup_dict);
    ܦܢܐ ret;
}

    ساكن صحيح
ConvertFromPyObject(PyObject *obj, typval_T *tv)
{
    PyObject	*lookup_dict;
    صحيح		ret;

    ܐܢ (!(lookup_dict = PyDict_New()))
	ܦܢܐ -1;
    ret = _ConvertFromPyObject(obj, tv, lookup_dict);
    Py_DECREF(lookup_dict);
    ܦܢܐ ret;
}

    ساكن صحيح
_ConvertFromPyObject(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    ܐܢ (PyType_IsSubtype(obj->ob_type, DictionaryTypePtr))
    {
	tv->v_type = VAR_DICT;
	tv->vval.v_dict = (((DictionaryObject *)(obj))->dict);
	++tv->vval.v_dict->dv_refcount;
    }
    ܐܚܪܢܐ ܐܢ (PyType_IsSubtype(obj->ob_type, ListTypePtr))
    {
	tv->v_type = VAR_LIST;
	tv->vval.v_list = (((ListObject *)(obj))->list);
	++tv->vval.v_list->lv_refcount;
    }
    ܐܚܪܢܐ ܐܢ (PyType_IsSubtype(obj->ob_type, TupleTypePtr))
    {
	tv->v_type = VAR_TUPLE;
	tv->vval.v_tuple = (((TupleObject *)(obj))->tuple);
	++tv->vval.v_tuple->tv_refcount;
    }
    ܐܚܪܢܐ ܐܢ (PyType_IsSubtype(obj->ob_type, FunctionTypePtr))
    {
	FunctionObject *func = (FunctionObject *) obj;
	ܐܢ (func->self != عدم || func->argv != عدم)
	{
	    partial_T *pt = ALLOC_CLEAR_ONE(partial_T);

	    set_partial(func, pt, TRUE);
	    tv->vval.v_partial = pt;
	    tv->v_type = VAR_PARTIAL;
	}
	ܐܚܪܢܐ
	{
	    ܐܢ (set_string_copy(func->name, tv) == -1)
		ܦܢܐ -1;

	    tv->v_type = VAR_FUNC;
	}
	func_ref(func->name);
    }
    ܐܚܪܢܐ ܐܢ (PyBytes_Check(obj))
    {
	char_u	*str;

	ܐܢ (PyBytes_AsStringAndSize(obj, (حرف **) &str, عدم) == -1)
	    ܦܢܐ -1;
	ܐܢ (str == عدم)
	    ܦܢܐ -1;

	ܐܢ (set_string_copy(str, tv) == -1)
	    ܦܢܐ -1;

	tv->v_type = VAR_STRING;
    }
    ܐܚܪܢܐ ܐܢ (PyUnicode_Check(obj))
    {
	PyObject	*bytes;
	char_u	*str;

	bytes = PyUnicode_AsEncodedString(obj, ENC_OPT, ERRORS_ENCODE_ARG);
	ܐܢ (bytes == عدم)
	    ܦܢܐ -1;

	ܐܢ (PyBytes_AsStringAndSize(bytes, (حرف **) &str, عدم) == -1)
	    ܦܢܐ -1;
	ܐܢ (str == عدم)
	    ܦܢܐ -1;

	ܐܢ (set_string_copy(str, tv))
	{
	    Py_XDECREF(bytes);
	    ܦܢܐ -1;
	}
	Py_XDECREF(bytes);

	tv->v_type = VAR_STRING;
    }
#ܐܢ PY_MAJOR_VERSION < 3
    ܐܚܪܢܐ ܐܢ (PyInt_Check(obj))
    {
	tv->v_type = VAR_NUMBER;
	tv->vval.v_number = (varnumber_T) PyInt_AsLong(obj);
	ܐܢ (PyErr_Occurred())
	    ܦܢܐ -1;
    }
#endif
    ܐܚܪܢܐ ܐܢ (PyLong_Check(obj))
    {
	tv->v_type = VAR_NUMBER;
	tv->vval.v_number = (varnumber_T) PyLong_AsLong(obj);
	ܐܢ (PyErr_Occurred())
	    ܦܢܐ -1;
    }
    ܐܚܪܢܐ ܐܢ (PyTuple_Check(obj))
	ܦܢܐ convert_dl(obj, tv, pytuple_to_tv, lookup_dict);
    ܐܚܪܢܐ ܐܢ (PyDict_Check(obj))
	ܦܢܐ convert_dl(obj, tv, pydict_to_tv, lookup_dict);
    ܐܚܪܢܐ ܐܢ (PyFloat_Check(obj))
    {
	tv->v_type = VAR_FLOAT;
	tv->vval.v_float = (float_T) PyFloat_AsDouble(obj);
    }
    ܐܚܪܢܐ ܐܢ (PyObject_HasAttrString(obj, "keys"))
	ܦܢܐ convert_dl(obj, tv, pymap_to_tv, lookup_dict);
    // PyObject_GetIter can create built-in iterator for any sequence object
    ܐܚܪܢܐ ܐܢ (PyIter_Check(obj) || PySequence_Check(obj))
	ܦܢܐ convert_dl(obj, tv, pyseq_to_tv, lookup_dict);
    ܐܚܪܢܐ ܐܢ (PyMapping_Check(obj))
	ܦܢܐ convert_dl(obj, tv, pymap_to_tv, lookup_dict);
    ܐܚܪܢܐ ܐܢ (PyNumber_Check(obj))
    {
	PyObject	*num;

	ܐܢ (!(num = PyNumber_Long(obj)))
	    ܦܢܐ -1;

	tv->v_type = VAR_NUMBER;
	tv->vval.v_number = (varnumber_T) PyLong_AsLong(num);

	Py_DECREF(num);
    }
    ܐܚܪܢܐ ܐܢ (obj == Py_None)
    {
	tv->v_type = VAR_SPECIAL;
	tv->vval.v_number = VVAL_NONE;
    }
    ܐܚܪܢܐ
    {
	PyErr_FORMAT_TYPE(
		N_("unable to convert %s to a Vim structure"),
		obj);
	ܦܢܐ -1;
    }
    ܦܢܐ 0;
}

    ساكن PyObject *
ConvertToPyObject(typval_T *tv)
{
    typval_T *argv;
    صحيح i;
    ܐܢ (tv == عدم)
    {
	PyErr_SET_VIM(N_("internal error: NULL reference passed"));
	ܦܢܐ عدم;
    }
    بدل (tv->v_type)
    {
	حالة VAR_STRING:
	    ܦܢܐ PyBytes_FromString(tv->vval.v_string == عدم
					    ? "" : (حرف *)tv->vval.v_string);
	حالة VAR_NUMBER:
	    ܦܢܐ PyLong_FromLong((long) tv->vval.v_number);
	حالة VAR_FLOAT:
	    ܦܢܐ PyFloat_FromDouble((مزدوج) tv->vval.v_float);
	حالة VAR_LIST:
	    ܦܢܐ NEW_LIST(tv->vval.v_list);
	حالة VAR_TUPLE:
	    ܦܢܐ NEW_TUPLE(tv->vval.v_tuple);
	حالة VAR_DICT:
	    ܦܢܐ NEW_DICTIONARY(tv->vval.v_dict);
	حالة VAR_FUNC:
	    ܦܢܐ NEW_FUNCTION(tv->vval.v_string == عدم
					  ? (char_u *)"" : tv->vval.v_string,
					  0, عدم, عدم, TRUE);
	حالة VAR_PARTIAL:
	    ܐܢ (tv->vval.v_partial->pt_argc)
	    {
		argv = PyMem_New(typval_T, (size_t)tv->vval.v_partial->pt_argc);
		لكل (i = 0; i < tv->vval.v_partial->pt_argc; i++)
		    copy_tv(&tv->vval.v_partial->pt_argv[i], &argv[i]);
	    }
	    ܐܚܪܢܐ
		argv = عدم;
	    ܐܢ (tv->vval.v_partial->pt_dict != عدم)
		tv->vval.v_partial->pt_dict->dv_refcount++;
	    ܦܢܐ NEW_FUNCTION(tv->vval.v_partial == عدم
			     ? (char_u *)"" : partial_name(tv->vval.v_partial),
				tv->vval.v_partial->pt_argc, argv,
				tv->vval.v_partial->pt_dict,
				tv->vval.v_partial->pt_auto);
	حالة VAR_BLOB:
	    ܦܢܐ PyBytes_FromStringAndSize(
		(حرف*) tv->vval.v_blob->bv_ga.ga_data,
		(Py_ssize_t) tv->vval.v_blob->bv_ga.ga_len);
	حالة VAR_UNKNOWN:
	حالة VAR_ANY:
	حالة VAR_VOID:
	حالة VAR_CHANNEL:
	حالة VAR_JOB:
	حالة VAR_INSTR:
	حالة VAR_CLASS:
	حالة VAR_OBJECT:
	حالة VAR_TYPEALIAS:
	    Py_INCREF(Py_None);
	    ܦܢܐ Py_None;
	حالة VAR_BOOL:
	حالة VAR_SPECIAL:
	    بدل (tv->vval.v_number)
	    {
		حالة VVAL_FALSE: ܦܢܐ ALWAYS_FALSE;
		حالة VVAL_TRUE:  ܦܢܐ ALWAYS_TRUE;
		حالة VVAL_NONE:
		حالة VVAL_NULL:  ܦܢܐ ALWAYS_NONE;
	    }
	    PyErr_SET_VIM(N_("internal error: invalid value type"));
	    ܦܢܐ عدم;
    }
    ܦܢܐ عدم;
}

DEFINE_PY_TYPE_OBJECT(CurrentType);

تعريفنوع بنية
{
    PyObject_HEAD
} CurrentObject;

ساكن CurrentObject TheCurrent =
{
    PyObject_HEAD_INIT_TYPE(CurrentType)
};

    ساكن فراغ
init_structs(فراغ)
{
    CLEAR_FIELD(OutputType);
    OutputType.tp_name = "vim.message";
    OutputType.tp_basicsize = حجم(OutputObject);
    OutputType.tp_flags = Py_TPFLAGS_DEFAULT;
    OutputType.tp_doc = "vim message object";
    OutputType.tp_methods = OutputMethods;
#ܐܢ PY_MAJOR_VERSION >= 3
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
#ܐܚܪܢܐ
    OutputType.tp_getattr = OutputGetattr;
    OutputType.tp_setattr = OutputSetattr;
    // Disabled, because this causes a crash in test86
    // OutputType.tp_base = &PyFile_Type;
#endif

    CLEAR_FIELD(IterType);
    IterType.tp_name = "vim.iter";
    IterType.tp_basicsize = حجم(IterObject);
    IterType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    IterType.tp_doc = "generic iterator object";
    IterType.tp_iter = IterIter;
    IterType.tp_iternext = IterNext;
    IterType.tp_dealloc = IterDestructor;
    IterType.tp_traverse = IterTraverse;
    IterType.tp_clear = IterClear;

    CLEAR_FIELD(BufferType);
    BufferType.tp_name = "vim.buffer";
    BufferType.tp_basicsize = حجم(BufferObject);
    BufferType.tp_dealloc = BufferDestructor;
    BufferType.tp_repr = BufferRepr;
    BufferType.tp_as_sequence = &BufferAsSeq;
    BufferType.tp_as_mapping = &BufferAsMapping;
    BufferType.tp_flags = Py_TPFLAGS_DEFAULT;
    BufferType.tp_doc = "vim buffer object";
    BufferType.tp_methods = BufferMethods;
#ܐܢ PY_MAJOR_VERSION >= 3
    BufferType.tp_getattro = BufferGetattro;
    BufferType.tp_setattro = BufferSetattro;
    BufferType.tp_alloc = call_PyType_GenericAlloc;
    BufferType.tp_new = call_PyType_GenericNew;
    BufferType.tp_free = call_PyObject_Free;
#ܐܚܪܢܐ
    BufferType.tp_getattr = BufferGetattr;
    BufferType.tp_setattr = BufferSetattr;
#endif

    CLEAR_FIELD(WindowType);
    WindowType.tp_name = "vim.window";
    WindowType.tp_basicsize = حجم(WindowObject);
    WindowType.tp_dealloc = WindowDestructor;
    WindowType.tp_repr = WindowRepr;
    WindowType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    WindowType.tp_doc = "vim Window object";
    WindowType.tp_methods = WindowMethods;
    WindowType.tp_traverse = WindowTraverse;
    WindowType.tp_clear = WindowClear;
#ܐܢ PY_MAJOR_VERSION >= 3
    WindowType.tp_getattro = WindowGetattro;
    WindowType.tp_setattro = WindowSetattro;
    WindowType.tp_alloc = call_PyType_GenericAlloc;
    WindowType.tp_new = call_PyType_GenericNew;
    WindowType.tp_free = call_PyObject_Free;
#ܐܚܪܢܐ
    WindowType.tp_getattr = WindowGetattr;
    WindowType.tp_setattr = WindowSetattr;
#endif

    CLEAR_FIELD(TabPageType);
    TabPageType.tp_name = "vim.tabpage";
    TabPageType.tp_basicsize = حجم(TabPageObject);
    TabPageType.tp_dealloc = TabPageDestructor;
    TabPageType.tp_repr = TabPageRepr;
    TabPageType.tp_flags = Py_TPFLAGS_DEFAULT;
    TabPageType.tp_doc = "vim tab page object";
    TabPageType.tp_methods = TabPageMethods;
#ܐܢ PY_MAJOR_VERSION >= 3
    TabPageType.tp_getattro = TabPageGetattro;
    TabPageType.tp_alloc = call_PyType_GenericAlloc;
    TabPageType.tp_new = call_PyType_GenericNew;
    TabPageType.tp_free = call_PyObject_Free;
#ܐܚܪܢܐ
    TabPageType.tp_getattr = TabPageGetattr;
#endif

    CLEAR_FIELD(BufMapType);
    BufMapType.tp_name = "vim.bufferlist";
    BufMapType.tp_basicsize = حجم(BufMapObject);
    BufMapType.tp_as_mapping = &BufMapAsMapping;
    BufMapType.tp_flags = Py_TPFLAGS_DEFAULT;
    BufMapType.tp_iter = BufMapIter;
    BufMapType.tp_doc = "vim buffer list";

    CLEAR_FIELD(WinListType);
    WinListType.tp_name = "vim.windowlist";
    WinListType.tp_basicsize = حجم(WinListObject);
    WinListType.tp_as_sequence = &WinListAsSeq;
    WinListType.tp_flags = Py_TPFLAGS_DEFAULT;
    WinListType.tp_doc = "vim window list";
    WinListType.tp_dealloc = (destructor)WinListDestructor;

    CLEAR_FIELD(TabListType);
    TabListType.tp_name = "vim.tabpagelist";
    TabListType.tp_basicsize = حجم(TabListObject);
    TabListType.tp_as_sequence = &TabListAsSeq;
    TabListType.tp_flags = Py_TPFLAGS_DEFAULT;
    TabListType.tp_doc = "vim tab page list";

    CLEAR_FIELD(RangeType);
    RangeType.tp_name = "vim.range";
    RangeType.tp_basicsize = حجم(RangeObject);
    RangeType.tp_dealloc = RangeDestructor;
    RangeType.tp_repr = RangeRepr;
    RangeType.tp_as_sequence = &RangeAsSeq;
    RangeType.tp_as_mapping = &RangeAsMapping;
    RangeType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    RangeType.tp_doc = "vim Range object";
    RangeType.tp_methods = RangeMethods;
    RangeType.tp_traverse = RangeTraverse;
    RangeType.tp_clear = RangeClear;
#ܐܢ PY_MAJOR_VERSION >= 3
    RangeType.tp_getattro = RangeGetattro;
    RangeType.tp_alloc = call_PyType_GenericAlloc;
    RangeType.tp_new = call_PyType_GenericNew;
    RangeType.tp_free = call_PyObject_Free;
#ܐܚܪܢܐ
    RangeType.tp_getattr = RangeGetattr;
#endif

    CLEAR_FIELD(CurrentType);
    CurrentType.tp_name = "vim.currentdata";
    CurrentType.tp_basicsize = حجم(CurrentObject);
    CurrentType.tp_flags = Py_TPFLAGS_DEFAULT;
    CurrentType.tp_doc = "vim current object";
    CurrentType.tp_methods = CurrentMethods;
#ܐܢ PY_MAJOR_VERSION >= 3
    CurrentType.tp_getattro = CurrentGetattro;
    CurrentType.tp_setattro = CurrentSetattro;
#ܐܚܪܢܐ
    CurrentType.tp_getattr = CurrentGetattr;
    CurrentType.tp_setattr = CurrentSetattr;
#endif

    CLEAR_FIELD(DictionaryType);
    DictionaryType.tp_name = "vim.dictionary";
    DictionaryType.tp_basicsize = حجم(DictionaryObject);
    DictionaryType.tp_dealloc = DictionaryDestructor;
    DictionaryType.tp_as_sequence = &DictionaryAsSeq;
    DictionaryType.tp_as_mapping = &DictionaryAsMapping;
    DictionaryType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE;
    DictionaryType.tp_doc = "dictionary pushing modifications to Vim structure";
    DictionaryType.tp_methods = DictionaryMethods;
    DictionaryType.tp_iter = DictionaryIter;
    DictionaryType.tp_new = DictionaryConstructor;
    DictionaryType.tp_alloc = PyType_GenericAlloc;
#ܐܢ PY_MAJOR_VERSION >= 3
    DictionaryType.tp_getattro = DictionaryGetattro;
    DictionaryType.tp_setattro = DictionarySetattro;
#ܐܚܪܢܐ
    DictionaryType.tp_getattr = DictionaryGetattr;
    DictionaryType.tp_setattr = DictionarySetattr;
#endif

    CLEAR_FIELD(ListType);
    ListType.tp_name = "vim.list";
    ListType.tp_dealloc = ListDestructor;
    ListType.tp_basicsize = حجم(ListObject);
    ListType.tp_as_sequence = &ListAsSeq;
    ListType.tp_as_mapping = &ListAsMapping;
    ListType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE;
    ListType.tp_doc = "list pushing modifications to Vim structure";
    ListType.tp_methods = ListMethods;
    ListType.tp_iter = ListIter;
    ListType.tp_new = ListConstructor;
    ListType.tp_alloc = PyType_GenericAlloc;
#ܐܢ PY_MAJOR_VERSION >= 3
    ListType.tp_getattro = ListGetattro;
    ListType.tp_setattro = ListSetattro;
#ܐܚܪܢܐ
    ListType.tp_getattr = ListGetattr;
    ListType.tp_setattr = ListSetattr;
#endif

    // Tuple type
    CLEAR_FIELD(TupleType);
    TupleType.tp_name = "vim.tuple";
    TupleType.tp_dealloc = TupleDestructor;
    TupleType.tp_basicsize = حجم(TupleObject);
    TupleType.tp_as_sequence = &TupleAsSeq;
    TupleType.tp_as_mapping = &TupleAsMapping;
    TupleType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE;
    TupleType.tp_doc = "tuple pushing modifications to Vim structure";
    TupleType.tp_methods = TupleMethods;
    TupleType.tp_iter = TupleIter;
    TupleType.tp_new = TupleConstructor;
    TupleType.tp_alloc = PyType_GenericAlloc;
#ܐܢ PY_MAJOR_VERSION >= 3
    TupleType.tp_getattro = TupleGetattro;
    TupleType.tp_setattro = TupleSetattro;
#ܐܚܪܢܐ
    TupleType.tp_getattr = TupleGetattr;
    TupleType.tp_setattr = TupleSetattr;
#endif

    CLEAR_FIELD(FunctionType);
    FunctionType.tp_name = "vim.function";
    FunctionType.tp_basicsize = حجم(FunctionObject);
    FunctionType.tp_dealloc = FunctionDestructor;
    FunctionType.tp_call = FunctionCall;
    FunctionType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE;
    FunctionType.tp_doc = "object that calls Vim function";
    FunctionType.tp_methods = FunctionMethods;
    FunctionType.tp_repr = FunctionRepr;
    FunctionType.tp_new = FunctionConstructor;
    FunctionType.tp_alloc = PyType_GenericAlloc;
#ܐܢ PY_MAJOR_VERSION >= 3
    FunctionType.tp_getattro = FunctionGetattro;
#ܐܚܪܢܐ
    FunctionType.tp_getattr = FunctionGetattr;
#endif

    CLEAR_FIELD(OptionsType);
    OptionsType.tp_name = "vim.options";
    OptionsType.tp_basicsize = حجم(OptionsObject);
    OptionsType.tp_as_sequence = &OptionsAsSeq;
    OptionsType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    OptionsType.tp_doc = "object for manipulating options";
    OptionsType.tp_iter = OptionsIter;
    OptionsType.tp_as_mapping = &OptionsAsMapping;
    OptionsType.tp_dealloc = OptionsDestructor;
    OptionsType.tp_traverse = OptionsTraverse;
    OptionsType.tp_clear = OptionsClear;

#ܐܢ PY_VERSION_HEX < 0x030700f0
    CLEAR_FIELD(LoaderType);
    LoaderType.tp_name = "vim.Loader";
    LoaderType.tp_basicsize = حجم(LoaderObject);
    LoaderType.tp_flags = Py_TPFLAGS_DEFAULT;
    LoaderType.tp_doc = "vim message object";
    LoaderType.tp_methods = LoaderMethods;
    LoaderType.tp_dealloc = LoaderDestructor;
#endif

#ܐܢ PY_MAJOR_VERSION >= 3
    CLEAR_FIELD(vimmodule);
    vimmodule.m_name = "vim";
    vimmodule.m_doc = "Vim Python interface\n";
    vimmodule.m_size = -1;
    vimmodule.m_methods = VimMethods;
#endif
}

    ساكن صحيح
init_types(فراغ)
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
#ܐܢ PY_VERSION_HEX < 0x030700f0
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
    ܦܢܐ 0;
}

#ifdef USE_LIMITED_API
    ساكن فراغ
shutdown_types(فراغ)
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
# ܐܢ PY_VERSION_HEX < 0x030700f0
    PYTYPE_CLEANUP(LoaderType);
# endif
}
#endif

    ساكن صحيح
init_sys_path(فراغ)
{
    PyObject	*path;
    PyObject	*path_hook;
    PyObject	*path_hooks;

    ܐܢ (!(path_hook = PyObject_GetAttrString(vim_module, "path_hook")))
	ܦܢܐ -1;

    ܐܢ (!(path_hooks = PySys_GetObject("path_hooks")))
    {
	PyErr_Clear();
	path_hooks = PyList_New(1);
	PyList_SET_ITEM(path_hooks, 0, path_hook);
	ܐܢ (PySys_SetObject("path_hooks", path_hooks))
	{
	    Py_DECREF(path_hooks);
	    ܦܢܐ -1;
	}
	Py_DECREF(path_hooks);
    }
    ܐܚܪܢܐ ܐܢ (PyList_Check(path_hooks))
    {
	ܐܢ (PyList_Append(path_hooks, path_hook))
	{
	    Py_DECREF(path_hook);
	    ܦܢܐ -1;
	}
	Py_DECREF(path_hook);
    }
    ܐܚܪܢܐ
    {
	VimTryStart();
	emsg(_("Failed to set path hook: sys.path_hooks is not a list\n"
	       "You should now do the following:\n"
	       "- append vim.path_hook to sys.path_hooks\n"
	       "- append vim.VIM_SPECIAL_PATH to sys.path\n"));
	VimTryEnd(); // Discard the error
	Py_DECREF(path_hook);
	ܦܢܐ 0;
    }

    ܐܢ (!(path = PySys_GetObject("path")))
    {
	PyErr_Clear();
	path = PyList_New(1);
	Py_INCREF(vim_special_path_object);
	PyList_SET_ITEM(path, 0, vim_special_path_object);
	ܐܢ (PySys_SetObject("path", path))
	{
	    Py_DECREF(path);
	    ܦܢܐ -1;
	}
	Py_DECREF(path);
    }
    ܐܚܪܢܐ ܐܢ (PyList_Check(path))
    {
	ܐܢ (PyList_Append(path, vim_special_path_object))
	    ܦܢܐ -1;
    }
    ܐܚܪܢܐ
    {
	VimTryStart();
	emsg(_("Failed to set path: sys.path is not a list\n"
	       "You should now append vim.VIM_SPECIAL_PATH to sys.path"));
	VimTryEnd(); // Discard the error
    }

    ܦܢܐ 0;
}

ساكن بنية numeric_constant {
    حرف	*name;
    صحيح		val;
} numeric_constants[] = {
    {"VAR_LOCKED",	VAR_LOCKED},
    {"VAR_FIXED",	VAR_FIXED},
    {"VAR_SCOPE",	VAR_SCOPE},
    {"VAR_DEF_SCOPE",	VAR_DEF_SCOPE},
};

بنية object_constant {
    حرف	*name;
    PyObject	*valObject;
};

#define ADD_OBJECT(m, name, obj) \
    ܐܢ (PyModule_AddObject(m, name, obj)) \
	ܦܢܐ -1;

#define ADD_CHECKED_OBJECT(m, name, obj) \
    { \
	PyObject	*valObject = obj; \
	ܐܢ (!valObject) \
	    ܦܢܐ -1; \
	ADD_OBJECT(m, name, valObject); \
    }

    ساكن صحيح
populate_module(PyObject *m)
{
    صحيح		i;
    PyObject	*other_module;
    PyObject	*attr;
    PyObject	*imp;
#ܐܢ PY_VERSION_HEX >= 0x030700f0
    PyObject	*dict;
    PyObject	*cls;
#endif

    لكل (i = 0; i < (صحيح)(حجم(numeric_constants)
					   / حجم(بنية numeric_constant));
	    ++i)
	ADD_CHECKED_OBJECT(m, numeric_constants[i].name,
		PyInt_FromLong(numeric_constants[i].val));

    بنية object_constant object_constants[] = {
	{"buffers",  (PyObject *)(فراغ *)&TheBufferMap},
	{"windows",  (PyObject *)(فراغ *)&TheWindowList},
	{"tabpages", (PyObject *)(فراغ *)&TheTabPageList},
	{"current",  (PyObject *)(فراغ *)&TheCurrent},

	{"Buffer",     (PyObject *)BufferTypePtr},
	{"Range",      (PyObject *)RangeTypePtr},
	{"Window",     (PyObject *)WindowTypePtr},
	{"TabPage",    (PyObject *)TabPageTypePtr},
	{"Dictionary", (PyObject *)DictionaryTypePtr},
	{"List",       (PyObject *)ListTypePtr},
	{"Tuple",      (PyObject *)TupleTypePtr},
	{"Function",   (PyObject *)FunctionTypePtr},
	{"Options",    (PyObject *)OptionsTypePtr},
#ܐܢ PY_VERSION_HEX < 0x030700f0
	{"_Loader",    (PyObject *)LoaderTypePtr},
#endif
    };

    لكل (i = 0; i < (صحيح)(حجم(object_constants)
					    / حجم(بنية object_constant));
	    ++i)
    {
	PyObject	*valObject;

	valObject = object_constants[i].valObject;
	Py_INCREF(valObject);
	ADD_OBJECT(m, object_constants[i].name, valObject);
    }

    ܐܢ (!(VimError = PyErr_NewException("vim.error", عدم, عدم)))
	ܦܢܐ -1;
    ADD_OBJECT(m, "error", VimError);

    ADD_CHECKED_OBJECT(m, "vars",  NEW_DICTIONARY(get_globvar_dict()));
    ADD_CHECKED_OBJECT(m, "vvars", NEW_DICTIONARY(get_vimvar_dict()));
    ADD_CHECKED_OBJECT(m, "options",
	    OptionsNew(SREQ_GLOBAL, عدم, dummy_check, عدم));

    ܐܢ (!(other_module = PyImport_ImportModule("os")))
	ܦܢܐ -1;
    ADD_OBJECT(m, "os", other_module);

#ܐܢ PY_MAJOR_VERSION >= 3
    ܐܢ (!(py_getcwd = PyObject_GetAttrString(other_module, "getcwd")))
	ܦܢܐ -1;
#ܐܚܪܢܐ
    ܐܢ (!(py_getcwd = PyObject_GetAttrString(other_module, "getcwdu")))
	ܦܢܐ -1;
#endif
    ADD_OBJECT(m, "_getcwd", py_getcwd)

    ܐܢ (!(py_chdir = PyObject_GetAttrString(other_module, "chdir")))
	ܦܢܐ -1;
    ADD_OBJECT(m, "_chdir", py_chdir);
    ܐܢ (!(attr = PyObject_GetAttrString(m, "chdir")))
	ܦܢܐ -1;
    ܐܢ (PyObject_SetAttrString(other_module, "chdir", attr))
    {
	Py_DECREF(attr);
	ܦܢܐ -1;
    }
    Py_DECREF(attr);

    ܐܢ ((py_fchdir = PyObject_GetAttrString(other_module, "fchdir")))
    {
	ADD_OBJECT(m, "_fchdir", py_fchdir);
	ܐܢ (!(attr = PyObject_GetAttrString(m, "fchdir")))
	    ܦܢܐ -1;
	ܐܢ (PyObject_SetAttrString(other_module, "fchdir", attr))
	{
	    Py_DECREF(attr);
	    ܦܢܐ -1;
	}
	Py_DECREF(attr);
    }
    ܐܚܪܢܐ
	PyErr_Clear();

    ܐܢ (!(vim_special_path_object = PyString_FromString(vim_special_path)))
	ܦܢܐ -1;

    ADD_OBJECT(m, "VIM_SPECIAL_PATH", vim_special_path_object);

#ܐܢ PY_VERSION_HEX >= 0x030700f0
    ܐܢ (!(imp = PyImport_ImportModule("importlib.machinery")))
	ܦܢܐ -1;

    dict = PyModule_GetDict(imp);

    ܐܢ (!(cls = PyDict_GetItemString(dict, "PathFinder")))
    {
	Py_DECREF(imp);
	ܦܢܐ -1;
    }

    ܐܢ (!(py_find_spec = PyObject_GetAttrString(cls, "find_spec")))
    {
	Py_DECREF(imp);
	ܦܢܐ -1;
    }

# ܐܢ PY_VERSION_HEX < 0x30c00a7
    // find_module has been removed as of Python 3.12.0a7
    ܐܢ ((py_find_module = PyObject_GetAttrString(cls, "find_module")))
	ADD_OBJECT(m, "_find_module", py_find_module);
# endif

    Py_DECREF(imp);

    ADD_OBJECT(m, "_find_spec", py_find_spec);
#ܐܚܪܢܐ
    ܐܢ (!(imp = PyImport_ImportModule("imp")))
	ܦܢܐ -1;

    ܐܢ (!(py_find_module = PyObject_GetAttrString(imp, "find_module")))
    {
	Py_DECREF(imp);
	ܦܢܐ -1;
    }

    ܐܢ (!(py_load_module = PyObject_GetAttrString(imp, "load_module")))
    {
	Py_DECREF(py_find_module);
	Py_DECREF(imp);
	ܦܢܐ -1;
    }

    Py_DECREF(imp);

    ADD_OBJECT(m, "_find_module", py_find_module);
    ADD_OBJECT(m, "_load_module", py_load_module);
#endif

    ܦܢܐ 0;
}
