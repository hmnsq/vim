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

#அந்தர்பாவ "vim.h"

#எனில் defined(FEAT_EVAL)

/*
 * Allocate memory for a variable type-value, and make it empty (0 or NULL
 * value).
 */
    typval_T *
alloc_tv(வெறுமை)
{
    திரும்பு ALLOC_CLEAR_ONE(typval_T);
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
    எனில் (rettv != வெற்று)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = s;
    }
    இல்லையெனில்
	vim_free(s);
    திரும்பு rettv;
}

/*
 * Free the memory for a variable type-value.
 */
    வெறுமை
free_tv(typval_T *varp)
{
    எனில் (varp == வெற்று)
	திரும்பு;

    தேர்வு (varp->v_type)
    {
	நிலை VAR_FUNC:
	    func_unref(varp->vval.v_string);
	    // FALLTHROUGH
	நிலை VAR_STRING:
	    vim_free(varp->vval.v_string);
	    நிறுத்து;
	நிலை VAR_PARTIAL:
	    partial_unref(varp->vval.v_partial);
	    நிறுத்து;
	நிலை VAR_BLOB:
	    blob_unref(varp->vval.v_blob);
	    நிறுத்து;
	நிலை VAR_LIST:
	    list_unref(varp->vval.v_list);
	    நிறுத்து;
	நிலை VAR_TUPLE:
	    tuple_unref(varp->vval.v_tuple);
	    நிறுத்து;
	நிலை VAR_DICT:
	    dict_unref(varp->vval.v_dict);
	    நிறுத்து;
	நிலை VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    job_unref(varp->vval.v_job);
	    நிறுத்து;
#endif
	நிலை VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    channel_unref(varp->vval.v_channel);
	    நிறுத்து;
#endif
	நிலை VAR_CLASS:
	    class_unref(varp->vval.v_class);
	    நிறுத்து;
	நிலை VAR_OBJECT:
	    object_unref(varp->vval.v_object);
	    நிறுத்து;

	நிலை VAR_TYPEALIAS:
	    typealias_unref(varp->vval.v_typealias);
	    நிறுத்து;

	நிலை VAR_எண்:
	நிலை VAR_FLOAT:
	நிலை VAR_ANY:
	நிலை VAR_UNKNOWN:
	நிலை VAR_VOID:
	நிலை VAR_BOOL:
	நிலை VAR_SPECIAL:
	நிலை VAR_INSTR:
	    நிறுத்து;
    }
    vim_free(varp);
}

/*
 * Free the memory for a variable value and set the value to NULL or 0.
 */
    வெறுமை
clear_tv(typval_T *varp)
{
    எனில் (varp == வெற்று)
	திரும்பு;

    தேர்வு (varp->v_type)
    {
	நிலை VAR_FUNC:
	    func_unref(varp->vval.v_string);
	    // FALLTHROUGH
	நிலை VAR_STRING:
	    VIM_CLEAR(varp->vval.v_string);
	    நிறுத்து;
	நிலை VAR_PARTIAL:
	    partial_unref(varp->vval.v_partial);
	    varp->vval.v_partial = வெற்று;
	    நிறுத்து;
	நிலை VAR_BLOB:
	    blob_unref(varp->vval.v_blob);
	    varp->vval.v_blob = வெற்று;
	    நிறுத்து;
	நிலை VAR_LIST:
	    list_unref(varp->vval.v_list);
	    varp->vval.v_list = வெற்று;
	    நிறுத்து;
	நிலை VAR_TUPLE:
	    tuple_unref(varp->vval.v_tuple);
	    varp->vval.v_tuple = வெற்று;
	    நிறுத்து;
	நிலை VAR_DICT:
	    dict_unref(varp->vval.v_dict);
	    varp->vval.v_dict = வெற்று;
	    நிறுத்து;
	நிலை VAR_எண்:
	நிலை VAR_BOOL:
	நிலை VAR_SPECIAL:
	    varp->vval.v_எண் = 0;
	    நிறுத்து;
	நிலை VAR_FLOAT:
	    varp->vval.v_float = 0.0;
	    நிறுத்து;
	நிலை VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    job_unref(varp->vval.v_job);
	    varp->vval.v_job = வெற்று;
#endif
	    நிறுத்து;
	நிலை VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    channel_unref(varp->vval.v_channel);
	    varp->vval.v_channel = வெற்று;
#endif
	    நிறுத்து;
	நிலை VAR_INSTR:
	    VIM_CLEAR(varp->vval.v_instr);
	    நிறுத்து;
	நிலை VAR_CLASS:
	    class_unref(varp->vval.v_class);
	    varp->vval.v_class = வெற்று;
	    நிறுத்து;
	நிலை VAR_OBJECT:
	    object_unref(varp->vval.v_object);
	    varp->vval.v_object = வெற்று;
	    நிறுத்து;
	நிலை VAR_TYPEALIAS:
	    typealias_unref(varp->vval.v_typealias);
	    varp->vval.v_typealias = வெற்று;
	    நிறுத்து;
	நிலை VAR_UNKNOWN:
	நிலை VAR_ANY:
	நிலை VAR_VOID:
	    நிறுத்து;
    }
    varp->v_lock = 0;
}

/*
 * Set the value of a variable to NULL without freeing items.
 */
    வெறுமை
init_tv(typval_T *varp)
{
    எனில் (varp != வெற்று)
	CLEAR_POINTER(varp);
}

    நிலையான varnumber_T
