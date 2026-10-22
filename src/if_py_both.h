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

#اگر PY_نسخہ_HEX < 0x02050000
قسمتعریف صحیح Py_ssize_t;  // Python 2.4 and earlier don't have this type.
#اگر_ختم

// Use values that are known to work, others may make Vim crash.
#تعریف_کرو ENC_OPT (enc_utf8 ? "utf-8" : enc_dbcs ? "euc-jp" : (حرف *)p_enc)
#تعریف_کرو DOPY_FUNC "_vim_pydo"

ساکن ثابت حرف *vim_special_راستہ = "_vim_path_";

#تعریف_کرو Py_Err_سیٹ_STRING(exc, str) Py_Err_سیٹ_String(exc, _(str))
#تعریف_کرو Py_Err_سیٹ_Vim(str) Py_Err_سیٹ_String(VimError, str)
#تعریف_کرو Py_Err_سیٹ_VIM(str) Py_Err_سیٹ_STRING(VimError, str)
#تعریف_کرو PyErr_FORMAT(exc, str, arg) PyErr_Format(exc, _(str), arg)
#تعریف_کرو PyErr_FORMAT2(exc, str, arg1, arg2) PyErr_Format(exc, _(str), arg1,arg2)
#تعریف_کرو PyErr_VIM_FORMAT(str, arg) PyErr_FORMAT(VimError, str, arg)

#اگر_تعریف_شدہ USE_LIMITED_API
// Limited Python API. Need to call only exposed functions and remap macros.
// PyTypeObject is an opaque struct.

قسمتعریف ساخت {
    lenfunc sq_لمبائی;
    binaryfunc sq_concat;
    ssizeargfunc sq_repeat;
    ssizeargfunc sq_شے;
    خالی *was_sq_قاش;
    ssizeobjargproc sq_ass_شے;
    خالی *was_sq_ass_قاش;
    objobjproc sq_شامل_ہے;

    binaryfunc sq_inplace_concat;
    ssizeargfunc sq_inplace_repeat;
} PySequenceMethods;

قسمتعریف ساخت {
    lenfunc mp_لمبائی;
    binaryfunc mp_subscript;
    objobjargproc mp_ass_subscript;
} PyMappingMethods;

// This struct emulates the concrete _typeobject struct to allow the code to
// work the same way in both limited and full Python APIs.
ساخت typeobject_wrapper {
    ثابت حرف *tp_name;
    Py_ssize_t tp_basicsize;
    غیرعلامتی طویل tp_flags;

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

    ساخت Py_طریقہ_Def *tp_methods;
    ساخت _typeobject *tp_بنیاد;
    allocfunc tp_alloc;
    newfunc tp_new;
    freefunc tp_free;
};

# define DEFINE_PY_TYPE_OBJECT(type) \
    ساکن ساخت typeobject_wrapper type; \
    ساکن PyTypeObject* type##Ptr = تہی

// PyObject_HEAD_INIT_TYPE and PyObject_FINISH_INIT_TYPE need to come in pairs
// We first initialize with NULL because the type is not allocated until
// init_types() is called later. It's in FINISH_INIT_TYPE where we fill the
// type in with the newly allocated type.
# define Py_Object_سر_INIT_TYPE(type) Py_Object_سر_INIT(تہی)
# define Py_Object_اختتام_INIT_TYPE(obj, type) obj.ob_بنیاد.ob_type = type##Ptr

# define Py_TYPE_GET_TP_ALLOC(type) ((allocfunc)Py_Type_Get_خانہ(type, Py_tp_alloc))
# define Py_TYPE_GET_TP_METHODS(type) ((Py_طریقہ_Def *)Py_Type_Get_خانہ(type, Py_tp_methods))

