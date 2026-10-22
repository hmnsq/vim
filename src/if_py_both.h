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

#જો PY_VERSION_HEX < 0x02050000
પ્રકારવ્યાખ્યા પૂર્ણાંક Py_ssize_t;  // Python 2.4 and earlier don't have this type.
#endif

// Use values that are known to work, others may make Vim crash.
#define ENC_OPT (enc_utf8 ? "utf-8" : enc_dbcs ? "euc-jp" : (અક્ષર *)p_enc)
#define DOPY_FUNC "_vim_pydo"

સ્થિર અચલ અક્ષર *vim_special_path = "_vim_path_";

#define PyErr_SET_STRING(exc, str) PyErr_SetString(exc, _(str))
#define PyErr_SetVim(str) PyErr_SetString(VimError, str)
#define PyErr_SET_VIM(str) PyErr_SET_STRING(VimError, str)
#define PyErr_FORMAT(exc, str, arg) PyErr_Format(exc, _(str), arg)
#define PyErr_FORMAT2(exc, str, arg1, arg2) PyErr_Format(exc, _(str), arg1,arg2)
#define PyErr_VIM_FORMAT(str, arg) PyErr_FORMAT(VimError, str, arg)

#ifdef USE_LIMITED_API
// Limited Python API. Need to call only exposed functions and remap macros.
// PyTypeObject is an opaque struct.

પ્રકારવ્યાખ્યા સંરચના {
    lenfunc sq_length;
    binaryfunc sq_concat;
    ssizeargfunc sq_repeat;
    ssizeargfunc sq_item;
    શૂન્ય *was_sq_slice;
    ssizeobjargproc sq_ass_item;
    શૂન્ય *was_sq_ass_slice;
    objobjproc sq_contains;

    binaryfunc sq_inplace_concat;
    ssizeargfunc sq_inplace_repeat;
} PySequenceMethods;

પ્રકારવ્યાખ્યા સંરચના {
    lenfunc mp_length;
    binaryfunc mp_subscript;
    objobjargproc mp_ass_subscript;
} PyMappingMethods;

// This struct emulates the concrete _typeobject struct to allow the code to
// work the same way in both limited and full Python APIs.
સંરચના typeobject_wrapper {
    અચલ અક્ષર *tp_name;
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

    અચલ અક્ષર *tp_doc;

    traverseproc tp_traverse;

    inquiry tp_clear;

    getiterfunc tp_iter;
    iternextfunc tp_iternext;

    સંરચના PyMethodDef *tp_methods;
    સંરચના _typeobject *tp_base;
    allocfunc tp_alloc;
    newfunc tp_new;
    freefunc tp_free;
};

# define DEFINE_PY_TYPE_OBJECT(type) \
    સ્થિર સંરચના typeobject_wrapper type; \
    સ્થિર PyTypeObject* type##Ptr = ખાલી

// PyObject_HEAD_INIT_TYPE and PyObject_FINISH_INIT_TYPE need to come in pairs
// We first initialize with NULL because the type is not allocated until
// init_types() is called later. It's in FINISH_INIT_TYPE where we fill the
// type in with the newly allocated type.
# define PyObject_HEAD_INIT_TYPE(type) PyObject_HEAD_INIT(ખાલી)
# define PyObject_FINISH_INIT_TYPE(obj, type) obj.ob_base.ob_type = type##Ptr

# define Py_TYPE_GET_TP_ALLOC(type) ((allocfunc)PyType_GetSlot(type, Py_tp_alloc))
# define Py_TYPE_GET_TP_METHODS(type) ((PyMethodDef *)PyType_GetSlot(type, Py_tp_methods))

// PyObject_NEW is not part of stable ABI, but PyObject_Malloc/Init are.
સ્થિર PyObject* Vim_PyObject_New(PyTypeObject *type, size_t objsize)
{
    PyObject *obj = (PyObject *)PyObject_Malloc(objsize);
    જો (obj == ખાલી)
	પાછા PyErr_NoMemory();
    પાછા PyObject_Init(obj, type);
}
# undef PyObject_NEW
# define PyObject_NEW(type, typeobj) ((type *)Vim_PyObject_New(typeobj, કદ(type)))

// This is a somewhat convoluted because limited API doesn't expose an easy way
// to get the tp_name field, and so we have to manually reconstruct it as
// "__module__.__name__" (with __module__ omitted for builtins to emulate
// Python behavior). Also, some of the more convenient functions like
// PyUnicode_AsUTF8AndSize and PyType_GetQualName() are not available until
// late Python 3 versions, and won't be available if you set Py_LIMITED_API too
// low.
# define PyErr_FORMAT_TYPE(સંદેશ, obj) \
    કરો { \
	PyObject* qualname = PyObject_GetAttrString((PyObject*)(obj)->ob_type, "__qualname__"); \
	જો (qualname == ખાલી) \
	{ \
	    PyErr_FORMAT(PyExc_TypeError, સંદેશ, "(NULL)"); \
	    તોડો; \
	} \
	PyObject* module = PyObject_GetAttrString((PyObject*)(obj)->ob_type, "__module__"); \
	PyObject* full; \
	જો (module == ખાલી || PyUnicode_CompareWithASCIIString(module, "builtins") == 0 \
		|| PyUnicode_CompareWithASCIIString(module, "__main__") == 0) \
	{ \
	    full = qualname; \
	    Py_INCREF(full); \
	} \
	નહીંતર \
	    full = PyUnicode_FromFormat("%U.%U", module, qualname); \
	PyObject* full_bytes = PyUnicode_AsUTF8String(full); \
	અચલ અક્ષર* full_str = PyBytes_AsString(full_bytes); \
	full_str = full_str == ખાલી ? "(NULL)" : full_str; \
	PyErr_FORMAT(PyExc_TypeError, સંદેશ, full_str); \
	Py_DECREF(qualname); \
	Py_XDECREF(module); \
	Py_XDECREF(full); \
	Py_XDECREF(full_bytes); \
    } જ્યારે (0)

# define PyList_GET_ITEM(list, i) PyList_GetItem(list, i)
# define PyList_GET_SIZE(o) PyList_Size(o)

// PyList_SET_ITEM and PyList_SetItem have slightly different behaviors. The
// former will leave the old item dangling, and the latter will decref on it.
// Since we only use this on new lists, this difference doesn't matter.
# define PyList_SET_ITEM(list, i, item) PyList_SetItem(list, i, item)

# define PyTuple_GET_ITEM(o, pos) PyTuple_GetItem(o, pos)
# define PyTuple_GET_SIZE(o) PyTuple_Size(o)

# જો Py_LIMITED_API < 0x03080000
// PyIter_check only became part of stable ABI in 3.8, and there is no easy way
// to check for it in the API. We simply return false as a compromise. This
// does mean we should avoid compiling with stable ABI < 3.8.
#  undef PyIter_Check
#  define PyIter_Check(obj) (FALSE)
# endif

