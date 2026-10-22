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
প্রকারলেখ পূর্ণসংখ্যা Py_ssize_t;  // Python 2.4 and earlier don't have this type.
#endif

// Use values that are known to work, others may make Vim crash.
#define ENC_OPT (enc_utf8 ? "utf-8" : enc_dbcs ? "euc-jp" : (অক্ষর *)p_enc)
#define DOPY_FUNC "_vim_pydo"

স্থির ধ্রুবক অক্ষর *vim_special_path = "_vim_path_";

#define PyErr_SET_STRING(exc, str) PyErr_SetString(exc, _(str))
#define PyErr_SetVim(str) PyErr_SetString(VimError, str)
#define PyErr_SET_VIM(str) PyErr_SET_STRING(VimError, str)
#define PyErr_FORMAT(exc, str, arg) PyErr_Format(exc, _(str), arg)
#define PyErr_FORMAT2(exc, str, arg1, arg2) PyErr_Format(exc, _(str), arg1,arg2)
#define PyErr_VIM_FORMAT(str, arg) PyErr_FORMAT(VimError, str, arg)

#ifdef USE_LIMITED_API
// Limited Python API. Need to call only exposed functions and remap macros.
// PyTypeObject is an opaque struct.

প্রকারলেখ কাঠামো {
    lenfunc sq_দৈর্ঘ্য;
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

প্রকারলেখ কাঠামো {
    lenfunc mp_দৈর্ঘ্য;
    binaryfunc mp_subscript;
    objobjargproc mp_ass_subscript;
} PyMappingMethods;

// This struct emulates the concrete _typeobject struct to allow the code to
// work the same way in both limited and full Python APIs.
কাঠামো typeobject_wrapper {
    ধ্রুবক অক্ষর *tp_name;
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

    ধ্রুবক অক্ষর *tp_doc;

    traverseproc tp_traverse;

    inquiry tp_clear;

    getiterfunc tp_iter;
    iternextfunc tp_iternext;

    কাঠামো PyMethodDef *tp_methods;
    কাঠামো _typeobject *tp_base;
    allocfunc tp_alloc;
    newfunc tp_new;
    freefunc tp_free;
};

# define DEFINE_PY_TYPE_OBJECT(type) \
    স্থির কাঠামো typeobject_wrapper type; \
    স্থির PyTypeObject* type##Ptr = ফাঁকা

// PyObject_HEAD_INIT_TYPE and PyObject_FINISH_INIT_TYPE need to come in pairs
// We first initialize with NULL because the type is not allocated until
// init_types() is called later. It's in FINISH_INIT_TYPE where we fill the
// type in with the newly allocated type.
# define Py_Object_শীর্ষ_INIT_TYPE(type) Py_Object_শীর্ষ_INIT(ফাঁকা)
# define PyObject_FINISH_INIT_TYPE(obj, type) obj.ob_base.ob_type = type##Ptr

# define Py_TYPE_GET_TP_ALLOC(type) ((allocfunc)PyType_GetSlot(type, Py_tp_alloc))
# define Py_TYPE_GET_TP_METHODS(type) ((PyMethodDef *)PyType_GetSlot(type, Py_tp_methods))

// PyObject_NEW is not part of stable ABI, but PyObject_Malloc/Init are.
স্থির PyObject* Vim_PyObject_New(PyTypeObject *type, size_t objsize)
{
    PyObject *obj = (PyObject *)PyObject_Malloc(objsize);
    যদি (obj == ফাঁকা)
	ফেরত PyErr_NoMemory();
    ফেরত PyObject_Init(obj, type);
}
# undef PyObject_NEW
# define PyObject_NEW(type, typeobj) ((type *)Vim_PyObject_New(typeobj, আকার(type)))

// This is a somewhat convoluted because limited API doesn't expose an easy way
// to get the tp_name field, and so we have to manually reconstruct it as
// "__module__.__name__" (with __module__ omitted for builtins to emulate
// Python behavior). Also, some of the more convenient functions like
// PyUnicode_AsUTF8AndSize and PyType_GetQualName() are not available until
// late Python 3 versions, and won't be available if you set Py_LIMITED_API too
// low.
# define PyErr_FORMAT_TYPE(বার্তা, obj) \
    করো { \
	PyObject* qualname = PyObject_GetAttrString((PyObject*)(obj)->ob_type, "__qualname__"); \
	যদি (qualname == ফাঁকা) \
	{ \
	    PyErr_FORMAT(PyExc_TypeError, বার্তা, "(NULL)"); \
	    বিরতি; \
	} \
	PyObject* module = PyObject_GetAttrString((PyObject*)(obj)->ob_type, "__module__"); \
	PyObject* full; \
	যদি (module == ফাঁকা || PyUnicode_CompareWithASCIIString(module, "builtins") == 0 \
		|| PyUnicode_CompareWithASCIIString(module, "__main__") == 0) \
	{ \
	    full = qualname; \
	    Py_INCREF(full); \
	} \
	অন্যথা \
	    full = PyUnicode_FromFormat("%U.%U", module, qualname); \
	PyObject* full_bytes = PyUnicode_AsUTF8String(full); \
	ধ্রুবক অক্ষর* full_str = PyBytes_AsString(full_bytes); \
	full_str = full_str == ফাঁকা ? "(NULL)" : full_str; \
	PyErr_FORMAT(PyExc_TypeError, বার্তা, full_str); \
	Py_DECREF(qualname); \
	Py_XDECREF(module); \
	Py_XDECREF(full); \
	Py_XDECREF(full_bytes); \
    } যখন (0)

# define PyList_GET_ITEM(list, i) PyList_GetItem(list, i)
# define Py_List_GET_আকার(o) Py_List_আকার(o)

// PyList_SET_ITEM and PyList_SetItem have slightly different behaviors. The
// former will leave the old item dangling, and the latter will decref on it.
// Since we only use this on new lists, this difference doesn't matter.
# define PyList_SET_ITEM(list, i, item) PyList_SetItem(list, i, item)

# define PyTuple_GET_ITEM(o, pos) PyTuple_GetItem(o, pos)
# define Py_Tuple_GET_আকার(o) Py_Tuple_আকার(o)

# যদি Py_LIMITED_API < 0x03080000
// PyIter_check only became part of stable ABI in 3.8, and there is no easy way
// to check for it in the API. We simply return false as a compromise. This
// does mean we should avoid compiling with stable ABI < 3.8.
#  undef PyIter_Check
#  define PyIter_Check(obj) (FALSE)
# endif

স্থির PyTypeObject* AddHeapType(কাঠামো typeobject_wrapper* type_object)
{
    PyType_Spec type_spec;
    type_spec.name = type_object->tp_name;
    type_spec.basicsize = type_object->tp_basicsize;
    type_spec.itemsize = 0;
    type_spec.flags = type_object->tp_flags;

    // We just need to statically allocate a large enough buffer that can hold
    // all slots. We need to leave a null-terminated slot at the end.
    PyType_Slot slots[40] = { {0, ফাঁকা} };
    size_t slot_i = 0;

# define ADD_TP_SLOT(slot_name) \
    যদি (slot_i >= 40) ফেরত ফাঁকা; /* this should never happen */ \
    যদি (type_object->slot_name != ফাঁকা) \
    { \
	slots[slot_i].slot = Py_##slot_name; \
	slots[slot_i].pfunc = (শূন্য*)type_object->slot_name; \
	++slot_i; \
    }
# define ADD_TP_SUB_SLOT(sub_slot, slot_name) \
    যদি (slot_i >= 40) ফেরত ফাঁকা; /* this should never happen */ \
    যদি (type_object->sub_slot != ফাঁকা && type_object->sub_slot->slot_name != ফাঁকা) \
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

    ADD_TP_SUB_SLOT(tp_as_sequence, sq_দৈর্ঘ্য)
    ADD_TP_SUB_SLOT(tp_as_sequence, sq_concat)
    ADD_TP_SUB_SLOT(tp_as_sequence, sq_repeat)
    ADD_TP_SUB_SLOT(tp_as_sequence, sq_item)
    ADD_TP_SUB_SLOT(tp_as_sequence, sq_ass_item)
    ADD_TP_SUB_SLOT(tp_as_sequence, sq_contains)
    ADD_TP_SUB_SLOT(tp_as_sequence, sq_inplace_concat)
    ADD_TP_SUB_SLOT(tp_as_sequence, sq_inplace_repeat)

    ADD_TP_SUB_SLOT(tp_as_mapping, mp_দৈর্ঘ্য)
    ADD_TP_SUB_SLOT(tp_as_mapping, mp_subscript)
    ADD_TP_SUB_SLOT(tp_as_mapping, mp_ass_subscript)
# undef ADD_TP_SLOT
# undef ADD_TP_SUB_SLOT

    type_spec.slots = slots;

    PyObject* newtype = PyType_FromSpec(&type_spec);
    ফেরত (PyTypeObject*)newtype;
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
    যদি (type##Ptr == ফাঁকা) \
	ফেরত -1;
# define PYTYPE_CLEANUP(type) \
    type##Ptr = ফাঁকা;

// Limited API does not provide PyRun_* functions. Need to implement manually
// using PyCompile and PyEval.
স্থির PyObject* Vim_PyRun_String(ধ্রুবক অক্ষর *str, পূর্ণসংখ্যা start, PyObject *globals, PyObject *locals)
{
    // Just pass "" for filename for now.
    PyObject* compiled = Py_CompileString(str, "", start);
    যদি (compiled == ফাঁকা)
	ফেরত ফাঁকা;

    PyObject* eval_ফলাফল = PyEval_EvalCode(compiled, globals, locals);
    Py_DECREF(compiled);
    ফেরত eval_ফলাফল;
}
স্থির পূর্ণসংখ্যা Vim_PyRun_SimpleString(ধ্রুবক অক্ষর *str)
{
    // This function emulates CPython's implementation.
    PyObject* m = PyImport_AddModule("__main__");
    যদি (m == ফাঁকা)
	ফেরত -1;
    PyObject* d = PyModule_GetDict(m);
    PyObject* output = Vim_PyRun_String(str, Py_file_প্রবেশ, d, d);
    যদি (output == ফাঁকা)
    {
	PyErr_PrintEx(TRUE);
	ফেরত -1;
    }
    Py_DECREF(output);
    ফেরত 0;
}
# define PyRun_String Vim_PyRun_String
# define PyRun_SimpleString Vim_PyRun_SimpleString

#অন্যথা // !defined(USE_LIMITED_API)

// Full Python API. Can make use of structs and macros directly.
# define DEFINE_PY_TYPE_OBJECT(type) \
    স্থির PyTypeObject type; \
    স্থির PyTypeObject* type##Ptr = &type
# define Py_Object_শীর্ষ_INIT_TYPE(type) Py_Object_শীর্ষ_INIT(&type)

# define Py_TYPE_GET_TP_ALLOC(type) type->tp_alloc
# define Py_TYPE_GET_TP_METHODS(type) type->tp_methods

# define Py_TYPE_NAME(obj) ((obj)->ob_type->tp_name == ফাঁকা \
	? "(NULL)" \
	: (obj)->ob_type->tp_name)
# define PyErr_FORMAT_TYPE(বার্তা, obj) \
    PyErr_FORMAT(PyExc_TypeError, বার্তা, \
	    Py_TYPE_NAME(obj))

// Add a static type
# define PYTYPE_READY(type) \
    যদি (PyType_Ready(type##Ptr)) \
	ফেরত -1;

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
#define RAISE_INVALID_সূচক_TYPE(idx) \
    PyErr_FORMAT_TYPE(N_("index must be int or slice, not %s"), idx);

#define INVALID_বাফার_মান ((buf_T *)(-1))
#define INVALID_WINDOW_মান ((win_T *)(-1))
#define INVALID_TABPAGE_মান ((tabpage_T *)(-1))

প্রকারলেখ শূন্য (*rangeinitializer)(শূন্য *);
প্রকারলেখ শূন্য (*runner)(ধ্রুবক অক্ষর *, dict_T *, শূন্য *
#ifdef PY_CAN_RECURSE
	, PyGILState_STATE *
#endif
	);

স্থির পূর্ণসংখ্যা ConvertFromPyObject(PyObject *, typval_T *);
স্থির পূর্ণসংখ্যা _ConvertFromPyObject(PyObject *, typval_T *, PyObject *);
স্থির পূর্ণসংখ্যা ConvertFromPyMapping(PyObject *, typval_T *);
স্থির পূর্ণসংখ্যা ConvertFromPySequence(PyObject *, typval_T *);
স্থির PyObject *WindowNew(win_T *, tabpage_T *);
স্থির PyObject *বাফার_New (buf_T *);
স্থির PyObject *সারি_To_String(ধ্রুবক অক্ষর *);

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

	যদি (Py_Bytes_As_String_And_আকার(obj, (অক্ষর **) &str, ফাঁকা) == -1
		|| str == ফাঁকা)
	    ফেরত ফাঁকা;

	*todecref = ফাঁকা;
    }
    অন্যথা যদি (PyUnicode_Check(obj))
    {
	PyObject	*bytes;

	যদি (!(bytes = PyUnicode_AsEncodedString(obj, ENC_OPT,
							   ERRORS_ENCODE_ARG)))
	    ফেরত ফাঁকা;

	যদি (Py_Bytes_As_String_And_আকার(bytes, (অক্ষর **) &str, ফাঁকা) == -1
		|| str == ফাঁকা)
	{
	    Py_DECREF(bytes);
	    ফেরত ফাঁকা;
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
	ফেরত ফাঁকা;
    }

    ফেরত (char_u *) str;
}

#define সংখ্যা_LONG     1
#define সংখ্যা_INT      2
#define সংখ্যা_NATURAL  4
#define সংখ্যা_UNSIGNED 8

    স্থির পূর্ণসংখ্যা
সংখ্যা_To_Long(PyObject *obj, long *ফলাফল, পূর্ণসংখ্যা flags)
{
#যদি PY_MAJOR_VERSION < 3
    যদি (PyInt_Check(obj))
    {
	*ফলাফল = PyInt_AsLong(obj);
	যদি (PyErr_Occurred())
	    ফেরত -1;
    }
    অন্যথা
#endif
    যদি (PyLong_Check(obj))
    {
	*ফলাফল = PyLong_AsLong(obj);
	যদি (PyErr_Occurred())
	    ফেরত -1;
    }
    অন্যথা যদি (Py_সংখ্যা_Check(obj))
    {
	PyObject	*অঙ্ক;

	যদি (!(অঙ্ক = Py_সংখ্যা_Long(obj)))
	    ফেরত -1;

	*ফলাফল = PyLong_AsLong(অঙ্ক);

	Py_DECREF(অঙ্ক);

	যদি (PyErr_Occurred())
	    ফেরত -1;
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
	ফেরত -1;
    }

    যদি (flags & সংখ্যা_INT)
    {
	যদি (*ফলাফল > INT_MAX)
	{
	    PyErr_SET_STRING(PyExc_OverflowError,
		    N_("value is too large to fit into C int type"));
	    ফেরত -1;
	}
	অন্যথা যদি (*ফলাফল < INT_MIN)
	{
	    PyErr_SET_STRING(PyExc_OverflowError,
		    N_("value is too small to fit into C int type"));
	    ফেরত -1;
	}
    }

    যদি (flags & সংখ্যা_NATURAL)
    {
	যদি (*ফলাফল <= 0)
	{
	    PyErr_SET_STRING(Py_Exc_মান_Error,
		    N_("number must be greater than zero"));
	    ফেরত -1;
	}
    }
    অন্যথা যদি (flags & সংখ্যা_UNSIGNED)
    {
	যদি (*ফলাফল < 0)
	{
	    PyErr_SET_STRING(Py_Exc_মান_Error,
		    N_("number must be greater or equal to zero"));
	    ফেরত -1;
	}
    }

    ফেরত 0;
}

    স্থির পূর্ণসংখ্যা
add_string(PyObject *list, অক্ষর *s)
{
    PyObject	*string;

    যদি (!(string = PyString_FromString(s)))
	ফেরত -1;

    যদি (PyList_Append(list, string))
    {
	Py_DECREF(string);
	ফেরত -1;
    }

    Py_DECREF(string);
    ফেরত 0;
}

    স্থির PyObject *
ObjectDir(PyObject *self, অক্ষর **attributes)
{
    PyMethodDef	*method;
    অক্ষর	**attr;
    PyObject	*ret;

    যদি (!(ret = PyList_New(0)))
	ফেরত ফাঁকা;

    যদি (self)
	জন্য (method = Py_TYPE_GET_TP_METHODS(self->ob_type) ; method->ml_name != ফাঁকা ; ++method)
	    যদি (add_string(ret, (অক্ষর *)method->ml_name))
	    {
		Py_DECREF(ret);
		ফেরত ফাঁকা;
	    }

    জন্য (attr = attributes ; *attr ; ++attr)
	যদি (add_string(ret, *attr))
	{
	    Py_DECREF(ret);
	    ফেরত ফাঁকা;
	}

#যদি PY_MAJOR_VERSION < 3
    যদি (add_string(ret, "__members__"))
    {
	Py_DECREF(ret);
	ফেরত ফাঁকা;
    }
#endif

    ফেরত ret;
}

// Output buffer management

// Function to write a line, points to either msg() or emsg().
প্রকারলেখ পূর্ণসংখ্যা (*writefn)(অক্ষর *);

DEFINE_PY_TYPE_OBJECT(OutputType);

প্রকারলেখ কাঠামো
{
    Py_Object_শীর্ষ
    long softspace;
    long error;
} OutputObject;

স্থির অক্ষর *OutputAttrs[] = {
    "softspace",
    ফাঁকা
};

    স্থির PyObject *
OutputDir(PyObject *self, PyObject *args UNUSED)
{
    ফেরত ObjectDir(self, OutputAttrs);
}

    স্থির পূর্ণসংখ্যা
OutputSetattr(PyObject *self_obj, অক্ষর *name, PyObject *valObject)
{
    OutputObject *self = (OutputObject*)self_obj;
    যদি (valObject == ফাঁকা)
    {
	PyErr_SET_STRING(PyExc_AttributeError,
		N_("can't delete OutputObject attributes"));
	ফেরত -1;
    }

    যদি (strcmp(name, "softspace") == 0)
    {
	যদি (সংখ্যা_To_Long(valObject, &(self->softspace), সংখ্যা_UNSIGNED))
	    ফেরত -1;
	ফেরত 0;
    }

    PyErr_FORMAT(PyExc_AttributeError, N_("invalid attribute: %s"), name);
    ফেরত -1;
}

// Buffer IO, we write one whole line at a time.
স্থির garray_T io_ga = {0, 0, 1, 80, ফাঁকা};
স্থির writefn old_fn = ফাঁকা;

    স্থির শূন্য
PythonIO_Flush(শূন্য)
{
    যদি (old_fn != ফাঁকা && io_ga.ga_len > 0)
    {
	((অক্ষর *)io_ga.ga_তথ্য)[io_ga.ga_len] = NUL;
	// We don't know what emsg_severe should be here, so ... hope?
	old_fn((অক্ষর *)io_ga.ga_তথ্য);
    }
    io_ga.ga_len = 0;
}

    স্থির শূন্য
writer(writefn fn, char_u *str, PyInt n, পূর্ণসংখ্যা severe)
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
    যখন (n > 0 && (ptr = memchr(str, '\n', n)) != ফাঁকা
					  && (fn == (writefn)emsg || !got_int))
    {
	PyInt len = ptr - str;

	যদি (ga_grow(&io_ga, (পূর্ণসংখ্যা)(len + 1)) == FAIL)
	    বিরতি;

	mch_memmove(((অক্ষর *)io_ga.ga_তথ্য) + io_ga.ga_len, str, (size_t)len);
	((অক্ষর *)io_ga.ga_তথ্য)[io_ga.ga_len + len] = NUL;
	emsg_severe = severe;
	fn((অক্ষর *)io_ga.ga_তথ্য);
	str = ptr + 1;
	n -= len + 1;
	io_ga.ga_len = 0;
    }

    // Put the remaining text into io_ga for later printing.
    যদি (n > 0 && (fn == (writefn)emsg || !got_int)
					&& ga_grow(&io_ga, (পূর্ণসংখ্যা)(n + 1)) == OK)
    {
	mch_memmove(((অক্ষর *)io_ga.ga_তথ্য) + io_ga.ga_len, str, (size_t)n);
	io_ga.ga_len += (পূর্ণসংখ্যা)n;
    }
}

    স্থির পূর্ণসংখ্যা
write_output(OutputObject *self, PyObject *string)
{
    Py_ssize_t	len = 0;
    অক্ষর	*str = ফাঁকা;
    পূর্ণসংখ্যা		error = self->error;

    যদি (!PyArg_Parse(string, "et#", ENC_OPT, &str, &len))
	ফেরত -1;

    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    writer((writefn)(error ? emsg : বার্তা), (char_u *)str, len, error);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS
    PyMem_Free(str);

    ফেরত 0;
}

    স্থির PyObject *
OutputWrite(OutputObject *self, PyObject *string)
{
    যদি (write_output(self, string))
	ফেরত ফাঁকা;

    Py_INCREF(Py_None);
    ফেরত Py_None;
}

    স্থির PyObject *
OutputWritelines(OutputObject *self, PyObject *seq)
{
    PyObject	*iterator;
    PyObject	*item;

    যদি (!(iterator = PyObject_GetIter(seq)))
	ফেরত ফাঁকা;

    যখন ((item = Py_Iter_পরবর্তী(iterator)))
    {
	যদি (write_output(self, item))
	{
	    Py_DECREF(iterator);
	    Py_DECREF(item);
	    ফেরত ফাঁকা;
	}
	Py_DECREF(item);
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    যদি (PyErr_Occurred())
	ফেরত ফাঁকা;

    Py_INCREF(Py_None);
    ফেরত Py_None;
}

    স্থির PyObject *
AlwaysNone(PyObject *self UNUSED, PyObject *args UNUSED)
{
    // do nothing
    Py_INCREF(Py_None);
    ফেরত Py_None;
}
#define ALWAYS_NONE AlwaysNone(ফাঁকা, ফাঁকা)

    স্থির PyObject *
AlwaysFalse(PyObject *self UNUSED, PyObject *args UNUSED)
{
    // do nothing
    PyObject	*ret = Py_False;
    Py_INCREF(ret);
    ফেরত ret;
}
#define ALWAYS_FALSE AlwaysFalse(ফাঁকা, ফাঁকা)

    স্থির PyObject *
AlwaysTrue(PyObject *self UNUSED, PyObject *args UNUSED)
{
    // do nothing
    PyObject	*ret = Py_True;
    Py_INCREF(ret);
    ফেরত ret;
}
#define ALWAYS_TRUE AlwaysTrue(ফাঁকা, ফাঁকা)

/***************/

স্থির কাঠামো PyMethodDef OutputMethods[] = {
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
    { ফাঁকা,	    ফাঁকা,				0,		ফাঁকা}
};

স্থির OutputObject Output =
{
    Py_Object_শীর্ষ_INIT_TYPE(OutputType)
    0,
    0
};

স্থির OutputObject Error =
{
    Py_Object_শীর্ষ_INIT_TYPE(OutputType)
    0,
    1
};

    স্থির পূর্ণসংখ্যা
PythonIO_Init_io(শূন্য)
{
    যদি (PySys_SetObject("stdout", (PyObject *)(শূন্য *)&Output))
	ফেরত -1;
    যদি (PySys_SetObject("stderr", (PyObject *)(শূন্য *)&Error))
	ফেরত -1;

    যদি (PyErr_Occurred())
    {
	emsg(_(e_python_error_initialising_io_object));
	ফেরত -1;
    }

    ফেরত 0;
}

#যদি PY_VERSION_HEX < 0x030700f0
স্থির PyObject *call_load_module(অক্ষর *name, পূর্ণসংখ্যা len, PyObject *find_module_ফলাফল);

প্রকারলেখ কাঠামো
{
    Py_Object_শীর্ষ
    অক্ষর	*fullname;
    PyObject	*ফলাফল;
} LoaderObject;
DEFINE_PY_TYPE_OBJECT(LoaderType);

    স্থির শূন্য
LoaderDestructor(PyObject *self_obj)
{
    LoaderObject *self = (LoaderObject*)self_obj;
    vim_free(self->fullname);
    Py_XDECREF(self->ফলাফল);
    DESTRUCTOR_FINISH(self);
}

    স্থির PyObject *
LoaderLoadModule(LoaderObject *self, PyObject *args UNUSED)
{
    অক্ষর	*fullname = self->fullname;
    PyObject	*ফলাফল = self->ফলাফল;
    PyObject	*module;

    যদি (!fullname)
    {
	module = ফলাফল ? ফলাফল : Py_None;
	Py_INCREF(module);
	ফেরত module;
    }

    module = call_load_module(fullname, (পূর্ণসংখ্যা)STRLEN(fullname), ফলাফল);

    self->fullname = ফাঁকা;
    self->ফলাফল = module;

    vim_free(fullname);
    Py_DECREF(ফলাফল);

    যদি (!module)
    {
	যদি (PyErr_Occurred())
	    ফেরত ফাঁকা;

	Py_INCREF(Py_None);
	ফেরত Py_None;
    }

    Py_INCREF(module);
    ফেরত module;
}

স্থির কাঠামো PyMethodDef LoaderMethods[] = {
    // name,	    function,				calling,	doc
    {"load_module", (PyCFunction)LoaderLoadModule,	METH_VARARGS,	""},
    { ফাঁকা,	    ফাঁকা,				0,		ফাঁকা}
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

    স্থির পূর্ণসংখ্যা
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
	ফেরত -1;
    }
    অন্যথা যদি (বার্তা_list != ফাঁকা && *বার্তা_list != ফাঁকা)
    {
	পূর্ণসংখ্যা	should_free;
	অক্ষর	*বার্তা;

	বার্তা = get_exception_string(*বার্তা_list, ET_ERROR, ফাঁকা, &should_free);

	যদি (বার্তা == ফাঁকা)
	{
	    PyErr_NoMemory();
	    ফেরত -1;
	}

	PyErr_SetVim(বার্তা);

	free_global_msglist();

	যদি (should_free)
	    vim_free(বার্তা);

	ফেরত -1;
    }
    অন্যথা যদি (!did_throw)
	ফেরত (PyErr_Occurred() ? -1 : 0);
    // Python exception is preferred over Vim one; unlikely to occur though
    অন্যথা যদি (PyErr_Occurred())
    {
	discard_current_exception();
	ফেরত -1;
    }
    // Finally transform Vim script exception to python one
    অন্যথা
    {
	PyErr_SetVim((অক্ষর *)current_exception->মান);
	discard_current_exception();
	ফেরত -1;
    }
}

    স্থির পূর্ণসংখ্যা
VimCheckInterrupt(শূন্য)
{
    যদি (got_int)
    {
	PyErr_SetNone(PyExc_KeyboardInterrupt);
	ফেরত 1;
    }
    ফেরত 0;
}

// Vim module - Implementation

    স্থির PyObject *
VimCommand(PyObject *self UNUSED, PyObject *string)
{
    char_u	*cmd;
    PyObject	*ret;
    PyObject	*todecref;

    যদি (!(cmd = StringToChars(string, &todecref)))
	ফেরত ফাঁকা;

    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();

    VimTryStart();
    do_cmdline_cmd(cmd);
    update_screen(UPD_VALID);

    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    যদি (VimTryEnd())
	ret = ফাঁকা;
    অন্যথা
	ret = Py_None;

    Py_XINCREF(ret);
    Py_XDECREF(todecref);
    ফেরত ret;
}

/*
 * Function to translate a typval_T into a PyObject; this will recursively
 * translate lists/dictionaries into their Python equivalents.
 *
 * The depth parameter is to avoid infinite recursion, set it to 1 when
 * you call VimToPython.
 */
    স্থির PyObject *
VimToPython(typval_T *our_tv, পূর্ণসংখ্যা depth, PyObject *lookup_dict)
{
    PyObject	*ret;
    PyObject	*newObj;
    অক্ষর	ptrBuf[আকার(শূন্য *) * 2 + 3];

    // Avoid infinite recursion
    যদি (depth > 100)
    {
	Py_INCREF(Py_None);
	ret = Py_None;
	ফেরত ret;
    }

    // Check if we run into a recursive loop.  The item must be in lookup_dict
    // then and we can use it again.
    যদি ((our_tv->v_type == VAR_LIST && our_tv->vval.v_list != ফাঁকা)
	    || (our_tv->v_type == VAR_TUPLE && our_tv->vval.v_tuple != ফাঁকা)
	    || (our_tv->v_type == VAR_DICT && our_tv->vval.v_dict != ফাঁকা))
    {
	sprintf(ptrBuf, "%p",
		our_tv->v_type == VAR_LIST ? (শূন্য *)our_tv->vval.v_list
		    : our_tv->v_type == VAR_TUPLE ?
			(শূন্য *)our_tv->vval.v_tuple
			: (শূন্য *)our_tv->vval.v_dict);

	যদি ((ret = PyDict_GetItemString(lookup_dict, ptrBuf)))
	{
	    Py_INCREF(ret);
	    ফেরত ret;
	}
    }

    যদি (our_tv->v_type == VAR_STRING)
	ret = PyString_FromString(our_tv->vval.v_string == ফাঁকা
					? "" : (অক্ষর *)our_tv->vval.v_string);
    অন্যথা যদি (our_tv->v_type == VAR_সংখ্যা)
    {
	অক্ষর buf[NUMBUFLEN];

	// For backwards compatibility numbers are stored as strings.
	sprintf(buf, "%lld", (long long)our_tv->vval.v_সংখ্যা);
	ret = PyString_FromString((অক্ষর *)buf);
    }
    অন্যথা যদি (our_tv->v_type == VAR_FLOAT)
    {
	অক্ষর buf[NUMBUFLEN];

	sprintf(buf, "%f", our_tv->vval.v_float);
	ret = PyString_FromString((অক্ষর *)buf);
    }
    অন্যথা যদি (our_tv->v_type == VAR_LIST)
    {
	list_T		*list = our_tv->vval.v_list;
	listitem_T	*curr;

	যদি (list == ফাঁকা)
	    ফেরত ফাঁকা;

	যদি (!(ret = PyList_New(0)))
	    ফেরত ফাঁকা;

	যদি (PyDict_SetItemString(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    ফেরত ফাঁকা;
	}

	CHECK_LIST_MATERIALIZE(list);
	FOR_ALL_LIST_বস্তুসমূহ(list, curr)
	{
	    যদি (!(newObj = VimToPython(&curr->li_tv, depth + 1, lookup_dict)))
	    {
		Py_DECREF(ret);
		ফেরত ফাঁকা;
	    }
	    যদি (PyList_Append(ret, newObj))
	    {
		Py_DECREF(newObj);
		Py_DECREF(ret);
		ফেরত ফাঁকা;
	    }
	    Py_DECREF(newObj);
	}
    }
    অন্যথা যদি (our_tv->v_type == VAR_TUPLE)
    {
	tuple_T	*tuple = our_tv->vval.v_tuple;
	পূর্ণসংখ্যা	len;

	যদি (tuple == ফাঁকা)
	    ফেরত ফাঁকা;

	len = TUPLE_LEN(tuple);

	ret = PyTuple_New(len);
	যদি (ret == ফাঁকা)
	    ফেরত ফাঁকা;

	জন্য (পূর্ণসংখ্যা idx = 0; idx < len; idx++)
	{
	    typval_T	*item_tv = TUPLE_ITEM(tuple, idx);

	    newObj = VimToPython(item_tv, depth + 1, lookup_dict);
	    যদি (!newObj)
	    {
		Py_DECREF(ret);
		ফেরত ফাঁকা;
	    }
	    PyTuple_SetItem(ret, idx, newObj);
	}

	যদি (PyDict_SetItemString(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    ফেরত ফাঁকা;
	}
    }
    অন্যথা যদি (our_tv->v_type == VAR_DICT)
    {

	hashtab_T	*ht;
	long_u		todo;
	hashitem_T	*hi;
	dictitem_T	*di;

	যদি (our_tv->vval.v_dict == ফাঁকা)
	    ফেরত ফাঁকা;
	ht = &our_tv->vval.v_dict->dv_hashtab;

	যদি (!(ret = PyDict_New()))
	    ফেরত ফাঁকা;

	যদি (PyDict_SetItemString(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    ফেরত ফাঁকা;
	}

	todo = ht->ht_used;
	জন্য (hi = ht->ht_array; todo > 0; ++hi)
	{
	    যদি (!HASHITEM_EMPTY(hi))
	    {
		--todo;

		di = dict_lookup(hi);
		যদি (!(newObj = VimToPython(&di->di_tv, depth + 1, lookup_dict)))
		{
		    Py_DECREF(ret);
		    ফেরত ফাঁকা;
		}
		যদি (PyDict_SetItemString(ret, (অক্ষর *)hi->hi_চাবি, newObj))
		{
		    Py_DECREF(ret);
		    Py_DECREF(newObj);
		    ফেরত ফাঁকা;
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
	ফেরত ret;
    }
    অন্যথা যদি (our_tv->v_type == VAR_SPECIAL)
    {
	Py_INCREF(Py_None);
	ret = Py_None;
	ফেরত ret;
    }
    অন্যথা যদি (our_tv->v_type == VAR_BLOB)
	ret = Py_Bytes_From_String_And_আকার(
		(অক্ষর*) our_tv->vval.v_blob->bv_ga.ga_তথ্য,
		(Py_ssize_t) our_tv->vval.v_blob->bv_ga.ga_len);
    অন্যথা
    {
	Py_INCREF(Py_None);
	ret = Py_None;
    }

    ফেরত ret;
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
	ফেরত ফাঁকা;

    যদি (!(রাশিমালা = StringToChars(string, &todecref)))
	ফেরত ফাঁকা;

    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    VimTryStart();
    our_tv = eval_রাশিমালা(রাশিমালা, ফাঁকা);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    Py_XDECREF(todecref);

    যদি (VimTryEnd())
	ফেরত ফাঁকা;

    যদি (our_tv == ফাঁকা)
    {
	PyErr_SET_VIM(N_("invalid expression"));
	ফেরত ফাঁকা;
    }

    // Convert the Vim type into a Python type.  Create a dictionary that's
    // used to check for recursive loops.
    যদি (!(lookup_dict = PyDict_New()))
	ret = ফাঁকা;
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

    ফেরত ret;
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
	ফেরত ফাঁকা;

    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    VimTryStart();
    our_tv = eval_রাশিমালা(রাশিমালা, ফাঁকা);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    Py_XDECREF(todecref);

    যদি (VimTryEnd())
	ফেরত ফাঁকা;

    যদি (our_tv == ফাঁকা)
    {
	PyErr_SET_VIM(N_("invalid expression"));
	ফেরত ফাঁকা;
    }

    ret = ConvertToPyObject(our_tv);
    Py_BEGIN_ALLOW_THREADS
    Python_Lock_Vim();
    free_tv(our_tv);
    Python_Release_Vim();
    Py_END_ALLOW_THREADS

    ফেরত ret;
}

    স্থির PyObject *
VimStrwidth(PyObject *self UNUSED, PyObject *string)
{
    char_u	*str;
    PyObject	*todecref;
    পূর্ণসংখ্যা		len;

    যদি (!(str = StringToChars(string, &todecref)))
	ফেরত ফাঁকা;

    len = mb_string2cells(str, (পূর্ণসংখ্যা)STRLEN(str));

    Py_XDECREF(todecref);

    ফেরত PyLong_FromLong(len);
}

    স্থির PyObject *
_VimChdir(PyObject *_chdir, PyObject *args, PyObject *kwargs)
{
    PyObject	*ret;
    PyObject	*newwd;
    PyObject	*todecref;
    char_u	*new_dir;

    যদি (_chdir == ফাঁকা)
	ফেরত ফাঁকা;
    যদি (!(ret = PyObject_Call(_chdir, args, kwargs)))
	ফেরত ফাঁকা;

    যদি (!(newwd = PyObject_CallFunctionObjArgs(py_getcwd, ফাঁকা)))
    {
	Py_DECREF(ret);
	ফেরত ফাঁকা;
    }

    যদি (!(new_dir = StringToChars(newwd, &todecref)))
    {
	Py_DECREF(ret);
	Py_DECREF(newwd);
	ফেরত ফাঁকা;
    }

    VimTryStart();

    যদি (vim_chdir(new_dir))
    {
	Py_DECREF(ret);
	Py_DECREF(newwd);
	Py_XDECREF(todecref);

	যদি (VimTryEnd())
	    ফেরত ফাঁকা;

	PyErr_SET_VIM(N_("failed to change directory"));
	ফেরত ফাঁকা;
    }

    Py_DECREF(newwd);
    Py_XDECREF(todecref);

    post_chdir(CDSCOPE_GLOBAL);

    যদি (VimTryEnd())
    {
	Py_DECREF(ret);
	ফেরত ফাঁকা;
    }

    ফেরত ret;
}

    স্থির PyObject *
VimChdir(PyObject *self UNUSED, PyObject *args, PyObject *kwargs)
{
    ফেরত _VimChdir(py_chdir, args, kwargs);
}

    স্থির PyObject *
VimFchdir(PyObject *self UNUSED, PyObject *args, PyObject *kwargs)
{
    ফেরত _VimChdir(py_fchdir, args, kwargs);
}

প্রকারলেখ কাঠামো {
    PyObject	*callable;
    PyObject	*ফলাফল;
} map_rtp_তথ্য;

    স্থির শূন্য
map_rtp_callback(char_u *path, শূন্য *তথ্য_3)
{
    শূন্য	**তথ্য = (শূন্য **) তথ্য_3;
    PyObject	*pathObject;
    map_rtp_তথ্য	*mr_তথ্য = *((map_rtp_তথ্য **) তথ্য);

    যদি (!(pathObject = PyString_FromString((অক্ষর *)path)))
    {
	*তথ্য = ফাঁকা;
	ফেরত;
    }

    mr_তথ্য->ফলাফল = PyObject_CallFunctionObjArgs(mr_তথ্য->callable,
						   pathObject, ফাঁকা);

    Py_DECREF(pathObject);

    যদি (!mr_তথ্য->ফলাফল || mr_তথ্য->ফলাফল != Py_None)
	*তথ্য = ফাঁকা;
    অন্যথা
    {
	Py_DECREF(mr_তথ্য->ফলাফল);
	mr_তথ্য->ফলাফল = ফাঁকা;
    }
}

    স্থির PyObject *
VimForeachRTP(PyObject *self UNUSED, PyObject *callable)
{
    map_rtp_তথ্য	তথ্য;

    তথ্য.callable = callable;
    তথ্য.ফলাফল = ফাঁকা;

    do_in_runtimepath(ফাঁকা, 0, &map_rtp_callback, &তথ্য);

    যদি (তথ্য.ফলাফল == ফাঁকা)
    {
	যদি (PyErr_Occurred())
	    ফেরত ফাঁকা;
	অন্যথা
	{
	    Py_INCREF(Py_None);
	    ফেরত Py_None;
	}
    }
    ফেরত তথ্য.ফলাফল;
}

/*
 * _vim_runtimepath_ special path implementation.
 */

    স্থির শূন্য
map_finder_callback(char_u *path, শূন্য *তথ্য_3)
{
    শূন্য	**তথ্য = (শূন্য **) তথ্য_3;
    PyObject	*list = *((PyObject **) তথ্য);
    PyObject	*pathObject1, *pathObject2;
    অক্ষর	*pathbuf;
    size_t	pathlen;

    pathlen = STRLEN(path);

#যদি PY_MAJOR_VERSION < 3
# define PY_MAIN_DIR_STRING "python2"
#অন্যথা
# define PY_MAIN_DIR_STRING "python3"
#endif
#define PY_ALTERNATE_DIR_STRING "pythonx"

#define PYTHONX_STRING_দৈর্ঘ্য 7 // STRLEN("pythonx")
    যদি (!(pathbuf = PyMem_New(অক্ষর,
		    pathlen + STRLEN(PATHSEPSTR) + PYTHONX_STRING_দৈর্ঘ্য + 1)))
    {
	PyErr_NoMemory();
	*তথ্য = ফাঁকা;
	ফেরত;
    }

    mch_memmove(pathbuf, path, pathlen + 1);
    add_pathsep((char_u *) pathbuf);

    pathlen = STRLEN(pathbuf);
    mch_memmove(pathbuf + pathlen, PY_MAIN_DIR_STRING,
	    PYTHONX_STRING_দৈর্ঘ্য + 1);

    যদি (!(pathObject1 = PyString_FromString(pathbuf)))
    {
	*তথ্য = ফাঁকা;
	PyMem_Free(pathbuf);
	ফেরত;
    }

    mch_memmove(pathbuf + pathlen, PY_ALTERNATE_DIR_STRING,
	    PYTHONX_STRING_দৈর্ঘ্য + 1);

    যদি (!(pathObject2 = PyString_FromString(pathbuf)))
    {
	Py_DECREF(pathObject1);
	PyMem_Free(pathbuf);
	*তথ্য = ফাঁকা;
	ফেরত;
    }

    PyMem_Free(pathbuf);

    যদি (PyList_Append(list, pathObject1)
	    || PyList_Append(list, pathObject2))
	*তথ্য = ফাঁকা;

    Py_DECREF(pathObject1);
    Py_DECREF(pathObject2);
}

    স্থির PyObject *
Vim_GetPaths(PyObject *self UNUSED, PyObject *args UNUSED)
{
    PyObject	*ret;

    যদি (!(ret = PyList_New(0)))
	ফেরত ফাঁকা;

    do_in_runtimepath(ফাঁকা, 0, &map_finder_callback, ret);

    যদি (PyErr_Occurred())
    {
	Py_DECREF(ret);
	ফেরত ফাঁকা;
    }

    ফেরত ret;
}

#যদি PY_VERSION_HEX >= 0x030700f0
    স্থির PyObject *
FinderFindSpec(PyObject *self, PyObject *args)
{
    অক্ষর	*fullname;
    PyObject	*paths;
    PyObject	*target = Py_None;
    PyObject	*spec;

    যদি (!PyArg_ParseTuple(args, "s|O", &fullname, &target))
	ফেরত ফাঁকা;

    যদি (!(paths = Vim_GetPaths(self, ফাঁকা)))
	ফেরত ফাঁকা;

    spec = PyObject_CallFunction(py_find_spec, "sOO", fullname, paths, target);

    Py_DECREF(paths);

    যদি (!spec)
    {
	যদি (PyErr_Occurred())
	    ফেরত ফাঁকা;

	Py_INCREF(Py_None);
	ফেরত Py_None;
    }

    ফেরত spec;
}

    স্থির PyObject *
FinderFindModule(PyObject* self UNUSED, PyObject* args UNUSED)
{
    // Apparently returning None works.
    Py_INCREF(Py_None);
    ফেরত Py_None;
}
#অন্যথা
    স্থির PyObject *
call_load_module(অক্ষর *name, পূর্ণসংখ্যা len, PyObject *find_module_ফলাফল)
{
    PyObject	*fd, *pathname, *description;

    যদি (!PyTuple_Check(find_module_ফলাফল))
    {
	PyErr_FORMAT_TYPE(
		N_("expected 3-tuple as imp.find_module() result, but got %s"),
		find_module_ফলাফল);
	ফেরত ফাঁকা;
    }
    যদি (Py_Tuple_GET_আকার(find_module_ফলাফল) != 3)
    {
	PyErr_FORMAT(PyExc_TypeError,
		N_("expected 3-tuple as imp.find_module() result, but got "
		   "tuple of size %d"),
		(পূর্ণসংখ্যা) Py_Tuple_GET_আকার(find_module_ফলাফল));
	ফেরত ফাঁকা;
    }

    যদি (!(fd = PyTuple_GET_ITEM(find_module_ফলাফল, 0))
	    || !(pathname = PyTuple_GET_ITEM(find_module_ফলাফল, 1))
	    || !(description = PyTuple_GET_ITEM(find_module_ফলাফল, 2)))
    {
	PyErr_SET_STRING(PyExc_RuntimeError,
		N_("internal error: imp.find_module returned tuple with NULL"));
	ফেরত ফাঁকা;
    }

    ফেরত PyObject_CallFunction(py_load_module,
	    "s#OOO", name, len, fd, pathname, description);
}

    স্থির PyObject *
find_module(অক্ষর *fullname, অক্ষর *লেজ, PyObject *new_path)
{
    PyObject	*find_module_ফলাফল;
    PyObject	*module;
    অক্ষর	*dot;

    যদি ((dot = (অক্ষর *)vim_strchr((char_u *) লেজ, '.')))
    {
	/*
	 * There is a dot in the name: call find_module recursively without the
	 * first component
	 */
	PyObject	*newest_path;
	পূর্ণসংখ্যা		partlen = (পূর্ণসংখ্যা) (dot - 1 - লেজ);

	যদি (!(find_module_ফলাফল = PyObject_CallFunction(py_find_module,
			"s#O", লেজ, partlen, new_path)))
	{
	    যদি (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_ImportError))
		PyErr_Clear();
	    ফেরত ফাঁকা;
	}

	যদি (!(module = call_load_module(
			fullname,
			((পূর্ণসংখ্যা) (লেজ - fullname)) + partlen,
			find_module_ফলাফল)))
	{
	    Py_DECREF(find_module_ফলাফল);
	    ফেরত ফাঁকা;
	}

	Py_DECREF(find_module_ফলাফল);

	যদি (!(newest_path = PyObject_GetAttrString(module, "__path__")))
	{
	    Py_DECREF(module);
	    ফেরত ফাঁকা;
	}

	Py_DECREF(module);

	find_module_ফলাফল = find_module(fullname, dot + 1, newest_path);

	Py_DECREF(newest_path);

	ফেরত find_module_ফলাফল;
    }
    অন্যথা
    {
	যদি (!(find_module_ফলাফল = PyObject_CallFunction(py_find_module,
			"sO", লেজ, new_path)))
	{
	    যদি (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_ImportError))
		PyErr_Clear();
	    ফেরত ফাঁকা;
	}

	ফেরত find_module_ফলাফল;
    }
}

    স্থির PyObject *
FinderFindModule(PyObject *self, PyObject *args)
{
    অক্ষর	*fullname;
    PyObject	*ফলাফল;
    PyObject	*new_path;
    LoaderObject	*loader;

    যদি (!PyArg_ParseTuple(args, "s", &fullname))
	ফেরত ফাঁকা;

    যদি (!(new_path = Vim_GetPaths(self, ফাঁকা)))
	ফেরত ফাঁকা;

    ফলাফল = find_module(fullname, fullname, new_path);

    Py_DECREF(new_path);

    যদি (!ফলাফল)
    {
	যদি (PyErr_Occurred())
	    ফেরত ফাঁকা;

	Py_INCREF(Py_None);
	ফেরত Py_None;
    }

    যদি (!(fullname = (অক্ষর *)vim_strsave((char_u *)fullname)))
    {
	Py_DECREF(ফলাফল);
	PyErr_NoMemory();
	ফেরত ফাঁকা;
    }

    যদি (!(loader = PyObject_NEW(LoaderObject, LoaderTypePtr)))
    {
	vim_free(fullname);
	Py_DECREF(ফলাফল);
	ফেরত ফাঁকা;
    }

    loader->fullname = fullname;
    loader->ফলাফল = ফলাফল;

    ফেরত (PyObject *) loader;
}
#endif

    স্থির PyObject *
VimPathHook(PyObject *self UNUSED, PyObject *args)
{
    অক্ষর	*path;

    যদি (PyArg_ParseTuple(args, "s", &path)
	    && STRCMP(path, vim_special_path) == 0)
    {
	Py_INCREF(vim_module);
	ফেরত vim_module;
    }

    PyErr_Clear();
    PyErr_SetNone(PyExc_ImportError);
    ফেরত ফাঁকা;
}

/*
 * Vim module - Definitions
 */

স্থির কাঠামো PyMethodDef VimMethods[] = {
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
    { ফাঁকা,	    ফাঁকা,			0,				ফাঁকা}
};

/*
 * Generic iterator object
 */

DEFINE_PY_TYPE_OBJECT(IterType);

প্রকারলেখ PyObject *(*nextfun)(শূন্য **);
প্রকারলেখ শূন্য (*destructorfun)(শূন্য *);
প্রকারলেখ পূর্ণসংখ্যা (*traversefun)(শূন্য *, visitproc, শূন্য *);
প্রকারলেখ পূর্ণসংখ্যা (*clearfun)(শূন্য **);

// Main purpose of this object is removing the need for do python
// initialization (i.e. PyType_Ready and setting type attributes) for a big
// bunch of objects.
প্রকারলেখ কাঠামো
{
    Py_Object_শীর্ষ
    শূন্য *cur;
    nextfun পরবর্তী;
    destructorfun destruct;
    traversefun traverse;
    clearfun clear;
    PyObject *iter_object;
} IterObject;

    স্থির PyObject *
IterNew(শূন্য *start, destructorfun destruct, nextfun পরবর্তী, traversefun traverse,
	clearfun clear, PyObject *iter_object)
{
    IterObject *self;

    self = PyObject_GC_New(IterObject, IterTypePtr);
    self->cur = start;
    self->পরবর্তী = পরবর্তী;
    self->destruct = destruct;
    self->traverse = traverse;
    self->clear = clear;
    self->iter_object = iter_object;

    যদি (iter_object)
	Py_INCREF(iter_object);

    ফেরত (PyObject *)(self);
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

    স্থির পূর্ণসংখ্যা
IterTraverse(PyObject *self_obj, visitproc visit, শূন্য *arg)
{
    IterObject *self = (IterObject*)self_obj;
    যদি (self->traverse != ফাঁকা)
	ফেরত self->traverse(self->cur, visit, arg);
    অন্যথা
	ফেরত 0;
}

// Mac OSX defines clear() somewhere.
#ifdef clear
# undef clear
#endif

    স্থির পূর্ণসংখ্যা
IterClear(PyObject *self_obj)
{
    IterObject *self = (IterObject*)self_obj;
    যদি (self->clear != ফাঁকা)
	ফেরত self->clear(&self->cur);
    অন্যথা
	ফেরত 0;
}

    স্থির PyObject *
Iter_পরবর্তী(PyObject *self_obj)
{
    IterObject *self = (IterObject*)self_obj;
    ফেরত self->পরবর্তী(&self->cur);
}

    স্থির PyObject *
IterIter(PyObject *self)
{
    Py_INCREF(self);
    ফেরত self;
}

প্রকারলেখ কাঠামো pylinkedlist_S {
    কাঠামো pylinkedlist_S	*pll_পরবর্তী;
    কাঠামো pylinkedlist_S	*pll_পূর্ব;
    PyObject			*pll_obj;
} pylinkedlist_T;

স্থির pylinkedlist_T *lastdict = ফাঁকা;
স্থির pylinkedlist_T *lastlist = ফাঁকা;
স্থির pylinkedlist_T *lasttuple = ফাঁকা;
স্থির pylinkedlist_T *lastfunc = ফাঁকা;

    স্থির শূন্য
pyll_remove(pylinkedlist_T *ref, pylinkedlist_T **last)
{
    যদি (ref->pll_পূর্ব == ফাঁকা)
    {
	যদি (ref->pll_পরবর্তী == ফাঁকা)
	{
	    *last = ফাঁকা;
	    ফেরত;
	}
    }
    অন্যথা
	ref->pll_পূর্ব->pll_পরবর্তী = ref->pll_পরবর্তী;

    যদি (ref->pll_পরবর্তী == ফাঁকা)
	*last = ref->pll_পূর্ব;
    অন্যথা
	ref->pll_পরবর্তী->pll_পূর্ব = ref->pll_পূর্ব;
}

    স্থির শূন্য
pyll_add(PyObject *self, pylinkedlist_T *ref, pylinkedlist_T **last)
{
    যদি (*last == ফাঁকা)
	ref->pll_পূর্ব = ফাঁকা;
    অন্যথা
    {
	(*last)->pll_পরবর্তী = ref;
	ref->pll_পূর্ব = *last;
    }
    ref->pll_পরবর্তী = ফাঁকা;
    ref->pll_obj = self;
    *last = ref;
}

DEFINE_PY_TYPE_OBJECT(DictionaryType);

প্রকারলেখ কাঠামো
{
    Py_Object_শীর্ষ
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
    যদি (self == ফাঁকা)
	ফেরত ফাঁকা;
    self->dict = dict;
    ++dict->dv_refcount;

    pyll_add((PyObject *)(self), &self->ref, &lastdict);

    ফেরত (PyObject *)(self);
}

    স্থির dict_T *
py_dict_alloc(শূন্য)
{
    dict_T	*ret;

    যদি (!(ret = dict_alloc()))
    {
	PyErr_NoMemory();
	ফেরত ফাঁকা;
    }
    ++ret->dv_refcount;

    ফেরত ret;
}

    স্থির PyObject *
DictionaryConstructor(PyTypeObject *subtype, PyObject *args, PyObject *kwargs)
{
    DictionaryObject	*self;
    dict_T	*dict;

    যদি (!(dict = py_dict_alloc()))
	ফেরত ফাঁকা;

    self = (DictionaryObject *) DictionaryNew(subtype, dict);

    --dict->dv_refcount;

    যদি (kwargs || Py_Tuple_আকার(args))
    {
	PyObject	*tmp;
	যদি (!(tmp = DictionaryUpdate(self, args, kwargs)))
	{
	    Py_DECREF(self);
	    ফেরত ফাঁকা;
	}

	Py_DECREF(tmp);
    }

    ফেরত (PyObject *)(self);
}

    স্থির শূন্য
DictionaryDestructor(PyObject *self_obj)
{
    DictionaryObject *self = (DictionaryObject*)self_obj;
    pyll_remove(&self->ref, &lastdict);
    dict_unref(self->dict);

    DESTRUCTOR_FINISH(self);
}

স্থির অক্ষর *DictionaryAttrs[] = {
    "locked", "scope",
    ফাঁকা
};

    স্থির PyObject *
DictionaryDir(PyObject *self, PyObject *args UNUSED)
{
    ফেরত ObjectDir(self, DictionaryAttrs);
}

    স্থির পূর্ণসংখ্যা
DictionarySetattr(PyObject *self_obj, অক্ষর *name, PyObject *valObject)
{
    DictionaryObject *self = (DictionaryObject*)self_obj;
    যদি (valObject == ফাঁকা)
    {
	PyErr_SET_STRING(PyExc_AttributeError,
		N_("cannot delete vim.Dictionary attributes"));
	ফেরত -1;
    }

    যদি (strcmp(name, "locked") == 0)
    {
	যদি (self->dict->dv_lock == VAR_FIXED)
	{
	    PyErr_SET_STRING(PyExc_TypeError,
		    N_("cannot modify fixed dictionary"));
	    ফেরত -1;
	}
	অন্যথা
	{
	    পূর্ণসংখ্যা		istrue = PyObject_IsTrue(valObject);
	    যদি (istrue == -1)
		ফেরত -1;
	    অন্যথা যদি (istrue)
		self->dict->dv_lock = VAR_LOCKED;
	    অন্যথা
		self->dict->dv_lock = 0;
	}
	ফেরত 0;
    }
    অন্যথা
    {
	PyErr_FORMAT(PyExc_AttributeError, N_("cannot set attribute %s"), name);
	ফেরত -1;
    }
}

    স্থির PyInt
Dictionary_দৈর্ঘ্য(DictionaryObject *self)
{
    ফেরত ((PyInt) (self->dict->dv_hashtab.ht_used));
}

#define DICT_FLAG_HAS_DEFAULT	0x01
#define DICT_FLAG_বের_করো		0x02
#define DICT_FLAG_NONE_DEFAULT	0x04
#define DICT_FLAG_RETURN_BOOL	0x08 // Incompatible with DICT_FLAG_POP
#define DICT_FLAG_RETURN_PAIR	0x10

    স্থির PyObject *
_DictionaryItem(DictionaryObject *self, PyObject *args, পূর্ণসংখ্যা flags)
{
    PyObject	*চাবি_Object;
    PyObject	*defObject = ((flags & DICT_FLAG_NONE_DEFAULT)? Py_None : ফাঁকা);
    PyObject	*ret;
    char_u	*চাবি;
    dictitem_T	*di;
    dict_T	*dict = self->dict;
    hashitem_T	*hi;
    PyObject	*todecref;

    যদি (flags & DICT_FLAG_HAS_DEFAULT)
    {
	যদি (!PyArg_ParseTuple(args, "O|O", &চাবি_Object, &defObject))
	    ফেরত ফাঁকা;
    }
    অন্যথা
	চাবি_Object = args;

    যদি (flags & DICT_FLAG_RETURN_BOOL)
	defObject = Py_False;

    যদি (!(চাবি = StringToChars(চাবি_Object, &todecref)))
	ফেরত ফাঁকা;

    যদি (*চাবি == NUL)
    {
	RAISE_NO_EMPTY_চাবিগুলি;
	Py_XDECREF(todecref);
	ফেরত ফাঁকা;
    }

    hi = hash_find(&dict->dv_hashtab, চাবি);

    Py_XDECREF(todecref);

    যদি (HASHITEM_EMPTY(hi))
    {
	যদি (defObject)
	{
	    Py_INCREF(defObject);
	    ফেরত defObject;
	}
	অন্যথা
	{
	    PyErr_SetObject(Py_Exc_চাবি_Error, চাবি_Object);
	    ফেরত ফাঁকা;
	}
    }
    অন্যথা যদি (flags & DICT_FLAG_RETURN_BOOL)
    {
	ret = Py_True;
	Py_INCREF(ret);
	ফেরত ret;
    }

    di = dict_lookup(hi);

    যদি (!(ret = ConvertToPyObject(&di->di_tv)))
	ফেরত ফাঁকা;

    যদি (flags & DICT_FLAG_বের_করো)
    {
	যদি (dict->dv_lock)
	{
	    RAISE_LOCKED_DICTIONARY;
	    Py_DECREF(ret);
	    ফেরত ফাঁকা;
	}

	hash_remove(&dict->dv_hashtab, hi, "Python remove variable");
	dictitem_free(di);
    }

    ফেরত ret;
}

    স্থির PyObject *
DictionaryItem(DictionaryObject *self, PyObject *চাবি_Object)
{
    ফেরত _DictionaryItem(self, চাবি_Object, 0);
}

    স্থির পূর্ণসংখ্যা
DictionaryContains(DictionaryObject *self, PyObject *চাবি_Object)
{
    PyObject	*rObj = _DictionaryItem(self, চাবি_Object, DICT_FLAG_RETURN_BOOL);
    পূর্ণসংখ্যা		ret;

    যদি (rObj == ফাঁকা)
	ফেরত -1;

    ret = (rObj == Py_True);

    Py_DECREF(rObj);

    ফেরত ret;
}

প্রকারলেখ কাঠামো
{
    পূর্ণসংখ্যা		dii_changed;
    hashtab_T	*dii_ht;
    hashitem_T	*dii_hi;
    long_u	dii_todo;
} dictiterinfo_T;

    স্থির PyObject *
Dictionary_Iter_পরবর্তী(শূন্য **arg)
{
    PyObject	*ret;
    dictiterinfo_T **dii = (dictiterinfo_T**)arg;

    যদি (!(*dii)->dii_todo)
	ফেরত ফাঁকা;

    যদি ((*dii)->dii_ht->ht_changed != (*dii)->dii_changed)
    {
	PyErr_SET_STRING(PyExc_RuntimeError,
		N_("hashtab changed during iteration"));
	ফেরত ফাঁকা;
    }

    যখন (((*dii)->dii_todo) && HASHITEM_EMPTY((*dii)->dii_hi))
	++((*dii)->dii_hi);

    --((*dii)->dii_todo);

    যদি (!(ret = PyBytes_FromString((অক্ষর *)(*dii)->dii_hi->hi_চাবি)))
	ফেরত ফাঁকা;

    ফেরত ret;
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
	ফেরত ফাঁকা;
    }

    ht = &self->dict->dv_hashtab;
    dii->dii_changed = ht->ht_changed;
    dii->dii_ht = ht;
    dii->dii_hi = ht->ht_array;
    dii->dii_todo = ht->ht_used;

    ফেরত IterNew(dii,
	    PyMem_Free, Dictionary_Iter_পরবর্তী,
	    ফাঁকা, ফাঁকা, (PyObject *)self);
}

    স্থির পূর্ণসংখ্যা
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
	ফেরত -1;
    }

    যদি (!(চাবি = StringToChars(চাবি_Object, &todecref)))
	ফেরত -1;

    যদি (*চাবি == NUL)
    {
	RAISE_NO_EMPTY_চাবিগুলি;
	Py_XDECREF(todecref);
	ফেরত -1;
    }

    di = dict_find(dict, চাবি, -1);

    যদি (valObject == ফাঁকা)
    {
	hashitem_T	*hi;

	যদি (di == ফাঁকা)
	{
	    Py_XDECREF(todecref);
	    PyErr_SetObject(Py_Exc_চাবি_Error, চাবি_Object);
	    ফেরত -1;
	}
	hi = hash_find(&dict->dv_hashtab, di->di_চাবি);
	hash_remove(&dict->dv_hashtab, hi, "Python remove item");
	dictitem_free(di);
	Py_XDECREF(todecref);
	ফেরত 0;
    }

    যদি (ConvertFromPyObject(valObject, &tv) == -1)
    {
	Py_XDECREF(todecref);
	ফেরত -1;
    }

    যদি (di == ফাঁকা)
    {
	যদি (!(di = dictitem_alloc(চাবি)))
	{
	    Py_XDECREF(todecref);
	    PyErr_NoMemory();
	    ফেরত -1;
	}
	di->di_tv.v_type = VAR_UNKNOWN;

	যদি (dict_add(dict, di) == FAIL)
	{
	    dictitem_free(di);
	    RAISE_চাবি_ADD_FAIL(চাবি);
	    Py_XDECREF(todecref);
	    ফেরত -1;
	}
    }
    অন্যথা
	clear_tv(&di->di_tv);

    Py_XDECREF(todecref);

    copy_tv(&tv, &di->di_tv);
    clear_tv(&tv);
    ফেরত 0;
}

প্রকারলেখ PyObject *(*hi_to_py)(hashitem_T *);

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
    জন্য (hi = dict->dv_hashtab.ht_array; todo > 0; ++hi)
    {
	যদি (!HASHITEM_EMPTY(hi))
	{
	    যদি (!(newObj = hiconvert(hi)))
	    {
		Py_DECREF(ret);
		ফেরত ফাঁকা;
	    }
	    PyList_SET_ITEM(ret, i, newObj);
	    --todo;
	    ++i;
	}
    }
    ফেরত ret;
}

    স্থির PyObject *
dict_চাবি(hashitem_T *hi)
{
    ফেরত PyBytes_FromString((অক্ষর *)(hi->hi_চাবি));
}

    স্থির PyObject *
Dictionary_List_চাবিগুলি(DictionaryObject *self, PyObject *args UNUSED)
{
    ফেরত DictionaryListObjects(self, dict_চাবি);
}

    স্থির PyObject *
dict_val(hashitem_T *hi)
{
    dictitem_T	*di;

    di = dict_lookup(hi);
    ফেরত ConvertToPyObject(&di->di_tv);
}

    স্থির PyObject *
Dictionary_List_মানসমূহ(DictionaryObject *self, PyObject *args UNUSED)
{
    ফেরত DictionaryListObjects(self, dict_val);
}

    স্থির PyObject *
dict_item(hashitem_T *hi)
{
    PyObject	*চাবি_Object;
    PyObject	*valObject;
    PyObject	*ret;

    যদি (!(চাবি_Object = dict_চাবি(hi)))
	ফেরত ফাঁকা;

    যদি (!(valObject = dict_val(hi)))
    {
	Py_DECREF(চাবি_Object);
	ফেরত ফাঁকা;
    }

    ret = Py_Build_মান("(OO)", চাবি_Object, valObject);

    Py_DECREF(চাবি_Object);
    Py_DECREF(valObject);

    ফেরত ret;
}

    স্থির PyObject *
Dictionary_List_বস্তুসমূহ(DictionaryObject *self, PyObject *args UNUSED)
{
    ফেরত DictionaryListObjects(self, dict_item);
}

    স্থির PyObject *
DictionaryUpdate(DictionaryObject *self, PyObject *args, PyObject *kwargs)
{
    dict_T	*dict = self->dict;

    যদি (dict->dv_lock)
    {
	RAISE_LOCKED_DICTIONARY;
	ফেরত ফাঁকা;
    }

    যদি (kwargs)
    {
	typval_T	tv;

	যদি (ConvertFromPyMapping(kwargs, &tv) == -1)
	    ফেরত ফাঁকা;

	VimTryStart();
	dict_extend(self->dict, tv.vval.v_dict, (char_u *) "force", ফাঁকা);
	clear_tv(&tv);
	যদি (VimTryEnd())
	    ফেরত ফাঁকা;
    }
    অন্যথা
    {
	PyObject	*obj = ফাঁকা;

	যদি (!PyArg_ParseTuple(args, "|O", &obj))
	    ফেরত ফাঁকা;

	যদি (obj == ফাঁকা)
	{
	    Py_INCREF(Py_None);
	    ফেরত Py_None;
	}

	যদি (PyObject_HasAttrString(obj, "keys"))
	    ফেরত DictionaryUpdate(self, ফাঁকা, obj);
	অন্যথা
	{
	    PyObject	*iterator;
	    PyObject	*item;

	    যদি (!(iterator = PyObject_GetIter(obj)))
		ফেরত ফাঁকা;

	    যখন ((item = Py_Iter_পরবর্তী(iterator)))
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
		    ফেরত ফাঁকা;
		}

		Py_DECREF(item);

		যদি (Py_Sequence_আকার(fast) != 2)
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    PyErr_FORMAT(Py_Exc_মান_Error,
			    N_("expected sequence element of size 2, "
			    "but got sequence of size %d"),
			    (পূর্ণসংখ্যা) Py_Sequence_আকার(fast));
		    ফেরত ফাঁকা;
		}

		চাবি_Object = PySequence_GetItem(fast, 0);

		যদি (!(চাবি = StringToChars(চাবি_Object, &todecref)))
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    ফেরত ফাঁকা;
		}

		di = dictitem_alloc(চাবি);

		Py_XDECREF(todecref);

		যদি (di == ফাঁকা)
		{
		    Py_DECREF(fast);
		    Py_DECREF(iterator);
		    PyErr_NoMemory();
		    ফেরত ফাঁকা;
		}
		di->di_tv.v_type = VAR_UNKNOWN;

		valObject = PySequence_GetItem(fast, 1);

		যদি (ConvertFromPyObject(valObject, &di->di_tv) == -1)
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    dictitem_free(di);
		    ফেরত ফাঁকা;
		}

		Py_DECREF(fast);

		hi = hash_find(&dict->dv_hashtab, di->di_চাবি);
		যদি (!HASHITEM_EMPTY(hi) || dict_add(dict, di) == FAIL)
		{
		    RAISE_চাবি_ADD_FAIL(di->di_চাবি);
		    Py_DECREF(iterator);
		    dictitem_free(di);
		    ফেরত ফাঁকা;
		}
	    }

	    Py_DECREF(iterator);

	    // Iterator may have finished due to an exception
	    যদি (PyErr_Occurred())
		ফেরত ফাঁকা;
	}
    }
    Py_INCREF(Py_None);
    ফেরত Py_None;
}

    স্থির PyObject *
