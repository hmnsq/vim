/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * typval.c: functions that deal with a typval
 */

#شامل "vim.h"

#اگر defined(FEAT_EVAL)

/*
 * Allocate memory for a variable type-value, and make it empty (0 or NULL
 * value).
 */
    typval_T *
alloc_tv(خالی)
{
    واپس ALLOC_CLEAR_ONE(typval_T);
}

/*
 * Allocate memory for a variable type-value, and assign a string to it.
 * The string "s" must have been allocated, it is consumed.
 * Return NULL for out of memory, the variable otherwise.
 */
    typval_T *
alloc_string_tv(char_u *s)
{
    typval_T	*rettv;

    rettv = alloc_tv();
    اگر (rettv != تہی)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = s;
    }
    ورنہ
	vim_free(s);
    واپس rettv;
}

/*
 * Free the memory for a variable type-value.
 */
    خالی
free_tv(typval_T *varp)
{
    اگر (varp == تہی)
	واپس;

    انتخاب (varp->v_type)
    {
	حالت VAR_FUNC:
	    func_unref(varp->vval.v_string);
	    // FALLTHROUGH
	حالت VAR_STRING:
	    vim_free(varp->vval.v_string);
	    توڑو;
	حالت VAR_PARTIAL:
	    partial_unref(varp->vval.v_partial);
	    توڑو;
	حالت VAR_BLOB:
	    blob_unref(varp->vval.v_blob);
	    توڑو;
	حالت VAR_LIST:
	    list_unref(varp->vval.v_list);
	    توڑو;
	حالت VAR_TUPLE:
	    tuple_unref(varp->vval.v_tuple);
	    توڑو;
	حالت VAR_DICT:
	    dict_unref(varp->vval.v_dict);
	    توڑو;
	حالت VAR_کام:
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
	    کام_unref(varp->vval.v_کام);
	    توڑو;
#اگر_ختم
	حالت VAR_CHANNEL:
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
	    channel_unref(varp->vval.v_channel);
	    توڑو;
#اگر_ختم
	حالت VAR_CLASS:
	    class_unref(varp->vval.v_class);
	    توڑو;
	حالت VAR_OBJECT:
	    object_unref(varp->vval.v_object);
	    توڑو;

	حالت VAR_TYPEALIAS:
	    typealias_unref(varp->vval.v_typealias);
	    توڑو;

	حالت VAR_نمبر:
	حالت VAR_FLOAT:
	حالت VAR_ANY:
	حالت VAR_UNKNOWN:
	حالت VAR_VOID:
	حالت VAR_BOOL:
	حالت VAR_SPECIAL:
	حالت VAR_INSTR:
	    توڑو;
    }
    vim_free(varp);
}

/*
 * Free the memory for a variable value and set the value to NULL or 0.
 */
    خالی
clear_tv(typval_T *varp)
{
    اگر (varp == تہی)
	واپس;

    انتخاب (varp->v_type)
    {
	حالت VAR_FUNC:
	    func_unref(varp->vval.v_string);
	    // FALLTHROUGH
	حالت VAR_STRING:
	    VIM_CLEAR(varp->vval.v_string);
	    توڑو;
	حالت VAR_PARTIAL:
	    partial_unref(varp->vval.v_partial);
	    varp->vval.v_partial = تہی;
	    توڑو;
	حالت VAR_BLOB:
	    blob_unref(varp->vval.v_blob);
	    varp->vval.v_blob = تہی;
	    توڑو;
	حالت VAR_LIST:
	    list_unref(varp->vval.v_list);
	    varp->vval.v_list = تہی;
	    توڑو;
	حالت VAR_TUPLE:
	    tuple_unref(varp->vval.v_tuple);
	    varp->vval.v_tuple = تہی;
	    توڑو;
	حالت VAR_DICT:
	    dict_unref(varp->vval.v_dict);
	    varp->vval.v_dict = تہی;
	    توڑو;
	حالت VAR_نمبر:
	حالت VAR_BOOL:
	حالت VAR_SPECIAL:
	    varp->vval.v_نمبر = 0;
	    توڑو;
	حالت VAR_FLOAT:
	    varp->vval.v_float = 0.0;
	    توڑو;
	حالت VAR_کام:
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
	    کام_unref(varp->vval.v_کام);
	    varp->vval.v_کام = تہی;
#اگر_ختم
	    توڑو;
	حالت VAR_CHANNEL:
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
	    channel_unref(varp->vval.v_channel);
	    varp->vval.v_channel = تہی;
#اگر_ختم
	    توڑو;
	حالت VAR_INSTR:
	    VIM_CLEAR(varp->vval.v_instr);
	    توڑو;
	حالت VAR_CLASS:
	    class_unref(varp->vval.v_class);
	    varp->vval.v_class = تہی;
	    توڑو;
	حالت VAR_OBJECT:
	    object_unref(varp->vval.v_object);
	    varp->vval.v_object = تہی;
	    توڑو;
	حالت VAR_TYPEALIAS:
	    typealias_unref(varp->vval.v_typealias);
	    varp->vval.v_typealias = تہی;
	    توڑو;
	حالت VAR_UNKNOWN:
	حالت VAR_ANY:
	حالت VAR_VOID:
	    توڑو;
    }
    varp->v_تالا = 0;
}

/*
 * Set the value of a variable to NULL without freeing items.
 */
    خالی
init_tv(typval_T *varp)
{
    اگر (varp != تہی)
	CLEAR_اشارہ_گر(varp);
}

    ساکن varnumber_T
tv_get_bool_or_نمبر_chk(
	typval_T    *varp,
	صحیح	    *denote,
	صحیح	    want_bool,
	صحیح	    vim9_string_error) // in Vim9 using a string is an error
{
    varnumber_T	n = 0L;

    انتخاب (varp->v_type)
    {
	حالت VAR_نمبر:
	    اگر (اندر_vim9script() && want_bool && varp->vval.v_نمبر != 0
						   && varp->vval.v_نمبر != 1)
	    {
		semsg(_(e_using_نمبر_as_bool_nr), varp->vval.v_نمبر);
		توڑو;
	    }
	    واپس varp->vval.v_نمبر;
	حالت VAR_FLOAT:
	    emsg(_(e_using_float_as_نمبر));
	    توڑو;
	حالت VAR_FUNC:
	حالت VAR_PARTIAL:
	    emsg(_(e_using_funcref_as_نمبر));
	    توڑو;
	حالت VAR_STRING:
	    اگر (vim9_string_error && اندر_vim9script())
	    {
		emsg_using_string_as(varp, !want_bool);
		توڑو;
	    }
	    اگر (varp->vval.v_string != تہی)
		vim_str2nr(varp->vval.v_string, تہی, تہی,
					 STR2NR_ALL, &n, تہی, 0, FALSE, تہی);
	    واپس n;
	حالت VAR_LIST:
	    emsg(_(e_using_list_as_نمبر));
	    توڑو;
	حالت VAR_TUPLE:
	    emsg(_(e_using_tuple_as_نمبر));
	    توڑو;
	حالت VAR_DICT:
	    emsg(_(e_using_dictionary_as_نمبر));
	    توڑو;
	حالت VAR_BOOL:
	حالت VAR_SPECIAL:
	    اگر (!want_bool && اندر_vim9script())
	    {
		اگر (varp->v_type == VAR_BOOL)
		    emsg(_(e_using_bool_as_نمبر));
		ورنہ
		    emsg(_(e_using_special_as_نمبر));
		توڑو;
	    }
	    واپس varp->vval.v_نمبر == VVAL_TRUE ? 1 : 0;
	حالت VAR_کام:
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
	    emsg(_(e_using_کام_as_نمبر));
	    توڑو;
#اگر_ختم
	حالت VAR_CHANNEL:
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
	    emsg(_(e_using_channel_as_نمبر));
	    توڑو;
#اگر_ختم
	حالت VAR_BLOB:
	    emsg(_(e_using_blob_as_نمبر));
	    توڑو;
	حالت VAR_CLASS:
	حالت VAR_TYPEALIAS:
	    check_typval_is_قدر(varp);
	    توڑو;
	حالت VAR_OBJECT:
	    {
		اگر (varp->vval.v_object == تہی)
		    emsg(_(e_using_object_as_string));
		ورنہ
		{
		    class_T *cl = varp->vval.v_object->obj_class;
		    اگر (cl != تہی && IS_ENUM(cl))
			semsg(_(e_using_enum_str_as_نمبر), cl->class_name.string);
		    ورنہ
			emsg(_(e_using_object_as_نمبر));
		}
	    }
	    توڑو;
	حالت VAR_VOID:
	    emsg(_(e_cannot_use_void_قدر));
	    توڑو;
	حالت VAR_UNKNOWN:
	حالت VAR_ANY:
	حالت VAR_INSTR:
	    اندرونی_error_no_abort("tv_get_number(UNKNOWN)");
	    توڑو;
    }
    اگر (denote == تہی)		// useful for values that must be unsigned
	n = -1;
    ورنہ
	*denote = TRUE;
    واپس n;
}

/*
 * Get the number value of a variable.
 * If it is a String variable, uses vim_str2nr().
 * For incompatible types, return 0.
 * tv_get_number_chk() is similar to tv_get_number(), but informs the
 * caller of incompatible types: it sets *denote to TRUE if "denote"
 * is not NULL or returns -1 otherwise.
 */
    varnumber_T
tv_get_نمبر(typval_T *varp)
{
    صحیح		error = FALSE;

    واپس tv_get_نمبر_chk(varp, &error);	// return 0L on error
}

/*
 * Like tv_get_number() but in Vim9 script do convert a number in a string to a
 * number without giving an error.
 */
    varnumber_T
tv_مقصد_نمبر(typval_T *varp)
{
    صحیح		error = FALSE;

    واپس tv_get_bool_or_نمبر_chk(varp, &error, FALSE, FALSE);
}

    varnumber_T
tv_get_نمبر_chk(typval_T *varp, صحیح *denote)
{
    واپس tv_get_bool_or_نمبر_chk(varp, denote, FALSE, TRUE);
}

/*
 * Get the boolean value of "varp".  This is like tv_get_number_chk(),
 * but in Vim9 script accepts Number (0 and 1) and Bool/Special.
 */
    varnumber_T
tv_get_bool(typval_T *varp)
{
    واپس tv_get_bool_or_نمبر_chk(varp, تہی, TRUE, TRUE);
}

/*
 * Get the boolean value of "varp".  This is like tv_get_number_chk(),
 * but in Vim9 script accepts Number and Bool.
 */
    varnumber_T
tv_get_bool_chk(typval_T *varp, صحیح *denote)
{
    واپس tv_get_bool_or_نمبر_chk(varp, denote, TRUE, TRUE);
}

    ساکن float_T
