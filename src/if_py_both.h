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

#सचे PY_VERSION_HEX < 0x02050000
प्रकारनिर्देश पूर्णाङ्क Py_ssize_t;  // Python 2.4 and earlier don't have this type.
#endif

// Use values that are known to work, others may make Vim crash.
#define ENC_OPT (enc_utf8 ? "utf-8" : enc_dbcs ? "euc-jp" : (वर्ण *)p_enc)
#define DOPY_FUNC "_vim_pydo"

स्थिर ध्रुव वर्ण *vim_special_path = "_vim_path_";

#define PyErr_SET_STRING(exc, str) PyErr_SetString(exc, _(str))
#define PyErr_SetVim(str) PyErr_SetString(VimError, str)
#define PyErr_SET_VIM(str) PyErr_SET_STRING(VimError, str)
#define PyErr_FORMAT(exc, str, arg) PyErr_Format(exc, _(str), arg)
#define PyErr_FORMAT2(exc, str, arg1, arg2) PyErr_Format(exc, _(str), arg1,arg2)
#define PyErr_VIM_FORMAT(str, arg) PyErr_FORMAT(VimError, str, arg)

#ifdef USE_LIMITED_API
// Limited Python API. Need to call only exposed functions and remap macros.
// PyTypeObject is an opaque struct.

प्रकारनिर्देश संरचना {
    lenfunc sq_length;
    binaryfunc sq_concat;
    ssizeargfunc sq_repeat;
    ssizeargfunc sq_item;
    शून्य *was_sq_slice;
    ssizeobjargproc sq_ass_item;
    शून्य *was_sq_ass_slice;
    objobjproc sq_contains;

    binaryfunc sq_inplace_concat;
    ssizeargfunc sq_inplace_repeat;
} PySequenceMethods;

प्रकारनिर्देश संरचना {
    lenfunc mp_length;
    binaryfunc mp_subscript;
    objobjargproc mp_ass_subscript;
} PyMappingMethods;

// This struct emulates the concrete _typeobject struct to allow the code to
// work the same way in both limited and full Python APIs.
संरचना typeobject_wrapper {
    ध्रुव वर्ण *tp_name;
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

    ध्रुव वर्ण *tp_doc;

    traverseproc tp_traverse;

    inquiry tp_clear;

    getiterfunc tp_iter;
    iternextfunc tp_iternext;

    संरचना PyMethodDef *tp_methods;
    संरचना _typeobject *tp_base;
    allocfunc tp_alloc;
    newfunc tp_new;
    freefunc tp_free;
};

# define DEFINE_PY_TYPE_OBJECT(type) \
    स्थिर संरचना typeobject_wrapper type; \
    स्थिर PyTypeObject* type##Ptr = रिक्त

// PyObject_HEAD_INIT_TYPE and PyObject_FINISH_INIT_TYPE need to come in pairs
// We first initialize with NULL because the type is not allocated until
// init_types() is called later. It's in FINISH_INIT_TYPE where we fill the
// type in with the newly allocated type.
# define PyObject_HEAD_INIT_TYPE(type) PyObject_HEAD_INIT(रिक्त)
# define PyObject_FINISH_INIT_TYPE(obj, type) obj.ob_base.ob_type = type##Ptr

# define Py_TYPE_GET_TP_ALLOC(type) ((allocfunc)PyType_GetSlot(type, Py_tp_alloc))
# define Py_TYPE_GET_TP_METHODS(type) ((PyMethodDef *)PyType_GetSlot(type, Py_tp_methods))

// PyObject_NEW is not part of stable ABI, but PyObject_Malloc/Init are.
स्थिर PyObject* Vim_PyObject_New(PyTypeObject *type, size_t objsize)
{
    PyObject *obj = (PyObject *)PyObject_Malloc(objsize);
    सचे (obj == रिक्त)
	पटिनिवत्त PyErr_NoMemory();
    पटिनिवत्त PyObject_Init(obj, type);
}
# undef PyObject_NEW
# define PyObject_NEW(type, typeobj) ((type *)Vim_PyObject_New(typeobj, परिमाण(type)))

// This is a somewhat convoluted because limited API doesn't expose an easy way
// to get the tp_name field, and so we have to manually reconstruct it as
// "__module__.__name__" (with __module__ omitted for builtins to emulate
// Python behavior). Also, some of the more convenient functions like
// PyUnicode_AsUTF8AndSize and PyType_GetQualName() are not available until
// late Python 3 versions, and won't be available if you set Py_LIMITED_API too
// low.
# define PyErr_FORMAT_TYPE(सन्देशः, obj) \
    करोहि { \
	PyObject* qualname = PyObject_GetAttrString((PyObject*)(obj)->ob_type, "__qualname__"); \
	सचे (qualname == रिक्त) \
	{ \
	    PyErr_FORMAT(PyExc_TypeError, सन्देशः, "(NULL)"); \
	    भङ्ग; \
	} \
	PyObject* module = PyObject_GetAttrString((PyObject*)(obj)->ob_type, "__module__"); \
	PyObject* full; \
	सचे (module == रिक्त || PyUnicode_CompareWithASCIIString(module, "builtins") == 0 \
		|| PyUnicode_CompareWithASCIIString(module, "__main__") == 0) \
	{ \
	    full = qualname; \
	    Py_INCREF(full); \
	} \
	अञ्ञथा \
	    full = PyUnicode_FromFormat("%U.%U", module, qualname); \
	PyObject* full_bytes = PyUnicode_AsUTF8String(full); \
	ध्रुव वर्ण* full_str = PyBytes_AsString(full_bytes); \
	full_str = full_str == रिक्त ? "(NULL)" : full_str; \
	PyErr_FORMAT(PyExc_TypeError, सन्देशः, full_str); \
	Py_DECREF(qualname); \
	Py_XDECREF(module); \
	Py_XDECREF(full); \
	Py_XDECREF(full_bytes); \
    } याव (0)

# define PyList_GET_ITEM(list, i) PyList_GetItem(list, i)
# define PyList_GET_SIZE(o) PyList_Size(o)

// PyList_SET_ITEM and PyList_SetItem have slightly different behaviors. The
// former will leave the old item dangling, and the latter will decref on it.
// Since we only use this on new lists, this difference doesn't matter.
# define PyList_SET_ITEM(list, i, item) PyList_SetItem(list, i, item)

# define PyTuple_GET_ITEM(o, pos) PyTuple_GetItem(o, pos)
# define PyTuple_GET_SIZE(o) PyTuple_Size(o)

# सचे Py_LIMITED_API < 0x03080000
// PyIter_check only became part of stable ABI in 3.8, and there is no easy way
// to check for it in the API. We simply return false as a compromise. This
// does mean we should avoid compiling with stable ABI < 3.8.
#  undef PyIter_Check
#  define PyIter_Check(obj) (FALSE)
# endif