// PyObject_NEW is not part of stable ABI, but PyObject_Malloc/Init are.
ساکن PyObject* Vim_PyObject_New(PyTypeObject *type, size_t objsize)
{
    PyObject *obj = (PyObject *)PyObject_Malloc(objsize);
    اگر (obj == تہی)
	واپس Py_Err_No_حافظہ();
    واپس PyObject_Init(obj, type);
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
# define PyErr_FORMAT_TYPE(پیغام, obj) \
    کرو { \
	PyObject* qualname = PyObject_GetAttrString((PyObject*)(obj)->ob_type, "__qualname__"); \
	اگر (qualname == تہی) \
	{ \
	    PyErr_FORMAT(PyExc_TypeError, پیغام, "(NULL)"); \
	    توڑو; \
	} \
	PyObject* module = PyObject_GetAttrString((PyObject*)(obj)->ob_type, "__module__"); \
	PyObject* بھرا; \
	اگر (module == تہی || PyUnicode_CompareWithASCIIString(module, "builtins") == 0 \
		|| PyUnicode_CompareWithASCIIString(module, "__main__") == 0) \
	{ \
	    بھرا = qualname; \
	    Py_INCREF(بھرا); \
	} \
	ورنہ \
	    بھرا = PyUnicode_FromFormat("%U.%U", module, qualname); \
	PyObject* بھرا_بائٹیں = PyUnicode_AsUTF8String(بھرا); \
	ثابت حرف* بھرا_str = Py_بائٹیں_As_String(بھرا_بائٹیں); \
	بھرا_str = بھرا_str == تہی ? "(NULL)" : بھرا_str; \
	PyErr_FORMAT(PyExc_TypeError, پیغام, بھرا_str); \
	Py_DECREF(qualname); \
	Py_XDECREF(module); \
	Py_XDECREF(بھرا); \
	Py_XDECREF(بھرا_بائٹیں); \
    } جب (0)

# define Py_List_GET_شے(list, i) Py_List_Get_شے(list, i)
# define Py_List_GET_سائز(o) Py_List_سائز(o)

// PyList_SET_ITEM and PyList_SetItem have slightly different behaviors. The
// former will leave the old item dangling, and the latter will decref on it.
// Since we only use this on new lists, this difference doesn't matter.
# define Py_List_سیٹ_شے(list, i, شے) Py_List_سیٹ_شے_2(list, i, شے)

# define Py_Tuple_GET_شے(o, pos) Py_Tuple_Get_شے(o, pos)
# define Py_Tuple_GET_سائز(o) Py_Tuple_سائز(o)

# اگر Py_LIMITED_API < 0x03080000
// PyIter_check only became part of stable ABI in 3.8, and there is no easy way
// to check for it in the API. We simply return false as a compromise. This
// does mean we should avoid compiling with stable ABI < 3.8.
#  undef PyIter_Check
#  define PyIter_Check(obj) (FALSE)
# endif

ساکن PyTypeObject* جوڑو_ڈھیر_Type(ساخت typeobject_wrapper* type_object)
{
    PyType_Spec type_spec;
    type_spec.name = type_object->tp_name;
    type_spec.basicsize = type_object->tp_basicsize;
    type_spec.itemsize = 0;
    type_spec.flags = type_object->tp_flags;

    // We just need to statically allocate a large enough buffer that can hold
    // all slots. We need to leave a null-terminated slot at the end.
    Py_Type_خانہ slots[40] = { {0, تہی} };
    size_t خانہ_i = 0;

# define جوڑو_TP_خانہ(خانہ_name) \
    اگر (خانہ_i >= 40) واپس تہی; /* this should never happen */ \
    اگر (type_object->خانہ_name != تہی) \
    { \
	slots[خانہ_i].خانہ = Py_##slot_name; \
	slots[خانہ_i].pfunc = (خالی*)type_object->خانہ_name; \
	++خانہ_i; \
    }
# define جوڑو_TP_SUB_خانہ(sub_خانہ, خانہ_name) \
    اگر (خانہ_i >= 40) واپس تہی; /* this should never happen */ \
    اگر (type_object->sub_خانہ != تہی && type_object->sub_خانہ->خانہ_name != تہی) \
    { \
	slots[خانہ_i].خانہ = Py_##slot_name; \
	slots[خانہ_i].pfunc = (خالی*)type_object->sub_خانہ->خانہ_name; \
	++خانہ_i; \
    }

    جوڑو_TP_خانہ(tp_dealloc)
    جوڑو_TP_خانہ(tp_repr)
    جوڑو_TP_خانہ(tp_call)
    جوڑو_TP_خانہ(tp_getattro)
    جوڑو_TP_خانہ(tp_setattro)
    جوڑو_TP_خانہ(tp_doc)
    جوڑو_TP_خانہ(tp_traverse)
    جوڑو_TP_خانہ(tp_clear)
    جوڑو_TP_خانہ(tp_iter)
    جوڑو_TP_خانہ(tp_iternext)
    جوڑو_TP_خانہ(tp_methods)
    جوڑو_TP_خانہ(tp_بنیاد)
    جوڑو_TP_خانہ(tp_alloc)
    جوڑو_TP_خانہ(tp_new)
    جوڑو_TP_خانہ(tp_free)

    جوڑو_TP_SUB_خانہ(tp_as_sequence, sq_لمبائی)
    جوڑو_TP_SUB_خانہ(tp_as_sequence, sq_concat)
    جوڑو_TP_SUB_خانہ(tp_as_sequence, sq_repeat)
    جوڑو_TP_SUB_خانہ(tp_as_sequence, sq_شے)
    جوڑو_TP_SUB_خانہ(tp_as_sequence, sq_ass_شے)
    جوڑو_TP_SUB_خانہ(tp_as_sequence, sq_شامل_ہے)
    جوڑو_TP_SUB_خانہ(tp_as_sequence, sq_inplace_concat)
    جوڑو_TP_SUB_خانہ(tp_as_sequence, sq_inplace_repeat)

    جوڑو_TP_SUB_خانہ(tp_as_mapping, mp_لمبائی)
    جوڑو_TP_SUB_خانہ(tp_as_mapping, mp_subscript)
    جوڑو_TP_SUB_خانہ(tp_as_mapping, mp_ass_subscript)
# undef جوڑو_TP_خانہ
# undef جوڑو_TP_SUB_خانہ

    type_spec.slots = slots;

    PyObject* newtype = PyType_FromSpec(&type_spec);
    واپس (PyTypeObject*)newtype;
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
    type##Ptr = جوڑو_ڈھیر_Type(&(type)); \
    اگر (type##Ptr == تہی) \
	واپس -1;
# define PYTYPE_CLEANUP(type) \
    type##Ptr = تہی;

// Limited API does not provide PyRun_* functions. Need to implement manually
// using PyCompile and PyEval.
ساکن PyObject* Vim_Py_تسلسل_String(ثابت حرف *str, صحیح آغاز, PyObject *globals, PyObject *locals)
{
    // Just pass "" for filename for now.
    PyObject* compiled = Py_CompileString(str, "", آغاز);
    اگر (compiled == تہی)
	واپس تہی;

    PyObject* eval_نتیجہ = Py_Eval_Eval_رمز(compiled, globals, locals);
    Py_DECREF(compiled);
    واپس eval_نتیجہ;
}
ساکن صحیح Vim_Py_تسلسل_Simple_String(ثابت حرف *str)
{
    // This function emulates CPython's implementation.
    PyObject* m = Py_Import_جوڑو_Module("__main__");
    اگر (m == تہی)
	واپس -1;
    PyObject* d = PyModule_GetDict(m);
    PyObject* output = Vim_Py_تسلسل_String(str, Py_file_داخلہ, d, d);
    اگر (output == تہی)
    {
	PyErr_PrintEx(TRUE);
	واپس -1;
    }
    Py_DECREF(output);
    واپس 0;
}
# define Py_تسلسل_String Vim_Py_تسلسل_String
# define Py_تسلسل_Simple_String Vim_Py_تسلسل_Simple_String

#ورنہ // !defined(USE_LIMITED_API)

// Full Python API. Can make use of structs and macros directly.
# define DEFINE_PY_TYPE_OBJECT(type) \
    ساکن PyTypeObject type; \
    ساکن PyTypeObject* type##Ptr = &type
# define Py_Object_سر_INIT_TYPE(type) Py_Object_سر_INIT(&type)

# define Py_TYPE_GET_TP_ALLOC(type) type->tp_alloc
# define Py_TYPE_GET_TP_METHODS(type) type->tp_methods

# define Py_TYPE_NAME(obj) ((obj)->ob_type->tp_name == تہی \
	? "(NULL)" \
	: (obj)->ob_type->tp_name)
# define PyErr_FORMAT_TYPE(پیغام, obj) \
    PyErr_FORMAT(PyExc_TypeError, پیغام, \
	    Py_TYPE_NAME(obj))

// Add a static type
# define PYTYPE_READY(type) \
    اگر (PyType_Ready(type##Ptr)) \
	واپس -1;

#اگر_ختم


#تعریف_کرو RAISE_NO_خالی_خانے_کلیدیں Py_Err_سیٹ_STRING(Py_Exc_قدر_Error, \
					    N_("empty keys are not allowed"))
#تعریف_کرو RAISE_LOCKED_DICTIONARY Py_Err_سیٹ_VIM(N_("dictionary is locked"))
#تعریف_کرو RAISE_LOCKED_LIST Py_Err_سیٹ_VIM(N_("list is locked"))
#تعریف_کرو RAISE_UNDO_FAIL Py_Err_سیٹ_VIM(N_("cannot save undo information"))
#تعریف_کرو RAISE_DELETE_سطر_FAIL Py_Err_سیٹ_VIM(N_("cannot delete line"))
#تعریف_کرو RAISE_داخل_کرو_سطر_FAIL Py_Err_سیٹ_VIM(N_("cannot insert line"))
#تعریف_کرو RAISE_REPLACE_سطر_FAIL Py_Err_سیٹ_VIM(N_("cannot replace line"))
#تعریف_کرو RAISE_کلید_جوڑو_FAIL(کلید) \
    PyErr_VIM_FORMAT(N_("failed to add key '%s' to dictionary"), کلید)
#تعریف_کرو RAISE_INVALID_اشاریہ_TYPE(idx) \
    PyErr_FORMAT_TYPE(N_("index must be int or slice, not %s"), idx);

#تعریف_کرو INVALID_بفر_قدر ((buf_T *)(-1))
#تعریف_کرو INVALID_کھڑکی_قدر ((win_T *)(-1))
#تعریف_کرو INVALID_TABPAGE_قدر ((tabpage_T *)(-1))

قسمتعریف خالی (*rangeinitializer)(خالی *);
قسمتعریف خالی (*runner)(ثابت حرف *, dict_T *, خالی *
#اگر_تعریف_شدہ PY_CAN_RECURSE
	, PyGILState_STATE *
#اگر_ختم
	);

ساکن صحیح ConvertFromPyObject(PyObject *, typval_T *);
ساکن صحیح _ConvertFromPyObject(PyObject *, typval_T *, PyObject *);
ساکن صحیح ConvertFromPyMapping(PyObject *, typval_T *);
ساکن صحیح ConvertFromPySequence(PyObject *, typval_T *);
ساکن PyObject *کھڑکی_New(win_T *, tabpage_T *);
ساکن PyObject *بفر_New (buf_T *);
ساکن PyObject *سطر_مقصد_String(ثابت حرف *);

ساکن PyInt Range_آغاز;
ساکن PyInt Range_انتہا;

ساکن PyObject *globals;

ساکن PyObject *py_chdir;
ساکن PyObject *py_fchdir;
ساکن PyObject *py_getcwd;
ساکن PyObject *vim_module;
ساکن PyObject *vim_special_راستہ_object;

#اگر PY_نسخہ_HEX >= 0x030700f0
ساکن PyObject *py_ڈھونڈو_spec;
#ورنہ
ساکن PyObject *py_بھرو_module;
#اگر_ختم
#اگر PY_نسخہ_HEX < 0x30c00a7
ساکن PyObject *py_ڈھونڈو_module;
#اگر_ختم

ساکن PyObject *VimError;

/*
 * obtain a lock on the Vim data structures
 */
    ساکن خالی
Python_تالا_Vim(خالی)
{
}

/*
 * release a lock on the Vim data structures
 */
    ساکن خالی
Python_Release_Vim(خالی)
{
}

/*
 * The "todecref" argument holds a pointer to PyObject * that must be
 * DECREF'ed after returned char_u * is no longer needed or NULL if all what
 * was needed to generate returned value is object.
 *
 * Use Py_XDECREF to decrement reference count.
 */
    ساکن char_u *
String_مقصد_Chars(PyObject *obj, PyObject **todecref)
{
    char_u	*str;

    اگر (Py_بائٹیں_Check(obj))
    {

	اگر (Py_بائٹیں_As_String_And_سائز(obj, (حرف **) &str, تہی) == -1
		|| str == تہی)
	    واپس تہی;

	*todecref = تہی;
    }
    ورنہ اگر (PyUnicode_Check(obj))
    {
	PyObject	*بائٹیں;

	اگر (!(بائٹیں = PyUnicode_AsEncodedString(obj, ENC_OPT,
							   ERRORS_ENCODE_ARG)))
	    واپس تہی;

	اگر (Py_بائٹیں_As_String_And_سائز(بائٹیں, (حرف **) &str, تہی) == -1
		|| str == تہی)
	{
	    Py_DECREF(بائٹیں);
	    واپس تہی;
	}

	*todecref = بائٹیں;
    }
    ورنہ
    {
#اگر PY_MAJOR_نسخہ < 3
	PyErr_FORMAT_TYPE(N_("expected str() or unicode() instance, but got %s"),
		obj);
#ورنہ
	PyErr_FORMAT_TYPE(N_("expected bytes() or str() instance, but got %s"),
		obj);
#اگر_ختم
	واپس تہی;
    }

    واپس (char_u *) str;
}

#تعریف_کرو نمبر_LONG     1
#تعریف_کرو نمبر_INT      2
#تعریف_کرو نمبر_NATURAL  4
#تعریف_کرو نمبر_UNSIGNED 8

    ساکن صحیح
نمبر_مقصد_Long(PyObject *obj, طویل *نتیجہ, صحیح flags)
{
#اگر PY_MAJOR_نسخہ < 3
    اگر (PyInt_Check(obj))
    {
	*نتیجہ = PyInt_AsLong(obj);
	اگر (PyErr_Occurred())
	    واپس -1;
    }
    ورنہ
#اگر_ختم
    اگر (PyLong_Check(obj))
    {
	*نتیجہ = PyLong_AsLong(obj);
	اگر (PyErr_Occurred())
	    واپس -1;
    }
    ورنہ اگر (Py_نمبر_Check(obj))
    {
	PyObject	*عدد;

	اگر (!(عدد = Py_نمبر_Long(obj)))
	    واپس -1;

	*نتیجہ = PyLong_AsLong(عدد);

	Py_DECREF(عدد);

	اگر (PyErr_Occurred())
	    واپس -1;
    }
    ورنہ
    {
#اگر PY_MAJOR_نسخہ < 3
	PyErr_FORMAT_TYPE(
		N_("expected int(), long() or something supporting "
		   "coercing to long(), but got %s"),
		obj);
#ورنہ
	PyErr_FORMAT_TYPE(
		N_("expected int() or something supporting coercing to int(), "
		   "but got %s"),
		obj);
#اگر_ختم
	واپس -1;
    }

    اگر (flags & نمبر_INT)
    {
	اگر (*نتیجہ > زیادہ_سے_زیادہ_عدد)
	{
	    Py_Err_سیٹ_STRING(Py_Exc_حد_سے_تجاوز_Error,
		    N_("value is too large to fit into C int type"));
	    واپس -1;
	}
	ورنہ اگر (*نتیجہ < INT_MIN)
	{
	    Py_Err_سیٹ_STRING(Py_Exc_حد_سے_تجاوز_Error,
		    N_("value is too small to fit into C int type"));
	    واپس -1;
	}
    }

    اگر (flags & نمبر_NATURAL)
    {
	اگر (*نتیجہ <= 0)
	{
	    Py_Err_سیٹ_STRING(Py_Exc_قدر_Error,
		    N_("number must be greater than zero"));
	    واپس -1;
	}
    }
    ورنہ اگر (flags & نمبر_UNSIGNED)
    {
	اگر (*نتیجہ < 0)
	{
	    Py_Err_سیٹ_STRING(Py_Exc_قدر_Error,
		    N_("number must be greater or equal to zero"));
	    واپس -1;
	}
    }

    واپس 0;
}

    ساکن صحیح
جوڑو_string(PyObject *list, حرف *s)
{
    PyObject	*string;

    اگر (!(string = PyString_FromString(s)))
	واپس -1;

    اگر (PyList_Append(list, string))
    {
	Py_DECREF(string);
	واپس -1;
    }

    Py_DECREF(string);
    واپس 0;
}

    ساکن PyObject *
ObjectDir(PyObject *self, حرف **اوصاف)
{
    Py_طریقہ_Def	*طریقہ;
    حرف	**attr;
    PyObject	*ret;

    اگر (!(ret = PyList_New(0)))
	واپس تہی;

    اگر (self)
	برائے (طریقہ = Py_TYPE_GET_TP_METHODS(self->ob_type) ; طریقہ->ml_name != تہی ; ++طریقہ)
	    اگر (جوڑو_string(ret, (حرف *)طریقہ->ml_name))
	    {
		Py_DECREF(ret);
		واپس تہی;
	    }

    برائے (attr = اوصاف ; *attr ; ++attr)
	اگر (جوڑو_string(ret, *attr))
	{
	    Py_DECREF(ret);
	    واپس تہی;
	}

#اگر PY_MAJOR_نسخہ < 3
    اگر (جوڑو_string(ret, "__members__"))
    {
	Py_DECREF(ret);
	واپس تہی;
    }
#اگر_ختم

    واپس ret;
}

// Output buffer management

// Function to write a line, points to either msg() or emsg().
قسمتعریف صحیح (*writefn)(حرف *);

DEFINE_PY_TYPE_OBJECT(OutputType);

قسمتعریف ساخت
{
    Py_Object_سر
    طویل softspace;
    طویل error;
} OutputObject;

ساکن حرف *OutputAttrs[] = {
    "softspace",
    تہی
};

    ساکن PyObject *
OutputDir(PyObject *self, PyObject *دلائل غیر_مستعمل)
{
    واپس ObjectDir(self, OutputAttrs);
}

    ساکن صحیح
OutputSetattr(PyObject *self_obj, حرف *name, PyObject *valObject)
{
    OutputObject *self = (OutputObject*)self_obj;
    اگر (valObject == تہی)
    {
	Py_Err_سیٹ_STRING(PyExc_AttributeError,
		N_("can't delete OutputObject attributes"));
	واپس -1;
    }

    اگر (strcmp(name, "softspace") == 0)
    {
	اگر (نمبر_مقصد_Long(valObject, &(self->softspace), نمبر_UNSIGNED))
	    واپس -1;
	واپس 0;
    }

    PyErr_FORMAT(PyExc_AttributeError, N_("invalid attribute: %s"), name);
    واپس -1;
}

// Buffer IO, we write one whole line at a time.
ساکن garray_T io_ga = {0, 0, 1, 80, تہی};
ساکن writefn old_fn = تہی;

    ساکن خالی
PythonIO_Flush(خالی)
{
    اگر (old_fn != تہی && io_ga.ga_len > 0)
    {
	((حرف *)io_ga.ga_ڈیٹا)[io_ga.ga_len] = NUL;
	// We don't know what emsg_severe should be here, so ... hope?
	old_fn((حرف *)io_ga.ga_ڈیٹا);
    }
    io_ga.ga_len = 0;
}

    ساکن خالی
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
    جب (n > 0 && (ptr = memchr(str, '\n', n)) != تہی
					  && (fn == (writefn)emsg || !got_int))
    {
	PyInt len = ptr - str;

	اگر (ga_grow(&io_ga, (صحیح)(len + 1)) == FAIL)
	    توڑو;

	mch_memmove(((حرف *)io_ga.ga_ڈیٹا) + io_ga.ga_len, str, (size_t)len);
	((حرف *)io_ga.ga_ڈیٹا)[io_ga.ga_len + len] = NUL;
	emsg_severe = severe;
	fn((حرف *)io_ga.ga_ڈیٹا);
	str = ptr + 1;
	n -= len + 1;
	io_ga.ga_len = 0;
    }

    // Put the remaining text into io_ga for later printing.
    اگر (n > 0 && (fn == (writefn)emsg || !got_int)
					&& ga_grow(&io_ga, (صحیح)(n + 1)) == OK)
    {
	mch_memmove(((حرف *)io_ga.ga_ڈیٹا) + io_ga.ga_len, str, (size_t)n);
	io_ga.ga_len += (صحیح)n;
    }
}

    ساکن صحیح
write_output(OutputObject *self, PyObject *string)
{
    Py_ssize_t	len = 0;
    حرف	*str = تہی;
    صحیح		error = self->error;

    اگر (!PyArg_Parse(string, "et#", ENC_OPT, &str, &len))
	واپس -1;

    Py_BEGIN_ALLOW_دھاگے
    Python_تالا_Vim();
    writer((writefn)(error ? emsg : پیغام), (char_u *)str, len, error);
    Python_Release_Vim();
    Py_انتہا_ALLOW_دھاگے
    PyMem_Free(str);

    واپس 0;
}

    ساکن PyObject *
OutputWrite(OutputObject *self, PyObject *string)
{
    اگر (write_output(self, string))
	واپس تہی;

    Py_INCREF(Py_None);
    واپس Py_None;
}

    ساکن PyObject *
OutputWritelines(OutputObject *self, PyObject *seq)
{
    PyObject	*iterator;
    PyObject	*شے;

    اگر (!(iterator = PyObject_GetIter(seq)))
	واپس تہی;

    جب ((شے = Py_Iter_اگلا(iterator)))
    {
	اگر (write_output(self, شے))
	{
	    Py_DECREF(iterator);
	    Py_DECREF(شے);
	    واپس تہی;
	}
	Py_DECREF(شے);
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    اگر (PyErr_Occurred())
	واپس تہی;

    Py_INCREF(Py_None);
    واپس Py_None;
}

    ساکن PyObject *
AlwaysNone(PyObject *self غیر_مستعمل, PyObject *دلائل غیر_مستعمل)
{
    // do nothing
    Py_INCREF(Py_None);
    واپس Py_None;
}
#تعریف_کرو ALWAYS_NONE AlwaysNone(تہی, تہی)

    ساکن PyObject *
AlwaysFalse(PyObject *self غیر_مستعمل, PyObject *دلائل غیر_مستعمل)
{
    // do nothing
    PyObject	*ret = Py_False;
    Py_INCREF(ret);
    واپس ret;
}
#تعریف_کرو ALWAYS_FALSE AlwaysFalse(تہی, تہی)

    ساکن PyObject *
AlwaysTrue(PyObject *self غیر_مستعمل, PyObject *دلائل غیر_مستعمل)
{
    // do nothing
    PyObject	*ret = Py_True;
    Py_INCREF(ret);
    واپس ret;
}
#تعریف_کرو ALWAYS_TRUE AlwaysTrue(تہی, تہی)

/***************/

ساکن ساخت Py_طریقہ_Def OutputMethods[] = {
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
    { تہی,	    تہی,				0,		تہی}
};

ساکن OutputObject Output =
{
    Py_Object_سر_INIT_TYPE(OutputType)
    0,
    0
};

ساکن OutputObject Error =
{
    Py_Object_سر_INIT_TYPE(OutputType)
    0,
    1
};

    ساکن صحیح
PythonIO_Init_io(خالی)
{
    اگر (Py_Sys_سیٹ_Object("stdout", (PyObject *)(خالی *)&Output))
	واپس -1;
    اگر (Py_Sys_سیٹ_Object("stderr", (PyObject *)(خالی *)&Error))
	واپس -1;

    اگر (PyErr_Occurred())
    {
	emsg(_(e_python_error_initialising_io_object));
	واپس -1;
    }

    واپس 0;
}

#اگر PY_نسخہ_HEX < 0x030700f0
ساکن PyObject *call_بھرو_module(حرف *name, صحیح len, PyObject *ڈھونڈو_module_نتیجہ);

قسمتعریف ساخت
{
    Py_Object_سر
    حرف	*fullname;
    PyObject	*نتیجہ;
} LoaderObject;
DEFINE_PY_TYPE_OBJECT(LoaderType);

    ساکن خالی
LoaderDestructor(PyObject *self_obj)
{
    LoaderObject *self = (LoaderObject*)self_obj;
    vim_free(self->fullname);
    Py_XDECREF(self->نتیجہ);
    DESTRUCTOR_اختتام(self);
}

    ساکن PyObject *
Loader_بھرو_Module(LoaderObject *self, PyObject *دلائل غیر_مستعمل)
{
    حرف	*fullname = self->fullname;
    PyObject	*نتیجہ = self->نتیجہ;
    PyObject	*module;

    اگر (!fullname)
    {
	module = نتیجہ ? نتیجہ : Py_None;
	Py_INCREF(module);
	واپس module;
    }

    module = call_بھرو_module(fullname, (صحیح)STRLEN(fullname), نتیجہ);

    self->fullname = تہی;
    self->نتیجہ = module;

    vim_free(fullname);
    Py_DECREF(نتیجہ);

    اگر (!module)
    {
	اگر (PyErr_Occurred())
	    واپس تہی;

	Py_INCREF(Py_None);
	واپس Py_None;
    }

    Py_INCREF(module);
    واپس module;
}

ساکن ساخت Py_طریقہ_Def LoaderMethods[] = {
    // name,	    function,				calling,	doc
    {"load_module", (PyCFunction)Loader_بھرو_Module,	METH_VARARGS,	""},
    { تہی,	    تہی,				0,		تہی}
};
#اگر_ختم

/*
 * Check to see whether a Vim error has been reported, or a keyboard
 * interrupt has been detected.
 */
    ساکن خالی
Vim_Try_آغاز(خالی)
{
    ++trylevel;
}

    ساکن صحیح
Vim_Try_انتہا(خالی)
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
	Py_Err_سیٹ_None(Py_Exc_Keyboard_مداخلت);
	واپس -1;
    }
    ورنہ اگر (پیغام_list != تہی && *پیغام_list != تہی)
    {
	صحیح	should_free;
	حرف	*پیغام;

	پیغام = get_exception_string(*پیغام_list, ET_ERROR, تہی, &should_free);

	اگر (پیغام == تہی)
	{
	    Py_Err_No_حافظہ();
	    واپس -1;
	}

	Py_Err_سیٹ_Vim(پیغام);

	free_global_msglist();

	اگر (should_free)
	    vim_free(پیغام);

	واپس -1;
    }
    ورنہ اگر (!did_throw)
	واپس (PyErr_Occurred() ? -1 : 0);
    // Python exception is preferred over Vim one; unlikely to occur though
    ورنہ اگر (PyErr_Occurred())
    {
	discard_current_exception();
	واپس -1;
    }
    // Finally transform Vim script exception to python one
    ورنہ
    {
	Py_Err_سیٹ_Vim((حرف *)current_exception->قدر);
	discard_current_exception();
	واپس -1;
    }
}

    ساکن صحیح
Vim_Check_مداخلت(خالی)
{
    اگر (got_int)
    {
	Py_Err_سیٹ_None(Py_Exc_Keyboard_مداخلت);
	واپس 1;
    }
    واپس 0;
}

// Vim module - Implementation

    ساکن PyObject *
VimCommand(PyObject *self غیر_مستعمل, PyObject *string)
{
    char_u	*cmd;
    PyObject	*ret;
    PyObject	*todecref;

    اگر (!(cmd = String_مقصد_Chars(string, &todecref)))
	واپس تہی;

    Py_BEGIN_ALLOW_دھاگے
    Python_تالا_Vim();

    Vim_Try_آغاز();
    do_cmdline_cmd(cmd);
    update_screen(UPD_VALID);

    Python_Release_Vim();
    Py_انتہا_ALLOW_دھاگے

    اگر (Vim_Try_انتہا())
	ret = تہی;
    ورنہ
	ret = Py_None;

    Py_XINCREF(ret);
    Py_XDECREF(todecref);
    واپس ret;
}

/*
 * Function to translate a typval_T into a PyObject; this will recursively
 * translate lists/dictionaries into their Python equivalents.
 *
 * The depth parameter is to avoid infinite recursion, set it to 1 when
 * you call VimToPython.
 */
    ساکن PyObject *
Vim_مقصد_Python(typval_T *our_tv, صحیح گہرائی, PyObject *lookup_dict)
{
    PyObject	*ret;
    PyObject	*newObj;
    حرف	ptrBuf[حجم(خالی *) * 2 + 3];

    // Avoid infinite recursion
    اگر (گہرائی > 100)
    {
	Py_INCREF(Py_None);
	ret = Py_None;
	واپس ret;
    }

    // Check if we run into a recursive loop.  The item must be in lookup_dict
    // then and we can use it again.
    اگر ((our_tv->v_type == VAR_LIST && our_tv->vval.v_list != تہی)
	    || (our_tv->v_type == VAR_TUPLE && our_tv->vval.v_tuple != تہی)
	    || (our_tv->v_type == VAR_DICT && our_tv->vval.v_dict != تہی))
    {
	sprintf(ptrBuf, "%p",
		our_tv->v_type == VAR_LIST ? (خالی *)our_tv->vval.v_list
		    : our_tv->v_type == VAR_TUPLE ?
			(خالی *)our_tv->vval.v_tuple
			: (خالی *)our_tv->vval.v_dict);

	اگر ((ret = Py_Dict_Get_شے_String(lookup_dict, ptrBuf)))
	{
	    Py_INCREF(ret);
	    واپس ret;
	}
    }

    اگر (our_tv->v_type == VAR_STRING)
	ret = PyString_FromString(our_tv->vval.v_string == تہی
					? "" : (حرف *)our_tv->vval.v_string);
    ورنہ اگر (our_tv->v_type == VAR_نمبر)
    {
	حرف buf[NUMBUFLEN];

	// For backwards compatibility numbers are stored as strings.
	sprintf(buf, "%lld", (طویل طویل)our_tv->vval.v_نمبر);
	ret = PyString_FromString((حرف *)buf);
    }
    ورنہ اگر (our_tv->v_type == VAR_FLOAT)
    {
	حرف buf[NUMBUFLEN];

	sprintf(buf, "%f", our_tv->vval.v_float);
	ret = PyString_FromString((حرف *)buf);
    }
    ورنہ اگر (our_tv->v_type == VAR_LIST)
    {
	list_T		*list = our_tv->vval.v_list;
	listitem_T	*curr;

	اگر (list == تہی)
	    واپس تہی;

	اگر (!(ret = PyList_New(0)))
	    واپس تہی;

	اگر (Py_Dict_سیٹ_شے_String(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    واپس تہی;
	}

	CHECK_LIST_MATERIALIZE(list);
	FOR_ALL_LIST_اشیاء(list, curr)
	{
	    اگر (!(newObj = Vim_مقصد_Python(&curr->li_tv, گہرائی + 1, lookup_dict)))
	    {
		Py_DECREF(ret);
		واپس تہی;
	    }
	    اگر (PyList_Append(ret, newObj))
	    {
		Py_DECREF(newObj);
		Py_DECREF(ret);
		واپس تہی;
	    }
	    Py_DECREF(newObj);
	}
    }
    ورنہ اگر (our_tv->v_type == VAR_TUPLE)
    {
	tuple_T	*tuple = our_tv->vval.v_tuple;
	صحیح	len;

	اگر (tuple == تہی)
	    واپس تہی;

	len = TUPLE_LEN(tuple);

	ret = PyTuple_New(len);
	اگر (ret == تہی)
	    واپس تہی;

	برائے (صحیح idx = 0; idx < len; idx++)
	{
	    typval_T	*شے_tv = TUPLE_شے(tuple, idx);

	    newObj = Vim_مقصد_Python(شے_tv, گہرائی + 1, lookup_dict);
	    اگر (!newObj)
	    {
		Py_DECREF(ret);
		واپس تہی;
	    }
	    Py_Tuple_سیٹ_شے(ret, idx, newObj);
	}

	اگر (Py_Dict_سیٹ_شے_String(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    واپس تہی;
	}
    }
    ورنہ اگر (our_tv->v_type == VAR_DICT)
    {

	hashtab_T	*ht;
	long_u		todo;
	hashitem_T	*hi;
	dictitem_T	*di;

	اگر (our_tv->vval.v_dict == تہی)
	    واپس تہی;
	ht = &our_tv->vval.v_dict->dv_hashtab;

	اگر (!(ret = PyDict_New()))
	    واپس تہی;

	اگر (Py_Dict_سیٹ_شے_String(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    واپس تہی;
	}

	todo = ht->ht_مستعمل;
	برائے (hi = ht->ht_array; todo > 0; ++hi)
	{
	    اگر (!HASHITEM_خالی_خانے(hi))
	    {
		--todo;

		di = dict_lookup(hi);
		اگر (!(newObj = Vim_مقصد_Python(&di->di_tv, گہرائی + 1, lookup_dict)))
		{
		    Py_DECREF(ret);
		    واپس تہی;
		}
		اگر (Py_Dict_سیٹ_شے_String(ret, (حرف *)hi->hi_کلید, newObj))
		{
		    Py_DECREF(ret);
		    Py_DECREF(newObj);
		    واپس تہی;
		}
	    }
	}
    }
    ورنہ اگر (our_tv->v_type == VAR_BOOL)
    {
	اگر (our_tv->vval.v_نمبر == VVAL_FALSE)
	{
	    ret = Py_False;
	    Py_INCREF(ret);
	}
	ورنہ
	{
	    ret = Py_True;
	    Py_INCREF(ret);
	}
	واپس ret;
    }
    ورنہ اگر (our_tv->v_type == VAR_SPECIAL)
    {
	Py_INCREF(Py_None);
	ret = Py_None;
	واپس ret;
    }
    ورنہ اگر (our_tv->v_type == VAR_BLOB)
	ret = Py_بائٹیں_From_String_And_سائز(
		(حرف*) our_tv->vval.v_blob->bv_ga.ga_ڈیٹا,
		(Py_ssize_t) our_tv->vval.v_blob->bv_ga.ga_len);
    ورنہ
    {
	Py_INCREF(Py_None);
	ret = Py_None;
    }

    واپس ret;
}

    ساکن PyObject *
VimEval(PyObject *self غیر_مستعمل, PyObject *دلائل)
{
    char_u	*اظہاریہ;
    typval_T	*our_tv;
    PyObject	*string;
    PyObject	*todecref;
    PyObject	*ret;
    PyObject	*lookup_dict;

    اگر (!PyArg_ParseTuple(دلائل, "O", &string))
	واپس تہی;

    اگر (!(اظہاریہ = String_مقصد_Chars(string, &todecref)))
	واپس تہی;

    Py_BEGIN_ALLOW_دھاگے
    Python_تالا_Vim();
    Vim_Try_آغاز();
    our_tv = eval_اظہاریہ(اظہاریہ, تہی);
    Python_Release_Vim();
    Py_انتہا_ALLOW_دھاگے

    Py_XDECREF(todecref);

    اگر (Vim_Try_انتہا())
	واپس تہی;

    اگر (our_tv == تہی)
    {
	Py_Err_سیٹ_VIM(N_("invalid expression"));
	واپس تہی;
    }

    // Convert the Vim type into a Python type.  Create a dictionary that's
    // used to check for recursive loops.
    اگر (!(lookup_dict = PyDict_New()))
	ret = تہی;
    ورنہ
    {
	ret = Vim_مقصد_Python(our_tv, 1, lookup_dict);
	Py_DECREF(lookup_dict);
    }


    Py_BEGIN_ALLOW_دھاگے
    Python_تالا_Vim();
    free_tv(our_tv);
    Python_Release_Vim();
    Py_انتہا_ALLOW_دھاگے

    واپس ret;
}

ساکن PyObject *Convert_مقصد_Py_Object(typval_T *);

    ساکن PyObject *
VimEvalPy(PyObject *self غیر_مستعمل, PyObject *string)
{
    typval_T	*our_tv;
    PyObject	*ret;
    char_u	*اظہاریہ;
    PyObject	*todecref;

    اگر (!(اظہاریہ = String_مقصد_Chars(string, &todecref)))
	واپس تہی;

    Py_BEGIN_ALLOW_دھاگے
    Python_تالا_Vim();
    Vim_Try_آغاز();
    our_tv = eval_اظہاریہ(اظہاریہ, تہی);
    Python_Release_Vim();
    Py_انتہا_ALLOW_دھاگے

    Py_XDECREF(todecref);

    اگر (Vim_Try_انتہا())
	واپس تہی;

    اگر (our_tv == تہی)
    {
	Py_Err_سیٹ_VIM(N_("invalid expression"));
	واپس تہی;
    }

    ret = Convert_مقصد_Py_Object(our_tv);
    Py_BEGIN_ALLOW_دھاگے
    Python_تالا_Vim();
    free_tv(our_tv);
    Python_Release_Vim();
    Py_انتہا_ALLOW_دھاگے

    واپس ret;
}

    ساکن PyObject *
VimStrwidth(PyObject *self غیر_مستعمل, PyObject *string)
{
    char_u	*str;
    PyObject	*todecref;
    صحیح		len;

    اگر (!(str = String_مقصد_Chars(string, &todecref)))
	واپس تہی;

    len = mb_string2cells(str, (صحیح)STRLEN(str));

    Py_XDECREF(todecref);

    واپس PyLong_FromLong(len);
}

    ساکن PyObject *
_VimChdir(PyObject *_chdir, PyObject *دلائل, PyObject *kwargs)
{
    PyObject	*ret;
    PyObject	*newwd;
    PyObject	*todecref;
    char_u	*new_dir;

    اگر (_chdir == تہی)
	واپس تہی;
    اگر (!(ret = PyObject_Call(_chdir, دلائل, kwargs)))
	واپس تہی;

    اگر (!(newwd = Py_Object_Call_Function_Obj_دلائل(py_getcwd, تہی)))
    {
	Py_DECREF(ret);
	واپس تہی;
    }

    اگر (!(new_dir = String_مقصد_Chars(newwd, &todecref)))
    {
	Py_DECREF(ret);
	Py_DECREF(newwd);
	واپس تہی;
    }

    Vim_Try_آغاز();

    اگر (vim_chdir(new_dir))
    {
	Py_DECREF(ret);
	Py_DECREF(newwd);
	Py_XDECREF(todecref);

	اگر (Vim_Try_انتہا())
	    واپس تہی;

	Py_Err_سیٹ_VIM(N_("failed to change directory"));
	واپس تہی;
    }

    Py_DECREF(newwd);
    Py_XDECREF(todecref);

    post_chdir(CDSCOPE_GLOBAL);

    اگر (Vim_Try_انتہا())
    {
	Py_DECREF(ret);
	واپس تہی;
    }

    واپس ret;
}

    ساکن PyObject *
VimChdir(PyObject *self غیر_مستعمل, PyObject *دلائل, PyObject *kwargs)
{
    واپس _VimChdir(py_chdir, دلائل, kwargs);
}

    ساکن PyObject *
VimFchdir(PyObject *self غیر_مستعمل, PyObject *دلائل, PyObject *kwargs)
{
    واپس _VimChdir(py_fchdir, دلائل, kwargs);
}

قسمتعریف ساخت {
    PyObject	*callable;
    PyObject	*نتیجہ;
} map_rtp_ڈیٹا;

    ساکن خالی
map_rtp_callback(char_u *راستہ, خالی *ڈیٹا_3)
{
    خالی	**ڈیٹا = (خالی **) ڈیٹا_3;
    PyObject	*راستہ_Object;
    map_rtp_ڈیٹا	*mr_ڈیٹا = *((map_rtp_ڈیٹا **) ڈیٹا);

    اگر (!(راستہ_Object = PyString_FromString((حرف *)راستہ)))
    {
	*ڈیٹا = تہی;
	واپس;
    }

    mr_ڈیٹا->نتیجہ = Py_Object_Call_Function_Obj_دلائل(mr_ڈیٹا->callable,
						   راستہ_Object, تہی);

    Py_DECREF(راستہ_Object);

    اگر (!mr_ڈیٹا->نتیجہ || mr_ڈیٹا->نتیجہ != Py_None)
	*ڈیٹا = تہی;
    ورنہ
    {
	Py_DECREF(mr_ڈیٹا->نتیجہ);
	mr_ڈیٹا->نتیجہ = تہی;
    }
}

    ساکن PyObject *
VimForeachRTP(PyObject *self غیر_مستعمل, PyObject *callable)
{
    map_rtp_ڈیٹا	ڈیٹا;

    ڈیٹا.callable = callable;
    ڈیٹا.نتیجہ = تہی;

    do_اندر_runtimepath(تہی, 0, &map_rtp_callback, &ڈیٹا);

    اگر (ڈیٹا.نتیجہ == تہی)
    {
	اگر (PyErr_Occurred())
	    واپس تہی;
	ورنہ
	{
	    Py_INCREF(Py_None);
	    واپس Py_None;
	}
    }
    واپس ڈیٹا.نتیجہ;
}

/*
 * _vim_runtimepath_ special path implementation.
 */

    ساکن خالی
map_finder_callback(char_u *راستہ, خالی *ڈیٹا_3)
{
    خالی	**ڈیٹا = (خالی **) ڈیٹا_3;
    PyObject	*list = *((PyObject **) ڈیٹا);
    PyObject	*راستہ_Object1, *راستہ_Object2;
    حرف	*pathbuf;
    size_t	pathlen;

    pathlen = STRLEN(راستہ);

#اگر PY_MAJOR_نسخہ < 3
# define PY_MAIN_DIR_STRING "python2"
#ورنہ
# define PY_MAIN_DIR_STRING "python3"
#اگر_ختم
#تعریف_کرو PY_ALTERNATE_DIR_STRING "pythonx"

#تعریف_کرو PYTHONX_STRING_لمبائی 7 // STRLEN("pythonx")
    اگر (!(pathbuf = PyMem_New(حرف,
		    pathlen + STRLEN(PATHSEPSTR) + PYTHONX_STRING_لمبائی + 1)))
    {
	Py_Err_No_حافظہ();
	*ڈیٹا = تہی;
	واپس;
    }

    mch_memmove(pathbuf, راستہ, pathlen + 1);
    جوڑو_pathsep((char_u *) pathbuf);

    pathlen = STRLEN(pathbuf);
    mch_memmove(pathbuf + pathlen, PY_MAIN_DIR_STRING,
	    PYTHONX_STRING_لمبائی + 1);

    اگر (!(راستہ_Object1 = PyString_FromString(pathbuf)))
    {
	*ڈیٹا = تہی;
	PyMem_Free(pathbuf);
	واپس;
    }

    mch_memmove(pathbuf + pathlen, PY_ALTERNATE_DIR_STRING,
	    PYTHONX_STRING_لمبائی + 1);

    اگر (!(راستہ_Object2 = PyString_FromString(pathbuf)))
    {
	Py_DECREF(راستہ_Object1);
	PyMem_Free(pathbuf);
	*ڈیٹا = تہی;
	واپس;
    }

    PyMem_Free(pathbuf);

    اگر (PyList_Append(list, راستہ_Object1)
	    || PyList_Append(list, راستہ_Object2))
	*ڈیٹا = تہی;

    Py_DECREF(راستہ_Object1);
    Py_DECREF(راستہ_Object2);
}

    ساکن PyObject *
Vim_GetPaths(PyObject *self غیر_مستعمل, PyObject *دلائل غیر_مستعمل)
{
    PyObject	*ret;

    اگر (!(ret = PyList_New(0)))
	واپس تہی;

    do_اندر_runtimepath(تہی, 0, &map_finder_callback, ret);

    اگر (PyErr_Occurred())
    {
	Py_DECREF(ret);
	واپس تہی;
    }

    واپس ret;
}

#اگر PY_نسخہ_HEX >= 0x030700f0
    ساکن PyObject *
Finder_ڈھونڈو_Spec(PyObject *self, PyObject *دلائل)
{
    حرف	*fullname;
    PyObject	*paths;
    PyObject	*ہدف = Py_None;
    PyObject	*spec;

    اگر (!PyArg_ParseTuple(دلائل, "s|O", &fullname, &ہدف))
	واپس تہی;

    اگر (!(paths = Vim_GetPaths(self, تہی)))
	واپس تہی;

    spec = PyObject_CallFunction(py_ڈھونڈو_spec, "sOO", fullname, paths, ہدف);

    Py_DECREF(paths);

    اگر (!spec)
    {
	اگر (PyErr_Occurred())
	    واپس تہی;

	Py_INCREF(Py_None);
	واپس Py_None;
    }

    واپس spec;
}

    ساکن PyObject *
Finder_ڈھونڈو_Module(PyObject* self غیر_مستعمل, PyObject* دلائل غیر_مستعمل)
{
    // Apparently returning None works.
    Py_INCREF(Py_None);
    واپس Py_None;
}
#ورنہ
    ساکن PyObject *
call_بھرو_module(حرف *name, صحیح len, PyObject *ڈھونڈو_module_نتیجہ)
{
    PyObject	*fd, *pathname, *description;

    اگر (!PyTuple_Check(ڈھونڈو_module_نتیجہ))
    {
	PyErr_FORMAT_TYPE(
		N_("expected 3-tuple as imp.find_module() result, but got %s"),
		ڈھونڈو_module_نتیجہ);
	واپس تہی;
    }
    اگر (Py_Tuple_GET_سائز(ڈھونڈو_module_نتیجہ) != 3)
    {
	PyErr_FORMAT(PyExc_TypeError,
		N_("expected 3-tuple as imp.find_module() result, but got "
		   "tuple of size %d"),
		(صحیح) Py_Tuple_GET_سائز(ڈھونڈو_module_نتیجہ));
	واپس تہی;
    }

    اگر (!(fd = Py_Tuple_GET_شے(ڈھونڈو_module_نتیجہ, 0))
	    || !(pathname = Py_Tuple_GET_شے(ڈھونڈو_module_نتیجہ, 1))
	    || !(description = Py_Tuple_GET_شے(ڈھونڈو_module_نتیجہ, 2)))
    {
	Py_Err_سیٹ_STRING(PyExc_RuntimeError,
		N_("internal error: imp.find_module returned tuple with NULL"));
	واپس تہی;
    }

    واپس PyObject_CallFunction(py_بھرو_module,
	    "s#OOO", name, len, fd, pathname, description);
}

    ساکن PyObject *
ڈھونڈو_module(حرف *fullname, حرف *دم, PyObject *new_راستہ)
{
    PyObject	*ڈھونڈو_module_نتیجہ;
    PyObject	*module;
    حرف	*dot;

    اگر ((dot = (حرف *)vim_strchr((char_u *) دم, '.')))
    {
	/*
	 * There is a dot in the name: call find_module recursively without the
	 * first component
	 */
	PyObject	*newest_راستہ;
	صحیح		partlen = (صحیح) (dot - 1 - دم);

	اگر (!(ڈھونڈو_module_نتیجہ = PyObject_CallFunction(py_ڈھونڈو_module,
			"s#O", دم, partlen, new_راستہ)))
	{
	    اگر (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_ImportError))
		PyErr_Clear();
	    واپس تہی;
	}

	اگر (!(module = call_بھرو_module(
			fullname,
			((صحیح) (دم - fullname)) + partlen,
			ڈھونڈو_module_نتیجہ)))
	{
	    Py_DECREF(ڈھونڈو_module_نتیجہ);
	    واپس تہی;
	}

	Py_DECREF(ڈھونڈو_module_نتیجہ);

	اگر (!(newest_راستہ = PyObject_GetAttrString(module, "__path__")))
	{
	    Py_DECREF(module);
	    واپس تہی;
	}

	Py_DECREF(module);

	ڈھونڈو_module_نتیجہ = ڈھونڈو_module(fullname, dot + 1, newest_راستہ);

	Py_DECREF(newest_راستہ);

	واپس ڈھونڈو_module_نتیجہ;
    }
    ورنہ
    {
	اگر (!(ڈھونڈو_module_نتیجہ = PyObject_CallFunction(py_ڈھونڈو_module,
			"sO", دم, new_راستہ)))
	{
	    اگر (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_ImportError))
		PyErr_Clear();
	    واپس تہی;
	}

	واپس ڈھونڈو_module_نتیجہ;
    }
}

    ساکن PyObject *
Finder_ڈھونڈو_Module(PyObject *self, PyObject *دلائل)
{
    حرف	*fullname;
    PyObject	*نتیجہ;
    PyObject	*new_راستہ;
    LoaderObject	*loader;

    اگر (!PyArg_ParseTuple(دلائل, "s", &fullname))
	واپس تہی;

    اگر (!(new_راستہ = Vim_GetPaths(self, تہی)))
	واپس تہی;

    نتیجہ = ڈھونڈو_module(fullname, fullname, new_راستہ);

    Py_DECREF(new_راستہ);

    اگر (!نتیجہ)
    {
	اگر (PyErr_Occurred())
	    واپس تہی;

	Py_INCREF(Py_None);
	واپس Py_None;
    }

    اگر (!(fullname = (حرف *)vim_strsave((char_u *)fullname)))
    {
	Py_DECREF(نتیجہ);
	Py_Err_No_حافظہ();
	واپس تہی;
    }

    اگر (!(loader = PyObject_NEW(LoaderObject, LoaderTypePtr)))
    {
	vim_free(fullname);
	Py_DECREF(نتیجہ);
	واپس تہی;
    }

    loader->fullname = fullname;
    loader->نتیجہ = نتیجہ;

    واپس (PyObject *) loader;
}
#اگر_ختم

    ساکن PyObject *
Vim_راستہ_Hook(PyObject *self غیر_مستعمل, PyObject *دلائل)
{
    حرف	*راستہ;

    اگر (PyArg_ParseTuple(دلائل, "s", &راستہ)
	    && STRCMP(راستہ, vim_special_راستہ) == 0)
    {
	Py_INCREF(vim_module);
	واپس vim_module;
    }

    PyErr_Clear();
    Py_Err_سیٹ_None(PyExc_ImportError);
    واپس تہی;
}

/*
 * Vim module - Definitions
 */

ساکن ساخت Py_طریقہ_Def VimMethods[] = {
    // name,	    function,			calling,			documentation
    {"command",	    VimCommand,			METH_O,				"Execute a Vim ex-mode command" },
    {"eval",	    VimEval,			METH_VARARGS,			"Evaluate an expression using Vim evaluator" },
    {"bindeval",    VimEvalPy,			METH_O,				"Like eval(), but returns objects attached to Vim ones"},
    {"strwidth",    VimStrwidth,		METH_O,				"Screen string width, counts <Tab> as having width 1"},
    {"chdir",	    (PyCFunction)(خالی *)VimChdir,	METH_VARARGS|METH_KEYWORDS,	"Change directory"},
    {"fchdir",	    (PyCFunction)(خالی *)VimFchdir,	METH_VARARGS|METH_KEYWORDS,	"Change directory"},
    {"foreach_rtp", VimForeachRTP,		METH_O,				"Call given callable for each path in &rtp"},
#اگر PY_نسخہ_HEX >= 0x030700f0
    {"find_spec",   Finder_ڈھونڈو_Spec,		METH_VARARGS,			"Internal use only, returns spec object for any input it receives"},
#اگر_ختم
    {"find_module", Finder_ڈھونڈو_Module,		METH_VARARGS,			"Internal use only, returns loader object for any input it receives"},
    {"path_hook",   Vim_راستہ_Hook,		METH_VARARGS,			"Hook function to install in sys.path_hooks"},
    {"_get_paths",  (PyCFunction)Vim_GetPaths,	METH_NOARGS,			"Get &rtp-based additions to sys.path"},
    { تہی,	    تہی,			0,				تہی}
};

/*
 * Generic iterator object
 */

DEFINE_PY_TYPE_OBJECT(IterType);

قسمتعریف PyObject *(*nextfun)(خالی **);
قسمتعریف خالی (*destructorfun)(خالی *);
قسمتعریف صحیح (*traversefun)(خالی *, visitproc, خالی *);
قسمتعریف صحیح (*clearfun)(خالی **);

// Main purpose of this object is removing the need for do python
// initialization (i.e. PyType_Ready and setting type attributes) for a big
// bunch of objects.
قسمتعریف ساخت
{
    Py_Object_سر
    خالی *cur;
    nextfun اگلا;
    destructorfun destruct;
    traversefun traverse;
    clearfun clear;
    PyObject *iter_object;
} IterObject;

    ساکن PyObject *
IterNew(خالی *آغاز, destructorfun destruct, nextfun اگلا, traversefun traverse,
	clearfun clear, PyObject *iter_object)
{
    IterObject *self;

    self = PyObject_GC_New(IterObject, IterTypePtr);
    self->cur = آغاز;
    self->اگلا = اگلا;
    self->destruct = destruct;
    self->traverse = traverse;
    self->clear = clear;
    self->iter_object = iter_object;

    اگر (iter_object)
	Py_INCREF(iter_object);

    واپس (PyObject *)(self);
}

    ساکن خالی
IterDestructor(PyObject *self_obj)
{
    IterObject *self = (IterObject*)self_obj;
    اگر (self->iter_object)
	Py_DECREF(self->iter_object);
    PyObject_GC_UnTrack((خالی *)(self));
    self->destruct(self->cur);
    PyObject_GC_Del((خالی *)(self));
}

    ساکن صحیح
IterTraverse(PyObject *self_obj, visitproc دورہ, خالی *arg)
{
    IterObject *self = (IterObject*)self_obj;
    اگر (self->traverse != تہی)
	واپس self->traverse(self->cur, دورہ, arg);
    ورنہ
	واپس 0;
}

// Mac OSX defines clear() somewhere.
#اگر_تعریف_شدہ clear
# undef clear
#اگر_ختم

    ساکن صحیح
IterClear(PyObject *self_obj)
{
    IterObject *self = (IterObject*)self_obj;
    اگر (self->clear != تہی)
	واپس self->clear(&self->cur);
    ورنہ
	واپس 0;
}

    ساکن PyObject *
Iter_اگلا(PyObject *self_obj)
{
    IterObject *self = (IterObject*)self_obj;
    واپس self->اگلا(&self->cur);
}

    ساکن PyObject *
IterIter(PyObject *self)
{
    Py_INCREF(self);
    واپس self;
}

قسمتعریف ساخت pylinkedlist_S {
    ساخت pylinkedlist_S	*pll_اگلا;
    ساخت pylinkedlist_S	*pll_پچھلا;
    PyObject			*pll_obj;
} pylinkedlist_T;

ساکن pylinkedlist_T *lastdict = تہی;
ساکن pylinkedlist_T *lastlist = تہی;
ساکن pylinkedlist_T *lasttuple = تہی;
ساکن pylinkedlist_T *lastfunc = تہی;

    ساکن خالی
pyll_remove(pylinkedlist_T *ref, pylinkedlist_T **آخری)
{
    اگر (ref->pll_پچھلا == تہی)
    {
	اگر (ref->pll_اگلا == تہی)
	{
	    *آخری = تہی;
	    واپس;
	}
    }
    ورنہ
	ref->pll_پچھلا->pll_اگلا = ref->pll_اگلا;

    اگر (ref->pll_اگلا == تہی)
	*آخری = ref->pll_پچھلا;
    ورنہ
	ref->pll_اگلا->pll_پچھلا = ref->pll_پچھلا;
}

    ساکن خالی
pyll_جوڑو(PyObject *self, pylinkedlist_T *ref, pylinkedlist_T **آخری)
{
    اگر (*آخری == تہی)
	ref->pll_پچھلا = تہی;
    ورنہ
    {
	(*آخری)->pll_اگلا = ref;
	ref->pll_پچھلا = *آخری;
    }
    ref->pll_اگلا = تہی;
    ref->pll_obj = self;
    *آخری = ref;
}

DEFINE_PY_TYPE_OBJECT(DictionaryType);

قسمتعریف ساخت
{
    Py_Object_سر
    dict_T	*dict;
    pylinkedlist_T	ref;
} DictionaryObject;

ساکن PyObject *DictionaryUpdate(DictionaryObject *, PyObject *, PyObject *);

#تعریف_کرو NEW_DICTIONARY(dict) DictionaryNew(DictionaryTypePtr, dict)

    ساکن PyObject *
DictionaryNew(PyTypeObject *subtype, dict_T *dict)
{
    DictionaryObject	*self;

    self = (DictionaryObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    اگر (self == تہی)
	واپس تہی;
    self->dict = dict;
    ++dict->dv_refcount;

    pyll_جوڑو((PyObject *)(self), &self->ref, &lastdict);

    واپس (PyObject *)(self);
}

    ساکن dict_T *
py_dict_alloc(خالی)
{
    dict_T	*ret;

    اگر (!(ret = dict_alloc()))
    {
	Py_Err_No_حافظہ();
	واپس تہی;
    }
    ++ret->dv_refcount;

    واپس ret;
}

    ساکن PyObject *
DictionaryConstructor(PyTypeObject *subtype, PyObject *دلائل, PyObject *kwargs)
{
    DictionaryObject	*self;
    dict_T	*dict;

    اگر (!(dict = py_dict_alloc()))
	واپس تہی;

    self = (DictionaryObject *) DictionaryNew(subtype, dict);

    --dict->dv_refcount;

    اگر (kwargs || Py_Tuple_سائز(دلائل))
    {
	PyObject	*tmp;
	اگر (!(tmp = DictionaryUpdate(self, دلائل, kwargs)))
	{
	    Py_DECREF(self);
	    واپس تہی;
	}

	Py_DECREF(tmp);
    }

    واپس (PyObject *)(self);
}

    ساکن خالی
DictionaryDestructor(PyObject *self_obj)
{
    DictionaryObject *self = (DictionaryObject*)self_obj;
    pyll_remove(&self->ref, &lastdict);
    dict_unref(self->dict);

    DESTRUCTOR_اختتام(self);
}

ساکن حرف *DictionaryAttrs[] = {
    "locked", "scope",
    تہی
};

    ساکن PyObject *
DictionaryDir(PyObject *self, PyObject *دلائل غیر_مستعمل)
{
    واپس ObjectDir(self, DictionaryAttrs);
}

    ساکن صحیح
DictionarySetattr(PyObject *self_obj, حرف *name, PyObject *valObject)
{
    DictionaryObject *self = (DictionaryObject*)self_obj;
    اگر (valObject == تہی)
    {
	Py_Err_سیٹ_STRING(PyExc_AttributeError,
		N_("cannot delete vim.Dictionary attributes"));
	واپس -1;
    }

    اگر (strcmp(name, "locked") == 0)
    {
	اگر (self->dict->dv_تالا == VAR_FIXED)
	{
	    Py_Err_سیٹ_STRING(PyExc_TypeError,
		    N_("cannot modify fixed dictionary"));
	    واپس -1;
	}
	ورنہ
	{
	    صحیح		istrue = PyObject_IsTrue(valObject);
	    اگر (istrue == -1)
		واپس -1;
	    ورنہ اگر (istrue)
		self->dict->dv_تالا = VAR_LOCKED;
	    ورنہ
		self->dict->dv_تالا = 0;
	}
	واپس 0;
    }
    ورنہ
    {
	PyErr_FORMAT(PyExc_AttributeError, N_("cannot set attribute %s"), name);
	واپس -1;
    }
}

    ساکن PyInt
Dictionary_لمبائی(DictionaryObject *self)
{
    واپس ((PyInt) (self->dict->dv_hashtab.ht_مستعمل));
}

#تعریف_کرو DICT_FLAG_HAS_DEFAULT	0x01
#تعریف_کرو DICT_FLAG_نکالو		0x02
#تعریف_کرو DICT_FLAG_NONE_DEFAULT	0x04
#تعریف_کرو DICT_FLAG_RETURN_BOOL	0x08 // Incompatible with DICT_FLAG_POP
#تعریف_کرو DICT_FLAG_RETURN_PAIR	0x10

    ساکن PyObject *
Dictionary_شے(DictionaryObject *self, PyObject *دلائل, صحیح flags)
{
    PyObject	*کلید_Object;
    PyObject	*defObject = ((flags & DICT_FLAG_NONE_DEFAULT)? Py_None : تہی);
    PyObject	*ret;
    char_u	*کلید;
    dictitem_T	*di;
    dict_T	*dict = self->dict;
    hashitem_T	*hi;
    PyObject	*todecref;

    اگر (flags & DICT_FLAG_HAS_DEFAULT)
    {
	اگر (!PyArg_ParseTuple(دلائل, "O|O", &کلید_Object, &defObject))
	    واپس تہی;
    }
    ورنہ
	کلید_Object = دلائل;

    اگر (flags & DICT_FLAG_RETURN_BOOL)
	defObject = Py_False;

    اگر (!(کلید = String_مقصد_Chars(کلید_Object, &todecref)))
	واپس تہی;

    اگر (*کلید == NUL)
    {
	RAISE_NO_خالی_خانے_کلیدیں;
	Py_XDECREF(todecref);
	واپس تہی;
    }

    hi = hash_ڈھونڈو(&dict->dv_hashtab, کلید);

    Py_XDECREF(todecref);

    اگر (HASHITEM_خالی_خانے(hi))
    {
	اگر (defObject)
	{
	    Py_INCREF(defObject);
	    واپس defObject;
	}
	ورنہ
	{
	    Py_Err_سیٹ_Object(Py_Exc_کلید_Error, کلید_Object);
	    واپس تہی;
	}
    }
    ورنہ اگر (flags & DICT_FLAG_RETURN_BOOL)
    {
	ret = Py_True;
	Py_INCREF(ret);
	واپس ret;
    }

    di = dict_lookup(hi);

    اگر (!(ret = Convert_مقصد_Py_Object(&di->di_tv)))
	واپس تہی;

    اگر (flags & DICT_FLAG_نکالو)
    {
	اگر (dict->dv_تالا)
	{
	    RAISE_LOCKED_DICTIONARY;
	    Py_DECREF(ret);
	    واپس تہی;
	}

	hash_remove(&dict->dv_hashtab, hi, "Python remove variable");
	dictitem_free(di);
    }

    واپس ret;
}

    ساکن PyObject *
Dictionary_شے_2(DictionaryObject *self, PyObject *کلید_Object)
{
    واپس Dictionary_شے(self, کلید_Object, 0);
}

    ساکن صحیح
Dictionary_شامل_ہے(DictionaryObject *self, PyObject *کلید_Object)
{
    PyObject	*rObj = Dictionary_شے(self, کلید_Object, DICT_FLAG_RETURN_BOOL);
    صحیح		ret;

    اگر (rObj == تہی)
	واپس -1;

    ret = (rObj == Py_True);

    Py_DECREF(rObj);

    واپس ret;
}

قسمتعریف ساخت
{
    صحیح		dii_بدلا;
    hashtab_T	*dii_ht;
    hashitem_T	*dii_hi;
    long_u	dii_todo;
} dictiterinfo_T;

    ساکن PyObject *
Dictionary_Iter_اگلا(خالی **arg)
{
    PyObject	*ret;
    dictiterinfo_T **dii = (dictiterinfo_T**)arg;

    اگر (!(*dii)->dii_todo)
	واپس تہی;

    اگر ((*dii)->dii_ht->ht_بدلا != (*dii)->dii_بدلا)
    {
	Py_Err_سیٹ_STRING(PyExc_RuntimeError,
		N_("hashtab changed during iteration"));
	واپس تہی;
    }

    جب (((*dii)->dii_todo) && HASHITEM_خالی_خانے((*dii)->dii_hi))
	++((*dii)->dii_hi);

    --((*dii)->dii_todo);

    اگر (!(ret = Py_بائٹیں_From_String((حرف *)(*dii)->dii_hi->hi_کلید)))
	واپس تہی;

    واپس ret;
}

    ساکن PyObject *
DictionaryIter(PyObject *self_obj)
{
    DictionaryObject	*self = (DictionaryObject*)self_obj;
    dictiterinfo_T	*dii;
    hashtab_T		*ht;

    اگر (!(dii = PyMem_New(dictiterinfo_T, 1)))
    {
	Py_Err_No_حافظہ();
	واپس تہی;
    }

    ht = &self->dict->dv_hashtab;
    dii->dii_بدلا = ht->ht_بدلا;
    dii->dii_ht = ht;
    dii->dii_hi = ht->ht_array;
    dii->dii_todo = ht->ht_مستعمل;

    واپس IterNew(dii,
	    PyMem_Free, Dictionary_Iter_اگلا,
	    تہی, تہی, (PyObject *)self);
}

    ساکن صحیح
Dictionary_Ass_شے(
	DictionaryObject *self, PyObject *کلید_Object, PyObject *valObject)
{
    char_u	*کلید;
    typval_T	tv;
    dict_T	*dict = self->dict;
    dictitem_T	*di;
    PyObject	*todecref;

    اگر (dict->dv_تالا)
    {
	RAISE_LOCKED_DICTIONARY;
	واپس -1;
    }

    اگر (!(کلید = String_مقصد_Chars(کلید_Object, &todecref)))
	واپس -1;

    اگر (*کلید == NUL)
    {
	RAISE_NO_خالی_خانے_کلیدیں;
	Py_XDECREF(todecref);
	واپس -1;
    }

    di = dict_ڈھونڈو(dict, کلید, -1);

    اگر (valObject == تہی)
    {
	hashitem_T	*hi;

	اگر (di == تہی)
	{
	    Py_XDECREF(todecref);
	    Py_Err_سیٹ_Object(Py_Exc_کلید_Error, کلید_Object);
	    واپس -1;
	}
	hi = hash_ڈھونڈو(&dict->dv_hashtab, di->di_کلید);
	hash_remove(&dict->dv_hashtab, hi, "Python remove item");
	dictitem_free(di);
	Py_XDECREF(todecref);
	واپس 0;
    }

    اگر (ConvertFromPyObject(valObject, &tv) == -1)
    {
	Py_XDECREF(todecref);
	واپس -1;
    }

    اگر (di == تہی)
    {
	اگر (!(di = dictitem_alloc(کلید)))
	{
	    Py_XDECREF(todecref);
	    Py_Err_No_حافظہ();
	    واپس -1;
	}
	di->di_tv.v_type = VAR_UNKNOWN;

	اگر (dict_جوڑو(dict, di) == FAIL)
	{
	    dictitem_free(di);
	    RAISE_کلید_جوڑو_FAIL(کلید);
	    Py_XDECREF(todecref);
	    واپس -1;
	}
    }
    ورنہ
	clear_tv(&di->di_tv);

    Py_XDECREF(todecref);

    copy_tv(&tv, &di->di_tv);
    clear_tv(&tv);
    واپس 0;
}

قسمتعریف PyObject *(*hi_مقصد_py)(hashitem_T *);

    ساکن PyObject *
DictionaryListObjects(DictionaryObject *self, hi_مقصد_py hiconvert)
{
    dict_T	*dict = self->dict;
    long_u	todo = dict->dv_hashtab.ht_مستعمل;
    Py_ssize_t	i = 0;
    PyObject	*ret;
    hashitem_T	*hi;
    PyObject	*newObj;

    ret = PyList_New(todo);
    برائے (hi = dict->dv_hashtab.ht_array; todo > 0; ++hi)
    {
	اگر (!HASHITEM_خالی_خانے(hi))
	{
	    اگر (!(newObj = hiconvert(hi)))
	    {
		Py_DECREF(ret);
		واپس تہی;
	    }
	    Py_List_سیٹ_شے(ret, i, newObj);
	    --todo;
	    ++i;
	}
    }
    واپس ret;
}

    ساکن PyObject *
dict_کلید(hashitem_T *hi)
{
    واپس Py_بائٹیں_From_String((حرف *)(hi->hi_کلید));
}

    ساکن PyObject *
Dictionary_List_کلیدیں(DictionaryObject *self, PyObject *دلائل غیر_مستعمل)
{
    واپس DictionaryListObjects(self, dict_کلید);
}

    ساکن PyObject *
dict_val(hashitem_T *hi)
{
    dictitem_T	*di;

    di = dict_lookup(hi);
    واپس Convert_مقصد_Py_Object(&di->di_tv);
}

    ساکن PyObject *
Dictionary_List_اقدار(DictionaryObject *self, PyObject *دلائل غیر_مستعمل)
{
    واپس DictionaryListObjects(self, dict_val);
}

    ساکن PyObject *
dict_شے(hashitem_T *hi)
{
    PyObject	*کلید_Object;
    PyObject	*valObject;
    PyObject	*ret;

    اگر (!(کلید_Object = dict_کلید(hi)))
	واپس تہی;

    اگر (!(valObject = dict_val(hi)))
    {
	Py_DECREF(کلید_Object);
	واپس تہی;
    }

    ret = Py_Build_قدر("(OO)", کلید_Object, valObject);

    Py_DECREF(کلید_Object);
    Py_DECREF(valObject);

    واپس ret;
}

    ساکن PyObject *
Dictionary_List_اشیاء(DictionaryObject *self, PyObject *دلائل غیر_مستعمل)
{
    واپس DictionaryListObjects(self, dict_شے);
}

    ساکن PyObject *
DictionaryUpdate(DictionaryObject *self, PyObject *دلائل, PyObject *kwargs)
{
    dict_T	*dict = self->dict;

    اگر (dict->dv_تالا)
    {
	RAISE_LOCKED_DICTIONARY;
	واپس تہی;
    }

    اگر (kwargs)
    {
	typval_T	tv;

	اگر (ConvertFromPyMapping(kwargs, &tv) == -1)
	    واپس تہی;

	Vim_Try_آغاز();
	dict_extend(self->dict, tv.vval.v_dict, (char_u *) "force", تہی);
	clear_tv(&tv);
	اگر (Vim_Try_انتہا())
	    واپس تہی;
    }
    ورنہ
    {
	PyObject	*obj = تہی;

	اگر (!PyArg_ParseTuple(دلائل, "|O", &obj))
	    واپس تہی;

	اگر (obj == تہی)
	{
	    Py_INCREF(Py_None);
	    واپس Py_None;
	}

	اگر (PyObject_HasAttrString(obj, "keys"))
	    واپس DictionaryUpdate(self, تہی, obj);
	ورنہ
	{
	    PyObject	*iterator;
	    PyObject	*شے;

	    اگر (!(iterator = PyObject_GetIter(obj)))
		واپس تہی;

	    جب ((شے = Py_Iter_اگلا(iterator)))
	    {
		PyObject	*fast;
		PyObject	*کلید_Object;
		PyObject	*valObject;
		PyObject	*todecref;
		char_u		*کلید;
		dictitem_T	*di;
		hashitem_T	*hi;

		اگر (!(fast = PySequence_Fast(شے, "")))
		{
		    Py_DECREF(iterator);
		    Py_DECREF(شے);
		    واپس تہی;
		}

		Py_DECREF(شے);

		اگر (Py_Sequence_سائز(fast) != 2)
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    PyErr_FORMAT(Py_Exc_قدر_Error,
			    N_("expected sequence element of size 2, "
			    "but got sequence of size %d"),
			    (صحیح) Py_Sequence_سائز(fast));
		    واپس تہی;
		}

		کلید_Object = Py_Sequence_Get_شے(fast, 0);

		اگر (!(کلید = String_مقصد_Chars(کلید_Object, &todecref)))
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    واپس تہی;
		}

		di = dictitem_alloc(کلید);

		Py_XDECREF(todecref);

		اگر (di == تہی)
		{
		    Py_DECREF(fast);
		    Py_DECREF(iterator);
		    Py_Err_No_حافظہ();
		    واپس تہی;
		}
		di->di_tv.v_type = VAR_UNKNOWN;

		valObject = Py_Sequence_Get_شے(fast, 1);

		اگر (ConvertFromPyObject(valObject, &di->di_tv) == -1)
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    dictitem_free(di);
		    واپس تہی;
		}

		Py_DECREF(fast);

		hi = hash_ڈھونڈو(&dict->dv_hashtab, di->di_کلید);
		اگر (!HASHITEM_خالی_خانے(hi) || dict_جوڑو(dict, di) == FAIL)
		{
		    RAISE_کلید_جوڑو_FAIL(di->di_کلید);
		    Py_DECREF(iterator);
		    dictitem_free(di);
		    واپس تہی;
		}
	    }

	    Py_DECREF(iterator);

	    // Iterator may have finished due to an exception
	    اگر (PyErr_Occurred())
		واپس تہی;
	}
    }
    Py_INCREF(Py_None);
    واپس Py_None;
}

    ساکن PyObject *
