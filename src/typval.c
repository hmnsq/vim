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

#অন্তর্ভাব "vim.h"

#যদি defined(FEAT_EVAL)

/*
 * Allocate memory for a variable type-value, and make it empty (0 or NULL
 * value).
 */
    typval_T *
alloc_tv(শূন্য)
{
    ফেরত ALLOC_CLEAR_ONE(typval_T);
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
    যদি (rettv != ফাঁকা)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = s;
    }
    অন্যথা
	vim_free(s);
    ফেরত rettv;
}

/*
 * Free the memory for a variable type-value.
 */
    শূন্য
free_tv(typval_T *varp)
{
    যদি (varp == ফাঁকা)
	ফেরত;

    নির্বাচন (varp->v_type)
    {
	ক্ষেত্র VAR_FUNC:
	    func_unref(varp->vval.v_string);
	    // FALLTHROUGH
	ক্ষেত্র VAR_STRING:
	    vim_free(varp->vval.v_string);
	    বিরতি;
	ক্ষেত্র VAR_PARTIAL:
	    partial_unref(varp->vval.v_partial);
	    বিরতি;
	ক্ষেত্র VAR_BLOB:
	    blob_unref(varp->vval.v_blob);
	    বিরতি;
	ক্ষেত্র VAR_LIST:
	    list_unref(varp->vval.v_list);
	    বিরতি;
	ক্ষেত্র VAR_TUPLE:
	    tuple_unref(varp->vval.v_tuple);
	    বিরতি;
	ক্ষেত্র VAR_DICT:
	    dict_unref(varp->vval.v_dict);
	    বিরতি;
	ক্ষেত্র VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    job_unref(varp->vval.v_job);
	    বিরতি;
#endif
	ক্ষেত্র VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    channel_unref(varp->vval.v_channel);
	    বিরতি;
#endif
	ক্ষেত্র VAR_CLASS:
	    class_unref(varp->vval.v_class);
	    বিরতি;
	ক্ষেত্র VAR_OBJECT:
	    object_unref(varp->vval.v_object);
	    বিরতি;

	ক্ষেত্র VAR_TYPEALIAS:
	    typealias_unref(varp->vval.v_typealias);
	    বিরতি;

	ক্ষেত্র VAR_সংখ্যা:
	ক্ষেত্র VAR_FLOAT:
	ক্ষেত্র VAR_ANY:
	ক্ষেত্র VAR_UNKNOWN:
	ক্ষেত্র VAR_VOID:
	ক্ষেত্র VAR_BOOL:
	ক্ষেত্র VAR_SPECIAL:
	ক্ষেত্র VAR_INSTR:
	    বিরতি;
    }
    vim_free(varp);
}

/*
 * Free the memory for a variable value and set the value to NULL or 0.
 */
    শূন্য
clear_tv(typval_T *varp)
{
    যদি (varp == ফাঁকা)
	ফেরত;

    নির্বাচন (varp->v_type)
    {
	ক্ষেত্র VAR_FUNC:
	    func_unref(varp->vval.v_string);
	    // FALLTHROUGH
	ক্ষেত্র VAR_STRING:
	    VIM_CLEAR(varp->vval.v_string);
	    বিরতি;
	ক্ষেত্র VAR_PARTIAL:
	    partial_unref(varp->vval.v_partial);
	    varp->vval.v_partial = ফাঁকা;
	    বিরতি;
	ক্ষেত্র VAR_BLOB:
	    blob_unref(varp->vval.v_blob);
	    varp->vval.v_blob = ফাঁকা;
	    বিরতি;
	ক্ষেত্র VAR_LIST:
	    list_unref(varp->vval.v_list);
	    varp->vval.v_list = ফাঁকা;
	    বিরতি;
	ক্ষেত্র VAR_TUPLE:
	    tuple_unref(varp->vval.v_tuple);
	    varp->vval.v_tuple = ফাঁকা;
	    বিরতি;
	ক্ষেত্র VAR_DICT:
	    dict_unref(varp->vval.v_dict);
	    varp->vval.v_dict = ফাঁকা;
	    বিরতি;
	ক্ষেত্র VAR_সংখ্যা:
	ক্ষেত্র VAR_BOOL:
	ক্ষেত্র VAR_SPECIAL:
	    varp->vval.v_সংখ্যা = 0;
	    বিরতি;
	ক্ষেত্র VAR_FLOAT:
	    varp->vval.v_float = 0.0;
	    বিরতি;
	ক্ষেত্র VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    job_unref(varp->vval.v_job);
	    varp->vval.v_job = ফাঁকা;
#endif
	    বিরতি;
	ক্ষেত্র VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    channel_unref(varp->vval.v_channel);
	    varp->vval.v_channel = ফাঁকা;
#endif
	    বিরতি;
	ক্ষেত্র VAR_INSTR:
	    VIM_CLEAR(varp->vval.v_instr);
	    বিরতি;
	ক্ষেত্র VAR_CLASS:
	    class_unref(varp->vval.v_class);
	    varp->vval.v_class = ফাঁকা;
	    বিরতি;
	ক্ষেত্র VAR_OBJECT:
	    object_unref(varp->vval.v_object);
	    varp->vval.v_object = ফাঁকা;
	    বিরতি;
	ক্ষেত্র VAR_TYPEALIAS:
	    typealias_unref(varp->vval.v_typealias);
	    varp->vval.v_typealias = ফাঁকা;
	    বিরতি;
	ক্ষেত্র VAR_UNKNOWN:
	ক্ষেত্র VAR_ANY:
	ক্ষেত্র VAR_VOID:
	    বিরতি;
    }
    varp->v_lock = 0;
}

/*
 * Set the value of a variable to NULL without freeing items.
 */
    শূন্য
init_tv(typval_T *varp)
{
    যদি (varp != ফাঁকা)
	CLEAR_POINTER(varp);
}

    স্থির varnumber_T
