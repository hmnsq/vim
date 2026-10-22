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

#إذا PY_نسخه_HEX < 0x02050000
تعريفنوع صحيح Py_ssize_t;  // Python 2.4 and earlier don't have this type.
#د_که_پای

// Use values that are known to work, others may make Vim crash.
#تعریف_کړه ENC_OPT (enc_utf8 ? "utf-8" : enc_dbcs ? "euc-jp" : (حرف *)p_enc)
#تعریف_کړه DOPY_FUNC "_vim_pydo"

ساكن ثابت حرف *vim_special_لار = "_vim_path_";

#تعریف_کړه Py_Err_سېټ_STRING(exc, str) Py_Err_سېټ_String(exc, _(str))
#تعریف_کړه Py_Err_سېټ_Vim(str) Py_Err_سېټ_String(VimError, str)
#تعریف_کړه Py_Err_سېټ_VIM(str) Py_Err_سېټ_STRING(VimError, str)
#تعریف_کړه PyErr_FORMAT(exc, str, arg) PyErr_Format(exc, _(str), arg)
#تعریف_کړه PyErr_FORMAT2(exc, str, arg1, arg2) PyErr_Format(exc, _(str), arg1,arg2)
#تعریف_کړه PyErr_VIM_FORMAT(str, arg) PyErr_FORMAT(VimError, str, arg)

#که_تعریف_شوی USE_LIMITED_API
// Limited Python API. Need to call only exposed functions and remap macros.
// PyTypeObject is an opaque struct.

تعريفنوع بنية {
    lenfunc sq_اوږدتیا;
    binaryfunc sq_concat;
    ssizeargfunc sq_repeat;
    ssizeargfunc sq_توکی;
    فراغ *was_sq_ټوټه;
    ssizeobjargproc sq_ass_توکی;
    فراغ *was_sq_ass_ټوټه;
    objobjproc sq_لري;

    binaryfunc sq_inplace_concat;
    ssizeargfunc sq_inplace_repeat;
} PySequenceMethods;

تعريفنوع بنية {
    lenfunc mp_اوږدتیا;
    binaryfunc mp_subscript;
    objobjargproc mp_ass_subscript;
} PyMappingMethods;

