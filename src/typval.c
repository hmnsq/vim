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

#include "vim.h"

#якщо defined(FEAT_EVAL)

/*
 * Allocate memory for a variable type-value, and make it empty (0 or NULL
 * value).
 */
    typval_T *
alloc_tv(void)
{
    повернути ALLOC_CLEAR_ONE(typval_T);
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
    якщо (rettv != NULL)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = s;
    }
    інакше
	vim_free(s);
    повернути rettv;
}

/*
 * Free the memory for a variable type-value.
 */
    void
free_tv(typval_T *varp)
{
    якщо (varp == NULL)
	повернути;

    switch (varp->v_type)
    {
	case VAR_FUNC:
	    func_unref(varp->vval.v_string);
	    // FALLTHROUGH
	case VAR_STRING:
	    vim_free(varp->vval.v_string);
	    break;
	case VAR_PARTIAL:
	    partial_unref(varp->vval.v_partial);
	    break;
	case VAR_BLOB:
	    blob_unref(varp->vval.v_blob);
	    break;
	case VAR_LIST:
	    list_unref(varp->vval.v_list);
	    break;
	case VAR_TUPLE:
	    tuple_unref(varp->vval.v_tuple);
	    break;
	case VAR_DICT:
	    dict_unref(varp->vval.v_dict);
	    break;
	case VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    job_unref(varp->vval.v_job);
	    break;
#endif
	case VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    channel_unref(varp->vval.v_channel);
	    break;
#endif
	case VAR_CLASS:
	    class_unref(varp->vval.v_class);
	    break;
	case VAR_OBJECT:
	    object_unref(varp->vval.v_object);
	    break;

	case VAR_TYPEALIAS:
	    typealias_unref(varp->vval.v_typealias);
	    break;

	case VAR_NUMBER:
	case VAR_FLOAT:
	case VAR_ANY:
	case VAR_UNKNOWN:
	case VAR_VOID:
	case VAR_BOOL:
	case VAR_SPECIAL:
	case VAR_INSTR:
	    break;
    }
    vim_free(varp);
}

/*
 * Free the memory for a variable value and set the value to NULL or 0.
 */
    void
clear_tv(typval_T *varp)
{
    якщо (varp == NULL)
	повернути;

    switch (varp->v_type)
    {
	case VAR_FUNC:
	    func_unref(varp->vval.v_string);
	    // FALLTHROUGH
	case VAR_STRING:
	    VIM_CLEAR(varp->vval.v_string);
	    break;
	case VAR_PARTIAL:
	    partial_unref(varp->vval.v_partial);
	    varp->vval.v_partial = NULL;
	    break;
	case VAR_BLOB:
	    blob_unref(varp->vval.v_blob);
	    varp->vval.v_blob = NULL;
	    break;
	case VAR_LIST:
	    list_unref(varp->vval.v_list);
	    varp->vval.v_list = NULL;
	    break;
	case VAR_TUPLE:
	    tuple_unref(varp->vval.v_tuple);
	    varp->vval.v_tuple = NULL;
	    break;
	case VAR_DICT:
	    dict_unref(varp->vval.v_dict);
	    varp->vval.v_dict = NULL;
	    break;
	case VAR_NUMBER:
	case VAR_BOOL:
	case VAR_SPECIAL:
	    varp->vval.v_number = 0;
	    break;
	case VAR_FLOAT:
	    varp->vval.v_float = 0.0;
	    break;
	case VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    job_unref(varp->vval.v_job);
	    varp->vval.v_job = NULL;
#endif
	    break;
	case VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    channel_unref(varp->vval.v_channel);
	    varp->vval.v_channel = NULL;
#endif
	    break;
	case VAR_INSTR:
	    VIM_CLEAR(varp->vval.v_instr);
	    break;
	case VAR_CLASS:
	    class_unref(varp->vval.v_class);
	    varp->vval.v_class = NULL;
	    break;
	case VAR_OBJECT:
	    object_unref(varp->vval.v_object);
	    varp->vval.v_object = NULL;
	    break;
	case VAR_TYPEALIAS:
	    typealias_unref(varp->vval.v_typealias);
	    varp->vval.v_typealias = NULL;
	    break;
	case VAR_UNKNOWN:
	case VAR_ANY:
	case VAR_VOID:
	    break;
    }
    varp->v_lock = 0;
}

/*
 * Set the value of a variable to NULL without freeing items.
 */
    void
init_tv(typval_T *varp)
{
    якщо (varp != NULL)
	CLEAR_POINTER(varp);
}

    static varnumber_T
