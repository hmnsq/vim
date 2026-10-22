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

#যদি PY_VERSION_HEX < 0x02050000
প্রকারনির্দেশ পূর্ণাঙ্ক Py_ssize_t;  // Python 2.4 and earlier don't have this type.
#endif

// Use values that are known to work, others may make Vim crash.
#define ENC_OPT (enc_utf8 ? "utf-8" : enc_dbcs ? "euc-jp" : (বর্ণ *)p_enc)
#define DOPY_FUNC "_vim_pydo"

স্থির ধ্রুব বর্ণ *vim_special_path = "_vim_path_";

#define PyErr_SET_STRING(exc, str) PyErr_SetString(exc, _(str))
#define PyErr_SetVim(str) PyErr_SetString(VimError, str)
#define PyErr_SET_VIM(str) PyErr_SET_STRING(VimError, str)
#define PyErr_FORMAT(exc, str, arg) PyErr_Format(exc, _(str), arg)
#define PyErr_FORMAT2(exc, str, arg1, arg2) PyErr_Format(exc, _(str), arg1,arg2)
#define PyErr_VIM_FORMAT(str, arg) PyErr_FORMAT(VimError, str, arg)

#ifdef USE_LIMITED_API
// Limited Python API. Need to call only exposed functions and remap macros.
// PyTypeObject is an opaque struct.

প্রকারনির্দেশ সংরচনা {
    lenfunc sq_length;
    binaryfunc sq_concat;
    ssizeargfunc sq_repeat;
    ssizeargfunc sq_item;
    শূন্য *was_sq_slice;
    ssizeobjargproc sq_ass_item;
    শূন্য *was_sq_ass_slice;
    objobjproc sq_contains;

    binaryfunc sq_inplace_concat;
    ssizeargfunc sq_inplace_repeat;
} PySequenceMethods;

প্রকারনির্দেশ সংরচনা {
    lenfunc mp_length;
    binaryfunc mp_subscript;
    objobjargproc mp_ass_subscript;
} PyMappingMethods;

// This struct emulates the concrete _typeobject struct to allow the code to
// work the same way in both limited and full Python APIs.
সংরচনা typeobject_wrapper {
    ধ্রুব বর্ণ *tp_name;
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

    ধ্রুব বর্ণ *tp_doc;

    traverseproc tp_traverse;

    inquiry tp_clear;

    getiterfunc tp_iter;
    iternextfunc tp_iternext;

    সংরচনা PyMethodDef *tp_methods;
    সংরচনা _typeobject *tp_base;
    allocfunc tp_alloc;
    newfunc tp_new;
    freefunc tp_free;
};

# define DEFINE_PY_TYPE_OBJECT(type) \
    স্থির সংরচনা typeobject_wrapper type; \
    স্থির PyTypeObject* type##Ptr = রিক্ত

// PyObject_HEAD_INIT_TYPE and PyObject_FINISH_INIT_TYPE need to come in pairs
// We first initialize with NULL because the type is not allocated until
// init_types() is called later. It's in FINISH_INIT_TYPE where we fill the
// type in with the newly allocated type.
# define PyObject_HEAD_INIT_TYPE(type) PyObject_HEAD_INIT(রিক্ত)
# define PyObject_FINISH_INIT_TYPE(obj, type) obj.ob_base.ob_type = type##Ptr

# define Py_TYPE_GET_TP_ALLOC(type) ((allocfunc)PyType_GetSlot(type, Py_tp_alloc))
# define Py_TYPE_GET_TP_METHODS(type) ((PyMethodDef *)PyType_GetSlot(type, Py_tp_methods))

// PyObject_NEW is not part of stable ABI, but PyObject_Malloc/Init are.
স্থির PyObject* Vim_PyObject_New(PyTypeObject *type, size_t objsize)
{
    PyObject *obj = (PyObject *)PyObject_Malloc(objsize);
    যদি (obj == রিক্ত)
	প্রত্যাগম PyErr_NoMemory();
    প্রত্যাগম PyObject_Init(obj, type);
}
# undef PyObject_NEW
# define PyObject_NEW(type, typeobj) ((type *)Vim_PyObject_New(typeobj, পরিমাণ(type)))

// This is a somewhat convoluted because limited API doesn't expose an easy way
// to get the tp_name field, and so we have to manually reconstruct it as
// "__module__.__name__" (with __module__ omitted for builtins to emulate
// Python behavior). Also, some of the more convenient functions like
// PyUnicode_AsUTF8AndSize and PyType_GetQualName() are not available until
// late Python 3 versions, and won't be available if you set Py_LIMITED_API too
// low.
# define PyErr_FORMAT_TYPE(বার্তা, obj) \
    কুরু { \
	PyObject* qualname = PyObject_GetAttrString((PyObject*)(obj)->ob_type, "__qualname__"); \
	যদি (qualname == রিক্ত) \
	{ \
	    PyErr_FORMAT(PyExc_TypeError, বার্তা, "(NULL)"); \
	    ভঙ্গ; \
	} \
	PyObject* module = PyObject_GetAttrString((PyObject*)(obj)->ob_type, "__module__"); \
	PyObject* full; \
	যদি (module == রিক্ত || PyUnicode_CompareWithASCIIString(module, "builtins") == 0 \
		|| PyUnicode_CompareWithASCIIString(module, "__main__") == 0) \
	{ \
	    full = qualname; \
	    Py_INCREF(full); \
	} \
	অন্যথা \
	    full = PyUnicode_FromFormat("%U.%U", module, qualname); \
	PyObject* full_bytes = PyUnicode_AsUTF8String(full); \
	ধ্রুব বর্ণ* full_str = PyBytes_AsString(full_bytes); \
	full_str = full_str == রিক্ত ? "(NULL)" : full_str; \
	PyErr_FORMAT(PyExc_TypeError, বার্তা, full_str); \
	Py_DECREF(qualname); \
	Py_XDECREF(module); \
	Py_XDECREF(full); \
	Py_XDECREF(full_bytes); \
    } যাবত্ (0)

# define PyList_GET_ITEM(list, i) PyList_GetItem(list, i)
# define PyList_GET_SIZE(o) PyList_Size(o)

// PyList_SET_ITEM and PyList_SetItem have slightly different behaviors. The
// former will leave the old item dangling, and the latter will decref on it.
// Since we only use this on new lists, this difference doesn't matter.
# define PyList_SET_ITEM(list, i, item) PyList_SetItem(list, i, item)

# define PyTuple_GET_ITEM(o, pos) PyTuple_GetItem(o, pos)
# define PyTuple_GET_SIZE(o) PyTuple_Size(o)

# যদি Py_LIMITED_API < 0x03080000
// PyIter_check only became part of stable ABI in 3.8, and there is no easy way
// to check for it in the API. We simply return false as a compromise. This
// does mean we should avoid compiling with stable ABI < 3.8.
#  undef PyIter_Check
#  define PyIter_Check(obj) (FALSE)
# endif