સ્થિર PyTypeObject* AddHeapType(સંરચના typeobject_wrapper* type_object)
{
    PyType_Spec type_spec;
    type_spec.name = type_object->tp_name;
    type_spec.basicsize = type_object->tp_basicsize;
    type_spec.itemsize = 0;
    type_spec.flags = type_object->tp_flags;

    // We just need to statically allocate a large enough buffer that can hold
    // all slots. We need to leave a null-terminated slot at the end.
    PyType_Slot slots[40] = { {0, ખાલી} };
    size_t slot_i = 0;

# define ADD_TP_SLOT(slot_name) \
    જો (slot_i >= 40) પાછા ખાલી; /* this should never happen */ \
    જો (type_object->slot_name != ખાલી) \
    { \
	slots[slot_i].slot = Py_##slot_name; \
	slots[slot_i].pfunc = (શૂન્ય*)type_object->slot_name; \
	++slot_i; \
    }
# define ADD_TP_SUB_SLOT(sub_slot, slot_name) \
    જો (slot_i >= 40) પાછા ખાલી; /* this should never happen */ \
    જો (type_object->sub_slot != ખાલી && type_object->sub_slot->slot_name != ખાલી) \
    { \
	slots[slot_i].slot = Py_##slot_name; \
	slots[slot_i].pfunc = (શૂન્ય*)type_object->sub_slot->slot_name; \
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
    પાછા (PyTypeObject*)newtype;
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
    જો (type##Ptr == ખાલી) \
	પાછા -1;
# define PYTYPE_CLEANUP(type) \
    type##Ptr = ખાલી;

// Limited API does not provide PyRun_* functions. Need to implement manually
// using PyCompile and PyEval.
સ્થિર PyObject* Vim_PyRun_String(અચલ અક્ષર *str, પૂર્ણાંક start, PyObject *globals, PyObject *locals)
{
    // Just pass "" for filename for now.
    PyObject* compiled = Py_CompileString(str, "", start);
    જો (compiled == ખાલી)
	પાછા ખાલી;

    PyObject* eval_result = PyEval_EvalCode(compiled, globals, locals);
    Py_DECREF(compiled);
    પાછા eval_result;
}
સ્થિર પૂર્ણાંક Vim_PyRun_SimpleString(અચલ અક્ષર *str)
{
    // This function emulates CPython's implementation.
    PyObject* m = PyImport_AddModule("__main__");
    જો (m == ખાલી)
	પાછા -1;
    PyObject* d = PyModule_GetDict(m);
    PyObject* output = Vim_PyRun_String(str, Py_file_આગત, d, d);
    જો (output == ખાલી)
    {
	PyErr_PrintEx(TRUE);
	પાછા -1;
    }
    Py_DECREF(output);
    પાછા 0;
}
# define PyRun_String Vim_PyRun_String
# define PyRun_SimpleString Vim_PyRun_SimpleString

#નહીંતર // !defined(USE_LIMITED_API)

// Full Python API. Can make use of structs and macros directly.
# define DEFINE_PY_TYPE_OBJECT(type) \
    સ્થિર PyTypeObject type; \
    સ્થિર PyTypeObject* type##Ptr = &type
# define PyObject_HEAD_INIT_TYPE(type) PyObject_HEAD_INIT(&type)

# define Py_TYPE_GET_TP_ALLOC(type) type->tp_alloc
# define Py_TYPE_GET_TP_METHODS(type) type->tp_methods

# define Py_TYPE_NAME(obj) ((obj)->ob_type->tp_name == ખાલી \
	? "(NULL)" \
	: (obj)->ob_type->tp_name)
# define PyErr_FORMAT_TYPE(સંદેશ, obj) \
    PyErr_FORMAT(PyExc_TypeError, સંદેશ, \
	    Py_TYPE_NAME(obj))

// Add a static type
# define PYTYPE_READY(type) \
    જો (PyType_Ready(type##Ptr)) \
	પાછા -1;

#endif


#define RAISE_NO_EMPTY_ચાવીઓ PyErr_SET_STRING(Py_Exc_કિંમત_Error, \
					    N_("empty keys are not allowed"))
#define RAISE_LOCKED_DICTIONARY PyErr_SET_VIM(N_("dictionary is locked"))
#define RAISE_LOCKED_LIST PyErr_SET_VIM(N_("list is locked"))
#define RAISE_UNDO_FAIL PyErr_SET_VIM(N_("cannot save undo information"))
#define RAISE_DELETE_લીટી_FAIL PyErr_SET_VIM(N_("cannot delete line"))
#define RAISE_ઉમેરો_લીટી_FAIL PyErr_SET_VIM(N_("cannot insert line"))
#define RAISE_REPLACE_લીટી_FAIL PyErr_SET_VIM(N_("cannot replace line"))
#define RAISE_ચાવી_ADD_FAIL(ચાવી) \
    PyErr_VIM_FORMAT(N_("failed to add key '%s' to dictionary"), ચાવી)
#define RAISE_INVALID_INDEX_TYPE(idx) \
    PyErr_FORMAT_TYPE(N_("index must be int or slice, not %s"), idx);

#define INVALID_BUFFER_કિંમત ((buf_T *)(-1))
#define INVALID_WINDOW_કિંમત ((win_T *)(-1))
#define INVALID_TABPAGE_કિંમત ((tabpage_T *)(-1))

પ્રકારવ્યાખ્યા શૂન્ય (*rangeinitializer)(શૂન્ય *);
પ્રકારવ્યાખ્યા શૂન્ય (*runner)(અચલ અક્ષર *, dict_T *, શૂન્ય *
#ifdef PY_CAN_RECURSE
	, PyGILState_STATE *
#endif
	);

સ્થિર પૂર્ણાંક ConvertFromPyObject(PyObject *, typval_T *);
સ્થિર પૂર્ણાંક _ConvertFromPyObject(PyObject *, typval_T *, PyObject *);
સ્થિર પૂર્ણાંક ConvertFromPyMapping(PyObject *, typval_T *);
સ્થિર પૂર્ણાંક ConvertFromPySequence(PyObject *, typval_T *);
સ્થિર PyObject *WindowNew(win_T *, tabpage_T *);
સ્થિર PyObject *BufferNew (buf_T *);
સ્થિર PyObject *લીટી_To_String(અચલ અક્ષર *);

સ્થિર PyInt RangeStart;
સ્થિર PyInt RangeEnd;

સ્થિર PyObject *globals;

સ્થિર PyObject *py_chdir;
સ્થિર PyObject *py_fchdir;
સ્થિર PyObject *py_getcwd;
સ્થિર PyObject *vim_module;
સ્થિર PyObject *vim_special_path_object;

#જો PY_VERSION_HEX >= 0x030700f0
સ્થિર PyObject *py_find_spec;
#નહીંતર
સ્થિર PyObject *py_load_module;
#endif
#જો PY_VERSION_HEX < 0x30c00a7
સ્થિર PyObject *py_find_module;
#endif

સ્થિર PyObject *VimError;

/*
 * obtain a lock on the Vim data structures
 */
    સ્થિર શૂન્ય
Python_Lock_Vim(શૂન્ય)
{
}

/*
 * release a lock on the Vim data structures
 */
    સ્થિર શૂન્ય
Python_Release_Vim(શૂન્ય)
{
}

/*
 * The "todecref" argument holds a pointer to PyObject * that must be
 * DECREF'ed after returned char_u * is no longer needed or NULL if all what
 * was needed to generate returned value is object.
 *
 * Use Py_XDECREF to decrement reference count.
 */
    સ્થિર char_u *
StringToChars(PyObject *obj, PyObject **todecref)
{
    char_u	*str;

    જો (PyBytes_Check(obj))
    {

	જો (PyBytes_AsStringAndSize(obj, (અક્ષર **) &str, ખાલી) == -1
		|| str == ખાલી)
	    પાછા ખાલી;

	*todecref = ખાલી;
    }
    નહીંતર જો (PyUnicode_Check(obj))
    {
	PyObject	*bytes;

	જો (!(bytes = PyUnicode_AsEncodedString(obj, ENC_OPT,
							   ERRORS_ENCODE_ARG)))
	    પાછા ખાલી;

	જો (PyBytes_AsStringAndSize(bytes, (અક્ષર **) &str, ખાલી) == -1
		|| str == ખાલી)
	{
	    Py_DECREF(bytes);
	    પાછા ખાલી;
	}

	*todecref = bytes;
    }
    નહીંતર
    {
#જો PY_MAJOR_VERSION < 3
	PyErr_FORMAT_TYPE(N_("expected str() or unicode() instance, but got %s"),
		obj);
#નહીંતર
	PyErr_FORMAT_TYPE(N_("expected bytes() or str() instance, but got %s"),
		obj);
#endif
	પાછા ખાલી;
    }

    પાછા (char_u *) str;
}

#define સંખ્યા_LONG     1
#define સંખ્યા_INT      2
#define સંખ્યા_NATURAL  4
#define સંખ્યા_UNSIGNED 8

    સ્થિર પૂર્ણાંક
સંખ્યા_To_Long(PyObject *obj, long *result, પૂર્ણાંક flags)
{
#જો PY_MAJOR_VERSION < 3
    જો (PyInt_Check(obj))
    {
	*result = PyInt_AsLong(obj);
	જો (PyErr_Occurred())
	    પાછા -1;
    }
    નહીંતર
#endif
    જો (PyLong_Check(obj))
    {
	*result = PyLong_AsLong(obj);
	જો (PyErr_Occurred())
	    પાછા -1;
    }
    નહીંતર જો (Py_સંખ્યા_Check(obj))
    {
	PyObject	*અંક;

	જો (!(અંક = Py_સંખ્યા_Long(obj)))
	    પાછા -1;

	*result = PyLong_AsLong(અંક);

	Py_DECREF(અંક);

	જો (PyErr_Occurred())
	    પાછા -1;
    }
    નહીંતર
    {
#જો PY_MAJOR_VERSION < 3
	PyErr_FORMAT_TYPE(
		N_("expected int(), long() or something supporting "
		   "coercing to long(), but got %s"),
		obj);
#નહીંતર
	PyErr_FORMAT_TYPE(
		N_("expected int() or something supporting coercing to int(), "
		   "but got %s"),
		obj);
#endif
	પાછા -1;
    }

    જો (flags & સંખ્યા_INT)
    {
	જો (*result > INT_MAX)
	{
	    PyErr_SET_STRING(PyExc_OverflowError,
		    N_("value is too large to fit into C int type"));
	    પાછા -1;
	}
	નહીંતર જો (*result < INT_MIN)
	{
	    PyErr_SET_STRING(PyExc_OverflowError,
		    N_("value is too small to fit into C int type"));
	    પાછા -1;
	}
    }

    જો (flags & સંખ્યા_NATURAL)
    {
	જો (*result <= 0)
	{
	    PyErr_SET_STRING(Py_Exc_કિંમત_Error,
		    N_("number must be greater than zero"));
	    પાછા -1;
	}
    }
    નહીંતર જો (flags & સંખ્યા_UNSIGNED)
    {
	જો (*result < 0)
	{
	    PyErr_SET_STRING(Py_Exc_કિંમત_Error,
		    N_("number must be greater or equal to zero"));
	    પાછા -1;
	}
    }

    પાછા 0;
}

    સ્થિર પૂર્ણાંક
add_string(PyObject *list, અક્ષર *s)
{
    PyObject	*string;

    જો (!(string = PyString_FromString(s)))
	પાછા -1;

    જો (PyList_Append(list, string))
    {
	Py_DECREF(string);
	પાછા -1;
    }

    Py_DECREF(string);
    પાછા 0;
}

    સ્થિર PyObject *
ObjectDir(PyObject *self, અક્ષર **attributes)
{
    PyMethodDef	*method;
    અક્ષર	**attr;
    PyObject	*ret;

    જો (!(ret = PyList_New(0)))
	પાછા ખાલી;

    જો (self)
	માટે (method = Py_TYPE_GET_TP_METHODS(self->ob_type) ; method->ml_name != ખાલી ; ++method)
	    જો (add_string(ret, (અક્ષર *)method->ml_name))
	    {
		Py_DECREF(ret);
		પાછા ખાલી;
	    }

    માટે (attr = attributes ; *attr ; ++attr)
	જો (add_string(ret, *attr))
	{
	    Py_DECREF(ret);
	    પાછા ખાલી;
	}

#જો PY_MAJOR_VERSION < 3
    જો (add_string(ret, "__members__"))
    {
	Py_DECREF(ret);
	પાછા ખાલી;
    }
#endif

    પાછા ret;
}

// Output buffer management

// Function to write a line, points to either msg() or emsg().
પ્રકારવ્યાખ્યા પૂર્ણાંક (*writefn)(અક્ષર *);

DEFINE_PY_TYPE_OBJECT(OutputType);

પ્રકારવ્યાખ્યા સંરચના
{
    PyObject_HEAD
    long softspace;
    long error;
} OutputObject;

સ્થિર અક્ષર *OutputAttrs[] = {
    "softspace",
    ખાલી
};

    સ્થિર PyObject *
OutputDir(PyObject *self, PyObject *args UNUSED)
{
    પાછા ObjectDir(self, OutputAttrs);
}

    સ્થિર પૂર્ણાંક
OutputSetattr(PyObject *self_obj, અક્ષર *name, PyObject *valObject)
{
    OutputObject *self = (OutputObject*)self_obj;
    જો (valObject == ખાલી)
    {
	PyErr_SET_STRING(PyExc_AttributeError,
		N_("can't delete OutputObject attributes"));
	પાછા -1;
    }

    જો (strcmp(name, "softspace") == 0)
    {
	જો (સંખ્યા_To_Long(valObject, &(self->softspace), સંખ્યા_UNSIGNED))
	    પાછા -1;
	પાછા 0;
    }

    PyErr_FORMAT(PyExc_AttributeError, N_("invalid attribute: %s"), name);
    પાછા -1;
}

// Buffer IO, we write one whole line at a time.
સ્થિર garray_T io_ga = {0, 0, 1, 80, ખાલી};
સ્થિર writefn old_fn = ખાલી;

    સ્થિર શૂન્ય
PythonIO_Flush(શૂન્ય)
{
    જો (old_fn != ખાલી && io_ga.ga_len > 0)
    {
	((અક્ષર *)io_ga.ga_data)[io_ga.ga_len] = NUL;
	// We don't know what emsg_severe should be here, so ... hope?
	old_fn((અક્ષર *)io_ga.ga_data);
    }
    io_ga.ga_len = 0;
}

    સ્થિર શૂન્ય
writer(writefn fn, char_u *str, PyInt n, પૂર્ણાંક severe)
{
    char_u *ptr;

    // Flush when switching output function.
    જો (fn != old_fn)
	PythonIO_Flush();
    old_fn = fn;

    // Write each NL separated line.  Text after the last NL is kept for
    // writing later.
    // For normal messages: Do not output when "got_int" was set.  This avoids
    // a loop gone crazy flooding the terminal with messages.  Also for when
    // "q" is pressed at the more-prompt.
    જ્યારે (n > 0 && (ptr = memchr(str, '\n', n)) != ખાલી
					  && (fn == (writefn)emsg || !got_int))
    {
	PyInt len = ptr - str;

	જો (ga_grow(&io_ga, (પૂર્ણાંક)(len + 1)) == FAIL)
	    તોડો;

	mch_memmove(((અક્ષર *)io_ga.ga_data) + io_ga.ga_len, str, (size_t)len);
	((અક્ષર *)io_ga.ga_data)[io_ga.ga_len + len] = NUL;
	emsg_severe = severe;
	fn((અક્ષર *)io_ga.ga_data);
	str = ptr + 1;
	n -= len + 1;
	io_ga.ga_len = 0;
    }

    // Put the remaining text into io_ga for later printing.
    જો (n > 0 && (fn == (writefn)emsg || !got_int)
					&& ga_grow(&io_ga, (પૂર્ણાંક)(n + 1)) == OK)
    {
	mch_memmove(((અક્ષર *)io_ga.ga_data) + io_ga.ga_len, str, (size_t)n);
	io_ga.ga_len += (પૂર્ણાંક)n;
    }
}

    સ્થિર પૂર્ણાંક
write_output(OutputObject *self, PyObject *string)
{
    Py_ssize_t	len = 0;
    અક્ષર	*str = ખાલી;
    પૂર્ણાંક		error = self->error;

    જો (!PyArg_Parse(string, "et#", ENC_OPT, &str, &len))
	પાછા -1;

    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    writer((writefn)(error ? emsg : સંદેશ), (char_u *)str, len, error);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS
    PyMem_Free(str);

    પાછા 0;
}

    સ્થિર PyObject *
OutputWrite(OutputObject *self, PyObject *string)
{
    જો (write_output(self, string))
	પાછા ખાલી;

    Py_INCREF(Py_None);
    પાછા Py_None;
}

    સ્થિર PyObject *
OutputWritelines(OutputObject *self, PyObject *seq)
{
    PyObject	*iterator;
    PyObject	*item;

    જો (!(iterator = PyObject_GetIter(seq)))
	પાછા ખાલી;

    જ્યારે ((item = PyIter_Next(iterator)))
    {
	જો (write_output(self, item))
	{
	    Py_DECREF(iterator);
	    Py_DECREF(item);
	    પાછા ખાલી;
	}
	Py_DECREF(item);
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    જો (PyErr_Occurred())
	પાછા ખાલી;

    Py_INCREF(Py_None);
    પાછા Py_None;
}

    સ્થિર PyObject *
AlwaysNone(PyObject *self UNUSED, PyObject *args UNUSED)
{
    // do nothing
    Py_INCREF(Py_None);
    પાછા Py_None;
}
#define ALWAYS_NONE AlwaysNone(ખાલી, ખાલી)

    સ્થિર PyObject *
AlwaysFalse(PyObject *self UNUSED, PyObject *args UNUSED)
{
    // do nothing
    PyObject	*ret = Py_False;
    Py_INCREF(ret);
    પાછા ret;
}
#define ALWAYS_FALSE AlwaysFalse(ખાલી, ખાલી)

    સ્થિર PyObject *
AlwaysTrue(PyObject *self UNUSED, PyObject *args UNUSED)
{
    // do nothing
    PyObject	*ret = Py_True;
    Py_INCREF(ret);
    પાછા ret;
}
#define ALWAYS_TRUE AlwaysTrue(ખાલી, ખાલી)

/***************/

સ્થિર સંરચના PyMethodDef OutputMethods[] = {
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
    { ખાલી,	    ખાલી,				0,		ખાલી}
};

સ્થિર OutputObject Output =
{
    PyObject_HEAD_INIT_TYPE(OutputType)
    0,
    0
};

સ્થિર OutputObject Error =
{
    PyObject_HEAD_INIT_TYPE(OutputType)
    0,
    1
};

    સ્થિર પૂર્ણાંક
PythonIO_Init_io(શૂન્ય)
{
    જો (PySys_SetObject("stdout", (PyObject *)(શૂન્ય *)&Output))
	પાછા -1;
    જો (PySys_SetObject("stderr", (PyObject *)(શૂન્ય *)&Error))
	પાછા -1;

    જો (PyErr_Occurred())
    {
	emsg(_(e_python_error_initialising_io_object));
	પાછા -1;
    }

    પાછા 0;
}

#જો PY_VERSION_HEX < 0x030700f0
સ્થિર PyObject *call_load_module(અક્ષર *name, પૂર્ણાંક len, PyObject *find_module_result);

પ્રકારવ્યાખ્યા સંરચના
{
    PyObject_HEAD
    અક્ષર	*fullname;
    PyObject	*result;
} LoaderObject;
DEFINE_PY_TYPE_OBJECT(LoaderType);

    સ્થિર શૂન્ય
LoaderDestructor(PyObject *self_obj)
{
    LoaderObject *self = (LoaderObject*)self_obj;
    vim_free(self->fullname);
    Py_XDECREF(self->result);
    DESTRUCTOR_FINISH(self);
}

    સ્થિર PyObject *
LoaderLoadModule(LoaderObject *self, PyObject *args UNUSED)
{
    અક્ષર	*fullname = self->fullname;
    PyObject	*result = self->result;
    PyObject	*module;

    જો (!fullname)
    {
	module = result ? result : Py_None;
	Py_INCREF(module);
	પાછા module;
    }

    module = call_load_module(fullname, (પૂર્ણાંક)STRLEN(fullname), result);

    self->fullname = ખાલી;
    self->result = module;

    vim_free(fullname);
    Py_DECREF(result);

    જો (!module)
    {
	જો (PyErr_Occurred())
	    પાછા ખાલી;

	Py_INCREF(Py_None);
	પાછા Py_None;
    }

    Py_INCREF(module);
    પાછા module;
}

સ્થિર સંરચના PyMethodDef LoaderMethods[] = {
    // name,	    function,				calling,	doc
    {"load_module", (PyCFunction)LoaderLoadModule,	METH_VARARGS,	""},
    { ખાલી,	    ખાલી,				0,		ખાલી}
};
#endif

/*
 * Check to see whether a Vim error has been reported, or a keyboard
 * interrupt has been detected.
 */
    સ્થિર શૂન્ય
VimTryStart(શૂન્ય)
{
    ++trylevel;
}

    સ્થિર પૂર્ણાંક
VimTryEnd(શૂન્ય)
{
    --trylevel;
    // Without this it stops processing all subsequent Vim script commands and
    // generates strange error messages if I e.g. try calling Test() in a cycle
    did_emsg = FALSE;
    // Keyboard interrupt should be preferred over anything else
    જો (got_int)
    {
	જો (did_throw)
	    discard_current_exception();
	got_int = FALSE;
	PyErr_SetNone(PyExc_KeyboardInterrupt);
	પાછા -1;
    }
    નહીંતર જો (સંદેશ_list != ખાલી && *સંદેશ_list != ખાલી)
    {
	પૂર્ણાંક	should_free;
	અક્ષર	*સંદેશ;

	સંદેશ = get_exception_string(*સંદેશ_list, ET_ERROR, ખાલી, &should_free);

	જો (સંદેશ == ખાલી)
	{
	    PyErr_NoMemory();
	    પાછા -1;
	}

	PyErr_SetVim(સંદેશ);

	free_global_msglist();

	જો (should_free)
	    vim_free(સંદેશ);

	પાછા -1;
    }
    નહીંતર જો (!did_throw)
	પાછા (PyErr_Occurred() ? -1 : 0);
    // Python exception is preferred over Vim one; unlikely to occur though
    નહીંતર જો (PyErr_Occurred())
    {
	discard_current_exception();
	પાછા -1;
    }
    // Finally transform Vim script exception to python one
    નહીંતર
    {
	PyErr_SetVim((અક્ષર *)current_exception->કિંમત);
	discard_current_exception();
	પાછા -1;
    }
}

    સ્થિર પૂર્ણાંક
VimCheckInterrupt(શૂન્ય)
{
    જો (got_int)
    {
	PyErr_SetNone(PyExc_KeyboardInterrupt);
	પાછા 1;
    }
    પાછા 0;
}

// Vim module - Implementation

    સ્થિર PyObject *
VimCommand(PyObject *self UNUSED, PyObject *string)
{
    char_u	*cmd;
    PyObject	*ret;
    PyObject	*todecref;

    જો (!(cmd = StringToChars(string, &todecref)))
	પાછા ખાલી;

    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();

    VimTryStart();
    do_cmdline_cmd(cmd);
    update_screen(UPD_VALID);

    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    જો (VimTryEnd())
	ret = ખાલી;
    નહીંતર
	ret = Py_None;

    Py_XINCREF(ret);
    Py_XDECREF(todecref);
    પાછા ret;
}

/*
 * Function to translate a typval_T into a PyObject; this will recursively
 * translate lists/dictionaries into their Python equivalents.
 *
 * The depth parameter is to avoid infinite recursion, set it to 1 when
 * you call VimToPython.
 */
    સ્થિર PyObject *
VimToPython(typval_T *our_tv, પૂર્ણાંક depth, PyObject *lookup_dict)
{
    PyObject	*ret;
    PyObject	*newObj;
    અક્ષર	ptrBuf[કદ(શૂન્ય *) * 2 + 3];

    // Avoid infinite recursion
    જો (depth > 100)
    {
	Py_INCREF(Py_None);
	ret = Py_None;
	પાછા ret;
    }

    // Check if we run into a recursive loop.  The item must be in lookup_dict
    // then and we can use it again.
    જો ((our_tv->v_type == VAR_LIST && our_tv->vval.v_list != ખાલી)
	    || (our_tv->v_type == VAR_TUPLE && our_tv->vval.v_tuple != ખાલી)
	    || (our_tv->v_type == VAR_DICT && our_tv->vval.v_dict != ખાલી))
    {
	sprintf(ptrBuf, "%p",
		our_tv->v_type == VAR_LIST ? (શૂન્ય *)our_tv->vval.v_list
		    : our_tv->v_type == VAR_TUPLE ?
			(શૂન્ય *)our_tv->vval.v_tuple
			: (શૂન્ય *)our_tv->vval.v_dict);

	જો ((ret = PyDict_GetItemString(lookup_dict, ptrBuf)))
	{
	    Py_INCREF(ret);
	    પાછા ret;
	}
    }

    જો (our_tv->v_type == VAR_STRING)
	ret = PyString_FromString(our_tv->vval.v_string == ખાલી
					? "" : (અક્ષર *)our_tv->vval.v_string);
    નહીંતર જો (our_tv->v_type == VAR_સંખ્યા)
    {
	અક્ષર buf[NUMBUFLEN];

	// For backwards compatibility numbers are stored as strings.
	sprintf(buf, "%lld", (long long)our_tv->vval.v_સંખ્યા);
	ret = PyString_FromString((અક્ષર *)buf);
    }
    નહીંતર જો (our_tv->v_type == VAR_FLOAT)
    {
	અક્ષર buf[NUMBUFLEN];

	sprintf(buf, "%f", our_tv->vval.v_float);
	ret = PyString_FromString((અક્ષર *)buf);
    }
    નહીંતર જો (our_tv->v_type == VAR_LIST)
    {
	list_T		*list = our_tv->vval.v_list;
	listitem_T	*curr;

	જો (list == ખાલી)
	    પાછા ખાલી;

	જો (!(ret = PyList_New(0)))
	    પાછા ખાલી;

	જો (PyDict_SetItemString(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    પાછા ખાલી;
	}

	CHECK_LIST_MATERIALIZE(list);
	FOR_ALL_LIST_વસ્તુઓ(list, curr)
	{
	    જો (!(newObj = VimToPython(&curr->li_tv, depth + 1, lookup_dict)))
	    {
		Py_DECREF(ret);
		પાછા ખાલી;
	    }
	    જો (PyList_Append(ret, newObj))
	    {
		Py_DECREF(newObj);
		Py_DECREF(ret);
		પાછા ખાલી;
	    }
	    Py_DECREF(newObj);
	}
    }
    નહીંતર જો (our_tv->v_type == VAR_TUPLE)
    {
	tuple_T	*tuple = our_tv->vval.v_tuple;
	પૂર્ણાંક	len;

	જો (tuple == ખાલી)
	    પાછા ખાલી;

	len = TUPLE_LEN(tuple);

	ret = PyTuple_New(len);
	જો (ret == ખાલી)
	    પાછા ખાલી;

	માટે (પૂર્ણાંક idx = 0; idx < len; idx++)
	{
	    typval_T	*item_tv = TUPLE_ITEM(tuple, idx);

	    newObj = VimToPython(item_tv, depth + 1, lookup_dict);
	    જો (!newObj)
	    {
		Py_DECREF(ret);
		પાછા ખાલી;
	    }
	    PyTuple_SetItem(ret, idx, newObj);
	}

	જો (PyDict_SetItemString(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    પાછા ખાલી;
	}
    }
    નહીંતર જો (our_tv->v_type == VAR_DICT)
    {

	hashtab_T	*ht;
	long_u		todo;
	hashitem_T	*hi;
	dictitem_T	*di;

	જો (our_tv->vval.v_dict == ખાલી)
	    પાછા ખાલી;
	ht = &our_tv->vval.v_dict->dv_hashtab;

	જો (!(ret = PyDict_New()))
	    પાછા ખાલી;

	જો (PyDict_SetItemString(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    પાછા ખાલી;
	}

	todo = ht->ht_used;
	માટે (hi = ht->ht_array; todo > 0; ++hi)
	{
	    જો (!HASHITEM_EMPTY(hi))
	    {
		--todo;

		di = dict_lookup(hi);
		જો (!(newObj = VimToPython(&di->di_tv, depth + 1, lookup_dict)))
		{
		    Py_DECREF(ret);
		    પાછા ખાલી;
		}
		જો (PyDict_SetItemString(ret, (અક્ષર *)hi->hi_ચાવી, newObj))
		{
		    Py_DECREF(ret);
		    Py_DECREF(newObj);
		    પાછા ખાલી;
		}
	    }
	}
    }
    નહીંતર જો (our_tv->v_type == VAR_BOOL)
    {
	જો (our_tv->vval.v_સંખ્યા == VVAL_FALSE)
	{
	    ret = Py_False;
	    Py_INCREF(ret);
	}
	નહીંતર
	{
	    ret = Py_True;
	    Py_INCREF(ret);
	}
	પાછા ret;
    }
    નહીંતર જો (our_tv->v_type == VAR_SPECIAL)
    {
	Py_INCREF(Py_None);
	ret = Py_None;
	પાછા ret;
    }
    નહીંતર જો (our_tv->v_type == VAR_BLOB)
	ret = PyBytes_FromStringAndSize(
		(અક્ષર*) our_tv->vval.v_blob->bv_ga.ga_data,
		(Py_ssize_t) our_tv->vval.v_blob->bv_ga.ga_len);
    નહીંતર
    {
	Py_INCREF(Py_None);
	ret = Py_None;
    }

    પાછા ret;
}

    સ્થિર PyObject *
VimEval(PyObject *self UNUSED, PyObject *args)
{
    char_u	*પદાવલિ;
    typval_T	*our_tv;
    PyObject	*string;
    PyObject	*todecref;
    PyObject	*ret;
    PyObject	*lookup_dict;

    જો (!PyArg_ParseTuple(args, "O", &string))
	પાછા ખાલી;

    જો (!(પદાવલિ = StringToChars(string, &todecref)))
	પાછા ખાલી;

    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    VimTryStart();
    our_tv = eval_પદાવલિ(પદાવલિ, ખાલી);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    Py_XDECREF(todecref);

    જો (VimTryEnd())
	પાછા ખાલી;

    જો (our_tv == ખાલી)
    {
	PyErr_SET_VIM(N_("invalid expression"));
	પાછા ખાલી;
    }

    // Convert the Vim type into a Python type.  Create a dictionary that's
    // used to check for recursive loops.
    જો (!(lookup_dict = PyDict_New()))
	ret = ખાલી;
    નહીંતર
    {
	ret = VimToPython(our_tv, 1, lookup_dict);
	Py_DECREF(lookup_dict);
    }


    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    free_tv(our_tv);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    પાછા ret;
}

સ્થિર PyObject *ConvertToPyObject(typval_T *);

    સ્થિર PyObject *
VimEvalPy(PyObject *self UNUSED, PyObject *string)
{
    typval_T	*our_tv;
    PyObject	*ret;
    char_u	*પદાવલિ;
    PyObject	*todecref;

    જો (!(પદાવલિ = StringToChars(string, &todecref)))
	પાછા ખાલી;

    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    VimTryStart();
    our_tv = eval_પદાવલિ(પદાવલિ, ખાલી);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    Py_XDECREF(todecref);

    જો (VimTryEnd())
	પાછા ખાલી;

    જો (our_tv == ખાલી)
    {
	PyErr_SET_VIM(N_("invalid expression"));
	પાછા ખાલી;
    }

    ret = ConvertToPyObject(our_tv);
    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    free_tv(our_tv);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    પાછા ret;
}

    સ્થિર PyObject *
VimStrwidth(PyObject *self UNUSED, PyObject *string)
{
    char_u	*str;
    PyObject	*todecref;
    પૂર્ણાંક		len;

    જો (!(str = StringToChars(string, &todecref)))
	પાછા ખાલી;

    len = mb_string2cells(str, (પૂર્ણાંક)STRLEN(str));

    Py_XDECREF(todecref);

    પાછા PyLong_FromLong(len);
}

    સ્થિર PyObject *
_VimChdir(PyObject *_chdir, PyObject *args, PyObject *kwargs)
{
    PyObject	*ret;
    PyObject	*newwd;
    PyObject	*todecref;
    char_u	*new_dir;

    જો (_chdir == ખાલી)
	પાછા ખાલી;
    જો (!(ret = PyObject_Call(_chdir, args, kwargs)))
	પાછા ખાલી;

    જો (!(newwd = PyObject_CallFunctionObjArgs(py_getcwd, ખાલી)))
    {
	Py_DECREF(ret);
	પાછા ખાલી;
    }

    જો (!(new_dir = StringToChars(newwd, &todecref)))
    {
	Py_DECREF(ret);
	Py_DECREF(newwd);
	પાછા ખાલી;
    }

    VimTryStart();

    જો (vim_chdir(new_dir))
    {
	Py_DECREF(ret);
	Py_DECREF(newwd);
	Py_XDECREF(todecref);

	જો (VimTryEnd())
	    પાછા ખાલી;

	PyErr_SET_VIM(N_("failed to change directory"));
	પાછા ખાલી;
    }

    Py_DECREF(newwd);
    Py_XDECREF(todecref);

    post_chdir(CDSCOPE_GLOBAL);

    જો (VimTryEnd())
    {
	Py_DECREF(ret);
	પાછા ખાલી;
    }

    પાછા ret;
}

    સ્થિર PyObject *
VimChdir(PyObject *self UNUSED, PyObject *args, PyObject *kwargs)
{
    પાછા _VimChdir(py_chdir, args, kwargs);
}

    સ્થિર PyObject *
VimFchdir(PyObject *self UNUSED, PyObject *args, PyObject *kwargs)
{
    પાછા _VimChdir(py_fchdir, args, kwargs);
}

પ્રકારવ્યાખ્યા સંરચના {
    PyObject	*callable;
    PyObject	*result;
} map_rtp_data;

    સ્થિર શૂન્ય
map_rtp_callback(char_u *path, શૂન્ય *_data)
{
    શૂન્ય	**data = (શૂન્ય **) _data;
    PyObject	*pathObject;
    map_rtp_data	*mr_data = *((map_rtp_data **) data);

    જો (!(pathObject = PyString_FromString((અક્ષર *)path)))
    {
	*data = ખાલી;
	પાછા;
    }

    mr_data->result = PyObject_CallFunctionObjArgs(mr_data->callable,
						   pathObject, ખાલી);

    Py_DECREF(pathObject);

    જો (!mr_data->result || mr_data->result != Py_None)
	*data = ખાલી;
    નહીંતર
    {
	Py_DECREF(mr_data->result);
	mr_data->result = ખાલી;
    }
}

    સ્થિર PyObject *
VimForeachRTP(PyObject *self UNUSED, PyObject *callable)
{
    map_rtp_data	data;

    data.callable = callable;
    data.result = ખાલી;

    do_in_runtimepath(ખાલી, 0, &map_rtp_callback, &data);

    જો (data.result == ખાલી)
    {
	જો (PyErr_Occurred())
	    પાછા ખાલી;
	નહીંતર
	{
	    Py_INCREF(Py_None);
	    પાછા Py_None;
	}
    }
    પાછા data.result;
}

/*
 * _vim_runtimepath_ special path implementation.
 */

    સ્થિર શૂન્ય
map_finder_callback(char_u *path, શૂન્ય *_data)
{
    શૂન્ય	**data = (શૂન્ય **) _data;
    PyObject	*list = *((PyObject **) data);
    PyObject	*pathObject1, *pathObject2;
    અક્ષર	*pathbuf;
    size_t	pathlen;

    pathlen = STRLEN(path);

#જો PY_MAJOR_VERSION < 3
# define PY_MAIN_DIR_STRING "python2"
#નહીંતર
# define PY_MAIN_DIR_STRING "python3"
#endif
#define PY_ALTERNATE_DIR_STRING "pythonx"

#define PYTHONX_STRING_LENGTH 7 // STRLEN("pythonx")
    જો (!(pathbuf = PyMem_New(અક્ષર,
		    pathlen + STRLEN(PATHSEPSTR) + PYTHONX_STRING_LENGTH + 1)))
    {
	PyErr_NoMemory();
	*data = ખાલી;
	પાછા;
    }

    mch_memmove(pathbuf, path, pathlen + 1);
    add_pathsep((char_u *) pathbuf);

    pathlen = STRLEN(pathbuf);
    mch_memmove(pathbuf + pathlen, PY_MAIN_DIR_STRING,
	    PYTHONX_STRING_LENGTH + 1);

    જો (!(pathObject1 = PyString_FromString(pathbuf)))
    {
	*data = ખાલી;
	PyMem_Free(pathbuf);
	પાછા;
    }

    mch_memmove(pathbuf + pathlen, PY_ALTERNATE_DIR_STRING,
	    PYTHONX_STRING_LENGTH + 1);

    જો (!(pathObject2 = PyString_FromString(pathbuf)))
    {
	Py_DECREF(pathObject1);
	PyMem_Free(pathbuf);
	*data = ખાલી;
	પાછા;
    }

    PyMem_Free(pathbuf);

    જો (PyList_Append(list, pathObject1)
	    || PyList_Append(list, pathObject2))
	*data = ખાલી;

    Py_DECREF(pathObject1);
    Py_DECREF(pathObject2);
}

    સ્થિર PyObject *
Vim_GetPaths(PyObject *self UNUSED, PyObject *args UNUSED)
{
    PyObject	*ret;

    જો (!(ret = PyList_New(0)))
	પાછા ખાલી;

    do_in_runtimepath(ખાલી, 0, &map_finder_callback, ret);

    જો (PyErr_Occurred())
    {
	Py_DECREF(ret);
	પાછા ખાલી;
    }

    પાછા ret;
}

#જો PY_VERSION_HEX >= 0x030700f0
    સ્થિર PyObject *
FinderFindSpec(PyObject *self, PyObject *args)
{
    અક્ષર	*fullname;
    PyObject	*paths;
    PyObject	*target = Py_None;
    PyObject	*spec;

    જો (!PyArg_ParseTuple(args, "s|O", &fullname, &target))
	પાછા ખાલી;

    જો (!(paths = Vim_GetPaths(self, ખાલી)))
	પાછા ખાલી;

    spec = PyObject_CallFunction(py_find_spec, "sOO", fullname, paths, target);

    Py_DECREF(paths);

    જો (!spec)
    {
	જો (PyErr_Occurred())
	    પાછા ખાલી;

	Py_INCREF(Py_None);
	પાછા Py_None;
    }

    પાછા spec;
}

    સ્થિર PyObject *
FinderFindModule(PyObject* self UNUSED, PyObject* args UNUSED)
{
    // Apparently returning None works.
    Py_INCREF(Py_None);
    પાછા Py_None;
}
#નહીંતર
    સ્થિર PyObject *
call_load_module(અક્ષર *name, પૂર્ણાંક len, PyObject *find_module_result)
{
    PyObject	*fd, *pathname, *description;

    જો (!PyTuple_Check(find_module_result))
    {
	PyErr_FORMAT_TYPE(
		N_("expected 3-tuple as imp.find_module() result, but got %s"),
		find_module_result);
	પાછા ખાલી;
    }
    જો (PyTuple_GET_SIZE(find_module_result) != 3)
    {
	PyErr_FORMAT(PyExc_TypeError,
		N_("expected 3-tuple as imp.find_module() result, but got "
		   "tuple of size %d"),
		(પૂર્ણાંક) PyTuple_GET_SIZE(find_module_result));
	પાછા ખાલી;
    }

    જો (!(fd = PyTuple_GET_ITEM(find_module_result, 0))
	    || !(pathname = PyTuple_GET_ITEM(find_module_result, 1))
	    || !(description = PyTuple_GET_ITEM(find_module_result, 2)))
    {
	PyErr_SET_STRING(PyExc_RuntimeError,
		N_("internal error: imp.find_module returned tuple with NULL"));
	પાછા ખાલી;
    }

    પાછા PyObject_CallFunction(py_load_module,
	    "s#OOO", name, len, fd, pathname, description);
}

    સ્થિર PyObject *
find_module(અક્ષર *fullname, અક્ષર *tail, PyObject *new_path)
{
    PyObject	*find_module_result;
    PyObject	*module;
    અક્ષર	*dot;

    જો ((dot = (અક્ષર *)vim_strchr((char_u *) tail, '.')))
    {
	/*
	 * There is a dot in the name: call find_module recursively without the
	 * first component
	 */
	PyObject	*newest_path;
	પૂર્ણાંક		partlen = (પૂર્ણાંક) (dot - 1 - tail);

	જો (!(find_module_result = PyObject_CallFunction(py_find_module,
			"s#O", tail, partlen, new_path)))
	{
	    જો (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_ImportError))
		PyErr_Clear();
	    પાછા ખાલી;
	}

	જો (!(module = call_load_module(
			fullname,
			((પૂર્ણાંક) (tail - fullname)) + partlen,
			find_module_result)))
	{
	    Py_DECREF(find_module_result);
	    પાછા ખાલી;
	}

	Py_DECREF(find_module_result);

	જો (!(newest_path = PyObject_GetAttrString(module, "__path__")))
	{
	    Py_DECREF(module);
	    પાછા ખાલી;
	}

	Py_DECREF(module);

	find_module_result = find_module(fullname, dot + 1, newest_path);

	Py_DECREF(newest_path);

	પાછા find_module_result;
    }
    નહીંતર
    {
	જો (!(find_module_result = PyObject_CallFunction(py_find_module,
			"sO", tail, new_path)))
	{
	    જો (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_ImportError))
		PyErr_Clear();
	    પાછા ખાલી;
	}

	પાછા find_module_result;
    }
}

    સ્થિર PyObject *
FinderFindModule(PyObject *self, PyObject *args)
{
    અક્ષર	*fullname;
    PyObject	*result;
    PyObject	*new_path;
    LoaderObject	*loader;

    જો (!PyArg_ParseTuple(args, "s", &fullname))
	પાછા ખાલી;

    જો (!(new_path = Vim_GetPaths(self, ખાલી)))
	પાછા ખાલી;

    result = find_module(fullname, fullname, new_path);

    Py_DECREF(new_path);

    જો (!result)
    {
	જો (PyErr_Occurred())
	    પાછા ખાલી;

	Py_INCREF(Py_None);
	પાછા Py_None;
    }

    જો (!(fullname = (અક્ષર *)vim_strsave((char_u *)fullname)))
    {
	Py_DECREF(result);
	PyErr_NoMemory();
	પાછા ખાલી;
    }

    જો (!(loader = PyObject_NEW(LoaderObject, LoaderTypePtr)))
    {
	vim_free(fullname);
	Py_DECREF(result);
	પાછા ખાલી;
    }

    loader->fullname = fullname;
    loader->result = result;

    પાછા (PyObject *) loader;
}
#endif

    સ્થિર PyObject *
VimPathHook(PyObject *self UNUSED, PyObject *args)
{
    અક્ષર	*path;

    જો (PyArg_ParseTuple(args, "s", &path)
	    && STRCMP(path, vim_special_path) == 0)
    {
	Py_INCREF(vim_module);
	પાછા vim_module;
    }

    PyErr_Clear();
    PyErr_SetNone(PyExc_ImportError);
    પાછા ખાલી;
}

/*
 * Vim module - Definitions
 */

સ્થિર સંરચના PyMethodDef VimMethods[] = {
    // name,	    function,			calling,			documentation
    {"command",	    VimCommand,			METH_O,				"Execute a Vim ex-mode command" },
    {"eval",	    VimEval,			METH_VARARGS,			"Evaluate an expression using Vim evaluator" },
    {"bindeval",    VimEvalPy,			METH_O,				"Like eval(), but returns objects attached to Vim ones"},
    {"strwidth",    VimStrwidth,		METH_O,				"Screen string width, counts <Tab> as having width 1"},
    {"chdir",	    (PyCFunction)(શૂન્ય *)VimChdir,	METH_VARARGS|METH_KEYWORDS,	"Change directory"},
    {"fchdir",	    (PyCFunction)(શૂન્ય *)VimFchdir,	METH_VARARGS|METH_KEYWORDS,	"Change directory"},
    {"foreach_rtp", VimForeachRTP,		METH_O,				"Call given callable for each path in &rtp"},
#જો PY_VERSION_HEX >= 0x030700f0
    {"find_spec",   FinderFindSpec,		METH_VARARGS,			"Internal use only, returns spec object for any input it receives"},
#endif
    {"find_module", FinderFindModule,		METH_VARARGS,			"Internal use only, returns loader object for any input it receives"},
    {"path_hook",   VimPathHook,		METH_VARARGS,			"Hook function to install in sys.path_hooks"},
    {"_get_paths",  (PyCFunction)Vim_GetPaths,	METH_NOARGS,			"Get &rtp-based additions to sys.path"},
    { ખાલી,	    ખાલી,			0,				ખાલી}
};

/*
 * Generic iterator object
 */

DEFINE_PY_TYPE_OBJECT(IterType);

પ્રકારવ્યાખ્યા PyObject *(*nextfun)(શૂન્ય **);
પ્રકારવ્યાખ્યા શૂન્ય (*destructorfun)(શૂન્ય *);
પ્રકારવ્યાખ્યા પૂર્ણાંક (*traversefun)(શૂન્ય *, visitproc, શૂન્ય *);
પ્રકારવ્યાખ્યા પૂર્ણાંક (*clearfun)(શૂન્ય **);

// Main purpose of this object is removing the need for do python
// initialization (i.e. PyType_Ready and setting type attributes) for a big
// bunch of objects.
પ્રકારવ્યાખ્યા સંરચના
{
    PyObject_HEAD
    શૂન્ય *cur;
    nextfun next;
    destructorfun destruct;
    traversefun traverse;
    clearfun clear;
    PyObject *iter_object;
} IterObject;

    સ્થિર PyObject *
IterNew(શૂન્ય *start, destructorfun destruct, nextfun next, traversefun traverse,
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

    જો (iter_object)
	Py_INCREF(iter_object);

    પાછા (PyObject *)(self);
}

    સ્થિર શૂન્ય
IterDestructor(PyObject *self_obj)
{
    IterObject *self = (IterObject*)self_obj;
    જો (self->iter_object)
	Py_DECREF(self->iter_object);
    PyObject_GC_UnTrack((શૂન્ય *)(self));
    self->destruct(self->cur);
    PyObject_GC_Del((શૂન્ય *)(self));
}

    સ્થિર પૂર્ણાંક
IterTraverse(PyObject *self_obj, visitproc visit, શૂન્ય *arg)
{
    IterObject *self = (IterObject*)self_obj;
    જો (self->traverse != ખાલી)
	પાછા self->traverse(self->cur, visit, arg);
    નહીંતર
	પાછા 0;
}

// Mac OSX defines clear() somewhere.
#ifdef clear
# undef clear
#endif

    સ્થિર પૂર્ણાંક
IterClear(PyObject *self_obj)
{
    IterObject *self = (IterObject*)self_obj;
    જો (self->clear != ખાલી)
	પાછા self->clear(&self->cur);
    નહીંતર
	પાછા 0;
}

    સ્થિર PyObject *
IterNext(PyObject *self_obj)
{
    IterObject *self = (IterObject*)self_obj;
    પાછા self->next(&self->cur);
}

    સ્થિર PyObject *
IterIter(PyObject *self)
{
    Py_INCREF(self);
    પાછા self;
}

પ્રકારવ્યાખ્યા સંરચના pylinkedlist_S {
    સંરચના pylinkedlist_S	*pll_next;
    સંરચના pylinkedlist_S	*pll_prev;
    PyObject			*pll_obj;
} pylinkedlist_T;

સ્થિર pylinkedlist_T *lastdict = ખાલી;
સ્થિર pylinkedlist_T *lastlist = ખાલી;
સ્થિર pylinkedlist_T *lasttuple = ખાલી;
સ્થિર pylinkedlist_T *lastfunc = ખાલી;

    સ્થિર શૂન્ય
pyll_remove(pylinkedlist_T *ref, pylinkedlist_T **last)
{
    જો (ref->pll_prev == ખાલી)
    {
	જો (ref->pll_next == ખાલી)
	{
	    *last = ખાલી;
	    પાછા;
	}
    }
    નહીંતર
	ref->pll_prev->pll_next = ref->pll_next;

    જો (ref->pll_next == ખાલી)
	*last = ref->pll_prev;
    નહીંતર
	ref->pll_next->pll_prev = ref->pll_prev;
}

    સ્થિર શૂન્ય
pyll_add(PyObject *self, pylinkedlist_T *ref, pylinkedlist_T **last)
{
    જો (*last == ખાલી)
	ref->pll_prev = ખાલી;
    નહીંતર
    {
	(*last)->pll_next = ref;
	ref->pll_prev = *last;
    }
    ref->pll_next = ખાલી;
    ref->pll_obj = self;
    *last = ref;
}

DEFINE_PY_TYPE_OBJECT(DictionaryType);

પ્રકારવ્યાખ્યા સંરચના
{
    PyObject_HEAD
    dict_T	*dict;
    pylinkedlist_T	ref;
} DictionaryObject;

સ્થિર PyObject *DictionaryUpdate(DictionaryObject *, PyObject *, PyObject *);

#define NEW_DICTIONARY(dict) DictionaryNew(DictionaryTypePtr, dict)

    સ્થિર PyObject *
DictionaryNew(PyTypeObject *subtype, dict_T *dict)
{
    DictionaryObject	*self;

    self = (DictionaryObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    જો (self == ખાલી)
	પાછા ખાલી;
    self->dict = dict;
    ++dict->dv_refcount;

    pyll_add((PyObject *)(self), &self->ref, &lastdict);

    પાછા (PyObject *)(self);
}

    સ્થિર dict_T *
py_dict_alloc(શૂન્ય)
{
    dict_T	*ret;

    જો (!(ret = dict_alloc()))
    {
	PyErr_NoMemory();
	પાછા ખાલી;
    }
    ++ret->dv_refcount;

    પાછા ret;
}

    સ્થિર PyObject *
DictionaryConstructor(PyTypeObject *subtype, PyObject *args, PyObject *kwargs)
{
    DictionaryObject	*self;
    dict_T	*dict;

    જો (!(dict = py_dict_alloc()))
	પાછા ખાલી;

    self = (DictionaryObject *) DictionaryNew(subtype, dict);

    --dict->dv_refcount;

    જો (kwargs || PyTuple_Size(args))
    {
	PyObject	*tmp;
	જો (!(tmp = DictionaryUpdate(self, args, kwargs)))
	{
	    Py_DECREF(self);
	    પાછા ખાલી;
	}

	Py_DECREF(tmp);
    }

    પાછા (PyObject *)(self);
}

    સ્થિર શૂન્ય
DictionaryDestructor(PyObject *self_obj)
{
    DictionaryObject *self = (DictionaryObject*)self_obj;
    pyll_remove(&self->ref, &lastdict);
    dict_unref(self->dict);

    DESTRUCTOR_FINISH(self);
}

સ્થિર અક્ષર *DictionaryAttrs[] = {
    "locked", "scope",
    ખાલી
};

    સ્થિર PyObject *
DictionaryDir(PyObject *self, PyObject *args UNUSED)
{
    પાછા ObjectDir(self, DictionaryAttrs);
}

    સ્થિર પૂર્ણાંક
DictionarySetattr(PyObject *self_obj, અક્ષર *name, PyObject *valObject)
{
    DictionaryObject *self = (DictionaryObject*)self_obj;
    જો (valObject == ખાલી)
    {
	PyErr_SET_STRING(PyExc_AttributeError,
		N_("cannot delete vim.Dictionary attributes"));
	પાછા -1;
    }

    જો (strcmp(name, "locked") == 0)
    {
	જો (self->dict->dv_lock == VAR_FIXED)
	{
	    PyErr_SET_STRING(PyExc_TypeError,
		    N_("cannot modify fixed dictionary"));
	    પાછા -1;
	}
	નહીંતર
	{
	    પૂર્ણાંક		istrue = PyObject_IsTrue(valObject);
	    જો (istrue == -1)
		પાછા -1;
	    નહીંતર જો (istrue)
		self->dict->dv_lock = VAR_LOCKED;
	    નહીંતર
		self->dict->dv_lock = 0;
	}
	પાછા 0;
    }
    નહીંતર
    {
	PyErr_FORMAT(PyExc_AttributeError, N_("cannot set attribute %s"), name);
	પાછા -1;
    }
}

    સ્થિર PyInt
DictionaryLength(DictionaryObject *self)
{
    પાછા ((PyInt) (self->dict->dv_hashtab.ht_used));
}

#define DICT_FLAG_HAS_DEFAULT	0x01
#define DICT_FLAG_કાઢો		0x02
#define DICT_FLAG_NONE_DEFAULT	0x04
#define DICT_FLAG_RETURN_BOOL	0x08 // Incompatible with DICT_FLAG_POP
#define DICT_FLAG_RETURN_PAIR	0x10

    સ્થિર PyObject *
_DictionaryItem(DictionaryObject *self, PyObject *args, પૂર્ણાંક flags)
{
    PyObject	*ચાવી_Object;
    PyObject	*defObject = ((flags & DICT_FLAG_NONE_DEFAULT)? Py_None : ખાલી);
    PyObject	*ret;
    char_u	*ચાવી;
    dictitem_T	*di;
    dict_T	*dict = self->dict;
    hashitem_T	*hi;
    PyObject	*todecref;

    જો (flags & DICT_FLAG_HAS_DEFAULT)
    {
	જો (!PyArg_ParseTuple(args, "O|O", &ચાવી_Object, &defObject))
	    પાછા ખાલી;
    }
    નહીંતર
	ચાવી_Object = args;

    જો (flags & DICT_FLAG_RETURN_BOOL)
	defObject = Py_False;

    જો (!(ચાવી = StringToChars(ચાવી_Object, &todecref)))
	પાછા ખાલી;

    જો (*ચાવી == NUL)
    {
	RAISE_NO_EMPTY_ચાવીઓ;
	Py_XDECREF(todecref);
	પાછા ખાલી;
    }

    hi = hash_find(&dict->dv_hashtab, ચાવી);

    Py_XDECREF(todecref);

    જો (HASHITEM_EMPTY(hi))
    {
	જો (defObject)
	{
	    Py_INCREF(defObject);
	    પાછા defObject;
	}
	નહીંતર
	{
	    PyErr_SetObject(Py_Exc_ચાવી_Error, ચાવી_Object);
	    પાછા ખાલી;
	}
    }
    નહીંતર જો (flags & DICT_FLAG_RETURN_BOOL)
    {
	ret = Py_True;
	Py_INCREF(ret);
	પાછા ret;
    }

    di = dict_lookup(hi);

    જો (!(ret = ConvertToPyObject(&di->di_tv)))
	પાછા ખાલી;

    જો (flags & DICT_FLAG_કાઢો)
    {
	જો (dict->dv_lock)
	{
	    RAISE_LOCKED_DICTIONARY;
	    Py_DECREF(ret);
	    પાછા ખાલી;
	}

	hash_remove(&dict->dv_hashtab, hi, "Python remove variable");
	dictitem_free(di);
    }

    પાછા ret;
}

    સ્થિર PyObject *
DictionaryItem(DictionaryObject *self, PyObject *ચાવી_Object)
{
    પાછા _DictionaryItem(self, ચાવી_Object, 0);
}

    સ્થિર પૂર્ણાંક
DictionaryContains(DictionaryObject *self, PyObject *ચાવી_Object)
{
    PyObject	*rObj = _DictionaryItem(self, ચાવી_Object, DICT_FLAG_RETURN_BOOL);
    પૂર્ણાંક		ret;

    જો (rObj == ખાલી)
	પાછા -1;

    ret = (rObj == Py_True);

    Py_DECREF(rObj);

    પાછા ret;
}

પ્રકારવ્યાખ્યા સંરચના
{
    પૂર્ણાંક		dii_changed;
    hashtab_T	*dii_ht;
    hashitem_T	*dii_hi;
    long_u	dii_todo;
} dictiterinfo_T;

    સ્થિર PyObject *
DictionaryIterNext(શૂન્ય **arg)
{
    PyObject	*ret;
    dictiterinfo_T **dii = (dictiterinfo_T**)arg;

    જો (!(*dii)->dii_todo)
	પાછા ખાલી;

    જો ((*dii)->dii_ht->ht_changed != (*dii)->dii_changed)
    {
	PyErr_SET_STRING(PyExc_RuntimeError,
		N_("hashtab changed during iteration"));
	પાછા ખાલી;
    }

    જ્યારે (((*dii)->dii_todo) && HASHITEM_EMPTY((*dii)->dii_hi))
	++((*dii)->dii_hi);

    --((*dii)->dii_todo);

    જો (!(ret = PyBytes_FromString((અક્ષર *)(*dii)->dii_hi->hi_ચાવી)))
	પાછા ખાલી;

    પાછા ret;
}

    સ્થિર PyObject *
DictionaryIter(PyObject *self_obj)
{
    DictionaryObject	*self = (DictionaryObject*)self_obj;
    dictiterinfo_T	*dii;
    hashtab_T		*ht;

    જો (!(dii = PyMem_New(dictiterinfo_T, 1)))
    {
	PyErr_NoMemory();
	પાછા ખાલી;
    }

    ht = &self->dict->dv_hashtab;
    dii->dii_changed = ht->ht_changed;
    dii->dii_ht = ht;
    dii->dii_hi = ht->ht_array;
    dii->dii_todo = ht->ht_used;

    પાછા IterNew(dii,
	    PyMem_Free, DictionaryIterNext,
	    ખાલી, ખાલી, (PyObject *)self);
}

    સ્થિર પૂર્ણાંક
DictionaryAssItem(
	DictionaryObject *self, PyObject *ચાવી_Object, PyObject *valObject)
{
    char_u	*ચાવી;
    typval_T	tv;
    dict_T	*dict = self->dict;
    dictitem_T	*di;
    PyObject	*todecref;

    જો (dict->dv_lock)
    {
	RAISE_LOCKED_DICTIONARY;
	પાછા -1;
    }

    જો (!(ચાવી = StringToChars(ચાવી_Object, &todecref)))
	પાછા -1;

    જો (*ચાવી == NUL)
    {
	RAISE_NO_EMPTY_ચાવીઓ;
	Py_XDECREF(todecref);
	પાછા -1;
    }

    di = dict_find(dict, ચાવી, -1);

    જો (valObject == ખાલી)
    {
	hashitem_T	*hi;

	જો (di == ખાલી)
	{
	    Py_XDECREF(todecref);
	    PyErr_SetObject(Py_Exc_ચાવી_Error, ચાવી_Object);
	    પાછા -1;
	}
	hi = hash_find(&dict->dv_hashtab, di->di_ચાવી);
	hash_remove(&dict->dv_hashtab, hi, "Python remove item");
	dictitem_free(di);
	Py_XDECREF(todecref);
	પાછા 0;
    }

    જો (ConvertFromPyObject(valObject, &tv) == -1)
    {
	Py_XDECREF(todecref);
	પાછા -1;
    }

    જો (di == ખાલી)
    {
	જો (!(di = dictitem_alloc(ચાવી)))
	{
	    Py_XDECREF(todecref);
	    PyErr_NoMemory();
	    પાછા -1;
	}
	di->di_tv.v_type = VAR_UNKNOWN;

	જો (dict_add(dict, di) == FAIL)
	{
	    dictitem_free(di);
	    RAISE_ચાવી_ADD_FAIL(ચાવી);
	    Py_XDECREF(todecref);
	    પાછા -1;
	}
    }
    નહીંતર
	clear_tv(&di->di_tv);

    Py_XDECREF(todecref);

    copy_tv(&tv, &di->di_tv);
    clear_tv(&tv);
    પાછા 0;
}

પ્રકારવ્યાખ્યા PyObject *(*hi_to_py)(hashitem_T *);

    સ્થિર PyObject *
DictionaryListObjects(DictionaryObject *self, hi_to_py hiconvert)
{
    dict_T	*dict = self->dict;
    long_u	todo = dict->dv_hashtab.ht_used;
    Py_ssize_t	i = 0;
    PyObject	*ret;
    hashitem_T	*hi;
    PyObject	*newObj;

    ret = PyList_New(todo);
    માટે (hi = dict->dv_hashtab.ht_array; todo > 0; ++hi)
    {
	જો (!HASHITEM_EMPTY(hi))
	{
	    જો (!(newObj = hiconvert(hi)))
	    {
		Py_DECREF(ret);
		પાછા ખાલી;
	    }
	    PyList_SET_ITEM(ret, i, newObj);
	    --todo;
	    ++i;
	}
    }
    પાછા ret;
}

    સ્થિર PyObject *
dict_ચાવી(hashitem_T *hi)
{
    પાછા PyBytes_FromString((અક્ષર *)(hi->hi_ચાવી));
}

    સ્થિર PyObject *
Dictionary_List_ચાવીઓ(DictionaryObject *self, PyObject *args UNUSED)
{
    પાછા DictionaryListObjects(self, dict_ચાવી);
}

    સ્થિર PyObject *
dict_val(hashitem_T *hi)
{
    dictitem_T	*di;

    di = dict_lookup(hi);
    પાછા ConvertToPyObject(&di->di_tv);
}

    સ્થિર PyObject *
Dictionary_List_કિંમતો(DictionaryObject *self, PyObject *args UNUSED)
{
    પાછા DictionaryListObjects(self, dict_val);
}

    સ્થિર PyObject *
dict_item(hashitem_T *hi)
{
    PyObject	*ચાવી_Object;
    PyObject	*valObject;
    PyObject	*ret;

    જો (!(ચાવી_Object = dict_ચાવી(hi)))
	પાછા ખાલી;

    જો (!(valObject = dict_val(hi)))
    {
	Py_DECREF(ચાવી_Object);
	પાછા ખાલી;
    }

    ret = Py_Build_કિંમત("(OO)", ચાવી_Object, valObject);

    Py_DECREF(ચાવી_Object);
    Py_DECREF(valObject);

    પાછા ret;
}

    સ્થિર PyObject *
Dictionary_List_વસ્તુઓ(DictionaryObject *self, PyObject *args UNUSED)
{
    પાછા DictionaryListObjects(self, dict_item);
}

    સ્થિર PyObject *
DictionaryUpdate(DictionaryObject *self, PyObject *args, PyObject *kwargs)
{
    dict_T	*dict = self->dict;

    જો (dict->dv_lock)
    {
	RAISE_LOCKED_DICTIONARY;
	પાછા ખાલી;
    }

    જો (kwargs)
    {
	typval_T	tv;

	જો (ConvertFromPyMapping(kwargs, &tv) == -1)
	    પાછા ખાલી;

	VimTryStart();
	dict_extend(self->dict, tv.vval.v_dict, (char_u *) "force", ખાલી);
	clear_tv(&tv);
	જો (VimTryEnd())
	    પાછા ખાલી;
    }
    નહીંતર
    {
	PyObject	*obj = ખાલી;

	જો (!PyArg_ParseTuple(args, "|O", &obj))
	    પાછા ખાલી;

	જો (obj == ખાલી)
	{
	    Py_INCREF(Py_None);
	    પાછા Py_None;
	}

	જો (PyObject_HasAttrString(obj, "keys"))
	    પાછા DictionaryUpdate(self, ખાલી, obj);
	નહીંતર
	{
	    PyObject	*iterator;
	    PyObject	*item;

	    જો (!(iterator = PyObject_GetIter(obj)))
		પાછા ખાલી;

	    જ્યારે ((item = PyIter_Next(iterator)))
	    {
		PyObject	*fast;
		PyObject	*ચાવી_Object;
		PyObject	*valObject;
		PyObject	*todecref;
		char_u		*ચાવી;
		dictitem_T	*di;
		hashitem_T	*hi;

		જો (!(fast = PySequence_Fast(item, "")))
		{
		    Py_DECREF(iterator);
		    Py_DECREF(item);
		    પાછા ખાલી;
		}

		Py_DECREF(item);

		જો (PySequence_Size(fast) != 2)
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    PyErr_FORMAT(Py_Exc_કિંમત_Error,
			    N_("expected sequence element of size 2, "
			    "but got sequence of size %d"),
			    (પૂર્ણાંક) PySequence_Size(fast));
		    પાછા ખાલી;
		}

		ચાવી_Object = PySequence_GetItem(fast, 0);

		જો (!(ચાવી = StringToChars(ચાવી_Object, &todecref)))
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    પાછા ખાલી;
		}

		di = dictitem_alloc(ચાવી);

		Py_XDECREF(todecref);

		જો (di == ખાલી)
		{
		    Py_DECREF(fast);
		    Py_DECREF(iterator);
		    PyErr_NoMemory();
		    પાછા ખાલી;
		}
		di->di_tv.v_type = VAR_UNKNOWN;

		valObject = PySequence_GetItem(fast, 1);

		જો (ConvertFromPyObject(valObject, &di->di_tv) == -1)
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    dictitem_free(di);
		    પાછા ખાલી;
		}

		Py_DECREF(fast);

		hi = hash_find(&dict->dv_hashtab, di->di_ચાવી);
		જો (!HASHITEM_EMPTY(hi) || dict_add(dict, di) == FAIL)
		{
		    RAISE_ચાવી_ADD_FAIL(di->di_ચાવી);
		    Py_DECREF(iterator);
		    dictitem_free(di);
		    પાછા ખાલી;
		}
	    }

	    Py_DECREF(iterator);

	    // Iterator may have finished due to an exception
	    જો (PyErr_Occurred())
		પાછા ખાલી;
	}
    }
    Py_INCREF(Py_None);
    પાછા Py_None;
}

    સ્થિર PyObject *
DictionaryGet(DictionaryObject *self, PyObject *args)
{
    પાછા _DictionaryItem(self, args,
			    DICT_FLAG_HAS_DEFAULT|DICT_FLAG_NONE_DEFAULT);
}

    સ્થિર PyObject *
Dictionary_કાઢો(DictionaryObject *self, PyObject *args)
{
    પાછા _DictionaryItem(self, args, DICT_FLAG_HAS_DEFAULT|DICT_FLAG_કાઢો);
}

    સ્થિર PyObject *
Dictionary_કાઢો_Item(DictionaryObject *self, PyObject *args UNUSED)
{
    hashitem_T	*hi;
    PyObject	*ret;
    PyObject	*valObject;
    dictitem_T	*di;

    જો (self->dict->dv_hashtab.ht_used == 0)
    {
	PyErr_SetNone(Py_Exc_ચાવી_Error);
	પાછા ખાલી;
    }

    hi = self->dict->dv_hashtab.ht_array;
    જ્યારે (HASHITEM_EMPTY(hi))
	++hi;

    di = dict_lookup(hi);

    જો (!(valObject = ConvertToPyObject(&di->di_tv)))
	પાછા ખાલી;

    જો (!(ret = Py_Build_કિંમત("(" Py_bytes_fmt "O)", hi->hi_ચાવી, valObject)))
    {
	Py_DECREF(valObject);
	પાછા ખાલી;
    }

    hash_remove(&self->dict->dv_hashtab, hi, "Python pop item");
    dictitem_free(di);

    પાછા ret;
}

    સ્થિર PyObject *
Dictionary_Has_ચાવી(DictionaryObject *self, PyObject *ચાવી_Object)
{
    પાછા _DictionaryItem(self, ચાવી_Object, DICT_FLAG_RETURN_BOOL);
}

સ્થિર PySequenceMethods DictionaryAsSeq = {
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

સ્થિર PyMappingMethods DictionaryAsMapping = {
    (lenfunc)       DictionaryLength,
    (binaryfunc)    DictionaryItem,
    (objobjargproc) DictionaryAssItem,
};

સ્થિર સંરચના PyMethodDef DictionaryMethods[] = {
    {"keys",	(PyCFunction)Dictionary_List_ચાવીઓ,	METH_NOARGS,	""},
    {"values",	(PyCFunction)Dictionary_List_કિંમતો,	METH_NOARGS,	""},
    {"items",	(PyCFunction)Dictionary_List_વસ્તુઓ,	METH_NOARGS,	""},
    {"update",	(PyCFunction)(શૂન્ય *)DictionaryUpdate,		METH_VARARGS|METH_KEYWORDS, ""},
    {"get",	(PyCFunction)DictionaryGet,		METH_VARARGS,	""},
    {"pop",	(PyCFunction)Dictionary_કાઢો,		METH_VARARGS,	""},
    {"popitem",	(PyCFunction)Dictionary_કાઢો_Item,		METH_NOARGS,	""},
    {"has_key",	(PyCFunction)Dictionary_Has_ચાવી,		METH_O,		""},
    {"__dir__",	(PyCFunction)DictionaryDir,		METH_NOARGS,	""},
    { ખાલી,	ખાલી,					0,		ખાલી}
};

DEFINE_PY_TYPE_OBJECT(ListType);

પ્રકારવ્યાખ્યા સંરચના
{
    PyObject_HEAD
    list_T	*list;
    pylinkedlist_T	ref;
} ListObject;

#define NEW_LIST(list) ListNew(ListTypePtr, list)

    સ્થિર PyObject *
ListNew(PyTypeObject *subtype, list_T *list)
{
    ListObject	*self;

    જો (list == ખાલી)
	પાછા ખાલી;

    self = (ListObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    જો (self == ખાલી)
	પાછા ખાલી;
    self->list = list;
    ++list->lv_refcount;
    CHECK_LIST_MATERIALIZE(list);

    pyll_add((PyObject *)(self), &self->ref, &lastlist);

    પાછા (PyObject *)(self);
}

    સ્થિર list_T *
py_list_alloc(શૂન્ય)
{
    list_T	*ret;

    જો (!(ret = list_alloc()))
    {
	PyErr_NoMemory();
	પાછા ખાલી;
    }
    ++ret->lv_refcount;

    પાછા ret;
}

    સ્થિર પૂર્ણાંક
list_py_concat(list_T *l, PyObject *obj, PyObject *lookup_dict)
{
    PyObject	*iterator;
    PyObject	*item;
    listitem_T	*li;

    જો (!(iterator = PyObject_GetIter(obj)))
	પાછા -1;

    જ્યારે ((item = PyIter_Next(iterator)))
    {
	જો (!(li = listitem_alloc()))
	{
	    PyErr_NoMemory();
	    Py_DECREF(item);
	    Py_DECREF(iterator);
	    પાછા -1;
	}
	li->li_tv.v_lock = 0;
	li->li_tv.v_type = VAR_UNKNOWN;

	જો (_ConvertFromPyObject(item, &li->li_tv, lookup_dict) == -1)
	{
	    Py_DECREF(item);
	    Py_DECREF(iterator);
	    listitem_free(l, li);
	    પાછા -1;
	}

	Py_DECREF(item);

	list_append(l, li);
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    જો (PyErr_Occurred())
	પાછા -1;

    પાછા 0;
}

    સ્થિર PyObject *
ListConstructor(PyTypeObject *subtype, PyObject *args, PyObject *kwargs)
{
    list_T	*list;
    PyObject	*obj = ખાલી;

    જો (kwargs)
    {
	PyErr_SET_STRING(PyExc_TypeError,
		N_("list constructor does not accept keyword arguments"));
	પાછા ખાલી;
    }

    જો (!PyArg_ParseTuple(args, "|O", &obj))
	પાછા ખાલી;

    જો (!(list = py_list_alloc()))
	પાછા ખાલી;

    જો (obj)
    {
	PyObject	*lookup_dict;

	જો (!(lookup_dict = PyDict_New()))
	{
	    list_unref(list);
	    પાછા ખાલી;
	}

	જો (list_py_concat(list, obj, lookup_dict) == -1)
	{
	    Py_DECREF(lookup_dict);
	    list_unref(list);
	    પાછા ખાલી;
	}

	Py_DECREF(lookup_dict);
    }

    પાછા ListNew(subtype, list);
}

    સ્થિર શૂન્ય
ListDestructor(PyObject *self_obj)
{
    ListObject *self = (ListObject*)self_obj;
    pyll_remove(&self->ref, &lastlist);
    list_unref(self->list);

    DESTRUCTOR_FINISH(self);
}

    સ્થિર PyInt
ListLength(ListObject *self)
{
    પાછા ((PyInt) (self->list->lv_len));
}

    સ્થિર PyObject *
ListIndex(ListObject *self, Py_ssize_t index)
{
    listitem_T	*li;

    જો (index >= ListLength(self))
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("list index out of range"));
	પાછા ખાલી;
    }
    li = list_find(self->list, (long) index);
    જો (li == ખાલી)
    {
	// No more suitable format specifications in python-2.3
	PyErr_VIM_FORMAT(N_("internal error: failed to get Vim list item %d"),
		(પૂર્ણાંક) index);
	પાછા ખાલી;
    }
    પાછા ConvertToPyObject(&li->li_tv);
}

    સ્થિર PyObject *
ListSlice(ListObject *self, Py_ssize_t first, Py_ssize_t step,
	  Py_ssize_t slicelen)
{
    PyInt	i;
    PyObject	*list;

    જો (step == 0)
    {
	PyErr_SET_STRING(Py_Exc_કિંમત_Error, N_("slice step cannot be zero"));
	પાછા ખાલી;
    }

    list = PyList_New(slicelen);
    જો (list == ખાલી)
	પાછા ખાલી;

    માટે (i = 0; i < slicelen; ++i)
    {
	PyObject	*item;

	item = ListIndex(self, first + i*step);
	જો (item == ખાલી)
	{
	    Py_DECREF(list);
	    પાછા ખાલી;
	}

	PyList_SET_ITEM(list, i, item);
    }

    પાછા list;
}

    સ્થિર PyObject *
ListItem(ListObject *self, PyObject* idx)
{
#જો PY_MAJOR_VERSION < 3
    જો (PyInt_Check(idx))
    {
	long _idx = PyInt_AsLong(idx);
	પાછા ListIndex(self, _idx);
    }
    નહીંતર
#endif
    જો (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	પાછા ListIndex(self, _idx);
    }
    નહીંતર જો (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	જો (PySlice_GetIndicesEx((PySliceObject_T *)idx, ListLength(self),
				 &start, &stop, &step, &slicelen) < 0)
	    પાછા ખાલી;
	પાછા ListSlice(self, start, step, slicelen);
    }
    નહીંતર
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	પાછા ખાલી;
    }
}

    સ્થિર શૂન્ય
list_restore(Py_ssize_t numadded, Py_ssize_t numreplaced, Py_ssize_t slicelen,
	list_T *l, listitem_T **lis, listitem_T *lastaddedli)
{
    જ્યારે (numreplaced--)
    {
	list_ઉમેરો(l, lis[numreplaced], lis[slicelen + numreplaced]);
	listitem_remove(l, lis[slicelen + numreplaced]);
    }
    જ્યારે (numadded--)
    {
	listitem_T	*next;

	next = lastaddedli->li_prev;
	listitem_remove(l, lastaddedli);
	lastaddedli = next;
    }
}

    સ્થિર પૂર્ણાંક
ListAssSlice(ListObject *self, Py_ssize_t first,
	     Py_ssize_t step, Py_ssize_t slicelen, PyObject *obj)
{
    PyObject	*iterator;
    PyObject	*item;
    listitem_T	*li;
    listitem_T	*lastaddedli = ખાલી;
    listitem_T	*next;
    typval_T	v;
    list_T	*l = self->list;
    PyInt	i;
    PyInt	j;
    PyInt	numreplaced = 0;
    PyInt	numadded = 0;
    PyInt	size;
    listitem_T	**lis = ખાલી;

    size = ListLength(self);

    જો (l->lv_lock)
    {
	RAISE_LOCKED_LIST;
	પાછા -1;
    }

    જો (step == 0)
    {
	PyErr_SET_STRING(Py_Exc_કિંમત_Error, N_("slice step cannot be zero"));
	પાછા -1;
    }

    જો (step != 1 && slicelen == 0)
    {
	// Nothing to do. Only error out if obj has some items.
	પૂર્ણાંક		ret = 0;

	જો (obj == ખાલી)
	    પાછા 0;

	જો (!(iterator = PyObject_GetIter(obj)))
	    પાછા -1;

	જો ((item = PyIter_Next(iterator)))
	{
	    PyErr_FORMAT(Py_Exc_કિંમત_Error,
		    N_("attempt to assign sequence of size greater than %d "
			"to extended slice"), 0);
	    Py_DECREF(item);
	    ret = -1;
	}
	Py_DECREF(iterator);
	પાછા ret;
    }

    જો (obj != ખાલી)
	// XXX May allocate zero bytes.
	જો (!(lis = PyMem_New(listitem_T *, slicelen * 2)))
	{
	    PyErr_NoMemory();
	    પાછા -1;
	}

    જો (first == size)
	li = ખાલી;
    નહીંતર
    {
	li = list_find(l, (long) first);
	જો (li == ખાલી)
	{
	    PyErr_VIM_FORMAT(N_("internal error: no Vim list item %d"),
		    (પૂર્ણાંક)first);
	    જો (obj != ખાલી)
		PyMem_Free(lis);
	    પાછા -1;
	}
	i = slicelen;
	જ્યારે (i-- && li != ખાલી)
	{
	    j = step;
	    next = li;
	    જો (step > 0)
		જ્યારે (next != ખાલી && ((next = next->li_next) != ખાલી) && --j);
	    નહીંતર
		જ્યારે (next != ખાલી && ((next = next->li_prev) != ખાલી) && ++j);

	    જો (obj == ખાલી)
		listitem_remove(l, li);
	    નહીંતર
		lis[slicelen - i - 1] = li;

	    li = next;
	}
	જો (li == ખાલી && i != -1)
	{
	    PyErr_SET_VIM(N_("internal error: not enough list items"));
	    જો (obj != ખાલી)
		PyMem_Free(lis);
	    પાછા -1;
	}
    }

    જો (obj == ખાલી)
	પાછા 0;

    જો (!(iterator = PyObject_GetIter(obj)))
    {
	PyMem_Free(lis);
	પાછા -1;
    }

    i = 0;
    જ્યારે ((item = PyIter_Next(iterator)))
    {
	જો (ConvertFromPyObject(item, &v) == -1)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(item);
	    PyMem_Free(lis);
	    પાછા -1;
	}
	Py_DECREF(item);
	જો (list_ઉમેરો_tv(l, &v, numreplaced < slicelen
				    ? lis[numreplaced]
				    : li) == FAIL)
	{
	    clear_tv(&v);
	    PyErr_SET_VIM(N_("internal error: failed to add item to list"));
	    list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	    PyMem_Free(lis);
	    પાછા -1;
	}
	જો (numreplaced < slicelen)
	{
	    lis[slicelen + numreplaced] = lis[numreplaced]->li_prev;
	    vimlist_remove(l, lis[numreplaced], lis[numreplaced]);
	    numreplaced++;
	}
	નહીંતર
	{
	    જો (li)
		lastaddedli = li->li_prev;
	    નહીંતર
		lastaddedli = l->lv_u.mat.lv_last;
	    numadded++;
	}
	clear_tv(&v);
	જો (step != 1 && i >= slicelen)
	{
	    Py_DECREF(iterator);
	    PyErr_FORMAT(Py_Exc_કિંમત_Error,
		    N_("attempt to assign sequence of size greater than %d "
			"to extended slice"), (પૂર્ણાંક) slicelen);
	    list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	    PyMem_Free(lis);
	    પાછા -1;
	}
	++i;
    }
    Py_DECREF(iterator);

    જો (step != 1 && i != slicelen)
    {
	PyErr_FORMAT2(Py_Exc_કિંમત_Error,
		N_("attempt to assign sequence of size %d to extended slice "
		    "of size %d"), (પૂર્ણાંક) i, (પૂર્ણાંક) slicelen);
	list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	PyMem_Free(lis);
	પાછા -1;
    }

    જો (PyErr_Occurred())
    {
	list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	PyMem_Free(lis);
	પાછા -1;
    }

    માટે (i = 0; i < numreplaced; i++)
	listitem_free(l, lis[i]);
    જો (step == 1)
	માટે (i = numreplaced; i < slicelen; i++)
	    listitem_remove(l, lis[i]);

    PyMem_Free(lis);

    પાછા 0;
}

    સ્થિર પૂર્ણાંક
ListAssIndex(ListObject *self, Py_ssize_t index, PyObject *obj)
{
    typval_T	tv;
    list_T	*l = self->list;
    listitem_T	*li;
    Py_ssize_t	length = ListLength(self);

    જો (l->lv_lock)
    {
	RAISE_LOCKED_LIST;
	પાછા -1;
    }
    જો (index > length || (index == length && obj == ખાલી))
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("list index out of range"));
	પાછા -1;
    }

    જો (obj == ખાલી)
    {
	li = list_find(l, (long) index);
	જો (li == ખાલી)
	{
	    PyErr_VIM_FORMAT(N_("internal error: failed to get Vim "
			"list item %d"), (પૂર્ણાંક) index);
	    પાછા -1;
	}
	vimlist_remove(l, li, li);
	clear_tv(&li->li_tv);
	vim_free(li);
	પાછા 0;
    }

    જો (ConvertFromPyObject(obj, &tv) == -1)
	પાછા -1;

    જો (index == length)
    {
	જો (list_append_tv(l, &tv) == FAIL)
	{
	    clear_tv(&tv);
	    PyErr_SET_VIM(N_("failed to add item to list"));
	    પાછા -1;
	}
    }
    નહીંતર
    {
	li = list_find(l, (long) index);
	જો (li == ખાલી)
	{
	    PyErr_VIM_FORMAT(N_("internal error: failed to get Vim "
			"list item %d"), (પૂર્ણાંક) index);
	    પાછા -1;
	}
	clear_tv(&li->li_tv);
	copy_tv(&tv, &li->li_tv);
	clear_tv(&tv);
    }
    પાછા 0;
}

    સ્થિર પૂર્ણાંક
