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

#ប្រសិនបើ defined(FEAT_EVAL)

/*
 * Allocate memory for a variable type-value, and make it empty (0 or NULL
 * value).
 */
    typval_T *
alloc_tv(void)
{
    ត្រឡប់ ALLOC_CLEAR_ONE(typval_T);
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
    ប្រសិនបើ (rettv != NULL)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = s;
    }
    បើមិនដូច្នេះ
	vim_free(s);
    ត្រឡប់ rettv;
}

/*
 * Free the memory for a variable type-value.
 */
    void
free_tv(typval_T *varp)
{
    ប្រសិនបើ (varp == NULL)
	ត្រឡប់;

    switch (varp->v_type)
    {
	pak_sa VAR_FUNC:
	    func_unref(varp->vval.v_string);
	    // FALLTHROUGH
	pak_sa VAR_STRING:
	    vim_free(varp->vval.v_string);
	    bha_nga;
	pak_sa VAR_PARTIAL:
	    partial_unref(varp->vval.v_partial);
	    bha_nga;
	pak_sa VAR_BLOB:
	    blob_unref(varp->vval.v_blob);
	    bha_nga;
	pak_sa VAR_LIST:
	    list_unref(varp->vval.v_list);
	    bha_nga;
	pak_sa VAR_TUPLE:
	    tuple_unref(varp->vval.v_tuple);
	    bha_nga;
	pak_sa VAR_DICT:
	    dict_unref(varp->vval.v_dict);
	    bha_nga;
	pak_sa VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    job_unref(varp->vval.v_job);
	    bha_nga;
#endif
	pak_sa VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    channel_unref(varp->vval.v_channel);
	    bha_nga;
#endif
	pak_sa VAR_CLASS:
	    class_unref(varp->vval.v_class);
	    bha_nga;
	pak_sa VAR_OBJECT:
	    object_unref(varp->vval.v_object);
	    bha_nga;

	pak_sa VAR_TYPEALIAS:
	    typealias_unref(varp->vval.v_typealias);
	    bha_nga;

	pak_sa VAR_NUMBER:
	pak_sa VAR_FLOAT:
	pak_sa VAR_ANY:
	pak_sa VAR_UNKNOWN:
	pak_sa VAR_VOID:
	pak_sa VAR_BOOL:
	pak_sa VAR_SPECIAL:
	pak_sa VAR_INSTR:
	    bha_nga;
    }
    vim_free(varp);
}

/*
 * Free the memory for a variable value and set the value to NULL or 0.
 */
    void
clear_tv(typval_T *varp)
{
    ប្រសិនបើ (varp == NULL)
	ត្រឡប់;

    switch (varp->v_type)
    {
	pak_sa VAR_FUNC:
	    func_unref(varp->vval.v_string);
	    // FALLTHROUGH
	pak_sa VAR_STRING:
	    VIM_CLEAR(varp->vval.v_string);
	    bha_nga;
	pak_sa VAR_PARTIAL:
	    partial_unref(varp->vval.v_partial);
	    varp->vval.v_partial = NULL;
	    bha_nga;
	pak_sa VAR_BLOB:
	    blob_unref(varp->vval.v_blob);
	    varp->vval.v_blob = NULL;
	    bha_nga;
	pak_sa VAR_LIST:
	    list_unref(varp->vval.v_list);
	    varp->vval.v_list = NULL;
	    bha_nga;
	pak_sa VAR_TUPLE:
	    tuple_unref(varp->vval.v_tuple);
	    varp->vval.v_tuple = NULL;
	    bha_nga;
	pak_sa VAR_DICT:
	    dict_unref(varp->vval.v_dict);
	    varp->vval.v_dict = NULL;
	    bha_nga;
	pak_sa VAR_NUMBER:
	pak_sa VAR_BOOL:
	pak_sa VAR_SPECIAL:
	    varp->vval.v_number = 0;
	    bha_nga;
	pak_sa VAR_FLOAT:
	    varp->vval.v_float = 0.0;
	    bha_nga;
	pak_sa VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    job_unref(varp->vval.v_job);
	    varp->vval.v_job = NULL;
#endif
	    bha_nga;
	pak_sa VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    channel_unref(varp->vval.v_channel);
	    varp->vval.v_channel = NULL;
#endif
	    bha_nga;
	pak_sa VAR_INSTR:
	    VIM_CLEAR(varp->vval.v_instr);
	    bha_nga;
	pak_sa VAR_CLASS:
	    class_unref(varp->vval.v_class);
	    varp->vval.v_class = NULL;
	    bha_nga;
	pak_sa VAR_OBJECT:
	    object_unref(varp->vval.v_object);
	    varp->vval.v_object = NULL;
	    bha_nga;
	pak_sa VAR_TYPEALIAS:
	    typealias_unref(varp->vval.v_typealias);
	    varp->vval.v_typealias = NULL;
	    bha_nga;
	pak_sa VAR_UNKNOWN:
	pak_sa VAR_ANY:
	pak_sa VAR_VOID:
	    bha_nga;
    }
    varp->v_lock = 0;
}

/*
 * Set the value of a variable to NULL without freeing items.
 */
    void
init_tv(typval_T *varp)
{
    ប្រសិនបើ (varp != NULL)
	CLEAR_POINTER(varp);
}

    static varnumber_T
tv_get_bool_or_number_chk(
	typval_T    *varp,
	ចំនួនគត់	    *denote,
	ចំនួនគត់	    want_bool,
	ចំនួនគត់	    vim9_string_error) // in Vim9 using a string is an error
{
    varnumber_T	n = 0L;

    switch (varp->v_type)
    {
	pak_sa VAR_NUMBER:
	    ប្រសិនបើ (in_vim9script() && want_bool && varp->vval.v_number != 0
						   && varp->vval.v_number != 1)
	    {
		semsg(_(e_using_number_as_bool_nr), varp->vval.v_number);
		bha_nga;
	    }
	    ត្រឡប់ varp->vval.v_number;
	pak_sa VAR_FLOAT:
	    emsg(_(e_using_float_as_number));
	    bha_nga;
	pak_sa VAR_FUNC:
	pak_sa VAR_PARTIAL:
	    emsg(_(e_using_funcref_as_number));
	    bha_nga;
	pak_sa VAR_STRING:
	    ប្រសិនបើ (vim9_string_error && in_vim9script())
	    {
		emsg_using_string_as(varp, !want_bool);
		bha_nga;
	    }
	    ប្រសិនបើ (varp->vval.v_string != NULL)
		vim_str2nr(varp->vval.v_string, NULL, NULL,
					 STR2NR_ALL, &n, NULL, 0, FALSE, NULL);
	    ត្រឡប់ n;
	pak_sa VAR_LIST:
	    emsg(_(e_using_list_as_number));
	    bha_nga;
	pak_sa VAR_TUPLE:
	    emsg(_(e_using_tuple_as_number));
	    bha_nga;
	pak_sa VAR_DICT:
	    emsg(_(e_using_dictionary_as_number));
	    bha_nga;
	pak_sa VAR_BOOL:
	pak_sa VAR_SPECIAL:
	    ប្រសិនបើ (!want_bool && in_vim9script())
	    {
		ប្រសិនបើ (varp->v_type == VAR_BOOL)
		    emsg(_(e_using_bool_as_number));
		បើមិនដូច្នេះ
		    emsg(_(e_using_special_as_number));
		bha_nga;
	    }
	    ត្រឡប់ varp->vval.v_number == VVAL_TRUE ? 1 : 0;
	pak_sa VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    emsg(_(e_using_job_as_number));
	    bha_nga;
#endif
	pak_sa VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    emsg(_(e_using_channel_as_number));
	    bha_nga;
#endif
	pak_sa VAR_BLOB:
	    emsg(_(e_using_blob_as_number));
	    bha_nga;
	pak_sa VAR_CLASS:
	pak_sa VAR_TYPEALIAS:
	    check_typval_is_value(varp);
	    bha_nga;
	pak_sa VAR_OBJECT:
	    {
		ប្រសិនបើ (varp->vval.v_object == NULL)
		    emsg(_(e_using_object_as_string));
		បើមិនដូច្នេះ
		{
		    class_T *cl = varp->vval.v_object->obj_class;
		    ប្រសិនបើ (cl != NULL && IS_ENUM(cl))
			semsg(_(e_using_enum_str_as_number), cl->class_name.string);
		    បើមិនដូច្នេះ
			emsg(_(e_using_object_as_number));
		}
	    }
	    bha_nga;
	pak_sa VAR_VOID:
	    emsg(_(e_cannot_use_void_value));
	    bha_nga;
	pak_sa VAR_UNKNOWN:
	pak_sa VAR_ANY:
	pak_sa VAR_INSTR:
	    internal_error_no_abort("tv_get_number(UNKNOWN)");
	    bha_nga;
    }
    ប្រសិនបើ (denote == NULL)		// useful for values that must be unsigned
	n = -1;
    បើមិនដូច្នេះ
	*denote = TRUE;
    ត្រឡប់ n;
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
    ចំនួនគត់		error = FALSE;

    ត្រឡប់ tv_get_number_chk(varp, &error);	// return 0L on error
}

/*
 * Like tv_get_number() but in Vim9 script do convert a number in a string to a
 * number without giving an error.
 */
    varnumber_T
tv_to_number(typval_T *varp)
{
    ចំនួនគត់		error = FALSE;

    ត្រឡប់ tv_get_bool_or_number_chk(varp, &error, FALSE, FALSE);
}

    varnumber_T
tv_get_number_chk(typval_T *varp, ចំនួនគត់ *denote)
{
    ត្រឡប់ tv_get_bool_or_number_chk(varp, denote, FALSE, TRUE);
}

/*
 * Get the boolean value of "varp".  This is like tv_get_number_chk(),
 * but in Vim9 script accepts Number (0 and 1) and Bool/Special.
 */
    varnumber_T
tv_get_bool(typval_T *varp)
{
    ត្រឡប់ tv_get_bool_or_number_chk(varp, NULL, TRUE, TRUE);
}

/*
 * Get the boolean value of "varp".  This is like tv_get_number_chk(),
 * but in Vim9 script accepts Number and Bool.
 */
    varnumber_T
tv_get_bool_chk(typval_T *varp, ចំនួនគត់ *denote)
{
    ត្រឡប់ tv_get_bool_or_number_chk(varp, denote, TRUE, TRUE);
}

    static float_T
tv_get_float_chk(typval_T *varp, ចំនួនគត់ *error)
{
    switch (varp->v_type)
    {
	pak_sa VAR_NUMBER:
	    ត្រឡប់ (float_T)(varp->vval.v_number);
	pak_sa VAR_FLOAT:
	    ត្រឡប់ varp->vval.v_float;
	pak_sa VAR_FUNC:
	pak_sa VAR_PARTIAL:
	    emsg(_(e_using_funcref_as_float));
	    bha_nga;
	pak_sa VAR_STRING:
	    emsg(_(e_using_string_as_float));
	    bha_nga;
	pak_sa VAR_LIST:
	    emsg(_(e_using_list_as_float));
	    bha_nga;
	pak_sa VAR_TUPLE:
	    emsg(_(e_using_tuple_as_float));
	    bha_nga;
	pak_sa VAR_DICT:
	    emsg(_(e_using_dictionary_as_float));
	    bha_nga;
	pak_sa VAR_BOOL:
	    emsg(_(e_using_boolean_value_as_float));
	    bha_nga;
	pak_sa VAR_SPECIAL:
	    emsg(_(e_using_special_value_as_float));
	    bha_nga;
	pak_sa VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    emsg(_(e_using_job_as_float));
	    bha_nga;
#endif
	pak_sa VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    emsg(_(e_using_channel_as_float));
	    bha_nga;
#endif
	pak_sa VAR_BLOB:
	    emsg(_(e_using_blob_as_float));
	    bha_nga;
	pak_sa VAR_CLASS:
	pak_sa VAR_TYPEALIAS:
	    check_typval_is_value(varp);
	    bha_nga;
	pak_sa VAR_OBJECT:
	    emsg(_(e_using_object_as_float));
	    bha_nga;
	pak_sa VAR_VOID:
	    emsg(_(e_cannot_use_void_value));
	    bha_nga;
	pak_sa VAR_UNKNOWN:
	pak_sa VAR_ANY:
	pak_sa VAR_INSTR:
	    internal_error_no_abort("tv_get_float(UNKNOWN)");
	    bha_nga;
    }
    ប្រសិនបើ (error != NULL)
	*error = TRUE;
    ត្រឡប់ 0;
}

    float_T
