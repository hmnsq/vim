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

#अन्तर्भाव "vim.h"

#जर defined(FEAT_EVAL)

/*
 * Allocate memory for a variable type-value, and make it empty (0 or NULL
 * value).
 */
    typval_T *
alloc_tv(शून्य)
{
    परत ALLOC_CLEAR_ONE(typval_T);
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
    जर (rettv != रिक्त)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = s;
    }
    अन्यथा
	vim_free(s);
    परत rettv;
}

/*
 * Free the memory for a variable type-value.
 */
    शून्य
free_tv(typval_T *varp)
{
    जर (varp == रिक्त)
	परत;

    निवड (varp->v_type)
    {
	स्थिती VAR_FUNC:
	    func_unref(varp->vval.v_string);
	    // FALLTHROUGH
	स्थिती VAR_STRING:
	    vim_free(varp->vval.v_string);
	    थांबा;
	स्थिती VAR_PARTIAL:
	    partial_unref(varp->vval.v_partial);
	    थांबा;
	स्थिती VAR_BLOB:
	    blob_unref(varp->vval.v_blob);
	    थांबा;
	स्थिती VAR_LIST:
	    list_unref(varp->vval.v_list);
	    थांबा;
	स्थिती VAR_TUPLE:
	    tuple_unref(varp->vval.v_tuple);
	    थांबा;
	स्थिती VAR_DICT:
	    dict_unref(varp->vval.v_dict);
	    थांबा;
	स्थिती VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    job_unref(varp->vval.v_job);
	    थांबा;
#endif
	स्थिती VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    channel_unref(varp->vval.v_channel);
	    थांबा;
#endif
	स्थिती VAR_CLASS:
	    class_unref(varp->vval.v_class);
	    थांबा;
	स्थिती VAR_OBJECT:
	    object_unref(varp->vval.v_object);
	    थांबा;

	स्थिती VAR_TYPEALIAS:
	    typealias_unref(varp->vval.v_typealias);
	    थांबा;

	स्थिती VAR_संख्या:
	स्थिती VAR_FLOAT:
	स्थिती VAR_ANY:
	स्थिती VAR_UNKNOWN:
	स्थिती VAR_VOID:
	स्थिती VAR_BOOL:
	स्थिती VAR_SPECIAL:
	स्थिती VAR_INSTR:
	    थांबा;
    }
    vim_free(varp);
}

/*
 * Free the memory for a variable value and set the value to NULL or 0.
 */
    शून्य
clear_tv(typval_T *varp)
{
    जर (varp == रिक्त)
	परत;

    निवड (varp->v_type)
    {
	स्थिती VAR_FUNC:
	    func_unref(varp->vval.v_string);
	    // FALLTHROUGH
	स्थिती VAR_STRING:
	    VIM_CLEAR(varp->vval.v_string);
	    थांबा;
	स्थिती VAR_PARTIAL:
	    partial_unref(varp->vval.v_partial);
	    varp->vval.v_partial = रिक्त;
	    थांबा;
	स्थिती VAR_BLOB:
	    blob_unref(varp->vval.v_blob);
	    varp->vval.v_blob = रिक्त;
	    थांबा;
	स्थिती VAR_LIST:
	    list_unref(varp->vval.v_list);
	    varp->vval.v_list = रिक्त;
	    थांबा;
	स्थिती VAR_TUPLE:
	    tuple_unref(varp->vval.v_tuple);
	    varp->vval.v_tuple = रिक्त;
	    थांबा;
	स्थिती VAR_DICT:
	    dict_unref(varp->vval.v_dict);
	    varp->vval.v_dict = रिक्त;
	    थांबा;
	स्थिती VAR_संख्या:
	स्थिती VAR_BOOL:
	स्थिती VAR_SPECIAL:
	    varp->vval.v_संख्या = 0;
	    थांबा;
	स्थिती VAR_FLOAT:
	    varp->vval.v_float = 0.0;
	    थांबा;
	स्थिती VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    job_unref(varp->vval.v_job);
	    varp->vval.v_job = रिक्त;
#endif
	    थांबा;
	स्थिती VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    channel_unref(varp->vval.v_channel);
	    varp->vval.v_channel = रिक्त;
#endif
	    थांबा;
	स्थिती VAR_INSTR:
	    VIM_CLEAR(varp->vval.v_instr);
	    थांबा;
	स्थिती VAR_CLASS:
	    class_unref(varp->vval.v_class);
	    varp->vval.v_class = रिक्त;
	    थांबा;
	स्थिती VAR_OBJECT:
	    object_unref(varp->vval.v_object);
	    varp->vval.v_object = रिक्त;
	    थांबा;
	स्थिती VAR_TYPEALIAS:
	    typealias_unref(varp->vval.v_typealias);
	    varp->vval.v_typealias = रिक्त;
	    थांबा;
	स्थिती VAR_UNKNOWN:
	स्थिती VAR_ANY:
	स्थिती VAR_VOID:
	    थांबा;
    }
    varp->v_lock = 0;
}

/*
 * Set the value of a variable to NULL without freeing items.
 */
    शून्य
init_tv(typval_T *varp)
{
    जर (varp != रिक्त)
	CLEAR_POINTER(varp);
}

    स्थिर varnumber_T