tv_get_bool_or_number_chk(
	typval_T    *varp,
	ціле	    *denote,
	ціле	    want_bool,
	ціле	    vim9_string_error) // in Vim9 using a string is an error
{
    varnumber_T	n = 0L;

    switch (varp->v_type)
    {
	case VAR_NUMBER:
	    якщо (in_vim9script() && want_bool && varp->vval.v_number != 0
						   && varp->vval.v_number != 1)
	    {
		semsg(_(e_using_number_as_bool_nr), varp->vval.v_number);
		break;
	    }
	    повернути varp->vval.v_number;
	case VAR_FLOAT:
	    emsg(_(e_using_float_as_number));
	    break;
	case VAR_FUNC:
	case VAR_PARTIAL:
	    emsg(_(e_using_funcref_as_number));
	    break;
	case VAR_STRING:
	    якщо (vim9_string_error && in_vim9script())
	    {
		emsg_using_string_as(varp, !want_bool);
		break;
	    }
	    якщо (varp->vval.v_string != NULL)
		vim_str2nr(varp->vval.v_string, NULL, NULL,
					 STR2NR_ALL, &n, NULL, 0, FALSE, NULL);
	    повернути n;
	case VAR_LIST:
	    emsg(_(e_using_list_as_number));
	    break;
	case VAR_TUPLE:
	    emsg(_(e_using_tuple_as_number));
	    break;
	case VAR_DICT:
	    emsg(_(e_using_dictionary_as_number));
	    break;
	case VAR_BOOL:
	case VAR_SPECIAL:
	    якщо (!want_bool && in_vim9script())
	    {
		якщо (varp->v_type == VAR_BOOL)
		    emsg(_(e_using_bool_as_number));
		інакше
		    emsg(_(e_using_special_as_number));
		break;
	    }
	    повернути varp->vval.v_number == VVAL_TRUE ? 1 : 0;
	case VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    emsg(_(e_using_job_as_number));
	    break;
#endif
	case VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    emsg(_(e_using_channel_as_number));
	    break;
#endif
	case VAR_BLOB:
	    emsg(_(e_using_blob_as_number));
	    break;
	case VAR_CLASS:
	case VAR_TYPEALIAS:
	    check_typval_is_value(varp);
	    break;
	case VAR_OBJECT:
	    {
		якщо (varp->vval.v_object == NULL)
		    emsg(_(e_using_object_as_string));
		інакше
		{
		    class_T *cl = varp->vval.v_object->obj_class;
		    якщо (cl != NULL && IS_ENUM(cl))
			semsg(_(e_using_enum_str_as_number), cl->class_name.string);
		    інакше
			emsg(_(e_using_object_as_number));
		}
	    }
	    break;
	case VAR_VOID:
	    emsg(_(e_cannot_use_void_value));
	    break;
	case VAR_UNKNOWN:
	case VAR_ANY:
	case VAR_INSTR:
	    internal_error_no_abort("tv_get_number(UNKNOWN)");
	    break;
    }
    якщо (denote == NULL)		// useful for values that must be unsigned
	n = -1;
    інакше
	*denote = TRUE;
    повернути n;
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
tv_get_number(typval_T *varp)
{
    ціле		error = FALSE;

    повернути tv_get_number_chk(varp, &error);	// return 0L on error
}

/*
 * Like tv_get_number() but in Vim9 script do convert a number in a string to a
 * number without giving an error.
 */
    varnumber_T
tv_to_number(typval_T *varp)
{
    ціле		error = FALSE;

    повернути tv_get_bool_or_number_chk(varp, &error, FALSE, FALSE);
}

    varnumber_T
tv_get_number_chk(typval_T *varp, ціле *denote)
{
    повернути tv_get_bool_or_number_chk(varp, denote, FALSE, TRUE);
}

/*
 * Get the boolean value of "varp".  This is like tv_get_number_chk(),
 * but in Vim9 script accepts Number (0 and 1) and Bool/Special.
 */
    varnumber_T
tv_get_bool(typval_T *varp)
{
    повернути tv_get_bool_or_number_chk(varp, NULL, TRUE, TRUE);
}

/*
 * Get the boolean value of "varp".  This is like tv_get_number_chk(),
 * but in Vim9 script accepts Number and Bool.
 */
    varnumber_T
tv_get_bool_chk(typval_T *varp, ціле *denote)
{
    повернути tv_get_bool_or_number_chk(varp, denote, TRUE, TRUE);
}

    static float_T
tv_get_float_chk(typval_T *varp, ціле *error)
{
    switch (varp->v_type)
    {
	case VAR_NUMBER:
	    повернути (float_T)(varp->vval.v_number);
	case VAR_FLOAT:
	    повернути varp->vval.v_float;
	case VAR_FUNC:
	case VAR_PARTIAL:
	    emsg(_(e_using_funcref_as_float));
	    break;
	case VAR_STRING:
	    emsg(_(e_using_string_as_float));
	    break;
	case VAR_LIST:
	    emsg(_(e_using_list_as_float));
	    break;
	case VAR_TUPLE:
	    emsg(_(e_using_tuple_as_float));
	    break;
	case VAR_DICT:
	    emsg(_(e_using_dictionary_as_float));
	    break;
	case VAR_BOOL:
	    emsg(_(e_using_boolean_value_as_float));
	    break;
	case VAR_SPECIAL:
	    emsg(_(e_using_special_value_as_float));
	    break;
	case VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    emsg(_(e_using_job_as_float));
	    break;
#endif
	case VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    emsg(_(e_using_channel_as_float));
	    break;
#endif
	case VAR_BLOB:
	    emsg(_(e_using_blob_as_float));
	    break;
	case VAR_CLASS:
	case VAR_TYPEALIAS:
	    check_typval_is_value(varp);
	    break;
	case VAR_OBJECT:
	    emsg(_(e_using_object_as_float));
	    break;
	case VAR_VOID:
	    emsg(_(e_cannot_use_void_value));
	    break;
	case VAR_UNKNOWN:
	case VAR_ANY:
	case VAR_INSTR:
	    internal_error_no_abort("tv_get_float(UNKNOWN)");
	    break;
    }
    якщо (error != NULL)
	*error = TRUE;
    повернути 0;
}

    float_T
tv_get_float(typval_T *varp)
{
    повернути tv_get_float_chk(varp, NULL);
}

/*
 * Give an error and return FAIL unless "args[idx]" is unknown
 */
    ціле
check_for_unknown_arg(typval_T *args, ціле idx)
{
    якщо (args[idx].v_type != VAR_UNKNOWN)
    {
	semsg(_(e_too_many_arguments), idx + 1);
	повернути FAIL;
    }
    повернути OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string.
 */
    ціле
check_for_string_arg(typval_T *args, ціле idx)
{
    якщо (args[idx].v_type != VAR_STRING)
    {
	semsg(_(e_string_required_for_argument_nr), idx + 1);
	повернути FAIL;
    }
    повернути OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a non-empty string.
 */
    ціле
check_for_nonempty_string_arg(typval_T *args, ціле idx)
{
    якщо (check_for_string_arg(args, idx) == FAIL)
	повернути FAIL;
    якщо (args[idx].vval.v_string == NULL || *args[idx].vval.v_string == NUL)
    {
	semsg(_(e_non_empty_string_required_for_argument_nr), idx + 1);
	повернути FAIL;
    }
    повернути OK;
}

/*
 * Check for an optional string argument at 'idx'
 */
    ціле
check_for_opt_string_arg(typval_T *args, ціле idx)
{
    повернути (args[idx].v_type == VAR_UNKNOWN
	    || check_for_string_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a number.
 */
    ціле
check_for_number_arg(typval_T *args, ціле idx)
{
    якщо (args[idx].v_type != VAR_NUMBER)
    {
	semsg(_(e_number_required_for_argument_nr), idx + 1);
	повернути FAIL;
    }
    повернути OK;
}

/*
 * Check for an optional number argument at 'idx'
 */
    ціле
check_for_opt_number_arg(typval_T *args, ціле idx)
{
    повернути (args[idx].v_type == VAR_UNKNOWN
	    || check_for_number_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a float or a number.
 */
    ціле
check_for_float_or_nr_arg(typval_T *args, ціле idx)
{
    якщо (args[idx].v_type != VAR_FLOAT && args[idx].v_type != VAR_NUMBER)
    {
	semsg(_(e_float_or_number_required_for_argument_nr), idx + 1);
	повернути FAIL;
    }
    повернути OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a bool.
 */
    ціле
check_for_bool_arg(typval_T *args, ціле idx)
{
    якщо (args[idx].v_type != VAR_BOOL
	    && !(args[idx].v_type == VAR_NUMBER
		&& (args[idx].vval.v_number == 0
		    || args[idx].vval.v_number == 1)))
    {
	semsg(_(e_bool_required_for_argument_nr), idx + 1);
	повернути FAIL;
    }
    повернути OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a bool or a number.
 */
    static ціле
check_for_bool_or_number_arg(typval_T *args, ціле idx)
{
    якщо (args[idx].v_type != VAR_BOOL && args[idx].v_type != VAR_NUMBER)
    {
	semsg(_(e_bool_or_number_required_for_argument_nr), idx + 1);
	повернути FAIL;
    }
    повернути OK;
}

/*
 * Check for an optional bool argument at 'idx'.
 * Return FAIL if the type is wrong.
 */
    ціле
check_for_opt_bool_arg(typval_T *args, ціле idx)
{
    якщо (args[idx].v_type == VAR_UNKNOWN)
	повернути OK;
    повернути check_for_bool_arg(args, idx);
}

/*
 * Check for an optional bool or number argument at 'idx'.
 * Return FAIL if the type is wrong.
 */
    ціле
check_for_opt_bool_or_number_arg(typval_T *args, ціле idx)
{
    якщо (args[idx].v_type == VAR_UNKNOWN)
	повернути OK;
    повернути check_for_bool_or_number_arg(args, idx);
}

/*
 * Check for an optional bool or dict argument at 'idx'.
 * Return FAIL if the type is wrong.
 */
    ціле
check_for_opt_bool_or_dict_arg(typval_T *args, ціле idx)
{
    якщо (args[idx].v_type == VAR_UNKNOWN || args[idx].v_type == VAR_DICT)
	повернути OK;
    повернути check_for_bool_arg(args, idx);
}

/*
 * Give an error and return FAIL unless "args[idx]" is a blob.
 */
    ціле
check_for_blob_arg(typval_T *args, ціле idx)
{
    якщо (args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_blob_required_for_argument_nr), idx + 1);
	повернути FAIL;
    }
    повернути OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list.
 */
    ціле
check_for_list_arg(typval_T *args, ціле idx)
{
    якщо (args[idx].v_type != VAR_LIST)
    {
	semsg(_(e_list_required_for_argument_nr), idx + 1);
	повернути FAIL;
    }
    повернути OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a non-NULL list.
 */
    ціле
check_for_nonnull_list_arg(typval_T *args, ціле idx)
{
    якщо (check_for_list_arg(args, idx) == FAIL)
	повернути FAIL;

    якщо (args[idx].vval.v_list == NULL)
    {
	semsg(_(e_non_null_list_required_for_argument_nr), idx + 1);
	повернути FAIL;
    }
    повернути OK;
}

/*
 * Check for an optional list argument at 'idx'
 */
    ціле
check_for_opt_list_arg(typval_T *args, ціле idx)
{
    повернути (args[idx].v_type == VAR_UNKNOWN
	    || check_for_list_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a tuple.
 */
    ціле
check_for_tuple_arg(typval_T *args, ціле idx)
{
    якщо (args[idx].v_type != VAR_TUPLE)
    {
	semsg(_(e_tuple_required_for_argument_nr), idx + 1);
	повернути FAIL;
    }
    повернути OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a dict.
 */
    ціле
check_for_dict_arg(typval_T *args, ціле idx)
{
    якщо (args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_dict_required_for_argument_nr), idx + 1);
	повернути FAIL;
    }
    повернути OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a non-NULL dict.
 */
    ціле
check_for_nonnull_dict_arg(typval_T *args, ціле idx)
{
    якщо (check_for_dict_arg(args, idx) == FAIL)
	повернути FAIL;

    якщо (args[idx].vval.v_dict == NULL)
    {
	semsg(_(e_non_null_dict_required_for_argument_nr), idx + 1);
	повернути FAIL;
    }
    повернути OK;
}

/*
 * Check for an optional dict argument at 'idx'
 */
    ціле
check_for_opt_dict_arg(typval_T *args, ціле idx)
{
    повернути (args[idx].v_type == VAR_UNKNOWN
	    || check_for_dict_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Check for an optional non-NULL dict argument at 'idx'
 */
    ціле
check_for_opt_nonnull_dict_arg(typval_T *args, ціле idx)
{
    повернути (args[idx].v_type == VAR_UNKNOWN
	    || check_for_nonnull_dict_arg(args, idx) != FAIL) ? OK : FAIL;
}

#якщо defined(FEAT_JOB_CHANNEL)
/*
 * Give an error and return FAIL unless "args[idx]" is a channel or a job.
 */
    ціле
check_for_chan_or_job_arg(typval_T *args, ціле idx)
{
    якщо (args[idx].v_type != VAR_CHANNEL && args[idx].v_type != VAR_JOB)
    {
	semsg(_(e_chan_or_job_required_for_argument_nr), idx + 1);
	повернути FAIL;
    }
    повернути OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an optional channel or a
 * job.
 */
    ціле
check_for_opt_chan_or_job_arg(typval_T *args, ціле idx)
{
    повернути (args[idx].v_type == VAR_UNKNOWN
	    || check_for_chan_or_job_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a job.
 */
    ціле
check_for_job_arg(typval_T *args, ціле idx)
{
    якщо (args[idx].v_type != VAR_JOB)
    {
	semsg(_(e_job_required_for_argument_nr), idx + 1);
	повернути FAIL;
    }
    повернути OK;
}

/*
 * Check for an optional job argument at 'idx'.
 */
    ціле
check_for_opt_job_arg(typval_T *args, ціле idx)
{
    повернути (args[idx].v_type == VAR_UNKNOWN
	    || check_for_job_arg(args, idx) != FAIL) ? OK : FAIL;
}
#інакше
/*
 * Give an error and return FAIL unless "args[idx]" is an optional channel or a
 * job.  Used without the +channel feature, thus only VAR_UNKNOWN is accepted.
 */
    ціле
check_for_opt_chan_or_job_arg(typval_T *args, ціле idx)
{
    повернути args[idx].v_type == VAR_UNKNOWN ? OK : FAIL;
}
#endif

/*
 * Give an error and return FAIL unless "args[idx]" is a string or
 * a number.
 */
    ціле
check_for_string_or_number_arg(typval_T *args, ціле idx)
{
    якщо (args[idx].v_type != VAR_STRING && args[idx].v_type != VAR_NUMBER)
    {
	semsg(_(e_string_or_number_required_for_argument_nr), idx + 1);
	повернути FAIL;
    }
    повернути OK;
}

/*
 * Check for an optional string or number argument at 'idx'.
 */
    ціле
check_for_opt_string_or_number_arg(typval_T *args, ціле idx)
{
    повернути (args[idx].v_type == VAR_UNKNOWN
	    || check_for_string_or_number_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a buffer number.
 * Buffer number can be a number or a string.
 */
    ціле
check_for_buffer_arg(typval_T *args, ціле idx)
{
    повернути check_for_string_or_number_arg(args, idx);
}

/*
 * Check for an optional buffer argument at 'idx'
 */
    ціле
check_for_opt_buffer_arg(typval_T *args, ціле idx)
{
    повернути (args[idx].v_type == VAR_UNKNOWN
	    || check_for_buffer_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a line number.
 * Line number can be a number or a string.
 */
    ціле
check_for_lnum_arg(typval_T *args, ціле idx)
{
    повернути check_for_string_or_number_arg(args, idx);
}

/*
 * Check for an optional line number argument at 'idx'
 */
    ціле
check_for_opt_lnum_arg(typval_T *args, ціле idx)
{
    повернути (args[idx].v_type == VAR_UNKNOWN
	    || check_for_lnum_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a blob.
 */
    ціле
check_for_string_or_blob_arg(typval_T *args, ціле idx)
{
    якщо (args[idx].v_type != VAR_STRING && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_string_or_blob_required_for_argument_nr), idx + 1);
	повернути FAIL;
    }
    повернути OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a list.
 */
    ціле
check_for_string_or_list_arg(typval_T *args, ціле idx)
{
    якщо (args[idx].v_type != VAR_STRING && args[idx].v_type != VAR_LIST)
    {
	semsg(_(e_string_or_list_required_for_argument_nr), idx + 1);
	повернути FAIL;
    }
    повернути OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string, a list, a
 * tuple or a blob.
 */
    ціле
check_for_string_or_list_or_tuple_or_blob_arg(typval_T *args, ціле idx)
{
    якщо (args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_string_list_tuple_or_blob_required_for_argument_nr), idx + 1);
	повернути FAIL;
    }
    повернути OK;
}

/*
 * Check for an optional string or list argument at 'idx'
 */
    ціле
check_for_opt_string_or_list_arg(typval_T *args, ціле idx)
{
    повернути (args[idx].v_type == VAR_UNKNOWN
	    || check_for_string_or_list_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a dict.
 */
    ціле
check_for_string_or_dict_arg(typval_T *args, ціле idx)
{
    якщо (args[idx].v_type != VAR_STRING && args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_string_or_dict_required_for_argument_nr), idx + 1);
	повернути FAIL;
    }
    повернути OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a number
 * or a list.
 */
    ціле
check_for_string_or_number_or_list_arg(typval_T *args, ціле idx)
{
    якщо (args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_NUMBER
	    && args[idx].v_type != VAR_LIST)
    {
	semsg(_(e_string_number_or_list_required_for_argument_nr), idx + 1);
	повернути FAIL;
    }
    повернути OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an optional string
 * or number or a list
 */
    ціле
check_for_opt_string_or_number_or_list_arg(typval_T *args, ціле idx)
{
    повернути (args[idx].v_type == VAR_UNKNOWN
	    || check_for_string_or_number_or_list_arg(args, idx)
							!= FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string, a number, a
 * list, a tuple or a blob.
 */
    ціле
check_for_repeat_func_arg(typval_T *args, ціле idx)
{
    якщо (args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_NUMBER
	    && args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_repeatable_type_required_for_argument_nr), idx + 1);
	повернути FAIL;
    }
    повернути OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string, a list, a
 * tuple or a dict.
 */
    ціле
check_for_string_list_tuple_or_dict_arg(typval_T *args, ціле idx)
{
    якщо (args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_string_list_tuple_or_dict_required_for_argument_nr), idx + 1);
	повернути FAIL;
    }
    повернути OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string
 * or a function reference.
 */
    ціле
check_for_string_or_func_arg(typval_T *args, ціле idx)
{
    якщо (args[idx].v_type != VAR_PARTIAL
	    && args[idx].v_type != VAR_FUNC
	    && args[idx].v_type != VAR_STRING)
    {
	semsg(_(e_string_or_function_required_for_argument_nr), idx + 1);
	повернути FAIL;
    }
    повернути OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list or a blob.
 */
    ціле
check_for_list_or_blob_arg(typval_T *args, ціле idx)
{
    якщо (args[idx].v_type != VAR_LIST && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_list_or_blob_required_for_argument_nr), idx + 1);
	повернути FAIL;
    }
    повернути OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list or a tuple.
 */
    ціле
check_for_list_or_tuple_arg(typval_T *args, ціле idx)
{
    якщо (args[idx].v_type != VAR_LIST && args[idx].v_type != VAR_TUPLE)
    {
	semsg(_(e_list_or_tuple_required_for_argument_nr), idx + 1);
	повернути FAIL;
    }
    повернути OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list, a tuple or a
 * blob.
 */
    ціле
check_for_list_or_tuple_or_blob_arg(typval_T *args, ціле idx)
{
    якщо (args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_list_or_tuple_or_blob_required_for_argument_nr), idx + 1);
	повернути FAIL;
    }
    повернути OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list, a tuple or a
 * dict
 */
    ціле
check_for_list_or_tuple_or_dict_arg(typval_T *args, ціле idx)
{
    якщо (args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_list_or_tuple_or_dict_required_for_argument_nr), idx + 1);
	повернути FAIL;
    }
    повернути OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list or dict or a
 * blob.
 */
    ціле
check_for_list_or_dict_or_blob_arg(typval_T *args, ціле idx)
{
    якщо (args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_DICT
	    && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_list_dict_or_blob_required_for_argument_nr), idx + 1);
	повернути FAIL;
    }
    повернути OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list, a tuple, a dict,
 * a blob or a string.
 */
    ціле
check_for_list_tuple_dict_blob_or_string_arg(typval_T *args, ціле idx)
{
    якщо (args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_DICT
	    && args[idx].v_type != VAR_BLOB
	    && args[idx].v_type != VAR_STRING)
    {
	semsg(_(e_list_tuple_dict_blob_or_string_required_for_argument_nr), idx + 1);
	повернути FAIL;
    }
    повернути OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an optional buffer
 * number or a dict.
 */
    ціле
check_for_opt_buffer_or_dict_arg(typval_T *args, ціле idx)
{
    якщо (args[idx].v_type != VAR_UNKNOWN
	    && args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_NUMBER
	    && args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_string_required_for_argument_nr), idx + 1);
	повернути FAIL;
    }
    повернути OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an object.
 */
    ціле
check_for_object_arg(typval_T *args, ціле idx)
{
    якщо (args[idx].v_type != VAR_OBJECT)
    {
	semsg(_(e_object_required_for_argument_nr), idx + 1);
	повернути FAIL;
    }
    повернути OK;
}

/*
 * Returns TRUE if "tv" is a type alias for a class
 */
    static ціле
tv_class_alias(typval_T *tv)
{
    повернути tv->v_type == VAR_TYPEALIAS &&
			tv->vval.v_typealias->ta_type->tt_type == VAR_OBJECT;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a class
 * or class typealias.
 */
    ціле
check_for_class_or_typealias_args(typval_T *args, ціле idx)
{
    для (ціле i = idx; args[i].v_type != VAR_UNKNOWN; ++i)
    {
	якщо (args[i].v_type != VAR_CLASS && !tv_class_alias(&args[idx]))
	{
	    semsg(_(e_class_or_typealias_required_for_argument_nr), i + 1);
	    повернути FAIL;
	}
    }
    повернути OK;
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
    static char_u   mybuf[NUMBUFLEN];

    повернути tv_get_string_buf(varp, mybuf);
}

/*
 * Like tv_get_string() but don't allow number to string conversion for Vim9.
 */
    char_u *
tv_get_string_strict(typval_T *varp)
{
    static char_u   mybuf[NUMBUFLEN];
    char_u	    *res =  tv_get_string_buf_chk_strict(
						 varp, mybuf, in_vim9script());

    повернути res != NULL ? res : (char_u *)"";
}

    char_u *
tv_get_string_buf(typval_T *varp, char_u *buf)
{
    char_u	*res = tv_get_string_buf_chk(varp, buf);

    повернути res != NULL ? res : (char_u *)"";
}

/*
 * Careful: This uses a single, static buffer.  YOU CAN ONLY USE IT ONCE!
 */
    char_u *
tv_get_string_chk(typval_T *varp)
{
    static char_u   mybuf[NUMBUFLEN];

    повернути tv_get_string_buf_chk(varp, mybuf);
}

    char_u *
tv_get_string_buf_chk(typval_T *varp, char_u *buf)
{
    повернути tv_get_string_buf_chk_strict(varp, buf, FALSE);
}

    char_u *
tv_get_string_buf_chk_strict(typval_T *varp, char_u *buf, ціле strict)
{
    switch (varp->v_type)
    {
	case VAR_NUMBER:
	    якщо (strict)
	    {
		emsg(_(e_using_number_as_string));
		break;
	    }
	    vim_snprintf((char *)buf, NUMBUFLEN, "%lld",
					    (varnumber_T)varp->vval.v_number);
	    повернути buf;
	case VAR_FUNC:
	case VAR_PARTIAL:
	    emsg(_(e_using_funcref_as_string));
	    break;
	case VAR_LIST:
	    emsg(_(e_using_list_as_string));
	    break;
	case VAR_TUPLE:
	    emsg(_(e_using_tuple_as_string));
	    break;
	case VAR_DICT:
	    emsg(_(e_using_dictionary_as_string));
	    break;
	case VAR_FLOAT:
	    якщо (strict)
	    {
		emsg(_(e_using_float_as_string));
		break;
	    }
	    vim_snprintf((char *)buf, NUMBUFLEN, "%g", varp->vval.v_float);
	    повернути buf;
	case VAR_STRING:
	    якщо (varp->vval.v_string != NULL)
		повернути varp->vval.v_string;
	    повернути (char_u *)"";
	case VAR_BOOL:
	case VAR_SPECIAL:
	    STRCPY(buf, get_var_special_name(varp->vval.v_number));
	    повернути buf;
	case VAR_BLOB:
	    emsg(_(e_using_blob_as_string));
	    break;
	case VAR_CLASS:
	case VAR_TYPEALIAS:
	    check_typval_is_value(varp);
	    break;
	case VAR_OBJECT:
	    {
		якщо (varp->vval.v_object == NULL)
		    emsg(_(e_using_object_as_string));
		інакше
		{
		    class_T *cl = varp->vval.v_object->obj_class;
		    якщо (cl != NULL && IS_ENUM(cl))
			semsg(_(e_using_enum_str_as_string), cl->class_name.string);
		    інакше
			emsg(_(e_using_object_as_string));
		}
	    }
	    break;
	case VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    якщо (in_vim9script())
	    {
		semsg(_(e_using_invalid_value_as_string_str), "job");
		break;
	    }
	    повернути job_to_string_buf(varp, buf);
#endif
	    break;
	case VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    якщо (in_vim9script())
	    {
		semsg(_(e_using_invalid_value_as_string_str), "channel");
		break;
	    }
	    повернути channel_to_string_buf(varp, buf);
#endif
	    break;
	case VAR_VOID:
	    emsg(_(e_cannot_use_void_value));
	    break;
	case VAR_UNKNOWN:
	case VAR_ANY:
	case VAR_INSTR:
	    semsg(_(e_using_invalid_value_as_string_str),
						  vartype_name(varp->v_type));
	    break;
    }
    повернути NULL;
}

/*
 * Turn a typeval into a string.  Similar to tv_get_string_buf() but uses
 * string() on Dict, List, etc.
 */
    char_u *
tv_stringify(typval_T *varp, char_u *buf)
{
    якщо (varp->v_type == VAR_LIST
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
	повернути tmp.vval.v_string;
    }
    повернути tv_get_string_buf(varp, buf);
}

/*
 * Return TRUE if typeval "tv" and its value are set to be locked (immutable).
 * Also give an error message, using "name" or _("name") when use_gettext is
 * TRUE.
 */
    ціле
tv_check_lock(typval_T *tv, char_u *name, ціле use_gettext)
{
    ціле	lock = 0;

    switch (tv->v_type)
    {
	case VAR_BLOB:
	    якщо (tv->vval.v_blob != NULL)
		lock = tv->vval.v_blob->bv_lock;
	    break;
	case VAR_LIST:
	    якщо (tv->vval.v_list != NULL)
		lock = tv->vval.v_list->lv_lock;
	    break;
	case VAR_TUPLE:
	    якщо (tv->vval.v_tuple != NULL)
		lock = tv->vval.v_tuple->tv_lock;
	    break;
	case VAR_DICT:
	    якщо (tv->vval.v_dict != NULL)
		lock = tv->vval.v_dict->dv_lock;
	    break;
	default:
	    break;
    }
    повернути value_check_lock(tv->v_lock, name, use_gettext)
		   || (lock != 0 && value_check_lock(lock, name, use_gettext));
}

/*
 * Copy the values from typval_T "from" to typval_T "to".
 * When needed allocates string or increases reference count.
 * Does not make a copy of a list, blob or dict but copies the reference!
 * It is OK for "from" and "to" to point to the same item.  This is used to
 * make a copy later.
 */
    ціле
copy_tv(typval_T *from, typval_T *to)
{
    ціле		ret = OK;

    to->v_type = from->v_type;
    to->v_lock = 0;
    switch (from->v_type)
    {
	case VAR_NUMBER:
	case VAR_BOOL:
	case VAR_SPECIAL:
	    to->vval.v_number = from->vval.v_number;
	    break;
	case VAR_FLOAT:
	    to->vval.v_float = from->vval.v_float;
	    break;
	case VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    to->vval.v_job = from->vval.v_job;
	    якщо (to->vval.v_job != NULL)
		++to->vval.v_job->jv_refcount;
	    break;
#endif
	case VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    to->vval.v_channel = from->vval.v_channel;
	    якщо (to->vval.v_channel != NULL)
		++to->vval.v_channel->ch_refcount;
	    break;
#endif
	case VAR_INSTR:
	    to->vval.v_instr = from->vval.v_instr;
	    break;

	case VAR_CLASS:
	    copy_class(from, to);
	    break;

	case VAR_OBJECT:
	    copy_object(from, to);
	    break;

	case VAR_STRING:
	case VAR_FUNC:
	    якщо (from->vval.v_string == NULL)
		to->vval.v_string = NULL;
	    інакше
	    {
		to->vval.v_string = vim_strsave(from->vval.v_string);
		якщо (from->v_type == VAR_FUNC)
		    func_ref(to->vval.v_string);
	    }
	    break;
	case VAR_PARTIAL:
	    якщо (from->vval.v_partial == NULL)
		to->vval.v_partial = NULL;
	    інакше
	    {
		to->vval.v_partial = from->vval.v_partial;
		++to->vval.v_partial->pt_refcount;
	    }
	    break;
	case VAR_BLOB:
	    якщо (from->vval.v_blob == NULL)
		to->vval.v_blob = NULL;
	    інакше
	    {
		to->vval.v_blob = from->vval.v_blob;
		++to->vval.v_blob->bv_refcount;
	    }
	    break;
	case VAR_LIST:
	    якщо (from->vval.v_list == NULL)
		to->vval.v_list = NULL;
	    інакше
	    {
		to->vval.v_list = from->vval.v_list;
		++to->vval.v_list->lv_refcount;
	    }
	    break;
	case VAR_TUPLE:
	    якщо (from->vval.v_tuple == NULL)
		to->vval.v_tuple = NULL;
	    інакше
	    {
		to->vval.v_tuple = from->vval.v_tuple;
		++to->vval.v_tuple->tv_refcount;
	    }
	    break;
	case VAR_DICT:
	    якщо (from->vval.v_dict == NULL)
		to->vval.v_dict = NULL;
	    інакше
	    {
		to->vval.v_dict = from->vval.v_dict;
		++to->vval.v_dict->dv_refcount;
	    }
	    break;
	case VAR_TYPEALIAS:
	    якщо (from->vval.v_typealias == NULL)
		to->vval.v_typealias = NULL;
	    інакше
	    {
		to->vval.v_typealias = from->vval.v_typealias;
		++to->vval.v_typealias->ta_refcount;
	    }
	    break;
	case VAR_VOID:
	    emsg(_(e_cannot_use_void_value));
	    ret = FAIL;
	    break;
	case VAR_UNKNOWN:
	case VAR_ANY:
	    internal_error_no_abort("copy_tv(UNKNOWN)");
	    ret = FAIL;
	    break;
    }

    повернути ret;
}

/*
 * Compare "tv1" and "tv2".
 */
    ціле
typval_compare2(
    typval_T	*tv1,	// first operand
    typval_T	*tv2,	// second operand
    exprtype_T	type,	// operator
    ціле		ic,	// ignore case
    ціле		*res)	// comparison result
{
    varnumber_T	n1, n2;
    ціле		type_is = type == EXPR_IS || type == EXPR_ISNOT;

    якщо (check_typval_is_value(tv1) == FAIL
	|| check_typval_is_value(tv2) == FAIL)
	повернути FAIL;
    інакше якщо (type_is && tv1->v_type != tv2->v_type)
    {
	// For "is" a different type always means FALSE, for "isnot"
	// it means TRUE.
	*res = (type == EXPR_ISNOT);
    }
    інакше якщо (((tv1->v_type == VAR_SPECIAL && tv1->vval.v_number == VVAL_NULL)
		|| (tv2->v_type == VAR_SPECIAL
					   && tv2->vval.v_number == VVAL_NULL))
	    && tv1->v_type != tv2->v_type
	    && (type == EXPR_EQUAL || type == EXPR_NEQUAL))
    {
	n1 = typval_compare_null(tv1, tv2);
	якщо (n1 == MAYBE)
	    повернути FAIL;
	якщо (type == EXPR_NEQUAL)
	    n1 = !n1;
	*res = n1;
    }
    інакше якщо (tv1->v_type == VAR_BLOB || tv2->v_type == VAR_BLOB)
    {
	якщо (typval_compare_blob(tv1, tv2, type, res) == FAIL)
	    повернути FAIL;
    }
    інакше якщо (tv1->v_type == VAR_LIST || tv2->v_type == VAR_LIST)
    {
	якщо (typval_compare_list(tv1, tv2, type, ic, res) == FAIL)
	    повернути FAIL;
    }
    інакше якщо (tv1->v_type == VAR_TUPLE || tv2->v_type == VAR_TUPLE)
    {
	якщо (typval_compare_tuple(tv1, tv2, type, ic, res) == FAIL)
	    повернути FAIL;
    }
    інакше якщо (tv1->v_type == VAR_OBJECT || tv2->v_type == VAR_OBJECT)
    {
	якщо (typval_compare_object(tv1, tv2, type, ic, res) == FAIL)
	    повернути FAIL;
    }
    інакше якщо (tv1->v_type == VAR_DICT || tv2->v_type == VAR_DICT)
    {
	якщо (typval_compare_dict(tv1, tv2, type, ic, res) == FAIL)
	    повернути FAIL;
    }
    інакше якщо (tv1->v_type == VAR_FUNC || tv2->v_type == VAR_FUNC
	|| tv1->v_type == VAR_PARTIAL || tv2->v_type == VAR_PARTIAL)
    {
	якщо (typval_compare_func(tv1, tv2, type, ic, res) == FAIL)
	    повернути FAIL;
    }

    // If one of the two variables is a float, compare as a float.
    // When using "=~" or "!~", always compare as string.
    інакше якщо ((tv1->v_type == VAR_FLOAT || tv2->v_type == VAR_FLOAT)
	    && type != EXPR_MATCH && type != EXPR_NOMATCH)
    {
	float_T f1, f2;
	ціле	error = FALSE;

	f1 = tv_get_float_chk(tv1, &error);
	якщо (!error)
	    f2 = tv_get_float_chk(tv2, &error);
	якщо (error)
	    повернути FAIL;
	n1 = FALSE;
	switch (type)
	{
	    case EXPR_IS:
	    case EXPR_EQUAL:    n1 = (f1 == f2); break;
	    case EXPR_ISNOT:
	    case EXPR_NEQUAL:   n1 = (f1 != f2); break;
	    case EXPR_GREATER:  n1 = (f1 > f2); break;
	    case EXPR_GEQUAL:   n1 = (f1 >= f2); break;
	    case EXPR_SMALLER:  n1 = (f1 < f2); break;
	    case EXPR_SEQUAL:   n1 = (f1 <= f2); break;
	    case EXPR_UNKNOWN:
	    case EXPR_MATCH:
	    default:  break;  // avoid gcc warning
	}
	*res = n1;
    }

    // If one of the two variables is a number, compare as a number.
    // When using "=~" or "!~", always compare as string.
    інакше якщо ((tv1->v_type == VAR_NUMBER || tv2->v_type == VAR_NUMBER)
	    && type != EXPR_MATCH && type != EXPR_NOMATCH)
    {
	ціле error = FALSE;

	n1 = tv_get_number_chk(tv1, &error);
	якщо (!error)
	    n2 = tv_get_number_chk(tv2, &error);
	якщо (error)
	    повернути FAIL;
	switch (type)
	{
	    case EXPR_IS:
	    case EXPR_EQUAL:    n1 = (n1 == n2); break;
	    case EXPR_ISNOT:
	    case EXPR_NEQUAL:   n1 = (n1 != n2); break;
	    case EXPR_GREATER:  n1 = (n1 > n2); break;
	    case EXPR_GEQUAL:   n1 = (n1 >= n2); break;
	    case EXPR_SMALLER:  n1 = (n1 < n2); break;
	    case EXPR_SEQUAL:   n1 = (n1 <= n2); break;
	    case EXPR_UNKNOWN:
	    case EXPR_MATCH:
	    default:  break;  // avoid gcc warning
	}
	*res = n1;
    }
    інакше якщо (in_vim9script() && (tv1->v_type == VAR_BOOL
				    || tv2->v_type == VAR_BOOL
				    || (tv1->v_type == VAR_SPECIAL
					      && tv2->v_type == VAR_SPECIAL)))
    {
	якщо (tv1->v_type != tv2->v_type)
	{
	    semsg(_(e_cannot_compare_str_with_str),
		       vartype_name(tv1->v_type), vartype_name(tv2->v_type));
	    повернути FAIL;
	}
	n1 = tv1->vval.v_number;
	n2 = tv2->vval.v_number;
	switch (type)
	{
	    case EXPR_IS:
	    case EXPR_EQUAL:    n1 = (n1 == n2); break;
	    case EXPR_ISNOT:
	    case EXPR_NEQUAL:   n1 = (n1 != n2); break;
	    default:
		semsg(_(e_invalid_operation_for_str),
						   vartype_name(tv1->v_type));
		повернути FAIL;
	}
	*res = n1;
    }
#ifdef FEAT_JOB_CHANNEL
    інакше якщо (tv1->v_type == tv2->v_type
	    && (tv1->v_type == VAR_CHANNEL || tv1->v_type == VAR_JOB)
	    && (type == EXPR_NEQUAL || type == EXPR_EQUAL))
    {
	якщо (tv1->v_type == VAR_CHANNEL)
	    n1 = tv1->vval.v_channel == tv2->vval.v_channel;
	інакше
	    n1 = tv1->vval.v_job == tv2->vval.v_job;
	якщо (type == EXPR_NEQUAL)
	    n1 = !n1;
	*res = n1;
    }
#endif
    інакше
    {
	якщо (typval_compare_string(tv1, tv2, type, ic, res) == FAIL)
	    повернути FAIL;
    }

    повернути OK;
}

/*
 * Compare "tv1" and "tv2".
 * Put the result in "tv1".  Caller should clear "tv2".
 */
    ціле
typval_compare(
    typval_T	*tv1,	// first operand
    typval_T	*tv2,	// second operand
    exprtype_T	type,   // operator
    ціле		ic)     // ignore case
{
    ціле res;

    якщо (typval_compare2(tv1, tv2, type, ic, &res) == FAIL)
    {
	clear_tv(tv1);
	повернути FAIL;
    }

    clear_tv(tv1);
    якщо (in_vim9script())
    {
	tv1->v_type = VAR_BOOL;
	tv1->vval.v_number = res ? VVAL_TRUE : VVAL_FALSE;
    }
    інакше
    {
	tv1->v_type = VAR_NUMBER;
	tv1->vval.v_number = res;
    }

    повернути OK;
}

/*
 * Compare "tv1" to "tv2" as lists according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    ціле
typval_compare_list(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	ціле	    ic,
	ціле	    *res)
{
    ціле	    val = 0;

    якщо (type == EXPR_IS || type == EXPR_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
				      && tv1->vval.v_list == tv2->vval.v_list);
	якщо (type == EXPR_ISNOT)
	    val = !val;
    }
    інакше якщо (tv1->v_type != tv2->v_type
	    || (type != EXPR_EQUAL && type != EXPR_NEQUAL))
    {
	якщо (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_list_with_list));
	інакше
	    emsg(_(e_invalid_operation_for_list));
	повернути FAIL;
    }
    інакше
    {
	val = list_equal(tv1->vval.v_list, tv2->vval.v_list, ic);
	якщо (type == EXPR_NEQUAL)
	    val = !val;
    }
    *res = val;
    повернути OK;
}

/*
 * Compare "tv1" to "tv2" as tuples according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    ціле
typval_compare_tuple(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	ціле	    ic,
	ціле	    *res)
{
    ціле	    val = 0;

    якщо (type == EXPR_IS || type == EXPR_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
			      && tv1->vval.v_tuple == tv2->vval.v_tuple);
	якщо (type == EXPR_ISNOT)
	    val = !val;
    }
    інакше якщо (tv1->v_type != tv2->v_type
	    || (type != EXPR_EQUAL && type != EXPR_NEQUAL))
    {
	якщо (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_tuple_with_tuple));
	інакше
	    emsg(_(e_invalid_operation_for_tuple));
	повернути FAIL;
    }
    інакше
    {
	val = tuple_equal(tv1->vval.v_tuple, tv2->vval.v_tuple, ic);
	якщо (type == EXPR_NEQUAL)
	    val = !val;
    }
    *res = val;
    повернути OK;
}

/*
 * Compare v:null with another type.  Return TRUE if the value is NULL.
 */
    ціле
typval_compare_null(typval_T *tv1, typval_T *tv2)
{
    якщо ((tv1->v_type == VAR_SPECIAL && tv1->vval.v_number == VVAL_NULL)
	    || (tv2->v_type == VAR_SPECIAL && tv2->vval.v_number == VVAL_NULL))
    {
	typval_T	*tv = tv1->v_type == VAR_SPECIAL ? tv2 : tv1;

	switch (tv->v_type)
	{
	    case VAR_BLOB: повернути tv->vval.v_blob == NULL;
#ifdef FEAT_JOB_CHANNEL
	    case VAR_CHANNEL: повернути tv->vval.v_channel == NULL;
#endif
	    // TODO: null_class handling
	    // case VAR_CLASS: return tv->vval.v_class == NULL;
	    case VAR_DICT: повернути tv->vval.v_dict == NULL;
	    case VAR_FUNC: повернути tv->vval.v_string == NULL;
#ifdef FEAT_JOB_CHANNEL
	    case VAR_JOB: повернути tv->vval.v_job == NULL;
#endif
	    case VAR_LIST: повернути tv->vval.v_list == NULL;
	    case VAR_TUPLE: повернути tv->vval.v_tuple == NULL;
	    case VAR_OBJECT: повернути tv->vval.v_object == NULL;
	    case VAR_PARTIAL: повернути tv->vval.v_partial == NULL;
	    case VAR_STRING: повернути tv->vval.v_string == NULL;

	    case VAR_NUMBER: якщо (!in_vim9script())
				 повернути tv->vval.v_number == 0;
			     break;
	    case VAR_FLOAT: якщо (!in_vim9script())
				 повернути tv->vval.v_float == 0.0;
			     break;
	    case VAR_TYPEALIAS: повернути tv->vval.v_typealias == NULL;
	    default: break;
	}
    }
    // although comparing null with number, float or bool is not very useful
    // we won't give an error
    повернути FALSE;
}

/*
 * Compare "tv1" to "tv2" as blobs according to "type".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    ціле
typval_compare_blob(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	ціле	    *res)
{
    ціле	    val = 0;

    якщо (type == EXPR_IS || type == EXPR_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
			&& tv1->vval.v_blob == tv2->vval.v_blob);
	якщо (type == EXPR_ISNOT)
	    val = !val;
    }
    інакше якщо (tv1->v_type != tv2->v_type
	    || (type != EXPR_EQUAL && type != EXPR_NEQUAL))
    {
	якщо (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_blob_with_blob));
	інакше
	    emsg(_(e_invalid_operation_for_blob));
	повернути FAIL;
    }
    інакше
    {
	val = blob_equal(tv1->vval.v_blob, tv2->vval.v_blob);
	якщо (type == EXPR_NEQUAL)
	    val = !val;
    }
    *res = val;
    повернути OK;
}

/*
 * Compare "tv1" to "tv2" as objects according to "type".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    ціле
typval_compare_object(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	ціле	    ic,
	ціле	    *res)
{
    ціле res_match = type == EXPR_EQUAL || type == EXPR_IS ? TRUE : FALSE;

    якщо (tv1->vval.v_object == NULL && tv2->vval.v_object == NULL)
    {
	*res = res_match;
	повернути OK;
    }
    якщо (tv1->vval.v_object == NULL || tv2->vval.v_object == NULL)
    {
	*res = !res_match;
	повернути OK;
    }

    object_T *obj1 = tv1->vval.v_object;
    object_T *obj2 = tv2->vval.v_object;
    якщо (type == EXPR_IS || type == EXPR_ISNOT)
    {
	*res = obj1 == obj2 ? res_match : !res_match;
	повернути OK;
    }
    інакше якщо (tv1->v_type != tv2->v_type)
    {
	emsg(_(e_can_only_compare_object_with_object));
	повернути FAIL;
    }

    *res = object_equal(obj1, obj2, ic) ? res_match : !res_match;
    повернути OK;
}

/*
 * Compare "tv1" to "tv2" as dictionaries according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    ціле
typval_compare_dict(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	ціле	    ic,
	ціле	    *res)
{
    ціле	    val;

    якщо (type == EXPR_IS || type == EXPR_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
			&& tv1->vval.v_dict == tv2->vval.v_dict);
	якщо (type == EXPR_ISNOT)
	    val = !val;
    }
    інакше якщо (tv1->v_type != tv2->v_type
		|| (type != EXPR_EQUAL && type != EXPR_NEQUAL))
    {
	якщо (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_dictionary_with_dictionary));
	інакше
	    emsg(_(e_invalid_operation_for_dictionary));
	повернути FAIL;
    }
    інакше
    {
	val = dict_equal(tv1->vval.v_dict, tv2->vval.v_dict, ic);
	якщо (type == EXPR_NEQUAL)
	    val = !val;
    }
    *res = val;
    повернути OK;
}

/*
 * Compare "tv1" to "tv2" as funcrefs according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    ціле
typval_compare_func(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	ціле	    ic,
	ціле	    *res)
{
    ціле	    val = 0;

    якщо (type != EXPR_EQUAL && type != EXPR_NEQUAL
	    && type != EXPR_IS && type != EXPR_ISNOT)
    {
	emsg(_(e_invalid_operation_for_funcrefs));
	повернути FAIL;
    }
    якщо ((tv1->v_type == VAR_PARTIAL && tv1->vval.v_partial == NULL)
	    || (tv2->v_type == VAR_PARTIAL && tv2->vval.v_partial == NULL))
	// When both partials are NULL, then they are equal.
	// Otherwise they are not equal.
	val = (tv1->vval.v_partial == tv2->vval.v_partial);
    інакше якщо (type == EXPR_IS || type == EXPR_ISNOT)
    {
	якщо (tv1->v_type == VAR_FUNC && tv2->v_type == VAR_FUNC)
	    // strings are considered the same if their value is
	    // the same
	    val = tv_equal(tv1, tv2, ic);
	інакше якщо (tv1->v_type == VAR_PARTIAL && tv2->v_type == VAR_PARTIAL)
	    val = (tv1->vval.v_partial == tv2->vval.v_partial);
	інакше
	    val = FALSE;
    }
    інакше
	val = tv_equal(tv1, tv2, ic);
    якщо (type == EXPR_NEQUAL || type == EXPR_ISNOT)
	val = !val;
    *res = val;
    повернути OK;
}

/*
 * Compare "tv1" to "tv2" as strings according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    ціле
typval_compare_string(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	ціле	    ic,
	ціле	    *res)
{
    ціле		i = 0;
    ціле		val = FALSE;
    char_u	*s1, *s2;
    char_u	buf1[NUMBUFLEN], buf2[NUMBUFLEN];

    якщо (in_vim9script()
	  && ((tv1->v_type != VAR_STRING && tv1->v_type != VAR_SPECIAL)
	   || (tv2->v_type != VAR_STRING && tv2->v_type != VAR_SPECIAL)))
    {
	semsg(_(e_cannot_compare_str_with_str),
		   vartype_name(tv1->v_type), vartype_name(tv2->v_type));
	повернути FAIL;
    }
    s1 = tv_get_string_buf(tv1, buf1);
    s2 = tv_get_string_buf(tv2, buf2);
    якщо (type != EXPR_MATCH && type != EXPR_NOMATCH)
	i = ic ? MB_STRICMP(s1, s2) : STRCMP(s1, s2);
    switch (type)
    {
	case EXPR_IS:	    якщо (in_vim9script())
			    {
				// Really check it is the same string, not just
				// the same value.
				val = tv1->vval.v_string == tv2->vval.v_string;
				break;
			    }
			    // FALLTHROUGH
	case EXPR_EQUAL:    val = (i == 0); break;
	case EXPR_ISNOT:    якщо (in_vim9script())
			    {
				// Really check it is not the same string, not
				// just a different value.
				val = tv1->vval.v_string != tv2->vval.v_string;
				break;
			    }
			    // FALLTHROUGH
	case EXPR_NEQUAL:   val = (i != 0); break;
	case EXPR_GREATER:  val = (i > 0); break;
	case EXPR_GEQUAL:   val = (i >= 0); break;
	case EXPR_SMALLER:  val = (i < 0); break;
	case EXPR_SEQUAL:   val = (i <= 0); break;

	case EXPR_MATCH:
	case EXPR_NOMATCH:
		val = pattern_match(s2, s1, ic);
		якщо (type == EXPR_NOMATCH)
		    val = !val;
		break;

	default:  break;  // avoid gcc warning
    }
    *res = val;
    повернути OK;
}
/*
 * Convert any type to a string, never give an error.
 * When "quotes" is TRUE add quotes to a string.
 * Returns an allocated string.
 */
    char_u *
typval_tostring(typval_T *arg, ціле quotes)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    char_u	*ret = NULL;

    якщо (arg == NULL)
	повернути vim_strsave((char_u *)"(does not exist)");
    якщо (!quotes && arg->v_type == VAR_STRING)
    {
	ret = vim_strsave(arg->vval.v_string == NULL ? (char_u *)""
							 : arg->vval.v_string);
    }
    інакше
    {
	ret = tv2string(arg, &tofree, numbuf, 0);
	// Make a copy if we have a value but it's not in allocated memory.
	якщо (ret != NULL && tofree == NULL)
	    ret = vim_strsave(ret);
    }
    повернути ret;
}

/*
 * Return TRUE if typeval "tv" is locked: Either that value is locked itself
 * or it refers to a List or Dictionary that is locked.
 */
    ціле
tv_islocked(typval_T *tv)
{
    повернути (tv->v_lock & VAR_LOCKED)
	|| (tv->v_type == VAR_LIST
		&& tv->vval.v_list != NULL
		&& (tv->vval.v_list->lv_lock & VAR_LOCKED))
	|| (tv->v_type == VAR_TUPLE
		&& tv->vval.v_tuple != NULL
		&& (tv->vval.v_tuple->tv_lock & VAR_LOCKED))
	|| (tv->v_type == VAR_DICT
		&& tv->vval.v_dict != NULL
		&& (tv->vval.v_dict->dv_lock & VAR_LOCKED));
}

    static ціле
func_equal(
    typval_T *tv1,
    typval_T *tv2,
    ціле	     ic)	    // ignore case
{
    char_u	*s1, *s2;
    dict_T	*d1, *d2;
    ціле		a1, a2;
    ціле		i;

    // empty and NULL function name considered the same
    s1 = tv1->v_type == VAR_FUNC ? tv1->vval.v_string
					   : partial_name(tv1->vval.v_partial);
    якщо (s1 != NULL && *s1 == NUL)
	s1 = NULL;
    s2 = tv2->v_type == VAR_FUNC ? tv2->vval.v_string
					   : partial_name(tv2->vval.v_partial);
    якщо (s2 != NULL && *s2 == NUL)
	s2 = NULL;
    якщо (s1 == NULL || s2 == NULL)
    {
	якщо (s1 != s2)
	    повернути FALSE;
    }
    інакше якщо (STRCMP(s1, s2) != 0)
	повернути FALSE;

    // empty dict and NULL dict is different
    d1 = tv1->v_type == VAR_FUNC ? NULL : tv1->vval.v_partial->pt_dict;
    d2 = tv2->v_type == VAR_FUNC ? NULL : tv2->vval.v_partial->pt_dict;
    якщо (d1 == NULL || d2 == NULL)
    {
	якщо (d1 != d2)
	    повернути FALSE;
    }
    інакше якщо (!dict_equal(d1, d2, ic))
	повернути FALSE;

    // empty list and no list considered the same
    a1 = tv1->v_type == VAR_FUNC ? 0 : tv1->vval.v_partial->pt_argc;
    a2 = tv2->v_type == VAR_FUNC ? 0 : tv2->vval.v_partial->pt_argc;
    якщо (a1 != a2)
	повернути FALSE;
    для (i = 0; i < a1; ++i)
	якщо (!tv_equal(tv1->vval.v_partial->pt_argv + i,
		      tv2->vval.v_partial->pt_argv + i, ic))
	    повернути FALSE;

    повернути TRUE;
}

/*
 * Return TRUE if "tv1" and "tv2" have the same value.
 * Compares the items just like "==" would compare them, but strings and
 * numbers are different.  Floats and numbers are also different.
 */
    ціле
tv_equal(
    typval_T *tv1,
    typval_T *tv2,
    ціле	     ic)	    // ignore case
{
    char_u	buf1[NUMBUFLEN], buf2[NUMBUFLEN];
    char_u	*s1, *s2;
    static ціле  recursive_cnt = 0;	    // catch recursive loops
    ціле		r;
    static ціле	tv_equal_recurse_limit;

    // Catch lists and dicts that have an endless loop by limiting
    // recursiveness to a limit.  We guess they are equal then.
    // A fixed limit has the problem of still taking an awful long time.
    // Reduce the limit every time running into it. That should work fine for
    // deeply linked structures that are not recursively linked and catch
    // recursiveness quickly.
    якщо (recursive_cnt == 0)
	tv_equal_recurse_limit = 1000;
    якщо (recursive_cnt >= tv_equal_recurse_limit)
    {
	--tv_equal_recurse_limit;
	повернути TRUE;
    }

    // For VAR_FUNC and VAR_PARTIAL compare the function name, bound dict and
    // arguments.
    якщо ((tv1->v_type == VAR_FUNC
		|| (tv1->v_type == VAR_PARTIAL && tv1->vval.v_partial != NULL))
	    && (tv2->v_type == VAR_FUNC
		|| (tv2->v_type == VAR_PARTIAL && tv2->vval.v_partial != NULL)))
    {
	++recursive_cnt;
	r = func_equal(tv1, tv2, ic);
	--recursive_cnt;
	повернути r;
    }

    якщо (tv1->v_type != tv2->v_type
	    && ((tv1->v_type != VAR_BOOL && tv1->v_type != VAR_SPECIAL)
		|| (tv2->v_type != VAR_BOOL && tv2->v_type != VAR_SPECIAL)))
	повернути FALSE;

    switch (tv1->v_type)
    {
	case VAR_LIST:
	    ++recursive_cnt;
	    r = list_equal(tv1->vval.v_list, tv2->vval.v_list, ic);
	    --recursive_cnt;
	    повернути r;

	case VAR_TUPLE:
	    ++recursive_cnt;
	    r = tuple_equal(tv1->vval.v_tuple, tv2->vval.v_tuple, ic);
	    --recursive_cnt;
	    повернути r;

	case VAR_DICT:
	    ++recursive_cnt;
	    r = dict_equal(tv1->vval.v_dict, tv2->vval.v_dict, ic);
	    --recursive_cnt;
	    повернути r;

	case VAR_BLOB:
	    повернути blob_equal(tv1->vval.v_blob, tv2->vval.v_blob);

	case VAR_NUMBER:
	case VAR_BOOL:
	case VAR_SPECIAL:
	    повернути tv1->vval.v_number == tv2->vval.v_number;

	case VAR_STRING:
	    s1 = tv_get_string_buf(tv1, buf1);
	    s2 = tv_get_string_buf(tv2, buf2);
	    повернути ((ic ? MB_STRICMP(s1, s2) : STRCMP(s1, s2)) == 0);

	case VAR_FLOAT:
	    повернути tv1->vval.v_float == tv2->vval.v_float;
	case VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    повернути tv1->vval.v_job == tv2->vval.v_job;
#endif
	case VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    повернути tv1->vval.v_channel == tv2->vval.v_channel;
#endif
	case VAR_INSTR:
	    повернути tv1->vval.v_instr == tv2->vval.v_instr;

	case VAR_CLASS:
	    // A class only exists once, equality is identity.
	    повернути tv1->vval.v_class == tv2->vval.v_class;

	case VAR_OBJECT:
	    ++recursive_cnt;
	    r = object_equal(tv1->vval.v_object, tv2->vval.v_object, ic);
	    --recursive_cnt;
	    повернути r;

	case VAR_PARTIAL:
	    повернути tv1->vval.v_partial == tv2->vval.v_partial;

	case VAR_FUNC:
	    повернути tv1->vval.v_string == tv2->vval.v_string;

	case VAR_TYPEALIAS:
	    повернути tv1->vval.v_typealias == tv2->vval.v_typealias;

	case VAR_UNKNOWN:
	case VAR_ANY:
	case VAR_VOID:
	    break;
    }

    // VAR_UNKNOWN can be the result of a invalid expression, let's say it
    // does not equal anything, not even itself.
    повернути FALSE;
}

/*
 * Get an option value.
 * "arg" points to the '&' or '+' before the option name.
 * "arg" is advanced to character after the option name.
 * Return OK or FAIL.
 */
    ціле
eval_option(
    char_u	**arg,
    typval_T	*rettv,	// when NULL, only check if option exists
    ціле		evaluate)
{
    char_u	*option_end;
    long	numval;
    char_u	*stringval;
    getoption_T	opt_type;
    ціле		c;
    ціле		working = (**arg == '+');    // has("+option")
    ціле		ret = OK;
    ціле		scope;

    // Isolate the option name and find its value.
    option_end = find_option_end(arg, &scope);
    якщо (option_end == NULL)
    {
	якщо (rettv != NULL)
	    semsg(_(e_option_name_missing_str), *arg);
	повернути FAIL;
    }

    якщо (!evaluate)
    {
	*arg = option_end;
	повернути OK;
    }

    c = *option_end;
    *option_end = NUL;
    opt_type = get_option_value(*arg, &numval,
			       rettv == NULL ? NULL : &stringval, NULL, scope);

    якщо (opt_type == gov_unknown)
    {
	якщо (rettv != NULL)
	    semsg(_(e_unknown_option_str), *arg);
	ret = FAIL;
    }
    інакше якщо (rettv != NULL)
    {
	rettv->v_lock = 0;
	якщо (opt_type == gov_hidden_string)
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = NULL;
	}
	інакше якщо (opt_type == gov_hidden_bool || opt_type == gov_hidden_number)
	{
	    rettv->v_type = in_vim9script() && opt_type == gov_hidden_bool
						       ? VAR_BOOL : VAR_NUMBER;
	    rettv->vval.v_number = 0;
	}
	інакше якщо (opt_type == gov_bool || opt_type == gov_number)
	{
	    якщо (in_vim9script() && opt_type == gov_bool)
	    {
		rettv->v_type = VAR_BOOL;
		rettv->vval.v_number = numval ? VVAL_TRUE : VVAL_FALSE;
	    }
	    інакше
	    {
		rettv->v_type = VAR_NUMBER;
		rettv->vval.v_number = numval;
	    }
	}
	інакше				// string option
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = stringval;
	}
    }
    інакше якщо (working && (opt_type == gov_hidden_bool
			|| opt_type == gov_hidden_number
			|| opt_type == gov_hidden_string))
	ret = FAIL;

    *option_end = c;		    // put back for error messages
    *arg = option_end;

    повернути ret;
}

/*
 * Allocate a variable for a number constant.  Also deals with "0z" for blob.
 * Return OK or FAIL.
 */
    ціле
eval_number(
	char_u	    **arg,
	typval_T    *rettv,
	ціле	    evaluate,
	ціле	    want_string)
{
    ціле		len;
    ціле		skip_quotes = !in_old_script(4);
    char_u	*p;
    ціле		get_float = FALSE;

    // We accept a float when the format matches
    // "[0-9]\+\.[0-9]\+\([eE][+-]\?[0-9]\+\)\?".  This is very
    // strict to avoid backwards compatibility problems.
    // With script version 2 and later the leading digit can be
    // omitted.
    // Don't look for a float after the "." operator, so that
    // ":let vers = 1.2.3" doesn't fail.
    якщо (**arg == '.')
	p = *arg;
    інакше
    {
	p = *arg + 1;
	якщо (skip_quotes)
	    для (;;)
	    {
		якщо (*p == '\'')
		    ++p;
		якщо (!vim_isdigit(*p))
		    break;
		p = skipdigits(p);
	    }
	інакше
	    p = skipdigits(p);
    }
    якщо (!want_string && p[0] == '.' && vim_isdigit(p[1]))
    {
	get_float = TRUE;
	p = skipdigits(p + 2);
	якщо (*p == 'e' || *p == 'E')
	{
	    ++p;
	    якщо (*p == '-' || *p == '+')
		++p;
	    якщо (!vim_isdigit(*p))
		get_float = FALSE;
	    інакше
		p = skipdigits(p + 1);
	}
	якщо (ASCII_ISALPHA(*p) || *p == '.')
	    get_float = FALSE;
    }
    якщо (get_float)
    {
	float_T	f;

	*arg += string2float(*arg, &f, skip_quotes);
	якщо (evaluate)
	{
	    rettv->v_type = VAR_FLOAT;
	    rettv->vval.v_float = f;
	}
    }
    інакше
    якщо (**arg == '0' && ((*arg)[1] == 'z' || (*arg)[1] == 'Z'))
    {
	char_u  *bp;
	blob_T  *blob = NULL;  // init for gcc

	// Blob constant: 0z0123456789abcdef
	якщо (evaluate)
	    blob = blob_alloc();
	для (bp = *arg + 2; vim_isxdigit(bp[0]); bp += 2)
	{
	    якщо (!vim_isxdigit(bp[1]))
	    {
		якщо (blob != NULL)
		{
		    emsg(_(e_blob_literal_should_have_an_even_number_of_hex_characters));
		    ga_clear(&blob->bv_ga);
		    VIM_CLEAR(blob);
		}
		повернути FAIL;
	    }
	    якщо (blob != NULL)
		ga_append(&blob->bv_ga,
			     (hex2nr(*bp) << 4) + hex2nr(*(bp+1)));
	    якщо (bp[2] == '.' && vim_isxdigit(bp[3]))
		++bp;
	}
	якщо (blob != NULL)
	    rettv_blob_set(rettv, blob);
	*arg = bp;
    }
    інакше
    {
	varnumber_T	n;

	// decimal, hex or octal number
	vim_str2nr(*arg, NULL, &len, skip_quotes
		      ? STR2NR_NO_OCT + STR2NR_QUOTE
		      : STR2NR_ALL, &n, NULL, 0, TRUE, NULL);
	якщо (len == 0)
	{
	    якщо (evaluate)
		semsg(_(e_invalid_expression_str), *arg);
	    повернути FAIL;
	}
	*arg += len;
	якщо (evaluate)
	{
	    rettv->v_type = VAR_NUMBER;
	    rettv->vval.v_number = n;
	}
    }
    повернути OK;
}

/*
 * Evaluate a string constant and put the result in "rettv".
 * "*arg" points to the double quote or to after it when "interpolate" is TRUE.
 * When "interpolate" is TRUE reduce "{{" to "{", reduce "}}" to "}" and stop
 * at a single "{".
 * Return OK or FAIL.
 */
    ціле
eval_string(char_u **arg, typval_T *rettv, ціле evaluate, ціле interpolate)
{
    char_u	*p;
    char_u	*end;
    ціле		extra = interpolate ? 1 : 0;
    ціле		off = interpolate ? 0 : 1;
    ціле		len;

    // Find the end of the string, skipping backslashed characters.
    для (p = *arg + off; *p != NUL && *p != '"'; MB_PTR_ADV(p))
    {
	якщо (*p == '\\' && p[1] != NUL)
	{
	    ++p;
	    // A "\<x>" form occupies at least 4 characters, and produces up
	    // to 9 characters (6 for the char and 3 for a modifier):
	    // reserve space for 5 extra.
	    якщо (*p == '<')
	    {
		ціле		modifiers = 0;
		ціле		flags = FSK_KEYCODE | FSK_IN_STRING;

		extra += 5;

		// Skip to the '>' to avoid using '{' inside for string
		// interpolation.
		якщо (p[1] != '*')
		    flags |= FSK_SIMPLIFY;
		якщо (find_special_key(&p, &modifiers, flags, NULL) != 0)
		    --p;  // leave "p" on the ">"
	    }
	}
	інакше якщо (interpolate && (*p == '{' || *p == '}'))
	{
	    якщо (*p == '{' && p[1] != '{') // start of expression
		break;
	    ++p;
	    якщо (p[-1] == '}' && *p != '}') // single '}' is an error
	    {
		semsg(_(e_stray_closing_curly_str), *arg);
		повернути FAIL;
	    }
	    --extra;  // "{{" becomes "{", "}}" becomes "}"
	}
    }

    якщо (*p != '"' && !(interpolate && *p == '{'))
    {
	semsg(_(e_missing_double_quote_str), *arg);
	повернути FAIL;
    }

    // If only parsing, set *arg and return here
    якщо (!evaluate)
    {
	*arg = p + off;
	повернути OK;
    }

    // Copy the string into allocated memory, handling backslashed
    // characters.
    rettv->v_type = VAR_STRING;
    len = (ціле)(p - *arg + extra);
    rettv->vval.v_string = alloc(len);
    якщо (rettv->vval.v_string == NULL)
	повернути FAIL;
    end = rettv->vval.v_string;

    для (p = *arg + off; *p != NUL && *p != '"'; )
    {
	якщо (*p == '\\')
	{
	    switch (*++p)
	    {
		case 'b': *end++ = BS; ++p; break;
		case 'e': *end++ = ESC; ++p; break;
		case 'f': *end++ = FF; ++p; break;
		case 'n': *end++ = NL; ++p; break;
		case 'r': *end++ = CAR; ++p; break;
		case 't': *end++ = TAB; ++p; break;

		case 'X': // hex: "\x1", "\x12"
		case 'x':
		case 'u': // Unicode: "\u0023"
		case 'U':
			  якщо (vim_isxdigit(p[1]))
			  {
			      ціле	n, nr;
			      ціле	c = SAFE_toupper(*p);

			      якщо (c == 'X')
				  n = 2;
			      інакше якщо (*p == 'u')
				  n = 4;
			      інакше
				  n = 8;
			      nr = 0;
			      поки (--n >= 0 && vim_isxdigit(p[1]))
			      {
				  ++p;
				  nr = (nr << 4) + hex2nr(*p);
			      }
			      ++p;
			      // For "\u" store the number according to
			      // 'encoding'.
			      якщо (c != 'X')
				  end += (*mb_char2bytes)(nr, end);
			      інакше
				  *end++ = nr;
			  }
			  break;

			  // octal: "\1", "\12", "\123"
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7': *end = *p++ - '0';
			  якщо (*p >= '0' && *p <= '7')
			  {
			      *end = (*end << 3) + *p++ - '0';
			      якщо (*p >= '0' && *p <= '7')
				  *end = (*end << 3) + *p++ - '0';
			  }
			  ++end;
			  break;

			  // Special key, e.g.: "\<C-W>"
		case '<':
			  {
			      ціле flags = FSK_KEYCODE | FSK_IN_STRING;

			      якщо (p[1] != '*')
				  flags |= FSK_SIMPLIFY;
			      extra = trans_special(&p, end, flags, FALSE, NULL);
			      якщо (extra != 0)
			      {
				  end += extra;
				  якщо (end >= rettv->vval.v_string + len)
				      iemsg("eval_string() used more space than allocated");
				  break;
			      }
			  }
			  // FALLTHROUGH

		default: MB_COPY_CHAR(p, end);
			  break;
	    }
	}
	інакше
	{
	    якщо (interpolate && (*p == '{' || *p == '}'))
	    {
		якщо (*p == '{' && p[1] != '{') // start of expression
		    break;
		++p;  // reduce "{{" to "{" and "}}" to "}"
	    }
	    MB_COPY_CHAR(p, end);
	}
    }
    *end = NUL;
    якщо (*p == '"' && !interpolate)
	++p;
    *arg = p;

    повернути OK;
}

/*
 * Allocate a variable for a 'str''ing' constant.
 * When "interpolate" is TRUE reduce "{{" to "{" and stop at a single "{".
 * Return OK when a "rettv" was set to the string.
 * Return FAIL on error, "rettv" is not set.
 */
    ціле
eval_lit_string(char_u **arg, typval_T *rettv, ціле evaluate, ціле interpolate)
{
    char_u	*p;
    char_u	*str;
    ціле		reduce = interpolate ? -1 : 0;
    ціле		off = interpolate ? 0 : 1;

    // Find the end of the string, skipping ''.
    для (p = *arg + off; *p != NUL; MB_PTR_ADV(p))
    {
	якщо (*p == '\'')
	{
	    якщо (p[1] != '\'')
		break;
	    ++reduce;
	    ++p;
	}
	інакше якщо (interpolate)
	{
	    якщо (*p == '{')
	    {
		якщо (p[1] != '{')
		    break;
		++p;
		++reduce;
	    }
	    інакше якщо (*p == '}')
	    {
		++p;
		якщо (*p != '}')
		{
		    semsg(_(e_stray_closing_curly_str), *arg);
		    повернути FAIL;
		}
		++reduce;
	    }
	}
    }

    якщо (*p != '\'' && !(interpolate && *p == '{'))
    {
	semsg(_(e_missing_single_quote_str), *arg);
	повернути FAIL;
    }

    // If only parsing return after setting "*arg"
    якщо (!evaluate)
    {
	*arg = p + off;
	повернути OK;
    }

    // Copy the string into allocated memory, handling '' to ' reduction and
    // any expressions.
    str = alloc((p - *arg) - reduce);
    якщо (str == NULL)
	повернути FAIL;
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = str;

    для (p = *arg + off; *p != NUL; )
    {
	якщо (*p == '\'')
	{
	    якщо (p[1] != '\'')
		break;
	    ++p;
	}
	інакше якщо (interpolate && (*p == '{' || *p == '}'))
	{
	    якщо (*p == '{' && p[1] != '{')
		break;
	    ++p;
	}
	MB_COPY_CHAR(p, str);
    }
    *str = NUL;
    *arg = p + off;

    повернути OK;
}

/*
 * Evaluate a single or double quoted string possibly containing expressions.
 * "arg" points to the '$'.  The result is put in "rettv".
 * Returns OK or FAIL.
 */
    ціле
eval_interp_string(char_u **arg, typval_T *rettv, ціле evaluate)
{
    typval_T	tv;
    ціле		ret = OK;
    ціле		quote;
    garray_T	ga;
    char_u	*p;

    ga_init2(&ga, 1, 80);

    // *arg is on the '$' character, move it to the first string character.
    ++*arg;
    quote = **arg;
    ++*arg;

    для (;;)
    {
	// Get the string up to the matching quote or to a single '{'.
	// "arg" is advanced to either the quote or the '{'.
	якщо (quote == '"')
	    ret = eval_string(arg, &tv, evaluate, TRUE);
	інакше
	    ret = eval_lit_string(arg, &tv, evaluate, TRUE);
	якщо (ret == FAIL)
	    break;
	якщо (evaluate)
	{
	    ga_concat(&ga, tv.vval.v_string);
	    clear_tv(&tv);
	}

	якщо (**arg != '{')
	{
	    // found terminating quote
	    ++*arg;
	    break;
	}
	p = eval_one_expr_in_str(*arg, &ga, evaluate);
	якщо (p == NULL)
	{
	    ret = FAIL;
	    break;
	}
	*arg = p;
    }

    rettv->v_type = VAR_STRING;
    якщо (ret == FAIL || !evaluate || ga_append(&ga, NUL) == FAIL)
    {
	ga_clear(&ga);
	rettv->vval.v_string = NULL;
	повернути ret;
    }

    rettv->vval.v_string = ga.ga_data;
    повернути OK;
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
    ціле		copyID)
{
    повернути echo_string_core(tv, tofree, numbuf, copyID, FALSE, TRUE, FALSE);
}

/*
 * Get the value of an environment variable.
 * "arg" is pointing to the '$'.  It is advanced to after the name.
 * If the environment variable was not set, silently assume it is empty.
 * Return FAIL if the name is invalid.
 */
    ціле
eval_env_var(char_u **arg, typval_T *rettv, ціле evaluate)
{
    char_u	*string = NULL;
    ціле		len;
    ціле		cc;
    char_u	*name;
    ціле		mustfree = FALSE;

    ++*arg;
    name = *arg;
    len = get_env_len(arg);
    якщо (evaluate)
    {
	якщо (len == 0)
	    повернути FAIL; // invalid empty name

	cc = name[len];
	name[len] = NUL;
	// first try vim_getenv(), fast for normal environment vars
	string = vim_getenv(name, &mustfree);
	якщо (string != NULL && *string != NUL)
	{
	    якщо (!mustfree)
		string = vim_strsave(string);
	}
	інакше
	{
	    якщо (mustfree)
		vim_free(string);

	    // next try expanding things like $VIM and ${HOME}
	    string = expand_env_save(name - 1);
	    якщо (string != NULL && *string == '$')
		VIM_CLEAR(string);
	}
	name[len] = cc;

	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = string;
	rettv->v_lock = 0;
    }

    повернути OK;
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
    ціле		did_emsg_before = did_emsg;

    якщо (argvars[0].v_type != VAR_STRING || !in_vim9script())
	lnum = (linenr_T)tv_get_number_chk(&argvars[0], NULL);
    якщо (lnum <= 0 && did_emsg_before == did_emsg
					    && argvars[0].v_type != VAR_NUMBER)
    {
	ціле	fnum;
	pos_T	*fp;

	// no valid number, try using arg like line()
	fp = var2fpos(&argvars[0], TRUE, &fnum, FALSE);
	якщо (fp != NULL)
	    lnum = fp->lnum;
    }
    повернути lnum;
}

/*
 * Get the lnum from the first argument.
 * Also accepts "$", then "buf" is used.
 * Returns 0 on error.
 */
    linenr_T
tv_get_lnum_buf(typval_T *argvars, buf_T *buf)
{
    якщо (argvars[0].v_type == VAR_STRING
	    && argvars[0].vval.v_string != NULL
	    && argvars[0].vval.v_string[0] == '$'
	    && argvars[0].vval.v_string[1] == NUL
	    && buf != NULL)
	повернути buf->b_ml.ml_line_count;
    повернути (linenr_T)tv_get_number_chk(&argvars[0], NULL);
}

/*
 * Get buffer by number or pattern.
 */
    buf_T *
tv_get_buf(typval_T *tv, ціле curtab_only)
{
    char_u	*name = tv->vval.v_string;
    buf_T	*buf;

    якщо (tv->v_type == VAR_NUMBER)
	повернути buflist_findnr((ціле)tv->vval.v_number);
    якщо (tv->v_type != VAR_STRING)
	повернути NULL;
    якщо (name == NULL || *name == NUL)
	повернути curbuf;
    якщо (name[0] == '$' && name[1] == NUL)
	повернути lastbuf;

    buf = buflist_find_by_name(name, curtab_only);

    // If not found, try expanding the name, like done for bufexists().
    якщо (buf == NULL)
	buf = find_buffer(tv);

    повернути buf;
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
    якщо (buf == NULL
	    && tv->v_type != VAR_NUMBER
	    && tv->v_type != VAR_STRING)
	// issue errmsg for type error
	(void)tv_get_number(tv);
    повернути buf;
}

/*
 * Check that repeating "slen" items "n" times, does not
 * overflow the int used for the resulting length.
 */
    ціле
check_repeat_count(varnumber_T slen, varnumber_T n)
{
    // Use a division so that the multiplication cannot overflow.
    якщо (n <= (varnumber_T)INT_MAX / slen)
	повернути OK;

    char	buf[NUMBUFLEN];

    vim_snprintf(buf, sizeof(buf), "%lld", (long long)n);
    semsg(_(e_val_too_large), buf);
    повернути FAIL;
}

#endif // FEAT_EVAL