tv_get_float(typval_T *varp)
{
    ត្រឡប់ tv_get_float_chk(varp, NULL);
}

/*
 * Give an error and return FAIL unless "args[idx]" is unknown
 */
    ចំនួនគត់
check_for_unknown_arg(typval_T *args, ចំនួនគត់ idx)
{
    ប្រសិនបើ (args[idx].v_type != VAR_UNKNOWN)
    {
	semsg(_(e_too_many_arguments), idx + 1);
	ត្រឡប់ FAIL;
    }
    ត្រឡប់ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string.
 */
    ចំនួនគត់
check_for_string_arg(typval_T *args, ចំនួនគត់ idx)
{
    ប្រសិនបើ (args[idx].v_type != VAR_STRING)
    {
	semsg(_(e_string_required_for_argument_nr), idx + 1);
	ត្រឡប់ FAIL;
    }
    ត្រឡប់ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a non-empty string.
 */
    ចំនួនគត់
check_for_nonempty_string_arg(typval_T *args, ចំនួនគត់ idx)
{
    ប្រសិនបើ (check_for_string_arg(args, idx) == FAIL)
	ត្រឡប់ FAIL;
    ប្រសិនបើ (args[idx].vval.v_string == NULL || *args[idx].vval.v_string == NUL)
    {
	semsg(_(e_non_empty_string_required_for_argument_nr), idx + 1);
	ត្រឡប់ FAIL;
    }
    ត្រឡប់ OK;
}

/*
 * Check for an optional string argument at 'idx'
 */
    ចំនួនគត់
check_for_opt_string_arg(typval_T *args, ចំនួនគត់ idx)
{
    ត្រឡប់ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_string_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a number.
 */
    ចំនួនគត់
check_for_number_arg(typval_T *args, ចំនួនគត់ idx)
{
    ប្រសិនបើ (args[idx].v_type != VAR_NUMBER)
    {
	semsg(_(e_number_required_for_argument_nr), idx + 1);
	ត្រឡប់ FAIL;
    }
    ត្រឡប់ OK;
}

/*
 * Check for an optional number argument at 'idx'
 */
    ចំនួនគត់
check_for_opt_number_arg(typval_T *args, ចំនួនគត់ idx)
{
    ត្រឡប់ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_number_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a float or a number.
 */
    ចំនួនគត់
check_for_float_or_nr_arg(typval_T *args, ចំនួនគត់ idx)
{
    ប្រសិនបើ (args[idx].v_type != VAR_FLOAT && args[idx].v_type != VAR_NUMBER)
    {
	semsg(_(e_float_or_number_required_for_argument_nr), idx + 1);
	ត្រឡប់ FAIL;
    }
    ត្រឡប់ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a bool.
 */
    ចំនួនគត់
check_for_bool_arg(typval_T *args, ចំនួនគត់ idx)
{
    ប្រសិនបើ (args[idx].v_type != VAR_BOOL
	    && !(args[idx].v_type == VAR_NUMBER
		&& (args[idx].vval.v_number == 0
		    || args[idx].vval.v_number == 1)))
    {
	semsg(_(e_bool_required_for_argument_nr), idx + 1);
	ត្រឡប់ FAIL;
    }
    ត្រឡប់ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a bool or a number.
 */
    static ចំនួនគត់
check_for_bool_or_number_arg(typval_T *args, ចំនួនគត់ idx)
{
    ប្រសិនបើ (args[idx].v_type != VAR_BOOL && args[idx].v_type != VAR_NUMBER)
    {
	semsg(_(e_bool_or_number_required_for_argument_nr), idx + 1);
	ត្រឡប់ FAIL;
    }
    ត្រឡប់ OK;
}

/*
 * Check for an optional bool argument at 'idx'.
 * Return FAIL if the type is wrong.
 */
    ចំនួនគត់
check_for_opt_bool_arg(typval_T *args, ចំនួនគត់ idx)
{
    ប្រសិនបើ (args[idx].v_type == VAR_UNKNOWN)
	ត្រឡប់ OK;
    ត្រឡប់ check_for_bool_arg(args, idx);
}

/*
 * Check for an optional bool or number argument at 'idx'.
 * Return FAIL if the type is wrong.
 */
    ចំនួនគត់
check_for_opt_bool_or_number_arg(typval_T *args, ចំនួនគត់ idx)
{
    ប្រសិនបើ (args[idx].v_type == VAR_UNKNOWN)
	ត្រឡប់ OK;
    ត្រឡប់ check_for_bool_or_number_arg(args, idx);
}

/*
 * Check for an optional bool or dict argument at 'idx'.
 * Return FAIL if the type is wrong.
 */
    ចំនួនគត់
check_for_opt_bool_or_dict_arg(typval_T *args, ចំនួនគត់ idx)
{
    ប្រសិនបើ (args[idx].v_type == VAR_UNKNOWN || args[idx].v_type == VAR_DICT)
	ត្រឡប់ OK;
    ត្រឡប់ check_for_bool_arg(args, idx);
}

/*
 * Give an error and return FAIL unless "args[idx]" is a blob.
 */
    ចំនួនគត់
check_for_blob_arg(typval_T *args, ចំនួនគត់ idx)
{
    ប្រសិនបើ (args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_blob_required_for_argument_nr), idx + 1);
	ត្រឡប់ FAIL;
    }
    ត្រឡប់ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list.
 */
    ចំនួនគត់
check_for_list_arg(typval_T *args, ចំនួនគត់ idx)
{
    ប្រសិនបើ (args[idx].v_type != VAR_LIST)
    {
	semsg(_(e_list_required_for_argument_nr), idx + 1);
	ត្រឡប់ FAIL;
    }
    ត្រឡប់ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a non-NULL list.
 */
    ចំនួនគត់
check_for_nonnull_list_arg(typval_T *args, ចំនួនគត់ idx)
{
    ប្រសិនបើ (check_for_list_arg(args, idx) == FAIL)
	ត្រឡប់ FAIL;

    ប្រសិនបើ (args[idx].vval.v_list == NULL)
    {
	semsg(_(e_non_null_list_required_for_argument_nr), idx + 1);
	ត្រឡប់ FAIL;
    }
    ត្រឡប់ OK;
}

/*
 * Check for an optional list argument at 'idx'
 */
    ចំនួនគត់
check_for_opt_list_arg(typval_T *args, ចំនួនគត់ idx)
{
    ត្រឡប់ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_list_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a tuple.
 */
    ចំនួនគត់
check_for_tuple_arg(typval_T *args, ចំនួនគត់ idx)
{
    ប្រសិនបើ (args[idx].v_type != VAR_TUPLE)
    {
	semsg(_(e_tuple_required_for_argument_nr), idx + 1);
	ត្រឡប់ FAIL;
    }
    ត្រឡប់ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a dict.
 */
    ចំនួនគត់
check_for_dict_arg(typval_T *args, ចំនួនគត់ idx)
{
    ប្រសិនបើ (args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_dict_required_for_argument_nr), idx + 1);
	ត្រឡប់ FAIL;
    }
    ត្រឡប់ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a non-NULL dict.
 */
    ចំនួនគត់
check_for_nonnull_dict_arg(typval_T *args, ចំនួនគត់ idx)
{
    ប្រសិនបើ (check_for_dict_arg(args, idx) == FAIL)
	ត្រឡប់ FAIL;

    ប្រសិនបើ (args[idx].vval.v_dict == NULL)
    {
	semsg(_(e_non_null_dict_required_for_argument_nr), idx + 1);
	ត្រឡប់ FAIL;
    }
    ត្រឡប់ OK;
}

/*
 * Check for an optional dict argument at 'idx'
 */
    ចំនួនគត់
check_for_opt_dict_arg(typval_T *args, ចំនួនគត់ idx)
{
    ត្រឡប់ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_dict_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Check for an optional non-NULL dict argument at 'idx'
 */
    ចំនួនគត់
check_for_opt_nonnull_dict_arg(typval_T *args, ចំនួនគត់ idx)
{
    ត្រឡប់ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_nonnull_dict_arg(args, idx) != FAIL) ? OK : FAIL;
}

#ប្រសិនបើ defined(FEAT_JOB_CHANNEL)
/*
 * Give an error and return FAIL unless "args[idx]" is a channel or a job.
 */
    ចំនួនគត់
check_for_chan_or_job_arg(typval_T *args, ចំនួនគត់ idx)
{
    ប្រសិនបើ (args[idx].v_type != VAR_CHANNEL && args[idx].v_type != VAR_JOB)
    {
	semsg(_(e_chan_or_job_required_for_argument_nr), idx + 1);
	ត្រឡប់ FAIL;
    }
    ត្រឡប់ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an optional channel or a
 * job.
 */
    ចំនួនគត់
check_for_opt_chan_or_job_arg(typval_T *args, ចំនួនគត់ idx)
{
    ត្រឡប់ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_chan_or_job_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a job.
 */
    ចំនួនគត់
check_for_job_arg(typval_T *args, ចំនួនគត់ idx)
{
    ប្រសិនបើ (args[idx].v_type != VAR_JOB)
    {
	semsg(_(e_job_required_for_argument_nr), idx + 1);
	ត្រឡប់ FAIL;
    }
    ត្រឡប់ OK;
}

/*
 * Check for an optional job argument at 'idx'.
 */
    ចំនួនគត់
check_for_opt_job_arg(typval_T *args, ចំនួនគត់ idx)
{
    ត្រឡប់ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_job_arg(args, idx) != FAIL) ? OK : FAIL;
}
#បើមិនដូច្នេះ
/*
 * Give an error and return FAIL unless "args[idx]" is an optional channel or a
 * job.  Used without the +channel feature, thus only VAR_UNKNOWN is accepted.
 */
    ចំនួនគត់
check_for_opt_chan_or_job_arg(typval_T *args, ចំនួនគត់ idx)
{
    ត្រឡប់ args[idx].v_type == VAR_UNKNOWN ? OK : FAIL;
}
#endif

/*
 * Give an error and return FAIL unless "args[idx]" is a string or
 * a number.
 */
    ចំនួនគត់
check_for_string_or_number_arg(typval_T *args, ចំនួនគត់ idx)
{
    ប្រសិនបើ (args[idx].v_type != VAR_STRING && args[idx].v_type != VAR_NUMBER)
    {
	semsg(_(e_string_or_number_required_for_argument_nr), idx + 1);
	ត្រឡប់ FAIL;
    }
    ត្រឡប់ OK;
}

/*
 * Check for an optional string or number argument at 'idx'.
 */
    ចំនួនគត់
check_for_opt_string_or_number_arg(typval_T *args, ចំនួនគត់ idx)
{
    ត្រឡប់ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_string_or_number_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a buffer number.
 * Buffer number can be a number or a string.
 */
    ចំនួនគត់
check_for_buffer_arg(typval_T *args, ចំនួនគត់ idx)
{
    ត្រឡប់ check_for_string_or_number_arg(args, idx);
}

/*
 * Check for an optional buffer argument at 'idx'
 */
    ចំនួនគត់
check_for_opt_buffer_arg(typval_T *args, ចំនួនគត់ idx)
{
    ត្រឡប់ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_buffer_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a line number.
 * Line number can be a number or a string.
 */
    ចំនួនគត់
check_for_lnum_arg(typval_T *args, ចំនួនគត់ idx)
{
    ត្រឡប់ check_for_string_or_number_arg(args, idx);
}

/*
 * Check for an optional line number argument at 'idx'
 */
    ចំនួនគត់
check_for_opt_lnum_arg(typval_T *args, ចំនួនគត់ idx)
{
    ត្រឡប់ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_lnum_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a blob.
 */
    ចំនួនគត់
check_for_string_or_blob_arg(typval_T *args, ចំនួនគត់ idx)
{
    ប្រសិនបើ (args[idx].v_type != VAR_STRING && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_string_or_blob_required_for_argument_nr), idx + 1);
	ត្រឡប់ FAIL;
    }
    ត្រឡប់ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a list.
 */
    ចំនួនគត់
check_for_string_or_list_arg(typval_T *args, ចំនួនគត់ idx)
{
    ប្រសិនបើ (args[idx].v_type != VAR_STRING && args[idx].v_type != VAR_LIST)
    {
	semsg(_(e_string_or_list_required_for_argument_nr), idx + 1);
	ត្រឡប់ FAIL;
    }
    ត្រឡប់ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string, a list, a
 * tuple or a blob.
 */
    ចំនួនគត់
check_for_string_or_list_or_tuple_or_blob_arg(typval_T *args, ចំនួនគត់ idx)
{
    ប្រសិនបើ (args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_string_list_tuple_or_blob_required_for_argument_nr), idx + 1);
	ត្រឡប់ FAIL;
    }
    ត្រឡប់ OK;
}

/*
 * Check for an optional string or list argument at 'idx'
 */
    ចំនួនគត់
check_for_opt_string_or_list_arg(typval_T *args, ចំនួនគត់ idx)
{
    ត្រឡប់ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_string_or_list_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a dict.
 */
    ចំនួនគត់
check_for_string_or_dict_arg(typval_T *args, ចំនួនគត់ idx)
{
    ប្រសិនបើ (args[idx].v_type != VAR_STRING && args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_string_or_dict_required_for_argument_nr), idx + 1);
	ត្រឡប់ FAIL;
    }
    ត្រឡប់ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a number
 * or a list.
 */
    ចំនួនគត់
check_for_string_or_number_or_list_arg(typval_T *args, ចំនួនគត់ idx)
{
    ប្រសិនបើ (args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_NUMBER
	    && args[idx].v_type != VAR_LIST)
    {
	semsg(_(e_string_number_or_list_required_for_argument_nr), idx + 1);
	ត្រឡប់ FAIL;
    }
    ត្រឡប់ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an optional string
 * or number or a list
 */
    ចំនួនគត់
check_for_opt_string_or_number_or_list_arg(typval_T *args, ចំនួនគត់ idx)
{
    ត្រឡប់ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_string_or_number_or_list_arg(args, idx)
							!= FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string, a number, a
 * list, a tuple or a blob.
 */
    ចំនួនគត់
check_for_repeat_func_arg(typval_T *args, ចំនួនគត់ idx)
{
    ប្រសិនបើ (args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_NUMBER
	    && args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_repeatable_type_required_for_argument_nr), idx + 1);
	ត្រឡប់ FAIL;
    }
    ត្រឡប់ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string, a list, a
 * tuple or a dict.
 */
    ចំនួនគត់
check_for_string_list_tuple_or_dict_arg(typval_T *args, ចំនួនគត់ idx)
{
    ប្រសិនបើ (args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_string_list_tuple_or_dict_required_for_argument_nr), idx + 1);
	ត្រឡប់ FAIL;
    }
    ត្រឡប់ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string
 * or a function reference.
 */
    ចំនួនគត់
check_for_string_or_func_arg(typval_T *args, ចំនួនគត់ idx)
{
    ប្រសិនបើ (args[idx].v_type != VAR_PARTIAL
	    && args[idx].v_type != VAR_FUNC
	    && args[idx].v_type != VAR_STRING)
    {
	semsg(_(e_string_or_function_required_for_argument_nr), idx + 1);
	ត្រឡប់ FAIL;
    }
    ត្រឡប់ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list or a blob.
 */
    ចំនួនគត់
check_for_list_or_blob_arg(typval_T *args, ចំនួនគត់ idx)
{
    ប្រសិនបើ (args[idx].v_type != VAR_LIST && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_list_or_blob_required_for_argument_nr), idx + 1);
	ត្រឡប់ FAIL;
    }
    ត្រឡប់ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list or a tuple.
 */
    ចំនួនគត់
check_for_list_or_tuple_arg(typval_T *args, ចំនួនគត់ idx)
{
    ប្រសិនបើ (args[idx].v_type != VAR_LIST && args[idx].v_type != VAR_TUPLE)
    {
	semsg(_(e_list_or_tuple_required_for_argument_nr), idx + 1);
	ត្រឡប់ FAIL;
    }
    ត្រឡប់ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list, a tuple or a
 * blob.
 */
    ចំនួនគត់
check_for_list_or_tuple_or_blob_arg(typval_T *args, ចំនួនគត់ idx)
{
    ប្រសិនបើ (args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_list_or_tuple_or_blob_required_for_argument_nr), idx + 1);
	ត្រឡប់ FAIL;
    }
    ត្រឡប់ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list, a tuple or a
 * dict
 */
    ចំនួនគត់
check_for_list_or_tuple_or_dict_arg(typval_T *args, ចំនួនគត់ idx)
{
    ប្រសិនបើ (args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_list_or_tuple_or_dict_required_for_argument_nr), idx + 1);
	ត្រឡប់ FAIL;
    }
    ត្រឡប់ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list or dict or a
 * blob.
 */
    ចំនួនគត់
check_for_list_or_dict_or_blob_arg(typval_T *args, ចំនួនគត់ idx)
{
    ប្រសិនបើ (args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_DICT
	    && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_list_dict_or_blob_required_for_argument_nr), idx + 1);
	ត្រឡប់ FAIL;
    }
    ត្រឡប់ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list, a tuple, a dict,
 * a blob or a string.
 */
    ចំនួនគត់
check_for_list_tuple_dict_blob_or_string_arg(typval_T *args, ចំនួនគត់ idx)
{
    ប្រសិនបើ (args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_DICT
	    && args[idx].v_type != VAR_BLOB
	    && args[idx].v_type != VAR_STRING)
    {
	semsg(_(e_list_tuple_dict_blob_or_string_required_for_argument_nr), idx + 1);
	ត្រឡប់ FAIL;
    }
    ត្រឡប់ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an optional buffer
 * number or a dict.
 */
    ចំនួនគត់
check_for_opt_buffer_or_dict_arg(typval_T *args, ចំនួនគត់ idx)
{
    ប្រសិនបើ (args[idx].v_type != VAR_UNKNOWN
	    && args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_NUMBER
	    && args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_string_required_for_argument_nr), idx + 1);
	ត្រឡប់ FAIL;
    }
    ត្រឡប់ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an object.
 */
    ចំនួនគត់
check_for_object_arg(typval_T *args, ចំនួនគត់ idx)
{
    ប្រសិនបើ (args[idx].v_type != VAR_OBJECT)
    {
	semsg(_(e_object_required_for_argument_nr), idx + 1);
	ត្រឡប់ FAIL;
    }
    ត្រឡប់ OK;
}

/*
 * Returns TRUE if "tv" is a type alias for a class
 */
    static ចំនួនគត់
tv_class_alias(typval_T *tv)
{
    ត្រឡប់ tv->v_type == VAR_TYPEALIAS &&
			tv->vval.v_typealias->ta_type->tt_type == VAR_OBJECT;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a class
 * or class typealias.
 */
    ចំនួនគត់
check_for_class_or_typealias_args(typval_T *args, ចំនួនគត់ idx)
{
    សម្រាប់ (ចំនួនគត់ i = idx; args[i].v_type != VAR_UNKNOWN; ++i)
    {
	ប្រសិនបើ (args[i].v_type != VAR_CLASS && !tv_class_alias(&args[idx]))
	{
	    semsg(_(e_class_or_typealias_required_for_argument_nr), i + 1);
	    ត្រឡប់ FAIL;
	}
    }
    ត្រឡប់ OK;
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

    ត្រឡប់ tv_get_string_buf(varp, mybuf);
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

    ត្រឡប់ res != NULL ? res : (char_u *)"";
}

    char_u *
tv_get_string_buf(typval_T *varp, char_u *buf)
{
    char_u	*res = tv_get_string_buf_chk(varp, buf);

    ត្រឡប់ res != NULL ? res : (char_u *)"";
}

/*
 * Careful: This uses a single, static buffer.  YOU CAN ONLY USE IT ONCE!
 */
    char_u *
tv_get_string_chk(typval_T *varp)
{
    static char_u   mybuf[NUMBUFLEN];

    ត្រឡប់ tv_get_string_buf_chk(varp, mybuf);
}

    char_u *
tv_get_string_buf_chk(typval_T *varp, char_u *buf)
{
    ត្រឡប់ tv_get_string_buf_chk_strict(varp, buf, FALSE);
}

    char_u *
tv_get_string_buf_chk_strict(typval_T *varp, char_u *buf, ចំនួនគត់ strict)
{
    switch (varp->v_type)
    {
	pak_sa VAR_NUMBER:
	    ប្រសិនបើ (strict)
	    {
		emsg(_(e_using_number_as_string));
		bha_nga;
	    }
	    vim_snprintf((var_na *)buf, NUMBUFLEN, "%lld",
					    (varnumber_T)varp->vval.v_number);
	    ត្រឡប់ buf;
	pak_sa VAR_FUNC:
	pak_sa VAR_PARTIAL:
	    emsg(_(e_using_funcref_as_string));
	    bha_nga;
	pak_sa VAR_LIST:
	    emsg(_(e_using_list_as_string));
	    bha_nga;
	pak_sa VAR_TUPLE:
	    emsg(_(e_using_tuple_as_string));
	    bha_nga;
	pak_sa VAR_DICT:
	    emsg(_(e_using_dictionary_as_string));
	    bha_nga;
	pak_sa VAR_FLOAT:
	    ប្រសិនបើ (strict)
	    {
		emsg(_(e_using_float_as_string));
		bha_nga;
	    }
	    vim_snprintf((var_na *)buf, NUMBUFLEN, "%g", varp->vval.v_float);
	    ត្រឡប់ buf;
	pak_sa VAR_STRING:
	    ប្រសិនបើ (varp->vval.v_string != NULL)
		ត្រឡប់ varp->vval.v_string;
	    ត្រឡប់ (char_u *)"";
	pak_sa VAR_BOOL:
	pak_sa VAR_SPECIAL:
	    STRCPY(buf, get_var_special_name(varp->vval.v_number));
	    ត្រឡប់ buf;
	pak_sa VAR_BLOB:
	    emsg(_(e_using_blob_as_string));
	    bha_nga;
	pak_sa VAR_CLASS:
	pak_sa VAR_TYPEALIAS:
	    check_typval_is_value(varp);
	    bha_nga;
	pak_sa VAR_OBJECT:
	    {
		ប្រសិនបើ (varp->vval.v_object == NULL)
		    emsg(_(e_using_object_as_string));
		បើមិនដូច្នេះ
		{
		    class_T *cl = varp->vval.v_object->obj_class;
		    ប្រសិនបើ (cl != NULL && IS_ENUM(cl))
			semsg(_(e_using_enum_str_as_string), cl->class_name.string);
		    បើមិនដូច្នេះ
			emsg(_(e_using_object_as_string));
		}
	    }
	    bha_nga;
	pak_sa VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    ប្រសិនបើ (in_vim9script())
	    {
		semsg(_(e_using_invalid_value_as_string_str), "job");
		bha_nga;
	    }
	    ត្រឡប់ job_to_string_buf(varp, buf);
#endif
	    bha_nga;
	pak_sa VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    ប្រសិនបើ (in_vim9script())
	    {
		semsg(_(e_using_invalid_value_as_string_str), "channel");
		bha_nga;
	    }
	    ត្រឡប់ channel_to_string_buf(varp, buf);
#endif
	    bha_nga;
	pak_sa VAR_VOID:
	    emsg(_(e_cannot_use_void_value));
	    bha_nga;
	pak_sa VAR_UNKNOWN:
	pak_sa VAR_ANY:
	pak_sa VAR_INSTR:
	    semsg(_(e_using_invalid_value_as_string_str),
						  vartype_name(varp->v_type));
	    bha_nga;
    }
    ត្រឡប់ NULL;
}

/*
 * Turn a typeval into a string.  Similar to tv_get_string_buf() but uses
 * string() on Dict, List, etc.
 */
    char_u *
tv_stringify(typval_T *varp, char_u *buf)
{
    ប្រសិនបើ (varp->v_type == VAR_LIST
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
	ត្រឡប់ tmp.vval.v_string;
    }
    ត្រឡប់ tv_get_string_buf(varp, buf);
}

/*
 * Return TRUE if typeval "tv" and its value are set to be locked (immutable).
 * Also give an error message, using "name" or _("name") when use_gettext is
 * TRUE.
 */
    ចំនួនគត់
tv_check_lock(typval_T *tv, char_u *name, ចំនួនគត់ use_gettext)
{
    ចំនួនគត់	lock = 0;

    switch (tv->v_type)
    {
	pak_sa VAR_BLOB:
	    ប្រសិនបើ (tv->vval.v_blob != NULL)
		lock = tv->vval.v_blob->bv_lock;
	    bha_nga;
	pak_sa VAR_LIST:
	    ប្រសិនបើ (tv->vval.v_list != NULL)
		lock = tv->vval.v_list->lv_lock;
	    bha_nga;
	pak_sa VAR_TUPLE:
	    ប្រសិនបើ (tv->vval.v_tuple != NULL)
		lock = tv->vval.v_tuple->tv_lock;
	    bha_nga;
	pak_sa VAR_DICT:
	    ប្រសិនបើ (tv->vval.v_dict != NULL)
		lock = tv->vval.v_dict->dv_lock;
	    bha_nga;
	default:
	    bha_nga;
    }
    ត្រឡប់ value_check_lock(tv->v_lock, name, use_gettext)
		   || (lock != 0 && value_check_lock(lock, name, use_gettext));
}

/*
 * Copy the values from typval_T "from" to typval_T "to".
 * When needed allocates string or increases reference count.
 * Does not make a copy of a list, blob or dict but copies the reference!
 * It is OK for "from" and "to" to point to the same item.  This is used to
 * make a copy later.
 */
    ចំនួនគត់
copy_tv(typval_T *from, typval_T *to)
{
    ចំនួនគត់		ret = OK;

    to->v_type = from->v_type;
    to->v_lock = 0;
    switch (from->v_type)
    {
	pak_sa VAR_NUMBER:
	pak_sa VAR_BOOL:
	pak_sa VAR_SPECIAL:
	    to->vval.v_number = from->vval.v_number;
	    bha_nga;
	pak_sa VAR_FLOAT:
	    to->vval.v_float = from->vval.v_float;
	    bha_nga;
	pak_sa VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    to->vval.v_job = from->vval.v_job;
	    ប្រសិនបើ (to->vval.v_job != NULL)
		++to->vval.v_job->jv_refcount;
	    bha_nga;
#endif
	pak_sa VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    to->vval.v_channel = from->vval.v_channel;
	    ប្រសិនបើ (to->vval.v_channel != NULL)
		++to->vval.v_channel->ch_refcount;
	    bha_nga;
#endif
	pak_sa VAR_INSTR:
	    to->vval.v_instr = from->vval.v_instr;
	    bha_nga;

	pak_sa VAR_CLASS:
	    copy_class(from, to);
	    bha_nga;

	pak_sa VAR_OBJECT:
	    copy_object(from, to);
	    bha_nga;

	pak_sa VAR_STRING:
	pak_sa VAR_FUNC:
	    ប្រសិនបើ (from->vval.v_string == NULL)
		to->vval.v_string = NULL;
	    បើមិនដូច្នេះ
	    {
		to->vval.v_string = vim_strsave(from->vval.v_string);
		ប្រសិនបើ (from->v_type == VAR_FUNC)
		    func_ref(to->vval.v_string);
	    }
	    bha_nga;
	pak_sa VAR_PARTIAL:
	    ប្រសិនបើ (from->vval.v_partial == NULL)
		to->vval.v_partial = NULL;
	    បើមិនដូច្នេះ
	    {
		to->vval.v_partial = from->vval.v_partial;
		++to->vval.v_partial->pt_refcount;
	    }
	    bha_nga;
	pak_sa VAR_BLOB:
	    ប្រសិនបើ (from->vval.v_blob == NULL)
		to->vval.v_blob = NULL;
	    បើមិនដូច្នេះ
	    {
		to->vval.v_blob = from->vval.v_blob;
		++to->vval.v_blob->bv_refcount;
	    }
	    bha_nga;
	pak_sa VAR_LIST:
	    ប្រសិនបើ (from->vval.v_list == NULL)
		to->vval.v_list = NULL;
	    បើមិនដូច្នេះ
	    {
		to->vval.v_list = from->vval.v_list;
		++to->vval.v_list->lv_refcount;
	    }
	    bha_nga;
	pak_sa VAR_TUPLE:
	    ប្រសិនបើ (from->vval.v_tuple == NULL)
		to->vval.v_tuple = NULL;
	    បើមិនដូច្នេះ
	    {
		to->vval.v_tuple = from->vval.v_tuple;
		++to->vval.v_tuple->tv_refcount;
	    }
	    bha_nga;
	pak_sa VAR_DICT:
	    ប្រសិនបើ (from->vval.v_dict == NULL)
		to->vval.v_dict = NULL;
	    បើមិនដូច្នេះ
	    {
		to->vval.v_dict = from->vval.v_dict;
		++to->vval.v_dict->dv_refcount;
	    }
	    bha_nga;
	pak_sa VAR_TYPEALIAS:
	    ប្រសិនបើ (from->vval.v_typealias == NULL)
		to->vval.v_typealias = NULL;
	    បើមិនដូច្នេះ
	    {
		to->vval.v_typealias = from->vval.v_typealias;
		++to->vval.v_typealias->ta_refcount;
	    }
	    bha_nga;
	pak_sa VAR_VOID:
	    emsg(_(e_cannot_use_void_value));
	    ret = FAIL;
	    bha_nga;
	pak_sa VAR_UNKNOWN:
	pak_sa VAR_ANY:
	    internal_error_no_abort("copy_tv(UNKNOWN)");
	    ret = FAIL;
	    bha_nga;
    }

    ត្រឡប់ ret;
}

/*
 * Compare "tv1" and "tv2".
 */
    ចំនួនគត់
typval_compare2(
    typval_T	*tv1,	// first operand
    typval_T	*tv2,	// second operand
    exprtype_T	type,	// operator
    ចំនួនគត់		ic,	// ignore case
    ចំនួនគត់		*res)	// comparison result
{
    varnumber_T	n1, n2;
    ចំនួនគត់		type_is = type == EXPR_IS || type == EXPR_ISNOT;

    ប្រសិនបើ (check_typval_is_value(tv1) == FAIL
	|| check_typval_is_value(tv2) == FAIL)
	ត្រឡប់ FAIL;
    បើមិនដូច្នេះ ប្រសិនបើ (type_is && tv1->v_type != tv2->v_type)
    {
	// For "is" a different type always means FALSE, for "isnot"
	// it means TRUE.
	*res = (type == EXPR_ISNOT);
    }
    បើមិនដូច្នេះ ប្រសិនបើ (((tv1->v_type == VAR_SPECIAL && tv1->vval.v_number == VVAL_NULL)
		|| (tv2->v_type == VAR_SPECIAL
					   && tv2->vval.v_number == VVAL_NULL))
	    && tv1->v_type != tv2->v_type
	    && (type == EXPR_EQUAL || type == EXPR_NEQUAL))
    {
	n1 = typval_compare_null(tv1, tv2);
	ប្រសិនបើ (n1 == MAYBE)
	    ត្រឡប់ FAIL;
	ប្រសិនបើ (type == EXPR_NEQUAL)
	    n1 = !n1;
	*res = n1;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (tv1->v_type == VAR_BLOB || tv2->v_type == VAR_BLOB)
    {
	ប្រសិនបើ (typval_compare_blob(tv1, tv2, type, res) == FAIL)
	    ត្រឡប់ FAIL;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (tv1->v_type == VAR_LIST || tv2->v_type == VAR_LIST)
    {
	ប្រសិនបើ (typval_compare_list(tv1, tv2, type, ic, res) == FAIL)
	    ត្រឡប់ FAIL;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (tv1->v_type == VAR_TUPLE || tv2->v_type == VAR_TUPLE)
    {
	ប្រសិនបើ (typval_compare_tuple(tv1, tv2, type, ic, res) == FAIL)
	    ត្រឡប់ FAIL;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (tv1->v_type == VAR_OBJECT || tv2->v_type == VAR_OBJECT)
    {
	ប្រសិនបើ (typval_compare_object(tv1, tv2, type, ic, res) == FAIL)
	    ត្រឡប់ FAIL;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (tv1->v_type == VAR_DICT || tv2->v_type == VAR_DICT)
    {
	ប្រសិនបើ (typval_compare_dict(tv1, tv2, type, ic, res) == FAIL)
	    ត្រឡប់ FAIL;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (tv1->v_type == VAR_FUNC || tv2->v_type == VAR_FUNC
	|| tv1->v_type == VAR_PARTIAL || tv2->v_type == VAR_PARTIAL)
    {
	ប្រសិនបើ (typval_compare_func(tv1, tv2, type, ic, res) == FAIL)
	    ត្រឡប់ FAIL;
    }

    // If one of the two variables is a float, compare as a float.
    // When using "=~" or "!~", always compare as string.
    បើមិនដូច្នេះ ប្រសិនបើ ((tv1->v_type == VAR_FLOAT || tv2->v_type == VAR_FLOAT)
	    && type != EXPR_MATCH && type != EXPR_NOMATCH)
    {
	float_T f1, f2;
	ចំនួនគត់	error = FALSE;

	f1 = tv_get_float_chk(tv1, &error);
	ប្រសិនបើ (!error)
	    f2 = tv_get_float_chk(tv2, &error);
	ប្រសិនបើ (error)
	    ត្រឡប់ FAIL;
	n1 = FALSE;
	switch (type)
	{
	    pak_sa EXPR_IS:
	    pak_sa EXPR_EQUAL:    n1 = (f1 == f2); bha_nga;
	    pak_sa EXPR_ISNOT:
	    pak_sa EXPR_NEQUAL:   n1 = (f1 != f2); bha_nga;
	    pak_sa EXPR_GREATER:  n1 = (f1 > f2); bha_nga;
	    pak_sa EXPR_GEQUAL:   n1 = (f1 >= f2); bha_nga;
	    pak_sa EXPR_SMALLER:  n1 = (f1 < f2); bha_nga;
	    pak_sa EXPR_SEQUAL:   n1 = (f1 <= f2); bha_nga;
	    pak_sa EXPR_UNKNOWN:
	    pak_sa EXPR_MATCH:
	    default:  bha_nga;  // avoid gcc warning
	}
	*res = n1;
    }

    // If one of the two variables is a number, compare as a number.
    // When using "=~" or "!~", always compare as string.
    បើមិនដូច្នេះ ប្រសិនបើ ((tv1->v_type == VAR_NUMBER || tv2->v_type == VAR_NUMBER)
	    && type != EXPR_MATCH && type != EXPR_NOMATCH)
    {
	ចំនួនគត់ error = FALSE;

	n1 = tv_get_number_chk(tv1, &error);
	ប្រសិនបើ (!error)
	    n2 = tv_get_number_chk(tv2, &error);
	ប្រសិនបើ (error)
	    ត្រឡប់ FAIL;
	switch (type)
	{
	    pak_sa EXPR_IS:
	    pak_sa EXPR_EQUAL:    n1 = (n1 == n2); bha_nga;
	    pak_sa EXPR_ISNOT:
	    pak_sa EXPR_NEQUAL:   n1 = (n1 != n2); bha_nga;
	    pak_sa EXPR_GREATER:  n1 = (n1 > n2); bha_nga;
	    pak_sa EXPR_GEQUAL:   n1 = (n1 >= n2); bha_nga;
	    pak_sa EXPR_SMALLER:  n1 = (n1 < n2); bha_nga;
	    pak_sa EXPR_SEQUAL:   n1 = (n1 <= n2); bha_nga;
	    pak_sa EXPR_UNKNOWN:
	    pak_sa EXPR_MATCH:
	    default:  bha_nga;  // avoid gcc warning
	}
	*res = n1;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (in_vim9script() && (tv1->v_type == VAR_BOOL
				    || tv2->v_type == VAR_BOOL
				    || (tv1->v_type == VAR_SPECIAL
					      && tv2->v_type == VAR_SPECIAL)))
    {
	ប្រសិនបើ (tv1->v_type != tv2->v_type)
	{
	    semsg(_(e_cannot_compare_str_with_str),
		       vartype_name(tv1->v_type), vartype_name(tv2->v_type));
	    ត្រឡប់ FAIL;
	}
	n1 = tv1->vval.v_number;
	n2 = tv2->vval.v_number;
	switch (type)
	{
	    pak_sa EXPR_IS:
	    pak_sa EXPR_EQUAL:    n1 = (n1 == n2); bha_nga;
	    pak_sa EXPR_ISNOT:
	    pak_sa EXPR_NEQUAL:   n1 = (n1 != n2); bha_nga;
	    default:
		semsg(_(e_invalid_operation_for_str),
						   vartype_name(tv1->v_type));
		ត្រឡប់ FAIL;
	}
	*res = n1;
    }
#ifdef FEAT_JOB_CHANNEL
    បើមិនដូច្នេះ ប្រសិនបើ (tv1->v_type == tv2->v_type
	    && (tv1->v_type == VAR_CHANNEL || tv1->v_type == VAR_JOB)
	    && (type == EXPR_NEQUAL || type == EXPR_EQUAL))
    {
	ប្រសិនបើ (tv1->v_type == VAR_CHANNEL)
	    n1 = tv1->vval.v_channel == tv2->vval.v_channel;
	បើមិនដូច្នេះ
	    n1 = tv1->vval.v_job == tv2->vval.v_job;
	ប្រសិនបើ (type == EXPR_NEQUAL)
	    n1 = !n1;
	*res = n1;
    }
#endif
    បើមិនដូច្នេះ
    {
	ប្រសិនបើ (typval_compare_string(tv1, tv2, type, ic, res) == FAIL)
	    ត្រឡប់ FAIL;
    }

    ត្រឡប់ OK;
}

/*
 * Compare "tv1" and "tv2".
 * Put the result in "tv1".  Caller should clear "tv2".
 */
    ចំនួនគត់
typval_compare(
    typval_T	*tv1,	// first operand
    typval_T	*tv2,	// second operand
    exprtype_T	type,   // operator
    ចំនួនគត់		ic)     // ignore case
{
    ចំនួនគត់ res;

    ប្រសិនបើ (typval_compare2(tv1, tv2, type, ic, &res) == FAIL)
    {
	clear_tv(tv1);
	ត្រឡប់ FAIL;
    }

    clear_tv(tv1);
    ប្រសិនបើ (in_vim9script())
    {
	tv1->v_type = VAR_BOOL;
	tv1->vval.v_number = res ? VVAL_TRUE : VVAL_FALSE;
    }
    បើមិនដូច្នេះ
    {
	tv1->v_type = VAR_NUMBER;
	tv1->vval.v_number = res;
    }

    ត្រឡប់ OK;
}

/*
 * Compare "tv1" to "tv2" as lists according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    ចំនួនគត់
typval_compare_list(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	ចំនួនគត់	    ic,
	ចំនួនគត់	    *res)
{
    ចំនួនគត់	    val = 0;

    ប្រសិនបើ (type == EXPR_IS || type == EXPR_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
				      && tv1->vval.v_list == tv2->vval.v_list);
	ប្រសិនបើ (type == EXPR_ISNOT)
	    val = !val;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (tv1->v_type != tv2->v_type
	    || (type != EXPR_EQUAL && type != EXPR_NEQUAL))
    {
	ប្រសិនបើ (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_list_with_list));
	បើមិនដូច្នេះ
	    emsg(_(e_invalid_operation_for_list));
	ត្រឡប់ FAIL;
    }
    បើមិនដូច្នេះ
    {
	val = list_equal(tv1->vval.v_list, tv2->vval.v_list, ic);
	ប្រសិនបើ (type == EXPR_NEQUAL)
	    val = !val;
    }
    *res = val;
    ត្រឡប់ OK;
}

/*
 * Compare "tv1" to "tv2" as tuples according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    ចំនួនគត់
typval_compare_tuple(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	ចំនួនគត់	    ic,
	ចំនួនគត់	    *res)
{
    ចំនួនគត់	    val = 0;

    ប្រសិនបើ (type == EXPR_IS || type == EXPR_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
			      && tv1->vval.v_tuple == tv2->vval.v_tuple);
	ប្រសិនបើ (type == EXPR_ISNOT)
	    val = !val;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (tv1->v_type != tv2->v_type
	    || (type != EXPR_EQUAL && type != EXPR_NEQUAL))
    {
	ប្រសិនបើ (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_tuple_with_tuple));
	បើមិនដូច្នេះ
	    emsg(_(e_invalid_operation_for_tuple));
	ត្រឡប់ FAIL;
    }
    បើមិនដូច្នេះ
    {
	val = tuple_equal(tv1->vval.v_tuple, tv2->vval.v_tuple, ic);
	ប្រសិនបើ (type == EXPR_NEQUAL)
	    val = !val;
    }
    *res = val;
    ត្រឡប់ OK;
}

/*
 * Compare v:null with another type.  Return TRUE if the value is NULL.
 */
    ចំនួនគត់
typval_compare_null(typval_T *tv1, typval_T *tv2)
{
    ប្រសិនបើ ((tv1->v_type == VAR_SPECIAL && tv1->vval.v_number == VVAL_NULL)
	    || (tv2->v_type == VAR_SPECIAL && tv2->vval.v_number == VVAL_NULL))
    {
	typval_T	*tv = tv1->v_type == VAR_SPECIAL ? tv2 : tv1;

	switch (tv->v_type)
	{
	    pak_sa VAR_BLOB: ត្រឡប់ tv->vval.v_blob == NULL;
#ifdef FEAT_JOB_CHANNEL
	    pak_sa VAR_CHANNEL: ត្រឡប់ tv->vval.v_channel == NULL;
#endif
	    // TODO: null_class handling
	    // case VAR_CLASS: return tv->vval.v_class == NULL;
	    pak_sa VAR_DICT: ត្រឡប់ tv->vval.v_dict == NULL;
	    pak_sa VAR_FUNC: ត្រឡប់ tv->vval.v_string == NULL;
#ifdef FEAT_JOB_CHANNEL
	    pak_sa VAR_JOB: ត្រឡប់ tv->vval.v_job == NULL;
#endif
	    pak_sa VAR_LIST: ត្រឡប់ tv->vval.v_list == NULL;
	    pak_sa VAR_TUPLE: ត្រឡប់ tv->vval.v_tuple == NULL;
	    pak_sa VAR_OBJECT: ត្រឡប់ tv->vval.v_object == NULL;
	    pak_sa VAR_PARTIAL: ត្រឡប់ tv->vval.v_partial == NULL;
	    pak_sa VAR_STRING: ត្រឡប់ tv->vval.v_string == NULL;

	    pak_sa VAR_NUMBER: ប្រសិនបើ (!in_vim9script())
				 ត្រឡប់ tv->vval.v_number == 0;
			     bha_nga;
	    pak_sa VAR_FLOAT: ប្រសិនបើ (!in_vim9script())
				 ត្រឡប់ tv->vval.v_float == 0.0;
			     bha_nga;
	    pak_sa VAR_TYPEALIAS: ត្រឡប់ tv->vval.v_typealias == NULL;
	    default: bha_nga;
	}
    }
    // although comparing null with number, float or bool is not very useful
    // we won't give an error
    ត្រឡប់ FALSE;
}

/*
 * Compare "tv1" to "tv2" as blobs according to "type".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    ចំនួនគត់
typval_compare_blob(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	ចំនួនគត់	    *res)
{
    ចំនួនគត់	    val = 0;

    ប្រសិនបើ (type == EXPR_IS || type == EXPR_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
			&& tv1->vval.v_blob == tv2->vval.v_blob);
	ប្រសិនបើ (type == EXPR_ISNOT)
	    val = !val;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (tv1->v_type != tv2->v_type
	    || (type != EXPR_EQUAL && type != EXPR_NEQUAL))
    {
	ប្រសិនបើ (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_blob_with_blob));
	បើមិនដូច្នេះ
	    emsg(_(e_invalid_operation_for_blob));
	ត្រឡប់ FAIL;
    }
    បើមិនដូច្នេះ
    {
	val = blob_equal(tv1->vval.v_blob, tv2->vval.v_blob);
	ប្រសិនបើ (type == EXPR_NEQUAL)
	    val = !val;
    }
    *res = val;
    ត្រឡប់ OK;
}

/*
 * Compare "tv1" to "tv2" as objects according to "type".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    ចំនួនគត់
typval_compare_object(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	ចំនួនគត់	    ic,
	ចំនួនគត់	    *res)
{
    ចំនួនគត់ res_match = type == EXPR_EQUAL || type == EXPR_IS ? TRUE : FALSE;

    ប្រសិនបើ (tv1->vval.v_object == NULL && tv2->vval.v_object == NULL)
    {
	*res = res_match;
	ត្រឡប់ OK;
    }
    ប្រសិនបើ (tv1->vval.v_object == NULL || tv2->vval.v_object == NULL)
    {
	*res = !res_match;
	ត្រឡប់ OK;
    }

    object_T *obj1 = tv1->vval.v_object;
    object_T *obj2 = tv2->vval.v_object;
    ប្រសិនបើ (type == EXPR_IS || type == EXPR_ISNOT)
    {
	*res = obj1 == obj2 ? res_match : !res_match;
	ត្រឡប់ OK;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (tv1->v_type != tv2->v_type)
    {
	emsg(_(e_can_only_compare_object_with_object));
	ត្រឡប់ FAIL;
    }

    *res = object_equal(obj1, obj2, ic) ? res_match : !res_match;
    ត្រឡប់ OK;
}

/*
 * Compare "tv1" to "tv2" as dictionaries according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    ចំនួនគត់
typval_compare_dict(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	ចំនួនគត់	    ic,
	ចំនួនគត់	    *res)
{
    ចំនួនគត់	    val;

    ប្រសិនបើ (type == EXPR_IS || type == EXPR_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
			&& tv1->vval.v_dict == tv2->vval.v_dict);
	ប្រសិនបើ (type == EXPR_ISNOT)
	    val = !val;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (tv1->v_type != tv2->v_type
		|| (type != EXPR_EQUAL && type != EXPR_NEQUAL))
    {
	ប្រសិនបើ (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_dictionary_with_dictionary));
	បើមិនដូច្នេះ
	    emsg(_(e_invalid_operation_for_dictionary));
	ត្រឡប់ FAIL;
    }
    បើមិនដូច្នេះ
    {
	val = dict_equal(tv1->vval.v_dict, tv2->vval.v_dict, ic);
	ប្រសិនបើ (type == EXPR_NEQUAL)
	    val = !val;
    }
    *res = val;
    ត្រឡប់ OK;
}

/*
 * Compare "tv1" to "tv2" as funcrefs according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    ចំនួនគត់
typval_compare_func(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	ចំនួនគត់	    ic,
	ចំនួនគត់	    *res)
{
    ចំនួនគត់	    val = 0;

    ប្រសិនបើ (type != EXPR_EQUAL && type != EXPR_NEQUAL
	    && type != EXPR_IS && type != EXPR_ISNOT)
    {
	emsg(_(e_invalid_operation_for_funcrefs));
	ត្រឡប់ FAIL;
    }
    ប្រសិនបើ ((tv1->v_type == VAR_PARTIAL && tv1->vval.v_partial == NULL)
	    || (tv2->v_type == VAR_PARTIAL && tv2->vval.v_partial == NULL))
	// When both partials are NULL, then they are equal.
	// Otherwise they are not equal.
	val = (tv1->vval.v_partial == tv2->vval.v_partial);
    បើមិនដូច្នេះ ប្រសិនបើ (type == EXPR_IS || type == EXPR_ISNOT)
    {
	ប្រសិនបើ (tv1->v_type == VAR_FUNC && tv2->v_type == VAR_FUNC)
	    // strings are considered the same if their value is
	    // the same
	    val = tv_equal(tv1, tv2, ic);
	បើមិនដូច្នេះ ប្រសិនបើ (tv1->v_type == VAR_PARTIAL && tv2->v_type == VAR_PARTIAL)
	    val = (tv1->vval.v_partial == tv2->vval.v_partial);
	បើមិនដូច្នេះ
	    val = FALSE;
    }
    បើមិនដូច្នេះ
	val = tv_equal(tv1, tv2, ic);
    ប្រសិនបើ (type == EXPR_NEQUAL || type == EXPR_ISNOT)
	val = !val;
    *res = val;
    ត្រឡប់ OK;
}

/*
 * Compare "tv1" to "tv2" as strings according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    ចំនួនគត់
typval_compare_string(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	ចំនួនគត់	    ic,
	ចំនួនគត់	    *res)
{
    ចំនួនគត់		i = 0;
    ចំនួនគត់		val = FALSE;
    char_u	*s1, *s2;
    char_u	buf1[NUMBUFLEN], buf2[NUMBUFLEN];

    ប្រសិនបើ (in_vim9script()
	  && ((tv1->v_type != VAR_STRING && tv1->v_type != VAR_SPECIAL)
	   || (tv2->v_type != VAR_STRING && tv2->v_type != VAR_SPECIAL)))
    {
	semsg(_(e_cannot_compare_str_with_str),
		   vartype_name(tv1->v_type), vartype_name(tv2->v_type));
	ត្រឡប់ FAIL;
    }
    s1 = tv_get_string_buf(tv1, buf1);
    s2 = tv_get_string_buf(tv2, buf2);
    ប្រសិនបើ (type != EXPR_MATCH && type != EXPR_NOMATCH)
	i = ic ? MB_STRICMP(s1, s2) : STRCMP(s1, s2);
    switch (type)
    {
	pak_sa EXPR_IS:	    ប្រសិនបើ (in_vim9script())
			    {
				// Really check it is the same string, not just
				// the same value.
				val = tv1->vval.v_string == tv2->vval.v_string;
				bha_nga;
			    }
			    // FALLTHROUGH
	pak_sa EXPR_EQUAL:    val = (i == 0); bha_nga;
	pak_sa EXPR_ISNOT:    ប្រសិនបើ (in_vim9script())
			    {
				// Really check it is not the same string, not
				// just a different value.
				val = tv1->vval.v_string != tv2->vval.v_string;
				bha_nga;
			    }
			    // FALLTHROUGH
	pak_sa EXPR_NEQUAL:   val = (i != 0); bha_nga;
	pak_sa EXPR_GREATER:  val = (i > 0); bha_nga;
	pak_sa EXPR_GEQUAL:   val = (i >= 0); bha_nga;
	pak_sa EXPR_SMALLER:  val = (i < 0); bha_nga;
	pak_sa EXPR_SEQUAL:   val = (i <= 0); bha_nga;

	pak_sa EXPR_MATCH:
	pak_sa EXPR_NOMATCH:
		val = pattern_match(s2, s1, ic);
		ប្រសិនបើ (type == EXPR_NOMATCH)
		    val = !val;
		bha_nga;

	default:  bha_nga;  // avoid gcc warning
    }
    *res = val;
    ត្រឡប់ OK;
}
/*
 * Convert any type to a string, never give an error.
 * When "quotes" is TRUE add quotes to a string.
 * Returns an allocated string.
 */
    char_u *
typval_tostring(typval_T *arg, ចំនួនគត់ quotes)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    char_u	*ret = NULL;

    ប្រសិនបើ (arg == NULL)
	ត្រឡប់ vim_strsave((char_u *)"(does not exist)");
    ប្រសិនបើ (!quotes && arg->v_type == VAR_STRING)
    {
	ret = vim_strsave(arg->vval.v_string == NULL ? (char_u *)""
							 : arg->vval.v_string);
    }
    បើមិនដូច្នេះ
    {
	ret = tv2string(arg, &tofree, numbuf, 0);
	// Make a copy if we have a value but it's not in allocated memory.
	ប្រសិនបើ (ret != NULL && tofree == NULL)
	    ret = vim_strsave(ret);
    }
    ត្រឡប់ ret;
}

/*
 * Return TRUE if typeval "tv" is locked: Either that value is locked itself
 * or it refers to a List or Dictionary that is locked.
 */
    ចំនួនគត់
tv_islocked(typval_T *tv)
{
    ត្រឡប់ (tv->v_lock & VAR_LOCKED)
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

    static ចំនួនគត់
func_equal(
    typval_T *tv1,
    typval_T *tv2,
    ចំនួនគត់	     ic)	    // ignore case
{
    char_u	*s1, *s2;
    dict_T	*d1, *d2;
    ចំនួនគត់		a1, a2;
    ចំនួនគត់		i;

    // empty and NULL function name considered the same
    s1 = tv1->v_type == VAR_FUNC ? tv1->vval.v_string
					   : partial_name(tv1->vval.v_partial);
    ប្រសិនបើ (s1 != NULL && *s1 == NUL)
	s1 = NULL;
    s2 = tv2->v_type == VAR_FUNC ? tv2->vval.v_string
					   : partial_name(tv2->vval.v_partial);
    ប្រសិនបើ (s2 != NULL && *s2 == NUL)
	s2 = NULL;
    ប្រសិនបើ (s1 == NULL || s2 == NULL)
    {
	ប្រសិនបើ (s1 != s2)
	    ត្រឡប់ FALSE;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (STRCMP(s1, s2) != 0)
	ត្រឡប់ FALSE;

    // empty dict and NULL dict is different
    d1 = tv1->v_type == VAR_FUNC ? NULL : tv1->vval.v_partial->pt_dict;
    d2 = tv2->v_type == VAR_FUNC ? NULL : tv2->vval.v_partial->pt_dict;
    ប្រសិនបើ (d1 == NULL || d2 == NULL)
    {
	ប្រសិនបើ (d1 != d2)
	    ត្រឡប់ FALSE;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (!dict_equal(d1, d2, ic))
	ត្រឡប់ FALSE;

    // empty list and no list considered the same
    a1 = tv1->v_type == VAR_FUNC ? 0 : tv1->vval.v_partial->pt_argc;
    a2 = tv2->v_type == VAR_FUNC ? 0 : tv2->vval.v_partial->pt_argc;
    ប្រសិនបើ (a1 != a2)
	ត្រឡប់ FALSE;
    សម្រាប់ (i = 0; i < a1; ++i)
	ប្រសិនបើ (!tv_equal(tv1->vval.v_partial->pt_argv + i,
		      tv2->vval.v_partial->pt_argv + i, ic))
	    ត្រឡប់ FALSE;

    ត្រឡប់ TRUE;
}

/*
 * Return TRUE if "tv1" and "tv2" have the same value.
 * Compares the items just like "==" would compare them, but strings and
 * numbers are different.  Floats and numbers are also different.
 */
    ចំនួនគត់
tv_equal(
    typval_T *tv1,
    typval_T *tv2,
    ចំនួនគត់	     ic)	    // ignore case
{
    char_u	buf1[NUMBUFLEN], buf2[NUMBUFLEN];
    char_u	*s1, *s2;
    static ចំនួនគត់  recursive_cnt = 0;	    // catch recursive loops
    ចំនួនគត់		r;
    static ចំនួនគត់	tv_equal_recurse_limit;

    // Catch lists and dicts that have an endless loop by limiting
    // recursiveness to a limit.  We guess they are equal then.
    // A fixed limit has the problem of still taking an awful long time.
    // Reduce the limit every time running into it. That should work fine for
    // deeply linked structures that are not recursively linked and catch
    // recursiveness quickly.
    ប្រសិនបើ (recursive_cnt == 0)
	tv_equal_recurse_limit = 1000;
    ប្រសិនបើ (recursive_cnt >= tv_equal_recurse_limit)
    {
	--tv_equal_recurse_limit;
	ត្រឡប់ TRUE;
    }

    // For VAR_FUNC and VAR_PARTIAL compare the function name, bound dict and
    // arguments.
    ប្រសិនបើ ((tv1->v_type == VAR_FUNC
		|| (tv1->v_type == VAR_PARTIAL && tv1->vval.v_partial != NULL))
	    && (tv2->v_type == VAR_FUNC
		|| (tv2->v_type == VAR_PARTIAL && tv2->vval.v_partial != NULL)))
    {
	++recursive_cnt;
	r = func_equal(tv1, tv2, ic);
	--recursive_cnt;
	ត្រឡប់ r;
    }

    ប្រសិនបើ (tv1->v_type != tv2->v_type
	    && ((tv1->v_type != VAR_BOOL && tv1->v_type != VAR_SPECIAL)
		|| (tv2->v_type != VAR_BOOL && tv2->v_type != VAR_SPECIAL)))
	ត្រឡប់ FALSE;

    switch (tv1->v_type)
    {
	pak_sa VAR_LIST:
	    ++recursive_cnt;
	    r = list_equal(tv1->vval.v_list, tv2->vval.v_list, ic);
	    --recursive_cnt;
	    ត្រឡប់ r;

	pak_sa VAR_TUPLE:
	    ++recursive_cnt;
	    r = tuple_equal(tv1->vval.v_tuple, tv2->vval.v_tuple, ic);
	    --recursive_cnt;
	    ត្រឡប់ r;

	pak_sa VAR_DICT:
	    ++recursive_cnt;
	    r = dict_equal(tv1->vval.v_dict, tv2->vval.v_dict, ic);
	    --recursive_cnt;
	    ត្រឡប់ r;

	pak_sa VAR_BLOB:
	    ត្រឡប់ blob_equal(tv1->vval.v_blob, tv2->vval.v_blob);

	pak_sa VAR_NUMBER:
	pak_sa VAR_BOOL:
	pak_sa VAR_SPECIAL:
	    ត្រឡប់ tv1->vval.v_number == tv2->vval.v_number;

	pak_sa VAR_STRING:
	    s1 = tv_get_string_buf(tv1, buf1);
	    s2 = tv_get_string_buf(tv2, buf2);
	    ត្រឡប់ ((ic ? MB_STRICMP(s1, s2) : STRCMP(s1, s2)) == 0);

	pak_sa VAR_FLOAT:
	    ត្រឡប់ tv1->vval.v_float == tv2->vval.v_float;
	pak_sa VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    ត្រឡប់ tv1->vval.v_job == tv2->vval.v_job;
#endif
	pak_sa VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    ត្រឡប់ tv1->vval.v_channel == tv2->vval.v_channel;
#endif
	pak_sa VAR_INSTR:
	    ត្រឡប់ tv1->vval.v_instr == tv2->vval.v_instr;

	pak_sa VAR_CLASS:
	    // A class only exists once, equality is identity.
	    ត្រឡប់ tv1->vval.v_class == tv2->vval.v_class;

	pak_sa VAR_OBJECT:
	    ++recursive_cnt;
	    r = object_equal(tv1->vval.v_object, tv2->vval.v_object, ic);
	    --recursive_cnt;
	    ត្រឡប់ r;

	pak_sa VAR_PARTIAL:
	    ត្រឡប់ tv1->vval.v_partial == tv2->vval.v_partial;

	pak_sa VAR_FUNC:
	    ត្រឡប់ tv1->vval.v_string == tv2->vval.v_string;

	pak_sa VAR_TYPEALIAS:
	    ត្រឡប់ tv1->vval.v_typealias == tv2->vval.v_typealias;

	pak_sa VAR_UNKNOWN:
	pak_sa VAR_ANY:
	pak_sa VAR_VOID:
	    bha_nga;
    }

    // VAR_UNKNOWN can be the result of a invalid expression, let's say it
    // does not equal anything, not even itself.
    ត្រឡប់ FALSE;
}

/*
 * Get an option value.
 * "arg" points to the '&' or '+' before the option name.
 * "arg" is advanced to character after the option name.
 * Return OK or FAIL.
 */
    ចំនួនគត់
eval_option(
    char_u	**arg,
    typval_T	*rettv,	// when NULL, only check if option exists
    ចំនួនគត់		evaluate)
{
    char_u	*option_end;
    long	numval;
    char_u	*stringval;
    getoption_T	opt_type;
    ចំនួនគត់		c;
    ចំនួនគត់		working = (**arg == '+');    // has("+option")
    ចំនួនគត់		ret = OK;
    ចំនួនគត់		scope;

    // Isolate the option name and find its value.
    option_end = find_option_end(arg, &scope);
    ប្រសិនបើ (option_end == NULL)
    {
	ប្រសិនបើ (rettv != NULL)
	    semsg(_(e_option_name_missing_str), *arg);
	ត្រឡប់ FAIL;
    }

    ប្រសិនបើ (!evaluate)
    {
	*arg = option_end;
	ត្រឡប់ OK;
    }

    c = *option_end;
    *option_end = NUL;
    opt_type = get_option_value(*arg, &numval,
			       rettv == NULL ? NULL : &stringval, NULL, scope);

    ប្រសិនបើ (opt_type == gov_unknown)
    {
	ប្រសិនបើ (rettv != NULL)
	    semsg(_(e_unknown_option_str), *arg);
	ret = FAIL;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (rettv != NULL)
    {
	rettv->v_lock = 0;
	ប្រសិនបើ (opt_type == gov_hidden_string)
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = NULL;
	}
	បើមិនដូច្នេះ ប្រសិនបើ (opt_type == gov_hidden_bool || opt_type == gov_hidden_number)
	{
	    rettv->v_type = in_vim9script() && opt_type == gov_hidden_bool
						       ? VAR_BOOL : VAR_NUMBER;
	    rettv->vval.v_number = 0;
	}
	បើមិនដូច្នេះ ប្រសិនបើ (opt_type == gov_bool || opt_type == gov_number)
	{
	    ប្រសិនបើ (in_vim9script() && opt_type == gov_bool)
	    {
		rettv->v_type = VAR_BOOL;
		rettv->vval.v_number = numval ? VVAL_TRUE : VVAL_FALSE;
	    }
	    បើមិនដូច្នេះ
	    {
		rettv->v_type = VAR_NUMBER;
		rettv->vval.v_number = numval;
	    }
	}
	បើមិនដូច្នេះ				// string option
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = stringval;
	}
    }
    បើមិនដូច្នេះ ប្រសិនបើ (working && (opt_type == gov_hidden_bool
			|| opt_type == gov_hidden_number
			|| opt_type == gov_hidden_string))
	ret = FAIL;

    *option_end = c;		    // put back for error messages
    *arg = option_end;

    ត្រឡប់ ret;
}

/*
 * Allocate a variable for a number constant.  Also deals with "0z" for blob.
 * Return OK or FAIL.
 */
    ចំនួនគត់
eval_number(
	char_u	    **arg,
	typval_T    *rettv,
	ចំនួនគត់	    evaluate,
	ចំនួនគត់	    want_string)
{
    ចំនួនគត់		len;
    ចំនួនគត់		skip_quotes = !in_old_script(4);
    char_u	*p;
    ចំនួនគត់		get_float = FALSE;

    // We accept a float when the format matches
    // "[0-9]\+\.[0-9]\+\([eE][+-]\?[0-9]\+\)\?".  This is very
    // strict to avoid backwards compatibility problems.
    // With script version 2 and later the leading digit can be
    // omitted.
    // Don't look for a float after the "." operator, so that
    // ":let vers = 1.2.3" doesn't fail.
    ប្រសិនបើ (**arg == '.')
	p = *arg;
    បើមិនដូច្នេះ
    {
	p = *arg + 1;
	ប្រសិនបើ (skip_quotes)
	    សម្រាប់ (;;)
	    {
		ប្រសិនបើ (*p == '\'')
		    ++p;
		ប្រសិនបើ (!vim_isdigit(*p))
		    bha_nga;
		p = skipdigits(p);
	    }
	បើមិនដូច្នេះ
	    p = skipdigits(p);
    }
    ប្រសិនបើ (!want_string && p[0] == '.' && vim_isdigit(p[1]))
    {
	get_float = TRUE;
	p = skipdigits(p + 2);
	ប្រសិនបើ (*p == 'e' || *p == 'E')
	{
	    ++p;
	    ប្រសិនបើ (*p == '-' || *p == '+')
		++p;
	    ប្រសិនបើ (!vim_isdigit(*p))
		get_float = FALSE;
	    បើមិនដូច្នេះ
		p = skipdigits(p + 1);
	}
	ប្រសិនបើ (ASCII_ISALPHA(*p) || *p == '.')
	    get_float = FALSE;
    }
    ប្រសិនបើ (get_float)
    {
	float_T	f;

	*arg += string2float(*arg, &f, skip_quotes);
	ប្រសិនបើ (evaluate)
	{
	    rettv->v_type = VAR_FLOAT;
	    rettv->vval.v_float = f;
	}
    }
    បើមិនដូច្នេះ
    ប្រសិនបើ (**arg == '0' && ((*arg)[1] == 'z' || (*arg)[1] == 'Z'))
    {
	char_u  *bp;
	blob_T  *blob = NULL;  // init for gcc

	// Blob constant: 0z0123456789abcdef
	ប្រសិនបើ (evaluate)
	    blob = blob_alloc();
	សម្រាប់ (bp = *arg + 2; vim_isxdigit(bp[0]); bp += 2)
	{
	    ប្រសិនបើ (!vim_isxdigit(bp[1]))
	    {
		ប្រសិនបើ (blob != NULL)
		{
		    emsg(_(e_blob_literal_should_have_an_even_number_of_hex_characters));
		    ga_clear(&blob->bv_ga);
		    VIM_CLEAR(blob);
		}
		ត្រឡប់ FAIL;
	    }
	    ប្រសិនបើ (blob != NULL)
		ga_append(&blob->bv_ga,
			     (hex2nr(*bp) << 4) + hex2nr(*(bp+1)));
	    ប្រសិនបើ (bp[2] == '.' && vim_isxdigit(bp[3]))
		++bp;
	}
	ប្រសិនបើ (blob != NULL)
	    rettv_blob_set(rettv, blob);
	*arg = bp;
    }
    បើមិនដូច្នេះ
    {
	varnumber_T	n;

	// decimal, hex or octal number
	vim_str2nr(*arg, NULL, &len, skip_quotes
		      ? STR2NR_NO_OCT + STR2NR_QUOTE
		      : STR2NR_ALL, &n, NULL, 0, TRUE, NULL);
	ប្រសិនបើ (len == 0)
	{
	    ប្រសិនបើ (evaluate)
		semsg(_(e_invalid_expression_str), *arg);
	    ត្រឡប់ FAIL;
	}
	*arg += len;
	ប្រសិនបើ (evaluate)
	{
	    rettv->v_type = VAR_NUMBER;
	    rettv->vval.v_number = n;
	}
    }
    ត្រឡប់ OK;
}

/*
 * Evaluate a string constant and put the result in "rettv".
 * "*arg" points to the double quote or to after it when "interpolate" is TRUE.
 * When "interpolate" is TRUE reduce "{{" to "{", reduce "}}" to "}" and stop
 * at a single "{".
 * Return OK or FAIL.
 */
    ចំនួនគត់
eval_string(char_u **arg, typval_T *rettv, ចំនួនគត់ evaluate, ចំនួនគត់ interpolate)
{
    char_u	*p;
    char_u	*end;
    ចំនួនគត់		extra = interpolate ? 1 : 0;
    ចំនួនគត់		off = interpolate ? 0 : 1;
    ចំនួនគត់		len;

    // Find the end of the string, skipping backslashed characters.
    សម្រាប់ (p = *arg + off; *p != NUL && *p != '"'; MB_PTR_ADV(p))
    {
	ប្រសិនបើ (*p == '\\' && p[1] != NUL)
	{
	    ++p;
	    // A "\<x>" form occupies at least 4 characters, and produces up
	    // to 9 characters (6 for the char and 3 for a modifier):
	    // reserve space for 5 extra.
	    ប្រសិនបើ (*p == '<')
	    {
		ចំនួនគត់		modifiers = 0;
		ចំនួនគត់		flags = FSK_KEYCODE | FSK_IN_STRING;

		extra += 5;

		// Skip to the '>' to avoid using '{' inside for string
		// interpolation.
		ប្រសិនបើ (p[1] != '*')
		    flags |= FSK_SIMPLIFY;
		ប្រសិនបើ (find_special_key(&p, &modifiers, flags, NULL) != 0)
		    --p;  // leave "p" on the ">"
	    }
	}
	បើមិនដូច្នេះ ប្រសិនបើ (interpolate && (*p == '{' || *p == '}'))
	{
	    ប្រសិនបើ (*p == '{' && p[1] != '{') // start of expression
		bha_nga;
	    ++p;
	    ប្រសិនបើ (p[-1] == '}' && *p != '}') // single '}' is an error
	    {
		semsg(_(e_stray_closing_curly_str), *arg);
		ត្រឡប់ FAIL;
	    }
	    --extra;  // "{{" becomes "{", "}}" becomes "}"
	}
    }

    ប្រសិនបើ (*p != '"' && !(interpolate && *p == '{'))
    {
	semsg(_(e_missing_double_quote_str), *arg);
	ត្រឡប់ FAIL;
    }

    // If only parsing, set *arg and return here
    ប្រសិនបើ (!evaluate)
    {
	*arg = p + off;
	ត្រឡប់ OK;
    }

    // Copy the string into allocated memory, handling backslashed
    // characters.
    rettv->v_type = VAR_STRING;
    len = (ចំនួនគត់)(p - *arg + extra);
    rettv->vval.v_string = alloc(len);
    ប្រសិនបើ (rettv->vval.v_string == NULL)
	ត្រឡប់ FAIL;
    end = rettv->vval.v_string;

    សម្រាប់ (p = *arg + off; *p != NUL && *p != '"'; )
    {
	ប្រសិនបើ (*p == '\\')
	{
	    switch (*++p)
	    {
		pak_sa 'b': *end++ = BS; ++p; bha_nga;
		pak_sa 'e': *end++ = ESC; ++p; bha_nga;
		pak_sa 'f': *end++ = FF; ++p; bha_nga;
		pak_sa 'n': *end++ = NL; ++p; bha_nga;
		pak_sa 'r': *end++ = CAR; ++p; bha_nga;
		pak_sa 't': *end++ = TAB; ++p; bha_nga;

		pak_sa 'X': // hex: "\x1", "\x12"
		pak_sa 'x':
		pak_sa 'u': // Unicode: "\u0023"
		pak_sa 'U':
			  ប្រសិនបើ (vim_isxdigit(p[1]))
			  {
			      ចំនួនគត់	n, nr;
			      ចំនួនគត់	c = SAFE_toupper(*p);

			      ប្រសិនបើ (c == 'X')
				  n = 2;
			      បើមិនដូច្នេះ ប្រសិនបើ (*p == 'u')
				  n = 4;
			      បើមិនដូច្នេះ
				  n = 8;
			      nr = 0;
			      ខណៈពេល (--n >= 0 && vim_isxdigit(p[1]))
			      {
				  ++p;
				  nr = (nr << 4) + hex2nr(*p);
			      }
			      ++p;
			      // For "\u" store the number according to
			      // 'encoding'.
			      ប្រសិនបើ (c != 'X')
				  end += (*mb_char2bytes)(nr, end);
			      បើមិនដូច្នេះ
				  *end++ = nr;
			  }
			  bha_nga;

			  // octal: "\1", "\12", "\123"
		pak_sa '0':
		pak_sa '1':
		pak_sa '2':
		pak_sa '3':
		pak_sa '4':
		pak_sa '5':
		pak_sa '6':
		pak_sa '7': *end = *p++ - '0';
			  ប្រសិនបើ (*p >= '0' && *p <= '7')
			  {
			      *end = (*end << 3) + *p++ - '0';
			      ប្រសិនបើ (*p >= '0' && *p <= '7')
				  *end = (*end << 3) + *p++ - '0';
			  }
			  ++end;
			  bha_nga;

			  // Special key, e.g.: "\<C-W>"
		pak_sa '<':
			  {
			      ចំនួនគត់ flags = FSK_KEYCODE | FSK_IN_STRING;

			      ប្រសិនបើ (p[1] != '*')
				  flags |= FSK_SIMPLIFY;
			      extra = trans_special(&p, end, flags, FALSE, NULL);
			      ប្រសិនបើ (extra != 0)
			      {
				  end += extra;
				  ប្រសិនបើ (end >= rettv->vval.v_string + len)
				      iemsg("eval_string() used more space than allocated");
				  bha_nga;
			      }
			  }
			  // FALLTHROUGH

		default: MB_COPY_CHAR(p, end);
			  bha_nga;
	    }
	}
	បើមិនដូច្នេះ
	{
	    ប្រសិនបើ (interpolate && (*p == '{' || *p == '}'))
	    {
		ប្រសិនបើ (*p == '{' && p[1] != '{') // start of expression
		    bha_nga;
		++p;  // reduce "{{" to "{" and "}}" to "}"
	    }
	    MB_COPY_CHAR(p, end);
	}
    }
    *end = NUL;
    ប្រសិនបើ (*p == '"' && !interpolate)
	++p;
    *arg = p;

    ត្រឡប់ OK;
}

/*
 * Allocate a variable for a 'str''ing' constant.
 * When "interpolate" is TRUE reduce "{{" to "{" and stop at a single "{".
 * Return OK when a "rettv" was set to the string.
 * Return FAIL on error, "rettv" is not set.
 */
    ចំនួនគត់
eval_lit_string(char_u **arg, typval_T *rettv, ចំនួនគត់ evaluate, ចំនួនគត់ interpolate)
{
    char_u	*p;
    char_u	*str;
    ចំនួនគត់		reduce = interpolate ? -1 : 0;
    ចំនួនគត់		off = interpolate ? 0 : 1;

    // Find the end of the string, skipping ''.
    សម្រាប់ (p = *arg + off; *p != NUL; MB_PTR_ADV(p))
    {
	ប្រសិនបើ (*p == '\'')
	{
	    ប្រសិនបើ (p[1] != '\'')
		bha_nga;
	    ++reduce;
	    ++p;
	}
	បើមិនដូច្នេះ ប្រសិនបើ (interpolate)
	{
	    ប្រសិនបើ (*p == '{')
	    {
		ប្រសិនបើ (p[1] != '{')
		    bha_nga;
		++p;
		++reduce;
	    }
	    បើមិនដូច្នេះ ប្រសិនបើ (*p == '}')
	    {
		++p;
		ប្រសិនបើ (*p != '}')
		{
		    semsg(_(e_stray_closing_curly_str), *arg);
		    ត្រឡប់ FAIL;
		}
		++reduce;
	    }
	}
    }

    ប្រសិនបើ (*p != '\'' && !(interpolate && *p == '{'))
    {
	semsg(_(e_missing_single_quote_str), *arg);
	ត្រឡប់ FAIL;
    }

    // If only parsing return after setting "*arg"
    ប្រសិនបើ (!evaluate)
    {
	*arg = p + off;
	ត្រឡប់ OK;
    }

    // Copy the string into allocated memory, handling '' to ' reduction and
    // any expressions.
    str = alloc((p - *arg) - reduce);
    ប្រសិនបើ (str == NULL)
	ត្រឡប់ FAIL;
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = str;

    សម្រាប់ (p = *arg + off; *p != NUL; )
    {
	ប្រសិនបើ (*p == '\'')
	{
	    ប្រសិនបើ (p[1] != '\'')
		bha_nga;
	    ++p;
	}
	បើមិនដូច្នេះ ប្រសិនបើ (interpolate && (*p == '{' || *p == '}'))
	{
	    ប្រសិនបើ (*p == '{' && p[1] != '{')
		bha_nga;
	    ++p;
	}
	MB_COPY_CHAR(p, str);
    }
    *str = NUL;
    *arg = p + off;

    ត្រឡប់ OK;
}

/*
 * Evaluate a single or double quoted string possibly containing expressions.
 * "arg" points to the '$'.  The result is put in "rettv".
 * Returns OK or FAIL.
 */
    ចំនួនគត់
eval_interp_string(char_u **arg, typval_T *rettv, ចំនួនគត់ evaluate)
{
    typval_T	tv;
    ចំនួនគត់		ret = OK;
    ចំនួនគត់		quote;
    garray_T	ga;
    char_u	*p;

    ga_init2(&ga, 1, 80);

    // *arg is on the '$' character, move it to the first string character.
    ++*arg;
    quote = **arg;
    ++*arg;

    សម្រាប់ (;;)
    {
	// Get the string up to the matching quote or to a single '{'.
	// "arg" is advanced to either the quote or the '{'.
	ប្រសិនបើ (quote == '"')
	    ret = eval_string(arg, &tv, evaluate, TRUE);
	បើមិនដូច្នេះ
	    ret = eval_lit_string(arg, &tv, evaluate, TRUE);
	ប្រសិនបើ (ret == FAIL)
	    bha_nga;
	ប្រសិនបើ (evaluate)
	{
	    ga_concat(&ga, tv.vval.v_string);
	    clear_tv(&tv);
	}

	ប្រសិនបើ (**arg != '{')
	{
	    // found terminating quote
	    ++*arg;
	    bha_nga;
	}
	p = eval_one_expr_in_str(*arg, &ga, evaluate);
	ប្រសិនបើ (p == NULL)
	{
	    ret = FAIL;
	    bha_nga;
	}
	*arg = p;
    }

    rettv->v_type = VAR_STRING;
    ប្រសិនបើ (ret == FAIL || !evaluate || ga_append(&ga, NUL) == FAIL)
    {
	ga_clear(&ga);
	rettv->vval.v_string = NULL;
	ត្រឡប់ ret;
    }

    rettv->vval.v_string = ga.ga_data;
    ត្រឡប់ OK;
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
    ចំនួនគត់		copyID)
{
    ត្រឡប់ echo_string_core(tv, tofree, numbuf, copyID, FALSE, TRUE, FALSE);
}

/*
 * Get the value of an environment variable.
 * "arg" is pointing to the '$'.  It is advanced to after the name.
 * If the environment variable was not set, silently assume it is empty.
 * Return FAIL if the name is invalid.
 */
    ចំនួនគត់
eval_env_var(char_u **arg, typval_T *rettv, ចំនួនគត់ evaluate)
{
    char_u	*string = NULL;
    ចំនួនគត់		len;
    ចំនួនគត់		cc;
    char_u	*name;
    ចំនួនគត់		mustfree = FALSE;

    ++*arg;
    name = *arg;
    len = get_env_len(arg);
    ប្រសិនបើ (evaluate)
    {
	ប្រសិនបើ (len == 0)
	    ត្រឡប់ FAIL; // invalid empty name

	cc = name[len];
	name[len] = NUL;
	// first try vim_getenv(), fast for normal environment vars
	string = vim_getenv(name, &mustfree);
	ប្រសិនបើ (string != NULL && *string != NUL)
	{
	    ប្រសិនបើ (!mustfree)
		string = vim_strsave(string);
	}
	បើមិនដូច្នេះ
	{
	    ប្រសិនបើ (mustfree)
		vim_free(string);

	    // next try expanding things like $VIM and ${HOME}
	    string = expand_env_save(name - 1);
	    ប្រសិនបើ (string != NULL && *string == '$')
		VIM_CLEAR(string);
	}
	name[len] = cc;

	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = string;
	rettv->v_lock = 0;
    }

    ត្រឡប់ OK;
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
    ចំនួនគត់		did_emsg_before = did_emsg;

    ប្រសិនបើ (argvars[0].v_type != VAR_STRING || !in_vim9script())
	lnum = (linenr_T)tv_get_number_chk(&argvars[0], NULL);
    ប្រសិនបើ (lnum <= 0 && did_emsg_before == did_emsg
					    && argvars[0].v_type != VAR_NUMBER)
    {
	ចំនួនគត់	fnum;
	pos_T	*fp;

	// no valid number, try using arg like line()
	fp = var2fpos(&argvars[0], TRUE, &fnum, FALSE);
	ប្រសិនបើ (fp != NULL)
	    lnum = fp->lnum;
    }
    ត្រឡប់ lnum;
}

/*
 * Get the lnum from the first argument.
 * Also accepts "$", then "buf" is used.
 * Returns 0 on error.
 */
    linenr_T
tv_get_lnum_buf(typval_T *argvars, buf_T *buf)
{
    ប្រសិនបើ (argvars[0].v_type == VAR_STRING
	    && argvars[0].vval.v_string != NULL
	    && argvars[0].vval.v_string[0] == '$'
	    && argvars[0].vval.v_string[1] == NUL
	    && buf != NULL)
	ត្រឡប់ buf->b_ml.ml_line_count;
    ត្រឡប់ (linenr_T)tv_get_number_chk(&argvars[0], NULL);
}

/*
 * Get buffer by number or pattern.
 */
    buf_T *
tv_get_buf(typval_T *tv, ចំនួនគត់ curtab_only)
{
    char_u	*name = tv->vval.v_string;
    buf_T	*buf;

    ប្រសិនបើ (tv->v_type == VAR_NUMBER)
	ត្រឡប់ buflist_findnr((ចំនួនគត់)tv->vval.v_number);
    ប្រសិនបើ (tv->v_type != VAR_STRING)
	ត្រឡប់ NULL;
    ប្រសិនបើ (name == NULL || *name == NUL)
	ត្រឡប់ curbuf;
    ប្រសិនបើ (name[0] == '$' && name[1] == NUL)
	ត្រឡប់ lastbuf;

    buf = buflist_find_by_name(name, curtab_only);

    // If not found, try expanding the name, like done for bufexists().
    ប្រសិនបើ (buf == NULL)
	buf = find_buffer(tv);

    ត្រឡប់ buf;
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
    ប្រសិនបើ (buf == NULL
	    && tv->v_type != VAR_NUMBER
	    && tv->v_type != VAR_STRING)
	// issue errmsg for type error
	(void)tv_get_number(tv);
    ត្រឡប់ buf;
}

/*
 * Check that repeating "slen" items "n" times, does not
 * overflow the int used for the resulting length.
 */
    ចំនួនគត់
check_repeat_count(varnumber_T slen, varnumber_T n)
{
    // Use a division so that the multiplication cannot overflow.
    ប្រសិនបើ (n <= (varnumber_T)INT_MAX / slen)
	ត្រឡប់ OK;

    var_na	buf[NUMBUFLEN];

    vim_snprintf(buf, parimaa_na(buf), "%lld", (long long)n);
    semsg(_(e_val_too_large), buf);
    ត្រឡប់ FAIL;
}

#endif // FEAT_EVAL
