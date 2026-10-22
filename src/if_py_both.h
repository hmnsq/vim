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

#எனில் PY_VERSION_HEX < 0x02050000
வகைவரை முழு Py_ssize_t;  // Python 2.4 and earlier don't have this type.
#endif

// Use values that are known to work, others may make Vim crash.
#define ENC_OPT (enc_utf8 ? "utf-8" : enc_dbcs ? "euc-jp" : (எழுத்து *)p_enc)
#define DOPY_FUNC "_vim_pydo"

நிலையான மாறிலி எழுத்து *vim_special_path = "_vim_path_";

#define PyErr_SET_STRING(exc, str) PyErr_SetString(exc, _(str))
#define PyErr_SetVim(str) PyErr_SetString(VimError, str)
#define PyErr_SET_VIM(str) PyErr_SET_STRING(VimError, str)
#define PyErr_FORMAT(exc, str, arg) PyErr_Format(exc, _(str), arg)
#define PyErr_FORMAT2(exc, str, arg1, arg2) PyErr_Format(exc, _(str), arg1,arg2)
#define PyErr_VIM_FORMAT(str, arg) PyErr_FORMAT(VimError, str, arg)

#ifdef USE_LIMITED_API
// Limited Python API. Need to call only exposed functions and remap macros.
// PyTypeObject is an opaque struct.

வகைவரை கட்டமைப்பு {
    lenfunc sq_length;
    binaryfunc sq_concat;
    ssizeargfunc sq_repeat;
    ssizeargfunc sq_item;
    வெறுமை *was_sq_slice;
    ssizeobjargproc sq_ass_item;
    வெறுமை *was_sq_ass_slice;
    objobjproc sq_contains;

    binaryfunc sq_inplace_concat;
    ssizeargfunc sq_inplace_repeat;
} PySequenceMethods;

வகைவரை கட்டமைப்பு {
    lenfunc mp_length;
    binaryfunc mp_subscript;
    objobjargproc mp_ass_subscript;
} PyMappingMethods;

// This struct emulates the concrete _typeobject struct to allow the code to
// work the same way in both limited and full Python APIs.
கட்டமைப்பு typeobject_wrapper {
    மாறிலி எழுத்து *tp_name;
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

    மாறிலி எழுத்து *tp_doc;

    traverseproc tp_traverse;

    inquiry tp_clear;

    getiterfunc tp_iter;
    iternextfunc tp_iternext;

    கட்டமைப்பு PyMethodDef *tp_methods;
    கட்டமைப்பு _typeobject *tp_base;
    allocfunc tp_alloc;
    newfunc tp_new;
    freefunc tp_free;
};

# define DEFINE_PY_TYPE_OBJECT(type) \
    நிலையான கட்டமைப்பு typeobject_wrapper type; \
    நிலையான PyTypeObject* type##Ptr = வெற்று

// PyObject_HEAD_INIT_TYPE and PyObject_FINISH_INIT_TYPE need to come in pairs
// We first initialize with NULL because the type is not allocated until
// init_types() is called later. It's in FINISH_INIT_TYPE where we fill the
// type in with the newly allocated type.
# define PyObject_HEAD_INIT_TYPE(type) PyObject_HEAD_INIT(வெற்று)
# define PyObject_FINISH_INIT_TYPE(obj, type) obj.ob_base.ob_type = type##Ptr

# define Py_TYPE_GET_TP_ALLOC(type) ((allocfunc)PyType_GetSlot(type, Py_tp_alloc))
# define Py_TYPE_GET_TP_METHODS(type) ((PyMethodDef *)PyType_GetSlot(type, Py_tp_methods))

// PyObject_NEW is not part of stable ABI, but PyObject_Malloc/Init are.
நிலையான PyObject* Vim_PyObject_New(PyTypeObject *type, size_t objsize)
{
    PyObject *obj = (PyObject *)PyObject_Malloc(objsize);
    எனில் (obj == வெற்று)
	திரும்பு PyErr_NoMemory();
    திரும்பு PyObject_Init(obj, type);
}
# undef PyObject_NEW
# define PyObject_NEW(type, typeobj) ((type *)Vim_PyObject_New(typeobj, அளவு(type)))

// This is a somewhat convoluted because limited API doesn't expose an easy way
// to get the tp_name field, and so we have to manually reconstruct it as
// "__module__.__name__" (with __module__ omitted for builtins to emulate
// Python behavior). Also, some of the more convenient functions like
// PyUnicode_AsUTF8AndSize and PyType_GetQualName() are not available until
// late Python 3 versions, and won't be available if you set Py_LIMITED_API too
// low.
# define PyErr_FORMAT_TYPE(செய்தி, obj) \
    செய் { \
	PyObject* qualname = PyObject_GetAttrString((PyObject*)(obj)->ob_type, "__qualname__"); \
	எனில் (qualname == வெற்று) \
	{ \
	    PyErr_FORMAT(PyExc_TypeError, செய்தி, "(NULL)"); \
	    நிறுத்து; \
	} \
	PyObject* module = PyObject_GetAttrString((PyObject*)(obj)->ob_type, "__module__"); \
	PyObject* full; \
	எனில் (module == வெற்று || PyUnicode_CompareWithASCIIString(module, "builtins") == 0 \
		|| PyUnicode_CompareWithASCIIString(module, "__main__") == 0) \
	{ \
	    full = qualname; \
	    Py_INCREF(full); \
	} \
	இல்லையெனில் \
	    full = PyUnicode_FromFormat("%U.%U", module, qualname); \
	PyObject* full_bytes = PyUnicode_AsUTF8String(full); \
	மாறிலி எழுத்து* full_str = PyBytes_AsString(full_bytes); \
	full_str = full_str == வெற்று ? "(NULL)" : full_str; \
	PyErr_FORMAT(PyExc_TypeError, செய்தி, full_str); \
	Py_DECREF(qualname); \
	Py_XDECREF(module); \
	Py_XDECREF(full); \
	Py_XDECREF(full_bytes); \
    } போது (0)

# define PyList_GET_ITEM(list, i) PyList_GetItem(list, i)
# define PyList_GET_SIZE(o) PyList_Size(o)

// PyList_SET_ITEM and PyList_SetItem have slightly different behaviors. The
// former will leave the old item dangling, and the latter will decref on it.
// Since we only use this on new lists, this difference doesn't matter.
# define PyList_SET_ITEM(list, i, item) PyList_SetItem(list, i, item)

# define PyTuple_GET_ITEM(o, pos) PyTuple_GetItem(o, pos)
# define PyTuple_GET_SIZE(o) PyTuple_Size(o)

# எனில் Py_LIMITED_API < 0x03080000
// PyIter_check only became part of stable ABI in 3.8, and there is no easy way
// to check for it in the API. We simply return false as a compromise. This
// does mean we should avoid compiling with stable ABI < 3.8.
#  undef PyIter_Check
#  define PyIter_Check(obj) (FALSE)
# endif