tv_get_bool_or_எண்_chk(
	typval_T    *varp,
	முழு	    *denote,
	முழு	    want_bool,
	முழு	    vim9_string_error) // in Vim9 using a string is an error
{
    varnumber_T	n = 0L;

    தேர்வு (varp->v_type)
    {
	நிலை VAR_எண்:
	    எனில் (in_vim9script() && want_bool && varp->vval.v_எண் != 0
						   && varp->vval.v_எண் != 1)
	    {
		semsg(_(e_using_எண்_as_bool_nr), varp->vval.v_எண்);
		நிறுத்து;
	    }
	    திரும்பு varp->vval.v_எண்;
	நிலை VAR_FLOAT:
	    emsg(_(e_using_float_as_எண்));
	    நிறுத்து;
	நிலை VAR_FUNC:
	நிலை VAR_PARTIAL:
	    emsg(_(e_using_funcref_as_எண்));
	    நிறுத்து;
	நிலை VAR_STRING:
	    எனில் (vim9_string_error && in_vim9script())
	    {
		emsg_using_string_as(varp, !want_bool);
		நிறுத்து;
	    }
	    எனில் (varp->vval.v_string != வெற்று)
		vim_str2nr(varp->vval.v_string, வெற்று, வெற்று,
					 STR2NR_ALL, &n, வெற்று, 0, FALSE, வெற்று);
	    திரும்பு n;
	நிலை VAR_LIST:
	    emsg(_(e_using_list_as_எண்));
	    நிறுத்து;
	நிலை VAR_TUPLE:
	    emsg(_(e_using_tuple_as_எண்));
	    நிறுத்து;
	நிலை VAR_DICT:
	    emsg(_(e_using_dictionary_as_எண்));
	    நிறுத்து;
	நிலை VAR_BOOL:
	நிலை VAR_SPECIAL:
	    எனில் (!want_bool && in_vim9script())
	    {
		எனில் (varp->v_type == VAR_BOOL)
		    emsg(_(e_using_bool_as_எண்));
		இல்லையெனில்
		    emsg(_(e_using_special_as_எண்));
		நிறுத்து;
	    }
	    திரும்பு varp->vval.v_எண் == VVAL_TRUE ? 1 : 0;
	நிலை VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    emsg(_(e_using_job_as_எண்));
	    நிறுத்து;
#endif
	நிலை VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    emsg(_(e_using_channel_as_எண்));
	    நிறுத்து;
#endif
	நிலை VAR_BLOB:
	    emsg(_(e_using_blob_as_எண்));
	    நிறுத்து;
	நிலை VAR_CLASS:
	நிலை VAR_TYPEALIAS:
	    check_typval_is_மதிப்பு(varp);
	    நிறுத்து;
	நிலை VAR_OBJECT:
	    {
		எனில் (varp->vval.v_object == வெற்று)
		    emsg(_(e_using_object_as_string));
		இல்லையெனில்
		{
		    class_T *cl = varp->vval.v_object->obj_class;
		    எனில் (cl != வெற்று && IS_ENUM(cl))
			semsg(_(e_using_enum_str_as_எண்), cl->class_name.string);
		    இல்லையெனில்
			emsg(_(e_using_object_as_எண்));
		}
	    }
	    நிறுத்து;
	நிலை VAR_VOID:
	    emsg(_(e_cannot_use_void_மதிப்பு));
	    நிறுத்து;
	நிலை VAR_UNKNOWN:
	நிலை VAR_ANY:
	நிலை VAR_INSTR:
	    internal_error_no_abort("tv_get_number(UNKNOWN)");
	    நிறுத்து;
    }
    எனில் (denote == வெற்று)		// useful for values that must be unsigned
	n = -1;
    இல்லையெனில்
	*denote = TRUE;
    திரும்பு n;
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
tv_get_எண்(typval_T *varp)
{
    முழு		error = FALSE;

    திரும்பு tv_get_எண்_chk(varp, &error);	// return 0L on error
}

/*
 * Like tv_get_number() but in Vim9 script do convert a number in a string to a
 * number without giving an error.
 */
    varnumber_T
tv_to_எண்(typval_T *varp)
{
    முழு		error = FALSE;

    திரும்பு tv_get_bool_or_எண்_chk(varp, &error, FALSE, FALSE);
}

    varnumber_T
tv_get_எண்_chk(typval_T *varp, முழு *denote)
{
    திரும்பு tv_get_bool_or_எண்_chk(varp, denote, FALSE, TRUE);
}

/*
 * Get the boolean value of "varp".  This is like tv_get_number_chk(),
 * but in Vim9 script accepts Number (0 and 1) and Bool/Special.
 */
    varnumber_T
tv_get_bool(typval_T *varp)
{
    திரும்பு tv_get_bool_or_எண்_chk(varp, வெற்று, TRUE, TRUE);
}

/*
 * Get the boolean value of "varp".  This is like tv_get_number_chk(),
 * but in Vim9 script accepts Number and Bool.
 */
    varnumber_T
tv_get_bool_chk(typval_T *varp, முழு *denote)
{
    திரும்பு tv_get_bool_or_எண்_chk(varp, denote, TRUE, TRUE);
}

    நிலையான float_T
tv_get_float_chk(typval_T *varp, முழு *error)
{
    தேர்வு (varp->v_type)
    {
	நிலை VAR_எண்:
	    திரும்பு (float_T)(varp->vval.v_எண்);
	நிலை VAR_FLOAT:
	    திரும்பு varp->vval.v_float;
	நிலை VAR_FUNC:
	நிலை VAR_PARTIAL:
	    emsg(_(e_using_funcref_as_float));
	    நிறுத்து;
	நிலை VAR_STRING:
	    emsg(_(e_using_string_as_float));
	    நிறுத்து;
	நிலை VAR_LIST:
	    emsg(_(e_using_list_as_float));
	    நிறுத்து;
	நிலை VAR_TUPLE:
	    emsg(_(e_using_tuple_as_float));
	    நிறுத்து;
	நிலை VAR_DICT:
	    emsg(_(e_using_dictionary_as_float));
	    நிறுத்து;
	நிலை VAR_BOOL:
	    emsg(_(e_using_boolean_மதிப்பு_as_float));
	    நிறுத்து;
	நிலை VAR_SPECIAL:
	    emsg(_(e_using_special_மதிப்பு_as_float));
	    நிறுத்து;
	நிலை VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    emsg(_(e_using_job_as_float));
	    நிறுத்து;
#endif
	நிலை VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    emsg(_(e_using_channel_as_float));
	    நிறுத்து;
#endif
	நிலை VAR_BLOB:
	    emsg(_(e_using_blob_as_float));
	    நிறுத்து;
	நிலை VAR_CLASS:
	நிலை VAR_TYPEALIAS:
	    check_typval_is_மதிப்பு(varp);
	    நிறுத்து;
	நிலை VAR_OBJECT:
	    emsg(_(e_using_object_as_float));
	    நிறுத்து;
	நிலை VAR_VOID:
	    emsg(_(e_cannot_use_void_மதிப்பு));
	    நிறுத்து;
	நிலை VAR_UNKNOWN:
	நிலை VAR_ANY:
	நிலை VAR_INSTR:
	    internal_error_no_abort("tv_get_float(UNKNOWN)");
	    நிறுத்து;
    }
    எனில் (error != வெற்று)
	*error = TRUE;
    திரும்பு 0;
}

    float_T
tv_get_float(typval_T *varp)
{
    திரும்பு tv_get_float_chk(varp, வெற்று);
}

/*
 * Give an error and return FAIL unless "args[idx]" is unknown
 */
    முழு
check_for_unknown_arg(typval_T *args, முழு idx)
{
    எனில் (args[idx].v_type != VAR_UNKNOWN)
    {
	semsg(_(e_too_many_arguments), idx + 1);
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string.
 */
    முழு
check_for_string_arg(typval_T *args, முழு idx)
{
    எனில் (args[idx].v_type != VAR_STRING)
    {
	semsg(_(e_string_required_for_argument_nr), idx + 1);
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a non-empty string.
 */
    முழு
check_for_nonempty_string_arg(typval_T *args, முழு idx)
{
    எனில் (check_for_string_arg(args, idx) == FAIL)
	திரும்பு FAIL;
    எனில் (args[idx].vval.v_string == வெற்று || *args[idx].vval.v_string == NUL)
    {
	semsg(_(e_non_empty_string_required_for_argument_nr), idx + 1);
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Check for an optional string argument at 'idx'
 */
    முழு
check_for_opt_string_arg(typval_T *args, முழு idx)
{
    திரும்பு (args[idx].v_type == VAR_UNKNOWN
	    || check_for_string_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a number.
 */
    முழு
check_for_எண்_arg(typval_T *args, முழு idx)
{
    எனில் (args[idx].v_type != VAR_எண்)
    {
	semsg(_(e_எண்_required_for_argument_nr), idx + 1);
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Check for an optional number argument at 'idx'
 */
    முழு
check_for_opt_எண்_arg(typval_T *args, முழு idx)
{
    திரும்பு (args[idx].v_type == VAR_UNKNOWN
	    || check_for_எண்_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a float or a number.
 */
    முழு
check_for_float_or_nr_arg(typval_T *args, முழு idx)
{
    எனில் (args[idx].v_type != VAR_FLOAT && args[idx].v_type != VAR_எண்)
    {
	semsg(_(e_float_or_எண்_required_for_argument_nr), idx + 1);
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a bool.
 */
    முழு
check_for_bool_arg(typval_T *args, முழு idx)
{
    எனில் (args[idx].v_type != VAR_BOOL
	    && !(args[idx].v_type == VAR_எண்
		&& (args[idx].vval.v_எண் == 0
		    || args[idx].vval.v_எண் == 1)))
    {
	semsg(_(e_bool_required_for_argument_nr), idx + 1);
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a bool or a number.
 */
    நிலையான முழு
check_for_bool_or_எண்_arg(typval_T *args, முழு idx)
{
    எனில் (args[idx].v_type != VAR_BOOL && args[idx].v_type != VAR_எண்)
    {
	semsg(_(e_bool_or_எண்_required_for_argument_nr), idx + 1);
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Check for an optional bool argument at 'idx'.
 * Return FAIL if the type is wrong.
 */
    முழு
check_for_opt_bool_arg(typval_T *args, முழு idx)
{
    எனில் (args[idx].v_type == VAR_UNKNOWN)
	திரும்பு OK;
    திரும்பு check_for_bool_arg(args, idx);
}

/*
 * Check for an optional bool or number argument at 'idx'.
 * Return FAIL if the type is wrong.
 */
    முழு
check_for_opt_bool_or_எண்_arg(typval_T *args, முழு idx)
{
    எனில் (args[idx].v_type == VAR_UNKNOWN)
	திரும்பு OK;
    திரும்பு check_for_bool_or_எண்_arg(args, idx);
}

/*
 * Check for an optional bool or dict argument at 'idx'.
 * Return FAIL if the type is wrong.
 */
    முழு
check_for_opt_bool_or_dict_arg(typval_T *args, முழு idx)
{
    எனில் (args[idx].v_type == VAR_UNKNOWN || args[idx].v_type == VAR_DICT)
	திரும்பு OK;
    திரும்பு check_for_bool_arg(args, idx);
}

/*
 * Give an error and return FAIL unless "args[idx]" is a blob.
 */
    முழு
check_for_blob_arg(typval_T *args, முழு idx)
{
    எனில் (args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_blob_required_for_argument_nr), idx + 1);
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list.
 */
    முழு
check_for_list_arg(typval_T *args, முழு idx)
{
    எனில் (args[idx].v_type != VAR_LIST)
    {
	semsg(_(e_list_required_for_argument_nr), idx + 1);
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a non-NULL list.
 */
    முழு
check_for_nonnull_list_arg(typval_T *args, முழு idx)
{
    எனில் (check_for_list_arg(args, idx) == FAIL)
	திரும்பு FAIL;

    எனில் (args[idx].vval.v_list == வெற்று)
    {
	semsg(_(e_non_null_list_required_for_argument_nr), idx + 1);
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Check for an optional list argument at 'idx'
 */
    முழு
check_for_opt_list_arg(typval_T *args, முழு idx)
{
    திரும்பு (args[idx].v_type == VAR_UNKNOWN
	    || check_for_list_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a tuple.
 */
    முழு
check_for_tuple_arg(typval_T *args, முழு idx)
{
    எனில் (args[idx].v_type != VAR_TUPLE)
    {
	semsg(_(e_tuple_required_for_argument_nr), idx + 1);
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a dict.
 */
    முழு
check_for_dict_arg(typval_T *args, முழு idx)
{
    எனில் (args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_dict_required_for_argument_nr), idx + 1);
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a non-NULL dict.
 */
    முழு
check_for_nonnull_dict_arg(typval_T *args, முழு idx)
{
    எனில் (check_for_dict_arg(args, idx) == FAIL)
	திரும்பு FAIL;

    எனில் (args[idx].vval.v_dict == வெற்று)
    {
	semsg(_(e_non_null_dict_required_for_argument_nr), idx + 1);
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Check for an optional dict argument at 'idx'
 */
    முழு
check_for_opt_dict_arg(typval_T *args, முழு idx)
{
    திரும்பு (args[idx].v_type == VAR_UNKNOWN
	    || check_for_dict_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Check for an optional non-NULL dict argument at 'idx'
 */
    முழு
check_for_opt_nonnull_dict_arg(typval_T *args, முழு idx)
{
    திரும்பு (args[idx].v_type == VAR_UNKNOWN
	    || check_for_nonnull_dict_arg(args, idx) != FAIL) ? OK : FAIL;
}

#எனில் defined(FEAT_JOB_CHANNEL)
/*
 * Give an error and return FAIL unless "args[idx]" is a channel or a job.
 */
    முழு
check_for_chan_or_job_arg(typval_T *args, முழு idx)
{
    எனில் (args[idx].v_type != VAR_CHANNEL && args[idx].v_type != VAR_JOB)
    {
	semsg(_(e_chan_or_job_required_for_argument_nr), idx + 1);
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an optional channel or a
 * job.
 */
    முழு
check_for_opt_chan_or_job_arg(typval_T *args, முழு idx)
{
    திரும்பு (args[idx].v_type == VAR_UNKNOWN
	    || check_for_chan_or_job_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a job.
 */
    முழு
check_for_job_arg(typval_T *args, முழு idx)
{
    எனில் (args[idx].v_type != VAR_JOB)
    {
	semsg(_(e_job_required_for_argument_nr), idx + 1);
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Check for an optional job argument at 'idx'.
 */
    முழு
check_for_opt_job_arg(typval_T *args, முழு idx)
{
    திரும்பு (args[idx].v_type == VAR_UNKNOWN
	    || check_for_job_arg(args, idx) != FAIL) ? OK : FAIL;
}
#இல்லையெனில்
/*
 * Give an error and return FAIL unless "args[idx]" is an optional channel or a
 * job.  Used without the +channel feature, thus only VAR_UNKNOWN is accepted.
 */
    முழு
check_for_opt_chan_or_job_arg(typval_T *args, முழு idx)
{
    திரும்பு args[idx].v_type == VAR_UNKNOWN ? OK : FAIL;
}
#endif

/*
 * Give an error and return FAIL unless "args[idx]" is a string or
 * a number.
 */
    முழு
check_for_string_or_எண்_arg(typval_T *args, முழு idx)
{
    எனில் (args[idx].v_type != VAR_STRING && args[idx].v_type != VAR_எண்)
    {
	semsg(_(e_string_or_எண்_required_for_argument_nr), idx + 1);
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Check for an optional string or number argument at 'idx'.
 */
    முழு
check_for_opt_string_or_எண்_arg(typval_T *args, முழு idx)
{
    திரும்பு (args[idx].v_type == VAR_UNKNOWN
	    || check_for_string_or_எண்_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a buffer number.
 * Buffer number can be a number or a string.
 */
    முழு
check_for_buffer_arg(typval_T *args, முழு idx)
{
    திரும்பு check_for_string_or_எண்_arg(args, idx);
}

/*
 * Check for an optional buffer argument at 'idx'
 */
    முழு
check_for_opt_buffer_arg(typval_T *args, முழு idx)
{
    திரும்பு (args[idx].v_type == VAR_UNKNOWN
	    || check_for_buffer_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a line number.
 * Line number can be a number or a string.
 */
    முழு
check_for_lnum_arg(typval_T *args, முழு idx)
{
    திரும்பு check_for_string_or_எண்_arg(args, idx);
}

/*
 * Check for an optional line number argument at 'idx'
 */
    முழு
check_for_opt_lnum_arg(typval_T *args, முழு idx)
{
    திரும்பு (args[idx].v_type == VAR_UNKNOWN
	    || check_for_lnum_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a blob.
 */
    முழு
check_for_string_or_blob_arg(typval_T *args, முழு idx)
{
    எனில் (args[idx].v_type != VAR_STRING && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_string_or_blob_required_for_argument_nr), idx + 1);
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a list.
 */
    முழு
check_for_string_or_list_arg(typval_T *args, முழு idx)
{
    எனில் (args[idx].v_type != VAR_STRING && args[idx].v_type != VAR_LIST)
    {
	semsg(_(e_string_or_list_required_for_argument_nr), idx + 1);
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string, a list, a
 * tuple or a blob.
 */
    முழு
check_for_string_or_list_or_tuple_or_blob_arg(typval_T *args, முழு idx)
{
    எனில் (args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_string_list_tuple_or_blob_required_for_argument_nr), idx + 1);
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Check for an optional string or list argument at 'idx'
 */
    முழு
check_for_opt_string_or_list_arg(typval_T *args, முழு idx)
{
    திரும்பு (args[idx].v_type == VAR_UNKNOWN
	    || check_for_string_or_list_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a dict.
 */
    முழு
check_for_string_or_dict_arg(typval_T *args, முழு idx)
{
    எனில் (args[idx].v_type != VAR_STRING && args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_string_or_dict_required_for_argument_nr), idx + 1);
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a number
 * or a list.
 */
    முழு
check_for_string_or_எண்_or_list_arg(typval_T *args, முழு idx)
{
    எனில் (args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_எண்
	    && args[idx].v_type != VAR_LIST)
    {
	semsg(_(e_string_எண்_or_list_required_for_argument_nr), idx + 1);
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an optional string
 * or number or a list
 */
    முழு
check_for_opt_string_or_எண்_or_list_arg(typval_T *args, முழு idx)
{
    திரும்பு (args[idx].v_type == VAR_UNKNOWN
	    || check_for_string_or_எண்_or_list_arg(args, idx)
							!= FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string, a number, a
 * list, a tuple or a blob.
 */
    முழு
check_for_repeat_func_arg(typval_T *args, முழு idx)
{
    எனில் (args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_எண்
	    && args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_repeatable_type_required_for_argument_nr), idx + 1);
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string, a list, a
 * tuple or a dict.
 */
    முழு
check_for_string_list_tuple_or_dict_arg(typval_T *args, முழு idx)
{
    எனில் (args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_string_list_tuple_or_dict_required_for_argument_nr), idx + 1);
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string
 * or a function reference.
 */
    முழு
check_for_string_or_func_arg(typval_T *args, முழு idx)
{
    எனில் (args[idx].v_type != VAR_PARTIAL
	    && args[idx].v_type != VAR_FUNC
	    && args[idx].v_type != VAR_STRING)
    {
	semsg(_(e_string_or_function_required_for_argument_nr), idx + 1);
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list or a blob.
 */
    முழு
check_for_list_or_blob_arg(typval_T *args, முழு idx)
{
    எனில் (args[idx].v_type != VAR_LIST && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_list_or_blob_required_for_argument_nr), idx + 1);
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list or a tuple.
 */
    முழு
check_for_list_or_tuple_arg(typval_T *args, முழு idx)
{
    எனில் (args[idx].v_type != VAR_LIST && args[idx].v_type != VAR_TUPLE)
    {
	semsg(_(e_list_or_tuple_required_for_argument_nr), idx + 1);
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list, a tuple or a
 * blob.
 */
    முழு
check_for_list_or_tuple_or_blob_arg(typval_T *args, முழு idx)
{
    எனில் (args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_list_or_tuple_or_blob_required_for_argument_nr), idx + 1);
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list, a tuple or a
 * dict
 */
    முழு
check_for_list_or_tuple_or_dict_arg(typval_T *args, முழு idx)
{
    எனில் (args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_list_or_tuple_or_dict_required_for_argument_nr), idx + 1);
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list or dict or a
 * blob.
 */
    முழு
check_for_list_or_dict_or_blob_arg(typval_T *args, முழு idx)
{
    எனில் (args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_DICT
	    && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_list_dict_or_blob_required_for_argument_nr), idx + 1);
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list, a tuple, a dict,
 * a blob or a string.
 */
    முழு
check_for_list_tuple_dict_blob_or_string_arg(typval_T *args, முழு idx)
{
    எனில் (args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_DICT
	    && args[idx].v_type != VAR_BLOB
	    && args[idx].v_type != VAR_STRING)
    {
	semsg(_(e_list_tuple_dict_blob_or_string_required_for_argument_nr), idx + 1);
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an optional buffer
 * number or a dict.
 */
    முழு
check_for_opt_buffer_or_dict_arg(typval_T *args, முழு idx)
{
    எனில் (args[idx].v_type != VAR_UNKNOWN
	    && args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_எண்
	    && args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_string_required_for_argument_nr), idx + 1);
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an object.
 */
    முழு
check_for_object_arg(typval_T *args, முழு idx)
{
    எனில் (args[idx].v_type != VAR_OBJECT)
    {
	semsg(_(e_object_required_for_argument_nr), idx + 1);
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Returns TRUE if "tv" is a type alias for a class
 */
    நிலையான முழு
tv_class_alias(typval_T *tv)
{
    திரும்பு tv->v_type == VAR_TYPEALIAS &&
			tv->vval.v_typealias->ta_type->tt_type == VAR_OBJECT;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a class
 * or class typealias.
 */
    முழு
check_for_class_or_typealias_args(typval_T *args, முழு idx)
{
    க்கு (முழு i = idx; args[i].v_type != VAR_UNKNOWN; ++i)
    {
	எனில் (args[i].v_type != VAR_CLASS && !tv_class_alias(&args[idx]))
	{
	    semsg(_(e_class_or_typealias_required_for_argument_nr), i + 1);
	    திரும்பு FAIL;
	}
    }
    திரும்பு OK;
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
    நிலையான char_u   mybuf[NUMBUFLEN];

    திரும்பு tv_get_string_buf(varp, mybuf);
}

/*
 * Like tv_get_string() but don't allow number to string conversion for Vim9.
 */
    char_u *
tv_get_string_strict(typval_T *varp)
{
    நிலையான char_u   mybuf[NUMBUFLEN];
    char_u	    *res =  tv_get_string_buf_chk_strict(
						 varp, mybuf, in_vim9script());

    திரும்பு res != வெற்று ? res : (char_u *)"";
}

    char_u *
tv_get_string_buf(typval_T *varp, char_u *buf)
{
    char_u	*res = tv_get_string_buf_chk(varp, buf);

    திரும்பு res != வெற்று ? res : (char_u *)"";
}

/*
 * Careful: This uses a single, static buffer.  YOU CAN ONLY USE IT ONCE!
 */
    char_u *
tv_get_string_chk(typval_T *varp)
{
    நிலையான char_u   mybuf[NUMBUFLEN];

    திரும்பு tv_get_string_buf_chk(varp, mybuf);
}

    char_u *
tv_get_string_buf_chk(typval_T *varp, char_u *buf)
{
    திரும்பு tv_get_string_buf_chk_strict(varp, buf, FALSE);
}

    char_u *
tv_get_string_buf_chk_strict(typval_T *varp, char_u *buf, முழு strict)
{
    தேர்வு (varp->v_type)
    {
	நிலை VAR_எண்:
	    எனில் (strict)
	    {
		emsg(_(e_using_எண்_as_string));
		நிறுத்து;
	    }
	    vim_snprintf((எழுத்து *)buf, NUMBUFLEN, "%lld",
					    (varnumber_T)varp->vval.v_எண்);
	    திரும்பு buf;
	நிலை VAR_FUNC:
	நிலை VAR_PARTIAL:
	    emsg(_(e_using_funcref_as_string));
	    நிறுத்து;
	நிலை VAR_LIST:
	    emsg(_(e_using_list_as_string));
	    நிறுத்து;
	நிலை VAR_TUPLE:
	    emsg(_(e_using_tuple_as_string));
	    நிறுத்து;
	நிலை VAR_DICT:
	    emsg(_(e_using_dictionary_as_string));
	    நிறுத்து;
	நிலை VAR_FLOAT:
	    எனில் (strict)
	    {
		emsg(_(e_using_float_as_string));
		நிறுத்து;
	    }
	    vim_snprintf((எழுத்து *)buf, NUMBUFLEN, "%g", varp->vval.v_float);
	    திரும்பு buf;
	நிலை VAR_STRING:
	    எனில் (varp->vval.v_string != வெற்று)
		திரும்பு varp->vval.v_string;
	    திரும்பு (char_u *)"";
	நிலை VAR_BOOL:
	நிலை VAR_SPECIAL:
	    STRCPY(buf, get_var_special_name(varp->vval.v_எண்));
	    திரும்பு buf;
	நிலை VAR_BLOB:
	    emsg(_(e_using_blob_as_string));
	    நிறுத்து;
	நிலை VAR_CLASS:
	நிலை VAR_TYPEALIAS:
	    check_typval_is_மதிப்பு(varp);
	    நிறுத்து;
	நிலை VAR_OBJECT:
	    {
		எனில் (varp->vval.v_object == வெற்று)
		    emsg(_(e_using_object_as_string));
		இல்லையெனில்
		{
		    class_T *cl = varp->vval.v_object->obj_class;
		    எனில் (cl != வெற்று && IS_ENUM(cl))
			semsg(_(e_using_enum_str_as_string), cl->class_name.string);
		    இல்லையெனில்
			emsg(_(e_using_object_as_string));
		}
	    }
	    நிறுத்து;
	நிலை VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    எனில் (in_vim9script())
	    {
		semsg(_(e_using_invalid_மதிப்பு_as_string_str), "job");
		நிறுத்து;
	    }
	    திரும்பு job_to_string_buf(varp, buf);
#endif
	    நிறுத்து;
	நிலை VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    எனில் (in_vim9script())
	    {
		semsg(_(e_using_invalid_மதிப்பு_as_string_str), "channel");
		நிறுத்து;
	    }
	    திரும்பு channel_to_string_buf(varp, buf);
#endif
	    நிறுத்து;
	நிலை VAR_VOID:
	    emsg(_(e_cannot_use_void_மதிப்பு));
	    நிறுத்து;
	நிலை VAR_UNKNOWN:
	நிலை VAR_ANY:
	நிலை VAR_INSTR:
	    semsg(_(e_using_invalid_மதிப்பு_as_string_str),
						  vartype_name(varp->v_type));
	    நிறுத்து;
    }
    திரும்பு வெற்று;
}

/*
 * Turn a typeval into a string.  Similar to tv_get_string_buf() but uses
 * string() on Dict, List, etc.
 */
    char_u *
tv_stringify(typval_T *varp, char_u *buf)
{
    எனில் (varp->v_type == VAR_LIST
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
	திரும்பு tmp.vval.v_string;
    }
    திரும்பு tv_get_string_buf(varp, buf);
}

/*
 * Return TRUE if typeval "tv" and its value are set to be locked (immutable).
 * Also give an error message, using "name" or _("name") when use_gettext is
 * TRUE.
 */
    முழு
tv_check_lock(typval_T *tv, char_u *name, முழு use_gettext)
{
    முழு	lock = 0;

    தேர்வு (tv->v_type)
    {
	நிலை VAR_BLOB:
	    எனில் (tv->vval.v_blob != வெற்று)
		lock = tv->vval.v_blob->bv_lock;
	    நிறுத்து;
	நிலை VAR_LIST:
	    எனில் (tv->vval.v_list != வெற்று)
		lock = tv->vval.v_list->lv_lock;
	    நிறுத்து;
	நிலை VAR_TUPLE:
	    எனில் (tv->vval.v_tuple != வெற்று)
		lock = tv->vval.v_tuple->tv_lock;
	    நிறுத்து;
	நிலை VAR_DICT:
	    எனில் (tv->vval.v_dict != வெற்று)
		lock = tv->vval.v_dict->dv_lock;
	    நிறுத்து;
	இயல்பு:
	    நிறுத்து;
    }
    திரும்பு மதிப்பு_check_lock(tv->v_lock, name, use_gettext)
		   || (lock != 0 && மதிப்பு_check_lock(lock, name, use_gettext));
}

/*
 * Copy the values from typval_T "from" to typval_T "to".
 * When needed allocates string or increases reference count.
 * Does not make a copy of a list, blob or dict but copies the reference!
 * It is OK for "from" and "to" to point to the same item.  This is used to
 * make a copy later.
 */
    முழு
copy_tv(typval_T *from, typval_T *to)
{
    முழு		ret = OK;

    to->v_type = from->v_type;
    to->v_lock = 0;
    தேர்வு (from->v_type)
    {
	நிலை VAR_எண்:
	நிலை VAR_BOOL:
	நிலை VAR_SPECIAL:
	    to->vval.v_எண் = from->vval.v_எண்;
	    நிறுத்து;
	நிலை VAR_FLOAT:
	    to->vval.v_float = from->vval.v_float;
	    நிறுத்து;
	நிலை VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    to->vval.v_job = from->vval.v_job;
	    எனில் (to->vval.v_job != வெற்று)
		++to->vval.v_job->jv_refcount;
	    நிறுத்து;
#endif
	நிலை VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    to->vval.v_channel = from->vval.v_channel;
	    எனில் (to->vval.v_channel != வெற்று)
		++to->vval.v_channel->ch_refcount;
	    நிறுத்து;
#endif
	நிலை VAR_INSTR:
	    to->vval.v_instr = from->vval.v_instr;
	    நிறுத்து;

	நிலை VAR_CLASS:
	    copy_class(from, to);
	    நிறுத்து;

	நிலை VAR_OBJECT:
	    copy_object(from, to);
	    நிறுத்து;

	நிலை VAR_STRING:
	நிலை VAR_FUNC:
	    எனில் (from->vval.v_string == வெற்று)
		to->vval.v_string = வெற்று;
	    இல்லையெனில்
	    {
		to->vval.v_string = vim_strsave(from->vval.v_string);
		எனில் (from->v_type == VAR_FUNC)
		    func_ref(to->vval.v_string);
	    }
	    நிறுத்து;
	நிலை VAR_PARTIAL:
	    எனில் (from->vval.v_partial == வெற்று)
		to->vval.v_partial = வெற்று;
	    இல்லையெனில்
	    {
		to->vval.v_partial = from->vval.v_partial;
		++to->vval.v_partial->pt_refcount;
	    }
	    நிறுத்து;
	நிலை VAR_BLOB:
	    எனில் (from->vval.v_blob == வெற்று)
		to->vval.v_blob = வெற்று;
	    இல்லையெனில்
	    {
		to->vval.v_blob = from->vval.v_blob;
		++to->vval.v_blob->bv_refcount;
	    }
	    நிறுத்து;
	நிலை VAR_LIST:
	    எனில் (from->vval.v_list == வெற்று)
		to->vval.v_list = வெற்று;
	    இல்லையெனில்
	    {
		to->vval.v_list = from->vval.v_list;
		++to->vval.v_list->lv_refcount;
	    }
	    நிறுத்து;
	நிலை VAR_TUPLE:
	    எனில் (from->vval.v_tuple == வெற்று)
		to->vval.v_tuple = வெற்று;
	    இல்லையெனில்
	    {
		to->vval.v_tuple = from->vval.v_tuple;
		++to->vval.v_tuple->tv_refcount;
	    }
	    நிறுத்து;
	நிலை VAR_DICT:
	    எனில் (from->vval.v_dict == வெற்று)
		to->vval.v_dict = வெற்று;
	    இல்லையெனில்
	    {
		to->vval.v_dict = from->vval.v_dict;
		++to->vval.v_dict->dv_refcount;
	    }
	    நிறுத்து;
	நிலை VAR_TYPEALIAS:
	    எனில் (from->vval.v_typealias == வெற்று)
		to->vval.v_typealias = வெற்று;
	    இல்லையெனில்
	    {
		to->vval.v_typealias = from->vval.v_typealias;
		++to->vval.v_typealias->ta_refcount;
	    }
	    நிறுத்து;
	நிலை VAR_VOID:
	    emsg(_(e_cannot_use_void_மதிப்பு));
	    ret = FAIL;
	    நிறுத்து;
	நிலை VAR_UNKNOWN:
	நிலை VAR_ANY:
	    internal_error_no_abort("copy_tv(UNKNOWN)");
	    ret = FAIL;
	    நிறுத்து;
    }

    திரும்பு ret;
}

/*
 * Compare "tv1" and "tv2".
 */
    முழு
typval_compare2(
    typval_T	*tv1,	// first operand
    typval_T	*tv2,	// second operand
    exprtype_T	type,	// operator
    முழு		ic,	// ignore case
    முழு		*res)	// comparison result
{
    varnumber_T	n1, n2;
    முழு		type_is = type == கோவை_IS || type == கோவை_ISNOT;

    எனில் (check_typval_is_மதிப்பு(tv1) == FAIL
	|| check_typval_is_மதிப்பு(tv2) == FAIL)
	திரும்பு FAIL;
    இல்லையெனில் எனில் (type_is && tv1->v_type != tv2->v_type)
    {
	// For "is" a different type always means FALSE, for "isnot"
	// it means TRUE.
	*res = (type == கோவை_ISNOT);
    }
    இல்லையெனில் எனில் (((tv1->v_type == VAR_SPECIAL && tv1->vval.v_எண் == VVAL_NULL)
		|| (tv2->v_type == VAR_SPECIAL
					   && tv2->vval.v_எண் == VVAL_NULL))
	    && tv1->v_type != tv2->v_type
	    && (type == கோவை_EQUAL || type == கோவை_NEQUAL))
    {
	n1 = typval_compare_null(tv1, tv2);
	எனில் (n1 == MAYBE)
	    திரும்பு FAIL;
	எனில் (type == கோவை_NEQUAL)
	    n1 = !n1;
	*res = n1;
    }
    இல்லையெனில் எனில் (tv1->v_type == VAR_BLOB || tv2->v_type == VAR_BLOB)
    {
	எனில் (typval_compare_blob(tv1, tv2, type, res) == FAIL)
	    திரும்பு FAIL;
    }
    இல்லையெனில் எனில் (tv1->v_type == VAR_LIST || tv2->v_type == VAR_LIST)
    {
	எனில் (typval_compare_list(tv1, tv2, type, ic, res) == FAIL)
	    திரும்பு FAIL;
    }
    இல்லையெனில் எனில் (tv1->v_type == VAR_TUPLE || tv2->v_type == VAR_TUPLE)
    {
	எனில் (typval_compare_tuple(tv1, tv2, type, ic, res) == FAIL)
	    திரும்பு FAIL;
    }
    இல்லையெனில் எனில் (tv1->v_type == VAR_OBJECT || tv2->v_type == VAR_OBJECT)
    {
	எனில் (typval_compare_object(tv1, tv2, type, ic, res) == FAIL)
	    திரும்பு FAIL;
    }
    இல்லையெனில் எனில் (tv1->v_type == VAR_DICT || tv2->v_type == VAR_DICT)
    {
	எனில் (typval_compare_dict(tv1, tv2, type, ic, res) == FAIL)
	    திரும்பு FAIL;
    }
    இல்லையெனில் எனில் (tv1->v_type == VAR_FUNC || tv2->v_type == VAR_FUNC
	|| tv1->v_type == VAR_PARTIAL || tv2->v_type == VAR_PARTIAL)
    {
	எனில் (typval_compare_func(tv1, tv2, type, ic, res) == FAIL)
	    திரும்பு FAIL;
    }

    // If one of the two variables is a float, compare as a float.
    // When using "=~" or "!~", always compare as string.
    இல்லையெனில் எனில் ((tv1->v_type == VAR_FLOAT || tv2->v_type == VAR_FLOAT)
	    && type != கோவை_MATCH && type != கோவை_NOMATCH)
    {
	float_T f1, f2;
	முழு	error = FALSE;

	f1 = tv_get_float_chk(tv1, &error);
	எனில் (!error)
	    f2 = tv_get_float_chk(tv2, &error);
	எனில் (error)
	    திரும்பு FAIL;
	n1 = FALSE;
	தேர்வு (type)
	{
	    நிலை கோவை_IS:
	    நிலை கோவை_EQUAL:    n1 = (f1 == f2); நிறுத்து;
	    நிலை கோவை_ISNOT:
	    நிலை கோவை_NEQUAL:   n1 = (f1 != f2); நிறுத்து;
	    நிலை கோவை_GREATER:  n1 = (f1 > f2); நிறுத்து;
	    நிலை கோவை_GEQUAL:   n1 = (f1 >= f2); நிறுத்து;
	    நிலை கோவை_SMALLER:  n1 = (f1 < f2); நிறுத்து;
	    நிலை கோவை_SEQUAL:   n1 = (f1 <= f2); நிறுத்து;
	    நிலை கோவை_UNKNOWN:
	    நிலை கோவை_MATCH:
	    இயல்பு:  நிறுத்து;  // avoid gcc warning
	}
	*res = n1;
    }

    // If one of the two variables is a number, compare as a number.
    // When using "=~" or "!~", always compare as string.
    இல்லையெனில் எனில் ((tv1->v_type == VAR_எண் || tv2->v_type == VAR_எண்)
	    && type != கோவை_MATCH && type != கோவை_NOMATCH)
    {
	முழு error = FALSE;

	n1 = tv_get_எண்_chk(tv1, &error);
	எனில் (!error)
	    n2 = tv_get_எண்_chk(tv2, &error);
	எனில் (error)
	    திரும்பு FAIL;
	தேர்வு (type)
	{
	    நிலை கோவை_IS:
	    நிலை கோவை_EQUAL:    n1 = (n1 == n2); நிறுத்து;
	    நிலை கோவை_ISNOT:
	    நிலை கோவை_NEQUAL:   n1 = (n1 != n2); நிறுத்து;
	    நிலை கோவை_GREATER:  n1 = (n1 > n2); நிறுத்து;
	    நிலை கோவை_GEQUAL:   n1 = (n1 >= n2); நிறுத்து;
	    நிலை கோவை_SMALLER:  n1 = (n1 < n2); நிறுத்து;
	    நிலை கோவை_SEQUAL:   n1 = (n1 <= n2); நிறுத்து;
	    நிலை கோவை_UNKNOWN:
	    நிலை கோவை_MATCH:
	    இயல்பு:  நிறுத்து;  // avoid gcc warning
	}
	*res = n1;
    }
    இல்லையெனில் எனில் (in_vim9script() && (tv1->v_type == VAR_BOOL
				    || tv2->v_type == VAR_BOOL
				    || (tv1->v_type == VAR_SPECIAL
					      && tv2->v_type == VAR_SPECIAL)))
    {
	எனில் (tv1->v_type != tv2->v_type)
	{
	    semsg(_(e_cannot_compare_str_with_str),
		       vartype_name(tv1->v_type), vartype_name(tv2->v_type));
	    திரும்பு FAIL;
	}
	n1 = tv1->vval.v_எண்;
	n2 = tv2->vval.v_எண்;
	தேர்வு (type)
	{
	    நிலை கோவை_IS:
	    நிலை கோவை_EQUAL:    n1 = (n1 == n2); நிறுத்து;
	    நிலை கோவை_ISNOT:
	    நிலை கோவை_NEQUAL:   n1 = (n1 != n2); நிறுத்து;
	    இயல்பு:
		semsg(_(e_invalid_operation_for_str),
						   vartype_name(tv1->v_type));
		திரும்பு FAIL;
	}
	*res = n1;
    }
#ifdef FEAT_JOB_CHANNEL
    இல்லையெனில் எனில் (tv1->v_type == tv2->v_type
	    && (tv1->v_type == VAR_CHANNEL || tv1->v_type == VAR_JOB)
	    && (type == கோவை_NEQUAL || type == கோவை_EQUAL))
    {
	எனில் (tv1->v_type == VAR_CHANNEL)
	    n1 = tv1->vval.v_channel == tv2->vval.v_channel;
	இல்லையெனில்
	    n1 = tv1->vval.v_job == tv2->vval.v_job;
	எனில் (type == கோவை_NEQUAL)
	    n1 = !n1;
	*res = n1;
    }
#endif
    இல்லையெனில்
    {
	எனில் (typval_compare_string(tv1, tv2, type, ic, res) == FAIL)
	    திரும்பு FAIL;
    }

    திரும்பு OK;
}

/*
 * Compare "tv1" and "tv2".
 * Put the result in "tv1".  Caller should clear "tv2".
 */
    முழு
typval_compare(
    typval_T	*tv1,	// first operand
    typval_T	*tv2,	// second operand
    exprtype_T	type,   // operator
    முழு		ic)     // ignore case
{
    முழு res;

    எனில் (typval_compare2(tv1, tv2, type, ic, &res) == FAIL)
    {
	clear_tv(tv1);
	திரும்பு FAIL;
    }

    clear_tv(tv1);
    எனில் (in_vim9script())
    {
	tv1->v_type = VAR_BOOL;
	tv1->vval.v_எண் = res ? VVAL_TRUE : VVAL_FALSE;
    }
    இல்லையெனில்
    {
	tv1->v_type = VAR_எண்;
	tv1->vval.v_எண் = res;
    }

    திரும்பு OK;
}

/*
 * Compare "tv1" to "tv2" as lists according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    முழு
typval_compare_list(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	முழு	    ic,
	முழு	    *res)
{
    முழு	    val = 0;

    எனில் (type == கோவை_IS || type == கோவை_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
				      && tv1->vval.v_list == tv2->vval.v_list);
	எனில் (type == கோவை_ISNOT)
	    val = !val;
    }
    இல்லையெனில் எனில் (tv1->v_type != tv2->v_type
	    || (type != கோவை_EQUAL && type != கோவை_NEQUAL))
    {
	எனில் (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_list_with_list));
	இல்லையெனில்
	    emsg(_(e_invalid_operation_for_list));
	திரும்பு FAIL;
    }
    இல்லையெனில்
    {
	val = list_equal(tv1->vval.v_list, tv2->vval.v_list, ic);
	எனில் (type == கோவை_NEQUAL)
	    val = !val;
    }
    *res = val;
    திரும்பு OK;
}

/*
 * Compare "tv1" to "tv2" as tuples according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    முழு
typval_compare_tuple(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	முழு	    ic,
	முழு	    *res)
{
    முழு	    val = 0;

    எனில் (type == கோவை_IS || type == கோவை_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
			      && tv1->vval.v_tuple == tv2->vval.v_tuple);
	எனில் (type == கோவை_ISNOT)
	    val = !val;
    }
    இல்லையெனில் எனில் (tv1->v_type != tv2->v_type
	    || (type != கோவை_EQUAL && type != கோவை_NEQUAL))
    {
	எனில் (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_tuple_with_tuple));
	இல்லையெனில்
	    emsg(_(e_invalid_operation_for_tuple));
	திரும்பு FAIL;
    }
    இல்லையெனில்
    {
	val = tuple_equal(tv1->vval.v_tuple, tv2->vval.v_tuple, ic);
	எனில் (type == கோவை_NEQUAL)
	    val = !val;
    }
    *res = val;
    திரும்பு OK;
}

/*
 * Compare v:null with another type.  Return TRUE if the value is NULL.
 */
    முழு
typval_compare_null(typval_T *tv1, typval_T *tv2)
{
    எனில் ((tv1->v_type == VAR_SPECIAL && tv1->vval.v_எண் == VVAL_NULL)
	    || (tv2->v_type == VAR_SPECIAL && tv2->vval.v_எண் == VVAL_NULL))
    {
	typval_T	*tv = tv1->v_type == VAR_SPECIAL ? tv2 : tv1;

	தேர்வு (tv->v_type)
	{
	    நிலை VAR_BLOB: திரும்பு tv->vval.v_blob == வெற்று;
#ifdef FEAT_JOB_CHANNEL
	    நிலை VAR_CHANNEL: திரும்பு tv->vval.v_channel == வெற்று;
#endif
	    // TODO: null_class handling
	    // case VAR_CLASS: return tv->vval.v_class == NULL;
	    நிலை VAR_DICT: திரும்பு tv->vval.v_dict == வெற்று;
	    நிலை VAR_FUNC: திரும்பு tv->vval.v_string == வெற்று;
#ifdef FEAT_JOB_CHANNEL
	    நிலை VAR_JOB: திரும்பு tv->vval.v_job == வெற்று;
#endif
	    நிலை VAR_LIST: திரும்பு tv->vval.v_list == வெற்று;
	    நிலை VAR_TUPLE: திரும்பு tv->vval.v_tuple == வெற்று;
	    நிலை VAR_OBJECT: திரும்பு tv->vval.v_object == வெற்று;
	    நிலை VAR_PARTIAL: திரும்பு tv->vval.v_partial == வெற்று;
	    நிலை VAR_STRING: திரும்பு tv->vval.v_string == வெற்று;

	    நிலை VAR_எண்: எனில் (!in_vim9script())
				 திரும்பு tv->vval.v_எண் == 0;
			     நிறுத்து;
	    நிலை VAR_FLOAT: எனில் (!in_vim9script())
				 திரும்பு tv->vval.v_float == 0.0;
			     நிறுத்து;
	    நிலை VAR_TYPEALIAS: திரும்பு tv->vval.v_typealias == வெற்று;
	    இயல்பு: நிறுத்து;
	}
    }
    // although comparing null with number, float or bool is not very useful
    // we won't give an error
    திரும்பு FALSE;
}

/*
 * Compare "tv1" to "tv2" as blobs according to "type".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    முழு
typval_compare_blob(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	முழு	    *res)
{
    முழு	    val = 0;

    எனில் (type == கோவை_IS || type == கோவை_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
			&& tv1->vval.v_blob == tv2->vval.v_blob);
	எனில் (type == கோவை_ISNOT)
	    val = !val;
    }
    இல்லையெனில் எனில் (tv1->v_type != tv2->v_type
	    || (type != கோவை_EQUAL && type != கோவை_NEQUAL))
    {
	எனில் (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_blob_with_blob));
	இல்லையெனில்
	    emsg(_(e_invalid_operation_for_blob));
	திரும்பு FAIL;
    }
    இல்லையெனில்
    {
	val = blob_equal(tv1->vval.v_blob, tv2->vval.v_blob);
	எனில் (type == கோவை_NEQUAL)
	    val = !val;
    }
    *res = val;
    திரும்பு OK;
}

/*
 * Compare "tv1" to "tv2" as objects according to "type".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    முழு
typval_compare_object(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	முழு	    ic,
	முழு	    *res)
{
    முழு res_match = type == கோவை_EQUAL || type == கோவை_IS ? TRUE : FALSE;

    எனில் (tv1->vval.v_object == வெற்று && tv2->vval.v_object == வெற்று)
    {
	*res = res_match;
	திரும்பு OK;
    }
    எனில் (tv1->vval.v_object == வெற்று || tv2->vval.v_object == வெற்று)
    {
	*res = !res_match;
	திரும்பு OK;
    }

    object_T *obj1 = tv1->vval.v_object;
    object_T *obj2 = tv2->vval.v_object;
    எனில் (type == கோவை_IS || type == கோவை_ISNOT)
    {
	*res = obj1 == obj2 ? res_match : !res_match;
	திரும்பு OK;
    }
    இல்லையெனில் எனில் (tv1->v_type != tv2->v_type)
    {
	emsg(_(e_can_only_compare_object_with_object));
	திரும்பு FAIL;
    }

    *res = object_equal(obj1, obj2, ic) ? res_match : !res_match;
    திரும்பு OK;
}

/*
 * Compare "tv1" to "tv2" as dictionaries according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    முழு
typval_compare_dict(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	முழு	    ic,
	முழு	    *res)
{
    முழு	    val;

    எனில் (type == கோவை_IS || type == கோவை_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
			&& tv1->vval.v_dict == tv2->vval.v_dict);
	எனில் (type == கோவை_ISNOT)
	    val = !val;
    }
    இல்லையெனில் எனில் (tv1->v_type != tv2->v_type
		|| (type != கோவை_EQUAL && type != கோவை_NEQUAL))
    {
	எனில் (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_dictionary_with_dictionary));
	இல்லையெனில்
	    emsg(_(e_invalid_operation_for_dictionary));
	திரும்பு FAIL;
    }
    இல்லையெனில்
    {
	val = dict_equal(tv1->vval.v_dict, tv2->vval.v_dict, ic);
	எனில் (type == கோவை_NEQUAL)
	    val = !val;
    }
    *res = val;
    திரும்பு OK;
}

/*
 * Compare "tv1" to "tv2" as funcrefs according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    முழு
typval_compare_func(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	முழு	    ic,
	முழு	    *res)
{
    முழு	    val = 0;

    எனில் (type != கோவை_EQUAL && type != கோவை_NEQUAL
	    && type != கோவை_IS && type != கோவை_ISNOT)
    {
	emsg(_(e_invalid_operation_for_funcrefs));
	திரும்பு FAIL;
    }
    எனில் ((tv1->v_type == VAR_PARTIAL && tv1->vval.v_partial == வெற்று)
	    || (tv2->v_type == VAR_PARTIAL && tv2->vval.v_partial == வெற்று))
	// When both partials are NULL, then they are equal.
	// Otherwise they are not equal.
	val = (tv1->vval.v_partial == tv2->vval.v_partial);
    இல்லையெனில் எனில் (type == கோவை_IS || type == கோவை_ISNOT)
    {
	எனில் (tv1->v_type == VAR_FUNC && tv2->v_type == VAR_FUNC)
	    // strings are considered the same if their value is
	    // the same
	    val = tv_equal(tv1, tv2, ic);
	இல்லையெனில் எனில் (tv1->v_type == VAR_PARTIAL && tv2->v_type == VAR_PARTIAL)
	    val = (tv1->vval.v_partial == tv2->vval.v_partial);
	இல்லையெனில்
	    val = FALSE;
    }
    இல்லையெனில்
	val = tv_equal(tv1, tv2, ic);
    எனில் (type == கோவை_NEQUAL || type == கோவை_ISNOT)
	val = !val;
    *res = val;
    திரும்பு OK;
}

/*
 * Compare "tv1" to "tv2" as strings according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    முழு
typval_compare_string(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	முழு	    ic,
	முழு	    *res)
{
    முழு		i = 0;
    முழு		val = FALSE;
    char_u	*s1, *s2;
    char_u	buf1[NUMBUFLEN], buf2[NUMBUFLEN];

    எனில் (in_vim9script()
	  && ((tv1->v_type != VAR_STRING && tv1->v_type != VAR_SPECIAL)
	   || (tv2->v_type != VAR_STRING && tv2->v_type != VAR_SPECIAL)))
    {
	semsg(_(e_cannot_compare_str_with_str),
		   vartype_name(tv1->v_type), vartype_name(tv2->v_type));
	திரும்பு FAIL;
    }
    s1 = tv_get_string_buf(tv1, buf1);
    s2 = tv_get_string_buf(tv2, buf2);
    எனில் (type != கோவை_MATCH && type != கோவை_NOMATCH)
	i = ic ? MB_STRICMP(s1, s2) : STRCMP(s1, s2);
    தேர்வு (type)
    {
	நிலை கோவை_IS:	    எனில் (in_vim9script())
			    {
				// Really check it is the same string, not just
				// the same value.
				val = tv1->vval.v_string == tv2->vval.v_string;
				நிறுத்து;
			    }
			    // FALLTHROUGH
	நிலை கோவை_EQUAL:    val = (i == 0); நிறுத்து;
	நிலை கோவை_ISNOT:    எனில் (in_vim9script())
			    {
				// Really check it is not the same string, not
				// just a different value.
				val = tv1->vval.v_string != tv2->vval.v_string;
				நிறுத்து;
			    }
			    // FALLTHROUGH
	நிலை கோவை_NEQUAL:   val = (i != 0); நிறுத்து;
	நிலை கோவை_GREATER:  val = (i > 0); நிறுத்து;
	நிலை கோவை_GEQUAL:   val = (i >= 0); நிறுத்து;
	நிலை கோவை_SMALLER:  val = (i < 0); நிறுத்து;
	நிலை கோவை_SEQUAL:   val = (i <= 0); நிறுத்து;

	நிலை கோவை_MATCH:
	நிலை கோவை_NOMATCH:
		val = pattern_match(s2, s1, ic);
		எனில் (type == கோவை_NOMATCH)
		    val = !val;
		நிறுத்து;

	இயல்பு:  நிறுத்து;  // avoid gcc warning
    }
    *res = val;
    திரும்பு OK;
}
/*
 * Convert any type to a string, never give an error.
 * When "quotes" is TRUE add quotes to a string.
 * Returns an allocated string.
 */
    char_u *
typval_tostring(typval_T *arg, முழு quotes)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    char_u	*ret = வெற்று;

    எனில் (arg == வெற்று)
	திரும்பு vim_strsave((char_u *)"(does not exist)");
    எனில் (!quotes && arg->v_type == VAR_STRING)
    {
	ret = vim_strsave(arg->vval.v_string == வெற்று ? (char_u *)""
							 : arg->vval.v_string);
    }
    இல்லையெனில்
    {
	ret = tv2string(arg, &tofree, numbuf, 0);
	// Make a copy if we have a value but it's not in allocated memory.
	எனில் (ret != வெற்று && tofree == வெற்று)
	    ret = vim_strsave(ret);
    }
    திரும்பு ret;
}

/*
 * Return TRUE if typeval "tv" is locked: Either that value is locked itself
 * or it refers to a List or Dictionary that is locked.
 */
    முழு
tv_islocked(typval_T *tv)
{
    திரும்பு (tv->v_lock & VAR_LOCKED)
	|| (tv->v_type == VAR_LIST
		&& tv->vval.v_list != வெற்று
		&& (tv->vval.v_list->lv_lock & VAR_LOCKED))
	|| (tv->v_type == VAR_TUPLE
		&& tv->vval.v_tuple != வெற்று
		&& (tv->vval.v_tuple->tv_lock & VAR_LOCKED))
	|| (tv->v_type == VAR_DICT
		&& tv->vval.v_dict != வெற்று
		&& (tv->vval.v_dict->dv_lock & VAR_LOCKED));
}

    நிலையான முழு
func_equal(
    typval_T *tv1,
    typval_T *tv2,
    முழு	     ic)	    // ignore case
{
    char_u	*s1, *s2;
    dict_T	*d1, *d2;
    முழு		a1, a2;
    முழு		i;

    // empty and NULL function name considered the same
    s1 = tv1->v_type == VAR_FUNC ? tv1->vval.v_string
					   : partial_name(tv1->vval.v_partial);
    எனில் (s1 != வெற்று && *s1 == NUL)
	s1 = வெற்று;
    s2 = tv2->v_type == VAR_FUNC ? tv2->vval.v_string
					   : partial_name(tv2->vval.v_partial);
    எனில் (s2 != வெற்று && *s2 == NUL)
	s2 = வெற்று;
    எனில் (s1 == வெற்று || s2 == வெற்று)
    {
	எனில் (s1 != s2)
	    திரும்பு FALSE;
    }
    இல்லையெனில் எனில் (STRCMP(s1, s2) != 0)
	திரும்பு FALSE;

    // empty dict and NULL dict is different
    d1 = tv1->v_type == VAR_FUNC ? வெற்று : tv1->vval.v_partial->pt_dict;
    d2 = tv2->v_type == VAR_FUNC ? வெற்று : tv2->vval.v_partial->pt_dict;
    எனில் (d1 == வெற்று || d2 == வெற்று)
    {
	எனில் (d1 != d2)
	    திரும்பு FALSE;
    }
    இல்லையெனில் எனில் (!dict_equal(d1, d2, ic))
	திரும்பு FALSE;

    // empty list and no list considered the same
    a1 = tv1->v_type == VAR_FUNC ? 0 : tv1->vval.v_partial->pt_argc;
    a2 = tv2->v_type == VAR_FUNC ? 0 : tv2->vval.v_partial->pt_argc;
    எனில் (a1 != a2)
	திரும்பு FALSE;
    க்கு (i = 0; i < a1; ++i)
	எனில் (!tv_equal(tv1->vval.v_partial->pt_argv + i,
		      tv2->vval.v_partial->pt_argv + i, ic))
	    திரும்பு FALSE;

    திரும்பு TRUE;
}

/*
 * Return TRUE if "tv1" and "tv2" have the same value.
 * Compares the items just like "==" would compare them, but strings and
 * numbers are different.  Floats and numbers are also different.
 */
    முழு
tv_equal(
    typval_T *tv1,
    typval_T *tv2,
    முழு	     ic)	    // ignore case
{
    char_u	buf1[NUMBUFLEN], buf2[NUMBUFLEN];
    char_u	*s1, *s2;
    நிலையான முழு  recursive_cnt = 0;	    // catch recursive loops
    முழு		r;
    நிலையான முழு	tv_equal_recurse_limit;

    // Catch lists and dicts that have an endless loop by limiting
    // recursiveness to a limit.  We guess they are equal then.
    // A fixed limit has the problem of still taking an awful long time.
    // Reduce the limit every time running into it. That should work fine for
    // deeply linked structures that are not recursively linked and catch
    // recursiveness quickly.
    எனில் (recursive_cnt == 0)
	tv_equal_recurse_limit = 1000;
    எனில் (recursive_cnt >= tv_equal_recurse_limit)
    {
	--tv_equal_recurse_limit;
	திரும்பு TRUE;
    }

    // For VAR_FUNC and VAR_PARTIAL compare the function name, bound dict and
    // arguments.
    எனில் ((tv1->v_type == VAR_FUNC
		|| (tv1->v_type == VAR_PARTIAL && tv1->vval.v_partial != வெற்று))
	    && (tv2->v_type == VAR_FUNC
		|| (tv2->v_type == VAR_PARTIAL && tv2->vval.v_partial != வெற்று)))
    {
	++recursive_cnt;
	r = func_equal(tv1, tv2, ic);
	--recursive_cnt;
	திரும்பு r;
    }

    எனில் (tv1->v_type != tv2->v_type
	    && ((tv1->v_type != VAR_BOOL && tv1->v_type != VAR_SPECIAL)
		|| (tv2->v_type != VAR_BOOL && tv2->v_type != VAR_SPECIAL)))
	திரும்பு FALSE;

    தேர்வு (tv1->v_type)
    {
	நிலை VAR_LIST:
	    ++recursive_cnt;
	    r = list_equal(tv1->vval.v_list, tv2->vval.v_list, ic);
	    --recursive_cnt;
	    திரும்பு r;

	நிலை VAR_TUPLE:
	    ++recursive_cnt;
	    r = tuple_equal(tv1->vval.v_tuple, tv2->vval.v_tuple, ic);
	    --recursive_cnt;
	    திரும்பு r;

	நிலை VAR_DICT:
	    ++recursive_cnt;
	    r = dict_equal(tv1->vval.v_dict, tv2->vval.v_dict, ic);
	    --recursive_cnt;
	    திரும்பு r;

	நிலை VAR_BLOB:
	    திரும்பு blob_equal(tv1->vval.v_blob, tv2->vval.v_blob);

	நிலை VAR_எண்:
	நிலை VAR_BOOL:
	நிலை VAR_SPECIAL:
	    திரும்பு tv1->vval.v_எண் == tv2->vval.v_எண்;

	நிலை VAR_STRING:
	    s1 = tv_get_string_buf(tv1, buf1);
	    s2 = tv_get_string_buf(tv2, buf2);
	    திரும்பு ((ic ? MB_STRICMP(s1, s2) : STRCMP(s1, s2)) == 0);

	நிலை VAR_FLOAT:
	    திரும்பு tv1->vval.v_float == tv2->vval.v_float;
	நிலை VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    திரும்பு tv1->vval.v_job == tv2->vval.v_job;
#endif
	நிலை VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    திரும்பு tv1->vval.v_channel == tv2->vval.v_channel;
#endif
	நிலை VAR_INSTR:
	    திரும்பு tv1->vval.v_instr == tv2->vval.v_instr;

	நிலை VAR_CLASS:
	    // A class only exists once, equality is identity.
	    திரும்பு tv1->vval.v_class == tv2->vval.v_class;

	நிலை VAR_OBJECT:
	    ++recursive_cnt;
	    r = object_equal(tv1->vval.v_object, tv2->vval.v_object, ic);
	    --recursive_cnt;
	    திரும்பு r;

	நிலை VAR_PARTIAL:
	    திரும்பு tv1->vval.v_partial == tv2->vval.v_partial;

	நிலை VAR_FUNC:
	    திரும்பு tv1->vval.v_string == tv2->vval.v_string;

	நிலை VAR_TYPEALIAS:
	    திரும்பு tv1->vval.v_typealias == tv2->vval.v_typealias;

	நிலை VAR_UNKNOWN:
	நிலை VAR_ANY:
	நிலை VAR_VOID:
	    நிறுத்து;
    }

    // VAR_UNKNOWN can be the result of a invalid expression, let's say it
    // does not equal anything, not even itself.
    திரும்பு FALSE;
}

/*
 * Get an option value.
 * "arg" points to the '&' or '+' before the option name.
 * "arg" is advanced to character after the option name.
 * Return OK or FAIL.
 */
    முழு
eval_option(
    char_u	**arg,
    typval_T	*rettv,	// when NULL, only check if option exists
    முழு		evaluate)
{
    char_u	*option_end;
    long	numval;
    char_u	*stringval;
    getoption_T	opt_type;
    முழு		c;
    முழு		working = (**arg == '+');    // has("+option")
    முழு		ret = OK;
    முழு		scope;

    // Isolate the option name and find its value.
    option_end = find_option_end(arg, &scope);
    எனில் (option_end == வெற்று)
    {
	எனில் (rettv != வெற்று)
	    semsg(_(e_option_name_missing_str), *arg);
	திரும்பு FAIL;
    }

    எனில் (!evaluate)
    {
	*arg = option_end;
	திரும்பு OK;
    }

    c = *option_end;
    *option_end = NUL;
    opt_type = get_option_மதிப்பு(*arg, &numval,
			       rettv == வெற்று ? வெற்று : &stringval, வெற்று, scope);

    எனில் (opt_type == gov_unknown)
    {
	எனில் (rettv != வெற்று)
	    semsg(_(e_unknown_option_str), *arg);
	ret = FAIL;
    }
    இல்லையெனில் எனில் (rettv != வெற்று)
    {
	rettv->v_lock = 0;
	எனில் (opt_type == gov_hidden_string)
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = வெற்று;
	}
	இல்லையெனில் எனில் (opt_type == gov_hidden_bool || opt_type == gov_hidden_எண்)
	{
	    rettv->v_type = in_vim9script() && opt_type == gov_hidden_bool
						       ? VAR_BOOL : VAR_எண்;
	    rettv->vval.v_எண் = 0;
	}
	இல்லையெனில் எனில் (opt_type == gov_bool || opt_type == gov_எண்)
	{
	    எனில் (in_vim9script() && opt_type == gov_bool)
	    {
		rettv->v_type = VAR_BOOL;
		rettv->vval.v_எண் = numval ? VVAL_TRUE : VVAL_FALSE;
	    }
	    இல்லையெனில்
	    {
		rettv->v_type = VAR_எண்;
		rettv->vval.v_எண் = numval;
	    }
	}
	இல்லையெனில்				// string option
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = stringval;
	}
    }
    இல்லையெனில் எனில் (working && (opt_type == gov_hidden_bool
			|| opt_type == gov_hidden_எண்
			|| opt_type == gov_hidden_string))
	ret = FAIL;

    *option_end = c;		    // put back for error messages
    *arg = option_end;

    திரும்பு ret;
}

/*
 * Allocate a variable for a number constant.  Also deals with "0z" for blob.
 * Return OK or FAIL.
 */
    முழு
eval_எண்(
	char_u	    **arg,
	typval_T    *rettv,
	முழு	    evaluate,
	முழு	    want_string)
{
    முழு		len;
    முழு		skip_quotes = !in_old_script(4);
    char_u	*p;
    முழு		get_float = FALSE;

    // We accept a float when the format matches
    // "[0-9]\+\.[0-9]\+\([eE][+-]\?[0-9]\+\)\?".  This is very
    // strict to avoid backwards compatibility problems.
    // With script version 2 and later the leading digit can be
    // omitted.
    // Don't look for a float after the "." operator, so that
    // ":let vers = 1.2.3" doesn't fail.
    எனில் (**arg == '.')
	p = *arg;
    இல்லையெனில்
    {
	p = *arg + 1;
	எனில் (skip_quotes)
	    க்கு (;;)
	    {
		எனில் (*p == '\'')
		    ++p;
		எனில் (!vim_isdigit(*p))
		    நிறுத்து;
		p = skipdigits(p);
	    }
	இல்லையெனில்
	    p = skipdigits(p);
    }
    எனில் (!want_string && p[0] == '.' && vim_isdigit(p[1]))
    {
	get_float = TRUE;
	p = skipdigits(p + 2);
	எனில் (*p == 'e' || *p == 'E')
	{
	    ++p;
	    எனில் (*p == '-' || *p == '+')
		++p;
	    எனில் (!vim_isdigit(*p))
		get_float = FALSE;
	    இல்லையெனில்
		p = skipdigits(p + 1);
	}
	எனில் (ASCII_ISALPHA(*p) || *p == '.')
	    get_float = FALSE;
    }
    எனில் (get_float)
    {
	float_T	f;

	*arg += string2float(*arg, &f, skip_quotes);
	எனில் (evaluate)
	{
	    rettv->v_type = VAR_FLOAT;
	    rettv->vval.v_float = f;
	}
    }
    இல்லையெனில்
    எனில் (**arg == '0' && ((*arg)[1] == 'z' || (*arg)[1] == 'Z'))
    {
	char_u  *bp;
	blob_T  *blob = வெற்று;  // init for gcc

	// Blob constant: 0z0123456789abcdef
	எனில் (evaluate)
	    blob = blob_alloc();
	க்கு (bp = *arg + 2; vim_isxdigit(bp[0]); bp += 2)
	{
	    எனில் (!vim_isxdigit(bp[1]))
	    {
		எனில் (blob != வெற்று)
		{
		    emsg(_(e_blob_literal_should_have_an_even_எண்_of_hex_characters));
		    ga_clear(&blob->bv_ga);
		    VIM_CLEAR(blob);
		}
		திரும்பு FAIL;
	    }
	    எனில் (blob != வெற்று)
		ga_append(&blob->bv_ga,
			     (hex2nr(*bp) << 4) + hex2nr(*(bp+1)));
	    எனில் (bp[2] == '.' && vim_isxdigit(bp[3]))
		++bp;
	}
	எனில் (blob != வெற்று)
	    rettv_blob_set(rettv, blob);
	*arg = bp;
    }
    இல்லையெனில்
    {
	varnumber_T	n;

	// decimal, hex or octal number
	vim_str2nr(*arg, வெற்று, &len, skip_quotes
		      ? STR2NR_NO_OCT + STR2NR_QUOTE
		      : STR2NR_ALL, &n, வெற்று, 0, TRUE, வெற்று);
	எனில் (len == 0)
	{
	    எனில் (evaluate)
		semsg(_(e_invalid_expression_str), *arg);
	    திரும்பு FAIL;
	}
	*arg += len;
	எனில் (evaluate)
	{
	    rettv->v_type = VAR_எண்;
	    rettv->vval.v_எண் = n;
	}
    }
    திரும்பு OK;
}

/*
 * Evaluate a string constant and put the result in "rettv".
 * "*arg" points to the double quote or to after it when "interpolate" is TRUE.
 * When "interpolate" is TRUE reduce "{{" to "{", reduce "}}" to "}" and stop
 * at a single "{".
 * Return OK or FAIL.
 */
    முழு
eval_string(char_u **arg, typval_T *rettv, முழு evaluate, முழு interpolate)
{
    char_u	*p;
    char_u	*end;
    முழு		extra = interpolate ? 1 : 0;
    முழு		off = interpolate ? 0 : 1;
    முழு		len;

    // Find the end of the string, skipping backslashed characters.
    க்கு (p = *arg + off; *p != NUL && *p != '"'; MB_PTR_ADV(p))
    {
	எனில் (*p == '\\' && p[1] != NUL)
	{
	    ++p;
	    // A "\<x>" form occupies at least 4 characters, and produces up
	    // to 9 characters (6 for the char and 3 for a modifier):
	    // reserve space for 5 extra.
	    எனில் (*p == '<')
	    {
		முழு		modifiers = 0;
		முழு		flags = FSK_KEYCODE | FSK_IN_STRING;

		extra += 5;

		// Skip to the '>' to avoid using '{' inside for string
		// interpolation.
		எனில் (p[1] != '*')
		    flags |= FSK_SIMPLIFY;
		எனில் (find_special_சாவி(&p, &modifiers, flags, வெற்று) != 0)
		    --p;  // leave "p" on the ">"
	    }
	}
	இல்லையெனில் எனில் (interpolate && (*p == '{' || *p == '}'))
	{
	    எனில் (*p == '{' && p[1] != '{') // start of expression
		நிறுத்து;
	    ++p;
	    எனில் (p[-1] == '}' && *p != '}') // single '}' is an error
	    {
		semsg(_(e_stray_closing_curly_str), *arg);
		திரும்பு FAIL;
	    }
	    --extra;  // "{{" becomes "{", "}}" becomes "}"
	}
    }

    எனில் (*p != '"' && !(interpolate && *p == '{'))
    {
	semsg(_(e_missing_double_quote_str), *arg);
	திரும்பு FAIL;
    }

    // If only parsing, set *arg and return here
    எனில் (!evaluate)
    {
	*arg = p + off;
	திரும்பு OK;
    }

    // Copy the string into allocated memory, handling backslashed
    // characters.
    rettv->v_type = VAR_STRING;
    len = (முழு)(p - *arg + extra);
    rettv->vval.v_string = alloc(len);
    எனில் (rettv->vval.v_string == வெற்று)
	திரும்பு FAIL;
    end = rettv->vval.v_string;

    க்கு (p = *arg + off; *p != NUL && *p != '"'; )
    {
	எனில் (*p == '\\')
	{
	    தேர்வு (*++p)
	    {
		நிலை 'b': *end++ = BS; ++p; நிறுத்து;
		நிலை 'e': *end++ = ESC; ++p; நிறுத்து;
		நிலை 'f': *end++ = FF; ++p; நிறுத்து;
		நிலை 'n': *end++ = NL; ++p; நிறுத்து;
		நிலை 'r': *end++ = CAR; ++p; நிறுத்து;
		நிலை 't': *end++ = TAB; ++p; நிறுத்து;

		நிலை 'X': // hex: "\x1", "\x12"
		நிலை 'x':
		நிலை 'u': // Unicode: "\u0023"
		நிலை 'U':
			  எனில் (vim_isxdigit(p[1]))
			  {
			      முழு	n, nr;
			      முழு	c = SAFE_toupper(*p);

			      எனில் (c == 'X')
				  n = 2;
			      இல்லையெனில் எனில் (*p == 'u')
				  n = 4;
			      இல்லையெனில்
				  n = 8;
			      nr = 0;
			      போது (--n >= 0 && vim_isxdigit(p[1]))
			      {
				  ++p;
				  nr = (nr << 4) + hex2nr(*p);
			      }
			      ++p;
			      // For "\u" store the number according to
			      // 'encoding'.
			      எனில் (c != 'X')
				  end += (*mb_char2bytes)(nr, end);
			      இல்லையெனில்
				  *end++ = nr;
			  }
			  நிறுத்து;

			  // octal: "\1", "\12", "\123"
		நிலை '0':
		நிலை '1':
		நிலை '2':
		நிலை '3':
		நிலை '4':
		நிலை '5':
		நிலை '6':
		நிலை '7': *end = *p++ - '0';
			  எனில் (*p >= '0' && *p <= '7')
			  {
			      *end = (*end << 3) + *p++ - '0';
			      எனில் (*p >= '0' && *p <= '7')
				  *end = (*end << 3) + *p++ - '0';
			  }
			  ++end;
			  நிறுத்து;

			  // Special key, e.g.: "\<C-W>"
		நிலை '<':
			  {
			      முழு flags = FSK_KEYCODE | FSK_IN_STRING;

			      எனில் (p[1] != '*')
				  flags |= FSK_SIMPLIFY;
			      extra = trans_special(&p, end, flags, FALSE, வெற்று);
			      எனில் (extra != 0)
			      {
				  end += extra;
				  எனில் (end >= rettv->vval.v_string + len)
				      iemsg("eval_string() used more space than allocated");
				  நிறுத்து;
			      }
			  }
			  // FALLTHROUGH

		இயல்பு: MB_COPY_CHAR(p, end);
			  நிறுத்து;
	    }
	}
	இல்லையெனில்
	{
	    எனில் (interpolate && (*p == '{' || *p == '}'))
	    {
		எனில் (*p == '{' && p[1] != '{') // start of expression
		    நிறுத்து;
		++p;  // reduce "{{" to "{" and "}}" to "}"
	    }
	    MB_COPY_CHAR(p, end);
	}
    }
    *end = NUL;
    எனில் (*p == '"' && !interpolate)
	++p;
    *arg = p;

    திரும்பு OK;
}

/*
 * Allocate a variable for a 'str''ing' constant.
 * When "interpolate" is TRUE reduce "{{" to "{" and stop at a single "{".
 * Return OK when a "rettv" was set to the string.
 * Return FAIL on error, "rettv" is not set.
 */
    முழு
eval_lit_string(char_u **arg, typval_T *rettv, முழு evaluate, முழு interpolate)
{
    char_u	*p;
    char_u	*str;
    முழு		reduce = interpolate ? -1 : 0;
    முழு		off = interpolate ? 0 : 1;

    // Find the end of the string, skipping ''.
    க்கு (p = *arg + off; *p != NUL; MB_PTR_ADV(p))
    {
	எனில் (*p == '\'')
	{
	    எனில் (p[1] != '\'')
		நிறுத்து;
	    ++reduce;
	    ++p;
	}
	இல்லையெனில் எனில் (interpolate)
	{
	    எனில் (*p == '{')
	    {
		எனில் (p[1] != '{')
		    நிறுத்து;
		++p;
		++reduce;
	    }
	    இல்லையெனில் எனில் (*p == '}')
	    {
		++p;
		எனில் (*p != '}')
		{
		    semsg(_(e_stray_closing_curly_str), *arg);
		    திரும்பு FAIL;
		}
		++reduce;
	    }
	}
    }

    எனில் (*p != '\'' && !(interpolate && *p == '{'))
    {
	semsg(_(e_missing_single_quote_str), *arg);
	திரும்பு FAIL;
    }

    // If only parsing return after setting "*arg"
    எனில் (!evaluate)
    {
	*arg = p + off;
	திரும்பு OK;
    }

    // Copy the string into allocated memory, handling '' to ' reduction and
    // any expressions.
    str = alloc((p - *arg) - reduce);
    எனில் (str == வெற்று)
	திரும்பு FAIL;
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = str;

    க்கு (p = *arg + off; *p != NUL; )
    {
	எனில் (*p == '\'')
	{
	    எனில் (p[1] != '\'')
		நிறுத்து;
	    ++p;
	}
	இல்லையெனில் எனில் (interpolate && (*p == '{' || *p == '}'))
	{
	    எனில் (*p == '{' && p[1] != '{')
		நிறுத்து;
	    ++p;
	}
	MB_COPY_CHAR(p, str);
    }
    *str = NUL;
    *arg = p + off;

    திரும்பு OK;
}

/*
 * Evaluate a single or double quoted string possibly containing expressions.
 * "arg" points to the '$'.  The result is put in "rettv".
 * Returns OK or FAIL.
 */
    முழு
eval_interp_string(char_u **arg, typval_T *rettv, முழு evaluate)
{
    typval_T	tv;
    முழு		ret = OK;
    முழு		quote;
    garray_T	ga;
    char_u	*p;

    ga_init2(&ga, 1, 80);

    // *arg is on the '$' character, move it to the first string character.
    ++*arg;
    quote = **arg;
    ++*arg;

    க்கு (;;)
    {
	// Get the string up to the matching quote or to a single '{'.
	// "arg" is advanced to either the quote or the '{'.
	எனில் (quote == '"')
	    ret = eval_string(arg, &tv, evaluate, TRUE);
	இல்லையெனில்
	    ret = eval_lit_string(arg, &tv, evaluate, TRUE);
	எனில் (ret == FAIL)
	    நிறுத்து;
	எனில் (evaluate)
	{
	    ga_concat(&ga, tv.vval.v_string);
	    clear_tv(&tv);
	}

	எனில் (**arg != '{')
	{
	    // found terminating quote
	    ++*arg;
	    நிறுத்து;
	}
	p = eval_one_கோவை_in_str(*arg, &ga, evaluate);
	எனில் (p == வெற்று)
	{
	    ret = FAIL;
	    நிறுத்து;
	}
	*arg = p;
    }

    rettv->v_type = VAR_STRING;
    எனில் (ret == FAIL || !evaluate || ga_append(&ga, NUL) == FAIL)
    {
	ga_clear(&ga);
	rettv->vval.v_string = வெற்று;
	திரும்பு ret;
    }

    rettv->vval.v_string = ga.ga_data;
    திரும்பு OK;
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
    முழு		copyID)
{
    திரும்பு echo_string_core(tv, tofree, numbuf, copyID, FALSE, TRUE, FALSE);
}

/*
 * Get the value of an environment variable.
 * "arg" is pointing to the '$'.  It is advanced to after the name.
 * If the environment variable was not set, silently assume it is empty.
 * Return FAIL if the name is invalid.
 */
    முழு
eval_env_var(char_u **arg, typval_T *rettv, முழு evaluate)
{
    char_u	*string = வெற்று;
    முழு		len;
    முழு		cc;
    char_u	*name;
    முழு		mustfree = FALSE;

    ++*arg;
    name = *arg;
    len = get_env_len(arg);
    எனில் (evaluate)
    {
	எனில் (len == 0)
	    திரும்பு FAIL; // invalid empty name

	cc = name[len];
	name[len] = NUL;
	// first try vim_getenv(), fast for normal environment vars
	string = vim_getenv(name, &mustfree);
	எனில் (string != வெற்று && *string != NUL)
	{
	    எனில் (!mustfree)
		string = vim_strsave(string);
	}
	இல்லையெனில்
	{
	    எனில் (mustfree)
		vim_free(string);

	    // next try expanding things like $VIM and ${HOME}
	    string = expand_env_save(name - 1);
	    எனில் (string != வெற்று && *string == '$')
		VIM_CLEAR(string);
	}
	name[len] = cc;

	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = string;
	rettv->v_lock = 0;
    }

    திரும்பு OK;
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
    முழு		did_emsg_before = did_emsg;

    எனில் (argvars[0].v_type != VAR_STRING || !in_vim9script())
	lnum = (linenr_T)tv_get_எண்_chk(&argvars[0], வெற்று);
    எனில் (lnum <= 0 && did_emsg_before == did_emsg
					    && argvars[0].v_type != VAR_எண்)
    {
	முழு	fnum;
	pos_T	*fp;

	// no valid number, try using arg like line()
	fp = var2fpos(&argvars[0], TRUE, &fnum, FALSE);
	எனில் (fp != வெற்று)
	    lnum = fp->lnum;
    }
    திரும்பு lnum;
}

/*
 * Get the lnum from the first argument.
 * Also accepts "$", then "buf" is used.
 * Returns 0 on error.
 */
    linenr_T
tv_get_lnum_buf(typval_T *argvars, buf_T *buf)
{
    எனில் (argvars[0].v_type == VAR_STRING
	    && argvars[0].vval.v_string != வெற்று
	    && argvars[0].vval.v_string[0] == '$'
	    && argvars[0].vval.v_string[1] == NUL
	    && buf != வெற்று)
	திரும்பு buf->b_ml.ml_வரி_மொத்தம்;
    திரும்பு (linenr_T)tv_get_எண்_chk(&argvars[0], வெற்று);
}

/*
 * Get buffer by number or pattern.
 */
    buf_T *
tv_get_buf(typval_T *tv, முழு curtab_only)
{
    char_u	*name = tv->vval.v_string;
    buf_T	*buf;

    எனில் (tv->v_type == VAR_எண்)
	திரும்பு buflist_findnr((முழு)tv->vval.v_எண்);
    எனில் (tv->v_type != VAR_STRING)
	திரும்பு வெற்று;
    எனில் (name == வெற்று || *name == NUL)
	திரும்பு curbuf;
    எனில் (name[0] == '$' && name[1] == NUL)
	திரும்பு lastbuf;

    buf = buflist_find_by_name(name, curtab_only);

    // If not found, try expanding the name, like done for bufexists().
    எனில் (buf == வெற்று)
	buf = find_buffer(tv);

    திரும்பு buf;
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
    எனில் (buf == வெற்று
	    && tv->v_type != VAR_எண்
	    && tv->v_type != VAR_STRING)
	// issue errmsg for type error
	(வெறுமை)tv_get_எண்(tv);
    திரும்பு buf;
}

/*
 * Check that repeating "slen" items "n" times, does not
 * overflow the int used for the resulting length.
 */
    முழு
check_repeat_மொத்தம்(varnumber_T slen, varnumber_T n)
{
    // Use a division so that the multiplication cannot overflow.
    எனில் (n <= (varnumber_T)INT_MAX / slen)
	திரும்பு OK;

    எழுத்து	buf[NUMBUFLEN];

    vim_snprintf(buf, அளவு(buf), "%lld", (long long)n);
    semsg(_(e_val_too_large), buf);
    திரும்பு FAIL;
}

#endif // FEAT_EVAL