স্থির PyTypeObject* AddHeapType(সংরচনা typeobject_wrapper* type_object)
{
    PyType_Spec type_spec;
    type_spec.name = type_object->tp_name;
    type_spec.basicsize = type_object->tp_basicsize;
    type_spec.itemsize = 0;
    type_spec.flags = type_object->tp_flags;

    // We just need to statically allocate a large enough buffer that can hold
    // all slots. We need to leave a null-terminated slot at the end.
    PyType_Slot slots[40] = { {0, রিক্ত} };
    size_t slot_i = 0;

# define ADD_TP_SLOT(slot_name) \
    যদি (slot_i >= 40) প্রত্যাগম রিক্ত; /* this should never happen */ \
    যদি (type_object->slot_name != রিক্ত) \
    { \
	slots[slot_i].slot = Py_##slot_name; \
	slots[slot_i].pfunc = (শূন্য*)type_object->slot_name; \
	++slot_i; \
    }
# define ADD_TP_SUB_SLOT(sub_slot, slot_name) \
    যদি (slot_i >= 40) প্রত্যাগম রিক্ত; /* this should never happen */ \
    যদি (type_object->sub_slot != রিক্ত && type_object->sub_slot->slot_name != রিক্ত) \
    { \
	slots[slot_i].slot = Py_##slot_name; \
	slots[slot_i].pfunc = (শূন্য*)type_object->sub_slot->slot_name; \
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
    প্রত্যাগম (PyTypeObject*)newtype;
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
    যদি (type##Ptr == রিক্ত) \
	প্রত্যাগম -1;
# define PYTYPE_CLEANUP(type) \
    type##Ptr = রিক্ত;

// Limited API does not provide PyRun_* functions. Need to implement manually
// using PyCompile and PyEval.
স্থির PyObject* Vim_PyRun_String(ধ্রুব বর্ণ *str, পূর্ণাঙ্ক start, PyObject *globals, PyObject *locals)
{
    // Just pass "" for filename for now.
    PyObject* compiled = Py_CompileString(str, "", start);
    যদি (compiled == রিক্ত)
	প্রত্যাগম রিক্ত;

    PyObject* eval_result = PyEval_EvalCode(compiled, globals, locals);
    Py_DECREF(compiled);
    প্রত্যাগম eval_result;
}
স্থির পূর্ণাঙ্ক Vim_PyRun_SimpleString(ধ্রুব বর্ণ *str)
{
    // This function emulates CPython's implementation.
    PyObject* m = PyImport_AddModule("__main__");
    যদি (m == রিক্ত)
	প্রত্যাগম -1;
    PyObject* d = PyModule_GetDict(m);
    PyObject* output = Vim_PyRun_String(str, Py_file_প্রবেশ, d, d);
    যদি (output == রিক্ত)
    {
	PyErr_PrintEx(TRUE);
	প্রত্যাগম -1;
    }
    Py_DECREF(output);
    প্রত্যাগম 0;
}
# define PyRun_String Vim_PyRun_String
# define PyRun_SimpleString Vim_PyRun_SimpleString

#অন্যথা // !defined(USE_LIMITED_API)

// Full Python API. Can make use of structs and macros directly.
# define DEFINE_PY_TYPE_OBJECT(type) \
    স্থির PyTypeObject type; \
    স্থির PyTypeObject* type##Ptr = &type
# define PyObject_HEAD_INIT_TYPE(type) PyObject_HEAD_INIT(&type)

# define Py_TYPE_GET_TP_ALLOC(type) type->tp_alloc
# define Py_TYPE_GET_TP_METHODS(type) type->tp_methods

# define Py_TYPE_NAME(obj) ((obj)->ob_type->tp_name == রিক্ত \
	? "(NULL)" \
	: (obj)->ob_type->tp_name)
# define PyErr_FORMAT_TYPE(বার্তা, obj) \
    PyErr_FORMAT(PyExc_TypeError, বার্তা, \
	    Py_TYPE_NAME(obj))

// Add a static type
# define PYTYPE_READY(type) \
    যদি (PyType_Ready(type##Ptr)) \
	প্রত্যাগম -1;

#endif


#define RAISE_NO_EMPTY_চাবিগুলি PyErr_SET_STRING(Py_Exc_মান_Error, \
					    N_("empty keys are not allowed"))
#define RAISE_LOCKED_DICTIONARY PyErr_SET_VIM(N_("dictionary is locked"))
#define RAISE_LOCKED_LIST PyErr_SET_VIM(N_("list is locked"))
#define RAISE_UNDO_FAIL PyErr_SET_VIM(N_("cannot save undo information"))
#define RAISE_DELETE_সারি_FAIL PyErr_SET_VIM(N_("cannot delete line"))
#define RAISE_ঢোকাও_সারি_FAIL PyErr_SET_VIM(N_("cannot insert line"))
#define RAISE_REPLACE_সারি_FAIL PyErr_SET_VIM(N_("cannot replace line"))
#define RAISE_চাবি_ADD_FAIL(চাবি) \
    PyErr_VIM_FORMAT(N_("failed to add key '%s' to dictionary"), চাবি)
#define RAISE_INVALID_INDEX_TYPE(idx) \
    PyErr_FORMAT_TYPE(N_("index must be int or slice, not %s"), idx);

#define INVALID_BUFFER_মান ((buf_T *)(-1))
#define INVALID_WINDOW_মান ((win_T *)(-1))
#define INVALID_TABPAGE_মান ((tabpage_T *)(-1))

প্রকারনির্দেশ শূন্য (*rangeinitializer)(শূন্য *);
প্রকারনির্দেশ শূন্য (*runner)(ধ্রুব বর্ণ *, dict_T *, শূন্য *
#ifdef PY_CAN_RECURSE
	, PyGILState_STATE *
#endif
	);

স্থির পূর্ণাঙ্ক ConvertFromPyObject(PyObject *, typval_T *);
স্থির পূর্ণাঙ্ক _ConvertFromPyObject(PyObject *, typval_T *, PyObject *);
স্থির পূর্ণাঙ্ক ConvertFromPyMapping(PyObject *, typval_T *);
স্থির পূর্ণাঙ্ক ConvertFromPySequence(PyObject *, typval_T *);
স্থির PyObject *WindowNew(win_T *, tabpage_T *);
স্থির PyObject *BufferNew (buf_T *);
স্থির PyObject *সারি_To_String(ধ্রুব বর্ণ *);

স্থির PyInt RangeStart;
স্থির PyInt RangeEnd;

স্থির PyObject *globals;

স্থির PyObject *py_chdir;
স্থির PyObject *py_fchdir;
স্থির PyObject *py_getcwd;
স্থির PyObject *vim_module;
স্থির PyObject *vim_special_path_object;

#যদি PY_VERSION_HEX >= 0x030700f0
স্থির PyObject *py_find_spec;
#অন্যথা
স্থির PyObject *py_load_module;
#endif
#যদি PY_VERSION_HEX < 0x30c00a7
স্থির PyObject *py_find_module;
#endif

স্থির PyObject *VimError;

/*
 * obtain a lock on the Vim data structures
 */
    স্থির শূন্য
Python_Lock_Vim(শূন্য)
{
}

/*
 * release a lock on the Vim data structures
 */
    স্থির শূন্য
Python_Release_Vim(শূন্য)
{
}

/*
 * The "todecref" argument holds a pointer to PyObject * that must be
 * DECREF'ed after returned char_u * is no longer needed or NULL if all what
 * was needed to generate returned value is object.
 *
 * Use Py_XDECREF to decrement reference count.
 */
    স্থির char_u *
StringToChars(PyObject *obj, PyObject **todecref)
{
    char_u	*str;

    যদি (PyBytes_Check(obj))
    {

	যদি (PyBytes_AsStringAndSize(obj, (বর্ণ **) &str, রিক্ত) == -1
		|| str == রিক্ত)
	    প্রত্যাগম রিক্ত;

	*todecref = রিক্ত;
    }
    অন্যথা যদি (PyUnicode_Check(obj))
    {
	PyObject	*bytes;

	যদি (!(bytes = PyUnicode_AsEncodedString(obj, ENC_OPT,
							   ERRORS_ENCODE_ARG)))
	    প্রত্যাগম রিক্ত;

	যদি (PyBytes_AsStringAndSize(bytes, (বর্ণ **) &str, রিক্ত) == -1
		|| str == রিক্ত)
	{
	    Py_DECREF(bytes);
	    প্রত্যাগম রিক্ত;
	}

	*todecref = bytes;
    }
    অন্যথা
    {
#যদি PY_MAJOR_VERSION < 3
	PyErr_FORMAT_TYPE(N_("expected str() or unicode() instance, but got %s"),
		obj);
#অন্যথা
	PyErr_FORMAT_TYPE(N_("expected bytes() or str() instance, but got %s"),
		obj);
#endif
	প্রত্যাগম রিক্ত;
    }

    প্রত্যাগম (char_u *) str;
}

#define সংখ্যা_LONG     1
#define সংখ্যা_INT      2
#define সংখ্যা_NATURAL  4
#define সংখ্যা_UNSIGNED 8

    স্থির পূর্ণাঙ্ক
সংখ্যা_To_Long(PyObject *obj, long *result, পূর্ণাঙ্ক flags)
{
#যদি PY_MAJOR_VERSION < 3
    যদি (PyInt_Check(obj))
    {
	*result = PyInt_AsLong(obj);
	যদি (PyErr_Occurred())
	    প্রত্যাগম -1;
    }
    অন্যথা
#endif
    যদি (PyLong_Check(obj))
    {
	*result = PyLong_AsLong(obj);
	যদি (PyErr_Occurred())
	    প্রত্যাগম -1;
    }
    অন্যথা যদি (Py_সংখ্যা_Check(obj))
    {
	PyObject	*অঙ্ক;

	যদি (!(অঙ্ক = Py_সংখ্যা_Long(obj)))
	    প্রত্যাগম -1;

	*result = PyLong_AsLong(অঙ্ক);

	Py_DECREF(অঙ্ক);

	যদি (PyErr_Occurred())
	    প্রত্যাগম -1;
    }
    অন্যথা
    {
#যদি PY_MAJOR_VERSION < 3
	PyErr_FORMAT_TYPE(
		N_("expected int(), long() or something supporting "
		   "coercing to long(), but got %s"),
		obj);
#অন্যথা
	PyErr_FORMAT_TYPE(
		N_("expected int() or something supporting coercing to int(), "
		   "but got %s"),
		obj);
#endif
	প্রত্যাগম -1;
    }

    যদি (flags & সংখ্যা_INT)
    {
	যদি (*result > INT_MAX)
	{
	    PyErr_SET_STRING(PyExc_OverflowError,
		    N_("value is too large to fit into C int type"));
	    প্রত্যাগম -1;
	}
	অন্যথা যদি (*result < INT_MIN)
	{
	    PyErr_SET_STRING(PyExc_OverflowError,
		    N_("value is too small to fit into C int type"));
	    প্রত্যাগম -1;
	}
    }

    যদি (flags & সংখ্যা_NATURAL)
    {
	যদি (*result <= 0)
	{
	    PyErr_SET_STRING(Py_Exc_মান_Error,
		    N_("number must be greater than zero"));
	    প্রত্যাগম -1;
	}
    }
    অন্যথা যদি (flags & সংখ্যা_UNSIGNED)
    {
	যদি (*result < 0)
	{
	    PyErr_SET_STRING(Py_Exc_মান_Error,
		    N_("number must be greater or equal to zero"));
	    প্রত্যাগম -1;
	}
    }

    প্রত্যাগম 0;
}

    স্থির পূর্ণাঙ্ক
add_string(PyObject *list, বর্ণ *s)
{
    PyObject	*string;

    যদি (!(string = PyString_FromString(s)))
	প্রত্যাগম -1;

    যদি (PyList_Append(list, string))
    {
	Py_DECREF(string);
	প্রত্যাগম -1;
    }

    Py_DECREF(string);
    প্রত্যাগম 0;
}

    স্থির PyObject *
ObjectDir(PyObject *self, বর্ণ **attributes)
{
    PyMethodDef	*method;
    বর্ণ	**attr;
    PyObject	*ret;

    যদি (!(ret = PyList_New(0)))
	প্রত্যাগম রিক্ত;

    যদি (self)
	প্রতি (method = Py_TYPE_GET_TP_METHODS(self->ob_type) ; method->ml_name != রিক্ত ; ++method)
	    যদি (add_string(ret, (বর্ণ *)method->ml_name))
	    {
		Py_DECREF(ret);
		প্রত্যাগম রিক্ত;
	    }

    প্রতি (attr = attributes ; *attr ; ++attr)
	যদি (add_string(ret, *attr))
	{
	    Py_DECREF(ret);
	    প্রত্যাগম রিক্ত;
	}

#যদি PY_MAJOR_VERSION < 3
    যদি (add_string(ret, "__members__"))
    {
	Py_DECREF(ret);
	প্রত্যাগম রিক্ত;
    }
#endif

    প্রত্যাগম ret;
}

// Output buffer management

// Function to write a line, points to either msg() or emsg().
প্রকারনির্দেশ পূর্ণাঙ্ক (*writefn)(বর্ণ *);

DEFINE_PY_TYPE_OBJECT(OutputType);

প্রকারনির্দেশ সংরচনা
{
    PyObject_HEAD
    long softspace;
    long error;
} OutputObject;

স্থির বর্ণ *OutputAttrs[] = {
    "softspace",
    রিক্ত
};

    স্থির PyObject *
OutputDir(PyObject *self, PyObject *args UNUSED)
{
    প্রত্যাগম ObjectDir(self, OutputAttrs);
}

    স্থির পূর্ণাঙ্ক
OutputSetattr(PyObject *self_obj, বর্ণ *name, PyObject *valObject)
{
    OutputObject *self = (OutputObject*)self_obj;
    যদি (valObject == রিক্ত)
    {
	PyErr_SET_STRING(PyExc_AttributeError,
		N_("can't delete OutputObject attributes"));
	প্রত্যাগম -1;
    }

    যদি (strcmp(name, "softspace") == 0)
    {
	যদি (সংখ্যা_To_Long(valObject, &(self->softspace), সংখ্যা_UNSIGNED))
	    প্রত্যাগম -1;
	প্রত্যাগম 0;
    }

    PyErr_FORMAT(PyExc_AttributeError, N_("invalid attribute: %s"), name);
    প্রত্যাগম -1;
}

// Buffer IO, we write one whole line at a time.
স্থির garray_T io_ga = {0, 0, 1, 80, রিক্ত};
স্থির writefn old_fn = রিক্ত;

    স্থির শূন্য
PythonIO_Flush(শূন্য)
{
    যদি (old_fn != রিক্ত && io_ga.ga_len > 0)
    {
	((বর্ণ *)io_ga.ga_data)[io_ga.ga_len] = NUL;
	// We don't know what emsg_severe should be here, so ... hope?
	old_fn((বর্ণ *)io_ga.ga_data);
    }
    io_ga.ga_len = 0;
}

    স্থির শূন্য
writer(writefn fn, char_u *str, PyInt n, পূর্ণাঙ্ক severe)
{
    char_u *ptr;

    // Flush when switching output function.
    যদি (fn != old_fn)
	PythonIO_Flush();
    old_fn = fn;

    // Write each NL separated line.  Text after the last NL is kept for
    // writing later.
    // For normal messages: Do not output when "got_int" was set.  This avoids
    // a loop gone crazy flooding the terminal with messages.  Also for when
    // "q" is pressed at the more-prompt.
    যাবত্ (n > 0 && (ptr = memchr(str, '\n', n)) != রিক্ত
					  && (fn == (writefn)emsg || !got_int))
    {
	PyInt len = ptr - str;

	যদি (ga_grow(&io_ga, (পূর্ণাঙ্ক)(len + 1)) == FAIL)
	    ভঙ্গ;

	mch_memmove(((বর্ণ *)io_ga.ga_data) + io_ga.ga_len, str, (size_t)len);
	((বর্ণ *)io_ga.ga_data)[io_ga.ga_len + len] = NUL;
	emsg_severe = severe;
	fn((বর্ণ *)io_ga.ga_data);
	str = ptr + 1;
	n -= len + 1;
	io_ga.ga_len = 0;
    }

    // Put the remaining text into io_ga for later printing.
    যদি (n > 0 && (fn == (writefn)emsg || !got_int)
					&& ga_grow(&io_ga, (পূর্ণাঙ্ক)(n + 1)) == OK)
    {
	mch_memmove(((বর্ণ *)io_ga.ga_data) + io_ga.ga_len, str, (size_t)n);
	io_ga.ga_len += (পূর্ণাঙ্ক)n;
    }
}

    স্থির পূর্ণাঙ্ক
write_output(OutputObject *self, PyObject *string)
{
    Py_ssize_t	len = 0;
    বর্ণ	*str = রিক্ত;
    পূর্ণাঙ্ক		error = self->error;

    যদি (!PyArg_Parse(string, "et#", ENC_OPT, &str, &len))
	প্রত্যাগম -1;

    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    writer((writefn)(error ? emsg : বার্তা), (char_u *)str, len, error);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS
    PyMem_Free(str);

    প্রত্যাগম 0;
}

    স্থির PyObject *
OutputWrite(OutputObject *self, PyObject *string)
{
    যদি (write_output(self, string))
	প্রত্যাগম রিক্ত;

    Py_INCREF(Py_None);
    প্রত্যাগম Py_None;
}

    স্থির PyObject *
OutputWritelines(OutputObject *self, PyObject *seq)
{
    PyObject	*iterator;
    PyObject	*item;

    যদি (!(iterator = PyObject_GetIter(seq)))
	প্রত্যাগম রিক্ত;

    যাবত্ ((item = PyIter_Next(iterator)))
    {
	যদি (write_output(self, item))
	{
	    Py_DECREF(iterator);
	    Py_DECREF(item);
	    প্রত্যাগম রিক্ত;
	}
	Py_DECREF(item);
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    যদি (PyErr_Occurred())
	প্রত্যাগম রিক্ত;

    Py_INCREF(Py_None);
    প্রত্যাগম Py_None;
}

    স্থির PyObject *
AlwaysNone(PyObject *self UNUSED, PyObject *args UNUSED)
{
    // do nothing
    Py_INCREF(Py_None);
    প্রত্যাগম Py_None;
}
#define ALWAYS_NONE AlwaysNone(রিক্ত, রিক্ত)

    স্থির PyObject *
AlwaysFalse(PyObject *self UNUSED, PyObject *args UNUSED)
{
    // do nothing
    PyObject	*ret = Py_False;
    Py_INCREF(ret);
    প্রত্যাগম ret;
}
#define ALWAYS_FALSE AlwaysFalse(রিক্ত, রিক্ত)

    স্থির PyObject *
AlwaysTrue(PyObject *self UNUSED, PyObject *args UNUSED)
{
    // do nothing
    PyObject	*ret = Py_True;
    Py_INCREF(ret);
    প্রত্যাগম ret;
}
#define ALWAYS_TRUE AlwaysTrue(রিক্ত, রিক্ত)

/***************/

স্থির সংরচনা PyMethodDef OutputMethods[] = {
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
    { রিক্ত,	    রিক্ত,				0,		রিক্ত}
};

স্থির OutputObject Output =
{
    PyObject_HEAD_INIT_TYPE(OutputType)
    0,
    0
};

স্থির OutputObject Error =
{
    PyObject_HEAD_INIT_TYPE(OutputType)
    0,
    1
};

    স্থির পূর্ণাঙ্ক
PythonIO_Init_io(শূন্য)
{
    যদি (PySys_SetObject("stdout", (PyObject *)(শূন্য *)&Output))
	প্রত্যাগম -1;
    যদি (PySys_SetObject("stderr", (PyObject *)(শূন্য *)&Error))
	প্রত্যাগম -1;

    যদি (PyErr_Occurred())
    {
	emsg(_(e_python_error_initialising_io_object));
	প্রত্যাগম -1;
    }

    প্রত্যাগম 0;
}

#যদি PY_VERSION_HEX < 0x030700f0
স্থির PyObject *call_load_module(বর্ণ *name, পূর্ণাঙ্ক len, PyObject *find_module_result);

প্রকারনির্দেশ সংরচনা
{
    PyObject_HEAD
    বর্ণ	*fullname;
    PyObject	*result;
} LoaderObject;
DEFINE_PY_TYPE_OBJECT(LoaderType);

    স্থির শূন্য
LoaderDestructor(PyObject *self_obj)
{
    LoaderObject *self = (LoaderObject*)self_obj;
    vim_free(self->fullname);
    Py_XDECREF(self->result);
    DESTRUCTOR_FINISH(self);
}

    স্থির PyObject *
LoaderLoadModule(LoaderObject *self, PyObject *args UNUSED)
{
    বর্ণ	*fullname = self->fullname;
    PyObject	*result = self->result;
    PyObject	*module;

    যদি (!fullname)
    {
	module = result ? result : Py_None;
	Py_INCREF(module);
	প্রত্যাগম module;
    }

    module = call_load_module(fullname, (পূর্ণাঙ্ক)STRLEN(fullname), result);

    self->fullname = রিক্ত;
    self->result = module;

    vim_free(fullname);
    Py_DECREF(result);

    যদি (!module)
    {
	যদি (PyErr_Occurred())
	    প্রত্যাগম রিক্ত;

	Py_INCREF(Py_None);
	প্রত্যাগম Py_None;
    }

    Py_INCREF(module);
    প্রত্যাগম module;
}

স্থির সংরচনা PyMethodDef LoaderMethods[] = {
    // name,	    function,				calling,	doc
    {"load_module", (PyCFunction)LoaderLoadModule,	METH_VARARGS,	""},
    { রিক্ত,	    রিক্ত,				0,		রিক্ত}
};
#endif

/*
 * Check to see whether a Vim error has been reported, or a keyboard
 * interrupt has been detected.
 */
    স্থির শূন্য
VimTryStart(শূন্য)
{
    ++trylevel;
}

    স্থির পূর্ণাঙ্ক
VimTryEnd(শূন্য)
{
    --trylevel;
    // Without this it stops processing all subsequent Vim script commands and
    // generates strange error messages if I e.g. try calling Test() in a cycle
    did_emsg = FALSE;
    // Keyboard interrupt should be preferred over anything else
    যদি (got_int)
    {
	যদি (did_throw)
	    discard_current_exception();
	got_int = FALSE;
	PyErr_SetNone(PyExc_KeyboardInterrupt);
	প্রত্যাগম -1;
    }
    অন্যথা যদি (বার্তা_list != রিক্ত && *বার্তা_list != রিক্ত)
    {
	পূর্ণাঙ্ক	should_free;
	বর্ণ	*বার্তা;

	বার্তা = get_exception_string(*বার্তা_list, ET_ERROR, রিক্ত, &should_free);

	যদি (বার্তা == রিক্ত)
	{
	    PyErr_NoMemory();
	    প্রত্যাগম -1;
	}

	PyErr_SetVim(বার্তা);

	free_global_msglist();

	যদি (should_free)
	    vim_free(বার্তা);

	প্রত্যাগম -1;
    }
    অন্যথা যদি (!did_throw)
	প্রত্যাগম (PyErr_Occurred() ? -1 : 0);
    // Python exception is preferred over Vim one; unlikely to occur though
    অন্যথা যদি (PyErr_Occurred())
    {
	discard_current_exception();
	প্রত্যাগম -1;
    }
    // Finally transform Vim script exception to python one
    অন্যথা
    {
	PyErr_SetVim((বর্ণ *)current_exception->মান);
	discard_current_exception();
	প্রত্যাগম -1;
    }
}

    স্থির পূর্ণাঙ্ক
VimCheckInterrupt(শূন্য)
{
    যদি (got_int)
    {
	PyErr_SetNone(PyExc_KeyboardInterrupt);
	প্রত্যাগম 1;
    }
    প্রত্যাগম 0;
}

// Vim module - Implementation

    স্থির PyObject *
VimCommand(PyObject *self UNUSED, PyObject *string)
{
    char_u	*cmd;
    PyObject	*ret;
    PyObject	*todecref;

    যদি (!(cmd = StringToChars(string, &todecref)))
	প্রত্যাগম রিক্ত;

    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();

    VimTryStart();
    do_cmdline_cmd(cmd);
    update_screen(UPD_VALID);

    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    যদি (VimTryEnd())
	ret = রিক্ত;
    অন্যথা
	ret = Py_None;

    Py_XINCREF(ret);
    Py_XDECREF(todecref);
    প্রত্যাগম ret;
}

/*
 * Function to translate a typval_T into a PyObject; this will recursively
 * translate lists/dictionaries into their Python equivalents.
 *
 * The depth parameter is to avoid infinite recursion, set it to 1 when
 * you call VimToPython.
 */
    স্থির PyObject *
VimToPython(typval_T *our_tv, পূর্ণাঙ্ক depth, PyObject *lookup_dict)
{
    PyObject	*ret;
    PyObject	*newObj;
    বর্ণ	ptrBuf[পরিমাণ(শূন্য *) * 2 + 3];

    // Avoid infinite recursion
    যদি (depth > 100)
    {
	Py_INCREF(Py_None);
	ret = Py_None;
	প্রত্যাগম ret;
    }

    // Check if we run into a recursive loop.  The item must be in lookup_dict
    // then and we can use it again.
    যদি ((our_tv->v_type == VAR_LIST && our_tv->vval.v_list != রিক্ত)
	    || (our_tv->v_type == VAR_TUPLE && our_tv->vval.v_tuple != রিক্ত)
	    || (our_tv->v_type == VAR_DICT && our_tv->vval.v_dict != রিক্ত))
    {
	sprintf(ptrBuf, "%p",
		our_tv->v_type == VAR_LIST ? (শূন্য *)our_tv->vval.v_list
		    : our_tv->v_type == VAR_TUPLE ?
			(শূন্য *)our_tv->vval.v_tuple
			: (শূন্য *)our_tv->vval.v_dict);

	যদি ((ret = PyDict_GetItemString(lookup_dict, ptrBuf)))
	{
	    Py_INCREF(ret);
	    প্রত্যাগম ret;
	}
    }

    যদি (our_tv->v_type == VAR_STRING)
	ret = PyString_FromString(our_tv->vval.v_string == রিক্ত
					? "" : (বর্ণ *)our_tv->vval.v_string);
    অন্যথা যদি (our_tv->v_type == VAR_সংখ্যা)
    {
	বর্ণ buf[NUMBUFLEN];

	// For backwards compatibility numbers are stored as strings.
	sprintf(buf, "%lld", (long long)our_tv->vval.v_সংখ্যা);
	ret = PyString_FromString((বর্ণ *)buf);
    }
    অন্যথা যদি (our_tv->v_type == VAR_FLOAT)
    {
	বর্ণ buf[NUMBUFLEN];

	sprintf(buf, "%f", our_tv->vval.v_float);
	ret = PyString_FromString((বর্ণ *)buf);
    }
    অন্যথা যদি (our_tv->v_type == VAR_LIST)
    {
	list_T		*list = our_tv->vval.v_list;
	listitem_T	*curr;

	যদি (list == রিক্ত)
	    প্রত্যাগম রিক্ত;

	যদি (!(ret = PyList_New(0)))
	    প্রত্যাগম রিক্ত;

	যদি (PyDict_SetItemString(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    প্রত্যাগম রিক্ত;
	}

	CHECK_LIST_MATERIALIZE(list);
	FOR_ALL_LIST_বস্তুসমূহ(list, curr)
	{
	    যদি (!(newObj = VimToPython(&curr->li_tv, depth + 1, lookup_dict)))
	    {
		Py_DECREF(ret);
		প্রত্যাগম রিক্ত;
	    }
	    যদি (PyList_Append(ret, newObj))
	    {
		Py_DECREF(newObj);
		Py_DECREF(ret);
		প্রত্যাগম রিক্ত;
	    }
	    Py_DECREF(newObj);
	}
    }
    অন্যথা যদি (our_tv->v_type == VAR_TUPLE)
    {
	tuple_T	*tuple = our_tv->vval.v_tuple;
	পূর্ণাঙ্ক	len;

	যদি (tuple == রিক্ত)
	    প্রত্যাগম রিক্ত;

	len = TUPLE_LEN(tuple);

	ret = PyTuple_New(len);
	যদি (ret == রিক্ত)
	    প্রত্যাগম রিক্ত;

	প্রতি (পূর্ণাঙ্ক idx = 0; idx < len; idx++)
	{
	    typval_T	*item_tv = TUPLE_ITEM(tuple, idx);

	    newObj = VimToPython(item_tv, depth + 1, lookup_dict);
	    যদি (!newObj)
	    {
		Py_DECREF(ret);
		প্রত্যাগম রিক্ত;
	    }
	    PyTuple_SetItem(ret, idx, newObj);
	}

	যদি (PyDict_SetItemString(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    প্রত্যাগম রিক্ত;
	}
    }
    অন্যথা যদি (our_tv->v_type == VAR_DICT)
    {

	hashtab_T	*ht;
	long_u		todo;
	hashitem_T	*hi;
	dictitem_T	*di;

	যদি (our_tv->vval.v_dict == রিক্ত)
	    প্রত্যাগম রিক্ত;
	ht = &our_tv->vval.v_dict->dv_hashtab;

	যদি (!(ret = PyDict_New()))
	    প্রত্যাগম রিক্ত;

	যদি (PyDict_SetItemString(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    প্রত্যাগম রিক্ত;
	}

	todo = ht->ht_used;
	প্রতি (hi = ht->ht_array; todo > 0; ++hi)
	{
	    যদি (!HASHITEM_EMPTY(hi))
	    {
		--todo;

		di = dict_lookup(hi);
		যদি (!(newObj = VimToPython(&di->di_tv, depth + 1, lookup_dict)))
		{
		    Py_DECREF(ret);
		    প্রত্যাগম রিক্ত;
		}
		যদি (PyDict_SetItemString(ret, (বর্ণ *)hi->hi_চাবি, newObj))
		{
		    Py_DECREF(ret);
		    Py_DECREF(newObj);
		    প্রত্যাগম রিক্ত;
		}
	    }
	}
    }
    অন্যথা যদি (our_tv->v_type == VAR_BOOL)
    {
	যদি (our_tv->vval.v_সংখ্যা == VVAL_FALSE)
	{
	    ret = Py_False;
	    Py_INCREF(ret);
	}
	অন্যথা
	{
	    ret = Py_True;
	    Py_INCREF(ret);
	}
	প্রত্যাগম ret;
    }
    অন্যথা যদি (our_tv->v_type == VAR_SPECIAL)
    {
	Py_INCREF(Py_None);
	ret = Py_None;
	প্রত্যাগম ret;
    }
    অন্যথা যদি (our_tv->v_type == VAR_BLOB)
	ret = PyBytes_FromStringAndSize(
		(বর্ণ*) our_tv->vval.v_blob->bv_ga.ga_data,
		(Py_ssize_t) our_tv->vval.v_blob->bv_ga.ga_len);
    অন্যথা
    {
	Py_INCREF(Py_None);
	ret = Py_None;
    }

    প্রত্যাগম ret;
}

    স্থির PyObject *
VimEval(PyObject *self UNUSED, PyObject *args)
{
    char_u	*রাশিমালা;
    typval_T	*our_tv;
    PyObject	*string;
    PyObject	*todecref;
    PyObject	*ret;
    PyObject	*lookup_dict;

    যদি (!PyArg_ParseTuple(args, "O", &string))
	প্রত্যাগম রিক্ত;

    যদি (!(রাশিমালা = StringToChars(string, &todecref)))
	প্রত্যাগম রিক্ত;

    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    VimTryStart();
    our_tv = eval_রাশিমালা(রাশিমালা, রিক্ত);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    Py_XDECREF(todecref);

    যদি (VimTryEnd())
	প্রত্যাগম রিক্ত;

    যদি (our_tv == রিক্ত)
    {
	PyErr_SET_VIM(N_("invalid expression"));
	প্রত্যাগম রিক্ত;
    }

    // Convert the Vim type into a Python type.  Create a dictionary that's
    // used to check for recursive loops.
    যদি (!(lookup_dict = PyDict_New()))
	ret = রিক্ত;
    অন্যথা
    {
	ret = VimToPython(our_tv, 1, lookup_dict);
	Py_DECREF(lookup_dict);
    }


    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    free_tv(our_tv);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    প্রত্যাগম ret;
}

স্থির PyObject *ConvertToPyObject(typval_T *);

    স্থির PyObject *
VimEvalPy(PyObject *self UNUSED, PyObject *string)
{
    typval_T	*our_tv;
    PyObject	*ret;
    char_u	*রাশিমালা;
    PyObject	*todecref;

    যদি (!(রাশিমালা = StringToChars(string, &todecref)))
	প্রত্যাগম রিক্ত;

    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    VimTryStart();
    our_tv = eval_রাশিমালা(রাশিমালা, রিক্ত);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    Py_XDECREF(todecref);

    যদি (VimTryEnd())
	প্রত্যাগম রিক্ত;

    যদি (our_tv == রিক্ত)
    {
	PyErr_SET_VIM(N_("invalid expression"));
	প্রত্যাগম রিক্ত;
    }

    ret = ConvertToPyObject(our_tv);
    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    free_tv(our_tv);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    প্রত্যাগম ret;
}

    স্থির PyObject *
VimStrwidth(PyObject *self UNUSED, PyObject *string)
{
    char_u	*str;
    PyObject	*todecref;
    পূর্ণাঙ্ক		len;

    যদি (!(str = StringToChars(string, &todecref)))
	প্রত্যাগম রিক্ত;

    len = mb_string2cells(str, (পূর্ণাঙ্ক)STRLEN(str));

    Py_XDECREF(todecref);

    প্রত্যাগম PyLong_FromLong(len);
}

    স্থির PyObject *
_VimChdir(PyObject *_chdir, PyObject *args, PyObject *kwargs)
{
    PyObject	*ret;
    PyObject	*newwd;
    PyObject	*todecref;
    char_u	*new_dir;

    যদি (_chdir == রিক্ত)
	প্রত্যাগম রিক্ত;
    যদি (!(ret = PyObject_Call(_chdir, args, kwargs)))
	প্রত্যাগম রিক্ত;

    যদি (!(newwd = PyObject_CallFunctionObjArgs(py_getcwd, রিক্ত)))
    {
	Py_DECREF(ret);
	প্রত্যাগম রিক্ত;
    }

    যদি (!(new_dir = StringToChars(newwd, &todecref)))
    {
	Py_DECREF(ret);
	Py_DECREF(newwd);
	প্রত্যাগম রিক্ত;
    }

    VimTryStart();

    যদি (vim_chdir(new_dir))
    {
	Py_DECREF(ret);
	Py_DECREF(newwd);
	Py_XDECREF(todecref);

	যদি (VimTryEnd())
	    প্রত্যাগম রিক্ত;

	PyErr_SET_VIM(N_("failed to change directory"));
	প্রত্যাগম রিক্ত;
    }

    Py_DECREF(newwd);
    Py_XDECREF(todecref);

    post_chdir(CDSCOPE_GLOBAL);

    যদি (VimTryEnd())
    {
	Py_DECREF(ret);
	প্রত্যাগম রিক্ত;
    }

    প্রত্যাগম ret;
}

    স্থির PyObject *
VimChdir(PyObject *self UNUSED, PyObject *args, PyObject *kwargs)
{
    প্রত্যাগম _VimChdir(py_chdir, args, kwargs);
}

    স্থির PyObject *
VimFchdir(PyObject *self UNUSED, PyObject *args, PyObject *kwargs)
{
    প্রত্যাগম _VimChdir(py_fchdir, args, kwargs);
}

প্রকারনির্দেশ সংরচনা {
    PyObject	*callable;
    PyObject	*result;
} map_rtp_data;

    স্থির শূন্য
map_rtp_callback(char_u *path, শূন্য *_data)
{
    শূন্য	**data = (শূন্য **) _data;
    PyObject	*pathObject;
    map_rtp_data	*mr_data = *((map_rtp_data **) data);

    যদি (!(pathObject = PyString_FromString((বর্ণ *)path)))
    {
	*data = রিক্ত;
	প্রত্যাগম;
    }

    mr_data->result = PyObject_CallFunctionObjArgs(mr_data->callable,
						   pathObject, রিক্ত);

    Py_DECREF(pathObject);

    যদি (!mr_data->result || mr_data->result != Py_None)
	*data = রিক্ত;
    অন্যথা
    {
	Py_DECREF(mr_data->result);
	mr_data->result = রিক্ত;
    }
}

    স্থির PyObject *
VimForeachRTP(PyObject *self UNUSED, PyObject *callable)
{
    map_rtp_data	data;

    data.callable = callable;
    data.result = রিক্ত;

    do_in_runtimepath(রিক্ত, 0, &map_rtp_callback, &data);

    যদি (data.result == রিক্ত)
    {
	যদি (PyErr_Occurred())
	    প্রত্যাগম রিক্ত;
	অন্যথা
	{
	    Py_INCREF(Py_None);
	    প্রত্যাগম Py_None;
	}
    }
    প্রত্যাগম data.result;
}

/*
 * _vim_runtimepath_ special path implementation.
 */

    স্থির শূন্য
map_finder_callback(char_u *path, শূন্য *_data)
{
    শূন্য	**data = (শূন্য **) _data;
    PyObject	*list = *((PyObject **) data);
    PyObject	*pathObject1, *pathObject2;
    বর্ণ	*pathbuf;
    size_t	pathlen;

    pathlen = STRLEN(path);

#যদি PY_MAJOR_VERSION < 3
# define PY_MAIN_DIR_STRING "python2"
#অন্যথা
# define PY_MAIN_DIR_STRING "python3"
#endif
#define PY_ALTERNATE_DIR_STRING "pythonx"

#define PYTHONX_STRING_LENGTH 7 // STRLEN("pythonx")
    যদি (!(pathbuf = PyMem_New(বর্ণ,
		    pathlen + STRLEN(PATHSEPSTR) + PYTHONX_STRING_LENGTH + 1)))
    {
	PyErr_NoMemory();
	*data = রিক্ত;
	প্রত্যাগম;
    }

    mch_memmove(pathbuf, path, pathlen + 1);
    add_pathsep((char_u *) pathbuf);

    pathlen = STRLEN(pathbuf);
    mch_memmove(pathbuf + pathlen, PY_MAIN_DIR_STRING,
	    PYTHONX_STRING_LENGTH + 1);

    যদি (!(pathObject1 = PyString_FromString(pathbuf)))
    {
	*data = রিক্ত;
	PyMem_Free(pathbuf);
	প্রত্যাগম;
    }

    mch_memmove(pathbuf + pathlen, PY_ALTERNATE_DIR_STRING,
	    PYTHONX_STRING_LENGTH + 1);

    যদি (!(pathObject2 = PyString_FromString(pathbuf)))
    {
	Py_DECREF(pathObject1);
	PyMem_Free(pathbuf);
	*data = রিক্ত;
	প্রত্যাগম;
    }

    PyMem_Free(pathbuf);

    যদি (PyList_Append(list, pathObject1)
	    || PyList_Append(list, pathObject2))
	*data = রিক্ত;

    Py_DECREF(pathObject1);
    Py_DECREF(pathObject2);
}

    স্থির PyObject *
Vim_GetPaths(PyObject *self UNUSED, PyObject *args UNUSED)
{
    PyObject	*ret;

    যদি (!(ret = PyList_New(0)))
	প্রত্যাগম রিক্ত;

    do_in_runtimepath(রিক্ত, 0, &map_finder_callback, ret);

    যদি (PyErr_Occurred())
    {
	Py_DECREF(ret);
	প্রত্যাগম রিক্ত;
    }

    প্রত্যাগম ret;
}

#যদি PY_VERSION_HEX >= 0x030700f0
    স্থির PyObject *
FinderFindSpec(PyObject *self, PyObject *args)
{
    বর্ণ	*fullname;
    PyObject	*paths;
    PyObject	*target = Py_None;
    PyObject	*spec;

    যদি (!PyArg_ParseTuple(args, "s|O", &fullname, &target))
	প্রত্যাগম রিক্ত;

    যদি (!(paths = Vim_GetPaths(self, রিক্ত)))
	প্রত্যাগম রিক্ত;

    spec = PyObject_CallFunction(py_find_spec, "sOO", fullname, paths, target);

    Py_DECREF(paths);

    যদি (!spec)
    {
	যদি (PyErr_Occurred())
	    প্রত্যাগম রিক্ত;

	Py_INCREF(Py_None);
	প্রত্যাগম Py_None;
    }

    প্রত্যাগম spec;
}

    স্থির PyObject *
FinderFindModule(PyObject* self UNUSED, PyObject* args UNUSED)
{
    // Apparently returning None works.
    Py_INCREF(Py_None);
    প্রত্যাগম Py_None;
}
#অন্যথা
    স্থির PyObject *
call_load_module(বর্ণ *name, পূর্ণাঙ্ক len, PyObject *find_module_result)
{
    PyObject	*fd, *pathname, *description;

    যদি (!PyTuple_Check(find_module_result))
    {
	PyErr_FORMAT_TYPE(
		N_("expected 3-tuple as imp.find_module() result, but got %s"),
		find_module_result);
	প্রত্যাগম রিক্ত;
    }
    যদি (PyTuple_GET_SIZE(find_module_result) != 3)
    {
	PyErr_FORMAT(PyExc_TypeError,
		N_("expected 3-tuple as imp.find_module() result, but got "
		   "tuple of size %d"),
		(পূর্ণাঙ্ক) PyTuple_GET_SIZE(find_module_result));
	প্রত্যাগম রিক্ত;
    }

    যদি (!(fd = PyTuple_GET_ITEM(find_module_result, 0))
	    || !(pathname = PyTuple_GET_ITEM(find_module_result, 1))
	    || !(description = PyTuple_GET_ITEM(find_module_result, 2)))
    {
	PyErr_SET_STRING(PyExc_RuntimeError,
		N_("internal error: imp.find_module returned tuple with NULL"));
	প্রত্যাগম রিক্ত;
    }

    প্রত্যাগম PyObject_CallFunction(py_load_module,
	    "s#OOO", name, len, fd, pathname, description);
}

    স্থির PyObject *
find_module(বর্ণ *fullname, বর্ণ *tail, PyObject *new_path)
{
    PyObject	*find_module_result;
    PyObject	*module;
    বর্ণ	*dot;

    যদি ((dot = (বর্ণ *)vim_strchr((char_u *) tail, '.')))
    {
	/*
	 * There is a dot in the name: call find_module recursively without the
	 * first component
	 */
	PyObject	*newest_path;
	পূর্ণাঙ্ক		partlen = (পূর্ণাঙ্ক) (dot - 1 - tail);

	যদি (!(find_module_result = PyObject_CallFunction(py_find_module,
			"s#O", tail, partlen, new_path)))
	{
	    যদি (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_ImportError))
		PyErr_Clear();
	    প্রত্যাগম রিক্ত;
	}

	যদি (!(module = call_load_module(
			fullname,
			((পূর্ণাঙ্ক) (tail - fullname)) + partlen,
			find_module_result)))
	{
	    Py_DECREF(find_module_result);
	    প্রত্যাগম রিক্ত;
	}

	Py_DECREF(find_module_result);

	যদি (!(newest_path = PyObject_GetAttrString(module, "__path__")))
	{
	    Py_DECREF(module);
	    প্রত্যাগম রিক্ত;
	}

	Py_DECREF(module);

	find_module_result = find_module(fullname, dot + 1, newest_path);

	Py_DECREF(newest_path);

	প্রত্যাগম find_module_result;
    }
    অন্যথা
    {
	যদি (!(find_module_result = PyObject_CallFunction(py_find_module,
			"sO", tail, new_path)))
	{
	    যদি (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_ImportError))
		PyErr_Clear();
	    প্রত্যাগম রিক্ত;
	}

	প্রত্যাগম find_module_result;
    }
}

    স্থির PyObject *
FinderFindModule(PyObject *self, PyObject *args)
{
    বর্ণ	*fullname;
    PyObject	*result;
    PyObject	*new_path;
    LoaderObject	*loader;

    যদি (!PyArg_ParseTuple(args, "s", &fullname))
	প্রত্যাগম রিক্ত;

    যদি (!(new_path = Vim_GetPaths(self, রিক্ত)))
	প্রত্যাগম রিক্ত;

    result = find_module(fullname, fullname, new_path);

    Py_DECREF(new_path);

    যদি (!result)
    {
	যদি (PyErr_Occurred())
	    প্রত্যাগম রিক্ত;

	Py_INCREF(Py_None);
	প্রত্যাগম Py_None;
    }

    যদি (!(fullname = (বর্ণ *)vim_strsave((char_u *)fullname)))
    {
	Py_DECREF(result);
	PyErr_NoMemory();
	প্রত্যাগম রিক্ত;
    }

    যদি (!(loader = PyObject_NEW(LoaderObject, LoaderTypePtr)))
    {
	vim_free(fullname);
	Py_DECREF(result);
	প্রত্যাগম রিক্ত;
    }

    loader->fullname = fullname;
    loader->result = result;

    প্রত্যাগম (PyObject *) loader;
}
#endif

    স্থির PyObject *
VimPathHook(PyObject *self UNUSED, PyObject *args)
{
    বর্ণ	*path;

    যদি (PyArg_ParseTuple(args, "s", &path)
	    && STRCMP(path, vim_special_path) == 0)
    {
	Py_INCREF(vim_module);
	প্রত্যাগম vim_module;
    }

    PyErr_Clear();
    PyErr_SetNone(PyExc_ImportError);
    প্রত্যাগম রিক্ত;
}

/*
 * Vim module - Definitions
 */

স্থির সংরচনা PyMethodDef VimMethods[] = {
    // name,	    function,			calling,			documentation
    {"command",	    VimCommand,			METH_O,				"Execute a Vim ex-mode command" },
    {"eval",	    VimEval,			METH_VARARGS,			"Evaluate an expression using Vim evaluator" },
    {"bindeval",    VimEvalPy,			METH_O,				"Like eval(), but returns objects attached to Vim ones"},
    {"strwidth",    VimStrwidth,		METH_O,				"Screen string width, counts <Tab> as having width 1"},
    {"chdir",	    (PyCFunction)(শূন্য *)VimChdir,	METH_VARARGS|METH_KEYWORDS,	"Change directory"},
    {"fchdir",	    (PyCFunction)(শূন্য *)VimFchdir,	METH_VARARGS|METH_KEYWORDS,	"Change directory"},
    {"foreach_rtp", VimForeachRTP,		METH_O,				"Call given callable for each path in &rtp"},
#যদি PY_VERSION_HEX >= 0x030700f0
    {"find_spec",   FinderFindSpec,		METH_VARARGS,			"Internal use only, returns spec object for any input it receives"},
#endif
    {"find_module", FinderFindModule,		METH_VARARGS,			"Internal use only, returns loader object for any input it receives"},
    {"path_hook",   VimPathHook,		METH_VARARGS,			"Hook function to install in sys.path_hooks"},
    {"_get_paths",  (PyCFunction)Vim_GetPaths,	METH_NOARGS,			"Get &rtp-based additions to sys.path"},
    { রিক্ত,	    রিক্ত,			0,				রিক্ত}
};

/*
 * Generic iterator object
 */

DEFINE_PY_TYPE_OBJECT(IterType);

প্রকারনির্দেশ PyObject *(*nextfun)(শূন্য **);
প্রকারনির্দেশ শূন্য (*destructorfun)(শূন্য *);
প্রকারনির্দেশ পূর্ণাঙ্ক (*traversefun)(শূন্য *, visitproc, শূন্য *);
প্রকারনির্দেশ পূর্ণাঙ্ক (*clearfun)(শূন্য **);

// Main purpose of this object is removing the need for do python
// initialization (i.e. PyType_Ready and setting type attributes) for a big
// bunch of objects.
প্রকারনির্দেশ সংরচনা
{
    PyObject_HEAD
    শূন্য *cur;
    nextfun next;
    destructorfun destruct;
    traversefun traverse;
    clearfun clear;
    PyObject *iter_object;
} IterObject;

    স্থির PyObject *
IterNew(শূন্য *start, destructorfun destruct, nextfun next, traversefun traverse,
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

    যদি (iter_object)
	Py_INCREF(iter_object);

    প্রত্যাগম (PyObject *)(self);
}

    স্থির শূন্য
IterDestructor(PyObject *self_obj)
{
    IterObject *self = (IterObject*)self_obj;
    যদি (self->iter_object)
	Py_DECREF(self->iter_object);
    PyObject_GC_UnTrack((শূন্য *)(self));
    self->destruct(self->cur);
    PyObject_GC_Del((শূন্য *)(self));
}

    স্থির পূর্ণাঙ্ক
IterTraverse(PyObject *self_obj, visitproc visit, শূন্য *arg)
{
    IterObject *self = (IterObject*)self_obj;
    যদি (self->traverse != রিক্ত)
	প্রত্যাগম self->traverse(self->cur, visit, arg);
    অন্যথা
	প্রত্যাগম 0;
}

// Mac OSX defines clear() somewhere.
#ifdef clear
# undef clear
#endif

    স্থির পূর্ণাঙ্ক
IterClear(PyObject *self_obj)
{
    IterObject *self = (IterObject*)self_obj;
    যদি (self->clear != রিক্ত)
	প্রত্যাগম self->clear(&self->cur);
    অন্যথা
	প্রত্যাগম 0;
}

    স্থির PyObject *
IterNext(PyObject *self_obj)
{
    IterObject *self = (IterObject*)self_obj;
    প্রত্যাগম self->next(&self->cur);
}

    স্থির PyObject *
IterIter(PyObject *self)
{
    Py_INCREF(self);
    প্রত্যাগম self;
}

প্রকারনির্দেশ সংরচনা pylinkedlist_S {
    সংরচনা pylinkedlist_S	*pll_next;
    সংরচনা pylinkedlist_S	*pll_prev;
    PyObject			*pll_obj;
} pylinkedlist_T;

স্থির pylinkedlist_T *lastdict = রিক্ত;
স্থির pylinkedlist_T *lastlist = রিক্ত;
স্থির pylinkedlist_T *lasttuple = রিক্ত;
স্থির pylinkedlist_T *lastfunc = রিক্ত;

    স্থির শূন্য
pyll_remove(pylinkedlist_T *ref, pylinkedlist_T **last)
{
    যদি (ref->pll_prev == রিক্ত)
    {
	যদি (ref->pll_next == রিক্ত)
	{
	    *last = রিক্ত;
	    প্রত্যাগম;
	}
    }
    অন্যথা
	ref->pll_prev->pll_next = ref->pll_next;

    যদি (ref->pll_next == রিক্ত)
	*last = ref->pll_prev;
    অন্যথা
	ref->pll_next->pll_prev = ref->pll_prev;
}

    স্থির শূন্য
pyll_add(PyObject *self, pylinkedlist_T *ref, pylinkedlist_T **last)
{
    যদি (*last == রিক্ত)
	ref->pll_prev = রিক্ত;
    অন্যথা
    {
	(*last)->pll_next = ref;
	ref->pll_prev = *last;
    }
    ref->pll_next = রিক্ত;
    ref->pll_obj = self;
    *last = ref;
}

DEFINE_PY_TYPE_OBJECT(DictionaryType);

প্রকারনির্দেশ সংরচনা
{
    PyObject_HEAD
    dict_T	*dict;
    pylinkedlist_T	ref;
} DictionaryObject;

স্থির PyObject *DictionaryUpdate(DictionaryObject *, PyObject *, PyObject *);

#define NEW_DICTIONARY(dict) DictionaryNew(DictionaryTypePtr, dict)

    স্থির PyObject *
DictionaryNew(PyTypeObject *subtype, dict_T *dict)
{
    DictionaryObject	*self;

    self = (DictionaryObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    যদি (self == রিক্ত)
	প্রত্যাগম রিক্ত;
    self->dict = dict;
    ++dict->dv_refcount;

    pyll_add((PyObject *)(self), &self->ref, &lastdict);

    প্রত্যাগম (PyObject *)(self);
}

    স্থির dict_T *
py_dict_alloc(শূন্য)
{
    dict_T	*ret;

    যদি (!(ret = dict_alloc()))
    {
	PyErr_NoMemory();
	প্রত্যাগম রিক্ত;
    }
    ++ret->dv_refcount;

    প্রত্যাগম ret;
}

    স্থির PyObject *
DictionaryConstructor(PyTypeObject *subtype, PyObject *args, PyObject *kwargs)
{
    DictionaryObject	*self;
    dict_T	*dict;

    যদি (!(dict = py_dict_alloc()))
	প্রত্যাগম রিক্ত;

    self = (DictionaryObject *) DictionaryNew(subtype, dict);

    --dict->dv_refcount;

    যদি (kwargs || PyTuple_Size(args))
    {
	PyObject	*tmp;
	যদি (!(tmp = DictionaryUpdate(self, args, kwargs)))
	{
	    Py_DECREF(self);
	    প্রত্যাগম রিক্ত;
	}

	Py_DECREF(tmp);
    }

    প্রত্যাগম (PyObject *)(self);
}

    স্থির শূন্য
DictionaryDestructor(PyObject *self_obj)
{
    DictionaryObject *self = (DictionaryObject*)self_obj;
    pyll_remove(&self->ref, &lastdict);
    dict_unref(self->dict);

    DESTRUCTOR_FINISH(self);
}

স্থির বর্ণ *DictionaryAttrs[] = {
    "locked", "scope",
    রিক্ত
};

    স্থির PyObject *
DictionaryDir(PyObject *self, PyObject *args UNUSED)
{
    প্রত্যাগম ObjectDir(self, DictionaryAttrs);
}

    স্থির পূর্ণাঙ্ক
DictionarySetattr(PyObject *self_obj, বর্ণ *name, PyObject *valObject)
{
    DictionaryObject *self = (DictionaryObject*)self_obj;
    যদি (valObject == রিক্ত)
    {
	PyErr_SET_STRING(PyExc_AttributeError,
		N_("cannot delete vim.Dictionary attributes"));
	প্রত্যাগম -1;
    }

    যদি (strcmp(name, "locked") == 0)
    {
	যদি (self->dict->dv_lock == VAR_FIXED)
	{
	    PyErr_SET_STRING(PyExc_TypeError,
		    N_("cannot modify fixed dictionary"));
	    প্রত্যাগম -1;
	}
	অন্যথা
	{
	    পূর্ণাঙ্ক		istrue = PyObject_IsTrue(valObject);
	    যদি (istrue == -1)
		প্রত্যাগম -1;
	    অন্যথা যদি (istrue)
		self->dict->dv_lock = VAR_LOCKED;
	    অন্যথা
		self->dict->dv_lock = 0;
	}
	প্রত্যাগম 0;
    }
    অন্যথা
    {
	PyErr_FORMAT(PyExc_AttributeError, N_("cannot set attribute %s"), name);
	প্রত্যাগম -1;
    }
}

    স্থির PyInt
DictionaryLength(DictionaryObject *self)
{
    প্রত্যাগম ((PyInt) (self->dict->dv_hashtab.ht_used));
}

#define DICT_FLAG_HAS_DEFAULT	0x01
#define DICT_FLAG_বের_করো		0x02
#define DICT_FLAG_NONE_DEFAULT	0x04
#define DICT_FLAG_RETURN_BOOL	0x08 // Incompatible with DICT_FLAG_POP
#define DICT_FLAG_RETURN_PAIR	0x10

    স্থির PyObject *
_DictionaryItem(DictionaryObject *self, PyObject *args, পূর্ণাঙ্ক flags)
{
    PyObject	*চাবি_Object;
    PyObject	*defObject = ((flags & DICT_FLAG_NONE_DEFAULT)? Py_None : রিক্ত);
    PyObject	*ret;
    char_u	*চাবি;
    dictitem_T	*di;
    dict_T	*dict = self->dict;
    hashitem_T	*hi;
    PyObject	*todecref;

    যদি (flags & DICT_FLAG_HAS_DEFAULT)
    {
	যদি (!PyArg_ParseTuple(args, "O|O", &চাবি_Object, &defObject))
	    প্রত্যাগম রিক্ত;
    }
    অন্যথা
	চাবি_Object = args;

    যদি (flags & DICT_FLAG_RETURN_BOOL)
	defObject = Py_False;

    যদি (!(চাবি = StringToChars(চাবি_Object, &todecref)))
	প্রত্যাগম রিক্ত;

    যদি (*চাবি == NUL)
    {
	RAISE_NO_EMPTY_চাবিগুলি;
	Py_XDECREF(todecref);
	প্রত্যাগম রিক্ত;
    }

    hi = hash_find(&dict->dv_hashtab, চাবি);

    Py_XDECREF(todecref);

    যদি (HASHITEM_EMPTY(hi))
    {
	যদি (defObject)
	{
	    Py_INCREF(defObject);
	    প্রত্যাগম defObject;
	}
	অন্যথা
	{
	    PyErr_SetObject(Py_Exc_চাবি_Error, চাবি_Object);
	    প্রত্যাগম রিক্ত;
	}
    }
    অন্যথা যদি (flags & DICT_FLAG_RETURN_BOOL)
    {
	ret = Py_True;
	Py_INCREF(ret);
	প্রত্যাগম ret;
    }

    di = dict_lookup(hi);

    যদি (!(ret = ConvertToPyObject(&di->di_tv)))
	প্রত্যাগম রিক্ত;

    যদি (flags & DICT_FLAG_বের_করো)
    {
	যদি (dict->dv_lock)
	{
	    RAISE_LOCKED_DICTIONARY;
	    Py_DECREF(ret);
	    প্রত্যাগম রিক্ত;
	}

	hash_remove(&dict->dv_hashtab, hi, "Python remove variable");
	dictitem_free(di);
    }

    প্রত্যাগম ret;
}

    স্থির PyObject *
DictionaryItem(DictionaryObject *self, PyObject *চাবি_Object)
{
    প্রত্যাগম _DictionaryItem(self, চাবি_Object, 0);
}

    স্থির পূর্ণাঙ্ক
DictionaryContains(DictionaryObject *self, PyObject *চাবি_Object)
{
    PyObject	*rObj = _DictionaryItem(self, চাবি_Object, DICT_FLAG_RETURN_BOOL);
    পূর্ণাঙ্ক		ret;

    যদি (rObj == রিক্ত)
	প্রত্যাগম -1;

    ret = (rObj == Py_True);

    Py_DECREF(rObj);

    প্রত্যাগম ret;
}

প্রকারনির্দেশ সংরচনা
{
    পূর্ণাঙ্ক		dii_changed;
    hashtab_T	*dii_ht;
    hashitem_T	*dii_hi;
    long_u	dii_todo;
} dictiterinfo_T;

    স্থির PyObject *
DictionaryIterNext(শূন্য **arg)
{
    PyObject	*ret;
    dictiterinfo_T **dii = (dictiterinfo_T**)arg;

    যদি (!(*dii)->dii_todo)
	প্রত্যাগম রিক্ত;

    যদি ((*dii)->dii_ht->ht_changed != (*dii)->dii_changed)
    {
	PyErr_SET_STRING(PyExc_RuntimeError,
		N_("hashtab changed during iteration"));
	প্রত্যাগম রিক্ত;
    }

    যাবত্ (((*dii)->dii_todo) && HASHITEM_EMPTY((*dii)->dii_hi))
	++((*dii)->dii_hi);

    --((*dii)->dii_todo);

    যদি (!(ret = PyBytes_FromString((বর্ণ *)(*dii)->dii_hi->hi_চাবি)))
	প্রত্যাগম রিক্ত;

    প্রত্যাগম ret;
}

    স্থির PyObject *
DictionaryIter(PyObject *self_obj)
{
    DictionaryObject	*self = (DictionaryObject*)self_obj;
    dictiterinfo_T	*dii;
    hashtab_T		*ht;

    যদি (!(dii = PyMem_New(dictiterinfo_T, 1)))
    {
	PyErr_NoMemory();
	প্রত্যাগম রিক্ত;
    }

    ht = &self->dict->dv_hashtab;
    dii->dii_changed = ht->ht_changed;
    dii->dii_ht = ht;
    dii->dii_hi = ht->ht_array;
    dii->dii_todo = ht->ht_used;

    প্রত্যাগম IterNew(dii,
	    PyMem_Free, DictionaryIterNext,
	    রিক্ত, রিক্ত, (PyObject *)self);
}

    স্থির পূর্ণাঙ্ক
DictionaryAssItem(
	DictionaryObject *self, PyObject *চাবি_Object, PyObject *valObject)
{
    char_u	*চাবি;
    typval_T	tv;
    dict_T	*dict = self->dict;
    dictitem_T	*di;
    PyObject	*todecref;

    যদি (dict->dv_lock)
    {
	RAISE_LOCKED_DICTIONARY;
	প্রত্যাগম -1;
    }

    যদি (!(চাবি = StringToChars(চাবি_Object, &todecref)))
	প্রত্যাগম -1;

    যদি (*চাবি == NUL)
    {
	RAISE_NO_EMPTY_চাবিগুলি;
	Py_XDECREF(todecref);
	প্রত্যাগম -1;
    }

    di = dict_find(dict, চাবি, -1);

    যদি (valObject == রিক্ত)
    {
	hashitem_T	*hi;

	যদি (di == রিক্ত)
	{
	    Py_XDECREF(todecref);
	    PyErr_SetObject(Py_Exc_চাবি_Error, চাবি_Object);
	    প্রত্যাগম -1;
	}
	hi = hash_find(&dict->dv_hashtab, di->di_চাবি);
	hash_remove(&dict->dv_hashtab, hi, "Python remove item");
	dictitem_free(di);
	Py_XDECREF(todecref);
	প্রত্যাগম 0;
    }

    যদি (ConvertFromPyObject(valObject, &tv) == -1)
    {
	Py_XDECREF(todecref);
	প্রত্যাগম -1;
    }

    যদি (di == রিক্ত)
    {
	যদি (!(di = dictitem_alloc(চাবি)))
	{
	    Py_XDECREF(todecref);
	    PyErr_NoMemory();
	    প্রত্যাগম -1;
	}
	di->di_tv.v_type = VAR_UNKNOWN;

	যদি (dict_add(dict, di) == FAIL)
	{
	    dictitem_free(di);
	    RAISE_চাবি_ADD_FAIL(চাবি);
	    Py_XDECREF(todecref);
	    প্রত্যাগম -1;
	}
    }
    অন্যথা
	clear_tv(&di->di_tv);

    Py_XDECREF(todecref);

    copy_tv(&tv, &di->di_tv);
    clear_tv(&tv);
    প্রত্যাগম 0;
}

প্রকারনির্দেশ PyObject *(*hi_to_py)(hashitem_T *);

    স্থির PyObject *
DictionaryListObjects(DictionaryObject *self, hi_to_py hiconvert)
{
    dict_T	*dict = self->dict;
    long_u	todo = dict->dv_hashtab.ht_used;
    Py_ssize_t	i = 0;
    PyObject	*ret;
    hashitem_T	*hi;
    PyObject	*newObj;

    ret = PyList_New(todo);
    প্রতি (hi = dict->dv_hashtab.ht_array; todo > 0; ++hi)
    {
	যদি (!HASHITEM_EMPTY(hi))
	{
	    যদি (!(newObj = hiconvert(hi)))
	    {
		Py_DECREF(ret);
		প্রত্যাগম রিক্ত;
	    }
	    PyList_SET_ITEM(ret, i, newObj);
	    --todo;
	    ++i;
	}
    }
    প্রত্যাগম ret;
}

    স্থির PyObject *
dict_চাবি(hashitem_T *hi)
{
    প্রত্যাগম PyBytes_FromString((বর্ণ *)(hi->hi_চাবি));
}

    স্থির PyObject *
Dictionary_List_চাবিগুলি(DictionaryObject *self, PyObject *args UNUSED)
{
    প্রত্যাগম DictionaryListObjects(self, dict_চাবি);
}

    স্থির PyObject *
dict_val(hashitem_T *hi)
{
    dictitem_T	*di;

    di = dict_lookup(hi);
    প্রত্যাগম ConvertToPyObject(&di->di_tv);
}

    স্থির PyObject *
Dictionary_List_মানসমূহ(DictionaryObject *self, PyObject *args UNUSED)
{
    প্রত্যাগম DictionaryListObjects(self, dict_val);
}

    স্থির PyObject *
dict_item(hashitem_T *hi)
{
    PyObject	*চাবি_Object;
    PyObject	*valObject;
    PyObject	*ret;

    যদি (!(চাবি_Object = dict_চাবি(hi)))
	প্রত্যাগম রিক্ত;

    যদি (!(valObject = dict_val(hi)))
    {
	Py_DECREF(চাবি_Object);
	প্রত্যাগম রিক্ত;
    }

    ret = Py_Build_মান("(OO)", চাবি_Object, valObject);

    Py_DECREF(চাবি_Object);
    Py_DECREF(valObject);

    প্রত্যাগম ret;
}

    স্থির PyObject *
Dictionary_List_বস্তুসমূহ(DictionaryObject *self, PyObject *args UNUSED)
{
    প্রত্যাগম DictionaryListObjects(self, dict_item);
}

    স্থির PyObject *
DictionaryUpdate(DictionaryObject *self, PyObject *args, PyObject *kwargs)
{
    dict_T	*dict = self->dict;

    যদি (dict->dv_lock)
    {
	RAISE_LOCKED_DICTIONARY;
	প্রত্যাগম রিক্ত;
    }

    যদি (kwargs)
    {
	typval_T	tv;

	যদি (ConvertFromPyMapping(kwargs, &tv) == -1)
	    প্রত্যাগম রিক্ত;

	VimTryStart();
	dict_extend(self->dict, tv.vval.v_dict, (char_u *) "force", রিক্ত);
	clear_tv(&tv);
	যদি (VimTryEnd())
	    প্রত্যাগম রিক্ত;
    }
    অন্যথা
    {
	PyObject	*obj = রিক্ত;

	যদি (!PyArg_ParseTuple(args, "|O", &obj))
	    প্রত্যাগম রিক্ত;

	যদি (obj == রিক্ত)
	{
	    Py_INCREF(Py_None);
	    প্রত্যাগম Py_None;
	}

	যদি (PyObject_HasAttrString(obj, "keys"))
	    প্রত্যাগম DictionaryUpdate(self, রিক্ত, obj);
	অন্যথা
	{
	    PyObject	*iterator;
	    PyObject	*item;

	    যদি (!(iterator = PyObject_GetIter(obj)))
		প্রত্যাগম রিক্ত;

	    যাবত্ ((item = PyIter_Next(iterator)))
	    {
		PyObject	*fast;
		PyObject	*চাবি_Object;
		PyObject	*valObject;
		PyObject	*todecref;
		char_u		*চাবি;
		dictitem_T	*di;
		hashitem_T	*hi;

		যদি (!(fast = PySequence_Fast(item, "")))
		{
		    Py_DECREF(iterator);
		    Py_DECREF(item);
		    প্রত্যাগম রিক্ত;
		}

		Py_DECREF(item);

		যদি (PySequence_Size(fast) != 2)
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    PyErr_FORMAT(Py_Exc_মান_Error,
			    N_("expected sequence element of size 2, "
			    "but got sequence of size %d"),
			    (পূর্ণাঙ্ক) PySequence_Size(fast));
		    প্রত্যাগম রিক্ত;
		}

		চাবি_Object = PySequence_GetItem(fast, 0);

		যদি (!(চাবি = StringToChars(চাবি_Object, &todecref)))
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    প্রত্যাগম রিক্ত;
		}

		di = dictitem_alloc(চাবি);

		Py_XDECREF(todecref);

		যদি (di == রিক্ত)
		{
		    Py_DECREF(fast);
		    Py_DECREF(iterator);
		    PyErr_NoMemory();
		    প্রত্যাগম রিক্ত;
		}
		di->di_tv.v_type = VAR_UNKNOWN;

		valObject = PySequence_GetItem(fast, 1);

		যদি (ConvertFromPyObject(valObject, &di->di_tv) == -1)
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    dictitem_free(di);
		    প্রত্যাগম রিক্ত;
		}

		Py_DECREF(fast);

		hi = hash_find(&dict->dv_hashtab, di->di_চাবি);
		যদি (!HASHITEM_EMPTY(hi) || dict_add(dict, di) == FAIL)
		{
		    RAISE_চাবি_ADD_FAIL(di->di_চাবি);
		    Py_DECREF(iterator);
		    dictitem_free(di);
		    প্রত্যাগম রিক্ত;
		}
	    }

	    Py_DECREF(iterator);

	    // Iterator may have finished due to an exception
	    যদি (PyErr_Occurred())
		প্রত্যাগম রিক্ত;
	}
    }
    Py_INCREF(Py_None);
    প্রত্যাগম Py_None;
}

    স্থির PyObject *
DictionaryGet(DictionaryObject *self, PyObject *args)
{
    প্রত্যাগম _DictionaryItem(self, args,
			    DICT_FLAG_HAS_DEFAULT|DICT_FLAG_NONE_DEFAULT);
}

    স্থির PyObject *
Dictionary_বের_করো(DictionaryObject *self, PyObject *args)
{
    প্রত্যাগম _DictionaryItem(self, args, DICT_FLAG_HAS_DEFAULT|DICT_FLAG_বের_করো);
}

    স্থির PyObject *
Dictionary_বের_করো_Item(DictionaryObject *self, PyObject *args UNUSED)
{
    hashitem_T	*hi;
    PyObject	*ret;
    PyObject	*valObject;
    dictitem_T	*di;

    যদি (self->dict->dv_hashtab.ht_used == 0)
    {
	PyErr_SetNone(Py_Exc_চাবি_Error);
	প্রত্যাগম রিক্ত;
    }

    hi = self->dict->dv_hashtab.ht_array;
    যাবত্ (HASHITEM_EMPTY(hi))
	++hi;

    di = dict_lookup(hi);

    যদি (!(valObject = ConvertToPyObject(&di->di_tv)))
	প্রত্যাগম রিক্ত;

    যদি (!(ret = Py_Build_মান("(" Py_bytes_fmt "O)", hi->hi_চাবি, valObject)))
    {
	Py_DECREF(valObject);
	প্রত্যাগম রিক্ত;
    }

    hash_remove(&self->dict->dv_hashtab, hi, "Python pop item");
    dictitem_free(di);

    প্রত্যাগম ret;
}

    স্থির PyObject *
Dictionary_Has_চাবি(DictionaryObject *self, PyObject *চাবি_Object)
{
    প্রত্যাগম _DictionaryItem(self, চাবি_Object, DICT_FLAG_RETURN_BOOL);
}

স্থির PySequenceMethods DictionaryAsSeq = {
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

স্থির PyMappingMethods DictionaryAsMapping = {
    (lenfunc)       DictionaryLength,
    (binaryfunc)    DictionaryItem,
    (objobjargproc) DictionaryAssItem,
};

স্থির সংরচনা PyMethodDef DictionaryMethods[] = {
    {"keys",	(PyCFunction)Dictionary_List_চাবিগুলি,	METH_NOARGS,	""},
    {"values",	(PyCFunction)Dictionary_List_মানসমূহ,	METH_NOARGS,	""},
    {"items",	(PyCFunction)Dictionary_List_বস্তুসমূহ,	METH_NOARGS,	""},
    {"update",	(PyCFunction)(শূন্য *)DictionaryUpdate,		METH_VARARGS|METH_KEYWORDS, ""},
    {"get",	(PyCFunction)DictionaryGet,		METH_VARARGS,	""},
    {"pop",	(PyCFunction)Dictionary_বের_করো,		METH_VARARGS,	""},
    {"popitem",	(PyCFunction)Dictionary_বের_করো_Item,		METH_NOARGS,	""},
    {"has_key",	(PyCFunction)Dictionary_Has_চাবি,		METH_O,		""},
    {"__dir__",	(PyCFunction)DictionaryDir,		METH_NOARGS,	""},
    { রিক্ত,	রিক্ত,					0,		রিক্ত}
};

DEFINE_PY_TYPE_OBJECT(ListType);

প্রকারনির্দেশ সংরচনা
{
    PyObject_HEAD
    list_T	*list;
    pylinkedlist_T	ref;
} ListObject;

#define NEW_LIST(list) ListNew(ListTypePtr, list)

    স্থির PyObject *
ListNew(PyTypeObject *subtype, list_T *list)
{
    ListObject	*self;

    যদি (list == রিক্ত)
	প্রত্যাগম রিক্ত;

    self = (ListObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    যদি (self == রিক্ত)
	প্রত্যাগম রিক্ত;
    self->list = list;
    ++list->lv_refcount;
    CHECK_LIST_MATERIALIZE(list);

    pyll_add((PyObject *)(self), &self->ref, &lastlist);

    প্রত্যাগম (PyObject *)(self);
}

    স্থির list_T *
py_list_alloc(শূন্য)
{
    list_T	*ret;

    যদি (!(ret = list_alloc()))
    {
	PyErr_NoMemory();
	প্রত্যাগম রিক্ত;
    }
    ++ret->lv_refcount;

    প্রত্যাগম ret;
}

    স্থির পূর্ণাঙ্ক
list_py_concat(list_T *l, PyObject *obj, PyObject *lookup_dict)
{
    PyObject	*iterator;
    PyObject	*item;
    listitem_T	*li;

    যদি (!(iterator = PyObject_GetIter(obj)))
	প্রত্যাগম -1;

    যাবত্ ((item = PyIter_Next(iterator)))
    {
	যদি (!(li = listitem_alloc()))
	{
	    PyErr_NoMemory();
	    Py_DECREF(item);
	    Py_DECREF(iterator);
	    প্রত্যাগম -1;
	}
	li->li_tv.v_lock = 0;
	li->li_tv.v_type = VAR_UNKNOWN;

	যদি (_ConvertFromPyObject(item, &li->li_tv, lookup_dict) == -1)
	{
	    Py_DECREF(item);
	    Py_DECREF(iterator);
	    listitem_free(l, li);
	    প্রত্যাগম -1;
	}

	Py_DECREF(item);

	list_append(l, li);
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    যদি (PyErr_Occurred())
	প্রত্যাগম -1;

    প্রত্যাগম 0;
}

    স্থির PyObject *
ListConstructor(PyTypeObject *subtype, PyObject *args, PyObject *kwargs)
{
    list_T	*list;
    PyObject	*obj = রিক্ত;

    যদি (kwargs)
    {
	PyErr_SET_STRING(PyExc_TypeError,
		N_("list constructor does not accept keyword arguments"));
	প্রত্যাগম রিক্ত;
    }

    যদি (!PyArg_ParseTuple(args, "|O", &obj))
	প্রত্যাগম রিক্ত;

    যদি (!(list = py_list_alloc()))
	প্রত্যাগম রিক্ত;

    যদি (obj)
    {
	PyObject	*lookup_dict;

	যদি (!(lookup_dict = PyDict_New()))
	{
	    list_unref(list);
	    প্রত্যাগম রিক্ত;
	}

	যদি (list_py_concat(list, obj, lookup_dict) == -1)
	{
	    Py_DECREF(lookup_dict);
	    list_unref(list);
	    প্রত্যাগম রিক্ত;
	}

	Py_DECREF(lookup_dict);
    }

    প্রত্যাগম ListNew(subtype, list);
}

    স্থির শূন্য
ListDestructor(PyObject *self_obj)
{
    ListObject *self = (ListObject*)self_obj;
    pyll_remove(&self->ref, &lastlist);
    list_unref(self->list);

    DESTRUCTOR_FINISH(self);
}

    স্থির PyInt
ListLength(ListObject *self)
{
    প্রত্যাগম ((PyInt) (self->list->lv_len));
}

    স্থির PyObject *
ListIndex(ListObject *self, Py_ssize_t index)
{
    listitem_T	*li;

    যদি (index >= ListLength(self))
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("list index out of range"));
	প্রত্যাগম রিক্ত;
    }
    li = list_find(self->list, (long) index);
    যদি (li == রিক্ত)
    {
	// No more suitable format specifications in python-2.3
	PyErr_VIM_FORMAT(N_("internal error: failed to get Vim list item %d"),
		(পূর্ণাঙ্ক) index);
	প্রত্যাগম রিক্ত;
    }
    প্রত্যাগম ConvertToPyObject(&li->li_tv);
}

    স্থির PyObject *
ListSlice(ListObject *self, Py_ssize_t first, Py_ssize_t step,
	  Py_ssize_t slicelen)
{
    PyInt	i;
    PyObject	*list;

    যদি (step == 0)
    {
	PyErr_SET_STRING(Py_Exc_মান_Error, N_("slice step cannot be zero"));
	প্রত্যাগম রিক্ত;
    }

    list = PyList_New(slicelen);
    যদি (list == রিক্ত)
	প্রত্যাগম রিক্ত;

    প্রতি (i = 0; i < slicelen; ++i)
    {
	PyObject	*item;

	item = ListIndex(self, first + i*step);
	যদি (item == রিক্ত)
	{
	    Py_DECREF(list);
	    প্রত্যাগম রিক্ত;
	}

	PyList_SET_ITEM(list, i, item);
    }

    প্রত্যাগম list;
}

    স্থির PyObject *
ListItem(ListObject *self, PyObject* idx)
{
#যদি PY_MAJOR_VERSION < 3
    যদি (PyInt_Check(idx))
    {
	long _idx = PyInt_AsLong(idx);
	প্রত্যাগম ListIndex(self, _idx);
    }
    অন্যথা
#endif
    যদি (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	প্রত্যাগম ListIndex(self, _idx);
    }
    অন্যথা যদি (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	যদি (PySlice_GetIndicesEx((PySliceObject_T *)idx, ListLength(self),
				 &start, &stop, &step, &slicelen) < 0)
	    প্রত্যাগম রিক্ত;
	প্রত্যাগম ListSlice(self, start, step, slicelen);
    }
    অন্যথা
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	প্রত্যাগম রিক্ত;
    }
}

    স্থির শূন্য
list_restore(Py_ssize_t numadded, Py_ssize_t numreplaced, Py_ssize_t slicelen,
	list_T *l, listitem_T **lis, listitem_T *lastaddedli)
{
    যাবত্ (numreplaced--)
    {
	list_ঢোকাও(l, lis[numreplaced], lis[slicelen + numreplaced]);
	listitem_remove(l, lis[slicelen + numreplaced]);
    }
    যাবত্ (numadded--)
    {
	listitem_T	*next;

	next = lastaddedli->li_prev;
	listitem_remove(l, lastaddedli);
	lastaddedli = next;
    }
}

    স্থির পূর্ণাঙ্ক
ListAssSlice(ListObject *self, Py_ssize_t first,
	     Py_ssize_t step, Py_ssize_t slicelen, PyObject *obj)
{
    PyObject	*iterator;
    PyObject	*item;
    listitem_T	*li;
    listitem_T	*lastaddedli = রিক্ত;
    listitem_T	*next;
    typval_T	v;
    list_T	*l = self->list;
    PyInt	i;
    PyInt	j;
    PyInt	numreplaced = 0;
    PyInt	numadded = 0;
    PyInt	size;
    listitem_T	**lis = রিক্ত;

    size = ListLength(self);

    যদি (l->lv_lock)
    {
	RAISE_LOCKED_LIST;
	প্রত্যাগম -1;
    }

    যদি (step == 0)
    {
	PyErr_SET_STRING(Py_Exc_মান_Error, N_("slice step cannot be zero"));
	প্রত্যাগম -1;
    }

    যদি (step != 1 && slicelen == 0)
    {
	// Nothing to do. Only error out if obj has some items.
	পূর্ণাঙ্ক		ret = 0;

	যদি (obj == রিক্ত)
	    প্রত্যাগম 0;

	যদি (!(iterator = PyObject_GetIter(obj)))
	    প্রত্যাগম -1;

	যদি ((item = PyIter_Next(iterator)))
	{
	    PyErr_FORMAT(Py_Exc_মান_Error,
		    N_("attempt to assign sequence of size greater than %d "
			"to extended slice"), 0);
	    Py_DECREF(item);
	    ret = -1;
	}
	Py_DECREF(iterator);
	প্রত্যাগম ret;
    }

    যদি (obj != রিক্ত)
	// XXX May allocate zero bytes.
	যদি (!(lis = PyMem_New(listitem_T *, slicelen * 2)))
	{
	    PyErr_NoMemory();
	    প্রত্যাগম -1;
	}

    যদি (first == size)
	li = রিক্ত;
    অন্যথা
    {
	li = list_find(l, (long) first);
	যদি (li == রিক্ত)
	{
	    PyErr_VIM_FORMAT(N_("internal error: no Vim list item %d"),
		    (পূর্ণাঙ্ক)first);
	    যদি (obj != রিক্ত)
		PyMem_Free(lis);
	    প্রত্যাগম -1;
	}
	i = slicelen;
	যাবত্ (i-- && li != রিক্ত)
	{
	    j = step;
	    next = li;
	    যদি (step > 0)
		যাবত্ (next != রিক্ত && ((next = next->li_next) != রিক্ত) && --j);
	    অন্যথা
		যাবত্ (next != রিক্ত && ((next = next->li_prev) != রিক্ত) && ++j);

	    যদি (obj == রিক্ত)
		listitem_remove(l, li);
	    অন্যথা
		lis[slicelen - i - 1] = li;

	    li = next;
	}
	যদি (li == রিক্ত && i != -1)
	{
	    PyErr_SET_VIM(N_("internal error: not enough list items"));
	    যদি (obj != রিক্ত)
		PyMem_Free(lis);
	    প্রত্যাগম -1;
	}
    }

    যদি (obj == রিক্ত)
	প্রত্যাগম 0;

    যদি (!(iterator = PyObject_GetIter(obj)))
    {
	PyMem_Free(lis);
	প্রত্যাগম -1;
    }

    i = 0;
    যাবত্ ((item = PyIter_Next(iterator)))
    {
	যদি (ConvertFromPyObject(item, &v) == -1)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(item);
	    PyMem_Free(lis);
	    প্রত্যাগম -1;
	}
	Py_DECREF(item);
	যদি (list_ঢোকাও_tv(l, &v, numreplaced < slicelen
				    ? lis[numreplaced]
				    : li) == FAIL)
	{
	    clear_tv(&v);
	    PyErr_SET_VIM(N_("internal error: failed to add item to list"));
	    list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	    PyMem_Free(lis);
	    প্রত্যাগম -1;
	}
	যদি (numreplaced < slicelen)
	{
	    lis[slicelen + numreplaced] = lis[numreplaced]->li_prev;
	    vimlist_remove(l, lis[numreplaced], lis[numreplaced]);
	    numreplaced++;
	}
	অন্যথা
	{
	    যদি (li)
		lastaddedli = li->li_prev;
	    অন্যথা
		lastaddedli = l->lv_u.mat.lv_last;
	    numadded++;
	}
	clear_tv(&v);
	যদি (step != 1 && i >= slicelen)
	{
	    Py_DECREF(iterator);
	    PyErr_FORMAT(Py_Exc_মান_Error,
		    N_("attempt to assign sequence of size greater than %d "
			"to extended slice"), (পূর্ণাঙ্ক) slicelen);
	    list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	    PyMem_Free(lis);
	    প্রত্যাগম -1;
	}
	++i;
    }
    Py_DECREF(iterator);

    যদি (step != 1 && i != slicelen)
    {
	PyErr_FORMAT2(Py_Exc_মান_Error,
		N_("attempt to assign sequence of size %d to extended slice "
		    "of size %d"), (পূর্ণাঙ্ক) i, (পূর্ণাঙ্ক) slicelen);
	list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	PyMem_Free(lis);
	প্রত্যাগম -1;
    }

    যদি (PyErr_Occurred())
    {
	list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	PyMem_Free(lis);
	প্রত্যাগম -1;
    }

    প্রতি (i = 0; i < numreplaced; i++)
	listitem_free(l, lis[i]);
    যদি (step == 1)
	প্রতি (i = numreplaced; i < slicelen; i++)
	    listitem_remove(l, lis[i]);

    PyMem_Free(lis);

    প্রত্যাগম 0;
}

    স্থির পূর্ণাঙ্ক
ListAssIndex(ListObject *self, Py_ssize_t index, PyObject *obj)
{
    typval_T	tv;
    list_T	*l = self->list;
    listitem_T	*li;
    Py_ssize_t	length = ListLength(self);

    যদি (l->lv_lock)
    {
	RAISE_LOCKED_LIST;
	প্রত্যাগম -1;
    }
    যদি (index > length || (index == length && obj == রিক্ত))
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("list index out of range"));
	প্রত্যাগম -1;
    }

    যদি (obj == রিক্ত)
    {
	li = list_find(l, (long) index);
	যদি (li == রিক্ত)
	{
	    PyErr_VIM_FORMAT(N_("internal error: failed to get Vim "
			"list item %d"), (পূর্ণাঙ্ক) index);
	    প্রত্যাগম -1;
	}
	vimlist_remove(l, li, li);
	clear_tv(&li->li_tv);
	vim_free(li);
	প্রত্যাগম 0;
    }

    যদি (ConvertFromPyObject(obj, &tv) == -1)
	প্রত্যাগম -1;

    যদি (index == length)
    {
	যদি (list_append_tv(l, &tv) == FAIL)
	{
	    clear_tv(&tv);
	    PyErr_SET_VIM(N_("failed to add item to list"));
	    প্রত্যাগম -1;
	}
    }
    অন্যথা
    {
	li = list_find(l, (long) index);
	যদি (li == রিক্ত)
	{
	    PyErr_VIM_FORMAT(N_("internal error: failed to get Vim "
			"list item %d"), (পূর্ণাঙ্ক) index);
	    প্রত্যাগম -1;
	}
	clear_tv(&li->li_tv);
	copy_tv(&tv, &li->li_tv);
	clear_tv(&tv);
    }
    প্রত্যাগম 0;
}

    স্থির পূর্ণাঙ্ক
