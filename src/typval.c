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

#ຖ້າ defined(FEAT_EVAL)

/*
 * Allocate memory for a variable type-value, and make it empty (0 or NULL
 * value).
 */
    typval_T *
alloc_tv(void)
{
    ສົ່ງຄືນ ALLOC_CLEAR_ONE(typval_T);
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
    ຖ້າ (rettv != NULL)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = s;
    }
    ບໍ່ດັ່ງນັ້ນ
	vim_free(s);
    ສົ່ງຄືນ rettv;
}

/*
 * Free the memory for a variable type-value.
 */
    void
free_tv(typval_T *varp)
{
    ຖ້າ (varp == NULL)
	ສົ່ງຄືນ;

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
    ຖ້າ (varp == NULL)
	ສົ່ງຄືນ;

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
    ຖ້າ (varp != NULL)
	CLEAR_POINTER(varp);
}

    static varnumber_T
tv_get_bool_or_number_chk(
	typval_T    *varp,
	ຈຳນວນເຕັມ	    *denote,
	ຈຳນວນເຕັມ	    want_bool,
	ຈຳນວນເຕັມ	    vim9_string_error) // in Vim9 using a string is an error
{
    varnumber_T	n = 0L;

    switch (varp->v_type)
    {
	pak_sa VAR_NUMBER:
	    ຖ້າ (in_vim9script() && want_bool && varp->vval.v_number != 0
						   && varp->vval.v_number != 1)
	    {
		semsg(_(e_using_number_as_bool_nr), varp->vval.v_number);
		bha_nga;
	    }
	    ສົ່ງຄືນ varp->vval.v_number;
	pak_sa VAR_FLOAT:
	    emsg(_(e_using_float_as_number));
	    bha_nga;
	pak_sa VAR_FUNC:
	pak_sa VAR_PARTIAL:
	    emsg(_(e_using_funcref_as_number));
	    bha_nga;
	pak_sa VAR_STRING:
	    ຖ້າ (vim9_string_error && in_vim9script())
	    {
		emsg_using_string_as(varp, !want_bool);
		bha_nga;
	    }
	    ຖ້າ (varp->vval.v_string != NULL)
		vim_str2nr(varp->vval.v_string, NULL, NULL,
					 STR2NR_ALL, &n, NULL, 0, FALSE, NULL);
	    ສົ່ງຄືນ n;
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
	    ຖ້າ (!want_bool && in_vim9script())
	    {
		ຖ້າ (varp->v_type == VAR_BOOL)
		    emsg(_(e_using_bool_as_number));
		ບໍ່ດັ່ງນັ້ນ
		    emsg(_(e_using_special_as_number));
		bha_nga;
	    }
	    ສົ່ງຄືນ varp->vval.v_number == VVAL_TRUE ? 1 : 0;
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
		ຖ້າ (varp->vval.v_object == NULL)
		    emsg(_(e_using_object_as_string));
		ບໍ່ດັ່ງນັ້ນ
		{
		    class_T *cl = varp->vval.v_object->obj_class;
		    ຖ້າ (cl != NULL && IS_ENUM(cl))
			semsg(_(e_using_enum_str_as_number), cl->class_name.string);
		    ບໍ່ດັ່ງນັ້ນ
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
    ຖ້າ (denote == NULL)		// useful for values that must be unsigned
	n = -1;
    ບໍ່ດັ່ງນັ້ນ
	*denote = TRUE;
    ສົ່ງຄືນ n;
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
    ຈຳນວນເຕັມ		error = FALSE;

    ສົ່ງຄືນ tv_get_number_chk(varp, &error);	// return 0L on error
}

/*
 * Like tv_get_number() but in Vim9 script do convert a number in a string to a
 * number without giving an error.
 */
    varnumber_T
tv_to_number(typval_T *varp)
{
    ຈຳນວນເຕັມ		error = FALSE;

    ສົ່ງຄືນ tv_get_bool_or_number_chk(varp, &error, FALSE, FALSE);
}

    varnumber_T
tv_get_number_chk(typval_T *varp, ຈຳນວນເຕັມ *denote)
{
    ສົ່ງຄືນ tv_get_bool_or_number_chk(varp, denote, FALSE, TRUE);
}

/*
 * Get the boolean value of "varp".  This is like tv_get_number_chk(),
 * but in Vim9 script accepts Number (0 and 1) and Bool/Special.
 */
    varnumber_T
tv_get_bool(typval_T *varp)
{
    ສົ່ງຄືນ tv_get_bool_or_number_chk(varp, NULL, TRUE, TRUE);
}

/*
 * Get the boolean value of "varp".  This is like tv_get_number_chk(),
 * but in Vim9 script accepts Number and Bool.
 */
    varnumber_T
tv_get_bool_chk(typval_T *varp, ຈຳນວນເຕັມ *denote)
{
    ສົ່ງຄືນ tv_get_bool_or_number_chk(varp, denote, TRUE, TRUE);
}

    static float_T
tv_get_float_chk(typval_T *varp, ຈຳນວນເຕັມ *error)
{
    switch (varp->v_type)
    {
	pak_sa VAR_NUMBER:
	    ສົ່ງຄືນ (float_T)(varp->vval.v_number);
	pak_sa VAR_FLOAT:
	    ສົ່ງຄືນ varp->vval.v_float;
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
    ຖ້າ (error != NULL)
	*error = TRUE;
    ສົ່ງຄືນ 0;
}

    float_T
tv_get_float(typval_T *varp)
{
    ສົ່ງຄືນ tv_get_float_chk(varp, NULL);
}

/*
 * Give an error and return FAIL unless "args[idx]" is unknown
 */
    ຈຳນວນເຕັມ
check_for_unknown_arg(typval_T *args, ຈຳນວນເຕັມ idx)
{
    ຖ້າ (args[idx].v_type != VAR_UNKNOWN)
    {
	semsg(_(e_too_many_arguments), idx + 1);
	ສົ່ງຄືນ FAIL;
    }
    ສົ່ງຄືນ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string.
 */
    ຈຳນວນເຕັມ
check_for_string_arg(typval_T *args, ຈຳນວນເຕັມ idx)
{
    ຖ້າ (args[idx].v_type != VAR_STRING)
    {
	semsg(_(e_string_required_for_argument_nr), idx + 1);
	ສົ່ງຄືນ FAIL;
    }
    ສົ່ງຄືນ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a non-empty string.
 */
    ຈຳນວນເຕັມ
check_for_nonempty_string_arg(typval_T *args, ຈຳນວນເຕັມ idx)
{
    ຖ້າ (check_for_string_arg(args, idx) == FAIL)
	ສົ່ງຄືນ FAIL;
    ຖ້າ (args[idx].vval.v_string == NULL || *args[idx].vval.v_string == NUL)
    {
	semsg(_(e_non_empty_string_required_for_argument_nr), idx + 1);
	ສົ່ງຄືນ FAIL;
    }
    ສົ່ງຄືນ OK;
}

/*
 * Check for an optional string argument at 'idx'
 */
    ຈຳນວນເຕັມ
check_for_opt_string_arg(typval_T *args, ຈຳນວນເຕັມ idx)
{
    ສົ່ງຄືນ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_string_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a number.
 */
    ຈຳນວນເຕັມ
check_for_number_arg(typval_T *args, ຈຳນວນເຕັມ idx)
{
    ຖ້າ (args[idx].v_type != VAR_NUMBER)
    {
	semsg(_(e_number_required_for_argument_nr), idx + 1);
	ສົ່ງຄືນ FAIL;
    }
    ສົ່ງຄືນ OK;
}

/*
 * Check for an optional number argument at 'idx'
 */
    ຈຳນວນເຕັມ
check_for_opt_number_arg(typval_T *args, ຈຳນວນເຕັມ idx)
{
    ສົ່ງຄືນ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_number_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a float or a number.
 */
    ຈຳນວນເຕັມ
check_for_float_or_nr_arg(typval_T *args, ຈຳນວນເຕັມ idx)
{
    ຖ້າ (args[idx].v_type != VAR_FLOAT && args[idx].v_type != VAR_NUMBER)
    {
	semsg(_(e_float_or_number_required_for_argument_nr), idx + 1);
	ສົ່ງຄືນ FAIL;
    }
    ສົ່ງຄືນ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a bool.
 */
    ຈຳນວນເຕັມ
check_for_bool_arg(typval_T *args, ຈຳນວນເຕັມ idx)
{
    ຖ້າ (args[idx].v_type != VAR_BOOL
	    && !(args[idx].v_type == VAR_NUMBER
		&& (args[idx].vval.v_number == 0
		    || args[idx].vval.v_number == 1)))
    {
	semsg(_(e_bool_required_for_argument_nr), idx + 1);
	ສົ່ງຄືນ FAIL;
    }
    ສົ່ງຄືນ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a bool or a number.
 */
    static ຈຳນວນເຕັມ
check_for_bool_or_number_arg(typval_T *args, ຈຳນວນເຕັມ idx)
{
    ຖ້າ (args[idx].v_type != VAR_BOOL && args[idx].v_type != VAR_NUMBER)
    {
	semsg(_(e_bool_or_number_required_for_argument_nr), idx + 1);
	ສົ່ງຄືນ FAIL;
    }
    ສົ່ງຄືນ OK;
}

/*
 * Check for an optional bool argument at 'idx'.
 * Return FAIL if the type is wrong.
 */
    ຈຳນວນເຕັມ
check_for_opt_bool_arg(typval_T *args, ຈຳນວນເຕັມ idx)
{
    ຖ້າ (args[idx].v_type == VAR_UNKNOWN)
	ສົ່ງຄືນ OK;
    ສົ່ງຄືນ check_for_bool_arg(args, idx);
}

/*
 * Check for an optional bool or number argument at 'idx'.
 * Return FAIL if the type is wrong.
 */
    ຈຳນວນເຕັມ
check_for_opt_bool_or_number_arg(typval_T *args, ຈຳນວນເຕັມ idx)
{
    ຖ້າ (args[idx].v_type == VAR_UNKNOWN)
	ສົ່ງຄືນ OK;
    ສົ່ງຄືນ check_for_bool_or_number_arg(args, idx);
}

/*
 * Check for an optional bool or dict argument at 'idx'.
 * Return FAIL if the type is wrong.
 */
    ຈຳນວນເຕັມ
check_for_opt_bool_or_dict_arg(typval_T *args, ຈຳນວນເຕັມ idx)
{
    ຖ້າ (args[idx].v_type == VAR_UNKNOWN || args[idx].v_type == VAR_DICT)
	ສົ່ງຄືນ OK;
    ສົ່ງຄືນ check_for_bool_arg(args, idx);
}

/*
 * Give an error and return FAIL unless "args[idx]" is a blob.
 */
    ຈຳນວນເຕັມ
check_for_blob_arg(typval_T *args, ຈຳນວນເຕັມ idx)
{
    ຖ້າ (args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_blob_required_for_argument_nr), idx + 1);
	ສົ່ງຄືນ FAIL;
    }
    ສົ່ງຄືນ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list.
 */
    ຈຳນວນເຕັມ
check_for_list_arg(typval_T *args, ຈຳນວນເຕັມ idx)
{
    ຖ້າ (args[idx].v_type != VAR_LIST)
    {
	semsg(_(e_list_required_for_argument_nr), idx + 1);
	ສົ່ງຄືນ FAIL;
    }
    ສົ່ງຄືນ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a non-NULL list.
 */
    ຈຳນວນເຕັມ
check_for_nonnull_list_arg(typval_T *args, ຈຳນວນເຕັມ idx)
{
    ຖ້າ (check_for_list_arg(args, idx) == FAIL)
	ສົ່ງຄືນ FAIL;

    ຖ້າ (args[idx].vval.v_list == NULL)
    {
	semsg(_(e_non_null_list_required_for_argument_nr), idx + 1);
	ສົ່ງຄືນ FAIL;
    }
    ສົ່ງຄືນ OK;
}

/*
 * Check for an optional list argument at 'idx'
 */
    ຈຳນວນເຕັມ
check_for_opt_list_arg(typval_T *args, ຈຳນວນເຕັມ idx)
{
    ສົ່ງຄືນ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_list_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a tuple.
 */
    ຈຳນວນເຕັມ
check_for_tuple_arg(typval_T *args, ຈຳນວນເຕັມ idx)
{
    ຖ້າ (args[idx].v_type != VAR_TUPLE)
    {
	semsg(_(e_tuple_required_for_argument_nr), idx + 1);
	ສົ່ງຄືນ FAIL;
    }
    ສົ່ງຄືນ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a dict.
 */
    ຈຳນວນເຕັມ
check_for_dict_arg(typval_T *args, ຈຳນວນເຕັມ idx)
{
    ຖ້າ (args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_dict_required_for_argument_nr), idx + 1);
	ສົ່ງຄືນ FAIL;
    }
    ສົ່ງຄືນ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a non-NULL dict.
 */
    ຈຳນວນເຕັມ
check_for_nonnull_dict_arg(typval_T *args, ຈຳນວນເຕັມ idx)
{
    ຖ້າ (check_for_dict_arg(args, idx) == FAIL)
	ສົ່ງຄືນ FAIL;

    ຖ້າ (args[idx].vval.v_dict == NULL)
    {
	semsg(_(e_non_null_dict_required_for_argument_nr), idx + 1);
	ສົ່ງຄືນ FAIL;
    }
    ສົ່ງຄືນ OK;
}

/*
 * Check for an optional dict argument at 'idx'
 */
    ຈຳນວນເຕັມ
check_for_opt_dict_arg(typval_T *args, ຈຳນວນເຕັມ idx)
{
    ສົ່ງຄືນ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_dict_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Check for an optional non-NULL dict argument at 'idx'
 */
    ຈຳນວນເຕັມ
check_for_opt_nonnull_dict_arg(typval_T *args, ຈຳນວນເຕັມ idx)
{
    ສົ່ງຄືນ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_nonnull_dict_arg(args, idx) != FAIL) ? OK : FAIL;
}

#ຖ້າ defined(FEAT_JOB_CHANNEL)
/*
 * Give an error and return FAIL unless "args[idx]" is a channel or a job.
 */
    ຈຳນວນເຕັມ
check_for_chan_or_job_arg(typval_T *args, ຈຳນວນເຕັມ idx)
{
    ຖ້າ (args[idx].v_type != VAR_CHANNEL && args[idx].v_type != VAR_JOB)
    {
	semsg(_(e_chan_or_job_required_for_argument_nr), idx + 1);
	ສົ່ງຄືນ FAIL;
    }
    ສົ່ງຄືນ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an optional channel or a
 * job.
 */
    ຈຳນວນເຕັມ
check_for_opt_chan_or_job_arg(typval_T *args, ຈຳນວນເຕັມ idx)
{
    ສົ່ງຄືນ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_chan_or_job_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a job.
 */
    ຈຳນວນເຕັມ
check_for_job_arg(typval_T *args, ຈຳນວນເຕັມ idx)
{
    ຖ້າ (args[idx].v_type != VAR_JOB)
    {
	semsg(_(e_job_required_for_argument_nr), idx + 1);
	ສົ່ງຄືນ FAIL;
    }
    ສົ່ງຄືນ OK;
}

/*
 * Check for an optional job argument at 'idx'.
 */
    ຈຳນວນເຕັມ
check_for_opt_job_arg(typval_T *args, ຈຳນວນເຕັມ idx)
{
    ສົ່ງຄືນ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_job_arg(args, idx) != FAIL) ? OK : FAIL;
}
#ບໍ່ດັ່ງນັ້ນ
/*
 * Give an error and return FAIL unless "args[idx]" is an optional channel or a
 * job.  Used without the +channel feature, thus only VAR_UNKNOWN is accepted.
 */
    ຈຳນວນເຕັມ
check_for_opt_chan_or_job_arg(typval_T *args, ຈຳນວນເຕັມ idx)
{
    ສົ່ງຄືນ args[idx].v_type == VAR_UNKNOWN ? OK : FAIL;
}
#endif

/*
 * Give an error and return FAIL unless "args[idx]" is a string or
 * a number.
 */
    ຈຳນວນເຕັມ
check_for_string_or_number_arg(typval_T *args, ຈຳນວນເຕັມ idx)
{
    ຖ້າ (args[idx].v_type != VAR_STRING && args[idx].v_type != VAR_NUMBER)
    {
	semsg(_(e_string_or_number_required_for_argument_nr), idx + 1);
	ສົ່ງຄືນ FAIL;
    }
    ສົ່ງຄືນ OK;
}

/*
 * Check for an optional string or number argument at 'idx'.
 */
    ຈຳນວນເຕັມ
check_for_opt_string_or_number_arg(typval_T *args, ຈຳນວນເຕັມ idx)
{
    ສົ່ງຄືນ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_string_or_number_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a buffer number.
 * Buffer number can be a number or a string.
 */
    ຈຳນວນເຕັມ
check_for_buffer_arg(typval_T *args, ຈຳນວນເຕັມ idx)
{
    ສົ່ງຄືນ check_for_string_or_number_arg(args, idx);
}

/*
 * Check for an optional buffer argument at 'idx'
 */
    ຈຳນວນເຕັມ
check_for_opt_buffer_arg(typval_T *args, ຈຳນວນເຕັມ idx)
{
    ສົ່ງຄືນ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_buffer_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a line number.
 * Line number can be a number or a string.
 */
    ຈຳນວນເຕັມ
check_for_lnum_arg(typval_T *args, ຈຳນວນເຕັມ idx)
{
    ສົ່ງຄືນ check_for_string_or_number_arg(args, idx);
}

/*
 * Check for an optional line number argument at 'idx'
 */
    ຈຳນວນເຕັມ
check_for_opt_lnum_arg(typval_T *args, ຈຳນວນເຕັມ idx)
{
    ສົ່ງຄືນ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_lnum_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a blob.
 */
    ຈຳນວນເຕັມ
check_for_string_or_blob_arg(typval_T *args, ຈຳນວນເຕັມ idx)
{
    ຖ້າ (args[idx].v_type != VAR_STRING && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_string_or_blob_required_for_argument_nr), idx + 1);
	ສົ່ງຄືນ FAIL;
    }
    ສົ່ງຄືນ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a list.
 */
    ຈຳນວນເຕັມ
check_for_string_or_list_arg(typval_T *args, ຈຳນວນເຕັມ idx)
{
    ຖ້າ (args[idx].v_type != VAR_STRING && args[idx].v_type != VAR_LIST)
    {
	semsg(_(e_string_or_list_required_for_argument_nr), idx + 1);
	ສົ່ງຄືນ FAIL;
    }
    ສົ່ງຄືນ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string, a list, a
 * tuple or a blob.
 */
    ຈຳນວນເຕັມ
check_for_string_or_list_or_tuple_or_blob_arg(typval_T *args, ຈຳນວນເຕັມ idx)
{
    ຖ້າ (args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_string_list_tuple_or_blob_required_for_argument_nr), idx + 1);
	ສົ່ງຄືນ FAIL;
    }
    ສົ່ງຄືນ OK;
}

/*
 * Check for an optional string or list argument at 'idx'
 */
    ຈຳນວນເຕັມ
check_for_opt_string_or_list_arg(typval_T *args, ຈຳນວນເຕັມ idx)
{
    ສົ່ງຄືນ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_string_or_list_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a dict.
 */
    ຈຳນວນເຕັມ
check_for_string_or_dict_arg(typval_T *args, ຈຳນວນເຕັມ idx)
{
    ຖ້າ (args[idx].v_type != VAR_STRING && args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_string_or_dict_required_for_argument_nr), idx + 1);
	ສົ່ງຄືນ FAIL;
    }
    ສົ່ງຄືນ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a number
 * or a list.
 */
    ຈຳນວນເຕັມ
check_for_string_or_number_or_list_arg(typval_T *args, ຈຳນວນເຕັມ idx)
{
    ຖ້າ (args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_NUMBER
	    && args[idx].v_type != VAR_LIST)
    {
	semsg(_(e_string_number_or_list_required_for_argument_nr), idx + 1);
	ສົ່ງຄືນ FAIL;
    }
    ສົ່ງຄືນ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an optional string
 * or number or a list
 */
    ຈຳນວນເຕັມ
check_for_opt_string_or_number_or_list_arg(typval_T *args, ຈຳນວນເຕັມ idx)
{
    ສົ່ງຄືນ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_string_or_number_or_list_arg(args, idx)
							!= FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string, a number, a
 * list, a tuple or a blob.
 */
    ຈຳນວນເຕັມ
check_for_repeat_func_arg(typval_T *args, ຈຳນວນເຕັມ idx)
{
    ຖ້າ (args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_NUMBER
	    && args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_repeatable_type_required_for_argument_nr), idx + 1);
	ສົ່ງຄືນ FAIL;
    }
    ສົ່ງຄືນ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string, a list, a
 * tuple or a dict.
 */
    ຈຳນວນເຕັມ
check_for_string_list_tuple_or_dict_arg(typval_T *args, ຈຳນວນເຕັມ idx)
{
    ຖ້າ (args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_string_list_tuple_or_dict_required_for_argument_nr), idx + 1);
	ສົ່ງຄືນ FAIL;
    }
    ສົ່ງຄືນ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string
 * or a function reference.
 */
    ຈຳນວນເຕັມ
check_for_string_or_func_arg(typval_T *args, ຈຳນວນເຕັມ idx)
{
    ຖ້າ (args[idx].v_type != VAR_PARTIAL
	    && args[idx].v_type != VAR_FUNC
	    && args[idx].v_type != VAR_STRING)
    {
	semsg(_(e_string_or_function_required_for_argument_nr), idx + 1);
	ສົ່ງຄືນ FAIL;
    }
    ສົ່ງຄືນ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list or a blob.
 */
    ຈຳນວນເຕັມ
check_for_list_or_blob_arg(typval_T *args, ຈຳນວນເຕັມ idx)
{
    ຖ້າ (args[idx].v_type != VAR_LIST && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_list_or_blob_required_for_argument_nr), idx + 1);
	ສົ່ງຄືນ FAIL;
    }
    ສົ່ງຄືນ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list or a tuple.
 */
    ຈຳນວນເຕັມ
check_for_list_or_tuple_arg(typval_T *args, ຈຳນວນເຕັມ idx)
{
    ຖ້າ (args[idx].v_type != VAR_LIST && args[idx].v_type != VAR_TUPLE)
    {
	semsg(_(e_list_or_tuple_required_for_argument_nr), idx + 1);
	ສົ່ງຄືນ FAIL;
    }
    ສົ່ງຄືນ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list, a tuple or a
 * blob.
 */
    ຈຳນວນເຕັມ
check_for_list_or_tuple_or_blob_arg(typval_T *args, ຈຳນວນເຕັມ idx)
{
    ຖ້າ (args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_list_or_tuple_or_blob_required_for_argument_nr), idx + 1);
	ສົ່ງຄືນ FAIL;
    }
    ສົ່ງຄືນ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list, a tuple or a
 * dict
 */
    ຈຳນວນເຕັມ
check_for_list_or_tuple_or_dict_arg(typval_T *args, ຈຳນວນເຕັມ idx)
{
    ຖ້າ (args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_list_or_tuple_or_dict_required_for_argument_nr), idx + 1);
	ສົ່ງຄືນ FAIL;
    }
    ສົ່ງຄືນ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list or dict or a
 * blob.
 */
    ຈຳນວນເຕັມ
check_for_list_or_dict_or_blob_arg(typval_T *args, ຈຳນວນເຕັມ idx)
{
    ຖ້າ (args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_DICT
	    && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_list_dict_or_blob_required_for_argument_nr), idx + 1);
	ສົ່ງຄືນ FAIL;
    }
    ສົ່ງຄືນ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list, a tuple, a dict,
 * a blob or a string.
 */
    ຈຳນວນເຕັມ
check_for_list_tuple_dict_blob_or_string_arg(typval_T *args, ຈຳນວນເຕັມ idx)
{
    ຖ້າ (args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_DICT
	    && args[idx].v_type != VAR_BLOB
	    && args[idx].v_type != VAR_STRING)
    {
	semsg(_(e_list_tuple_dict_blob_or_string_required_for_argument_nr), idx + 1);
	ສົ່ງຄືນ FAIL;
    }
    ສົ່ງຄືນ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an optional buffer
 * number or a dict.
 */
    ຈຳນວນເຕັມ
check_for_opt_buffer_or_dict_arg(typval_T *args, ຈຳນວນເຕັມ idx)
{
    ຖ້າ (args[idx].v_type != VAR_UNKNOWN
	    && args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_NUMBER
	    && args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_string_required_for_argument_nr), idx + 1);
	ສົ່ງຄືນ FAIL;
    }
    ສົ່ງຄືນ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an object.
 */
    ຈຳນວນເຕັມ
check_for_object_arg(typval_T *args, ຈຳນວນເຕັມ idx)
{
    ຖ້າ (args[idx].v_type != VAR_OBJECT)
    {
	semsg(_(e_object_required_for_argument_nr), idx + 1);
	ສົ່ງຄືນ FAIL;
    }
    ສົ່ງຄືນ OK;
}

/*
 * Returns TRUE if "tv" is a type alias for a class
 */
    static ຈຳນວນເຕັມ
tv_class_alias(typval_T *tv)
{
    ສົ່ງຄືນ tv->v_type == VAR_TYPEALIAS &&
			tv->vval.v_typealias->ta_type->tt_type == VAR_OBJECT;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a class
 * or class typealias.
 */
    ຈຳນວນເຕັມ
check_for_class_or_typealias_args(typval_T *args, ຈຳນວນເຕັມ idx)
{
    ສຳລັບ (ຈຳນວນເຕັມ i = idx; args[i].v_type != VAR_UNKNOWN; ++i)
    {
	ຖ້າ (args[i].v_type != VAR_CLASS && !tv_class_alias(&args[idx]))
	{
	    semsg(_(e_class_or_typealias_required_for_argument_nr), i + 1);
	    ສົ່ງຄືນ FAIL;
	}
    }
    ສົ່ງຄືນ OK;
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

    ສົ່ງຄືນ tv_get_string_buf(varp, mybuf);
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

    ສົ່ງຄືນ res != NULL ? res : (char_u *)"";
}

    char_u *
tv_get_string_buf(typval_T *varp, char_u *buf)
{
    char_u	*res = tv_get_string_buf_chk(varp, buf);

    ສົ່ງຄືນ res != NULL ? res : (char_u *)"";
}

/*
 * Careful: This uses a single, static buffer.  YOU CAN ONLY USE IT ONCE!
 */
    char_u *
tv_get_string_chk(typval_T *varp)
{
    static char_u   mybuf[NUMBUFLEN];

    ສົ່ງຄືນ tv_get_string_buf_chk(varp, mybuf);
}

    char_u *
tv_get_string_buf_chk(typval_T *varp, char_u *buf)
{
    ສົ່ງຄືນ tv_get_string_buf_chk_strict(varp, buf, FALSE);
}

    char_u *
tv_get_string_buf_chk_strict(typval_T *varp, char_u *buf, ຈຳນວນເຕັມ strict)
{
    switch (varp->v_type)
    {
	pak_sa VAR_NUMBER:
	    ຖ້າ (strict)
	    {
		emsg(_(e_using_number_as_string));
		bha_nga;
	    }
	    vim_snprintf((var_na *)buf, NUMBUFLEN, "%lld",
					    (varnumber_T)varp->vval.v_number);
	    ສົ່ງຄືນ buf;
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
	    ຖ້າ (strict)
	    {
		emsg(_(e_using_float_as_string));
		bha_nga;
	    }
	    vim_snprintf((var_na *)buf, NUMBUFLEN, "%g", varp->vval.v_float);
	    ສົ່ງຄືນ buf;
	pak_sa VAR_STRING:
	    ຖ້າ (varp->vval.v_string != NULL)
		ສົ່ງຄືນ varp->vval.v_string;
	    ສົ່ງຄືນ (char_u *)"";
	pak_sa VAR_BOOL:
	pak_sa VAR_SPECIAL:
	    STRCPY(buf, get_var_special_name(varp->vval.v_number));
	    ສົ່ງຄືນ buf;
	pak_sa VAR_BLOB:
	    emsg(_(e_using_blob_as_string));
	    bha_nga;
	pak_sa VAR_CLASS:
	pak_sa VAR_TYPEALIAS:
	    check_typval_is_value(varp);
	    bha_nga;
	pak_sa VAR_OBJECT:
	    {
		ຖ້າ (varp->vval.v_object == NULL)
		    emsg(_(e_using_object_as_string));
		ບໍ່ດັ່ງນັ້ນ
		{
		    class_T *cl = varp->vval.v_object->obj_class;
		    ຖ້າ (cl != NULL && IS_ENUM(cl))
			semsg(_(e_using_enum_str_as_string), cl->class_name.string);
		    ບໍ່ດັ່ງນັ້ນ
			emsg(_(e_using_object_as_string));
		}
	    }
	    bha_nga;
	pak_sa VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    ຖ້າ (in_vim9script())
	    {
		semsg(_(e_using_invalid_value_as_string_str), "job");
		bha_nga;
	    }
	    ສົ່ງຄືນ job_to_string_buf(varp, buf);
#endif
	    bha_nga;
	pak_sa VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    ຖ້າ (in_vim9script())
	    {
		semsg(_(e_using_invalid_value_as_string_str), "channel");
		bha_nga;
	    }
	    ສົ່ງຄືນ channel_to_string_buf(varp, buf);
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
    ສົ່ງຄືນ NULL;
}

/*
 * Turn a typeval into a string.  Similar to tv_get_string_buf() but uses
 * string() on Dict, List, etc.
 */
    char_u *
tv_stringify(typval_T *varp, char_u *buf)
{
    ຖ້າ (varp->v_type == VAR_LIST
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
	ສົ່ງຄືນ tmp.vval.v_string;
    }
    ສົ່ງຄືນ tv_get_string_buf(varp, buf);
}

/*
 * Return TRUE if typeval "tv" and its value are set to be locked (immutable).
 * Also give an error message, using "name" or _("name") when use_gettext is
 * TRUE.
 */
    ຈຳນວນເຕັມ
tv_check_lock(typval_T *tv, char_u *name, ຈຳນວນເຕັມ use_gettext)
{
    ຈຳນວນເຕັມ	lock = 0;

    switch (tv->v_type)
    {
	pak_sa VAR_BLOB:
	    ຖ້າ (tv->vval.v_blob != NULL)
		lock = tv->vval.v_blob->bv_lock;
	    bha_nga;
	pak_sa VAR_LIST:
	    ຖ້າ (tv->vval.v_list != NULL)
		lock = tv->vval.v_list->lv_lock;
	    bha_nga;
	pak_sa VAR_TUPLE:
	    ຖ້າ (tv->vval.v_tuple != NULL)
		lock = tv->vval.v_tuple->tv_lock;
	    bha_nga;
	pak_sa VAR_DICT:
	    ຖ້າ (tv->vval.v_dict != NULL)
		lock = tv->vval.v_dict->dv_lock;
	    bha_nga;
	default:
	    bha_nga;
    }
    ສົ່ງຄືນ value_check_lock(tv->v_lock, name, use_gettext)
		   || (lock != 0 && value_check_lock(lock, name, use_gettext));
}

/*
 * Copy the values from typval_T "from" to typval_T "to".
 * When needed allocates string or increases reference count.
 * Does not make a copy of a list, blob or dict but copies the reference!
 * It is OK for "from" and "to" to point to the same item.  This is used to
 * make a copy later.
 */
    ຈຳນວນເຕັມ
copy_tv(typval_T *from, typval_T *to)
{
    ຈຳນວນເຕັມ		ret = OK;

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
	    ຖ້າ (to->vval.v_job != NULL)
		++to->vval.v_job->jv_refcount;
	    bha_nga;
#endif
	pak_sa VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    to->vval.v_channel = from->vval.v_channel;
	    ຖ້າ (to->vval.v_channel != NULL)
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
	    ຖ້າ (from->vval.v_string == NULL)
		to->vval.v_string = NULL;
	    ບໍ່ດັ່ງນັ້ນ
	    {
		to->vval.v_string = vim_strsave(from->vval.v_string);
		ຖ້າ (from->v_type == VAR_FUNC)
		    func_ref(to->vval.v_string);
	    }
	    bha_nga;
	pak_sa VAR_PARTIAL:
	    ຖ້າ (from->vval.v_partial == NULL)
		to->vval.v_partial = NULL;
	    ບໍ່ດັ່ງນັ້ນ
	    {
		to->vval.v_partial = from->vval.v_partial;
		++to->vval.v_partial->pt_refcount;
	    }
	    bha_nga;
	pak_sa VAR_BLOB:
	    ຖ້າ (from->vval.v_blob == NULL)
		to->vval.v_blob = NULL;
	    ບໍ່ດັ່ງນັ້ນ
	    {
		to->vval.v_blob = from->vval.v_blob;
		++to->vval.v_blob->bv_refcount;
	    }
	    bha_nga;
	pak_sa VAR_LIST:
	    ຖ້າ (from->vval.v_list == NULL)
		to->vval.v_list = NULL;
	    ບໍ່ດັ່ງນັ້ນ
	    {
		to->vval.v_list = from->vval.v_list;
		++to->vval.v_list->lv_refcount;
	    }
	    bha_nga;
	pak_sa VAR_TUPLE:
	    ຖ້າ (from->vval.v_tuple == NULL)
		to->vval.v_tuple = NULL;
	    ບໍ່ດັ່ງນັ້ນ
	    {
		to->vval.v_tuple = from->vval.v_tuple;
		++to->vval.v_tuple->tv_refcount;
	    }
	    bha_nga;
	pak_sa VAR_DICT:
	    ຖ້າ (from->vval.v_dict == NULL)
		to->vval.v_dict = NULL;
	    ບໍ່ດັ່ງນັ້ນ
	    {
		to->vval.v_dict = from->vval.v_dict;
		++to->vval.v_dict->dv_refcount;
	    }
	    bha_nga;
	pak_sa VAR_TYPEALIAS:
	    ຖ້າ (from->vval.v_typealias == NULL)
		to->vval.v_typealias = NULL;
	    ບໍ່ດັ່ງນັ້ນ
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

    ສົ່ງຄືນ ret;
}

/*
 * Compare "tv1" and "tv2".
 */
    ຈຳນວນເຕັມ
typval_compare2(
    typval_T	*tv1,	// first operand
    typval_T	*tv2,	// second operand
    exprtype_T	type,	// operator
    ຈຳນວນເຕັມ		ic,	// ignore case
    ຈຳນວນເຕັມ		*res)	// comparison result
{
    varnumber_T	n1, n2;
    ຈຳນວນເຕັມ		type_is = type == EXPR_IS || type == EXPR_ISNOT;

    ຖ້າ (check_typval_is_value(tv1) == FAIL
	|| check_typval_is_value(tv2) == FAIL)
	ສົ່ງຄືນ FAIL;
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (type_is && tv1->v_type != tv2->v_type)
    {
	// For "is" a different type always means FALSE, for "isnot"
	// it means TRUE.
	*res = (type == EXPR_ISNOT);
    }
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (((tv1->v_type == VAR_SPECIAL && tv1->vval.v_number == VVAL_NULL)
		|| (tv2->v_type == VAR_SPECIAL
					   && tv2->vval.v_number == VVAL_NULL))
	    && tv1->v_type != tv2->v_type
	    && (type == EXPR_EQUAL || type == EXPR_NEQUAL))
    {
	n1 = typval_compare_null(tv1, tv2);
	ຖ້າ (n1 == MAYBE)
	    ສົ່ງຄືນ FAIL;
	ຖ້າ (type == EXPR_NEQUAL)
	    n1 = !n1;
	*res = n1;
    }
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (tv1->v_type == VAR_BLOB || tv2->v_type == VAR_BLOB)
    {
	ຖ້າ (typval_compare_blob(tv1, tv2, type, res) == FAIL)
	    ສົ່ງຄືນ FAIL;
    }
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (tv1->v_type == VAR_LIST || tv2->v_type == VAR_LIST)
    {
	ຖ້າ (typval_compare_list(tv1, tv2, type, ic, res) == FAIL)
	    ສົ່ງຄືນ FAIL;
    }
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (tv1->v_type == VAR_TUPLE || tv2->v_type == VAR_TUPLE)
    {
	ຖ້າ (typval_compare_tuple(tv1, tv2, type, ic, res) == FAIL)
	    ສົ່ງຄືນ FAIL;
    }
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (tv1->v_type == VAR_OBJECT || tv2->v_type == VAR_OBJECT)
    {
	ຖ້າ (typval_compare_object(tv1, tv2, type, ic, res) == FAIL)
	    ສົ່ງຄືນ FAIL;
    }
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (tv1->v_type == VAR_DICT || tv2->v_type == VAR_DICT)
    {
	ຖ້າ (typval_compare_dict(tv1, tv2, type, ic, res) == FAIL)
	    ສົ່ງຄືນ FAIL;
    }
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (tv1->v_type == VAR_FUNC || tv2->v_type == VAR_FUNC
	|| tv1->v_type == VAR_PARTIAL || tv2->v_type == VAR_PARTIAL)
    {
	ຖ້າ (typval_compare_func(tv1, tv2, type, ic, res) == FAIL)
	    ສົ່ງຄືນ FAIL;
    }

    // If one of the two variables is a float, compare as a float.
    // When using "=~" or "!~", always compare as string.
    ບໍ່ດັ່ງນັ້ນ ຖ້າ ((tv1->v_type == VAR_FLOAT || tv2->v_type == VAR_FLOAT)
	    && type != EXPR_MATCH && type != EXPR_NOMATCH)
    {
	float_T f1, f2;
	ຈຳນວນເຕັມ	error = FALSE;

	f1 = tv_get_float_chk(tv1, &error);
	ຖ້າ (!error)
	    f2 = tv_get_float_chk(tv2, &error);
	ຖ້າ (error)
	    ສົ່ງຄືນ FAIL;
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
    ບໍ່ດັ່ງນັ້ນ ຖ້າ ((tv1->v_type == VAR_NUMBER || tv2->v_type == VAR_NUMBER)
	    && type != EXPR_MATCH && type != EXPR_NOMATCH)
    {
	ຈຳນວນເຕັມ error = FALSE;

	n1 = tv_get_number_chk(tv1, &error);
	ຖ້າ (!error)
	    n2 = tv_get_number_chk(tv2, &error);
	ຖ້າ (error)
	    ສົ່ງຄືນ FAIL;
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
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (in_vim9script() && (tv1->v_type == VAR_BOOL
				    || tv2->v_type == VAR_BOOL
				    || (tv1->v_type == VAR_SPECIAL
					      && tv2->v_type == VAR_SPECIAL)))
    {
	ຖ້າ (tv1->v_type != tv2->v_type)
	{
	    semsg(_(e_cannot_compare_str_with_str),
		       vartype_name(tv1->v_type), vartype_name(tv2->v_type));
	    ສົ່ງຄືນ FAIL;
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
		ສົ່ງຄືນ FAIL;
	}
	*res = n1;
    }
#ifdef FEAT_JOB_CHANNEL
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (tv1->v_type == tv2->v_type
	    && (tv1->v_type == VAR_CHANNEL || tv1->v_type == VAR_JOB)
	    && (type == EXPR_NEQUAL || type == EXPR_EQUAL))
    {
	ຖ້າ (tv1->v_type == VAR_CHANNEL)
	    n1 = tv1->vval.v_channel == tv2->vval.v_channel;
	ບໍ່ດັ່ງນັ້ນ
	    n1 = tv1->vval.v_job == tv2->vval.v_job;
	ຖ້າ (type == EXPR_NEQUAL)
	    n1 = !n1;
	*res = n1;
    }
#endif
    ບໍ່ດັ່ງນັ້ນ
    {
	ຖ້າ (typval_compare_string(tv1, tv2, type, ic, res) == FAIL)
	    ສົ່ງຄືນ FAIL;
    }

    ສົ່ງຄືນ OK;
}

/*
 * Compare "tv1" and "tv2".
 * Put the result in "tv1".  Caller should clear "tv2".
 */
    ຈຳນວນເຕັມ
typval_compare(
    typval_T	*tv1,	// first operand
    typval_T	*tv2,	// second operand
    exprtype_T	type,   // operator
    ຈຳນວນເຕັມ		ic)     // ignore case
{
    ຈຳນວນເຕັມ res;

    ຖ້າ (typval_compare2(tv1, tv2, type, ic, &res) == FAIL)
    {
	clear_tv(tv1);
	ສົ່ງຄືນ FAIL;
    }

    clear_tv(tv1);
    ຖ້າ (in_vim9script())
    {
	tv1->v_type = VAR_BOOL;
	tv1->vval.v_number = res ? VVAL_TRUE : VVAL_FALSE;
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	tv1->v_type = VAR_NUMBER;
	tv1->vval.v_number = res;
    }

    ສົ່ງຄືນ OK;
}

/*
 * Compare "tv1" to "tv2" as lists according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    ຈຳນວນເຕັມ
typval_compare_list(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	ຈຳນວນເຕັມ	    ic,
	ຈຳນວນເຕັມ	    *res)
{
    ຈຳນວນເຕັມ	    val = 0;

    ຖ້າ (type == EXPR_IS || type == EXPR_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
				      && tv1->vval.v_list == tv2->vval.v_list);
	ຖ້າ (type == EXPR_ISNOT)
	    val = !val;
    }
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (tv1->v_type != tv2->v_type
	    || (type != EXPR_EQUAL && type != EXPR_NEQUAL))
    {
	ຖ້າ (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_list_with_list));
	ບໍ່ດັ່ງນັ້ນ
	    emsg(_(e_invalid_operation_for_list));
	ສົ່ງຄືນ FAIL;
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	val = list_equal(tv1->vval.v_list, tv2->vval.v_list, ic);
	ຖ້າ (type == EXPR_NEQUAL)
	    val = !val;
    }
    *res = val;
    ສົ່ງຄືນ OK;
}

/*
 * Compare "tv1" to "tv2" as tuples according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    ຈຳນວນເຕັມ
typval_compare_tuple(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	ຈຳນວນເຕັມ	    ic,
	ຈຳນວນເຕັມ	    *res)
{
    ຈຳນວນເຕັມ	    val = 0;

    ຖ້າ (type == EXPR_IS || type == EXPR_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
			      && tv1->vval.v_tuple == tv2->vval.v_tuple);
	ຖ້າ (type == EXPR_ISNOT)
	    val = !val;
    }
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (tv1->v_type != tv2->v_type
	    || (type != EXPR_EQUAL && type != EXPR_NEQUAL))
    {
	ຖ້າ (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_tuple_with_tuple));
	ບໍ່ດັ່ງນັ້ນ
	    emsg(_(e_invalid_operation_for_tuple));
	ສົ່ງຄືນ FAIL;
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	val = tuple_equal(tv1->vval.v_tuple, tv2->vval.v_tuple, ic);
	ຖ້າ (type == EXPR_NEQUAL)
	    val = !val;
    }
    *res = val;
    ສົ່ງຄືນ OK;
}

/*
 * Compare v:null with another type.  Return TRUE if the value is NULL.
 */
    ຈຳນວນເຕັມ
typval_compare_null(typval_T *tv1, typval_T *tv2)
{
    ຖ້າ ((tv1->v_type == VAR_SPECIAL && tv1->vval.v_number == VVAL_NULL)
	    || (tv2->v_type == VAR_SPECIAL && tv2->vval.v_number == VVAL_NULL))
    {
	typval_T	*tv = tv1->v_type == VAR_SPECIAL ? tv2 : tv1;

	switch (tv->v_type)
	{
	    pak_sa VAR_BLOB: ສົ່ງຄືນ tv->vval.v_blob == NULL;
#ifdef FEAT_JOB_CHANNEL
	    pak_sa VAR_CHANNEL: ສົ່ງຄືນ tv->vval.v_channel == NULL;
#endif
	    // TODO: null_class handling
	    // case VAR_CLASS: return tv->vval.v_class == NULL;
	    pak_sa VAR_DICT: ສົ່ງຄືນ tv->vval.v_dict == NULL;
	    pak_sa VAR_FUNC: ສົ່ງຄືນ tv->vval.v_string == NULL;
#ifdef FEAT_JOB_CHANNEL
	    pak_sa VAR_JOB: ສົ່ງຄືນ tv->vval.v_job == NULL;
#endif
	    pak_sa VAR_LIST: ສົ່ງຄືນ tv->vval.v_list == NULL;
	    pak_sa VAR_TUPLE: ສົ່ງຄືນ tv->vval.v_tuple == NULL;
	    pak_sa VAR_OBJECT: ສົ່ງຄືນ tv->vval.v_object == NULL;
	    pak_sa VAR_PARTIAL: ສົ່ງຄືນ tv->vval.v_partial == NULL;
	    pak_sa VAR_STRING: ສົ່ງຄືນ tv->vval.v_string == NULL;

	    pak_sa VAR_NUMBER: ຖ້າ (!in_vim9script())
				 ສົ່ງຄືນ tv->vval.v_number == 0;
			     bha_nga;
	    pak_sa VAR_FLOAT: ຖ້າ (!in_vim9script())
				 ສົ່ງຄືນ tv->vval.v_float == 0.0;
			     bha_nga;
	    pak_sa VAR_TYPEALIAS: ສົ່ງຄືນ tv->vval.v_typealias == NULL;
	    default: bha_nga;
	}
    }
    // although comparing null with number, float or bool is not very useful
    // we won't give an error
    ສົ່ງຄືນ FALSE;
}

/*
 * Compare "tv1" to "tv2" as blobs according to "type".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    ຈຳນວນເຕັມ
typval_compare_blob(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	ຈຳນວນເຕັມ	    *res)
{
    ຈຳນວນເຕັມ	    val = 0;

    ຖ້າ (type == EXPR_IS || type == EXPR_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
			&& tv1->vval.v_blob == tv2->vval.v_blob);
	ຖ້າ (type == EXPR_ISNOT)
	    val = !val;
    }
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (tv1->v_type != tv2->v_type
	    || (type != EXPR_EQUAL && type != EXPR_NEQUAL))
    {
	ຖ້າ (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_blob_with_blob));
	ບໍ່ດັ່ງນັ້ນ
	    emsg(_(e_invalid_operation_for_blob));
	ສົ່ງຄືນ FAIL;
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	val = blob_equal(tv1->vval.v_blob, tv2->vval.v_blob);
	ຖ້າ (type == EXPR_NEQUAL)
	    val = !val;
    }
    *res = val;
    ສົ່ງຄືນ OK;
}

/*
 * Compare "tv1" to "tv2" as objects according to "type".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    ຈຳນວນເຕັມ
typval_compare_object(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	ຈຳນວນເຕັມ	    ic,
	ຈຳນວນເຕັມ	    *res)
{
    ຈຳນວນເຕັມ res_match = type == EXPR_EQUAL || type == EXPR_IS ? TRUE : FALSE;

    ຖ້າ (tv1->vval.v_object == NULL && tv2->vval.v_object == NULL)
    {
	*res = res_match;
	ສົ່ງຄືນ OK;
    }
    ຖ້າ (tv1->vval.v_object == NULL || tv2->vval.v_object == NULL)
    {
	*res = !res_match;
	ສົ່ງຄືນ OK;
    }

    object_T *obj1 = tv1->vval.v_object;
    object_T *obj2 = tv2->vval.v_object;
    ຖ້າ (type == EXPR_IS || type == EXPR_ISNOT)
    {
	*res = obj1 == obj2 ? res_match : !res_match;
	ສົ່ງຄືນ OK;
    }
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (tv1->v_type != tv2->v_type)
    {
	emsg(_(e_can_only_compare_object_with_object));
	ສົ່ງຄືນ FAIL;
    }

    *res = object_equal(obj1, obj2, ic) ? res_match : !res_match;
    ສົ່ງຄືນ OK;
}

/*
 * Compare "tv1" to "tv2" as dictionaries according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    ຈຳນວນເຕັມ
typval_compare_dict(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	ຈຳນວນເຕັມ	    ic,
	ຈຳນວນເຕັມ	    *res)
{
    ຈຳນວນເຕັມ	    val;

    ຖ້າ (type == EXPR_IS || type == EXPR_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
			&& tv1->vval.v_dict == tv2->vval.v_dict);
	ຖ້າ (type == EXPR_ISNOT)
	    val = !val;
    }
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (tv1->v_type != tv2->v_type
		|| (type != EXPR_EQUAL && type != EXPR_NEQUAL))
    {
	ຖ້າ (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_dictionary_with_dictionary));
	ບໍ່ດັ່ງນັ້ນ
	    emsg(_(e_invalid_operation_for_dictionary));
	ສົ່ງຄືນ FAIL;
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	val = dict_equal(tv1->vval.v_dict, tv2->vval.v_dict, ic);
	ຖ້າ (type == EXPR_NEQUAL)
	    val = !val;
    }
    *res = val;
    ສົ່ງຄືນ OK;
}

/*
 * Compare "tv1" to "tv2" as funcrefs according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    ຈຳນວນເຕັມ
typval_compare_func(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	ຈຳນວນເຕັມ	    ic,
	ຈຳນວນເຕັມ	    *res)
{
    ຈຳນວນເຕັມ	    val = 0;

    ຖ້າ (type != EXPR_EQUAL && type != EXPR_NEQUAL
	    && type != EXPR_IS && type != EXPR_ISNOT)
    {
	emsg(_(e_invalid_operation_for_funcrefs));
	ສົ່ງຄືນ FAIL;
    }
    ຖ້າ ((tv1->v_type == VAR_PARTIAL && tv1->vval.v_partial == NULL)
	    || (tv2->v_type == VAR_PARTIAL && tv2->vval.v_partial == NULL))
	// When both partials are NULL, then they are equal.
	// Otherwise they are not equal.
	val = (tv1->vval.v_partial == tv2->vval.v_partial);
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (type == EXPR_IS || type == EXPR_ISNOT)
    {
	ຖ້າ (tv1->v_type == VAR_FUNC && tv2->v_type == VAR_FUNC)
	    // strings are considered the same if their value is
	    // the same
	    val = tv_equal(tv1, tv2, ic);
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (tv1->v_type == VAR_PARTIAL && tv2->v_type == VAR_PARTIAL)
	    val = (tv1->vval.v_partial == tv2->vval.v_partial);
	ບໍ່ດັ່ງນັ້ນ
	    val = FALSE;
    }
    ບໍ່ດັ່ງນັ້ນ
	val = tv_equal(tv1, tv2, ic);
    ຖ້າ (type == EXPR_NEQUAL || type == EXPR_ISNOT)
	val = !val;
    *res = val;
    ສົ່ງຄືນ OK;
}

/*
 * Compare "tv1" to "tv2" as strings according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    ຈຳນວນເຕັມ
typval_compare_string(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	ຈຳນວນເຕັມ	    ic,
	ຈຳນວນເຕັມ	    *res)
{
    ຈຳນວນເຕັມ		i = 0;
    ຈຳນວນເຕັມ		val = FALSE;
    char_u	*s1, *s2;
    char_u	buf1[NUMBUFLEN], buf2[NUMBUFLEN];

    ຖ້າ (in_vim9script()
	  && ((tv1->v_type != VAR_STRING && tv1->v_type != VAR_SPECIAL)
	   || (tv2->v_type != VAR_STRING && tv2->v_type != VAR_SPECIAL)))
    {
	semsg(_(e_cannot_compare_str_with_str),
		   vartype_name(tv1->v_type), vartype_name(tv2->v_type));
	ສົ່ງຄືນ FAIL;
    }
    s1 = tv_get_string_buf(tv1, buf1);
    s2 = tv_get_string_buf(tv2, buf2);
    ຖ້າ (type != EXPR_MATCH && type != EXPR_NOMATCH)
	i = ic ? MB_STRICMP(s1, s2) : STRCMP(s1, s2);
    switch (type)
    {
	pak_sa EXPR_IS:	    ຖ້າ (in_vim9script())
			    {
				// Really check it is the same string, not just
				// the same value.
				val = tv1->vval.v_string == tv2->vval.v_string;
				bha_nga;
			    }
			    // FALLTHROUGH
	pak_sa EXPR_EQUAL:    val = (i == 0); bha_nga;
	pak_sa EXPR_ISNOT:    ຖ້າ (in_vim9script())
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
		ຖ້າ (type == EXPR_NOMATCH)
		    val = !val;
		bha_nga;

	default:  bha_nga;  // avoid gcc warning
    }
    *res = val;
    ສົ່ງຄືນ OK;
}
/*
 * Convert any type to a string, never give an error.
 * When "quotes" is TRUE add quotes to a string.
 * Returns an allocated string.
 */
    char_u *
typval_tostring(typval_T *arg, ຈຳນວນເຕັມ quotes)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    char_u	*ret = NULL;

    ຖ້າ (arg == NULL)
	ສົ່ງຄືນ vim_strsave((char_u *)"(does not exist)");
    ຖ້າ (!quotes && arg->v_type == VAR_STRING)
    {
	ret = vim_strsave(arg->vval.v_string == NULL ? (char_u *)""
							 : arg->vval.v_string);
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	ret = tv2string(arg, &tofree, numbuf, 0);
	// Make a copy if we have a value but it's not in allocated memory.
	ຖ້າ (ret != NULL && tofree == NULL)
	    ret = vim_strsave(ret);
    }
    ສົ່ງຄືນ ret;
}

/*
 * Return TRUE if typeval "tv" is locked: Either that value is locked itself
 * or it refers to a List or Dictionary that is locked.
 */
    ຈຳນວນເຕັມ
tv_islocked(typval_T *tv)
{
    ສົ່ງຄືນ (tv->v_lock & VAR_LOCKED)
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

    static ຈຳນວນເຕັມ
func_equal(
    typval_T *tv1,
    typval_T *tv2,
    ຈຳນວນເຕັມ	     ic)	    // ignore case
{
    char_u	*s1, *s2;
    dict_T	*d1, *d2;
    ຈຳນວນເຕັມ		a1, a2;
    ຈຳນວນເຕັມ		i;

    // empty and NULL function name considered the same
    s1 = tv1->v_type == VAR_FUNC ? tv1->vval.v_string
					   : partial_name(tv1->vval.v_partial);
    ຖ້າ (s1 != NULL && *s1 == NUL)
	s1 = NULL;
    s2 = tv2->v_type == VAR_FUNC ? tv2->vval.v_string
					   : partial_name(tv2->vval.v_partial);
    ຖ້າ (s2 != NULL && *s2 == NUL)
	s2 = NULL;
    ຖ້າ (s1 == NULL || s2 == NULL)
    {
	ຖ້າ (s1 != s2)
	    ສົ່ງຄືນ FALSE;
    }
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (STRCMP(s1, s2) != 0)
	ສົ່ງຄືນ FALSE;

    // empty dict and NULL dict is different
    d1 = tv1->v_type == VAR_FUNC ? NULL : tv1->vval.v_partial->pt_dict;
    d2 = tv2->v_type == VAR_FUNC ? NULL : tv2->vval.v_partial->pt_dict;
    ຖ້າ (d1 == NULL || d2 == NULL)
    {
	ຖ້າ (d1 != d2)
	    ສົ່ງຄືນ FALSE;
    }
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (!dict_equal(d1, d2, ic))
	ສົ່ງຄືນ FALSE;

    // empty list and no list considered the same
    a1 = tv1->v_type == VAR_FUNC ? 0 : tv1->vval.v_partial->pt_argc;
    a2 = tv2->v_type == VAR_FUNC ? 0 : tv2->vval.v_partial->pt_argc;
    ຖ້າ (a1 != a2)
	ສົ່ງຄືນ FALSE;
    ສຳລັບ (i = 0; i < a1; ++i)
	ຖ້າ (!tv_equal(tv1->vval.v_partial->pt_argv + i,
		      tv2->vval.v_partial->pt_argv + i, ic))
	    ສົ່ງຄືນ FALSE;

    ສົ່ງຄືນ TRUE;
}

/*
 * Return TRUE if "tv1" and "tv2" have the same value.
 * Compares the items just like "==" would compare them, but strings and
 * numbers are different.  Floats and numbers are also different.
 */
    ຈຳນວນເຕັມ
tv_equal(
    typval_T *tv1,
    typval_T *tv2,
    ຈຳນວນເຕັມ	     ic)	    // ignore case
{
    char_u	buf1[NUMBUFLEN], buf2[NUMBUFLEN];
    char_u	*s1, *s2;
    static ຈຳນວນເຕັມ  recursive_cnt = 0;	    // catch recursive loops
    ຈຳນວນເຕັມ		r;
    static ຈຳນວນເຕັມ	tv_equal_recurse_limit;

    // Catch lists and dicts that have an endless loop by limiting
    // recursiveness to a limit.  We guess they are equal then.
    // A fixed limit has the problem of still taking an awful long time.
    // Reduce the limit every time running into it. That should work fine for
    // deeply linked structures that are not recursively linked and catch
    // recursiveness quickly.
    ຖ້າ (recursive_cnt == 0)
	tv_equal_recurse_limit = 1000;
    ຖ້າ (recursive_cnt >= tv_equal_recurse_limit)
    {
	--tv_equal_recurse_limit;
	ສົ່ງຄືນ TRUE;
    }

    // For VAR_FUNC and VAR_PARTIAL compare the function name, bound dict and
    // arguments.
    ຖ້າ ((tv1->v_type == VAR_FUNC
		|| (tv1->v_type == VAR_PARTIAL && tv1->vval.v_partial != NULL))
	    && (tv2->v_type == VAR_FUNC
		|| (tv2->v_type == VAR_PARTIAL && tv2->vval.v_partial != NULL)))
    {
	++recursive_cnt;
	r = func_equal(tv1, tv2, ic);
	--recursive_cnt;
	ສົ່ງຄືນ r;
    }

    ຖ້າ (tv1->v_type != tv2->v_type
	    && ((tv1->v_type != VAR_BOOL && tv1->v_type != VAR_SPECIAL)
		|| (tv2->v_type != VAR_BOOL && tv2->v_type != VAR_SPECIAL)))
	ສົ່ງຄືນ FALSE;

    switch (tv1->v_type)
    {
	pak_sa VAR_LIST:
	    ++recursive_cnt;
	    r = list_equal(tv1->vval.v_list, tv2->vval.v_list, ic);
	    --recursive_cnt;
	    ສົ່ງຄືນ r;

	pak_sa VAR_TUPLE:
	    ++recursive_cnt;
	    r = tuple_equal(tv1->vval.v_tuple, tv2->vval.v_tuple, ic);
	    --recursive_cnt;
	    ສົ່ງຄືນ r;

	pak_sa VAR_DICT:
	    ++recursive_cnt;
	    r = dict_equal(tv1->vval.v_dict, tv2->vval.v_dict, ic);
	    --recursive_cnt;
	    ສົ່ງຄືນ r;

	pak_sa VAR_BLOB:
	    ສົ່ງຄືນ blob_equal(tv1->vval.v_blob, tv2->vval.v_blob);

	pak_sa VAR_NUMBER:
	pak_sa VAR_BOOL:
	pak_sa VAR_SPECIAL:
	    ສົ່ງຄືນ tv1->vval.v_number == tv2->vval.v_number;

	pak_sa VAR_STRING:
	    s1 = tv_get_string_buf(tv1, buf1);
	    s2 = tv_get_string_buf(tv2, buf2);
	    ສົ່ງຄືນ ((ic ? MB_STRICMP(s1, s2) : STRCMP(s1, s2)) == 0);

	pak_sa VAR_FLOAT:
	    ສົ່ງຄືນ tv1->vval.v_float == tv2->vval.v_float;
	pak_sa VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    ສົ່ງຄືນ tv1->vval.v_job == tv2->vval.v_job;
#endif
	pak_sa VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    ສົ່ງຄືນ tv1->vval.v_channel == tv2->vval.v_channel;
#endif
	pak_sa VAR_INSTR:
	    ສົ່ງຄືນ tv1->vval.v_instr == tv2->vval.v_instr;

	pak_sa VAR_CLASS:
	    // A class only exists once, equality is identity.
	    ສົ່ງຄືນ tv1->vval.v_class == tv2->vval.v_class;

	pak_sa VAR_OBJECT:
	    ++recursive_cnt;
	    r = object_equal(tv1->vval.v_object, tv2->vval.v_object, ic);
	    --recursive_cnt;
	    ສົ່ງຄືນ r;

	pak_sa VAR_PARTIAL:
	    ສົ່ງຄືນ tv1->vval.v_partial == tv2->vval.v_partial;

	pak_sa VAR_FUNC:
	    ສົ່ງຄືນ tv1->vval.v_string == tv2->vval.v_string;

	pak_sa VAR_TYPEALIAS:
	    ສົ່ງຄືນ tv1->vval.v_typealias == tv2->vval.v_typealias;

	pak_sa VAR_UNKNOWN:
	pak_sa VAR_ANY:
	pak_sa VAR_VOID:
	    bha_nga;
    }

    // VAR_UNKNOWN can be the result of a invalid expression, let's say it
    // does not equal anything, not even itself.
    ສົ່ງຄືນ FALSE;
}

/*
 * Get an option value.
 * "arg" points to the '&' or '+' before the option name.
 * "arg" is advanced to character after the option name.
 * Return OK or FAIL.
 */
    ຈຳນວນເຕັມ
eval_option(
    char_u	**arg,
    typval_T	*rettv,	// when NULL, only check if option exists
    ຈຳນວນເຕັມ		evaluate)
{
    char_u	*option_end;
    long	numval;
    char_u	*stringval;
    getoption_T	opt_type;
    ຈຳນວນເຕັມ		c;
    ຈຳນວນເຕັມ		working = (**arg == '+');    // has("+option")
    ຈຳນວນເຕັມ		ret = OK;
    ຈຳນວນເຕັມ		scope;

    // Isolate the option name and find its value.
    option_end = find_option_end(arg, &scope);
    ຖ້າ (option_end == NULL)
    {
	ຖ້າ (rettv != NULL)
	    semsg(_(e_option_name_missing_str), *arg);
	ສົ່ງຄືນ FAIL;
    }

    ຖ້າ (!evaluate)
    {
	*arg = option_end;
	ສົ່ງຄືນ OK;
    }

    c = *option_end;
    *option_end = NUL;
    opt_type = get_option_value(*arg, &numval,
			       rettv == NULL ? NULL : &stringval, NULL, scope);

    ຖ້າ (opt_type == gov_unknown)
    {
	ຖ້າ (rettv != NULL)
	    semsg(_(e_unknown_option_str), *arg);
	ret = FAIL;
    }
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (rettv != NULL)
    {
	rettv->v_lock = 0;
	ຖ້າ (opt_type == gov_hidden_string)
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = NULL;
	}
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (opt_type == gov_hidden_bool || opt_type == gov_hidden_number)
	{
	    rettv->v_type = in_vim9script() && opt_type == gov_hidden_bool
						       ? VAR_BOOL : VAR_NUMBER;
	    rettv->vval.v_number = 0;
	}
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (opt_type == gov_bool || opt_type == gov_number)
	{
	    ຖ້າ (in_vim9script() && opt_type == gov_bool)
	    {
		rettv->v_type = VAR_BOOL;
		rettv->vval.v_number = numval ? VVAL_TRUE : VVAL_FALSE;
	    }
	    ບໍ່ດັ່ງນັ້ນ
	    {
		rettv->v_type = VAR_NUMBER;
		rettv->vval.v_number = numval;
	    }
	}
	ບໍ່ດັ່ງນັ້ນ				// string option
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = stringval;
	}
    }
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (working && (opt_type == gov_hidden_bool
			|| opt_type == gov_hidden_number
			|| opt_type == gov_hidden_string))
	ret = FAIL;

    *option_end = c;		    // put back for error messages
    *arg = option_end;

    ສົ່ງຄືນ ret;
}

/*
 * Allocate a variable for a number constant.  Also deals with "0z" for blob.
 * Return OK or FAIL.
 */
    ຈຳນວນເຕັມ
eval_number(
	char_u	    **arg,
	typval_T    *rettv,
	ຈຳນວນເຕັມ	    evaluate,
	ຈຳນວນເຕັມ	    want_string)
{
    ຈຳນວນເຕັມ		len;
    ຈຳນວນເຕັມ		skip_quotes = !in_old_script(4);
    char_u	*p;
    ຈຳນວນເຕັມ		get_float = FALSE;

    // We accept a float when the format matches
    // "[0-9]\+\.[0-9]\+\([eE][+-]\?[0-9]\+\)\?".  This is very
    // strict to avoid backwards compatibility problems.
    // With script version 2 and later the leading digit can be
    // omitted.
    // Don't look for a float after the "." operator, so that
    // ":let vers = 1.2.3" doesn't fail.
    ຖ້າ (**arg == '.')
	p = *arg;
    ບໍ່ດັ່ງນັ້ນ
    {
	p = *arg + 1;
	ຖ້າ (skip_quotes)
	    ສຳລັບ (;;)
	    {
		ຖ້າ (*p == '\'')
		    ++p;
		ຖ້າ (!vim_isdigit(*p))
		    bha_nga;
		p = skipdigits(p);
	    }
	ບໍ່ດັ່ງນັ້ນ
	    p = skipdigits(p);
    }
    ຖ້າ (!want_string && p[0] == '.' && vim_isdigit(p[1]))
    {
	get_float = TRUE;
	p = skipdigits(p + 2);
	ຖ້າ (*p == 'e' || *p == 'E')
	{
	    ++p;
	    ຖ້າ (*p == '-' || *p == '+')
		++p;
	    ຖ້າ (!vim_isdigit(*p))
		get_float = FALSE;
	    ບໍ່ດັ່ງນັ້ນ
		p = skipdigits(p + 1);
	}
	ຖ້າ (ASCII_ISALPHA(*p) || *p == '.')
	    get_float = FALSE;
    }
    ຖ້າ (get_float)
    {
	float_T	f;

	*arg += string2float(*arg, &f, skip_quotes);
	ຖ້າ (evaluate)
	{
	    rettv->v_type = VAR_FLOAT;
	    rettv->vval.v_float = f;
	}
    }
    ບໍ່ດັ່ງນັ້ນ
    ຖ້າ (**arg == '0' && ((*arg)[1] == 'z' || (*arg)[1] == 'Z'))
    {
	char_u  *bp;
	blob_T  *blob = NULL;  // init for gcc

	// Blob constant: 0z0123456789abcdef
	ຖ້າ (evaluate)
	    blob = blob_alloc();
	ສຳລັບ (bp = *arg + 2; vim_isxdigit(bp[0]); bp += 2)
	{
	    ຖ້າ (!vim_isxdigit(bp[1]))
	    {
		ຖ້າ (blob != NULL)
		{
		    emsg(_(e_blob_literal_should_have_an_even_number_of_hex_characters));
		    ga_clear(&blob->bv_ga);
		    VIM_CLEAR(blob);
		}
		ສົ່ງຄືນ FAIL;
	    }
	    ຖ້າ (blob != NULL)
		ga_append(&blob->bv_ga,
			     (hex2nr(*bp) << 4) + hex2nr(*(bp+1)));
	    ຖ້າ (bp[2] == '.' && vim_isxdigit(bp[3]))
		++bp;
	}
	ຖ້າ (blob != NULL)
	    rettv_blob_set(rettv, blob);
	*arg = bp;
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	varnumber_T	n;

	// decimal, hex or octal number
	vim_str2nr(*arg, NULL, &len, skip_quotes
		      ? STR2NR_NO_OCT + STR2NR_QUOTE
		      : STR2NR_ALL, &n, NULL, 0, TRUE, NULL);
	ຖ້າ (len == 0)
	{
	    ຖ້າ (evaluate)
		semsg(_(e_invalid_expression_str), *arg);
	    ສົ່ງຄືນ FAIL;
	}
	*arg += len;
	ຖ້າ (evaluate)
	{
	    rettv->v_type = VAR_NUMBER;
	    rettv->vval.v_number = n;
	}
    }
    ສົ່ງຄືນ OK;
}

/*
 * Evaluate a string constant and put the result in "rettv".
 * "*arg" points to the double quote or to after it when "interpolate" is TRUE.
 * When "interpolate" is TRUE reduce "{{" to "{", reduce "}}" to "}" and stop
 * at a single "{".
 * Return OK or FAIL.
 */
    ຈຳນວນເຕັມ
eval_string(char_u **arg, typval_T *rettv, ຈຳນວນເຕັມ evaluate, ຈຳນວນເຕັມ interpolate)
{
    char_u	*p;
    char_u	*end;
    ຈຳນວນເຕັມ		extra = interpolate ? 1 : 0;
    ຈຳນວນເຕັມ		off = interpolate ? 0 : 1;
    ຈຳນວນເຕັມ		len;

    // Find the end of the string, skipping backslashed characters.
    ສຳລັບ (p = *arg + off; *p != NUL && *p != '"'; MB_PTR_ADV(p))
    {
	ຖ້າ (*p == '\\' && p[1] != NUL)
	{
	    ++p;
	    // A "\<x>" form occupies at least 4 characters, and produces up
	    // to 9 characters (6 for the char and 3 for a modifier):
	    // reserve space for 5 extra.
	    ຖ້າ (*p == '<')
	    {
		ຈຳນວນເຕັມ		modifiers = 0;
		ຈຳນວນເຕັມ		flags = FSK_KEYCODE | FSK_IN_STRING;

		extra += 5;

		// Skip to the '>' to avoid using '{' inside for string
		// interpolation.
		ຖ້າ (p[1] != '*')
		    flags |= FSK_SIMPLIFY;
		ຖ້າ (find_special_key(&p, &modifiers, flags, NULL) != 0)
		    --p;  // leave "p" on the ">"
	    }
	}
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (interpolate && (*p == '{' || *p == '}'))
	{
	    ຖ້າ (*p == '{' && p[1] != '{') // start of expression
		bha_nga;
	    ++p;
	    ຖ້າ (p[-1] == '}' && *p != '}') // single '}' is an error
	    {
		semsg(_(e_stray_closing_curly_str), *arg);
		ສົ່ງຄືນ FAIL;
	    }
	    --extra;  // "{{" becomes "{", "}}" becomes "}"
	}
    }

    ຖ້າ (*p != '"' && !(interpolate && *p == '{'))
    {
	semsg(_(e_missing_double_quote_str), *arg);
	ສົ່ງຄືນ FAIL;
    }

    // If only parsing, set *arg and return here
    ຖ້າ (!evaluate)
    {
	*arg = p + off;
	ສົ່ງຄືນ OK;
    }

    // Copy the string into allocated memory, handling backslashed
    // characters.
    rettv->v_type = VAR_STRING;
    len = (ຈຳນວນເຕັມ)(p - *arg + extra);
    rettv->vval.v_string = alloc(len);
    ຖ້າ (rettv->vval.v_string == NULL)
	ສົ່ງຄືນ FAIL;
    end = rettv->vval.v_string;

    ສຳລັບ (p = *arg + off; *p != NUL && *p != '"'; )
    {
	ຖ້າ (*p == '\\')
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
			  ຖ້າ (vim_isxdigit(p[1]))
			  {
			      ຈຳນວນເຕັມ	n, nr;
			      ຈຳນວນເຕັມ	c = SAFE_toupper(*p);

			      ຖ້າ (c == 'X')
				  n = 2;
			      ບໍ່ດັ່ງນັ້ນ ຖ້າ (*p == 'u')
				  n = 4;
			      ບໍ່ດັ່ງນັ້ນ
				  n = 8;
			      nr = 0;
			      ໃນຂະນະທີ່ (--n >= 0 && vim_isxdigit(p[1]))
			      {
				  ++p;
				  nr = (nr << 4) + hex2nr(*p);
			      }
			      ++p;
			      // For "\u" store the number according to
			      // 'encoding'.
			      ຖ້າ (c != 'X')
				  end += (*mb_char2bytes)(nr, end);
			      ບໍ່ດັ່ງນັ້ນ
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
			  ຖ້າ (*p >= '0' && *p <= '7')
			  {
			      *end = (*end << 3) + *p++ - '0';
			      ຖ້າ (*p >= '0' && *p <= '7')
				  *end = (*end << 3) + *p++ - '0';
			  }
			  ++end;
			  bha_nga;

			  // Special key, e.g.: "\<C-W>"
		pak_sa '<':
			  {
			      ຈຳນວນເຕັມ flags = FSK_KEYCODE | FSK_IN_STRING;

			      ຖ້າ (p[1] != '*')
				  flags |= FSK_SIMPLIFY;
			      extra = trans_special(&p, end, flags, FALSE, NULL);
			      ຖ້າ (extra != 0)
			      {
				  end += extra;
				  ຖ້າ (end >= rettv->vval.v_string + len)
				      iemsg("eval_string() used more space than allocated");
				  bha_nga;
			      }
			  }
			  // FALLTHROUGH

		default: MB_COPY_CHAR(p, end);
			  bha_nga;
	    }
	}
	ບໍ່ດັ່ງນັ້ນ
	{
	    ຖ້າ (interpolate && (*p == '{' || *p == '}'))
	    {
		ຖ້າ (*p == '{' && p[1] != '{') // start of expression
		    bha_nga;
		++p;  // reduce "{{" to "{" and "}}" to "}"
	    }
	    MB_COPY_CHAR(p, end);
	}
    }
    *end = NUL;
    ຖ້າ (*p == '"' && !interpolate)
	++p;
    *arg = p;

    ສົ່ງຄືນ OK;
}

/*
 * Allocate a variable for a 'str''ing' constant.
 * When "interpolate" is TRUE reduce "{{" to "{" and stop at a single "{".
 * Return OK when a "rettv" was set to the string.
 * Return FAIL on error, "rettv" is not set.
 */
    ຈຳນວນເຕັມ
eval_lit_string(char_u **arg, typval_T *rettv, ຈຳນວນເຕັມ evaluate, ຈຳນວນເຕັມ interpolate)
{
    char_u	*p;
    char_u	*str;
    ຈຳນວນເຕັມ		reduce = interpolate ? -1 : 0;
    ຈຳນວນເຕັມ		off = interpolate ? 0 : 1;

    // Find the end of the string, skipping ''.
    ສຳລັບ (p = *arg + off; *p != NUL; MB_PTR_ADV(p))
    {
	ຖ້າ (*p == '\'')
	{
	    ຖ້າ (p[1] != '\'')
		bha_nga;
	    ++reduce;
	    ++p;
	}
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (interpolate)
	{
	    ຖ້າ (*p == '{')
	    {
		ຖ້າ (p[1] != '{')
		    bha_nga;
		++p;
		++reduce;
	    }
	    ບໍ່ດັ່ງນັ້ນ ຖ້າ (*p == '}')
	    {
		++p;
		ຖ້າ (*p != '}')
		{
		    semsg(_(e_stray_closing_curly_str), *arg);
		    ສົ່ງຄືນ FAIL;
		}
		++reduce;
	    }
	}
    }

    ຖ້າ (*p != '\'' && !(interpolate && *p == '{'))
    {
	semsg(_(e_missing_single_quote_str), *arg);
	ສົ່ງຄືນ FAIL;
    }

    // If only parsing return after setting "*arg"
    ຖ້າ (!evaluate)
    {
	*arg = p + off;
	ສົ່ງຄືນ OK;
    }

    // Copy the string into allocated memory, handling '' to ' reduction and
    // any expressions.
    str = alloc((p - *arg) - reduce);
    ຖ້າ (str == NULL)
	ສົ່ງຄືນ FAIL;
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = str;

    ສຳລັບ (p = *arg + off; *p != NUL; )
    {
	ຖ້າ (*p == '\'')
	{
	    ຖ້າ (p[1] != '\'')
		bha_nga;
	    ++p;
	}
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (interpolate && (*p == '{' || *p == '}'))
	{
	    ຖ້າ (*p == '{' && p[1] != '{')
		bha_nga;
	    ++p;
	}
	MB_COPY_CHAR(p, str);
    }
    *str = NUL;
    *arg = p + off;

    ສົ່ງຄືນ OK;
}

/*
 * Evaluate a single or double quoted string possibly containing expressions.
 * "arg" points to the '$'.  The result is put in "rettv".
 * Returns OK or FAIL.
 */
    ຈຳນວນເຕັມ
eval_interp_string(char_u **arg, typval_T *rettv, ຈຳນວນເຕັມ evaluate)
{
    typval_T	tv;
    ຈຳນວນເຕັມ		ret = OK;
    ຈຳນວນເຕັມ		quote;
    garray_T	ga;
    char_u	*p;

    ga_init2(&ga, 1, 80);

    // *arg is on the '$' character, move it to the first string character.
    ++*arg;
    quote = **arg;
    ++*arg;

    ສຳລັບ (;;)
    {
	// Get the string up to the matching quote or to a single '{'.
	// "arg" is advanced to either the quote or the '{'.
	ຖ້າ (quote == '"')
	    ret = eval_string(arg, &tv, evaluate, TRUE);
	ບໍ່ດັ່ງນັ້ນ
	    ret = eval_lit_string(arg, &tv, evaluate, TRUE);
	ຖ້າ (ret == FAIL)
	    bha_nga;
	ຖ້າ (evaluate)
	{
	    ga_concat(&ga, tv.vval.v_string);
	    clear_tv(&tv);
	}

	ຖ້າ (**arg != '{')
	{
	    // found terminating quote
	    ++*arg;
	    bha_nga;
	}
	p = eval_one_expr_in_str(*arg, &ga, evaluate);
	ຖ້າ (p == NULL)
	{
	    ret = FAIL;
	    bha_nga;
	}
	*arg = p;
    }

    rettv->v_type = VAR_STRING;
    ຖ້າ (ret == FAIL || !evaluate || ga_append(&ga, NUL) == FAIL)
    {
	ga_clear(&ga);
	rettv->vval.v_string = NULL;
	ສົ່ງຄືນ ret;
    }

    rettv->vval.v_string = ga.ga_data;
    ສົ່ງຄືນ OK;
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
    ຈຳນວນເຕັມ		copyID)
{
    ສົ່ງຄືນ echo_string_core(tv, tofree, numbuf, copyID, FALSE, TRUE, FALSE);
}

/*
 * Get the value of an environment variable.
 * "arg" is pointing to the '$'.  It is advanced to after the name.
 * If the environment variable was not set, silently assume it is empty.
 * Return FAIL if the name is invalid.
 */
    ຈຳນວນເຕັມ
eval_env_var(char_u **arg, typval_T *rettv, ຈຳນວນເຕັມ evaluate)
{
    char_u	*string = NULL;
    ຈຳນວນເຕັມ		len;
    ຈຳນວນເຕັມ		cc;
    char_u	*name;
    ຈຳນວນເຕັມ		mustfree = FALSE;

    ++*arg;
    name = *arg;
    len = get_env_len(arg);
    ຖ້າ (evaluate)
    {
	ຖ້າ (len == 0)
	    ສົ່ງຄືນ FAIL; // invalid empty name

	cc = name[len];
	name[len] = NUL;
	// first try vim_getenv(), fast for normal environment vars
	string = vim_getenv(name, &mustfree);
	ຖ້າ (string != NULL && *string != NUL)
	{
	    ຖ້າ (!mustfree)
		string = vim_strsave(string);
	}
	ບໍ່ດັ່ງນັ້ນ
	{
	    ຖ້າ (mustfree)
		vim_free(string);

	    // next try expanding things like $VIM and ${HOME}
	    string = expand_env_save(name - 1);
	    ຖ້າ (string != NULL && *string == '$')
		VIM_CLEAR(string);
	}
	name[len] = cc;

	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = string;
	rettv->v_lock = 0;
    }

    ສົ່ງຄືນ OK;
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
    ຈຳນວນເຕັມ		did_emsg_before = did_emsg;

    ຖ້າ (argvars[0].v_type != VAR_STRING || !in_vim9script())
	lnum = (linenr_T)tv_get_number_chk(&argvars[0], NULL);
    ຖ້າ (lnum <= 0 && did_emsg_before == did_emsg
					    && argvars[0].v_type != VAR_NUMBER)
    {
	ຈຳນວນເຕັມ	fnum;
	pos_T	*fp;

	// no valid number, try using arg like line()
	fp = var2fpos(&argvars[0], TRUE, &fnum, FALSE);
	ຖ້າ (fp != NULL)
	    lnum = fp->lnum;
    }
    ສົ່ງຄືນ lnum;
}

/*
 * Get the lnum from the first argument.
 * Also accepts "$", then "buf" is used.
 * Returns 0 on error.
 */
    linenr_T
tv_get_lnum_buf(typval_T *argvars, buf_T *buf)
{
    ຖ້າ (argvars[0].v_type == VAR_STRING
	    && argvars[0].vval.v_string != NULL
	    && argvars[0].vval.v_string[0] == '$'
	    && argvars[0].vval.v_string[1] == NUL
	    && buf != NULL)
	ສົ່ງຄືນ buf->b_ml.ml_line_count;
    ສົ່ງຄືນ (linenr_T)tv_get_number_chk(&argvars[0], NULL);
}

/*
 * Get buffer by number or pattern.
 */
    buf_T *
tv_get_buf(typval_T *tv, ຈຳນວນເຕັມ curtab_only)
{
    char_u	*name = tv->vval.v_string;
    buf_T	*buf;

    ຖ້າ (tv->v_type == VAR_NUMBER)
	ສົ່ງຄືນ buflist_findnr((ຈຳນວນເຕັມ)tv->vval.v_number);
    ຖ້າ (tv->v_type != VAR_STRING)
	ສົ່ງຄືນ NULL;
    ຖ້າ (name == NULL || *name == NUL)
	ສົ່ງຄືນ curbuf;
    ຖ້າ (name[0] == '$' && name[1] == NUL)
	ສົ່ງຄືນ lastbuf;

    buf = buflist_find_by_name(name, curtab_only);

    // If not found, try expanding the name, like done for bufexists().
    ຖ້າ (buf == NULL)
	buf = find_buffer(tv);

    ສົ່ງຄືນ buf;
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
    ຖ້າ (buf == NULL
	    && tv->v_type != VAR_NUMBER
	    && tv->v_type != VAR_STRING)
	// issue errmsg for type error
	(void)tv_get_number(tv);
    ສົ່ງຄືນ buf;
}

/*
 * Check that repeating "slen" items "n" times, does not
 * overflow the int used for the resulting length.
 */
    ຈຳນວນເຕັມ
check_repeat_count(varnumber_T slen, varnumber_T n)
{
    // Use a division so that the multiplication cannot overflow.
    ຖ້າ (n <= (varnumber_T)INT_MAX / slen)
	ສົ່ງຄືນ OK;

    var_na	buf[NUMBUFLEN];

    vim_snprintf(buf, parimaa_na(buf), "%lld", (long long)n);
    semsg(_(e_val_too_large), buf);
    ສົ່ງຄືນ FAIL;
}

#endif // FEAT_EVAL
