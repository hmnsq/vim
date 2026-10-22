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

#എങ്കിൽ PY_VERSION_HEX < 0x02050000
തരനിർവചനം പൂർണ്ണസംഖ്യ Py_ssize_t;  // Python 2.4 and earlier don't have this type.
#endif

// Use values that are known to work, others may make Vim crash.
#define ENC_OPT (enc_utf8 ? "utf-8" : enc_dbcs ? "euc-jp" : (അക്ഷരം *)p_enc)
#define DOPY_FUNC "_vim_pydo"

സ്ഥിതം സ്ഥിരം അക്ഷരം *vim_special_path = "_vim_path_";

#define PyErr_SET_STRING(exc, str) PyErr_SetString(exc, _(str))
#define PyErr_SetVim(str) PyErr_SetString(VimError, str)
#define PyErr_SET_VIM(str) PyErr_SET_STRING(VimError, str)
#define PyErr_FORMAT(exc, str, arg) PyErr_Format(exc, _(str), arg)
#define PyErr_FORMAT2(exc, str, arg1, arg2) PyErr_Format(exc, _(str), arg1,arg2)
#define PyErr_VIM_FORMAT(str, arg) PyErr_FORMAT(VimError, str, arg)

#ifdef USE_LIMITED_API
// Limited Python API. Need to call only exposed functions and remap macros.
// PyTypeObject is an opaque struct.

തരനിർവചനം ഘടന {
    lenfunc sq_length;
    binaryfunc sq_concat;
    ssizeargfunc sq_repeat;
    ssizeargfunc sq_item;
    ശൂന്യം *was_sq_slice;
    ssizeobjargproc sq_ass_item;
    ശൂന്യം *was_sq_ass_slice;
    objobjproc sq_contains;

    binaryfunc sq_inplace_concat;
    ssizeargfunc sq_inplace_repeat;
} PySequenceMethods;

തരനിർവചനം ഘടന {
    lenfunc mp_length;
    binaryfunc mp_subscript;
    objobjargproc mp_ass_subscript;
} PyMappingMethods;

// This struct emulates the concrete _typeobject struct to allow the code to
// work the same way in both limited and full Python APIs.
ഘടന typeobject_wrapper {
    സ്ഥിരം അക്ഷരം *tp_name;
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

    സ്ഥിരം അക്ഷരം *tp_doc;

    traverseproc tp_traverse;

    inquiry tp_clear;

    getiterfunc tp_iter;
    iternextfunc tp_iternext;

    ഘടന PyMethodDef *tp_methods;
    ഘടന _typeobject *tp_base;
    allocfunc tp_alloc;
    newfunc tp_new;
    freefunc tp_free;
};

# define DEFINE_PY_TYPE_OBJECT(type) \
    സ്ഥിതം ഘടന typeobject_wrapper type; \
    സ്ഥിതം PyTypeObject* type##Ptr = ശൂന്യ

// PyObject_HEAD_INIT_TYPE and PyObject_FINISH_INIT_TYPE need to come in pairs
// We first initialize with NULL because the type is not allocated until
// init_types() is called later. It's in FINISH_INIT_TYPE where we fill the
// type in with the newly allocated type.
# define PyObject_HEAD_INIT_TYPE(type) PyObject_HEAD_INIT(ശൂന്യ)
# define PyObject_FINISH_INIT_TYPE(obj, type) obj.ob_base.ob_type = type##Ptr

# define Py_TYPE_GET_TP_ALLOC(type) ((allocfunc)PyType_GetSlot(type, Py_tp_alloc))
# define Py_TYPE_GET_TP_METHODS(type) ((PyMethodDef *)PyType_GetSlot(type, Py_tp_methods))

// PyObject_NEW is not part of stable ABI, but PyObject_Malloc/Init are.
സ്ഥിതം PyObject* Vim_PyObject_New(PyTypeObject *type, size_t objsize)
{
    PyObject *obj = (PyObject *)PyObject_Malloc(objsize);
    എങ്കിൽ (obj == ശൂന്യ)
	തിരികെ PyErr_NoMemory();
    തിരികെ PyObject_Init(obj, type);
}
# undef PyObject_NEW
# define PyObject_NEW(type, typeobj) ((type *)Vim_PyObject_New(typeobj, വലുപ്പം(type)))

// This is a somewhat convoluted because limited API doesn't expose an easy way
// to get the tp_name field, and so we have to manually reconstruct it as
// "__module__.__name__" (with __module__ omitted for builtins to emulate
// Python behavior). Also, some of the more convenient functions like
// PyUnicode_AsUTF8AndSize and PyType_GetQualName() are not available until
// late Python 3 versions, and won't be available if you set Py_LIMITED_API too
// low.
# define PyErr_FORMAT_TYPE(സന്ദേശം, obj) \
    ചെയ്യുക { \
	PyObject* qualname = PyObject_GetAttrString((PyObject*)(obj)->ob_type, "__qualname__"); \
	എങ്കിൽ (qualname == ശൂന്യ) \
	{ \
	    PyErr_FORMAT(PyExc_TypeError, സന്ദേശം, "(NULL)"); \
	    നിർത്തുക; \
	} \
	PyObject* module = PyObject_GetAttrString((PyObject*)(obj)->ob_type, "__module__"); \
	PyObject* full; \
	എങ്കിൽ (module == ശൂന്യ || PyUnicode_CompareWithASCIIString(module, "builtins") == 0 \
		|| PyUnicode_CompareWithASCIIString(module, "__main__") == 0) \
	{ \
	    full = qualname; \
	    Py_INCREF(full); \
	} \
	അല്ലെങ്കിൽ \
	    full = PyUnicode_FromFormat("%U.%U", module, qualname); \
	PyObject* full_bytes = PyUnicode_AsUTF8String(full); \
	സ്ഥിരം അക്ഷരം* full_str = PyBytes_AsString(full_bytes); \
	full_str = full_str == ശൂന്യ ? "(NULL)" : full_str; \
	PyErr_FORMAT(PyExc_TypeError, സന്ദേശം, full_str); \
	Py_DECREF(qualname); \
	Py_XDECREF(module); \
	Py_XDECREF(full); \
	Py_XDECREF(full_bytes); \
    } ആകുമ്പോൾ (0)

# define PyList_GET_ITEM(list, i) PyList_GetItem(list, i)
# define PyList_GET_SIZE(o) PyList_Size(o)

// PyList_SET_ITEM and PyList_SetItem have slightly different behaviors. The
// former will leave the old item dangling, and the latter will decref on it.
// Since we only use this on new lists, this difference doesn't matter.
# define PyList_SET_ITEM(list, i, item) PyList_SetItem(list, i, item)

# define PyTuple_GET_ITEM(o, pos) PyTuple_GetItem(o, pos)
# define PyTuple_GET_SIZE(o) PyTuple_Size(o)

# എങ്കിൽ Py_LIMITED_API < 0x03080000
// PyIter_check only became part of stable ABI in 3.8, and there is no easy way
// to check for it in the API. We simply return false as a compromise. This
// does mean we should avoid compiling with stable ABI < 3.8.
#  undef PyIter_Check
#  define PyIter_Check(obj) (FALSE)
# endif