ListAssItem(ListObject *self, PyObject *idx, PyObject *obj)
{
#যদি PY_MAJOR_VERSION < 3
    যদি (PyInt_Check(idx))
    {
	long _idx = PyInt_AsLong(idx);
	প্রত্যাগম (পূর্ণাঙ্ক)ListAssIndex(self, _idx, obj);
    }
    অন্যথা
#endif
    যদি (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	প্রত্যাগম (পূর্ণাঙ্ক)ListAssIndex(self, _idx, obj);
    }
    অন্যথা যদি (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	যদি (PySlice_GetIndicesEx((PySliceObject_T *)idx, ListLength(self),
				 &start, &stop, &step, &slicelen) < 0)
	    প্রত্যাগম -1;
	প্রত্যাগম (পূর্ণাঙ্ক)ListAssSlice(self, start, step, slicelen,
		obj);
    }
    অন্যথা
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	প্রত্যাগম -1;
    }
}

    স্থির PyObject *
ListConcatInPlace(ListObject *self, PyObject *obj)
{
    list_T	*l = self->list;
    PyObject	*lookup_dict;

    যদি (l->lv_lock)
    {
	RAISE_LOCKED_LIST;
	প্রত্যাগম রিক্ত;
    }

    যদি (!(lookup_dict = PyDict_New()))
	প্রত্যাগম রিক্ত;

    যদি (list_py_concat(l, obj, lookup_dict) == -1)
    {
	Py_DECREF(lookup_dict);
	প্রত্যাগম রিক্ত;
    }
    Py_DECREF(lookup_dict);

    Py_INCREF((PyObject *)self);
    প্রত্যাগম (PyObject *)(self);
}