स्थिर PyTypeObject* AddHeapType(संरचना typeobject_wrapper* type_object)
{
    PyType_Spec type_spec;
    type_spec.name = type_object->tp_name;
    type_spec.basicsize = type_object->tp_basicsize;
    type_spec.itemsize = 0;
    type_spec.flags = type_object->tp_flags;

    // We just need to statically allocate a large enough buffer that can hold
    // all slots. We need to leave a null-terminated slot at the end.
    PyType_Slot slots[40] = { {0, रिक्त} };
    size_t slot_i = 0;

# define ADD_TP_SLOT(slot_name) \
    सचे (slot_i >= 40) पटिनिवत्त रिक्त; /* this should never happen */ \
    सचे (type_object->slot_name != रिक्त) \
    { \
	slots[slot_i].slot = Py_##slot_name; \
	slots[slot_i].pfunc = (शून्य*)type_object->slot_name; \
	++slot_i; \
    }
# define ADD_TP_SUB_SLOT(sub_slot, slot_name) \
    सचे (slot_i >= 40) पटिनिवत्त रिक्त; /* this should never happen */ \
    सचे (type_object->sub_slot != रिक्त && type_object->sub_slot->slot_name != रिक्त) \
    { \
	slots[slot_i].slot = Py_##slot_name; \
	slots[slot_i].pfunc = (शून्य*)type_object->sub_slot->slot_name; \
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
    पटिनिवत्त (PyTypeObject*)newtype;
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
    सचे (type##Ptr == रिक्त) \
	पटिनिवत्त -1;
# define PYTYPE_CLEANUP(type) \
    type##Ptr = रिक्त;

// Limited API does not provide PyRun_* functions. Need to implement manually
// using PyCompile and PyEval.
स्थिर PyObject* Vim_PyRun_String(ध्रुव वर्ण *str, पूर्णाङ्क start, PyObject *globals, PyObject *locals)
{
    // Just pass "" for filename for now.
    PyObject* compiled = Py_CompileString(str, "", start);
    सचे (compiled == रिक्त)
	पटिनिवत्त रिक्त;

    PyObject* eval_result = PyEval_EvalCode(compiled, globals, locals);
    Py_DECREF(compiled);
    पटिनिवत्त eval_result;
}
स्थिर पूर्णाङ्क Vim_PyRun_SimpleString(ध्रुव वर्ण *str)
{
    // This function emulates CPython's implementation.
    PyObject* m = PyImport_AddModule("__main__");
    सचे (m == रिक्त)
	पटिनिवत्त -1;
    PyObject* d = PyModule_GetDict(m);
    PyObject* output = Vim_PyRun_String(str, Py_file_निवेशः, d, d);
    सचे (output == रिक्त)
    {
	PyErr_PrintEx(TRUE);
	पटिनिवत्त -1;
    }
    Py_DECREF(output);
    पटिनिवत्त 0;
}
# define PyRun_String Vim_PyRun_String
# define PyRun_SimpleString Vim_PyRun_SimpleString

#अञ्ञथा // !defined(USE_LIMITED_API)

// Full Python API. Can make use of structs and macros directly.
# define DEFINE_PY_TYPE_OBJECT(type) \
    स्थिर PyTypeObject type; \
    स्थिर PyTypeObject* type##Ptr = &type
# define PyObject_HEAD_INIT_TYPE(type) PyObject_HEAD_INIT(&type)

# define Py_TYPE_GET_TP_ALLOC(type) type->tp_alloc
# define Py_TYPE_GET_TP_METHODS(type) type->tp_methods

# define Py_TYPE_NAME(obj) ((obj)->ob_type->tp_name == रिक्त \
	? "(NULL)" \
	: (obj)->ob_type->tp_name)
# define PyErr_FORMAT_TYPE(सन्देशः, obj) \
    PyErr_FORMAT(PyExc_TypeError, सन्देशः, \
	    Py_TYPE_NAME(obj))

// Add a static type
# define PYTYPE_READY(type) \
    सचे (PyType_Ready(type##Ptr)) \
	पटिनिवत्त -1;

#endif


#define RAISE_NO_EMPTY_कुञ्चिकाः PyErr_SET_STRING(Py_Exc_मानम्_Error, \
					    N_("empty keys are not allowed"))
#define RAISE_LOCKED_DICTIONARY PyErr_SET_VIM(N_("dictionary is locked"))
#define RAISE_LOCKED_LIST PyErr_SET_VIM(N_("list is locked"))
#define RAISE_UNDO_FAIL PyErr_SET_VIM(N_("cannot save undo information"))
#define RAISE_DELETE_पङ्क्तिः_FAIL PyErr_SET_VIM(N_("cannot delete line"))
#define RAISE_निवेशय_पङ्क्तिः_FAIL PyErr_SET_VIM(N_("cannot insert line"))
#define RAISE_REPLACE_पङ्क्तिः_FAIL PyErr_SET_VIM(N_("cannot replace line"))
#define RAISE_कुञ्चिका_ADD_FAIL(कुञ्चिका) \
    PyErr_VIM_FORMAT(N_("failed to add key '%s' to dictionary"), कुञ्चिका)
#define RAISE_INVALID_INDEX_TYPE(idx) \
    PyErr_FORMAT_TYPE(N_("index must be int or slice, not %s"), idx);

#define INVALID_BUFFER_मानम् ((buf_T *)(-1))
#define INVALID_WINDOW_मानम् ((win_T *)(-1))
#define INVALID_TABPAGE_मानम् ((tabpage_T *)(-1))

प्रकारनिर्देश शून्य (*rangeinitializer)(शून्य *);
प्रकारनिर्देश शून्य (*runner)(ध्रुव वर्ण *, dict_T *, शून्य *
#ifdef PY_CAN_RECURSE
	, PyGILState_STATE *
#endif
	);

स्थिर पूर्णाङ्क ConvertFromPyObject(PyObject *, typval_T *);
स्थिर पूर्णाङ्क _ConvertFromPyObject(PyObject *, typval_T *, PyObject *);
स्थिर पूर्णाङ्क ConvertFromPyMapping(PyObject *, typval_T *);
स्थिर पूर्णाङ्क ConvertFromPySequence(PyObject *, typval_T *);
स्थिर PyObject *WindowNew(win_T *, tabpage_T *);
स्थिर PyObject *BufferNew (buf_T *);
स्थिर PyObject *पङ्क्तिः_To_String(ध्रुव वर्ण *);

स्थिर PyInt RangeStart;
स्थिर PyInt RangeEnd;

स्थिर PyObject *globals;

स्थिर PyObject *py_chdir;
स्थिर PyObject *py_fchdir;
स्थिर PyObject *py_getcwd;
स्थिर PyObject *vim_module;
स्थिर PyObject *vim_special_path_object;

#सचे PY_VERSION_HEX >= 0x030700f0
स्थिर PyObject *py_find_spec;
#अञ्ञथा
स्थिर PyObject *py_load_module;
#endif
#सचे PY_VERSION_HEX < 0x30c00a7
स्थिर PyObject *py_find_module;
#endif

स्थिर PyObject *VimError;

/*
 * obtain a lock on the Vim data structures
 */
    स्थिर शून्य
Python_Lock_Vim(शून्य)
{
}

/*
 * release a lock on the Vim data structures
 */
    स्थिर शून्य
Python_Release_Vim(शून्य)
{
}

/*
 * The "todecref" argument holds a pointer to PyObject * that must be
 * DECREF'ed after returned char_u * is no longer needed or NULL if all what
 * was needed to generate returned value is object.
 *
 * Use Py_XDECREF to decrement reference count.
 */
    स्थिर char_u *
StringToChars(PyObject *obj, PyObject **todecref)
{
    char_u	*str;

    सचे (PyBytes_Check(obj))
    {

	सचे (PyBytes_AsStringAndSize(obj, (वर्ण **) &str, रिक्त) == -1
		|| str == रिक्त)
	    पटिनिवत्त रिक्त;

	*todecref = रिक्त;
    }
    अञ्ञथा सचे (PyUnicode_Check(obj))
    {
	PyObject	*bytes;

	सचे (!(bytes = PyUnicode_AsEncodedString(obj, ENC_OPT,
							   ERRORS_ENCODE_ARG)))
	    पटिनिवत्त रिक्त;

	सचे (PyBytes_AsStringAndSize(bytes, (वर्ण **) &str, रिक्त) == -1
		|| str == रिक्त)
	{
	    Py_DECREF(bytes);
	    पटिनिवत्त रिक्त;
	}

	*todecref = bytes;
    }
    अञ्ञथा
    {
#सचे PY_MAJOR_VERSION < 3
	PyErr_FORMAT_TYPE(N_("expected str() or unicode() instance, but got %s"),
		obj);
#अञ्ञथा
	PyErr_FORMAT_TYPE(N_("expected bytes() or str() instance, but got %s"),
		obj);
#endif
	पटिनिवत्त रिक्त;
    }

    पटिनिवत्त (char_u *) str;
}

#define सङ्ख्या_LONG     1
#define सङ्ख्या_INT      2
#define सङ्ख्या_NATURAL  4
#define सङ्ख्या_UNSIGNED 8

    स्थिर पूर्णाङ्क
सङ्ख्या_To_Long(PyObject *obj, long *result, पूर्णाङ्क flags)
{
#सचे PY_MAJOR_VERSION < 3
    सचे (PyInt_Check(obj))
    {
	*result = PyInt_AsLong(obj);
	सचे (PyErr_Occurred())
	    पटिनिवत्त -1;
    }
    अञ्ञथा
#endif
    सचे (PyLong_Check(obj))
    {
	*result = PyLong_AsLong(obj);
	सचे (PyErr_Occurred())
	    पटिनिवत्त -1;
    }
    अञ्ञथा सचे (Py_सङ्ख्या_Check(obj))
    {
	PyObject	*अङ्कः;

	सचे (!(अङ्कः = Py_सङ्ख्या_Long(obj)))
	    पटिनिवत्त -1;

	*result = PyLong_AsLong(अङ्कः);

	Py_DECREF(अङ्कः);

	सचे (PyErr_Occurred())
	    पटिनिवत्त -1;
    }
    अञ्ञथा
    {
#सचे PY_MAJOR_VERSION < 3
	PyErr_FORMAT_TYPE(
		N_("expected int(), long() or something supporting "
		   "coercing to long(), but got %s"),
		obj);
#अञ्ञथा
	PyErr_FORMAT_TYPE(
		N_("expected int() or something supporting coercing to int(), "
		   "but got %s"),
		obj);
#endif
	पटिनिवत्त -1;
    }

    सचे (flags & सङ्ख्या_INT)
    {
	सचे (*result > INT_MAX)
	{
	    PyErr_SET_STRING(PyExc_OverflowError,
		    N_("value is too large to fit into C int type"));
	    पटिनिवत्त -1;
	}
	अञ्ञथा सचे (*result < INT_MIN)
	{
	    PyErr_SET_STRING(PyExc_OverflowError,
		    N_("value is too small to fit into C int type"));
	    पटिनिवत्त -1;
	}
    }

    सचे (flags & सङ्ख्या_NATURAL)
    {
	सचे (*result <= 0)
	{
	    PyErr_SET_STRING(Py_Exc_मानम्_Error,
		    N_("number must be greater than zero"));
	    पटिनिवत्त -1;
	}
    }
    अञ्ञथा सचे (flags & सङ्ख्या_UNSIGNED)
    {
	सचे (*result < 0)
	{
	    PyErr_SET_STRING(Py_Exc_मानम्_Error,
		    N_("number must be greater or equal to zero"));
	    पटिनिवत्त -1;
	}
    }

    पटिनिवत्त 0;
}

    स्थिर पूर्णाङ्क
add_string(PyObject *list, वर्ण *s)
{
    PyObject	*string;

    सचे (!(string = PyString_FromString(s)))
	पटिनिवत्त -1;

    सचे (PyList_Append(list, string))
    {
	Py_DECREF(string);
	पटिनिवत्त -1;
    }

    Py_DECREF(string);
    पटिनिवत्त 0;
}

    स्थिर PyObject *
ObjectDir(PyObject *self, वर्ण **attributes)
{
    PyMethodDef	*method;
    वर्ण	**attr;
    PyObject	*ret;

    सचे (!(ret = PyList_New(0)))
	पटिनिवत्त रिक्त;

    सचे (self)
	अत्थाय (method = Py_TYPE_GET_TP_METHODS(self->ob_type) ; method->ml_name != रिक्त ; ++method)
	    सचे (add_string(ret, (वर्ण *)method->ml_name))
	    {
		Py_DECREF(ret);
		पटिनिवत्त रिक्त;
	    }

    अत्थाय (attr = attributes ; *attr ; ++attr)
	सचे (add_string(ret, *attr))
	{
	    Py_DECREF(ret);
	    पटिनिवत्त रिक्त;
	}

#सचे PY_MAJOR_VERSION < 3
    सचे (add_string(ret, "__members__"))
    {
	Py_DECREF(ret);
	पटिनिवत्त रिक्त;
    }
#endif

    पटिनिवत्त ret;
}

// Output buffer management

// Function to write a line, points to either msg() or emsg().
प्रकारनिर्देश पूर्णाङ्क (*writefn)(वर्ण *);

DEFINE_PY_TYPE_OBJECT(OutputType);

प्रकारनिर्देश संरचना
{
    PyObject_HEAD
    long softspace;
    long error;
} OutputObject;

स्थिर वर्ण *OutputAttrs[] = {
    "softspace",
    रिक्त
};

    स्थिर PyObject *
OutputDir(PyObject *self, PyObject *args UNUSED)
{
    पटिनिवत्त ObjectDir(self, OutputAttrs);
}

    स्थिर पूर्णाङ्क
OutputSetattr(PyObject *self_obj, वर्ण *name, PyObject *valObject)
{
    OutputObject *self = (OutputObject*)self_obj;
    सचे (valObject == रिक्त)
    {
	PyErr_SET_STRING(PyExc_AttributeError,
		N_("can't delete OutputObject attributes"));
	पटिनिवत्त -1;
    }

    सचे (strcmp(name, "softspace") == 0)
    {
	सचे (सङ्ख्या_To_Long(valObject, &(self->softspace), सङ्ख्या_UNSIGNED))
	    पटिनिवत्त -1;
	पटिनिवत्त 0;
    }

    PyErr_FORMAT(PyExc_AttributeError, N_("invalid attribute: %s"), name);
    पटिनिवत्त -1;
}

// Buffer IO, we write one whole line at a time.
स्थिर garray_T io_ga = {0, 0, 1, 80, रिक्त};
स्थिर writefn old_fn = रिक्त;

    स्थिर शून्य
PythonIO_Flush(शून्य)
{
    सचे (old_fn != रिक्त && io_ga.ga_len > 0)
    {
	((वर्ण *)io_ga.ga_data)[io_ga.ga_len] = NUL;
	// We don't know what emsg_severe should be here, so ... hope?
	old_fn((वर्ण *)io_ga.ga_data);
    }
    io_ga.ga_len = 0;
}

    स्थिर शून्य
writer(writefn fn, char_u *str, PyInt n, पूर्णाङ्क severe)
{
    char_u *ptr;

    // Flush when switching output function.
    सचे (fn != old_fn)
	PythonIO_Flush();
    old_fn = fn;

    // Write each NL separated line.  Text after the last NL is kept for
    // writing later.
    // For normal messages: Do not output when "got_int" was set.  This avoids
    // a loop gone crazy flooding the terminal with messages.  Also for when
    // "q" is pressed at the more-prompt.
    याव (n > 0 && (ptr = memchr(str, '\n', n)) != रिक्त
					  && (fn == (writefn)emsg || !got_int))
    {
	PyInt len = ptr - str;

	सचे (ga_grow(&io_ga, (पूर्णाङ्क)(len + 1)) == FAIL)
	    भङ्ग;

	mch_memmove(((वर्ण *)io_ga.ga_data) + io_ga.ga_len, str, (size_t)len);
	((वर्ण *)io_ga.ga_data)[io_ga.ga_len + len] = NUL;
	emsg_severe = severe;
	fn((वर्ण *)io_ga.ga_data);
	str = ptr + 1;
	n -= len + 1;
	io_ga.ga_len = 0;
    }

    // Put the remaining text into io_ga for later printing.
    सचे (n > 0 && (fn == (writefn)emsg || !got_int)
					&& ga_grow(&io_ga, (पूर्णाङ्क)(n + 1)) == OK)
    {
	mch_memmove(((वर्ण *)io_ga.ga_data) + io_ga.ga_len, str, (size_t)n);
	io_ga.ga_len += (पूर्णाङ्क)n;
    }
}

    स्थिर पूर्णाङ्क
write_output(OutputObject *self, PyObject *string)
{
    Py_ssize_t	len = 0;
    वर्ण	*str = रिक्त;
    पूर्णाङ्क		error = self->error;

    सचे (!PyArg_Parse(string, "et#", ENC_OPT, &str, &len))
	पटिनिवत्त -1;

    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    writer((writefn)(error ? emsg : सन्देशः), (char_u *)str, len, error);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS
    PyMem_Free(str);

    पटिनिवत्त 0;
}

    स्थिर PyObject *
OutputWrite(OutputObject *self, PyObject *string)
{
    सचे (write_output(self, string))
	पटिनिवत्त रिक्त;

    Py_INCREF(Py_None);
    पटिनिवत्त Py_None;
}

    स्थिर PyObject *
OutputWritelines(OutputObject *self, PyObject *seq)
{
    PyObject	*iterator;
    PyObject	*item;

    सचे (!(iterator = PyObject_GetIter(seq)))
	पटिनिवत्त रिक्त;

    याव ((item = PyIter_Next(iterator)))
    {
	सचे (write_output(self, item))
	{
	    Py_DECREF(iterator);
	    Py_DECREF(item);
	    पटिनिवत्त रिक्त;
	}
	Py_DECREF(item);
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    सचे (PyErr_Occurred())
	पटिनिवत्त रिक्त;

    Py_INCREF(Py_None);
    पटिनिवत्त Py_None;
}

    स्थिर PyObject *
AlwaysNone(PyObject *self UNUSED, PyObject *args UNUSED)
{
    // do nothing
    Py_INCREF(Py_None);
    पटिनिवत्त Py_None;
}
#define ALWAYS_NONE AlwaysNone(रिक्त, रिक्त)

    स्थिर PyObject *
AlwaysFalse(PyObject *self UNUSED, PyObject *args UNUSED)
{
    // do nothing
    PyObject	*ret = Py_False;
    Py_INCREF(ret);
    पटिनिवत्त ret;
}
#define ALWAYS_FALSE AlwaysFalse(रिक्त, रिक्त)

    स्थिर PyObject *
AlwaysTrue(PyObject *self UNUSED, PyObject *args UNUSED)
{
    // do nothing
    PyObject	*ret = Py_True;
    Py_INCREF(ret);
    पटिनिवत्त ret;
}
#define ALWAYS_TRUE AlwaysTrue(रिक्त, रिक्त)

/***************/

स्थिर संरचना PyMethodDef OutputMethods[] = {
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
    { रिक्त,	    रिक्त,				0,		रिक्त}
};

स्थिर OutputObject Output =
{
    PyObject_HEAD_INIT_TYPE(OutputType)
    0,
    0
};

स्थिर OutputObject Error =
{
    PyObject_HEAD_INIT_TYPE(OutputType)
    0,
    1
};

    स्थिर पूर्णाङ्क
PythonIO_Init_io(शून्य)
{
    सचे (PySys_SetObject("stdout", (PyObject *)(शून्य *)&Output))
	पटिनिवत्त -1;
    सचे (PySys_SetObject("stderr", (PyObject *)(शून्य *)&Error))
	पटिनिवत्त -1;

    सचे (PyErr_Occurred())
    {
	emsg(_(e_python_error_initialising_io_object));
	पटिनिवत्त -1;
    }

    पटिनिवत्त 0;
}

#सचे PY_VERSION_HEX < 0x030700f0
स्थिर PyObject *call_load_module(वर्ण *name, पूर्णाङ्क len, PyObject *find_module_result);

प्रकारनिर्देश संरचना
{
    PyObject_HEAD
    वर्ण	*fullname;
    PyObject	*result;
} LoaderObject;
DEFINE_PY_TYPE_OBJECT(LoaderType);

    स्थिर शून्य
LoaderDestructor(PyObject *self_obj)
{
    LoaderObject *self = (LoaderObject*)self_obj;
    vim_free(self->fullname);
    Py_XDECREF(self->result);
    DESTRUCTOR_FINISH(self);
}

    स्थिर PyObject *
LoaderLoadModule(LoaderObject *self, PyObject *args UNUSED)
{
    वर्ण	*fullname = self->fullname;
    PyObject	*result = self->result;
    PyObject	*module;

    सचे (!fullname)
    {
	module = result ? result : Py_None;
	Py_INCREF(module);
	पटिनिवत्त module;
    }

    module = call_load_module(fullname, (पूर्णाङ्क)STRLEN(fullname), result);

    self->fullname = रिक्त;
    self->result = module;

    vim_free(fullname);
    Py_DECREF(result);

    सचे (!module)
    {
	सचे (PyErr_Occurred())
	    पटिनिवत्त रिक्त;

	Py_INCREF(Py_None);
	पटिनिवत्त Py_None;
    }

    Py_INCREF(module);
    पटिनिवत्त module;
}

स्थिर संरचना PyMethodDef LoaderMethods[] = {
    // name,	    function,				calling,	doc
    {"load_module", (PyCFunction)LoaderLoadModule,	METH_VARARGS,	""},
    { रिक्त,	    रिक्त,				0,		रिक्त}
};
#endif

/*
 * Check to see whether a Vim error has been reported, or a keyboard
 * interrupt has been detected.
 */
    स्थिर शून्य
VimTryStart(शून्य)
{
    ++trylevel;
}

    स्थिर पूर्णाङ्क
VimTryEnd(शून्य)
{
    --trylevel;
    // Without this it stops processing all subsequent Vim script commands and
    // generates strange error messages if I e.g. try calling Test() in a cycle
    did_emsg = FALSE;
    // Keyboard interrupt should be preferred over anything else
    सचे (got_int)
    {
	सचे (did_throw)
	    discard_current_exception();
	got_int = FALSE;
	PyErr_SetNone(PyExc_KeyboardInterrupt);
	पटिनिवत्त -1;
    }
    अञ्ञथा सचे (सन्देशः_list != रिक्त && *सन्देशः_list != रिक्त)
    {
	पूर्णाङ्क	should_free;
	वर्ण	*सन्देशः;

	सन्देशः = get_exception_string(*सन्देशः_list, ET_ERROR, रिक्त, &should_free);

	सचे (सन्देशः == रिक्त)
	{
	    PyErr_NoMemory();
	    पटिनिवत्त -1;
	}

	PyErr_SetVim(सन्देशः);

	free_global_msglist();

	सचे (should_free)
	    vim_free(सन्देशः);

	पटिनिवत्त -1;
    }
    अञ्ञथा सचे (!did_throw)
	पटिनिवत्त (PyErr_Occurred() ? -1 : 0);
    // Python exception is preferred over Vim one; unlikely to occur though
    अञ्ञथा सचे (PyErr_Occurred())
    {
	discard_current_exception();
	पटिनिवत्त -1;
    }
    // Finally transform Vim script exception to python one
    अञ्ञथा
    {
	PyErr_SetVim((वर्ण *)current_exception->मानम्);
	discard_current_exception();
	पटिनिवत्त -1;
    }
}

    स्थिर पूर्णाङ्क
VimCheckInterrupt(शून्य)
{
    सचे (got_int)
    {
	PyErr_SetNone(PyExc_KeyboardInterrupt);
	पटिनिवत्त 1;
    }
    पटिनिवत्त 0;
}

// Vim module - Implementation

    स्थिर PyObject *
VimCommand(PyObject *self UNUSED, PyObject *string)
{
    char_u	*cmd;
    PyObject	*ret;
    PyObject	*todecref;

    सचे (!(cmd = StringToChars(string, &todecref)))
	पटिनिवत्त रिक्त;

    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();

    VimTryStart();
    do_cmdline_cmd(cmd);
    update_screen(UPD_VALID);

    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    सचे (VimTryEnd())
	ret = रिक्त;
    अञ्ञथा
	ret = Py_None;

    Py_XINCREF(ret);
    Py_XDECREF(todecref);
    पटिनिवत्त ret;
}

/*
 * Function to translate a typval_T into a PyObject; this will recursively
 * translate lists/dictionaries into their Python equivalents.
 *
 * The depth parameter is to avoid infinite recursion, set it to 1 when
 * you call VimToPython.
 */
    स्थिर PyObject *
VimToPython(typval_T *our_tv, पूर्णाङ्क depth, PyObject *lookup_dict)
{
    PyObject	*ret;
    PyObject	*newObj;
    वर्ण	ptrBuf[परिमाण(शून्य *) * 2 + 3];

    // Avoid infinite recursion
    सचे (depth > 100)
    {
	Py_INCREF(Py_None);
	ret = Py_None;
	पटिनिवत्त ret;
    }

    // Check if we run into a recursive loop.  The item must be in lookup_dict
    // then and we can use it again.
    सचे ((our_tv->v_type == VAR_LIST && our_tv->vval.v_list != रिक्त)
	    || (our_tv->v_type == VAR_TUPLE && our_tv->vval.v_tuple != रिक्त)
	    || (our_tv->v_type == VAR_DICT && our_tv->vval.v_dict != रिक्त))
    {
	sprintf(ptrBuf, "%p",
		our_tv->v_type == VAR_LIST ? (शून्य *)our_tv->vval.v_list
		    : our_tv->v_type == VAR_TUPLE ?
			(शून्य *)our_tv->vval.v_tuple
			: (शून्य *)our_tv->vval.v_dict);

	सचे ((ret = PyDict_GetItemString(lookup_dict, ptrBuf)))
	{
	    Py_INCREF(ret);
	    पटिनिवत्त ret;
	}
    }

    सचे (our_tv->v_type == VAR_STRING)
	ret = PyString_FromString(our_tv->vval.v_string == रिक्त
					? "" : (वर्ण *)our_tv->vval.v_string);
    अञ्ञथा सचे (our_tv->v_type == VAR_सङ्ख्या)
    {
	वर्ण buf[NUMBUFLEN];

	// For backwards compatibility numbers are stored as strings.
	sprintf(buf, "%lld", (long long)our_tv->vval.v_सङ्ख्या);
	ret = PyString_FromString((वर्ण *)buf);
    }
    अञ्ञथा सचे (our_tv->v_type == VAR_FLOAT)
    {
	वर्ण buf[NUMBUFLEN];

	sprintf(buf, "%f", our_tv->vval.v_float);
	ret = PyString_FromString((वर्ण *)buf);
    }
    अञ्ञथा सचे (our_tv->v_type == VAR_LIST)
    {
	list_T		*list = our_tv->vval.v_list;
	listitem_T	*curr;

	सचे (list == रिक्त)
	    पटिनिवत्त रिक्त;

	सचे (!(ret = PyList_New(0)))
	    पटिनिवत्त रिक्त;

	सचे (PyDict_SetItemString(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    पटिनिवत्त रिक्त;
	}

	CHECK_LIST_MATERIALIZE(list);
	FOR_ALL_LIST_वस्तूनि(list, curr)
	{
	    सचे (!(newObj = VimToPython(&curr->li_tv, depth + 1, lookup_dict)))
	    {
		Py_DECREF(ret);
		पटिनिवत्त रिक्त;
	    }
	    सचे (PyList_Append(ret, newObj))
	    {
		Py_DECREF(newObj);
		Py_DECREF(ret);
		पटिनिवत्त रिक्त;
	    }
	    Py_DECREF(newObj);
	}
    }
    अञ्ञथा सचे (our_tv->v_type == VAR_TUPLE)
    {
	tuple_T	*tuple = our_tv->vval.v_tuple;
	पूर्णाङ्क	len;

	सचे (tuple == रिक्त)
	    पटिनिवत्त रिक्त;

	len = TUPLE_LEN(tuple);

	ret = PyTuple_New(len);
	सचे (ret == रिक्त)
	    पटिनिवत्त रिक्त;

	अत्थाय (पूर्णाङ्क idx = 0; idx < len; idx++)
	{
	    typval_T	*item_tv = TUPLE_ITEM(tuple, idx);

	    newObj = VimToPython(item_tv, depth + 1, lookup_dict);
	    सचे (!newObj)
	    {
		Py_DECREF(ret);
		पटिनिवत्त रिक्त;
	    }
	    PyTuple_SetItem(ret, idx, newObj);
	}

	सचे (PyDict_SetItemString(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    पटिनिवत्त रिक्त;
	}
    }
    अञ्ञथा सचे (our_tv->v_type == VAR_DICT)
    {

	hashtab_T	*ht;
	long_u		todo;
	hashitem_T	*hi;
	dictitem_T	*di;

	सचे (our_tv->vval.v_dict == रिक्त)
	    पटिनिवत्त रिक्त;
	ht = &our_tv->vval.v_dict->dv_hashtab;

	सचे (!(ret = PyDict_New()))
	    पटिनिवत्त रिक्त;

	सचे (PyDict_SetItemString(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    पटिनिवत्त रिक्त;
	}

	todo = ht->ht_used;
	अत्थाय (hi = ht->ht_array; todo > 0; ++hi)
	{
	    सचे (!HASHITEM_EMPTY(hi))
	    {
		--todo;

		di = dict_lookup(hi);
		सचे (!(newObj = VimToPython(&di->di_tv, depth + 1, lookup_dict)))
		{
		    Py_DECREF(ret);
		    पटिनिवत्त रिक्त;
		}
		सचे (PyDict_SetItemString(ret, (वर्ण *)hi->hi_कुञ्चिका, newObj))
		{
		    Py_DECREF(ret);
		    Py_DECREF(newObj);
		    पटिनिवत्त रिक्त;
		}
	    }
	}
    }
    अञ्ञथा सचे (our_tv->v_type == VAR_BOOL)
    {
	सचे (our_tv->vval.v_सङ्ख्या == VVAL_FALSE)
	{
	    ret = Py_False;
	    Py_INCREF(ret);
	}
	अञ्ञथा
	{
	    ret = Py_True;
	    Py_INCREF(ret);
	}
	पटिनिवत्त ret;
    }
    अञ्ञथा सचे (our_tv->v_type == VAR_SPECIAL)
    {
	Py_INCREF(Py_None);
	ret = Py_None;
	पटिनिवत्त ret;
    }
    अञ्ञथा सचे (our_tv->v_type == VAR_BLOB)
	ret = PyBytes_FromStringAndSize(
		(वर्ण*) our_tv->vval.v_blob->bv_ga.ga_data,
		(Py_ssize_t) our_tv->vval.v_blob->bv_ga.ga_len);
    अञ्ञथा
    {
	Py_INCREF(Py_None);
	ret = Py_None;
    }

    पटिनिवत्त ret;
}

    स्थिर PyObject *
VimEval(PyObject *self UNUSED, PyObject *args)
{
    char_u	*व्यञ्जकम्;
    typval_T	*our_tv;
    PyObject	*string;
    PyObject	*todecref;
    PyObject	*ret;
    PyObject	*lookup_dict;

    सचे (!PyArg_ParseTuple(args, "O", &string))
	पटिनिवत्त रिक्त;

    सचे (!(व्यञ्जकम् = StringToChars(string, &todecref)))
	पटिनिवत्त रिक्त;

    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    VimTryStart();
    our_tv = eval_व्यञ्जकम्(व्यञ्जकम्, रिक्त);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    Py_XDECREF(todecref);

    सचे (VimTryEnd())
	पटिनिवत्त रिक्त;

    सचे (our_tv == रिक्त)
    {
	PyErr_SET_VIM(N_("invalid expression"));
	पटिनिवत्त रिक्त;
    }

    // Convert the Vim type into a Python type.  Create a dictionary that's
    // used to check for recursive loops.
    सचे (!(lookup_dict = PyDict_New()))
	ret = रिक्त;
    अञ्ञथा
    {
	ret = VimToPython(our_tv, 1, lookup_dict);
	Py_DECREF(lookup_dict);
    }


    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    free_tv(our_tv);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    पटिनिवत्त ret;
}

स्थिर PyObject *ConvertToPyObject(typval_T *);

    स्थिर PyObject *
VimEvalPy(PyObject *self UNUSED, PyObject *string)
{
    typval_T	*our_tv;
    PyObject	*ret;
    char_u	*व्यञ्जकम्;
    PyObject	*todecref;

    सचे (!(व्यञ्जकम् = StringToChars(string, &todecref)))
	पटिनिवत्त रिक्त;

    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    VimTryStart();
    our_tv = eval_व्यञ्जकम्(व्यञ्जकम्, रिक्त);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    Py_XDECREF(todecref);

    सचे (VimTryEnd())
	पटिनिवत्त रिक्त;

    सचे (our_tv == रिक्त)
    {
	PyErr_SET_VIM(N_("invalid expression"));
	पटिनिवत्त रिक्त;
    }

    ret = ConvertToPyObject(our_tv);
    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    free_tv(our_tv);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    पटिनिवत्त ret;
}

    स्थिर PyObject *
VimStrwidth(PyObject *self UNUSED, PyObject *string)
{
    char_u	*str;
    PyObject	*todecref;
    पूर्णाङ्क		len;

    सचे (!(str = StringToChars(string, &todecref)))
	पटिनिवत्त रिक्त;

    len = mb_string2cells(str, (पूर्णाङ्क)STRLEN(str));

    Py_XDECREF(todecref);

    पटिनिवत्त PyLong_FromLong(len);
}

    स्थिर PyObject *
_VimChdir(PyObject *_chdir, PyObject *args, PyObject *kwargs)
{
    PyObject	*ret;
    PyObject	*newwd;
    PyObject	*todecref;
    char_u	*new_dir;

    सचे (_chdir == रिक्त)
	पटिनिवत्त रिक्त;
    सचे (!(ret = PyObject_Call(_chdir, args, kwargs)))
	पटिनिवत्त रिक्त;

    सचे (!(newwd = PyObject_CallFunctionObjArgs(py_getcwd, रिक्त)))
    {
	Py_DECREF(ret);
	पटिनिवत्त रिक्त;
    }

    सचे (!(new_dir = StringToChars(newwd, &todecref)))
    {
	Py_DECREF(ret);
	Py_DECREF(newwd);
	पटिनिवत्त रिक्त;
    }

    VimTryStart();

    सचे (vim_chdir(new_dir))
    {
	Py_DECREF(ret);
	Py_DECREF(newwd);
	Py_XDECREF(todecref);

	सचे (VimTryEnd())
	    पटिनिवत्त रिक्त;

	PyErr_SET_VIM(N_("failed to change directory"));
	पटिनिवत्त रिक्त;
    }

    Py_DECREF(newwd);
    Py_XDECREF(todecref);

    post_chdir(CDSCOPE_GLOBAL);

    सचे (VimTryEnd())
    {
	Py_DECREF(ret);
	पटिनिवत्त रिक्त;
    }

    पटिनिवत्त ret;
}

    स्थिर PyObject *
VimChdir(PyObject *self UNUSED, PyObject *args, PyObject *kwargs)
{
    पटिनिवत्त _VimChdir(py_chdir, args, kwargs);
}

    स्थिर PyObject *
VimFchdir(PyObject *self UNUSED, PyObject *args, PyObject *kwargs)
{
    पटिनिवत्त _VimChdir(py_fchdir, args, kwargs);
}

प्रकारनिर्देश संरचना {
    PyObject	*callable;
    PyObject	*result;
} map_rtp_data;

    स्थिर शून्य
map_rtp_callback(char_u *path, शून्य *_data)
{
    शून्य	**data = (शून्य **) _data;
    PyObject	*pathObject;
    map_rtp_data	*mr_data = *((map_rtp_data **) data);

    सचे (!(pathObject = PyString_FromString((वर्ण *)path)))
    {
	*data = रिक्त;
	पटिनिवत्त;
    }

    mr_data->result = PyObject_CallFunctionObjArgs(mr_data->callable,
						   pathObject, रिक्त);

    Py_DECREF(pathObject);

    सचे (!mr_data->result || mr_data->result != Py_None)
	*data = रिक्त;
    अञ्ञथा
    {
	Py_DECREF(mr_data->result);
	mr_data->result = रिक्त;
    }
}

    स्थिर PyObject *
VimForeachRTP(PyObject *self UNUSED, PyObject *callable)
{
    map_rtp_data	data;

    data.callable = callable;
    data.result = रिक्त;

    do_in_runtimepath(रिक्त, 0, &map_rtp_callback, &data);

    सचे (data.result == रिक्त)
    {
	सचे (PyErr_Occurred())
	    पटिनिवत्त रिक्त;
	अञ्ञथा
	{
	    Py_INCREF(Py_None);
	    पटिनिवत्त Py_None;
	}
    }
    पटिनिवत्त data.result;
}

/*
 * _vim_runtimepath_ special path implementation.
 */

    स्थिर शून्य
map_finder_callback(char_u *path, शून्य *_data)
{
    शून्य	**data = (शून्य **) _data;
    PyObject	*list = *((PyObject **) data);
    PyObject	*pathObject1, *pathObject2;
    वर्ण	*pathbuf;
    size_t	pathlen;

    pathlen = STRLEN(path);

#सचे PY_MAJOR_VERSION < 3
# define PY_MAIN_DIR_STRING "python2"
#अञ्ञथा
# define PY_MAIN_DIR_STRING "python3"
#endif
#define PY_ALTERNATE_DIR_STRING "pythonx"

#define PYTHONX_STRING_LENGTH 7 // STRLEN("pythonx")
    सचे (!(pathbuf = PyMem_New(वर्ण,
		    pathlen + STRLEN(PATHSEPSTR) + PYTHONX_STRING_LENGTH + 1)))
    {
	PyErr_NoMemory();
	*data = रिक्त;
	पटिनिवत्त;
    }

    mch_memmove(pathbuf, path, pathlen + 1);
    add_pathsep((char_u *) pathbuf);

    pathlen = STRLEN(pathbuf);
    mch_memmove(pathbuf + pathlen, PY_MAIN_DIR_STRING,
	    PYTHONX_STRING_LENGTH + 1);

    सचे (!(pathObject1 = PyString_FromString(pathbuf)))
    {
	*data = रिक्त;
	PyMem_Free(pathbuf);
	पटिनिवत्त;
    }

    mch_memmove(pathbuf + pathlen, PY_ALTERNATE_DIR_STRING,
	    PYTHONX_STRING_LENGTH + 1);

    सचे (!(pathObject2 = PyString_FromString(pathbuf)))
    {
	Py_DECREF(pathObject1);
	PyMem_Free(pathbuf);
	*data = रिक्त;
	पटिनिवत्त;
    }

    PyMem_Free(pathbuf);

    सचे (PyList_Append(list, pathObject1)
	    || PyList_Append(list, pathObject2))
	*data = रिक्त;

    Py_DECREF(pathObject1);
    Py_DECREF(pathObject2);
}

    स्थिर PyObject *
Vim_GetPaths(PyObject *self UNUSED, PyObject *args UNUSED)
{
    PyObject	*ret;

    सचे (!(ret = PyList_New(0)))
	पटिनिवत्त रिक्त;

    do_in_runtimepath(रिक्त, 0, &map_finder_callback, ret);

    सचे (PyErr_Occurred())
    {
	Py_DECREF(ret);
	पटिनिवत्त रिक्त;
    }

    पटिनिवत्त ret;
}

#सचे PY_VERSION_HEX >= 0x030700f0
    स्थिर PyObject *
FinderFindSpec(PyObject *self, PyObject *args)
{
    वर्ण	*fullname;
    PyObject	*paths;
    PyObject	*target = Py_None;
    PyObject	*spec;

    सचे (!PyArg_ParseTuple(args, "s|O", &fullname, &target))
	पटिनिवत्त रिक्त;

    सचे (!(paths = Vim_GetPaths(self, रिक्त)))
	पटिनिवत्त रिक्त;

    spec = PyObject_CallFunction(py_find_spec, "sOO", fullname, paths, target);

    Py_DECREF(paths);

    सचे (!spec)
    {
	सचे (PyErr_Occurred())
	    पटिनिवत्त रिक्त;

	Py_INCREF(Py_None);
	पटिनिवत्त Py_None;
    }

    पटिनिवत्त spec;
}

    स्थिर PyObject *
FinderFindModule(PyObject* self UNUSED, PyObject* args UNUSED)
{
    // Apparently returning None works.
    Py_INCREF(Py_None);
    पटिनिवत्त Py_None;
}
#अञ्ञथा
    स्थिर PyObject *
call_load_module(वर्ण *name, पूर्णाङ्क len, PyObject *find_module_result)
{
    PyObject	*fd, *pathname, *description;

    सचे (!PyTuple_Check(find_module_result))
    {
	PyErr_FORMAT_TYPE(
		N_("expected 3-tuple as imp.find_module() result, but got %s"),
		find_module_result);
	पटिनिवत्त रिक्त;
    }
    सचे (PyTuple_GET_SIZE(find_module_result) != 3)
    {
	PyErr_FORMAT(PyExc_TypeError,
		N_("expected 3-tuple as imp.find_module() result, but got "
		   "tuple of size %d"),
		(पूर्णाङ्क) PyTuple_GET_SIZE(find_module_result));
	पटिनिवत्त रिक्त;
    }

    सचे (!(fd = PyTuple_GET_ITEM(find_module_result, 0))
	    || !(pathname = PyTuple_GET_ITEM(find_module_result, 1))
	    || !(description = PyTuple_GET_ITEM(find_module_result, 2)))
    {
	PyErr_SET_STRING(PyExc_RuntimeError,
		N_("internal error: imp.find_module returned tuple with NULL"));
	पटिनिवत्त रिक्त;
    }

    पटिनिवत्त PyObject_CallFunction(py_load_module,
	    "s#OOO", name, len, fd, pathname, description);
}

    स्थिर PyObject *
find_module(वर्ण *fullname, वर्ण *tail, PyObject *new_path)
{
    PyObject	*find_module_result;
    PyObject	*module;
    वर्ण	*dot;

    सचे ((dot = (वर्ण *)vim_strchr((char_u *) tail, '.')))
    {
	/*
	 * There is a dot in the name: call find_module recursively without the
	 * first component
	 */
	PyObject	*newest_path;
	पूर्णाङ्क		partlen = (पूर्णाङ्क) (dot - 1 - tail);

	सचे (!(find_module_result = PyObject_CallFunction(py_find_module,
			"s#O", tail, partlen, new_path)))
	{
	    सचे (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_ImportError))
		PyErr_Clear();
	    पटिनिवत्त रिक्त;
	}

	सचे (!(module = call_load_module(
			fullname,
			((पूर्णाङ्क) (tail - fullname)) + partlen,
			find_module_result)))
	{
	    Py_DECREF(find_module_result);
	    पटिनिवत्त रिक्त;
	}

	Py_DECREF(find_module_result);

	सचे (!(newest_path = PyObject_GetAttrString(module, "__path__")))
	{
	    Py_DECREF(module);
	    पटिनिवत्त रिक्त;
	}

	Py_DECREF(module);

	find_module_result = find_module(fullname, dot + 1, newest_path);

	Py_DECREF(newest_path);

	पटिनिवत्त find_module_result;
    }
    अञ्ञथा
    {
	सचे (!(find_module_result = PyObject_CallFunction(py_find_module,
			"sO", tail, new_path)))
	{
	    सचे (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_ImportError))
		PyErr_Clear();
	    पटिनिवत्त रिक्त;
	}

	पटिनिवत्त find_module_result;
    }
}

    स्थिर PyObject *
FinderFindModule(PyObject *self, PyObject *args)
{
    वर्ण	*fullname;
    PyObject	*result;
    PyObject	*new_path;
    LoaderObject	*loader;

    सचे (!PyArg_ParseTuple(args, "s", &fullname))
	पटिनिवत्त रिक्त;

    सचे (!(new_path = Vim_GetPaths(self, रिक्त)))
	पटिनिवत्त रिक्त;

    result = find_module(fullname, fullname, new_path);

    Py_DECREF(new_path);

    सचे (!result)
    {
	सचे (PyErr_Occurred())
	    पटिनिवत्त रिक्त;

	Py_INCREF(Py_None);
	पटिनिवत्त Py_None;
    }

    सचे (!(fullname = (वर्ण *)vim_strsave((char_u *)fullname)))
    {
	Py_DECREF(result);
	PyErr_NoMemory();
	पटिनिवत्त रिक्त;
    }

    सचे (!(loader = PyObject_NEW(LoaderObject, LoaderTypePtr)))
    {
	vim_free(fullname);
	Py_DECREF(result);
	पटिनिवत्त रिक्त;
    }

    loader->fullname = fullname;
    loader->result = result;

    पटिनिवत्त (PyObject *) loader;
}
#endif

    स्थिर PyObject *
VimPathHook(PyObject *self UNUSED, PyObject *args)
{
    वर्ण	*path;

    सचे (PyArg_ParseTuple(args, "s", &path)
	    && STRCMP(path, vim_special_path) == 0)
    {
	Py_INCREF(vim_module);
	पटिनिवत्त vim_module;
    }

    PyErr_Clear();
    PyErr_SetNone(PyExc_ImportError);
    पटिनिवत्त रिक्त;
}

/*
 * Vim module - Definitions
 */

स्थिर संरचना PyMethodDef VimMethods[] = {
    // name,	    function,			calling,			documentation
    {"command",	    VimCommand,			METH_O,				"Execute a Vim ex-mode command" },
    {"eval",	    VimEval,			METH_VARARGS,			"Evaluate an expression using Vim evaluator" },
    {"bindeval",    VimEvalPy,			METH_O,				"Like eval(), but returns objects attached to Vim ones"},
    {"strwidth",    VimStrwidth,		METH_O,				"Screen string width, counts <Tab> as having width 1"},
    {"chdir",	    (PyCFunction)(शून्य *)VimChdir,	METH_VARARGS|METH_KEYWORDS,	"Change directory"},
    {"fchdir",	    (PyCFunction)(शून्य *)VimFchdir,	METH_VARARGS|METH_KEYWORDS,	"Change directory"},
    {"foreach_rtp", VimForeachRTP,		METH_O,				"Call given callable for each path in &rtp"},
#सचे PY_VERSION_HEX >= 0x030700f0
    {"find_spec",   FinderFindSpec,		METH_VARARGS,			"Internal use only, returns spec object for any input it receives"},
#endif
    {"find_module", FinderFindModule,		METH_VARARGS,			"Internal use only, returns loader object for any input it receives"},
    {"path_hook",   VimPathHook,		METH_VARARGS,			"Hook function to install in sys.path_hooks"},
    {"_get_paths",  (PyCFunction)Vim_GetPaths,	METH_NOARGS,			"Get &rtp-based additions to sys.path"},
    { रिक्त,	    रिक्त,			0,				रिक्त}
};

/*
 * Generic iterator object
 */

DEFINE_PY_TYPE_OBJECT(IterType);

प्रकारनिर्देश PyObject *(*nextfun)(शून्य **);
प्रकारनिर्देश शून्य (*destructorfun)(शून्य *);
प्रकारनिर्देश पूर्णाङ्क (*traversefun)(शून्य *, visitproc, शून्य *);
प्रकारनिर्देश पूर्णाङ्क (*clearfun)(शून्य **);

// Main purpose of this object is removing the need for do python
// initialization (i.e. PyType_Ready and setting type attributes) for a big
// bunch of objects.
प्रकारनिर्देश संरचना
{
    PyObject_HEAD
    शून्य *cur;
    nextfun next;
    destructorfun destruct;
    traversefun traverse;
    clearfun clear;
    PyObject *iter_object;
} IterObject;

    स्थिर PyObject *
IterNew(शून्य *start, destructorfun destruct, nextfun next, traversefun traverse,
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

    सचे (iter_object)
	Py_INCREF(iter_object);

    पटिनिवत्त (PyObject *)(self);
}

    स्थिर शून्य
IterDestructor(PyObject *self_obj)
{
    IterObject *self = (IterObject*)self_obj;
    सचे (self->iter_object)
	Py_DECREF(self->iter_object);
    PyObject_GC_UnTrack((शून्य *)(self));
    self->destruct(self->cur);
    PyObject_GC_Del((शून्य *)(self));
}

    स्थिर पूर्णाङ्क
IterTraverse(PyObject *self_obj, visitproc visit, शून्य *arg)
{
    IterObject *self = (IterObject*)self_obj;
    सचे (self->traverse != रिक्त)
	पटिनिवत्त self->traverse(self->cur, visit, arg);
    अञ्ञथा
	पटिनिवत्त 0;
}

// Mac OSX defines clear() somewhere.
#ifdef clear
# undef clear
#endif

    स्थिर पूर्णाङ्क
IterClear(PyObject *self_obj)
{
    IterObject *self = (IterObject*)self_obj;
    सचे (self->clear != रिक्त)
	पटिनिवत्त self->clear(&self->cur);
    अञ्ञथा
	पटिनिवत्त 0;
}

    स्थिर PyObject *
IterNext(PyObject *self_obj)
{
    IterObject *self = (IterObject*)self_obj;
    पटिनिवत्त self->next(&self->cur);
}

    स्थिर PyObject *
IterIter(PyObject *self)
{
    Py_INCREF(self);
    पटिनिवत्त self;
}

प्रकारनिर्देश संरचना pylinkedlist_S {
    संरचना pylinkedlist_S	*pll_next;
    संरचना pylinkedlist_S	*pll_prev;
    PyObject			*pll_obj;
} pylinkedlist_T;

स्थिर pylinkedlist_T *lastdict = रिक्त;
स्थिर pylinkedlist_T *lastlist = रिक्त;
स्थिर pylinkedlist_T *lasttuple = रिक्त;
स्थिर pylinkedlist_T *lastfunc = रिक्त;

    स्थिर शून्य
pyll_remove(pylinkedlist_T *ref, pylinkedlist_T **last)
{
    सचे (ref->pll_prev == रिक्त)
    {
	सचे (ref->pll_next == रिक्त)
	{
	    *last = रिक्त;
	    पटिनिवत्त;
	}
    }
    अञ्ञथा
	ref->pll_prev->pll_next = ref->pll_next;

    सचे (ref->pll_next == रिक्त)
	*last = ref->pll_prev;
    अञ्ञथा
	ref->pll_next->pll_prev = ref->pll_prev;
}

    स्थिर शून्य
pyll_add(PyObject *self, pylinkedlist_T *ref, pylinkedlist_T **last)
{
    सचे (*last == रिक्त)
	ref->pll_prev = रिक्त;
    अञ्ञथा
    {
	(*last)->pll_next = ref;
	ref->pll_prev = *last;
    }
    ref->pll_next = रिक्त;
    ref->pll_obj = self;
    *last = ref;
}

DEFINE_PY_TYPE_OBJECT(DictionaryType);

प्रकारनिर्देश संरचना
{
    PyObject_HEAD
    dict_T	*dict;
    pylinkedlist_T	ref;
} DictionaryObject;

स्थिर PyObject *DictionaryUpdate(DictionaryObject *, PyObject *, PyObject *);

#define NEW_DICTIONARY(dict) DictionaryNew(DictionaryTypePtr, dict)

    स्थिर PyObject *
DictionaryNew(PyTypeObject *subtype, dict_T *dict)
{
    DictionaryObject	*self;

    self = (DictionaryObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    सचे (self == रिक्त)
	पटिनिवत्त रिक्त;
    self->dict = dict;
    ++dict->dv_refcount;

    pyll_add((PyObject *)(self), &self->ref, &lastdict);

    पटिनिवत्त (PyObject *)(self);
}

    स्थिर dict_T *
py_dict_alloc(शून्य)
{
    dict_T	*ret;

    सचे (!(ret = dict_alloc()))
    {
	PyErr_NoMemory();
	पटिनिवत्त रिक्त;
    }
    ++ret->dv_refcount;

    पटिनिवत्त ret;
}

    स्थिर PyObject *
DictionaryConstructor(PyTypeObject *subtype, PyObject *args, PyObject *kwargs)
{
    DictionaryObject	*self;
    dict_T	*dict;

    सचे (!(dict = py_dict_alloc()))
	पटिनिवत्त रिक्त;

    self = (DictionaryObject *) DictionaryNew(subtype, dict);

    --dict->dv_refcount;

    सचे (kwargs || PyTuple_Size(args))
    {
	PyObject	*tmp;
	सचे (!(tmp = DictionaryUpdate(self, args, kwargs)))
	{
	    Py_DECREF(self);
	    पटिनिवत्त रिक्त;
	}

	Py_DECREF(tmp);
    }

    पटिनिवत्त (PyObject *)(self);
}

    स्थिर शून्य
DictionaryDestructor(PyObject *self_obj)
{
    DictionaryObject *self = (DictionaryObject*)self_obj;
    pyll_remove(&self->ref, &lastdict);
    dict_unref(self->dict);

    DESTRUCTOR_FINISH(self);
}

स्थिर वर्ण *DictionaryAttrs[] = {
    "locked", "scope",
    रिक्त
};

    स्थिर PyObject *
DictionaryDir(PyObject *self, PyObject *args UNUSED)
{
    पटिनिवत्त ObjectDir(self, DictionaryAttrs);
}

    स्थिर पूर्णाङ्क
DictionarySetattr(PyObject *self_obj, वर्ण *name, PyObject *valObject)
{
    DictionaryObject *self = (DictionaryObject*)self_obj;
    सचे (valObject == रिक्त)
    {
	PyErr_SET_STRING(PyExc_AttributeError,
		N_("cannot delete vim.Dictionary attributes"));
	पटिनिवत्त -1;
    }

    सचे (strcmp(name, "locked") == 0)
    {
	सचे (self->dict->dv_lock == VAR_FIXED)
	{
	    PyErr_SET_STRING(PyExc_TypeError,
		    N_("cannot modify fixed dictionary"));
	    पटिनिवत्त -1;
	}
	अञ्ञथा
	{
	    पूर्णाङ्क		istrue = PyObject_IsTrue(valObject);
	    सचे (istrue == -1)
		पटिनिवत्त -1;
	    अञ्ञथा सचे (istrue)
		self->dict->dv_lock = VAR_LOCKED;
	    अञ्ञथा
		self->dict->dv_lock = 0;
	}
	पटिनिवत्त 0;
    }
    अञ्ञथा
    {
	PyErr_FORMAT(PyExc_AttributeError, N_("cannot set attribute %s"), name);
	पटिनिवत्त -1;
    }
}

    स्थिर PyInt
DictionaryLength(DictionaryObject *self)
{
    पटिनिवत्त ((PyInt) (self->dict->dv_hashtab.ht_used));
}

#define DICT_FLAG_HAS_DEFAULT	0x01
#define DICT_FLAG_निष्कासय		0x02
#define DICT_FLAG_NONE_DEFAULT	0x04
#define DICT_FLAG_RETURN_BOOL	0x08 // Incompatible with DICT_FLAG_POP
#define DICT_FLAG_RETURN_PAIR	0x10

    स्थिर PyObject *
_DictionaryItem(DictionaryObject *self, PyObject *args, पूर्णाङ्क flags)
{
    PyObject	*कुञ्चिका_Object;
    PyObject	*defObject = ((flags & DICT_FLAG_NONE_DEFAULT)? Py_None : रिक्त);
    PyObject	*ret;
    char_u	*कुञ्चिका;
    dictitem_T	*di;
    dict_T	*dict = self->dict;
    hashitem_T	*hi;
    PyObject	*todecref;

    सचे (flags & DICT_FLAG_HAS_DEFAULT)
    {
	सचे (!PyArg_ParseTuple(args, "O|O", &कुञ्चिका_Object, &defObject))
	    पटिनिवत्त रिक्त;
    }
    अञ्ञथा
	कुञ्चिका_Object = args;

    सचे (flags & DICT_FLAG_RETURN_BOOL)
	defObject = Py_False;

    सचे (!(कुञ्चिका = StringToChars(कुञ्चिका_Object, &todecref)))
	पटिनिवत्त रिक्त;

    सचे (*कुञ्चिका == NUL)
    {
	RAISE_NO_EMPTY_कुञ्चिकाः;
	Py_XDECREF(todecref);
	पटिनिवत्त रिक्त;
    }

    hi = hash_find(&dict->dv_hashtab, कुञ्चिका);

    Py_XDECREF(todecref);

    सचे (HASHITEM_EMPTY(hi))
    {
	सचे (defObject)
	{
	    Py_INCREF(defObject);
	    पटिनिवत्त defObject;
	}
	अञ्ञथा
	{
	    PyErr_SetObject(Py_Exc_कुञ्चिका_Error, कुञ्चिका_Object);
	    पटिनिवत्त रिक्त;
	}
    }
    अञ्ञथा सचे (flags & DICT_FLAG_RETURN_BOOL)
    {
	ret = Py_True;
	Py_INCREF(ret);
	पटिनिवत्त ret;
    }

    di = dict_lookup(hi);

    सचे (!(ret = ConvertToPyObject(&di->di_tv)))
	पटिनिवत्त रिक्त;

    सचे (flags & DICT_FLAG_निष्कासय)
    {
	सचे (dict->dv_lock)
	{
	    RAISE_LOCKED_DICTIONARY;
	    Py_DECREF(ret);
	    पटिनिवत्त रिक्त;
	}

	hash_remove(&dict->dv_hashtab, hi, "Python remove variable");
	dictitem_free(di);
    }

    पटिनिवत्त ret;
}

    स्थिर PyObject *
DictionaryItem(DictionaryObject *self, PyObject *कुञ्चिका_Object)
{
    पटिनिवत्त _DictionaryItem(self, कुञ्चिका_Object, 0);
}

    स्थिर पूर्णाङ्क
DictionaryContains(DictionaryObject *self, PyObject *कुञ्चिका_Object)
{
    PyObject	*rObj = _DictionaryItem(self, कुञ्चिका_Object, DICT_FLAG_RETURN_BOOL);
    पूर्णाङ्क		ret;

    सचे (rObj == रिक्त)
	पटिनिवत्त -1;

    ret = (rObj == Py_True);

    Py_DECREF(rObj);

    पटिनिवत्त ret;
}

प्रकारनिर्देश संरचना
{
    पूर्णाङ्क		dii_changed;
    hashtab_T	*dii_ht;
    hashitem_T	*dii_hi;
    long_u	dii_todo;
} dictiterinfo_T;

    स्थिर PyObject *
DictionaryIterNext(शून्य **arg)
{
    PyObject	*ret;
    dictiterinfo_T **dii = (dictiterinfo_T**)arg;

    सचे (!(*dii)->dii_todo)
	पटिनिवत्त रिक्त;

    सचे ((*dii)->dii_ht->ht_changed != (*dii)->dii_changed)
    {
	PyErr_SET_STRING(PyExc_RuntimeError,
		N_("hashtab changed during iteration"));
	पटिनिवत्त रिक्त;
    }

    याव (((*dii)->dii_todo) && HASHITEM_EMPTY((*dii)->dii_hi))
	++((*dii)->dii_hi);

    --((*dii)->dii_todo);

    सचे (!(ret = PyBytes_FromString((वर्ण *)(*dii)->dii_hi->hi_कुञ्चिका)))
	पटिनिवत्त रिक्त;

    पटिनिवत्त ret;
}

    स्थिर PyObject *
DictionaryIter(PyObject *self_obj)
{
    DictionaryObject	*self = (DictionaryObject*)self_obj;
    dictiterinfo_T	*dii;
    hashtab_T		*ht;

    सचे (!(dii = PyMem_New(dictiterinfo_T, 1)))
    {
	PyErr_NoMemory();
	पटिनिवत्त रिक्त;
    }

    ht = &self->dict->dv_hashtab;
    dii->dii_changed = ht->ht_changed;
    dii->dii_ht = ht;
    dii->dii_hi = ht->ht_array;
    dii->dii_todo = ht->ht_used;

    पटिनिवत्त IterNew(dii,
	    PyMem_Free, DictionaryIterNext,
	    रिक्त, रिक्त, (PyObject *)self);
}

    स्थिर पूर्णाङ्क
DictionaryAssItem(
	DictionaryObject *self, PyObject *कुञ्चिका_Object, PyObject *valObject)
{
    char_u	*कुञ्चिका;
    typval_T	tv;
    dict_T	*dict = self->dict;
    dictitem_T	*di;
    PyObject	*todecref;

    सचे (dict->dv_lock)
    {
	RAISE_LOCKED_DICTIONARY;
	पटिनिवत्त -1;
    }

    सचे (!(कुञ्चिका = StringToChars(कुञ्चिका_Object, &todecref)))
	पटिनिवत्त -1;

    सचे (*कुञ्चिका == NUL)
    {
	RAISE_NO_EMPTY_कुञ्चिकाः;
	Py_XDECREF(todecref);
	पटिनिवत्त -1;
    }

    di = dict_find(dict, कुञ्चिका, -1);

    सचे (valObject == रिक्त)
    {
	hashitem_T	*hi;

	सचे (di == रिक्त)
	{
	    Py_XDECREF(todecref);
	    PyErr_SetObject(Py_Exc_कुञ्चिका_Error, कुञ्चिका_Object);
	    पटिनिवत्त -1;
	}
	hi = hash_find(&dict->dv_hashtab, di->di_कुञ्चिका);
	hash_remove(&dict->dv_hashtab, hi, "Python remove item");
	dictitem_free(di);
	Py_XDECREF(todecref);
	पटिनिवत्त 0;
    }

    सचे (ConvertFromPyObject(valObject, &tv) == -1)
    {
	Py_XDECREF(todecref);
	पटिनिवत्त -1;
    }

    सचे (di == रिक्त)
    {
	सचे (!(di = dictitem_alloc(कुञ्चिका)))
	{
	    Py_XDECREF(todecref);
	    PyErr_NoMemory();
	    पटिनिवत्त -1;
	}
	di->di_tv.v_type = VAR_UNKNOWN;

	सचे (dict_add(dict, di) == FAIL)
	{
	    dictitem_free(di);
	    RAISE_कुञ्चिका_ADD_FAIL(कुञ्चिका);
	    Py_XDECREF(todecref);
	    पटिनिवत्त -1;
	}
    }
    अञ्ञथा
	clear_tv(&di->di_tv);

    Py_XDECREF(todecref);

    copy_tv(&tv, &di->di_tv);
    clear_tv(&tv);
    पटिनिवत्त 0;
}

प्रकारनिर्देश PyObject *(*hi_to_py)(hashitem_T *);

    स्थिर PyObject *
DictionaryListObjects(DictionaryObject *self, hi_to_py hiconvert)
{
    dict_T	*dict = self->dict;
    long_u	todo = dict->dv_hashtab.ht_used;
    Py_ssize_t	i = 0;
    PyObject	*ret;
    hashitem_T	*hi;
    PyObject	*newObj;

    ret = PyList_New(todo);
    अत्थाय (hi = dict->dv_hashtab.ht_array; todo > 0; ++hi)
    {
	सचे (!HASHITEM_EMPTY(hi))
	{
	    सचे (!(newObj = hiconvert(hi)))
	    {
		Py_DECREF(ret);
		पटिनिवत्त रिक्त;
	    }
	    PyList_SET_ITEM(ret, i, newObj);
	    --todo;
	    ++i;
	}
    }
    पटिनिवत्त ret;
}

    स्थिर PyObject *
dict_कुञ्चिका(hashitem_T *hi)
{
    पटिनिवत्त PyBytes_FromString((वर्ण *)(hi->hi_कुञ्चिका));
}

    स्थिर PyObject *
Dictionary_List_कुञ्चिकाः(DictionaryObject *self, PyObject *args UNUSED)
{
    पटिनिवत्त DictionaryListObjects(self, dict_कुञ्चिका);
}

    स्थिर PyObject *
dict_val(hashitem_T *hi)
{
    dictitem_T	*di;

    di = dict_lookup(hi);
    पटिनिवत्त ConvertToPyObject(&di->di_tv);
}

    स्थिर PyObject *
Dictionary_List_मानानि(DictionaryObject *self, PyObject *args UNUSED)
{
    पटिनिवत्त DictionaryListObjects(self, dict_val);
}

    स्थिर PyObject *
dict_item(hashitem_T *hi)
{
    PyObject	*कुञ्चिका_Object;
    PyObject	*valObject;
    PyObject	*ret;

    सचे (!(कुञ्चिका_Object = dict_कुञ्चिका(hi)))
	पटिनिवत्त रिक्त;

    सचे (!(valObject = dict_val(hi)))
    {
	Py_DECREF(कुञ्चिका_Object);
	पटिनिवत्त रिक्त;
    }

    ret = Py_Build_मानम्("(OO)", कुञ्चिका_Object, valObject);

    Py_DECREF(कुञ्चिका_Object);
    Py_DECREF(valObject);

    पटिनिवत्त ret;
}

    स्थिर PyObject *
Dictionary_List_वस्तूनि(DictionaryObject *self, PyObject *args UNUSED)
{
    पटिनिवत्त DictionaryListObjects(self, dict_item);
}

    स्थिर PyObject *
DictionaryUpdate(DictionaryObject *self, PyObject *args, PyObject *kwargs)
{
    dict_T	*dict = self->dict;

    सचे (dict->dv_lock)
    {
	RAISE_LOCKED_DICTIONARY;
	पटिनिवत्त रिक्त;
    }

    सचे (kwargs)
    {
	typval_T	tv;

	सचे (ConvertFromPyMapping(kwargs, &tv) == -1)
	    पटिनिवत्त रिक्त;

	VimTryStart();
	dict_extend(self->dict, tv.vval.v_dict, (char_u *) "force", रिक्त);
	clear_tv(&tv);
	सचे (VimTryEnd())
	    पटिनिवत्त रिक्त;
    }
    अञ्ञथा
    {
	PyObject	*obj = रिक्त;

	सचे (!PyArg_ParseTuple(args, "|O", &obj))
	    पटिनिवत्त रिक्त;

	सचे (obj == रिक्त)
	{
	    Py_INCREF(Py_None);
	    पटिनिवत्त Py_None;
	}

	सचे (PyObject_HasAttrString(obj, "keys"))
	    पटिनिवत्त DictionaryUpdate(self, रिक्त, obj);
	अञ्ञथा
	{
	    PyObject	*iterator;
	    PyObject	*item;

	    सचे (!(iterator = PyObject_GetIter(obj)))
		पटिनिवत्त रिक्त;

	    याव ((item = PyIter_Next(iterator)))
	    {
		PyObject	*fast;
		PyObject	*कुञ्चिका_Object;
		PyObject	*valObject;
		PyObject	*todecref;
		char_u		*कुञ्चिका;
		dictitem_T	*di;
		hashitem_T	*hi;

		सचे (!(fast = PySequence_Fast(item, "")))
		{
		    Py_DECREF(iterator);
		    Py_DECREF(item);
		    पटिनिवत्त रिक्त;
		}

		Py_DECREF(item);

		सचे (PySequence_Size(fast) != 2)
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    PyErr_FORMAT(Py_Exc_मानम्_Error,
			    N_("expected sequence element of size 2, "
			    "but got sequence of size %d"),
			    (पूर्णाङ्क) PySequence_Size(fast));
		    पटिनिवत्त रिक्त;
		}

		कुञ्चिका_Object = PySequence_GetItem(fast, 0);

		सचे (!(कुञ्चिका = StringToChars(कुञ्चिका_Object, &todecref)))
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    पटिनिवत्त रिक्त;
		}

		di = dictitem_alloc(कुञ्चिका);

		Py_XDECREF(todecref);

		सचे (di == रिक्त)
		{
		    Py_DECREF(fast);
		    Py_DECREF(iterator);
		    PyErr_NoMemory();
		    पटिनिवत्त रिक्त;
		}
		di->di_tv.v_type = VAR_UNKNOWN;

		valObject = PySequence_GetItem(fast, 1);

		सचे (ConvertFromPyObject(valObject, &di->di_tv) == -1)
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    dictitem_free(di);
		    पटिनिवत्त रिक्त;
		}

		Py_DECREF(fast);

		hi = hash_find(&dict->dv_hashtab, di->di_कुञ्चिका);
		सचे (!HASHITEM_EMPTY(hi) || dict_add(dict, di) == FAIL)
		{
		    RAISE_कुञ्चिका_ADD_FAIL(di->di_कुञ्चिका);
		    Py_DECREF(iterator);
		    dictitem_free(di);
		    पटिनिवत्त रिक्त;
		}
	    }

	    Py_DECREF(iterator);

	    // Iterator may have finished due to an exception
	    सचे (PyErr_Occurred())
		पटिनिवत्त रिक्त;
	}
    }
    Py_INCREF(Py_None);
    पटिनिवत्त Py_None;
}

    स्थिर PyObject *
DictionaryGet(DictionaryObject *self, PyObject *args)
{
    पटिनिवत्त _DictionaryItem(self, args,
			    DICT_FLAG_HAS_DEFAULT|DICT_FLAG_NONE_DEFAULT);
}

    स्थिर PyObject *
Dictionary_निष्कासय(DictionaryObject *self, PyObject *args)
{
    पटिनिवत्त _DictionaryItem(self, args, DICT_FLAG_HAS_DEFAULT|DICT_FLAG_निष्कासय);
}

    स्थिर PyObject *
Dictionary_निष्कासय_Item(DictionaryObject *self, PyObject *args UNUSED)
{
    hashitem_T	*hi;
    PyObject	*ret;
    PyObject	*valObject;
    dictitem_T	*di;

    सचे (self->dict->dv_hashtab.ht_used == 0)
    {
	PyErr_SetNone(Py_Exc_कुञ्चिका_Error);
	पटिनिवत्त रिक्त;
    }

    hi = self->dict->dv_hashtab.ht_array;
    याव (HASHITEM_EMPTY(hi))
	++hi;

    di = dict_lookup(hi);

    सचे (!(valObject = ConvertToPyObject(&di->di_tv)))
	पटिनिवत्त रिक्त;

    सचे (!(ret = Py_Build_मानम्("(" Py_bytes_fmt "O)", hi->hi_कुञ्चिका, valObject)))
    {
	Py_DECREF(valObject);
	पटिनिवत्त रिक्त;
    }

    hash_remove(&self->dict->dv_hashtab, hi, "Python pop item");
    dictitem_free(di);

    पटिनिवत्त ret;
}

    स्थिर PyObject *
Dictionary_Has_कुञ्चिका(DictionaryObject *self, PyObject *कुञ्चिका_Object)
{
    पटिनिवत्त _DictionaryItem(self, कुञ्चिका_Object, DICT_FLAG_RETURN_BOOL);
}

स्थिर PySequenceMethods DictionaryAsSeq = {
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

स्थिर PyMappingMethods DictionaryAsMapping = {
    (lenfunc)       DictionaryLength,
    (binaryfunc)    DictionaryItem,
    (objobjargproc) DictionaryAssItem,
};

स्थिर संरचना PyMethodDef DictionaryMethods[] = {
    {"keys",	(PyCFunction)Dictionary_List_कुञ्चिकाः,	METH_NOARGS,	""},
    {"values",	(PyCFunction)Dictionary_List_मानानि,	METH_NOARGS,	""},
    {"items",	(PyCFunction)Dictionary_List_वस्तूनि,	METH_NOARGS,	""},
    {"update",	(PyCFunction)(शून्य *)DictionaryUpdate,		METH_VARARGS|METH_KEYWORDS, ""},
    {"get",	(PyCFunction)DictionaryGet,		METH_VARARGS,	""},
    {"pop",	(PyCFunction)Dictionary_निष्कासय,		METH_VARARGS,	""},
    {"popitem",	(PyCFunction)Dictionary_निष्कासय_Item,		METH_NOARGS,	""},
    {"has_key",	(PyCFunction)Dictionary_Has_कुञ्चिका,		METH_O,		""},
    {"__dir__",	(PyCFunction)DictionaryDir,		METH_NOARGS,	""},
    { रिक्त,	रिक्त,					0,		रिक्त}
};

DEFINE_PY_TYPE_OBJECT(ListType);

प्रकारनिर्देश संरचना
{
    PyObject_HEAD
    list_T	*list;
    pylinkedlist_T	ref;
} ListObject;

#define NEW_LIST(list) ListNew(ListTypePtr, list)

    स्थिर PyObject *
ListNew(PyTypeObject *subtype, list_T *list)
{
    ListObject	*self;

    सचे (list == रिक्त)
	पटिनिवत्त रिक्त;

    self = (ListObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    सचे (self == रिक्त)
	पटिनिवत्त रिक्त;
    self->list = list;
    ++list->lv_refcount;
    CHECK_LIST_MATERIALIZE(list);

    pyll_add((PyObject *)(self), &self->ref, &lastlist);

    पटिनिवत्त (PyObject *)(self);
}

    स्थिर list_T *
py_list_alloc(शून्य)
{
    list_T	*ret;

    सचे (!(ret = list_alloc()))
    {
	PyErr_NoMemory();
	पटिनिवत्त रिक्त;
    }
    ++ret->lv_refcount;

    पटिनिवत्त ret;
}

    स्थिर पूर्णाङ्क
list_py_concat(list_T *l, PyObject *obj, PyObject *lookup_dict)
{
    PyObject	*iterator;
    PyObject	*item;
    listitem_T	*li;

    सचे (!(iterator = PyObject_GetIter(obj)))
	पटिनिवत्त -1;

    याव ((item = PyIter_Next(iterator)))
    {
	सचे (!(li = listitem_alloc()))
	{
	    PyErr_NoMemory();
	    Py_DECREF(item);
	    Py_DECREF(iterator);
	    पटिनिवत्त -1;
	}
	li->li_tv.v_lock = 0;
	li->li_tv.v_type = VAR_UNKNOWN;

	सचे (_ConvertFromPyObject(item, &li->li_tv, lookup_dict) == -1)
	{
	    Py_DECREF(item);
	    Py_DECREF(iterator);
	    listitem_free(l, li);
	    पटिनिवत्त -1;
	}

	Py_DECREF(item);

	list_append(l, li);
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    सचे (PyErr_Occurred())
	पटिनिवत्त -1;

    पटिनिवत्त 0;
}

    स्थिर PyObject *
ListConstructor(PyTypeObject *subtype, PyObject *args, PyObject *kwargs)
{
    list_T	*list;
    PyObject	*obj = रिक्त;

    सचे (kwargs)
    {
	PyErr_SET_STRING(PyExc_TypeError,
		N_("list constructor does not accept keyword arguments"));
	पटिनिवत्त रिक्त;
    }

    सचे (!PyArg_ParseTuple(args, "|O", &obj))
	पटिनिवत्त रिक्त;

    सचे (!(list = py_list_alloc()))
	पटिनिवत्त रिक्त;

    सचे (obj)
    {
	PyObject	*lookup_dict;

	सचे (!(lookup_dict = PyDict_New()))
	{
	    list_unref(list);
	    पटिनिवत्त रिक्त;
	}

	सचे (list_py_concat(list, obj, lookup_dict) == -1)
	{
	    Py_DECREF(lookup_dict);
	    list_unref(list);
	    पटिनिवत्त रिक्त;
	}

	Py_DECREF(lookup_dict);
    }

    पटिनिवत्त ListNew(subtype, list);
}

    स्थिर शून्य
ListDestructor(PyObject *self_obj)
{
    ListObject *self = (ListObject*)self_obj;
    pyll_remove(&self->ref, &lastlist);
    list_unref(self->list);

    DESTRUCTOR_FINISH(self);
}

    स्थिर PyInt
ListLength(ListObject *self)
{
    पटिनिवत्त ((PyInt) (self->list->lv_len));
}

    स्थिर PyObject *
ListIndex(ListObject *self, Py_ssize_t index)
{
    listitem_T	*li;

    सचे (index >= ListLength(self))
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("list index out of range"));
	पटिनिवत्त रिक्त;
    }
    li = list_find(self->list, (long) index);
    सचे (li == रिक्त)
    {
	// No more suitable format specifications in python-2.3
	PyErr_VIM_FORMAT(N_("internal error: failed to get Vim list item %d"),
		(पूर्णाङ्क) index);
	पटिनिवत्त रिक्त;
    }
    पटिनिवत्त ConvertToPyObject(&li->li_tv);
}

    स्थिर PyObject *
ListSlice(ListObject *self, Py_ssize_t first, Py_ssize_t step,
	  Py_ssize_t slicelen)
{
    PyInt	i;
    PyObject	*list;

    सचे (step == 0)
    {
	PyErr_SET_STRING(Py_Exc_मानम्_Error, N_("slice step cannot be zero"));
	पटिनिवत्त रिक्त;
    }

    list = PyList_New(slicelen);
    सचे (list == रिक्त)
	पटिनिवत्त रिक्त;

    अत्थाय (i = 0; i < slicelen; ++i)
    {
	PyObject	*item;

	item = ListIndex(self, first + i*step);
	सचे (item == रिक्त)
	{
	    Py_DECREF(list);
	    पटिनिवत्त रिक्त;
	}

	PyList_SET_ITEM(list, i, item);
    }

    पटिनिवत्त list;
}

    स्थिर PyObject *
ListItem(ListObject *self, PyObject* idx)
{
#सचे PY_MAJOR_VERSION < 3
    सचे (PyInt_Check(idx))
    {
	long _idx = PyInt_AsLong(idx);
	पटिनिवत्त ListIndex(self, _idx);
    }
    अञ्ञथा
#endif
    सचे (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	पटिनिवत्त ListIndex(self, _idx);
    }
    अञ्ञथा सचे (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	सचे (PySlice_GetIndicesEx((PySliceObject_T *)idx, ListLength(self),
				 &start, &stop, &step, &slicelen) < 0)
	    पटिनिवत्त रिक्त;
	पटिनिवत्त ListSlice(self, start, step, slicelen);
    }
    अञ्ञथा
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	पटिनिवत्त रिक्त;
    }
}

    स्थिर शून्य
list_restore(Py_ssize_t numadded, Py_ssize_t numreplaced, Py_ssize_t slicelen,
	list_T *l, listitem_T **lis, listitem_T *lastaddedli)
{
    याव (numreplaced--)
    {
	list_निवेशय(l, lis[numreplaced], lis[slicelen + numreplaced]);
	listitem_remove(l, lis[slicelen + numreplaced]);
    }
    याव (numadded--)
    {
	listitem_T	*next;

	next = lastaddedli->li_prev;
	listitem_remove(l, lastaddedli);
	lastaddedli = next;
    }
}

    स्थिर पूर्णाङ्क
ListAssSlice(ListObject *self, Py_ssize_t first,
	     Py_ssize_t step, Py_ssize_t slicelen, PyObject *obj)
{
    PyObject	*iterator;
    PyObject	*item;
    listitem_T	*li;
    listitem_T	*lastaddedli = रिक्त;
    listitem_T	*next;
    typval_T	v;
    list_T	*l = self->list;
    PyInt	i;
    PyInt	j;
    PyInt	numreplaced = 0;
    PyInt	numadded = 0;
    PyInt	size;
    listitem_T	**lis = रिक्त;

    size = ListLength(self);

    सचे (l->lv_lock)
    {
	RAISE_LOCKED_LIST;
	पटिनिवत्त -1;
    }

    सचे (step == 0)
    {
	PyErr_SET_STRING(Py_Exc_मानम्_Error, N_("slice step cannot be zero"));
	पटिनिवत्त -1;
    }

    सचे (step != 1 && slicelen == 0)
    {
	// Nothing to do. Only error out if obj has some items.
	पूर्णाङ्क		ret = 0;

	सचे (obj == रिक्त)
	    पटिनिवत्त 0;

	सचे (!(iterator = PyObject_GetIter(obj)))
	    पटिनिवत्त -1;

	सचे ((item = PyIter_Next(iterator)))
	{
	    PyErr_FORMAT(Py_Exc_मानम्_Error,
		    N_("attempt to assign sequence of size greater than %d "
			"to extended slice"), 0);
	    Py_DECREF(item);
	    ret = -1;
	}
	Py_DECREF(iterator);
	पटिनिवत्त ret;
    }

    सचे (obj != रिक्त)
	// XXX May allocate zero bytes.
	सचे (!(lis = PyMem_New(listitem_T *, slicelen * 2)))
	{
	    PyErr_NoMemory();
	    पटिनिवत्त -1;
	}

    सचे (first == size)
	li = रिक्त;
    अञ्ञथा
    {
	li = list_find(l, (long) first);
	सचे (li == रिक्त)
	{
	    PyErr_VIM_FORMAT(N_("internal error: no Vim list item %d"),
		    (पूर्णाङ्क)first);
	    सचे (obj != रिक्त)
		PyMem_Free(lis);
	    पटिनिवत्त -1;
	}
	i = slicelen;
	याव (i-- && li != रिक्त)
	{
	    j = step;
	    next = li;
	    सचे (step > 0)
		याव (next != रिक्त && ((next = next->li_next) != रिक्त) && --j);
	    अञ्ञथा
		याव (next != रिक्त && ((next = next->li_prev) != रिक्त) && ++j);

	    सचे (obj == रिक्त)
		listitem_remove(l, li);
	    अञ्ञथा
		lis[slicelen - i - 1] = li;

	    li = next;
	}
	सचे (li == रिक्त && i != -1)
	{
	    PyErr_SET_VIM(N_("internal error: not enough list items"));
	    सचे (obj != रिक्त)
		PyMem_Free(lis);
	    पटिनिवत्त -1;
	}
    }

    सचे (obj == रिक्त)
	पटिनिवत्त 0;

    सचे (!(iterator = PyObject_GetIter(obj)))
    {
	PyMem_Free(lis);
	पटिनिवत्त -1;
    }

    i = 0;
    याव ((item = PyIter_Next(iterator)))
    {
	सचे (ConvertFromPyObject(item, &v) == -1)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(item);
	    PyMem_Free(lis);
	    पटिनिवत्त -1;
	}
	Py_DECREF(item);
	सचे (list_निवेशय_tv(l, &v, numreplaced < slicelen
				    ? lis[numreplaced]
				    : li) == FAIL)
	{
	    clear_tv(&v);
	    PyErr_SET_VIM(N_("internal error: failed to add item to list"));
	    list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	    PyMem_Free(lis);
	    पटिनिवत्त -1;
	}
	सचे (numreplaced < slicelen)
	{
	    lis[slicelen + numreplaced] = lis[numreplaced]->li_prev;
	    vimlist_remove(l, lis[numreplaced], lis[numreplaced]);
	    numreplaced++;
	}
	अञ्ञथा
	{
	    सचे (li)
		lastaddedli = li->li_prev;
	    अञ्ञथा
		lastaddedli = l->lv_u.mat.lv_last;
	    numadded++;
	}
	clear_tv(&v);
	सचे (step != 1 && i >= slicelen)
	{
	    Py_DECREF(iterator);
	    PyErr_FORMAT(Py_Exc_मानम्_Error,
		    N_("attempt to assign sequence of size greater than %d "
			"to extended slice"), (पूर्णाङ्क) slicelen);
	    list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	    PyMem_Free(lis);
	    पटिनिवत्त -1;
	}
	++i;
    }
    Py_DECREF(iterator);

    सचे (step != 1 && i != slicelen)
    {
	PyErr_FORMAT2(Py_Exc_मानम्_Error,
		N_("attempt to assign sequence of size %d to extended slice "
		    "of size %d"), (पूर्णाङ्क) i, (पूर्णाङ्क) slicelen);
	list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	PyMem_Free(lis);
	पटिनिवत्त -1;
    }

    सचे (PyErr_Occurred())
    {
	list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	PyMem_Free(lis);
	पटिनिवत्त -1;
    }

    अत्थाय (i = 0; i < numreplaced; i++)
	listitem_free(l, lis[i]);
    सचे (step == 1)
	अत्थाय (i = numreplaced; i < slicelen; i++)
	    listitem_remove(l, lis[i]);

    PyMem_Free(lis);

    पटिनिवत्त 0;
}

    स्थिर पूर्णाङ्क
ListAssIndex(ListObject *self, Py_ssize_t index, PyObject *obj)
{
    typval_T	tv;
    list_T	*l = self->list;
    listitem_T	*li;
    Py_ssize_t	length = ListLength(self);

    सचे (l->lv_lock)
    {
	RAISE_LOCKED_LIST;
	पटिनिवत्त -1;
    }
    सचे (index > length || (index == length && obj == रिक्त))
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("list index out of range"));
	पटिनिवत्त -1;
    }

    सचे (obj == रिक्त)
    {
	li = list_find(l, (long) index);
	सचे (li == रिक्त)
	{
	    PyErr_VIM_FORMAT(N_("internal error: failed to get Vim "
			"list item %d"), (पूर्णाङ्क) index);
	    पटिनिवत्त -1;
	}
	vimlist_remove(l, li, li);
	clear_tv(&li->li_tv);
	vim_free(li);
	पटिनिवत्त 0;
    }

    सचे (ConvertFromPyObject(obj, &tv) == -1)
	पटिनिवत्त -1;

    सचे (index == length)
    {
	सचे (list_append_tv(l, &tv) == FAIL)
	{
	    clear_tv(&tv);
	    PyErr_SET_VIM(N_("failed to add item to list"));
	    पटिनिवत्त -1;
	}
    }
    अञ्ञथा
    {
	li = list_find(l, (long) index);
	सचे (li == रिक्त)
	{
	    PyErr_VIM_FORMAT(N_("internal error: failed to get Vim "
			"list item %d"), (पूर्णाङ्क) index);
	    पटिनिवत्त -1;
	}
	clear_tv(&li->li_tv);
	copy_tv(&tv, &li->li_tv);
	clear_tv(&tv);
    }
    पटिनिवत्त 0;
}

    स्थिर पूर्णाङ्क