நிலையான PyTypeObject* AddHeapType(கட்டமைப்பு typeobject_wrapper* type_object)
{
    PyType_Spec type_spec;
    type_spec.name = type_object->tp_name;
    type_spec.basicsize = type_object->tp_basicsize;
    type_spec.itemsize = 0;
    type_spec.flags = type_object->tp_flags;

    // We just need to statically allocate a large enough buffer that can hold
    // all slots. We need to leave a null-terminated slot at the end.
    PyType_Slot slots[40] = { {0, வெற்று} };
    size_t slot_i = 0;

# define ADD_TP_SLOT(slot_name) \
    எனில் (slot_i >= 40) திரும்பு வெற்று; /* this should never happen */ \
    எனில் (type_object->slot_name != வெற்று) \
    { \
	slots[slot_i].slot = Py_##slot_name; \
	slots[slot_i].pfunc = (வெறுமை*)type_object->slot_name; \
	++slot_i; \
    }
# define ADD_TP_SUB_SLOT(sub_slot, slot_name) \
    எனில் (slot_i >= 40) திரும்பு வெற்று; /* this should never happen */ \
    எனில் (type_object->sub_slot != வெற்று && type_object->sub_slot->slot_name != வெற்று) \
    { \
	slots[slot_i].slot = Py_##slot_name; \
	slots[slot_i].pfunc = (வெறுமை*)type_object->sub_slot->slot_name; \
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
    திரும்பு (PyTypeObject*)newtype;
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
    எனில் (type##Ptr == வெற்று) \
	திரும்பு -1;
# define PYTYPE_CLEANUP(type) \
    type##Ptr = வெற்று;

// Limited API does not provide PyRun_* functions. Need to implement manually
// using PyCompile and PyEval.
நிலையான PyObject* Vim_PyRun_String(மாறிலி எழுத்து *str, முழு start, PyObject *globals, PyObject *locals)
{
    // Just pass "" for filename for now.
    PyObject* compiled = Py_CompileString(str, "", start);
    எனில் (compiled == வெற்று)
	திரும்பு வெற்று;

    PyObject* eval_result = PyEval_EvalCode(compiled, globals, locals);
    Py_DECREF(compiled);
    திரும்பு eval_result;
}
நிலையான முழு Vim_PyRun_SimpleString(மாறிலி எழுத்து *str)
{
    // This function emulates CPython's implementation.
    PyObject* m = PyImport_AddModule("__main__");
    எனில் (m == வெற்று)
	திரும்பு -1;
    PyObject* d = PyModule_GetDict(m);
    PyObject* output = Vim_PyRun_String(str, Py_file_உள்ளீடு, d, d);
    எனில் (output == வெற்று)
    {
	PyErr_PrintEx(TRUE);
	திரும்பு -1;
    }
    Py_DECREF(output);
    திரும்பு 0;
}
# define PyRun_String Vim_PyRun_String
# define PyRun_SimpleString Vim_PyRun_SimpleString

#இல்லையெனில் // !defined(USE_LIMITED_API)

// Full Python API. Can make use of structs and macros directly.
# define DEFINE_PY_TYPE_OBJECT(type) \
    நிலையான PyTypeObject type; \
    நிலையான PyTypeObject* type##Ptr = &type
# define PyObject_HEAD_INIT_TYPE(type) PyObject_HEAD_INIT(&type)

# define Py_TYPE_GET_TP_ALLOC(type) type->tp_alloc
# define Py_TYPE_GET_TP_METHODS(type) type->tp_methods

# define Py_TYPE_NAME(obj) ((obj)->ob_type->tp_name == வெற்று \
	? "(NULL)" \
	: (obj)->ob_type->tp_name)
# define PyErr_FORMAT_TYPE(செய்தி, obj) \
    PyErr_FORMAT(PyExc_TypeError, செய்தி, \
	    Py_TYPE_NAME(obj))

// Add a static type
# define PYTYPE_READY(type) \
    எனில் (PyType_Ready(type##Ptr)) \
	திரும்பு -1;

#endif


#define RAISE_NO_EMPTY_சாவிகள் PyErr_SET_STRING(Py_Exc_மதிப்பு_Error, \
					    N_("empty keys are not allowed"))
#define RAISE_LOCKED_DICTIONARY PyErr_SET_VIM(N_("dictionary is locked"))
#define RAISE_LOCKED_LIST PyErr_SET_VIM(N_("list is locked"))
#define RAISE_UNDO_FAIL PyErr_SET_VIM(N_("cannot save undo information"))
#define RAISE_DELETE_வரி_FAIL PyErr_SET_VIM(N_("cannot delete line"))
#define RAISE_செருகு_வரி_FAIL PyErr_SET_VIM(N_("cannot insert line"))
#define RAISE_REPLACE_வரி_FAIL PyErr_SET_VIM(N_("cannot replace line"))
#define RAISE_சாவி_ADD_FAIL(சாவி) \
    PyErr_VIM_FORMAT(N_("failed to add key '%s' to dictionary"), சாவி)
#define RAISE_INVALID_INDEX_TYPE(idx) \
    PyErr_FORMAT_TYPE(N_("index must be int or slice, not %s"), idx);

#define INVALID_BUFFER_மதிப்பு ((buf_T *)(-1))
#define INVALID_WINDOW_மதிப்பு ((win_T *)(-1))
#define INVALID_TABPAGE_மதிப்பு ((tabpage_T *)(-1))

வகைவரை வெறுமை (*rangeinitializer)(வெறுமை *);
வகைவரை வெறுமை (*runner)(மாறிலி எழுத்து *, dict_T *, வெறுமை *
#ifdef PY_CAN_RECURSE
	, PyGILState_STATE *
#endif
	);

நிலையான முழு ConvertFromPyObject(PyObject *, typval_T *);
நிலையான முழு _ConvertFromPyObject(PyObject *, typval_T *, PyObject *);
நிலையான முழு ConvertFromPyMapping(PyObject *, typval_T *);
நிலையான முழு ConvertFromPySequence(PyObject *, typval_T *);
நிலையான PyObject *WindowNew(win_T *, tabpage_T *);
நிலையான PyObject *BufferNew (buf_T *);
நிலையான PyObject *வரி_To_String(மாறிலி எழுத்து *);

நிலையான PyInt RangeStart;
நிலையான PyInt RangeEnd;

நிலையான PyObject *globals;

நிலையான PyObject *py_chdir;
நிலையான PyObject *py_fchdir;
நிலையான PyObject *py_getcwd;
நிலையான PyObject *vim_module;
நிலையான PyObject *vim_special_path_object;

#எனில் PY_VERSION_HEX >= 0x030700f0
நிலையான PyObject *py_find_spec;
#இல்லையெனில்
நிலையான PyObject *py_load_module;
#endif
#எனில் PY_VERSION_HEX < 0x30c00a7
நிலையான PyObject *py_find_module;
#endif

நிலையான PyObject *VimError;

/*
 * obtain a lock on the Vim data structures
 */
    நிலையான வெறுமை
Python_Lock_Vim(வெறுமை)
{
}

/*
 * release a lock on the Vim data structures
 */
    நிலையான வெறுமை
Python_Release_Vim(வெறுமை)
{
}

/*
 * The "todecref" argument holds a pointer to PyObject * that must be
 * DECREF'ed after returned char_u * is no longer needed or NULL if all what
 * was needed to generate returned value is object.
 *
 * Use Py_XDECREF to decrement reference count.
 */
    நிலையான char_u *
StringToChars(PyObject *obj, PyObject **todecref)
{
    char_u	*str;

    எனில் (PyBytes_Check(obj))
    {

	எனில் (PyBytes_AsStringAndSize(obj, (எழுத்து **) &str, வெற்று) == -1
		|| str == வெற்று)
	    திரும்பு வெற்று;

	*todecref = வெற்று;
    }
    இல்லையெனில் எனில் (PyUnicode_Check(obj))
    {
	PyObject	*bytes;

	எனில் (!(bytes = PyUnicode_AsEncodedString(obj, ENC_OPT,
							   ERRORS_ENCODE_ARG)))
	    திரும்பு வெற்று;

	எனில் (PyBytes_AsStringAndSize(bytes, (எழுத்து **) &str, வெற்று) == -1
		|| str == வெற்று)
	{
	    Py_DECREF(bytes);
	    திரும்பு வெற்று;
	}

	*todecref = bytes;
    }
    இல்லையெனில்
    {
#எனில் PY_MAJOR_VERSION < 3
	PyErr_FORMAT_TYPE(N_("expected str() or unicode() instance, but got %s"),
		obj);
#இல்லையெனில்
	PyErr_FORMAT_TYPE(N_("expected bytes() or str() instance, but got %s"),
		obj);
#endif
	திரும்பு வெற்று;
    }

    திரும்பு (char_u *) str;
}

#define எண்_LONG     1
#define எண்_INT      2
#define எண்_NATURAL  4
#define எண்_UNSIGNED 8

    நிலையான முழு
எண்_To_Long(PyObject *obj, long *result, முழு flags)
{
#எனில் PY_MAJOR_VERSION < 3
    எனில் (PyInt_Check(obj))
    {
	*result = PyInt_AsLong(obj);
	எனில் (PyErr_Occurred())
	    திரும்பு -1;
    }
    இல்லையெனில்
#endif
    எனில் (PyLong_Check(obj))
    {
	*result = PyLong_AsLong(obj);
	எனில் (PyErr_Occurred())
	    திரும்பு -1;
    }
    இல்லையெனில் எனில் (Py_எண்_Check(obj))
    {
	PyObject	*இலக்கம்;

	எனில் (!(இலக்கம் = Py_எண்_Long(obj)))
	    திரும்பு -1;

	*result = PyLong_AsLong(இலக்கம்);

	Py_DECREF(இலக்கம்);

	எனில் (PyErr_Occurred())
	    திரும்பு -1;
    }
    இல்லையெனில்
    {
#எனில் PY_MAJOR_VERSION < 3
	PyErr_FORMAT_TYPE(
		N_("expected int(), long() or something supporting "
		   "coercing to long(), but got %s"),
		obj);
#இல்லையெனில்
	PyErr_FORMAT_TYPE(
		N_("expected int() or something supporting coercing to int(), "
		   "but got %s"),
		obj);
#endif
	திரும்பு -1;
    }

    எனில் (flags & எண்_INT)
    {
	எனில் (*result > INT_MAX)
	{
	    PyErr_SET_STRING(PyExc_OverflowError,
		    N_("value is too large to fit into C int type"));
	    திரும்பு -1;
	}
	இல்லையெனில் எனில் (*result < INT_MIN)
	{
	    PyErr_SET_STRING(PyExc_OverflowError,
		    N_("value is too small to fit into C int type"));
	    திரும்பு -1;
	}
    }

    எனில் (flags & எண்_NATURAL)
    {
	எனில் (*result <= 0)
	{
	    PyErr_SET_STRING(Py_Exc_மதிப்பு_Error,
		    N_("number must be greater than zero"));
	    திரும்பு -1;
	}
    }
    இல்லையெனில் எனில் (flags & எண்_UNSIGNED)
    {
	எனில் (*result < 0)
	{
	    PyErr_SET_STRING(Py_Exc_மதிப்பு_Error,
		    N_("number must be greater or equal to zero"));
	    திரும்பு -1;
	}
    }

    திரும்பு 0;
}

    நிலையான முழு
add_string(PyObject *list, எழுத்து *s)
{
    PyObject	*string;

    எனில் (!(string = PyString_FromString(s)))
	திரும்பு -1;

    எனில் (PyList_Append(list, string))
    {
	Py_DECREF(string);
	திரும்பு -1;
    }

    Py_DECREF(string);
    திரும்பு 0;
}

    நிலையான PyObject *
ObjectDir(PyObject *self, எழுத்து **attributes)
{
    PyMethodDef	*method;
    எழுத்து	**attr;
    PyObject	*ret;

    எனில் (!(ret = PyList_New(0)))
	திரும்பு வெற்று;

    எனில் (self)
	க்கு (method = Py_TYPE_GET_TP_METHODS(self->ob_type) ; method->ml_name != வெற்று ; ++method)
	    எனில் (add_string(ret, (எழுத்து *)method->ml_name))
	    {
		Py_DECREF(ret);
		திரும்பு வெற்று;
	    }

    க்கு (attr = attributes ; *attr ; ++attr)
	எனில் (add_string(ret, *attr))
	{
	    Py_DECREF(ret);
	    திரும்பு வெற்று;
	}

#எனில் PY_MAJOR_VERSION < 3
    எனில் (add_string(ret, "__members__"))
    {
	Py_DECREF(ret);
	திரும்பு வெற்று;
    }
#endif

    திரும்பு ret;
}

// Output buffer management

// Function to write a line, points to either msg() or emsg().
வகைவரை முழு (*writefn)(எழுத்து *);

DEFINE_PY_TYPE_OBJECT(OutputType);

வகைவரை கட்டமைப்பு
{
    PyObject_HEAD
    long softspace;
    long error;
} OutputObject;

நிலையான எழுத்து *OutputAttrs[] = {
    "softspace",
    வெற்று
};

    நிலையான PyObject *
OutputDir(PyObject *self, PyObject *args UNUSED)
{
    திரும்பு ObjectDir(self, OutputAttrs);
}

    நிலையான முழு
OutputSetattr(PyObject *self_obj, எழுத்து *name, PyObject *valObject)
{
    OutputObject *self = (OutputObject*)self_obj;
    எனில் (valObject == வெற்று)
    {
	PyErr_SET_STRING(PyExc_AttributeError,
		N_("can't delete OutputObject attributes"));
	திரும்பு -1;
    }

    எனில் (strcmp(name, "softspace") == 0)
    {
	எனில் (எண்_To_Long(valObject, &(self->softspace), எண்_UNSIGNED))
	    திரும்பு -1;
	திரும்பு 0;
    }

    PyErr_FORMAT(PyExc_AttributeError, N_("invalid attribute: %s"), name);
    திரும்பு -1;
}

// Buffer IO, we write one whole line at a time.
நிலையான garray_T io_ga = {0, 0, 1, 80, வெற்று};
நிலையான writefn old_fn = வெற்று;

    நிலையான வெறுமை
PythonIO_Flush(வெறுமை)
{
    எனில் (old_fn != வெற்று && io_ga.ga_len > 0)
    {
	((எழுத்து *)io_ga.ga_data)[io_ga.ga_len] = NUL;
	// We don't know what emsg_severe should be here, so ... hope?
	old_fn((எழுத்து *)io_ga.ga_data);
    }
    io_ga.ga_len = 0;
}

    நிலையான வெறுமை
writer(writefn fn, char_u *str, PyInt n, முழு severe)
{
    char_u *ptr;

    // Flush when switching output function.
    எனில் (fn != old_fn)
	PythonIO_Flush();
    old_fn = fn;

    // Write each NL separated line.  Text after the last NL is kept for
    // writing later.
    // For normal messages: Do not output when "got_int" was set.  This avoids
    // a loop gone crazy flooding the terminal with messages.  Also for when
    // "q" is pressed at the more-prompt.
    போது (n > 0 && (ptr = memchr(str, '\n', n)) != வெற்று
					  && (fn == (writefn)emsg || !got_int))
    {
	PyInt len = ptr - str;

	எனில் (ga_grow(&io_ga, (முழு)(len + 1)) == FAIL)
	    நிறுத்து;

	mch_memmove(((எழுத்து *)io_ga.ga_data) + io_ga.ga_len, str, (size_t)len);
	((எழுத்து *)io_ga.ga_data)[io_ga.ga_len + len] = NUL;
	emsg_severe = severe;
	fn((எழுத்து *)io_ga.ga_data);
	str = ptr + 1;
	n -= len + 1;
	io_ga.ga_len = 0;
    }

    // Put the remaining text into io_ga for later printing.
    எனில் (n > 0 && (fn == (writefn)emsg || !got_int)
					&& ga_grow(&io_ga, (முழு)(n + 1)) == OK)
    {
	mch_memmove(((எழுத்து *)io_ga.ga_data) + io_ga.ga_len, str, (size_t)n);
	io_ga.ga_len += (முழு)n;
    }
}

    நிலையான முழு
write_output(OutputObject *self, PyObject *string)
{
    Py_ssize_t	len = 0;
    எழுத்து	*str = வெற்று;
    முழு		error = self->error;

    எனில் (!PyArg_Parse(string, "et#", ENC_OPT, &str, &len))
	திரும்பு -1;

    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    writer((writefn)(error ? emsg : செய்தி), (char_u *)str, len, error);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS
    PyMem_Free(str);

    திரும்பு 0;
}

    நிலையான PyObject *
OutputWrite(OutputObject *self, PyObject *string)
{
    எனில் (write_output(self, string))
	திரும்பு வெற்று;

    Py_INCREF(Py_None);
    திரும்பு Py_None;
}

    நிலையான PyObject *
OutputWritelines(OutputObject *self, PyObject *seq)
{
    PyObject	*iterator;
    PyObject	*item;

    எனில் (!(iterator = PyObject_GetIter(seq)))
	திரும்பு வெற்று;

    போது ((item = PyIter_Next(iterator)))
    {
	எனில் (write_output(self, item))
	{
	    Py_DECREF(iterator);
	    Py_DECREF(item);
	    திரும்பு வெற்று;
	}
	Py_DECREF(item);
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    எனில் (PyErr_Occurred())
	திரும்பு வெற்று;

    Py_INCREF(Py_None);
    திரும்பு Py_None;
}

    நிலையான PyObject *
AlwaysNone(PyObject *self UNUSED, PyObject *args UNUSED)
{
    // do nothing
    Py_INCREF(Py_None);
    திரும்பு Py_None;
}
#define ALWAYS_NONE AlwaysNone(வெற்று, வெற்று)

    நிலையான PyObject *
AlwaysFalse(PyObject *self UNUSED, PyObject *args UNUSED)
{
    // do nothing
    PyObject	*ret = Py_False;
    Py_INCREF(ret);
    திரும்பு ret;
}
#define ALWAYS_FALSE AlwaysFalse(வெற்று, வெற்று)

    நிலையான PyObject *
AlwaysTrue(PyObject *self UNUSED, PyObject *args UNUSED)
{
    // do nothing
    PyObject	*ret = Py_True;
    Py_INCREF(ret);
    திரும்பு ret;
}
#define ALWAYS_TRUE AlwaysTrue(வெற்று, வெற்று)

/***************/

நிலையான கட்டமைப்பு PyMethodDef OutputMethods[] = {
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
    { வெற்று,	    வெற்று,				0,		வெற்று}
};

நிலையான OutputObject Output =
{
    PyObject_HEAD_INIT_TYPE(OutputType)
    0,
    0
};

நிலையான OutputObject Error =
{
    PyObject_HEAD_INIT_TYPE(OutputType)
    0,
    1
};

    நிலையான முழு
PythonIO_Init_io(வெறுமை)
{
    எனில் (PySys_SetObject("stdout", (PyObject *)(வெறுமை *)&Output))
	திரும்பு -1;
    எனில் (PySys_SetObject("stderr", (PyObject *)(வெறுமை *)&Error))
	திரும்பு -1;

    எனில் (PyErr_Occurred())
    {
	emsg(_(e_python_error_initialising_io_object));
	திரும்பு -1;
    }

    திரும்பு 0;
}

#எனில் PY_VERSION_HEX < 0x030700f0
நிலையான PyObject *call_load_module(எழுத்து *name, முழு len, PyObject *find_module_result);

வகைவரை கட்டமைப்பு
{
    PyObject_HEAD
    எழுத்து	*fullname;
    PyObject	*result;
} LoaderObject;
DEFINE_PY_TYPE_OBJECT(LoaderType);

    நிலையான வெறுமை
LoaderDestructor(PyObject *self_obj)
{
    LoaderObject *self = (LoaderObject*)self_obj;
    vim_free(self->fullname);
    Py_XDECREF(self->result);
    DESTRUCTOR_FINISH(self);
}

    நிலையான PyObject *
LoaderLoadModule(LoaderObject *self, PyObject *args UNUSED)
{
    எழுத்து	*fullname = self->fullname;
    PyObject	*result = self->result;
    PyObject	*module;

    எனில் (!fullname)
    {
	module = result ? result : Py_None;
	Py_INCREF(module);
	திரும்பு module;
    }

    module = call_load_module(fullname, (முழு)STRLEN(fullname), result);

    self->fullname = வெற்று;
    self->result = module;

    vim_free(fullname);
    Py_DECREF(result);

    எனில் (!module)
    {
	எனில் (PyErr_Occurred())
	    திரும்பு வெற்று;

	Py_INCREF(Py_None);
	திரும்பு Py_None;
    }

    Py_INCREF(module);
    திரும்பு module;
}

நிலையான கட்டமைப்பு PyMethodDef LoaderMethods[] = {
    // name,	    function,				calling,	doc
    {"load_module", (PyCFunction)LoaderLoadModule,	METH_VARARGS,	""},
    { வெற்று,	    வெற்று,				0,		வெற்று}
};
#endif

/*
 * Check to see whether a Vim error has been reported, or a keyboard
 * interrupt has been detected.
 */
    நிலையான வெறுமை
VimTryStart(வெறுமை)
{
    ++trylevel;
}

    நிலையான முழு
VimTryEnd(வெறுமை)
{
    --trylevel;
    // Without this it stops processing all subsequent Vim script commands and
    // generates strange error messages if I e.g. try calling Test() in a cycle
    did_emsg = FALSE;
    // Keyboard interrupt should be preferred over anything else
    எனில் (got_int)
    {
	எனில் (did_throw)
	    discard_current_exception();
	got_int = FALSE;
	PyErr_SetNone(PyExc_KeyboardInterrupt);
	திரும்பு -1;
    }
    இல்லையெனில் எனில் (செய்தி_list != வெற்று && *செய்தி_list != வெற்று)
    {
	முழு	should_free;
	எழுத்து	*செய்தி;

	செய்தி = get_exception_string(*செய்தி_list, ET_ERROR, வெற்று, &should_free);

	எனில் (செய்தி == வெற்று)
	{
	    PyErr_NoMemory();
	    திரும்பு -1;
	}

	PyErr_SetVim(செய்தி);

	free_global_msglist();

	எனில் (should_free)
	    vim_free(செய்தி);

	திரும்பு -1;
    }
    இல்லையெனில் எனில் (!did_throw)
	திரும்பு (PyErr_Occurred() ? -1 : 0);
    // Python exception is preferred over Vim one; unlikely to occur though
    இல்லையெனில் எனில் (PyErr_Occurred())
    {
	discard_current_exception();
	திரும்பு -1;
    }
    // Finally transform Vim script exception to python one
    இல்லையெனில்
    {
	PyErr_SetVim((எழுத்து *)current_exception->மதிப்பு);
	discard_current_exception();
	திரும்பு -1;
    }
}

    நிலையான முழு
VimCheckInterrupt(வெறுமை)
{
    எனில் (got_int)
    {
	PyErr_SetNone(PyExc_KeyboardInterrupt);
	திரும்பு 1;
    }
    திரும்பு 0;
}

// Vim module - Implementation

    நிலையான PyObject *
VimCommand(PyObject *self UNUSED, PyObject *string)
{
    char_u	*cmd;
    PyObject	*ret;
    PyObject	*todecref;

    எனில் (!(cmd = StringToChars(string, &todecref)))
	திரும்பு வெற்று;

    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();

    VimTryStart();
    do_cmdline_cmd(cmd);
    update_screen(UPD_VALID);

    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    எனில் (VimTryEnd())
	ret = வெற்று;
    இல்லையெனில்
	ret = Py_None;

    Py_XINCREF(ret);
    Py_XDECREF(todecref);
    திரும்பு ret;
}

/*
 * Function to translate a typval_T into a PyObject; this will recursively
 * translate lists/dictionaries into their Python equivalents.
 *
 * The depth parameter is to avoid infinite recursion, set it to 1 when
 * you call VimToPython.
 */
    நிலையான PyObject *
VimToPython(typval_T *our_tv, முழு depth, PyObject *lookup_dict)
{
    PyObject	*ret;
    PyObject	*newObj;
    எழுத்து	ptrBuf[அளவு(வெறுமை *) * 2 + 3];

    // Avoid infinite recursion
    எனில் (depth > 100)
    {
	Py_INCREF(Py_None);
	ret = Py_None;
	திரும்பு ret;
    }

    // Check if we run into a recursive loop.  The item must be in lookup_dict
    // then and we can use it again.
    எனில் ((our_tv->v_type == VAR_LIST && our_tv->vval.v_list != வெற்று)
	    || (our_tv->v_type == VAR_TUPLE && our_tv->vval.v_tuple != வெற்று)
	    || (our_tv->v_type == VAR_DICT && our_tv->vval.v_dict != வெற்று))
    {
	sprintf(ptrBuf, "%p",
		our_tv->v_type == VAR_LIST ? (வெறுமை *)our_tv->vval.v_list
		    : our_tv->v_type == VAR_TUPLE ?
			(வெறுமை *)our_tv->vval.v_tuple
			: (வெறுமை *)our_tv->vval.v_dict);

	எனில் ((ret = PyDict_GetItemString(lookup_dict, ptrBuf)))
	{
	    Py_INCREF(ret);
	    திரும்பு ret;
	}
    }

    எனில் (our_tv->v_type == VAR_STRING)
	ret = PyString_FromString(our_tv->vval.v_string == வெற்று
					? "" : (எழுத்து *)our_tv->vval.v_string);
    இல்லையெனில் எனில் (our_tv->v_type == VAR_எண்)
    {
	எழுத்து buf[NUMBUFLEN];

	// For backwards compatibility numbers are stored as strings.
	sprintf(buf, "%lld", (long long)our_tv->vval.v_எண்);
	ret = PyString_FromString((எழுத்து *)buf);
    }
    இல்லையெனில் எனில் (our_tv->v_type == VAR_FLOAT)
    {
	எழுத்து buf[NUMBUFLEN];

	sprintf(buf, "%f", our_tv->vval.v_float);
	ret = PyString_FromString((எழுத்து *)buf);
    }
    இல்லையெனில் எனில் (our_tv->v_type == VAR_LIST)
    {
	list_T		*list = our_tv->vval.v_list;
	listitem_T	*curr;

	எனில் (list == வெற்று)
	    திரும்பு வெற்று;

	எனில் (!(ret = PyList_New(0)))
	    திரும்பு வெற்று;

	எனில் (PyDict_SetItemString(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    திரும்பு வெற்று;
	}

	CHECK_LIST_MATERIALIZE(list);
	FOR_ALL_LIST_பொருட்கள்(list, curr)
	{
	    எனில் (!(newObj = VimToPython(&curr->li_tv, depth + 1, lookup_dict)))
	    {
		Py_DECREF(ret);
		திரும்பு வெற்று;
	    }
	    எனில் (PyList_Append(ret, newObj))
	    {
		Py_DECREF(newObj);
		Py_DECREF(ret);
		திரும்பு வெற்று;
	    }
	    Py_DECREF(newObj);
	}
    }
    இல்லையெனில் எனில் (our_tv->v_type == VAR_TUPLE)
    {
	tuple_T	*tuple = our_tv->vval.v_tuple;
	முழு	len;

	எனில் (tuple == வெற்று)
	    திரும்பு வெற்று;

	len = TUPLE_LEN(tuple);

	ret = PyTuple_New(len);
	எனில் (ret == வெற்று)
	    திரும்பு வெற்று;

	க்கு (முழு idx = 0; idx < len; idx++)
	{
	    typval_T	*item_tv = TUPLE_ITEM(tuple, idx);

	    newObj = VimToPython(item_tv, depth + 1, lookup_dict);
	    எனில் (!newObj)
	    {
		Py_DECREF(ret);
		திரும்பு வெற்று;
	    }
	    PyTuple_SetItem(ret, idx, newObj);
	}

	எனில் (PyDict_SetItemString(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    திரும்பு வெற்று;
	}
    }
    இல்லையெனில் எனில் (our_tv->v_type == VAR_DICT)
    {

	hashtab_T	*ht;
	long_u		todo;
	hashitem_T	*hi;
	dictitem_T	*di;

	எனில் (our_tv->vval.v_dict == வெற்று)
	    திரும்பு வெற்று;
	ht = &our_tv->vval.v_dict->dv_hashtab;

	எனில் (!(ret = PyDict_New()))
	    திரும்பு வெற்று;

	எனில் (PyDict_SetItemString(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    திரும்பு வெற்று;
	}

	todo = ht->ht_used;
	க்கு (hi = ht->ht_array; todo > 0; ++hi)
	{
	    எனில் (!HASHITEM_EMPTY(hi))
	    {
		--todo;

		di = dict_lookup(hi);
		எனில் (!(newObj = VimToPython(&di->di_tv, depth + 1, lookup_dict)))
		{
		    Py_DECREF(ret);
		    திரும்பு வெற்று;
		}
		எனில் (PyDict_SetItemString(ret, (எழுத்து *)hi->hi_சாவி, newObj))
		{
		    Py_DECREF(ret);
		    Py_DECREF(newObj);
		    திரும்பு வெற்று;
		}
	    }
	}
    }
    இல்லையெனில் எனில் (our_tv->v_type == VAR_BOOL)
    {
	எனில் (our_tv->vval.v_எண் == VVAL_FALSE)
	{
	    ret = Py_False;
	    Py_INCREF(ret);
	}
	இல்லையெனில்
	{
	    ret = Py_True;
	    Py_INCREF(ret);
	}
	திரும்பு ret;
    }
    இல்லையெனில் எனில் (our_tv->v_type == VAR_SPECIAL)
    {
	Py_INCREF(Py_None);
	ret = Py_None;
	திரும்பு ret;
    }
    இல்லையெனில் எனில் (our_tv->v_type == VAR_BLOB)
	ret = PyBytes_FromStringAndSize(
		(எழுத்து*) our_tv->vval.v_blob->bv_ga.ga_data,
		(Py_ssize_t) our_tv->vval.v_blob->bv_ga.ga_len);
    இல்லையெனில்
    {
	Py_INCREF(Py_None);
	ret = Py_None;
    }

    திரும்பு ret;
}

    நிலையான PyObject *
VimEval(PyObject *self UNUSED, PyObject *args)
{
    char_u	*கோவை;
    typval_T	*our_tv;
    PyObject	*string;
    PyObject	*todecref;
    PyObject	*ret;
    PyObject	*lookup_dict;

    எனில் (!PyArg_ParseTuple(args, "O", &string))
	திரும்பு வெற்று;

    எனில் (!(கோவை = StringToChars(string, &todecref)))
	திரும்பு வெற்று;

    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    VimTryStart();
    our_tv = eval_கோவை(கோவை, வெற்று);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    Py_XDECREF(todecref);

    எனில் (VimTryEnd())
	திரும்பு வெற்று;

    எனில் (our_tv == வெற்று)
    {
	PyErr_SET_VIM(N_("invalid expression"));
	திரும்பு வெற்று;
    }

    // Convert the Vim type into a Python type.  Create a dictionary that's
    // used to check for recursive loops.
    எனில் (!(lookup_dict = PyDict_New()))
	ret = வெற்று;
    இல்லையெனில்
    {
	ret = VimToPython(our_tv, 1, lookup_dict);
	Py_DECREF(lookup_dict);
    }


    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    free_tv(our_tv);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    திரும்பு ret;
}

நிலையான PyObject *ConvertToPyObject(typval_T *);

    நிலையான PyObject *
VimEvalPy(PyObject *self UNUSED, PyObject *string)
{
    typval_T	*our_tv;
    PyObject	*ret;
    char_u	*கோவை;
    PyObject	*todecref;

    எனில் (!(கோவை = StringToChars(string, &todecref)))
	திரும்பு வெற்று;

    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    VimTryStart();
    our_tv = eval_கோவை(கோவை, வெற்று);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    Py_XDECREF(todecref);

    எனில் (VimTryEnd())
	திரும்பு வெற்று;

    எனில் (our_tv == வெற்று)
    {
	PyErr_SET_VIM(N_("invalid expression"));
	திரும்பு வெற்று;
    }

    ret = ConvertToPyObject(our_tv);
    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    free_tv(our_tv);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    திரும்பு ret;
}

    நிலையான PyObject *
VimStrwidth(PyObject *self UNUSED, PyObject *string)
{
    char_u	*str;
    PyObject	*todecref;
    முழு		len;

    எனில் (!(str = StringToChars(string, &todecref)))
	திரும்பு வெற்று;

    len = mb_string2cells(str, (முழு)STRLEN(str));

    Py_XDECREF(todecref);

    திரும்பு PyLong_FromLong(len);
}

    நிலையான PyObject *
_VimChdir(PyObject *_chdir, PyObject *args, PyObject *kwargs)
{
    PyObject	*ret;
    PyObject	*newwd;
    PyObject	*todecref;
    char_u	*new_dir;

    எனில் (_chdir == வெற்று)
	திரும்பு வெற்று;
    எனில் (!(ret = PyObject_Call(_chdir, args, kwargs)))
	திரும்பு வெற்று;

    எனில் (!(newwd = PyObject_CallFunctionObjArgs(py_getcwd, வெற்று)))
    {
	Py_DECREF(ret);
	திரும்பு வெற்று;
    }

    எனில் (!(new_dir = StringToChars(newwd, &todecref)))
    {
	Py_DECREF(ret);
	Py_DECREF(newwd);
	திரும்பு வெற்று;
    }

    VimTryStart();

    எனில் (vim_chdir(new_dir))
    {
	Py_DECREF(ret);
	Py_DECREF(newwd);
	Py_XDECREF(todecref);

	எனில் (VimTryEnd())
	    திரும்பு வெற்று;

	PyErr_SET_VIM(N_("failed to change directory"));
	திரும்பு வெற்று;
    }

    Py_DECREF(newwd);
    Py_XDECREF(todecref);

    post_chdir(CDSCOPE_GLOBAL);

    எனில் (VimTryEnd())
    {
	Py_DECREF(ret);
	திரும்பு வெற்று;
    }

    திரும்பு ret;
}

    நிலையான PyObject *
VimChdir(PyObject *self UNUSED, PyObject *args, PyObject *kwargs)
{
    திரும்பு _VimChdir(py_chdir, args, kwargs);
}

    நிலையான PyObject *
VimFchdir(PyObject *self UNUSED, PyObject *args, PyObject *kwargs)
{
    திரும்பு _VimChdir(py_fchdir, args, kwargs);
}

வகைவரை கட்டமைப்பு {
    PyObject	*callable;
    PyObject	*result;
} map_rtp_data;

    நிலையான வெறுமை
map_rtp_callback(char_u *path, வெறுமை *_data)
{
    வெறுமை	**data = (வெறுமை **) _data;
    PyObject	*pathObject;
    map_rtp_data	*mr_data = *((map_rtp_data **) data);

    எனில் (!(pathObject = PyString_FromString((எழுத்து *)path)))
    {
	*data = வெற்று;
	திரும்பு;
    }

    mr_data->result = PyObject_CallFunctionObjArgs(mr_data->callable,
						   pathObject, வெற்று);

    Py_DECREF(pathObject);

    எனில் (!mr_data->result || mr_data->result != Py_None)
	*data = வெற்று;
    இல்லையெனில்
    {
	Py_DECREF(mr_data->result);
	mr_data->result = வெற்று;
    }
}

    நிலையான PyObject *
VimForeachRTP(PyObject *self UNUSED, PyObject *callable)
{
    map_rtp_data	data;

    data.callable = callable;
    data.result = வெற்று;

    do_in_runtimepath(வெற்று, 0, &map_rtp_callback, &data);

    எனில் (data.result == வெற்று)
    {
	எனில் (PyErr_Occurred())
	    திரும்பு வெற்று;
	இல்லையெனில்
	{
	    Py_INCREF(Py_None);
	    திரும்பு Py_None;
	}
    }
    திரும்பு data.result;
}

/*
 * _vim_runtimepath_ special path implementation.
 */

    நிலையான வெறுமை
map_finder_callback(char_u *path, வெறுமை *_data)
{
    வெறுமை	**data = (வெறுமை **) _data;
    PyObject	*list = *((PyObject **) data);
    PyObject	*pathObject1, *pathObject2;
    எழுத்து	*pathbuf;
    size_t	pathlen;

    pathlen = STRLEN(path);

#எனில் PY_MAJOR_VERSION < 3
# define PY_MAIN_DIR_STRING "python2"
#இல்லையெனில்
# define PY_MAIN_DIR_STRING "python3"
#endif
#define PY_ALTERNATE_DIR_STRING "pythonx"

#define PYTHONX_STRING_LENGTH 7 // STRLEN("pythonx")
    எனில் (!(pathbuf = PyMem_New(எழுத்து,
		    pathlen + STRLEN(PATHSEPSTR) + PYTHONX_STRING_LENGTH + 1)))
    {
	PyErr_NoMemory();
	*data = வெற்று;
	திரும்பு;
    }

    mch_memmove(pathbuf, path, pathlen + 1);
    add_pathsep((char_u *) pathbuf);

    pathlen = STRLEN(pathbuf);
    mch_memmove(pathbuf + pathlen, PY_MAIN_DIR_STRING,
	    PYTHONX_STRING_LENGTH + 1);

    எனில் (!(pathObject1 = PyString_FromString(pathbuf)))
    {
	*data = வெற்று;
	PyMem_Free(pathbuf);
	திரும்பு;
    }

    mch_memmove(pathbuf + pathlen, PY_ALTERNATE_DIR_STRING,
	    PYTHONX_STRING_LENGTH + 1);

    எனில் (!(pathObject2 = PyString_FromString(pathbuf)))
    {
	Py_DECREF(pathObject1);
	PyMem_Free(pathbuf);
	*data = வெற்று;
	திரும்பு;
    }

    PyMem_Free(pathbuf);

    எனில் (PyList_Append(list, pathObject1)
	    || PyList_Append(list, pathObject2))
	*data = வெற்று;

    Py_DECREF(pathObject1);
    Py_DECREF(pathObject2);
}

    நிலையான PyObject *
Vim_GetPaths(PyObject *self UNUSED, PyObject *args UNUSED)
{
    PyObject	*ret;

    எனில் (!(ret = PyList_New(0)))
	திரும்பு வெற்று;

    do_in_runtimepath(வெற்று, 0, &map_finder_callback, ret);

    எனில் (PyErr_Occurred())
    {
	Py_DECREF(ret);
	திரும்பு வெற்று;
    }

    திரும்பு ret;
}

#எனில் PY_VERSION_HEX >= 0x030700f0
    நிலையான PyObject *
FinderFindSpec(PyObject *self, PyObject *args)
{
    எழுத்து	*fullname;
    PyObject	*paths;
    PyObject	*target = Py_None;
    PyObject	*spec;

    எனில் (!PyArg_ParseTuple(args, "s|O", &fullname, &target))
	திரும்பு வெற்று;

    எனில் (!(paths = Vim_GetPaths(self, வெற்று)))
	திரும்பு வெற்று;

    spec = PyObject_CallFunction(py_find_spec, "sOO", fullname, paths, target);

    Py_DECREF(paths);

    எனில் (!spec)
    {
	எனில் (PyErr_Occurred())
	    திரும்பு வெற்று;

	Py_INCREF(Py_None);
	திரும்பு Py_None;
    }

    திரும்பு spec;
}

    நிலையான PyObject *
FinderFindModule(PyObject* self UNUSED, PyObject* args UNUSED)
{
    // Apparently returning None works.
    Py_INCREF(Py_None);
    திரும்பு Py_None;
}
#இல்லையெனில்
    நிலையான PyObject *
call_load_module(எழுத்து *name, முழு len, PyObject *find_module_result)
{
    PyObject	*fd, *pathname, *description;

    எனில் (!PyTuple_Check(find_module_result))
    {
	PyErr_FORMAT_TYPE(
		N_("expected 3-tuple as imp.find_module() result, but got %s"),
		find_module_result);
	திரும்பு வெற்று;
    }
    எனில் (PyTuple_GET_SIZE(find_module_result) != 3)
    {
	PyErr_FORMAT(PyExc_TypeError,
		N_("expected 3-tuple as imp.find_module() result, but got "
		   "tuple of size %d"),
		(முழு) PyTuple_GET_SIZE(find_module_result));
	திரும்பு வெற்று;
    }

    எனில் (!(fd = PyTuple_GET_ITEM(find_module_result, 0))
	    || !(pathname = PyTuple_GET_ITEM(find_module_result, 1))
	    || !(description = PyTuple_GET_ITEM(find_module_result, 2)))
    {
	PyErr_SET_STRING(PyExc_RuntimeError,
		N_("internal error: imp.find_module returned tuple with NULL"));
	திரும்பு வெற்று;
    }

    திரும்பு PyObject_CallFunction(py_load_module,
	    "s#OOO", name, len, fd, pathname, description);
}

    நிலையான PyObject *
find_module(எழுத்து *fullname, எழுத்து *tail, PyObject *new_path)
{
    PyObject	*find_module_result;
    PyObject	*module;
    எழுத்து	*dot;

    எனில் ((dot = (எழுத்து *)vim_strchr((char_u *) tail, '.')))
    {
	/*
	 * There is a dot in the name: call find_module recursively without the
	 * first component
	 */
	PyObject	*newest_path;
	முழு		partlen = (முழு) (dot - 1 - tail);

	எனில் (!(find_module_result = PyObject_CallFunction(py_find_module,
			"s#O", tail, partlen, new_path)))
	{
	    எனில் (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_ImportError))
		PyErr_Clear();
	    திரும்பு வெற்று;
	}

	எனில் (!(module = call_load_module(
			fullname,
			((முழு) (tail - fullname)) + partlen,
			find_module_result)))
	{
	    Py_DECREF(find_module_result);
	    திரும்பு வெற்று;
	}

	Py_DECREF(find_module_result);

	எனில் (!(newest_path = PyObject_GetAttrString(module, "__path__")))
	{
	    Py_DECREF(module);
	    திரும்பு வெற்று;
	}

	Py_DECREF(module);

	find_module_result = find_module(fullname, dot + 1, newest_path);

	Py_DECREF(newest_path);

	திரும்பு find_module_result;
    }
    இல்லையெனில்
    {
	எனில் (!(find_module_result = PyObject_CallFunction(py_find_module,
			"sO", tail, new_path)))
	{
	    எனில் (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_ImportError))
		PyErr_Clear();
	    திரும்பு வெற்று;
	}

	திரும்பு find_module_result;
    }
}

    நிலையான PyObject *
FinderFindModule(PyObject *self, PyObject *args)
{
    எழுத்து	*fullname;
    PyObject	*result;
    PyObject	*new_path;
    LoaderObject	*loader;

    எனில் (!PyArg_ParseTuple(args, "s", &fullname))
	திரும்பு வெற்று;

    எனில் (!(new_path = Vim_GetPaths(self, வெற்று)))
	திரும்பு வெற்று;

    result = find_module(fullname, fullname, new_path);

    Py_DECREF(new_path);

    எனில் (!result)
    {
	எனில் (PyErr_Occurred())
	    திரும்பு வெற்று;

	Py_INCREF(Py_None);
	திரும்பு Py_None;
    }

    எனில் (!(fullname = (எழுத்து *)vim_strsave((char_u *)fullname)))
    {
	Py_DECREF(result);
	PyErr_NoMemory();
	திரும்பு வெற்று;
    }

    எனில் (!(loader = PyObject_NEW(LoaderObject, LoaderTypePtr)))
    {
	vim_free(fullname);
	Py_DECREF(result);
	திரும்பு வெற்று;
    }

    loader->fullname = fullname;
    loader->result = result;

    திரும்பு (PyObject *) loader;
}
#endif

    நிலையான PyObject *
VimPathHook(PyObject *self UNUSED, PyObject *args)
{
    எழுத்து	*path;

    எனில் (PyArg_ParseTuple(args, "s", &path)
	    && STRCMP(path, vim_special_path) == 0)
    {
	Py_INCREF(vim_module);
	திரும்பு vim_module;
    }

    PyErr_Clear();
    PyErr_SetNone(PyExc_ImportError);
    திரும்பு வெற்று;
}

/*
 * Vim module - Definitions
 */

நிலையான கட்டமைப்பு PyMethodDef VimMethods[] = {
    // name,	    function,			calling,			documentation
    {"command",	    VimCommand,			METH_O,				"Execute a Vim ex-mode command" },
    {"eval",	    VimEval,			METH_VARARGS,			"Evaluate an expression using Vim evaluator" },
    {"bindeval",    VimEvalPy,			METH_O,				"Like eval(), but returns objects attached to Vim ones"},
    {"strwidth",    VimStrwidth,		METH_O,				"Screen string width, counts <Tab> as having width 1"},
    {"chdir",	    (PyCFunction)(வெறுமை *)VimChdir,	METH_VARARGS|METH_KEYWORDS,	"Change directory"},
    {"fchdir",	    (PyCFunction)(வெறுமை *)VimFchdir,	METH_VARARGS|METH_KEYWORDS,	"Change directory"},
    {"foreach_rtp", VimForeachRTP,		METH_O,				"Call given callable for each path in &rtp"},
#எனில் PY_VERSION_HEX >= 0x030700f0
    {"find_spec",   FinderFindSpec,		METH_VARARGS,			"Internal use only, returns spec object for any input it receives"},
#endif
    {"find_module", FinderFindModule,		METH_VARARGS,			"Internal use only, returns loader object for any input it receives"},
    {"path_hook",   VimPathHook,		METH_VARARGS,			"Hook function to install in sys.path_hooks"},
    {"_get_paths",  (PyCFunction)Vim_GetPaths,	METH_NOARGS,			"Get &rtp-based additions to sys.path"},
    { வெற்று,	    வெற்று,			0,				வெற்று}
};

/*
 * Generic iterator object
 */

DEFINE_PY_TYPE_OBJECT(IterType);

வகைவரை PyObject *(*nextfun)(வெறுமை **);
வகைவரை வெறுமை (*destructorfun)(வெறுமை *);
வகைவரை முழு (*traversefun)(வெறுமை *, visitproc, வெறுமை *);
வகைவரை முழு (*clearfun)(வெறுமை **);

// Main purpose of this object is removing the need for do python
// initialization (i.e. PyType_Ready and setting type attributes) for a big
// bunch of objects.
வகைவரை கட்டமைப்பு
{
    PyObject_HEAD
    வெறுமை *cur;
    nextfun next;
    destructorfun destruct;
    traversefun traverse;
    clearfun clear;
    PyObject *iter_object;
} IterObject;

    நிலையான PyObject *
IterNew(வெறுமை *start, destructorfun destruct, nextfun next, traversefun traverse,
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

    எனில் (iter_object)
	Py_INCREF(iter_object);

    திரும்பு (PyObject *)(self);
}

    நிலையான வெறுமை
IterDestructor(PyObject *self_obj)
{
    IterObject *self = (IterObject*)self_obj;
    எனில் (self->iter_object)
	Py_DECREF(self->iter_object);
    PyObject_GC_UnTrack((வெறுமை *)(self));
    self->destruct(self->cur);
    PyObject_GC_Del((வெறுமை *)(self));
}

    நிலையான முழு
IterTraverse(PyObject *self_obj, visitproc visit, வெறுமை *arg)
{
    IterObject *self = (IterObject*)self_obj;
    எனில் (self->traverse != வெற்று)
	திரும்பு self->traverse(self->cur, visit, arg);
    இல்லையெனில்
	திரும்பு 0;
}

// Mac OSX defines clear() somewhere.
#ifdef clear
# undef clear
#endif

    நிலையான முழு
IterClear(PyObject *self_obj)
{
    IterObject *self = (IterObject*)self_obj;
    எனில் (self->clear != வெற்று)
	திரும்பு self->clear(&self->cur);
    இல்லையெனில்
	திரும்பு 0;
}

    நிலையான PyObject *
IterNext(PyObject *self_obj)
{
    IterObject *self = (IterObject*)self_obj;
    திரும்பு self->next(&self->cur);
}

    நிலையான PyObject *
IterIter(PyObject *self)
{
    Py_INCREF(self);
    திரும்பு self;
}

வகைவரை கட்டமைப்பு pylinkedlist_S {
    கட்டமைப்பு pylinkedlist_S	*pll_next;
    கட்டமைப்பு pylinkedlist_S	*pll_prev;
    PyObject			*pll_obj;
} pylinkedlist_T;

நிலையான pylinkedlist_T *lastdict = வெற்று;
நிலையான pylinkedlist_T *lastlist = வெற்று;
நிலையான pylinkedlist_T *lasttuple = வெற்று;
நிலையான pylinkedlist_T *lastfunc = வெற்று;

    நிலையான வெறுமை
pyll_remove(pylinkedlist_T *ref, pylinkedlist_T **last)
{
    எனில் (ref->pll_prev == வெற்று)
    {
	எனில் (ref->pll_next == வெற்று)
	{
	    *last = வெற்று;
	    திரும்பு;
	}
    }
    இல்லையெனில்
	ref->pll_prev->pll_next = ref->pll_next;

    எனில் (ref->pll_next == வெற்று)
	*last = ref->pll_prev;
    இல்லையெனில்
	ref->pll_next->pll_prev = ref->pll_prev;
}

    நிலையான வெறுமை
pyll_add(PyObject *self, pylinkedlist_T *ref, pylinkedlist_T **last)
{
    எனில் (*last == வெற்று)
	ref->pll_prev = வெற்று;
    இல்லையெனில்
    {
	(*last)->pll_next = ref;
	ref->pll_prev = *last;
    }
    ref->pll_next = வெற்று;
    ref->pll_obj = self;
    *last = ref;
}

DEFINE_PY_TYPE_OBJECT(DictionaryType);

வகைவரை கட்டமைப்பு
{
    PyObject_HEAD
    dict_T	*dict;
    pylinkedlist_T	ref;
} DictionaryObject;

நிலையான PyObject *DictionaryUpdate(DictionaryObject *, PyObject *, PyObject *);

#define NEW_DICTIONARY(dict) DictionaryNew(DictionaryTypePtr, dict)

    நிலையான PyObject *
DictionaryNew(PyTypeObject *subtype, dict_T *dict)
{
    DictionaryObject	*self;

    self = (DictionaryObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    எனில் (self == வெற்று)
	திரும்பு வெற்று;
    self->dict = dict;
    ++dict->dv_refcount;

    pyll_add((PyObject *)(self), &self->ref, &lastdict);

    திரும்பு (PyObject *)(self);
}

    நிலையான dict_T *
py_dict_alloc(வெறுமை)
{
    dict_T	*ret;

    எனில் (!(ret = dict_alloc()))
    {
	PyErr_NoMemory();
	திரும்பு வெற்று;
    }
    ++ret->dv_refcount;

    திரும்பு ret;
}

    நிலையான PyObject *
DictionaryConstructor(PyTypeObject *subtype, PyObject *args, PyObject *kwargs)
{
    DictionaryObject	*self;
    dict_T	*dict;

    எனில் (!(dict = py_dict_alloc()))
	திரும்பு வெற்று;

    self = (DictionaryObject *) DictionaryNew(subtype, dict);

    --dict->dv_refcount;

    எனில் (kwargs || PyTuple_Size(args))
    {
	PyObject	*tmp;
	எனில் (!(tmp = DictionaryUpdate(self, args, kwargs)))
	{
	    Py_DECREF(self);
	    திரும்பு வெற்று;
	}

	Py_DECREF(tmp);
    }

    திரும்பு (PyObject *)(self);
}

    நிலையான வெறுமை
DictionaryDestructor(PyObject *self_obj)
{
    DictionaryObject *self = (DictionaryObject*)self_obj;
    pyll_remove(&self->ref, &lastdict);
    dict_unref(self->dict);

    DESTRUCTOR_FINISH(self);
}

நிலையான எழுத்து *DictionaryAttrs[] = {
    "locked", "scope",
    வெற்று
};

    நிலையான PyObject *
DictionaryDir(PyObject *self, PyObject *args UNUSED)
{
    திரும்பு ObjectDir(self, DictionaryAttrs);
}

    நிலையான முழு
DictionarySetattr(PyObject *self_obj, எழுத்து *name, PyObject *valObject)
{
    DictionaryObject *self = (DictionaryObject*)self_obj;
    எனில் (valObject == வெற்று)
    {
	PyErr_SET_STRING(PyExc_AttributeError,
		N_("cannot delete vim.Dictionary attributes"));
	திரும்பு -1;
    }

    எனில் (strcmp(name, "locked") == 0)
    {
	எனில் (self->dict->dv_lock == VAR_FIXED)
	{
	    PyErr_SET_STRING(PyExc_TypeError,
		    N_("cannot modify fixed dictionary"));
	    திரும்பு -1;
	}
	இல்லையெனில்
	{
	    முழு		istrue = PyObject_IsTrue(valObject);
	    எனில் (istrue == -1)
		திரும்பு -1;
	    இல்லையெனில் எனில் (istrue)
		self->dict->dv_lock = VAR_LOCKED;
	    இல்லையெனில்
		self->dict->dv_lock = 0;
	}
	திரும்பு 0;
    }
    இல்லையெனில்
    {
	PyErr_FORMAT(PyExc_AttributeError, N_("cannot set attribute %s"), name);
	திரும்பு -1;
    }
}

    நிலையான PyInt
DictionaryLength(DictionaryObject *self)
{
    திரும்பு ((PyInt) (self->dict->dv_hashtab.ht_used));
}

#define DICT_FLAG_HAS_DEFAULT	0x01
#define DICT_FLAG_எடு		0x02
#define DICT_FLAG_NONE_DEFAULT	0x04
#define DICT_FLAG_RETURN_BOOL	0x08 // Incompatible with DICT_FLAG_POP
#define DICT_FLAG_RETURN_PAIR	0x10

    நிலையான PyObject *
_DictionaryItem(DictionaryObject *self, PyObject *args, முழு flags)
{
    PyObject	*சாவி_Object;
    PyObject	*defObject = ((flags & DICT_FLAG_NONE_DEFAULT)? Py_None : வெற்று);
    PyObject	*ret;
    char_u	*சாவி;
    dictitem_T	*di;
    dict_T	*dict = self->dict;
    hashitem_T	*hi;
    PyObject	*todecref;

    எனில் (flags & DICT_FLAG_HAS_DEFAULT)
    {
	எனில் (!PyArg_ParseTuple(args, "O|O", &சாவி_Object, &defObject))
	    திரும்பு வெற்று;
    }
    இல்லையெனில்
	சாவி_Object = args;

    எனில் (flags & DICT_FLAG_RETURN_BOOL)
	defObject = Py_False;

    எனில் (!(சாவி = StringToChars(சாவி_Object, &todecref)))
	திரும்பு வெற்று;

    எனில் (*சாவி == NUL)
    {
	RAISE_NO_EMPTY_சாவிகள்;
	Py_XDECREF(todecref);
	திரும்பு வெற்று;
    }

    hi = hash_find(&dict->dv_hashtab, சாவி);

    Py_XDECREF(todecref);

    எனில் (HASHITEM_EMPTY(hi))
    {
	எனில் (defObject)
	{
	    Py_INCREF(defObject);
	    திரும்பு defObject;
	}
	இல்லையெனில்
	{
	    PyErr_SetObject(Py_Exc_சாவி_Error, சாவி_Object);
	    திரும்பு வெற்று;
	}
    }
    இல்லையெனில் எனில் (flags & DICT_FLAG_RETURN_BOOL)
    {
	ret = Py_True;
	Py_INCREF(ret);
	திரும்பு ret;
    }

    di = dict_lookup(hi);

    எனில் (!(ret = ConvertToPyObject(&di->di_tv)))
	திரும்பு வெற்று;

    எனில் (flags & DICT_FLAG_எடு)
    {
	எனில் (dict->dv_lock)
	{
	    RAISE_LOCKED_DICTIONARY;
	    Py_DECREF(ret);
	    திரும்பு வெற்று;
	}

	hash_remove(&dict->dv_hashtab, hi, "Python remove variable");
	dictitem_free(di);
    }

    திரும்பு ret;
}

    நிலையான PyObject *
DictionaryItem(DictionaryObject *self, PyObject *சாவி_Object)
{
    திரும்பு _DictionaryItem(self, சாவி_Object, 0);
}

    நிலையான முழு
DictionaryContains(DictionaryObject *self, PyObject *சாவி_Object)
{
    PyObject	*rObj = _DictionaryItem(self, சாவி_Object, DICT_FLAG_RETURN_BOOL);
    முழு		ret;

    எனில் (rObj == வெற்று)
	திரும்பு -1;

    ret = (rObj == Py_True);

    Py_DECREF(rObj);

    திரும்பு ret;
}

வகைவரை கட்டமைப்பு
{
    முழு		dii_changed;
    hashtab_T	*dii_ht;
    hashitem_T	*dii_hi;
    long_u	dii_todo;
} dictiterinfo_T;

    நிலையான PyObject *
DictionaryIterNext(வெறுமை **arg)
{
    PyObject	*ret;
    dictiterinfo_T **dii = (dictiterinfo_T**)arg;

    எனில் (!(*dii)->dii_todo)
	திரும்பு வெற்று;

    எனில் ((*dii)->dii_ht->ht_changed != (*dii)->dii_changed)
    {
	PyErr_SET_STRING(PyExc_RuntimeError,
		N_("hashtab changed during iteration"));
	திரும்பு வெற்று;
    }

    போது (((*dii)->dii_todo) && HASHITEM_EMPTY((*dii)->dii_hi))
	++((*dii)->dii_hi);

    --((*dii)->dii_todo);

    எனில் (!(ret = PyBytes_FromString((எழுத்து *)(*dii)->dii_hi->hi_சாவி)))
	திரும்பு வெற்று;

    திரும்பு ret;
}

    நிலையான PyObject *
DictionaryIter(PyObject *self_obj)
{
    DictionaryObject	*self = (DictionaryObject*)self_obj;
    dictiterinfo_T	*dii;
    hashtab_T		*ht;

    எனில் (!(dii = PyMem_New(dictiterinfo_T, 1)))
    {
	PyErr_NoMemory();
	திரும்பு வெற்று;
    }

    ht = &self->dict->dv_hashtab;
    dii->dii_changed = ht->ht_changed;
    dii->dii_ht = ht;
    dii->dii_hi = ht->ht_array;
    dii->dii_todo = ht->ht_used;

    திரும்பு IterNew(dii,
	    PyMem_Free, DictionaryIterNext,
	    வெற்று, வெற்று, (PyObject *)self);
}

    நிலையான முழு
DictionaryAssItem(
	DictionaryObject *self, PyObject *சாவி_Object, PyObject *valObject)
{
    char_u	*சாவி;
    typval_T	tv;
    dict_T	*dict = self->dict;
    dictitem_T	*di;
    PyObject	*todecref;

    எனில் (dict->dv_lock)
    {
	RAISE_LOCKED_DICTIONARY;
	திரும்பு -1;
    }

    எனில் (!(சாவி = StringToChars(சாவி_Object, &todecref)))
	திரும்பு -1;

    எனில் (*சாவி == NUL)
    {
	RAISE_NO_EMPTY_சாவிகள்;
	Py_XDECREF(todecref);
	திரும்பு -1;
    }

    di = dict_find(dict, சாவி, -1);

    எனில் (valObject == வெற்று)
    {
	hashitem_T	*hi;

	எனில் (di == வெற்று)
	{
	    Py_XDECREF(todecref);
	    PyErr_SetObject(Py_Exc_சாவி_Error, சாவி_Object);
	    திரும்பு -1;
	}
	hi = hash_find(&dict->dv_hashtab, di->di_சாவி);
	hash_remove(&dict->dv_hashtab, hi, "Python remove item");
	dictitem_free(di);
	Py_XDECREF(todecref);
	திரும்பு 0;
    }

    எனில் (ConvertFromPyObject(valObject, &tv) == -1)
    {
	Py_XDECREF(todecref);
	திரும்பு -1;
    }

    எனில் (di == வெற்று)
    {
	எனில் (!(di = dictitem_alloc(சாவி)))
	{
	    Py_XDECREF(todecref);
	    PyErr_NoMemory();
	    திரும்பு -1;
	}
	di->di_tv.v_type = VAR_UNKNOWN;

	எனில் (dict_add(dict, di) == FAIL)
	{
	    dictitem_free(di);
	    RAISE_சாவி_ADD_FAIL(சாவி);
	    Py_XDECREF(todecref);
	    திரும்பு -1;
	}
    }
    இல்லையெனில்
	clear_tv(&di->di_tv);

    Py_XDECREF(todecref);

    copy_tv(&tv, &di->di_tv);
    clear_tv(&tv);
    திரும்பு 0;
}

வகைவரை PyObject *(*hi_to_py)(hashitem_T *);

    நிலையான PyObject *
DictionaryListObjects(DictionaryObject *self, hi_to_py hiconvert)
{
    dict_T	*dict = self->dict;
    long_u	todo = dict->dv_hashtab.ht_used;
    Py_ssize_t	i = 0;
    PyObject	*ret;
    hashitem_T	*hi;
    PyObject	*newObj;

    ret = PyList_New(todo);
    க்கு (hi = dict->dv_hashtab.ht_array; todo > 0; ++hi)
    {
	எனில் (!HASHITEM_EMPTY(hi))
	{
	    எனில் (!(newObj = hiconvert(hi)))
	    {
		Py_DECREF(ret);
		திரும்பு வெற்று;
	    }
	    PyList_SET_ITEM(ret, i, newObj);
	    --todo;
	    ++i;
	}
    }
    திரும்பு ret;
}

    நிலையான PyObject *
dict_சாவி(hashitem_T *hi)
{
    திரும்பு PyBytes_FromString((எழுத்து *)(hi->hi_சாவி));
}

    நிலையான PyObject *
Dictionary_List_சாவிகள்(DictionaryObject *self, PyObject *args UNUSED)
{
    திரும்பு DictionaryListObjects(self, dict_சாவி);
}

    நிலையான PyObject *
dict_val(hashitem_T *hi)
{
    dictitem_T	*di;

    di = dict_lookup(hi);
    திரும்பு ConvertToPyObject(&di->di_tv);
}

    நிலையான PyObject *
Dictionary_List_மதிப்புகள்(DictionaryObject *self, PyObject *args UNUSED)
{
    திரும்பு DictionaryListObjects(self, dict_val);
}

    நிலையான PyObject *
dict_item(hashitem_T *hi)
{
    PyObject	*சாவி_Object;
    PyObject	*valObject;
    PyObject	*ret;

    எனில் (!(சாவி_Object = dict_சாவி(hi)))
	திரும்பு வெற்று;

    எனில் (!(valObject = dict_val(hi)))
    {
	Py_DECREF(சாவி_Object);
	திரும்பு வெற்று;
    }

    ret = Py_Build_மதிப்பு("(OO)", சாவி_Object, valObject);

    Py_DECREF(சாவி_Object);
    Py_DECREF(valObject);

    திரும்பு ret;
}

    நிலையான PyObject *
Dictionary_List_பொருட்கள்(DictionaryObject *self, PyObject *args UNUSED)
{
    திரும்பு DictionaryListObjects(self, dict_item);
}

    நிலையான PyObject *
DictionaryUpdate(DictionaryObject *self, PyObject *args, PyObject *kwargs)
{
    dict_T	*dict = self->dict;

    எனில் (dict->dv_lock)
    {
	RAISE_LOCKED_DICTIONARY;
	திரும்பு வெற்று;
    }

    எனில் (kwargs)
    {
	typval_T	tv;

	எனில் (ConvertFromPyMapping(kwargs, &tv) == -1)
	    திரும்பு வெற்று;

	VimTryStart();
	dict_extend(self->dict, tv.vval.v_dict, (char_u *) "force", வெற்று);
	clear_tv(&tv);
	எனில் (VimTryEnd())
	    திரும்பு வெற்று;
    }
    இல்லையெனில்
    {
	PyObject	*obj = வெற்று;

	எனில் (!PyArg_ParseTuple(args, "|O", &obj))
	    திரும்பு வெற்று;

	எனில் (obj == வெற்று)
	{
	    Py_INCREF(Py_None);
	    திரும்பு Py_None;
	}

	எனில் (PyObject_HasAttrString(obj, "keys"))
	    திரும்பு DictionaryUpdate(self, வெற்று, obj);
	இல்லையெனில்
	{
	    PyObject	*iterator;
	    PyObject	*item;

	    எனில் (!(iterator = PyObject_GetIter(obj)))
		திரும்பு வெற்று;

	    போது ((item = PyIter_Next(iterator)))
	    {
		PyObject	*fast;
		PyObject	*சாவி_Object;
		PyObject	*valObject;
		PyObject	*todecref;
		char_u		*சாவி;
		dictitem_T	*di;
		hashitem_T	*hi;

		எனில் (!(fast = PySequence_Fast(item, "")))
		{
		    Py_DECREF(iterator);
		    Py_DECREF(item);
		    திரும்பு வெற்று;
		}

		Py_DECREF(item);

		எனில் (PySequence_Size(fast) != 2)
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    PyErr_FORMAT(Py_Exc_மதிப்பு_Error,
			    N_("expected sequence element of size 2, "
			    "but got sequence of size %d"),
			    (முழு) PySequence_Size(fast));
		    திரும்பு வெற்று;
		}

		சாவி_Object = PySequence_GetItem(fast, 0);

		எனில் (!(சாவி = StringToChars(சாவி_Object, &todecref)))
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    திரும்பு வெற்று;
		}

		di = dictitem_alloc(சாவி);

		Py_XDECREF(todecref);

		எனில் (di == வெற்று)
		{
		    Py_DECREF(fast);
		    Py_DECREF(iterator);
		    PyErr_NoMemory();
		    திரும்பு வெற்று;
		}
		di->di_tv.v_type = VAR_UNKNOWN;

		valObject = PySequence_GetItem(fast, 1);

		எனில் (ConvertFromPyObject(valObject, &di->di_tv) == -1)
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    dictitem_free(di);
		    திரும்பு வெற்று;
		}

		Py_DECREF(fast);

		hi = hash_find(&dict->dv_hashtab, di->di_சாவி);
		எனில் (!HASHITEM_EMPTY(hi) || dict_add(dict, di) == FAIL)
		{
		    RAISE_சாவி_ADD_FAIL(di->di_சாவி);
		    Py_DECREF(iterator);
		    dictitem_free(di);
		    திரும்பு வெற்று;
		}
	    }

	    Py_DECREF(iterator);

	    // Iterator may have finished due to an exception
	    எனில் (PyErr_Occurred())
		திரும்பு வெற்று;
	}
    }
    Py_INCREF(Py_None);
    திரும்பு Py_None;
}

    நிலையான PyObject *
DictionaryGet(DictionaryObject *self, PyObject *args)
{
    திரும்பு _DictionaryItem(self, args,
			    DICT_FLAG_HAS_DEFAULT|DICT_FLAG_NONE_DEFAULT);
}

    நிலையான PyObject *
Dictionary_எடு(DictionaryObject *self, PyObject *args)
{
    திரும்பு _DictionaryItem(self, args, DICT_FLAG_HAS_DEFAULT|DICT_FLAG_எடு);
}

    நிலையான PyObject *
Dictionary_எடு_Item(DictionaryObject *self, PyObject *args UNUSED)
{
    hashitem_T	*hi;
    PyObject	*ret;
    PyObject	*valObject;
    dictitem_T	*di;

    எனில் (self->dict->dv_hashtab.ht_used == 0)
    {
	PyErr_SetNone(Py_Exc_சாவி_Error);
	திரும்பு வெற்று;
    }

    hi = self->dict->dv_hashtab.ht_array;
    போது (HASHITEM_EMPTY(hi))
	++hi;

    di = dict_lookup(hi);

    எனில் (!(valObject = ConvertToPyObject(&di->di_tv)))
	திரும்பு வெற்று;

    எனில் (!(ret = Py_Build_மதிப்பு("(" Py_bytes_fmt "O)", hi->hi_சாவி, valObject)))
    {
	Py_DECREF(valObject);
	திரும்பு வெற்று;
    }

    hash_remove(&self->dict->dv_hashtab, hi, "Python pop item");
    dictitem_free(di);

    திரும்பு ret;
}

    நிலையான PyObject *
Dictionary_Has_சாவி(DictionaryObject *self, PyObject *சாவி_Object)
{
    திரும்பு _DictionaryItem(self, சாவி_Object, DICT_FLAG_RETURN_BOOL);
}

நிலையான PySequenceMethods DictionaryAsSeq = {
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

நிலையான PyMappingMethods DictionaryAsMapping = {
    (lenfunc)       DictionaryLength,
    (binaryfunc)    DictionaryItem,
    (objobjargproc) DictionaryAssItem,
};

நிலையான கட்டமைப்பு PyMethodDef DictionaryMethods[] = {
    {"keys",	(PyCFunction)Dictionary_List_சாவிகள்,	METH_NOARGS,	""},
    {"values",	(PyCFunction)Dictionary_List_மதிப்புகள்,	METH_NOARGS,	""},
    {"items",	(PyCFunction)Dictionary_List_பொருட்கள்,	METH_NOARGS,	""},
    {"update",	(PyCFunction)(வெறுமை *)DictionaryUpdate,		METH_VARARGS|METH_KEYWORDS, ""},
    {"get",	(PyCFunction)DictionaryGet,		METH_VARARGS,	""},
    {"pop",	(PyCFunction)Dictionary_எடு,		METH_VARARGS,	""},
    {"popitem",	(PyCFunction)Dictionary_எடு_Item,		METH_NOARGS,	""},
    {"has_key",	(PyCFunction)Dictionary_Has_சாவி,		METH_O,		""},
    {"__dir__",	(PyCFunction)DictionaryDir,		METH_NOARGS,	""},
    { வெற்று,	வெற்று,					0,		வெற்று}
};

DEFINE_PY_TYPE_OBJECT(ListType);

வகைவரை கட்டமைப்பு
{
    PyObject_HEAD
    list_T	*list;
    pylinkedlist_T	ref;
} ListObject;

#define NEW_LIST(list) ListNew(ListTypePtr, list)

    நிலையான PyObject *
ListNew(PyTypeObject *subtype, list_T *list)
{
    ListObject	*self;

    எனில் (list == வெற்று)
	திரும்பு வெற்று;

    self = (ListObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    எனில் (self == வெற்று)
	திரும்பு வெற்று;
    self->list = list;
    ++list->lv_refcount;
    CHECK_LIST_MATERIALIZE(list);

    pyll_add((PyObject *)(self), &self->ref, &lastlist);

    திரும்பு (PyObject *)(self);
}

    நிலையான list_T *
py_list_alloc(வெறுமை)
{
    list_T	*ret;

    எனில் (!(ret = list_alloc()))
    {
	PyErr_NoMemory();
	திரும்பு வெற்று;
    }
    ++ret->lv_refcount;

    திரும்பு ret;
}

    நிலையான முழு
list_py_concat(list_T *l, PyObject *obj, PyObject *lookup_dict)
{
    PyObject	*iterator;
    PyObject	*item;
    listitem_T	*li;

    எனில் (!(iterator = PyObject_GetIter(obj)))
	திரும்பு -1;

    போது ((item = PyIter_Next(iterator)))
    {
	எனில் (!(li = listitem_alloc()))
	{
	    PyErr_NoMemory();
	    Py_DECREF(item);
	    Py_DECREF(iterator);
	    திரும்பு -1;
	}
	li->li_tv.v_lock = 0;
	li->li_tv.v_type = VAR_UNKNOWN;

	எனில் (_ConvertFromPyObject(item, &li->li_tv, lookup_dict) == -1)
	{
	    Py_DECREF(item);
	    Py_DECREF(iterator);
	    listitem_free(l, li);
	    திரும்பு -1;
	}

	Py_DECREF(item);

	list_append(l, li);
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    எனில் (PyErr_Occurred())
	திரும்பு -1;

    திரும்பு 0;
}

    நிலையான PyObject *
ListConstructor(PyTypeObject *subtype, PyObject *args, PyObject *kwargs)
{
    list_T	*list;
    PyObject	*obj = வெற்று;

    எனில் (kwargs)
    {
	PyErr_SET_STRING(PyExc_TypeError,
		N_("list constructor does not accept keyword arguments"));
	திரும்பு வெற்று;
    }

    எனில் (!PyArg_ParseTuple(args, "|O", &obj))
	திரும்பு வெற்று;

    எனில் (!(list = py_list_alloc()))
	திரும்பு வெற்று;

    எனில் (obj)
    {
	PyObject	*lookup_dict;

	எனில் (!(lookup_dict = PyDict_New()))
	{
	    list_unref(list);
	    திரும்பு வெற்று;
	}

	எனில் (list_py_concat(list, obj, lookup_dict) == -1)
	{
	    Py_DECREF(lookup_dict);
	    list_unref(list);
	    திரும்பு வெற்று;
	}

	Py_DECREF(lookup_dict);
    }

    திரும்பு ListNew(subtype, list);
}

    நிலையான வெறுமை
ListDestructor(PyObject *self_obj)
{
    ListObject *self = (ListObject*)self_obj;
    pyll_remove(&self->ref, &lastlist);
    list_unref(self->list);

    DESTRUCTOR_FINISH(self);
}

    நிலையான PyInt
ListLength(ListObject *self)
{
    திரும்பு ((PyInt) (self->list->lv_len));
}

    நிலையான PyObject *
ListIndex(ListObject *self, Py_ssize_t index)
{
    listitem_T	*li;

    எனில் (index >= ListLength(self))
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("list index out of range"));
	திரும்பு வெற்று;
    }
    li = list_find(self->list, (long) index);
    எனில் (li == வெற்று)
    {
	// No more suitable format specifications in python-2.3
	PyErr_VIM_FORMAT(N_("internal error: failed to get Vim list item %d"),
		(முழு) index);
	திரும்பு வெற்று;
    }
    திரும்பு ConvertToPyObject(&li->li_tv);
}

    நிலையான PyObject *
ListSlice(ListObject *self, Py_ssize_t first, Py_ssize_t step,
	  Py_ssize_t slicelen)
{
    PyInt	i;
    PyObject	*list;

    எனில் (step == 0)
    {
	PyErr_SET_STRING(Py_Exc_மதிப்பு_Error, N_("slice step cannot be zero"));
	திரும்பு வெற்று;
    }

    list = PyList_New(slicelen);
    எனில் (list == வெற்று)
	திரும்பு வெற்று;

    க்கு (i = 0; i < slicelen; ++i)
    {
	PyObject	*item;

	item = ListIndex(self, first + i*step);
	எனில் (item == வெற்று)
	{
	    Py_DECREF(list);
	    திரும்பு வெற்று;
	}

	PyList_SET_ITEM(list, i, item);
    }

    திரும்பு list;
}

    நிலையான PyObject *
ListItem(ListObject *self, PyObject* idx)
{
#எனில் PY_MAJOR_VERSION < 3
    எனில் (PyInt_Check(idx))
    {
	long _idx = PyInt_AsLong(idx);
	திரும்பு ListIndex(self, _idx);
    }
    இல்லையெனில்
#endif
    எனில் (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	திரும்பு ListIndex(self, _idx);
    }
    இல்லையெனில் எனில் (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	எனில் (PySlice_GetIndicesEx((PySliceObject_T *)idx, ListLength(self),
				 &start, &stop, &step, &slicelen) < 0)
	    திரும்பு வெற்று;
	திரும்பு ListSlice(self, start, step, slicelen);
    }
    இல்லையெனில்
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	திரும்பு வெற்று;
    }
}

    நிலையான வெறுமை
list_restore(Py_ssize_t numadded, Py_ssize_t numreplaced, Py_ssize_t slicelen,
	list_T *l, listitem_T **lis, listitem_T *lastaddedli)
{
    போது (numreplaced--)
    {
	list_செருகு(l, lis[numreplaced], lis[slicelen + numreplaced]);
	listitem_remove(l, lis[slicelen + numreplaced]);
    }
    போது (numadded--)
    {
	listitem_T	*next;

	next = lastaddedli->li_prev;
	listitem_remove(l, lastaddedli);
	lastaddedli = next;
    }
}

    நிலையான முழு
ListAssSlice(ListObject *self, Py_ssize_t first,
	     Py_ssize_t step, Py_ssize_t slicelen, PyObject *obj)
{
    PyObject	*iterator;
    PyObject	*item;
    listitem_T	*li;
    listitem_T	*lastaddedli = வெற்று;
    listitem_T	*next;
    typval_T	v;
    list_T	*l = self->list;
    PyInt	i;
    PyInt	j;
    PyInt	numreplaced = 0;
    PyInt	numadded = 0;
    PyInt	size;
    listitem_T	**lis = வெற்று;

    size = ListLength(self);

    எனில் (l->lv_lock)
    {
	RAISE_LOCKED_LIST;
	திரும்பு -1;
    }

    எனில் (step == 0)
    {
	PyErr_SET_STRING(Py_Exc_மதிப்பு_Error, N_("slice step cannot be zero"));
	திரும்பு -1;
    }

    எனில் (step != 1 && slicelen == 0)
    {
	// Nothing to do. Only error out if obj has some items.
	முழு		ret = 0;

	எனில் (obj == வெற்று)
	    திரும்பு 0;

	எனில் (!(iterator = PyObject_GetIter(obj)))
	    திரும்பு -1;

	எனில் ((item = PyIter_Next(iterator)))
	{
	    PyErr_FORMAT(Py_Exc_மதிப்பு_Error,
		    N_("attempt to assign sequence of size greater than %d "
			"to extended slice"), 0);
	    Py_DECREF(item);
	    ret = -1;
	}
	Py_DECREF(iterator);
	திரும்பு ret;
    }

    எனில் (obj != வெற்று)
	// XXX May allocate zero bytes.
	எனில் (!(lis = PyMem_New(listitem_T *, slicelen * 2)))
	{
	    PyErr_NoMemory();
	    திரும்பு -1;
	}

    எனில் (first == size)
	li = வெற்று;
    இல்லையெனில்
    {
	li = list_find(l, (long) first);
	எனில் (li == வெற்று)
	{
	    PyErr_VIM_FORMAT(N_("internal error: no Vim list item %d"),
		    (முழு)first);
	    எனில் (obj != வெற்று)
		PyMem_Free(lis);
	    திரும்பு -1;
	}
	i = slicelen;
	போது (i-- && li != வெற்று)
	{
	    j = step;
	    next = li;
	    எனில் (step > 0)
		போது (next != வெற்று && ((next = next->li_next) != வெற்று) && --j);
	    இல்லையெனில்
		போது (next != வெற்று && ((next = next->li_prev) != வெற்று) && ++j);

	    எனில் (obj == வெற்று)
		listitem_remove(l, li);
	    இல்லையெனில்
		lis[slicelen - i - 1] = li;

	    li = next;
	}
	எனில் (li == வெற்று && i != -1)
	{
	    PyErr_SET_VIM(N_("internal error: not enough list items"));
	    எனில் (obj != வெற்று)
		PyMem_Free(lis);
	    திரும்பு -1;
	}
    }

    எனில் (obj == வெற்று)
	திரும்பு 0;

    எனில் (!(iterator = PyObject_GetIter(obj)))
    {
	PyMem_Free(lis);
	திரும்பு -1;
    }

    i = 0;
    போது ((item = PyIter_Next(iterator)))
    {
	எனில் (ConvertFromPyObject(item, &v) == -1)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(item);
	    PyMem_Free(lis);
	    திரும்பு -1;
	}
	Py_DECREF(item);
	எனில் (list_செருகு_tv(l, &v, numreplaced < slicelen
				    ? lis[numreplaced]
				    : li) == FAIL)
	{
	    clear_tv(&v);
	    PyErr_SET_VIM(N_("internal error: failed to add item to list"));
	    list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	    PyMem_Free(lis);
	    திரும்பு -1;
	}
	எனில் (numreplaced < slicelen)
	{
	    lis[slicelen + numreplaced] = lis[numreplaced]->li_prev;
	    vimlist_remove(l, lis[numreplaced], lis[numreplaced]);
	    numreplaced++;
	}
	இல்லையெனில்
	{
	    எனில் (li)
		lastaddedli = li->li_prev;
	    இல்லையெனில்
		lastaddedli = l->lv_u.mat.lv_last;
	    numadded++;
	}
	clear_tv(&v);
	எனில் (step != 1 && i >= slicelen)
	{
	    Py_DECREF(iterator);
	    PyErr_FORMAT(Py_Exc_மதிப்பு_Error,
		    N_("attempt to assign sequence of size greater than %d "
			"to extended slice"), (முழு) slicelen);
	    list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	    PyMem_Free(lis);
	    திரும்பு -1;
	}
	++i;
    }
    Py_DECREF(iterator);

    எனில் (step != 1 && i != slicelen)
    {
	PyErr_FORMAT2(Py_Exc_மதிப்பு_Error,
		N_("attempt to assign sequence of size %d to extended slice "
		    "of size %d"), (முழு) i, (முழு) slicelen);
	list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	PyMem_Free(lis);
	திரும்பு -1;
    }

    எனில் (PyErr_Occurred())
    {
	list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	PyMem_Free(lis);
	திரும்பு -1;
    }

    க்கு (i = 0; i < numreplaced; i++)
	listitem_free(l, lis[i]);
    எனில் (step == 1)
	க்கு (i = numreplaced; i < slicelen; i++)
	    listitem_remove(l, lis[i]);

    PyMem_Free(lis);

    திரும்பு 0;
}

    நிலையான முழு
ListAssIndex(ListObject *self, Py_ssize_t index, PyObject *obj)
{
    typval_T	tv;
    list_T	*l = self->list;
    listitem_T	*li;
    Py_ssize_t	length = ListLength(self);

    எனில் (l->lv_lock)
    {
	RAISE_LOCKED_LIST;
	திரும்பு -1;
    }
    எனில் (index > length || (index == length && obj == வெற்று))
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("list index out of range"));
	திரும்பு -1;
    }

    எனில் (obj == வெற்று)
    {
	li = list_find(l, (long) index);
	எனில் (li == வெற்று)
	{
	    PyErr_VIM_FORMAT(N_("internal error: failed to get Vim "
			"list item %d"), (முழு) index);
	    திரும்பு -1;
	}
	vimlist_remove(l, li, li);
	clear_tv(&li->li_tv);
	vim_free(li);
	திரும்பு 0;
    }

    எனில் (ConvertFromPyObject(obj, &tv) == -1)
	திரும்பு -1;

    எனில் (index == length)
    {
	எனில் (list_append_tv(l, &tv) == FAIL)
	{
	    clear_tv(&tv);
	    PyErr_SET_VIM(N_("failed to add item to list"));
	    திரும்பு -1;
	}
    }
    இல்லையெனில்
    {
	li = list_find(l, (long) index);
	எனில் (li == வெற்று)
	{
	    PyErr_VIM_FORMAT(N_("internal error: failed to get Vim "
			"list item %d"), (முழு) index);
	    திரும்பு -1;
	}
	clear_tv(&li->li_tv);
	copy_tv(&tv, &li->li_tv);
	clear_tv(&tv);
    }
    திரும்பு 0;
}

    நிலையான முழு