প্রকারনির্দেশ সংরচনা
{
    listwatch_T	lw;
    list_T	*list;
} listiterinfo_T;

    স্থির শূন্য
ListIterDestruct(শূন্য *arg)
{
    listiterinfo_T *lii = (listiterinfo_T*)arg;
    list_rem_watch(lii->list, &lii->lw);
    list_unref(lii->list);
    PyMem_Free(lii);
}

    স্থির PyObject *
ListIterNext(শূন্য **arg)
{
    PyObject	*ret;
    listiterinfo_T **lii = (listiterinfo_T**)arg;

    যদি (!((*lii)->lw.lw_item))
	প্রত্যাগম রিক্ত;

    যদি (!(ret = ConvertToPyObject(&((*lii)->lw.lw_item->li_tv))))
	প্রত্যাগম রিক্ত;

    (*lii)->lw.lw_item = (*lii)->lw.lw_item->li_next;

    প্রত্যাগম ret;
}

    স্থির PyObject *
ListIter(PyObject *self_obj)
{
    ListObject	*self = (ListObject*)self_obj;
    listiterinfo_T	*lii;
    list_T	*l = self->list;

    যদি (!(lii = PyMem_New(listiterinfo_T, 1)))
    {
	PyErr_NoMemory();
	প্রত্যাগম রিক্ত;
    }

    CHECK_LIST_MATERIALIZE(l);
    list_add_watch(l, &lii->lw);
    lii->lw.lw_item = l->lv_first;
    lii->list = l;
    ++l->lv_refcount;

    প্রত্যাগম IterNew(lii,
	    ListIterDestruct, ListIterNext,
	    রিক্ত, রিক্ত, (PyObject *)self);
}

স্থির বর্ণ *ListAttrs[] = {
    "locked",
    রিক্ত
};

    স্থির PyObject *
ListDir(PyObject *self, PyObject *args UNUSED)
{
    প্রত্যাগম ObjectDir(self, ListAttrs);
}

    স্থির পূর্ণাঙ্ক
ListSetattr(PyObject *self_obj, বর্ণ *name, PyObject *valObject)
{
    ListObject *self = (ListObject*)self_obj;
    যদি (valObject == রিক্ত)
    {
	PyErr_SET_STRING(PyExc_AttributeError,
		N_("cannot delete vim.List attributes"));
	প্রত্যাগম -1;
    }

    যদি (strcmp(name, "locked") == 0)
    {
	যদি (self->list->lv_lock == VAR_FIXED)
	{
	    PyErr_SET_STRING(PyExc_TypeError, N_("cannot modify fixed list"));
	    প্রত্যাগম -1;
	}
	অন্যথা
	{
	    পূর্ণাঙ্ক		istrue = PyObject_IsTrue(valObject);
	    যদি (istrue == -1)
		প্রত্যাগম -1;
	    অন্যথা যদি (istrue)
		self->list->lv_lock = VAR_LOCKED;
	    অন্যথা
		self->list->lv_lock = 0;
	}
	প্রত্যাগম 0;
    }
    অন্যথা
    {
	PyErr_FORMAT(PyExc_AttributeError, N_("cannot set attribute %s"), name);
	প্রত্যাগম -1;
    }
}

স্থির PySequenceMethods ListAsSeq = {
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

স্থির PyMappingMethods ListAsMapping = {
    /* mp_length	*/ (lenfunc) ListLength,
    /* mp_subscript     */ (binaryfunc) ListItem,
    /* mp_ass_subscript */ (objobjargproc) ListAssItem,
};

স্থির সংরচনা PyMethodDef ListMethods[] = {
    {"extend",	(PyCFunction)ListConcatInPlace,	METH_O,		""},
    {"__dir__",	(PyCFunction)ListDir,		METH_NOARGS,	""},
    { রিক্ত,	রিক্ত,				0,		রিক্ত}
};

DEFINE_PY_TYPE_OBJECT(TupleType);

প্রকারনির্দেশ সংরচনা
{
    PyObject_HEAD
    tuple_T	*tuple;
    pylinkedlist_T	ref;
} TupleObject;

#define NEW_TUPLE(tuple) TupleNew(TupleTypePtr, tuple)

    স্থির PyObject *
TupleNew(PyTypeObject *subtype, tuple_T *tuple)
{
    TupleObject	*self;

    যদি (tuple == রিক্ত)
	প্রত্যাগম রিক্ত;

    self = (TupleObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    যদি (self == রিক্ত)
	প্রত্যাগম রিক্ত;
    self->tuple = tuple;
    ++tuple->tv_refcount;

    pyll_add((PyObject *)(self), &self->ref, &lasttuple);

    প্রত্যাগম (PyObject *)(self);
}

    স্থির tuple_T *
py_tuple_alloc(শূন্য)
{
    tuple_T	*ret;

    যদি (!(ret = tuple_alloc()))
    {
	PyErr_NoMemory();
	প্রত্যাগম রিক্ত;
    }
    ++ret->tv_refcount;

    প্রত্যাগম ret;
}

    স্থির পূর্ণাঙ্ক
tuple_py_concat(tuple_T *t, PyObject *obj, PyObject *lookup_dict)
{
    PyObject	*iterator;
    PyObject	*item;

    যদি (!(iterator = PyObject_GetIter(obj)))
	প্রত্যাগম -1;

    যাবত্ ((item = PyIter_Next(iterator)))
    {
	typval_T    new_tv;

	যদি (_ConvertFromPyObject(item, &new_tv, lookup_dict) == -1)
	{
	    Py_DECREF(item);
	    Py_DECREF(iterator);
	    প্রত্যাগম -1;
	}

	Py_DECREF(item);

	যদি (tuple_append_tv(t, &new_tv) == FAIL)
	{
	    Py_DECREF(iterator);
	    প্রত্যাগম -1;
	}
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    যদি (PyErr_Occurred())
	প্রত্যাগম -1;

    প্রত্যাগম 0;
}

    স্থির PyObject *
TupleConstructor(PyTypeObject *subtype, PyObject *args, PyObject *kwargs)
{
    tuple_T	*tuple;
    PyObject	*obj = রিক্ত;

    যদি (kwargs)
    {
	PyErr_SET_STRING(PyExc_TypeError,
		N_("tuple constructor does not accept keyword arguments"));
	প্রত্যাগম রিক্ত;
    }

    যদি (!PyArg_ParseTuple(args, "|O", &obj))
	প্রত্যাগম রিক্ত;

    যদি (!(tuple = py_tuple_alloc()))
	প্রত্যাগম রিক্ত;

    যদি (obj)
    {
	PyObject	*lookup_dict;

	যদি (!(lookup_dict = PyDict_New()))
	{
	    tuple_unref(tuple);
	    প্রত্যাগম রিক্ত;
	}

	যদি (tuple_py_concat(tuple, obj, lookup_dict) == -1)
	{
	    Py_DECREF(lookup_dict);
	    tuple_unref(tuple);
	    প্রত্যাগম রিক্ত;
	}

	Py_DECREF(lookup_dict);
    }

    প্রত্যাগম TupleNew(subtype, tuple);
}

    স্থির শূন্য
TupleDestructor(PyObject *self_obj)
{
    TupleObject *self = (TupleObject *)self_obj;
    pyll_remove(&self->ref, &lasttuple);
    tuple_unref(self->tuple);

    DESTRUCTOR_FINISH(self);
}

    স্থির PyInt
TupleLength(TupleObject *self)
{
    প্রত্যাগম ((PyInt)(tuple_len(self->tuple)));
}

    স্থির PyObject *
TupleIndex(TupleObject *self, Py_ssize_t index)
{
    PyInt	len = TupleLength(self);

    যদি (index < 0)
	index = len + index;

    যদি (index < 0 || index >= len)
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("tuple index out of range"));
	প্রত্যাগম রিক্ত;
    }
    প্রত্যাগম ConvertToPyObject(TUPLE_ITEM(self->tuple, index));
}

/*
 * Return a new tuple object for the tuple slice starting from the index
 * "first" and of length "slicelen" skipping "step" items.
 */
    স্থির PyObject *
TupleSlice(
    TupleObject		*self,
    Py_ssize_t		first,
    Py_ssize_t		step,
    Py_ssize_t		slicelen)
{
    PyInt	i;
    PyObject	*tuple;

    tuple = PyTuple_New(slicelen);
    যদি (tuple == রিক্ত)
	প্রত্যাগম রিক্ত;

    প্রতি (i = 0; i < slicelen; ++i)
    {
	PyObject	*item;

	item = TupleIndex(self, first + i * step);
	যদি (item == রিক্ত)
	{
	    Py_DECREF(tuple);
	    প্রত্যাগম রিক্ত;
	}

	PyTuple_SetItem(tuple, i, item);
    }

    প্রত্যাগম tuple;
}

    স্থির PyObject *
TupleItem(TupleObject *self, PyObject* idx)
{
#যদি PY_MAJOR_VERSION < 3
    যদি (PyInt_Check(idx))
    {
	long _idx = PyInt_AsLong(idx);
	প্রত্যাগম TupleIndex(self, _idx);
    }
    অন্যথা
#endif
    যদি (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	প্রত্যাগম TupleIndex(self, _idx);
    }
    অন্যথা যদি (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	যদি (PySlice_GetIndicesEx((PySliceObject_T *)idx, TupleLength(self),
				 &start, &stop, &step, &slicelen) < 0)
	    প্রত্যাগম রিক্ত;
	প্রত্যাগম TupleSlice(self, start, step, slicelen);
    }
    অন্যথা
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	প্রত্যাগম রিক্ত;
    }
}

প্রকারনির্দেশ সংরচনা
{
    tuple_T	*tuple;
    পূর্ণাঙ্ক		index;
} tupleiterinfo_T;

    স্থির শূন্য
TupleIterDestruct(শূন্য *arg)
{
    tupleiterinfo_T *tii = (tupleiterinfo_T*)arg;
    tuple_unref(tii->tuple);
    PyMem_Free(tii);
}

    স্থির PyObject *
TupleIterNext(শূন্য **arg)
{
    PyObject	*ret;
    tupleiterinfo_T **tii = (tupleiterinfo_T**)arg;

    যদি ((*tii)->index >= TUPLE_LEN((*tii)->tuple))
	প্রত্যাগম রিক্ত;

    যদি (!(ret = ConvertToPyObject(TUPLE_ITEM((*tii)->tuple, (*tii)->index))))
	প্রত্যাগম রিক্ত;

    (*tii)->index++;

    প্রত্যাগম ret;
}

    স্থির PyObject *
TupleIter(PyObject *self_obj)
{
    TupleObject	*self = (TupleObject*)self_obj;
    tupleiterinfo_T	*tii;
    tuple_T	*t = self->tuple;

    যদি (!(tii = PyMem_New(tupleiterinfo_T, 1)))
    {
	PyErr_NoMemory();
	প্রত্যাগম রিক্ত;
    }

    tii->tuple = t;
    tii->index = 0;
    ++t->tv_refcount;

    প্রত্যাগম IterNew(tii,
	    TupleIterDestruct, TupleIterNext,
	    রিক্ত, রিক্ত, (PyObject *)self);
}

স্থির বর্ণ *TupleAttrs[] = {
    "locked",
    রিক্ত
};

    স্থির PyObject *
TupleDir(PyObject *self, PyObject *args UNUSED)
{
    প্রত্যাগম ObjectDir(self, TupleAttrs);
}

    স্থির পূর্ণাঙ্ক
TupleSetattr(PyObject *self_obj, বর্ণ *name, PyObject *valObject)
{
    TupleObject *self = (TupleObject*)self_obj;
    যদি (valObject == রিক্ত)
    {
	PyErr_SET_STRING(PyExc_AttributeError,
		N_("cannot delete vim.Tuple attributes"));
	প্রত্যাগম -1;
    }

    যদি (strcmp(name, "locked") == 0)
    {
	যদি (self->tuple->tv_lock == VAR_FIXED)
	{
	    PyErr_SET_STRING(PyExc_TypeError, N_("cannot modify fixed tuple"));
	    প্রত্যাগম -1;
	}
	অন্যথা
	{
	    পূর্ণাঙ্ক		istrue = PyObject_IsTrue(valObject);
	    যদি (istrue == -1)
		প্রত্যাগম -1;
	    অন্যথা যদি (istrue)
		self->tuple->tv_lock = VAR_LOCKED;
	    অন্যথা
		self->tuple->tv_lock = 0;
	}
	প্রত্যাগম 0;
    }
    অন্যথা
    {
	PyErr_FORMAT(PyExc_AttributeError, N_("cannot set attribute %s"), name);
	প্রত্যাগম -1;
    }
}