DictionaryGet(DictionaryObject *self, PyObject *args)
{
    ফেরত _DictionaryItem(self, args,
			    DICT_FLAG_HAS_DEFAULT|DICT_FLAG_NONE_DEFAULT);
}

    স্থির PyObject *
Dictionary_বের_করো(DictionaryObject *self, PyObject *args)
{
    ফেরত _DictionaryItem(self, args, DICT_FLAG_HAS_DEFAULT|DICT_FLAG_বের_করো);
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
	ফেরত ফাঁকা;
    }

    hi = self->dict->dv_hashtab.ht_array;
    যখন (HASHITEM_EMPTY(hi))
	++hi;

    di = dict_lookup(hi);

    যদি (!(valObject = ConvertToPyObject(&di->di_tv)))
	ফেরত ফাঁকা;

    যদি (!(ret = Py_Build_মান("(" Py_bytes_fmt "O)", hi->hi_চাবি, valObject)))
    {
	Py_DECREF(valObject);
	ফেরত ফাঁকা;
    }

    hash_remove(&self->dict->dv_hashtab, hi, "Python pop item");
    dictitem_free(di);

    ফেরত ret;
}

    স্থির PyObject *
Dictionary_Has_চাবি(DictionaryObject *self, PyObject *চাবি_Object)
{
    ফেরত _DictionaryItem(self, চাবি_Object, DICT_FLAG_RETURN_BOOL);
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
    (lenfunc)       Dictionary_দৈর্ঘ্য,
    (binaryfunc)    DictionaryItem,
    (objobjargproc) DictionaryAssItem,
};

