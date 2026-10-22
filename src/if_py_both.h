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

#إذا PY_نسخہ_HEX < 0x02050000
تعريفنوع صحيح Py_ssize_t;  // Python 2.4 and earlier don't have this type.
#جے_انت

// Use values that are known to work, others may make Vim crash.
#تعریف_کرو ENC_OPT (enc_utf8 ? "utf-8" : enc_dbcs ? "euc-jp" : (حرف *)p_enc)
#تعریف_کرو DOPY_FUNC "_vim_pydo"

ساكن ثابت حرف *vim_special_رستہ = "_vim_path_";

#تعریف_کرو Py_Err_سیٹ_STRING(exc, str) Py_Err_سیٹ_String(exc, _(str))
#تعریف_کرو Py_Err_سیٹ_Vim(str) Py_Err_سیٹ_String(VimError, str)
#تعریف_کرو Py_Err_سیٹ_VIM(str) Py_Err_سیٹ_STRING(VimError, str)
#تعریف_کرو PyErr_FORMAT(exc, str, arg) PyErr_Format(exc, _(str), arg)
#تعریف_کرو PyErr_FORMAT2(exc, str, arg1, arg2) PyErr_Format(exc, _(str), arg1,arg2)
#تعریف_کرو PyErr_VIM_FORMAT(str, arg) PyErr_FORMAT(VimError, str, arg)

#جے_تعریف_شدہ USE_LIMITED_API
// Limited Python API. Need to call only exposed functions and remap macros.
// PyTypeObject is an opaque struct.

تعريفنوع بنية {
    lenfunc sq_لمائی;
    binaryfunc sq_concat;
    ssizeargfunc sq_repeat;
    ssizeargfunc sq_شے;
    فراغ *was_sq_ٹکڑا;
    ssizeobjargproc sq_ass_شے;
    فراغ *was_sq_ass_ٹکڑا;
    objobjproc sq_شامل_اے;

    binaryfunc sq_inplace_concat;
    ssizeargfunc sq_inplace_repeat;
} PySequenceMethods;

تعريفنوع بنية {
    lenfunc mp_لمائی;
    binaryfunc mp_subscript;
    objobjargproc mp_ass_subscript;
} PyMappingMethods;

