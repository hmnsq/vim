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

#ضمن "vim.h"

#إذا defined(FEAT_EVAL)

/*
 * Allocate memory for a variable type-value, and make it empty (0 or NULL
 * value).
 */
    typval_T *
alloc_tv(فراغ)
{
    أرجع ALLOC_CLEAR_ONE(typval_T);
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
    إذا (rettv != عدم)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = s;
    }
    وإلا
	vim_free(s);
    أرجع rettv;
}

/*
 * Free the memory for a variable type-value.
 */
    فراغ
free_tv(typval_T *varp)
{
    إذا (varp == عدم)
	أرجع;

    بدل (varp->v_type)
    {
	حالة VAR_FUNC:
	    func_unref(varp->vval.v_string);
	    // FALLTHROUGH
	حالة VAR_STRING:
	    vim_free(varp->vval.v_string);
	    اكسر;
	حالة VAR_PARTIAL:
	    partial_unref(varp->vval.v_partial);
	    اكسر;
	حالة VAR_BLOB:
	    blob_unref(varp->vval.v_blob);
	    اكسر;
	حالة VAR_LIST:
	    list_unref(varp->vval.v_list);
	    اكسر;
	حالة VAR_TUPLE:
	    tuple_unref(varp->vval.v_tuple);
	    اكسر;
	حالة VAR_DICT:
	    dict_unref(varp->vval.v_dict);
	    اكسر;
	حالة VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    job_unref(varp->vval.v_job);
	    اكسر;
#endif
	حالة VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    channel_unref(varp->vval.v_channel);
	    اكسر;
#endif
	حالة VAR_CLASS:
	    class_unref(varp->vval.v_class);
	    اكسر;
	حالة VAR_OBJECT:
	    object_unref(varp->vval.v_object);
	    اكسر;

	حالة VAR_TYPEALIAS:
	    typealias_unref(varp->vval.v_typealias);
	    اكسر;

	حالة VAR_رقم:
	حالة VAR_FLOAT:
	حالة VAR_ANY:
	حالة VAR_UNKNOWN:
	حالة VAR_VOID:
	حالة VAR_BOOL:
	حالة VAR_SPECIAL:
	حالة VAR_INSTR:
	    اكسر;
    }
    vim_free(varp);
}

/*
 * Free the memory for a variable value and set the value to NULL or 0.
 */
    فراغ
clear_tv(typval_T *varp)
{
    إذا (varp == عدم)
	أرجع;

    بدل (varp->v_type)
    {
	حالة VAR_FUNC:
	    func_unref(varp->vval.v_string);
	    // FALLTHROUGH
	حالة VAR_STRING:
	    VIM_CLEAR(varp->vval.v_string);
	    اكسر;
	حالة VAR_PARTIAL:
	    partial_unref(varp->vval.v_partial);
	    varp->vval.v_partial = عدم;
	    اكسر;
	حالة VAR_BLOB:
	    blob_unref(varp->vval.v_blob);
	    varp->vval.v_blob = عدم;
	    اكسر;
	حالة VAR_LIST:
	    list_unref(varp->vval.v_list);
	    varp->vval.v_list = عدم;
	    اكسر;
	حالة VAR_TUPLE:
	    tuple_unref(varp->vval.v_tuple);
	    varp->vval.v_tuple = عدم;
	    اكسر;
	حالة VAR_DICT:
	    dict_unref(varp->vval.v_dict);
	    varp->vval.v_dict = عدم;
	    اكسر;
	حالة VAR_رقم:
	حالة VAR_BOOL:
	حالة VAR_SPECIAL:
	    varp->vval.v_رقم = 0;
	    اكسر;
	حالة VAR_FLOAT:
	    varp->vval.v_float = 0.0;
	    اكسر;
	حالة VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    job_unref(varp->vval.v_job);
	    varp->vval.v_job = عدم;
#endif
	    اكسر;
	حالة VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    channel_unref(varp->vval.v_channel);
	    varp->vval.v_channel = عدم;
#endif
	    اكسر;
	حالة VAR_INSTR:
	    VIM_CLEAR(varp->vval.v_instr);
	    اكسر;
	حالة VAR_CLASS:
	    class_unref(varp->vval.v_class);
	    varp->vval.v_class = عدم;
	    اكسر;
	حالة VAR_OBJECT:
	    object_unref(varp->vval.v_object);
	    varp->vval.v_object = عدم;
	    اكسر;
	حالة VAR_TYPEALIAS:
	    typealias_unref(varp->vval.v_typealias);
	    varp->vval.v_typealias = عدم;
	    اكسر;
	حالة VAR_UNKNOWN:
	حالة VAR_ANY:
	حالة VAR_VOID:
	    اكسر;
    }
    varp->v_lock = 0;
}

/*
 * Set the value of a variable to NULL without freeing items.
 */
    فراغ
init_tv(typval_T *varp)
{
    إذا (varp != عدم)
	CLEAR_POINTER(varp);
}

    ساكن varnumber_T