স্থির কাঠামো PyMethodDef DictionaryMethods[] = {
    {"keys",	(PyCFunction)Dictionary_List_চাবিগুলি,	METH_NOARGS,	""},
    {"values",	(PyCFunction)Dictionary_List_মানসমূহ,	METH_NOARGS,	""},
    {"items",	(PyCFunction)Dictionary_List_বস্তুসমূহ,	METH_NOARGS,	""},
    {"update",	(PyCFunction)(শূন্য *)DictionaryUpdate,		METH_VARARGS|METH_KEYWORDS, ""},
    {"get",	(PyCFunction)DictionaryGet,		METH_VARARGS,	""},
    {"pop",	(PyCFunction)Dictionary_বের_করো,		METH_VARARGS,	""},
    {"popitem",	(PyCFunction)Dictionary_বের_করো_Item,		METH_NOARGS,	""},
    {"has_key",	(PyCFunction)Dictionary_Has_চাবি,		METH_O,		""},
    {"__dir__",	(PyCFunction)DictionaryDir,		METH_NOARGS,	""},
    { ফাঁকা,	ফাঁকা,					0,		ফাঁকা}
};

DEFINE_PY_TYPE_OBJECT(ListType);

প্রকারলেখ কাঠামো
{
    Py_Object_শীর্ষ
    list_T	*list;
    pylinkedlist_T	ref;
} ListObject;

#define NEW_LIST(list) ListNew(ListTypePtr, list)

    স্থির PyObject *
ListNew(PyTypeObject *subtype, list_T *list)
{
    ListObject	*self;

    যদি (list == ফাঁকা)
	ফেরত ফাঁকা;

    self = (ListObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    যদি (self == ফাঁকা)
	ফেরত ফাঁকা;
    self->list = list;
    ++list->lv_refcount;
    CHECK_LIST_MATERIALIZE(list);

    pyll_add((PyObject *)(self), &self->ref, &lastlist);

    ফেরত (PyObject *)(self);
}

    স্থির list_T *
py_list_alloc(শূন্য)
{
    list_T	*ret;

    যদি (!(ret = list_alloc()))
    {
	PyErr_NoMemory();
	ফেরত ফাঁকা;
    }
    ++ret->lv_refcount;

    ফেরত ret;
}

    স্থির পূর্ণসংখ্যা
list_py_concat(list_T *l, PyObject *obj, PyObject *lookup_dict)
{
    PyObject	*iterator;
    PyObject	*item;
    listitem_T	*li;

    যদি (!(iterator = PyObject_GetIter(obj)))
	ফেরত -1;

    যখন ((item = Py_Iter_পরবর্তী(iterator)))
    {
	যদি (!(li = listitem_alloc()))
	{
	    PyErr_NoMemory();
	    Py_DECREF(item);
	    Py_DECREF(iterator);
	    ফেরত -1;
	}
	li->li_tv.v_lock = 0;
	li->li_tv.v_type = VAR_UNKNOWN;

	যদি (_ConvertFromPyObject(item, &li->li_tv, lookup_dict) == -1)
	{
	    Py_DECREF(item);
	    Py_DECREF(iterator);
	    listitem_free(l, li);
	    ফেরত -1;
	}

	Py_DECREF(item);

	list_append(l, li);
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    যদি (PyErr_Occurred())
	ফেরত -1;

    ফেরত 0;
}

    স্থির PyObject *
ListConstructor(PyTypeObject *subtype, PyObject *args, PyObject *kwargs)
{
    list_T	*list;
    PyObject	*obj = ফাঁকা;

    যদি (kwargs)
    {
	PyErr_SET_STRING(PyExc_TypeError,
		N_("list constructor does not accept keyword arguments"));
	ফেরত ফাঁকা;
    }

    যদি (!PyArg_ParseTuple(args, "|O", &obj))
	ফেরত ফাঁকা;

    যদি (!(list = py_list_alloc()))
	ফেরত ফাঁকা;

    যদি (obj)
    {
	PyObject	*lookup_dict;

	যদি (!(lookup_dict = PyDict_New()))
	{
	    list_unref(list);
	    ফেরত ফাঁকা;
	}

	যদি (list_py_concat(list, obj, lookup_dict) == -1)
	{
	    Py_DECREF(lookup_dict);
	    list_unref(list);
	    ফেরত ফাঁকা;
	}

	Py_DECREF(lookup_dict);
    }

    ফেরত ListNew(subtype, list);
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
List_দৈর্ঘ্য(ListObject *self)
{
    ফেরত ((PyInt) (self->list->lv_len));
}

    স্থির PyObject *
List_সূচক(ListObject *self, Py_ssize_t সূচক)
{
    listitem_T	*li;

    যদি (সূচক >= List_দৈর্ঘ্য(self))
    {
	PyErr_SET_STRING(Py_Exc_সূচক_Error, N_("list index out of range"));
	ফেরত ফাঁকা;
    }
    li = list_find(self->list, (long) সূচক);
    যদি (li == ফাঁকা)
    {
	// No more suitable format specifications in python-2.3
	PyErr_VIM_FORMAT(N_("internal error: failed to get Vim list item %d"),
		(পূর্ণসংখ্যা) সূচক);
	ফেরত ফাঁকা;
    }
    ফেরত ConvertToPyObject(&li->li_tv);
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
	ফেরত ফাঁকা;
    }

    list = PyList_New(slicelen);
    যদি (list == ফাঁকা)
	ফেরত ফাঁকা;

    জন্য (i = 0; i < slicelen; ++i)
    {
	PyObject	*item;

	item = List_সূচক(self, first + i*step);
	যদি (item == ফাঁকা)
	{
	    Py_DECREF(list);
	    ফেরত ফাঁকা;
	}

	PyList_SET_ITEM(list, i, item);
    }

    ফেরত list;
}

    স্থির PyObject *
ListItem(ListObject *self, PyObject* idx)
{
#যদি PY_MAJOR_VERSION < 3
    যদি (PyInt_Check(idx))
    {
	long _idx = PyInt_AsLong(idx);
	ফেরত List_সূচক(self, _idx);
    }
    অন্যথা
#endif
    যদি (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	ফেরত List_সূচক(self, _idx);
    }
    অন্যথা যদি (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	যদি (PySlice_GetIndicesEx((PySliceObject_T *)idx, List_দৈর্ঘ্য(self),
				 &start, &stop, &step, &slicelen) < 0)
	    ফেরত ফাঁকা;
	ফেরত ListSlice(self, start, step, slicelen);
    }
    অন্যথা
    {
	RAISE_INVALID_সূচক_TYPE(idx);
	ফেরত ফাঁকা;
    }
}

    স্থির শূন্য
list_restore(Py_ssize_t numadded, Py_ssize_t numreplaced, Py_ssize_t slicelen,
	list_T *l, listitem_T **lis, listitem_T *lastaddedli)
{
    যখন (numreplaced--)
    {
	list_ঢোকাও(l, lis[numreplaced], lis[slicelen + numreplaced]);
	listitem_remove(l, lis[slicelen + numreplaced]);
    }
    যখন (numadded--)
    {
	listitem_T	*পরবর্তী;

	পরবর্তী = lastaddedli->li_পূর্ব;
	listitem_remove(l, lastaddedli);
	lastaddedli = পরবর্তী;
    }
}

    স্থির পূর্ণসংখ্যা
ListAssSlice(ListObject *self, Py_ssize_t first,
	     Py_ssize_t step, Py_ssize_t slicelen, PyObject *obj)
{
    PyObject	*iterator;
    PyObject	*item;
    listitem_T	*li;
    listitem_T	*lastaddedli = ফাঁকা;
    listitem_T	*পরবর্তী;
    typval_T	v;
    list_T	*l = self->list;
    PyInt	i;
    PyInt	j;
    PyInt	numreplaced = 0;
    PyInt	numadded = 0;
    PyInt	আকার_2;
    listitem_T	**lis = ফাঁকা;

    আকার_2 = List_দৈর্ঘ্য(self);

    যদি (l->lv_lock)
    {
	RAISE_LOCKED_LIST;
	ফেরত -1;
    }

    যদি (step == 0)
    {
	PyErr_SET_STRING(Py_Exc_মান_Error, N_("slice step cannot be zero"));
	ফেরত -1;
    }

    যদি (step != 1 && slicelen == 0)
    {
	// Nothing to do. Only error out if obj has some items.
	পূর্ণসংখ্যা		ret = 0;

	যদি (obj == ফাঁকা)
	    ফেরত 0;

	যদি (!(iterator = PyObject_GetIter(obj)))
	    ফেরত -1;

	যদি ((item = Py_Iter_পরবর্তী(iterator)))
	{
	    PyErr_FORMAT(Py_Exc_মান_Error,
		    N_("attempt to assign sequence of size greater than %d "
			"to extended slice"), 0);
	    Py_DECREF(item);
	    ret = -1;
	}
	Py_DECREF(iterator);
	ফেরত ret;
    }

    যদি (obj != ফাঁকা)
	// XXX May allocate zero bytes.
	যদি (!(lis = PyMem_New(listitem_T *, slicelen * 2)))
	{
	    PyErr_NoMemory();
	    ফেরত -1;
	}

    যদি (first == আকার_2)
	li = ফাঁকা;
    অন্যথা
    {
	li = list_find(l, (long) first);
	যদি (li == ফাঁকা)
	{
	    PyErr_VIM_FORMAT(N_("internal error: no Vim list item %d"),
		    (পূর্ণসংখ্যা)first);
	    যদি (obj != ফাঁকা)
		PyMem_Free(lis);
	    ফেরত -1;
	}
	i = slicelen;
	যখন (i-- && li != ফাঁকা)
	{
	    j = step;
	    পরবর্তী = li;
	    যদি (step > 0)
		যখন (পরবর্তী != ফাঁকা && ((পরবর্তী = পরবর্তী->li_পরবর্তী) != ফাঁকা) && --j);
	    অন্যথা
		যখন (পরবর্তী != ফাঁকা && ((পরবর্তী = পরবর্তী->li_পূর্ব) != ফাঁকা) && ++j);

	    যদি (obj == ফাঁকা)
		listitem_remove(l, li);
	    অন্যথা
		lis[slicelen - i - 1] = li;

	    li = পরবর্তী;
	}
	যদি (li == ফাঁকা && i != -1)
	{
	    PyErr_SET_VIM(N_("internal error: not enough list items"));
	    যদি (obj != ফাঁকা)
		PyMem_Free(lis);
	    ফেরত -1;
	}
    }

    যদি (obj == ফাঁকা)
	ফেরত 0;

    যদি (!(iterator = PyObject_GetIter(obj)))
    {
	PyMem_Free(lis);
	ফেরত -1;
    }

    i = 0;
    যখন ((item = Py_Iter_পরবর্তী(iterator)))
    {
	যদি (ConvertFromPyObject(item, &v) == -1)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(item);
	    PyMem_Free(lis);
	    ফেরত -1;
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
	    ফেরত -1;
	}
	যদি (numreplaced < slicelen)
	{
	    lis[slicelen + numreplaced] = lis[numreplaced]->li_পূর্ব;
	    vimlist_remove(l, lis[numreplaced], lis[numreplaced]);
	    numreplaced++;
	}
	অন্যথা
	{
	    যদি (li)
		lastaddedli = li->li_পূর্ব;
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
			"to extended slice"), (পূর্ণসংখ্যা) slicelen);
	    list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	    PyMem_Free(lis);
	    ফেরত -1;
	}
	++i;
    }
    Py_DECREF(iterator);

    যদি (step != 1 && i != slicelen)
    {
	PyErr_FORMAT2(Py_Exc_মান_Error,
		N_("attempt to assign sequence of size %d to extended slice "
		    "of size %d"), (পূর্ণসংখ্যা) i, (পূর্ণসংখ্যা) slicelen);
	list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	PyMem_Free(lis);
	ফেরত -1;
    }

    যদি (PyErr_Occurred())
    {
	list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	PyMem_Free(lis);
	ফেরত -1;
    }

    জন্য (i = 0; i < numreplaced; i++)
	listitem_free(l, lis[i]);
    যদি (step == 1)
	জন্য (i = numreplaced; i < slicelen; i++)
	    listitem_remove(l, lis[i]);

    PyMem_Free(lis);

    ফেরত 0;
}

    স্থির পূর্ণসংখ্যা