tv_get_float_chk(typval_T *varp, صحیح *error)
{
    انتخاب (varp->v_type)
    {
	حالت VAR_نمبر:
	    واپس (float_T)(varp->vval.v_نمبر);
	حالت VAR_FLOAT:
	    واپس varp->vval.v_float;
	حالت VAR_FUNC:
	حالت VAR_PARTIAL:
	    emsg(_(e_using_funcref_as_float));
	    توڑو;
	حالت VAR_STRING:
	    emsg(_(e_using_string_as_float));
	    توڑو;
	حالت VAR_LIST:
	    emsg(_(e_using_list_as_float));
	    توڑو;
	حالت VAR_TUPLE:
	    emsg(_(e_using_tuple_as_float));
	    توڑو;
	حالت VAR_DICT:
	    emsg(_(e_using_dictionary_as_float));
	    توڑو;
	حالت VAR_BOOL:
	    emsg(_(e_using_boolean_قدر_as_float));
	    توڑو;
	حالت VAR_SPECIAL:
	    emsg(_(e_using_special_قدر_as_float));
	    توڑو;
	حالت VAR_کام:
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
	    emsg(_(e_using_کام_as_float));
	    توڑو;
#اگر_ختم
	حالت VAR_CHANNEL:
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
	    emsg(_(e_using_channel_as_float));
	    توڑو;
#اگر_ختم
	حالت VAR_BLOB:
	    emsg(_(e_using_blob_as_float));
	    توڑو;
	حالت VAR_CLASS:
	حالت VAR_TYPEALIAS:
	    check_typval_is_قدر(varp);
	    توڑو;
	حالت VAR_OBJECT:
	    emsg(_(e_using_object_as_float));
	    توڑو;
	حالت VAR_VOID:
	    emsg(_(e_cannot_use_void_قدر));
	    توڑو;
	حالت VAR_UNKNOWN:
	حالت VAR_ANY:
	حالت VAR_INSTR:
	    اندرونی_error_no_abort("tv_get_float(UNKNOWN)");
	    توڑو;
    }
    اگر (error != تہی)
	*error = TRUE;
    واپس 0;
}

    float_T
tv_get_float(typval_T *varp)
{
    واپس tv_get_float_chk(varp, تہی);
}

/*
 * Give an error and return FAIL unless "args[idx]" is unknown
 */
    صحیح