ListAssItem(ListObject *self, PyObject *idx, PyObject *obj)
{
#सचे PY_MAJOR_VERSION < 3
    सचे (PyInt_Check(idx))
    {
	long _idx = PyInt_AsLong(idx);
	पटिनिवत्त (पूर्णाङ्क)ListAssIndex(self, _idx, obj);
    }
    अञ्ञथा
#endif
    सचे (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	पटिनिवत्त (पूर्णाङ्क)ListAssIndex(self, _idx, obj);
    }
    अञ्ञथा सचे (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	सचे (PySlice_GetIndicesEx((PySliceObject_T *)idx, ListLength(self),
				 &start, &stop, &step, &slicelen) < 0)
	    पटिनिवत्त -1;
	पटिनिवत्त (पूर्णाङ्क)ListAssSlice(self, start, step, slicelen,
		obj);
    }
    अञ्ञथा
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	पटिनिवत्त -1;
    }
}

    स्थिर PyObject *
ListConcatInPlace(ListObject *self, PyObject *obj)
{
    list_T	*l = self->list;
    PyObject	*lookup_dict;

    सचे (l->lv_lock)
    {
	RAISE_LOCKED_LIST;
	पटिनिवत्त रिक्त;
    }

    सचे (!(lookup_dict = PyDict_New()))
	पटिनिवत्त रिक्त;

    सचे (list_py_concat(l, obj, lookup_dict) == -1)
    {
	Py_DECREF(lookup_dict);
	पटिनिवत्त रिक्त;
    }
    Py_DECREF(lookup_dict);

    Py_INCREF((PyObject *)self);
    पटिनिवत्त (PyObject *)(self);
}