List_Ass_সূচক(ListObject *self, Py_ssize_t সূচক, PyObject *obj)
{
    typval_T	tv;
    list_T	*l = self->list;
    listitem_T	*li;
    Py_ssize_t	দৈর্ঘ্য = List_দৈর্ঘ্য(self);

    যদি (l->lv_lock)
    {
	RAISE_LOCKED_LIST;
	ফেরত -1;
    }
    যদি (সূচক > দৈর্ঘ্য || (সূচক == দৈর্ঘ্য && obj == ফাঁকা))
    {
	PyErr_SET_STRING(Py_Exc_সূচক_Error, N_("list index out of range"));
	ফেরত -1;
    }

    যদি (obj == ফাঁকা)
    {
	li = list_find(l, (long) সূচক);
	যদি (li == ফাঁকা)
	{
	    PyErr_VIM_FORMAT(N_("internal error: failed to get Vim "
			"list item %d"), (পূর্ণসংখ্যা) সূচক);
	    ফেরত -1;
	}
	vimlist_remove(l, li, li);
	clear_tv(&li->li_tv);
	vim_free(li);
	ফেরত 0;
    }

    যদি (ConvertFromPyObject(obj, &tv) == -1)
	ফেরত -1;

    যদি (সূচক == দৈর্ঘ্য)
    {
	যদি (list_append_tv(l, &tv) == FAIL)
	{
	    clear_tv(&tv);
	    PyErr_SET_VIM(N_("failed to add item to list"));
	    ফেরত -1;
	}
    }
    অন্যথা
    {
	li = list_find(l, (long) সূচক);
	যদি (li == ফাঁকা)
	{
	    PyErr_VIM_FORMAT(N_("internal error: failed to get Vim "
			"list item %d"), (পূর্ণসংখ্যা) সূচক);
	    ফেরত -1;
	}
	clear_tv(&li->li_tv);
	copy_tv(&tv, &li->li_tv);
	clear_tv(&tv);
    }
    ফেরত 0;
}

    স্থির পূর্ণসংখ্যা
ListAssItem(ListObject *self, PyObject *idx, PyObject *obj)
{
#যদি PY_MAJOR_VERSION < 3
    যদি (PyInt_Check(idx))
    {
	long _idx = PyInt_AsLong(idx);
	ফেরত (পূর্ণসংখ্যা)List_Ass_সূচক(self, _idx, obj);
    }
    অন্যথা
#endif
    যদি (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	ফেরত (পূর্ণসংখ্যা)List_Ass_সূচক(self, _idx, obj);
    }
    অন্যথা যদি (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	যদি (PySlice_GetIndicesEx((PySliceObject_T *)idx, List_দৈর্ঘ্য(self),
				 &start, &stop, &step, &slicelen) < 0)
	    ফেরত -1;
	ফেরত (পূর্ণসংখ্যা)ListAssSlice(self, start, step, slicelen,
		obj);
    }
    অন্যথা
    {
	RAISE_INVALID_সূচক_TYPE(idx);
	ফেরত -1;
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
	ফেরত ফাঁকা;
    }

    যদি (!(lookup_dict = PyDict_New()))
	ফেরত ফাঁকা;

    যদি (list_py_concat(l, obj, lookup_dict) == -1)
    {
	Py_DECREF(lookup_dict);
	ফেরত ফাঁকা;
    }
    Py_DECREF(lookup_dict);

    Py_INCREF((PyObject *)self);
    ফেরত (PyObject *)(self);
}

প্রকারলেখ কাঠামো
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
List_Iter_পরবর্তী(শূন্য **arg)
{
    PyObject	*ret;
    listiterinfo_T **lii = (listiterinfo_T**)arg;

    যদি (!((*lii)->lw.lw_item))
	ফেরত ফাঁকা;

    যদি (!(ret = ConvertToPyObject(&((*lii)->lw.lw_item->li_tv))))
	ফেরত ফাঁকা;

    (*lii)->lw.lw_item = (*lii)->lw.lw_item->li_পরবর্তী;

    ফেরত ret;
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
	ফেরত ফাঁকা;
    }

    CHECK_LIST_MATERIALIZE(l);
    list_add_watch(l, &lii->lw);
    lii->lw.lw_item = l->lv_first;
    lii->list = l;
    ++l->lv_refcount;

    ফেরত IterNew(lii,
	    ListIterDestruct, List_Iter_পরবর্তী,
	    ফাঁকা, ফাঁকা, (PyObject *)self);
}

স্থির অক্ষর *ListAttrs[] = {
    "locked",
    ফাঁকা
};

    স্থির PyObject *
ListDir(PyObject *self, PyObject *args UNUSED)
{
    ফেরত ObjectDir(self, ListAttrs);
}

    স্থির পূর্ণসংখ্যা
ListSetattr(PyObject *self_obj, অক্ষর *name, PyObject *valObject)
{
    ListObject *self = (ListObject*)self_obj;
    যদি (valObject == ফাঁকা)
    {
	PyErr_SET_STRING(PyExc_AttributeError,
		N_("cannot delete vim.List attributes"));
	ফেরত -1;
    }

    যদি (strcmp(name, "locked") == 0)
    {
	যদি (self->list->lv_lock == VAR_FIXED)
	{
	    PyErr_SET_STRING(PyExc_TypeError, N_("cannot modify fixed list"));
	    ফেরত -1;
	}
	অন্যথা
	{
	    পূর্ণসংখ্যা		istrue = PyObject_IsTrue(valObject);
	    যদি (istrue == -1)
		ফেরত -1;
	    অন্যথা যদি (istrue)
		self->list->lv_lock = VAR_LOCKED;
	    অন্যথা
		self->list->lv_lock = 0;
	}
	ফেরত 0;
    }
    অন্যথা
    {
	PyErr_FORMAT(PyExc_AttributeError, N_("cannot set attribute %s"), name);
	ফেরত -1;
    }
}

স্থির PySequenceMethods ListAsSeq = {
    (lenfunc)		List_দৈর্ঘ্য,	 // sq_length,	  len(x)
    (binaryfunc)	0,		 // RangeConcat, sq_concat,  x+y
    0,					 // RangeRepeat, sq_repeat,  x*n
    (PyIntArgFunc)	List_সূচক,	 // sq_item,	  x[i]
    0,					 // was_sq_slice,     x[i:j]
    (PyIntObjArgProc)	List_Ass_সূচক,	 // sq_as_item,  x[i]=v
    0,					 // was_sq_ass_slice, x[i:j]=v
    0,					 // sq_contains
    (binaryfunc)	ListConcatInPlace,// sq_inplace_concat
    0,					 // sq_inplace_repeat
};

স্থির PyMappingMethods ListAsMapping = {
    /* mp_length	*/ (lenfunc) List_দৈর্ঘ্য,
    /* mp_subscript     */ (binaryfunc) ListItem,
    /* mp_ass_subscript */ (objobjargproc) ListAssItem,
};

স্থির কাঠামো PyMethodDef ListMethods[] = {
    {"extend",	(PyCFunction)ListConcatInPlace,	METH_O,		""},
    {"__dir__",	(PyCFunction)ListDir,		METH_NOARGS,	""},
    { ফাঁকা,	ফাঁকা,				0,		ফাঁকা}
};

DEFINE_PY_TYPE_OBJECT(TupleType);

প্রকারলেখ কাঠামো
{
    Py_Object_শীর্ষ
    tuple_T	*tuple;
    pylinkedlist_T	ref;
} TupleObject;

#define NEW_TUPLE(tuple) TupleNew(TupleTypePtr, tuple)

    স্থির PyObject *