check_for_unknown_arg(typval_T *دلائل, صحیح idx)
{
    اگر (دلائل[idx].v_type != VAR_UNKNOWN)
    {
	semsg(_(e_too_many_arguments), idx + 1);
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string.
 */
    صحیح
check_for_string_arg(typval_T *دلائل, صحیح idx)
{
    اگر (دلائل[idx].v_type != VAR_STRING)
    {
	semsg(_(e_string_required_for_دلیل_nr), idx + 1);
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a non-empty string.
 */
    صحیح
check_for_nonempty_string_arg(typval_T *دلائل, صحیح idx)
{
    اگر (check_for_string_arg(دلائل, idx) == FAIL)
	واپس FAIL;
    اگر (دلائل[idx].vval.v_string == تہی || *دلائل[idx].vval.v_string == NUL)
    {
	semsg(_(e_non_خالی_خانے_string_required_for_دلیل_nr), idx + 1);
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Check for an optional string argument at 'idx'
 */
    صحیح
check_for_opt_string_arg(typval_T *دلائل, صحیح idx)
{
    واپس (دلائل[idx].v_type == VAR_UNKNOWN
	    || check_for_string_arg(دلائل, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a number.
 */
    صحیح
check_for_نمبر_arg(typval_T *دلائل, صحیح idx)
{
    اگر (دلائل[idx].v_type != VAR_نمبر)
    {
	semsg(_(e_نمبر_required_for_دلیل_nr), idx + 1);
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Check for an optional number argument at 'idx'
 */
    صحیح
check_for_opt_نمبر_arg(typval_T *دلائل, صحیح idx)
{
    واپس (دلائل[idx].v_type == VAR_UNKNOWN
	    || check_for_نمبر_arg(دلائل, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a float or a number.
 */
    صحیح
check_for_float_or_nr_arg(typval_T *دلائل, صحیح idx)
{
    اگر (دلائل[idx].v_type != VAR_FLOAT && دلائل[idx].v_type != VAR_نمبر)
    {
	semsg(_(e_float_or_نمبر_required_for_دلیل_nr), idx + 1);
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a bool.
 */
    صحیح
check_for_bool_arg(typval_T *دلائل, صحیح idx)
{
    اگر (دلائل[idx].v_type != VAR_BOOL
	    && !(دلائل[idx].v_type == VAR_نمبر
		&& (دلائل[idx].vval.v_نمبر == 0
		    || دلائل[idx].vval.v_نمبر == 1)))
    {
	semsg(_(e_bool_required_for_دلیل_nr), idx + 1);
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a bool or a number.
 */
    ساکن صحیح
check_for_bool_or_نمبر_arg(typval_T *دلائل, صحیح idx)
{
    اگر (دلائل[idx].v_type != VAR_BOOL && دلائل[idx].v_type != VAR_نمبر)
    {
	semsg(_(e_bool_or_نمبر_required_for_دلیل_nr), idx + 1);
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Check for an optional bool argument at 'idx'.
 * Return FAIL if the type is wrong.
 */
    صحیح
check_for_opt_bool_arg(typval_T *دلائل, صحیح idx)
{
    اگر (دلائل[idx].v_type == VAR_UNKNOWN)
	واپس OK;
    واپس check_for_bool_arg(دلائل, idx);
}

/*
 * Check for an optional bool or number argument at 'idx'.
 * Return FAIL if the type is wrong.
 */
    صحیح
check_for_opt_bool_or_نمبر_arg(typval_T *دلائل, صحیح idx)
{
    اگر (دلائل[idx].v_type == VAR_UNKNOWN)
	واپس OK;
    واپس check_for_bool_or_نمبر_arg(دلائل, idx);
}

/*
 * Check for an optional bool or dict argument at 'idx'.
 * Return FAIL if the type is wrong.
 */
    صحیح
check_for_opt_bool_or_dict_arg(typval_T *دلائل, صحیح idx)
{
    اگر (دلائل[idx].v_type == VAR_UNKNOWN || دلائل[idx].v_type == VAR_DICT)
	واپس OK;
    واپس check_for_bool_arg(دلائل, idx);
}

/*
 * Give an error and return FAIL unless "args[idx]" is a blob.
 */
    صحیح
check_for_blob_arg(typval_T *دلائل, صحیح idx)
{
    اگر (دلائل[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_blob_required_for_دلیل_nr), idx + 1);
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list.
 */
    صحیح
check_for_list_arg(typval_T *دلائل, صحیح idx)
{
    اگر (دلائل[idx].v_type != VAR_LIST)
    {
	semsg(_(e_list_required_for_دلیل_nr), idx + 1);
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a non-NULL list.
 */
    صحیح
check_for_nonnull_list_arg(typval_T *دلائل, صحیح idx)
{
    اگر (check_for_list_arg(دلائل, idx) == FAIL)
	واپس FAIL;

    اگر (دلائل[idx].vval.v_list == تہی)
    {
	semsg(_(e_non_null_list_required_for_دلیل_nr), idx + 1);
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Check for an optional list argument at 'idx'
 */
    صحیح
check_for_opt_list_arg(typval_T *دلائل, صحیح idx)
{
    واپس (دلائل[idx].v_type == VAR_UNKNOWN
	    || check_for_list_arg(دلائل, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a tuple.
 */
    صحیح
check_for_tuple_arg(typval_T *دلائل, صحیح idx)
{
    اگر (دلائل[idx].v_type != VAR_TUPLE)
    {
	semsg(_(e_tuple_required_for_دلیل_nr), idx + 1);
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a dict.
 */
    صحیح
check_for_dict_arg(typval_T *دلائل, صحیح idx)
{
    اگر (دلائل[idx].v_type != VAR_DICT)
    {
	semsg(_(e_dict_required_for_دلیل_nr), idx + 1);
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a non-NULL dict.
 */
    صحیح
check_for_nonnull_dict_arg(typval_T *دلائل, صحیح idx)
{
    اگر (check_for_dict_arg(دلائل, idx) == FAIL)
	واپس FAIL;

    اگر (دلائل[idx].vval.v_dict == تہی)
    {
	semsg(_(e_non_null_dict_required_for_دلیل_nr), idx + 1);
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Check for an optional dict argument at 'idx'
 */
    صحیح
check_for_opt_dict_arg(typval_T *دلائل, صحیح idx)
{
    واپس (دلائل[idx].v_type == VAR_UNKNOWN
	    || check_for_dict_arg(دلائل, idx) != FAIL) ? OK : FAIL;
}

/*
 * Check for an optional non-NULL dict argument at 'idx'
 */
    صحیح
check_for_opt_nonnull_dict_arg(typval_T *دلائل, صحیح idx)
{
    واپس (دلائل[idx].v_type == VAR_UNKNOWN
	    || check_for_nonnull_dict_arg(دلائل, idx) != FAIL) ? OK : FAIL;
}

#اگر defined(FEAT_کام_CHANNEL)
/*
 * Give an error and return FAIL unless "args[idx]" is a channel or a job.
 */
    صحیح
check_for_chan_or_کام_arg(typval_T *دلائل, صحیح idx)
{
    اگر (دلائل[idx].v_type != VAR_CHANNEL && دلائل[idx].v_type != VAR_کام)
    {
	semsg(_(e_chan_or_کام_required_for_دلیل_nr), idx + 1);
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an optional channel or a
 * job.
 */
    صحیح
check_for_opt_chan_or_کام_arg(typval_T *دلائل, صحیح idx)
{
    واپس (دلائل[idx].v_type == VAR_UNKNOWN
	    || check_for_chan_or_کام_arg(دلائل, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a job.
 */
    صحیح
check_for_کام_arg(typval_T *دلائل, صحیح idx)
{
    اگر (دلائل[idx].v_type != VAR_کام)
    {
	semsg(_(e_کام_required_for_دلیل_nr), idx + 1);
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Check for an optional job argument at 'idx'.
 */
    صحیح
check_for_opt_کام_arg(typval_T *دلائل, صحیح idx)
{
    واپس (دلائل[idx].v_type == VAR_UNKNOWN
	    || check_for_کام_arg(دلائل, idx) != FAIL) ? OK : FAIL;
}
#ورنہ
/*
 * Give an error and return FAIL unless "args[idx]" is an optional channel or a
 * job.  Used without the +channel feature, thus only VAR_UNKNOWN is accepted.
 */
    صحیح
check_for_opt_chan_or_کام_arg(typval_T *دلائل, صحیح idx)
{
    واپس دلائل[idx].v_type == VAR_UNKNOWN ? OK : FAIL;
}
#اگر_ختم

/*
 * Give an error and return FAIL unless "args[idx]" is a string or
 * a number.
 */
    صحیح
check_for_string_or_نمبر_arg(typval_T *دلائل, صحیح idx)
{
    اگر (دلائل[idx].v_type != VAR_STRING && دلائل[idx].v_type != VAR_نمبر)
    {
	semsg(_(e_string_or_نمبر_required_for_دلیل_nr), idx + 1);
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Check for an optional string or number argument at 'idx'.
 */
    صحیح
check_for_opt_string_or_نمبر_arg(typval_T *دلائل, صحیح idx)
{
    واپس (دلائل[idx].v_type == VAR_UNKNOWN
	    || check_for_string_or_نمبر_arg(دلائل, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a buffer number.
 * Buffer number can be a number or a string.
 */
    صحیح
check_for_بفر_arg(typval_T *دلائل, صحیح idx)
{
    واپس check_for_string_or_نمبر_arg(دلائل, idx);
}

/*
 * Check for an optional buffer argument at 'idx'
 */
    صحیح
check_for_opt_بفر_arg(typval_T *دلائل, صحیح idx)
{
    واپس (دلائل[idx].v_type == VAR_UNKNOWN
	    || check_for_بفر_arg(دلائل, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a line number.
 * Line number can be a number or a string.
 */
    صحیح
check_for_lnum_arg(typval_T *دلائل, صحیح idx)
{
    واپس check_for_string_or_نمبر_arg(دلائل, idx);
}

/*
 * Check for an optional line number argument at 'idx'
 */
    صحیح
check_for_opt_lnum_arg(typval_T *دلائل, صحیح idx)
{
    واپس (دلائل[idx].v_type == VAR_UNKNOWN
	    || check_for_lnum_arg(دلائل, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a blob.
 */
    صحیح
check_for_string_or_blob_arg(typval_T *دلائل, صحیح idx)
{
    اگر (دلائل[idx].v_type != VAR_STRING && دلائل[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_string_or_blob_required_for_دلیل_nr), idx + 1);
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a list.
 */
    صحیح
check_for_string_or_list_arg(typval_T *دلائل, صحیح idx)
{
    اگر (دلائل[idx].v_type != VAR_STRING && دلائل[idx].v_type != VAR_LIST)
    {
	semsg(_(e_string_or_list_required_for_دلیل_nr), idx + 1);
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string, a list, a
 * tuple or a blob.
 */
    صحیح
check_for_string_or_list_or_tuple_or_blob_arg(typval_T *دلائل, صحیح idx)
{
    اگر (دلائل[idx].v_type != VAR_STRING
	    && دلائل[idx].v_type != VAR_LIST
	    && دلائل[idx].v_type != VAR_TUPLE
	    && دلائل[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_string_list_tuple_or_blob_required_for_دلیل_nr), idx + 1);
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Check for an optional string or list argument at 'idx'
 */
    صحیح
check_for_opt_string_or_list_arg(typval_T *دلائل, صحیح idx)
{
    واپس (دلائل[idx].v_type == VAR_UNKNOWN
	    || check_for_string_or_list_arg(دلائل, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a dict.
 */
    صحیح
check_for_string_or_dict_arg(typval_T *دلائل, صحیح idx)
{
    اگر (دلائل[idx].v_type != VAR_STRING && دلائل[idx].v_type != VAR_DICT)
    {
	semsg(_(e_string_or_dict_required_for_دلیل_nr), idx + 1);
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a number
 * or a list.
 */
    صحیح
check_for_string_or_نمبر_or_list_arg(typval_T *دلائل, صحیح idx)
{
    اگر (دلائل[idx].v_type != VAR_STRING
	    && دلائل[idx].v_type != VAR_نمبر
	    && دلائل[idx].v_type != VAR_LIST)
    {
	semsg(_(e_string_نمبر_or_list_required_for_دلیل_nr), idx + 1);
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an optional string
 * or number or a list
 */
    صحیح
check_for_opt_string_or_نمبر_or_list_arg(typval_T *دلائل, صحیح idx)
{
    واپس (دلائل[idx].v_type == VAR_UNKNOWN
	    || check_for_string_or_نمبر_or_list_arg(دلائل, idx)
							!= FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string, a number, a
 * list, a tuple or a blob.
 */
    صحیح
check_for_repeat_func_arg(typval_T *دلائل, صحیح idx)
{
    اگر (دلائل[idx].v_type != VAR_STRING
	    && دلائل[idx].v_type != VAR_نمبر
	    && دلائل[idx].v_type != VAR_LIST
	    && دلائل[idx].v_type != VAR_TUPLE
	    && دلائل[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_repeatable_type_required_for_دلیل_nr), idx + 1);
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string, a list, a
 * tuple or a dict.
 */
    صحیح
check_for_string_list_tuple_or_dict_arg(typval_T *دلائل, صحیح idx)
{
    اگر (دلائل[idx].v_type != VAR_STRING
	    && دلائل[idx].v_type != VAR_LIST
	    && دلائل[idx].v_type != VAR_TUPLE
	    && دلائل[idx].v_type != VAR_DICT)
    {
	semsg(_(e_string_list_tuple_or_dict_required_for_دلیل_nr), idx + 1);
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string
 * or a function reference.
 */
    صحیح
check_for_string_or_func_arg(typval_T *دلائل, صحیح idx)
{
    اگر (دلائل[idx].v_type != VAR_PARTIAL
	    && دلائل[idx].v_type != VAR_FUNC
	    && دلائل[idx].v_type != VAR_STRING)
    {
	semsg(_(e_string_or_function_required_for_دلیل_nr), idx + 1);
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list or a blob.
 */
    صحیح
check_for_list_or_blob_arg(typval_T *دلائل, صحیح idx)
{
    اگر (دلائل[idx].v_type != VAR_LIST && دلائل[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_list_or_blob_required_for_دلیل_nr), idx + 1);
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list or a tuple.
 */
    صحیح
check_for_list_or_tuple_arg(typval_T *دلائل, صحیح idx)
{
    اگر (دلائل[idx].v_type != VAR_LIST && دلائل[idx].v_type != VAR_TUPLE)
    {
	semsg(_(e_list_or_tuple_required_for_دلیل_nr), idx + 1);
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list, a tuple or a
 * blob.
 */
    صحیح
check_for_list_or_tuple_or_blob_arg(typval_T *دلائل, صحیح idx)
{
    اگر (دلائل[idx].v_type != VAR_LIST
	    && دلائل[idx].v_type != VAR_TUPLE
	    && دلائل[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_list_or_tuple_or_blob_required_for_دلیل_nr), idx + 1);
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list, a tuple or a
 * dict
 */
    صحیح
check_for_list_or_tuple_or_dict_arg(typval_T *دلائل, صحیح idx)
{
    اگر (دلائل[idx].v_type != VAR_LIST
	    && دلائل[idx].v_type != VAR_TUPLE
	    && دلائل[idx].v_type != VAR_DICT)
    {
	semsg(_(e_list_or_tuple_or_dict_required_for_دلیل_nr), idx + 1);
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list or dict or a
 * blob.
 */
    صحیح
check_for_list_or_dict_or_blob_arg(typval_T *دلائل, صحیح idx)
{
    اگر (دلائل[idx].v_type != VAR_LIST
	    && دلائل[idx].v_type != VAR_DICT
	    && دلائل[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_list_dict_or_blob_required_for_دلیل_nr), idx + 1);
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list, a tuple, a dict,
 * a blob or a string.
 */
    صحیح
check_for_list_tuple_dict_blob_or_string_arg(typval_T *دلائل, صحیح idx)
{
    اگر (دلائل[idx].v_type != VAR_LIST
	    && دلائل[idx].v_type != VAR_TUPLE
	    && دلائل[idx].v_type != VAR_DICT
	    && دلائل[idx].v_type != VAR_BLOB
	    && دلائل[idx].v_type != VAR_STRING)
    {
	semsg(_(e_list_tuple_dict_blob_or_string_required_for_دلیل_nr), idx + 1);
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an optional buffer
 * number or a dict.
 */
    صحیح
check_for_opt_بفر_or_dict_arg(typval_T *دلائل, صحیح idx)
{
    اگر (دلائل[idx].v_type != VAR_UNKNOWN
	    && دلائل[idx].v_type != VAR_STRING
	    && دلائل[idx].v_type != VAR_نمبر
	    && دلائل[idx].v_type != VAR_DICT)
    {
	semsg(_(e_string_required_for_دلیل_nr), idx + 1);
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an object.
 */
    صحیح
check_for_object_arg(typval_T *دلائل, صحیح idx)
{
    اگر (دلائل[idx].v_type != VAR_OBJECT)
    {
	semsg(_(e_object_required_for_دلیل_nr), idx + 1);
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Returns TRUE if "tv" is a type alias for a class
 */
    ساکن صحیح
tv_class_alias(typval_T *tv)
{
    واپس tv->v_type == VAR_TYPEALIAS &&
			tv->vval.v_typealias->ta_type->tt_type == VAR_OBJECT;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a class
 * or class typealias.
 */
    صحیح
check_for_class_or_typealias_دلائل(typval_T *دلائل, صحیح idx)
{
    برائے (صحیح i = idx; دلائل[i].v_type != VAR_UNKNOWN; ++i)
    {
	اگر (دلائل[i].v_type != VAR_CLASS && !tv_class_alias(&دلائل[idx]))
	{
	    semsg(_(e_class_or_typealias_required_for_دلیل_nr), i + 1);
	    واپس FAIL;
	}
    }
    واپس OK;
}

/*
 * Get the string value of a variable.
 * If it is a Number variable, the number is converted into a string.
 * tv_get_string() uses a single, static buffer.  YOU CAN ONLY USE IT ONCE!
 * tv_get_string_buf() uses a given buffer.
 * If the String variable has never been set, return an empty string.
 * Never returns NULL;
 * tv_get_string_chk() and tv_get_string_buf_chk() are similar, but return
 * NULL on error.
 */
    char_u *
tv_get_string(typval_T *varp)
{
    ساکن char_u   mybuf[NUMBUFLEN];

    واپس tv_get_string_buf(varp, mybuf);
}

/*
 * Like tv_get_string() but don't allow number to string conversion for Vim9.
 */
    char_u *
tv_get_string_strict(typval_T *varp)
{
    ساکن char_u   mybuf[NUMBUFLEN];
    char_u	    *res =  tv_get_string_buf_chk_strict(
						 varp, mybuf, اندر_vim9script());

    واپس res != تہی ? res : (char_u *)"";
}

    char_u *
tv_get_string_buf(typval_T *varp, char_u *buf)
{
    char_u	*res = tv_get_string_buf_chk(varp, buf);

    واپس res != تہی ? res : (char_u *)"";
}

/*
 * Careful: This uses a single, static buffer.  YOU CAN ONLY USE IT ONCE!
 */
    char_u *
tv_get_string_chk(typval_T *varp)
{
    ساکن char_u   mybuf[NUMBUFLEN];

    واپس tv_get_string_buf_chk(varp, mybuf);
}

    char_u *
tv_get_string_buf_chk(typval_T *varp, char_u *buf)
{
    واپس tv_get_string_buf_chk_strict(varp, buf, FALSE);
}

    char_u *
tv_get_string_buf_chk_strict(typval_T *varp, char_u *buf, صحیح strict)
{
    انتخاب (varp->v_type)
    {
	حالت VAR_نمبر:
	    اگر (strict)
	    {
		emsg(_(e_using_نمبر_as_string));
		توڑو;
	    }
	    vim_snprintf((حرف *)buf, NUMBUFLEN, "%lld",
					    (varnumber_T)varp->vval.v_نمبر);
	    واپس buf;
	حالت VAR_FUNC:
	حالت VAR_PARTIAL:
	    emsg(_(e_using_funcref_as_string));
	    توڑو;
	حالت VAR_LIST:
	    emsg(_(e_using_list_as_string));
	    توڑو;
	حالت VAR_TUPLE:
	    emsg(_(e_using_tuple_as_string));
	    توڑو;
	حالت VAR_DICT:
	    emsg(_(e_using_dictionary_as_string));
	    توڑو;
	حالت VAR_FLOAT:
	    اگر (strict)
	    {
		emsg(_(e_using_float_as_string));
		توڑو;
	    }
	    vim_snprintf((حرف *)buf, NUMBUFLEN, "%g", varp->vval.v_float);
	    واپس buf;
	حالت VAR_STRING:
	    اگر (varp->vval.v_string != تہی)
		واپس varp->vval.v_string;
	    واپس (char_u *)"";
	حالت VAR_BOOL:
	حالت VAR_SPECIAL:
	    STRCPY(buf, get_var_special_name(varp->vval.v_نمبر));
	    واپس buf;
	حالت VAR_BLOB:
	    emsg(_(e_using_blob_as_string));
	    توڑو;
	حالت VAR_CLASS:
	حالت VAR_TYPEALIAS:
	    check_typval_is_قدر(varp);
	    توڑو;
	حالت VAR_OBJECT:
	    {
		اگر (varp->vval.v_object == تہی)
		    emsg(_(e_using_object_as_string));
		ورنہ
		{
		    class_T *cl = varp->vval.v_object->obj_class;
		    اگر (cl != تہی && IS_ENUM(cl))
			semsg(_(e_using_enum_str_as_string), cl->class_name.string);
		    ورنہ
			emsg(_(e_using_object_as_string));
		}
	    }
	    توڑو;
	حالت VAR_کام:
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
	    اگر (اندر_vim9script())
	    {
		semsg(_(e_using_invalid_قدر_as_string_str), "job");
		توڑو;
	    }
	    واپس کام_مقصد_string_buf(varp, buf);
#اگر_ختم
	    توڑو;
	حالت VAR_CHANNEL:
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
	    اگر (اندر_vim9script())
	    {
		semsg(_(e_using_invalid_قدر_as_string_str), "channel");
		توڑو;
	    }
	    واپس channel_مقصد_string_buf(varp, buf);
#اگر_ختم
	    توڑو;
	حالت VAR_VOID:
	    emsg(_(e_cannot_use_void_قدر));
	    توڑو;
	حالت VAR_UNKNOWN:
	حالت VAR_ANY:
	حالت VAR_INSTR:
	    semsg(_(e_using_invalid_قدر_as_string_str),
						  vartype_name(varp->v_type));
	    توڑو;
    }
    واپس تہی;
}

/*
 * Turn a typeval into a string.  Similar to tv_get_string_buf() but uses
 * string() on Dict, List, etc.
 */
    char_u *
tv_stringify(typval_T *varp, char_u *buf)
{
    اگر (varp->v_type == VAR_LIST
	    || varp->v_type == VAR_DICT
	    || varp->v_type == VAR_BLOB
	    || varp->v_type == VAR_FUNC
	    || varp->v_type == VAR_PARTIAL
	    || varp->v_type == VAR_FLOAT)
    {
	typval_T tmp;

	init_tv(&tmp);
	f_string(varp, &tmp);
	tv_get_string_buf(&tmp, buf);
	clear_tv(varp);
	*varp = tmp;
	واپس tmp.vval.v_string;
    }
    واپس tv_get_string_buf(varp, buf);
}

/*
 * Return TRUE if typeval "tv" and its value are set to be locked (immutable).
 * Also give an error message, using "name" or _("name") when use_gettext is
 * TRUE.
 */
    صحیح
tv_check_تالا(typval_T *tv, char_u *name, صحیح use_gettext)
{
    صحیح	تالا = 0;

    انتخاب (tv->v_type)
    {
	حالت VAR_BLOB:
	    اگر (tv->vval.v_blob != تہی)
		تالا = tv->vval.v_blob->bv_تالا;
	    توڑو;
	حالت VAR_LIST:
	    اگر (tv->vval.v_list != تہی)
		تالا = tv->vval.v_list->lv_تالا;
	    توڑو;
	حالت VAR_TUPLE:
	    اگر (tv->vval.v_tuple != تہی)
		تالا = tv->vval.v_tuple->tv_تالا;
	    توڑو;
	حالت VAR_DICT:
	    اگر (tv->vval.v_dict != تہی)
		تالا = tv->vval.v_dict->dv_تالا;
	    توڑو;
	طےشدہ:
	    توڑو;
    }
    واپس قدر_check_تالا(tv->v_تالا, name, use_gettext)
		   || (تالا != 0 && قدر_check_تالا(تالا, name, use_gettext));
}

/*
 * Copy the values from typval_T "from" to typval_T "to".
 * When needed allocates string or increases reference count.
 * Does not make a copy of a list, blob or dict but copies the reference!
 * It is OK for "from" and "to" to point to the same item.  This is used to
 * make a copy later.
 */
    صحیح
copy_tv(typval_T *from, typval_T *مقصد)
{
    صحیح		ret = OK;

    مقصد->v_type = from->v_type;
    مقصد->v_تالا = 0;
    انتخاب (from->v_type)
    {
	حالت VAR_نمبر:
	حالت VAR_BOOL:
	حالت VAR_SPECIAL:
	    مقصد->vval.v_نمبر = from->vval.v_نمبر;
	    توڑو;
	حالت VAR_FLOAT:
	    مقصد->vval.v_float = from->vval.v_float;
	    توڑو;
	حالت VAR_کام:
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
	    مقصد->vval.v_کام = from->vval.v_کام;
	    اگر (مقصد->vval.v_کام != تہی)
		++مقصد->vval.v_کام->jv_refcount;
	    توڑو;
#اگر_ختم
	حالت VAR_CHANNEL:
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
	    مقصد->vval.v_channel = from->vval.v_channel;
	    اگر (مقصد->vval.v_channel != تہی)
		++مقصد->vval.v_channel->ch_refcount;
	    توڑو;
#اگر_ختم
	حالت VAR_INSTR:
	    مقصد->vval.v_instr = from->vval.v_instr;
	    توڑو;

	حالت VAR_CLASS:
	    copy_class(from, مقصد);
	    توڑو;

	حالت VAR_OBJECT:
	    copy_object(from, مقصد);
	    توڑو;

	حالت VAR_STRING:
	حالت VAR_FUNC:
	    اگر (from->vval.v_string == تہی)
		مقصد->vval.v_string = تہی;
	    ورنہ
	    {
		مقصد->vval.v_string = vim_strsave(from->vval.v_string);
		اگر (from->v_type == VAR_FUNC)
		    func_ref(مقصد->vval.v_string);
	    }
	    توڑو;
	حالت VAR_PARTIAL:
	    اگر (from->vval.v_partial == تہی)
		مقصد->vval.v_partial = تہی;
	    ورنہ
	    {
		مقصد->vval.v_partial = from->vval.v_partial;
		++مقصد->vval.v_partial->pt_refcount;
	    }
	    توڑو;
	حالت VAR_BLOB:
	    اگر (from->vval.v_blob == تہی)
		مقصد->vval.v_blob = تہی;
	    ورنہ
	    {
		مقصد->vval.v_blob = from->vval.v_blob;
		++مقصد->vval.v_blob->bv_refcount;
	    }
	    توڑو;
	حالت VAR_LIST:
	    اگر (from->vval.v_list == تہی)
		مقصد->vval.v_list = تہی;
	    ورنہ
	    {
		مقصد->vval.v_list = from->vval.v_list;
		++مقصد->vval.v_list->lv_refcount;
	    }
	    توڑو;
	حالت VAR_TUPLE:
	    اگر (from->vval.v_tuple == تہی)
		مقصد->vval.v_tuple = تہی;
	    ورنہ
	    {
		مقصد->vval.v_tuple = from->vval.v_tuple;
		++مقصد->vval.v_tuple->tv_refcount;
	    }
	    توڑو;
	حالت VAR_DICT:
	    اگر (from->vval.v_dict == تہی)
		مقصد->vval.v_dict = تہی;
	    ورنہ
	    {
		مقصد->vval.v_dict = from->vval.v_dict;
		++مقصد->vval.v_dict->dv_refcount;
	    }
	    توڑو;
	حالت VAR_TYPEALIAS:
	    اگر (from->vval.v_typealias == تہی)
		مقصد->vval.v_typealias = تہی;
	    ورنہ
	    {
		مقصد->vval.v_typealias = from->vval.v_typealias;
		++مقصد->vval.v_typealias->ta_refcount;
	    }
	    توڑو;
	حالت VAR_VOID:
	    emsg(_(e_cannot_use_void_قدر));
	    ret = FAIL;
	    توڑو;
	حالت VAR_UNKNOWN:
	حالت VAR_ANY:
	    اندرونی_error_no_abort("copy_tv(UNKNOWN)");
	    ret = FAIL;
	    توڑو;
    }

    واپس ret;
}

/*
 * Compare "tv1" and "tv2".
 */
    صحیح
typval_compare2(
    typval_T	*tv1,	// first operand
    typval_T	*tv2,	// second operand
    exprtype_T	type,	// operator
    صحیح		ic,	// ignore case
    صحیح		*res)	// comparison result
{
    varnumber_T	n1, n2;
    صحیح		type_is = type == اظہاریہ_IS || type == اظہاریہ_ISNOT;

    اگر (check_typval_is_قدر(tv1) == FAIL
	|| check_typval_is_قدر(tv2) == FAIL)
	واپس FAIL;
    ورنہ اگر (type_is && tv1->v_type != tv2->v_type)
    {
	// For "is" a different type always means FALSE, for "isnot"
	// it means TRUE.
	*res = (type == اظہاریہ_ISNOT);
    }
    ورنہ اگر (((tv1->v_type == VAR_SPECIAL && tv1->vval.v_نمبر == VVAL_NULL)
		|| (tv2->v_type == VAR_SPECIAL
					   && tv2->vval.v_نمبر == VVAL_NULL))
	    && tv1->v_type != tv2->v_type
	    && (type == اظہاریہ_EQUAL || type == اظہاریہ_NEQUAL))
    {
	n1 = typval_compare_null(tv1, tv2);
	اگر (n1 == MAYBE)
	    واپس FAIL;
	اگر (type == اظہاریہ_NEQUAL)
	    n1 = !n1;
	*res = n1;
    }
    ورنہ اگر (tv1->v_type == VAR_BLOB || tv2->v_type == VAR_BLOB)
    {
	اگر (typval_compare_blob(tv1, tv2, type, res) == FAIL)
	    واپس FAIL;
    }
    ورنہ اگر (tv1->v_type == VAR_LIST || tv2->v_type == VAR_LIST)
    {
	اگر (typval_compare_list(tv1, tv2, type, ic, res) == FAIL)
	    واپس FAIL;
    }
    ورنہ اگر (tv1->v_type == VAR_TUPLE || tv2->v_type == VAR_TUPLE)
    {
	اگر (typval_compare_tuple(tv1, tv2, type, ic, res) == FAIL)
	    واپس FAIL;
    }
    ورنہ اگر (tv1->v_type == VAR_OBJECT || tv2->v_type == VAR_OBJECT)
    {
	اگر (typval_compare_object(tv1, tv2, type, ic, res) == FAIL)
	    واپس FAIL;
    }
    ورنہ اگر (tv1->v_type == VAR_DICT || tv2->v_type == VAR_DICT)
    {
	اگر (typval_compare_dict(tv1, tv2, type, ic, res) == FAIL)
	    واپس FAIL;
    }
    ورنہ اگر (tv1->v_type == VAR_FUNC || tv2->v_type == VAR_FUNC
	|| tv1->v_type == VAR_PARTIAL || tv2->v_type == VAR_PARTIAL)
    {
	اگر (typval_compare_func(tv1, tv2, type, ic, res) == FAIL)
	    واپس FAIL;
    }

    // If one of the two variables is a float, compare as a float.
    // When using "=~" or "!~", always compare as string.
    ورنہ اگر ((tv1->v_type == VAR_FLOAT || tv2->v_type == VAR_FLOAT)
	    && type != اظہاریہ_مطابقت && type != اظہاریہ_NOMATCH)
    {
	float_T f1, f2;
	صحیح	error = FALSE;

	f1 = tv_get_float_chk(tv1, &error);
	اگر (!error)
	    f2 = tv_get_float_chk(tv2, &error);
	اگر (error)
	    واپس FAIL;
	n1 = FALSE;
	انتخاب (type)
	{
	    حالت اظہاریہ_IS:
	    حالت اظہاریہ_EQUAL:    n1 = (f1 == f2); توڑو;
	    حالت اظہاریہ_ISNOT:
	    حالت اظہاریہ_NEQUAL:   n1 = (f1 != f2); توڑو;
	    حالت اظہاریہ_GREATER:  n1 = (f1 > f2); توڑو;
	    حالت اظہاریہ_GEQUAL:   n1 = (f1 >= f2); توڑو;
	    حالت اظہاریہ_SMALLER:  n1 = (f1 < f2); توڑو;
	    حالت اظہاریہ_SEQUAL:   n1 = (f1 <= f2); توڑو;
	    حالت اظہاریہ_UNKNOWN:
	    حالت اظہاریہ_مطابقت:
	    طےشدہ:  توڑو;  // avoid gcc warning
	}
	*res = n1;
    }

    // If one of the two variables is a number, compare as a number.
    // When using "=~" or "!~", always compare as string.
    ورنہ اگر ((tv1->v_type == VAR_نمبر || tv2->v_type == VAR_نمبر)
	    && type != اظہاریہ_مطابقت && type != اظہاریہ_NOMATCH)
    {
	صحیح error = FALSE;

	n1 = tv_get_نمبر_chk(tv1, &error);
	اگر (!error)
	    n2 = tv_get_نمبر_chk(tv2, &error);
	اگر (error)
	    واپس FAIL;
	انتخاب (type)
	{
	    حالت اظہاریہ_IS:
	    حالت اظہاریہ_EQUAL:    n1 = (n1 == n2); توڑو;
	    حالت اظہاریہ_ISNOT:
	    حالت اظہاریہ_NEQUAL:   n1 = (n1 != n2); توڑو;
	    حالت اظہاریہ_GREATER:  n1 = (n1 > n2); توڑو;
	    حالت اظہاریہ_GEQUAL:   n1 = (n1 >= n2); توڑو;
	    حالت اظہاریہ_SMALLER:  n1 = (n1 < n2); توڑو;
	    حالت اظہاریہ_SEQUAL:   n1 = (n1 <= n2); توڑو;
	    حالت اظہاریہ_UNKNOWN:
	    حالت اظہاریہ_مطابقت:
	    طےشدہ:  توڑو;  // avoid gcc warning
	}
	*res = n1;
    }
    ورنہ اگر (اندر_vim9script() && (tv1->v_type == VAR_BOOL
				    || tv2->v_type == VAR_BOOL
				    || (tv1->v_type == VAR_SPECIAL
					      && tv2->v_type == VAR_SPECIAL)))
    {
	اگر (tv1->v_type != tv2->v_type)
	{
	    semsg(_(e_cannot_compare_str_with_str),
		       vartype_name(tv1->v_type), vartype_name(tv2->v_type));
	    واپس FAIL;
	}
	n1 = tv1->vval.v_نمبر;
	n2 = tv2->vval.v_نمبر;
	انتخاب (type)
	{
	    حالت اظہاریہ_IS:
	    حالت اظہاریہ_EQUAL:    n1 = (n1 == n2); توڑو;
	    حالت اظہاریہ_ISNOT:
	    حالت اظہاریہ_NEQUAL:   n1 = (n1 != n2); توڑو;
	    طےشدہ:
		semsg(_(e_invalid_operation_for_str),
						   vartype_name(tv1->v_type));
		واپس FAIL;
	}
	*res = n1;
    }
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
    ورنہ اگر (tv1->v_type == tv2->v_type
	    && (tv1->v_type == VAR_CHANNEL || tv1->v_type == VAR_کام)
	    && (type == اظہاریہ_NEQUAL || type == اظہاریہ_EQUAL))
    {
	اگر (tv1->v_type == VAR_CHANNEL)
	    n1 = tv1->vval.v_channel == tv2->vval.v_channel;
	ورنہ
	    n1 = tv1->vval.v_کام == tv2->vval.v_کام;
	اگر (type == اظہاریہ_NEQUAL)
	    n1 = !n1;
	*res = n1;
    }
#اگر_ختم
    ورنہ
    {
	اگر (typval_compare_string(tv1, tv2, type, ic, res) == FAIL)
	    واپس FAIL;
    }

    واپس OK;
}

/*
 * Compare "tv1" and "tv2".
 * Put the result in "tv1".  Caller should clear "tv2".
 */
    صحیح
typval_compare(
    typval_T	*tv1,	// first operand
    typval_T	*tv2,	// second operand
    exprtype_T	type,   // operator
    صحیح		ic)     // ignore case
{
    صحیح res;

    اگر (typval_compare2(tv1, tv2, type, ic, &res) == FAIL)
    {
	clear_tv(tv1);
	واپس FAIL;
    }

    clear_tv(tv1);
    اگر (اندر_vim9script())
    {
	tv1->v_type = VAR_BOOL;
	tv1->vval.v_نمبر = res ? VVAL_TRUE : VVAL_FALSE;
    }
    ورنہ
    {
	tv1->v_type = VAR_نمبر;
	tv1->vval.v_نمبر = res;
    }

    واپس OK;
}

/*
 * Compare "tv1" to "tv2" as lists according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    صحیح
typval_compare_list(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	صحیح	    ic,
	صحیح	    *res)
{
    صحیح	    val = 0;

    اگر (type == اظہاریہ_IS || type == اظہاریہ_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
				      && tv1->vval.v_list == tv2->vval.v_list);
	اگر (type == اظہاریہ_ISNOT)
	    val = !val;
    }
    ورنہ اگر (tv1->v_type != tv2->v_type
	    || (type != اظہاریہ_EQUAL && type != اظہاریہ_NEQUAL))
    {
	اگر (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_list_with_list));
	ورنہ
	    emsg(_(e_invalid_operation_for_list));
	واپس FAIL;
    }
    ورنہ
    {
	val = list_equal(tv1->vval.v_list, tv2->vval.v_list, ic);
	اگر (type == اظہاریہ_NEQUAL)
	    val = !val;
    }
    *res = val;
    واپس OK;
}

/*
 * Compare "tv1" to "tv2" as tuples according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    صحیح
typval_compare_tuple(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	صحیح	    ic,
	صحیح	    *res)
{
    صحیح	    val = 0;

    اگر (type == اظہاریہ_IS || type == اظہاریہ_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
			      && tv1->vval.v_tuple == tv2->vval.v_tuple);
	اگر (type == اظہاریہ_ISNOT)
	    val = !val;
    }
    ورنہ اگر (tv1->v_type != tv2->v_type
	    || (type != اظہاریہ_EQUAL && type != اظہاریہ_NEQUAL))
    {
	اگر (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_tuple_with_tuple));
	ورنہ
	    emsg(_(e_invalid_operation_for_tuple));
	واپس FAIL;
    }
    ورنہ
    {
	val = tuple_equal(tv1->vval.v_tuple, tv2->vval.v_tuple, ic);
	اگر (type == اظہاریہ_NEQUAL)
	    val = !val;
    }
    *res = val;
    واپس OK;
}

/*
 * Compare v:null with another type.  Return TRUE if the value is NULL.
 */
    صحیح
typval_compare_null(typval_T *tv1, typval_T *tv2)
{
    اگر ((tv1->v_type == VAR_SPECIAL && tv1->vval.v_نمبر == VVAL_NULL)
	    || (tv2->v_type == VAR_SPECIAL && tv2->vval.v_نمبر == VVAL_NULL))
    {
	typval_T	*tv = tv1->v_type == VAR_SPECIAL ? tv2 : tv1;

	انتخاب (tv->v_type)
	{
	    حالت VAR_BLOB: واپس tv->vval.v_blob == تہی;
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
	    حالت VAR_CHANNEL: واپس tv->vval.v_channel == تہی;
#اگر_ختم
	    // TODO: null_class handling
	    // case VAR_CLASS: return tv->vval.v_class == NULL;
	    حالت VAR_DICT: واپس tv->vval.v_dict == تہی;
	    حالت VAR_FUNC: واپس tv->vval.v_string == تہی;
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
	    حالت VAR_کام: واپس tv->vval.v_کام == تہی;
#اگر_ختم
	    حالت VAR_LIST: واپس tv->vval.v_list == تہی;
	    حالت VAR_TUPLE: واپس tv->vval.v_tuple == تہی;
	    حالت VAR_OBJECT: واپس tv->vval.v_object == تہی;
	    حالت VAR_PARTIAL: واپس tv->vval.v_partial == تہی;
	    حالت VAR_STRING: واپس tv->vval.v_string == تہی;

	    حالت VAR_نمبر: اگر (!اندر_vim9script())
				 واپس tv->vval.v_نمبر == 0;
			     توڑو;
	    حالت VAR_FLOAT: اگر (!اندر_vim9script())
				 واپس tv->vval.v_float == 0.0;
			     توڑو;
	    حالت VAR_TYPEALIAS: واپس tv->vval.v_typealias == تہی;
	    طےشدہ: توڑو;
	}
    }
    // although comparing null with number, float or bool is not very useful
    // we won't give an error
    واپس FALSE;
}

/*
 * Compare "tv1" to "tv2" as blobs according to "type".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    صحیح
typval_compare_blob(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	صحیح	    *res)
{
    صحیح	    val = 0;

    اگر (type == اظہاریہ_IS || type == اظہاریہ_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
			&& tv1->vval.v_blob == tv2->vval.v_blob);
	اگر (type == اظہاریہ_ISNOT)
	    val = !val;
    }
    ورنہ اگر (tv1->v_type != tv2->v_type
	    || (type != اظہاریہ_EQUAL && type != اظہاریہ_NEQUAL))
    {
	اگر (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_blob_with_blob));
	ورنہ
	    emsg(_(e_invalid_operation_for_blob));
	واپس FAIL;
    }
    ورنہ
    {
	val = blob_equal(tv1->vval.v_blob, tv2->vval.v_blob);
	اگر (type == اظہاریہ_NEQUAL)
	    val = !val;
    }
    *res = val;
    واپس OK;
}

/*
 * Compare "tv1" to "tv2" as objects according to "type".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    صحیح
typval_compare_object(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	صحیح	    ic,
	صحیح	    *res)
{
    صحیح res_مطابقت = type == اظہاریہ_EQUAL || type == اظہاریہ_IS ? TRUE : FALSE;

    اگر (tv1->vval.v_object == تہی && tv2->vval.v_object == تہی)
    {
	*res = res_مطابقت;
	واپس OK;
    }
    اگر (tv1->vval.v_object == تہی || tv2->vval.v_object == تہی)
    {
	*res = !res_مطابقت;
	واپس OK;
    }

    object_T *obj1 = tv1->vval.v_object;
    object_T *obj2 = tv2->vval.v_object;
    اگر (type == اظہاریہ_IS || type == اظہاریہ_ISNOT)
    {
	*res = obj1 == obj2 ? res_مطابقت : !res_مطابقت;
	واپس OK;
    }
    ورنہ اگر (tv1->v_type != tv2->v_type)
    {
	emsg(_(e_can_only_compare_object_with_object));
	واپس FAIL;
    }

    *res = object_equal(obj1, obj2, ic) ? res_مطابقت : !res_مطابقت;
    واپس OK;
}

/*
 * Compare "tv1" to "tv2" as dictionaries according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    صحیح
typval_compare_dict(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	صحیح	    ic,
	صحیح	    *res)
{
    صحیح	    val;

    اگر (type == اظہاریہ_IS || type == اظہاریہ_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
			&& tv1->vval.v_dict == tv2->vval.v_dict);
	اگر (type == اظہاریہ_ISNOT)
	    val = !val;
    }
    ورنہ اگر (tv1->v_type != tv2->v_type
		|| (type != اظہاریہ_EQUAL && type != اظہاریہ_NEQUAL))
    {
	اگر (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_dictionary_with_dictionary));
	ورنہ
	    emsg(_(e_invalid_operation_for_dictionary));
	واپس FAIL;
    }
    ورنہ
    {
	val = dict_equal(tv1->vval.v_dict, tv2->vval.v_dict, ic);
	اگر (type == اظہاریہ_NEQUAL)
	    val = !val;
    }
    *res = val;
    واپس OK;
}

/*
 * Compare "tv1" to "tv2" as funcrefs according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    صحیح
typval_compare_func(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	صحیح	    ic,
	صحیح	    *res)
{
    صحیح	    val = 0;

    اگر (type != اظہاریہ_EQUAL && type != اظہاریہ_NEQUAL
	    && type != اظہاریہ_IS && type != اظہاریہ_ISNOT)
    {
	emsg(_(e_invalid_operation_for_funcrefs));
	واپس FAIL;
    }
    اگر ((tv1->v_type == VAR_PARTIAL && tv1->vval.v_partial == تہی)
	    || (tv2->v_type == VAR_PARTIAL && tv2->vval.v_partial == تہی))
	// When both partials are NULL, then they are equal.
	// Otherwise they are not equal.
	val = (tv1->vval.v_partial == tv2->vval.v_partial);
    ورنہ اگر (type == اظہاریہ_IS || type == اظہاریہ_ISNOT)
    {
	اگر (tv1->v_type == VAR_FUNC && tv2->v_type == VAR_FUNC)
	    // strings are considered the same if their value is
	    // the same
	    val = tv_equal(tv1, tv2, ic);
	ورنہ اگر (tv1->v_type == VAR_PARTIAL && tv2->v_type == VAR_PARTIAL)
	    val = (tv1->vval.v_partial == tv2->vval.v_partial);
	ورنہ
	    val = FALSE;
    }
    ورنہ
	val = tv_equal(tv1, tv2, ic);
    اگر (type == اظہاریہ_NEQUAL || type == اظہاریہ_ISNOT)
	val = !val;
    *res = val;
    واپس OK;
}

/*
 * Compare "tv1" to "tv2" as strings according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    صحیح
typval_compare_string(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	صحیح	    ic,
	صحیح	    *res)
{
    صحیح		i = 0;
    صحیح		val = FALSE;
    char_u	*s1, *s2;
    char_u	buf1[NUMBUFLEN], buf2[NUMBUFLEN];

    اگر (اندر_vim9script()
	  && ((tv1->v_type != VAR_STRING && tv1->v_type != VAR_SPECIAL)
	   || (tv2->v_type != VAR_STRING && tv2->v_type != VAR_SPECIAL)))
    {
	semsg(_(e_cannot_compare_str_with_str),
		   vartype_name(tv1->v_type), vartype_name(tv2->v_type));
	واپس FAIL;
    }
    s1 = tv_get_string_buf(tv1, buf1);
    s2 = tv_get_string_buf(tv2, buf2);
    اگر (type != اظہاریہ_مطابقت && type != اظہاریہ_NOMATCH)
	i = ic ? MB_STRICMP(s1, s2) : STRCMP(s1, s2);
    انتخاب (type)
    {
	حالت اظہاریہ_IS:	    اگر (اندر_vim9script())
			    {
				// Really check it is the same string, not just
				// the same value.
				val = tv1->vval.v_string == tv2->vval.v_string;
				توڑو;
			    }
			    // FALLTHROUGH
	حالت اظہاریہ_EQUAL:    val = (i == 0); توڑو;
	حالت اظہاریہ_ISNOT:    اگر (اندر_vim9script())
			    {
				// Really check it is not the same string, not
				// just a different value.
				val = tv1->vval.v_string != tv2->vval.v_string;
				توڑو;
			    }
			    // FALLTHROUGH
	حالت اظہاریہ_NEQUAL:   val = (i != 0); توڑو;
	حالت اظہاریہ_GREATER:  val = (i > 0); توڑو;
	حالت اظہاریہ_GEQUAL:   val = (i >= 0); توڑو;
	حالت اظہاریہ_SMALLER:  val = (i < 0); توڑو;
	حالت اظہاریہ_SEQUAL:   val = (i <= 0); توڑو;

	حالت اظہاریہ_مطابقت:
	حالت اظہاریہ_NOMATCH:
		val = نمونہ_مطابقت(s2, s1, ic);
		اگر (type == اظہاریہ_NOMATCH)
		    val = !val;
		توڑو;

	طےشدہ:  توڑو;  // avoid gcc warning
    }
    *res = val;
    واپس OK;
}
/*
 * Convert any type to a string, never give an error.
 * When "quotes" is TRUE add quotes to a string.
 * Returns an allocated string.
 */
    char_u *
typval_tostring(typval_T *arg, صحیح quotes)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    char_u	*ret = تہی;

    اگر (arg == تہی)
	واپس vim_strsave((char_u *)"(does not exist)");
    اگر (!quotes && arg->v_type == VAR_STRING)
    {
	ret = vim_strsave(arg->vval.v_string == تہی ? (char_u *)""
							 : arg->vval.v_string);
    }
    ورنہ
    {
	ret = tv2string(arg, &tofree, numbuf, 0);
	// Make a copy if we have a value but it's not in allocated memory.
	اگر (ret != تہی && tofree == تہی)
	    ret = vim_strsave(ret);
    }
    واپس ret;
}

/*
 * Return TRUE if typeval "tv" is locked: Either that value is locked itself
 * or it refers to a List or Dictionary that is locked.
 */
    صحیح
tv_islocked(typval_T *tv)
{
    واپس (tv->v_تالا & VAR_LOCKED)
	|| (tv->v_type == VAR_LIST
		&& tv->vval.v_list != تہی
		&& (tv->vval.v_list->lv_تالا & VAR_LOCKED))
	|| (tv->v_type == VAR_TUPLE
		&& tv->vval.v_tuple != تہی
		&& (tv->vval.v_tuple->tv_تالا & VAR_LOCKED))
	|| (tv->v_type == VAR_DICT
		&& tv->vval.v_dict != تہی
		&& (tv->vval.v_dict->dv_تالا & VAR_LOCKED));
}

    ساکن صحیح
func_equal(
    typval_T *tv1,
    typval_T *tv2,
    صحیح	     ic)	    // ignore case
{
    char_u	*s1, *s2;
    dict_T	*d1, *d2;
    صحیح		a1, a2;
    صحیح		i;

    // empty and NULL function name considered the same
    s1 = tv1->v_type == VAR_FUNC ? tv1->vval.v_string
					   : partial_name(tv1->vval.v_partial);
    اگر (s1 != تہی && *s1 == NUL)
	s1 = تہی;
    s2 = tv2->v_type == VAR_FUNC ? tv2->vval.v_string
					   : partial_name(tv2->vval.v_partial);
    اگر (s2 != تہی && *s2 == NUL)
	s2 = تہی;
    اگر (s1 == تہی || s2 == تہی)
    {
	اگر (s1 != s2)
	    واپس FALSE;
    }
    ورنہ اگر (STRCMP(s1, s2) != 0)
	واپس FALSE;

    // empty dict and NULL dict is different
    d1 = tv1->v_type == VAR_FUNC ? تہی : tv1->vval.v_partial->pt_dict;
    d2 = tv2->v_type == VAR_FUNC ? تہی : tv2->vval.v_partial->pt_dict;
    اگر (d1 == تہی || d2 == تہی)
    {
	اگر (d1 != d2)
	    واپس FALSE;
    }
    ورنہ اگر (!dict_equal(d1, d2, ic))
	واپس FALSE;

    // empty list and no list considered the same
    a1 = tv1->v_type == VAR_FUNC ? 0 : tv1->vval.v_partial->pt_argc;
    a2 = tv2->v_type == VAR_FUNC ? 0 : tv2->vval.v_partial->pt_argc;
    اگر (a1 != a2)
	واپس FALSE;
    برائے (i = 0; i < a1; ++i)
	اگر (!tv_equal(tv1->vval.v_partial->pt_argv + i,
		      tv2->vval.v_partial->pt_argv + i, ic))
	    واپس FALSE;

    واپس TRUE;
}

/*
 * Return TRUE if "tv1" and "tv2" have the same value.
 * Compares the items just like "==" would compare them, but strings and
 * numbers are different.  Floats and numbers are also different.
 */
    صحیح
tv_equal(
    typval_T *tv1,
    typval_T *tv2,
    صحیح	     ic)	    // ignore case
{
    char_u	buf1[NUMBUFLEN], buf2[NUMBUFLEN];
    char_u	*s1, *s2;
    ساکن صحیح  recursive_cnt = 0;	    // catch recursive loops
    صحیح		r;
    ساکن صحیح	tv_equal_recurse_حد_بندی;

    // Catch lists and dicts that have an endless loop by limiting
    // recursiveness to a limit.  We guess they are equal then.
    // A fixed limit has the problem of still taking an awful long time.
    // Reduce the limit every time running into it. That should work fine for
    // deeply linked structures that are not recursively linked and catch
    // recursiveness quickly.
    اگر (recursive_cnt == 0)
	tv_equal_recurse_حد_بندی = 1000;
    اگر (recursive_cnt >= tv_equal_recurse_حد_بندی)
    {
	--tv_equal_recurse_حد_بندی;
	واپس TRUE;
    }

    // For VAR_FUNC and VAR_PARTIAL compare the function name, bound dict and
    // arguments.
    اگر ((tv1->v_type == VAR_FUNC
		|| (tv1->v_type == VAR_PARTIAL && tv1->vval.v_partial != تہی))
	    && (tv2->v_type == VAR_FUNC
		|| (tv2->v_type == VAR_PARTIAL && tv2->vval.v_partial != تہی)))
    {
	++recursive_cnt;
	r = func_equal(tv1, tv2, ic);
	--recursive_cnt;
	واپس r;
    }

    اگر (tv1->v_type != tv2->v_type
	    && ((tv1->v_type != VAR_BOOL && tv1->v_type != VAR_SPECIAL)
		|| (tv2->v_type != VAR_BOOL && tv2->v_type != VAR_SPECIAL)))
	واپس FALSE;

    انتخاب (tv1->v_type)
    {
	حالت VAR_LIST:
	    ++recursive_cnt;
	    r = list_equal(tv1->vval.v_list, tv2->vval.v_list, ic);
	    --recursive_cnt;
	    واپس r;

	حالت VAR_TUPLE:
	    ++recursive_cnt;
	    r = tuple_equal(tv1->vval.v_tuple, tv2->vval.v_tuple, ic);
	    --recursive_cnt;
	    واپس r;

	حالت VAR_DICT:
	    ++recursive_cnt;
	    r = dict_equal(tv1->vval.v_dict, tv2->vval.v_dict, ic);
	    --recursive_cnt;
	    واپس r;

	حالت VAR_BLOB:
	    واپس blob_equal(tv1->vval.v_blob, tv2->vval.v_blob);

	حالت VAR_نمبر:
	حالت VAR_BOOL:
	حالت VAR_SPECIAL:
	    واپس tv1->vval.v_نمبر == tv2->vval.v_نمبر;

	حالت VAR_STRING:
	    s1 = tv_get_string_buf(tv1, buf1);
	    s2 = tv_get_string_buf(tv2, buf2);
	    واپس ((ic ? MB_STRICMP(s1, s2) : STRCMP(s1, s2)) == 0);

	حالت VAR_FLOAT:
	    واپس tv1->vval.v_float == tv2->vval.v_float;
	حالت VAR_کام:
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
	    واپس tv1->vval.v_کام == tv2->vval.v_کام;
#اگر_ختم
	حالت VAR_CHANNEL:
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
	    واپس tv1->vval.v_channel == tv2->vval.v_channel;
#اگر_ختم
	حالت VAR_INSTR:
	    واپس tv1->vval.v_instr == tv2->vval.v_instr;

	حالت VAR_CLASS:
	    // A class only exists once, equality is identity.
	    واپس tv1->vval.v_class == tv2->vval.v_class;

	حالت VAR_OBJECT:
	    ++recursive_cnt;
	    r = object_equal(tv1->vval.v_object, tv2->vval.v_object, ic);
	    --recursive_cnt;
	    واپس r;

	حالت VAR_PARTIAL:
	    واپس tv1->vval.v_partial == tv2->vval.v_partial;

	حالت VAR_FUNC:
	    واپس tv1->vval.v_string == tv2->vval.v_string;

	حالت VAR_TYPEALIAS:
	    واپس tv1->vval.v_typealias == tv2->vval.v_typealias;

	حالت VAR_UNKNOWN:
	حالت VAR_ANY:
	حالت VAR_VOID:
	    توڑو;
    }

    // VAR_UNKNOWN can be the result of a invalid expression, let's say it
    // does not equal anything, not even itself.
    واپس FALSE;
}

/*
 * Get an option value.
 * "arg" points to the '&' or '+' before the option name.
 * "arg" is advanced to character after the option name.
 * Return OK or FAIL.
 */
    صحیح
eval_option(
    char_u	**arg,
    typval_T	*rettv,	// when NULL, only check if option exists
    صحیح		evaluate)
{
    char_u	*option_انتہا;
    طویل	numval;
    char_u	*stringval;
    getoption_T	opt_type;
    صحیح		c;
    صحیح		working = (**arg == '+');    // has("+option")
    صحیح		ret = OK;
    صحیح		scope;

    // Isolate the option name and find its value.
    option_انتہا = ڈھونڈو_option_انتہا(arg, &scope);
    اگر (option_انتہا == تہی)
    {
	اگر (rettv != تہی)
	    semsg(_(e_option_name_missing_str), *arg);
	واپس FAIL;
    }

    اگر (!evaluate)
    {
	*arg = option_انتہا;
	واپس OK;
    }

    c = *option_انتہا;
    *option_انتہا = NUL;
    opt_type = get_option_قدر(*arg, &numval,
			       rettv == تہی ? تہی : &stringval, تہی, scope);

    اگر (opt_type == gov_unknown)
    {
	اگر (rettv != تہی)
	    semsg(_(e_unknown_option_str), *arg);
	ret = FAIL;
    }
    ورنہ اگر (rettv != تہی)
    {
	rettv->v_تالا = 0;
	اگر (opt_type == gov_hidden_string)
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = تہی;
	}
	ورنہ اگر (opt_type == gov_hidden_bool || opt_type == gov_hidden_نمبر)
	{
	    rettv->v_type = اندر_vim9script() && opt_type == gov_hidden_bool
						       ? VAR_BOOL : VAR_نمبر;
	    rettv->vval.v_نمبر = 0;
	}
	ورنہ اگر (opt_type == gov_bool || opt_type == gov_نمبر)
	{
	    اگر (اندر_vim9script() && opt_type == gov_bool)
	    {
		rettv->v_type = VAR_BOOL;
		rettv->vval.v_نمبر = numval ? VVAL_TRUE : VVAL_FALSE;
	    }
	    ورنہ
	    {
		rettv->v_type = VAR_نمبر;
		rettv->vval.v_نمبر = numval;
	    }
	}
	ورنہ				// string option
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = stringval;
	}
    }
    ورنہ اگر (working && (opt_type == gov_hidden_bool
			|| opt_type == gov_hidden_نمبر
			|| opt_type == gov_hidden_string))
	ret = FAIL;

    *option_انتہا = c;		    // put back for error messages
    *arg = option_انتہا;

    واپس ret;
}

/*
 * Allocate a variable for a number constant.  Also deals with "0z" for blob.
 * Return OK or FAIL.
 */
    صحیح
eval_نمبر(
	char_u	    **arg,
	typval_T    *rettv,
	صحیح	    evaluate,
	صحیح	    want_string)
{
    صحیح		len;
    صحیح		skip_quotes = !اندر_old_script(4);
    char_u	*p;
    صحیح		get_float = FALSE;

    // We accept a float when the format matches
    // "[0-9]\+\.[0-9]\+\([eE][+-]\?[0-9]\+\)\?".  This is very
    // strict to avoid backwards compatibility problems.
    // With script version 2 and later the leading digit can be
    // omitted.
    // Don't look for a float after the "." operator, so that
    // ":let vers = 1.2.3" doesn't fail.
    اگر (**arg == '.')
	p = *arg;
    ورنہ
    {
	p = *arg + 1;
	اگر (skip_quotes)
	    برائے (;;)
	    {
		اگر (*p == '\'')
		    ++p;
		اگر (!vim_isdigit(*p))
		    توڑو;
		p = skipdigits(p);
	    }
	ورنہ
	    p = skipdigits(p);
    }
    اگر (!want_string && p[0] == '.' && vim_isdigit(p[1]))
    {
	get_float = TRUE;
	p = skipdigits(p + 2);
	اگر (*p == 'e' || *p == 'E')
	{
	    ++p;
	    اگر (*p == '-' || *p == '+')
		++p;
	    اگر (!vim_isdigit(*p))
		get_float = FALSE;
	    ورنہ
		p = skipdigits(p + 1);
	}
	اگر (ASCII_ISALPHA(*p) || *p == '.')
	    get_float = FALSE;
    }
    اگر (get_float)
    {
	float_T	f;

	*arg += string2float(*arg, &f, skip_quotes);
	اگر (evaluate)
	{
	    rettv->v_type = VAR_FLOAT;
	    rettv->vval.v_float = f;
	}
    }
    ورنہ
    اگر (**arg == '0' && ((*arg)[1] == 'z' || (*arg)[1] == 'Z'))
    {
	char_u  *bp;
	blob_T  *blob = تہی;  // init for gcc

	// Blob constant: 0z0123456789abcdef
	اگر (evaluate)
	    blob = blob_alloc();
	برائے (bp = *arg + 2; vim_isxdigit(bp[0]); bp += 2)
	{
	    اگر (!vim_isxdigit(bp[1]))
	    {
		اگر (blob != تہی)
		{
		    emsg(_(e_blob_literal_should_have_an_even_نمبر_کا_hex_characters));
		    ga_clear(&blob->bv_ga);
		    VIM_CLEAR(blob);
		}
		واپس FAIL;
	    }
	    اگر (blob != تہی)
		ga_append(&blob->bv_ga,
			     (hex2nr(*bp) << 4) + hex2nr(*(bp+1)));
	    اگر (bp[2] == '.' && vim_isxdigit(bp[3]))
		++bp;
	}
	اگر (blob != تہی)
	    rettv_blob_سیٹ(rettv, blob);
	*arg = bp;
    }
    ورنہ
    {
	varnumber_T	n;

	// decimal, hex or octal number
	vim_str2nr(*arg, تہی, &len, skip_quotes
		      ? STR2NR_NO_OCT + STR2NR_QUOTE
		      : STR2NR_ALL, &n, تہی, 0, TRUE, تہی);
	اگر (len == 0)
	{
	    اگر (evaluate)
		semsg(_(e_invalid_عبارت_str), *arg);
	    واپس FAIL;
	}
	*arg += len;
	اگر (evaluate)
	{
	    rettv->v_type = VAR_نمبر;
	    rettv->vval.v_نمبر = n;
	}
    }
    واپس OK;
}

/*
 * Evaluate a string constant and put the result in "rettv".
 * "*arg" points to the double quote or to after it when "interpolate" is TRUE.
 * When "interpolate" is TRUE reduce "{{" to "{", reduce "}}" to "}" and stop
 * at a single "{".
 * Return OK or FAIL.
 */
    صحیح
eval_string(char_u **arg, typval_T *rettv, صحیح evaluate, صحیح interpolate)
{
    char_u	*p;
    char_u	*انتہا;
    صحیح		extra = interpolate ? 1 : 0;
    صحیح		off = interpolate ? 0 : 1;
    صحیح		len;

    // Find the end of the string, skipping backslashed characters.
    برائے (p = *arg + off; *p != NUL && *p != '"'; MB_PTR_ADV(p))
    {
	اگر (*p == '\\' && p[1] != NUL)
	{
	    ++p;
	    // A "\<x>" form occupies at least 4 characters, and produces up
	    // to 9 characters (6 for the char and 3 for a modifier):
	    // reserve space for 5 extra.
	    اگر (*p == '<')
	    {
		صحیح		modifiers = 0;
		صحیح		flags = FSK_KEYCODE | FSK_اندر_STRING;

		extra += 5;

		// Skip to the '>' to avoid using '{' inside for string
		// interpolation.
		اگر (p[1] != '*')
		    flags |= FSK_SIMPLIFY;
		اگر (ڈھونڈو_special_کلید(&p, &modifiers, flags, تہی) != 0)
		    --p;  // leave "p" on the ">"
	    }
	}
	ورنہ اگر (interpolate && (*p == '{' || *p == '}'))
	{
	    اگر (*p == '{' && p[1] != '{') // start of expression
		توڑو;
	    ++p;
	    اگر (p[-1] == '}' && *p != '}') // single '}' is an error
	    {
		semsg(_(e_stray_closing_curly_str), *arg);
		واپس FAIL;
	    }
	    --extra;  // "{{" becomes "{", "}}" becomes "}"
	}
    }

    اگر (*p != '"' && !(interpolate && *p == '{'))
    {
	semsg(_(e_missing_double_quote_str), *arg);
	واپس FAIL;
    }

    // If only parsing, set *arg and return here
    اگر (!evaluate)
    {
	*arg = p + off;
	واپس OK;
    }

    // Copy the string into allocated memory, handling backslashed
    // characters.
    rettv->v_type = VAR_STRING;
    len = (صحیح)(p - *arg + extra);
    rettv->vval.v_string = alloc(len);
    اگر (rettv->vval.v_string == تہی)
	واپس FAIL;
    انتہا = rettv->vval.v_string;

    برائے (p = *arg + off; *p != NUL && *p != '"'; )
    {
	اگر (*p == '\\')
	{
	    انتخاب (*++p)
	    {
		حالت 'b': *انتہا++ = BS; ++p; توڑو;
		حالت 'e': *انتہا++ = ESC; ++p; توڑو;
		حالت 'f': *انتہا++ = FF; ++p; توڑو;
		حالت 'n': *انتہا++ = NL; ++p; توڑو;
		حالت 'r': *انتہا++ = CAR; ++p; توڑو;
		حالت 't': *انتہا++ = TAB; ++p; توڑو;

		حالت 'X': // hex: "\x1", "\x12"
		حالت 'x':
		حالت 'u': // Unicode: "\u0023"
		حالت 'U':
			  اگر (vim_isxdigit(p[1]))
			  {
			      صحیح	n, nr;
			      صحیح	c = محفوظ_toupper(*p);

			      اگر (c == 'X')
				  n = 2;
			      ورنہ اگر (*p == 'u')
				  n = 4;
			      ورنہ
				  n = 8;
			      nr = 0;
			      جب (--n >= 0 && vim_isxdigit(p[1]))
			      {
				  ++p;
				  nr = (nr << 4) + hex2nr(*p);
			      }
			      ++p;
			      // For "\u" store the number according to
			      // 'encoding'.
			      اگر (c != 'X')
				  انتہا += (*mb_char2bytes)(nr, انتہا);
			      ورنہ
				  *انتہا++ = nr;
			  }
			  توڑو;

			  // octal: "\1", "\12", "\123"
		حالت '0':
		حالت '1':
		حالت '2':
		حالت '3':
		حالت '4':
		حالت '5':
		حالت '6':
		حالت '7': *انتہا = *p++ - '0';
			  اگر (*p >= '0' && *p <= '7')
			  {
			      *انتہا = (*انتہا << 3) + *p++ - '0';
			      اگر (*p >= '0' && *p <= '7')
				  *انتہا = (*انتہا << 3) + *p++ - '0';
			  }
			  ++انتہا;
			  توڑو;

			  // Special key, e.g.: "\<C-W>"
		حالت '<':
			  {
			      صحیح flags = FSK_KEYCODE | FSK_اندر_STRING;

			      اگر (p[1] != '*')
				  flags |= FSK_SIMPLIFY;
			      extra = trans_special(&p, انتہا, flags, FALSE, تہی);
			      اگر (extra != 0)
			      {
				  انتہا += extra;
				  اگر (انتہا >= rettv->vval.v_string + len)
				      iemsg("eval_string() used more space than allocated");
				  توڑو;
			      }
			  }
			  // FALLTHROUGH

		طےشدہ: MB_COPY_CHAR(p, انتہا);
			  توڑو;
	    }
	}
	ورنہ
	{
	    اگر (interpolate && (*p == '{' || *p == '}'))
	    {
		اگر (*p == '{' && p[1] != '{') // start of expression
		    توڑو;
		++p;  // reduce "{{" to "{" and "}}" to "}"
	    }
	    MB_COPY_CHAR(p, انتہا);
	}
    }
    *انتہا = NUL;
    اگر (*p == '"' && !interpolate)
	++p;
    *arg = p;

    واپس OK;
}

/*
 * Allocate a variable for a 'str''ing' constant.
 * When "interpolate" is TRUE reduce "{{" to "{" and stop at a single "{".
 * Return OK when a "rettv" was set to the string.
 * Return FAIL on error, "rettv" is not set.
 */
    صحیح
eval_lit_string(char_u **arg, typval_T *rettv, صحیح evaluate, صحیح interpolate)
{
    char_u	*p;
    char_u	*str;
    صحیح		reduce = interpolate ? -1 : 0;
    صحیح		off = interpolate ? 0 : 1;

    // Find the end of the string, skipping ''.
    برائے (p = *arg + off; *p != NUL; MB_PTR_ADV(p))
    {
	اگر (*p == '\'')
	{
	    اگر (p[1] != '\'')
		توڑو;
	    ++reduce;
	    ++p;
	}
	ورنہ اگر (interpolate)
	{
	    اگر (*p == '{')
	    {
		اگر (p[1] != '{')
		    توڑو;
		++p;
		++reduce;
	    }
	    ورنہ اگر (*p == '}')
	    {
		++p;
		اگر (*p != '}')
		{
		    semsg(_(e_stray_closing_curly_str), *arg);
		    واپس FAIL;
		}
		++reduce;
	    }
	}
    }

    اگر (*p != '\'' && !(interpolate && *p == '{'))
    {
	semsg(_(e_missing_single_quote_str), *arg);
	واپس FAIL;
    }

    // If only parsing return after setting "*arg"
    اگر (!evaluate)
    {
	*arg = p + off;
	واپس OK;
    }

    // Copy the string into allocated memory, handling '' to ' reduction and
    // any expressions.
    str = alloc((p - *arg) - reduce);
    اگر (str == تہی)
	واپس FAIL;
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = str;

    برائے (p = *arg + off; *p != NUL; )
    {
	اگر (*p == '\'')
	{
	    اگر (p[1] != '\'')
		توڑو;
	    ++p;
	}
	ورنہ اگر (interpolate && (*p == '{' || *p == '}'))
	{
	    اگر (*p == '{' && p[1] != '{')
		توڑو;
	    ++p;
	}
	MB_COPY_CHAR(p, str);
    }
    *str = NUL;
    *arg = p + off;

    واپس OK;
}

/*
 * Evaluate a single or double quoted string possibly containing expressions.
 * "arg" points to the '$'.  The result is put in "rettv".
 * Returns OK or FAIL.
 */
    صحیح
eval_interp_string(char_u **arg, typval_T *rettv, صحیح evaluate)
{
    typval_T	tv;
    صحیح		ret = OK;
    صحیح		quote;
    garray_T	ga;
    char_u	*p;

    ga_init2(&ga, 1, 80);

    // *arg is on the '$' character, move it to the first string character.
    ++*arg;
    quote = **arg;
    ++*arg;

    برائے (;;)
    {
	// Get the string up to the matching quote or to a single '{'.
	// "arg" is advanced to either the quote or the '{'.
	اگر (quote == '"')
	    ret = eval_string(arg, &tv, evaluate, TRUE);
	ورنہ
	    ret = eval_lit_string(arg, &tv, evaluate, TRUE);
	اگر (ret == FAIL)
	    توڑو;
	اگر (evaluate)
	{
	    ga_concat(&ga, tv.vval.v_string);
	    clear_tv(&tv);
	}

	اگر (**arg != '{')
	{
	    // found terminating quote
	    ++*arg;
	    توڑو;
	}
	p = eval_one_اظہاریہ_اندر_str(*arg, &ga, evaluate);
	اگر (p == تہی)
	{
	    ret = FAIL;
	    توڑو;
	}
	*arg = p;
    }

    rettv->v_type = VAR_STRING;
    اگر (ret == FAIL || !evaluate || ga_append(&ga, NUL) == FAIL)
    {
	ga_clear(&ga);
	rettv->vval.v_string = تہی;
	واپس ret;
    }

    rettv->vval.v_string = ga.ga_ڈیٹا;
    واپس OK;
}

/*
 * Return a string with the string representation of a variable.
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * "numbuf" is used for a number.
 * Puts quotes around strings, so that they can be parsed back by eval().
 * May return NULL.
 */
    char_u *
tv2string(
    typval_T	*tv,
    char_u	**tofree,
    char_u	*numbuf,
    صحیح		copyID)
{
    واپس echo_string_core(tv, tofree, numbuf, copyID, FALSE, TRUE, FALSE);
}

/*
 * Get the value of an environment variable.
 * "arg" is pointing to the '$'.  It is advanced to after the name.
 * If the environment variable was not set, silently assume it is empty.
 * Return FAIL if the name is invalid.
 */
    صحیح
eval_env_var(char_u **arg, typval_T *rettv, صحیح evaluate)
{
    char_u	*string = تہی;
    صحیح		len;
    صحیح		cc;
    char_u	*name;
    صحیح		mustfree = FALSE;

    ++*arg;
    name = *arg;
    len = get_env_len(arg);
    اگر (evaluate)
    {
	اگر (len == 0)
	    واپس FAIL; // invalid empty name

	cc = name[len];
	name[len] = NUL;
	// first try vim_getenv(), fast for normal environment vars
	string = vim_getenv(name, &mustfree);
	اگر (string != تہی && *string != NUL)
	{
	    اگر (!mustfree)
		string = vim_strsave(string);
	}
	ورنہ
	{
	    اگر (mustfree)
		vim_free(string);

	    // next try expanding things like $VIM and ${HOME}
	    string = expand_env_save(name - 1);
	    اگر (string != تہی && *string == '$')
		VIM_CLEAR(string);
	}
	name[len] = cc;

	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = string;
	rettv->v_تالا = 0;
    }

    واپس OK;
}

/*
 * Get the lnum from the first argument.
 * Also accepts ".", "$", etc., but that only works for the current buffer.
 * Returns -1 on error.
 */
    linenr_T
tv_get_lnum(typval_T *argvars)
{
    linenr_T	lnum = -1;
    صحیح		did_emsg_before = did_emsg;

    اگر (argvars[0].v_type != VAR_STRING || !اندر_vim9script())
	lnum = (linenr_T)tv_get_نمبر_chk(&argvars[0], تہی);
    اگر (lnum <= 0 && did_emsg_before == did_emsg
					    && argvars[0].v_type != VAR_نمبر)
    {
	صحیح	fnum;
	pos_T	*fp;

	// no valid number, try using arg like line()
	fp = var2fpos(&argvars[0], TRUE, &fnum, FALSE);
	اگر (fp != تہی)
	    lnum = fp->lnum;
    }
    واپس lnum;
}

/*
 * Get the lnum from the first argument.
 * Also accepts "$", then "buf" is used.
 * Returns 0 on error.
 */
    linenr_T
tv_get_lnum_buf(typval_T *argvars, buf_T *buf)
{
    اگر (argvars[0].v_type == VAR_STRING
	    && argvars[0].vval.v_string != تہی
	    && argvars[0].vval.v_string[0] == '$'
	    && argvars[0].vval.v_string[1] == NUL
	    && buf != تہی)
	واپس buf->b_ml.ml_سطر_گنتی;
    واپس (linenr_T)tv_get_نمبر_chk(&argvars[0], تہی);
}

/*
 * Get buffer by number or pattern.
 */
    buf_T *
tv_get_buf(typval_T *tv, صحیح curtab_only)
{
    char_u	*name = tv->vval.v_string;
    buf_T	*buf;

    اگر (tv->v_type == VAR_نمبر)
	واپس buflist_findnr((صحیح)tv->vval.v_نمبر);
    اگر (tv->v_type != VAR_STRING)
	واپس تہی;
    اگر (name == تہی || *name == NUL)
	واپس curbuf;
    اگر (name[0] == '$' && name[1] == NUL)
	واپس lastbuf;

    buf = buflist_ڈھونڈو_by_name(name, curtab_only);

    // If not found, try expanding the name, like done for bufexists().
    اگر (buf == تہی)
	buf = ڈھونڈو_بفر(tv);

    واپس buf;
}

/*
 * Like tv_get_buf() but give an error message is the type is wrong.
 */
    buf_T *
tv_get_buf_from_arg(typval_T *tv)
{
    buf_T *buf;

    ++emsg_off;
    buf = tv_get_buf(tv, FALSE);
    --emsg_off;
    اگر (buf == تہی
	    && tv->v_type != VAR_نمبر
	    && tv->v_type != VAR_STRING)
	// issue errmsg for type error
	(خالی)tv_get_نمبر(tv);
    واپس buf;
}

/*
 * Check that repeating "slen" items "n" times, does not
 * overflow the int used for the resulting length.
 */
    صحیح
check_repeat_گنتی(varnumber_T slen, varnumber_T n)
{
    // Use a division so that the multiplication cannot overflow.
    اگر (n <= (varnumber_T)زیادہ_سے_زیادہ_عدد / slen)
	واپس OK;

    حرف	buf[NUMBUFLEN];

    vim_snprintf(buf, حجم(buf), "%lld", (طویل طویل)n);
    semsg(_(e_val_too_large), buf);
    واپس FAIL;
}

#اگر_ختم // FEAT_EVAL