ListAssItem(ListObject *self, PyObject *idx, PyObject *obj)
{
#எனில் PY_MAJOR_VERSION < 3
    எனில் (PyInt_Check(idx))
    {
	long _idx = PyInt_AsLong(idx);
	திரும்பு (முழு)ListAssIndex(self, _idx, obj);
    }
    இல்லையெனில்
#endif
    எனில் (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	திரும்பு (முழு)ListAssIndex(self, _idx, obj);
    }
    இல்லையெனில் எனில் (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	எனில் (PySlice_GetIndicesEx((PySliceObject_T *)idx, ListLength(self),
				 &start, &stop, &step, &slicelen) < 0)
	    திரும்பு -1;
	திரும்பு (முழு)ListAssSlice(self, start, step, slicelen,
		obj);
    }
    இல்லையெனில்
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	திரும்பு -1;
    }
}

    நிலையான PyObject *
ListConcatInPlace(ListObject *self, PyObject *obj)
{
    list_T	*l = self->list;
    PyObject	*lookup_dict;

    எனில் (l->lv_lock)
    {
	RAISE_LOCKED_LIST;
	திரும்பு வெற்று;
    }

    எனில் (!(lookup_dict = PyDict_New()))
	திரும்பு வெற்று;

    எனில் (list_py_concat(l, obj, lookup_dict) == -1)
    {
	Py_DECREF(lookup_dict);
	திரும்பு வெற்று;
    }
    Py_DECREF(lookup_dict);

    Py_INCREF((PyObject *)self);
    திரும்பு (PyObject *)(self);
}