tv_get_bool_or_رقم_chk(
	typval_T    *varp,
	صحيح	    *denote,
	صحيح	    want_bool,
	صحيح	    vim9_string_error) // in Vim9 using a string is an error
{
    varnumber_T	n = 0L;

    بدل (varp->v_type)
    {
	حالة VAR_رقم:
	    إذا (in_vim9script() && want_bool && varp->vval.v_رقم != 0
						   && varp->vval.v_رقم != 1)
	    {
		semsg(_(e_using_رقم_as_bool_nr), varp->vval.v_رقم);
		اكسر;
	    }
	    أرجع varp->vval.v_رقم;
	حالة VAR_FLOAT:
	    emsg(_(e_using_float_as_رقم));
	    اكسر;
	حالة VAR_FUNC:
	حالة VAR_PARTIAL:
	    emsg(_(e_using_funcref_as_رقم));
	    اكسر;
	حالة VAR_STRING:
	    إذا (vim9_string_error && in_vim9script())
	    {
		emsg_using_string_as(varp, !want_bool);
		اكسر;
	    }
	    إذا (varp->vval.v_string != عدم)
		vim_str2nr(varp->vval.v_string, عدم, عدم,
					 STR2NR_ALL, &n, عدم, 0, FALSE, عدم);
	    أرجع n;
	حالة VAR_LIST:
	    emsg(_(e_using_list_as_رقم));
	    اكسر;
	حالة VAR_TUPLE:
	    emsg(_(e_using_tuple_as_رقم));
	    اكسر;
	حالة VAR_DICT:
	    emsg(_(e_using_dictionary_as_رقم));
	    اكسر;
	حالة VAR_BOOL:
	حالة VAR_SPECIAL:
	    إذا (!want_bool && in_vim9script())
	    {
		إذا (varp->v_type == VAR_BOOL)
		    emsg(_(e_using_bool_as_رقم));
		وإلا
		    emsg(_(e_using_special_as_رقم));
		اكسر;
	    }
	    أرجع varp->vval.v_رقم == VVAL_TRUE ? 1 : 0;
	حالة VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    emsg(_(e_using_job_as_رقم));
	    اكسر;
#endif
	حالة VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    emsg(_(e_using_channel_as_رقم));
	    اكسر;
#endif
	حالة VAR_BLOB:
	    emsg(_(e_using_blob_as_رقم));
	    اكسر;
	حالة VAR_CLASS:
	حالة VAR_TYPEALIAS:
	    check_typval_is_قيمة(varp);
	    اكسر;
	حالة VAR_OBJECT:
	    {
		إذا (varp->vval.v_object == عدم)
		    emsg(_(e_using_object_as_string));
		وإلا
		{
		    class_T *cl = varp->vval.v_object->obj_class;
		    إذا (cl != عدم && IS_ENUM(cl))
			semsg(_(e_using_enum_str_as_رقم), cl->class_name.string);
		    وإلا
			emsg(_(e_using_object_as_رقم));
		}
	    }
	    اكسر;
	حالة VAR_VOID:
	    emsg(_(e_cannot_use_void_قيمة));
	    اكسر;
	حالة VAR_UNKNOWN:
	حالة VAR_ANY:
	حالة VAR_INSTR:
	    internal_error_no_abort("tv_get_number(UNKNOWN)");
	    اكسر;
    }
    إذا (denote == عدم)		// useful for values that must be unsigned
	n = -1;
    وإلا
	*denote = TRUE;
    أرجع n;
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
tv_get_رقم(typval_T *varp)
{
    صحيح		error = FALSE;

    أرجع tv_get_رقم_chk(varp, &error);	// return 0L on error
}

/*
 * Like tv_get_number() but in Vim9 script do convert a number in a string to a
 * number without giving an error.
 */
    varnumber_T
tv_to_رقم(typval_T *varp)
{
    صحيح		error = FALSE;

    أرجع tv_get_bool_or_رقم_chk(varp, &error, FALSE, FALSE);
}

    varnumber_T
tv_get_رقم_chk(typval_T *varp, صحيح *denote)
{
    أرجع tv_get_bool_or_رقم_chk(varp, denote, FALSE, TRUE);
}

/*
 * Get the boolean value of "varp".  This is like tv_get_number_chk(),
 * but in Vim9 script accepts Number (0 and 1) and Bool/Special.
 */
    varnumber_T
tv_get_bool(typval_T *varp)
{
    أرجع tv_get_bool_or_رقم_chk(varp, عدم, TRUE, TRUE);
}

/*
 * Get the boolean value of "varp".  This is like tv_get_number_chk(),
 * but in Vim9 script accepts Number and Bool.
 */
    varnumber_T
tv_get_bool_chk(typval_T *varp, صحيح *denote)
{
    أرجع tv_get_bool_or_رقم_chk(varp, denote, TRUE, TRUE);
}

    ساكن float_T
tv_get_float_chk(typval_T *varp, صحيح *error)
{
    بدل (varp->v_type)
    {
	حالة VAR_رقم:
	    أرجع (float_T)(varp->vval.v_رقم);
	حالة VAR_FLOAT:
	    أرجع varp->vval.v_float;
	حالة VAR_FUNC:
	حالة VAR_PARTIAL:
	    emsg(_(e_using_funcref_as_float));
	    اكسر;
	حالة VAR_STRING:
	    emsg(_(e_using_string_as_float));
	    اكسر;
	حالة VAR_LIST:
	    emsg(_(e_using_list_as_float));
	    اكسر;
	حالة VAR_TUPLE:
	    emsg(_(e_using_tuple_as_float));
	    اكسر;
	حالة VAR_DICT:
	    emsg(_(e_using_dictionary_as_float));
	    اكسر;
	حالة VAR_BOOL:
	    emsg(_(e_using_boolean_قيمة_as_float));
	    اكسر;
	حالة VAR_SPECIAL:
	    emsg(_(e_using_special_قيمة_as_float));
	    اكسر;
	حالة VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    emsg(_(e_using_job_as_float));
	    اكسر;
#endif
	حالة VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    emsg(_(e_using_channel_as_float));
	    اكسر;
#endif
	حالة VAR_BLOB:
	    emsg(_(e_using_blob_as_float));
	    اكسر;
	حالة VAR_CLASS:
	حالة VAR_TYPEALIAS:
	    check_typval_is_قيمة(varp);
	    اكسر;
	حالة VAR_OBJECT:
	    emsg(_(e_using_object_as_float));
	    اكسر;
	حالة VAR_VOID:
	    emsg(_(e_cannot_use_void_قيمة));
	    اكسر;
	حالة VAR_UNKNOWN:
	حالة VAR_ANY:
	حالة VAR_INSTR:
	    internal_error_no_abort("tv_get_float(UNKNOWN)");
	    اكسر;
    }
    إذا (error != عدم)
	*error = TRUE;
    أرجع 0;
}

    float_T
tv_get_float(typval_T *varp)
{
    أرجع tv_get_float_chk(varp, عدم);
}

/*
 * Give an error and return FAIL unless "args[idx]" is unknown
 */
    صحيح
check_for_unknown_arg(typval_T *وسائط, صحيح idx)
{
    إذا (وسائط[idx].v_type != VAR_UNKNOWN)
    {
	semsg(_(e_too_many_arguments), idx + 1);
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string.
 */
    صحيح
check_for_string_arg(typval_T *وسائط, صحيح idx)
{
    إذا (وسائط[idx].v_type != VAR_STRING)
    {
	semsg(_(e_string_required_for_argument_nr), idx + 1);
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a non-empty string.
 */
    صحيح
check_for_nonempty_string_arg(typval_T *وسائط, صحيح idx)
{
    إذا (check_for_string_arg(وسائط, idx) == FAIL)
	أرجع FAIL;
    إذا (وسائط[idx].vval.v_string == عدم || *وسائط[idx].vval.v_string == NUL)
    {
	semsg(_(e_non_empty_string_required_for_argument_nr), idx + 1);
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Check for an optional string argument at 'idx'
 */
    صحيح
check_for_opt_string_arg(typval_T *وسائط, صحيح idx)
{
    أرجع (وسائط[idx].v_type == VAR_UNKNOWN
	    || check_for_string_arg(وسائط, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a number.
 */
    صحيح
check_for_رقم_arg(typval_T *وسائط, صحيح idx)
{
    إذا (وسائط[idx].v_type != VAR_رقم)
    {
	semsg(_(e_رقم_required_for_argument_nr), idx + 1);
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Check for an optional number argument at 'idx'
 */
    صحيح
check_for_opt_رقم_arg(typval_T *وسائط, صحيح idx)
{
    أرجع (وسائط[idx].v_type == VAR_UNKNOWN
	    || check_for_رقم_arg(وسائط, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a float or a number.
 */
    صحيح
check_for_float_or_nr_arg(typval_T *وسائط, صحيح idx)
{
    إذا (وسائط[idx].v_type != VAR_FLOAT && وسائط[idx].v_type != VAR_رقم)
    {
	semsg(_(e_float_or_رقم_required_for_argument_nr), idx + 1);
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a bool.
 */
    صحيح
check_for_bool_arg(typval_T *وسائط, صحيح idx)
{
    إذا (وسائط[idx].v_type != VAR_BOOL
	    && !(وسائط[idx].v_type == VAR_رقم
		&& (وسائط[idx].vval.v_رقم == 0
		    || وسائط[idx].vval.v_رقم == 1)))
    {
	semsg(_(e_bool_required_for_argument_nr), idx + 1);
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a bool or a number.
 */
    ساكن صحيح
check_for_bool_or_رقم_arg(typval_T *وسائط, صحيح idx)
{
    إذا (وسائط[idx].v_type != VAR_BOOL && وسائط[idx].v_type != VAR_رقم)
    {
	semsg(_(e_bool_or_رقم_required_for_argument_nr), idx + 1);
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Check for an optional bool argument at 'idx'.
 * Return FAIL if the type is wrong.
 */
    صحيح
check_for_opt_bool_arg(typval_T *وسائط, صحيح idx)
{
    إذا (وسائط[idx].v_type == VAR_UNKNOWN)
	أرجع OK;
    أرجع check_for_bool_arg(وسائط, idx);
}

/*
 * Check for an optional bool or number argument at 'idx'.
 * Return FAIL if the type is wrong.
 */
    صحيح
check_for_opt_bool_or_رقم_arg(typval_T *وسائط, صحيح idx)
{
    إذا (وسائط[idx].v_type == VAR_UNKNOWN)
	أرجع OK;
    أرجع check_for_bool_or_رقم_arg(وسائط, idx);
}

/*
 * Check for an optional bool or dict argument at 'idx'.
 * Return FAIL if the type is wrong.
 */
    صحيح
check_for_opt_bool_or_dict_arg(typval_T *وسائط, صحيح idx)
{
    إذا (وسائط[idx].v_type == VAR_UNKNOWN || وسائط[idx].v_type == VAR_DICT)
	أرجع OK;
    أرجع check_for_bool_arg(وسائط, idx);
}

/*
 * Give an error and return FAIL unless "args[idx]" is a blob.
 */
    صحيح
check_for_blob_arg(typval_T *وسائط, صحيح idx)
{
    إذا (وسائط[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_blob_required_for_argument_nr), idx + 1);
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list.
 */
    صحيح
check_for_list_arg(typval_T *وسائط, صحيح idx)
{
    إذا (وسائط[idx].v_type != VAR_LIST)
    {
	semsg(_(e_list_required_for_argument_nr), idx + 1);
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a non-NULL list.
 */
    صحيح
check_for_nonnull_list_arg(typval_T *وسائط, صحيح idx)
{
    إذا (check_for_list_arg(وسائط, idx) == FAIL)
	أرجع FAIL;

    إذا (وسائط[idx].vval.v_list == عدم)
    {
	semsg(_(e_non_null_list_required_for_argument_nr), idx + 1);
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Check for an optional list argument at 'idx'
 */
    صحيح
check_for_opt_list_arg(typval_T *وسائط, صحيح idx)
{
    أرجع (وسائط[idx].v_type == VAR_UNKNOWN
	    || check_for_list_arg(وسائط, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a tuple.
 */
    صحيح
check_for_tuple_arg(typval_T *وسائط, صحيح idx)
{
    إذا (وسائط[idx].v_type != VAR_TUPLE)
    {
	semsg(_(e_tuple_required_for_argument_nr), idx + 1);
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a dict.
 */
    صحيح
check_for_dict_arg(typval_T *وسائط, صحيح idx)
{
    إذا (وسائط[idx].v_type != VAR_DICT)
    {
	semsg(_(e_dict_required_for_argument_nr), idx + 1);
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a non-NULL dict.
 */
    صحيح
check_for_nonnull_dict_arg(typval_T *وسائط, صحيح idx)
{
    إذا (check_for_dict_arg(وسائط, idx) == FAIL)
	أرجع FAIL;

    إذا (وسائط[idx].vval.v_dict == عدم)
    {
	semsg(_(e_non_null_dict_required_for_argument_nr), idx + 1);
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Check for an optional dict argument at 'idx'
 */
    صحيح
check_for_opt_dict_arg(typval_T *وسائط, صحيح idx)
{
    أرجع (وسائط[idx].v_type == VAR_UNKNOWN
	    || check_for_dict_arg(وسائط, idx) != FAIL) ? OK : FAIL;
}

/*
 * Check for an optional non-NULL dict argument at 'idx'
 */
    صحيح
check_for_opt_nonnull_dict_arg(typval_T *وسائط, صحيح idx)
{
    أرجع (وسائط[idx].v_type == VAR_UNKNOWN
	    || check_for_nonnull_dict_arg(وسائط, idx) != FAIL) ? OK : FAIL;
}

#إذا defined(FEAT_JOB_CHANNEL)
/*
 * Give an error and return FAIL unless "args[idx]" is a channel or a job.
 */
    صحيح
check_for_chan_or_job_arg(typval_T *وسائط, صحيح idx)
{
    إذا (وسائط[idx].v_type != VAR_CHANNEL && وسائط[idx].v_type != VAR_JOB)
    {
	semsg(_(e_chan_or_job_required_for_argument_nr), idx + 1);
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an optional channel or a
 * job.
 */
    صحيح
check_for_opt_chan_or_job_arg(typval_T *وسائط, صحيح idx)
{
    أرجع (وسائط[idx].v_type == VAR_UNKNOWN
	    || check_for_chan_or_job_arg(وسائط, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a job.
 */
    صحيح
check_for_job_arg(typval_T *وسائط, صحيح idx)
{
    إذا (وسائط[idx].v_type != VAR_JOB)
    {
	semsg(_(e_job_required_for_argument_nr), idx + 1);
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Check for an optional job argument at 'idx'.
 */
    صحيح
check_for_opt_job_arg(typval_T *وسائط, صحيح idx)
{
    أرجع (وسائط[idx].v_type == VAR_UNKNOWN
	    || check_for_job_arg(وسائط, idx) != FAIL) ? OK : FAIL;
}
#وإلا
/*
 * Give an error and return FAIL unless "args[idx]" is an optional channel or a
 * job.  Used without the +channel feature, thus only VAR_UNKNOWN is accepted.
 */
    صحيح
check_for_opt_chan_or_job_arg(typval_T *وسائط, صحيح idx)
{
    أرجع وسائط[idx].v_type == VAR_UNKNOWN ? OK : FAIL;
}
#endif

/*
 * Give an error and return FAIL unless "args[idx]" is a string or
 * a number.
 */
    صحيح
check_for_string_or_رقم_arg(typval_T *وسائط, صحيح idx)
{
    إذا (وسائط[idx].v_type != VAR_STRING && وسائط[idx].v_type != VAR_رقم)
    {
	semsg(_(e_string_or_رقم_required_for_argument_nr), idx + 1);
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Check for an optional string or number argument at 'idx'.
 */
    صحيح
check_for_opt_string_or_رقم_arg(typval_T *وسائط, صحيح idx)
{
    أرجع (وسائط[idx].v_type == VAR_UNKNOWN
	    || check_for_string_or_رقم_arg(وسائط, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a buffer number.
 * Buffer number can be a number or a string.
 */
    صحيح
check_for_buffer_arg(typval_T *وسائط, صحيح idx)
{
    أرجع check_for_string_or_رقم_arg(وسائط, idx);
}

/*
 * Check for an optional buffer argument at 'idx'
 */
    صحيح
check_for_opt_buffer_arg(typval_T *وسائط, صحيح idx)
{
    أرجع (وسائط[idx].v_type == VAR_UNKNOWN
	    || check_for_buffer_arg(وسائط, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a line number.
 * Line number can be a number or a string.
 */
    صحيح
check_for_lnum_arg(typval_T *وسائط, صحيح idx)
{
    أرجع check_for_string_or_رقم_arg(وسائط, idx);
}

/*
 * Check for an optional line number argument at 'idx'
 */
    صحيح
check_for_opt_lnum_arg(typval_T *وسائط, صحيح idx)
{
    أرجع (وسائط[idx].v_type == VAR_UNKNOWN
	    || check_for_lnum_arg(وسائط, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a blob.
 */
    صحيح
check_for_string_or_blob_arg(typval_T *وسائط, صحيح idx)
{
    إذا (وسائط[idx].v_type != VAR_STRING && وسائط[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_string_or_blob_required_for_argument_nr), idx + 1);
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a list.
 */
    صحيح
check_for_string_or_list_arg(typval_T *وسائط, صحيح idx)
{
    إذا (وسائط[idx].v_type != VAR_STRING && وسائط[idx].v_type != VAR_LIST)
    {
	semsg(_(e_string_or_list_required_for_argument_nr), idx + 1);
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string, a list, a
 * tuple or a blob.
 */
    صحيح
check_for_string_or_list_or_tuple_or_blob_arg(typval_T *وسائط, صحيح idx)
{
    إذا (وسائط[idx].v_type != VAR_STRING
	    && وسائط[idx].v_type != VAR_LIST
	    && وسائط[idx].v_type != VAR_TUPLE
	    && وسائط[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_string_list_tuple_or_blob_required_for_argument_nr), idx + 1);
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Check for an optional string or list argument at 'idx'
 */
    صحيح
check_for_opt_string_or_list_arg(typval_T *وسائط, صحيح idx)
{
    أرجع (وسائط[idx].v_type == VAR_UNKNOWN
	    || check_for_string_or_list_arg(وسائط, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a dict.
 */
    صحيح
check_for_string_or_dict_arg(typval_T *وسائط, صحيح idx)
{
    إذا (وسائط[idx].v_type != VAR_STRING && وسائط[idx].v_type != VAR_DICT)
    {
	semsg(_(e_string_or_dict_required_for_argument_nr), idx + 1);
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a number
 * or a list.
 */
    صحيح
check_for_string_or_رقم_or_list_arg(typval_T *وسائط, صحيح idx)
{
    إذا (وسائط[idx].v_type != VAR_STRING
	    && وسائط[idx].v_type != VAR_رقم
	    && وسائط[idx].v_type != VAR_LIST)
    {
	semsg(_(e_string_رقم_or_list_required_for_argument_nr), idx + 1);
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an optional string
 * or number or a list
 */
    صحيح
check_for_opt_string_or_رقم_or_list_arg(typval_T *وسائط, صحيح idx)
{
    أرجع (وسائط[idx].v_type == VAR_UNKNOWN
	    || check_for_string_or_رقم_or_list_arg(وسائط, idx)
							!= FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string, a number, a
 * list, a tuple or a blob.
 */
    صحيح
check_for_repeat_func_arg(typval_T *وسائط, صحيح idx)
{
    إذا (وسائط[idx].v_type != VAR_STRING
	    && وسائط[idx].v_type != VAR_رقم
	    && وسائط[idx].v_type != VAR_LIST
	    && وسائط[idx].v_type != VAR_TUPLE
	    && وسائط[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_repeatable_type_required_for_argument_nr), idx + 1);
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string, a list, a
 * tuple or a dict.
 */
    صحيح
check_for_string_list_tuple_or_dict_arg(typval_T *وسائط, صحيح idx)
{
    إذا (وسائط[idx].v_type != VAR_STRING
	    && وسائط[idx].v_type != VAR_LIST
	    && وسائط[idx].v_type != VAR_TUPLE
	    && وسائط[idx].v_type != VAR_DICT)
    {
	semsg(_(e_string_list_tuple_or_dict_required_for_argument_nr), idx + 1);
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string
 * or a function reference.
 */
    صحيح
check_for_string_or_func_arg(typval_T *وسائط, صحيح idx)
{
    إذا (وسائط[idx].v_type != VAR_PARTIAL
	    && وسائط[idx].v_type != VAR_FUNC
	    && وسائط[idx].v_type != VAR_STRING)
    {
	semsg(_(e_string_or_function_required_for_argument_nr), idx + 1);
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list or a blob.
 */
    صحيح
check_for_list_or_blob_arg(typval_T *وسائط, صحيح idx)
{
    إذا (وسائط[idx].v_type != VAR_LIST && وسائط[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_list_or_blob_required_for_argument_nr), idx + 1);
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list or a tuple.
 */
    صحيح
check_for_list_or_tuple_arg(typval_T *وسائط, صحيح idx)
{
    إذا (وسائط[idx].v_type != VAR_LIST && وسائط[idx].v_type != VAR_TUPLE)
    {
	semsg(_(e_list_or_tuple_required_for_argument_nr), idx + 1);
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list, a tuple or a
 * blob.
 */
    صحيح
check_for_list_or_tuple_or_blob_arg(typval_T *وسائط, صحيح idx)
{
    إذا (وسائط[idx].v_type != VAR_LIST
	    && وسائط[idx].v_type != VAR_TUPLE
	    && وسائط[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_list_or_tuple_or_blob_required_for_argument_nr), idx + 1);
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list, a tuple or a
 * dict
 */
    صحيح
check_for_list_or_tuple_or_dict_arg(typval_T *وسائط, صحيح idx)
{
    إذا (وسائط[idx].v_type != VAR_LIST
	    && وسائط[idx].v_type != VAR_TUPLE
	    && وسائط[idx].v_type != VAR_DICT)
    {
	semsg(_(e_list_or_tuple_or_dict_required_for_argument_nr), idx + 1);
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list or dict or a
 * blob.
 */
    صحيح
check_for_list_or_dict_or_blob_arg(typval_T *وسائط, صحيح idx)
{
    إذا (وسائط[idx].v_type != VAR_LIST
	    && وسائط[idx].v_type != VAR_DICT
	    && وسائط[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_list_dict_or_blob_required_for_argument_nr), idx + 1);
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list, a tuple, a dict,
 * a blob or a string.
 */
    صحيح
check_for_list_tuple_dict_blob_or_string_arg(typval_T *وسائط, صحيح idx)
{
    إذا (وسائط[idx].v_type != VAR_LIST
	    && وسائط[idx].v_type != VAR_TUPLE
	    && وسائط[idx].v_type != VAR_DICT
	    && وسائط[idx].v_type != VAR_BLOB
	    && وسائط[idx].v_type != VAR_STRING)
    {
	semsg(_(e_list_tuple_dict_blob_or_string_required_for_argument_nr), idx + 1);
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an optional buffer
 * number or a dict.
 */
    صحيح
check_for_opt_buffer_or_dict_arg(typval_T *وسائط, صحيح idx)
{
    إذا (وسائط[idx].v_type != VAR_UNKNOWN
	    && وسائط[idx].v_type != VAR_STRING
	    && وسائط[idx].v_type != VAR_رقم
	    && وسائط[idx].v_type != VAR_DICT)
    {
	semsg(_(e_string_required_for_argument_nr), idx + 1);
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an object.
 */
    صحيح
check_for_object_arg(typval_T *وسائط, صحيح idx)
{
    إذا (وسائط[idx].v_type != VAR_OBJECT)
    {
	semsg(_(e_object_required_for_argument_nr), idx + 1);
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Returns TRUE if "tv" is a type alias for a class
 */
    ساكن صحيح
tv_class_alias(typval_T *tv)
{
    أرجع tv->v_type == VAR_TYPEALIAS &&
			tv->vval.v_typealias->ta_type->tt_type == VAR_OBJECT;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a class
 * or class typealias.
 */
    صحيح
check_for_class_or_typealias_وسائط(typval_T *وسائط, صحيح idx)
{
    لكل (صحيح i = idx; وسائط[i].v_type != VAR_UNKNOWN; ++i)
    {
	إذا (وسائط[i].v_type != VAR_CLASS && !tv_class_alias(&وسائط[idx]))
	{
	    semsg(_(e_class_or_typealias_required_for_argument_nr), i + 1);
	    أرجع FAIL;
	}
    }
    أرجع OK;
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
    ساكن char_u   mybuf[NUMBUFLEN];

    أرجع tv_get_string_buf(varp, mybuf);
}

/*
 * Like tv_get_string() but don't allow number to string conversion for Vim9.
 */
    char_u *
tv_get_string_strict(typval_T *varp)
{
    ساكن char_u   mybuf[NUMBUFLEN];
    char_u	    *res =  tv_get_string_buf_chk_strict(
						 varp, mybuf, in_vim9script());

    أرجع res != عدم ? res : (char_u *)"";
}

    char_u *
tv_get_string_buf(typval_T *varp, char_u *buf)
{
    char_u	*res = tv_get_string_buf_chk(varp, buf);

    أرجع res != عدم ? res : (char_u *)"";
}

/*
 * Careful: This uses a single, static buffer.  YOU CAN ONLY USE IT ONCE!
 */
    char_u *
tv_get_string_chk(typval_T *varp)
{
    ساكن char_u   mybuf[NUMBUFLEN];

    أرجع tv_get_string_buf_chk(varp, mybuf);
}

    char_u *
tv_get_string_buf_chk(typval_T *varp, char_u *buf)
{
    أرجع tv_get_string_buf_chk_strict(varp, buf, FALSE);
}

    char_u *
tv_get_string_buf_chk_strict(typval_T *varp, char_u *buf, صحيح strict)
{
    بدل (varp->v_type)
    {
	حالة VAR_رقم:
	    إذا (strict)
	    {
		emsg(_(e_using_رقم_as_string));
		اكسر;
	    }
	    vim_snprintf((حرف *)buf, NUMBUFLEN, "%lld",
					    (varnumber_T)varp->vval.v_رقم);
	    أرجع buf;
	حالة VAR_FUNC:
	حالة VAR_PARTIAL:
	    emsg(_(e_using_funcref_as_string));
	    اكسر;
	حالة VAR_LIST:
	    emsg(_(e_using_list_as_string));
	    اكسر;
	حالة VAR_TUPLE:
	    emsg(_(e_using_tuple_as_string));
	    اكسر;
	حالة VAR_DICT:
	    emsg(_(e_using_dictionary_as_string));
	    اكسر;
	حالة VAR_FLOAT:
	    إذا (strict)
	    {
		emsg(_(e_using_float_as_string));
		اكسر;
	    }
	    vim_snprintf((حرف *)buf, NUMBUFLEN, "%g", varp->vval.v_float);
	    أرجع buf;
	حالة VAR_STRING:
	    إذا (varp->vval.v_string != عدم)
		أرجع varp->vval.v_string;
	    أرجع (char_u *)"";
	حالة VAR_BOOL:
	حالة VAR_SPECIAL:
	    STRCPY(buf, get_var_special_name(varp->vval.v_رقم));
	    أرجع buf;
	حالة VAR_BLOB:
	    emsg(_(e_using_blob_as_string));
	    اكسر;
	حالة VAR_CLASS:
	حالة VAR_TYPEALIAS:
	    check_typval_is_قيمة(varp);
	    اكسر;
	حالة VAR_OBJECT:
	    {
		إذا (varp->vval.v_object == عدم)
		    emsg(_(e_using_object_as_string));
		وإلا
		{
		    class_T *cl = varp->vval.v_object->obj_class;
		    إذا (cl != عدم && IS_ENUM(cl))
			semsg(_(e_using_enum_str_as_string), cl->class_name.string);
		    وإلا
			emsg(_(e_using_object_as_string));
		}
	    }
	    اكسر;
	حالة VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    إذا (in_vim9script())
	    {
		semsg(_(e_using_invalid_قيمة_as_string_str), "job");
		اكسر;
	    }
	    أرجع job_to_string_buf(varp, buf);
#endif
	    اكسر;
	حالة VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    إذا (in_vim9script())
	    {
		semsg(_(e_using_invalid_قيمة_as_string_str), "channel");
		اكسر;
	    }
	    أرجع channel_to_string_buf(varp, buf);
#endif
	    اكسر;
	حالة VAR_VOID:
	    emsg(_(e_cannot_use_void_قيمة));
	    اكسر;
	حالة VAR_UNKNOWN:
	حالة VAR_ANY:
	حالة VAR_INSTR:
	    semsg(_(e_using_invalid_قيمة_as_string_str),
						  vartype_name(varp->v_type));
	    اكسر;
    }
    أرجع عدم;
}

/*
 * Turn a typeval into a string.  Similar to tv_get_string_buf() but uses
 * string() on Dict, List, etc.
 */
    char_u *
tv_stringify(typval_T *varp, char_u *buf)
{
    إذا (varp->v_type == VAR_LIST
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
	أرجع tmp.vval.v_string;
    }
    أرجع tv_get_string_buf(varp, buf);
}

/*
 * Return TRUE if typeval "tv" and its value are set to be locked (immutable).
 * Also give an error message, using "name" or _("name") when use_gettext is
 * TRUE.
 */
    صحيح
tv_check_lock(typval_T *tv, char_u *name, صحيح use_gettext)
{
    صحيح	lock = 0;

    بدل (tv->v_type)
    {
	حالة VAR_BLOB:
	    إذا (tv->vval.v_blob != عدم)
		lock = tv->vval.v_blob->bv_lock;
	    اكسر;
	حالة VAR_LIST:
	    إذا (tv->vval.v_list != عدم)
		lock = tv->vval.v_list->lv_lock;
	    اكسر;
	حالة VAR_TUPLE:
	    إذا (tv->vval.v_tuple != عدم)
		lock = tv->vval.v_tuple->tv_lock;
	    اكسر;
	حالة VAR_DICT:
	    إذا (tv->vval.v_dict != عدم)
		lock = tv->vval.v_dict->dv_lock;
	    اكسر;
	افتراضي:
	    اكسر;
    }
    أرجع قيمة_check_lock(tv->v_lock, name, use_gettext)
		   || (lock != 0 && قيمة_check_lock(lock, name, use_gettext));
}

/*
 * Copy the values from typval_T "from" to typval_T "to".
 * When needed allocates string or increases reference count.
 * Does not make a copy of a list, blob or dict but copies the reference!
 * It is OK for "from" and "to" to point to the same item.  This is used to
 * make a copy later.
 */
    صحيح
copy_tv(typval_T *from, typval_T *to)
{
    صحيح		ret = OK;

    to->v_type = from->v_type;
    to->v_lock = 0;
    بدل (from->v_type)
    {
	حالة VAR_رقم:
	حالة VAR_BOOL:
	حالة VAR_SPECIAL:
	    to->vval.v_رقم = from->vval.v_رقم;
	    اكسر;
	حالة VAR_FLOAT:
	    to->vval.v_float = from->vval.v_float;
	    اكسر;
	حالة VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    to->vval.v_job = from->vval.v_job;
	    إذا (to->vval.v_job != عدم)
		++to->vval.v_job->jv_refcount;
	    اكسر;
#endif
	حالة VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    to->vval.v_channel = from->vval.v_channel;
	    إذا (to->vval.v_channel != عدم)
		++to->vval.v_channel->ch_refcount;
	    اكسر;
#endif
	حالة VAR_INSTR:
	    to->vval.v_instr = from->vval.v_instr;
	    اكسر;

	حالة VAR_CLASS:
	    copy_class(from, to);
	    اكسر;

	حالة VAR_OBJECT:
	    copy_object(from, to);
	    اكسر;

	حالة VAR_STRING:
	حالة VAR_FUNC:
	    إذا (from->vval.v_string == عدم)
		to->vval.v_string = عدم;
	    وإلا
	    {
		to->vval.v_string = vim_strsave(from->vval.v_string);
		إذا (from->v_type == VAR_FUNC)
		    func_ref(to->vval.v_string);
	    }
	    اكسر;
	حالة VAR_PARTIAL:
	    إذا (from->vval.v_partial == عدم)
		to->vval.v_partial = عدم;
	    وإلا
	    {
		to->vval.v_partial = from->vval.v_partial;
		++to->vval.v_partial->pt_refcount;
	    }
	    اكسر;
	حالة VAR_BLOB:
	    إذا (from->vval.v_blob == عدم)
		to->vval.v_blob = عدم;
	    وإلا
	    {
		to->vval.v_blob = from->vval.v_blob;
		++to->vval.v_blob->bv_refcount;
	    }
	    اكسر;
	حالة VAR_LIST:
	    إذا (from->vval.v_list == عدم)
		to->vval.v_list = عدم;
	    وإلا
	    {
		to->vval.v_list = from->vval.v_list;
		++to->vval.v_list->lv_refcount;
	    }
	    اكسر;
	حالة VAR_TUPLE:
	    إذا (from->vval.v_tuple == عدم)
		to->vval.v_tuple = عدم;
	    وإلا
	    {
		to->vval.v_tuple = from->vval.v_tuple;
		++to->vval.v_tuple->tv_refcount;
	    }
	    اكسر;
	حالة VAR_DICT:
	    إذا (from->vval.v_dict == عدم)
		to->vval.v_dict = عدم;
	    وإلا
	    {
		to->vval.v_dict = from->vval.v_dict;
		++to->vval.v_dict->dv_refcount;
	    }
	    اكسر;
	حالة VAR_TYPEALIAS:
	    إذا (from->vval.v_typealias == عدم)
		to->vval.v_typealias = عدم;
	    وإلا
	    {
		to->vval.v_typealias = from->vval.v_typealias;
		++to->vval.v_typealias->ta_refcount;
	    }
	    اكسر;
	حالة VAR_VOID:
	    emsg(_(e_cannot_use_void_قيمة));
	    ret = FAIL;
	    اكسر;
	حالة VAR_UNKNOWN:
	حالة VAR_ANY:
	    internal_error_no_abort("copy_tv(UNKNOWN)");
	    ret = FAIL;
	    اكسر;
    }

    أرجع ret;
}

/*
 * Compare "tv1" and "tv2".
 */
    صحيح
typval_compare2(
    typval_T	*tv1,	// first operand
    typval_T	*tv2,	// second operand
    exprtype_T	type,	// operator
    صحيح		ic,	// ignore case
    صحيح		*res)	// comparison result
{
    varnumber_T	n1, n2;
    صحيح		type_is = type == تعبير_IS || type == تعبير_ISNOT;

    إذا (check_typval_is_قيمة(tv1) == FAIL
	|| check_typval_is_قيمة(tv2) == FAIL)
	أرجع FAIL;
    وإلا إذا (type_is && tv1->v_type != tv2->v_type)
    {
	// For "is" a different type always means FALSE, for "isnot"
	// it means TRUE.
	*res = (type == تعبير_ISNOT);
    }
    وإلا إذا (((tv1->v_type == VAR_SPECIAL && tv1->vval.v_رقم == VVAL_NULL)
		|| (tv2->v_type == VAR_SPECIAL
					   && tv2->vval.v_رقم == VVAL_NULL))
	    && tv1->v_type != tv2->v_type
	    && (type == تعبير_EQUAL || type == تعبير_NEQUAL))
    {
	n1 = typval_compare_null(tv1, tv2);
	إذا (n1 == MAYBE)
	    أرجع FAIL;
	إذا (type == تعبير_NEQUAL)
	    n1 = !n1;
	*res = n1;
    }
    وإلا إذا (tv1->v_type == VAR_BLOB || tv2->v_type == VAR_BLOB)
    {
	إذا (typval_compare_blob(tv1, tv2, type, res) == FAIL)
	    أرجع FAIL;
    }
    وإلا إذا (tv1->v_type == VAR_LIST || tv2->v_type == VAR_LIST)
    {
	إذا (typval_compare_list(tv1, tv2, type, ic, res) == FAIL)
	    أرجع FAIL;
    }
    وإلا إذا (tv1->v_type == VAR_TUPLE || tv2->v_type == VAR_TUPLE)
    {
	إذا (typval_compare_tuple(tv1, tv2, type, ic, res) == FAIL)
	    أرجع FAIL;
    }
    وإلا إذا (tv1->v_type == VAR_OBJECT || tv2->v_type == VAR_OBJECT)
    {
	إذا (typval_compare_object(tv1, tv2, type, ic, res) == FAIL)
	    أرجع FAIL;
    }
    وإلا إذا (tv1->v_type == VAR_DICT || tv2->v_type == VAR_DICT)
    {
	إذا (typval_compare_dict(tv1, tv2, type, ic, res) == FAIL)
	    أرجع FAIL;
    }
    وإلا إذا (tv1->v_type == VAR_FUNC || tv2->v_type == VAR_FUNC
	|| tv1->v_type == VAR_PARTIAL || tv2->v_type == VAR_PARTIAL)
    {
	إذا (typval_compare_func(tv1, tv2, type, ic, res) == FAIL)
	    أرجع FAIL;
    }

    // If one of the two variables is a float, compare as a float.
    // When using "=~" or "!~", always compare as string.
    وإلا إذا ((tv1->v_type == VAR_FLOAT || tv2->v_type == VAR_FLOAT)
	    && type != تعبير_MATCH && type != تعبير_NOMATCH)
    {
	float_T f1, f2;
	صحيح	error = FALSE;

	f1 = tv_get_float_chk(tv1, &error);
	إذا (!error)
	    f2 = tv_get_float_chk(tv2, &error);
	إذا (error)
	    أرجع FAIL;
	n1 = FALSE;
	بدل (type)
	{
	    حالة تعبير_IS:
	    حالة تعبير_EQUAL:    n1 = (f1 == f2); اكسر;
	    حالة تعبير_ISNOT:
	    حالة تعبير_NEQUAL:   n1 = (f1 != f2); اكسر;
	    حالة تعبير_GREATER:  n1 = (f1 > f2); اكسر;
	    حالة تعبير_GEQUAL:   n1 = (f1 >= f2); اكسر;
	    حالة تعبير_SMALLER:  n1 = (f1 < f2); اكسر;
	    حالة تعبير_SEQUAL:   n1 = (f1 <= f2); اكسر;
	    حالة تعبير_UNKNOWN:
	    حالة تعبير_MATCH:
	    افتراضي:  اكسر;  // avoid gcc warning
	}
	*res = n1;
    }

    // If one of the two variables is a number, compare as a number.
    // When using "=~" or "!~", always compare as string.
    وإلا إذا ((tv1->v_type == VAR_رقم || tv2->v_type == VAR_رقم)
	    && type != تعبير_MATCH && type != تعبير_NOMATCH)
    {
	صحيح error = FALSE;

	n1 = tv_get_رقم_chk(tv1, &error);
	إذا (!error)
	    n2 = tv_get_رقم_chk(tv2, &error);
	إذا (error)
	    أرجع FAIL;
	بدل (type)
	{
	    حالة تعبير_IS:
	    حالة تعبير_EQUAL:    n1 = (n1 == n2); اكسر;
	    حالة تعبير_ISNOT:
	    حالة تعبير_NEQUAL:   n1 = (n1 != n2); اكسر;
	    حالة تعبير_GREATER:  n1 = (n1 > n2); اكسر;
	    حالة تعبير_GEQUAL:   n1 = (n1 >= n2); اكسر;
	    حالة تعبير_SMALLER:  n1 = (n1 < n2); اكسر;
	    حالة تعبير_SEQUAL:   n1 = (n1 <= n2); اكسر;
	    حالة تعبير_UNKNOWN:
	    حالة تعبير_MATCH:
	    افتراضي:  اكسر;  // avoid gcc warning
	}
	*res = n1;
    }
    وإلا إذا (in_vim9script() && (tv1->v_type == VAR_BOOL
				    || tv2->v_type == VAR_BOOL
				    || (tv1->v_type == VAR_SPECIAL
					      && tv2->v_type == VAR_SPECIAL)))
    {
	إذا (tv1->v_type != tv2->v_type)
	{
	    semsg(_(e_cannot_compare_str_with_str),
		       vartype_name(tv1->v_type), vartype_name(tv2->v_type));
	    أرجع FAIL;
	}
	n1 = tv1->vval.v_رقم;
	n2 = tv2->vval.v_رقم;
	بدل (type)
	{
	    حالة تعبير_IS:
	    حالة تعبير_EQUAL:    n1 = (n1 == n2); اكسر;
	    حالة تعبير_ISNOT:
	    حالة تعبير_NEQUAL:   n1 = (n1 != n2); اكسر;
	    افتراضي:
		semsg(_(e_invalid_operation_for_str),
						   vartype_name(tv1->v_type));
		أرجع FAIL;
	}
	*res = n1;
    }
#ifdef FEAT_JOB_CHANNEL
    وإلا إذا (tv1->v_type == tv2->v_type
	    && (tv1->v_type == VAR_CHANNEL || tv1->v_type == VAR_JOB)
	    && (type == تعبير_NEQUAL || type == تعبير_EQUAL))
    {
	إذا (tv1->v_type == VAR_CHANNEL)
	    n1 = tv1->vval.v_channel == tv2->vval.v_channel;
	وإلا
	    n1 = tv1->vval.v_job == tv2->vval.v_job;
	إذا (type == تعبير_NEQUAL)
	    n1 = !n1;
	*res = n1;
    }
#endif
    وإلا
    {
	إذا (typval_compare_string(tv1, tv2, type, ic, res) == FAIL)
	    أرجع FAIL;
    }

    أرجع OK;
}

/*
 * Compare "tv1" and "tv2".
 * Put the result in "tv1".  Caller should clear "tv2".
 */
    صحيح
typval_compare(
    typval_T	*tv1,	// first operand
    typval_T	*tv2,	// second operand
    exprtype_T	type,   // operator
    صحيح		ic)     // ignore case
{
    صحيح res;

    إذا (typval_compare2(tv1, tv2, type, ic, &res) == FAIL)
    {
	clear_tv(tv1);
	أرجع FAIL;
    }

    clear_tv(tv1);
    إذا (in_vim9script())
    {
	tv1->v_type = VAR_BOOL;
	tv1->vval.v_رقم = res ? VVAL_TRUE : VVAL_FALSE;
    }
    وإلا
    {
	tv1->v_type = VAR_رقم;
	tv1->vval.v_رقم = res;
    }

    أرجع OK;
}

/*
 * Compare "tv1" to "tv2" as lists according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    صحيح
typval_compare_list(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	صحيح	    ic,
	صحيح	    *res)
{
    صحيح	    val = 0;

    إذا (type == تعبير_IS || type == تعبير_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
				      && tv1->vval.v_list == tv2->vval.v_list);
	إذا (type == تعبير_ISNOT)
	    val = !val;
    }
    وإلا إذا (tv1->v_type != tv2->v_type
	    || (type != تعبير_EQUAL && type != تعبير_NEQUAL))
    {
	إذا (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_list_with_list));
	وإلا
	    emsg(_(e_invalid_operation_for_list));
	أرجع FAIL;
    }
    وإلا
    {
	val = list_equal(tv1->vval.v_list, tv2->vval.v_list, ic);
	إذا (type == تعبير_NEQUAL)
	    val = !val;
    }
    *res = val;
    أرجع OK;
}

/*
 * Compare "tv1" to "tv2" as tuples according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    صحيح
typval_compare_tuple(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	صحيح	    ic,
	صحيح	    *res)
{
    صحيح	    val = 0;

    إذا (type == تعبير_IS || type == تعبير_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
			      && tv1->vval.v_tuple == tv2->vval.v_tuple);
	إذا (type == تعبير_ISNOT)
	    val = !val;
    }
    وإلا إذا (tv1->v_type != tv2->v_type
	    || (type != تعبير_EQUAL && type != تعبير_NEQUAL))
    {
	إذا (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_tuple_with_tuple));
	وإلا
	    emsg(_(e_invalid_operation_for_tuple));
	أرجع FAIL;
    }
    وإلا
    {
	val = tuple_equal(tv1->vval.v_tuple, tv2->vval.v_tuple, ic);
	إذا (type == تعبير_NEQUAL)
	    val = !val;
    }
    *res = val;
    أرجع OK;
}

/*
 * Compare v:null with another type.  Return TRUE if the value is NULL.
 */
    صحيح
typval_compare_null(typval_T *tv1, typval_T *tv2)
{
    إذا ((tv1->v_type == VAR_SPECIAL && tv1->vval.v_رقم == VVAL_NULL)
	    || (tv2->v_type == VAR_SPECIAL && tv2->vval.v_رقم == VVAL_NULL))
    {
	typval_T	*tv = tv1->v_type == VAR_SPECIAL ? tv2 : tv1;

	بدل (tv->v_type)
	{
	    حالة VAR_BLOB: أرجع tv->vval.v_blob == عدم;
#ifdef FEAT_JOB_CHANNEL
	    حالة VAR_CHANNEL: أرجع tv->vval.v_channel == عدم;
#endif
	    // TODO: null_class handling
	    // case VAR_CLASS: return tv->vval.v_class == NULL;
	    حالة VAR_DICT: أرجع tv->vval.v_dict == عدم;
	    حالة VAR_FUNC: أرجع tv->vval.v_string == عدم;
#ifdef FEAT_JOB_CHANNEL
	    حالة VAR_JOB: أرجع tv->vval.v_job == عدم;
#endif
	    حالة VAR_LIST: أرجع tv->vval.v_list == عدم;
	    حالة VAR_TUPLE: أرجع tv->vval.v_tuple == عدم;
	    حالة VAR_OBJECT: أرجع tv->vval.v_object == عدم;
	    حالة VAR_PARTIAL: أرجع tv->vval.v_partial == عدم;
	    حالة VAR_STRING: أرجع tv->vval.v_string == عدم;

	    حالة VAR_رقم: إذا (!in_vim9script())
				 أرجع tv->vval.v_رقم == 0;
			     اكسر;
	    حالة VAR_FLOAT: إذا (!in_vim9script())
				 أرجع tv->vval.v_float == 0.0;
			     اكسر;
	    حالة VAR_TYPEALIAS: أرجع tv->vval.v_typealias == عدم;
	    افتراضي: اكسر;
	}
    }
    // although comparing null with number, float or bool is not very useful
    // we won't give an error
    أرجع FALSE;
}

/*
 * Compare "tv1" to "tv2" as blobs according to "type".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    صحيح
typval_compare_blob(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	صحيح	    *res)
{
    صحيح	    val = 0;

    إذا (type == تعبير_IS || type == تعبير_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
			&& tv1->vval.v_blob == tv2->vval.v_blob);
	إذا (type == تعبير_ISNOT)
	    val = !val;
    }
    وإلا إذا (tv1->v_type != tv2->v_type
	    || (type != تعبير_EQUAL && type != تعبير_NEQUAL))
    {
	إذا (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_blob_with_blob));
	وإلا
	    emsg(_(e_invalid_operation_for_blob));
	أرجع FAIL;
    }
    وإلا
    {
	val = blob_equal(tv1->vval.v_blob, tv2->vval.v_blob);
	إذا (type == تعبير_NEQUAL)
	    val = !val;
    }
    *res = val;
    أرجع OK;
}

/*
 * Compare "tv1" to "tv2" as objects according to "type".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    صحيح
typval_compare_object(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	صحيح	    ic,
	صحيح	    *res)
{
    صحيح res_match = type == تعبير_EQUAL || type == تعبير_IS ? TRUE : FALSE;

    إذا (tv1->vval.v_object == عدم && tv2->vval.v_object == عدم)
    {
	*res = res_match;
	أرجع OK;
    }
    إذا (tv1->vval.v_object == عدم || tv2->vval.v_object == عدم)
    {
	*res = !res_match;
	أرجع OK;
    }

    object_T *obj1 = tv1->vval.v_object;
    object_T *obj2 = tv2->vval.v_object;
    إذا (type == تعبير_IS || type == تعبير_ISNOT)
    {
	*res = obj1 == obj2 ? res_match : !res_match;
	أرجع OK;
    }
    وإلا إذا (tv1->v_type != tv2->v_type)
    {
	emsg(_(e_can_only_compare_object_with_object));
	أرجع FAIL;
    }

    *res = object_equal(obj1, obj2, ic) ? res_match : !res_match;
    أرجع OK;
}

/*
 * Compare "tv1" to "tv2" as dictionaries according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    صحيح
typval_compare_dict(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	صحيح	    ic,
	صحيح	    *res)
{
    صحيح	    val;

    إذا (type == تعبير_IS || type == تعبير_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
			&& tv1->vval.v_dict == tv2->vval.v_dict);
	إذا (type == تعبير_ISNOT)
	    val = !val;
    }
    وإلا إذا (tv1->v_type != tv2->v_type
		|| (type != تعبير_EQUAL && type != تعبير_NEQUAL))
    {
	إذا (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_dictionary_with_dictionary));
	وإلا
	    emsg(_(e_invalid_operation_for_dictionary));
	أرجع FAIL;
    }
    وإلا
    {
	val = dict_equal(tv1->vval.v_dict, tv2->vval.v_dict, ic);
	إذا (type == تعبير_NEQUAL)
	    val = !val;
    }
    *res = val;
    أرجع OK;
}

/*
 * Compare "tv1" to "tv2" as funcrefs according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    صحيح
typval_compare_func(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	صحيح	    ic,
	صحيح	    *res)
{
    صحيح	    val = 0;

    إذا (type != تعبير_EQUAL && type != تعبير_NEQUAL
	    && type != تعبير_IS && type != تعبير_ISNOT)
    {
	emsg(_(e_invalid_operation_for_funcrefs));
	أرجع FAIL;
    }
    إذا ((tv1->v_type == VAR_PARTIAL && tv1->vval.v_partial == عدم)
	    || (tv2->v_type == VAR_PARTIAL && tv2->vval.v_partial == عدم))
	// When both partials are NULL, then they are equal.
	// Otherwise they are not equal.
	val = (tv1->vval.v_partial == tv2->vval.v_partial);
    وإلا إذا (type == تعبير_IS || type == تعبير_ISNOT)
    {
	إذا (tv1->v_type == VAR_FUNC && tv2->v_type == VAR_FUNC)
	    // strings are considered the same if their value is
	    // the same
	    val = tv_equal(tv1, tv2, ic);
	وإلا إذا (tv1->v_type == VAR_PARTIAL && tv2->v_type == VAR_PARTIAL)
	    val = (tv1->vval.v_partial == tv2->vval.v_partial);
	وإلا
	    val = FALSE;
    }
    وإلا
	val = tv_equal(tv1, tv2, ic);
    إذا (type == تعبير_NEQUAL || type == تعبير_ISNOT)
	val = !val;
    *res = val;
    أرجع OK;
}

/*
 * Compare "tv1" to "tv2" as strings according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    صحيح
typval_compare_string(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	صحيح	    ic,
	صحيح	    *res)
{
    صحيح		i = 0;
    صحيح		val = FALSE;
    char_u	*s1, *s2;
    char_u	buf1[NUMBUFLEN], buf2[NUMBUFLEN];

    إذا (in_vim9script()
	  && ((tv1->v_type != VAR_STRING && tv1->v_type != VAR_SPECIAL)
	   || (tv2->v_type != VAR_STRING && tv2->v_type != VAR_SPECIAL)))
    {
	semsg(_(e_cannot_compare_str_with_str),
		   vartype_name(tv1->v_type), vartype_name(tv2->v_type));
	أرجع FAIL;
    }
    s1 = tv_get_string_buf(tv1, buf1);
    s2 = tv_get_string_buf(tv2, buf2);
    إذا (type != تعبير_MATCH && type != تعبير_NOMATCH)
	i = ic ? MB_STRICMP(s1, s2) : STRCMP(s1, s2);
    بدل (type)
    {
	حالة تعبير_IS:	    إذا (in_vim9script())
			    {
				// Really check it is the same string, not just
				// the same value.
				val = tv1->vval.v_string == tv2->vval.v_string;
				اكسر;
			    }
			    // FALLTHROUGH
	حالة تعبير_EQUAL:    val = (i == 0); اكسر;
	حالة تعبير_ISNOT:    إذا (in_vim9script())
			    {
				// Really check it is not the same string, not
				// just a different value.
				val = tv1->vval.v_string != tv2->vval.v_string;
				اكسر;
			    }
			    // FALLTHROUGH
	حالة تعبير_NEQUAL:   val = (i != 0); اكسر;
	حالة تعبير_GREATER:  val = (i > 0); اكسر;
	حالة تعبير_GEQUAL:   val = (i >= 0); اكسر;
	حالة تعبير_SMALLER:  val = (i < 0); اكسر;
	حالة تعبير_SEQUAL:   val = (i <= 0); اكسر;

	حالة تعبير_MATCH:
	حالة تعبير_NOMATCH:
		val = pattern_match(s2, s1, ic);
		إذا (type == تعبير_NOMATCH)
		    val = !val;
		اكسر;

	افتراضي:  اكسر;  // avoid gcc warning
    }
    *res = val;
    أرجع OK;
}
/*
 * Convert any type to a string, never give an error.
 * When "quotes" is TRUE add quotes to a string.
 * Returns an allocated string.
 */
    char_u *
typval_tostring(typval_T *arg, صحيح quotes)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    char_u	*ret = عدم;

    إذا (arg == عدم)
	أرجع vim_strsave((char_u *)"(does not exist)");
    إذا (!quotes && arg->v_type == VAR_STRING)
    {
	ret = vim_strsave(arg->vval.v_string == عدم ? (char_u *)""
							 : arg->vval.v_string);
    }
    وإلا
    {
	ret = tv2string(arg, &tofree, numbuf, 0);
	// Make a copy if we have a value but it's not in allocated memory.
	إذا (ret != عدم && tofree == عدم)
	    ret = vim_strsave(ret);
    }
    أرجع ret;
}

/*
 * Return TRUE if typeval "tv" is locked: Either that value is locked itself
 * or it refers to a List or Dictionary that is locked.
 */
    صحيح
tv_islocked(typval_T *tv)
{
    أرجع (tv->v_lock & VAR_LOCKED)
	|| (tv->v_type == VAR_LIST
		&& tv->vval.v_list != عدم
		&& (tv->vval.v_list->lv_lock & VAR_LOCKED))
	|| (tv->v_type == VAR_TUPLE
		&& tv->vval.v_tuple != عدم
		&& (tv->vval.v_tuple->tv_lock & VAR_LOCKED))
	|| (tv->v_type == VAR_DICT
		&& tv->vval.v_dict != عدم
		&& (tv->vval.v_dict->dv_lock & VAR_LOCKED));
}

    ساكن صحيح
func_equal(
    typval_T *tv1,
    typval_T *tv2,
    صحيح	     ic)	    // ignore case
{
    char_u	*s1, *s2;
    dict_T	*d1, *d2;
    صحيح		a1, a2;
    صحيح		i;

    // empty and NULL function name considered the same
    s1 = tv1->v_type == VAR_FUNC ? tv1->vval.v_string
					   : partial_name(tv1->vval.v_partial);
    إذا (s1 != عدم && *s1 == NUL)
	s1 = عدم;
    s2 = tv2->v_type == VAR_FUNC ? tv2->vval.v_string
					   : partial_name(tv2->vval.v_partial);
    إذا (s2 != عدم && *s2 == NUL)
	s2 = عدم;
    إذا (s1 == عدم || s2 == عدم)
    {
	إذا (s1 != s2)
	    أرجع FALSE;
    }
    وإلا إذا (STRCMP(s1, s2) != 0)
	أرجع FALSE;

    // empty dict and NULL dict is different
    d1 = tv1->v_type == VAR_FUNC ? عدم : tv1->vval.v_partial->pt_dict;
    d2 = tv2->v_type == VAR_FUNC ? عدم : tv2->vval.v_partial->pt_dict;
    إذا (d1 == عدم || d2 == عدم)
    {
	إذا (d1 != d2)
	    أرجع FALSE;
    }
    وإلا إذا (!dict_equal(d1, d2, ic))
	أرجع FALSE;

    // empty list and no list considered the same
    a1 = tv1->v_type == VAR_FUNC ? 0 : tv1->vval.v_partial->pt_argc;
    a2 = tv2->v_type == VAR_FUNC ? 0 : tv2->vval.v_partial->pt_argc;
    إذا (a1 != a2)
	أرجع FALSE;
    لكل (i = 0; i < a1; ++i)
	إذا (!tv_equal(tv1->vval.v_partial->pt_argv + i,
		      tv2->vval.v_partial->pt_argv + i, ic))
	    أرجع FALSE;

    أرجع TRUE;
}

/*
 * Return TRUE if "tv1" and "tv2" have the same value.
 * Compares the items just like "==" would compare them, but strings and
 * numbers are different.  Floats and numbers are also different.
 */
    صحيح
tv_equal(
    typval_T *tv1,
    typval_T *tv2,
    صحيح	     ic)	    // ignore case
{
    char_u	buf1[NUMBUFLEN], buf2[NUMBUFLEN];
    char_u	*s1, *s2;
    ساكن صحيح  recursive_cnt = 0;	    // catch recursive loops
    صحيح		r;
    ساكن صحيح	tv_equal_recurse_limit;

    // Catch lists and dicts that have an endless loop by limiting
    // recursiveness to a limit.  We guess they are equal then.
    // A fixed limit has the problem of still taking an awful long time.
    // Reduce the limit every time running into it. That should work fine for
    // deeply linked structures that are not recursively linked and catch
    // recursiveness quickly.
    إذا (recursive_cnt == 0)
	tv_equal_recurse_limit = 1000;
    إذا (recursive_cnt >= tv_equal_recurse_limit)
    {
	--tv_equal_recurse_limit;
	أرجع TRUE;
    }

    // For VAR_FUNC and VAR_PARTIAL compare the function name, bound dict and
    // arguments.
    إذا ((tv1->v_type == VAR_FUNC
		|| (tv1->v_type == VAR_PARTIAL && tv1->vval.v_partial != عدم))
	    && (tv2->v_type == VAR_FUNC
		|| (tv2->v_type == VAR_PARTIAL && tv2->vval.v_partial != عدم)))
    {
	++recursive_cnt;
	r = func_equal(tv1, tv2, ic);
	--recursive_cnt;
	أرجع r;
    }

    إذا (tv1->v_type != tv2->v_type
	    && ((tv1->v_type != VAR_BOOL && tv1->v_type != VAR_SPECIAL)
		|| (tv2->v_type != VAR_BOOL && tv2->v_type != VAR_SPECIAL)))
	أرجع FALSE;

    بدل (tv1->v_type)
    {
	حالة VAR_LIST:
	    ++recursive_cnt;
	    r = list_equal(tv1->vval.v_list, tv2->vval.v_list, ic);
	    --recursive_cnt;
	    أرجع r;

	حالة VAR_TUPLE:
	    ++recursive_cnt;
	    r = tuple_equal(tv1->vval.v_tuple, tv2->vval.v_tuple, ic);
	    --recursive_cnt;
	    أرجع r;

	حالة VAR_DICT:
	    ++recursive_cnt;
	    r = dict_equal(tv1->vval.v_dict, tv2->vval.v_dict, ic);
	    --recursive_cnt;
	    أرجع r;

	حالة VAR_BLOB:
	    أرجع blob_equal(tv1->vval.v_blob, tv2->vval.v_blob);

	حالة VAR_رقم:
	حالة VAR_BOOL:
	حالة VAR_SPECIAL:
	    أرجع tv1->vval.v_رقم == tv2->vval.v_رقم;

	حالة VAR_STRING:
	    s1 = tv_get_string_buf(tv1, buf1);
	    s2 = tv_get_string_buf(tv2, buf2);
	    أرجع ((ic ? MB_STRICMP(s1, s2) : STRCMP(s1, s2)) == 0);

	حالة VAR_FLOAT:
	    أرجع tv1->vval.v_float == tv2->vval.v_float;
	حالة VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    أرجع tv1->vval.v_job == tv2->vval.v_job;
#endif
	حالة VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    أرجع tv1->vval.v_channel == tv2->vval.v_channel;
#endif
	حالة VAR_INSTR:
	    أرجع tv1->vval.v_instr == tv2->vval.v_instr;

	حالة VAR_CLASS:
	    // A class only exists once, equality is identity.
	    أرجع tv1->vval.v_class == tv2->vval.v_class;

	حالة VAR_OBJECT:
	    ++recursive_cnt;
	    r = object_equal(tv1->vval.v_object, tv2->vval.v_object, ic);
	    --recursive_cnt;
	    أرجع r;

	حالة VAR_PARTIAL:
	    أرجع tv1->vval.v_partial == tv2->vval.v_partial;

	حالة VAR_FUNC:
	    أرجع tv1->vval.v_string == tv2->vval.v_string;

	حالة VAR_TYPEALIAS:
	    أرجع tv1->vval.v_typealias == tv2->vval.v_typealias;

	حالة VAR_UNKNOWN:
	حالة VAR_ANY:
	حالة VAR_VOID:
	    اكسر;
    }

    // VAR_UNKNOWN can be the result of a invalid expression, let's say it
    // does not equal anything, not even itself.
    أرجع FALSE;
}

/*
 * Get an option value.
 * "arg" points to the '&' or '+' before the option name.
 * "arg" is advanced to character after the option name.
 * Return OK or FAIL.
 */
    صحيح
eval_option(
    char_u	**arg,
    typval_T	*rettv,	// when NULL, only check if option exists
    صحيح		evaluate)
{
    char_u	*option_end;
    long	numval;
    char_u	*stringval;
    getoption_T	opt_type;
    صحيح		c;
    صحيح		working = (**arg == '+');    // has("+option")
    صحيح		ret = OK;
    صحيح		scope;

    // Isolate the option name and find its value.
    option_end = find_option_end(arg, &scope);
    إذا (option_end == عدم)
    {
	إذا (rettv != عدم)
	    semsg(_(e_option_name_missing_str), *arg);
	أرجع FAIL;
    }

    إذا (!evaluate)
    {
	*arg = option_end;
	أرجع OK;
    }

    c = *option_end;
    *option_end = NUL;
    opt_type = get_option_قيمة(*arg, &numval,
			       rettv == عدم ? عدم : &stringval, عدم, scope);

    إذا (opt_type == gov_unknown)
    {
	إذا (rettv != عدم)
	    semsg(_(e_unknown_option_str), *arg);
	ret = FAIL;
    }
    وإلا إذا (rettv != عدم)
    {
	rettv->v_lock = 0;
	إذا (opt_type == gov_hidden_string)
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = عدم;
	}
	وإلا إذا (opt_type == gov_hidden_bool || opt_type == gov_hidden_رقم)
	{
	    rettv->v_type = in_vim9script() && opt_type == gov_hidden_bool
						       ? VAR_BOOL : VAR_رقم;
	    rettv->vval.v_رقم = 0;
	}
	وإلا إذا (opt_type == gov_bool || opt_type == gov_رقم)
	{
	    إذا (in_vim9script() && opt_type == gov_bool)
	    {
		rettv->v_type = VAR_BOOL;
		rettv->vval.v_رقم = numval ? VVAL_TRUE : VVAL_FALSE;
	    }
	    وإلا
	    {
		rettv->v_type = VAR_رقم;
		rettv->vval.v_رقم = numval;
	    }
	}
	وإلا				// string option
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = stringval;
	}
    }
    وإلا إذا (working && (opt_type == gov_hidden_bool
			|| opt_type == gov_hidden_رقم
			|| opt_type == gov_hidden_string))
	ret = FAIL;

    *option_end = c;		    // put back for error messages
    *arg = option_end;

    أرجع ret;
}

/*
 * Allocate a variable for a number constant.  Also deals with "0z" for blob.
 * Return OK or FAIL.
 */
    صحيح
eval_رقم(
	char_u	    **arg,
	typval_T    *rettv,
	صحيح	    evaluate,
	صحيح	    want_string)
{
    صحيح		len;
    صحيح		skip_quotes = !in_old_script(4);
    char_u	*p;
    صحيح		get_float = FALSE;

    // We accept a float when the format matches
    // "[0-9]\+\.[0-9]\+\([eE][+-]\?[0-9]\+\)\?".  This is very
    // strict to avoid backwards compatibility problems.
    // With script version 2 and later the leading digit can be
    // omitted.
    // Don't look for a float after the "." operator, so that
    // ":let vers = 1.2.3" doesn't fail.
    إذا (**arg == '.')
	p = *arg;
    وإلا
    {
	p = *arg + 1;
	إذا (skip_quotes)
	    لكل (;;)
	    {
		إذا (*p == '\'')
		    ++p;
		إذا (!vim_isdigit(*p))
		    اكسر;
		p = skipdigits(p);
	    }
	وإلا
	    p = skipdigits(p);
    }
    إذا (!want_string && p[0] == '.' && vim_isdigit(p[1]))
    {
	get_float = TRUE;
	p = skipdigits(p + 2);
	إذا (*p == 'e' || *p == 'E')
	{
	    ++p;
	    إذا (*p == '-' || *p == '+')
		++p;
	    إذا (!vim_isdigit(*p))
		get_float = FALSE;
	    وإلا
		p = skipdigits(p + 1);
	}
	إذا (ASCII_ISALPHA(*p) || *p == '.')
	    get_float = FALSE;
    }
    إذا (get_float)
    {
	float_T	f;

	*arg += string2float(*arg, &f, skip_quotes);
	إذا (evaluate)
	{
	    rettv->v_type = VAR_FLOAT;
	    rettv->vval.v_float = f;
	}
    }
    وإلا
    إذا (**arg == '0' && ((*arg)[1] == 'z' || (*arg)[1] == 'Z'))
    {
	char_u  *bp;
	blob_T  *blob = عدم;  // init for gcc

	// Blob constant: 0z0123456789abcdef
	إذا (evaluate)
	    blob = blob_alloc();
	لكل (bp = *arg + 2; vim_isxdigit(bp[0]); bp += 2)
	{
	    إذا (!vim_isxdigit(bp[1]))
	    {
		إذا (blob != عدم)
		{
		    emsg(_(e_blob_literal_should_have_an_even_رقم_of_hex_characters));
		    ga_clear(&blob->bv_ga);
		    VIM_CLEAR(blob);
		}
		أرجع FAIL;
	    }
	    إذا (blob != عدم)
		ga_append(&blob->bv_ga,
			     (hex2nr(*bp) << 4) + hex2nr(*(bp+1)));
	    إذا (bp[2] == '.' && vim_isxdigit(bp[3]))
		++bp;
	}
	إذا (blob != عدم)
	    rettv_blob_set(rettv, blob);
	*arg = bp;
    }
    وإلا
    {
	varnumber_T	n;

	// decimal, hex or octal number
	vim_str2nr(*arg, عدم, &len, skip_quotes
		      ? STR2NR_NO_OCT + STR2NR_QUOTE
		      : STR2NR_ALL, &n, عدم, 0, TRUE, عدم);
	إذا (len == 0)
	{
	    إذا (evaluate)
		semsg(_(e_invalid_expression_str), *arg);
	    أرجع FAIL;
	}
	*arg += len;
	إذا (evaluate)
	{
	    rettv->v_type = VAR_رقم;
	    rettv->vval.v_رقم = n;
	}
    }
    أرجع OK;
}

/*
 * Evaluate a string constant and put the result in "rettv".
 * "*arg" points to the double quote or to after it when "interpolate" is TRUE.
 * When "interpolate" is TRUE reduce "{{" to "{", reduce "}}" to "}" and stop
 * at a single "{".
 * Return OK or FAIL.
 */
    صحيح
eval_string(char_u **arg, typval_T *rettv, صحيح evaluate, صحيح interpolate)
{
    char_u	*p;
    char_u	*end;
    صحيح		extra = interpolate ? 1 : 0;
    صحيح		off = interpolate ? 0 : 1;
    صحيح		len;

    // Find the end of the string, skipping backslashed characters.
    لكل (p = *arg + off; *p != NUL && *p != '"'; MB_PTR_ADV(p))
    {
	إذا (*p == '\\' && p[1] != NUL)
	{
	    ++p;
	    // A "\<x>" form occupies at least 4 characters, and produces up
	    // to 9 characters (6 for the char and 3 for a modifier):
	    // reserve space for 5 extra.
	    إذا (*p == '<')
	    {
		صحيح		modifiers = 0;
		صحيح		flags = FSK_KEYCODE | FSK_IN_STRING;

		extra += 5;

		// Skip to the '>' to avoid using '{' inside for string
		// interpolation.
		إذا (p[1] != '*')
		    flags |= FSK_SIMPLIFY;
		إذا (find_special_مفتاح(&p, &modifiers, flags, عدم) != 0)
		    --p;  // leave "p" on the ">"
	    }
	}
	وإلا إذا (interpolate && (*p == '{' || *p == '}'))
	{
	    إذا (*p == '{' && p[1] != '{') // start of expression
		اكسر;
	    ++p;
	    إذا (p[-1] == '}' && *p != '}') // single '}' is an error
	    {
		semsg(_(e_stray_closing_curly_str), *arg);
		أرجع FAIL;
	    }
	    --extra;  // "{{" becomes "{", "}}" becomes "}"
	}
    }

    إذا (*p != '"' && !(interpolate && *p == '{'))
    {
	semsg(_(e_missing_double_quote_str), *arg);
	أرجع FAIL;
    }

    // If only parsing, set *arg and return here
    إذا (!evaluate)
    {
	*arg = p + off;
	أرجع OK;
    }

    // Copy the string into allocated memory, handling backslashed
    // characters.
    rettv->v_type = VAR_STRING;
    len = (صحيح)(p - *arg + extra);
    rettv->vval.v_string = alloc(len);
    إذا (rettv->vval.v_string == عدم)
	أرجع FAIL;
    end = rettv->vval.v_string;

    لكل (p = *arg + off; *p != NUL && *p != '"'; )
    {
	إذا (*p == '\\')
	{
	    بدل (*++p)
	    {
		حالة 'b': *end++ = BS; ++p; اكسر;
		حالة 'e': *end++ = ESC; ++p; اكسر;
		حالة 'f': *end++ = FF; ++p; اكسر;
		حالة 'n': *end++ = NL; ++p; اكسر;
		حالة 'r': *end++ = CAR; ++p; اكسر;
		حالة 't': *end++ = TAB; ++p; اكسر;

		حالة 'X': // hex: "\x1", "\x12"
		حالة 'x':
		حالة 'u': // Unicode: "\u0023"
		حالة 'U':
			  إذا (vim_isxdigit(p[1]))
			  {
			      صحيح	n, nr;
			      صحيح	c = SAFE_toupper(*p);

			      إذا (c == 'X')
				  n = 2;
			      وإلا إذا (*p == 'u')
				  n = 4;
			      وإلا
				  n = 8;
			      nr = 0;
			      طالما (--n >= 0 && vim_isxdigit(p[1]))
			      {
				  ++p;
				  nr = (nr << 4) + hex2nr(*p);
			      }
			      ++p;
			      // For "\u" store the number according to
			      // 'encoding'.
			      إذا (c != 'X')
				  end += (*mb_char2bytes)(nr, end);
			      وإلا
				  *end++ = nr;
			  }
			  اكسر;

			  // octal: "\1", "\12", "\123"
		حالة '0':
		حالة '1':
		حالة '2':
		حالة '3':
		حالة '4':
		حالة '5':
		حالة '6':
		حالة '7': *end = *p++ - '0';
			  إذا (*p >= '0' && *p <= '7')
			  {
			      *end = (*end << 3) + *p++ - '0';
			      إذا (*p >= '0' && *p <= '7')
				  *end = (*end << 3) + *p++ - '0';
			  }
			  ++end;
			  اكسر;

			  // Special key, e.g.: "\<C-W>"
		حالة '<':
			  {
			      صحيح flags = FSK_KEYCODE | FSK_IN_STRING;

			      إذا (p[1] != '*')
				  flags |= FSK_SIMPLIFY;
			      extra = trans_special(&p, end, flags, FALSE, عدم);
			      إذا (extra != 0)
			      {
				  end += extra;
				  إذا (end >= rettv->vval.v_string + len)
				      iemsg("eval_string() used more space than allocated");
				  اكسر;
			      }
			  }
			  // FALLTHROUGH

		افتراضي: MB_COPY_CHAR(p, end);
			  اكسر;
	    }
	}
	وإلا
	{
	    إذا (interpolate && (*p == '{' || *p == '}'))
	    {
		إذا (*p == '{' && p[1] != '{') // start of expression
		    اكسر;
		++p;  // reduce "{{" to "{" and "}}" to "}"
	    }
	    MB_COPY_CHAR(p, end);
	}
    }
    *end = NUL;
    إذا (*p == '"' && !interpolate)
	++p;
    *arg = p;

    أرجع OK;
}

/*
 * Allocate a variable for a 'str''ing' constant.
 * When "interpolate" is TRUE reduce "{{" to "{" and stop at a single "{".
 * Return OK when a "rettv" was set to the string.
 * Return FAIL on error, "rettv" is not set.
 */
    صحيح
eval_lit_string(char_u **arg, typval_T *rettv, صحيح evaluate, صحيح interpolate)
{
    char_u	*p;
    char_u	*str;
    صحيح		reduce = interpolate ? -1 : 0;
    صحيح		off = interpolate ? 0 : 1;

    // Find the end of the string, skipping ''.
    لكل (p = *arg + off; *p != NUL; MB_PTR_ADV(p))
    {
	إذا (*p == '\'')
	{
	    إذا (p[1] != '\'')
		اكسر;
	    ++reduce;
	    ++p;
	}
	وإلا إذا (interpolate)
	{
	    إذا (*p == '{')
	    {
		إذا (p[1] != '{')
		    اكسر;
		++p;
		++reduce;
	    }
	    وإلا إذا (*p == '}')
	    {
		++p;
		إذا (*p != '}')
		{
		    semsg(_(e_stray_closing_curly_str), *arg);
		    أرجع FAIL;
		}
		++reduce;
	    }
	}
    }

    إذا (*p != '\'' && !(interpolate && *p == '{'))
    {
	semsg(_(e_missing_single_quote_str), *arg);
	أرجع FAIL;
    }

    // If only parsing return after setting "*arg"
    إذا (!evaluate)
    {
	*arg = p + off;
	أرجع OK;
    }

    // Copy the string into allocated memory, handling '' to ' reduction and
    // any expressions.
    str = alloc((p - *arg) - reduce);
    إذا (str == عدم)
	أرجع FAIL;
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = str;

    لكل (p = *arg + off; *p != NUL; )
    {
	إذا (*p == '\'')
	{
	    إذا (p[1] != '\'')
		اكسر;
	    ++p;
	}
	وإلا إذا (interpolate && (*p == '{' || *p == '}'))
	{
	    إذا (*p == '{' && p[1] != '{')
		اكسر;
	    ++p;
	}
	MB_COPY_CHAR(p, str);
    }
    *str = NUL;
    *arg = p + off;

    أرجع OK;
}

/*
 * Evaluate a single or double quoted string possibly containing expressions.
 * "arg" points to the '$'.  The result is put in "rettv".
 * Returns OK or FAIL.
 */
    صحيح
eval_interp_string(char_u **arg, typval_T *rettv, صحيح evaluate)
{
    typval_T	tv;
    صحيح		ret = OK;
    صحيح		quote;
    garray_T	ga;
    char_u	*p;

    ga_init2(&ga, 1, 80);

    // *arg is on the '$' character, move it to the first string character.
    ++*arg;
    quote = **arg;
    ++*arg;

    لكل (;;)
    {
	// Get the string up to the matching quote or to a single '{'.
	// "arg" is advanced to either the quote or the '{'.
	إذا (quote == '"')
	    ret = eval_string(arg, &tv, evaluate, TRUE);
	وإلا
	    ret = eval_lit_string(arg, &tv, evaluate, TRUE);
	إذا (ret == FAIL)
	    اكسر;
	إذا (evaluate)
	{
	    ga_concat(&ga, tv.vval.v_string);
	    clear_tv(&tv);
	}

	إذا (**arg != '{')
	{
	    // found terminating quote
	    ++*arg;
	    اكسر;
	}
	p = eval_one_تعبير_in_str(*arg, &ga, evaluate);
	إذا (p == عدم)
	{
	    ret = FAIL;
	    اكسر;
	}
	*arg = p;
    }

    rettv->v_type = VAR_STRING;
    إذا (ret == FAIL || !evaluate || ga_append(&ga, NUL) == FAIL)
    {
	ga_clear(&ga);
	rettv->vval.v_string = عدم;
	أرجع ret;
    }

    rettv->vval.v_string = ga.ga_data;
    أرجع OK;
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
    صحيح		copyID)
{
    أرجع echo_string_core(tv, tofree, numbuf, copyID, FALSE, TRUE, FALSE);
}

/*
 * Get the value of an environment variable.
 * "arg" is pointing to the '$'.  It is advanced to after the name.
 * If the environment variable was not set, silently assume it is empty.
 * Return FAIL if the name is invalid.
 */
    صحيح
eval_env_var(char_u **arg, typval_T *rettv, صحيح evaluate)
{
    char_u	*string = عدم;
    صحيح		len;
    صحيح		cc;
    char_u	*name;
    صحيح		mustfree = FALSE;

    ++*arg;
    name = *arg;
    len = get_env_len(arg);
    إذا (evaluate)
    {
	إذا (len == 0)
	    أرجع FAIL; // invalid empty name

	cc = name[len];
	name[len] = NUL;
	// first try vim_getenv(), fast for normal environment vars
	string = vim_getenv(name, &mustfree);
	إذا (string != عدم && *string != NUL)
	{
	    إذا (!mustfree)
		string = vim_strsave(string);
	}
	وإلا
	{
	    إذا (mustfree)
		vim_free(string);

	    // next try expanding things like $VIM and ${HOME}
	    string = expand_env_save(name - 1);
	    إذا (string != عدم && *string == '$')
		VIM_CLEAR(string);
	}
	name[len] = cc;

	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = string;
	rettv->v_lock = 0;
    }

    أرجع OK;
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
    صحيح		did_emsg_before = did_emsg;

    إذا (argvars[0].v_type != VAR_STRING || !in_vim9script())
	lnum = (linenr_T)tv_get_رقم_chk(&argvars[0], عدم);
    إذا (lnum <= 0 && did_emsg_before == did_emsg
					    && argvars[0].v_type != VAR_رقم)
    {
	صحيح	fnum;
	pos_T	*fp;

	// no valid number, try using arg like line()
	fp = var2fpos(&argvars[0], TRUE, &fnum, FALSE);
	إذا (fp != عدم)
	    lnum = fp->lnum;
    }
    أرجع lnum;
}

/*
 * Get the lnum from the first argument.
 * Also accepts "$", then "buf" is used.
 * Returns 0 on error.
 */
    linenr_T
tv_get_lnum_buf(typval_T *argvars, buf_T *buf)
{
    إذا (argvars[0].v_type == VAR_STRING
	    && argvars[0].vval.v_string != عدم
	    && argvars[0].vval.v_string[0] == '$'
	    && argvars[0].vval.v_string[1] == NUL
	    && buf != عدم)
	أرجع buf->b_ml.ml_سطر_عدد;
    أرجع (linenr_T)tv_get_رقم_chk(&argvars[0], عدم);
}

/*
 * Get buffer by number or pattern.
 */
    buf_T *
tv_get_buf(typval_T *tv, صحيح curtab_only)
{
    char_u	*name = tv->vval.v_string;
    buf_T	*buf;

    إذا (tv->v_type == VAR_رقم)
	أرجع buflist_findnr((صحيح)tv->vval.v_رقم);
    إذا (tv->v_type != VAR_STRING)
	أرجع عدم;
    إذا (name == عدم || *name == NUL)
	أرجع curbuf;
    إذا (name[0] == '$' && name[1] == NUL)
	أرجع lastbuf;

    buf = buflist_find_by_name(name, curtab_only);

    // If not found, try expanding the name, like done for bufexists().
    إذا (buf == عدم)
	buf = find_buffer(tv);

    أرجع buf;
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
    إذا (buf == عدم
	    && tv->v_type != VAR_رقم
	    && tv->v_type != VAR_STRING)
	// issue errmsg for type error
	(فراغ)tv_get_رقم(tv);
    أرجع buf;
}

/*
 * Check that repeating "slen" items "n" times, does not
 * overflow the int used for the resulting length.
 */
    صحيح
check_repeat_عدد(varnumber_T slen, varnumber_T n)
{
    // Use a division so that the multiplication cannot overflow.
    إذا (n <= (varnumber_T)INT_MAX / slen)
	أرجع OK;

    حرف	buf[NUMBUFLEN];

    vim_snprintf(buf, حجم(buf), "%lld", (long long)n);
    semsg(_(e_val_too_large), buf);
    أرجع FAIL;
}

#endif // FEAT_EVAL