ListAssItem(ListObject *self, PyObject *idx, PyObject *obj)
{
#જો PY_MAJOR_VERSION < 3
    જો (PyInt_Check(idx))
    {
	long _idx = PyInt_AsLong(idx);
	પાછા (પૂર્ણાંક)ListAssIndex(self, _idx, obj);
    }
    નહીંતર
#endif
    જો (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	પાછા (પૂર્ણાંક)ListAssIndex(self, _idx, obj);
    }
    નહીંતર જો (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	જો (PySlice_GetIndicesEx((PySliceObject_T *)idx, ListLength(self),
				 &start, &stop, &step, &slicelen) < 0)
	    પાછા -1;
	પાછા (પૂર્ણાંક)ListAssSlice(self, start, step, slicelen,
		obj);
    }
    નહીંતર
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	પાછા -1;
    }
}

    સ્થિર PyObject *
ListConcatInPlace(ListObject *self, PyObject *obj)
{
    list_T	*l = self->list;
    PyObject	*lookup_dict;

    જો (l->lv_lock)
    {
	RAISE_LOCKED_LIST;
	પાછા ખાલી;
    }

    જો (!(lookup_dict = PyDict_New()))
	પાછા ખાલી;

    જો (list_py_concat(l, obj, lookup_dict) == -1)
    {
	Py_DECREF(lookup_dict);
	પાછા ખાલી;
    }
    Py_DECREF(lookup_dict);

    Py_INCREF((PyObject *)self);
    પાછા (PyObject *)(self);
}