வகைவரை கட்டமைப்பு
{
    listwatch_T	lw;
    list_T	*list;
} listiterinfo_T;

    நிலையான வெறுமை
ListIterDestruct(வெறுமை *arg)
{
    listiterinfo_T *lii = (listiterinfo_T*)arg;
    list_rem_watch(lii->list, &lii->lw);
    list_unref(lii->list);
    PyMem_Free(lii);
}

    நிலையான PyObject *
ListIterNext(வெறுமை **arg)
{
    PyObject	*ret;
    listiterinfo_T **lii = (listiterinfo_T**)arg;

    எனில் (!((*lii)->lw.lw_item))
	திரும்பு வெற்று;

    எனில் (!(ret = ConvertToPyObject(&((*lii)->lw.lw_item->li_tv))))
	திரும்பு வெற்று;

    (*lii)->lw.lw_item = (*lii)->lw.lw_item->li_next;

    திரும்பு ret;
}

    நிலையான PyObject *
ListIter(PyObject *self_obj)
{
    ListObject	*self = (ListObject*)self_obj;
    listiterinfo_T	*lii;
    list_T	*l = self->list;

    எனில் (!(lii = PyMem_New(listiterinfo_T, 1)))
    {
	PyErr_NoMemory();
	திரும்பு வெற்று;
    }

    CHECK_LIST_MATERIALIZE(l);
    list_add_watch(l, &lii->lw);
    lii->lw.lw_item = l->lv_first;
    lii->list = l;
    ++l->lv_refcount;

    திரும்பு IterNew(lii,
	    ListIterDestruct, ListIterNext,
	    வெற்று, வெற்று, (PyObject *)self);
}

நிலையான எழுத்து *ListAttrs[] = {
    "locked",
    வெற்று
};

    நிலையான PyObject *
ListDir(PyObject *self, PyObject *args UNUSED)
{
    திரும்பு ObjectDir(self, ListAttrs);
}

    நிலையான முழு
ListSetattr(PyObject *self_obj, எழுத்து *name, PyObject *valObject)
{
    ListObject *self = (ListObject*)self_obj;
    எனில் (valObject == வெற்று)
    {
	PyErr_SET_STRING(PyExc_AttributeError,
		N_("cannot delete vim.List attributes"));
	திரும்பு -1;
    }

    எனில் (strcmp(name, "locked") == 0)
    {
	எனில் (self->list->lv_lock == VAR_FIXED)
	{
	    PyErr_SET_STRING(PyExc_TypeError, N_("cannot modify fixed list"));
	    திரும்பு -1;
	}
	இல்லையெனில்
	{
	    முழு		istrue = PyObject_IsTrue(valObject);
	    எனில் (istrue == -1)
		திரும்பு -1;
	    இல்லையெனில் எனில் (istrue)
		self->list->lv_lock = VAR_LOCKED;
	    இல்லையெனில்
		self->list->lv_lock = 0;
	}
	திரும்பு 0;
    }
    இல்லையெனில்
    {
	PyErr_FORMAT(PyExc_AttributeError, N_("cannot set attribute %s"), name);
	திரும்பு -1;
    }
}

நிலையான PySequenceMethods ListAsSeq = {
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

நிலையான PyMappingMethods ListAsMapping = {
    /* mp_length	*/ (lenfunc) ListLength,
    /* mp_subscript     */ (binaryfunc) ListItem,
    /* mp_ass_subscript */ (objobjargproc) ListAssItem,
};

நிலையான கட்டமைப்பு PyMethodDef ListMethods[] = {
    {"extend",	(PyCFunction)ListConcatInPlace,	METH_O,		""},
    {"__dir__",	(PyCFunction)ListDir,		METH_NOARGS,	""},
    { வெற்று,	வெற்று,				0,		வெற்று}
};

DEFINE_PY_TYPE_OBJECT(TupleType);

வகைவரை கட்டமைப்பு
{
    PyObject_HEAD
    tuple_T	*tuple;
    pylinkedlist_T	ref;
} TupleObject;

#define NEW_TUPLE(tuple) TupleNew(TupleTypePtr, tuple)

    நிலையான PyObject *
TupleNew(PyTypeObject *subtype, tuple_T *tuple)
{
    TupleObject	*self;

    எனில் (tuple == வெற்று)
	திரும்பு வெற்று;

    self = (TupleObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    எனில் (self == வெற்று)
	திரும்பு வெற்று;
    self->tuple = tuple;
    ++tuple->tv_refcount;

    pyll_add((PyObject *)(self), &self->ref, &lasttuple);

    திரும்பு (PyObject *)(self);
}

    நிலையான tuple_T *
py_tuple_alloc(வெறுமை)
{
    tuple_T	*ret;

    எனில் (!(ret = tuple_alloc()))
    {
	PyErr_NoMemory();
	திரும்பு வெற்று;
    }
    ++ret->tv_refcount;

    திரும்பு ret;
}

    நிலையான முழு
tuple_py_concat(tuple_T *t, PyObject *obj, PyObject *lookup_dict)
{
    PyObject	*iterator;
    PyObject	*item;

    எனில் (!(iterator = PyObject_GetIter(obj)))
	திரும்பு -1;

    போது ((item = PyIter_Next(iterator)))
    {
	typval_T    new_tv;

	எனில் (_ConvertFromPyObject(item, &new_tv, lookup_dict) == -1)
	{
	    Py_DECREF(item);
	    Py_DECREF(iterator);
	    திரும்பு -1;
	}

	Py_DECREF(item);

	எனில் (tuple_append_tv(t, &new_tv) == FAIL)
	{
	    Py_DECREF(iterator);
	    திரும்பு -1;
	}
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    எனில் (PyErr_Occurred())
	திரும்பு -1;

    திரும்பு 0;
}

    நிலையான PyObject *
TupleConstructor(PyTypeObject *subtype, PyObject *args, PyObject *kwargs)
{
    tuple_T	*tuple;
    PyObject	*obj = வெற்று;

    எனில் (kwargs)
    {
	PyErr_SET_STRING(PyExc_TypeError,
		N_("tuple constructor does not accept keyword arguments"));
	திரும்பு வெற்று;
    }

    எனில் (!PyArg_ParseTuple(args, "|O", &obj))
	திரும்பு வெற்று;

    எனில் (!(tuple = py_tuple_alloc()))
	திரும்பு வெற்று;

    எனில் (obj)
    {
	PyObject	*lookup_dict;

	எனில் (!(lookup_dict = PyDict_New()))
	{
	    tuple_unref(tuple);
	    திரும்பு வெற்று;
	}

	எனில் (tuple_py_concat(tuple, obj, lookup_dict) == -1)
	{
	    Py_DECREF(lookup_dict);
	    tuple_unref(tuple);
	    திரும்பு வெற்று;
	}

	Py_DECREF(lookup_dict);
    }

    திரும்பு TupleNew(subtype, tuple);
}

    நிலையான வெறுமை
TupleDestructor(PyObject *self_obj)
{
    TupleObject *self = (TupleObject *)self_obj;
    pyll_remove(&self->ref, &lasttuple);
    tuple_unref(self->tuple);

    DESTRUCTOR_FINISH(self);
}

    நிலையான PyInt
TupleLength(TupleObject *self)
{
    திரும்பு ((PyInt)(tuple_len(self->tuple)));
}

    நிலையான PyObject *
TupleIndex(TupleObject *self, Py_ssize_t index)
{
    PyInt	len = TupleLength(self);

    எனில் (index < 0)
	index = len + index;

    எனில் (index < 0 || index >= len)
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("tuple index out of range"));
	திரும்பு வெற்று;
    }
    திரும்பு ConvertToPyObject(TUPLE_ITEM(self->tuple, index));
}

/*
 * Return a new tuple object for the tuple slice starting from the index
 * "first" and of length "slicelen" skipping "step" items.
 */
    நிலையான PyObject *
TupleSlice(
    TupleObject		*self,
    Py_ssize_t		first,
    Py_ssize_t		step,
    Py_ssize_t		slicelen)
{
    PyInt	i;
    PyObject	*tuple;

    tuple = PyTuple_New(slicelen);
    எனில் (tuple == வெற்று)
	திரும்பு வெற்று;

    க்கு (i = 0; i < slicelen; ++i)
    {
	PyObject	*item;

	item = TupleIndex(self, first + i * step);
	எனில் (item == வெற்று)
	{
	    Py_DECREF(tuple);
	    திரும்பு வெற்று;
	}

	PyTuple_SetItem(tuple, i, item);
    }

    திரும்பு tuple;
}

    நிலையான PyObject *
TupleItem(TupleObject *self, PyObject* idx)
{
#எனில் PY_MAJOR_VERSION < 3
    எனில் (PyInt_Check(idx))
    {
	long _idx = PyInt_AsLong(idx);
	திரும்பு TupleIndex(self, _idx);
    }
    இல்லையெனில்
#endif
    எனில் (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	திரும்பு TupleIndex(self, _idx);
    }
    இல்லையெனில் எனில் (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	எனில் (PySlice_GetIndicesEx((PySliceObject_T *)idx, TupleLength(self),
				 &start, &stop, &step, &slicelen) < 0)
	    திரும்பு வெற்று;
	திரும்பு TupleSlice(self, start, step, slicelen);
    }
    இல்லையெனில்
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	திரும்பு வெற்று;
    }
}

வகைவரை கட்டமைப்பு
{
    tuple_T	*tuple;
    முழு		index;
} tupleiterinfo_T;

    நிலையான வெறுமை
TupleIterDestruct(வெறுமை *arg)
{
    tupleiterinfo_T *tii = (tupleiterinfo_T*)arg;
    tuple_unref(tii->tuple);
    PyMem_Free(tii);
}

    நிலையான PyObject *
TupleIterNext(வெறுமை **arg)
{
    PyObject	*ret;
    tupleiterinfo_T **tii = (tupleiterinfo_T**)arg;

    எனில் ((*tii)->index >= TUPLE_LEN((*tii)->tuple))
	திரும்பு வெற்று;

    எனில் (!(ret = ConvertToPyObject(TUPLE_ITEM((*tii)->tuple, (*tii)->index))))
	திரும்பு வெற்று;

    (*tii)->index++;

    திரும்பு ret;
}

    நிலையான PyObject *
TupleIter(PyObject *self_obj)
{
    TupleObject	*self = (TupleObject*)self_obj;
    tupleiterinfo_T	*tii;
    tuple_T	*t = self->tuple;

    எனில் (!(tii = PyMem_New(tupleiterinfo_T, 1)))
    {
	PyErr_NoMemory();
	திரும்பு வெற்று;
    }

    tii->tuple = t;
    tii->index = 0;
    ++t->tv_refcount;

    திரும்பு IterNew(tii,
	    TupleIterDestruct, TupleIterNext,
	    வெற்று, வெற்று, (PyObject *)self);
}

நிலையான எழுத்து *TupleAttrs[] = {
    "locked",
    வெற்று
};

    நிலையான PyObject *
TupleDir(PyObject *self, PyObject *args UNUSED)
{
    திரும்பு ObjectDir(self, TupleAttrs);
}

    நிலையான முழு
TupleSetattr(PyObject *self_obj, எழுத்து *name, PyObject *valObject)
{
    TupleObject *self = (TupleObject*)self_obj;
    எனில் (valObject == வெற்று)
    {
	PyErr_SET_STRING(PyExc_AttributeError,
		N_("cannot delete vim.Tuple attributes"));
	திரும்பு -1;
    }

    எனில் (strcmp(name, "locked") == 0)
    {
	எனில் (self->tuple->tv_lock == VAR_FIXED)
	{
	    PyErr_SET_STRING(PyExc_TypeError, N_("cannot modify fixed tuple"));
	    திரும்பு -1;
	}
	இல்லையெனில்
	{
	    முழு		istrue = PyObject_IsTrue(valObject);
	    எனில் (istrue == -1)
		திரும்பு -1;
	    இல்லையெனில் எனில் (istrue)
		self->tuple->tv_lock = VAR_LOCKED;
	    இல்லையெனில்
		self->tuple->tv_lock = 0;
	}
	திரும்பு 0;
    }
    இல்லையெனில்
    {
	PyErr_FORMAT(PyExc_AttributeError, N_("cannot set attribute %s"), name);
	திரும்பு -1;
    }
}