प्रकारनिर्देश संरचना
{
    listwatch_T	lw;
    list_T	*list;
} listiterinfo_T;

    स्थिर शून्य
ListIterDestruct(शून्य *arg)
{
    listiterinfo_T *lii = (listiterinfo_T*)arg;
    list_rem_watch(lii->list, &lii->lw);
    list_unref(lii->list);
    PyMem_Free(lii);
}

    स्थिर PyObject *
ListIterNext(शून्य **arg)
{
    PyObject	*ret;
    listiterinfo_T **lii = (listiterinfo_T**)arg;

    सचे (!((*lii)->lw.lw_item))
	पटिनिवत्त रिक्त;

    सचे (!(ret = ConvertToPyObject(&((*lii)->lw.lw_item->li_tv))))
	पटिनिवत्त रिक्त;

    (*lii)->lw.lw_item = (*lii)->lw.lw_item->li_next;

    पटिनिवत्त ret;
}

    स्थिर PyObject *
ListIter(PyObject *self_obj)
{
    ListObject	*self = (ListObject*)self_obj;
    listiterinfo_T	*lii;
    list_T	*l = self->list;

    सचे (!(lii = PyMem_New(listiterinfo_T, 1)))
    {
	PyErr_NoMemory();
	पटिनिवत्त रिक्त;
    }

    CHECK_LIST_MATERIALIZE(l);
    list_add_watch(l, &lii->lw);
    lii->lw.lw_item = l->lv_first;
    lii->list = l;
    ++l->lv_refcount;

    पटिनिवत्त IterNew(lii,
	    ListIterDestruct, ListIterNext,
	    रिक्त, रिक्त, (PyObject *)self);
}

स्थिर वर्ण *ListAttrs[] = {
    "locked",
    रिक्त
};

    स्थिर PyObject *
ListDir(PyObject *self, PyObject *args UNUSED)
{
    पटिनिवत्त ObjectDir(self, ListAttrs);
}

    स्थिर पूर्णाङ्क
ListSetattr(PyObject *self_obj, वर्ण *name, PyObject *valObject)
{
    ListObject *self = (ListObject*)self_obj;
    सचे (valObject == रिक्त)
    {
	PyErr_SET_STRING(PyExc_AttributeError,
		N_("cannot delete vim.List attributes"));
	पटिनिवत्त -1;
    }

    सचे (strcmp(name, "locked") == 0)
    {
	सचे (self->list->lv_lock == VAR_FIXED)
	{
	    PyErr_SET_STRING(PyExc_TypeError, N_("cannot modify fixed list"));
	    पटिनिवत्त -1;
	}
	अञ्ञथा
	{
	    पूर्णाङ्क		istrue = PyObject_IsTrue(valObject);
	    सचे (istrue == -1)
		पटिनिवत्त -1;
	    अञ्ञथा सचे (istrue)
		self->list->lv_lock = VAR_LOCKED;
	    अञ्ञथा
		self->list->lv_lock = 0;
	}
	पटिनिवत्त 0;
    }
    अञ्ञथा
    {
	PyErr_FORMAT(PyExc_AttributeError, N_("cannot set attribute %s"), name);
	पटिनिवत्त -1;
    }
}