স্থির PySequenceMethods TupleAsSeq = {
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

স্থির PyMappingMethods TupleAsMapping = {
    /* mp_length	*/ (lenfunc) TupleLength,
    /* mp_subscript     */ (binaryfunc) TupleItem,
    /* mp_ass_subscript */ (objobjargproc) 0,
};

স্থির সংরচনা PyMethodDef TupleMethods[] = {
    {"__dir__",	(PyCFunction)TupleDir,		METH_NOARGS,	""},
    { রিক্ত,	রিক্ত,				0,		রিক্ত}
};

প্রকারনির্দেশ সংরচনা
{
    PyObject_HEAD
    char_u	*name;
    পূর্ণাঙ্ক		argc;
    typval_T	*argv;
    dict_T	*self;
    pylinkedlist_T	ref;
    পূর্ণাঙ্ক		auto_rebind;
} FunctionObject;

DEFINE_PY_TYPE_OBJECT(FunctionType);

#define NEW_FUNCTION(name, argc, argv, self, pt_auto) \
    FunctionNew(FunctionTypePtr, (name), (argc), (argv), (self), (pt_auto))

    স্থির PyObject *
FunctionNew(PyTypeObject *subtype, char_u *name, পূর্ণাঙ্ক argc, typval_T *argv,
	dict_T *selfdict, পূর্ণাঙ্ক auto_rebind)
{
    FunctionObject	*self;

    self = (FunctionObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    যদি (self == রিক্ত)
	প্রত্যাগম রিক্ত;

    যদি (isdigit((unsigned বর্ণ)*name))
    {
	যদি (!translated_function_exists(name, FALSE))
	{
	    PyErr_FORMAT(Py_Exc_মান_Error,
		    N_("unnamed function %s does not exist"), name);
	    প্রত্যাগম রিক্ত;
	}
	self->name = vim_strsave(name);
    }
    অন্যথা
    {
	char_u *p;

	যদি ((p = get_expanded_name(name,
			    vim_strchr(name, AUTOLOAD_CHAR) == রিক্ত)) == রিক্ত)
	{
	    PyErr_FORMAT(Py_Exc_মান_Error,
		    N_("function %s does not exist"), name);
	    প্রত্যাগম রিক্ত;
	}

	যদি (p[0] == K_SPECIAL && p[1] == KS_EXTRA && p[2] == (পূর্ণাঙ্ক)KE_SNR)
	{
	    char_u *np;
	    size_t len = STRLEN(p) + 1;

	    যদি ((np = alloc(len + 2)) == রিক্ত)
	    {
		vim_free(p);
		প্রত্যাগম রিক্ত;
	    }
	    mch_memmove(np, "<SNR>", 5);
	    mch_memmove(np + 5, p + 3, len - 3);
	    vim_free(p);
	    self->name = np;
	}
	অন্যথা
	    self->name = p;
    }

    func_ref(self->name);
    self->argc = argc;
    self->argv = argv;
    self->self = selfdict;
    self->auto_rebind = selfdict == রিক্ত ? TRUE : auto_rebind;

    যদি (self->argv || self->self)
	pyll_add((PyObject *)(self), &self->ref, &lastfunc);

    প্রত্যাগম (PyObject *)(self);
}

    স্থির PyObject *
FunctionConstructor(PyTypeObject *subtype, PyObject *args, PyObject *kwargs)
{
    PyObject	*self;
    PyObject	*selfdictObject;
    PyObject	*autoRebindObject;
    PyObject	*argsObject = রিক্ত;
    char_u	*name;
    typval_T	selfdicttv;
    typval_T	argstv;
    list_T	*argslist = রিক্ত;
    dict_T	*selfdict = রিক্ত;
    পূর্ণাঙ্ক		argc = 0;
    পূর্ণাঙ্ক		auto_rebind = TRUE;
    typval_T	*argv = রিক্ত;
    typval_T	*curtv;
    listitem_T	*li;

    যদি (kwargs != রিক্ত)
    {
	selfdictObject = PyDict_GetItemString(kwargs, "self");
	যদি (selfdictObject != রিক্ত)
	{
	    যদি (ConvertFromPyMapping(selfdictObject, &selfdicttv) == -1)
		প্রত্যাগম রিক্ত;
	    selfdict = selfdicttv.vval.v_dict;
	}
	argsObject = PyDict_GetItemString(kwargs, "args");
	যদি (argsObject != রিক্ত)
	{
	    যদি (ConvertFromPySequence(argsObject, &argstv) == -1)
	    {
		dict_unref(selfdict);
		প্রত্যাগম রিক্ত;
	    }
	    argslist = argstv.vval.v_list;
	    CHECK_LIST_MATERIALIZE(argslist);

	    argc = argslist->lv_len;
	    যদি (argc != 0)
	    {
		argv = PyMem_New(typval_T, (size_t) argc);
		যদি (argv == রিক্ত)
		{
		    PyErr_NoMemory();
		    dict_unref(selfdict);
		    list_unref(argslist);
		    প্রত্যাগম রিক্ত;
		}
		curtv = argv;
		FOR_ALL_LIST_বস্তুসমূহ(argslist, li)
		    copy_tv(&li->li_tv, curtv++);
	    }
	    list_unref(argslist);
	}
	যদি (selfdict != রিক্ত)
	{
	    auto_rebind = FALSE;
	    autoRebindObject = PyDict_GetItemString(kwargs, "auto_rebind");
	    যদি (autoRebindObject != রিক্ত)
	    {
		auto_rebind = PyObject_IsTrue(autoRebindObject);
		যদি (auto_rebind == -1)
		{
		    dict_unref(selfdict);
		    list_unref(argslist);
		    প্রত্যাগম রিক্ত;
		}
	    }
	}
    }

    যদি (!PyArg_ParseTuple(args, "et", "ascii", &name))
    {
	dict_unref(selfdict);
	যাবত্ (argc--)
	    clear_tv(&argv[argc]);
	PyMem_Free(argv);
	প্রত্যাগম রিক্ত;
    }

    self = FunctionNew(subtype, name, argc, argv, selfdict, auto_rebind);

    PyMem_Free(name);

    প্রত্যাগম self;
}

    স্থির শূন্য
FunctionDestructor(PyObject *self_obj)
{
    FunctionObject *self = (FunctionObject*)self_obj;
    পূর্ণাঙ্ক i;
    func_unref(self->name);
    vim_free(self->name);
    প্রতি (i = 0; i < self->argc; ++i)
	clear_tv(&self->argv[i]);
    PyMem_Free(self->argv);
    dict_unref(self->self);
    যদি (self->argv || self->self)
	pyll_remove(&self->ref, &lastfunc);

    DESTRUCTOR_FINISH(self);
}

স্থির বর্ণ *FunctionAttrs[] = {
    "softspace", "args", "self", "auto_rebind",
    রিক্ত
};

    স্থির PyObject *
FunctionDir(PyObject *self, PyObject *args UNUSED)
{
    প্রত্যাগম ObjectDir(self, FunctionAttrs);
}

    স্থির PyObject *
FunctionAttr(FunctionObject *self, বর্ণ *name)
{
    list_T *list;
    পূর্ণাঙ্ক i;
    যদি (strcmp(name, "name") == 0)
	প্রত্যাগম PyString_FromString((বর্ণ *)(self->name));
    অন্যথা যদি (strcmp(name, "args") == 0)
    {
	যদি (self->argv == রিক্ত || (list = list_alloc()) == রিক্ত)
	    প্রত্যাগম ALWAYS_NONE;

	প্রতি (i = 0; i < self->argc; ++i)
	    list_append_tv(list, &self->argv[i]);
	প্রত্যাগম NEW_LIST(list);
    }
    অন্যথা যদি (strcmp(name, "self") == 0)
	প্রত্যাগম self->self == রিক্ত
	    ? ALWAYS_NONE
	    : NEW_DICTIONARY(self->self);
    অন্যথা যদি (strcmp(name, "auto_rebind") == 0)
	প্রত্যাগম self->auto_rebind
	    ? ALWAYS_TRUE
	    : ALWAYS_FALSE;
    অন্যথা যদি (strcmp(name, "__members__") == 0)
	প্রত্যাগম ObjectDir(রিক্ত, FunctionAttrs);
    প্রত্যাগম রিক্ত;
}

/*
 * Populate partial_T given function object.
 *
 * "exported" should be set to true when it is needed to construct a partial
 * that may be stored in a variable (i.e. may be freed by Vim).
 */
    স্থির শূন্য
set_partial(FunctionObject *self, partial_T *pt, পূর্ণাঙ্ক exported)
{
    পূর্ণাঙ্ক i;

    pt->pt_name = self->name;
    যদি (self->argv)
    {
	pt->pt_argc = self->argc;
	যদি (exported)
	{
	    pt->pt_argv = ALLOC_CLEAR_MULT(typval_T, self->argc);
	    প্রতি (i = 0; i < pt->pt_argc; ++i)
		copy_tv(&self->argv[i], &pt->pt_argv[i]);
	}
	অন্যথা
	    pt->pt_argv = self->argv;
    }
    অন্যথা
    {
	pt->pt_argc = 0;
	pt->pt_argv = রিক্ত;
    }
    pt->pt_auto = self->auto_rebind || !exported;
    pt->pt_dict = self->self;
    যদি (exported && self->self)
	++pt->pt_dict->dv_refcount;
    যদি (exported)
	pt->pt_name = vim_strsave(pt->pt_name);
    pt->pt_refcount = 1;
}

    স্থির PyObject *
FunctionCall(PyObject *self_obj, PyObject *argsObject, PyObject *kwargs)
{
    FunctionObject *self = (FunctionObject*)self_obj;
    char_u	*name = self->name;
    typval_T	args;
    typval_T	selfdicttv;
    typval_T	rettv;
    dict_T	*selfdict = রিক্ত;
    PyObject	*selfdictObject;
    PyObject	*ret;
    পূর্ণাঙ্ক		error;
    partial_T	pt;
    partial_T	*pt_ptr = রিক্ত;

    যদি (ConvertFromPySequence(argsObject, &args) == -1)
	প্রত্যাগম রিক্ত;

    যদি (kwargs != রিক্ত)
    {
	selfdictObject = PyDict_GetItemString(kwargs, "self");
	যদি (selfdictObject != রিক্ত)
	{
	    যদি (ConvertFromPyMapping(selfdictObject, &selfdicttv) == -1)
	    {
		clear_tv(&args);
		প্রত্যাগম রিক্ত;
	    }
	    selfdict = selfdicttv.vval.v_dict;
	}
    }

    যদি (self->argv || self->self)
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

    যদি (VimTryEnd())
	ret = রিক্ত;
    অন্যথা যদি (error != OK)
    {
	ret = রিক্ত;
	PyErr_VIM_FORMAT(N_("failed to run function %s"), (বর্ণ *)name);
    }
    অন্যথা
	ret = ConvertToPyObject(&rettv);

    clear_tv(&args);
    clear_tv(&rettv);
    যদি (selfdict != রিক্ত)
	clear_tv(&selfdicttv);

    প্রত্যাগম ret;
}

    স্থির PyObject *
FunctionRepr(PyObject *self_obj)
{
    FunctionObject *self = (FunctionObject*)self_obj;
    PyObject *ret;
    garray_T repr_ga;
    পূর্ণাঙ্ক i;
    char_u *tofree = রিক্ত;
    typval_T tv;
    char_u numbuf[NUMBUFLEN];

    ga_init2(&repr_ga, পরিমাণ(বর্ণ), 70);
    ga_concat(&repr_ga, (char_u *)"<vim.Function '");
    যদি (self->name)
	ga_concat(&repr_ga, self->name);
    অন্যথা
	ga_concat(&repr_ga, (char_u *)"<NULL>");
    ga_append(&repr_ga, '\'');
    যদি (self->argv)
    {
	ga_concat(&repr_ga, (char_u *)", args=[");
	++emsg_silent;
	প্রতি (i = 0; i < self->argc; i++)
	{
	    যদি (i != 0)
		ga_concat(&repr_ga, (char_u *)", ");
	    ga_concat(&repr_ga, tv2string(&self->argv[i], &tofree, numbuf,
			get_copyID()));
	    vim_free(tofree);
	}
	--emsg_silent;
	ga_append(&repr_ga, ']');
    }
    যদি (self->self)
    {
	ga_concat(&repr_ga, (char_u *)", self=");
	tv.v_type = VAR_DICT;
	tv.vval.v_dict = self->self;
	++emsg_silent;
	ga_concat(&repr_ga, tv2string(&tv, &tofree, numbuf, get_copyID()));
	--emsg_silent;
	vim_free(tofree);
	যদি (self->auto_rebind)
	    ga_concat(&repr_ga, (char_u *)", auto_rebind=True");
    }
    ga_append(&repr_ga, '>');
    ret = PyString_FromString((বর্ণ *)repr_ga.ga_data);
    ga_clear(&repr_ga);
    প্রত্যাগম ret;
}

স্থির সংরচনা PyMethodDef FunctionMethods[] = {
    {"__dir__",	(PyCFunction)FunctionDir,   METH_NOARGS,		""},
    { রিক্ত,	রিক্ত,			0,				রিক্ত}
};

/*
 * Options object
 */

DEFINE_PY_TYPE_OBJECT(OptionsType);

প্রকারনির্দেশ পূর্ণাঙ্ক (*checkfun)(শূন্য *);

প্রকারনির্দেশ সংরচনা
{
    PyObject_HEAD
    পূর্ণাঙ্ক		opt_type;
    শূন্য	*from;
    checkfun	Check;
    PyObject	*fromObj;
} OptionsObject;

    স্থির পূর্ণাঙ্ক
dummy_check(শূন্য *arg UNUSED)
{
    প্রত্যাগম 0;
}

    স্থির PyObject *
OptionsNew(পূর্ণাঙ্ক opt_type, শূন্য *from, checkfun Check, PyObject *fromObj)
{
    OptionsObject	*self;

    self = PyObject_GC_New(OptionsObject, OptionsTypePtr);
    যদি (self == রিক্ত)
	প্রত্যাগম রিক্ত;

    self->opt_type = opt_type;
    self->from = from;
    self->Check = Check;
    self->fromObj = fromObj;
    যদি (fromObj)
	Py_INCREF(fromObj);

    প্রত্যাগম (PyObject *)(self);
}

    স্থির শূন্য
OptionsDestructor(PyObject *self_obj)
{
    OptionsObject *self = (OptionsObject*)self_obj;
    PyObject_GC_UnTrack((শূন্য *)(self));
    Py_XDECREF(self->fromObj);
    PyObject_GC_Del((শূন্য *)(self));
}

    স্থির পূর্ণাঙ্ক
OptionsTraverse(PyObject *self_obj, visitproc visit, শূন্য *arg)
{
    OptionsObject *self = (OptionsObject*)self_obj;
    Py_VISIT(self->fromObj);
    প্রত্যাগম 0;
}

    স্থির পূর্ণাঙ্ক
OptionsClear(PyObject *self_obj)
{
    OptionsObject *self = (OptionsObject*)self_obj;
    Py_CLEAR(self->fromObj);
    প্রত্যাগম 0;
}

    স্থির PyObject *
OptionsItem(OptionsObject *self, PyObject *চাবি_Object)
{
    char_u	*চাবি;
    পূর্ণাঙ্ক		flags;
    long	numval;
    char_u	*stringval;
    PyObject	*todecref;

    যদি (self->Check(self->fromObj))
	প্রত্যাগম রিক্ত;

    যদি (!(চাবি = StringToChars(চাবি_Object, &todecref)))
	প্রত্যাগম রিক্ত;

    যদি (*চাবি == NUL)
    {
	RAISE_NO_EMPTY_চাবিগুলি;
	Py_XDECREF(todecref);
	প্রত্যাগম রিক্ত;
    }

    flags = get_option_মান_strict(চাবি, &numval, &stringval,
				    self->opt_type, self->from);

    Py_XDECREF(todecref);

    যদি (flags == 0)
    {
	PyErr_SetObject(Py_Exc_চাবি_Error, চাবি_Object);
	প্রত্যাগম রিক্ত;
    }

    যদি (flags & SOPT_UNSET)
    {
	Py_INCREF(Py_None);
	প্রত্যাগম Py_None;
    }
    অন্যথা যদি (flags & SOPT_BOOL)
    {
	PyObject	*ret;
	ret = numval ? Py_True : Py_False;
	Py_INCREF(ret);
	প্রত্যাগম ret;
    }
    অন্যথা যদি (flags & SOPT_অঙ্ক)
	প্রত্যাগম PyInt_FromLong(numval);
    অন্যথা যদি (flags & SOPT_STRING)
    {
	যদি (stringval)
	{
	    PyObject	*ret = PyBytes_FromString((বর্ণ *)stringval);
	    vim_free(stringval);
	    প্রত্যাগম ret;
	}
	অন্যথা
	{
	    PyErr_SET_STRING(PyExc_RuntimeError,
		    N_("unable to get option value"));
	    প্রত্যাগম রিক্ত;
	}
    }
    অন্যথা
    {
	PyErr_SET_VIM(N_("internal error: unknown option type"));
	প্রত্যাগম রিক্ত;
    }
}

    স্থির পূর্ণাঙ্ক
OptionsContains(OptionsObject *self, PyObject *চাবি_Object)
{
    char_u	*চাবি;
    PyObject	*todecref;

    যদি (!(চাবি = StringToChars(চাবি_Object, &todecref)))
	প্রত্যাগম -1;

    যদি (*চাবি == NUL)
    {
	Py_XDECREF(todecref);
	প্রত্যাগম 0;
    }

    যদি (get_option_মান_strict(চাবি, রিক্ত, রিক্ত, self->opt_type, রিক্ত))
    {
	Py_XDECREF(todecref);
	প্রত্যাগম 1;
    }
    অন্যথা
    {
	Py_XDECREF(todecref);
	প্রত্যাগম 0;
    }
}

প্রকারনির্দেশ সংরচনা
{
    শূন্য	*lastoption;
    পূর্ণাঙ্ক		opt_type;
} optiterinfo_T;

    স্থির PyObject *
OptionsIterNext(শূন্য **arg)
{
    char_u	*name;
    optiterinfo_T **oii = (optiterinfo_T**)arg;

    যদি ((name = option_iter_next(&((*oii)->lastoption), (*oii)->opt_type)))
	প্রত্যাগম PyString_FromString((বর্ণ *)name);

    প্রত্যাগম রিক্ত;
}

    স্থির PyObject *
OptionsIter(PyObject *self_obj)
{
    OptionsObject	*self = (OptionsObject*)self_obj;
    optiterinfo_T	*oii;

    যদি (!(oii = PyMem_New(optiterinfo_T, 1)))
    {
	PyErr_NoMemory();
	প্রত্যাগম রিক্ত;
    }

    oii->opt_type = self->opt_type;
    oii->lastoption = রিক্ত;

    প্রত্যাগম IterNew(oii,
	    PyMem_Free, OptionsIterNext,
	    রিক্ত, রিক্ত, (PyObject *)self);
}

    স্থির পূর্ণাঙ্ক
set_option_মান_err(char_u *চাবি, পূর্ণাঙ্ক numval, char_u *stringval, পূর্ণাঙ্ক opt_flags)
{
    বর্ণ	*errmsg;

    যদি ((errmsg = set_option_মান(চাবি, numval, stringval, opt_flags)))
    {
	যদি (VimTryEnd())
	    প্রত্যাগম FAIL;
	PyErr_SetVim(errmsg);
	প্রত্যাগম FAIL;
    }
    প্রত্যাগম OK;
}

    স্থির পূর্ণাঙ্ক
set_option_মান_for(
	char_u *চাবি,
	পূর্ণাঙ্ক numval,
	char_u *stringval,
	পূর্ণাঙ্ক opt_flags,
	পূর্ণাঙ্ক opt_type,
	শূন্য *from)
{
    switchwin_T	switchwin;
    bufref_T	save_curbuf;
    পূর্ণাঙ্ক		set_ret = 0;

    VimTryStart();
    বিকল্প (opt_type)
    {
	পক্ষ SREQ_WIN:
	    যদি (switch_win(&switchwin, (win_T *)from,
			      win_find_tabpage((win_T *)from), FALSE) == FAIL)
	    {
		restore_win(&switchwin, TRUE);
		যদি (VimTryEnd())
		    প্রত্যাগম -1;
		PyErr_SET_VIM(N_("problem while switching windows"));
		প্রত্যাগম -1;
	    }
	    set_ret = set_option_মান_err(চাবি, numval, stringval, opt_flags);
	    restore_win(&switchwin, TRUE);
	    ভঙ্গ;
	পক্ষ SREQ_BUF:
	    switch_buffer(&save_curbuf, (buf_T *)from);
	    set_ret = set_option_মান_err(চাবি, numval, stringval, opt_flags);
	    restore_buffer(&save_curbuf);
	    ভঙ্গ;
	পক্ষ SREQ_GLOBAL:
	    set_ret = set_option_মান_err(চাবি, numval, stringval, opt_flags);
	    ভঙ্গ;
    }
    যদি (set_ret == FAIL)
	প্রত্যাগম -1;
    প্রত্যাগম VimTryEnd();
}

    স্থির পূর্ণাঙ্ক
OptionsAssItem(OptionsObject *self, PyObject *চাবি_Object, PyObject *valObject)
{
    char_u	*চাবি;
    পূর্ণাঙ্ক		flags;
    পূর্ণাঙ্ক		opt_flags;
    পূর্ণাঙ্ক		ret = 0;
    PyObject	*todecref;

    যদি (self->Check(self->fromObj))
	প্রত্যাগম -1;

    যদি (!(চাবি = StringToChars(চাবি_Object, &todecref)))
	প্রত্যাগম -1;

    যদি (*চাবি == NUL)
    {
	RAISE_NO_EMPTY_চাবিগুলি;
	Py_XDECREF(todecref);
	প্রত্যাগম -1;
    }

    flags = get_option_মান_strict(চাবি, রিক্ত, রিক্ত,
				    self->opt_type, self->from);

    যদি (flags == 0)
    {
	PyErr_SetObject(Py_Exc_চাবি_Error, চাবি_Object);
	Py_XDECREF(todecref);
	প্রত্যাগম -1;
    }

    যদি (valObject == রিক্ত)
    {
	যদি (self->opt_type == SREQ_GLOBAL)
	{
	    PyErr_FORMAT(Py_Exc_মান_Error,
		    N_("unable to unset global option %s"), চাবি);
	    Py_XDECREF(todecref);
	    প্রত্যাগম -1;
	}
	অন্যথা যদি (!(flags & SOPT_GLOBAL))
	{
	    PyErr_FORMAT(Py_Exc_মান_Error,
		    N_("unable to unset option %s "
		       "which does not have global value"), চাবি);
	    Py_XDECREF(todecref);
	    প্রত্যাগম -1;
	}
	অন্যথা
	{
	    unset_global_local_option(চাবি, self->from);
	    Py_XDECREF(todecref);
	    প্রত্যাগম 0;
	}
    }

    opt_flags = (self->opt_type ? OPT_LOCAL : OPT_GLOBAL);

    যদি (flags & SOPT_BOOL)
    {
	পূর্ণাঙ্ক	istrue = PyObject_IsTrue(valObject);

	যদি (istrue == -1)
	    ret = -1;
	অন্যথা
	    ret = set_option_মান_for(চাবি, istrue, রিক্ত,
				    opt_flags, self->opt_type, self->from);
    }
    অন্যথা যদি (flags & SOPT_অঙ্ক)
    {
	long	val;

	যদি (সংখ্যা_To_Long(valObject, &val, সংখ্যা_INT))
	{
	    Py_XDECREF(todecref);
	    প্রত্যাগম -1;
	}

	ret = set_option_মান_for(চাবি, (পূর্ণাঙ্ক) val, রিক্ত, opt_flags,
				self->opt_type, self->from);
    }
    অন্যথা
    {
	char_u		*val;
	PyObject	*todecref2;

	যদি ((val = StringToChars(valObject, &todecref2)))
	{
	    ret = set_option_মান_for(চাবি, 0, val, opt_flags,
				    self->opt_type, self->from);
	    Py_XDECREF(todecref2);
	}
	অন্যথা
	    ret = -1;
    }

    Py_XDECREF(todecref);

    প্রত্যাগম ret;
}

স্থির PySequenceMethods OptionsAsSeq = {
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

স্থির PyMappingMethods OptionsAsMapping = {
    (lenfunc)       রিক্ত,
    (binaryfunc)    OptionsItem,
    (objobjargproc) OptionsAssItem,
};

// Tabpage object

প্রকারনির্দেশ সংরচনা
{
    PyObject_HEAD
    tabpage_T	*tab;
} TabPageObject;

স্থির PyObject *WinListNew(TabPageObject *tabObject);

DEFINE_PY_TYPE_OBJECT(TabPageType);

    স্থির পূর্ণাঙ্ক
CheckTabPage(TabPageObject *self)
{
    যদি (self->tab == INVALID_TABPAGE_মান)
    {
	PyErr_SET_VIM(N_("attempt to refer to deleted tab page"));
	প্রত্যাগম -1;
    }

    প্রত্যাগম 0;
}

    স্থির PyObject *
TabPageNew(tabpage_T *tab)
{
    TabPageObject *self;

    যদি (TAB_PYTHON_REF(tab))
    {
	self = TAB_PYTHON_REF(tab);
	Py_INCREF((PyObject *)self);
    }
    অন্যথা
    {
	self = PyObject_NEW(TabPageObject, TabPageTypePtr);
	যদি (self == রিক্ত)
	    প্রত্যাগম রিক্ত;
	self->tab = tab;
	TAB_PYTHON_REF(tab) = self;
    }

    প্রত্যাগম (PyObject *)(self);
}

    স্থির শূন্য
TabPageDestructor(PyObject *self_obj)
{
    TabPageObject *self = (TabPageObject*)self_obj;
    যদি (self->tab && self->tab != INVALID_TABPAGE_মান)
	TAB_PYTHON_REF(self->tab) = রিক্ত;

    DESTRUCTOR_FINISH(self);
}

স্থির বর্ণ *TabPageAttrs[] = {
    "windows", "number", "vars", "window", "valid",
    রিক্ত
};

    স্থির PyObject *
TabPageDir(PyObject *self, PyObject *args UNUSED)
{
    প্রত্যাগম ObjectDir(self, TabPageAttrs);
}

    স্থির PyObject *
TabPageAttrValid(TabPageObject *self, বর্ণ *name)
{
    PyObject	*ret;

    যদি (strcmp(name, "valid") != 0)
	প্রত্যাগম রিক্ত;

    ret = ((self->tab == INVALID_TABPAGE_মান) ? Py_False : Py_True);
    Py_INCREF(ret);
    প্রত্যাগম ret;
}

    স্থির PyObject *
TabPageAttr(TabPageObject *self, বর্ণ *name)
{
    যদি (strcmp(name, "windows") == 0)
	প্রত্যাগম WinListNew(self);
    অন্যথা যদি (strcmp(name, "number") == 0)
	প্রত্যাগম PyLong_FromLong((long) get_tab_সংখ্যা(self->tab));
    অন্যথা যদি (strcmp(name, "vars") == 0)
	প্রত্যাগম NEW_DICTIONARY(self->tab->tp_vars);
    অন্যথা যদি (strcmp(name, "window") == 0)
    {
	// For current tab window.c does not bother to set or update tp_curwin
	যদি (self->tab == curtab)
	    প্রত্যাগম WindowNew(curwin, curtab);
	অন্যথা
	    প্রত্যাগম WindowNew(self->tab->tp_curwin, self->tab);
    }
    অন্যথা যদি (strcmp(name, "__members__") == 0)
	প্রত্যাগম ObjectDir(রিক্ত, TabPageAttrs);
    প্রত্যাগম রিক্ত;
}

    স্থির PyObject *
TabPageRepr(PyObject *self_obj)
{
    TabPageObject *self = (TabPageObject*)self_obj;
    যদি (self->tab == INVALID_TABPAGE_মান)
	প্রত্যাগম PyString_FromFormat("<tabpage object (deleted) at %p>", (শূন্য *)self);
    অন্যথা
    {
	পূর্ণাঙ্ক	t = get_tab_সংখ্যা(self->tab);

	যদি (t == 0)
	    প্রত্যাগম PyString_FromFormat("<tabpage object (unknown) at %p>",
					(শূন্য *)self);
	অন্যথা
	    প্রত্যাগম PyString_FromFormat("<tabpage %d>", t - 1);
    }
}

স্থির সংরচনা PyMethodDef TabPageMethods[] = {
    // name,	    function,			calling,	documentation
    {"__dir__",	    (PyCFunction)TabPageDir,	METH_NOARGS,	""},
    { রিক্ত,	    রিক্ত,			0,		রিক্ত}
};

/*
 * Window list object
 */

DEFINE_PY_TYPE_OBJECT(TabListType);
স্থির PySequenceMethods TabListAsSeq;

প্রকারনির্দেশ সংরচনা
{
    PyObject_HEAD
} TabListObject;

স্থির TabListObject TheTabPageList =
{
    PyObject_HEAD_INIT_TYPE(TabListType)
};

    স্থির PyInt
TabListLength(PyObject *self UNUSED)
{
    tabpage_T	*tp = first_tabpage;
    PyInt	n = 0;

    যাবত্ (tp != রিক্ত)
    {
	++n;
	tp = tp->tp_next;
    }

    প্রত্যাগম n;
}

    স্থির PyObject *
TabListItem(PyObject *self UNUSED, PyInt n)
{
    tabpage_T	*tp;

    প্রতি (tp = first_tabpage; tp != রিক্ত; tp = tp->tp_next, --n)
	যদি (n == 0)
	    প্রত্যাগম TabPageNew(tp);

    PyErr_SET_STRING(PyExc_IndexError, N_("no such tab page"));
    প্রত্যাগম রিক্ত;
}

/*
 * Window object
 */

প্রকারনির্দেশ সংরচনা
{
    PyObject_HEAD
    win_T	*win;
    TabPageObject	*tabObject;
} WindowObject;

DEFINE_PY_TYPE_OBJECT(WindowType);

    স্থির পূর্ণাঙ্ক
CheckWindow(WindowObject *self)
{
    যদি (self->win == INVALID_WINDOW_মান)
    {
	PyErr_SET_VIM(N_("attempt to refer to deleted window"));
	প্রত্যাগম -1;
    }

    প্রত্যাগম 0;
}

    স্থির পূর্ণাঙ্ক
CheckWindowCb(শূন্য *self)
{
    প্রত্যাগম CheckWindow((WindowObject*)self);
}

    স্থির PyObject *
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

    যদি (WIN_PYTHON_REF(win))
    {
	self = WIN_PYTHON_REF(win);
	Py_INCREF((PyObject *)self);
    }
    অন্যথা
    {
	self = PyObject_GC_New(WindowObject, WindowTypePtr);
	যদি (self == রিক্ত)
	    প্রত্যাগম রিক্ত;
	self->win = win;
	WIN_PYTHON_REF(win) = self;
    }

    self->tabObject = ((TabPageObject *)(TabPageNew(tab)));

    প্রত্যাগম (PyObject *)(self);
}

    স্থির শূন্য
WindowDestructor(PyObject *self_obj)
{
    WindowObject *self = (WindowObject*)self_obj;
    PyObject_GC_UnTrack((শূন্য *)(self));
    যদি (self->win && self->win != INVALID_WINDOW_মান)
	WIN_PYTHON_REF(self->win) = রিক্ত;
    Py_XDECREF(((PyObject *)(self->tabObject)));
    PyObject_GC_Del((শূন্য *)(self));
}

    স্থির পূর্ণাঙ্ক
WindowTraverse(PyObject *self_obj, visitproc visit, শূন্য *arg)
{
    WindowObject *self = (WindowObject*)self_obj;
    Py_VISIT(((PyObject *)(self->tabObject)));
    প্রত্যাগম 0;
}

    স্থির পূর্ণাঙ্ক
WindowClear(PyObject *self_obj)
{
    WindowObject *self = (WindowObject*)self_obj;
    Py_CLEAR(self->tabObject);
    প্রত্যাগম 0;
}

    স্থির win_T *
get_firstwin(TabPageObject *tabObject)
{
    যদি (tabObject)
    {
	যদি (CheckTabPage(tabObject))
	    প্রত্যাগম রিক্ত;
	// For current tab window.c does not bother to set or update tp_firstwin
	অন্যথা যদি (tabObject->tab == curtab)
	    প্রত্যাগম firstwin;
	অন্যথা
	    প্রত্যাগম tabObject->tab->tp_firstwin;
    }
    অন্যথা
	প্রত্যাগম firstwin;
}

// Use the same order as in the WindowAttr() function.
স্থির বর্ণ *WindowAttrs[] = {
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
    রিক্ত
};

    স্থির PyObject *
WindowDir(PyObject *self, PyObject *args UNUSED)
{
    প্রত্যাগম ObjectDir(self, WindowAttrs);
}

    স্থির PyObject *
WindowAttrValid(WindowObject *self, বর্ণ *name)
{
    PyObject	*ret;

    যদি (strcmp(name, "valid") != 0)
	প্রত্যাগম রিক্ত;

    ret = ((self->win == INVALID_WINDOW_মান) ? Py_False : Py_True);
    Py_INCREF(ret);
    প্রত্যাগম ret;
}

    স্থির PyObject *
WindowAttr(WindowObject *self, বর্ণ *name)
{
    যদি (strcmp(name, "buffer") == 0)
	প্রত্যাগম (PyObject *)BufferNew(self->win->w_buffer);
    অন্যথা যদি (strcmp(name, "cursor") == 0)
    {
	pos_T *pos = &self->win->w_cursor;

	প্রত্যাগম Py_Build_মান("(ll)", (long)(pos->lnum), (long)(pos->col));
    }
    অন্যথা যদি (strcmp(name, "height") == 0)
	প্রত্যাগম PyLong_FromLong((long)(self->win->w_height));
    অন্যথা যদি (strcmp(name, "row") == 0)
	প্রত্যাগম PyLong_FromLong((long)(self->win->w_winrow));
    অন্যথা যদি (strcmp(name, "width") == 0)
	প্রত্যাগম PyLong_FromLong((long)(self->win->w_width));
    অন্যথা যদি (strcmp(name, "col") == 0)
	প্রত্যাগম PyLong_FromLong((long)(self->win->w_wincol));
    অন্যথা যদি (strcmp(name, "vars") == 0)
	প্রত্যাগম NEW_DICTIONARY(self->win->w_vars);
    অন্যথা যদি (strcmp(name, "options") == 0)
	প্রত্যাগম OptionsNew(SREQ_WIN, self->win, CheckWindowCb,
			(PyObject *) self);
    অন্যথা যদি (strcmp(name, "number") == 0)
    {
	যদি (CheckTabPage(self->tabObject))
	    প্রত্যাগম রিক্ত;
	প্রত্যাগম PyLong_FromLong((long)
		get_win_সংখ্যা(self->win, get_firstwin(self->tabObject)));
    }
    অন্যথা যদি (strcmp(name, "tabpage") == 0)
    {
	Py_INCREF((PyObject *)self->tabObject);
	প্রত্যাগম (PyObject *)(self->tabObject);
    }
    অন্যথা যদি (strcmp(name, "__members__") == 0)
	প্রত্যাগম ObjectDir(রিক্ত, WindowAttrs);
    অন্যথা
	প্রত্যাগম রিক্ত;
}

    স্থির পূর্ণাঙ্ক
WindowSetattr(PyObject *self_obj, বর্ণ *name, PyObject *valObject)
{
    WindowObject *self = (WindowObject*)self_obj;
    যদি (CheckWindow(self))
	প্রত্যাগম -1;

    যদি (strcmp(name, "buffer") == 0)
    {
	PyErr_SET_STRING(PyExc_TypeError, N_("readonly attribute: buffer"));
	প্রত্যাগম -1;
    }
    অন্যথা যদি (strcmp(name, "cursor") == 0)
    {
	long lnum;
	long col;

	যদি (!PyArg_Parse(valObject, "(ll)", &lnum, &col))
	    প্রত্যাগম -1;

	যদি (lnum <= 0 || lnum > self->win->w_buffer->b_ml.ml_সারি_গুনতি)
	{
	    PyErr_SET_VIM(N_("cursor position outside buffer"));
	    প্রত্যাগম -1;
	}

	// Check for keyboard interrupts
	যদি (VimCheckInterrupt())
	    প্রত্যাগম -1;

	self->win->w_cursor.lnum = lnum;
	self->win->w_cursor.col = col;
	self->win->w_set_curswant = TRUE;
	self->win->w_cursor.coladd = 0;
	// When column is out of range silently correct it.
	check_cursor_col_win(self->win);

	update_screen(UPD_VALID);
	প্রত্যাগম 0;
    }
    অন্যথা যদি (strcmp(name, "height") == 0)
    {
	long	height;
	win_T	*savewin;

	যদি (সংখ্যা_To_Long(valObject, &height, সংখ্যা_INT|সংখ্যা_UNSIGNED))
	    প্রত্যাগম -1;

#ifdef FEAT_GUI
	need_mouse_correct = TRUE;
#endif
	savewin = curwin;
	curwin = self->win;
	curbuf = curwin->w_buffer;

	VimTryStart();
	win_setheight((পূর্ণাঙ্ক) height);
	curwin = savewin;
	curbuf = curwin->w_buffer;
	যদি (VimTryEnd())
	    প্রত্যাগম -1;

	প্রত্যাগম 0;
    }
    অন্যথা যদি (strcmp(name, "width") == 0)
    {
	long	width;
	win_T	*savewin;

	যদি (সংখ্যা_To_Long(valObject, &width, সংখ্যা_INT|সংখ্যা_UNSIGNED))
	    প্রত্যাগম -1;

#ifdef FEAT_GUI
	need_mouse_correct = TRUE;
#endif
	savewin = curwin;
	curwin = self->win;
	curbuf = curwin->w_buffer;

	VimTryStart();
	win_setwidth((পূর্ণাঙ্ক) width);
	curwin = savewin;
	curbuf = curwin->w_buffer;
	যদি (VimTryEnd())
	    প্রত্যাগম -1;

	প্রত্যাগম 0;
    }
    অন্যথা
    {
	PyErr_SetString(PyExc_AttributeError, name);
	প্রত্যাগম -1;
    }
}

    স্থির PyObject *
WindowRepr(PyObject *self_obj)
{
    WindowObject *self = (WindowObject*)self_obj;
    যদি (self->win == INVALID_WINDOW_মান)
	প্রত্যাগম PyString_FromFormat("<window object (deleted) at %p>", (শূন্য *)self);
    অন্যথা
    {
	পূর্ণাঙ্ক	w = get_win_সংখ্যা(self->win, firstwin);

	যদি (w == 0)
	    প্রত্যাগম PyString_FromFormat("<window object (unknown) at %p>",
								      (শূন্য *)self);
	অন্যথা
	    প্রত্যাগম PyString_FromFormat("<window %d>", w - 1);
    }
}

স্থির সংরচনা PyMethodDef WindowMethods[] = {
    // name,	    function,			calling,	documentation
    {"__dir__",	    (PyCFunction)WindowDir,	METH_NOARGS,	""},
    { রিক্ত,	    রিক্ত,			0,		রিক্ত}
};

/*
 * Window list object
 */

DEFINE_PY_TYPE_OBJECT(WinListType);
স্থির PySequenceMethods WinListAsSeq;

প্রকারনির্দেশ সংরচনা
{
    PyObject_HEAD
    TabPageObject	*tabObject;
} WinListObject;

স্থির WinListObject TheWindowList =
{
    PyObject_HEAD_INIT_TYPE(WinListType)
    রিক্ত
};

    স্থির PyObject *
WinListNew(TabPageObject *tabObject)
{
    WinListObject	*self;

    self = PyObject_NEW(WinListObject, WinListTypePtr);
    self->tabObject = tabObject;
    Py_INCREF((PyObject *)tabObject);

    প্রত্যাগম (PyObject *)(self);
}

    স্থির শূন্য
WinListDestructor(WinListObject *self)
{
    TabPageObject	*tabObject = self->tabObject;

    যদি (tabObject)
    {
	Py_DECREF((PyObject *)(tabObject));
    }

    DESTRUCTOR_FINISH(self);
}

    স্থির PyInt
WinListLength(WinListObject *self)
{
    win_T	*w;
    PyInt	n = 0;

    যদি (!(w = get_firstwin(self->tabObject)))
	প্রত্যাগম -1;

    যাবত্ (w != রিক্ত)
    {
	++n;
	w = W_NEXT(w);
    }

    প্রত্যাগম n;
}

    স্থির PyObject *
WinListItem(WinListObject *self, PyInt n)
{
    win_T *w;

    যদি (!(w = get_firstwin(self->tabObject)))
	প্রত্যাগম রিক্ত;

    প্রতি (; w != রিক্ত; w = W_NEXT(w), --n)
	যদি (n == 0)
	    প্রত্যাগম WindowNew(w, self->tabObject? self->tabObject->tab: curtab);

    PyErr_SET_STRING(PyExc_IndexError, N_("no such window"));
    প্রত্যাগম রিক্ত;
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
    স্থির বর্ণ *
String_To_সারি(PyObject *obj)
{
    বর্ণ	*str;
    বর্ণ	*save;
    PyObject	*bytes = রিক্ত;
    Py_ssize_t	len = 0;
    PyInt	i;
    বর্ণ	*p;

    যদি (PyBytes_Check(obj))
    {
	যদি (PyBytes_AsStringAndSize(obj, &str, &len) == -1
		|| str == রিক্ত)
	    প্রত্যাগম রিক্ত;
    }
    অন্যথা যদি (PyUnicode_Check(obj))
    {
	যদি (!(bytes = PyUnicode_AsEncodedString(obj, ENC_OPT,
							   ERRORS_ENCODE_ARG)))
	    প্রত্যাগম রিক্ত;

	যদি (PyBytes_AsStringAndSize(bytes, &str, &len) == -1
		|| str == রিক্ত)
	{
	    Py_DECREF(bytes);
	    প্রত্যাগম রিক্ত;
	}
    }
    অন্যথা
    {
#যদি PY_MAJOR_VERSION < 3
	PyErr_FORMAT_TYPE(
		N_("expected str() or unicode() instance, but got %s"),
		obj);
#অন্যথা
	PyErr_FORMAT_TYPE(
		N_("expected bytes() or str() instance, but got %s"),
		obj);
#endif
	প্রত্যাগম রিক্ত;
    }

    /*
     * Error checking: String must not contain newlines, as we
     * are replacing a single line, and we must replace it with
     * a single line.
     * A trailing newline is removed, so that append(f.readlines()) works.
     */
    p = memchr(str, '\n', len);
    যদি (p != রিক্ত)
    {
	যদি (p == str + len - 1)
	    --len;
	অন্যথা
	{
	    PyErr_SET_VIM(N_("string cannot contain newlines"));
	    Py_XDECREF(bytes);
	    প্রত্যাগম রিক্ত;
	}
    }

    /*
     * Create a copy of the string, with internal nulls replaced by
     * newline characters, as is the Vim convention.
     */
    save = alloc(len+1);
    যদি (save == রিক্ত)
    {
	PyErr_NoMemory();
	Py_XDECREF(bytes);
	প্রত্যাগম রিক্ত;
    }

    প্রতি (i = 0; i < len; ++i)
    {
	যদি (str[i] == '\0')
	    save[i] = '\n';
	অন্যথা
	    save[i] = str[i];
    }

    save[i] = '\0';
    Py_XDECREF(bytes);  // Python 2 does nothing here

    প্রত্যাগম save;
}

/*
 * Get a line from the specified buffer. The line number is
 * in Vim format (1-based). The line is returned as a Python
 * string object.
 */
    স্থির PyObject *
Get_Buffer_সারি(buf_T *buf, PyInt n)
{
    প্রত্যাগম সারি_To_String((বর্ণ *)ml_get_buf(buf, (linenr_T)n, FALSE));
}


/*
 * Get a list of lines from the specified buffer. The line numbers
 * are in Vim format (1-based). The range is from lo up to, but not
 * including, hi. The list is returned as a Python list of string objects.
 */
    স্থির PyObject *
Get_Buffer_সারি_List(buf_T *buf, PyInt lo, PyInt hi)
{
    PyInt	i;
    PyInt	n = hi - lo;
    PyObject	*list = PyList_New(n);

    যদি (list == রিক্ত)
	প্রত্যাগম রিক্ত;

    প্রতি (i = 0; i < n; ++i)
    {
	linenr_T	lnum = (linenr_T)(lo + i);
	বর্ণ		*text;
	PyObject	*string;

	যদি (lnum > buf->b_ml.ml_সারি_গুনতি)
	    text = "";
	অন্যথা
	    text = (বর্ণ *)ml_get_buf(buf, lnum, FALSE);
	string = সারি_To_String(text);
	যদি (string == রিক্ত)
	{
	    Py_DECREF(list);
	    প্রত্যাগম রিক্ত;
	}

	PyList_SET_ITEM(list, i, string);
    }

    // The ownership of the Python list is passed to the caller (ie,
    // the caller should Py_DECREF() the object when it is finished
    // with it).

    প্রত্যাগম list;
}

/*
 * Check if deleting lines made the cursor position invalid.
 * Changed the lines from "lo" to "hi" and added "extra" lines (negative if
 * deleted).
 */
    স্থির শূন্য
py_fix_cursor(linenr_T lo, linenr_T hi, linenr_T extra)
{
    যদি (curwin->w_cursor.lnum >= lo)
    {
	// Adjust the cursor position if it's in/after the changed
	// lines.
	যদি (curwin->w_cursor.lnum >= hi)
	{
	    curwin->w_cursor.lnum += extra;
	    check_cursor_col();
	}
	অন্যথা যদি (extra < 0)
	{
	    curwin->w_cursor.lnum = lo;
	    check_cursor();
	}
	অন্যথা
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
    স্থির পূর্ণাঙ্ক
Set_Buffer_সারি(buf_T *buf, PyInt n, PyObject *সারি, PyInt *len_change)
{
    bufref_T	save_curbuf = {রিক্ত, 0, 0};
    switchwin_T	switchwin;

    // First of all, we check the type of the supplied Python object.
    // There are three cases:
    //	  1. NULL, or None - this is a deletion.
    //	  2. A string	   - this is a replacement.
    //	  3. Anything else - this is an error.
    যদি (সারি == Py_None || সারি == রিক্ত)
    {
	PyErr_Clear();
	switchwin.sw_curwin = রিক্ত;
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	VimTryStart();

	যদি (u_savedel((linenr_T)n, 1L) == FAIL)
	    RAISE_UNDO_FAIL;
	অন্যথা যদি (ml_delete((linenr_T)n) == FAIL)
	    RAISE_DELETE_সারি_FAIL;
	অন্যথা
	{
	    যদি (buf == curbuf && (switchwin.sw_curwin != রিক্ত
					   || save_curbuf.br_buf == রিক্ত))
		// Using an existing window for the buffer, adjust the cursor
		// position.
		py_fix_cursor((linenr_T)n, (linenr_T)n + 1, (linenr_T)-1);
	    যদি (save_curbuf.br_buf == রিক্ত)
		// Only adjust marks if we managed to switch to a window that
		// holds the buffer, otherwise line numbers will be invalid.
		deleted_সারিগুলি_mark((linenr_T)n, 1L);
	}

	restore_win_for_buf(&switchwin, &save_curbuf);

	যদি (VimTryEnd())
	    প্রত্যাগম FAIL;

	যদি (len_change)
	    *len_change = -1;

	প্রত্যাগম OK;
    }
    অন্যথা যদি (PyBytes_Check(সারি) || PyUnicode_Check(সারি))
    {
	বর্ণ	*save = String_To_সারি(সারি);

	যদি (save == রিক্ত)
	    প্রত্যাগম FAIL;

	VimTryStart();

	// We do not need to free "save" if ml_replace() consumes it.
	PyErr_Clear();
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	যদি (u_savesub((linenr_T)n) == FAIL)
	{
	    RAISE_UNDO_FAIL;
	    vim_free(save);
	}
	অন্যথা যদি (ml_replace((linenr_T)n, (char_u *)save, FALSE) == FAIL)
	{
	    RAISE_REPLACE_সারি_FAIL;
	    vim_free(save);
	}
	অন্যথা
	    changed_bytes((linenr_T)n, 0);

	restore_win_for_buf(&switchwin, &save_curbuf);

	// Check that the cursor is not beyond the end of the line now.
	যদি (buf == curbuf)
	    check_cursor_col();

	যদি (VimTryEnd())
	    প্রত্যাগম FAIL;

	যদি (len_change)
	    *len_change = 0;

	প্রত্যাগম OK;
    }
    অন্যথা
    {
	PyErr_BadArgument();
	প্রত্যাগম FAIL;
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
    স্থির পূর্ণাঙ্ক
Set_Buffer_সারি_List(
	buf_T *buf,
	PyInt lo,
	PyInt hi,
	PyObject *list,
	PyInt *len_change)
{
    bufref_T	save_curbuf = {রিক্ত, 0, 0};
    switchwin_T	switchwin;

    // First of all, we check the type of the supplied Python object.
    // There are three cases:
    //	  1. NULL, or None - this is a deletion.
    //	  2. A list	   - this is a replacement.
    //	  3. Anything else - this is an error.
    যদি (list == Py_None || list == রিক্ত)
    {
	PyInt	i;
	PyInt	n = (পূর্ণাঙ্ক)(hi - lo);

	PyErr_Clear();
	VimTryStart();
	switchwin.sw_curwin = রিক্ত;
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	যদি (u_savedel((linenr_T)lo, (long)n) == FAIL)
	    RAISE_UNDO_FAIL;
	অন্যথা
	{
	    প্রতি (i = 0; i < n; ++i)
	    {
		যদি (ml_delete((linenr_T)lo) == FAIL)
		{
		    RAISE_DELETE_সারি_FAIL;
		    ভঙ্গ;
		}
	    }
	    যদি (buf == curbuf && (switchwin.sw_curwin != রিক্ত
					       || save_curbuf.br_buf == রিক্ত))
		// Using an existing window for the buffer, adjust the cursor
		// position.
		py_fix_cursor((linenr_T)lo, (linenr_T)hi, (linenr_T)-n);
	    যদি (save_curbuf.br_buf == রিক্ত)
		// Only adjust marks if we managed to switch to a window that
		// holds the buffer, otherwise line numbers will be invalid.
		deleted_সারিগুলি_mark((linenr_T)lo, (long)i);
	}

	restore_win_for_buf(&switchwin, &save_curbuf);

	যদি (VimTryEnd())
	    প্রত্যাগম FAIL;

	যদি (len_change)
	    *len_change = -n;

	প্রত্যাগম OK;
    }
    অন্যথা যদি (PyList_Check(list))
    {
	PyInt	i;
	PyInt	new_len = PyList_Size(list);
	PyInt	old_len = hi - lo;
	PyInt	extra = 0;	// lines added to text, can be negative
	বর্ণ	**array;

	যদি (new_len == 0)	// avoid allocating zero bytes
	    array = রিক্ত;
	অন্যথা
	{
	    array = PyMem_New(বর্ণ *, new_len);
	    যদি (array == রিক্ত)
	    {
		PyErr_NoMemory();
		প্রত্যাগম FAIL;
	    }
	}

	প্রতি (i = 0; i < new_len; ++i)
	{
	    PyObject *সারি;

	    যদি (!(সারি = PyList_GetItem(list, i)) ||
		!(array[i] = String_To_সারি(সারি)))
	    {
		যাবত্ (i)
		    vim_free(array[--i]);
		PyMem_Free(array);
		প্রত্যাগম FAIL;
	    }
	}

	VimTryStart();
	PyErr_Clear();

	// START of region without "return".  Must call restore_buffer()!
	switchwin.sw_curwin = রিক্ত;
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	যদি (u_save((linenr_T)(lo-1), (linenr_T)hi) == FAIL)
	    RAISE_UNDO_FAIL;

	// If the size of the range is reducing (ie, new_len < old_len) we
	// need to delete some old_len. We do this at the start, by
	// repeatedly deleting line "lo".
	যদি (!PyErr_Occurred())
	{
	    প্রতি (i = 0; i < old_len - new_len; ++i)
		যদি (ml_delete((linenr_T)lo) == FAIL)
		{
		    RAISE_DELETE_সারি_FAIL;
		    ভঙ্গ;
		}
	    extra -= i;
	}

	// For as long as possible, replace the existing old_len with the
	// new old_len. This is a more efficient operation, as it requires
	// less memory allocation and freeing.
	যদি (!PyErr_Occurred())
	{
	    প্রতি (i = 0; i < old_len && i < new_len; ++i)
		যদি (ml_replace((linenr_T)(lo+i), (char_u *)array[i], FALSE)
								      == FAIL)
		{
		    RAISE_REPLACE_সারি_FAIL;
		    ভঙ্গ;
		}
	}
	অন্যথা
	    i = 0;

	// Now we may need to insert the remaining new old_len. If we do, we
	// must free the strings as we finish with them (we can't pass the
	// responsibility to Vim in this case).
	যদি (!PyErr_Occurred())
	{
	    যাবত্ (i < new_len)
	    {
		যদি (ml_append((linenr_T)(lo + i - 1),
					(char_u *)array[i], 0, FALSE) == FAIL)
		{
		    RAISE_ঢোকাও_সারি_FAIL;
		    ভঙ্গ;
		}
		vim_free(array[i]);
		++i;
		++extra;
	    }
	}

	// Free any left-over old_len, as a result of an error
	যাবত্ (i < new_len)
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
	যদি (save_curbuf.br_buf == রিক্ত)
	{
	    mark_adjust((linenr_T)lo, (linenr_T)(hi - 1),
						  (long)MAXLNUM, (long)extra);
	    changed_সারিগুলি((linenr_T)lo, 0, (linenr_T)hi, (long)extra);
	}

	যদি (buf == curbuf && (switchwin.sw_curwin != রিক্ত
					   || save_curbuf.br_buf == রিক্ত))
	    // Using an existing window for the buffer, adjust the cursor
	    // position.
	    py_fix_cursor((linenr_T)lo, (linenr_T)hi, (linenr_T)extra);

	// END of region without "return".
	restore_win_for_buf(&switchwin, &save_curbuf);

	যদি (VimTryEnd())
	    প্রত্যাগম FAIL;

	যদি (len_change)
	    *len_change = new_len - old_len;

	প্রত্যাগম OK;
    }
    অন্যথা
    {
	PyErr_BadArgument();
	প্রত্যাগম FAIL;
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
    স্থির পূর্ণাঙ্ক
ঢোকাও_Buffer_সারিগুলি(buf_T *buf, PyInt n, PyObject *সারিগুলি, PyInt *len_change)
{
    bufref_T	save_curbuf = {রিক্ত, 0, 0};
    switchwin_T	switchwin;

    // First of all, we check the type of the supplied Python object.
    // It must be a string or a list, or the call is in error.
    যদি (PyBytes_Check(সারিগুলি) || PyUnicode_Check(সারিগুলি))
    {
	বর্ণ		*str = String_To_সারি(সারিগুলি);

	যদি (str == রিক্ত)
	    প্রত্যাগম FAIL;

	PyErr_Clear();
	VimTryStart();
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	যদি (u_save((linenr_T)n, (linenr_T)(n + 1)) == FAIL)
	    RAISE_UNDO_FAIL;
	অন্যথা যদি (ml_append((linenr_T)n, (char_u *)str, 0, FALSE) == FAIL)
	    RAISE_ঢোকাও_সারি_FAIL;
	অন্যথা যদি (save_curbuf.br_buf == রিক্ত)
	    // Only adjust marks if we managed to switch to a window that
	    // holds the buffer, otherwise line numbers will be invalid.
	    appended_সারিগুলি_mark((linenr_T)n, 1L);

	vim_free(str);
	restore_win_for_buf(&switchwin, &save_curbuf);
	update_screen(UPD_VALID);

	যদি (VimTryEnd())
	    প্রত্যাগম FAIL;

	যদি (len_change)
	    *len_change = 1;

	প্রত্যাগম OK;
    }
    অন্যথা যদি (PyList_Check(সারিগুলি))
    {
	PyInt	i;
	PyInt	size = PyList_Size(সারিগুলি);
	বর্ণ	**array;

	array = PyMem_New(বর্ণ *, size);
	যদি (array == রিক্ত)
	{
	    PyErr_NoMemory();
	    প্রত্যাগম FAIL;
	}

	প্রতি (i = 0; i < size; ++i)
	{
	    PyObject *সারি;

	    যদি (!(সারি = PyList_GetItem(সারিগুলি, i)) ||
		!(array[i] = String_To_সারি(সারি)))
	    {
		যাবত্ (i)
		    vim_free(array[--i]);
		PyMem_Free(array);
		প্রত্যাগম FAIL;
	    }
	}

	PyErr_Clear();
	VimTryStart();
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	যদি (u_save((linenr_T)n, (linenr_T)(n + 1)) == FAIL)
	    RAISE_UNDO_FAIL;
	অন্যথা
	{
	    প্রতি (i = 0; i < size; ++i)
	    {
		যদি (ml_append((linenr_T)(n + i),
					(char_u *)array[i], 0, FALSE) == FAIL)
		{
		    RAISE_ঢোকাও_সারি_FAIL;

		    // Free the rest of the lines
		    যাবত্ (i < size)
			vim_free(array[i++]);

		    ভঙ্গ;
		}
		vim_free(array[i]);
	    }
	    যদি (i > 0 && save_curbuf.br_buf == রিক্ত)
		// Only adjust marks if we managed to switch to a window that
		// holds the buffer, otherwise line numbers will be invalid.
		appended_সারিগুলি_mark((linenr_T)n, (long)i);
	}

	// Free the array of lines. All of its contents have now
	// been freed.
	PyMem_Free(array);
	restore_win_for_buf(&switchwin, &save_curbuf);

	update_screen(UPD_VALID);

	যদি (VimTryEnd())
	    প্রত্যাগম FAIL;

	যদি (len_change)
	    *len_change = size;

	প্রত্যাগম OK;
    }
    অন্যথা
    {
	PyErr_BadArgument();
	প্রত্যাগম FAIL;
    }
}

/*
 * Common routines for buffers and line ranges
 * -------------------------------------------
 */

প্রকারনির্দেশ সংরচনা
{
    PyObject_HEAD
    buf_T *buf;
} BufferObject;

    স্থির পূর্ণাঙ্ক
CheckBuffer(BufferObject *self)
{
    যদি (self->buf == INVALID_BUFFER_মান)
    {
	PyErr_SET_VIM(N_("attempt to refer to deleted buffer"));
	প্রত্যাগম -1;
    }

    প্রত্যাগম 0;
}

    স্থির পূর্ণাঙ্ক
CheckBufferCb(শূন্য *self)
{
    প্রত্যাগম CheckBuffer((BufferObject*)self);
}

    স্থির PyObject *
RBItem(BufferObject *self, PyInt n, PyInt start, PyInt end)
{
    যদি (CheckBuffer(self))
	প্রত্যাগম রিক্ত;

    যদি (end == -1)
	end = self->buf->b_ml.ml_সারি_গুনতি;

    যদি (n < 0)
	n += end - start + 1;

    যদি (n < 0 || n > end - start)
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("line number out of range"));
	প্রত্যাগম রিক্ত;
    }

    প্রত্যাগম Get_Buffer_সারি(self->buf, n+start);
}

    স্থির PyObject *
RBSlice(BufferObject *self, PyInt lo, PyInt hi, PyInt start, PyInt end)
{
    PyInt size;

    যদি (CheckBuffer(self))
	প্রত্যাগম রিক্ত;

    যদি (end == -1)
	end = self->buf->b_ml.ml_সারি_গুনতি;

    size = end - start + 1;

    যদি (lo < 0)
	lo = 0;
    অন্যথা যদি (lo > size)
	lo = size;
    যদি (hi < 0)
	hi = 0;
    যদি (hi < lo)
	hi = lo;
    অন্যথা যদি (hi > size)
	hi = size;

    প্রত্যাগম Get_Buffer_সারি_List(self->buf, lo+start, hi+start);
}

    স্থির PyInt
RBAsItem(
	BufferObject *self,
	PyInt n,
	PyObject *valObject,
	PyInt start,
	PyInt end,
	PyInt *new_end)
{
    PyInt len_change;

    যদি (CheckBuffer(self))
	প্রত্যাগম -1;

    যদি (end == -1)
	end = self->buf->b_ml.ml_সারি_গুনতি;

    যদি (n < 0)
	n += end - start + 1;

    যদি (n < 0 || n > end - start)
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("line number out of range"));
	প্রত্যাগম -1;
    }

    যদি (Set_Buffer_সারি(self->buf, n+start, valObject, &len_change) == FAIL)
	প্রত্যাগম -1;

    যদি (new_end)
	*new_end = end + len_change;

    প্রত্যাগম 0;
}

    স্থির PyInt
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
    যদি (CheckBuffer(self))
	প্রত্যাগম -1;

    যদি (end == -1)
	end = self->buf->b_ml.ml_সারি_গুনতি;

    // Sort out the slice range
    size = end - start + 1;

    যদি (lo < 0)
	lo = 0;
    অন্যথা যদি (lo > size)
	lo = size;
    যদি (hi < 0)
	hi = 0;
    যদি (hi < lo)
	hi = lo;
    অন্যথা যদি (hi > size)
	hi = size;

    যদি (Set_Buffer_সারি_List(self->buf, lo + start, hi + start,
						valObject, &len_change) == FAIL)
	প্রত্যাগম -1;

    যদি (new_end)
	*new_end = end + len_change;

    প্রত্যাগম 0;
}


    স্থির PyObject *
RBAppend(
	BufferObject *self,
	PyObject *args,
	PyInt start,
	PyInt end,
	PyInt *new_end)
{
    PyObject *সারিগুলি;
    PyInt len_change;
    PyInt max;
    PyInt n;

    যদি (CheckBuffer(self))
	প্রত্যাগম রিক্ত;

    যদি (end == -1)
	end = self->buf->b_ml.ml_সারি_গুনতি;

    max = n = end - start + 1;

    যদি (!PyArg_ParseTuple(args, "O|n", &সারিগুলি, &n))
	প্রত্যাগম রিক্ত;

    যদি (n < 0 || n > max)
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("line number out of range"));
	প্রত্যাগম রিক্ত;
    }

    যদি (ঢোকাও_Buffer_সারিগুলি(self->buf, n + start - 1, সারিগুলি, &len_change) == FAIL)
	প্রত্যাগম রিক্ত;

    যদি (new_end)
	*new_end = end + len_change;

    Py_INCREF(Py_None);
    প্রত্যাগম Py_None;
}

// Range object

DEFINE_PY_TYPE_OBJECT(RangeType);
স্থির PySequenceMethods RangeAsSeq;
স্থির PyMappingMethods RangeAsMapping;

প্রকারনির্দেশ সংরচনা
{
    PyObject_HEAD
    BufferObject *buf;
    PyInt start;
    PyInt end;
} RangeObject;

    স্থির PyObject *
RangeNew(buf_T *buf, PyInt start, PyInt end)
{
    BufferObject *bufr;
    RangeObject *self;
    self = PyObject_GC_New(RangeObject, RangeTypePtr);
    যদি (self == রিক্ত)
	প্রত্যাগম রিক্ত;

    bufr = (BufferObject *)BufferNew(buf);
    যদি (bufr == রিক্ত)
    {
	Py_DECREF(self);
	প্রত্যাগম রিক্ত;
    }
    Py_INCREF((PyObject *)bufr);

    self->buf = bufr;
    self->start = start;
    self->end = end;

    প্রত্যাগম (PyObject *)(self);
}

    স্থির শূন্য
RangeDestructor(PyObject *self_obj)
{
    RangeObject *self = (RangeObject*)self_obj;
    PyObject_GC_UnTrack((শূন্য *)(self));
    Py_XDECREF(((PyObject *)(self->buf)));
    PyObject_GC_Del((শূন্য *)(self));
}

    স্থির পূর্ণাঙ্ক
RangeTraverse(PyObject *self_obj, visitproc visit, শূন্য *arg)
{
    RangeObject *self = (RangeObject*)self_obj;
    Py_VISIT(((PyObject *)(self->buf)));
    প্রত্যাগম 0;
}

    স্থির পূর্ণাঙ্ক
RangeClear(PyObject *self_obj)
{
    RangeObject *self = (RangeObject*)self_obj;
    Py_CLEAR(self->buf);
    প্রত্যাগম 0;
}

    স্থির PyInt
RangeLength(RangeObject *self)
{
    // HOW DO WE SIGNAL AN ERROR FROM THIS FUNCTION?
    যদি (CheckBuffer(self->buf))
	প্রত্যাগম -1; // ???

    প্রত্যাগম (self->end - self->start + 1);
}

    স্থির PyObject *
RangeItem(RangeObject *self, PyInt n)
{
    প্রত্যাগম RBItem(self->buf, n, self->start, self->end);
}

    স্থির PyObject *
RangeSlice(RangeObject *self, PyInt lo, PyInt hi)
{
    প্রত্যাগম RBSlice(self->buf, lo, hi, self->start, self->end);
}

স্থির বর্ণ *RangeAttrs[] = {
    "start", "end",
    রিক্ত
};

    স্থির PyObject *
RangeDir(PyObject *self, PyObject *args UNUSED)
{
    প্রত্যাগম ObjectDir(self, RangeAttrs);
}

    স্থির PyObject *
RangeAppend(RangeObject *self, PyObject *args)
{
    প্রত্যাগম RBAppend(self->buf, args, self->start, self->end, &self->end);
}

    স্থির PyObject *
RangeRepr(PyObject *self_obj)
{
    RangeObject *self = (RangeObject*)self_obj;
    যদি (self->buf->buf == INVALID_BUFFER_মান)
	প্রত্যাগম PyString_FromFormat("<range object (for deleted buffer) at %p>",
				    (শূন্য *)self);
    অন্যথা
    {
	বর্ণ *name = (বর্ণ *)self->buf->buf->b_fname;

	যদি (name == রিক্ত)
	    name = "";

	প্রত্যাগম PyString_FromFormat("<range %s (%d:%d)>",
				    name, (পূর্ণাঙ্ক)self->start, (পূর্ণাঙ্ক)self->end);
    }
}

স্থির সংরচনা PyMethodDef RangeMethods[] = {
    // name,	function,			calling,	documentation
    {"append",	(PyCFunction)RangeAppend,	METH_VARARGS,	"Append data to the Vim range" },
    {"__dir__",	(PyCFunction)RangeDir,		METH_NOARGS,	""},
    { রিক্ত,	রিক্ত,				0,		রিক্ত}
};

DEFINE_PY_TYPE_OBJECT(BufferType);
স্থির PySequenceMethods BufferAsSeq;
স্থির PyMappingMethods BufferAsMapping;

    স্থির PyObject *
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

    যদি (BUF_PYTHON_REF(buf) != রিক্ত)
    {
	self = BUF_PYTHON_REF(buf);
	Py_INCREF((PyObject *)self);
    }
    অন্যথা
    {
	self = PyObject_NEW(BufferObject, BufferTypePtr);
	যদি (self == রিক্ত)
	    প্রত্যাগম রিক্ত;
	self->buf = buf;
	BUF_PYTHON_REF(buf) = self;
    }

    প্রত্যাগম (PyObject *)(self);
}

    স্থির শূন্য
BufferDestructor(PyObject *self_obj)
{
    BufferObject *self = (BufferObject*)self_obj;
    যদি (self->buf && self->buf != INVALID_BUFFER_মান)
	BUF_PYTHON_REF(self->buf) = রিক্ত;

    DESTRUCTOR_FINISH(self);
}

    স্থির PyInt
BufferLength(BufferObject *self)
{
    // HOW DO WE SIGNAL AN ERROR FROM THIS FUNCTION?
    যদি (CheckBuffer(self))
	প্রত্যাগম -1; // ???

    প্রত্যাগম (PyInt)(self->buf->b_ml.ml_সারি_গুনতি);
}

    স্থির PyObject *
BufferItem(BufferObject *self, PyInt n)
{
    প্রত্যাগম RBItem(self, n, 1, -1);
}

    স্থির PyObject *
BufferSlice(BufferObject *self, PyInt lo, PyInt hi)
{
    প্রত্যাগম RBSlice(self, lo, hi, 1, -1);
}

স্থির বর্ণ *BufferAttrs[] = {
    "name", "number", "vars", "options", "valid",
    রিক্ত
};

    স্থির PyObject *
BufferDir(PyObject *self, PyObject *args UNUSED)
{
    প্রত্যাগম ObjectDir(self, BufferAttrs);
}

    স্থির PyObject *
BufferAttrValid(BufferObject *self, বর্ণ *name)
{
    PyObject	*ret;

    যদি (strcmp(name, "valid") != 0)
	প্রত্যাগম রিক্ত;

    ret = ((self->buf == INVALID_BUFFER_মান) ? Py_False : Py_True);
    Py_INCREF(ret);
    প্রত্যাগম ret;
}

    স্থির PyObject *
BufferAttr(BufferObject *self, বর্ণ *name)
{
    যদি (strcmp(name, "name") == 0)
	প্রত্যাগম PyString_FromString((self->buf->b_ffname == রিক্ত
				    ? "" : (বর্ণ *)self->buf->b_ffname));
    অন্যথা যদি (strcmp(name, "number") == 0)
	প্রত্যাগম Py_Build_মান(Py_ssize_t_fmt, self->buf->b_fnum);
    অন্যথা যদি (strcmp(name, "vars") == 0)
	প্রত্যাগম NEW_DICTIONARY(self->buf->b_vars);
    অন্যথা যদি (strcmp(name, "options") == 0)
	প্রত্যাগম OptionsNew(SREQ_BUF, self->buf, CheckBufferCb,
			(PyObject *) self);
    অন্যথা যদি (strcmp(name, "__members__") == 0)
	প্রত্যাগম ObjectDir(রিক্ত, BufferAttrs);
    অন্যথা
	প্রত্যাগম রিক্ত;
}

    স্থির পূর্ণাঙ্ক
BufferSetattr(PyObject *self_obj, বর্ণ *name, PyObject *valObject)
{
    BufferObject *self = (BufferObject*)self_obj;
    যদি (CheckBuffer(self))
	প্রত্যাগম -1;

    যদি (strcmp(name, "name") == 0)
    {
	char_u		*val;
	aco_save_T	aco;
	পূর্ণাঙ্ক		ren_ret = OK;
	PyObject	*todecref;

	যদি (!(val = StringToChars(valObject, &todecref)))
	    প্রত্যাগম -1;

	VimTryStart();
	// Using aucmd_*: autocommands will be executed by rename_buffer
	aucmd_prepbuf(&aco, self->buf);
	যদি (curbuf == self->buf)
	{
	    ren_ret = rename_buffer(val);
	    aucmd_restbuf(&aco);
	}
	Py_XDECREF(todecref);
	যদি (VimTryEnd())
	    প্রত্যাগম -1;

	যদি (ren_ret == FAIL)
	{
	    PyErr_SET_VIM(N_("failed to rename buffer"));
	    প্রত্যাগম -1;
	}
	প্রত্যাগম 0;
    }
    অন্যথা
    {
	PyErr_SetString(PyExc_AttributeError, name);
	প্রত্যাগম -1;
    }
}

    স্থির PyObject *
BufferAppend(BufferObject *self, PyObject *args)
{
    প্রত্যাগম RBAppend(self, args, 1, -1, রিক্ত);
}

    স্থির PyObject *
BufferMark(BufferObject *self, PyObject *pmarkObject)
{
    pos_T	*posp;
    char_u	*pmark;
    char_u	mark;
    bufref_T	savebuf;
    PyObject	*todecref;

    যদি (CheckBuffer(self))
	প্রত্যাগম রিক্ত;

    যদি (!(pmark = StringToChars(pmarkObject, &todecref)))
	প্রত্যাগম রিক্ত;

    যদি (pmark[0] == '\0' || pmark[1] != '\0')
    {
	PyErr_SET_STRING(Py_Exc_মান_Error,
		N_("mark name must be a single character"));
	Py_XDECREF(todecref);
	প্রত্যাগম রিক্ত;
    }

    mark = *pmark;

    Py_XDECREF(todecref);

    VimTryStart();
    switch_buffer(&savebuf, self->buf);
    posp = getmark(mark, FALSE);
    restore_buffer(&savebuf);
    যদি (VimTryEnd())
	প্রত্যাগম রিক্ত;

    যদি (posp == রিক্ত)
    {
	PyErr_SET_VIM(N_("invalid mark name"));
	প্রত্যাগম রিক্ত;
    }

    যদি (posp->lnum <= 0)
    {
	// Or raise an error?
	Py_INCREF(Py_None);
	প্রত্যাগম Py_None;
    }

    প্রত্যাগম Py_Build_মান("(ll)", (long)(posp->lnum), (long)(posp->col));
}

    স্থির PyObject *
BufferRange(BufferObject *self, PyObject *args)
{
    PyInt start;
    PyInt end;

    যদি (CheckBuffer(self))
	প্রত্যাগম রিক্ত;

    যদি (!PyArg_ParseTuple(args, "nn", &start, &end))
	প্রত্যাগম রিক্ত;

    প্রত্যাগম RangeNew(self->buf, start, end);
}

    স্থির PyObject *
BufferRepr(PyObject *self_obj)
{
    BufferObject *self = (BufferObject*)self_obj;
    যদি (self->buf == INVALID_BUFFER_মান)
	প্রত্যাগম PyString_FromFormat("<buffer object (deleted) at %p>", (শূন্য *)self);
    অন্যথা
    {
	বর্ণ	*name = (বর্ণ *)self->buf->b_fname;

	যদি (name == রিক্ত)
	    name = "";

	প্রত্যাগম PyString_FromFormat("<buffer %s>", name);
    }
}

স্থির সংরচনা PyMethodDef BufferMethods[] = {
    // name,	    function,			calling,	documentation
    {"append",	    (PyCFunction)BufferAppend,	METH_VARARGS,	"Append data to Vim buffer" },
    {"mark",	    (PyCFunction)BufferMark,	METH_O,		"Return (row,col) representing position of named mark" },
    {"range",	    (PyCFunction)BufferRange,	METH_VARARGS,	"Return a range object which represents the part of the given buffer between line numbers s and e" },
    {"__dir__",	    (PyCFunction)BufferDir,	METH_NOARGS,	""},
    { রিক্ত,	    রিক্ত,			0,		রিক্ত}
};

/*
 * Buffer list object - Implementation
 */

DEFINE_PY_TYPE_OBJECT(BufMapType);

প্রকারনির্দেশ সংরচনা
{
    PyObject_HEAD
} BufMapObject;

স্থির BufMapObject TheBufferMap =
{
    PyObject_HEAD_INIT_TYPE(BufMapType)
};

    স্থির PyInt
BufMapLength(PyObject *self UNUSED)
{
    buf_T	*b = firstbuf;
    PyInt	n = 0;

    যাবত্ (b)
    {
	++n;
	b = b->b_next;
    }

    প্রত্যাগম n;
}

    স্থির PyObject *
BufMapItem(PyObject *self UNUSED, PyObject *চাবি_Object)
{
    buf_T	*b;
    long	bnr;

    যদি (সংখ্যা_To_Long(চাবি_Object, &bnr, সংখ্যা_INT|সংখ্যা_NATURAL))
	প্রত্যাগম রিক্ত;

    b = buflist_findnr((পূর্ণাঙ্ক) bnr);

    যদি (b)
	প্রত্যাগম BufferNew(b);
    অন্যথা
    {
	PyErr_SetObject(Py_Exc_চাবি_Error, চাবি_Object);
	প্রত্যাগম রিক্ত;
    }
}

    স্থির শূন্য
BufMapIterDestruct(শূন্য* arg)
{
    PyObject *buffer = (PyObject*)arg;
    // Iteration was stopped before all buffers were processed
    যদি (buffer)
    {
	Py_DECREF(buffer);
    }
}

    স্থির পূর্ণাঙ্ক
BufMapIterTraverse(শূন্য *iter, visitproc visit, শূন্য *arg)
{
    PyObject *buffer = (PyObject*)iter;
    যদি (buffer)
	Py_VISIT(buffer);
    প্রত্যাগম 0;
}

    স্থির পূর্ণাঙ্ক
BufMapIterClear(শূন্য **iter)
{
    PyObject **buffer = (PyObject**)iter;
    যদি (*buffer)
	Py_CLEAR(*buffer);
    প্রত্যাগম 0;
}

    স্থির PyObject *
BufMapIterNext(শূন্য **arg)
{
    PyObject	*next;
    PyObject	*ret;
    PyObject	**buffer = (PyObject**)arg;

    যদি (!*buffer)
	প্রত্যাগম রিক্ত;

    ret = *buffer;

    যদি (CheckBuffer((BufferObject *)(ret)))
    {
	*buffer = রিক্ত;
	প্রত্যাগম রিক্ত;
    }

    যদি (!((BufferObject *)(ret))->buf->b_next)
	next = রিক্ত;
    অন্যথা যদি (!(next = BufferNew(((BufferObject *)(ret))->buf->b_next)))
	প্রত্যাগম রিক্ত;
    *buffer = next;
    // Do not increment reference: we no longer hold it (decref), but whoever
    // on other side will hold (incref). Decref+incref = nothing.
    প্রত্যাগম ret;
}

    স্থির PyObject *
BufMapIter(PyObject *self)
{
    PyObject *buffer;

    buffer = BufferNew(firstbuf);
    প্রত্যাগম IterNew(buffer,
	    BufMapIterDestruct, BufMapIterNext,
	    BufMapIterTraverse, BufMapIterClear,
	    self);
}

স্থির PyMappingMethods BufMapAsMapping = {
    (lenfunc)       BufMapLength,
    (binaryfunc)    BufMapItem,
    (objobjargproc) 0,
};

// Current items object

স্থির বর্ণ *CurrentAttrs[] = {
    "buffer", "window", "line", "range", "tabpage",
    রিক্ত
};

    স্থির PyObject *
CurrentDir(PyObject *self, PyObject *args UNUSED)
{
    প্রত্যাগম ObjectDir(self, CurrentAttrs);
}

    স্থির PyObject *
CurrentGetattr(PyObject *self UNUSED, বর্ণ *name)
{
    যদি (strcmp(name, "buffer") == 0)
	প্রত্যাগম (PyObject *)BufferNew(curbuf);
    অন্যথা যদি (strcmp(name, "window") == 0)
	প্রত্যাগম (PyObject *)WindowNew(curwin, curtab);
    অন্যথা যদি (strcmp(name, "tabpage") == 0)
	প্রত্যাগম (PyObject *)TabPageNew(curtab);
    অন্যথা যদি (strcmp(name, "line") == 0)
	প্রত্যাগম Get_Buffer_সারি(curbuf, (PyInt)curwin->w_cursor.lnum);
    অন্যথা যদি (strcmp(name, "range") == 0)
	প্রত্যাগম RangeNew(curbuf, RangeStart, RangeEnd);
    অন্যথা যদি (strcmp(name, "__members__") == 0)
	প্রত্যাগম ObjectDir(রিক্ত, CurrentAttrs);
    অন্যথা
#যদি PY_MAJOR_VERSION < 3
	প্রত্যাগম Py_FindMethod(WindowMethods, self, name);
#অন্যথা
	প্রত্যাগম রিক্ত;
#endif
}

    স্থির পূর্ণাঙ্ক
CurrentSetattr(PyObject *self UNUSED, বর্ণ *name, PyObject *valObject)
{
    যদি (strcmp(name, "line") == 0)
    {
	যদি (Set_Buffer_সারি(curbuf, (PyInt)curwin->w_cursor.lnum, valObject,
			  রিক্ত) == FAIL)
	    প্রত্যাগম -1;

	প্রত্যাগম 0;
    }
    অন্যথা যদি (strcmp(name, "buffer") == 0)
    {
	পূর্ণাঙ্ক গুনতি;

	যদি (valObject->ob_type != BufferTypePtr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.Buffer object, but got %s"),
		    valObject);
	    প্রত্যাগম -1;
	}

	যদি (CheckBuffer((BufferObject *)(valObject)))
	    প্রত্যাগম -1;
	গুনতি = ((BufferObject *)(valObject))->buf->b_fnum;

	VimTryStart();
	যদি (do_buffer(DOBUF_GOTO, DOBUF_FIRST, FORWARD, গুনতি, 0) == FAIL)
	{
	    যদি (VimTryEnd())
		প্রত্যাগম -1;
	    PyErr_VIM_FORMAT(N_("failed to switch to buffer %d"), গুনতি);
	    প্রত্যাগম -1;
	}

	প্রত্যাগম VimTryEnd();
    }
    অন্যথা যদি (strcmp(name, "window") == 0)
    {
	পূর্ণাঙ্ক গুনতি;

	যদি (valObject->ob_type != WindowTypePtr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.Window object, but got %s"),
		    valObject);
	    প্রত্যাগম -1;
	}

	যদি (CheckWindow((WindowObject *)(valObject)))
	    প্রত্যাগম -1;
	গুনতি = get_win_সংখ্যা(((WindowObject *)(valObject))->win, firstwin);

	যদি (!গুনতি)
	{
	    PyErr_SET_STRING(Py_Exc_মান_Error,
		    N_("failed to find window in the current tab page"));
	    প্রত্যাগম -1;
	}

	VimTryStart();
	win_goto(((WindowObject *)(valObject))->win);
	যদি (((WindowObject *)(valObject))->win != curwin)
	{
	    যদি (VimTryEnd())
		প্রত্যাগম -1;
	    PyErr_SET_STRING(PyExc_RuntimeError,
		    N_("did not switch to the specified window"));
	    প্রত্যাগম -1;
	}

	প্রত্যাগম VimTryEnd();
    }
    অন্যথা যদি (strcmp(name, "tabpage") == 0)
    {
	যদি (valObject->ob_type != TabPageTypePtr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.TabPage object, but got %s"),
		    valObject);
	    প্রত্যাগম -1;
	}

	যদি (CheckTabPage((TabPageObject *)(valObject)))
	    প্রত্যাগম -1;

	VimTryStart();
	goto_tabpage_tp(((TabPageObject *)(valObject))->tab, TRUE, TRUE);
	যদি (((TabPageObject *)(valObject))->tab != curtab)
	{
	    যদি (VimTryEnd())
		প্রত্যাগম -1;
	    PyErr_SET_STRING(PyExc_RuntimeError,
		    N_("did not switch to the specified tab page"));
	    প্রত্যাগম -1;
	}

	প্রত্যাগম VimTryEnd();
    }
    অন্যথা
    {
	PyErr_SetString(PyExc_AttributeError, name);
	প্রত্যাগম -1;
    }
}