நிலையான PySequenceMethods TupleAsSeq = {
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

நிலையான PyMappingMethods TupleAsMapping = {
    /* mp_length	*/ (lenfunc) TupleLength,
    /* mp_subscript     */ (binaryfunc) TupleItem,
    /* mp_ass_subscript */ (objobjargproc) 0,
};

நிலையான கட்டமைப்பு PyMethodDef TupleMethods[] = {
    {"__dir__",	(PyCFunction)TupleDir,		METH_NOARGS,	""},
    { வெற்று,	வெற்று,				0,		வெற்று}
};

வகைவரை கட்டமைப்பு
{
    PyObject_HEAD
    char_u	*name;
    முழு		argc;
    typval_T	*argv;
    dict_T	*self;
    pylinkedlist_T	ref;
    முழு		auto_rebind;
} FunctionObject;

DEFINE_PY_TYPE_OBJECT(FunctionType);

#define NEW_FUNCTION(name, argc, argv, self, pt_auto) \
    FunctionNew(FunctionTypePtr, (name), (argc), (argv), (self), (pt_auto))

    நிலையான PyObject *
FunctionNew(PyTypeObject *subtype, char_u *name, முழு argc, typval_T *argv,
	dict_T *selfdict, முழு auto_rebind)
{
    FunctionObject	*self;

    self = (FunctionObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    எனில் (self == வெற்று)
	திரும்பு வெற்று;

    எனில் (isdigit((unsigned எழுத்து)*name))
    {
	எனில் (!translated_function_exists(name, FALSE))
	{
	    PyErr_FORMAT(Py_Exc_மதிப்பு_Error,
		    N_("unnamed function %s does not exist"), name);
	    திரும்பு வெற்று;
	}
	self->name = vim_strsave(name);
    }
    இல்லையெனில்
    {
	char_u *p;

	எனில் ((p = get_expanded_name(name,
			    vim_strchr(name, AUTOLOAD_CHAR) == வெற்று)) == வெற்று)
	{
	    PyErr_FORMAT(Py_Exc_மதிப்பு_Error,
		    N_("function %s does not exist"), name);
	    திரும்பு வெற்று;
	}

	எனில் (p[0] == K_SPECIAL && p[1] == KS_EXTRA && p[2] == (முழு)KE_SNR)
	{
	    char_u *np;
	    size_t len = STRLEN(p) + 1;

	    எனில் ((np = alloc(len + 2)) == வெற்று)
	    {
		vim_free(p);
		திரும்பு வெற்று;
	    }
	    mch_memmove(np, "<SNR>", 5);
	    mch_memmove(np + 5, p + 3, len - 3);
	    vim_free(p);
	    self->name = np;
	}
	இல்லையெனில்
	    self->name = p;
    }

    func_ref(self->name);
    self->argc = argc;
    self->argv = argv;
    self->self = selfdict;
    self->auto_rebind = selfdict == வெற்று ? TRUE : auto_rebind;

    எனில் (self->argv || self->self)
	pyll_add((PyObject *)(self), &self->ref, &lastfunc);

    திரும்பு (PyObject *)(self);
}

    நிலையான PyObject *
FunctionConstructor(PyTypeObject *subtype, PyObject *args, PyObject *kwargs)
{
    PyObject	*self;
    PyObject	*selfdictObject;
    PyObject	*autoRebindObject;
    PyObject	*argsObject = வெற்று;
    char_u	*name;
    typval_T	selfdicttv;
    typval_T	argstv;
    list_T	*argslist = வெற்று;
    dict_T	*selfdict = வெற்று;
    முழு		argc = 0;
    முழு		auto_rebind = TRUE;
    typval_T	*argv = வெற்று;
    typval_T	*curtv;
    listitem_T	*li;

    எனில் (kwargs != வெற்று)
    {
	selfdictObject = PyDict_GetItemString(kwargs, "self");
	எனில் (selfdictObject != வெற்று)
	{
	    எனில் (ConvertFromPyMapping(selfdictObject, &selfdicttv) == -1)
		திரும்பு வெற்று;
	    selfdict = selfdicttv.vval.v_dict;
	}
	argsObject = PyDict_GetItemString(kwargs, "args");
	எனில் (argsObject != வெற்று)
	{
	    எனில் (ConvertFromPySequence(argsObject, &argstv) == -1)
	    {
		dict_unref(selfdict);
		திரும்பு வெற்று;
	    }
	    argslist = argstv.vval.v_list;
	    CHECK_LIST_MATERIALIZE(argslist);

	    argc = argslist->lv_len;
	    எனில் (argc != 0)
	    {
		argv = PyMem_New(typval_T, (size_t) argc);
		எனில் (argv == வெற்று)
		{
		    PyErr_NoMemory();
		    dict_unref(selfdict);
		    list_unref(argslist);
		    திரும்பு வெற்று;
		}
		curtv = argv;
		FOR_ALL_LIST_பொருட்கள்(argslist, li)
		    copy_tv(&li->li_tv, curtv++);
	    }
	    list_unref(argslist);
	}
	எனில் (selfdict != வெற்று)
	{
	    auto_rebind = FALSE;
	    autoRebindObject = PyDict_GetItemString(kwargs, "auto_rebind");
	    எனில் (autoRebindObject != வெற்று)
	    {
		auto_rebind = PyObject_IsTrue(autoRebindObject);
		எனில் (auto_rebind == -1)
		{
		    dict_unref(selfdict);
		    list_unref(argslist);
		    திரும்பு வெற்று;
		}
	    }
	}
    }

    எனில் (!PyArg_ParseTuple(args, "et", "ascii", &name))
    {
	dict_unref(selfdict);
	போது (argc--)
	    clear_tv(&argv[argc]);
	PyMem_Free(argv);
	திரும்பு வெற்று;
    }

    self = FunctionNew(subtype, name, argc, argv, selfdict, auto_rebind);

    PyMem_Free(name);

    திரும்பு self;
}

    நிலையான வெறுமை
FunctionDestructor(PyObject *self_obj)
{
    FunctionObject *self = (FunctionObject*)self_obj;
    முழு i;
    func_unref(self->name);
    vim_free(self->name);
    க்கு (i = 0; i < self->argc; ++i)
	clear_tv(&self->argv[i]);
    PyMem_Free(self->argv);
    dict_unref(self->self);
    எனில் (self->argv || self->self)
	pyll_remove(&self->ref, &lastfunc);

    DESTRUCTOR_FINISH(self);
}

நிலையான எழுத்து *FunctionAttrs[] = {
    "softspace", "args", "self", "auto_rebind",
    வெற்று
};

    நிலையான PyObject *
FunctionDir(PyObject *self, PyObject *args UNUSED)
{
    திரும்பு ObjectDir(self, FunctionAttrs);
}

    நிலையான PyObject *
FunctionAttr(FunctionObject *self, எழுத்து *name)
{
    list_T *list;
    முழு i;
    எனில் (strcmp(name, "name") == 0)
	திரும்பு PyString_FromString((எழுத்து *)(self->name));
    இல்லையெனில் எனில் (strcmp(name, "args") == 0)
    {
	எனில் (self->argv == வெற்று || (list = list_alloc()) == வெற்று)
	    திரும்பு ALWAYS_NONE;

	க்கு (i = 0; i < self->argc; ++i)
	    list_append_tv(list, &self->argv[i]);
	திரும்பு NEW_LIST(list);
    }
    இல்லையெனில் எனில் (strcmp(name, "self") == 0)
	திரும்பு self->self == வெற்று
	    ? ALWAYS_NONE
	    : NEW_DICTIONARY(self->self);
    இல்லையெனில் எனில் (strcmp(name, "auto_rebind") == 0)
	திரும்பு self->auto_rebind
	    ? ALWAYS_TRUE
	    : ALWAYS_FALSE;
    இல்லையெனில் எனில் (strcmp(name, "__members__") == 0)
	திரும்பு ObjectDir(வெற்று, FunctionAttrs);
    திரும்பு வெற்று;
}

/*
 * Populate partial_T given function object.
 *
 * "exported" should be set to true when it is needed to construct a partial
 * that may be stored in a variable (i.e. may be freed by Vim).
 */
    நிலையான வெறுமை
set_partial(FunctionObject *self, partial_T *pt, முழு exported)
{
    முழு i;

    pt->pt_name = self->name;
    எனில் (self->argv)
    {
	pt->pt_argc = self->argc;
	எனில் (exported)
	{
	    pt->pt_argv = ALLOC_CLEAR_MULT(typval_T, self->argc);
	    க்கு (i = 0; i < pt->pt_argc; ++i)
		copy_tv(&self->argv[i], &pt->pt_argv[i]);
	}
	இல்லையெனில்
	    pt->pt_argv = self->argv;
    }
    இல்லையெனில்
    {
	pt->pt_argc = 0;
	pt->pt_argv = வெற்று;
    }
    pt->pt_auto = self->auto_rebind || !exported;
    pt->pt_dict = self->self;
    எனில் (exported && self->self)
	++pt->pt_dict->dv_refcount;
    எனில் (exported)
	pt->pt_name = vim_strsave(pt->pt_name);
    pt->pt_refcount = 1;
}

    நிலையான PyObject *
FunctionCall(PyObject *self_obj, PyObject *argsObject, PyObject *kwargs)
{
    FunctionObject *self = (FunctionObject*)self_obj;
    char_u	*name = self->name;
    typval_T	args;
    typval_T	selfdicttv;
    typval_T	rettv;
    dict_T	*selfdict = வெற்று;
    PyObject	*selfdictObject;
    PyObject	*ret;
    முழு		error;
    partial_T	pt;
    partial_T	*pt_ptr = வெற்று;

    எனில் (ConvertFromPySequence(argsObject, &args) == -1)
	திரும்பு வெற்று;

    எனில் (kwargs != வெற்று)
    {
	selfdictObject = PyDict_GetItemString(kwargs, "self");
	எனில் (selfdictObject != வெற்று)
	{
	    எனில் (ConvertFromPyMapping(selfdictObject, &selfdicttv) == -1)
	    {
		clear_tv(&args);
		திரும்பு வெற்று;
	    }
	    selfdict = selfdicttv.vval.v_dict;
	}
    }

    எனில் (self->argv || self->self)
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

    எனில் (VimTryEnd())
	ret = வெற்று;
    இல்லையெனில் எனில் (error != OK)
    {
	ret = வெற்று;
	PyErr_VIM_FORMAT(N_("failed to run function %s"), (எழுத்து *)name);
    }
    இல்லையெனில்
	ret = ConvertToPyObject(&rettv);

    clear_tv(&args);
    clear_tv(&rettv);
    எனில் (selfdict != வெற்று)
	clear_tv(&selfdicttv);

    திரும்பு ret;
}

    நிலையான PyObject *
FunctionRepr(PyObject *self_obj)
{
    FunctionObject *self = (FunctionObject*)self_obj;
    PyObject *ret;
    garray_T repr_ga;
    முழு i;
    char_u *tofree = வெற்று;
    typval_T tv;
    char_u numbuf[NUMBUFLEN];

    ga_init2(&repr_ga, அளவு(எழுத்து), 70);
    ga_concat(&repr_ga, (char_u *)"<vim.Function '");
    எனில் (self->name)
	ga_concat(&repr_ga, self->name);
    இல்லையெனில்
	ga_concat(&repr_ga, (char_u *)"<NULL>");
    ga_append(&repr_ga, '\'');
    எனில் (self->argv)
    {
	ga_concat(&repr_ga, (char_u *)", args=[");
	++emsg_silent;
	க்கு (i = 0; i < self->argc; i++)
	{
	    எனில் (i != 0)
		ga_concat(&repr_ga, (char_u *)", ");
	    ga_concat(&repr_ga, tv2string(&self->argv[i], &tofree, numbuf,
			get_copyID()));
	    vim_free(tofree);
	}
	--emsg_silent;
	ga_append(&repr_ga, ']');
    }
    எனில் (self->self)
    {
	ga_concat(&repr_ga, (char_u *)", self=");
	tv.v_type = VAR_DICT;
	tv.vval.v_dict = self->self;
	++emsg_silent;
	ga_concat(&repr_ga, tv2string(&tv, &tofree, numbuf, get_copyID()));
	--emsg_silent;
	vim_free(tofree);
	எனில் (self->auto_rebind)
	    ga_concat(&repr_ga, (char_u *)", auto_rebind=True");
    }
    ga_append(&repr_ga, '>');
    ret = PyString_FromString((எழுத்து *)repr_ga.ga_data);
    ga_clear(&repr_ga);
    திரும்பு ret;
}

நிலையான கட்டமைப்பு PyMethodDef FunctionMethods[] = {
    {"__dir__",	(PyCFunction)FunctionDir,   METH_NOARGS,		""},
    { வெற்று,	வெற்று,			0,				வெற்று}
};

/*
 * Options object
 */

DEFINE_PY_TYPE_OBJECT(OptionsType);

வகைவரை முழு (*checkfun)(வெறுமை *);

வகைவரை கட்டமைப்பு
{
    PyObject_HEAD
    முழு		opt_type;
    வெறுமை	*from;
    checkfun	Check;
    PyObject	*fromObj;
} OptionsObject;

    நிலையான முழு
dummy_check(வெறுமை *arg UNUSED)
{
    திரும்பு 0;
}

    நிலையான PyObject *
OptionsNew(முழு opt_type, வெறுமை *from, checkfun Check, PyObject *fromObj)
{
    OptionsObject	*self;

    self = PyObject_GC_New(OptionsObject, OptionsTypePtr);
    எனில் (self == வெற்று)
	திரும்பு வெற்று;

    self->opt_type = opt_type;
    self->from = from;
    self->Check = Check;
    self->fromObj = fromObj;
    எனில் (fromObj)
	Py_INCREF(fromObj);

    திரும்பு (PyObject *)(self);
}

    நிலையான வெறுமை
OptionsDestructor(PyObject *self_obj)
{
    OptionsObject *self = (OptionsObject*)self_obj;
    PyObject_GC_UnTrack((வெறுமை *)(self));
    Py_XDECREF(self->fromObj);
    PyObject_GC_Del((வெறுமை *)(self));
}

    நிலையான முழு
OptionsTraverse(PyObject *self_obj, visitproc visit, வெறுமை *arg)
{
    OptionsObject *self = (OptionsObject*)self_obj;
    Py_VISIT(self->fromObj);
    திரும்பு 0;
}

    நிலையான முழு
OptionsClear(PyObject *self_obj)
{
    OptionsObject *self = (OptionsObject*)self_obj;
    Py_CLEAR(self->fromObj);
    திரும்பு 0;
}

    நிலையான PyObject *
OptionsItem(OptionsObject *self, PyObject *சாவி_Object)
{
    char_u	*சாவி;
    முழு		flags;
    long	numval;
    char_u	*stringval;
    PyObject	*todecref;

    எனில் (self->Check(self->fromObj))
	திரும்பு வெற்று;

    எனில் (!(சாவி = StringToChars(சாவி_Object, &todecref)))
	திரும்பு வெற்று;

    எனில் (*சாவி == NUL)
    {
	RAISE_NO_EMPTY_சாவிகள்;
	Py_XDECREF(todecref);
	திரும்பு வெற்று;
    }

    flags = get_option_மதிப்பு_strict(சாவி, &numval, &stringval,
				    self->opt_type, self->from);

    Py_XDECREF(todecref);

    எனில் (flags == 0)
    {
	PyErr_SetObject(Py_Exc_சாவி_Error, சாவி_Object);
	திரும்பு வெற்று;
    }

    எனில் (flags & SOPT_UNSET)
    {
	Py_INCREF(Py_None);
	திரும்பு Py_None;
    }
    இல்லையெனில் எனில் (flags & SOPT_BOOL)
    {
	PyObject	*ret;
	ret = numval ? Py_True : Py_False;
	Py_INCREF(ret);
	திரும்பு ret;
    }
    இல்லையெனில் எனில் (flags & SOPT_இலக்கம்)
	திரும்பு PyInt_FromLong(numval);
    இல்லையெனில் எனில் (flags & SOPT_STRING)
    {
	எனில் (stringval)
	{
	    PyObject	*ret = PyBytes_FromString((எழுத்து *)stringval);
	    vim_free(stringval);
	    திரும்பு ret;
	}
	இல்லையெனில்
	{
	    PyErr_SET_STRING(PyExc_RuntimeError,
		    N_("unable to get option value"));
	    திரும்பு வெற்று;
	}
    }
    இல்லையெனில்
    {
	PyErr_SET_VIM(N_("internal error: unknown option type"));
	திரும்பு வெற்று;
    }
}

    நிலையான முழு
OptionsContains(OptionsObject *self, PyObject *சாவி_Object)
{
    char_u	*சாவி;
    PyObject	*todecref;

    எனில் (!(சாவி = StringToChars(சாவி_Object, &todecref)))
	திரும்பு -1;

    எனில் (*சாவி == NUL)
    {
	Py_XDECREF(todecref);
	திரும்பு 0;
    }

    எனில் (get_option_மதிப்பு_strict(சாவி, வெற்று, வெற்று, self->opt_type, வெற்று))
    {
	Py_XDECREF(todecref);
	திரும்பு 1;
    }
    இல்லையெனில்
    {
	Py_XDECREF(todecref);
	திரும்பு 0;
    }
}

வகைவரை கட்டமைப்பு
{
    வெறுமை	*lastoption;
    முழு		opt_type;
} optiterinfo_T;

    நிலையான PyObject *
OptionsIterNext(வெறுமை **arg)
{
    char_u	*name;
    optiterinfo_T **oii = (optiterinfo_T**)arg;

    எனில் ((name = option_iter_next(&((*oii)->lastoption), (*oii)->opt_type)))
	திரும்பு PyString_FromString((எழுத்து *)name);

    திரும்பு வெற்று;
}

    நிலையான PyObject *
OptionsIter(PyObject *self_obj)
{
    OptionsObject	*self = (OptionsObject*)self_obj;
    optiterinfo_T	*oii;

    எனில் (!(oii = PyMem_New(optiterinfo_T, 1)))
    {
	PyErr_NoMemory();
	திரும்பு வெற்று;
    }

    oii->opt_type = self->opt_type;
    oii->lastoption = வெற்று;

    திரும்பு IterNew(oii,
	    PyMem_Free, OptionsIterNext,
	    வெற்று, வெற்று, (PyObject *)self);
}

    நிலையான முழு
set_option_மதிப்பு_err(char_u *சாவி, முழு numval, char_u *stringval, முழு opt_flags)
{
    எழுத்து	*errmsg;

    எனில் ((errmsg = set_option_மதிப்பு(சாவி, numval, stringval, opt_flags)))
    {
	எனில் (VimTryEnd())
	    திரும்பு FAIL;
	PyErr_SetVim(errmsg);
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

    நிலையான முழு
set_option_மதிப்பு_for(
	char_u *சாவி,
	முழு numval,
	char_u *stringval,
	முழு opt_flags,
	முழு opt_type,
	வெறுமை *from)
{
    switchwin_T	switchwin;
    bufref_T	save_curbuf;
    முழு		set_ret = 0;

    VimTryStart();
    தேர்வு (opt_type)
    {
	நிலை SREQ_WIN:
	    எனில் (switch_win(&switchwin, (win_T *)from,
			      win_find_tabpage((win_T *)from), FALSE) == FAIL)
	    {
		restore_win(&switchwin, TRUE);
		எனில் (VimTryEnd())
		    திரும்பு -1;
		PyErr_SET_VIM(N_("problem while switching windows"));
		திரும்பு -1;
	    }
	    set_ret = set_option_மதிப்பு_err(சாவி, numval, stringval, opt_flags);
	    restore_win(&switchwin, TRUE);
	    நிறுத்து;
	நிலை SREQ_BUF:
	    switch_buffer(&save_curbuf, (buf_T *)from);
	    set_ret = set_option_மதிப்பு_err(சாவி, numval, stringval, opt_flags);
	    restore_buffer(&save_curbuf);
	    நிறுத்து;
	நிலை SREQ_GLOBAL:
	    set_ret = set_option_மதிப்பு_err(சாவி, numval, stringval, opt_flags);
	    நிறுத்து;
    }
    எனில் (set_ret == FAIL)
	திரும்பு -1;
    திரும்பு VimTryEnd();
}

    நிலையான முழு
OptionsAssItem(OptionsObject *self, PyObject *சாவி_Object, PyObject *valObject)
{
    char_u	*சாவி;
    முழு		flags;
    முழு		opt_flags;
    முழு		ret = 0;
    PyObject	*todecref;

    எனில் (self->Check(self->fromObj))
	திரும்பு -1;

    எனில் (!(சாவி = StringToChars(சாவி_Object, &todecref)))
	திரும்பு -1;

    எனில் (*சாவி == NUL)
    {
	RAISE_NO_EMPTY_சாவிகள்;
	Py_XDECREF(todecref);
	திரும்பு -1;
    }

    flags = get_option_மதிப்பு_strict(சாவி, வெற்று, வெற்று,
				    self->opt_type, self->from);

    எனில் (flags == 0)
    {
	PyErr_SetObject(Py_Exc_சாவி_Error, சாவி_Object);
	Py_XDECREF(todecref);
	திரும்பு -1;
    }

    எனில் (valObject == வெற்று)
    {
	எனில் (self->opt_type == SREQ_GLOBAL)
	{
	    PyErr_FORMAT(Py_Exc_மதிப்பு_Error,
		    N_("unable to unset global option %s"), சாவி);
	    Py_XDECREF(todecref);
	    திரும்பு -1;
	}
	இல்லையெனில் எனில் (!(flags & SOPT_GLOBAL))
	{
	    PyErr_FORMAT(Py_Exc_மதிப்பு_Error,
		    N_("unable to unset option %s "
		       "which does not have global value"), சாவி);
	    Py_XDECREF(todecref);
	    திரும்பு -1;
	}
	இல்லையெனில்
	{
	    unset_global_local_option(சாவி, self->from);
	    Py_XDECREF(todecref);
	    திரும்பு 0;
	}
    }

    opt_flags = (self->opt_type ? OPT_LOCAL : OPT_GLOBAL);

    எனில் (flags & SOPT_BOOL)
    {
	முழு	istrue = PyObject_IsTrue(valObject);

	எனில் (istrue == -1)
	    ret = -1;
	இல்லையெனில்
	    ret = set_option_மதிப்பு_for(சாவி, istrue, வெற்று,
				    opt_flags, self->opt_type, self->from);
    }
    இல்லையெனில் எனில் (flags & SOPT_இலக்கம்)
    {
	long	val;

	எனில் (எண்_To_Long(valObject, &val, எண்_INT))
	{
	    Py_XDECREF(todecref);
	    திரும்பு -1;
	}

	ret = set_option_மதிப்பு_for(சாவி, (முழு) val, வெற்று, opt_flags,
				self->opt_type, self->from);
    }
    இல்லையெனில்
    {
	char_u		*val;
	PyObject	*todecref2;

	எனில் ((val = StringToChars(valObject, &todecref2)))
	{
	    ret = set_option_மதிப்பு_for(சாவி, 0, val, opt_flags,
				    self->opt_type, self->from);
	    Py_XDECREF(todecref2);
	}
	இல்லையெனில்
	    ret = -1;
    }

    Py_XDECREF(todecref);

    திரும்பு ret;
}

நிலையான PySequenceMethods OptionsAsSeq = {
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

நிலையான PyMappingMethods OptionsAsMapping = {
    (lenfunc)       வெற்று,
    (binaryfunc)    OptionsItem,
    (objobjargproc) OptionsAssItem,
};

// Tabpage object

வகைவரை கட்டமைப்பு
{
    PyObject_HEAD
    tabpage_T	*tab;
} TabPageObject;

நிலையான PyObject *WinListNew(TabPageObject *tabObject);

DEFINE_PY_TYPE_OBJECT(TabPageType);

    நிலையான முழு
CheckTabPage(TabPageObject *self)
{
    எனில் (self->tab == INVALID_TABPAGE_மதிப்பு)
    {
	PyErr_SET_VIM(N_("attempt to refer to deleted tab page"));
	திரும்பு -1;
    }

    திரும்பு 0;
}

    நிலையான PyObject *
TabPageNew(tabpage_T *tab)
{
    TabPageObject *self;

    எனில் (TAB_PYTHON_REF(tab))
    {
	self = TAB_PYTHON_REF(tab);
	Py_INCREF((PyObject *)self);
    }
    இல்லையெனில்
    {
	self = PyObject_NEW(TabPageObject, TabPageTypePtr);
	எனில் (self == வெற்று)
	    திரும்பு வெற்று;
	self->tab = tab;
	TAB_PYTHON_REF(tab) = self;
    }

    திரும்பு (PyObject *)(self);
}

    நிலையான வெறுமை
TabPageDestructor(PyObject *self_obj)
{
    TabPageObject *self = (TabPageObject*)self_obj;
    எனில் (self->tab && self->tab != INVALID_TABPAGE_மதிப்பு)
	TAB_PYTHON_REF(self->tab) = வெற்று;

    DESTRUCTOR_FINISH(self);
}

நிலையான எழுத்து *TabPageAttrs[] = {
    "windows", "number", "vars", "window", "valid",
    வெற்று
};

    நிலையான PyObject *
TabPageDir(PyObject *self, PyObject *args UNUSED)
{
    திரும்பு ObjectDir(self, TabPageAttrs);
}

    நிலையான PyObject *
TabPageAttrValid(TabPageObject *self, எழுத்து *name)
{
    PyObject	*ret;

    எனில் (strcmp(name, "valid") != 0)
	திரும்பு வெற்று;

    ret = ((self->tab == INVALID_TABPAGE_மதிப்பு) ? Py_False : Py_True);
    Py_INCREF(ret);
    திரும்பு ret;
}

    நிலையான PyObject *
TabPageAttr(TabPageObject *self, எழுத்து *name)
{
    எனில் (strcmp(name, "windows") == 0)
	திரும்பு WinListNew(self);
    இல்லையெனில் எனில் (strcmp(name, "number") == 0)
	திரும்பு PyLong_FromLong((long) get_tab_எண்(self->tab));
    இல்லையெனில் எனில் (strcmp(name, "vars") == 0)
	திரும்பு NEW_DICTIONARY(self->tab->tp_vars);
    இல்லையெனில் எனில் (strcmp(name, "window") == 0)
    {
	// For current tab window.c does not bother to set or update tp_curwin
	எனில் (self->tab == curtab)
	    திரும்பு WindowNew(curwin, curtab);
	இல்லையெனில்
	    திரும்பு WindowNew(self->tab->tp_curwin, self->tab);
    }
    இல்லையெனில் எனில் (strcmp(name, "__members__") == 0)
	திரும்பு ObjectDir(வெற்று, TabPageAttrs);
    திரும்பு வெற்று;
}

    நிலையான PyObject *
TabPageRepr(PyObject *self_obj)
{
    TabPageObject *self = (TabPageObject*)self_obj;
    எனில் (self->tab == INVALID_TABPAGE_மதிப்பு)
	திரும்பு PyString_FromFormat("<tabpage object (deleted) at %p>", (வெறுமை *)self);
    இல்லையெனில்
    {
	முழு	t = get_tab_எண்(self->tab);

	எனில் (t == 0)
	    திரும்பு PyString_FromFormat("<tabpage object (unknown) at %p>",
					(வெறுமை *)self);
	இல்லையெனில்
	    திரும்பு PyString_FromFormat("<tabpage %d>", t - 1);
    }
}

நிலையான கட்டமைப்பு PyMethodDef TabPageMethods[] = {
    // name,	    function,			calling,	documentation
    {"__dir__",	    (PyCFunction)TabPageDir,	METH_NOARGS,	""},
    { வெற்று,	    வெற்று,			0,		வெற்று}
};

/*
 * Window list object
 */

DEFINE_PY_TYPE_OBJECT(TabListType);
நிலையான PySequenceMethods TabListAsSeq;

வகைவரை கட்டமைப்பு
{
    PyObject_HEAD
} TabListObject;

நிலையான TabListObject TheTabPageList =
{
    PyObject_HEAD_INIT_TYPE(TabListType)
};

    நிலையான PyInt
TabListLength(PyObject *self UNUSED)
{
    tabpage_T	*tp = first_tabpage;
    PyInt	n = 0;

    போது (tp != வெற்று)
    {
	++n;
	tp = tp->tp_next;
    }

    திரும்பு n;
}

    நிலையான PyObject *
TabListItem(PyObject *self UNUSED, PyInt n)
{
    tabpage_T	*tp;

    க்கு (tp = first_tabpage; tp != வெற்று; tp = tp->tp_next, --n)
	எனில் (n == 0)
	    திரும்பு TabPageNew(tp);

    PyErr_SET_STRING(PyExc_IndexError, N_("no such tab page"));
    திரும்பு வெற்று;
}

/*
 * Window object
 */

வகைவரை கட்டமைப்பு
{
    PyObject_HEAD
    win_T	*win;
    TabPageObject	*tabObject;
} WindowObject;

DEFINE_PY_TYPE_OBJECT(WindowType);

    நிலையான முழு
CheckWindow(WindowObject *self)
{
    எனில் (self->win == INVALID_WINDOW_மதிப்பு)
    {
	PyErr_SET_VIM(N_("attempt to refer to deleted window"));
	திரும்பு -1;
    }

    திரும்பு 0;
}

    நிலையான முழு
CheckWindowCb(வெறுமை *self)
{
    திரும்பு CheckWindow((WindowObject*)self);
}

    நிலையான PyObject *
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

    எனில் (WIN_PYTHON_REF(win))
    {
	self = WIN_PYTHON_REF(win);
	Py_INCREF((PyObject *)self);
    }
    இல்லையெனில்
    {
	self = PyObject_GC_New(WindowObject, WindowTypePtr);
	எனில் (self == வெற்று)
	    திரும்பு வெற்று;
	self->win = win;
	WIN_PYTHON_REF(win) = self;
    }

    self->tabObject = ((TabPageObject *)(TabPageNew(tab)));

    திரும்பு (PyObject *)(self);
}

    நிலையான வெறுமை
WindowDestructor(PyObject *self_obj)
{
    WindowObject *self = (WindowObject*)self_obj;
    PyObject_GC_UnTrack((வெறுமை *)(self));
    எனில் (self->win && self->win != INVALID_WINDOW_மதிப்பு)
	WIN_PYTHON_REF(self->win) = வெற்று;
    Py_XDECREF(((PyObject *)(self->tabObject)));
    PyObject_GC_Del((வெறுமை *)(self));
}

    நிலையான முழு
WindowTraverse(PyObject *self_obj, visitproc visit, வெறுமை *arg)
{
    WindowObject *self = (WindowObject*)self_obj;
    Py_VISIT(((PyObject *)(self->tabObject)));
    திரும்பு 0;
}

    நிலையான முழு
WindowClear(PyObject *self_obj)
{
    WindowObject *self = (WindowObject*)self_obj;
    Py_CLEAR(self->tabObject);
    திரும்பு 0;
}

    நிலையான win_T *
get_firstwin(TabPageObject *tabObject)
{
    எனில் (tabObject)
    {
	எனில் (CheckTabPage(tabObject))
	    திரும்பு வெற்று;
	// For current tab window.c does not bother to set or update tp_firstwin
	இல்லையெனில் எனில் (tabObject->tab == curtab)
	    திரும்பு firstwin;
	இல்லையெனில்
	    திரும்பு tabObject->tab->tp_firstwin;
    }
    இல்லையெனில்
	திரும்பு firstwin;
}

// Use the same order as in the WindowAttr() function.
நிலையான எழுத்து *WindowAttrs[] = {
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
    வெற்று
};

    நிலையான PyObject *
WindowDir(PyObject *self, PyObject *args UNUSED)
{
    திரும்பு ObjectDir(self, WindowAttrs);
}

    நிலையான PyObject *
WindowAttrValid(WindowObject *self, எழுத்து *name)
{
    PyObject	*ret;

    எனில் (strcmp(name, "valid") != 0)
	திரும்பு வெற்று;

    ret = ((self->win == INVALID_WINDOW_மதிப்பு) ? Py_False : Py_True);
    Py_INCREF(ret);
    திரும்பு ret;
}

    நிலையான PyObject *
WindowAttr(WindowObject *self, எழுத்து *name)
{
    எனில் (strcmp(name, "buffer") == 0)
	திரும்பு (PyObject *)BufferNew(self->win->w_buffer);
    இல்லையெனில் எனில் (strcmp(name, "cursor") == 0)
    {
	pos_T *pos = &self->win->w_cursor;

	திரும்பு Py_Build_மதிப்பு("(ll)", (long)(pos->lnum), (long)(pos->col));
    }
    இல்லையெனில் எனில் (strcmp(name, "height") == 0)
	திரும்பு PyLong_FromLong((long)(self->win->w_height));
    இல்லையெனில் எனில் (strcmp(name, "row") == 0)
	திரும்பு PyLong_FromLong((long)(self->win->w_winrow));
    இல்லையெனில் எனில் (strcmp(name, "width") == 0)
	திரும்பு PyLong_FromLong((long)(self->win->w_width));
    இல்லையெனில் எனில் (strcmp(name, "col") == 0)
	திரும்பு PyLong_FromLong((long)(self->win->w_wincol));
    இல்லையெனில் எனில் (strcmp(name, "vars") == 0)
	திரும்பு NEW_DICTIONARY(self->win->w_vars);
    இல்லையெனில் எனில் (strcmp(name, "options") == 0)
	திரும்பு OptionsNew(SREQ_WIN, self->win, CheckWindowCb,
			(PyObject *) self);
    இல்லையெனில் எனில் (strcmp(name, "number") == 0)
    {
	எனில் (CheckTabPage(self->tabObject))
	    திரும்பு வெற்று;
	திரும்பு PyLong_FromLong((long)
		get_win_எண்(self->win, get_firstwin(self->tabObject)));
    }
    இல்லையெனில் எனில் (strcmp(name, "tabpage") == 0)
    {
	Py_INCREF((PyObject *)self->tabObject);
	திரும்பு (PyObject *)(self->tabObject);
    }
    இல்லையெனில் எனில் (strcmp(name, "__members__") == 0)
	திரும்பு ObjectDir(வெற்று, WindowAttrs);
    இல்லையெனில்
	திரும்பு வெற்று;
}

    நிலையான முழு
WindowSetattr(PyObject *self_obj, எழுத்து *name, PyObject *valObject)
{
    WindowObject *self = (WindowObject*)self_obj;
    எனில் (CheckWindow(self))
	திரும்பு -1;

    எனில் (strcmp(name, "buffer") == 0)
    {
	PyErr_SET_STRING(PyExc_TypeError, N_("readonly attribute: buffer"));
	திரும்பு -1;
    }
    இல்லையெனில் எனில் (strcmp(name, "cursor") == 0)
    {
	long lnum;
	long col;

	எனில் (!PyArg_Parse(valObject, "(ll)", &lnum, &col))
	    திரும்பு -1;

	எனில் (lnum <= 0 || lnum > self->win->w_buffer->b_ml.ml_வரி_மொத்தம்)
	{
	    PyErr_SET_VIM(N_("cursor position outside buffer"));
	    திரும்பு -1;
	}

	// Check for keyboard interrupts
	எனில் (VimCheckInterrupt())
	    திரும்பு -1;

	self->win->w_cursor.lnum = lnum;
	self->win->w_cursor.col = col;
	self->win->w_set_curswant = TRUE;
	self->win->w_cursor.coladd = 0;
	// When column is out of range silently correct it.
	check_cursor_col_win(self->win);

	update_screen(UPD_VALID);
	திரும்பு 0;
    }
    இல்லையெனில் எனில் (strcmp(name, "height") == 0)
    {
	long	height;
	win_T	*savewin;

	எனில் (எண்_To_Long(valObject, &height, எண்_INT|எண்_UNSIGNED))
	    திரும்பு -1;

#ifdef FEAT_GUI
	need_mouse_correct = TRUE;
#endif
	savewin = curwin;
	curwin = self->win;
	curbuf = curwin->w_buffer;

	VimTryStart();
	win_setheight((முழு) height);
	curwin = savewin;
	curbuf = curwin->w_buffer;
	எனில் (VimTryEnd())
	    திரும்பு -1;

	திரும்பு 0;
    }
    இல்லையெனில் எனில் (strcmp(name, "width") == 0)
    {
	long	width;
	win_T	*savewin;

	எனில் (எண்_To_Long(valObject, &width, எண்_INT|எண்_UNSIGNED))
	    திரும்பு -1;

#ifdef FEAT_GUI
	need_mouse_correct = TRUE;
#endif
	savewin = curwin;
	curwin = self->win;
	curbuf = curwin->w_buffer;

	VimTryStart();
	win_setwidth((முழு) width);
	curwin = savewin;
	curbuf = curwin->w_buffer;
	எனில் (VimTryEnd())
	    திரும்பு -1;

	திரும்பு 0;
    }
    இல்லையெனில்
    {
	PyErr_SetString(PyExc_AttributeError, name);
	திரும்பு -1;
    }
}

    நிலையான PyObject *
WindowRepr(PyObject *self_obj)
{
    WindowObject *self = (WindowObject*)self_obj;
    எனில் (self->win == INVALID_WINDOW_மதிப்பு)
	திரும்பு PyString_FromFormat("<window object (deleted) at %p>", (வெறுமை *)self);
    இல்லையெனில்
    {
	முழு	w = get_win_எண்(self->win, firstwin);

	எனில் (w == 0)
	    திரும்பு PyString_FromFormat("<window object (unknown) at %p>",
								      (வெறுமை *)self);
	இல்லையெனில்
	    திரும்பு PyString_FromFormat("<window %d>", w - 1);
    }
}

நிலையான கட்டமைப்பு PyMethodDef WindowMethods[] = {
    // name,	    function,			calling,	documentation
    {"__dir__",	    (PyCFunction)WindowDir,	METH_NOARGS,	""},
    { வெற்று,	    வெற்று,			0,		வெற்று}
};

/*
 * Window list object
 */

DEFINE_PY_TYPE_OBJECT(WinListType);
நிலையான PySequenceMethods WinListAsSeq;

வகைவரை கட்டமைப்பு
{
    PyObject_HEAD
    TabPageObject	*tabObject;
} WinListObject;

நிலையான WinListObject TheWindowList =
{
    PyObject_HEAD_INIT_TYPE(WinListType)
    வெற்று
};

    நிலையான PyObject *
WinListNew(TabPageObject *tabObject)
{
    WinListObject	*self;

    self = PyObject_NEW(WinListObject, WinListTypePtr);
    self->tabObject = tabObject;
    Py_INCREF((PyObject *)tabObject);

    திரும்பு (PyObject *)(self);
}

    நிலையான வெறுமை
WinListDestructor(WinListObject *self)
{
    TabPageObject	*tabObject = self->tabObject;

    எனில் (tabObject)
    {
	Py_DECREF((PyObject *)(tabObject));
    }

    DESTRUCTOR_FINISH(self);
}

    நிலையான PyInt
WinListLength(WinListObject *self)
{
    win_T	*w;
    PyInt	n = 0;

    எனில் (!(w = get_firstwin(self->tabObject)))
	திரும்பு -1;

    போது (w != வெற்று)
    {
	++n;
	w = W_NEXT(w);
    }

    திரும்பு n;
}

    நிலையான PyObject *
WinListItem(WinListObject *self, PyInt n)
{
    win_T *w;

    எனில் (!(w = get_firstwin(self->tabObject)))
	திரும்பு வெற்று;

    க்கு (; w != வெற்று; w = W_NEXT(w), --n)
	எனில் (n == 0)
	    திரும்பு WindowNew(w, self->tabObject? self->tabObject->tab: curtab);

    PyErr_SET_STRING(PyExc_IndexError, N_("no such window"));
    திரும்பு வெற்று;
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
    நிலையான எழுத்து *
String_To_வரி(PyObject *obj)
{
    எழுத்து	*str;
    எழுத்து	*save;
    PyObject	*bytes = வெற்று;
    Py_ssize_t	len = 0;
    PyInt	i;
    எழுத்து	*p;

    எனில் (PyBytes_Check(obj))
    {
	எனில் (PyBytes_AsStringAndSize(obj, &str, &len) == -1
		|| str == வெற்று)
	    திரும்பு வெற்று;
    }
    இல்லையெனில் எனில் (PyUnicode_Check(obj))
    {
	எனில் (!(bytes = PyUnicode_AsEncodedString(obj, ENC_OPT,
							   ERRORS_ENCODE_ARG)))
	    திரும்பு வெற்று;

	எனில் (PyBytes_AsStringAndSize(bytes, &str, &len) == -1
		|| str == வெற்று)
	{
	    Py_DECREF(bytes);
	    திரும்பு வெற்று;
	}
    }
    இல்லையெனில்
    {
#எனில் PY_MAJOR_VERSION < 3
	PyErr_FORMAT_TYPE(
		N_("expected str() or unicode() instance, but got %s"),
		obj);
#இல்லையெனில்
	PyErr_FORMAT_TYPE(
		N_("expected bytes() or str() instance, but got %s"),
		obj);
#endif
	திரும்பு வெற்று;
    }

    /*
     * Error checking: String must not contain newlines, as we
     * are replacing a single line, and we must replace it with
     * a single line.
     * A trailing newline is removed, so that append(f.readlines()) works.
     */
    p = memchr(str, '\n', len);
    எனில் (p != வெற்று)
    {
	எனில் (p == str + len - 1)
	    --len;
	இல்லையெனில்
	{
	    PyErr_SET_VIM(N_("string cannot contain newlines"));
	    Py_XDECREF(bytes);
	    திரும்பு வெற்று;
	}
    }

    /*
     * Create a copy of the string, with internal nulls replaced by
     * newline characters, as is the Vim convention.
     */
    save = alloc(len+1);
    எனில் (save == வெற்று)
    {
	PyErr_NoMemory();
	Py_XDECREF(bytes);
	திரும்பு வெற்று;
    }

    க்கு (i = 0; i < len; ++i)
    {
	எனில் (str[i] == '\0')
	    save[i] = '\n';
	இல்லையெனில்
	    save[i] = str[i];
    }

    save[i] = '\0';
    Py_XDECREF(bytes);  // Python 2 does nothing here

    திரும்பு save;
}

/*
 * Get a line from the specified buffer. The line number is
 * in Vim format (1-based). The line is returned as a Python
 * string object.
 */
    நிலையான PyObject *
Get_Buffer_வரி(buf_T *buf, PyInt n)
{
    திரும்பு வரி_To_String((எழுத்து *)ml_get_buf(buf, (linenr_T)n, FALSE));
}


/*
 * Get a list of lines from the specified buffer. The line numbers
 * are in Vim format (1-based). The range is from lo up to, but not
 * including, hi. The list is returned as a Python list of string objects.
 */
    நிலையான PyObject *
Get_Buffer_வரி_List(buf_T *buf, PyInt lo, PyInt hi)
{
    PyInt	i;
    PyInt	n = hi - lo;
    PyObject	*list = PyList_New(n);

    எனில் (list == வெற்று)
	திரும்பு வெற்று;

    க்கு (i = 0; i < n; ++i)
    {
	linenr_T	lnum = (linenr_T)(lo + i);
	எழுத்து		*text;
	PyObject	*string;

	எனில் (lnum > buf->b_ml.ml_வரி_மொத்தம்)
	    text = "";
	இல்லையெனில்
	    text = (எழுத்து *)ml_get_buf(buf, lnum, FALSE);
	string = வரி_To_String(text);
	எனில் (string == வெற்று)
	{
	    Py_DECREF(list);
	    திரும்பு வெற்று;
	}

	PyList_SET_ITEM(list, i, string);
    }

    // The ownership of the Python list is passed to the caller (ie,
    // the caller should Py_DECREF() the object when it is finished
    // with it).

    திரும்பு list;
}

/*
 * Check if deleting lines made the cursor position invalid.
 * Changed the lines from "lo" to "hi" and added "extra" lines (negative if
 * deleted).
 */
    நிலையான வெறுமை
py_fix_cursor(linenr_T lo, linenr_T hi, linenr_T extra)
{
    எனில் (curwin->w_cursor.lnum >= lo)
    {
	// Adjust the cursor position if it's in/after the changed
	// lines.
	எனில் (curwin->w_cursor.lnum >= hi)
	{
	    curwin->w_cursor.lnum += extra;
	    check_cursor_col();
	}
	இல்லையெனில் எனில் (extra < 0)
	{
	    curwin->w_cursor.lnum = lo;
	    check_cursor();
	}
	இல்லையெனில்
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
    நிலையான முழு
Set_Buffer_வரி(buf_T *buf, PyInt n, PyObject *வரி, PyInt *len_change)
{
    bufref_T	save_curbuf = {வெற்று, 0, 0};
    switchwin_T	switchwin;

    // First of all, we check the type of the supplied Python object.
    // There are three cases:
    //	  1. NULL, or None - this is a deletion.
    //	  2. A string	   - this is a replacement.
    //	  3. Anything else - this is an error.
    எனில் (வரி == Py_None || வரி == வெற்று)
    {
	PyErr_Clear();
	switchwin.sw_curwin = வெற்று;
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	VimTryStart();

	எனில் (u_savedel((linenr_T)n, 1L) == FAIL)
	    RAISE_UNDO_FAIL;
	இல்லையெனில் எனில் (ml_delete((linenr_T)n) == FAIL)
	    RAISE_DELETE_வரி_FAIL;
	இல்லையெனில்
	{
	    எனில் (buf == curbuf && (switchwin.sw_curwin != வெற்று
					   || save_curbuf.br_buf == வெற்று))
		// Using an existing window for the buffer, adjust the cursor
		// position.
		py_fix_cursor((linenr_T)n, (linenr_T)n + 1, (linenr_T)-1);
	    எனில் (save_curbuf.br_buf == வெற்று)
		// Only adjust marks if we managed to switch to a window that
		// holds the buffer, otherwise line numbers will be invalid.
		deleted_வரிகள்_mark((linenr_T)n, 1L);
	}

	restore_win_for_buf(&switchwin, &save_curbuf);

	எனில் (VimTryEnd())
	    திரும்பு FAIL;

	எனில் (len_change)
	    *len_change = -1;

	திரும்பு OK;
    }
    இல்லையெனில் எனில் (PyBytes_Check(வரி) || PyUnicode_Check(வரி))
    {
	எழுத்து	*save = String_To_வரி(வரி);

	எனில் (save == வெற்று)
	    திரும்பு FAIL;

	VimTryStart();

	// We do not need to free "save" if ml_replace() consumes it.
	PyErr_Clear();
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	எனில் (u_savesub((linenr_T)n) == FAIL)
	{
	    RAISE_UNDO_FAIL;
	    vim_free(save);
	}
	இல்லையெனில் எனில் (ml_replace((linenr_T)n, (char_u *)save, FALSE) == FAIL)
	{
	    RAISE_REPLACE_வரி_FAIL;
	    vim_free(save);
	}
	இல்லையெனில்
	    changed_bytes((linenr_T)n, 0);

	restore_win_for_buf(&switchwin, &save_curbuf);

	// Check that the cursor is not beyond the end of the line now.
	எனில் (buf == curbuf)
	    check_cursor_col();

	எனில் (VimTryEnd())
	    திரும்பு FAIL;

	எனில் (len_change)
	    *len_change = 0;

	திரும்பு OK;
    }
    இல்லையெனில்
    {
	PyErr_BadArgument();
	திரும்பு FAIL;
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
    நிலையான முழு
Set_Buffer_வரி_List(
	buf_T *buf,
	PyInt lo,
	PyInt hi,
	PyObject *list,
	PyInt *len_change)
{
    bufref_T	save_curbuf = {வெற்று, 0, 0};
    switchwin_T	switchwin;

    // First of all, we check the type of the supplied Python object.
    // There are three cases:
    //	  1. NULL, or None - this is a deletion.
    //	  2. A list	   - this is a replacement.
    //	  3. Anything else - this is an error.
    எனில் (list == Py_None || list == வெற்று)
    {
	PyInt	i;
	PyInt	n = (முழு)(hi - lo);

	PyErr_Clear();
	VimTryStart();
	switchwin.sw_curwin = வெற்று;
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	எனில் (u_savedel((linenr_T)lo, (long)n) == FAIL)
	    RAISE_UNDO_FAIL;
	இல்லையெனில்
	{
	    க்கு (i = 0; i < n; ++i)
	    {
		எனில் (ml_delete((linenr_T)lo) == FAIL)
		{
		    RAISE_DELETE_வரி_FAIL;
		    நிறுத்து;
		}
	    }
	    எனில் (buf == curbuf && (switchwin.sw_curwin != வெற்று
					       || save_curbuf.br_buf == வெற்று))
		// Using an existing window for the buffer, adjust the cursor
		// position.
		py_fix_cursor((linenr_T)lo, (linenr_T)hi, (linenr_T)-n);
	    எனில் (save_curbuf.br_buf == வெற்று)
		// Only adjust marks if we managed to switch to a window that
		// holds the buffer, otherwise line numbers will be invalid.
		deleted_வரிகள்_mark((linenr_T)lo, (long)i);
	}

	restore_win_for_buf(&switchwin, &save_curbuf);

	எனில் (VimTryEnd())
	    திரும்பு FAIL;

	எனில் (len_change)
	    *len_change = -n;

	திரும்பு OK;
    }
    இல்லையெனில் எனில் (PyList_Check(list))
    {
	PyInt	i;
	PyInt	new_len = PyList_Size(list);
	PyInt	old_len = hi - lo;
	PyInt	extra = 0;	// lines added to text, can be negative
	எழுத்து	**array;

	எனில் (new_len == 0)	// avoid allocating zero bytes
	    array = வெற்று;
	இல்லையெனில்
	{
	    array = PyMem_New(எழுத்து *, new_len);
	    எனில் (array == வெற்று)
	    {
		PyErr_NoMemory();
		திரும்பு FAIL;
	    }
	}

	க்கு (i = 0; i < new_len; ++i)
	{
	    PyObject *வரி;

	    எனில் (!(வரி = PyList_GetItem(list, i)) ||
		!(array[i] = String_To_வரி(வரி)))
	    {
		போது (i)
		    vim_free(array[--i]);
		PyMem_Free(array);
		திரும்பு FAIL;
	    }
	}

	VimTryStart();
	PyErr_Clear();

	// START of region without "return".  Must call restore_buffer()!
	switchwin.sw_curwin = வெற்று;
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	எனில் (u_save((linenr_T)(lo-1), (linenr_T)hi) == FAIL)
	    RAISE_UNDO_FAIL;

	// If the size of the range is reducing (ie, new_len < old_len) we
	// need to delete some old_len. We do this at the start, by
	// repeatedly deleting line "lo".
	எனில் (!PyErr_Occurred())
	{
	    க்கு (i = 0; i < old_len - new_len; ++i)
		எனில் (ml_delete((linenr_T)lo) == FAIL)
		{
		    RAISE_DELETE_வரி_FAIL;
		    நிறுத்து;
		}
	    extra -= i;
	}

	// For as long as possible, replace the existing old_len with the
	// new old_len. This is a more efficient operation, as it requires
	// less memory allocation and freeing.
	எனில் (!PyErr_Occurred())
	{
	    க்கு (i = 0; i < old_len && i < new_len; ++i)
		எனில் (ml_replace((linenr_T)(lo+i), (char_u *)array[i], FALSE)
								      == FAIL)
		{
		    RAISE_REPLACE_வரி_FAIL;
		    நிறுத்து;
		}
	}
	இல்லையெனில்
	    i = 0;

	// Now we may need to insert the remaining new old_len. If we do, we
	// must free the strings as we finish with them (we can't pass the
	// responsibility to Vim in this case).
	எனில் (!PyErr_Occurred())
	{
	    போது (i < new_len)
	    {
		எனில் (ml_append((linenr_T)(lo + i - 1),
					(char_u *)array[i], 0, FALSE) == FAIL)
		{
		    RAISE_செருகு_வரி_FAIL;
		    நிறுத்து;
		}
		vim_free(array[i]);
		++i;
		++extra;
	    }
	}

	// Free any left-over old_len, as a result of an error
	போது (i < new_len)
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
	எனில் (save_curbuf.br_buf == வெற்று)
	{
	    mark_adjust((linenr_T)lo, (linenr_T)(hi - 1),
						  (long)MAXLNUM, (long)extra);
	    changed_வரிகள்((linenr_T)lo, 0, (linenr_T)hi, (long)extra);
	}

	எனில் (buf == curbuf && (switchwin.sw_curwin != வெற்று
					   || save_curbuf.br_buf == வெற்று))
	    // Using an existing window for the buffer, adjust the cursor
	    // position.
	    py_fix_cursor((linenr_T)lo, (linenr_T)hi, (linenr_T)extra);

	// END of region without "return".
	restore_win_for_buf(&switchwin, &save_curbuf);

	எனில் (VimTryEnd())
	    திரும்பு FAIL;

	எனில் (len_change)
	    *len_change = new_len - old_len;

	திரும்பு OK;
    }
    இல்லையெனில்
    {
	PyErr_BadArgument();
	திரும்பு FAIL;
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
    நிலையான முழு
செருகு_Buffer_வரிகள்(buf_T *buf, PyInt n, PyObject *வரிகள், PyInt *len_change)
{
    bufref_T	save_curbuf = {வெற்று, 0, 0};
    switchwin_T	switchwin;

    // First of all, we check the type of the supplied Python object.
    // It must be a string or a list, or the call is in error.
    எனில் (PyBytes_Check(வரிகள்) || PyUnicode_Check(வரிகள்))
    {
	எழுத்து		*str = String_To_வரி(வரிகள்);

	எனில் (str == வெற்று)
	    திரும்பு FAIL;

	PyErr_Clear();
	VimTryStart();
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	எனில் (u_save((linenr_T)n, (linenr_T)(n + 1)) == FAIL)
	    RAISE_UNDO_FAIL;
	இல்லையெனில் எனில் (ml_append((linenr_T)n, (char_u *)str, 0, FALSE) == FAIL)
	    RAISE_செருகு_வரி_FAIL;
	இல்லையெனில் எனில் (save_curbuf.br_buf == வெற்று)
	    // Only adjust marks if we managed to switch to a window that
	    // holds the buffer, otherwise line numbers will be invalid.
	    appended_வரிகள்_mark((linenr_T)n, 1L);

	vim_free(str);
	restore_win_for_buf(&switchwin, &save_curbuf);
	update_screen(UPD_VALID);

	எனில் (VimTryEnd())
	    திரும்பு FAIL;

	எனில் (len_change)
	    *len_change = 1;

	திரும்பு OK;
    }
    இல்லையெனில் எனில் (PyList_Check(வரிகள்))
    {
	PyInt	i;
	PyInt	size = PyList_Size(வரிகள்);
	எழுத்து	**array;

	array = PyMem_New(எழுத்து *, size);
	எனில் (array == வெற்று)
	{
	    PyErr_NoMemory();
	    திரும்பு FAIL;
	}

	க்கு (i = 0; i < size; ++i)
	{
	    PyObject *வரி;

	    எனில் (!(வரி = PyList_GetItem(வரிகள், i)) ||
		!(array[i] = String_To_வரி(வரி)))
	    {
		போது (i)
		    vim_free(array[--i]);
		PyMem_Free(array);
		திரும்பு FAIL;
	    }
	}

	PyErr_Clear();
	VimTryStart();
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	எனில் (u_save((linenr_T)n, (linenr_T)(n + 1)) == FAIL)
	    RAISE_UNDO_FAIL;
	இல்லையெனில்
	{
	    க்கு (i = 0; i < size; ++i)
	    {
		எனில் (ml_append((linenr_T)(n + i),
					(char_u *)array[i], 0, FALSE) == FAIL)
		{
		    RAISE_செருகு_வரி_FAIL;

		    // Free the rest of the lines
		    போது (i < size)
			vim_free(array[i++]);

		    நிறுத்து;
		}
		vim_free(array[i]);
	    }
	    எனில் (i > 0 && save_curbuf.br_buf == வெற்று)
		// Only adjust marks if we managed to switch to a window that
		// holds the buffer, otherwise line numbers will be invalid.
		appended_வரிகள்_mark((linenr_T)n, (long)i);
	}

	// Free the array of lines. All of its contents have now
	// been freed.
	PyMem_Free(array);
	restore_win_for_buf(&switchwin, &save_curbuf);

	update_screen(UPD_VALID);

	எனில் (VimTryEnd())
	    திரும்பு FAIL;

	எனில் (len_change)
	    *len_change = size;

	திரும்பு OK;
    }
    இல்லையெனில்
    {
	PyErr_BadArgument();
	திரும்பு FAIL;
    }
}

/*
 * Common routines for buffers and line ranges
 * -------------------------------------------
 */

வகைவரை கட்டமைப்பு
{
    PyObject_HEAD
    buf_T *buf;
} BufferObject;

    நிலையான முழு
CheckBuffer(BufferObject *self)
{
    எனில் (self->buf == INVALID_BUFFER_மதிப்பு)
    {
	PyErr_SET_VIM(N_("attempt to refer to deleted buffer"));
	திரும்பு -1;
    }

    திரும்பு 0;
}

    நிலையான முழு
CheckBufferCb(வெறுமை *self)
{
    திரும்பு CheckBuffer((BufferObject*)self);
}

    நிலையான PyObject *
RBItem(BufferObject *self, PyInt n, PyInt start, PyInt end)
{
    எனில் (CheckBuffer(self))
	திரும்பு வெற்று;

    எனில் (end == -1)
	end = self->buf->b_ml.ml_வரி_மொத்தம்;

    எனில் (n < 0)
	n += end - start + 1;

    எனில் (n < 0 || n > end - start)
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("line number out of range"));
	திரும்பு வெற்று;
    }

    திரும்பு Get_Buffer_வரி(self->buf, n+start);
}

    நிலையான PyObject *
RBSlice(BufferObject *self, PyInt lo, PyInt hi, PyInt start, PyInt end)
{
    PyInt size;

    எனில் (CheckBuffer(self))
	திரும்பு வெற்று;

    எனில் (end == -1)
	end = self->buf->b_ml.ml_வரி_மொத்தம்;

    size = end - start + 1;

    எனில் (lo < 0)
	lo = 0;
    இல்லையெனில் எனில் (lo > size)
	lo = size;
    எனில் (hi < 0)
	hi = 0;
    எனில் (hi < lo)
	hi = lo;
    இல்லையெனில் எனில் (hi > size)
	hi = size;

    திரும்பு Get_Buffer_வரி_List(self->buf, lo+start, hi+start);
}

    நிலையான PyInt
RBAsItem(
	BufferObject *self,
	PyInt n,
	PyObject *valObject,
	PyInt start,
	PyInt end,
	PyInt *new_end)
{
    PyInt len_change;

    எனில் (CheckBuffer(self))
	திரும்பு -1;

    எனில் (end == -1)
	end = self->buf->b_ml.ml_வரி_மொத்தம்;

    எனில் (n < 0)
	n += end - start + 1;

    எனில் (n < 0 || n > end - start)
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("line number out of range"));
	திரும்பு -1;
    }

    எனில் (Set_Buffer_வரி(self->buf, n+start, valObject, &len_change) == FAIL)
	திரும்பு -1;

    எனில் (new_end)
	*new_end = end + len_change;

    திரும்பு 0;
}

    நிலையான PyInt
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
    எனில் (CheckBuffer(self))
	திரும்பு -1;

    எனில் (end == -1)
	end = self->buf->b_ml.ml_வரி_மொத்தம்;

    // Sort out the slice range
    size = end - start + 1;

    எனில் (lo < 0)
	lo = 0;
    இல்லையெனில் எனில் (lo > size)
	lo = size;
    எனில் (hi < 0)
	hi = 0;
    எனில் (hi < lo)
	hi = lo;
    இல்லையெனில் எனில் (hi > size)
	hi = size;

    எனில் (Set_Buffer_வரி_List(self->buf, lo + start, hi + start,
						valObject, &len_change) == FAIL)
	திரும்பு -1;

    எனில் (new_end)
	*new_end = end + len_change;

    திரும்பு 0;
}


    நிலையான PyObject *
RBAppend(
	BufferObject *self,
	PyObject *args,
	PyInt start,
	PyInt end,
	PyInt *new_end)
{
    PyObject *வரிகள்;
    PyInt len_change;
    PyInt max;
    PyInt n;

    எனில் (CheckBuffer(self))
	திரும்பு வெற்று;

    எனில் (end == -1)
	end = self->buf->b_ml.ml_வரி_மொத்தம்;

    max = n = end - start + 1;

    எனில் (!PyArg_ParseTuple(args, "O|n", &வரிகள், &n))
	திரும்பு வெற்று;

    எனில் (n < 0 || n > max)
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("line number out of range"));
	திரும்பு வெற்று;
    }

    எனில் (செருகு_Buffer_வரிகள்(self->buf, n + start - 1, வரிகள், &len_change) == FAIL)
	திரும்பு வெற்று;

    எனில் (new_end)
	*new_end = end + len_change;

    Py_INCREF(Py_None);
    திரும்பு Py_None;
}

// Range object

DEFINE_PY_TYPE_OBJECT(RangeType);
நிலையான PySequenceMethods RangeAsSeq;
நிலையான PyMappingMethods RangeAsMapping;

வகைவரை கட்டமைப்பு
{
    PyObject_HEAD
    BufferObject *buf;
    PyInt start;
    PyInt end;
} RangeObject;

    நிலையான PyObject *
RangeNew(buf_T *buf, PyInt start, PyInt end)
{
    BufferObject *bufr;
    RangeObject *self;
    self = PyObject_GC_New(RangeObject, RangeTypePtr);
    எனில் (self == வெற்று)
	திரும்பு வெற்று;

    bufr = (BufferObject *)BufferNew(buf);
    எனில் (bufr == வெற்று)
    {
	Py_DECREF(self);
	திரும்பு வெற்று;
    }
    Py_INCREF((PyObject *)bufr);

    self->buf = bufr;
    self->start = start;
    self->end = end;

    திரும்பு (PyObject *)(self);
}

    நிலையான வெறுமை
RangeDestructor(PyObject *self_obj)
{
    RangeObject *self = (RangeObject*)self_obj;
    PyObject_GC_UnTrack((வெறுமை *)(self));
    Py_XDECREF(((PyObject *)(self->buf)));
    PyObject_GC_Del((வெறுமை *)(self));
}

    நிலையான முழு
RangeTraverse(PyObject *self_obj, visitproc visit, வெறுமை *arg)
{
    RangeObject *self = (RangeObject*)self_obj;
    Py_VISIT(((PyObject *)(self->buf)));
    திரும்பு 0;
}

    நிலையான முழு
RangeClear(PyObject *self_obj)
{
    RangeObject *self = (RangeObject*)self_obj;
    Py_CLEAR(self->buf);
    திரும்பு 0;
}

    நிலையான PyInt
RangeLength(RangeObject *self)
{
    // HOW DO WE SIGNAL AN ERROR FROM THIS FUNCTION?
    எனில் (CheckBuffer(self->buf))
	திரும்பு -1; // ???

    திரும்பு (self->end - self->start + 1);
}

    நிலையான PyObject *
RangeItem(RangeObject *self, PyInt n)
{
    திரும்பு RBItem(self->buf, n, self->start, self->end);
}

    நிலையான PyObject *
RangeSlice(RangeObject *self, PyInt lo, PyInt hi)
{
    திரும்பு RBSlice(self->buf, lo, hi, self->start, self->end);
}

நிலையான எழுத்து *RangeAttrs[] = {
    "start", "end",
    வெற்று
};

    நிலையான PyObject *
RangeDir(PyObject *self, PyObject *args UNUSED)
{
    திரும்பு ObjectDir(self, RangeAttrs);
}

    நிலையான PyObject *
RangeAppend(RangeObject *self, PyObject *args)
{
    திரும்பு RBAppend(self->buf, args, self->start, self->end, &self->end);
}

    நிலையான PyObject *
RangeRepr(PyObject *self_obj)
{
    RangeObject *self = (RangeObject*)self_obj;
    எனில் (self->buf->buf == INVALID_BUFFER_மதிப்பு)
	திரும்பு PyString_FromFormat("<range object (for deleted buffer) at %p>",
				    (வெறுமை *)self);
    இல்லையெனில்
    {
	எழுத்து *name = (எழுத்து *)self->buf->buf->b_fname;

	எனில் (name == வெற்று)
	    name = "";

	திரும்பு PyString_FromFormat("<range %s (%d:%d)>",
				    name, (முழு)self->start, (முழு)self->end);
    }
}

நிலையான கட்டமைப்பு PyMethodDef RangeMethods[] = {
    // name,	function,			calling,	documentation
    {"append",	(PyCFunction)RangeAppend,	METH_VARARGS,	"Append data to the Vim range" },
    {"__dir__",	(PyCFunction)RangeDir,		METH_NOARGS,	""},
    { வெற்று,	வெற்று,				0,		வெற்று}
};

DEFINE_PY_TYPE_OBJECT(BufferType);
நிலையான PySequenceMethods BufferAsSeq;
நிலையான PyMappingMethods BufferAsMapping;

    நிலையான PyObject *
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

    எனில் (BUF_PYTHON_REF(buf) != வெற்று)
    {
	self = BUF_PYTHON_REF(buf);
	Py_INCREF((PyObject *)self);
    }
    இல்லையெனில்
    {
	self = PyObject_NEW(BufferObject, BufferTypePtr);
	எனில் (self == வெற்று)
	    திரும்பு வெற்று;
	self->buf = buf;
	BUF_PYTHON_REF(buf) = self;
    }

    திரும்பு (PyObject *)(self);
}

    நிலையான வெறுமை
BufferDestructor(PyObject *self_obj)
{
    BufferObject *self = (BufferObject*)self_obj;
    எனில் (self->buf && self->buf != INVALID_BUFFER_மதிப்பு)
	BUF_PYTHON_REF(self->buf) = வெற்று;

    DESTRUCTOR_FINISH(self);
}

    நிலையான PyInt
BufferLength(BufferObject *self)
{
    // HOW DO WE SIGNAL AN ERROR FROM THIS FUNCTION?
    எனில் (CheckBuffer(self))
	திரும்பு -1; // ???

    திரும்பு (PyInt)(self->buf->b_ml.ml_வரி_மொத்தம்);
}

    நிலையான PyObject *
BufferItem(BufferObject *self, PyInt n)
{
    திரும்பு RBItem(self, n, 1, -1);
}

    நிலையான PyObject *
BufferSlice(BufferObject *self, PyInt lo, PyInt hi)
{
    திரும்பு RBSlice(self, lo, hi, 1, -1);
}

நிலையான எழுத்து *BufferAttrs[] = {
    "name", "number", "vars", "options", "valid",
    வெற்று
};

    நிலையான PyObject *
BufferDir(PyObject *self, PyObject *args UNUSED)
{
    திரும்பு ObjectDir(self, BufferAttrs);
}

    நிலையான PyObject *
BufferAttrValid(BufferObject *self, எழுத்து *name)
{
    PyObject	*ret;

    எனில் (strcmp(name, "valid") != 0)
	திரும்பு வெற்று;

    ret = ((self->buf == INVALID_BUFFER_மதிப்பு) ? Py_False : Py_True);
    Py_INCREF(ret);
    திரும்பு ret;
}

    நிலையான PyObject *
BufferAttr(BufferObject *self, எழுத்து *name)
{
    எனில் (strcmp(name, "name") == 0)
	திரும்பு PyString_FromString((self->buf->b_ffname == வெற்று
				    ? "" : (எழுத்து *)self->buf->b_ffname));
    இல்லையெனில் எனில் (strcmp(name, "number") == 0)
	திரும்பு Py_Build_மதிப்பு(Py_ssize_t_fmt, self->buf->b_fnum);
    இல்லையெனில் எனில் (strcmp(name, "vars") == 0)
	திரும்பு NEW_DICTIONARY(self->buf->b_vars);
    இல்லையெனில் எனில் (strcmp(name, "options") == 0)
	திரும்பு OptionsNew(SREQ_BUF, self->buf, CheckBufferCb,
			(PyObject *) self);
    இல்லையெனில் எனில் (strcmp(name, "__members__") == 0)
	திரும்பு ObjectDir(வெற்று, BufferAttrs);
    இல்லையெனில்
	திரும்பு வெற்று;
}

    நிலையான முழு
BufferSetattr(PyObject *self_obj, எழுத்து *name, PyObject *valObject)
{
    BufferObject *self = (BufferObject*)self_obj;
    எனில் (CheckBuffer(self))
	திரும்பு -1;

    எனில் (strcmp(name, "name") == 0)
    {
	char_u		*val;
	aco_save_T	aco;
	முழு		ren_ret = OK;
	PyObject	*todecref;

	எனில் (!(val = StringToChars(valObject, &todecref)))
	    திரும்பு -1;

	VimTryStart();
	// Using aucmd_*: autocommands will be executed by rename_buffer
	aucmd_prepbuf(&aco, self->buf);
	எனில் (curbuf == self->buf)
	{
	    ren_ret = rename_buffer(val);
	    aucmd_restbuf(&aco);
	}
	Py_XDECREF(todecref);
	எனில் (VimTryEnd())
	    திரும்பு -1;

	எனில் (ren_ret == FAIL)
	{
	    PyErr_SET_VIM(N_("failed to rename buffer"));
	    திரும்பு -1;
	}
	திரும்பு 0;
    }
    இல்லையெனில்
    {
	PyErr_SetString(PyExc_AttributeError, name);
	திரும்பு -1;
    }
}

    நிலையான PyObject *
BufferAppend(BufferObject *self, PyObject *args)
{
    திரும்பு RBAppend(self, args, 1, -1, வெற்று);
}

    நிலையான PyObject *
BufferMark(BufferObject *self, PyObject *pmarkObject)
{
    pos_T	*posp;
    char_u	*pmark;
    char_u	mark;
    bufref_T	savebuf;
    PyObject	*todecref;

    எனில் (CheckBuffer(self))
	திரும்பு வெற்று;

    எனில் (!(pmark = StringToChars(pmarkObject, &todecref)))
	திரும்பு வெற்று;

    எனில் (pmark[0] == '\0' || pmark[1] != '\0')
    {
	PyErr_SET_STRING(Py_Exc_மதிப்பு_Error,
		N_("mark name must be a single character"));
	Py_XDECREF(todecref);
	திரும்பு வெற்று;
    }

    mark = *pmark;

    Py_XDECREF(todecref);

    VimTryStart();
    switch_buffer(&savebuf, self->buf);
    posp = getmark(mark, FALSE);
    restore_buffer(&savebuf);
    எனில் (VimTryEnd())
	திரும்பு வெற்று;

    எனில் (posp == வெற்று)
    {
	PyErr_SET_VIM(N_("invalid mark name"));
	திரும்பு வெற்று;
    }

    எனில் (posp->lnum <= 0)
    {
	// Or raise an error?
	Py_INCREF(Py_None);
	திரும்பு Py_None;
    }

    திரும்பு Py_Build_மதிப்பு("(ll)", (long)(posp->lnum), (long)(posp->col));
}

    நிலையான PyObject *
BufferRange(BufferObject *self, PyObject *args)
{
    PyInt start;
    PyInt end;

    எனில் (CheckBuffer(self))
	திரும்பு வெற்று;

    எனில் (!PyArg_ParseTuple(args, "nn", &start, &end))
	திரும்பு வெற்று;

    திரும்பு RangeNew(self->buf, start, end);
}

    நிலையான PyObject *
BufferRepr(PyObject *self_obj)
{
    BufferObject *self = (BufferObject*)self_obj;
    எனில் (self->buf == INVALID_BUFFER_மதிப்பு)
	திரும்பு PyString_FromFormat("<buffer object (deleted) at %p>", (வெறுமை *)self);
    இல்லையெனில்
    {
	எழுத்து	*name = (எழுத்து *)self->buf->b_fname;

	எனில் (name == வெற்று)
	    name = "";

	திரும்பு PyString_FromFormat("<buffer %s>", name);
    }
}

நிலையான கட்டமைப்பு PyMethodDef BufferMethods[] = {
    // name,	    function,			calling,	documentation
    {"append",	    (PyCFunction)BufferAppend,	METH_VARARGS,	"Append data to Vim buffer" },
    {"mark",	    (PyCFunction)BufferMark,	METH_O,		"Return (row,col) representing position of named mark" },
    {"range",	    (PyCFunction)BufferRange,	METH_VARARGS,	"Return a range object which represents the part of the given buffer between line numbers s and e" },
    {"__dir__",	    (PyCFunction)BufferDir,	METH_NOARGS,	""},
    { வெற்று,	    வெற்று,			0,		வெற்று}
};

/*
 * Buffer list object - Implementation
 */

DEFINE_PY_TYPE_OBJECT(BufMapType);

வகைவரை கட்டமைப்பு
{
    PyObject_HEAD
} BufMapObject;

நிலையான BufMapObject TheBufferMap =
{
    PyObject_HEAD_INIT_TYPE(BufMapType)
};

    நிலையான PyInt
BufMapLength(PyObject *self UNUSED)
{
    buf_T	*b = firstbuf;
    PyInt	n = 0;

    போது (b)
    {
	++n;
	b = b->b_next;
    }

    திரும்பு n;
}

    நிலையான PyObject *
BufMapItem(PyObject *self UNUSED, PyObject *சாவி_Object)
{
    buf_T	*b;
    long	bnr;

    எனில் (எண்_To_Long(சாவி_Object, &bnr, எண்_INT|எண்_NATURAL))
	திரும்பு வெற்று;

    b = buflist_findnr((முழு) bnr);

    எனில் (b)
	திரும்பு BufferNew(b);
    இல்லையெனில்
    {
	PyErr_SetObject(Py_Exc_சாவி_Error, சாவி_Object);
	திரும்பு வெற்று;
    }
}

    நிலையான வெறுமை
BufMapIterDestruct(வெறுமை* arg)
{
    PyObject *buffer = (PyObject*)arg;
    // Iteration was stopped before all buffers were processed
    எனில் (buffer)
    {
	Py_DECREF(buffer);
    }
}

    நிலையான முழு
BufMapIterTraverse(வெறுமை *iter, visitproc visit, வெறுமை *arg)
{
    PyObject *buffer = (PyObject*)iter;
    எனில் (buffer)
	Py_VISIT(buffer);
    திரும்பு 0;
}

    நிலையான முழு
BufMapIterClear(வெறுமை **iter)
{
    PyObject **buffer = (PyObject**)iter;
    எனில் (*buffer)
	Py_CLEAR(*buffer);
    திரும்பு 0;
}

    நிலையான PyObject *
BufMapIterNext(வெறுமை **arg)
{
    PyObject	*next;
    PyObject	*ret;
    PyObject	**buffer = (PyObject**)arg;

    எனில் (!*buffer)
	திரும்பு வெற்று;

    ret = *buffer;

    எனில் (CheckBuffer((BufferObject *)(ret)))
    {
	*buffer = வெற்று;
	திரும்பு வெற்று;
    }

    எனில் (!((BufferObject *)(ret))->buf->b_next)
	next = வெற்று;
    இல்லையெனில் எனில் (!(next = BufferNew(((BufferObject *)(ret))->buf->b_next)))
	திரும்பு வெற்று;
    *buffer = next;
    // Do not increment reference: we no longer hold it (decref), but whoever
    // on other side will hold (incref). Decref+incref = nothing.
    திரும்பு ret;
}

    நிலையான PyObject *
BufMapIter(PyObject *self)
{
    PyObject *buffer;

    buffer = BufferNew(firstbuf);
    திரும்பு IterNew(buffer,
	    BufMapIterDestruct, BufMapIterNext,
	    BufMapIterTraverse, BufMapIterClear,
	    self);
}

நிலையான PyMappingMethods BufMapAsMapping = {
    (lenfunc)       BufMapLength,
    (binaryfunc)    BufMapItem,
    (objobjargproc) 0,
};

// Current items object

நிலையான எழுத்து *CurrentAttrs[] = {
    "buffer", "window", "line", "range", "tabpage",
    வெற்று
};

    நிலையான PyObject *
CurrentDir(PyObject *self, PyObject *args UNUSED)
{
    திரும்பு ObjectDir(self, CurrentAttrs);
}

    நிலையான PyObject *
CurrentGetattr(PyObject *self UNUSED, எழுத்து *name)
{
    எனில் (strcmp(name, "buffer") == 0)
	திரும்பு (PyObject *)BufferNew(curbuf);
    இல்லையெனில் எனில் (strcmp(name, "window") == 0)
	திரும்பு (PyObject *)WindowNew(curwin, curtab);
    இல்லையெனில் எனில் (strcmp(name, "tabpage") == 0)
	திரும்பு (PyObject *)TabPageNew(curtab);
    இல்லையெனில் எனில் (strcmp(name, "line") == 0)
	திரும்பு Get_Buffer_வரி(curbuf, (PyInt)curwin->w_cursor.lnum);
    இல்லையெனில் எனில் (strcmp(name, "range") == 0)
	திரும்பு RangeNew(curbuf, RangeStart, RangeEnd);
    இல்லையெனில் எனில் (strcmp(name, "__members__") == 0)
	திரும்பு ObjectDir(வெற்று, CurrentAttrs);
    இல்லையெனில்
#எனில் PY_MAJOR_VERSION < 3
	திரும்பு Py_FindMethod(WindowMethods, self, name);
#இல்லையெனில்
	திரும்பு வெற்று;
#endif
}

    நிலையான முழு
CurrentSetattr(PyObject *self UNUSED, எழுத்து *name, PyObject *valObject)
{
    எனில் (strcmp(name, "line") == 0)
    {
	எனில் (Set_Buffer_வரி(curbuf, (PyInt)curwin->w_cursor.lnum, valObject,
			  வெற்று) == FAIL)
	    திரும்பு -1;

	திரும்பு 0;
    }
    இல்லையெனில் எனில் (strcmp(name, "buffer") == 0)
    {
	முழு மொத்தம்;

	எனில் (valObject->ob_type != BufferTypePtr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.Buffer object, but got %s"),
		    valObject);
	    திரும்பு -1;
	}

	எனில் (CheckBuffer((BufferObject *)(valObject)))
	    திரும்பு -1;
	மொத்தம் = ((BufferObject *)(valObject))->buf->b_fnum;

	VimTryStart();
	எனில் (do_buffer(DOBUF_GOTO, DOBUF_FIRST, FORWARD, மொத்தம், 0) == FAIL)
	{
	    எனில் (VimTryEnd())
		திரும்பு -1;
	    PyErr_VIM_FORMAT(N_("failed to switch to buffer %d"), மொத்தம்);
	    திரும்பு -1;
	}

	திரும்பு VimTryEnd();
    }
    இல்லையெனில் எனில் (strcmp(name, "window") == 0)
    {
	முழு மொத்தம்;

	எனில் (valObject->ob_type != WindowTypePtr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.Window object, but got %s"),
		    valObject);
	    திரும்பு -1;
	}

	எனில் (CheckWindow((WindowObject *)(valObject)))
	    திரும்பு -1;
	மொத்தம் = get_win_எண்(((WindowObject *)(valObject))->win, firstwin);

	எனில் (!மொத்தம்)
	{
	    PyErr_SET_STRING(Py_Exc_மதிப்பு_Error,
		    N_("failed to find window in the current tab page"));
	    திரும்பு -1;
	}

	VimTryStart();
	win_goto(((WindowObject *)(valObject))->win);
	எனில் (((WindowObject *)(valObject))->win != curwin)
	{
	    எனில் (VimTryEnd())
		திரும்பு -1;
	    PyErr_SET_STRING(PyExc_RuntimeError,
		    N_("did not switch to the specified window"));
	    திரும்பு -1;
	}

	திரும்பு VimTryEnd();
    }
    இல்லையெனில் எனில் (strcmp(name, "tabpage") == 0)
    {
	எனில் (valObject->ob_type != TabPageTypePtr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.TabPage object, but got %s"),
		    valObject);
	    திரும்பு -1;
	}

	எனில் (CheckTabPage((TabPageObject *)(valObject)))
	    திரும்பு -1;

	VimTryStart();
	goto_tabpage_tp(((TabPageObject *)(valObject))->tab, TRUE, TRUE);
	எனில் (((TabPageObject *)(valObject))->tab != curtab)
	{
	    எனில் (VimTryEnd())
		திரும்பு -1;
	    PyErr_SET_STRING(PyExc_RuntimeError,
		    N_("did not switch to the specified tab page"));
	    திரும்பு -1;
	}

	திரும்பு VimTryEnd();
    }
    இல்லையெனில்
    {
	PyErr_SetString(PyExc_AttributeError, name);
	திரும்பு -1;
    }
}