സ്ഥിതം PyTypeObject* AddHeapType(ഘടന typeobject_wrapper* type_object)
{
    PyType_Spec type_spec;
    type_spec.name = type_object->tp_name;
    type_spec.basicsize = type_object->tp_basicsize;
    type_spec.itemsize = 0;
    type_spec.flags = type_object->tp_flags;

    // We just need to statically allocate a large enough buffer that can hold
    // all slots. We need to leave a null-terminated slot at the end.
    PyType_Slot slots[40] = { {0, ശൂന്യ} };
    size_t slot_i = 0;

# define ADD_TP_SLOT(slot_name) \
    എങ്കിൽ (slot_i >= 40) തിരികെ ശൂന്യ; /* this should never happen */ \
    എങ്കിൽ (type_object->slot_name != ശൂന്യ) \
    { \
	slots[slot_i].slot = Py_##slot_name; \
	slots[slot_i].pfunc = (ശൂന്യം*)type_object->slot_name; \
	++slot_i; \
    }
# define ADD_TP_SUB_SLOT(sub_slot, slot_name) \
    എങ്കിൽ (slot_i >= 40) തിരികെ ശൂന്യ; /* this should never happen */ \
    എങ്കിൽ (type_object->sub_slot != ശൂന്യ && type_object->sub_slot->slot_name != ശൂന്യ) \
    { \
	slots[slot_i].slot = Py_##slot_name; \
	slots[slot_i].pfunc = (ശൂന്യം*)type_object->sub_slot->slot_name; \
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
    തിരികെ (PyTypeObject*)newtype;
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
    എങ്കിൽ (type##Ptr == ശൂന്യ) \
	തിരികെ -1;
# define PYTYPE_CLEANUP(type) \
    type##Ptr = ശൂന്യ;

// Limited API does not provide PyRun_* functions. Need to implement manually
// using PyCompile and PyEval.
സ്ഥിതം PyObject* Vim_PyRun_String(സ്ഥിരം അക്ഷരം *str, പൂർണ്ണസംഖ്യ start, PyObject *globals, PyObject *locals)
{
    // Just pass "" for filename for now.
    PyObject* compiled = Py_CompileString(str, "", start);
    എങ്കിൽ (compiled == ശൂന്യ)
	തിരികെ ശൂന്യ;

    PyObject* eval_result = PyEval_EvalCode(compiled, globals, locals);
    Py_DECREF(compiled);
    തിരികെ eval_result;
}
സ്ഥിതം പൂർണ്ണസംഖ്യ Vim_PyRun_SimpleString(സ്ഥിരം അക്ഷരം *str)
{
    // This function emulates CPython's implementation.
    PyObject* m = PyImport_AddModule("__main__");
    എങ്കിൽ (m == ശൂന്യ)
	തിരികെ -1;
    PyObject* d = PyModule_GetDict(m);
    PyObject* output = Vim_PyRun_String(str, Py_file_നിവേശം, d, d);
    എങ്കിൽ (output == ശൂന്യ)
    {
	PyErr_PrintEx(TRUE);
	തിരികെ -1;
    }
    Py_DECREF(output);
    തിരികെ 0;
}
# define PyRun_String Vim_PyRun_String
# define PyRun_SimpleString Vim_PyRun_SimpleString

#അല്ലെങ്കിൽ // !defined(USE_LIMITED_API)

// Full Python API. Can make use of structs and macros directly.
# define DEFINE_PY_TYPE_OBJECT(type) \
    സ്ഥിതം PyTypeObject type; \
    സ്ഥിതം PyTypeObject* type##Ptr = &type
# define PyObject_HEAD_INIT_TYPE(type) PyObject_HEAD_INIT(&type)

# define Py_TYPE_GET_TP_ALLOC(type) type->tp_alloc
# define Py_TYPE_GET_TP_METHODS(type) type->tp_methods

# define Py_TYPE_NAME(obj) ((obj)->ob_type->tp_name == ശൂന്യ \
	? "(NULL)" \
	: (obj)->ob_type->tp_name)
# define PyErr_FORMAT_TYPE(സന്ദേശം, obj) \
    PyErr_FORMAT(PyExc_TypeError, സന്ദേശം, \
	    Py_TYPE_NAME(obj))

// Add a static type
# define PYTYPE_READY(type) \
    എങ്കിൽ (PyType_Ready(type##Ptr)) \
	തിരികെ -1;

#endif


#define RAISE_NO_EMPTY_താക്കോലുകൾ PyErr_SET_STRING(Py_Exc_മൂല്യം_Error, \
					    N_("empty keys are not allowed"))
#define RAISE_LOCKED_DICTIONARY PyErr_SET_VIM(N_("dictionary is locked"))
#define RAISE_LOCKED_LIST PyErr_SET_VIM(N_("list is locked"))
#define RAISE_UNDO_FAIL PyErr_SET_VIM(N_("cannot save undo information"))
#define RAISE_DELETE_വരി_FAIL PyErr_SET_VIM(N_("cannot delete line"))
#define RAISE_തിരുകുക_വരി_FAIL PyErr_SET_VIM(N_("cannot insert line"))
#define RAISE_REPLACE_വരി_FAIL PyErr_SET_VIM(N_("cannot replace line"))
#define RAISE_താക്കോൽ_ADD_FAIL(താക്കോൽ) \
    PyErr_VIM_FORMAT(N_("failed to add key '%s' to dictionary"), താക്കോൽ)
#define RAISE_INVALID_INDEX_TYPE(idx) \
    PyErr_FORMAT_TYPE(N_("index must be int or slice, not %s"), idx);

#define INVALID_BUFFER_മൂല്യം ((buf_T *)(-1))
#define INVALID_WINDOW_മൂല്യം ((win_T *)(-1))
#define INVALID_TABPAGE_മൂല്യം ((tabpage_T *)(-1))

തരനിർവചനം ശൂന്യം (*rangeinitializer)(ശൂന്യം *);
തരനിർവചനം ശൂന്യം (*runner)(സ്ഥിരം അക്ഷരം *, dict_T *, ശൂന്യം *
#ifdef PY_CAN_RECURSE
	, PyGILState_STATE *
#endif
	);

സ്ഥിതം പൂർണ്ണസംഖ്യ ConvertFromPyObject(PyObject *, typval_T *);
സ്ഥിതം പൂർണ്ണസംഖ്യ _ConvertFromPyObject(PyObject *, typval_T *, PyObject *);
സ്ഥിതം പൂർണ്ണസംഖ്യ ConvertFromPyMapping(PyObject *, typval_T *);
സ്ഥിതം പൂർണ്ണസംഖ്യ ConvertFromPySequence(PyObject *, typval_T *);
സ്ഥിതം PyObject *WindowNew(win_T *, tabpage_T *);
സ്ഥിതം PyObject *BufferNew (buf_T *);
സ്ഥിതം PyObject *വരി_To_String(സ്ഥിരം അക്ഷരം *);

സ്ഥിതം PyInt RangeStart;
സ്ഥിതം PyInt RangeEnd;

സ്ഥിതം PyObject *globals;

സ്ഥിതം PyObject *py_chdir;
സ്ഥിതം PyObject *py_fchdir;
സ്ഥിതം PyObject *py_getcwd;
സ്ഥിതം PyObject *vim_module;
സ്ഥിതം PyObject *vim_special_path_object;

#എങ്കിൽ PY_VERSION_HEX >= 0x030700f0
സ്ഥിതം PyObject *py_find_spec;
#അല്ലെങ്കിൽ
സ്ഥിതം PyObject *py_load_module;
#endif
#എങ്കിൽ PY_VERSION_HEX < 0x30c00a7
സ്ഥിതം PyObject *py_find_module;
#endif

സ്ഥിതം PyObject *VimError;

/*
 * obtain a lock on the Vim data structures
 */
    സ്ഥിതം ശൂന്യം
Python_Lock_Vim(ശൂന്യം)
{
}

/*
 * release a lock on the Vim data structures
 */
    സ്ഥിതം ശൂന്യം
Python_Release_Vim(ശൂന്യം)
{
}

/*
 * The "todecref" argument holds a pointer to PyObject * that must be
 * DECREF'ed after returned char_u * is no longer needed or NULL if all what
 * was needed to generate returned value is object.
 *
 * Use Py_XDECREF to decrement reference count.
 */
    സ്ഥിതം char_u *
StringToChars(PyObject *obj, PyObject **todecref)
{
    char_u	*str;

    എങ്കിൽ (PyBytes_Check(obj))
    {

	എങ്കിൽ (PyBytes_AsStringAndSize(obj, (അക്ഷരം **) &str, ശൂന്യ) == -1
		|| str == ശൂന്യ)
	    തിരികെ ശൂന്യ;

	*todecref = ശൂന്യ;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (PyUnicode_Check(obj))
    {
	PyObject	*bytes;

	എങ്കിൽ (!(bytes = PyUnicode_AsEncodedString(obj, ENC_OPT,
							   ERRORS_ENCODE_ARG)))
	    തിരികെ ശൂന്യ;

	എങ്കിൽ (PyBytes_AsStringAndSize(bytes, (അക്ഷരം **) &str, ശൂന്യ) == -1
		|| str == ശൂന്യ)
	{
	    Py_DECREF(bytes);
	    തിരികെ ശൂന്യ;
	}

	*todecref = bytes;
    }
    അല്ലെങ്കിൽ
    {
#എങ്കിൽ PY_MAJOR_VERSION < 3
	PyErr_FORMAT_TYPE(N_("expected str() or unicode() instance, but got %s"),
		obj);
#അല്ലെങ്കിൽ
	PyErr_FORMAT_TYPE(N_("expected bytes() or str() instance, but got %s"),
		obj);
#endif
	തിരികെ ശൂന്യ;
    }

    തിരികെ (char_u *) str;
}

#define സംഖ്യ_LONG     1
#define സംഖ്യ_INT      2
#define സംഖ്യ_NATURAL  4
#define സംഖ്യ_UNSIGNED 8

    സ്ഥിതം പൂർണ്ണസംഖ്യ
സംഖ്യ_To_Long(PyObject *obj, long *result, പൂർണ്ണസംഖ്യ flags)
{
#എങ്കിൽ PY_MAJOR_VERSION < 3
    എങ്കിൽ (PyInt_Check(obj))
    {
	*result = PyInt_AsLong(obj);
	എങ്കിൽ (PyErr_Occurred())
	    തിരികെ -1;
    }
    അല്ലെങ്കിൽ
#endif
    എങ്കിൽ (PyLong_Check(obj))
    {
	*result = PyLong_AsLong(obj);
	എങ്കിൽ (PyErr_Occurred())
	    തിരികെ -1;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (Py_സംഖ്യ_Check(obj))
    {
	PyObject	*അക്കം;

	എങ്കിൽ (!(അക്കം = Py_സംഖ്യ_Long(obj)))
	    തിരികെ -1;

	*result = PyLong_AsLong(അക്കം);

	Py_DECREF(അക്കം);

	എങ്കിൽ (PyErr_Occurred())
	    തിരികെ -1;
    }
    അല്ലെങ്കിൽ
    {
#എങ്കിൽ PY_MAJOR_VERSION < 3
	PyErr_FORMAT_TYPE(
		N_("expected int(), long() or something supporting "
		   "coercing to long(), but got %s"),
		obj);
#അല്ലെങ്കിൽ
	PyErr_FORMAT_TYPE(
		N_("expected int() or something supporting coercing to int(), "
		   "but got %s"),
		obj);
#endif
	തിരികെ -1;
    }

    എങ്കിൽ (flags & സംഖ്യ_INT)
    {
	എങ്കിൽ (*result > INT_MAX)
	{
	    PyErr_SET_STRING(PyExc_OverflowError,
		    N_("value is too large to fit into C int type"));
	    തിരികെ -1;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (*result < INT_MIN)
	{
	    PyErr_SET_STRING(PyExc_OverflowError,
		    N_("value is too small to fit into C int type"));
	    തിരികെ -1;
	}
    }

    എങ്കിൽ (flags & സംഖ്യ_NATURAL)
    {
	എങ്കിൽ (*result <= 0)
	{
	    PyErr_SET_STRING(Py_Exc_മൂല്യം_Error,
		    N_("number must be greater than zero"));
	    തിരികെ -1;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (flags & സംഖ്യ_UNSIGNED)
    {
	എങ്കിൽ (*result < 0)
	{
	    PyErr_SET_STRING(Py_Exc_മൂല്യം_Error,
		    N_("number must be greater or equal to zero"));
	    തിരികെ -1;
	}
    }

    തിരികെ 0;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
add_string(PyObject *list, അക്ഷരം *s)
{
    PyObject	*string;

    എങ്കിൽ (!(string = PyString_FromString(s)))
	തിരികെ -1;

    എങ്കിൽ (PyList_Append(list, string))
    {
	Py_DECREF(string);
	തിരികെ -1;
    }

    Py_DECREF(string);
    തിരികെ 0;
}

    സ്ഥിതം PyObject *
ObjectDir(PyObject *self, അക്ഷരം **attributes)
{
    PyMethodDef	*method;
    അക്ഷരം	**attr;
    PyObject	*ret;

    എങ്കിൽ (!(ret = PyList_New(0)))
	തിരികെ ശൂന്യ;

    എങ്കിൽ (self)
	വേണ്ടി (method = Py_TYPE_GET_TP_METHODS(self->ob_type) ; method->ml_name != ശൂന്യ ; ++method)
	    എങ്കിൽ (add_string(ret, (അക്ഷരം *)method->ml_name))
	    {
		Py_DECREF(ret);
		തിരികെ ശൂന്യ;
	    }

    വേണ്ടി (attr = attributes ; *attr ; ++attr)
	എങ്കിൽ (add_string(ret, *attr))
	{
	    Py_DECREF(ret);
	    തിരികെ ശൂന്യ;
	}

#എങ്കിൽ PY_MAJOR_VERSION < 3
    എങ്കിൽ (add_string(ret, "__members__"))
    {
	Py_DECREF(ret);
	തിരികെ ശൂന്യ;
    }
#endif

    തിരികെ ret;
}

// Output buffer management

// Function to write a line, points to either msg() or emsg().
തരനിർവചനം പൂർണ്ണസംഖ്യ (*writefn)(അക്ഷരം *);

DEFINE_PY_TYPE_OBJECT(OutputType);

തരനിർവചനം ഘടന
{
    PyObject_HEAD
    long softspace;
    long error;
} OutputObject;

സ്ഥിതം അക്ഷരം *OutputAttrs[] = {
    "softspace",
    ശൂന്യ
};

    സ്ഥിതം PyObject *
OutputDir(PyObject *self, PyObject *args UNUSED)
{
    തിരികെ ObjectDir(self, OutputAttrs);
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
OutputSetattr(PyObject *self_obj, അക്ഷരം *name, PyObject *valObject)
{
    OutputObject *self = (OutputObject*)self_obj;
    എങ്കിൽ (valObject == ശൂന്യ)
    {
	PyErr_SET_STRING(PyExc_AttributeError,
		N_("can't delete OutputObject attributes"));
	തിരികെ -1;
    }

    എങ്കിൽ (strcmp(name, "softspace") == 0)
    {
	എങ്കിൽ (സംഖ്യ_To_Long(valObject, &(self->softspace), സംഖ്യ_UNSIGNED))
	    തിരികെ -1;
	തിരികെ 0;
    }

    PyErr_FORMAT(PyExc_AttributeError, N_("invalid attribute: %s"), name);
    തിരികെ -1;
}

// Buffer IO, we write one whole line at a time.
സ്ഥിതം garray_T io_ga = {0, 0, 1, 80, ശൂന്യ};
സ്ഥിതം writefn old_fn = ശൂന്യ;

    സ്ഥിതം ശൂന്യം
PythonIO_Flush(ശൂന്യം)
{
    എങ്കിൽ (old_fn != ശൂന്യ && io_ga.ga_len > 0)
    {
	((അക്ഷരം *)io_ga.ga_data)[io_ga.ga_len] = NUL;
	// We don't know what emsg_severe should be here, so ... hope?
	old_fn((അക്ഷരം *)io_ga.ga_data);
    }
    io_ga.ga_len = 0;
}

    സ്ഥിതം ശൂന്യം
writer(writefn fn, char_u *str, PyInt n, പൂർണ്ണസംഖ്യ severe)
{
    char_u *ptr;

    // Flush when switching output function.
    എങ്കിൽ (fn != old_fn)
	PythonIO_Flush();
    old_fn = fn;

    // Write each NL separated line.  Text after the last NL is kept for
    // writing later.
    // For normal messages: Do not output when "got_int" was set.  This avoids
    // a loop gone crazy flooding the terminal with messages.  Also for when
    // "q" is pressed at the more-prompt.
    ആകുമ്പോൾ (n > 0 && (ptr = memchr(str, '\n', n)) != ശൂന്യ
					  && (fn == (writefn)emsg || !got_int))
    {
	PyInt len = ptr - str;

	എങ്കിൽ (ga_grow(&io_ga, (പൂർണ്ണസംഖ്യ)(len + 1)) == FAIL)
	    നിർത്തുക;

	mch_memmove(((അക്ഷരം *)io_ga.ga_data) + io_ga.ga_len, str, (size_t)len);
	((അക്ഷരം *)io_ga.ga_data)[io_ga.ga_len + len] = NUL;
	emsg_severe = severe;
	fn((അക്ഷരം *)io_ga.ga_data);
	str = ptr + 1;
	n -= len + 1;
	io_ga.ga_len = 0;
    }

    // Put the remaining text into io_ga for later printing.
    എങ്കിൽ (n > 0 && (fn == (writefn)emsg || !got_int)
					&& ga_grow(&io_ga, (പൂർണ്ണസംഖ്യ)(n + 1)) == OK)
    {
	mch_memmove(((അക്ഷരം *)io_ga.ga_data) + io_ga.ga_len, str, (size_t)n);
	io_ga.ga_len += (പൂർണ്ണസംഖ്യ)n;
    }
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
write_output(OutputObject *self, PyObject *string)
{
    Py_ssize_t	len = 0;
    അക്ഷരം	*str = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		error = self->error;

    എങ്കിൽ (!PyArg_Parse(string, "et#", ENC_OPT, &str, &len))
	തിരികെ -1;

    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    writer((writefn)(error ? emsg : സന്ദേശം), (char_u *)str, len, error);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS
    PyMem_Free(str);

    തിരികെ 0;
}

    സ്ഥിതം PyObject *
OutputWrite(OutputObject *self, PyObject *string)
{
    എങ്കിൽ (write_output(self, string))
	തിരികെ ശൂന്യ;

    Py_INCREF(Py_None);
    തിരികെ Py_None;
}

    സ്ഥിതം PyObject *
OutputWritelines(OutputObject *self, PyObject *seq)
{
    PyObject	*iterator;
    PyObject	*item;

    എങ്കിൽ (!(iterator = PyObject_GetIter(seq)))
	തിരികെ ശൂന്യ;

    ആകുമ്പോൾ ((item = PyIter_Next(iterator)))
    {
	എങ്കിൽ (write_output(self, item))
	{
	    Py_DECREF(iterator);
	    Py_DECREF(item);
	    തിരികെ ശൂന്യ;
	}
	Py_DECREF(item);
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    എങ്കിൽ (PyErr_Occurred())
	തിരികെ ശൂന്യ;

    Py_INCREF(Py_None);
    തിരികെ Py_None;
}

    സ്ഥിതം PyObject *
AlwaysNone(PyObject *self UNUSED, PyObject *args UNUSED)
{
    // do nothing
    Py_INCREF(Py_None);
    തിരികെ Py_None;
}
#define ALWAYS_NONE AlwaysNone(ശൂന്യ, ശൂന്യ)

    സ്ഥിതം PyObject *
AlwaysFalse(PyObject *self UNUSED, PyObject *args UNUSED)
{
    // do nothing
    PyObject	*ret = Py_False;
    Py_INCREF(ret);
    തിരികെ ret;
}
#define ALWAYS_FALSE AlwaysFalse(ശൂന്യ, ശൂന്യ)

    സ്ഥിതം PyObject *
AlwaysTrue(PyObject *self UNUSED, PyObject *args UNUSED)
{
    // do nothing
    PyObject	*ret = Py_True;
    Py_INCREF(ret);
    തിരികെ ret;
}
#define ALWAYS_TRUE AlwaysTrue(ശൂന്യ, ശൂന്യ)

/***************/

സ്ഥിതം ഘടന PyMethodDef OutputMethods[] = {
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
    { ശൂന്യ,	    ശൂന്യ,				0,		ശൂന്യ}
};

സ്ഥിതം OutputObject Output =
{
    PyObject_HEAD_INIT_TYPE(OutputType)
    0,
    0
};

സ്ഥിതം OutputObject Error =
{
    PyObject_HEAD_INIT_TYPE(OutputType)
    0,
    1
};

    സ്ഥിതം പൂർണ്ണസംഖ്യ
PythonIO_Init_io(ശൂന്യം)
{
    എങ്കിൽ (PySys_SetObject("stdout", (PyObject *)(ശൂന്യം *)&Output))
	തിരികെ -1;
    എങ്കിൽ (PySys_SetObject("stderr", (PyObject *)(ശൂന്യം *)&Error))
	തിരികെ -1;

    എങ്കിൽ (PyErr_Occurred())
    {
	emsg(_(e_python_error_initialising_io_object));
	തിരികെ -1;
    }

    തിരികെ 0;
}

#എങ്കിൽ PY_VERSION_HEX < 0x030700f0
സ്ഥിതം PyObject *call_load_module(അക്ഷരം *name, പൂർണ്ണസംഖ്യ len, PyObject *find_module_result);

തരനിർവചനം ഘടന
{
    PyObject_HEAD
    അക്ഷരം	*fullname;
    PyObject	*result;
} LoaderObject;
DEFINE_PY_TYPE_OBJECT(LoaderType);

    സ്ഥിതം ശൂന്യം
LoaderDestructor(PyObject *self_obj)
{
    LoaderObject *self = (LoaderObject*)self_obj;
    vim_free(self->fullname);
    Py_XDECREF(self->result);
    DESTRUCTOR_FINISH(self);
}

    സ്ഥിതം PyObject *
LoaderLoadModule(LoaderObject *self, PyObject *args UNUSED)
{
    അക്ഷരം	*fullname = self->fullname;
    PyObject	*result = self->result;
    PyObject	*module;

    എങ്കിൽ (!fullname)
    {
	module = result ? result : Py_None;
	Py_INCREF(module);
	തിരികെ module;
    }

    module = call_load_module(fullname, (പൂർണ്ണസംഖ്യ)STRLEN(fullname), result);

    self->fullname = ശൂന്യ;
    self->result = module;

    vim_free(fullname);
    Py_DECREF(result);

    എങ്കിൽ (!module)
    {
	എങ്കിൽ (PyErr_Occurred())
	    തിരികെ ശൂന്യ;

	Py_INCREF(Py_None);
	തിരികെ Py_None;
    }

    Py_INCREF(module);
    തിരികെ module;
}

സ്ഥിതം ഘടന PyMethodDef LoaderMethods[] = {
    // name,	    function,				calling,	doc
    {"load_module", (PyCFunction)LoaderLoadModule,	METH_VARARGS,	""},
    { ശൂന്യ,	    ശൂന്യ,				0,		ശൂന്യ}
};
#endif

/*
 * Check to see whether a Vim error has been reported, or a keyboard
 * interrupt has been detected.
 */
    സ്ഥിതം ശൂന്യം
VimTryStart(ശൂന്യം)
{
    ++trylevel;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
VimTryEnd(ശൂന്യം)
{
    --trylevel;
    // Without this it stops processing all subsequent Vim script commands and
    // generates strange error messages if I e.g. try calling Test() in a cycle
    did_emsg = FALSE;
    // Keyboard interrupt should be preferred over anything else
    എങ്കിൽ (got_int)
    {
	എങ്കിൽ (did_throw)
	    discard_current_exception();
	got_int = FALSE;
	PyErr_SetNone(PyExc_KeyboardInterrupt);
	തിരികെ -1;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (സന്ദേശം_list != ശൂന്യ && *സന്ദേശം_list != ശൂന്യ)
    {
	പൂർണ്ണസംഖ്യ	should_free;
	അക്ഷരം	*സന്ദേശം;

	സന്ദേശം = get_exception_string(*സന്ദേശം_list, ET_ERROR, ശൂന്യ, &should_free);

	എങ്കിൽ (സന്ദേശം == ശൂന്യ)
	{
	    PyErr_NoMemory();
	    തിരികെ -1;
	}

	PyErr_SetVim(സന്ദേശം);

	free_global_msglist();

	എങ്കിൽ (should_free)
	    vim_free(സന്ദേശം);

	തിരികെ -1;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (!did_throw)
	തിരികെ (PyErr_Occurred() ? -1 : 0);
    // Python exception is preferred over Vim one; unlikely to occur though
    അല്ലെങ്കിൽ എങ്കിൽ (PyErr_Occurred())
    {
	discard_current_exception();
	തിരികെ -1;
    }
    // Finally transform Vim script exception to python one
    അല്ലെങ്കിൽ
    {
	PyErr_SetVim((അക്ഷരം *)current_exception->മൂല്യം);
	discard_current_exception();
	തിരികെ -1;
    }
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
VimCheckInterrupt(ശൂന്യം)
{
    എങ്കിൽ (got_int)
    {
	PyErr_SetNone(PyExc_KeyboardInterrupt);
	തിരികെ 1;
    }
    തിരികെ 0;
}

// Vim module - Implementation

    സ്ഥിതം PyObject *
VimCommand(PyObject *self UNUSED, PyObject *string)
{
    char_u	*cmd;
    PyObject	*ret;
    PyObject	*todecref;

    എങ്കിൽ (!(cmd = StringToChars(string, &todecref)))
	തിരികെ ശൂന്യ;

    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();

    VimTryStart();
    do_cmdline_cmd(cmd);
    update_screen(UPD_VALID);

    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    എങ്കിൽ (VimTryEnd())
	ret = ശൂന്യ;
    അല്ലെങ്കിൽ
	ret = Py_None;

    Py_XINCREF(ret);
    Py_XDECREF(todecref);
    തിരികെ ret;
}

/*
 * Function to translate a typval_T into a PyObject; this will recursively
 * translate lists/dictionaries into their Python equivalents.
 *
 * The depth parameter is to avoid infinite recursion, set it to 1 when
 * you call VimToPython.
 */
    സ്ഥിതം PyObject *
VimToPython(typval_T *our_tv, പൂർണ്ണസംഖ്യ depth, PyObject *lookup_dict)
{
    PyObject	*ret;
    PyObject	*newObj;
    അക്ഷരം	ptrBuf[വലുപ്പം(ശൂന്യം *) * 2 + 3];

    // Avoid infinite recursion
    എങ്കിൽ (depth > 100)
    {
	Py_INCREF(Py_None);
	ret = Py_None;
	തിരികെ ret;
    }

    // Check if we run into a recursive loop.  The item must be in lookup_dict
    // then and we can use it again.
    എങ്കിൽ ((our_tv->v_type == VAR_LIST && our_tv->vval.v_list != ശൂന്യ)
	    || (our_tv->v_type == VAR_TUPLE && our_tv->vval.v_tuple != ശൂന്യ)
	    || (our_tv->v_type == VAR_DICT && our_tv->vval.v_dict != ശൂന്യ))
    {
	sprintf(ptrBuf, "%p",
		our_tv->v_type == VAR_LIST ? (ശൂന്യം *)our_tv->vval.v_list
		    : our_tv->v_type == VAR_TUPLE ?
			(ശൂന്യം *)our_tv->vval.v_tuple
			: (ശൂന്യം *)our_tv->vval.v_dict);

	എങ്കിൽ ((ret = PyDict_GetItemString(lookup_dict, ptrBuf)))
	{
	    Py_INCREF(ret);
	    തിരികെ ret;
	}
    }

    എങ്കിൽ (our_tv->v_type == VAR_STRING)
	ret = PyString_FromString(our_tv->vval.v_string == ശൂന്യ
					? "" : (അക്ഷരം *)our_tv->vval.v_string);
    അല്ലെങ്കിൽ എങ്കിൽ (our_tv->v_type == VAR_സംഖ്യ)
    {
	അക്ഷരം buf[NUMBUFLEN];

	// For backwards compatibility numbers are stored as strings.
	sprintf(buf, "%lld", (long long)our_tv->vval.v_സംഖ്യ);
	ret = PyString_FromString((അക്ഷരം *)buf);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (our_tv->v_type == VAR_FLOAT)
    {
	അക്ഷരം buf[NUMBUFLEN];

	sprintf(buf, "%f", our_tv->vval.v_float);
	ret = PyString_FromString((അക്ഷരം *)buf);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (our_tv->v_type == VAR_LIST)
    {
	list_T		*list = our_tv->vval.v_list;
	listitem_T	*curr;

	എങ്കിൽ (list == ശൂന്യ)
	    തിരികെ ശൂന്യ;

	എങ്കിൽ (!(ret = PyList_New(0)))
	    തിരികെ ശൂന്യ;

	എങ്കിൽ (PyDict_SetItemString(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    തിരികെ ശൂന്യ;
	}

	CHECK_LIST_MATERIALIZE(list);
	FOR_ALL_LIST_ഇനങ്ങൾ(list, curr)
	{
	    എങ്കിൽ (!(newObj = VimToPython(&curr->li_tv, depth + 1, lookup_dict)))
	    {
		Py_DECREF(ret);
		തിരികെ ശൂന്യ;
	    }
	    എങ്കിൽ (PyList_Append(ret, newObj))
	    {
		Py_DECREF(newObj);
		Py_DECREF(ret);
		തിരികെ ശൂന്യ;
	    }
	    Py_DECREF(newObj);
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (our_tv->v_type == VAR_TUPLE)
    {
	tuple_T	*tuple = our_tv->vval.v_tuple;
	പൂർണ്ണസംഖ്യ	len;

	എങ്കിൽ (tuple == ശൂന്യ)
	    തിരികെ ശൂന്യ;

	len = TUPLE_LEN(tuple);

	ret = PyTuple_New(len);
	എങ്കിൽ (ret == ശൂന്യ)
	    തിരികെ ശൂന്യ;

	വേണ്ടി (പൂർണ്ണസംഖ്യ idx = 0; idx < len; idx++)
	{
	    typval_T	*item_tv = TUPLE_ITEM(tuple, idx);

	    newObj = VimToPython(item_tv, depth + 1, lookup_dict);
	    എങ്കിൽ (!newObj)
	    {
		Py_DECREF(ret);
		തിരികെ ശൂന്യ;
	    }
	    PyTuple_SetItem(ret, idx, newObj);
	}

	എങ്കിൽ (PyDict_SetItemString(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    തിരികെ ശൂന്യ;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (our_tv->v_type == VAR_DICT)
    {

	hashtab_T	*ht;
	long_u		todo;
	hashitem_T	*hi;
	dictitem_T	*di;

	എങ്കിൽ (our_tv->vval.v_dict == ശൂന്യ)
	    തിരികെ ശൂന്യ;
	ht = &our_tv->vval.v_dict->dv_hashtab;

	എങ്കിൽ (!(ret = PyDict_New()))
	    തിരികെ ശൂന്യ;

	എങ്കിൽ (PyDict_SetItemString(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    തിരികെ ശൂന്യ;
	}

	todo = ht->ht_used;
	വേണ്ടി (hi = ht->ht_array; todo > 0; ++hi)
	{
	    എങ്കിൽ (!HASHITEM_EMPTY(hi))
	    {
		--todo;

		di = dict_lookup(hi);
		എങ്കിൽ (!(newObj = VimToPython(&di->di_tv, depth + 1, lookup_dict)))
		{
		    Py_DECREF(ret);
		    തിരികെ ശൂന്യ;
		}
		എങ്കിൽ (PyDict_SetItemString(ret, (അക്ഷരം *)hi->hi_താക്കോൽ, newObj))
		{
		    Py_DECREF(ret);
		    Py_DECREF(newObj);
		    തിരികെ ശൂന്യ;
		}
	    }
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (our_tv->v_type == VAR_BOOL)
    {
	എങ്കിൽ (our_tv->vval.v_സംഖ്യ == VVAL_FALSE)
	{
	    ret = Py_False;
	    Py_INCREF(ret);
	}
	അല്ലെങ്കിൽ
	{
	    ret = Py_True;
	    Py_INCREF(ret);
	}
	തിരികെ ret;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (our_tv->v_type == VAR_SPECIAL)
    {
	Py_INCREF(Py_None);
	ret = Py_None;
	തിരികെ ret;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (our_tv->v_type == VAR_BLOB)
	ret = PyBytes_FromStringAndSize(
		(അക്ഷരം*) our_tv->vval.v_blob->bv_ga.ga_data,
		(Py_ssize_t) our_tv->vval.v_blob->bv_ga.ga_len);
    അല്ലെങ്കിൽ
    {
	Py_INCREF(Py_None);
	ret = Py_None;
    }

    തിരികെ ret;
}

    സ്ഥിതം PyObject *
VimEval(PyObject *self UNUSED, PyObject *args)
{
    char_u	*വ്യഞ്ജകം;
    typval_T	*our_tv;
    PyObject	*string;
    PyObject	*todecref;
    PyObject	*ret;
    PyObject	*lookup_dict;

    എങ്കിൽ (!PyArg_ParseTuple(args, "O", &string))
	തിരികെ ശൂന്യ;

    എങ്കിൽ (!(വ്യഞ്ജകം = StringToChars(string, &todecref)))
	തിരികെ ശൂന്യ;

    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    VimTryStart();
    our_tv = eval_വ്യഞ്ജകം(വ്യഞ്ജകം, ശൂന്യ);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    Py_XDECREF(todecref);

    എങ്കിൽ (VimTryEnd())
	തിരികെ ശൂന്യ;

    എങ്കിൽ (our_tv == ശൂന്യ)
    {
	PyErr_SET_VIM(N_("invalid expression"));
	തിരികെ ശൂന്യ;
    }

    // Convert the Vim type into a Python type.  Create a dictionary that's
    // used to check for recursive loops.
    എങ്കിൽ (!(lookup_dict = PyDict_New()))
	ret = ശൂന്യ;
    അല്ലെങ്കിൽ
    {
	ret = VimToPython(our_tv, 1, lookup_dict);
	Py_DECREF(lookup_dict);
    }


    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    free_tv(our_tv);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    തിരികെ ret;
}

സ്ഥിതം PyObject *ConvertToPyObject(typval_T *);

    സ്ഥിതം PyObject *
VimEvalPy(PyObject *self UNUSED, PyObject *string)
{
    typval_T	*our_tv;
    PyObject	*ret;
    char_u	*വ്യഞ്ജകം;
    PyObject	*todecref;

    എങ്കിൽ (!(വ്യഞ്ജകം = StringToChars(string, &todecref)))
	തിരികെ ശൂന്യ;

    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    VimTryStart();
    our_tv = eval_വ്യഞ്ജകം(വ്യഞ്ജകം, ശൂന്യ);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    Py_XDECREF(todecref);

    എങ്കിൽ (VimTryEnd())
	തിരികെ ശൂന്യ;

    എങ്കിൽ (our_tv == ശൂന്യ)
    {
	PyErr_SET_VIM(N_("invalid expression"));
	തിരികെ ശൂന്യ;
    }

    ret = ConvertToPyObject(our_tv);
    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    free_tv(our_tv);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    തിരികെ ret;
}

    സ്ഥിതം PyObject *
VimStrwidth(PyObject *self UNUSED, PyObject *string)
{
    char_u	*str;
    PyObject	*todecref;
    പൂർണ്ണസംഖ്യ		len;

    എങ്കിൽ (!(str = StringToChars(string, &todecref)))
	തിരികെ ശൂന്യ;

    len = mb_string2cells(str, (പൂർണ്ണസംഖ്യ)STRLEN(str));

    Py_XDECREF(todecref);

    തിരികെ PyLong_FromLong(len);
}

    സ്ഥിതം PyObject *
_VimChdir(PyObject *_chdir, PyObject *args, PyObject *kwargs)
{
    PyObject	*ret;
    PyObject	*newwd;
    PyObject	*todecref;
    char_u	*new_dir;

    എങ്കിൽ (_chdir == ശൂന്യ)
	തിരികെ ശൂന്യ;
    എങ്കിൽ (!(ret = PyObject_Call(_chdir, args, kwargs)))
	തിരികെ ശൂന്യ;

    എങ്കിൽ (!(newwd = PyObject_CallFunctionObjArgs(py_getcwd, ശൂന്യ)))
    {
	Py_DECREF(ret);
	തിരികെ ശൂന്യ;
    }

    എങ്കിൽ (!(new_dir = StringToChars(newwd, &todecref)))
    {
	Py_DECREF(ret);
	Py_DECREF(newwd);
	തിരികെ ശൂന്യ;
    }

    VimTryStart();

    എങ്കിൽ (vim_chdir(new_dir))
    {
	Py_DECREF(ret);
	Py_DECREF(newwd);
	Py_XDECREF(todecref);

	എങ്കിൽ (VimTryEnd())
	    തിരികെ ശൂന്യ;

	PyErr_SET_VIM(N_("failed to change directory"));
	തിരികെ ശൂന്യ;
    }

    Py_DECREF(newwd);
    Py_XDECREF(todecref);

    post_chdir(CDSCOPE_GLOBAL);

    എങ്കിൽ (VimTryEnd())
    {
	Py_DECREF(ret);
	തിരികെ ശൂന്യ;
    }

    തിരികെ ret;
}

    സ്ഥിതം PyObject *
VimChdir(PyObject *self UNUSED, PyObject *args, PyObject *kwargs)
{
    തിരികെ _VimChdir(py_chdir, args, kwargs);
}

    സ്ഥിതം PyObject *
VimFchdir(PyObject *self UNUSED, PyObject *args, PyObject *kwargs)
{
    തിരികെ _VimChdir(py_fchdir, args, kwargs);
}

തരനിർവചനം ഘടന {
    PyObject	*callable;
    PyObject	*result;
} map_rtp_data;

    സ്ഥിതം ശൂന്യം
map_rtp_callback(char_u *path, ശൂന്യം *_data)
{
    ശൂന്യം	**data = (ശൂന്യം **) _data;
    PyObject	*pathObject;
    map_rtp_data	*mr_data = *((map_rtp_data **) data);

    എങ്കിൽ (!(pathObject = PyString_FromString((അക്ഷരം *)path)))
    {
	*data = ശൂന്യ;
	തിരികെ;
    }

    mr_data->result = PyObject_CallFunctionObjArgs(mr_data->callable,
						   pathObject, ശൂന്യ);

    Py_DECREF(pathObject);

    എങ്കിൽ (!mr_data->result || mr_data->result != Py_None)
	*data = ശൂന്യ;
    അല്ലെങ്കിൽ
    {
	Py_DECREF(mr_data->result);
	mr_data->result = ശൂന്യ;
    }
}

    സ്ഥിതം PyObject *
VimForeachRTP(PyObject *self UNUSED, PyObject *callable)
{
    map_rtp_data	data;

    data.callable = callable;
    data.result = ശൂന്യ;

    do_in_runtimepath(ശൂന്യ, 0, &map_rtp_callback, &data);

    എങ്കിൽ (data.result == ശൂന്യ)
    {
	എങ്കിൽ (PyErr_Occurred())
	    തിരികെ ശൂന്യ;
	അല്ലെങ്കിൽ
	{
	    Py_INCREF(Py_None);
	    തിരികെ Py_None;
	}
    }
    തിരികെ data.result;
}

/*
 * _vim_runtimepath_ special path implementation.
 */

    സ്ഥിതം ശൂന്യം
map_finder_callback(char_u *path, ശൂന്യം *_data)
{
    ശൂന്യം	**data = (ശൂന്യം **) _data;
    PyObject	*list = *((PyObject **) data);
    PyObject	*pathObject1, *pathObject2;
    അക്ഷരം	*pathbuf;
    size_t	pathlen;

    pathlen = STRLEN(path);

#എങ്കിൽ PY_MAJOR_VERSION < 3
# define PY_MAIN_DIR_STRING "python2"
#അല്ലെങ്കിൽ
# define PY_MAIN_DIR_STRING "python3"
#endif
#define PY_ALTERNATE_DIR_STRING "pythonx"

#define PYTHONX_STRING_LENGTH 7 // STRLEN("pythonx")
    എങ്കിൽ (!(pathbuf = PyMem_New(അക്ഷരം,
		    pathlen + STRLEN(PATHSEPSTR) + PYTHONX_STRING_LENGTH + 1)))
    {
	PyErr_NoMemory();
	*data = ശൂന്യ;
	തിരികെ;
    }

    mch_memmove(pathbuf, path, pathlen + 1);
    add_pathsep((char_u *) pathbuf);

    pathlen = STRLEN(pathbuf);
    mch_memmove(pathbuf + pathlen, PY_MAIN_DIR_STRING,
	    PYTHONX_STRING_LENGTH + 1);

    എങ്കിൽ (!(pathObject1 = PyString_FromString(pathbuf)))
    {
	*data = ശൂന്യ;
	PyMem_Free(pathbuf);
	തിരികെ;
    }

    mch_memmove(pathbuf + pathlen, PY_ALTERNATE_DIR_STRING,
	    PYTHONX_STRING_LENGTH + 1);

    എങ്കിൽ (!(pathObject2 = PyString_FromString(pathbuf)))
    {
	Py_DECREF(pathObject1);
	PyMem_Free(pathbuf);
	*data = ശൂന്യ;
	തിരികെ;
    }

    PyMem_Free(pathbuf);

    എങ്കിൽ (PyList_Append(list, pathObject1)
	    || PyList_Append(list, pathObject2))
	*data = ശൂന്യ;

    Py_DECREF(pathObject1);
    Py_DECREF(pathObject2);
}

    സ്ഥിതം PyObject *
Vim_GetPaths(PyObject *self UNUSED, PyObject *args UNUSED)
{
    PyObject	*ret;

    എങ്കിൽ (!(ret = PyList_New(0)))
	തിരികെ ശൂന്യ;

    do_in_runtimepath(ശൂന്യ, 0, &map_finder_callback, ret);

    എങ്കിൽ (PyErr_Occurred())
    {
	Py_DECREF(ret);
	തിരികെ ശൂന്യ;
    }

    തിരികെ ret;
}

#എങ്കിൽ PY_VERSION_HEX >= 0x030700f0
    സ്ഥിതം PyObject *
FinderFindSpec(PyObject *self, PyObject *args)
{
    അക്ഷരം	*fullname;
    PyObject	*paths;
    PyObject	*target = Py_None;
    PyObject	*spec;

    എങ്കിൽ (!PyArg_ParseTuple(args, "s|O", &fullname, &target))
	തിരികെ ശൂന്യ;

    എങ്കിൽ (!(paths = Vim_GetPaths(self, ശൂന്യ)))
	തിരികെ ശൂന്യ;

    spec = PyObject_CallFunction(py_find_spec, "sOO", fullname, paths, target);

    Py_DECREF(paths);

    എങ്കിൽ (!spec)
    {
	എങ്കിൽ (PyErr_Occurred())
	    തിരികെ ശൂന്യ;

	Py_INCREF(Py_None);
	തിരികെ Py_None;
    }

    തിരികെ spec;
}

    സ്ഥിതം PyObject *
FinderFindModule(PyObject* self UNUSED, PyObject* args UNUSED)
{
    // Apparently returning None works.
    Py_INCREF(Py_None);
    തിരികെ Py_None;
}
#അല്ലെങ്കിൽ
    സ്ഥിതം PyObject *
call_load_module(അക്ഷരം *name, പൂർണ്ണസംഖ്യ len, PyObject *find_module_result)
{
    PyObject	*fd, *pathname, *description;

    എങ്കിൽ (!PyTuple_Check(find_module_result))
    {
	PyErr_FORMAT_TYPE(
		N_("expected 3-tuple as imp.find_module() result, but got %s"),
		find_module_result);
	തിരികെ ശൂന്യ;
    }
    എങ്കിൽ (PyTuple_GET_SIZE(find_module_result) != 3)
    {
	PyErr_FORMAT(PyExc_TypeError,
		N_("expected 3-tuple as imp.find_module() result, but got "
		   "tuple of size %d"),
		(പൂർണ്ണസംഖ്യ) PyTuple_GET_SIZE(find_module_result));
	തിരികെ ശൂന്യ;
    }

    എങ്കിൽ (!(fd = PyTuple_GET_ITEM(find_module_result, 0))
	    || !(pathname = PyTuple_GET_ITEM(find_module_result, 1))
	    || !(description = PyTuple_GET_ITEM(find_module_result, 2)))
    {
	PyErr_SET_STRING(PyExc_RuntimeError,
		N_("internal error: imp.find_module returned tuple with NULL"));
	തിരികെ ശൂന്യ;
    }

    തിരികെ PyObject_CallFunction(py_load_module,
	    "s#OOO", name, len, fd, pathname, description);
}

    സ്ഥിതം PyObject *
find_module(അക്ഷരം *fullname, അക്ഷരം *tail, PyObject *new_path)
{
    PyObject	*find_module_result;
    PyObject	*module;
    അക്ഷരം	*dot;

    എങ്കിൽ ((dot = (അക്ഷരം *)vim_strchr((char_u *) tail, '.')))
    {
	/*
	 * There is a dot in the name: call find_module recursively without the
	 * first component
	 */
	PyObject	*newest_path;
	പൂർണ്ണസംഖ്യ		partlen = (പൂർണ്ണസംഖ്യ) (dot - 1 - tail);

	എങ്കിൽ (!(find_module_result = PyObject_CallFunction(py_find_module,
			"s#O", tail, partlen, new_path)))
	{
	    എങ്കിൽ (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_ImportError))
		PyErr_Clear();
	    തിരികെ ശൂന്യ;
	}

	എങ്കിൽ (!(module = call_load_module(
			fullname,
			((പൂർണ്ണസംഖ്യ) (tail - fullname)) + partlen,
			find_module_result)))
	{
	    Py_DECREF(find_module_result);
	    തിരികെ ശൂന്യ;
	}

	Py_DECREF(find_module_result);

	എങ്കിൽ (!(newest_path = PyObject_GetAttrString(module, "__path__")))
	{
	    Py_DECREF(module);
	    തിരികെ ശൂന്യ;
	}

	Py_DECREF(module);

	find_module_result = find_module(fullname, dot + 1, newest_path);

	Py_DECREF(newest_path);

	തിരികെ find_module_result;
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (!(find_module_result = PyObject_CallFunction(py_find_module,
			"sO", tail, new_path)))
	{
	    എങ്കിൽ (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_ImportError))
		PyErr_Clear();
	    തിരികെ ശൂന്യ;
	}

	തിരികെ find_module_result;
    }
}

    സ്ഥിതം PyObject *
FinderFindModule(PyObject *self, PyObject *args)
{
    അക്ഷരം	*fullname;
    PyObject	*result;
    PyObject	*new_path;
    LoaderObject	*loader;

    എങ്കിൽ (!PyArg_ParseTuple(args, "s", &fullname))
	തിരികെ ശൂന്യ;

    എങ്കിൽ (!(new_path = Vim_GetPaths(self, ശൂന്യ)))
	തിരികെ ശൂന്യ;

    result = find_module(fullname, fullname, new_path);

    Py_DECREF(new_path);

    എങ്കിൽ (!result)
    {
	എങ്കിൽ (PyErr_Occurred())
	    തിരികെ ശൂന്യ;

	Py_INCREF(Py_None);
	തിരികെ Py_None;
    }

    എങ്കിൽ (!(fullname = (അക്ഷരം *)vim_strsave((char_u *)fullname)))
    {
	Py_DECREF(result);
	PyErr_NoMemory();
	തിരികെ ശൂന്യ;
    }

    എങ്കിൽ (!(loader = PyObject_NEW(LoaderObject, LoaderTypePtr)))
    {
	vim_free(fullname);
	Py_DECREF(result);
	തിരികെ ശൂന്യ;
    }

    loader->fullname = fullname;
    loader->result = result;

    തിരികെ (PyObject *) loader;
}
#endif

    സ്ഥിതം PyObject *
VimPathHook(PyObject *self UNUSED, PyObject *args)
{
    അക്ഷരം	*path;

    എങ്കിൽ (PyArg_ParseTuple(args, "s", &path)
	    && STRCMP(path, vim_special_path) == 0)
    {
	Py_INCREF(vim_module);
	തിരികെ vim_module;
    }

    PyErr_Clear();
    PyErr_SetNone(PyExc_ImportError);
    തിരികെ ശൂന്യ;
}

/*
 * Vim module - Definitions
 */

സ്ഥിതം ഘടന PyMethodDef VimMethods[] = {
    // name,	    function,			calling,			documentation
    {"command",	    VimCommand,			METH_O,				"Execute a Vim ex-mode command" },
    {"eval",	    VimEval,			METH_VARARGS,			"Evaluate an expression using Vim evaluator" },
    {"bindeval",    VimEvalPy,			METH_O,				"Like eval(), but returns objects attached to Vim ones"},
    {"strwidth",    VimStrwidth,		METH_O,				"Screen string width, counts <Tab> as having width 1"},
    {"chdir",	    (PyCFunction)(ശൂന്യം *)VimChdir,	METH_VARARGS|METH_KEYWORDS,	"Change directory"},
    {"fchdir",	    (PyCFunction)(ശൂന്യം *)VimFchdir,	METH_VARARGS|METH_KEYWORDS,	"Change directory"},
    {"foreach_rtp", VimForeachRTP,		METH_O,				"Call given callable for each path in &rtp"},
#എങ്കിൽ PY_VERSION_HEX >= 0x030700f0
    {"find_spec",   FinderFindSpec,		METH_VARARGS,			"Internal use only, returns spec object for any input it receives"},
#endif
    {"find_module", FinderFindModule,		METH_VARARGS,			"Internal use only, returns loader object for any input it receives"},
    {"path_hook",   VimPathHook,		METH_VARARGS,			"Hook function to install in sys.path_hooks"},
    {"_get_paths",  (PyCFunction)Vim_GetPaths,	METH_NOARGS,			"Get &rtp-based additions to sys.path"},
    { ശൂന്യ,	    ശൂന്യ,			0,				ശൂന്യ}
};

/*
 * Generic iterator object
 */

DEFINE_PY_TYPE_OBJECT(IterType);

തരനിർവചനം PyObject *(*nextfun)(ശൂന്യം **);
തരനിർവചനം ശൂന്യം (*destructorfun)(ശൂന്യം *);
തരനിർവചനം പൂർണ്ണസംഖ്യ (*traversefun)(ശൂന്യം *, visitproc, ശൂന്യം *);
തരനിർവചനം പൂർണ്ണസംഖ്യ (*clearfun)(ശൂന്യം **);

// Main purpose of this object is removing the need for do python
// initialization (i.e. PyType_Ready and setting type attributes) for a big
// bunch of objects.
തരനിർവചനം ഘടന
{
    PyObject_HEAD
    ശൂന്യം *cur;
    nextfun next;
    destructorfun destruct;
    traversefun traverse;
    clearfun clear;
    PyObject *iter_object;
} IterObject;

    സ്ഥിതം PyObject *
IterNew(ശൂന്യം *start, destructorfun destruct, nextfun next, traversefun traverse,
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

    എങ്കിൽ (iter_object)
	Py_INCREF(iter_object);

    തിരികെ (PyObject *)(self);
}

    സ്ഥിതം ശൂന്യം
IterDestructor(PyObject *self_obj)
{
    IterObject *self = (IterObject*)self_obj;
    എങ്കിൽ (self->iter_object)
	Py_DECREF(self->iter_object);
    PyObject_GC_UnTrack((ശൂന്യം *)(self));
    self->destruct(self->cur);
    PyObject_GC_Del((ശൂന്യം *)(self));
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
IterTraverse(PyObject *self_obj, visitproc visit, ശൂന്യം *arg)
{
    IterObject *self = (IterObject*)self_obj;
    എങ്കിൽ (self->traverse != ശൂന്യ)
	തിരികെ self->traverse(self->cur, visit, arg);
    അല്ലെങ്കിൽ
	തിരികെ 0;
}

// Mac OSX defines clear() somewhere.
#ifdef clear
# undef clear
#endif

    സ്ഥിതം പൂർണ്ണസംഖ്യ
IterClear(PyObject *self_obj)
{
    IterObject *self = (IterObject*)self_obj;
    എങ്കിൽ (self->clear != ശൂന്യ)
	തിരികെ self->clear(&self->cur);
    അല്ലെങ്കിൽ
	തിരികെ 0;
}

    സ്ഥിതം PyObject *
IterNext(PyObject *self_obj)
{
    IterObject *self = (IterObject*)self_obj;
    തിരികെ self->next(&self->cur);
}

    സ്ഥിതം PyObject *
IterIter(PyObject *self)
{
    Py_INCREF(self);
    തിരികെ self;
}

തരനിർവചനം ഘടന pylinkedlist_S {
    ഘടന pylinkedlist_S	*pll_next;
    ഘടന pylinkedlist_S	*pll_prev;
    PyObject			*pll_obj;
} pylinkedlist_T;

സ്ഥിതം pylinkedlist_T *lastdict = ശൂന്യ;
സ്ഥിതം pylinkedlist_T *lastlist = ശൂന്യ;
സ്ഥിതം pylinkedlist_T *lasttuple = ശൂന്യ;
സ്ഥിതം pylinkedlist_T *lastfunc = ശൂന്യ;

    സ്ഥിതം ശൂന്യം
pyll_remove(pylinkedlist_T *ref, pylinkedlist_T **last)
{
    എങ്കിൽ (ref->pll_prev == ശൂന്യ)
    {
	എങ്കിൽ (ref->pll_next == ശൂന്യ)
	{
	    *last = ശൂന്യ;
	    തിരികെ;
	}
    }
    അല്ലെങ്കിൽ
	ref->pll_prev->pll_next = ref->pll_next;

    എങ്കിൽ (ref->pll_next == ശൂന്യ)
	*last = ref->pll_prev;
    അല്ലെങ്കിൽ
	ref->pll_next->pll_prev = ref->pll_prev;
}

    സ്ഥിതം ശൂന്യം
pyll_add(PyObject *self, pylinkedlist_T *ref, pylinkedlist_T **last)
{
    എങ്കിൽ (*last == ശൂന്യ)
	ref->pll_prev = ശൂന്യ;
    അല്ലെങ്കിൽ
    {
	(*last)->pll_next = ref;
	ref->pll_prev = *last;
    }
    ref->pll_next = ശൂന്യ;
    ref->pll_obj = self;
    *last = ref;
}

DEFINE_PY_TYPE_OBJECT(DictionaryType);

തരനിർവചനം ഘടന
{
    PyObject_HEAD
    dict_T	*dict;
    pylinkedlist_T	ref;
} DictionaryObject;

സ്ഥിതം PyObject *DictionaryUpdate(DictionaryObject *, PyObject *, PyObject *);

#define NEW_DICTIONARY(dict) DictionaryNew(DictionaryTypePtr, dict)

    സ്ഥിതം PyObject *
DictionaryNew(PyTypeObject *subtype, dict_T *dict)
{
    DictionaryObject	*self;

    self = (DictionaryObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    എങ്കിൽ (self == ശൂന്യ)
	തിരികെ ശൂന്യ;
    self->dict = dict;
    ++dict->dv_refcount;

    pyll_add((PyObject *)(self), &self->ref, &lastdict);

    തിരികെ (PyObject *)(self);
}

    സ്ഥിതം dict_T *
py_dict_alloc(ശൂന്യം)
{
    dict_T	*ret;

    എങ്കിൽ (!(ret = dict_alloc()))
    {
	PyErr_NoMemory();
	തിരികെ ശൂന്യ;
    }
    ++ret->dv_refcount;

    തിരികെ ret;
}

    സ്ഥിതം PyObject *
DictionaryConstructor(PyTypeObject *subtype, PyObject *args, PyObject *kwargs)
{
    DictionaryObject	*self;
    dict_T	*dict;

    എങ്കിൽ (!(dict = py_dict_alloc()))
	തിരികെ ശൂന്യ;

    self = (DictionaryObject *) DictionaryNew(subtype, dict);

    --dict->dv_refcount;

    എങ്കിൽ (kwargs || PyTuple_Size(args))
    {
	PyObject	*tmp;
	എങ്കിൽ (!(tmp = DictionaryUpdate(self, args, kwargs)))
	{
	    Py_DECREF(self);
	    തിരികെ ശൂന്യ;
	}

	Py_DECREF(tmp);
    }

    തിരികെ (PyObject *)(self);
}

    സ്ഥിതം ശൂന്യം
DictionaryDestructor(PyObject *self_obj)
{
    DictionaryObject *self = (DictionaryObject*)self_obj;
    pyll_remove(&self->ref, &lastdict);
    dict_unref(self->dict);

    DESTRUCTOR_FINISH(self);
}

സ്ഥിതം അക്ഷരം *DictionaryAttrs[] = {
    "locked", "scope",
    ശൂന്യ
};

    സ്ഥിതം PyObject *
DictionaryDir(PyObject *self, PyObject *args UNUSED)
{
    തിരികെ ObjectDir(self, DictionaryAttrs);
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
DictionarySetattr(PyObject *self_obj, അക്ഷരം *name, PyObject *valObject)
{
    DictionaryObject *self = (DictionaryObject*)self_obj;
    എങ്കിൽ (valObject == ശൂന്യ)
    {
	PyErr_SET_STRING(PyExc_AttributeError,
		N_("cannot delete vim.Dictionary attributes"));
	തിരികെ -1;
    }

    എങ്കിൽ (strcmp(name, "locked") == 0)
    {
	എങ്കിൽ (self->dict->dv_lock == VAR_FIXED)
	{
	    PyErr_SET_STRING(PyExc_TypeError,
		    N_("cannot modify fixed dictionary"));
	    തിരികെ -1;
	}
	അല്ലെങ്കിൽ
	{
	    പൂർണ്ണസംഖ്യ		istrue = PyObject_IsTrue(valObject);
	    എങ്കിൽ (istrue == -1)
		തിരികെ -1;
	    അല്ലെങ്കിൽ എങ്കിൽ (istrue)
		self->dict->dv_lock = VAR_LOCKED;
	    അല്ലെങ്കിൽ
		self->dict->dv_lock = 0;
	}
	തിരികെ 0;
    }
    അല്ലെങ്കിൽ
    {
	PyErr_FORMAT(PyExc_AttributeError, N_("cannot set attribute %s"), name);
	തിരികെ -1;
    }
}

    സ്ഥിതം PyInt
DictionaryLength(DictionaryObject *self)
{
    തിരികെ ((PyInt) (self->dict->dv_hashtab.ht_used));
}

#define DICT_FLAG_HAS_DEFAULT	0x01
#define DICT_FLAG_എടുക്കുക		0x02
#define DICT_FLAG_NONE_DEFAULT	0x04
#define DICT_FLAG_RETURN_BOOL	0x08 // Incompatible with DICT_FLAG_POP
#define DICT_FLAG_RETURN_PAIR	0x10

    സ്ഥിതം PyObject *
_DictionaryItem(DictionaryObject *self, PyObject *args, പൂർണ്ണസംഖ്യ flags)
{
    PyObject	*താക്കോൽ_Object;
    PyObject	*defObject = ((flags & DICT_FLAG_NONE_DEFAULT)? Py_None : ശൂന്യ);
    PyObject	*ret;
    char_u	*താക്കോൽ;
    dictitem_T	*di;
    dict_T	*dict = self->dict;
    hashitem_T	*hi;
    PyObject	*todecref;

    എങ്കിൽ (flags & DICT_FLAG_HAS_DEFAULT)
    {
	എങ്കിൽ (!PyArg_ParseTuple(args, "O|O", &താക്കോൽ_Object, &defObject))
	    തിരികെ ശൂന്യ;
    }
    അല്ലെങ്കിൽ
	താക്കോൽ_Object = args;

    എങ്കിൽ (flags & DICT_FLAG_RETURN_BOOL)
	defObject = Py_False;

    എങ്കിൽ (!(താക്കോൽ = StringToChars(താക്കോൽ_Object, &todecref)))
	തിരികെ ശൂന്യ;

    എങ്കിൽ (*താക്കോൽ == NUL)
    {
	RAISE_NO_EMPTY_താക്കോലുകൾ;
	Py_XDECREF(todecref);
	തിരികെ ശൂന്യ;
    }

    hi = hash_find(&dict->dv_hashtab, താക്കോൽ);

    Py_XDECREF(todecref);

    എങ്കിൽ (HASHITEM_EMPTY(hi))
    {
	എങ്കിൽ (defObject)
	{
	    Py_INCREF(defObject);
	    തിരികെ defObject;
	}
	അല്ലെങ്കിൽ
	{
	    PyErr_SetObject(Py_Exc_താക്കോൽ_Error, താക്കോൽ_Object);
	    തിരികെ ശൂന്യ;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (flags & DICT_FLAG_RETURN_BOOL)
    {
	ret = Py_True;
	Py_INCREF(ret);
	തിരികെ ret;
    }

    di = dict_lookup(hi);

    എങ്കിൽ (!(ret = ConvertToPyObject(&di->di_tv)))
	തിരികെ ശൂന്യ;

    എങ്കിൽ (flags & DICT_FLAG_എടുക്കുക)
    {
	എങ്കിൽ (dict->dv_lock)
	{
	    RAISE_LOCKED_DICTIONARY;
	    Py_DECREF(ret);
	    തിരികെ ശൂന്യ;
	}

	hash_remove(&dict->dv_hashtab, hi, "Python remove variable");
	dictitem_free(di);
    }

    തിരികെ ret;
}

    സ്ഥിതം PyObject *
DictionaryItem(DictionaryObject *self, PyObject *താക്കോൽ_Object)
{
    തിരികെ _DictionaryItem(self, താക്കോൽ_Object, 0);
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
DictionaryContains(DictionaryObject *self, PyObject *താക്കോൽ_Object)
{
    PyObject	*rObj = _DictionaryItem(self, താക്കോൽ_Object, DICT_FLAG_RETURN_BOOL);
    പൂർണ്ണസംഖ്യ		ret;

    എങ്കിൽ (rObj == ശൂന്യ)
	തിരികെ -1;

    ret = (rObj == Py_True);

    Py_DECREF(rObj);

    തിരികെ ret;
}

തരനിർവചനം ഘടന
{
    പൂർണ്ണസംഖ്യ		dii_changed;
    hashtab_T	*dii_ht;
    hashitem_T	*dii_hi;
    long_u	dii_todo;
} dictiterinfo_T;

    സ്ഥിതം PyObject *
DictionaryIterNext(ശൂന്യം **arg)
{
    PyObject	*ret;
    dictiterinfo_T **dii = (dictiterinfo_T**)arg;

    എങ്കിൽ (!(*dii)->dii_todo)
	തിരികെ ശൂന്യ;

    എങ്കിൽ ((*dii)->dii_ht->ht_changed != (*dii)->dii_changed)
    {
	PyErr_SET_STRING(PyExc_RuntimeError,
		N_("hashtab changed during iteration"));
	തിരികെ ശൂന്യ;
    }

    ആകുമ്പോൾ (((*dii)->dii_todo) && HASHITEM_EMPTY((*dii)->dii_hi))
	++((*dii)->dii_hi);

    --((*dii)->dii_todo);

    എങ്കിൽ (!(ret = PyBytes_FromString((അക്ഷരം *)(*dii)->dii_hi->hi_താക്കോൽ)))
	തിരികെ ശൂന്യ;

    തിരികെ ret;
}

    സ്ഥിതം PyObject *
DictionaryIter(PyObject *self_obj)
{
    DictionaryObject	*self = (DictionaryObject*)self_obj;
    dictiterinfo_T	*dii;
    hashtab_T		*ht;

    എങ്കിൽ (!(dii = PyMem_New(dictiterinfo_T, 1)))
    {
	PyErr_NoMemory();
	തിരികെ ശൂന്യ;
    }

    ht = &self->dict->dv_hashtab;
    dii->dii_changed = ht->ht_changed;
    dii->dii_ht = ht;
    dii->dii_hi = ht->ht_array;
    dii->dii_todo = ht->ht_used;

    തിരികെ IterNew(dii,
	    PyMem_Free, DictionaryIterNext,
	    ശൂന്യ, ശൂന്യ, (PyObject *)self);
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
DictionaryAssItem(
	DictionaryObject *self, PyObject *താക്കോൽ_Object, PyObject *valObject)
{
    char_u	*താക്കോൽ;
    typval_T	tv;
    dict_T	*dict = self->dict;
    dictitem_T	*di;
    PyObject	*todecref;

    എങ്കിൽ (dict->dv_lock)
    {
	RAISE_LOCKED_DICTIONARY;
	തിരികെ -1;
    }

    എങ്കിൽ (!(താക്കോൽ = StringToChars(താക്കോൽ_Object, &todecref)))
	തിരികെ -1;

    എങ്കിൽ (*താക്കോൽ == NUL)
    {
	RAISE_NO_EMPTY_താക്കോലുകൾ;
	Py_XDECREF(todecref);
	തിരികെ -1;
    }

    di = dict_find(dict, താക്കോൽ, -1);

    എങ്കിൽ (valObject == ശൂന്യ)
    {
	hashitem_T	*hi;

	എങ്കിൽ (di == ശൂന്യ)
	{
	    Py_XDECREF(todecref);
	    PyErr_SetObject(Py_Exc_താക്കോൽ_Error, താക്കോൽ_Object);
	    തിരികെ -1;
	}
	hi = hash_find(&dict->dv_hashtab, di->di_താക്കോൽ);
	hash_remove(&dict->dv_hashtab, hi, "Python remove item");
	dictitem_free(di);
	Py_XDECREF(todecref);
	തിരികെ 0;
    }

    എങ്കിൽ (ConvertFromPyObject(valObject, &tv) == -1)
    {
	Py_XDECREF(todecref);
	തിരികെ -1;
    }

    എങ്കിൽ (di == ശൂന്യ)
    {
	എങ്കിൽ (!(di = dictitem_alloc(താക്കോൽ)))
	{
	    Py_XDECREF(todecref);
	    PyErr_NoMemory();
	    തിരികെ -1;
	}
	di->di_tv.v_type = VAR_UNKNOWN;

	എങ്കിൽ (dict_add(dict, di) == FAIL)
	{
	    dictitem_free(di);
	    RAISE_താക്കോൽ_ADD_FAIL(താക്കോൽ);
	    Py_XDECREF(todecref);
	    തിരികെ -1;
	}
    }
    അല്ലെങ്കിൽ
	clear_tv(&di->di_tv);

    Py_XDECREF(todecref);

    copy_tv(&tv, &di->di_tv);
    clear_tv(&tv);
    തിരികെ 0;
}

തരനിർവചനം PyObject *(*hi_to_py)(hashitem_T *);

    സ്ഥിതം PyObject *
DictionaryListObjects(DictionaryObject *self, hi_to_py hiconvert)
{
    dict_T	*dict = self->dict;
    long_u	todo = dict->dv_hashtab.ht_used;
    Py_ssize_t	i = 0;
    PyObject	*ret;
    hashitem_T	*hi;
    PyObject	*newObj;

    ret = PyList_New(todo);
    വേണ്ടി (hi = dict->dv_hashtab.ht_array; todo > 0; ++hi)
    {
	എങ്കിൽ (!HASHITEM_EMPTY(hi))
	{
	    എങ്കിൽ (!(newObj = hiconvert(hi)))
	    {
		Py_DECREF(ret);
		തിരികെ ശൂന്യ;
	    }
	    PyList_SET_ITEM(ret, i, newObj);
	    --todo;
	    ++i;
	}
    }
    തിരികെ ret;
}

    സ്ഥിതം PyObject *
dict_താക്കോൽ(hashitem_T *hi)
{
    തിരികെ PyBytes_FromString((അക്ഷരം *)(hi->hi_താക്കോൽ));
}

    സ്ഥിതം PyObject *
Dictionary_List_താക്കോലുകൾ(DictionaryObject *self, PyObject *args UNUSED)
{
    തിരികെ DictionaryListObjects(self, dict_താക്കോൽ);
}

    സ്ഥിതം PyObject *
dict_val(hashitem_T *hi)
{
    dictitem_T	*di;

    di = dict_lookup(hi);
    തിരികെ ConvertToPyObject(&di->di_tv);
}

    സ്ഥിതം PyObject *
Dictionary_List_മൂല്യങ്ങൾ(DictionaryObject *self, PyObject *args UNUSED)
{
    തിരികെ DictionaryListObjects(self, dict_val);
}

    സ്ഥിതം PyObject *
dict_item(hashitem_T *hi)
{
    PyObject	*താക്കോൽ_Object;
    PyObject	*valObject;
    PyObject	*ret;

    എങ്കിൽ (!(താക്കോൽ_Object = dict_താക്കോൽ(hi)))
	തിരികെ ശൂന്യ;

    എങ്കിൽ (!(valObject = dict_val(hi)))
    {
	Py_DECREF(താക്കോൽ_Object);
	തിരികെ ശൂന്യ;
    }

    ret = Py_Build_മൂല്യം("(OO)", താക്കോൽ_Object, valObject);

    Py_DECREF(താക്കോൽ_Object);
    Py_DECREF(valObject);

    തിരികെ ret;
}

    സ്ഥിതം PyObject *
Dictionary_List_ഇനങ്ങൾ(DictionaryObject *self, PyObject *args UNUSED)
{
    തിരികെ DictionaryListObjects(self, dict_item);
}

    സ്ഥിതം PyObject *
DictionaryUpdate(DictionaryObject *self, PyObject *args, PyObject *kwargs)
{
    dict_T	*dict = self->dict;

    എങ്കിൽ (dict->dv_lock)
    {
	RAISE_LOCKED_DICTIONARY;
	തിരികെ ശൂന്യ;
    }

    എങ്കിൽ (kwargs)
    {
	typval_T	tv;

	എങ്കിൽ (ConvertFromPyMapping(kwargs, &tv) == -1)
	    തിരികെ ശൂന്യ;

	VimTryStart();
	dict_extend(self->dict, tv.vval.v_dict, (char_u *) "force", ശൂന്യ);
	clear_tv(&tv);
	എങ്കിൽ (VimTryEnd())
	    തിരികെ ശൂന്യ;
    }
    അല്ലെങ്കിൽ
    {
	PyObject	*obj = ശൂന്യ;

	എങ്കിൽ (!PyArg_ParseTuple(args, "|O", &obj))
	    തിരികെ ശൂന്യ;

	എങ്കിൽ (obj == ശൂന്യ)
	{
	    Py_INCREF(Py_None);
	    തിരികെ Py_None;
	}

	എങ്കിൽ (PyObject_HasAttrString(obj, "keys"))
	    തിരികെ DictionaryUpdate(self, ശൂന്യ, obj);
	അല്ലെങ്കിൽ
	{
	    PyObject	*iterator;
	    PyObject	*item;

	    എങ്കിൽ (!(iterator = PyObject_GetIter(obj)))
		തിരികെ ശൂന്യ;

	    ആകുമ്പോൾ ((item = PyIter_Next(iterator)))
	    {
		PyObject	*fast;
		PyObject	*താക്കോൽ_Object;
		PyObject	*valObject;
		PyObject	*todecref;
		char_u		*താക്കോൽ;
		dictitem_T	*di;
		hashitem_T	*hi;

		എങ്കിൽ (!(fast = PySequence_Fast(item, "")))
		{
		    Py_DECREF(iterator);
		    Py_DECREF(item);
		    തിരികെ ശൂന്യ;
		}

		Py_DECREF(item);

		എങ്കിൽ (PySequence_Size(fast) != 2)
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    PyErr_FORMAT(Py_Exc_മൂല്യം_Error,
			    N_("expected sequence element of size 2, "
			    "but got sequence of size %d"),
			    (പൂർണ്ണസംഖ്യ) PySequence_Size(fast));
		    തിരികെ ശൂന്യ;
		}

		താക്കോൽ_Object = PySequence_GetItem(fast, 0);

		എങ്കിൽ (!(താക്കോൽ = StringToChars(താക്കോൽ_Object, &todecref)))
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    തിരികെ ശൂന്യ;
		}

		di = dictitem_alloc(താക്കോൽ);

		Py_XDECREF(todecref);

		എങ്കിൽ (di == ശൂന്യ)
		{
		    Py_DECREF(fast);
		    Py_DECREF(iterator);
		    PyErr_NoMemory();
		    തിരികെ ശൂന്യ;
		}
		di->di_tv.v_type = VAR_UNKNOWN;

		valObject = PySequence_GetItem(fast, 1);

		എങ്കിൽ (ConvertFromPyObject(valObject, &di->di_tv) == -1)
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    dictitem_free(di);
		    തിരികെ ശൂന്യ;
		}

		Py_DECREF(fast);

		hi = hash_find(&dict->dv_hashtab, di->di_താക്കോൽ);
		എങ്കിൽ (!HASHITEM_EMPTY(hi) || dict_add(dict, di) == FAIL)
		{
		    RAISE_താക്കോൽ_ADD_FAIL(di->di_താക്കോൽ);
		    Py_DECREF(iterator);
		    dictitem_free(di);
		    തിരികെ ശൂന്യ;
		}
	    }

	    Py_DECREF(iterator);

	    // Iterator may have finished due to an exception
	    എങ്കിൽ (PyErr_Occurred())
		തിരികെ ശൂന്യ;
	}
    }
    Py_INCREF(Py_None);
    തിരികെ Py_None;
}

    സ്ഥിതം PyObject *
DictionaryGet(DictionaryObject *self, PyObject *args)
{
    തിരികെ _DictionaryItem(self, args,
			    DICT_FLAG_HAS_DEFAULT|DICT_FLAG_NONE_DEFAULT);
}

    സ്ഥിതം PyObject *
Dictionary_എടുക്കുക(DictionaryObject *self, PyObject *args)
{
    തിരികെ _DictionaryItem(self, args, DICT_FLAG_HAS_DEFAULT|DICT_FLAG_എടുക്കുക);
}

    സ്ഥിതം PyObject *
Dictionary_എടുക്കുക_Item(DictionaryObject *self, PyObject *args UNUSED)
{
    hashitem_T	*hi;
    PyObject	*ret;
    PyObject	*valObject;
    dictitem_T	*di;

    എങ്കിൽ (self->dict->dv_hashtab.ht_used == 0)
    {
	PyErr_SetNone(Py_Exc_താക്കോൽ_Error);
	തിരികെ ശൂന്യ;
    }

    hi = self->dict->dv_hashtab.ht_array;
    ആകുമ്പോൾ (HASHITEM_EMPTY(hi))
	++hi;

    di = dict_lookup(hi);

    എങ്കിൽ (!(valObject = ConvertToPyObject(&di->di_tv)))
	തിരികെ ശൂന്യ;

    എങ്കിൽ (!(ret = Py_Build_മൂല്യം("(" Py_bytes_fmt "O)", hi->hi_താക്കോൽ, valObject)))
    {
	Py_DECREF(valObject);
	തിരികെ ശൂന്യ;
    }

    hash_remove(&self->dict->dv_hashtab, hi, "Python pop item");
    dictitem_free(di);

    തിരികെ ret;
}

    സ്ഥിതം PyObject *
Dictionary_Has_താക്കോൽ(DictionaryObject *self, PyObject *താക്കോൽ_Object)
{
    തിരികെ _DictionaryItem(self, താക്കോൽ_Object, DICT_FLAG_RETURN_BOOL);
}

സ്ഥിതം PySequenceMethods DictionaryAsSeq = {
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

സ്ഥിതം PyMappingMethods DictionaryAsMapping = {
    (lenfunc)       DictionaryLength,
    (binaryfunc)    DictionaryItem,
    (objobjargproc) DictionaryAssItem,
};

സ്ഥിതം ഘടന PyMethodDef DictionaryMethods[] = {
    {"keys",	(PyCFunction)Dictionary_List_താക്കോലുകൾ,	METH_NOARGS,	""},
    {"values",	(PyCFunction)Dictionary_List_മൂല്യങ്ങൾ,	METH_NOARGS,	""},
    {"items",	(PyCFunction)Dictionary_List_ഇനങ്ങൾ,	METH_NOARGS,	""},
    {"update",	(PyCFunction)(ശൂന്യം *)DictionaryUpdate,		METH_VARARGS|METH_KEYWORDS, ""},
    {"get",	(PyCFunction)DictionaryGet,		METH_VARARGS,	""},
    {"pop",	(PyCFunction)Dictionary_എടുക്കുക,		METH_VARARGS,	""},
    {"popitem",	(PyCFunction)Dictionary_എടുക്കുക_Item,		METH_NOARGS,	""},
    {"has_key",	(PyCFunction)Dictionary_Has_താക്കോൽ,		METH_O,		""},
    {"__dir__",	(PyCFunction)DictionaryDir,		METH_NOARGS,	""},
    { ശൂന്യ,	ശൂന്യ,					0,		ശൂന്യ}
};

DEFINE_PY_TYPE_OBJECT(ListType);

തരനിർവചനം ഘടന
{
    PyObject_HEAD
    list_T	*list;
    pylinkedlist_T	ref;
} ListObject;

#define NEW_LIST(list) ListNew(ListTypePtr, list)

    സ്ഥിതം PyObject *
ListNew(PyTypeObject *subtype, list_T *list)
{
    ListObject	*self;

    എങ്കിൽ (list == ശൂന്യ)
	തിരികെ ശൂന്യ;

    self = (ListObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    എങ്കിൽ (self == ശൂന്യ)
	തിരികെ ശൂന്യ;
    self->list = list;
    ++list->lv_refcount;
    CHECK_LIST_MATERIALIZE(list);

    pyll_add((PyObject *)(self), &self->ref, &lastlist);

    തിരികെ (PyObject *)(self);
}

    സ്ഥിതം list_T *
py_list_alloc(ശൂന്യം)
{
    list_T	*ret;

    എങ്കിൽ (!(ret = list_alloc()))
    {
	PyErr_NoMemory();
	തിരികെ ശൂന്യ;
    }
    ++ret->lv_refcount;

    തിരികെ ret;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
list_py_concat(list_T *l, PyObject *obj, PyObject *lookup_dict)
{
    PyObject	*iterator;
    PyObject	*item;
    listitem_T	*li;

    എങ്കിൽ (!(iterator = PyObject_GetIter(obj)))
	തിരികെ -1;

    ആകുമ്പോൾ ((item = PyIter_Next(iterator)))
    {
	എങ്കിൽ (!(li = listitem_alloc()))
	{
	    PyErr_NoMemory();
	    Py_DECREF(item);
	    Py_DECREF(iterator);
	    തിരികെ -1;
	}
	li->li_tv.v_lock = 0;
	li->li_tv.v_type = VAR_UNKNOWN;

	എങ്കിൽ (_ConvertFromPyObject(item, &li->li_tv, lookup_dict) == -1)
	{
	    Py_DECREF(item);
	    Py_DECREF(iterator);
	    listitem_free(l, li);
	    തിരികെ -1;
	}

	Py_DECREF(item);

	list_append(l, li);
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    എങ്കിൽ (PyErr_Occurred())
	തിരികെ -1;

    തിരികെ 0;
}

    സ്ഥിതം PyObject *
ListConstructor(PyTypeObject *subtype, PyObject *args, PyObject *kwargs)
{
    list_T	*list;
    PyObject	*obj = ശൂന്യ;

    എങ്കിൽ (kwargs)
    {
	PyErr_SET_STRING(PyExc_TypeError,
		N_("list constructor does not accept keyword arguments"));
	തിരികെ ശൂന്യ;
    }

    എങ്കിൽ (!PyArg_ParseTuple(args, "|O", &obj))
	തിരികെ ശൂന്യ;

    എങ്കിൽ (!(list = py_list_alloc()))
	തിരികെ ശൂന്യ;

    എങ്കിൽ (obj)
    {
	PyObject	*lookup_dict;

	എങ്കിൽ (!(lookup_dict = PyDict_New()))
	{
	    list_unref(list);
	    തിരികെ ശൂന്യ;
	}

	എങ്കിൽ (list_py_concat(list, obj, lookup_dict) == -1)
	{
	    Py_DECREF(lookup_dict);
	    list_unref(list);
	    തിരികെ ശൂന്യ;
	}

	Py_DECREF(lookup_dict);
    }

    തിരികെ ListNew(subtype, list);
}

    സ്ഥിതം ശൂന്യം
ListDestructor(PyObject *self_obj)
{
    ListObject *self = (ListObject*)self_obj;
    pyll_remove(&self->ref, &lastlist);
    list_unref(self->list);

    DESTRUCTOR_FINISH(self);
}

    സ്ഥിതം PyInt
ListLength(ListObject *self)
{
    തിരികെ ((PyInt) (self->list->lv_len));
}

    സ്ഥിതം PyObject *
ListIndex(ListObject *self, Py_ssize_t index)
{
    listitem_T	*li;

    എങ്കിൽ (index >= ListLength(self))
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("list index out of range"));
	തിരികെ ശൂന്യ;
    }
    li = list_find(self->list, (long) index);
    എങ്കിൽ (li == ശൂന്യ)
    {
	// No more suitable format specifications in python-2.3
	PyErr_VIM_FORMAT(N_("internal error: failed to get Vim list item %d"),
		(പൂർണ്ണസംഖ്യ) index);
	തിരികെ ശൂന്യ;
    }
    തിരികെ ConvertToPyObject(&li->li_tv);
}

    സ്ഥിതം PyObject *
ListSlice(ListObject *self, Py_ssize_t first, Py_ssize_t step,
	  Py_ssize_t slicelen)
{
    PyInt	i;
    PyObject	*list;

    എങ്കിൽ (step == 0)
    {
	PyErr_SET_STRING(Py_Exc_മൂല്യം_Error, N_("slice step cannot be zero"));
	തിരികെ ശൂന്യ;
    }

    list = PyList_New(slicelen);
    എങ്കിൽ (list == ശൂന്യ)
	തിരികെ ശൂന്യ;

    വേണ്ടി (i = 0; i < slicelen; ++i)
    {
	PyObject	*item;

	item = ListIndex(self, first + i*step);
	എങ്കിൽ (item == ശൂന്യ)
	{
	    Py_DECREF(list);
	    തിരികെ ശൂന്യ;
	}

	PyList_SET_ITEM(list, i, item);
    }

    തിരികെ list;
}

    സ്ഥിതം PyObject *
ListItem(ListObject *self, PyObject* idx)
{
#എങ്കിൽ PY_MAJOR_VERSION < 3
    എങ്കിൽ (PyInt_Check(idx))
    {
	long _idx = PyInt_AsLong(idx);
	തിരികെ ListIndex(self, _idx);
    }
    അല്ലെങ്കിൽ
#endif
    എങ്കിൽ (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	തിരികെ ListIndex(self, _idx);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	എങ്കിൽ (PySlice_GetIndicesEx((PySliceObject_T *)idx, ListLength(self),
				 &start, &stop, &step, &slicelen) < 0)
	    തിരികെ ശൂന്യ;
	തിരികെ ListSlice(self, start, step, slicelen);
    }
    അല്ലെങ്കിൽ
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	തിരികെ ശൂന്യ;
    }
}

    സ്ഥിതം ശൂന്യം
list_restore(Py_ssize_t numadded, Py_ssize_t numreplaced, Py_ssize_t slicelen,
	list_T *l, listitem_T **lis, listitem_T *lastaddedli)
{
    ആകുമ്പോൾ (numreplaced--)
    {
	list_തിരുകുക(l, lis[numreplaced], lis[slicelen + numreplaced]);
	listitem_remove(l, lis[slicelen + numreplaced]);
    }
    ആകുമ്പോൾ (numadded--)
    {
	listitem_T	*next;

	next = lastaddedli->li_prev;
	listitem_remove(l, lastaddedli);
	lastaddedli = next;
    }
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
ListAssSlice(ListObject *self, Py_ssize_t first,
	     Py_ssize_t step, Py_ssize_t slicelen, PyObject *obj)
{
    PyObject	*iterator;
    PyObject	*item;
    listitem_T	*li;
    listitem_T	*lastaddedli = ശൂന്യ;
    listitem_T	*next;
    typval_T	v;
    list_T	*l = self->list;
    PyInt	i;
    PyInt	j;
    PyInt	numreplaced = 0;
    PyInt	numadded = 0;
    PyInt	size;
    listitem_T	**lis = ശൂന്യ;

    size = ListLength(self);

    എങ്കിൽ (l->lv_lock)
    {
	RAISE_LOCKED_LIST;
	തിരികെ -1;
    }

    എങ്കിൽ (step == 0)
    {
	PyErr_SET_STRING(Py_Exc_മൂല്യം_Error, N_("slice step cannot be zero"));
	തിരികെ -1;
    }

    എങ്കിൽ (step != 1 && slicelen == 0)
    {
	// Nothing to do. Only error out if obj has some items.
	പൂർണ്ണസംഖ്യ		ret = 0;

	എങ്കിൽ (obj == ശൂന്യ)
	    തിരികെ 0;

	എങ്കിൽ (!(iterator = PyObject_GetIter(obj)))
	    തിരികെ -1;

	എങ്കിൽ ((item = PyIter_Next(iterator)))
	{
	    PyErr_FORMAT(Py_Exc_മൂല്യം_Error,
		    N_("attempt to assign sequence of size greater than %d "
			"to extended slice"), 0);
	    Py_DECREF(item);
	    ret = -1;
	}
	Py_DECREF(iterator);
	തിരികെ ret;
    }

    എങ്കിൽ (obj != ശൂന്യ)
	// XXX May allocate zero bytes.
	എങ്കിൽ (!(lis = PyMem_New(listitem_T *, slicelen * 2)))
	{
	    PyErr_NoMemory();
	    തിരികെ -1;
	}

    എങ്കിൽ (first == size)
	li = ശൂന്യ;
    അല്ലെങ്കിൽ
    {
	li = list_find(l, (long) first);
	എങ്കിൽ (li == ശൂന്യ)
	{
	    PyErr_VIM_FORMAT(N_("internal error: no Vim list item %d"),
		    (പൂർണ്ണസംഖ്യ)first);
	    എങ്കിൽ (obj != ശൂന്യ)
		PyMem_Free(lis);
	    തിരികെ -1;
	}
	i = slicelen;
	ആകുമ്പോൾ (i-- && li != ശൂന്യ)
	{
	    j = step;
	    next = li;
	    എങ്കിൽ (step > 0)
		ആകുമ്പോൾ (next != ശൂന്യ && ((next = next->li_next) != ശൂന്യ) && --j);
	    അല്ലെങ്കിൽ
		ആകുമ്പോൾ (next != ശൂന്യ && ((next = next->li_prev) != ശൂന്യ) && ++j);

	    എങ്കിൽ (obj == ശൂന്യ)
		listitem_remove(l, li);
	    അല്ലെങ്കിൽ
		lis[slicelen - i - 1] = li;

	    li = next;
	}
	എങ്കിൽ (li == ശൂന്യ && i != -1)
	{
	    PyErr_SET_VIM(N_("internal error: not enough list items"));
	    എങ്കിൽ (obj != ശൂന്യ)
		PyMem_Free(lis);
	    തിരികെ -1;
	}
    }

    എങ്കിൽ (obj == ശൂന്യ)
	തിരികെ 0;

    എങ്കിൽ (!(iterator = PyObject_GetIter(obj)))
    {
	PyMem_Free(lis);
	തിരികെ -1;
    }

    i = 0;
    ആകുമ്പോൾ ((item = PyIter_Next(iterator)))
    {
	എങ്കിൽ (ConvertFromPyObject(item, &v) == -1)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(item);
	    PyMem_Free(lis);
	    തിരികെ -1;
	}
	Py_DECREF(item);
	എങ്കിൽ (list_തിരുകുക_tv(l, &v, numreplaced < slicelen
				    ? lis[numreplaced]
				    : li) == FAIL)
	{
	    clear_tv(&v);
	    PyErr_SET_VIM(N_("internal error: failed to add item to list"));
	    list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	    PyMem_Free(lis);
	    തിരികെ -1;
	}
	എങ്കിൽ (numreplaced < slicelen)
	{
	    lis[slicelen + numreplaced] = lis[numreplaced]->li_prev;
	    vimlist_remove(l, lis[numreplaced], lis[numreplaced]);
	    numreplaced++;
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (li)
		lastaddedli = li->li_prev;
	    അല്ലെങ്കിൽ
		lastaddedli = l->lv_u.mat.lv_last;
	    numadded++;
	}
	clear_tv(&v);
	എങ്കിൽ (step != 1 && i >= slicelen)
	{
	    Py_DECREF(iterator);
	    PyErr_FORMAT(Py_Exc_മൂല്യം_Error,
		    N_("attempt to assign sequence of size greater than %d "
			"to extended slice"), (പൂർണ്ണസംഖ്യ) slicelen);
	    list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	    PyMem_Free(lis);
	    തിരികെ -1;
	}
	++i;
    }
    Py_DECREF(iterator);

    എങ്കിൽ (step != 1 && i != slicelen)
    {
	PyErr_FORMAT2(Py_Exc_മൂല്യം_Error,
		N_("attempt to assign sequence of size %d to extended slice "
		    "of size %d"), (പൂർണ്ണസംഖ്യ) i, (പൂർണ്ണസംഖ്യ) slicelen);
	list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	PyMem_Free(lis);
	തിരികെ -1;
    }

    എങ്കിൽ (PyErr_Occurred())
    {
	list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	PyMem_Free(lis);
	തിരികെ -1;
    }

    വേണ്ടി (i = 0; i < numreplaced; i++)
	listitem_free(l, lis[i]);
    എങ്കിൽ (step == 1)
	വേണ്ടി (i = numreplaced; i < slicelen; i++)
	    listitem_remove(l, lis[i]);

    PyMem_Free(lis);

    തിരികെ 0;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
ListAssIndex(ListObject *self, Py_ssize_t index, PyObject *obj)
{
    typval_T	tv;
    list_T	*l = self->list;
    listitem_T	*li;
    Py_ssize_t	length = ListLength(self);

    എങ്കിൽ (l->lv_lock)
    {
	RAISE_LOCKED_LIST;
	തിരികെ -1;
    }
    എങ്കിൽ (index > length || (index == length && obj == ശൂന്യ))
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("list index out of range"));
	തിരികെ -1;
    }

    എങ്കിൽ (obj == ശൂന്യ)
    {
	li = list_find(l, (long) index);
	എങ്കിൽ (li == ശൂന്യ)
	{
	    PyErr_VIM_FORMAT(N_("internal error: failed to get Vim "
			"list item %d"), (പൂർണ്ണസംഖ്യ) index);
	    തിരികെ -1;
	}
	vimlist_remove(l, li, li);
	clear_tv(&li->li_tv);
	vim_free(li);
	തിരികെ 0;
    }

    എങ്കിൽ (ConvertFromPyObject(obj, &tv) == -1)
	തിരികെ -1;

    എങ്കിൽ (index == length)
    {
	എങ്കിൽ (list_append_tv(l, &tv) == FAIL)
	{
	    clear_tv(&tv);
	    PyErr_SET_VIM(N_("failed to add item to list"));
	    തിരികെ -1;
	}
    }
    അല്ലെങ്കിൽ
    {
	li = list_find(l, (long) index);
	എങ്കിൽ (li == ശൂന്യ)
	{
	    PyErr_VIM_FORMAT(N_("internal error: failed to get Vim "
			"list item %d"), (പൂർണ്ണസംഖ്യ) index);
	    തിരികെ -1;
	}
	clear_tv(&li->li_tv);
	copy_tv(&tv, &li->li_tv);
	clear_tv(&tv);
    }
    തിരികെ 0;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
ListAssItem(ListObject *self, PyObject *idx, PyObject *obj)
{
#എങ്കിൽ PY_MAJOR_VERSION < 3
    എങ്കിൽ (PyInt_Check(idx))
    {
	long _idx = PyInt_AsLong(idx);
	തിരികെ (പൂർണ്ണസംഖ്യ)ListAssIndex(self, _idx, obj);
    }
    അല്ലെങ്കിൽ
#endif
    എങ്കിൽ (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	തിരികെ (പൂർണ്ണസംഖ്യ)ListAssIndex(self, _idx, obj);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	എങ്കിൽ (PySlice_GetIndicesEx((PySliceObject_T *)idx, ListLength(self),
				 &start, &stop, &step, &slicelen) < 0)
	    തിരികെ -1;
	തിരികെ (പൂർണ്ണസംഖ്യ)ListAssSlice(self, start, step, slicelen,
		obj);
    }
    അല്ലെങ്കിൽ
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	തിരികെ -1;
    }
}

    സ്ഥിതം PyObject *
ListConcatInPlace(ListObject *self, PyObject *obj)
{
    list_T	*l = self->list;
    PyObject	*lookup_dict;

    എങ്കിൽ (l->lv_lock)
    {
	RAISE_LOCKED_LIST;
	തിരികെ ശൂന്യ;
    }

    എങ്കിൽ (!(lookup_dict = PyDict_New()))
	തിരികെ ശൂന്യ;

    എങ്കിൽ (list_py_concat(l, obj, lookup_dict) == -1)
    {
	Py_DECREF(lookup_dict);
	തിരികെ ശൂന്യ;
    }
    Py_DECREF(lookup_dict);

    Py_INCREF((PyObject *)self);
    തിരികെ (PyObject *)(self);
}

തരനിർവചനം ഘടന
{
    listwatch_T	lw;
    list_T	*list;
} listiterinfo_T;

    സ്ഥിതം ശൂന്യം
ListIterDestruct(ശൂന്യം *arg)
{
    listiterinfo_T *lii = (listiterinfo_T*)arg;
    list_rem_watch(lii->list, &lii->lw);
    list_unref(lii->list);
    PyMem_Free(lii);
}

    സ്ഥിതം PyObject *
ListIterNext(ശൂന്യം **arg)
{
    PyObject	*ret;
    listiterinfo_T **lii = (listiterinfo_T**)arg;

    എങ്കിൽ (!((*lii)->lw.lw_item))
	തിരികെ ശൂന്യ;

    എങ്കിൽ (!(ret = ConvertToPyObject(&((*lii)->lw.lw_item->li_tv))))
	തിരികെ ശൂന്യ;

    (*lii)->lw.lw_item = (*lii)->lw.lw_item->li_next;

    തിരികെ ret;
}

    സ്ഥിതം PyObject *
ListIter(PyObject *self_obj)
{
    ListObject	*self = (ListObject*)self_obj;
    listiterinfo_T	*lii;
    list_T	*l = self->list;

    എങ്കിൽ (!(lii = PyMem_New(listiterinfo_T, 1)))
    {
	PyErr_NoMemory();
	തിരികെ ശൂന്യ;
    }

    CHECK_LIST_MATERIALIZE(l);
    list_add_watch(l, &lii->lw);
    lii->lw.lw_item = l->lv_first;
    lii->list = l;
    ++l->lv_refcount;

    തിരികെ IterNew(lii,
	    ListIterDestruct, ListIterNext,
	    ശൂന്യ, ശൂന്യ, (PyObject *)self);
}

സ്ഥിതം അക്ഷരം *ListAttrs[] = {
    "locked",
    ശൂന്യ
};

    സ്ഥിതം PyObject *
ListDir(PyObject *self, PyObject *args UNUSED)
{
    തിരികെ ObjectDir(self, ListAttrs);
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
ListSetattr(PyObject *self_obj, അക്ഷരം *name, PyObject *valObject)
{
    ListObject *self = (ListObject*)self_obj;
    എങ്കിൽ (valObject == ശൂന്യ)
    {
	PyErr_SET_STRING(PyExc_AttributeError,
		N_("cannot delete vim.List attributes"));
	തിരികെ -1;
    }

    എങ്കിൽ (strcmp(name, "locked") == 0)
    {
	എങ്കിൽ (self->list->lv_lock == VAR_FIXED)
	{
	    PyErr_SET_STRING(PyExc_TypeError, N_("cannot modify fixed list"));
	    തിരികെ -1;
	}
	അല്ലെങ്കിൽ
	{
	    പൂർണ്ണസംഖ്യ		istrue = PyObject_IsTrue(valObject);
	    എങ്കിൽ (istrue == -1)
		തിരികെ -1;
	    അല്ലെങ്കിൽ എങ്കിൽ (istrue)
		self->list->lv_lock = VAR_LOCKED;
	    അല്ലെങ്കിൽ
		self->list->lv_lock = 0;
	}
	തിരികെ 0;
    }
    അല്ലെങ്കിൽ
    {
	PyErr_FORMAT(PyExc_AttributeError, N_("cannot set attribute %s"), name);
	തിരികെ -1;
    }
}

സ്ഥിതം PySequenceMethods ListAsSeq = {
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

സ്ഥിതം PyMappingMethods ListAsMapping = {
    /* mp_length	*/ (lenfunc) ListLength,
    /* mp_subscript     */ (binaryfunc) ListItem,
    /* mp_ass_subscript */ (objobjargproc) ListAssItem,
};

സ്ഥിതം ഘടന PyMethodDef ListMethods[] = {
    {"extend",	(PyCFunction)ListConcatInPlace,	METH_O,		""},
    {"__dir__",	(PyCFunction)ListDir,		METH_NOARGS,	""},
    { ശൂന്യ,	ശൂന്യ,				0,		ശൂന്യ}
};

DEFINE_PY_TYPE_OBJECT(TupleType);

തരനിർവചനം ഘടന
{
    PyObject_HEAD
    tuple_T	*tuple;
    pylinkedlist_T	ref;
} TupleObject;

#define NEW_TUPLE(tuple) TupleNew(TupleTypePtr, tuple)

    സ്ഥിതം PyObject *
TupleNew(PyTypeObject *subtype, tuple_T *tuple)
{
    TupleObject	*self;

    എങ്കിൽ (tuple == ശൂന്യ)
	തിരികെ ശൂന്യ;

    self = (TupleObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    എങ്കിൽ (self == ശൂന്യ)
	തിരികെ ശൂന്യ;
    self->tuple = tuple;
    ++tuple->tv_refcount;

    pyll_add((PyObject *)(self), &self->ref, &lasttuple);

    തിരികെ (PyObject *)(self);
}

    സ്ഥിതം tuple_T *
py_tuple_alloc(ശൂന്യം)
{
    tuple_T	*ret;

    എങ്കിൽ (!(ret = tuple_alloc()))
    {
	PyErr_NoMemory();
	തിരികെ ശൂന്യ;
    }
    ++ret->tv_refcount;

    തിരികെ ret;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
tuple_py_concat(tuple_T *t, PyObject *obj, PyObject *lookup_dict)
{
    PyObject	*iterator;
    PyObject	*item;

    എങ്കിൽ (!(iterator = PyObject_GetIter(obj)))
	തിരികെ -1;

    ആകുമ്പോൾ ((item = PyIter_Next(iterator)))
    {
	typval_T    new_tv;

	എങ്കിൽ (_ConvertFromPyObject(item, &new_tv, lookup_dict) == -1)
	{
	    Py_DECREF(item);
	    Py_DECREF(iterator);
	    തിരികെ -1;
	}

	Py_DECREF(item);

	എങ്കിൽ (tuple_append_tv(t, &new_tv) == FAIL)
	{
	    Py_DECREF(iterator);
	    തിരികെ -1;
	}
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    എങ്കിൽ (PyErr_Occurred())
	തിരികെ -1;

    തിരികെ 0;
}

    സ്ഥിതം PyObject *
TupleConstructor(PyTypeObject *subtype, PyObject *args, PyObject *kwargs)
{
    tuple_T	*tuple;
    PyObject	*obj = ശൂന്യ;

    എങ്കിൽ (kwargs)
    {
	PyErr_SET_STRING(PyExc_TypeError,
		N_("tuple constructor does not accept keyword arguments"));
	തിരികെ ശൂന്യ;
    }

    എങ്കിൽ (!PyArg_ParseTuple(args, "|O", &obj))
	തിരികെ ശൂന്യ;

    എങ്കിൽ (!(tuple = py_tuple_alloc()))
	തിരികെ ശൂന്യ;

    എങ്കിൽ (obj)
    {
	PyObject	*lookup_dict;

	എങ്കിൽ (!(lookup_dict = PyDict_New()))
	{
	    tuple_unref(tuple);
	    തിരികെ ശൂന്യ;
	}

	എങ്കിൽ (tuple_py_concat(tuple, obj, lookup_dict) == -1)
	{
	    Py_DECREF(lookup_dict);
	    tuple_unref(tuple);
	    തിരികെ ശൂന്യ;
	}

	Py_DECREF(lookup_dict);
    }

    തിരികെ TupleNew(subtype, tuple);
}

    സ്ഥിതം ശൂന്യം
TupleDestructor(PyObject *self_obj)
{
    TupleObject *self = (TupleObject *)self_obj;
    pyll_remove(&self->ref, &lasttuple);
    tuple_unref(self->tuple);

    DESTRUCTOR_FINISH(self);
}

    സ്ഥിതം PyInt
TupleLength(TupleObject *self)
{
    തിരികെ ((PyInt)(tuple_len(self->tuple)));
}

    സ്ഥിതം PyObject *
TupleIndex(TupleObject *self, Py_ssize_t index)
{
    PyInt	len = TupleLength(self);

    എങ്കിൽ (index < 0)
	index = len + index;

    എങ്കിൽ (index < 0 || index >= len)
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("tuple index out of range"));
	തിരികെ ശൂന്യ;
    }
    തിരികെ ConvertToPyObject(TUPLE_ITEM(self->tuple, index));
}

/*
 * Return a new tuple object for the tuple slice starting from the index
 * "first" and of length "slicelen" skipping "step" items.
 */
    സ്ഥിതം PyObject *
TupleSlice(
    TupleObject		*self,
    Py_ssize_t		first,
    Py_ssize_t		step,
    Py_ssize_t		slicelen)
{
    PyInt	i;
    PyObject	*tuple;

    tuple = PyTuple_New(slicelen);
    എങ്കിൽ (tuple == ശൂന്യ)
	തിരികെ ശൂന്യ;

    വേണ്ടി (i = 0; i < slicelen; ++i)
    {
	PyObject	*item;

	item = TupleIndex(self, first + i * step);
	എങ്കിൽ (item == ശൂന്യ)
	{
	    Py_DECREF(tuple);
	    തിരികെ ശൂന്യ;
	}

	PyTuple_SetItem(tuple, i, item);
    }

    തിരികെ tuple;
}

    സ്ഥിതം PyObject *
TupleItem(TupleObject *self, PyObject* idx)
{
#എങ്കിൽ PY_MAJOR_VERSION < 3
    എങ്കിൽ (PyInt_Check(idx))
    {
	long _idx = PyInt_AsLong(idx);
	തിരികെ TupleIndex(self, _idx);
    }
    അല്ലെങ്കിൽ
#endif
    എങ്കിൽ (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	തിരികെ TupleIndex(self, _idx);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	എങ്കിൽ (PySlice_GetIndicesEx((PySliceObject_T *)idx, TupleLength(self),
				 &start, &stop, &step, &slicelen) < 0)
	    തിരികെ ശൂന്യ;
	തിരികെ TupleSlice(self, start, step, slicelen);
    }
    അല്ലെങ്കിൽ
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	തിരികെ ശൂന്യ;
    }
}

തരനിർവചനം ഘടന
{
    tuple_T	*tuple;
    പൂർണ്ണസംഖ്യ		index;
} tupleiterinfo_T;

    സ്ഥിതം ശൂന്യം
TupleIterDestruct(ശൂന്യം *arg)
{
    tupleiterinfo_T *tii = (tupleiterinfo_T*)arg;
    tuple_unref(tii->tuple);
    PyMem_Free(tii);
}

    സ്ഥിതം PyObject *
TupleIterNext(ശൂന്യം **arg)
{
    PyObject	*ret;
    tupleiterinfo_T **tii = (tupleiterinfo_T**)arg;

    എങ്കിൽ ((*tii)->index >= TUPLE_LEN((*tii)->tuple))
	തിരികെ ശൂന്യ;

    എങ്കിൽ (!(ret = ConvertToPyObject(TUPLE_ITEM((*tii)->tuple, (*tii)->index))))
	തിരികെ ശൂന്യ;

    (*tii)->index++;

    തിരികെ ret;
}

    സ്ഥിതം PyObject *
TupleIter(PyObject *self_obj)
{
    TupleObject	*self = (TupleObject*)self_obj;
    tupleiterinfo_T	*tii;
    tuple_T	*t = self->tuple;

    എങ്കിൽ (!(tii = PyMem_New(tupleiterinfo_T, 1)))
    {
	PyErr_NoMemory();
	തിരികെ ശൂന്യ;
    }

    tii->tuple = t;
    tii->index = 0;
    ++t->tv_refcount;

    തിരികെ IterNew(tii,
	    TupleIterDestruct, TupleIterNext,
	    ശൂന്യ, ശൂന്യ, (PyObject *)self);
}

സ്ഥിതം അക്ഷരം *TupleAttrs[] = {
    "locked",
    ശൂന്യ
};

    സ്ഥിതം PyObject *
TupleDir(PyObject *self, PyObject *args UNUSED)
{
    തിരികെ ObjectDir(self, TupleAttrs);
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
TupleSetattr(PyObject *self_obj, അക്ഷരം *name, PyObject *valObject)
{
    TupleObject *self = (TupleObject*)self_obj;
    എങ്കിൽ (valObject == ശൂന്യ)
    {
	PyErr_SET_STRING(PyExc_AttributeError,
		N_("cannot delete vim.Tuple attributes"));
	തിരികെ -1;
    }

    എങ്കിൽ (strcmp(name, "locked") == 0)
    {
	എങ്കിൽ (self->tuple->tv_lock == VAR_FIXED)
	{
	    PyErr_SET_STRING(PyExc_TypeError, N_("cannot modify fixed tuple"));
	    തിരികെ -1;
	}
	അല്ലെങ്കിൽ
	{
	    പൂർണ്ണസംഖ്യ		istrue = PyObject_IsTrue(valObject);
	    എങ്കിൽ (istrue == -1)
		തിരികെ -1;
	    അല്ലെങ്കിൽ എങ്കിൽ (istrue)
		self->tuple->tv_lock = VAR_LOCKED;
	    അല്ലെങ്കിൽ
		self->tuple->tv_lock = 0;
	}
	തിരികെ 0;
    }
    അല്ലെങ്കിൽ
    {
	PyErr_FORMAT(PyExc_AttributeError, N_("cannot set attribute %s"), name);
	തിരികെ -1;
    }
}

സ്ഥിതം PySequenceMethods TupleAsSeq = {
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

സ്ഥിതം PyMappingMethods TupleAsMapping = {
    /* mp_length	*/ (lenfunc) TupleLength,
    /* mp_subscript     */ (binaryfunc) TupleItem,
    /* mp_ass_subscript */ (objobjargproc) 0,
};

സ്ഥിതം ഘടന PyMethodDef TupleMethods[] = {
    {"__dir__",	(PyCFunction)TupleDir,		METH_NOARGS,	""},
    { ശൂന്യ,	ശൂന്യ,				0,		ശൂന്യ}
};

തരനിർവചനം ഘടന
{
    PyObject_HEAD
    char_u	*name;
    പൂർണ്ണസംഖ്യ		argc;
    typval_T	*argv;
    dict_T	*self;
    pylinkedlist_T	ref;
    പൂർണ്ണസംഖ്യ		auto_rebind;
} FunctionObject;

DEFINE_PY_TYPE_OBJECT(FunctionType);

#define NEW_FUNCTION(name, argc, argv, self, pt_auto) \
    FunctionNew(FunctionTypePtr, (name), (argc), (argv), (self), (pt_auto))

    സ്ഥിതം PyObject *
FunctionNew(PyTypeObject *subtype, char_u *name, പൂർണ്ണസംഖ്യ argc, typval_T *argv,
	dict_T *selfdict, പൂർണ്ണസംഖ്യ auto_rebind)
{
    FunctionObject	*self;

    self = (FunctionObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    എങ്കിൽ (self == ശൂന്യ)
	തിരികെ ശൂന്യ;

    എങ്കിൽ (isdigit((unsigned അക്ഷരം)*name))
    {
	എങ്കിൽ (!translated_function_exists(name, FALSE))
	{
	    PyErr_FORMAT(Py_Exc_മൂല്യം_Error,
		    N_("unnamed function %s does not exist"), name);
	    തിരികെ ശൂന്യ;
	}
	self->name = vim_strsave(name);
    }
    അല്ലെങ്കിൽ
    {
	char_u *p;

	എങ്കിൽ ((p = get_expanded_name(name,
			    vim_strchr(name, AUTOLOAD_CHAR) == ശൂന്യ)) == ശൂന്യ)
	{
	    PyErr_FORMAT(Py_Exc_മൂല്യം_Error,
		    N_("function %s does not exist"), name);
	    തിരികെ ശൂന്യ;
	}

	എങ്കിൽ (p[0] == K_SPECIAL && p[1] == KS_EXTRA && p[2] == (പൂർണ്ണസംഖ്യ)KE_SNR)
	{
	    char_u *np;
	    size_t len = STRLEN(p) + 1;

	    എങ്കിൽ ((np = alloc(len + 2)) == ശൂന്യ)
	    {
		vim_free(p);
		തിരികെ ശൂന്യ;
	    }
	    mch_memmove(np, "<SNR>", 5);
	    mch_memmove(np + 5, p + 3, len - 3);
	    vim_free(p);
	    self->name = np;
	}
	അല്ലെങ്കിൽ
	    self->name = p;
    }

    func_ref(self->name);
    self->argc = argc;
    self->argv = argv;
    self->self = selfdict;
    self->auto_rebind = selfdict == ശൂന്യ ? TRUE : auto_rebind;

    എങ്കിൽ (self->argv || self->self)
	pyll_add((PyObject *)(self), &self->ref, &lastfunc);

    തിരികെ (PyObject *)(self);
}

    സ്ഥിതം PyObject *
FunctionConstructor(PyTypeObject *subtype, PyObject *args, PyObject *kwargs)
{
    PyObject	*self;
    PyObject	*selfdictObject;
    PyObject	*autoRebindObject;
    PyObject	*argsObject = ശൂന്യ;
    char_u	*name;
    typval_T	selfdicttv;
    typval_T	argstv;
    list_T	*argslist = ശൂന്യ;
    dict_T	*selfdict = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		argc = 0;
    പൂർണ്ണസംഖ്യ		auto_rebind = TRUE;
    typval_T	*argv = ശൂന്യ;
    typval_T	*curtv;
    listitem_T	*li;

    എങ്കിൽ (kwargs != ശൂന്യ)
    {
	selfdictObject = PyDict_GetItemString(kwargs, "self");
	എങ്കിൽ (selfdictObject != ശൂന്യ)
	{
	    എങ്കിൽ (ConvertFromPyMapping(selfdictObject, &selfdicttv) == -1)
		തിരികെ ശൂന്യ;
	    selfdict = selfdicttv.vval.v_dict;
	}
	argsObject = PyDict_GetItemString(kwargs, "args");
	എങ്കിൽ (argsObject != ശൂന്യ)
	{
	    എങ്കിൽ (ConvertFromPySequence(argsObject, &argstv) == -1)
	    {
		dict_unref(selfdict);
		തിരികെ ശൂന്യ;
	    }
	    argslist = argstv.vval.v_list;
	    CHECK_LIST_MATERIALIZE(argslist);

	    argc = argslist->lv_len;
	    എങ്കിൽ (argc != 0)
	    {
		argv = PyMem_New(typval_T, (size_t) argc);
		എങ്കിൽ (argv == ശൂന്യ)
		{
		    PyErr_NoMemory();
		    dict_unref(selfdict);
		    list_unref(argslist);
		    തിരികെ ശൂന്യ;
		}
		curtv = argv;
		FOR_ALL_LIST_ഇനങ്ങൾ(argslist, li)
		    copy_tv(&li->li_tv, curtv++);
	    }
	    list_unref(argslist);
	}
	എങ്കിൽ (selfdict != ശൂന്യ)
	{
	    auto_rebind = FALSE;
	    autoRebindObject = PyDict_GetItemString(kwargs, "auto_rebind");
	    എങ്കിൽ (autoRebindObject != ശൂന്യ)
	    {
		auto_rebind = PyObject_IsTrue(autoRebindObject);
		എങ്കിൽ (auto_rebind == -1)
		{
		    dict_unref(selfdict);
		    list_unref(argslist);
		    തിരികെ ശൂന്യ;
		}
	    }
	}
    }

    എങ്കിൽ (!PyArg_ParseTuple(args, "et", "ascii", &name))
    {
	dict_unref(selfdict);
	ആകുമ്പോൾ (argc--)
	    clear_tv(&argv[argc]);
	PyMem_Free(argv);
	തിരികെ ശൂന്യ;
    }

    self = FunctionNew(subtype, name, argc, argv, selfdict, auto_rebind);

    PyMem_Free(name);

    തിരികെ self;
}

    സ്ഥിതം ശൂന്യം
FunctionDestructor(PyObject *self_obj)
{
    FunctionObject *self = (FunctionObject*)self_obj;
    പൂർണ്ണസംഖ്യ i;
    func_unref(self->name);
    vim_free(self->name);
    വേണ്ടി (i = 0; i < self->argc; ++i)
	clear_tv(&self->argv[i]);
    PyMem_Free(self->argv);
    dict_unref(self->self);
    എങ്കിൽ (self->argv || self->self)
	pyll_remove(&self->ref, &lastfunc);

    DESTRUCTOR_FINISH(self);
}

സ്ഥിതം അക്ഷരം *FunctionAttrs[] = {
    "softspace", "args", "self", "auto_rebind",
    ശൂന്യ
};

    സ്ഥിതം PyObject *
FunctionDir(PyObject *self, PyObject *args UNUSED)
{
    തിരികെ ObjectDir(self, FunctionAttrs);
}

    സ്ഥിതം PyObject *
FunctionAttr(FunctionObject *self, അക്ഷരം *name)
{
    list_T *list;
    പൂർണ്ണസംഖ്യ i;
    എങ്കിൽ (strcmp(name, "name") == 0)
	തിരികെ PyString_FromString((അക്ഷരം *)(self->name));
    അല്ലെങ്കിൽ എങ്കിൽ (strcmp(name, "args") == 0)
    {
	എങ്കിൽ (self->argv == ശൂന്യ || (list = list_alloc()) == ശൂന്യ)
	    തിരികെ ALWAYS_NONE;

	വേണ്ടി (i = 0; i < self->argc; ++i)
	    list_append_tv(list, &self->argv[i]);
	തിരികെ NEW_LIST(list);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (strcmp(name, "self") == 0)
	തിരികെ self->self == ശൂന്യ
	    ? ALWAYS_NONE
	    : NEW_DICTIONARY(self->self);
    അല്ലെങ്കിൽ എങ്കിൽ (strcmp(name, "auto_rebind") == 0)
	തിരികെ self->auto_rebind
	    ? ALWAYS_TRUE
	    : ALWAYS_FALSE;
    അല്ലെങ്കിൽ എങ്കിൽ (strcmp(name, "__members__") == 0)
	തിരികെ ObjectDir(ശൂന്യ, FunctionAttrs);
    തിരികെ ശൂന്യ;
}

/*
 * Populate partial_T given function object.
 *
 * "exported" should be set to true when it is needed to construct a partial
 * that may be stored in a variable (i.e. may be freed by Vim).
 */
    സ്ഥിതം ശൂന്യം
set_partial(FunctionObject *self, partial_T *pt, പൂർണ്ണസംഖ്യ exported)
{
    പൂർണ്ണസംഖ്യ i;

    pt->pt_name = self->name;
    എങ്കിൽ (self->argv)
    {
	pt->pt_argc = self->argc;
	എങ്കിൽ (exported)
	{
	    pt->pt_argv = ALLOC_CLEAR_MULT(typval_T, self->argc);
	    വേണ്ടി (i = 0; i < pt->pt_argc; ++i)
		copy_tv(&self->argv[i], &pt->pt_argv[i]);
	}
	അല്ലെങ്കിൽ
	    pt->pt_argv = self->argv;
    }
    അല്ലെങ്കിൽ
    {
	pt->pt_argc = 0;
	pt->pt_argv = ശൂന്യ;
    }
    pt->pt_auto = self->auto_rebind || !exported;
    pt->pt_dict = self->self;
    എങ്കിൽ (exported && self->self)
	++pt->pt_dict->dv_refcount;
    എങ്കിൽ (exported)
	pt->pt_name = vim_strsave(pt->pt_name);
    pt->pt_refcount = 1;
}

    സ്ഥിതം PyObject *
FunctionCall(PyObject *self_obj, PyObject *argsObject, PyObject *kwargs)
{
    FunctionObject *self = (FunctionObject*)self_obj;
    char_u	*name = self->name;
    typval_T	args;
    typval_T	selfdicttv;
    typval_T	rettv;
    dict_T	*selfdict = ശൂന്യ;
    PyObject	*selfdictObject;
    PyObject	*ret;
    പൂർണ്ണസംഖ്യ		error;
    partial_T	pt;
    partial_T	*pt_ptr = ശൂന്യ;

    എങ്കിൽ (ConvertFromPySequence(argsObject, &args) == -1)
	തിരികെ ശൂന്യ;

    എങ്കിൽ (kwargs != ശൂന്യ)
    {
	selfdictObject = PyDict_GetItemString(kwargs, "self");
	എങ്കിൽ (selfdictObject != ശൂന്യ)
	{
	    എങ്കിൽ (ConvertFromPyMapping(selfdictObject, &selfdicttv) == -1)
	    {
		clear_tv(&args);
		തിരികെ ശൂന്യ;
	    }
	    selfdict = selfdicttv.vval.v_dict;
	}
    }

    എങ്കിൽ (self->argv || self->self)
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

    എങ്കിൽ (VimTryEnd())
	ret = ശൂന്യ;
    അല്ലെങ്കിൽ എങ്കിൽ (error != OK)
    {
	ret = ശൂന്യ;
	PyErr_VIM_FORMAT(N_("failed to run function %s"), (അക്ഷരം *)name);
    }
    അല്ലെങ്കിൽ
	ret = ConvertToPyObject(&rettv);

    clear_tv(&args);
    clear_tv(&rettv);
    എങ്കിൽ (selfdict != ശൂന്യ)
	clear_tv(&selfdicttv);

    തിരികെ ret;
}

    സ്ഥിതം PyObject *
FunctionRepr(PyObject *self_obj)
{
    FunctionObject *self = (FunctionObject*)self_obj;
    PyObject *ret;
    garray_T repr_ga;
    പൂർണ്ണസംഖ്യ i;
    char_u *tofree = ശൂന്യ;
    typval_T tv;
    char_u numbuf[NUMBUFLEN];

    ga_init2(&repr_ga, വലുപ്പം(അക്ഷരം), 70);
    ga_concat(&repr_ga, (char_u *)"<vim.Function '");
    എങ്കിൽ (self->name)
	ga_concat(&repr_ga, self->name);
    അല്ലെങ്കിൽ
	ga_concat(&repr_ga, (char_u *)"<NULL>");
    ga_append(&repr_ga, '\'');
    എങ്കിൽ (self->argv)
    {
	ga_concat(&repr_ga, (char_u *)", args=[");
	++emsg_silent;
	വേണ്ടി (i = 0; i < self->argc; i++)
	{
	    എങ്കിൽ (i != 0)
		ga_concat(&repr_ga, (char_u *)", ");
	    ga_concat(&repr_ga, tv2string(&self->argv[i], &tofree, numbuf,
			get_copyID()));
	    vim_free(tofree);
	}
	--emsg_silent;
	ga_append(&repr_ga, ']');
    }
    എങ്കിൽ (self->self)
    {
	ga_concat(&repr_ga, (char_u *)", self=");
	tv.v_type = VAR_DICT;
	tv.vval.v_dict = self->self;
	++emsg_silent;
	ga_concat(&repr_ga, tv2string(&tv, &tofree, numbuf, get_copyID()));
	--emsg_silent;
	vim_free(tofree);
	എങ്കിൽ (self->auto_rebind)
	    ga_concat(&repr_ga, (char_u *)", auto_rebind=True");
    }
    ga_append(&repr_ga, '>');
    ret = PyString_FromString((അക്ഷരം *)repr_ga.ga_data);
    ga_clear(&repr_ga);
    തിരികെ ret;
}

സ്ഥിതം ഘടന PyMethodDef FunctionMethods[] = {
    {"__dir__",	(PyCFunction)FunctionDir,   METH_NOARGS,		""},
    { ശൂന്യ,	ശൂന്യ,			0,				ശൂന്യ}
};

/*
 * Options object
 */

DEFINE_PY_TYPE_OBJECT(OptionsType);

തരനിർവചനം പൂർണ്ണസംഖ്യ (*checkfun)(ശൂന്യം *);

തരനിർവചനം ഘടന
{
    PyObject_HEAD
    പൂർണ്ണസംഖ്യ		opt_type;
    ശൂന്യം	*from;
    checkfun	Check;
    PyObject	*fromObj;
} OptionsObject;

    സ്ഥിതം പൂർണ്ണസംഖ്യ
dummy_check(ശൂന്യം *arg UNUSED)
{
    തിരികെ 0;
}

    സ്ഥിതം PyObject *
OptionsNew(പൂർണ്ണസംഖ്യ opt_type, ശൂന്യം *from, checkfun Check, PyObject *fromObj)
{
    OptionsObject	*self;

    self = PyObject_GC_New(OptionsObject, OptionsTypePtr);
    എങ്കിൽ (self == ശൂന്യ)
	തിരികെ ശൂന്യ;

    self->opt_type = opt_type;
    self->from = from;
    self->Check = Check;
    self->fromObj = fromObj;
    എങ്കിൽ (fromObj)
	Py_INCREF(fromObj);

    തിരികെ (PyObject *)(self);
}

    സ്ഥിതം ശൂന്യം
OptionsDestructor(PyObject *self_obj)
{
    OptionsObject *self = (OptionsObject*)self_obj;
    PyObject_GC_UnTrack((ശൂന്യം *)(self));
    Py_XDECREF(self->fromObj);
    PyObject_GC_Del((ശൂന്യം *)(self));
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
OptionsTraverse(PyObject *self_obj, visitproc visit, ശൂന്യം *arg)
{
    OptionsObject *self = (OptionsObject*)self_obj;
    Py_VISIT(self->fromObj);
    തിരികെ 0;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
OptionsClear(PyObject *self_obj)
{
    OptionsObject *self = (OptionsObject*)self_obj;
    Py_CLEAR(self->fromObj);
    തിരികെ 0;
}

    സ്ഥിതം PyObject *
OptionsItem(OptionsObject *self, PyObject *താക്കോൽ_Object)
{
    char_u	*താക്കോൽ;
    പൂർണ്ണസംഖ്യ		flags;
    long	numval;
    char_u	*stringval;
    PyObject	*todecref;

    എങ്കിൽ (self->Check(self->fromObj))
	തിരികെ ശൂന്യ;

    എങ്കിൽ (!(താക്കോൽ = StringToChars(താക്കോൽ_Object, &todecref)))
	തിരികെ ശൂന്യ;

    എങ്കിൽ (*താക്കോൽ == NUL)
    {
	RAISE_NO_EMPTY_താക്കോലുകൾ;
	Py_XDECREF(todecref);
	തിരികെ ശൂന്യ;
    }

    flags = get_option_മൂല്യം_strict(താക്കോൽ, &numval, &stringval,
				    self->opt_type, self->from);

    Py_XDECREF(todecref);

    എങ്കിൽ (flags == 0)
    {
	PyErr_SetObject(Py_Exc_താക്കോൽ_Error, താക്കോൽ_Object);
	തിരികെ ശൂന്യ;
    }

    എങ്കിൽ (flags & SOPT_UNSET)
    {
	Py_INCREF(Py_None);
	തിരികെ Py_None;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (flags & SOPT_BOOL)
    {
	PyObject	*ret;
	ret = numval ? Py_True : Py_False;
	Py_INCREF(ret);
	തിരികെ ret;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (flags & SOPT_അക്കം)
	തിരികെ PyInt_FromLong(numval);
    അല്ലെങ്കിൽ എങ്കിൽ (flags & SOPT_STRING)
    {
	എങ്കിൽ (stringval)
	{
	    PyObject	*ret = PyBytes_FromString((അക്ഷരം *)stringval);
	    vim_free(stringval);
	    തിരികെ ret;
	}
	അല്ലെങ്കിൽ
	{
	    PyErr_SET_STRING(PyExc_RuntimeError,
		    N_("unable to get option value"));
	    തിരികെ ശൂന്യ;
	}
    }
    അല്ലെങ്കിൽ
    {
	PyErr_SET_VIM(N_("internal error: unknown option type"));
	തിരികെ ശൂന്യ;
    }
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
OptionsContains(OptionsObject *self, PyObject *താക്കോൽ_Object)
{
    char_u	*താക്കോൽ;
    PyObject	*todecref;

    എങ്കിൽ (!(താക്കോൽ = StringToChars(താക്കോൽ_Object, &todecref)))
	തിരികെ -1;

    എങ്കിൽ (*താക്കോൽ == NUL)
    {
	Py_XDECREF(todecref);
	തിരികെ 0;
    }

    എങ്കിൽ (get_option_മൂല്യം_strict(താക്കോൽ, ശൂന്യ, ശൂന്യ, self->opt_type, ശൂന്യ))
    {
	Py_XDECREF(todecref);
	തിരികെ 1;
    }
    അല്ലെങ്കിൽ
    {
	Py_XDECREF(todecref);
	തിരികെ 0;
    }
}

തരനിർവചനം ഘടന
{
    ശൂന്യം	*lastoption;
    പൂർണ്ണസംഖ്യ		opt_type;
} optiterinfo_T;

    സ്ഥിതം PyObject *
OptionsIterNext(ശൂന്യം **arg)
{
    char_u	*name;
    optiterinfo_T **oii = (optiterinfo_T**)arg;

    എങ്കിൽ ((name = option_iter_next(&((*oii)->lastoption), (*oii)->opt_type)))
	തിരികെ PyString_FromString((അക്ഷരം *)name);

    തിരികെ ശൂന്യ;
}

    സ്ഥിതം PyObject *
OptionsIter(PyObject *self_obj)
{
    OptionsObject	*self = (OptionsObject*)self_obj;
    optiterinfo_T	*oii;

    എങ്കിൽ (!(oii = PyMem_New(optiterinfo_T, 1)))
    {
	PyErr_NoMemory();
	തിരികെ ശൂന്യ;
    }

    oii->opt_type = self->opt_type;
    oii->lastoption = ശൂന്യ;

    തിരികെ IterNew(oii,
	    PyMem_Free, OptionsIterNext,
	    ശൂന്യ, ശൂന്യ, (PyObject *)self);
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
set_option_മൂല്യം_err(char_u *താക്കോൽ, പൂർണ്ണസംഖ്യ numval, char_u *stringval, പൂർണ്ണസംഖ്യ opt_flags)
{
    അക്ഷരം	*errmsg;

    എങ്കിൽ ((errmsg = set_option_മൂല്യം(താക്കോൽ, numval, stringval, opt_flags)))
    {
	എങ്കിൽ (VimTryEnd())
	    തിരികെ FAIL;
	PyErr_SetVim(errmsg);
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
set_option_മൂല്യം_for(
	char_u *താക്കോൽ,
	പൂർണ്ണസംഖ്യ numval,
	char_u *stringval,
	പൂർണ്ണസംഖ്യ opt_flags,
	പൂർണ്ണസംഖ്യ opt_type,
	ശൂന്യം *from)
{
    switchwin_T	switchwin;
    bufref_T	save_curbuf;
    പൂർണ്ണസംഖ്യ		set_ret = 0;

    VimTryStart();
    തിരഞ്ഞെടുപ്പ് (opt_type)
    {
	അവസ്ഥ SREQ_WIN:
	    എങ്കിൽ (switch_win(&switchwin, (win_T *)from,
			      win_find_tabpage((win_T *)from), FALSE) == FAIL)
	    {
		restore_win(&switchwin, TRUE);
		എങ്കിൽ (VimTryEnd())
		    തിരികെ -1;
		PyErr_SET_VIM(N_("problem while switching windows"));
		തിരികെ -1;
	    }
	    set_ret = set_option_മൂല്യം_err(താക്കോൽ, numval, stringval, opt_flags);
	    restore_win(&switchwin, TRUE);
	    നിർത്തുക;
	അവസ്ഥ SREQ_BUF:
	    switch_buffer(&save_curbuf, (buf_T *)from);
	    set_ret = set_option_മൂല്യം_err(താക്കോൽ, numval, stringval, opt_flags);
	    restore_buffer(&save_curbuf);
	    നിർത്തുക;
	അവസ്ഥ SREQ_GLOBAL:
	    set_ret = set_option_മൂല്യം_err(താക്കോൽ, numval, stringval, opt_flags);
	    നിർത്തുക;
    }
    എങ്കിൽ (set_ret == FAIL)
	തിരികെ -1;
    തിരികെ VimTryEnd();
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
OptionsAssItem(OptionsObject *self, PyObject *താക്കോൽ_Object, PyObject *valObject)
{
    char_u	*താക്കോൽ;
    പൂർണ്ണസംഖ്യ		flags;
    പൂർണ്ണസംഖ്യ		opt_flags;
    പൂർണ്ണസംഖ്യ		ret = 0;
    PyObject	*todecref;

    എങ്കിൽ (self->Check(self->fromObj))
	തിരികെ -1;

    എങ്കിൽ (!(താക്കോൽ = StringToChars(താക്കോൽ_Object, &todecref)))
	തിരികെ -1;

    എങ്കിൽ (*താക്കോൽ == NUL)
    {
	RAISE_NO_EMPTY_താക്കോലുകൾ;
	Py_XDECREF(todecref);
	തിരികെ -1;
    }

    flags = get_option_മൂല്യം_strict(താക്കോൽ, ശൂന്യ, ശൂന്യ,
				    self->opt_type, self->from);

    എങ്കിൽ (flags == 0)
    {
	PyErr_SetObject(Py_Exc_താക്കോൽ_Error, താക്കോൽ_Object);
	Py_XDECREF(todecref);
	തിരികെ -1;
    }

    എങ്കിൽ (valObject == ശൂന്യ)
    {
	എങ്കിൽ (self->opt_type == SREQ_GLOBAL)
	{
	    PyErr_FORMAT(Py_Exc_മൂല്യം_Error,
		    N_("unable to unset global option %s"), താക്കോൽ);
	    Py_XDECREF(todecref);
	    തിരികെ -1;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (!(flags & SOPT_GLOBAL))
	{
	    PyErr_FORMAT(Py_Exc_മൂല്യം_Error,
		    N_("unable to unset option %s "
		       "which does not have global value"), താക്കോൽ);
	    Py_XDECREF(todecref);
	    തിരികെ -1;
	}
	അല്ലെങ്കിൽ
	{
	    unset_global_local_option(താക്കോൽ, self->from);
	    Py_XDECREF(todecref);
	    തിരികെ 0;
	}
    }

    opt_flags = (self->opt_type ? OPT_LOCAL : OPT_GLOBAL);

    എങ്കിൽ (flags & SOPT_BOOL)
    {
	പൂർണ്ണസംഖ്യ	istrue = PyObject_IsTrue(valObject);

	എങ്കിൽ (istrue == -1)
	    ret = -1;
	അല്ലെങ്കിൽ
	    ret = set_option_മൂല്യം_for(താക്കോൽ, istrue, ശൂന്യ,
				    opt_flags, self->opt_type, self->from);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (flags & SOPT_അക്കം)
    {
	long	val;

	എങ്കിൽ (സംഖ്യ_To_Long(valObject, &val, സംഖ്യ_INT))
	{
	    Py_XDECREF(todecref);
	    തിരികെ -1;
	}

	ret = set_option_മൂല്യം_for(താക്കോൽ, (പൂർണ്ണസംഖ്യ) val, ശൂന്യ, opt_flags,
				self->opt_type, self->from);
    }
    അല്ലെങ്കിൽ
    {
	char_u		*val;
	PyObject	*todecref2;

	എങ്കിൽ ((val = StringToChars(valObject, &todecref2)))
	{
	    ret = set_option_മൂല്യം_for(താക്കോൽ, 0, val, opt_flags,
				    self->opt_type, self->from);
	    Py_XDECREF(todecref2);
	}
	അല്ലെങ്കിൽ
	    ret = -1;
    }

    Py_XDECREF(todecref);

    തിരികെ ret;
}

സ്ഥിതം PySequenceMethods OptionsAsSeq = {
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

സ്ഥിതം PyMappingMethods OptionsAsMapping = {
    (lenfunc)       ശൂന്യ,
    (binaryfunc)    OptionsItem,
    (objobjargproc) OptionsAssItem,
};

// Tabpage object

തരനിർവചനം ഘടന
{
    PyObject_HEAD
    tabpage_T	*tab;
} TabPageObject;

സ്ഥിതം PyObject *WinListNew(TabPageObject *tabObject);

DEFINE_PY_TYPE_OBJECT(TabPageType);

    സ്ഥിതം പൂർണ്ണസംഖ്യ
CheckTabPage(TabPageObject *self)
{
    എങ്കിൽ (self->tab == INVALID_TABPAGE_മൂല്യം)
    {
	PyErr_SET_VIM(N_("attempt to refer to deleted tab page"));
	തിരികെ -1;
    }

    തിരികെ 0;
}

    സ്ഥിതം PyObject *
TabPageNew(tabpage_T *tab)
{
    TabPageObject *self;

    എങ്കിൽ (TAB_PYTHON_REF(tab))
    {
	self = TAB_PYTHON_REF(tab);
	Py_INCREF((PyObject *)self);
    }
    അല്ലെങ്കിൽ
    {
	self = PyObject_NEW(TabPageObject, TabPageTypePtr);
	എങ്കിൽ (self == ശൂന്യ)
	    തിരികെ ശൂന്യ;
	self->tab = tab;
	TAB_PYTHON_REF(tab) = self;
    }

    തിരികെ (PyObject *)(self);
}

    സ്ഥിതം ശൂന്യം
TabPageDestructor(PyObject *self_obj)
{
    TabPageObject *self = (TabPageObject*)self_obj;
    എങ്കിൽ (self->tab && self->tab != INVALID_TABPAGE_മൂല്യം)
	TAB_PYTHON_REF(self->tab) = ശൂന്യ;

    DESTRUCTOR_FINISH(self);
}

സ്ഥിതം അക്ഷരം *TabPageAttrs[] = {
    "windows", "number", "vars", "window", "valid",
    ശൂന്യ
};

    സ്ഥിതം PyObject *
TabPageDir(PyObject *self, PyObject *args UNUSED)
{
    തിരികെ ObjectDir(self, TabPageAttrs);
}

    സ്ഥിതം PyObject *
TabPageAttrValid(TabPageObject *self, അക്ഷരം *name)
{
    PyObject	*ret;

    എങ്കിൽ (strcmp(name, "valid") != 0)
	തിരികെ ശൂന്യ;

    ret = ((self->tab == INVALID_TABPAGE_മൂല്യം) ? Py_False : Py_True);
    Py_INCREF(ret);
    തിരികെ ret;
}

    സ്ഥിതം PyObject *
TabPageAttr(TabPageObject *self, അക്ഷരം *name)
{
    എങ്കിൽ (strcmp(name, "windows") == 0)
	തിരികെ WinListNew(self);
    അല്ലെങ്കിൽ എങ്കിൽ (strcmp(name, "number") == 0)
	തിരികെ PyLong_FromLong((long) get_tab_സംഖ്യ(self->tab));
    അല്ലെങ്കിൽ എങ്കിൽ (strcmp(name, "vars") == 0)
	തിരികെ NEW_DICTIONARY(self->tab->tp_vars);
    അല്ലെങ്കിൽ എങ്കിൽ (strcmp(name, "window") == 0)
    {
	// For current tab window.c does not bother to set or update tp_curwin
	എങ്കിൽ (self->tab == curtab)
	    തിരികെ WindowNew(curwin, curtab);
	അല്ലെങ്കിൽ
	    തിരികെ WindowNew(self->tab->tp_curwin, self->tab);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (strcmp(name, "__members__") == 0)
	തിരികെ ObjectDir(ശൂന്യ, TabPageAttrs);
    തിരികെ ശൂന്യ;
}

    സ്ഥിതം PyObject *
TabPageRepr(PyObject *self_obj)
{
    TabPageObject *self = (TabPageObject*)self_obj;
    എങ്കിൽ (self->tab == INVALID_TABPAGE_മൂല്യം)
	തിരികെ PyString_FromFormat("<tabpage object (deleted) at %p>", (ശൂന്യം *)self);
    അല്ലെങ്കിൽ
    {
	പൂർണ്ണസംഖ്യ	t = get_tab_സംഖ്യ(self->tab);

	എങ്കിൽ (t == 0)
	    തിരികെ PyString_FromFormat("<tabpage object (unknown) at %p>",
					(ശൂന്യം *)self);
	അല്ലെങ്കിൽ
	    തിരികെ PyString_FromFormat("<tabpage %d>", t - 1);
    }
}

സ്ഥിതം ഘടന PyMethodDef TabPageMethods[] = {
    // name,	    function,			calling,	documentation
    {"__dir__",	    (PyCFunction)TabPageDir,	METH_NOARGS,	""},
    { ശൂന്യ,	    ശൂന്യ,			0,		ശൂന്യ}
};

/*
 * Window list object
 */

DEFINE_PY_TYPE_OBJECT(TabListType);
സ്ഥിതം PySequenceMethods TabListAsSeq;

തരനിർവചനം ഘടന
{
    PyObject_HEAD
} TabListObject;

സ്ഥിതം TabListObject TheTabPageList =
{
    PyObject_HEAD_INIT_TYPE(TabListType)
};

    സ്ഥിതം PyInt
TabListLength(PyObject *self UNUSED)
{
    tabpage_T	*tp = first_tabpage;
    PyInt	n = 0;

    ആകുമ്പോൾ (tp != ശൂന്യ)
    {
	++n;
	tp = tp->tp_next;
    }

    തിരികെ n;
}

    സ്ഥിതം PyObject *
TabListItem(PyObject *self UNUSED, PyInt n)
{
    tabpage_T	*tp;

    വേണ്ടി (tp = first_tabpage; tp != ശൂന്യ; tp = tp->tp_next, --n)
	എങ്കിൽ (n == 0)
	    തിരികെ TabPageNew(tp);

    PyErr_SET_STRING(PyExc_IndexError, N_("no such tab page"));
    തിരികെ ശൂന്യ;
}

/*
 * Window object
 */

തരനിർവചനം ഘടന
{
    PyObject_HEAD
    win_T	*win;
    TabPageObject	*tabObject;
} WindowObject;

DEFINE_PY_TYPE_OBJECT(WindowType);

    സ്ഥിതം പൂർണ്ണസംഖ്യ
CheckWindow(WindowObject *self)
{
    എങ്കിൽ (self->win == INVALID_WINDOW_മൂല്യം)
    {
	PyErr_SET_VIM(N_("attempt to refer to deleted window"));
	തിരികെ -1;
    }

    തിരികെ 0;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
CheckWindowCb(ശൂന്യം *self)
{
    തിരികെ CheckWindow((WindowObject*)self);
}

    സ്ഥിതം PyObject *
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

    എങ്കിൽ (WIN_PYTHON_REF(win))
    {
	self = WIN_PYTHON_REF(win);
	Py_INCREF((PyObject *)self);
    }
    അല്ലെങ്കിൽ
    {
	self = PyObject_GC_New(WindowObject, WindowTypePtr);
	എങ്കിൽ (self == ശൂന്യ)
	    തിരികെ ശൂന്യ;
	self->win = win;
	WIN_PYTHON_REF(win) = self;
    }

    self->tabObject = ((TabPageObject *)(TabPageNew(tab)));

    തിരികെ (PyObject *)(self);
}

    സ്ഥിതം ശൂന്യം
WindowDestructor(PyObject *self_obj)
{
    WindowObject *self = (WindowObject*)self_obj;
    PyObject_GC_UnTrack((ശൂന്യം *)(self));
    എങ്കിൽ (self->win && self->win != INVALID_WINDOW_മൂല്യം)
	WIN_PYTHON_REF(self->win) = ശൂന്യ;
    Py_XDECREF(((PyObject *)(self->tabObject)));
    PyObject_GC_Del((ശൂന്യം *)(self));
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
WindowTraverse(PyObject *self_obj, visitproc visit, ശൂന്യം *arg)
{
    WindowObject *self = (WindowObject*)self_obj;
    Py_VISIT(((PyObject *)(self->tabObject)));
    തിരികെ 0;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
WindowClear(PyObject *self_obj)
{
    WindowObject *self = (WindowObject*)self_obj;
    Py_CLEAR(self->tabObject);
    തിരികെ 0;
}

    സ്ഥിതം win_T *
get_firstwin(TabPageObject *tabObject)
{
    എങ്കിൽ (tabObject)
    {
	എങ്കിൽ (CheckTabPage(tabObject))
	    തിരികെ ശൂന്യ;
	// For current tab window.c does not bother to set or update tp_firstwin
	അല്ലെങ്കിൽ എങ്കിൽ (tabObject->tab == curtab)
	    തിരികെ firstwin;
	അല്ലെങ്കിൽ
	    തിരികെ tabObject->tab->tp_firstwin;
    }
    അല്ലെങ്കിൽ
	തിരികെ firstwin;
}

// Use the same order as in the WindowAttr() function.
സ്ഥിതം അക്ഷരം *WindowAttrs[] = {
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
    ശൂന്യ
};

    സ്ഥിതം PyObject *
WindowDir(PyObject *self, PyObject *args UNUSED)
{
    തിരികെ ObjectDir(self, WindowAttrs);
}

    സ്ഥിതം PyObject *
WindowAttrValid(WindowObject *self, അക്ഷരം *name)
{
    PyObject	*ret;

    എങ്കിൽ (strcmp(name, "valid") != 0)
	തിരികെ ശൂന്യ;

    ret = ((self->win == INVALID_WINDOW_മൂല്യം) ? Py_False : Py_True);
    Py_INCREF(ret);
    തിരികെ ret;
}

    സ്ഥിതം PyObject *
WindowAttr(WindowObject *self, അക്ഷരം *name)
{
    എങ്കിൽ (strcmp(name, "buffer") == 0)
	തിരികെ (PyObject *)BufferNew(self->win->w_buffer);
    അല്ലെങ്കിൽ എങ്കിൽ (strcmp(name, "cursor") == 0)
    {
	pos_T *pos = &self->win->w_cursor;

	തിരികെ Py_Build_മൂല്യം("(ll)", (long)(pos->lnum), (long)(pos->col));
    }
    അല്ലെങ്കിൽ എങ്കിൽ (strcmp(name, "height") == 0)
	തിരികെ PyLong_FromLong((long)(self->win->w_height));
    അല്ലെങ്കിൽ എങ്കിൽ (strcmp(name, "row") == 0)
	തിരികെ PyLong_FromLong((long)(self->win->w_winrow));
    അല്ലെങ്കിൽ എങ്കിൽ (strcmp(name, "width") == 0)
	തിരികെ PyLong_FromLong((long)(self->win->w_width));
    അല്ലെങ്കിൽ എങ്കിൽ (strcmp(name, "col") == 0)
	തിരികെ PyLong_FromLong((long)(self->win->w_wincol));
    അല്ലെങ്കിൽ എങ്കിൽ (strcmp(name, "vars") == 0)
	തിരികെ NEW_DICTIONARY(self->win->w_vars);
    അല്ലെങ്കിൽ എങ്കിൽ (strcmp(name, "options") == 0)
	തിരികെ OptionsNew(SREQ_WIN, self->win, CheckWindowCb,
			(PyObject *) self);
    അല്ലെങ്കിൽ എങ്കിൽ (strcmp(name, "number") == 0)
    {
	എങ്കിൽ (CheckTabPage(self->tabObject))
	    തിരികെ ശൂന്യ;
	തിരികെ PyLong_FromLong((long)
		get_win_സംഖ്യ(self->win, get_firstwin(self->tabObject)));
    }
    അല്ലെങ്കിൽ എങ്കിൽ (strcmp(name, "tabpage") == 0)
    {
	Py_INCREF((PyObject *)self->tabObject);
	തിരികെ (PyObject *)(self->tabObject);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (strcmp(name, "__members__") == 0)
	തിരികെ ObjectDir(ശൂന്യ, WindowAttrs);
    അല്ലെങ്കിൽ
	തിരികെ ശൂന്യ;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
WindowSetattr(PyObject *self_obj, അക്ഷരം *name, PyObject *valObject)
{
    WindowObject *self = (WindowObject*)self_obj;
    എങ്കിൽ (CheckWindow(self))
	തിരികെ -1;

    എങ്കിൽ (strcmp(name, "buffer") == 0)
    {
	PyErr_SET_STRING(PyExc_TypeError, N_("readonly attribute: buffer"));
	തിരികെ -1;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (strcmp(name, "cursor") == 0)
    {
	long lnum;
	long col;

	എങ്കിൽ (!PyArg_Parse(valObject, "(ll)", &lnum, &col))
	    തിരികെ -1;

	എങ്കിൽ (lnum <= 0 || lnum > self->win->w_buffer->b_ml.ml_വരി_എണ്ണം)
	{
	    PyErr_SET_VIM(N_("cursor position outside buffer"));
	    തിരികെ -1;
	}

	// Check for keyboard interrupts
	എങ്കിൽ (VimCheckInterrupt())
	    തിരികെ -1;

	self->win->w_cursor.lnum = lnum;
	self->win->w_cursor.col = col;
	self->win->w_set_curswant = TRUE;
	self->win->w_cursor.coladd = 0;
	// When column is out of range silently correct it.
	check_cursor_col_win(self->win);

	update_screen(UPD_VALID);
	തിരികെ 0;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (strcmp(name, "height") == 0)
    {
	long	height;
	win_T	*savewin;

	എങ്കിൽ (സംഖ്യ_To_Long(valObject, &height, സംഖ്യ_INT|സംഖ്യ_UNSIGNED))
	    തിരികെ -1;

#ifdef FEAT_GUI
	need_mouse_correct = TRUE;
#endif
	savewin = curwin;
	curwin = self->win;
	curbuf = curwin->w_buffer;

	VimTryStart();
	win_setheight((പൂർണ്ണസംഖ്യ) height);
	curwin = savewin;
	curbuf = curwin->w_buffer;
	എങ്കിൽ (VimTryEnd())
	    തിരികെ -1;

	തിരികെ 0;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (strcmp(name, "width") == 0)
    {
	long	width;
	win_T	*savewin;

	എങ്കിൽ (സംഖ്യ_To_Long(valObject, &width, സംഖ്യ_INT|സംഖ്യ_UNSIGNED))
	    തിരികെ -1;

#ifdef FEAT_GUI
	need_mouse_correct = TRUE;
#endif
	savewin = curwin;
	curwin = self->win;
	curbuf = curwin->w_buffer;

	VimTryStart();
	win_setwidth((പൂർണ്ണസംഖ്യ) width);
	curwin = savewin;
	curbuf = curwin->w_buffer;
	എങ്കിൽ (VimTryEnd())
	    തിരികെ -1;

	തിരികെ 0;
    }
    അല്ലെങ്കിൽ
    {
	PyErr_SetString(PyExc_AttributeError, name);
	തിരികെ -1;
    }
}

    സ്ഥിതം PyObject *
WindowRepr(PyObject *self_obj)
{
    WindowObject *self = (WindowObject*)self_obj;
    എങ്കിൽ (self->win == INVALID_WINDOW_മൂല്യം)
	തിരികെ PyString_FromFormat("<window object (deleted) at %p>", (ശൂന്യം *)self);
    അല്ലെങ്കിൽ
    {
	പൂർണ്ണസംഖ്യ	w = get_win_സംഖ്യ(self->win, firstwin);

	എങ്കിൽ (w == 0)
	    തിരികെ PyString_FromFormat("<window object (unknown) at %p>",
								      (ശൂന്യം *)self);
	അല്ലെങ്കിൽ
	    തിരികെ PyString_FromFormat("<window %d>", w - 1);
    }
}

സ്ഥിതം ഘടന PyMethodDef WindowMethods[] = {
    // name,	    function,			calling,	documentation
    {"__dir__",	    (PyCFunction)WindowDir,	METH_NOARGS,	""},
    { ശൂന്യ,	    ശൂന്യ,			0,		ശൂന്യ}
};

/*
 * Window list object
 */

DEFINE_PY_TYPE_OBJECT(WinListType);
സ്ഥിതം PySequenceMethods WinListAsSeq;

തരനിർവചനം ഘടന
{
    PyObject_HEAD
    TabPageObject	*tabObject;
} WinListObject;

സ്ഥിതം WinListObject TheWindowList =
{
    PyObject_HEAD_INIT_TYPE(WinListType)
    ശൂന്യ
};

    സ്ഥിതം PyObject *
WinListNew(TabPageObject *tabObject)
{
    WinListObject	*self;

    self = PyObject_NEW(WinListObject, WinListTypePtr);
    self->tabObject = tabObject;
    Py_INCREF((PyObject *)tabObject);

    തിരികെ (PyObject *)(self);
}

    സ്ഥിതം ശൂന്യം
WinListDestructor(WinListObject *self)
{
    TabPageObject	*tabObject = self->tabObject;

    എങ്കിൽ (tabObject)
    {
	Py_DECREF((PyObject *)(tabObject));
    }

    DESTRUCTOR_FINISH(self);
}

    സ്ഥിതം PyInt
WinListLength(WinListObject *self)
{
    win_T	*w;
    PyInt	n = 0;

    എങ്കിൽ (!(w = get_firstwin(self->tabObject)))
	തിരികെ -1;

    ആകുമ്പോൾ (w != ശൂന്യ)
    {
	++n;
	w = W_NEXT(w);
    }

    തിരികെ n;
}

    സ്ഥിതം PyObject *
WinListItem(WinListObject *self, PyInt n)
{
    win_T *w;

    എങ്കിൽ (!(w = get_firstwin(self->tabObject)))
	തിരികെ ശൂന്യ;

    വേണ്ടി (; w != ശൂന്യ; w = W_NEXT(w), --n)
	എങ്കിൽ (n == 0)
	    തിരികെ WindowNew(w, self->tabObject? self->tabObject->tab: curtab);

    PyErr_SET_STRING(PyExc_IndexError, N_("no such window"));
    തിരികെ ശൂന്യ;
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
    സ്ഥിതം അക്ഷരം *
String_To_വരി(PyObject *obj)
{
    അക്ഷരം	*str;
    അക്ഷരം	*save;
    PyObject	*bytes = ശൂന്യ;
    Py_ssize_t	len = 0;
    PyInt	i;
    അക്ഷരം	*p;

    എങ്കിൽ (PyBytes_Check(obj))
    {
	എങ്കിൽ (PyBytes_AsStringAndSize(obj, &str, &len) == -1
		|| str == ശൂന്യ)
	    തിരികെ ശൂന്യ;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (PyUnicode_Check(obj))
    {
	എങ്കിൽ (!(bytes = PyUnicode_AsEncodedString(obj, ENC_OPT,
							   ERRORS_ENCODE_ARG)))
	    തിരികെ ശൂന്യ;

	എങ്കിൽ (PyBytes_AsStringAndSize(bytes, &str, &len) == -1
		|| str == ശൂന്യ)
	{
	    Py_DECREF(bytes);
	    തിരികെ ശൂന്യ;
	}
    }
    അല്ലെങ്കിൽ
    {
#എങ്കിൽ PY_MAJOR_VERSION < 3
	PyErr_FORMAT_TYPE(
		N_("expected str() or unicode() instance, but got %s"),
		obj);
#അല്ലെങ്കിൽ
	PyErr_FORMAT_TYPE(
		N_("expected bytes() or str() instance, but got %s"),
		obj);
#endif
	തിരികെ ശൂന്യ;
    }

    /*
     * Error checking: String must not contain newlines, as we
     * are replacing a single line, and we must replace it with
     * a single line.
     * A trailing newline is removed, so that append(f.readlines()) works.
     */
    p = memchr(str, '\n', len);
    എങ്കിൽ (p != ശൂന്യ)
    {
	എങ്കിൽ (p == str + len - 1)
	    --len;
	അല്ലെങ്കിൽ
	{
	    PyErr_SET_VIM(N_("string cannot contain newlines"));
	    Py_XDECREF(bytes);
	    തിരികെ ശൂന്യ;
	}
    }

    /*
     * Create a copy of the string, with internal nulls replaced by
     * newline characters, as is the Vim convention.
     */
    save = alloc(len+1);
    എങ്കിൽ (save == ശൂന്യ)
    {
	PyErr_NoMemory();
	Py_XDECREF(bytes);
	തിരികെ ശൂന്യ;
    }

    വേണ്ടി (i = 0; i < len; ++i)
    {
	എങ്കിൽ (str[i] == '\0')
	    save[i] = '\n';
	അല്ലെങ്കിൽ
	    save[i] = str[i];
    }

    save[i] = '\0';
    Py_XDECREF(bytes);  // Python 2 does nothing here

    തിരികെ save;
}

/*
 * Get a line from the specified buffer. The line number is
 * in Vim format (1-based). The line is returned as a Python
 * string object.
 */
    സ്ഥിതം PyObject *
Get_Buffer_വരി(buf_T *buf, PyInt n)
{
    തിരികെ വരി_To_String((അക്ഷരം *)ml_get_buf(buf, (linenr_T)n, FALSE));
}


/*
 * Get a list of lines from the specified buffer. The line numbers
 * are in Vim format (1-based). The range is from lo up to, but not
 * including, hi. The list is returned as a Python list of string objects.
 */
    സ്ഥിതം PyObject *
Get_Buffer_വരി_List(buf_T *buf, PyInt lo, PyInt hi)
{
    PyInt	i;
    PyInt	n = hi - lo;
    PyObject	*list = PyList_New(n);

    എങ്കിൽ (list == ശൂന്യ)
	തിരികെ ശൂന്യ;

    വേണ്ടി (i = 0; i < n; ++i)
    {
	linenr_T	lnum = (linenr_T)(lo + i);
	അക്ഷരം		*text;
	PyObject	*string;

	എങ്കിൽ (lnum > buf->b_ml.ml_വരി_എണ്ണം)
	    text = "";
	അല്ലെങ്കിൽ
	    text = (അക്ഷരം *)ml_get_buf(buf, lnum, FALSE);
	string = വരി_To_String(text);
	എങ്കിൽ (string == ശൂന്യ)
	{
	    Py_DECREF(list);
	    തിരികെ ശൂന്യ;
	}

	PyList_SET_ITEM(list, i, string);
    }

    // The ownership of the Python list is passed to the caller (ie,
    // the caller should Py_DECREF() the object when it is finished
    // with it).

    തിരികെ list;
}

/*
 * Check if deleting lines made the cursor position invalid.
 * Changed the lines from "lo" to "hi" and added "extra" lines (negative if
 * deleted).
 */
    സ്ഥിതം ശൂന്യം
py_fix_cursor(linenr_T lo, linenr_T hi, linenr_T extra)
{
    എങ്കിൽ (curwin->w_cursor.lnum >= lo)
    {
	// Adjust the cursor position if it's in/after the changed
	// lines.
	എങ്കിൽ (curwin->w_cursor.lnum >= hi)
	{
	    curwin->w_cursor.lnum += extra;
	    check_cursor_col();
	}
	അല്ലെങ്കിൽ എങ്കിൽ (extra < 0)
	{
	    curwin->w_cursor.lnum = lo;
	    check_cursor();
	}
	അല്ലെങ്കിൽ
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
Set_Buffer_വരി(buf_T *buf, PyInt n, PyObject *വരി, PyInt *len_change)
{
    bufref_T	save_curbuf = {ശൂന്യ, 0, 0};
    switchwin_T	switchwin;

    // First of all, we check the type of the supplied Python object.
    // There are three cases:
    //	  1. NULL, or None - this is a deletion.
    //	  2. A string	   - this is a replacement.
    //	  3. Anything else - this is an error.
    എങ്കിൽ (വരി == Py_None || വരി == ശൂന്യ)
    {
	PyErr_Clear();
	switchwin.sw_curwin = ശൂന്യ;
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	VimTryStart();

	എങ്കിൽ (u_savedel((linenr_T)n, 1L) == FAIL)
	    RAISE_UNDO_FAIL;
	അല്ലെങ്കിൽ എങ്കിൽ (ml_delete((linenr_T)n) == FAIL)
	    RAISE_DELETE_വരി_FAIL;
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (buf == curbuf && (switchwin.sw_curwin != ശൂന്യ
					   || save_curbuf.br_buf == ശൂന്യ))
		// Using an existing window for the buffer, adjust the cursor
		// position.
		py_fix_cursor((linenr_T)n, (linenr_T)n + 1, (linenr_T)-1);
	    എങ്കിൽ (save_curbuf.br_buf == ശൂന്യ)
		// Only adjust marks if we managed to switch to a window that
		// holds the buffer, otherwise line numbers will be invalid.
		deleted_വരികൾ_mark((linenr_T)n, 1L);
	}

	restore_win_for_buf(&switchwin, &save_curbuf);

	എങ്കിൽ (VimTryEnd())
	    തിരികെ FAIL;

	എങ്കിൽ (len_change)
	    *len_change = -1;

	തിരികെ OK;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (PyBytes_Check(വരി) || PyUnicode_Check(വരി))
    {
	അക്ഷരം	*save = String_To_വരി(വരി);

	എങ്കിൽ (save == ശൂന്യ)
	    തിരികെ FAIL;

	VimTryStart();

	// We do not need to free "save" if ml_replace() consumes it.
	PyErr_Clear();
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	എങ്കിൽ (u_savesub((linenr_T)n) == FAIL)
	{
	    RAISE_UNDO_FAIL;
	    vim_free(save);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (ml_replace((linenr_T)n, (char_u *)save, FALSE) == FAIL)
	{
	    RAISE_REPLACE_വരി_FAIL;
	    vim_free(save);
	}
	അല്ലെങ്കിൽ
	    changed_bytes((linenr_T)n, 0);

	restore_win_for_buf(&switchwin, &save_curbuf);

	// Check that the cursor is not beyond the end of the line now.
	എങ്കിൽ (buf == curbuf)
	    check_cursor_col();

	എങ്കിൽ (VimTryEnd())
	    തിരികെ FAIL;

	എങ്കിൽ (len_change)
	    *len_change = 0;

	തിരികെ OK;
    }
    അല്ലെങ്കിൽ
    {
	PyErr_BadArgument();
	തിരികെ FAIL;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
Set_Buffer_വരി_List(
	buf_T *buf,
	PyInt lo,
	PyInt hi,
	PyObject *list,
	PyInt *len_change)
{
    bufref_T	save_curbuf = {ശൂന്യ, 0, 0};
    switchwin_T	switchwin;

    // First of all, we check the type of the supplied Python object.
    // There are three cases:
    //	  1. NULL, or None - this is a deletion.
    //	  2. A list	   - this is a replacement.
    //	  3. Anything else - this is an error.
    എങ്കിൽ (list == Py_None || list == ശൂന്യ)
    {
	PyInt	i;
	PyInt	n = (പൂർണ്ണസംഖ്യ)(hi - lo);

	PyErr_Clear();
	VimTryStart();
	switchwin.sw_curwin = ശൂന്യ;
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	എങ്കിൽ (u_savedel((linenr_T)lo, (long)n) == FAIL)
	    RAISE_UNDO_FAIL;
	അല്ലെങ്കിൽ
	{
	    വേണ്ടി (i = 0; i < n; ++i)
	    {
		എങ്കിൽ (ml_delete((linenr_T)lo) == FAIL)
		{
		    RAISE_DELETE_വരി_FAIL;
		    നിർത്തുക;
		}
	    }
	    എങ്കിൽ (buf == curbuf && (switchwin.sw_curwin != ശൂന്യ
					       || save_curbuf.br_buf == ശൂന്യ))
		// Using an existing window for the buffer, adjust the cursor
		// position.
		py_fix_cursor((linenr_T)lo, (linenr_T)hi, (linenr_T)-n);
	    എങ്കിൽ (save_curbuf.br_buf == ശൂന്യ)
		// Only adjust marks if we managed to switch to a window that
		// holds the buffer, otherwise line numbers will be invalid.
		deleted_വരികൾ_mark((linenr_T)lo, (long)i);
	}

	restore_win_for_buf(&switchwin, &save_curbuf);

	എങ്കിൽ (VimTryEnd())
	    തിരികെ FAIL;

	എങ്കിൽ (len_change)
	    *len_change = -n;

	തിരികെ OK;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (PyList_Check(list))
    {
	PyInt	i;
	PyInt	new_len = PyList_Size(list);
	PyInt	old_len = hi - lo;
	PyInt	extra = 0;	// lines added to text, can be negative
	അക്ഷരം	**array;

	എങ്കിൽ (new_len == 0)	// avoid allocating zero bytes
	    array = ശൂന്യ;
	അല്ലെങ്കിൽ
	{
	    array = PyMem_New(അക്ഷരം *, new_len);
	    എങ്കിൽ (array == ശൂന്യ)
	    {
		PyErr_NoMemory();
		തിരികെ FAIL;
	    }
	}

	വേണ്ടി (i = 0; i < new_len; ++i)
	{
	    PyObject *വരി;

	    എങ്കിൽ (!(വരി = PyList_GetItem(list, i)) ||
		!(array[i] = String_To_വരി(വരി)))
	    {
		ആകുമ്പോൾ (i)
		    vim_free(array[--i]);
		PyMem_Free(array);
		തിരികെ FAIL;
	    }
	}

	VimTryStart();
	PyErr_Clear();

	// START of region without "return".  Must call restore_buffer()!
	switchwin.sw_curwin = ശൂന്യ;
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	എങ്കിൽ (u_save((linenr_T)(lo-1), (linenr_T)hi) == FAIL)
	    RAISE_UNDO_FAIL;

	// If the size of the range is reducing (ie, new_len < old_len) we
	// need to delete some old_len. We do this at the start, by
	// repeatedly deleting line "lo".
	എങ്കിൽ (!PyErr_Occurred())
	{
	    വേണ്ടി (i = 0; i < old_len - new_len; ++i)
		എങ്കിൽ (ml_delete((linenr_T)lo) == FAIL)
		{
		    RAISE_DELETE_വരി_FAIL;
		    നിർത്തുക;
		}
	    extra -= i;
	}

	// For as long as possible, replace the existing old_len with the
	// new old_len. This is a more efficient operation, as it requires
	// less memory allocation and freeing.
	എങ്കിൽ (!PyErr_Occurred())
	{
	    വേണ്ടി (i = 0; i < old_len && i < new_len; ++i)
		എങ്കിൽ (ml_replace((linenr_T)(lo+i), (char_u *)array[i], FALSE)
								      == FAIL)
		{
		    RAISE_REPLACE_വരി_FAIL;
		    നിർത്തുക;
		}
	}
	അല്ലെങ്കിൽ
	    i = 0;

	// Now we may need to insert the remaining new old_len. If we do, we
	// must free the strings as we finish with them (we can't pass the
	// responsibility to Vim in this case).
	എങ്കിൽ (!PyErr_Occurred())
	{
	    ആകുമ്പോൾ (i < new_len)
	    {
		എങ്കിൽ (ml_append((linenr_T)(lo + i - 1),
					(char_u *)array[i], 0, FALSE) == FAIL)
		{
		    RAISE_തിരുകുക_വരി_FAIL;
		    നിർത്തുക;
		}
		vim_free(array[i]);
		++i;
		++extra;
	    }
	}

	// Free any left-over old_len, as a result of an error
	ആകുമ്പോൾ (i < new_len)
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
	എങ്കിൽ (save_curbuf.br_buf == ശൂന്യ)
	{
	    mark_adjust((linenr_T)lo, (linenr_T)(hi - 1),
						  (long)MAXLNUM, (long)extra);
	    changed_വരികൾ((linenr_T)lo, 0, (linenr_T)hi, (long)extra);
	}

	എങ്കിൽ (buf == curbuf && (switchwin.sw_curwin != ശൂന്യ
					   || save_curbuf.br_buf == ശൂന്യ))
	    // Using an existing window for the buffer, adjust the cursor
	    // position.
	    py_fix_cursor((linenr_T)lo, (linenr_T)hi, (linenr_T)extra);

	// END of region without "return".
	restore_win_for_buf(&switchwin, &save_curbuf);

	എങ്കിൽ (VimTryEnd())
	    തിരികെ FAIL;

	എങ്കിൽ (len_change)
	    *len_change = new_len - old_len;

	തിരികെ OK;
    }
    അല്ലെങ്കിൽ
    {
	PyErr_BadArgument();
	തിരികെ FAIL;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
തിരുകുക_Buffer_വരികൾ(buf_T *buf, PyInt n, PyObject *വരികൾ, PyInt *len_change)
{
    bufref_T	save_curbuf = {ശൂന്യ, 0, 0};
    switchwin_T	switchwin;

    // First of all, we check the type of the supplied Python object.
    // It must be a string or a list, or the call is in error.
    എങ്കിൽ (PyBytes_Check(വരികൾ) || PyUnicode_Check(വരികൾ))
    {
	അക്ഷരം		*str = String_To_വരി(വരികൾ);

	എങ്കിൽ (str == ശൂന്യ)
	    തിരികെ FAIL;

	PyErr_Clear();
	VimTryStart();
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	എങ്കിൽ (u_save((linenr_T)n, (linenr_T)(n + 1)) == FAIL)
	    RAISE_UNDO_FAIL;
	അല്ലെങ്കിൽ എങ്കിൽ (ml_append((linenr_T)n, (char_u *)str, 0, FALSE) == FAIL)
	    RAISE_തിരുകുക_വരി_FAIL;
	അല്ലെങ്കിൽ എങ്കിൽ (save_curbuf.br_buf == ശൂന്യ)
	    // Only adjust marks if we managed to switch to a window that
	    // holds the buffer, otherwise line numbers will be invalid.
	    appended_വരികൾ_mark((linenr_T)n, 1L);

	vim_free(str);
	restore_win_for_buf(&switchwin, &save_curbuf);
	update_screen(UPD_VALID);

	എങ്കിൽ (VimTryEnd())
	    തിരികെ FAIL;

	എങ്കിൽ (len_change)
	    *len_change = 1;

	തിരികെ OK;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (PyList_Check(വരികൾ))
    {
	PyInt	i;
	PyInt	size = PyList_Size(വരികൾ);
	അക്ഷരം	**array;

	array = PyMem_New(അക്ഷരം *, size);
	എങ്കിൽ (array == ശൂന്യ)
	{
	    PyErr_NoMemory();
	    തിരികെ FAIL;
	}

	വേണ്ടി (i = 0; i < size; ++i)
	{
	    PyObject *വരി;

	    എങ്കിൽ (!(വരി = PyList_GetItem(വരികൾ, i)) ||
		!(array[i] = String_To_വരി(വരി)))
	    {
		ആകുമ്പോൾ (i)
		    vim_free(array[--i]);
		PyMem_Free(array);
		തിരികെ FAIL;
	    }
	}

	PyErr_Clear();
	VimTryStart();
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	എങ്കിൽ (u_save((linenr_T)n, (linenr_T)(n + 1)) == FAIL)
	    RAISE_UNDO_FAIL;
	അല്ലെങ്കിൽ
	{
	    വേണ്ടി (i = 0; i < size; ++i)
	    {
		എങ്കിൽ (ml_append((linenr_T)(n + i),
					(char_u *)array[i], 0, FALSE) == FAIL)
		{
		    RAISE_തിരുകുക_വരി_FAIL;

		    // Free the rest of the lines
		    ആകുമ്പോൾ (i < size)
			vim_free(array[i++]);

		    നിർത്തുക;
		}
		vim_free(array[i]);
	    }
	    എങ്കിൽ (i > 0 && save_curbuf.br_buf == ശൂന്യ)
		// Only adjust marks if we managed to switch to a window that
		// holds the buffer, otherwise line numbers will be invalid.
		appended_വരികൾ_mark((linenr_T)n, (long)i);
	}

	// Free the array of lines. All of its contents have now
	// been freed.
	PyMem_Free(array);
	restore_win_for_buf(&switchwin, &save_curbuf);

	update_screen(UPD_VALID);

	എങ്കിൽ (VimTryEnd())
	    തിരികെ FAIL;

	എങ്കിൽ (len_change)
	    *len_change = size;

	തിരികെ OK;
    }
    അല്ലെങ്കിൽ
    {
	PyErr_BadArgument();
	തിരികെ FAIL;
    }
}

/*
 * Common routines for buffers and line ranges
 * -------------------------------------------
 */

തരനിർവചനം ഘടന
{
    PyObject_HEAD
    buf_T *buf;
} BufferObject;

    സ്ഥിതം പൂർണ്ണസംഖ്യ
CheckBuffer(BufferObject *self)
{
    എങ്കിൽ (self->buf == INVALID_BUFFER_മൂല്യം)
    {
	PyErr_SET_VIM(N_("attempt to refer to deleted buffer"));
	തിരികെ -1;
    }

    തിരികെ 0;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
CheckBufferCb(ശൂന്യം *self)
{
    തിരികെ CheckBuffer((BufferObject*)self);
}

    സ്ഥിതം PyObject *
RBItem(BufferObject *self, PyInt n, PyInt start, PyInt end)
{
    എങ്കിൽ (CheckBuffer(self))
	തിരികെ ശൂന്യ;

    എങ്കിൽ (end == -1)
	end = self->buf->b_ml.ml_വരി_എണ്ണം;

    എങ്കിൽ (n < 0)
	n += end - start + 1;

    എങ്കിൽ (n < 0 || n > end - start)
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("line number out of range"));
	തിരികെ ശൂന്യ;
    }

    തിരികെ Get_Buffer_വരി(self->buf, n+start);
}

    സ്ഥിതം PyObject *
RBSlice(BufferObject *self, PyInt lo, PyInt hi, PyInt start, PyInt end)
{
    PyInt size;

    എങ്കിൽ (CheckBuffer(self))
	തിരികെ ശൂന്യ;

    എങ്കിൽ (end == -1)
	end = self->buf->b_ml.ml_വരി_എണ്ണം;

    size = end - start + 1;

    എങ്കിൽ (lo < 0)
	lo = 0;
    അല്ലെങ്കിൽ എങ്കിൽ (lo > size)
	lo = size;
    എങ്കിൽ (hi < 0)
	hi = 0;
    എങ്കിൽ (hi < lo)
	hi = lo;
    അല്ലെങ്കിൽ എങ്കിൽ (hi > size)
	hi = size;

    തിരികെ Get_Buffer_വരി_List(self->buf, lo+start, hi+start);
}

    സ്ഥിതം PyInt
RBAsItem(
	BufferObject *self,
	PyInt n,
	PyObject *valObject,
	PyInt start,
	PyInt end,
	PyInt *new_end)
{
    PyInt len_change;

    എങ്കിൽ (CheckBuffer(self))
	തിരികെ -1;

    എങ്കിൽ (end == -1)
	end = self->buf->b_ml.ml_വരി_എണ്ണം;

    എങ്കിൽ (n < 0)
	n += end - start + 1;

    എങ്കിൽ (n < 0 || n > end - start)
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("line number out of range"));
	തിരികെ -1;
    }

    എങ്കിൽ (Set_Buffer_വരി(self->buf, n+start, valObject, &len_change) == FAIL)
	തിരികെ -1;

    എങ്കിൽ (new_end)
	*new_end = end + len_change;

    തിരികെ 0;
}

    സ്ഥിതം PyInt
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
    എങ്കിൽ (CheckBuffer(self))
	തിരികെ -1;

    എങ്കിൽ (end == -1)
	end = self->buf->b_ml.ml_വരി_എണ്ണം;

    // Sort out the slice range
    size = end - start + 1;

    എങ്കിൽ (lo < 0)
	lo = 0;
    അല്ലെങ്കിൽ എങ്കിൽ (lo > size)
	lo = size;
    എങ്കിൽ (hi < 0)
	hi = 0;
    എങ്കിൽ (hi < lo)
	hi = lo;
    അല്ലെങ്കിൽ എങ്കിൽ (hi > size)
	hi = size;

    എങ്കിൽ (Set_Buffer_വരി_List(self->buf, lo + start, hi + start,
						valObject, &len_change) == FAIL)
	തിരികെ -1;

    എങ്കിൽ (new_end)
	*new_end = end + len_change;

    തിരികെ 0;
}


    സ്ഥിതം PyObject *
RBAppend(
	BufferObject *self,
	PyObject *args,
	PyInt start,
	PyInt end,
	PyInt *new_end)
{
    PyObject *വരികൾ;
    PyInt len_change;
    PyInt max;
    PyInt n;

    എങ്കിൽ (CheckBuffer(self))
	തിരികെ ശൂന്യ;

    എങ്കിൽ (end == -1)
	end = self->buf->b_ml.ml_വരി_എണ്ണം;

    max = n = end - start + 1;

    എങ്കിൽ (!PyArg_ParseTuple(args, "O|n", &വരികൾ, &n))
	തിരികെ ശൂന്യ;

    എങ്കിൽ (n < 0 || n > max)
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("line number out of range"));
	തിരികെ ശൂന്യ;
    }

    എങ്കിൽ (തിരുകുക_Buffer_വരികൾ(self->buf, n + start - 1, വരികൾ, &len_change) == FAIL)
	തിരികെ ശൂന്യ;

    എങ്കിൽ (new_end)
	*new_end = end + len_change;

    Py_INCREF(Py_None);
    തിരികെ Py_None;
}

// Range object

DEFINE_PY_TYPE_OBJECT(RangeType);
സ്ഥിതം PySequenceMethods RangeAsSeq;
സ്ഥിതം PyMappingMethods RangeAsMapping;

തരനിർവചനം ഘടന
{
    PyObject_HEAD
    BufferObject *buf;
    PyInt start;
    PyInt end;
} RangeObject;

    സ്ഥിതം PyObject *
RangeNew(buf_T *buf, PyInt start, PyInt end)
{
    BufferObject *bufr;
    RangeObject *self;
    self = PyObject_GC_New(RangeObject, RangeTypePtr);
    എങ്കിൽ (self == ശൂന്യ)
	തിരികെ ശൂന്യ;

    bufr = (BufferObject *)BufferNew(buf);
    എങ്കിൽ (bufr == ശൂന്യ)
    {
	Py_DECREF(self);
	തിരികെ ശൂന്യ;
    }
    Py_INCREF((PyObject *)bufr);

    self->buf = bufr;
    self->start = start;
    self->end = end;

    തിരികെ (PyObject *)(self);
}

    സ്ഥിതം ശൂന്യം
RangeDestructor(PyObject *self_obj)
{
    RangeObject *self = (RangeObject*)self_obj;
    PyObject_GC_UnTrack((ശൂന്യം *)(self));
    Py_XDECREF(((PyObject *)(self->buf)));
    PyObject_GC_Del((ശൂന്യം *)(self));
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
RangeTraverse(PyObject *self_obj, visitproc visit, ശൂന്യം *arg)
{
    RangeObject *self = (RangeObject*)self_obj;
    Py_VISIT(((PyObject *)(self->buf)));
    തിരികെ 0;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
RangeClear(PyObject *self_obj)
{
    RangeObject *self = (RangeObject*)self_obj;
    Py_CLEAR(self->buf);
    തിരികെ 0;
}

    സ്ഥിതം PyInt
RangeLength(RangeObject *self)
{
    // HOW DO WE SIGNAL AN ERROR FROM THIS FUNCTION?
    എങ്കിൽ (CheckBuffer(self->buf))
	തിരികെ -1; // ???

    തിരികെ (self->end - self->start + 1);
}

    സ്ഥിതം PyObject *
RangeItem(RangeObject *self, PyInt n)
{
    തിരികെ RBItem(self->buf, n, self->start, self->end);
}

    സ്ഥിതം PyObject *
RangeSlice(RangeObject *self, PyInt lo, PyInt hi)
{
    തിരികെ RBSlice(self->buf, lo, hi, self->start, self->end);
}

സ്ഥിതം അക്ഷരം *RangeAttrs[] = {
    "start", "end",
    ശൂന്യ
};

    സ്ഥിതം PyObject *
RangeDir(PyObject *self, PyObject *args UNUSED)
{
    തിരികെ ObjectDir(self, RangeAttrs);
}

    സ്ഥിതം PyObject *
RangeAppend(RangeObject *self, PyObject *args)
{
    തിരികെ RBAppend(self->buf, args, self->start, self->end, &self->end);
}

    സ്ഥിതം PyObject *
RangeRepr(PyObject *self_obj)
{
    RangeObject *self = (RangeObject*)self_obj;
    എങ്കിൽ (self->buf->buf == INVALID_BUFFER_മൂല്യം)
	തിരികെ PyString_FromFormat("<range object (for deleted buffer) at %p>",
				    (ശൂന്യം *)self);
    അല്ലെങ്കിൽ
    {
	അക്ഷരം *name = (അക്ഷരം *)self->buf->buf->b_fname;

	എങ്കിൽ (name == ശൂന്യ)
	    name = "";

	തിരികെ PyString_FromFormat("<range %s (%d:%d)>",
				    name, (പൂർണ്ണസംഖ്യ)self->start, (പൂർണ്ണസംഖ്യ)self->end);
    }
}

സ്ഥിതം ഘടന PyMethodDef RangeMethods[] = {
    // name,	function,			calling,	documentation
    {"append",	(PyCFunction)RangeAppend,	METH_VARARGS,	"Append data to the Vim range" },
    {"__dir__",	(PyCFunction)RangeDir,		METH_NOARGS,	""},
    { ശൂന്യ,	ശൂന്യ,				0,		ശൂന്യ}
};

DEFINE_PY_TYPE_OBJECT(BufferType);
സ്ഥിതം PySequenceMethods BufferAsSeq;
സ്ഥിതം PyMappingMethods BufferAsMapping;

    സ്ഥിതം PyObject *
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

    എങ്കിൽ (BUF_PYTHON_REF(buf) != ശൂന്യ)
    {
	self = BUF_PYTHON_REF(buf);
	Py_INCREF((PyObject *)self);
    }
    അല്ലെങ്കിൽ
    {
	self = PyObject_NEW(BufferObject, BufferTypePtr);
	എങ്കിൽ (self == ശൂന്യ)
	    തിരികെ ശൂന്യ;
	self->buf = buf;
	BUF_PYTHON_REF(buf) = self;
    }

    തിരികെ (PyObject *)(self);
}

    സ്ഥിതം ശൂന്യം
BufferDestructor(PyObject *self_obj)
{
    BufferObject *self = (BufferObject*)self_obj;
    എങ്കിൽ (self->buf && self->buf != INVALID_BUFFER_മൂല്യം)
	BUF_PYTHON_REF(self->buf) = ശൂന്യ;

    DESTRUCTOR_FINISH(self);
}

    സ്ഥിതം PyInt
BufferLength(BufferObject *self)
{
    // HOW DO WE SIGNAL AN ERROR FROM THIS FUNCTION?
    എങ്കിൽ (CheckBuffer(self))
	തിരികെ -1; // ???

    തിരികെ (PyInt)(self->buf->b_ml.ml_വരി_എണ്ണം);
}

    സ്ഥിതം PyObject *
BufferItem(BufferObject *self, PyInt n)
{
    തിരികെ RBItem(self, n, 1, -1);
}

    സ്ഥിതം PyObject *
BufferSlice(BufferObject *self, PyInt lo, PyInt hi)
{
    തിരികെ RBSlice(self, lo, hi, 1, -1);
}

സ്ഥിതം അക്ഷരം *BufferAttrs[] = {
    "name", "number", "vars", "options", "valid",
    ശൂന്യ
};

    സ്ഥിതം PyObject *
BufferDir(PyObject *self, PyObject *args UNUSED)
{
    തിരികെ ObjectDir(self, BufferAttrs);
}

    സ്ഥിതം PyObject *
BufferAttrValid(BufferObject *self, അക്ഷരം *name)
{
    PyObject	*ret;

    എങ്കിൽ (strcmp(name, "valid") != 0)
	തിരികെ ശൂന്യ;

    ret = ((self->buf == INVALID_BUFFER_മൂല്യം) ? Py_False : Py_True);
    Py_INCREF(ret);
    തിരികെ ret;
}

    സ്ഥിതം PyObject *
BufferAttr(BufferObject *self, അക്ഷരം *name)
{
    എങ്കിൽ (strcmp(name, "name") == 0)
	തിരികെ PyString_FromString((self->buf->b_ffname == ശൂന്യ
				    ? "" : (അക്ഷരം *)self->buf->b_ffname));
    അല്ലെങ്കിൽ എങ്കിൽ (strcmp(name, "number") == 0)
	തിരികെ Py_Build_മൂല്യം(Py_ssize_t_fmt, self->buf->b_fnum);
    അല്ലെങ്കിൽ എങ്കിൽ (strcmp(name, "vars") == 0)
	തിരികെ NEW_DICTIONARY(self->buf->b_vars);
    അല്ലെങ്കിൽ എങ്കിൽ (strcmp(name, "options") == 0)
	തിരികെ OptionsNew(SREQ_BUF, self->buf, CheckBufferCb,
			(PyObject *) self);
    അല്ലെങ്കിൽ എങ്കിൽ (strcmp(name, "__members__") == 0)
	തിരികെ ObjectDir(ശൂന്യ, BufferAttrs);
    അല്ലെങ്കിൽ
	തിരികെ ശൂന്യ;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
BufferSetattr(PyObject *self_obj, അക്ഷരം *name, PyObject *valObject)
{
    BufferObject *self = (BufferObject*)self_obj;
    എങ്കിൽ (CheckBuffer(self))
	തിരികെ -1;

    എങ്കിൽ (strcmp(name, "name") == 0)
    {
	char_u		*val;
	aco_save_T	aco;
	പൂർണ്ണസംഖ്യ		ren_ret = OK;
	PyObject	*todecref;

	എങ്കിൽ (!(val = StringToChars(valObject, &todecref)))
	    തിരികെ -1;

	VimTryStart();
	// Using aucmd_*: autocommands will be executed by rename_buffer
	aucmd_prepbuf(&aco, self->buf);
	എങ്കിൽ (curbuf == self->buf)
	{
	    ren_ret = rename_buffer(val);
	    aucmd_restbuf(&aco);
	}
	Py_XDECREF(todecref);
	എങ്കിൽ (VimTryEnd())
	    തിരികെ -1;

	എങ്കിൽ (ren_ret == FAIL)
	{
	    PyErr_SET_VIM(N_("failed to rename buffer"));
	    തിരികെ -1;
	}
	തിരികെ 0;
    }
    അല്ലെങ്കിൽ
    {
	PyErr_SetString(PyExc_AttributeError, name);
	തിരികെ -1;
    }
}

    സ്ഥിതം PyObject *
BufferAppend(BufferObject *self, PyObject *args)
{
    തിരികെ RBAppend(self, args, 1, -1, ശൂന്യ);
}

    സ്ഥിതം PyObject *
BufferMark(BufferObject *self, PyObject *pmarkObject)
{
    pos_T	*posp;
    char_u	*pmark;
    char_u	mark;
    bufref_T	savebuf;
    PyObject	*todecref;

    എങ്കിൽ (CheckBuffer(self))
	തിരികെ ശൂന്യ;

    എങ്കിൽ (!(pmark = StringToChars(pmarkObject, &todecref)))
	തിരികെ ശൂന്യ;

    എങ്കിൽ (pmark[0] == '\0' || pmark[1] != '\0')
    {
	PyErr_SET_STRING(Py_Exc_മൂല്യം_Error,
		N_("mark name must be a single character"));
	Py_XDECREF(todecref);
	തിരികെ ശൂന്യ;
    }

    mark = *pmark;

    Py_XDECREF(todecref);

    VimTryStart();
    switch_buffer(&savebuf, self->buf);
    posp = getmark(mark, FALSE);
    restore_buffer(&savebuf);
    എങ്കിൽ (VimTryEnd())
	തിരികെ ശൂന്യ;

    എങ്കിൽ (posp == ശൂന്യ)
    {
	PyErr_SET_VIM(N_("invalid mark name"));
	തിരികെ ശൂന്യ;
    }

    എങ്കിൽ (posp->lnum <= 0)
    {
	// Or raise an error?
	Py_INCREF(Py_None);
	തിരികെ Py_None;
    }

    തിരികെ Py_Build_മൂല്യം("(ll)", (long)(posp->lnum), (long)(posp->col));
}

    സ്ഥിതം PyObject *
BufferRange(BufferObject *self, PyObject *args)
{
    PyInt start;
    PyInt end;

    എങ്കിൽ (CheckBuffer(self))
	തിരികെ ശൂന്യ;

    എങ്കിൽ (!PyArg_ParseTuple(args, "nn", &start, &end))
	തിരികെ ശൂന്യ;

    തിരികെ RangeNew(self->buf, start, end);
}

    സ്ഥിതം PyObject *
BufferRepr(PyObject *self_obj)
{
    BufferObject *self = (BufferObject*)self_obj;
    എങ്കിൽ (self->buf == INVALID_BUFFER_മൂല്യം)
	തിരികെ PyString_FromFormat("<buffer object (deleted) at %p>", (ശൂന്യം *)self);
    അല്ലെങ്കിൽ
    {
	അക്ഷരം	*name = (അക്ഷരം *)self->buf->b_fname;

	എങ്കിൽ (name == ശൂന്യ)
	    name = "";

	തിരികെ PyString_FromFormat("<buffer %s>", name);
    }
}

സ്ഥിതം ഘടന PyMethodDef BufferMethods[] = {
    // name,	    function,			calling,	documentation
    {"append",	    (PyCFunction)BufferAppend,	METH_VARARGS,	"Append data to Vim buffer" },
    {"mark",	    (PyCFunction)BufferMark,	METH_O,		"Return (row,col) representing position of named mark" },
    {"range",	    (PyCFunction)BufferRange,	METH_VARARGS,	"Return a range object which represents the part of the given buffer between line numbers s and e" },
    {"__dir__",	    (PyCFunction)BufferDir,	METH_NOARGS,	""},
    { ശൂന്യ,	    ശൂന്യ,			0,		ശൂന്യ}
};

/*
 * Buffer list object - Implementation
 */

DEFINE_PY_TYPE_OBJECT(BufMapType);

തരനിർവചനം ഘടന
{
    PyObject_HEAD
} BufMapObject;

സ്ഥിതം BufMapObject TheBufferMap =
{
    PyObject_HEAD_INIT_TYPE(BufMapType)
};

    സ്ഥിതം PyInt
BufMapLength(PyObject *self UNUSED)
{
    buf_T	*b = firstbuf;
    PyInt	n = 0;

    ആകുമ്പോൾ (b)
    {
	++n;
	b = b->b_next;
    }

    തിരികെ n;
}

    സ്ഥിതം PyObject *
BufMapItem(PyObject *self UNUSED, PyObject *താക്കോൽ_Object)
{
    buf_T	*b;
    long	bnr;

    എങ്കിൽ (സംഖ്യ_To_Long(താക്കോൽ_Object, &bnr, സംഖ്യ_INT|സംഖ്യ_NATURAL))
	തിരികെ ശൂന്യ;

    b = buflist_findnr((പൂർണ്ണസംഖ്യ) bnr);

    എങ്കിൽ (b)
	തിരികെ BufferNew(b);
    അല്ലെങ്കിൽ
    {
	PyErr_SetObject(Py_Exc_താക്കോൽ_Error, താക്കോൽ_Object);
	തിരികെ ശൂന്യ;
    }
}

    സ്ഥിതം ശൂന്യം
BufMapIterDestruct(ശൂന്യം* arg)
{
    PyObject *buffer = (PyObject*)arg;
    // Iteration was stopped before all buffers were processed
    എങ്കിൽ (buffer)
    {
	Py_DECREF(buffer);
    }
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
BufMapIterTraverse(ശൂന്യം *iter, visitproc visit, ശൂന്യം *arg)
{
    PyObject *buffer = (PyObject*)iter;
    എങ്കിൽ (buffer)
	Py_VISIT(buffer);
    തിരികെ 0;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
BufMapIterClear(ശൂന്യം **iter)
{
    PyObject **buffer = (PyObject**)iter;
    എങ്കിൽ (*buffer)
	Py_CLEAR(*buffer);
    തിരികെ 0;
}

    സ്ഥിതം PyObject *
BufMapIterNext(ശൂന്യം **arg)
{
    PyObject	*next;
    PyObject	*ret;
    PyObject	**buffer = (PyObject**)arg;

    എങ്കിൽ (!*buffer)
	തിരികെ ശൂന്യ;

    ret = *buffer;

    എങ്കിൽ (CheckBuffer((BufferObject *)(ret)))
    {
	*buffer = ശൂന്യ;
	തിരികെ ശൂന്യ;
    }

    എങ്കിൽ (!((BufferObject *)(ret))->buf->b_next)
	next = ശൂന്യ;
    അല്ലെങ്കിൽ എങ്കിൽ (!(next = BufferNew(((BufferObject *)(ret))->buf->b_next)))
	തിരികെ ശൂന്യ;
    *buffer = next;
    // Do not increment reference: we no longer hold it (decref), but whoever
    // on other side will hold (incref). Decref+incref = nothing.
    തിരികെ ret;
}

    സ്ഥിതം PyObject *
BufMapIter(PyObject *self)
{
    PyObject *buffer;

    buffer = BufferNew(firstbuf);
    തിരികെ IterNew(buffer,
	    BufMapIterDestruct, BufMapIterNext,
	    BufMapIterTraverse, BufMapIterClear,
	    self);
}

സ്ഥിതം PyMappingMethods BufMapAsMapping = {
    (lenfunc)       BufMapLength,
    (binaryfunc)    BufMapItem,
    (objobjargproc) 0,
};

// Current items object

സ്ഥിതം അക്ഷരം *CurrentAttrs[] = {
    "buffer", "window", "line", "range", "tabpage",
    ശൂന്യ
};

    സ്ഥിതം PyObject *
CurrentDir(PyObject *self, PyObject *args UNUSED)
{
    തിരികെ ObjectDir(self, CurrentAttrs);
}

    സ്ഥിതം PyObject *
CurrentGetattr(PyObject *self UNUSED, അക്ഷരം *name)
{
    എങ്കിൽ (strcmp(name, "buffer") == 0)
	തിരികെ (PyObject *)BufferNew(curbuf);
    അല്ലെങ്കിൽ എങ്കിൽ (strcmp(name, "window") == 0)
	തിരികെ (PyObject *)WindowNew(curwin, curtab);
    അല്ലെങ്കിൽ എങ്കിൽ (strcmp(name, "tabpage") == 0)
	തിരികെ (PyObject *)TabPageNew(curtab);
    അല്ലെങ്കിൽ എങ്കിൽ (strcmp(name, "line") == 0)
	തിരികെ Get_Buffer_വരി(curbuf, (PyInt)curwin->w_cursor.lnum);
    അല്ലെങ്കിൽ എങ്കിൽ (strcmp(name, "range") == 0)
	തിരികെ RangeNew(curbuf, RangeStart, RangeEnd);
    അല്ലെങ്കിൽ എങ്കിൽ (strcmp(name, "__members__") == 0)
	തിരികെ ObjectDir(ശൂന്യ, CurrentAttrs);
    അല്ലെങ്കിൽ
#എങ്കിൽ PY_MAJOR_VERSION < 3
	തിരികെ Py_FindMethod(WindowMethods, self, name);
#അല്ലെങ്കിൽ
	തിരികെ ശൂന്യ;
#endif
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
CurrentSetattr(PyObject *self UNUSED, അക്ഷരം *name, PyObject *valObject)
{
    എങ്കിൽ (strcmp(name, "line") == 0)
    {
	എങ്കിൽ (Set_Buffer_വരി(curbuf, (PyInt)curwin->w_cursor.lnum, valObject,
			  ശൂന്യ) == FAIL)
	    തിരികെ -1;

	തിരികെ 0;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (strcmp(name, "buffer") == 0)
    {
	പൂർണ്ണസംഖ്യ എണ്ണം;

	എങ്കിൽ (valObject->ob_type != BufferTypePtr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.Buffer object, but got %s"),
		    valObject);
	    തിരികെ -1;
	}

	എങ്കിൽ (CheckBuffer((BufferObject *)(valObject)))
	    തിരികെ -1;
	എണ്ണം = ((BufferObject *)(valObject))->buf->b_fnum;

	VimTryStart();
	എങ്കിൽ (do_buffer(DOBUF_GOTO, DOBUF_FIRST, FORWARD, എണ്ണം, 0) == FAIL)
	{
	    എങ്കിൽ (VimTryEnd())
		തിരികെ -1;
	    PyErr_VIM_FORMAT(N_("failed to switch to buffer %d"), എണ്ണം);
	    തിരികെ -1;
	}

	തിരികെ VimTryEnd();
    }
    അല്ലെങ്കിൽ എങ്കിൽ (strcmp(name, "window") == 0)
    {
	പൂർണ്ണസംഖ്യ എണ്ണം;

	എങ്കിൽ (valObject->ob_type != WindowTypePtr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.Window object, but got %s"),
		    valObject);
	    തിരികെ -1;
	}

	എങ്കിൽ (CheckWindow((WindowObject *)(valObject)))
	    തിരികെ -1;
	എണ്ണം = get_win_സംഖ്യ(((WindowObject *)(valObject))->win, firstwin);

	എങ്കിൽ (!എണ്ണം)
	{
	    PyErr_SET_STRING(Py_Exc_മൂല്യം_Error,
		    N_("failed to find window in the current tab page"));
	    തിരികെ -1;
	}

	VimTryStart();
	win_goto(((WindowObject *)(valObject))->win);
	എങ്കിൽ (((WindowObject *)(valObject))->win != curwin)
	{
	    എങ്കിൽ (VimTryEnd())
		തിരികെ -1;
	    PyErr_SET_STRING(PyExc_RuntimeError,
		    N_("did not switch to the specified window"));
	    തിരികെ -1;
	}

	തിരികെ VimTryEnd();
    }
    അല്ലെങ്കിൽ എങ്കിൽ (strcmp(name, "tabpage") == 0)
    {
	എങ്കിൽ (valObject->ob_type != TabPageTypePtr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.TabPage object, but got %s"),
		    valObject);
	    തിരികെ -1;
	}

	എങ്കിൽ (CheckTabPage((TabPageObject *)(valObject)))
	    തിരികെ -1;

	VimTryStart();
	goto_tabpage_tp(((TabPageObject *)(valObject))->tab, TRUE, TRUE);
	എങ്കിൽ (((TabPageObject *)(valObject))->tab != curtab)
	{
	    എങ്കിൽ (VimTryEnd())
		തിരികെ -1;
	    PyErr_SET_STRING(PyExc_RuntimeError,
		    N_("did not switch to the specified tab page"));
	    തിരികെ -1;
	}

	തിരികെ VimTryEnd();
    }
    അല്ലെങ്കിൽ
    {
	PyErr_SetString(PyExc_AttributeError, name);
	തിരികെ -1;
    }
}

സ്ഥിതം ഘടന PyMethodDef CurrentMethods[] = {
    // name,	    function,			calling,	documentation
    {"__dir__",	    (PyCFunction)CurrentDir,	METH_NOARGS,	""},
    { ശൂന്യ,	    ശൂന്യ,			0,		ശൂന്യ}
};

    സ്ഥിതം ശൂന്യം
init_range_cmd(ശൂന്യം *arg)
{
    exarg_T *eap = (exarg_T*)arg;
    RangeStart = eap->line1;
    RangeEnd = eap->line2;
}

    സ്ഥിതം ശൂന്യം
init_range_eval(ശൂന്യം *rettv UNUSED)
{
    RangeStart = (PyInt) curwin->w_cursor.lnum;
    RangeEnd = RangeStart;
}

    സ്ഥിതം ശൂന്യം
run_cmd(സ്ഥിരം അക്ഷരം *cmd, dict_T* locals UNUSED, ശൂന്യം *arg UNUSED
#ifdef PY_CAN_RECURSE
	, PyGILState_STATE *pygilstate UNUSED
#endif
	)
{
    PyObject	*run_ret;
    run_ret = PyRun_String((അക്ഷരം *)cmd, Py_file_നിവേശം, globals, globals);
    എങ്കിൽ (run_ret != ശൂന്യ)
    {
	Py_DECREF(run_ret);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
    {
	emsg(_(e_cant_handle_systemexit_of_python_exception_in_vim));
	PyErr_Clear();
    }
    അല്ലെങ്കിൽ
	PyErr_PrintEx(1);
}

സ്ഥിതം സ്ഥിരം അക്ഷരം	*code_hdr = "def " DOPY_FUNC "(line, linenr):\n ";
സ്ഥിതം പൂർണ്ണസംഖ്യ		code_hdr_len = 30;

    സ്ഥിതം ശൂന്യം
run_do(സ്ഥിരം അക്ഷരം *cmd, dict_T* locals UNUSED, ശൂന്യം *arg UNUSED
#ifdef PY_CAN_RECURSE
	, PyGILState_STATE *pygilstate
#endif
	)
{
    PyInt	lnum;
    size_t	len;
    അക്ഷരം	*code;
    പൂർണ്ണസംഖ്യ		status;
    PyObject	*pyfunc, *pymain;
    PyObject	*run_ret;
    buf_T	*was_curbuf = curbuf;

    എങ്കിൽ (u_save((linenr_T)RangeStart - 1, (linenr_T)RangeEnd + 1) != OK)
    {
	emsg(_("cannot save undo information"));
	തിരികെ;
    }

    len = code_hdr_len + STRLEN(cmd);
    code = PyMem_New(അക്ഷരം, len + 1);
    memcpy(code, code_hdr, code_hdr_len);
    STRCPY(code + code_hdr_len, cmd);
    run_ret = PyRun_String(code, Py_file_നിവേശം, globals, globals);
    status = -1;
    എങ്കിൽ (run_ret != ശൂന്യ)
    {
	status = 0;
	Py_DECREF(run_ret);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
    {
	PyMem_Free(code);
	emsg(_(e_cant_handle_systemexit_of_python_exception_in_vim));
	PyErr_Clear();
	തിരികെ;
    }
    അല്ലെങ്കിൽ
	PyErr_PrintEx(1);

    PyMem_Free(code);

    എങ്കിൽ (status)
    {
	emsg(_("failed to run the code"));
	തിരികെ;
    }

    status = 0;
    pymain = PyImport_AddModule("__main__");
    pyfunc = PyObject_GetAttrString(pymain, DOPY_FUNC);
#ifdef PY_CAN_RECURSE
    PyGILState_Release(*pygilstate);
#endif

    വേണ്ടി (lnum = RangeStart; lnum <= RangeEnd; ++lnum)
    {
	PyObject	*വരി;
	PyObject	*linenr;
	PyObject	*ret;

#ifdef PY_CAN_RECURSE
	*pygilstate = PyGILState_Ensure();
#endif
	// Check the line number, the command my have deleted lines.
	എങ്കിൽ (lnum > curbuf->b_ml.ml_വരി_എണ്ണം
		|| !(വരി = Get_Buffer_വരി(curbuf, lnum)))
	    പോകുക err;
	എങ്കിൽ (!(linenr = PyInt_FromLong((long) lnum)))
	{
	    Py_DECREF(വരി);
	    പോകുക err;
	}
	ret = PyObject_CallFunctionObjArgs(pyfunc, വരി, linenr, ശൂന്യ);
	Py_DECREF(വരി);
	Py_DECREF(linenr);
	എങ്കിൽ (!ret)
	    പോകുക err;

	// Check that the command didn't switch to another buffer.
	// Check the line number, the command my have deleted lines.
	എങ്കിൽ (curbuf != was_curbuf || lnum > curbuf->b_ml.ml_വരി_എണ്ണം)
	{
	    Py_XDECREF(ret);
	    പോകുക err;
	}

	എങ്കിൽ (ret != Py_None)
	    എങ്കിൽ (Set_Buffer_വരി(curbuf, lnum, ret, ശൂന്യ) == FAIL)
	    {
		Py_XDECREF(ret);
		പോകുക err;
	    }

	Py_XDECREF(ret);
	PythonIO_Flush();
#ifdef PY_CAN_RECURSE
	PyGILState_Release(*pygilstate);
#endif
    }
    പോകുക out;
err:
#ifdef PY_CAN_RECURSE
    *pygilstate = PyGILState_Ensure();
#endif
    PyErr_PrintEx(0);
    PythonIO_Flush();
    status = 1;
out:
#ifdef PY_CAN_RECURSE
    എങ്കിൽ (!status)
	*pygilstate = PyGILState_Ensure();
#endif
    Py_DECREF(pyfunc);
    PyObject_SetAttrString(pymain, DOPY_FUNC, ശൂന്യ);
    എങ്കിൽ (status)
	തിരികെ;
    check_cursor();
    update_curbuf(UPD_NOT_VALID);
}

    സ്ഥിതം ശൂന്യം
run_eval(സ്ഥിരം അക്ഷരം *cmd, dict_T *locals, ശൂന്യം *arg
#ifdef PY_CAN_RECURSE
	, PyGILState_STATE *pygilstate UNUSED
#endif
	)
{
    PyObject	*run_ret;
    typval_T	*rettv = (typval_T*)arg;
    PyObject	*pylocals = locals ? NEW_DICTIONARY(locals) : globals;

    run_ret = PyRun_String((അക്ഷരം *)cmd, Py_eval_നിവേശം, globals, pylocals);
    എങ്കിൽ (run_ret == ശൂന്യ)
    {
	എങ്കിൽ (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
	{
	    emsg(_(e_cant_handle_systemexit_of_python_exception_in_vim));
	    PyErr_Clear();
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (PyErr_Occurred() && !സന്ദേശം_silent)
		PyErr_PrintEx(0);
	    emsg(_(e_eval_did_not_return_valid_python_object));
	}
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (ConvertFromPyObject(run_ret, rettv) == -1)
	    emsg(_(e_failed_to_convert_returned_python_object_to_vim_മൂല്യം));
	Py_DECREF(run_ret);
    }
    PyErr_Clear();
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
set_ref_in_py(സ്ഥിരം പൂർണ്ണസംഖ്യ copyID)
{
    pylinkedlist_T	*cur;
    list_T		*ll;
    tuple_T		*tt;
    പൂർണ്ണസംഖ്യ			i;
    പൂർണ്ണസംഖ്യ			abort = FALSE;
    FunctionObject	*func;

    എങ്കിൽ (lastdict != ശൂന്യ)
    {
	വേണ്ടി (cur = lastdict ; !abort && cur != ശൂന്യ ; cur = cur->pll_prev)
	    abort = set_ref_in_dict(((DictionaryObject *)(cur->pll_obj))->dict,
								       copyID);
    }

    എങ്കിൽ (lastlist != ശൂന്യ)
    {
	വേണ്ടി (cur = lastlist ; !abort && cur != ശൂന്യ ; cur = cur->pll_prev)
	{
	    ll = ((ListObject *) (cur->pll_obj))->list;
	    abort = set_ref_in_list(ll, copyID);
	}
    }

    എങ്കിൽ (lasttuple != ശൂന്യ)
    {
	വേണ്ടി (cur = lasttuple ; !abort && cur != ശൂന്യ ; cur = cur->pll_prev)
	{
	    tt = ((TupleObject *) (cur->pll_obj))->tuple;
	    abort = set_ref_in_tuple(tt, copyID);
	}
    }

    എങ്കിൽ (lastfunc != ശൂന്യ)
    {
	വേണ്ടി (cur = lastfunc ; !abort && cur != ശൂന്യ ; cur = cur->pll_prev)
	{
	    func = (FunctionObject *) cur->pll_obj;
	    abort = set_ref_in_dict(func->self, copyID);
	    എങ്കിൽ (func->argc)
		വേണ്ടി (i = 0; !abort && i < func->argc; ++i)
		    abort = abort
			|| set_ref_in_item(&func->argv[i], copyID, ശൂന്യ, ശൂന്യ, ശൂന്യ);
	}
    }

    തിരികെ abort;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
set_string_copy(char_u *str, typval_T *tv)
{
    tv->vval.v_string = vim_strsave(str);
    എങ്കിൽ (tv->vval.v_string == ശൂന്യ)
    {
	PyErr_NoMemory();
	തിരികെ -1;
    }
    തിരികെ 0;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
pydict_to_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    dict_T	*dict;
    char_u	*താക്കോൽ;
    dictitem_T	*di;
    PyObject	*താക്കോൽ_Object;
    PyObject	*valObject;
    Py_ssize_t	iter = 0;

    എങ്കിൽ (!(dict = py_dict_alloc()))
	തിരികെ -1;

    tv->v_type = VAR_DICT;
    tv->vval.v_dict = dict;

    ആകുമ്പോൾ (PyDict_Next(obj, &iter, &താക്കോൽ_Object, &valObject))
    {
	PyObject	*todecref = ശൂന്യ;

	എങ്കിൽ (താക്കോൽ_Object == ശൂന്യ || valObject == ശൂന്യ)
	{
	    dict_unref(dict);
	    തിരികെ -1;
	}

	എങ്കിൽ (!(താക്കോൽ = StringToChars(താക്കോൽ_Object, &todecref)))
	{
	    dict_unref(dict);
	    തിരികെ -1;
	}

	എങ്കിൽ (*താക്കോൽ == NUL)
	{
	    dict_unref(dict);
	    Py_XDECREF(todecref);
	    RAISE_NO_EMPTY_താക്കോലുകൾ;
	    തിരികെ -1;
	}

	di = dictitem_alloc(താക്കോൽ);

	Py_XDECREF(todecref);

	എങ്കിൽ (di == ശൂന്യ)
	{
	    PyErr_NoMemory();
	    dict_unref(dict);
	    തിരികെ -1;
	}

	എങ്കിൽ (_ConvertFromPyObject(valObject, &di->di_tv, lookup_dict) == -1)
	{
	    vim_free(di);
	    dict_unref(dict);
	    തിരികെ -1;
	}

	എങ്കിൽ (dict_add(dict, di) == FAIL)
	{
	    RAISE_താക്കോൽ_ADD_FAIL(di->di_താക്കോൽ);
	    clear_tv(&di->di_tv);
	    vim_free(di);
	    dict_unref(dict);
	    തിരികെ -1;
	}
    }

    --dict->dv_refcount;
    തിരികെ 0;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
pymap_to_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    dict_T	*dict;
    char_u	*താക്കോൽ;
    dictitem_T	*di;
    PyObject	*list;
    PyObject	*iterator;
    PyObject	*താക്കോൽ_Object;
    PyObject	*valObject;

    എങ്കിൽ (!(dict = py_dict_alloc()))
	തിരികെ -1;

    tv->v_type = VAR_DICT;
    tv->vval.v_dict = dict;

    എങ്കിൽ (!(list = Py_Mapping_താക്കോലുകൾ(obj)))
    {
	dict_unref(dict);
	തിരികെ -1;
    }

    എങ്കിൽ (!(iterator = PyObject_GetIter(list)))
    {
	dict_unref(dict);
	Py_DECREF(list);
	തിരികെ -1;
    }
    Py_DECREF(list);

    ആകുമ്പോൾ ((താക്കോൽ_Object = PyIter_Next(iterator)))
    {
	PyObject	*todecref;

	എങ്കിൽ (!(താക്കോൽ = StringToChars(താക്കോൽ_Object, &todecref)))
	{
	    Py_DECREF(താക്കോൽ_Object);
	    Py_DECREF(iterator);
	    dict_unref(dict);
	    തിരികെ -1;
	}

	എങ്കിൽ (*താക്കോൽ == NUL)
	{
	    Py_DECREF(താക്കോൽ_Object);
	    Py_DECREF(iterator);
	    Py_XDECREF(todecref);
	    dict_unref(dict);
	    RAISE_NO_EMPTY_താക്കോലുകൾ;
	    തിരികെ -1;
	}

	എങ്കിൽ (!(valObject = PyObject_GetItem(obj, താക്കോൽ_Object)))
	{
	    Py_DECREF(താക്കോൽ_Object);
	    Py_DECREF(iterator);
	    Py_XDECREF(todecref);
	    dict_unref(dict);
	    തിരികെ -1;
	}

	di = dictitem_alloc(താക്കോൽ);

	Py_DECREF(താക്കോൽ_Object);
	Py_XDECREF(todecref);

	എങ്കിൽ (di == ശൂന്യ)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(valObject);
	    dict_unref(dict);
	    PyErr_NoMemory();
	    തിരികെ -1;
	}

	എങ്കിൽ (_ConvertFromPyObject(valObject, &di->di_tv, lookup_dict) == -1)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(valObject);
	    vim_free(di);
	    dict_unref(dict);
	    തിരികെ -1;
	}

	Py_DECREF(valObject);

	എങ്കിൽ (dict_add(dict, di) == FAIL)
	{
	    RAISE_താക്കോൽ_ADD_FAIL(di->di_താക്കോൽ);
	    Py_DECREF(iterator);
	    dictitem_free(di);
	    dict_unref(dict);
	    തിരികെ -1;
	}
    }
    Py_DECREF(iterator);
    --dict->dv_refcount;
    തിരികെ 0;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
pyseq_to_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    list_T	*l;

    എങ്കിൽ (!(l = py_list_alloc()))
	തിരികെ -1;

    tv->v_type = VAR_LIST;
    tv->vval.v_list = l;

    എങ്കിൽ (list_py_concat(l, obj, lookup_dict) == -1)
    {
	list_unref(l);
	തിരികെ -1;
    }

    --l->lv_refcount;
    തിരികെ 0;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
pytuple_to_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    tuple_T	*t;

    എങ്കിൽ (!(t = py_tuple_alloc()))
	തിരികെ -1;

    tv->v_type = VAR_TUPLE;
    tv->vval.v_tuple = t;

    എങ്കിൽ (tuple_py_concat(t, obj, lookup_dict) == -1)
    {
	tuple_unref(t);
	തിരികെ -1;
    }

    --t->tv_refcount;
    തിരികെ 0;
}

തരനിർവചനം പൂർണ്ണസംഖ്യ (*pytotvfunc)(PyObject *, typval_T *, PyObject *);

    സ്ഥിതം പൂർണ്ണസംഖ്യ
convert_dl(PyObject *obj, typval_T *tv,
				    pytotvfunc py_to_tv, PyObject *lookup_dict)
{
    PyObject	*capsule;
    അക്ഷരം	hexBuf[വലുപ്പം(ശൂന്യം *) * 2 + 3];

    sprintf(hexBuf, "%p", (ശൂന്യം *)obj);

#ifdef PY_USE_CAPSULE
    capsule = PyDict_GetItemString(lookup_dict, hexBuf);
#അല്ലെങ്കിൽ
    capsule = (PyObject *)PyDict_GetItemString(lookup_dict, hexBuf);
#endif
    എങ്കിൽ (capsule == ശൂന്യ)
    {
#ifdef PY_USE_CAPSULE
	capsule = PyCapsule_New(tv, ശൂന്യ, ശൂന്യ);
#അല്ലെങ്കിൽ
	capsule = PyCObject_FromVoidPtr(tv, ശൂന്യ);
#endif
	എങ്കിൽ (PyDict_SetItemString(lookup_dict, hexBuf, capsule))
	{
	    Py_DECREF(capsule);
	    tv->v_type = VAR_UNKNOWN;
	    തിരികെ -1;
	}

	Py_DECREF(capsule);

	എങ്കിൽ (py_to_tv(obj, tv, lookup_dict) == -1)
	{
	    tv->v_type = VAR_UNKNOWN;
	    തിരികെ -1;
	}
	// As we are not using copy_tv which increments reference count we must
	// do it ourself.
	എങ്കിൽ (tv->v_type == VAR_DICT)
	    ++tv->vval.v_dict->dv_refcount;
	അല്ലെങ്കിൽ എങ്കിൽ (tv->v_type == VAR_LIST)
	    ++tv->vval.v_list->lv_refcount;
	അല്ലെങ്കിൽ എങ്കിൽ (tv->v_type == VAR_TUPLE)
	    ++tv->vval.v_tuple->tv_refcount;
    }
    അല്ലെങ്കിൽ
    {
	typval_T	*v;

#ifdef PY_USE_CAPSULE
	v = PyCapsule_GetPointer(capsule, ശൂന്യ);
#അല്ലെങ്കിൽ
	v = PyCObject_AsVoidPtr(capsule);
#endif
	copy_tv(v, tv);
    }
    തിരികെ 0;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
ConvertFromPyMapping(PyObject *obj, typval_T *tv)
{
    PyObject	*lookup_dict;
    പൂർണ്ണസംഖ്യ		ret;

    എങ്കിൽ (!(lookup_dict = PyDict_New()))
	തിരികെ -1;

    എങ്കിൽ (PyType_IsSubtype(obj->ob_type, DictionaryTypePtr))
    {
	tv->v_type = VAR_DICT;
	tv->vval.v_dict = (((DictionaryObject *)(obj))->dict);
	++tv->vval.v_dict->dv_refcount;
	ret = 0;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (PyDict_Check(obj))
	ret = convert_dl(obj, tv, pydict_to_tv, lookup_dict);
    അല്ലെങ്കിൽ എങ്കിൽ (PyMapping_Check(obj))
	ret = convert_dl(obj, tv, pymap_to_tv, lookup_dict);
    അല്ലെങ്കിൽ
    {
	PyErr_FORMAT_TYPE(
		N_("unable to convert %s to a Vim dictionary"),
		obj);
	ret = -1;
    }
    Py_DECREF(lookup_dict);
    തിരികെ ret;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
ConvertFromPySequence(PyObject *obj, typval_T *tv)
{
    PyObject	*lookup_dict;
    പൂർണ്ണസംഖ്യ		ret;

    എങ്കിൽ (!(lookup_dict = PyDict_New()))
	തിരികെ -1;

    എങ്കിൽ (PyType_IsSubtype(obj->ob_type, ListTypePtr))
    {
	tv->v_type = VAR_LIST;
	tv->vval.v_list = (((ListObject *)(obj))->list);
	++tv->vval.v_list->lv_refcount;
	ret = 0;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (PyIter_Check(obj) || PySequence_Check(obj))
	ret = convert_dl(obj, tv, pyseq_to_tv, lookup_dict);
    അല്ലെങ്കിൽ
    {
	PyErr_FORMAT_TYPE(
		N_("unable to convert %s to a Vim list"),
		obj);
	ret = -1;
    }
    Py_DECREF(lookup_dict);
    തിരികെ ret;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
ConvertFromPyObject(PyObject *obj, typval_T *tv)
{
    PyObject	*lookup_dict;
    പൂർണ്ണസംഖ്യ		ret;

    എങ്കിൽ (!(lookup_dict = PyDict_New()))
	തിരികെ -1;
    ret = _ConvertFromPyObject(obj, tv, lookup_dict);
    Py_DECREF(lookup_dict);
    തിരികെ ret;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
_ConvertFromPyObject(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    എങ്കിൽ (PyType_IsSubtype(obj->ob_type, DictionaryTypePtr))
    {
	tv->v_type = VAR_DICT;
	tv->vval.v_dict = (((DictionaryObject *)(obj))->dict);
	++tv->vval.v_dict->dv_refcount;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (PyType_IsSubtype(obj->ob_type, ListTypePtr))
    {
	tv->v_type = VAR_LIST;
	tv->vval.v_list = (((ListObject *)(obj))->list);
	++tv->vval.v_list->lv_refcount;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (PyType_IsSubtype(obj->ob_type, TupleTypePtr))
    {
	tv->v_type = VAR_TUPLE;
	tv->vval.v_tuple = (((TupleObject *)(obj))->tuple);
	++tv->vval.v_tuple->tv_refcount;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (PyType_IsSubtype(obj->ob_type, FunctionTypePtr))
    {
	FunctionObject *func = (FunctionObject *) obj;
	എങ്കിൽ (func->self != ശൂന്യ || func->argv != ശൂന്യ)
	{
	    partial_T *pt = ALLOC_CLEAR_ONE(partial_T);

	    set_partial(func, pt, TRUE);
	    tv->vval.v_partial = pt;
	    tv->v_type = VAR_PARTIAL;
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (set_string_copy(func->name, tv) == -1)
		തിരികെ -1;

	    tv->v_type = VAR_FUNC;
	}
	func_ref(func->name);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (PyBytes_Check(obj))
    {
	char_u	*str;

	എങ്കിൽ (PyBytes_AsStringAndSize(obj, (അക്ഷരം **) &str, ശൂന്യ) == -1)
	    തിരികെ -1;
	എങ്കിൽ (str == ശൂന്യ)
	    തിരികെ -1;

	എങ്കിൽ (set_string_copy(str, tv) == -1)
	    തിരികെ -1;

	tv->v_type = VAR_STRING;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (PyUnicode_Check(obj))
    {
	PyObject	*bytes;
	char_u	*str;

	bytes = PyUnicode_AsEncodedString(obj, ENC_OPT, ERRORS_ENCODE_ARG);
	എങ്കിൽ (bytes == ശൂന്യ)
	    തിരികെ -1;

	എങ്കിൽ (PyBytes_AsStringAndSize(bytes, (അക്ഷരം **) &str, ശൂന്യ) == -1)
	    തിരികെ -1;
	എങ്കിൽ (str == ശൂന്യ)
	    തിരികെ -1;

	എങ്കിൽ (set_string_copy(str, tv))
	{
	    Py_XDECREF(bytes);
	    തിരികെ -1;
	}
	Py_XDECREF(bytes);

	tv->v_type = VAR_STRING;
    }
#എങ്കിൽ PY_MAJOR_VERSION < 3
    അല്ലെങ്കിൽ എങ്കിൽ (PyInt_Check(obj))
    {
	tv->v_type = VAR_സംഖ്യ;
	tv->vval.v_സംഖ്യ = (varnumber_T) PyInt_AsLong(obj);
	എങ്കിൽ (PyErr_Occurred())
	    തിരികെ -1;
    }
#endif
    അല്ലെങ്കിൽ എങ്കിൽ (PyLong_Check(obj))
    {
	tv->v_type = VAR_സംഖ്യ;
	tv->vval.v_സംഖ്യ = (varnumber_T) PyLong_AsLong(obj);
	എങ്കിൽ (PyErr_Occurred())
	    തിരികെ -1;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (PyTuple_Check(obj))
	തിരികെ convert_dl(obj, tv, pytuple_to_tv, lookup_dict);
    അല്ലെങ്കിൽ എങ്കിൽ (PyDict_Check(obj))
	തിരികെ convert_dl(obj, tv, pydict_to_tv, lookup_dict);
    അല്ലെങ്കിൽ എങ്കിൽ (PyFloat_Check(obj))
    {
	tv->v_type = VAR_FLOAT;
	tv->vval.v_float = (float_T) PyFloat_AsDouble(obj);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (PyObject_HasAttrString(obj, "keys"))
	തിരികെ convert_dl(obj, tv, pymap_to_tv, lookup_dict);
    // PyObject_GetIter can create built-in iterator for any sequence object
    അല്ലെങ്കിൽ എങ്കിൽ (PyIter_Check(obj) || PySequence_Check(obj))
	തിരികെ convert_dl(obj, tv, pyseq_to_tv, lookup_dict);
    അല്ലെങ്കിൽ എങ്കിൽ (PyMapping_Check(obj))
	തിരികെ convert_dl(obj, tv, pymap_to_tv, lookup_dict);
    അല്ലെങ്കിൽ എങ്കിൽ (Py_സംഖ്യ_Check(obj))
    {
	PyObject	*അക്കം;

	എങ്കിൽ (!(അക്കം = Py_സംഖ്യ_Long(obj)))
	    തിരികെ -1;

	tv->v_type = VAR_സംഖ്യ;
	tv->vval.v_സംഖ്യ = (varnumber_T) PyLong_AsLong(അക്കം);

	Py_DECREF(അക്കം);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (obj == Py_None)
    {
	tv->v_type = VAR_SPECIAL;
	tv->vval.v_സംഖ്യ = VVAL_NONE;
    }
    അല്ലെങ്കിൽ
    {
	PyErr_FORMAT_TYPE(
		N_("unable to convert %s to a Vim structure"),
		obj);
	തിരികെ -1;
    }
    തിരികെ 0;
}

    സ്ഥിതം PyObject *
ConvertToPyObject(typval_T *tv)
{
    typval_T *argv;
    പൂർണ്ണസംഖ്യ i;
    എങ്കിൽ (tv == ശൂന്യ)
    {
	PyErr_SET_VIM(N_("internal error: NULL reference passed"));
	തിരികെ ശൂന്യ;
    }
    തിരഞ്ഞെടുപ്പ് (tv->v_type)
    {
	അവസ്ഥ VAR_STRING:
	    തിരികെ PyBytes_FromString(tv->vval.v_string == ശൂന്യ
					    ? "" : (അക്ഷരം *)tv->vval.v_string);
	അവസ്ഥ VAR_സംഖ്യ:
	    തിരികെ PyLong_FromLong((long) tv->vval.v_സംഖ്യ);
	അവസ്ഥ VAR_FLOAT:
	    തിരികെ PyFloat_FromDouble((ഇരട്ട) tv->vval.v_float);
	അവസ്ഥ VAR_LIST:
	    തിരികെ NEW_LIST(tv->vval.v_list);
	അവസ്ഥ VAR_TUPLE:
	    തിരികെ NEW_TUPLE(tv->vval.v_tuple);
	അവസ്ഥ VAR_DICT:
	    തിരികെ NEW_DICTIONARY(tv->vval.v_dict);
	അവസ്ഥ VAR_FUNC:
	    തിരികെ NEW_FUNCTION(tv->vval.v_string == ശൂന്യ
					  ? (char_u *)"" : tv->vval.v_string,
					  0, ശൂന്യ, ശൂന്യ, TRUE);
	അവസ്ഥ VAR_PARTIAL:
	    എങ്കിൽ (tv->vval.v_partial->pt_argc)
	    {
		argv = PyMem_New(typval_T, (size_t)tv->vval.v_partial->pt_argc);
		വേണ്ടി (i = 0; i < tv->vval.v_partial->pt_argc; i++)
		    copy_tv(&tv->vval.v_partial->pt_argv[i], &argv[i]);
	    }
	    അല്ലെങ്കിൽ
		argv = ശൂന്യ;
	    എങ്കിൽ (tv->vval.v_partial->pt_dict != ശൂന്യ)
		tv->vval.v_partial->pt_dict->dv_refcount++;
	    തിരികെ NEW_FUNCTION(tv->vval.v_partial == ശൂന്യ
			     ? (char_u *)"" : partial_name(tv->vval.v_partial),
				tv->vval.v_partial->pt_argc, argv,
				tv->vval.v_partial->pt_dict,
				tv->vval.v_partial->pt_auto);
	അവസ്ഥ VAR_BLOB:
	    തിരികെ PyBytes_FromStringAndSize(
		(അക്ഷരം*) tv->vval.v_blob->bv_ga.ga_data,
		(Py_ssize_t) tv->vval.v_blob->bv_ga.ga_len);
	അവസ്ഥ VAR_UNKNOWN:
	അവസ്ഥ VAR_ANY:
	അവസ്ഥ VAR_VOID:
	അവസ്ഥ VAR_CHANNEL:
	അവസ്ഥ VAR_JOB:
	അവസ്ഥ VAR_INSTR:
	അവസ്ഥ VAR_CLASS:
	അവസ്ഥ VAR_OBJECT:
	അവസ്ഥ VAR_TYPEALIAS:
	    Py_INCREF(Py_None);
	    തിരികെ Py_None;
	അവസ്ഥ VAR_BOOL:
	അവസ്ഥ VAR_SPECIAL:
	    തിരഞ്ഞെടുപ്പ് (tv->vval.v_സംഖ്യ)
	    {
		അവസ്ഥ VVAL_FALSE: തിരികെ ALWAYS_FALSE;
		അവസ്ഥ VVAL_TRUE:  തിരികെ ALWAYS_TRUE;
		അവസ്ഥ VVAL_NONE:
		അവസ്ഥ VVAL_NULL:  തിരികെ ALWAYS_NONE;
	    }
	    PyErr_SET_VIM(N_("internal error: invalid value type"));
	    തിരികെ ശൂന്യ;
    }
    തിരികെ ശൂന്യ;
}

DEFINE_PY_TYPE_OBJECT(CurrentType);

തരനിർവചനം ഘടന
{
    PyObject_HEAD
} CurrentObject;

സ്ഥിതം CurrentObject TheCurrent =
{
    PyObject_HEAD_INIT_TYPE(CurrentType)
};

    സ്ഥിതം ശൂന്യം
init_structs(ശൂന്യം)
{
    CLEAR_FIELD(OutputType);
    OutputType.tp_name = "vim.message";
    OutputType.tp_basicsize = വലുപ്പം(OutputObject);
    OutputType.tp_flags = Py_TPFLAGS_DEFAULT;
    OutputType.tp_doc = "vim message object";
    OutputType.tp_methods = OutputMethods;
#എങ്കിൽ PY_MAJOR_VERSION >= 3
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
#അല്ലെങ്കിൽ
    OutputType.tp_getattr = OutputGetattr;
    OutputType.tp_setattr = OutputSetattr;
    // Disabled, because this causes a crash in test86
    // OutputType.tp_base = &PyFile_Type;
#endif

    CLEAR_FIELD(IterType);
    IterType.tp_name = "vim.iter";
    IterType.tp_basicsize = വലുപ്പം(IterObject);
    IterType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    IterType.tp_doc = "generic iterator object";
    IterType.tp_iter = IterIter;
    IterType.tp_iternext = IterNext;
    IterType.tp_dealloc = IterDestructor;
    IterType.tp_traverse = IterTraverse;
    IterType.tp_clear = IterClear;

    CLEAR_FIELD(BufferType);
    BufferType.tp_name = "vim.buffer";
    BufferType.tp_basicsize = വലുപ്പം(BufferObject);
    BufferType.tp_dealloc = BufferDestructor;
    BufferType.tp_repr = BufferRepr;
    BufferType.tp_as_sequence = &BufferAsSeq;
    BufferType.tp_as_mapping = &BufferAsMapping;
    BufferType.tp_flags = Py_TPFLAGS_DEFAULT;
    BufferType.tp_doc = "vim buffer object";
    BufferType.tp_methods = BufferMethods;
#എങ്കിൽ PY_MAJOR_VERSION >= 3
    BufferType.tp_getattro = BufferGetattro;
    BufferType.tp_setattro = BufferSetattro;
    BufferType.tp_alloc = call_PyType_GenericAlloc;
    BufferType.tp_new = call_PyType_GenericNew;
    BufferType.tp_free = call_PyObject_Free;
#അല്ലെങ്കിൽ
    BufferType.tp_getattr = BufferGetattr;
    BufferType.tp_setattr = BufferSetattr;
#endif

    CLEAR_FIELD(WindowType);
    WindowType.tp_name = "vim.window";
    WindowType.tp_basicsize = വലുപ്പം(WindowObject);
    WindowType.tp_dealloc = WindowDestructor;
    WindowType.tp_repr = WindowRepr;
    WindowType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    WindowType.tp_doc = "vim Window object";
    WindowType.tp_methods = WindowMethods;
    WindowType.tp_traverse = WindowTraverse;
    WindowType.tp_clear = WindowClear;
#എങ്കിൽ PY_MAJOR_VERSION >= 3
    WindowType.tp_getattro = WindowGetattro;
    WindowType.tp_setattro = WindowSetattro;
    WindowType.tp_alloc = call_PyType_GenericAlloc;
    WindowType.tp_new = call_PyType_GenericNew;
    WindowType.tp_free = call_PyObject_Free;
#അല്ലെങ്കിൽ
    WindowType.tp_getattr = WindowGetattr;
    WindowType.tp_setattr = WindowSetattr;
#endif

    CLEAR_FIELD(TabPageType);
    TabPageType.tp_name = "vim.tabpage";
    TabPageType.tp_basicsize = വലുപ്പം(TabPageObject);
    TabPageType.tp_dealloc = TabPageDestructor;
    TabPageType.tp_repr = TabPageRepr;
    TabPageType.tp_flags = Py_TPFLAGS_DEFAULT;
    TabPageType.tp_doc = "vim tab page object";
    TabPageType.tp_methods = TabPageMethods;
#എങ്കിൽ PY_MAJOR_VERSION >= 3
    TabPageType.tp_getattro = TabPageGetattro;
    TabPageType.tp_alloc = call_PyType_GenericAlloc;
    TabPageType.tp_new = call_PyType_GenericNew;
    TabPageType.tp_free = call_PyObject_Free;
#അല്ലെങ്കിൽ
    TabPageType.tp_getattr = TabPageGetattr;
#endif

    CLEAR_FIELD(BufMapType);
    BufMapType.tp_name = "vim.bufferlist";
    BufMapType.tp_basicsize = വലുപ്പം(BufMapObject);
    BufMapType.tp_as_mapping = &BufMapAsMapping;
    BufMapType.tp_flags = Py_TPFLAGS_DEFAULT;
    BufMapType.tp_iter = BufMapIter;
    BufMapType.tp_doc = "vim buffer list";

    CLEAR_FIELD(WinListType);
    WinListType.tp_name = "vim.windowlist";
    WinListType.tp_basicsize = വലുപ്പം(WinListObject);
    WinListType.tp_as_sequence = &WinListAsSeq;
    WinListType.tp_flags = Py_TPFLAGS_DEFAULT;
    WinListType.tp_doc = "vim window list";
    WinListType.tp_dealloc = (destructor)WinListDestructor;

    CLEAR_FIELD(TabListType);
    TabListType.tp_name = "vim.tabpagelist";
    TabListType.tp_basicsize = വലുപ്പം(TabListObject);
    TabListType.tp_as_sequence = &TabListAsSeq;
    TabListType.tp_flags = Py_TPFLAGS_DEFAULT;
    TabListType.tp_doc = "vim tab page list";

    CLEAR_FIELD(RangeType);
    RangeType.tp_name = "vim.range";
    RangeType.tp_basicsize = വലുപ്പം(RangeObject);
    RangeType.tp_dealloc = RangeDestructor;
    RangeType.tp_repr = RangeRepr;
    RangeType.tp_as_sequence = &RangeAsSeq;
    RangeType.tp_as_mapping = &RangeAsMapping;
    RangeType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    RangeType.tp_doc = "vim Range object";
    RangeType.tp_methods = RangeMethods;
    RangeType.tp_traverse = RangeTraverse;
    RangeType.tp_clear = RangeClear;
#എങ്കിൽ PY_MAJOR_VERSION >= 3
    RangeType.tp_getattro = RangeGetattro;
    RangeType.tp_alloc = call_PyType_GenericAlloc;
    RangeType.tp_new = call_PyType_GenericNew;
    RangeType.tp_free = call_PyObject_Free;
#അല്ലെങ്കിൽ
    RangeType.tp_getattr = RangeGetattr;
#endif

    CLEAR_FIELD(CurrentType);
    CurrentType.tp_name = "vim.currentdata";
    CurrentType.tp_basicsize = വലുപ്പം(CurrentObject);
    CurrentType.tp_flags = Py_TPFLAGS_DEFAULT;
    CurrentType.tp_doc = "vim current object";
    CurrentType.tp_methods = CurrentMethods;
#എങ്കിൽ PY_MAJOR_VERSION >= 3
    CurrentType.tp_getattro = CurrentGetattro;
    CurrentType.tp_setattro = CurrentSetattro;
#അല്ലെങ്കിൽ
    CurrentType.tp_getattr = CurrentGetattr;
    CurrentType.tp_setattr = CurrentSetattr;
#endif

    CLEAR_FIELD(DictionaryType);
    DictionaryType.tp_name = "vim.dictionary";
    DictionaryType.tp_basicsize = വലുപ്പം(DictionaryObject);
    DictionaryType.tp_dealloc = DictionaryDestructor;
    DictionaryType.tp_as_sequence = &DictionaryAsSeq;
    DictionaryType.tp_as_mapping = &DictionaryAsMapping;
    DictionaryType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE;
    DictionaryType.tp_doc = "dictionary pushing modifications to Vim structure";
    DictionaryType.tp_methods = DictionaryMethods;
    DictionaryType.tp_iter = DictionaryIter;
    DictionaryType.tp_new = DictionaryConstructor;
    DictionaryType.tp_alloc = PyType_GenericAlloc;
#എങ്കിൽ PY_MAJOR_VERSION >= 3
    DictionaryType.tp_getattro = DictionaryGetattro;
    DictionaryType.tp_setattro = DictionarySetattro;
#അല്ലെങ്കിൽ
    DictionaryType.tp_getattr = DictionaryGetattr;
    DictionaryType.tp_setattr = DictionarySetattr;
#endif

    CLEAR_FIELD(ListType);
    ListType.tp_name = "vim.list";
    ListType.tp_dealloc = ListDestructor;
    ListType.tp_basicsize = വലുപ്പം(ListObject);
    ListType.tp_as_sequence = &ListAsSeq;
    ListType.tp_as_mapping = &ListAsMapping;
    ListType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE;
    ListType.tp_doc = "list pushing modifications to Vim structure";
    ListType.tp_methods = ListMethods;
    ListType.tp_iter = ListIter;
    ListType.tp_new = ListConstructor;
    ListType.tp_alloc = PyType_GenericAlloc;
#എങ്കിൽ PY_MAJOR_VERSION >= 3
    ListType.tp_getattro = ListGetattro;
    ListType.tp_setattro = ListSetattro;
#അല്ലെങ്കിൽ
    ListType.tp_getattr = ListGetattr;
    ListType.tp_setattr = ListSetattr;
#endif

    // Tuple type
    CLEAR_FIELD(TupleType);
    TupleType.tp_name = "vim.tuple";
    TupleType.tp_dealloc = TupleDestructor;
    TupleType.tp_basicsize = വലുപ്പം(TupleObject);
    TupleType.tp_as_sequence = &TupleAsSeq;
    TupleType.tp_as_mapping = &TupleAsMapping;
    TupleType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE;
    TupleType.tp_doc = "tuple pushing modifications to Vim structure";
    TupleType.tp_methods = TupleMethods;
    TupleType.tp_iter = TupleIter;
    TupleType.tp_new = TupleConstructor;
    TupleType.tp_alloc = PyType_GenericAlloc;
#എങ്കിൽ PY_MAJOR_VERSION >= 3
    TupleType.tp_getattro = TupleGetattro;
    TupleType.tp_setattro = TupleSetattro;
#അല്ലെങ്കിൽ
    TupleType.tp_getattr = TupleGetattr;
    TupleType.tp_setattr = TupleSetattr;
#endif

    CLEAR_FIELD(FunctionType);
    FunctionType.tp_name = "vim.function";
    FunctionType.tp_basicsize = വലുപ്പം(FunctionObject);
    FunctionType.tp_dealloc = FunctionDestructor;
    FunctionType.tp_call = FunctionCall;
    FunctionType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE;
    FunctionType.tp_doc = "object that calls Vim function";
    FunctionType.tp_methods = FunctionMethods;
    FunctionType.tp_repr = FunctionRepr;
    FunctionType.tp_new = FunctionConstructor;
    FunctionType.tp_alloc = PyType_GenericAlloc;
#എങ്കിൽ PY_MAJOR_VERSION >= 3
    FunctionType.tp_getattro = FunctionGetattro;
#അല്ലെങ്കിൽ
    FunctionType.tp_getattr = FunctionGetattr;
#endif

    CLEAR_FIELD(OptionsType);
    OptionsType.tp_name = "vim.options";
    OptionsType.tp_basicsize = വലുപ്പം(OptionsObject);
    OptionsType.tp_as_sequence = &OptionsAsSeq;
    OptionsType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    OptionsType.tp_doc = "object for manipulating options";
    OptionsType.tp_iter = OptionsIter;
    OptionsType.tp_as_mapping = &OptionsAsMapping;
    OptionsType.tp_dealloc = OptionsDestructor;
    OptionsType.tp_traverse = OptionsTraverse;
    OptionsType.tp_clear = OptionsClear;

#എങ്കിൽ PY_VERSION_HEX < 0x030700f0
    CLEAR_FIELD(LoaderType);
    LoaderType.tp_name = "vim.Loader";
    LoaderType.tp_basicsize = വലുപ്പം(LoaderObject);
    LoaderType.tp_flags = Py_TPFLAGS_DEFAULT;
    LoaderType.tp_doc = "vim message object";
    LoaderType.tp_methods = LoaderMethods;
    LoaderType.tp_dealloc = LoaderDestructor;
#endif

#എങ്കിൽ PY_MAJOR_VERSION >= 3
    CLEAR_FIELD(vimmodule);
    vimmodule.m_name = "vim";
    vimmodule.m_doc = "Vim Python interface\n";
    vimmodule.m_size = -1;
    vimmodule.m_methods = VimMethods;
#endif
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
init_types(ശൂന്യം)
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
#എങ്കിൽ PY_VERSION_HEX < 0x030700f0
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
    തിരികെ 0;
}

#ifdef USE_LIMITED_API
    സ്ഥിതം ശൂന്യം
shutdown_types(ശൂന്യം)
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
# എങ്കിൽ PY_VERSION_HEX < 0x030700f0
    PYTYPE_CLEANUP(LoaderType);
# endif
}
#endif

    സ്ഥിതം പൂർണ്ണസംഖ്യ
init_sys_path(ശൂന്യം)
{
    PyObject	*path;
    PyObject	*path_hook;
    PyObject	*path_hooks;

    എങ്കിൽ (!(path_hook = PyObject_GetAttrString(vim_module, "path_hook")))
	തിരികെ -1;

    എങ്കിൽ (!(path_hooks = PySys_GetObject("path_hooks")))
    {
	PyErr_Clear();
	path_hooks = PyList_New(1);
	PyList_SET_ITEM(path_hooks, 0, path_hook);
	എങ്കിൽ (PySys_SetObject("path_hooks", path_hooks))
	{
	    Py_DECREF(path_hooks);
	    തിരികെ -1;
	}
	Py_DECREF(path_hooks);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (PyList_Check(path_hooks))
    {
	എങ്കിൽ (PyList_Append(path_hooks, path_hook))
	{
	    Py_DECREF(path_hook);
	    തിരികെ -1;
	}
	Py_DECREF(path_hook);
    }
    അല്ലെങ്കിൽ
    {
	VimTryStart();
	emsg(_("Failed to set path hook: sys.path_hooks is not a list\n"
	       "You should now do the following:\n"
	       "- append vim.path_hook to sys.path_hooks\n"
	       "- append vim.VIM_SPECIAL_PATH to sys.path\n"));
	VimTryEnd(); // Discard the error
	Py_DECREF(path_hook);
	തിരികെ 0;
    }

    എങ്കിൽ (!(path = PySys_GetObject("path")))
    {
	PyErr_Clear();
	path = PyList_New(1);
	Py_INCREF(vim_special_path_object);
	PyList_SET_ITEM(path, 0, vim_special_path_object);
	എങ്കിൽ (PySys_SetObject("path", path))
	{
	    Py_DECREF(path);
	    തിരികെ -1;
	}
	Py_DECREF(path);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (PyList_Check(path))
    {
	എങ്കിൽ (PyList_Append(path, vim_special_path_object))
	    തിരികെ -1;
    }
    അല്ലെങ്കിൽ
    {
	VimTryStart();
	emsg(_("Failed to set path: sys.path is not a list\n"
	       "You should now append vim.VIM_SPECIAL_PATH to sys.path"));
	VimTryEnd(); // Discard the error
    }

    തിരികെ 0;
}

സ്ഥിതം ഘടന numeric_constant {
    അക്ഷരം	*name;
    പൂർണ്ണസംഖ്യ		val;
} numeric_constants[] = {
    {"VAR_LOCKED",	VAR_LOCKED},
    {"VAR_FIXED",	VAR_FIXED},
    {"VAR_SCOPE",	VAR_SCOPE},
    {"VAR_DEF_SCOPE",	VAR_DEF_SCOPE},
};

ഘടന object_constant {
    അക്ഷരം	*name;
    PyObject	*valObject;
};

#define ADD_OBJECT(m, name, obj) \
    എങ്കിൽ (PyModule_AddObject(m, name, obj)) \
	തിരികെ -1;

#define ADD_CHECKED_OBJECT(m, name, obj) \
    { \
	PyObject	*valObject = obj; \
	എങ്കിൽ (!valObject) \
	    തിരികെ -1; \
	ADD_OBJECT(m, name, valObject); \
    }

    സ്ഥിതം പൂർണ്ണസംഖ്യ
populate_module(PyObject *m)
{
    പൂർണ്ണസംഖ്യ		i;
    PyObject	*other_module;
    PyObject	*attr;
    PyObject	*imp;
#എങ്കിൽ PY_VERSION_HEX >= 0x030700f0
    PyObject	*dict;
    PyObject	*cls;
#endif

    വേണ്ടി (i = 0; i < (പൂർണ്ണസംഖ്യ)(വലുപ്പം(numeric_constants)
					   / വലുപ്പം(ഘടന numeric_constant));
	    ++i)
	ADD_CHECKED_OBJECT(m, numeric_constants[i].name,
		PyInt_FromLong(numeric_constants[i].val));

    ഘടന object_constant object_constants[] = {
	{"buffers",  (PyObject *)(ശൂന്യം *)&TheBufferMap},
	{"windows",  (PyObject *)(ശൂന്യം *)&TheWindowList},
	{"tabpages", (PyObject *)(ശൂന്യം *)&TheTabPageList},
	{"current",  (PyObject *)(ശൂന്യം *)&TheCurrent},

	{"Buffer",     (PyObject *)BufferTypePtr},
	{"Range",      (PyObject *)RangeTypePtr},
	{"Window",     (PyObject *)WindowTypePtr},
	{"TabPage",    (PyObject *)TabPageTypePtr},
	{"Dictionary", (PyObject *)DictionaryTypePtr},
	{"List",       (PyObject *)ListTypePtr},
	{"Tuple",      (PyObject *)TupleTypePtr},
	{"Function",   (PyObject *)FunctionTypePtr},
	{"Options",    (PyObject *)OptionsTypePtr},
#എങ്കിൽ PY_VERSION_HEX < 0x030700f0
	{"_Loader",    (PyObject *)LoaderTypePtr},
#endif
    };

    വേണ്ടി (i = 0; i < (പൂർണ്ണസംഖ്യ)(വലുപ്പം(object_constants)
					    / വലുപ്പം(ഘടന object_constant));
	    ++i)
    {
	PyObject	*valObject;

	valObject = object_constants[i].valObject;
	Py_INCREF(valObject);
	ADD_OBJECT(m, object_constants[i].name, valObject);
    }

    എങ്കിൽ (!(VimError = PyErr_NewException("vim.error", ശൂന്യ, ശൂന്യ)))
	തിരികെ -1;
    ADD_OBJECT(m, "error", VimError);

    ADD_CHECKED_OBJECT(m, "vars",  NEW_DICTIONARY(get_globvar_dict()));
    ADD_CHECKED_OBJECT(m, "vvars", NEW_DICTIONARY(get_vimvar_dict()));
    ADD_CHECKED_OBJECT(m, "options",
	    OptionsNew(SREQ_GLOBAL, ശൂന്യ, dummy_check, ശൂന്യ));

    എങ്കിൽ (!(other_module = PyImport_ImportModule("os")))
	തിരികെ -1;
    ADD_OBJECT(m, "os", other_module);

#എങ്കിൽ PY_MAJOR_VERSION >= 3
    എങ്കിൽ (!(py_getcwd = PyObject_GetAttrString(other_module, "getcwd")))
	തിരികെ -1;
#അല്ലെങ്കിൽ
    എങ്കിൽ (!(py_getcwd = PyObject_GetAttrString(other_module, "getcwdu")))
	തിരികെ -1;
#endif
    ADD_OBJECT(m, "_getcwd", py_getcwd)

    എങ്കിൽ (!(py_chdir = PyObject_GetAttrString(other_module, "chdir")))
	തിരികെ -1;
    ADD_OBJECT(m, "_chdir", py_chdir);
    എങ്കിൽ (!(attr = PyObject_GetAttrString(m, "chdir")))
	തിരികെ -1;
    എങ്കിൽ (PyObject_SetAttrString(other_module, "chdir", attr))
    {
	Py_DECREF(attr);
	തിരികെ -1;
    }
    Py_DECREF(attr);

    എങ്കിൽ ((py_fchdir = PyObject_GetAttrString(other_module, "fchdir")))
    {
	ADD_OBJECT(m, "_fchdir", py_fchdir);
	എങ്കിൽ (!(attr = PyObject_GetAttrString(m, "fchdir")))
	    തിരികെ -1;
	എങ്കിൽ (PyObject_SetAttrString(other_module, "fchdir", attr))
	{
	    Py_DECREF(attr);
	    തിരികെ -1;
	}
	Py_DECREF(attr);
    }
    അല്ലെങ്കിൽ
	PyErr_Clear();

    എങ്കിൽ (!(vim_special_path_object = PyString_FromString(vim_special_path)))
	തിരികെ -1;

    ADD_OBJECT(m, "VIM_SPECIAL_PATH", vim_special_path_object);

#എങ്കിൽ PY_VERSION_HEX >= 0x030700f0
    എങ്കിൽ (!(imp = PyImport_ImportModule("importlib.machinery")))
	തിരികെ -1;

    dict = PyModule_GetDict(imp);

    എങ്കിൽ (!(cls = PyDict_GetItemString(dict, "PathFinder")))
    {
	Py_DECREF(imp);
	തിരികെ -1;
    }

    എങ്കിൽ (!(py_find_spec = PyObject_GetAttrString(cls, "find_spec")))
    {
	Py_DECREF(imp);
	തിരികെ -1;
    }

# എങ്കിൽ PY_VERSION_HEX < 0x30c00a7
    // find_module has been removed as of Python 3.12.0a7
    എങ്കിൽ ((py_find_module = PyObject_GetAttrString(cls, "find_module")))
	ADD_OBJECT(m, "_find_module", py_find_module);
# endif

    Py_DECREF(imp);

    ADD_OBJECT(m, "_find_spec", py_find_spec);
#അല്ലെങ്കിൽ
    എങ്കിൽ (!(imp = PyImport_ImportModule("imp")))
	തിരികെ -1;

    എങ്കിൽ (!(py_find_module = PyObject_GetAttrString(imp, "find_module")))
    {
	Py_DECREF(imp);
	തിരികെ -1;
    }

    എങ്കിൽ (!(py_load_module = PyObject_GetAttrString(imp, "load_module")))
    {
	Py_DECREF(py_find_module);
	Py_DECREF(imp);
	തിരികെ -1;
    }

    Py_DECREF(imp);

    ADD_OBJECT(m, "_find_module", py_find_module);
    ADD_OBJECT(m, "_load_module", py_load_module);
#endif

    തിരികെ 0;
}