// This struct emulates the concrete _typeobject struct to allow the code to
// work the same way in both limited and full Python APIs.
بنية typeobject_wrapper {
    ثابت حرف *tp_name;
    Py_ssize_t tp_basicsize;
    بینښه اوږد tp_flags;

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

    بنية Py_لاره_Def *tp_methods;
    بنية _typeobject *tp_بنسټ;
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
# define Py_Object_پای_INIT_TYPE(obj, type) obj.ob_بنسټ.ob_type = type##Ptr

# define Py_TYPE_GET_TP_ALLOC(type) ((allocfunc)Py_Type_Get_خونه(type, Py_tp_alloc))
# define Py_TYPE_GET_TP_METHODS(type) ((Py_لاره_Def *)Py_Type_Get_خونه(type, Py_tp_methods))

// PyObject_NEW is not part of stable ABI, but PyObject_Malloc/Init are.
ساكن PyObject* Vim_PyObject_New(PyTypeObject *type, size_t objsize)
{
    PyObject *obj = (PyObject *)PyObject_Malloc(objsize);
    إذا (obj == عدم)
	أرجع Py_Err_No_حافظه();
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
	PyObject* ډک; \
	إذا (module == عدم || PyUnicode_CompareWithASCIIString(module, "builtins") == 0 \
		|| PyUnicode_CompareWithASCIIString(module, "__main__") == 0) \
	{ \
	    ډک = qualname; \
	    Py_INCREF(ډک); \
	} \
	وإلا \
	    ډک = PyUnicode_FromFormat("%U.%U", module, qualname); \
	PyObject* ډک_بایټونه = PyUnicode_AsUTF8String(ډک); \
	ثابت حرف* ډک_str = Py_بایټونه_As_String(ډک_بایټونه); \
	ډک_str = ډک_str == عدم ? "(NULL)" : ډک_str; \
	PyErr_FORMAT(PyExc_TypeError, پیغام, ډک_str); \
	Py_DECREF(qualname); \
	Py_XDECREF(module); \
	Py_XDECREF(ډک); \
	Py_XDECREF(ډک_بایټونه); \
    } طالما (0)

# define Py_List_GET_توکی(list, i) Py_List_Get_توکی(list, i)
# define Py_List_GET_اندازه(o) Py_List_اندازه(o)

// PyList_SET_ITEM and PyList_SetItem have slightly different behaviors. The
// former will leave the old item dangling, and the latter will decref on it.
// Since we only use this on new lists, this difference doesn't matter.
# define Py_List_سېټ_توکی(list, i, توکی) Py_List_سېټ_توکی_2(list, i, توکی)

# define Py_Tuple_GET_توکی(o, pos) Py_Tuple_Get_توکی(o, pos)
# define Py_Tuple_GET_اندازه(o) Py_Tuple_اندازه(o)

# إذا Py_LIMITED_API < 0x03080000
// PyIter_check only became part of stable ABI in 3.8, and there is no easy way
// to check for it in the API. We simply return false as a compromise. This
// does mean we should avoid compiling with stable ABI < 3.8.
#  undef PyIter_Check
#  define PyIter_Check(obj) (FALSE)
# endif

ساكن PyTypeObject* جمع_کړه_ډېرۍ_Type(بنية typeobject_wrapper* type_object)
{
    PyType_Spec type_spec;
    type_spec.name = type_object->tp_name;
    type_spec.basicsize = type_object->tp_basicsize;
    type_spec.itemsize = 0;
    type_spec.flags = type_object->tp_flags;

    // We just need to statically allocate a large enough buffer that can hold
    // all slots. We need to leave a null-terminated slot at the end.
    Py_Type_خونه slots[40] = { {0, عدم} };
    size_t خونه_i = 0;

# define جمع_کړه_TP_خونه(خونه_name) \
    إذا (خونه_i >= 40) أرجع عدم; /* this should never happen */ \
    إذا (type_object->خونه_name != عدم) \
    { \
	slots[خونه_i].خونه = Py_##slot_name; \
	slots[خونه_i].pfunc = (فراغ*)type_object->خونه_name; \
	++خونه_i; \
    }
# define جمع_کړه_TP_SUB_خونه(sub_خونه, خونه_name) \
    إذا (خونه_i >= 40) أرجع عدم; /* this should never happen */ \
    إذا (type_object->sub_خونه != عدم && type_object->sub_خونه->خونه_name != عدم) \
    { \
	slots[خونه_i].خونه = Py_##slot_name; \
	slots[خونه_i].pfunc = (فراغ*)type_object->sub_خونه->خونه_name; \
	++خونه_i; \
    }

    جمع_کړه_TP_خونه(tp_dealloc)
    جمع_کړه_TP_خونه(tp_repr)
    جمع_کړه_TP_خونه(tp_call)
    جمع_کړه_TP_خونه(tp_getattro)
    جمع_کړه_TP_خونه(tp_setattro)
    جمع_کړه_TP_خونه(tp_doc)
    جمع_کړه_TP_خونه(tp_traverse)
    جمع_کړه_TP_خونه(tp_clear)
    جمع_کړه_TP_خونه(tp_iter)
    جمع_کړه_TP_خونه(tp_iternext)
    جمع_کړه_TP_خونه(tp_methods)
    جمع_کړه_TP_خونه(tp_بنسټ)
    جمع_کړه_TP_خونه(tp_alloc)
    جمع_کړه_TP_خونه(tp_new)
    جمع_کړه_TP_خونه(tp_free)

    جمع_کړه_TP_SUB_خونه(tp_as_sequence, sq_اوږدتیا)
    جمع_کړه_TP_SUB_خونه(tp_as_sequence, sq_concat)
    جمع_کړه_TP_SUB_خونه(tp_as_sequence, sq_repeat)
    جمع_کړه_TP_SUB_خونه(tp_as_sequence, sq_توکی)
    جمع_کړه_TP_SUB_خونه(tp_as_sequence, sq_ass_توکی)
    جمع_کړه_TP_SUB_خونه(tp_as_sequence, sq_لري)
    جمع_کړه_TP_SUB_خونه(tp_as_sequence, sq_inplace_concat)
    جمع_کړه_TP_SUB_خونه(tp_as_sequence, sq_inplace_repeat)

    جمع_کړه_TP_SUB_خونه(tp_as_mapping, mp_اوږدتیا)
    جمع_کړه_TP_SUB_خونه(tp_as_mapping, mp_subscript)
    جمع_کړه_TP_SUB_خونه(tp_as_mapping, mp_ass_subscript)
# undef جمع_کړه_TP_خونه
# undef جمع_کړه_TP_SUB_خونه

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
    type##Ptr = جمع_کړه_ډېرۍ_Type(&(type)); \
    إذا (type##Ptr == عدم) \
	أرجع -1;
# define PYTYPE_CLEANUP(type) \
    type##Ptr = عدم;

// Limited API does not provide PyRun_* functions. Need to implement manually
// using PyCompile and PyEval.
ساكن PyObject* Vim_Py_پرلپسې_String(ثابت حرف *str, صحيح پیل, PyObject *globals, PyObject *locals)
{
    // Just pass "" for filename for now.
    PyObject* compiled = Py_CompileString(str, "", پیل);
    إذا (compiled == عدم)
	أرجع عدم;

    PyObject* eval_پایله = Py_Eval_Eval_کوډ(compiled, globals, locals);
    Py_DECREF(compiled);
    أرجع eval_پایله;
}
ساكن صحيح Vim_Py_پرلپسې_Simple_String(ثابت حرف *str)
{
    // This function emulates CPython's implementation.
    PyObject* m = Py_Import_جمع_کړه_Module("__main__");
    إذا (m == عدم)
	أرجع -1;
    PyObject* d = PyModule_GetDict(m);
    PyObject* output = Vim_Py_پرلپسې_String(str, Py_file_ننوت, d, d);
    إذا (output == عدم)
    {
	PyErr_PrintEx(TRUE);
	أرجع -1;
    }
    Py_DECREF(output);
    أرجع 0;
}
# define Py_پرلپسې_String Vim_Py_پرلپسې_String
# define Py_پرلپسې_Simple_String Vim_Py_پرلپسې_Simple_String

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

#د_که_پای


#تعریف_کړه RAISE_NO_تشه_کلیګانې Py_Err_سېټ_STRING(Py_Exc_ارزښت_Error, \
					    N_("empty keys are not allowed"))
#تعریف_کړه RAISE_LOCKED_DICTIONARY Py_Err_سېټ_VIM(N_("dictionary is locked"))
#تعریف_کړه RAISE_LOCKED_LIST Py_Err_سېټ_VIM(N_("list is locked"))
#تعریف_کړه RAISE_UNDO_FAIL Py_Err_سېټ_VIM(N_("cannot save undo information"))
#تعریف_کړه RAISE_DELETE_کرښه_FAIL Py_Err_سېټ_VIM(N_("cannot delete line"))
#تعریف_کړه RAISE_ورننباسه_کرښه_FAIL Py_Err_سېټ_VIM(N_("cannot insert line"))
#تعریف_کړه RAISE_REPLACE_کرښه_FAIL Py_Err_سېټ_VIM(N_("cannot replace line"))
#تعریف_کړه RAISE_کلی_جمع_کړه_FAIL(کلی) \
    PyErr_VIM_FORMAT(N_("failed to add key '%s' to dictionary"), کلی)
#تعریف_کړه RAISE_INVALID_شاخص_TYPE(idx) \
    PyErr_FORMAT_TYPE(N_("index must be int or slice, not %s"), idx);

#تعریف_کړه INVALID_بفر_ارزښت ((buf_T *)(-1))
#تعریف_کړه INVALID_کړکۍ_ارزښت ((win_T *)(-1))
#تعریف_کړه INVALID_TABPAGE_ارزښت ((tabpage_T *)(-1))

تعريفنوع فراغ (*rangeinitializer)(فراغ *);
تعريفنوع فراغ (*runner)(ثابت حرف *, dict_T *, فراغ *
#که_تعریف_شوی PY_CAN_RECURSE
	, PyGILState_STATE *
#د_که_پای
	);

ساكن صحيح ConvertFromPyObject(PyObject *, typval_T *);
ساكن صحيح _ConvertFromPyObject(PyObject *, typval_T *, PyObject *);
ساكن صحيح ConvertFromPyMapping(PyObject *, typval_T *);
ساكن صحيح ConvertFromPySequence(PyObject *, typval_T *);
ساكن PyObject *کړکۍ_New(win_T *, tabpage_T *);
ساكن PyObject *بفر_New (buf_T *);
ساكن PyObject *کرښه_منزل_String(ثابت حرف *);

ساكن PyInt Range_پیل;
ساكن PyInt Range_پای_ټکی;

ساكن PyObject *globals;

ساكن PyObject *py_chdir;
ساكن PyObject *py_fchdir;
ساكن PyObject *py_getcwd;
ساكن PyObject *vim_module;
ساكن PyObject *vim_special_لار_object;

#إذا PY_نسخه_HEX >= 0x030700f0
ساكن PyObject *py_پیدا_کړه_spec;
#وإلا
ساكن PyObject *py_بارول_module;
#د_که_پای
#إذا PY_نسخه_HEX < 0x30c00a7
ساكن PyObject *py_پیدا_کړه_module;
#د_که_پای

ساكن PyObject *VimError;

/*
 * obtain a lock on the Vim data structures
 */
    ساكن فراغ
Python_قلف_Vim(فراغ)
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

    إذا (Py_بایټونه_Check(obj))
    {

	إذا (Py_بایټونه_As_String_And_اندازه(obj, (حرف **) &str, عدم) == -1
		|| str == عدم)
	    أرجع عدم;

	*todecref = عدم;
    }
    وإلا إذا (PyUnicode_Check(obj))
    {
	PyObject	*بایټونه;

	إذا (!(بایټونه = PyUnicode_AsEncodedString(obj, ENC_OPT,
							   ERRORS_ENCODE_ARG)))
	    أرجع عدم;

	إذا (Py_بایټونه_As_String_And_اندازه(بایټونه, (حرف **) &str, عدم) == -1
		|| str == عدم)
	{
	    Py_DECREF(بایټونه);
	    أرجع عدم;
	}

	*todecref = بایټونه;
    }
    وإلا
    {
#إذا PY_MAJOR_نسخه < 3
	PyErr_FORMAT_TYPE(N_("expected str() or unicode() instance, but got %s"),
		obj);
#وإلا
	PyErr_FORMAT_TYPE(N_("expected bytes() or str() instance, but got %s"),
		obj);
#د_که_پای
	أرجع عدم;
    }

    أرجع (char_u *) str;
}

#تعریف_کړه شمېره_LONG     1
#تعریف_کړه شمېره_INT      2
#تعریف_کړه شمېره_NATURAL  4
#تعریف_کړه شمېره_UNSIGNED 8

    ساكن صحيح
شمېره_منزل_Long(PyObject *obj, اوږد *پایله, صحيح flags)
{
#إذا PY_MAJOR_نسخه < 3
    إذا (PyInt_Check(obj))
    {
	*پایله = PyInt_AsLong(obj);
	إذا (PyErr_Occurred())
	    أرجع -1;
    }
    وإلا
#د_که_پای
    إذا (PyLong_Check(obj))
    {
	*پایله = PyLong_AsLong(obj);
	إذا (PyErr_Occurred())
	    أرجع -1;
    }
    وإلا إذا (Py_شمېره_Check(obj))
    {
	PyObject	*عدد;

	إذا (!(عدد = Py_شمېره_Long(obj)))
	    أرجع -1;

	*پایله = PyLong_AsLong(عدد);

	Py_DECREF(عدد);

	إذا (PyErr_Occurred())
	    أرجع -1;
    }
    وإلا
    {
#إذا PY_MAJOR_نسخه < 3
	PyErr_FORMAT_TYPE(
		N_("expected int(), long() or something supporting "
		   "coercing to long(), but got %s"),
		obj);
#وإلا
	PyErr_FORMAT_TYPE(
		N_("expected int() or something supporting coercing to int(), "
		   "but got %s"),
		obj);
#د_که_پای
	أرجع -1;
    }

    إذا (flags & شمېره_INT)
    {
	إذا (*پایله > لوی_عدد)
	{
	    Py_Err_سېټ_STRING(Py_Exc_ډکېدنه_Error,
		    N_("value is too large to fit into C int type"));
	    أرجع -1;
	}
	وإلا إذا (*پایله < INT_MIN)
	{
	    Py_Err_سېټ_STRING(Py_Exc_ډکېدنه_Error,
		    N_("value is too small to fit into C int type"));
	    أرجع -1;
	}
    }

    إذا (flags & شمېره_NATURAL)
    {
	إذا (*پایله <= 0)
	{
	    Py_Err_سېټ_STRING(Py_Exc_ارزښت_Error,
		    N_("number must be greater than zero"));
	    أرجع -1;
	}
    }
    وإلا إذا (flags & شمېره_UNSIGNED)
    {
	إذا (*پایله < 0)
	{
	    Py_Err_سېټ_STRING(Py_Exc_ارزښت_Error,
		    N_("number must be greater or equal to zero"));
	    أرجع -1;
	}
    }

    أرجع 0;
}

    ساكن صحيح
جمع_کړه_string(PyObject *list, حرف *s)
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
ObjectDir(PyObject *self, حرف **ځانګړتیاوې)
{
    Py_لاره_Def	*لاره;
    حرف	**attr;
    PyObject	*ret;

    إذا (!(ret = PyList_New(0)))
	أرجع عدم;

    إذا (self)
	لكل (لاره = Py_TYPE_GET_TP_METHODS(self->ob_type) ; لاره->ml_name != عدم ; ++لاره)
	    إذا (جمع_کړه_string(ret, (حرف *)لاره->ml_name))
	    {
		Py_DECREF(ret);
		أرجع عدم;
	    }

    لكل (attr = ځانګړتیاوې ; *attr ; ++attr)
	إذا (جمع_کړه_string(ret, *attr))
	{
	    Py_DECREF(ret);
	    أرجع عدم;
	}

#إذا PY_MAJOR_نسخه < 3
    إذا (جمع_کړه_string(ret, "__members__"))
    {
	Py_DECREF(ret);
	أرجع عدم;
    }
#د_که_پای

    أرجع ret;
}

// Output buffer management

// Function to write a line, points to either msg() or emsg().
تعريفنوع صحيح (*writefn)(حرف *);

DEFINE_PY_TYPE_OBJECT(OutputType);

تعريفنوع بنية
{
    Py_Object_سر
    اوږد softspace;
    اوږد error;
} OutputObject;

ساكن حرف *OutputAttrs[] = {
    "softspace",
    عدم
};

    ساكن PyObject *
OutputDir(PyObject *self, PyObject *دلیلونه ناکارول_شوی)
{
    أرجع ObjectDir(self, OutputAttrs);
}

    ساكن صحيح
OutputSetattr(PyObject *self_obj, حرف *name, PyObject *valObject)
{
    OutputObject *self = (OutputObject*)self_obj;
    إذا (valObject == عدم)
    {
	Py_Err_سېټ_STRING(PyExc_AttributeError,
		N_("can't delete OutputObject attributes"));
	أرجع -1;
    }

    إذا (strcmp(name, "softspace") == 0)
    {
	إذا (شمېره_منزل_Long(valObject, &(self->softspace), شمېره_UNSIGNED))
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
	((حرف *)io_ga.ga_معلومات)[io_ga.ga_len] = NUL;
	// We don't know what emsg_severe should be here, so ... hope?
	old_fn((حرف *)io_ga.ga_معلومات);
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

	mch_memmove(((حرف *)io_ga.ga_معلومات) + io_ga.ga_len, str, (size_t)len);
	((حرف *)io_ga.ga_معلومات)[io_ga.ga_len + len] = NUL;
	emsg_severe = severe;
	fn((حرف *)io_ga.ga_معلومات);
	str = ptr + 1;
	n -= len + 1;
	io_ga.ga_len = 0;
    }

    // Put the remaining text into io_ga for later printing.
    إذا (n > 0 && (fn == (writefn)emsg || !got_int)
					&& ga_grow(&io_ga, (صحيح)(n + 1)) == OK)
    {
	mch_memmove(((حرف *)io_ga.ga_معلومات) + io_ga.ga_len, str, (size_t)n);
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

    Py_BEGIN_ALLOW_تارونه
    Python_قلف_Vim();
    writer((writefn)(error ? emsg : پیغام), (char_u *)str, len, error);
    Python_Release_Vim();
    Py_پای_ټکی_ALLOW_تارونه
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
    PyObject	*توکی;

    إذا (!(iterator = PyObject_GetIter(seq)))
	أرجع عدم;

    طالما ((توکی = Py_Iter_بل(iterator)))
    {
	إذا (write_output(self, توکی))
	{
	    Py_DECREF(iterator);
	    Py_DECREF(توکی);
	    أرجع عدم;
	}
	Py_DECREF(توکی);
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    إذا (PyErr_Occurred())
	أرجع عدم;

    Py_INCREF(Py_None);
    أرجع Py_None;
}

    ساكن PyObject *
AlwaysNone(PyObject *self ناکارول_شوی, PyObject *دلیلونه ناکارول_شوی)
{
    // do nothing
    Py_INCREF(Py_None);
    أرجع Py_None;
}
#تعریف_کړه ALWAYS_NONE AlwaysNone(عدم, عدم)

    ساكن PyObject *
AlwaysFalse(PyObject *self ناکارول_شوی, PyObject *دلیلونه ناکارول_شوی)
{
    // do nothing
    PyObject	*ret = Py_False;
    Py_INCREF(ret);
    أرجع ret;
}
#تعریف_کړه ALWAYS_FALSE AlwaysFalse(عدم, عدم)

    ساكن PyObject *
AlwaysTrue(PyObject *self ناکارول_شوی, PyObject *دلیلونه ناکارول_شوی)
{
    // do nothing
    PyObject	*ret = Py_True;
    Py_INCREF(ret);
    أرجع ret;
}
#تعریف_کړه ALWAYS_TRUE AlwaysTrue(عدم, عدم)

/***************/

ساكن بنية Py_لاره_Def OutputMethods[] = {
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
    إذا (Py_Sys_سېټ_Object("stdout", (PyObject *)(فراغ *)&Output))
	أرجع -1;
    إذا (Py_Sys_سېټ_Object("stderr", (PyObject *)(فراغ *)&Error))
	أرجع -1;

    إذا (PyErr_Occurred())
    {
	emsg(_(e_python_error_initialising_io_object));
	أرجع -1;
    }

    أرجع 0;
}

#إذا PY_نسخه_HEX < 0x030700f0
ساكن PyObject *call_بارول_module(حرف *name, صحيح len, PyObject *پیدا_کړه_module_پایله);

تعريفنوع بنية
{
    Py_Object_سر
    حرف	*fullname;
    PyObject	*پایله;
} LoaderObject;
DEFINE_PY_TYPE_OBJECT(LoaderType);

    ساكن فراغ
LoaderDestructor(PyObject *self_obj)
{
    LoaderObject *self = (LoaderObject*)self_obj;
    vim_free(self->fullname);
    Py_XDECREF(self->پایله);
    DESTRUCTOR_پای(self);
}

    ساكن PyObject *
Loader_بارول_Module(LoaderObject *self, PyObject *دلیلونه ناکارول_شوی)
{
    حرف	*fullname = self->fullname;
    PyObject	*پایله = self->پایله;
    PyObject	*module;

    إذا (!fullname)
    {
	module = پایله ? پایله : Py_None;
	Py_INCREF(module);
	أرجع module;
    }

    module = call_بارول_module(fullname, (صحيح)STRLEN(fullname), پایله);

    self->fullname = عدم;
    self->پایله = module;

    vim_free(fullname);
    Py_DECREF(پایله);

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

ساكن بنية Py_لاره_Def LoaderMethods[] = {
    // name,	    function,				calling,	doc
    {"load_module", (PyCFunction)Loader_بارول_Module,	METH_VARARGS,	""},
    { عدم,	    عدم,				0,		عدم}
};
#د_که_پای

/*
 * Check to see whether a Vim error has been reported, or a keyboard
 * interrupt has been detected.
 */
    ساكن فراغ
Vim_Try_پیل(فراغ)
{
    ++trylevel;
}

    ساكن صحيح
Vim_Try_پای_ټکی(فراغ)
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
	Py_Err_سېټ_None(Py_Exc_Keyboard_مداخله);
	أرجع -1;
    }
    وإلا إذا (پیغام_list != عدم && *پیغام_list != عدم)
    {
	صحيح	should_free;
	حرف	*پیغام;

	پیغام = get_exception_string(*پیغام_list, ET_ERROR, عدم, &should_free);

	إذا (پیغام == عدم)
	{
	    Py_Err_No_حافظه();
	    أرجع -1;
	}

	Py_Err_سېټ_Vim(پیغام);

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
	Py_Err_سېټ_Vim((حرف *)current_exception->ارزښت);
	discard_current_exception();
	أرجع -1;
    }
}

    ساكن صحيح
Vim_Check_مداخله(فراغ)
{
    إذا (got_int)
    {
	Py_Err_سېټ_None(Py_Exc_Keyboard_مداخله);
	أرجع 1;
    }
    أرجع 0;
}

// Vim module - Implementation

    ساكن PyObject *
VimCommand(PyObject *self ناکارول_شوی, PyObject *string)
{
    char_u	*cmd;
    PyObject	*ret;
    PyObject	*todecref;

    إذا (!(cmd = String_منزل_Chars(string, &todecref)))
	أرجع عدم;

    Py_BEGIN_ALLOW_تارونه
    Python_قلف_Vim();

    Vim_Try_پیل();
    do_cmdline_cmd(cmd);
    update_screen(UPD_VALID);

    Python_Release_Vim();
    Py_پای_ټکی_ALLOW_تارونه

    إذا (Vim_Try_پای_ټکی())
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
Vim_منزل_Python(typval_T *our_tv, صحيح ژوروالی, PyObject *lookup_dict)
{
    PyObject	*ret;
    PyObject	*newObj;
    حرف	ptrBuf[حجم(فراغ *) * 2 + 3];

    // Avoid infinite recursion
    إذا (ژوروالی > 100)
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

	إذا ((ret = Py_Dict_Get_توکی_String(lookup_dict, ptrBuf)))
	{
	    Py_INCREF(ret);
	    أرجع ret;
	}
    }

    إذا (our_tv->v_type == VAR_STRING)
	ret = PyString_FromString(our_tv->vval.v_string == عدم
					? "" : (حرف *)our_tv->vval.v_string);
    وإلا إذا (our_tv->v_type == VAR_شمېره)
    {
	حرف buf[NUMBUFLEN];

	// For backwards compatibility numbers are stored as strings.
	sprintf(buf, "%lld", (اوږد اوږد)our_tv->vval.v_شمېره);
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

	إذا (Py_Dict_سېټ_توکی_String(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    أرجع عدم;
	}

	CHECK_LIST_MATERIALIZE(list);
	FOR_ALL_LIST_توکي(list, curr)
	{
	    إذا (!(newObj = Vim_منزل_Python(&curr->li_tv, ژوروالی + 1, lookup_dict)))
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
	    typval_T	*توکی_tv = TUPLE_توکی(tuple, idx);

	    newObj = Vim_منزل_Python(توکی_tv, ژوروالی + 1, lookup_dict);
	    إذا (!newObj)
	    {
		Py_DECREF(ret);
		أرجع عدم;
	    }
	    Py_Tuple_سېټ_توکی(ret, idx, newObj);
	}

	إذا (Py_Dict_سېټ_توکی_String(lookup_dict, ptrBuf, ret))
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

	إذا (Py_Dict_سېټ_توکی_String(lookup_dict, ptrBuf, ret))
	{
	    Py_DECREF(ret);
	    أرجع عدم;
	}

	todo = ht->ht_کارول_شوی;
	لكل (hi = ht->ht_array; todo > 0; ++hi)
	{
	    إذا (!HASHITEM_تشه(hi))
	    {
		--todo;

		di = dict_lookup(hi);
		إذا (!(newObj = Vim_منزل_Python(&di->di_tv, ژوروالی + 1, lookup_dict)))
		{
		    Py_DECREF(ret);
		    أرجع عدم;
		}
		إذا (Py_Dict_سېټ_توکی_String(ret, (حرف *)hi->hi_کلی, newObj))
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
	إذا (our_tv->vval.v_شمېره == VVAL_FALSE)
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
	ret = Py_بایټونه_From_String_And_اندازه(
		(حرف*) our_tv->vval.v_blob->bv_ga.ga_معلومات,
		(Py_ssize_t) our_tv->vval.v_blob->bv_ga.ga_len);
    وإلا
    {
	Py_INCREF(Py_None);
	ret = Py_None;
    }

    أرجع ret;
}

    ساكن PyObject *
VimEval(PyObject *self ناکارول_شوی, PyObject *دلیلونه)
{
    char_u	*بیان;
    typval_T	*our_tv;
    PyObject	*string;
    PyObject	*todecref;
    PyObject	*ret;
    PyObject	*lookup_dict;

    إذا (!PyArg_ParseTuple(دلیلونه, "O", &string))
	أرجع عدم;

    إذا (!(بیان = String_منزل_Chars(string, &todecref)))
	أرجع عدم;

    Py_BEGIN_ALLOW_تارونه
    Python_قلف_Vim();
    Vim_Try_پیل();
    our_tv = eval_بیان(بیان, عدم);
    Python_Release_Vim();
    Py_پای_ټکی_ALLOW_تارونه

    Py_XDECREF(todecref);

    إذا (Vim_Try_پای_ټکی())
	أرجع عدم;

    إذا (our_tv == عدم)
    {
	Py_Err_سېټ_VIM(N_("invalid expression"));
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


    Py_BEGIN_ALLOW_تارونه
    Python_قلف_Vim();
    free_tv(our_tv);
    Python_Release_Vim();
    Py_پای_ټکی_ALLOW_تارونه

    أرجع ret;
}

ساكن PyObject *Convert_منزل_Py_Object(typval_T *);

    ساكن PyObject *
VimEvalPy(PyObject *self ناکارول_شوی, PyObject *string)
{
    typval_T	*our_tv;
    PyObject	*ret;
    char_u	*بیان;
    PyObject	*todecref;

    إذا (!(بیان = String_منزل_Chars(string, &todecref)))
	أرجع عدم;

    Py_BEGIN_ALLOW_تارونه
    Python_قلف_Vim();
    Vim_Try_پیل();
    our_tv = eval_بیان(بیان, عدم);
    Python_Release_Vim();
    Py_پای_ټکی_ALLOW_تارونه

    Py_XDECREF(todecref);

    إذا (Vim_Try_پای_ټکی())
	أرجع عدم;

    إذا (our_tv == عدم)
    {
	Py_Err_سېټ_VIM(N_("invalid expression"));
	أرجع عدم;
    }

    ret = Convert_منزل_Py_Object(our_tv);
    Py_BEGIN_ALLOW_تارونه
    Python_قلف_Vim();
    free_tv(our_tv);
    Python_Release_Vim();
    Py_پای_ټکی_ALLOW_تارونه

    أرجع ret;
}

    ساكن PyObject *
VimStrwidth(PyObject *self ناکارول_شوی, PyObject *string)
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
_VimChdir(PyObject *_chdir, PyObject *دلیلونه, PyObject *kwargs)
{
    PyObject	*ret;
    PyObject	*newwd;
    PyObject	*todecref;
    char_u	*new_dir;

    إذا (_chdir == عدم)
	أرجع عدم;
    إذا (!(ret = PyObject_Call(_chdir, دلیلونه, kwargs)))
	أرجع عدم;

    إذا (!(newwd = Py_Object_Call_Function_Obj_دلیلونه(py_getcwd, عدم)))
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

    Vim_Try_پیل();

    إذا (vim_chdir(new_dir))
    {
	Py_DECREF(ret);
	Py_DECREF(newwd);
	Py_XDECREF(todecref);

	إذا (Vim_Try_پای_ټکی())
	    أرجع عدم;

	Py_Err_سېټ_VIM(N_("failed to change directory"));
	أرجع عدم;
    }

    Py_DECREF(newwd);
    Py_XDECREF(todecref);

    post_chdir(CDSCOPE_GLOBAL);

    إذا (Vim_Try_پای_ټکی())
    {
	Py_DECREF(ret);
	أرجع عدم;
    }

    أرجع ret;
}

    ساكن PyObject *
VimChdir(PyObject *self ناکارول_شوی, PyObject *دلیلونه, PyObject *kwargs)
{
    أرجع _VimChdir(py_chdir, دلیلونه, kwargs);
}

    ساكن PyObject *
VimFchdir(PyObject *self ناکارول_شوی, PyObject *دلیلونه, PyObject *kwargs)
{
    أرجع _VimChdir(py_fchdir, دلیلونه, kwargs);
}

تعريفنوع بنية {
    PyObject	*callable;
    PyObject	*پایله;
} map_rtp_معلومات;

    ساكن فراغ
map_rtp_callback(char_u *لار, فراغ *معلومات_3)
{
    فراغ	**معلومات = (فراغ **) معلومات_3;
    PyObject	*لار_Object;
    map_rtp_معلومات	*mr_معلومات = *((map_rtp_معلومات **) معلومات);

    إذا (!(لار_Object = PyString_FromString((حرف *)لار)))
    {
	*معلومات = عدم;
	أرجع;
    }

    mr_معلومات->پایله = Py_Object_Call_Function_Obj_دلیلونه(mr_معلومات->callable,
						   لار_Object, عدم);

    Py_DECREF(لار_Object);

    إذا (!mr_معلومات->پایله || mr_معلومات->پایله != Py_None)
	*معلومات = عدم;
    وإلا
    {
	Py_DECREF(mr_معلومات->پایله);
	mr_معلومات->پایله = عدم;
    }
}

    ساكن PyObject *
VimForeachRTP(PyObject *self ناکارول_شوی, PyObject *callable)
{
    map_rtp_معلومات	معلومات;

    معلومات.callable = callable;
    معلومات.پایله = عدم;

    do_دننه_runtimepath(عدم, 0, &map_rtp_callback, &معلومات);

    إذا (معلومات.پایله == عدم)
    {
	إذا (PyErr_Occurred())
	    أرجع عدم;
	وإلا
	{
	    Py_INCREF(Py_None);
	    أرجع Py_None;
	}
    }
    أرجع معلومات.پایله;
}

/*
 * _vim_runtimepath_ special path implementation.
 */

    ساكن فراغ
map_finder_callback(char_u *لار, فراغ *معلومات_3)
{
    فراغ	**معلومات = (فراغ **) معلومات_3;
    PyObject	*list = *((PyObject **) معلومات);
    PyObject	*لار_Object1, *لار_Object2;
    حرف	*pathbuf;
    size_t	pathlen;

    pathlen = STRLEN(لار);

#إذا PY_MAJOR_نسخه < 3
# define PY_MAIN_DIR_STRING "python2"
#وإلا
# define PY_MAIN_DIR_STRING "python3"
#د_که_پای
#تعریف_کړه PY_ALTERNATE_DIR_STRING "pythonx"

#تعریف_کړه PYTHONX_STRING_اوږدتیا 7 // STRLEN("pythonx")
    إذا (!(pathbuf = PyMem_New(حرف,
		    pathlen + STRLEN(PATHSEPSTR) + PYTHONX_STRING_اوږدتیا + 1)))
    {
	Py_Err_No_حافظه();
	*معلومات = عدم;
	أرجع;
    }

    mch_memmove(pathbuf, لار, pathlen + 1);
    جمع_کړه_pathsep((char_u *) pathbuf);

    pathlen = STRLEN(pathbuf);
    mch_memmove(pathbuf + pathlen, PY_MAIN_DIR_STRING,
	    PYTHONX_STRING_اوږدتیا + 1);

    إذا (!(لار_Object1 = PyString_FromString(pathbuf)))
    {
	*معلومات = عدم;
	PyMem_Free(pathbuf);
	أرجع;
    }

    mch_memmove(pathbuf + pathlen, PY_ALTERNATE_DIR_STRING,
	    PYTHONX_STRING_اوږدتیا + 1);

    إذا (!(لار_Object2 = PyString_FromString(pathbuf)))
    {
	Py_DECREF(لار_Object1);
	PyMem_Free(pathbuf);
	*معلومات = عدم;
	أرجع;
    }

    PyMem_Free(pathbuf);

    إذا (PyList_Append(list, لار_Object1)
	    || PyList_Append(list, لار_Object2))
	*معلومات = عدم;

    Py_DECREF(لار_Object1);
    Py_DECREF(لار_Object2);
}

    ساكن PyObject *
Vim_GetPaths(PyObject *self ناکارول_شوی, PyObject *دلیلونه ناکارول_شوی)
{
    PyObject	*ret;

    إذا (!(ret = PyList_New(0)))
	أرجع عدم;

    do_دننه_runtimepath(عدم, 0, &map_finder_callback, ret);

    إذا (PyErr_Occurred())
    {
	Py_DECREF(ret);
	أرجع عدم;
    }

    أرجع ret;
}

#إذا PY_نسخه_HEX >= 0x030700f0
    ساكن PyObject *
Finder_پیدا_کړه_Spec(PyObject *self, PyObject *دلیلونه)
{
    حرف	*fullname;
    PyObject	*paths;
    PyObject	*موخه = Py_None;
    PyObject	*spec;

    إذا (!PyArg_ParseTuple(دلیلونه, "s|O", &fullname, &موخه))
	أرجع عدم;

    إذا (!(paths = Vim_GetPaths(self, عدم)))
	أرجع عدم;

    spec = PyObject_CallFunction(py_پیدا_کړه_spec, "sOO", fullname, paths, موخه);

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
Finder_پیدا_کړه_Module(PyObject* self ناکارول_شوی, PyObject* دلیلونه ناکارول_شوی)
{
    // Apparently returning None works.
    Py_INCREF(Py_None);
    أرجع Py_None;
}
#وإلا
    ساكن PyObject *
call_بارول_module(حرف *name, صحيح len, PyObject *پیدا_کړه_module_پایله)
{
    PyObject	*fd, *pathname, *description;

    إذا (!PyTuple_Check(پیدا_کړه_module_پایله))
    {
	PyErr_FORMAT_TYPE(
		N_("expected 3-tuple as imp.find_module() result, but got %s"),
		پیدا_کړه_module_پایله);
	أرجع عدم;
    }
    إذا (Py_Tuple_GET_اندازه(پیدا_کړه_module_پایله) != 3)
    {
	PyErr_FORMAT(PyExc_TypeError,
		N_("expected 3-tuple as imp.find_module() result, but got "
		   "tuple of size %d"),
		(صحيح) Py_Tuple_GET_اندازه(پیدا_کړه_module_پایله));
	أرجع عدم;
    }

    إذا (!(fd = Py_Tuple_GET_توکی(پیدا_کړه_module_پایله, 0))
	    || !(pathname = Py_Tuple_GET_توکی(پیدا_کړه_module_پایله, 1))
	    || !(description = Py_Tuple_GET_توکی(پیدا_کړه_module_پایله, 2)))
    {
	Py_Err_سېټ_STRING(PyExc_RuntimeError,
		N_("internal error: imp.find_module returned tuple with NULL"));
	أرجع عدم;
    }

    أرجع PyObject_CallFunction(py_بارول_module,
	    "s#OOO", name, len, fd, pathname, description);
}

    ساكن PyObject *
پیدا_کړه_module(حرف *fullname, حرف *لکۍ, PyObject *new_لار)
{
    PyObject	*پیدا_کړه_module_پایله;
    PyObject	*module;
    حرف	*dot;

    إذا ((dot = (حرف *)vim_strchr((char_u *) لکۍ, '.')))
    {
	/*
	 * There is a dot in the name: call find_module recursively without the
	 * first component
	 */
	PyObject	*newest_لار;
	صحيح		partlen = (صحيح) (dot - 1 - لکۍ);

	إذا (!(پیدا_کړه_module_پایله = PyObject_CallFunction(py_پیدا_کړه_module,
			"s#O", لکۍ, partlen, new_لار)))
	{
	    إذا (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_ImportError))
		PyErr_Clear();
	    أرجع عدم;
	}

	إذا (!(module = call_بارول_module(
			fullname,
			((صحيح) (لکۍ - fullname)) + partlen,
			پیدا_کړه_module_پایله)))
	{
	    Py_DECREF(پیدا_کړه_module_پایله);
	    أرجع عدم;
	}

	Py_DECREF(پیدا_کړه_module_پایله);

	إذا (!(newest_لار = PyObject_GetAttrString(module, "__path__")))
	{
	    Py_DECREF(module);
	    أرجع عدم;
	}

	Py_DECREF(module);

	پیدا_کړه_module_پایله = پیدا_کړه_module(fullname, dot + 1, newest_لار);

	Py_DECREF(newest_لار);

	أرجع پیدا_کړه_module_پایله;
    }
    وإلا
    {
	إذا (!(پیدا_کړه_module_پایله = PyObject_CallFunction(py_پیدا_کړه_module,
			"sO", لکۍ, new_لار)))
	{
	    إذا (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_ImportError))
		PyErr_Clear();
	    أرجع عدم;
	}

	أرجع پیدا_کړه_module_پایله;
    }
}

    ساكن PyObject *
Finder_پیدا_کړه_Module(PyObject *self, PyObject *دلیلونه)
{
    حرف	*fullname;
    PyObject	*پایله;
    PyObject	*new_لار;
    LoaderObject	*loader;

    إذا (!PyArg_ParseTuple(دلیلونه, "s", &fullname))
	أرجع عدم;

    إذا (!(new_لار = Vim_GetPaths(self, عدم)))
	أرجع عدم;

    پایله = پیدا_کړه_module(fullname, fullname, new_لار);

    Py_DECREF(new_لار);

    إذا (!پایله)
    {
	إذا (PyErr_Occurred())
	    أرجع عدم;

	Py_INCREF(Py_None);
	أرجع Py_None;
    }

    إذا (!(fullname = (حرف *)vim_strsave((char_u *)fullname)))
    {
	Py_DECREF(پایله);
	Py_Err_No_حافظه();
	أرجع عدم;
    }

    إذا (!(loader = PyObject_NEW(LoaderObject, LoaderTypePtr)))
    {
	vim_free(fullname);
	Py_DECREF(پایله);
	أرجع عدم;
    }

    loader->fullname = fullname;
    loader->پایله = پایله;

    أرجع (PyObject *) loader;
}
#د_که_پای

    ساكن PyObject *
Vim_لار_Hook(PyObject *self ناکارول_شوی, PyObject *دلیلونه)
{
    حرف	*لار;

    إذا (PyArg_ParseTuple(دلیلونه, "s", &لار)
	    && STRCMP(لار, vim_special_لار) == 0)
    {
	Py_INCREF(vim_module);
	أرجع vim_module;
    }

    PyErr_Clear();
    Py_Err_سېټ_None(PyExc_ImportError);
    أرجع عدم;
}

/*
 * Vim module - Definitions
 */

ساكن بنية Py_لاره_Def VimMethods[] = {
    // name,	    function,			calling,			documentation
    {"command",	    VimCommand,			METH_O,				"Execute a Vim ex-mode command" },
    {"eval",	    VimEval,			METH_VARARGS,			"Evaluate an expression using Vim evaluator" },
    {"bindeval",    VimEvalPy,			METH_O,				"Like eval(), but returns objects attached to Vim ones"},
    {"strwidth",    VimStrwidth,		METH_O,				"Screen string width, counts <Tab> as having width 1"},
    {"chdir",	    (PyCFunction)(فراغ *)VimChdir,	METH_VARARGS|METH_KEYWORDS,	"Change directory"},
    {"fchdir",	    (PyCFunction)(فراغ *)VimFchdir,	METH_VARARGS|METH_KEYWORDS,	"Change directory"},
    {"foreach_rtp", VimForeachRTP,		METH_O,				"Call given callable for each path in &rtp"},
#إذا PY_نسخه_HEX >= 0x030700f0
    {"find_spec",   Finder_پیدا_کړه_Spec,		METH_VARARGS,			"Internal use only, returns spec object for any input it receives"},
#د_که_پای
    {"find_module", Finder_پیدا_کړه_Module,		METH_VARARGS,			"Internal use only, returns loader object for any input it receives"},
    {"path_hook",   Vim_لار_Hook,		METH_VARARGS,			"Hook function to install in sys.path_hooks"},
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
    nextfun بل;
    destructorfun destruct;
    traversefun traverse;
    clearfun clear;
    PyObject *iter_object;
} IterObject;

    ساكن PyObject *
IterNew(فراغ *پیل, destructorfun destruct, nextfun بل, traversefun traverse,
	clearfun clear, PyObject *iter_object)
{
    IterObject *self;

    self = PyObject_GC_New(IterObject, IterTypePtr);
    self->cur = پیل;
    self->بل = بل;
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
IterTraverse(PyObject *self_obj, visitproc لیدنه, فراغ *arg)
{
    IterObject *self = (IterObject*)self_obj;
    إذا (self->traverse != عدم)
	أرجع self->traverse(self->cur, لیدنه, arg);
    وإلا
	أرجع 0;
}

// Mac OSX defines clear() somewhere.
#که_تعریف_شوی clear
# undef clear
#د_که_پای

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
Iter_بل(PyObject *self_obj)
{
    IterObject *self = (IterObject*)self_obj;
    أرجع self->بل(&self->cur);
}

    ساكن PyObject *
IterIter(PyObject *self)
{
    Py_INCREF(self);
    أرجع self;
}

تعريفنوع بنية pylinkedlist_S {
    بنية pylinkedlist_S	*pll_بل;
    بنية pylinkedlist_S	*pll_prev;
    PyObject			*pll_obj;
} pylinkedlist_T;

ساكن pylinkedlist_T *lastdict = عدم;
ساكن pylinkedlist_T *lastlist = عدم;
ساكن pylinkedlist_T *lasttuple = عدم;
ساكن pylinkedlist_T *lastfunc = عدم;

    ساكن فراغ
pyll_remove(pylinkedlist_T *ref, pylinkedlist_T **وروستی)
{
    إذا (ref->pll_prev == عدم)
    {
	إذا (ref->pll_بل == عدم)
	{
	    *وروستی = عدم;
	    أرجع;
	}
    }
    وإلا
	ref->pll_prev->pll_بل = ref->pll_بل;

    إذا (ref->pll_بل == عدم)
	*وروستی = ref->pll_prev;
    وإلا
	ref->pll_بل->pll_prev = ref->pll_prev;
}

    ساكن فراغ
pyll_جمع_کړه(PyObject *self, pylinkedlist_T *ref, pylinkedlist_T **وروستی)
{
    إذا (*وروستی == عدم)
	ref->pll_prev = عدم;
    وإلا
    {
	(*وروستی)->pll_بل = ref;
	ref->pll_prev = *وروستی;
    }
    ref->pll_بل = عدم;
    ref->pll_obj = self;
    *وروستی = ref;
}

DEFINE_PY_TYPE_OBJECT(DictionaryType);

تعريفنوع بنية
{
    Py_Object_سر
    dict_T	*dict;
    pylinkedlist_T	ref;
} DictionaryObject;

ساكن PyObject *DictionaryUpdate(DictionaryObject *, PyObject *, PyObject *);

#تعریف_کړه NEW_DICTIONARY(dict) DictionaryNew(DictionaryTypePtr, dict)

    ساكن PyObject *
DictionaryNew(PyTypeObject *subtype, dict_T *dict)
{
    DictionaryObject	*self;

    self = (DictionaryObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    إذا (self == عدم)
	أرجع عدم;
    self->dict = dict;
    ++dict->dv_refcount;

    pyll_جمع_کړه((PyObject *)(self), &self->ref, &lastdict);

    أرجع (PyObject *)(self);
}

    ساكن dict_T *
py_dict_alloc(فراغ)
{
    dict_T	*ret;

    إذا (!(ret = dict_alloc()))
    {
	Py_Err_No_حافظه();
	أرجع عدم;
    }
    ++ret->dv_refcount;

    أرجع ret;
}

    ساكن PyObject *
DictionaryConstructor(PyTypeObject *subtype, PyObject *دلیلونه, PyObject *kwargs)
{
    DictionaryObject	*self;
    dict_T	*dict;

    إذا (!(dict = py_dict_alloc()))
	أرجع عدم;

    self = (DictionaryObject *) DictionaryNew(subtype, dict);

    --dict->dv_refcount;

    إذا (kwargs || Py_Tuple_اندازه(دلیلونه))
    {
	PyObject	*tmp;
	إذا (!(tmp = DictionaryUpdate(self, دلیلونه, kwargs)))
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

    DESTRUCTOR_پای(self);
}

ساكن حرف *DictionaryAttrs[] = {
    "locked", "scope",
    عدم
};

    ساكن PyObject *
DictionaryDir(PyObject *self, PyObject *دلیلونه ناکارول_شوی)
{
    أرجع ObjectDir(self, DictionaryAttrs);
}

    ساكن صحيح
DictionarySetattr(PyObject *self_obj, حرف *name, PyObject *valObject)
{
    DictionaryObject *self = (DictionaryObject*)self_obj;
    إذا (valObject == عدم)
    {
	Py_Err_سېټ_STRING(PyExc_AttributeError,
		N_("cannot delete vim.Dictionary attributes"));
	أرجع -1;
    }

    إذا (strcmp(name, "locked") == 0)
    {
	إذا (self->dict->dv_قلف == VAR_FIXED)
	{
	    Py_Err_سېټ_STRING(PyExc_TypeError,
		    N_("cannot modify fixed dictionary"));
	    أرجع -1;
	}
	وإلا
	{
	    صحيح		istrue = PyObject_IsTrue(valObject);
	    إذا (istrue == -1)
		أرجع -1;
	    وإلا إذا (istrue)
		self->dict->dv_قلف = VAR_LOCKED;
	    وإلا
		self->dict->dv_قلف = 0;
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
Dictionary_اوږدتیا(DictionaryObject *self)
{
    أرجع ((PyInt) (self->dict->dv_hashtab.ht_کارول_شوی));
}

#تعریف_کړه DICT_FLAG_HAS_DEFAULT	0x01
#تعریف_کړه DICT_FLAG_راوباسه		0x02
#تعریف_کړه DICT_FLAG_NONE_DEFAULT	0x04
#تعریف_کړه DICT_FLAG_RETURN_BOOL	0x08 // Incompatible with DICT_FLAG_POP
#تعریف_کړه DICT_FLAG_RETURN_PAIR	0x10

    ساكن PyObject *
Dictionary_توکی(DictionaryObject *self, PyObject *دلیلونه, صحيح flags)
{
    PyObject	*کلی_Object;
    PyObject	*defObject = ((flags & DICT_FLAG_NONE_DEFAULT)? Py_None : عدم);
    PyObject	*ret;
    char_u	*کلی;
    dictitem_T	*di;
    dict_T	*dict = self->dict;
    hashitem_T	*hi;
    PyObject	*todecref;

    إذا (flags & DICT_FLAG_HAS_DEFAULT)
    {
	إذا (!PyArg_ParseTuple(دلیلونه, "O|O", &کلی_Object, &defObject))
	    أرجع عدم;
    }
    وإلا
	کلی_Object = دلیلونه;

    إذا (flags & DICT_FLAG_RETURN_BOOL)
	defObject = Py_False;

    إذا (!(کلی = String_منزل_Chars(کلی_Object, &todecref)))
	أرجع عدم;

    إذا (*کلی == NUL)
    {
	RAISE_NO_تشه_کلیګانې;
	Py_XDECREF(todecref);
	أرجع عدم;
    }

    hi = hash_پیدا_کړه(&dict->dv_hashtab, کلی);

    Py_XDECREF(todecref);

    إذا (HASHITEM_تشه(hi))
    {
	إذا (defObject)
	{
	    Py_INCREF(defObject);
	    أرجع defObject;
	}
	وإلا
	{
	    Py_Err_سېټ_Object(Py_Exc_کلی_Error, کلی_Object);
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

    إذا (flags & DICT_FLAG_راوباسه)
    {
	إذا (dict->dv_قلف)
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
Dictionary_توکی_2(DictionaryObject *self, PyObject *کلی_Object)
{
    أرجع Dictionary_توکی(self, کلی_Object, 0);
}

    ساكن صحيح
Dictionary_لري(DictionaryObject *self, PyObject *کلی_Object)
{
    PyObject	*rObj = Dictionary_توکی(self, کلی_Object, DICT_FLAG_RETURN_BOOL);
    صحيح		ret;

    إذا (rObj == عدم)
	أرجع -1;

    ret = (rObj == Py_True);

    Py_DECREF(rObj);

    أرجع ret;
}

تعريفنوع بنية
{
    صحيح		dii_بدل_شوی;
    hashtab_T	*dii_ht;
    hashitem_T	*dii_hi;
    long_u	dii_todo;
} dictiterinfo_T;

    ساكن PyObject *
Dictionary_Iter_بل(فراغ **arg)
{
    PyObject	*ret;
    dictiterinfo_T **dii = (dictiterinfo_T**)arg;

    إذا (!(*dii)->dii_todo)
	أرجع عدم;

    إذا ((*dii)->dii_ht->ht_بدل_شوی != (*dii)->dii_بدل_شوی)
    {
	Py_Err_سېټ_STRING(PyExc_RuntimeError,
		N_("hashtab changed during iteration"));
	أرجع عدم;
    }

    طالما (((*dii)->dii_todo) && HASHITEM_تشه((*dii)->dii_hi))
	++((*dii)->dii_hi);

    --((*dii)->dii_todo);

    إذا (!(ret = Py_بایټونه_From_String((حرف *)(*dii)->dii_hi->hi_کلی)))
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
	Py_Err_No_حافظه();
	أرجع عدم;
    }

    ht = &self->dict->dv_hashtab;
    dii->dii_بدل_شوی = ht->ht_بدل_شوی;
    dii->dii_ht = ht;
    dii->dii_hi = ht->ht_array;
    dii->dii_todo = ht->ht_کارول_شوی;

    أرجع IterNew(dii,
	    PyMem_Free, Dictionary_Iter_بل,
	    عدم, عدم, (PyObject *)self);
}

    ساكن صحيح
Dictionary_Ass_توکی(
	DictionaryObject *self, PyObject *کلی_Object, PyObject *valObject)
{
    char_u	*کلی;
    typval_T	tv;
    dict_T	*dict = self->dict;
    dictitem_T	*di;
    PyObject	*todecref;

    إذا (dict->dv_قلف)
    {
	RAISE_LOCKED_DICTIONARY;
	أرجع -1;
    }

    إذا (!(کلی = String_منزل_Chars(کلی_Object, &todecref)))
	أرجع -1;

    إذا (*کلی == NUL)
    {
	RAISE_NO_تشه_کلیګانې;
	Py_XDECREF(todecref);
	أرجع -1;
    }

    di = dict_پیدا_کړه(dict, کلی, -1);

    إذا (valObject == عدم)
    {
	hashitem_T	*hi;

	إذا (di == عدم)
	{
	    Py_XDECREF(todecref);
	    Py_Err_سېټ_Object(Py_Exc_کلی_Error, کلی_Object);
	    أرجع -1;
	}
	hi = hash_پیدا_کړه(&dict->dv_hashtab, di->di_کلی);
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
	إذا (!(di = dictitem_alloc(کلی)))
	{
	    Py_XDECREF(todecref);
	    Py_Err_No_حافظه();
	    أرجع -1;
	}
	di->di_tv.v_type = VAR_UNKNOWN;

	إذا (dict_جمع_کړه(dict, di) == FAIL)
	{
	    dictitem_free(di);
	    RAISE_کلی_جمع_کړه_FAIL(کلی);
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
    long_u	todo = dict->dv_hashtab.ht_کارول_شوی;
    Py_ssize_t	i = 0;
    PyObject	*ret;
    hashitem_T	*hi;
    PyObject	*newObj;

    ret = PyList_New(todo);
    لكل (hi = dict->dv_hashtab.ht_array; todo > 0; ++hi)
    {
	إذا (!HASHITEM_تشه(hi))
	{
	    إذا (!(newObj = hiconvert(hi)))
	    {
		Py_DECREF(ret);
		أرجع عدم;
	    }
	    Py_List_سېټ_توکی(ret, i, newObj);
	    --todo;
	    ++i;
	}
    }
    أرجع ret;
}

    ساكن PyObject *
dict_کلی(hashitem_T *hi)
{
    أرجع Py_بایټونه_From_String((حرف *)(hi->hi_کلی));
}

    ساكن PyObject *
Dictionary_List_کلیګانې(DictionaryObject *self, PyObject *دلیلونه ناکارول_شوی)
{
    أرجع DictionaryListObjects(self, dict_کلی);
}

    ساكن PyObject *
dict_val(hashitem_T *hi)
{
    dictitem_T	*di;

    di = dict_lookup(hi);
    أرجع Convert_منزل_Py_Object(&di->di_tv);
}

    ساكن PyObject *
Dictionary_List_ارزښتونه(DictionaryObject *self, PyObject *دلیلونه ناکارول_شوی)
{
    أرجع DictionaryListObjects(self, dict_val);
}

    ساكن PyObject *
dict_توکی(hashitem_T *hi)
{
    PyObject	*کلی_Object;
    PyObject	*valObject;
    PyObject	*ret;

    إذا (!(کلی_Object = dict_کلی(hi)))
	أرجع عدم;

    إذا (!(valObject = dict_val(hi)))
    {
	Py_DECREF(کلی_Object);
	أرجع عدم;
    }

    ret = Py_Build_ارزښت("(OO)", کلی_Object, valObject);

    Py_DECREF(کلی_Object);
    Py_DECREF(valObject);

    أرجع ret;
}

    ساكن PyObject *
Dictionary_List_توکي(DictionaryObject *self, PyObject *دلیلونه ناکارول_شوی)
{
    أرجع DictionaryListObjects(self, dict_توکی);
}

    ساكن PyObject *
DictionaryUpdate(DictionaryObject *self, PyObject *دلیلونه, PyObject *kwargs)
{
    dict_T	*dict = self->dict;

    إذا (dict->dv_قلف)
    {
	RAISE_LOCKED_DICTIONARY;
	أرجع عدم;
    }

    إذا (kwargs)
    {
	typval_T	tv;

	إذا (ConvertFromPyMapping(kwargs, &tv) == -1)
	    أرجع عدم;

	Vim_Try_پیل();
	dict_extend(self->dict, tv.vval.v_dict, (char_u *) "force", عدم);
	clear_tv(&tv);
	إذا (Vim_Try_پای_ټکی())
	    أرجع عدم;
    }
    وإلا
    {
	PyObject	*obj = عدم;

	إذا (!PyArg_ParseTuple(دلیلونه, "|O", &obj))
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
	    PyObject	*توکی;

	    إذا (!(iterator = PyObject_GetIter(obj)))
		أرجع عدم;

	    طالما ((توکی = Py_Iter_بل(iterator)))
	    {
		PyObject	*fast;
		PyObject	*کلی_Object;
		PyObject	*valObject;
		PyObject	*todecref;
		char_u		*کلی;
		dictitem_T	*di;
		hashitem_T	*hi;

		إذا (!(fast = PySequence_Fast(توکی, "")))
		{
		    Py_DECREF(iterator);
		    Py_DECREF(توکی);
		    أرجع عدم;
		}

		Py_DECREF(توکی);

		إذا (Py_Sequence_اندازه(fast) != 2)
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    PyErr_FORMAT(Py_Exc_ارزښت_Error,
			    N_("expected sequence element of size 2, "
			    "but got sequence of size %d"),
			    (صحيح) Py_Sequence_اندازه(fast));
		    أرجع عدم;
		}

		کلی_Object = Py_Sequence_Get_توکی(fast, 0);

		إذا (!(کلی = String_منزل_Chars(کلی_Object, &todecref)))
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    أرجع عدم;
		}

		di = dictitem_alloc(کلی);

		Py_XDECREF(todecref);

		إذا (di == عدم)
		{
		    Py_DECREF(fast);
		    Py_DECREF(iterator);
		    Py_Err_No_حافظه();
		    أرجع عدم;
		}
		di->di_tv.v_type = VAR_UNKNOWN;

		valObject = Py_Sequence_Get_توکی(fast, 1);

		إذا (ConvertFromPyObject(valObject, &di->di_tv) == -1)
		{
		    Py_DECREF(iterator);
		    Py_DECREF(fast);
		    dictitem_free(di);
		    أرجع عدم;
		}

		Py_DECREF(fast);

		hi = hash_پیدا_کړه(&dict->dv_hashtab, di->di_کلی);
		إذا (!HASHITEM_تشه(hi) || dict_جمع_کړه(dict, di) == FAIL)
		{
		    RAISE_کلی_جمع_کړه_FAIL(di->di_کلی);
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
DictionaryGet(DictionaryObject *self, PyObject *دلیلونه)
{
    أرجع Dictionary_توکی(self, دلیلونه,
			    DICT_FLAG_HAS_DEFAULT|DICT_FLAG_NONE_DEFAULT);
}

    ساكن PyObject *
Dictionary_راوباسه(DictionaryObject *self, PyObject *دلیلونه)
{
    أرجع Dictionary_توکی(self, دلیلونه, DICT_FLAG_HAS_DEFAULT|DICT_FLAG_راوباسه);
}

    ساكن PyObject *
Dictionary_راوباسه_توکی(DictionaryObject *self, PyObject *دلیلونه ناکارول_شوی)
{
    hashitem_T	*hi;
    PyObject	*ret;
    PyObject	*valObject;
    dictitem_T	*di;

    إذا (self->dict->dv_hashtab.ht_کارول_شوی == 0)
    {
	Py_Err_سېټ_None(Py_Exc_کلی_Error);
	أرجع عدم;
    }

    hi = self->dict->dv_hashtab.ht_array;
    طالما (HASHITEM_تشه(hi))
	++hi;

    di = dict_lookup(hi);

    إذا (!(valObject = Convert_منزل_Py_Object(&di->di_tv)))
	أرجع عدم;

    إذا (!(ret = Py_Build_ارزښت("(" Py_بایټونه_fmt "O)", hi->hi_کلی, valObject)))
    {
	Py_DECREF(valObject);
	أرجع عدم;
    }

    hash_remove(&self->dict->dv_hashtab, hi, "Python pop item");
    dictitem_free(di);

    أرجع ret;
}

    ساكن PyObject *
Dictionary_Has_کلی(DictionaryObject *self, PyObject *کلی_Object)
{
    أرجع Dictionary_توکی(self, کلی_Object, DICT_FLAG_RETURN_BOOL);
}

ساكن PySequenceMethods DictionaryAsSeq = {
    0,					// sq_length
    0,					// sq_concat
    0,					// sq_repeat
    0,					// sq_item
    0,					// sq_slice
    0,					// sq_ass_item
    0,					// sq_ass_slice
    (objobjproc) Dictionary_لري,	// sq_contains
    0,					// sq_inplace_concat
    0,					// sq_inplace_repeat
};

ساكن PyMappingMethods DictionaryAsMapping = {
    (lenfunc)       Dictionary_اوږدتیا,
    (binaryfunc)    Dictionary_توکی_2,
    (objobjargproc) Dictionary_Ass_توکی,
};

ساكن بنية Py_لاره_Def DictionaryMethods[] = {
    {"keys",	(PyCFunction)Dictionary_List_کلیګانې,	METH_NOARGS,	""},
    {"values",	(PyCFunction)Dictionary_List_ارزښتونه,	METH_NOARGS,	""},
    {"items",	(PyCFunction)Dictionary_List_توکي,	METH_NOARGS,	""},
    {"update",	(PyCFunction)(فراغ *)DictionaryUpdate,		METH_VARARGS|METH_KEYWORDS, ""},
    {"get",	(PyCFunction)DictionaryGet,		METH_VARARGS,	""},
    {"pop",	(PyCFunction)Dictionary_راوباسه,		METH_VARARGS,	""},
    {"popitem",	(PyCFunction)Dictionary_راوباسه_توکی,		METH_NOARGS,	""},
    {"has_key",	(PyCFunction)Dictionary_Has_کلی,		METH_O,		""},
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

#تعریف_کړه NEW_LIST(list) ListNew(ListTypePtr, list)

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

    pyll_جمع_کړه((PyObject *)(self), &self->ref, &lastlist);

    أرجع (PyObject *)(self);
}

    ساكن list_T *
py_list_alloc(فراغ)
{
    list_T	*ret;

    إذا (!(ret = list_alloc()))
    {
	Py_Err_No_حافظه();
	أرجع عدم;
    }
    ++ret->lv_refcount;

    أرجع ret;
}

    ساكن صحيح
list_py_concat(list_T *l, PyObject *obj, PyObject *lookup_dict)
{
    PyObject	*iterator;
    PyObject	*توکی;
    listitem_T	*li;

    إذا (!(iterator = PyObject_GetIter(obj)))
	أرجع -1;

    طالما ((توکی = Py_Iter_بل(iterator)))
    {
	إذا (!(li = listitem_alloc()))
	{
	    Py_Err_No_حافظه();
	    Py_DECREF(توکی);
	    Py_DECREF(iterator);
	    أرجع -1;
	}
	li->li_tv.v_قلف = 0;
	li->li_tv.v_type = VAR_UNKNOWN;

	إذا (_ConvertFromPyObject(توکی, &li->li_tv, lookup_dict) == -1)
	{
	    Py_DECREF(توکی);
	    Py_DECREF(iterator);
	    listitem_free(l, li);
	    أرجع -1;
	}

	Py_DECREF(توکی);

	list_append(l, li);
    }

    Py_DECREF(iterator);

    // Iterator may have finished due to an exception
    إذا (PyErr_Occurred())
	أرجع -1;

    أرجع 0;
}

    ساكن PyObject *
ListConstructor(PyTypeObject *subtype, PyObject *دلیلونه, PyObject *kwargs)
{
    list_T	*list;
    PyObject	*obj = عدم;

    إذا (kwargs)
    {
	Py_Err_سېټ_STRING(PyExc_TypeError,
		N_("list constructor does not accept keyword arguments"));
	أرجع عدم;
    }

    إذا (!PyArg_ParseTuple(دلیلونه, "|O", &obj))
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

    DESTRUCTOR_پای(self);
}

    ساكن PyInt
List_اوږدتیا(ListObject *self)
{
    أرجع ((PyInt) (self->list->lv_len));
}

    ساكن PyObject *
List_شاخص(ListObject *self, Py_ssize_t شاخص)
{
    listitem_T	*li;

    إذا (شاخص >= List_اوږدتیا(self))
    {
	Py_Err_سېټ_STRING(Py_Exc_شاخص_Error, N_("list index out of range"));
	أرجع عدم;
    }
    li = list_پیدا_کړه(self->list, (اوږد) شاخص);
    إذا (li == عدم)
    {
	// No more suitable format specifications in python-2.3
	PyErr_VIM_FORMAT(N_("internal error: failed to get Vim list item %d"),
		(صحيح) شاخص);
	أرجع عدم;
    }
    أرجع Convert_منزل_Py_Object(&li->li_tv);
}

    ساكن PyObject *
List_ټوټه(ListObject *self, Py_ssize_t لومړی, Py_ssize_t ګام,
	  Py_ssize_t slicelen)
{
    PyInt	i;
    PyObject	*list;

    إذا (ګام == 0)
    {
	Py_Err_سېټ_STRING(Py_Exc_ارزښت_Error, N_("slice step cannot be zero"));
	أرجع عدم;
    }

    list = PyList_New(slicelen);
    إذا (list == عدم)
	أرجع عدم;

    لكل (i = 0; i < slicelen; ++i)
    {
	PyObject	*توکی;

	توکی = List_شاخص(self, لومړی + i*ګام);
	إذا (توکی == عدم)
	{
	    Py_DECREF(list);
	    أرجع عدم;
	}

	Py_List_سېټ_توکی(list, i, توکی);
    }

    أرجع list;
}

    ساكن PyObject *
List_توکی(ListObject *self, PyObject* idx)
{
#إذا PY_MAJOR_نسخه < 3
    إذا (PyInt_Check(idx))
    {
	اوږد _idx = PyInt_AsLong(idx);
	أرجع List_شاخص(self, _idx);
    }
    وإلا
#د_که_پای
    إذا (PyLong_Check(idx))
    {
	اوږد _idx = PyLong_AsLong(idx);
	أرجع List_شاخص(self, _idx);
    }
    وإلا إذا (Py_ټوټه_Check(idx))
    {
	Py_ssize_t پیل, stop, ګام, slicelen;

	إذا (Py_ټوټه_Get_Indices_Ex((Py_ټوټه_Object_T *)idx, List_اوږدتیا(self),
				 &پیل, &stop, &ګام, &slicelen) < 0)
	    أرجع عدم;
	أرجع List_ټوټه(self, پیل, ګام, slicelen);
    }
    وإلا
    {
	RAISE_INVALID_شاخص_TYPE(idx);
	أرجع عدم;
    }
}

    ساكن فراغ
list_restore(Py_ssize_t numadded, Py_ssize_t numreplaced, Py_ssize_t slicelen,
	list_T *l, listitem_T **lis, listitem_T *lastaddedli)
{
    طالما (numreplaced--)
    {
	list_ورننباسه(l, lis[numreplaced], lis[slicelen + numreplaced]);
	listitem_remove(l, lis[slicelen + numreplaced]);
    }
    طالما (numadded--)
    {
	listitem_T	*بل;

	بل = lastaddedli->li_prev;
	listitem_remove(l, lastaddedli);
	lastaddedli = بل;
    }
}

    ساكن صحيح
List_Ass_ټوټه(ListObject *self, Py_ssize_t لومړی,
	     Py_ssize_t ګام, Py_ssize_t slicelen, PyObject *obj)
{
    PyObject	*iterator;
    PyObject	*توکی;
    listitem_T	*li;
    listitem_T	*lastaddedli = عدم;
    listitem_T	*بل;
    typval_T	v;
    list_T	*l = self->list;
    PyInt	i;
    PyInt	j;
    PyInt	numreplaced = 0;
    PyInt	numadded = 0;
    PyInt	اندازه;
    listitem_T	**lis = عدم;

    اندازه = List_اوږدتیا(self);

    إذا (l->lv_قلف)
    {
	RAISE_LOCKED_LIST;
	أرجع -1;
    }

    إذا (ګام == 0)
    {
	Py_Err_سېټ_STRING(Py_Exc_ارزښت_Error, N_("slice step cannot be zero"));
	أرجع -1;
    }

    إذا (ګام != 1 && slicelen == 0)
    {
	// Nothing to do. Only error out if obj has some items.
	صحيح		ret = 0;

	إذا (obj == عدم)
	    أرجع 0;

	إذا (!(iterator = PyObject_GetIter(obj)))
	    أرجع -1;

	إذا ((توکی = Py_Iter_بل(iterator)))
	{
	    PyErr_FORMAT(Py_Exc_ارزښت_Error,
		    N_("attempt to assign sequence of size greater than %d "
			"to extended slice"), 0);
	    Py_DECREF(توکی);
	    ret = -1;
	}
	Py_DECREF(iterator);
	أرجع ret;
    }

    إذا (obj != عدم)
	// XXX May allocate zero bytes.
	إذا (!(lis = PyMem_New(listitem_T *, slicelen * 2)))
	{
	    Py_Err_No_حافظه();
	    أرجع -1;
	}

    إذا (لومړی == اندازه)
	li = عدم;
    وإلا
    {
	li = list_پیدا_کړه(l, (اوږد) لومړی);
	إذا (li == عدم)
	{
	    PyErr_VIM_FORMAT(N_("internal error: no Vim list item %d"),
		    (صحيح)لومړی);
	    إذا (obj != عدم)
		PyMem_Free(lis);
	    أرجع -1;
	}
	i = slicelen;
	طالما (i-- && li != عدم)
	{
	    j = ګام;
	    بل = li;
	    إذا (ګام > 0)
		طالما (بل != عدم && ((بل = بل->li_بل) != عدم) && --j);
	    وإلا
		طالما (بل != عدم && ((بل = بل->li_prev) != عدم) && ++j);

	    إذا (obj == عدم)
		listitem_remove(l, li);
	    وإلا
		lis[slicelen - i - 1] = li;

	    li = بل;
	}
	إذا (li == عدم && i != -1)
	{
	    Py_Err_سېټ_VIM(N_("internal error: not enough list items"));
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
    طالما ((توکی = Py_Iter_بل(iterator)))
    {
	إذا (ConvertFromPyObject(توکی, &v) == -1)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(توکی);
	    PyMem_Free(lis);
	    أرجع -1;
	}
	Py_DECREF(توکی);
	إذا (list_ورننباسه_tv(l, &v, numreplaced < slicelen
				    ? lis[numreplaced]
				    : li) == FAIL)
	{
	    clear_tv(&v);
	    Py_Err_سېټ_VIM(N_("internal error: failed to add item to list"));
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
		lastaddedli = l->lv_u.mat.lv_وروستی;
	    numadded++;
	}
	clear_tv(&v);
	إذا (ګام != 1 && i >= slicelen)
	{
	    Py_DECREF(iterator);
	    PyErr_FORMAT(Py_Exc_ارزښت_Error,
		    N_("attempt to assign sequence of size greater than %d "
			"to extended slice"), (صحيح) slicelen);
	    list_restore(numadded, numreplaced, slicelen, l, lis, lastaddedli);
	    PyMem_Free(lis);
	    أرجع -1;
	}
	++i;
    }
    Py_DECREF(iterator);

    إذا (ګام != 1 && i != slicelen)
    {
	PyErr_FORMAT2(Py_Exc_ارزښت_Error,
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
    إذا (ګام == 1)
	لكل (i = numreplaced; i < slicelen; i++)
	    listitem_remove(l, lis[i]);

    PyMem_Free(lis);

    أرجع 0;
}

    ساكن صحيح
List_Ass_شاخص(ListObject *self, Py_ssize_t شاخص, PyObject *obj)
{
    typval_T	tv;
    list_T	*l = self->list;
    listitem_T	*li;
    Py_ssize_t	اوږدتیا = List_اوږدتیا(self);

    إذا (l->lv_قلف)
    {
	RAISE_LOCKED_LIST;
	أرجع -1;
    }
    إذا (شاخص > اوږدتیا || (شاخص == اوږدتیا && obj == عدم))
    {
	Py_Err_سېټ_STRING(Py_Exc_شاخص_Error, N_("list index out of range"));
	أرجع -1;
    }

    إذا (obj == عدم)
    {
	li = list_پیدا_کړه(l, (اوږد) شاخص);
	إذا (li == عدم)
	{
	    PyErr_VIM_FORMAT(N_("internal error: failed to get Vim "
			"list item %d"), (صحيح) شاخص);
	    أرجع -1;
	}
	vimlist_remove(l, li, li);
	clear_tv(&li->li_tv);
	vim_free(li);
	أرجع 0;
    }

    إذا (ConvertFromPyObject(obj, &tv) == -1)
	أرجع -1;

    إذا (شاخص == اوږدتیا)
    {
	إذا (list_append_tv(l, &tv) == FAIL)
	{
	    clear_tv(&tv);
	    Py_Err_سېټ_VIM(N_("failed to add item to list"));
	    أرجع -1;
	}
    }
    وإلا
    {
	li = list_پیدا_کړه(l, (اوږد) شاخص);
	إذا (li == عدم)
	{
	    PyErr_VIM_FORMAT(N_("internal error: failed to get Vim "
			"list item %d"), (صحيح) شاخص);
	    أرجع -1;
	}
	clear_tv(&li->li_tv);
	copy_tv(&tv, &li->li_tv);
	clear_tv(&tv);
    }
    أرجع 0;
}

    ساكن صحيح
List_Ass_توکی(ListObject *self, PyObject *idx, PyObject *obj)
{
#إذا PY_MAJOR_نسخه < 3
    إذا (PyInt_Check(idx))
    {
	اوږد _idx = PyInt_AsLong(idx);
	أرجع (صحيح)List_Ass_شاخص(self, _idx, obj);
    }
    وإلا
#د_که_پای
    إذا (PyLong_Check(idx))
    {
	اوږد _idx = PyLong_AsLong(idx);
	أرجع (صحيح)List_Ass_شاخص(self, _idx, obj);
    }
    وإلا إذا (Py_ټوټه_Check(idx))
    {
	Py_ssize_t پیل, stop, ګام, slicelen;

	إذا (Py_ټوټه_Get_Indices_Ex((Py_ټوټه_Object_T *)idx, List_اوږدتیا(self),
				 &پیل, &stop, &ګام, &slicelen) < 0)
	    أرجع -1;
	أرجع (صحيح)List_Ass_ټوټه(self, پیل, ګام, slicelen,
		obj);
    }
    وإلا
    {
	RAISE_INVALID_شاخص_TYPE(idx);
	أرجع -1;
    }
}

    ساكن PyObject *
List_Concat_دننه_ځای_پرځای_کړه(ListObject *self, PyObject *obj)
{
    list_T	*l = self->list;
    PyObject	*lookup_dict;

    إذا (l->lv_قلف)
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
List_Iter_بل(فراغ **arg)
{
    PyObject	*ret;
    listiterinfo_T **lii = (listiterinfo_T**)arg;

    إذا (!((*lii)->lw.lw_توکی))
	أرجع عدم;

    إذا (!(ret = Convert_منزل_Py_Object(&((*lii)->lw.lw_توکی->li_tv))))
	أرجع عدم;

    (*lii)->lw.lw_توکی = (*lii)->lw.lw_توکی->li_بل;

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
	Py_Err_No_حافظه();
	أرجع عدم;
    }

    CHECK_LIST_MATERIALIZE(l);
    list_جمع_کړه_watch(l, &lii->lw);
    lii->lw.lw_توکی = l->lv_لومړی;
    lii->list = l;
    ++l->lv_refcount;

    أرجع IterNew(lii,
	    ListIterDestruct, List_Iter_بل,
	    عدم, عدم, (PyObject *)self);
}

ساكن حرف *ListAttrs[] = {
    "locked",
    عدم
};

    ساكن PyObject *
ListDir(PyObject *self, PyObject *دلیلونه ناکارول_شوی)
{
    أرجع ObjectDir(self, ListAttrs);
}

    ساكن صحيح
ListSetattr(PyObject *self_obj, حرف *name, PyObject *valObject)
{
    ListObject *self = (ListObject*)self_obj;
    إذا (valObject == عدم)
    {
	Py_Err_سېټ_STRING(PyExc_AttributeError,
		N_("cannot delete vim.List attributes"));
	أرجع -1;
    }

    إذا (strcmp(name, "locked") == 0)
    {
	إذا (self->list->lv_قلف == VAR_FIXED)
	{
	    Py_Err_سېټ_STRING(PyExc_TypeError, N_("cannot modify fixed list"));
	    أرجع -1;
	}
	وإلا
	{
	    صحيح		istrue = PyObject_IsTrue(valObject);
	    إذا (istrue == -1)
		أرجع -1;
	    وإلا إذا (istrue)
		self->list->lv_قلف = VAR_LOCKED;
	    وإلا
		self->list->lv_قلف = 0;
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
    (lenfunc)		List_اوږدتیا,	 // sq_length,	  len(x)
    (binaryfunc)	0,		 // RangeConcat, sq_concat,  x+y
    0,					 // RangeRepeat, sq_repeat,  x*n
    (PyIntArgFunc)	List_شاخص,	 // sq_item,	  x[i]
    0,					 // was_sq_slice,     x[i:j]
    (PyIntObjArgProc)	List_Ass_شاخص,	 // sq_as_item,  x[i]=v
    0,					 // was_sq_ass_slice, x[i:j]=v
    0,					 // sq_contains
    (binaryfunc)	List_Concat_دننه_ځای_پرځای_کړه,// sq_inplace_concat
    0,					 // sq_inplace_repeat
};

ساكن PyMappingMethods ListAsMapping = {
    /* mp_length	*/ (lenfunc) List_اوږدتیا,
    /* mp_subscript     */ (binaryfunc) List_توکی,
    /* mp_ass_subscript */ (objobjargproc) List_Ass_توکی,
};

ساكن بنية Py_لاره_Def ListMethods[] = {
    {"extend",	(PyCFunction)List_Concat_دننه_ځای_پرځای_کړه,	METH_O,		""},
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

#تعریف_کړه NEW_TUPLE(tuple) TupleNew(TupleTypePtr, tuple)

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

    pyll_جمع_کړه((PyObject *)(self), &self->ref, &lasttuple);

    أرجع (PyObject *)(self);
}

    ساكن tuple_T *
py_tuple_alloc(فراغ)
{
    tuple_T	*ret;

    إذا (!(ret = tuple_alloc()))
    {
	Py_Err_No_حافظه();
	أرجع عدم;
    }
    ++ret->tv_refcount;

    أرجع ret;
}

    ساكن صحيح
tuple_py_concat(tuple_T *t, PyObject *obj, PyObject *lookup_dict)
{
    PyObject	*iterator;
    PyObject	*توکی;

    إذا (!(iterator = PyObject_GetIter(obj)))
	أرجع -1;

    طالما ((توکی = Py_Iter_بل(iterator)))
    {
	typval_T    new_tv;

	إذا (_ConvertFromPyObject(توکی, &new_tv, lookup_dict) == -1)
	{
	    Py_DECREF(توکی);
	    Py_DECREF(iterator);
	    أرجع -1;
	}

	Py_DECREF(توکی);

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
TupleConstructor(PyTypeObject *subtype, PyObject *دلیلونه, PyObject *kwargs)
{
    tuple_T	*tuple;
    PyObject	*obj = عدم;

    إذا (kwargs)
    {
	Py_Err_سېټ_STRING(PyExc_TypeError,
		N_("tuple constructor does not accept keyword arguments"));
	أرجع عدم;
    }

    إذا (!PyArg_ParseTuple(دلیلونه, "|O", &obj))
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

    DESTRUCTOR_پای(self);
}

    ساكن PyInt
Tuple_اوږدتیا(TupleObject *self)
{
    أرجع ((PyInt)(tuple_len(self->tuple)));
}

    ساكن PyObject *
Tuple_شاخص(TupleObject *self, Py_ssize_t شاخص)
{
    PyInt	len = Tuple_اوږدتیا(self);

    إذا (شاخص < 0)
	شاخص = len + شاخص;

    إذا (شاخص < 0 || شاخص >= len)
    {
	Py_Err_سېټ_STRING(Py_Exc_شاخص_Error, N_("tuple index out of range"));
	أرجع عدم;
    }
    أرجع Convert_منزل_Py_Object(TUPLE_توکی(self->tuple, شاخص));
}

/*
 * Return a new tuple object for the tuple slice starting from the index
 * "first" and of length "slicelen" skipping "step" items.
 */
    ساكن PyObject *
Tuple_ټوټه(
    TupleObject		*self,
    Py_ssize_t		لومړی,
    Py_ssize_t		ګام,
    Py_ssize_t		slicelen)
{
    PyInt	i;
    PyObject	*tuple;

    tuple = PyTuple_New(slicelen);
    إذا (tuple == عدم)
	أرجع عدم;

    لكل (i = 0; i < slicelen; ++i)
    {
	PyObject	*توکی;

	توکی = Tuple_شاخص(self, لومړی + i * ګام);
	إذا (توکی == عدم)
	{
	    Py_DECREF(tuple);
	    أرجع عدم;
	}

	Py_Tuple_سېټ_توکی(tuple, i, توکی);
    }

    أرجع tuple;
}

    ساكن PyObject *
Tuple_توکی(TupleObject *self, PyObject* idx)
{
#إذا PY_MAJOR_نسخه < 3
    إذا (PyInt_Check(idx))
    {
	اوږد _idx = PyInt_AsLong(idx);
	أرجع Tuple_شاخص(self, _idx);
    }
    وإلا
#د_که_پای
    إذا (PyLong_Check(idx))
    {
	اوږد _idx = PyLong_AsLong(idx);
	أرجع Tuple_شاخص(self, _idx);
    }
    وإلا إذا (Py_ټوټه_Check(idx))
    {
	Py_ssize_t پیل, stop, ګام, slicelen;

	إذا (Py_ټوټه_Get_Indices_Ex((Py_ټوټه_Object_T *)idx, Tuple_اوږدتیا(self),
				 &پیل, &stop, &ګام, &slicelen) < 0)
	    أرجع عدم;
	أرجع Tuple_ټوټه(self, پیل, ګام, slicelen);
    }
    وإلا
    {
	RAISE_INVALID_شاخص_TYPE(idx);
	أرجع عدم;
    }
}

تعريفنوع بنية
{
    tuple_T	*tuple;
    صحيح		شاخص;
} tupleiterinfo_T;

    ساكن فراغ
TupleIterDestruct(فراغ *arg)
{
    tupleiterinfo_T *tii = (tupleiterinfo_T*)arg;
    tuple_unref(tii->tuple);
    PyMem_Free(tii);
}

    ساكن PyObject *
Tuple_Iter_بل(فراغ **arg)
{
    PyObject	*ret;
    tupleiterinfo_T **tii = (tupleiterinfo_T**)arg;

    إذا ((*tii)->شاخص >= TUPLE_LEN((*tii)->tuple))
	أرجع عدم;

    إذا (!(ret = Convert_منزل_Py_Object(TUPLE_توکی((*tii)->tuple, (*tii)->شاخص))))
	أرجع عدم;

    (*tii)->شاخص++;

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
	Py_Err_No_حافظه();
	أرجع عدم;
    }

    tii->tuple = t;
    tii->شاخص = 0;
    ++t->tv_refcount;

    أرجع IterNew(tii,
	    TupleIterDestruct, Tuple_Iter_بل,
	    عدم, عدم, (PyObject *)self);
}

ساكن حرف *TupleAttrs[] = {
    "locked",
    عدم
};

    ساكن PyObject *
TupleDir(PyObject *self, PyObject *دلیلونه ناکارول_شوی)
{
    أرجع ObjectDir(self, TupleAttrs);
}

    ساكن صحيح
TupleSetattr(PyObject *self_obj, حرف *name, PyObject *valObject)
{
    TupleObject *self = (TupleObject*)self_obj;
    إذا (valObject == عدم)
    {
	Py_Err_سېټ_STRING(PyExc_AttributeError,
		N_("cannot delete vim.Tuple attributes"));
	أرجع -1;
    }

    إذا (strcmp(name, "locked") == 0)
    {
	إذا (self->tuple->tv_قلف == VAR_FIXED)
	{
	    Py_Err_سېټ_STRING(PyExc_TypeError, N_("cannot modify fixed tuple"));
	    أرجع -1;
	}
	وإلا
	{
	    صحيح		istrue = PyObject_IsTrue(valObject);
	    إذا (istrue == -1)
		أرجع -1;
	    وإلا إذا (istrue)
		self->tuple->tv_قلف = VAR_LOCKED;
	    وإلا
		self->tuple->tv_قلف = 0;
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
    (lenfunc)		Tuple_اوږدتیا,	 // sq_length,	  len(x)
    (binaryfunc)	0,		 // RangeConcat, sq_concat,  x+y
    0,					 // RangeRepeat, sq_repeat,  x*n
    (PyIntArgFunc)	Tuple_شاخص,	 // sq_item,	  x[i]
    0,					 // was_sq_slice,     x[i:j]
    (PyIntObjArgProc)	0,		 // sq_as_item,  x[i]=v
    0,					 // was_sq_ass_slice, x[i:j]=v
    0,					 // sq_contains
    (binaryfunc)	0,		 // sq_inplace_concat
    0,					 // sq_inplace_repeat
};

ساكن PyMappingMethods TupleAsMapping = {
    /* mp_length	*/ (lenfunc) Tuple_اوږدتیا,
    /* mp_subscript     */ (binaryfunc) Tuple_توکی,
    /* mp_ass_subscript */ (objobjargproc) 0,
};

ساكن بنية Py_لاره_Def TupleMethods[] = {
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

#تعریف_کړه NEW_FUNCTION(name, argc, argv, self, pt_auto) \
    FunctionNew(FunctionTypePtr, (name), (argc), (argv), (self), (pt_auto))

    ساكن PyObject *
FunctionNew(PyTypeObject *subtype, char_u *name, صحيح argc, typval_T *argv,
	dict_T *selfdict, صحيح auto_rebind)
{
    FunctionObject	*self;

    self = (FunctionObject *) Py_TYPE_GET_TP_ALLOC(subtype)(subtype, 0);
    إذا (self == عدم)
	أرجع عدم;

    إذا (که_عدد((بینښه حرف)*name))
    {
	إذا (!translated_function_exists(name, FALSE))
	{
	    PyErr_FORMAT(Py_Exc_ارزښت_Error,
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
	    PyErr_FORMAT(Py_Exc_ارزښت_Error,
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
	pyll_جمع_کړه((PyObject *)(self), &self->ref, &lastfunc);

    أرجع (PyObject *)(self);
}

    ساكن PyObject *
FunctionConstructor(PyTypeObject *subtype, PyObject *دلیلونه, PyObject *kwargs)
{
    PyObject	*self;
    PyObject	*selfdictObject;
    PyObject	*autoRebindObject;
    PyObject	*دلیلونه_Object = عدم;
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
	selfdictObject = Py_Dict_Get_توکی_String(kwargs, "self");
	إذا (selfdictObject != عدم)
	{
	    إذا (ConvertFromPyMapping(selfdictObject, &selfdicttv) == -1)
		أرجع عدم;
	    selfdict = selfdicttv.vval.v_dict;
	}
	دلیلونه_Object = Py_Dict_Get_توکی_String(kwargs, "args");
	إذا (دلیلونه_Object != عدم)
	{
	    إذا (ConvertFromPySequence(دلیلونه_Object, &argstv) == -1)
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
		    Py_Err_No_حافظه();
		    dict_unref(selfdict);
		    list_unref(argslist);
		    أرجع عدم;
		}
		curtv = argv;
		FOR_ALL_LIST_توکي(argslist, li)
		    copy_tv(&li->li_tv, curtv++);
	    }
	    list_unref(argslist);
	}
	إذا (selfdict != عدم)
	{
	    auto_rebind = FALSE;
	    autoRebindObject = Py_Dict_Get_توکی_String(kwargs, "auto_rebind");
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

    إذا (!PyArg_ParseTuple(دلیلونه, "et", "ascii", &name))
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

    DESTRUCTOR_پای(self);
}

ساكن حرف *FunctionAttrs[] = {
    "softspace", "args", "self", "auto_rebind",
    عدم
};

    ساكن PyObject *
FunctionDir(PyObject *self, PyObject *دلیلونه ناکارول_شوی)
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
سېټ_partial(FunctionObject *self, partial_T *pt, صحيح exported)
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
FunctionCall(PyObject *self_obj, PyObject *دلیلونه_Object, PyObject *kwargs)
{
    FunctionObject *self = (FunctionObject*)self_obj;
    char_u	*name = self->name;
    typval_T	دلیلونه;
    typval_T	selfdicttv;
    typval_T	rettv;
    dict_T	*selfdict = عدم;
    PyObject	*selfdictObject;
    PyObject	*ret;
    صحيح		error;
    partial_T	pt;
    partial_T	*pt_ptr = عدم;

    إذا (ConvertFromPySequence(دلیلونه_Object, &دلیلونه) == -1)
	أرجع عدم;

    إذا (kwargs != عدم)
    {
	selfdictObject = Py_Dict_Get_توکی_String(kwargs, "self");
	إذا (selfdictObject != عدم)
	{
	    إذا (ConvertFromPyMapping(selfdictObject, &selfdicttv) == -1)
	    {
		clear_tv(&دلیلونه);
		أرجع عدم;
	    }
	    selfdict = selfdicttv.vval.v_dict;
	}
    }

    إذا (self->argv || self->self)
    {
	CLEAR_ډګر(pt);
	سېټ_partial(self, &pt, FALSE);
	pt_ptr = &pt;
    }

    Py_BEGIN_ALLOW_تارونه
    Python_قلف_Vim();

    Vim_Try_پیل();
    error = func_call(name, &دلیلونه, pt_ptr, selfdict, &rettv);

    Python_Release_Vim();
    Py_پای_ټکی_ALLOW_تارونه

    إذا (Vim_Try_پای_ټکی())
	ret = عدم;
    وإلا إذا (error != OK)
    {
	ret = عدم;
	PyErr_VIM_FORMAT(N_("failed to run function %s"), (حرف *)name);
    }
    وإلا
	ret = Convert_منزل_Py_Object(&rettv);

    clear_tv(&دلیلونه);
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
    ret = PyString_FromString((حرف *)repr_ga.ga_معلومات);
    ga_clear(&repr_ga);
    أرجع ret;
}

ساكن بنية Py_لاره_Def FunctionMethods[] = {
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
dummy_check(فراغ *arg ناکارول_شوی)
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
OptionsTraverse(PyObject *self_obj, visitproc لیدنه, فراغ *arg)
{
    OptionsObject *self = (OptionsObject*)self_obj;
    Py_لیدنه(self->fromObj);
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
Options_توکی(OptionsObject *self, PyObject *کلی_Object)
{
    char_u	*کلی;
    صحيح		flags;
    اوږد	numval;
    char_u	*stringval;
    PyObject	*todecref;

    إذا (self->Check(self->fromObj))
	أرجع عدم;

    إذا (!(کلی = String_منزل_Chars(کلی_Object, &todecref)))
	أرجع عدم;

    إذا (*کلی == NUL)
    {
	RAISE_NO_تشه_کلیګانې;
	Py_XDECREF(todecref);
	أرجع عدم;
    }

    flags = get_option_ارزښت_strict(کلی, &numval, &stringval,
				    self->opt_type, self->from);

    Py_XDECREF(todecref);

    إذا (flags == 0)
    {
	Py_Err_سېټ_Object(Py_Exc_کلی_Error, کلی_Object);
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
	    PyObject	*ret = Py_بایټونه_From_String((حرف *)stringval);
	    vim_free(stringval);
	    أرجع ret;
	}
	وإلا
	{
	    Py_Err_سېټ_STRING(PyExc_RuntimeError,
		    N_("unable to get option value"));
	    أرجع عدم;
	}
    }
    وإلا
    {
	Py_Err_سېټ_VIM(N_("internal error: unknown option type"));
	أرجع عدم;
    }
}

    ساكن صحيح
Options_لري(OptionsObject *self, PyObject *کلی_Object)
{
    char_u	*کلی;
    PyObject	*todecref;

    إذا (!(کلی = String_منزل_Chars(کلی_Object, &todecref)))
	أرجع -1;

    إذا (*کلی == NUL)
    {
	Py_XDECREF(todecref);
	أرجع 0;
    }

    إذا (get_option_ارزښت_strict(کلی, عدم, عدم, self->opt_type, عدم))
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
Options_Iter_بل(فراغ **arg)
{
    char_u	*name;
    optiterinfo_T **oii = (optiterinfo_T**)arg;

    إذا ((name = option_iter_بل(&((*oii)->lastoption), (*oii)->opt_type)))
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
	Py_Err_No_حافظه();
	أرجع عدم;
    }

    oii->opt_type = self->opt_type;
    oii->lastoption = عدم;

    أرجع IterNew(oii,
	    PyMem_Free, Options_Iter_بل,
	    عدم, عدم, (PyObject *)self);
}

    ساكن صحيح
سېټ_option_ارزښت_err(char_u *کلی, صحيح numval, char_u *stringval, صحيح opt_flags)
{
    حرف	*errmsg;

    إذا ((errmsg = سېټ_option_ارزښت(کلی, numval, stringval, opt_flags)))
    {
	إذا (Vim_Try_پای_ټکی())
	    أرجع FAIL;
	Py_Err_سېټ_Vim(errmsg);
	أرجع FAIL;
    }
    أرجع OK;
}

    ساكن صحيح
سېټ_option_ارزښت_for(
	char_u *کلی,
	صحيح numval,
	char_u *stringval,
	صحيح opt_flags,
	صحيح opt_type,
	فراغ *from)
{
    switchwin_T	switchwin;
    bufref_T	save_curbuf;
    صحيح		سېټ_ret = 0;

    Vim_Try_پیل();
    بدل (opt_type)
    {
	حالة SREQ_WIN:
	    إذا (switch_win(&switchwin, (win_T *)from,
			      win_پیدا_کړه_tabpage((win_T *)from), FALSE) == FAIL)
	    {
		restore_win(&switchwin, TRUE);
		إذا (Vim_Try_پای_ټکی())
		    أرجع -1;
		Py_Err_سېټ_VIM(N_("problem while switching windows"));
		أرجع -1;
	    }
	    سېټ_ret = سېټ_option_ارزښت_err(کلی, numval, stringval, opt_flags);
	    restore_win(&switchwin, TRUE);
	    اكسر;
	حالة SREQ_BUF:
	    switch_بفر(&save_curbuf, (buf_T *)from);
	    سېټ_ret = سېټ_option_ارزښت_err(کلی, numval, stringval, opt_flags);
	    restore_بفر(&save_curbuf);
	    اكسر;
	حالة SREQ_GLOBAL:
	    سېټ_ret = سېټ_option_ارزښت_err(کلی, numval, stringval, opt_flags);
	    اكسر;
    }
    إذا (سېټ_ret == FAIL)
	أرجع -1;
    أرجع Vim_Try_پای_ټکی();
}

    ساكن صحيح
Options_Ass_توکی(OptionsObject *self, PyObject *کلی_Object, PyObject *valObject)
{
    char_u	*کلی;
    صحيح		flags;
    صحيح		opt_flags;
    صحيح		ret = 0;
    PyObject	*todecref;

    إذا (self->Check(self->fromObj))
	أرجع -1;

    إذا (!(کلی = String_منزل_Chars(کلی_Object, &todecref)))
	أرجع -1;

    إذا (*کلی == NUL)
    {
	RAISE_NO_تشه_کلیګانې;
	Py_XDECREF(todecref);
	أرجع -1;
    }

    flags = get_option_ارزښت_strict(کلی, عدم, عدم,
				    self->opt_type, self->from);

    إذا (flags == 0)
    {
	Py_Err_سېټ_Object(Py_Exc_کلی_Error, کلی_Object);
	Py_XDECREF(todecref);
	أرجع -1;
    }

    إذا (valObject == عدم)
    {
	إذا (self->opt_type == SREQ_GLOBAL)
	{
	    PyErr_FORMAT(Py_Exc_ارزښت_Error,
		    N_("unable to unset global option %s"), کلی);
	    Py_XDECREF(todecref);
	    أرجع -1;
	}
	وإلا إذا (!(flags & SOPT_GLOBAL))
	{
	    PyErr_FORMAT(Py_Exc_ارزښت_Error,
		    N_("unable to unset option %s "
		       "which does not have global value"), کلی);
	    Py_XDECREF(todecref);
	    أرجع -1;
	}
	وإلا
	{
	    unset_global_local_option(کلی, self->from);
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
	    ret = سېټ_option_ارزښت_for(کلی, istrue, عدم,
				    opt_flags, self->opt_type, self->from);
    }
    وإلا إذا (flags & SOPT_عدد)
    {
	اوږد	val;

	إذا (شمېره_منزل_Long(valObject, &val, شمېره_INT))
	{
	    Py_XDECREF(todecref);
	    أرجع -1;
	}

	ret = سېټ_option_ارزښت_for(کلی, (صحيح) val, عدم, opt_flags,
				self->opt_type, self->from);
    }
    وإلا
    {
	char_u		*val;
	PyObject	*todecref2;

	إذا ((val = String_منزل_Chars(valObject, &todecref2)))
	{
	    ret = سېټ_option_ارزښت_for(کلی, 0, val, opt_flags,
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
    (objobjproc) Options_لري,	// sq_contains
    0,					// sq_inplace_concat
    0,					// sq_inplace_repeat
};

ساكن PyMappingMethods OptionsAsMapping = {
    (lenfunc)       عدم,
    (binaryfunc)    Options_توکی,
    (objobjargproc) Options_Ass_توکی,
};

// Tabpage object

تعريفنوع بنية
{
    Py_Object_سر
    tabpage_T	*tab;
} Tab_پاڼه_Object;

ساكن PyObject *WinListNew(Tab_پاڼه_Object *tabObject);

DEFINE_PY_TYPE_OBJECT(Tab_پاڼه_Type);

    ساكن صحيح
Check_Tab_پاڼه(Tab_پاڼه_Object *self)
{
    إذا (self->tab == INVALID_TABPAGE_ارزښت)
    {
	Py_Err_سېټ_VIM(N_("attempt to refer to deleted tab page"));
	أرجع -1;
    }

    أرجع 0;
}

    ساكن PyObject *
Tab_پاڼه_New(tabpage_T *tab)
{
    Tab_پاڼه_Object *self;

    إذا (TAB_PYTHON_REF(tab))
    {
	self = TAB_PYTHON_REF(tab);
	Py_INCREF((PyObject *)self);
    }
    وإلا
    {
	self = PyObject_NEW(Tab_پاڼه_Object, Tab_پاڼه_Type_Ptr);
	إذا (self == عدم)
	    أرجع عدم;
	self->tab = tab;
	TAB_PYTHON_REF(tab) = self;
    }

    أرجع (PyObject *)(self);
}

    ساكن فراغ
Tab_پاڼه_Destructor(PyObject *self_obj)
{
    Tab_پاڼه_Object *self = (Tab_پاڼه_Object*)self_obj;
    إذا (self->tab && self->tab != INVALID_TABPAGE_ارزښت)
	TAB_PYTHON_REF(self->tab) = عدم;

    DESTRUCTOR_پای(self);
}

ساكن حرف *Tab_پاڼه_Attrs[] = {
    "windows", "number", "vars", "window", "valid",
    عدم
};

    ساكن PyObject *
Tab_پاڼه_Dir(PyObject *self, PyObject *دلیلونه ناکارول_شوی)
{
    أرجع ObjectDir(self, Tab_پاڼه_Attrs);
}

    ساكن PyObject *
Tab_پاڼه_Attr_Valid(Tab_پاڼه_Object *self, حرف *name)
{
    PyObject	*ret;

    إذا (strcmp(name, "valid") != 0)
	أرجع عدم;

    ret = ((self->tab == INVALID_TABPAGE_ارزښت) ? Py_False : Py_True);
    Py_INCREF(ret);
    أرجع ret;
}

    ساكن PyObject *
Tab_پاڼه_Attr(Tab_پاڼه_Object *self, حرف *name)
{
    إذا (strcmp(name, "windows") == 0)
	أرجع WinListNew(self);
    وإلا إذا (strcmp(name, "number") == 0)
	أرجع PyLong_FromLong((اوږد) get_tab_شمېره(self->tab));
    وإلا إذا (strcmp(name, "vars") == 0)
	أرجع NEW_DICTIONARY(self->tab->tp_vars);
    وإلا إذا (strcmp(name, "window") == 0)
    {
	// For current tab window.c does not bother to set or update tp_curwin
	إذا (self->tab == curtab)
	    أرجع کړکۍ_New(curwin, curtab);
	وإلا
	    أرجع کړکۍ_New(self->tab->tp_curwin, self->tab);
    }
    وإلا إذا (strcmp(name, "__members__") == 0)
	أرجع ObjectDir(عدم, Tab_پاڼه_Attrs);
    أرجع عدم;
}

    ساكن PyObject *
Tab_پاڼه_Repr(PyObject *self_obj)
{
    Tab_پاڼه_Object *self = (Tab_پاڼه_Object*)self_obj;
    إذا (self->tab == INVALID_TABPAGE_ارزښت)
	أرجع PyString_FromFormat("<tabpage object (deleted) at %p>", (فراغ *)self);
    وإلا
    {
	صحيح	t = get_tab_شمېره(self->tab);

	إذا (t == 0)
	    أرجع PyString_FromFormat("<tabpage object (unknown) at %p>",
					(فراغ *)self);
	وإلا
	    أرجع PyString_FromFormat("<tabpage %d>", t - 1);
    }
}

ساكن بنية Py_لاره_Def Tab_پاڼه_Methods[] = {
    // name,	    function,			calling,	documentation
    {"__dir__",	    (PyCFunction)Tab_پاڼه_Dir,	METH_NOARGS,	""},
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

ساكن TabListObject The_Tab_پاڼه_List =
{
    Py_Object_سر_INIT_TYPE(TabListType)
};

    ساكن PyInt
Tab_List_اوږدتیا(PyObject *self ناکارول_شوی)
{
    tabpage_T	*tp = لومړی_tabpage;
    PyInt	n = 0;

    طالما (tp != عدم)
    {
	++n;
	tp = tp->tp_بل;
    }

    أرجع n;
}

    ساكن PyObject *
Tab_List_توکی(PyObject *self ناکارول_شوی, PyInt n)
{
    tabpage_T	*tp;

    لكل (tp = لومړی_tabpage; tp != عدم; tp = tp->tp_بل, --n)
	إذا (n == 0)
	    أرجع Tab_پاڼه_New(tp);

    Py_Err_سېټ_STRING(Py_Exc_شاخص_Error, N_("no such tab page"));
    أرجع عدم;
}

/*
 * Window object
 */

تعريفنوع بنية
{
    Py_Object_سر
    win_T	*win;
    Tab_پاڼه_Object	*tabObject;
} کړکۍ_Object;

DEFINE_PY_TYPE_OBJECT(کړکۍ_Type);

    ساكن صحيح
Check_کړکۍ(کړکۍ_Object *self)
{
    إذا (self->win == INVALID_کړکۍ_ارزښت)
    {
	Py_Err_سېټ_VIM(N_("attempt to refer to deleted window"));
	أرجع -1;
    }

    أرجع 0;
}

    ساكن صحيح
Check_کړکۍ_Cb(فراغ *self)
{
    أرجع Check_کړکۍ((کړکۍ_Object*)self);
}

    ساكن PyObject *
کړکۍ_New(win_T *win, tabpage_T *tab)
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

    کړکۍ_Object *self;

    إذا (WIN_PYTHON_REF(win))
    {
	self = WIN_PYTHON_REF(win);
	Py_INCREF((PyObject *)self);
    }
    وإلا
    {
	self = PyObject_GC_New(کړکۍ_Object, کړکۍ_Type_Ptr);
	إذا (self == عدم)
	    أرجع عدم;
	self->win = win;
	WIN_PYTHON_REF(win) = self;
    }

    self->tabObject = ((Tab_پاڼه_Object *)(Tab_پاڼه_New(tab)));

    أرجع (PyObject *)(self);
}

    ساكن فراغ
کړکۍ_Destructor(PyObject *self_obj)
{
    کړکۍ_Object *self = (کړکۍ_Object*)self_obj;
    PyObject_GC_UnTrack((فراغ *)(self));
    إذا (self->win && self->win != INVALID_کړکۍ_ارزښت)
	WIN_PYTHON_REF(self->win) = عدم;
    Py_XDECREF(((PyObject *)(self->tabObject)));
    PyObject_GC_Del((فراغ *)(self));
}

    ساكن صحيح
کړکۍ_Traverse(PyObject *self_obj, visitproc لیدنه, فراغ *arg)
{
    کړکۍ_Object *self = (کړکۍ_Object*)self_obj;
    Py_لیدنه(((PyObject *)(self->tabObject)));
    أرجع 0;
}

    ساكن صحيح
کړکۍ_Clear(PyObject *self_obj)
{
    کړکۍ_Object *self = (کړکۍ_Object*)self_obj;
    Py_CLEAR(self->tabObject);
    أرجع 0;
}

    ساكن win_T *
get_firstwin(Tab_پاڼه_Object *tabObject)
{
    إذا (tabObject)
    {
	إذا (Check_Tab_پاڼه(tabObject))
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
ساكن حرف *کړکۍ_Attrs[] = {
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
کړکۍ_Dir(PyObject *self, PyObject *دلیلونه ناکارول_شوی)
{
    أرجع ObjectDir(self, کړکۍ_Attrs);
}

    ساكن PyObject *
کړکۍ_Attr_Valid(کړکۍ_Object *self, حرف *name)
{
    PyObject	*ret;

    إذا (strcmp(name, "valid") != 0)
	أرجع عدم;

    ret = ((self->win == INVALID_کړکۍ_ارزښت) ? Py_False : Py_True);
    Py_INCREF(ret);
    أرجع ret;
}

    ساكن PyObject *
کړکۍ_Attr(کړکۍ_Object *self, حرف *name)
{
    إذا (strcmp(name, "buffer") == 0)
	أرجع (PyObject *)بفر_New(self->win->w_بفر);
    وإلا إذا (strcmp(name, "cursor") == 0)
    {
	pos_T *pos = &self->win->w_cursor;

	أرجع Py_Build_ارزښت("(ll)", (اوږد)(pos->lnum), (اوږد)(pos->ستون));
    }
    وإلا إذا (strcmp(name, "height") == 0)
	أرجع PyLong_FromLong((اوږد)(self->win->w_لوړوالی));
    وإلا إذا (strcmp(name, "row") == 0)
	أرجع PyLong_FromLong((اوږد)(self->win->w_winrow));
    وإلا إذا (strcmp(name, "width") == 0)
	أرجع PyLong_FromLong((اوږد)(self->win->w_پلنوالی));
    وإلا إذا (strcmp(name, "col") == 0)
	أرجع PyLong_FromLong((اوږد)(self->win->w_wincol));
    وإلا إذا (strcmp(name, "vars") == 0)
	أرجع NEW_DICTIONARY(self->win->w_vars);
    وإلا إذا (strcmp(name, "options") == 0)
	أرجع OptionsNew(SREQ_WIN, self->win, Check_کړکۍ_Cb,
			(PyObject *) self);
    وإلا إذا (strcmp(name, "number") == 0)
    {
	إذا (Check_Tab_پاڼه(self->tabObject))
	    أرجع عدم;
	أرجع PyLong_FromLong((اوږد)
		get_win_شمېره(self->win, get_firstwin(self->tabObject)));
    }
    وإلا إذا (strcmp(name, "tabpage") == 0)
    {
	Py_INCREF((PyObject *)self->tabObject);
	أرجع (PyObject *)(self->tabObject);
    }
    وإلا إذا (strcmp(name, "__members__") == 0)
	أرجع ObjectDir(عدم, کړکۍ_Attrs);
    وإلا
	أرجع عدم;
}

    ساكن صحيح
کړکۍ_Setattr(PyObject *self_obj, حرف *name, PyObject *valObject)
{
    کړکۍ_Object *self = (کړکۍ_Object*)self_obj;
    إذا (Check_کړکۍ(self))
	أرجع -1;

    إذا (strcmp(name, "buffer") == 0)
    {
	Py_Err_سېټ_STRING(PyExc_TypeError, N_("readonly attribute: buffer"));
	أرجع -1;
    }
    وإلا إذا (strcmp(name, "cursor") == 0)
    {
	اوږد lnum;
	اوږد ستون;

	إذا (!PyArg_Parse(valObject, "(ll)", &lnum, &ستون))
	    أرجع -1;

	إذا (lnum <= 0 || lnum > self->win->w_بفر->b_ml.ml_کرښه_شمېر)
	{
	    Py_Err_سېټ_VIM(N_("cursor position outside buffer"));
	    أرجع -1;
	}

	// Check for keyboard interrupts
	إذا (Vim_Check_مداخله())
	    أرجع -1;

	self->win->w_cursor.lnum = lnum;
	self->win->w_cursor.ستون = ستون;
	self->win->w_سېټ_curswant = TRUE;
	self->win->w_cursor.coladd = 0;
	// When column is out of range silently correct it.
	check_cursor_ستون_win(self->win);

	update_screen(UPD_VALID);
	أرجع 0;
    }
    وإلا إذا (strcmp(name, "height") == 0)
    {
	اوږد	لوړوالی;
	win_T	*savewin;

	إذا (شمېره_منزل_Long(valObject, &لوړوالی, شمېره_INT|شمېره_UNSIGNED))
	    أرجع -1;

#که_تعریف_شوی FEAT_GUI
	need_mouse_correct = TRUE;
#د_که_پای
	savewin = curwin;
	curwin = self->win;
	curbuf = curwin->w_بفر;

	Vim_Try_پیل();
	win_setheight((صحيح) لوړوالی);
	curwin = savewin;
	curbuf = curwin->w_بفر;
	إذا (Vim_Try_پای_ټکی())
	    أرجع -1;

	أرجع 0;
    }
    وإلا إذا (strcmp(name, "width") == 0)
    {
	اوږد	پلنوالی;
	win_T	*savewin;

	إذا (شمېره_منزل_Long(valObject, &پلنوالی, شمېره_INT|شمېره_UNSIGNED))
	    أرجع -1;

#که_تعریف_شوی FEAT_GUI
	need_mouse_correct = TRUE;
#د_که_پای
	savewin = curwin;
	curwin = self->win;
	curbuf = curwin->w_بفر;

	Vim_Try_پیل();
	win_setwidth((صحيح) پلنوالی);
	curwin = savewin;
	curbuf = curwin->w_بفر;
	إذا (Vim_Try_پای_ټکی())
	    أرجع -1;

	أرجع 0;
    }
    وإلا
    {
	Py_Err_سېټ_String(PyExc_AttributeError, name);
	أرجع -1;
    }
}

    ساكن PyObject *
کړکۍ_Repr(PyObject *self_obj)
{
    کړکۍ_Object *self = (کړکۍ_Object*)self_obj;
    إذا (self->win == INVALID_کړکۍ_ارزښت)
	أرجع PyString_FromFormat("<window object (deleted) at %p>", (فراغ *)self);
    وإلا
    {
	صحيح	w = get_win_شمېره(self->win, firstwin);

	إذا (w == 0)
	    أرجع PyString_FromFormat("<window object (unknown) at %p>",
								      (فراغ *)self);
	وإلا
	    أرجع PyString_FromFormat("<window %d>", w - 1);
    }
}

ساكن بنية Py_لاره_Def کړکۍ_Methods[] = {
    // name,	    function,			calling,	documentation
    {"__dir__",	    (PyCFunction)کړکۍ_Dir,	METH_NOARGS,	""},
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
    Tab_پاڼه_Object	*tabObject;
} WinListObject;

ساكن WinListObject The_کړکۍ_List =
{
    Py_Object_سر_INIT_TYPE(WinListType)
    عدم
};

    ساكن PyObject *
WinListNew(Tab_پاڼه_Object *tabObject)
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
    Tab_پاڼه_Object	*tabObject = self->tabObject;

    إذا (tabObject)
    {
	Py_DECREF((PyObject *)(tabObject));
    }

    DESTRUCTOR_پای(self);
}

    ساكن PyInt
Win_List_اوږدتیا(WinListObject *self)
{
    win_T	*w;
    PyInt	n = 0;

    إذا (!(w = get_firstwin(self->tabObject)))
	أرجع -1;

    طالما (w != عدم)
    {
	++n;
	w = W_بل(w);
    }

    أرجع n;
}

    ساكن PyObject *
Win_List_توکی(WinListObject *self, PyInt n)
{
    win_T *w;

    إذا (!(w = get_firstwin(self->tabObject)))
	أرجع عدم;

    لكل (; w != عدم; w = W_بل(w), --n)
	إذا (n == 0)
	    أرجع کړکۍ_New(w, self->tabObject? self->tabObject->tab: curtab);

    Py_Err_سېټ_STRING(Py_Exc_شاخص_Error, N_("no such window"));
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
String_منزل_کرښه(PyObject *obj)
{
    حرف	*str;
    حرف	*save;
    PyObject	*بایټونه = عدم;
    Py_ssize_t	len = 0;
    PyInt	i;
    حرف	*p;

    إذا (Py_بایټونه_Check(obj))
    {
	إذا (Py_بایټونه_As_String_And_اندازه(obj, &str, &len) == -1
		|| str == عدم)
	    أرجع عدم;
    }
    وإلا إذا (PyUnicode_Check(obj))
    {
	إذا (!(بایټونه = PyUnicode_AsEncodedString(obj, ENC_OPT,
							   ERRORS_ENCODE_ARG)))
	    أرجع عدم;

	إذا (Py_بایټونه_As_String_And_اندازه(بایټونه, &str, &len) == -1
		|| str == عدم)
	{
	    Py_DECREF(بایټونه);
	    أرجع عدم;
	}
    }
    وإلا
    {
#إذا PY_MAJOR_نسخه < 3
	PyErr_FORMAT_TYPE(
		N_("expected str() or unicode() instance, but got %s"),
		obj);
#وإلا
	PyErr_FORMAT_TYPE(
		N_("expected bytes() or str() instance, but got %s"),
		obj);
#د_که_پای
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
	    Py_Err_سېټ_VIM(N_("string cannot contain newlines"));
	    Py_XDECREF(بایټونه);
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
	Py_Err_No_حافظه();
	Py_XDECREF(بایټونه);
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
    Py_XDECREF(بایټونه);  // Python 2 does nothing here

    أرجع save;
}

/*
 * Get a line from the specified buffer. The line number is
 * in Vim format (1-based). The line is returned as a Python
 * string object.
 */
    ساكن PyObject *
Get_بفر_کرښه(buf_T *buf, PyInt n)
{
    أرجع کرښه_منزل_String((حرف *)ml_get_buf(buf, (linenr_T)n, FALSE));
}


/*
 * Get a list of lines from the specified buffer. The line numbers
 * are in Vim format (1-based). The range is from lo up to, but not
 * including, hi. The list is returned as a Python list of string objects.
 */
    ساكن PyObject *
Get_بفر_کرښه_List(buf_T *buf, PyInt lo, PyInt hi)
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

	إذا (lnum > buf->b_ml.ml_کرښه_شمېر)
	    متن = "";
	وإلا
	    متن = (حرف *)ml_get_buf(buf, lnum, FALSE);
	string = کرښه_منزل_String(متن);
	إذا (string == عدم)
	{
	    Py_DECREF(list);
	    أرجع عدم;
	}

	Py_List_سېټ_توکی(list, i, string);
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
	    check_cursor_ستون();
	}
	وإلا إذا (extra < 0)
	{
	    curwin->w_cursor.lnum = lo;
	    check_cursor();
	}
	وإلا
	    check_cursor_ستون();
	بدل_شوی_cline_bef_curs();
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
سېټ_بفر_کرښه_2(buf_T *buf, PyInt n, PyObject *کرښه, PyInt *len_change)
{
    bufref_T	save_curbuf = {عدم, 0, 0};
    switchwin_T	switchwin;

    // First of all, we check the type of the supplied Python object.
    // There are three cases:
    //	  1. NULL, or None - this is a deletion.
    //	  2. A string	   - this is a replacement.
    //	  3. Anything else - this is an error.
    إذا (کرښه == Py_None || کرښه == عدم)
    {
	PyErr_Clear();
	switchwin.sw_curwin = عدم;
	switch_منزل_win_for_buf(buf, &switchwin, &save_curbuf);

	Vim_Try_پیل();

	إذا (u_savedel((linenr_T)n, 1L) == FAIL)
	    RAISE_UNDO_FAIL;
	وإلا إذا (ml_delete((linenr_T)n) == FAIL)
	    RAISE_DELETE_کرښه_FAIL;
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
		deleted_کرښې_mark((linenr_T)n, 1L);
	}

	restore_win_for_buf(&switchwin, &save_curbuf);

	إذا (Vim_Try_پای_ټکی())
	    أرجع FAIL;

	إذا (len_change)
	    *len_change = -1;

	أرجع OK;
    }
    وإلا إذا (Py_بایټونه_Check(کرښه) || PyUnicode_Check(کرښه))
    {
	حرف	*save = String_منزل_کرښه(کرښه);

	إذا (save == عدم)
	    أرجع FAIL;

	Vim_Try_پیل();

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
	    RAISE_REPLACE_کرښه_FAIL;
	    vim_free(save);
	}
	وإلا
	    بدل_شوی_بایټونه((linenr_T)n, 0);

	restore_win_for_buf(&switchwin, &save_curbuf);

	// Check that the cursor is not beyond the end of the line now.
	إذا (buf == curbuf)
	    check_cursor_ستون();

	إذا (Vim_Try_پای_ټکی())
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
سېټ_بفر_کرښه_List(
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
	Vim_Try_پیل();
	switchwin.sw_curwin = عدم;
	switch_منزل_win_for_buf(buf, &switchwin, &save_curbuf);

	إذا (u_savedel((linenr_T)lo, (اوږد)n) == FAIL)
	    RAISE_UNDO_FAIL;
	وإلا
	{
	    لكل (i = 0; i < n; ++i)
	    {
		إذا (ml_delete((linenr_T)lo) == FAIL)
		{
		    RAISE_DELETE_کرښه_FAIL;
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
		deleted_کرښې_mark((linenr_T)lo, (اوږد)i);
	}

	restore_win_for_buf(&switchwin, &save_curbuf);

	إذا (Vim_Try_پای_ټکی())
	    أرجع FAIL;

	إذا (len_change)
	    *len_change = -n;

	أرجع OK;
    }
    وإلا إذا (PyList_Check(list))
    {
	PyInt	i;
	PyInt	new_len = Py_List_اندازه(list);
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
		Py_Err_No_حافظه();
		أرجع FAIL;
	    }
	}

	لكل (i = 0; i < new_len; ++i)
	{
	    PyObject *کرښه;

	    إذا (!(کرښه = Py_List_Get_توکی(list, i)) ||
		!(array[i] = String_منزل_کرښه(کرښه)))
	    {
		طالما (i)
		    vim_free(array[--i]);
		PyMem_Free(array);
		أرجع FAIL;
	    }
	}

	Vim_Try_پیل();
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
		    RAISE_DELETE_کرښه_FAIL;
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
		    RAISE_REPLACE_کرښه_FAIL;
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
		    RAISE_ورننباسه_کرښه_FAIL;
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
						  (اوږد)MAXLNUM, (اوږد)extra);
	    بدل_شوی_کرښې((linenr_T)lo, 0, (linenr_T)hi, (اوږد)extra);
	}

	إذا (buf == curbuf && (switchwin.sw_curwin != عدم
					   || save_curbuf.br_buf == عدم))
	    // Using an existing window for the buffer, adjust the cursor
	    // position.
	    py_fix_cursor((linenr_T)lo, (linenr_T)hi, (linenr_T)extra);

	// END of region without "return".
	restore_win_for_buf(&switchwin, &save_curbuf);

	إذا (Vim_Try_پای_ټکی())
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
ورننباسه_بفر_کرښې(buf_T *buf, PyInt n, PyObject *کرښې, PyInt *len_change)
{
    bufref_T	save_curbuf = {عدم, 0, 0};
    switchwin_T	switchwin;

    // First of all, we check the type of the supplied Python object.
    // It must be a string or a list, or the call is in error.
    إذا (Py_بایټونه_Check(کرښې) || PyUnicode_Check(کرښې))
    {
	حرف		*str = String_منزل_کرښه(کرښې);

	إذا (str == عدم)
	    أرجع FAIL;

	PyErr_Clear();
	Vim_Try_پیل();
	switch_منزل_win_for_buf(buf, &switchwin, &save_curbuf);

	إذا (u_save((linenr_T)n, (linenr_T)(n + 1)) == FAIL)
	    RAISE_UNDO_FAIL;
	وإلا إذا (ml_append((linenr_T)n, (char_u *)str, 0, FALSE) == FAIL)
	    RAISE_ورننباسه_کرښه_FAIL;
	وإلا إذا (save_curbuf.br_buf == عدم)
	    // Only adjust marks if we managed to switch to a window that
	    // holds the buffer, otherwise line numbers will be invalid.
	    appended_کرښې_mark((linenr_T)n, 1L);

	vim_free(str);
	restore_win_for_buf(&switchwin, &save_curbuf);
	update_screen(UPD_VALID);

	إذا (Vim_Try_پای_ټکی())
	    أرجع FAIL;

	إذا (len_change)
	    *len_change = 1;

	أرجع OK;
    }
    وإلا إذا (PyList_Check(کرښې))
    {
	PyInt	i;
	PyInt	اندازه = Py_List_اندازه(کرښې);
	حرف	**array;

	array = PyMem_New(حرف *, اندازه);
	إذا (array == عدم)
	{
	    Py_Err_No_حافظه();
	    أرجع FAIL;
	}

	لكل (i = 0; i < اندازه; ++i)
	{
	    PyObject *کرښه;

	    إذا (!(کرښه = Py_List_Get_توکی(کرښې, i)) ||
		!(array[i] = String_منزل_کرښه(کرښه)))
	    {
		طالما (i)
		    vim_free(array[--i]);
		PyMem_Free(array);
		أرجع FAIL;
	    }
	}

	PyErr_Clear();
	Vim_Try_پیل();
	switch_منزل_win_for_buf(buf, &switchwin, &save_curbuf);

	إذا (u_save((linenr_T)n, (linenr_T)(n + 1)) == FAIL)
	    RAISE_UNDO_FAIL;
	وإلا
	{
	    لكل (i = 0; i < اندازه; ++i)
	    {
		إذا (ml_append((linenr_T)(n + i),
					(char_u *)array[i], 0, FALSE) == FAIL)
		{
		    RAISE_ورننباسه_کرښه_FAIL;

		    // Free the rest of the lines
		    طالما (i < اندازه)
			vim_free(array[i++]);

		    اكسر;
		}
		vim_free(array[i]);
	    }
	    إذا (i > 0 && save_curbuf.br_buf == عدم)
		// Only adjust marks if we managed to switch to a window that
		// holds the buffer, otherwise line numbers will be invalid.
		appended_کرښې_mark((linenr_T)n, (اوږد)i);
	}

	// Free the array of lines. All of its contents have now
	// been freed.
	PyMem_Free(array);
	restore_win_for_buf(&switchwin, &save_curbuf);

	update_screen(UPD_VALID);

	إذا (Vim_Try_پای_ټکی())
	    أرجع FAIL;

	إذا (len_change)
	    *len_change = اندازه;

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
    إذا (self->buf == INVALID_بفر_ارزښت)
    {
	Py_Err_سېټ_VIM(N_("attempt to refer to deleted buffer"));
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
RBItem(بفر_Object *self, PyInt n, PyInt پیل, PyInt پای_ټکی)
{
    إذا (Check_بفر(self))
	أرجع عدم;

    إذا (پای_ټکی == -1)
	پای_ټکی = self->buf->b_ml.ml_کرښه_شمېر;

    إذا (n < 0)
	n += پای_ټکی - پیل + 1;

    إذا (n < 0 || n > پای_ټکی - پیل)
    {
	Py_Err_سېټ_STRING(Py_Exc_شاخص_Error, N_("line number out of range"));
	أرجع عدم;
    }

    أرجع Get_بفر_کرښه(self->buf, n+پیل);
}

    ساكن PyObject *
RBSlice(بفر_Object *self, PyInt lo, PyInt hi, PyInt پیل, PyInt پای_ټکی)
{
    PyInt اندازه;

    إذا (Check_بفر(self))
	أرجع عدم;

    إذا (پای_ټکی == -1)
	پای_ټکی = self->buf->b_ml.ml_کرښه_شمېر;

    اندازه = پای_ټکی - پیل + 1;

    إذا (lo < 0)
	lo = 0;
    وإلا إذا (lo > اندازه)
	lo = اندازه;
    إذا (hi < 0)
	hi = 0;
    إذا (hi < lo)
	hi = lo;
    وإلا إذا (hi > اندازه)
	hi = اندازه;

    أرجع Get_بفر_کرښه_List(self->buf, lo+پیل, hi+پیل);
}

    ساكن PyInt
RBAs_توکی(
	بفر_Object *self,
	PyInt n,
	PyObject *valObject,
	PyInt پیل,
	PyInt پای_ټکی,
	PyInt *new_پای_ټکی)
{
    PyInt len_change;

    إذا (Check_بفر(self))
	أرجع -1;

    إذا (پای_ټکی == -1)
	پای_ټکی = self->buf->b_ml.ml_کرښه_شمېر;

    إذا (n < 0)
	n += پای_ټکی - پیل + 1;

    إذا (n < 0 || n > پای_ټکی - پیل)
    {
	Py_Err_سېټ_STRING(Py_Exc_شاخص_Error, N_("line number out of range"));
	أرجع -1;
    }

    إذا (سېټ_بفر_کرښه_2(self->buf, n+پیل, valObject, &len_change) == FAIL)
	أرجع -1;

    إذا (new_پای_ټکی)
	*new_پای_ټکی = پای_ټکی + len_change;

    أرجع 0;
}

    ساكن PyInt
RBAs_ټوټه(
	بفر_Object *self,
	PyInt lo,
	PyInt hi,
	PyObject *valObject,
	PyInt پیل,
	PyInt پای_ټکی,
	PyInt *new_پای_ټکی)
{
    PyInt اندازه;
    PyInt len_change;

    // Self must be a valid buffer
    إذا (Check_بفر(self))
	أرجع -1;

    إذا (پای_ټکی == -1)
	پای_ټکی = self->buf->b_ml.ml_کرښه_شمېر;

    // Sort out the slice range
    اندازه = پای_ټکی - پیل + 1;

    إذا (lo < 0)
	lo = 0;
    وإلا إذا (lo > اندازه)
	lo = اندازه;
    إذا (hi < 0)
	hi = 0;
    إذا (hi < lo)
	hi = lo;
    وإلا إذا (hi > اندازه)
	hi = اندازه;

    إذا (سېټ_بفر_کرښه_List(self->buf, lo + پیل, hi + پیل,
						valObject, &len_change) == FAIL)
	أرجع -1;

    إذا (new_پای_ټکی)
	*new_پای_ټکی = پای_ټکی + len_change;

    أرجع 0;
}


    ساكن PyObject *
RBAppend(
	بفر_Object *self,
	PyObject *دلیلونه,
	PyInt پیل,
	PyInt پای_ټکی,
	PyInt *new_پای_ټکی)
{
    PyObject *کرښې;
    PyInt len_change;
    PyInt max;
    PyInt n;

    إذا (Check_بفر(self))
	أرجع عدم;

    إذا (پای_ټکی == -1)
	پای_ټکی = self->buf->b_ml.ml_کرښه_شمېر;

    max = n = پای_ټکی - پیل + 1;

    إذا (!PyArg_ParseTuple(دلیلونه, "O|n", &کرښې, &n))
	أرجع عدم;

    إذا (n < 0 || n > max)
    {
	Py_Err_سېټ_STRING(Py_Exc_شاخص_Error, N_("line number out of range"));
	أرجع عدم;
    }

    إذا (ورننباسه_بفر_کرښې(self->buf, n + پیل - 1, کرښې, &len_change) == FAIL)
	أرجع عدم;

    إذا (new_پای_ټکی)
	*new_پای_ټکی = پای_ټکی + len_change;

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
    PyInt پیل;
    PyInt پای_ټکی;
} RangeObject;

    ساكن PyObject *
RangeNew(buf_T *buf, PyInt پیل, PyInt پای_ټکی)
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
    self->پیل = پیل;
    self->پای_ټکی = پای_ټکی;

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
RangeTraverse(PyObject *self_obj, visitproc لیدنه, فراغ *arg)
{
    RangeObject *self = (RangeObject*)self_obj;
    Py_لیدنه(((PyObject *)(self->buf)));
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
Range_اوږدتیا(RangeObject *self)
{
    // HOW DO WE SIGNAL AN ERROR FROM THIS FUNCTION?
    إذا (Check_بفر(self->buf))
	أرجع -1; // ???

    أرجع (self->پای_ټکی - self->پیل + 1);
}

    ساكن PyObject *
Range_توکی(RangeObject *self, PyInt n)
{
    أرجع RBItem(self->buf, n, self->پیل, self->پای_ټکی);
}

    ساكن PyObject *
Range_ټوټه(RangeObject *self, PyInt lo, PyInt hi)
{
    أرجع RBSlice(self->buf, lo, hi, self->پیل, self->پای_ټکی);
}

ساكن حرف *RangeAttrs[] = {
    "start", "end",
    عدم
};

    ساكن PyObject *
RangeDir(PyObject *self, PyObject *دلیلونه ناکارول_شوی)
{
    أرجع ObjectDir(self, RangeAttrs);
}

    ساكن PyObject *
RangeAppend(RangeObject *self, PyObject *دلیلونه)
{
    أرجع RBAppend(self->buf, دلیلونه, self->پیل, self->پای_ټکی, &self->پای_ټکی);
}

    ساكن PyObject *
RangeRepr(PyObject *self_obj)
{
    RangeObject *self = (RangeObject*)self_obj;
    إذا (self->buf->buf == INVALID_بفر_ارزښت)
	أرجع PyString_FromFormat("<range object (for deleted buffer) at %p>",
				    (فراغ *)self);
    وإلا
    {
	حرف *name = (حرف *)self->buf->buf->b_fname;

	إذا (name == عدم)
	    name = "";

	أرجع PyString_FromFormat("<range %s (%d:%d)>",
				    name, (صحيح)self->پیل, (صحيح)self->پای_ټکی);
    }
}

ساكن بنية Py_لاره_Def RangeMethods[] = {
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
    إذا (self->buf && self->buf != INVALID_بفر_ارزښت)
	BUF_PYTHON_REF(self->buf) = عدم;

    DESTRUCTOR_پای(self);
}

    ساكن PyInt
بفر_اوږدتیا(بفر_Object *self)
{
    // HOW DO WE SIGNAL AN ERROR FROM THIS FUNCTION?
    إذا (Check_بفر(self))
	أرجع -1; // ???

    أرجع (PyInt)(self->buf->b_ml.ml_کرښه_شمېر);
}

    ساكن PyObject *
بفر_توکی(بفر_Object *self, PyInt n)
{
    أرجع RBItem(self, n, 1, -1);
}

    ساكن PyObject *
بفر_ټوټه(بفر_Object *self, PyInt lo, PyInt hi)
{
    أرجع RBSlice(self, lo, hi, 1, -1);
}

ساكن حرف *بفر_Attrs[] = {
    "name", "number", "vars", "options", "valid",
    عدم
};

    ساكن PyObject *
بفر_Dir(PyObject *self, PyObject *دلیلونه ناکارول_شوی)
{
    أرجع ObjectDir(self, بفر_Attrs);
}

    ساكن PyObject *
بفر_Attr_Valid(بفر_Object *self, حرف *name)
{
    PyObject	*ret;

    إذا (strcmp(name, "valid") != 0)
	أرجع عدم;

    ret = ((self->buf == INVALID_بفر_ارزښت) ? Py_False : Py_True);
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
	أرجع Py_Build_ارزښت(Py_ssize_t_fmt, self->buf->b_fnum);
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

	Vim_Try_پیل();
	// Using aucmd_*: autocommands will be executed by rename_buffer
	aucmd_prepbuf(&aco, self->buf);
	إذا (curbuf == self->buf)
	{
	    ren_ret = rename_بفر(val);
	    aucmd_restbuf(&aco);
	}
	Py_XDECREF(todecref);
	إذا (Vim_Try_پای_ټکی())
	    أرجع -1;

	إذا (ren_ret == FAIL)
	{
	    Py_Err_سېټ_VIM(N_("failed to rename buffer"));
	    أرجع -1;
	}
	أرجع 0;
    }
    وإلا
    {
	Py_Err_سېټ_String(PyExc_AttributeError, name);
	أرجع -1;
    }
}

    ساكن PyObject *
بفر_Append(بفر_Object *self, PyObject *دلیلونه)
{
    أرجع RBAppend(self, دلیلونه, 1, -1, عدم);
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
	Py_Err_سېټ_STRING(Py_Exc_ارزښت_Error,
		N_("mark name must be a single character"));
	Py_XDECREF(todecref);
	أرجع عدم;
    }

    mark = *pmark;

    Py_XDECREF(todecref);

    Vim_Try_پیل();
    switch_بفر(&savebuf, self->buf);
    posp = getmark(mark, FALSE);
    restore_بفر(&savebuf);
    إذا (Vim_Try_پای_ټکی())
	أرجع عدم;

    إذا (posp == عدم)
    {
	Py_Err_سېټ_VIM(N_("invalid mark name"));
	أرجع عدم;
    }

    إذا (posp->lnum <= 0)
    {
	// Or raise an error?
	Py_INCREF(Py_None);
	أرجع Py_None;
    }

    أرجع Py_Build_ارزښت("(ll)", (اوږد)(posp->lnum), (اوږد)(posp->ستون));
}

    ساكن PyObject *
بفر_Range(بفر_Object *self, PyObject *دلیلونه)
{
    PyInt پیل;
    PyInt پای_ټکی;

    إذا (Check_بفر(self))
	أرجع عدم;

    إذا (!PyArg_ParseTuple(دلیلونه, "nn", &پیل, &پای_ټکی))
	أرجع عدم;

    أرجع RangeNew(self->buf, پیل, پای_ټکی);
}

    ساكن PyObject *
بفر_Repr(PyObject *self_obj)
{
    بفر_Object *self = (بفر_Object*)self_obj;
    إذا (self->buf == INVALID_بفر_ارزښت)
	أرجع PyString_FromFormat("<buffer object (deleted) at %p>", (فراغ *)self);
    وإلا
    {
	حرف	*name = (حرف *)self->buf->b_fname;

	إذا (name == عدم)
	    name = "";

	أرجع PyString_FromFormat("<buffer %s>", name);
    }
}

ساكن بنية Py_لاره_Def بفر_Methods[] = {
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
Buf_Map_اوږدتیا(PyObject *self ناکارول_شوی)
{
    buf_T	*b = firstbuf;
    PyInt	n = 0;

    طالما (b)
    {
	++n;
	b = b->b_بل;
    }

    أرجع n;
}

    ساكن PyObject *
Buf_Map_توکی(PyObject *self ناکارول_شوی, PyObject *کلی_Object)
{
    buf_T	*b;
    اوږد	bnr;

    إذا (شمېره_منزل_Long(کلی_Object, &bnr, شمېره_INT|شمېره_NATURAL))
	أرجع عدم;

    b = buflist_findnr((صحيح) bnr);

    إذا (b)
	أرجع بفر_New(b);
    وإلا
    {
	Py_Err_سېټ_Object(Py_Exc_کلی_Error, کلی_Object);
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
BufMapIterTraverse(فراغ *iter, visitproc لیدنه, فراغ *arg)
{
    PyObject *بفر = (PyObject*)iter;
    إذا (بفر)
	Py_لیدنه(بفر);
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
Buf_Map_Iter_بل(فراغ **arg)
{
    PyObject	*بل;
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

    إذا (!((بفر_Object *)(ret))->buf->b_بل)
	بل = عدم;
    وإلا إذا (!(بل = بفر_New(((بفر_Object *)(ret))->buf->b_بل)))
	أرجع عدم;
    *بفر = بل;
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
	    BufMapIterDestruct, Buf_Map_Iter_بل,
	    BufMapIterTraverse, BufMapIterClear,
	    self);
}

ساكن PyMappingMethods BufMapAsMapping = {
    (lenfunc)       Buf_Map_اوږدتیا,
    (binaryfunc)    Buf_Map_توکی,
    (objobjargproc) 0,
};

// Current items object

ساكن حرف *CurrentAttrs[] = {
    "buffer", "window", "line", "range", "tabpage",
    عدم
};

    ساكن PyObject *
CurrentDir(PyObject *self, PyObject *دلیلونه ناکارول_شوی)
{
    أرجع ObjectDir(self, CurrentAttrs);
}

    ساكن PyObject *
CurrentGetattr(PyObject *self ناکارول_شوی, حرف *name)
{
    إذا (strcmp(name, "buffer") == 0)
	أرجع (PyObject *)بفر_New(curbuf);
    وإلا إذا (strcmp(name, "window") == 0)
	أرجع (PyObject *)کړکۍ_New(curwin, curtab);
    وإلا إذا (strcmp(name, "tabpage") == 0)
	أرجع (PyObject *)Tab_پاڼه_New(curtab);
    وإلا إذا (strcmp(name, "line") == 0)
	أرجع Get_بفر_کرښه(curbuf, (PyInt)curwin->w_cursor.lnum);
    وإلا إذا (strcmp(name, "range") == 0)
	أرجع RangeNew(curbuf, Range_پیل, Range_پای_ټکی);
    وإلا إذا (strcmp(name, "__members__") == 0)
	أرجع ObjectDir(عدم, CurrentAttrs);
    وإلا
#إذا PY_MAJOR_نسخه < 3
	أرجع Py_پیدا_کړه_لاره(کړکۍ_Methods, self, name);
#وإلا
	أرجع عدم;
#د_که_پای
}

    ساكن صحيح
CurrentSetattr(PyObject *self ناکارول_شوی, حرف *name, PyObject *valObject)
{
    إذا (strcmp(name, "line") == 0)
    {
	إذا (سېټ_بفر_کرښه_2(curbuf, (PyInt)curwin->w_cursor.lnum, valObject,
			  عدم) == FAIL)
	    أرجع -1;

	أرجع 0;
    }
    وإلا إذا (strcmp(name, "buffer") == 0)
    {
	صحيح شمېر;

	إذا (valObject->ob_type != بفر_Type_Ptr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.Buffer object, but got %s"),
		    valObject);
	    أرجع -1;
	}

	إذا (Check_بفر((بفر_Object *)(valObject)))
	    أرجع -1;
	شمېر = ((بفر_Object *)(valObject))->buf->b_fnum;

	Vim_Try_پیل();
	إذا (do_بفر(DOBUF_GOTO, DOBUF_لومړی, FORWARD, شمېر, 0) == FAIL)
	{
	    إذا (Vim_Try_پای_ټکی())
		أرجع -1;
	    PyErr_VIM_FORMAT(N_("failed to switch to buffer %d"), شمېر);
	    أرجع -1;
	}

	أرجع Vim_Try_پای_ټکی();
    }
    وإلا إذا (strcmp(name, "window") == 0)
    {
	صحيح شمېر;

	إذا (valObject->ob_type != کړکۍ_Type_Ptr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.Window object, but got %s"),
		    valObject);
	    أرجع -1;
	}

	إذا (Check_کړکۍ((کړکۍ_Object *)(valObject)))
	    أرجع -1;
	شمېر = get_win_شمېره(((کړکۍ_Object *)(valObject))->win, firstwin);

	إذا (!شمېر)
	{
	    Py_Err_سېټ_STRING(Py_Exc_ارزښت_Error,
		    N_("failed to find window in the current tab page"));
	    أرجع -1;
	}

	Vim_Try_پیل();
	win_goto(((کړکۍ_Object *)(valObject))->win);
	إذا (((کړکۍ_Object *)(valObject))->win != curwin)
	{
	    إذا (Vim_Try_پای_ټکی())
		أرجع -1;
	    Py_Err_سېټ_STRING(PyExc_RuntimeError,
		    N_("did not switch to the specified window"));
	    أرجع -1;
	}

	أرجع Vim_Try_پای_ټکی();
    }
    وإلا إذا (strcmp(name, "tabpage") == 0)
    {
	إذا (valObject->ob_type != Tab_پاڼه_Type_Ptr)
	{
	    PyErr_FORMAT_TYPE(
		    N_("expected vim.TabPage object, but got %s"),
		    valObject);
	    أرجع -1;
	}

	إذا (Check_Tab_پاڼه((Tab_پاڼه_Object *)(valObject)))
	    أرجع -1;

	Vim_Try_پیل();
	goto_tabpage_tp(((Tab_پاڼه_Object *)(valObject))->tab, TRUE, TRUE);
	إذا (((Tab_پاڼه_Object *)(valObject))->tab != curtab)
	{
	    إذا (Vim_Try_پای_ټکی())
		أرجع -1;
	    Py_Err_سېټ_STRING(PyExc_RuntimeError,
		    N_("did not switch to the specified tab page"));
	    أرجع -1;
	}

	أرجع Vim_Try_پای_ټکی();
    }
    وإلا
    {
	Py_Err_سېټ_String(PyExc_AttributeError, name);
	أرجع -1;
    }
}

ساكن بنية Py_لاره_Def CurrentMethods[] = {
    // name,	    function,			calling,	documentation
    {"__dir__",	    (PyCFunction)CurrentDir,	METH_NOARGS,	""},
    { عدم,	    عدم,			0,		عدم}
};

    ساكن فراغ
init_range_cmd(فراغ *arg)
{
    exarg_T *eap = (exarg_T*)arg;
    Range_پیل = eap->line1;
    Range_پای_ټکی = eap->line2;
}

    ساكن فراغ
init_range_eval(فراغ *rettv ناکارول_شوی)
{
    Range_پیل = (PyInt) curwin->w_cursor.lnum;
    Range_پای_ټکی = Range_پیل;
}

    ساكن فراغ
پرلپسې_cmd(ثابت حرف *cmd, dict_T* locals ناکارول_شوی, فراغ *arg ناکارول_شوی
#که_تعریف_شوی PY_CAN_RECURSE
	, PyGILState_STATE *pygilstate ناکارول_شوی
#د_که_پای
	)
{
    PyObject	*پرلپسې_ret;
    پرلپسې_ret = Py_پرلپسې_String((حرف *)cmd, Py_file_ننوت, globals, globals);
    إذا (پرلپسې_ret != عدم)
    {
	Py_DECREF(پرلپسې_ret);
    }
    وإلا إذا (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
    {
	emsg(_(e_cant_handle_systemexit_د_python_exception_دننه_vim));
	PyErr_Clear();
    }
    وإلا
	PyErr_PrintEx(1);
}

ساكن ثابت حرف	*کوډ_hdr = "def " DOPY_FUNC "(line, linenr):\n ";
ساكن صحيح		کوډ_hdr_len = 30;

    ساكن فراغ
پرلپسې_do(ثابت حرف *cmd, dict_T* locals ناکارول_شوی, فراغ *arg ناکارول_شوی
#که_تعریف_شوی PY_CAN_RECURSE
	, PyGILState_STATE *pygilstate
#د_که_پای
	)
{
    PyInt	lnum;
    size_t	len;
    حرف	*کوډ;
    صحيح		حالت;
    PyObject	*pyfunc, *pymain;
    PyObject	*پرلپسې_ret;
    buf_T	*was_curbuf = curbuf;

    إذا (u_save((linenr_T)Range_پیل - 1, (linenr_T)Range_پای_ټکی + 1) != OK)
    {
	emsg(_("cannot save undo information"));
	أرجع;
    }

    len = کوډ_hdr_len + STRLEN(cmd);
    کوډ = PyMem_New(حرف, len + 1);
    حافظه_نقل(کوډ, کوډ_hdr, کوډ_hdr_len);
    STRCPY(کوډ + کوډ_hdr_len, cmd);
    پرلپسې_ret = Py_پرلپسې_String(کوډ, Py_file_ننوت, globals, globals);
    حالت = -1;
    إذا (پرلپسې_ret != عدم)
    {
	حالت = 0;
	Py_DECREF(پرلپسې_ret);
    }
    وإلا إذا (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
    {
	PyMem_Free(کوډ);
	emsg(_(e_cant_handle_systemexit_د_python_exception_دننه_vim));
	PyErr_Clear();
	أرجع;
    }
    وإلا
	PyErr_PrintEx(1);

    PyMem_Free(کوډ);

    إذا (حالت)
    {
	emsg(_("failed to run the code"));
	أرجع;
    }

    حالت = 0;
    pymain = Py_Import_جمع_کړه_Module("__main__");
    pyfunc = PyObject_GetAttrString(pymain, DOPY_FUNC);
#که_تعریف_شوی PY_CAN_RECURSE
    PyGILState_Release(*pygilstate);
#د_که_پای

    لكل (lnum = Range_پیل; lnum <= Range_پای_ټکی; ++lnum)
    {
	PyObject	*کرښه;
	PyObject	*linenr;
	PyObject	*ret;

#که_تعریف_شوی PY_CAN_RECURSE
	*pygilstate = PyGILState_Ensure();
#د_که_پای
	// Check the line number, the command my have deleted lines.
	إذا (lnum > curbuf->b_ml.ml_کرښه_شمېر
		|| !(کرښه = Get_بفر_کرښه(curbuf, lnum)))
	    اذهب err;
	إذا (!(linenr = PyInt_FromLong((اوږد) lnum)))
	{
	    Py_DECREF(کرښه);
	    اذهب err;
	}
	ret = Py_Object_Call_Function_Obj_دلیلونه(pyfunc, کرښه, linenr, عدم);
	Py_DECREF(کرښه);
	Py_DECREF(linenr);
	إذا (!ret)
	    اذهب err;

	// Check that the command didn't switch to another buffer.
	// Check the line number, the command my have deleted lines.
	إذا (curbuf != was_curbuf || lnum > curbuf->b_ml.ml_کرښه_شمېر)
	{
	    Py_XDECREF(ret);
	    اذهب err;
	}

	إذا (ret != Py_None)
	    إذا (سېټ_بفر_کرښه_2(curbuf, lnum, ret, عدم) == FAIL)
	    {
		Py_XDECREF(ret);
		اذهب err;
	    }

	Py_XDECREF(ret);
	PythonIO_Flush();
#که_تعریف_شوی PY_CAN_RECURSE
	PyGILState_Release(*pygilstate);
#د_که_پای
    }
    اذهب بهر;
err:
#که_تعریف_شوی PY_CAN_RECURSE
    *pygilstate = PyGILState_Ensure();
#د_که_پای
    PyErr_PrintEx(0);
    PythonIO_Flush();
    حالت = 1;
بهر:
#که_تعریف_شوی PY_CAN_RECURSE
    إذا (!حالت)
	*pygilstate = PyGILState_Ensure();
#د_که_پای
    Py_DECREF(pyfunc);
    Py_Object_سېټ_Attr_String(pymain, DOPY_FUNC, عدم);
    إذا (حالت)
	أرجع;
    check_cursor();
    update_curbuf(UPD_NOT_VALID);
}

    ساكن فراغ
پرلپسې_eval(ثابت حرف *cmd, dict_T *locals, فراغ *arg
#که_تعریف_شوی PY_CAN_RECURSE
	, PyGILState_STATE *pygilstate ناکارول_شوی
#د_که_پای
	)
{
    PyObject	*پرلپسې_ret;
    typval_T	*rettv = (typval_T*)arg;
    PyObject	*pylocals = locals ? NEW_DICTIONARY(locals) : globals;

    پرلپسې_ret = Py_پرلپسې_String((حرف *)cmd, Py_eval_ننوت, globals, pylocals);
    إذا (پرلپسې_ret == عدم)
    {
	إذا (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SystemExit))
	{
	    emsg(_(e_cant_handle_systemexit_د_python_exception_دننه_vim));
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
	إذا (ConvertFromPyObject(پرلپسې_ret, rettv) == -1)
	    emsg(_(e_failed_منزل_convert_returned_python_object_منزل_vim_ارزښت));
	Py_DECREF(پرلپسې_ret);
    }
    PyErr_Clear();
}

    ساكن صحيح
سېټ_ref_دننه_py(ثابت صحيح copyID)
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
	    abort = سېټ_ref_دننه_dict(((DictionaryObject *)(cur->pll_obj))->dict,
								       copyID);
    }

    إذا (lastlist != عدم)
    {
	لكل (cur = lastlist ; !abort && cur != عدم ; cur = cur->pll_prev)
	{
	    ll = ((ListObject *) (cur->pll_obj))->list;
	    abort = سېټ_ref_دننه_list(ll, copyID);
	}
    }

    إذا (lasttuple != عدم)
    {
	لكل (cur = lasttuple ; !abort && cur != عدم ; cur = cur->pll_prev)
	{
	    tt = ((TupleObject *) (cur->pll_obj))->tuple;
	    abort = سېټ_ref_دننه_tuple(tt, copyID);
	}
    }

    إذا (lastfunc != عدم)
    {
	لكل (cur = lastfunc ; !abort && cur != عدم ; cur = cur->pll_prev)
	{
	    func = (FunctionObject *) cur->pll_obj;
	    abort = سېټ_ref_دننه_dict(func->self, copyID);
	    إذا (func->argc)
		لكل (i = 0; !abort && i < func->argc; ++i)
		    abort = abort
			|| سېټ_ref_دننه_توکی(&func->argv[i], copyID, عدم, عدم, عدم);
	}
    }

    أرجع abort;
}

    ساكن صحيح
سېټ_string_copy(char_u *str, typval_T *tv)
{
    tv->vval.v_string = vim_strsave(str);
    إذا (tv->vval.v_string == عدم)
    {
	Py_Err_No_حافظه();
	أرجع -1;
    }
    أرجع 0;
}

    ساكن صحيح
pydict_منزل_tv(PyObject *obj, typval_T *tv, PyObject *lookup_dict)
{
    dict_T	*dict;
    char_u	*کلی;
    dictitem_T	*di;
    PyObject	*کلی_Object;
    PyObject	*valObject;
    Py_ssize_t	iter = 0;

    إذا (!(dict = py_dict_alloc()))
	أرجع -1;

    tv->v_type = VAR_DICT;
    tv->vval.v_dict = dict;

    طالما (Py_Dict_بل(obj, &iter, &کلی_Object, &valObject))
    {
	PyObject	*todecref = عدم;

	إذا (کلی_Object == عدم || valObject == عدم)
	{
	    dict_unref(dict);
	    أرجع -1;
	}

	إذا (!(کلی = String_منزل_Chars(کلی_Object, &todecref)))
	{
	    dict_unref(dict);
	    أرجع -1;
	}

	إذا (*کلی == NUL)
	{
	    dict_unref(dict);
	    Py_XDECREF(todecref);
	    RAISE_NO_تشه_کلیګانې;
	    أرجع -1;
	}

	di = dictitem_alloc(کلی);

	Py_XDECREF(todecref);

	إذا (di == عدم)
	{
	    Py_Err_No_حافظه();
	    dict_unref(dict);
	    أرجع -1;
	}

	إذا (_ConvertFromPyObject(valObject, &di->di_tv, lookup_dict) == -1)
	{
	    vim_free(di);
	    dict_unref(dict);
	    أرجع -1;
	}

	إذا (dict_جمع_کړه(dict, di) == FAIL)
	{
	    RAISE_کلی_جمع_کړه_FAIL(di->di_کلی);
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
    char_u	*کلی;
    dictitem_T	*di;
    PyObject	*list;
    PyObject	*iterator;
    PyObject	*کلی_Object;
    PyObject	*valObject;

    إذا (!(dict = py_dict_alloc()))
	أرجع -1;

    tv->v_type = VAR_DICT;
    tv->vval.v_dict = dict;

    إذا (!(list = Py_Mapping_کلیګانې(obj)))
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

    طالما ((کلی_Object = Py_Iter_بل(iterator)))
    {
	PyObject	*todecref;

	إذا (!(کلی = String_منزل_Chars(کلی_Object, &todecref)))
	{
	    Py_DECREF(کلی_Object);
	    Py_DECREF(iterator);
	    dict_unref(dict);
	    أرجع -1;
	}

	إذا (*کلی == NUL)
	{
	    Py_DECREF(کلی_Object);
	    Py_DECREF(iterator);
	    Py_XDECREF(todecref);
	    dict_unref(dict);
	    RAISE_NO_تشه_کلیګانې;
	    أرجع -1;
	}

	إذا (!(valObject = Py_Object_Get_توکی(obj, کلی_Object)))
	{
	    Py_DECREF(کلی_Object);
	    Py_DECREF(iterator);
	    Py_XDECREF(todecref);
	    dict_unref(dict);
	    أرجع -1;
	}

	di = dictitem_alloc(کلی);

	Py_DECREF(کلی_Object);
	Py_XDECREF(todecref);

	إذا (di == عدم)
	{
	    Py_DECREF(iterator);
	    Py_DECREF(valObject);
	    dict_unref(dict);
	    Py_Err_No_حافظه();
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

	إذا (dict_جمع_کړه(dict, di) == FAIL)
	{
	    RAISE_کلی_جمع_کړه_FAIL(di->di_کلی);
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

#که_تعریف_شوی PY_USE_CAPSULE
    capsule = Py_Dict_Get_توکی_String(lookup_dict, hexBuf);
#وإلا
    capsule = (PyObject *)Py_Dict_Get_توکی_String(lookup_dict, hexBuf);
#د_که_پای
    إذا (capsule == عدم)
    {
#که_تعریف_شوی PY_USE_CAPSULE
	capsule = PyCapsule_New(tv, عدم, عدم);
#وإلا
	capsule = PyCObject_FromVoidPtr(tv, عدم);
#د_که_پای
	إذا (Py_Dict_سېټ_توکی_String(lookup_dict, hexBuf, capsule))
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

#که_تعریف_شوی PY_USE_CAPSULE
	v = Py_Capsule_Get_اشاره_ګر(capsule, عدم);
#وإلا
	v = PyCObject_AsVoidPtr(capsule);
#د_که_پای
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

	    سېټ_partial(func, pt, TRUE);
	    tv->vval.v_partial = pt;
	    tv->v_type = VAR_PARTIAL;
	}
	وإلا
	{
	    إذا (سېټ_string_copy(func->name, tv) == -1)
		أرجع -1;

	    tv->v_type = VAR_FUNC;
	}
	func_ref(func->name);
    }
    وإلا إذا (Py_بایټونه_Check(obj))
    {
	char_u	*str;

	إذا (Py_بایټونه_As_String_And_اندازه(obj, (حرف **) &str, عدم) == -1)
	    أرجع -1;
	إذا (str == عدم)
	    أرجع -1;

	إذا (سېټ_string_copy(str, tv) == -1)
	    أرجع -1;

	tv->v_type = VAR_STRING;
    }
    وإلا إذا (PyUnicode_Check(obj))
    {
	PyObject	*بایټونه;
	char_u	*str;

	بایټونه = PyUnicode_AsEncodedString(obj, ENC_OPT, ERRORS_ENCODE_ARG);
	إذا (بایټونه == عدم)
	    أرجع -1;

	إذا (Py_بایټونه_As_String_And_اندازه(بایټونه, (حرف **) &str, عدم) == -1)
	    أرجع -1;
	إذا (str == عدم)
	    أرجع -1;

	إذا (سېټ_string_copy(str, tv))
	{
	    Py_XDECREF(بایټونه);
	    أرجع -1;
	}
	Py_XDECREF(بایټونه);

	tv->v_type = VAR_STRING;
    }
#إذا PY_MAJOR_نسخه < 3
    وإلا إذا (PyInt_Check(obj))
    {
	tv->v_type = VAR_شمېره;
	tv->vval.v_شمېره = (varnumber_T) PyInt_AsLong(obj);
	إذا (PyErr_Occurred())
	    أرجع -1;
    }
#د_که_پای
    وإلا إذا (PyLong_Check(obj))
    {
	tv->v_type = VAR_شمېره;
	tv->vval.v_شمېره = (varnumber_T) PyLong_AsLong(obj);
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
    وإلا إذا (Py_شمېره_Check(obj))
    {
	PyObject	*عدد;

	إذا (!(عدد = Py_شمېره_Long(obj)))
	    أرجع -1;

	tv->v_type = VAR_شمېره;
	tv->vval.v_شمېره = (varnumber_T) PyLong_AsLong(عدد);

	Py_DECREF(عدد);
    }
    وإلا إذا (obj == Py_None)
    {
	tv->v_type = VAR_SPECIAL;
	tv->vval.v_شمېره = VVAL_NONE;
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
	Py_Err_سېټ_VIM(N_("internal error: NULL reference passed"));
	أرجع عدم;
    }
    بدل (tv->v_type)
    {
	حالة VAR_STRING:
	    أرجع Py_بایټونه_From_String(tv->vval.v_string == عدم
					    ? "" : (حرف *)tv->vval.v_string);
	حالة VAR_شمېره:
	    أرجع PyLong_FromLong((اوږد) tv->vval.v_شمېره);
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
	    أرجع Py_بایټونه_From_String_And_اندازه(
		(حرف*) tv->vval.v_blob->bv_ga.ga_معلومات,
		(Py_ssize_t) tv->vval.v_blob->bv_ga.ga_len);
	حالة VAR_UNKNOWN:
	حالة VAR_ANY:
	حالة VAR_VOID:
	حالة VAR_CHANNEL:
	حالة VAR_دنده:
	حالة VAR_INSTR:
	حالة VAR_CLASS:
	حالة VAR_OBJECT:
	حالة VAR_TYPEALIAS:
	    Py_INCREF(Py_None);
	    أرجع Py_None;
	حالة VAR_BOOL:
	حالة VAR_SPECIAL:
	    بدل (tv->vval.v_شمېره)
	    {
		حالة VVAL_FALSE: أرجع ALWAYS_FALSE;
		حالة VVAL_TRUE:  أرجع ALWAYS_TRUE;
		حالة VVAL_NONE:
		حالة VVAL_NULL:  أرجع ALWAYS_NONE;
	    }
	    Py_Err_سېټ_VIM(N_("internal error: invalid value type"));
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
    CLEAR_ډګر(OutputType);
    OutputType.tp_name = "vim.message";
    OutputType.tp_basicsize = حجم(OutputObject);
    OutputType.tp_flags = Py_TPFLAGS_DEFAULT;
    OutputType.tp_doc = "vim message object";
    OutputType.tp_methods = OutputMethods;
#إذا PY_MAJOR_نسخه >= 3
    OutputType.tp_getattro = OutputGetattro;
    OutputType.tp_setattro = OutputSetattro;
    OutputType.tp_alloc = call_PyType_GenericAlloc;
    OutputType.tp_new = call_PyType_GenericNew;
    OutputType.tp_free = call_PyObject_Free;
# ifndef USE_LIMITED_API
    // The std printer type is only exposed in full API. It is not essential
    // anyway and so in limited API we don't set it.
    OutputType.tp_بنسټ = &PyStdPrinter_Type;
# endif
#وإلا
    OutputType.tp_getattr = OutputGetattr;
    OutputType.tp_setattr = OutputSetattr;
    // Disabled, because this causes a crash in test86
    // OutputType.tp_base = &PyFile_Type;
#د_که_پای

    CLEAR_ډګر(IterType);
    IterType.tp_name = "vim.iter";
    IterType.tp_basicsize = حجم(IterObject);
    IterType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    IterType.tp_doc = "generic iterator object";
    IterType.tp_iter = IterIter;
    IterType.tp_iternext = Iter_بل;
    IterType.tp_dealloc = IterDestructor;
    IterType.tp_traverse = IterTraverse;
    IterType.tp_clear = IterClear;

    CLEAR_ډګر(بفر_Type);
    بفر_Type.tp_name = "vim.buffer";
    بفر_Type.tp_basicsize = حجم(بفر_Object);
    بفر_Type.tp_dealloc = بفر_Destructor;
    بفر_Type.tp_repr = بفر_Repr;
    بفر_Type.tp_as_sequence = &بفر_As_Seq;
    بفر_Type.tp_as_mapping = &بفر_As_Mapping;
    بفر_Type.tp_flags = Py_TPFLAGS_DEFAULT;
    بفر_Type.tp_doc = "vim buffer object";
    بفر_Type.tp_methods = بفر_Methods;
#إذا PY_MAJOR_نسخه >= 3
    بفر_Type.tp_getattro = بفر_Getattro;
    بفر_Type.tp_setattro = بفر_Setattro;
    بفر_Type.tp_alloc = call_PyType_GenericAlloc;
    بفر_Type.tp_new = call_PyType_GenericNew;
    بفر_Type.tp_free = call_PyObject_Free;
#وإلا
    بفر_Type.tp_getattr = بفر_Getattr;
    بفر_Type.tp_setattr = بفر_Setattr;
#د_که_پای

    CLEAR_ډګر(کړکۍ_Type);
    کړکۍ_Type.tp_name = "vim.window";
    کړکۍ_Type.tp_basicsize = حجم(کړکۍ_Object);
    کړکۍ_Type.tp_dealloc = کړکۍ_Destructor;
    کړکۍ_Type.tp_repr = کړکۍ_Repr;
    کړکۍ_Type.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC;
    کړکۍ_Type.tp_doc = "vim Window object";
    کړکۍ_Type.tp_methods = کړکۍ_Methods;
    کړکۍ_Type.tp_traverse = کړکۍ_Traverse;
    کړکۍ_Type.tp_clear = کړکۍ_Clear;
#إذا PY_MAJOR_نسخه >= 3
    کړکۍ_Type.tp_getattro = کړکۍ_Getattro;
    کړکۍ_Type.tp_setattro = کړکۍ_Setattro;
    کړکۍ_Type.tp_alloc = call_PyType_GenericAlloc;
    کړکۍ_Type.tp_new = call_PyType_GenericNew;
    کړکۍ_Type.tp_free = call_PyObject_Free;
#وإلا
    کړکۍ_Type.tp_getattr = کړکۍ_Getattr;
    کړکۍ_Type.tp_setattr = کړکۍ_Setattr;
#د_که_پای

    CLEAR_ډګر(Tab_پاڼه_Type);
    Tab_پاڼه_Type.tp_name = "vim.tabpage";
    Tab_پاڼه_Type.tp_basicsize = حجم(Tab_پاڼه_Object);
    Tab_پاڼه_Type.tp_dealloc = Tab_پاڼه_Destructor;
    Tab_پاڼه_Type.tp_repr = Tab_پاڼه_Repr;
    Tab_پاڼه_Type.tp_flags = Py_TPFLAGS_DEFAULT;
    Tab_پاڼه_Type.tp_doc = "vim tab page object";
    Tab_پاڼه_Type.tp_methods = Tab_پاڼه_Methods;
#إذا PY_MAJOR_نسخه >= 3
    Tab_پاڼه_Type.tp_getattro = Tab_پاڼه_Getattro;
    Tab_پاڼه_Type.tp_alloc = call_PyType_GenericAlloc;
    Tab_پاڼه_Type.tp_new = call_PyType_GenericNew;
    Tab_پاڼه_Type.tp_free = call_PyObject_Free;
#وإلا
    Tab_پاڼه_Type.tp_getattr = Tab_پاڼه_Getattr;
#د_که_پای

    CLEAR_ډګر(BufMapType);
    BufMapType.tp_name = "vim.bufferlist";
    BufMapType.tp_basicsize = حجم(BufMapObject);
    BufMapType.tp_as_mapping = &BufMapAsMapping;
    BufMapType.tp_flags = Py_TPFLAGS_DEFAULT;
    BufMapType.tp_iter = BufMapIter;
    BufMapType.tp_doc = "vim buffer list";

    CLEAR_ډګر(WinListType);
    WinListType.tp_name = "vim.windowlist";
    WinListType.tp_basicsize = حجم(WinListObject);
    WinListType.tp_as_sequence = &WinListAsSeq;
    WinListType.tp_flags = Py_TPFLAGS_DEFAULT;
    WinListType.tp_doc = "vim window list";
    WinListType.tp_dealloc = (destructor)WinListDestructor;

    CLEAR_ډګر(TabListType);
    TabListType.tp_name = "vim.tabpagelist";
    TabListType.tp_basicsize = حجم(TabListObject);
    TabListType.tp_as_sequence = &TabListAsSeq;
    TabListType.tp_flags = Py_TPFLAGS_DEFAULT;
    TabListType.tp_doc = "vim tab page list";

    CLEAR_ډګر(RangeType);
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
#إذا PY_MAJOR_نسخه >= 3
    RangeType.tp_getattro = RangeGetattro;
    RangeType.tp_alloc = call_PyType_GenericAlloc;
    RangeType.tp_new = call_PyType_GenericNew;
    RangeType.tp_free = call_PyObject_Free;
#وإلا
    RangeType.tp_getattr = RangeGetattr;
#د_که_پای

    CLEAR_ډګر(CurrentType);
    CurrentType.tp_name = "vim.currentdata";
    CurrentType.tp_basicsize = حجم(CurrentObject);
    CurrentType.tp_flags = Py_TPFLAGS_DEFAULT;
    CurrentType.tp_doc = "vim current object";
    CurrentType.tp_methods = CurrentMethods;
#إذا PY_MAJOR_نسخه >= 3
    CurrentType.tp_getattro = CurrentGetattro;
    CurrentType.tp_setattro = CurrentSetattro;
#وإلا
    CurrentType.tp_getattr = CurrentGetattr;
    CurrentType.tp_setattr = CurrentSetattr;
#د_که_پای

    CLEAR_ډګر(DictionaryType);
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
#إذا PY_MAJOR_نسخه >= 3
    DictionaryType.tp_getattro = DictionaryGetattro;
    DictionaryType.tp_setattro = DictionarySetattro;
#وإلا
    DictionaryType.tp_getattr = DictionaryGetattr;
    DictionaryType.tp_setattr = DictionarySetattr;
#د_که_پای

    CLEAR_ډګر(ListType);
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
#إذا PY_MAJOR_نسخه >= 3
    ListType.tp_getattro = ListGetattro;
    ListType.tp_setattro = ListSetattro;
#وإلا
    ListType.tp_getattr = ListGetattr;
    ListType.tp_setattr = ListSetattr;
#د_که_پای

    // Tuple type
    CLEAR_ډګر(TupleType);
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
#إذا PY_MAJOR_نسخه >= 3
    TupleType.tp_getattro = TupleGetattro;
    TupleType.tp_setattro = TupleSetattro;
#وإلا
    TupleType.tp_getattr = TupleGetattr;
    TupleType.tp_setattr = TupleSetattr;
#د_که_پای

    CLEAR_ډګر(FunctionType);
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
#إذا PY_MAJOR_نسخه >= 3
    FunctionType.tp_getattro = FunctionGetattro;
#وإلا
    FunctionType.tp_getattr = FunctionGetattr;
#د_که_پای

    CLEAR_ډګر(OptionsType);
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

#إذا PY_نسخه_HEX < 0x030700f0
    CLEAR_ډګر(LoaderType);
    LoaderType.tp_name = "vim.Loader";
    LoaderType.tp_basicsize = حجم(LoaderObject);
    LoaderType.tp_flags = Py_TPFLAGS_DEFAULT;
    LoaderType.tp_doc = "vim message object";
    LoaderType.tp_methods = LoaderMethods;
    LoaderType.tp_dealloc = LoaderDestructor;
#د_که_پای

#إذا PY_MAJOR_نسخه >= 3
    CLEAR_ډګر(vimmodule);
    vimmodule.m_name = "vim";
    vimmodule.m_doc = "Vim Python interface\n";
    vimmodule.m_اندازه_2 = -1;
    vimmodule.m_methods = VimMethods;
#د_که_پای
}

    ساكن صحيح
init_types(فراغ)
{
    PYTYPE_READY(IterType);
    PYTYPE_READY(بفر_Type);
    PYTYPE_READY(RangeType);
    PYTYPE_READY(کړکۍ_Type);
    PYTYPE_READY(Tab_پاڼه_Type);
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
#إذا PY_نسخه_HEX < 0x030700f0
    PYTYPE_READY(LoaderType);
#د_که_پای

#که_تعریف_شوی USE_LIMITED_API
    // We need to finish initializing all the static objects because the types
    // are only just allocated on the heap now.
    // Each PyObject_HEAD_INIT_TYPE should correspond to a
    // PyObject_FINISH_INIT_TYPE below.
    Py_Object_پای_INIT_TYPE(Output, OutputType);
    Py_Object_پای_INIT_TYPE(Error, OutputType);
    Py_Object_پای_INIT_TYPE(The_بفر_Map, BufMapType);
    Py_Object_پای_INIT_TYPE(The_کړکۍ_List, WinListType);
    Py_Object_پای_INIT_TYPE(TheCurrent, CurrentType);
    Py_Object_پای_INIT_TYPE(The_Tab_پاڼه_List, TabListType);
#د_که_پای
    أرجع 0;
}

#که_تعریف_شوی USE_LIMITED_API
    ساكن فراغ
shutdown_types(فراغ)
{
    PYTYPE_CLEANUP(IterType);
    PYTYPE_CLEANUP(بفر_Type);
    PYTYPE_CLEANUP(RangeType);
    PYTYPE_CLEANUP(کړکۍ_Type);
    PYTYPE_CLEANUP(Tab_پاڼه_Type);
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
# إذا PY_نسخه_HEX < 0x030700f0
    PYTYPE_CLEANUP(LoaderType);
# endif
}
#د_که_پای

    ساكن صحيح
init_sys_لار(فراغ)
{
    PyObject	*لار;
    PyObject	*لار_hook;
    PyObject	*لار_hooks;

    إذا (!(لار_hook = PyObject_GetAttrString(vim_module, "path_hook")))
	أرجع -1;

    إذا (!(لار_hooks = PySys_GetObject("path_hooks")))
    {
	PyErr_Clear();
	لار_hooks = PyList_New(1);
	Py_List_سېټ_توکی(لار_hooks, 0, لار_hook);
	إذا (Py_Sys_سېټ_Object("path_hooks", لار_hooks))
	{
	    Py_DECREF(لار_hooks);
	    أرجع -1;
	}
	Py_DECREF(لار_hooks);
    }
    وإلا إذا (PyList_Check(لار_hooks))
    {
	إذا (PyList_Append(لار_hooks, لار_hook))
	{
	    Py_DECREF(لار_hook);
	    أرجع -1;
	}
	Py_DECREF(لار_hook);
    }
    وإلا
    {
	Vim_Try_پیل();
	emsg(_("Failed to set path hook: sys.path_hooks is not a list\n"
	       "You should now do the following:\n"
	       "- append vim.path_hook to sys.path_hooks\n"
	       "- append vim.VIM_SPECIAL_PATH to sys.path\n"));
	Vim_Try_پای_ټکی(); // Discard the error
	Py_DECREF(لار_hook);
	أرجع 0;
    }

    إذا (!(لار = PySys_GetObject("path")))
    {
	PyErr_Clear();
	لار = PyList_New(1);
	Py_INCREF(vim_special_لار_object);
	Py_List_سېټ_توکی(لار, 0, vim_special_لار_object);
	إذا (Py_Sys_سېټ_Object("path", لار))
	{
	    Py_DECREF(لار);
	    أرجع -1;
	}
	Py_DECREF(لار);
    }
    وإلا إذا (PyList_Check(لار))
    {
	إذا (PyList_Append(لار, vim_special_لار_object))
	    أرجع -1;
    }
    وإلا
    {
	Vim_Try_پیل();
	emsg(_("Failed to set path: sys.path is not a list\n"
	       "You should now append vim.VIM_SPECIAL_PATH to sys.path"));
	Vim_Try_پای_ټکی(); // Discard the error
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

#تعریف_کړه جمع_کړه_OBJECT(m, name, obj) \
    إذا (Py_Module_جمع_کړه_Object(m, name, obj)) \
	أرجع -1;

#تعریف_کړه جمع_کړه_CHECKED_OBJECT(m, name, obj) \
    { \
	PyObject	*valObject = obj; \
	إذا (!valObject) \
	    أرجع -1; \
	جمع_کړه_OBJECT(m, name, valObject); \
    }

    ساكن صحيح
populate_module(PyObject *m)
{
    صحيح		i;
    PyObject	*other_module;
    PyObject	*attr;
    PyObject	*imp;
#إذا PY_نسخه_HEX >= 0x030700f0
    PyObject	*dict;
    PyObject	*cls;
#د_که_پای

    لكل (i = 0; i < (صحيح)(حجم(numeric_constants)
					   / حجم(بنية numeric_constant));
	    ++i)
	جمع_کړه_CHECKED_OBJECT(m, numeric_constants[i].name,
		PyInt_FromLong(numeric_constants[i].val));

    بنية object_constant object_constants[] = {
	{"buffers",  (PyObject *)(فراغ *)&The_بفر_Map},
	{"windows",  (PyObject *)(فراغ *)&The_کړکۍ_List},
	{"tabpages", (PyObject *)(فراغ *)&The_Tab_پاڼه_List},
	{"current",  (PyObject *)(فراغ *)&TheCurrent},

	{"Buffer",     (PyObject *)بفر_Type_Ptr},
	{"Range",      (PyObject *)RangeTypePtr},
	{"Window",     (PyObject *)کړکۍ_Type_Ptr},
	{"TabPage",    (PyObject *)Tab_پاڼه_Type_Ptr},
	{"Dictionary", (PyObject *)DictionaryTypePtr},
	{"List",       (PyObject *)ListTypePtr},
	{"Tuple",      (PyObject *)TupleTypePtr},
	{"Function",   (PyObject *)FunctionTypePtr},
	{"Options",    (PyObject *)OptionsTypePtr},
#إذا PY_نسخه_HEX < 0x030700f0
	{"_Loader",    (PyObject *)LoaderTypePtr},
#د_که_پای
    };

    لكل (i = 0; i < (صحيح)(حجم(object_constants)
					    / حجم(بنية object_constant));
	    ++i)
    {
	PyObject	*valObject;

	valObject = object_constants[i].valObject;
	Py_INCREF(valObject);
	جمع_کړه_OBJECT(m, object_constants[i].name, valObject);
    }

    إذا (!(VimError = PyErr_NewException("vim.error", عدم, عدم)))
	أرجع -1;
    جمع_کړه_OBJECT(m, "error", VimError);

    جمع_کړه_CHECKED_OBJECT(m, "vars",  NEW_DICTIONARY(get_globvar_dict()));
    جمع_کړه_CHECKED_OBJECT(m, "vvars", NEW_DICTIONARY(get_vimvar_dict()));
    جمع_کړه_CHECKED_OBJECT(m, "options",
	    OptionsNew(SREQ_GLOBAL, عدم, dummy_check, عدم));

    إذا (!(other_module = PyImport_ImportModule("os")))
	أرجع -1;
    جمع_کړه_OBJECT(m, "os", other_module);

#إذا PY_MAJOR_نسخه >= 3
    إذا (!(py_getcwd = PyObject_GetAttrString(other_module, "getcwd")))
	أرجع -1;
#وإلا
    إذا (!(py_getcwd = PyObject_GetAttrString(other_module, "getcwdu")))
	أرجع -1;
#د_که_پای
    جمع_کړه_OBJECT(m, "_getcwd", py_getcwd)

    إذا (!(py_chdir = PyObject_GetAttrString(other_module, "chdir")))
	أرجع -1;
    جمع_کړه_OBJECT(m, "_chdir", py_chdir);
    إذا (!(attr = PyObject_GetAttrString(m, "chdir")))
	أرجع -1;
    إذا (Py_Object_سېټ_Attr_String(other_module, "chdir", attr))
    {
	Py_DECREF(attr);
	أرجع -1;
    }
    Py_DECREF(attr);

    إذا ((py_fchdir = PyObject_GetAttrString(other_module, "fchdir")))
    {
	جمع_کړه_OBJECT(m, "_fchdir", py_fchdir);
	إذا (!(attr = PyObject_GetAttrString(m, "fchdir")))
	    أرجع -1;
	إذا (Py_Object_سېټ_Attr_String(other_module, "fchdir", attr))
	{
	    Py_DECREF(attr);
	    أرجع -1;
	}
	Py_DECREF(attr);
    }
    وإلا
	PyErr_Clear();

    إذا (!(vim_special_لار_object = PyString_FromString(vim_special_لار)))
	أرجع -1;

    جمع_کړه_OBJECT(m, "VIM_SPECIAL_PATH", vim_special_لار_object);

#إذا PY_نسخه_HEX >= 0x030700f0
    إذا (!(imp = PyImport_ImportModule("importlib.machinery")))
	أرجع -1;

    dict = PyModule_GetDict(imp);

    إذا (!(cls = Py_Dict_Get_توکی_String(dict, "PathFinder")))
    {
	Py_DECREF(imp);
	أرجع -1;
    }

    إذا (!(py_پیدا_کړه_spec = PyObject_GetAttrString(cls, "find_spec")))
    {
	Py_DECREF(imp);
	أرجع -1;
    }

# إذا PY_نسخه_HEX < 0x30c00a7
    // find_module has been removed as of Python 3.12.0a7
    إذا ((py_پیدا_کړه_module = PyObject_GetAttrString(cls, "find_module")))
	جمع_کړه_OBJECT(m, "_find_module", py_پیدا_کړه_module);
# endif

    Py_DECREF(imp);

    جمع_کړه_OBJECT(m, "_find_spec", py_پیدا_کړه_spec);
#وإلا
    إذا (!(imp = PyImport_ImportModule("imp")))
	أرجع -1;

    إذا (!(py_پیدا_کړه_module = PyObject_GetAttrString(imp, "find_module")))
    {
	Py_DECREF(imp);
	أرجع -1;
    }

    إذا (!(py_بارول_module = PyObject_GetAttrString(imp, "load_module")))
    {
	Py_DECREF(py_پیدا_کړه_module);
	Py_DECREF(imp);
	أرجع -1;
    }

    Py_DECREF(imp);

    جمع_کړه_OBJECT(m, "_find_module", py_پیدا_کړه_module);
    جمع_کړه_OBJECT(m, "_load_module", py_بارول_module);
#د_که_پای

    أرجع 0;
}