TupleNew(PyTypeObject *subtype, tuple_T *tuple)
{
    TupleObject	*self;

    যদি (tuple == ফাঁকা)
	ফেরত ফাঁকা;

    self = (TupleObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    যদি (self == ফাঁকা)
	ফেরত ফাঁকা;
    self->tuple = tuple;
    ++tuple->tv_refcount;

    pyll_add((PyObject *)(self), &self->ref, &lasttuple);

    ফেরত (PyObject *)(self);
}

    স্থির tuple_T *
py_tuple_alloc(শূন্য)
{
    tuple_T	*ret;

    যদি (!(ret = tuple_alloc()))
    {
	PyErr_NoMemory();
	ফেরত ফাঁকা;
    }
    ++ret->tv_refcount;

    ফেরত ret;
}

    স্থির পূর্ণসংখ্যা
tuple_py_concat(tuple_T *t, PyObject *obj, PyObject *lookup_dict)
{
    PyObject	*iterator;
    PyObject	*item;

    যদি (!(iterator = PyObject_GetIter(obj)))
	ফেরত -1;

    যখন ((item = Py_Iter_পরবর্তী(iterator)))
    {
	typval_T    new_tv;

	যদি (_ConvertFromPyObject(item, &new_tv, lookup_dict) == -1)
	{
	    Py_DECREF(item);
	    Py_DECREF(iterator);
	    ফেরত -1;
	}

	Py_DECREF(item);

	যদি (tuple_append_tv(t, &new_tv) == FAIL)
	{
	    Py_DECREF(iterator);
	    ফেরত -1;
	}
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    যদি (PyErr_Occurred())
	ফেরত -1;

    ফেরত 0;
}

    স্থির PyObject *
TupleConstructor(PyTypeObject *subtype, PyObject *args, PyObject *kwargs)
{
    tuple_T	*tuple;
    PyObject	*obj = ফাঁকা;

    যদি (kwargs)
    {
	PyErr_SET_STRING(PyExc_TypeError,
		N_("tuple constructor does not accept keyword arguments"));
	ফেরত ফাঁকা;
    }

    যদি (!PyArg_ParseTuple(args, "|O", &obj))
	ফেরত ফাঁকা;

    যদি (!(tuple = py_tuple_alloc()))
	ফেরত ফাঁকা;

    যদি (obj)
    {
	PyObject	*lookup_dict;

	যদি (!(lookup_dict = PyDict_New()))
	{
	    tuple_unref(tuple);
	    ফেরত ফাঁকা;
	}

	যদি (tuple_py_concat(tuple, obj, lookup_dict) == -1)
	{
	    Py_DECREF(lookup_dict);
	    tuple_unref(tuple);
	    ফেরত ফাঁকা;
	}

	Py_DECREF(lookup_dict);
    }

    ফেরত TupleNew(subtype, tuple);
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
Tuple_দৈর্ঘ্য(TupleObject *self)
{
    ফেরত ((PyInt)(tuple_len(self->tuple)));
}

    স্থির PyObject *
Tuple_সূচক(TupleObject *self, Py_ssize_t সূচক)
{
    PyInt	len = Tuple_দৈর্ঘ্য(self);

    যদি (সূচক < 0)
	সূচক = len + সূচক;

    যদি (সূচক < 0 || সূচক >= len)
    {
	PyErr_SET_STRING(Py_Exc_সূচক_Error, N_("tuple index out of range"));
	ফেরত ফাঁকা;
    }
    ফেরত ConvertToPyObject(TUPLE_ITEM(self->tuple, সূচক));
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
    যদি (tuple == ফাঁকা)
	ফেরত ফাঁকা;

    জন্য (i = 0; i < slicelen; ++i)
    {
	PyObject	*item;

	item = Tuple_সূচক(self, first + i * step);
	যদি (item == ফাঁকা)
	{
	    Py_DECREF(tuple);
	    ফেরত ফাঁকা;
	}

	PyTuple_SetItem(tuple, i, item);
    }

    ফেরত tuple;
}

    স্থির PyObject *
TupleItem(TupleObject *self, PyObject* idx)
{
#যদি PY_MAJOR_VERSION < 3
    যদি (PyInt_Check(idx))
    {
	long _idx = PyInt_AsLong(idx);
	ফেরত Tuple_সূচক(self, _idx);
    }
    অন্যথা
#endif
    যদি (PyLong_Check(idx))
    {
	long _idx = PyLong_AsLong(idx);
	ফেরত Tuple_সূচক(self, _idx);
    }
    অন্যথা যদি (PySlice_Check(idx))
    {
	Py_ssize_t start, stop, step, slicelen;

	যদি (PySlice_GetIndicesEx((PySliceObject_T *)idx, Tuple_দৈর্ঘ্য(self),
				 &start, &stop, &step, &slicelen) < 0)
	    ফেরত ফাঁকা;
	ফেরত TupleSlice(self, start, step, slicelen);
    }
    অন্যথা
    {
	RAISE_INVALID_সূচক_TYPE(idx);
	ফেরত ফাঁকা;
    }
}

প্রকারলেখ কাঠামো
{
    tuple_T	*tuple;
    পূর্ণসংখ্যা		সূচক;
} tupleiterinfo_T;

    স্থির শূন্য
TupleIterDestruct(শূন্য *arg)
{
    tupleiterinfo_T *tii = (tupleiterinfo_T*)arg;
    tuple_unref(tii->tuple);
    PyMem_Free(tii);
}

    স্থির PyObject *
Tuple_Iter_পরবর্তী(শূন্য **arg)
{
    PyObject	*ret;
    tupleiterinfo_T **tii = (tupleiterinfo_T**)arg;

    যদি ((*tii)->সূচক >= TUPLE_LEN((*tii)->tuple))
	ফেরত ফাঁকা;

    যদি (!(ret = ConvertToPyObject(TUPLE_ITEM((*tii)->tuple, (*tii)->সূচক))))
	ফেরত ফাঁকা;

    (*tii)->সূচক++;

    ফেরত ret;
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
	ফেরত ফাঁকা;
    }

    tii->tuple = t;
    tii->সূচক = 0;
    ++t->tv_refcount;

    ফেরত IterNew(tii,
	    TupleIterDestruct, Tuple_Iter_পরবর্তী,
	    ফাঁকা, ফাঁকা, (PyObject *)self);
}

স্থির অক্ষর *TupleAttrs[] = {
    "locked",
    ফাঁকা
};

    স্থির PyObject *
TupleDir(PyObject *self, PyObject *args UNUSED)
{
    ফেরত ObjectDir(self, TupleAttrs);
}

    স্থির পূর্ণসংখ্যা
TupleSetattr(PyObject *self_obj, অক্ষর *name, PyObject *valObject)
{
    TupleObject *self = (TupleObject*)self_obj;
    যদি (valObject == ফাঁকা)
    {
	PyErr_SET_STRING(PyExc_AttributeError,
		N_("cannot delete vim.Tuple attributes"));
	ফেরত -1;
    }

    যদি (strcmp(name, "locked") == 0)
    {
	যদি (self->tuple->tv_lock == VAR_FIXED)
	{
	    PyErr_SET_STRING(PyExc_TypeError, N_("cannot modify fixed tuple"));
	    ফেরত -1;
	}
	অন্যথা
	{
	    পূর্ণসংখ্যা		istrue = PyObject_IsTrue(valObject);
	    যদি (istrue == -1)
		ফেরত -1;
	    অন্যথা যদি (istrue)
		self->tuple->tv_lock = VAR_LOCKED;
	    অন্যথা
		self->tuple->tv_lock = 0;
	}
	ফেরত 0;
    }
    অন্যথা
    {
	PyErr_FORMAT(PyExc_AttributeError, N_("cannot set attribute %s"), name);
	ফেরত -1;
    }
}

স্থির PySequenceMethods TupleAsSeq = {
    (lenfunc)		Tuple_দৈর্ঘ্য,	 // sq_length,	  len(x)
    (binaryfunc)	0,		 // RangeConcat, sq_concat,  x+y
    0,					 // RangeRepeat, sq_repeat,  x*n
    (PyIntArgFunc)	Tuple_সূচক,	 // sq_item,	  x[i]
    0,					 // was_sq_slice,     x[i:j]
    (PyIntObjArgProc)	0,		 // sq_as_item,  x[i]=v
    0,					 // was_sq_ass_slice, x[i:j]=v
    0,					 // sq_contains
    (binaryfunc)	0,		 // sq_inplace_concat
    0,					 // sq_inplace_repeat
};

স্থির PyMappingMethods TupleAsMapping = {
    /* mp_length	*/ (lenfunc) Tuple_দৈর্ঘ্য,
    /* mp_subscript     */ (binaryfunc) TupleItem,
    /* mp_ass_subscript */ (objobjargproc) 0,
};

স্থির কাঠামো PyMethodDef TupleMethods[] = {
    {"__dir__",	(PyCFunction)TupleDir,		METH_NOARGS,	""},
    { ফাঁকা,	ফাঁকা,				0,		ফাঁকা}
};

প্রকারলেখ কাঠামো
{
    Py_Object_শীর্ষ
    char_u	*name;
    পূর্ণসংখ্যা		argc;
    typval_T	*argv;
    dict_T	*self;
    pylinkedlist_T	ref;
    পূর্ণসংখ্যা		auto_rebind;
} FunctionObject;

DEFINE_PY_TYPE_OBJECT(FunctionType);

#define NEW_FUNCTION(name, argc, argv, self, pt_auto) \
    FunctionNew(FunctionTypePtr, (name), (argc), (argv), (self), (pt_auto))

    স্থির PyObject *
FunctionNew(PyTypeObject *subtype, char_u *name, পূর্ণসংখ্যা argc, typval_T *argv,
	dict_T *selfdict, পূর্ণসংখ্যা auto_rebind)
{
    FunctionObject	*self;

    self = (FunctionObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    যদি (self == ফাঁকা)
	ফেরত ফাঁকা;

    যদি (isdigit((unsigned অক্ষর)*name))
    {
	যদি (!translated_function_exists(name, FALSE))
	{
	    PyErr_FORMAT(Py_Exc_মান_Error,
		    N_("unnamed function %s does not exist"), name);
	    ফেরত ফাঁকা;
	}
	self->name = vim_strsave(name);
    }
    অন্যথা
    {
	char_u *p;

	যদি ((p = get_expanded_name(name,
			    vim_strchr(name, AUTOLOAD_CHAR) == ফাঁকা)) == ফাঁকা)
	{
	    PyErr_FORMAT(Py_Exc_মান_Error,
		    N_("function %s does not exist"), name);
	    ফেরত ফাঁকা;
	}

	যদি (p[0] == K_SPECIAL && p[1] == KS_EXTRA && p[2] == (পূর্ণসংখ্যা)KE_SNR)
	{
	    char_u *np;
	    size_t len = STRLEN(p) + 1;

	    যদি ((np = alloc(len + 2)) == ফাঁকা)
	    {
		vim_free(p);
		ফেরত ফাঁকা;
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
    self->auto_rebind = selfdict == ফাঁকা ? TRUE : auto_rebind;

    যদি (self->argv || self->self)
	pyll_add((PyObject *)(self), &self->ref, &lastfunc);

    ফেরত (PyObject *)(self);
}

    স্থির PyObject *
FunctionConstructor(PyTypeObject *subtype, PyObject *args, PyObject *kwargs)
{
    PyObject	*self;
    PyObject	*selfdictObject;
    PyObject	*autoRebindObject;
    PyObject	*argsObject = ফাঁকা;
    char_u	*name;
    typval_T	selfdicttv;
    typval_T	argstv;
    list_T	*argslist = ফাঁকা;
    dict_T	*selfdict = ফাঁকা;
    পূর্ণসংখ্যা		argc = 0;
    পূর্ণসংখ্যা		auto_rebind = TRUE;
    typval_T	*argv = ফাঁকা;
    typval_T	*curtv;
    listitem_T	*li;

    যদি (kwargs != ফাঁকা)
    {
	selfdictObject = PyDict_GetItemString(kwargs, "self");
	যদি (selfdictObject != ফাঁকা)
	{
	    যদি (ConvertFromPyMapping(selfdictObject, &selfdicttv) == -1)
		ফেরত ফাঁকা;
	    selfdict = selfdicttv.vval.v_dict;
	}
	argsObject = PyDict_GetItemString(kwargs, "args");
	যদি (argsObject != ফাঁকা)
	{
	    যদি (ConvertFromPySequence(argsObject, &argstv) == -1)
	    {
		dict_unref(selfdict);
		ফেরত ফাঁকা;
	    }
	    argslist = argstv.vval.v_list;
	    CHECK_LIST_MATERIALIZE(argslist);

	    argc = argslist->lv_len;
	    যদি (argc != 0)
	    {
		argv = PyMem_New(typval_T, (size_t) argc);
		যদি (argv == ফাঁকা)
		{
		    PyErr_NoMemory();
		    dict_unref(selfdict);
		    list_unref(argslist);
		    ফেরত ফাঁকা;
		}
		curtv = argv;
		FOR_ALL_LIST_বস্তুসমূহ(argslist, li)
		    copy_tv(&li->li_tv, curtv++);
	    }
	    list_unref(argslist);
	}
	যদি (selfdict != ফাঁকা)
	{
	    auto_rebind = FALSE;
	    autoRebindObject = PyDict_GetItemString(kwargs, "auto_rebind");
	    যদি (autoRebindObject != ফাঁকা)
	    {
		auto_rebind = PyObject_IsTrue(autoRebindObject);
		যদি (auto_rebind == -1)
		{
		    dict_unref(selfdict);
		    list_unref(argslist);
		    ফেরত ফাঁকা;
		}
	    }
	}
    }

    যদি (!PyArg_ParseTuple(args, "et", "ascii", &name))
    {
	dict_unref(selfdict);
	যখন (argc--)
	    clear_tv(&argv[argc]);
	PyMem_Free(argv);
	ফেরত ফাঁকা;
    }

    self = FunctionNew(subtype, name, argc, argv, selfdict, auto_rebind);

    PyMem_Free(name);

    ফেরত self;
}

    স্থির শূন্য
FunctionDestructor(PyObject *self_obj)
{
    FunctionObject *self = (FunctionObject*)self_obj;
    পূর্ণসংখ্যা i;
    func_unref(self->name);
    vim_free(self->name);
    জন্য (i = 0; i < self->argc; ++i)
	clear_tv(&self->argv[i]);
    PyMem_Free(self->argv);
    dict_unref(self->self);
    যদি (self->argv || self->self)
	pyll_remove(&self->ref, &lastfunc);

    DESTRUCTOR_FINISH(self);
}

স্থির অক্ষর *FunctionAttrs[] = {
    "softspace", "args", "self", "auto_rebind",
    ফাঁকা
};

    স্থির PyObject *
FunctionDir(PyObject *self, PyObject *args UNUSED)
{
    ফেরত ObjectDir(self, FunctionAttrs);
}

    স্থির PyObject *
FunctionAttr(FunctionObject *self, অক্ষর *name)
{
    list_T *list;
    পূর্ণসংখ্যা i;
    যদি (strcmp(name, "name") == 0)
	ফেরত PyString_FromString((অক্ষর *)(self->name));
    অন্যথা যদি (strcmp(name, "args") == 0)
    {
	যদি (self->argv == ফাঁকা || (list = list_alloc()) == ফাঁকা)
	    ফেরত ALWAYS_NONE;

	জন্য (i = 0; i < self->argc; ++i)
	    list_append_tv(list, &self->argv[i]);
	ফেরত NEW_LIST(list);
    }
    অন্যথা যদি (strcmp(name, "self") == 0)
	ফেরত self->self == ফাঁকা
	    ? ALWAYS_NONE
	    : NEW_DICTIONARY(self->self);
    অন্যথা যদি (strcmp(name, "auto_rebind") == 0)
	ফেরত self->auto_rebind
	    ? ALWAYS_TRUE
	    : ALWAYS_FALSE;
    অন্যথা যদি (strcmp(name, "__members__") == 0)
	ফেরত ObjectDir(ফাঁকা, FunctionAttrs);
    ফেরত ফাঁকা;
}

/*
 * Populate partial_T given function object.
 *
 * "exported" should be set to true when it is needed to construct a partial
 * that may be stored in a variable (i.e. may be freed by Vim).
 */
    স্থির শূন্য
set_partial(FunctionObject *self, partial_T *pt, পূর্ণসংখ্যা exported)
{
    পূর্ণসংখ্যা i;

    pt->pt_name = self->name;
    যদি (self->argv)
    {
	pt->pt_argc = self->argc;
	যদি (exported)
	{
	    pt->pt_argv = ALLOC_CLEAR_MULT(typval_T, self->argc);
	    জন্য (i = 0; i < pt->pt_argc; ++i)
		copy_tv(&self->argv[i], &pt->pt_argv[i]);
	}
	অন্যথা
	    pt->pt_argv = self->argv;
    }
    অন্যথা
    {
	pt->pt_argc = 0;
	pt->pt_argv = ফাঁকা;
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
    dict_T	*selfdict = ফাঁকা;
    PyObject	*selfdictObject;
    PyObject	*ret;
    পূর্ণসংখ্যা		error;
    partial_T	pt;
    partial_T	*pt_ptr = ফাঁকা;

    যদি (ConvertFromPySequence(argsObject, &args) == -1)
	ফেরত ফাঁকা;

    যদি (kwargs != ফাঁকা)
    {
	selfdictObject = PyDict_GetItemString(kwargs, "self");
	যদি (selfdictObject != ফাঁকা)
	{
	    যদি (ConvertFromPyMapping(selfdictObject, &selfdicttv) == -1)
	    {
		clear_tv(&args);
		ফেরত ফাঁকা;
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
	ret = ফাঁকা;
    অন্যথা যদি (error != OK)
    {
	ret = ফাঁকা;
	PyErr_VIM_FORMAT(N_("failed to run function %s"), (অক্ষর *)name);
    }
    অন্যথা
	ret = ConvertToPyObject(&rettv);

    clear_tv(&args);
    clear_tv(&rettv);
    যদি (selfdict != ফাঁকা)
	clear_tv(&selfdicttv);

    ফেরত ret;
}

    স্থির PyObject *
FunctionRepr(PyObject *self_obj)
{
    FunctionObject *self = (FunctionObject*)self_obj;
    PyObject *ret;
    garray_T repr_ga;
    পূর্ণসংখ্যা i;
    char_u *tofree = ফাঁকা;
    typval_T tv;
    char_u numbuf[NUMBUFLEN];

    ga_init2(&repr_ga, আকার(অক্ষর), 70);
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
	জন্য (i = 0; i < self->argc; i++)
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
    ret = PyString_FromString((অক্ষর *)repr_ga.ga_তথ্য);
    ga_clear(&repr_ga);
    ফেরত ret;
}

স্থির কাঠামো PyMethodDef FunctionMethods[] = {
    {"__dir__",	(PyCFunction)FunctionDir,   METH_NOARGS,		""},
    { ফাঁকা,	ফাঁকা,			0,				ফাঁকা}
};

/*
 * Options object
 */

DEFINE_PY_TYPE_OBJECT(OptionsType);

প্রকারলেখ পূর্ণসংখ্যা (*checkfun)(শূন্য *);

প্রকারলেখ কাঠামো
{
    Py_Object_শীর্ষ
    পূর্ণসংখ্যা		opt_type;
    শূন্য	*from;
    checkfun	Check;
    PyObject	*fromObj;
} OptionsObject;

    স্থির পূর্ণসংখ্যা
dummy_check(শূন্য *arg UNUSED)
{
    ফেরত 0;
}

    স্থির PyObject *
OptionsNew(পূর্ণসংখ্যা opt_type, শূন্য *from, checkfun Check, PyObject *fromObj)
{
    OptionsObject	*self;

    self = PyObject_GC_New(OptionsObject, OptionsTypePtr);
    যদি (self == ফাঁকা)
	ফেরত ফাঁকা;

    self->opt_type = opt_type;
    self->from = from;
    self->Check = Check;
    self->fromObj = fromObj;
    যদি (fromObj)
	Py_INCREF(fromObj);

    ফেরত (PyObject *)(self);
}

    স্থির শূন্য
OptionsDestructor(PyObject *self_obj)
{
    OptionsObject *self = (OptionsObject*)self_obj;
    PyObject_GC_UnTrack((শূন্য *)(self));
    Py_XDECREF(self->fromObj);
    PyObject_GC_Del((শূন্য *)(self));
}

    স্থির পূর্ণসংখ্যা
OptionsTraverse(PyObject *self_obj, visitproc visit, শূন্য *arg)
{
    OptionsObject *self = (OptionsObject*)self_obj;
    Py_VISIT(self->fromObj);
    ফেরত 0;
}

    স্থির পূর্ণসংখ্যা
OptionsClear(PyObject *self_obj)
{
    OptionsObject *self = (OptionsObject*)self_obj;
    Py_CLEAR(self->fromObj);
    ফেরত 0;
}

    স্থির PyObject *
OptionsItem(OptionsObject *self, PyObject *চাবি_Object)
{
    char_u	*চাবি;
    পূর্ণসংখ্যা		flags;
    long	numval;
    char_u	*stringval;
    PyObject	*todecref;

    যদি (self->Check(self->fromObj))
	ফেরত ফাঁকা;

    যদি (!(চাবি = StringToChars(চাবি_Object, &todecref)))
	ফেরত ফাঁকা;

    যদি (*চাবি == NUL)
    {
	RAISE_NO_EMPTY_চাবিগুলি;
	Py_XDECREF(todecref);
	ফেরত ফাঁকা;
    }

    flags = get_option_মান_strict(চাবি, &numval, &stringval,
				    self->opt_type, self->from);

    Py_XDECREF(todecref);

    যদি (flags == 0)
    {
	PyErr_SetObject(Py_Exc_চাবি_Error, চাবি_Object);
	ফেরত ফাঁকা;
    }

    যদি (flags & SOPT_UNSET)
    {
	Py_INCREF(Py_None);
	ফেরত Py_None;
    }
    অন্যথা যদি (flags & SOPT_BOOL)
    {
	PyObject	*ret;
	ret = numval ? Py_True : Py_False;
	Py_INCREF(ret);
	ফেরত ret;
    }
    অন্যথা যদি (flags & SOPT_অঙ্ক)
	ফেরত PyInt_FromLong(numval);
    অন্যথা যদি (flags & SOPT_STRING)
    {
	যদি (stringval)
	{
	    PyObject	*ret = PyBytes_FromString((অক্ষর *)stringval);
	    vim_free(stringval);
	    ফেরত ret;
	}
	অন্যথা
	{
	    PyErr_SET_STRING(PyExc_RuntimeError,
		    N_("unable to get option value"));
	    ফেরত ফাঁকা;
	}
    }
    অন্যথা
    {
	PyErr_SET_VIM(N_("internal error: unknown option type"));
	ফেরত ফাঁকা;
    }
}

    স্থির পূর্ণসংখ্যা
OptionsContains(OptionsObject *self, PyObject *চাবি_Object)
{
    char_u	*চাবি;
    PyObject	*todecref;

    যদি (!(চাবি = StringToChars(চাবি_Object, &todecref)))
	ফেরত -1;

    যদি (*চাবি == NUL)
    {
	Py_XDECREF(todecref);
	ফেরত 0;
    }

    যদি (get_option_মান_strict(চাবি, ফাঁকা, ফাঁকা, self->opt_type, ফাঁকা))
    {
	Py_XDECREF(todecref);
	ফেরত 1;
    }
    অন্যথা
    {
	Py_XDECREF(todecref);
	ফেরত 0;
    }
}

প্রকারলেখ কাঠামো
{
    শূন্য	*lastoption;
    পূর্ণসংখ্যা		opt_type;
} optiterinfo_T;

    স্থির PyObject *
Options_Iter_পরবর্তী(শূন্য **arg)
{
    char_u	*name;
    optiterinfo_T **oii = (optiterinfo_T**)arg;

    যদি ((name = option_iter_পরবর্তী(&((*oii)->lastoption), (*oii)->opt_type)))
	ফেরত PyString_FromString((অক্ষর *)name);

    ফেরত ফাঁকা;
}

    স্থির PyObject *
OptionsIter(PyObject *self_obj)
{
    OptionsObject	*self = (OptionsObject*)self_obj;
    optiterinfo_T	*oii;

    যদি (!(oii = PyMem_New(optiterinfo_T, 1)))
    {
	PyErr_NoMemory();
	ফেরত ফাঁকা;
    }

    oii->opt_type = self->opt_type;
    oii->lastoption = ফাঁকা;

    ফেরত IterNew(oii,
	    PyMem_Free, Options_Iter_পরবর্তী,
	    ফাঁকা, ফাঁকা, (PyObject *)self);
}

    স্থির পূর্ণসংখ্যা
set_option_মান_err(char_u *চাবি, পূর্ণসংখ্যা numval, char_u *stringval, পূর্ণসংখ্যা opt_flags)
{
    অক্ষর	*errmsg;

    যদি ((errmsg = set_option_মান(চাবি, numval, stringval, opt_flags)))
    {
	যদি (VimTryEnd())
	    ফেরত FAIL;
	PyErr_SetVim(errmsg);
	ফেরত FAIL;
    }
    ফেরত OK;
}

    স্থির পূর্ণসংখ্যা
set_option_মান_for(
	char_u *চাবি,
	পূর্ণসংখ্যা numval,
	char_u *stringval,
	পূর্ণসংখ্যা opt_flags,
	পূর্ণসংখ্যা opt_type,
	শূন্য *from)
{
    switchwin_T	switchwin;
    bufref_T	save_curbuf;
    পূর্ণসংখ্যা		set_ret = 0;

    VimTryStart();
    নির্বাচন (opt_type)
    {
	ক্ষেত্র SREQ_WIN:
	    যদি (switch_win(&switchwin, (win_T *)from,
			      win_find_tabpage((win_T *)from), FALSE) == FAIL)
	    {
		restore_win(&switchwin, TRUE);
		যদি (VimTryEnd())
		    ফেরত -1;
		PyErr_SET_VIM(N_("problem while switching windows"));
		ফেরত -1;
	    }
	    set_ret = set_option_মান_err(চাবি, numval, stringval, opt_flags);
	    restore_win(&switchwin, TRUE);
	    বিরতি;
	ক্ষেত্র SREQ_BUF:
	    switch_বাফার(&save_curbuf, (buf_T *)from);
	    set_ret = set_option_মান_err(চাবি, numval, stringval, opt_flags);
	    restore_বাফার(&save_curbuf);
	    বিরতি;
	ক্ষেত্র SREQ_GLOBAL:
	    set_ret = set_option_মান_err(চাবি, numval, stringval, opt_flags);
	    বিরতি;
    }
    যদি (set_ret == FAIL)
	ফেরত -1;
    ফেরত VimTryEnd();
}

    স্থির পূর্ণসংখ্যা
OptionsAssItem(OptionsObject *self, PyObject *চাবি_Object, PyObject *valObject)
{
    char_u	*চাবি;
    পূর্ণসংখ্যা		flags;
    পূর্ণসংখ্যা		opt_flags;
    পূর্ণসংখ্যা		ret = 0;
    PyObject	*todecref;

    যদি (self->Check(self->fromObj))
	ফেরত -1;

    যদি (!(চাবি = StringToChars(চাবি_Object, &todecref)))
	ফেরত -1;

    যদি (*চাবি == NUL)
    {
	RAISE_NO_EMPTY_চাবিগুলি;
	Py_XDECREF(todecref);
	ফেরত -1;
    }

    flags = get_option_মান_strict(চাবি, ফাঁকা, ফাঁকা,
				    self->opt_type, self->from);

    যদি (flags == 0)
    {
	PyErr_SetObject(Py_Exc_চাবি_Error, চাবি_Object);
	Py_XDECREF(todecref);
	ফেরত -1;
    }

    যদি (valObject == ফাঁকা)
    {
	যদি (self->opt_type == SREQ_GLOBAL)
	{
	    PyErr_FORMAT(Py_Exc_মান_Error,
		    N_("unable to unset global option %s"), চাবি);
	    Py_XDECREF(todecref);
	    ফেরত -1;
	}
	অন্যথা যদি (!(flags & SOPT_GLOBAL))
	{
	    PyErr_FORMAT(Py_Exc_মান_Error,
		    N_("unable to unset option %s "
		       "which does not have global value"), চাবি);
	    Py_XDECREF(todecref);
	    ফেরত -1;
	}
	অন্যথা
	{
	    unset_global_local_option(চাবি, self->from);
	    Py_XDECREF(todecref);
	    ফেরত 0;
	}
    }

    opt_flags = (self->opt_type ? OPT_LOCAL : OPT_GLOBAL);

    যদি (flags & SOPT_BOOL)
    {
	পূর্ণসংখ্যা	istrue = PyObject_IsTrue(valObject);

	যদি (istrue == -1)
	    ret = -1;
	অন্যথা
	    ret = set_option_মান_for(চাবি, istrue, ফাঁকা,
				    opt_flags, self->opt_type, self->from);
    }
    অন্যথা যদি (flags & SOPT_অঙ্ক)
    {
	long	val;

	যদি (সংখ্যা_To_Long(valObject, &val, সংখ্যা_INT))
	{
	    Py_XDECREF(todecref);
	    ফেরত -1;
	}

	ret = set_option_মান_for(চাবি, (পূর্ণসংখ্যা) val, ফাঁকা, opt_flags,
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

    ফেরত ret;
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
    (lenfunc)       ফাঁকা,
    (binaryfunc)    OptionsItem,
    (objobjargproc) OptionsAssItem,
};

// Tabpage object

প্রকারলেখ কাঠামো
{
    Py_Object_শীর্ষ
    tabpage_T	*tab;
} TabPageObject;

স্থির PyObject *WinListNew(TabPageObject *tabObject);

DEFINE_PY_TYPE_OBJECT(TabPageType);

    স্থির পূর্ণসংখ্যা
CheckTabPage(TabPageObject *self)
{
    যদি (self->tab == INVALID_TABPAGE_মান)
    {
	PyErr_SET_VIM(N_("attempt to refer to deleted tab page"));
	ফেরত -1;
    }

    ফেরত 0;
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
	যদি (self == ফাঁকা)
	    ফেরত ফাঁকা;
	self->tab = tab;
	TAB_PYTHON_REF(tab) = self;
    }

    ফেরত (PyObject *)(self);
}

    স্থির শূন্য
TabPageDestructor(PyObject *self_obj)
{
    TabPageObject *self = (TabPageObject*)self_obj;
    যদি (self->tab && self->tab != INVALID_TABPAGE_মান)
	TAB_PYTHON_REF(self->tab) = ফাঁকা;

    DESTRUCTOR_FINISH(self);
}

স্থির অক্ষর *TabPageAttrs[] = {
    "windows", "number", "vars", "window", "valid",
    ফাঁকা
};

    স্থির PyObject *
TabPageDir(PyObject *self, PyObject *args UNUSED)
{
    ফেরত ObjectDir(self, TabPageAttrs);
}

    স্থির PyObject *
TabPageAttrValid(TabPageObject *self, অক্ষর *name)
{
    PyObject	*ret;

    যদি (strcmp(name, "valid") != 0)
	ফেরত ফাঁকা;

    ret = ((self->tab == INVALID_TABPAGE_মান) ? Py_False : Py_True);
    Py_INCREF(ret);
    ফেরত ret;
}

    স্থির PyObject *
TabPageAttr(TabPageObject *self, অক্ষর *name)
{
    যদি (strcmp(name, "windows") == 0)
	ফেরত WinListNew(self);
    অন্যথা যদি (strcmp(name, "number") == 0)
	ফেরত PyLong_FromLong((long) get_tab_সংখ্যা(self->tab));
    অন্যথা যদি (strcmp(name, "vars") == 0)
	ফেরত NEW_DICTIONARY(self->tab->tp_vars);
    অন্যথা যদি (strcmp(name, "window") == 0)
    {
	// For current tab window.c does not bother to set or update tp_curwin
	যদি (self->tab == curtab)
	    ফেরত WindowNew(curwin, curtab);
	অন্যথা
	    ফেরত WindowNew(self->tab->tp_curwin, self->tab);
    }
    অন্যথা যদি (strcmp(name, "__members__") == 0)
	ফেরত ObjectDir(ফাঁকা, TabPageAttrs);
    ফেরত ফাঁকা;
}

    স্থির PyObject *
TabPageRepr(PyObject *self_obj)
{
    TabPageObject *self = (TabPageObject*)self_obj;
    যদি (self->tab == INVALID_TABPAGE_মান)
	ফেরত PyString_FromFormat("<tabpage object (deleted) at %p>", (শূন্য *)self);
    অন্যথা
    {
	পূর্ণসংখ্যা	t = get_tab_সংখ্যা(self->tab);

	যদি (t == 0)
	    ফেরত PyString_FromFormat("<tabpage object (unknown) at %p>",
					(শূন্য *)self);
	অন্যথা
	    ফেরত PyString_FromFormat("<tabpage %d>", t - 1);
    }
}

স্থির কাঠামো PyMethodDef TabPageMethods[] = {
    // name,	    function,			calling,	documentation
    {"__dir__",	    (PyCFunction)TabPageDir,	METH_NOARGS,	""},
    { ফাঁকা,	    ফাঁকা,			0,		ফাঁকা}
};

/*
 * Window list object
 */

DEFINE_PY_TYPE_OBJECT(TabListType);
স্থির PySequenceMethods TabListAsSeq;

প্রকারলেখ কাঠামো
{
    Py_Object_শীর্ষ
} TabListObject;

স্থির TabListObject TheTabPageList =
{
    Py_Object_শীর্ষ_INIT_TYPE(TabListType)
};

    স্থির PyInt
Tab_List_দৈর্ঘ্য(PyObject *self UNUSED)
{
    tabpage_T	*tp = first_tabpage;
    PyInt	n = 0;

    যখন (tp != ফাঁকা)
    {
	++n;
	tp = tp->tp_পরবর্তী;
    }

    ফেরত n;
}

    স্থির PyObject *
TabListItem(PyObject *self UNUSED, PyInt n)
{
    tabpage_T	*tp;

    জন্য (tp = first_tabpage; tp != ফাঁকা; tp = tp->tp_পরবর্তী, --n)
	যদি (n == 0)
	    ফেরত TabPageNew(tp);

    PyErr_SET_STRING(Py_Exc_সূচক_Error, N_("no such tab page"));
    ফেরত ফাঁকা;
}

/*
 * Window object
 */

প্রকারলেখ কাঠামো
{
    Py_Object_শীর্ষ
    win_T	*win;
    TabPageObject	*tabObject;
} WindowObject;

DEFINE_PY_TYPE_OBJECT(WindowType);

    স্থির পূর্ণসংখ্যা
CheckWindow(WindowObject *self)
{
    যদি (self->win == INVALID_WINDOW_মান)
    {
	PyErr_SET_VIM(N_("attempt to refer to deleted window"));
	ফেরত -1;
    }

    ফেরত 0;
}

    স্থির পূর্ণসংখ্যা
CheckWindowCb(শূন্য *self)
{
    ফেরত CheckWindow((WindowObject*)self);
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
	যদি (self == ফাঁকা)
	    ফেরত ফাঁকা;
	self->win = win;
	WIN_PYTHON_REF(win) = self;
    }

    self->tabObject = ((TabPageObject *)(TabPageNew(tab)));

    ফেরত (PyObject *)(self);
}

    স্থির শূন্য
WindowDestructor(PyObject *self_obj)
{
    WindowObject *self = (WindowObject*)self_obj;
    PyObject_GC_UnTrack((শূন্য *)(self));
    যদি (self->win && self->win != INVALID_WINDOW_মান)
	WIN_PYTHON_REF(self->win) = ফাঁকা;
    Py_XDECREF(((PyObject *)(self->tabObject)));
    PyObject_GC_Del((শূন্য *)(self));
}

    স্থির পূর্ণসংখ্যা
WindowTraverse(PyObject *self_obj, visitproc visit, শূন্য *arg)
{
    WindowObject *self = (WindowObject*)self_obj;
    Py_VISIT(((PyObject *)(self->tabObject)));
    ফেরত 0;
}

    স্থির পূর্ণসংখ্যা
WindowClear(PyObject *self_obj)
{
    WindowObject *self = (WindowObject*)self_obj;
    Py_CLEAR(self->tabObject);
    ফেরত 0;
}

    স্থির win_T *
get_firstwin(TabPageObject *tabObject)
{
    যদি (tabObject)
    {
	যদি (CheckTabPage(tabObject))
	    ফেরত ফাঁকা;
	// For current tab window.c does not bother to set or update tp_firstwin
	অন্যথা যদি (tabObject->tab == curtab)
	    ফেরত firstwin;
	অন্যথা
	    ফেরত tabObject->tab->tp_firstwin;
    }
    অন্যথা
	ফেরত firstwin;
}

// Use the same order as in the WindowAttr() function.
স্থির অক্ষর *WindowAttrs[] = {
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
    ফাঁকা
};

    স্থির PyObject *
WindowDir(PyObject *self, PyObject *args UNUSED)
{
    ফেরত ObjectDir(self, WindowAttrs);
}

    স্থির PyObject *
WindowAttrValid(WindowObject *self, অক্ষর *name)
{
    PyObject	*ret;

    যদি (strcmp(name, "valid") != 0)
	ফেরত ফাঁকা;

    ret = ((self->win == INVALID_WINDOW_মান) ? Py_False : Py_True);
    Py_INCREF(ret);
    ফেরত ret;
}

    স্থির PyObject *
WindowAttr(WindowObject *self, অক্ষর *name)
{
    যদি (strcmp(name, "buffer") == 0)
	ফেরত (PyObject *)বাফার_New(self->win->w_বাফার);
    অন্যথা যদি (strcmp(name, "cursor") == 0)
    {
	pos_T *pos = &self->win->w_cursor;

	ফেরত Py_Build_মান("(ll)", (long)(pos->lnum), (long)(pos->col));
    }
    অন্যথা যদি (strcmp(name, "height") == 0)
	ফেরত PyLong_FromLong((long)(self->win->w_height));
    অন্যথা যদি (strcmp(name, "row") == 0)
	ফেরত PyLong_FromLong((long)(self->win->w_winrow));
    অন্যথা যদি (strcmp(name, "width") == 0)
	ফেরত PyLong_FromLong((long)(self->win->w_width));
    অন্যথা যদি (strcmp(name, "col") == 0)
	ফেরত PyLong_FromLong((long)(self->win->w_wincol));
    অন্যথা যদি (strcmp(name, "vars") == 0)
	ফেরত NEW_DICTIONARY(self->win->w_vars);
    অন্যথা যদি (strcmp(name, "options") == 0)
	ফেরত OptionsNew(SREQ_WIN, self->win, CheckWindowCb,
			(PyObject *) self);
    অন্যথা যদি (strcmp(name, "number") == 0)
    {
	যদি (CheckTabPage(self->tabObject))
	    ফেরত ফাঁকা;
	ফেরত PyLong_FromLong((long)
		get_win_সংখ্যা(self->win, get_firstwin(self->tabObject)));
    }
    অন্যথা যদি (strcmp(name, "tabpage") == 0)
    {
	Py_INCREF((PyObject *)self->tabObject);
	ফেরত (PyObject *)(self->tabObject);
    }
    অন্যথা যদি (strcmp(name, "__members__") == 0)
	ফেরত ObjectDir(ফাঁকা, WindowAttrs);
    অন্যথা
	ফেরত ফাঁকা;
}

    স্থির পূর্ণসংখ্যা
WindowSetattr(PyObject *self_obj, অক্ষর *name, PyObject *valObject)
{
    WindowObject *self = (WindowObject*)self_obj;
    যদি (CheckWindow(self))
	ফেরত -1;

    যদি (strcmp(name, "buffer") == 0)
    {
	PyErr_SET_STRING(PyExc_TypeError, N_("readonly attribute: buffer"));
	ফেরত -1;
    }
    অন্যথা যদি (strcmp(name, "cursor") == 0)
    {
	long lnum;
	long col;

	যদি (!PyArg_Parse(valObject, "(ll)", &lnum, &col))
	    ফেরত -1;

	যদি (lnum <= 0 || lnum > self->win->w_বাফার->b_ml.ml_সারি_গুনতি)
	{
	    PyErr_SET_VIM(N_("cursor position outside buffer"));
	    ফেরত -1;
	}

	// Check for keyboard interrupts
	যদি (VimCheckInterrupt())
	    ফেরত -1;

	self->win->w_cursor.lnum = lnum;
	self->win->w_cursor.col = col;
	self->win->w_set_curswant = TRUE;
	self->win->w_cursor.coladd = 0;
	// When column is out of range silently correct it.
	check_cursor_col_win(self->win);

	update_screen(UPD_VALID);
	ফেরত 0;
    }
    অন্যথা যদি (strcmp(name, "height") == 0)
    {
	long	height;
	win_T	*savewin;

	যদি (সংখ্যা_To_Long(valObject, &height, সংখ্যা_INT|সংখ্যা_UNSIGNED))
	    ফেরত -1;

#ifdef FEAT_GUI
	need_mouse_correct = TRUE;
#endif
	savewin = curwin;
	curwin = self->win;
	curbuf = curwin->w_বাফার;

	VimTryStart();
	win_setheight((পূর্ণসংখ্যা) height);
	curwin = savewin;
	curbuf = curwin->w_বাফার;
	যদি (VimTryEnd())
	    ফেরত -1;

	ফেরত 0;
    }
    অন্যথা যদি (strcmp(name, "width") == 0)
    {
	long	width;
	win_T	*savewin;

	যদি (সংখ্যা_To_Long(valObject, &width, সংখ্যা_INT|সংখ্যা_UNSIGNED))
	    ফেরত -1;

#ifdef FEAT_GUI
	need_mouse_correct = TRUE;
#endif
	savewin = curwin;
	curwin = self->win;
	curbuf = curwin->w_বাফার;

	VimTryStart();
	win_setwidth((পূর্ণসংখ্যা) width);
	curwin = savewin;
	curbuf = curwin->w_বাফার;
	যদি (VimTryEnd())
	    ফেরত -1;

	ফেরত 0;
    }
    অন্যথা
    {
	PyErr_SetString(PyExc_AttributeError, name);
	ফেরত -1;
    }
}

    স্থির PyObject *
WindowRepr(PyObject *self_obj)
{
    WindowObject *self = (WindowObject*)self_obj;
    যদি (self->win == INVALID_WINDOW_মান)
	ফেরত PyString_FromFormat("<window object (deleted) at %p>", (শূন্য *)self);
    অন্যথা
    {
	পূর্ণসংখ্যা	w = get_win_সংখ্যা(self->win, firstwin);

	যদি (w == 0)
	    ফেরত PyString_FromFormat("<window object (unknown) at %p>",
								      (শূন্য *)self);
	অন্যথা
	    ফেরত PyString_FromFormat("<window %d>", w - 1);
    }
}

স্থির কাঠামো PyMethodDef WindowMethods[] = {
    // name,	    function,			calling,	documentation
    {"__dir__",	    (PyCFunction)WindowDir,	METH_NOARGS,	""},
    { ফাঁকা,	    ফাঁকা,			0,		ফাঁকা}
};

/*
 * Window list object
 */

DEFINE_PY_TYPE_OBJECT(WinListType);
স্থির PySequenceMethods WinListAsSeq;

প্রকারলেখ কাঠামো
{
    Py_Object_শীর্ষ
    TabPageObject	*tabObject;
} WinListObject;

স্থির WinListObject TheWindowList =
{
    Py_Object_শীর্ষ_INIT_TYPE(WinListType)
    ফাঁকা
};

    স্থির PyObject *
WinListNew(TabPageObject *tabObject)
{
    WinListObject	*self;

    self = PyObject_NEW(WinListObject, WinListTypePtr);
    self->tabObject = tabObject;
    Py_INCREF((PyObject *)tabObject);

    ফেরত (PyObject *)(self);
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
Win_List_দৈর্ঘ্য(WinListObject *self)
{
    win_T	*w;
    PyInt	n = 0;

    যদি (!(w = get_firstwin(self->tabObject)))
	ফেরত -1;

    যখন (w != ফাঁকা)
    {
	++n;
	w = W_পরবর্তী(w);
    }

    ফেরত n;
}

    স্থির PyObject *
WinListItem(WinListObject *self, PyInt n)
{
    win_T *w;

    যদি (!(w = get_firstwin(self->tabObject)))
	ফেরত ফাঁকা;

    জন্য (; w != ফাঁকা; w = W_পরবর্তী(w), --n)
	যদি (n == 0)
	    ফেরত WindowNew(w, self->tabObject? self->tabObject->tab: curtab);

    PyErr_SET_STRING(Py_Exc_সূচক_Error, N_("no such window"));
    ফেরত ফাঁকা;
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
    স্থির অক্ষর *
String_To_সারি(PyObject *obj)
{
    অক্ষর	*str;
    অক্ষর	*save;
    PyObject	*bytes = ফাঁকা;
    Py_ssize_t	len = 0;
    PyInt	i;
    অক্ষর	*p;

    যদি (PyBytes_Check(obj))
    {
	যদি (Py_Bytes_As_String_And_আকার(obj, &str, &len) == -1
		|| str == ফাঁকা)
	    ফেরত ফাঁকা;
    }
    অন্যথা যদি (PyUnicode_Check(obj))
    {
	যদি (!(bytes = PyUnicode_AsEncodedString(obj, ENC_OPT,
							   ERRORS_ENCODE_ARG)))
	    ফেরত ফাঁকা;

	যদি (Py_Bytes_As_String_And_আকার(bytes, &str, &len) == -1
		|| str == ফাঁকা)
	{
	    Py_DECREF(bytes);
	    ফেরত ফাঁকা;
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
	ফেরত ফাঁকা;
    }

    /*
     * Error checking: String must not contain newlines, as we
     * are replacing a single line, and we must replace it with
     * a single line.
     * A trailing newline is removed, so that append(f.readlines()) works.
     */
    p = memchr(str, '\n', len);
    যদি (p != ফাঁকা)
    {
	যদি (p == str + len - 1)
	    --len;
	অন্যথা
	{
	    PyErr_SET_VIM(N_("string cannot contain newlines"));
	    Py_XDECREF(bytes);
	    ফেরত ফাঁকা;
	}
    }

    /*
     * Create a copy of the string, with internal nulls replaced by
     * newline characters, as is the Vim convention.
     */
    save = alloc(len+1);
    যদি (save == ফাঁকা)
    {
	PyErr_NoMemory();
	Py_XDECREF(bytes);
	ফেরত ফাঁকা;
    }

    জন্য (i = 0; i < len; ++i)
    {
	যদি (str[i] == '\0')
	    save[i] = '\n';
	অন্যথা
	    save[i] = str[i];
    }

    save[i] = '\0';
    Py_XDECREF(bytes);  // Python 2 does nothing here

    ফেরত save;
}

/*
 * Get a line from the specified buffer. The line number is
 * in Vim format (1-based). The line is returned as a Python
 * string object.
 */
    স্থির PyObject *
Get_বাফার_সারি(buf_T *buf, PyInt n)
{
    ফেরত সারি_To_String((অক্ষর *)ml_get_buf(buf, (linenr_T)n, FALSE));
}


/*
 * Get a list of lines from the specified buffer. The line numbers
 * are in Vim format (1-based). The range is from lo up to, but not
 * including, hi. The list is returned as a Python list of string objects.
 */
    স্থির PyObject *
Get_বাফার_সারি_List(buf_T *buf, PyInt lo, PyInt hi)
{
    PyInt	i;
    PyInt	n = hi - lo;
    PyObject	*list = PyList_New(n);

    যদি (list == ফাঁকা)
	ফেরত ফাঁকা;

    জন্য (i = 0; i < n; ++i)
    {
	linenr_T	lnum = (linenr_T)(lo + i);
	অক্ষর		*text;
	PyObject	*string;

	যদি (lnum > buf->b_ml.ml_সারি_গুনতি)
	    text = "";
	অন্যথা
	    text = (অক্ষর *)ml_get_buf(buf, lnum, FALSE);
	string = সারি_To_String(text);
	যদি (string == ফাঁকা)
	{
	    Py_DECREF(list);
	    ফেরত ফাঁকা;
	}

	PyList_SET_ITEM(list, i, string);
    }

    // The ownership of the Python list is passed to the caller (ie,
    // the caller should Py_DECREF() the object when it is finished
    // with it).

    ফেরত list;
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
    স্থির পূর্ণসংখ্যা
Set_বাফার_সারি(buf_T *buf, PyInt n, PyObject *সারি, PyInt *len_change)
{
    bufref_T	save_curbuf = {ফাঁকা, 0, 0};
    switchwin_T	switchwin;

    // First of all, we check the type of the supplied Python object.
    // There are three cases:
    //	  1. NULL, or None - this is a deletion.
    //	  2. A string	   - this is a replacement.
    //	  3. Anything else - this is an error.
    যদি (সারি == Py_None || সারি == ফাঁকা)
    {
	PyErr_Clear();
	switchwin.sw_curwin = ফাঁকা;
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	VimTryStart();

	যদি (u_savedel((linenr_T)n, 1L) == FAIL)
	    RAISE_UNDO_FAIL;
	অন্যথা যদি (ml_delete((linenr_T)n) == FAIL)
	    RAISE_DELETE_সারি_FAIL;
	অন্যথা
	{
	    যদি (buf == curbuf && (switchwin.sw_curwin != ফাঁকা
					   || save_curbuf.br_buf == ফাঁকা))
		// Using an existing window for the buffer, adjust the cursor
		// position.
		py_fix_cursor((linenr_T)n, (linenr_T)n + 1, (linenr_T)-1);
	    যদি (save_curbuf.br_buf == ফাঁকা)
		// Only adjust marks if we managed to switch to a window that
		// holds the buffer, otherwise line numbers will be invalid.
		deleted_সারিগুলি_mark((linenr_T)n, 1L);
	}

	restore_win_for_buf(&switchwin, &save_curbuf);

	যদি (VimTryEnd())
	    ফেরত FAIL;

	যদি (len_change)
	    *len_change = -1;

	ফেরত OK;
    }
    অন্যথা যদি (PyBytes_Check(সারি) || PyUnicode_Check(সারি))
    {
	অক্ষর	*save = String_To_সারি(সারি);

	যদি (save == ফাঁকা)
	    ফেরত FAIL;

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
	    ফেরত FAIL;

	যদি (len_change)
	    *len_change = 0;

	ফেরত OK;
    }
    অন্যথা
    {
	PyErr_BadArgument();
	ফেরত FAIL;
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
    স্থির পূর্ণসংখ্যা
Set_বাফার_সারি_List(
	buf_T *buf,
	PyInt lo,
	PyInt hi,
	PyObject *list,
	PyInt *len_change)
{
    bufref_T	save_curbuf = {ফাঁকা, 0, 0};
    switchwin_T	switchwin;

    // First of all, we check the type of the supplied Python object.
    // There are three cases:
    //	  1. NULL, or None - this is a deletion.
    //	  2. A list	   - this is a replacement.
    //	  3. Anything else - this is an error.
    যদি (list == Py_None || list == ফাঁকা)
    {
	PyInt	i;
	PyInt	n = (পূর্ণসংখ্যা)(hi - lo);

	PyErr_Clear();
	VimTryStart();
	switchwin.sw_curwin = ফাঁকা;
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	যদি (u_savedel((linenr_T)lo, (long)n) == FAIL)
	    RAISE_UNDO_FAIL;
	অন্যথা
	{
	    জন্য (i = 0; i < n; ++i)
	    {
		যদি (ml_delete((linenr_T)lo) == FAIL)
		{
		    RAISE_DELETE_সারি_FAIL;
		    বিরতি;
		}
	    }
	    যদি (buf == curbuf && (switchwin.sw_curwin != ফাঁকা
					       || save_curbuf.br_buf == ফাঁকা))
		// Using an existing window for the buffer, adjust the cursor
		// position.
		py_fix_cursor((linenr_T)lo, (linenr_T)hi, (linenr_T)-n);
	    যদি (save_curbuf.br_buf == ফাঁকা)
		// Only adjust marks if we managed to switch to a window that
		// holds the buffer, otherwise line numbers will be invalid.
		deleted_সারিগুলি_mark((linenr_T)lo, (long)i);
	}

	restore_win_for_buf(&switchwin, &save_curbuf);

	যদি (VimTryEnd())
	    ফেরত FAIL;

	যদি (len_change)
	    *len_change = -n;

	ফেরত OK;
    }
    অন্যথা যদি (PyList_Check(list))
    {
	PyInt	i;
	PyInt	new_len = Py_List_আকার(list);
	PyInt	old_len = hi - lo;
	PyInt	extra = 0;	// lines added to text, can be negative
	অক্ষর	**array;

	যদি (new_len == 0)	// avoid allocating zero bytes
	    array = ফাঁকা;
	অন্যথা
	{
	    array = PyMem_New(অক্ষর *, new_len);
	    যদি (array == ফাঁকা)
	    {
		PyErr_NoMemory();
		ফেরত FAIL;
	    }
	}

	জন্য (i = 0; i < new_len; ++i)
	{
	    PyObject *সারি;

	    যদি (!(সারি = PyList_GetItem(list, i)) ||
		!(array[i] = String_To_সারি(সারি)))
	    {
		যখন (i)
		    vim_free(array[--i]);
		PyMem_Free(array);
		ফেরত FAIL;
	    }
	}

	VimTryStart();
	PyErr_Clear();

	// START of region without "return".  Must call restore_buffer()!
	switchwin.sw_curwin = ফাঁকা;
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	যদি (u_save((linenr_T)(lo-1), (linenr_T)hi) == FAIL)
	    RAISE_UNDO_FAIL;

	// If the size of the range is reducing (ie, new_len < old_len) we
	// need to delete some old_len. We do this at the start, by
	// repeatedly deleting line "lo".
	যদি (!PyErr_Occurred())
	{
	    জন্য (i = 0; i < old_len - new_len; ++i)
		যদি (ml_delete((linenr_T)lo) == FAIL)
		{
		    RAISE_DELETE_সারি_FAIL;
		    বিরতি;
		}
	    extra -= i;
	}

	// For as long as possible, replace the existing old_len with the
	// new old_len. This is a more efficient operation, as it requires
	// less memory allocation and freeing.
	যদি (!PyErr_Occurred())
	{
	    জন্য (i = 0; i < old_len && i < new_len; ++i)
		যদি (ml_replace((linenr_T)(lo+i), (char_u *)array[i], FALSE)
								      == FAIL)
		{
		    RAISE_REPLACE_সারি_FAIL;
		    বিরতি;
		}
	}
	অন্যথা
	    i = 0;

	// Now we may need to insert the remaining new old_len. If we do, we
	// must free the strings as we finish with them (we can't pass the
	// responsibility to Vim in this case).
	যদি (!PyErr_Occurred())
	{
	    যখন (i < new_len)
	    {
		যদি (ml_append((linenr_T)(lo + i - 1),
					(char_u *)array[i], 0, FALSE) == FAIL)
		{
		    RAISE_ঢোকাও_সারি_FAIL;
		    বিরতি;
		}
		vim_free(array[i]);
		++i;
		++extra;
	    }
	}

	// Free any left-over old_len, as a result of an error
	যখন (i < new_len)
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
	যদি (save_curbuf.br_buf == ফাঁকা)
	{
	    mark_adjust((linenr_T)lo, (linenr_T)(hi - 1),
						  (long)MAXLNUM, (long)extra);
	    changed_সারিগুলি((linenr_T)lo, 0, (linenr_T)hi, (long)extra);
	}

	যদি (buf == curbuf && (switchwin.sw_curwin != ফাঁকা
					   || save_curbuf.br_buf == ফাঁকা))
	    // Using an existing window for the buffer, adjust the cursor
	    // position.
	    py_fix_cursor((linenr_T)lo, (linenr_T)hi, (linenr_T)extra);

	// END of region without "return".
	restore_win_for_buf(&switchwin, &save_curbuf);

	যদি (VimTryEnd())
	    ফেরত FAIL;

	যদি (len_change)
	    *len_change = new_len - old_len;

	ফেরত OK;
    }
    অন্যথা
    {
	PyErr_BadArgument();
	ফেরত FAIL;
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
    স্থির পূর্ণসংখ্যা
ঢোকাও_বাফার_সারিগুলি(buf_T *buf, PyInt n, PyObject *সারিগুলি, PyInt *len_change)
{
    bufref_T	save_curbuf = {ফাঁকা, 0, 0};
    switchwin_T	switchwin;

    // First of all, we check the type of the supplied Python object.
    // It must be a string or a list, or the call is in error.
    যদি (PyBytes_Check(সারিগুলি) || PyUnicode_Check(সারিগুলি))
    {
	অক্ষর		*str = String_To_সারি(সারিগুলি);

	যদি (str == ফাঁকা)
	    ফেরত FAIL;

	PyErr_Clear();
	VimTryStart();
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	যদি (u_save((linenr_T)n, (linenr_T)(n + 1)) == FAIL)
	    RAISE_UNDO_FAIL;
	অন্যথা যদি (ml_append((linenr_T)n, (char_u *)str, 0, FALSE) == FAIL)
	    RAISE_ঢোকাও_সারি_FAIL;
	অন্যথা যদি (save_curbuf.br_buf == ফাঁকা)
	    // Only adjust marks if we managed to switch to a window that
	    // holds the buffer, otherwise line numbers will be invalid.
	    appended_সারিগুলি_mark((linenr_T)n, 1L);

	vim_free(str);
	restore_win_for_buf(&switchwin, &save_curbuf);
	update_screen(UPD_VALID);

	যদি (VimTryEnd())
	    ফেরত FAIL;

	যদি (len_change)
	    *len_change = 1;

	ফেরত OK;
    }
    অন্যথা যদি (PyList_Check(সারিগুলি))
    {
	PyInt	i;
	PyInt	আকার_2 = Py_List_আকার(সারিগুলি);
	অক্ষর	**array;

	array = PyMem_New(অক্ষর *, আকার_2);
	যদি (array == ফাঁকা)
	{
	    PyErr_NoMemory();
	    ফেরত FAIL;
	}

	জন্য (i = 0; i < আকার_2; ++i)
	{
	    PyObject *সারি;

	    যদি (!(সারি = PyList_GetItem(সারিগুলি, i)) ||
		!(array[i] = String_To_সারি(সারি)))
	    {
		যখন (i)
		    vim_free(array[--i]);
		PyMem_Free(array);
		ফেরত FAIL;
	    }
	}

	PyErr_Clear();
	VimTryStart();
	switch_to_win_for_buf(buf, &switchwin, &save_curbuf);

	যদি (u_save((linenr_T)n, (linenr_T)(n + 1)) == FAIL)
	    RAISE_UNDO_FAIL;
	অন্যথা
	{
	    জন্য (i = 0; i < আকার_2; ++i)
	    {
		যদি (ml_append((linenr_T)(n + i),
					(char_u *)array[i], 0, FALSE) == FAIL)
		{
		    RAISE_ঢোকাও_সারি_FAIL;

		    // Free the rest of the lines
		    যখন (i < আকার_2)
			vim_free(array[i++]);

		    বিরতি;
		}
		vim_free(array[i]);
	    }
	    যদি (i > 0 && save_curbuf.br_buf == ফাঁকা)
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
	    ফেরত FAIL;

	যদি (len_change)
	    *len_change = আকার_2;

	ফেরত OK;
    }
    অন্যথা
    {
	PyErr_BadArgument();
	ফেরত FAIL;
    }
}

/*
 * Common routines for buffers and line ranges
 * -------------------------------------------
 */

প্রকারলেখ কাঠামো
{
    Py_Object_শীর্ষ
    buf_T *buf;
} বাফার_Object;

    স্থির পূর্ণসংখ্যা
Check_বাফার(বাফার_Object *self)
{
    যদি (self->buf == INVALID_বাফার_মান)
    {
	PyErr_SET_VIM(N_("attempt to refer to deleted buffer"));
	ফেরত -1;
    }

    ফেরত 0;
}

    স্থির পূর্ণসংখ্যা
Check_বাফার_Cb(শূন্য *self)
{
    ফেরত Check_বাফার((বাফার_Object*)self);
}

    স্থির PyObject *
RBItem(বাফার_Object *self, PyInt n, PyInt start, PyInt end)
{
    যদি (Check_বাফার(self))
	ফেরত ফাঁকা;

    যদি (end == -1)
	end = self->buf->b_ml.ml_সারি_গুনতি;

    যদি (n < 0)
	n += end - start + 1;

    যদি (n < 0 || n > end - start)
    {
	PyErr_SET_STRING(Py_Exc_সূচক_Error, N_("line number out of range"));
	ফেরত ফাঁকা;
    }

    ফেরত Get_বাফার_সারি(self->buf, n+start);
}

    স্থির PyObject *
RBSlice(বাফার_Object *self, PyInt lo, PyInt hi, PyInt start, PyInt end)
{
    PyInt আকার_2;

    যদি (Check_বাফার(self))
	ফেরত ফাঁকা;

    যদি (end == -1)
	end = self->buf->b_ml.ml_সারি_গুনতি;

    আকার_2 = end - start + 1;

    যদি (lo < 0)
	lo = 0;
    অন্যথা যদি (lo > আকার_2)
	lo = আকার_2;
    যদি (hi < 0)
	hi = 0;
    যদি (hi < lo)
	hi = lo;
    অন্যথা যদি (hi > আকার_2)
	hi = আকার_2;

    ফেরত Get_বাফার_সারি_List(self->buf, lo+start, hi+start);
}

    স্থির PyInt
RBAsItem(
	বাফার_Object *self,
	PyInt n,
	PyObject *valObject,
	PyInt start,
	PyInt end,
	PyInt *new_end)
{
    PyInt len_change;

    যদি (Check_বাফার(self))
	ফেরত -1;

    যদি (end == -1)
	end = self->buf->b_ml.ml_সারি_গুনতি;

    যদি (n < 0)
	n += end - start + 1;

    যদি (n < 0 || n > end - start)
    {
	PyErr_SET_STRING(Py_Exc_সূচক_Error, N_("line number out of range"));
	ফেরত -1;
    }

    যদি (Set_বাফার_সারি(self->buf, n+start, valObject, &len_change) == FAIL)
	ফেরত -1;

    যদি (new_end)
	*new_end = end + len_change;

    ফেরত 0;
}

    স্থির PyInt
RBAsSlice(
	বাফার_Object *self,
	PyInt lo,
	PyInt hi,
	PyObject *valObject,
	PyInt start,
	PyInt end,
	PyInt *new_end)
{
    PyInt আকার_2;
    PyInt len_change;

    // Self must be a valid buffer
    যদি (Check_বাফার(self))
	ফেরত -1;

    যদি (end == -1)
	end = self->buf->b_ml.ml_সারি_গুনতি;

    // Sort out the slice range
    আকার_2 = end - start + 1;

    যদি (lo < 0)
	lo = 0;
    অন্যথা যদি (lo > আকার_2)
	lo = আকার_2;
    যদি (hi < 0)
	hi = 0;
    যদি (hi < lo)
	hi = lo;
    অন্যথা যদি (hi > আকার_2)
	hi = আকার_2;

    যদি (Set_বাফার_সারি_List(self->buf, lo + start, hi + start,
						valObject, &len_change) == FAIL)
	ফেরত -1;

    যদি (new_end)
	*new_end = end + len_change;

    ফেরত 0;
}


    স্থির PyObject *
RBAppend(
	বাফার_Object *self,
	PyObject *args,
	PyInt start,
	PyInt end,
	PyInt *new_end)
{
    PyObject *সারিগুলি;
    PyInt len_change;
    PyInt max;
    PyInt n;

    যদি (Check_বাফার(self))
	ফেরত ফাঁকা;

    যদি (end == -1)
	end = self->buf->b_ml.ml_সারি_গুনতি;

    max = n = end - start + 1;

    যদি (!PyArg_ParseTuple(args, "O|n", &সারিগুলি, &n))
	ফেরত ফাঁকা;

    যদি (n < 0 || n > max)
    {
	PyErr_SET_STRING(Py_Exc_সূচক_Error, N_("line number out of range"));
	ফেরত ফাঁকা;
    }

    যদি (ঢোকাও_বাফার_সারিগুলি(self->buf, n + start - 1, সারিগুলি, &len_change) == FAIL)
	ফেরত ফাঁকা;

    যদি (new_end)
	*new_end = end + len_change;

    Py_INCREF(Py_None);
    ফেরত Py_None;
}

// Range object

DEFINE_PY_TYPE_OBJECT(RangeType);
স্থির PySequenceMethods RangeAsSeq;
স্থির PyMappingMethods RangeAsMapping;

প্রকারলেখ কাঠামো
{
    Py_Object_শীর্ষ
    বাফার_Object *buf;
    PyInt start;
    PyInt end;
} RangeObject;

    স্থির PyObject *
RangeNew(buf_T *buf, PyInt start, PyInt end)
{
    বাফার_Object *bufr;
    RangeObject *self;
    self = PyObject_GC_New(RangeObject, RangeTypePtr);
    যদি (self == ফাঁকা)
	ফেরত ফাঁকা;

    bufr = (বাফার_Object *)বাফার_New(buf);
    যদি (bufr == ফাঁকা)
    {
	Py_DECREF(self);
	ফেরত ফাঁকা;
    }
    Py_INCREF((PyObject *)bufr);

    self->buf = bufr;
    self->start = start;
    self->end = end;

    ফেরত (PyObject *)(self);
}

    স্থির শূন্য
RangeDestructor(PyObject *self_obj)
{
    RangeObject *self = (RangeObject*)self_obj;
    PyObject_GC_UnTrack((শূন্য *)(self));
    Py_XDECREF(((PyObject *)(self->buf)));
    PyObject_GC_Del((শূন্য *)(self));
}

    স্থির পূর্ণসংখ্যা
RangeTraverse(PyObject *self_obj, visitproc visit, শূন্য *arg)
{
    RangeObject *self = (RangeObject*)self_obj;
    Py_VISIT(((PyObject *)(self->buf)));
    ফেরত 0;
}

    স্থির পূর্ণসংখ্যা
RangeClear(PyObject *self_obj)
{
    RangeObject *self = (RangeObject*)self_obj;
    Py_CLEAR(self->buf);
    ফেরত 0;
}

    স্থির PyInt
Range_দৈর্ঘ্য(RangeObject *self)
{
    // HOW DO WE SIGNAL AN ERROR FROM THIS FUNCTION?
    যদি (Check_বাফার(self->buf))
	ফেরত -1; // ???

    ফেরত (self->end - self->start + 1);
}

    স্থির PyObject *
RangeItem(RangeObject *self, PyInt n)
{
    ফেরত RBItem(self->buf, n, self->start, self->end);
}

    স্থির PyObject *
RangeSlice(RangeObject *self, PyInt lo, PyInt hi)
{
    ফেরত RBSlice(self->buf, lo, hi, self->start, self->end);
}

স্থির অক্ষর *RangeAttrs[] = {
    "start", "end",
    ফাঁকা
};

    স্থির PyObject *
RangeDir(PyObject *self, PyObject *args UNUSED)
{
    ফেরত ObjectDir(self, RangeAttrs);
}

    স্থির PyObject *
RangeAppend(RangeObject *self, PyObject *args)
{
    ফেরত RBAppend(self->buf, args, self->start, self->end, &self->end);
}

    স্থির PyObject *
RangeRepr(PyObject *self_obj)
{
    RangeObject *self = (RangeObject*)self_obj;
    যদি (self->buf->buf == INVALID_বাফার_মান)
	ফেরত PyString_FromFormat("<range object (for deleted buffer) at %p>",
				    (শূন্য *)self);
    অন্যথা
    {
	অক্ষর *name = (অক্ষর *)self->buf->buf->b_fname;

	যদি (name == ফাঁকা)
	    name = "";

	ফেরত PyString_FromFormat("<range %s (%d:%d)>",
				    name, (পূর্ণসংখ্যা)self->start, (পূর্ণসংখ্যা)self->end);
    }
}

স্থির কাঠামো PyMethodDef RangeMethods[] = {
    // name,	function,			calling,	documentation
    {"append",	(PyCFunction)RangeAppend,	METH_VARARGS,	"Append data to the Vim range" },
    {"__dir__",	(PyCFunction)RangeDir,		METH_NOARGS,	""},
    { ফাঁকা,	ফাঁকা,				0,		ফাঁকা}
};

DEFINE_PY_TYPE_OBJECT(বাফার_Type);
স্থির PySequenceMethods বাফার_As_Seq;
স্থির PyMappingMethods বাফার_As_Mapping;

    স্থির PyObject *
বাফার_New(buf_T *buf)
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

    বাফার_Object *self;

    যদি (BUF_PYTHON_REF(buf) != ফাঁকা)
    {
	self = BUF_PYTHON_REF(buf);
	Py_INCREF((PyObject *)self);
    }
    অন্যথা
    {
	self = PyObject_NEW(বাফার_Object, বাফার_Type_Ptr);
	যদি (self == ফাঁকা)
	    ফেরত ফাঁকা;
	self->buf = buf;
	BUF_PYTHON_REF(buf) = self;
    }

    ফেরত (PyObject *)(self);
}

    স্থির শূন্য
বাফার_Destructor(PyObject *self_obj)
{
    বাফার_Object *self = (বাফার_Object*)self_obj;
    যদি (self->buf && self->buf != INVALID_বাফার_মান)
	BUF_PYTHON_REF(self->buf) = ফাঁকা;

    DESTRUCTOR_FINISH(self);
}

    স্থির PyInt
বাফার_দৈর্ঘ্য(বাফার_Object *self)
{
    // HOW DO WE SIGNAL AN ERROR FROM THIS FUNCTION?
    যদি (Check_বাফার(self))
	ফেরত -1; // ???

    ফেরত (PyInt)(self->buf->b_ml.ml_সারি_গুনতি);
}

    স্থির PyObject *
বাফার_Item(বাফার_Object *self, PyInt n)
{
    ফেরত RBItem(self, n, 1, -1);
}

    স্থির PyObject *
বাফার_Slice(বাফার_Object *self, PyInt lo, PyInt hi)
{
    ফেরত RBSlice(self, lo, hi, 1, -1);
}

স্থির অক্ষর *বাফার_Attrs[] = {
    "name", "number", "vars", "options", "valid",
    ফাঁকা
};

    স্থির PyObject *
বাফার_Dir(PyObject *self, PyObject *args UNUSED)
{
    ফেরত ObjectDir(self, বাফার_Attrs);
}

    স্থির PyObject *
বাফার_Attr_Valid(বাফার_Object *self, অক্ষর *name)
{
    PyObject	*ret;

    যদি (strcmp(name, "valid") != 0)
	ফেরত ফাঁকা;

    ret = ((self->buf == INVALID_বাফার_মান) ? Py_False : Py_True);
    Py_INCREF(ret);
    ফেরত ret;
}

    স্থির PyObject *
বাফার_Attr(বাফার_Object *self, অক্ষর *name)
{
    যদি (strcmp(name, "name") == 0)
	ফেরত PyString_FromString((self->buf->b_ffname == ফাঁকা
				    ? "" : (অক্ষর *)self->buf->b_ffname));
    অন্যথা যদি (strcmp(name, "number") == 0)
	ফেরত Py_Build_মান(Py_ssize_t_fmt, self->buf->b_fnum);
    অন্যথা যদি (strcmp(name, "vars") == 0)
	ফেরত NEW_DICTIONARY(self->buf->b_vars);
    অন্যথা যদি (strcmp(name, "options") == 0)
	ফেরত OptionsNew(SREQ_BUF, self->buf, Check_বাফার_Cb,
			(PyObject *) self);
    অন্যথা যদি (strcmp(name, "__members__") == 0)
	ফেরত ObjectDir(ফাঁকা, বাফার_Attrs);
    অন্যথা
	ফেরত ফাঁকা;
}

    স্থির পূর্ণসংখ্যা
বাফার_Setattr(PyObject *self_obj, অক্ষর *name, PyObject *valObject)
{
    বাফার_Object *self = (বাফার_Object*)self_obj;
    যদি (Check_বাফার(self))
	ফেরত -1;

    যদি (strcmp(name, "name") == 0)
    {
	char_u		*val;
	aco_save_T	aco;
	পূর্ণসংখ্যা		ren_ret = OK;
	PyObject	*todecref;

	যদি (!(val = StringToChars(valObject, &todecref)))
	    ফেরত -1;

	VimTryStart();
	// Using aucmd_*: autocommands will be executed by rename_buffer
	aucmd_prepbuf(&aco, self->buf);
	যদি (curbuf == self->buf)
	{
	    ren_ret = rename_বাফার(val);
	    aucmd_restbuf(&aco);
	}
	Py_XDECREF(todecref);
	যদি (VimTryEnd())
	    ফেরত -1;

	যদি (ren_ret == FAIL)
	{
	    PyErr_SET_VIM(N_("failed to rename buffer"));
	    ফেরত -1;
	}
	ফেরত 0;
    }
    অন্যথা
    {
	PyErr_SetString(PyExc_AttributeError, name);
	ফেরত -1;
    }
}

    স্থির PyObject *
বাফার_Append(বাফার_Object *self, PyObject *args)
{
    ফেরত RBAppend(self, args, 1, -1, ফাঁকা);
}

    স্থির PyObject *
বাফার_Mark(বাফার_Object *self, PyObject *pmarkObject)
{
    pos_T	*posp;
    char_u	*pmark;
    char_u	mark;
    bufref_T	savebuf;
    PyObject	*todecref;

    যদি (Check_বাফার(self))
	ফেরত ফাঁকা;

    যদি (!(pmark = StringToChars(pmarkObject, &todecref)))
	ফেরত ফাঁকা;

    যদি (pmark[0] == '\0' || pmark[1] != '\0')
    {
	PyErr_SET_STRING(Py_Exc_মান_Error,
		N_("mark name must be a single character"));
	Py_XDECREF(todecref);
	ফেরত ফাঁকা;
    }

    mark = *pmark;

    Py_XDECREF(todecref);

    VimTryStart();
    switch_বাফার(&savebuf, self->buf);
    posp = getmark(mark, FALSE);
    restore_বাফার(&savebuf);
    যদি (VimTryEnd())
	ফেরত ফাঁকা;

    যদি (posp == ফাঁকা)
    {
	PyErr_SET_VIM(N_("invalid mark name"));
	ফেরত ফাঁকা;
    }

    যদি (posp->lnum <= 0)
    {
	// Or raise an error?
	Py_INCREF(Py_None);
	ফেরত Py_None;
    }

    ফেরত Py_Build_মান("(ll)", (long)(posp->lnum), (long)(posp->col));
}

    স্থির PyObject *
বাফার_Range(বাফার_Object *self, PyObject *args)
{
    PyInt start;
    PyInt end;

    যদি (Check_বাফার(self))
	ফেরত ফাঁকা;

    যদি (!PyArg_ParseTuple(args, "nn", &start, &end))
	ফেরত ফাঁকা;

    ফেরত RangeNew(self->buf, start, end);
}

    স্থির PyObject *
বাফার_Repr(PyObject *self_obj)
{
    বাফার_Object *self = (বাফার_Object*)self_obj;
    যদি (self->buf == INVALID_বাফার_মান)
	ফেরত PyString_FromFormat("<buffer object (deleted) at %p>", (শূন্য *)self);
    অন্যথা
    {
	অক্ষর	*name = (অক্ষর *)self->buf->b_fname;

	যদি (name == ফাঁকা)
	    name = "";

	ফেরত PyString_FromFormat("<buffer %s>", name);
    }
}

স্থির কাঠামো PyMethodDef বাফার_Methods[] = {
    // name,	    function,			calling,	documentation
    {"append",	    (PyCFunction)বাফার_Append,	METH_VARARGS,	"Append data to Vim buffer" },
    {"mark",	    (PyCFunction)বাফার_Mark,	METH_O,		"Return (row,col) representing position of named mark" },
    {"range",	    (PyCFunction)বাফার_Range,	METH_VARARGS,	"Return a range object which represents the part of the given buffer between line numbers s and e" },
    {"__dir__",	    (PyCFunction)বাফার_Dir,	METH_NOARGS,	""},
    { ফাঁকা,	    ফাঁকা,			0,		ফাঁকা}
};

/*
 * Buffer list object - Implementation
 */

DEFINE_PY_TYPE_OBJECT(BufMapType);

প্রকারলেখ কাঠামো
{
    Py_Object_শীর্ষ
} BufMapObject;

স্থির BufMapObject The_বাফার_Map =
{
    Py_Object_শীর্ষ_INIT_TYPE(BufMapType)
};

    স্থির PyInt
Buf_Map_দৈর্ঘ্য(PyObject *self UNUSED)
{
    buf_T	*b = firstbuf;
    PyInt	n = 0;

    যখন (b)
    {
	++n;
	b = b->b_পরবর্তী;
    }

    ফেরত n;
}

    স্থির PyObject *
BufMapItem(PyObject *self UNUSED, PyObject *চাবি_Object)
{
    buf_T	*b;
    long	bnr;

    যদি (সংখ্যা_To_Long(চাবি_Object, &bnr, সংখ্যা_INT|সংখ্যা_NATURAL))
	ফেরত ফাঁকা;

    b = buflist_findnr((পূর্ণসংখ্যা) bnr);

    যদি (b)
	ফেরত বাফার_New(b);
    অন্যথা
    {
	PyErr_SetObject(Py_Exc_চাবি_Error, চাবি_Object);
	ফেরত ফাঁকা;
    }
}

    স্থির শূন্য
BufMapIterDestruct(শূন্য* arg)
{
    PyObject *বাফার = (PyObject*)arg;
    // Iteration was stopped before all buffers were processed
    যদি (বাফার)
    {
	Py_DECREF(বাফার);
    }
}

    স্থির পূর্ণসংখ্যা
BufMapIterTraverse(শূন্য *iter, visitproc visit, শূন্য *arg)
{
    PyObject *বাফার = (PyObject*)iter;
    যদি (বাফার)
	Py_VISIT(বাফার);
    ফেরত 0;
}

    স্থির পূর্ণসংখ্যা
BufMapIterClear(শূন্য **iter)
{
    PyObject **বাফার = (PyObject**)iter;
    যদি (*বাফার)
	Py_CLEAR(*বাফার);
    ফেরত 0;
}

    স্থির PyObject *
Buf_Map_Iter_পরবর্তী(শূন্য **arg)
{
    PyObject	*পরবর্তী;
    PyObject	*ret;
    PyObject	**বাফার = (PyObject**)arg;

    যদি (!*বাফার)
	ফেরত ফাঁকা;

    ret = *বাফার;

    যদি (Check_বাফার((বাফার_Object *)(ret)))
    {
	*বাফার = ফাঁকা;
	ফেরত ফাঁকা;
    }

    যদি (!((বাফার_Object *)(ret))->buf->b_পরবর্তী)
	পরবর্তী = ফাঁকা;
    অন্যথা যদি (!(পরবর্তী = বাফার_New(((বাফার_Object *)(ret))->buf->b_পরবর্তী)))
	ফেরত ফাঁকা;
    *বাফার = পরবর্তী;
    // Do not increment reference: we no longer hold it (decref), but whoever
    // on other side will hold (incref). Decref+incref = nothing.
    ফেরত ret;
}

    স্থির PyObject *
BufMapIter(PyObject *self)
{
    PyObject *বাফার;

    বাফার = বাফার_New(firstbuf);
    ফেরত IterNew(বাফার,
	    BufMapIterDestruct, Buf_Map_Iter_পরবর্তী,
	    BufMapIterTraverse, BufMapIterClear,
	    self);
}

স্থির PyMappingMethods BufMapAsMapping = {
    (lenfunc)       Buf_Map_দৈর্ঘ্য,
    (binaryfunc)    BufMapItem,
    (objobjargproc) 0,
};

// Current items object

স্থির অক্ষর *CurrentAttrs[] = {
    "buffer", "window", "line", "range", "tabpage",
    ফাঁকা
};

    স্থির PyObject *
CurrentDir(PyObject *self, PyObject *args UNUSED)
{
    ফেরত ObjectDir(self, CurrentAttrs);
}

    স্থির PyObject *
CurrentGetattr(PyObject *self UNUSED, অক্ষর *name)
{
    যদি (strcmp(name, "buffer") == 0)
	ফেরত (PyObject *)বাফার_New(curbuf);
    অন্যথা যদি (strcmp(name, "window") == 0)
	ফেরত (PyObject *)WindowNew(curwin, curtab);
    অন্যথা যদি (strcmp(name, "tabpage") == 0)
	ফেরত (PyObject *)TabPageNew(curtab);
    অন্যথা যদি (strcmp(name, "line") == 0)
	ফেরত Get_বাফার_সারি(curbuf, (PyInt)curwin->w_cursor.lnum);
    অন্যথা যদি (strcmp(name, "range") == 0)
	ফেরত RangeNew(curbuf, RangeStart, RangeEnd);
    অন্যথা যদি (strcmp(name, "__members__") == 0)
	ফেরত ObjectDir(ফাঁকা, CurrentAttrs);
    অন্যথা
#যদি PY_MAJOR_VERSION < 3
	ফেরত Py_FindMethod(WindowMethods, self, name);
#অন্যথা
	ফেরত ফাঁকা;
#endif
}

    স্থির পূর্ণসংখ্যা
CurrentSetattr(PyObject *self UNUSED, অক্ষর *name, PyObject *valObject)
{
    যদি (strcmp(name, "line") == 0)
    {
	যদি (Set_বাফার_সারি(curbuf, (PyInt)curwin->w_cursor.lnum, valObject,
			  ফাঁকা) == FAIL)
	    ফেরত -1;

	ফেরত 0;
    }
    অন্যথা যদি (strcmp(name, "buffer") == 0)
    {
	পূর্ণসংখ্যা গুনতি;

	যদি (valObject->ob_type != বাফার_Type_Ptr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.Buffer object, but got %s"),
		    valObject);
	    ফেরত -1;
	}

	যদি (Check_বাফার((বাফার_Object *)(valObject)))
	    ফেরত -1;
	গুনতি = ((বাফার_Object *)(valObject))->buf->b_fnum;

	VimTryStart();
	যদি (do_বাফার(DOBUF_GOTO, DOBUF_FIRST, FORWARD, গুনতি, 0) == FAIL)
	{
	    যদি (VimTryEnd())
		ফেরত -1;
	    PyErr_VIM_FORMAT(N_("failed to switch to buffer %d"), গুনতি);
	    ফেরত -1;
	}

	ফেরত VimTryEnd();
    }
    অন্যথা যদি (strcmp(name, "window") == 0)
    {
	পূর্ণসংখ্যা গুনতি;

	যদি (valObject->ob_type != WindowTypePtr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.Window object, but got %s"),
		    valObject);
	    ফেরত -1;
	}

	যদি (CheckWindow((WindowObject *)(valObject)))
	    ফেরত -1;
	গুনতি = get_win_সংখ্যা(((WindowObject *)(valObject))->win, firstwin);

	যদি (!গুনতি)
	{
	    PyErr_SET_STRING(Py_Exc_মান_Error,
		    N_("failed to find window in the current tab page"));
	    ফেরত -1;
	}

	VimTryStart();
	win_goto(((WindowObject *)(valObject))->win);
	যদি (((WindowObject *)(valObject))->win != curwin)
	{
	    যদি (VimTryEnd())
		ফেরত -1;
	    PyErr_SET_STRING(PyExc_RuntimeError,
		    N_("did not switch to the specified window"));
	    ফেরত -1;
	}

	ফেরত VimTryEnd();
    }
    অন্যথা যদি (strcmp(name, "tabpage") == 0)
    {
	যদি (valObject->ob_type != TabPageTypePtr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.TabPage object, but got %s"),
		    valObject);
	    ফেরত -1;
	}

	যদি (CheckTabPage((TabPageObject *)(valObject)))
	    ফেরত -1;

	VimTryStart();
	goto_tabpage_tp(((TabPageObject *)(valObject))->tab, TRUE, TRUE);
	যদি (((TabPageObject *)(valObject))->tab != curtab)
	{
	    যদি (VimTryEnd())
		ফেরত -1;
	    PyErr_SET_STRING(PyExc_RuntimeError,
		    N_("did not switch to the specified tab page"));
	    ফেরত -1;
	}

	ফেরত VimTryEnd();
    }
    অন্যথা
    {
	PyErr_SetString(PyExc_AttributeError, name);
	ফেরত -1;
    }
}

স্থির কাঠামো PyMethodDef CurrentMethods[] = {
    // name,	    function,			calling,	documentation
    {"__dir__",	    (PyCFunction)CurrentDir,	METH_NOARGS,	""},
    { ফাঁকা,	    ফাঁকা,			0,		ফাঁকা}
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
run_cmd(ধ্রুবক অক্ষর *cmd, dict_T* locals UNUSED, শূন্য *arg UNUSED
#ifdef PY_CAN_RECURSE
	, PyGILState_STATE *pygilstate UNUSED
#endif
	)
{
    PyObject	*run_ret;
    run_ret = PyRun_String((অক্ষর *)cmd, Py_file_প্রবেশ, globals, globals);
    যদি (run_ret != ফাঁকা)
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

স্থির ধ্রুবক অক্ষর	*code_hdr = "def " DOPY_FUNC "(line, linenr):\n ";
স্থির পূর্ণসংখ্যা		code_hdr_len = 30;

    স্থির শূন্য
run_do(ধ্রুবক অক্ষর *cmd, dict_T* locals UNUSED, শূন্য *arg UNUSED
#ifdef PY_CAN_RECURSE
	, PyGILState_STATE *pygilstate
#endif
	)
{
    PyInt	lnum;
    size_t	len;
    অক্ষর	*code;
    পূর্ণসংখ্যা		status;
    PyObject	*pyfunc, *pymain;
    PyObject	*run_ret;
    buf_T	*was_curbuf = curbuf;

    যদি (u_save((linenr_T)RangeStart - 1, (linenr_T)RangeEnd + 1) != OK)
    {
	emsg(_("cannot save undo information"));
	ফেরত;
    }

    len = code_hdr_len + STRLEN(cmd);
    code = PyMem_New(অক্ষর, len + 1);
    memcpy(code, code_hdr, code_hdr_len);
    STRCPY(code + code_hdr_len, cmd);
    run_ret = PyRun_String(code, Py_file_প্রবেশ, globals, globals);
    status = -1;
    যদি (run_ret != ফাঁকা)
    {
	status = 0;
	Py_DECREF(run_ret);
    }
    অন্যথা যদি (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
    {
	PyMem_Free(code);
	emsg(_(e_cant_handle_systemexit_of_python_exception_in_vim));
	PyErr_Clear();
	ফেরত;
    }
    অন্যথা
	PyErr_PrintEx(1);

    PyMem_Free(code);

    যদি (status)
    {
	emsg(_("failed to run the code"));
	ফেরত;
    }

    status = 0;
    pymain = PyImport_AddModule("__main__");
    pyfunc = PyObject_GetAttrString(pymain, DOPY_FUNC);
#ifdef PY_CAN_RECURSE
    PyGILState_Release(*pygilstate);
#endif

    জন্য (lnum = RangeStart; lnum <= RangeEnd; ++lnum)
    {
	PyObject	*সারি;
	PyObject	*linenr;
	PyObject	*ret;

#ifdef PY_CAN_RECURSE
	*pygilstate = PyGILState_Ensure();
#endif
	// Check the line number, the command my have deleted lines.
	যদি (lnum > curbuf->b_ml.ml_সারি_গুনতি
		|| !(সারি = Get_বাফার_সারি(curbuf, lnum)))
	    যাও err;
	যদি (!(linenr = PyInt_FromLong((long) lnum)))
	{
	    Py_DECREF(সারি);
	    যাও err;
	}
	ret = PyObject_CallFunctionObjArgs(pyfunc, সারি, linenr, ফাঁকা);
	Py_DECREF(সারি);
	Py_DECREF(linenr);
	যদি (!ret)
	    যাও err;

	// Check that the command didn't switch to another buffer.
	// Check the line number, the command my have deleted lines.
	যদি (curbuf != was_curbuf || lnum > curbuf->b_ml.ml_সারি_গুনতি)
	{
	    Py_XDECREF(ret);
	    যাও err;
	}

	যদি (ret != Py_None)
	    যদি (Set_বাফার_সারি(curbuf, lnum, ret, ফাঁকা) == FAIL)
	    {
		Py_XDECREF(ret);
		যাও err;
	    }

	Py_XDECREF(ret);
	PythonIO_Flush();
#ifdef PY_CAN_RECURSE
	PyGILState_Release(*pygilstate);
#endif
    }
    যাও out;
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
    PyObject_SetAttrString(pymain, DOPY_FUNC, ফাঁকা);
    যদি (status)
	ফেরত;
    check_cursor();
    update_curbuf(UPD_NOT_VALID);
}

    স্থির শূন্য
run_eval(ধ্রুবক অক্ষর *cmd, dict_T *locals, শূন্য *arg
#ifdef PY_CAN_RECURSE
	, PyGILState_STATE *pygilstate UNUSED
#endif
	)
{
    PyObject	*run_ret;
    typval_T	*rettv = (typval_T*)arg;
    PyObject	*pylocals = locals ? NEW_DICTIONARY(locals) : globals;

    run_ret = PyRun_String((অক্ষর *)cmd, Py_eval_প্রবেশ, globals, pylocals);
    যদি (run_ret == ফাঁকা)
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

    স্থির পূর্ণসংখ্যা
set_ref_in_py(ধ্রুবক পূর্ণসংখ্যা copyID)
{
    pylinkedlist_T	*cur;
    list_T		*ll;
    tuple_T		*tt;
    পূর্ণসংখ্যা			i;
    পূর্ণসংখ্যা			abort = FALSE;
    FunctionObject	*func;

    যদি (lastdict != ফাঁকা)
    {
	জন্য (cur = lastdict ; !abort && cur != ফাঁকা ; cur = cur->pll_পূর্ব)
	    abort = set_ref_in_dict(((DictionaryObject *)(cur->pll_obj))->dict,
								       copyID);
    }

    যদি (lastlist != ফাঁকা)
    {
	জন্য (cur = lastlist ; !abort && cur != ফাঁকা ; cur = cur->pll_পূর্ব)
	{
	    ll = ((ListObject *) (cur->pll_obj))->list;
	    abort = set_ref_in_list(ll, copyID);
	}
    }

    যদি (lasttuple != ফাঁকা)
    {
	জন্য (cur = lasttuple ; !abort && cur != ফাঁকা ; cur = cur->pll_পূর্ব)
	{
	    tt = ((TupleObject *) (cur->pll_obj))->tuple;
	    abort = set_ref_in_tuple(tt, copyID);
	}
    }

    যদি (lastfunc != ফাঁকা)
    {
	জন্য (cur = lastfunc ; !abort && cur != ফাঁকা ; cur = cur->pll_পূর্ব)
	{
	    func = (FunctionObject *) cur->pll_obj;
	    abort = set_ref_in_dict(func->self, copyID);
	    যদি (func->argc)
		জন্য (i = 0; !abort && i < func->argc; ++i)
		    abort = abort
			|| set_ref_in_item(&func->argv[i], copyID, ফাঁকা, ফাঁকা, ফাঁকা);
	}
    }

    ফেরত abort;
}

    স্থির পূর্ণসংখ্যা
set_string_copy(char_u *str, typval_T *tv)
{
    tv->vval.v_string = vim_strsave(str);
    যদি (tv->vval.v_string == ফাঁকা)
    {
	PyErr_NoMemory();
	ফেরত -1;
    }
    ফেরত 0;
}

    স্থির পূর্ণসংখ্যা
pydict_to_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    dict_T	*dict;
    char_u	*চাবি;
    dictitem_T	*di;
    PyObject	*চাবি_Object;
    PyObject	*valObject;
    Py_ssize_t	iter = 0;

    যদি (!(dict = py_dict_alloc()))
	ফেরত -1;

    tv->v_type = VAR_DICT;
    tv->vval.v_dict = dict;

    যখন (Py_Dict_পরবর্তী(obj, &iter, &চাবি_Object, &valObject))
    {
	PyObject	*todecref = ফাঁকা;

	যদি (চাবি_Object == ফাঁকা || valObject == ফাঁকা)
	{
	    dict_unref(dict);
	    ফেরত -1;
	}

	যদি (!(চাবি = StringToChars(চাবি_Object, &todecref)))
	{
	    dict_unref(dict);
	    ফেরত -1;
	}

	যদি (*চাবি == NUL)
	{
	    dict_unref(dict);
	    Py_XDECREF(todecref);
	    RAISE_NO_EMPTY_চাবিগুলি;
	    ফেরত -1;
	}

	di = dictitem_alloc(চাবি);

	Py_XDECREF(todecref);

	যদি (di == ফাঁকা)
	{
	    PyErr_NoMemory();
	    dict_unref(dict);
	    ফেরত -1;
	}

	যদি (_ConvertFromPyObject(valObject, &di->di_tv, lookup_dict) == -1)
	{
	    vim_free(di);
	    dict_unref(dict);
	    ফেরত -1;
	}

	যদি (dict_add(dict, di) == FAIL)
	{
	    RAISE_চাবি_ADD_FAIL(di->di_চাবি);
	    clear_tv(&di->di_tv);
	    vim_free(di);
	    dict_unref(dict);
	    ফেরত -1;
	}
    }

    --dict->dv_refcount;
    ফেরত 0;
}

    স্থির পূর্ণসংখ্যা
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
	ফেরত -1;

    tv->v_type = VAR_DICT;
    tv->vval.v_dict = dict;

    যদি (!(list = Py_Mapping_চাবিগুলি(obj)))
    {
	dict_unref(dict);
	ফেরত -1;
    }

    যদি (!(iterator = PyObject_GetIter(list)))
    {
	dict_unref(dict);
	Py_DECREF(list);
	ফেরত -1;
    }
    Py_DECREF(list);

    যখন ((চাবি_Object = Py_Iter_পরবর্তী(iterator)))
    {
	PyObject	*todecref;

	যদি (!(চাবি = StringToChars(চাবি_Object, &todecref)))
	{
	    Py_DECREF(চাবি_Object);
	    Py_DECREF(iterator);
	    dict_unref(dict);
	    ফেরত -1;
	}

	যদি (*চাবি == NUL)
	{
	    Py_DECREF(চাবি_Object);
	    Py_DECREF(iterator);
	    Py_XDECREF(todecref);
	    dict_unref(dict);
	    RAISE_NO_EMPTY_চাবিগুলি;
	    ফেরত -1;
	}

	যদি (!(valObject = PyObject_GetItem(obj, চাবি_Object)))
	{
	    Py_DECREF(চাবি_Object);
	    Py_DECREF(iterator);
	    Py_XDECREF(todecref);
	    dict_unref(dict);
	    ফেরত -1;
	}

	di = dictitem_alloc(চাবি);

	Py_DECREF(চাবি_Object);
	Py_XDECREF(todecref);

	যদি (di == ফাঁকা)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(valObject);
	    dict_unref(dict);
	    PyErr_NoMemory();
	    ফেরত -1;
	}

	যদি (_ConvertFromPyObject(valObject, &di->di_tv, lookup_dict) == -1)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(valObject);
	    vim_free(di);
	    dict_unref(dict);
	    ফেরত -1;
	}

	Py_DECREF(valObject);

	যদি (dict_add(dict, di) == FAIL)
	{
	    RAISE_চাবি_ADD_FAIL(di->di_চাবি);
	    Py_DECREF(iterator);
	    dictitem_free(di);
	    dict_unref(dict);
	    ফেরত -1;
	}
    }
    Py_DECREF(iterator);
    --dict->dv_refcount;
    ফেরত 0;
}

    স্থির পূর্ণসংখ্যা
pyseq_to_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    list_T	*l;

    যদি (!(l = py_list_alloc()))
	ফেরত -1;

    tv->v_type = VAR_LIST;
    tv->vval.v_list = l;

    যদি (list_py_concat(l, obj, lookup_dict) == -1)
    {
	list_unref(l);
	ফেরত -1;
    }

    --l->lv_refcount;
    ফেরত 0;
}

    স্থির পূর্ণসংখ্যা
pytuple_to_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    tuple_T	*t;

    যদি (!(t = py_tuple_alloc()))
	ফেরত -1;

    tv->v_type = VAR_TUPLE;
    tv->vval.v_tuple = t;

    যদি (tuple_py_concat(t, obj, lookup_dict) == -1)
    {
	tuple_unref(t);
	ফেরত -1;
    }

    --t->tv_refcount;
    ফেরত 0;
}

প্রকারলেখ পূর্ণসংখ্যা (*pytotvfunc)(PyObject *, typval_T *, PyObject *);

    স্থির পূর্ণসংখ্যা
convert_dl(PyObject *obj, typval_T *tv,
				    pytotvfunc py_to_tv, PyObject *lookup_dict)
{
    PyObject	*capsule;
    অক্ষর	hexBuf[আকার(শূন্য *) * 2 + 3];

    sprintf(hexBuf, "%p", (শূন্য *)obj);

#ifdef PY_USE_CAPSULE
    capsule = PyDict_GetItemString(lookup_dict, hexBuf);
#অন্যথা
    capsule = (PyObject *)PyDict_GetItemString(lookup_dict, hexBuf);
#endif
    যদি (capsule == ফাঁকা)
    {
#ifdef PY_USE_CAPSULE
	capsule = PyCapsule_New(tv, ফাঁকা, ফাঁকা);
#অন্যথা
	capsule = PyCObject_FromVoidPtr(tv, ফাঁকা);
#endif
	যদি (PyDict_SetItemString(lookup_dict, hexBuf, capsule))
	{
	    Py_DECREF(capsule);
	    tv->v_type = VAR_UNKNOWN;
	    ফেরত -1;
	}

	Py_DECREF(capsule);

	যদি (py_to_tv(obj, tv, lookup_dict) == -1)
	{
	    tv->v_type = VAR_UNKNOWN;
	    ফেরত -1;
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
	v = PyCapsule_GetPointer(capsule, ফাঁকা);
#অন্যথা
	v = PyCObject_AsVoidPtr(capsule);
#endif
	copy_tv(v, tv);
    }
    ফেরত 0;
}

    স্থির পূর্ণসংখ্যা
ConvertFromPyMapping(PyObject *obj, typval_T *tv)
{
    PyObject	*lookup_dict;
    পূর্ণসংখ্যা		ret;

    যদি (!(lookup_dict = PyDict_New()))
	ফেরত -1;

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
    ফেরত ret;
}

    স্থির পূর্ণসংখ্যা
ConvertFromPySequence(PyObject *obj, typval_T *tv)
{
    PyObject	*lookup_dict;
    পূর্ণসংখ্যা		ret;

    যদি (!(lookup_dict = PyDict_New()))
	ফেরত -1;

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
    ফেরত ret;
}

    স্থির পূর্ণসংখ্যা
ConvertFromPyObject(PyObject *obj, typval_T *tv)
{
    PyObject	*lookup_dict;
    পূর্ণসংখ্যা		ret;

    যদি (!(lookup_dict = PyDict_New()))
	ফেরত -1;
    ret = _ConvertFromPyObject(obj, tv, lookup_dict);
    Py_DECREF(lookup_dict);
    ফেরত ret;
}

    স্থির পূর্ণসংখ্যা
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
	যদি (func->self != ফাঁকা || func->argv != ফাঁকা)
	{
	    partial_T *pt = ALLOC_CLEAR_ONE(partial_T);

	    set_partial(func, pt, TRUE);
	    tv->vval.v_partial = pt;
	    tv->v_type = VAR_PARTIAL;
	}
	অন্যথা
	{
	    যদি (set_string_copy(func->name, tv) == -1)
		ফেরত -1;

	    tv->v_type = VAR_FUNC;
	}
	func_ref(func->name);
    }
    অন্যথা যদি (PyBytes_Check(obj))
    {
	char_u	*str;

	যদি (Py_Bytes_As_String_And_আকার(obj, (অক্ষর **) &str, ফাঁকা) == -1)
	    ফেরত -1;
	যদি (str == ফাঁকা)
	    ফেরত -1;

	যদি (set_string_copy(str, tv) == -1)
	    ফেরত -1;

	tv->v_type = VAR_STRING;
    }
    অন্যথা যদি (PyUnicode_Check(obj))
    {
	PyObject	*bytes;
	char_u	*str;

	bytes = PyUnicode_AsEncodedString(obj, ENC_OPT, ERRORS_ENCODE_ARG);
	যদি (bytes == ফাঁকা)
	    ফেরত -1;

	যদি (Py_Bytes_As_String_And_আকার(bytes, (অক্ষর **) &str, ফাঁকা) == -1)
	    ফেরত -1;
	যদি (str == ফাঁকা)
	    ফেরত -1;

	যদি (set_string_copy(str, tv))
	{
	    Py_XDECREF(bytes);
	    ফেরত -1;
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
	    ফেরত -1;
    }
#endif
    অন্যথা যদি (PyLong_Check(obj))
    {
	tv->v_type = VAR_সংখ্যা;
	tv->vval.v_সংখ্যা = (varnumber_T) PyLong_AsLong(obj);
	যদি (PyErr_Occurred())
	    ফেরত -1;
    }
    অন্যথা যদি (PyTuple_Check(obj))
	ফেরত convert_dl(obj, tv, pytuple_to_tv, lookup_dict);
    অন্যথা যদি (PyDict_Check(obj))
	ফেরত convert_dl(obj, tv, pydict_to_tv, lookup_dict);
    অন্যথা যদি (PyFloat_Check(obj))
    {
	tv->v_type = VAR_FLOAT;
	tv->vval.v_float = (float_T) PyFloat_AsDouble(obj);
    }
    অন্যথা যদি (PyObject_HasAttrString(obj, "keys"))
	ফেরত convert_dl(obj, tv, pymap_to_tv, lookup_dict);
    // PyObject_GetIter can create built-in iterator for any sequence object
    অন্যথা যদি (PyIter_Check(obj) || PySequence_Check(obj))
	ফেরত convert_dl(obj, tv, pyseq_to_tv, lookup_dict);
    অন্যথা যদি (PyMapping_Check(obj))
	ফেরত convert_dl(obj, tv, pymap_to_tv, lookup_dict);
    অন্যথা যদি (Py_সংখ্যা_Check(obj))
    {
	PyObject	*অঙ্ক;

	যদি (!(অঙ্ক = Py_সংখ্যা_Long(obj)))
	    ফেরত -1;

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
	ফেরত -1;
    }
    ফেরত 0;
}

    স্থির PyObject *
ConvertToPyObject(typval_T *tv)
{
    typval_T *argv;
    পূর্ণসংখ্যা i;
    যদি (tv == ফাঁকা)
    {
	PyErr_SET_VIM(N_("internal error: NULL reference passed"));
	ফেরত ফাঁকা;
    }
    নির্বাচন (tv->v_type)
    {
	ক্ষেত্র VAR_STRING:
	    ফেরত PyBytes_FromString(tv->vval.v_string == ফাঁকা
					    ? "" : (অক্ষর *)tv->vval.v_string);
	ক্ষেত্র VAR_সংখ্যা:
	    ফেরত PyLong_FromLong((long) tv->vval.v_সংখ্যা);
	ক্ষেত্র VAR_FLOAT:
	    ফেরত PyFloat_FromDouble((দ্বিগুণ) tv->vval.v_float);
	ক্ষেত্র VAR_LIST:
	    ফেরত NEW_LIST(tv->vval.v_list);
	ক্ষেত্র VAR_TUPLE:
	    ফেরত NEW_TUPLE(tv->vval.v_tuple);
	ক্ষেত্র VAR_DICT:
	    ফেরত NEW_DICTIONARY(tv->vval.v_dict);
	ক্ষেত্র VAR_FUNC:
	    ফেরত NEW_FUNCTION(tv->vval.v_string == ফাঁকা
					  ? (char_u *)"" : tv->vval.v_string,
					  0, ফাঁকা, ফাঁকা, TRUE);
	ক্ষেত্র VAR_PARTIAL:
	    যদি (tv->vval.v_partial->pt_argc)
	    {
		argv = PyMem_New(typval_T, (size_t)tv->vval.v_partial->pt_argc);
		জন্য (i = 0; i < tv->vval.v_partial->pt_argc; i++)
		    copy_tv(&tv->vval.v_partial->pt_argv[i], &argv[i]);
	    }
	    অন্যথা
		argv = ফাঁকা;
	    যদি (tv->vval.v_partial->pt_dict != ফাঁকা)
		tv->vval.v_partial->pt_dict->dv_refcount++;
	    ফেরত NEW_FUNCTION(tv->vval.v_partial == ফাঁকা
			     ? (char_u *)"" : partial_name(tv->vval.v_partial),
				tv->vval.v_partial->pt_argc, argv,
				tv->vval.v_partial->pt_dict,
				tv->vval.v_partial->pt_auto);
	ক্ষেত্র VAR_BLOB:
	    ফেরত Py_Bytes_From_String_And_আকার(
		(অক্ষর*) tv->vval.v_blob->bv_ga.ga_তথ্য,
		(Py_ssize_t) tv->vval.v_blob->bv_ga.ga_len);
	ক্ষেত্র VAR_UNKNOWN:
	ক্ষেত্র VAR_ANY:
	ক্ষেত্র VAR_VOID:
	ক্ষেত্র VAR_CHANNEL:
	ক্ষেত্র VAR_JOB:
	ক্ষেত্র VAR_INSTR:
	ক্ষেত্র VAR_CLASS:
	ক্ষেত্র VAR_OBJECT:
	ক্ষেত্র VAR_TYPEALIAS:
	    Py_INCREF(Py_None);
	    ফেরত Py_None;
	ক্ষেত্র VAR_BOOL:
	ক্ষেত্র VAR_SPECIAL:
	    নির্বাচন (tv->vval.v_সংখ্যা)
	    {
		ক্ষেত্র VVAL_FALSE: ফেরত ALWAYS_FALSE;
		ক্ষেত্র VVAL_TRUE:  ফেরত ALWAYS_TRUE;
		ক্ষেত্র VVAL_NONE:
		ক্ষেত্র VVAL_NULL:  ফেরত ALWAYS_NONE;
	    }
	    PyErr_SET_VIM(N_("internal error: invalid value type"));
	    ফেরত ফাঁকা;
    }
    ফেরত ফাঁকা;
}

DEFINE_PY_TYPE_OBJECT(CurrentType);

প্রকারলেখ কাঠামো
{
    Py_Object_শীর্ষ
} CurrentObject;

স্থির CurrentObject TheCurrent =
{
    Py_Object_শীর্ষ_INIT_TYPE(CurrentType)
};

    স্থির শূন্য
init_structs(শূন্য)
{
    CLEAR_FIELD(OutputType);
    OutputType.tp_name = "vim.message";
    OutputType.tp_basicsize = আকার(OutputObject);
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
    IterType.tp_basicsize = আকার(IterObject);
    IterType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    IterType.tp_doc = "generic iterator object";
    IterType.tp_iter = IterIter;
    IterType.tp_iternext = Iter_পরবর্তী;
    IterType.tp_dealloc = IterDestructor;
    IterType.tp_traverse = IterTraverse;
    IterType.tp_clear = IterClear;

    CLEAR_FIELD(বাফার_Type);
    বাফার_Type.tp_name = "vim.buffer";
    বাফার_Type.tp_basicsize = আকার(বাফার_Object);
    বাফার_Type.tp_dealloc = বাফার_Destructor;
    বাফার_Type.tp_repr = বাফার_Repr;
    বাফার_Type.tp_as_sequence = &বাফার_As_Seq;
    বাফার_Type.tp_as_mapping = &বাফার_As_Mapping;
    বাফার_Type.tp_flags = Py_TPFLAGS_DEFAULT;
    বাফার_Type.tp_doc = "vim buffer object";
    বাফার_Type.tp_methods = বাফার_Methods;
#যদি PY_MAJOR_VERSION >= 3
    বাফার_Type.tp_getattro = বাফার_Getattro;
    বাফার_Type.tp_setattro = বাফার_Setattro;
    বাফার_Type.tp_alloc = call_PyType_GenericAlloc;
    বাফার_Type.tp_new = call_PyType_GenericNew;
    বাফার_Type.tp_free = call_PyObject_Free;
#অন্যথা
    বাফার_Type.tp_getattr = বাফার_Getattr;
    বাফার_Type.tp_setattr = বাফার_Setattr;
#endif

    CLEAR_FIELD(WindowType);
    WindowType.tp_name = "vim.window";
    WindowType.tp_basicsize = আকার(WindowObject);
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
    TabPageType.tp_basicsize = আকার(TabPageObject);
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
    BufMapType.tp_basicsize = আকার(BufMapObject);
    BufMapType.tp_as_mapping = &BufMapAsMapping;
    BufMapType.tp_flags = Py_TPFLAGS_DEFAULT;
    BufMapType.tp_iter = BufMapIter;
    BufMapType.tp_doc = "vim buffer list";

    CLEAR_FIELD(WinListType);
    WinListType.tp_name = "vim.windowlist";
    WinListType.tp_basicsize = আকার(WinListObject);
    WinListType.tp_as_sequence = &WinListAsSeq;
    WinListType.tp_flags = Py_TPFLAGS_DEFAULT;
    WinListType.tp_doc = "vim window list";
    WinListType.tp_dealloc = (destructor)WinListDestructor;

    CLEAR_FIELD(TabListType);
    TabListType.tp_name = "vim.tabpagelist";
    TabListType.tp_basicsize = আকার(TabListObject);
    TabListType.tp_as_sequence = &TabListAsSeq;
    TabListType.tp_flags = Py_TPFLAGS_DEFAULT;
    TabListType.tp_doc = "vim tab page list";

    CLEAR_FIELD(RangeType);
    RangeType.tp_name = "vim.range";
    RangeType.tp_basicsize = আকার(RangeObject);
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
    CurrentType.tp_basicsize = আকার(CurrentObject);
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
    DictionaryType.tp_basicsize = আকার(DictionaryObject);
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
    ListType.tp_basicsize = আকার(ListObject);
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
    TupleType.tp_basicsize = আকার(TupleObject);
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
    FunctionType.tp_basicsize = আকার(FunctionObject);
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
    OptionsType.tp_basicsize = আকার(OptionsObject);
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
    LoaderType.tp_basicsize = আকার(LoaderObject);
    LoaderType.tp_flags = Py_TPFLAGS_DEFAULT;
    LoaderType.tp_doc = "vim message object";
    LoaderType.tp_methods = LoaderMethods;
    LoaderType.tp_dealloc = LoaderDestructor;
#endif

#যদি PY_MAJOR_VERSION >= 3
    CLEAR_FIELD(vimmodule);
    vimmodule.m_name = "vim";
    vimmodule.m_doc = "Vim Python interface\n";
    vimmodule.m_আকার_2 = -1;
    vimmodule.m_methods = VimMethods;
#endif
}

    স্থির পূর্ণসংখ্যা
init_types(শূন্য)
{
    PYTYPE_READY(IterType);
    PYTYPE_READY(বাফার_Type);
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
    PyObject_FINISH_INIT_TYPE(The_বাফার_Map, BufMapType);
    PyObject_FINISH_INIT_TYPE(TheWindowList, WinListType);
    PyObject_FINISH_INIT_TYPE(TheCurrent, CurrentType);
    PyObject_FINISH_INIT_TYPE(TheTabPageList, TabListType);
#endif
    ফেরত 0;
}

#ifdef USE_LIMITED_API
    স্থির শূন্য
shutdown_types(শূন্য)
{
    PYTYPE_CLEANUP(IterType);
    PYTYPE_CLEANUP(বাফার_Type);
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

    স্থির পূর্ণসংখ্যা
init_sys_path(শূন্য)
{
    PyObject	*path;
    PyObject	*path_hook;
    PyObject	*path_hooks;

    যদি (!(path_hook = PyObject_GetAttrString(vim_module, "path_hook")))
	ফেরত -1;

    যদি (!(path_hooks = PySys_GetObject("path_hooks")))
    {
	PyErr_Clear();
	path_hooks = PyList_New(1);
	PyList_SET_ITEM(path_hooks, 0, path_hook);
	যদি (PySys_SetObject("path_hooks", path_hooks))
	{
	    Py_DECREF(path_hooks);
	    ফেরত -1;
	}
	Py_DECREF(path_hooks);
    }
    অন্যথা যদি (PyList_Check(path_hooks))
    {
	যদি (PyList_Append(path_hooks, path_hook))
	{
	    Py_DECREF(path_hook);
	    ফেরত -1;
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
	ফেরত 0;
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
	    ফেরত -1;
	}
	Py_DECREF(path);
    }
    অন্যথা যদি (PyList_Check(path))
    {
	যদি (PyList_Append(path, vim_special_path_object))
	    ফেরত -1;
    }
    অন্যথা
    {
	VimTryStart();
	emsg(_("Failed to set path: sys.path is not a list\n"
	       "You should now append vim.VIM_SPECIAL_PATH to sys.path"));
	VimTryEnd(); // Discard the error
    }

    ফেরত 0;
}

স্থির কাঠামো numeric_constant {
    অক্ষর	*name;
    পূর্ণসংখ্যা		val;
} numeric_constants[] = {
    {"VAR_LOCKED",	VAR_LOCKED},
    {"VAR_FIXED",	VAR_FIXED},
    {"VAR_SCOPE",	VAR_SCOPE},
    {"VAR_DEF_SCOPE",	VAR_DEF_SCOPE},
};

কাঠামো object_constant {
    অক্ষর	*name;
    PyObject	*valObject;
};

#define ADD_OBJECT(m, name, obj) \
    যদি (PyModule_AddObject(m, name, obj)) \
	ফেরত -1;

#define ADD_CHECKED_OBJECT(m, name, obj) \
    { \
	PyObject	*valObject = obj; \
	যদি (!valObject) \
	    ফেরত -1; \
	ADD_OBJECT(m, name, valObject); \
    }

    স্থির পূর্ণসংখ্যা
populate_module(PyObject *m)
{
    পূর্ণসংখ্যা		i;
    PyObject	*other_module;
    PyObject	*attr;
    PyObject	*imp;
#যদি PY_VERSION_HEX >= 0x030700f0
    PyObject	*dict;
    PyObject	*cls;
#endif

    জন্য (i = 0; i < (পূর্ণসংখ্যা)(আকার(numeric_constants)
					   / আকার(কাঠামো numeric_constant));
	    ++i)
	ADD_CHECKED_OBJECT(m, numeric_constants[i].name,
		PyInt_FromLong(numeric_constants[i].val));

    কাঠামো object_constant object_constants[] = {
	{"buffers",  (PyObject *)(শূন্য *)&The_বাফার_Map},
	{"windows",  (PyObject *)(শূন্য *)&TheWindowList},
	{"tabpages", (PyObject *)(শূন্য *)&TheTabPageList},
	{"current",  (PyObject *)(শূন্য *)&TheCurrent},

	{"Buffer",     (PyObject *)বাফার_Type_Ptr},
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

    জন্য (i = 0; i < (পূর্ণসংখ্যা)(আকার(object_constants)
					    / আকার(কাঠামো object_constant));
	    ++i)
    {
	PyObject	*valObject;

	valObject = object_constants[i].valObject;
	Py_INCREF(valObject);
	ADD_OBJECT(m, object_constants[i].name, valObject);
    }

    যদি (!(VimError = PyErr_NewException("vim.error", ফাঁকা, ফাঁকা)))
	ফেরত -1;
    ADD_OBJECT(m, "error", VimError);

    ADD_CHECKED_OBJECT(m, "vars",  NEW_DICTIONARY(get_globvar_dict()));
    ADD_CHECKED_OBJECT(m, "vvars", NEW_DICTIONARY(get_vimvar_dict()));
    ADD_CHECKED_OBJECT(m, "options",
	    OptionsNew(SREQ_GLOBAL, ফাঁকা, dummy_check, ফাঁকা));

    যদি (!(other_module = PyImport_ImportModule("os")))
	ফেরত -1;
    ADD_OBJECT(m, "os", other_module);

#যদি PY_MAJOR_VERSION >= 3
    যদি (!(py_getcwd = PyObject_GetAttrString(other_module, "getcwd")))
	ফেরত -1;
#অন্যথা
    যদি (!(py_getcwd = PyObject_GetAttrString(other_module, "getcwdu")))
	ফেরত -1;
#endif
    ADD_OBJECT(m, "_getcwd", py_getcwd)

    যদি (!(py_chdir = PyObject_GetAttrString(other_module, "chdir")))
	ফেরত -1;
    ADD_OBJECT(m, "_chdir", py_chdir);
    যদি (!(attr = PyObject_GetAttrString(m, "chdir")))
	ফেরত -1;
    যদি (PyObject_SetAttrString(other_module, "chdir", attr))
    {
	Py_DECREF(attr);
	ফেরত -1;
    }
    Py_DECREF(attr);

    যদি ((py_fchdir = PyObject_GetAttrString(other_module, "fchdir")))
    {
	ADD_OBJECT(m, "_fchdir", py_fchdir);
	যদি (!(attr = PyObject_GetAttrString(m, "fchdir")))
	    ফেরত -1;
	যদি (PyObject_SetAttrString(other_module, "fchdir", attr))
	{
	    Py_DECREF(attr);
	    ফেরত -1;
	}
	Py_DECREF(attr);
    }
    অন্যথা
	PyErr_Clear();

    যদি (!(vim_special_path_object = PyString_FromString(vim_special_path)))
	ফেরত -1;

    ADD_OBJECT(m, "VIM_SPECIAL_PATH", vim_special_path_object);

#যদি PY_VERSION_HEX >= 0x030700f0
    যদি (!(imp = PyImport_ImportModule("importlib.machinery")))
	ফেরত -1;

    dict = PyModule_GetDict(imp);

    যদি (!(cls = PyDict_GetItemString(dict, "PathFinder")))
    {
	Py_DECREF(imp);
	ফেরত -1;
    }

    যদি (!(py_find_spec = PyObject_GetAttrString(cls, "find_spec")))
    {
	Py_DECREF(imp);
	ফেরত -1;
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
	ফেরত -1;

    যদি (!(py_find_module = PyObject_GetAttrString(imp, "find_module")))
    {
	Py_DECREF(imp);
	ফেরত -1;
    }

    যদি (!(py_load_module = PyObject_GetAttrString(imp, "load_module")))
    {
	Py_DECREF(py_find_module);
	Py_DECREF(imp);
	ফেরত -1;
    }

    Py_DECREF(imp);

    ADD_OBJECT(m, "_find_module", py_find_module);
    ADD_OBJECT(m, "_load_module", py_load_module);
#endif

    ফেরত 0;
}