tv_get_bool_or_সংখ্যা_chk(
	typval_T    *varp,
	পূর্ণসংখ্যা	    *denote,
	পূর্ণসংখ্যা	    want_bool,
	পূর্ণসংখ্যা	    vim9_string_error) // in Vim9 using a string is an error
{
    varnumber_T	n = 0L;

    নির্বাচন (varp->v_type)
    {
	ক্ষেত্র VAR_সংখ্যা:
	    যদি (in_vim9script() && want_bool && varp->vval.v_সংখ্যা != 0
						   && varp->vval.v_সংখ্যা != 1)
	    {
		semsg(_(e_using_সংখ্যা_as_bool_nr), varp->vval.v_সংখ্যা);
		বিরতি;
	    }
	    ফেরত varp->vval.v_সংখ্যা;
	ক্ষেত্র VAR_FLOAT:
	    emsg(_(e_using_float_as_সংখ্যা));
	    বিরতি;
	ক্ষেত্র VAR_FUNC:
	ক্ষেত্র VAR_PARTIAL:
	    emsg(_(e_using_funcref_as_সংখ্যা));
	    বিরতি;
	ক্ষেত্র VAR_STRING:
	    যদি (vim9_string_error && in_vim9script())
	    {
		emsg_using_string_as(varp, !want_bool);
		বিরতি;
	    }
	    যদি (varp->vval.v_string != ফাঁকা)
		vim_str2nr(varp->vval.v_string, ফাঁকা, ফাঁকা,
					 STR2NR_ALL, &n, ফাঁকা, 0, FALSE, ফাঁকা);
	    ফেরত n;
	ক্ষেত্র VAR_LIST:
	    emsg(_(e_using_list_as_সংখ্যা));
	    বিরতি;
	ক্ষেত্র VAR_TUPLE:
	    emsg(_(e_using_tuple_as_সংখ্যা));
	    বিরতি;
	ক্ষেত্র VAR_DICT:
	    emsg(_(e_using_dictionary_as_সংখ্যা));
	    বিরতি;
	ক্ষেত্র VAR_BOOL:
	ক্ষেত্র VAR_SPECIAL:
	    যদি (!want_bool && in_vim9script())
	    {
		যদি (varp->v_type == VAR_BOOL)
		    emsg(_(e_using_bool_as_সংখ্যা));
		অন্যথা
		    emsg(_(e_using_special_as_সংখ্যা));
		বিরতি;
	    }
	    ফেরত varp->vval.v_সংখ্যা == VVAL_TRUE ? 1 : 0;
	ক্ষেত্র VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    emsg(_(e_using_job_as_সংখ্যা));
	    বিরতি;
#endif
	ক্ষেত্র VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    emsg(_(e_using_channel_as_সংখ্যা));
	    বিরতি;
#endif
	ক্ষেত্র VAR_BLOB:
	    emsg(_(e_using_blob_as_সংখ্যা));
	    বিরতি;
	ক্ষেত্র VAR_CLASS:
	ক্ষেত্র VAR_TYPEALIAS:
	    check_typval_is_মান(varp);
	    বিরতি;
	ক্ষেত্র VAR_OBJECT:
	    {
		যদি (varp->vval.v_object == ফাঁকা)
		    emsg(_(e_using_object_as_string));
		অন্যথা
		{
		    class_T *cl = varp->vval.v_object->obj_class;
		    যদি (cl != ফাঁকা && IS_ENUM(cl))
			semsg(_(e_using_enum_str_as_সংখ্যা), cl->class_name.string);
		    অন্যথা
			emsg(_(e_using_object_as_সংখ্যা));
		}
	    }
	    বিরতি;
	ক্ষেত্র VAR_VOID:
	    emsg(_(e_cannot_use_void_মান));
	    বিরতি;
	ক্ষেত্র VAR_UNKNOWN:
	ক্ষেত্র VAR_ANY:
	ক্ষেত্র VAR_INSTR:
	    internal_error_no_abort("tv_get_number(UNKNOWN)");
	    বিরতি;
    }
    যদি (denote == ফাঁকা)		// useful for values that must be unsigned
	n = -1;
    অন্যথা
	*denote = TRUE;
    ফেরত n;
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
tv_get_সংখ্যা(typval_T *varp)
{
    পূর্ণসংখ্যা		error = FALSE;

    ফেরত tv_get_সংখ্যা_chk(varp, &error);	// return 0L on error
}

/*
 * Like tv_get_number() but in Vim9 script do convert a number in a string to a
 * number without giving an error.
 */
    varnumber_T
tv_to_সংখ্যা(typval_T *varp)
{
    পূর্ণসংখ্যা		error = FALSE;

    ফেরত tv_get_bool_or_সংখ্যা_chk(varp, &error, FALSE, FALSE);
}

    varnumber_T
tv_get_সংখ্যা_chk(typval_T *varp, পূর্ণসংখ্যা *denote)
{
    ফেরত tv_get_bool_or_সংখ্যা_chk(varp, denote, FALSE, TRUE);
}

/*
 * Get the boolean value of "varp".  This is like tv_get_number_chk(),
 * but in Vim9 script accepts Number (0 and 1) and Bool/Special.
 */
    varnumber_T
tv_get_bool(typval_T *varp)
{
    ফেরত tv_get_bool_or_সংখ্যা_chk(varp, ফাঁকা, TRUE, TRUE);
}

/*
 * Get the boolean value of "varp".  This is like tv_get_number_chk(),
 * but in Vim9 script accepts Number and Bool.
 */
    varnumber_T
tv_get_bool_chk(typval_T *varp, পূর্ণসংখ্যা *denote)
{
    ফেরত tv_get_bool_or_সংখ্যা_chk(varp, denote, TRUE, TRUE);
}

    স্থির float_T
tv_get_float_chk(typval_T *varp, পূর্ণসংখ্যা *error)
{
    নির্বাচন (varp->v_type)
    {
	ক্ষেত্র VAR_সংখ্যা:
	    ফেরত (float_T)(varp->vval.v_সংখ্যা);
	ক্ষেত্র VAR_FLOAT:
	    ফেরত varp->vval.v_float;
	ক্ষেত্র VAR_FUNC:
	ক্ষেত্র VAR_PARTIAL:
	    emsg(_(e_using_funcref_as_float));
	    বিরতি;
	ক্ষেত্র VAR_STRING:
	    emsg(_(e_using_string_as_float));
	    বিরতি;
	ক্ষেত্র VAR_LIST:
	    emsg(_(e_using_list_as_float));
	    বিরতি;
	ক্ষেত্র VAR_TUPLE:
	    emsg(_(e_using_tuple_as_float));
	    বিরতি;
	ক্ষেত্র VAR_DICT:
	    emsg(_(e_using_dictionary_as_float));
	    বিরতি;
	ক্ষেত্র VAR_BOOL:
	    emsg(_(e_using_boolean_মান_as_float));
	    বিরতি;
	ক্ষেত্র VAR_SPECIAL:
	    emsg(_(e_using_special_মান_as_float));
	    বিরতি;
	ক্ষেত্র VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    emsg(_(e_using_job_as_float));
	    বিরতি;
#endif
	ক্ষেত্র VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    emsg(_(e_using_channel_as_float));
	    বিরতি;
#endif
	ক্ষেত্র VAR_BLOB:
	    emsg(_(e_using_blob_as_float));
	    বিরতি;
	ক্ষেত্র VAR_CLASS:
	ক্ষেত্র VAR_TYPEALIAS:
	    check_typval_is_মান(varp);
	    বিরতি;
	ক্ষেত্র VAR_OBJECT:
	    emsg(_(e_using_object_as_float));
	    বিরতি;
	ক্ষেত্র VAR_VOID:
	    emsg(_(e_cannot_use_void_মান));
	    বিরতি;
	ক্ষেত্র VAR_UNKNOWN:
	ক্ষেত্র VAR_ANY:
	ক্ষেত্র VAR_INSTR:
	    internal_error_no_abort("tv_get_float(UNKNOWN)");
	    বিরতি;
    }
    যদি (error != ফাঁকা)
	*error = TRUE;
    ফেরত 0;
}

    float_T
tv_get_float(typval_T *varp)
{
    ফেরত tv_get_float_chk(varp, ফাঁকা);
}

/*
 * Give an error and return FAIL unless "args[idx]" is unknown
 */
    পূর্ণসংখ্যা
check_for_unknown_arg(typval_T *args, পূর্ণসংখ্যা idx)
{
    যদি (args[idx].v_type != VAR_UNKNOWN)
    {
	semsg(_(e_too_many_arguments), idx + 1);
	ফেরত FAIL;
    }
    ফেরত OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string.
 */
    পূর্ণসংখ্যা
check_for_string_arg(typval_T *args, পূর্ণসংখ্যা idx)
{
    যদি (args[idx].v_type != VAR_STRING)
    {
	semsg(_(e_string_required_for_argument_nr), idx + 1);
	ফেরত FAIL;
    }
    ফেরত OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a non-empty string.
 */
    পূর্ণসংখ্যা
check_for_nonempty_string_arg(typval_T *args, পূর্ণসংখ্যা idx)
{
    যদি (check_for_string_arg(args, idx) == FAIL)
	ফেরত FAIL;
    যদি (args[idx].vval.v_string == ফাঁকা || *args[idx].vval.v_string == NUL)
    {
	semsg(_(e_non_empty_string_required_for_argument_nr), idx + 1);
	ফেরত FAIL;
    }
    ফেরত OK;
}

/*
 * Check for an optional string argument at 'idx'
 */
    পূর্ণসংখ্যা
check_for_opt_string_arg(typval_T *args, পূর্ণসংখ্যা idx)
{
    ফেরত (args[idx].v_type == VAR_UNKNOWN
	    || check_for_string_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a number.
 */
    পূর্ণসংখ্যা
check_for_সংখ্যা_arg(typval_T *args, পূর্ণসংখ্যা idx)
{
    যদি (args[idx].v_type != VAR_সংখ্যা)
    {
	semsg(_(e_সংখ্যা_required_for_argument_nr), idx + 1);
	ফেরত FAIL;
    }
    ফেরত OK;
}

/*
 * Check for an optional number argument at 'idx'
 */
    পূর্ণসংখ্যা
check_for_opt_সংখ্যা_arg(typval_T *args, পূর্ণসংখ্যা idx)
{
    ফেরত (args[idx].v_type == VAR_UNKNOWN
	    || check_for_সংখ্যা_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a float or a number.
 */
    পূর্ণসংখ্যা
check_for_float_or_nr_arg(typval_T *args, পূর্ণসংখ্যা idx)
{
    যদি (args[idx].v_type != VAR_FLOAT && args[idx].v_type != VAR_সংখ্যা)
    {
	semsg(_(e_float_or_সংখ্যা_required_for_argument_nr), idx + 1);
	ফেরত FAIL;
    }
    ফেরত OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a bool.
 */
    পূর্ণসংখ্যা
check_for_bool_arg(typval_T *args, পূর্ণসংখ্যা idx)
{
    যদি (args[idx].v_type != VAR_BOOL
	    && !(args[idx].v_type == VAR_সংখ্যা
		&& (args[idx].vval.v_সংখ্যা == 0
		    || args[idx].vval.v_সংখ্যা == 1)))
    {
	semsg(_(e_bool_required_for_argument_nr), idx + 1);
	ফেরত FAIL;
    }
    ফেরত OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a bool or a number.
 */
    স্থির পূর্ণসংখ্যা
check_for_bool_or_সংখ্যা_arg(typval_T *args, পূর্ণসংখ্যা idx)
{
    যদি (args[idx].v_type != VAR_BOOL && args[idx].v_type != VAR_সংখ্যা)
    {
	semsg(_(e_bool_or_সংখ্যা_required_for_argument_nr), idx + 1);
	ফেরত FAIL;
    }
    ফেরত OK;
}

/*
 * Check for an optional bool argument at 'idx'.
 * Return FAIL if the type is wrong.
 */
    পূর্ণসংখ্যা
check_for_opt_bool_arg(typval_T *args, পূর্ণসংখ্যা idx)
{
    যদি (args[idx].v_type == VAR_UNKNOWN)
	ফেরত OK;
    ফেরত check_for_bool_arg(args, idx);
}

/*
 * Check for an optional bool or number argument at 'idx'.
 * Return FAIL if the type is wrong.
 */
    পূর্ণসংখ্যা
check_for_opt_bool_or_সংখ্যা_arg(typval_T *args, পূর্ণসংখ্যা idx)
{
    যদি (args[idx].v_type == VAR_UNKNOWN)
	ফেরত OK;
    ফেরত check_for_bool_or_সংখ্যা_arg(args, idx);
}

/*
 * Check for an optional bool or dict argument at 'idx'.
 * Return FAIL if the type is wrong.
 */
    পূর্ণসংখ্যা
check_for_opt_bool_or_dict_arg(typval_T *args, পূর্ণসংখ্যা idx)
{
    যদি (args[idx].v_type == VAR_UNKNOWN || args[idx].v_type == VAR_DICT)
	ফেরত OK;
    ফেরত check_for_bool_arg(args, idx);
}

/*
 * Give an error and return FAIL unless "args[idx]" is a blob.
 */
    পূর্ণসংখ্যা
check_for_blob_arg(typval_T *args, পূর্ণসংখ্যা idx)
{
    যদি (args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_blob_required_for_argument_nr), idx + 1);
	ফেরত FAIL;
    }
    ফেরত OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list.
 */
    পূর্ণসংখ্যা
check_for_list_arg(typval_T *args, পূর্ণসংখ্যা idx)
{
    যদি (args[idx].v_type != VAR_LIST)
    {
	semsg(_(e_list_required_for_argument_nr), idx + 1);
	ফেরত FAIL;
    }
    ফেরত OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a non-NULL list.
 */
    পূর্ণসংখ্যা
check_for_nonnull_list_arg(typval_T *args, পূর্ণসংখ্যা idx)
{
    যদি (check_for_list_arg(args, idx) == FAIL)
	ফেরত FAIL;

    যদি (args[idx].vval.v_list == ফাঁকা)
    {
	semsg(_(e_non_null_list_required_for_argument_nr), idx + 1);
	ফেরত FAIL;
    }
    ফেরত OK;
}

/*
 * Check for an optional list argument at 'idx'
 */
    পূর্ণসংখ্যা
check_for_opt_list_arg(typval_T *args, পূর্ণসংখ্যা idx)
{
    ফেরত (args[idx].v_type == VAR_UNKNOWN
	    || check_for_list_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a tuple.
 */
    পূর্ণসংখ্যা
check_for_tuple_arg(typval_T *args, পূর্ণসংখ্যা idx)
{
    যদি (args[idx].v_type != VAR_TUPLE)
    {
	semsg(_(e_tuple_required_for_argument_nr), idx + 1);
	ফেরত FAIL;
    }
    ফেরত OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a dict.
 */
    পূর্ণসংখ্যা
check_for_dict_arg(typval_T *args, পূর্ণসংখ্যা idx)
{
    যদি (args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_dict_required_for_argument_nr), idx + 1);
	ফেরত FAIL;
    }
    ফেরত OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a non-NULL dict.
 */
    পূর্ণসংখ্যা
check_for_nonnull_dict_arg(typval_T *args, পূর্ণসংখ্যা idx)
{
    যদি (check_for_dict_arg(args, idx) == FAIL)
	ফেরত FAIL;

    যদি (args[idx].vval.v_dict == ফাঁকা)
    {
	semsg(_(e_non_null_dict_required_for_argument_nr), idx + 1);
	ফেরত FAIL;
    }
    ফেরত OK;
}

/*
 * Check for an optional dict argument at 'idx'
 */
    পূর্ণসংখ্যা
check_for_opt_dict_arg(typval_T *args, পূর্ণসংখ্যা idx)
{
    ফেরত (args[idx].v_type == VAR_UNKNOWN
	    || check_for_dict_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Check for an optional non-NULL dict argument at 'idx'
 */
    পূর্ণসংখ্যা
check_for_opt_nonnull_dict_arg(typval_T *args, পূর্ণসংখ্যা idx)
{
    ফেরত (args[idx].v_type == VAR_UNKNOWN
	    || check_for_nonnull_dict_arg(args, idx) != FAIL) ? OK : FAIL;
}

#যদি defined(FEAT_JOB_CHANNEL)
/*
 * Give an error and return FAIL unless "args[idx]" is a channel or a job.
 */
    পূর্ণসংখ্যা
check_for_chan_or_job_arg(typval_T *args, পূর্ণসংখ্যা idx)
{
    যদি (args[idx].v_type != VAR_CHANNEL && args[idx].v_type != VAR_JOB)
    {
	semsg(_(e_chan_or_job_required_for_argument_nr), idx + 1);
	ফেরত FAIL;
    }
    ফেরত OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an optional channel or a
 * job.
 */
    পূর্ণসংখ্যা
check_for_opt_chan_or_job_arg(typval_T *args, পূর্ণসংখ্যা idx)
{
    ফেরত (args[idx].v_type == VAR_UNKNOWN
	    || check_for_chan_or_job_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a job.
 */
    পূর্ণসংখ্যা
check_for_job_arg(typval_T *args, পূর্ণসংখ্যা idx)
{
    যদি (args[idx].v_type != VAR_JOB)
    {
	semsg(_(e_job_required_for_argument_nr), idx + 1);
	ফেরত FAIL;
    }
    ফেরত OK;
}

/*
 * Check for an optional job argument at 'idx'.
 */
    পূর্ণসংখ্যা
check_for_opt_job_arg(typval_T *args, পূর্ণসংখ্যা idx)
{
    ফেরত (args[idx].v_type == VAR_UNKNOWN
	    || check_for_job_arg(args, idx) != FAIL) ? OK : FAIL;
}
#অন্যথা
/*
 * Give an error and return FAIL unless "args[idx]" is an optional channel or a
 * job.  Used without the +channel feature, thus only VAR_UNKNOWN is accepted.
 */
    পূর্ণসংখ্যা
check_for_opt_chan_or_job_arg(typval_T *args, পূর্ণসংখ্যা idx)
{
    ফেরত args[idx].v_type == VAR_UNKNOWN ? OK : FAIL;
}
#endif

/*
 * Give an error and return FAIL unless "args[idx]" is a string or
 * a number.
 */
    পূর্ণসংখ্যা
check_for_string_or_সংখ্যা_arg(typval_T *args, পূর্ণসংখ্যা idx)
{
    যদি (args[idx].v_type != VAR_STRING && args[idx].v_type != VAR_সংখ্যা)
    {
	semsg(_(e_string_or_সংখ্যা_required_for_argument_nr), idx + 1);
	ফেরত FAIL;
    }
    ফেরত OK;
}

/*
 * Check for an optional string or number argument at 'idx'.
 */
    পূর্ণসংখ্যা
check_for_opt_string_or_সংখ্যা_arg(typval_T *args, পূর্ণসংখ্যা idx)
{
    ফেরত (args[idx].v_type == VAR_UNKNOWN
	    || check_for_string_or_সংখ্যা_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a buffer number.
 * Buffer number can be a number or a string.
 */
    পূর্ণসংখ্যা
check_for_বাফার_arg(typval_T *args, পূর্ণসংখ্যা idx)
{
    ফেরত check_for_string_or_সংখ্যা_arg(args, idx);
}

/*
 * Check for an optional buffer argument at 'idx'
 */
    পূর্ণসংখ্যা
check_for_opt_বাফার_arg(typval_T *args, পূর্ণসংখ্যা idx)
{
    ফেরত (args[idx].v_type == VAR_UNKNOWN
	    || check_for_বাফার_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a line number.
 * Line number can be a number or a string.
 */
    পূর্ণসংখ্যা
check_for_lnum_arg(typval_T *args, পূর্ণসংখ্যা idx)
{
    ফেরত check_for_string_or_সংখ্যা_arg(args, idx);
}

/*
 * Check for an optional line number argument at 'idx'
 */
    পূর্ণসংখ্যা
check_for_opt_lnum_arg(typval_T *args, পূর্ণসংখ্যা idx)
{
    ফেরত (args[idx].v_type == VAR_UNKNOWN
	    || check_for_lnum_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a blob.
 */
    পূর্ণসংখ্যা
check_for_string_or_blob_arg(typval_T *args, পূর্ণসংখ্যা idx)
{
    যদি (args[idx].v_type != VAR_STRING && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_string_or_blob_required_for_argument_nr), idx + 1);
	ফেরত FAIL;
    }
    ফেরত OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a list.
 */
    পূর্ণসংখ্যা
check_for_string_or_list_arg(typval_T *args, পূর্ণসংখ্যা idx)
{
    যদি (args[idx].v_type != VAR_STRING && args[idx].v_type != VAR_LIST)
    {
	semsg(_(e_string_or_list_required_for_argument_nr), idx + 1);
	ফেরত FAIL;
    }
    ফেরত OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string, a list, a
 * tuple or a blob.
 */
    পূর্ণসংখ্যা
check_for_string_or_list_or_tuple_or_blob_arg(typval_T *args, পূর্ণসংখ্যা idx)
{
    যদি (args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_string_list_tuple_or_blob_required_for_argument_nr), idx + 1);
	ফেরত FAIL;
    }
    ফেরত OK;
}

/*
 * Check for an optional string or list argument at 'idx'
 */
    পূর্ণসংখ্যা
check_for_opt_string_or_list_arg(typval_T *args, পূর্ণসংখ্যা idx)
{
    ফেরত (args[idx].v_type == VAR_UNKNOWN
	    || check_for_string_or_list_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a dict.
 */
    পূর্ণসংখ্যা
check_for_string_or_dict_arg(typval_T *args, পূর্ণসংখ্যা idx)
{
    যদি (args[idx].v_type != VAR_STRING && args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_string_or_dict_required_for_argument_nr), idx + 1);
	ফেরত FAIL;
    }
    ফেরত OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a number
 * or a list.
 */
    পূর্ণসংখ্যা
check_for_string_or_সংখ্যা_or_list_arg(typval_T *args, পূর্ণসংখ্যা idx)
{
    যদি (args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_সংখ্যা
	    && args[idx].v_type != VAR_LIST)
    {
	semsg(_(e_string_সংখ্যা_or_list_required_for_argument_nr), idx + 1);
	ফেরত FAIL;
    }
    ফেরত OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an optional string
 * or number or a list
 */
    পূর্ণসংখ্যা
check_for_opt_string_or_সংখ্যা_or_list_arg(typval_T *args, পূর্ণসংখ্যা idx)
{
    ফেরত (args[idx].v_type == VAR_UNKNOWN
	    || check_for_string_or_সংখ্যা_or_list_arg(args, idx)
							!= FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string, a number, a
 * list, a tuple or a blob.
 */
    পূর্ণসংখ্যা
check_for_repeat_func_arg(typval_T *args, পূর্ণসংখ্যা idx)
{
    যদি (args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_সংখ্যা
	    && args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_repeatable_type_required_for_argument_nr), idx + 1);
	ফেরত FAIL;
    }
    ফেরত OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string, a list, a
 * tuple or a dict.
 */
    পূর্ণসংখ্যা
check_for_string_list_tuple_or_dict_arg(typval_T *args, পূর্ণসংখ্যা idx)
{
    যদি (args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_string_list_tuple_or_dict_required_for_argument_nr), idx + 1);
	ফেরত FAIL;
    }
    ফেরত OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string
 * or a function reference.
 */
    পূর্ণসংখ্যা
check_for_string_or_func_arg(typval_T *args, পূর্ণসংখ্যা idx)
{
    যদি (args[idx].v_type != VAR_PARTIAL
	    && args[idx].v_type != VAR_FUNC
	    && args[idx].v_type != VAR_STRING)
    {
	semsg(_(e_string_or_function_required_for_argument_nr), idx + 1);
	ফেরত FAIL;
    }
    ফেরত OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list or a blob.
 */
    পূর্ণসংখ্যা
check_for_list_or_blob_arg(typval_T *args, পূর্ণসংখ্যা idx)
{
    যদি (args[idx].v_type != VAR_LIST && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_list_or_blob_required_for_argument_nr), idx + 1);
	ফেরত FAIL;
    }
    ফেরত OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list or a tuple.
 */
    পূর্ণসংখ্যা
check_for_list_or_tuple_arg(typval_T *args, পূর্ণসংখ্যা idx)
{
    যদি (args[idx].v_type != VAR_LIST && args[idx].v_type != VAR_TUPLE)
    {
	semsg(_(e_list_or_tuple_required_for_argument_nr), idx + 1);
	ফেরত FAIL;
    }
    ফেরত OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list, a tuple or a
 * blob.
 */
    পূর্ণসংখ্যা
check_for_list_or_tuple_or_blob_arg(typval_T *args, পূর্ণসংখ্যা idx)
{
    যদি (args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_list_or_tuple_or_blob_required_for_argument_nr), idx + 1);
	ফেরত FAIL;
    }
    ফেরত OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list, a tuple or a
 * dict
 */
    পূর্ণসংখ্যা
check_for_list_or_tuple_or_dict_arg(typval_T *args, পূর্ণসংখ্যা idx)
{
    যদি (args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_list_or_tuple_or_dict_required_for_argument_nr), idx + 1);
	ফেরত FAIL;
    }
    ফেরত OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list or dict or a
 * blob.
 */
    পূর্ণসংখ্যা
check_for_list_or_dict_or_blob_arg(typval_T *args, পূর্ণসংখ্যা idx)
{
    যদি (args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_DICT
	    && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_list_dict_or_blob_required_for_argument_nr), idx + 1);
	ফেরত FAIL;
    }
    ফেরত OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list, a tuple, a dict,
 * a blob or a string.
 */
    পূর্ণসংখ্যা
check_for_list_tuple_dict_blob_or_string_arg(typval_T *args, পূর্ণসংখ্যা idx)
{
    যদি (args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_DICT
	    && args[idx].v_type != VAR_BLOB
	    && args[idx].v_type != VAR_STRING)
    {
	semsg(_(e_list_tuple_dict_blob_or_string_required_for_argument_nr), idx + 1);
	ফেরত FAIL;
    }
    ফেরত OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an optional buffer
 * number or a dict.
 */
    পূর্ণসংখ্যা
check_for_opt_বাফার_or_dict_arg(typval_T *args, পূর্ণসংখ্যা idx)
{
    যদি (args[idx].v_type != VAR_UNKNOWN
	    && args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_সংখ্যা
	    && args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_string_required_for_argument_nr), idx + 1);
	ফেরত FAIL;
    }
    ফেরত OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an object.
 */
    পূর্ণসংখ্যা
check_for_object_arg(typval_T *args, পূর্ণসংখ্যা idx)
{
    যদি (args[idx].v_type != VAR_OBJECT)
    {
	semsg(_(e_object_required_for_argument_nr), idx + 1);
	ফেরত FAIL;
    }
    ফেরত OK;
}

/*
 * Returns TRUE if "tv" is a type alias for a class
 */
    স্থির পূর্ণসংখ্যা
tv_class_alias(typval_T *tv)
{
    ফেরত tv->v_type == VAR_TYPEALIAS &&
			tv->vval.v_typealias->ta_type->tt_type == VAR_OBJECT;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a class
 * or class typealias.
 */
    পূর্ণসংখ্যা
check_for_class_or_typealias_args(typval_T *args, পূর্ণসংখ্যা idx)
{
    জন্য (পূর্ণসংখ্যা i = idx; args[i].v_type != VAR_UNKNOWN; ++i)
    {
	যদি (args[i].v_type != VAR_CLASS && !tv_class_alias(&args[idx]))
	{
	    semsg(_(e_class_or_typealias_required_for_argument_nr), i + 1);
	    ফেরত FAIL;
	}
    }
    ফেরত OK;
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
    স্থির char_u   mybuf[NUMBUFLEN];

    ফেরত tv_get_string_buf(varp, mybuf);
}

/*
 * Like tv_get_string() but don't allow number to string conversion for Vim9.
 */
    char_u *
tv_get_string_strict(typval_T *varp)
{
    স্থির char_u   mybuf[NUMBUFLEN];
    char_u	    *res =  tv_get_string_buf_chk_strict(
						 varp, mybuf, in_vim9script());

    ফেরত res != ফাঁকা ? res : (char_u *)"";
}

    char_u *
tv_get_string_buf(typval_T *varp, char_u *buf)
{
    char_u	*res = tv_get_string_buf_chk(varp, buf);

    ফেরত res != ফাঁকা ? res : (char_u *)"";
}

/*
 * Careful: This uses a single, static buffer.  YOU CAN ONLY USE IT ONCE!
 */
    char_u *
tv_get_string_chk(typval_T *varp)
{
    স্থির char_u   mybuf[NUMBUFLEN];

    ফেরত tv_get_string_buf_chk(varp, mybuf);
}

    char_u *
tv_get_string_buf_chk(typval_T *varp, char_u *buf)
{
    ফেরত tv_get_string_buf_chk_strict(varp, buf, FALSE);
}

    char_u *
tv_get_string_buf_chk_strict(typval_T *varp, char_u *buf, পূর্ণসংখ্যা strict)
{
    নির্বাচন (varp->v_type)
    {
	ক্ষেত্র VAR_সংখ্যা:
	    যদি (strict)
	    {
		emsg(_(e_using_সংখ্যা_as_string));
		বিরতি;
	    }
	    vim_snprintf((অক্ষর *)buf, NUMBUFLEN, "%lld",
					    (varnumber_T)varp->vval.v_সংখ্যা);
	    ফেরত buf;
	ক্ষেত্র VAR_FUNC:
	ক্ষেত্র VAR_PARTIAL:
	    emsg(_(e_using_funcref_as_string));
	    বিরতি;
	ক্ষেত্র VAR_LIST:
	    emsg(_(e_using_list_as_string));
	    বিরতি;
	ক্ষেত্র VAR_TUPLE:
	    emsg(_(e_using_tuple_as_string));
	    বিরতি;
	ক্ষেত্র VAR_DICT:
	    emsg(_(e_using_dictionary_as_string));
	    বিরতি;
	ক্ষেত্র VAR_FLOAT:
	    যদি (strict)
	    {
		emsg(_(e_using_float_as_string));
		বিরতি;
	    }
	    vim_snprintf((অক্ষর *)buf, NUMBUFLEN, "%g", varp->vval.v_float);
	    ফেরত buf;
	ক্ষেত্র VAR_STRING:
	    যদি (varp->vval.v_string != ফাঁকা)
		ফেরত varp->vval.v_string;
	    ফেরত (char_u *)"";
	ক্ষেত্র VAR_BOOL:
	ক্ষেত্র VAR_SPECIAL:
	    STRCPY(buf, get_var_special_name(varp->vval.v_সংখ্যা));
	    ফেরত buf;
	ক্ষেত্র VAR_BLOB:
	    emsg(_(e_using_blob_as_string));
	    বিরতি;
	ক্ষেত্র VAR_CLASS:
	ক্ষেত্র VAR_TYPEALIAS:
	    check_typval_is_মান(varp);
	    বিরতি;
	ক্ষেত্র VAR_OBJECT:
	    {
		যদি (varp->vval.v_object == ফাঁকা)
		    emsg(_(e_using_object_as_string));
		অন্যথা
		{
		    class_T *cl = varp->vval.v_object->obj_class;
		    যদি (cl != ফাঁকা && IS_ENUM(cl))
			semsg(_(e_using_enum_str_as_string), cl->class_name.string);
		    অন্যথা
			emsg(_(e_using_object_as_string));
		}
	    }
	    বিরতি;
	ক্ষেত্র VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    যদি (in_vim9script())
	    {
		semsg(_(e_using_invalid_মান_as_string_str), "job");
		বিরতি;
	    }
	    ফেরত job_to_string_buf(varp, buf);
#endif
	    বিরতি;
	ক্ষেত্র VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    যদি (in_vim9script())
	    {
		semsg(_(e_using_invalid_মান_as_string_str), "channel");
		বিরতি;
	    }
	    ফেরত channel_to_string_buf(varp, buf);
#endif
	    বিরতি;
	ক্ষেত্র VAR_VOID:
	    emsg(_(e_cannot_use_void_মান));
	    বিরতি;
	ক্ষেত্র VAR_UNKNOWN:
	ক্ষেত্র VAR_ANY:
	ক্ষেত্র VAR_INSTR:
	    semsg(_(e_using_invalid_মান_as_string_str),
						  vartype_name(varp->v_type));
	    বিরতি;
    }
    ফেরত ফাঁকা;
}

/*
 * Turn a typeval into a string.  Similar to tv_get_string_buf() but uses
 * string() on Dict, List, etc.
 */
    char_u *
tv_stringify(typval_T *varp, char_u *buf)
{
    যদি (varp->v_type == VAR_LIST
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
	ফেরত tmp.vval.v_string;
    }
    ফেরত tv_get_string_buf(varp, buf);
}

/*
 * Return TRUE if typeval "tv" and its value are set to be locked (immutable).
 * Also give an error message, using "name" or _("name") when use_gettext is
 * TRUE.
 */
    পূর্ণসংখ্যা
tv_check_lock(typval_T *tv, char_u *name, পূর্ণসংখ্যা use_gettext)
{
    পূর্ণসংখ্যা	lock = 0;

    নির্বাচন (tv->v_type)
    {
	ক্ষেত্র VAR_BLOB:
	    যদি (tv->vval.v_blob != ফাঁকা)
		lock = tv->vval.v_blob->bv_lock;
	    বিরতি;
	ক্ষেত্র VAR_LIST:
	    যদি (tv->vval.v_list != ফাঁকা)
		lock = tv->vval.v_list->lv_lock;
	    বিরতি;
	ক্ষেত্র VAR_TUPLE:
	    যদি (tv->vval.v_tuple != ফাঁকা)
		lock = tv->vval.v_tuple->tv_lock;
	    বিরতি;
	ক্ষেত্র VAR_DICT:
	    যদি (tv->vval.v_dict != ফাঁকা)
		lock = tv->vval.v_dict->dv_lock;
	    বিরতি;
	পূর্বনির্ধারিত:
	    বিরতি;
    }
    ফেরত মান_check_lock(tv->v_lock, name, use_gettext)
		   || (lock != 0 && মান_check_lock(lock, name, use_gettext));
}

/*
 * Copy the values from typval_T "from" to typval_T "to".
 * When needed allocates string or increases reference count.
 * Does not make a copy of a list, blob or dict but copies the reference!
 * It is OK for "from" and "to" to point to the same item.  This is used to
 * make a copy later.
 */
    পূর্ণসংখ্যা
copy_tv(typval_T *from, typval_T *to)
{
    পূর্ণসংখ্যা		ret = OK;

    to->v_type = from->v_type;
    to->v_lock = 0;
    নির্বাচন (from->v_type)
    {
	ক্ষেত্র VAR_সংখ্যা:
	ক্ষেত্র VAR_BOOL:
	ক্ষেত্র VAR_SPECIAL:
	    to->vval.v_সংখ্যা = from->vval.v_সংখ্যা;
	    বিরতি;
	ক্ষেত্র VAR_FLOAT:
	    to->vval.v_float = from->vval.v_float;
	    বিরতি;
	ক্ষেত্র VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    to->vval.v_job = from->vval.v_job;
	    যদি (to->vval.v_job != ফাঁকা)
		++to->vval.v_job->jv_refcount;
	    বিরতি;
#endif
	ক্ষেত্র VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    to->vval.v_channel = from->vval.v_channel;
	    যদি (to->vval.v_channel != ফাঁকা)
		++to->vval.v_channel->ch_refcount;
	    বিরতি;
#endif
	ক্ষেত্র VAR_INSTR:
	    to->vval.v_instr = from->vval.v_instr;
	    বিরতি;

	ক্ষেত্র VAR_CLASS:
	    copy_class(from, to);
	    বিরতি;

	ক্ষেত্র VAR_OBJECT:
	    copy_object(from, to);
	    বিরতি;

	ক্ষেত্র VAR_STRING:
	ক্ষেত্র VAR_FUNC:
	    যদি (from->vval.v_string == ফাঁকা)
		to->vval.v_string = ফাঁকা;
	    অন্যথা
	    {
		to->vval.v_string = vim_strsave(from->vval.v_string);
		যদি (from->v_type == VAR_FUNC)
		    func_ref(to->vval.v_string);
	    }
	    বিরতি;
	ক্ষেত্র VAR_PARTIAL:
	    যদি (from->vval.v_partial == ফাঁকা)
		to->vval.v_partial = ফাঁকা;
	    অন্যথা
	    {
		to->vval.v_partial = from->vval.v_partial;
		++to->vval.v_partial->pt_refcount;
	    }
	    বিরতি;
	ক্ষেত্র VAR_BLOB:
	    যদি (from->vval.v_blob == ফাঁকা)
		to->vval.v_blob = ফাঁকা;
	    অন্যথা
	    {
		to->vval.v_blob = from->vval.v_blob;
		++to->vval.v_blob->bv_refcount;
	    }
	    বিরতি;
	ক্ষেত্র VAR_LIST:
	    যদি (from->vval.v_list == ফাঁকা)
		to->vval.v_list = ফাঁকা;
	    অন্যথা
	    {
		to->vval.v_list = from->vval.v_list;
		++to->vval.v_list->lv_refcount;
	    }
	    বিরতি;
	ক্ষেত্র VAR_TUPLE:
	    যদি (from->vval.v_tuple == ফাঁকা)
		to->vval.v_tuple = ফাঁকা;
	    অন্যথা
	    {
		to->vval.v_tuple = from->vval.v_tuple;
		++to->vval.v_tuple->tv_refcount;
	    }
	    বিরতি;
	ক্ষেত্র VAR_DICT:
	    যদি (from->vval.v_dict == ফাঁকা)
		to->vval.v_dict = ফাঁকা;
	    অন্যথা
	    {
		to->vval.v_dict = from->vval.v_dict;
		++to->vval.v_dict->dv_refcount;
	    }
	    বিরতি;
	ক্ষেত্র VAR_TYPEALIAS:
	    যদি (from->vval.v_typealias == ফাঁকা)
		to->vval.v_typealias = ফাঁকা;
	    অন্যথা
	    {
		to->vval.v_typealias = from->vval.v_typealias;
		++to->vval.v_typealias->ta_refcount;
	    }
	    বিরতি;
	ক্ষেত্র VAR_VOID:
	    emsg(_(e_cannot_use_void_মান));
	    ret = FAIL;
	    বিরতি;
	ক্ষেত্র VAR_UNKNOWN:
	ক্ষেত্র VAR_ANY:
	    internal_error_no_abort("copy_tv(UNKNOWN)");
	    ret = FAIL;
	    বিরতি;
    }

    ফেরত ret;
}

/*
 * Compare "tv1" and "tv2".
 */
    পূর্ণসংখ্যা
typval_compare2(
    typval_T	*tv1,	// first operand
    typval_T	*tv2,	// second operand
    exprtype_T	type,	// operator
    পূর্ণসংখ্যা		ic,	// ignore case
    পূর্ণসংখ্যা		*res)	// comparison result
{
    varnumber_T	n1, n2;
    পূর্ণসংখ্যা		type_is = type == রাশিমালা_IS || type == রাশিমালা_ISNOT;

    যদি (check_typval_is_মান(tv1) == FAIL
	|| check_typval_is_মান(tv2) == FAIL)
	ফেরত FAIL;
    অন্যথা যদি (type_is && tv1->v_type != tv2->v_type)
    {
	// For "is" a different type always means FALSE, for "isnot"
	// it means TRUE.
	*res = (type == রাশিমালা_ISNOT);
    }
    অন্যথা যদি (((tv1->v_type == VAR_SPECIAL && tv1->vval.v_সংখ্যা == VVAL_NULL)
		|| (tv2->v_type == VAR_SPECIAL
					   && tv2->vval.v_সংখ্যা == VVAL_NULL))
	    && tv1->v_type != tv2->v_type
	    && (type == রাশিমালা_EQUAL || type == রাশিমালা_NEQUAL))
    {
	n1 = typval_compare_null(tv1, tv2);
	যদি (n1 == MAYBE)
	    ফেরত FAIL;
	যদি (type == রাশিমালা_NEQUAL)
	    n1 = !n1;
	*res = n1;
    }
    অন্যথা যদি (tv1->v_type == VAR_BLOB || tv2->v_type == VAR_BLOB)
    {
	যদি (typval_compare_blob(tv1, tv2, type, res) == FAIL)
	    ফেরত FAIL;
    }
    অন্যথা যদি (tv1->v_type == VAR_LIST || tv2->v_type == VAR_LIST)
    {
	যদি (typval_compare_list(tv1, tv2, type, ic, res) == FAIL)
	    ফেরত FAIL;
    }
    অন্যথা যদি (tv1->v_type == VAR_TUPLE || tv2->v_type == VAR_TUPLE)
    {
	যদি (typval_compare_tuple(tv1, tv2, type, ic, res) == FAIL)
	    ফেরত FAIL;
    }
    অন্যথা যদি (tv1->v_type == VAR_OBJECT || tv2->v_type == VAR_OBJECT)
    {
	যদি (typval_compare_object(tv1, tv2, type, ic, res) == FAIL)
	    ফেরত FAIL;
    }
    অন্যথা যদি (tv1->v_type == VAR_DICT || tv2->v_type == VAR_DICT)
    {
	যদি (typval_compare_dict(tv1, tv2, type, ic, res) == FAIL)
	    ফেরত FAIL;
    }
    অন্যথা যদি (tv1->v_type == VAR_FUNC || tv2->v_type == VAR_FUNC
	|| tv1->v_type == VAR_PARTIAL || tv2->v_type == VAR_PARTIAL)
    {
	যদি (typval_compare_func(tv1, tv2, type, ic, res) == FAIL)
	    ফেরত FAIL;
    }

    // If one of the two variables is a float, compare as a float.
    // When using "=~" or "!~", always compare as string.
    অন্যথা যদি ((tv1->v_type == VAR_FLOAT || tv2->v_type == VAR_FLOAT)
	    && type != রাশিমালা_MATCH && type != রাশিমালা_NOMATCH)
    {
	float_T f1, f2;
	পূর্ণসংখ্যা	error = FALSE;

	f1 = tv_get_float_chk(tv1, &error);
	যদি (!error)
	    f2 = tv_get_float_chk(tv2, &error);
	যদি (error)
	    ফেরত FAIL;
	n1 = FALSE;
	নির্বাচন (type)
	{
	    ক্ষেত্র রাশিমালা_IS:
	    ক্ষেত্র রাশিমালা_EQUAL:    n1 = (f1 == f2); বিরতি;
	    ক্ষেত্র রাশিমালা_ISNOT:
	    ক্ষেত্র রাশিমালা_NEQUAL:   n1 = (f1 != f2); বিরতি;
	    ক্ষেত্র রাশিমালা_GREATER:  n1 = (f1 > f2); বিরতি;
	    ক্ষেত্র রাশিমালা_GEQUAL:   n1 = (f1 >= f2); বিরতি;
	    ক্ষেত্র রাশিমালা_SMALLER:  n1 = (f1 < f2); বিরতি;
	    ক্ষেত্র রাশিমালা_SEQUAL:   n1 = (f1 <= f2); বিরতি;
	    ক্ষেত্র রাশিমালা_UNKNOWN:
	    ক্ষেত্র রাশিমালা_MATCH:
	    পূর্বনির্ধারিত:  বিরতি;  // avoid gcc warning
	}
	*res = n1;
    }

    // If one of the two variables is a number, compare as a number.
    // When using "=~" or "!~", always compare as string.
    অন্যথা যদি ((tv1->v_type == VAR_সংখ্যা || tv2->v_type == VAR_সংখ্যা)
	    && type != রাশিমালা_MATCH && type != রাশিমালা_NOMATCH)
    {
	পূর্ণসংখ্যা error = FALSE;

	n1 = tv_get_সংখ্যা_chk(tv1, &error);
	যদি (!error)
	    n2 = tv_get_সংখ্যা_chk(tv2, &error);
	যদি (error)
	    ফেরত FAIL;
	নির্বাচন (type)
	{
	    ক্ষেত্র রাশিমালা_IS:
	    ক্ষেত্র রাশিমালা_EQUAL:    n1 = (n1 == n2); বিরতি;
	    ক্ষেত্র রাশিমালা_ISNOT:
	    ক্ষেত্র রাশিমালা_NEQUAL:   n1 = (n1 != n2); বিরতি;
	    ক্ষেত্র রাশিমালা_GREATER:  n1 = (n1 > n2); বিরতি;
	    ক্ষেত্র রাশিমালা_GEQUAL:   n1 = (n1 >= n2); বিরতি;
	    ক্ষেত্র রাশিমালা_SMALLER:  n1 = (n1 < n2); বিরতি;
	    ক্ষেত্র রাশিমালা_SEQUAL:   n1 = (n1 <= n2); বিরতি;
	    ক্ষেত্র রাশিমালা_UNKNOWN:
	    ক্ষেত্র রাশিমালা_MATCH:
	    পূর্বনির্ধারিত:  বিরতি;  // avoid gcc warning
	}
	*res = n1;
    }
    অন্যথা যদি (in_vim9script() && (tv1->v_type == VAR_BOOL
				    || tv2->v_type == VAR_BOOL
				    || (tv1->v_type == VAR_SPECIAL
					      && tv2->v_type == VAR_SPECIAL)))
    {
	যদি (tv1->v_type != tv2->v_type)
	{
	    semsg(_(e_cannot_compare_str_with_str),
		       vartype_name(tv1->v_type), vartype_name(tv2->v_type));
	    ফেরত FAIL;
	}
	n1 = tv1->vval.v_সংখ্যা;
	n2 = tv2->vval.v_সংখ্যা;
	নির্বাচন (type)
	{
	    ক্ষেত্র রাশিমালা_IS:
	    ক্ষেত্র রাশিমালা_EQUAL:    n1 = (n1 == n2); বিরতি;
	    ক্ষেত্র রাশিমালা_ISNOT:
	    ক্ষেত্র রাশিমালা_NEQUAL:   n1 = (n1 != n2); বিরতি;
	    পূর্বনির্ধারিত:
		semsg(_(e_invalid_operation_for_str),
						   vartype_name(tv1->v_type));
		ফেরত FAIL;
	}
	*res = n1;
    }
#ifdef FEAT_JOB_CHANNEL
    অন্যথা যদি (tv1->v_type == tv2->v_type
	    && (tv1->v_type == VAR_CHANNEL || tv1->v_type == VAR_JOB)
	    && (type == রাশিমালা_NEQUAL || type == রাশিমালা_EQUAL))
    {
	যদি (tv1->v_type == VAR_CHANNEL)
	    n1 = tv1->vval.v_channel == tv2->vval.v_channel;
	অন্যথা
	    n1 = tv1->vval.v_job == tv2->vval.v_job;
	যদি (type == রাশিমালা_NEQUAL)
	    n1 = !n1;
	*res = n1;
    }
#endif
    অন্যথা
    {
	যদি (typval_compare_string(tv1, tv2, type, ic, res) == FAIL)
	    ফেরত FAIL;
    }

    ফেরত OK;
}

/*
 * Compare "tv1" and "tv2".
 * Put the result in "tv1".  Caller should clear "tv2".
 */
    পূর্ণসংখ্যা
typval_compare(
    typval_T	*tv1,	// first operand
    typval_T	*tv2,	// second operand
    exprtype_T	type,   // operator
    পূর্ণসংখ্যা		ic)     // ignore case
{
    পূর্ণসংখ্যা res;

    যদি (typval_compare2(tv1, tv2, type, ic, &res) == FAIL)
    {
	clear_tv(tv1);
	ফেরত FAIL;
    }

    clear_tv(tv1);
    যদি (in_vim9script())
    {
	tv1->v_type = VAR_BOOL;
	tv1->vval.v_সংখ্যা = res ? VVAL_TRUE : VVAL_FALSE;
    }
    অন্যথা
    {
	tv1->v_type = VAR_সংখ্যা;
	tv1->vval.v_সংখ্যা = res;
    }

    ফেরত OK;
}

/*
 * Compare "tv1" to "tv2" as lists according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    পূর্ণসংখ্যা
typval_compare_list(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	পূর্ণসংখ্যা	    ic,
	পূর্ণসংখ্যা	    *res)
{
    পূর্ণসংখ্যা	    val = 0;

    যদি (type == রাশিমালা_IS || type == রাশিমালা_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
				      && tv1->vval.v_list == tv2->vval.v_list);
	যদি (type == রাশিমালা_ISNOT)
	    val = !val;
    }
    অন্যথা যদি (tv1->v_type != tv2->v_type
	    || (type != রাশিমালা_EQUAL && type != রাশিমালা_NEQUAL))
    {
	যদি (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_list_with_list));
	অন্যথা
	    emsg(_(e_invalid_operation_for_list));
	ফেরত FAIL;
    }
    অন্যথা
    {
	val = list_equal(tv1->vval.v_list, tv2->vval.v_list, ic);
	যদি (type == রাশিমালা_NEQUAL)
	    val = !val;
    }
    *res = val;
    ফেরত OK;
}

/*
 * Compare "tv1" to "tv2" as tuples according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    পূর্ণসংখ্যা
typval_compare_tuple(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	পূর্ণসংখ্যা	    ic,
	পূর্ণসংখ্যা	    *res)
{
    পূর্ণসংখ্যা	    val = 0;

    যদি (type == রাশিমালা_IS || type == রাশিমালা_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
			      && tv1->vval.v_tuple == tv2->vval.v_tuple);
	যদি (type == রাশিমালা_ISNOT)
	    val = !val;
    }
    অন্যথা যদি (tv1->v_type != tv2->v_type
	    || (type != রাশিমালা_EQUAL && type != রাশিমালা_NEQUAL))
    {
	যদি (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_tuple_with_tuple));
	অন্যথা
	    emsg(_(e_invalid_operation_for_tuple));
	ফেরত FAIL;
    }
    অন্যথা
    {
	val = tuple_equal(tv1->vval.v_tuple, tv2->vval.v_tuple, ic);
	যদি (type == রাশিমালা_NEQUAL)
	    val = !val;
    }
    *res = val;
    ফেরত OK;
}

/*
 * Compare v:null with another type.  Return TRUE if the value is NULL.
 */
    পূর্ণসংখ্যা
typval_compare_null(typval_T *tv1, typval_T *tv2)
{
    যদি ((tv1->v_type == VAR_SPECIAL && tv1->vval.v_সংখ্যা == VVAL_NULL)
	    || (tv2->v_type == VAR_SPECIAL && tv2->vval.v_সংখ্যা == VVAL_NULL))
    {
	typval_T	*tv = tv1->v_type == VAR_SPECIAL ? tv2 : tv1;

	নির্বাচন (tv->v_type)
	{
	    ক্ষেত্র VAR_BLOB: ফেরত tv->vval.v_blob == ফাঁকা;
#ifdef FEAT_JOB_CHANNEL
	    ক্ষেত্র VAR_CHANNEL: ফেরত tv->vval.v_channel == ফাঁকা;
#endif
	    // TODO: null_class handling
	    // case VAR_CLASS: return tv->vval.v_class == NULL;
	    ক্ষেত্র VAR_DICT: ফেরত tv->vval.v_dict == ফাঁকা;
	    ক্ষেত্র VAR_FUNC: ফেরত tv->vval.v_string == ফাঁকা;
#ifdef FEAT_JOB_CHANNEL
	    ক্ষেত্র VAR_JOB: ফেরত tv->vval.v_job == ফাঁকা;
#endif
	    ক্ষেত্র VAR_LIST: ফেরত tv->vval.v_list == ফাঁকা;
	    ক্ষেত্র VAR_TUPLE: ফেরত tv->vval.v_tuple == ফাঁকা;
	    ক্ষেত্র VAR_OBJECT: ফেরত tv->vval.v_object == ফাঁকা;
	    ক্ষেত্র VAR_PARTIAL: ফেরত tv->vval.v_partial == ফাঁকা;
	    ক্ষেত্র VAR_STRING: ফেরত tv->vval.v_string == ফাঁকা;

	    ক্ষেত্র VAR_সংখ্যা: যদি (!in_vim9script())
				 ফেরত tv->vval.v_সংখ্যা == 0;
			     বিরতি;
	    ক্ষেত্র VAR_FLOAT: যদি (!in_vim9script())
				 ফেরত tv->vval.v_float == 0.0;
			     বিরতি;
	    ক্ষেত্র VAR_TYPEALIAS: ফেরত tv->vval.v_typealias == ফাঁকা;
	    পূর্বনির্ধারিত: বিরতি;
	}
    }
    // although comparing null with number, float or bool is not very useful
    // we won't give an error
    ফেরত FALSE;
}

/*
 * Compare "tv1" to "tv2" as blobs according to "type".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    পূর্ণসংখ্যা
typval_compare_blob(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	পূর্ণসংখ্যা	    *res)
{
    পূর্ণসংখ্যা	    val = 0;

    যদি (type == রাশিমালা_IS || type == রাশিমালা_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
			&& tv1->vval.v_blob == tv2->vval.v_blob);
	যদি (type == রাশিমালা_ISNOT)
	    val = !val;
    }
    অন্যথা যদি (tv1->v_type != tv2->v_type
	    || (type != রাশিমালা_EQUAL && type != রাশিমালা_NEQUAL))
    {
	যদি (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_blob_with_blob));
	অন্যথা
	    emsg(_(e_invalid_operation_for_blob));
	ফেরত FAIL;
    }
    অন্যথা
    {
	val = blob_equal(tv1->vval.v_blob, tv2->vval.v_blob);
	যদি (type == রাশিমালা_NEQUAL)
	    val = !val;
    }
    *res = val;
    ফেরত OK;
}

/*
 * Compare "tv1" to "tv2" as objects according to "type".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    পূর্ণসংখ্যা
typval_compare_object(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	পূর্ণসংখ্যা	    ic,
	পূর্ণসংখ্যা	    *res)
{
    পূর্ণসংখ্যা res_match = type == রাশিমালা_EQUAL || type == রাশিমালা_IS ? TRUE : FALSE;

    যদি (tv1->vval.v_object == ফাঁকা && tv2->vval.v_object == ফাঁকা)
    {
	*res = res_match;
	ফেরত OK;
    }
    যদি (tv1->vval.v_object == ফাঁকা || tv2->vval.v_object == ফাঁকা)
    {
	*res = !res_match;
	ফেরত OK;
    }

    object_T *obj1 = tv1->vval.v_object;
    object_T *obj2 = tv2->vval.v_object;
    যদি (type == রাশিমালা_IS || type == রাশিমালা_ISNOT)
    {
	*res = obj1 == obj2 ? res_match : !res_match;
	ফেরত OK;
    }
    অন্যথা যদি (tv1->v_type != tv2->v_type)
    {
	emsg(_(e_can_only_compare_object_with_object));
	ফেরত FAIL;
    }

    *res = object_equal(obj1, obj2, ic) ? res_match : !res_match;
    ফেরত OK;
}

/*
 * Compare "tv1" to "tv2" as dictionaries according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    পূর্ণসংখ্যা
typval_compare_dict(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	পূর্ণসংখ্যা	    ic,
	পূর্ণসংখ্যা	    *res)
{
    পূর্ণসংখ্যা	    val;

    যদি (type == রাশিমালা_IS || type == রাশিমালা_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
			&& tv1->vval.v_dict == tv2->vval.v_dict);
	যদি (type == রাশিমালা_ISNOT)
	    val = !val;
    }
    অন্যথা যদি (tv1->v_type != tv2->v_type
		|| (type != রাশিমালা_EQUAL && type != রাশিমালা_NEQUAL))
    {
	যদি (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_dictionary_with_dictionary));
	অন্যথা
	    emsg(_(e_invalid_operation_for_dictionary));
	ফেরত FAIL;
    }
    অন্যথা
    {
	val = dict_equal(tv1->vval.v_dict, tv2->vval.v_dict, ic);
	যদি (type == রাশিমালা_NEQUAL)
	    val = !val;
    }
    *res = val;
    ফেরত OK;
}

/*
 * Compare "tv1" to "tv2" as funcrefs according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    পূর্ণসংখ্যা
typval_compare_func(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	পূর্ণসংখ্যা	    ic,
	পূর্ণসংখ্যা	    *res)
{
    পূর্ণসংখ্যা	    val = 0;

    যদি (type != রাশিমালা_EQUAL && type != রাশিমালা_NEQUAL
	    && type != রাশিমালা_IS && type != রাশিমালা_ISNOT)
    {
	emsg(_(e_invalid_operation_for_funcrefs));
	ফেরত FAIL;
    }
    যদি ((tv1->v_type == VAR_PARTIAL && tv1->vval.v_partial == ফাঁকা)
	    || (tv2->v_type == VAR_PARTIAL && tv2->vval.v_partial == ফাঁকা))
	// When both partials are NULL, then they are equal.
	// Otherwise they are not equal.
	val = (tv1->vval.v_partial == tv2->vval.v_partial);
    অন্যথা যদি (type == রাশিমালা_IS || type == রাশিমালা_ISNOT)
    {
	যদি (tv1->v_type == VAR_FUNC && tv2->v_type == VAR_FUNC)
	    // strings are considered the same if their value is
	    // the same
	    val = tv_equal(tv1, tv2, ic);
	অন্যথা যদি (tv1->v_type == VAR_PARTIAL && tv2->v_type == VAR_PARTIAL)
	    val = (tv1->vval.v_partial == tv2->vval.v_partial);
	অন্যথা
	    val = FALSE;
    }
    অন্যথা
	val = tv_equal(tv1, tv2, ic);
    যদি (type == রাশিমালা_NEQUAL || type == রাশিমালা_ISNOT)
	val = !val;
    *res = val;
    ফেরত OK;
}

/*
 * Compare "tv1" to "tv2" as strings according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    পূর্ণসংখ্যা
typval_compare_string(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	পূর্ণসংখ্যা	    ic,
	পূর্ণসংখ্যা	    *res)
{
    পূর্ণসংখ্যা		i = 0;
    পূর্ণসংখ্যা		val = FALSE;
    char_u	*s1, *s2;
    char_u	buf1[NUMBUFLEN], buf2[NUMBUFLEN];

    যদি (in_vim9script()
	  && ((tv1->v_type != VAR_STRING && tv1->v_type != VAR_SPECIAL)
	   || (tv2->v_type != VAR_STRING && tv2->v_type != VAR_SPECIAL)))
    {
	semsg(_(e_cannot_compare_str_with_str),
		   vartype_name(tv1->v_type), vartype_name(tv2->v_type));
	ফেরত FAIL;
    }
    s1 = tv_get_string_buf(tv1, buf1);
    s2 = tv_get_string_buf(tv2, buf2);
    যদি (type != রাশিমালা_MATCH && type != রাশিমালা_NOMATCH)
	i = ic ? MB_STRICMP(s1, s2) : STRCMP(s1, s2);
    নির্বাচন (type)
    {
	ক্ষেত্র রাশিমালা_IS:	    যদি (in_vim9script())
			    {
				// Really check it is the same string, not just
				// the same value.
				val = tv1->vval.v_string == tv2->vval.v_string;
				বিরতি;
			    }
			    // FALLTHROUGH
	ক্ষেত্র রাশিমালা_EQUAL:    val = (i == 0); বিরতি;
	ক্ষেত্র রাশিমালা_ISNOT:    যদি (in_vim9script())
			    {
				// Really check it is not the same string, not
				// just a different value.
				val = tv1->vval.v_string != tv2->vval.v_string;
				বিরতি;
			    }
			    // FALLTHROUGH
	ক্ষেত্র রাশিমালা_NEQUAL:   val = (i != 0); বিরতি;
	ক্ষেত্র রাশিমালা_GREATER:  val = (i > 0); বিরতি;
	ক্ষেত্র রাশিমালা_GEQUAL:   val = (i >= 0); বিরতি;
	ক্ষেত্র রাশিমালা_SMALLER:  val = (i < 0); বিরতি;
	ক্ষেত্র রাশিমালা_SEQUAL:   val = (i <= 0); বিরতি;

	ক্ষেত্র রাশিমালা_MATCH:
	ক্ষেত্র রাশিমালা_NOMATCH:
		val = pattern_match(s2, s1, ic);
		যদি (type == রাশিমালা_NOMATCH)
		    val = !val;
		বিরতি;

	পূর্বনির্ধারিত:  বিরতি;  // avoid gcc warning
    }
    *res = val;
    ফেরত OK;
}
/*
 * Convert any type to a string, never give an error.
 * When "quotes" is TRUE add quotes to a string.
 * Returns an allocated string.
 */
    char_u *
typval_tostring(typval_T *arg, পূর্ণসংখ্যা quotes)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    char_u	*ret = ফাঁকা;

    যদি (arg == ফাঁকা)
	ফেরত vim_strsave((char_u *)"(does not exist)");
    যদি (!quotes && arg->v_type == VAR_STRING)
    {
	ret = vim_strsave(arg->vval.v_string == ফাঁকা ? (char_u *)""
							 : arg->vval.v_string);
    }
    অন্যথা
    {
	ret = tv2string(arg, &tofree, numbuf, 0);
	// Make a copy if we have a value but it's not in allocated memory.
	যদি (ret != ফাঁকা && tofree == ফাঁকা)
	    ret = vim_strsave(ret);
    }
    ফেরত ret;
}

/*
 * Return TRUE if typeval "tv" is locked: Either that value is locked itself
 * or it refers to a List or Dictionary that is locked.
 */
    পূর্ণসংখ্যা
tv_islocked(typval_T *tv)
{
    ফেরত (tv->v_lock & VAR_LOCKED)
	|| (tv->v_type == VAR_LIST
		&& tv->vval.v_list != ফাঁকা
		&& (tv->vval.v_list->lv_lock & VAR_LOCKED))
	|| (tv->v_type == VAR_TUPLE
		&& tv->vval.v_tuple != ফাঁকা
		&& (tv->vval.v_tuple->tv_lock & VAR_LOCKED))
	|| (tv->v_type == VAR_DICT
		&& tv->vval.v_dict != ফাঁকা
		&& (tv->vval.v_dict->dv_lock & VAR_LOCKED));
}

    স্থির পূর্ণসংখ্যা
func_equal(
    typval_T *tv1,
    typval_T *tv2,
    পূর্ণসংখ্যা	     ic)	    // ignore case
{
    char_u	*s1, *s2;
    dict_T	*d1, *d2;
    পূর্ণসংখ্যা		a1, a2;
    পূর্ণসংখ্যা		i;

    // empty and NULL function name considered the same
    s1 = tv1->v_type == VAR_FUNC ? tv1->vval.v_string
					   : partial_name(tv1->vval.v_partial);
    যদি (s1 != ফাঁকা && *s1 == NUL)
	s1 = ফাঁকা;
    s2 = tv2->v_type == VAR_FUNC ? tv2->vval.v_string
					   : partial_name(tv2->vval.v_partial);
    যদি (s2 != ফাঁকা && *s2 == NUL)
	s2 = ফাঁকা;
    যদি (s1 == ফাঁকা || s2 == ফাঁকা)
    {
	যদি (s1 != s2)
	    ফেরত FALSE;
    }
    অন্যথা যদি (STRCMP(s1, s2) != 0)
	ফেরত FALSE;

    // empty dict and NULL dict is different
    d1 = tv1->v_type == VAR_FUNC ? ফাঁকা : tv1->vval.v_partial->pt_dict;
    d2 = tv2->v_type == VAR_FUNC ? ফাঁকা : tv2->vval.v_partial->pt_dict;
    যদি (d1 == ফাঁকা || d2 == ফাঁকা)
    {
	যদি (d1 != d2)
	    ফেরত FALSE;
    }
    অন্যথা যদি (!dict_equal(d1, d2, ic))
	ফেরত FALSE;

    // empty list and no list considered the same
    a1 = tv1->v_type == VAR_FUNC ? 0 : tv1->vval.v_partial->pt_argc;
    a2 = tv2->v_type == VAR_FUNC ? 0 : tv2->vval.v_partial->pt_argc;
    যদি (a1 != a2)
	ফেরত FALSE;
    জন্য (i = 0; i < a1; ++i)
	যদি (!tv_equal(tv1->vval.v_partial->pt_argv + i,
		      tv2->vval.v_partial->pt_argv + i, ic))
	    ফেরত FALSE;

    ফেরত TRUE;
}

/*
 * Return TRUE if "tv1" and "tv2" have the same value.
 * Compares the items just like "==" would compare them, but strings and
 * numbers are different.  Floats and numbers are also different.
 */
    পূর্ণসংখ্যা
tv_equal(
    typval_T *tv1,
    typval_T *tv2,
    পূর্ণসংখ্যা	     ic)	    // ignore case
{
    char_u	buf1[NUMBUFLEN], buf2[NUMBUFLEN];
    char_u	*s1, *s2;
    স্থির পূর্ণসংখ্যা  recursive_cnt = 0;	    // catch recursive loops
    পূর্ণসংখ্যা		r;
    স্থির পূর্ণসংখ্যা	tv_equal_recurse_limit;

    // Catch lists and dicts that have an endless loop by limiting
    // recursiveness to a limit.  We guess they are equal then.
    // A fixed limit has the problem of still taking an awful long time.
    // Reduce the limit every time running into it. That should work fine for
    // deeply linked structures that are not recursively linked and catch
    // recursiveness quickly.
    যদি (recursive_cnt == 0)
	tv_equal_recurse_limit = 1000;
    যদি (recursive_cnt >= tv_equal_recurse_limit)
    {
	--tv_equal_recurse_limit;
	ফেরত TRUE;
    }

    // For VAR_FUNC and VAR_PARTIAL compare the function name, bound dict and
    // arguments.
    যদি ((tv1->v_type == VAR_FUNC
		|| (tv1->v_type == VAR_PARTIAL && tv1->vval.v_partial != ফাঁকা))
	    && (tv2->v_type == VAR_FUNC
		|| (tv2->v_type == VAR_PARTIAL && tv2->vval.v_partial != ফাঁকা)))
    {
	++recursive_cnt;
	r = func_equal(tv1, tv2, ic);
	--recursive_cnt;
	ফেরত r;
    }

    যদি (tv1->v_type != tv2->v_type
	    && ((tv1->v_type != VAR_BOOL && tv1->v_type != VAR_SPECIAL)
		|| (tv2->v_type != VAR_BOOL && tv2->v_type != VAR_SPECIAL)))
	ফেরত FALSE;

    নির্বাচন (tv1->v_type)
    {
	ক্ষেত্র VAR_LIST:
	    ++recursive_cnt;
	    r = list_equal(tv1->vval.v_list, tv2->vval.v_list, ic);
	    --recursive_cnt;
	    ফেরত r;

	ক্ষেত্র VAR_TUPLE:
	    ++recursive_cnt;
	    r = tuple_equal(tv1->vval.v_tuple, tv2->vval.v_tuple, ic);
	    --recursive_cnt;
	    ফেরত r;

	ক্ষেত্র VAR_DICT:
	    ++recursive_cnt;
	    r = dict_equal(tv1->vval.v_dict, tv2->vval.v_dict, ic);
	    --recursive_cnt;
	    ফেরত r;

	ক্ষেত্র VAR_BLOB:
	    ফেরত blob_equal(tv1->vval.v_blob, tv2->vval.v_blob);

	ক্ষেত্র VAR_সংখ্যা:
	ক্ষেত্র VAR_BOOL:
	ক্ষেত্র VAR_SPECIAL:
	    ফেরত tv1->vval.v_সংখ্যা == tv2->vval.v_সংখ্যা;

	ক্ষেত্র VAR_STRING:
	    s1 = tv_get_string_buf(tv1, buf1);
	    s2 = tv_get_string_buf(tv2, buf2);
	    ফেরত ((ic ? MB_STRICMP(s1, s2) : STRCMP(s1, s2)) == 0);

	ক্ষেত্র VAR_FLOAT:
	    ফেরত tv1->vval.v_float == tv2->vval.v_float;
	ক্ষেত্র VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    ফেরত tv1->vval.v_job == tv2->vval.v_job;
#endif
	ক্ষেত্র VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    ফেরত tv1->vval.v_channel == tv2->vval.v_channel;
#endif
	ক্ষেত্র VAR_INSTR:
	    ফেরত tv1->vval.v_instr == tv2->vval.v_instr;

	ক্ষেত্র VAR_CLASS:
	    // A class only exists once, equality is identity.
	    ফেরত tv1->vval.v_class == tv2->vval.v_class;

	ক্ষেত্র VAR_OBJECT:
	    ++recursive_cnt;
	    r = object_equal(tv1->vval.v_object, tv2->vval.v_object, ic);
	    --recursive_cnt;
	    ফেরত r;

	ক্ষেত্র VAR_PARTIAL:
	    ফেরত tv1->vval.v_partial == tv2->vval.v_partial;

	ক্ষেত্র VAR_FUNC:
	    ফেরত tv1->vval.v_string == tv2->vval.v_string;

	ক্ষেত্র VAR_TYPEALIAS:
	    ফেরত tv1->vval.v_typealias == tv2->vval.v_typealias;

	ক্ষেত্র VAR_UNKNOWN:
	ক্ষেত্র VAR_ANY:
	ক্ষেত্র VAR_VOID:
	    বিরতি;
    }

    // VAR_UNKNOWN can be the result of a invalid expression, let's say it
    // does not equal anything, not even itself.
    ফেরত FALSE;
}

/*
 * Get an option value.
 * "arg" points to the '&' or '+' before the option name.
 * "arg" is advanced to character after the option name.
 * Return OK or FAIL.
 */
    পূর্ণসংখ্যা
eval_option(
    char_u	**arg,
    typval_T	*rettv,	// when NULL, only check if option exists
    পূর্ণসংখ্যা		evaluate)
{
    char_u	*option_end;
    long	numval;
    char_u	*stringval;
    getoption_T	opt_type;
    পূর্ণসংখ্যা		c;
    পূর্ণসংখ্যা		working = (**arg == '+');    // has("+option")
    পূর্ণসংখ্যা		ret = OK;
    পূর্ণসংখ্যা		scope;

    // Isolate the option name and find its value.
    option_end = find_option_end(arg, &scope);
    যদি (option_end == ফাঁকা)
    {
	যদি (rettv != ফাঁকা)
	    semsg(_(e_option_name_missing_str), *arg);
	ফেরত FAIL;
    }

    যদি (!evaluate)
    {
	*arg = option_end;
	ফেরত OK;
    }

    c = *option_end;
    *option_end = NUL;
    opt_type = get_option_মান(*arg, &numval,
			       rettv == ফাঁকা ? ফাঁকা : &stringval, ফাঁকা, scope);

    যদি (opt_type == gov_unknown)
    {
	যদি (rettv != ফাঁকা)
	    semsg(_(e_unknown_option_str), *arg);
	ret = FAIL;
    }
    অন্যথা যদি (rettv != ফাঁকা)
    {
	rettv->v_lock = 0;
	যদি (opt_type == gov_hidden_string)
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = ফাঁকা;
	}
	অন্যথা যদি (opt_type == gov_hidden_bool || opt_type == gov_hidden_সংখ্যা)
	{
	    rettv->v_type = in_vim9script() && opt_type == gov_hidden_bool
						       ? VAR_BOOL : VAR_সংখ্যা;
	    rettv->vval.v_সংখ্যা = 0;
	}
	অন্যথা যদি (opt_type == gov_bool || opt_type == gov_সংখ্যা)
	{
	    যদি (in_vim9script() && opt_type == gov_bool)
	    {
		rettv->v_type = VAR_BOOL;
		rettv->vval.v_সংখ্যা = numval ? VVAL_TRUE : VVAL_FALSE;
	    }
	    অন্যথা
	    {
		rettv->v_type = VAR_সংখ্যা;
		rettv->vval.v_সংখ্যা = numval;
	    }
	}
	অন্যথা				// string option
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = stringval;
	}
    }
    অন্যথা যদি (working && (opt_type == gov_hidden_bool
			|| opt_type == gov_hidden_সংখ্যা
			|| opt_type == gov_hidden_string))
	ret = FAIL;

    *option_end = c;		    // put back for error messages
    *arg = option_end;

    ফেরত ret;
}

/*
 * Allocate a variable for a number constant.  Also deals with "0z" for blob.
 * Return OK or FAIL.
 */
    পূর্ণসংখ্যা
eval_সংখ্যা(
	char_u	    **arg,
	typval_T    *rettv,
	পূর্ণসংখ্যা	    evaluate,
	পূর্ণসংখ্যা	    want_string)
{
    পূর্ণসংখ্যা		len;
    পূর্ণসংখ্যা		skip_quotes = !in_old_script(4);
    char_u	*p;
    পূর্ণসংখ্যা		get_float = FALSE;

    // We accept a float when the format matches
    // "[0-9]\+\.[0-9]\+\([eE][+-]\?[0-9]\+\)\?".  This is very
    // strict to avoid backwards compatibility problems.
    // With script version 2 and later the leading digit can be
    // omitted.
    // Don't look for a float after the "." operator, so that
    // ":let vers = 1.2.3" doesn't fail.
    যদি (**arg == '.')
	p = *arg;
    অন্যথা
    {
	p = *arg + 1;
	যদি (skip_quotes)
	    জন্য (;;)
	    {
		যদি (*p == '\'')
		    ++p;
		যদি (!vim_isdigit(*p))
		    বিরতি;
		p = skipdigits(p);
	    }
	অন্যথা
	    p = skipdigits(p);
    }
    যদি (!want_string && p[0] == '.' && vim_isdigit(p[1]))
    {
	get_float = TRUE;
	p = skipdigits(p + 2);
	যদি (*p == 'e' || *p == 'E')
	{
	    ++p;
	    যদি (*p == '-' || *p == '+')
		++p;
	    যদি (!vim_isdigit(*p))
		get_float = FALSE;
	    অন্যথা
		p = skipdigits(p + 1);
	}
	যদি (ASCII_ISALPHA(*p) || *p == '.')
	    get_float = FALSE;
    }
    যদি (get_float)
    {
	float_T	f;

	*arg += string2float(*arg, &f, skip_quotes);
	যদি (evaluate)
	{
	    rettv->v_type = VAR_FLOAT;
	    rettv->vval.v_float = f;
	}
    }
    অন্যথা
    যদি (**arg == '0' && ((*arg)[1] == 'z' || (*arg)[1] == 'Z'))
    {
	char_u  *bp;
	blob_T  *blob = ফাঁকা;  // init for gcc

	// Blob constant: 0z0123456789abcdef
	যদি (evaluate)
	    blob = blob_alloc();
	জন্য (bp = *arg + 2; vim_isxdigit(bp[0]); bp += 2)
	{
	    যদি (!vim_isxdigit(bp[1]))
	    {
		যদি (blob != ফাঁকা)
		{
		    emsg(_(e_blob_literal_should_have_an_even_সংখ্যা_of_hex_characters));
		    ga_clear(&blob->bv_ga);
		    VIM_CLEAR(blob);
		}
		ফেরত FAIL;
	    }
	    যদি (blob != ফাঁকা)
		ga_append(&blob->bv_ga,
			     (hex2nr(*bp) << 4) + hex2nr(*(bp+1)));
	    যদি (bp[2] == '.' && vim_isxdigit(bp[3]))
		++bp;
	}
	যদি (blob != ফাঁকা)
	    rettv_blob_set(rettv, blob);
	*arg = bp;
    }
    অন্যথা
    {
	varnumber_T	n;

	// decimal, hex or octal number
	vim_str2nr(*arg, ফাঁকা, &len, skip_quotes
		      ? STR2NR_NO_OCT + STR2NR_QUOTE
		      : STR2NR_ALL, &n, ফাঁকা, 0, TRUE, ফাঁকা);
	যদি (len == 0)
	{
	    যদি (evaluate)
		semsg(_(e_invalid_expression_str), *arg);
	    ফেরত FAIL;
	}
	*arg += len;
	যদি (evaluate)
	{
	    rettv->v_type = VAR_সংখ্যা;
	    rettv->vval.v_সংখ্যা = n;
	}
    }
    ফেরত OK;
}

/*
 * Evaluate a string constant and put the result in "rettv".
 * "*arg" points to the double quote or to after it when "interpolate" is TRUE.
 * When "interpolate" is TRUE reduce "{{" to "{", reduce "}}" to "}" and stop
 * at a single "{".
 * Return OK or FAIL.
 */
    পূর্ণসংখ্যা
eval_string(char_u **arg, typval_T *rettv, পূর্ণসংখ্যা evaluate, পূর্ণসংখ্যা interpolate)
{
    char_u	*p;
    char_u	*end;
    পূর্ণসংখ্যা		extra = interpolate ? 1 : 0;
    পূর্ণসংখ্যা		off = interpolate ? 0 : 1;
    পূর্ণসংখ্যা		len;

    // Find the end of the string, skipping backslashed characters.
    জন্য (p = *arg + off; *p != NUL && *p != '"'; MB_PTR_ADV(p))
    {
	যদি (*p == '\\' && p[1] != NUL)
	{
	    ++p;
	    // A "\<x>" form occupies at least 4 characters, and produces up
	    // to 9 characters (6 for the char and 3 for a modifier):
	    // reserve space for 5 extra.
	    যদি (*p == '<')
	    {
		পূর্ণসংখ্যা		modifiers = 0;
		পূর্ণসংখ্যা		flags = FSK_KEYCODE | FSK_IN_STRING;

		extra += 5;

		// Skip to the '>' to avoid using '{' inside for string
		// interpolation.
		যদি (p[1] != '*')
		    flags |= FSK_SIMPLIFY;
		যদি (find_special_চাবি(&p, &modifiers, flags, ফাঁকা) != 0)
		    --p;  // leave "p" on the ">"
	    }
	}
	অন্যথা যদি (interpolate && (*p == '{' || *p == '}'))
	{
	    যদি (*p == '{' && p[1] != '{') // start of expression
		বিরতি;
	    ++p;
	    যদি (p[-1] == '}' && *p != '}') // single '}' is an error
	    {
		semsg(_(e_stray_closing_curly_str), *arg);
		ফেরত FAIL;
	    }
	    --extra;  // "{{" becomes "{", "}}" becomes "}"
	}
    }

    যদি (*p != '"' && !(interpolate && *p == '{'))
    {
	semsg(_(e_missing_double_quote_str), *arg);
	ফেরত FAIL;
    }

    // If only parsing, set *arg and return here
    যদি (!evaluate)
    {
	*arg = p + off;
	ফেরত OK;
    }

    // Copy the string into allocated memory, handling backslashed
    // characters.
    rettv->v_type = VAR_STRING;
    len = (পূর্ণসংখ্যা)(p - *arg + extra);
    rettv->vval.v_string = alloc(len);
    যদি (rettv->vval.v_string == ফাঁকা)
	ফেরত FAIL;
    end = rettv->vval.v_string;

    জন্য (p = *arg + off; *p != NUL && *p != '"'; )
    {
	যদি (*p == '\\')
	{
	    নির্বাচন (*++p)
	    {
		ক্ষেত্র 'b': *end++ = BS; ++p; বিরতি;
		ক্ষেত্র 'e': *end++ = ESC; ++p; বিরতি;
		ক্ষেত্র 'f': *end++ = FF; ++p; বিরতি;
		ক্ষেত্র 'n': *end++ = NL; ++p; বিরতি;
		ক্ষেত্র 'r': *end++ = CAR; ++p; বিরতি;
		ক্ষেত্র 't': *end++ = TAB; ++p; বিরতি;

		ক্ষেত্র 'X': // hex: "\x1", "\x12"
		ক্ষেত্র 'x':
		ক্ষেত্র 'u': // Unicode: "\u0023"
		ক্ষেত্র 'U':
			  যদি (vim_isxdigit(p[1]))
			  {
			      পূর্ণসংখ্যা	n, nr;
			      পূর্ণসংখ্যা	c = SAFE_toupper(*p);

			      যদি (c == 'X')
				  n = 2;
			      অন্যথা যদি (*p == 'u')
				  n = 4;
			      অন্যথা
				  n = 8;
			      nr = 0;
			      যখন (--n >= 0 && vim_isxdigit(p[1]))
			      {
				  ++p;
				  nr = (nr << 4) + hex2nr(*p);
			      }
			      ++p;
			      // For "\u" store the number according to
			      // 'encoding'.
			      যদি (c != 'X')
				  end += (*mb_char2bytes)(nr, end);
			      অন্যথা
				  *end++ = nr;
			  }
			  বিরতি;

			  // octal: "\1", "\12", "\123"
		ক্ষেত্র '0':
		ক্ষেত্র '1':
		ক্ষেত্র '2':
		ক্ষেত্র '3':
		ক্ষেত্র '4':
		ক্ষেত্র '5':
		ক্ষেত্র '6':
		ক্ষেত্র '7': *end = *p++ - '0';
			  যদি (*p >= '0' && *p <= '7')
			  {
			      *end = (*end << 3) + *p++ - '0';
			      যদি (*p >= '0' && *p <= '7')
				  *end = (*end << 3) + *p++ - '0';
			  }
			  ++end;
			  বিরতি;

			  // Special key, e.g.: "\<C-W>"
		ক্ষেত্র '<':
			  {
			      পূর্ণসংখ্যা flags = FSK_KEYCODE | FSK_IN_STRING;

			      যদি (p[1] != '*')
				  flags |= FSK_SIMPLIFY;
			      extra = trans_special(&p, end, flags, FALSE, ফাঁকা);
			      যদি (extra != 0)
			      {
				  end += extra;
				  যদি (end >= rettv->vval.v_string + len)
				      iemsg("eval_string() used more space than allocated");
				  বিরতি;
			      }
			  }
			  // FALLTHROUGH

		পূর্বনির্ধারিত: MB_COPY_CHAR(p, end);
			  বিরতি;
	    }
	}
	অন্যথা
	{
	    যদি (interpolate && (*p == '{' || *p == '}'))
	    {
		যদি (*p == '{' && p[1] != '{') // start of expression
		    বিরতি;
		++p;  // reduce "{{" to "{" and "}}" to "}"
	    }
	    MB_COPY_CHAR(p, end);
	}
    }
    *end = NUL;
    যদি (*p == '"' && !interpolate)
	++p;
    *arg = p;

    ফেরত OK;
}

/*
 * Allocate a variable for a 'str''ing' constant.
 * When "interpolate" is TRUE reduce "{{" to "{" and stop at a single "{".
 * Return OK when a "rettv" was set to the string.
 * Return FAIL on error, "rettv" is not set.
 */
    পূর্ণসংখ্যা
eval_lit_string(char_u **arg, typval_T *rettv, পূর্ণসংখ্যা evaluate, পূর্ণসংখ্যা interpolate)
{
    char_u	*p;
    char_u	*str;
    পূর্ণসংখ্যা		reduce = interpolate ? -1 : 0;
    পূর্ণসংখ্যা		off = interpolate ? 0 : 1;

    // Find the end of the string, skipping ''.
    জন্য (p = *arg + off; *p != NUL; MB_PTR_ADV(p))
    {
	যদি (*p == '\'')
	{
	    যদি (p[1] != '\'')
		বিরতি;
	    ++reduce;
	    ++p;
	}
	অন্যথা যদি (interpolate)
	{
	    যদি (*p == '{')
	    {
		যদি (p[1] != '{')
		    বিরতি;
		++p;
		++reduce;
	    }
	    অন্যথা যদি (*p == '}')
	    {
		++p;
		যদি (*p != '}')
		{
		    semsg(_(e_stray_closing_curly_str), *arg);
		    ফেরত FAIL;
		}
		++reduce;
	    }
	}
    }

    যদি (*p != '\'' && !(interpolate && *p == '{'))
    {
	semsg(_(e_missing_single_quote_str), *arg);
	ফেরত FAIL;
    }

    // If only parsing return after setting "*arg"
    যদি (!evaluate)
    {
	*arg = p + off;
	ফেরত OK;
    }

    // Copy the string into allocated memory, handling '' to ' reduction and
    // any expressions.
    str = alloc((p - *arg) - reduce);
    যদি (str == ফাঁকা)
	ফেরত FAIL;
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = str;

    জন্য (p = *arg + off; *p != NUL; )
    {
	যদি (*p == '\'')
	{
	    যদি (p[1] != '\'')
		বিরতি;
	    ++p;
	}
	অন্যথা যদি (interpolate && (*p == '{' || *p == '}'))
	{
	    যদি (*p == '{' && p[1] != '{')
		বিরতি;
	    ++p;
	}
	MB_COPY_CHAR(p, str);
    }
    *str = NUL;
    *arg = p + off;

    ফেরত OK;
}

/*
 * Evaluate a single or double quoted string possibly containing expressions.
 * "arg" points to the '$'.  The result is put in "rettv".
 * Returns OK or FAIL.
 */
    পূর্ণসংখ্যা
eval_interp_string(char_u **arg, typval_T *rettv, পূর্ণসংখ্যা evaluate)
{
    typval_T	tv;
    পূর্ণসংখ্যা		ret = OK;
    পূর্ণসংখ্যা		quote;
    garray_T	ga;
    char_u	*p;

    ga_init2(&ga, 1, 80);

    // *arg is on the '$' character, move it to the first string character.
    ++*arg;
    quote = **arg;
    ++*arg;

    জন্য (;;)
    {
	// Get the string up to the matching quote or to a single '{'.
	// "arg" is advanced to either the quote or the '{'.
	যদি (quote == '"')
	    ret = eval_string(arg, &tv, evaluate, TRUE);
	অন্যথা
	    ret = eval_lit_string(arg, &tv, evaluate, TRUE);
	যদি (ret == FAIL)
	    বিরতি;
	যদি (evaluate)
	{
	    ga_concat(&ga, tv.vval.v_string);
	    clear_tv(&tv);
	}

	যদি (**arg != '{')
	{
	    // found terminating quote
	    ++*arg;
	    বিরতি;
	}
	p = eval_one_রাশিমালা_in_str(*arg, &ga, evaluate);
	যদি (p == ফাঁকা)
	{
	    ret = FAIL;
	    বিরতি;
	}
	*arg = p;
    }

    rettv->v_type = VAR_STRING;
    যদি (ret == FAIL || !evaluate || ga_append(&ga, NUL) == FAIL)
    {
	ga_clear(&ga);
	rettv->vval.v_string = ফাঁকা;
	ফেরত ret;
    }

    rettv->vval.v_string = ga.ga_তথ্য;
    ফেরত OK;
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
    পূর্ণসংখ্যা		copyID)
{
    ফেরত echo_string_core(tv, tofree, numbuf, copyID, FALSE, TRUE, FALSE);
}

/*
 * Get the value of an environment variable.
 * "arg" is pointing to the '$'.  It is advanced to after the name.
 * If the environment variable was not set, silently assume it is empty.
 * Return FAIL if the name is invalid.
 */
    পূর্ণসংখ্যা
eval_env_var(char_u **arg, typval_T *rettv, পূর্ণসংখ্যা evaluate)
{
    char_u	*string = ফাঁকা;
    পূর্ণসংখ্যা		len;
    পূর্ণসংখ্যা		cc;
    char_u	*name;
    পূর্ণসংখ্যা		mustfree = FALSE;

    ++*arg;
    name = *arg;
    len = get_env_len(arg);
    যদি (evaluate)
    {
	যদি (len == 0)
	    ফেরত FAIL; // invalid empty name

	cc = name[len];
	name[len] = NUL;
	// first try vim_getenv(), fast for normal environment vars
	string = vim_getenv(name, &mustfree);
	যদি (string != ফাঁকা && *string != NUL)
	{
	    যদি (!mustfree)
		string = vim_strsave(string);
	}
	অন্যথা
	{
	    যদি (mustfree)
		vim_free(string);

	    // next try expanding things like $VIM and ${HOME}
	    string = expand_env_save(name - 1);
	    যদি (string != ফাঁকা && *string == '$')
		VIM_CLEAR(string);
	}
	name[len] = cc;

	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = string;
	rettv->v_lock = 0;
    }

    ফেরত OK;
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
    পূর্ণসংখ্যা		did_emsg_before = did_emsg;

    যদি (argvars[0].v_type != VAR_STRING || !in_vim9script())
	lnum = (linenr_T)tv_get_সংখ্যা_chk(&argvars[0], ফাঁকা);
    যদি (lnum <= 0 && did_emsg_before == did_emsg
					    && argvars[0].v_type != VAR_সংখ্যা)
    {
	পূর্ণসংখ্যা	fnum;
	pos_T	*fp;

	// no valid number, try using arg like line()
	fp = var2fpos(&argvars[0], TRUE, &fnum, FALSE);
	যদি (fp != ফাঁকা)
	    lnum = fp->lnum;
    }
    ফেরত lnum;
}

/*
 * Get the lnum from the first argument.
 * Also accepts "$", then "buf" is used.
 * Returns 0 on error.
 */
    linenr_T
tv_get_lnum_buf(typval_T *argvars, buf_T *buf)
{
    যদি (argvars[0].v_type == VAR_STRING
	    && argvars[0].vval.v_string != ফাঁকা
	    && argvars[0].vval.v_string[0] == '$'
	    && argvars[0].vval.v_string[1] == NUL
	    && buf != ফাঁকা)
	ফেরত buf->b_ml.ml_সারি_গুনতি;
    ফেরত (linenr_T)tv_get_সংখ্যা_chk(&argvars[0], ফাঁকা);
}

/*
 * Get buffer by number or pattern.
 */
    buf_T *
tv_get_buf(typval_T *tv, পূর্ণসংখ্যা curtab_only)
{
    char_u	*name = tv->vval.v_string;
    buf_T	*buf;

    যদি (tv->v_type == VAR_সংখ্যা)
	ফেরত buflist_findnr((পূর্ণসংখ্যা)tv->vval.v_সংখ্যা);
    যদি (tv->v_type != VAR_STRING)
	ফেরত ফাঁকা;
    যদি (name == ফাঁকা || *name == NUL)
	ফেরত curbuf;
    যদি (name[0] == '$' && name[1] == NUL)
	ফেরত lastbuf;

    buf = buflist_find_by_name(name, curtab_only);

    // If not found, try expanding the name, like done for bufexists().
    যদি (buf == ফাঁকা)
	buf = find_বাফার(tv);

    ফেরত buf;
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
    যদি (buf == ফাঁকা
	    && tv->v_type != VAR_সংখ্যা
	    && tv->v_type != VAR_STRING)
	// issue errmsg for type error
	(শূন্য)tv_get_সংখ্যা(tv);
    ফেরত buf;
}

/*
 * Check that repeating "slen" items "n" times, does not
 * overflow the int used for the resulting length.
 */
    পূর্ণসংখ্যা
check_repeat_গুনতি(varnumber_T slen, varnumber_T n)
{
    // Use a division so that the multiplication cannot overflow.
    যদি (n <= (varnumber_T)INT_MAX / slen)
	ফেরত OK;

    অক্ষর	buf[NUMBUFLEN];

    vim_snprintf(buf, আকার(buf), "%lld", (long long)n);
    semsg(_(e_val_too_large), buf);
    ফেরত FAIL;
}

#endif // FEAT_EVAL