நிலையான கட்டமைப்பு PyMethodDef CurrentMethods[] = {
    // name,	    function,			calling,	documentation
    {"__dir__",	    (PyCFunction)CurrentDir,	METH_NOARGS,	""},
    { வெற்று,	    வெற்று,			0,		வெற்று}
};

    நிலையான வெறுமை
init_range_cmd(வெறுமை *arg)
{
    exarg_T *eap = (exarg_T*)arg;
    RangeStart = eap->line1;
    RangeEnd = eap->line2;
}

    நிலையான வெறுமை
init_range_eval(வெறுமை *rettv UNUSED)
{
    RangeStart = (PyInt) curwin->w_cursor.lnum;
    RangeEnd = RangeStart;
}

    நிலையான வெறுமை
run_cmd(மாறிலி எழுத்து *cmd, dict_T* locals UNUSED, வெறுமை *arg UNUSED
#ifdef PY_CAN_RECURSE
	, PyGILState_STATE *pygilstate UNUSED
#endif
	)
{
    PyObject	*run_ret;
    run_ret = PyRun_String((எழுத்து *)cmd, Py_file_உள்ளீடு, globals, globals);
    எனில் (run_ret != வெற்று)
    {
	Py_DECREF(run_ret);
    }
    இல்லையெனில் எனில் (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
    {
	emsg(_(e_cant_handle_systemexit_of_python_exception_in_vim));
	PyErr_Clear();
    }
    இல்லையெனில்
	PyErr_PrintEx(1);
}

நிலையான மாறிலி எழுத்து	*code_hdr = "def " DOPY_FUNC "(line, linenr):\n ";
நிலையான முழு		code_hdr_len = 30;

    நிலையான வெறுமை
run_do(மாறிலி எழுத்து *cmd, dict_T* locals UNUSED, வெறுமை *arg UNUSED
#ifdef PY_CAN_RECURSE
	, PyGILState_STATE *pygilstate
#endif
	)
{
    PyInt	lnum;
    size_t	len;
    எழுத்து	*code;
    முழு		status;
    PyObject	*pyfunc, *pymain;
    PyObject	*run_ret;
    buf_T	*was_curbuf = curbuf;

    எனில் (u_save((linenr_T)RangeStart - 1, (linenr_T)RangeEnd + 1) != OK)
    {
	emsg(_("cannot save undo information"));
	திரும்பு;
    }

    len = code_hdr_len + STRLEN(cmd);
    code = PyMem_New(எழுத்து, len + 1);
    memcpy(code, code_hdr, code_hdr_len);
    STRCPY(code + code_hdr_len, cmd);
    run_ret = PyRun_String(code, Py_file_உள்ளீடு, globals, globals);
    status = -1;
    எனில் (run_ret != வெற்று)
    {
	status = 0;
	Py_DECREF(run_ret);
    }
    இல்லையெனில் எனில் (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
    {
	PyMem_Free(code);
	emsg(_(e_cant_handle_systemexit_of_python_exception_in_vim));
	PyErr_Clear();
	திரும்பு;
    }
    இல்லையெனில்
	PyErr_PrintEx(1);

    PyMem_Free(code);

    எனில் (status)
    {
	emsg(_("failed to run the code"));
	திரும்பு;
    }

    status = 0;
    pymain = PyImport_AddModule("__main__");
    pyfunc = PyObject_GetAttrString(pymain, DOPY_FUNC);
#ifdef PY_CAN_RECURSE
    PyGILState_Release(*pygilstate);
#endif

    க்கு (lnum = RangeStart; lnum <= RangeEnd; ++lnum)
    {
	PyObject	*வரி;
	PyObject	*linenr;
	PyObject	*ret;

#ifdef PY_CAN_RECURSE
	*pygilstate = PyGILState_Ensure();
#endif
	// Check the line number, the command my have deleted lines.
	எனில் (lnum > curbuf->b_ml.ml_வரி_மொத்தம்
		|| !(வரி = Get_Buffer_வரி(curbuf, lnum)))
	    செல் err;
	எனில் (!(linenr = PyInt_FromLong((long) lnum)))
	{
	    Py_DECREF(வரி);
	    செல் err;
	}
	ret = PyObject_CallFunctionObjArgs(pyfunc, வரி, linenr, வெற்று);
	Py_DECREF(வரி);
	Py_DECREF(linenr);
	எனில் (!ret)
	    செல் err;

	// Check that the command didn't switch to another buffer.
	// Check the line number, the command my have deleted lines.
	எனில் (curbuf != was_curbuf || lnum > curbuf->b_ml.ml_வரி_மொத்தம்)
	{
	    Py_XDECREF(ret);
	    செல் err;
	}

	எனில் (ret != Py_None)
	    எனில் (Set_Buffer_வரி(curbuf, lnum, ret, வெற்று) == FAIL)
	    {
		Py_XDECREF(ret);
		செல் err;
	    }

	Py_XDECREF(ret);
	PythonIO_Flush();
#ifdef PY_CAN_RECURSE
	PyGILState_Release(*pygilstate);
#endif
    }
    செல் out;
err:
#ifdef PY_CAN_RECURSE
    *pygilstate = PyGILState_Ensure();
#endif
    PyErr_PrintEx(0);
    PythonIO_Flush();
    status = 1;
out:
#ifdef PY_CAN_RECURSE
    எனில் (!status)
	*pygilstate = PyGILState_Ensure();
#endif
    Py_DECREF(pyfunc);
    PyObject_SetAttrString(pymain, DOPY_FUNC, வெற்று);
    எனில் (status)
	திரும்பு;
    check_cursor();
    update_curbuf(UPD_NOT_VALID);
}

    நிலையான வெறுமை
run_eval(மாறிலி எழுத்து *cmd, dict_T *locals, வெறுமை *arg
#ifdef PY_CAN_RECURSE
	, PyGILState_STATE *pygilstate UNUSED
#endif
	)
{
    PyObject	*run_ret;
    typval_T	*rettv = (typval_T*)arg;
    PyObject	*pylocals = locals ? NEW_DICTIONARY(locals) : globals;

    run_ret = PyRun_String((எழுத்து *)cmd, Py_eval_உள்ளீடு, globals, pylocals);
    எனில் (run_ret == வெற்று)
    {
	எனில் (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
	{
	    emsg(_(e_cant_handle_systemexit_of_python_exception_in_vim));
	    PyErr_Clear();
	}
	இல்லையெனில்
	{
	    எனில் (PyErr_Occurred() && !செய்தி_silent)
		PyErr_PrintEx(0);
	    emsg(_(e_eval_did_not_return_valid_python_object));
	}
    }
    இல்லையெனில்
    {
	எனில் (ConvertFromPyObject(run_ret, rettv) == -1)
	    emsg(_(e_failed_to_convert_returned_python_object_to_vim_மதிப்பு));
	Py_DECREF(run_ret);
    }
    PyErr_Clear();
}

    நிலையான முழு
set_ref_in_py(மாறிலி முழு copyID)
{
    pylinkedlist_T	*cur;
    list_T		*ll;
    tuple_T		*tt;
    முழு			i;
    முழு			abort = FALSE;
    FunctionObject	*func;

    எனில் (lastdict != வெற்று)
    {
	க்கு (cur = lastdict ; !abort && cur != வெற்று ; cur = cur->pll_prev)
	    abort = set_ref_in_dict(((DictionaryObject *)(cur->pll_obj))->dict,
								       copyID);
    }

    எனில் (lastlist != வெற்று)
    {
	க்கு (cur = lastlist ; !abort && cur != வெற்று ; cur = cur->pll_prev)
	{
	    ll = ((ListObject *) (cur->pll_obj))->list;
	    abort = set_ref_in_list(ll, copyID);
	}
    }

    எனில் (lasttuple != வெற்று)
    {
	க்கு (cur = lasttuple ; !abort && cur != வெற்று ; cur = cur->pll_prev)
	{
	    tt = ((TupleObject *) (cur->pll_obj))->tuple;
	    abort = set_ref_in_tuple(tt, copyID);
	}
    }

    எனில் (lastfunc != வெற்று)
    {
	க்கு (cur = lastfunc ; !abort && cur != வெற்று ; cur = cur->pll_prev)
	{
	    func = (FunctionObject *) cur->pll_obj;
	    abort = set_ref_in_dict(func->self, copyID);
	    எனில் (func->argc)
		க்கு (i = 0; !abort && i < func->argc; ++i)
		    abort = abort
			|| set_ref_in_item(&func->argv[i], copyID, வெற்று, வெற்று, வெற்று);
	}
    }

    திரும்பு abort;
}

    நிலையான முழு
set_string_copy(char_u *str, typval_T *tv)
{
    tv->vval.v_string = vim_strsave(str);
    எனில் (tv->vval.v_string == வெற்று)
    {
	PyErr_NoMemory();
	திரும்பு -1;
    }
    திரும்பு 0;
}

    நிலையான முழு
pydict_to_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    dict_T	*dict;
    char_u	*சாவி;
    dictitem_T	*di;
    PyObject	*சாவி_Object;
    PyObject	*valObject;
    Py_ssize_t	iter = 0;

    எனில் (!(dict = py_dict_alloc()))
	திரும்பு -1;

    tv->v_type = VAR_DICT;
    tv->vval.v_dict = dict;

    போது (PyDict_Next(obj, &iter, &சாவி_Object, &valObject))
    {
	PyObject	*todecref = வெற்று;

	எனில் (சாவி_Object == வெற்று || valObject == வெற்று)
	{
	    dict_unref(dict);
	    திரும்பு -1;
	}

	எனில் (!(சாவி = StringToChars(சாவி_Object, &todecref)))
	{
	    dict_unref(dict);
	    திரும்பு -1;
	}

	எனில் (*சாவி == NUL)
	{
	    dict_unref(dict);
	    Py_XDECREF(todecref);
	    RAISE_NO_EMPTY_சாவிகள்;
	    திரும்பு -1;
	}

	di = dictitem_alloc(சாவி);

	Py_XDECREF(todecref);

	எனில் (di == வெற்று)
	{
	    PyErr_NoMemory();
	    dict_unref(dict);
	    திரும்பு -1;
	}

	எனில் (_ConvertFromPyObject(valObject, &di->di_tv, lookup_dict) == -1)
	{
	    vim_free(di);
	    dict_unref(dict);
	    திரும்பு -1;
	}

	எனில் (dict_add(dict, di) == FAIL)
	{
	    RAISE_சாவி_ADD_FAIL(di->di_சாவி);
	    clear_tv(&di->di_tv);
	    vim_free(di);
	    dict_unref(dict);
	    திரும்பு -1;
	}
    }

    --dict->dv_refcount;
    திரும்பு 0;
}

    நிலையான முழு
pymap_to_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    dict_T	*dict;
    char_u	*சாவி;
    dictitem_T	*di;
    PyObject	*list;
    PyObject	*iterator;
    PyObject	*சாவி_Object;
    PyObject	*valObject;

    எனில் (!(dict = py_dict_alloc()))
	திரும்பு -1;

    tv->v_type = VAR_DICT;
    tv->vval.v_dict = dict;

    எனில் (!(list = Py_Mapping_சாவிகள்(obj)))
    {
	dict_unref(dict);
	திரும்பு -1;
    }

    எனில் (!(iterator = PyObject_GetIter(list)))
    {
	dict_unref(dict);
	Py_DECREF(list);
	திரும்பு -1;
    }
    Py_DECREF(list);

    போது ((சாவி_Object = PyIter_Next(iterator)))
    {
	PyObject	*todecref;

	எனில் (!(சாவி = StringToChars(சாவி_Object, &todecref)))
	{
	    Py_DECREF(சாவி_Object);
	    Py_DECREF(iterator);
	    dict_unref(dict);
	    திரும்பு -1;
	}

	எனில் (*சாவி == NUL)
	{
	    Py_DECREF(சாவி_Object);
	    Py_DECREF(iterator);
	    Py_XDECREF(todecref);
	    dict_unref(dict);
	    RAISE_NO_EMPTY_சாவிகள்;
	    திரும்பு -1;
	}

	எனில் (!(valObject = PyObject_GetItem(obj, சாவி_Object)))
	{
	    Py_DECREF(சாவி_Object);
	    Py_DECREF(iterator);
	    Py_XDECREF(todecref);
	    dict_unref(dict);
	    திரும்பு -1;
	}

	di = dictitem_alloc(சாவி);

	Py_DECREF(சாவி_Object);
	Py_XDECREF(todecref);

	எனில் (di == வெற்று)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(valObject);
	    dict_unref(dict);
	    PyErr_NoMemory();
	    திரும்பு -1;
	}

	எனில் (_ConvertFromPyObject(valObject, &di->di_tv, lookup_dict) == -1)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(valObject);
	    vim_free(di);
	    dict_unref(dict);
	    திரும்பு -1;
	}

	Py_DECREF(valObject);

	எனில் (dict_add(dict, di) == FAIL)
	{
	    RAISE_சாவி_ADD_FAIL(di->di_சாவி);
	    Py_DECREF(iterator);
	    dictitem_free(di);
	    dict_unref(dict);
	    திரும்பு -1;
	}
    }
    Py_DECREF(iterator);
    --dict->dv_refcount;
    திரும்பு 0;
}

    நிலையான முழு