પ્રકારવ્યાખ્યા સંરચના
{
    listwatch_T	lw;
    list_T	*list;
} listiterinfo_T;

    સ્થિર શૂન્ય
ListIterDestruct(શૂન્ય *arg)
{
    listiterinfo_T *lii = (listiterinfo_T*)arg;
    list_rem_watch(lii->list, &lii->lw);
    list_unref(lii->list);
    PyMem_Free(lii);
}

    સ્થિર PyObject *
ListIterNext(શૂન્ય **arg)
{
    PyObject	*ret;
    listiterinfo_T **lii = (listiterinfo_T**)arg;

    જો (!((*lii)->lw.lw_item))
	પાછા ખાલી;

    જો (!(ret = ConvertToPyObject(&((*lii)->lw.lw_item->li_tv))))
	પાછા ખાલી;

    (*lii)->lw.lw_item = (*lii)->lw.lw_item->li_next;

    પાછા ret;
}

    સ્થિર PyObject *
ListIter(PyObject *self_obj)
{
    ListObject	*self = (ListObject*)self_obj;
    listiterinfo_T	*lii;
    list_T	*l = self->list;

    જો (!(lii = PyMem_New(listiterinfo_T, 1)))
    {
	PyErr_NoMemory();
	પાછા ખાલી;
    }

    CHECK_LIST_MATERIALIZE(l);
    list_add_watch(l, &lii->lw);
    lii->lw.lw_item = l->lv_first;
    lii->list = l;
    ++l->lv_refcount;

    પાછા IterNew(lii,
	    ListIterDestruct, ListIterNext,
	    ખાલી, ખાલી, (PyObject *)self);
}

સ્થિર અક્ષર *ListAttrs[] = {
    "locked",
    ખાલી
};

    સ્થિર PyObject *
ListDir(PyObject *self, PyObject *args UNUSED)
{
    પાછા ObjectDir(self, ListAttrs);
}

    સ્થિર પૂર્ણાંક
ListSetattr(PyObject *self_obj, અક્ષર *name, PyObject *valObject)
{
    ListObject *self = (ListObject*)self_obj;
    જો (valObject == ખાલી)
    {
	PyErr_SET_STRING(PyExc_AttributeError,
		N_("cannot delete vim.List attributes"));
	પાછા -1;
    }

    જો (strcmp(name, "locked") == 0)
    {
	જો (self->list->lv_lock == VAR_FIXED)
	{
	    PyErr_SET_STRING(PyExc_TypeError, N_("cannot modify fixed list"));
	    પાછા -1;
	}
	નહીંતર
	{
	    પૂર્ણાંક		istrue = PyObject_IsTrue(valObject);
	    જો (istrue == -1)
		પાછા -1;
	    નહીંતર જો (istrue)
		self->list->lv_lock = VAR_LOCKED;
	    નહીંતર
		self->list->lv_lock = 0;
	}
	પાછા 0;
    }
    નહીંતર
    {
	PyErr_FORMAT(PyExc_AttributeError, N_("cannot set attribute %s"), name);
	પાછા -1;
    }
}

સ્થિર PySequenceMethods ListAsSeq = {
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

સ્થિર PyMappingMethods ListAsMapping = {
    /* mp_length	*/ (lenfunc) ListLength,
    /* mp_subscript     */ (binaryfunc) ListItem,
    /* mp_ass_subscript */ (objobjargproc) ListAssItem,
};

સ્થિર સંરચના PyMethodDef ListMethods[] = {
    {"extend",	(PyCFunction)ListConcatInPlace,	METH_O,		""},
    {"__dir__",	(PyCFunction)ListDir,		METH_NOARGS,	""},
    { ખાલી,	ખાલી,				0,		ખાલી}
};

DEFINE_PY_TYPE_OBJECT(TupleType);

પ્રકારવ્યાખ્યા સંરચના
{
    PyObject_HEAD
    tuple_T	*tuple;
    pylinkedlist_T	ref;
} TupleObject;

#define NEW_TUPLE(tuple) TupleNew(TupleTypePtr, tuple)

    સ્થિર PyObject *
TupleNew(PyTypeObject *subtype, tuple_T *tuple)
{
    TupleObject	*self;

    જો (tuple == ખાલી)
	પાછા ખાલી;

    self = (TupleObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    જો (self == ખાલી)
	પાછા ખાલી;
    self->tuple = tuple;
    ++tuple->tv_refcount;

    pyll_add((PyObject *)(self), &self->ref, &lasttuple);

    પાછા (PyObject *)(self);
}

    સ્થિર tuple_T *
py_tuple_alloc(શૂન્ય)
{
    tuple_T	*ret;

    જો (!(ret = tuple_alloc()))
    {
	PyErr_NoMemory();
	પાછા ખાલી;
    }
    ++ret->tv_refcount;

    પાછા ret;
}

    સ્થિર પૂર્ણાંક
tuple_py_concat(tuple_T *t, PyObject *obj, PyObject *lookup_dict)
{
    PyObject	*iterator;
    PyObject	*item;

    જો (!(iterator = PyObject_GetIter(obj)))
	પાછા -1;

    જ્યારે ((item = PyIter_Next(iterator)))
    {
	typval_T    new_tv;

	જો (_ConvertFromPyObject(item, &new_tv, lookup_dict) == -1)
	{
	    Py_DECREF(item);
	    Py_DECREF(iterator);
	    પાછા -1;
	}

	Py_DECREF(item);

	જો (tuple_append_tv(t, &new_tv) == FAIL)
	{
	    Py_DECREF(iterator);
	    પાછા -1;
	}
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    જો (PyErr_Occurred())
	પાછા -1;

    પાછા 0;
}

    સ્થિર PyObject *
TupleConstructor(PyTypeObject *subtype, PyObject *args, PyObject *kwargs)
{
    tuple_T	*tuple;
    PyObject	*obj = ખાલી;

    જો (kwargs)
    {
	PyErr_SET_STRING(PyExc_TypeError,
		N_("tuple constructor does not accept keyword arguments"));
	પાછા ખાલી;
    }

    જો (!PyArg_ParseTuple(args, "|O", &obj))
	પાછા ખાલી;

    જો (!(tuple = py_tuple_alloc()))
	પાછા ખાલી;

    જો (obj)
    {
	PyObject	*lookup_dict;

	જો (!(lookup_dict = PyDict_New()))
	{
	    tuple_unref(tuple);
	    પાછા ખાલી;
	}

	જો (tuple_py_concat(tuple, obj, lookup_dict) == -1)
	{
	    Py_DECREF(lookup_dict);
	    tuple_unref(tuple);
	    પાછા ખાલી;
	}

	Py_DECREF(lookup_dict);
    }

    પાછા TupleNew(subtype, tuple);
}

    સ્થિર શૂન્ય
TupleDestructor(PyObject *self_obj)
{
    TupleObject *self = (TupleObject *)self_obj;
    pyll_remove(&self->ref, &lasttuple);
    tuple_unref(self->tuple);

    DESTRUCTOR_FINISH(self);
}

    સ્થિર PyInt
TupleLength(TupleObject *self)
{
    પાછા ((PyInt)(tuple_len(self->tuple)));
}

    સ્થિર PyObject *
TupleIndex(TupleObject *self, Py_ssize_t index)
{
    PyInt	len = TupleLength(self);

    જો (index < 0)
	index = len + index;

    જો (index < 0 || index >= len)
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("tuple index out of range"));
	પાછા ખાલી;
    }
    પાછા ConvertToPyObject(TUPLE_ITEM(self->tuple, index));
}

/*
 * Return a new tuple object for the tuple slice starting from the index
 * "first" and of length "slicelen" skipping "step" items.
 */
    સ્થિર PyObject *
TupleSlice(
    TupleObject		*self,
    Py_ssize_t		first,
    Py_ssize_t		step,
    Py_ssize_t		slicelen)
{
    PyInt	i;
    PyObject	*tuple;

    tuple = PyTuple_New(slicelen);
    જો (tuple == ખાલી)
	પાછા ખાલી;

    માટે (i = 0; i < slicelen; ++i)
    {
	PyObject	*item;

	item = TupleIndex(self, first + i * step);
	જો (item == ખાલી)
	{
	    Py_DECREF(tuple);
	    પાછા ખાલી;
	}

	PyTuple_SetItem(tuple, i, item);
    }

    પાછા tuple;
}

    સ્થિર PyObject *
TupleItem(TupleObject *self, PyObject* idx)
{
#જો PY_MAJOR_VERSION < 3
    જો (PyInt_Check(idx))
    {
	long _idx = PyInt_AsLong(idx);
	પાછા TupleIndex(self, _idx);
    }
    નહીંતર
#endif
    જો (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	પાછા TupleIndex(self, _idx);
    }
    નહીંતર જો (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	જો (PySlice_GetIndicesEx((PySliceObject_T *)idx, TupleLength(self),
				 &start, &stop, &step, &slicelen) < 0)
	    પાછા ખાલી;
	પાછા TupleSlice(self, start, step, slicelen);
    }
    નહીંતર
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	પાછા ખાલી;
    }
}

પ્રકારવ્યાખ્યા સંરચના
{
    tuple_T	*tuple;
    પૂર્ણાંક		index;
} tupleiterinfo_T;

    સ્થિર શૂન્ય
TupleIterDestruct(શૂન્ય *arg)
{
    tupleiterinfo_T *tii = (tupleiterinfo_T*)arg;
    tuple_unref(tii->tuple);
    PyMem_Free(tii);
}

    સ્થિર PyObject *
TupleIterNext(શૂન્ય **arg)
{
    PyObject	*ret;
    tupleiterinfo_T **tii = (tupleiterinfo_T**)arg;

    જો ((*tii)->index >= TUPLE_LEN((*tii)->tuple))
	પાછા ખાલી;

    જો (!(ret = ConvertToPyObject(TUPLE_ITEM((*tii)->tuple, (*tii)->index))))
	પાછા ખાલી;

    (*tii)->index++;

    પાછા ret;
}

    સ્થિર PyObject *
TupleIter(PyObject *self_obj)
{
    TupleObject	*self = (TupleObject*)self_obj;
    tupleiterinfo_T	*tii;
    tuple_T	*t = self->tuple;

    જો (!(tii = PyMem_New(tupleiterinfo_T, 1)))
    {
	PyErr_NoMemory();
	પાછા ખાલી;
    }

    tii->tuple = t;
    tii->index = 0;
    ++t->tv_refcount;

    પાછા IterNew(tii,
	    TupleIterDestruct, TupleIterNext,
	    ખાલી, ખાલી, (PyObject *)self);
}

સ્થિર અક્ષર *TupleAttrs[] = {
    "locked",
    ખાલી
};

    સ્થિર PyObject *
TupleDir(PyObject *self, PyObject *args UNUSED)
{
    પાછા ObjectDir(self, TupleAttrs);
}

    સ્થિર પૂર્ણાંક
TupleSetattr(PyObject *self_obj, અક્ષર *name, PyObject *valObject)
{
    TupleObject *self = (TupleObject*)self_obj;
    જો (valObject == ખાલી)
    {
	PyErr_SET_STRING(PyExc_AttributeError,
		N_("cannot delete vim.Tuple attributes"));
	પાછા -1;
    }

    જો (strcmp(name, "locked") == 0)
    {
	જો (self->tuple->tv_lock == VAR_FIXED)
	{
	    PyErr_SET_STRING(PyExc_TypeError, N_("cannot modify fixed tuple"));
	    પાછા -1;
	}
	નહીંતર
	{
	    પૂર્ણાંક		istrue = PyObject_IsTrue(valObject);
	    જો (istrue == -1)
		પાછા -1;
	    નહીંતર જો (istrue)
		self->tuple->tv_lock = VAR_LOCKED;
	    નહીંતર
		self->tuple->tv_lock = 0;
	}
	પાછા 0;
    }
    નહીંતર
    {
	PyErr_FORMAT(PyExc_AttributeError, N_("cannot set attribute %s"), name);
	પાછા -1;
    }
}