// This struct emulates the concrete _typeobject struct to allow the code to
// work the same way in both limited and full Python APIs.
بنية typeobject_wrapper {
    ثابت حرف *tp_name;
    Py_ssize_t tp_basicsize;
    بنانشانی لما tp_flags;

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

    بنية Py_طریقہ_Def *tp_methods;
    بنية _typeobject *tp_ادھار;
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
# define Py_Object_سر_INIT_TYPE(type) Py_Object_سر_INIT(عدم)
# define Py_Object_انت_INIT_TYPE(obj, type) obj.ob_ادھار.ob_type = type##Ptr

# define Py_TYPE_GET_TP_ALLOC(type) ((allocfunc)Py_Type_Get_تھاں(type, Py_tp_alloc))
# define Py_TYPE_GET_TP_METHODS(type) ((Py_طریقہ_Def *)Py_Type_Get_تھاں(type, Py_tp_methods))

// PyObject_NEW is not part of stable ABI, but PyObject_Malloc/Init are.
ساكن PyObject* Vim_PyObject_New(PyTypeObject *type, size_t objsize)
{
    PyObject *obj = (PyObject *)PyObject_Malloc(objsize);
    إذا (obj == عدم)
	أرجع Py_Err_No_حافظہ();
    أرجع PyObject_Init(obj, type);
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
    افعل { \
	PyObject* qualname = PyObject_GetAttrString((PyObject*)(obj)->ob_type, "__qualname__"); \
	إذا (qualname == عدم) \
	{ \
	    PyErr_FORMAT(PyExc_TypeError, پیغام, "(NULL)"); \
	    اكسر; \
	} \
	PyObject* module = PyObject_GetAttrString((PyObject*)(obj)->ob_type, "__module__"); \
	PyObject* بھریا; \
	إذا (module == عدم || PyUnicode_CompareWithASCIIString(module, "builtins") == 0 \
		|| PyUnicode_CompareWithASCIIString(module, "__main__") == 0) \
	{ \
	    بھریا = qualname; \
	    Py_INCREF(بھریا); \
	} \
	وإلا \
	    بھریا = PyUnicode_FromFormat("%U.%U", module, qualname); \
	PyObject* بھریا_بائٹاں = PyUnicode_AsUTF8String(بھریا); \
	ثابت حرف* بھریا_str = Py_بائٹاں_As_String(بھریا_بائٹاں); \
	بھریا_str = بھریا_str == عدم ? "(NULL)" : بھریا_str; \
	PyErr_FORMAT(PyExc_TypeError, پیغام, بھریا_str); \
	Py_DECREF(qualname); \
	Py_XDECREF(module); \
	Py_XDECREF(بھریا); \
	Py_XDECREF(بھریا_بائٹاں); \
    } طالما (0)

# define Py_List_GET_شے(list, i) Py_List_Get_شے(list, i)
# define Py_List_GET_ناپ(o) Py_List_ناپ(o)

// PyList_SET_ITEM and PyList_SetItem have slightly different behaviors. The
// former will leave the old item dangling, and the latter will decref on it.
// Since we only use this on new lists, this difference doesn't matter.
# define Py_List_سیٹ_شے(list, i, شے) Py_List_سیٹ_شے_2(list, i, شے)

# define Py_Tuple_GET_شے(o, pos) Py_Tuple_Get_شے(o, pos)
# define Py_Tuple_GET_ناپ(o) Py_Tuple_ناپ(o)

# إذا Py_LIMITED_API < 0x03080000
// PyIter_check only became part of stable ABI in 3.8, and there is no easy way
// to check for it in the API. We simply return false as a compromise. This
// does mean we should avoid compiling with stable ABI < 3.8.
#  undef PyIter_Check
#  define PyIter_Check(obj) (FALSE)
# endif

ساكن PyTypeObject* جوڑو_ڈھیر_Type(بنية typeobject_wrapper* type_object)
{
    PyType_Spec type_spec;
    type_spec.name = type_object->tp_name;
    type_spec.basicsize = type_object->tp_basicsize;
    type_spec.itemsize = 0;
    type_spec.flags = type_object->tp_flags;

    // We just need to statically allocate a large enough buffer that can hold
    // all slots. We need to leave a null-terminated slot at the end.
    Py_Type_تھاں slots[40] = { {0, عدم} };
    size_t تھاں_i = 0;

# define جوڑو_TP_تھاں(تھاں_name) \
    إذا (تھاں_i >= 40) أرجع عدم; /* this should never happen */ \
    إذا (type_object->تھاں_name != عدم) \
    { \
	slots[تھاں_i].تھاں = Py_##slot_name; \
	slots[تھاں_i].pfunc = (فراغ*)type_object->تھاں_name; \
	++تھاں_i; \
    }
# define جوڑو_TP_SUB_تھاں(sub_تھاں, تھاں_name) \
    إذا (تھاں_i >= 40) أرجع عدم; /* this should never happen */ \
    إذا (type_object->sub_تھاں != عدم && type_object->sub_تھاں->تھاں_name != عدم) \
    { \
	slots[تھاں_i].تھاں = Py_##slot_name; \
	slots[تھاں_i].pfunc = (فراغ*)type_object->sub_تھاں->تھاں_name; \
	++تھاں_i; \
    }

    جوڑو_TP_تھاں(tp_dealloc)
    جوڑو_TP_تھاں(tp_repr)
    جوڑو_TP_تھاں(tp_call)
    جوڑو_TP_تھاں(tp_getattro)
    جوڑو_TP_تھاں(tp_setattro)
    جوڑو_TP_تھاں(tp_doc)
    جوڑو_TP_تھاں(tp_traverse)
    جوڑو_TP_تھاں(tp_clear)
    جوڑو_TP_تھاں(tp_iter)
    جوڑو_TP_تھاں(tp_iternext)
    جوڑو_TP_تھاں(tp_methods)
    جوڑو_TP_تھاں(tp_ادھار)
    جوڑو_TP_تھاں(tp_alloc)
    جوڑو_TP_تھاں(tp_new)
    جوڑو_TP_تھاں(tp_free)

    جوڑو_TP_SUB_تھاں(tp_as_sequence, sq_لمائی)
    جوڑو_TP_SUB_تھاں(tp_as_sequence, sq_concat)
    جوڑو_TP_SUB_تھاں(tp_as_sequence, sq_repeat)
    جوڑو_TP_SUB_تھاں(tp_as_sequence, sq_شے)
    جوڑو_TP_SUB_تھاں(tp_as_sequence, sq_ass_شے)
    جوڑو_TP_SUB_تھاں(tp_as_sequence, sq_شامل_اے)
    جوڑو_TP_SUB_تھاں(tp_as_sequence, sq_inplace_concat)
    جوڑو_TP_SUB_تھاں(tp_as_sequence, sq_inplace_repeat)

    جوڑو_TP_SUB_تھاں(tp_as_mapping, mp_لمائی)
    جوڑو_TP_SUB_تھاں(tp_as_mapping, mp_subscript)
    جوڑو_TP_SUB_تھاں(tp_as_mapping, mp_ass_subscript)
# undef جوڑو_TP_تھاں
# undef جوڑو_TP_SUB_تھاں

    type_spec.slots = slots;

    PyObject* newtype = PyType_FromSpec(&type_spec);
    أرجع (PyTypeObject*)newtype;
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
    إذا (type##Ptr == عدم) \
	أرجع -1;
# define PYTYPE_CLEANUP(type) \
    type##Ptr = عدم;

// Limited API does not provide PyRun_* functions. Need to implement manually
// using PyCompile and PyEval.
ساكن PyObject* Vim_Py_لگاتار_String(ثابت حرف *str, صحيح شروع, PyObject *globals, PyObject *locals)
{
    // Just pass "" for filename for now.
    PyObject* compiled = Py_CompileString(str, "", شروع);
    إذا (compiled == عدم)
	أرجع عدم;

    PyObject* eval_نتیجہ = Py_Eval_Eval_کوڈ(compiled, globals, locals);
    Py_DECREF(compiled);
    أرجع eval_نتیجہ;
}
ساكن صحيح Vim_Py_لگاتار_Simple_String(ثابت حرف *str)
{
    // This function emulates CPython's implementation.
    PyObject* m = Py_Import_جوڑو_Module("__main__");
    إذا (m == عدم)
	أرجع -1;
    PyObject* d = PyModule_GetDict(m);
    PyObject* output = Vim_Py_لگاتار_String(str, Py_file_داخلہ, d, d);
    إذا (output == عدم)
    {
	PyErr_PrintEx(TRUE);
	أرجع -1;
    }
    Py_DECREF(output);
    أرجع 0;
}
# define Py_لگاتار_String Vim_Py_لگاتار_String
# define Py_لگاتار_Simple_String Vim_Py_لگاتار_Simple_String

#وإلا // !defined(USE_LIMITED_API)

// Full Python API. Can make use of structs and macros directly.
# define DEFINE_PY_TYPE_OBJECT(type) \
    ساكن PyTypeObject type; \
    ساكن PyTypeObject* type##Ptr = &type
# define Py_Object_سر_INIT_TYPE(type) Py_Object_سر_INIT(&type)

# define Py_TYPE_GET_TP_ALLOC(type) type->tp_alloc
# define Py_TYPE_GET_TP_METHODS(type) type->tp_methods

# define Py_TYPE_NAME(obj) ((obj)->ob_type->tp_name == عدم \
	? "(NULL)" \
	: (obj)->ob_type->tp_name)
# define PyErr_FORMAT_TYPE(پیغام, obj) \
    PyErr_FORMAT(PyExc_TypeError, پیغام, \
	    Py_TYPE_NAME(obj))

// Add a static type
# define PYTYPE_READY(type) \
    إذا (PyType_Ready(type##Ptr)) \
	أرجع -1;

#جے_انت


#تعریف_کرو RAISE_NO_خالی_تھاواں_کلیدیں Py_Err_سیٹ_STRING(Py_Exc_قدر_Error, \
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

تعريفنوع فراغ (*rangeinitializer)(فراغ *);
تعريفنوع فراغ (*runner)(ثابت حرف *, dict_T *, فراغ *
#جے_تعریف_شدہ PY_CAN_RECURSE
	, PyGILState_STATE *
#جے_انت
	);

ساكن صحيح ConvertFromPyObject(PyObject *, typval_T *);
ساكن صحيح _ConvertFromPyObject(PyObject *, typval_T *, PyObject *);
ساكن صحيح ConvertFromPyMapping(PyObject *, typval_T *);
ساكن صحيح ConvertFromPySequence(PyObject *, typval_T *);
ساكن PyObject *کھڑکی_New(win_T *, tabpage_T *);
ساكن PyObject *بفر_New (buf_T *);
ساكن PyObject *سطر_منزل_String(ثابت حرف *);

ساكن PyInt Range_شروع;
ساكن PyInt Range_سرا;

ساكن PyObject *globals;

ساكن PyObject *py_chdir;
ساكن PyObject *py_fchdir;
ساكن PyObject *py_getcwd;
ساكن PyObject *vim_module;
ساكن PyObject *vim_special_رستہ_object;

#إذا PY_نسخہ_HEX >= 0x030700f0
ساكن PyObject *py_لبھو_spec;
#وإلا
ساكن PyObject *py_لدو_module;
#جے_انت
#إذا PY_نسخہ_HEX < 0x30c00a7
ساكن PyObject *py_لبھو_module;
#جے_انت

ساكن PyObject *VimError;

/*
 * obtain a lock on the Vim data structures
 */
    ساكن فراغ
Python_جندرا_Vim(فراغ)
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
String_منزل_Chars(PyObject *obj, PyObject **todecref)
{
    char_u	*str;

    إذا (Py_بائٹاں_Check(obj))
    {

	إذا (Py_بائٹاں_As_String_And_ناپ(obj, (حرف **) &str, عدم) == -1
		|| str == عدم)
	    أرجع عدم;

	*todecref = عدم;
    }
    وإلا إذا (PyUnicode_Check(obj))
    {
	PyObject	*بائٹاں;

	إذا (!(بائٹاں = PyUnicode_AsEncodedString(obj, ENC_OPT,
							   ERRORS_ENCODE_ARG)))
	    أرجع عدم;

	إذا (Py_بائٹاں_As_String_And_ناپ(بائٹاں, (حرف **) &str, عدم) == -1
		|| str == عدم)
	{
	    Py_DECREF(بائٹاں);
	    أرجع عدم;
	}

	*todecref = بائٹاں;
    }
    وإلا
    {
#إذا PY_MAJOR_نسخہ < 3
	PyErr_FORMAT_TYPE(N_("expected str() or unicode() instance, but got %s"),
		obj);
#وإلا
	PyErr_FORMAT_TYPE(N_("expected bytes() or str() instance, but got %s"),
		obj);
#جے_انت
	أرجع عدم;
    }

    أرجع (char_u *) str;
}

#تعریف_کرو نمبر_LONG     1
#تعریف_کرو نمبر_INT      2
#تعریف_کرو نمبر_NATURAL  4
#تعریف_کرو نمبر_UNSIGNED 8

    ساكن صحيح
نمبر_منزل_Long(PyObject *obj, لما *نتیجہ, صحيح flags)
{
#إذا PY_MAJOR_نسخہ < 3
    إذا (PyInt_Check(obj))
    {
	*نتیجہ = PyInt_AsLong(obj);
	إذا (PyErr_Occurred())
	    أرجع -1;
    }
    وإلا
#جے_انت
    إذا (PyLong_Check(obj))
    {
	*نتیجہ = PyLong_AsLong(obj);
	إذا (PyErr_Occurred())
	    أرجع -1;
    }
    وإلا إذا (Py_نمبر_Check(obj))
    {
	PyObject	*عدد;

	إذا (!(عدد = Py_نمبر_Long(obj)))
	    أرجع -1;

	*نتیجہ = PyLong_AsLong(عدد);

	Py_DECREF(عدد);

	إذا (PyErr_Occurred())
	    أرجع -1;
    }
    وإلا
    {
#إذا PY_MAJOR_نسخہ < 3
	PyErr_FORMAT_TYPE(
		N_("expected int(), long() or something supporting "
		   "coercing to long(), but got %s"),
		obj);
#وإلا
	PyErr_FORMAT_TYPE(
		N_("expected int() or something supporting coercing to int(), "
		   "but got %s"),
		obj);
#جے_انت
	أرجع -1;
    }

    إذا (flags & نمبر_INT)
    {
	إذا (*نتیجہ > ودھ_توں_ودھ_عدد)
	{
	    Py_Err_سیٹ_STRING(Py_Exc_حد_توں_ودھ_Error,
		    N_("value is too large to fit into C int type"));
	    أرجع -1;
	}
	وإلا إذا (*نتیجہ < INT_MIN)
	{
	    Py_Err_سیٹ_STRING(Py_Exc_حد_توں_ودھ_Error,
		    N_("value is too small to fit into C int type"));
	    أرجع -1;
	}
    }

    إذا (flags & نمبر_NATURAL)
    {
	إذا (*نتیجہ <= 0)
	{
	    Py_Err_سیٹ_STRING(Py_Exc_قدر_Error,
		    N_("number must be greater than zero"));
	    أرجع -1;
	}
    }
    وإلا إذا (flags & نمبر_UNSIGNED)
    {
	إذا (*نتیجہ < 0)
	{
	    Py_Err_سیٹ_STRING(Py_Exc_قدر_Error,
		    N_("number must be greater or equal to zero"));
	    أرجع -1;
	}
    }

    أرجع 0;
}

    ساكن صحيح
جوڑو_string(PyObject *list, حرف *s)
{
    PyObject	*string;

    إذا (!(string = PyString_FromString(s)))
	أرجع -1;

    إذا (PyList_Append(list, string))
    {
	Py_DECREF(string);
	أرجع -1;
    }

    Py_DECREF(string);
    أرجع 0;
}

    ساكن PyObject *
ObjectDir(PyObject *self, حرف **گن)
{
    Py_طریقہ_Def	*طریقہ;
    حرف	**attr;
    PyObject	*ret;

    إذا (!(ret = PyList_New(0)))
	أرجع عدم;

    إذا (self)
	لكل (طریقہ = Py_TYPE_GET_TP_METHODS(self->ob_type) ; طریقہ->ml_name != عدم ; ++طریقہ)
	    إذا (جوڑو_string(ret, (حرف *)طریقہ->ml_name))
	    {
		Py_DECREF(ret);
		أرجع عدم;
	    }

    لكل (attr = گن ; *attr ; ++attr)
	إذا (جوڑو_string(ret, *attr))
	{
	    Py_DECREF(ret);
	    أرجع عدم;
	}

#إذا PY_MAJOR_نسخہ < 3
    إذا (جوڑو_string(ret, "__members__"))
    {
	Py_DECREF(ret);
	أرجع عدم;
    }
#جے_انت

    أرجع ret;
}

// Output buffer management

// Function to write a line, points to either msg() or emsg().
تعريفنوع صحيح (*writefn)(حرف *);

DEFINE_PY_TYPE_OBJECT(OutputType);

تعريفنوع بنية
{
    Py_Object_سر
    لما softspace;
    لما error;
} OutputObject;

ساكن حرف *OutputAttrs[] = {
    "softspace",
    عدم
};

    ساكن PyObject *
OutputDir(PyObject *self, PyObject *دلیلاں بن_ورتیا)
{
    أرجع ObjectDir(self, OutputAttrs);
}

    ساكن صحيح
OutputSetattr(PyObject *self_obj, حرف *name, PyObject *valObject)
{
    OutputObject *self = (OutputObject*)self_obj;
    إذا (valObject == عدم)
    {
	Py_Err_سیٹ_STRING(PyExc_AttributeError,
		N_("can't delete OutputObject attributes"));
	أرجع -1;
    }

    إذا (strcmp(name, "softspace") == 0)
    {
	إذا (نمبر_منزل_Long(valObject, &(self->softspace), نمبر_UNSIGNED))
	    أرجع -1;
	أرجع 0;
    }

    PyErr_FORMAT(PyExc_AttributeError, N_("invalid attribute: %s"), name);
    أرجع -1;
}

// Buffer IO, we write one whole line at a time.
ساكن garray_T io_ga = {0, 0, 1, 80, عدم};
ساكن writefn old_fn = عدم;

    ساكن فراغ
PythonIO_Flush(فراغ)
{
    إذا (old_fn != عدم && io_ga.ga_len > 0)
    {
	((حرف *)io_ga.ga_انکڑے)[io_ga.ga_len] = NUL;
	// We don't know what emsg_severe should be here, so ... hope?
	old_fn((حرف *)io_ga.ga_انکڑے);
    }
    io_ga.ga_len = 0;
}

    ساكن فراغ
writer(writefn fn, char_u *str, PyInt n, صحيح severe)
{
    char_u *ptr;

    // Flush when switching output function.
    إذا (fn != old_fn)
	PythonIO_Flush();
    old_fn = fn;

    // Write each NL separated line.  Text after the last NL is kept for
    // writing later.
    // For normal messages: Do not output when "got_int" was set.  This avoids
    // a loop gone crazy flooding the terminal with messages.  Also for when
    // "q" is pressed at the more-prompt.
    طالما (n > 0 && (ptr = memchr(str, '\n', n)) != عدم
					  && (fn == (writefn)emsg || !got_int))
    {
	PyInt len = ptr - str;

	إذا (ga_grow(&io_ga, (صحيح)(len + 1)) == FAIL)
	    اكسر;

	mch_memmove(((حرف *)io_ga.ga_انکڑے) + io_ga.ga_len, str, (size_t)len);
	((حرف *)io_ga.ga_انکڑے)[io_ga.ga_len + len] = NUL;
	emsg_severe = severe;
	fn((حرف *)io_ga.ga_انکڑے);
	str = ptr + 1;
	n -= len + 1;
	io_ga.ga_len = 0;
    }

    // Put the remaining text into io_ga for later printing.
    إذا (n > 0 && (fn == (writefn)emsg || !got_int)
					&& ga_grow(&io_ga, (صحيح)(n + 1)) == OK)
    {
	mch_memmove(((حرف *)io_ga.ga_انکڑے) + io_ga.ga_len, str, (size_t)n);
	io_ga.ga_len += (صحيح)n;
    }
}

    ساكن صحيح
write_output(OutputObject *self, PyObject *string)
{
    Py_ssize_t	len = 0;
    حرف	*str = عدم;
    صحيح		error = self->error;

    إذا (!PyArg_Parse(string, "et#", ENC_OPT, &str, &len))
	أرجع -1;

    Py_BEGIN_ALLOW_دھاگے
    Python_جندرا_Vim();
    writer((writefn)(error ? emsg : پیغام), (char_u *)str, len, error);
    Python_Release_Vim();
    Py_سرا_ALLOW_دھاگے
    PyMem_Free(str);

    أرجع 0;
}

    ساكن PyObject *
OutputWrite(OutputObject *self, PyObject *string)
{
    إذا (write_output(self, string))
	أرجع عدم;

    Py_INCREF(Py_None);
    أرجع Py_None;
}

    ساكن PyObject *
OutputWritelines(OutputObject *self, PyObject *seq)
{
    PyObject	*iterator;
    PyObject	*شے;

    إذا (!(iterator = PyObject_GetIter(seq)))
	أرجع عدم;

    طالما ((شے = Py_Iter_اگلا(iterator)))
    {
	إذا (write_output(self, شے))
	{
	    Py_DECREF(iterator);
	    Py_DECREF(شے);
	    أرجع عدم;
	}
	Py_DECREF(شے);
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    إذا (PyErr_Occurred())
	أرجع عدم;

    Py_INCREF(Py_None);
    أرجع Py_None;
}

    ساكن PyObject *
AlwaysNone(PyObject *self بن_ورتیا, PyObject *دلیلاں بن_ورتیا)
{
    // do nothing
    Py_INCREF(Py_None);
    أرجع Py_None;
}
#تعریف_کرو ALWAYS_NONE AlwaysNone(عدم, عدم)

    ساكن PyObject *
AlwaysFalse(PyObject *self بن_ورتیا, PyObject *دلیلاں بن_ورتیا)
{
    // do nothing
    PyObject	*ret = Py_False;
    Py_INCREF(ret);
    أرجع ret;
}
#تعریف_کرو ALWAYS_FALSE AlwaysFalse(عدم, عدم)

    ساكن PyObject *
AlwaysTrue(PyObject *self بن_ورتیا, PyObject *دلیلاں بن_ورتیا)
{
    // do nothing
    PyObject	*ret = Py_True;
    Py_INCREF(ret);
    أرجع ret;
}
#تعریف_کرو ALWAYS_TRUE AlwaysTrue(عدم, عدم)

/***************/

ساكن بنية Py_طریقہ_Def OutputMethods[] = {
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
    Py_Object_سر_INIT_TYPE(OutputType)
    0,
    0
};

ساكن OutputObject Error =
{
    Py_Object_سر_INIT_TYPE(OutputType)
    0,
    1
};

    ساكن صحيح
PythonIO_Init_io(فراغ)
{
    إذا (Py_Sys_سیٹ_Object("stdout", (PyObject *)(فراغ *)&Output))
	أرجع -1;
    إذا (Py_Sys_سیٹ_Object("stderr", (PyObject *)(فراغ *)&Error))
	أرجع -1;

    إذا (PyErr_Occurred())
    {
	emsg(_(e_python_error_initialising_io_object));
	أرجع -1;
    }

    أرجع 0;
}

#إذا PY_نسخہ_HEX < 0x030700f0
ساكن PyObject *call_لدو_module(حرف *name, صحيح len, PyObject *لبھو_module_نتیجہ);

تعريفنوع بنية
{
    Py_Object_سر
    حرف	*fullname;
    PyObject	*نتیجہ;
} LoaderObject;
DEFINE_PY_TYPE_OBJECT(LoaderType);

    ساكن فراغ
LoaderDestructor(PyObject *self_obj)
{
    LoaderObject *self = (LoaderObject*)self_obj;
    vim_free(self->fullname);
    Py_XDECREF(self->نتیجہ);
    DESTRUCTOR_انت(self);
}

    ساكن PyObject *
Loader_لدو_Module(LoaderObject *self, PyObject *دلیلاں بن_ورتیا)
{
    حرف	*fullname = self->fullname;
    PyObject	*نتیجہ = self->نتیجہ;
    PyObject	*module;

    إذا (!fullname)
    {
	module = نتیجہ ? نتیجہ : Py_None;
	Py_INCREF(module);
	أرجع module;
    }

    module = call_لدو_module(fullname, (صحيح)STRLEN(fullname), نتیجہ);

    self->fullname = عدم;
    self->نتیجہ = module;

    vim_free(fullname);
    Py_DECREF(نتیجہ);

    إذا (!module)
    {
	إذا (PyErr_Occurred())
	    أرجع عدم;

	Py_INCREF(Py_None);
	أرجع Py_None;
    }

    Py_INCREF(module);
    أرجع module;
}

ساكن بنية Py_طریقہ_Def LoaderMethods[] = {
    // name,	    function,				calling,	doc
    {"load_module", (PyCFunction)Loader_لدو_Module,	METH_VARARGS,	""},
    { عدم,	    عدم,				0,		عدم}
};
#جے_انت

/*
 * Check to see whether a Vim error has been reported, or a keyboard
 * interrupt has been detected.
 */
    ساكن فراغ
Vim_Try_شروع(فراغ)
{
    ++trylevel;
}

    ساكن صحيح
Vim_Try_سرا(فراغ)
{
    --trylevel;
    // Without this it stops processing all subsequent Vim script commands and
    // generates strange error messages if I e.g. try calling Test() in a cycle
    did_emsg = FALSE;
    // Keyboard interrupt should be preferred over anything else
    إذا (got_int)
    {
	إذا (did_throw)
	    discard_current_exception();
	got_int = FALSE;
	Py_Err_سیٹ_None(Py_Exc_Keyboard_رکاوٹ);
	أرجع -1;
    }
    وإلا إذا (پیغام_list != عدم && *پیغام_list != عدم)
    {
	صحيح	should_free;
	حرف	*پیغام;

	پیغام = get_exception_string(*پیغام_list, ET_ERROR, عدم, &should_free);

	إذا (پیغام == عدم)
	{
	    Py_Err_No_حافظہ();
	    أرجع -1;
	}

	Py_Err_سیٹ_Vim(پیغام);

	free_global_msglist();

	إذا (should_free)
	    vim_free(پیغام);

	أرجع -1;
    }
    وإلا إذا (!did_throw)
	أرجع (PyErr_Occurred() ? -1 : 0);
    // Python exception is preferred over Vim one; unlikely to occur though
    وإلا إذا (PyErr_Occurred())
    {
	discard_current_exception();
	أرجع -1;
    }
    // Finally transform Vim script exception to python one
    وإلا
    {
	Py_Err_سیٹ_Vim((حرف *)current_exception->قدر);
	discard_current_exception();
	أرجع -1;
    }
}

    ساكن صحيح
Vim_Check_رکاوٹ(فراغ)
{
    إذا (got_int)
    {
	Py_Err_سیٹ_None(Py_Exc_Keyboard_رکاوٹ);
	أرجع 1;
    }
    أرجع 0;
}

// Vim module - Implementation

    ساكن PyObject *
VimCommand(PyObject *self بن_ورتیا, PyObject *string)
{
    char_u	*cmd;
    PyObject	*ret;
    PyObject	*todecref;

    إذا (!(cmd = String_منزل_Chars(string, &todecref)))
	أرجع عدم;

    Py_BEGIN_ALLOW_دھاگے
    Python_جندرا_Vim();

    Vim_Try_شروع();
    do_cmdline_cmd(cmd);
    update_screen(UPD_VALID);

    Python_Release_Vim();
    Py_سرا_ALLOW_دھاگے

    إذا (Vim_Try_سرا())
	ret = عدم;
    وإلا
	ret = Py_None;

    Py_XINCREF(ret);
    Py_XDECREF(todecref);
    أرجع ret;
}

/*
 * Function to translate a typval_T into a PyObject; this will recursively
 * translate lists/dictionaries into their Python equivalents.
 *
 * The depth parameter is to avoid infinite recursion, set it to 1 when
 * you call VimToPython.
 */
    ساكن PyObject *
Vim_منزل_Python(typval_T *our_tv, صحيح ڈونگھائی, PyObject *lookup_dict)
{
    PyObject	*ret;
    PyObject	*newObj;
    حرف	ptrBuf[حجم(فراغ *) * 2 + 3];

    // Avoid infinite recursion
    إذا (ڈونگھائی > 100)
    {
	Py_INCREF(Py_None);
	ret = Py_None;
	أرجع ret;
    }

    // Check if we run into a recursive loop.  The item must be in lookup_dict
    // then and we can use it again.
    إذا ((our_tv->v_type == VAR_LIST && our_tv->vval.v_list != عدم)
	    || (our_tv->v_type == VAR_TUPLE && our_tv->vval.v_tuple != عدم)
	    || (our_tv->v_type == VAR_DICT && our_tv->vval.v_dict != عدم))
    {
	sprintf(ptrBuf, "%p",
		our_tv->v_type == VAR_LIST ? (فراغ *)our_tv->vval.v_list
		    : our_tv->v_type == VAR_TUPLE ?
			(فراغ *)our_tv->vval.v_tuple
			: (فراغ *)our_tv->vval.v_dict);

	إذا ((ret = Py_Dict_Get_شے_String(lookup_dict, ptrBuf)))
	{
	    Py_INCREF(ret);
	    أرجع ret;
	}
    }

    إذا (our_tv->v_type == VAR_STRING)
	ret = PyString_FromString(our_tv->vval.v_string == عدم
					? "" : (حرف *)our_tv->vval.v_string);
    وإلا إذا (our_tv->v_type == VAR_نمبر)
    {
	حرف buf[NUMBUFLEN];

	// For backwards compatibility numbers are stored as strings.
	sprintf(buf, "%lld", (لما لما)our_tv->vval.v_نمبر);
	ret = PyString_FromString((حرف *)buf);
    }
    وإلا إذا (our_tv->v_type == VAR_FLOAT)
    {
	حرف buf[NUMBUFLEN];

	sprintf(buf, "%f", our_tv->vval.v_float);
	ret = PyString_FromString((حرف *)buf);
    }
    وإلا إذا (our_tv->v_type == VAR_LIST)
    {
	list_T		*list = our_tv->vval.v_list;
	listitem_T	*curr;

	إذا (list == عدم)
	    أرجع عدم;

	إذا (!(ret = PyList_New(0)))
	    أرجع عدم;

	إذا (Py_Dict_سیٹ_شے_String(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    أرجع عدم;
	}

	CHECK_LIST_MATERIALIZE(list);
	FOR_ALL_LIST_اشیاء(list, curr)
	{
	    إذا (!(newObj = Vim_منزل_Python(&curr->li_tv, ڈونگھائی + 1, lookup_dict)))
	    {
		Py_DECREF(ret);
		أرجع عدم;
	    }
	    إذا (PyList_Append(ret, newObj))
	    {
		Py_DECREF(newObj);
		Py_DECREF(ret);
		أرجع عدم;
	    }
	    Py_DECREF(newObj);
	}
    }
    وإلا إذا (our_tv->v_type == VAR_TUPLE)
    {
	tuple_T	*tuple = our_tv->vval.v_tuple;
	صحيح	len;

	إذا (tuple == عدم)
	    أرجع عدم;

	len = TUPLE_LEN(tuple);

	ret = PyTuple_New(len);
	إذا (ret == عدم)
	    أرجع عدم;

	لكل (صحيح idx = 0; idx < len; idx++)
	{
	    typval_T	*شے_tv = TUPLE_شے(tuple, idx);

	    newObj = Vim_منزل_Python(شے_tv, ڈونگھائی + 1, lookup_dict);
	    إذا (!newObj)
	    {
		Py_DECREF(ret);
		أرجع عدم;
	    }
	    Py_Tuple_سیٹ_شے(ret, idx, newObj);
	}

	إذا (Py_Dict_سیٹ_شے_String(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    أرجع عدم;
	}
    }
    وإلا إذا (our_tv->v_type == VAR_DICT)
    {

	hashtab_T	*ht;
	long_u		todo;
	hashitem_T	*hi;
	dictitem_T	*di;

	إذا (our_tv->vval.v_dict == عدم)
	    أرجع عدم;
	ht = &our_tv->vval.v_dict->dv_hashtab;

	إذا (!(ret = PyDict_New()))
	    أرجع عدم;

	إذا (Py_Dict_سیٹ_شے_String(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    أرجع عدم;
	}

	todo = ht->ht_ورتیا;
	لكل (hi = ht->ht_array; todo > 0; ++hi)
	{
	    إذا (!HASHITEM_خالی_تھاواں(hi))
	    {
		--todo;

		di = dict_lookup(hi);
		إذا (!(newObj = Vim_منزل_Python(&di->di_tv, ڈونگھائی + 1, lookup_dict)))
		{
		    Py_DECREF(ret);
		    أرجع عدم;
		}
		إذا (Py_Dict_سیٹ_شے_String(ret, (حرف *)hi->hi_کلید, newObj))
		{
		    Py_DECREF(ret);
		    Py_DECREF(newObj);
		    أرجع عدم;
		}
	    }
	}
    }
    وإلا إذا (our_tv->v_type == VAR_BOOL)
    {
	إذا (our_tv->vval.v_نمبر == VVAL_FALSE)
	{
	    ret = Py_False;
	    Py_INCREF(ret);
	}
	وإلا
	{
	    ret = Py_True;
	    Py_INCREF(ret);
	}
	أرجع ret;
    }
    وإلا إذا (our_tv->v_type == VAR_SPECIAL)
    {
	Py_INCREF(Py_None);
	ret = Py_None;
	أرجع ret;
    }
    وإلا إذا (our_tv->v_type == VAR_BLOB)
	ret = Py_بائٹاں_From_String_And_ناپ(
		(حرف*) our_tv->vval.v_blob->bv_ga.ga_انکڑے,
		(Py_ssize_t) our_tv->vval.v_blob->bv_ga.ga_len);
    وإلا
    {
	Py_INCREF(Py_None);
	ret = Py_None;
    }

    أرجع ret;
}

    ساكن PyObject *
VimEval(PyObject *self بن_ورتیا, PyObject *دلیلاں)
{
    char_u	*اظہاریہ;
    typval_T	*our_tv;
    PyObject	*string;
    PyObject	*todecref;
    PyObject	*ret;
    PyObject	*lookup_dict;

    إذا (!PyArg_ParseTuple(دلیلاں, "O", &string))
	أرجع عدم;

    إذا (!(اظہاریہ = String_منزل_Chars(string, &todecref)))
	أرجع عدم;

    Py_BEGIN_ALLOW_دھاگے
    Python_جندرا_Vim();
    Vim_Try_شروع();
    our_tv = eval_اظہاریہ(اظہاریہ, عدم);
    Python_Release_Vim();
    Py_سرا_ALLOW_دھاگے

    Py_XDECREF(todecref);

    إذا (Vim_Try_سرا())
	أرجع عدم;

    إذا (our_tv == عدم)
    {
	Py_Err_سیٹ_VIM(N_("invalid expression"));
	أرجع عدم;
    }

    // Convert the Vim type into a Python type.  Create a dictionary that's
    // used to check for recursive loops.
    إذا (!(lookup_dict = PyDict_New()))
	ret = عدم;
    وإلا
    {
	ret = Vim_منزل_Python(our_tv, 1, lookup_dict);
	Py_DECREF(lookup_dict);
    }


    Py_BEGIN_ALLOW_دھاگے
    Python_جندرا_Vim();
    free_tv(our_tv);
    Python_Release_Vim();
    Py_سرا_ALLOW_دھاگے

    أرجع ret;
}

ساكن PyObject *Convert_منزل_Py_Object(typval_T *);

    ساكن PyObject *
VimEvalPy(PyObject *self بن_ورتیا, PyObject *string)
{
    typval_T	*our_tv;
    PyObject	*ret;
    char_u	*اظہاریہ;
    PyObject	*todecref;

    إذا (!(اظہاریہ = String_منزل_Chars(string, &todecref)))
	أرجع عدم;

    Py_BEGIN_ALLOW_دھاگے
    Python_جندرا_Vim();
    Vim_Try_شروع();
    our_tv = eval_اظہاریہ(اظہاریہ, عدم);
    Python_Release_Vim();
    Py_سرا_ALLOW_دھاگے

    Py_XDECREF(todecref);

    إذا (Vim_Try_سرا())
	أرجع عدم;

    إذا (our_tv == عدم)
    {
	Py_Err_سیٹ_VIM(N_("invalid expression"));
	أرجع عدم;
    }

    ret = Convert_منزل_Py_Object(our_tv);
    Py_BEGIN_ALLOW_دھاگے
    Python_جندرا_Vim();
    free_tv(our_tv);
    Python_Release_Vim();
    Py_سرا_ALLOW_دھاگے

    أرجع ret;
}

    ساكن PyObject *
VimStrwidth(PyObject *self بن_ورتیا, PyObject *string)
{
    char_u	*str;
    PyObject	*todecref;
    صحيح		len;

    إذا (!(str = String_منزل_Chars(string, &todecref)))
	أرجع عدم;

    len = mb_string2cells(str, (صحيح)STRLEN(str));

    Py_XDECREF(todecref);

    أرجع PyLong_FromLong(len);
}

    ساكن PyObject *
_VimChdir(PyObject *_chdir, PyObject *دلیلاں, PyObject *kwargs)
{
    PyObject	*ret;
    PyObject	*newwd;
    PyObject	*todecref;
    char_u	*new_dir;

    إذا (_chdir == عدم)
	أرجع عدم;
    إذا (!(ret = PyObject_Call(_chdir, دلیلاں, kwargs)))
	أرجع عدم;

    إذا (!(newwd = Py_Object_Call_Function_Obj_دلیلاں(py_getcwd, عدم)))
    {
	Py_DECREF(ret);
	أرجع عدم;
    }

    إذا (!(new_dir = String_منزل_Chars(newwd, &todecref)))
    {
	Py_DECREF(ret);
	Py_DECREF(newwd);
	أرجع عدم;
    }

    Vim_Try_شروع();

    إذا (vim_chdir(new_dir))
    {
	Py_DECREF(ret);
	Py_DECREF(newwd);
	Py_XDECREF(todecref);

	إذا (Vim_Try_سرا())
	    أرجع عدم;

	Py_Err_سیٹ_VIM(N_("failed to change directory"));
	أرجع عدم;
    }

    Py_DECREF(newwd);
    Py_XDECREF(todecref);

    post_chdir(CDSCOPE_GLOBAL);

    إذا (Vim_Try_سرا())
    {
	Py_DECREF(ret);
	أرجع عدم;
    }

    أرجع ret;
}

    ساكن PyObject *
VimChdir(PyObject *self بن_ورتیا, PyObject *دلیلاں, PyObject *kwargs)
{
    أرجع _VimChdir(py_chdir, دلیلاں, kwargs);
}

    ساكن PyObject *
VimFchdir(PyObject *self بن_ورتیا, PyObject *دلیلاں, PyObject *kwargs)
{
    أرجع _VimChdir(py_fchdir, دلیلاں, kwargs);
}

تعريفنوع بنية {
    PyObject	*callable;
    PyObject	*نتیجہ;
} map_rtp_انکڑے;

    ساكن فراغ
map_rtp_callback(char_u *رستہ, فراغ *انکڑے_3)
{
    فراغ	**انکڑے = (فراغ **) انکڑے_3;
    PyObject	*رستہ_Object;
    map_rtp_انکڑے	*mr_انکڑے = *((map_rtp_انکڑے **) انکڑے);

    إذا (!(رستہ_Object = PyString_FromString((حرف *)رستہ)))
    {
	*انکڑے = عدم;
	أرجع;
    }

    mr_انکڑے->نتیجہ = Py_Object_Call_Function_Obj_دلیلاں(mr_انکڑے->callable,
						   رستہ_Object, عدم);

    Py_DECREF(رستہ_Object);

    إذا (!mr_انکڑے->نتیجہ || mr_انکڑے->نتیجہ != Py_None)
	*انکڑے = عدم;
    وإلا
    {
	Py_DECREF(mr_انکڑے->نتیجہ);
	mr_انکڑے->نتیجہ = عدم;
    }
}

    ساكن PyObject *
VimForeachRTP(PyObject *self بن_ورتیا, PyObject *callable)
{
    map_rtp_انکڑے	انکڑے;

    انکڑے.callable = callable;
    انکڑے.نتیجہ = عدم;

    do_وچ_runtimepath(عدم, 0, &map_rtp_callback, &انکڑے);

    إذا (انکڑے.نتیجہ == عدم)
    {
	إذا (PyErr_Occurred())
	    أرجع عدم;
	وإلا
	{
	    Py_INCREF(Py_None);
	    أرجع Py_None;
	}
    }
    أرجع انکڑے.نتیجہ;
}

/*
 * _vim_runtimepath_ special path implementation.
 */

    ساكن فراغ
map_finder_callback(char_u *رستہ, فراغ *انکڑے_3)
{
    فراغ	**انکڑے = (فراغ **) انکڑے_3;
    PyObject	*list = *((PyObject **) انکڑے);
    PyObject	*رستہ_Object1, *رستہ_Object2;
    حرف	*pathbuf;
    size_t	pathlen;

    pathlen = STRLEN(رستہ);

#إذا PY_MAJOR_نسخہ < 3
# define PY_MAIN_DIR_STRING "python2"
#وإلا
# define PY_MAIN_DIR_STRING "python3"
#جے_انت
#تعریف_کرو PY_ALTERNATE_DIR_STRING "pythonx"

#تعریف_کرو PYTHONX_STRING_لمائی 7 // STRLEN("pythonx")
    إذا (!(pathbuf = PyMem_New(حرف,
		    pathlen + STRLEN(PATHSEPSTR) + PYTHONX_STRING_لمائی + 1)))
    {
	Py_Err_No_حافظہ();
	*انکڑے = عدم;
	أرجع;
    }

    mch_memmove(pathbuf, رستہ, pathlen + 1);
    جوڑو_pathsep((char_u *) pathbuf);

    pathlen = STRLEN(pathbuf);
    mch_memmove(pathbuf + pathlen, PY_MAIN_DIR_STRING,
	    PYTHONX_STRING_لمائی + 1);

    إذا (!(رستہ_Object1 = PyString_FromString(pathbuf)))
    {
	*انکڑے = عدم;
	PyMem_Free(pathbuf);
	أرجع;
    }

    mch_memmove(pathbuf + pathlen, PY_ALTERNATE_DIR_STRING,
	    PYTHONX_STRING_لمائی + 1);

    إذا (!(رستہ_Object2 = PyString_FromString(pathbuf)))
    {
	Py_DECREF(رستہ_Object1);
	PyMem_Free(pathbuf);
	*انکڑے = عدم;
	أرجع;
    }

    PyMem_Free(pathbuf);

    إذا (PyList_Append(list, رستہ_Object1)
	    || PyList_Append(list, رستہ_Object2))
	*انکڑے = عدم;

    Py_DECREF(رستہ_Object1);
    Py_DECREF(رستہ_Object2);
}

    ساكن PyObject *
Vim_GetPaths(PyObject *self بن_ورتیا, PyObject *دلیلاں بن_ورتیا)
{
    PyObject	*ret;

    إذا (!(ret = PyList_New(0)))
	أرجع عدم;

    do_وچ_runtimepath(عدم, 0, &map_finder_callback, ret);

    إذا (PyErr_Occurred())
    {
	Py_DECREF(ret);
	أرجع عدم;
    }

    أرجع ret;
}

#إذا PY_نسخہ_HEX >= 0x030700f0
    ساكن PyObject *
Finder_لبھو_Spec(PyObject *self, PyObject *دلیلاں)
{
    حرف	*fullname;
    PyObject	*paths;
    PyObject	*نشانہ = Py_None;
    PyObject	*spec;

    إذا (!PyArg_ParseTuple(دلیلاں, "s|O", &fullname, &نشانہ))
	أرجع عدم;

    إذا (!(paths = Vim_GetPaths(self, عدم)))
	أرجع عدم;

    spec = PyObject_CallFunction(py_لبھو_spec, "sOO", fullname, paths, نشانہ);

    Py_DECREF(paths);

    إذا (!spec)
    {
	إذا (PyErr_Occurred())
	    أرجع عدم;

	Py_INCREF(Py_None);
	أرجع Py_None;
    }

    أرجع spec;
}

    ساكن PyObject *
Finder_لبھو_Module(PyObject* self بن_ورتیا, PyObject* دلیلاں بن_ورتیا)
{
    // Apparently returning None works.
    Py_INCREF(Py_None);
    أرجع Py_None;
}
#وإلا
    ساكن PyObject *
call_لدو_module(حرف *name, صحيح len, PyObject *لبھو_module_نتیجہ)
{
    PyObject	*fd, *pathname, *description;

    إذا (!PyTuple_Check(لبھو_module_نتیجہ))
    {
	PyErr_FORMAT_TYPE(
		N_("expected 3-tuple as imp.find_module() result, but got %s"),
		لبھو_module_نتیجہ);
	أرجع عدم;
    }
    إذا (Py_Tuple_GET_ناپ(لبھو_module_نتیجہ) != 3)
    {
	PyErr_FORMAT(PyExc_TypeError,
		N_("expected 3-tuple as imp.find_module() result, but got "
		   "tuple of size %d"),
		(صحيح) Py_Tuple_GET_ناپ(لبھو_module_نتیجہ));
	أرجع عدم;
    }

    إذا (!(fd = Py_Tuple_GET_شے(لبھو_module_نتیجہ, 0))
	    || !(pathname = Py_Tuple_GET_شے(لبھو_module_نتیجہ, 1))
	    || !(description = Py_Tuple_GET_شے(لبھو_module_نتیجہ, 2)))
    {
	Py_Err_سیٹ_STRING(PyExc_RuntimeError,
		N_("internal error: imp.find_module returned tuple with NULL"));
	أرجع عدم;
    }

    أرجع PyObject_CallFunction(py_لدو_module,
	    "s#OOO", name, len, fd, pathname, description);
}

    ساكن PyObject *
لبھو_module(حرف *fullname, حرف *پوچھ, PyObject *new_رستہ)
{
    PyObject	*لبھو_module_نتیجہ;
    PyObject	*module;
    حرف	*dot;

    إذا ((dot = (حرف *)vim_strchr((char_u *) پوچھ, '.')))
    {
	/*
	 * There is a dot in the name: call find_module recursively without the
	 * first component
	 */
	PyObject	*newest_رستہ;
	صحيح		partlen = (صحيح) (dot - 1 - پوچھ);

	إذا (!(لبھو_module_نتیجہ = PyObject_CallFunction(py_لبھو_module,
			"s#O", پوچھ, partlen, new_رستہ)))
	{
	    إذا (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_ImportError))
		PyErr_Clear();
	    أرجع عدم;
	}

	إذا (!(module = call_لدو_module(
			fullname,
			((صحيح) (پوچھ - fullname)) + partlen,
			لبھو_module_نتیجہ)))
	{
	    Py_DECREF(لبھو_module_نتیجہ);
	    أرجع عدم;
	}

	Py_DECREF(لبھو_module_نتیجہ);

	إذا (!(newest_رستہ = PyObject_GetAttrString(module, "__path__")))
	{
	    Py_DECREF(module);
	    أرجع عدم;
	}

	Py_DECREF(module);

	لبھو_module_نتیجہ = لبھو_module(fullname, dot + 1, newest_رستہ);

	Py_DECREF(newest_رستہ);

	أرجع لبھو_module_نتیجہ;
    }
    وإلا
    {
	إذا (!(لبھو_module_نتیجہ = PyObject_CallFunction(py_لبھو_module,
			"sO", پوچھ, new_رستہ)))
	{
	    إذا (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_ImportError))
		PyErr_Clear();
	    أرجع عدم;
	}

	أرجع لبھو_module_نتیجہ;
    }
}

    ساكن PyObject *
Finder_لبھو_Module(PyObject *self, PyObject *دلیلاں)
{
    حرف	*fullname;
    PyObject	*نتیجہ;
    PyObject	*new_رستہ;
    LoaderObject	*loader;

    إذا (!PyArg_ParseTuple(دلیلاں, "s", &fullname))
	أرجع عدم;

    إذا (!(new_رستہ = Vim_GetPaths(self, عدم)))
	أرجع عدم;

    نتیجہ = لبھو_module(fullname, fullname, new_رستہ);

    Py_DECREF(new_رستہ);

    إذا (!نتیجہ)
    {
	إذا (PyErr_Occurred())
	    أرجع عدم;

	Py_INCREF(Py_None);
	أرجع Py_None;
    }

    إذا (!(fullname = (حرف *)vim_strsave((char_u *)fullname)))
    {
	Py_DECREF(نتیجہ);
	Py_Err_No_حافظہ();
	أرجع عدم;
    }

    إذا (!(loader = PyObject_NEW(LoaderObject, LoaderTypePtr)))
    {
	vim_free(fullname);
	Py_DECREF(نتیجہ);
	أرجع عدم;
    }

    loader->fullname = fullname;
    loader->نتیجہ = نتیجہ;

    أرجع (PyObject *) loader;
}
#جے_انت

    ساكن PyObject *
Vim_رستہ_Hook(PyObject *self بن_ورتیا, PyObject *دلیلاں)
{
    حرف	*رستہ;

    إذا (PyArg_ParseTuple(دلیلاں, "s", &رستہ)
	    && STRCMP(رستہ, vim_special_رستہ) == 0)
    {
	Py_INCREF(vim_module);
	أرجع vim_module;
    }

    PyErr_Clear();
    Py_Err_سیٹ_None(PyExc_ImportError);
    أرجع عدم;
}

/*
 * Vim module - Definitions
 */

ساكن بنية Py_طریقہ_Def VimMethods[] = {
    // name,	    function,			calling,			documentation
    {"command",	    VimCommand,			METH_O,				"Execute a Vim ex-mode command" },
    {"eval",	    VimEval,			METH_VARARGS,			"Evaluate an expression using Vim evaluator" },
    {"bindeval",    VimEvalPy,			METH_O,				"Like eval(), but returns objects attached to Vim ones"},
    {"strwidth",    VimStrwidth,		METH_O,				"Screen string width, counts <Tab> as having width 1"},
    {"chdir",	    (PyCFunction)(فراغ *)VimChdir,	METH_VARARGS|METH_KEYWORDS,	"Change directory"},
    {"fchdir",	    (PyCFunction)(فراغ *)VimFchdir,	METH_VARARGS|METH_KEYWORDS,	"Change directory"},
    {"foreach_rtp", VimForeachRTP,		METH_O,				"Call given callable for each path in &rtp"},
#إذا PY_نسخہ_HEX >= 0x030700f0
    {"find_spec",   Finder_لبھو_Spec,		METH_VARARGS,			"Internal use only, returns spec object for any input it receives"},
#جے_انت
    {"find_module", Finder_لبھو_Module,		METH_VARARGS,			"Internal use only, returns loader object for any input it receives"},
    {"path_hook",   Vim_رستہ_Hook,		METH_VARARGS,			"Hook function to install in sys.path_hooks"},
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
    Py_Object_سر
    فراغ *cur;
    nextfun اگلا;
    destructorfun destruct;
    traversefun traverse;
    clearfun clear;
    PyObject *iter_object;
} IterObject;

    ساكن PyObject *
IterNew(فراغ *شروع, destructorfun destruct, nextfun اگلا, traversefun traverse,
	clearfun clear, PyObject *iter_object)
{
    IterObject *self;

    self = PyObject_GC_New(IterObject, IterTypePtr);
    self->cur = شروع;
    self->اگلا = اگلا;
    self->destruct = destruct;
    self->traverse = traverse;
    self->clear = clear;
    self->iter_object = iter_object;

    إذا (iter_object)
	Py_INCREF(iter_object);

    أرجع (PyObject *)(self);
}

    ساكن فراغ
IterDestructor(PyObject *self_obj)
{
    IterObject *self = (IterObject*)self_obj;
    إذا (self->iter_object)
	Py_DECREF(self->iter_object);
    PyObject_GC_UnTrack((فراغ *)(self));
    self->destruct(self->cur);
    PyObject_GC_Del((فراغ *)(self));
}

    ساكن صحيح
IterTraverse(PyObject *self_obj, visitproc ویکھو, فراغ *arg)
{
    IterObject *self = (IterObject*)self_obj;
    إذا (self->traverse != عدم)
	أرجع self->traverse(self->cur, ویکھو, arg);
    وإلا
	أرجع 0;
}

// Mac OSX defines clear() somewhere.
#جے_تعریف_شدہ clear
# undef clear
#جے_انت

    ساكن صحيح
IterClear(PyObject *self_obj)
{
    IterObject *self = (IterObject*)self_obj;
    إذا (self->clear != عدم)
	أرجع self->clear(&self->cur);
    وإلا
	أرجع 0;
}

    ساكن PyObject *
Iter_اگلا(PyObject *self_obj)
{
    IterObject *self = (IterObject*)self_obj;
    أرجع self->اگلا(&self->cur);
}

    ساكن PyObject *
IterIter(PyObject *self)
{
    Py_INCREF(self);
    أرجع self;
}

تعريفنوع بنية pylinkedlist_S {
    بنية pylinkedlist_S	*pll_اگلا;
    بنية pylinkedlist_S	*pll_prev;
    PyObject			*pll_obj;
} pylinkedlist_T;

ساكن pylinkedlist_T *lastdict = عدم;
ساكن pylinkedlist_T *lastlist = عدم;
ساكن pylinkedlist_T *lasttuple = عدم;
ساكن pylinkedlist_T *lastfunc = عدم;

    ساكن فراغ
pyll_remove(pylinkedlist_T *ref, pylinkedlist_T **آخری)
{
    إذا (ref->pll_prev == عدم)
    {
	إذا (ref->pll_اگلا == عدم)
	{
	    *آخری = عدم;
	    أرجع;
	}
    }
    وإلا
	ref->pll_prev->pll_اگلا = ref->pll_اگلا;

    إذا (ref->pll_اگلا == عدم)
	*آخری = ref->pll_prev;
    وإلا
	ref->pll_اگلا->pll_prev = ref->pll_prev;
}

    ساكن فراغ
pyll_جوڑو(PyObject *self, pylinkedlist_T *ref, pylinkedlist_T **آخری)
{
    إذا (*آخری == عدم)
	ref->pll_prev = عدم;
    وإلا
    {
	(*آخری)->pll_اگلا = ref;
	ref->pll_prev = *آخری;
    }
    ref->pll_اگلا = عدم;
    ref->pll_obj = self;
    *آخری = ref;
}

DEFINE_PY_TYPE_OBJECT(DictionaryType);

تعريفنوع بنية
{
    Py_Object_سر
    dict_T	*dict;
    pylinkedlist_T	ref;
} DictionaryObject;

ساكن PyObject *DictionaryUpdate(DictionaryObject *, PyObject *, PyObject *);

#تعریف_کرو NEW_DICTIONARY(dict) DictionaryNew(DictionaryTypePtr, dict)

    ساكن PyObject *
DictionaryNew(PyTypeObject *subtype, dict_T *dict)
{
    DictionaryObject	*self;

    self = (DictionaryObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    إذا (self == عدم)
	أرجع عدم;
    self->dict = dict;
    ++dict->dv_refcount;

    pyll_جوڑو((PyObject *)(self), &self->ref, &lastdict);

    أرجع (PyObject *)(self);
}

    ساكن dict_T *
py_dict_alloc(فراغ)
{
    dict_T	*ret;

    إذا (!(ret = dict_alloc()))
    {
	Py_Err_No_حافظہ();
	أرجع عدم;
    }
    ++ret->dv_refcount;

    أرجع ret;
}

    ساكن PyObject *
DictionaryConstructor(PyTypeObject *subtype, PyObject *دلیلاں, PyObject *kwargs)
{
    DictionaryObject	*self;
    dict_T	*dict;

    إذا (!(dict = py_dict_alloc()))
	أرجع عدم;

    self = (DictionaryObject *) DictionaryNew(subtype, dict);

    --dict->dv_refcount;

    إذا (kwargs || Py_Tuple_ناپ(دلیلاں))
    {
	PyObject	*tmp;
	إذا (!(tmp = DictionaryUpdate(self, دلیلاں, kwargs)))
	{
	    Py_DECREF(self);
	    أرجع عدم;
	}

	Py_DECREF(tmp);
    }

    أرجع (PyObject *)(self);
}

    ساكن فراغ
DictionaryDestructor(PyObject *self_obj)
{
    DictionaryObject *self = (DictionaryObject*)self_obj;
    pyll_remove(&self->ref, &lastdict);
    dict_unref(self->dict);

    DESTRUCTOR_انت(self);
}

ساكن حرف *DictionaryAttrs[] = {
    "locked", "scope",
    عدم
};

    ساكن PyObject *
DictionaryDir(PyObject *self, PyObject *دلیلاں بن_ورتیا)
{
    أرجع ObjectDir(self, DictionaryAttrs);
}

    ساكن صحيح
DictionarySetattr(PyObject *self_obj, حرف *name, PyObject *valObject)
{
    DictionaryObject *self = (DictionaryObject*)self_obj;
    إذا (valObject == عدم)
    {
	Py_Err_سیٹ_STRING(PyExc_AttributeError,
		N_("cannot delete vim.Dictionary attributes"));
	أرجع -1;
    }

    إذا (strcmp(name, "locked") == 0)
    {
	إذا (self->dict->dv_جندرا == VAR_FIXED)
	{
	    Py_Err_سیٹ_STRING(PyExc_TypeError,
		    N_("cannot modify fixed dictionary"));
	    أرجع -1;
	}
	وإلا
	{
	    صحيح		istrue = PyObject_IsTrue(valObject);
	    إذا (istrue == -1)
		أرجع -1;
	    وإلا إذا (istrue)
		self->dict->dv_جندرا = VAR_LOCKED;
	    وإلا
		self->dict->dv_جندرا = 0;
	}
	أرجع 0;
    }
    وإلا
    {
	PyErr_FORMAT(PyExc_AttributeError, N_("cannot set attribute %s"), name);
	أرجع -1;
    }
}

    ساكن PyInt
Dictionary_لمائی(DictionaryObject *self)
{
    أرجع ((PyInt) (self->dict->dv_hashtab.ht_ورتیا));
}

#تعریف_کرو DICT_FLAG_HAS_DEFAULT	0x01
#تعریف_کرو DICT_FLAG_نکالو		0x02
#تعریف_کرو DICT_FLAG_NONE_DEFAULT	0x04
#تعریف_کرو DICT_FLAG_RETURN_BOOL	0x08 // Incompatible with DICT_FLAG_POP
#تعریف_کرو DICT_FLAG_RETURN_PAIR	0x10

    ساكن PyObject *
Dictionary_شے(DictionaryObject *self, PyObject *دلیلاں, صحيح flags)
{
    PyObject	*کلید_Object;
    PyObject	*defObject = ((flags & DICT_FLAG_NONE_DEFAULT)? Py_None : عدم);
    PyObject	*ret;
    char_u	*کلید;
    dictitem_T	*di;
    dict_T	*dict = self->dict;
    hashitem_T	*hi;
    PyObject	*todecref;

    إذا (flags & DICT_FLAG_HAS_DEFAULT)
    {
	إذا (!PyArg_ParseTuple(دلیلاں, "O|O", &کلید_Object, &defObject))
	    أرجع عدم;
    }
    وإلا
	کلید_Object = دلیلاں;

    إذا (flags & DICT_FLAG_RETURN_BOOL)
	defObject = Py_False;

    إذا (!(کلید = String_منزل_Chars(کلید_Object, &todecref)))
	أرجع عدم;

    إذا (*کلید == NUL)
    {
	RAISE_NO_خالی_تھاواں_کلیدیں;
	Py_XDECREF(todecref);
	أرجع عدم;
    }

    hi = hash_لبھو(&dict->dv_hashtab, کلید);

    Py_XDECREF(todecref);

    إذا (HASHITEM_خالی_تھاواں(hi))
    {
	إذا (defObject)
	{
	    Py_INCREF(defObject);
	    أرجع defObject;
	}
	وإلا
	{
	    Py_Err_سیٹ_Object(Py_Exc_کلید_Error, کلید_Object);
	    أرجع عدم;
	}
    }
    وإلا إذا (flags & DICT_FLAG_RETURN_BOOL)
    {
	ret = Py_True;
	Py_INCREF(ret);
	أرجع ret;
    }

    di = dict_lookup(hi);

    إذا (!(ret = Convert_منزل_Py_Object(&di->di_tv)))
	أرجع عدم;

    إذا (flags & DICT_FLAG_نکالو)
    {
	إذا (dict->dv_جندرا)
	{
	    RAISE_LOCKED_DICTIONARY;
	    Py_DECREF(ret);
	    أرجع عدم;
	}

	hash_remove(&dict->dv_hashtab, hi, "Python remove variable");
	dictitem_free(di);
    }

    أرجع ret;
}

    ساكن PyObject *
Dictionary_شے_2(DictionaryObject *self, PyObject *کلید_Object)
{
    أرجع Dictionary_شے(self, کلید_Object, 0);
}

    ساكن صحيح
Dictionary_شامل_اے(DictionaryObject *self, PyObject *کلید_Object)
{
    PyObject	*rObj = Dictionary_شے(self, کلید_Object, DICT_FLAG_RETURN_BOOL);
    صحيح		ret;

    إذا (rObj == عدم)
	أرجع -1;

    ret = (rObj == Py_True);

    Py_DECREF(rObj);

    أرجع ret;
}

تعريفنوع بنية
{
    صحيح		dii_بدلیا;
    hashtab_T	*dii_ht;
    hashitem_T	*dii_hi;
    long_u	dii_todo;
} dictiterinfo_T;

    ساكن PyObject *
Dictionary_Iter_اگلا(فراغ **arg)
{
    PyObject	*ret;
    dictiterinfo_T **dii = (dictiterinfo_T**)arg;

    إذا (!(*dii)->dii_todo)
	أرجع عدم;

    إذا ((*dii)->dii_ht->ht_بدلیا != (*dii)->dii_بدلیا)
    {
	Py_Err_سیٹ_STRING(PyExc_RuntimeError,
		N_("hashtab changed during iteration"));
	أرجع عدم;
    }

    طالما (((*dii)->dii_todo) && HASHITEM_خالی_تھاواں((*dii)->dii_hi))
	++((*dii)->dii_hi);

    --((*dii)->dii_todo);

    إذا (!(ret = Py_بائٹاں_From_String((حرف *)(*dii)->dii_hi->hi_کلید)))
	أرجع عدم;

    أرجع ret;
}

    ساكن PyObject *
DictionaryIter(PyObject *self_obj)
{
    DictionaryObject	*self = (DictionaryObject*)self_obj;
    dictiterinfo_T	*dii;
    hashtab_T		*ht;

    إذا (!(dii = PyMem_New(dictiterinfo_T, 1)))
    {
	Py_Err_No_حافظہ();
	أرجع عدم;
    }

    ht = &self->dict->dv_hashtab;
    dii->dii_بدلیا = ht->ht_بدلیا;
    dii->dii_ht = ht;
    dii->dii_hi = ht->ht_array;
    dii->dii_todo = ht->ht_ورتیا;

    أرجع IterNew(dii,
	    PyMem_Free, Dictionary_Iter_اگلا,
	    عدم, عدم, (PyObject *)self);
}

    ساكن صحيح
Dictionary_Ass_شے(
	DictionaryObject *self, PyObject *کلید_Object, PyObject *valObject)
{
    char_u	*کلید;
    typval_T	tv;
    dict_T	*dict = self->dict;
    dictitem_T	*di;
    PyObject	*todecref;

    إذا (dict->dv_جندرا)
    {
	RAISE_LOCKED_DICTIONARY;
	أرجع -1;
    }

    إذا (!(کلید = String_منزل_Chars(کلید_Object, &todecref)))
	أرجع -1;

    إذا (*کلید == NUL)
    {
	RAISE_NO_خالی_تھاواں_کلیدیں;
	Py_XDECREF(todecref);
	أرجع -1;
    }

    di = dict_لبھو(dict, کلید, -1);

    إذا (valObject == عدم)
    {
	hashitem_T	*hi;

	إذا (di == عدم)
	{
	    Py_XDECREF(todecref);
	    Py_Err_سیٹ_Object(Py_Exc_کلید_Error, کلید_Object);
	    أرجع -1;
	}
	hi = hash_لبھو(&dict->dv_hashtab, di->di_کلید);
	hash_remove(&dict->dv_hashtab, hi, "Python remove item");
	dictitem_free(di);
	Py_XDECREF(todecref);
	أرجع 0;
    }

    إذا (ConvertFromPyObject(valObject, &tv) == -1)
    {
	Py_XDECREF(todecref);
	أرجع -1;
    }

    إذا (di == عدم)
    {
	إذا (!(di = dictitem_alloc(کلید)))
	{
	    Py_XDECREF(todecref);
	    Py_Err_No_حافظہ();
	    أرجع -1;
	}
	di->di_tv.v_type = VAR_UNKNOWN;

	إذا (dict_جوڑو(dict, di) == FAIL)
	{
	    dictitem_free(di);
	    RAISE_کلید_جوڑو_FAIL(کلید);
	    Py_XDECREF(todecref);
	    أرجع -1;
	}
    }
    وإلا
	clear_tv(&di->di_tv);

    Py_XDECREF(todecref);

    copy_tv(&tv, &di->di_tv);
    clear_tv(&tv);
    أرجع 0;
}

تعريفنوع PyObject *(*hi_منزل_py)(hashitem_T *);

    ساكن PyObject *
DictionaryListObjects(DictionaryObject *self, hi_منزل_py hiconvert)
{
    dict_T	*dict = self->dict;
    long_u	todo = dict->dv_hashtab.ht_ورتیا;
    Py_ssize_t	i = 0;
    PyObject	*ret;
    hashitem_T	*hi;
    PyObject	*newObj;

    ret = PyList_New(todo);
    لكل (hi = dict->dv_hashtab.ht_array; todo > 0; ++hi)
    {
	إذا (!HASHITEM_خالی_تھاواں(hi))
	{
	    إذا (!(newObj = hiconvert(hi)))
	    {
		Py_DECREF(ret);
		أرجع عدم;
	    }
	    Py_List_سیٹ_شے(ret, i, newObj);
	    --todo;
	    ++i;
	}
    }
    أرجع ret;
}

    ساكن PyObject *
dict_کلید(hashitem_T *hi)
{
    أرجع Py_بائٹاں_From_String((حرف *)(hi->hi_کلید));
}

    ساكن PyObject *
Dictionary_List_کلیدیں(DictionaryObject *self, PyObject *دلیلاں بن_ورتیا)
{
    أرجع DictionaryListObjects(self, dict_کلید);
}

    ساكن PyObject *
dict_val(hashitem_T *hi)
{
    dictitem_T	*di;

    di = dict_lookup(hi);
    أرجع Convert_منزل_Py_Object(&di->di_tv);
}

    ساكن PyObject *
Dictionary_List_اقدار(DictionaryObject *self, PyObject *دلیلاں بن_ورتیا)
{
    أرجع DictionaryListObjects(self, dict_val);
}

    ساكن PyObject *
dict_شے(hashitem_T *hi)
{
    PyObject	*کلید_Object;
    PyObject	*valObject;
    PyObject	*ret;

    إذا (!(کلید_Object = dict_کلید(hi)))
	أرجع عدم;

    إذا (!(valObject = dict_val(hi)))
    {
	Py_DECREF(کلید_Object);
	أرجع عدم;
    }

    ret = Py_Build_قدر("(OO)", کلید_Object, valObject);

    Py_DECREF(کلید_Object);
    Py_DECREF(valObject);

    أرجع ret;
}

    ساكن PyObject *
Dictionary_List_اشیاء(DictionaryObject *self, PyObject *دلیلاں بن_ورتیا)
{
    أرجع DictionaryListObjects(self, dict_شے);
}

    ساكن PyObject *
DictionaryUpdate(DictionaryObject *self, PyObject *دلیلاں, PyObject *kwargs)
{
    dict_T	*dict = self->dict;

    إذا (dict->dv_جندرا)
    {
	RAISE_LOCKED_DICTIONARY;
	أرجع عدم;
    }

    إذا (kwargs)
    {
	typval_T	tv;

	إذا (ConvertFromPyMapping(kwargs, &tv) == -1)
	    أرجع عدم;

	Vim_Try_شروع();
	dict_extend(self->dict, tv.vval.v_dict, (char_u *) "force", عدم);
	clear_tv(&tv);
	إذا (Vim_Try_سرا())
	    أرجع عدم;
    }
    وإلا
    {
	PyObject	*obj = عدم;

	إذا (!PyArg_ParseTuple(دلیلاں, "|O", &obj))
	    أرجع عدم;

	إذا (obj == عدم)
	{
	    Py_INCREF(Py_None);
	    أرجع Py_None;
	}

	إذا (PyObject_HasAttrString(obj, "keys"))
	    أرجع DictionaryUpdate(self, عدم, obj);
	وإلا
	{
	    PyObject	*iterator;
	    PyObject	*شے;

	    إذا (!(iterator = PyObject_GetIter(obj)))
		أرجع عدم;

	    طالما ((شے = Py_Iter_اگلا(iterator)))
	    {
		PyObject	*fast;
		PyObject	*کلید_Object;
		PyObject	*valObject;
		PyObject	*todecref;
		char_u		*کلید;
		dictitem_T	*di;
		hashitem_T	*hi;

		إذا (!(fast = PySequence_Fast(شے, "")))
		{
		    Py_DECREF(iterator);
		    Py_DECREF(شے);
		    أرجع عدم;
		}

		Py_DECREF(شے);

		إذا (Py_Sequence_ناپ(fast) != 2)
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    PyErr_FORMAT(Py_Exc_قدر_Error,
			    N_("expected sequence element of size 2, "
			    "but got sequence of size %d"),
			    (صحيح) Py_Sequence_ناپ(fast));
		    أرجع عدم;
		}

		کلید_Object = Py_Sequence_Get_شے(fast, 0);

		إذا (!(کلید = String_منزل_Chars(کلید_Object, &todecref)))
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    أرجع عدم;
		}

		di = dictitem_alloc(کلید);

		Py_XDECREF(todecref);

		إذا (di == عدم)
		{
		    Py_DECREF(fast);
		    Py_DECREF(iterator);
		    Py_Err_No_حافظہ();
		    أرجع عدم;
		}
		di->di_tv.v_type = VAR_UNKNOWN;

		valObject = Py_Sequence_Get_شے(fast, 1);

		إذا (ConvertFromPyObject(valObject, &di->di_tv) == -1)
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    dictitem_free(di);
		    أرجع عدم;
		}

		Py_DECREF(fast);

		hi = hash_لبھو(&dict->dv_hashtab, di->di_کلید);
		إذا (!HASHITEM_خالی_تھاواں(hi) || dict_جوڑو(dict, di) == FAIL)
		{
		    RAISE_کلید_جوڑو_FAIL(di->di_کلید);
		    Py_DECREF(iterator);
		    dictitem_free(di);
		    أرجع عدم;
		}
	    }

	    Py_DECREF(iterator);

	    // Iterator may have finished due to an exception
	    إذا (PyErr_Occurred())
		أرجع عدم;
	}
    }
    Py_INCREF(Py_None);
    أرجع Py_None;
}

    ساكن PyObject *
DictionaryGet(DictionaryObject *self, PyObject *دلیلاں)
{
    أرجع Dictionary_شے(self, دلیلاں,
			    DICT_FLAG_HAS_DEFAULT|DICT_FLAG_NONE_DEFAULT);
}

    ساكن PyObject *
Dictionary_نکالو(DictionaryObject *self, PyObject *دلیلاں)
{
    أرجع Dictionary_شے(self, دلیلاں, DICT_FLAG_HAS_DEFAULT|DICT_FLAG_نکالو);
}

    ساكن PyObject *
Dictionary_نکالو_شے(DictionaryObject *self, PyObject *دلیلاں بن_ورتیا)
{
    hashitem_T	*hi;
    PyObject	*ret;
    PyObject	*valObject;
    dictitem_T	*di;

    إذا (self->dict->dv_hashtab.ht_ورتیا == 0)
    {
	Py_Err_سیٹ_None(Py_Exc_کلید_Error);
	أرجع عدم;
    }

    hi = self->dict->dv_hashtab.ht_array;
    طالما (HASHITEM_خالی_تھاواں(hi))
	++hi;

    di = dict_lookup(hi);

    إذا (!(valObject = Convert_منزل_Py_Object(&di->di_tv)))
	أرجع عدم;

    إذا (!(ret = Py_Build_قدر("(" Py_بائٹاں_fmt "O)", hi->hi_کلید, valObject)))
    {
	Py_DECREF(valObject);
	أرجع عدم;
    }

    hash_remove(&self->dict->dv_hashtab, hi, "Python pop item");
    dictitem_free(di);

    أرجع ret;
}

    ساكن PyObject *
Dictionary_Has_کلید(DictionaryObject *self, PyObject *کلید_Object)
{
    أرجع Dictionary_شے(self, کلید_Object, DICT_FLAG_RETURN_BOOL);
}

ساكن PySequenceMethods DictionaryAsSeq = {
    0,					// sq_length
    0,					// sq_concat
    0,					// sq_repeat
    0,					// sq_item
    0,					// sq_slice
    0,					// sq_ass_item
    0,					// sq_ass_slice
    (objobjproc) Dictionary_شامل_اے,	// sq_contains
    0,					// sq_inplace_concat
    0,					// sq_inplace_repeat
};

ساكن PyMappingMethods DictionaryAsMapping = {
    (lenfunc)       Dictionary_لمائی,
    (binaryfunc)    Dictionary_شے_2,
    (objobjargproc) Dictionary_Ass_شے,
};

ساكن بنية Py_طریقہ_Def DictionaryMethods[] = {
    {"keys",	(PyCFunction)Dictionary_List_کلیدیں,	METH_NOARGS,	""},
    {"values",	(PyCFunction)Dictionary_List_اقدار,	METH_NOARGS,	""},
    {"items",	(PyCFunction)Dictionary_List_اشیاء,	METH_NOARGS,	""},
    {"update",	(PyCFunction)(فراغ *)DictionaryUpdate,		METH_VARARGS|METH_KEYWORDS, ""},
    {"get",	(PyCFunction)DictionaryGet,		METH_VARARGS,	""},
    {"pop",	(PyCFunction)Dictionary_نکالو,		METH_VARARGS,	""},
    {"popitem",	(PyCFunction)Dictionary_نکالو_شے,		METH_NOARGS,	""},
    {"has_key",	(PyCFunction)Dictionary_Has_کلید,		METH_O,		""},
    {"__dir__",	(PyCFunction)DictionaryDir,		METH_NOARGS,	""},
    { عدم,	عدم,					0,		عدم}
};

DEFINE_PY_TYPE_OBJECT(ListType);

تعريفنوع بنية
{
    Py_Object_سر
    list_T	*list;
    pylinkedlist_T	ref;
} ListObject;

#تعریف_کرو NEW_LIST(list) ListNew(ListTypePtr, list)

    ساكن PyObject *
ListNew(PyTypeObject *subtype, list_T *list)
{
    ListObject	*self;

    إذا (list == عدم)
	أرجع عدم;

    self = (ListObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    إذا (self == عدم)
	أرجع عدم;
    self->list = list;
    ++list->lv_refcount;
    CHECK_LIST_MATERIALIZE(list);

    pyll_جوڑو((PyObject *)(self), &self->ref, &lastlist);

    أرجع (PyObject *)(self);
}

    ساكن list_T *
py_list_alloc(فراغ)
{
    list_T	*ret;

    إذا (!(ret = list_alloc()))
    {
	Py_Err_No_حافظہ();
	أرجع عدم;
    }
    ++ret->lv_refcount;

    أرجع ret;
}

    ساكن صحيح
list_py_concat(list_T *l, PyObject *obj, PyObject *lookup_dict)
{
    PyObject	*iterator;
    PyObject	*شے;
    listitem_T	*li;

    إذا (!(iterator = PyObject_GetIter(obj)))
	أرجع -1;

    طالما ((شے = Py_Iter_اگلا(iterator)))
    {
	إذا (!(li = listitem_alloc()))
	{
	    Py_Err_No_حافظہ();
	    Py_DECREF(شے);
	    Py_DECREF(iterator);
	    أرجع -1;
	}
	li->li_tv.v_جندرا = 0;
	li->li_tv.v_type = VAR_UNKNOWN;

	إذا (_ConvertFromPyObject(شے, &li->li_tv, lookup_dict) == -1)
	{
	    Py_DECREF(شے);
	    Py_DECREF(iterator);
	    listitem_free(l, li);
	    أرجع -1;
	}

	Py_DECREF(شے);

	list_append(l, li);
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    إذا (PyErr_Occurred())
	أرجع -1;

    أرجع 0;
}

    ساكن PyObject *
ListConstructor(PyTypeObject *subtype, PyObject *دلیلاں, PyObject *kwargs)
{
    list_T	*list;
    PyObject	*obj = عدم;

    إذا (kwargs)
    {
	Py_Err_سیٹ_STRING(PyExc_TypeError,
		N_("list constructor does not accept keyword arguments"));
	أرجع عدم;
    }

    إذا (!PyArg_ParseTuple(دلیلاں, "|O", &obj))
	أرجع عدم;

    إذا (!(list = py_list_alloc()))
	أرجع عدم;

    إذا (obj)
    {
	PyObject	*lookup_dict;

	إذا (!(lookup_dict = PyDict_New()))
	{
	    list_unref(list);
	    أرجع عدم;
	}

	إذا (list_py_concat(list, obj, lookup_dict) == -1)
	{
	    Py_DECREF(lookup_dict);
	    list_unref(list);
	    أرجع عدم;
	}

	Py_DECREF(lookup_dict);
    }

    أرجع ListNew(subtype, list);
}

    ساكن فراغ
ListDestructor(PyObject *self_obj)
{
    ListObject *self = (ListObject*)self_obj;
    pyll_remove(&self->ref, &lastlist);
    list_unref(self->list);

    DESTRUCTOR_انت(self);
}

    ساكن PyInt
List_لمائی(ListObject *self)
{
    أرجع ((PyInt) (self->list->lv_len));
}

    ساكن PyObject *
List_اشاریہ(ListObject *self, Py_ssize_t اشاریہ)
{
    listitem_T	*li;

    إذا (اشاریہ >= List_لمائی(self))
    {
	Py_Err_سیٹ_STRING(Py_Exc_اشاریہ_Error, N_("list index out of range"));
	أرجع عدم;
    }
    li = list_لبھو(self->list, (لما) اشاریہ);
    إذا (li == عدم)
    {
	// No more suitable format specifications in python-2.3
	PyErr_VIM_FORMAT(N_("internal error: failed to get Vim list item %d"),
		(صحيح) اشاریہ);
	أرجع عدم;
    }
    أرجع Convert_منزل_Py_Object(&li->li_tv);
}

    ساكن PyObject *
List_ٹکڑا(ListObject *self, Py_ssize_t پہلا, Py_ssize_t قدم,
	  Py_ssize_t slicelen)
{
    PyInt	i;
    PyObject	*list;

    إذا (قدم == 0)
    {
	Py_Err_سیٹ_STRING(Py_Exc_قدر_Error, N_("slice step cannot be zero"));
	أرجع عدم;
    }

    list = PyList_New(slicelen);
    إذا (list == عدم)
	أرجع عدم;

    لكل (i = 0; i < slicelen; ++i)
    {
	PyObject	*شے;

	شے = List_اشاریہ(self, پہلا + i*قدم);
	إذا (شے == عدم)
	{
	    Py_DECREF(list);
	    أرجع عدم;
	}

	Py_List_سیٹ_شے(list, i, شے);
    }

    أرجع list;
}

    ساكن PyObject *
List_شے(ListObject *self, PyObject* idx)
{
#إذا PY_MAJOR_نسخہ < 3
    إذا (PyInt_Check(idx))
    {
	لما _idx = PyInt_AsLong(idx);
	أرجع List_اشاریہ(self, _idx);
    }
    وإلا
#جے_انت
    إذا (PyLong_Check(idx))
    {
	لما _idx = PyLong_AsLong(idx);
	أرجع List_اشاریہ(self, _idx);
    }
    وإلا إذا (Py_ٹکڑا_Check(idx))
    {
	Py_ssize_t شروع, stop, قدم, slicelen;

	إذا (Py_ٹکڑا_Get_Indices_Ex((Py_ٹکڑا_Object_T *)idx, List_لمائی(self),
				 &شروع, &stop, &قدم, &slicelen) < 0)
	    أرجع عدم;
	أرجع List_ٹکڑا(self, شروع, قدم, slicelen);
    }
    وإلا
    {
	RAISE_INVALID_اشاریہ_TYPE(idx);
	أرجع عدم;
    }
}

    ساكن فراغ
list_restore(Py_ssize_t numadded, Py_ssize_t numreplaced, Py_ssize_t slicelen,
	list_T *l, listitem_T **lis, listitem_T *lastaddedli)
{
    طالما (numreplaced--)
    {
	list_داخل_کرو(l, lis[numreplaced], lis[slicelen + numreplaced]);
	listitem_remove(l, lis[slicelen + numreplaced]);
    }
    طالما (numadded--)
    {
	listitem_T	*اگلا;

	اگلا = lastaddedli->li_prev;
	listitem_remove(l, lastaddedli);
	lastaddedli = اگلا;
    }
}

    ساكن صحيح
List_Ass_ٹکڑا(ListObject *self, Py_ssize_t پہلا,
	     Py_ssize_t قدم, Py_ssize_t slicelen, PyObject *obj)
{
    PyObject	*iterator;
    PyObject	*شے;
    listitem_T	*li;
    listitem_T	*lastaddedli = عدم;
    listitem_T	*اگلا;
    typval_T	v;
    list_T	*l = self->list;
    PyInt	i;
    PyInt	j;
    PyInt	numreplaced = 0;
    PyInt	numadded = 0;
    PyInt	ناپ;
    listitem_T	**lis = عدم;

    ناپ = List_لمائی(self);

    إذا (l->lv_جندرا)
    {
	RAISE_LOCKED_LIST;
	أرجع -1;
    }

    إذا (قدم == 0)
    {
	Py_Err_سیٹ_STRING(Py_Exc_قدر_Error, N_("slice step cannot be zero"));
	أرجع -1;
    }

    إذا (قدم != 1 && slicelen == 0)
    {
	// Nothing to do. Only error out if obj has some items.
	صحيح		ret = 0;

	إذا (obj == عدم)
	    أرجع 0;

	إذا (!(iterator = PyObject_GetIter(obj)))
	    أرجع -1;

	إذا ((شے = Py_Iter_اگلا(iterator)))
	{
	    PyErr_FORMAT(Py_Exc_قدر_Error,
		    N_("attempt to assign sequence of size greater than %d "
			"to extended slice"), 0);
	    Py_DECREF(شے);
	    ret = -1;
	}
	Py_DECREF(iterator);
	أرجع ret;
    }

    إذا (obj != عدم)
	// XXX May allocate zero bytes.
	إذا (!(lis = PyMem_New(listitem_T *, slicelen * 2)))
	{
	    Py_Err_No_حافظہ();
	    أرجع -1;
	}

    إذا (پہلا == ناپ)
	li = عدم;
    وإلا
    {
	li = list_لبھو(l, (لما) پہلا);
	إذا (li == عدم)
	{
	    PyErr_VIM_FORMAT(N_("internal error: no Vim list item %d"),
		    (صحيح)پہلا);
	    إذا (obj != عدم)
		PyMem_Free(lis);
	    أرجع -1;
	}
	i = slicelen;
	طالما (i-- && li != عدم)
	{
	    j = قدم;
	    اگلا = li;
	    إذا (قدم > 0)
		طالما (اگلا != عدم && ((اگلا = اگلا->li_اگلا) != عدم) && --j);
	    وإلا
		طالما (اگلا != عدم && ((اگلا = اگلا->li_prev) != عدم) && ++j);

	    إذا (obj == عدم)
		listitem_remove(l, li);
	    وإلا
		lis[slicelen - i - 1] = li;

	    li = اگلا;
	}
	إذا (li == عدم && i != -1)
	{
	    Py_Err_سیٹ_VIM(N_("internal error: not enough list items"));
	    إذا (obj != عدم)
		PyMem_Free(lis);
	    أرجع -1;
	}
    }

    إذا (obj == عدم)
	أرجع 0;

    إذا (!(iterator = PyObject_GetIter(obj)))
    {
	PyMem_Free(lis);
	أرجع -1;
    }

    i = 0;
    طالما ((شے = Py_Iter_اگلا(iterator)))
    {
	إذا (ConvertFromPyObject(شے, &v) == -1)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(شے);
	    PyMem_Free(lis);
	    أرجع -1;
	}
	Py_DECREF(شے);
	إذا (list_داخل_کرو_tv(l, &v, numreplaced < slicelen
				    ? lis[numreplaced]
				    : li) == FAIL)
	{
	    clear_tv(&v);
	    Py_Err_سیٹ_VIM(N_("internal error: failed to add item to list"));
	    list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	    PyMem_Free(lis);
	    أرجع -1;
	}
	إذا (numreplaced < slicelen)
	{
	    lis[slicelen + numreplaced] = lis[numreplaced]->li_prev;
	    vimlist_remove(l, lis[numreplaced], lis[numreplaced]);
	    numreplaced++;
	}
	وإلا
	{
	    إذا (li)
		lastaddedli = li->li_prev;
	    وإلا
		lastaddedli = l->lv_u.mat.lv_آخری;
	    numadded++;
	}
	clear_tv(&v);
	إذا (قدم != 1 && i >= slicelen)
	{
	    Py_DECREF(iterator);
	    PyErr_FORMAT(Py_Exc_قدر_Error,
		    N_("attempt to assign sequence of size greater than %d "
			"to extended slice"), (صحيح) slicelen);
	    list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	    PyMem_Free(lis);
	    أرجع -1;
	}
	++i;
    }
    Py_DECREF(iterator);

    إذا (قدم != 1 && i != slicelen)
    {
	PyErr_FORMAT2(Py_Exc_قدر_Error,
		N_("attempt to assign sequence of size %d to extended slice "
		    "of size %d"), (صحيح) i, (صحيح) slicelen);
	list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	PyMem_Free(lis);
	أرجع -1;
    }

    إذا (PyErr_Occurred())
    {
	list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	PyMem_Free(lis);
	أرجع -1;
    }

    لكل (i = 0; i < numreplaced; i++)
	listitem_free(l, lis[i]);
    إذا (قدم == 1)
	لكل (i = numreplaced; i < slicelen; i++)
	    listitem_remove(l, lis[i]);

    PyMem_Free(lis);

    أرجع 0;
}

    ساكن صحيح
List_Ass_اشاریہ(ListObject *self, Py_ssize_t اشاریہ, PyObject *obj)
{
    typval_T	tv;
    list_T	*l = self->list;
    listitem_T	*li;
    Py_ssize_t	لمائی = List_لمائی(self);

    إذا (l->lv_جندرا)
    {
	RAISE_LOCKED_LIST;
	أرجع -1;
    }
    إذا (اشاریہ > لمائی || (اشاریہ == لمائی && obj == عدم))
    {
	Py_Err_سیٹ_STRING(Py_Exc_اشاریہ_Error, N_("list index out of range"));
	أرجع -1;
    }

    إذا (obj == عدم)
    {
	li = list_لبھو(l, (لما) اشاریہ);
	إذا (li == عدم)
	{
	    PyErr_VIM_FORMAT(N_("internal error: failed to get Vim "
			"list item %d"), (صحيح) اشاریہ);
	    أرجع -1;
	}
	vimlist_remove(l, li, li);
	clear_tv(&li->li_tv);
	vim_free(li);
	أرجع 0;
    }

    إذا (ConvertFromPyObject(obj, &tv) == -1)
	أرجع -1;

    إذا (اشاریہ == لمائی)
    {
	إذا (list_append_tv(l, &tv) == FAIL)
	{
	    clear_tv(&tv);
	    Py_Err_سیٹ_VIM(N_("failed to add item to list"));
	    أرجع -1;
	}
    }
    وإلا
    {
	li = list_لبھو(l, (لما) اشاریہ);
	إذا (li == عدم)
	{
	    PyErr_VIM_FORMAT(N_("internal error: failed to get Vim "
			"list item %d"), (صحيح) اشاریہ);
	    أرجع -1;
	}
	clear_tv(&li->li_tv);
	copy_tv(&tv, &li->li_tv);
	clear_tv(&tv);
    }
    أرجع 0;
}

    ساكن صحيح
List_Ass_شے(ListObject *self, PyObject *idx, PyObject *obj)
{
#إذا PY_MAJOR_نسخہ < 3
    إذا (PyInt_Check(idx))
    {
	لما _idx = PyInt_AsLong(idx);
	أرجع (صحيح)List_Ass_اشاریہ(self, _idx, obj);
    }
    وإلا
#جے_انت
    إذا (PyLong_Check(idx))
    {
	لما _idx = PyLong_AsLong(idx);
	أرجع (صحيح)List_Ass_اشاریہ(self, _idx, obj);
    }
    وإلا إذا (Py_ٹکڑا_Check(idx))
    {
	Py_ssize_t شروع, stop, قدم, slicelen;

	إذا (Py_ٹکڑا_Get_Indices_Ex((Py_ٹکڑا_Object_T *)idx, List_لمائی(self),
				 &شروع, &stop, &قدم, &slicelen) < 0)
	    أرجع -1;
	أرجع (صحيح)List_Ass_ٹکڑا(self, شروع, قدم, slicelen,
		obj);
    }
    وإلا
    {
	RAISE_INVALID_اشاریہ_TYPE(idx);
	أرجع -1;
    }
}

    ساكن PyObject *
List_Concat_وچ_رکھو(ListObject *self, PyObject *obj)
{
    list_T	*l = self->list;
    PyObject	*lookup_dict;

    إذا (l->lv_جندرا)
    {
	RAISE_LOCKED_LIST;
	أرجع عدم;
    }

    إذا (!(lookup_dict = PyDict_New()))
	أرجع عدم;

    إذا (list_py_concat(l, obj, lookup_dict) == -1)
    {
	Py_DECREF(lookup_dict);
	أرجع عدم;
    }
    Py_DECREF(lookup_dict);

    Py_INCREF((PyObject *)self);
    أرجع (PyObject *)(self);
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
List_Iter_اگلا(فراغ **arg)
{
    PyObject	*ret;
    listiterinfo_T **lii = (listiterinfo_T**)arg;

    إذا (!((*lii)->lw.lw_شے))
	أرجع عدم;

    إذا (!(ret = Convert_منزل_Py_Object(&((*lii)->lw.lw_شے->li_tv))))
	أرجع عدم;

    (*lii)->lw.lw_شے = (*lii)->lw.lw_شے->li_اگلا;

    أرجع ret;
}

    ساكن PyObject *
ListIter(PyObject *self_obj)
{
    ListObject	*self = (ListObject*)self_obj;
    listiterinfo_T	*lii;
    list_T	*l = self->list;

    إذا (!(lii = PyMem_New(listiterinfo_T, 1)))
    {
	Py_Err_No_حافظہ();
	أرجع عدم;
    }

    CHECK_LIST_MATERIALIZE(l);
    list_جوڑو_watch(l, &lii->lw);
    lii->lw.lw_شے = l->lv_پہلا;
    lii->list = l;
    ++l->lv_refcount;

    أرجع IterNew(lii,
	    ListIterDestruct, List_Iter_اگلا,
	    عدم, عدم, (PyObject *)self);
}

ساكن حرف *ListAttrs[] = {
    "locked",
    عدم
};

    ساكن PyObject *
ListDir(PyObject *self, PyObject *دلیلاں بن_ورتیا)
{
    أرجع ObjectDir(self, ListAttrs);
}

    ساكن صحيح
ListSetattr(PyObject *self_obj, حرف *name, PyObject *valObject)
{
    ListObject *self = (ListObject*)self_obj;
    إذا (valObject == عدم)
    {
	Py_Err_سیٹ_STRING(PyExc_AttributeError,
		N_("cannot delete vim.List attributes"));
	أرجع -1;
    }

    إذا (strcmp(name, "locked") == 0)
    {
	إذا (self->list->lv_جندرا == VAR_FIXED)
	{
	    Py_Err_سیٹ_STRING(PyExc_TypeError, N_("cannot modify fixed list"));
	    أرجع -1;
	}
	وإلا
	{
	    صحيح		istrue = PyObject_IsTrue(valObject);
	    إذا (istrue == -1)
		أرجع -1;
	    وإلا إذا (istrue)
		self->list->lv_جندرا = VAR_LOCKED;
	    وإلا
		self->list->lv_جندرا = 0;
	}
	أرجع 0;
    }
    وإلا
    {
	PyErr_FORMAT(PyExc_AttributeError, N_("cannot set attribute %s"), name);
	أرجع -1;
    }
}

ساكن PySequenceMethods ListAsSeq = {
    (lenfunc)		List_لمائی,	 // sq_length,	  len(x)
    (binaryfunc)	0,		 // RangeConcat, sq_concat,  x+y
    0,					 // RangeRepeat, sq_repeat,  x*n
    (PyIntArgFunc)	List_اشاریہ,	 // sq_item,	  x[i]
    0,					 // was_sq_slice,     x[i:j]
    (PyIntObjArgProc)	List_Ass_اشاریہ,	 // sq_as_item,  x[i]=v
    0,					 // was_sq_ass_slice, x[i:j]=v
    0,					 // sq_contains
    (binaryfunc)	List_Concat_وچ_رکھو,// sq_inplace_concat
    0,					 // sq_inplace_repeat
};

ساكن PyMappingMethods ListAsMapping = {
    /* mp_length	*/ (lenfunc) List_لمائی,
    /* mp_subscript     */ (binaryfunc) List_شے,
    /* mp_ass_subscript */ (objobjargproc) List_Ass_شے,
};

ساكن بنية Py_طریقہ_Def ListMethods[] = {
    {"extend",	(PyCFunction)List_Concat_وچ_رکھو,	METH_O,		""},
    {"__dir__",	(PyCFunction)ListDir,		METH_NOARGS,	""},
    { عدم,	عدم,				0,		عدم}
};

DEFINE_PY_TYPE_OBJECT(TupleType);

تعريفنوع بنية
{
    Py_Object_سر
    tuple_T	*tuple;
    pylinkedlist_T	ref;
} TupleObject;

#تعریف_کرو NEW_TUPLE(tuple) TupleNew(TupleTypePtr, tuple)

    ساكن PyObject *
TupleNew(PyTypeObject *subtype, tuple_T *tuple)
{
    TupleObject	*self;

    إذا (tuple == عدم)
	أرجع عدم;

    self = (TupleObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    إذا (self == عدم)
	أرجع عدم;
    self->tuple = tuple;
    ++tuple->tv_refcount;

    pyll_جوڑو((PyObject *)(self), &self->ref, &lasttuple);

    أرجع (PyObject *)(self);
}

    ساكن tuple_T *
py_tuple_alloc(فراغ)
{
    tuple_T	*ret;

    إذا (!(ret = tuple_alloc()))
    {
	Py_Err_No_حافظہ();
	أرجع عدم;
    }
    ++ret->tv_refcount;

    أرجع ret;
}

    ساكن صحيح
tuple_py_concat(tuple_T *t, PyObject *obj, PyObject *lookup_dict)
{
    PyObject	*iterator;
    PyObject	*شے;

    إذا (!(iterator = PyObject_GetIter(obj)))
	أرجع -1;

    طالما ((شے = Py_Iter_اگلا(iterator)))
    {
	typval_T    new_tv;

	إذا (_ConvertFromPyObject(شے, &new_tv, lookup_dict) == -1)
	{
	    Py_DECREF(شے);
	    Py_DECREF(iterator);
	    أرجع -1;
	}

	Py_DECREF(شے);

	إذا (tuple_append_tv(t, &new_tv) == FAIL)
	{
	    Py_DECREF(iterator);
	    أرجع -1;
	}
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    إذا (PyErr_Occurred())
	أرجع -1;

    أرجع 0;
}

    ساكن PyObject *
TupleConstructor(PyTypeObject *subtype, PyObject *دلیلاں, PyObject *kwargs)
{
    tuple_T	*tuple;
    PyObject	*obj = عدم;

    إذا (kwargs)
    {
	Py_Err_سیٹ_STRING(PyExc_TypeError,
		N_("tuple constructor does not accept keyword arguments"));
	أرجع عدم;
    }

    إذا (!PyArg_ParseTuple(دلیلاں, "|O", &obj))
	أرجع عدم;

    إذا (!(tuple = py_tuple_alloc()))
	أرجع عدم;

    إذا (obj)
    {
	PyObject	*lookup_dict;

	إذا (!(lookup_dict = PyDict_New()))
	{
	    tuple_unref(tuple);
	    أرجع عدم;
	}

	إذا (tuple_py_concat(tuple, obj, lookup_dict) == -1)
	{
	    Py_DECREF(lookup_dict);
	    tuple_unref(tuple);
	    أرجع عدم;
	}

	Py_DECREF(lookup_dict);
    }

    أرجع TupleNew(subtype, tuple);
}

    ساكن فراغ
TupleDestructor(PyObject *self_obj)
{
    TupleObject *self = (TupleObject *)self_obj;
    pyll_remove(&self->ref, &lasttuple);
    tuple_unref(self->tuple);

    DESTRUCTOR_انت(self);
}

    ساكن PyInt
Tuple_لمائی(TupleObject *self)
{
    أرجع ((PyInt)(tuple_len(self->tuple)));
}

    ساكن PyObject *
Tuple_اشاریہ(TupleObject *self, Py_ssize_t اشاریہ)
{
    PyInt	len = Tuple_لمائی(self);

    إذا (اشاریہ < 0)
	اشاریہ = len + اشاریہ;

    إذا (اشاریہ < 0 || اشاریہ >= len)
    {
	Py_Err_سیٹ_STRING(Py_Exc_اشاریہ_Error, N_("tuple index out of range"));
	أرجع عدم;
    }
    أرجع Convert_منزل_Py_Object(TUPLE_شے(self->tuple, اشاریہ));
}

/*
 * Return a new tuple object for the tuple slice starting from the index
 * "first" and of length "slicelen" skipping "step" items.
 */
    ساكن PyObject *
Tuple_ٹکڑا(
    TupleObject		*self,
    Py_ssize_t		پہلا,
    Py_ssize_t		قدم,
    Py_ssize_t		slicelen)
{
    PyInt	i;
    PyObject	*tuple;

    tuple = PyTuple_New(slicelen);
    إذا (tuple == عدم)
	أرجع عدم;

    لكل (i = 0; i < slicelen; ++i)
    {
	PyObject	*شے;

	شے = Tuple_اشاریہ(self, پہلا + i * قدم);
	إذا (شے == عدم)
	{
	    Py_DECREF(tuple);
	    أرجع عدم;
	}

	Py_Tuple_سیٹ_شے(tuple, i, شے);
    }

    أرجع tuple;
}

    ساكن PyObject *
Tuple_شے(TupleObject *self, PyObject* idx)
{
#إذا PY_MAJOR_نسخہ < 3
    إذا (PyInt_Check(idx))
    {
	لما _idx = PyInt_AsLong(idx);
	أرجع Tuple_اشاریہ(self, _idx);
    }
    وإلا
#جے_انت
    إذا (PyLong_Check(idx))
    {
	لما _idx = PyLong_AsLong(idx);
	أرجع Tuple_اشاریہ(self, _idx);
    }
    وإلا إذا (Py_ٹکڑا_Check(idx))
    {
	Py_ssize_t شروع, stop, قدم, slicelen;

	إذا (Py_ٹکڑا_Get_Indices_Ex((Py_ٹکڑا_Object_T *)idx, Tuple_لمائی(self),
				 &شروع, &stop, &قدم, &slicelen) < 0)
	    أرجع عدم;
	أرجع Tuple_ٹکڑا(self, شروع, قدم, slicelen);
    }
    وإلا
    {
	RAISE_INVALID_اشاریہ_TYPE(idx);
	أرجع عدم;
    }
}

تعريفنوع بنية
{
    tuple_T	*tuple;
    صحيح		اشاریہ;
} tupleiterinfo_T;

    ساكن فراغ
TupleIterDestruct(فراغ *arg)
{
    tupleiterinfo_T *tii = (tupleiterinfo_T*)arg;
    tuple_unref(tii->tuple);
    PyMem_Free(tii);
}

    ساكن PyObject *
Tuple_Iter_اگلا(فراغ **arg)
{
    PyObject	*ret;
    tupleiterinfo_T **tii = (tupleiterinfo_T**)arg;

    إذا ((*tii)->اشاریہ >= TUPLE_LEN((*tii)->tuple))
	أرجع عدم;

    إذا (!(ret = Convert_منزل_Py_Object(TUPLE_شے((*tii)->tuple, (*tii)->اشاریہ))))
	أرجع عدم;

    (*tii)->اشاریہ++;

    أرجع ret;
}

    ساكن PyObject *
TupleIter(PyObject *self_obj)
{
    TupleObject	*self = (TupleObject*)self_obj;
    tupleiterinfo_T	*tii;
    tuple_T	*t = self->tuple;

    إذا (!(tii = PyMem_New(tupleiterinfo_T, 1)))
    {
	Py_Err_No_حافظہ();
	أرجع عدم;
    }

    tii->tuple = t;
    tii->اشاریہ = 0;
    ++t->tv_refcount;

    أرجع IterNew(tii,
	    TupleIterDestruct, Tuple_Iter_اگلا,
	    عدم, عدم, (PyObject *)self);
}

ساكن حرف *TupleAttrs[] = {
    "locked",
    عدم
};

    ساكن PyObject *
TupleDir(PyObject *self, PyObject *دلیلاں بن_ورتیا)
{
    أرجع ObjectDir(self, TupleAttrs);
}

    ساكن صحيح
TupleSetattr(PyObject *self_obj, حرف *name, PyObject *valObject)
{
    TupleObject *self = (TupleObject*)self_obj;
    إذا (valObject == عدم)
    {
	Py_Err_سیٹ_STRING(PyExc_AttributeError,
		N_("cannot delete vim.Tuple attributes"));
	أرجع -1;
    }

    إذا (strcmp(name, "locked") == 0)
    {
	إذا (self->tuple->tv_جندرا == VAR_FIXED)
	{
	    Py_Err_سیٹ_STRING(PyExc_TypeError, N_("cannot modify fixed tuple"));
	    أرجع -1;
	}
	وإلا
	{
	    صحيح		istrue = PyObject_IsTrue(valObject);
	    إذا (istrue == -1)
		أرجع -1;
	    وإلا إذا (istrue)
		self->tuple->tv_جندرا = VAR_LOCKED;
	    وإلا
		self->tuple->tv_جندرا = 0;
	}
	أرجع 0;
    }
    وإلا
    {
	PyErr_FORMAT(PyExc_AttributeError, N_("cannot set attribute %s"), name);
	أرجع -1;
    }
}

ساكن PySequenceMethods TupleAsSeq = {
    (lenfunc)		Tuple_لمائی,	 // sq_length,	  len(x)
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

ساكن PyMappingMethods TupleAsMapping = {
    /* mp_length	*/ (lenfunc) Tuple_لمائی,
    /* mp_subscript     */ (binaryfunc) Tuple_شے,
    /* mp_ass_subscript */ (objobjargproc) 0,
};

ساكن بنية Py_طریقہ_Def TupleMethods[] = {
    {"__dir__",	(PyCFunction)TupleDir,		METH_NOARGS,	""},
    { عدم,	عدم,				0,		عدم}
};

تعريفنوع بنية
{
    Py_Object_سر
    char_u	*name;
    صحيح		argc;
    typval_T	*argv;
    dict_T	*self;
    pylinkedlist_T	ref;
    صحيح		auto_rebind;
} FunctionObject;

DEFINE_PY_TYPE_OBJECT(FunctionType);

#تعریف_کرو NEW_FUNCTION(name, argc, argv, self, pt_auto) \
    FunctionNew(FunctionTypePtr, (name), (argc), (argv), (self), (pt_auto))

    ساكن PyObject *
FunctionNew(PyTypeObject *subtype, char_u *name, صحيح argc, typval_T *argv,
	dict_T *selfdict, صحيح auto_rebind)
{
    FunctionObject	*self;

    self = (FunctionObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    إذا (self == عدم)
	أرجع عدم;

    إذا (ہندسہ_اے((بنانشانی حرف)*name))
    {
	إذا (!translated_function_exists(name, FALSE))
	{
	    PyErr_FORMAT(Py_Exc_قدر_Error,
		    N_("unnamed function %s does not exist"), name);
	    أرجع عدم;
	}
	self->name = vim_strsave(name);
    }
    وإلا
    {
	char_u *p;

	إذا ((p = get_expanded_name(name,
			    vim_strchr(name, AUTOLOAD_CHAR) == عدم)) == عدم)
	{
	    PyErr_FORMAT(Py_Exc_قدر_Error,
		    N_("function %s does not exist"), name);
	    أرجع عدم;
	}

	إذا (p[0] == K_SPECIAL && p[1] == KS_EXTRA && p[2] == (صحيح)KE_SNR)
	{
	    char_u *np;
	    size_t len = STRLEN(p) + 1;

	    إذا ((np = alloc(len + 2)) == عدم)
	    {
		vim_free(p);
		أرجع عدم;
	    }
	    mch_memmove(np, "<SNR>", 5);
	    mch_memmove(np + 5, p + 3, len - 3);
	    vim_free(p);
	    self->name = np;
	}
	وإلا
	    self->name = p;
    }

    func_ref(self->name);
    self->argc = argc;
    self->argv = argv;
    self->self = selfdict;
    self->auto_rebind = selfdict == عدم ? TRUE : auto_rebind;

    إذا (self->argv || self->self)
	pyll_جوڑو((PyObject *)(self), &self->ref, &lastfunc);

    أرجع (PyObject *)(self);
}

    ساكن PyObject *
FunctionConstructor(PyTypeObject *subtype, PyObject *دلیلاں, PyObject *kwargs)
{
    PyObject	*self;
    PyObject	*selfdictObject;
    PyObject	*autoRebindObject;
    PyObject	*دلیلاں_Object = عدم;
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

    إذا (kwargs != عدم)
    {
	selfdictObject = Py_Dict_Get_شے_String(kwargs, "self");
	إذا (selfdictObject != عدم)
	{
	    إذا (ConvertFromPyMapping(selfdictObject, &selfdicttv) == -1)
		أرجع عدم;
	    selfdict = selfdicttv.vval.v_dict;
	}
	دلیلاں_Object = Py_Dict_Get_شے_String(kwargs, "args");
	إذا (دلیلاں_Object != عدم)
	{
	    إذا (ConvertFromPySequence(دلیلاں_Object, &argstv) == -1)
	    {
		dict_unref(selfdict);
		أرجع عدم;
	    }
	    argslist = argstv.vval.v_list;
	    CHECK_LIST_MATERIALIZE(argslist);

	    argc = argslist->lv_len;
	    إذا (argc != 0)
	    {
		argv = PyMem_New(typval_T, (size_t) argc);
		إذا (argv == عدم)
		{
		    Py_Err_No_حافظہ();
		    dict_unref(selfdict);
		    list_unref(argslist);
		    أرجع عدم;
		}
		curtv = argv;
		FOR_ALL_LIST_اشیاء(argslist, li)
		    copy_tv(&li->li_tv, curtv++);
	    }
	    list_unref(argslist);
	}
	إذا (selfdict != عدم)
	{
	    auto_rebind = FALSE;
	    autoRebindObject = Py_Dict_Get_شے_String(kwargs, "auto_rebind");
	    إذا (autoRebindObject != عدم)
	    {
		auto_rebind = PyObject_IsTrue(autoRebindObject);
		إذا (auto_rebind == -1)
		{
		    dict_unref(selfdict);
		    list_unref(argslist);
		    أرجع عدم;
		}
	    }
	}
    }

    إذا (!PyArg_ParseTuple(دلیلاں, "et", "ascii", &name))
    {
	dict_unref(selfdict);
	طالما (argc--)
	    clear_tv(&argv[argc]);
	PyMem_Free(argv);
	أرجع عدم;
    }

    self = FunctionNew(subtype, name, argc, argv, selfdict, auto_rebind);

    PyMem_Free(name);

    أرجع self;
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
    إذا (self->argv || self->self)
	pyll_remove(&self->ref, &lastfunc);

    DESTRUCTOR_انت(self);
}

ساكن حرف *FunctionAttrs[] = {
    "softspace", "args", "self", "auto_rebind",
    عدم
};

    ساكن PyObject *
FunctionDir(PyObject *self, PyObject *دلیلاں بن_ورتیا)
{
    أرجع ObjectDir(self, FunctionAttrs);
}

    ساكن PyObject *
FunctionAttr(FunctionObject *self, حرف *name)
{
    list_T *list;
    صحيح i;
    إذا (strcmp(name, "name") == 0)
	أرجع PyString_FromString((حرف *)(self->name));
    وإلا إذا (strcmp(name, "args") == 0)
    {
	إذا (self->argv == عدم || (list = list_alloc()) == عدم)
	    أرجع ALWAYS_NONE;

	لكل (i = 0; i < self->argc; ++i)
	    list_append_tv(list, &self->argv[i]);
	أرجع NEW_LIST(list);
    }
    وإلا إذا (strcmp(name, "self") == 0)
	أرجع self->self == عدم
	    ? ALWAYS_NONE
	    : NEW_DICTIONARY(self->self);
    وإلا إذا (strcmp(name, "auto_rebind") == 0)
	أرجع self->auto_rebind
	    ? ALWAYS_TRUE
	    : ALWAYS_FALSE;
    وإلا إذا (strcmp(name, "__members__") == 0)
	أرجع ObjectDir(عدم, FunctionAttrs);
    أرجع عدم;
}

/*
 * Populate partial_T given function object.
 *
 * "exported" should be set to true when it is needed to construct a partial
 * that may be stored in a variable (i.e. may be freed by Vim).
 */
    ساكن فراغ
سیٹ_partial(FunctionObject *self, partial_T *pt, صحيح exported)
{
    صحيح i;

    pt->pt_name = self->name;
    إذا (self->argv)
    {
	pt->pt_argc = self->argc;
	إذا (exported)
	{
	    pt->pt_argv = ALLOC_CLEAR_MULT(typval_T, self->argc);
	    لكل (i = 0; i < pt->pt_argc; ++i)
		copy_tv(&self->argv[i], &pt->pt_argv[i]);
	}
	وإلا
	    pt->pt_argv = self->argv;
    }
    وإلا
    {
	pt->pt_argc = 0;
	pt->pt_argv = عدم;
    }
    pt->pt_auto = self->auto_rebind || !exported;
    pt->pt_dict = self->self;
    إذا (exported && self->self)
	++pt->pt_dict->dv_refcount;
    إذا (exported)
	pt->pt_name = vim_strsave(pt->pt_name);
    pt->pt_refcount = 1;
}

    ساكن PyObject *
FunctionCall(PyObject *self_obj, PyObject *دلیلاں_Object, PyObject *kwargs)
{
    FunctionObject *self = (FunctionObject*)self_obj;
    char_u	*name = self->name;
    typval_T	دلیلاں;
    typval_T	selfdicttv;
    typval_T	rettv;
    dict_T	*selfdict = عدم;
    PyObject	*selfdictObject;
    PyObject	*ret;
    صحيح		error;
    partial_T	pt;
    partial_T	*pt_ptr = عدم;

    إذا (ConvertFromPySequence(دلیلاں_Object, &دلیلاں) == -1)
	أرجع عدم;

    إذا (kwargs != عدم)
    {
	selfdictObject = Py_Dict_Get_شے_String(kwargs, "self");
	إذا (selfdictObject != عدم)
	{
	    إذا (ConvertFromPyMapping(selfdictObject, &selfdicttv) == -1)
	    {
		clear_tv(&دلیلاں);
		أرجع عدم;
	    }
	    selfdict = selfdicttv.vval.v_dict;
	}
    }

    إذا (self->argv || self->self)
    {
	CLEAR_میدان(pt);
	سیٹ_partial(self, &pt, FALSE);
	pt_ptr = &pt;
    }

    Py_BEGIN_ALLOW_دھاگے
    Python_جندرا_Vim();

    Vim_Try_شروع();
    error = func_call(name, &دلیلاں, pt_ptr, selfdict, &rettv);

    Python_Release_Vim();
    Py_سرا_ALLOW_دھاگے

    إذا (Vim_Try_سرا())
	ret = عدم;
    وإلا إذا (error != OK)
    {
	ret = عدم;
	PyErr_VIM_FORMAT(N_("failed to run function %s"), (حرف *)name);
    }
    وإلا
	ret = Convert_منزل_Py_Object(&rettv);

    clear_tv(&دلیلاں);
    clear_tv(&rettv);
    إذا (selfdict != عدم)
	clear_tv(&selfdicttv);

    أرجع ret;
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
    إذا (self->name)
	ga_concat(&repr_ga, self->name);
    وإلا
	ga_concat(&repr_ga, (char_u *)"<NULL>");
    ga_append(&repr_ga, '\'');
    إذا (self->argv)
    {
	ga_concat(&repr_ga, (char_u *)", args=[");
	++emsg_silent;
	لكل (i = 0; i < self->argc; i++)
	{
	    إذا (i != 0)
		ga_concat(&repr_ga, (char_u *)", ");
	    ga_concat(&repr_ga, tv2string(&self->argv[i], &tofree, numbuf,
			get_copyID()));
	    vim_free(tofree);
	}
	--emsg_silent;
	ga_append(&repr_ga, ']');
    }
    إذا (self->self)
    {
	ga_concat(&repr_ga, (char_u *)", self=");
	tv.v_type = VAR_DICT;
	tv.vval.v_dict = self->self;
	++emsg_silent;
	ga_concat(&repr_ga, tv2string(&tv, &tofree, numbuf, get_copyID()));
	--emsg_silent;
	vim_free(tofree);
	إذا (self->auto_rebind)
	    ga_concat(&repr_ga, (char_u *)", auto_rebind=True");
    }
    ga_append(&repr_ga, '>');
    ret = PyString_FromString((حرف *)repr_ga.ga_انکڑے);
    ga_clear(&repr_ga);
    أرجع ret;
}

ساكن بنية Py_طریقہ_Def FunctionMethods[] = {
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
    Py_Object_سر
    صحيح		opt_type;
    فراغ	*from;
    checkfun	Check;
    PyObject	*fromObj;
} OptionsObject;

    ساكن صحيح
dummy_check(فراغ *arg بن_ورتیا)
{
    أرجع 0;
}

    ساكن PyObject *
OptionsNew(صحيح opt_type, فراغ *from, checkfun Check, PyObject *fromObj)
{
    OptionsObject	*self;

    self = PyObject_GC_New(OptionsObject, OptionsTypePtr);
    إذا (self == عدم)
	أرجع عدم;

    self->opt_type = opt_type;
    self->from = from;
    self->Check = Check;
    self->fromObj = fromObj;
    إذا (fromObj)
	Py_INCREF(fromObj);

    أرجع (PyObject *)(self);
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
OptionsTraverse(PyObject *self_obj, visitproc ویکھو, فراغ *arg)
{
    OptionsObject *self = (OptionsObject*)self_obj;
    Py_ویکھو(self->fromObj);
    أرجع 0;
}

    ساكن صحيح
OptionsClear(PyObject *self_obj)
{
    OptionsObject *self = (OptionsObject*)self_obj;
    Py_CLEAR(self->fromObj);
    أرجع 0;
}

    ساكن PyObject *
Options_شے(OptionsObject *self, PyObject *کلید_Object)
{
    char_u	*کلید;
    صحيح		flags;
    لما	numval;
    char_u	*stringval;
    PyObject	*todecref;

    إذا (self->Check(self->fromObj))
	أرجع عدم;

    إذا (!(کلید = String_منزل_Chars(کلید_Object, &todecref)))
	أرجع عدم;

    إذا (*کلید == NUL)
    {
	RAISE_NO_خالی_تھاواں_کلیدیں;
	Py_XDECREF(todecref);
	أرجع عدم;
    }

    flags = get_option_قدر_strict(کلید, &numval, &stringval,
				    self->opt_type, self->from);

    Py_XDECREF(todecref);

    إذا (flags == 0)
    {
	Py_Err_سیٹ_Object(Py_Exc_کلید_Error, کلید_Object);
	أرجع عدم;
    }

    إذا (flags & SOPT_UNSET)
    {
	Py_INCREF(Py_None);
	أرجع Py_None;
    }
    وإلا إذا (flags & SOPT_BOOL)
    {
	PyObject	*ret;
	ret = numval ? Py_True : Py_False;
	Py_INCREF(ret);
	أرجع ret;
    }
    وإلا إذا (flags & SOPT_عدد)
	أرجع PyInt_FromLong(numval);
    وإلا إذا (flags & SOPT_STRING)
    {
	إذا (stringval)
	{
	    PyObject	*ret = Py_بائٹاں_From_String((حرف *)stringval);
	    vim_free(stringval);
	    أرجع ret;
	}
	وإلا
	{
	    Py_Err_سیٹ_STRING(PyExc_RuntimeError,
		    N_("unable to get option value"));
	    أرجع عدم;
	}
    }
    وإلا
    {
	Py_Err_سیٹ_VIM(N_("internal error: unknown option type"));
	أرجع عدم;
    }
}

    ساكن صحيح
Options_شامل_اے(OptionsObject *self, PyObject *کلید_Object)
{
    char_u	*کلید;
    PyObject	*todecref;

    إذا (!(کلید = String_منزل_Chars(کلید_Object, &todecref)))
	أرجع -1;

    إذا (*کلید == NUL)
    {
	Py_XDECREF(todecref);
	أرجع 0;
    }

    إذا (get_option_قدر_strict(کلید, عدم, عدم, self->opt_type, عدم))
    {
	Py_XDECREF(todecref);
	أرجع 1;
    }
    وإلا
    {
	Py_XDECREF(todecref);
	أرجع 0;
    }
}

تعريفنوع بنية
{
    فراغ	*lastoption;
    صحيح		opt_type;
} optiterinfo_T;

    ساكن PyObject *
Options_Iter_اگلا(فراغ **arg)
{
    char_u	*name;
    optiterinfo_T **oii = (optiterinfo_T**)arg;

    إذا ((name = option_iter_اگلا(&((*oii)->lastoption), (*oii)->opt_type)))
	أرجع PyString_FromString((حرف *)name);

    أرجع عدم;
}

    ساكن PyObject *
OptionsIter(PyObject *self_obj)
{
    OptionsObject	*self = (OptionsObject*)self_obj;
    optiterinfo_T	*oii;

    إذا (!(oii = PyMem_New(optiterinfo_T, 1)))
    {
	Py_Err_No_حافظہ();
	أرجع عدم;
    }

    oii->opt_type = self->opt_type;
    oii->lastoption = عدم;

    أرجع IterNew(oii,
	    PyMem_Free, Options_Iter_اگلا,
	    عدم, عدم, (PyObject *)self);
}

    ساكن صحيح
سیٹ_option_قدر_err(char_u *کلید, صحيح numval, char_u *stringval, صحيح opt_flags)
{
    حرف	*errmsg;

    إذا ((errmsg = سیٹ_option_قدر(کلید, numval, stringval, opt_flags)))
    {
	إذا (Vim_Try_سرا())
	    أرجع FAIL;
	Py_Err_سیٹ_Vim(errmsg);
	أرجع FAIL;
    }
    أرجع OK;
}

    ساكن صحيح
سیٹ_option_قدر_for(
	char_u *کلید,
	صحيح numval,
	char_u *stringval,
	صحيح opt_flags,
	صحيح opt_type,
	فراغ *from)
{
    switchwin_T	switchwin;
    bufref_T	save_curbuf;
    صحيح		سیٹ_ret = 0;

    Vim_Try_شروع();
    بدل (opt_type)
    {
	حالة SREQ_WIN:
	    إذا (switch_win(&switchwin, (win_T *)from,
			      win_لبھو_tabpage((win_T *)from), FALSE) == FAIL)
	    {
		restore_win(&switchwin, TRUE);
		إذا (Vim_Try_سرا())
		    أرجع -1;
		Py_Err_سیٹ_VIM(N_("problem while switching windows"));
		أرجع -1;
	    }
	    سیٹ_ret = سیٹ_option_قدر_err(کلید, numval, stringval, opt_flags);
	    restore_win(&switchwin, TRUE);
	    اكسر;
	حالة SREQ_BUF:
	    switch_بفر(&save_curbuf, (buf_T *)from);
	    سیٹ_ret = سیٹ_option_قدر_err(کلید, numval, stringval, opt_flags);
	    restore_بفر(&save_curbuf);
	    اكسر;
	حالة SREQ_GLOBAL:
	    سیٹ_ret = سیٹ_option_قدر_err(کلید, numval, stringval, opt_flags);
	    اكسر;
    }
    إذا (سیٹ_ret == FAIL)
	أرجع -1;
    أرجع Vim_Try_سرا();
}

    ساكن صحيح
Options_Ass_شے(OptionsObject *self, PyObject *کلید_Object, PyObject *valObject)
{
    char_u	*کلید;
    صحيح		flags;
    صحيح		opt_flags;
    صحيح		ret = 0;
    PyObject	*todecref;

    إذا (self->Check(self->fromObj))
	أرجع -1;

    إذا (!(کلید = String_منزل_Chars(کلید_Object, &todecref)))
	أرجع -1;

    إذا (*کلید == NUL)
    {
	RAISE_NO_خالی_تھاواں_کلیدیں;
	Py_XDECREF(todecref);
	أرجع -1;
    }

    flags = get_option_قدر_strict(کلید, عدم, عدم,
				    self->opt_type, self->from);

    إذا (flags == 0)
    {
	Py_Err_سیٹ_Object(Py_Exc_کلید_Error, کلید_Object);
	Py_XDECREF(todecref);
	أرجع -1;
    }

    إذا (valObject == عدم)
    {
	إذا (self->opt_type == SREQ_GLOBAL)
	{
	    PyErr_FORMAT(Py_Exc_قدر_Error,
		    N_("unable to unset global option %s"), کلید);
	    Py_XDECREF(todecref);
	    أرجع -1;
	}
	وإلا إذا (!(flags & SOPT_GLOBAL))
	{
	    PyErr_FORMAT(Py_Exc_قدر_Error,
		    N_("unable to unset option %s "
		       "which does not have global value"), کلید);
	    Py_XDECREF(todecref);
	    أرجع -1;
	}
	وإلا
	{
	    unset_global_local_option(کلید, self->from);
	    Py_XDECREF(todecref);
	    أرجع 0;
	}
    }

    opt_flags = (self->opt_type ? OPT_LOCAL : OPT_GLOBAL);

    إذا (flags & SOPT_BOOL)
    {
	صحيح	istrue = PyObject_IsTrue(valObject);

	إذا (istrue == -1)
	    ret = -1;
	وإلا
	    ret = سیٹ_option_قدر_for(کلید, istrue, عدم,
				    opt_flags, self->opt_type, self->from);
    }
    وإلا إذا (flags & SOPT_عدد)
    {
	لما	val;

	إذا (نمبر_منزل_Long(valObject, &val, نمبر_INT))
	{
	    Py_XDECREF(todecref);
	    أرجع -1;
	}

	ret = سیٹ_option_قدر_for(کلید, (صحيح) val, عدم, opt_flags,
				self->opt_type, self->from);
    }
    وإلا
    {
	char_u		*val;
	PyObject	*todecref2;

	إذا ((val = String_منزل_Chars(valObject, &todecref2)))
	{
	    ret = سیٹ_option_قدر_for(کلید, 0, val, opt_flags,
				    self->opt_type, self->from);
	    Py_XDECREF(todecref2);
	}
	وإلا
	    ret = -1;
    }

    Py_XDECREF(todecref);

    أرجع ret;
}

ساكن PySequenceMethods OptionsAsSeq = {
    0,					// sq_length
    0,					// sq_concat
    0,					// sq_repeat
    0,					// sq_item
    0,					// sq_slice
    0,					// sq_ass_item
    0,					// sq_ass_slice
    (objobjproc) Options_شامل_اے,	// sq_contains
    0,					// sq_inplace_concat
    0,					// sq_inplace_repeat
};

ساكن PyMappingMethods OptionsAsMapping = {
    (lenfunc)       عدم,
    (binaryfunc)    Options_شے,
    (objobjargproc) Options_Ass_شے,
};

// Tabpage object

تعريفنوع بنية
{
    Py_Object_سر
    tabpage_T	*tab;
} Tab_ورقہ_Object;

ساكن PyObject *WinListNew(Tab_ورقہ_Object *tabObject);

DEFINE_PY_TYPE_OBJECT(Tab_ورقہ_Type);

    ساكن صحيح
Check_Tab_ورقہ(Tab_ورقہ_Object *self)
{
    إذا (self->tab == INVALID_TABPAGE_قدر)
    {
	Py_Err_سیٹ_VIM(N_("attempt to refer to deleted tab page"));
	أرجع -1;
    }

    أرجع 0;
}

    ساكن PyObject *
Tab_ورقہ_New(tabpage_T *tab)
{
    Tab_ورقہ_Object *self;

    إذا (TAB_PYTHON_REF(tab))
    {
	self = TAB_PYTHON_REF(tab);
	Py_INCREF((PyObject *)self);
    }
    وإلا
    {
	self = PyObject_NEW(Tab_ورقہ_Object, Tab_ورقہ_Type_Ptr);
	إذا (self == عدم)
	    أرجع عدم;
	self->tab = tab;
	TAB_PYTHON_REF(tab) = self;
    }

    أرجع (PyObject *)(self);
}

    ساكن فراغ
Tab_ورقہ_Destructor(PyObject *self_obj)
{
    Tab_ورقہ_Object *self = (Tab_ورقہ_Object*)self_obj;
    إذا (self->tab && self->tab != INVALID_TABPAGE_قدر)
	TAB_PYTHON_REF(self->tab) = عدم;

    DESTRUCTOR_انت(self);
}

ساكن حرف *Tab_ورقہ_Attrs[] = {
    "windows", "number", "vars", "window", "valid",
    عدم
};

    ساكن PyObject *
Tab_ورقہ_Dir(PyObject *self, PyObject *دلیلاں بن_ورتیا)
{
    أرجع ObjectDir(self, Tab_ورقہ_Attrs);
}

    ساكن PyObject *
Tab_ورقہ_Attr_Valid(Tab_ورقہ_Object *self, حرف *name)
{
    PyObject	*ret;

    إذا (strcmp(name, "valid") != 0)
	أرجع عدم;

    ret = ((self->tab == INVALID_TABPAGE_قدر) ? Py_False : Py_True);
    Py_INCREF(ret);
    أرجع ret;
}

    ساكن PyObject *
Tab_ورقہ_Attr(Tab_ورقہ_Object *self, حرف *name)
{
    إذا (strcmp(name, "windows") == 0)
	أرجع WinListNew(self);
    وإلا إذا (strcmp(name, "number") == 0)
	أرجع PyLong_FromLong((لما) get_tab_نمبر(self->tab));
    وإلا إذا (strcmp(name, "vars") == 0)
	أرجع NEW_DICTIONARY(self->tab->tp_vars);
    وإلا إذا (strcmp(name, "window") == 0)
    {
	// For current tab window.c does not bother to set or update tp_curwin
	إذا (self->tab == curtab)
	    أرجع کھڑکی_New(curwin, curtab);
	وإلا
	    أرجع کھڑکی_New(self->tab->tp_curwin, self->tab);
    }
    وإلا إذا (strcmp(name, "__members__") == 0)
	أرجع ObjectDir(عدم, Tab_ورقہ_Attrs);
    أرجع عدم;
}

    ساكن PyObject *
Tab_ورقہ_Repr(PyObject *self_obj)
{
    Tab_ورقہ_Object *self = (Tab_ورقہ_Object*)self_obj;
    إذا (self->tab == INVALID_TABPAGE_قدر)
	أرجع PyString_FromFormat("<tabpage object (deleted) at %p>", (فراغ *)self);
    وإلا
    {
	صحيح	t = get_tab_نمبر(self->tab);

	إذا (t == 0)
	    أرجع PyString_FromFormat("<tabpage object (unknown) at %p>",
					(فراغ *)self);
	وإلا
	    أرجع PyString_FromFormat("<tabpage %d>", t - 1);
    }
}

ساكن بنية Py_طریقہ_Def Tab_ورقہ_Methods[] = {
    // name,	    function,			calling,	documentation
    {"__dir__",	    (PyCFunction)Tab_ورقہ_Dir,	METH_NOARGS,	""},
    { عدم,	    عدم,			0,		عدم}
};

/*
 * Window list object
 */

DEFINE_PY_TYPE_OBJECT(TabListType);
ساكن PySequenceMethods TabListAsSeq;

تعريفنوع بنية
{
    Py_Object_سر
} TabListObject;

ساكن TabListObject The_Tab_ورقہ_List =
{
    Py_Object_سر_INIT_TYPE(TabListType)
};

    ساكن PyInt
Tab_List_لمائی(PyObject *self بن_ورتیا)
{
    tabpage_T	*tp = پہلا_tabpage;
    PyInt	n = 0;

    طالما (tp != عدم)
    {
	++n;
	tp = tp->tp_اگلا;
    }

    أرجع n;
}

    ساكن PyObject *
Tab_List_شے(PyObject *self بن_ورتیا, PyInt n)
{
    tabpage_T	*tp;

    لكل (tp = پہلا_tabpage; tp != عدم; tp = tp->tp_اگلا, --n)
	إذا (n == 0)
	    أرجع Tab_ورقہ_New(tp);

    Py_Err_سیٹ_STRING(Py_Exc_اشاریہ_Error, N_("no such tab page"));
    أرجع عدم;
}

/*
 * Window object
 */

تعريفنوع بنية
{
    Py_Object_سر
    win_T	*win;
    Tab_ورقہ_Object	*tabObject;
} کھڑکی_Object;

DEFINE_PY_TYPE_OBJECT(کھڑکی_Type);

    ساكن صحيح
Check_کھڑکی(کھڑکی_Object *self)
{
    إذا (self->win == INVALID_کھڑکی_قدر)
    {
	Py_Err_سیٹ_VIM(N_("attempt to refer to deleted window"));
	أرجع -1;
    }

    أرجع 0;
}

    ساكن صحيح
Check_کھڑکی_Cb(فراغ *self)
{
    أرجع Check_کھڑکی((کھڑکی_Object*)self);
}

    ساكن PyObject *
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

    إذا (WIN_PYTHON_REF(win))
    {
	self = WIN_PYTHON_REF(win);
	Py_INCREF((PyObject *)self);
    }
    وإلا
    {
	self = PyObject_GC_New(کھڑکی_Object, کھڑکی_Type_Ptr);
	إذا (self == عدم)
	    أرجع عدم;
	self->win = win;
	WIN_PYTHON_REF(win) = self;
    }

    self->tabObject = ((Tab_ورقہ_Object *)(Tab_ورقہ_New(tab)));

    أرجع (PyObject *)(self);
}

    ساكن فراغ
کھڑکی_Destructor(PyObject *self_obj)
{
    کھڑکی_Object *self = (کھڑکی_Object*)self_obj;
    PyObject_GC_UnTrack((فراغ *)(self));
    إذا (self->win && self->win != INVALID_کھڑکی_قدر)
	WIN_PYTHON_REF(self->win) = عدم;
    Py_XDECREF(((PyObject *)(self->tabObject)));
    PyObject_GC_Del((فراغ *)(self));
}

    ساكن صحيح
کھڑکی_Traverse(PyObject *self_obj, visitproc ویکھو, فراغ *arg)
{
    کھڑکی_Object *self = (کھڑکی_Object*)self_obj;
    Py_ویکھو(((PyObject *)(self->tabObject)));
    أرجع 0;
}

    ساكن صحيح
کھڑکی_Clear(PyObject *self_obj)
{
    کھڑکی_Object *self = (کھڑکی_Object*)self_obj;
    Py_CLEAR(self->tabObject);
    أرجع 0;
}

    ساكن win_T *
get_firstwin(Tab_ورقہ_Object *tabObject)
{
    إذا (tabObject)
    {
	إذا (Check_Tab_ورقہ(tabObject))
	    أرجع عدم;
	// For current tab window.c does not bother to set or update tp_firstwin
	وإلا إذا (tabObject->tab == curtab)
	    أرجع firstwin;
	وإلا
	    أرجع tabObject->tab->tp_firstwin;
    }
    وإلا
	أرجع firstwin;
}

// Use the same order as in the WindowAttr() function.
ساكن حرف *کھڑکی_Attrs[] = {
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
کھڑکی_Dir(PyObject *self, PyObject *دلیلاں بن_ورتیا)
{
    أرجع ObjectDir(self, کھڑکی_Attrs);
}

    ساكن PyObject *
کھڑکی_Attr_Valid(کھڑکی_Object *self, حرف *name)
{
    PyObject	*ret;

    إذا (strcmp(name, "valid") != 0)
	أرجع عدم;

    ret = ((self->win == INVALID_کھڑکی_قدر) ? Py_False : Py_True);
    Py_INCREF(ret);
    أرجع ret;
}

    ساكن PyObject *
کھڑکی_Attr(کھڑکی_Object *self, حرف *name)
{
    إذا (strcmp(name, "buffer") == 0)
	أرجع (PyObject *)بفر_New(self->win->w_بفر);
    وإلا إذا (strcmp(name, "cursor") == 0)
    {
	pos_T *pos = &self->win->w_cursor;

	أرجع Py_Build_قدر("(ll)", (لما)(pos->lnum), (لما)(pos->خانہ));
    }
    وإلا إذا (strcmp(name, "height") == 0)
	أرجع PyLong_FromLong((لما)(self->win->w_اچائی));
    وإلا إذا (strcmp(name, "row") == 0)
	أرجع PyLong_FromLong((لما)(self->win->w_winrow));
    وإلا إذا (strcmp(name, "width") == 0)
	أرجع PyLong_FromLong((لما)(self->win->w_چوڑائی));
    وإلا إذا (strcmp(name, "col") == 0)
	أرجع PyLong_FromLong((لما)(self->win->w_wincol));
    وإلا إذا (strcmp(name, "vars") == 0)
	أرجع NEW_DICTIONARY(self->win->w_vars);
    وإلا إذا (strcmp(name, "options") == 0)
	أرجع OptionsNew(SREQ_WIN, self->win, Check_کھڑکی_Cb,
			(PyObject *) self);
    وإلا إذا (strcmp(name, "number") == 0)
    {
	إذا (Check_Tab_ورقہ(self->tabObject))
	    أرجع عدم;
	أرجع PyLong_FromLong((لما)
		get_win_نمبر(self->win, get_firstwin(self->tabObject)));
    }
    وإلا إذا (strcmp(name, "tabpage") == 0)
    {
	Py_INCREF((PyObject *)self->tabObject);
	أرجع (PyObject *)(self->tabObject);
    }
    وإلا إذا (strcmp(name, "__members__") == 0)
	أرجع ObjectDir(عدم, کھڑکی_Attrs);
    وإلا
	أرجع عدم;
}

    ساكن صحيح
کھڑکی_Setattr(PyObject *self_obj, حرف *name, PyObject *valObject)
{
    کھڑکی_Object *self = (کھڑکی_Object*)self_obj;
    إذا (Check_کھڑکی(self))
	أرجع -1;

    إذا (strcmp(name, "buffer") == 0)
    {
	Py_Err_سیٹ_STRING(PyExc_TypeError, N_("readonly attribute: buffer"));
	أرجع -1;
    }
    وإلا إذا (strcmp(name, "cursor") == 0)
    {
	لما lnum;
	لما خانہ;

	إذا (!PyArg_Parse(valObject, "(ll)", &lnum, &خانہ))
	    أرجع -1;

	إذا (lnum <= 0 || lnum > self->win->w_بفر->b_ml.ml_سطر_گنتی)
	{
	    Py_Err_سیٹ_VIM(N_("cursor position outside buffer"));
	    أرجع -1;
	}

	// Check for keyboard interrupts
	إذا (Vim_Check_رکاوٹ())
	    أرجع -1;

	self->win->w_cursor.lnum = lnum;
	self->win->w_cursor.خانہ = خانہ;
	self->win->w_سیٹ_curswant = TRUE;
	self->win->w_cursor.coladd = 0;
	// When column is out of range silently correct it.
	check_cursor_خانہ_win(self->win);

	update_screen(UPD_VALID);
	أرجع 0;
    }
    وإلا إذا (strcmp(name, "height") == 0)
    {
	لما	اچائی;
	win_T	*savewin;

	إذا (نمبر_منزل_Long(valObject, &اچائی, نمبر_INT|نمبر_UNSIGNED))
	    أرجع -1;

#جے_تعریف_شدہ FEAT_GUI
	need_mouse_correct = TRUE;
#جے_انت
	savewin = curwin;
	curwin = self->win;
	curbuf = curwin->w_بفر;

	Vim_Try_شروع();
	win_setheight((صحيح) اچائی);
	curwin = savewin;
	curbuf = curwin->w_بفر;
	إذا (Vim_Try_سرا())
	    أرجع -1;

	أرجع 0;
    }
    وإلا إذا (strcmp(name, "width") == 0)
    {
	لما	چوڑائی;
	win_T	*savewin;

	إذا (نمبر_منزل_Long(valObject, &چوڑائی, نمبر_INT|نمبر_UNSIGNED))
	    أرجع -1;

#جے_تعریف_شدہ FEAT_GUI
	need_mouse_correct = TRUE;
#جے_انت
	savewin = curwin;
	curwin = self->win;
	curbuf = curwin->w_بفر;

	Vim_Try_شروع();
	win_setwidth((صحيح) چوڑائی);
	curwin = savewin;
	curbuf = curwin->w_بفر;
	إذا (Vim_Try_سرا())
	    أرجع -1;

	أرجع 0;
    }
    وإلا
    {
	Py_Err_سیٹ_String(PyExc_AttributeError, name);
	أرجع -1;
    }
}

    ساكن PyObject *
کھڑکی_Repr(PyObject *self_obj)
{
    کھڑکی_Object *self = (کھڑکی_Object*)self_obj;
    إذا (self->win == INVALID_کھڑکی_قدر)
	أرجع PyString_FromFormat("<window object (deleted) at %p>", (فراغ *)self);
    وإلا
    {
	صحيح	w = get_win_نمبر(self->win, firstwin);

	إذا (w == 0)
	    أرجع PyString_FromFormat("<window object (unknown) at %p>",
								      (فراغ *)self);
	وإلا
	    أرجع PyString_FromFormat("<window %d>", w - 1);
    }
}

ساكن بنية Py_طریقہ_Def کھڑکی_Methods[] = {
    // name,	    function,			calling,	documentation
    {"__dir__",	    (PyCFunction)کھڑکی_Dir,	METH_NOARGS,	""},
    { عدم,	    عدم,			0,		عدم}
};

/*
 * Window list object
 */

DEFINE_PY_TYPE_OBJECT(WinListType);
ساكن PySequenceMethods WinListAsSeq;

تعريفنوع بنية
{
    Py_Object_سر
    Tab_ورقہ_Object	*tabObject;
} WinListObject;

ساكن WinListObject The_کھڑکی_List =
{
    Py_Object_سر_INIT_TYPE(WinListType)
    عدم
};

    ساكن PyObject *
WinListNew(Tab_ورقہ_Object *tabObject)
{
    WinListObject	*self;

    self = PyObject_NEW(WinListObject, WinListTypePtr);
    self->tabObject = tabObject;
    Py_INCREF((PyObject *)tabObject);

    أرجع (PyObject *)(self);
}

    ساكن فراغ
WinListDestructor(WinListObject *self)
{
    Tab_ورقہ_Object	*tabObject = self->tabObject;

    إذا (tabObject)
    {
	Py_DECREF((PyObject *)(tabObject));
    }

    DESTRUCTOR_انت(self);
}

    ساكن PyInt
Win_List_لمائی(WinListObject *self)
{
    win_T	*w;
    PyInt	n = 0;

    إذا (!(w = get_firstwin(self->tabObject)))
	أرجع -1;

    طالما (w != عدم)
    {
	++n;
	w = W_اگلا(w);
    }

    أرجع n;
}

    ساكن PyObject *
Win_List_شے(WinListObject *self, PyInt n)
{
    win_T *w;

    إذا (!(w = get_firstwin(self->tabObject)))
	أرجع عدم;

    لكل (; w != عدم; w = W_اگلا(w), --n)
	إذا (n == 0)
	    أرجع کھڑکی_New(w, self->tabObject? self->tabObject->tab: curtab);

    Py_Err_سیٹ_STRING(Py_Exc_اشاریہ_Error, N_("no such window"));
    أرجع عدم;
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
String_منزل_سطر(PyObject *obj)
{
    حرف	*str;
    حرف	*save;
    PyObject	*بائٹاں = عدم;
    Py_ssize_t	len = 0;
    PyInt	i;
    حرف	*p;

    إذا (Py_بائٹاں_Check(obj))
    {
	إذا (Py_بائٹاں_As_String_And_ناپ(obj, &str, &len) == -1
		|| str == عدم)
	    أرجع عدم;
    }
    وإلا إذا (PyUnicode_Check(obj))
    {
	إذا (!(بائٹاں = PyUnicode_AsEncodedString(obj, ENC_OPT,
							   ERRORS_ENCODE_ARG)))
	    أرجع عدم;

	إذا (Py_بائٹاں_As_String_And_ناپ(بائٹاں, &str, &len) == -1
		|| str == عدم)
	{
	    Py_DECREF(بائٹاں);
	    أرجع عدم;
	}
    }
    وإلا
    {
#إذا PY_MAJOR_نسخہ < 3
	PyErr_FORMAT_TYPE(
		N_("expected str() or unicode() instance, but got %s"),
		obj);
#وإلا
	PyErr_FORMAT_TYPE(
		N_("expected bytes() or str() instance, but got %s"),
		obj);
#جے_انت
	أرجع عدم;
    }

    /*
     * Error checking: String must not contain newlines, as we
     * are replacing a single line, and we must replace it with
     * a single line.
     * A trailing newline is removed, so that append(f.readlines()) works.
     */
    p = memchr(str, '\n', len);
    إذا (p != عدم)
    {
	إذا (p == str + len - 1)
	    --len;
	وإلا
	{
	    Py_Err_سیٹ_VIM(N_("string cannot contain newlines"));
	    Py_XDECREF(بائٹاں);
	    أرجع عدم;
	}
    }

    /*
     * Create a copy of the string, with internal nulls replaced by
     * newline characters, as is the Vim convention.
     */
    save = alloc(len+1);
    إذا (save == عدم)
    {
	Py_Err_No_حافظہ();
	Py_XDECREF(بائٹاں);
	أرجع عدم;
    }

    لكل (i = 0; i < len; ++i)
    {
	إذا (str[i] == '\0')
	    save[i] = '\n';
	وإلا
	    save[i] = str[i];
    }

    save[i] = '\0';
    Py_XDECREF(بائٹاں);  // Python 2 does nothing here

    أرجع save;
}

/*
 * Get a line from the specified buffer. The line number is
 * in Vim format (1-based). The line is returned as a Python
 * string object.
 */
    ساكن PyObject *
Get_بفر_سطر(buf_T *buf, PyInt n)
{
    أرجع سطر_منزل_String((حرف *)ml_get_buf(buf, (linenr_T)n, FALSE));
}


/*
 * Get a list of lines from the specified buffer. The line numbers
 * are in Vim format (1-based). The range is from lo up to, but not
 * including, hi. The list is returned as a Python list of string objects.
 */
    ساكن PyObject *
Get_بفر_سطر_List(buf_T *buf, PyInt lo, PyInt hi)
{
    PyInt	i;
    PyInt	n = hi - lo;
    PyObject	*list = PyList_New(n);

    إذا (list == عدم)
	أرجع عدم;

    لكل (i = 0; i < n; ++i)
    {
	linenr_T	lnum = (linenr_T)(lo + i);
	حرف		*متن;
	PyObject	*string;

	إذا (lnum > buf->b_ml.ml_سطر_گنتی)
	    متن = "";
	وإلا
	    متن = (حرف *)ml_get_buf(buf, lnum, FALSE);
	string = سطر_منزل_String(متن);
	إذا (string == عدم)
	{
	    Py_DECREF(list);
	    أرجع عدم;
	}

	Py_List_سیٹ_شے(list, i, string);
    }

    // The ownership of the Python list is passed to the caller (ie,
    // the caller should Py_DECREF() the object when it is finished
    // with it).

    أرجع list;
}

/*
 * Check if deleting lines made the cursor position invalid.
 * Changed the lines from "lo" to "hi" and added "extra" lines (negative if
 * deleted).
 */
    ساكن فراغ
py_fix_cursor(linenr_T lo, linenr_T hi, linenr_T extra)
{
    إذا (curwin->w_cursor.lnum >= lo)
    {
	// Adjust the cursor position if it's in/after the changed
	// lines.
	إذا (curwin->w_cursor.lnum >= hi)
	{
	    curwin->w_cursor.lnum += extra;
	    check_cursor_خانہ();
	}
	وإلا إذا (extra < 0)
	{
	    curwin->w_cursor.lnum = lo;
	    check_cursor();
	}
	وإلا
	    check_cursor_خانہ();
	بدلیا_cline_bef_curs();
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
سیٹ_بفر_سطر_2(buf_T *buf, PyInt n, PyObject *سطر, PyInt *len_change)
{
    bufref_T	save_curbuf = {عدم, 0, 0};
    switchwin_T	switchwin;

    // First of all, we check the type of the supplied Python object.
    // There are three cases:
    //	  1. NULL, or None - this is a deletion.
    //	  2. A string	   - this is a replacement.
    //	  3. Anything else - this is an error.
    إذا (سطر == Py_None || سطر == عدم)
    {
	PyErr_Clear();
	switchwin.sw_curwin = عدم;
	switch_منزل_win_for_buf(buf, &switchwin, &save_curbuf);

	Vim_Try_شروع();

	إذا (u_savedel((linenr_T)n, 1L) == FAIL)
	    RAISE_UNDO_FAIL;
	وإلا إذا (ml_delete((linenr_T)n) == FAIL)
	    RAISE_DELETE_سطر_FAIL;
	وإلا
	{
	    إذا (buf == curbuf && (switchwin.sw_curwin != عدم
					   || save_curbuf.br_buf == عدم))
		// Using an existing window for the buffer, adjust the cursor
		// position.
		py_fix_cursor((linenr_T)n, (linenr_T)n + 1, (linenr_T)-1);
	    إذا (save_curbuf.br_buf == عدم)
		// Only adjust marks if we managed to switch to a window that
		// holds the buffer, otherwise line numbers will be invalid.
		deleted_سطریں_mark((linenr_T)n, 1L);
	}

	restore_win_for_buf(&switchwin, &save_curbuf);

	إذا (Vim_Try_سرا())
	    أرجع FAIL;

	إذا (len_change)
	    *len_change = -1;

	أرجع OK;
    }
    وإلا إذا (Py_بائٹاں_Check(سطر) || PyUnicode_Check(سطر))
    {
	حرف	*save = String_منزل_سطر(سطر);

	إذا (save == عدم)
	    أرجع FAIL;

	Vim_Try_شروع();

	// We do not need to free "save" if ml_replace() consumes it.
	PyErr_Clear();
	switch_منزل_win_for_buf(buf, &switchwin, &save_curbuf);

	إذا (u_savesub((linenr_T)n) == FAIL)
	{
	    RAISE_UNDO_FAIL;
	    vim_free(save);
	}
	وإلا إذا (ml_replace((linenr_T)n, (char_u *)save, FALSE) == FAIL)
	{
	    RAISE_REPLACE_سطر_FAIL;
	    vim_free(save);
	}
	وإلا
	    بدلیا_بائٹاں((linenr_T)n, 0);

	restore_win_for_buf(&switchwin, &save_curbuf);

	// Check that the cursor is not beyond the end of the line now.
	إذا (buf == curbuf)
	    check_cursor_خانہ();

	إذا (Vim_Try_سرا())
	    أرجع FAIL;

	إذا (len_change)
	    *len_change = 0;

	أرجع OK;
    }
    وإلا
    {
	Py_Err_Bad_دلیل();
	أرجع FAIL;
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
سیٹ_بفر_سطر_List(
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
    إذا (list == Py_None || list == عدم)
    {
	PyInt	i;
	PyInt	n = (صحيح)(hi - lo);

	PyErr_Clear();
	Vim_Try_شروع();
	switchwin.sw_curwin = عدم;
	switch_منزل_win_for_buf(buf, &switchwin, &save_curbuf);

	إذا (u_savedel((linenr_T)lo, (لما)n) == FAIL)
	    RAISE_UNDO_FAIL;
	وإلا
	{
	    لكل (i = 0; i < n; ++i)
	    {
		إذا (ml_delete((linenr_T)lo) == FAIL)
		{
		    RAISE_DELETE_سطر_FAIL;
		    اكسر;
		}
	    }
	    إذا (buf == curbuf && (switchwin.sw_curwin != عدم
					       || save_curbuf.br_buf == عدم))
		// Using an existing window for the buffer, adjust the cursor
		// position.
		py_fix_cursor((linenr_T)lo, (linenr_T)hi, (linenr_T)-n);
	    إذا (save_curbuf.br_buf == عدم)
		// Only adjust marks if we managed to switch to a window that
		// holds the buffer, otherwise line numbers will be invalid.
		deleted_سطریں_mark((linenr_T)lo, (لما)i);
	}

	restore_win_for_buf(&switchwin, &save_curbuf);

	إذا (Vim_Try_سرا())
	    أرجع FAIL;

	إذا (len_change)
	    *len_change = -n;

	أرجع OK;
    }
    وإلا إذا (PyList_Check(list))
    {
	PyInt	i;
	PyInt	new_len = Py_List_ناپ(list);
	PyInt	old_len = hi - lo;
	PyInt	extra = 0;	// lines added to text, can be negative
	حرف	**array;

	إذا (new_len == 0)	// avoid allocating zero bytes
	    array = عدم;
	وإلا
	{
	    array = PyMem_New(حرف *, new_len);
	    إذا (array == عدم)
	    {
		Py_Err_No_حافظہ();
		أرجع FAIL;
	    }
	}

	لكل (i = 0; i < new_len; ++i)
	{
	    PyObject *سطر;

	    إذا (!(سطر = Py_List_Get_شے(list, i)) ||
		!(array[i] = String_منزل_سطر(سطر)))
	    {
		طالما (i)
		    vim_free(array[--i]);
		PyMem_Free(array);
		أرجع FAIL;
	    }
	}

	Vim_Try_شروع();
	PyErr_Clear();

	// START of region without "return".  Must call restore_buffer()!
	switchwin.sw_curwin = عدم;
	switch_منزل_win_for_buf(buf, &switchwin, &save_curbuf);

	إذا (u_save((linenr_T)(lo-1), (linenr_T)hi) == FAIL)
	    RAISE_UNDO_FAIL;

	// If the size of the range is reducing (ie, new_len < old_len) we
	// need to delete some old_len. We do this at the start, by
	// repeatedly deleting line "lo".
	إذا (!PyErr_Occurred())
	{
	    لكل (i = 0; i < old_len - new_len; ++i)
		إذا (ml_delete((linenr_T)lo) == FAIL)
		{
		    RAISE_DELETE_سطر_FAIL;
		    اكسر;
		}
	    extra -= i;
	}

	// For as long as possible, replace the existing old_len with the
	// new old_len. This is a more efficient operation, as it requires
	// less memory allocation and freeing.
	إذا (!PyErr_Occurred())
	{
	    لكل (i = 0; i < old_len && i < new_len; ++i)
		إذا (ml_replace((linenr_T)(lo+i), (char_u *)array[i], FALSE)
								      == FAIL)
		{
		    RAISE_REPLACE_سطر_FAIL;
		    اكسر;
		}
	}
	وإلا
	    i = 0;

	// Now we may need to insert the remaining new old_len. If we do, we
	// must free the strings as we finish with them (we can't pass the
	// responsibility to Vim in this case).
	إذا (!PyErr_Occurred())
	{
	    طالما (i < new_len)
	    {
		إذا (ml_append((linenr_T)(lo + i - 1),
					(char_u *)array[i], 0, FALSE) == FAIL)
		{
		    RAISE_داخل_کرو_سطر_FAIL;
		    اكسر;
		}
		vim_free(array[i]);
		++i;
		++extra;
	    }
	}

	// Free any left-over old_len, as a result of an error
	طالما (i < new_len)
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
	إذا (save_curbuf.br_buf == عدم)
	{
	    mark_adjust((linenr_T)lo, (linenr_T)(hi - 1),
						  (لما)MAXLNUM, (لما)extra);
	    بدلیا_سطریں((linenr_T)lo, 0, (linenr_T)hi, (لما)extra);
	}

	إذا (buf == curbuf && (switchwin.sw_curwin != عدم
					   || save_curbuf.br_buf == عدم))
	    // Using an existing window for the buffer, adjust the cursor
	    // position.
	    py_fix_cursor((linenr_T)lo, (linenr_T)hi, (linenr_T)extra);

	// END of region without "return".
	restore_win_for_buf(&switchwin, &save_curbuf);

	إذا (Vim_Try_سرا())
	    أرجع FAIL;

	إذا (len_change)
	    *len_change = new_len - old_len;

	أرجع OK;
    }
    وإلا
    {
	Py_Err_Bad_دلیل();
	أرجع FAIL;
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
داخل_کرو_بفر_سطریں(buf_T *buf, PyInt n, PyObject *سطریں, PyInt *len_change)
{
    bufref_T	save_curbuf = {عدم, 0, 0};
    switchwin_T	switchwin;

    // First of all, we check the type of the supplied Python object.
    // It must be a string or a list, or the call is in error.
    إذا (Py_بائٹاں_Check(سطریں) || PyUnicode_Check(سطریں))
    {
	حرف		*str = String_منزل_سطر(سطریں);

	إذا (str == عدم)
	    أرجع FAIL;

	PyErr_Clear();
	Vim_Try_شروع();
	switch_منزل_win_for_buf(buf, &switchwin, &save_curbuf);

	إذا (u_save((linenr_T)n, (linenr_T)(n + 1)) == FAIL)
	    RAISE_UNDO_FAIL;
	وإلا إذا (ml_append((linenr_T)n, (char_u *)str, 0, FALSE) == FAIL)
	    RAISE_داخل_کرو_سطر_FAIL;
	وإلا إذا (save_curbuf.br_buf == عدم)
	    // Only adjust marks if we managed to switch to a window that
	    // holds the buffer, otherwise line numbers will be invalid.
	    appended_سطریں_mark((linenr_T)n, 1L);

	vim_free(str);
	restore_win_for_buf(&switchwin, &save_curbuf);
	update_screen(UPD_VALID);

	إذا (Vim_Try_سرا())
	    أرجع FAIL;

	إذا (len_change)
	    *len_change = 1;

	أرجع OK;
    }
    وإلا إذا (PyList_Check(سطریں))
    {
	PyInt	i;
	PyInt	ناپ = Py_List_ناپ(سطریں);
	حرف	**array;

	array = PyMem_New(حرف *, ناپ);
	إذا (array == عدم)
	{
	    Py_Err_No_حافظہ();
	    أرجع FAIL;
	}

	لكل (i = 0; i < ناپ; ++i)
	{
	    PyObject *سطر;

	    إذا (!(سطر = Py_List_Get_شے(سطریں, i)) ||
		!(array[i] = String_منزل_سطر(سطر)))
	    {
		طالما (i)
		    vim_free(array[--i]);
		PyMem_Free(array);
		أرجع FAIL;
	    }
	}

	PyErr_Clear();
	Vim_Try_شروع();
	switch_منزل_win_for_buf(buf, &switchwin, &save_curbuf);

	إذا (u_save((linenr_T)n, (linenr_T)(n + 1)) == FAIL)
	    RAISE_UNDO_FAIL;
	وإلا
	{
	    لكل (i = 0; i < ناپ; ++i)
	    {
		إذا (ml_append((linenr_T)(n + i),
					(char_u *)array[i], 0, FALSE) == FAIL)
		{
		    RAISE_داخل_کرو_سطر_FAIL;

		    // Free the rest of the lines
		    طالما (i < ناپ)
			vim_free(array[i++]);

		    اكسر;
		}
		vim_free(array[i]);
	    }
	    إذا (i > 0 && save_curbuf.br_buf == عدم)
		// Only adjust marks if we managed to switch to a window that
		// holds the buffer, otherwise line numbers will be invalid.
		appended_سطریں_mark((linenr_T)n, (لما)i);
	}

	// Free the array of lines. All of its contents have now
	// been freed.
	PyMem_Free(array);
	restore_win_for_buf(&switchwin, &save_curbuf);

	update_screen(UPD_VALID);

	إذا (Vim_Try_سرا())
	    أرجع FAIL;

	إذا (len_change)
	    *len_change = ناپ;

	أرجع OK;
    }
    وإلا
    {
	Py_Err_Bad_دلیل();
	أرجع FAIL;
    }
}

/*
 * Common routines for buffers and line ranges
 * -------------------------------------------
 */

تعريفنوع بنية
{
    Py_Object_سر
    buf_T *buf;
} بفر_Object;

    ساكن صحيح
Check_بفر(بفر_Object *self)
{
    إذا (self->buf == INVALID_بفر_قدر)
    {
	Py_Err_سیٹ_VIM(N_("attempt to refer to deleted buffer"));
	أرجع -1;
    }

    أرجع 0;
}

    ساكن صحيح
Check_بفر_Cb(فراغ *self)
{
    أرجع Check_بفر((بفر_Object*)self);
}

    ساكن PyObject *
RBItem(بفر_Object *self, PyInt n, PyInt شروع, PyInt سرا)
{
    إذا (Check_بفر(self))
	أرجع عدم;

    إذا (سرا == -1)
	سرا = self->buf->b_ml.ml_سطر_گنتی;

    إذا (n < 0)
	n += سرا - شروع + 1;

    إذا (n < 0 || n > سرا - شروع)
    {
	Py_Err_سیٹ_STRING(Py_Exc_اشاریہ_Error, N_("line number out of range"));
	أرجع عدم;
    }

    أرجع Get_بفر_سطر(self->buf, n+شروع);
}

    ساكن PyObject *
RBSlice(بفر_Object *self, PyInt lo, PyInt hi, PyInt شروع, PyInt سرا)
{
    PyInt ناپ;

    إذا (Check_بفر(self))
	أرجع عدم;

    إذا (سرا == -1)
	سرا = self->buf->b_ml.ml_سطر_گنتی;

    ناپ = سرا - شروع + 1;

    إذا (lo < 0)
	lo = 0;
    وإلا إذا (lo > ناپ)
	lo = ناپ;
    إذا (hi < 0)
	hi = 0;
    إذا (hi < lo)
	hi = lo;
    وإلا إذا (hi > ناپ)
	hi = ناپ;

    أرجع Get_بفر_سطر_List(self->buf, lo+شروع, hi+شروع);
}

    ساكن PyInt
RBAs_شے(
	بفر_Object *self,
	PyInt n,
	PyObject *valObject,
	PyInt شروع,
	PyInt سرا,
	PyInt *new_سرا)
{
    PyInt len_change;

    إذا (Check_بفر(self))
	أرجع -1;

    إذا (سرا == -1)
	سرا = self->buf->b_ml.ml_سطر_گنتی;

    إذا (n < 0)
	n += سرا - شروع + 1;

    إذا (n < 0 || n > سرا - شروع)
    {
	Py_Err_سیٹ_STRING(Py_Exc_اشاریہ_Error, N_("line number out of range"));
	أرجع -1;
    }

    إذا (سیٹ_بفر_سطر_2(self->buf, n+شروع, valObject, &len_change) == FAIL)
	أرجع -1;

    إذا (new_سرا)
	*new_سرا = سرا + len_change;

    أرجع 0;
}

    ساكن PyInt
RBAs_ٹکڑا(
	بفر_Object *self,
	PyInt lo,
	PyInt hi,
	PyObject *valObject,
	PyInt شروع,
	PyInt سرا,
	PyInt *new_سرا)
{
    PyInt ناپ;
    PyInt len_change;

    // Self must be a valid buffer
    إذا (Check_بفر(self))
	أرجع -1;

    إذا (سرا == -1)
	سرا = self->buf->b_ml.ml_سطر_گنتی;

    // Sort out the slice range
    ناپ = سرا - شروع + 1;

    إذا (lo < 0)
	lo = 0;
    وإلا إذا (lo > ناپ)
	lo = ناپ;
    إذا (hi < 0)
	hi = 0;
    إذا (hi < lo)
	hi = lo;
    وإلا إذا (hi > ناپ)
	hi = ناپ;

    إذا (سیٹ_بفر_سطر_List(self->buf, lo + شروع, hi + شروع,
						valObject, &len_change) == FAIL)
	أرجع -1;

    إذا (new_سرا)
	*new_سرا = سرا + len_change;

    أرجع 0;
}


    ساكن PyObject *
RBAppend(
	بفر_Object *self,
	PyObject *دلیلاں,
	PyInt شروع,
	PyInt سرا,
	PyInt *new_سرا)
{
    PyObject *سطریں;
    PyInt len_change;
    PyInt max;
    PyInt n;

    إذا (Check_بفر(self))
	أرجع عدم;

    إذا (سرا == -1)
	سرا = self->buf->b_ml.ml_سطر_گنتی;

    max = n = سرا - شروع + 1;

    إذا (!PyArg_ParseTuple(دلیلاں, "O|n", &سطریں, &n))
	أرجع عدم;

    إذا (n < 0 || n > max)
    {
	Py_Err_سیٹ_STRING(Py_Exc_اشاریہ_Error, N_("line number out of range"));
	أرجع عدم;
    }

    إذا (داخل_کرو_بفر_سطریں(self->buf, n + شروع - 1, سطریں, &len_change) == FAIL)
	أرجع عدم;

    إذا (new_سرا)
	*new_سرا = سرا + len_change;

    Py_INCREF(Py_None);
    أرجع Py_None;
}

// Range object

DEFINE_PY_TYPE_OBJECT(RangeType);
ساكن PySequenceMethods RangeAsSeq;
ساكن PyMappingMethods RangeAsMapping;

تعريفنوع بنية
{
    Py_Object_سر
    بفر_Object *buf;
    PyInt شروع;
    PyInt سرا;
} RangeObject;

    ساكن PyObject *
RangeNew(buf_T *buf, PyInt شروع, PyInt سرا)
{
    بفر_Object *bufr;
    RangeObject *self;
    self = PyObject_GC_New(RangeObject, RangeTypePtr);
    إذا (self == عدم)
	أرجع عدم;

    bufr = (بفر_Object *)بفر_New(buf);
    إذا (bufr == عدم)
    {
	Py_DECREF(self);
	أرجع عدم;
    }
    Py_INCREF((PyObject *)bufr);

    self->buf = bufr;
    self->شروع = شروع;
    self->سرا = سرا;

    أرجع (PyObject *)(self);
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
RangeTraverse(PyObject *self_obj, visitproc ویکھو, فراغ *arg)
{
    RangeObject *self = (RangeObject*)self_obj;
    Py_ویکھو(((PyObject *)(self->buf)));
    أرجع 0;
}

    ساكن صحيح
RangeClear(PyObject *self_obj)
{
    RangeObject *self = (RangeObject*)self_obj;
    Py_CLEAR(self->buf);
    أرجع 0;
}

    ساكن PyInt
Range_لمائی(RangeObject *self)
{
    // HOW DO WE SIGNAL AN ERROR FROM THIS FUNCTION?
    إذا (Check_بفر(self->buf))
	أرجع -1; // ???

    أرجع (self->سرا - self->شروع + 1);
}

    ساكن PyObject *
Range_شے(RangeObject *self, PyInt n)
{
    أرجع RBItem(self->buf, n, self->شروع, self->سرا);
}

    ساكن PyObject *
Range_ٹکڑا(RangeObject *self, PyInt lo, PyInt hi)
{
    أرجع RBSlice(self->buf, lo, hi, self->شروع, self->سرا);
}

ساكن حرف *RangeAttrs[] = {
    "start", "end",
    عدم
};

    ساكن PyObject *
RangeDir(PyObject *self, PyObject *دلیلاں بن_ورتیا)
{
    أرجع ObjectDir(self, RangeAttrs);
}

    ساكن PyObject *
RangeAppend(RangeObject *self, PyObject *دلیلاں)
{
    أرجع RBAppend(self->buf, دلیلاں, self->شروع, self->سرا, &self->سرا);
}

    ساكن PyObject *
RangeRepr(PyObject *self_obj)
{
    RangeObject *self = (RangeObject*)self_obj;
    إذا (self->buf->buf == INVALID_بفر_قدر)
	أرجع PyString_FromFormat("<range object (for deleted buffer) at %p>",
				    (فراغ *)self);
    وإلا
    {
	حرف *name = (حرف *)self->buf->buf->b_fname;

	إذا (name == عدم)
	    name = "";

	أرجع PyString_FromFormat("<range %s (%d:%d)>",
				    name, (صحيح)self->شروع, (صحيح)self->سرا);
    }
}

ساكن بنية Py_طریقہ_Def RangeMethods[] = {
    // name,	function,			calling,	documentation
    {"append",	(PyCFunction)RangeAppend,	METH_VARARGS,	"Append data to the Vim range" },
    {"__dir__",	(PyCFunction)RangeDir,		METH_NOARGS,	""},
    { عدم,	عدم,				0,		عدم}
};

DEFINE_PY_TYPE_OBJECT(بفر_Type);
ساكن PySequenceMethods بفر_As_Seq;
ساكن PyMappingMethods بفر_As_Mapping;

    ساكن PyObject *
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

    إذا (BUF_PYTHON_REF(buf) != عدم)
    {
	self = BUF_PYTHON_REF(buf);
	Py_INCREF((PyObject *)self);
    }
    وإلا
    {
	self = PyObject_NEW(بفر_Object, بفر_Type_Ptr);
	إذا (self == عدم)
	    أرجع عدم;
	self->buf = buf;
	BUF_PYTHON_REF(buf) = self;
    }

    أرجع (PyObject *)(self);
}

    ساكن فراغ
بفر_Destructor(PyObject *self_obj)
{
    بفر_Object *self = (بفر_Object*)self_obj;
    إذا (self->buf && self->buf != INVALID_بفر_قدر)
	BUF_PYTHON_REF(self->buf) = عدم;

    DESTRUCTOR_انت(self);
}

    ساكن PyInt
بفر_لمائی(بفر_Object *self)
{
    // HOW DO WE SIGNAL AN ERROR FROM THIS FUNCTION?
    إذا (Check_بفر(self))
	أرجع -1; // ???

    أرجع (PyInt)(self->buf->b_ml.ml_سطر_گنتی);
}

    ساكن PyObject *
بفر_شے(بفر_Object *self, PyInt n)
{
    أرجع RBItem(self, n, 1, -1);
}

    ساكن PyObject *
بفر_ٹکڑا(بفر_Object *self, PyInt lo, PyInt hi)
{
    أرجع RBSlice(self, lo, hi, 1, -1);
}

ساكن حرف *بفر_Attrs[] = {
    "name", "number", "vars", "options", "valid",
    عدم
};

    ساكن PyObject *
بفر_Dir(PyObject *self, PyObject *دلیلاں بن_ورتیا)
{
    أرجع ObjectDir(self, بفر_Attrs);
}

    ساكن PyObject *
بفر_Attr_Valid(بفر_Object *self, حرف *name)
{
    PyObject	*ret;

    إذا (strcmp(name, "valid") != 0)
	أرجع عدم;

    ret = ((self->buf == INVALID_بفر_قدر) ? Py_False : Py_True);
    Py_INCREF(ret);
    أرجع ret;
}

    ساكن PyObject *
بفر_Attr(بفر_Object *self, حرف *name)
{
    إذا (strcmp(name, "name") == 0)
	أرجع PyString_FromString((self->buf->b_ffname == عدم
				    ? "" : (حرف *)self->buf->b_ffname));
    وإلا إذا (strcmp(name, "number") == 0)
	أرجع Py_Build_قدر(Py_ssize_t_fmt, self->buf->b_fnum);
    وإلا إذا (strcmp(name, "vars") == 0)
	أرجع NEW_DICTIONARY(self->buf->b_vars);
    وإلا إذا (strcmp(name, "options") == 0)
	أرجع OptionsNew(SREQ_BUF, self->buf, Check_بفر_Cb,
			(PyObject *) self);
    وإلا إذا (strcmp(name, "__members__") == 0)
	أرجع ObjectDir(عدم, بفر_Attrs);
    وإلا
	أرجع عدم;
}

    ساكن صحيح
بفر_Setattr(PyObject *self_obj, حرف *name, PyObject *valObject)
{
    بفر_Object *self = (بفر_Object*)self_obj;
    إذا (Check_بفر(self))
	أرجع -1;

    إذا (strcmp(name, "name") == 0)
    {
	char_u		*val;
	aco_save_T	aco;
	صحيح		ren_ret = OK;
	PyObject	*todecref;

	إذا (!(val = String_منزل_Chars(valObject, &todecref)))
	    أرجع -1;

	Vim_Try_شروع();
	// Using aucmd_*: autocommands will be executed by rename_buffer
	aucmd_prepbuf(&aco, self->buf);
	إذا (curbuf == self->buf)
	{
	    ren_ret = rename_بفر(val);
	    aucmd_restbuf(&aco);
	}
	Py_XDECREF(todecref);
	إذا (Vim_Try_سرا())
	    أرجع -1;

	إذا (ren_ret == FAIL)
	{
	    Py_Err_سیٹ_VIM(N_("failed to rename buffer"));
	    أرجع -1;
	}
	أرجع 0;
    }
    وإلا
    {
	Py_Err_سیٹ_String(PyExc_AttributeError, name);
	أرجع -1;
    }
}

    ساكن PyObject *
بفر_Append(بفر_Object *self, PyObject *دلیلاں)
{
    أرجع RBAppend(self, دلیلاں, 1, -1, عدم);
}

    ساكن PyObject *
بفر_Mark(بفر_Object *self, PyObject *pmarkObject)
{
    pos_T	*posp;
    char_u	*pmark;
    char_u	mark;
    bufref_T	savebuf;
    PyObject	*todecref;

    إذا (Check_بفر(self))
	أرجع عدم;

    إذا (!(pmark = String_منزل_Chars(pmarkObject, &todecref)))
	أرجع عدم;

    إذا (pmark[0] == '\0' || pmark[1] != '\0')
    {
	Py_Err_سیٹ_STRING(Py_Exc_قدر_Error,
		N_("mark name must be a single character"));
	Py_XDECREF(todecref);
	أرجع عدم;
    }

    mark = *pmark;

    Py_XDECREF(todecref);

    Vim_Try_شروع();
    switch_بفر(&savebuf, self->buf);
    posp = getmark(mark, FALSE);
    restore_بفر(&savebuf);
    إذا (Vim_Try_سرا())
	أرجع عدم;

    إذا (posp == عدم)
    {
	Py_Err_سیٹ_VIM(N_("invalid mark name"));
	أرجع عدم;
    }

    إذا (posp->lnum <= 0)
    {
	// Or raise an error?
	Py_INCREF(Py_None);
	أرجع Py_None;
    }

    أرجع Py_Build_قدر("(ll)", (لما)(posp->lnum), (لما)(posp->خانہ));
}

    ساكن PyObject *
بفر_Range(بفر_Object *self, PyObject *دلیلاں)
{
    PyInt شروع;
    PyInt سرا;

    إذا (Check_بفر(self))
	أرجع عدم;

    إذا (!PyArg_ParseTuple(دلیلاں, "nn", &شروع, &سرا))
	أرجع عدم;

    أرجع RangeNew(self->buf, شروع, سرا);
}

    ساكن PyObject *
بفر_Repr(PyObject *self_obj)
{
    بفر_Object *self = (بفر_Object*)self_obj;
    إذا (self->buf == INVALID_بفر_قدر)
	أرجع PyString_FromFormat("<buffer object (deleted) at %p>", (فراغ *)self);
    وإلا
    {
	حرف	*name = (حرف *)self->buf->b_fname;

	إذا (name == عدم)
	    name = "";

	أرجع PyString_FromFormat("<buffer %s>", name);
    }
}

ساكن بنية Py_طریقہ_Def بفر_Methods[] = {
    // name,	    function,			calling,	documentation
    {"append",	    (PyCFunction)بفر_Append,	METH_VARARGS,	"Append data to Vim buffer" },
    {"mark",	    (PyCFunction)بفر_Mark,	METH_O,		"Return (row,col) representing position of named mark" },
    {"range",	    (PyCFunction)بفر_Range,	METH_VARARGS,	"Return a range object which represents the part of the given buffer between line numbers s and e" },
    {"__dir__",	    (PyCFunction)بفر_Dir,	METH_NOARGS,	""},
    { عدم,	    عدم,			0,		عدم}
};

/*
 * Buffer list object - Implementation
 */

DEFINE_PY_TYPE_OBJECT(BufMapType);

تعريفنوع بنية
{
    Py_Object_سر
} BufMapObject;

ساكن BufMapObject The_بفر_Map =
{
    Py_Object_سر_INIT_TYPE(BufMapType)
};

    ساكن PyInt
Buf_Map_لمائی(PyObject *self بن_ورتیا)
{
    buf_T	*b = firstbuf;
    PyInt	n = 0;

    طالما (b)
    {
	++n;
	b = b->b_اگلا;
    }

    أرجع n;
}

    ساكن PyObject *
Buf_Map_شے(PyObject *self بن_ورتیا, PyObject *کلید_Object)
{
    buf_T	*b;
    لما	bnr;

    إذا (نمبر_منزل_Long(کلید_Object, &bnr, نمبر_INT|نمبر_NATURAL))
	أرجع عدم;

    b = buflist_findnr((صحيح) bnr);

    إذا (b)
	أرجع بفر_New(b);
    وإلا
    {
	Py_Err_سیٹ_Object(Py_Exc_کلید_Error, کلید_Object);
	أرجع عدم;
    }
}

    ساكن فراغ
BufMapIterDestruct(فراغ* arg)
{
    PyObject *بفر = (PyObject*)arg;
    // Iteration was stopped before all buffers were processed
    إذا (بفر)
    {
	Py_DECREF(بفر);
    }
}

    ساكن صحيح
BufMapIterTraverse(فراغ *iter, visitproc ویکھو, فراغ *arg)
{
    PyObject *بفر = (PyObject*)iter;
    إذا (بفر)
	Py_ویکھو(بفر);
    أرجع 0;
}

    ساكن صحيح
BufMapIterClear(فراغ **iter)
{
    PyObject **بفر = (PyObject**)iter;
    إذا (*بفر)
	Py_CLEAR(*بفر);
    أرجع 0;
}

    ساكن PyObject *
Buf_Map_Iter_اگلا(فراغ **arg)
{
    PyObject	*اگلا;
    PyObject	*ret;
    PyObject	**بفر = (PyObject**)arg;

    إذا (!*بفر)
	أرجع عدم;

    ret = *بفر;

    إذا (Check_بفر((بفر_Object *)(ret)))
    {
	*بفر = عدم;
	أرجع عدم;
    }

    إذا (!((بفر_Object *)(ret))->buf->b_اگلا)
	اگلا = عدم;
    وإلا إذا (!(اگلا = بفر_New(((بفر_Object *)(ret))->buf->b_اگلا)))
	أرجع عدم;
    *بفر = اگلا;
    // Do not increment reference: we no longer hold it (decref), but whoever
    // on other side will hold (incref). Decref+incref = nothing.
    أرجع ret;
}

    ساكن PyObject *
BufMapIter(PyObject *self)
{
    PyObject *بفر;

    بفر = بفر_New(firstbuf);
    أرجع IterNew(بفر,
	    BufMapIterDestruct, Buf_Map_Iter_اگلا,
	    BufMapIterTraverse, BufMapIterClear,
	    self);
}

ساكن PyMappingMethods BufMapAsMapping = {
    (lenfunc)       Buf_Map_لمائی,
    (binaryfunc)    Buf_Map_شے,
    (objobjargproc) 0,
};

// Current items object

ساكن حرف *CurrentAttrs[] = {
    "buffer", "window", "line", "range", "tabpage",
    عدم
};

    ساكن PyObject *
CurrentDir(PyObject *self, PyObject *دلیلاں بن_ورتیا)
{
    أرجع ObjectDir(self, CurrentAttrs);
}

    ساكن PyObject *
CurrentGetattr(PyObject *self بن_ورتیا, حرف *name)
{
    إذا (strcmp(name, "buffer") == 0)
	أرجع (PyObject *)بفر_New(curbuf);
    وإلا إذا (strcmp(name, "window") == 0)
	أرجع (PyObject *)کھڑکی_New(curwin, curtab);
    وإلا إذا (strcmp(name, "tabpage") == 0)
	أرجع (PyObject *)Tab_ورقہ_New(curtab);
    وإلا إذا (strcmp(name, "line") == 0)
	أرجع Get_بفر_سطر(curbuf, (PyInt)curwin->w_cursor.lnum);
    وإلا إذا (strcmp(name, "range") == 0)
	أرجع RangeNew(curbuf, Range_شروع, Range_سرا);
    وإلا إذا (strcmp(name, "__members__") == 0)
	أرجع ObjectDir(عدم, CurrentAttrs);
    وإلا
#إذا PY_MAJOR_نسخہ < 3
	أرجع Py_لبھو_طریقہ(کھڑکی_Methods, self, name);
#وإلا
	أرجع عدم;
#جے_انت
}

    ساكن صحيح
CurrentSetattr(PyObject *self بن_ورتیا, حرف *name, PyObject *valObject)
{
    إذا (strcmp(name, "line") == 0)
    {
	إذا (سیٹ_بفر_سطر_2(curbuf, (PyInt)curwin->w_cursor.lnum, valObject,
			  عدم) == FAIL)
	    أرجع -1;

	أرجع 0;
    }
    وإلا إذا (strcmp(name, "buffer") == 0)
    {
	صحيح گنتی;

	إذا (valObject->ob_type != بفر_Type_Ptr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.Buffer object, but got %s"),
		    valObject);
	    أرجع -1;
	}

	إذا (Check_بفر((بفر_Object *)(valObject)))
	    أرجع -1;
	گنتی = ((بفر_Object *)(valObject))->buf->b_fnum;

	Vim_Try_شروع();
	إذا (do_بفر(DOBUF_GOTO, DOBUF_پہلا, FORWARD, گنتی, 0) == FAIL)
	{
	    إذا (Vim_Try_سرا())
		أرجع -1;
	    PyErr_VIM_FORMAT(N_("failed to switch to buffer %d"), گنتی);
	    أرجع -1;
	}

	أرجع Vim_Try_سرا();
    }
    وإلا إذا (strcmp(name, "window") == 0)
    {
	صحيح گنتی;

	إذا (valObject->ob_type != کھڑکی_Type_Ptr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.Window object, but got %s"),
		    valObject);
	    أرجع -1;
	}

	إذا (Check_کھڑکی((کھڑکی_Object *)(valObject)))
	    أرجع -1;
	گنتی = get_win_نمبر(((کھڑکی_Object *)(valObject))->win, firstwin);

	إذا (!گنتی)
	{
	    Py_Err_سیٹ_STRING(Py_Exc_قدر_Error,
		    N_("failed to find window in the current tab page"));
	    أرجع -1;
	}

	Vim_Try_شروع();
	win_goto(((کھڑکی_Object *)(valObject))->win);
	إذا (((کھڑکی_Object *)(valObject))->win != curwin)
	{
	    إذا (Vim_Try_سرا())
		أرجع -1;
	    Py_Err_سیٹ_STRING(PyExc_RuntimeError,
		    N_("did not switch to the specified window"));
	    أرجع -1;
	}

	أرجع Vim_Try_سرا();
    }
    وإلا إذا (strcmp(name, "tabpage") == 0)
    {
	إذا (valObject->ob_type != Tab_ورقہ_Type_Ptr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.TabPage object, but got %s"),
		    valObject);
	    أرجع -1;
	}

	إذا (Check_Tab_ورقہ((Tab_ورقہ_Object *)(valObject)))
	    أرجع -1;

	Vim_Try_شروع();
	goto_tabpage_tp(((Tab_ورقہ_Object *)(valObject))->tab, TRUE, TRUE);
	إذا (((Tab_ورقہ_Object *)(valObject))->tab != curtab)
	{
	    إذا (Vim_Try_سرا())
		أرجع -1;
	    Py_Err_سیٹ_STRING(PyExc_RuntimeError,
		    N_("did not switch to the specified tab page"));
	    أرجع -1;
	}

	أرجع Vim_Try_سرا();
    }
    وإلا
    {
	Py_Err_سیٹ_String(PyExc_AttributeError, name);
	أرجع -1;
    }
}

ساكن بنية Py_طریقہ_Def CurrentMethods[] = {
    // name,	    function,			calling,	documentation
    {"__dir__",	    (PyCFunction)CurrentDir,	METH_NOARGS,	""},
    { عدم,	    عدم,			0,		عدم}
};

    ساكن فراغ
init_range_cmd(فراغ *arg)
{
    exarg_T *eap = (exarg_T*)arg;
    Range_شروع = eap->line1;
    Range_سرا = eap->line2;
}

    ساكن فراغ
init_range_eval(فراغ *rettv بن_ورتیا)
{
    Range_شروع = (PyInt) curwin->w_cursor.lnum;
    Range_سرا = Range_شروع;
}

    ساكن فراغ
لگاتار_cmd(ثابت حرف *cmd, dict_T* locals بن_ورتیا, فراغ *arg بن_ورتیا
#جے_تعریف_شدہ PY_CAN_RECURSE
	, PyGILState_STATE *pygilstate بن_ورتیا
#جے_انت
	)
{
    PyObject	*لگاتار_ret;
    لگاتار_ret = Py_لگاتار_String((حرف *)cmd, Py_file_داخلہ, globals, globals);
    إذا (لگاتار_ret != عدم)
    {
	Py_DECREF(لگاتار_ret);
    }
    وإلا إذا (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
    {
	emsg(_(e_cant_handle_systemexit_دا_python_exception_وچ_vim));
	PyErr_Clear();
    }
    وإلا
	PyErr_PrintEx(1);
}

ساكن ثابت حرف	*کوڈ_hdr = "def " DOPY_FUNC "(line, linenr):\n ";
ساكن صحيح		کوڈ_hdr_len = 30;

    ساكن فراغ
لگاتار_do(ثابت حرف *cmd, dict_T* locals بن_ورتیا, فراغ *arg بن_ورتیا
#جے_تعریف_شدہ PY_CAN_RECURSE
	, PyGILState_STATE *pygilstate
#جے_انت
	)
{
    PyInt	lnum;
    size_t	len;
    حرف	*کوڈ;
    صحيح		حالت;
    PyObject	*pyfunc, *pymain;
    PyObject	*لگاتار_ret;
    buf_T	*was_curbuf = curbuf;

    إذا (u_save((linenr_T)Range_شروع - 1, (linenr_T)Range_سرا + 1) != OK)
    {
	emsg(_("cannot save undo information"));
	أرجع;
    }

    len = کوڈ_hdr_len + STRLEN(cmd);
    کوڈ = PyMem_New(حرف, len + 1);
    حافظہ_نقل(کوڈ, کوڈ_hdr, کوڈ_hdr_len);
    STRCPY(کوڈ + کوڈ_hdr_len, cmd);
    لگاتار_ret = Py_لگاتار_String(کوڈ, Py_file_داخلہ, globals, globals);
    حالت = -1;
    إذا (لگاتار_ret != عدم)
    {
	حالت = 0;
	Py_DECREF(لگاتار_ret);
    }
    وإلا إذا (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
    {
	PyMem_Free(کوڈ);
	emsg(_(e_cant_handle_systemexit_دا_python_exception_وچ_vim));
	PyErr_Clear();
	أرجع;
    }
    وإلا
	PyErr_PrintEx(1);

    PyMem_Free(کوڈ);

    إذا (حالت)
    {
	emsg(_("failed to run the code"));
	أرجع;
    }

    حالت = 0;
    pymain = Py_Import_جوڑو_Module("__main__");
    pyfunc = PyObject_GetAttrString(pymain, DOPY_FUNC);
#جے_تعریف_شدہ PY_CAN_RECURSE
    PyGILState_Release(*pygilstate);
#جے_انت

    لكل (lnum = Range_شروع; lnum <= Range_سرا; ++lnum)
    {
	PyObject	*سطر;
	PyObject	*linenr;
	PyObject	*ret;

#جے_تعریف_شدہ PY_CAN_RECURSE
	*pygilstate = PyGILState_Ensure();
#جے_انت
	// Check the line number, the command my have deleted lines.
	إذا (lnum > curbuf->b_ml.ml_سطر_گنتی
		|| !(سطر = Get_بفر_سطر(curbuf, lnum)))
	    اذهب err;
	إذا (!(linenr = PyInt_FromLong((لما) lnum)))
	{
	    Py_DECREF(سطر);
	    اذهب err;
	}
	ret = Py_Object_Call_Function_Obj_دلیلاں(pyfunc, سطر, linenr, عدم);
	Py_DECREF(سطر);
	Py_DECREF(linenr);
	إذا (!ret)
	    اذهب err;

	// Check that the command didn't switch to another buffer.
	// Check the line number, the command my have deleted lines.
	إذا (curbuf != was_curbuf || lnum > curbuf->b_ml.ml_سطر_گنتی)
	{
	    Py_XDECREF(ret);
	    اذهب err;
	}

	إذا (ret != Py_None)
	    إذا (سیٹ_بفر_سطر_2(curbuf, lnum, ret, عدم) == FAIL)
	    {
		Py_XDECREF(ret);
		اذهب err;
	    }

	Py_XDECREF(ret);
	PythonIO_Flush();
#جے_تعریف_شدہ PY_CAN_RECURSE
	PyGILState_Release(*pygilstate);
#جے_انت
    }
    اذهب باہرلا;
err:
#جے_تعریف_شدہ PY_CAN_RECURSE
    *pygilstate = PyGILState_Ensure();
#جے_انت
    PyErr_PrintEx(0);
    PythonIO_Flush();
    حالت = 1;
باہرلا:
#جے_تعریف_شدہ PY_CAN_RECURSE
    إذا (!حالت)
	*pygilstate = PyGILState_Ensure();
#جے_انت
    Py_DECREF(pyfunc);
    Py_Object_سیٹ_Attr_String(pymain, DOPY_FUNC, عدم);
    إذا (حالت)
	أرجع;
    check_cursor();
    update_curbuf(UPD_NOT_VALID);
}

    ساكن فراغ
لگاتار_eval(ثابت حرف *cmd, dict_T *locals, فراغ *arg
#جے_تعریف_شدہ PY_CAN_RECURSE
	, PyGILState_STATE *pygilstate بن_ورتیا
#جے_انت
	)
{
    PyObject	*لگاتار_ret;
    typval_T	*rettv = (typval_T*)arg;
    PyObject	*pylocals = locals ? NEW_DICTIONARY(locals) : globals;

    لگاتار_ret = Py_لگاتار_String((حرف *)cmd, Py_eval_داخلہ, globals, pylocals);
    إذا (لگاتار_ret == عدم)
    {
	إذا (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
	{
	    emsg(_(e_cant_handle_systemexit_دا_python_exception_وچ_vim));
	    PyErr_Clear();
	}
	وإلا
	{
	    إذا (PyErr_Occurred() && !پیغام_silent)
		PyErr_PrintEx(0);
	    emsg(_(e_eval_did_not_return_valid_python_object));
	}
    }
    وإلا
    {
	إذا (ConvertFromPyObject(لگاتار_ret, rettv) == -1)
	    emsg(_(e_failed_منزل_convert_returned_python_object_منزل_vim_قدر));
	Py_DECREF(لگاتار_ret);
    }
    PyErr_Clear();
}

    ساكن صحيح
سیٹ_ref_وچ_py(ثابت صحيح copyID)
{
    pylinkedlist_T	*cur;
    list_T		*ll;
    tuple_T		*tt;
    صحيح			i;
    صحيح			abort = FALSE;
    FunctionObject	*func;

    إذا (lastdict != عدم)
    {
	لكل (cur = lastdict ; !abort && cur != عدم ; cur = cur->pll_prev)
	    abort = سیٹ_ref_وچ_dict(((DictionaryObject *)(cur->pll_obj))->dict,
								       copyID);
    }

    إذا (lastlist != عدم)
    {
	لكل (cur = lastlist ; !abort && cur != عدم ; cur = cur->pll_prev)
	{
	    ll = ((ListObject *) (cur->pll_obj))->list;
	    abort = سیٹ_ref_وچ_list(ll, copyID);
	}
    }

    إذا (lasttuple != عدم)
    {
	لكل (cur = lasttuple ; !abort && cur != عدم ; cur = cur->pll_prev)
	{
	    tt = ((TupleObject *) (cur->pll_obj))->tuple;
	    abort = سیٹ_ref_وچ_tuple(tt, copyID);
	}
    }

    إذا (lastfunc != عدم)
    {
	لكل (cur = lastfunc ; !abort && cur != عدم ; cur = cur->pll_prev)
	{
	    func = (FunctionObject *) cur->pll_obj;
	    abort = سیٹ_ref_وچ_dict(func->self, copyID);
	    إذا (func->argc)
		لكل (i = 0; !abort && i < func->argc; ++i)
		    abort = abort
			|| سیٹ_ref_وچ_شے(&func->argv[i], copyID, عدم, عدم, عدم);
	}
    }

    أرجع abort;
}

    ساكن صحيح
سیٹ_string_copy(char_u *str, typval_T *tv)
{
    tv->vval.v_string = vim_strsave(str);
    إذا (tv->vval.v_string == عدم)
    {
	Py_Err_No_حافظہ();
	أرجع -1;
    }
    أرجع 0;
}

    ساكن صحيح
pydict_منزل_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    dict_T	*dict;
    char_u	*کلید;
    dictitem_T	*di;
    PyObject	*کلید_Object;
    PyObject	*valObject;
    Py_ssize_t	iter = 0;

    إذا (!(dict = py_dict_alloc()))
	أرجع -1;

    tv->v_type = VAR_DICT;
    tv->vval.v_dict = dict;

    طالما (Py_Dict_اگلا(obj, &iter, &کلید_Object, &valObject))
    {
	PyObject	*todecref = عدم;

	إذا (کلید_Object == عدم || valObject == عدم)
	{
	    dict_unref(dict);
	    أرجع -1;
	}

	إذا (!(کلید = String_منزل_Chars(کلید_Object, &todecref)))
	{
	    dict_unref(dict);
	    أرجع -1;
	}

	إذا (*کلید == NUL)
	{
	    dict_unref(dict);
	    Py_XDECREF(todecref);
	    RAISE_NO_خالی_تھاواں_کلیدیں;
	    أرجع -1;
	}

	di = dictitem_alloc(کلید);

	Py_XDECREF(todecref);

	إذا (di == عدم)
	{
	    Py_Err_No_حافظہ();
	    dict_unref(dict);
	    أرجع -1;
	}

	إذا (_ConvertFromPyObject(valObject, &di->di_tv, lookup_dict) == -1)
	{
	    vim_free(di);
	    dict_unref(dict);
	    أرجع -1;
	}

	إذا (dict_جوڑو(dict, di) == FAIL)
	{
	    RAISE_کلید_جوڑو_FAIL(di->di_کلید);
	    clear_tv(&di->di_tv);
	    vim_free(di);
	    dict_unref(dict);
	    أرجع -1;
	}
    }

    --dict->dv_refcount;
    أرجع 0;
}

    ساكن صحيح
pymap_منزل_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    dict_T	*dict;
    char_u	*کلید;
    dictitem_T	*di;
    PyObject	*list;
    PyObject	*iterator;
    PyObject	*کلید_Object;
    PyObject	*valObject;

    إذا (!(dict = py_dict_alloc()))
	أرجع -1;

    tv->v_type = VAR_DICT;
    tv->vval.v_dict = dict;

    إذا (!(list = Py_Mapping_کلیدیں(obj)))
    {
	dict_unref(dict);
	أرجع -1;
    }

    إذا (!(iterator = PyObject_GetIter(list)))
    {
	dict_unref(dict);
	Py_DECREF(list);
	أرجع -1;
    }
    Py_DECREF(list);

    طالما ((کلید_Object = Py_Iter_اگلا(iterator)))
    {
	PyObject	*todecref;

	إذا (!(کلید = String_منزل_Chars(کلید_Object, &todecref)))
	{
	    Py_DECREF(کلید_Object);
	    Py_DECREF(iterator);
	    dict_unref(dict);
	    أرجع -1;
	}

	إذا (*کلید == NUL)
	{
	    Py_DECREF(کلید_Object);
	    Py_DECREF(iterator);
	    Py_XDECREF(todecref);
	    dict_unref(dict);
	    RAISE_NO_خالی_تھاواں_کلیدیں;
	    أرجع -1;
	}

	إذا (!(valObject = Py_Object_Get_شے(obj, کلید_Object)))
	{
	    Py_DECREF(کلید_Object);
	    Py_DECREF(iterator);
	    Py_XDECREF(todecref);
	    dict_unref(dict);
	    أرجع -1;
	}

	di = dictitem_alloc(کلید);

	Py_DECREF(کلید_Object);
	Py_XDECREF(todecref);

	إذا (di == عدم)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(valObject);
	    dict_unref(dict);
	    Py_Err_No_حافظہ();
	    أرجع -1;
	}

	إذا (_ConvertFromPyObject(valObject, &di->di_tv, lookup_dict) == -1)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(valObject);
	    vim_free(di);
	    dict_unref(dict);
	    أرجع -1;
	}

	Py_DECREF(valObject);

	إذا (dict_جوڑو(dict, di) == FAIL)
	{
	    RAISE_کلید_جوڑو_FAIL(di->di_کلید);
	    Py_DECREF(iterator);
	    dictitem_free(di);
	    dict_unref(dict);
	    أرجع -1;
	}
    }
    Py_DECREF(iterator);
    --dict->dv_refcount;
    أرجع 0;
}

    ساكن صحيح
pyseq_منزل_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    list_T	*l;

    إذا (!(l = py_list_alloc()))
	أرجع -1;

    tv->v_type = VAR_LIST;
    tv->vval.v_list = l;

    إذا (list_py_concat(l, obj, lookup_dict) == -1)
    {
	list_unref(l);
	أرجع -1;
    }

    --l->lv_refcount;
    أرجع 0;
}

    ساكن صحيح
pytuple_منزل_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    tuple_T	*t;

    إذا (!(t = py_tuple_alloc()))
	أرجع -1;

    tv->v_type = VAR_TUPLE;
    tv->vval.v_tuple = t;

    إذا (tuple_py_concat(t, obj, lookup_dict) == -1)
    {
	tuple_unref(t);
	أرجع -1;
    }

    --t->tv_refcount;
    أرجع 0;
}

تعريفنوع صحيح (*pytotvfunc)(PyObject *, typval_T *, PyObject *);

    ساكن صحيح
convert_dl(PyObject *obj, typval_T *tv,
				    pytotvfunc py_منزل_tv, PyObject *lookup_dict)
{
    PyObject	*capsule;
    حرف	hexBuf[حجم(فراغ *) * 2 + 3];

    sprintf(hexBuf, "%p", (فراغ *)obj);

#جے_تعریف_شدہ PY_USE_CAPSULE
    capsule = Py_Dict_Get_شے_String(lookup_dict, hexBuf);
#وإلا
    capsule = (PyObject *)Py_Dict_Get_شے_String(lookup_dict, hexBuf);
#جے_انت
    إذا (capsule == عدم)
    {
#جے_تعریف_شدہ PY_USE_CAPSULE
	capsule = PyCapsule_New(tv, عدم, عدم);
#وإلا
	capsule = PyCObject_FromVoidPtr(tv, عدم);
#جے_انت
	إذا (Py_Dict_سیٹ_شے_String(lookup_dict, hexBuf, capsule))
	{
	    Py_DECREF(capsule);
	    tv->v_type = VAR_UNKNOWN;
	    أرجع -1;
	}

	Py_DECREF(capsule);

	إذا (py_منزل_tv(obj, tv, lookup_dict) == -1)
	{
	    tv->v_type = VAR_UNKNOWN;
	    أرجع -1;
	}
	// As we are not using copy_tv which increments reference count we must
	// do it ourself.
	إذا (tv->v_type == VAR_DICT)
	    ++tv->vval.v_dict->dv_refcount;
	وإلا إذا (tv->v_type == VAR_LIST)
	    ++tv->vval.v_list->lv_refcount;
	وإلا إذا (tv->v_type == VAR_TUPLE)
	    ++tv->vval.v_tuple->tv_refcount;
    }
    وإلا
    {
	typval_T	*v;

#جے_تعریف_شدہ PY_USE_CAPSULE
	v = Py_Capsule_Get_اشارہ_گر(capsule, عدم);
#وإلا
	v = PyCObject_AsVoidPtr(capsule);
#جے_انت
	copy_tv(v, tv);
    }
    أرجع 0;
}

    ساكن صحيح
ConvertFromPyMapping(PyObject *obj, typval_T *tv)
{
    PyObject	*lookup_dict;
    صحيح		ret;

    إذا (!(lookup_dict = PyDict_New()))
	أرجع -1;

    إذا (PyType_IsSubtype(obj->ob_type, DictionaryTypePtr))
    {
	tv->v_type = VAR_DICT;
	tv->vval.v_dict = (((DictionaryObject *)(obj))->dict);
	++tv->vval.v_dict->dv_refcount;
	ret = 0;
    }
    وإلا إذا (PyDict_Check(obj))
	ret = convert_dl(obj, tv, pydict_منزل_tv, lookup_dict);
    وإلا إذا (PyMapping_Check(obj))
	ret = convert_dl(obj, tv, pymap_منزل_tv, lookup_dict);
    وإلا
    {
	PyErr_FORMAT_TYPE(
		N_("unable to convert %s to a Vim dictionary"),
		obj);
	ret = -1;
    }
    Py_DECREF(lookup_dict);
    أرجع ret;
}

    ساكن صحيح
ConvertFromPySequence(PyObject *obj, typval_T *tv)
{
    PyObject	*lookup_dict;
    صحيح		ret;

    إذا (!(lookup_dict = PyDict_New()))
	أرجع -1;

    إذا (PyType_IsSubtype(obj->ob_type, ListTypePtr))
    {
	tv->v_type = VAR_LIST;
	tv->vval.v_list = (((ListObject *)(obj))->list);
	++tv->vval.v_list->lv_refcount;
	ret = 0;
    }
    وإلا إذا (PyIter_Check(obj) || PySequence_Check(obj))
	ret = convert_dl(obj, tv, pyseq_منزل_tv, lookup_dict);
    وإلا
    {
	PyErr_FORMAT_TYPE(
		N_("unable to convert %s to a Vim list"),
		obj);
	ret = -1;
    }
    Py_DECREF(lookup_dict);
    أرجع ret;
}

    ساكن صحيح
ConvertFromPyObject(PyObject *obj, typval_T *tv)
{
    PyObject	*lookup_dict;
    صحيح		ret;

    إذا (!(lookup_dict = PyDict_New()))
	أرجع -1;
    ret = _ConvertFromPyObject(obj, tv, lookup_dict);
    Py_DECREF(lookup_dict);
    أرجع ret;
}

    ساكن صحيح
_ConvertFromPyObject(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    إذا (PyType_IsSubtype(obj->ob_type, DictionaryTypePtr))
    {
	tv->v_type = VAR_DICT;
	tv->vval.v_dict = (((DictionaryObject *)(obj))->dict);
	++tv->vval.v_dict->dv_refcount;
    }
    وإلا إذا (PyType_IsSubtype(obj->ob_type, ListTypePtr))
    {
	tv->v_type = VAR_LIST;
	tv->vval.v_list = (((ListObject *)(obj))->list);
	++tv->vval.v_list->lv_refcount;
    }
    وإلا إذا (PyType_IsSubtype(obj->ob_type, TupleTypePtr))
    {
	tv->v_type = VAR_TUPLE;
	tv->vval.v_tuple = (((TupleObject *)(obj))->tuple);
	++tv->vval.v_tuple->tv_refcount;
    }
    وإلا إذا (PyType_IsSubtype(obj->ob_type, FunctionTypePtr))
    {
	FunctionObject *func = (FunctionObject *) obj;
	إذا (func->self != عدم || func->argv != عدم)
	{
	    partial_T *pt = ALLOC_CLEAR_ONE(partial_T);

	    سیٹ_partial(func, pt, TRUE);
	    tv->vval.v_partial = pt;
	    tv->v_type = VAR_PARTIAL;
	}
	وإلا
	{
	    إذا (سیٹ_string_copy(func->name, tv) == -1)
		أرجع -1;

	    tv->v_type = VAR_FUNC;
	}
	func_ref(func->name);
    }
    وإلا إذا (Py_بائٹاں_Check(obj))
    {
	char_u	*str;

	إذا (Py_بائٹاں_As_String_And_ناپ(obj, (حرف **) &str, عدم) == -1)
	    أرجع -1;
	إذا (str == عدم)
	    أرجع -1;

	إذا (سیٹ_string_copy(str, tv) == -1)
	    أرجع -1;

	tv->v_type = VAR_STRING;
    }
    وإلا إذا (PyUnicode_Check(obj))
    {
	PyObject	*بائٹاں;
	char_u	*str;

	بائٹاں = PyUnicode_AsEncodedString(obj, ENC_OPT, ERRORS_ENCODE_ARG);
	إذا (بائٹاں == عدم)
	    أرجع -1;

	إذا (Py_بائٹاں_As_String_And_ناپ(بائٹاں, (حرف **) &str, عدم) == -1)
	    أرجع -1;
	إذا (str == عدم)
	    أرجع -1;

	إذا (سیٹ_string_copy(str, tv))
	{
	    Py_XDECREF(بائٹاں);
	    أرجع -1;
	}
	Py_XDECREF(بائٹاں);

	tv->v_type = VAR_STRING;
    }
#إذا PY_MAJOR_نسخہ < 3
    وإلا إذا (PyInt_Check(obj))
    {
	tv->v_type = VAR_نمبر;
	tv->vval.v_نمبر = (varnumber_T) PyInt_AsLong(obj);
	إذا (PyErr_Occurred())
	    أرجع -1;
    }
#جے_انت
    وإلا إذا (PyLong_Check(obj))
    {
	tv->v_type = VAR_نمبر;
	tv->vval.v_نمبر = (varnumber_T) PyLong_AsLong(obj);
	إذا (PyErr_Occurred())
	    أرجع -1;
    }
    وإلا إذا (PyTuple_Check(obj))
	أرجع convert_dl(obj, tv, pytuple_منزل_tv, lookup_dict);
    وإلا إذا (PyDict_Check(obj))
	أرجع convert_dl(obj, tv, pydict_منزل_tv, lookup_dict);
    وإلا إذا (PyFloat_Check(obj))
    {
	tv->v_type = VAR_FLOAT;
	tv->vval.v_float = (float_T) PyFloat_AsDouble(obj);
    }
    وإلا إذا (PyObject_HasAttrString(obj, "keys"))
	أرجع convert_dl(obj, tv, pymap_منزل_tv, lookup_dict);
    // PyObject_GetIter can create built-in iterator for any sequence object
    وإلا إذا (PyIter_Check(obj) || PySequence_Check(obj))
	أرجع convert_dl(obj, tv, pyseq_منزل_tv, lookup_dict);
    وإلا إذا (PyMapping_Check(obj))
	أرجع convert_dl(obj, tv, pymap_منزل_tv, lookup_dict);
    وإلا إذا (Py_نمبر_Check(obj))
    {
	PyObject	*عدد;

	إذا (!(عدد = Py_نمبر_Long(obj)))
	    أرجع -1;

	tv->v_type = VAR_نمبر;
	tv->vval.v_نمبر = (varnumber_T) PyLong_AsLong(عدد);

	Py_DECREF(عدد);
    }
    وإلا إذا (obj == Py_None)
    {
	tv->v_type = VAR_SPECIAL;
	tv->vval.v_نمبر = VVAL_NONE;
    }
    وإلا
    {
	PyErr_FORMAT_TYPE(
		N_("unable to convert %s to a Vim structure"),
		obj);
	أرجع -1;
    }
    أرجع 0;
}

    ساكن PyObject *
Convert_منزل_Py_Object(typval_T *tv)
{
    typval_T *argv;
    صحيح i;
    إذا (tv == عدم)
    {
	Py_Err_سیٹ_VIM(N_("internal error: NULL reference passed"));
	أرجع عدم;
    }
    بدل (tv->v_type)
    {
	حالة VAR_STRING:
	    أرجع Py_بائٹاں_From_String(tv->vval.v_string == عدم
					    ? "" : (حرف *)tv->vval.v_string);
	حالة VAR_نمبر:
	    أرجع PyLong_FromLong((لما) tv->vval.v_نمبر);
	حالة VAR_FLOAT:
	    أرجع PyFloat_FromDouble((مزدوج) tv->vval.v_float);
	حالة VAR_LIST:
	    أرجع NEW_LIST(tv->vval.v_list);
	حالة VAR_TUPLE:
	    أرجع NEW_TUPLE(tv->vval.v_tuple);
	حالة VAR_DICT:
	    أرجع NEW_DICTIONARY(tv->vval.v_dict);
	حالة VAR_FUNC:
	    أرجع NEW_FUNCTION(tv->vval.v_string == عدم
					  ? (char_u *)"" : tv->vval.v_string,
					  0, عدم, عدم, TRUE);
	حالة VAR_PARTIAL:
	    إذا (tv->vval.v_partial->pt_argc)
	    {
		argv = PyMem_New(typval_T, (size_t)tv->vval.v_partial->pt_argc);
		لكل (i = 0; i < tv->vval.v_partial->pt_argc; i++)
		    copy_tv(&tv->vval.v_partial->pt_argv[i], &argv[i]);
	    }
	    وإلا
		argv = عدم;
	    إذا (tv->vval.v_partial->pt_dict != عدم)
		tv->vval.v_partial->pt_dict->dv_refcount++;
	    أرجع NEW_FUNCTION(tv->vval.v_partial == عدم
			     ? (char_u *)"" : partial_name(tv->vval.v_partial),
				tv->vval.v_partial->pt_argc, argv,
				tv->vval.v_partial->pt_dict,
				tv->vval.v_partial->pt_auto);
	حالة VAR_BLOB:
	    أرجع Py_بائٹاں_From_String_And_ناپ(
		(حرف*) tv->vval.v_blob->bv_ga.ga_انکڑے,
		(Py_ssize_t) tv->vval.v_blob->bv_ga.ga_len);
	حالة VAR_UNKNOWN:
	حالة VAR_ANY:
	حالة VAR_VOID:
	حالة VAR_CHANNEL:
	حالة VAR_کم:
	حالة VAR_INSTR:
	حالة VAR_CLASS:
	حالة VAR_OBJECT:
	حالة VAR_TYPEALIAS:
	    Py_INCREF(Py_None);
	    أرجع Py_None;
	حالة VAR_BOOL:
	حالة VAR_SPECIAL:
	    بدل (tv->vval.v_نمبر)
	    {
		حالة VVAL_FALSE: أرجع ALWAYS_FALSE;
		حالة VVAL_TRUE:  أرجع ALWAYS_TRUE;
		حالة VVAL_NONE:
		حالة VVAL_NULL:  أرجع ALWAYS_NONE;
	    }
	    Py_Err_سیٹ_VIM(N_("internal error: invalid value type"));
	    أرجع عدم;
    }
    أرجع عدم;
}

DEFINE_PY_TYPE_OBJECT(CurrentType);

تعريفنوع بنية
{
    Py_Object_سر
} CurrentObject;

ساكن CurrentObject TheCurrent =
{
    Py_Object_سر_INIT_TYPE(CurrentType)
};

    ساكن فراغ
init_structs(فراغ)
{
    CLEAR_میدان(OutputType);
    OutputType.tp_name = "vim.message";
    OutputType.tp_basicsize = حجم(OutputObject);
    OutputType.tp_flags = Py_TPFLAGS_DEFAULT;
    OutputType.tp_doc = "vim message object";
    OutputType.tp_methods = OutputMethods;
#إذا PY_MAJOR_نسخہ >= 3
    OutputType.tp_getattro = OutputGetattro;
    OutputType.tp_setattro = OutputSetattro;
    OutputType.tp_alloc = call_PyType_GenericAlloc;
    OutputType.tp_new = call_PyType_GenericNew;
    OutputType.tp_free = call_PyObject_Free;
# ifndef USE_LIMITED_API
    // The std printer type is only exposed in full API. It is not essential
    // anyway and so in limited API we don't set it.
    OutputType.tp_ادھار = &PyStdPrinter_Type;
# endif
#وإلا
    OutputType.tp_getattr = OutputGetattr;
    OutputType.tp_setattr = OutputSetattr;
    // Disabled, because this causes a crash in test86
    // OutputType.tp_base = &PyFile_Type;
#جے_انت

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
#إذا PY_MAJOR_نسخہ >= 3
    بفر_Type.tp_getattro = بفر_Getattro;
    بفر_Type.tp_setattro = بفر_Setattro;
    بفر_Type.tp_alloc = call_PyType_GenericAlloc;
    بفر_Type.tp_new = call_PyType_GenericNew;
    بفر_Type.tp_free = call_PyObject_Free;
#وإلا
    بفر_Type.tp_getattr = بفر_Getattr;
    بفر_Type.tp_setattr = بفر_Setattr;
#جے_انت

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
#إذا PY_MAJOR_نسخہ >= 3
    کھڑکی_Type.tp_getattro = کھڑکی_Getattro;
    کھڑکی_Type.tp_setattro = کھڑکی_Setattro;
    کھڑکی_Type.tp_alloc = call_PyType_GenericAlloc;
    کھڑکی_Type.tp_new = call_PyType_GenericNew;
    کھڑکی_Type.tp_free = call_PyObject_Free;
#وإلا
    کھڑکی_Type.tp_getattr = کھڑکی_Getattr;
    کھڑکی_Type.tp_setattr = کھڑکی_Setattr;
#جے_انت

    CLEAR_میدان(Tab_ورقہ_Type);
    Tab_ورقہ_Type.tp_name = "vim.tabpage";
    Tab_ورقہ_Type.tp_basicsize = حجم(Tab_ورقہ_Object);
    Tab_ورقہ_Type.tp_dealloc = Tab_ورقہ_Destructor;
    Tab_ورقہ_Type.tp_repr = Tab_ورقہ_Repr;
    Tab_ورقہ_Type.tp_flags = Py_TPFLAGS_DEFAULT;
    Tab_ورقہ_Type.tp_doc = "vim tab page object";
    Tab_ورقہ_Type.tp_methods = Tab_ورقہ_Methods;
#إذا PY_MAJOR_نسخہ >= 3
    Tab_ورقہ_Type.tp_getattro = Tab_ورقہ_Getattro;
    Tab_ورقہ_Type.tp_alloc = call_PyType_GenericAlloc;
    Tab_ورقہ_Type.tp_new = call_PyType_GenericNew;
    Tab_ورقہ_Type.tp_free = call_PyObject_Free;
#وإلا
    Tab_ورقہ_Type.tp_getattr = Tab_ورقہ_Getattr;
#جے_انت

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
#إذا PY_MAJOR_نسخہ >= 3
    RangeType.tp_getattro = RangeGetattro;
    RangeType.tp_alloc = call_PyType_GenericAlloc;
    RangeType.tp_new = call_PyType_GenericNew;
    RangeType.tp_free = call_PyObject_Free;
#وإلا
    RangeType.tp_getattr = RangeGetattr;
#جے_انت

    CLEAR_میدان(CurrentType);
    CurrentType.tp_name = "vim.currentdata";
    CurrentType.tp_basicsize = حجم(CurrentObject);
    CurrentType.tp_flags = Py_TPFLAGS_DEFAULT;
    CurrentType.tp_doc = "vim current object";
    CurrentType.tp_methods = CurrentMethods;
#إذا PY_MAJOR_نسخہ >= 3
    CurrentType.tp_getattro = CurrentGetattro;
    CurrentType.tp_setattro = CurrentSetattro;
#وإلا
    CurrentType.tp_getattr = CurrentGetattr;
    CurrentType.tp_setattr = CurrentSetattr;
#جے_انت

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
#إذا PY_MAJOR_نسخہ >= 3
    DictionaryType.tp_getattro = DictionaryGetattro;
    DictionaryType.tp_setattro = DictionarySetattro;
#وإلا
    DictionaryType.tp_getattr = DictionaryGetattr;
    DictionaryType.tp_setattr = DictionarySetattr;
#جے_انت

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
#إذا PY_MAJOR_نسخہ >= 3
    ListType.tp_getattro = ListGetattro;
    ListType.tp_setattro = ListSetattro;
#وإلا
    ListType.tp_getattr = ListGetattr;
    ListType.tp_setattr = ListSetattr;
#جے_انت

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
#إذا PY_MAJOR_نسخہ >= 3
    TupleType.tp_getattro = TupleGetattro;
    TupleType.tp_setattro = TupleSetattro;
#وإلا
    TupleType.tp_getattr = TupleGetattr;
    TupleType.tp_setattr = TupleSetattr;
#جے_انت

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
#إذا PY_MAJOR_نسخہ >= 3
    FunctionType.tp_getattro = FunctionGetattro;
#وإلا
    FunctionType.tp_getattr = FunctionGetattr;
#جے_انت

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

#إذا PY_نسخہ_HEX < 0x030700f0
    CLEAR_میدان(LoaderType);
    LoaderType.tp_name = "vim.Loader";
    LoaderType.tp_basicsize = حجم(LoaderObject);
    LoaderType.tp_flags = Py_TPFLAGS_DEFAULT;
    LoaderType.tp_doc = "vim message object";
    LoaderType.tp_methods = LoaderMethods;
    LoaderType.tp_dealloc = LoaderDestructor;
#جے_انت

#إذا PY_MAJOR_نسخہ >= 3
    CLEAR_میدان(vimmodule);
    vimmodule.m_name = "vim";
    vimmodule.m_doc = "Vim Python interface\n";
    vimmodule.m_ناپ_2 = -1;
    vimmodule.m_methods = VimMethods;
#جے_انت
}

    ساكن صحيح
init_types(فراغ)
{
    PYTYPE_READY(IterType);
    PYTYPE_READY(بفر_Type);
    PYTYPE_READY(RangeType);
    PYTYPE_READY(کھڑکی_Type);
    PYTYPE_READY(Tab_ورقہ_Type);
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
#إذا PY_نسخہ_HEX < 0x030700f0
    PYTYPE_READY(LoaderType);
#جے_انت

#جے_تعریف_شدہ USE_LIMITED_API
    // We need to finish initializing all the static objects because the types
    // are only just allocated on the heap now.
    // Each PyObject_HEAD_INIT_TYPE should correspond to a
    // PyObject_FINISH_INIT_TYPE below.
    Py_Object_انت_INIT_TYPE(Output, OutputType);
    Py_Object_انت_INIT_TYPE(Error, OutputType);
    Py_Object_انت_INIT_TYPE(The_بفر_Map, BufMapType);
    Py_Object_انت_INIT_TYPE(The_کھڑکی_List, WinListType);
    Py_Object_انت_INIT_TYPE(TheCurrent, CurrentType);
    Py_Object_انت_INIT_TYPE(The_Tab_ورقہ_List, TabListType);
#جے_انت
    أرجع 0;
}

#جے_تعریف_شدہ USE_LIMITED_API
    ساكن فراغ
shutdown_types(فراغ)
{
    PYTYPE_CLEANUP(IterType);
    PYTYPE_CLEANUP(بفر_Type);
    PYTYPE_CLEANUP(RangeType);
    PYTYPE_CLEANUP(کھڑکی_Type);
    PYTYPE_CLEANUP(Tab_ورقہ_Type);
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
# إذا PY_نسخہ_HEX < 0x030700f0
    PYTYPE_CLEANUP(LoaderType);
# endif
}
#جے_انت

    ساكن صحيح
init_sys_رستہ(فراغ)
{
    PyObject	*رستہ;
    PyObject	*رستہ_hook;
    PyObject	*رستہ_hooks;

    إذا (!(رستہ_hook = PyObject_GetAttrString(vim_module, "path_hook")))
	أرجع -1;

    إذا (!(رستہ_hooks = PySys_GetObject("path_hooks")))
    {
	PyErr_Clear();
	رستہ_hooks = PyList_New(1);
	Py_List_سیٹ_شے(رستہ_hooks, 0, رستہ_hook);
	إذا (Py_Sys_سیٹ_Object("path_hooks", رستہ_hooks))
	{
	    Py_DECREF(رستہ_hooks);
	    أرجع -1;
	}
	Py_DECREF(رستہ_hooks);
    }
    وإلا إذا (PyList_Check(رستہ_hooks))
    {
	إذا (PyList_Append(رستہ_hooks, رستہ_hook))
	{
	    Py_DECREF(رستہ_hook);
	    أرجع -1;
	}
	Py_DECREF(رستہ_hook);
    }
    وإلا
    {
	Vim_Try_شروع();
	emsg(_("Failed to set path hook: sys.path_hooks is not a list\n"
	       "You should now do the following:\n"
	       "- append vim.path_hook to sys.path_hooks\n"
	       "- append vim.VIM_SPECIAL_PATH to sys.path\n"));
	Vim_Try_سرا(); // Discard the error
	Py_DECREF(رستہ_hook);
	أرجع 0;
    }

    إذا (!(رستہ = PySys_GetObject("path")))
    {
	PyErr_Clear();
	رستہ = PyList_New(1);
	Py_INCREF(vim_special_رستہ_object);
	Py_List_سیٹ_شے(رستہ, 0, vim_special_رستہ_object);
	إذا (Py_Sys_سیٹ_Object("path", رستہ))
	{
	    Py_DECREF(رستہ);
	    أرجع -1;
	}
	Py_DECREF(رستہ);
    }
    وإلا إذا (PyList_Check(رستہ))
    {
	إذا (PyList_Append(رستہ, vim_special_رستہ_object))
	    أرجع -1;
    }
    وإلا
    {
	Vim_Try_شروع();
	emsg(_("Failed to set path: sys.path is not a list\n"
	       "You should now append vim.VIM_SPECIAL_PATH to sys.path"));
	Vim_Try_سرا(); // Discard the error
    }

    أرجع 0;
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

#تعریف_کرو جوڑو_OBJECT(m, name, obj) \
    إذا (Py_Module_جوڑو_Object(m, name, obj)) \
	أرجع -1;

#تعریف_کرو جوڑو_CHECKED_OBJECT(m, name, obj) \
    { \
	PyObject	*valObject = obj; \
	إذا (!valObject) \
	    أرجع -1; \
	جوڑو_OBJECT(m, name, valObject); \
    }

    ساكن صحيح
populate_module(PyObject *m)
{
    صحيح		i;
    PyObject	*other_module;
    PyObject	*attr;
    PyObject	*imp;
#إذا PY_نسخہ_HEX >= 0x030700f0
    PyObject	*dict;
    PyObject	*cls;
#جے_انت

    لكل (i = 0; i < (صحيح)(حجم(numeric_constants)
					   / حجم(بنية numeric_constant));
	    ++i)
	جوڑو_CHECKED_OBJECT(m, numeric_constants[i].name,
		PyInt_FromLong(numeric_constants[i].val));

    بنية object_constant object_constants[] = {
	{"buffers",  (PyObject *)(فراغ *)&The_بفر_Map},
	{"windows",  (PyObject *)(فراغ *)&The_کھڑکی_List},
	{"tabpages", (PyObject *)(فراغ *)&The_Tab_ورقہ_List},
	{"current",  (PyObject *)(فراغ *)&TheCurrent},

	{"Buffer",     (PyObject *)بفر_Type_Ptr},
	{"Range",      (PyObject *)RangeTypePtr},
	{"Window",     (PyObject *)کھڑکی_Type_Ptr},
	{"TabPage",    (PyObject *)Tab_ورقہ_Type_Ptr},
	{"Dictionary", (PyObject *)DictionaryTypePtr},
	{"List",       (PyObject *)ListTypePtr},
	{"Tuple",      (PyObject *)TupleTypePtr},
	{"Function",   (PyObject *)FunctionTypePtr},
	{"Options",    (PyObject *)OptionsTypePtr},
#إذا PY_نسخہ_HEX < 0x030700f0
	{"_Loader",    (PyObject *)LoaderTypePtr},
#جے_انت
    };

    لكل (i = 0; i < (صحيح)(حجم(object_constants)
					    / حجم(بنية object_constant));
	    ++i)
    {
	PyObject	*valObject;

	valObject = object_constants[i].valObject;
	Py_INCREF(valObject);
	جوڑو_OBJECT(m, object_constants[i].name, valObject);
    }

    إذا (!(VimError = PyErr_NewException("vim.error", عدم, عدم)))
	أرجع -1;
    جوڑو_OBJECT(m, "error", VimError);

    جوڑو_CHECKED_OBJECT(m, "vars",  NEW_DICTIONARY(get_globvar_dict()));
    جوڑو_CHECKED_OBJECT(m, "vvars", NEW_DICTIONARY(get_vimvar_dict()));
    جوڑو_CHECKED_OBJECT(m, "options",
	    OptionsNew(SREQ_GLOBAL, عدم, dummy_check, عدم));

    إذا (!(other_module = PyImport_ImportModule("os")))
	أرجع -1;
    جوڑو_OBJECT(m, "os", other_module);

#إذا PY_MAJOR_نسخہ >= 3
    إذا (!(py_getcwd = PyObject_GetAttrString(other_module, "getcwd")))
	أرجع -1;
#وإلا
    إذا (!(py_getcwd = PyObject_GetAttrString(other_module, "getcwdu")))
	أرجع -1;
#جے_انت
    جوڑو_OBJECT(m, "_getcwd", py_getcwd)

    إذا (!(py_chdir = PyObject_GetAttrString(other_module, "chdir")))
	أرجع -1;
    جوڑو_OBJECT(m, "_chdir", py_chdir);
    إذا (!(attr = PyObject_GetAttrString(m, "chdir")))
	أرجع -1;
    إذا (Py_Object_سیٹ_Attr_String(other_module, "chdir", attr))
    {
	Py_DECREF(attr);
	أرجع -1;
    }
    Py_DECREF(attr);

    إذا ((py_fchdir = PyObject_GetAttrString(other_module, "fchdir")))
    {
	جوڑو_OBJECT(m, "_fchdir", py_fchdir);
	إذا (!(attr = PyObject_GetAttrString(m, "fchdir")))
	    أرجع -1;
	إذا (Py_Object_سیٹ_Attr_String(other_module, "fchdir", attr))
	{
	    Py_DECREF(attr);
	    أرجع -1;
	}
	Py_DECREF(attr);
    }
    وإلا
	PyErr_Clear();

    إذا (!(vim_special_رستہ_object = PyString_FromString(vim_special_رستہ)))
	أرجع -1;

    جوڑو_OBJECT(m, "VIM_SPECIAL_PATH", vim_special_رستہ_object);

#إذا PY_نسخہ_HEX >= 0x030700f0
    إذا (!(imp = PyImport_ImportModule("importlib.machinery")))
	أرجع -1;

    dict = PyModule_GetDict(imp);

    إذا (!(cls = Py_Dict_Get_شے_String(dict, "PathFinder")))
    {
	Py_DECREF(imp);
	أرجع -1;
    }

    إذا (!(py_لبھو_spec = PyObject_GetAttrString(cls, "find_spec")))
    {
	Py_DECREF(imp);
	أرجع -1;
    }

# إذا PY_نسخہ_HEX < 0x30c00a7
    // find_module has been removed as of Python 3.12.0a7
    إذا ((py_لبھو_module = PyObject_GetAttrString(cls, "find_module")))
	جوڑو_OBJECT(m, "_find_module", py_لبھو_module);
# endif

    Py_DECREF(imp);

    جوڑو_OBJECT(m, "_find_spec", py_لبھو_spec);
#وإلا
    إذا (!(imp = PyImport_ImportModule("imp")))
	أرجع -1;

    إذا (!(py_لبھو_module = PyObject_GetAttrString(imp, "find_module")))
    {
	Py_DECREF(imp);
	أرجع -1;
    }

    إذا (!(py_لدو_module = PyObject_GetAttrString(imp, "load_module")))
    {
	Py_DECREF(py_لبھو_module);
	Py_DECREF(imp);
	أرجع -1;
    }

    Py_DECREF(imp);

    جوڑو_OBJECT(m, "_find_module", py_لبھو_module);
    جوڑو_OBJECT(m, "_load_module", py_لدو_module);
#جے_انت

    أرجع 0;
}