tv_get_bool_or_संख्या_chk(
	typval_T    *varp,
	पूर्णांक	    *denote,
	पूर्णांक	    want_bool,
	पूर्णांक	    vim9_string_error) // in Vim9 using a string is an error
{
    varnumber_T	n = 0L;

    निवड (varp->v_type)
    {
	स्थिती VAR_संख्या:
	    जर (in_vim9script() && want_bool && varp->vval.v_संख्या != 0
						   && varp->vval.v_संख्या != 1)
	    {
		semsg(_(e_using_संख्या_as_bool_nr), varp->vval.v_संख्या);
		थांबा;
	    }
	    परत varp->vval.v_संख्या;
	स्थिती VAR_FLOAT:
	    emsg(_(e_using_float_as_संख्या));
	    थांबा;
	स्थिती VAR_FUNC:
	स्थिती VAR_PARTIAL:
	    emsg(_(e_using_funcref_as_संख्या));
	    थांबा;
	स्थिती VAR_STRING:
	    जर (vim9_string_error && in_vim9script())
	    {
		emsg_using_string_as(varp, !want_bool);
		थांबा;
	    }
	    जर (varp->vval.v_string != रिक्त)
		vim_str2nr(varp->vval.v_string, रिक्त, रिक्त,
					 STR2NR_ALL, &n, रिक्त, 0, FALSE, रिक्त);
	    परत n;
	स्थिती VAR_LIST:
	    emsg(_(e_using_list_as_संख्या));
	    थांबा;
	स्थिती VAR_TUPLE:
	    emsg(_(e_using_tuple_as_संख्या));
	    थांबा;
	स्थिती VAR_DICT:
	    emsg(_(e_using_dictionary_as_संख्या));
	    थांबा;
	स्थिती VAR_BOOL:
	स्थिती VAR_SPECIAL:
	    जर (!want_bool && in_vim9script())
	    {
		जर (varp->v_type == VAR_BOOL)
		    emsg(_(e_using_bool_as_संख्या));
		अन्यथा
		    emsg(_(e_using_special_as_संख्या));
		थांबा;
	    }
	    परत varp->vval.v_संख्या == VVAL_TRUE ? 1 : 0;
	स्थिती VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    emsg(_(e_using_job_as_संख्या));
	    थांबा;
#endif
	स्थिती VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    emsg(_(e_using_channel_as_संख्या));
	    थांबा;
#endif
	स्थिती VAR_BLOB:
	    emsg(_(e_using_blob_as_संख्या));
	    थांबा;
	स्थिती VAR_CLASS:
	स्थिती VAR_TYPEALIAS:
	    check_typval_is_मूल्य(varp);
	    थांबा;
	स्थिती VAR_OBJECT:
	    {
		जर (varp->vval.v_object == रिक्त)
		    emsg(_(e_using_object_as_string));
		अन्यथा
		{
		    class_T *cl = varp->vval.v_object->obj_class;
		    जर (cl != रिक्त && IS_ENUM(cl))
			semsg(_(e_using_enum_str_as_संख्या), cl->class_name.string);
		    अन्यथा
			emsg(_(e_using_object_as_संख्या));
		}
	    }
	    थांबा;
	स्थिती VAR_VOID:
	    emsg(_(e_cannot_use_void_मूल्य));
	    थांबा;
	स्थिती VAR_UNKNOWN:
	स्थिती VAR_ANY:
	स्थिती VAR_INSTR:
	    internal_error_no_abort("tv_get_number(UNKNOWN)");
	    थांबा;
    }
    जर (denote == रिक्त)		// useful for values that must be unsigned
	n = -1;
    अन्यथा
	*denote = TRUE;
    परत n;
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
tv_get_संख्या(typval_T *varp)
{
    पूर्णांक		error = FALSE;

    परत tv_get_संख्या_chk(varp, &error);	// return 0L on error
}

/*
 * Like tv_get_number() but in Vim9 script do convert a number in a string to a
 * number without giving an error.
 */
    varnumber_T
tv_to_संख्या(typval_T *varp)
{
    पूर्णांक		error = FALSE;

    परत tv_get_bool_or_संख्या_chk(varp, &error, FALSE, FALSE);
}

    varnumber_T
tv_get_संख्या_chk(typval_T *varp, पूर्णांक *denote)
{
    परत tv_get_bool_or_संख्या_chk(varp, denote, FALSE, TRUE);
}

/*
 * Get the boolean value of "varp".  This is like tv_get_number_chk(),
 * but in Vim9 script accepts Number (0 and 1) and Bool/Special.
 */
    varnumber_T
tv_get_bool(typval_T *varp)
{
    परत tv_get_bool_or_संख्या_chk(varp, रिक्त, TRUE, TRUE);
}

/*
 * Get the boolean value of "varp".  This is like tv_get_number_chk(),
 * but in Vim9 script accepts Number and Bool.
 */
    varnumber_T
tv_get_bool_chk(typval_T *varp, पूर्णांक *denote)
{
    परत tv_get_bool_or_संख्या_chk(varp, denote, TRUE, TRUE);
}

    स्थिर float_T
tv_get_float_chk(typval_T *varp, पूर्णांक *error)
{
    निवड (varp->v_type)
    {
	स्थिती VAR_संख्या:
	    परत (float_T)(varp->vval.v_संख्या);
	स्थिती VAR_FLOAT:
	    परत varp->vval.v_float;
	स्थिती VAR_FUNC:
	स्थिती VAR_PARTIAL:
	    emsg(_(e_using_funcref_as_float));
	    थांबा;
	स्थिती VAR_STRING:
	    emsg(_(e_using_string_as_float));
	    थांबा;
	स्थिती VAR_LIST:
	    emsg(_(e_using_list_as_float));
	    थांबा;
	स्थिती VAR_TUPLE:
	    emsg(_(e_using_tuple_as_float));
	    थांबा;
	स्थिती VAR_DICT:
	    emsg(_(e_using_dictionary_as_float));
	    थांबा;
	स्थिती VAR_BOOL:
	    emsg(_(e_using_boolean_मूल्य_as_float));
	    थांबा;
	स्थिती VAR_SPECIAL:
	    emsg(_(e_using_special_मूल्य_as_float));
	    थांबा;
	स्थिती VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    emsg(_(e_using_job_as_float));
	    थांबा;
#endif
	स्थिती VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    emsg(_(e_using_channel_as_float));
	    थांबा;
#endif
	स्थिती VAR_BLOB:
	    emsg(_(e_using_blob_as_float));
	    थांबा;
	स्थिती VAR_CLASS:
	स्थिती VAR_TYPEALIAS:
	    check_typval_is_मूल्य(varp);
	    थांबा;
	स्थिती VAR_OBJECT:
	    emsg(_(e_using_object_as_float));
	    थांबा;
	स्थिती VAR_VOID:
	    emsg(_(e_cannot_use_void_मूल्य));
	    थांबा;
	स्थिती VAR_UNKNOWN:
	स्थिती VAR_ANY:
	स्थिती VAR_INSTR:
	    internal_error_no_abort("tv_get_float(UNKNOWN)");
	    थांबा;
    }
    जर (error != रिक्त)
	*error = TRUE;
    परत 0;
}

    float_T
tv_get_float(typval_T *varp)
{
    परत tv_get_float_chk(varp, रिक्त);
}

/*
 * Give an error and return FAIL unless "args[idx]" is unknown
 */
    पूर्णांक
check_for_unknown_arg(typval_T *args, पूर्णांक idx)
{
    जर (args[idx].v_type != VAR_UNKNOWN)
    {
	semsg(_(e_too_many_arguments), idx + 1);
	परत FAIL;
    }
    परत OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string.
 */
    पूर्णांक
check_for_string_arg(typval_T *args, पूर्णांक idx)
{
    जर (args[idx].v_type != VAR_STRING)
    {
	semsg(_(e_string_required_for_argument_nr), idx + 1);
	परत FAIL;
    }
    परत OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a non-empty string.
 */
    पूर्णांक
check_for_nonempty_string_arg(typval_T *args, पूर्णांक idx)
{
    जर (check_for_string_arg(args, idx) == FAIL)
	परत FAIL;
    जर (args[idx].vval.v_string == रिक्त || *args[idx].vval.v_string == NUL)
    {
	semsg(_(e_non_empty_string_required_for_argument_nr), idx + 1);
	परत FAIL;
    }
    परत OK;
}

/*
 * Check for an optional string argument at 'idx'
 */
    पूर्णांक
check_for_opt_string_arg(typval_T *args, पूर्णांक idx)
{
    परत (args[idx].v_type == VAR_UNKNOWN
	    || check_for_string_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a number.
 */
    पूर्णांक
check_for_संख्या_arg(typval_T *args, पूर्णांक idx)
{
    जर (args[idx].v_type != VAR_संख्या)
    {
	semsg(_(e_संख्या_required_for_argument_nr), idx + 1);
	परत FAIL;
    }
    परत OK;
}

/*
 * Check for an optional number argument at 'idx'
 */
    पूर्णांक
check_for_opt_संख्या_arg(typval_T *args, पूर्णांक idx)
{
    परत (args[idx].v_type == VAR_UNKNOWN
	    || check_for_संख्या_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a float or a number.
 */
    पूर्णांक
check_for_float_or_nr_arg(typval_T *args, पूर्णांक idx)
{
    जर (args[idx].v_type != VAR_FLOAT && args[idx].v_type != VAR_संख्या)
    {
	semsg(_(e_float_or_संख्या_required_for_argument_nr), idx + 1);
	परत FAIL;
    }
    परत OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a bool.
 */
    पूर्णांक
check_for_bool_arg(typval_T *args, पूर्णांक idx)
{
    जर (args[idx].v_type != VAR_BOOL
	    && !(args[idx].v_type == VAR_संख्या
		&& (args[idx].vval.v_संख्या == 0
		    || args[idx].vval.v_संख्या == 1)))
    {
	semsg(_(e_bool_required_for_argument_nr), idx + 1);
	परत FAIL;
    }
    परत OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a bool or a number.
 */
    स्थिर पूर्णांक
check_for_bool_or_संख्या_arg(typval_T *args, पूर्णांक idx)
{
    जर (args[idx].v_type != VAR_BOOL && args[idx].v_type != VAR_संख्या)
    {
	semsg(_(e_bool_or_संख्या_required_for_argument_nr), idx + 1);
	परत FAIL;
    }
    परत OK;
}

/*
 * Check for an optional bool argument at 'idx'.
 * Return FAIL if the type is wrong.
 */
    पूर्णांक
check_for_opt_bool_arg(typval_T *args, पूर्णांक idx)
{
    जर (args[idx].v_type == VAR_UNKNOWN)
	परत OK;
    परत check_for_bool_arg(args, idx);
}

/*
 * Check for an optional bool or number argument at 'idx'.
 * Return FAIL if the type is wrong.
 */
    पूर्णांक
check_for_opt_bool_or_संख्या_arg(typval_T *args, पूर्णांक idx)
{
    जर (args[idx].v_type == VAR_UNKNOWN)
	परत OK;
    परत check_for_bool_or_संख्या_arg(args, idx);
}

/*
 * Check for an optional bool or dict argument at 'idx'.
 * Return FAIL if the type is wrong.
 */
    पूर्णांक
check_for_opt_bool_or_dict_arg(typval_T *args, पूर्णांक idx)
{
    जर (args[idx].v_type == VAR_UNKNOWN || args[idx].v_type == VAR_DICT)
	परत OK;
    परत check_for_bool_arg(args, idx);
}

/*
 * Give an error and return FAIL unless "args[idx]" is a blob.
 */
    पूर्णांक
check_for_blob_arg(typval_T *args, पूर्णांक idx)
{
    जर (args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_blob_required_for_argument_nr), idx + 1);
	परत FAIL;
    }
    परत OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list.
 */
    पूर्णांक
check_for_list_arg(typval_T *args, पूर्णांक idx)
{
    जर (args[idx].v_type != VAR_LIST)
    {
	semsg(_(e_list_required_for_argument_nr), idx + 1);
	परत FAIL;
    }
    परत OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a non-NULL list.
 */
    पूर्णांक
check_for_nonnull_list_arg(typval_T *args, पूर्णांक idx)
{
    जर (check_for_list_arg(args, idx) == FAIL)
	परत FAIL;

    जर (args[idx].vval.v_list == रिक्त)
    {
	semsg(_(e_non_null_list_required_for_argument_nr), idx + 1);
	परत FAIL;
    }
    परत OK;
}

/*
 * Check for an optional list argument at 'idx'
 */
    पूर्णांक
check_for_opt_list_arg(typval_T *args, पूर्णांक idx)
{
    परत (args[idx].v_type == VAR_UNKNOWN
	    || check_for_list_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a tuple.
 */
    पूर्णांक
check_for_tuple_arg(typval_T *args, पूर्णांक idx)
{
    जर (args[idx].v_type != VAR_TUPLE)
    {
	semsg(_(e_tuple_required_for_argument_nr), idx + 1);
	परत FAIL;
    }
    परत OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a dict.
 */
    पूर्णांक
check_for_dict_arg(typval_T *args, पूर्णांक idx)
{
    जर (args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_dict_required_for_argument_nr), idx + 1);
	परत FAIL;
    }
    परत OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a non-NULL dict.
 */
    पूर्णांक
check_for_nonnull_dict_arg(typval_T *args, पूर्णांक idx)
{
    जर (check_for_dict_arg(args, idx) == FAIL)
	परत FAIL;

    जर (args[idx].vval.v_dict == रिक्त)
    {
	semsg(_(e_non_null_dict_required_for_argument_nr), idx + 1);
	परत FAIL;
    }
    परत OK;
}

/*
 * Check for an optional dict argument at 'idx'
 */
    पूर्णांक
check_for_opt_dict_arg(typval_T *args, पूर्णांक idx)
{
    परत (args[idx].v_type == VAR_UNKNOWN
	    || check_for_dict_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Check for an optional non-NULL dict argument at 'idx'
 */
    पूर्णांक
check_for_opt_nonnull_dict_arg(typval_T *args, पूर्णांक idx)
{
    परत (args[idx].v_type == VAR_UNKNOWN
	    || check_for_nonnull_dict_arg(args, idx) != FAIL) ? OK : FAIL;
}

#जर defined(FEAT_JOB_CHANNEL)
/*
 * Give an error and return FAIL unless "args[idx]" is a channel or a job.
 */
    पूर्णांक
check_for_chan_or_job_arg(typval_T *args, पूर्णांक idx)
{
    जर (args[idx].v_type != VAR_CHANNEL && args[idx].v_type != VAR_JOB)
    {
	semsg(_(e_chan_or_job_required_for_argument_nr), idx + 1);
	परत FAIL;
    }
    परत OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an optional channel or a
 * job.
 */
    पूर्णांक
check_for_opt_chan_or_job_arg(typval_T *args, पूर्णांक idx)
{
    परत (args[idx].v_type == VAR_UNKNOWN
	    || check_for_chan_or_job_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a job.
 */
    पूर्णांक
check_for_job_arg(typval_T *args, पूर्णांक idx)
{
    जर (args[idx].v_type != VAR_JOB)
    {
	semsg(_(e_job_required_for_argument_nr), idx + 1);
	परत FAIL;
    }
    परत OK;
}

/*
 * Check for an optional job argument at 'idx'.
 */
    पूर्णांक
check_for_opt_job_arg(typval_T *args, पूर्णांक idx)
{
    परत (args[idx].v_type == VAR_UNKNOWN
	    || check_for_job_arg(args, idx) != FAIL) ? OK : FAIL;
}
#अन्यथा
/*
 * Give an error and return FAIL unless "args[idx]" is an optional channel or a
 * job.  Used without the +channel feature, thus only VAR_UNKNOWN is accepted.
 */
    पूर्णांक
check_for_opt_chan_or_job_arg(typval_T *args, पूर्णांक idx)
{
    परत args[idx].v_type == VAR_UNKNOWN ? OK : FAIL;
}
#endif

/*
 * Give an error and return FAIL unless "args[idx]" is a string or
 * a number.
 */
    पूर्णांक
check_for_string_or_संख्या_arg(typval_T *args, पूर्णांक idx)
{
    जर (args[idx].v_type != VAR_STRING && args[idx].v_type != VAR_संख्या)
    {
	semsg(_(e_string_or_संख्या_required_for_argument_nr), idx + 1);
	परत FAIL;
    }
    परत OK;
}

/*
 * Check for an optional string or number argument at 'idx'.
 */
    पूर्णांक
check_for_opt_string_or_संख्या_arg(typval_T *args, पूर्णांक idx)
{
    परत (args[idx].v_type == VAR_UNKNOWN
	    || check_for_string_or_संख्या_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a buffer number.
 * Buffer number can be a number or a string.
 */
    पूर्णांक
check_for_buffer_arg(typval_T *args, पूर्णांक idx)
{
    परत check_for_string_or_संख्या_arg(args, idx);
}

/*
 * Check for an optional buffer argument at 'idx'
 */
    पूर्णांक
check_for_opt_buffer_arg(typval_T *args, पूर्णांक idx)
{
    परत (args[idx].v_type == VAR_UNKNOWN
	    || check_for_buffer_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a line number.
 * Line number can be a number or a string.
 */
    पूर्णांक
check_for_lnum_arg(typval_T *args, पूर्णांक idx)
{
    परत check_for_string_or_संख्या_arg(args, idx);
}

/*
 * Check for an optional line number argument at 'idx'
 */
    पूर्णांक
check_for_opt_lnum_arg(typval_T *args, पूर्णांक idx)
{
    परत (args[idx].v_type == VAR_UNKNOWN
	    || check_for_lnum_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a blob.
 */
    पूर्णांक
check_for_string_or_blob_arg(typval_T *args, पूर्णांक idx)
{
    जर (args[idx].v_type != VAR_STRING && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_string_or_blob_required_for_argument_nr), idx + 1);
	परत FAIL;
    }
    परत OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a list.
 */
    पूर्णांक
check_for_string_or_list_arg(typval_T *args, पूर्णांक idx)
{
    जर (args[idx].v_type != VAR_STRING && args[idx].v_type != VAR_LIST)
    {
	semsg(_(e_string_or_list_required_for_argument_nr), idx + 1);
	परत FAIL;
    }
    परत OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string, a list, a
 * tuple or a blob.
 */
    पूर्णांक
check_for_string_or_list_or_tuple_or_blob_arg(typval_T *args, पूर्णांक idx)
{
    जर (args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_string_list_tuple_or_blob_required_for_argument_nr), idx + 1);
	परत FAIL;
    }
    परत OK;
}

/*
 * Check for an optional string or list argument at 'idx'
 */
    पूर्णांक
check_for_opt_string_or_list_arg(typval_T *args, पूर्णांक idx)
{
    परत (args[idx].v_type == VAR_UNKNOWN
	    || check_for_string_or_list_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a dict.
 */
    पूर्णांक
check_for_string_or_dict_arg(typval_T *args, पूर्णांक idx)
{
    जर (args[idx].v_type != VAR_STRING && args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_string_or_dict_required_for_argument_nr), idx + 1);
	परत FAIL;
    }
    परत OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a number
 * or a list.
 */
    पूर्णांक
check_for_string_or_संख्या_or_list_arg(typval_T *args, पूर्णांक idx)
{
    जर (args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_संख्या
	    && args[idx].v_type != VAR_LIST)
    {
	semsg(_(e_string_संख्या_or_list_required_for_argument_nr), idx + 1);
	परत FAIL;
    }
    परत OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an optional string
 * or number or a list
 */
    पूर्णांक
check_for_opt_string_or_संख्या_or_list_arg(typval_T *args, पूर्णांक idx)
{
    परत (args[idx].v_type == VAR_UNKNOWN
	    || check_for_string_or_संख्या_or_list_arg(args, idx)
							!= FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string, a number, a
 * list, a tuple or a blob.
 */
    पूर्णांक
check_for_repeat_func_arg(typval_T *args, पूर्णांक idx)
{
    जर (args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_संख्या
	    && args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_repeatable_type_required_for_argument_nr), idx + 1);
	परत FAIL;
    }
    परत OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string, a list, a
 * tuple or a dict.
 */
    पूर्णांक
check_for_string_list_tuple_or_dict_arg(typval_T *args, पूर्णांक idx)
{
    जर (args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_string_list_tuple_or_dict_required_for_argument_nr), idx + 1);
	परत FAIL;
    }
    परत OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string
 * or a function reference.
 */
    पूर्णांक
check_for_string_or_func_arg(typval_T *args, पूर्णांक idx)
{
    जर (args[idx].v_type != VAR_PARTIAL
	    && args[idx].v_type != VAR_FUNC
	    && args[idx].v_type != VAR_STRING)
    {
	semsg(_(e_string_or_function_required_for_argument_nr), idx + 1);
	परत FAIL;
    }
    परत OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list or a blob.
 */
    पूर्णांक
check_for_list_or_blob_arg(typval_T *args, पूर्णांक idx)
{
    जर (args[idx].v_type != VAR_LIST && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_list_or_blob_required_for_argument_nr), idx + 1);
	परत FAIL;
    }
    परत OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list or a tuple.
 */
    पूर्णांक
check_for_list_or_tuple_arg(typval_T *args, पूर्णांक idx)
{
    जर (args[idx].v_type != VAR_LIST && args[idx].v_type != VAR_TUPLE)
    {
	semsg(_(e_list_or_tuple_required_for_argument_nr), idx + 1);
	परत FAIL;
    }
    परत OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list, a tuple or a
 * blob.
 */
    पूर्णांक
check_for_list_or_tuple_or_blob_arg(typval_T *args, पूर्णांक idx)
{
    जर (args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_list_or_tuple_or_blob_required_for_argument_nr), idx + 1);
	परत FAIL;
    }
    परत OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list, a tuple or a
 * dict
 */
    पूर्णांक
check_for_list_or_tuple_or_dict_arg(typval_T *args, पूर्णांक idx)
{
    जर (args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_list_or_tuple_or_dict_required_for_argument_nr), idx + 1);
	परत FAIL;
    }
    परत OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list or dict or a
 * blob.
 */
    पूर्णांक
check_for_list_or_dict_or_blob_arg(typval_T *args, पूर्णांक idx)
{
    जर (args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_DICT
	    && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_list_dict_or_blob_required_for_argument_nr), idx + 1);
	परत FAIL;
    }
    परत OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list, a tuple, a dict,
 * a blob or a string.
 */
    पूर्णांक
check_for_list_tuple_dict_blob_or_string_arg(typval_T *args, पूर्णांक idx)
{
    जर (args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_DICT
	    && args[idx].v_type != VAR_BLOB
	    && args[idx].v_type != VAR_STRING)
    {
	semsg(_(e_list_tuple_dict_blob_or_string_required_for_argument_nr), idx + 1);
	परत FAIL;
    }
    परत OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an optional buffer
 * number or a dict.
 */
    पूर्णांक
check_for_opt_buffer_or_dict_arg(typval_T *args, पूर्णांक idx)
{
    जर (args[idx].v_type != VAR_UNKNOWN
	    && args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_संख्या
	    && args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_string_required_for_argument_nr), idx + 1);
	परत FAIL;
    }
    परत OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an object.
 */
    पूर्णांक
check_for_object_arg(typval_T *args, पूर्णांक idx)
{
    जर (args[idx].v_type != VAR_OBJECT)
    {
	semsg(_(e_object_required_for_argument_nr), idx + 1);
	परत FAIL;
    }
    परत OK;
}

/*
 * Returns TRUE if "tv" is a type alias for a class
 */
    स्थिर पूर्णांक
tv_class_alias(typval_T *tv)
{
    परत tv->v_type == VAR_TYPEALIAS &&
			tv->vval.v_typealias->ta_type->tt_type == VAR_OBJECT;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a class
 * or class typealias.
 */
    पूर्णांक
check_for_class_or_typealias_args(typval_T *args, पूर्णांक idx)
{
    साठी (पूर्णांक i = idx; args[i].v_type != VAR_UNKNOWN; ++i)
    {
	जर (args[i].v_type != VAR_CLASS && !tv_class_alias(&args[idx]))
	{
	    semsg(_(e_class_or_typealias_required_for_argument_nr), i + 1);
	    परत FAIL;
	}
    }
    परत OK;
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
    स्थिर char_u   mybuf[NUMBUFLEN];

    परत tv_get_string_buf(varp, mybuf);
}

/*
 * Like tv_get_string() but don't allow number to string conversion for Vim9.
 */
    char_u *
tv_get_string_strict(typval_T *varp)
{
    स्थिर char_u   mybuf[NUMBUFLEN];
    char_u	    *res =  tv_get_string_buf_chk_strict(
						 varp, mybuf, in_vim9script());

    परत res != रिक्त ? res : (char_u *)"";
}

    char_u *
tv_get_string_buf(typval_T *varp, char_u *buf)
{
    char_u	*res = tv_get_string_buf_chk(varp, buf);

    परत res != रिक्त ? res : (char_u *)"";
}

/*
 * Careful: This uses a single, static buffer.  YOU CAN ONLY USE IT ONCE!
 */
    char_u *
tv_get_string_chk(typval_T *varp)
{
    स्थिर char_u   mybuf[NUMBUFLEN];

    परत tv_get_string_buf_chk(varp, mybuf);
}

    char_u *
tv_get_string_buf_chk(typval_T *varp, char_u *buf)
{
    परत tv_get_string_buf_chk_strict(varp, buf, FALSE);
}

    char_u *
tv_get_string_buf_chk_strict(typval_T *varp, char_u *buf, पूर्णांक strict)
{
    निवड (varp->v_type)
    {
	स्थिती VAR_संख्या:
	    जर (strict)
	    {
		emsg(_(e_using_संख्या_as_string));
		थांबा;
	    }
	    vim_snprintf((अक्षर *)buf, NUMBUFLEN, "%lld",
					    (varnumber_T)varp->vval.v_संख्या);
	    परत buf;
	स्थिती VAR_FUNC:
	स्थिती VAR_PARTIAL:
	    emsg(_(e_using_funcref_as_string));
	    थांबा;
	स्थिती VAR_LIST:
	    emsg(_(e_using_list_as_string));
	    थांबा;
	स्थिती VAR_TUPLE:
	    emsg(_(e_using_tuple_as_string));
	    थांबा;
	स्थिती VAR_DICT:
	    emsg(_(e_using_dictionary_as_string));
	    थांबा;
	स्थिती VAR_FLOAT:
	    जर (strict)
	    {
		emsg(_(e_using_float_as_string));
		थांबा;
	    }
	    vim_snprintf((अक्षर *)buf, NUMBUFLEN, "%g", varp->vval.v_float);
	    परत buf;
	स्थिती VAR_STRING:
	    जर (varp->vval.v_string != रिक्त)
		परत varp->vval.v_string;
	    परत (char_u *)"";
	स्थिती VAR_BOOL:
	स्थिती VAR_SPECIAL:
	    STRCPY(buf, get_var_special_name(varp->vval.v_संख्या));
	    परत buf;
	स्थिती VAR_BLOB:
	    emsg(_(e_using_blob_as_string));
	    थांबा;
	स्थिती VAR_CLASS:
	स्थिती VAR_TYPEALIAS:
	    check_typval_is_मूल्य(varp);
	    थांबा;
	स्थिती VAR_OBJECT:
	    {
		जर (varp->vval.v_object == रिक्त)
		    emsg(_(e_using_object_as_string));
		अन्यथा
		{
		    class_T *cl = varp->vval.v_object->obj_class;
		    जर (cl != रिक्त && IS_ENUM(cl))
			semsg(_(e_using_enum_str_as_string), cl->class_name.string);
		    अन्यथा
			emsg(_(e_using_object_as_string));
		}
	    }
	    थांबा;
	स्थिती VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    जर (in_vim9script())
	    {
		semsg(_(e_using_invalid_मूल्य_as_string_str), "job");
		थांबा;
	    }
	    परत job_to_string_buf(varp, buf);
#endif
	    थांबा;
	स्थिती VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    जर (in_vim9script())
	    {
		semsg(_(e_using_invalid_मूल्य_as_string_str), "channel");
		थांबा;
	    }
	    परत channel_to_string_buf(varp, buf);
#endif
	    थांबा;
	स्थिती VAR_VOID:
	    emsg(_(e_cannot_use_void_मूल्य));
	    थांबा;
	स्थिती VAR_UNKNOWN:
	स्थिती VAR_ANY:
	स्थिती VAR_INSTR:
	    semsg(_(e_using_invalid_मूल्य_as_string_str),
						  vartype_name(varp->v_type));
	    थांबा;
    }
    परत रिक्त;
}

/*
 * Turn a typeval into a string.  Similar to tv_get_string_buf() but uses
 * string() on Dict, List, etc.
 */
    char_u *
tv_stringify(typval_T *varp, char_u *buf)
{
    जर (varp->v_type == VAR_LIST
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
	परत tmp.vval.v_string;
    }
    परत tv_get_string_buf(varp, buf);
}

/*
 * Return TRUE if typeval "tv" and its value are set to be locked (immutable).
 * Also give an error message, using "name" or _("name") when use_gettext is
 * TRUE.
 */
    पूर्णांक
tv_check_lock(typval_T *tv, char_u *name, पूर्णांक use_gettext)
{
    पूर्णांक	lock = 0;

    निवड (tv->v_type)
    {
	स्थिती VAR_BLOB:
	    जर (tv->vval.v_blob != रिक्त)
		lock = tv->vval.v_blob->bv_lock;
	    थांबा;
	स्थिती VAR_LIST:
	    जर (tv->vval.v_list != रिक्त)
		lock = tv->vval.v_list->lv_lock;
	    थांबा;
	स्थिती VAR_TUPLE:
	    जर (tv->vval.v_tuple != रिक्त)
		lock = tv->vval.v_tuple->tv_lock;
	    थांबा;
	स्थिती VAR_DICT:
	    जर (tv->vval.v_dict != रिक्त)
		lock = tv->vval.v_dict->dv_lock;
	    थांबा;
	पूर्वनिर्धारित:
	    थांबा;
    }
    परत मूल्य_check_lock(tv->v_lock, name, use_gettext)
		   || (lock != 0 && मूल्य_check_lock(lock, name, use_gettext));
}

/*
 * Copy the values from typval_T "from" to typval_T "to".
 * When needed allocates string or increases reference count.
 * Does not make a copy of a list, blob or dict but copies the reference!
 * It is OK for "from" and "to" to point to the same item.  This is used to
 * make a copy later.
 */
    पूर्णांक
copy_tv(typval_T *from, typval_T *to)
{
    पूर्णांक		ret = OK;

    to->v_type = from->v_type;
    to->v_lock = 0;
    निवड (from->v_type)
    {
	स्थिती VAR_संख्या:
	स्थिती VAR_BOOL:
	स्थिती VAR_SPECIAL:
	    to->vval.v_संख्या = from->vval.v_संख्या;
	    थांबा;
	स्थिती VAR_FLOAT:
	    to->vval.v_float = from->vval.v_float;
	    थांबा;
	स्थिती VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    to->vval.v_job = from->vval.v_job;
	    जर (to->vval.v_job != रिक्त)
		++to->vval.v_job->jv_refcount;
	    थांबा;
#endif
	स्थिती VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    to->vval.v_channel = from->vval.v_channel;
	    जर (to->vval.v_channel != रिक्त)
		++to->vval.v_channel->ch_refcount;
	    थांबा;
#endif
	स्थिती VAR_INSTR:
	    to->vval.v_instr = from->vval.v_instr;
	    थांबा;

	स्थिती VAR_CLASS:
	    copy_class(from, to);
	    थांबा;

	स्थिती VAR_OBJECT:
	    copy_object(from, to);
	    थांबा;

	स्थिती VAR_STRING:
	स्थिती VAR_FUNC:
	    जर (from->vval.v_string == रिक्त)
		to->vval.v_string = रिक्त;
	    अन्यथा
	    {
		to->vval.v_string = vim_strsave(from->vval.v_string);
		जर (from->v_type == VAR_FUNC)
		    func_ref(to->vval.v_string);
	    }
	    थांबा;
	स्थिती VAR_PARTIAL:
	    जर (from->vval.v_partial == रिक्त)
		to->vval.v_partial = रिक्त;
	    अन्यथा
	    {
		to->vval.v_partial = from->vval.v_partial;
		++to->vval.v_partial->pt_refcount;
	    }
	    थांबा;
	स्थिती VAR_BLOB:
	    जर (from->vval.v_blob == रिक्त)
		to->vval.v_blob = रिक्त;
	    अन्यथा
	    {
		to->vval.v_blob = from->vval.v_blob;
		++to->vval.v_blob->bv_refcount;
	    }
	    थांबा;
	स्थिती VAR_LIST:
	    जर (from->vval.v_list == रिक्त)
		to->vval.v_list = रिक्त;
	    अन्यथा
	    {
		to->vval.v_list = from->vval.v_list;
		++to->vval.v_list->lv_refcount;
	    }
	    थांबा;
	स्थिती VAR_TUPLE:
	    जर (from->vval.v_tuple == रिक्त)
		to->vval.v_tuple = रिक्त;
	    अन्यथा
	    {
		to->vval.v_tuple = from->vval.v_tuple;
		++to->vval.v_tuple->tv_refcount;
	    }
	    थांबा;
	स्थिती VAR_DICT:
	    जर (from->vval.v_dict == रिक्त)
		to->vval.v_dict = रिक्त;
	    अन्यथा
	    {
		to->vval.v_dict = from->vval.v_dict;
		++to->vval.v_dict->dv_refcount;
	    }
	    थांबा;
	स्थिती VAR_TYPEALIAS:
	    जर (from->vval.v_typealias == रिक्त)
		to->vval.v_typealias = रिक्त;
	    अन्यथा
	    {
		to->vval.v_typealias = from->vval.v_typealias;
		++to->vval.v_typealias->ta_refcount;
	    }
	    थांबा;
	स्थिती VAR_VOID:
	    emsg(_(e_cannot_use_void_मूल्य));
	    ret = FAIL;
	    थांबा;
	स्थिती VAR_UNKNOWN:
	स्थिती VAR_ANY:
	    internal_error_no_abort("copy_tv(UNKNOWN)");
	    ret = FAIL;
	    थांबा;
    }

    परत ret;
}

/*
 * Compare "tv1" and "tv2".
 */
    पूर्णांक
typval_compare2(
    typval_T	*tv1,	// first operand
    typval_T	*tv2,	// second operand
    exprtype_T	type,	// operator
    पूर्णांक		ic,	// ignore case
    पूर्णांक		*res)	// comparison result
{
    varnumber_T	n1, n2;
    पूर्णांक		type_is = type == अभिव्यक्ती_IS || type == अभिव्यक्ती_ISNOT;

    जर (check_typval_is_मूल्य(tv1) == FAIL
	|| check_typval_is_मूल्य(tv2) == FAIL)
	परत FAIL;
    अन्यथा जर (type_is && tv1->v_type != tv2->v_type)
    {
	// For "is" a different type always means FALSE, for "isnot"
	// it means TRUE.
	*res = (type == अभिव्यक्ती_ISNOT);
    }
    अन्यथा जर (((tv1->v_type == VAR_SPECIAL && tv1->vval.v_संख्या == VVAL_NULL)
		|| (tv2->v_type == VAR_SPECIAL
					   && tv2->vval.v_संख्या == VVAL_NULL))
	    && tv1->v_type != tv2->v_type
	    && (type == अभिव्यक्ती_EQUAL || type == अभिव्यक्ती_NEQUAL))
    {
	n1 = typval_compare_null(tv1, tv2);
	जर (n1 == MAYBE)
	    परत FAIL;
	जर (type == अभिव्यक्ती_NEQUAL)
	    n1 = !n1;
	*res = n1;
    }
    अन्यथा जर (tv1->v_type == VAR_BLOB || tv2->v_type == VAR_BLOB)
    {
	जर (typval_compare_blob(tv1, tv2, type, res) == FAIL)
	    परत FAIL;
    }
    अन्यथा जर (tv1->v_type == VAR_LIST || tv2->v_type == VAR_LIST)
    {
	जर (typval_compare_list(tv1, tv2, type, ic, res) == FAIL)
	    परत FAIL;
    }
    अन्यथा जर (tv1->v_type == VAR_TUPLE || tv2->v_type == VAR_TUPLE)
    {
	जर (typval_compare_tuple(tv1, tv2, type, ic, res) == FAIL)
	    परत FAIL;
    }
    अन्यथा जर (tv1->v_type == VAR_OBJECT || tv2->v_type == VAR_OBJECT)
    {
	जर (typval_compare_object(tv1, tv2, type, ic, res) == FAIL)
	    परत FAIL;
    }
    अन्यथा जर (tv1->v_type == VAR_DICT || tv2->v_type == VAR_DICT)
    {
	जर (typval_compare_dict(tv1, tv2, type, ic, res) == FAIL)
	    परत FAIL;
    }
    अन्यथा जर (tv1->v_type == VAR_FUNC || tv2->v_type == VAR_FUNC
	|| tv1->v_type == VAR_PARTIAL || tv2->v_type == VAR_PARTIAL)
    {
	जर (typval_compare_func(tv1, tv2, type, ic, res) == FAIL)
	    परत FAIL;
    }

    // If one of the two variables is a float, compare as a float.
    // When using "=~" or "!~", always compare as string.
    अन्यथा जर ((tv1->v_type == VAR_FLOAT || tv2->v_type == VAR_FLOAT)
	    && type != अभिव्यक्ती_MATCH && type != अभिव्यक्ती_NOMATCH)
    {
	float_T f1, f2;
	पूर्णांक	error = FALSE;

	f1 = tv_get_float_chk(tv1, &error);
	जर (!error)
	    f2 = tv_get_float_chk(tv2, &error);
	जर (error)
	    परत FAIL;
	n1 = FALSE;
	निवड (type)
	{
	    स्थिती अभिव्यक्ती_IS:
	    स्थिती अभिव्यक्ती_EQUAL:    n1 = (f1 == f2); थांबा;
	    स्थिती अभिव्यक्ती_ISNOT:
	    स्थिती अभिव्यक्ती_NEQUAL:   n1 = (f1 != f2); थांबा;
	    स्थिती अभिव्यक्ती_GREATER:  n1 = (f1 > f2); थांबा;
	    स्थिती अभिव्यक्ती_GEQUAL:   n1 = (f1 >= f2); थांबा;
	    स्थिती अभिव्यक्ती_SMALLER:  n1 = (f1 < f2); थांबा;
	    स्थिती अभिव्यक्ती_SEQUAL:   n1 = (f1 <= f2); थांबा;
	    स्थिती अभिव्यक्ती_UNKNOWN:
	    स्थिती अभिव्यक्ती_MATCH:
	    पूर्वनिर्धारित:  थांबा;  // avoid gcc warning
	}
	*res = n1;
    }

    // If one of the two variables is a number, compare as a number.
    // When using "=~" or "!~", always compare as string.
    अन्यथा जर ((tv1->v_type == VAR_संख्या || tv2->v_type == VAR_संख्या)
	    && type != अभिव्यक्ती_MATCH && type != अभिव्यक्ती_NOMATCH)
    {
	पूर्णांक error = FALSE;

	n1 = tv_get_संख्या_chk(tv1, &error);
	जर (!error)
	    n2 = tv_get_संख्या_chk(tv2, &error);
	जर (error)
	    परत FAIL;
	निवड (type)
	{
	    स्थिती अभिव्यक्ती_IS:
	    स्थिती अभिव्यक्ती_EQUAL:    n1 = (n1 == n2); थांबा;
	    स्थिती अभिव्यक्ती_ISNOT:
	    स्थिती अभिव्यक्ती_NEQUAL:   n1 = (n1 != n2); थांबा;
	    स्थिती अभिव्यक्ती_GREATER:  n1 = (n1 > n2); थांबा;
	    स्थिती अभिव्यक्ती_GEQUAL:   n1 = (n1 >= n2); थांबा;
	    स्थिती अभिव्यक्ती_SMALLER:  n1 = (n1 < n2); थांबा;
	    स्थिती अभिव्यक्ती_SEQUAL:   n1 = (n1 <= n2); थांबा;
	    स्थिती अभिव्यक्ती_UNKNOWN:
	    स्थिती अभिव्यक्ती_MATCH:
	    पूर्वनिर्धारित:  थांबा;  // avoid gcc warning
	}
	*res = n1;
    }
    अन्यथा जर (in_vim9script() && (tv1->v_type == VAR_BOOL
				    || tv2->v_type == VAR_BOOL
				    || (tv1->v_type == VAR_SPECIAL
					      && tv2->v_type == VAR_SPECIAL)))
    {
	जर (tv1->v_type != tv2->v_type)
	{
	    semsg(_(e_cannot_compare_str_with_str),
		       vartype_name(tv1->v_type), vartype_name(tv2->v_type));
	    परत FAIL;
	}
	n1 = tv1->vval.v_संख्या;
	n2 = tv2->vval.v_संख्या;
	निवड (type)
	{
	    स्थिती अभिव्यक्ती_IS:
	    स्थिती अभिव्यक्ती_EQUAL:    n1 = (n1 == n2); थांबा;
	    स्थिती अभिव्यक्ती_ISNOT:
	    स्थिती अभिव्यक्ती_NEQUAL:   n1 = (n1 != n2); थांबा;
	    पूर्वनिर्धारित:
		semsg(_(e_invalid_operation_for_str),
						   vartype_name(tv1->v_type));
		परत FAIL;
	}
	*res = n1;
    }
#ifdef FEAT_JOB_CHANNEL
    अन्यथा जर (tv1->v_type == tv2->v_type
	    && (tv1->v_type == VAR_CHANNEL || tv1->v_type == VAR_JOB)
	    && (type == अभिव्यक्ती_NEQUAL || type == अभिव्यक्ती_EQUAL))
    {
	जर (tv1->v_type == VAR_CHANNEL)
	    n1 = tv1->vval.v_channel == tv2->vval.v_channel;
	अन्यथा
	    n1 = tv1->vval.v_job == tv2->vval.v_job;
	जर (type == अभिव्यक्ती_NEQUAL)
	    n1 = !n1;
	*res = n1;
    }
#endif
    अन्यथा
    {
	जर (typval_compare_string(tv1, tv2, type, ic, res) == FAIL)
	    परत FAIL;
    }

    परत OK;
}

/*
 * Compare "tv1" and "tv2".
 * Put the result in "tv1".  Caller should clear "tv2".
 */
    पूर्णांक
typval_compare(
    typval_T	*tv1,	// first operand
    typval_T	*tv2,	// second operand
    exprtype_T	type,   // operator
    पूर्णांक		ic)     // ignore case
{
    पूर्णांक res;

    जर (typval_compare2(tv1, tv2, type, ic, &res) == FAIL)
    {
	clear_tv(tv1);
	परत FAIL;
    }

    clear_tv(tv1);
    जर (in_vim9script())
    {
	tv1->v_type = VAR_BOOL;
	tv1->vval.v_संख्या = res ? VVAL_TRUE : VVAL_FALSE;
    }
    अन्यथा
    {
	tv1->v_type = VAR_संख्या;
	tv1->vval.v_संख्या = res;
    }

    परत OK;
}

/*
 * Compare "tv1" to "tv2" as lists according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    पूर्णांक
typval_compare_list(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	पूर्णांक	    ic,
	पूर्णांक	    *res)
{
    पूर्णांक	    val = 0;

    जर (type == अभिव्यक्ती_IS || type == अभिव्यक्ती_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
				      && tv1->vval.v_list == tv2->vval.v_list);
	जर (type == अभिव्यक्ती_ISNOT)
	    val = !val;
    }
    अन्यथा जर (tv1->v_type != tv2->v_type
	    || (type != अभिव्यक्ती_EQUAL && type != अभिव्यक्ती_NEQUAL))
    {
	जर (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_list_with_list));
	अन्यथा
	    emsg(_(e_invalid_operation_for_list));
	परत FAIL;
    }
    अन्यथा
    {
	val = list_equal(tv1->vval.v_list, tv2->vval.v_list, ic);
	जर (type == अभिव्यक्ती_NEQUAL)
	    val = !val;
    }
    *res = val;
    परत OK;
}

/*
 * Compare "tv1" to "tv2" as tuples according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    पूर्णांक
typval_compare_tuple(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	पूर्णांक	    ic,
	पूर्णांक	    *res)
{
    पूर्णांक	    val = 0;

    जर (type == अभिव्यक्ती_IS || type == अभिव्यक्ती_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
			      && tv1->vval.v_tuple == tv2->vval.v_tuple);
	जर (type == अभिव्यक्ती_ISNOT)
	    val = !val;
    }
    अन्यथा जर (tv1->v_type != tv2->v_type
	    || (type != अभिव्यक्ती_EQUAL && type != अभिव्यक्ती_NEQUAL))
    {
	जर (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_tuple_with_tuple));
	अन्यथा
	    emsg(_(e_invalid_operation_for_tuple));
	परत FAIL;
    }
    अन्यथा
    {
	val = tuple_equal(tv1->vval.v_tuple, tv2->vval.v_tuple, ic);
	जर (type == अभिव्यक्ती_NEQUAL)
	    val = !val;
    }
    *res = val;
    परत OK;
}

/*
 * Compare v:null with another type.  Return TRUE if the value is NULL.
 */
    पूर्णांक
typval_compare_null(typval_T *tv1, typval_T *tv2)
{
    जर ((tv1->v_type == VAR_SPECIAL && tv1->vval.v_संख्या == VVAL_NULL)
	    || (tv2->v_type == VAR_SPECIAL && tv2->vval.v_संख्या == VVAL_NULL))
    {
	typval_T	*tv = tv1->v_type == VAR_SPECIAL ? tv2 : tv1;

	निवड (tv->v_type)
	{
	    स्थिती VAR_BLOB: परत tv->vval.v_blob == रिक्त;
#ifdef FEAT_JOB_CHANNEL
	    स्थिती VAR_CHANNEL: परत tv->vval.v_channel == रिक्त;
#endif
	    // TODO: null_class handling
	    // case VAR_CLASS: return tv->vval.v_class == NULL;
	    स्थिती VAR_DICT: परत tv->vval.v_dict == रिक्त;
	    स्थिती VAR_FUNC: परत tv->vval.v_string == रिक्त;
#ifdef FEAT_JOB_CHANNEL
	    स्थिती VAR_JOB: परत tv->vval.v_job == रिक्त;
#endif
	    स्थिती VAR_LIST: परत tv->vval.v_list == रिक्त;
	    स्थिती VAR_TUPLE: परत tv->vval.v_tuple == रिक्त;
	    स्थिती VAR_OBJECT: परत tv->vval.v_object == रिक्त;
	    स्थिती VAR_PARTIAL: परत tv->vval.v_partial == रिक्त;
	    स्थिती VAR_STRING: परत tv->vval.v_string == रिक्त;

	    स्थिती VAR_संख्या: जर (!in_vim9script())
				 परत tv->vval.v_संख्या == 0;
			     थांबा;
	    स्थिती VAR_FLOAT: जर (!in_vim9script())
				 परत tv->vval.v_float == 0.0;
			     थांबा;
	    स्थिती VAR_TYPEALIAS: परत tv->vval.v_typealias == रिक्त;
	    पूर्वनिर्धारित: थांबा;
	}
    }
    // although comparing null with number, float or bool is not very useful
    // we won't give an error
    परत FALSE;
}

/*
 * Compare "tv1" to "tv2" as blobs according to "type".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    पूर्णांक
typval_compare_blob(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	पूर्णांक	    *res)
{
    पूर्णांक	    val = 0;

    जर (type == अभिव्यक्ती_IS || type == अभिव्यक्ती_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
			&& tv1->vval.v_blob == tv2->vval.v_blob);
	जर (type == अभिव्यक्ती_ISNOT)
	    val = !val;
    }
    अन्यथा जर (tv1->v_type != tv2->v_type
	    || (type != अभिव्यक्ती_EQUAL && type != अभिव्यक्ती_NEQUAL))
    {
	जर (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_blob_with_blob));
	अन्यथा
	    emsg(_(e_invalid_operation_for_blob));
	परत FAIL;
    }
    अन्यथा
    {
	val = blob_equal(tv1->vval.v_blob, tv2->vval.v_blob);
	जर (type == अभिव्यक्ती_NEQUAL)
	    val = !val;
    }
    *res = val;
    परत OK;
}

/*
 * Compare "tv1" to "tv2" as objects according to "type".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    पूर्णांक
typval_compare_object(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	पूर्णांक	    ic,
	पूर्णांक	    *res)
{
    पूर्णांक res_match = type == अभिव्यक्ती_EQUAL || type == अभिव्यक्ती_IS ? TRUE : FALSE;

    जर (tv1->vval.v_object == रिक्त && tv2->vval.v_object == रिक्त)
    {
	*res = res_match;
	परत OK;
    }
    जर (tv1->vval.v_object == रिक्त || tv2->vval.v_object == रिक्त)
    {
	*res = !res_match;
	परत OK;
    }

    object_T *obj1 = tv1->vval.v_object;
    object_T *obj2 = tv2->vval.v_object;
    जर (type == अभिव्यक्ती_IS || type == अभिव्यक्ती_ISNOT)
    {
	*res = obj1 == obj2 ? res_match : !res_match;
	परत OK;
    }
    अन्यथा जर (tv1->v_type != tv2->v_type)
    {
	emsg(_(e_can_only_compare_object_with_object));
	परत FAIL;
    }

    *res = object_equal(obj1, obj2, ic) ? res_match : !res_match;
    परत OK;
}

/*
 * Compare "tv1" to "tv2" as dictionaries according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    पूर्णांक
typval_compare_dict(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	पूर्णांक	    ic,
	पूर्णांक	    *res)
{
    पूर्णांक	    val;

    जर (type == अभिव्यक्ती_IS || type == अभिव्यक्ती_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
			&& tv1->vval.v_dict == tv2->vval.v_dict);
	जर (type == अभिव्यक्ती_ISNOT)
	    val = !val;
    }
    अन्यथा जर (tv1->v_type != tv2->v_type
		|| (type != अभिव्यक्ती_EQUAL && type != अभिव्यक्ती_NEQUAL))
    {
	जर (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_dictionary_with_dictionary));
	अन्यथा
	    emsg(_(e_invalid_operation_for_dictionary));
	परत FAIL;
    }
    अन्यथा
    {
	val = dict_equal(tv1->vval.v_dict, tv2->vval.v_dict, ic);
	जर (type == अभिव्यक्ती_NEQUAL)
	    val = !val;
    }
    *res = val;
    परत OK;
}

/*
 * Compare "tv1" to "tv2" as funcrefs according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    पूर्णांक
typval_compare_func(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	पूर्णांक	    ic,
	पूर्णांक	    *res)
{
    पूर्णांक	    val = 0;

    जर (type != अभिव्यक्ती_EQUAL && type != अभिव्यक्ती_NEQUAL
	    && type != अभिव्यक्ती_IS && type != अभिव्यक्ती_ISNOT)
    {
	emsg(_(e_invalid_operation_for_funcrefs));
	परत FAIL;
    }
    जर ((tv1->v_type == VAR_PARTIAL && tv1->vval.v_partial == रिक्त)
	    || (tv2->v_type == VAR_PARTIAL && tv2->vval.v_partial == रिक्त))
	// When both partials are NULL, then they are equal.
	// Otherwise they are not equal.
	val = (tv1->vval.v_partial == tv2->vval.v_partial);
    अन्यथा जर (type == अभिव्यक्ती_IS || type == अभिव्यक्ती_ISNOT)
    {
	जर (tv1->v_type == VAR_FUNC && tv2->v_type == VAR_FUNC)
	    // strings are considered the same if their value is
	    // the same
	    val = tv_equal(tv1, tv2, ic);
	अन्यथा जर (tv1->v_type == VAR_PARTIAL && tv2->v_type == VAR_PARTIAL)
	    val = (tv1->vval.v_partial == tv2->vval.v_partial);
	अन्यथा
	    val = FALSE;
    }
    अन्यथा
	val = tv_equal(tv1, tv2, ic);
    जर (type == अभिव्यक्ती_NEQUAL || type == अभिव्यक्ती_ISNOT)
	val = !val;
    *res = val;
    परत OK;
}

/*
 * Compare "tv1" to "tv2" as strings according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    पूर्णांक
typval_compare_string(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	पूर्णांक	    ic,
	पूर्णांक	    *res)
{
    पूर्णांक		i = 0;
    पूर्णांक		val = FALSE;
    char_u	*s1, *s2;
    char_u	buf1[NUMBUFLEN], buf2[NUMBUFLEN];

    जर (in_vim9script()
	  && ((tv1->v_type != VAR_STRING && tv1->v_type != VAR_SPECIAL)
	   || (tv2->v_type != VAR_STRING && tv2->v_type != VAR_SPECIAL)))
    {
	semsg(_(e_cannot_compare_str_with_str),
		   vartype_name(tv1->v_type), vartype_name(tv2->v_type));
	परत FAIL;
    }
    s1 = tv_get_string_buf(tv1, buf1);
    s2 = tv_get_string_buf(tv2, buf2);
    जर (type != अभिव्यक्ती_MATCH && type != अभिव्यक्ती_NOMATCH)
	i = ic ? MB_STRICMP(s1, s2) : STRCMP(s1, s2);
    निवड (type)
    {
	स्थिती अभिव्यक्ती_IS:	    जर (in_vim9script())
			    {
				// Really check it is the same string, not just
				// the same value.
				val = tv1->vval.v_string == tv2->vval.v_string;
				थांबा;
			    }
			    // FALLTHROUGH
	स्थिती अभिव्यक्ती_EQUAL:    val = (i == 0); थांबा;
	स्थिती अभिव्यक्ती_ISNOT:    जर (in_vim9script())
			    {
				// Really check it is not the same string, not
				// just a different value.
				val = tv1->vval.v_string != tv2->vval.v_string;
				थांबा;
			    }
			    // FALLTHROUGH
	स्थिती अभिव्यक्ती_NEQUAL:   val = (i != 0); थांबा;
	स्थिती अभिव्यक्ती_GREATER:  val = (i > 0); थांबा;
	स्थिती अभिव्यक्ती_GEQUAL:   val = (i >= 0); थांबा;
	स्थिती अभिव्यक्ती_SMALLER:  val = (i < 0); थांबा;
	स्थिती अभिव्यक्ती_SEQUAL:   val = (i <= 0); थांबा;

	स्थिती अभिव्यक्ती_MATCH:
	स्थिती अभिव्यक्ती_NOMATCH:
		val = pattern_match(s2, s1, ic);
		जर (type == अभिव्यक्ती_NOMATCH)
		    val = !val;
		थांबा;

	पूर्वनिर्धारित:  थांबा;  // avoid gcc warning
    }
    *res = val;
    परत OK;
}
/*
 * Convert any type to a string, never give an error.
 * When "quotes" is TRUE add quotes to a string.
 * Returns an allocated string.
 */
    char_u *
typval_tostring(typval_T *arg, पूर्णांक quotes)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    char_u	*ret = रिक्त;

    जर (arg == रिक्त)
	परत vim_strsave((char_u *)"(does not exist)");
    जर (!quotes && arg->v_type == VAR_STRING)
    {
	ret = vim_strsave(arg->vval.v_string == रिक्त ? (char_u *)""
							 : arg->vval.v_string);
    }
    अन्यथा
    {
	ret = tv2string(arg, &tofree, numbuf, 0);
	// Make a copy if we have a value but it's not in allocated memory.
	जर (ret != रिक्त && tofree == रिक्त)
	    ret = vim_strsave(ret);
    }
    परत ret;
}

/*
 * Return TRUE if typeval "tv" is locked: Either that value is locked itself
 * or it refers to a List or Dictionary that is locked.
 */
    पूर्णांक
tv_islocked(typval_T *tv)
{
    परत (tv->v_lock & VAR_LOCKED)
	|| (tv->v_type == VAR_LIST
		&& tv->vval.v_list != रिक्त
		&& (tv->vval.v_list->lv_lock & VAR_LOCKED))
	|| (tv->v_type == VAR_TUPLE
		&& tv->vval.v_tuple != रिक्त
		&& (tv->vval.v_tuple->tv_lock & VAR_LOCKED))
	|| (tv->v_type == VAR_DICT
		&& tv->vval.v_dict != रिक्त
		&& (tv->vval.v_dict->dv_lock & VAR_LOCKED));
}

    स्थिर पूर्णांक
func_equal(
    typval_T *tv1,
    typval_T *tv2,
    पूर्णांक	     ic)	    // ignore case
{
    char_u	*s1, *s2;
    dict_T	*d1, *d2;
    पूर्णांक		a1, a2;
    पूर्णांक		i;

    // empty and NULL function name considered the same
    s1 = tv1->v_type == VAR_FUNC ? tv1->vval.v_string
					   : partial_name(tv1->vval.v_partial);
    जर (s1 != रिक्त && *s1 == NUL)
	s1 = रिक्त;
    s2 = tv2->v_type == VAR_FUNC ? tv2->vval.v_string
					   : partial_name(tv2->vval.v_partial);
    जर (s2 != रिक्त && *s2 == NUL)
	s2 = रिक्त;
    जर (s1 == रिक्त || s2 == रिक्त)
    {
	जर (s1 != s2)
	    परत FALSE;
    }
    अन्यथा जर (STRCMP(s1, s2) != 0)
	परत FALSE;

    // empty dict and NULL dict is different
    d1 = tv1->v_type == VAR_FUNC ? रिक्त : tv1->vval.v_partial->pt_dict;
    d2 = tv2->v_type == VAR_FUNC ? रिक्त : tv2->vval.v_partial->pt_dict;
    जर (d1 == रिक्त || d2 == रिक्त)
    {
	जर (d1 != d2)
	    परत FALSE;
    }
    अन्यथा जर (!dict_equal(d1, d2, ic))
	परत FALSE;

    // empty list and no list considered the same
    a1 = tv1->v_type == VAR_FUNC ? 0 : tv1->vval.v_partial->pt_argc;
    a2 = tv2->v_type == VAR_FUNC ? 0 : tv2->vval.v_partial->pt_argc;
    जर (a1 != a2)
	परत FALSE;
    साठी (i = 0; i < a1; ++i)
	जर (!tv_equal(tv1->vval.v_partial->pt_argv + i,
		      tv2->vval.v_partial->pt_argv + i, ic))
	    परत FALSE;

    परत TRUE;
}

/*
 * Return TRUE if "tv1" and "tv2" have the same value.
 * Compares the items just like "==" would compare them, but strings and
 * numbers are different.  Floats and numbers are also different.
 */
    पूर्णांक
tv_equal(
    typval_T *tv1,
    typval_T *tv2,
    पूर्णांक	     ic)	    // ignore case
{
    char_u	buf1[NUMBUFLEN], buf2[NUMBUFLEN];
    char_u	*s1, *s2;
    स्थिर पूर्णांक  recursive_cnt = 0;	    // catch recursive loops
    पूर्णांक		r;
    स्थिर पूर्णांक	tv_equal_recurse_limit;

    // Catch lists and dicts that have an endless loop by limiting
    // recursiveness to a limit.  We guess they are equal then.
    // A fixed limit has the problem of still taking an awful long time.
    // Reduce the limit every time running into it. That should work fine for
    // deeply linked structures that are not recursively linked and catch
    // recursiveness quickly.
    जर (recursive_cnt == 0)
	tv_equal_recurse_limit = 1000;
    जर (recursive_cnt >= tv_equal_recurse_limit)
    {
	--tv_equal_recurse_limit;
	परत TRUE;
    }

    // For VAR_FUNC and VAR_PARTIAL compare the function name, bound dict and
    // arguments.
    जर ((tv1->v_type == VAR_FUNC
		|| (tv1->v_type == VAR_PARTIAL && tv1->vval.v_partial != रिक्त))
	    && (tv2->v_type == VAR_FUNC
		|| (tv2->v_type == VAR_PARTIAL && tv2->vval.v_partial != रिक्त)))
    {
	++recursive_cnt;
	r = func_equal(tv1, tv2, ic);
	--recursive_cnt;
	परत r;
    }

    जर (tv1->v_type != tv2->v_type
	    && ((tv1->v_type != VAR_BOOL && tv1->v_type != VAR_SPECIAL)
		|| (tv2->v_type != VAR_BOOL && tv2->v_type != VAR_SPECIAL)))
	परत FALSE;

    निवड (tv1->v_type)
    {
	स्थिती VAR_LIST:
	    ++recursive_cnt;
	    r = list_equal(tv1->vval.v_list, tv2->vval.v_list, ic);
	    --recursive_cnt;
	    परत r;

	स्थिती VAR_TUPLE:
	    ++recursive_cnt;
	    r = tuple_equal(tv1->vval.v_tuple, tv2->vval.v_tuple, ic);
	    --recursive_cnt;
	    परत r;

	स्थिती VAR_DICT:
	    ++recursive_cnt;
	    r = dict_equal(tv1->vval.v_dict, tv2->vval.v_dict, ic);
	    --recursive_cnt;
	    परत r;

	स्थिती VAR_BLOB:
	    परत blob_equal(tv1->vval.v_blob, tv2->vval.v_blob);

	स्थिती VAR_संख्या:
	स्थिती VAR_BOOL:
	स्थिती VAR_SPECIAL:
	    परत tv1->vval.v_संख्या == tv2->vval.v_संख्या;

	स्थिती VAR_STRING:
	    s1 = tv_get_string_buf(tv1, buf1);
	    s2 = tv_get_string_buf(tv2, buf2);
	    परत ((ic ? MB_STRICMP(s1, s2) : STRCMP(s1, s2)) == 0);

	स्थिती VAR_FLOAT:
	    परत tv1->vval.v_float == tv2->vval.v_float;
	स्थिती VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    परत tv1->vval.v_job == tv2->vval.v_job;
#endif
	स्थिती VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    परत tv1->vval.v_channel == tv2->vval.v_channel;
#endif
	स्थिती VAR_INSTR:
	    परत tv1->vval.v_instr == tv2->vval.v_instr;

	स्थिती VAR_CLASS:
	    // A class only exists once, equality is identity.
	    परत tv1->vval.v_class == tv2->vval.v_class;

	स्थिती VAR_OBJECT:
	    ++recursive_cnt;
	    r = object_equal(tv1->vval.v_object, tv2->vval.v_object, ic);
	    --recursive_cnt;
	    परत r;

	स्थिती VAR_PARTIAL:
	    परत tv1->vval.v_partial == tv2->vval.v_partial;

	स्थिती VAR_FUNC:
	    परत tv1->vval.v_string == tv2->vval.v_string;

	स्थिती VAR_TYPEALIAS:
	    परत tv1->vval.v_typealias == tv2->vval.v_typealias;

	स्थिती VAR_UNKNOWN:
	स्थिती VAR_ANY:
	स्थिती VAR_VOID:
	    थांबा;
    }

    // VAR_UNKNOWN can be the result of a invalid expression, let's say it
    // does not equal anything, not even itself.
    परत FALSE;
}

/*
 * Get an option value.
 * "arg" points to the '&' or '+' before the option name.
 * "arg" is advanced to character after the option name.
 * Return OK or FAIL.
 */
    पूर्णांक
eval_option(
    char_u	**arg,
    typval_T	*rettv,	// when NULL, only check if option exists
    पूर्णांक		evaluate)
{
    char_u	*option_end;
    दीर्घ	numval;
    char_u	*stringval;
    getoption_T	opt_type;
    पूर्णांक		c;
    पूर्णांक		working = (**arg == '+');    // has("+option")
    पूर्णांक		ret = OK;
    पूर्णांक		scope;

    // Isolate the option name and find its value.
    option_end = find_option_end(arg, &scope);
    जर (option_end == रिक्त)
    {
	जर (rettv != रिक्त)
	    semsg(_(e_option_name_missing_str), *arg);
	परत FAIL;
    }

    जर (!evaluate)
    {
	*arg = option_end;
	परत OK;
    }

    c = *option_end;
    *option_end = NUL;
    opt_type = get_option_मूल्य(*arg, &numval,
			       rettv == रिक्त ? रिक्त : &stringval, रिक्त, scope);

    जर (opt_type == gov_unknown)
    {
	जर (rettv != रिक्त)
	    semsg(_(e_unknown_option_str), *arg);
	ret = FAIL;
    }
    अन्यथा जर (rettv != रिक्त)
    {
	rettv->v_lock = 0;
	जर (opt_type == gov_hidden_string)
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = रिक्त;
	}
	अन्यथा जर (opt_type == gov_hidden_bool || opt_type == gov_hidden_संख्या)
	{
	    rettv->v_type = in_vim9script() && opt_type == gov_hidden_bool
						       ? VAR_BOOL : VAR_संख्या;
	    rettv->vval.v_संख्या = 0;
	}
	अन्यथा जर (opt_type == gov_bool || opt_type == gov_संख्या)
	{
	    जर (in_vim9script() && opt_type == gov_bool)
	    {
		rettv->v_type = VAR_BOOL;
		rettv->vval.v_संख्या = numval ? VVAL_TRUE : VVAL_FALSE;
	    }
	    अन्यथा
	    {
		rettv->v_type = VAR_संख्या;
		rettv->vval.v_संख्या = numval;
	    }
	}
	अन्यथा				// string option
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = stringval;
	}
    }
    अन्यथा जर (working && (opt_type == gov_hidden_bool
			|| opt_type == gov_hidden_संख्या
			|| opt_type == gov_hidden_string))
	ret = FAIL;

    *option_end = c;		    // put back for error messages
    *arg = option_end;

    परत ret;
}

/*
 * Allocate a variable for a number constant.  Also deals with "0z" for blob.
 * Return OK or FAIL.
 */
    पूर्णांक
eval_संख्या(
	char_u	    **arg,
	typval_T    *rettv,
	पूर्णांक	    evaluate,
	पूर्णांक	    want_string)
{
    पूर्णांक		len;
    पूर्णांक		skip_quotes = !in_old_script(4);
    char_u	*p;
    पूर्णांक		get_float = FALSE;

    // We accept a float when the format matches
    // "[0-9]\+\.[0-9]\+\([eE][+-]\?[0-9]\+\)\?".  This is very
    // strict to avoid backwards compatibility problems.
    // With script version 2 and later the leading digit can be
    // omitted.
    // Don't look for a float after the "." operator, so that
    // ":let vers = 1.2.3" doesn't fail.
    जर (**arg == '.')
	p = *arg;
    अन्यथा
    {
	p = *arg + 1;
	जर (skip_quotes)
	    साठी (;;)
	    {
		जर (*p == '\'')
		    ++p;
		जर (!vim_isdigit(*p))
		    थांबा;
		p = skipdigits(p);
	    }
	अन्यथा
	    p = skipdigits(p);
    }
    जर (!want_string && p[0] == '.' && vim_isdigit(p[1]))
    {
	get_float = TRUE;
	p = skipdigits(p + 2);
	जर (*p == 'e' || *p == 'E')
	{
	    ++p;
	    जर (*p == '-' || *p == '+')
		++p;
	    जर (!vim_isdigit(*p))
		get_float = FALSE;
	    अन्यथा
		p = skipdigits(p + 1);
	}
	जर (ASCII_ISALPHA(*p) || *p == '.')
	    get_float = FALSE;
    }
    जर (get_float)
    {
	float_T	f;

	*arg += string2float(*arg, &f, skip_quotes);
	जर (evaluate)
	{
	    rettv->v_type = VAR_FLOAT;
	    rettv->vval.v_float = f;
	}
    }
    अन्यथा
    जर (**arg == '0' && ((*arg)[1] == 'z' || (*arg)[1] == 'Z'))
    {
	char_u  *bp;
	blob_T  *blob = रिक्त;  // init for gcc

	// Blob constant: 0z0123456789abcdef
	जर (evaluate)
	    blob = blob_alloc();
	साठी (bp = *arg + 2; vim_isxdigit(bp[0]); bp += 2)
	{
	    जर (!vim_isxdigit(bp[1]))
	    {
		जर (blob != रिक्त)
		{
		    emsg(_(e_blob_literal_should_have_an_even_संख्या_of_hex_characters));
		    ga_clear(&blob->bv_ga);
		    VIM_CLEAR(blob);
		}
		परत FAIL;
	    }
	    जर (blob != रिक्त)
		ga_append(&blob->bv_ga,
			     (hex2nr(*bp) << 4) + hex2nr(*(bp+1)));
	    जर (bp[2] == '.' && vim_isxdigit(bp[3]))
		++bp;
	}
	जर (blob != रिक्त)
	    rettv_blob_set(rettv, blob);
	*arg = bp;
    }
    अन्यथा
    {
	varnumber_T	n;

	// decimal, hex or octal number
	vim_str2nr(*arg, रिक्त, &len, skip_quotes
		      ? STR2NR_NO_OCT + STR2NR_QUOTE
		      : STR2NR_ALL, &n, रिक्त, 0, TRUE, रिक्त);
	जर (len == 0)
	{
	    जर (evaluate)
		semsg(_(e_invalid_expression_str), *arg);
	    परत FAIL;
	}
	*arg += len;
	जर (evaluate)
	{
	    rettv->v_type = VAR_संख्या;
	    rettv->vval.v_संख्या = n;
	}
    }
    परत OK;
}

/*
 * Evaluate a string constant and put the result in "rettv".
 * "*arg" points to the double quote or to after it when "interpolate" is TRUE.
 * When "interpolate" is TRUE reduce "{{" to "{", reduce "}}" to "}" and stop
 * at a single "{".
 * Return OK or FAIL.
 */
    पूर्णांक
eval_string(char_u **arg, typval_T *rettv, पूर्णांक evaluate, पूर्णांक interpolate)
{
    char_u	*p;
    char_u	*end;
    पूर्णांक		extra = interpolate ? 1 : 0;
    पूर्णांक		off = interpolate ? 0 : 1;
    पूर्णांक		len;

    // Find the end of the string, skipping backslashed characters.
    साठी (p = *arg + off; *p != NUL && *p != '"'; MB_PTR_ADV(p))
    {
	जर (*p == '\\' && p[1] != NUL)
	{
	    ++p;
	    // A "\<x>" form occupies at least 4 characters, and produces up
	    // to 9 characters (6 for the char and 3 for a modifier):
	    // reserve space for 5 extra.
	    जर (*p == '<')
	    {
		पूर्णांक		modifiers = 0;
		पूर्णांक		flags = FSK_KEYCODE | FSK_IN_STRING;

		extra += 5;

		// Skip to the '>' to avoid using '{' inside for string
		// interpolation.
		जर (p[1] != '*')
		    flags |= FSK_SIMPLIFY;
		जर (find_special_किल्ली(&p, &modifiers, flags, रिक्त) != 0)
		    --p;  // leave "p" on the ">"
	    }
	}
	अन्यथा जर (interpolate && (*p == '{' || *p == '}'))
	{
	    जर (*p == '{' && p[1] != '{') // start of expression
		थांबा;
	    ++p;
	    जर (p[-1] == '}' && *p != '}') // single '}' is an error
	    {
		semsg(_(e_stray_closing_curly_str), *arg);
		परत FAIL;
	    }
	    --extra;  // "{{" becomes "{", "}}" becomes "}"
	}
    }

    जर (*p != '"' && !(interpolate && *p == '{'))
    {
	semsg(_(e_missing_double_quote_str), *arg);
	परत FAIL;
    }

    // If only parsing, set *arg and return here
    जर (!evaluate)
    {
	*arg = p + off;
	परत OK;
    }

    // Copy the string into allocated memory, handling backslashed
    // characters.
    rettv->v_type = VAR_STRING;
    len = (पूर्णांक)(p - *arg + extra);
    rettv->vval.v_string = alloc(len);
    जर (rettv->vval.v_string == रिक्त)
	परत FAIL;
    end = rettv->vval.v_string;

    साठी (p = *arg + off; *p != NUL && *p != '"'; )
    {
	जर (*p == '\\')
	{
	    निवड (*++p)
	    {
		स्थिती 'b': *end++ = BS; ++p; थांबा;
		स्थिती 'e': *end++ = ESC; ++p; थांबा;
		स्थिती 'f': *end++ = FF; ++p; थांबा;
		स्थिती 'n': *end++ = NL; ++p; थांबा;
		स्थिती 'r': *end++ = CAR; ++p; थांबा;
		स्थिती 't': *end++ = TAB; ++p; थांबा;

		स्थिती 'X': // hex: "\x1", "\x12"
		स्थिती 'x':
		स्थिती 'u': // Unicode: "\u0023"
		स्थिती 'U':
			  जर (vim_isxdigit(p[1]))
			  {
			      पूर्णांक	n, nr;
			      पूर्णांक	c = SAFE_toupper(*p);

			      जर (c == 'X')
				  n = 2;
			      अन्यथा जर (*p == 'u')
				  n = 4;
			      अन्यथा
				  n = 8;
			      nr = 0;
			      पर्यंत (--n >= 0 && vim_isxdigit(p[1]))
			      {
				  ++p;
				  nr = (nr << 4) + hex2nr(*p);
			      }
			      ++p;
			      // For "\u" store the number according to
			      // 'encoding'.
			      जर (c != 'X')
				  end += (*mb_char2bytes)(nr, end);
			      अन्यथा
				  *end++ = nr;
			  }
			  थांबा;

			  // octal: "\1", "\12", "\123"
		स्थिती '0':
		स्थिती '1':
		स्थिती '2':
		स्थिती '3':
		स्थिती '4':
		स्थिती '5':
		स्थिती '6':
		स्थिती '7': *end = *p++ - '0';
			  जर (*p >= '0' && *p <= '7')
			  {
			      *end = (*end << 3) + *p++ - '0';
			      जर (*p >= '0' && *p <= '7')
				  *end = (*end << 3) + *p++ - '0';
			  }
			  ++end;
			  थांबा;

			  // Special key, e.g.: "\<C-W>"
		स्थिती '<':
			  {
			      पूर्णांक flags = FSK_KEYCODE | FSK_IN_STRING;

			      जर (p[1] != '*')
				  flags |= FSK_SIMPLIFY;
			      extra = trans_special(&p, end, flags, FALSE, रिक्त);
			      जर (extra != 0)
			      {
				  end += extra;
				  जर (end >= rettv->vval.v_string + len)
				      iemsg("eval_string() used more space than allocated");
				  थांबा;
			      }
			  }
			  // FALLTHROUGH

		पूर्वनिर्धारित: MB_COPY_CHAR(p, end);
			  थांबा;
	    }
	}
	अन्यथा
	{
	    जर (interpolate && (*p == '{' || *p == '}'))
	    {
		जर (*p == '{' && p[1] != '{') // start of expression
		    थांबा;
		++p;  // reduce "{{" to "{" and "}}" to "}"
	    }
	    MB_COPY_CHAR(p, end);
	}
    }
    *end = NUL;
    जर (*p == '"' && !interpolate)
	++p;
    *arg = p;

    परत OK;
}

/*
 * Allocate a variable for a 'str''ing' constant.
 * When "interpolate" is TRUE reduce "{{" to "{" and stop at a single "{".
 * Return OK when a "rettv" was set to the string.
 * Return FAIL on error, "rettv" is not set.
 */
    पूर्णांक
eval_lit_string(char_u **arg, typval_T *rettv, पूर्णांक evaluate, पूर्णांक interpolate)
{
    char_u	*p;
    char_u	*str;
    पूर्णांक		reduce = interpolate ? -1 : 0;
    पूर्णांक		off = interpolate ? 0 : 1;

    // Find the end of the string, skipping ''.
    साठी (p = *arg + off; *p != NUL; MB_PTR_ADV(p))
    {
	जर (*p == '\'')
	{
	    जर (p[1] != '\'')
		थांबा;
	    ++reduce;
	    ++p;
	}
	अन्यथा जर (interpolate)
	{
	    जर (*p == '{')
	    {
		जर (p[1] != '{')
		    थांबा;
		++p;
		++reduce;
	    }
	    अन्यथा जर (*p == '}')
	    {
		++p;
		जर (*p != '}')
		{
		    semsg(_(e_stray_closing_curly_str), *arg);
		    परत FAIL;
		}
		++reduce;
	    }
	}
    }

    जर (*p != '\'' && !(interpolate && *p == '{'))
    {
	semsg(_(e_missing_single_quote_str), *arg);
	परत FAIL;
    }

    // If only parsing return after setting "*arg"
    जर (!evaluate)
    {
	*arg = p + off;
	परत OK;
    }

    // Copy the string into allocated memory, handling '' to ' reduction and
    // any expressions.
    str = alloc((p - *arg) - reduce);
    जर (str == रिक्त)
	परत FAIL;
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = str;

    साठी (p = *arg + off; *p != NUL; )
    {
	जर (*p == '\'')
	{
	    जर (p[1] != '\'')
		थांबा;
	    ++p;
	}
	अन्यथा जर (interpolate && (*p == '{' || *p == '}'))
	{
	    जर (*p == '{' && p[1] != '{')
		थांबा;
	    ++p;
	}
	MB_COPY_CHAR(p, str);
    }
    *str = NUL;
    *arg = p + off;

    परत OK;
}

/*
 * Evaluate a single or double quoted string possibly containing expressions.
 * "arg" points to the '$'.  The result is put in "rettv".
 * Returns OK or FAIL.
 */
    पूर्णांक
eval_interp_string(char_u **arg, typval_T *rettv, पूर्णांक evaluate)
{
    typval_T	tv;
    पूर्णांक		ret = OK;
    पूर्णांक		quote;
    garray_T	ga;
    char_u	*p;

    ga_init2(&ga, 1, 80);

    // *arg is on the '$' character, move it to the first string character.
    ++*arg;
    quote = **arg;
    ++*arg;

    साठी (;;)
    {
	// Get the string up to the matching quote or to a single '{'.
	// "arg" is advanced to either the quote or the '{'.
	जर (quote == '"')
	    ret = eval_string(arg, &tv, evaluate, TRUE);
	अन्यथा
	    ret = eval_lit_string(arg, &tv, evaluate, TRUE);
	जर (ret == FAIL)
	    थांबा;
	जर (evaluate)
	{
	    ga_concat(&ga, tv.vval.v_string);
	    clear_tv(&tv);
	}

	जर (**arg != '{')
	{
	    // found terminating quote
	    ++*arg;
	    थांबा;
	}
	p = eval_one_अभिव्यक्ती_in_str(*arg, &ga, evaluate);
	जर (p == रिक्त)
	{
	    ret = FAIL;
	    थांबा;
	}
	*arg = p;
    }

    rettv->v_type = VAR_STRING;
    जर (ret == FAIL || !evaluate || ga_append(&ga, NUL) == FAIL)
    {
	ga_clear(&ga);
	rettv->vval.v_string = रिक्त;
	परत ret;
    }

    rettv->vval.v_string = ga.ga_data;
    परत OK;
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
    पूर्णांक		copyID)
{
    परत echo_string_core(tv, tofree, numbuf, copyID, FALSE, TRUE, FALSE);
}

/*
 * Get the value of an environment variable.
 * "arg" is pointing to the '$'.  It is advanced to after the name.
 * If the environment variable was not set, silently assume it is empty.
 * Return FAIL if the name is invalid.
 */
    पूर्णांक
eval_env_var(char_u **arg, typval_T *rettv, पूर्णांक evaluate)
{
    char_u	*string = रिक्त;
    पूर्णांक		len;
    पूर्णांक		cc;
    char_u	*name;
    पूर्णांक		mustfree = FALSE;

    ++*arg;
    name = *arg;
    len = get_env_len(arg);
    जर (evaluate)
    {
	जर (len == 0)
	    परत FAIL; // invalid empty name

	cc = name[len];
	name[len] = NUL;
	// first try vim_getenv(), fast for normal environment vars
	string = vim_getenv(name, &mustfree);
	जर (string != रिक्त && *string != NUL)
	{
	    जर (!mustfree)
		string = vim_strsave(string);
	}
	अन्यथा
	{
	    जर (mustfree)
		vim_free(string);

	    // next try expanding things like $VIM and ${HOME}
	    string = expand_env_save(name - 1);
	    जर (string != रिक्त && *string == '$')
		VIM_CLEAR(string);
	}
	name[len] = cc;

	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = string;
	rettv->v_lock = 0;
    }

    परत OK;
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
    पूर्णांक		did_emsg_before = did_emsg;

    जर (argvars[0].v_type != VAR_STRING || !in_vim9script())
	lnum = (linenr_T)tv_get_संख्या_chk(&argvars[0], रिक्त);
    जर (lnum <= 0 && did_emsg_before == did_emsg
					    && argvars[0].v_type != VAR_संख्या)
    {
	पूर्णांक	fnum;
	pos_T	*fp;

	// no valid number, try using arg like line()
	fp = var2fpos(&argvars[0], TRUE, &fnum, FALSE);
	जर (fp != रिक्त)
	    lnum = fp->lnum;
    }
    परत lnum;
}

/*
 * Get the lnum from the first argument.
 * Also accepts "$", then "buf" is used.
 * Returns 0 on error.
 */
    linenr_T
tv_get_lnum_buf(typval_T *argvars, buf_T *buf)
{
    जर (argvars[0].v_type == VAR_STRING
	    && argvars[0].vval.v_string != रिक्त
	    && argvars[0].vval.v_string[0] == '$'
	    && argvars[0].vval.v_string[1] == NUL
	    && buf != रिक्त)
	परत buf->b_ml.ml_ओळ_मोजणी;
    परत (linenr_T)tv_get_संख्या_chk(&argvars[0], रिक्त);
}

/*
 * Get buffer by number or pattern.
 */
    buf_T *
tv_get_buf(typval_T *tv, पूर्णांक curtab_only)
{
    char_u	*name = tv->vval.v_string;
    buf_T	*buf;

    जर (tv->v_type == VAR_संख्या)
	परत buflist_findnr((पूर्णांक)tv->vval.v_संख्या);
    जर (tv->v_type != VAR_STRING)
	परत रिक्त;
    जर (name == रिक्त || *name == NUL)
	परत curbuf;
    जर (name[0] == '$' && name[1] == NUL)
	परत lastbuf;

    buf = buflist_find_by_name(name, curtab_only);

    // If not found, try expanding the name, like done for bufexists().
    जर (buf == रिक्त)
	buf = find_buffer(tv);

    परत buf;
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
    जर (buf == रिक्त
	    && tv->v_type != VAR_संख्या
	    && tv->v_type != VAR_STRING)
	// issue errmsg for type error
	(शून्य)tv_get_संख्या(tv);
    परत buf;
}

/*
 * Check that repeating "slen" items "n" times, does not
 * overflow the int used for the resulting length.
 */
    पूर्णांक
check_repeat_मोजणी(varnumber_T slen, varnumber_T n)
{
    // Use a division so that the multiplication cannot overflow.
    जर (n <= (varnumber_T)पूर्णांक_उच्च / slen)
	परत OK;

    अक्षर	buf[NUMBUFLEN];

    vim_snprintf(buf, आकार(buf), "%lld", (दीर्घ दीर्घ)n);
    semsg(_(e_val_too_large), buf);
    परत FAIL;
}

#endif // FEAT_EVAL