स्थिर PySequenceMethods ListAsSeq = {
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

स्थिर PyMappingMethods ListAsMapping = {
    /* mp_length	*/ (lenfunc) ListLength,
    /* mp_subscript     */ (binaryfunc) ListItem,
    /* mp_ass_subscript */ (objobjargproc) ListAssItem,
};

स्थिर संरचना PyMethodDef ListMethods[] = {
    {"extend",	(PyCFunction)ListConcatInPlace,	METH_O,		""},
    {"__dir__",	(PyCFunction)ListDir,		METH_NOARGS,	""},
    { रिक्त,	रिक्त,				0,		रिक्त}
};

DEFINE_PY_TYPE_OBJECT(TupleType);

प्रकारनिर्देश संरचना
{
    PyObject_HEAD
    tuple_T	*tuple;
    pylinkedlist_T	ref;
} TupleObject;

#define NEW_TUPLE(tuple) TupleNew(TupleTypePtr, tuple)

    स्थिर PyObject *
TupleNew(PyTypeObject *subtype, tuple_T *tuple)
{
    TupleObject	*self;

    सचे (tuple == रिक्त)
	पटिनिवत्त रिक्त;

    self = (TupleObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    सचे (self == रिक्त)
	पटिनिवत्त रिक्त;
    self->tuple = tuple;
    ++tuple->tv_refcount;

    pyll_add((PyObject *)(self), &self->ref, &lasttuple);

    पटिनिवत्त (PyObject *)(self);
}

    स्थिर tuple_T *
py_tuple_alloc(शून्य)
{
    tuple_T	*ret;

    सचे (!(ret = tuple_alloc()))
    {
	PyErr_NoMemory();
	पटिनिवत्त रिक्त;
    }
    ++ret->tv_refcount;

    पटिनिवत्त ret;
}

    स्थिर पूर्णाङ्क
tuple_py_concat(tuple_T *t, PyObject *obj, PyObject *lookup_dict)
{
    PyObject	*iterator;
    PyObject	*item;

    सचे (!(iterator = PyObject_GetIter(obj)))
	पटिनिवत्त -1;

    याव ((item = PyIter_Next(iterator)))
    {
	typval_T    new_tv;

	सचे (_ConvertFromPyObject(item, &new_tv, lookup_dict) == -1)
	{
	    Py_DECREF(item);
	    Py_DECREF(iterator);
	    पटिनिवत्त -1;
	}

	Py_DECREF(item);

	सचे (tuple_append_tv(t, &new_tv) == FAIL)
	{
	    Py_DECREF(iterator);
	    पटिनिवत्त -1;
	}
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    सचे (PyErr_Occurred())
	पटिनिवत्त -1;

    पटिनिवत्त 0;
}

    स्थिर PyObject *
TupleConstructor(PyTypeObject *subtype, PyObject *args, PyObject *kwargs)
{
    tuple_T	*tuple;
    PyObject	*obj = रिक्त;

    सचे (kwargs)
    {
	PyErr_SET_STRING(PyExc_TypeError,
		N_("tuple constructor does not accept keyword arguments"));
	पटिनिवत्त रिक्त;
    }

    सचे (!PyArg_ParseTuple(args, "|O", &obj))
	पटिनिवत्त रिक्त;

    सचे (!(tuple = py_tuple_alloc()))
	पटिनिवत्त रिक्त;

    सचे (obj)
    {
	PyObject	*lookup_dict;

	सचे (!(lookup_dict = PyDict_New()))
	{
	    tuple_unref(tuple);
	    पटिनिवत्त रिक्त;
	}

	सचे (tuple_py_concat(tuple, obj, lookup_dict) == -1)
	{
	    Py_DECREF(lookup_dict);
	    tuple_unref(tuple);
	    पटिनिवत्त रिक्त;
	}

	Py_DECREF(lookup_dict);
    }

    पटिनिवत्त TupleNew(subtype, tuple);
}

    स्थिर शून्य
TupleDestructor(PyObject *self_obj)
{
    TupleObject *self = (TupleObject *)self_obj;
    pyll_remove(&self->ref, &lasttuple);
    tuple_unref(self->tuple);

    DESTRUCTOR_FINISH(self);
}

    स्थिर PyInt
TupleLength(TupleObject *self)
{
    पटिनिवत्त ((PyInt)(tuple_len(self->tuple)));
}

    स्थिर PyObject *
TupleIndex(TupleObject *self, Py_ssize_t index)
{
    PyInt	len = TupleLength(self);

    सचे (index < 0)
	index = len + index;

    सचे (index < 0 || index >= len)
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("tuple index out of range"));
	पटिनिवत्त रिक्त;
    }
    पटिनिवत्त ConvertToPyObject(TUPLE_ITEM(self->tuple, index));
}

/*
 * Return a new tuple object for the tuple slice starting from the index
 * "first" and of length "slicelen" skipping "step" items.
 */
    स्थिर PyObject *
TupleSlice(
    TupleObject		*self,
    Py_ssize_t		first,
    Py_ssize_t		step,
    Py_ssize_t		slicelen)
{
    PyInt	i;
    PyObject	*tuple;

    tuple = PyTuple_New(slicelen);
    सचे (tuple == रिक्त)
	पटिनिवत्त रिक्त;

    अत्थाय (i = 0; i < slicelen; ++i)
    {
	PyObject	*item;

	item = TupleIndex(self, first + i * step);
	सचे (item == रिक्त)
	{
	    Py_DECREF(tuple);
	    पटिनिवत्त रिक्त;
	}

	PyTuple_SetItem(tuple, i, item);
    }

    पटिनिवत्त tuple;
}

    स्थिर PyObject *
TupleItem(TupleObject *self, PyObject* idx)
{
#सचे PY_MAJOR_VERSION < 3
    सचे (PyInt_Check(idx))
    {
	long _idx = PyInt_AsLong(idx);
	पटिनिवत्त TupleIndex(self, _idx);
    }
    अञ्ञथा
#endif
    सचे (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	पटिनिवत्त TupleIndex(self, _idx);
    }
    अञ्ञथा सचे (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	सचे (PySlice_GetIndicesEx((PySliceObject_T *)idx, TupleLength(self),
				 &start, &stop, &step, &slicelen) < 0)
	    पटिनिवत्त रिक्त;
	पटिनिवत्त TupleSlice(self, start, step, slicelen);
    }
    अञ्ञथा
    {
	RAISE_INVALID_INDEX_TYPE(idx);
	पटिनिवत्त रिक्त;
    }
}

प्रकारनिर्देश संरचना
{
    tuple_T	*tuple;
    पूर्णाङ्क		index;
} tupleiterinfo_T;

    स्थिर शून्य
TupleIterDestruct(शून्य *arg)
{
    tupleiterinfo_T *tii = (tupleiterinfo_T*)arg;
    tuple_unref(tii->tuple);
    PyMem_Free(tii);
}

    स्थिर PyObject *
TupleIterNext(शून्य **arg)
{
    PyObject	*ret;
    tupleiterinfo_T **tii = (tupleiterinfo_T**)arg;

    सचे ((*tii)->index >= TUPLE_LEN((*tii)->tuple))
	पटिनिवत्त रिक्त;

    सचे (!(ret = ConvertToPyObject(TUPLE_ITEM((*tii)->tuple, (*tii)->index))))
	पटिनिवत्त रिक्त;

    (*tii)->index++;

    पटिनिवत्त ret;
}

    स्थिर PyObject *
TupleIter(PyObject *self_obj)
{
    TupleObject	*self = (TupleObject*)self_obj;
    tupleiterinfo_T	*tii;
    tuple_T	*t = self->tuple;

    सचे (!(tii = PyMem_New(tupleiterinfo_T, 1)))
    {
	PyErr_NoMemory();
	पटिनिवत्त रिक्त;
    }

    tii->tuple = t;
    tii->index = 0;
    ++t->tv_refcount;

    पटिनिवत्त IterNew(tii,
	    TupleIterDestruct, TupleIterNext,
	    रिक्त, रिक्त, (PyObject *)self);
}

स्थिर वर्ण *TupleAttrs[] = {
    "locked",
    रिक्त
};

    स्थिर PyObject *
TupleDir(PyObject *self, PyObject *args UNUSED)
{
    पटिनिवत्त ObjectDir(self, TupleAttrs);
}

    स्थिर पूर्णाङ्क
TupleSetattr(PyObject *self_obj, वर्ण *name, PyObject *valObject)
{
    TupleObject *self = (TupleObject*)self_obj;
    सचे (valObject == रिक्त)
    {
	PyErr_SET_STRING(PyExc_AttributeError,
		N_("cannot delete vim.Tuple attributes"));
	पटिनिवत्त -1;
    }

    सचे (strcmp(name, "locked") == 0)
    {
	सचे (self->tuple->tv_lock == VAR_FIXED)
	{
	    PyErr_SET_STRING(PyExc_TypeError, N_("cannot modify fixed tuple"));
	    पटिनिवत्त -1;
	}
	अञ्ञथा
	{
	    पूर्णाङ्क		istrue = PyObject_IsTrue(valObject);
	    सचे (istrue == -1)
		पटिनिवत्त -1;
	    अञ्ञथा सचे (istrue)
		self->tuple->tv_lock = VAR_LOCKED;
	    अञ्ञथा
		self->tuple->tv_lock = 0;
	}
	पटिनिवत्त 0;
    }
    अञ्ञथा
    {
	PyErr_FORMAT(PyExc_AttributeError, N_("cannot set attribute %s"), name);
	पटिनिवत्त -1;
    }
}

स्थिर PySequenceMethods TupleAsSeq = {
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

स्थिर PyMappingMethods TupleAsMapping = {
    /* mp_length	*/ (lenfunc) TupleLength,
    /* mp_subscript     */ (binaryfunc) TupleItem,
    /* mp_ass_subscript */ (objobjargproc) 0,
};

स्थिर संरचना PyMethodDef TupleMethods[] = {
    {"__dir__",	(PyCFunction)TupleDir,		METH_NOARGS,	""},
    { रिक्त,	रिक्त,				0,		रिक्त}
};

प्रकारनिर्देश संरचना
{
    PyObject_HEAD
    char_u	*name;
    पूर्णाङ्क		argc;
    typval_T	*argv;
    dict_T	*self;
    pylinkedlist_T	ref;
    पूर्णाङ्क		auto_rebind;
} FunctionObject;

DEFINE_PY_TYPE_OBJECT(FunctionType);

#define NEW_FUNCTION(name, argc, argv, self, pt_auto) \
    FunctionNew(FunctionTypePtr, (name), (argc), (argv), (self), (pt_auto))

    स्थिर PyObject *
FunctionNew(PyTypeObject *subtype, char_u *name, पूर्णाङ्क argc, typval_T *argv,
	dict_T *selfdict, पूर्णाङ्क auto_rebind)
{
    FunctionObject	*self;

    self = (FunctionObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    सचे (self == रिक्त)
	पटिनिवत्त रिक्त;

    सचे (isdigit((unsigned वर्ण)*name))
    {
	सचे (!translated_function_exists(name, FALSE))
	{
	    PyErr_FORMAT(Py_Exc_मानम्_Error,
		    N_("unnamed function %s does not exist"), name);
	    पटिनिवत्त रिक्त;
	}
	self->name = vim_strsave(name);
    }
    अञ्ञथा
    {
	char_u *p;

	सचे ((p = get_expanded_name(name,
			    vim_strchr(name, AUTOLOAD_CHAR) == रिक्त)) == रिक्त)
	{
	    PyErr_FORMAT(Py_Exc_मानम्_Error,
		    N_("function %s does not exist"), name);
	    पटिनिवत्त रिक्त;
	}

	सचे (p[0] == K_SPECIAL && p[1] == KS_EXTRA && p[2] == (पूर्णाङ्क)KE_SNR)
	{
	    char_u *np;
	    size_t len = STRLEN(p) + 1;

	    सचे ((np = alloc(len + 2)) == रिक्त)
	    {
		vim_free(p);
		पटिनिवत्त रिक्त;
	    }
	    mch_memmove(np, "<SNR>", 5);
	    mch_memmove(np + 5, p + 3, len - 3);
	    vim_free(p);
	    self->name = np;
	}
	अञ्ञथा
	    self->name = p;
    }

    func_ref(self->name);
    self->argc = argc;
    self->argv = argv;
    self->self = selfdict;
    self->auto_rebind = selfdict == रिक्त ? TRUE : auto_rebind;

    सचे (self->argv || self->self)
	pyll_add((PyObject *)(self), &self->ref, &lastfunc);

    पटिनिवत्त (PyObject *)(self);
}

    स्थिर PyObject *
FunctionConstructor(PyTypeObject *subtype, PyObject *args, PyObject *kwargs)
{
    PyObject	*self;
    PyObject	*selfdictObject;
    PyObject	*autoRebindObject;
    PyObject	*argsObject = रिक्त;
    char_u	*name;
    typval_T	selfdicttv;
    typval_T	argstv;
    list_T	*argslist = रिक्त;
    dict_T	*selfdict = रिक्त;
    पूर्णाङ्क		argc = 0;
    पूर्णाङ्क		auto_rebind = TRUE;
    typval_T	*argv = रिक्त;
    typval_T	*curtv;
    listitem_T	*li;

    सचे (kwargs != रिक्त)
    {
	selfdictObject = PyDict_GetItemString(kwargs, "self");
	सचे (selfdictObject != रिक्त)
	{
	    सचे (ConvertFromPyMapping(selfdictObject, &selfdicttv) == -1)
		पटिनिवत्त रिक्त;
	    selfdict = selfdicttv.vval.v_dict;
	}
	argsObject = PyDict_GetItemString(kwargs, "args");
	सचे (argsObject != रिक्त)
	{
	    सचे (ConvertFromPySequence(argsObject, &argstv) == -1)
	    {
		dict_unref(selfdict);
		पटिनिवत्त रिक्त;
	    }
	    argslist = argstv.vval.v_list;
	    CHECK_LIST_MATERIALIZE(argslist);

	    argc = argslist->lv_len;
	    सचे (argc != 0)
	    {
		argv = PyMem_New(typval_T, (size_t) argc);
		सचे (argv == रिक्त)
		{
		    PyErr_NoMemory();
		    dict_unref(selfdict);
		    list_unref(argslist);
		    पटिनिवत्त रिक्त;
		}
		curtv = argv;
		FOR_ALL_LIST_वस्तूनि(argslist, li)
		    copy_tv(&li->li_tv, curtv++);
	    }
	    list_unref(argslist);
	}
	सचे (selfdict != रिक्त)
	{
	    auto_rebind = FALSE;
	    autoRebindObject = PyDict_GetItemString(kwargs, "auto_rebind");
	    सचे (autoRebindObject != रिक्त)
	    {
		auto_rebind = PyObject_IsTrue(autoRebindObject);
		सचे (auto_rebind == -1)
		{
		    dict_unref(selfdict);
		    list_unref(argslist);
		    पटिनिवत्त रिक्त;
		}
	    }
	}
    }

    सचे (!PyArg_ParseTuple(args, "et", "ascii", &name))
    {
	dict_unref(selfdict);
	याव (argc--)
	    clear_tv(&argv[argc]);
	PyMem_Free(argv);
	पटिनिवत्त रिक्त;
    }

    self = FunctionNew(subtype, name, argc, argv, selfdict, auto_rebind);

    PyMem_Free(name);

    पटिनिवत्त self;
}

    स्थिर शून्य
FunctionDestructor(PyObject *self_obj)
{
    FunctionObject *self = (FunctionObject*)self_obj;
    पूर्णाङ्क i;
    func_unref(self->name);
    vim_free(self->name);
    अत्थाय (i = 0; i < self->argc; ++i)
	clear_tv(&self->argv[i]);
    PyMem_Free(self->argv);
    dict_unref(self->self);
    सचे (self->argv || self->self)
	pyll_remove(&self->ref, &lastfunc);

    DESTRUCTOR_FINISH(self);
}

स्थिर वर्ण *FunctionAttrs[] = {
    "softspace", "args", "self", "auto_rebind",
    रिक्त
};

    स्थिर PyObject *
FunctionDir(PyObject *self, PyObject *args UNUSED)
{
    पटिनिवत्त ObjectDir(self, FunctionAttrs);
}

    स्थिर PyObject *
FunctionAttr(FunctionObject *self, वर्ण *name)
{
    list_T *list;
    पूर्णाङ्क i;
    सचे (strcmp(name, "name") == 0)
	पटिनिवत्त PyString_FromString((वर्ण *)(self->name));
    अञ्ञथा सचे (strcmp(name, "args") == 0)
    {
	सचे (self->argv == रिक्त || (list = list_alloc()) == रिक्त)
	    पटिनिवत्त ALWAYS_NONE;

	अत्थाय (i = 0; i < self->argc; ++i)
	    list_append_tv(list, &self->argv[i]);
	पटिनिवत्त NEW_LIST(list);
    }
    अञ्ञथा सचे (strcmp(name, "self") == 0)
	पटिनिवत्त self->self == रिक्त
	    ? ALWAYS_NONE
	    : NEW_DICTIONARY(self->self);
    अञ्ञथा सचे (strcmp(name, "auto_rebind") == 0)
	पटिनिवत्त self->auto_rebind
	    ? ALWAYS_TRUE
	    : ALWAYS_FALSE;
    अञ्ञथा सचे (strcmp(name, "__members__") == 0)
	पटिनिवत्त ObjectDir(रिक्त, FunctionAttrs);
    पटिनिवत्त रिक्त;
}

/*
 * Populate partial_T given function object.
 *
 * "exported" should be set to true when it is needed to construct a partial
 * that may be stored in a variable (i.e. may be freed by Vim).
 */
    स्थिर शून्य
set_partial(FunctionObject *self, partial_T *pt, पूर्णाङ्क exported)
{
    पूर्णाङ्क i;

    pt->pt_name = self->name;
    सचे (self->argv)
    {
	pt->pt_argc = self->argc;
	सचे (exported)
	{
	    pt->pt_argv = ALLOC_CLEAR_MULT(typval_T, self->argc);
	    अत्थाय (i = 0; i < pt->pt_argc; ++i)
		copy_tv(&self->argv[i], &pt->pt_argv[i]);
	}
	अञ्ञथा
	    pt->pt_argv = self->argv;
    }
    अञ्ञथा
    {
	pt->pt_argc = 0;
	pt->pt_argv = रिक्त;
    }
    pt->pt_auto = self->auto_rebind || !exported;
    pt->pt_dict = self->self;
    सचे (exported && self->self)
	++pt->pt_dict->dv_refcount;
    सचे (exported)
	pt->pt_name = vim_strsave(pt->pt_name);
    pt->pt_refcount = 1;
}

    स्थिर PyObject *
FunctionCall(PyObject *self_obj, PyObject *argsObject, PyObject *kwargs)
{
    FunctionObject *self = (FunctionObject*)self_obj;
    char_u	*name = self->name;
    typval_T	args;
    typval_T	selfdicttv;
    typval_T	rettv;
    dict_T	*selfdict = रिक्त;
    PyObject	*selfdictObject;
    PyObject	*ret;
    पूर्णाङ्क		error;
    partial_T	pt;
    partial_T	*pt_ptr = रिक्त;

    सचे (ConvertFromPySequence(argsObject, &args) == -1)
	पटिनिवत्त रिक्त;

    सचे (kwargs != रिक्त)
    {
	selfdictObject = PyDict_GetItemString(kwargs, "self");
	सचे (selfdictObject != रिक्त)
	{
	    सचे (ConvertFromPyMapping(selfdictObject, &selfdicttv) == -1)
	    {
		clear_tv(&args);
		पटिनिवत्त रिक्त;
	    }
	    selfdict = selfdicttv.vval.v_dict;
	}
    }

    सचे (self->argv || self->self)
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

    सचे (VimTryEnd())
	ret = रिक्त;
    अञ्ञथा सचे (error != OK)
    {
	ret = रिक्त;
	PyErr_VIM_FORMAT(N_("failed to run function %s"), (वर्ण *)name);
    }
    अञ्ञथा
	ret = ConvertToPyObject(&rettv);

    clear_tv(&args);
    clear_tv(&rettv);
    सचे (selfdict != रिक्त)
	clear_tv(&selfdicttv);

    पटिनिवत्त ret;
}

    स्थिर PyObject *
FunctionRepr(PyObject *self_obj)
{
    FunctionObject *self = (FunctionObject*)self_obj;
    PyObject *ret;
    garray_T repr_ga;
    पूर्णाङ्क i;
    char_u *tofree = रिक्त;
    typval_T tv;
    char_u numbuf[NUMBUFLEN];

    ga_init2(&repr_ga, परिमाण(वर्ण), 70);
    ga_concat(&repr_ga, (char_u *)"<vim.Function '");
    सचे (self->name)
	ga_concat(&repr_ga, self->name);
    अञ्ञथा
	ga_concat(&repr_ga, (char_u *)"<NULL>");
    ga_append(&repr_ga, '\'');
    सचे (self->argv)
    {
	ga_concat(&repr_ga, (char_u *)", args=[");
	++emsg_silent;
	अत्थाय (i = 0; i < self->argc; i++)
	{
	    सचे (i != 0)
		ga_concat(&repr_ga, (char_u *)", ");
	    ga_concat(&repr_ga, tv2string(&self->argv[i], &tofree, numbuf,
			get_copyID()));
	    vim_free(tofree);
	}
	--emsg_silent;
	ga_append(&repr_ga, ']');
    }
    सचे (self->self)
    {
	ga_concat(&repr_ga, (char_u *)", self=");
	tv.v_type = VAR_DICT;
	tv.vval.v_dict = self->self;
	++emsg_silent;
	ga_concat(&repr_ga, tv2string(&tv, &tofree, numbuf, get_copyID()));
	--emsg_silent;
	vim_free(tofree);
	सचे (self->auto_rebind)
	    ga_concat(&repr_ga, (char_u *)", auto_rebind=True");
    }
    ga_append(&repr_ga, '>');
    ret = PyString_FromString((वर्ण *)repr_ga.ga_data);
    ga_clear(&repr_ga);
    पटिनिवत्त ret;
}

स्थिर संरचना PyMethodDef FunctionMethods[] = {
    {"__dir__",	(PyCFunction)FunctionDir,   METH_NOARGS,		""},
    { रिक्त,	रिक्त,			0,				रिक्त}
};

/*
 * Options object
 */

DEFINE_PY_TYPE_OBJECT(OptionsType);

प्रकारनिर्देश पूर्णाङ्क (*checkfun)(शून्य *);

प्रकारनिर्देश संरचना
{
    PyObject_HEAD
    पूर्णाङ्क		opt_type;
    शून्य	*from;
    checkfun	Check;
    PyObject	*fromObj;
} OptionsObject;

    स्थिर पूर्णाङ्क
dummy_check(शून्य *arg UNUSED)
{
    पटिनिवत्त 0;
}

    स्थिर PyObject *
OptionsNew(पूर्णाङ्क opt_type, शून्य *from, checkfun Check, PyObject *fromObj)
{
    OptionsObject	*self;

    self = PyObject_GC_New(OptionsObject, OptionsTypePtr);
    सचे (self == रिक्त)
	पटिनिवत्त रिक्त;

    self->opt_type = opt_type;
    self->from = from;
    self->Check = Check;
    self->fromObj = fromObj;
    सचे (fromObj)
	Py_INCREF(fromObj);

    पटिनिवत्त (PyObject *)(self);
}

    स्थिर शून्य
OptionsDestructor(PyObject *self_obj)
{
    OptionsObject *self = (OptionsObject*)self_obj;
    PyObject_GC_UnTrack((शून्य *)(self));
    Py_XDECREF(self->fromObj);
    PyObject_GC_Del((शून्य *)(self));
}

    स्थिर पूर्णाङ्क
OptionsTraverse(PyObject *self_obj, visitproc visit, शून्य *arg)
{
    OptionsObject *self = (OptionsObject*)self_obj;
    Py_VISIT(self->fromObj);
    पटिनिवत्त 0;
}

    स्थिर पूर्णाङ्क
OptionsClear(PyObject *self_obj)
{
    OptionsObject *self = (OptionsObject*)self_obj;
    Py_CLEAR(self->fromObj);
    पटिनिवत्त 0;
}

    स्थिर PyObject *
OptionsItem(OptionsObject *self, PyObject *कुञ्चिका_Object)
{
    char_u	*कुञ्चिका;
    पूर्णाङ्क		flags;
    long	numval;
    char_u	*stringval;
    PyObject	*todecref;

    सचे (self->Check(self->fromObj))
	पटिनिवत्त रिक्त;

    सचे (!(कुञ्चिका = StringToChars(कुञ्चिका_Object, &todecref)))
	पटिनिवत्त रिक्त;

    सचे (*कुञ्चिका == NUL)
    {
	RAISE_NO_EMPTY_कुञ्चिकाः;
	Py_XDECREF(todecref);
	पटिनिवत्त रिक्त;
    }

    flags = get_option_मानम्_strict(कुञ्चिका, &numval, &stringval,
				    self->opt_type, self->from);

    Py_XDECREF(todecref);

    सचे (flags == 0)
    {
	PyErr_SetObject(Py_Exc_कुञ्चिका_Error, कुञ्चिका_Object);
	पटिनिवत्त रिक्त;
    }

    सचे (flags & SOPT_UNSET)
    {
	Py_INCREF(Py_None);
	पटिनिवत्त Py_None;
    }
    अञ्ञथा सचे (flags & SOPT_BOOL)
    {
	PyObject	*ret;
	ret = numval ? Py_True : Py_False;
	Py_INCREF(ret);
	पटिनिवत्त ret;
    }
    अञ्ञथा सचे (flags & SOPT_अङ्कः)
	पटिनिवत्त PyInt_FromLong(numval);
    अञ्ञथा सचे (flags & SOPT_STRING)
    {
	सचे (stringval)
	{
	    PyObject	*ret = PyBytes_FromString((वर्ण *)stringval);
	    vim_free(stringval);
	    पटिनिवत्त ret;
	}
	अञ्ञथा
	{
	    PyErr_SET_STRING(PyExc_RuntimeError,
		    N_("unable to get option value"));
	    पटिनिवत्त रिक्त;
	}
    }
    अञ्ञथा
    {
	PyErr_SET_VIM(N_("internal error: unknown option type"));
	पटिनिवत्त रिक्त;
    }
}

    स्थिर पूर्णाङ्क
OptionsContains(OptionsObject *self, PyObject *कुञ्चिका_Object)
{
    char_u	*कुञ्चिका;
    PyObject	*todecref;

    सचे (!(कुञ्चिका = StringToChars(कुञ्चिका_Object, &todecref)))
	पटिनिवत्त -1;

    सचे (*कुञ्चिका == NUL)
    {
	Py_XDECREF(todecref);
	पटिनिवत्त 0;
    }

    सचे (get_option_मानम्_strict(कुञ्चिका, रिक्त, रिक्त, self->opt_type, रिक्त))
    {
	Py_XDECREF(todecref);
	पटिनिवत्त 1;
    }
    अञ्ञथा
    {
	Py_XDECREF(todecref);
	पटिनिवत्त 0;
    }
}

प्रकारनिर्देश संरचना
{
    शून्य	*lastoption;
    पूर्णाङ्क		opt_type;
} optiterinfo_T;

    स्थिर PyObject *
OptionsIterNext(शून्य **arg)
{
    char_u	*name;
    optiterinfo_T **oii = (optiterinfo_T**)arg;

    सचे ((name = option_iter_next(&((*oii)->lastoption), (*oii)->opt_type)))
	पटिनिवत्त PyString_FromString((वर्ण *)name);

    पटिनिवत्त रिक्त;
}

    स्थिर PyObject *
OptionsIter(PyObject *self_obj)
{
    OptionsObject	*self = (OptionsObject*)self_obj;
    optiterinfo_T	*oii;

    सचे (!(oii = PyMem_New(optiterinfo_T, 1)))
    {
	PyErr_NoMemory();
	पटिनिवत्त रिक्त;
    }

    oii->opt_type = self->opt_type;
    oii->lastoption = रिक्त;

    पटिनिवत्त IterNew(oii,
	    PyMem_Free, OptionsIterNext,
	    रिक्त, रिक्त, (PyObject *)self);
}

    स्थिर पूर्णाङ्क
set_option_मानम्_err(char_u *कुञ्चिका, पूर्णाङ्क numval, char_u *stringval, पूर्णाङ्क opt_flags)
{
    वर्ण	*errmsg;

    सचे ((errmsg = set_option_मानम्(कुञ्चिका, numval, stringval, opt_flags)))
    {
	सचे (VimTryEnd())
	    पटिनिवत्त FAIL;
	PyErr_SetVim(errmsg);
	पटिनिवत्त FAIL;
    }
    पटिनिवत्त OK;
}

    स्थिर पूर्णाङ्क
set_option_मानम्_for(
	char_u *कुञ्चिका,
	पूर्णाङ्क numval,
	char_u *stringval,
	पूर्णाङ्क opt_flags,
	पूर्णाङ्क opt_type,
	शून्य *from)
{
    switchwin_T	switchwin;
    bufref_T	save_curbuf;
    पूर्णाङ्क		set_ret = 0;

    VimTryStart();
    विकल्प (opt_type)
    {
	पक्ष SREQ_WIN:
	    सचे (switch_win(&switchwin, (win_T *)from,
			      win_find_tabpage((win_T *)from), FALSE) == FAIL)
	    {
		restore_win(&switchwin, TRUE);
		सचे (VimTryEnd())
		    पटिनिवत्त -1;
		PyErr_SET_VIM(N_("problem while switching windows"));
		पटिनिवत्त -1;
	    }
	    set_ret = set_option_मानम्_err(कुञ्चिका, numval, stringval, opt_flags);
	    restore_win(&switchwin, TRUE);
	    भङ्ग;
	पक्ष SREQ_BUF:
	    switch_buffer(&save_curbuf, (buf_T *)from);
	    set_ret = set_option_मानम्_err(कुञ्चिका, numval, stringval, opt_flags);
	    restore_buffer(&save_curbuf);
	    भङ्ग;
	पक्ष SREQ_GLOBAL:
	    set_ret = set_option_मानम्_err(कुञ्चिका, numval, stringval, opt_flags);
	    भङ्ग;
    }
    सचे (set_ret == FAIL)
	पटिनिवत्त -1;
    पटिनिवत्त VimTryEnd();
}

    स्थिर पूर्णाङ्क
OptionsAssItem(OptionsObject *self, PyObject *कुञ्चिका_Object, PyObject *valObject)
{
    char_u	*कुञ्चिका;
    पूर्णाङ्क		flags;
    पूर्णाङ्क		opt_flags;
    पूर्णाङ्क		ret = 0;
    PyObject	*todecref;

    सचे (self->Check(self->fromObj))
	पटिनिवत्त -1;

    सचे (!(कुञ्चिका = StringToChars(कुञ्चिका_Object, &todecref)))
	पटिनिवत्त -1;

    सचे (*कुञ्चिका == NUL)
    {
	RAISE_NO_EMPTY_कुञ्चिकाः;
	Py_XDECREF(todecref);
	पटिनिवत्त -1;
    }

    flags = get_option_मानम्_strict(कुञ्चिका, रिक्त, रिक्त,
				    self->opt_type, self->from);

    सचे (flags == 0)
    {
	PyErr_SetObject(Py_Exc_कुञ्चिका_Error, कुञ्चिका_Object);
	Py_XDECREF(todecref);
	पटिनिवत्त -1;
    }

    सचे (valObject == रिक्त)
    {
	सचे (self->opt_type == SREQ_GLOBAL)
	{
	    PyErr_FORMAT(Py_Exc_मानम्_Error,
		    N_("unable to unset global option %s"), कुञ्चिका);
	    Py_XDECREF(todecref);
	    पटिनिवत्त -1;
	}
	अञ्ञथा सचे (!(flags & SOPT_GLOBAL))
	{
	    PyErr_FORMAT(Py_Exc_मानम्_Error,
		    N_("unable to unset option %s "
		       "which does not have global value"), कुञ्चिका);
	    Py_XDECREF(todecref);
	    पटिनिवत्त -1;
	}
	अञ्ञथा
	{
	    unset_global_local_option(कुञ्चिका, self->from);
	    Py_XDECREF(todecref);
	    पटिनिवत्त 0;
	}
    }

    opt_flags = (self->opt_type ? OPT_LOCAL : OPT_GLOBAL);

    सचे (flags & SOPT_BOOL)
    {
	पूर्णाङ्क	istrue = PyObject_IsTrue(valObject);

	सचे (istrue == -1)
	    ret = -1;
	अञ्ञथा
	    ret = set_option_मानम्_for(कुञ्चिका, istrue, रिक्त,
				    opt_flags, self->opt_type, self->from);
    }
    अञ्ञथा सचे (flags & SOPT_अङ्कः)
    {
	long	val;

	सचे (सङ्ख्या_To_Long(valObject, &val, सङ्ख्या_INT))
	{
	    Py_XDECREF(todecref);
	    पटिनिवत्त -1;
	}

	ret = set_option_मानम्_for(कुञ्चिका, (पूर्णाङ्क) val, रिक्त, opt_flags,
				self->opt_type, self->from);
    }
    अञ्ञथा
    {
	char_u		*val;
	PyObject	*todecref2;

	सचे ((val = StringToChars(valObject, &todecref2)))
	{
	    ret = set_option_मानम्_for(कुञ्चिका, 0, val, opt_flags,
				    self->opt_type, self->from);
	    Py_XDECREF(todecref2);
	}
	अञ्ञथा
	    ret = -1;
    }

    Py_XDECREF(todecref);

    पटिनिवत्त ret;
}

स्थिर PySequenceMethods OptionsAsSeq = {
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

स्थिर PyMappingMethods OptionsAsMapping = {
    (lenfunc)       रिक्त,
    (binaryfunc)    OptionsItem,
    (objobjargproc) OptionsAssItem,
};

// Tabpage object

प्रकारनिर्देश संरचना
{
    PyObject_HEAD
    tabpage_T	*tab;
} TabPageObject;

स्थिर PyObject *WinListNew(TabPageObject *tabObject);

DEFINE_PY_TYPE_OBJECT(TabPageType);

    स्थिर पूर्णाङ्क
CheckTabPage(TabPageObject *self)
{
    सचे (self->tab == INVALID_TABPAGE_मानम्)
    {
	PyErr_SET_VIM(N_("attempt to refer to deleted tab page"));
	पटिनिवत्त -1;
    }

    पटिनिवत्त 0;
}

    स्थिर PyObject *
TabPageNew(tabpage_T *tab)
{
    TabPageObject *self;

    सचे (TAB_PYTHON_REF(tab))
    {
	self = TAB_PYTHON_REF(tab);
	Py_INCREF((PyObject *)self);
    }
    अञ्ञथा
    {
	self = PyObject_NEW(TabPageObject, TabPageTypePtr);
	सचे (self == रिक्त)
	    पटिनिवत्त रिक्त;
	self->tab = tab;
	TAB_PYTHON_REF(tab) = self;
    }

    पटिनिवत्त (PyObject *)(self);
}

    स्थिर शून्य
TabPageDestructor(PyObject *self_obj)
{
    TabPageObject *self = (TabPageObject*)self_obj;
    सचे (self->tab && self->tab != INVALID_TABPAGE_मानम्)
	TAB_PYTHON_REF(self->tab) = रिक्त;

    DESTRUCTOR_FINISH(self);
}

स्थिर वर्ण *TabPageAttrs[] = {
    "windows", "number", "vars", "window", "valid",
    रिक्त
};

    स्थिर PyObject *
TabPageDir(PyObject *self, PyObject *args UNUSED)
{
    पटिनिवत्त ObjectDir(self, TabPageAttrs);
}

    स्थिर PyObject *
TabPageAttrValid(TabPageObject *self, वर्ण *name)
{
    PyObject	*ret;

    सचे (strcmp(name, "valid") != 0)
	पटिनिवत्त रिक्त;

    ret = ((self->tab == INVALID_TABPAGE_मानम्) ? Py_False : Py_True);
    Py_INCREF(ret);
    पटिनिवत्त ret;
}

    स्थिर PyObject *
TabPageAttr(TabPageObject *self, वर्ण *name)
{
    सचे (strcmp(name, "windows") == 0)
	पटिनिवत्त WinListNew(self);
    अञ्ञथा सचे (strcmp(name, "number") == 0)
	पटिनिवत्त PyLong_FromLong((long) get_tab_सङ्ख्या(self->tab));
    अञ्ञथा सचे (strcmp(name, "vars") == 0)
	पटिनिवत्त NEW_DICTIONARY(self->tab->tp_vars);
    अञ्ञथा सचे (strcmp(name, "window") == 0)
    {
	// For current tab window.c does not bother to set or update tp_curwin
	सचे (self->tab == curtab)
	    पटिनिवत्त WindowNew(curwin, curtab);
	अञ्ञथा
	    पटिनिवत्त WindowNew(self->tab->tp_curwin, self->tab);
    }
    अञ्ञथा सचे (strcmp(name, "__members__") == 0)
	पटिनिवत्त ObjectDir(रिक्त, TabPageAttrs);
    पटिनिवत्त रिक्त;
}

    स्थिर PyObject *
TabPageRepr(PyObject *self_obj)
{
    TabPageObject *self = (TabPageObject*)self_obj;
    सचे (self->tab == INVALID_TABPAGE_मानम्)
	पटिनिवत्त PyString_FromFormat("<tabpage object (deleted) at %p>", (शून्य *)self);
    अञ्ञथा
    {
	पूर्णाङ्क	t = get_tab_सङ्ख्या(self->tab);

	सचे (t == 0)
	    पटिनिवत्त PyString_FromFormat("<tabpage object (unknown) at %p>",
					(शून्य *)self);
	अञ्ञथा
	    पटिनिवत्त PyString_FromFormat("<tabpage %d>", t - 1);
    }
}

स्थिर संरचना PyMethodDef TabPageMethods[] = {
    // name,	    function,			calling,	documentation
    {"__dir__",	    (PyCFunction)TabPageDir,	METH_NOARGS,	""},
    { रिक्त,	    रिक्त,			0,		रिक्त}
};

/*
 * Window list object
 */

DEFINE_PY_TYPE_OBJECT(TabListType);
स्थिर PySequenceMethods TabListAsSeq;

प्रकारनिर्देश संरचना
{
    PyObject_HEAD
} TabListObject;

स्थिर TabListObject TheTabPageList =
{
    PyObject_HEAD_INIT_TYPE(TabListType)
};

    स्थिर PyInt
TabListLength(PyObject *self UNUSED)
{
    tabpage_T	*tp = first_tabpage;
    PyInt	n = 0;

    याव (tp != रिक्त)
    {
	++n;
	tp = tp->tp_next;
    }

    पटिनिवत्त n;
}

    स्थिर PyObject *
TabListItem(PyObject *self UNUSED, PyInt n)
{
    tabpage_T	*tp;

    अत्थाय (tp = first_tabpage; tp != रिक्त; tp = tp->tp_next, --n)
	सचे (n == 0)
	    पटिनिवत्त TabPageNew(tp);

    PyErr_SET_STRING(PyExc_IndexError, N_("no such tab page"));
    पटिनिवत्त रिक्त;
}

/*
 * Window object
 */

प्रकारनिर्देश संरचना
{
    PyObject_HEAD
    win_T	*win;
    TabPageObject	*tabObject;
} WindowObject;

DEFINE_PY_TYPE_OBJECT(WindowType);

    स्थिर पूर्णाङ्क
CheckWindow(WindowObject *self)
{
    सचे (self->win == INVALID_WINDOW_मानम्)
    {
	PyErr_SET_VIM(N_("attempt to refer to deleted window"));
	पटिनिवत्त -1;
    }

    पटिनिवत्त 0;
}

    स्थिर पूर्णाङ्क
CheckWindowCb(शून्य *self)
{
    पटिनिवत्त CheckWindow((WindowObject*)self);
}

    स्थिर PyObject *
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

    सचे (WIN_PYTHON_REF(win))
    {
	self = WIN_PYTHON_REF(win);
	Py_INCREF((PyObject *)self);
    }
    अञ्ञथा
    {
	self = PyObject_GC_New(WindowObject, WindowTypePtr);
	सचे (self == रिक्त)
	    पटिनिवत्त रिक्त;
	self->win = win;
	WIN_PYTHON_REF(win) = self;
    }

    self->tabObject = ((TabPageObject *)(TabPageNew(tab)));

    पटिनिवत्त (PyObject *)(self);
}

    स्थिर शून्य
WindowDestructor(PyObject *self_obj)
{
    WindowObject *self = (WindowObject*)self_obj;
    PyObject_GC_UnTrack((शून्य *)(self));
    सचे (self->win && self->win != INVALID_WINDOW_मानम्)
	WIN_PYTHON_REF(self->win) = रिक्त;
    Py_XDECREF(((PyObject *)(self->tabObject)));
    PyObject_GC_Del((शून्य *)(self));
}

    स्थिर पूर्णाङ्क
WindowTraverse(PyObject *self_obj, visitproc visit, शून्य *arg)
{
    WindowObject *self = (WindowObject*)self_obj;
    Py_VISIT(((PyObject *)(self->tabObject)));
    पटिनिवत्त 0;
}

    स्थिर पूर्णाङ्क
WindowClear(PyObject *self_obj)
{
    WindowObject *self = (WindowObject*)self_obj;
    Py_CLEAR(self->tabObject);
    पटिनिवत्त 0;
}

    स्थिर win_T *
get_firstwin(TabPageObject *tabObject)
{
    सचे (tabObject)
    {
	सचे (CheckTabPage(tabObject))
	    पटिनिवत्त रिक्त;
	// For current tab window.c does not bother to set or update tp_firstwin
	अञ्ञथा सचे (tabObject->tab == curtab)
	    पटिनिवत्त firstwin;
	अञ्ञथा
	    पटिनिवत्त tabObject->tab->tp_firstwin;
    }
    अञ्ञथा
	पटिनिवत्त firstwin;
}

// Use the same order as in the WindowAttr() function.
स्थिर वर्ण *WindowAttrs[] = {
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
    रिक्त
};

    स्थिर PyObject *
WindowDir(PyObject *self, PyObject *args UNUSED)
{
    पटिनिवत्त ObjectDir(self, WindowAttrs);
}

    स्थिर PyObject *
WindowAttrValid(WindowObject *self, वर्ण *name)
{
    PyObject	*ret;

    सचे (strcmp(name, "valid") != 0)
	पटिनिवत्त रिक्त;

    ret = ((self->win == INVALID_WINDOW_मानम्) ? Py_False : Py_True);
    Py_INCREF(ret);
    पटिनिवत्त ret;
}

    स्थिर PyObject *
WindowAttr(WindowObject *self, वर्ण *name)
{
    सचे (strcmp(name, "buffer") == 0)
	पटिनिवत्त (PyObject *)BufferNew(self->win->w_buffer);
    अञ्ञथा सचे (strcmp(name, "cursor") == 0)
    {
	pos_T *pos = &self->win->w_cursor;

	पटिनिवत्त Py_Build_मानम्("(ll)", (long)(pos->lnum), (long)(pos->col));
    }
    अञ्ञथा सचे (strcmp(name, "height") == 0)
	पटिनिवत्त PyLong_FromLong((long)(self->win->w_height));
    अञ्ञथा सचे (strcmp(name, "row") == 0)
	पटिनिवत्त PyLong_FromLong((long)(self->win->w_winrow));
    अञ्ञथा सचे (strcmp(name, "width") == 0)
	पटिनिवत्त PyLong_FromLong((long)(self->win->w_width));
    अञ्ञथा सचे (strcmp(name, "col") == 0)
	पटिनिवत्त PyLong_FromLong((long)(self->win->w_wincol));
    अञ्ञथा सचे (strcmp(name, "vars") == 0)
	पटिनिवत्त NEW_DICTIONARY(self->win->w_vars);
    अञ्ञथा सचे (strcmp(name, "options") == 0)
	पटिनिवत्त OptionsNew(SREQ_WIN, self->win, CheckWindowCb,
			(PyObject *) self);
    अञ्ञथा सचे (strcmp(name, "number") == 0)
    {
	सचे (CheckTabPage(self->tabObject))
	    पटिनिवत्त रिक्त;
	पटिनिवत्त PyLong_FromLong((long)
		get_win_सङ्ख्या(self->win, get_firstwin(self->tabObject)));
    }
    अञ्ञथा सचे (strcmp(name, "tabpage") == 0)
    {
	Py_INCREF((PyObject *)self->tabObject);
	पटिनिवत्त (PyObject *)(self->tabObject);
    }
    अञ्ञथा सचे (strcmp(name, "__members__") == 0)
	पटिनिवत्त ObjectDir(रिक्त, WindowAttrs);
    अञ्ञथा
	पटिनिवत्त रिक्त;
}

    स्थिर पूर्णाङ्क
WindowSetattr(PyObject *self_obj, वर्ण *name, PyObject *valObject)
{
    WindowObject *self = (WindowObject*)self_obj;
    सचे (CheckWindow(self))
	पटिनिवत्त -1;

    सचे (strcmp(name, "buffer") == 0)
    {
	PyErr_SET_STRING(PyExc_TypeError, N_("readonly attribute: buffer"));
	पटिनिवत्त -1;
    }
    अञ्ञथा सचे (strcmp(name, "cursor") == 0)
    {
	long lnum;
	long col;

	सचे (!PyArg_Parse(valObject, "(ll)", &lnum, &col))
	    पटिनिवत्त -1;

	सचे (lnum <= 0 || lnum > self->win->w_buffer->b_ml.ml_पङ्क्तिः_गणनम्)
	{
	    PyErr_SET_VIM(N_("cursor position outside buffer"));
	    पटिनिवत्त -1;
	}

	// Check for keyboard interrupts
	सचे (VimCheckInterrupt())
	    पटिनिवत्त -1;

	self->win->w_cursor.lnum = lnum;
	self->win->w_cursor.col = col;
	self->win->w_set_curswant = TRUE;
	self->win->w_cursor.coladd = 0;
	// When column is out of range silently correct it.
	check_cursor_col_win(self->win);

	update_screen(UPD_VALID);
	पटिनिवत्त 0;
    }
    अञ्ञथा सचे (strcmp(name, "height") == 0)
    {
	long	height;
	win_T	*savewin;

	सचे (सङ्ख्या_To_Long(valObject, &height, सङ्ख्या_INT|सङ्ख्या_UNSIGNED))
	    पटिनिवत्त -1;

#ifdef FEAT_GUI
	need_mouse_correct = TRUE;
#endif
	savewin = curwin;
	curwin = self->win;
	curbuf = curwin->w_buffer;

	VimTryStart();
	win_setheight((पूर्णाङ्क) height);
	curwin = savewin;
	curbuf = curwin->w_buffer;
	सचे (VimTryEnd())
	    पटिनिवत्त -1;

	पटिनिवत्त 0;
    }
    अञ्ञथा सचे (strcmp(name, "width") == 0)
    {
	long	width;
	win_T	*savewin;

	सचे (सङ्ख्या_To_Long(valObject, &width, सङ्ख्या_INT|सङ्ख्या_UNSIGNED))
	    पटिनिवत्त -1;

#ifdef FEAT_GUI
	need_mouse_correct = TRUE;
#endif
	savewin = curwin;
	curwin = self->win;
	curbuf = curwin->w_buffer;

	VimTryStart();
	win_setwidth((पूर्णाङ्क) width);
	curwin = savewin;
	curbuf = curwin->w_buffer;
	सचे (VimTryEnd())
	    पटिनिवत्त -1;

	पटिनिवत्त 0;
    }
    अञ्ञथा
    {
	PyErr_SetString(PyExc_AttributeError, name);
	पटिनिवत्त -1;
    }
}

    स्थिर PyObject *
WindowRepr(PyObject *self_obj)
{
    WindowObject *self = (WindowObject*)self_obj;
    सचे (self->win == INVALID_WINDOW_मानम्)
	पटिनिवत्त PyString_FromFormat("<window object (deleted) at %p>", (शून्य *)self);
    अञ्ञथा
    {
	पूर्णाङ्क	w = get_win_सङ्ख्या(self->win, firstwin);

	सचे (w == 0)
	    पटिनिवत्त PyString_FromFormat("<window object (unknown) at %p>",
								      (शून्य *)self);
	अञ्ञथा
	    पटिनिवत्त PyString_FromFormat("<window %d>", w - 1);
    }
}

स्थिर संरचना PyMethodDef WindowMethods[] = {
    // name,	    function,			calling,	documentation
    {"__dir__",	    (PyCFunction)WindowDir,	METH_NOARGS,	""},
    { रिक्त,	    रिक्त,			0,		रिक्त}
};

/*
 * Window list object
 */

DEFINE_PY_TYPE_OBJECT(WinListType);
स्थिर PySequenceMethods WinListAsSeq;

प्रकारनिर्देश संरचना
{
    PyObject_HEAD
    TabPageObject	*tabObject;
} WinListObject;

स्थिर WinListObject TheWindowList =
{
    PyObject_HEAD_INIT_TYPE(WinListType)
    रिक्त
};

    स्थिर PyObject *
WinListNew(TabPageObject *tabObject)
{
    WinListObject	*self;

    self = PyObject_NEW(WinListObject, WinListTypePtr);
    self->tabObject = tabObject;
    Py_INCREF((PyObject *)tabObject);

    पटिनिवत्त (PyObject *)(self);
}

    स्थिर शून्य
WinListDestructor(WinListObject *self)
{
    TabPageObject	*tabObject = self->tabObject;

    सचे (tabObject)
    {
	Py_DECREF((PyObject *)(tabObject));
    }

    DESTRUCTOR_FINISH(self);
}

    स्थिर PyInt
WinListLength(WinListObject *self)
{
    win_T	*w;
    PyInt	n = 0;

    सचे (!(w = get_firstwin(self->tabObject)))
	पटिनिवत्त -1;

    याव (w != रिक्त)
    {
	++n;
	w = W_NEXT(w);
    }

    पटिनिवत्त n;
}

    स्थिर PyObject *
WinListItem(WinListObject *self, PyInt n)
{
    win_T *w;

    सचे (!(w = get_firstwin(self->tabObject)))
	पटिनिवत्त रिक्त;

    अत्थाय (; w != रिक्त; w = W_NEXT(w), --n)
	सचे (n == 0)
	    पटिनिवत्त WindowNew(w, self->tabObject? self->tabObject->tab: curtab);

    PyErr_SET_STRING(PyExc_IndexError, N_("no such window"));
    पटिनिवत्त रिक्त;
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
    स्थिर वर्ण *
String_To_पङ्क्तिः(PyObject *obj)
{
    वर्ण	*str;
    वर्ण	*save;
    PyObject	*bytes = रिक्त;
    Py_ssize_t	len = 0;
    PyInt	i;
    वर्ण	*p;

    सचे (PyBytes_Check(obj))
    {
	सचे (PyBytes_AsStringAndSize(obj, &str, &len) == -1
		|| str == रिक्त)
	    पटिनिवत्त रिक्त;
    }
    अञ्ञथा सचे (PyUnicode_Check(obj))
    {
	सचे (!(bytes = PyUnicode_AsEncodedString(obj, ENC_OPT,
							   ERRORS_ENCODE_ARG)))
	    पटिनिवत्त रिक्त;

	सचे (PyBytes_AsStringAndSize(bytes, &str, &len) == -1
		|| str == रिक्त)
	{
	    Py_DECREF(bytes);
	    पटिनिवत्त रिक्त;
	}
    }
    अञ्ञथा
    {
#सचे PY_MAJOR_VERSION < 3
	PyErr_FORMAT_TYPE(
		N_("expected str() or unicode() instance, but got %s"),
		obj);
#अञ्ञथा
	PyErr_FORMAT_TYPE(
		N_("expected bytes() or str() instance, but got %s"),
		obj);
#endif
	पटिनिवत्त रिक्त;
    }

    /*
     * Error checking: String must not contain newlines, as we
     * are replacing a single line, and we must replace it with
     * a single line.
     * A trailing newline is removed, so that append(f.readlines()) works.
     */
    p = memchr(str, '\n', len);
    सचे (p != रिक्त)
    {
	सचे (p == str + len - 1)
	    --len;
	अञ्ञथा
	{
	    PyErr_SET_VIM(N_("string cannot contain newlines"));
	    Py_XDECREF(bytes);
	    पटिनिवत्त रिक्त;
	}
    }

    /*
     * Create a copy of the string, with internal nulls replaced by
     * newline characters, as is the Vim convention.
     */
    save = alloc(len+1);
    सचे (save == रिक्त)
    {
	PyErr_NoMemory();
	Py_XDECREF(bytes);
	पटिनिवत्त रिक्त;
    }

    अत्थाय (i = 0; i < len; ++i)
    {
	सचे (str[i] == '\0')
	    save[i] = '\n';
	अञ्ञथा
	    save[i] = str[i];
    }

    save[i] = '\0';
    Py_XDECREF(bytes);  // Python 2 does nothing here

    पटिनिवत्त save;
}

/*
 * Get a line from the specified buffer. The line number is
 * in Vim format (1-based). The line is returned as a Python
 * string object.
 */
    स्थिर PyObject *
Get_Buffer_पङ्क्तिः(buf_T *buf, PyInt n)
{
    पटिनिवत्त पङ्क्तिः_To_String((वर्ण *)ml_get_buf(buf, (linenr_T)n, FALSE));
}


/*
 * Get a list of lines from the specified buffer. The line numbers
 * are in Vim format (1-based). The range is from lo up to, but not
 * including, hi. The list is returned as a Python list of string objects.
 */
    स्थिर PyObject *
Get_Buffer_पङ्क्तिः_List(buf_T *buf, PyInt lo, PyInt hi)
{
    PyInt	i;
    PyInt	n = hi - lo;
    PyObject	*list = PyList_New(n);

    सचे (list == रिक्त)
	पटिनिवत्त रिक्त;

    अत्थाय (i = 0; i < n; ++i)
    {
	linenr_T	lnum = (linenr_T)(lo + i);
	वर्ण		*text;
	PyObject	*string;

	सचे (lnum > buf->b_ml.ml_पङ्क्तिः_गणनम्)
	    text = "";
	अञ्ञथा
	    text = (वर्ण *)ml_get_buf(buf, lnum, FALSE);
	string = पङ्क्तिः_To_String(text);
	सचे (string == रिक्त)
	{
	    Py_DECREF(list);
	    पटिनिवत्त रिक्त;
	}

	PyList_SET_ITEM(list, i, string);
    }

    // The ownership of the Python list is passed to the caller (ie,
    // the caller should Py_DECREF() the object when it is finished
    // with it).

    पटिनिवत्त list;
}

/*
 * Check if deleting lines made the cursor position invalid.
 * Changed the lines from "lo" to "hi" and added "extra" lines (negative if
 * deleted).
 */
    स्थिर शून्य
py_fix_cursor(linenr_T lo, linenr_T hi, linenr_T extra)
{
    सचे (curwin->w_cursor.lnum >= lo)
    {
	// Adjust the cursor position if it's in/after the changed
	// lines.
	सचे (curwin->w_cursor.lnum >= hi)
	{
	    curwin->w_cursor.lnum += extra;
	    check_cursor_col();
	}
	अञ्ञथा सचे (extra < 0)
	{
	    curwin->w_cursor.lnum = lo;
	    check_cursor();
	}
	अञ्ञथा
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
    स्थिर पूर्णाङ्क
Set_Buffer_पङ्क्तिः(buf_T *buf, PyInt n, PyObject *पङ्क्तिः, PyInt *len_change)
{
    bufref_T	save_curbuf = {रिक्त, 0, 0};
    switchwin_T	switchwin;

    // First of all, we check the type of the supplied Python object.
    // There are three cases:
    //	  1. NULL, or None - this is a deletion.
    //	  2. A string	   - this is a replacement.
    //	  3. Anything else - this is an error.
    सचे (पङ्क्तिः == Py_None || पङ्क्तिः == रिक्त)
    {
	PyErr_Clear();
	switchwin.sw_curwin = रिक्त;
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	VimTryStart();

	सचे (u_savedel((linenr_T)n, 1L) == FAIL)
	    RAISE_UNDO_FAIL;
	अञ्ञथा सचे (ml_delete((linenr_T)n) == FAIL)
	    RAISE_DELETE_पङ्क्तिः_FAIL;
	अञ्ञथा
	{
	    सचे (buf == curbuf && (switchwin.sw_curwin != रिक्त
					   || save_curbuf.br_buf == रिक्त))
		// Using an existing window for the buffer, adjust the cursor
		// position.
		py_fix_cursor((linenr_T)n, (linenr_T)n + 1, (linenr_T)-1);
	    सचे (save_curbuf.br_buf == रिक्त)
		// Only adjust marks if we managed to switch to a window that
		// holds the buffer, otherwise line numbers will be invalid.
		deleted_पङ्क्तयः_mark((linenr_T)n, 1L);
	}

	restore_win_for_buf(&switchwin, &save_curbuf);

	सचे (VimTryEnd())
	    पटिनिवत्त FAIL;

	सचे (len_change)
	    *len_change = -1;

	पटिनिवत्त OK;
    }
    अञ्ञथा सचे (PyBytes_Check(पङ्क्तिः) || PyUnicode_Check(पङ्क्तिः))
    {
	वर्ण	*save = String_To_पङ्क्तिः(पङ्क्तिः);

	सचे (save == रिक्त)
	    पटिनिवत्त FAIL;

	VimTryStart();

	// We do not need to free "save" if ml_replace() consumes it.
	PyErr_Clear();
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	सचे (u_savesub((linenr_T)n) == FAIL)
	{
	    RAISE_UNDO_FAIL;
	    vim_free(save);
	}
	अञ्ञथा सचे (ml_replace((linenr_T)n, (char_u *)save, FALSE) == FAIL)
	{
	    RAISE_REPLACE_पङ्क्तिः_FAIL;
	    vim_free(save);
	}
	अञ्ञथा
	    changed_bytes((linenr_T)n, 0);

	restore_win_for_buf(&switchwin, &save_curbuf);

	// Check that the cursor is not beyond the end of the line now.
	सचे (buf == curbuf)
	    check_cursor_col();

	सचे (VimTryEnd())
	    पटिनिवत्त FAIL;

	सचे (len_change)
	    *len_change = 0;

	पटिनिवत्त OK;
    }
    अञ्ञथा
    {
	PyErr_BadArgument();
	पटिनिवत्त FAIL;
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
    स्थिर पूर्णाङ्क
Set_Buffer_पङ्क्तिः_List(
	buf_T *buf,
	PyInt lo,
	PyInt hi,
	PyObject *list,
	PyInt *len_change)
{
    bufref_T	save_curbuf = {रिक्त, 0, 0};
    switchwin_T	switchwin;

    // First of all, we check the type of the supplied Python object.
    // There are three cases:
    //	  1. NULL, or None - this is a deletion.
    //	  2. A list	   - this is a replacement.
    //	  3. Anything else - this is an error.
    सचे (list == Py_None || list == रिक्त)
    {
	PyInt	i;
	PyInt	n = (पूर्णाङ्क)(hi - lo);

	PyErr_Clear();
	VimTryStart();
	switchwin.sw_curwin = रिक्त;
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	सचे (u_savedel((linenr_T)lo, (long)n) == FAIL)
	    RAISE_UNDO_FAIL;
	अञ्ञथा
	{
	    अत्थाय (i = 0; i < n; ++i)
	    {
		सचे (ml_delete((linenr_T)lo) == FAIL)
		{
		    RAISE_DELETE_पङ्क्तिः_FAIL;
		    भङ्ग;
		}
	    }
	    सचे (buf == curbuf && (switchwin.sw_curwin != रिक्त
					       || save_curbuf.br_buf == रिक्त))
		// Using an existing window for the buffer, adjust the cursor
		// position.
		py_fix_cursor((linenr_T)lo, (linenr_T)hi, (linenr_T)-n);
	    सचे (save_curbuf.br_buf == रिक्त)
		// Only adjust marks if we managed to switch to a window that
		// holds the buffer, otherwise line numbers will be invalid.
		deleted_पङ्क्तयः_mark((linenr_T)lo, (long)i);
	}

	restore_win_for_buf(&switchwin, &save_curbuf);

	सचे (VimTryEnd())
	    पटिनिवत्त FAIL;

	सचे (len_change)
	    *len_change = -n;

	पटिनिवत्त OK;
    }
    अञ्ञथा सचे (PyList_Check(list))
    {
	PyInt	i;
	PyInt	new_len = PyList_Size(list);
	PyInt	old_len = hi - lo;
	PyInt	extra = 0;	// lines added to text, can be negative
	वर्ण	**array;

	सचे (new_len == 0)	// avoid allocating zero bytes
	    array = रिक्त;
	अञ्ञथा
	{
	    array = PyMem_New(वर्ण *, new_len);
	    सचे (array == रिक्त)
	    {
		PyErr_NoMemory();
		पटिनिवत्त FAIL;
	    }
	}

	अत्थाय (i = 0; i < new_len; ++i)
	{
	    PyObject *पङ्क्तिः;

	    सचे (!(पङ्क्तिः = PyList_GetItem(list, i)) ||
		!(array[i] = String_To_पङ्क्तिः(पङ्क्तिः)))
	    {
		याव (i)
		    vim_free(array[--i]);
		PyMem_Free(array);
		पटिनिवत्त FAIL;
	    }
	}

	VimTryStart();
	PyErr_Clear();

	// START of region without "return".  Must call restore_buffer()!
	switchwin.sw_curwin = रिक्त;
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	सचे (u_save((linenr_T)(lo-1), (linenr_T)hi) == FAIL)
	    RAISE_UNDO_FAIL;

	// If the size of the range is reducing (ie, new_len < old_len) we
	// need to delete some old_len. We do this at the start, by
	// repeatedly deleting line "lo".
	सचे (!PyErr_Occurred())
	{
	    अत्थाय (i = 0; i < old_len - new_len; ++i)
		सचे (ml_delete((linenr_T)lo) == FAIL)
		{
		    RAISE_DELETE_पङ्क्तिः_FAIL;
		    भङ्ग;
		}
	    extra -= i;
	}

	// For as long as possible, replace the existing old_len with the
	// new old_len. This is a more efficient operation, as it requires
	// less memory allocation and freeing.
	सचे (!PyErr_Occurred())
	{
	    अत्थाय (i = 0; i < old_len && i < new_len; ++i)
		सचे (ml_replace((linenr_T)(lo+i), (char_u *)array[i], FALSE)
								      == FAIL)
		{
		    RAISE_REPLACE_पङ्क्तिः_FAIL;
		    भङ्ग;
		}
	}
	अञ्ञथा
	    i = 0;

	// Now we may need to insert the remaining new old_len. If we do, we
	// must free the strings as we finish with them (we can't pass the
	// responsibility to Vim in this case).
	सचे (!PyErr_Occurred())
	{
	    याव (i < new_len)
	    {
		सचे (ml_append((linenr_T)(lo + i - 1),
					(char_u *)array[i], 0, FALSE) == FAIL)
		{
		    RAISE_निवेशय_पङ्क्तिः_FAIL;
		    भङ्ग;
		}
		vim_free(array[i]);
		++i;
		++extra;
	    }
	}

	// Free any left-over old_len, as a result of an error
	याव (i < new_len)
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
	सचे (save_curbuf.br_buf == रिक्त)
	{
	    mark_adjust((linenr_T)lo, (linenr_T)(hi - 1),
						  (long)MAXLNUM, (long)extra);
	    changed_पङ्क्तयः((linenr_T)lo, 0, (linenr_T)hi, (long)extra);
	}

	सचे (buf == curbuf && (switchwin.sw_curwin != रिक्त
					   || save_curbuf.br_buf == रिक्त))
	    // Using an existing window for the buffer, adjust the cursor
	    // position.
	    py_fix_cursor((linenr_T)lo, (linenr_T)hi, (linenr_T)extra);

	// END of region without "return".
	restore_win_for_buf(&switchwin, &save_curbuf);

	सचे (VimTryEnd())
	    पटिनिवत्त FAIL;

	सचे (len_change)
	    *len_change = new_len - old_len;

	पटिनिवत्त OK;
    }
    अञ्ञथा
    {
	PyErr_BadArgument();
	पटिनिवत्त FAIL;
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
    स्थिर पूर्णाङ्क
निवेशय_Buffer_पङ्क्तयः(buf_T *buf, PyInt n, PyObject *पङ्क्तयः, PyInt *len_change)
{
    bufref_T	save_curbuf = {रिक्त, 0, 0};
    switchwin_T	switchwin;

    // First of all, we check the type of the supplied Python object.
    // It must be a string or a list, or the call is in error.
    सचे (PyBytes_Check(पङ्क्तयः) || PyUnicode_Check(पङ्क्तयः))
    {
	वर्ण		*str = String_To_पङ्क्तिः(पङ्क्तयः);

	सचे (str == रिक्त)
	    पटिनिवत्त FAIL;

	PyErr_Clear();
	VimTryStart();
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	सचे (u_save((linenr_T)n, (linenr_T)(n + 1)) == FAIL)
	    RAISE_UNDO_FAIL;
	अञ्ञथा सचे (ml_append((linenr_T)n, (char_u *)str, 0, FALSE) == FAIL)
	    RAISE_निवेशय_पङ्क्तिः_FAIL;
	अञ्ञथा सचे (save_curbuf.br_buf == रिक्त)
	    // Only adjust marks if we managed to switch to a window that
	    // holds the buffer, otherwise line numbers will be invalid.
	    appended_पङ्क्तयः_mark((linenr_T)n, 1L);

	vim_free(str);
	restore_win_for_buf(&switchwin, &save_curbuf);
	update_screen(UPD_VALID);

	सचे (VimTryEnd())
	    पटिनिवत्त FAIL;

	सचे (len_change)
	    *len_change = 1;

	पटिनिवत्त OK;
    }
    अञ्ञथा सचे (PyList_Check(पङ्क्तयः))
    {
	PyInt	i;
	PyInt	size = PyList_Size(पङ्क्तयः);
	वर्ण	**array;

	array = PyMem_New(वर्ण *, size);
	सचे (array == रिक्त)
	{
	    PyErr_NoMemory();
	    पटिनिवत्त FAIL;
	}

	अत्थाय (i = 0; i < size; ++i)
	{
	    PyObject *पङ्क्तिः;

	    सचे (!(पङ्क्तिः = PyList_GetItem(पङ्क्तयः, i)) ||
		!(array[i] = String_To_पङ्क्तिः(पङ्क्तिः)))
	    {
		याव (i)
		    vim_free(array[--i]);
		PyMem_Free(array);
		पटिनिवत्त FAIL;
	    }
	}

	PyErr_Clear();
	VimTryStart();
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	सचे (u_save((linenr_T)n, (linenr_T)(n + 1)) == FAIL)
	    RAISE_UNDO_FAIL;
	अञ्ञथा
	{
	    अत्थाय (i = 0; i < size; ++i)
	    {
		सचे (ml_append((linenr_T)(n + i),
					(char_u *)array[i], 0, FALSE) == FAIL)
		{
		    RAISE_निवेशय_पङ्क्तिः_FAIL;

		    // Free the rest of the lines
		    याव (i < size)
			vim_free(array[i++]);

		    भङ्ग;
		}
		vim_free(array[i]);
	    }
	    सचे (i > 0 && save_curbuf.br_buf == रिक्त)
		// Only adjust marks if we managed to switch to a window that
		// holds the buffer, otherwise line numbers will be invalid.
		appended_पङ्क्तयः_mark((linenr_T)n, (long)i);
	}

	// Free the array of lines. All of its contents have now
	// been freed.
	PyMem_Free(array);
	restore_win_for_buf(&switchwin, &save_curbuf);

	update_screen(UPD_VALID);

	सचे (VimTryEnd())
	    पटिनिवत्त FAIL;

	सचे (len_change)
	    *len_change = size;

	पटिनिवत्त OK;
    }
    अञ्ञथा
    {
	PyErr_BadArgument();
	पटिनिवत्त FAIL;
    }
}

/*
 * Common routines for buffers and line ranges
 * -------------------------------------------
 */

प्रकारनिर्देश संरचना
{
    PyObject_HEAD
    buf_T *buf;
} BufferObject;

    स्थिर पूर्णाङ्क
CheckBuffer(BufferObject *self)
{
    सचे (self->buf == INVALID_BUFFER_मानम्)
    {
	PyErr_SET_VIM(N_("attempt to refer to deleted buffer"));
	पटिनिवत्त -1;
    }

    पटिनिवत्त 0;
}

    स्थिर पूर्णाङ्क
CheckBufferCb(शून्य *self)
{
    पटिनिवत्त CheckBuffer((BufferObject*)self);
}

    स्थिर PyObject *
RBItem(BufferObject *self, PyInt n, PyInt start, PyInt end)
{
    सचे (CheckBuffer(self))
	पटिनिवत्त रिक्त;

    सचे (end == -1)
	end = self->buf->b_ml.ml_पङ्क्तिः_गणनम्;

    सचे (n < 0)
	n += end - start + 1;

    सचे (n < 0 || n > end - start)
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("line number out of range"));
	पटिनिवत्त रिक्त;
    }

    पटिनिवत्त Get_Buffer_पङ्क्तिः(self->buf, n+start);
}

    स्थिर PyObject *
RBSlice(BufferObject *self, PyInt lo, PyInt hi, PyInt start, PyInt end)
{
    PyInt size;

    सचे (CheckBuffer(self))
	पटिनिवत्त रिक्त;

    सचे (end == -1)
	end = self->buf->b_ml.ml_पङ्क्तिः_गणनम्;

    size = end - start + 1;

    सचे (lo < 0)
	lo = 0;
    अञ्ञथा सचे (lo > size)
	lo = size;
    सचे (hi < 0)
	hi = 0;
    सचे (hi < lo)
	hi = lo;
    अञ्ञथा सचे (hi > size)
	hi = size;

    पटिनिवत्त Get_Buffer_पङ्क्तिः_List(self->buf, lo+start, hi+start);
}

    स्थिर PyInt
RBAsItem(
	BufferObject *self,
	PyInt n,
	PyObject *valObject,
	PyInt start,
	PyInt end,
	PyInt *new_end)
{
    PyInt len_change;

    सचे (CheckBuffer(self))
	पटिनिवत्त -1;

    सचे (end == -1)
	end = self->buf->b_ml.ml_पङ्क्तिः_गणनम्;

    सचे (n < 0)
	n += end - start + 1;

    सचे (n < 0 || n > end - start)
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("line number out of range"));
	पटिनिवत्त -1;
    }

    सचे (Set_Buffer_पङ्क्तिः(self->buf, n+start, valObject, &len_change) == FAIL)
	पटिनिवत्त -1;

    सचे (new_end)
	*new_end = end + len_change;

    पटिनिवत्त 0;
}

    स्थिर PyInt
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
    सचे (CheckBuffer(self))
	पटिनिवत्त -1;

    सचे (end == -1)
	end = self->buf->b_ml.ml_पङ्क्तिः_गणनम्;

    // Sort out the slice range
    size = end - start + 1;

    सचे (lo < 0)
	lo = 0;
    अञ्ञथा सचे (lo > size)
	lo = size;
    सचे (hi < 0)
	hi = 0;
    सचे (hi < lo)
	hi = lo;
    अञ्ञथा सचे (hi > size)
	hi = size;

    सचे (Set_Buffer_पङ्क्तिः_List(self->buf, lo + start, hi + start,
						valObject, &len_change) == FAIL)
	पटिनिवत्त -1;

    सचे (new_end)
	*new_end = end + len_change;

    पटिनिवत्त 0;
}


    स्थिर PyObject *
RBAppend(
	BufferObject *self,
	PyObject *args,
	PyInt start,
	PyInt end,
	PyInt *new_end)
{
    PyObject *पङ्क्तयः;
    PyInt len_change;
    PyInt max;
    PyInt n;

    सचे (CheckBuffer(self))
	पटिनिवत्त रिक्त;

    सचे (end == -1)
	end = self->buf->b_ml.ml_पङ्क्तिः_गणनम्;

    max = n = end - start + 1;

    सचे (!PyArg_ParseTuple(args, "O|n", &पङ्क्तयः, &n))
	पटिनिवत्त रिक्त;

    सचे (n < 0 || n > max)
    {
	PyErr_SET_STRING(PyExc_IndexError, N_("line number out of range"));
	पटिनिवत्त रिक्त;
    }

    सचे (निवेशय_Buffer_पङ्क्तयः(self->buf, n + start - 1, पङ्क्तयः, &len_change) == FAIL)
	पटिनिवत्त रिक्त;

    सचे (new_end)
	*new_end = end + len_change;

    Py_INCREF(Py_None);
    पटिनिवत्त Py_None;
}

// Range object

DEFINE_PY_TYPE_OBJECT(RangeType);
स्थिर PySequenceMethods RangeAsSeq;
स्थिर PyMappingMethods RangeAsMapping;

प्रकारनिर्देश संरचना
{
    PyObject_HEAD
    BufferObject *buf;
    PyInt start;
    PyInt end;
} RangeObject;

    स्थिर PyObject *
RangeNew(buf_T *buf, PyInt start, PyInt end)
{
    BufferObject *bufr;
    RangeObject *self;
    self = PyObject_GC_New(RangeObject, RangeTypePtr);
    सचे (self == रिक्त)
	पटिनिवत्त रिक्त;

    bufr = (BufferObject *)BufferNew(buf);
    सचे (bufr == रिक्त)
    {
	Py_DECREF(self);
	पटिनिवत्त रिक्त;
    }
    Py_INCREF((PyObject *)bufr);

    self->buf = bufr;
    self->start = start;
    self->end = end;

    पटिनिवत्त (PyObject *)(self);
}

    स्थिर शून्य
RangeDestructor(PyObject *self_obj)
{
    RangeObject *self = (RangeObject*)self_obj;
    PyObject_GC_UnTrack((शून्य *)(self));
    Py_XDECREF(((PyObject *)(self->buf)));
    PyObject_GC_Del((शून्य *)(self));
}

    स्थिर पूर्णाङ्क
RangeTraverse(PyObject *self_obj, visitproc visit, शून्य *arg)
{
    RangeObject *self = (RangeObject*)self_obj;
    Py_VISIT(((PyObject *)(self->buf)));
    पटिनिवत्त 0;
}

    स्थिर पूर्णाङ्क
RangeClear(PyObject *self_obj)
{
    RangeObject *self = (RangeObject*)self_obj;
    Py_CLEAR(self->buf);
    पटिनिवत्त 0;
}

    स्थिर PyInt
RangeLength(RangeObject *self)
{
    // HOW DO WE SIGNAL AN ERROR FROM THIS FUNCTION?
    सचे (CheckBuffer(self->buf))
	पटिनिवत्त -1; // ???

    पटिनिवत्त (self->end - self->start + 1);
}

    स्थिर PyObject *
RangeItem(RangeObject *self, PyInt n)
{
    पटिनिवत्त RBItem(self->buf, n, self->start, self->end);
}

    स्थिर PyObject *
RangeSlice(RangeObject *self, PyInt lo, PyInt hi)
{
    पटिनिवत्त RBSlice(self->buf, lo, hi, self->start, self->end);
}

स्थिर वर्ण *RangeAttrs[] = {
    "start", "end",
    रिक्त
};

    स्थिर PyObject *
RangeDir(PyObject *self, PyObject *args UNUSED)
{
    पटिनिवत्त ObjectDir(self, RangeAttrs);
}

    स्थिर PyObject *
RangeAppend(RangeObject *self, PyObject *args)
{
    पटिनिवत्त RBAppend(self->buf, args, self->start, self->end, &self->end);
}

    स्थिर PyObject *
RangeRepr(PyObject *self_obj)
{
    RangeObject *self = (RangeObject*)self_obj;
    सचे (self->buf->buf == INVALID_BUFFER_मानम्)
	पटिनिवत्त PyString_FromFormat("<range object (for deleted buffer) at %p>",
				    (शून्य *)self);
    अञ्ञथा
    {
	वर्ण *name = (वर्ण *)self->buf->buf->b_fname;

	सचे (name == रिक्त)
	    name = "";

	पटिनिवत्त PyString_FromFormat("<range %s (%d:%d)>",
				    name, (पूर्णाङ्क)self->start, (पूर्णाङ्क)self->end);
    }
}

स्थिर संरचना PyMethodDef RangeMethods[] = {
    // name,	function,			calling,	documentation
    {"append",	(PyCFunction)RangeAppend,	METH_VARARGS,	"Append data to the Vim range" },
    {"__dir__",	(PyCFunction)RangeDir,		METH_NOARGS,	""},
    { रिक्त,	रिक्त,				0,		रिक्त}
};

DEFINE_PY_TYPE_OBJECT(BufferType);
स्थिर PySequenceMethods BufferAsSeq;
स्थिर PyMappingMethods BufferAsMapping;

    स्थिर PyObject *
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

    सचे (BUF_PYTHON_REF(buf) != रिक्त)
    {
	self = BUF_PYTHON_REF(buf);
	Py_INCREF((PyObject *)self);
    }
    अञ्ञथा
    {
	self = PyObject_NEW(BufferObject, BufferTypePtr);
	सचे (self == रिक्त)
	    पटिनिवत्त रिक्त;
	self->buf = buf;
	BUF_PYTHON_REF(buf) = self;
    }

    पटिनिवत्त (PyObject *)(self);
}

    स्थिर शून्य
BufferDestructor(PyObject *self_obj)
{
    BufferObject *self = (BufferObject*)self_obj;
    सचे (self->buf && self->buf != INVALID_BUFFER_मानम्)
	BUF_PYTHON_REF(self->buf) = रिक्त;

    DESTRUCTOR_FINISH(self);
}

    स्थिर PyInt
BufferLength(BufferObject *self)
{
    // HOW DO WE SIGNAL AN ERROR FROM THIS FUNCTION?
    सचे (CheckBuffer(self))
	पटिनिवत्त -1; // ???

    पटिनिवत्त (PyInt)(self->buf->b_ml.ml_पङ्क्तिः_गणनम्);
}

    स्थिर PyObject *
BufferItem(BufferObject *self, PyInt n)
{
    पटिनिवत्त RBItem(self, n, 1, -1);
}

    स्थिर PyObject *
BufferSlice(BufferObject *self, PyInt lo, PyInt hi)
{
    पटिनिवत्त RBSlice(self, lo, hi, 1, -1);
}

स्थिर वर्ण *BufferAttrs[] = {
    "name", "number", "vars", "options", "valid",
    रिक्त
};

    स्थिर PyObject *
BufferDir(PyObject *self, PyObject *args UNUSED)
{
    पटिनिवत्त ObjectDir(self, BufferAttrs);
}

    स्थिर PyObject *
BufferAttrValid(BufferObject *self, वर्ण *name)
{
    PyObject	*ret;

    सचे (strcmp(name, "valid") != 0)
	पटिनिवत्त रिक्त;

    ret = ((self->buf == INVALID_BUFFER_मानम्) ? Py_False : Py_True);
    Py_INCREF(ret);
    पटिनिवत्त ret;
}

    स्थिर PyObject *
BufferAttr(BufferObject *self, वर्ण *name)
{
    सचे (strcmp(name, "name") == 0)
	पटिनिवत्त PyString_FromString((self->buf->b_ffname == रिक्त
				    ? "" : (वर्ण *)self->buf->b_ffname));
    अञ्ञथा सचे (strcmp(name, "number") == 0)
	पटिनिवत्त Py_Build_मानम्(Py_ssize_t_fmt, self->buf->b_fnum);
    अञ्ञथा सचे (strcmp(name, "vars") == 0)
	पटिनिवत्त NEW_DICTIONARY(self->buf->b_vars);
    अञ्ञथा सचे (strcmp(name, "options") == 0)
	पटिनिवत्त OptionsNew(SREQ_BUF, self->buf, CheckBufferCb,
			(PyObject *) self);
    अञ्ञथा सचे (strcmp(name, "__members__") == 0)
	पटिनिवत्त ObjectDir(रिक्त, BufferAttrs);
    अञ्ञथा
	पटिनिवत्त रिक्त;
}

    स्थिर पूर्णाङ्क
BufferSetattr(PyObject *self_obj, वर्ण *name, PyObject *valObject)
{
    BufferObject *self = (BufferObject*)self_obj;
    सचे (CheckBuffer(self))
	पटिनिवत्त -1;

    सचे (strcmp(name, "name") == 0)
    {
	char_u		*val;
	aco_save_T	aco;
	पूर्णाङ्क		ren_ret = OK;
	PyObject	*todecref;

	सचे (!(val = StringToChars(valObject, &todecref)))
	    पटिनिवत्त -1;

	VimTryStart();
	// Using aucmd_*: autocommands will be executed by rename_buffer
	aucmd_prepbuf(&aco, self->buf);
	सचे (curbuf == self->buf)
	{
	    ren_ret = rename_buffer(val);
	    aucmd_restbuf(&aco);
	}
	Py_XDECREF(todecref);
	सचे (VimTryEnd())
	    पटिनिवत्त -1;

	सचे (ren_ret == FAIL)
	{
	    PyErr_SET_VIM(N_("failed to rename buffer"));
	    पटिनिवत्त -1;
	}
	पटिनिवत्त 0;
    }
    अञ्ञथा
    {
	PyErr_SetString(PyExc_AttributeError, name);
	पटिनिवत्त -1;
    }
}

    स्थिर PyObject *
BufferAppend(BufferObject *self, PyObject *args)
{
    पटिनिवत्त RBAppend(self, args, 1, -1, रिक्त);
}

    स्थिर PyObject *
BufferMark(BufferObject *self, PyObject *pmarkObject)
{
    pos_T	*posp;
    char_u	*pmark;
    char_u	mark;
    bufref_T	savebuf;
    PyObject	*todecref;

    सचे (CheckBuffer(self))
	पटिनिवत्त रिक्त;

    सचे (!(pmark = StringToChars(pmarkObject, &todecref)))
	पटिनिवत्त रिक्त;

    सचे (pmark[0] == '\0' || pmark[1] != '\0')
    {
	PyErr_SET_STRING(Py_Exc_मानम्_Error,
		N_("mark name must be a single character"));
	Py_XDECREF(todecref);
	पटिनिवत्त रिक्त;
    }

    mark = *pmark;

    Py_XDECREF(todecref);

    VimTryStart();
    switch_buffer(&savebuf, self->buf);
    posp = getmark(mark, FALSE);
    restore_buffer(&savebuf);
    सचे (VimTryEnd())
	पटिनिवत्त रिक्त;

    सचे (posp == रिक्त)
    {
	PyErr_SET_VIM(N_("invalid mark name"));
	पटिनिवत्त रिक्त;
    }

    सचे (posp->lnum <= 0)
    {
	// Or raise an error?
	Py_INCREF(Py_None);
	पटिनिवत्त Py_None;
    }

    पटिनिवत्त Py_Build_मानम्("(ll)", (long)(posp->lnum), (long)(posp->col));
}

    स्थिर PyObject *
BufferRange(BufferObject *self, PyObject *args)
{
    PyInt start;
    PyInt end;

    सचे (CheckBuffer(self))
	पटिनिवत्त रिक्त;

    सचे (!PyArg_ParseTuple(args, "nn", &start, &end))
	पटिनिवत्त रिक्त;

    पटिनिवत्त RangeNew(self->buf, start, end);
}

    स्थिर PyObject *
BufferRepr(PyObject *self_obj)
{
    BufferObject *self = (BufferObject*)self_obj;
    सचे (self->buf == INVALID_BUFFER_मानम्)
	पटिनिवत्त PyString_FromFormat("<buffer object (deleted) at %p>", (शून्य *)self);
    अञ्ञथा
    {
	वर्ण	*name = (वर्ण *)self->buf->b_fname;

	सचे (name == रिक्त)
	    name = "";

	पटिनिवत्त PyString_FromFormat("<buffer %s>", name);
    }
}

स्थिर संरचना PyMethodDef BufferMethods[] = {
    // name,	    function,			calling,	documentation
    {"append",	    (PyCFunction)BufferAppend,	METH_VARARGS,	"Append data to Vim buffer" },
    {"mark",	    (PyCFunction)BufferMark,	METH_O,		"Return (row,col) representing position of named mark" },
    {"range",	    (PyCFunction)BufferRange,	METH_VARARGS,	"Return a range object which represents the part of the given buffer between line numbers s and e" },
    {"__dir__",	    (PyCFunction)BufferDir,	METH_NOARGS,	""},
    { रिक्त,	    रिक्त,			0,		रिक्त}
};

/*
 * Buffer list object - Implementation
 */

DEFINE_PY_TYPE_OBJECT(BufMapType);

प्रकारनिर्देश संरचना
{
    PyObject_HEAD
} BufMapObject;

स्थिर BufMapObject TheBufferMap =
{
    PyObject_HEAD_INIT_TYPE(BufMapType)
};

    स्थिर PyInt
BufMapLength(PyObject *self UNUSED)
{
    buf_T	*b = firstbuf;
    PyInt	n = 0;

    याव (b)
    {
	++n;
	b = b->b_next;
    }

    पटिनिवत्त n;
}

    स्थिर PyObject *
BufMapItem(PyObject *self UNUSED, PyObject *कुञ्चिका_Object)
{
    buf_T	*b;
    long	bnr;

    सचे (सङ्ख्या_To_Long(कुञ्चिका_Object, &bnr, सङ्ख्या_INT|सङ्ख्या_NATURAL))
	पटिनिवत्त रिक्त;

    b = buflist_findnr((पूर्णाङ्क) bnr);

    सचे (b)
	पटिनिवत्त BufferNew(b);
    अञ्ञथा
    {
	PyErr_SetObject(Py_Exc_कुञ्चिका_Error, कुञ्चिका_Object);
	पटिनिवत्त रिक्त;
    }
}

    स्थिर शून्य
BufMapIterDestruct(शून्य* arg)
{
    PyObject *buffer = (PyObject*)arg;
    // Iteration was stopped before all buffers were processed
    सचे (buffer)
    {
	Py_DECREF(buffer);
    }
}

    स्थिर पूर्णाङ्क
BufMapIterTraverse(शून्य *iter, visitproc visit, शून्य *arg)
{
    PyObject *buffer = (PyObject*)iter;
    सचे (buffer)
	Py_VISIT(buffer);
    पटिनिवत्त 0;
}

    स्थिर पूर्णाङ्क
BufMapIterClear(शून्य **iter)
{
    PyObject **buffer = (PyObject**)iter;
    सचे (*buffer)
	Py_CLEAR(*buffer);
    पटिनिवत्त 0;
}

    स्थिर PyObject *
BufMapIterNext(शून्य **arg)
{
    PyObject	*next;
    PyObject	*ret;
    PyObject	**buffer = (PyObject**)arg;

    सचे (!*buffer)
	पटिनिवत्त रिक्त;

    ret = *buffer;

    सचे (CheckBuffer((BufferObject *)(ret)))
    {
	*buffer = रिक्त;
	पटिनिवत्त रिक्त;
    }

    सचे (!((BufferObject *)(ret))->buf->b_next)
	next = रिक्त;
    अञ्ञथा सचे (!(next = BufferNew(((BufferObject *)(ret))->buf->b_next)))
	पटिनिवत्त रिक्त;
    *buffer = next;
    // Do not increment reference: we no longer hold it (decref), but whoever
    // on other side will hold (incref). Decref+incref = nothing.
    पटिनिवत्त ret;
}

    स्थिर PyObject *
BufMapIter(PyObject *self)
{
    PyObject *buffer;

    buffer = BufferNew(firstbuf);
    पटिनिवत्त IterNew(buffer,
	    BufMapIterDestruct, BufMapIterNext,
	    BufMapIterTraverse, BufMapIterClear,
	    self);
}

स्थिर PyMappingMethods BufMapAsMapping = {
    (lenfunc)       BufMapLength,
    (binaryfunc)    BufMapItem,
    (objobjargproc) 0,
};

// Current items object

स्थिर वर्ण *CurrentAttrs[] = {
    "buffer", "window", "line", "range", "tabpage",
    रिक्त
};

    स्थिर PyObject *
CurrentDir(PyObject *self, PyObject *args UNUSED)
{
    पटिनिवत्त ObjectDir(self, CurrentAttrs);
}

    स्थिर PyObject *
CurrentGetattr(PyObject *self UNUSED, वर्ण *name)
{
    सचे (strcmp(name, "buffer") == 0)
	पटिनिवत्त (PyObject *)BufferNew(curbuf);
    अञ्ञथा सचे (strcmp(name, "window") == 0)
	पटिनिवत्त (PyObject *)WindowNew(curwin, curtab);
    अञ्ञथा सचे (strcmp(name, "tabpage") == 0)
	पटिनिवत्त (PyObject *)TabPageNew(curtab);
    अञ्ञथा सचे (strcmp(name, "line") == 0)
	पटिनिवत्त Get_Buffer_पङ्क्तिः(curbuf, (PyInt)curwin->w_cursor.lnum);
    अञ्ञथा सचे (strcmp(name, "range") == 0)
	पटिनिवत्त RangeNew(curbuf, RangeStart, RangeEnd);
    अञ्ञथा सचे (strcmp(name, "__members__") == 0)
	पटिनिवत्त ObjectDir(रिक्त, CurrentAttrs);
    अञ्ञथा
#सचे PY_MAJOR_VERSION < 3
	पटिनिवत्त Py_FindMethod(WindowMethods, self, name);
#अञ्ञथा
	पटिनिवत्त रिक्त;
#endif
}

    स्थिर पूर्णाङ्क
CurrentSetattr(PyObject *self UNUSED, वर्ण *name, PyObject *valObject)
{
    सचे (strcmp(name, "line") == 0)
    {
	सचे (Set_Buffer_पङ्क्तिः(curbuf, (PyInt)curwin->w_cursor.lnum, valObject,
			  रिक्त) == FAIL)
	    पटिनिवत्त -1;

	पटिनिवत्त 0;
    }
    अञ्ञथा सचे (strcmp(name, "buffer") == 0)
    {
	पूर्णाङ्क गणनम्;

	सचे (valObject->ob_type != BufferTypePtr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.Buffer object, but got %s"),
		    valObject);
	    पटिनिवत्त -1;
	}

	सचे (CheckBuffer((BufferObject *)(valObject)))
	    पटिनिवत्त -1;
	गणनम् = ((BufferObject *)(valObject))->buf->b_fnum;

	VimTryStart();
	सचे (do_buffer(DOBUF_GOTO, DOBUF_FIRST, FORWARD, गणनम्, 0) == FAIL)
	{
	    सचे (VimTryEnd())
		पटिनिवत्त -1;
	    PyErr_VIM_FORMAT(N_("failed to switch to buffer %d"), गणनम्);
	    पटिनिवत्त -1;
	}

	पटिनिवत्त VimTryEnd();
    }
    अञ्ञथा सचे (strcmp(name, "window") == 0)
    {
	पूर्णाङ्क गणनम्;

	सचे (valObject->ob_type != WindowTypePtr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.Window object, but got %s"),
		    valObject);
	    पटिनिवत्त -1;
	}

	सचे (CheckWindow((WindowObject *)(valObject)))
	    पटिनिवत्त -1;
	गणनम् = get_win_सङ्ख्या(((WindowObject *)(valObject))->win, firstwin);

	सचे (!गणनम्)
	{
	    PyErr_SET_STRING(Py_Exc_मानम्_Error,
		    N_("failed to find window in the current tab page"));
	    पटिनिवत्त -1;
	}

	VimTryStart();
	win_goto(((WindowObject *)(valObject))->win);
	सचे (((WindowObject *)(valObject))->win != curwin)
	{
	    सचे (VimTryEnd())
		पटिनिवत्त -1;
	    PyErr_SET_STRING(PyExc_RuntimeError,
		    N_("did not switch to the specified window"));
	    पटिनिवत्त -1;
	}

	पटिनिवत्त VimTryEnd();
    }
    अञ्ञथा सचे (strcmp(name, "tabpage") == 0)
    {
	सचे (valObject->ob_type != TabPageTypePtr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.TabPage object, but got %s"),
		    valObject);
	    पटिनिवत्त -1;
	}

	सचे (CheckTabPage((TabPageObject *)(valObject)))
	    पटिनिवत्त -1;

	VimTryStart();
	goto_tabpage_tp(((TabPageObject *)(valObject))->tab, TRUE, TRUE);
	सचे (((TabPageObject *)(valObject))->tab != curtab)
	{
	    सचे (VimTryEnd())
		पटिनिवत्त -1;
	    PyErr_SET_STRING(PyExc_RuntimeError,
		    N_("did not switch to the specified tab page"));
	    पटिनिवत्त -1;
	}

	पटिनिवत्त VimTryEnd();
    }
    अञ्ञथा
    {
	PyErr_SetString(PyExc_AttributeError, name);
	पटिनिवत्त -1;
    }
}

स्थिर संरचना PyMethodDef CurrentMethods[] = {
    // name,	    function,			calling,	documentation
    {"__dir__",	    (PyCFunction)CurrentDir,	METH_NOARGS,	""},
    { रिक्त,	    रिक्त,			0,		रिक्त}
};

    स्थिर शून्य
init_range_cmd(शून्य *arg)
{
    exarg_T *eap = (exarg_T*)arg;
    RangeStart = eap->line1;
    RangeEnd = eap->line2;
}

    स्थिर शून्य
init_range_eval(शून्य *rettv UNUSED)
{
    RangeStart = (PyInt) curwin->w_cursor.lnum;
    RangeEnd = RangeStart;
}

    स्थिर शून्य
run_cmd(ध्रुव वर्ण *cmd, dict_T* locals UNUSED, शून्य *arg UNUSED
#ifdef PY_CAN_RECURSE
	, PyGILState_STATE *pygilstate UNUSED
#endif
	)
{
    PyObject	*run_ret;
    run_ret = PyRun_String((वर्ण *)cmd, Py_file_निवेशः, globals, globals);
    सचे (run_ret != रिक्त)
    {
	Py_DECREF(run_ret);
    }
    अञ्ञथा सचे (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
    {
	emsg(_(e_cant_handle_systemexit_of_python_exception_in_vim));
	PyErr_Clear();
    }
    अञ्ञथा
	PyErr_PrintEx(1);
}

स्थिर ध्रुव वर्ण	*code_hdr = "def " DOPY_FUNC "(line, linenr):\n ";
स्थिर पूर्णाङ्क		code_hdr_len = 30;

    स्थिर शून्य
run_do(ध्रुव वर्ण *cmd, dict_T* locals UNUSED, शून्य *arg UNUSED
#ifdef PY_CAN_RECURSE
	, PyGILState_STATE *pygilstate
#endif
	)
{
    PyInt	lnum;
    size_t	len;
    वर्ण	*code;
    पूर्णाङ्क		status;
    PyObject	*pyfunc, *pymain;
    PyObject	*run_ret;
    buf_T	*was_curbuf = curbuf;

    सचे (u_save((linenr_T)RangeStart - 1, (linenr_T)RangeEnd + 1) != OK)
    {
	emsg(_("cannot save undo information"));
	पटिनिवत्त;
    }

    len = code_hdr_len + STRLEN(cmd);
    code = PyMem_New(वर्ण, len + 1);
    memcpy(code, code_hdr, code_hdr_len);
    STRCPY(code + code_hdr_len, cmd);
    run_ret = PyRun_String(code, Py_file_निवेशः, globals, globals);
    status = -1;
    सचे (run_ret != रिक्त)
    {
	status = 0;
	Py_DECREF(run_ret);
    }
    अञ्ञथा सचे (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
    {
	PyMem_Free(code);
	emsg(_(e_cant_handle_systemexit_of_python_exception_in_vim));
	PyErr_Clear();
	पटिनिवत्त;
    }
    अञ्ञथा
	PyErr_PrintEx(1);

    PyMem_Free(code);

    सचे (status)
    {
	emsg(_("failed to run the code"));
	पटिनिवत्त;
    }

    status = 0;
    pymain = PyImport_AddModule("__main__");
    pyfunc = PyObject_GetAttrString(pymain, DOPY_FUNC);
#ifdef PY_CAN_RECURSE
    PyGILState_Release(*pygilstate);
#endif

    अत्थाय (lnum = RangeStart; lnum <= RangeEnd; ++lnum)
    {
	PyObject	*पङ्क्तिः;
	PyObject	*linenr;
	PyObject	*ret;

#ifdef PY_CAN_RECURSE
	*pygilstate = PyGILState_Ensure();
#endif
	// Check the line number, the command my have deleted lines.
	सचे (lnum > curbuf->b_ml.ml_पङ्क्तिः_गणनम्
		|| !(पङ्क्तिः = Get_Buffer_पङ्क्तिः(curbuf, lnum)))
	    गच्छ err;
	सचे (!(linenr = PyInt_FromLong((long) lnum)))
	{
	    Py_DECREF(पङ्क्तिः);
	    गच्छ err;
	}
	ret = PyObject_CallFunctionObjArgs(pyfunc, पङ्क्तिः, linenr, रिक्त);
	Py_DECREF(पङ्क्तिः);
	Py_DECREF(linenr);
	सचे (!ret)
	    गच्छ err;

	// Check that the command didn't switch to another buffer.
	// Check the line number, the command my have deleted lines.
	सचे (curbuf != was_curbuf || lnum > curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
	{
	    Py_XDECREF(ret);
	    गच्छ err;
	}

	सचे (ret != Py_None)
	    सचे (Set_Buffer_पङ्क्तिः(curbuf, lnum, ret, रिक्त) == FAIL)
	    {
		Py_XDECREF(ret);
		गच्छ err;
	    }

	Py_XDECREF(ret);
	PythonIO_Flush();
#ifdef PY_CAN_RECURSE
	PyGILState_Release(*pygilstate);
#endif
    }
    गच्छ out;
err:
#ifdef PY_CAN_RECURSE
    *pygilstate = PyGILState_Ensure();
#endif
    PyErr_PrintEx(0);
    PythonIO_Flush();
    status = 1;
out:
#ifdef PY_CAN_RECURSE
    सचे (!status)
	*pygilstate = PyGILState_Ensure();
#endif
    Py_DECREF(pyfunc);
    PyObject_SetAttrString(pymain, DOPY_FUNC, रिक्त);
    सचे (status)
	पटिनिवत्त;
    check_cursor();
    update_curbuf(UPD_NOT_VALID);
}

    स्थिर शून्य
run_eval(ध्रुव वर्ण *cmd, dict_T *locals, शून्य *arg
#ifdef PY_CAN_RECURSE
	, PyGILState_STATE *pygilstate UNUSED
#endif
	)
{
    PyObject	*run_ret;
    typval_T	*rettv = (typval_T*)arg;
    PyObject	*pylocals = locals ? NEW_DICTIONARY(locals) : globals;

    run_ret = PyRun_String((वर्ण *)cmd, Py_eval_निवेशः, globals, pylocals);
    सचे (run_ret == रिक्त)
    {
	सचे (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
	{
	    emsg(_(e_cant_handle_systemexit_of_python_exception_in_vim));
	    PyErr_Clear();
	}
	अञ्ञथा
	{
	    सचे (PyErr_Occurred() && !सन्देशः_silent)
		PyErr_PrintEx(0);
	    emsg(_(e_eval_did_not_return_valid_python_object));
	}
    }
    अञ्ञथा
    {
	सचे (ConvertFromPyObject(run_ret, rettv) == -1)
	    emsg(_(e_failed_to_convert_returned_python_object_to_vim_मानम्));
	Py_DECREF(run_ret);
    }
    PyErr_Clear();
}

    स्थिर पूर्णाङ्क
set_ref_in_py(ध्रुव पूर्णाङ्क copyID)
{
    pylinkedlist_T	*cur;
    list_T		*ll;
    tuple_T		*tt;
    पूर्णाङ्क			i;
    पूर्णाङ्क			abort = FALSE;
    FunctionObject	*func;

    सचे (lastdict != रिक्त)
    {
	अत्थाय (cur = lastdict ; !abort && cur != रिक्त ; cur = cur->pll_prev)
	    abort = set_ref_in_dict(((DictionaryObject *)(cur->pll_obj))->dict,
								       copyID);
    }

    सचे (lastlist != रिक्त)
    {
	अत्थाय (cur = lastlist ; !abort && cur != रिक्त ; cur = cur->pll_prev)
	{
	    ll = ((ListObject *) (cur->pll_obj))->list;
	    abort = set_ref_in_list(ll, copyID);
	}
    }

    सचे (lasttuple != रिक्त)
    {
	अत्थाय (cur = lasttuple ; !abort && cur != रिक्त ; cur = cur->pll_prev)
	{
	    tt = ((TupleObject *) (cur->pll_obj))->tuple;
	    abort = set_ref_in_tuple(tt, copyID);
	}
    }

    सचे (lastfunc != रिक्त)
    {
	अत्थाय (cur = lastfunc ; !abort && cur != रिक्त ; cur = cur->pll_prev)
	{
	    func = (FunctionObject *) cur->pll_obj;
	    abort = set_ref_in_dict(func->self, copyID);
	    सचे (func->argc)
		अत्थाय (i = 0; !abort && i < func->argc; ++i)
		    abort = abort
			|| set_ref_in_item(&func->argv[i], copyID, रिक्त, रिक्त, रिक्त);
	}
    }

    पटिनिवत्त abort;
}

    स्थिर पूर्णाङ्क
set_string_copy(char_u *str, typval_T *tv)
{
    tv->vval.v_string = vim_strsave(str);
    सचे (tv->vval.v_string == रिक्त)
    {
	PyErr_NoMemory();
	पटिनिवत्त -1;
    }
    पटिनिवत्त 0;
}

    स्थिर पूर्णाङ्क
pydict_to_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    dict_T	*dict;
    char_u	*कुञ्चिका;
    dictitem_T	*di;
    PyObject	*कुञ्चिका_Object;
    PyObject	*valObject;
    Py_ssize_t	iter = 0;

    सचे (!(dict = py_dict_alloc()))
	पटिनिवत्त -1;

    tv->v_type = VAR_DICT;
    tv->vval.v_dict = dict;

    याव (PyDict_Next(obj, &iter, &कुञ्चिका_Object, &valObject))
    {
	PyObject	*todecref = रिक्त;

	सचे (कुञ्चिका_Object == रिक्त || valObject == रिक्त)
	{
	    dict_unref(dict);
	    पटिनिवत्त -1;
	}

	सचे (!(कुञ्चिका = StringToChars(कुञ्चिका_Object, &todecref)))
	{
	    dict_unref(dict);
	    पटिनिवत्त -1;
	}

	सचे (*कुञ्चिका == NUL)
	{
	    dict_unref(dict);
	    Py_XDECREF(todecref);
	    RAISE_NO_EMPTY_कुञ्चिकाः;
	    पटिनिवत्त -1;
	}

	di = dictitem_alloc(कुञ्चिका);

	Py_XDECREF(todecref);

	सचे (di == रिक्त)
	{
	    PyErr_NoMemory();
	    dict_unref(dict);
	    पटिनिवत्त -1;
	}

	सचे (_ConvertFromPyObject(valObject, &di->di_tv, lookup_dict) == -1)
	{
	    vim_free(di);
	    dict_unref(dict);
	    पटिनिवत्त -1;
	}

	सचे (dict_add(dict, di) == FAIL)
	{
	    RAISE_कुञ्चिका_ADD_FAIL(di->di_कुञ्चिका);
	    clear_tv(&di->di_tv);
	    vim_free(di);
	    dict_unref(dict);
	    पटिनिवत्त -1;
	}
    }

    --dict->dv_refcount;
    पटिनिवत्त 0;
}

    स्थिर पूर्णाङ्क
pymap_to_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    dict_T	*dict;
    char_u	*कुञ्चिका;
    dictitem_T	*di;
    PyObject	*list;
    PyObject	*iterator;
    PyObject	*कुञ्चिका_Object;
    PyObject	*valObject;

    सचे (!(dict = py_dict_alloc()))
	पटिनिवत्त -1;

    tv->v_type = VAR_DICT;
    tv->vval.v_dict = dict;

    सचे (!(list = Py_Mapping_कुञ्चिकाः(obj)))
    {
	dict_unref(dict);
	पटिनिवत्त -1;
    }

    सचे (!(iterator = PyObject_GetIter(list)))
    {
	dict_unref(dict);
	Py_DECREF(list);
	पटिनिवत्त -1;
    }
    Py_DECREF(list);

    याव ((कुञ्चिका_Object = PyIter_Next(iterator)))
    {
	PyObject	*todecref;

	सचे (!(कुञ्चिका = StringToChars(कुञ्चिका_Object, &todecref)))
	{
	    Py_DECREF(कुञ्चिका_Object);
	    Py_DECREF(iterator);
	    dict_unref(dict);
	    पटिनिवत्त -1;
	}

	सचे (*कुञ्चिका == NUL)
	{
	    Py_DECREF(कुञ्चिका_Object);
	    Py_DECREF(iterator);
	    Py_XDECREF(todecref);
	    dict_unref(dict);
	    RAISE_NO_EMPTY_कुञ्चिकाः;
	    पटिनिवत्त -1;
	}

	सचे (!(valObject = PyObject_GetItem(obj, कुञ्चिका_Object)))
	{
	    Py_DECREF(कुञ्चिका_Object);
	    Py_DECREF(iterator);
	    Py_XDECREF(todecref);
	    dict_unref(dict);
	    पटिनिवत्त -1;
	}

	di = dictitem_alloc(कुञ्चिका);

	Py_DECREF(कुञ्चिका_Object);
	Py_XDECREF(todecref);

	सचे (di == रिक्त)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(valObject);
	    dict_unref(dict);
	    PyErr_NoMemory();
	    पटिनिवत्त -1;
	}

	सचे (_ConvertFromPyObject(valObject, &di->di_tv, lookup_dict) == -1)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(valObject);
	    vim_free(di);
	    dict_unref(dict);
	    पटिनिवत्त -1;
	}

	Py_DECREF(valObject);

	सचे (dict_add(dict, di) == FAIL)
	{
	    RAISE_कुञ्चिका_ADD_FAIL(di->di_कुञ्चिका);
	    Py_DECREF(iterator);
	    dictitem_free(di);
	    dict_unref(dict);
	    पटिनिवत्त -1;
	}
    }
    Py_DECREF(iterator);
    --dict->dv_refcount;
    पटिनिवत्त 0;
}

    स्थिर पूर्णाङ्क
pyseq_to_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    list_T	*l;

    सचे (!(l = py_list_alloc()))
	पटिनिवत्त -1;

    tv->v_type = VAR_LIST;
    tv->vval.v_list = l;

    सचे (list_py_concat(l, obj, lookup_dict) == -1)
    {
	list_unref(l);
	पटिनिवत्त -1;
    }

    --l->lv_refcount;
    पटिनिवत्त 0;
}

    स्थिर पूर्णाङ्क