સ્થિર PySequenceMethods TupleAsSeq = {
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

સ્થિર PyMappingMethods TupleAsMapping = {
    /* mp_length	*/ (lenfunc) TupleLength,
    /* mp_subscript     */ (binaryfunc) TupleItem,
    /* mp_ass_subscript */ (objobjargproc) 0,
};

સ્થિર સંરચના PyMethodDef TupleMethods[] = {
    {"__dir__",	(PyCFunction)TupleDir,		METH_NOARGS,	""},
    { ખાલી,	ખાલી,				0,		ખાલી}
};

પ્રકારવ્યાખ્યા સંરચના
{
    PyObject_HEAD
    char_u	*name;
    પૂર્ણાંક		argc;
    typval_T	*argv;
    dict_T	*self;
    pylinkedlist_T	ref;
    પૂર્ણાંક		auto_rebind;
} FunctionObject;

DEFINE_PY_TYPE_OBJECT(FunctionType);

#define NEW_FUNCTION(name, argc, argv, self, pt_auto) \
    FunctionNew(FunctionTypePtr, (name), (argc), (argv), (self), (pt_auto))

    સ્થિર PyObject *
FunctionNew(PyTypeObject *subtype, char_u *name, પૂર્ણાંક argc, typval_T *argv,
	dict_T *selfdict, પૂર્ણાંક auto_rebind)
{
    FunctionObject	*self;

    self = (FunctionObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    જો (self == ખાલી)
	પાછા ખાલી;

    જો (isdigit((unsigned અક્ષર)*name))
    {
	જો (!translated_function_exists(name, FALSE))
	{
	    PyErr_FORMAT(Py_Exc_કિંમત_Error,
		    N_("unnamed function %s does not exist"), name);
	    પાછા ખાલી;
	}
	self->name = vim_strsave(name);
    }
    નહીંતર
    {
	char_u *p;

	જો ((p = get_expanded_name(name,
			    vim_strchr(name, AUTOLOAD_CHAR) == ખાલી)) == ખાલી)
	{
	    PyErr_FORMAT(Py_Exc_કિંમત_Error,
		    N_("function %s does not exist"), name);
	    પાછા ખાલી;
	}

	જો (p[0] == K_SPECIAL && p[1] == KS_EXTRA && p[2] == (પૂર્ણાંક)KE_SNR)
	{
	    char_u *np;
	    size_t len = STRLEN(p) + 1;

	    જો ((np = alloc(len + 2)) == ખાલી)
	    {
		vim_free(p);
		પાછા ખાલી;
	    }
	    mch_memmove(np, "<SNR>", 5);
	    mch_memmove(np + 5, p + 3, len - 3);
	    vim_free(p);
	    self->name = np;
	}
	નહીંતર
	    self->name = p;
    }

    func_ref(self->name);
    self->argc = argc;
    self->argv = argv;
    self->self = selfdict;
    self->auto_rebind = selfdict == ખાલી ? TRUE : auto_rebind;

    જો (self->argv || self->self)
	pyll_add((PyObject *)(self), &self->ref, &lastfunc);

    પાછા (PyObject *)(self);
}

    સ્થિર PyObject *
FunctionConstructor(PyTypeObject *subtype, PyObject *args, PyObject *kwargs)
{
    PyObject	*self;
    PyObject	*selfdictObject;
    PyObject	*autoRebindObject;
    PyObject	*argsObject = ખાલી;
    char_u	*name;
    typval_T	selfdicttv;
    typval_T	argstv;
    list_T	*argslist = ખાલી;
    dict_T	*selfdict = ખાલી;
    પૂર્ણાંક		argc = 0;
    પૂર્ણાંક		auto_rebind = TRUE;
    typval_T	*argv = ખાલી;
    typval_T	*curtv;
    listitem_T	*li;

    જો (kwargs != ખાલી)
    {
	selfdictObject = PyDict_GetItemString(kwargs, "self");
	જો (selfdictObject != ખાલી)
	{
	    જો (ConvertFromPyMapping(selfdictObject, &selfdicttv) == -1)
		પાછા ખાલી;
	    selfdict = selfdicttv.vval.v_dict;
	}
	argsObject = PyDict_GetItemString(kwargs, "args");
	જો (argsObject != ખાલી)
	{
	    જો (ConvertFromPySequence(argsObject, &argstv) == -1)
	    {
		dict_unref(selfdict);
		પાછા ખાલી;
	    }
	    argslist = argstv.vval.v_list;
	    CHECK_LIST_MATERIALIZE(argslist);

	    argc = argslist->lv_len;
	    જો (argc != 0)
	    {
		argv = PyMem_New(typval_T, (size_t) argc);
		જો (argv == ખાલી)
		{
		    PyErr_NoMemory();
		    dict_unref(selfdict);
		    list_unref(argslist);
		    પાછા ખાલી;
		}
		curtv = argv;
		FOR_ALL_LIST_વસ્તુઓ(argslist, li)
		    copy_tv(&li->li_tv, curtv++);
	    }
	    list_unref(argslist);
	}
	જો (selfdict != ખાલી)
	{
	    auto_rebind = FALSE;
	    autoRebindObject = PyDict_GetItemString(kwargs, "auto_rebind");
	    જો (autoRebindObject != ખાલી)
	    {
		auto_rebind = PyObject_IsTrue(autoRebindObject);
		જો (auto_rebind == -1)
		{
		    dict_unref(selfdict);
		    list_unref(argslist);
		    પાછા ખાલી;
		}
	    }
	}
    }

    જો (!PyArg_ParseTuple(args, "et", "ascii", &name))
    {
	dict_unref(selfdict);
	જ્યારે (argc--)
	    clear_tv(&argv[argc]);
	PyMem_Free(argv);
	પાછા ખાલી;
    }

    self = FunctionNew(subtype, name, argc, argv, selfdict, auto_rebind);

    PyMem_Free(name);

    પાછા self;
}

    સ્થિર શૂન્ય
FunctionDestructor(PyObject *self_obj)
{
    FunctionObject *self = (FunctionObject*)self_obj;
    પૂર્ણાંક i;
    func_unref(self->name);
    vim_free(self->name);
    માટે (i = 0; i < self->argc; ++i)
	clear_tv(&self->argv[i]);
    PyMem_Free(self->argv);
    dict_unref(self->self);
    જો (self->argv || self->self)
	pyll_remove(&self->ref, &lastfunc);

    DESTRUCTOR_FINISH(self);
}

સ્થિર અક્ષર *FunctionAttrs[] = {
    "softspace", "args", "self", "auto_rebind",
    ખાલી
};

    સ્થિર PyObject *
FunctionDir(PyObject *self, PyObject *args UNUSED)
{
    પાછા ObjectDir(self, FunctionAttrs);
}

    સ્થિર PyObject *
FunctionAttr(FunctionObject *self, અક્ષર *name)
{
    list_T *list;
    પૂર્ણાંક i;
    જો (strcmp(name, "name") == 0)
	પાછા PyString_FromString((અક્ષર *)(self->name));
    નહીંતર જો (strcmp(name, "args") == 0)
    {
	જો (self->argv == ખાલી || (list = list_alloc()) == ખાલી)
	    પાછા ALWAYS_NONE;

	માટે (i = 0; i < self->argc; ++i)
	    list_append_tv(list, &self->argv[i]);
	પાછા NEW_LIST(list);
    }
    નહીંતર જો (strcmp(name, "self") == 0)
	પાછા self->self == ખાલી
	    ? ALWAYS_NONE
	    : NEW_DICTIONARY(self->self);
    નહીંતર જો (strcmp(name, "auto_rebind") == 0)
	પાછા self->auto_rebind
	    ? ALWAYS_TRUE
	    : ALWAYS_FALSE;
    નહીંતર જો (strcmp(name, "__members__") == 0)
	પાછા ObjectDir(ખાલી, FunctionAttrs);
    પાછા ખાલી;
}

/*
 * Populate partial_T given function object.
 *
 * "exported" should be set to true when it is needed to construct a partial
 * that may be stored in a variable (i.e. may be freed by Vim).
 */
    સ્થિર શૂન્ય
set_partial(FunctionObject *self, partial_T *pt, પૂર્ણાંક exported)
{
    પૂર્ણાંક i;

    pt->pt_name = self->name;
    જો (self->argv)
    {
	pt->pt_argc = self->argc;
	જો (exported)
	{
	    pt->pt_argv = ALLOC_CLEAR_MULT(typval_T, self->argc);
	    માટે (i = 0; i < pt->pt_argc; ++i)
		copy_tv(&self->argv[i], &pt->pt_argv[i]);
	}
	નહીંતર
	    pt->pt_argv = self->argv;
    }
    નહીંતર
    {
	pt->pt_argc = 0;
	pt->pt_argv = ખાલી;
    }
    pt->pt_auto = self->auto_rebind || !exported;
    pt->pt_dict = self->self;
    જો (exported && self->self)
	++pt->pt_dict->dv_refcount;
    જો (exported)
	pt->pt_name = vim_strsave(pt->pt_name);
    pt->pt_refcount = 1;
}

    સ્થિર PyObject *
FunctionCall(PyObject *self_obj, PyObject *argsObject, PyObject *kwargs)
{
    FunctionObject *self = (FunctionObject*)self_obj;
    char_u	*name = self->name;
    typval_T	args;
    typval_T	selfdicttv;
    typval_T	rettv;
    dict_T	*selfdict = ખાલી;
    PyObject	*selfdictObject;
    PyObject	*ret;
    પૂર્ણાંક		error;
    partial_T	pt;
    partial_T	*pt_ptr = ખાલી;

    જો (ConvertFromPySequence(argsObject, &args) == -1)
	પાછા ખાલી;

    જો (kwargs != ખાલી)
    {
	selfdictObject = PyDict_GetItemString(kwargs, "self");
	જો (selfdictObject != ખાલી)
	{
	    જો (ConvertFromPyMapping(selfdictObject, &selfdicttv) == -1)
	    {
		clear_tv(&args);
		પાછા ખાલી;
	    }
	    selfdict = selfdicttv.vval.v_dict;
	}
    }

    જો (self->argv || self->self)
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

    જો (VimTryEnd())
	ret = ખાલી;
    નહીંતર જો (error != OK)
    {
	ret = ખાલી;
	PyErr_VIM_FORMAT(N_("failed to run function %s"), (અક્ષર *)name);
    }
    નહીંતર
	ret = ConvertToPyObject(&rettv);

    clear_tv(&args);
    clear_tv(&rettv);
    જો (selfdict != ખાલી)
	clear_tv(&selfdicttv);

    પાછા ret;
}

    સ્થિર PyObject *
FunctionRepr(PyObject *self_obj)
{
    FunctionObject *self = (FunctionObject*)self_obj;
    PyObject *ret;
    garray_T repr_ga;
    પૂર્ણાંક i;
    char_u *tofree = ખાલી;
    typval_T tv;
    char_u numbuf[NUMBUFLEN];

    ga_init2(&repr_ga, કદ(અક્ષર), 70);
    ga_concat(&repr_ga, (char_u *)"<vim.Function '");
    જો (self->name)
	ga_concat(&repr_ga, self->name);
    નહીંતર
	ga_concat(&repr_ga, (char_u *)"<NULL>");
    ga_append(&repr_ga, '\'');
    જો (self->argv)
    {
	ga_concat(&repr_ga, (char_u *)", args=[");
	++emsg_silent;
	માટે (i = 0; i < self->argc; i++)
	{
	    જો (i != 0)
		ga_concat(&repr_ga, (char_u *)", ");
	    ga_concat(&repr_ga, tv2string(&self->argv[i], &tofree, numbuf,
			get_copyID()));
	    vim_free(tofree);
	}
	--emsg_silent;
	ga_append(&repr_ga, ']');
    }
    જો (self->self)
    {
	ga_concat(&repr_ga, (char_u *)", self=");
	tv.v_type = VAR_DICT;
	tv.vval.v_dict = self->self;
	++emsg_silent;
	ga_concat(&repr_ga, tv2string(&tv, &tofree, numbuf, get_copyID()));
	--emsg_silent;
	vim_free(tofree);
	જો (self->auto_rebind)
	    ga_concat(&repr_ga, (char_u *)", auto_rebind=True");
    }
    ga_append(&repr_ga, '>');
    ret = PyString_FromString((અક્ષર *)repr_ga.ga_data);
    ga_clear(&repr_ga);
    પાછા ret;
}

સ્થિર સંરચના PyMethodDef FunctionMethods[] = {
    {"__dir__",	(PyCFunction)FunctionDir,   METH_NOARGS,		""},
    { ખાલી,	ખાલી,			0,				ખાલી}
};

/*
 * Options object
 */

DEFINE_PY_TYPE_OBJECT(OptionsType);

પ્રકારવ્યાખ્યા પૂર્ણાંક (*checkfun)(શૂન્ય *);

પ્રકારવ્યાખ્યા સંરચના
{
    PyObject_HEAD
    પૂર્ણાંક		opt_type;
    શૂન્ય	*from;
    checkfun	Check;
    PyObject	*fromObj;
} OptionsObject;

    સ્થિર પૂર્ણાંક
dummy_check(શૂન્ય *arg UNUSED)
{
    પાછા 0;
}

    સ્થિર PyObject *
OptionsNew(પૂર્ણાંક opt_type, શૂન્ય *from, checkfun Check, PyObject *fromObj)
{
    OptionsObject	*self;

    self = PyObject_GC_New(OptionsObject, OptionsTypePtr);
    જો (self == ખાલી)
	પાછા ખાલી;

    self->opt_type = opt_type;
    self->from = from;
    self->Check = Check;
    self->fromObj = fromObj;
    જો (fromObj)
	Py_INCREF(fromObj);

    પાછા (PyObject *)(self);
}

    સ્થિર શૂન્ય
OptionsDestructor(PyObject *self_obj)
{
    OptionsObject *self = (OptionsObject*)self_obj;
    PyObject_GC_UnTrack((શૂન્ય *)(self));
    Py_XDECREF(self->fromObj);
    PyObject_GC_Del((શૂન્ય *)(self));
}

    સ્થિર પૂર્ણાંક
OptionsTraverse(PyObject *self_obj, visitproc visit, શૂન્ય *arg)
{
    OptionsObject *self = (OptionsObject*)self_obj;
    Py_VISIT(self->fromObj);
    પાછા 0;
}

    સ્થિર પૂર્ણાંક
OptionsClear(PyObject *self_obj)
{
    OptionsObject *self = (OptionsObject*)self_obj;
    Py_CLEAR(self->fromObj);
    પાછા 0;
}

    સ્થિર PyObject *
OptionsItem(OptionsObject *self, PyObject *ચાવી_Object)
{
    char_u	*ચાવી;
    પૂર્ણાંક		flags;
    long	numval;
    char_u	*stringval;
    PyObject	*todecref;

    જો (self->Check(self->fromObj))
	પાછા ખાલી;

    જો (!(ચાવી = StringToChars(ચાવી_Object, &todecref)))
	પાછા ખાલી;

    જો (*ચાવી == NUL)
    {
	RAISE_NO_EMPTY_ચાવીઓ;
	Py_XDECREF(todecref);
	પાછા ખાલી;
    }

    flags = get_option_કિંમત_strict(ચાવી, &numval, &stringval,
				    self->opt_type, self->from);

    Py_XDECREF(todecref);

    જો (flags == 0)
    {
	PyErr_SetObject(Py_Exc_ચાવી_Error, ચાવી_Object);
	પાછા ખાલી;
    }

    જો (flags & SOPT_UNSET)
    {
	Py_INCREF(Py_None);
	પાછા Py_None;
    }
    નહીંતર જો (flags & SOPT_BOOL)
    {
	PyObject	*ret;
	ret = numval ? Py_True : Py_False;
	Py_INCREF(ret);
	પાછા ret;
    }
    નહીંતર જો (flags & SOPT_અંક)
	પાછા PyInt_FromLong(numval);
    નહીંતર જો (flags & SOPT_STRING)
    {
	જો (stringval)
	{
	    PyObject	*ret = PyBytes_FromString((અક્ષર *)stringval);
	    vim_free(stringval);
	    પાછા ret;
	}
	નહીંતર
	{
	    PyErr_SET_STRING(PyExc_RuntimeError,
		    N_("unable to get option value"));
	    પાછા ખાલી;
	}
    }
    નહીંતર
    {
	PyErr_SET_VIM(N_("internal error: unknown option type"));
	પાછા ખાલી;
    }
}

    સ્થિર પૂર્ણાંક
OptionsContains(OptionsObject *self, PyObject *ચાવી_Object)
{
    char_u	*ચાવી;
    PyObject	*todecref;

    જો (!(ચાવી = StringToChars(ચાવી_Object, &todecref)))
	પાછા -1;

    જો (*ચાવી == NUL)
    {
	Py_XDECREF(todecref);
	પાછા 0;
    }

    જો (get_option_કિંમત_strict(ચાવી, ખાલી, ખાલી, self->opt_type, ખાલી))
    {
	Py_XDECREF(todecref);
	પાછા 1;
    }
    નહીંતર
    {
	Py_XDECREF(todecref);
	પાછા 0;
    }
}

પ્રકારવ્યાખ્યા સંરચના
{
    શૂન્ય	*lastoption;
    પૂર્ણાંક		opt_type;
} optiterinfo_T;

    સ્થિર PyObject *
OptionsIterNext(શૂન્ય **arg)
{
    char_u	*name;
    optiterinfo_T **oii = (optiterinfo_T**)arg;

    જો ((name = option_iter_next(&((*oii)->lastoption), (*oii)->opt_type)))
	પાછા PyString_FromString((અક્ષર *)name);

    પાછા ખાલી;
}

    સ્થિર PyObject *
OptionsIter(PyObject *self_obj)
{
    OptionsObject	*self = (OptionsObject*)self_obj;
    optiterinfo_T	*oii;

    જો (!(oii = PyMem_New(optiterinfo_T, 1)))
    {
	PyErr_NoMemory();
	પાછા ખાલી;
    }

    oii->opt_type = self->opt_type;
    oii->lastoption = ખાલી;

    પાછા IterNew(oii,
	    PyMem_Free, OptionsIterNext,
	    ખાલી, ખાલી, (PyObject *)self);
}

    સ્થિર પૂર્ણાંક
set_option_કિંમત_err(char_u *ચાવી, પૂર્ણાંક numval, char_u *stringval, પૂર્ણાંક opt_flags)
{
    અક્ષર	*errmsg;

    જો ((errmsg = set_option_કિંમત(ચાવી, numval, stringval, opt_flags)))
    {
	જો (VimTryEnd())
	    પાછા FAIL;
	PyErr_SetVim(errmsg);
	પાછા FAIL;
    }
    પાછા OK;
}

    સ્થિર પૂર્ણાંક
set_option_કિંમત_for(
	char_u *ચાવી,
	પૂર્ણાંક numval,
	char_u *stringval,
	પૂર્ણાંક opt_flags,
	પૂર્ણાંક opt_type,
	શૂન્ય *from)
{
    switchwin_T	switchwin;
    bufref_T	save_curbuf;
    પૂર્ણાંક		set_ret = 0;

    VimTryStart();
    પસંદગી (opt_type)
    {
	સ્થિતિ SREQ_WIN:
	    જો (switch_win(&switchwin, (win_T *)from,
			      win_find_tabpage((win_T *)from), FALSE) == FAIL)
	    {
		restore_win(&switchwin, TRUE);
		જો (VimTryEnd())
		    પાછા -1;
		PyErr_SET_VIM(N_("problem while switching windows"));
		પાછા -1;
	    }
	    set_ret = set_option_કિંમત_err(ચાવી, numval, stringval, opt_flags);
	    restore_win(&switchwin, TRUE);
	    તોડો;
	સ્થિતિ SREQ_BUF:
	    switch_buffer(&save_curbuf, (buf_T *)from);
	    set_ret = set_option_કિંમત_err(ચાવી, numval, stringval, opt_flags);
	    restore_buffer(&save_curbuf);
	    તોડો;
	સ્થિતિ SREQ_GLOBAL:
	    set_ret = set_option_કિંમત_err(ચાવી, numval, stringval, opt_flags);
	    તોડો;
    }
    જો (set_ret == FAIL)
	પાછા -1;
    પાછા VimTryEnd();
}

    સ્થિર પૂર્ણાંક
OptionsAssItem(OptionsObject *self, PyObject *ચાવી_Object, PyObject *valObject)
{
    char_u	*ચાવી;
    પૂર્ણાંક		flags;
    પૂર્ણાંક		opt_flags;
    પૂર્ણાંક		ret = 0;
    PyObject	*todecref;

    જો (self->Check(self->fromObj))
	પાછા -1;

    જો (!(ચાવી = StringToChars(ચાવી_Object, &todecref)))
	પાછા -1;

    જો (*ચાવી == NUL)
    {
	RAISE_NO_EMPTY_ચાવીઓ;
	Py_XDECREF(todecref);
	પાછા -1;
    }

    flags = get_option_કિંમત_strict(ચાવી, ખાલી, ખાલી,
				    self->opt_type, self->from);

    જો (flags == 0)
    {
	PyErr_SetObject(Py_Exc_ચાવી_Error, ચાવી_Object);
	Py_XDECREF(todecref);
	પાછા -1;
    }

    જો (valObject == ખાલી)
    {
	જો (self->opt_type == SREQ_GLOBAL)
	{
	    PyErr_FORMAT(Py_Exc_કિંમત_Error,
		    N_("unable to unset global option %s"), ચાવી);
	    Py_XDECREF(todecref);
	    પાછા -1;
	}
	નહીંતર જો (!(flags & SOPT_GLOBAL))
	{
	    PyErr_FORMAT(Py_Exc_કિંમત_Error,
		    N_("unable to unset option %s "
		       "which does not have global value"), ચાવી);
	    Py_XDECREF(todecref);
	    પાછા -1;
	}
	નહીંતર
	{
	    unset_global_local_option(ચાવી, self->from);
	    Py_XDECREF(todecref);
	    પાછા 0;
	}
    }

    opt_flags = (self->opt_type ? OPT_LOCAL : OPT_GLOBAL);

    જો (flags & SOPT_BOOL)
    {
	પૂર્ણાંક	istrue = PyObject_IsTrue(valObject);

	જો (istrue == -1)
	    ret = -1;
	નહીંતર
	    ret = set_option_કિંમત_for(ચાવી, istrue, ખાલી,
				    opt_flags, self->opt_type, self->from);
    }
    નહીંતર જો (flags & SOPT_અંક)
    {
	long	val;

	જો (સંખ્યા_To_Long(valObject, &val, સંખ્યા_INT))
	{
	    Py_XDECREF(todecref);
	    પાછા -1;
	}

	ret = set_option_કિંમત_for(ચાવી, (પૂર્ણાંક) val, ખાલી, opt_flags,
				self->opt_type, self->from);
    }
    નહીંતર
    {
	char_u		*val;
	PyObject	*todecref2;

	જો ((val = StringToChars(valObject, &todecref2)))
	{
	    ret = set_option_કિંમત_for(ચાવી, 0, val, opt_flags,
				    self->opt_type, self->from);
	    Py_XDECREF(todecref2);
	}
	નહીંતર
	    ret = -1;
    }

    Py_XDECREF(todecref);

    પાછા ret;
}

સ્થિર PySequenceMethods OptionsAsSeq = {
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

સ્થિર PyMappingMethods OptionsAsMapping = {
    (lenfunc)       ખાલી,
    (binaryfunc)    OptionsItem,
    (objobjargproc) OptionsAssItem,
};

// Tabpage object

પ્રકારવ્યાખ્યા સંરચના
{
    PyObject_HEAD
    tabpage_T	*tab;
} TabPageObject;

સ્થિર PyObject *WinListNew(TabPageObject *tabObject);

DEFINE_PY_TYPE_OBJECT(TabPageType);

    સ્થિર પૂર્ણાંક
CheckTabPage(TabPageObject *self)
{
    જો (self->tab == INVALID_TABPAGE_કિંમત)
    {
	PyErr_SET_VIM(N_("attempt to refer to deleted tab page"));
	પાછા -1;
    }

    પાછા 0;
}

    સ્થિર PyObject *
TabPageNew(tabpage_T *tab)
{
    TabPageObject *self;

    જો (TAB_PYTHON_REF(tab))
    {
	self = TAB_PYTHON_REF(tab);
	Py_INCREF((PyObject *)self);
    }
    નહીંતર
    {
	self = PyObject_NEW(TabPageObject, TabPageTypePtr);
	જો (self == ખાલી)
	    પાછા ખાલી;
	self->tab = tab;
	TAB_PYTHON_REF(tab) = self;
    }

    પાછા (PyObject *)(self);
}

    સ્થિર શૂન્ય
TabPageDestructor(PyObject *self_obj)
{
    TabPageObject *self = (TabPageObject*)self_obj;
    જો (self->tab && self->tab != INVALID_TABPAGE_કિંમત)
	TAB_PYTHON_REF(self->tab) = ખાલી;

    DESTRUCTOR_FINISH(self);
}

સ્થિર અક્ષર *TabPageAttrs[] = {
    "windows", "number", "vars", "window", "valid",
    ખાલી
};

    સ્થિર PyObject *
TabPageDir(PyObject *self, PyObject *args UNUSED)
{
    પાછા ObjectDir(self, TabPageAttrs);
}

    સ્થિર PyObject *
TabPageAttrValid(TabPageObject *self, અક્ષર *name)
{
    PyObject	*ret;

    જો (strcmp(name, "valid") != 0)
	પાછા ખાલી;

    ret = ((self->tab == INVALID_TABPAGE_કિંમત) ? Py_False : Py_True);
    Py_INCREF(ret);
    પાછા ret;
}

    સ્થિર PyObject *
TabPageAttr(TabPageObject *self, અક્ષર *name)
{
    જો (strcmp(name, "windows") == 0)
	પાછા WinListNew(self);
    નહીંતર જો (strcmp(name, "number") == 0)
	પાછા PyLong_FromLong((long) get_tab_સંખ્યા(self->tab));
    નહીંતર જો (strcmp(name, "vars") == 0)
	પાછા NEW_DICTIONARY(self->tab->tp_vars);
    નહીંતર જો (strcmp(name, "window") == 0)
    {
	// For current tab window.c does not bother to set or update tp_curwin
	જો (self->tab == curtab)
	    પાછા WindowNew(curwin, curtab);
	નહીંતર
	    પાછા WindowNew(self->tab->tp_curwin, self->tab);
    }
    નહીંતર જો (strcmp(name, "__members__") == 0)
	પાછા ObjectDir(ખાલી, TabPageAttrs);
    પાછા ખાલી;
}

    સ્થિર PyObject *
TabPageRepr(PyObject *self_obj)
{
    TabPageObject *self = (TabPageObject*)self_obj;
    જો (self->tab == INVALID_TABPAGE_કિંમત)
	પાછા PyString_FromFormat("<tabpage object (deleted) at %p>", (શૂન્ય *)self);
    નહીંતર
    {
	પૂર્ણાંક	t = get_tab_સંખ્યા(self->tab);

	જો (t == 0)
	    પાછા PyString_FromFormat("<tabpage object (unknown) at %p>",
					(શૂન્ય *)self);
	નહીંતર
	    પાછા PyString_FromFormat("<tabpage %d>", t - 1);
    }
}

સ્થિર સંરચના PyMethodDef TabPageMethods[] = {
    // name,	    function,			calling,	documentation
    {"__dir__",	    (PyCFunction)TabPageDir,	METH_NOARGS,	""},
    { ખાલી,	    ખાલી,			0,		ખાલી}
};

/*
 * Window list object
 */

DEFINE_PY_TYPE_OBJECT(TabListType);
સ્થિર PySequenceMethods TabListAsSeq;

પ્રકારવ્યાખ્યા સંરચના
{
    PyObject_HEAD
} TabListObject;

સ્થિર TabListObject TheTabPageList =
{
    PyObject_HEAD_INIT_TYPE(TabListType)
};

    સ્થિર PyInt
TabListLength(PyObject *self UNUSED)
{
    tabpage_T	*tp = first_tabpage;
    PyInt	n = 0;

    જ્યારે (tp != ખાલી)
    {
	++n;
	tp = tp->tp_next;
    }

    પાછા n;
}

    સ્થિર PyObject *
TabListItem(PyObject *self UNUSED, PyInt n)
{
    tabpage_T	*tp;

    માટે (tp = first_tabpage; tp != ખાલી; tp = tp->tp_next, --n)
	જો (n == 0)
	    પાછા TabPageNew(tp);

    PyErr_SET_STRING(PyExc_IndexError, N_("no such tab page"));
    પાછા ખાલી;
}

/*
 * Window object
 */

પ્રકારવ્યાખ્યા સંરચના
{
    PyObject_HEAD
    win_T	*win;
    TabPageObject	*tabObject;
} WindowObject;

DEFINE_PY_TYPE_OBJECT(WindowType);

    સ્થિર પૂર્ણાંક
CheckWindow(WindowObject *self)
{
    જો (self->win == INVALID_WINDOW_કિંમત)
    {
	PyErr_SET_VIM(N_("attempt to refer to deleted window"));
	પાછા -1;
    }

    પાછા 0;
}

    સ્થિર પૂર્ણાંક
CheckWindowCb(શૂન્ય *self)
{
    પાછા CheckWindow((WindowObject*)self);
}

    સ્થિર PyObject *
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

    જો (WIN_PYTHON_REF(win))
    {
	self = WIN_PYTHON_REF(win);
	Py_INCREF((PyObject *)self);
    }
    નહીંતર
    {
	self = PyObject_GC_New(WindowObject, WindowTypePtr);
	જો (self == ખાલી)
	    પાછા ખાલી;
	self->win = win;
	WIN_PYTHON_REF(win) = self;
    }

    self->tabObject = ((TabPageObject *)(TabPageNew(tab)));

    પાછા (PyObject *)(self);
}

    સ્થિર શૂન્ય
WindowDestructor(PyObject *self_obj)
{
    WindowObject *self = (WindowObject*)self_obj;
    PyObject_GC_UnTrack((શૂન્ય *)(self));
    જો (self->win && self->win != INVALID_WINDOW_કિંમત)
	WIN_PYTHON_REF(self->win) = ખાલી;
    Py_XDECREF(((PyObject *)(self->tabObject)));
    PyObject_GC_Del((શૂન્ય *)(self));
}

    સ્થિર પૂર્ણાંક
WindowTraverse(PyObject *self_obj, visitproc visit, શૂન્ય *arg)
{
    WindowObject *self = (WindowObject*)self_obj;
    Py_VISIT(((PyObject *)(self->tabObject)));
    પાછા 0;
}

    સ્થિર પૂર્ણાંક
WindowClear(PyObject *self_obj)
{
    WindowObject *self = (WindowObject*)self_obj;
    Py_CLEAR(self->tabObject);
    પાછા 0;
}

    સ્થિર win_T *
get_firstwin(TabPageObject *tabObject)
{
    જો (tabObject)
    {
	જો (CheckTabPage(tabObject))
	    પાછા ખાલી;
	// For current tab window.c does not bother to set or update tp_firstwin
	નહીંતર જો (tabObject->tab == curtab)
	    પાછા firstwin;
	નહીંતર
	    પાછા tabObject->tab->tp_firstwin;
    }
    નહીંતર
	પાછા firstwin;
}

// Use the same order as in the WindowAttr() function.
સ્થિર અક્ષર *WindowAttrs[] = {
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
    ખાલી
};

    સ્થિર PyObject *
WindowDir(PyObject *self, PyObject *args UNUSED)
{
    પાછા ObjectDir(self, WindowAttrs);
}

    સ્થિર PyObject *
WindowAttrValid(WindowObject *self, અક્ષર *name)
{
    PyObject	*ret;

    જો (strcmp(name, "valid") != 0)
	પાછા ખાલી;

    ret = ((self->win == INVALID_WINDOW_કિંમત) ? Py_False : Py_True);
    Py_INCREF(ret);
    પાછા ret;
}

    સ્થિર PyObject *
WindowAttr(WindowObject *self, અક્ષર *name)
{
    જો (strcmp(name, "buffer") == 0)
	પાછા (PyObject *)BufferNew(self->win->w_buffer);
    નહીંતર જો (strcmp(name, "cursor") == 0)
    {
	pos_T *pos = &self->win->w_cursor;

	પાછા Py_Build_કિંમત("(ll)", (long)(pos->lnum), (long)(pos->col));
    }
    નહીંતર જો (strcmp(name, "height") == 0)
	પાછા PyLong_FromLong((long)(self->win->w_height));
    નહીંતર જો (strcmp(name, "row") == 0)
	પાછા PyLong_FromLong((long)(self->win->w_winrow));
    નહીંતર જો (strcmp(name, "width") == 0)
	પાછા PyLong_FromLong((long)(self->win->w_width));
    નહીંતર જો (strcmp(name, "col") == 0)
	પાછા PyLong_FromLong((long)(self->win->w_wincol));
    નહીંતર જો (strcmp(name, "vars") == 0)
	પાછા NEW_DICTIONARY(self->win->w_vars);
    નહીંતર જો (strcmp(name, "options") == 0)
	પાછા OptionsNew(SREQ_WIN, self->win, CheckWindowCb,
			(PyObject *) self);
    નહીંતર જો (strcmp(name, "number") == 0)
    {
	જો (CheckTabPage(self->tabObject))
	    પાછા ખાલી;
	પાછા PyLong_FromLong((long)
		get_win_સંખ્યા(self->win, get_firstwin(self->tabObject)));
    }
    નહીંતર જો (strcmp(name, "tabpage") == 0)
    {
	Py_INCREF((PyObject *)self->tabObject);
	પાછા (PyObject *)(self->tabObject);
    }
    નહીંતર જો (strcmp(name, "__members__") == 0)
	પાછા ObjectDir(ખાલી, WindowAttrs);
    નહીંતર
	પાછા ખાલી;
}

    સ્થિર પૂર્ણાંક
WindowSetattr(PyObject *self_obj, અક્ષર *name, PyObject *valObject)
{
    WindowObject *self = (WindowObject*)self_obj;
    જો (CheckWindow(self))
	પાછા -1;

    જો (strcmp(name, "buffer") == 0)
    {
	PyErr_SET_STRING(PyExc_TypeError, N_("readonly attribute: buffer"));
	પાછા -1;
    }
    નહીંતર જો (strcmp(name, "cursor") == 0)
    {
	long lnum;
	long col;

	જો (!PyArg_Parse(valObject, "(ll)", &lnum, &col))
	    પાછા -1;

	જો (lnum <= 0 || lnum > self->win->w_buffer->b_ml.ml_લીટી_ગણતરી)
	{
	    PyErr_SET_VIM(N_("cursor position outside buffer"));
	    પાછા -1;
	}

	// Check for keyboard interrupts
	જો (VimCheckInterrupt())
	    પાછા -1;

	self->win->w_cursor.lnum = lnum;
	self->win->w_cursor.col = col;
	self->win->w_set_curswant = TRUE;
	self->win->w_cursor.coladd = 0;
	// When column is out of range silently correct it.
	check_cursor_col_win(self->win);

	update_screen(UPD_VALID);
	પાછા 0;
    }
    નહીંતર જો (strcmp(name, "height") == 0)
    {
	long	height;
	win_T	*savewin;

	જો (સંખ્યા_To_Long(valObject, &height, સંખ્યા_INT|સંખ્યા_UNSIGNED))
	    પાછા -1;

#ifdef FEAT_GUI
	need_mouse_correct = TRUE;
#endif
	savewin = curwin;
	curwin = self->win;
	curbuf = curwin->w_buffer;

	VimTryStart();
	win_setheight((પૂર્ણાંક) height);
	curwin = savewin;
	curbuf = curwin->w_buffer;
	જો (VimTryEnd())
	    પાછા -1;

	પાછા 0;
    }
    નહીંતર જો (strcmp(name, "width") == 0)
    {
	long	width;
	win_T	*savewin;

	જો (સંખ્યા_To_Long(valObject, &width, સંખ્યા_INT|સંખ્યા_UNSIGNED))
	    પાછા -1;

#ifdef FEAT_GUI
	need_mouse_correct = TRUE;
#endif
	savewin = curwin;
	curwin = self->win;
	curbuf = curwin->w_buffer;

	VimTryStart();
	win_setwidth((પૂર્ણાંક) width);
	curwin = savewin;
	curbuf = curwin->w_buffer;
	જો (VimTryEnd())
	    પાછા -1;

	પાછા 0;
    }
    નહીંતર
    {
	PyErr_SetString(PyExc_AttributeError, name);
	પાછા -1;
    }
}

    સ્થિર PyObject *
WindowRepr(PyObject *self_obj)
{
    WindowObject *self = (WindowObject*)self_obj;
    જો (self->win == INVALID_WINDOW_કિંમત)
	પાછા PyString_FromFormat("<window object (deleted) at %p>", (શૂન્ય *)self);
    નહીંતર
    {
	પૂર્ણાંક	w = get_win_સંખ્યા(self->win, firstwin);

	જો (w == 0)
	    પાછા PyString_FromFormat("<window object (unknown) at %p>",
								      (શૂન્ય *)self);
	નહીંતર
	    પાછા PyString_FromFormat("<window %d>", w - 1);
    }
}

સ્થિર સંરચના PyMethodDef WindowMethods[] = {
    // name,	    function,			calling,	documentation
    {"__dir__",	    (PyCFunction)WindowDir,	METH_NOARGS,	""},
    { ખાલી,	    ખાલી,			0,		ખાલી}
};

/*
 * Window list object
 */

DEFINE_PY_TYPE_OBJECT(WinListType);
સ્થિર PySequenceMethods WinListAsSeq;

પ્રકારવ્યાખ્યા સંરચના
{
    PyObject_HEAD
    TabPageObject	*tabObject;
} WinListObject;

સ્થિર WinListObject TheWindowList =
{
    PyObject_HEAD_INIT_TYPE(WinListType)
    ખાલી
};

    સ્થિર PyObject *
WinListNew(TabPageObject *tabObject)
{
    WinListObject	*self;

    self = PyObject_NEW(WinListObject, WinListTypePtr);
    self->tabObject = tabObject;
    Py_INCREF((PyObject *)tabObject);

    પાછા (PyObject *)(self);
}

    સ્થિર શૂન્ય
WinListDestructor(WinListObject *self)
{
    TabPageObject	*tabObject = self->tabObject;

    જો (tabObject)
    {
	Py_DECREF((PyObject *)(tabObject));
    }

    DESTRUCTOR_FINISH(self);
}

    સ્થિર PyInt
WinListLength(WinListObject *self)
{
    win_T	*w;
    PyInt	n = 0;

    જો (!(w = get_firstwin(self->tabObject)))
	પાછા -1;

    જ્યારે (w != ખાલી)
    {
	++n;
	w = W_NEXT(w);
    }

    પાછા n;
}

    સ્થિર PyObject *
WinListItem(WinListObject *self, PyInt n)
{
    win_T *w;

    જો (!(w = get_firstwin(self->tabObject)))
	પાછા ખાલી;

    માટે (; w != ખાલી; w = W_NEXT(w), --n)
	જો (n == 0)
	    પાછા WindowNew(w, self->tabObject? self->tabObject->tab: curtab);

    PyErr_SET_STRING(PyExc_IndexError, N_("no such window"));
    પાછા ખાલી;
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
    સ્થિર અક્ષર *
String_To_લીટી(PyObject *obj)
{
    અક્ષર	*str;
    અક્ષર	*save;
    PyObject	*bytes = ખાલી;
    Py_ssize_t	len = 0;
    PyInt	i;
    અક્ષર	*p;

    જો (PyBytes_Check(obj))
    {
	જો (PyBytes_AsStringAndSize(obj, &str, &len) == -1
		|| str == ખાલી)
	    પાછા ખાલી;
    }
    નહીંતર જો (PyUnicode_Check(obj))
    {
	જો (!(bytes = PyUnicode_AsEncodedString(obj, ENC_OPT,
							   ERRORS_ENCODE_ARG)))
	    પાછા ખાલી;

	જો (PyBytes_AsStringAndSize(bytes, &str, &len) == -1
		|| str == ખાલી)
	{
	    Py_DECREF(bytes);
	    પાછા ખાલી;
	}
    }
    નહીંતર
    {
#જો PY_MAJOR_VERSION < 3
	PyErr_FORMAT_TYPE(
		N_("expected str() or unicode() instance, but got %s"),
		obj);
#નહીંતર
	PyErr_FORMAT_TYPE(
		N_("expected bytes() or str() instance, but got %s"),
		obj);
#endif
	પાછા ખાલી;
    }

    /*
     * Error checking: String must not contain newlines, as we
     * are replacing a single line, and we must replace it with
     * a single line.
     * A trailing newline is removed, so that append(f.readlines()) works.
     */
    p = memchr(str, '\n', len);
    જો (p != ખાલી)
    {
	જો (p == str + len - 1)
	    --len;
	નહીંતર
	{
	    PyErr_SET_VIM(N_("string cannot contain newlines"));
	    Py_XDECREF(bytes);
	    પાછા ખાલી;
	}
    }

    /*
     * Create a copy of the string, with internal nulls replaced by
     * newline characters, as is the Vim convention.
     */
    save = alloc(len+1);
    જો (save == ખાલી)
    {
	PyErr_NoMemory();
	Py_XDECREF(bytes);
	પાછા ખાલી;
    }

    માટે (i = 0; i < len; ++i)
    {
	જો (str[i] == '\0')
	    save[i] = '\n';
	નહીંતર
	    save[i] = str[i];
    }

    save[i] = '\0';
    Py_XDECREF(bytes);  // Python 2 does nothing here

    પાછા save;
}

/*
 * Get a line from the specified buffer. The line number is
 * in Vim format (1-based). The line is returned as a Python
 * string object.
 */
    સ્થિર PyObject *
Get_Buffer_લીટી(buf_T *buf, PyInt n)
{
    પાછા લીટી_To_String((અક્ષર *)ml_get_buf(buf, (linenr_T)n, FALSE));
}


/*
 * Get a list of lines from the specified buffer. The line numbers
 * are in Vim format (1-based). The range is from lo up to, but not
 * including, hi. The list is returned as a Python list of string objects.
 */
    સ્થિર PyObject *
Get_Buffer_લીટી_List(buf_T *buf, PyInt lo, PyInt hi)
{
    PyInt	i;
    PyInt	n = hi - lo;
    PyObject	*list = PyList_New(n);

    જો (list == ખાલી)
	પાછા ખાલી;

    માટે (i = 0; i < n; ++i)
    {
	linenr_T	lnum = (linenr_T)(lo + i);
	અક્ષર		*text;
	PyObject	*string;

	જો (lnum > buf->b_ml.ml_લીટી_ગણતરી)
	    text = "";
	નહીંતર
	    text = (અક્ષર *)ml_get_buf(buf, lnum, FALSE);
	string = લીટી_To_String(text);
	જો (string == ખાલી)
	{
	    Py_DECREF(list);
	    પાછા ખાલી;
	}

	PyList_SET_ITEM(list, i, string);
    }

    // The ownership of the Python list is passed to the caller (ie,
    // the caller should Py_DECREF() the object when it is finished
    // with it).

    પાછા list;
}

/*
 * Check if deleting lines made the cursor position invalid.
 * Changed the lines from "lo" to "hi" and added "extra" lines (negative if
 * deleted).
 */
    સ્થિર શૂન્ય
py_fix_cursor(linenr_T lo, linenr_T hi, linenr_T extra)
{
    જો (curwin->w_cursor.lnum >= lo)
    {
	// Adjust the cursor position if it's in/after the changed
	// lines.
	જો (curwin->w_cursor.lnum >= hi)
	{
	    curwin->w_cursor.lnum += extra;
	    check_cursor_col();
	}
	નહીંતર જો (extra < 0)
	{
	    curwin->w_cursor.lnum = lo;
	    check_cursor();
	}
	નહીંતર
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
    સ્થિર પૂર્ણાંક
Set_Buffer_લીટી(buf_T *buf, PyInt n, PyObject *લીટી, PyInt *len_change)
{
    bufref_T	save_curbuf = {ખાલી, 0, 0};
    switchwin_T	switchwin;

    // First of all, we check the type of the supplied Python object.
    // There are three cases:
    //	  1. NULL, or None - this is a deletion.
    //	  2. A string	   - this is a replacement.
    //	  3. Anything else - this is an error.
    જો (લીટી == Py_None || લીટી == ખાલી)
    {
	PyErr_Clear();
	switchwin.sw_curwin = ખાલી;
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	VimTryStart();

	જો (u_savedel((linenr_T)n, 1L) == FAIL)
	    RAISE_UNDO_FAIL;
	નહીંતર જો (ml_delete((linenr_T)n) == FAIL)
	    RAISE_DELETE_લીટી_FAIL;
	નહીંતર
	{
	    જો (buf == curbuf && (switchwin.sw_curwin != ખાલી
					   || save_curbuf.br_buf == ખાલી))
		// Using an existing window for the buffer, adjust the cursor
		// position.
		py_fix_cursor((linenr_T)n, (linenr_T)n + 1, (linenr_T)-1);
	    જો (save_curbuf.br_buf == ખાલી)
		// Only adjust marks if we managed to switch to a window that
		// holds the buffer, otherwise line numbers will be invalid.
		deleted_લીટીઓ_mark((linenr_T)n, 1L);
	}

	restore_win_for_buf(&switchwin, &save_curbuf);

	જો (VimTryEnd())
	    પાછા FAIL;

	જો (len_change)
	    *len_change = -1;

	પાછા OK;
    }
    નહીંતર જો (PyBytes_Check(લીટી) || PyUnicode_Check(લીટી))
    {
	અક્ષર	*save = String_To_લીટી(લીટી);

	જો (save == ખાલી)
	    પાછા FAIL;

	VimTryStart();

	// We do not need to free "save" if ml_replace() consumes it.
	PyErr_Clear();
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	જો (u_savesub((linenr_T)n) == FAIL)
	{
	    RAISE_UNDO_FAIL;
	    vim_free(save);
	}
	નહીંતર જો (ml_replace((linenr_T)n, (char_u *)save, FALSE) == FAIL)
	{
	    RAISE_REPLACE_લીટી_FAIL;
	    vim_free(save);
	}
	નહીંતર
	    changed_bytes((linenr_T)n, 0);

	restore_win_for_buf(&switchwin, &save_curbuf);

	// Check that the cursor is not beyond the end of the line now.
	જો (buf == curbuf)
	    check_cursor_col();

	જો (VimTryEnd())
	    પાછા FAIL;

	જો (len_change)
	    *len_change = 0;

	પાછા OK;
    }
    નહીંતર
    {
	PyErr_BadArgument();
	પાછા FAIL;
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
    સ્થિર પૂર્ણાંક
Set_Buffer_લીટી_List(
	buf_T *buf,
	PyInt lo,
	PyInt hi,
	PyObject *list,
	PyInt *len_change)
{
    bufref_T	save_curbuf = {ખાલી, 0, 0};
    switchwin_T	switchwin;

    // First of all, we check the type of the supplied Python object.
    // There are three cases:
    //	  1. NULL, or None - this is a deletion.
    //	  2. A list	   - this is a replacement.
    //	  3. Anything else - this is an error.
    જો (list == Py_None || list == ખાલી)
    {
	PyInt	i;
	PyInt	n = (પૂર્ણાંક)(hi - lo);

	PyErr_Clear();
	VimTryStart();
	switchwin.sw_curwin = ખાલી;
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	જો (u_savedel((linenr_T)lo, (long)n) == FAIL)
	    RAISE_UNDO_FAIL;
	નહીંતર
	{
	    માટે (i = 0; i < n; ++i)
	    {
		જો (ml_delete((linenr_T)lo) == FAIL)
		{
		    RAISE_DELETE_લીટી_FAIL;
		    તોડો;
		}
	    }
	    જો (buf == curbuf && (switchwin.sw_curwin != ખાલી
					       || save_curbuf.br_buf == ખાલી))
		// Using an existing window for the buffer, adjust the cursor
		// position.
		py_fix_cursor((linenr_T)lo, (linenr_T)hi, (linenr_T)-n);
	    જો (save_curbuf.br_buf == ખાલી)
		// Only adjust marks if we managed to switch to a window that
		// holds the buffer, otherwise line numbers will be invalid.
		deleted_લીટીઓ_mark((linenr_T)lo, (long)i);
	}

	restore_win_for_buf(&switchwin, &save_curbuf);

	જો (VimTryEnd())
	    પાછા FAIL;

	જો (len_change)
	    *len_change = -n;

	પાછા OK;
    }
    નહીંતર જો (PyList_Check(list))
    {
	PyInt	i;
	PyInt	new_len = PyList_Size(list);
	PyInt	old_len = hi - lo;
	PyInt	extra = 0;	// lines added to text, can be negative
	અક્ષર	**array;

	જો (new_len == 0)	// avoid allocating zero bytes
	    array = ખાલી;
	નહીંતર
	{
	    array = PyMem_New(અક્ષર *, new_len);
	    જો (array == ખાલી)
	    {
		PyErr_NoMemory();
		પાછા FAIL;
	    }
	}

	માટે (i = 0; i < new_len; ++i)
	{
	    PyObject *લીટી;

	    જો (!(લીટી = PyList_GetItem(list, i)) ||
		!(array[i] = String_To_લીટી(લીટી)))
	    {
		જ્યારે (i)
		    vim_free(array[--i]);
		PyMem_Free(array);
		પાછા FAIL;
	    }
	}

	VimTryStart();
	PyErr_Clear();

	// START of region without "return".  Must call restore_buffer()!
	switchwin.sw_curwin = ખાલી;
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	જો (u_save((linenr_T)(lo-1), (linenr_T)hi) == FAIL)
	    RAISE_UNDO_FAIL;

	// If the size of the range is reducing (ie, new_len < old_len) we
	// need to delete some old_len. We do this at the start, by
	// repeatedly deleting line "lo".
	જો (!PyErr_Occurred())
	{
	    માટે (i = 0; i < old_len - new_len; ++i)
		જો (ml_delete((linenr_T)lo) == FAIL)
		{
		    RAISE_DELETE_લીટી_FAIL;
		    તોડો;
		}
	    extra -= i;
	}

	// For as long as possible, replace the existing old_len with the
	// new old_len. This is a more efficient operation, as it requires
	// less memory allocation and freeing.
	જો (!PyErr_Occurred())
	{
	    માટે (i = 0; i < old_len && i < new_len; ++i)
		જો (ml_replace((linenr_T)(lo+i), (char_u *)array[i], FALSE)
								      == FAIL)
		{
		    RAISE_REPLACE_લીટી_FAIL;
		    તોડો;
		}
	}
	નહીંતર
	    i = 0;

	// Now we may need to insert the remaining new old_len. If we do, we
	// must free the strings as we finish with them (we can't pass the
	// responsibility to Vim in this case).
	જો (!PyErr_Occurred())
	{
	    જ્યારે (i < new_len)
	    {
		જો (ml_append((linenr_T)(lo + i - 1),
					(char_u *)array[i], 0, FALSE) == FAIL)
		{
		    RAISE_ઉમેરો_લીટી_FAIL;
		    તોડો;
		}
		vim_free(array[i]);
		++i;
		++extra;
	    }
	}

	// Free any left-over old_len, as a result of an error
	જ્યારે (i < new_len)
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
	જો (save_curbuf.br_buf == ખાલી)
	{
	    mark_adjust((linenr_T)lo, (linenr_T)(hi - 1),
						  (long)MAXLNUM, (long)extra);
	    changed_લીટીઓ((linenr_T)lo, 0, (linenr_T)hi, (long)extra);
	}

	જો (buf == curbuf && (switchwin.sw_curwin != ખાલી
					   || save_curbuf.br_buf == ખાલી))
	    // Using an existing window for the buffer, adjust the cursor
	    // position.
	    py_fix_cursor((linenr_T)lo, (linenr_T)hi, (linenr_T)extra);

	// END of region without "return".
	restore_win_for_buf(&switchwin, &save_curbuf);

	જો (VimTryEnd())
	    પાછા FAIL;

	જો (len_change)
	    *len_change = new_len - old_len;

	પાછા OK;
    }
    નહીંતર
    {
	PyErr_BadArgument();
	પાછા FAIL;
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
    સ્થિર પૂર્ણાંક
ઉમેરો_Buffer_લીટીઓ(buf_T *buf, PyInt n, PyObject *લીટીઓ, PyInt *len_change)
{
    bufref_T	save_curbuf = {ખાલી, 0, 0};
    switchwin_T	switchwin;

    // First of all, we check the type of the supplied Python object.
    // It must be a string or a list, or the call is in error.
    જો (PyBytes_Check(લીટીઓ) || PyUnicode_Check(લીટીઓ))
    {
	અક્ષર		*str = String_To_લીટી(લીટીઓ);

	જો (str == ખાલી)
	    પાછા FAIL;

	PyErr_Clear();
	VimTryStart();
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	જો (u_save((linenr_T)n, (linenr_T)(n + 1)) == FAIL)
	    RAISE_UNDO_FAIL;
	નહીંતર જો (ml_append((linenr_T)n, (char_u *)str, 0, FALSE) == FAIL)
	    RAISE_ઉમેરો_લીટી_FAIL;
	નહીંતર જો (save_curbuf.br_buf == ખાલી)
	    // Only adjust marks if we managed to switch to a window that
	    // holds the buffer, otherwise line numbers will be invalid.
	    appended_લીટીઓ_mark((linenr_T)n, 1L);

	vim_free(str);
	restore_win_for_buf(&switchwin, &save_curbuf);
	update_screen(UPD_VALID);

	જો (VimTryEnd())
	    પાછા FAIL;

	જો (len_change)
	    *len_change = 1;

	પાછા OK;
    }
    નહીંતર જો (PyList_Check(લીટીઓ))
    {
	PyInt	i;
	PyInt	size = PyList_Size(લીટીઓ);
	અક્ષર	**array;

	array = PyMem_New(અક્ષર *, size);
	જો (array == ખાલી)
	{
	    PyErr_NoMemory();
	    પાછા FAIL;
	}

	માટે (i = 0; i < size; ++i)
	{
	    PyObject *લીટી;

	    જો (!(લીટી = PyList_GetItem(લીટીઓ, i)) ||
		!(array[i] = String_To_લીટી(લીટી)))
	    {
		જ્યારે (i)
		    vim_free(array[--i]);
		PyMem_Free(array);
		પાછા FAIL;
	    }
	}

	PyErr_Clear();
	VimTryStart();
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	જો (u_save((linenr_T)n, (linenr_T)(n + 1)) == FAIL)
	    RAISE_UNDO_FAIL;
	નહીંતર
	{
	    માટે (i = 0; i < size; ++i)
	    {
		જો (ml_append((linenr_T)(n + i),
					(char_u *)array[i], 0, FALSE) == FAIL)
		{
		    RAISE_ઉમેરો_લીટી_FAIL;

		    // Free the rest of the lines
		    જ્યારે (i < size)
			vim_free(array[i++]);

		    તોડો;
		}
		vim_free(array[i]);
	    }
	    જો (i > 0 && save_curbuf.br_buf == ખાલી)
		// Only adjust marks if we managed to switch to a window that
		// holds the buffer, otherwise line numbers will be invalid.
		appended_લીટીઓ_mark((linenr_T)n, (long)i);
	}

	// Free the array of lines. All of its contents have now
	// been freed.
	PyMem_Free(array);
	restore_win_for_buf(&switchwin, &save_curbuf);

	update_screen(UPD_VALID);

	જો (VimTryEnd())
	    પાછા FAIL;

	જો (len_change)
	    *len_change = size;

	પાછા OK;
    }
    નહીંતર
    {
	PyErr_BadArgument();
	પાછા FAIL;
    }
}

/*
 * Common routines for buffers and line ranges
 * -------------------------------------------
 */

પ્રકારવ્યાખ્યા સંરચના
{
    PyObject_HEAD
    buf_T *buf;
} BufferObject;

    સ્થિર પૂર્ણાંક
CheckBuffer(BufferObject *self)
{
    જો (self->buf == INVALID_BUFFER_કિંમત)
    {
	PyErr_SET_VIM(N_("attempt to refer to deleted buffer"));
	પાછા -1;
    }

    પાછા 0;
}

    સ્થિર પૂર્ણાંક
CheckBufferCb(શૂન્ય *self)
{
    પાછા CheckBuffer((BufferObject*)self);
}

    સ્થિર PyObject *
RBItem(BufferObject *self, PyInt n, PyInt start, PyInt end)
{
    જો (CheckBuffer(self))
	પાછા ખાલી;

    જો (end == -1)
	end = self->buf->b_ml.ml_લીટી_ગણતરી;

    જો (n < 0)
	n += end - start + 1;

    જો (n < 0 || n > end - start)
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("line number out of range"));
	પાછા ખાલી;
    }

    પાછા Get_Buffer_લીટી(self->buf, n+start);
}

    સ્થિર PyObject *
RBSlice(BufferObject *self, PyInt lo, PyInt hi, PyInt start, PyInt end)
{
    PyInt size;

    જો (CheckBuffer(self))
	પાછા ખાલી;

    જો (end == -1)
	end = self->buf->b_ml.ml_લીટી_ગણતરી;

    size = end - start + 1;

    જો (lo < 0)
	lo = 0;
    નહીંતર જો (lo > size)
	lo = size;
    જો (hi < 0)
	hi = 0;
    જો (hi < lo)
	hi = lo;
    નહીંતર જો (hi > size)
	hi = size;

    પાછા Get_Buffer_લીટી_List(self->buf, lo+start, hi+start);
}

    સ્થિર PyInt
RBAsItem(
	BufferObject *self,
	PyInt n,
	PyObject *valObject,
	PyInt start,
	PyInt end,
	PyInt *new_end)
{
    PyInt len_change;

    જો (CheckBuffer(self))
	પાછા -1;

    જો (end == -1)
	end = self->buf->b_ml.ml_લીટી_ગણતરી;

    જો (n < 0)
	n += end - start + 1;

    જો (n < 0 || n > end - start)
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("line number out of range"));
	પાછા -1;
    }

    જો (Set_Buffer_લીટી(self->buf, n+start, valObject, &len_change) == FAIL)
	પાછા -1;

    જો (new_end)
	*new_end = end + len_change;

    પાછા 0;
}

    સ્થિર PyInt
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
    જો (CheckBuffer(self))
	પાછા -1;

    જો (end == -1)
	end = self->buf->b_ml.ml_લીટી_ગણતરી;

    // Sort out the slice range
    size = end - start + 1;

    જો (lo < 0)
	lo = 0;
    નહીંતર જો (lo > size)
	lo = size;
    જો (hi < 0)
	hi = 0;
    જો (hi < lo)
	hi = lo;
    નહીંતર જો (hi > size)
	hi = size;

    જો (Set_Buffer_લીટી_List(self->buf, lo + start, hi + start,
						valObject, &len_change) == FAIL)
	પાછા -1;

    જો (new_end)
	*new_end = end + len_change;

    પાછા 0;
}


    સ્થિર PyObject *
RBAppend(
	BufferObject *self,
	PyObject *args,
	PyInt start,
	PyInt end,
	PyInt *new_end)
{
    PyObject *લીટીઓ;
    PyInt len_change;
    PyInt max;
    PyInt n;

    જો (CheckBuffer(self))
	પાછા ખાલી;

    જો (end == -1)
	end = self->buf->b_ml.ml_લીટી_ગણતરી;

    max = n = end - start + 1;

    જો (!PyArg_ParseTuple(args, "O|n", &લીટીઓ, &n))
	પાછા ખાલી;

    જો (n < 0 || n > max)
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("line number out of range"));
	પાછા ખાલી;
    }

    જો (ઉમેરો_Buffer_લીટીઓ(self->buf, n + start - 1, લીટીઓ, &len_change) == FAIL)
	પાછા ખાલી;

    જો (new_end)
	*new_end = end + len_change;

    Py_INCREF(Py_None);
    પાછા Py_None;
}

// Range object

DEFINE_PY_TYPE_OBJECT(RangeType);
સ્થિર PySequenceMethods RangeAsSeq;
સ્થિર PyMappingMethods RangeAsMapping;

પ્રકારવ્યાખ્યા સંરચના
{
    PyObject_HEAD
    BufferObject *buf;
    PyInt start;
    PyInt end;
} RangeObject;

    સ્થિર PyObject *
RangeNew(buf_T *buf, PyInt start, PyInt end)
{
    BufferObject *bufr;
    RangeObject *self;
    self = PyObject_GC_New(RangeObject, RangeTypePtr);
    જો (self == ખાલી)
	પાછા ખાલી;

    bufr = (BufferObject *)BufferNew(buf);
    જો (bufr == ખાલી)
    {
	Py_DECREF(self);
	પાછા ખાલી;
    }
    Py_INCREF((PyObject *)bufr);

    self->buf = bufr;
    self->start = start;
    self->end = end;

    પાછા (PyObject *)(self);
}

    સ્થિર શૂન્ય
RangeDestructor(PyObject *self_obj)
{
    RangeObject *self = (RangeObject*)self_obj;
    PyObject_GC_UnTrack((શૂન્ય *)(self));
    Py_XDECREF(((PyObject *)(self->buf)));
    PyObject_GC_Del((શૂન્ય *)(self));
}

    સ્થિર પૂર્ણાંક
RangeTraverse(PyObject *self_obj, visitproc visit, શૂન્ય *arg)
{
    RangeObject *self = (RangeObject*)self_obj;
    Py_VISIT(((PyObject *)(self->buf)));
    પાછા 0;
}

    સ્થિર પૂર્ણાંક
RangeClear(PyObject *self_obj)
{
    RangeObject *self = (RangeObject*)self_obj;
    Py_CLEAR(self->buf);
    પાછા 0;
}

    સ્થિર PyInt
RangeLength(RangeObject *self)
{
    // HOW DO WE SIGNAL AN ERROR FROM THIS FUNCTION?
    જો (CheckBuffer(self->buf))
	પાછા -1; // ???

    પાછા (self->end - self->start + 1);
}

    સ્થિર PyObject *
RangeItem(RangeObject *self, PyInt n)
{
    પાછા RBItem(self->buf, n, self->start, self->end);
}

    સ્થિર PyObject *
RangeSlice(RangeObject *self, PyInt lo, PyInt hi)
{
    પાછા RBSlice(self->buf, lo, hi, self->start, self->end);
}

સ્થિર અક્ષર *RangeAttrs[] = {
    "start", "end",
    ખાલી
};

    સ્થિર PyObject *
RangeDir(PyObject *self, PyObject *args UNUSED)
{
    પાછા ObjectDir(self, RangeAttrs);
}

    સ્થિર PyObject *
RangeAppend(RangeObject *self, PyObject *args)
{
    પાછા RBAppend(self->buf, args, self->start, self->end, &self->end);
}

    સ્થિર PyObject *
RangeRepr(PyObject *self_obj)
{
    RangeObject *self = (RangeObject*)self_obj;
    જો (self->buf->buf == INVALID_BUFFER_કિંમત)
	પાછા PyString_FromFormat("<range object (for deleted buffer) at %p>",
				    (શૂન્ય *)self);
    નહીંતર
    {
	અક્ષર *name = (અક્ષર *)self->buf->buf->b_fname;

	જો (name == ખાલી)
	    name = "";

	પાછા PyString_FromFormat("<range %s (%d:%d)>",
				    name, (પૂર્ણાંક)self->start, (પૂર્ણાંક)self->end);
    }
}

સ્થિર સંરચના PyMethodDef RangeMethods[] = {
    // name,	function,			calling,	documentation
    {"append",	(PyCFunction)RangeAppend,	METH_VARARGS,	"Append data to the Vim range" },
    {"__dir__",	(PyCFunction)RangeDir,		METH_NOARGS,	""},
    { ખાલી,	ખાલી,				0,		ખાલી}
};

DEFINE_PY_TYPE_OBJECT(BufferType);
સ્થિર PySequenceMethods BufferAsSeq;
સ્થિર PyMappingMethods BufferAsMapping;

    સ્થિર PyObject *
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

    જો (BUF_PYTHON_REF(buf) != ખાલી)
    {
	self = BUF_PYTHON_REF(buf);
	Py_INCREF((PyObject *)self);
    }
    નહીંતર
    {
	self = PyObject_NEW(BufferObject, BufferTypePtr);
	જો (self == ખાલી)
	    પાછા ખાલી;
	self->buf = buf;
	BUF_PYTHON_REF(buf) = self;
    }

    પાછા (PyObject *)(self);
}

    સ્થિર શૂન્ય
BufferDestructor(PyObject *self_obj)
{
    BufferObject *self = (BufferObject*)self_obj;
    જો (self->buf && self->buf != INVALID_BUFFER_કિંમત)
	BUF_PYTHON_REF(self->buf) = ખાલી;

    DESTRUCTOR_FINISH(self);
}

    સ્થિર PyInt
BufferLength(BufferObject *self)
{
    // HOW DO WE SIGNAL AN ERROR FROM THIS FUNCTION?
    જો (CheckBuffer(self))
	પાછા -1; // ???

    પાછા (PyInt)(self->buf->b_ml.ml_લીટી_ગણતરી);
}

    સ્થિર PyObject *
BufferItem(BufferObject *self, PyInt n)
{
    પાછા RBItem(self, n, 1, -1);
}

    સ્થિર PyObject *
BufferSlice(BufferObject *self, PyInt lo, PyInt hi)
{
    પાછા RBSlice(self, lo, hi, 1, -1);
}

સ્થિર અક્ષર *BufferAttrs[] = {
    "name", "number", "vars", "options", "valid",
    ખાલી
};

    સ્થિર PyObject *
BufferDir(PyObject *self, PyObject *args UNUSED)
{
    પાછા ObjectDir(self, BufferAttrs);
}

    સ્થિર PyObject *
BufferAttrValid(BufferObject *self, અક્ષર *name)
{
    PyObject	*ret;

    જો (strcmp(name, "valid") != 0)
	પાછા ખાલી;

    ret = ((self->buf == INVALID_BUFFER_કિંમત) ? Py_False : Py_True);
    Py_INCREF(ret);
    પાછા ret;
}

    સ્થિર PyObject *
BufferAttr(BufferObject *self, અક્ષર *name)
{
    જો (strcmp(name, "name") == 0)
	પાછા PyString_FromString((self->buf->b_ffname == ખાલી
				    ? "" : (અક્ષર *)self->buf->b_ffname));
    નહીંતર જો (strcmp(name, "number") == 0)
	પાછા Py_Build_કિંમત(Py_ssize_t_fmt, self->buf->b_fnum);
    નહીંતર જો (strcmp(name, "vars") == 0)
	પાછા NEW_DICTIONARY(self->buf->b_vars);
    નહીંતર જો (strcmp(name, "options") == 0)
	પાછા OptionsNew(SREQ_BUF, self->buf, CheckBufferCb,
			(PyObject *) self);
    નહીંતર જો (strcmp(name, "__members__") == 0)
	પાછા ObjectDir(ખાલી, BufferAttrs);
    નહીંતર
	પાછા ખાલી;
}

    સ્થિર પૂર્ણાંક
BufferSetattr(PyObject *self_obj, અક્ષર *name, PyObject *valObject)
{
    BufferObject *self = (BufferObject*)self_obj;
    જો (CheckBuffer(self))
	પાછા -1;

    જો (strcmp(name, "name") == 0)
    {
	char_u		*val;
	aco_save_T	aco;
	પૂર્ણાંક		ren_ret = OK;
	PyObject	*todecref;

	જો (!(val = StringToChars(valObject, &todecref)))
	    પાછા -1;

	VimTryStart();
	// Using aucmd_*: autocommands will be executed by rename_buffer
	aucmd_prepbuf(&aco, self->buf);
	જો (curbuf == self->buf)
	{
	    ren_ret = rename_buffer(val);
	    aucmd_restbuf(&aco);
	}
	Py_XDECREF(todecref);
	જો (VimTryEnd())
	    પાછા -1;

	જો (ren_ret == FAIL)
	{
	    PyErr_SET_VIM(N_("failed to rename buffer"));
	    પાછા -1;
	}
	પાછા 0;
    }
    નહીંતર
    {
	PyErr_SetString(PyExc_AttributeError, name);
	પાછા -1;
    }
}

    સ્થિર PyObject *
BufferAppend(BufferObject *self, PyObject *args)
{
    પાછા RBAppend(self, args, 1, -1, ખાલી);
}

    સ્થિર PyObject *
BufferMark(BufferObject *self, PyObject *pmarkObject)
{
    pos_T	*posp;
    char_u	*pmark;
    char_u	mark;
    bufref_T	savebuf;
    PyObject	*todecref;

    જો (CheckBuffer(self))
	પાછા ખાલી;

    જો (!(pmark = StringToChars(pmarkObject, &todecref)))
	પાછા ખાલી;

    જો (pmark[0] == '\0' || pmark[1] != '\0')
    {
	PyErr_SET_STRING(Py_Exc_કિંમત_Error,
		N_("mark name must be a single character"));
	Py_XDECREF(todecref);
	પાછા ખાલી;
    }

    mark = *pmark;

    Py_XDECREF(todecref);

    VimTryStart();
    switch_buffer(&savebuf, self->buf);
    posp = getmark(mark, FALSE);
    restore_buffer(&savebuf);
    જો (VimTryEnd())
	પાછા ખાલી;

    જો (posp == ખાલી)
    {
	PyErr_SET_VIM(N_("invalid mark name"));
	પાછા ખાલી;
    }

    જો (posp->lnum <= 0)
    {
	// Or raise an error?
	Py_INCREF(Py_None);
	પાછા Py_None;
    }

    પાછા Py_Build_કિંમત("(ll)", (long)(posp->lnum), (long)(posp->col));
}

    સ્થિર PyObject *
BufferRange(BufferObject *self, PyObject *args)
{
    PyInt start;
    PyInt end;

    જો (CheckBuffer(self))
	પાછા ખાલી;

    જો (!PyArg_ParseTuple(args, "nn", &start, &end))
	પાછા ખાલી;

    પાછા RangeNew(self->buf, start, end);
}

    સ્થિર PyObject *
BufferRepr(PyObject *self_obj)
{
    BufferObject *self = (BufferObject*)self_obj;
    જો (self->buf == INVALID_BUFFER_કિંમત)
	પાછા PyString_FromFormat("<buffer object (deleted) at %p>", (શૂન્ય *)self);
    નહીંતર
    {
	અક્ષર	*name = (અક્ષર *)self->buf->b_fname;

	જો (name == ખાલી)
	    name = "";

	પાછા PyString_FromFormat("<buffer %s>", name);
    }
}

સ્થિર સંરચના PyMethodDef BufferMethods[] = {
    // name,	    function,			calling,	documentation
    {"append",	    (PyCFunction)BufferAppend,	METH_VARARGS,	"Append data to Vim buffer" },
    {"mark",	    (PyCFunction)BufferMark,	METH_O,		"Return (row,col) representing position of named mark" },
    {"range",	    (PyCFunction)BufferRange,	METH_VARARGS,	"Return a range object which represents the part of the given buffer between line numbers s and e" },
    {"__dir__",	    (PyCFunction)BufferDir,	METH_NOARGS,	""},
    { ખાલી,	    ખાલી,			0,		ખાલી}
};

/*
 * Buffer list object - Implementation
 */

DEFINE_PY_TYPE_OBJECT(BufMapType);

પ્રકારવ્યાખ્યા સંરચના
{
    PyObject_HEAD
} BufMapObject;

સ્થિર BufMapObject TheBufferMap =
{
    PyObject_HEAD_INIT_TYPE(BufMapType)
};

    સ્થિર PyInt
BufMapLength(PyObject *self UNUSED)
{
    buf_T	*b = firstbuf;
    PyInt	n = 0;

    જ્યારે (b)
    {
	++n;
	b = b->b_next;
    }

    પાછા n;
}

    સ્થિર PyObject *
BufMapItem(PyObject *self UNUSED, PyObject *ચાવી_Object)
{
    buf_T	*b;
    long	bnr;

    જો (સંખ્યા_To_Long(ચાવી_Object, &bnr, સંખ્યા_INT|સંખ્યા_NATURAL))
	પાછા ખાલી;

    b = buflist_findnr((પૂર્ણાંક) bnr);

    જો (b)
	પાછા BufferNew(b);
    નહીંતર
    {
	PyErr_SetObject(Py_Exc_ચાવી_Error, ચાવી_Object);
	પાછા ખાલી;
    }
}

    સ્થિર શૂન્ય
BufMapIterDestruct(શૂન્ય* arg)
{
    PyObject *buffer = (PyObject*)arg;
    // Iteration was stopped before all buffers were processed
    જો (buffer)
    {
	Py_DECREF(buffer);
    }
}

    સ્થિર પૂર્ણાંક
BufMapIterTraverse(શૂન્ય *iter, visitproc visit, શૂન્ય *arg)
{
    PyObject *buffer = (PyObject*)iter;
    જો (buffer)
	Py_VISIT(buffer);
    પાછા 0;
}

    સ્થિર પૂર્ણાંક
BufMapIterClear(શૂન્ય **iter)
{
    PyObject **buffer = (PyObject**)iter;
    જો (*buffer)
	Py_CLEAR(*buffer);
    પાછા 0;
}

    સ્થિર PyObject *
BufMapIterNext(શૂન્ય **arg)
{
    PyObject	*next;
    PyObject	*ret;
    PyObject	**buffer = (PyObject**)arg;

    જો (!*buffer)
	પાછા ખાલી;

    ret = *buffer;

    જો (CheckBuffer((BufferObject *)(ret)))
    {
	*buffer = ખાલી;
	પાછા ખાલી;
    }

    જો (!((BufferObject *)(ret))->buf->b_next)
	next = ખાલી;
    નહીંતર જો (!(next = BufferNew(((BufferObject *)(ret))->buf->b_next)))
	પાછા ખાલી;
    *buffer = next;
    // Do not increment reference: we no longer hold it (decref), but whoever
    // on other side will hold (incref). Decref+incref = nothing.
    પાછા ret;
}

    સ્થિર PyObject *
BufMapIter(PyObject *self)
{
    PyObject *buffer;

    buffer = BufferNew(firstbuf);
    પાછા IterNew(buffer,
	    BufMapIterDestruct, BufMapIterNext,
	    BufMapIterTraverse, BufMapIterClear,
	    self);
}

સ્થિર PyMappingMethods BufMapAsMapping = {
    (lenfunc)       BufMapLength,
    (binaryfunc)    BufMapItem,
    (objobjargproc) 0,
};

// Current items object

સ્થિર અક્ષર *CurrentAttrs[] = {
    "buffer", "window", "line", "range", "tabpage",
    ખાલી
};

    સ્થિર PyObject *
CurrentDir(PyObject *self, PyObject *args UNUSED)
{
    પાછા ObjectDir(self, CurrentAttrs);
}

    સ્થિર PyObject *
CurrentGetattr(PyObject *self UNUSED, અક્ષર *name)
{
    જો (strcmp(name, "buffer") == 0)
	પાછા (PyObject *)BufferNew(curbuf);
    નહીંતર જો (strcmp(name, "window") == 0)
	પાછા (PyObject *)WindowNew(curwin, curtab);
    નહીંતર જો (strcmp(name, "tabpage") == 0)
	પાછા (PyObject *)TabPageNew(curtab);
    નહીંતર જો (strcmp(name, "line") == 0)
	પાછા Get_Buffer_લીટી(curbuf, (PyInt)curwin->w_cursor.lnum);
    નહીંતર જો (strcmp(name, "range") == 0)
	પાછા RangeNew(curbuf, RangeStart, RangeEnd);
    નહીંતર જો (strcmp(name, "__members__") == 0)
	પાછા ObjectDir(ખાલી, CurrentAttrs);
    નહીંતર
#જો PY_MAJOR_VERSION < 3
	પાછા Py_FindMethod(WindowMethods, self, name);
#નહીંતર
	પાછા ખાલી;
#endif
}

    સ્થિર પૂર્ણાંક
CurrentSetattr(PyObject *self UNUSED, અક્ષર *name, PyObject *valObject)
{
    જો (strcmp(name, "line") == 0)
    {
	જો (Set_Buffer_લીટી(curbuf, (PyInt)curwin->w_cursor.lnum, valObject,
			  ખાલી) == FAIL)
	    પાછા -1;

	પાછા 0;
    }
    નહીંતર જો (strcmp(name, "buffer") == 0)
    {
	પૂર્ણાંક ગણતરી;

	જો (valObject->ob_type != BufferTypePtr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.Buffer object, but got %s"),
		    valObject);
	    પાછા -1;
	}

	જો (CheckBuffer((BufferObject *)(valObject)))
	    પાછા -1;
	ગણતરી = ((BufferObject *)(valObject))->buf->b_fnum;

	VimTryStart();
	જો (do_buffer(DOBUF_GOTO, DOBUF_FIRST, FORWARD, ગણતરી, 0) == FAIL)
	{
	    જો (VimTryEnd())
		પાછા -1;
	    PyErr_VIM_FORMAT(N_("failed to switch to buffer %d"), ગણતરી);
	    પાછા -1;
	}

	પાછા VimTryEnd();
    }
    નહીંતર જો (strcmp(name, "window") == 0)
    {
	પૂર્ણાંક ગણતરી;

	જો (valObject->ob_type != WindowTypePtr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.Window object, but got %s"),
		    valObject);
	    પાછા -1;
	}

	જો (CheckWindow((WindowObject *)(valObject)))
	    પાછા -1;
	ગણતરી = get_win_સંખ્યા(((WindowObject *)(valObject))->win, firstwin);

	જો (!ગણતરી)
	{
	    PyErr_SET_STRING(Py_Exc_કિંમત_Error,
		    N_("failed to find window in the current tab page"));
	    પાછા -1;
	}

	VimTryStart();
	win_goto(((WindowObject *)(valObject))->win);
	જો (((WindowObject *)(valObject))->win != curwin)
	{
	    જો (VimTryEnd())
		પાછા -1;
	    PyErr_SET_STRING(PyExc_RuntimeError,
		    N_("did not switch to the specified window"));
	    પાછા -1;
	}

	પાછા VimTryEnd();
    }
    નહીંતર જો (strcmp(name, "tabpage") == 0)
    {
	જો (valObject->ob_type != TabPageTypePtr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.TabPage object, but got %s"),
		    valObject);
	    પાછા -1;
	}

	જો (CheckTabPage((TabPageObject *)(valObject)))
	    પાછા -1;

	VimTryStart();
	goto_tabpage_tp(((TabPageObject *)(valObject))->tab, TRUE, TRUE);
	જો (((TabPageObject *)(valObject))->tab != curtab)
	{
	    જો (VimTryEnd())
		પાછા -1;
	    PyErr_SET_STRING(PyExc_RuntimeError,
		    N_("did not switch to the specified tab page"));
	    પાછા -1;
	}

	પાછા VimTryEnd();
    }
    નહીંતર
    {
	PyErr_SetString(PyExc_AttributeError, name);
	પાછા -1;
    }
}