স্থির সংরচনা PyMethodDef CurrentMethods[] = {
    // name,	    function,			calling,	documentation
    {"__dir__",	    (PyCFunction)CurrentDir,	METH_NOARGS,	""},
    { রিক্ত,	    রিক্ত,			0,		রিক্ত}
};

    স্থির শূন্য
init_range_cmd(শূন্য *arg)
{
    exarg_T *eap = (exarg_T*)arg;
    RangeStart = eap->line1;
    RangeEnd = eap->line2;
}

    স্থির শূন্য
init_range_eval(শূন্য *rettv UNUSED)
{
    RangeStart = (PyInt) curwin->w_cursor.lnum;
    RangeEnd = RangeStart;
}

    স্থির শূন্য
run_cmd(ধ্রুব বর্ণ *cmd, dict_T* locals UNUSED, শূন্য *arg UNUSED
#ifdef PY_CAN_RECURSE
	, PyGILState_STATE *pygilstate UNUSED
#endif
	)
{
    PyObject	*run_ret;
    run_ret = PyRun_String((বর্ণ *)cmd, Py_file_প্রবেশ, globals, globals);
    যদি (run_ret != রিক্ত)
    {
	Py_DECREF(run_ret);
    }
    অন্যথা যদি (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
    {
	emsg(_(e_cant_handle_systemexit_of_python_exception_in_vim));
	PyErr_Clear();
    }
    অন্যথা
	PyErr_PrintEx(1);
}

স্থির ধ্রুব বর্ণ	*code_hdr = "def " DOPY_FUNC "(line, linenr):\n ";
স্থির পূর্ণাঙ্ক		code_hdr_len = 30;

    স্থির শূন্য
run_do(ধ্রুব বর্ণ *cmd, dict_T* locals UNUSED, শূন্য *arg UNUSED
#ifdef PY_CAN_RECURSE
	, PyGILState_STATE *pygilstate
#endif
	)
{
    PyInt	lnum;
    size_t	len;
    বর্ণ	*code;
    পূর্ণাঙ্ক		status;
    PyObject	*pyfunc, *pymain;
    PyObject	*run_ret;
    buf_T	*was_curbuf = curbuf;

    যদি (u_save((linenr_T)RangeStart - 1, (linenr_T)RangeEnd + 1) != OK)
    {
	emsg(_("cannot save undo information"));
	প্রত্যাগম;
    }

    len = code_hdr_len + STRLEN(cmd);
    code = PyMem_New(বর্ণ, len + 1);
    memcpy(code, code_hdr, code_hdr_len);
    STRCPY(code + code_hdr_len, cmd);
    run_ret = PyRun_String(code, Py_file_প্রবেশ, globals, globals);
    status = -1;
    যদি (run_ret != রিক্ত)
    {
	status = 0;
	Py_DECREF(run_ret);
    }
    অন্যথা যদি (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
    {
	PyMem_Free(code);
	emsg(_(e_cant_handle_systemexit_of_python_exception_in_vim));
	PyErr_Clear();
	প্রত্যাগম;
    }
    অন্যথা
	PyErr_PrintEx(1);

    PyMem_Free(code);

    যদি (status)
    {
	emsg(_("failed to run the code"));
	প্রত্যাগম;
    }

    status = 0;
    pymain = PyImport_AddModule("__main__");
    pyfunc = PyObject_GetAttrString(pymain, DOPY_FUNC);
#ifdef PY_CAN_RECURSE
    PyGILState_Release(*pygilstate);
#endif

    প্রতি (lnum = RangeStart; lnum <= RangeEnd; ++lnum)
    {
	PyObject	*সারি;
	PyObject	*linenr;
	PyObject	*ret;

#ifdef PY_CAN_RECURSE
	*pygilstate = PyGILState_Ensure();
#endif
	// Check the line number, the command my have deleted lines.
	যদি (lnum > curbuf->b_ml.ml_সারি_গুনতি
		|| !(সারি = Get_Buffer_সারি(curbuf, lnum)))
	    গচ্ছ err;
	যদি (!(linenr = PyInt_FromLong((long) lnum)))
	{
	    Py_DECREF(সারি);
	    গচ্ছ err;
	}
	ret = PyObject_CallFunctionObjArgs(pyfunc, সারি, linenr, রিক্ত);
	Py_DECREF(সারি);
	Py_DECREF(linenr);
	যদি (!ret)
	    গচ্ছ err;

	// Check that the command didn't switch to another buffer.
	// Check the line number, the command my have deleted lines.
	যদি (curbuf != was_curbuf || lnum > curbuf->b_ml.ml_সারি_গুনতি)
	{
	    Py_XDECREF(ret);
	    গচ্ছ err;
	}

	যদি (ret != Py_None)
	    যদি (Set_Buffer_সারি(curbuf, lnum, ret, রিক্ত) == FAIL)
	    {
		Py_XDECREF(ret);
		গচ্ছ err;
	    }

	Py_XDECREF(ret);
	PythonIO_Flush();
#ifdef PY_CAN_RECURSE
	PyGILState_Release(*pygilstate);
#endif
    }
    গচ্ছ out;
err:
#ifdef PY_CAN_RECURSE
    *pygilstate = PyGILState_Ensure();
#endif
    PyErr_PrintEx(0);
    PythonIO_Flush();
    status = 1;
out:
#ifdef PY_CAN_RECURSE
    যদি (!status)
	*pygilstate = PyGILState_Ensure();
#endif
    Py_DECREF(pyfunc);
    PyObject_SetAttrString(pymain, DOPY_FUNC, রিক্ত);
    যদি (status)
	প্রত্যাগম;
    check_cursor();
    update_curbuf(UPD_NOT_VALID);
}

    স্থির শূন্য
run_eval(ধ্রুব বর্ণ *cmd, dict_T *locals, শূন্য *arg
#ifdef PY_CAN_RECURSE
	, PyGILState_STATE *pygilstate UNUSED
#endif
	)
{
    PyObject	*run_ret;
    typval_T	*rettv = (typval_T*)arg;
    PyObject	*pylocals = locals ? NEW_DICTIONARY(locals) : globals;

    run_ret = PyRun_String((বর্ণ *)cmd, Py_eval_প্রবেশ, globals, pylocals);
    যদি (run_ret == রিক্ত)
    {
	যদি (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
	{
	    emsg(_(e_cant_handle_systemexit_of_python_exception_in_vim));
	    PyErr_Clear();
	}
	অন্যথা
	{
	    যদি (PyErr_Occurred() && !বার্তা_silent)
		PyErr_PrintEx(0);
	    emsg(_(e_eval_did_not_return_valid_python_object));
	}
    }
    অন্যথা
    {
	যদি (ConvertFromPyObject(run_ret, rettv) == -1)
	    emsg(_(e_failed_to_convert_returned_python_object_to_vim_মান));
	Py_DECREF(run_ret);
    }
    PyErr_Clear();
}

    স্থির পূর্ণাঙ্ক
set_ref_in_py(ধ্রুব পূর্ণাঙ্ক copyID)
{
    pylinkedlist_T	*cur;
    list_T		*ll;
    tuple_T		*tt;
    পূর্ণাঙ্ক			i;
    পূর্ণাঙ্ক			abort = FALSE;
    FunctionObject	*func;

    যদি (lastdict != রিক্ত)
    {
	প্রতি (cur = lastdict ; !abort && cur != রিক্ত ; cur = cur->pll_prev)
	    abort = set_ref_in_dict(((DictionaryObject *)(cur->pll_obj))->dict,
								       copyID);
    }

    যদি (lastlist != রিক্ত)
    {
	প্রতি (cur = lastlist ; !abort && cur != রিক্ত ; cur = cur->pll_prev)
	{
	    ll = ((ListObject *) (cur->pll_obj))->list;
	    abort = set_ref_in_list(ll, copyID);
	}
    }

    যদি (lasttuple != রিক্ত)
    {
	প্রতি (cur = lasttuple ; !abort && cur != রিক্ত ; cur = cur->pll_prev)
	{
	    tt = ((TupleObject *) (cur->pll_obj))->tuple;
	    abort = set_ref_in_tuple(tt, copyID);
	}
    }

    যদি (lastfunc != রিক্ত)
    {
	প্রতি (cur = lastfunc ; !abort && cur != রিক্ত ; cur = cur->pll_prev)
	{
	    func = (FunctionObject *) cur->pll_obj;
	    abort = set_ref_in_dict(func->self, copyID);
	    যদি (func->argc)
		প্রতি (i = 0; !abort && i < func->argc; ++i)
		    abort = abort
			|| set_ref_in_item(&func->argv[i], copyID, রিক্ত, রিক্ত, রিক্ত);
	}
    }

    প্রত্যাগম abort;
}

    স্থির পূর্ণাঙ্ক
set_string_copy(char_u *str, typval_T *tv)
{
    tv->vval.v_string = vim_strsave(str);
    যদি (tv->vval.v_string == রিক্ত)
    {
	PyErr_NoMemory();
	প্রত্যাগম -1;
    }
    প্রত্যাগম 0;
}

    স্থির পূর্ণাঙ্ক
pydict_to_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    dict_T	*dict;
    char_u	*চাবি;
    dictitem_T	*di;
    PyObject	*চাবি_Object;
    PyObject	*valObject;
    Py_ssize_t	iter = 0;

    যদি (!(dict = py_dict_alloc()))
	প্রত্যাগম -1;

    tv->v_type = VAR_DICT;
    tv->vval.v_dict = dict;

    যাবত্ (PyDict_Next(obj, &iter, &চাবি_Object, &valObject))
    {
	PyObject	*todecref = রিক্ত;

	যদি (চাবি_Object == রিক্ত || valObject == রিক্ত)
	{
	    dict_unref(dict);
	    প্রত্যাগম -1;
	}

	যদি (!(চাবি = StringToChars(চাবি_Object, &todecref)))
	{
	    dict_unref(dict);
	    প্রত্যাগম -1;
	}

	যদি (*চাবি == NUL)
	{
	    dict_unref(dict);
	    Py_XDECREF(todecref);
	    RAISE_NO_EMPTY_চাবিগুলি;
	    প্রত্যাগম -1;
	}

	di = dictitem_alloc(চাবি);

	Py_XDECREF(todecref);

	যদি (di == রিক্ত)
	{
	    PyErr_NoMemory();
	    dict_unref(dict);
	    প্রত্যাগম -1;
	}

	যদি (_ConvertFromPyObject(valObject, &di->di_tv, lookup_dict) == -1)
	{
	    vim_free(di);
	    dict_unref(dict);
	    প্রত্যাগম -1;
	}

	যদি (dict_add(dict, di) == FAIL)
	{
	    RAISE_চাবি_ADD_FAIL(di->di_চাবি);
	    clear_tv(&di->di_tv);
	    vim_free(di);
	    dict_unref(dict);
	    প্রত্যাগম -1;
	}
    }

    --dict->dv_refcount;
    প্রত্যাগম 0;
}

    স্থির পূর্ণাঙ্ক
pymap_to_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    dict_T	*dict;
    char_u	*চাবি;
    dictitem_T	*di;
    PyObject	*list;
    PyObject	*iterator;
    PyObject	*চাবি_Object;
    PyObject	*valObject;

    যদি (!(dict = py_dict_alloc()))
	প্রত্যাগম -1;

    tv->v_type = VAR_DICT;
    tv->vval.v_dict = dict;

    যদি (!(list = Py_Mapping_চাবিগুলি(obj)))
    {
	dict_unref(dict);
	প্রত্যাগম -1;
    }

    যদি (!(iterator = PyObject_GetIter(list)))
    {
	dict_unref(dict);
	Py_DECREF(list);
	প্রত্যাগম -1;
    }
    Py_DECREF(list);

    যাবত্ ((চাবি_Object = PyIter_Next(iterator)))
    {
	PyObject	*todecref;

	যদি (!(চাবি = StringToChars(চাবি_Object, &todecref)))
	{
	    Py_DECREF(চাবি_Object);
	    Py_DECREF(iterator);
	    dict_unref(dict);
	    প্রত্যাগম -1;
	}

	যদি (*চাবি == NUL)
	{
	    Py_DECREF(চাবি_Object);
	    Py_DECREF(iterator);
	    Py_XDECREF(todecref);
	    dict_unref(dict);
	    RAISE_NO_EMPTY_চাবিগুলি;
	    প্রত্যাগম -1;
	}

	যদি (!(valObject = PyObject_GetItem(obj, চাবি_Object)))
	{
	    Py_DECREF(চাবি_Object);
	    Py_DECREF(iterator);
	    Py_XDECREF(todecref);
	    dict_unref(dict);
	    প্রত্যাগম -1;
	}

	di = dictitem_alloc(চাবি);

	Py_DECREF(চাবি_Object);
	Py_XDECREF(todecref);

	যদি (di == রিক্ত)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(valObject);
	    dict_unref(dict);
	    PyErr_NoMemory();
	    প্রত্যাগম -1;
	}

	যদি (_ConvertFromPyObject(valObject, &di->di_tv, lookup_dict) == -1)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(valObject);
	    vim_free(di);
	    dict_unref(dict);
	    প্রত্যাগম -1;
	}

	Py_DECREF(valObject);

	যদি (dict_add(dict, di) == FAIL)
	{
	    RAISE_চাবি_ADD_FAIL(di->di_চাবি);
	    Py_DECREF(iterator);
	    dictitem_free(di);
	    dict_unref(dict);
	    প্রত্যাগম -1;
	}
    }
    Py_DECREF(iterator);
    --dict->dv_refcount;
    প্রত্যাগম 0;
}

    স্থির পূর্ণাঙ্ক