pyseq_to_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    list_T	*l;

    எனில் (!(l = py_list_alloc()))
	திரும்பு -1;

    tv->v_type = VAR_LIST;
    tv->vval.v_list = l;

    எனில் (list_py_concat(l, obj, lookup_dict) == -1)
    {
	list_unref(l);
	திரும்பு -1;
    }

    --l->lv_refcount;
    திரும்பு 0;
}

    நிலையான முழு
pytuple_to_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    tuple_T	*t;

    எனில் (!(t = py_tuple_alloc()))
	திரும்பு -1;

    tv->v_type = VAR_TUPLE;
    tv->vval.v_tuple = t;

    எனில் (tuple_py_concat(t, obj, lookup_dict) == -1)
    {
	tuple_unref(t);
	திரும்பு -1;
    }

    --t->tv_refcount;
    திரும்பு 0;
}

வகைவரை முழு (*pytotvfunc)(PyObject *, typval_T *, PyObject *);

    நிலையான முழு
convert_dl(PyObject *obj, typval_T *tv,
				    pytotvfunc py_to_tv, PyObject *lookup_dict)
{
    PyObject	*capsule;
    எழுத்து	hexBuf[அளவு(வெறுமை *) * 2 + 3];

    sprintf(hexBuf, "%p", (வெறுமை *)obj);

#ifdef PY_USE_CAPSULE
    capsule = PyDict_GetItemString(lookup_dict, hexBuf);
#இல்லையெனில்
    capsule = (PyObject *)PyDict_GetItemString(lookup_dict, hexBuf);
#endif
    எனில் (capsule == வெற்று)
    {
#ifdef PY_USE_CAPSULE
	capsule = PyCapsule_New(tv, வெற்று, வெற்று);
#இல்லையெனில்
	capsule = PyCObject_FromVoidPtr(tv, வெற்று);
#endif
	எனில் (PyDict_SetItemString(lookup_dict, hexBuf, capsule))
	{
	    Py_DECREF(capsule);
	    tv->v_type = VAR_UNKNOWN;
	    திரும்பு -1;
	}

	Py_DECREF(capsule);

	எனில் (py_to_tv(obj, tv, lookup_dict) == -1)
	{
	    tv->v_type = VAR_UNKNOWN;
	    திரும்பு -1;
	}
	// As we are not using copy_tv which increments reference count we must
	// do it ourself.
	எனில் (tv->v_type == VAR_DICT)
	    ++tv->vval.v_dict->dv_refcount;
	இல்லையெனில் எனில் (tv->v_type == VAR_LIST)
	    ++tv->vval.v_list->lv_refcount;
	இல்லையெனில் எனில் (tv->v_type == VAR_TUPLE)
	    ++tv->vval.v_tuple->tv_refcount;
    }
    இல்லையெனில்
    {
	typval_T	*v;

#ifdef PY_USE_CAPSULE
	v = PyCapsule_GetPointer(capsule, வெற்று);
#இல்லையெனில்
	v = PyCObject_AsVoidPtr(capsule);
#endif
	copy_tv(v, tv);
    }
    திரும்பு 0;
}

    நிலையான முழு
ConvertFromPyMapping(PyObject *obj, typval_T *tv)
{
    PyObject	*lookup_dict;
    முழு		ret;

    எனில் (!(lookup_dict = PyDict_New()))
	திரும்பு -1;

    எனில் (PyType_IsSubtype(obj->ob_type, DictionaryTypePtr))
    {
	tv->v_type = VAR_DICT;
	tv->vval.v_dict = (((DictionaryObject *)(obj))->dict);
	++tv->vval.v_dict->dv_refcount;
	ret = 0;
    }
    இல்லையெனில் எனில் (PyDict_Check(obj))
	ret = convert_dl(obj, tv, pydict_to_tv, lookup_dict);
    இல்லையெனில் எனில் (PyMapping_Check(obj))
	ret = convert_dl(obj, tv, pymap_to_tv, lookup_dict);
    இல்லையெனில்
    {
	PyErr_FORMAT_TYPE(
		N_("unable to convert %s to a Vim dictionary"),
		obj);
	ret = -1;
    }
    Py_DECREF(lookup_dict);
    திரும்பு ret;
}

    நிலையான முழு
ConvertFromPySequence(PyObject *obj, typval_T *tv)
{
    PyObject	*lookup_dict;
    முழு		ret;

    எனில் (!(lookup_dict = PyDict_New()))
	திரும்பு -1;

    எனில் (PyType_IsSubtype(obj->ob_type, ListTypePtr))
    {
	tv->v_type = VAR_LIST;
	tv->vval.v_list = (((ListObject *)(obj))->list);
	++tv->vval.v_list->lv_refcount;
	ret = 0;
    }
    இல்லையெனில் எனில் (PyIter_Check(obj) || PySequence_Check(obj))
	ret = convert_dl(obj, tv, pyseq_to_tv, lookup_dict);
    இல்லையெனில்
    {
	PyErr_FORMAT_TYPE(
		N_("unable to convert %s to a Vim list"),
		obj);
	ret = -1;
    }
    Py_DECREF(lookup_dict);
    திரும்பு ret;
}

    நிலையான முழு
ConvertFromPyObject(PyObject *obj, typval_T *tv)
{
    PyObject	*lookup_dict;
    முழு		ret;

    எனில் (!(lookup_dict = PyDict_New()))
	திரும்பு -1;
    ret = _ConvertFromPyObject(obj, tv, lookup_dict);
    Py_DECREF(lookup_dict);
    திரும்பு ret;
}

    நிலையான முழு