સ્થિર સંરચના PyMethodDef CurrentMethods[] = {
    // name,	    function,			calling,	documentation
    {"__dir__",	    (PyCFunction)CurrentDir,	METH_NOARGS,	""},
    { ખાલી,	    ખાલી,			0,		ખાલી}
};

    સ્થિર શૂન્ય
init_range_cmd(શૂન્ય *arg)
{
    exarg_T *eap = (exarg_T*)arg;
    RangeStart = eap->line1;
    RangeEnd = eap->line2;
}

    સ્થિર શૂન્ય
init_range_eval(શૂન્ય *rettv UNUSED)
{
    RangeStart = (PyInt) curwin->w_cursor.lnum;
    RangeEnd = RangeStart;
}

    સ્થિર શૂન્ય
run_cmd(અચલ અક્ષર *cmd, dict_T* locals UNUSED, શૂન્ય *arg UNUSED
#ifdef PY_CAN_RECURSE
	, PyGILState_STATE *pygilstate UNUSED
#endif
	)
{
    PyObject	*run_ret;
    run_ret = PyRun_String((અક્ષર *)cmd, Py_file_આગત, globals, globals);
    જો (run_ret != ખાલી)
    {
	Py_DECREF(run_ret);
    }
    નહીંતર જો (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
    {
	emsg(_(e_cant_handle_systemexit_of_python_exception_in_vim));
	PyErr_Clear();
    }
    નહીંતર
	PyErr_PrintEx(1);
}

સ્થિર અચલ અક્ષર	*code_hdr = "def " DOPY_FUNC "(line, linenr):\n ";
સ્થિર પૂર્ણાંક		code_hdr_len = 30;

    સ્થિર શૂન્ય
run_do(અચલ અક્ષર *cmd, dict_T* locals UNUSED, શૂન્ય *arg UNUSED
#ifdef PY_CAN_RECURSE
	, PyGILState_STATE *pygilstate
#endif
	)
{
    PyInt	lnum;
    size_t	len;
    અક્ષર	*code;
    પૂર્ણાંક		status;
    PyObject	*pyfunc, *pymain;
    PyObject	*run_ret;
    buf_T	*was_curbuf = curbuf;

    જો (u_save((linenr_T)RangeStart - 1, (linenr_T)RangeEnd + 1) != OK)
    {
	emsg(_("cannot save undo information"));
	પાછા;
    }

    len = code_hdr_len + STRLEN(cmd);
    code = PyMem_New(અક્ષર, len + 1);
    memcpy(code, code_hdr, code_hdr_len);
    STRCPY(code + code_hdr_len, cmd);
    run_ret = PyRun_String(code, Py_file_આગત, globals, globals);
    status = -1;
    જો (run_ret != ખાલી)
    {
	status = 0;
	Py_DECREF(run_ret);
    }
    નહીંતર જો (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
    {
	PyMem_Free(code);
	emsg(_(e_cant_handle_systemexit_of_python_exception_in_vim));
	PyErr_Clear();
	પાછા;
    }
    નહીંતર
	PyErr_PrintEx(1);

    PyMem_Free(code);

    જો (status)
    {
	emsg(_("failed to run the code"));
	પાછા;
    }

    status = 0;
    pymain = PyImport_AddModule("__main__");
    pyfunc = PyObject_GetAttrString(pymain, DOPY_FUNC);
#ifdef PY_CAN_RECURSE
    PyGILState_Release(*pygilstate);
#endif

    માટે (lnum = RangeStart; lnum <= RangeEnd; ++lnum)
    {
	PyObject	*લીટી;
	PyObject	*linenr;
	PyObject	*ret;

#ifdef PY_CAN_RECURSE
	*pygilstate = PyGILState_Ensure();
#endif
	// Check the line number, the command my have deleted lines.
	જો (lnum > curbuf->b_ml.ml_લીટી_ગણતરી
		|| !(લીટી = Get_Buffer_લીટી(curbuf, lnum)))
	    જાઓ err;
	જો (!(linenr = PyInt_FromLong((long) lnum)))
	{
	    Py_DECREF(લીટી);
	    જાઓ err;
	}
	ret = PyObject_CallFunctionObjArgs(pyfunc, લીટી, linenr, ખાલી);
	Py_DECREF(લીટી);
	Py_DECREF(linenr);
	જો (!ret)
	    જાઓ err;

	// Check that the command didn't switch to another buffer.
	// Check the line number, the command my have deleted lines.
	જો (curbuf != was_curbuf || lnum > curbuf->b_ml.ml_લીટી_ગણતરી)
	{
	    Py_XDECREF(ret);
	    જાઓ err;
	}

	જો (ret != Py_None)
	    જો (Set_Buffer_લીટી(curbuf, lnum, ret, ખાલી) == FAIL)
	    {
		Py_XDECREF(ret);
		જાઓ err;
	    }

	Py_XDECREF(ret);
	PythonIO_Flush();
#ifdef PY_CAN_RECURSE
	PyGILState_Release(*pygilstate);
#endif
    }
    જાઓ out;
err:
#ifdef PY_CAN_RECURSE
    *pygilstate = PyGILState_Ensure();
#endif
    PyErr_PrintEx(0);
    PythonIO_Flush();
    status = 1;
out:
#ifdef PY_CAN_RECURSE
    જો (!status)
	*pygilstate = PyGILState_Ensure();
#endif
    Py_DECREF(pyfunc);
    PyObject_SetAttrString(pymain, DOPY_FUNC, ખાલી);
    જો (status)
	પાછા;
    check_cursor();
    update_curbuf(UPD_NOT_VALID);
}

    સ્થિર શૂન્ય
run_eval(અચલ અક્ષર *cmd, dict_T *locals, શૂન્ય *arg
#ifdef PY_CAN_RECURSE
	, PyGILState_STATE *pygilstate UNUSED
#endif
	)
{
    PyObject	*run_ret;
    typval_T	*rettv = (typval_T*)arg;
    PyObject	*pylocals = locals ? NEW_DICTIONARY(locals) : globals;

    run_ret = PyRun_String((અક્ષર *)cmd, Py_eval_આગત, globals, pylocals);
    જો (run_ret == ખાલી)
    {
	જો (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
	{
	    emsg(_(e_cant_handle_systemexit_of_python_exception_in_vim));
	    PyErr_Clear();
	}
	નહીંતર
	{
	    જો (PyErr_Occurred() && !સંદેશ_silent)
		PyErr_PrintEx(0);
	    emsg(_(e_eval_did_not_return_valid_python_object));
	}
    }
    નહીંતર
    {
	જો (ConvertFromPyObject(run_ret, rettv) == -1)
	    emsg(_(e_failed_to_convert_returned_python_object_to_vim_કિંમત));
	Py_DECREF(run_ret);
    }
    PyErr_Clear();
}

    સ્થિર પૂર્ણાંક
set_ref_in_py(અચલ પૂર્ણાંક copyID)
{
    pylinkedlist_T	*cur;
    list_T		*ll;
    tuple_T		*tt;
    પૂર્ણાંક			i;
    પૂર્ણાંક			abort = FALSE;
    FunctionObject	*func;

    જો (lastdict != ખાલી)
    {
	માટે (cur = lastdict ; !abort && cur != ખાલી ; cur = cur->pll_prev)
	    abort = set_ref_in_dict(((DictionaryObject *)(cur->pll_obj))->dict,
								       copyID);
    }

    જો (lastlist != ખાલી)
    {
	માટે (cur = lastlist ; !abort && cur != ખાલી ; cur = cur->pll_prev)
	{
	    ll = ((ListObject *) (cur->pll_obj))->list;
	    abort = set_ref_in_list(ll, copyID);
	}
    }

    જો (lasttuple != ખાલી)
    {
	માટે (cur = lasttuple ; !abort && cur != ખાલી ; cur = cur->pll_prev)
	{
	    tt = ((TupleObject *) (cur->pll_obj))->tuple;
	    abort = set_ref_in_tuple(tt, copyID);
	}
    }

    જો (lastfunc != ખાલી)
    {
	માટે (cur = lastfunc ; !abort && cur != ખાલી ; cur = cur->pll_prev)
	{
	    func = (FunctionObject *) cur->pll_obj;
	    abort = set_ref_in_dict(func->self, copyID);
	    જો (func->argc)
		માટે (i = 0; !abort && i < func->argc; ++i)
		    abort = abort
			|| set_ref_in_item(&func->argv[i], copyID, ખાલી, ખાલી, ખાલી);
	}
    }

    પાછા abort;
}

    સ્થિર પૂર્ણાંક
set_string_copy(char_u *str, typval_T *tv)
{
    tv->vval.v_string = vim_strsave(str);
    જો (tv->vval.v_string == ખાલી)
    {
	PyErr_NoMemory();
	પાછા -1;
    }
    પાછા 0;
}

    સ્થિર પૂર્ણાંક
pydict_to_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    dict_T	*dict;
    char_u	*ચાવી;
    dictitem_T	*di;
    PyObject	*ચાવી_Object;
    PyObject	*valObject;
    Py_ssize_t	iter = 0;

    જો (!(dict = py_dict_alloc()))
	પાછા -1;

    tv->v_type = VAR_DICT;
    tv->vval.v_dict = dict;

    જ્યારે (PyDict_Next(obj, &iter, &ચાવી_Object, &valObject))
    {
	PyObject	*todecref = ખાલી;

	જો (ચાવી_Object == ખાલી || valObject == ખાલી)
	{
	    dict_unref(dict);
	    પાછા -1;
	}

	જો (!(ચાવી = StringToChars(ચાવી_Object, &todecref)))
	{
	    dict_unref(dict);
	    પાછા -1;
	}

	જો (*ચાવી == NUL)
	{
	    dict_unref(dict);
	    Py_XDECREF(todecref);
	    RAISE_NO_EMPTY_ચાવીઓ;
	    પાછા -1;
	}

	di = dictitem_alloc(ચાવી);

	Py_XDECREF(todecref);

	જો (di == ખાલી)
	{
	    PyErr_NoMemory();
	    dict_unref(dict);
	    પાછા -1;
	}

	જો (_ConvertFromPyObject(valObject, &di->di_tv, lookup_dict) == -1)
	{
	    vim_free(di);
	    dict_unref(dict);
	    પાછા -1;
	}

	જો (dict_add(dict, di) == FAIL)
	{
	    RAISE_ચાવી_ADD_FAIL(di->di_ચાવી);
	    clear_tv(&di->di_tv);
	    vim_free(di);
	    dict_unref(dict);
	    પાછા -1;
	}
    }

    --dict->dv_refcount;
    પાછા 0;
}

    સ્થિર પૂર્ણાંક
pymap_to_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    dict_T	*dict;
    char_u	*ચાવી;
    dictitem_T	*di;
    PyObject	*list;
    PyObject	*iterator;
    PyObject	*ચાવી_Object;
    PyObject	*valObject;

    જો (!(dict = py_dict_alloc()))
	પાછા -1;

    tv->v_type = VAR_DICT;
    tv->vval.v_dict = dict;

    જો (!(list = Py_Mapping_ચાવીઓ(obj)))
    {
	dict_unref(dict);
	પાછા -1;
    }

    જો (!(iterator = PyObject_GetIter(list)))
    {
	dict_unref(dict);
	Py_DECREF(list);
	પાછા -1;
    }
    Py_DECREF(list);

    જ્યારે ((ચાવી_Object = PyIter_Next(iterator)))
    {
	PyObject	*todecref;

	જો (!(ચાવી = StringToChars(ચાવી_Object, &todecref)))
	{
	    Py_DECREF(ચાવી_Object);
	    Py_DECREF(iterator);
	    dict_unref(dict);
	    પાછા -1;
	}

	જો (*ચાવી == NUL)
	{
	    Py_DECREF(ચાવી_Object);
	    Py_DECREF(iterator);
	    Py_XDECREF(todecref);
	    dict_unref(dict);
	    RAISE_NO_EMPTY_ચાવીઓ;
	    પાછા -1;
	}

	જો (!(valObject = PyObject_GetItem(obj, ચાવી_Object)))
	{
	    Py_DECREF(ચાવી_Object);
	    Py_DECREF(iterator);
	    Py_XDECREF(todecref);
	    dict_unref(dict);
	    પાછા -1;
	}

	di = dictitem_alloc(ચાવી);

	Py_DECREF(ચાવી_Object);
	Py_XDECREF(todecref);

	જો (di == ખાલી)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(valObject);
	    dict_unref(dict);
	    PyErr_NoMemory();
	    પાછા -1;
	}

	જો (_ConvertFromPyObject(valObject, &di->di_tv, lookup_dict) == -1)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(valObject);
	    vim_free(di);
	    dict_unref(dict);
	    પાછા -1;
	}

	Py_DECREF(valObject);

	જો (dict_add(dict, di) == FAIL)
	{
	    RAISE_ચાવી_ADD_FAIL(di->di_ચાવી);
	    Py_DECREF(iterator);
	    dictitem_free(di);
	    dict_unref(dict);
	    પાછા -1;
	}
    }
    Py_DECREF(iterator);
    --dict->dv_refcount;
    પાછા 0;
}

    સ્થિર પૂર્ણાંક