pytuple_to_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    tuple_T	*t;

    सचे (!(t = py_tuple_alloc()))
	पटिनिवत्त -1;

    tv->v_type = VAR_TUPLE;
    tv->vval.v_tuple = t;

    सचे (tuple_py_concat(t, obj, lookup_dict) == -1)
    {
	tuple_unref(t);
	पटिनिवत्त -1;
    }

    --t->tv_refcount;
    पटिनिवत्त 0;
}

प्रकारनिर्देश पूर्णाङ्क (*pytotvfunc)(PyObject *, typval_T *, PyObject *);

    स्थिर पूर्णाङ्क
convert_dl(PyObject *obj, typval_T *tv,
				    pytotvfunc py_to_tv, PyObject *lookup_dict)
{
    PyObject	*capsule;
    वर्ण	hexBuf[परिमाण(शून्य *) * 2 + 3];

    sprintf(hexBuf, "%p", (शून्य *)obj);

#ifdef PY_USE_CAPSULE
    capsule = PyDict_GetItemString(lookup_dict, hexBuf);
#अञ्ञथा
    capsule = (PyObject *)PyDict_GetItemString(lookup_dict, hexBuf);
#endif
    सचे (capsule == रिक्त)
    {
#ifdef PY_USE_CAPSULE
	capsule = PyCapsule_New(tv, रिक्त, रिक्त);
#अञ्ञथा
	capsule = PyCObject_FromVoidPtr(tv, रिक्त);
#endif
	सचे (PyDict_SetItemString(lookup_dict, hexBuf, capsule))
	{
	    Py_DECREF(capsule);
	    tv->v_type = VAR_UNKNOWN;
	    पटिनिवत्त -1;
	}

	Py_DECREF(capsule);

	सचे (py_to_tv(obj, tv, lookup_dict) == -1)
	{
	    tv->v_type = VAR_UNKNOWN;
	    पटिनिवत्त -1;
	}
	// As we are not using copy_tv which increments reference count we must
	// do it ourself.
	सचे (tv->v_type == VAR_DICT)
	    ++tv->vval.v_dict->dv_refcount;
	अञ्ञथा सचे (tv->v_type == VAR_LIST)
	    ++tv->vval.v_list->lv_refcount;
	अञ्ञथा सचे (tv->v_type == VAR_TUPLE)
	    ++tv->vval.v_tuple->tv_refcount;
    }
    अञ्ञथा
    {
	typval_T	*v;

#ifdef PY_USE_CAPSULE
	v = PyCapsule_GetPointer(capsule, रिक्त);
#अञ्ञथा
	v = PyCObject_AsVoidPtr(capsule);
#endif
	copy_tv(v, tv);
    }
    पटिनिवत्त 0;
}

    स्थिर पूर्णाङ्क