pyseq_to_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    list_T	*l;

    যদি (!(l = py_list_alloc()))
	প্রত্যাগম -1;

    tv->v_type = VAR_LIST;
    tv->vval.v_list = l;

    যদি (list_py_concat(l, obj, lookup_dict) == -1)
    {
	list_unref(l);
	প্রত্যাগম -1;
    }

    --l->lv_refcount;
    প্রত্যাগম 0;
}

    স্থির পূর্ণাঙ্ক
pytuple_to_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    tuple_T	*t;

    যদি (!(t = py_tuple_alloc()))
	প্রত্যাগম -1;

    tv->v_type = VAR_TUPLE;
    tv->vval.v_tuple = t;

    যদি (tuple_py_concat(t, obj, lookup_dict) == -1)
    {
	tuple_unref(t);
	প্রত্যাগম -1;
    }

    --t->tv_refcount;
    প্রত্যাগম 0;
}

প্রকারনির্দেশ পূর্ণাঙ্ক (*pytotvfunc)(PyObject *, typval_T *, PyObject *);

    স্থির পূর্ণাঙ্ক
convert_dl(PyObject *obj, typval_T *tv,
				    pytotvfunc py_to_tv, PyObject *lookup_dict)
{
    PyObject	*capsule;
    বর্ণ	hexBuf[পরিমাণ(শূন্য *) * 2 + 3];

    sprintf(hexBuf, "%p", (শূন্য *)obj);

#ifdef PY_USE_CAPSULE
    capsule = PyDict_GetItemString(lookup_dict, hexBuf);
#অন্যথা
    capsule = (PyObject *)PyDict_GetItemString(lookup_dict, hexBuf);
#endif
    যদি (capsule == রিক্ত)
    {
#ifdef PY_USE_CAPSULE
	capsule = PyCapsule_New(tv, রিক্ত, রিক্ত);
#অন্যথা
	capsule = PyCObject_FromVoidPtr(tv, রিক্ত);
#endif
	যদি (PyDict_SetItemString(lookup_dict, hexBuf, capsule))
	{
	    Py_DECREF(capsule);
	    tv->v_type = VAR_UNKNOWN;
	    প্রত্যাগম -1;
	}

	Py_DECREF(capsule);

	যদি (py_to_tv(obj, tv, lookup_dict) == -1)
	{
	    tv->v_type = VAR_UNKNOWN;
	    প্রত্যাগম -1;
	}
	// As we are not using copy_tv which increments reference count we must
	// do it ourself.
	যদি (tv->v_type == VAR_DICT)
	    ++tv->vval.v_dict->dv_refcount;
	অন্যথা যদি (tv->v_type == VAR_LIST)
	    ++tv->vval.v_list->lv_refcount;
	অন্যথা যদি (tv->v_type == VAR_TUPLE)
	    ++tv->vval.v_tuple->tv_refcount;
    }
    অন্যথা
    {
	typval_T	*v;

#ifdef PY_USE_CAPSULE
	v = PyCapsule_GetPointer(capsule, রিক্ত);
#অন্যথা
	v = PyCObject_AsVoidPtr(capsule);
#endif
	copy_tv(v, tv);
    }
    প্রত্যাগম 0;
}

    স্থির পূর্ণাঙ্ক
ConvertFromPyMapping(PyObject *obj, typval_T *tv)
{
    PyObject	*lookup_dict;
    পূর্ণাঙ্ক		ret;

    যদি (!(lookup_dict = PyDict_New()))
	প্রত্যাগম -1;

    যদি (PyType_IsSubtype(obj->ob_type, DictionaryTypePtr))
    {
	tv->v_type = VAR_DICT;
	tv->vval.v_dict = (((DictionaryObject *)(obj))->dict);
	++tv->vval.v_dict->dv_refcount;
	ret = 0;
    }
    অন্যথা যদি (PyDict_Check(obj))
	ret = convert_dl(obj, tv, pydict_to_tv, lookup_dict);
    অন্যথা যদি (PyMapping_Check(obj))
	ret = convert_dl(obj, tv, pymap_to_tv, lookup_dict);
    অন্যথা
    {
	PyErr_FORMAT_TYPE(
		N_("unable to convert %s to a Vim dictionary"),
		obj);
	ret = -1;
    }
    Py_DECREF(lookup_dict);
    প্রত্যাগম ret;
}

    স্থির পূর্ণাঙ্ক
ConvertFromPySequence(PyObject *obj, typval_T *tv)
{
    PyObject	*lookup_dict;
    পূর্ণাঙ্ক		ret;

    যদি (!(lookup_dict = PyDict_New()))
	প্রত্যাগম -1;

    যদি (PyType_IsSubtype(obj->ob_type, ListTypePtr))
    {
	tv->v_type = VAR_LIST;
	tv->vval.v_list = (((ListObject *)(obj))->list);
	++tv->vval.v_list->lv_refcount;
	ret = 0;
    }
    অন্যথা যদি (PyIter_Check(obj) || PySequence_Check(obj))
	ret = convert_dl(obj, tv, pyseq_to_tv, lookup_dict);
    অন্যথা
    {
	PyErr_FORMAT_TYPE(
		N_("unable to convert %s to a Vim list"),
		obj);
	ret = -1;
    }
    Py_DECREF(lookup_dict);
    প্রত্যাগম ret;
}

    স্থির পূর্ণাঙ্ক
ConvertFromPyObject(PyObject *obj, typval_T *tv)
{
    PyObject	*lookup_dict;
    পূর্ণাঙ্ক		ret;

    যদি (!(lookup_dict = PyDict_New()))
	প্রত্যাগম -1;
    ret = _ConvertFromPyObject(obj, tv, lookup_dict);
    Py_DECREF(lookup_dict);
    প্রত্যাগম ret;
}

    স্থির পূর্ণাঙ্ক