_ConvertFromPyObject(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    எனில் (PyType_IsSubtype(obj->ob_type, DictionaryTypePtr))
    {
	tv->v_type = VAR_DICT;
	tv->vval.v_dict = (((DictionaryObject *)(obj))->dict);
	++tv->vval.v_dict->dv_refcount;
    }
    இல்லையெனில் எனில் (PyType_IsSubtype(obj->ob_type, ListTypePtr))
    {
	tv->v_type = VAR_LIST;
	tv->vval.v_list = (((ListObject *)(obj))->list);
	++tv->vval.v_list->lv_refcount;
    }
    இல்லையெனில் எனில் (PyType_IsSubtype(obj->ob_type, TupleTypePtr))
    {
	tv->v_type = VAR_TUPLE;
	tv->vval.v_tuple = (((TupleObject *)(obj))->tuple);
	++tv->vval.v_tuple->tv_refcount;
    }
    இல்லையெனில் எனில் (PyType_IsSubtype(obj->ob_type, FunctionTypePtr))
    {
	FunctionObject *func = (FunctionObject *) obj;
	எனில் (func->self != வெற்று || func->argv != வெற்று)
	{
	    partial_T *pt = ALLOC_CLEAR_ONE(partial_T);

	    set_partial(func, pt, TRUE);
	    tv->vval.v_partial = pt;
	    tv->v_type = VAR_PARTIAL;
	}
	இல்லையெனில்
	{
	    எனில் (set_string_copy(func->name, tv) == -1)
		திரும்பு -1;

	    tv->v_type = VAR_FUNC;
	}
	func_ref(func->name);
    }
    இல்லையெனில் எனில் (PyBytes_Check(obj))
    {
	char_u	*str;

	எனில் (PyBytes_AsStringAndSize(obj, (எழுத்து **) &str, வெற்று) == -1)
	    திரும்பு -1;
	எனில் (str == வெற்று)
	    திரும்பு -1;

	எனில் (set_string_copy(str, tv) == -1)
	    திரும்பு -1;

	tv->v_type = VAR_STRING;
    }
    இல்லையெனில் எனில் (PyUnicode_Check(obj))
    {
	PyObject	*bytes;
	char_u	*str;

	bytes = PyUnicode_AsEncodedString(obj, ENC_OPT, ERRORS_ENCODE_ARG);
	எனில் (bytes == வெற்று)
	    திரும்பு -1;

	எனில் (PyBytes_AsStringAndSize(bytes, (எழுத்து **) &str, வெற்று) == -1)
	    திரும்பு -1;
	எனில் (str == வெற்று)
	    திரும்பு -1;

	எனில் (set_string_copy(str, tv))
	{
	    Py_XDECREF(bytes);
	    திரும்பு -1;
	}
	Py_XDECREF(bytes);

	tv->v_type = VAR_STRING;
    }
#எனில் PY_MAJOR_VERSION < 3
    இல்லையெனில் எனில் (PyInt_Check(obj))
    {
	tv->v_type = VAR_எண்;
	tv->vval.v_எண் = (varnumber_T) PyInt_AsLong(obj);
	எனில் (PyErr_Occurred())
	    திரும்பு -1;
    }
#endif
    இல்லையெனில் எனில் (PyLong_Check(obj))
    {
	tv->v_type = VAR_எண்;
	tv->vval.v_எண் = (varnumber_T) PyLong_AsLong(obj);
	எனில் (PyErr_Occurred())
	    திரும்பு -1;
    }
    இல்லையெனில் எனில் (PyTuple_Check(obj))
	திரும்பு convert_dl(obj, tv, pytuple_to_tv, lookup_dict);
    இல்லையெனில் எனில் (PyDict_Check(obj))
	திரும்பு convert_dl(obj, tv, pydict_to_tv, lookup_dict);
    இல்லையெனில் எனில் (PyFloat_Check(obj))
    {
	tv->v_type = VAR_FLOAT;
	tv->vval.v_float = (float_T) PyFloat_AsDouble(obj);
    }
    இல்லையெனில் எனில் (PyObject_HasAttrString(obj, "keys"))
	திரும்பு convert_dl(obj, tv, pymap_to_tv, lookup_dict);
    // PyObject_GetIter can create built-in iterator for any sequence object
    இல்லையெனில் எனில் (PyIter_Check(obj) || PySequence_Check(obj))
	திரும்பு convert_dl(obj, tv, pyseq_to_tv, lookup_dict);
    இல்லையெனில் எனில் (PyMapping_Check(obj))
	திரும்பு convert_dl(obj, tv, pymap_to_tv, lookup_dict);
    இல்லையெனில் எனில் (Py_எண்_Check(obj))
    {
	PyObject	*இலக்கம்;

	எனில் (!(இலக்கம் = Py_எண்_Long(obj)))
	    திரும்பு -1;

	tv->v_type = VAR_எண்;
	tv->vval.v_எண் = (varnumber_T) PyLong_AsLong(இலக்கம்);

	Py_DECREF(இலக்கம்);
    }
    இல்லையெனில் எனில் (obj == Py_None)
    {
	tv->v_type = VAR_SPECIAL;
	tv->vval.v_எண் = VVAL_NONE;
    }
    இல்லையெனில்
    {
	PyErr_FORMAT_TYPE(
		N_("unable to convert %s to a Vim structure"),
		obj);
	திரும்பு -1;
    }
    திரும்பு 0;
}

    நிலையான PyObject *
ConvertToPyObject(typval_T *tv)
{
    typval_T *argv;
    முழு i;
    எனில் (tv == வெற்று)
    {
	PyErr_SET_VIM(N_("internal error: NULL reference passed"));
	திரும்பு வெற்று;
    }
    தேர்வு (tv->v_type)
    {
	நிலை VAR_STRING:
	    திரும்பு PyBytes_FromString(tv->vval.v_string == வெற்று
					    ? "" : (எழுத்து *)tv->vval.v_string);
	நிலை VAR_எண்:
	    திரும்பு PyLong_FromLong((long) tv->vval.v_எண்);
	நிலை VAR_FLOAT:
	    திரும்பு PyFloat_FromDouble((இரட்டை) tv->vval.v_float);
	நிலை VAR_LIST:
	    திரும்பு NEW_LIST(tv->vval.v_list);
	நிலை VAR_TUPLE:
	    திரும்பு NEW_TUPLE(tv->vval.v_tuple);
	நிலை VAR_DICT:
	    திரும்பு NEW_DICTIONARY(tv->vval.v_dict);
	நிலை VAR_FUNC:
	    திரும்பு NEW_FUNCTION(tv->vval.v_string == வெற்று
					  ? (char_u *)"" : tv->vval.v_string,
					  0, வெற்று, வெற்று, TRUE);
	நிலை VAR_PARTIAL:
	    எனில் (tv->vval.v_partial->pt_argc)
	    {
		argv = PyMem_New(typval_T, (size_t)tv->vval.v_partial->pt_argc);
		க்கு (i = 0; i < tv->vval.v_partial->pt_argc; i++)
		    copy_tv(&tv->vval.v_partial->pt_argv[i], &argv[i]);
	    }
	    இல்லையெனில்
		argv = வெற்று;
	    எனில் (tv->vval.v_partial->pt_dict != வெற்று)
		tv->vval.v_partial->pt_dict->dv_refcount++;
	    திரும்பு NEW_FUNCTION(tv->vval.v_partial == வெற்று
			     ? (char_u *)"" : partial_name(tv->vval.v_partial),
				tv->vval.v_partial->pt_argc, argv,
				tv->vval.v_partial->pt_dict,
				tv->vval.v_partial->pt_auto);
	நிலை VAR_BLOB:
	    திரும்பு PyBytes_FromStringAndSize(
		(எழுத்து*) tv->vval.v_blob->bv_ga.ga_data,
		(Py_ssize_t) tv->vval.v_blob->bv_ga.ga_len);
	நிலை VAR_UNKNOWN:
	நிலை VAR_ANY:
	நிலை VAR_VOID:
	நிலை VAR_CHANNEL:
	நிலை VAR_JOB:
	நிலை VAR_INSTR:
	நிலை VAR_CLASS:
	நிலை VAR_OBJECT:
	நிலை VAR_TYPEALIAS:
	    Py_INCREF(Py_None);
	    திரும்பு Py_None;
	நிலை VAR_BOOL:
	நிலை VAR_SPECIAL:
	    தேர்வு (tv->vval.v_எண்)
	    {
		நிலை VVAL_FALSE: திரும்பு ALWAYS_FALSE;
		நிலை VVAL_TRUE:  திரும்பு ALWAYS_TRUE;
		நிலை VVAL_NONE:
		நிலை VVAL_NULL:  திரும்பு ALWAYS_NONE;
	    }
	    PyErr_SET_VIM(N_("internal error: invalid value type"));
	    திரும்பு வெற்று;
    }
    திரும்பு வெற்று;
}

DEFINE_PY_TYPE_OBJECT(CurrentType);

வகைவரை கட்டமைப்பு
{
    PyObject_HEAD
} CurrentObject;

நிலையான CurrentObject TheCurrent =
{
    PyObject_HEAD_INIT_TYPE(CurrentType)
};

    நிலையான வெறுமை
init_structs(வெறுமை)
{
    CLEAR_FIELD(OutputType);
    OutputType.tp_name = "vim.message";
    OutputType.tp_basicsize = அளவு(OutputObject);
    OutputType.tp_flags = Py_TPFLAGS_DEFAULT;
    OutputType.tp_doc = "vim message object";
    OutputType.tp_methods = OutputMethods;
#எனில் PY_MAJOR_VERSION >= 3
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
#இல்லையெனில்
    OutputType.tp_getattr = OutputGetattr;
    OutputType.tp_setattr = OutputSetattr;
    // Disabled, because this causes a crash in test86
    // OutputType.tp_base = &PyFile_Type;
#endif

    CLEAR_FIELD(IterType);
    IterType.tp_name = "vim.iter";
    IterType.tp_basicsize = அளவு(IterObject);
    IterType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    IterType.tp_doc = "generic iterator object";
    IterType.tp_iter = IterIter;
    IterType.tp_iternext = IterNext;
    IterType.tp_dealloc = IterDestructor;
    IterType.tp_traverse = IterTraverse;
    IterType.tp_clear = IterClear;

    CLEAR_FIELD(BufferType);
    BufferType.tp_name = "vim.buffer";
    BufferType.tp_basicsize = அளவு(BufferObject);
    BufferType.tp_dealloc = BufferDestructor;
    BufferType.tp_repr = BufferRepr;
    BufferType.tp_as_sequence = &BufferAsSeq;
    BufferType.tp_as_mapping = &BufferAsMapping;
    BufferType.tp_flags = Py_TPFLAGS_DEFAULT;
    BufferType.tp_doc = "vim buffer object";
    BufferType.tp_methods = BufferMethods;
#எனில் PY_MAJOR_VERSION >= 3
    BufferType.tp_getattro = BufferGetattro;
    BufferType.tp_setattro = BufferSetattro;
    BufferType.tp_alloc = call_PyType_GenericAlloc;
    BufferType.tp_new = call_PyType_GenericNew;
    BufferType.tp_free = call_PyObject_Free;
#இல்லையெனில்
    BufferType.tp_getattr = BufferGetattr;
    BufferType.tp_setattr = BufferSetattr;
#endif

    CLEAR_FIELD(WindowType);
    WindowType.tp_name = "vim.window";
    WindowType.tp_basicsize = அளவு(WindowObject);
    WindowType.tp_dealloc = WindowDestructor;
    WindowType.tp_repr = WindowRepr;
    WindowType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    WindowType.tp_doc = "vim Window object";
    WindowType.tp_methods = WindowMethods;
    WindowType.tp_traverse = WindowTraverse;
    WindowType.tp_clear = WindowClear;
#எனில் PY_MAJOR_VERSION >= 3
    WindowType.tp_getattro = WindowGetattro;
    WindowType.tp_setattro = WindowSetattro;
    WindowType.tp_alloc = call_PyType_GenericAlloc;
    WindowType.tp_new = call_PyType_GenericNew;
    WindowType.tp_free = call_PyObject_Free;
#இல்லையெனில்
    WindowType.tp_getattr = WindowGetattr;
    WindowType.tp_setattr = WindowSetattr;
#endif

    CLEAR_FIELD(TabPageType);
    TabPageType.tp_name = "vim.tabpage";
    TabPageType.tp_basicsize = அளவு(TabPageObject);
    TabPageType.tp_dealloc = TabPageDestructor;
    TabPageType.tp_repr = TabPageRepr;
    TabPageType.tp_flags = Py_TPFLAGS_DEFAULT;
    TabPageType.tp_doc = "vim tab page object";
    TabPageType.tp_methods = TabPageMethods;
#எனில் PY_MAJOR_VERSION >= 3
    TabPageType.tp_getattro = TabPageGetattro;
    TabPageType.tp_alloc = call_PyType_GenericAlloc;
    TabPageType.tp_new = call_PyType_GenericNew;
    TabPageType.tp_free = call_PyObject_Free;
#இல்லையெனில்
    TabPageType.tp_getattr = TabPageGetattr;
#endif

    CLEAR_FIELD(BufMapType);
    BufMapType.tp_name = "vim.bufferlist";
    BufMapType.tp_basicsize = அளவு(BufMapObject);
    BufMapType.tp_as_mapping = &BufMapAsMapping;
    BufMapType.tp_flags = Py_TPFLAGS_DEFAULT;
    BufMapType.tp_iter = BufMapIter;
    BufMapType.tp_doc = "vim buffer list";

    CLEAR_FIELD(WinListType);
    WinListType.tp_name = "vim.windowlist";
    WinListType.tp_basicsize = அளவு(WinListObject);
    WinListType.tp_as_sequence = &WinListAsSeq;
    WinListType.tp_flags = Py_TPFLAGS_DEFAULT;
    WinListType.tp_doc = "vim window list";
    WinListType.tp_dealloc = (destructor)WinListDestructor;

    CLEAR_FIELD(TabListType);
    TabListType.tp_name = "vim.tabpagelist";
    TabListType.tp_basicsize = அளவு(TabListObject);
    TabListType.tp_as_sequence = &TabListAsSeq;
    TabListType.tp_flags = Py_TPFLAGS_DEFAULT;
    TabListType.tp_doc = "vim tab page list";

    CLEAR_FIELD(RangeType);
    RangeType.tp_name = "vim.range";
    RangeType.tp_basicsize = அளவு(RangeObject);
    RangeType.tp_dealloc = RangeDestructor;
    RangeType.tp_repr = RangeRepr;
    RangeType.tp_as_sequence = &RangeAsSeq;
    RangeType.tp_as_mapping = &RangeAsMapping;
    RangeType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    RangeType.tp_doc = "vim Range object";
    RangeType.tp_methods = RangeMethods;
    RangeType.tp_traverse = RangeTraverse;
    RangeType.tp_clear = RangeClear;
#எனில் PY_MAJOR_VERSION >= 3
    RangeType.tp_getattro = RangeGetattro;
    RangeType.tp_alloc = call_PyType_GenericAlloc;
    RangeType.tp_new = call_PyType_GenericNew;
    RangeType.tp_free = call_PyObject_Free;
#இல்லையெனில்
    RangeType.tp_getattr = RangeGetattr;
#endif

    CLEAR_FIELD(CurrentType);
    CurrentType.tp_name = "vim.currentdata";
    CurrentType.tp_basicsize = அளவு(CurrentObject);
    CurrentType.tp_flags = Py_TPFLAGS_DEFAULT;
    CurrentType.tp_doc = "vim current object";
    CurrentType.tp_methods = CurrentMethods;
#எனில் PY_MAJOR_VERSION >= 3
    CurrentType.tp_getattro = CurrentGetattro;
    CurrentType.tp_setattro = CurrentSetattro;
#இல்லையெனில்
    CurrentType.tp_getattr = CurrentGetattr;
    CurrentType.tp_setattr = CurrentSetattr;
#endif

    CLEAR_FIELD(DictionaryType);
    DictionaryType.tp_name = "vim.dictionary";
    DictionaryType.tp_basicsize = அளவு(DictionaryObject);
    DictionaryType.tp_dealloc = DictionaryDestructor;
    DictionaryType.tp_as_sequence = &DictionaryAsSeq;
    DictionaryType.tp_as_mapping = &DictionaryAsMapping;
    DictionaryType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE;
    DictionaryType.tp_doc = "dictionary pushing modifications to Vim structure";
    DictionaryType.tp_methods = DictionaryMethods;
    DictionaryType.tp_iter = DictionaryIter;
    DictionaryType.tp_new = DictionaryConstructor;
    DictionaryType.tp_alloc = PyType_GenericAlloc;
#எனில் PY_MAJOR_VERSION >= 3
    DictionaryType.tp_getattro = DictionaryGetattro;
    DictionaryType.tp_setattro = DictionarySetattro;
#இல்லையெனில்
    DictionaryType.tp_getattr = DictionaryGetattr;
    DictionaryType.tp_setattr = DictionarySetattr;
#endif

    CLEAR_FIELD(ListType);
    ListType.tp_name = "vim.list";
    ListType.tp_dealloc = ListDestructor;
    ListType.tp_basicsize = அளவு(ListObject);
    ListType.tp_as_sequence = &ListAsSeq;
    ListType.tp_as_mapping = &ListAsMapping;
    ListType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE;
    ListType.tp_doc = "list pushing modifications to Vim structure";
    ListType.tp_methods = ListMethods;
    ListType.tp_iter = ListIter;
    ListType.tp_new = ListConstructor;
    ListType.tp_alloc = PyType_GenericAlloc;
#எனில் PY_MAJOR_VERSION >= 3
    ListType.tp_getattro = ListGetattro;
    ListType.tp_setattro = ListSetattro;
#இல்லையெனில்
    ListType.tp_getattr = ListGetattr;
    ListType.tp_setattr = ListSetattr;
#endif

    // Tuple type
    CLEAR_FIELD(TupleType);
    TupleType.tp_name = "vim.tuple";
    TupleType.tp_dealloc = TupleDestructor;
    TupleType.tp_basicsize = அளவு(TupleObject);
    TupleType.tp_as_sequence = &TupleAsSeq;
    TupleType.tp_as_mapping = &TupleAsMapping;
    TupleType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE;
    TupleType.tp_doc = "tuple pushing modifications to Vim structure";
    TupleType.tp_methods = TupleMethods;
    TupleType.tp_iter = TupleIter;
    TupleType.tp_new = TupleConstructor;
    TupleType.tp_alloc = PyType_GenericAlloc;
#எனில் PY_MAJOR_VERSION >= 3
    TupleType.tp_getattro = TupleGetattro;
    TupleType.tp_setattro = TupleSetattro;
#இல்லையெனில்
    TupleType.tp_getattr = TupleGetattr;
    TupleType.tp_setattr = TupleSetattr;
#endif

    CLEAR_FIELD(FunctionType);
    FunctionType.tp_name = "vim.function";
    FunctionType.tp_basicsize = அளவு(FunctionObject);
    FunctionType.tp_dealloc = FunctionDestructor;
    FunctionType.tp_call = FunctionCall;
    FunctionType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE;
    FunctionType.tp_doc = "object that calls Vim function";
    FunctionType.tp_methods = FunctionMethods;
    FunctionType.tp_repr = FunctionRepr;
    FunctionType.tp_new = FunctionConstructor;
    FunctionType.tp_alloc = PyType_GenericAlloc;
#எனில் PY_MAJOR_VERSION >= 3
    FunctionType.tp_getattro = FunctionGetattro;
#இல்லையெனில்
    FunctionType.tp_getattr = FunctionGetattr;
#endif

    CLEAR_FIELD(OptionsType);
    OptionsType.tp_name = "vim.options";
    OptionsType.tp_basicsize = அளவு(OptionsObject);
    OptionsType.tp_as_sequence = &OptionsAsSeq;
    OptionsType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    OptionsType.tp_doc = "object for manipulating options";
    OptionsType.tp_iter = OptionsIter;
    OptionsType.tp_as_mapping = &OptionsAsMapping;
    OptionsType.tp_dealloc = OptionsDestructor;
    OptionsType.tp_traverse = OptionsTraverse;
    OptionsType.tp_clear = OptionsClear;

#எனில் PY_VERSION_HEX < 0x030700f0
    CLEAR_FIELD(LoaderType);
    LoaderType.tp_name = "vim.Loader";
    LoaderType.tp_basicsize = அளவு(LoaderObject);
    LoaderType.tp_flags = Py_TPFLAGS_DEFAULT;
    LoaderType.tp_doc = "vim message object";
    LoaderType.tp_methods = LoaderMethods;
    LoaderType.tp_dealloc = LoaderDestructor;
#endif

#எனில் PY_MAJOR_VERSION >= 3
    CLEAR_FIELD(vimmodule);
    vimmodule.m_name = "vim";
    vimmodule.m_doc = "Vim Python interface\n";
    vimmodule.m_size = -1;
    vimmodule.m_methods = VimMethods;
#endif
}

    நிலையான முழு
init_types(வெறுமை)
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
#எனில் PY_VERSION_HEX < 0x030700f0
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
    திரும்பு 0;
}

#ifdef USE_LIMITED_API
    நிலையான வெறுமை
shutdown_types(வெறுமை)
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
# எனில் PY_VERSION_HEX < 0x030700f0
    PYTYPE_CLEANUP(LoaderType);
# endif
}
#endif

    நிலையான முழு
init_sys_path(வெறுமை)
{
    PyObject	*path;
    PyObject	*path_hook;
    PyObject	*path_hooks;

    எனில் (!(path_hook = PyObject_GetAttrString(vim_module, "path_hook")))
	திரும்பு -1;

    எனில் (!(path_hooks = PySys_GetObject("path_hooks")))
    {
	PyErr_Clear();
	path_hooks = PyList_New(1);
	PyList_SET_ITEM(path_hooks, 0, path_hook);
	எனில் (PySys_SetObject("path_hooks", path_hooks))
	{
	    Py_DECREF(path_hooks);
	    திரும்பு -1;
	}
	Py_DECREF(path_hooks);
    }
    இல்லையெனில் எனில் (PyList_Check(path_hooks))
    {
	எனில் (PyList_Append(path_hooks, path_hook))
	{
	    Py_DECREF(path_hook);
	    திரும்பு -1;
	}
	Py_DECREF(path_hook);
    }
    இல்லையெனில்
    {
	VimTryStart();
	emsg(_("Failed to set path hook: sys.path_hooks is not a list\n"
	       "You should now do the following:\n"
	       "- append vim.path_hook to sys.path_hooks\n"
	       "- append vim.VIM_SPECIAL_PATH to sys.path\n"));
	VimTryEnd(); // Discard the error
	Py_DECREF(path_hook);
	திரும்பு 0;
    }

    எனில் (!(path = PySys_GetObject("path")))
    {
	PyErr_Clear();
	path = PyList_New(1);
	Py_INCREF(vim_special_path_object);
	PyList_SET_ITEM(path, 0, vim_special_path_object);
	எனில் (PySys_SetObject("path", path))
	{
	    Py_DECREF(path);
	    திரும்பு -1;
	}
	Py_DECREF(path);
    }
    இல்லையெனில் எனில் (PyList_Check(path))
    {
	எனில் (PyList_Append(path, vim_special_path_object))
	    திரும்பு -1;
    }
    இல்லையெனில்
    {
	VimTryStart();
	emsg(_("Failed to set path: sys.path is not a list\n"
	       "You should now append vim.VIM_SPECIAL_PATH to sys.path"));
	VimTryEnd(); // Discard the error
    }

    திரும்பு 0;
}

நிலையான கட்டமைப்பு numeric_constant {
    எழுத்து	*name;
    முழு		val;
} numeric_constants[] = {
    {"VAR_LOCKED",	VAR_LOCKED},
    {"VAR_FIXED",	VAR_FIXED},
    {"VAR_SCOPE",	VAR_SCOPE},
    {"VAR_DEF_SCOPE",	VAR_DEF_SCOPE},
};

கட்டமைப்பு object_constant {
    எழுத்து	*name;
    PyObject	*valObject;
};

#define ADD_OBJECT(m, name, obj) \
    எனில் (PyModule_AddObject(m, name, obj)) \
	திரும்பு -1;

#define ADD_CHECKED_OBJECT(m, name, obj) \
    { \
	PyObject	*valObject = obj; \
	எனில் (!valObject) \
	    திரும்பு -1; \
	ADD_OBJECT(m, name, valObject); \
    }

    நிலையான முழு
populate_module(PyObject *m)
{
    முழு		i;
    PyObject	*other_module;
    PyObject	*attr;
    PyObject	*imp;
#எனில் PY_VERSION_HEX >= 0x030700f0
    PyObject	*dict;
    PyObject	*cls;
#endif

    க்கு (i = 0; i < (முழு)(அளவு(numeric_constants)
					   / அளவு(கட்டமைப்பு numeric_constant));
	    ++i)
	ADD_CHECKED_OBJECT(m, numeric_constants[i].name,
		PyInt_FromLong(numeric_constants[i].val));

    கட்டமைப்பு object_constant object_constants[] = {
	{"buffers",  (PyObject *)(வெறுமை *)&TheBufferMap},
	{"windows",  (PyObject *)(வெறுமை *)&TheWindowList},
	{"tabpages", (PyObject *)(வெறுமை *)&TheTabPageList},
	{"current",  (PyObject *)(வெறுமை *)&TheCurrent},

	{"Buffer",     (PyObject *)BufferTypePtr},
	{"Range",      (PyObject *)RangeTypePtr},
	{"Window",     (PyObject *)WindowTypePtr},
	{"TabPage",    (PyObject *)TabPageTypePtr},
	{"Dictionary", (PyObject *)DictionaryTypePtr},
	{"List",       (PyObject *)ListTypePtr},
	{"Tuple",      (PyObject *)TupleTypePtr},
	{"Function",   (PyObject *)FunctionTypePtr},
	{"Options",    (PyObject *)OptionsTypePtr},
#எனில் PY_VERSION_HEX < 0x030700f0
	{"_Loader",    (PyObject *)LoaderTypePtr},
#endif
    };

    க்கு (i = 0; i < (முழு)(அளவு(object_constants)
					    / அளவு(கட்டமைப்பு object_constant));
	    ++i)
    {
	PyObject	*valObject;

	valObject = object_constants[i].valObject;
	Py_INCREF(valObject);
	ADD_OBJECT(m, object_constants[i].name, valObject);
    }

    எனில் (!(VimError = PyErr_NewException("vim.error", வெற்று, வெற்று)))
	திரும்பு -1;
    ADD_OBJECT(m, "error", VimError);

    ADD_CHECKED_OBJECT(m, "vars",  NEW_DICTIONARY(get_globvar_dict()));
    ADD_CHECKED_OBJECT(m, "vvars", NEW_DICTIONARY(get_vimvar_dict()));
    ADD_CHECKED_OBJECT(m, "options",
	    OptionsNew(SREQ_GLOBAL, வெற்று, dummy_check, வெற்று));

    எனில் (!(other_module = PyImport_ImportModule("os")))
	திரும்பு -1;
    ADD_OBJECT(m, "os", other_module);

#எனில் PY_MAJOR_VERSION >= 3
    எனில் (!(py_getcwd = PyObject_GetAttrString(other_module, "getcwd")))
	திரும்பு -1;
#இல்லையெனில்
    எனில் (!(py_getcwd = PyObject_GetAttrString(other_module, "getcwdu")))
	திரும்பு -1;
#endif
    ADD_OBJECT(m, "_getcwd", py_getcwd)

    எனில் (!(py_chdir = PyObject_GetAttrString(other_module, "chdir")))
	திரும்பு -1;
    ADD_OBJECT(m, "_chdir", py_chdir);
    எனில் (!(attr = PyObject_GetAttrString(m, "chdir")))
	திரும்பு -1;
    எனில் (PyObject_SetAttrString(other_module, "chdir", attr))
    {
	Py_DECREF(attr);
	திரும்பு -1;
    }
    Py_DECREF(attr);

    எனில் ((py_fchdir = PyObject_GetAttrString(other_module, "fchdir")))
    {
	ADD_OBJECT(m, "_fchdir", py_fchdir);
	எனில் (!(attr = PyObject_GetAttrString(m, "fchdir")))
	    திரும்பு -1;
	எனில் (PyObject_SetAttrString(other_module, "fchdir", attr))
	{
	    Py_DECREF(attr);
	    திரும்பு -1;
	}
	Py_DECREF(attr);
    }
    இல்லையெனில்
	PyErr_Clear();

    எனில் (!(vim_special_path_object = PyString_FromString(vim_special_path)))
	திரும்பு -1;

    ADD_OBJECT(m, "VIM_SPECIAL_PATH", vim_special_path_object);

#எனில் PY_VERSION_HEX >= 0x030700f0
    எனில் (!(imp = PyImport_ImportModule("importlib.machinery")))
	திரும்பு -1;

    dict = PyModule_GetDict(imp);

    எனில் (!(cls = PyDict_GetItemString(dict, "PathFinder")))
    {
	Py_DECREF(imp);
	திரும்பு -1;
    }

    எனில் (!(py_find_spec = PyObject_GetAttrString(cls, "find_spec")))
    {
	Py_DECREF(imp);
	திரும்பு -1;
    }

# எனில் PY_VERSION_HEX < 0x30c00a7
    // find_module has been removed as of Python 3.12.0a7
    எனில் ((py_find_module = PyObject_GetAttrString(cls, "find_module")))
	ADD_OBJECT(m, "_find_module", py_find_module);
# endif

    Py_DECREF(imp);

    ADD_OBJECT(m, "_find_spec", py_find_spec);
#இல்லையெனில்
    எனில் (!(imp = PyImport_ImportModule("imp")))
	திரும்பு -1;

    எனில் (!(py_find_module = PyObject_GetAttrString(imp, "find_module")))
    {
	Py_DECREF(imp);
	திரும்பு -1;
    }

    எனில் (!(py_load_module = PyObject_GetAttrString(imp, "load_module")))
    {
	Py_DECREF(py_find_module);
	Py_DECREF(imp);
	திரும்பு -1;
    }

    Py_DECREF(imp);

    ADD_OBJECT(m, "_find_module", py_find_module);
    ADD_OBJECT(m, "_load_module", py_load_module);
#endif

    திரும்பு 0;
}