pyseq_to_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    list_T	*l;

    જો (!(l = py_list_alloc()))
	પાછા -1;

    tv->v_type = VAR_LIST;
    tv->vval.v_list = l;

    જો (list_py_concat(l, obj, lookup_dict) == -1)
    {
	list_unref(l);
	પાછા -1;
    }

    --l->lv_refcount;
    પાછા 0;
}

    સ્થિર પૂર્ણાંક
pytuple_to_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    tuple_T	*t;

    જો (!(t = py_tuple_alloc()))
	પાછા -1;

    tv->v_type = VAR_TUPLE;
    tv->vval.v_tuple = t;

    જો (tuple_py_concat(t, obj, lookup_dict) == -1)
    {
	tuple_unref(t);
	પાછા -1;
    }

    --t->tv_refcount;
    પાછા 0;
}

પ્રકારવ્યાખ્યા પૂર્ણાંક (*pytotvfunc)(PyObject *, typval_T *, PyObject *);

    સ્થિર પૂર્ણાંક
convert_dl(PyObject *obj, typval_T *tv,
				    pytotvfunc py_to_tv, PyObject *lookup_dict)
{
    PyObject	*capsule;
    અક્ષર	hexBuf[કદ(શૂન્ય *) * 2 + 3];

    sprintf(hexBuf, "%p", (શૂન્ય *)obj);

#ifdef PY_USE_CAPSULE
    capsule = PyDict_GetItemString(lookup_dict, hexBuf);
#નહીંતર
    capsule = (PyObject *)PyDict_GetItemString(lookup_dict, hexBuf);
#endif
    જો (capsule == ખાલી)
    {
#ifdef PY_USE_CAPSULE
	capsule = PyCapsule_New(tv, ખાલી, ખાલી);
#નહીંતર
	capsule = PyCObject_FromVoidPtr(tv, ખાલી);
#endif
	જો (PyDict_SetItemString(lookup_dict, hexBuf, capsule))
	{
	    Py_DECREF(capsule);
	    tv->v_type = VAR_UNKNOWN;
	    પાછા -1;
	}

	Py_DECREF(capsule);

	જો (py_to_tv(obj, tv, lookup_dict) == -1)
	{
	    tv->v_type = VAR_UNKNOWN;
	    પાછા -1;
	}
	// As we are not using copy_tv which increments reference count we must
	// do it ourself.
	જો (tv->v_type == VAR_DICT)
	    ++tv->vval.v_dict->dv_refcount;
	નહીંતર જો (tv->v_type == VAR_LIST)
	    ++tv->vval.v_list->lv_refcount;
	નહીંતર જો (tv->v_type == VAR_TUPLE)
	    ++tv->vval.v_tuple->tv_refcount;
    }
    નહીંતર
    {
	typval_T	*v;

#ifdef PY_USE_CAPSULE
	v = PyCapsule_GetPointer(capsule, ખાલી);
#નહીંતર
	v = PyCObject_AsVoidPtr(capsule);
#endif
	copy_tv(v, tv);
    }
    પાછા 0;
}

    સ્થિર પૂર્ણાંક
ConvertFromPyMapping(PyObject *obj, typval_T *tv)
{
    PyObject	*lookup_dict;
    પૂર્ણાંક		ret;

    જો (!(lookup_dict = PyDict_New()))
	પાછા -1;

    જો (PyType_IsSubtype(obj->ob_type, DictionaryTypePtr))
    {
	tv->v_type = VAR_DICT;
	tv->vval.v_dict = (((DictionaryObject *)(obj))->dict);
	++tv->vval.v_dict->dv_refcount;
	ret = 0;
    }
    નહીંતર જો (PyDict_Check(obj))
	ret = convert_dl(obj, tv, pydict_to_tv, lookup_dict);
    નહીંતર જો (PyMapping_Check(obj))
	ret = convert_dl(obj, tv, pymap_to_tv, lookup_dict);
    નહીંતર
    {
	PyErr_FORMAT_TYPE(
		N_("unable to convert %s to a Vim dictionary"),
		obj);
	ret = -1;
    }
    Py_DECREF(lookup_dict);
    પાછા ret;
}

    સ્થિર પૂર્ણાંક
ConvertFromPySequence(PyObject *obj, typval_T *tv)
{
    PyObject	*lookup_dict;
    પૂર્ણાંક		ret;

    જો (!(lookup_dict = PyDict_New()))
	પાછા -1;

    જો (PyType_IsSubtype(obj->ob_type, ListTypePtr))
    {
	tv->v_type = VAR_LIST;
	tv->vval.v_list = (((ListObject *)(obj))->list);
	++tv->vval.v_list->lv_refcount;
	ret = 0;
    }
    નહીંતર જો (PyIter_Check(obj) || PySequence_Check(obj))
	ret = convert_dl(obj, tv, pyseq_to_tv, lookup_dict);
    નહીંતર
    {
	PyErr_FORMAT_TYPE(
		N_("unable to convert %s to a Vim list"),
		obj);
	ret = -1;
    }
    Py_DECREF(lookup_dict);
    પાછા ret;
}

    સ્થિર પૂર્ણાંક
ConvertFromPyObject(PyObject *obj, typval_T *tv)
{
    PyObject	*lookup_dict;
    પૂર્ણાંક		ret;

    જો (!(lookup_dict = PyDict_New()))
	પાછા -1;
    ret = _ConvertFromPyObject(obj, tv, lookup_dict);
    Py_DECREF(lookup_dict);
    પાછા ret;
}

    સ્થિર પૂર્ણાંક