_ConvertFromPyObject(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    যদি (PyType_IsSubtype(obj->ob_type, DictionaryTypePtr))
    {
	tv->v_type = VAR_DICT;
	tv->vval.v_dict = (((DictionaryObject *)(obj))->dict);
	++tv->vval.v_dict->dv_refcount;
    }
    অন্যথা যদি (PyType_IsSubtype(obj->ob_type, ListTypePtr))
    {
	tv->v_type = VAR_LIST;
	tv->vval.v_list = (((ListObject *)(obj))->list);
	++tv->vval.v_list->lv_refcount;
    }
    অন্যথা যদি (PyType_IsSubtype(obj->ob_type, TupleTypePtr))
    {
	tv->v_type = VAR_TUPLE;
	tv->vval.v_tuple = (((TupleObject *)(obj))->tuple);
	++tv->vval.v_tuple->tv_refcount;
    }
    অন্যথা যদি (PyType_IsSubtype(obj->ob_type, FunctionTypePtr))
    {
	FunctionObject *func = (FunctionObject *) obj;
	যদি (func->self != রিক্ত || func->argv != রিক্ত)
	{
	    partial_T *pt = ALLOC_CLEAR_ONE(partial_T);

	    set_partial(func, pt, TRUE);
	    tv->vval.v_partial = pt;
	    tv->v_type = VAR_PARTIAL;
	}
	অন্যথা
	{
	    যদি (set_string_copy(func->name, tv) == -1)
		প্রত্যাগম -1;

	    tv->v_type = VAR_FUNC;
	}
	func_ref(func->name);
    }
    অন্যথা যদি (PyBytes_Check(obj))
    {
	char_u	*str;

	যদি (PyBytes_AsStringAndSize(obj, (বর্ণ **) &str, রিক্ত) == -1)
	    প্রত্যাগম -1;
	যদি (str == রিক্ত)
	    প্রত্যাগম -1;

	যদি (set_string_copy(str, tv) == -1)
	    প্রত্যাগম -1;

	tv->v_type = VAR_STRING;
    }
    অন্যথা যদি (PyUnicode_Check(obj))
    {
	PyObject	*bytes;
	char_u	*str;

	bytes = PyUnicode_AsEncodedString(obj, ENC_OPT, ERRORS_ENCODE_ARG);
	যদি (bytes == রিক্ত)
	    প্রত্যাগম -1;

	যদি (PyBytes_AsStringAndSize(bytes, (বর্ণ **) &str, রিক্ত) == -1)
	    প্রত্যাগম -1;
	যদি (str == রিক্ত)
	    প্রত্যাগম -1;

	যদি (set_string_copy(str, tv))
	{
	    Py_XDECREF(bytes);
	    প্রত্যাগম -1;
	}
	Py_XDECREF(bytes);

	tv->v_type = VAR_STRING;
    }
#যদি PY_MAJOR_VERSION < 3
    অন্যথা যদি (PyInt_Check(obj))
    {
	tv->v_type = VAR_সংখ্যা;
	tv->vval.v_সংখ্যা = (varnumber_T) PyInt_AsLong(obj);
	যদি (PyErr_Occurred())
	    প্রত্যাগম -1;
    }
#endif
    অন্যথা যদি (PyLong_Check(obj))
    {
	tv->v_type = VAR_সংখ্যা;
	tv->vval.v_সংখ্যা = (varnumber_T) PyLong_AsLong(obj);
	যদি (PyErr_Occurred())
	    প্রত্যাগম -1;
    }
    অন্যথা যদি (PyTuple_Check(obj))
	প্রত্যাগম convert_dl(obj, tv, pytuple_to_tv, lookup_dict);
    অন্যথা যদি (PyDict_Check(obj))
	প্রত্যাগম convert_dl(obj, tv, pydict_to_tv, lookup_dict);
    অন্যথা যদি (PyFloat_Check(obj))
    {
	tv->v_type = VAR_FLOAT;
	tv->vval.v_float = (float_T) PyFloat_AsDouble(obj);
    }
    অন্যথা যদি (PyObject_HasAttrString(obj, "keys"))
	প্রত্যাগম convert_dl(obj, tv, pymap_to_tv, lookup_dict);
    // PyObject_GetIter can create built-in iterator for any sequence object
    অন্যথা যদি (PyIter_Check(obj) || PySequence_Check(obj))
	প্রত্যাগম convert_dl(obj, tv, pyseq_to_tv, lookup_dict);
    অন্যথা যদি (PyMapping_Check(obj))
	প্রত্যাগম convert_dl(obj, tv, pymap_to_tv, lookup_dict);
    অন্যথা যদি (Py_সংখ্যা_Check(obj))
    {
	PyObject	*অঙ্ক;

	যদি (!(অঙ্ক = Py_সংখ্যা_Long(obj)))
	    প্রত্যাগম -1;

	tv->v_type = VAR_সংখ্যা;
	tv->vval.v_সংখ্যা = (varnumber_T) PyLong_AsLong(অঙ্ক);

	Py_DECREF(অঙ্ক);
    }
    অন্যথা যদি (obj == Py_None)
    {
	tv->v_type = VAR_SPECIAL;
	tv->vval.v_সংখ্যা = VVAL_NONE;
    }
    অন্যথা
    {
	PyErr_FORMAT_TYPE(
		N_("unable to convert %s to a Vim structure"),
		obj);
	প্রত্যাগম -1;
    }
    প্রত্যাগম 0;
}

    স্থির PyObject *
ConvertToPyObject(typval_T *tv)
{
    typval_T *argv;
    পূর্ণাঙ্ক i;
    যদি (tv == রিক্ত)
    {
	PyErr_SET_VIM(N_("internal error: NULL reference passed"));
	প্রত্যাগম রিক্ত;
    }
    বিকল্প (tv->v_type)
    {
	পক্ষ VAR_STRING:
	    প্রত্যাগম PyBytes_FromString(tv->vval.v_string == রিক্ত
					    ? "" : (বর্ণ *)tv->vval.v_string);
	পক্ষ VAR_সংখ্যা:
	    প্রত্যাগম PyLong_FromLong((long) tv->vval.v_সংখ্যা);
	পক্ষ VAR_FLOAT:
	    প্রত্যাগম PyFloat_FromDouble((দ্বিগুণ) tv->vval.v_float);
	পক্ষ VAR_LIST:
	    প্রত্যাগম NEW_LIST(tv->vval.v_list);
	পক্ষ VAR_TUPLE:
	    প্রত্যাগম NEW_TUPLE(tv->vval.v_tuple);
	পক্ষ VAR_DICT:
	    প্রত্যাগম NEW_DICTIONARY(tv->vval.v_dict);
	পক্ষ VAR_FUNC:
	    প্রত্যাগম NEW_FUNCTION(tv->vval.v_string == রিক্ত
					  ? (char_u *)"" : tv->vval.v_string,
					  0, রিক্ত, রিক্ত, TRUE);
	পক্ষ VAR_PARTIAL:
	    যদি (tv->vval.v_partial->pt_argc)
	    {
		argv = PyMem_New(typval_T, (size_t)tv->vval.v_partial->pt_argc);
		প্রতি (i = 0; i < tv->vval.v_partial->pt_argc; i++)
		    copy_tv(&tv->vval.v_partial->pt_argv[i], &argv[i]);
	    }
	    অন্যথা
		argv = রিক্ত;
	    যদি (tv->vval.v_partial->pt_dict != রিক্ত)
		tv->vval.v_partial->pt_dict->dv_refcount++;
	    প্রত্যাগম NEW_FUNCTION(tv->vval.v_partial == রিক্ত
			     ? (char_u *)"" : partial_name(tv->vval.v_partial),
				tv->vval.v_partial->pt_argc, argv,
				tv->vval.v_partial->pt_dict,
				tv->vval.v_partial->pt_auto);
	পক্ষ VAR_BLOB:
	    প্রত্যাগম PyBytes_FromStringAndSize(
		(বর্ণ*) tv->vval.v_blob->bv_ga.ga_data,
		(Py_ssize_t) tv->vval.v_blob->bv_ga.ga_len);
	পক্ষ VAR_UNKNOWN:
	পক্ষ VAR_ANY:
	পক্ষ VAR_VOID:
	পক্ষ VAR_CHANNEL:
	পক্ষ VAR_JOB:
	পক্ষ VAR_INSTR:
	পক্ষ VAR_CLASS:
	পক্ষ VAR_OBJECT:
	পক্ষ VAR_TYPEALIAS:
	    Py_INCREF(Py_None);
	    প্রত্যাগম Py_None;
	পক্ষ VAR_BOOL:
	পক্ষ VAR_SPECIAL:
	    বিকল্প (tv->vval.v_সংখ্যা)
	    {
		পক্ষ VVAL_FALSE: প্রত্যাগম ALWAYS_FALSE;
		পক্ষ VVAL_TRUE:  প্রত্যাগম ALWAYS_TRUE;
		পক্ষ VVAL_NONE:
		পক্ষ VVAL_NULL:  প্রত্যাগম ALWAYS_NONE;
	    }
	    PyErr_SET_VIM(N_("internal error: invalid value type"));
	    প্রত্যাগম রিক্ত;
    }
    প্রত্যাগম রিক্ত;
}

DEFINE_PY_TYPE_OBJECT(CurrentType);

প্রকারনির্দেশ সংরচনা
{
    PyObject_HEAD
} CurrentObject;

স্থির CurrentObject TheCurrent =
{
    PyObject_HEAD_INIT_TYPE(CurrentType)
};

    স্থির শূন্য
init_structs(শূন্য)
{
    CLEAR_FIELD(OutputType);
    OutputType.tp_name = "vim.message";
    OutputType.tp_basicsize = পরিমাণ(OutputObject);
    OutputType.tp_flags = Py_TPFLAGS_DEFAULT;
    OutputType.tp_doc = "vim message object";
    OutputType.tp_methods = OutputMethods;
#যদি PY_MAJOR_VERSION >= 3
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
#অন্যথা
    OutputType.tp_getattr = OutputGetattr;
    OutputType.tp_setattr = OutputSetattr;
    // Disabled, because this causes a crash in test86
    // OutputType.tp_base = &PyFile_Type;
#endif

    CLEAR_FIELD(IterType);
    IterType.tp_name = "vim.iter";
    IterType.tp_basicsize = পরিমাণ(IterObject);
    IterType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    IterType.tp_doc = "generic iterator object";
    IterType.tp_iter = IterIter;
    IterType.tp_iternext = IterNext;
    IterType.tp_dealloc = IterDestructor;
    IterType.tp_traverse = IterTraverse;
    IterType.tp_clear = IterClear;

    CLEAR_FIELD(BufferType);
    BufferType.tp_name = "vim.buffer";
    BufferType.tp_basicsize = পরিমাণ(BufferObject);
    BufferType.tp_dealloc = BufferDestructor;
    BufferType.tp_repr = BufferRepr;
    BufferType.tp_as_sequence = &BufferAsSeq;
    BufferType.tp_as_mapping = &BufferAsMapping;
    BufferType.tp_flags = Py_TPFLAGS_DEFAULT;
    BufferType.tp_doc = "vim buffer object";
    BufferType.tp_methods = BufferMethods;
#যদি PY_MAJOR_VERSION >= 3
    BufferType.tp_getattro = BufferGetattro;
    BufferType.tp_setattro = BufferSetattro;
    BufferType.tp_alloc = call_PyType_GenericAlloc;
    BufferType.tp_new = call_PyType_GenericNew;
    BufferType.tp_free = call_PyObject_Free;
#অন্যথা
    BufferType.tp_getattr = BufferGetattr;
    BufferType.tp_setattr = BufferSetattr;
#endif

    CLEAR_FIELD(WindowType);
    WindowType.tp_name = "vim.window";
    WindowType.tp_basicsize = পরিমাণ(WindowObject);
    WindowType.tp_dealloc = WindowDestructor;
    WindowType.tp_repr = WindowRepr;
    WindowType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    WindowType.tp_doc = "vim Window object";
    WindowType.tp_methods = WindowMethods;
    WindowType.tp_traverse = WindowTraverse;
    WindowType.tp_clear = WindowClear;
#যদি PY_MAJOR_VERSION >= 3
    WindowType.tp_getattro = WindowGetattro;
    WindowType.tp_setattro = WindowSetattro;
    WindowType.tp_alloc = call_PyType_GenericAlloc;
    WindowType.tp_new = call_PyType_GenericNew;
    WindowType.tp_free = call_PyObject_Free;
#অন্যথা
    WindowType.tp_getattr = WindowGetattr;
    WindowType.tp_setattr = WindowSetattr;
#endif

    CLEAR_FIELD(TabPageType);
    TabPageType.tp_name = "vim.tabpage";
    TabPageType.tp_basicsize = পরিমাণ(TabPageObject);
    TabPageType.tp_dealloc = TabPageDestructor;
    TabPageType.tp_repr = TabPageRepr;
    TabPageType.tp_flags = Py_TPFLAGS_DEFAULT;
    TabPageType.tp_doc = "vim tab page object";
    TabPageType.tp_methods = TabPageMethods;
#যদি PY_MAJOR_VERSION >= 3
    TabPageType.tp_getattro = TabPageGetattro;
    TabPageType.tp_alloc = call_PyType_GenericAlloc;
    TabPageType.tp_new = call_PyType_GenericNew;
    TabPageType.tp_free = call_PyObject_Free;
#অন্যথা
    TabPageType.tp_getattr = TabPageGetattr;
#endif

    CLEAR_FIELD(BufMapType);
    BufMapType.tp_name = "vim.bufferlist";
    BufMapType.tp_basicsize = পরিমাণ(BufMapObject);
    BufMapType.tp_as_mapping = &BufMapAsMapping;
    BufMapType.tp_flags = Py_TPFLAGS_DEFAULT;
    BufMapType.tp_iter = BufMapIter;
    BufMapType.tp_doc = "vim buffer list";

    CLEAR_FIELD(WinListType);
    WinListType.tp_name = "vim.windowlist";
    WinListType.tp_basicsize = পরিমাণ(WinListObject);
    WinListType.tp_as_sequence = &WinListAsSeq;
    WinListType.tp_flags = Py_TPFLAGS_DEFAULT;
    WinListType.tp_doc = "vim window list";
    WinListType.tp_dealloc = (destructor)WinListDestructor;

    CLEAR_FIELD(TabListType);
    TabListType.tp_name = "vim.tabpagelist";
    TabListType.tp_basicsize = পরিমাণ(TabListObject);
    TabListType.tp_as_sequence = &TabListAsSeq;
    TabListType.tp_flags = Py_TPFLAGS_DEFAULT;
    TabListType.tp_doc = "vim tab page list";

    CLEAR_FIELD(RangeType);
    RangeType.tp_name = "vim.range";
    RangeType.tp_basicsize = পরিমাণ(RangeObject);
    RangeType.tp_dealloc = RangeDestructor;
    RangeType.tp_repr = RangeRepr;
    RangeType.tp_as_sequence = &RangeAsSeq;
    RangeType.tp_as_mapping = &RangeAsMapping;
    RangeType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    RangeType.tp_doc = "vim Range object";
    RangeType.tp_methods = RangeMethods;
    RangeType.tp_traverse = RangeTraverse;
    RangeType.tp_clear = RangeClear;
#যদি PY_MAJOR_VERSION >= 3
    RangeType.tp_getattro = RangeGetattro;
    RangeType.tp_alloc = call_PyType_GenericAlloc;
    RangeType.tp_new = call_PyType_GenericNew;
    RangeType.tp_free = call_PyObject_Free;
#অন্যথা
    RangeType.tp_getattr = RangeGetattr;
#endif

    CLEAR_FIELD(CurrentType);
    CurrentType.tp_name = "vim.currentdata";
    CurrentType.tp_basicsize = পরিমাণ(CurrentObject);
    CurrentType.tp_flags = Py_TPFLAGS_DEFAULT;
    CurrentType.tp_doc = "vim current object";
    CurrentType.tp_methods = CurrentMethods;
#যদি PY_MAJOR_VERSION >= 3
    CurrentType.tp_getattro = CurrentGetattro;
    CurrentType.tp_setattro = CurrentSetattro;
#অন্যথা
    CurrentType.tp_getattr = CurrentGetattr;
    CurrentType.tp_setattr = CurrentSetattr;
#endif

    CLEAR_FIELD(DictionaryType);
    DictionaryType.tp_name = "vim.dictionary";
    DictionaryType.tp_basicsize = পরিমাণ(DictionaryObject);
    DictionaryType.tp_dealloc = DictionaryDestructor;
    DictionaryType.tp_as_sequence = &DictionaryAsSeq;
    DictionaryType.tp_as_mapping = &DictionaryAsMapping;
    DictionaryType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE;
    DictionaryType.tp_doc = "dictionary pushing modifications to Vim structure";
    DictionaryType.tp_methods = DictionaryMethods;
    DictionaryType.tp_iter = DictionaryIter;
    DictionaryType.tp_new = DictionaryConstructor;
    DictionaryType.tp_alloc = PyType_GenericAlloc;
#যদি PY_MAJOR_VERSION >= 3
    DictionaryType.tp_getattro = DictionaryGetattro;
    DictionaryType.tp_setattro = DictionarySetattro;
#অন্যথা
    DictionaryType.tp_getattr = DictionaryGetattr;
    DictionaryType.tp_setattr = DictionarySetattr;
#endif

    CLEAR_FIELD(ListType);
    ListType.tp_name = "vim.list";
    ListType.tp_dealloc = ListDestructor;
    ListType.tp_basicsize = পরিমাণ(ListObject);
    ListType.tp_as_sequence = &ListAsSeq;
    ListType.tp_as_mapping = &ListAsMapping;
    ListType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE;
    ListType.tp_doc = "list pushing modifications to Vim structure";
    ListType.tp_methods = ListMethods;
    ListType.tp_iter = ListIter;
    ListType.tp_new = ListConstructor;
    ListType.tp_alloc = PyType_GenericAlloc;
#যদি PY_MAJOR_VERSION >= 3
    ListType.tp_getattro = ListGetattro;
    ListType.tp_setattro = ListSetattro;
#অন্যথা
    ListType.tp_getattr = ListGetattr;
    ListType.tp_setattr = ListSetattr;
#endif

    // Tuple type
    CLEAR_FIELD(TupleType);
    TupleType.tp_name = "vim.tuple";
    TupleType.tp_dealloc = TupleDestructor;
    TupleType.tp_basicsize = পরিমাণ(TupleObject);
    TupleType.tp_as_sequence = &TupleAsSeq;
    TupleType.tp_as_mapping = &TupleAsMapping;
    TupleType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE;
    TupleType.tp_doc = "tuple pushing modifications to Vim structure";
    TupleType.tp_methods = TupleMethods;
    TupleType.tp_iter = TupleIter;
    TupleType.tp_new = TupleConstructor;
    TupleType.tp_alloc = PyType_GenericAlloc;
#যদি PY_MAJOR_VERSION >= 3
    TupleType.tp_getattro = TupleGetattro;
    TupleType.tp_setattro = TupleSetattro;
#অন্যথা
    TupleType.tp_getattr = TupleGetattr;
    TupleType.tp_setattr = TupleSetattr;
#endif

    CLEAR_FIELD(FunctionType);
    FunctionType.tp_name = "vim.function";
    FunctionType.tp_basicsize = পরিমাণ(FunctionObject);
    FunctionType.tp_dealloc = FunctionDestructor;
    FunctionType.tp_call = FunctionCall;
    FunctionType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE;
    FunctionType.tp_doc = "object that calls Vim function";
    FunctionType.tp_methods = FunctionMethods;
    FunctionType.tp_repr = FunctionRepr;
    FunctionType.tp_new = FunctionConstructor;
    FunctionType.tp_alloc = PyType_GenericAlloc;
#যদি PY_MAJOR_VERSION >= 3
    FunctionType.tp_getattro = FunctionGetattro;
#অন্যথা
    FunctionType.tp_getattr = FunctionGetattr;
#endif

    CLEAR_FIELD(OptionsType);
    OptionsType.tp_name = "vim.options";
    OptionsType.tp_basicsize = পরিমাণ(OptionsObject);
    OptionsType.tp_as_sequence = &OptionsAsSeq;
    OptionsType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    OptionsType.tp_doc = "object for manipulating options";
    OptionsType.tp_iter = OptionsIter;
    OptionsType.tp_as_mapping = &OptionsAsMapping;
    OptionsType.tp_dealloc = OptionsDestructor;
    OptionsType.tp_traverse = OptionsTraverse;
    OptionsType.tp_clear = OptionsClear;

#যদি PY_VERSION_HEX < 0x030700f0
    CLEAR_FIELD(LoaderType);
    LoaderType.tp_name = "vim.Loader";
    LoaderType.tp_basicsize = পরিমাণ(LoaderObject);
    LoaderType.tp_flags = Py_TPFLAGS_DEFAULT;
    LoaderType.tp_doc = "vim message object";
    LoaderType.tp_methods = LoaderMethods;
    LoaderType.tp_dealloc = LoaderDestructor;
#endif

#যদি PY_MAJOR_VERSION >= 3
    CLEAR_FIELD(vimmodule);
    vimmodule.m_name = "vim";
    vimmodule.m_doc = "Vim Python interface\n";
    vimmodule.m_size = -1;
    vimmodule.m_methods = VimMethods;
#endif
}

    স্থির পূর্ণাঙ্ক
init_types(শূন্য)
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
#যদি PY_VERSION_HEX < 0x030700f0
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
    প্রত্যাগম 0;
}

#ifdef USE_LIMITED_API
    স্থির শূন্য
shutdown_types(শূন্য)
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
# যদি PY_VERSION_HEX < 0x030700f0
    PYTYPE_CLEANUP(LoaderType);
# endif
}
#endif

    স্থির পূর্ণাঙ্ক
init_sys_path(শূন্য)
{
    PyObject	*path;
    PyObject	*path_hook;
    PyObject	*path_hooks;

    যদি (!(path_hook = PyObject_GetAttrString(vim_module, "path_hook")))
	প্রত্যাগম -1;

    যদি (!(path_hooks = PySys_GetObject("path_hooks")))
    {
	PyErr_Clear();
	path_hooks = PyList_New(1);
	PyList_SET_ITEM(path_hooks, 0, path_hook);
	যদি (PySys_SetObject("path_hooks", path_hooks))
	{
	    Py_DECREF(path_hooks);
	    প্রত্যাগম -1;
	}
	Py_DECREF(path_hooks);
    }
    অন্যথা যদি (PyList_Check(path_hooks))
    {
	যদি (PyList_Append(path_hooks, path_hook))
	{
	    Py_DECREF(path_hook);
	    প্রত্যাগম -1;
	}
	Py_DECREF(path_hook);
    }
    অন্যথা
    {
	VimTryStart();
	emsg(_("Failed to set path hook: sys.path_hooks is not a list\n"
	       "You should now do the following:\n"
	       "- append vim.path_hook to sys.path_hooks\n"
	       "- append vim.VIM_SPECIAL_PATH to sys.path\n"));
	VimTryEnd(); // Discard the error
	Py_DECREF(path_hook);
	প্রত্যাগম 0;
    }

    যদি (!(path = PySys_GetObject("path")))
    {
	PyErr_Clear();
	path = PyList_New(1);
	Py_INCREF(vim_special_path_object);
	PyList_SET_ITEM(path, 0, vim_special_path_object);
	যদি (PySys_SetObject("path", path))
	{
	    Py_DECREF(path);
	    প্রত্যাগম -1;
	}
	Py_DECREF(path);
    }
    অন্যথা যদি (PyList_Check(path))
    {
	যদি (PyList_Append(path, vim_special_path_object))
	    প্রত্যাগম -1;
    }
    অন্যথা
    {
	VimTryStart();
	emsg(_("Failed to set path: sys.path is not a list\n"
	       "You should now append vim.VIM_SPECIAL_PATH to sys.path"));
	VimTryEnd(); // Discard the error
    }

    প্রত্যাগম 0;
}

স্থির সংরচনা numeric_constant {
    বর্ণ	*name;
    পূর্ণাঙ্ক		val;
} numeric_constants[] = {
    {"VAR_LOCKED",	VAR_LOCKED},
    {"VAR_FIXED",	VAR_FIXED},
    {"VAR_SCOPE",	VAR_SCOPE},
    {"VAR_DEF_SCOPE",	VAR_DEF_SCOPE},
};

সংরচনা object_constant {
    বর্ণ	*name;
    PyObject	*valObject;
};

#define ADD_OBJECT(m, name, obj) \
    যদি (PyModule_AddObject(m, name, obj)) \
	প্রত্যাগম -1;

#define ADD_CHECKED_OBJECT(m, name, obj) \
    { \
	PyObject	*valObject = obj; \
	যদি (!valObject) \
	    প্রত্যাগম -1; \
	ADD_OBJECT(m, name, valObject); \
    }

    স্থির পূর্ণাঙ্ক
populate_module(PyObject *m)
{
    পূর্ণাঙ্ক		i;
    PyObject	*other_module;
    PyObject	*attr;
    PyObject	*imp;
#যদি PY_VERSION_HEX >= 0x030700f0
    PyObject	*dict;
    PyObject	*cls;
#endif

    প্রতি (i = 0; i < (পূর্ণাঙ্ক)(পরিমাণ(numeric_constants)
					   / পরিমাণ(সংরচনা numeric_constant));
	    ++i)
	ADD_CHECKED_OBJECT(m, numeric_constants[i].name,
		PyInt_FromLong(numeric_constants[i].val));

    সংরচনা object_constant object_constants[] = {
	{"buffers",  (PyObject *)(শূন্য *)&TheBufferMap},
	{"windows",  (PyObject *)(শূন্য *)&TheWindowList},
	{"tabpages", (PyObject *)(শূন্য *)&TheTabPageList},
	{"current",  (PyObject *)(শূন্য *)&TheCurrent},

	{"Buffer",     (PyObject *)BufferTypePtr},
	{"Range",      (PyObject *)RangeTypePtr},
	{"Window",     (PyObject *)WindowTypePtr},
	{"TabPage",    (PyObject *)TabPageTypePtr},
	{"Dictionary", (PyObject *)DictionaryTypePtr},
	{"List",       (PyObject *)ListTypePtr},
	{"Tuple",      (PyObject *)TupleTypePtr},
	{"Function",   (PyObject *)FunctionTypePtr},
	{"Options",    (PyObject *)OptionsTypePtr},
#যদি PY_VERSION_HEX < 0x030700f0
	{"_Loader",    (PyObject *)LoaderTypePtr},
#endif
    };

    প্রতি (i = 0; i < (পূর্ণাঙ্ক)(পরিমাণ(object_constants)
					    / পরিমাণ(সংরচনা object_constant));
	    ++i)
    {
	PyObject	*valObject;

	valObject = object_constants[i].valObject;
	Py_INCREF(valObject);
	ADD_OBJECT(m, object_constants[i].name, valObject);
    }

    যদি (!(VimError = PyErr_NewException("vim.error", রিক্ত, রিক্ত)))
	প্রত্যাগম -1;
    ADD_OBJECT(m, "error", VimError);

    ADD_CHECKED_OBJECT(m, "vars",  NEW_DICTIONARY(get_globvar_dict()));
    ADD_CHECKED_OBJECT(m, "vvars", NEW_DICTIONARY(get_vimvar_dict()));
    ADD_CHECKED_OBJECT(m, "options",
	    OptionsNew(SREQ_GLOBAL, রিক্ত, dummy_check, রিক্ত));

    যদি (!(other_module = PyImport_ImportModule("os")))
	প্রত্যাগম -1;
    ADD_OBJECT(m, "os", other_module);

#যদি PY_MAJOR_VERSION >= 3
    যদি (!(py_getcwd = PyObject_GetAttrString(other_module, "getcwd")))
	প্রত্যাগম -1;
#অন্যথা
    যদি (!(py_getcwd = PyObject_GetAttrString(other_module, "getcwdu")))
	প্রত্যাগম -1;
#endif
    ADD_OBJECT(m, "_getcwd", py_getcwd)

    যদি (!(py_chdir = PyObject_GetAttrString(other_module, "chdir")))
	প্রত্যাগম -1;
    ADD_OBJECT(m, "_chdir", py_chdir);
    যদি (!(attr = PyObject_GetAttrString(m, "chdir")))
	প্রত্যাগম -1;
    যদি (PyObject_SetAttrString(other_module, "chdir", attr))
    {
	Py_DECREF(attr);
	প্রত্যাগম -1;
    }
    Py_DECREF(attr);

    যদি ((py_fchdir = PyObject_GetAttrString(other_module, "fchdir")))
    {
	ADD_OBJECT(m, "_fchdir", py_fchdir);
	যদি (!(attr = PyObject_GetAttrString(m, "fchdir")))
	    প্রত্যাগম -1;
	যদি (PyObject_SetAttrString(other_module, "fchdir", attr))
	{
	    Py_DECREF(attr);
	    প্রত্যাগম -1;
	}
	Py_DECREF(attr);
    }
    অন্যথা
	PyErr_Clear();

    যদি (!(vim_special_path_object = PyString_FromString(vim_special_path)))
	প্রত্যাগম -1;

    ADD_OBJECT(m, "VIM_SPECIAL_PATH", vim_special_path_object);

#যদি PY_VERSION_HEX >= 0x030700f0
    যদি (!(imp = PyImport_ImportModule("importlib.machinery")))
	প্রত্যাগম -1;

    dict = PyModule_GetDict(imp);

    যদি (!(cls = PyDict_GetItemString(dict, "PathFinder")))
    {
	Py_DECREF(imp);
	প্রত্যাগম -1;
    }

    যদি (!(py_find_spec = PyObject_GetAttrString(cls, "find_spec")))
    {
	Py_DECREF(imp);
	প্রত্যাগম -1;
    }

# যদি PY_VERSION_HEX < 0x30c00a7
    // find_module has been removed as of Python 3.12.0a7
    যদি ((py_find_module = PyObject_GetAttrString(cls, "find_module")))
	ADD_OBJECT(m, "_find_module", py_find_module);
# endif

    Py_DECREF(imp);

    ADD_OBJECT(m, "_find_spec", py_find_spec);
#অন্যথা
    যদি (!(imp = PyImport_ImportModule("imp")))
	প্রত্যাগম -1;

    যদি (!(py_find_module = PyObject_GetAttrString(imp, "find_module")))
    {
	Py_DECREF(imp);
	প্রত্যাগম -1;
    }

    যদি (!(py_load_module = PyObject_GetAttrString(imp, "load_module")))
    {
	Py_DECREF(py_find_module);
	Py_DECREF(imp);
	প্রত্যাগম -1;
    }

    Py_DECREF(imp);

    ADD_OBJECT(m, "_find_module", py_find_module);
    ADD_OBJECT(m, "_load_module", py_load_module);
#endif

    প্রত্যাগম 0;
}