DictionaryGet(DictionaryObject *self, PyObject *دلائل)
{
    واپس Dictionary_شے(self, دلائل,
			    DICT_FLAG_HAS_DEFAULT|DICT_FLAG_NONE_DEFAULT);
}

    ساکن PyObject *
Dictionary_نکالو(DictionaryObject *self, PyObject *دلائل)
{
    واپس Dictionary_شے(self, دلائل, DICT_FLAG_HAS_DEFAULT|DICT_FLAG_نکالو);
}

    ساکن PyObject *
Dictionary_نکالو_شے(DictionaryObject *self, PyObject *دلائل غیر_مستعمل)
{
    hashitem_T	*hi;
    PyObject	*ret;
    PyObject	*valObject;
    dictitem_T	*di;

    اگر (self->dict->dv_hashtab.ht_مستعمل == 0)
    {
	Py_Err_سیٹ_None(Py_Exc_کلید_Error);
	واپس تہی;
    }

    hi = self->dict->dv_hashtab.ht_array;
    جب (HASHITEM_خالی_خانے(hi))
	++hi;

    di = dict_lookup(hi);

    اگر (!(valObject = Convert_مقصد_Py_Object(&di->di_tv)))
	واپس تہی;

    اگر (!(ret = Py_Build_قدر("(" Py_بائٹیں_fmt "O)", hi->hi_کلید, valObject)))
    {
	Py_DECREF(valObject);
	واپس تہی;
    }

    hash_remove(&self->dict->dv_hashtab, hi, "Python pop item");
    dictitem_free(di);

    واپس ret;
}

    ساکن PyObject *
Dictionary_Has_کلید(DictionaryObject *self, PyObject *کلید_Object)
{
    واپس Dictionary_شے(self, کلید_Object, DICT_FLAG_RETURN_BOOL);
}

ساکن PySequenceMethods DictionaryAsSeq = {
    0,					// sq_length
    0,					// sq_concat
    0,					// sq_repeat
    0,					// sq_item
    0,					// sq_slice
    0,					// sq_ass_item
    0,					// sq_ass_slice
    (objobjproc) Dictionary_شامل_ہے,	// sq_contains
    0,					// sq_inplace_concat
    0,					// sq_inplace_repeat
};

ساکن PyMappingMethods DictionaryAsMapping = {
    (lenfunc)       Dictionary_لمبائی,
    (binaryfunc)    Dictionary_شے_2,
    (objobjargproc) Dictionary_Ass_شے,
};

ساکن ساخت Py_طریقہ_Def DictionaryMethods[] = {
    {"keys",	(PyCFunction)Dictionary_List_کلیدیں,	METH_NOARGS,	""},
    {"values",	(PyCFunction)Dictionary_List_اقدار,	METH_NOARGS,	""},
    {"items",	(PyCFunction)Dictionary_List_اشیاء,	METH_NOARGS,	""},
    {"update",	(PyCFunction)(خالی *)DictionaryUpdate,		METH_VARARGS|METH_KEYWORDS, ""},
    {"get",	(PyCFunction)DictionaryGet,		METH_VARARGS,	""},
    {"pop",	(PyCFunction)Dictionary_نکالو,		METH_VARARGS,	""},
    {"popitem",	(PyCFunction)Dictionary_نکالو_شے,		METH_NOARGS,	""},
    {"has_key",	(PyCFunction)Dictionary_Has_کلید,		METH_O,		""},
    {"__dir__",	(PyCFunction)DictionaryDir,		METH_NOARGS,	""},
    { تہی,	تہی,					0,		تہی}
};

DEFINE_PY_TYPE_OBJECT(ListType);

قسمتعریف ساخت
{
    Py_Object_سر
    list_T	*list;
    pylinkedlist_T	ref;
} ListObject;

#تعریف_کرو NEW_LIST(list) ListNew(ListTypePtr, list)

    ساکن PyObject *