_ConvertFromPyObject(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    જો (PyType_IsSubtype(obj->ob_type, DictionaryTypePtr))
    {
	tv->v_type = VAR_DICT;
	tv->vval.v_dict = (((DictionaryObject *)(obj))->dict);
	++tv->vval.v_dict->dv_refcount;
    }
    નહીંતર જો (PyType_IsSubtype(obj->ob_type, ListTypePtr))
    {
	tv->v_type = VAR_LIST;
	tv->vval.v_list = (((ListObject *)(obj))->list);
	++tv->vval.v_list->lv_refcount;
    }
    નહીંતર જો (PyType_IsSubtype(obj->ob_type, TupleTypePtr))
    {
	tv->v_type = VAR_TUPLE;
	tv->vval.v_tuple = (((TupleObject *)(obj))->tuple);
	++tv->vval.v_tuple->tv_refcount;
    }
    નહીંતર જો (PyType_IsSubtype(obj->ob_type, FunctionTypePtr))
    {
	FunctionObject *func = (FunctionObject *) obj;
	જો (func->self != ખાલી || func->argv != ખાલી)
	{
	    partial_T *pt = ALLOC_CLEAR_ONE(partial_T);

	    set_partial(func, pt, TRUE);
	    tv->vval.v_partial = pt;
	    tv->v_type = VAR_PARTIAL;
	}
	નહીંતર
	{
	    જો (set_string_copy(func->name, tv) == -1)
		પાછા -1;

	    tv->v_type = VAR_FUNC;
	}
	func_ref(func->name);
    }
    નહીંતર જો (PyBytes_Check(obj))
    {
	char_u	*str;

	જો (PyBytes_AsStringAndSize(obj, (અક્ષર **) &str, ખાલી) == -1)
	    પાછા -1;
	જો (str == ખાલી)
	    પાછા -1;

	જો (set_string_copy(str, tv) == -1)
	    પાછા -1;

	tv->v_type = VAR_STRING;
    }
    નહીંતર જો (PyUnicode_Check(obj))
    {
	PyObject	*bytes;
	char_u	*str;

	bytes = PyUnicode_AsEncodedString(obj, ENC_OPT, ERRORS_ENCODE_ARG);
	જો (bytes == ખાલી)
	    પાછા -1;

	જો (PyBytes_AsStringAndSize(bytes, (અક્ષર **) &str, ખાલી) == -1)
	    પાછા -1;
	જો (str == ખાલી)
	    પાછા -1;

	જો (set_string_copy(str, tv))
	{
	    Py_XDECREF(bytes);
	    પાછા -1;
	}
	Py_XDECREF(bytes);

	tv->v_type = VAR_STRING;
    }
#જો PY_MAJOR_VERSION < 3
    નહીંતર જો (PyInt_Check(obj))
    {
	tv->v_type = VAR_સંખ્યા;
	tv->vval.v_સંખ્યા = (varnumber_T) PyInt_AsLong(obj);
	જો (PyErr_Occurred())
	    પાછા -1;
    }
#endif
    નહીંતર જો (PyLong_Check(obj))
    {
	tv->v_type = VAR_સંખ્યા;
	tv->vval.v_સંખ્યા = (varnumber_T) PyLong_AsLong(obj);
	જો (PyErr_Occurred())
	    પાછા -1;
    }
    નહીંતર જો (PyTuple_Check(obj))
	પાછા convert_dl(obj, tv, pytuple_to_tv, lookup_dict);
    નહીંતર જો (PyDict_Check(obj))
	પાછા convert_dl(obj, tv, pydict_to_tv, lookup_dict);
    નહીંતર જો (PyFloat_Check(obj))
    {
	tv->v_type = VAR_FLOAT;
	tv->vval.v_float = (float_T) PyFloat_AsDouble(obj);
    }
    નહીંતર જો (PyObject_HasAttrString(obj, "keys"))
	પાછા convert_dl(obj, tv, pymap_to_tv, lookup_dict);
    // PyObject_GetIter can create built-in iterator for any sequence object
    નહીંતર જો (PyIter_Check(obj) || PySequence_Check(obj))
	પાછા convert_dl(obj, tv, pyseq_to_tv, lookup_dict);
    નહીંતર જો (PyMapping_Check(obj))
	પાછા convert_dl(obj, tv, pymap_to_tv, lookup_dict);
    નહીંતર જો (Py_સંખ્યા_Check(obj))
    {
	PyObject	*અંક;

	જો (!(અંક = Py_સંખ્યા_Long(obj)))
	    પાછા -1;

	tv->v_type = VAR_સંખ્યા;
	tv->vval.v_સંખ્યા = (varnumber_T) PyLong_AsLong(અંક);

	Py_DECREF(અંક);
    }
    નહીંતર જો (obj == Py_None)
    {
	tv->v_type = VAR_SPECIAL;
	tv->vval.v_સંખ્યા = VVAL_NONE;
    }
    નહીંતર
    {
	PyErr_FORMAT_TYPE(
		N_("unable to convert %s to a Vim structure"),
		obj);
	પાછા -1;
    }
    પાછા 0;
}

    સ્થિર PyObject *
ConvertToPyObject(typval_T *tv)
{
    typval_T *argv;
    પૂર્ણાંક i;
    જો (tv == ખાલી)
    {
	PyErr_SET_VIM(N_("internal error: NULL reference passed"));
	પાછા ખાલી;
    }
    પસંદગી (tv->v_type)
    {
	સ્થિતિ VAR_STRING:
	    પાછા PyBytes_FromString(tv->vval.v_string == ખાલી
					    ? "" : (અક્ષર *)tv->vval.v_string);
	સ્થિતિ VAR_સંખ્યા:
	    પાછા PyLong_FromLong((long) tv->vval.v_સંખ્યા);
	સ્થિતિ VAR_FLOAT:
	    પાછા PyFloat_FromDouble((દ્વિગુણ) tv->vval.v_float);
	સ્થિતિ VAR_LIST:
	    પાછા NEW_LIST(tv->vval.v_list);
	સ્થિતિ VAR_TUPLE:
	    પાછા NEW_TUPLE(tv->vval.v_tuple);
	સ્થિતિ VAR_DICT:
	    પાછા NEW_DICTIONARY(tv->vval.v_dict);
	સ્થિતિ VAR_FUNC:
	    પાછા NEW_FUNCTION(tv->vval.v_string == ખાલી
					  ? (char_u *)"" : tv->vval.v_string,
					  0, ખાલી, ખાલી, TRUE);
	સ્થિતિ VAR_PARTIAL:
	    જો (tv->vval.v_partial->pt_argc)
	    {
		argv = PyMem_New(typval_T, (size_t)tv->vval.v_partial->pt_argc);
		માટે (i = 0; i < tv->vval.v_partial->pt_argc; i++)
		    copy_tv(&tv->vval.v_partial->pt_argv[i], &argv[i]);
	    }
	    નહીંતર
		argv = ખાલી;
	    જો (tv->vval.v_partial->pt_dict != ખાલી)
		tv->vval.v_partial->pt_dict->dv_refcount++;
	    પાછા NEW_FUNCTION(tv->vval.v_partial == ખાલી
			     ? (char_u *)"" : partial_name(tv->vval.v_partial),
				tv->vval.v_partial->pt_argc, argv,
				tv->vval.v_partial->pt_dict,
				tv->vval.v_partial->pt_auto);
	સ્થિતિ VAR_BLOB:
	    પાછા PyBytes_FromStringAndSize(
		(અક્ષર*) tv->vval.v_blob->bv_ga.ga_data,
		(Py_ssize_t) tv->vval.v_blob->bv_ga.ga_len);
	સ્થિતિ VAR_UNKNOWN:
	સ્થિતિ VAR_ANY:
	સ્થિતિ VAR_VOID:
	સ્થિતિ VAR_CHANNEL:
	સ્થિતિ VAR_JOB:
	સ્થિતિ VAR_INSTR:
	સ્થિતિ VAR_CLASS:
	સ્થિતિ VAR_OBJECT:
	સ્થિતિ VAR_TYPEALIAS:
	    Py_INCREF(Py_None);
	    પાછા Py_None;
	સ્થિતિ VAR_BOOL:
	સ્થિતિ VAR_SPECIAL:
	    પસંદગી (tv->vval.v_સંખ્યા)
	    {
		સ્થિતિ VVAL_FALSE: પાછા ALWAYS_FALSE;
		સ્થિતિ VVAL_TRUE:  પાછા ALWAYS_TRUE;
		સ્થિતિ VVAL_NONE:
		સ્થિતિ VVAL_NULL:  પાછા ALWAYS_NONE;
	    }
	    PyErr_SET_VIM(N_("internal error: invalid value type"));
	    પાછા ખાલી;
    }
    પાછા ખાલી;
}

DEFINE_PY_TYPE_OBJECT(CurrentType);

પ્રકારવ્યાખ્યા સંરચના
{
    PyObject_HEAD
} CurrentObject;

સ્થિર CurrentObject TheCurrent =
{
    PyObject_HEAD_INIT_TYPE(CurrentType)
};

    સ્થિર શૂન્ય
init_structs(શૂન્ય)
{
    CLEAR_FIELD(OutputType);
    OutputType.tp_name = "vim.message";
    OutputType.tp_basicsize = કદ(OutputObject);
    OutputType.tp_flags = Py_TPFLAGS_DEFAULT;
    OutputType.tp_doc = "vim message object";
    OutputType.tp_methods = OutputMethods;
#જો PY_MAJOR_VERSION >= 3
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
#નહીંતર
    OutputType.tp_getattr = OutputGetattr;
    OutputType.tp_setattr = OutputSetattr;
    // Disabled, because this causes a crash in test86
    // OutputType.tp_base = &PyFile_Type;
#endif

    CLEAR_FIELD(IterType);
    IterType.tp_name = "vim.iter";
    IterType.tp_basicsize = કદ(IterObject);
    IterType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    IterType.tp_doc = "generic iterator object";
    IterType.tp_iter = IterIter;
    IterType.tp_iternext = IterNext;
    IterType.tp_dealloc = IterDestructor;
    IterType.tp_traverse = IterTraverse;
    IterType.tp_clear = IterClear;

    CLEAR_FIELD(BufferType);
    BufferType.tp_name = "vim.buffer";
    BufferType.tp_basicsize = કદ(BufferObject);
    BufferType.tp_dealloc = BufferDestructor;
    BufferType.tp_repr = BufferRepr;
    BufferType.tp_as_sequence = &BufferAsSeq;
    BufferType.tp_as_mapping = &BufferAsMapping;
    BufferType.tp_flags = Py_TPFLAGS_DEFAULT;
    BufferType.tp_doc = "vim buffer object";
    BufferType.tp_methods = BufferMethods;
#જો PY_MAJOR_VERSION >= 3
    BufferType.tp_getattro = BufferGetattro;
    BufferType.tp_setattro = BufferSetattro;
    BufferType.tp_alloc = call_PyType_GenericAlloc;
    BufferType.tp_new = call_PyType_GenericNew;
    BufferType.tp_free = call_PyObject_Free;
#નહીંતર
    BufferType.tp_getattr = BufferGetattr;
    BufferType.tp_setattr = BufferSetattr;
#endif

    CLEAR_FIELD(WindowType);
    WindowType.tp_name = "vim.window";
    WindowType.tp_basicsize = કદ(WindowObject);
    WindowType.tp_dealloc = WindowDestructor;
    WindowType.tp_repr = WindowRepr;
    WindowType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    WindowType.tp_doc = "vim Window object";
    WindowType.tp_methods = WindowMethods;
    WindowType.tp_traverse = WindowTraverse;
    WindowType.tp_clear = WindowClear;
#જો PY_MAJOR_VERSION >= 3
    WindowType.tp_getattro = WindowGetattro;
    WindowType.tp_setattro = WindowSetattro;
    WindowType.tp_alloc = call_PyType_GenericAlloc;
    WindowType.tp_new = call_PyType_GenericNew;
    WindowType.tp_free = call_PyObject_Free;
#નહીંતર
    WindowType.tp_getattr = WindowGetattr;
    WindowType.tp_setattr = WindowSetattr;
#endif

    CLEAR_FIELD(TabPageType);
    TabPageType.tp_name = "vim.tabpage";
    TabPageType.tp_basicsize = કદ(TabPageObject);
    TabPageType.tp_dealloc = TabPageDestructor;
    TabPageType.tp_repr = TabPageRepr;
    TabPageType.tp_flags = Py_TPFLAGS_DEFAULT;
    TabPageType.tp_doc = "vim tab page object";
    TabPageType.tp_methods = TabPageMethods;
#જો PY_MAJOR_VERSION >= 3
    TabPageType.tp_getattro = TabPageGetattro;
    TabPageType.tp_alloc = call_PyType_GenericAlloc;
    TabPageType.tp_new = call_PyType_GenericNew;
    TabPageType.tp_free = call_PyObject_Free;
#નહીંતર
    TabPageType.tp_getattr = TabPageGetattr;
#endif

    CLEAR_FIELD(BufMapType);
    BufMapType.tp_name = "vim.bufferlist";
    BufMapType.tp_basicsize = કદ(BufMapObject);
    BufMapType.tp_as_mapping = &BufMapAsMapping;
    BufMapType.tp_flags = Py_TPFLAGS_DEFAULT;
    BufMapType.tp_iter = BufMapIter;
    BufMapType.tp_doc = "vim buffer list";

    CLEAR_FIELD(WinListType);
    WinListType.tp_name = "vim.windowlist";
    WinListType.tp_basicsize = કદ(WinListObject);
    WinListType.tp_as_sequence = &WinListAsSeq;
    WinListType.tp_flags = Py_TPFLAGS_DEFAULT;
    WinListType.tp_doc = "vim window list";
    WinListType.tp_dealloc = (destructor)WinListDestructor;

    CLEAR_FIELD(TabListType);
    TabListType.tp_name = "vim.tabpagelist";
    TabListType.tp_basicsize = કદ(TabListObject);
    TabListType.tp_as_sequence = &TabListAsSeq;
    TabListType.tp_flags = Py_TPFLAGS_DEFAULT;
    TabListType.tp_doc = "vim tab page list";

    CLEAR_FIELD(RangeType);
    RangeType.tp_name = "vim.range";
    RangeType.tp_basicsize = કદ(RangeObject);
    RangeType.tp_dealloc = RangeDestructor;
    RangeType.tp_repr = RangeRepr;
    RangeType.tp_as_sequence = &RangeAsSeq;
    RangeType.tp_as_mapping = &RangeAsMapping;
    RangeType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    RangeType.tp_doc = "vim Range object";
    RangeType.tp_methods = RangeMethods;
    RangeType.tp_traverse = RangeTraverse;
    RangeType.tp_clear = RangeClear;
#જો PY_MAJOR_VERSION >= 3
    RangeType.tp_getattro = RangeGetattro;
    RangeType.tp_alloc = call_PyType_GenericAlloc;
    RangeType.tp_new = call_PyType_GenericNew;
    RangeType.tp_free = call_PyObject_Free;
#નહીંતર
    RangeType.tp_getattr = RangeGetattr;
#endif

    CLEAR_FIELD(CurrentType);
    CurrentType.tp_name = "vim.currentdata";
    CurrentType.tp_basicsize = કદ(CurrentObject);
    CurrentType.tp_flags = Py_TPFLAGS_DEFAULT;
    CurrentType.tp_doc = "vim current object";
    CurrentType.tp_methods = CurrentMethods;
#જો PY_MAJOR_VERSION >= 3
    CurrentType.tp_getattro = CurrentGetattro;
    CurrentType.tp_setattro = CurrentSetattro;
#નહીંતર
    CurrentType.tp_getattr = CurrentGetattr;
    CurrentType.tp_setattr = CurrentSetattr;
#endif

    CLEAR_FIELD(DictionaryType);
    DictionaryType.tp_name = "vim.dictionary";
    DictionaryType.tp_basicsize = કદ(DictionaryObject);
    DictionaryType.tp_dealloc = DictionaryDestructor;
    DictionaryType.tp_as_sequence = &DictionaryAsSeq;
    DictionaryType.tp_as_mapping = &DictionaryAsMapping;
    DictionaryType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE;
    DictionaryType.tp_doc = "dictionary pushing modifications to Vim structure";
    DictionaryType.tp_methods = DictionaryMethods;
    DictionaryType.tp_iter = DictionaryIter;
    DictionaryType.tp_new = DictionaryConstructor;
    DictionaryType.tp_alloc = PyType_GenericAlloc;
#જો PY_MAJOR_VERSION >= 3
    DictionaryType.tp_getattro = DictionaryGetattro;
    DictionaryType.tp_setattro = DictionarySetattro;
#નહીંતર
    DictionaryType.tp_getattr = DictionaryGetattr;
    DictionaryType.tp_setattr = DictionarySetattr;
#endif

    CLEAR_FIELD(ListType);
    ListType.tp_name = "vim.list";
    ListType.tp_dealloc = ListDestructor;
    ListType.tp_basicsize = કદ(ListObject);
    ListType.tp_as_sequence = &ListAsSeq;
    ListType.tp_as_mapping = &ListAsMapping;
    ListType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE;
    ListType.tp_doc = "list pushing modifications to Vim structure";
    ListType.tp_methods = ListMethods;
    ListType.tp_iter = ListIter;
    ListType.tp_new = ListConstructor;
    ListType.tp_alloc = PyType_GenericAlloc;
#જો PY_MAJOR_VERSION >= 3
    ListType.tp_getattro = ListGetattro;
    ListType.tp_setattro = ListSetattro;
#નહીંતર
    ListType.tp_getattr = ListGetattr;
    ListType.tp_setattr = ListSetattr;
#endif

    // Tuple type
    CLEAR_FIELD(TupleType);
    TupleType.tp_name = "vim.tuple";
    TupleType.tp_dealloc = TupleDestructor;
    TupleType.tp_basicsize = કદ(TupleObject);
    TupleType.tp_as_sequence = &TupleAsSeq;
    TupleType.tp_as_mapping = &TupleAsMapping;
    TupleType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE;
    TupleType.tp_doc = "tuple pushing modifications to Vim structure";
    TupleType.tp_methods = TupleMethods;
    TupleType.tp_iter = TupleIter;
    TupleType.tp_new = TupleConstructor;
    TupleType.tp_alloc = PyType_GenericAlloc;
#જો PY_MAJOR_VERSION >= 3
    TupleType.tp_getattro = TupleGetattro;
    TupleType.tp_setattro = TupleSetattro;
#નહીંતર
    TupleType.tp_getattr = TupleGetattr;
    TupleType.tp_setattr = TupleSetattr;
#endif

    CLEAR_FIELD(FunctionType);
    FunctionType.tp_name = "vim.function";
    FunctionType.tp_basicsize = કદ(FunctionObject);
    FunctionType.tp_dealloc = FunctionDestructor;
    FunctionType.tp_call = FunctionCall;
    FunctionType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE;
    FunctionType.tp_doc = "object that calls Vim function";
    FunctionType.tp_methods = FunctionMethods;
    FunctionType.tp_repr = FunctionRepr;
    FunctionType.tp_new = FunctionConstructor;
    FunctionType.tp_alloc = PyType_GenericAlloc;
#જો PY_MAJOR_VERSION >= 3
    FunctionType.tp_getattro = FunctionGetattro;
#નહીંતર
    FunctionType.tp_getattr = FunctionGetattr;
#endif

    CLEAR_FIELD(OptionsType);
    OptionsType.tp_name = "vim.options";
    OptionsType.tp_basicsize = કદ(OptionsObject);
    OptionsType.tp_as_sequence = &OptionsAsSeq;
    OptionsType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    OptionsType.tp_doc = "object for manipulating options";
    OptionsType.tp_iter = OptionsIter;
    OptionsType.tp_as_mapping = &OptionsAsMapping;
    OptionsType.tp_dealloc = OptionsDestructor;
    OptionsType.tp_traverse = OptionsTraverse;
    OptionsType.tp_clear = OptionsClear;

#જો PY_VERSION_HEX < 0x030700f0
    CLEAR_FIELD(LoaderType);
    LoaderType.tp_name = "vim.Loader";
    LoaderType.tp_basicsize = કદ(LoaderObject);
    LoaderType.tp_flags = Py_TPFLAGS_DEFAULT;
    LoaderType.tp_doc = "vim message object";
    LoaderType.tp_methods = LoaderMethods;
    LoaderType.tp_dealloc = LoaderDestructor;
#endif

#જો PY_MAJOR_VERSION >= 3
    CLEAR_FIELD(vimmodule);
    vimmodule.m_name = "vim";
    vimmodule.m_doc = "Vim Python interface\n";
    vimmodule.m_size = -1;
    vimmodule.m_methods = VimMethods;
#endif
}

    સ્થિર પૂર્ણાંક
init_types(શૂન્ય)
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
#જો PY_VERSION_HEX < 0x030700f0
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
    પાછા 0;
}

#ifdef USE_LIMITED_API
    સ્થિર શૂન્ય
shutdown_types(શૂન્ય)
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
# જો PY_VERSION_HEX < 0x030700f0
    PYTYPE_CLEANUP(LoaderType);
# endif
}
#endif

    સ્થિર પૂર્ણાંક
init_sys_path(શૂન્ય)
{
    PyObject	*path;
    PyObject	*path_hook;
    PyObject	*path_hooks;

    જો (!(path_hook = PyObject_GetAttrString(vim_module, "path_hook")))
	પાછા -1;

    જો (!(path_hooks = PySys_GetObject("path_hooks")))
    {
	PyErr_Clear();
	path_hooks = PyList_New(1);
	PyList_SET_ITEM(path_hooks, 0, path_hook);
	જો (PySys_SetObject("path_hooks", path_hooks))
	{
	    Py_DECREF(path_hooks);
	    પાછા -1;
	}
	Py_DECREF(path_hooks);
    }
    નહીંતર જો (PyList_Check(path_hooks))
    {
	જો (PyList_Append(path_hooks, path_hook))
	{
	    Py_DECREF(path_hook);
	    પાછા -1;
	}
	Py_DECREF(path_hook);
    }
    નહીંતર
    {
	VimTryStart();
	emsg(_("Failed to set path hook: sys.path_hooks is not a list\n"
	       "You should now do the following:\n"
	       "- append vim.path_hook to sys.path_hooks\n"
	       "- append vim.VIM_SPECIAL_PATH to sys.path\n"));
	VimTryEnd(); // Discard the error
	Py_DECREF(path_hook);
	પાછા 0;
    }

    જો (!(path = PySys_GetObject("path")))
    {
	PyErr_Clear();
	path = PyList_New(1);
	Py_INCREF(vim_special_path_object);
	PyList_SET_ITEM(path, 0, vim_special_path_object);
	જો (PySys_SetObject("path", path))
	{
	    Py_DECREF(path);
	    પાછા -1;
	}
	Py_DECREF(path);
    }
    નહીંતર જો (PyList_Check(path))
    {
	જો (PyList_Append(path, vim_special_path_object))
	    પાછા -1;
    }
    નહીંતર
    {
	VimTryStart();
	emsg(_("Failed to set path: sys.path is not a list\n"
	       "You should now append vim.VIM_SPECIAL_PATH to sys.path"));
	VimTryEnd(); // Discard the error
    }

    પાછા 0;
}

સ્થિર સંરચના numeric_constant {
    અક્ષર	*name;
    પૂર્ણાંક		val;
} numeric_constants[] = {
    {"VAR_LOCKED",	VAR_LOCKED},
    {"VAR_FIXED",	VAR_FIXED},
    {"VAR_SCOPE",	VAR_SCOPE},
    {"VAR_DEF_SCOPE",	VAR_DEF_SCOPE},
};

સંરચના object_constant {
    અક્ષર	*name;
    PyObject	*valObject;
};

#define ADD_OBJECT(m, name, obj) \
    જો (PyModule_AddObject(m, name, obj)) \
	પાછા -1;

#define ADD_CHECKED_OBJECT(m, name, obj) \
    { \
	PyObject	*valObject = obj; \
	જો (!valObject) \
	    પાછા -1; \
	ADD_OBJECT(m, name, valObject); \
    }

    સ્થિર પૂર્ણાંક
populate_module(PyObject *m)
{
    પૂર્ણાંક		i;
    PyObject	*other_module;
    PyObject	*attr;
    PyObject	*imp;
#જો PY_VERSION_HEX >= 0x030700f0
    PyObject	*dict;
    PyObject	*cls;
#endif

    માટે (i = 0; i < (પૂર્ણાંક)(કદ(numeric_constants)
					   / કદ(સંરચના numeric_constant));
	    ++i)
	ADD_CHECKED_OBJECT(m, numeric_constants[i].name,
		PyInt_FromLong(numeric_constants[i].val));

    સંરચના object_constant object_constants[] = {
	{"buffers",  (PyObject *)(શૂન્ય *)&TheBufferMap},
	{"windows",  (PyObject *)(શૂન્ય *)&TheWindowList},
	{"tabpages", (PyObject *)(શૂન્ય *)&TheTabPageList},
	{"current",  (PyObject *)(શૂન્ય *)&TheCurrent},

	{"Buffer",     (PyObject *)BufferTypePtr},
	{"Range",      (PyObject *)RangeTypePtr},
	{"Window",     (PyObject *)WindowTypePtr},
	{"TabPage",    (PyObject *)TabPageTypePtr},
	{"Dictionary", (PyObject *)DictionaryTypePtr},
	{"List",       (PyObject *)ListTypePtr},
	{"Tuple",      (PyObject *)TupleTypePtr},
	{"Function",   (PyObject *)FunctionTypePtr},
	{"Options",    (PyObject *)OptionsTypePtr},
#જો PY_VERSION_HEX < 0x030700f0
	{"_Loader",    (PyObject *)LoaderTypePtr},
#endif
    };

    માટે (i = 0; i < (પૂર્ણાંક)(કદ(object_constants)
					    / કદ(સંરચના object_constant));
	    ++i)
    {
	PyObject	*valObject;

	valObject = object_constants[i].valObject;
	Py_INCREF(valObject);
	ADD_OBJECT(m, object_constants[i].name, valObject);
    }

    જો (!(VimError = PyErr_NewException("vim.error", ખાલી, ખાલી)))
	પાછા -1;
    ADD_OBJECT(m, "error", VimError);

    ADD_CHECKED_OBJECT(m, "vars",  NEW_DICTIONARY(get_globvar_dict()));
    ADD_CHECKED_OBJECT(m, "vvars", NEW_DICTIONARY(get_vimvar_dict()));
    ADD_CHECKED_OBJECT(m, "options",
	    OptionsNew(SREQ_GLOBAL, ખાલી, dummy_check, ખાલી));

    જો (!(other_module = PyImport_ImportModule("os")))
	પાછા -1;
    ADD_OBJECT(m, "os", other_module);

#જો PY_MAJOR_VERSION >= 3
    જો (!(py_getcwd = PyObject_GetAttrString(other_module, "getcwd")))
	પાછા -1;
#નહીંતર
    જો (!(py_getcwd = PyObject_GetAttrString(other_module, "getcwdu")))
	પાછા -1;
#endif
    ADD_OBJECT(m, "_getcwd", py_getcwd)

    જો (!(py_chdir = PyObject_GetAttrString(other_module, "chdir")))
	પાછા -1;
    ADD_OBJECT(m, "_chdir", py_chdir);
    જો (!(attr = PyObject_GetAttrString(m, "chdir")))
	પાછા -1;
    જો (PyObject_SetAttrString(other_module, "chdir", attr))
    {
	Py_DECREF(attr);
	પાછા -1;
    }
    Py_DECREF(attr);

    જો ((py_fchdir = PyObject_GetAttrString(other_module, "fchdir")))
    {
	ADD_OBJECT(m, "_fchdir", py_fchdir);
	જો (!(attr = PyObject_GetAttrString(m, "fchdir")))
	    પાછા -1;
	જો (PyObject_SetAttrString(other_module, "fchdir", attr))
	{
	    Py_DECREF(attr);
	    પાછા -1;
	}
	Py_DECREF(attr);
    }
    નહીંતર
	PyErr_Clear();

    જો (!(vim_special_path_object = PyString_FromString(vim_special_path)))
	પાછા -1;

    ADD_OBJECT(m, "VIM_SPECIAL_PATH", vim_special_path_object);

#જો PY_VERSION_HEX >= 0x030700f0
    જો (!(imp = PyImport_ImportModule("importlib.machinery")))
	પાછા -1;

    dict = PyModule_GetDict(imp);

    જો (!(cls = PyDict_GetItemString(dict, "PathFinder")))
    {
	Py_DECREF(imp);
	પાછા -1;
    }

    જો (!(py_find_spec = PyObject_GetAttrString(cls, "find_spec")))
    {
	Py_DECREF(imp);
	પાછા -1;
    }

# જો PY_VERSION_HEX < 0x30c00a7
    // find_module has been removed as of Python 3.12.0a7
    જો ((py_find_module = PyObject_GetAttrString(cls, "find_module")))
	ADD_OBJECT(m, "_find_module", py_find_module);
# endif

    Py_DECREF(imp);

    ADD_OBJECT(m, "_find_spec", py_find_spec);
#નહીંતર
    જો (!(imp = PyImport_ImportModule("imp")))
	પાછા -1;

    જો (!(py_find_module = PyObject_GetAttrString(imp, "find_module")))
    {
	Py_DECREF(imp);
	પાછા -1;
    }

    જો (!(py_load_module = PyObject_GetAttrString(imp, "load_module")))
    {
	Py_DECREF(py_find_module);
	Py_DECREF(imp);
	પાછા -1;
    }

    Py_DECREF(imp);

    ADD_OBJECT(m, "_find_module", py_find_module);
    ADD_OBJECT(m, "_load_module", py_load_module);
#endif

    પાછા 0;
}