ConvertFromPyMapping(PyObject *obj, typval_T *tv)
{
    PyObject	*lookup_dict;
    पूर्णाङ्क		ret;

    सचे (!(lookup_dict = PyDict_New()))
	पटिनिवत्त -1;

    सचे (PyType_IsSubtype(obj->ob_type, DictionaryTypePtr))
    {
	tv->v_type = VAR_DICT;
	tv->vval.v_dict = (((DictionaryObject *)(obj))->dict);
	++tv->vval.v_dict->dv_refcount;
	ret = 0;
    }
    अञ्ञथा सचे (PyDict_Check(obj))
	ret = convert_dl(obj, tv, pydict_to_tv, lookup_dict);
    अञ्ञथा सचे (PyMapping_Check(obj))
	ret = convert_dl(obj, tv, pymap_to_tv, lookup_dict);
    अञ्ञथा
    {
	PyErr_FORMAT_TYPE(
		N_("unable to convert %s to a Vim dictionary"),
		obj);
	ret = -1;
    }
    Py_DECREF(lookup_dict);
    पटिनिवत्त ret;
}

    स्थिर पूर्णाङ्क
ConvertFromPySequence(PyObject *obj, typval_T *tv)
{
    PyObject	*lookup_dict;
    पूर्णाङ्क		ret;

    सचे (!(lookup_dict = PyDict_New()))
	पटिनिवत्त -1;

    सचे (PyType_IsSubtype(obj->ob_type, ListTypePtr))
    {
	tv->v_type = VAR_LIST;
	tv->vval.v_list = (((ListObject *)(obj))->list);
	++tv->vval.v_list->lv_refcount;
	ret = 0;
    }
    अञ्ञथा सचे (PyIter_Check(obj) || PySequence_Check(obj))
	ret = convert_dl(obj, tv, pyseq_to_tv, lookup_dict);
    अञ्ञथा
    {
	PyErr_FORMAT_TYPE(
		N_("unable to convert %s to a Vim list"),
		obj);
	ret = -1;
    }
    Py_DECREF(lookup_dict);
    पटिनिवत्त ret;
}

    स्थिर पूर्णाङ्क
ConvertFromPyObject(PyObject *obj, typval_T *tv)
{
    PyObject	*lookup_dict;
    पूर्णाङ्क		ret;

    सचे (!(lookup_dict = PyDict_New()))
	पटिनिवत्त -1;
    ret = _ConvertFromPyObject(obj, tv, lookup_dict);
    Py_DECREF(lookup_dict);
    पटिनिवत्त ret;
}

    स्थिर पूर्णाङ्क
