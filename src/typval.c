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

#もし defined(FEAT_EVAL)

/*
 * Allocate memory for a variable type-value, and make it empty (0 or NULL
 * value).
 */
    typval_T *
alloc_tv(void)
{
    戻る ALLOC_CLEAR_ONE(typval_T);
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
    もし (rettv != NULL)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = s;
    }
    否则
	vim_free(s);
    戻る rettv;
}

/*
 * Free the memory for a variable type-value.
 */
    void
free_tv(typval_T *varp)
{
    もし (varp == NULL)
	戻る;

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
    もし (varp == NULL)
	戻る;

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
    もし (varp != NULL)
	CLEAR_POINTER(varp);
}

    static varnumber_T
tv_get_bool_or_number_chk(
	typval_T    *varp,
	整数	    *denote,
	整数	    want_bool,
	整数	    vim9_string_error) // in Vim9 using a string is an error
{
    varnumber_T	n = 0L;

    switch (varp->v_type)
    {
	case VAR_NUMBER:
	    もし (in_vim9script() && want_bool && varp->vval.v_number != 0
						   && varp->vval.v_number != 1)
	    {
		semsg(_(e_using_number_as_bool_nr), varp->vval.v_number);
		break;
	    }
	    戻る varp->vval.v_number;
	case VAR_FLOAT:
	    emsg(_(e_using_float_as_number));
	    break;
	case VAR_FUNC:
	case VAR_PARTIAL:
	    emsg(_(e_using_funcref_as_number));
	    break;
	case VAR_STRING:
	    もし (vim9_string_error && in_vim9script())
	    {
		emsg_using_string_as(varp, !want_bool);
		break;
	    }
	    もし (varp->vval.v_string != NULL)
		vim_str2nr(varp->vval.v_string, NULL, NULL,
					 STR2NR_ALL, &n, NULL, 0, FALSE, NULL);
	    戻る n;
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
	    もし (!want_bool && in_vim9script())
	    {
		もし (varp->v_type == VAR_BOOL)
		    emsg(_(e_using_bool_as_number));
		否则
		    emsg(_(e_using_special_as_number));
		break;
	    }
	    戻る varp->vval.v_number == VVAL_TRUE ? 1 : 0;
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
		もし (varp->vval.v_object == NULL)
		    emsg(_(e_using_object_as_string));
		否则
		{
		    class_T *cl = varp->vval.v_object->obj_class;
		    もし (cl != NULL && IS_ENUM(cl))
			semsg(_(e_using_enum_str_as_number), cl->class_name.string);
		    否则
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
    もし (denote == NULL)		// useful for values that must be unsigned
	n = -1;
    否则
	*denote = TRUE;
    戻る n;
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
    整数		error = FALSE;

    戻る tv_get_number_chk(varp, &error);	// return 0L on error
}

/*
 * Like tv_get_number() but in Vim9 script do convert a number in a string to a
 * number without giving an error.
 */
    varnumber_T
tv_to_number(typval_T *varp)
{
    整数		error = FALSE;

    戻る tv_get_bool_or_number_chk(varp, &error, FALSE, FALSE);
}

    varnumber_T
tv_get_number_chk(typval_T *varp, 整数 *denote)
{
    戻る tv_get_bool_or_number_chk(varp, denote, FALSE, TRUE);
}

/*
 * Get the boolean value of "varp".  This is like tv_get_number_chk(),
 * but in Vim9 script accepts Number (0 and 1) and Bool/Special.
 */
    varnumber_T
tv_get_bool(typval_T *varp)
{
    戻る tv_get_bool_or_number_chk(varp, NULL, TRUE, TRUE);
}

/*
 * Get the boolean value of "varp".  This is like tv_get_number_chk(),
 * but in Vim9 script accepts Number and Bool.
 */
    varnumber_T
tv_get_bool_chk(typval_T *varp, 整数 *denote)
{
    戻る tv_get_bool_or_number_chk(varp, denote, TRUE, TRUE);
}

    static float_T
tv_get_float_chk(typval_T *varp, 整数 *error)
{
    switch (varp->v_type)
    {
	case VAR_NUMBER:
	    戻る (float_T)(varp->vval.v_number);
	case VAR_FLOAT:
	    戻る varp->vval.v_float;
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
    もし (error != NULL)
	*error = TRUE;
    戻る 0;
}

    float_T
tv_get_float(typval_T *varp)
{
    戻る tv_get_float_chk(varp, NULL);
}

/*
 * Give an error and return FAIL unless "args[idx]" is unknown
 */
    整数
check_for_unknown_arg(typval_T *args, 整数 idx)
{
    もし (args[idx].v_type != VAR_UNKNOWN)
    {
	semsg(_(e_too_many_arguments), idx + 1);
	戻る FAIL;
    }
    戻る OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string.
 */
    整数
check_for_string_arg(typval_T *args, 整数 idx)
{
    もし (args[idx].v_type != VAR_STRING)
    {
	semsg(_(e_string_required_for_argument_nr), idx + 1);
	戻る FAIL;
    }
    戻る OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a non-empty string.
 */
    整数
check_for_nonempty_string_arg(typval_T *args, 整数 idx)
{
    もし (check_for_string_arg(args, idx) == FAIL)
	戻る FAIL;
    もし (args[idx].vval.v_string == NULL || *args[idx].vval.v_string == NUL)
    {
	semsg(_(e_non_empty_string_required_for_argument_nr), idx + 1);
	戻る FAIL;
    }
    戻る OK;
}

/*
 * Check for an optional string argument at 'idx'
 */
    整数
check_for_opt_string_arg(typval_T *args, 整数 idx)
{
    戻る (args[idx].v_type == VAR_UNKNOWN
	    || check_for_string_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a number.
 */
    整数
check_for_number_arg(typval_T *args, 整数 idx)
{
    もし (args[idx].v_type != VAR_NUMBER)
    {
	semsg(_(e_number_required_for_argument_nr), idx + 1);
	戻る FAIL;
    }
    戻る OK;
}

/*
 * Check for an optional number argument at 'idx'
 */
    整数
check_for_opt_number_arg(typval_T *args, 整数 idx)
{
    戻る (args[idx].v_type == VAR_UNKNOWN
	    || check_for_number_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a float or a number.
 */
    整数
check_for_float_or_nr_arg(typval_T *args, 整数 idx)
{
    もし (args[idx].v_type != VAR_FLOAT && args[idx].v_type != VAR_NUMBER)
    {
	semsg(_(e_float_or_number_required_for_argument_nr), idx + 1);
	戻る FAIL;
    }
    戻る OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a bool.
 */
    整数
check_for_bool_arg(typval_T *args, 整数 idx)
{
    もし (args[idx].v_type != VAR_BOOL
	    && !(args[idx].v_type == VAR_NUMBER
		&& (args[idx].vval.v_number == 0
		    || args[idx].vval.v_number == 1)))
    {
	semsg(_(e_bool_required_for_argument_nr), idx + 1);
	戻る FAIL;
    }
    戻る OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a bool or a number.
 */
    static 整数
check_for_bool_or_number_arg(typval_T *args, 整数 idx)
{
    もし (args[idx].v_type != VAR_BOOL && args[idx].v_type != VAR_NUMBER)
    {
	semsg(_(e_bool_or_number_required_for_argument_nr), idx + 1);
	戻る FAIL;
    }
    戻る OK;
}

/*
 * Check for an optional bool argument at 'idx'.
 * Return FAIL if the type is wrong.
 */
    整数
check_for_opt_bool_arg(typval_T *args, 整数 idx)
{
    もし (args[idx].v_type == VAR_UNKNOWN)
	戻る OK;
    戻る check_for_bool_arg(args, idx);
}

/*
 * Check for an optional bool or number argument at 'idx'.
 * Return FAIL if the type is wrong.
 */
    整数
check_for_opt_bool_or_number_arg(typval_T *args, 整数 idx)
{
    もし (args[idx].v_type == VAR_UNKNOWN)
	戻る OK;
    戻る check_for_bool_or_number_arg(args, idx);
}

/*
 * Check for an optional bool or dict argument at 'idx'.
 * Return FAIL if the type is wrong.
 */
    整数
check_for_opt_bool_or_dict_arg(typval_T *args, 整数 idx)
{
    もし (args[idx].v_type == VAR_UNKNOWN || args[idx].v_type == VAR_DICT)
	戻る OK;
    戻る check_for_bool_arg(args, idx);
}

/*
 * Give an error and return FAIL unless "args[idx]" is a blob.
 */
    整数
check_for_blob_arg(typval_T *args, 整数 idx)
{
    もし (args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_blob_required_for_argument_nr), idx + 1);
	戻る FAIL;
    }
    戻る OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list.
 */
    整数
check_for_list_arg(typval_T *args, 整数 idx)
{
    もし (args[idx].v_type != VAR_LIST)
    {
	semsg(_(e_list_required_for_argument_nr), idx + 1);
	戻る FAIL;
    }
    戻る OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a non-NULL list.
 */
    整数
check_for_nonnull_list_arg(typval_T *args, 整数 idx)
{
    もし (check_for_list_arg(args, idx) == FAIL)
	戻る FAIL;

    もし (args[idx].vval.v_list == NULL)
    {
	semsg(_(e_non_null_list_required_for_argument_nr), idx + 1);
	戻る FAIL;
    }
    戻る OK;
}

/*
 * Check for an optional list argument at 'idx'
 */
    整数
check_for_opt_list_arg(typval_T *args, 整数 idx)
{
    戻る (args[idx].v_type == VAR_UNKNOWN
	    || check_for_list_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a tuple.
 */
    整数
check_for_tuple_arg(typval_T *args, 整数 idx)
{
    もし (args[idx].v_type != VAR_TUPLE)
    {
	semsg(_(e_tuple_required_for_argument_nr), idx + 1);
	戻る FAIL;
    }
    戻る OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a dict.
 */
    整数
check_for_dict_arg(typval_T *args, 整数 idx)
{
    もし (args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_dict_required_for_argument_nr), idx + 1);
	戻る FAIL;
    }
    戻る OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a non-NULL dict.
 */
    整数
check_for_nonnull_dict_arg(typval_T *args, 整数 idx)
{
    もし (check_for_dict_arg(args, idx) == FAIL)
	戻る FAIL;

    もし (args[idx].vval.v_dict == NULL)
    {
	semsg(_(e_non_null_dict_required_for_argument_nr), idx + 1);
	戻る FAIL;
    }
    戻る OK;
}

/*
 * Check for an optional dict argument at 'idx'
 */
    整数
check_for_opt_dict_arg(typval_T *args, 整数 idx)
{
    戻る (args[idx].v_type == VAR_UNKNOWN
	    || check_for_dict_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Check for an optional non-NULL dict argument at 'idx'
 */
    整数
check_for_opt_nonnull_dict_arg(typval_T *args, 整数 idx)
{
    戻る (args[idx].v_type == VAR_UNKNOWN
	    || check_for_nonnull_dict_arg(args, idx) != FAIL) ? OK : FAIL;
}

#もし defined(FEAT_JOB_CHANNEL)
/*
 * Give an error and return FAIL unless "args[idx]" is a channel or a job.
 */
    整数
check_for_chan_or_job_arg(typval_T *args, 整数 idx)
{
    もし (args[idx].v_type != VAR_CHANNEL && args[idx].v_type != VAR_JOB)
    {
	semsg(_(e_chan_or_job_required_for_argument_nr), idx + 1);
	戻る FAIL;
    }
    戻る OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an optional channel or a
 * job.
 */
    整数
check_for_opt_chan_or_job_arg(typval_T *args, 整数 idx)
{
    戻る (args[idx].v_type == VAR_UNKNOWN
	    || check_for_chan_or_job_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a job.
 */
    整数
check_for_job_arg(typval_T *args, 整数 idx)
{
    もし (args[idx].v_type != VAR_JOB)
    {
	semsg(_(e_job_required_for_argument_nr), idx + 1);
	戻る FAIL;
    }
    戻る OK;
}

/*
 * Check for an optional job argument at 'idx'.
 */
    整数
check_for_opt_job_arg(typval_T *args, 整数 idx)
{
    戻る (args[idx].v_type == VAR_UNKNOWN
	    || check_for_job_arg(args, idx) != FAIL) ? OK : FAIL;
}
#否则
/*
 * Give an error and return FAIL unless "args[idx]" is an optional channel or a
 * job.  Used without the +channel feature, thus only VAR_UNKNOWN is accepted.
 */
    整数
check_for_opt_chan_or_job_arg(typval_T *args, 整数 idx)
{
    戻る args[idx].v_type == VAR_UNKNOWN ? OK : FAIL;
}
#endif

/*
 * Give an error and return FAIL unless "args[idx]" is a string or
 * a number.
 */
    整数
check_for_string_or_number_arg(typval_T *args, 整数 idx)
{
    もし (args[idx].v_type != VAR_STRING && args[idx].v_type != VAR_NUMBER)
    {
	semsg(_(e_string_or_number_required_for_argument_nr), idx + 1);
	戻る FAIL;
    }
    戻る OK;
}

/*
 * Check for an optional string or number argument at 'idx'.
 */
    整数
check_for_opt_string_or_number_arg(typval_T *args, 整数 idx)
{
    戻る (args[idx].v_type == VAR_UNKNOWN
	    || check_for_string_or_number_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a buffer number.
 * Buffer number can be a number or a string.
 */
    整数
check_for_buffer_arg(typval_T *args, 整数 idx)
{
    戻る check_for_string_or_number_arg(args, idx);
}

/*
 * Check for an optional buffer argument at 'idx'
 */
    整数
check_for_opt_buffer_arg(typval_T *args, 整数 idx)
{
    戻る (args[idx].v_type == VAR_UNKNOWN
	    || check_for_buffer_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a line number.
 * Line number can be a number or a string.
 */
    整数
check_for_lnum_arg(typval_T *args, 整数 idx)
{
    戻る check_for_string_or_number_arg(args, idx);
}

/*
 * Check for an optional line number argument at 'idx'
 */
    整数
check_for_opt_lnum_arg(typval_T *args, 整数 idx)
{
    戻る (args[idx].v_type == VAR_UNKNOWN
	    || check_for_lnum_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a blob.
 */
    整数
check_for_string_or_blob_arg(typval_T *args, 整数 idx)
{
    もし (args[idx].v_type != VAR_STRING && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_string_or_blob_required_for_argument_nr), idx + 1);
	戻る FAIL;
    }
    戻る OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a list.
 */
    整数
check_for_string_or_list_arg(typval_T *args, 整数 idx)
{
    もし (args[idx].v_type != VAR_STRING && args[idx].v_type != VAR_LIST)
    {
	semsg(_(e_string_or_list_required_for_argument_nr), idx + 1);
	戻る FAIL;
    }
    戻る OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string, a list, a
 * tuple or a blob.
 */
    整数
check_for_string_or_list_or_tuple_or_blob_arg(typval_T *args, 整数 idx)
{
    もし (args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_string_list_tuple_or_blob_required_for_argument_nr), idx + 1);
	戻る FAIL;
    }
    戻る OK;
}

/*
 * Check for an optional string or list argument at 'idx'
 */
    整数
check_for_opt_string_or_list_arg(typval_T *args, 整数 idx)
{
    戻る (args[idx].v_type == VAR_UNKNOWN
	    || check_for_string_or_list_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a dict.
 */
    整数
check_for_string_or_dict_arg(typval_T *args, 整数 idx)
{
    もし (args[idx].v_type != VAR_STRING && args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_string_or_dict_required_for_argument_nr), idx + 1);
	戻る FAIL;
    }
    戻る OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a number
 * or a list.
 */
    整数
check_for_string_or_number_or_list_arg(typval_T *args, 整数 idx)
{
    もし (args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_NUMBER
	    && args[idx].v_type != VAR_LIST)
    {
	semsg(_(e_string_number_or_list_required_for_argument_nr), idx + 1);
	戻る FAIL;
    }
    戻る OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an optional string
 * or number or a list
 */
    整数
check_for_opt_string_or_number_or_list_arg(typval_T *args, 整数 idx)
{
    戻る (args[idx].v_type == VAR_UNKNOWN
	    || check_for_string_or_number_or_list_arg(args, idx)
							!= FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string, a number, a
 * list, a tuple or a blob.
 */
    整数
check_for_repeat_func_arg(typval_T *args, 整数 idx)
{
    もし (args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_NUMBER
	    && args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_repeatable_type_required_for_argument_nr), idx + 1);
	戻る FAIL;
    }
    戻る OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string, a list, a
 * tuple or a dict.
 */
    整数
check_for_string_list_tuple_or_dict_arg(typval_T *args, 整数 idx)
{
    もし (args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_string_list_tuple_or_dict_required_for_argument_nr), idx + 1);
	戻る FAIL;
    }
    戻る OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string
 * or a function reference.
 */
    整数
check_for_string_or_func_arg(typval_T *args, 整数 idx)
{
    もし (args[idx].v_type != VAR_PARTIAL
	    && args[idx].v_type != VAR_FUNC
	    && args[idx].v_type != VAR_STRING)
    {
	semsg(_(e_string_or_function_required_for_argument_nr), idx + 1);
	戻る FAIL;
    }
    戻る OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list or a blob.
 */
    整数
check_for_list_or_blob_arg(typval_T *args, 整数 idx)
{
    もし (args[idx].v_type != VAR_LIST && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_list_or_blob_required_for_argument_nr), idx + 1);
	戻る FAIL;
    }
    戻る OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list or a tuple.
 */
    整数
check_for_list_or_tuple_arg(typval_T *args, 整数 idx)
{
    もし (args[idx].v_type != VAR_LIST && args[idx].v_type != VAR_TUPLE)
    {
	semsg(_(e_list_or_tuple_required_for_argument_nr), idx + 1);
	戻る FAIL;
    }
    戻る OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list, a tuple or a
 * blob.
 */
    整数
check_for_list_or_tuple_or_blob_arg(typval_T *args, 整数 idx)
{
    もし (args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_list_or_tuple_or_blob_required_for_argument_nr), idx + 1);
	戻る FAIL;
    }
    戻る OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list, a tuple or a
 * dict
 */
    整数
check_for_list_or_tuple_or_dict_arg(typval_T *args, 整数 idx)
{
    もし (args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_list_or_tuple_or_dict_required_for_argument_nr), idx + 1);
	戻る FAIL;
    }
    戻る OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list or dict or a
 * blob.
 */
    整数
check_for_list_or_dict_or_blob_arg(typval_T *args, 整数 idx)
{
    もし (args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_DICT
	    && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_list_dict_or_blob_required_for_argument_nr), idx + 1);
	戻る FAIL;
    }
    戻る OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list, a tuple, a dict,
 * a blob or a string.
 */
    整数
check_for_list_tuple_dict_blob_or_string_arg(typval_T *args, 整数 idx)
{
    もし (args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_DICT
	    && args[idx].v_type != VAR_BLOB
	    && args[idx].v_type != VAR_STRING)
    {
	semsg(_(e_list_tuple_dict_blob_or_string_required_for_argument_nr), idx + 1);
	戻る FAIL;
    }
    戻る OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an optional buffer
 * number or a dict.
 */
    整数
check_for_opt_buffer_or_dict_arg(typval_T *args, 整数 idx)
{
    もし (args[idx].v_type != VAR_UNKNOWN
	    && args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_NUMBER
	    && args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_string_required_for_argument_nr), idx + 1);
	戻る FAIL;
    }
    戻る OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an object.
 */
    整数
check_for_object_arg(typval_T *args, 整数 idx)
{
    もし (args[idx].v_type != VAR_OBJECT)
    {
	semsg(_(e_object_required_for_argument_nr), idx + 1);
	戻る FAIL;
    }
    戻る OK;
}

/*
 * Returns TRUE if "tv" is a type alias for a class
 */
    static 整数
tv_class_alias(typval_T *tv)
{
    戻る tv->v_type == VAR_TYPEALIAS &&
			tv->vval.v_typealias->ta_type->tt_type == VAR_OBJECT;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a class
 * or class typealias.
 */
    整数
check_for_class_or_typealias_args(typval_T *args, 整数 idx)
{
    ために (整数 i = idx; args[i].v_type != VAR_UNKNOWN; ++i)
    {
	もし (args[i].v_type != VAR_CLASS && !tv_class_alias(&args[idx]))
	{
	    semsg(_(e_class_or_typealias_required_for_argument_nr), i + 1);
	    戻る FAIL;
	}
    }
    戻る OK;
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

    戻る tv_get_string_buf(varp, mybuf);
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

    戻る res != NULL ? res : (char_u *)"";
}

    char_u *
tv_get_string_buf(typval_T *varp, char_u *buf)
{
    char_u	*res = tv_get_string_buf_chk(varp, buf);

    戻る res != NULL ? res : (char_u *)"";
}

/*
 * Careful: This uses a single, static buffer.  YOU CAN ONLY USE IT ONCE!
 */
    char_u *
tv_get_string_chk(typval_T *varp)
{
    static char_u   mybuf[NUMBUFLEN];

    戻る tv_get_string_buf_chk(varp, mybuf);
}

    char_u *
tv_get_string_buf_chk(typval_T *varp, char_u *buf)
{
    戻る tv_get_string_buf_chk_strict(varp, buf, FALSE);
}

    char_u *
tv_get_string_buf_chk_strict(typval_T *varp, char_u *buf, 整数 strict)
{
    switch (varp->v_type)
    {
	case VAR_NUMBER:
	    もし (strict)
	    {
		emsg(_(e_using_number_as_string));
		break;
	    }
	    vim_snprintf((char *)buf, NUMBUFLEN, "%lld",
					    (varnumber_T)varp->vval.v_number);
	    戻る buf;
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
	    もし (strict)
	    {
		emsg(_(e_using_float_as_string));
		break;
	    }
	    vim_snprintf((char *)buf, NUMBUFLEN, "%g", varp->vval.v_float);
	    戻る buf;
	case VAR_STRING:
	    もし (varp->vval.v_string != NULL)
		戻る varp->vval.v_string;
	    戻る (char_u *)"";
	case VAR_BOOL:
	case VAR_SPECIAL:
	    STRCPY(buf, get_var_special_name(varp->vval.v_number));
	    戻る buf;
	case VAR_BLOB:
	    emsg(_(e_using_blob_as_string));
	    break;
	case VAR_CLASS:
	case VAR_TYPEALIAS:
	    check_typval_is_value(varp);
	    break;
	case VAR_OBJECT:
	    {
		もし (varp->vval.v_object == NULL)
		    emsg(_(e_using_object_as_string));
		否则
		{
		    class_T *cl = varp->vval.v_object->obj_class;
		    もし (cl != NULL && IS_ENUM(cl))
			semsg(_(e_using_enum_str_as_string), cl->class_name.string);
		    否则
			emsg(_(e_using_object_as_string));
		}
	    }
	    break;
	case VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    もし (in_vim9script())
	    {
		semsg(_(e_using_invalid_value_as_string_str), "job");
		break;
	    }
	    戻る job_to_string_buf(varp, buf);
#endif
	    break;
	case VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    もし (in_vim9script())
	    {
		semsg(_(e_using_invalid_value_as_string_str), "channel");
		break;
	    }
	    戻る channel_to_string_buf(varp, buf);
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
    戻る NULL;
}

/*
 * Turn a typeval into a string.  Similar to tv_get_string_buf() but uses
 * string() on Dict, List, etc.
 */
    char_u *
tv_stringify(typval_T *varp, char_u *buf)
{
    もし (varp->v_type == VAR_LIST
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
	戻る tmp.vval.v_string;
    }
    戻る tv_get_string_buf(varp, buf);
}

/*
 * Return TRUE if typeval "tv" and its value are set to be locked (immutable).
 * Also give an error message, using "name" or _("name") when use_gettext is
 * TRUE.
 */
    整数
tv_check_lock(typval_T *tv, char_u *name, 整数 use_gettext)
{
    整数	lock = 0;

    switch (tv->v_type)
    {
	case VAR_BLOB:
	    もし (tv->vval.v_blob != NULL)
		lock = tv->vval.v_blob->bv_lock;
	    break;
	case VAR_LIST:
	    もし (tv->vval.v_list != NULL)
		lock = tv->vval.v_list->lv_lock;
	    break;
	case VAR_TUPLE:
	    もし (tv->vval.v_tuple != NULL)
		lock = tv->vval.v_tuple->tv_lock;
	    break;
	case VAR_DICT:
	    もし (tv->vval.v_dict != NULL)
		lock = tv->vval.v_dict->dv_lock;
	    break;
	default:
	    break;
    }
    戻る value_check_lock(tv->v_lock, name, use_gettext)
		   || (lock != 0 && value_check_lock(lock, name, use_gettext));
}

/*
 * Copy the values from typval_T "from" to typval_T "to".
 * When needed allocates string or increases reference count.
 * Does not make a copy of a list, blob or dict but copies the reference!
 * It is OK for "from" and "to" to point to the same item.  This is used to
 * make a copy later.
 */
    整数
copy_tv(typval_T *from, typval_T *to)
{
    整数		ret = OK;

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
	    もし (to->vval.v_job != NULL)
		++to->vval.v_job->jv_refcount;
	    break;
#endif
	case VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    to->vval.v_channel = from->vval.v_channel;
	    もし (to->vval.v_channel != NULL)
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
	    もし (from->vval.v_string == NULL)
		to->vval.v_string = NULL;
	    否则
	    {
		to->vval.v_string = vim_strsave(from->vval.v_string);
		もし (from->v_type == VAR_FUNC)
		    func_ref(to->vval.v_string);
	    }
	    break;
	case VAR_PARTIAL:
	    もし (from->vval.v_partial == NULL)
		to->vval.v_partial = NULL;
	    否则
	    {
		to->vval.v_partial = from->vval.v_partial;
		++to->vval.v_partial->pt_refcount;
	    }
	    break;
	case VAR_BLOB:
	    もし (from->vval.v_blob == NULL)
		to->vval.v_blob = NULL;
	    否则
	    {
		to->vval.v_blob = from->vval.v_blob;
		++to->vval.v_blob->bv_refcount;
	    }
	    break;
	case VAR_LIST:
	    もし (from->vval.v_list == NULL)
		to->vval.v_list = NULL;
	    否则
	    {
		to->vval.v_list = from->vval.v_list;
		++to->vval.v_list->lv_refcount;
	    }
	    break;
	case VAR_TUPLE:
	    もし (from->vval.v_tuple == NULL)
		to->vval.v_tuple = NULL;
	    否则
	    {
		to->vval.v_tuple = from->vval.v_tuple;
		++to->vval.v_tuple->tv_refcount;
	    }
	    break;
	case VAR_DICT:
	    もし (from->vval.v_dict == NULL)
		to->vval.v_dict = NULL;
	    否则
	    {
		to->vval.v_dict = from->vval.v_dict;
		++to->vval.v_dict->dv_refcount;
	    }
	    break;
	case VAR_TYPEALIAS:
	    もし (from->vval.v_typealias == NULL)
		to->vval.v_typealias = NULL;
	    否则
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

    戻る ret;
}

/*
 * Compare "tv1" and "tv2".
 */
    整数
typval_compare2(
    typval_T	*tv1,	// first operand
    typval_T	*tv2,	// second operand
    exprtype_T	type,	// operator
    整数		ic,	// ignore case
    整数		*res)	// comparison result
{
    varnumber_T	n1, n2;
    整数		type_is = type == EXPR_IS || type == EXPR_ISNOT;

    もし (check_typval_is_value(tv1) == FAIL
	|| check_typval_is_value(tv2) == FAIL)
	戻る FAIL;
    否则 もし (type_is && tv1->v_type != tv2->v_type)
    {
	// For "is" a different type always means FALSE, for "isnot"
	// it means TRUE.
	*res = (type == EXPR_ISNOT);
    }
    否则 もし (((tv1->v_type == VAR_SPECIAL && tv1->vval.v_number == VVAL_NULL)
		|| (tv2->v_type == VAR_SPECIAL
					   && tv2->vval.v_number == VVAL_NULL))
	    && tv1->v_type != tv2->v_type
	    && (type == EXPR_EQUAL || type == EXPR_NEQUAL))
    {
	n1 = typval_compare_null(tv1, tv2);
	もし (n1 == MAYBE)
	    戻る FAIL;
	もし (type == EXPR_NEQUAL)
	    n1 = !n1;
	*res = n1;
    }
    否则 もし (tv1->v_type == VAR_BLOB || tv2->v_type == VAR_BLOB)
    {
	もし (typval_compare_blob(tv1, tv2, type, res) == FAIL)
	    戻る FAIL;
    }
    否则 もし (tv1->v_type == VAR_LIST || tv2->v_type == VAR_LIST)
    {
	もし (typval_compare_list(tv1, tv2, type, ic, res) == FAIL)
	    戻る FAIL;
    }
    否则 もし (tv1->v_type == VAR_TUPLE || tv2->v_type == VAR_TUPLE)
    {
	もし (typval_compare_tuple(tv1, tv2, type, ic, res) == FAIL)
	    戻る FAIL;
    }
    否则 もし (tv1->v_type == VAR_OBJECT || tv2->v_type == VAR_OBJECT)
    {
	もし (typval_compare_object(tv1, tv2, type, ic, res) == FAIL)
	    戻る FAIL;
    }
    否则 もし (tv1->v_type == VAR_DICT || tv2->v_type == VAR_DICT)
    {
	もし (typval_compare_dict(tv1, tv2, type, ic, res) == FAIL)
	    戻る FAIL;
    }
    否则 もし (tv1->v_type == VAR_FUNC || tv2->v_type == VAR_FUNC
	|| tv1->v_type == VAR_PARTIAL || tv2->v_type == VAR_PARTIAL)
    {
	もし (typval_compare_func(tv1, tv2, type, ic, res) == FAIL)
	    戻る FAIL;
    }

    // If one of the two variables is a float, compare as a float.
    // When using "=~" or "!~", always compare as string.
    否则 もし ((tv1->v_type == VAR_FLOAT || tv2->v_type == VAR_FLOAT)
	    && type != EXPR_MATCH && type != EXPR_NOMATCH)
    {
	float_T f1, f2;
	整数	error = FALSE;

	f1 = tv_get_float_chk(tv1, &error);
	もし (!error)
	    f2 = tv_get_float_chk(tv2, &error);
	もし (error)
	    戻る FAIL;
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
    否则 もし ((tv1->v_type == VAR_NUMBER || tv2->v_type == VAR_NUMBER)
	    && type != EXPR_MATCH && type != EXPR_NOMATCH)
    {
	整数 error = FALSE;

	n1 = tv_get_number_chk(tv1, &error);
	もし (!error)
	    n2 = tv_get_number_chk(tv2, &error);
	もし (error)
	    戻る FAIL;
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
    否则 もし (in_vim9script() && (tv1->v_type == VAR_BOOL
				    || tv2->v_type == VAR_BOOL
				    || (tv1->v_type == VAR_SPECIAL
					      && tv2->v_type == VAR_SPECIAL)))
    {
	もし (tv1->v_type != tv2->v_type)
	{
	    semsg(_(e_cannot_compare_str_with_str),
		       vartype_name(tv1->v_type), vartype_name(tv2->v_type));
	    戻る FAIL;
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
		戻る FAIL;
	}
	*res = n1;
    }
#ifdef FEAT_JOB_CHANNEL
    否则 もし (tv1->v_type == tv2->v_type
	    && (tv1->v_type == VAR_CHANNEL || tv1->v_type == VAR_JOB)
	    && (type == EXPR_NEQUAL || type == EXPR_EQUAL))
    {
	もし (tv1->v_type == VAR_CHANNEL)
	    n1 = tv1->vval.v_channel == tv2->vval.v_channel;
	否则
	    n1 = tv1->vval.v_job == tv2->vval.v_job;
	もし (type == EXPR_NEQUAL)
	    n1 = !n1;
	*res = n1;
    }
#endif
    否则
    {
	もし (typval_compare_string(tv1, tv2, type, ic, res) == FAIL)
	    戻る FAIL;
    }

    戻る OK;
}

/*
 * Compare "tv1" and "tv2".
 * Put the result in "tv1".  Caller should clear "tv2".
 */
    整数
typval_compare(
    typval_T	*tv1,	// first operand
    typval_T	*tv2,	// second operand
    exprtype_T	type,   // operator
    整数		ic)     // ignore case
{
    整数 res;

    もし (typval_compare2(tv1, tv2, type, ic, &res) == FAIL)
    {
	clear_tv(tv1);
	戻る FAIL;
    }

    clear_tv(tv1);
    もし (in_vim9script())
    {
	tv1->v_type = VAR_BOOL;
	tv1->vval.v_number = res ? VVAL_TRUE : VVAL_FALSE;
    }
    否则
    {
	tv1->v_type = VAR_NUMBER;
	tv1->vval.v_number = res;
    }

    戻る OK;
}

/*
 * Compare "tv1" to "tv2" as lists according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    整数
typval_compare_list(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	整数	    ic,
	整数	    *res)
{
    整数	    val = 0;

    もし (type == EXPR_IS || type == EXPR_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
				      && tv1->vval.v_list == tv2->vval.v_list);
	もし (type == EXPR_ISNOT)
	    val = !val;
    }
    否则 もし (tv1->v_type != tv2->v_type
	    || (type != EXPR_EQUAL && type != EXPR_NEQUAL))
    {
	もし (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_list_with_list));
	否则
	    emsg(_(e_invalid_operation_for_list));
	戻る FAIL;
    }
    否则
    {
	val = list_equal(tv1->vval.v_list, tv2->vval.v_list, ic);
	もし (type == EXPR_NEQUAL)
	    val = !val;
    }
    *res = val;
    戻る OK;
}

/*
 * Compare "tv1" to "tv2" as tuples according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    整数
typval_compare_tuple(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	整数	    ic,
	整数	    *res)
{
    整数	    val = 0;

    もし (type == EXPR_IS || type == EXPR_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
			      && tv1->vval.v_tuple == tv2->vval.v_tuple);
	もし (type == EXPR_ISNOT)
	    val = !val;
    }
    否则 もし (tv1->v_type != tv2->v_type
	    || (type != EXPR_EQUAL && type != EXPR_NEQUAL))
    {
	もし (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_tuple_with_tuple));
	否则
	    emsg(_(e_invalid_operation_for_tuple));
	戻る FAIL;
    }
    否则
    {
	val = tuple_equal(tv1->vval.v_tuple, tv2->vval.v_tuple, ic);
	もし (type == EXPR_NEQUAL)
	    val = !val;
    }
    *res = val;
    戻る OK;
}

/*
 * Compare v:null with another type.  Return TRUE if the value is NULL.
 */
    整数
typval_compare_null(typval_T *tv1, typval_T *tv2)
{
    もし ((tv1->v_type == VAR_SPECIAL && tv1->vval.v_number == VVAL_NULL)
	    || (tv2->v_type == VAR_SPECIAL && tv2->vval.v_number == VVAL_NULL))
    {
	typval_T	*tv = tv1->v_type == VAR_SPECIAL ? tv2 : tv1;

	switch (tv->v_type)
	{
	    case VAR_BLOB: 戻る tv->vval.v_blob == NULL;
#ifdef FEAT_JOB_CHANNEL
	    case VAR_CHANNEL: 戻る tv->vval.v_channel == NULL;
#endif
	    // TODO: null_class handling
	    // case VAR_CLASS: return tv->vval.v_class == NULL;
	    case VAR_DICT: 戻る tv->vval.v_dict == NULL;
	    case VAR_FUNC: 戻る tv->vval.v_string == NULL;
#ifdef FEAT_JOB_CHANNEL
	    case VAR_JOB: 戻る tv->vval.v_job == NULL;
#endif
	    case VAR_LIST: 戻る tv->vval.v_list == NULL;
	    case VAR_TUPLE: 戻る tv->vval.v_tuple == NULL;
	    case VAR_OBJECT: 戻る tv->vval.v_object == NULL;
	    case VAR_PARTIAL: 戻る tv->vval.v_partial == NULL;
	    case VAR_STRING: 戻る tv->vval.v_string == NULL;

	    case VAR_NUMBER: もし (!in_vim9script())
				 戻る tv->vval.v_number == 0;
			     break;
	    case VAR_FLOAT: もし (!in_vim9script())
				 戻る tv->vval.v_float == 0.0;
			     break;
	    case VAR_TYPEALIAS: 戻る tv->vval.v_typealias == NULL;
	    default: break;
	}
    }
    // although comparing null with number, float or bool is not very useful
    // we won't give an error
    戻る FALSE;
}

/*
 * Compare "tv1" to "tv2" as blobs according to "type".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    整数
typval_compare_blob(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	整数	    *res)
{
    整数	    val = 0;

    もし (type == EXPR_IS || type == EXPR_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
			&& tv1->vval.v_blob == tv2->vval.v_blob);
	もし (type == EXPR_ISNOT)
	    val = !val;
    }
    否则 もし (tv1->v_type != tv2->v_type
	    || (type != EXPR_EQUAL && type != EXPR_NEQUAL))
    {
	もし (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_blob_with_blob));
	否则
	    emsg(_(e_invalid_operation_for_blob));
	戻る FAIL;
    }
    否则
    {
	val = blob_equal(tv1->vval.v_blob, tv2->vval.v_blob);
	もし (type == EXPR_NEQUAL)
	    val = !val;
    }
    *res = val;
    戻る OK;
}

/*
 * Compare "tv1" to "tv2" as objects according to "type".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    整数
typval_compare_object(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	整数	    ic,
	整数	    *res)
{
    整数 res_match = type == EXPR_EQUAL || type == EXPR_IS ? TRUE : FALSE;

    もし (tv1->vval.v_object == NULL && tv2->vval.v_object == NULL)
    {
	*res = res_match;
	戻る OK;
    }
    もし (tv1->vval.v_object == NULL || tv2->vval.v_object == NULL)
    {
	*res = !res_match;
	戻る OK;
    }

    object_T *obj1 = tv1->vval.v_object;
    object_T *obj2 = tv2->vval.v_object;
    もし (type == EXPR_IS || type == EXPR_ISNOT)
    {
	*res = obj1 == obj2 ? res_match : !res_match;
	戻る OK;
    }
    否则 もし (tv1->v_type != tv2->v_type)
    {
	emsg(_(e_can_only_compare_object_with_object));
	戻る FAIL;
    }

    *res = object_equal(obj1, obj2, ic) ? res_match : !res_match;
    戻る OK;
}

/*
 * Compare "tv1" to "tv2" as dictionaries according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    整数
typval_compare_dict(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	整数	    ic,
	整数	    *res)
{
    整数	    val;

    もし (type == EXPR_IS || type == EXPR_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
			&& tv1->vval.v_dict == tv2->vval.v_dict);
	もし (type == EXPR_ISNOT)
	    val = !val;
    }
    否则 もし (tv1->v_type != tv2->v_type
		|| (type != EXPR_EQUAL && type != EXPR_NEQUAL))
    {
	もし (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_dictionary_with_dictionary));
	否则
	    emsg(_(e_invalid_operation_for_dictionary));
	戻る FAIL;
    }
    否则
    {
	val = dict_equal(tv1->vval.v_dict, tv2->vval.v_dict, ic);
	もし (type == EXPR_NEQUAL)
	    val = !val;
    }
    *res = val;
    戻る OK;
}

/*
 * Compare "tv1" to "tv2" as funcrefs according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    整数
typval_compare_func(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	整数	    ic,
	整数	    *res)
{
    整数	    val = 0;

    もし (type != EXPR_EQUAL && type != EXPR_NEQUAL
	    && type != EXPR_IS && type != EXPR_ISNOT)
    {
	emsg(_(e_invalid_operation_for_funcrefs));
	戻る FAIL;
    }
    もし ((tv1->v_type == VAR_PARTIAL && tv1->vval.v_partial == NULL)
	    || (tv2->v_type == VAR_PARTIAL && tv2->vval.v_partial == NULL))
	// When both partials are NULL, then they are equal.
	// Otherwise they are not equal.
	val = (tv1->vval.v_partial == tv2->vval.v_partial);
    否则 もし (type == EXPR_IS || type == EXPR_ISNOT)
    {
	もし (tv1->v_type == VAR_FUNC && tv2->v_type == VAR_FUNC)
	    // strings are considered the same if their value is
	    // the same
	    val = tv_equal(tv1, tv2, ic);
	否则 もし (tv1->v_type == VAR_PARTIAL && tv2->v_type == VAR_PARTIAL)
	    val = (tv1->vval.v_partial == tv2->vval.v_partial);
	否则
	    val = FALSE;
    }
    否则
	val = tv_equal(tv1, tv2, ic);
    もし (type == EXPR_NEQUAL || type == EXPR_ISNOT)
	val = !val;
    *res = val;
    戻る OK;
}

/*
 * Compare "tv1" to "tv2" as strings according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    整数
typval_compare_string(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	整数	    ic,
	整数	    *res)
{
    整数		i = 0;
    整数		val = FALSE;
    char_u	*s1, *s2;
    char_u	buf1[NUMBUFLEN], buf2[NUMBUFLEN];

    もし (in_vim9script()
	  && ((tv1->v_type != VAR_STRING && tv1->v_type != VAR_SPECIAL)
	   || (tv2->v_type != VAR_STRING && tv2->v_type != VAR_SPECIAL)))
    {
	semsg(_(e_cannot_compare_str_with_str),
		   vartype_name(tv1->v_type), vartype_name(tv2->v_type));
	戻る FAIL;
    }
    s1 = tv_get_string_buf(tv1, buf1);
    s2 = tv_get_string_buf(tv2, buf2);
    もし (type != EXPR_MATCH && type != EXPR_NOMATCH)
	i = ic ? MB_STRICMP(s1, s2) : STRCMP(s1, s2);
    switch (type)
    {
	case EXPR_IS:	    もし (in_vim9script())
			    {
				// Really check it is the same string, not just
				// the same value.
				val = tv1->vval.v_string == tv2->vval.v_string;
				break;
			    }
			    // FALLTHROUGH
	case EXPR_EQUAL:    val = (i == 0); break;
	case EXPR_ISNOT:    もし (in_vim9script())
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
		もし (type == EXPR_NOMATCH)
		    val = !val;
		break;

	default:  break;  // avoid gcc warning
    }
    *res = val;
    戻る OK;
}
/*
 * Convert any type to a string, never give an error.
 * When "quotes" is TRUE add quotes to a string.
 * Returns an allocated string.
 */
    char_u *
typval_tostring(typval_T *arg, 整数 quotes)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    char_u	*ret = NULL;

    もし (arg == NULL)
	戻る vim_strsave((char_u *)"(does not exist)");
    もし (!quotes && arg->v_type == VAR_STRING)
    {
	ret = vim_strsave(arg->vval.v_string == NULL ? (char_u *)""
							 : arg->vval.v_string);
    }
    否则
    {
	ret = tv2string(arg, &tofree, numbuf, 0);
	// Make a copy if we have a value but it's not in allocated memory.
	もし (ret != NULL && tofree == NULL)
	    ret = vim_strsave(ret);
    }
    戻る ret;
}

/*
 * Return TRUE if typeval "tv" is locked: Either that value is locked itself
 * or it refers to a List or Dictionary that is locked.
 */
    整数
tv_islocked(typval_T *tv)
{
    戻る (tv->v_lock & VAR_LOCKED)
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

    static 整数
func_equal(
    typval_T *tv1,
    typval_T *tv2,
    整数	     ic)	    // ignore case
{
    char_u	*s1, *s2;
    dict_T	*d1, *d2;
    整数		a1, a2;
    整数		i;

    // empty and NULL function name considered the same
    s1 = tv1->v_type == VAR_FUNC ? tv1->vval.v_string
					   : partial_name(tv1->vval.v_partial);
    もし (s1 != NULL && *s1 == NUL)
	s1 = NULL;
    s2 = tv2->v_type == VAR_FUNC ? tv2->vval.v_string
					   : partial_name(tv2->vval.v_partial);
    もし (s2 != NULL && *s2 == NUL)
	s2 = NULL;
    もし (s1 == NULL || s2 == NULL)
    {
	もし (s1 != s2)
	    戻る FALSE;
    }
    否则 もし (STRCMP(s1, s2) != 0)
	戻る FALSE;

    // empty dict and NULL dict is different
    d1 = tv1->v_type == VAR_FUNC ? NULL : tv1->vval.v_partial->pt_dict;
    d2 = tv2->v_type == VAR_FUNC ? NULL : tv2->vval.v_partial->pt_dict;
    もし (d1 == NULL || d2 == NULL)
    {
	もし (d1 != d2)
	    戻る FALSE;
    }
    否则 もし (!dict_equal(d1, d2, ic))
	戻る FALSE;

    // empty list and no list considered the same
    a1 = tv1->v_type == VAR_FUNC ? 0 : tv1->vval.v_partial->pt_argc;
    a2 = tv2->v_type == VAR_FUNC ? 0 : tv2->vval.v_partial->pt_argc;
    もし (a1 != a2)
	戻る FALSE;
    ために (i = 0; i < a1; ++i)
	もし (!tv_equal(tv1->vval.v_partial->pt_argv + i,
		      tv2->vval.v_partial->pt_argv + i, ic))
	    戻る FALSE;

    戻る TRUE;
}

/*
 * Return TRUE if "tv1" and "tv2" have the same value.
 * Compares the items just like "==" would compare them, but strings and
 * numbers are different.  Floats and numbers are also different.
 */
    整数
tv_equal(
    typval_T *tv1,
    typval_T *tv2,
    整数	     ic)	    // ignore case
{
    char_u	buf1[NUMBUFLEN], buf2[NUMBUFLEN];
    char_u	*s1, *s2;
    static 整数  recursive_cnt = 0;	    // catch recursive loops
    整数		r;
    static 整数	tv_equal_recurse_limit;

    // Catch lists and dicts that have an endless loop by limiting
    // recursiveness to a limit.  We guess they are equal then.
    // A fixed limit has the problem of still taking an awful long time.
    // Reduce the limit every time running into it. That should work fine for
    // deeply linked structures that are not recursively linked and catch
    // recursiveness quickly.
    もし (recursive_cnt == 0)
	tv_equal_recurse_limit = 1000;
    もし (recursive_cnt >= tv_equal_recurse_limit)
    {
	--tv_equal_recurse_limit;
	戻る TRUE;
    }

    // For VAR_FUNC and VAR_PARTIAL compare the function name, bound dict and
    // arguments.
    もし ((tv1->v_type == VAR_FUNC
		|| (tv1->v_type == VAR_PARTIAL && tv1->vval.v_partial != NULL))
	    && (tv2->v_type == VAR_FUNC
		|| (tv2->v_type == VAR_PARTIAL && tv2->vval.v_partial != NULL)))
    {
	++recursive_cnt;
	r = func_equal(tv1, tv2, ic);
	--recursive_cnt;
	戻る r;
    }

    もし (tv1->v_type != tv2->v_type
	    && ((tv1->v_type != VAR_BOOL && tv1->v_type != VAR_SPECIAL)
		|| (tv2->v_type != VAR_BOOL && tv2->v_type != VAR_SPECIAL)))
	戻る FALSE;

    switch (tv1->v_type)
    {
	case VAR_LIST:
	    ++recursive_cnt;
	    r = list_equal(tv1->vval.v_list, tv2->vval.v_list, ic);
	    --recursive_cnt;
	    戻る r;

	case VAR_TUPLE:
	    ++recursive_cnt;
	    r = tuple_equal(tv1->vval.v_tuple, tv2->vval.v_tuple, ic);
	    --recursive_cnt;
	    戻る r;

	case VAR_DICT:
	    ++recursive_cnt;
	    r = dict_equal(tv1->vval.v_dict, tv2->vval.v_dict, ic);
	    --recursive_cnt;
	    戻る r;

	case VAR_BLOB:
	    戻る blob_equal(tv1->vval.v_blob, tv2->vval.v_blob);

	case VAR_NUMBER:
	case VAR_BOOL:
	case VAR_SPECIAL:
	    戻る tv1->vval.v_number == tv2->vval.v_number;

	case VAR_STRING:
	    s1 = tv_get_string_buf(tv1, buf1);
	    s2 = tv_get_string_buf(tv2, buf2);
	    戻る ((ic ? MB_STRICMP(s1, s2) : STRCMP(s1, s2)) == 0);

	case VAR_FLOAT:
	    戻る tv1->vval.v_float == tv2->vval.v_float;
	case VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    戻る tv1->vval.v_job == tv2->vval.v_job;
#endif
	case VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    戻る tv1->vval.v_channel == tv2->vval.v_channel;
#endif
	case VAR_INSTR:
	    戻る tv1->vval.v_instr == tv2->vval.v_instr;

	case VAR_CLASS:
	    // A class only exists once, equality is identity.
	    戻る tv1->vval.v_class == tv2->vval.v_class;

	case VAR_OBJECT:
	    ++recursive_cnt;
	    r = object_equal(tv1->vval.v_object, tv2->vval.v_object, ic);
	    --recursive_cnt;
	    戻る r;

	case VAR_PARTIAL:
	    戻る tv1->vval.v_partial == tv2->vval.v_partial;

	case VAR_FUNC:
	    戻る tv1->vval.v_string == tv2->vval.v_string;

	case VAR_TYPEALIAS:
	    戻る tv1->vval.v_typealias == tv2->vval.v_typealias;

	case VAR_UNKNOWN:
	case VAR_ANY:
	case VAR_VOID:
	    break;
    }

    // VAR_UNKNOWN can be the result of a invalid expression, let's say it
    // does not equal anything, not even itself.
    戻る FALSE;
}

/*
 * Get an option value.
 * "arg" points to the '&' or '+' before the option name.
 * "arg" is advanced to character after the option name.
 * Return OK or FAIL.
 */
    整数
eval_option(
    char_u	**arg,
    typval_T	*rettv,	// when NULL, only check if option exists
    整数		evaluate)
{
    char_u	*option_end;
    long	numval;
    char_u	*stringval;
    getoption_T	opt_type;
    整数		c;
    整数		working = (**arg == '+');    // has("+option")
    整数		ret = OK;
    整数		scope;

    // Isolate the option name and find its value.
    option_end = find_option_end(arg, &scope);
    もし (option_end == NULL)
    {
	もし (rettv != NULL)
	    semsg(_(e_option_name_missing_str), *arg);
	戻る FAIL;
    }

    もし (!evaluate)
    {
	*arg = option_end;
	戻る OK;
    }

    c = *option_end;
    *option_end = NUL;
    opt_type = get_option_value(*arg, &numval,
			       rettv == NULL ? NULL : &stringval, NULL, scope);

    もし (opt_type == gov_unknown)
    {
	もし (rettv != NULL)
	    semsg(_(e_unknown_option_str), *arg);
	ret = FAIL;
    }
    否则 もし (rettv != NULL)
    {
	rettv->v_lock = 0;
	もし (opt_type == gov_hidden_string)
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = NULL;
	}
	否则 もし (opt_type == gov_hidden_bool || opt_type == gov_hidden_number)
	{
	    rettv->v_type = in_vim9script() && opt_type == gov_hidden_bool
						       ? VAR_BOOL : VAR_NUMBER;
	    rettv->vval.v_number = 0;
	}
	否则 もし (opt_type == gov_bool || opt_type == gov_number)
	{
	    もし (in_vim9script() && opt_type == gov_bool)
	    {
		rettv->v_type = VAR_BOOL;
		rettv->vval.v_number = numval ? VVAL_TRUE : VVAL_FALSE;
	    }
	    否则
	    {
		rettv->v_type = VAR_NUMBER;
		rettv->vval.v_number = numval;
	    }
	}
	否则				// string option
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = stringval;
	}
    }
    否则 もし (working && (opt_type == gov_hidden_bool
			|| opt_type == gov_hidden_number
			|| opt_type == gov_hidden_string))
	ret = FAIL;

    *option_end = c;		    // put back for error messages
    *arg = option_end;

    戻る ret;
}

/*
 * Allocate a variable for a number constant.  Also deals with "0z" for blob.
 * Return OK or FAIL.
 */
    整数
eval_number(
	char_u	    **arg,
	typval_T    *rettv,
	整数	    evaluate,
	整数	    want_string)
{
    整数		len;
    整数		skip_quotes = !in_old_script(4);
    char_u	*p;
    整数		get_float = FALSE;

    // We accept a float when the format matches
    // "[0-9]\+\.[0-9]\+\([eE][+-]\?[0-9]\+\)\?".  This is very
    // strict to avoid backwards compatibility problems.
    // With script version 2 and later the leading digit can be
    // omitted.
    // Don't look for a float after the "." operator, so that
    // ":let vers = 1.2.3" doesn't fail.
    もし (**arg == '.')
	p = *arg;
    否则
    {
	p = *arg + 1;
	もし (skip_quotes)
	    ために (;;)
	    {
		もし (*p == '\'')
		    ++p;
		もし (!vim_isdigit(*p))
		    break;
		p = skipdigits(p);
	    }
	否则
	    p = skipdigits(p);
    }
    もし (!want_string && p[0] == '.' && vim_isdigit(p[1]))
    {
	get_float = TRUE;
	p = skipdigits(p + 2);
	もし (*p == 'e' || *p == 'E')
	{
	    ++p;
	    もし (*p == '-' || *p == '+')
		++p;
	    もし (!vim_isdigit(*p))
		get_float = FALSE;
	    否则
		p = skipdigits(p + 1);
	}
	もし (ASCII_ISALPHA(*p) || *p == '.')
	    get_float = FALSE;
    }
    もし (get_float)
    {
	float_T	f;

	*arg += string2float(*arg, &f, skip_quotes);
	もし (evaluate)
	{
	    rettv->v_type = VAR_FLOAT;
	    rettv->vval.v_float = f;
	}
    }
    否则
    もし (**arg == '0' && ((*arg)[1] == 'z' || (*arg)[1] == 'Z'))
    {
	char_u  *bp;
	blob_T  *blob = NULL;  // init for gcc

	// Blob constant: 0z0123456789abcdef
	もし (evaluate)
	    blob = blob_alloc();
	ために (bp = *arg + 2; vim_isxdigit(bp[0]); bp += 2)
	{
	    もし (!vim_isxdigit(bp[1]))
	    {
		もし (blob != NULL)
		{
		    emsg(_(e_blob_literal_should_have_an_even_number_of_hex_characters));
		    ga_clear(&blob->bv_ga);
		    VIM_CLEAR(blob);
		}
		戻る FAIL;
	    }
	    もし (blob != NULL)
		ga_append(&blob->bv_ga,
			     (hex2nr(*bp) << 4) + hex2nr(*(bp+1)));
	    もし (bp[2] == '.' && vim_isxdigit(bp[3]))
		++bp;
	}
	もし (blob != NULL)
	    rettv_blob_set(rettv, blob);
	*arg = bp;
    }
    否则
    {
	varnumber_T	n;

	// decimal, hex or octal number
	vim_str2nr(*arg, NULL, &len, skip_quotes
		      ? STR2NR_NO_OCT + STR2NR_QUOTE
		      : STR2NR_ALL, &n, NULL, 0, TRUE, NULL);
	もし (len == 0)
	{
	    もし (evaluate)
		semsg(_(e_invalid_expression_str), *arg);
	    戻る FAIL;
	}
	*arg += len;
	もし (evaluate)
	{
	    rettv->v_type = VAR_NUMBER;
	    rettv->vval.v_number = n;
	}
    }
    戻る OK;
}

/*
 * Evaluate a string constant and put the result in "rettv".
 * "*arg" points to the double quote or to after it when "interpolate" is TRUE.
 * When "interpolate" is TRUE reduce "{{" to "{", reduce "}}" to "}" and stop
 * at a single "{".
 * Return OK or FAIL.
 */
    整数
eval_string(char_u **arg, typval_T *rettv, 整数 evaluate, 整数 interpolate)
{
    char_u	*p;
    char_u	*end;
    整数		extra = interpolate ? 1 : 0;
    整数		off = interpolate ? 0 : 1;
    整数		len;

    // Find the end of the string, skipping backslashed characters.
    ために (p = *arg + off; *p != NUL && *p != '"'; MB_PTR_ADV(p))
    {
	もし (*p == '\\' && p[1] != NUL)
	{
	    ++p;
	    // A "\<x>" form occupies at least 4 characters, and produces up
	    // to 9 characters (6 for the char and 3 for a modifier):
	    // reserve space for 5 extra.
	    もし (*p == '<')
	    {
		整数		modifiers = 0;
		整数		flags = FSK_KEYCODE | FSK_IN_STRING;

		extra += 5;

		// Skip to the '>' to avoid using '{' inside for string
		// interpolation.
		もし (p[1] != '*')
		    flags |= FSK_SIMPLIFY;
		もし (find_special_key(&p, &modifiers, flags, NULL) != 0)
		    --p;  // leave "p" on the ">"
	    }
	}
	否则 もし (interpolate && (*p == '{' || *p == '}'))
	{
	    もし (*p == '{' && p[1] != '{') // start of expression
		break;
	    ++p;
	    もし (p[-1] == '}' && *p != '}') // single '}' is an error
	    {
		semsg(_(e_stray_closing_curly_str), *arg);
		戻る FAIL;
	    }
	    --extra;  // "{{" becomes "{", "}}" becomes "}"
	}
    }

    もし (*p != '"' && !(interpolate && *p == '{'))
    {
	semsg(_(e_missing_double_quote_str), *arg);
	戻る FAIL;
    }

    // If only parsing, set *arg and return here
    もし (!evaluate)
    {
	*arg = p + off;
	戻る OK;
    }

    // Copy the string into allocated memory, handling backslashed
    // characters.
    rettv->v_type = VAR_STRING;
    len = (整数)(p - *arg + extra);
    rettv->vval.v_string = alloc(len);
    もし (rettv->vval.v_string == NULL)
	戻る FAIL;
    end = rettv->vval.v_string;

    ために (p = *arg + off; *p != NUL && *p != '"'; )
    {
	もし (*p == '\\')
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
			  もし (vim_isxdigit(p[1]))
			  {
			      整数	n, nr;
			      整数	c = SAFE_toupper(*p);

			      もし (c == 'X')
				  n = 2;
			      否则 もし (*p == 'u')
				  n = 4;
			      否则
				  n = 8;
			      nr = 0;
			      ながら (--n >= 0 && vim_isxdigit(p[1]))
			      {
				  ++p;
				  nr = (nr << 4) + hex2nr(*p);
			      }
			      ++p;
			      // For "\u" store the number according to
			      // 'encoding'.
			      もし (c != 'X')
				  end += (*mb_char2bytes)(nr, end);
			      否则
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
			  もし (*p >= '0' && *p <= '7')
			  {
			      *end = (*end << 3) + *p++ - '0';
			      もし (*p >= '0' && *p <= '7')
				  *end = (*end << 3) + *p++ - '0';
			  }
			  ++end;
			  break;

			  // Special key, e.g.: "\<C-W>"
		case '<':
			  {
			      整数 flags = FSK_KEYCODE | FSK_IN_STRING;

			      もし (p[1] != '*')
				  flags |= FSK_SIMPLIFY;
			      extra = trans_special(&p, end, flags, FALSE, NULL);
			      もし (extra != 0)
			      {
				  end += extra;
				  もし (end >= rettv->vval.v_string + len)
				      iemsg("eval_string() used more space than allocated");
				  break;
			      }
			  }
			  // FALLTHROUGH

		default: MB_COPY_CHAR(p, end);
			  break;
	    }
	}
	否则
	{
	    もし (interpolate && (*p == '{' || *p == '}'))
	    {
		もし (*p == '{' && p[1] != '{') // start of expression
		    break;
		++p;  // reduce "{{" to "{" and "}}" to "}"
	    }
	    MB_COPY_CHAR(p, end);
	}
    }
    *end = NUL;
    もし (*p == '"' && !interpolate)
	++p;
    *arg = p;

    戻る OK;
}

/*
 * Allocate a variable for a 'str''ing' constant.
 * When "interpolate" is TRUE reduce "{{" to "{" and stop at a single "{".
 * Return OK when a "rettv" was set to the string.
 * Return FAIL on error, "rettv" is not set.
 */
    整数
eval_lit_string(char_u **arg, typval_T *rettv, 整数 evaluate, 整数 interpolate)
{
    char_u	*p;
    char_u	*str;
    整数		reduce = interpolate ? -1 : 0;
    整数		off = interpolate ? 0 : 1;

    // Find the end of the string, skipping ''.
    ために (p = *arg + off; *p != NUL; MB_PTR_ADV(p))
    {
	もし (*p == '\'')
	{
	    もし (p[1] != '\'')
		break;
	    ++reduce;
	    ++p;
	}
	否则 もし (interpolate)
	{
	    もし (*p == '{')
	    {
		もし (p[1] != '{')
		    break;
		++p;
		++reduce;
	    }
	    否则 もし (*p == '}')
	    {
		++p;
		もし (*p != '}')
		{
		    semsg(_(e_stray_closing_curly_str), *arg);
		    戻る FAIL;
		}
		++reduce;
	    }
	}
    }

    もし (*p != '\'' && !(interpolate && *p == '{'))
    {
	semsg(_(e_missing_single_quote_str), *arg);
	戻る FAIL;
    }

    // If only parsing return after setting "*arg"
    もし (!evaluate)
    {
	*arg = p + off;
	戻る OK;
    }

    // Copy the string into allocated memory, handling '' to ' reduction and
    // any expressions.
    str = alloc((p - *arg) - reduce);
    もし (str == NULL)
	戻る FAIL;
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = str;

    ために (p = *arg + off; *p != NUL; )
    {
	もし (*p == '\'')
	{
	    もし (p[1] != '\'')
		break;
	    ++p;
	}
	否则 もし (interpolate && (*p == '{' || *p == '}'))
	{
	    もし (*p == '{' && p[1] != '{')
		break;
	    ++p;
	}
	MB_COPY_CHAR(p, str);
    }
    *str = NUL;
    *arg = p + off;

    戻る OK;
}

/*
 * Evaluate a single or double quoted string possibly containing expressions.
 * "arg" points to the '$'.  The result is put in "rettv".
 * Returns OK or FAIL.
 */
    整数
eval_interp_string(char_u **arg, typval_T *rettv, 整数 evaluate)
{
    typval_T	tv;
    整数		ret = OK;
    整数		quote;
    garray_T	ga;
    char_u	*p;

    ga_init2(&ga, 1, 80);

    // *arg is on the '$' character, move it to the first string character.
    ++*arg;
    quote = **arg;
    ++*arg;

    ために (;;)
    {
	// Get the string up to the matching quote or to a single '{'.
	// "arg" is advanced to either the quote or the '{'.
	もし (quote == '"')
	    ret = eval_string(arg, &tv, evaluate, TRUE);
	否则
	    ret = eval_lit_string(arg, &tv, evaluate, TRUE);
	もし (ret == FAIL)
	    break;
	もし (evaluate)
	{
	    ga_concat(&ga, tv.vval.v_string);
	    clear_tv(&tv);
	}

	もし (**arg != '{')
	{
	    // found terminating quote
	    ++*arg;
	    break;
	}
	p = eval_one_expr_in_str(*arg, &ga, evaluate);
	もし (p == NULL)
	{
	    ret = FAIL;
	    break;
	}
	*arg = p;
    }

    rettv->v_type = VAR_STRING;
    もし (ret == FAIL || !evaluate || ga_append(&ga, NUL) == FAIL)
    {
	ga_clear(&ga);
	rettv->vval.v_string = NULL;
	戻る ret;
    }

    rettv->vval.v_string = ga.ga_data;
    戻る OK;
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
    整数		copyID)
{
    戻る echo_string_core(tv, tofree, numbuf, copyID, FALSE, TRUE, FALSE);
}

/*
 * Get the value of an environment variable.
 * "arg" is pointing to the '$'.  It is advanced to after the name.
 * If the environment variable was not set, silently assume it is empty.
 * Return FAIL if the name is invalid.
 */
    整数
eval_env_var(char_u **arg, typval_T *rettv, 整数 evaluate)
{
    char_u	*string = NULL;
    整数		len;
    整数		cc;
    char_u	*name;
    整数		mustfree = FALSE;

    ++*arg;
    name = *arg;
    len = get_env_len(arg);
    もし (evaluate)
    {
	もし (len == 0)
	    戻る FAIL; // invalid empty name

	cc = name[len];
	name[len] = NUL;
	// first try vim_getenv(), fast for normal environment vars
	string = vim_getenv(name, &mustfree);
	もし (string != NULL && *string != NUL)
	{
	    もし (!mustfree)
		string = vim_strsave(string);
	}
	否则
	{
	    もし (mustfree)
		vim_free(string);

	    // next try expanding things like $VIM and ${HOME}
	    string = expand_env_save(name - 1);
	    もし (string != NULL && *string == '$')
		VIM_CLEAR(string);
	}
	name[len] = cc;

	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = string;
	rettv->v_lock = 0;
    }

    戻る OK;
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
    整数		did_emsg_before = did_emsg;

    もし (argvars[0].v_type != VAR_STRING || !in_vim9script())
	lnum = (linenr_T)tv_get_number_chk(&argvars[0], NULL);
    もし (lnum <= 0 && did_emsg_before == did_emsg
					    && argvars[0].v_type != VAR_NUMBER)
    {
	整数	fnum;
	pos_T	*fp;

	// no valid number, try using arg like line()
	fp = var2fpos(&argvars[0], TRUE, &fnum, FALSE);
	もし (fp != NULL)
	    lnum = fp->lnum;
    }
    戻る lnum;
}

/*
 * Get the lnum from the first argument.
 * Also accepts "$", then "buf" is used.
 * Returns 0 on error.
 */
    linenr_T
tv_get_lnum_buf(typval_T *argvars, buf_T *buf)
{
    もし (argvars[0].v_type == VAR_STRING
	    && argvars[0].vval.v_string != NULL
	    && argvars[0].vval.v_string[0] == '$'
	    && argvars[0].vval.v_string[1] == NUL
	    && buf != NULL)
	戻る buf->b_ml.ml_line_count;
    戻る (linenr_T)tv_get_number_chk(&argvars[0], NULL);
}

/*
 * Get buffer by number or pattern.
 */
    buf_T *
tv_get_buf(typval_T *tv, 整数 curtab_only)
{
    char_u	*name = tv->vval.v_string;
    buf_T	*buf;

    もし (tv->v_type == VAR_NUMBER)
	戻る buflist_findnr((整数)tv->vval.v_number);
    もし (tv->v_type != VAR_STRING)
	戻る NULL;
    もし (name == NULL || *name == NUL)
	戻る curbuf;
    もし (name[0] == '$' && name[1] == NUL)
	戻る lastbuf;

    buf = buflist_find_by_name(name, curtab_only);

    // If not found, try expanding the name, like done for bufexists().
    もし (buf == NULL)
	buf = find_buffer(tv);

    戻る buf;
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
    もし (buf == NULL
	    && tv->v_type != VAR_NUMBER
	    && tv->v_type != VAR_STRING)
	// issue errmsg for type error
	(void)tv_get_number(tv);
    戻る buf;
}

/*
 * Check that repeating "slen" items "n" times, does not
 * overflow the int used for the resulting length.
 */
    整数
check_repeat_count(varnumber_T slen, varnumber_T n)
{
    // Use a division so that the multiplication cannot overflow.
    もし (n <= (varnumber_T)INT_MAX / slen)
	戻る OK;

    char	buf[NUMBUFLEN];

    vim_snprintf(buf, sizeof(buf), "%lld", (long long)n);
    semsg(_(e_val_too_large), buf);
    戻る FAIL;
}

#endif // FEAT_EVAL