ListNew(PyTypeObject *subtype, list_T *list)
{
    ListObject	*self;

    اگر (list == تہی)
	واپس تہی;

    self = (ListObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    اگر (self == تہی)
	واپس تہی;
    self->list = list;
    ++list->lv_refcount;
    CHECK_LIST_MATERIALIZE(list);

    pyll_جوڑو((PyObject *)(self), &self->ref, &lastlist);

    واپس (PyObject *)(self);
}

    ساکن list_T *
py_list_alloc(خالی)
{
    list_T	*ret;

    اگر (!(ret = list_alloc()))
    {
	Py_Err_No_حافظہ();
	واپس تہی;
    }
    ++ret->lv_refcount;

    واپس ret;
}

    ساکن صحیح
list_py_concat(list_T *l, PyObject *obj, PyObject *lookup_dict)
{
    PyObject	*iterator;
    PyObject	*شے;
    listitem_T	*li;

    اگر (!(iterator = PyObject_GetIter(obj)))
	واپس -1;

    جب ((شے = Py_Iter_اگلا(iterator)))
    {
	اگر (!(li = listitem_alloc()))
	{
	    Py_Err_No_حافظہ();
	    Py_DECREF(شے);
	    Py_DECREF(iterator);
	    واپس -1;
	}
	li->li_tv.v_تالا = 0;
	li->li_tv.v_type = VAR_UNKNOWN;

	اگر (_ConvertFromPyObject(شے, &li->li_tv, lookup_dict) == -1)
	{
	    Py_DECREF(شے);
	    Py_DECREF(iterator);
	    listitem_free(l, li);
	    واپس -1;
	}

	Py_DECREF(شے);

	list_append(l, li);
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    اگر (PyErr_Occurred())
	واپس -1;

    واپس 0;
}

    ساکن PyObject *
ListConstructor(PyTypeObject *subtype, PyObject *دلائل, PyObject *kwargs)
{
    list_T	*list;
    PyObject	*obj = تہی;

    اگر (kwargs)
    {
	Py_Err_سیٹ_STRING(PyExc_TypeError,
		N_("list constructor does not accept keyword arguments"));
	واپس تہی;
    }

    اگر (!PyArg_ParseTuple(دلائل, "|O", &obj))
	واپس تہی;

    اگر (!(list = py_list_alloc()))
	واپس تہی;

    اگر (obj)
    {
	PyObject	*lookup_dict;

	اگر (!(lookup_dict = PyDict_New()))
	{
	    list_unref(list);
	    واپس تہی;
	}

	اگر (list_py_concat(list, obj, lookup_dict) == -1)
	{
	    Py_DECREF(lookup_dict);
	    list_unref(list);
	    واپس تہی;
	}

	Py_DECREF(lookup_dict);
    }

    واپس ListNew(subtype, list);
}

    ساکن خالی
ListDestructor(PyObject *self_obj)
{
    ListObject *self = (ListObject*)self_obj;
    pyll_remove(&self->ref, &lastlist);
    list_unref(self->list);

    DESTRUCTOR_اختتام(self);
}

    ساکن PyInt
List_لمبائی(ListObject *self)
{
    واپس ((PyInt) (self->list->lv_len));
}

    ساکن PyObject *
List_اشاریہ(ListObject *self, Py_ssize_t اشاریہ)
{
    listitem_T	*li;

    اگر (اشاریہ >= List_لمبائی(self))
    {
	Py_Err_سیٹ_STRING(Py_Exc_اشاریہ_Error, N_("list index out of range"));
	واپس تہی;
    }
    li = list_ڈھونڈو(self->list, (طویل) اشاریہ);
    اگر (li == تہی)
    {
	// No more suitable format specifications in python-2.3
	PyErr_VIM_FORMAT(N_("internal error: failed to get Vim list item %d"),
		(صحیح) اشاریہ);
	واپس تہی;
    }
    واپس Convert_مقصد_Py_Object(&li->li_tv);
}

    ساکن PyObject *
List_قاش(ListObject *self, Py_ssize_t پہلا, Py_ssize_t قدم,
	  Py_ssize_t slicelen)
{
    PyInt	i;
    PyObject	*list;

    اگر (قدم == 0)
    {
	Py_Err_سیٹ_STRING(Py_Exc_قدر_Error, N_("slice step cannot be zero"));
	واپس تہی;
    }

    list = PyList_New(slicelen);
    اگر (list == تہی)
	واپس تہی;

    برائے (i = 0; i < slicelen; ++i)
    {
	PyObject	*شے;

	شے = List_اشاریہ(self, پہلا + i*قدم);
	اگر (شے == تہی)
	{
	    Py_DECREF(list);
	    واپس تہی;
	}

	Py_List_سیٹ_شے(list, i, شے);
    }

    واپس list;
}

    ساکن PyObject *
List_شے(ListObject *self, PyObject* idx)
{
#اگر PY_MAJOR_نسخہ < 3
    اگر (PyInt_Check(idx))
    {
	طویل _idx = PyInt_AsLong(idx);
	واپس List_اشاریہ(self, _idx);
    }
    ورنہ
#اگر_ختم
    اگر (PyLong_Check(idx))
    {
	طویل _idx = PyLong_AsLong(idx);
	واپس List_اشاریہ(self, _idx);
    }
    ورنہ اگر (Py_قاش_Check(idx))
    {
	Py_ssize_t آغاز, stop, قدم, slicelen;

	اگر (Py_قاش_Get_Indices_Ex((Py_قاش_Object_T *)idx, List_لمبائی(self),
				 &آغاز, &stop, &قدم, &slicelen) < 0)
	    واپس تہی;
	واپس List_قاش(self, آغاز, قدم, slicelen);
    }
    ورنہ
    {
	RAISE_INVALID_اشاریہ_TYPE(idx);
	واپس تہی;
    }
}

    ساکن خالی
list_restore(Py_ssize_t numadded, Py_ssize_t numreplaced, Py_ssize_t slicelen,
	list_T *l, listitem_T **lis, listitem_T *lastaddedli)
{
    جب (numreplaced--)
    {
	list_داخل_کرو(l, lis[numreplaced], lis[slicelen + numreplaced]);
	listitem_remove(l, lis[slicelen + numreplaced]);
    }
    جب (numadded--)
    {
	listitem_T	*اگلا;

	اگلا = lastaddedli->li_پچھلا;
	listitem_remove(l, lastaddedli);
	lastaddedli = اگلا;
    }
}

    ساکن صحیح
List_Ass_قاش(ListObject *self, Py_ssize_t پہلا,
	     Py_ssize_t قدم, Py_ssize_t slicelen, PyObject *obj)
{
    PyObject	*iterator;
    PyObject	*شے;
    listitem_T	*li;
    listitem_T	*lastaddedli = تہی;
    listitem_T	*اگلا;
    typval_T	v;
    list_T	*l = self->list;
    PyInt	i;
    PyInt	j;
    PyInt	numreplaced = 0;
    PyInt	numadded = 0;
    PyInt	سائز;
    listitem_T	**lis = تہی;

    سائز = List_لمبائی(self);

    اگر (l->lv_تالا)
    {
	RAISE_LOCKED_LIST;
	واپس -1;
    }

    اگر (قدم == 0)
    {
	Py_Err_سیٹ_STRING(Py_Exc_قدر_Error, N_("slice step cannot be zero"));
	واپس -1;
    }

    اگر (قدم != 1 && slicelen == 0)
    {
	// Nothing to do. Only error out if obj has some items.
	صحیح		ret = 0;

	اگر (obj == تہی)
	    واپس 0;

	اگر (!(iterator = PyObject_GetIter(obj)))
	    واپس -1;

	اگر ((شے = Py_Iter_اگلا(iterator)))
	{
	    PyErr_FORMAT(Py_Exc_قدر_Error,
		    N_("attempt to assign sequence of size greater than %d "
			"to extended slice"), 0);
	    Py_DECREF(شے);
	    ret = -1;
	}
	Py_DECREF(iterator);
	واپس ret;
    }

    اگر (obj != تہی)
	// XXX May allocate zero bytes.
	اگر (!(lis = PyMem_New(listitem_T *, slicelen * 2)))
	{
	    Py_Err_No_حافظہ();
	    واپس -1;
	}

    اگر (پہلا == سائز)
	li = تہی;
    ورنہ
    {
	li = list_ڈھونڈو(l, (طویل) پہلا);
	اگر (li == تہی)
	{
	    PyErr_VIM_FORMAT(N_("internal error: no Vim list item %d"),
		    (صحیح)پہلا);
	    اگر (obj != تہی)
		PyMem_Free(lis);
	    واپس -1;
	}
	i = slicelen;
	جب (i-- && li != تہی)
	{
	    j = قدم;
	    اگلا = li;
	    اگر (قدم > 0)
		جب (اگلا != تہی && ((اگلا = اگلا->li_اگلا) != تہی) && --j);
	    ورنہ
		جب (اگلا != تہی && ((اگلا = اگلا->li_پچھلا) != تہی) && ++j);

	    اگر (obj == تہی)
		listitem_remove(l, li);
	    ورنہ
		lis[slicelen - i - 1] = li;

	    li = اگلا;
	}
	اگر (li == تہی && i != -1)
	{
	    Py_Err_سیٹ_VIM(N_("internal error: not enough list items"));
	    اگر (obj != تہی)
		PyMem_Free(lis);
	    واپس -1;
	}
    }

    اگر (obj == تہی)
	واپس 0;

    اگر (!(iterator = PyObject_GetIter(obj)))
    {
	PyMem_Free(lis);
	واپس -1;
    }

    i = 0;
    جب ((شے = Py_Iter_اگلا(iterator)))
    {
	اگر (ConvertFromPyObject(شے, &v) == -1)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(شے);
	    PyMem_Free(lis);
	    واپس -1;
	}
	Py_DECREF(شے);
	اگر (list_داخل_کرو_tv(l, &v, numreplaced < slicelen
				    ? lis[numreplaced]
				    : li) == FAIL)
	{
	    clear_tv(&v);
	    Py_Err_سیٹ_VIM(N_("internal error: failed to add item to list"));
	    list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	    PyMem_Free(lis);
	    واپس -1;
	}
	اگر (numreplaced < slicelen)
	{
	    lis[slicelen + numreplaced] = lis[numreplaced]->li_پچھلا;
	    vimlist_remove(l, lis[numreplaced], lis[numreplaced]);
	    numreplaced++;
	}
	ورنہ
	{
	    اگر (li)
		lastaddedli = li->li_پچھلا;
	    ورنہ
		lastaddedli = l->lv_u.mat.lv_آخری;
	    numadded++;
	}
	clear_tv(&v);
	اگر (قدم != 1 && i >= slicelen)
	{
	    Py_DECREF(iterator);
	    PyErr_FORMAT(Py_Exc_قدر_Error,
		    N_("attempt to assign sequence of size greater than %d "
			"to extended slice"), (صحیح) slicelen);
	    list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	    PyMem_Free(lis);
	    واپس -1;
	}
	++i;
    }
    Py_DECREF(iterator);

    اگر (قدم != 1 && i != slicelen)
    {
	PyErr_FORMAT2(Py_Exc_قدر_Error,
		N_("attempt to assign sequence of size %d to extended slice "
		    "of size %d"), (صحیح) i, (صحیح) slicelen);
	list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	PyMem_Free(lis);
	واپس -1;
    }

    اگر (PyErr_Occurred())
    {
	list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	PyMem_Free(lis);
	واپس -1;
    }

    برائے (i = 0; i < numreplaced; i++)
	listitem_free(l, lis[i]);
    اگر (قدم == 1)
	برائے (i = numreplaced; i < slicelen; i++)
	    listitem_remove(l, lis[i]);

    PyMem_Free(lis);

    واپس 0;
}

    ساکن صحیح
List_Ass_اشاریہ(ListObject *self, Py_ssize_t اشاریہ, PyObject *obj)
{
    typval_T	tv;
    list_T	*l = self->list;
    listitem_T	*li;
    Py_ssize_t	لمبائی = List_لمبائی(self);

    اگر (l->lv_تالا)
    {
	RAISE_LOCKED_LIST;
	واپس -1;
    }
    اگر (اشاریہ > لمبائی || (اشاریہ == لمبائی && obj == تہی))
    {
	Py_Err_سیٹ_STRING(Py_Exc_اشاریہ_Error, N_("list index out of range"));
	واپس -1;
    }

    اگر (obj == تہی)
    {
	li = list_ڈھونڈو(l, (طویل) اشاریہ);
	اگر (li == تہی)
	{
	    PyErr_VIM_FORMAT(N_("internal error: failed to get Vim "
			"list item %d"), (صحیح) اشاریہ);
	    واپس -1;
	}
	vimlist_remove(l, li, li);
	clear_tv(&li->li_tv);
	vim_free(li);
	واپس 0;
    }

    اگر (ConvertFromPyObject(obj, &tv) == -1)
	واپس -1;

    اگر (اشاریہ == لمبائی)
    {
	اگر (list_append_tv(l, &tv) == FAIL)
	{
	    clear_tv(&tv);
	    Py_Err_سیٹ_VIM(N_("failed to add item to list"));
	    واپس -1;
	}
    }
    ورنہ
    {
	li = list_ڈھونڈو(l, (طویل) اشاریہ);
	اگر (li == تہی)
	{
	    PyErr_VIM_FORMAT(N_("internal error: failed to get Vim "
			"list item %d"), (صحیح) اشاریہ);
	    واپس -1;
	}
	clear_tv(&li->li_tv);
	copy_tv(&tv, &li->li_tv);
	clear_tv(&tv);
    }
    واپس 0;
}

    ساکن صحیح
List_Ass_شے(ListObject *self, PyObject *idx, PyObject *obj)
{
#اگر PY_MAJOR_نسخہ < 3
    اگر (PyInt_Check(idx))
    {
	طویل _idx = PyInt_AsLong(idx);
	واپس (صحیح)List_Ass_اشاریہ(self, _idx, obj);
    }
    ورنہ
#اگر_ختم
    اگر (PyLong_Check(idx))
    {
	طویل _idx = PyLong_AsLong(idx);
	واپس (صحیح)List_Ass_اشاریہ(self, _idx, obj);
    }
    ورنہ اگر (Py_قاش_Check(idx))
    {
	Py_ssize_t آغاز, stop, قدم, slicelen;

	اگر (Py_قاش_Get_Indices_Ex((Py_قاش_Object_T *)idx, List_لمبائی(self),
				 &آغاز, &stop, &قدم, &slicelen) < 0)
	    واپس -1;
	واپس (صحیح)List_Ass_قاش(self, آغاز, قدم, slicelen,
		obj);
    }
    ورنہ
    {
	RAISE_INVALID_اشاریہ_TYPE(idx);
	واپس -1;
    }
}

    ساکن PyObject *
List_Concat_اندر_رکھو(ListObject *self, PyObject *obj)
{
    list_T	*l = self->list;
    PyObject	*lookup_dict;

    اگر (l->lv_تالا)
    {
	RAISE_LOCKED_LIST;
	واپس تہی;
    }

    اگر (!(lookup_dict = PyDict_New()))
	واپس تہی;

    اگر (list_py_concat(l, obj, lookup_dict) == -1)
    {
	Py_DECREF(lookup_dict);
	واپس تہی;
    }
    Py_DECREF(lookup_dict);

    Py_INCREF((PyObject *)self);
    واپس (PyObject *)(self);
}

قسمتعریف ساخت
{
    listwatch_T	lw;
    list_T	*list;
} listiterinfo_T;

    ساکن خالی
ListIterDestruct(خالی *arg)
{
    listiterinfo_T *lii = (listiterinfo_T*)arg;
    list_rem_watch(lii->list, &lii->lw);
    list_unref(lii->list);
    PyMem_Free(lii);
}

    ساکن PyObject *
List_Iter_اگلا(خالی **arg)
{
    PyObject	*ret;
    listiterinfo_T **lii = (listiterinfo_T**)arg;

    اگر (!((*lii)->lw.lw_شے))
	واپس تہی;

    اگر (!(ret = Convert_مقصد_Py_Object(&((*lii)->lw.lw_شے->li_tv))))
	واپس تہی;

    (*lii)->lw.lw_شے = (*lii)->lw.lw_شے->li_اگلا;

    واپس ret;
}

    ساکن PyObject *
ListIter(PyObject *self_obj)
{
    ListObject	*self = (ListObject*)self_obj;
    listiterinfo_T	*lii;
    list_T	*l = self->list;

    اگر (!(lii = PyMem_New(listiterinfo_T, 1)))
    {
	Py_Err_No_حافظہ();
	واپس تہی;
    }

    CHECK_LIST_MATERIALIZE(l);
    list_جوڑو_watch(l, &lii->lw);
    lii->lw.lw_شے = l->lv_پہلا;
    lii->list = l;
    ++l->lv_refcount;

    واپس IterNew(lii,
	    ListIterDestruct, List_Iter_اگلا,
	    تہی, تہی, (PyObject *)self);
}

ساکن حرف *ListAttrs[] = {
    "locked",
    تہی
};

    ساکن PyObject *
ListDir(PyObject *self, PyObject *دلائل غیر_مستعمل)
{
    واپس ObjectDir(self, ListAttrs);
}

    ساکن صحیح
ListSetattr(PyObject *self_obj, حرف *name, PyObject *valObject)
{
    ListObject *self = (ListObject*)self_obj;
    اگر (valObject == تہی)
    {
	Py_Err_سیٹ_STRING(PyExc_AttributeError,
		N_("cannot delete vim.List attributes"));
	واپس -1;
    }

    اگر (strcmp(name, "locked") == 0)
    {
	اگر (self->list->lv_تالا == VAR_FIXED)
	{
	    Py_Err_سیٹ_STRING(PyExc_TypeError, N_("cannot modify fixed list"));
	    واپس -1;
	}
	ورنہ
	{
	    صحیح		istrue = PyObject_IsTrue(valObject);
	    اگر (istrue == -1)
		واپس -1;
	    ورنہ اگر (istrue)
		self->list->lv_تالا = VAR_LOCKED;
	    ورنہ
		self->list->lv_تالا = 0;
	}
	واپس 0;
    }
    ورنہ
    {
	PyErr_FORMAT(PyExc_AttributeError, N_("cannot set attribute %s"), name);
	واپس -1;
    }
}

ساکن PySequenceMethods ListAsSeq = {
    (lenfunc)		List_لمبائی,	 // sq_length,	  len(x)
    (binaryfunc)	0,		 // RangeConcat, sq_concat,  x+y
    0,					 // RangeRepeat, sq_repeat,  x*n
    (PyIntArgFunc)	List_اشاریہ,	 // sq_item,	  x[i]
    0,					 // was_sq_slice,     x[i:j]
    (PyIntObjArgProc)	List_Ass_اشاریہ,	 // sq_as_item,  x[i]=v
    0,					 // was_sq_ass_slice, x[i:j]=v
    0,					 // sq_contains
    (binaryfunc)	List_Concat_اندر_رکھو,// sq_inplace_concat
    0,					 // sq_inplace_repeat
};

ساکن PyMappingMethods ListAsMapping = {
    /* mp_length	*/ (lenfunc) List_لمبائی,
    /* mp_subscript     */ (binaryfunc) List_شے,
    /* mp_ass_subscript */ (objobjargproc) List_Ass_شے,
};

ساکن ساخت Py_طریقہ_Def ListMethods[] = {
    {"extend",	(PyCFunction)List_Concat_اندر_رکھو,	METH_O,		""},
    {"__dir__",	(PyCFunction)ListDir,		METH_NOARGS,	""},
    { تہی,	تہی,				0,		تہی}
};

DEFINE_PY_TYPE_OBJECT(TupleType);

قسمتعریف ساخت
{
    Py_Object_سر
    tuple_T	*tuple;
    pylinkedlist_T	ref;
} TupleObject;

#تعریف_کرو NEW_TUPLE(tuple) TupleNew(TupleTypePtr, tuple)

    ساکن PyObject *
TupleNew(PyTypeObject *subtype, tuple_T *tuple)
{
    TupleObject	*self;

    اگر (tuple == تہی)
	واپس تہی;

    self = (TupleObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    اگر (self == تہی)
	واپس تہی;
    self->tuple = tuple;
    ++tuple->tv_refcount;

    pyll_جوڑو((PyObject *)(self), &self->ref, &lasttuple);

    واپس (PyObject *)(self);
}

    ساکن tuple_T *
py_tuple_alloc(خالی)
{
    tuple_T	*ret;

    اگر (!(ret = tuple_alloc()))
    {
	Py_Err_No_حافظہ();
	واپس تہی;
    }
    ++ret->tv_refcount;

    واپس ret;
}

    ساکن صحیح
tuple_py_concat(tuple_T *t, PyObject *obj, PyObject *lookup_dict)
{
    PyObject	*iterator;
    PyObject	*شے;

    اگر (!(iterator = PyObject_GetIter(obj)))
	واپس -1;

    جب ((شے = Py_Iter_اگلا(iterator)))
    {
	typval_T    new_tv;

	اگر (_ConvertFromPyObject(شے, &new_tv, lookup_dict) == -1)
	{
	    Py_DECREF(شے);
	    Py_DECREF(iterator);
	    واپس -1;
	}

	Py_DECREF(شے);

	اگر (tuple_append_tv(t, &new_tv) == FAIL)
	{
	    Py_DECREF(iterator);
	    واپس -1;
	}
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    اگر (PyErr_Occurred())
	واپس -1;

    واپس 0;
}

    ساکن PyObject *
TupleConstructor(PyTypeObject *subtype, PyObject *دلائل, PyObject *kwargs)
{
    tuple_T	*tuple;
    PyObject	*obj = تہی;

    اگر (kwargs)
    {
	Py_Err_سیٹ_STRING(PyExc_TypeError,
		N_("tuple constructor does not accept keyword arguments"));
	واپس تہی;
    }

    اگر (!PyArg_ParseTuple(دلائل, "|O", &obj))
	واپس تہی;

    اگر (!(tuple = py_tuple_alloc()))
	واپس تہی;

    اگر (obj)
    {
	PyObject	*lookup_dict;

	اگر (!(lookup_dict = PyDict_New()))
	{
	    tuple_unref(tuple);
	    واپس تہی;
	}

	اگر (tuple_py_concat(tuple, obj, lookup_dict) == -1)
	{
	    Py_DECREF(lookup_dict);
	    tuple_unref(tuple);
	    واپس تہی;
	}

	Py_DECREF(lookup_dict);
    }

    واپس TupleNew(subtype, tuple);
}

    ساکن خالی
TupleDestructor(PyObject *self_obj)
{
    TupleObject *self = (TupleObject *)self_obj;
    pyll_remove(&self->ref, &lasttuple);
    tuple_unref(self->tuple);

    DESTRUCTOR_اختتام(self);
}

    ساکن PyInt
Tuple_لمبائی(TupleObject *self)
{
    واپس ((PyInt)(tuple_len(self->tuple)));
}

    ساکن PyObject *
Tuple_اشاریہ(TupleObject *self, Py_ssize_t اشاریہ)
{
    PyInt	len = Tuple_لمبائی(self);

    اگر (اشاریہ < 0)
	اشاریہ = len + اشاریہ;

    اگر (اشاریہ < 0 || اشاریہ >= len)
    {
	Py_Err_سیٹ_STRING(Py_Exc_اشاریہ_Error, N_("tuple index out of range"));
	واپس تہی;
    }
    واپس Convert_مقصد_Py_Object(TUPLE_شے(self->tuple, اشاریہ));
}

/*
 * Return a new tuple object for the tuple slice starting from the index
 * "first" and of length "slicelen" skipping "step" items.
 */
    ساکن PyObject *
Tuple_قاش(
    TupleObject		*self,
    Py_ssize_t		پہلا,
    Py_ssize_t		قدم,
    Py_ssize_t		slicelen)
{
    PyInt	i;
    PyObject	*tuple;

    tuple = PyTuple_New(slicelen);
    اگر (tuple == تہی)
	واپس تہی;

    برائے (i = 0; i < slicelen; ++i)
    {
	PyObject	*شے;

	شے = Tuple_اشاریہ(self, پہلا + i * قدم);
	اگر (شے == تہی)
	{
	    Py_DECREF(tuple);
	    واپس تہی;
	}

	Py_Tuple_سیٹ_شے(tuple, i, شے);
    }

    واپس tuple;
}

    ساکن PyObject *
Tuple_شے(TupleObject *self, PyObject* idx)
{
#اگر PY_MAJOR_نسخہ < 3
    اگر (PyInt_Check(idx))
    {
	طویل _idx = PyInt_AsLong(idx);
	واپس Tuple_اشاریہ(self, _idx);
    }
    ورنہ
#اگر_ختم
    اگر (PyLong_Check(idx))
    {
	طویل _idx = PyLong_AsLong(idx);
	واپس Tuple_اشاریہ(self, _idx);
    }
    ورنہ اگر (Py_قاش_Check(idx))
    {
	Py_ssize_t آغاز, stop, قدم, slicelen;

	اگر (Py_قاش_Get_Indices_Ex((Py_قاش_Object_T *)idx, Tuple_لمبائی(self),
				 &آغاز, &stop, &قدم, &slicelen) < 0)
	    واپس تہی;
	واپس Tuple_قاش(self, آغاز, قدم, slicelen);
    }
    ورنہ
    {
	RAISE_INVALID_اشاریہ_TYPE(idx);
	واپس تہی;
    }
}

قسمتعریف ساخت
{
    tuple_T	*tuple;
    صحیح		اشاریہ;
} tupleiterinfo_T;

    ساکن خالی
TupleIterDestruct(خالی *arg)
{
    tupleiterinfo_T *tii = (tupleiterinfo_T*)arg;
    tuple_unref(tii->tuple);
    PyMem_Free(tii);
}

    ساکن PyObject *
Tuple_Iter_اگلا(خالی **arg)
{
    PyObject	*ret;
    tupleiterinfo_T **tii = (tupleiterinfo_T**)arg;

    اگر ((*tii)->اشاریہ >= TUPLE_LEN((*tii)->tuple))
	واپس تہی;

    اگر (!(ret = Convert_مقصد_Py_Object(TUPLE_شے((*tii)->tuple, (*tii)->اشاریہ))))
	واپس تہی;

    (*tii)->اشاریہ++;

    واپس ret;
}

    ساکن PyObject *
TupleIter(PyObject *self_obj)
{
    TupleObject	*self = (TupleObject*)self_obj;
    tupleiterinfo_T	*tii;
    tuple_T	*t = self->tuple;

    اگر (!(tii = PyMem_New(tupleiterinfo_T, 1)))
    {
	Py_Err_No_حافظہ();
	واپس تہی;
    }

    tii->tuple = t;
    tii->اشاریہ = 0;
    ++t->tv_refcount;

    واپس IterNew(tii,
	    TupleIterDestruct, Tuple_Iter_اگلا,
	    تہی, تہی, (PyObject *)self);
}

ساکن حرف *TupleAttrs[] = {
    "locked",
    تہی
};

    ساکن PyObject *
TupleDir(PyObject *self, PyObject *دلائل غیر_مستعمل)
{
    واپس ObjectDir(self, TupleAttrs);
}

    ساکن صحیح
TupleSetattr(PyObject *self_obj, حرف *name, PyObject *valObject)
{
    TupleObject *self = (TupleObject*)self_obj;
    اگر (valObject == تہی)
    {
	Py_Err_سیٹ_STRING(PyExc_AttributeError,
		N_("cannot delete vim.Tuple attributes"));
	واپس -1;
    }

    اگر (strcmp(name, "locked") == 0)
    {
	اگر (self->tuple->tv_تالا == VAR_FIXED)
	{
	    Py_Err_سیٹ_STRING(PyExc_TypeError, N_("cannot modify fixed tuple"));
	    واپس -1;
	}
	ورنہ
	{
	    صحیح		istrue = PyObject_IsTrue(valObject);
	    اگر (istrue == -1)
		واپس -1;
	    ورنہ اگر (istrue)
		self->tuple->tv_تالا = VAR_LOCKED;
	    ورنہ
		self->tuple->tv_تالا = 0;
	}
	واپس 0;
    }
    ورنہ
    {
	PyErr_FORMAT(PyExc_AttributeError, N_("cannot set attribute %s"), name);
	واپس -1;
    }
}

ساکن PySequenceMethods TupleAsSeq = {
    (lenfunc)		Tuple_لمبائی,	 // sq_length,	  len(x)
    (binaryfunc)	0,		 // RangeConcat, sq_concat,  x+y
    0,					 // RangeRepeat, sq_repeat,  x*n
    (PyIntArgFunc)	Tuple_اشاریہ,	 // sq_item,	  x[i]
    0,					 // was_sq_slice,     x[i:j]
    (PyIntObjArgProc)	0,		 // sq_as_item,  x[i]=v
    0,					 // was_sq_ass_slice, x[i:j]=v
    0,					 // sq_contains
    (binaryfunc)	0,		 // sq_inplace_concat
    0,					 // sq_inplace_repeat
};

ساکن PyMappingMethods TupleAsMapping = {
    /* mp_length	*/ (lenfunc) Tuple_لمبائی,
    /* mp_subscript     */ (binaryfunc) Tuple_شے,
    /* mp_ass_subscript */ (objobjargproc) 0,
};

ساکن ساخت Py_طریقہ_Def TupleMethods[] = {
    {"__dir__",	(PyCFunction)TupleDir,		METH_NOARGS,	""},
    { تہی,	تہی,				0,		تہی}
};

قسمتعریف ساخت
{
    Py_Object_سر
    char_u	*name;
    صحیح		argc;
    typval_T	*argv;
    dict_T	*self;
    pylinkedlist_T	ref;
    صحیح		auto_rebind;
} FunctionObject;

DEFINE_PY_TYPE_OBJECT(FunctionType);

#تعریف_کرو NEW_FUNCTION(name, argc, argv, self, pt_auto) \
    FunctionNew(FunctionTypePtr, (name), (argc), (argv), (self), (pt_auto))

    ساکن PyObject *
FunctionNew(PyTypeObject *subtype, char_u *name, صحیح argc, typval_T *argv,
	dict_T *selfdict, صحیح auto_rebind)
{
    FunctionObject	*self;

    self = (FunctionObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    اگر (self == تہی)
	واپس تہی;

    اگر (ہندسہ_ہے((غیرعلامتی حرف)*name))
    {
	اگر (!translated_function_exists(name, FALSE))
	{
	    PyErr_FORMAT(Py_Exc_قدر_Error,
		    N_("unnamed function %s does not exist"), name);
	    واپس تہی;
	}
	self->name = vim_strsave(name);
    }
    ورنہ
    {
	char_u *p;

	اگر ((p = get_expanded_name(name,
			    vim_strchr(name, AUTOLOAD_CHAR) == تہی)) == تہی)
	{
	    PyErr_FORMAT(Py_Exc_قدر_Error,
		    N_("function %s does not exist"), name);
	    واپس تہی;
	}

	اگر (p[0] == K_SPECIAL && p[1] == KS_EXTRA && p[2] == (صحیح)KE_SNR)
	{
	    char_u *np;
	    size_t len = STRLEN(p) + 1;

	    اگر ((np = alloc(len + 2)) == تہی)
	    {
		vim_free(p);
		واپس تہی;
	    }
	    mch_memmove(np, "<SNR>", 5);
	    mch_memmove(np + 5, p + 3, len - 3);
	    vim_free(p);
	    self->name = np;
	}
	ورنہ
	    self->name = p;
    }

    func_ref(self->name);
    self->argc = argc;
    self->argv = argv;
    self->self = selfdict;
    self->auto_rebind = selfdict == تہی ? TRUE : auto_rebind;

    اگر (self->argv || self->self)
	pyll_جوڑو((PyObject *)(self), &self->ref, &lastfunc);

    واپس (PyObject *)(self);
}

    ساکن PyObject *
FunctionConstructor(PyTypeObject *subtype, PyObject *دلائل, PyObject *kwargs)
{
    PyObject	*self;
    PyObject	*selfdictObject;
    PyObject	*autoRebindObject;
    PyObject	*دلائل_Object = تہی;
    char_u	*name;
    typval_T	selfdicttv;
    typval_T	argstv;
    list_T	*argslist = تہی;
    dict_T	*selfdict = تہی;
    صحیح		argc = 0;
    صحیح		auto_rebind = TRUE;
    typval_T	*argv = تہی;
    typval_T	*curtv;
    listitem_T	*li;

    اگر (kwargs != تہی)
    {
	selfdictObject = Py_Dict_Get_شے_String(kwargs, "self");
	اگر (selfdictObject != تہی)
	{
	    اگر (ConvertFromPyMapping(selfdictObject, &selfdicttv) == -1)
		واپس تہی;
	    selfdict = selfdicttv.vval.v_dict;
	}
	دلائل_Object = Py_Dict_Get_شے_String(kwargs, "args");
	اگر (دلائل_Object != تہی)
	{
	    اگر (ConvertFromPySequence(دلائل_Object, &argstv) == -1)
	    {
		dict_unref(selfdict);
		واپس تہی;
	    }
	    argslist = argstv.vval.v_list;
	    CHECK_LIST_MATERIALIZE(argslist);

	    argc = argslist->lv_len;
	    اگر (argc != 0)
	    {
		argv = PyMem_New(typval_T, (size_t) argc);
		اگر (argv == تہی)
		{
		    Py_Err_No_حافظہ();
		    dict_unref(selfdict);
		    list_unref(argslist);
		    واپس تہی;
		}
		curtv = argv;
		FOR_ALL_LIST_اشیاء(argslist, li)
		    copy_tv(&li->li_tv, curtv++);
	    }
	    list_unref(argslist);
	}
	اگر (selfdict != تہی)
	{
	    auto_rebind = FALSE;
	    autoRebindObject = Py_Dict_Get_شے_String(kwargs, "auto_rebind");
	    اگر (autoRebindObject != تہی)
	    {
		auto_rebind = PyObject_IsTrue(autoRebindObject);
		اگر (auto_rebind == -1)
		{
		    dict_unref(selfdict);
		    list_unref(argslist);
		    واپس تہی;
		}
	    }
	}
    }

    اگر (!PyArg_ParseTuple(دلائل, "et", "ascii", &name))
    {
	dict_unref(selfdict);
	جب (argc--)
	    clear_tv(&argv[argc]);
	PyMem_Free(argv);
	واپس تہی;
    }

    self = FunctionNew(subtype, name, argc, argv, selfdict, auto_rebind);

    PyMem_Free(name);

    واپس self;
}

    ساکن خالی
FunctionDestructor(PyObject *self_obj)
{
    FunctionObject *self = (FunctionObject*)self_obj;
    صحیح i;
    func_unref(self->name);
    vim_free(self->name);
    برائے (i = 0; i < self->argc; ++i)
	clear_tv(&self->argv[i]);
    PyMem_Free(self->argv);
    dict_unref(self->self);
    اگر (self->argv || self->self)
	pyll_remove(&self->ref, &lastfunc);

    DESTRUCTOR_اختتام(self);
}

ساکن حرف *FunctionAttrs[] = {
    "softspace", "args", "self", "auto_rebind",
    تہی
};

    ساکن PyObject *
FunctionDir(PyObject *self, PyObject *دلائل غیر_مستعمل)
{
    واپس ObjectDir(self, FunctionAttrs);
}

    ساکن PyObject *
FunctionAttr(FunctionObject *self, حرف *name)
{
    list_T *list;
    صحیح i;
    اگر (strcmp(name, "name") == 0)
	واپس PyString_FromString((حرف *)(self->name));
    ورنہ اگر (strcmp(name, "args") == 0)
    {
	اگر (self->argv == تہی || (list = list_alloc()) == تہی)
	    واپس ALWAYS_NONE;

	برائے (i = 0; i < self->argc; ++i)
	    list_append_tv(list, &self->argv[i]);
	واپس NEW_LIST(list);
    }
    ورنہ اگر (strcmp(name, "self") == 0)
	واپس self->self == تہی
	    ? ALWAYS_NONE
	    : NEW_DICTIONARY(self->self);
    ورنہ اگر (strcmp(name, "auto_rebind") == 0)
	واپس self->auto_rebind
	    ? ALWAYS_TRUE
	    : ALWAYS_FALSE;
    ورنہ اگر (strcmp(name, "__members__") == 0)
	واپس ObjectDir(تہی, FunctionAttrs);
    واپس تہی;
}

/*
 * Populate partial_T given function object.
 *
 * "exported" should be set to true when it is needed to construct a partial
 * that may be stored in a variable (i.e. may be freed by Vim).
 */
    ساکن خالی
سیٹ_partial(FunctionObject *self, partial_T *pt, صحیح exported)
{
    صحیح i;

    pt->pt_name = self->name;
    اگر (self->argv)
    {
	pt->pt_argc = self->argc;
	اگر (exported)
	{
	    pt->pt_argv = ALLOC_CLEAR_MULT(typval_T, self->argc);
	    برائے (i = 0; i < pt->pt_argc; ++i)
		copy_tv(&self->argv[i], &pt->pt_argv[i]);
	}
	ورنہ
	    pt->pt_argv = self->argv;
    }
    ورنہ
    {
	pt->pt_argc = 0;
	pt->pt_argv = تہی;
    }
    pt->pt_auto = self->auto_rebind || !exported;
    pt->pt_dict = self->self;
    اگر (exported && self->self)
	++pt->pt_dict->dv_refcount;
    اگر (exported)
	pt->pt_name = vim_strsave(pt->pt_name);
    pt->pt_refcount = 1;
}

    ساکن PyObject *
FunctionCall(PyObject *self_obj, PyObject *دلائل_Object, PyObject *kwargs)
{
    FunctionObject *self = (FunctionObject*)self_obj;
    char_u	*name = self->name;
    typval_T	دلائل;
    typval_T	selfdicttv;
    typval_T	rettv;
    dict_T	*selfdict = تہی;
    PyObject	*selfdictObject;
    PyObject	*ret;
    صحیح		error;
    partial_T	pt;
    partial_T	*pt_ptr = تہی;

    اگر (ConvertFromPySequence(دلائل_Object, &دلائل) == -1)
	واپس تہی;

    اگر (kwargs != تہی)
    {
	selfdictObject = Py_Dict_Get_شے_String(kwargs, "self");
	اگر (selfdictObject != تہی)
	{
	    اگر (ConvertFromPyMapping(selfdictObject, &selfdicttv) == -1)
	    {
		clear_tv(&دلائل);
		واپس تہی;
	    }
	    selfdict = selfdicttv.vval.v_dict;
	}
    }

    اگر (self->argv || self->self)
    {
	CLEAR_میدان(pt);
	سیٹ_partial(self, &pt, FALSE);
	pt_ptr = &pt;
    }

    Py_BEGIN_ALLOW_دھاگے
    Python_تالا_Vim();

    Vim_Try_آغاز();
    error = func_call(name, &دلائل, pt_ptr, selfdict, &rettv);

    Python_Release_Vim();
    Py_انتہا_ALLOW_دھاگے

    اگر (Vim_Try_انتہا())
	ret = تہی;
    ورنہ اگر (error != OK)
    {
	ret = تہی;
	PyErr_VIM_FORMAT(N_("failed to run function %s"), (حرف *)name);
    }
    ورنہ
	ret = Convert_مقصد_Py_Object(&rettv);

    clear_tv(&دلائل);
    clear_tv(&rettv);
    اگر (selfdict != تہی)
	clear_tv(&selfdicttv);

    واپس ret;
}

    ساکن PyObject *
FunctionRepr(PyObject *self_obj)
{
    FunctionObject *self = (FunctionObject*)self_obj;
    PyObject *ret;
    garray_T repr_ga;
    صحیح i;
    char_u *tofree = تہی;
    typval_T tv;
    char_u numbuf[NUMBUFLEN];

    ga_init2(&repr_ga, حجم(حرف), 70);
    ga_concat(&repr_ga, (char_u *)"<vim.Function '");
    اگر (self->name)
	ga_concat(&repr_ga, self->name);
    ورنہ
	ga_concat(&repr_ga, (char_u *)"<NULL>");
    ga_append(&repr_ga, '\'');
    اگر (self->argv)
    {
	ga_concat(&repr_ga, (char_u *)", args=[");
	++emsg_silent;
	برائے (i = 0; i < self->argc; i++)
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
    ret = PyString_FromString((حرف *)repr_ga.ga_ڈیٹا);
    ga_clear(&repr_ga);
    واپس ret;
}

ساکن ساخت Py_طریقہ_Def FunctionMethods[] = {
    {"__dir__",	(PyCFunction)FunctionDir,   METH_NOARGS,		""},
    { تہی,	تہی,			0,				تہی}
};

/*
 * Options object
 */

DEFINE_PY_TYPE_OBJECT(OptionsType);

قسمتعریف صحیح (*checkfun)(خالی *);

قسمتعریف ساخت
{
    Py_Object_سر
    صحیح		opt_type;
    خالی	*from;
    checkfun	Check;
    PyObject	*fromObj;
} OptionsObject;

    ساکن صحیح
dummy_check(خالی *arg غیر_مستعمل)
{
    واپس 0;
}

    ساکن PyObject *
OptionsNew(صحیح opt_type, خالی *from, checkfun Check, PyObject *fromObj)
{
    OptionsObject	*self;

    self = PyObject_GC_New(OptionsObject, OptionsTypePtr);
    اگر (self == تہی)
	واپس تہی;

    self->opt_type = opt_type;
    self->from = from;
    self->Check = Check;
    self->fromObj = fromObj;
    اگر (fromObj)
	Py_INCREF(fromObj);

    واپس (PyObject *)(self);
}

    ساکن خالی
OptionsDestructor(PyObject *self_obj)
{
    OptionsObject *self = (OptionsObject*)self_obj;
    PyObject_GC_UnTrack((خالی *)(self));
    Py_XDECREF(self->fromObj);
    PyObject_GC_Del((خالی *)(self));
}

    ساکن صحیح
OptionsTraverse(PyObject *self_obj, visitproc دورہ, خالی *arg)
{
    OptionsObject *self = (OptionsObject*)self_obj;
    Py_دورہ(self->fromObj);
    واپس 0;
}

    ساکن صحیح
OptionsClear(PyObject *self_obj)
{
    OptionsObject *self = (OptionsObject*)self_obj;
    Py_CLEAR(self->fromObj);
    واپس 0;
}

    ساکن PyObject *
Options_شے(OptionsObject *self, PyObject *کلید_Object)
{
    char_u	*کلید;
    صحیح		flags;
    طویل	numval;
    char_u	*stringval;
    PyObject	*todecref;

    اگر (self->Check(self->fromObj))
	واپس تہی;

    اگر (!(کلید = String_مقصد_Chars(کلید_Object, &todecref)))
	واپس تہی;

    اگر (*کلید == NUL)
    {
	RAISE_NO_خالی_خانے_کلیدیں;
	Py_XDECREF(todecref);
	واپس تہی;
    }

    flags = get_option_قدر_strict(کلید, &numval, &stringval,
				    self->opt_type, self->from);

    Py_XDECREF(todecref);

    اگر (flags == 0)
    {
	Py_Err_سیٹ_Object(Py_Exc_کلید_Error, کلید_Object);
	واپس تہی;
    }

    اگر (flags & SOPT_UNSET)
    {
	Py_INCREF(Py_None);
	واپس Py_None;
    }
    ورنہ اگر (flags & SOPT_BOOL)
    {
	PyObject	*ret;
	ret = numval ? Py_True : Py_False;
	Py_INCREF(ret);
	واپس ret;
    }
    ورنہ اگر (flags & SOPT_عدد)
	واپس PyInt_FromLong(numval);
    ورنہ اگر (flags & SOPT_STRING)
    {
	اگر (stringval)
	{
	    PyObject	*ret = Py_بائٹیں_From_String((حرف *)stringval);
	    vim_free(stringval);
	    واپس ret;
	}
	ورنہ
	{
	    Py_Err_سیٹ_STRING(PyExc_RuntimeError,
		    N_("unable to get option value"));
	    واپس تہی;
	}
    }
    ورنہ
    {
	Py_Err_سیٹ_VIM(N_("internal error: unknown option type"));
	واپس تہی;
    }
}

    ساکن صحیح
Options_شامل_ہے(OptionsObject *self, PyObject *کلید_Object)
{
    char_u	*کلید;
    PyObject	*todecref;

    اگر (!(کلید = String_مقصد_Chars(کلید_Object, &todecref)))
	واپس -1;

    اگر (*کلید == NUL)
    {
	Py_XDECREF(todecref);
	واپس 0;
    }

    اگر (get_option_قدر_strict(کلید, تہی, تہی, self->opt_type, تہی))
    {
	Py_XDECREF(todecref);
	واپس 1;
    }
    ورنہ
    {
	Py_XDECREF(todecref);
	واپس 0;
    }
}

قسمتعریف ساخت
{
    خالی	*lastoption;
    صحیح		opt_type;
} optiterinfo_T;

    ساکن PyObject *
Options_Iter_اگلا(خالی **arg)
{
    char_u	*name;
    optiterinfo_T **oii = (optiterinfo_T**)arg;

    اگر ((name = option_iter_اگلا(&((*oii)->lastoption), (*oii)->opt_type)))
	واپس PyString_FromString((حرف *)name);

    واپس تہی;
}

    ساکن PyObject *
OptionsIter(PyObject *self_obj)
{
    OptionsObject	*self = (OptionsObject*)self_obj;
    optiterinfo_T	*oii;

    اگر (!(oii = PyMem_New(optiterinfo_T, 1)))
    {
	Py_Err_No_حافظہ();
	واپس تہی;
    }

    oii->opt_type = self->opt_type;
    oii->lastoption = تہی;

    واپس IterNew(oii,
	    PyMem_Free, Options_Iter_اگلا,
	    تہی, تہی, (PyObject *)self);
}

    ساکن صحیح
سیٹ_option_قدر_err(char_u *کلید, صحیح numval, char_u *stringval, صحیح opt_flags)
{
    حرف	*errmsg;

    اگر ((errmsg = سیٹ_option_قدر(کلید, numval, stringval, opt_flags)))
    {
	اگر (Vim_Try_انتہا())
	    واپس FAIL;
	Py_Err_سیٹ_Vim(errmsg);
	واپس FAIL;
    }
    واپس OK;
}

    ساکن صحیح
سیٹ_option_قدر_for(
	char_u *کلید,
	صحیح numval,
	char_u *stringval,
	صحیح opt_flags,
	صحیح opt_type,
	خالی *from)
{
    switchwin_T	switchwin;
    bufref_T	save_curbuf;
    صحیح		سیٹ_ret = 0;

    Vim_Try_آغاز();
    انتخاب (opt_type)
    {
	حالت SREQ_WIN:
	    اگر (switch_win(&switchwin, (win_T *)from,
			      win_ڈھونڈو_tabpage((win_T *)from), FALSE) == FAIL)
	    {
		restore_win(&switchwin, TRUE);
		اگر (Vim_Try_انتہا())
		    واپس -1;
		Py_Err_سیٹ_VIM(N_("problem while switching windows"));
		واپس -1;
	    }
	    سیٹ_ret = سیٹ_option_قدر_err(کلید, numval, stringval, opt_flags);
	    restore_win(&switchwin, TRUE);
	    توڑو;
	حالت SREQ_BUF:
	    switch_بفر(&save_curbuf, (buf_T *)from);
	    سیٹ_ret = سیٹ_option_قدر_err(کلید, numval, stringval, opt_flags);
	    restore_بفر(&save_curbuf);
	    توڑو;
	حالت SREQ_GLOBAL:
	    سیٹ_ret = سیٹ_option_قدر_err(کلید, numval, stringval, opt_flags);
	    توڑو;
    }
    اگر (سیٹ_ret == FAIL)
	واپس -1;
    واپس Vim_Try_انتہا();
}

    ساکن صحیح
Options_Ass_شے(OptionsObject *self, PyObject *کلید_Object, PyObject *valObject)
{
    char_u	*کلید;
    صحیح		flags;
    صحیح		opt_flags;
    صحیح		ret = 0;
    PyObject	*todecref;

    اگر (self->Check(self->fromObj))
	واپس -1;

    اگر (!(کلید = String_مقصد_Chars(کلید_Object, &todecref)))
	واپس -1;

    اگر (*کلید == NUL)
    {
	RAISE_NO_خالی_خانے_کلیدیں;
	Py_XDECREF(todecref);
	واپس -1;
    }

    flags = get_option_قدر_strict(کلید, تہی, تہی,
				    self->opt_type, self->from);

    اگر (flags == 0)
    {
	Py_Err_سیٹ_Object(Py_Exc_کلید_Error, کلید_Object);
	Py_XDECREF(todecref);
	واپس -1;
    }

    اگر (valObject == تہی)
    {
	اگر (self->opt_type == SREQ_GLOBAL)
	{
	    PyErr_FORMAT(Py_Exc_قدر_Error,
		    N_("unable to unset global option %s"), کلید);
	    Py_XDECREF(todecref);
	    واپس -1;
	}
	ورنہ اگر (!(flags & SOPT_GLOBAL))
	{
	    PyErr_FORMAT(Py_Exc_قدر_Error,
		    N_("unable to unset option %s "
		       "which does not have global value"), کلید);
	    Py_XDECREF(todecref);
	    واپس -1;
	}
	ورنہ
	{
	    unset_global_local_option(کلید, self->from);
	    Py_XDECREF(todecref);
	    واپس 0;
	}
    }

    opt_flags = (self->opt_type ? OPT_LOCAL : OPT_GLOBAL);

    اگر (flags & SOPT_BOOL)
    {
	صحیح	istrue = PyObject_IsTrue(valObject);

	اگر (istrue == -1)
	    ret = -1;
	ورنہ
	    ret = سیٹ_option_قدر_for(کلید, istrue, تہی,
				    opt_flags, self->opt_type, self->from);
    }
    ورنہ اگر (flags & SOPT_عدد)
    {
	طویل	val;

	اگر (نمبر_مقصد_Long(valObject, &val, نمبر_INT))
	{
	    Py_XDECREF(todecref);
	    واپس -1;
	}

	ret = سیٹ_option_قدر_for(کلید, (صحیح) val, تہی, opt_flags,
				self->opt_type, self->from);
    }
    ورنہ
    {
	char_u		*val;
	PyObject	*todecref2;

	اگر ((val = String_مقصد_Chars(valObject, &todecref2)))
	{
	    ret = سیٹ_option_قدر_for(کلید, 0, val, opt_flags,
				    self->opt_type, self->from);
	    Py_XDECREF(todecref2);
	}
	ورنہ
	    ret = -1;
    }

    Py_XDECREF(todecref);

    واپس ret;
}

ساکن PySequenceMethods OptionsAsSeq = {
    0,					// sq_length
    0,					// sq_concat
    0,					// sq_repeat
    0,					// sq_item
    0,					// sq_slice
    0,					// sq_ass_item
    0,					// sq_ass_slice
    (objobjproc) Options_شامل_ہے,	// sq_contains
    0,					// sq_inplace_concat
    0,					// sq_inplace_repeat
};

ساکن PyMappingMethods OptionsAsMapping = {
    (lenfunc)       تہی,
    (binaryfunc)    Options_شے,
    (objobjargproc) Options_Ass_شے,
};

// Tabpage object

قسمتعریف ساخت
{
    Py_Object_سر
    tabpage_T	*tab;
} Tab_صفحہ_Object;

ساکن PyObject *WinListNew(Tab_صفحہ_Object *tabObject);

DEFINE_PY_TYPE_OBJECT(Tab_صفحہ_Type);

    ساکن صحیح
Check_Tab_صفحہ(Tab_صفحہ_Object *self)
{
    اگر (self->tab == INVALID_TABPAGE_قدر)
    {
	Py_Err_سیٹ_VIM(N_("attempt to refer to deleted tab page"));
	واپس -1;
    }

    واپس 0;
}

    ساکن PyObject *
Tab_صفحہ_New(tabpage_T *tab)
{
    Tab_صفحہ_Object *self;

    اگر (TAB_PYTHON_REF(tab))
    {
	self = TAB_PYTHON_REF(tab);
	Py_INCREF((PyObject *)self);
    }
    ورنہ
    {
	self = PyObject_NEW(Tab_صفحہ_Object, Tab_صفحہ_Type_Ptr);
	اگر (self == تہی)
	    واپس تہی;
	self->tab = tab;
	TAB_PYTHON_REF(tab) = self;
    }

    واپس (PyObject *)(self);
}

    ساکن خالی
Tab_صفحہ_Destructor(PyObject *self_obj)
{
    Tab_صفحہ_Object *self = (Tab_صفحہ_Object*)self_obj;
    اگر (self->tab && self->tab != INVALID_TABPAGE_قدر)
	TAB_PYTHON_REF(self->tab) = تہی;

    DESTRUCTOR_اختتام(self);
}

ساکن حرف *Tab_صفحہ_Attrs[] = {
    "windows", "number", "vars", "window", "valid",
    تہی
};

    ساکن PyObject *
Tab_صفحہ_Dir(PyObject *self, PyObject *دلائل غیر_مستعمل)
{
    واپس ObjectDir(self, Tab_صفحہ_Attrs);
}

    ساکن PyObject *
Tab_صفحہ_Attr_Valid(Tab_صفحہ_Object *self, حرف *name)
{
    PyObject	*ret;

    اگر (strcmp(name, "valid") != 0)
	واپس تہی;

    ret = ((self->tab == INVALID_TABPAGE_قدر) ? Py_False : Py_True);
    Py_INCREF(ret);
    واپس ret;
}

    ساکن PyObject *
Tab_صفحہ_Attr(Tab_صفحہ_Object *self, حرف *name)
{
    اگر (strcmp(name, "windows") == 0)
	واپس WinListNew(self);
    ورنہ اگر (strcmp(name, "number") == 0)
	واپس PyLong_FromLong((طویل) get_tab_نمبر(self->tab));
    ورنہ اگر (strcmp(name, "vars") == 0)
	واپس NEW_DICTIONARY(self->tab->tp_vars);
    ورنہ اگر (strcmp(name, "window") == 0)
    {
	// For current tab window.c does not bother to set or update tp_curwin
	اگر (self->tab == curtab)
	    واپس کھڑکی_New(curwin, curtab);
	ورنہ
	    واپس کھڑکی_New(self->tab->tp_curwin, self->tab);
    }
    ورنہ اگر (strcmp(name, "__members__") == 0)
	واپس ObjectDir(تہی, Tab_صفحہ_Attrs);
    واپس تہی;
}

    ساکن PyObject *
Tab_صفحہ_Repr(PyObject *self_obj)
{
    Tab_صفحہ_Object *self = (Tab_صفحہ_Object*)self_obj;
    اگر (self->tab == INVALID_TABPAGE_قدر)
	واپس PyString_FromFormat("<tabpage object (deleted) at %p>", (خالی *)self);
    ورنہ
    {
	صحیح	t = get_tab_نمبر(self->tab);

	اگر (t == 0)
	    واپس PyString_FromFormat("<tabpage object (unknown) at %p>",
					(خالی *)self);
	ورنہ
	    واپس PyString_FromFormat("<tabpage %d>", t - 1);
    }
}

ساکن ساخت Py_طریقہ_Def Tab_صفحہ_Methods[] = {
    // name,	    function,			calling,	documentation
    {"__dir__",	    (PyCFunction)Tab_صفحہ_Dir,	METH_NOARGS,	""},
    { تہی,	    تہی,			0,		تہی}
};

/*
 * Window list object
 */

DEFINE_PY_TYPE_OBJECT(TabListType);
ساکن PySequenceMethods TabListAsSeq;

قسمتعریف ساخت
{
    Py_Object_سر
} TabListObject;

ساکن TabListObject The_Tab_صفحہ_List =
{
    Py_Object_سر_INIT_TYPE(TabListType)
};

    ساکن PyInt
Tab_List_لمبائی(PyObject *self غیر_مستعمل)
{
    tabpage_T	*tp = پہلا_tabpage;
    PyInt	n = 0;

    جب (tp != تہی)
    {
	++n;
	tp = tp->tp_اگلا;
    }

    واپس n;
}

    ساکن PyObject *
Tab_List_شے(PyObject *self غیر_مستعمل, PyInt n)
{
    tabpage_T	*tp;

    برائے (tp = پہلا_tabpage; tp != تہی; tp = tp->tp_اگلا, --n)
	اگر (n == 0)
	    واپس Tab_صفحہ_New(tp);

    Py_Err_سیٹ_STRING(Py_Exc_اشاریہ_Error, N_("no such tab page"));
    واپس تہی;
}

/*
 * Window object
 */

قسمتعریف ساخت
{
    Py_Object_سر
    win_T	*win;
    Tab_صفحہ_Object	*tabObject;
} کھڑکی_Object;

DEFINE_PY_TYPE_OBJECT(کھڑکی_Type);

    ساکن صحیح
Check_کھڑکی(کھڑکی_Object *self)
{
    اگر (self->win == INVALID_کھڑکی_قدر)
    {
	Py_Err_سیٹ_VIM(N_("attempt to refer to deleted window"));
	واپس -1;
    }

    واپس 0;
}

    ساکن صحیح
Check_کھڑکی_Cb(خالی *self)
{
    واپس Check_کھڑکی((کھڑکی_Object*)self);
}

    ساکن PyObject *
کھڑکی_New(win_T *win, tabpage_T *tab)
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

    کھڑکی_Object *self;

    اگر (WIN_PYTHON_REF(win))
    {
	self = WIN_PYTHON_REF(win);
	Py_INCREF((PyObject *)self);
    }
    ورنہ
    {
	self = PyObject_GC_New(کھڑکی_Object, کھڑکی_Type_Ptr);
	اگر (self == تہی)
	    واپس تہی;
	self->win = win;
	WIN_PYTHON_REF(win) = self;
    }

    self->tabObject = ((Tab_صفحہ_Object *)(Tab_صفحہ_New(tab)));

    واپس (PyObject *)(self);
}

    ساکن خالی
کھڑکی_Destructor(PyObject *self_obj)
{
    کھڑکی_Object *self = (کھڑکی_Object*)self_obj;
    PyObject_GC_UnTrack((خالی *)(self));
    اگر (self->win && self->win != INVALID_کھڑکی_قدر)
	WIN_PYTHON_REF(self->win) = تہی;
    Py_XDECREF(((PyObject *)(self->tabObject)));
    PyObject_GC_Del((خالی *)(self));
}

    ساکن صحیح
کھڑکی_Traverse(PyObject *self_obj, visitproc دورہ, خالی *arg)
{
    کھڑکی_Object *self = (کھڑکی_Object*)self_obj;
    Py_دورہ(((PyObject *)(self->tabObject)));
    واپس 0;
}

    ساکن صحیح
کھڑکی_Clear(PyObject *self_obj)
{
    کھڑکی_Object *self = (کھڑکی_Object*)self_obj;
    Py_CLEAR(self->tabObject);
    واپس 0;
}

    ساکن win_T *
get_firstwin(Tab_صفحہ_Object *tabObject)
{
    اگر (tabObject)
    {
	اگر (Check_Tab_صفحہ(tabObject))
	    واپس تہی;
	// For current tab window.c does not bother to set or update tp_firstwin
	ورنہ اگر (tabObject->tab == curtab)
	    واپس firstwin;
	ورنہ
	    واپس tabObject->tab->tp_firstwin;
    }
    ورنہ
	واپس firstwin;
}

// Use the same order as in the WindowAttr() function.
ساکن حرف *کھڑکی_Attrs[] = {
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
    تہی
};

    ساکن PyObject *
کھڑکی_Dir(PyObject *self, PyObject *دلائل غیر_مستعمل)
{
    واپس ObjectDir(self, کھڑکی_Attrs);
}

    ساکن PyObject *
کھڑکی_Attr_Valid(کھڑکی_Object *self, حرف *name)
{
    PyObject	*ret;

    اگر (strcmp(name, "valid") != 0)
	واپس تہی;

    ret = ((self->win == INVALID_کھڑکی_قدر) ? Py_False : Py_True);
    Py_INCREF(ret);
    واپس ret;
}

    ساکن PyObject *
کھڑکی_Attr(کھڑکی_Object *self, حرف *name)
{
    اگر (strcmp(name, "buffer") == 0)
	واپس (PyObject *)بفر_New(self->win->w_بفر);
    ورنہ اگر (strcmp(name, "cursor") == 0)
    {
	pos_T *pos = &self->win->w_cursor;

	واپس Py_Build_قدر("(ll)", (طویل)(pos->lnum), (طویل)(pos->ستون));
    }
    ورنہ اگر (strcmp(name, "height") == 0)
	واپس PyLong_FromLong((طویل)(self->win->w_اونچائی));
    ورنہ اگر (strcmp(name, "row") == 0)
	واپس PyLong_FromLong((طویل)(self->win->w_winrow));
    ورنہ اگر (strcmp(name, "width") == 0)
	واپس PyLong_FromLong((طویل)(self->win->w_چوڑائی));
    ورنہ اگر (strcmp(name, "col") == 0)
	واپس PyLong_FromLong((طویل)(self->win->w_wincol));
    ورنہ اگر (strcmp(name, "vars") == 0)
	واپس NEW_DICTIONARY(self->win->w_vars);
    ورنہ اگر (strcmp(name, "options") == 0)
	واپس OptionsNew(SREQ_WIN, self->win, Check_کھڑکی_Cb,
			(PyObject *) self);
    ورنہ اگر (strcmp(name, "number") == 0)
    {
	اگر (Check_Tab_صفحہ(self->tabObject))
	    واپس تہی;
	واپس PyLong_FromLong((طویل)
		get_win_نمبر(self->win, get_firstwin(self->tabObject)));
    }
    ورنہ اگر (strcmp(name, "tabpage") == 0)
    {
	Py_INCREF((PyObject *)self->tabObject);
	واپس (PyObject *)(self->tabObject);
    }
    ورنہ اگر (strcmp(name, "__members__") == 0)
	واپس ObjectDir(تہی, کھڑکی_Attrs);
    ورنہ
	واپس تہی;
}

    ساکن صحیح
کھڑکی_Setattr(PyObject *self_obj, حرف *name, PyObject *valObject)
{
    کھڑکی_Object *self = (کھڑکی_Object*)self_obj;
    اگر (Check_کھڑکی(self))
	واپس -1;

    اگر (strcmp(name, "buffer") == 0)
    {
	Py_Err_سیٹ_STRING(PyExc_TypeError, N_("readonly attribute: buffer"));
	واپس -1;
    }
    ورنہ اگر (strcmp(name, "cursor") == 0)
    {
	طویل lnum;
	طویل ستون;

	اگر (!PyArg_Parse(valObject, "(ll)", &lnum, &ستون))
	    واپس -1;

	اگر (lnum <= 0 || lnum > self->win->w_بفر->b_ml.ml_سطر_گنتی)
	{
	    Py_Err_سیٹ_VIM(N_("cursor position outside buffer"));
	    واپس -1;
	}

	// Check for keyboard interrupts
	اگر (Vim_Check_مداخلت())
	    واپس -1;

	self->win->w_cursor.lnum = lnum;
	self->win->w_cursor.ستون = ستون;
	self->win->w_سیٹ_curswant = TRUE;
	self->win->w_cursor.coladd = 0;
	// When column is out of range silently correct it.
	check_cursor_ستون_win(self->win);

	update_screen(UPD_VALID);
	واپس 0;
    }
    ورنہ اگر (strcmp(name, "height") == 0)
    {
	طویل	اونچائی;
	win_T	*savewin;

	اگر (نمبر_مقصد_Long(valObject, &اونچائی, نمبر_INT|نمبر_UNSIGNED))
	    واپس -1;

#اگر_تعریف_شدہ FEAT_GUI
	need_mouse_correct = TRUE;
#اگر_ختم
	savewin = curwin;
	curwin = self->win;
	curbuf = curwin->w_بفر;

	Vim_Try_آغاز();
	win_setheight((صحیح) اونچائی);
	curwin = savewin;
	curbuf = curwin->w_بفر;
	اگر (Vim_Try_انتہا())
	    واپس -1;

	واپس 0;
    }
    ورنہ اگر (strcmp(name, "width") == 0)
    {
	طویل	چوڑائی;
	win_T	*savewin;

	اگر (نمبر_مقصد_Long(valObject, &چوڑائی, نمبر_INT|نمبر_UNSIGNED))
	    واپس -1;

#اگر_تعریف_شدہ FEAT_GUI
	need_mouse_correct = TRUE;
#اگر_ختم
	savewin = curwin;
	curwin = self->win;
	curbuf = curwin->w_بفر;

	Vim_Try_آغاز();
	win_setwidth((صحیح) چوڑائی);
	curwin = savewin;
	curbuf = curwin->w_بفر;
	اگر (Vim_Try_انتہا())
	    واپس -1;

	واپس 0;
    }
    ورنہ
    {
	Py_Err_سیٹ_String(PyExc_AttributeError, name);
	واپس -1;
    }
}

    ساکن PyObject *
کھڑکی_Repr(PyObject *self_obj)
{
    کھڑکی_Object *self = (کھڑکی_Object*)self_obj;
    اگر (self->win == INVALID_کھڑکی_قدر)
	واپس PyString_FromFormat("<window object (deleted) at %p>", (خالی *)self);
    ورنہ
    {
	صحیح	w = get_win_نمبر(self->win, firstwin);

	اگر (w == 0)
	    واپس PyString_FromFormat("<window object (unknown) at %p>",
								      (خالی *)self);
	ورنہ
	    واپس PyString_FromFormat("<window %d>", w - 1);
    }
}

ساکن ساخت Py_طریقہ_Def کھڑکی_Methods[] = {
    // name,	    function,			calling,	documentation
    {"__dir__",	    (PyCFunction)کھڑکی_Dir,	METH_NOARGS,	""},
    { تہی,	    تہی,			0,		تہی}
};

/*
 * Window list object
 */

DEFINE_PY_TYPE_OBJECT(WinListType);
ساکن PySequenceMethods WinListAsSeq;

قسمتعریف ساخت
{
    Py_Object_سر
    Tab_صفحہ_Object	*tabObject;
} WinListObject;

ساکن WinListObject The_کھڑکی_List =
{
    Py_Object_سر_INIT_TYPE(WinListType)
    تہی
};

    ساکن PyObject *
WinListNew(Tab_صفحہ_Object *tabObject)
{
    WinListObject	*self;

    self = PyObject_NEW(WinListObject, WinListTypePtr);
    self->tabObject = tabObject;
    Py_INCREF((PyObject *)tabObject);

    واپس (PyObject *)(self);
}

    ساکن خالی
WinListDestructor(WinListObject *self)
{
    Tab_صفحہ_Object	*tabObject = self->tabObject;

    اگر (tabObject)
    {
	Py_DECREF((PyObject *)(tabObject));
    }

    DESTRUCTOR_اختتام(self);
}

    ساکن PyInt
Win_List_لمبائی(WinListObject *self)
{
    win_T	*w;
    PyInt	n = 0;

    اگر (!(w = get_firstwin(self->tabObject)))
	واپس -1;

    جب (w != تہی)
    {
	++n;
	w = W_اگلا(w);
    }

    واپس n;
}

    ساکن PyObject *
Win_List_شے(WinListObject *self, PyInt n)
{
    win_T *w;

    اگر (!(w = get_firstwin(self->tabObject)))
	واپس تہی;

    برائے (; w != تہی; w = W_اگلا(w), --n)
	اگر (n == 0)
	    واپس کھڑکی_New(w, self->tabObject? self->tabObject->tab: curtab);

    Py_Err_سیٹ_STRING(Py_Exc_اشاریہ_Error, N_("no such window"));
    واپس تہی;
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
    ساکن حرف *
String_مقصد_سطر(PyObject *obj)
{
    حرف	*str;
    حرف	*save;
    PyObject	*بائٹیں = تہی;
    Py_ssize_t	len = 0;
    PyInt	i;
    حرف	*p;

    اگر (Py_بائٹیں_Check(obj))
    {
	اگر (Py_بائٹیں_As_String_And_سائز(obj, &str, &len) == -1
		|| str == تہی)
	    واپس تہی;
    }
    ورنہ اگر (PyUnicode_Check(obj))
    {
	اگر (!(بائٹیں = PyUnicode_AsEncodedString(obj, ENC_OPT,
							   ERRORS_ENCODE_ARG)))
	    واپس تہی;

	اگر (Py_بائٹیں_As_String_And_سائز(بائٹیں, &str, &len) == -1
		|| str == تہی)
	{
	    Py_DECREF(بائٹیں);
	    واپس تہی;
	}
    }
    ورنہ
    {
#اگر PY_MAJOR_نسخہ < 3
	PyErr_FORMAT_TYPE(
		N_("expected str() or unicode() instance, but got %s"),
		obj);
#ورنہ
	PyErr_FORMAT_TYPE(
		N_("expected bytes() or str() instance, but got %s"),
		obj);
#اگر_ختم
	واپس تہی;
    }

    /*
     * Error checking: String must not contain newlines, as we
     * are replacing a single line, and we must replace it with
     * a single line.
     * A trailing newline is removed, so that append(f.readlines()) works.
     */
    p = memchr(str, '\n', len);
    اگر (p != تہی)
    {
	اگر (p == str + len - 1)
	    --len;
	ورنہ
	{
	    Py_Err_سیٹ_VIM(N_("string cannot contain newlines"));
	    Py_XDECREF(بائٹیں);
	    واپس تہی;
	}
    }

    /*
     * Create a copy of the string, with internal nulls replaced by
     * newline characters, as is the Vim convention.
     */
    save = alloc(len+1);
    اگر (save == تہی)
    {
	Py_Err_No_حافظہ();
	Py_XDECREF(بائٹیں);
	واپس تہی;
    }

    برائے (i = 0; i < len; ++i)
    {
	اگر (str[i] == '\0')
	    save[i] = '\n';
	ورنہ
	    save[i] = str[i];
    }

    save[i] = '\0';
    Py_XDECREF(بائٹیں);  // Python 2 does nothing here

    واپس save;
}

/*
 * Get a line from the specified buffer. The line number is
 * in Vim format (1-based). The line is returned as a Python
 * string object.
 */
    ساکن PyObject *
Get_بفر_سطر(buf_T *buf, PyInt n)
{
    واپس سطر_مقصد_String((حرف *)ml_get_buf(buf, (linenr_T)n, FALSE));
}


/*
 * Get a list of lines from the specified buffer. The line numbers
 * are in Vim format (1-based). The range is from lo up to, but not
 * including, hi. The list is returned as a Python list of string objects.
 */
    ساکن PyObject *
Get_بفر_سطر_List(buf_T *buf, PyInt lo, PyInt hi)
{
    PyInt	i;
    PyInt	n = hi - lo;
    PyObject	*list = PyList_New(n);

    اگر (list == تہی)
	واپس تہی;

    برائے (i = 0; i < n; ++i)
    {
	linenr_T	lnum = (linenr_T)(lo + i);
	حرف		*متن;
	PyObject	*string;

	اگر (lnum > buf->b_ml.ml_سطر_گنتی)
	    متن = "";
	ورنہ
	    متن = (حرف *)ml_get_buf(buf, lnum, FALSE);
	string = سطر_مقصد_String(متن);
	اگر (string == تہی)
	{
	    Py_DECREF(list);
	    واپس تہی;
	}

	Py_List_سیٹ_شے(list, i, string);
    }

    // The ownership of the Python list is passed to the caller (ie,
    // the caller should Py_DECREF() the object when it is finished
    // with it).

    واپس list;
}

/*
 * Check if deleting lines made the cursor position invalid.
 * Changed the lines from "lo" to "hi" and added "extra" lines (negative if
 * deleted).
 */
    ساکن خالی
py_fix_cursor(linenr_T lo, linenr_T hi, linenr_T extra)
{
    اگر (curwin->w_cursor.lnum >= lo)
    {
	// Adjust the cursor position if it's in/after the changed
	// lines.
	اگر (curwin->w_cursor.lnum >= hi)
	{
	    curwin->w_cursor.lnum += extra;
	    check_cursor_ستون();
	}
	ورنہ اگر (extra < 0)
	{
	    curwin->w_cursor.lnum = lo;
	    check_cursor();
	}
	ورنہ
	    check_cursor_ستون();
	بدلا_cline_bef_curs();
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
    ساکن صحیح
سیٹ_بفر_سطر_2(buf_T *buf, PyInt n, PyObject *سطر, PyInt *len_change)
{
    bufref_T	save_curbuf = {تہی, 0, 0};
    switchwin_T	switchwin;

    // First of all, we check the type of the supplied Python object.
    // There are three cases:
    //	  1. NULL, or None - this is a deletion.
    //	  2. A string	   - this is a replacement.
    //	  3. Anything else - this is an error.
    اگر (سطر == Py_None || سطر == تہی)
    {
	PyErr_Clear();
	switchwin.sw_curwin = تہی;
	switch_مقصد_win_for_buf(buf, &switchwin, &save_curbuf);

	Vim_Try_آغاز();

	اگر (u_savedel((linenr_T)n, 1L) == FAIL)
	    RAISE_UNDO_FAIL;
	ورنہ اگر (ml_delete((linenr_T)n) == FAIL)
	    RAISE_DELETE_سطر_FAIL;
	ورنہ
	{
	    اگر (buf == curbuf && (switchwin.sw_curwin != تہی
					   || save_curbuf.br_buf == تہی))
		// Using an existing window for the buffer, adjust the cursor
		// position.
		py_fix_cursor((linenr_T)n, (linenr_T)n + 1, (linenr_T)-1);
	    اگر (save_curbuf.br_buf == تہی)
		// Only adjust marks if we managed to switch to a window that
		// holds the buffer, otherwise line numbers will be invalid.
		deleted_سطریں_mark((linenr_T)n, 1L);
	}

	restore_win_for_buf(&switchwin, &save_curbuf);

	اگر (Vim_Try_انتہا())
	    واپس FAIL;

	اگر (len_change)
	    *len_change = -1;

	واپس OK;
    }
    ورنہ اگر (Py_بائٹیں_Check(سطر) || PyUnicode_Check(سطر))
    {
	حرف	*save = String_مقصد_سطر(سطر);

	اگر (save == تہی)
	    واپس FAIL;

	Vim_Try_آغاز();

	// We do not need to free "save" if ml_replace() consumes it.
	PyErr_Clear();
	switch_مقصد_win_for_buf(buf, &switchwin, &save_curbuf);

	اگر (u_savesub((linenr_T)n) == FAIL)
	{
	    RAISE_UNDO_FAIL;
	    vim_free(save);
	}
	ورنہ اگر (ml_replace((linenr_T)n, (char_u *)save, FALSE) == FAIL)
	{
	    RAISE_REPLACE_سطر_FAIL;
	    vim_free(save);
	}
	ورنہ
	    بدلا_بائٹیں((linenr_T)n, 0);

	restore_win_for_buf(&switchwin, &save_curbuf);

	// Check that the cursor is not beyond the end of the line now.
	اگر (buf == curbuf)
	    check_cursor_ستون();

	اگر (Vim_Try_انتہا())
	    واپس FAIL;

	اگر (len_change)
	    *len_change = 0;

	واپس OK;
    }
    ورنہ
    {
	Py_Err_Bad_دلیل();
	واپس FAIL;
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
    ساکن صحیح
سیٹ_بفر_سطر_List(
	buf_T *buf,
	PyInt lo,
	PyInt hi,
	PyObject *list,
	PyInt *len_change)
{
    bufref_T	save_curbuf = {تہی, 0, 0};
    switchwin_T	switchwin;

    // First of all, we check the type of the supplied Python object.
    // There are three cases:
    //	  1. NULL, or None - this is a deletion.
    //	  2. A list	   - this is a replacement.
    //	  3. Anything else - this is an error.
    اگر (list == Py_None || list == تہی)
    {
	PyInt	i;
	PyInt	n = (صحیح)(hi - lo);

	PyErr_Clear();
	Vim_Try_آغاز();
	switchwin.sw_curwin = تہی;
	switch_مقصد_win_for_buf(buf, &switchwin, &save_curbuf);

	اگر (u_savedel((linenr_T)lo, (طویل)n) == FAIL)
	    RAISE_UNDO_FAIL;
	ورنہ
	{
	    برائے (i = 0; i < n; ++i)
	    {
		اگر (ml_delete((linenr_T)lo) == FAIL)
		{
		    RAISE_DELETE_سطر_FAIL;
		    توڑو;
		}
	    }
	    اگر (buf == curbuf && (switchwin.sw_curwin != تہی
					       || save_curbuf.br_buf == تہی))
		// Using an existing window for the buffer, adjust the cursor
		// position.
		py_fix_cursor((linenr_T)lo, (linenr_T)hi, (linenr_T)-n);
	    اگر (save_curbuf.br_buf == تہی)
		// Only adjust marks if we managed to switch to a window that
		// holds the buffer, otherwise line numbers will be invalid.
		deleted_سطریں_mark((linenr_T)lo, (طویل)i);
	}

	restore_win_for_buf(&switchwin, &save_curbuf);

	اگر (Vim_Try_انتہا())
	    واپس FAIL;

	اگر (len_change)
	    *len_change = -n;

	واپس OK;
    }
    ورنہ اگر (PyList_Check(list))
    {
	PyInt	i;
	PyInt	new_len = Py_List_سائز(list);
	PyInt	old_len = hi - lo;
	PyInt	extra = 0;	// lines added to text, can be negative
	حرف	**array;

	اگر (new_len == 0)	// avoid allocating zero bytes
	    array = تہی;
	ورنہ
	{
	    array = PyMem_New(حرف *, new_len);
	    اگر (array == تہی)
	    {
		Py_Err_No_حافظہ();
		واپس FAIL;
	    }
	}

	برائے (i = 0; i < new_len; ++i)
	{
	    PyObject *سطر;

	    اگر (!(سطر = Py_List_Get_شے(list, i)) ||
		!(array[i] = String_مقصد_سطر(سطر)))
	    {
		جب (i)
		    vim_free(array[--i]);
		PyMem_Free(array);
		واپس FAIL;
	    }
	}

	Vim_Try_آغاز();
	PyErr_Clear();

	// START of region without "return".  Must call restore_buffer()!
	switchwin.sw_curwin = تہی;
	switch_مقصد_win_for_buf(buf, &switchwin, &save_curbuf);

	اگر (u_save((linenr_T)(lo-1), (linenr_T)hi) == FAIL)
	    RAISE_UNDO_FAIL;

	// If the size of the range is reducing (ie, new_len < old_len) we
	// need to delete some old_len. We do this at the start, by
	// repeatedly deleting line "lo".
	اگر (!PyErr_Occurred())
	{
	    برائے (i = 0; i < old_len - new_len; ++i)
		اگر (ml_delete((linenr_T)lo) == FAIL)
		{
		    RAISE_DELETE_سطر_FAIL;
		    توڑو;
		}
	    extra -= i;
	}

	// For as long as possible, replace the existing old_len with the
	// new old_len. This is a more efficient operation, as it requires
	// less memory allocation and freeing.
	اگر (!PyErr_Occurred())
	{
	    برائے (i = 0; i < old_len && i < new_len; ++i)
		اگر (ml_replace((linenr_T)(lo+i), (char_u *)array[i], FALSE)
								      == FAIL)
		{
		    RAISE_REPLACE_سطر_FAIL;
		    توڑو;
		}
	}
	ورنہ
	    i = 0;

	// Now we may need to insert the remaining new old_len. If we do, we
	// must free the strings as we finish with them (we can't pass the
	// responsibility to Vim in this case).
	اگر (!PyErr_Occurred())
	{
	    جب (i < new_len)
	    {
		اگر (ml_append((linenr_T)(lo + i - 1),
					(char_u *)array[i], 0, FALSE) == FAIL)
		{
		    RAISE_داخل_کرو_سطر_FAIL;
		    توڑو;
		}
		vim_free(array[i]);
		++i;
		++extra;
	    }
	}

	// Free any left-over old_len, as a result of an error
	جب (i < new_len)
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
	اگر (save_curbuf.br_buf == تہی)
	{
	    mark_adjust((linenr_T)lo, (linenr_T)(hi - 1),
						  (طویل)MAXLNUM, (طویل)extra);
	    بدلا_سطریں((linenr_T)lo, 0, (linenr_T)hi, (طویل)extra);
	}

	اگر (buf == curbuf && (switchwin.sw_curwin != تہی
					   || save_curbuf.br_buf == تہی))
	    // Using an existing window for the buffer, adjust the cursor
	    // position.
	    py_fix_cursor((linenr_T)lo, (linenr_T)hi, (linenr_T)extra);

	// END of region without "return".
	restore_win_for_buf(&switchwin, &save_curbuf);

	اگر (Vim_Try_انتہا())
	    واپس FAIL;

	اگر (len_change)
	    *len_change = new_len - old_len;

	واپس OK;
    }
    ورنہ
    {
	Py_Err_Bad_دلیل();
	واپس FAIL;
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
    ساکن صحیح
داخل_کرو_بفر_سطریں(buf_T *buf, PyInt n, PyObject *سطریں, PyInt *len_change)
{
    bufref_T	save_curbuf = {تہی, 0, 0};
    switchwin_T	switchwin;

    // First of all, we check the type of the supplied Python object.
    // It must be a string or a list, or the call is in error.
    اگر (Py_بائٹیں_Check(سطریں) || PyUnicode_Check(سطریں))
    {
	حرف		*str = String_مقصد_سطر(سطریں);

	اگر (str == تہی)
	    واپس FAIL;

	PyErr_Clear();
	Vim_Try_آغاز();
	switch_مقصد_win_for_buf(buf, &switchwin, &save_curbuf);

	اگر (u_save((linenr_T)n, (linenr_T)(n + 1)) == FAIL)
	    RAISE_UNDO_FAIL;
	ورنہ اگر (ml_append((linenr_T)n, (char_u *)str, 0, FALSE) == FAIL)
	    RAISE_داخل_کرو_سطر_FAIL;
	ورنہ اگر (save_curbuf.br_buf == تہی)
	    // Only adjust marks if we managed to switch to a window that
	    // holds the buffer, otherwise line numbers will be invalid.
	    appended_سطریں_mark((linenr_T)n, 1L);

	vim_free(str);
	restore_win_for_buf(&switchwin, &save_curbuf);
	update_screen(UPD_VALID);

	اگر (Vim_Try_انتہا())
	    واپس FAIL;

	اگر (len_change)
	    *len_change = 1;

	واپس OK;
    }
    ورنہ اگر (PyList_Check(سطریں))
    {
	PyInt	i;
	PyInt	سائز = Py_List_سائز(سطریں);
	حرف	**array;

	array = PyMem_New(حرف *, سائز);
	اگر (array == تہی)
	{
	    Py_Err_No_حافظہ();
	    واپس FAIL;
	}

	برائے (i = 0; i < سائز; ++i)
	{
	    PyObject *سطر;

	    اگر (!(سطر = Py_List_Get_شے(سطریں, i)) ||
		!(array[i] = String_مقصد_سطر(سطر)))
	    {
		جب (i)
		    vim_free(array[--i]);
		PyMem_Free(array);
		واپس FAIL;
	    }
	}

	PyErr_Clear();
	Vim_Try_آغاز();
	switch_مقصد_win_for_buf(buf, &switchwin, &save_curbuf);

	اگر (u_save((linenr_T)n, (linenr_T)(n + 1)) == FAIL)
	    RAISE_UNDO_FAIL;
	ورنہ
	{
	    برائے (i = 0; i < سائز; ++i)
	    {
		اگر (ml_append((linenr_T)(n + i),
					(char_u *)array[i], 0, FALSE) == FAIL)
		{
		    RAISE_داخل_کرو_سطر_FAIL;

		    // Free the rest of the lines
		    جب (i < سائز)
			vim_free(array[i++]);

		    توڑو;
		}
		vim_free(array[i]);
	    }
	    اگر (i > 0 && save_curbuf.br_buf == تہی)
		// Only adjust marks if we managed to switch to a window that
		// holds the buffer, otherwise line numbers will be invalid.
		appended_سطریں_mark((linenr_T)n, (طویل)i);
	}

	// Free the array of lines. All of its contents have now
	// been freed.
	PyMem_Free(array);
	restore_win_for_buf(&switchwin, &save_curbuf);

	update_screen(UPD_VALID);

	اگر (Vim_Try_انتہا())
	    واپس FAIL;

	اگر (len_change)
	    *len_change = سائز;

	واپس OK;
    }
    ورنہ
    {
	Py_Err_Bad_دلیل();
	واپس FAIL;
    }
}

/*
 * Common routines for buffers and line ranges
 * -------------------------------------------
 */

قسمتعریف ساخت
{
    Py_Object_سر
    buf_T *buf;
} بفر_Object;

    ساکن صحیح
Check_بفر(بفر_Object *self)
{
    اگر (self->buf == INVALID_بفر_قدر)
    {
	Py_Err_سیٹ_VIM(N_("attempt to refer to deleted buffer"));
	واپس -1;
    }

    واپس 0;
}

    ساکن صحیح
Check_بفر_Cb(خالی *self)
{
    واپس Check_بفر((بفر_Object*)self);
}

    ساکن PyObject *
RBItem(بفر_Object *self, PyInt n, PyInt آغاز, PyInt انتہا)
{
    اگر (Check_بفر(self))
	واپس تہی;

    اگر (انتہا == -1)
	انتہا = self->buf->b_ml.ml_سطر_گنتی;

    اگر (n < 0)
	n += انتہا - آغاز + 1;

    اگر (n < 0 || n > انتہا - آغاز)
    {
	Py_Err_سیٹ_STRING(Py_Exc_اشاریہ_Error, N_("line number out of range"));
	واپس تہی;
    }

    واپس Get_بفر_سطر(self->buf, n+آغاز);
}

    ساکن PyObject *
RBSlice(بفر_Object *self, PyInt lo, PyInt hi, PyInt آغاز, PyInt انتہا)
{
    PyInt سائز;

    اگر (Check_بفر(self))
	واپس تہی;

    اگر (انتہا == -1)
	انتہا = self->buf->b_ml.ml_سطر_گنتی;

    سائز = انتہا - آغاز + 1;

    اگر (lo < 0)
	lo = 0;
    ورنہ اگر (lo > سائز)
	lo = سائز;
    اگر (hi < 0)
	hi = 0;
    اگر (hi < lo)
	hi = lo;
    ورنہ اگر (hi > سائز)
	hi = سائز;

    واپس Get_بفر_سطر_List(self->buf, lo+آغاز, hi+آغاز);
}

    ساکن PyInt
RBAs_شے(
	بفر_Object *self,
	PyInt n,
	PyObject *valObject,
	PyInt آغاز,
	PyInt انتہا,
	PyInt *new_انتہا)
{
    PyInt len_change;

    اگر (Check_بفر(self))
	واپس -1;

    اگر (انتہا == -1)
	انتہا = self->buf->b_ml.ml_سطر_گنتی;

    اگر (n < 0)
	n += انتہا - آغاز + 1;

    اگر (n < 0 || n > انتہا - آغاز)
    {
	Py_Err_سیٹ_STRING(Py_Exc_اشاریہ_Error, N_("line number out of range"));
	واپس -1;
    }

    اگر (سیٹ_بفر_سطر_2(self->buf, n+آغاز, valObject, &len_change) == FAIL)
	واپس -1;

    اگر (new_انتہا)
	*new_انتہا = انتہا + len_change;

    واپس 0;
}

    ساکن PyInt
RBAs_قاش(
	بفر_Object *self,
	PyInt lo,
	PyInt hi,
	PyObject *valObject,
	PyInt آغاز,
	PyInt انتہا,
	PyInt *new_انتہا)
{
    PyInt سائز;
    PyInt len_change;

    // Self must be a valid buffer
    اگر (Check_بفر(self))
	واپس -1;

    اگر (انتہا == -1)
	انتہا = self->buf->b_ml.ml_سطر_گنتی;

    // Sort out the slice range
    سائز = انتہا - آغاز + 1;

    اگر (lo < 0)
	lo = 0;
    ورنہ اگر (lo > سائز)
	lo = سائز;
    اگر (hi < 0)
	hi = 0;
    اگر (hi < lo)
	hi = lo;
    ورنہ اگر (hi > سائز)
	hi = سائز;

    اگر (سیٹ_بفر_سطر_List(self->buf, lo + آغاز, hi + آغاز,
						valObject, &len_change) == FAIL)
	واپس -1;

    اگر (new_انتہا)
	*new_انتہا = انتہا + len_change;

    واپس 0;
}


    ساکن PyObject *
RBAppend(
	بفر_Object *self,
	PyObject *دلائل,
	PyInt آغاز,
	PyInt انتہا,
	PyInt *new_انتہا)
{
    PyObject *سطریں;
    PyInt len_change;
    PyInt max;
    PyInt n;

    اگر (Check_بفر(self))
	واپس تہی;

    اگر (انتہا == -1)
	انتہا = self->buf->b_ml.ml_سطر_گنتی;

    max = n = انتہا - آغاز + 1;

    اگر (!PyArg_ParseTuple(دلائل, "O|n", &سطریں, &n))
	واپس تہی;

    اگر (n < 0 || n > max)
    {
	Py_Err_سیٹ_STRING(Py_Exc_اشاریہ_Error, N_("line number out of range"));
	واپس تہی;
    }

    اگر (داخل_کرو_بفر_سطریں(self->buf, n + آغاز - 1, سطریں, &len_change) == FAIL)
	واپس تہی;

    اگر (new_انتہا)
	*new_انتہا = انتہا + len_change;

    Py_INCREF(Py_None);
    واپس Py_None;
}

// Range object

DEFINE_PY_TYPE_OBJECT(RangeType);
ساکن PySequenceMethods RangeAsSeq;
ساکن PyMappingMethods RangeAsMapping;

قسمتعریف ساخت
{
    Py_Object_سر
    بفر_Object *buf;
    PyInt آغاز;
    PyInt انتہا;
} RangeObject;

    ساکن PyObject *
RangeNew(buf_T *buf, PyInt آغاز, PyInt انتہا)
{
    بفر_Object *bufr;
    RangeObject *self;
    self = PyObject_GC_New(RangeObject, RangeTypePtr);
    اگر (self == تہی)
	واپس تہی;

    bufr = (بفر_Object *)بفر_New(buf);
    اگر (bufr == تہی)
    {
	Py_DECREF(self);
	واپس تہی;
    }
    Py_INCREF((PyObject *)bufr);

    self->buf = bufr;
    self->آغاز = آغاز;
    self->انتہا = انتہا;

    واپس (PyObject *)(self);
}

    ساکن خالی
RangeDestructor(PyObject *self_obj)
{
    RangeObject *self = (RangeObject*)self_obj;
    PyObject_GC_UnTrack((خالی *)(self));
    Py_XDECREF(((PyObject *)(self->buf)));
    PyObject_GC_Del((خالی *)(self));
}

    ساکن صحیح
RangeTraverse(PyObject *self_obj, visitproc دورہ, خالی *arg)
{
    RangeObject *self = (RangeObject*)self_obj;
    Py_دورہ(((PyObject *)(self->buf)));
    واپس 0;
}

    ساکن صحیح
RangeClear(PyObject *self_obj)
{
    RangeObject *self = (RangeObject*)self_obj;
    Py_CLEAR(self->buf);
    واپس 0;
}

    ساکن PyInt
Range_لمبائی(RangeObject *self)
{
    // HOW DO WE SIGNAL AN ERROR FROM THIS FUNCTION?
    اگر (Check_بفر(self->buf))
	واپس -1; // ???

    واپس (self->انتہا - self->آغاز + 1);
}

    ساکن PyObject *
Range_شے(RangeObject *self, PyInt n)
{
    واپس RBItem(self->buf, n, self->آغاز, self->انتہا);
}

    ساکن PyObject *
Range_قاش(RangeObject *self, PyInt lo, PyInt hi)
{
    واپس RBSlice(self->buf, lo, hi, self->آغاز, self->انتہا);
}

ساکن حرف *RangeAttrs[] = {
    "start", "end",
    تہی
};

    ساکن PyObject *
RangeDir(PyObject *self, PyObject *دلائل غیر_مستعمل)
{
    واپس ObjectDir(self, RangeAttrs);
}

    ساکن PyObject *
RangeAppend(RangeObject *self, PyObject *دلائل)
{
    واپس RBAppend(self->buf, دلائل, self->آغاز, self->انتہا, &self->انتہا);
}

    ساکن PyObject *
RangeRepr(PyObject *self_obj)
{
    RangeObject *self = (RangeObject*)self_obj;
    اگر (self->buf->buf == INVALID_بفر_قدر)
	واپس PyString_FromFormat("<range object (for deleted buffer) at %p>",
				    (خالی *)self);
    ورنہ
    {
	حرف *name = (حرف *)self->buf->buf->b_fname;

	اگر (name == تہی)
	    name = "";

	واپس PyString_FromFormat("<range %s (%d:%d)>",
				    name, (صحیح)self->آغاز, (صحیح)self->انتہا);
    }
}

ساکن ساخت Py_طریقہ_Def RangeMethods[] = {
    // name,	function,			calling,	documentation
    {"append",	(PyCFunction)RangeAppend,	METH_VARARGS,	"Append data to the Vim range" },
    {"__dir__",	(PyCFunction)RangeDir,		METH_NOARGS,	""},
    { تہی,	تہی,				0,		تہی}
};

DEFINE_PY_TYPE_OBJECT(بفر_Type);
ساکن PySequenceMethods بفر_As_Seq;
ساکن PyMappingMethods بفر_As_Mapping;

    ساکن PyObject *
بفر_New(buf_T *buf)
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

    بفر_Object *self;

    اگر (BUF_PYTHON_REF(buf) != تہی)
    {
	self = BUF_PYTHON_REF(buf);
	Py_INCREF((PyObject *)self);
    }
    ورنہ
    {
	self = PyObject_NEW(بفر_Object, بفر_Type_Ptr);
	اگر (self == تہی)
	    واپس تہی;
	self->buf = buf;
	BUF_PYTHON_REF(buf) = self;
    }

    واپس (PyObject *)(self);
}

    ساکن خالی
بفر_Destructor(PyObject *self_obj)
{
    بفر_Object *self = (بفر_Object*)self_obj;
    اگر (self->buf && self->buf != INVALID_بفر_قدر)
	BUF_PYTHON_REF(self->buf) = تہی;

    DESTRUCTOR_اختتام(self);
}

    ساکن PyInt
بفر_لمبائی(بفر_Object *self)
{
    // HOW DO WE SIGNAL AN ERROR FROM THIS FUNCTION?
    اگر (Check_بفر(self))
	واپس -1; // ???

    واپس (PyInt)(self->buf->b_ml.ml_سطر_گنتی);
}

    ساکن PyObject *
بفر_شے(بفر_Object *self, PyInt n)
{
    واپس RBItem(self, n, 1, -1);
}

    ساکن PyObject *
بفر_قاش(بفر_Object *self, PyInt lo, PyInt hi)
{
    واپس RBSlice(self, lo, hi, 1, -1);
}

ساکن حرف *بفر_Attrs[] = {
    "name", "number", "vars", "options", "valid",
    تہی
};

    ساکن PyObject *
بفر_Dir(PyObject *self, PyObject *دلائل غیر_مستعمل)
{
    واپس ObjectDir(self, بفر_Attrs);
}

    ساکن PyObject *
بفر_Attr_Valid(بفر_Object *self, حرف *name)
{
    PyObject	*ret;

    اگر (strcmp(name, "valid") != 0)
	واپس تہی;

    ret = ((self->buf == INVALID_بفر_قدر) ? Py_False : Py_True);
    Py_INCREF(ret);
    واپس ret;
}

    ساکن PyObject *
بفر_Attr(بفر_Object *self, حرف *name)
{
    اگر (strcmp(name, "name") == 0)
	واپس PyString_FromString((self->buf->b_ffname == تہی
				    ? "" : (حرف *)self->buf->b_ffname));
    ورنہ اگر (strcmp(name, "number") == 0)
	واپس Py_Build_قدر(Py_ssize_t_fmt, self->buf->b_fnum);
    ورنہ اگر (strcmp(name, "vars") == 0)
	واپس NEW_DICTIONARY(self->buf->b_vars);
    ورنہ اگر (strcmp(name, "options") == 0)
	واپس OptionsNew(SREQ_BUF, self->buf, Check_بفر_Cb,
			(PyObject *) self);
    ورنہ اگر (strcmp(name, "__members__") == 0)
	واپس ObjectDir(تہی, بفر_Attrs);
    ورنہ
	واپس تہی;
}

    ساکن صحیح
بفر_Setattr(PyObject *self_obj, حرف *name, PyObject *valObject)
{
    بفر_Object *self = (بفر_Object*)self_obj;
    اگر (Check_بفر(self))
	واپس -1;

    اگر (strcmp(name, "name") == 0)
    {
	char_u		*val;
	aco_save_T	aco;
	صحیح		ren_ret = OK;
	PyObject	*todecref;

	اگر (!(val = String_مقصد_Chars(valObject, &todecref)))
	    واپس -1;

	Vim_Try_آغاز();
	// Using aucmd_*: autocommands will be executed by rename_buffer
	aucmd_prepbuf(&aco, self->buf);
	اگر (curbuf == self->buf)
	{
	    ren_ret = rename_بفر(val);
	    aucmd_restbuf(&aco);
	}
	Py_XDECREF(todecref);
	اگر (Vim_Try_انتہا())
	    واپس -1;

	اگر (ren_ret == FAIL)
	{
	    Py_Err_سیٹ_VIM(N_("failed to rename buffer"));
	    واپس -1;
	}
	واپس 0;
    }
    ورنہ
    {
	Py_Err_سیٹ_String(PyExc_AttributeError, name);
	واپس -1;
    }
}

    ساکن PyObject *
بفر_Append(بفر_Object *self, PyObject *دلائل)
{
    واپس RBAppend(self, دلائل, 1, -1, تہی);
}

    ساکن PyObject *
بفر_Mark(بفر_Object *self, PyObject *pmarkObject)
{
    pos_T	*posp;
    char_u	*pmark;
    char_u	mark;
    bufref_T	savebuf;
    PyObject	*todecref;

    اگر (Check_بفر(self))
	واپس تہی;

    اگر (!(pmark = String_مقصد_Chars(pmarkObject, &todecref)))
	واپس تہی;

    اگر (pmark[0] == '\0' || pmark[1] != '\0')
    {
	Py_Err_سیٹ_STRING(Py_Exc_قدر_Error,
		N_("mark name must be a single character"));
	Py_XDECREF(todecref);
	واپس تہی;
    }

    mark = *pmark;

    Py_XDECREF(todecref);

    Vim_Try_آغاز();
    switch_بفر(&savebuf, self->buf);
    posp = getmark(mark, FALSE);
    restore_بفر(&savebuf);
    اگر (Vim_Try_انتہا())
	واپس تہی;

    اگر (posp == تہی)
    {
	Py_Err_سیٹ_VIM(N_("invalid mark name"));
	واپس تہی;
    }

    اگر (posp->lnum <= 0)
    {
	// Or raise an error?
	Py_INCREF(Py_None);
	واپس Py_None;
    }

    واپس Py_Build_قدر("(ll)", (طویل)(posp->lnum), (طویل)(posp->ستون));
}

    ساکن PyObject *
بفر_Range(بفر_Object *self, PyObject *دلائل)
{
    PyInt آغاز;
    PyInt انتہا;

    اگر (Check_بفر(self))
	واپس تہی;

    اگر (!PyArg_ParseTuple(دلائل, "nn", &آغاز, &انتہا))
	واپس تہی;

    واپس RangeNew(self->buf, آغاز, انتہا);
}

    ساکن PyObject *
بفر_Repr(PyObject *self_obj)
{
    بفر_Object *self = (بفر_Object*)self_obj;
    اگر (self->buf == INVALID_بفر_قدر)
	واپس PyString_FromFormat("<buffer object (deleted) at %p>", (خالی *)self);
    ورنہ
    {
	حرف	*name = (حرف *)self->buf->b_fname;

	اگر (name == تہی)
	    name = "";

	واپس PyString_FromFormat("<buffer %s>", name);
    }
}

ساکن ساخت Py_طریقہ_Def بفر_Methods[] = {
    // name,	    function,			calling,	documentation
    {"append",	    (PyCFunction)بفر_Append,	METH_VARARGS,	"Append data to Vim buffer" },
    {"mark",	    (PyCFunction)بفر_Mark,	METH_O,		"Return (row,col) representing position of named mark" },
    {"range",	    (PyCFunction)بفر_Range,	METH_VARARGS,	"Return a range object which represents the part of the given buffer between line numbers s and e" },
    {"__dir__",	    (PyCFunction)بفر_Dir,	METH_NOARGS,	""},
    { تہی,	    تہی,			0,		تہی}
};

/*
 * Buffer list object - Implementation
 */

DEFINE_PY_TYPE_OBJECT(BufMapType);

قسمتعریف ساخت
{
    Py_Object_سر
} BufMapObject;

ساکن BufMapObject The_بفر_Map =
{
    Py_Object_سر_INIT_TYPE(BufMapType)
};

    ساکن PyInt
Buf_Map_لمبائی(PyObject *self غیر_مستعمل)
{
    buf_T	*b = firstbuf;
    PyInt	n = 0;

    جب (b)
    {
	++n;
	b = b->b_اگلا;
    }

    واپس n;
}

    ساکن PyObject *
Buf_Map_شے(PyObject *self غیر_مستعمل, PyObject *کلید_Object)
{
    buf_T	*b;
    طویل	bnr;

    اگر (نمبر_مقصد_Long(کلید_Object, &bnr, نمبر_INT|نمبر_NATURAL))
	واپس تہی;

    b = buflist_findnr((صحیح) bnr);

    اگر (b)
	واپس بفر_New(b);
    ورنہ
    {
	Py_Err_سیٹ_Object(Py_Exc_کلید_Error, کلید_Object);
	واپس تہی;
    }
}

    ساکن خالی
BufMapIterDestruct(خالی* arg)
{
    PyObject *بفر = (PyObject*)arg;
    // Iteration was stopped before all buffers were processed
    اگر (بفر)
    {
	Py_DECREF(بفر);
    }
}

    ساکن صحیح
BufMapIterTraverse(خالی *iter, visitproc دورہ, خالی *arg)
{
    PyObject *بفر = (PyObject*)iter;
    اگر (بفر)
	Py_دورہ(بفر);
    واپس 0;
}

    ساکن صحیح
BufMapIterClear(خالی **iter)
{
    PyObject **بفر = (PyObject**)iter;
    اگر (*بفر)
	Py_CLEAR(*بفر);
    واپس 0;
}

    ساکن PyObject *
Buf_Map_Iter_اگلا(خالی **arg)
{
    PyObject	*اگلا;
    PyObject	*ret;
    PyObject	**بفر = (PyObject**)arg;

    اگر (!*بفر)
	واپس تہی;

    ret = *بفر;

    اگر (Check_بفر((بفر_Object *)(ret)))
    {
	*بفر = تہی;
	واپس تہی;
    }

    اگر (!((بفر_Object *)(ret))->buf->b_اگلا)
	اگلا = تہی;
    ورنہ اگر (!(اگلا = بفر_New(((بفر_Object *)(ret))->buf->b_اگلا)))
	واپس تہی;
    *بفر = اگلا;
    // Do not increment reference: we no longer hold it (decref), but whoever
    // on other side will hold (incref). Decref+incref = nothing.
    واپس ret;
}

    ساکن PyObject *
BufMapIter(PyObject *self)
{
    PyObject *بفر;

    بفر = بفر_New(firstbuf);
    واپس IterNew(بفر,
	    BufMapIterDestruct, Buf_Map_Iter_اگلا,
	    BufMapIterTraverse, BufMapIterClear,
	    self);
}

ساکن PyMappingMethods BufMapAsMapping = {
    (lenfunc)       Buf_Map_لمبائی,
    (binaryfunc)    Buf_Map_شے,
    (objobjargproc) 0,
};

// Current items object

ساکن حرف *CurrentAttrs[] = {
    "buffer", "window", "line", "range", "tabpage",
    تہی
};

    ساکن PyObject *
CurrentDir(PyObject *self, PyObject *دلائل غیر_مستعمل)
{
    واپس ObjectDir(self, CurrentAttrs);
}

    ساکن PyObject *
CurrentGetattr(PyObject *self غیر_مستعمل, حرف *name)
{
    اگر (strcmp(name, "buffer") == 0)
	واپس (PyObject *)بفر_New(curbuf);
    ورنہ اگر (strcmp(name, "window") == 0)
	واپس (PyObject *)کھڑکی_New(curwin, curtab);
    ورنہ اگر (strcmp(name, "tabpage") == 0)
	واپس (PyObject *)Tab_صفحہ_New(curtab);
    ورنہ اگر (strcmp(name, "line") == 0)
	واپس Get_بفر_سطر(curbuf, (PyInt)curwin->w_cursor.lnum);
    ورنہ اگر (strcmp(name, "range") == 0)
	واپس RangeNew(curbuf, Range_آغاز, Range_انتہا);
    ورنہ اگر (strcmp(name, "__members__") == 0)
	واپس ObjectDir(تہی, CurrentAttrs);
    ورنہ
#اگر PY_MAJOR_نسخہ < 3
	واپس Py_ڈھونڈو_طریقہ(کھڑکی_Methods, self, name);
#ورنہ
	واپس تہی;
#اگر_ختم
}

    ساکن صحیح
CurrentSetattr(PyObject *self غیر_مستعمل, حرف *name, PyObject *valObject)
{
    اگر (strcmp(name, "line") == 0)
    {
	اگر (سیٹ_بفر_سطر_2(curbuf, (PyInt)curwin->w_cursor.lnum, valObject,
			  تہی) == FAIL)
	    واپس -1;

	واپس 0;
    }
    ورنہ اگر (strcmp(name, "buffer") == 0)
    {
	صحیح گنتی;

	اگر (valObject->ob_type != بفر_Type_Ptr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.Buffer object, but got %s"),
		    valObject);
	    واپس -1;
	}

	اگر (Check_بفر((بفر_Object *)(valObject)))
	    واپس -1;
	گنتی = ((بفر_Object *)(valObject))->buf->b_fnum;

	Vim_Try_آغاز();
	اگر (do_بفر(DOBUF_GOTO, DOBUF_پہلا, FORWARD, گنتی, 0) == FAIL)
	{
	    اگر (Vim_Try_انتہا())
		واپس -1;
	    PyErr_VIM_FORMAT(N_("failed to switch to buffer %d"), گنتی);
	    واپس -1;
	}

	واپس Vim_Try_انتہا();
    }
    ورنہ اگر (strcmp(name, "window") == 0)
    {
	صحیح گنتی;

	اگر (valObject->ob_type != کھڑکی_Type_Ptr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.Window object, but got %s"),
		    valObject);
	    واپس -1;
	}

	اگر (Check_کھڑکی((کھڑکی_Object *)(valObject)))
	    واپس -1;
	گنتی = get_win_نمبر(((کھڑکی_Object *)(valObject))->win, firstwin);

	اگر (!گنتی)
	{
	    Py_Err_سیٹ_STRING(Py_Exc_قدر_Error,
		    N_("failed to find window in the current tab page"));
	    واپس -1;
	}

	Vim_Try_آغاز();
	win_goto(((کھڑکی_Object *)(valObject))->win);
	اگر (((کھڑکی_Object *)(valObject))->win != curwin)
	{
	    اگر (Vim_Try_انتہا())
		واپس -1;
	    Py_Err_سیٹ_STRING(PyExc_RuntimeError,
		    N_("did not switch to the specified window"));
	    واپس -1;
	}

	واپس Vim_Try_انتہا();
    }
    ورنہ اگر (strcmp(name, "tabpage") == 0)
    {
	اگر (valObject->ob_type != Tab_صفحہ_Type_Ptr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.TabPage object, but got %s"),
		    valObject);
	    واپس -1;
	}

	اگر (Check_Tab_صفحہ((Tab_صفحہ_Object *)(valObject)))
	    واپس -1;

	Vim_Try_آغاز();
	goto_tabpage_tp(((Tab_صفحہ_Object *)(valObject))->tab, TRUE, TRUE);
	اگر (((Tab_صفحہ_Object *)(valObject))->tab != curtab)
	{
	    اگر (Vim_Try_انتہا())
		واپس -1;
	    Py_Err_سیٹ_STRING(PyExc_RuntimeError,
		    N_("did not switch to the specified tab page"));
	    واپس -1;
	}

	واپس Vim_Try_انتہا();
    }
    ورنہ
    {
	Py_Err_سیٹ_String(PyExc_AttributeError, name);
	واپس -1;
    }
}

ساکن ساخت Py_طریقہ_Def CurrentMethods[] = {
    // name,	    function,			calling,	documentation
    {"__dir__",	    (PyCFunction)CurrentDir,	METH_NOARGS,	""},
    { تہی,	    تہی,			0,		تہی}
};

    ساکن خالی
init_range_cmd(خالی *arg)
{
    exarg_T *eap = (exarg_T*)arg;
    Range_آغاز = eap->line1;
    Range_انتہا = eap->line2;
}

    ساکن خالی
init_range_eval(خالی *rettv غیر_مستعمل)
{
    Range_آغاز = (PyInt) curwin->w_cursor.lnum;
    Range_انتہا = Range_آغاز;
}

    ساکن خالی
تسلسل_cmd(ثابت حرف *cmd, dict_T* locals غیر_مستعمل, خالی *arg غیر_مستعمل
#اگر_تعریف_شدہ PY_CAN_RECURSE
	, PyGILState_STATE *pygilstate غیر_مستعمل
#اگر_ختم
	)
{
    PyObject	*تسلسل_ret;
    تسلسل_ret = Py_تسلسل_String((حرف *)cmd, Py_file_داخلہ, globals, globals);
    اگر (تسلسل_ret != تہی)
    {
	Py_DECREF(تسلسل_ret);
    }
    ورنہ اگر (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
    {
	emsg(_(e_cant_handle_systemexit_کا_python_exception_اندر_vim));
	PyErr_Clear();
    }
    ورنہ
	PyErr_PrintEx(1);
}

ساکن ثابت حرف	*رمز_hdr = "def " DOPY_FUNC "(line, linenr):\n ";
ساکن صحیح		رمز_hdr_len = 30;

    ساکن خالی
تسلسل_do(ثابت حرف *cmd, dict_T* locals غیر_مستعمل, خالی *arg غیر_مستعمل
#اگر_تعریف_شدہ PY_CAN_RECURSE
	, PyGILState_STATE *pygilstate
#اگر_ختم
	)
{
    PyInt	lnum;
    size_t	len;
    حرف	*رمز;
    صحیح		حالت_2;
    PyObject	*pyfunc, *pymain;
    PyObject	*تسلسل_ret;
    buf_T	*was_curbuf = curbuf;

    اگر (u_save((linenr_T)Range_آغاز - 1, (linenr_T)Range_انتہا + 1) != OK)
    {
	emsg(_("cannot save undo information"));
	واپس;
    }

    len = رمز_hdr_len + STRLEN(cmd);
    رمز = PyMem_New(حرف, len + 1);
    حافظہ_نقل(رمز, رمز_hdr, رمز_hdr_len);
    STRCPY(رمز + رمز_hdr_len, cmd);
    تسلسل_ret = Py_تسلسل_String(رمز, Py_file_داخلہ, globals, globals);
    حالت_2 = -1;
    اگر (تسلسل_ret != تہی)
    {
	حالت_2 = 0;
	Py_DECREF(تسلسل_ret);
    }
    ورنہ اگر (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
    {
	PyMem_Free(رمز);
	emsg(_(e_cant_handle_systemexit_کا_python_exception_اندر_vim));
	PyErr_Clear();
	واپس;
    }
    ورنہ
	PyErr_PrintEx(1);

    PyMem_Free(رمز);

    اگر (حالت_2)
    {
	emsg(_("failed to run the code"));
	واپس;
    }

    حالت_2 = 0;
    pymain = Py_Import_جوڑو_Module("__main__");
    pyfunc = PyObject_GetAttrString(pymain, DOPY_FUNC);
#اگر_تعریف_شدہ PY_CAN_RECURSE
    PyGILState_Release(*pygilstate);
#اگر_ختم

    برائے (lnum = Range_آغاز; lnum <= Range_انتہا; ++lnum)
    {
	PyObject	*سطر;
	PyObject	*linenr;
	PyObject	*ret;

#اگر_تعریف_شدہ PY_CAN_RECURSE
	*pygilstate = PyGILState_Ensure();
#اگر_ختم
	// Check the line number, the command my have deleted lines.
	اگر (lnum > curbuf->b_ml.ml_سطر_گنتی
		|| !(سطر = Get_بفر_سطر(curbuf, lnum)))
	    جاؤ err;
	اگر (!(linenr = PyInt_FromLong((طویل) lnum)))
	{
	    Py_DECREF(سطر);
	    جاؤ err;
	}
	ret = Py_Object_Call_Function_Obj_دلائل(pyfunc, سطر, linenr, تہی);
	Py_DECREF(سطر);
	Py_DECREF(linenr);
	اگر (!ret)
	    جاؤ err;

	// Check that the command didn't switch to another buffer.
	// Check the line number, the command my have deleted lines.
	اگر (curbuf != was_curbuf || lnum > curbuf->b_ml.ml_سطر_گنتی)
	{
	    Py_XDECREF(ret);
	    جاؤ err;
	}

	اگر (ret != Py_None)
	    اگر (سیٹ_بفر_سطر_2(curbuf, lnum, ret, تہی) == FAIL)
	    {
		Py_XDECREF(ret);
		جاؤ err;
	    }

	Py_XDECREF(ret);
	PythonIO_Flush();
#اگر_تعریف_شدہ PY_CAN_RECURSE
	PyGILState_Release(*pygilstate);
#اگر_ختم
    }
    جاؤ برآمد;
err:
#اگر_تعریف_شدہ PY_CAN_RECURSE
    *pygilstate = PyGILState_Ensure();
#اگر_ختم
    PyErr_PrintEx(0);
    PythonIO_Flush();
    حالت_2 = 1;
برآمد:
#اگر_تعریف_شدہ PY_CAN_RECURSE
    اگر (!حالت_2)
	*pygilstate = PyGILState_Ensure();
#اگر_ختم
    Py_DECREF(pyfunc);
    Py_Object_سیٹ_Attr_String(pymain, DOPY_FUNC, تہی);
    اگر (حالت_2)
	واپس;
    check_cursor();
    update_curbuf(UPD_NOT_VALID);
}

    ساکن خالی
تسلسل_eval(ثابت حرف *cmd, dict_T *locals, خالی *arg
#اگر_تعریف_شدہ PY_CAN_RECURSE
	, PyGILState_STATE *pygilstate غیر_مستعمل
#اگر_ختم
	)
{
    PyObject	*تسلسل_ret;
    typval_T	*rettv = (typval_T*)arg;
    PyObject	*pylocals = locals ? NEW_DICTIONARY(locals) : globals;

    تسلسل_ret = Py_تسلسل_String((حرف *)cmd, Py_eval_داخلہ, globals, pylocals);
    اگر (تسلسل_ret == تہی)
    {
	اگر (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
	{
	    emsg(_(e_cant_handle_systemexit_کا_python_exception_اندر_vim));
	    PyErr_Clear();
	}
	ورنہ
	{
	    اگر (PyErr_Occurred() && !پیغام_silent)
		PyErr_PrintEx(0);
	    emsg(_(e_eval_did_not_return_valid_python_object));
	}
    }
    ورنہ
    {
	اگر (ConvertFromPyObject(تسلسل_ret, rettv) == -1)
	    emsg(_(e_failed_مقصد_convert_returned_python_object_مقصد_vim_قدر));
	Py_DECREF(تسلسل_ret);
    }
    PyErr_Clear();
}

    ساکن صحیح
سیٹ_ref_اندر_py(ثابت صحیح copyID)
{
    pylinkedlist_T	*cur;
    list_T		*ll;
    tuple_T		*tt;
    صحیح			i;
    صحیح			abort = FALSE;
    FunctionObject	*func;

    اگر (lastdict != تہی)
    {
	برائے (cur = lastdict ; !abort && cur != تہی ; cur = cur->pll_پچھلا)
	    abort = سیٹ_ref_اندر_dict(((DictionaryObject *)(cur->pll_obj))->dict,
								       copyID);
    }

    اگر (lastlist != تہی)
    {
	برائے (cur = lastlist ; !abort && cur != تہی ; cur = cur->pll_پچھلا)
	{
	    ll = ((ListObject *) (cur->pll_obj))->list;
	    abort = سیٹ_ref_اندر_list(ll, copyID);
	}
    }

    اگر (lasttuple != تہی)
    {
	برائے (cur = lasttuple ; !abort && cur != تہی ; cur = cur->pll_پچھلا)
	{
	    tt = ((TupleObject *) (cur->pll_obj))->tuple;
	    abort = سیٹ_ref_اندر_tuple(tt, copyID);
	}
    }

    اگر (lastfunc != تہی)
    {
	برائے (cur = lastfunc ; !abort && cur != تہی ; cur = cur->pll_پچھلا)
	{
	    func = (FunctionObject *) cur->pll_obj;
	    abort = سیٹ_ref_اندر_dict(func->self, copyID);
	    اگر (func->argc)
		برائے (i = 0; !abort && i < func->argc; ++i)
		    abort = abort
			|| سیٹ_ref_اندر_شے(&func->argv[i], copyID, تہی, تہی, تہی);
	}
    }

    واپس abort;
}

    ساکن صحیح
سیٹ_string_copy(char_u *str, typval_T *tv)
{
    tv->vval.v_string = vim_strsave(str);
    اگر (tv->vval.v_string == تہی)
    {
	Py_Err_No_حافظہ();
	واپس -1;
    }
    واپس 0;
}

    ساکن صحیح
pydict_مقصد_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    dict_T	*dict;
    char_u	*کلید;
    dictitem_T	*di;
    PyObject	*کلید_Object;
    PyObject	*valObject;
    Py_ssize_t	iter = 0;

    اگر (!(dict = py_dict_alloc()))
	واپس -1;

    tv->v_type = VAR_DICT;
    tv->vval.v_dict = dict;

    جب (Py_Dict_اگلا(obj, &iter, &کلید_Object, &valObject))
    {
	PyObject	*todecref = تہی;

	اگر (کلید_Object == تہی || valObject == تہی)
	{
	    dict_unref(dict);
	    واپس -1;
	}

	اگر (!(کلید = String_مقصد_Chars(کلید_Object, &todecref)))
	{
	    dict_unref(dict);
	    واپس -1;
	}

	اگر (*کلید == NUL)
	{
	    dict_unref(dict);
	    Py_XDECREF(todecref);
	    RAISE_NO_خالی_خانے_کلیدیں;
	    واپس -1;
	}

	di = dictitem_alloc(کلید);

	Py_XDECREF(todecref);

	اگر (di == تہی)
	{
	    Py_Err_No_حافظہ();
	    dict_unref(dict);
	    واپس -1;
	}

	اگر (_ConvertFromPyObject(valObject, &di->di_tv, lookup_dict) == -1)
	{
	    vim_free(di);
	    dict_unref(dict);
	    واپس -1;
	}

	اگر (dict_جوڑو(dict, di) == FAIL)
	{
	    RAISE_کلید_جوڑو_FAIL(di->di_کلید);
	    clear_tv(&di->di_tv);
	    vim_free(di);
	    dict_unref(dict);
	    واپس -1;
	}
    }

    --dict->dv_refcount;
    واپس 0;
}

    ساکن صحیح
pymap_مقصد_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    dict_T	*dict;
    char_u	*کلید;
    dictitem_T	*di;
    PyObject	*list;
    PyObject	*iterator;
    PyObject	*کلید_Object;
    PyObject	*valObject;

    اگر (!(dict = py_dict_alloc()))
	واپس -1;

    tv->v_type = VAR_DICT;
    tv->vval.v_dict = dict;

    اگر (!(list = Py_Mapping_کلیدیں(obj)))
    {
	dict_unref(dict);
	واپس -1;
    }

    اگر (!(iterator = PyObject_GetIter(list)))
    {
	dict_unref(dict);
	Py_DECREF(list);
	واپس -1;
    }
    Py_DECREF(list);

    جب ((کلید_Object = Py_Iter_اگلا(iterator)))
    {
	PyObject	*todecref;

	اگر (!(کلید = String_مقصد_Chars(کلید_Object, &todecref)))
	{
	    Py_DECREF(کلید_Object);
	    Py_DECREF(iterator);
	    dict_unref(dict);
	    واپس -1;
	}

	اگر (*کلید == NUL)
	{
	    Py_DECREF(کلید_Object);
	    Py_DECREF(iterator);
	    Py_XDECREF(todecref);
	    dict_unref(dict);
	    RAISE_NO_خالی_خانے_کلیدیں;
	    واپس -1;
	}

	اگر (!(valObject = Py_Object_Get_شے(obj, کلید_Object)))
	{
	    Py_DECREF(کلید_Object);
	    Py_DECREF(iterator);
	    Py_XDECREF(todecref);
	    dict_unref(dict);
	    واپس -1;
	}

	di = dictitem_alloc(کلید);

	Py_DECREF(کلید_Object);
	Py_XDECREF(todecref);

	اگر (di == تہی)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(valObject);
	    dict_unref(dict);
	    Py_Err_No_حافظہ();
	    واپس -1;
	}

	اگر (_ConvertFromPyObject(valObject, &di->di_tv, lookup_dict) == -1)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(valObject);
	    vim_free(di);
	    dict_unref(dict);
	    واپس -1;
	}

	Py_DECREF(valObject);

	اگر (dict_جوڑو(dict, di) == FAIL)
	{
	    RAISE_کلید_جوڑو_FAIL(di->di_کلید);
	    Py_DECREF(iterator);
	    dictitem_free(di);
	    dict_unref(dict);
	    واپس -1;
	}
    }
    Py_DECREF(iterator);
    --dict->dv_refcount;
    واپس 0;
}

    ساکن صحیح
pyseq_مقصد_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    list_T	*l;

    اگر (!(l = py_list_alloc()))
	واپس -1;

    tv->v_type = VAR_LIST;
    tv->vval.v_list = l;

    اگر (list_py_concat(l, obj, lookup_dict) == -1)
    {
	list_unref(l);
	واپس -1;
    }

    --l->lv_refcount;
    واپس 0;
}

    ساکن صحیح
pytuple_مقصد_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    tuple_T	*t;

    اگر (!(t = py_tuple_alloc()))
	واپس -1;

    tv->v_type = VAR_TUPLE;
    tv->vval.v_tuple = t;

    اگر (tuple_py_concat(t, obj, lookup_dict) == -1)
    {
	tuple_unref(t);
	واپس -1;
    }

    --t->tv_refcount;
    واپس 0;
}

قسمتعریف صحیح (*pytotvfunc)(PyObject *, typval_T *, PyObject *);

    ساکن صحیح
convert_dl(PyObject *obj, typval_T *tv,
				    pytotvfunc py_مقصد_tv, PyObject *lookup_dict)
{
    PyObject	*capsule;
    حرف	hexBuf[حجم(خالی *) * 2 + 3];

    sprintf(hexBuf, "%p", (خالی *)obj);

#اگر_تعریف_شدہ PY_USE_CAPSULE
    capsule = Py_Dict_Get_شے_String(lookup_dict, hexBuf);
#ورنہ
    capsule = (PyObject *)Py_Dict_Get_شے_String(lookup_dict, hexBuf);
#اگر_ختم
    اگر (capsule == تہی)
    {
#اگر_تعریف_شدہ PY_USE_CAPSULE
	capsule = PyCapsule_New(tv, تہی, تہی);
#ورنہ
	capsule = PyCObject_FromVoidPtr(tv, تہی);
#اگر_ختم
	اگر (Py_Dict_سیٹ_شے_String(lookup_dict, hexBuf, capsule))
	{
	    Py_DECREF(capsule);
	    tv->v_type = VAR_UNKNOWN;
	    واپس -1;
	}

	Py_DECREF(capsule);

	اگر (py_مقصد_tv(obj, tv, lookup_dict) == -1)
	{
	    tv->v_type = VAR_UNKNOWN;
	    واپس -1;
	}
	// As we are not using copy_tv which increments reference count we must
	// do it ourself.
	اگر (tv->v_type == VAR_DICT)
	    ++tv->vval.v_dict->dv_refcount;
	ورنہ اگر (tv->v_type == VAR_LIST)
	    ++tv->vval.v_list->lv_refcount;
	ورنہ اگر (tv->v_type == VAR_TUPLE)
	    ++tv->vval.v_tuple->tv_refcount;
    }
    ورنہ
    {
	typval_T	*v;

#اگر_تعریف_شدہ PY_USE_CAPSULE
	v = Py_Capsule_Get_اشارہ_گر(capsule, تہی);
#ورنہ
	v = PyCObject_AsVoidPtr(capsule);
#اگر_ختم
	copy_tv(v, tv);
    }
    واپس 0;
}

    ساکن صحیح
ConvertFromPyMapping(PyObject *obj, typval_T *tv)
{
    PyObject	*lookup_dict;
    صحیح		ret;

    اگر (!(lookup_dict = PyDict_New()))
	واپس -1;

    اگر (PyType_IsSubtype(obj->ob_type, DictionaryTypePtr))
    {
	tv->v_type = VAR_DICT;
	tv->vval.v_dict = (((DictionaryObject *)(obj))->dict);
	++tv->vval.v_dict->dv_refcount;
	ret = 0;
    }
    ورنہ اگر (PyDict_Check(obj))
	ret = convert_dl(obj, tv, pydict_مقصد_tv, lookup_dict);
    ورنہ اگر (PyMapping_Check(obj))
	ret = convert_dl(obj, tv, pymap_مقصد_tv, lookup_dict);
    ورنہ
    {
	PyErr_FORMAT_TYPE(
		N_("unable to convert %s to a Vim dictionary"),
		obj);
	ret = -1;
    }
    Py_DECREF(lookup_dict);
    واپس ret;
}

    ساکن صحیح
ConvertFromPySequence(PyObject *obj, typval_T *tv)
{
    PyObject	*lookup_dict;
    صحیح		ret;

    اگر (!(lookup_dict = PyDict_New()))
	واپس -1;

    اگر (PyType_IsSubtype(obj->ob_type, ListTypePtr))
    {
	tv->v_type = VAR_LIST;
	tv->vval.v_list = (((ListObject *)(obj))->list);
	++tv->vval.v_list->lv_refcount;
	ret = 0;
    }
    ورنہ اگر (PyIter_Check(obj) || PySequence_Check(obj))
	ret = convert_dl(obj, tv, pyseq_مقصد_tv, lookup_dict);
    ورنہ
    {
	PyErr_FORMAT_TYPE(
		N_("unable to convert %s to a Vim list"),
		obj);
	ret = -1;
    }
    Py_DECREF(lookup_dict);
    واپس ret;
}

    ساکن صحیح
ConvertFromPyObject(PyObject *obj, typval_T *tv)
{
    PyObject	*lookup_dict;
    صحیح		ret;

    اگر (!(lookup_dict = PyDict_New()))
	واپس -1;
    ret = _ConvertFromPyObject(obj, tv, lookup_dict);
    Py_DECREF(lookup_dict);
    واپس ret;
}

    ساکن صحیح
_ConvertFromPyObject(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    اگر (PyType_IsSubtype(obj->ob_type, DictionaryTypePtr))
    {
	tv->v_type = VAR_DICT;
	tv->vval.v_dict = (((DictionaryObject *)(obj))->dict);
	++tv->vval.v_dict->dv_refcount;
    }
    ورنہ اگر (PyType_IsSubtype(obj->ob_type, ListTypePtr))
    {
	tv->v_type = VAR_LIST;
	tv->vval.v_list = (((ListObject *)(obj))->list);
	++tv->vval.v_list->lv_refcount;
    }
    ورنہ اگر (PyType_IsSubtype(obj->ob_type, TupleTypePtr))
    {
	tv->v_type = VAR_TUPLE;
	tv->vval.v_tuple = (((TupleObject *)(obj))->tuple);
	++tv->vval.v_tuple->tv_refcount;
    }
    ورنہ اگر (PyType_IsSubtype(obj->ob_type, FunctionTypePtr))
    {
	FunctionObject *func = (FunctionObject *) obj;
	اگر (func->self != تہی || func->argv != تہی)
	{
	    partial_T *pt = ALLOC_CLEAR_ONE(partial_T);

	    سیٹ_partial(func, pt, TRUE);
	    tv->vval.v_partial = pt;
	    tv->v_type = VAR_PARTIAL;
	}
	ورنہ
	{
	    اگر (سیٹ_string_copy(func->name, tv) == -1)
		واپس -1;

	    tv->v_type = VAR_FUNC;
	}
	func_ref(func->name);
    }
    ورنہ اگر (Py_بائٹیں_Check(obj))
    {
	char_u	*str;

	اگر (Py_بائٹیں_As_String_And_سائز(obj, (حرف **) &str, تہی) == -1)
	    واپس -1;
	اگر (str == تہی)
	    واپس -1;

	اگر (سیٹ_string_copy(str, tv) == -1)
	    واپس -1;

	tv->v_type = VAR_STRING;
    }
    ورنہ اگر (PyUnicode_Check(obj))
    {
	PyObject	*بائٹیں;
	char_u	*str;

	بائٹیں = PyUnicode_AsEncodedString(obj, ENC_OPT, ERRORS_ENCODE_ARG);
	اگر (بائٹیں == تہی)
	    واپس -1;

	اگر (Py_بائٹیں_As_String_And_سائز(بائٹیں, (حرف **) &str, تہی) == -1)
	    واپس -1;
	اگر (str == تہی)
	    واپس -1;

	اگر (سیٹ_string_copy(str, tv))
	{
	    Py_XDECREF(بائٹیں);
	    واپس -1;
	}
	Py_XDECREF(بائٹیں);

	tv->v_type = VAR_STRING;
    }
#اگر PY_MAJOR_نسخہ < 3
    ورنہ اگر (PyInt_Check(obj))
    {
	tv->v_type = VAR_نمبر;
	tv->vval.v_نمبر = (varnumber_T) PyInt_AsLong(obj);
	اگر (PyErr_Occurred())
	    واپس -1;
    }
#اگر_ختم
    ورنہ اگر (PyLong_Check(obj))
    {
	tv->v_type = VAR_نمبر;
	tv->vval.v_نمبر = (varnumber_T) PyLong_AsLong(obj);
	اگر (PyErr_Occurred())
	    واپس -1;
    }
    ورنہ اگر (PyTuple_Check(obj))
	واپس convert_dl(obj, tv, pytuple_مقصد_tv, lookup_dict);
    ورنہ اگر (PyDict_Check(obj))
	واپس convert_dl(obj, tv, pydict_مقصد_tv, lookup_dict);
    ورنہ اگر (PyFloat_Check(obj))
    {
	tv->v_type = VAR_FLOAT;
	tv->vval.v_float = (float_T) PyFloat_AsDouble(obj);
    }
    ورنہ اگر (PyObject_HasAttrString(obj, "keys"))
	واپس convert_dl(obj, tv, pymap_مقصد_tv, lookup_dict);
    // PyObject_GetIter can create built-in iterator for any sequence object
    ورنہ اگر (PyIter_Check(obj) || PySequence_Check(obj))
	واپس convert_dl(obj, tv, pyseq_مقصد_tv, lookup_dict);
    ورنہ اگر (PyMapping_Check(obj))
	واپس convert_dl(obj, tv, pymap_مقصد_tv, lookup_dict);
    ورنہ اگر (Py_نمبر_Check(obj))
    {
	PyObject	*عدد;

	اگر (!(عدد = Py_نمبر_Long(obj)))
	    واپس -1;

	tv->v_type = VAR_نمبر;
	tv->vval.v_نمبر = (varnumber_T) PyLong_AsLong(عدد);

	Py_DECREF(عدد);
    }
    ورنہ اگر (obj == Py_None)
    {
	tv->v_type = VAR_SPECIAL;
	tv->vval.v_نمبر = VVAL_NONE;
    }
    ورنہ
    {
	PyErr_FORMAT_TYPE(
		N_("unable to convert %s to a Vim structure"),
		obj);
	واپس -1;
    }
    واپس 0;
}

    ساکن PyObject *
Convert_مقصد_Py_Object(typval_T *tv)
{
    typval_T *argv;
    صحیح i;
    اگر (tv == تہی)
    {
	Py_Err_سیٹ_VIM(N_("internal error: NULL reference passed"));
	واپس تہی;
    }
    انتخاب (tv->v_type)
    {
	حالت VAR_STRING:
	    واپس Py_بائٹیں_From_String(tv->vval.v_string == تہی
					    ? "" : (حرف *)tv->vval.v_string);
	حالت VAR_نمبر:
	    واپس PyLong_FromLong((طویل) tv->vval.v_نمبر);
	حالت VAR_FLOAT:
	    واپس PyFloat_FromDouble((دوہرا) tv->vval.v_float);
	حالت VAR_LIST:
	    واپس NEW_LIST(tv->vval.v_list);
	حالت VAR_TUPLE:
	    واپس NEW_TUPLE(tv->vval.v_tuple);
	حالت VAR_DICT:
	    واپس NEW_DICTIONARY(tv->vval.v_dict);
	حالت VAR_FUNC:
	    واپس NEW_FUNCTION(tv->vval.v_string == تہی
					  ? (char_u *)"" : tv->vval.v_string,
					  0, تہی, تہی, TRUE);
	حالت VAR_PARTIAL:
	    اگر (tv->vval.v_partial->pt_argc)
	    {
		argv = PyMem_New(typval_T, (size_t)tv->vval.v_partial->pt_argc);
		برائے (i = 0; i < tv->vval.v_partial->pt_argc; i++)
		    copy_tv(&tv->vval.v_partial->pt_argv[i], &argv[i]);
	    }
	    ورنہ
		argv = تہی;
	    اگر (tv->vval.v_partial->pt_dict != تہی)
		tv->vval.v_partial->pt_dict->dv_refcount++;
	    واپس NEW_FUNCTION(tv->vval.v_partial == تہی
			     ? (char_u *)"" : partial_name(tv->vval.v_partial),
				tv->vval.v_partial->pt_argc, argv,
				tv->vval.v_partial->pt_dict,
				tv->vval.v_partial->pt_auto);
	حالت VAR_BLOB:
	    واپس Py_بائٹیں_From_String_And_سائز(
		(حرف*) tv->vval.v_blob->bv_ga.ga_ڈیٹا,
		(Py_ssize_t) tv->vval.v_blob->bv_ga.ga_len);
	حالت VAR_UNKNOWN:
	حالت VAR_ANY:
	حالت VAR_VOID:
	حالت VAR_CHANNEL:
	حالت VAR_کام:
	حالت VAR_INSTR:
	حالت VAR_CLASS:
	حالت VAR_OBJECT:
	حالت VAR_TYPEALIAS:
	    Py_INCREF(Py_None);
	    واپس Py_None;
	حالت VAR_BOOL:
	حالت VAR_SPECIAL:
	    انتخاب (tv->vval.v_نمبر)
	    {
		حالت VVAL_FALSE: واپس ALWAYS_FALSE;
		حالت VVAL_TRUE:  واپس ALWAYS_TRUE;
		حالت VVAL_NONE:
		حالت VVAL_NULL:  واپس ALWAYS_NONE;
	    }
	    Py_Err_سیٹ_VIM(N_("internal error: invalid value type"));
	    واپس تہی;
    }
    واپس تہی;
}

DEFINE_PY_TYPE_OBJECT(CurrentType);

قسمتعریف ساخت
{
    Py_Object_سر
} CurrentObject;

ساکن CurrentObject TheCurrent =
{
    Py_Object_سر_INIT_TYPE(CurrentType)
};

    ساکن خالی
init_structs(خالی)
{
    CLEAR_میدان(OutputType);
    OutputType.tp_name = "vim.message";
    OutputType.tp_basicsize = حجم(OutputObject);
    OutputType.tp_flags = Py_TPFLAGS_DEFAULT;
    OutputType.tp_doc = "vim message object";
    OutputType.tp_methods = OutputMethods;
#اگر PY_MAJOR_نسخہ >= 3
    OutputType.tp_getattro = OutputGetattro;
    OutputType.tp_setattro = OutputSetattro;
    OutputType.tp_alloc = call_PyType_GenericAlloc;
    OutputType.tp_new = call_PyType_GenericNew;
    OutputType.tp_free = call_PyObject_Free;
# ifndef USE_LIMITED_API
    // The std printer type is only exposed in full API. It is not essential
    // anyway and so in limited API we don't set it.
    OutputType.tp_بنیاد = &PyStdPrinter_Type;
# endif
#ورنہ
    OutputType.tp_getattr = OutputGetattr;
    OutputType.tp_setattr = OutputSetattr;
    // Disabled, because this causes a crash in test86
    // OutputType.tp_base = &PyFile_Type;
#اگر_ختم

    CLEAR_میدان(IterType);
    IterType.tp_name = "vim.iter";
    IterType.tp_basicsize = حجم(IterObject);
    IterType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    IterType.tp_doc = "generic iterator object";
    IterType.tp_iter = IterIter;
    IterType.tp_iternext = Iter_اگلا;
    IterType.tp_dealloc = IterDestructor;
    IterType.tp_traverse = IterTraverse;
    IterType.tp_clear = IterClear;

    CLEAR_میدان(بفر_Type);
    بفر_Type.tp_name = "vim.buffer";
    بفر_Type.tp_basicsize = حجم(بفر_Object);
    بفر_Type.tp_dealloc = بفر_Destructor;
    بفر_Type.tp_repr = بفر_Repr;
    بفر_Type.tp_as_sequence = &بفر_As_Seq;
    بفر_Type.tp_as_mapping = &بفر_As_Mapping;
    بفر_Type.tp_flags = Py_TPFLAGS_DEFAULT;
    بفر_Type.tp_doc = "vim buffer object";
    بفر_Type.tp_methods = بفر_Methods;
#اگر PY_MAJOR_نسخہ >= 3
    بفر_Type.tp_getattro = بفر_Getattro;
    بفر_Type.tp_setattro = بفر_Setattro;
    بفر_Type.tp_alloc = call_PyType_GenericAlloc;
    بفر_Type.tp_new = call_PyType_GenericNew;
    بفر_Type.tp_free = call_PyObject_Free;
#ورنہ
    بفر_Type.tp_getattr = بفر_Getattr;
    بفر_Type.tp_setattr = بفر_Setattr;
#اگر_ختم

    CLEAR_میدان(کھڑکی_Type);
    کھڑکی_Type.tp_name = "vim.window";
    کھڑکی_Type.tp_basicsize = حجم(کھڑکی_Object);
    کھڑکی_Type.tp_dealloc = کھڑکی_Destructor;
    کھڑکی_Type.tp_repr = کھڑکی_Repr;
    کھڑکی_Type.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    کھڑکی_Type.tp_doc = "vim Window object";
    کھڑکی_Type.tp_methods = کھڑکی_Methods;
    کھڑکی_Type.tp_traverse = کھڑکی_Traverse;
    کھڑکی_Type.tp_clear = کھڑکی_Clear;
#اگر PY_MAJOR_نسخہ >= 3
    کھڑکی_Type.tp_getattro = کھڑکی_Getattro;
    کھڑکی_Type.tp_setattro = کھڑکی_Setattro;
    کھڑکی_Type.tp_alloc = call_PyType_GenericAlloc;
    کھڑکی_Type.tp_new = call_PyType_GenericNew;
    کھڑکی_Type.tp_free = call_PyObject_Free;
#ورنہ
    کھڑکی_Type.tp_getattr = کھڑکی_Getattr;
    کھڑکی_Type.tp_setattr = کھڑکی_Setattr;
#اگر_ختم

    CLEAR_میدان(Tab_صفحہ_Type);
    Tab_صفحہ_Type.tp_name = "vim.tabpage";
    Tab_صفحہ_Type.tp_basicsize = حجم(Tab_صفحہ_Object);
    Tab_صفحہ_Type.tp_dealloc = Tab_صفحہ_Destructor;
    Tab_صفحہ_Type.tp_repr = Tab_صفحہ_Repr;
    Tab_صفحہ_Type.tp_flags = Py_TPFLAGS_DEFAULT;
    Tab_صفحہ_Type.tp_doc = "vim tab page object";
    Tab_صفحہ_Type.tp_methods = Tab_صفحہ_Methods;
#اگر PY_MAJOR_نسخہ >= 3
    Tab_صفحہ_Type.tp_getattro = Tab_صفحہ_Getattro;
    Tab_صفحہ_Type.tp_alloc = call_PyType_GenericAlloc;
    Tab_صفحہ_Type.tp_new = call_PyType_GenericNew;
    Tab_صفحہ_Type.tp_free = call_PyObject_Free;
#ورنہ
    Tab_صفحہ_Type.tp_getattr = Tab_صفحہ_Getattr;
#اگر_ختم

    CLEAR_میدان(BufMapType);
    BufMapType.tp_name = "vim.bufferlist";
    BufMapType.tp_basicsize = حجم(BufMapObject);
    BufMapType.tp_as_mapping = &BufMapAsMapping;
    BufMapType.tp_flags = Py_TPFLAGS_DEFAULT;
    BufMapType.tp_iter = BufMapIter;
    BufMapType.tp_doc = "vim buffer list";

    CLEAR_میدان(WinListType);
    WinListType.tp_name = "vim.windowlist";
    WinListType.tp_basicsize = حجم(WinListObject);
    WinListType.tp_as_sequence = &WinListAsSeq;
    WinListType.tp_flags = Py_TPFLAGS_DEFAULT;
    WinListType.tp_doc = "vim window list";
    WinListType.tp_dealloc = (destructor)WinListDestructor;

    CLEAR_میدان(TabListType);
    TabListType.tp_name = "vim.tabpagelist";
    TabListType.tp_basicsize = حجم(TabListObject);
    TabListType.tp_as_sequence = &TabListAsSeq;
    TabListType.tp_flags = Py_TPFLAGS_DEFAULT;
    TabListType.tp_doc = "vim tab page list";

    CLEAR_میدان(RangeType);
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
#اگر PY_MAJOR_نسخہ >= 3
    RangeType.tp_getattro = RangeGetattro;
    RangeType.tp_alloc = call_PyType_GenericAlloc;
    RangeType.tp_new = call_PyType_GenericNew;
    RangeType.tp_free = call_PyObject_Free;
#ورنہ
    RangeType.tp_getattr = RangeGetattr;
#اگر_ختم

    CLEAR_میدان(CurrentType);
    CurrentType.tp_name = "vim.currentdata";
    CurrentType.tp_basicsize = حجم(CurrentObject);
    CurrentType.tp_flags = Py_TPFLAGS_DEFAULT;
    CurrentType.tp_doc = "vim current object";
    CurrentType.tp_methods = CurrentMethods;
#اگر PY_MAJOR_نسخہ >= 3
    CurrentType.tp_getattro = CurrentGetattro;
    CurrentType.tp_setattro = CurrentSetattro;
#ورنہ
    CurrentType.tp_getattr = CurrentGetattr;
    CurrentType.tp_setattr = CurrentSetattr;
#اگر_ختم

    CLEAR_میدان(DictionaryType);
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
#اگر PY_MAJOR_نسخہ >= 3
    DictionaryType.tp_getattro = DictionaryGetattro;
    DictionaryType.tp_setattro = DictionarySetattro;
#ورنہ
    DictionaryType.tp_getattr = DictionaryGetattr;
    DictionaryType.tp_setattr = DictionarySetattr;
#اگر_ختم

    CLEAR_میدان(ListType);
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
#اگر PY_MAJOR_نسخہ >= 3
    ListType.tp_getattro = ListGetattro;
    ListType.tp_setattro = ListSetattro;
#ورنہ
    ListType.tp_getattr = ListGetattr;
    ListType.tp_setattr = ListSetattr;
#اگر_ختم

    // Tuple type
    CLEAR_میدان(TupleType);
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
#اگر PY_MAJOR_نسخہ >= 3
    TupleType.tp_getattro = TupleGetattro;
    TupleType.tp_setattro = TupleSetattro;
#ورنہ
    TupleType.tp_getattr = TupleGetattr;
    TupleType.tp_setattr = TupleSetattr;
#اگر_ختم

    CLEAR_میدان(FunctionType);
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
#اگر PY_MAJOR_نسخہ >= 3
    FunctionType.tp_getattro = FunctionGetattro;
#ورنہ
    FunctionType.tp_getattr = FunctionGetattr;
#اگر_ختم

    CLEAR_میدان(OptionsType);
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

#اگر PY_نسخہ_HEX < 0x030700f0
    CLEAR_میدان(LoaderType);
    LoaderType.tp_name = "vim.Loader";
    LoaderType.tp_basicsize = حجم(LoaderObject);
    LoaderType.tp_flags = Py_TPFLAGS_DEFAULT;
    LoaderType.tp_doc = "vim message object";
    LoaderType.tp_methods = LoaderMethods;
    LoaderType.tp_dealloc = LoaderDestructor;
#اگر_ختم

#اگر PY_MAJOR_نسخہ >= 3
    CLEAR_میدان(vimmodule);
    vimmodule.m_name = "vim";
    vimmodule.m_doc = "Vim Python interface\n";
    vimmodule.m_سائز_2 = -1;
    vimmodule.m_methods = VimMethods;
#اگر_ختم
}

    ساکن صحیح
init_types(خالی)
{
    PYTYPE_READY(IterType);
    PYTYPE_READY(بفر_Type);
    PYTYPE_READY(RangeType);
    PYTYPE_READY(کھڑکی_Type);
    PYTYPE_READY(Tab_صفحہ_Type);
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
#اگر PY_نسخہ_HEX < 0x030700f0
    PYTYPE_READY(LoaderType);
#اگر_ختم

#اگر_تعریف_شدہ USE_LIMITED_API
    // We need to finish initializing all the static objects because the types
    // are only just allocated on the heap now.
    // Each PyObject_HEAD_INIT_TYPE should correspond to a
    // PyObject_FINISH_INIT_TYPE below.
    Py_Object_اختتام_INIT_TYPE(Output, OutputType);
    Py_Object_اختتام_INIT_TYPE(Error, OutputType);
    Py_Object_اختتام_INIT_TYPE(The_بفر_Map, BufMapType);
    Py_Object_اختتام_INIT_TYPE(The_کھڑکی_List, WinListType);
    Py_Object_اختتام_INIT_TYPE(TheCurrent, CurrentType);
    Py_Object_اختتام_INIT_TYPE(The_Tab_صفحہ_List, TabListType);
#اگر_ختم
    واپس 0;
}

#اگر_تعریف_شدہ USE_LIMITED_API
    ساکن خالی
shutdown_types(خالی)
{
    PYTYPE_CLEANUP(IterType);
    PYTYPE_CLEANUP(بفر_Type);
    PYTYPE_CLEANUP(RangeType);
    PYTYPE_CLEANUP(کھڑکی_Type);
    PYTYPE_CLEANUP(Tab_صفحہ_Type);
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
# اگر PY_نسخہ_HEX < 0x030700f0
    PYTYPE_CLEANUP(LoaderType);
# endif
}
#اگر_ختم

    ساکن صحیح
init_sys_راستہ(خالی)
{
    PyObject	*راستہ;
    PyObject	*راستہ_hook;
    PyObject	*راستہ_hooks;

    اگر (!(راستہ_hook = PyObject_GetAttrString(vim_module, "path_hook")))
	واپس -1;

    اگر (!(راستہ_hooks = PySys_GetObject("path_hooks")))
    {
	PyErr_Clear();
	راستہ_hooks = PyList_New(1);
	Py_List_سیٹ_شے(راستہ_hooks, 0, راستہ_hook);
	اگر (Py_Sys_سیٹ_Object("path_hooks", راستہ_hooks))
	{
	    Py_DECREF(راستہ_hooks);
	    واپس -1;
	}
	Py_DECREF(راستہ_hooks);
    }
    ورنہ اگر (PyList_Check(راستہ_hooks))
    {
	اگر (PyList_Append(راستہ_hooks, راستہ_hook))
	{
	    Py_DECREF(راستہ_hook);
	    واپس -1;
	}
	Py_DECREF(راستہ_hook);
    }
    ورنہ
    {
	Vim_Try_آغاز();
	emsg(_("Failed to set path hook: sys.path_hooks is not a list\n"
	       "You should now do the following:\n"
	       "- append vim.path_hook to sys.path_hooks\n"
	       "- append vim.VIM_SPECIAL_PATH to sys.path\n"));
	Vim_Try_انتہا(); // Discard the error
	Py_DECREF(راستہ_hook);
	واپس 0;
    }

    اگر (!(راستہ = PySys_GetObject("path")))
    {
	PyErr_Clear();
	راستہ = PyList_New(1);
	Py_INCREF(vim_special_راستہ_object);
	Py_List_سیٹ_شے(راستہ, 0, vim_special_راستہ_object);
	اگر (Py_Sys_سیٹ_Object("path", راستہ))
	{
	    Py_DECREF(راستہ);
	    واپس -1;
	}
	Py_DECREF(راستہ);
    }
    ورنہ اگر (PyList_Check(راستہ))
    {
	اگر (PyList_Append(راستہ, vim_special_راستہ_object))
	    واپس -1;
    }
    ورنہ
    {
	Vim_Try_آغاز();
	emsg(_("Failed to set path: sys.path is not a list\n"
	       "You should now append vim.VIM_SPECIAL_PATH to sys.path"));
	Vim_Try_انتہا(); // Discard the error
    }

    واپس 0;
}

ساکن ساخت numeric_constant {
    حرف	*name;
    صحیح		val;
} numeric_constants[] = {
    {"VAR_LOCKED",	VAR_LOCKED},
    {"VAR_FIXED",	VAR_FIXED},
    {"VAR_SCOPE",	VAR_SCOPE},
    {"VAR_DEF_SCOPE",	VAR_DEF_SCOPE},
};

ساخت object_constant {
    حرف	*name;
    PyObject	*valObject;
};

#تعریف_کرو جوڑو_OBJECT(m, name, obj) \
    اگر (Py_Module_جوڑو_Object(m, name, obj)) \
	واپس -1;

#تعریف_کرو جوڑو_CHECKED_OBJECT(m, name, obj) \
    { \
	PyObject	*valObject = obj; \
	اگر (!valObject) \
	    واپس -1; \
	جوڑو_OBJECT(m, name, valObject); \
    }

    ساکن صحیح
populate_module(PyObject *m)
{
    صحیح		i;
    PyObject	*other_module;
    PyObject	*attr;
    PyObject	*imp;
#اگر PY_نسخہ_HEX >= 0x030700f0
    PyObject	*dict;
    PyObject	*cls;
#اگر_ختم

    برائے (i = 0; i < (صحیح)(حجم(numeric_constants)
					   / حجم(ساخت numeric_constant));
	    ++i)
	جوڑو_CHECKED_OBJECT(m, numeric_constants[i].name,
		PyInt_FromLong(numeric_constants[i].val));

    ساخت object_constant object_constants[] = {
	{"buffers",  (PyObject *)(خالی *)&The_بفر_Map},
	{"windows",  (PyObject *)(خالی *)&The_کھڑکی_List},
	{"tabpages", (PyObject *)(خالی *)&The_Tab_صفحہ_List},
	{"current",  (PyObject *)(خالی *)&TheCurrent},

	{"Buffer",     (PyObject *)بفر_Type_Ptr},
	{"Range",      (PyObject *)RangeTypePtr},
	{"Window",     (PyObject *)کھڑکی_Type_Ptr},
	{"TabPage",    (PyObject *)Tab_صفحہ_Type_Ptr},
	{"Dictionary", (PyObject *)DictionaryTypePtr},
	{"List",       (PyObject *)ListTypePtr},
	{"Tuple",      (PyObject *)TupleTypePtr},
	{"Function",   (PyObject *)FunctionTypePtr},
	{"Options",    (PyObject *)OptionsTypePtr},
#اگر PY_نسخہ_HEX < 0x030700f0
	{"_Loader",    (PyObject *)LoaderTypePtr},
#اگر_ختم
    };

    برائے (i = 0; i < (صحیح)(حجم(object_constants)
					    / حجم(ساخت object_constant));
	    ++i)
    {
	PyObject	*valObject;

	valObject = object_constants[i].valObject;
	Py_INCREF(valObject);
	جوڑو_OBJECT(m, object_constants[i].name, valObject);
    }

    اگر (!(VimError = PyErr_NewException("vim.error", تہی, تہی)))
	واپس -1;
    جوڑو_OBJECT(m, "error", VimError);

    جوڑو_CHECKED_OBJECT(m, "vars",  NEW_DICTIONARY(get_globvar_dict()));
    جوڑو_CHECKED_OBJECT(m, "vvars", NEW_DICTIONARY(get_vimvar_dict()));
    جوڑو_CHECKED_OBJECT(m, "options",
	    OptionsNew(SREQ_GLOBAL, تہی, dummy_check, تہی));

    اگر (!(other_module = PyImport_ImportModule("os")))
	واپس -1;
    جوڑو_OBJECT(m, "os", other_module);

#اگر PY_MAJOR_نسخہ >= 3
    اگر (!(py_getcwd = PyObject_GetAttrString(other_module, "getcwd")))
	واپس -1;
#ورنہ
    اگر (!(py_getcwd = PyObject_GetAttrString(other_module, "getcwdu")))
	واپس -1;
#اگر_ختم
    جوڑو_OBJECT(m, "_getcwd", py_getcwd)

    اگر (!(py_chdir = PyObject_GetAttrString(other_module, "chdir")))
	واپس -1;
    جوڑو_OBJECT(m, "_chdir", py_chdir);
    اگر (!(attr = PyObject_GetAttrString(m, "chdir")))
	واپس -1;
    اگر (Py_Object_سیٹ_Attr_String(other_module, "chdir", attr))
    {
	Py_DECREF(attr);
	واپس -1;
    }
    Py_DECREF(attr);

    اگر ((py_fchdir = PyObject_GetAttrString(other_module, "fchdir")))
    {
	جوڑو_OBJECT(m, "_fchdir", py_fchdir);
	اگر (!(attr = PyObject_GetAttrString(m, "fchdir")))
	    واپس -1;
	اگر (Py_Object_سیٹ_Attr_String(other_module, "fchdir", attr))
	{
	    Py_DECREF(attr);
	    واپس -1;
	}
	Py_DECREF(attr);
    }
    ورنہ
	PyErr_Clear();

    اگر (!(vim_special_راستہ_object = PyString_FromString(vim_special_راستہ)))
	واپس -1;

    جوڑو_OBJECT(m, "VIM_SPECIAL_PATH", vim_special_راستہ_object);

#اگر PY_نسخہ_HEX >= 0x030700f0
    اگر (!(imp = PyImport_ImportModule("importlib.machinery")))
	واپس -1;

    dict = PyModule_GetDict(imp);

    اگر (!(cls = Py_Dict_Get_شے_String(dict, "PathFinder")))
    {
	Py_DECREF(imp);
	واپس -1;
    }

    اگر (!(py_ڈھونڈو_spec = PyObject_GetAttrString(cls, "find_spec")))
    {
	Py_DECREF(imp);
	واپس -1;
    }

# اگر PY_نسخہ_HEX < 0x30c00a7
    // find_module has been removed as of Python 3.12.0a7
    اگر ((py_ڈھونڈو_module = PyObject_GetAttrString(cls, "find_module")))
	جوڑو_OBJECT(m, "_find_module", py_ڈھونڈو_module);
# endif

    Py_DECREF(imp);

    جوڑو_OBJECT(m, "_find_spec", py_ڈھونڈو_spec);
#ورنہ
    اگر (!(imp = PyImport_ImportModule("imp")))
	واپس -1;

    اگر (!(py_ڈھونڈو_module = PyObject_GetAttrString(imp, "find_module")))
    {
	Py_DECREF(imp);
	واپس -1;
    }

    اگر (!(py_بھرو_module = PyObject_GetAttrString(imp, "load_module")))
    {
	Py_DECREF(py_ڈھونڈو_module);
	Py_DECREF(imp);
	واپس -1;
    }

    Py_DECREF(imp);

    جوڑو_OBJECT(m, "_find_module", py_ڈھونڈو_module);
    جوڑو_OBJECT(m, "_load_module", py_بھرو_module);
#اگر_ختم

    واپس 0;
}