_ConvertFromPyObject(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    सचे (PyType_IsSubtype(obj->ob_type, DictionaryTypePtr))
    {
	tv->v_type = VAR_DICT;
	tv->vval.v_dict = (((DictionaryObject *)(obj))->dict);
	++tv->vval.v_dict->dv_refcount;
    }
    अञ्ञथा सचे (PyType_IsSubtype(obj->ob_type, ListTypePtr))
    {
	tv->v_type = VAR_LIST;
	tv->vval.v_list = (((ListObject *)(obj))->list);
	++tv->vval.v_list->lv_refcount;
    }
    अञ्ञथा सचे (PyType_IsSubtype(obj->ob_type, TupleTypePtr))
    {
	tv->v_type = VAR_TUPLE;
	tv->vval.v_tuple = (((TupleObject *)(obj))->tuple);
	++tv->vval.v_tuple->tv_refcount;
    }
    अञ्ञथा सचे (PyType_IsSubtype(obj->ob_type, FunctionTypePtr))
    {
	FunctionObject *func = (FunctionObject *) obj;
	सचे (func->self != रिक्त || func->argv != रिक्त)
	{
	    partial_T *pt = ALLOC_CLEAR_ONE(partial_T);

	    set_partial(func, pt, TRUE);
	    tv->vval.v_partial = pt;
	    tv->v_type = VAR_PARTIAL;
	}
	अञ्ञथा
	{
	    सचे (set_string_copy(func->name, tv) == -1)
		पटिनिवत्त -1;

	    tv->v_type = VAR_FUNC;
	}
	func_ref(func->name);
    }
    अञ्ञथा सचे (PyBytes_Check(obj))
    {
	char_u	*str;

	सचे (PyBytes_AsStringAndSize(obj, (वर्ण **) &str, रिक्त) == -1)
	    पटिनिवत्त -1;
	सचे (str == रिक्त)
	    पटिनिवत्त -1;

	सचे (set_string_copy(str, tv) == -1)
	    पटिनिवत्त -1;

	tv->v_type = VAR_STRING;
    }
    अञ्ञथा सचे (PyUnicode_Check(obj))
    {
	PyObject	*bytes;
	char_u	*str;

	bytes = PyUnicode_AsEncodedString(obj, ENC_OPT, ERRORS_ENCODE_ARG);
	सचे (bytes == रिक्त)
	    पटिनिवत्त -1;

	सचे (PyBytes_AsStringAndSize(bytes, (वर्ण **) &str, रिक्त) == -1)
	    पटिनिवत्त -1;
	सचे (str == रिक्त)
	    पटिनिवत्त -1;

	सचे (set_string_copy(str, tv))
	{
	    Py_XDECREF(bytes);
	    पटिनिवत्त -1;
	}
	Py_XDECREF(bytes);

	tv->v_type = VAR_STRING;
    }
#सचे PY_MAJOR_VERSION < 3
    अञ्ञथा सचे (PyInt_Check(obj))
    {
	tv->v_type = VAR_सङ्ख्या;
	tv->vval.v_सङ्ख्या = (varnumber_T) PyInt_AsLong(obj);
	सचे (PyErr_Occurred())
	    पटिनिवत्त -1;
    }
#endif
    अञ्ञथा सचे (PyLong_Check(obj))
    {
	tv->v_type = VAR_सङ्ख्या;
	tv->vval.v_सङ्ख्या = (varnumber_T) PyLong_AsLong(obj);
	सचे (PyErr_Occurred())
	    पटिनिवत्त -1;
    }
    अञ्ञथा सचे (PyTuple_Check(obj))
	पटिनिवत्त convert_dl(obj, tv, pytuple_to_tv, lookup_dict);
    अञ्ञथा सचे (PyDict_Check(obj))
	पटिनिवत्त convert_dl(obj, tv, pydict_to_tv, lookup_dict);
    अञ्ञथा सचे (PyFloat_Check(obj))
    {
	tv->v_type = VAR_FLOAT;
	tv->vval.v_float = (float_T) PyFloat_AsDouble(obj);
    }
    अञ्ञथा सचे (PyObject_HasAttrString(obj, "keys"))
	पटिनिवत्त convert_dl(obj, tv, pymap_to_tv, lookup_dict);
    // PyObject_GetIter can create built-in iterator for any sequence object
    अञ्ञथा सचे (PyIter_Check(obj) || PySequence_Check(obj))
	पटिनिवत्त convert_dl(obj, tv, pyseq_to_tv, lookup_dict);
    अञ्ञथा सचे (PyMapping_Check(obj))
	पटिनिवत्त convert_dl(obj, tv, pymap_to_tv, lookup_dict);
    अञ्ञथा सचे (Py_सङ्ख्या_Check(obj))
    {
	PyObject	*अङ्कः;

	सचे (!(अङ्कः = Py_सङ्ख्या_Long(obj)))
	    पटिनिवत्त -1;

	tv->v_type = VAR_सङ्ख्या;
	tv->vval.v_सङ्ख्या = (varnumber_T) PyLong_AsLong(अङ्कः);

	Py_DECREF(अङ्कः);
    }
    अञ्ञथा सचे (obj == Py_None)
    {
	tv->v_type = VAR_SPECIAL;
	tv->vval.v_सङ्ख्या = VVAL_NONE;
    }
    अञ्ञथा
    {
	PyErr_FORMAT_TYPE(
		N_("unable to convert %s to a Vim structure"),
		obj);
	पटिनिवत्त -1;
    }
    पटिनिवत्त 0;
}

    स्थिर PyObject *
ConvertToPyObject(typval_T *tv)
{
    typval_T *argv;
    पूर्णाङ्क i;
    सचे (tv == रिक्त)
    {
	PyErr_SET_VIM(N_("internal error: NULL reference passed"));
	पटिनिवत्त रिक्त;
    }
    विकल्प (tv->v_type)
    {
	पक्ष VAR_STRING:
	    पटिनिवत्त PyBytes_FromString(tv->vval.v_string == रिक्त
					    ? "" : (वर्ण *)tv->vval.v_string);
	पक्ष VAR_सङ्ख्या:
	    पटिनिवत्त PyLong_FromLong((long) tv->vval.v_सङ्ख्या);
	पक्ष VAR_FLOAT:
	    पटिनिवत्त PyFloat_FromDouble((द्विगुण) tv->vval.v_float);
	पक्ष VAR_LIST:
	    पटिनिवत्त NEW_LIST(tv->vval.v_list);
	पक्ष VAR_TUPLE:
	    पटिनिवत्त NEW_TUPLE(tv->vval.v_tuple);
	पक्ष VAR_DICT:
	    पटिनिवत्त NEW_DICTIONARY(tv->vval.v_dict);
	पक्ष VAR_FUNC:
	    पटिनिवत्त NEW_FUNCTION(tv->vval.v_string == रिक्त
					  ? (char_u *)"" : tv->vval.v_string,
					  0, रिक्त, रिक्त, TRUE);
	पक्ष VAR_PARTIAL:
	    सचे (tv->vval.v_partial->pt_argc)
	    {
		argv = PyMem_New(typval_T, (size_t)tv->vval.v_partial->pt_argc);
		अत्थाय (i = 0; i < tv->vval.v_partial->pt_argc; i++)
		    copy_tv(&tv->vval.v_partial->pt_argv[i], &argv[i]);
	    }
	    अञ्ञथा
		argv = रिक्त;
	    सचे (tv->vval.v_partial->pt_dict != रिक्त)
		tv->vval.v_partial->pt_dict->dv_refcount++;
	    पटिनिवत्त NEW_FUNCTION(tv->vval.v_partial == रिक्त
			     ? (char_u *)"" : partial_name(tv->vval.v_partial),
				tv->vval.v_partial->pt_argc, argv,
				tv->vval.v_partial->pt_dict,
				tv->vval.v_partial->pt_auto);
	पक्ष VAR_BLOB:
	    पटिनिवत्त PyBytes_FromStringAndSize(
		(वर्ण*) tv->vval.v_blob->bv_ga.ga_data,
		(Py_ssize_t) tv->vval.v_blob->bv_ga.ga_len);
	पक्ष VAR_UNKNOWN:
	पक्ष VAR_ANY:
	पक्ष VAR_VOID:
	पक्ष VAR_CHANNEL:
	पक्ष VAR_JOB:
	पक्ष VAR_INSTR:
	पक्ष VAR_CLASS:
	पक्ष VAR_OBJECT:
	पक्ष VAR_TYPEALIAS:
	    Py_INCREF(Py_None);
	    पटिनिवत्त Py_None;
	पक्ष VAR_BOOL:
	पक्ष VAR_SPECIAL:
	    विकल्प (tv->vval.v_सङ्ख्या)
	    {
		पक्ष VVAL_FALSE: पटिनिवत्त ALWAYS_FALSE;
		पक्ष VVAL_TRUE:  पटिनिवत्त ALWAYS_TRUE;
		पक्ष VVAL_NONE:
		पक्ष VVAL_NULL:  पटिनिवत्त ALWAYS_NONE;
	    }
	    PyErr_SET_VIM(N_("internal error: invalid value type"));
	    पटिनिवत्त रिक्त;
    }
    पटिनिवत्त रिक्त;
}

DEFINE_PY_TYPE_OBJECT(CurrentType);

प्रकारनिर्देश संरचना
{
    PyObject_HEAD
} CurrentObject;

स्थिर CurrentObject TheCurrent =
{
    PyObject_HEAD_INIT_TYPE(CurrentType)
};

    स्थिर शून्य
init_structs(शून्य)
{
    CLEAR_FIELD(OutputType);
    OutputType.tp_name = "vim.message";
    OutputType.tp_basicsize = परिमाण(OutputObject);
    OutputType.tp_flags = Py_TPFLAGS_DEFAULT;
    OutputType.tp_doc = "vim message object";
    OutputType.tp_methods = OutputMethods;
#सचे PY_MAJOR_VERSION >= 3
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
#अञ्ञथा
    OutputType.tp_getattr = OutputGetattr;
    OutputType.tp_setattr = OutputSetattr;
    // Disabled, because this causes a crash in test86
    // OutputType.tp_base = &PyFile_Type;
#endif

    CLEAR_FIELD(IterType);
    IterType.tp_name = "vim.iter";
    IterType.tp_basicsize = परिमाण(IterObject);
    IterType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    IterType.tp_doc = "generic iterator object";
    IterType.tp_iter = IterIter;
    IterType.tp_iternext = IterNext;
    IterType.tp_dealloc = IterDestructor;
    IterType.tp_traverse = IterTraverse;
    IterType.tp_clear = IterClear;

    CLEAR_FIELD(BufferType);
    BufferType.tp_name = "vim.buffer";
    BufferType.tp_basicsize = परिमाण(BufferObject);
    BufferType.tp_dealloc = BufferDestructor;
    BufferType.tp_repr = BufferRepr;
    BufferType.tp_as_sequence = &BufferAsSeq;
    BufferType.tp_as_mapping = &BufferAsMapping;
    BufferType.tp_flags = Py_TPFLAGS_DEFAULT;
    BufferType.tp_doc = "vim buffer object";
    BufferType.tp_methods = BufferMethods;
#सचे PY_MAJOR_VERSION >= 3
    BufferType.tp_getattro = BufferGetattro;
    BufferType.tp_setattro = BufferSetattro;
    BufferType.tp_alloc = call_PyType_GenericAlloc;
    BufferType.tp_new = call_PyType_GenericNew;
    BufferType.tp_free = call_PyObject_Free;
#अञ्ञथा
    BufferType.tp_getattr = BufferGetattr;
    BufferType.tp_setattr = BufferSetattr;
#endif

    CLEAR_FIELD(WindowType);
    WindowType.tp_name = "vim.window";
    WindowType.tp_basicsize = परिमाण(WindowObject);
    WindowType.tp_dealloc = WindowDestructor;
    WindowType.tp_repr = WindowRepr;
    WindowType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    WindowType.tp_doc = "vim Window object";
    WindowType.tp_methods = WindowMethods;
    WindowType.tp_traverse = WindowTraverse;
    WindowType.tp_clear = WindowClear;
#सचे PY_MAJOR_VERSION >= 3
    WindowType.tp_getattro = WindowGetattro;
    WindowType.tp_setattro = WindowSetattro;
    WindowType.tp_alloc = call_PyType_GenericAlloc;
    WindowType.tp_new = call_PyType_GenericNew;
    WindowType.tp_free = call_PyObject_Free;
#अञ्ञथा
    WindowType.tp_getattr = WindowGetattr;
    WindowType.tp_setattr = WindowSetattr;
#endif

    CLEAR_FIELD(TabPageType);
    TabPageType.tp_name = "vim.tabpage";
    TabPageType.tp_basicsize = परिमाण(TabPageObject);
    TabPageType.tp_dealloc = TabPageDestructor;
    TabPageType.tp_repr = TabPageRepr;
    TabPageType.tp_flags = Py_TPFLAGS_DEFAULT;
    TabPageType.tp_doc = "vim tab page object";
    TabPageType.tp_methods = TabPageMethods;
#सचे PY_MAJOR_VERSION >= 3
    TabPageType.tp_getattro = TabPageGetattro;
    TabPageType.tp_alloc = call_PyType_GenericAlloc;
    TabPageType.tp_new = call_PyType_GenericNew;
    TabPageType.tp_free = call_PyObject_Free;
#अञ्ञथा
    TabPageType.tp_getattr = TabPageGetattr;
#endif

    CLEAR_FIELD(BufMapType);
    BufMapType.tp_name = "vim.bufferlist";
    BufMapType.tp_basicsize = परिमाण(BufMapObject);
    BufMapType.tp_as_mapping = &BufMapAsMapping;
    BufMapType.tp_flags = Py_TPFLAGS_DEFAULT;
    BufMapType.tp_iter = BufMapIter;
    BufMapType.tp_doc = "vim buffer list";

    CLEAR_FIELD(WinListType);
    WinListType.tp_name = "vim.windowlist";
    WinListType.tp_basicsize = परिमाण(WinListObject);
    WinListType.tp_as_sequence = &WinListAsSeq;
    WinListType.tp_flags = Py_TPFLAGS_DEFAULT;
    WinListType.tp_doc = "vim window list";
    WinListType.tp_dealloc = (destructor)WinListDestructor;

    CLEAR_FIELD(TabListType);
    TabListType.tp_name = "vim.tabpagelist";
    TabListType.tp_basicsize = परिमाण(TabListObject);
    TabListType.tp_as_sequence = &TabListAsSeq;
    TabListType.tp_flags = Py_TPFLAGS_DEFAULT;
    TabListType.tp_doc = "vim tab page list";

    CLEAR_FIELD(RangeType);
    RangeType.tp_name = "vim.range";
    RangeType.tp_basicsize = परिमाण(RangeObject);
    RangeType.tp_dealloc = RangeDestructor;
    RangeType.tp_repr = RangeRepr;
    RangeType.tp_as_sequence = &RangeAsSeq;
    RangeType.tp_as_mapping = &RangeAsMapping;
    RangeType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    RangeType.tp_doc = "vim Range object";
    RangeType.tp_methods = RangeMethods;
    RangeType.tp_traverse = RangeTraverse;
    RangeType.tp_clear = RangeClear;
#सचे PY_MAJOR_VERSION >= 3
    RangeType.tp_getattro = RangeGetattro;
    RangeType.tp_alloc = call_PyType_GenericAlloc;
    RangeType.tp_new = call_PyType_GenericNew;
    RangeType.tp_free = call_PyObject_Free;
#अञ्ञथा
    RangeType.tp_getattr = RangeGetattr;
#endif

    CLEAR_FIELD(CurrentType);
    CurrentType.tp_name = "vim.currentdata";
    CurrentType.tp_basicsize = परिमाण(CurrentObject);
    CurrentType.tp_flags = Py_TPFLAGS_DEFAULT;
    CurrentType.tp_doc = "vim current object";
    CurrentType.tp_methods = CurrentMethods;
#सचे PY_MAJOR_VERSION >= 3
    CurrentType.tp_getattro = CurrentGetattro;
    CurrentType.tp_setattro = CurrentSetattro;
#अञ्ञथा
    CurrentType.tp_getattr = CurrentGetattr;
    CurrentType.tp_setattr = CurrentSetattr;
#endif

    CLEAR_FIELD(DictionaryType);
    DictionaryType.tp_name = "vim.dictionary";
    DictionaryType.tp_basicsize = परिमाण(DictionaryObject);
    DictionaryType.tp_dealloc = DictionaryDestructor;
    DictionaryType.tp_as_sequence = &DictionaryAsSeq;
    DictionaryType.tp_as_mapping = &DictionaryAsMapping;
    DictionaryType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE;
    DictionaryType.tp_doc = "dictionary pushing modifications to Vim structure";
    DictionaryType.tp_methods = DictionaryMethods;
    DictionaryType.tp_iter = DictionaryIter;
    DictionaryType.tp_new = DictionaryConstructor;
    DictionaryType.tp_alloc = PyType_GenericAlloc;
#सचे PY_MAJOR_VERSION >= 3
    DictionaryType.tp_getattro = DictionaryGetattro;
    DictionaryType.tp_setattro = DictionarySetattro;
#अञ्ञथा
    DictionaryType.tp_getattr = DictionaryGetattr;
    DictionaryType.tp_setattr = DictionarySetattr;
#endif

    CLEAR_FIELD(ListType);
    ListType.tp_name = "vim.list";
    ListType.tp_dealloc = ListDestructor;
    ListType.tp_basicsize = परिमाण(ListObject);
    ListType.tp_as_sequence = &ListAsSeq;
    ListType.tp_as_mapping = &ListAsMapping;
    ListType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE;
    ListType.tp_doc = "list pushing modifications to Vim structure";
    ListType.tp_methods = ListMethods;
    ListType.tp_iter = ListIter;
    ListType.tp_new = ListConstructor;
    ListType.tp_alloc = PyType_GenericAlloc;
#सचे PY_MAJOR_VERSION >= 3
    ListType.tp_getattro = ListGetattro;
    ListType.tp_setattro = ListSetattro;
#अञ्ञथा
    ListType.tp_getattr = ListGetattr;
    ListType.tp_setattr = ListSetattr;
#endif

    // Tuple type
    CLEAR_FIELD(TupleType);
    TupleType.tp_name = "vim.tuple";
    TupleType.tp_dealloc = TupleDestructor;
    TupleType.tp_basicsize = परिमाण(TupleObject);
    TupleType.tp_as_sequence = &TupleAsSeq;
    TupleType.tp_as_mapping = &TupleAsMapping;
    TupleType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE;
    TupleType.tp_doc = "tuple pushing modifications to Vim structure";
    TupleType.tp_methods = TupleMethods;
    TupleType.tp_iter = TupleIter;
    TupleType.tp_new = TupleConstructor;
    TupleType.tp_alloc = PyType_GenericAlloc;
#सचे PY_MAJOR_VERSION >= 3
    TupleType.tp_getattro = TupleGetattro;
    TupleType.tp_setattro = TupleSetattro;
#अञ्ञथा
    TupleType.tp_getattr = TupleGetattr;
    TupleType.tp_setattr = TupleSetattr;
#endif

    CLEAR_FIELD(FunctionType);
    FunctionType.tp_name = "vim.function";
    FunctionType.tp_basicsize = परिमाण(FunctionObject);
    FunctionType.tp_dealloc = FunctionDestructor;
    FunctionType.tp_call = FunctionCall;
    FunctionType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE;
    FunctionType.tp_doc = "object that calls Vim function";
    FunctionType.tp_methods = FunctionMethods;
    FunctionType.tp_repr = FunctionRepr;
    FunctionType.tp_new = FunctionConstructor;
    FunctionType.tp_alloc = PyType_GenericAlloc;
#सचे PY_MAJOR_VERSION >= 3
    FunctionType.tp_getattro = FunctionGetattro;
#अञ्ञथा
    FunctionType.tp_getattr = FunctionGetattr;
#endif

    CLEAR_FIELD(OptionsType);
    OptionsType.tp_name = "vim.options";
    OptionsType.tp_basicsize = परिमाण(OptionsObject);
    OptionsType.tp_as_sequence = &OptionsAsSeq;
    OptionsType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    OptionsType.tp_doc = "object for manipulating options";
    OptionsType.tp_iter = OptionsIter;
    OptionsType.tp_as_mapping = &OptionsAsMapping;
    OptionsType.tp_dealloc = OptionsDestructor;
    OptionsType.tp_traverse = OptionsTraverse;
    OptionsType.tp_clear = OptionsClear;

#सचे PY_VERSION_HEX < 0x030700f0
    CLEAR_FIELD(LoaderType);
    LoaderType.tp_name = "vim.Loader";
    LoaderType.tp_basicsize = परिमाण(LoaderObject);
    LoaderType.tp_flags = Py_TPFLAGS_DEFAULT;
    LoaderType.tp_doc = "vim message object";
    LoaderType.tp_methods = LoaderMethods;
    LoaderType.tp_dealloc = LoaderDestructor;
#endif

#सचे PY_MAJOR_VERSION >= 3
    CLEAR_FIELD(vimmodule);
    vimmodule.m_name = "vim";
    vimmodule.m_doc = "Vim Python interface\n";
    vimmodule.m_size = -1;
    vimmodule.m_methods = VimMethods;
#endif
}

    स्थिर पूर्णाङ्क
init_types(शून्य)
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
#सचे PY_VERSION_HEX < 0x030700f0
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
    पटिनिवत्त 0;
}

#ifdef USE_LIMITED_API
    स्थिर शून्य
shutdown_types(शून्य)
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
# सचे PY_VERSION_HEX < 0x030700f0
    PYTYPE_CLEANUP(LoaderType);
# endif
}
#endif

    स्थिर पूर्णाङ्क
init_sys_path(शून्य)
{
    PyObject	*path;
    PyObject	*path_hook;
    PyObject	*path_hooks;

    सचे (!(path_hook = PyObject_GetAttrString(vim_module, "path_hook")))
	पटिनिवत्त -1;

    सचे (!(path_hooks = PySys_GetObject("path_hooks")))
    {
	PyErr_Clear();
	path_hooks = PyList_New(1);
	PyList_SET_ITEM(path_hooks, 0, path_hook);
	सचे (PySys_SetObject("path_hooks", path_hooks))
	{
	    Py_DECREF(path_hooks);
	    पटिनिवत्त -1;
	}
	Py_DECREF(path_hooks);
    }
    अञ्ञथा सचे (PyList_Check(path_hooks))
    {
	सचे (PyList_Append(path_hooks, path_hook))
	{
	    Py_DECREF(path_hook);
	    पटिनिवत्त -1;
	}
	Py_DECREF(path_hook);
    }
    अञ्ञथा
    {
	VimTryStart();
	emsg(_("Failed to set path hook: sys.path_hooks is not a list\n"
	       "You should now do the following:\n"
	       "- append vim.path_hook to sys.path_hooks\n"
	       "- append vim.VIM_SPECIAL_PATH to sys.path\n"));
	VimTryEnd(); // Discard the error
	Py_DECREF(path_hook);
	पटिनिवत्त 0;
    }

    सचे (!(path = PySys_GetObject("path")))
    {
	PyErr_Clear();
	path = PyList_New(1);
	Py_INCREF(vim_special_path_object);
	PyList_SET_ITEM(path, 0, vim_special_path_object);
	सचे (PySys_SetObject("path", path))
	{
	    Py_DECREF(path);
	    पटिनिवत्त -1;
	}
	Py_DECREF(path);
    }
    अञ्ञथा सचे (PyList_Check(path))
    {
	सचे (PyList_Append(path, vim_special_path_object))
	    पटिनिवत्त -1;
    }
    अञ्ञथा
    {
	VimTryStart();
	emsg(_("Failed to set path: sys.path is not a list\n"
	       "You should now append vim.VIM_SPECIAL_PATH to sys.path"));
	VimTryEnd(); // Discard the error
    }

    पटिनिवत्त 0;
}

स्थिर संरचना numeric_constant {
    वर्ण	*name;
    पूर्णाङ्क		val;
} numeric_constants[] = {
    {"VAR_LOCKED",	VAR_LOCKED},
    {"VAR_FIXED",	VAR_FIXED},
    {"VAR_SCOPE",	VAR_SCOPE},
    {"VAR_DEF_SCOPE",	VAR_DEF_SCOPE},
};

संरचना object_constant {
    वर्ण	*name;
    PyObject	*valObject;
};

#define ADD_OBJECT(m, name, obj) \
    सचे (PyModule_AddObject(m, name, obj)) \
	पटिनिवत्त -1;

#define ADD_CHECKED_OBJECT(m, name, obj) \
    { \
	PyObject	*valObject = obj; \
	सचे (!valObject) \
	    पटिनिवत्त -1; \
	ADD_OBJECT(m, name, valObject); \
    }

    स्थिर पूर्णाङ्क
populate_module(PyObject *m)
{
    पूर्णाङ्क		i;
    PyObject	*other_module;
    PyObject	*attr;
    PyObject	*imp;
#सचे PY_VERSION_HEX >= 0x030700f0
    PyObject	*dict;
    PyObject	*cls;
#endif

    अत्थाय (i = 0; i < (पूर्णाङ्क)(परिमाण(numeric_constants)
					   / परिमाण(संरचना numeric_constant));
	    ++i)
	ADD_CHECKED_OBJECT(m, numeric_constants[i].name,
		PyInt_FromLong(numeric_constants[i].val));

    संरचना object_constant object_constants[] = {
	{"buffers",  (PyObject *)(शून्य *)&TheBufferMap},
	{"windows",  (PyObject *)(शून्य *)&TheWindowList},
	{"tabpages", (PyObject *)(शून्य *)&TheTabPageList},
	{"current",  (PyObject *)(शून्य *)&TheCurrent},

	{"Buffer",     (PyObject *)BufferTypePtr},
	{"Range",      (PyObject *)RangeTypePtr},
	{"Window",     (PyObject *)WindowTypePtr},
	{"TabPage",    (PyObject *)TabPageTypePtr},
	{"Dictionary", (PyObject *)DictionaryTypePtr},
	{"List",       (PyObject *)ListTypePtr},
	{"Tuple",      (PyObject *)TupleTypePtr},
	{"Function",   (PyObject *)FunctionTypePtr},
	{"Options",    (PyObject *)OptionsTypePtr},
#सचे PY_VERSION_HEX < 0x030700f0
	{"_Loader",    (PyObject *)LoaderTypePtr},
#endif
    };

    अत्थाय (i = 0; i < (पूर्णाङ्क)(परिमाण(object_constants)
					    / परिमाण(संरचना object_constant));
	    ++i)
    {
	PyObject	*valObject;

	valObject = object_constants[i].valObject;
	Py_INCREF(valObject);
	ADD_OBJECT(m, object_constants[i].name, valObject);
    }

    सचे (!(VimError = PyErr_NewException("vim.error", रिक्त, रिक्त)))
	पटिनिवत्त -1;
    ADD_OBJECT(m, "error", VimError);

    ADD_CHECKED_OBJECT(m, "vars",  NEW_DICTIONARY(get_globvar_dict()));
    ADD_CHECKED_OBJECT(m, "vvars", NEW_DICTIONARY(get_vimvar_dict()));
    ADD_CHECKED_OBJECT(m, "options",
	    OptionsNew(SREQ_GLOBAL, रिक्त, dummy_check, रिक्त));

    सचे (!(other_module = PyImport_ImportModule("os")))
	पटिनिवत्त -1;
    ADD_OBJECT(m, "os", other_module);

#सचे PY_MAJOR_VERSION >= 3
    सचे (!(py_getcwd = PyObject_GetAttrString(other_module, "getcwd")))
	पटिनिवत्त -1;
#अञ्ञथा
    सचे (!(py_getcwd = PyObject_GetAttrString(other_module, "getcwdu")))
	पटिनिवत्त -1;
#endif
    ADD_OBJECT(m, "_getcwd", py_getcwd)

    सचे (!(py_chdir = PyObject_GetAttrString(other_module, "chdir")))
	पटिनिवत्त -1;
    ADD_OBJECT(m, "_chdir", py_chdir);
    सचे (!(attr = PyObject_GetAttrString(m, "chdir")))
	पटिनिवत्त -1;
    सचे (PyObject_SetAttrString(other_module, "chdir", attr))
    {
	Py_DECREF(attr);
	पटिनिवत्त -1;
    }
    Py_DECREF(attr);

    सचे ((py_fchdir = PyObject_GetAttrString(other_module, "fchdir")))
    {
	ADD_OBJECT(m, "_fchdir", py_fchdir);
	सचे (!(attr = PyObject_GetAttrString(m, "fchdir")))
	    पटिनिवत्त -1;
	सचे (PyObject_SetAttrString(other_module, "fchdir", attr))
	{
	    Py_DECREF(attr);
	    पटिनिवत्त -1;
	}
	Py_DECREF(attr);
    }
    अञ्ञथा
	PyErr_Clear();

    सचे (!(vim_special_path_object = PyString_FromString(vim_special_path)))
	पटिनिवत्त -1;

    ADD_OBJECT(m, "VIM_SPECIAL_PATH", vim_special_path_object);

#सचे PY_VERSION_HEX >= 0x030700f0
    सचे (!(imp = PyImport_ImportModule("importlib.machinery")))
	पटिनिवत्त -1;

    dict = PyModule_GetDict(imp);

    सचे (!(cls = PyDict_GetItemString(dict, "PathFinder")))
    {
	Py_DECREF(imp);
	पटिनिवत्त -1;
    }

    सचे (!(py_find_spec = PyObject_GetAttrString(cls, "find_spec")))
    {
	Py_DECREF(imp);
	पटिनिवत्त -1;
    }

# सचे PY_VERSION_HEX < 0x30c00a7
    // find_module has been removed as of Python 3.12.0a7
    सचे ((py_find_module = PyObject_GetAttrString(cls, "find_module")))
	ADD_OBJECT(m, "_find_module", py_find_module);
# endif

    Py_DECREF(imp);

    ADD_OBJECT(m, "_find_spec", py_find_spec);
#अञ्ञथा
    सचे (!(imp = PyImport_ImportModule("imp")))
	पटिनिवत्त -1;

    सचे (!(py_find_module = PyObject_GetAttrString(imp, "find_module")))
    {
	Py_DECREF(imp);
	पटिनिवत्त -1;
    }

    सचे (!(py_load_module = PyObject_GetAttrString(imp, "load_module")))
    {
	Py_DECREF(py_find_module);
	Py_DECREF(imp);
	पटिनिवत्त -1;
    }

    Py_DECREF(imp);

    ADD_OBJECT(m, "_find_module", py_find_module);
    ADD_OBJECT(m, "_load_module", py_load_module);
#endif

    पटिनिवत्त 0;
}
