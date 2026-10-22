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

#અન્તર્ભાવ "vim.h"

#જો defined(FEAT_EVAL)

/*
 * Allocate memory for a variable type-value, and make it empty (0 or NULL
 * value).
 */
    typval_T *
alloc_tv(શૂન્ય)
{
    પાછા ALLOC_CLEAR_ONE(typval_T);
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
    જો (rettv != ખાલી)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = s;
    }
    નહીંતર
	vim_free(s);
    પાછા rettv;
}

/*
 * Free the memory for a variable type-value.
 */
    શૂન્ય
free_tv(typval_T *varp)
{
    જો (varp == ખાલી)
	પાછા;

    પસંદગી (varp->v_type)
    {
	સ્થિતિ VAR_FUNC:
	    func_unref(varp->vval.v_string);
	    // FALLTHROUGH
	સ્થિતિ VAR_STRING:
	    vim_free(varp->vval.v_string);
	    તોડો;
	સ્થિતિ VAR_PARTIAL:
	    partial_unref(varp->vval.v_partial);
	    તોડો;
	સ્થિતિ VAR_BLOB:
	    blob_unref(varp->vval.v_blob);
	    તોડો;
	સ્થિતિ VAR_LIST:
	    list_unref(varp->vval.v_list);
	    તોડો;
	સ્થિતિ VAR_TUPLE:
	    tuple_unref(varp->vval.v_tuple);
	    તોડો;
	સ્થિતિ VAR_DICT:
	    dict_unref(varp->vval.v_dict);
	    તોડો;
	સ્થિતિ VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    job_unref(varp->vval.v_job);
	    તોડો;
#endif
	સ્થિતિ VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    channel_unref(varp->vval.v_channel);
	    તોડો;
#endif
	સ્થિતિ VAR_CLASS:
	    class_unref(varp->vval.v_class);
	    તોડો;
	સ્થિતિ VAR_OBJECT:
	    object_unref(varp->vval.v_object);
	    તોડો;

	સ્થિતિ VAR_TYPEALIAS:
	    typealias_unref(varp->vval.v_typealias);
	    તોડો;

	સ્થિતિ VAR_સંખ્યા:
	સ્થિતિ VAR_FLOAT:
	સ્થિતિ VAR_ANY:
	સ્થિતિ VAR_UNKNOWN:
	સ્થિતિ VAR_VOID:
	સ્થિતિ VAR_BOOL:
	સ્થિતિ VAR_SPECIAL:
	સ્થિતિ VAR_INSTR:
	    તોડો;
    }
    vim_free(varp);
}

/*
 * Free the memory for a variable value and set the value to NULL or 0.
 */
    શૂન્ય
clear_tv(typval_T *varp)
{
    જો (varp == ખાલી)
	પાછા;

    પસંદગી (varp->v_type)
    {
	સ્થિતિ VAR_FUNC:
	    func_unref(varp->vval.v_string);
	    // FALLTHROUGH
	સ્થિતિ VAR_STRING:
	    VIM_CLEAR(varp->vval.v_string);
	    તોડો;
	સ્થિતિ VAR_PARTIAL:
	    partial_unref(varp->vval.v_partial);
	    varp->vval.v_partial = ખાલી;
	    તોડો;
	સ્થિતિ VAR_BLOB:
	    blob_unref(varp->vval.v_blob);
	    varp->vval.v_blob = ખાલી;
	    તોડો;
	સ્થિતિ VAR_LIST:
	    list_unref(varp->vval.v_list);
	    varp->vval.v_list = ખાલી;
	    તોડો;
	સ્થિતિ VAR_TUPLE:
	    tuple_unref(varp->vval.v_tuple);
	    varp->vval.v_tuple = ખાલી;
	    તોડો;
	સ્થિતિ VAR_DICT:
	    dict_unref(varp->vval.v_dict);
	    varp->vval.v_dict = ખાલી;
	    તોડો;
	સ્થિતિ VAR_સંખ્યા:
	સ્થિતિ VAR_BOOL:
	સ્થિતિ VAR_SPECIAL:
	    varp->vval.v_સંખ્યા = 0;
	    તોડો;
	સ્થિતિ VAR_FLOAT:
	    varp->vval.v_float = 0.0;
	    તોડો;
	સ્થિતિ VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    job_unref(varp->vval.v_job);
	    varp->vval.v_job = ખાલી;
#endif
	    તોડો;
	સ્થિતિ VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    channel_unref(varp->vval.v_channel);
	    varp->vval.v_channel = ખાલી;
#endif
	    તોડો;
	સ્થિતિ VAR_INSTR:
	    VIM_CLEAR(varp->vval.v_instr);
	    તોડો;
	સ્થિતિ VAR_CLASS:
	    class_unref(varp->vval.v_class);
	    varp->vval.v_class = ખાલી;
	    તોડો;
	સ્થિતિ VAR_OBJECT:
	    object_unref(varp->vval.v_object);
	    varp->vval.v_object = ખાલી;
	    તોડો;
	સ્થિતિ VAR_TYPEALIAS:
	    typealias_unref(varp->vval.v_typealias);
	    varp->vval.v_typealias = ખાલી;
	    તોડો;
	સ્થિતિ VAR_UNKNOWN:
	સ્થિતિ VAR_ANY:
	સ્થિતિ VAR_VOID:
	    તોડો;
    }
    varp->v_lock = 0;
}

/*
 * Set the value of a variable to NULL without freeing items.
 */
    શૂન્ય
init_tv(typval_T *varp)
{
    જો (varp != ખાલી)
	CLEAR_POINTER(varp);
}

    સ્થિર varnumber_T
tv_get_bool_or_સંખ્યા_chk(
	typval_T    *varp,
	પૂર્ણાંક	    *denote,
	પૂર્ણાંક	    want_bool,
	પૂર્ણાંક	    vim9_string_error) // in Vim9 using a string is an error
{
    varnumber_T	n = 0L;

    પસંદગી (varp->v_type)
    {
	સ્થિતિ VAR_સંખ્યા:
	    જો (in_vim9script() && want_bool && varp->vval.v_સંખ્યા != 0
						   && varp->vval.v_સંખ્યા != 1)
	    {
		semsg(_(e_using_સંખ્યા_as_bool_nr), varp->vval.v_સંખ્યા);
		તોડો;
	    }
	    પાછા varp->vval.v_સંખ્યા;
	સ્થિતિ VAR_FLOAT:
	    emsg(_(e_using_float_as_સંખ્યા));
	    તોડો;
	સ્થિતિ VAR_FUNC:
	સ્થિતિ VAR_PARTIAL:
	    emsg(_(e_using_funcref_as_સંખ્યા));
	    તોડો;
	સ્થિતિ VAR_STRING:
	    જો (vim9_string_error && in_vim9script())
	    {
		emsg_using_string_as(varp, !want_bool);
		તોડો;
	    }
	    જો (varp->vval.v_string != ખાલી)
		vim_str2nr(varp->vval.v_string, ખાલી, ખાલી,
					 STR2NR_ALL, &n, ખાલી, 0, FALSE, ખાલી);
	    પાછા n;
	સ્થિતિ VAR_LIST:
	    emsg(_(e_using_list_as_સંખ્યા));
	    તોડો;
	સ્થિતિ VAR_TUPLE:
	    emsg(_(e_using_tuple_as_સંખ્યા));
	    તોડો;
	સ્થિતિ VAR_DICT:
	    emsg(_(e_using_dictionary_as_સંખ્યા));
	    તોડો;
	સ્થિતિ VAR_BOOL:
	સ્થિતિ VAR_SPECIAL:
	    જો (!want_bool && in_vim9script())
	    {
		જો (varp->v_type == VAR_BOOL)
		    emsg(_(e_using_bool_as_સંખ્યા));
		નહીંતર
		    emsg(_(e_using_special_as_સંખ્યા));
		તોડો;
	    }
	    પાછા varp->vval.v_સંખ્યા == VVAL_TRUE ? 1 : 0;
	સ્થિતિ VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    emsg(_(e_using_job_as_સંખ્યા));
	    તોડો;
#endif
	સ્થિતિ VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    emsg(_(e_using_channel_as_સંખ્યા));
	    તોડો;
#endif
	સ્થિતિ VAR_BLOB:
	    emsg(_(e_using_blob_as_સંખ્યા));
	    તોડો;
	સ્થિતિ VAR_CLASS:
	સ્થિતિ VAR_TYPEALIAS:
	    check_typval_is_કિંમત(varp);
	    તોડો;
	સ્થિતિ VAR_OBJECT:
	    {
		જો (varp->vval.v_object == ખાલી)
		    emsg(_(e_using_object_as_string));
		નહીંતર
		{
		    class_T *cl = varp->vval.v_object->obj_class;
		    જો (cl != ખાલી && IS_ENUM(cl))
			semsg(_(e_using_enum_str_as_સંખ્યા), cl->class_name.string);
		    નહીંતર
			emsg(_(e_using_object_as_સંખ્યા));
		}
	    }
	    તોડો;
	સ્થિતિ VAR_VOID:
	    emsg(_(e_cannot_use_void_કિંમત));
	    તોડો;
	સ્થિતિ VAR_UNKNOWN:
	સ્થિતિ VAR_ANY:
	સ્થિતિ VAR_INSTR:
	    internal_error_no_abort("tv_get_number(UNKNOWN)");
	    તોડો;
    }
    જો (denote == ખાલી)		// useful for values that must be unsigned
	n = -1;
    નહીંતર
	*denote = TRUE;
    પાછા n;
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
tv_get_સંખ્યા(typval_T *varp)
{
    પૂર્ણાંક		error = FALSE;

    પાછા tv_get_સંખ્યા_chk(varp, &error);	// return 0L on error
}

/*
 * Like tv_get_number() but in Vim9 script do convert a number in a string to a
 * number without giving an error.
 */
    varnumber_T
tv_to_સંખ્યા(typval_T *varp)
{
    પૂર્ણાંક		error = FALSE;

    પાછા tv_get_bool_or_સંખ્યા_chk(varp, &error, FALSE, FALSE);
}

    varnumber_T
tv_get_સંખ્યા_chk(typval_T *varp, પૂર્ણાંક *denote)
{
    પાછા tv_get_bool_or_સંખ્યા_chk(varp, denote, FALSE, TRUE);
}

/*
 * Get the boolean value of "varp".  This is like tv_get_number_chk(),
 * but in Vim9 script accepts Number (0 and 1) and Bool/Special.
 */
    varnumber_T
tv_get_bool(typval_T *varp)
{
    પાછા tv_get_bool_or_સંખ્યા_chk(varp, ખાલી, TRUE, TRUE);
}

/*
 * Get the boolean value of "varp".  This is like tv_get_number_chk(),
 * but in Vim9 script accepts Number and Bool.
 */
    varnumber_T
tv_get_bool_chk(typval_T *varp, પૂર્ણાંક *denote)
{
    પાછા tv_get_bool_or_સંખ્યા_chk(varp, denote, TRUE, TRUE);
}

    સ્થિર float_T
tv_get_float_chk(typval_T *varp, પૂર્ણાંક *error)
{
    પસંદગી (varp->v_type)
    {
	સ્થિતિ VAR_સંખ્યા:
	    પાછા (float_T)(varp->vval.v_સંખ્યા);
	સ્થિતિ VAR_FLOAT:
	    પાછા varp->vval.v_float;
	સ્થિતિ VAR_FUNC:
	સ્થિતિ VAR_PARTIAL:
	    emsg(_(e_using_funcref_as_float));
	    તોડો;
	સ્થિતિ VAR_STRING:
	    emsg(_(e_using_string_as_float));
	    તોડો;
	સ્થિતિ VAR_LIST:
	    emsg(_(e_using_list_as_float));
	    તોડો;
	સ્થિતિ VAR_TUPLE:
	    emsg(_(e_using_tuple_as_float));
	    તોડો;
	સ્થિતિ VAR_DICT:
	    emsg(_(e_using_dictionary_as_float));
	    તોડો;
	સ્થિતિ VAR_BOOL:
	    emsg(_(e_using_boolean_કિંમત_as_float));
	    તોડો;
	સ્થિતિ VAR_SPECIAL:
	    emsg(_(e_using_special_કિંમત_as_float));
	    તોડો;
	સ્થિતિ VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    emsg(_(e_using_job_as_float));
	    તોડો;
#endif
	સ્થિતિ VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    emsg(_(e_using_channel_as_float));
	    તોડો;
#endif
	સ્થિતિ VAR_BLOB:
	    emsg(_(e_using_blob_as_float));
	    તોડો;
	સ્થિતિ VAR_CLASS:
	સ્થિતિ VAR_TYPEALIAS:
	    check_typval_is_કિંમત(varp);
	    તોડો;
	સ્થિતિ VAR_OBJECT:
	    emsg(_(e_using_object_as_float));
	    તોડો;
	સ્થિતિ VAR_VOID:
	    emsg(_(e_cannot_use_void_કિંમત));
	    તોડો;
	સ્થિતિ VAR_UNKNOWN:
	સ્થિતિ VAR_ANY:
	સ્થિતિ VAR_INSTR:
	    internal_error_no_abort("tv_get_float(UNKNOWN)");
	    તોડો;
    }
    જો (error != ખાલી)
	*error = TRUE;
    પાછા 0;
}

    float_T
tv_get_float(typval_T *varp)
{
    પાછા tv_get_float_chk(varp, ખાલી);
}

/*
 * Give an error and return FAIL unless "args[idx]" is unknown
 */
    પૂર્ણાંક
check_for_unknown_arg(typval_T *args, પૂર્ણાંક idx)
{
    જો (args[idx].v_type != VAR_UNKNOWN)
    {
	semsg(_(e_too_many_arguments), idx + 1);
	પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string.
 */
    પૂર્ણાંક
check_for_string_arg(typval_T *args, પૂર્ણાંક idx)
{
    જો (args[idx].v_type != VAR_STRING)
    {
	semsg(_(e_string_required_for_argument_nr), idx + 1);
	પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a non-empty string.
 */
    પૂર્ણાંક
check_for_nonempty_string_arg(typval_T *args, પૂર્ણાંક idx)
{
    જો (check_for_string_arg(args, idx) == FAIL)
	પાછા FAIL;
    જો (args[idx].vval.v_string == ખાલી || *args[idx].vval.v_string == NUL)
    {
	semsg(_(e_non_empty_string_required_for_argument_nr), idx + 1);
	પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Check for an optional string argument at 'idx'
 */
    પૂર્ણાંક
check_for_opt_string_arg(typval_T *args, પૂર્ણાંક idx)
{
    પાછા (args[idx].v_type == VAR_UNKNOWN
	    || check_for_string_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a number.
 */
    પૂર્ણાંક
check_for_સંખ્યા_arg(typval_T *args, પૂર્ણાંક idx)
{
    જો (args[idx].v_type != VAR_સંખ્યા)
    {
	semsg(_(e_સંખ્યા_required_for_argument_nr), idx + 1);
	પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Check for an optional number argument at 'idx'
 */
    પૂર્ણાંક
check_for_opt_સંખ્યા_arg(typval_T *args, પૂર્ણાંક idx)
{
    પાછા (args[idx].v_type == VAR_UNKNOWN
	    || check_for_સંખ્યા_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a float or a number.
 */
    પૂર્ણાંક
check_for_float_or_nr_arg(typval_T *args, પૂર્ણાંક idx)
{
    જો (args[idx].v_type != VAR_FLOAT && args[idx].v_type != VAR_સંખ્યા)
    {
	semsg(_(e_float_or_સંખ્યા_required_for_argument_nr), idx + 1);
	પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a bool.
 */
    પૂર્ણાંક
check_for_bool_arg(typval_T *args, પૂર્ણાંક idx)
{
    જો (args[idx].v_type != VAR_BOOL
	    && !(args[idx].v_type == VAR_સંખ્યા
		&& (args[idx].vval.v_સંખ્યા == 0
		    || args[idx].vval.v_સંખ્યા == 1)))
    {
	semsg(_(e_bool_required_for_argument_nr), idx + 1);
	પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a bool or a number.
 */
    સ્થિર પૂર્ણાંક
check_for_bool_or_સંખ્યા_arg(typval_T *args, પૂર્ણાંક idx)
{
    જો (args[idx].v_type != VAR_BOOL && args[idx].v_type != VAR_સંખ્યા)
    {
	semsg(_(e_bool_or_સંખ્યા_required_for_argument_nr), idx + 1);
	પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Check for an optional bool argument at 'idx'.
 * Return FAIL if the type is wrong.
 */
    પૂર્ણાંક
check_for_opt_bool_arg(typval_T *args, પૂર્ણાંક idx)
{
    જો (args[idx].v_type == VAR_UNKNOWN)
	પાછા OK;
    પાછા check_for_bool_arg(args, idx);
}

/*
 * Check for an optional bool or number argument at 'idx'.
 * Return FAIL if the type is wrong.
 */
    પૂર્ણાંક
check_for_opt_bool_or_સંખ્યા_arg(typval_T *args, પૂર્ણાંક idx)
{
    જો (args[idx].v_type == VAR_UNKNOWN)
	પાછા OK;
    પાછા check_for_bool_or_સંખ્યા_arg(args, idx);
}

/*
 * Check for an optional bool or dict argument at 'idx'.
 * Return FAIL if the type is wrong.
 */
    પૂર્ણાંક
check_for_opt_bool_or_dict_arg(typval_T *args, પૂર્ણાંક idx)
{
    જો (args[idx].v_type == VAR_UNKNOWN || args[idx].v_type == VAR_DICT)
	પાછા OK;
    પાછા check_for_bool_arg(args, idx);
}

/*
 * Give an error and return FAIL unless "args[idx]" is a blob.
 */
    પૂર્ણાંક
check_for_blob_arg(typval_T *args, પૂર્ણાંક idx)
{
    જો (args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_blob_required_for_argument_nr), idx + 1);
	પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list.
 */
    પૂર્ણાંક
check_for_list_arg(typval_T *args, પૂર્ણાંક idx)
{
    જો (args[idx].v_type != VAR_LIST)
    {
	semsg(_(e_list_required_for_argument_nr), idx + 1);
	પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a non-NULL list.
 */
    પૂર્ણાંક
check_for_nonnull_list_arg(typval_T *args, પૂર્ણાંક idx)
{
    જો (check_for_list_arg(args, idx) == FAIL)
	પાછા FAIL;

    જો (args[idx].vval.v_list == ખાલી)
    {
	semsg(_(e_non_null_list_required_for_argument_nr), idx + 1);
	પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Check for an optional list argument at 'idx'
 */
    પૂર્ણાંક
check_for_opt_list_arg(typval_T *args, પૂર્ણાંક idx)
{
    પાછા (args[idx].v_type == VAR_UNKNOWN
	    || check_for_list_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a tuple.
 */
    પૂર્ણાંક
check_for_tuple_arg(typval_T *args, પૂર્ણાંક idx)
{
    જો (args[idx].v_type != VAR_TUPLE)
    {
	semsg(_(e_tuple_required_for_argument_nr), idx + 1);
	પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a dict.
 */
    પૂર્ણાંક
check_for_dict_arg(typval_T *args, પૂર્ણાંક idx)
{
    જો (args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_dict_required_for_argument_nr), idx + 1);
	પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a non-NULL dict.
 */
    પૂર્ણાંક
check_for_nonnull_dict_arg(typval_T *args, પૂર્ણાંક idx)
{
    જો (check_for_dict_arg(args, idx) == FAIL)
	પાછા FAIL;

    જો (args[idx].vval.v_dict == ખાલી)
    {
	semsg(_(e_non_null_dict_required_for_argument_nr), idx + 1);
	પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Check for an optional dict argument at 'idx'
 */
    પૂર્ણાંક
check_for_opt_dict_arg(typval_T *args, પૂર્ણાંક idx)
{
    પાછા (args[idx].v_type == VAR_UNKNOWN
	    || check_for_dict_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Check for an optional non-NULL dict argument at 'idx'
 */
    પૂર્ણાંક
check_for_opt_nonnull_dict_arg(typval_T *args, પૂર્ણાંક idx)
{
    પાછા (args[idx].v_type == VAR_UNKNOWN
	    || check_for_nonnull_dict_arg(args, idx) != FAIL) ? OK : FAIL;
}

#જો defined(FEAT_JOB_CHANNEL)
/*
 * Give an error and return FAIL unless "args[idx]" is a channel or a job.
 */
    પૂર્ણાંક
check_for_chan_or_job_arg(typval_T *args, પૂર્ણાંક idx)
{
    જો (args[idx].v_type != VAR_CHANNEL && args[idx].v_type != VAR_JOB)
    {
	semsg(_(e_chan_or_job_required_for_argument_nr), idx + 1);
	પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an optional channel or a
 * job.
 */
    પૂર્ણાંક
check_for_opt_chan_or_job_arg(typval_T *args, પૂર્ણાંક idx)
{
    પાછા (args[idx].v_type == VAR_UNKNOWN
	    || check_for_chan_or_job_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a job.
 */
    પૂર્ણાંક
check_for_job_arg(typval_T *args, પૂર્ણાંક idx)
{
    જો (args[idx].v_type != VAR_JOB)
    {
	semsg(_(e_job_required_for_argument_nr), idx + 1);
	પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Check for an optional job argument at 'idx'.
 */
    પૂર્ણાંક
check_for_opt_job_arg(typval_T *args, પૂર્ણાંક idx)
{
    પાછા (args[idx].v_type == VAR_UNKNOWN
	    || check_for_job_arg(args, idx) != FAIL) ? OK : FAIL;
}
#નહીંતર
/*
 * Give an error and return FAIL unless "args[idx]" is an optional channel or a
 * job.  Used without the +channel feature, thus only VAR_UNKNOWN is accepted.
 */
    પૂર્ણાંક
check_for_opt_chan_or_job_arg(typval_T *args, પૂર્ણાંક idx)
{
    પાછા args[idx].v_type == VAR_UNKNOWN ? OK : FAIL;
}
#endif

/*
 * Give an error and return FAIL unless "args[idx]" is a string or
 * a number.
 */
    પૂર્ણાંક
check_for_string_or_સંખ્યા_arg(typval_T *args, પૂર્ણાંક idx)
{
    જો (args[idx].v_type != VAR_STRING && args[idx].v_type != VAR_સંખ્યા)
    {
	semsg(_(e_string_or_સંખ્યા_required_for_argument_nr), idx + 1);
	પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Check for an optional string or number argument at 'idx'.
 */
    પૂર્ણાંક
check_for_opt_string_or_સંખ્યા_arg(typval_T *args, પૂર્ણાંક idx)
{
    પાછા (args[idx].v_type == VAR_UNKNOWN
	    || check_for_string_or_સંખ્યા_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a buffer number.
 * Buffer number can be a number or a string.
 */
    પૂર્ણાંક
check_for_buffer_arg(typval_T *args, પૂર્ણાંક idx)
{
    પાછા check_for_string_or_સંખ્યા_arg(args, idx);
}

/*
 * Check for an optional buffer argument at 'idx'
 */
    પૂર્ણાંક
check_for_opt_buffer_arg(typval_T *args, પૂર્ણાંક idx)
{
    પાછા (args[idx].v_type == VAR_UNKNOWN
	    || check_for_buffer_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a line number.
 * Line number can be a number or a string.
 */
    પૂર્ણાંક
check_for_lnum_arg(typval_T *args, પૂર્ણાંક idx)
{
    પાછા check_for_string_or_સંખ્યા_arg(args, idx);
}

/*
 * Check for an optional line number argument at 'idx'
 */
    પૂર્ણાંક
check_for_opt_lnum_arg(typval_T *args, પૂર્ણાંક idx)
{
    પાછા (args[idx].v_type == VAR_UNKNOWN
	    || check_for_lnum_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a blob.
 */
    પૂર્ણાંક
check_for_string_or_blob_arg(typval_T *args, પૂર્ણાંક idx)
{
    જો (args[idx].v_type != VAR_STRING && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_string_or_blob_required_for_argument_nr), idx + 1);
	પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a list.
 */
    પૂર્ણાંક
check_for_string_or_list_arg(typval_T *args, પૂર્ણાંક idx)
{
    જો (args[idx].v_type != VAR_STRING && args[idx].v_type != VAR_LIST)
    {
	semsg(_(e_string_or_list_required_for_argument_nr), idx + 1);
	પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string, a list, a
 * tuple or a blob.
 */
    પૂર્ણાંક
check_for_string_or_list_or_tuple_or_blob_arg(typval_T *args, પૂર્ણાંક idx)
{
    જો (args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_string_list_tuple_or_blob_required_for_argument_nr), idx + 1);
	પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Check for an optional string or list argument at 'idx'
 */
    પૂર્ણાંક
check_for_opt_string_or_list_arg(typval_T *args, પૂર્ણાંક idx)
{
    પાછા (args[idx].v_type == VAR_UNKNOWN
	    || check_for_string_or_list_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a dict.
 */
    પૂર્ણાંક
check_for_string_or_dict_arg(typval_T *args, પૂર્ણાંક idx)
{
    જો (args[idx].v_type != VAR_STRING && args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_string_or_dict_required_for_argument_nr), idx + 1);
	પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a number
 * or a list.
 */
    પૂર્ણાંક
check_for_string_or_સંખ્યા_or_list_arg(typval_T *args, પૂર્ણાંક idx)
{
    જો (args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_સંખ્યા
	    && args[idx].v_type != VAR_LIST)
    {
	semsg(_(e_string_સંખ્યા_or_list_required_for_argument_nr), idx + 1);
	પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an optional string
 * or number or a list
 */
    પૂર્ણાંક
check_for_opt_string_or_સંખ્યા_or_list_arg(typval_T *args, પૂર્ણાંક idx)
{
    પાછા (args[idx].v_type == VAR_UNKNOWN
	    || check_for_string_or_સંખ્યા_or_list_arg(args, idx)
							!= FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string, a number, a
 * list, a tuple or a blob.
 */
    પૂર્ણાંક
check_for_repeat_func_arg(typval_T *args, પૂર્ણાંક idx)
{
    જો (args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_સંખ્યા
	    && args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_repeatable_type_required_for_argument_nr), idx + 1);
	પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string, a list, a
 * tuple or a dict.
 */
    પૂર્ણાંક
check_for_string_list_tuple_or_dict_arg(typval_T *args, પૂર્ણાંક idx)
{
    જો (args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_string_list_tuple_or_dict_required_for_argument_nr), idx + 1);
	પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string
 * or a function reference.
 */
    પૂર્ણાંક
check_for_string_or_func_arg(typval_T *args, પૂર્ણાંક idx)
{
    જો (args[idx].v_type != VAR_PARTIAL
	    && args[idx].v_type != VAR_FUNC
	    && args[idx].v_type != VAR_STRING)
    {
	semsg(_(e_string_or_function_required_for_argument_nr), idx + 1);
	પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list or a blob.
 */
    પૂર્ણાંક
check_for_list_or_blob_arg(typval_T *args, પૂર્ણાંક idx)
{
    જો (args[idx].v_type != VAR_LIST && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_list_or_blob_required_for_argument_nr), idx + 1);
	પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list or a tuple.
 */
    પૂર્ણાંક
check_for_list_or_tuple_arg(typval_T *args, પૂર્ણાંક idx)
{
    જો (args[idx].v_type != VAR_LIST && args[idx].v_type != VAR_TUPLE)
    {
	semsg(_(e_list_or_tuple_required_for_argument_nr), idx + 1);
	પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list, a tuple or a
 * blob.
 */
    પૂર્ણાંક
check_for_list_or_tuple_or_blob_arg(typval_T *args, પૂર્ણાંક idx)
{
    જો (args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_list_or_tuple_or_blob_required_for_argument_nr), idx + 1);
	પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list, a tuple or a
 * dict
 */
    પૂર્ણાંક
check_for_list_or_tuple_or_dict_arg(typval_T *args, પૂર્ણાંક idx)
{
    જો (args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_list_or_tuple_or_dict_required_for_argument_nr), idx + 1);
	પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list or dict or a
 * blob.
 */
    પૂર્ણાંક
check_for_list_or_dict_or_blob_arg(typval_T *args, પૂર્ણાંક idx)
{
    જો (args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_DICT
	    && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_list_dict_or_blob_required_for_argument_nr), idx + 1);
	પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list, a tuple, a dict,
 * a blob or a string.
 */
    પૂર્ણાંક
check_for_list_tuple_dict_blob_or_string_arg(typval_T *args, પૂર્ણાંક idx)
{
    જો (args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_DICT
	    && args[idx].v_type != VAR_BLOB
	    && args[idx].v_type != VAR_STRING)
    {
	semsg(_(e_list_tuple_dict_blob_or_string_required_for_argument_nr), idx + 1);
	પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an optional buffer
 * number or a dict.
 */
    પૂર્ણાંક
check_for_opt_buffer_or_dict_arg(typval_T *args, પૂર્ણાંક idx)
{
    જો (args[idx].v_type != VAR_UNKNOWN
	    && args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_સંખ્યા
	    && args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_string_required_for_argument_nr), idx + 1);
	પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an object.
 */
    પૂર્ણાંક
check_for_object_arg(typval_T *args, પૂર્ણાંક idx)
{
    જો (args[idx].v_type != VAR_OBJECT)
    {
	semsg(_(e_object_required_for_argument_nr), idx + 1);
	પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Returns TRUE if "tv" is a type alias for a class
 */
    સ્થિર પૂર્ણાંક
tv_class_alias(typval_T *tv)
{
    પાછા tv->v_type == VAR_TYPEALIAS &&
			tv->vval.v_typealias->ta_type->tt_type == VAR_OBJECT;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a class
 * or class typealias.
 */
    પૂર્ણાંક
check_for_class_or_typealias_args(typval_T *args, પૂર્ણાંક idx)
{
    માટે (પૂર્ણાંક i = idx; args[i].v_type != VAR_UNKNOWN; ++i)
    {
	જો (args[i].v_type != VAR_CLASS && !tv_class_alias(&args[idx]))
	{
	    semsg(_(e_class_or_typealias_required_for_argument_nr), i + 1);
	    પાછા FAIL;
	}
    }
    પાછા OK;
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
    સ્થિર char_u   mybuf[NUMBUFLEN];

    પાછા tv_get_string_buf(varp, mybuf);
}

/*
 * Like tv_get_string() but don't allow number to string conversion for Vim9.
 */
    char_u *
tv_get_string_strict(typval_T *varp)
{
    સ્થિર char_u   mybuf[NUMBUFLEN];
    char_u	    *res =  tv_get_string_buf_chk_strict(
						 varp, mybuf, in_vim9script());

    પાછા res != ખાલી ? res : (char_u *)"";
}

    char_u *
tv_get_string_buf(typval_T *varp, char_u *buf)
{
    char_u	*res = tv_get_string_buf_chk(varp, buf);

    પાછા res != ખાલી ? res : (char_u *)"";
}

/*
 * Careful: This uses a single, static buffer.  YOU CAN ONLY USE IT ONCE!
 */
    char_u *
tv_get_string_chk(typval_T *varp)
{
    સ્થિર char_u   mybuf[NUMBUFLEN];

    પાછા tv_get_string_buf_chk(varp, mybuf);
}

    char_u *
tv_get_string_buf_chk(typval_T *varp, char_u *buf)
{
    પાછા tv_get_string_buf_chk_strict(varp, buf, FALSE);
}

    char_u *
tv_get_string_buf_chk_strict(typval_T *varp, char_u *buf, પૂર્ણાંક strict)
{
    પસંદગી (varp->v_type)
    {
	સ્થિતિ VAR_સંખ્યા:
	    જો (strict)
	    {
		emsg(_(e_using_સંખ્યા_as_string));
		તોડો;
	    }
	    vim_snprintf((અક્ષર *)buf, NUMBUFLEN, "%lld",
					    (varnumber_T)varp->vval.v_સંખ્યા);
	    પાછા buf;
	સ્થિતિ VAR_FUNC:
	સ્થિતિ VAR_PARTIAL:
	    emsg(_(e_using_funcref_as_string));
	    તોડો;
	સ્થિતિ VAR_LIST:
	    emsg(_(e_using_list_as_string));
	    તોડો;
	સ્થિતિ VAR_TUPLE:
	    emsg(_(e_using_tuple_as_string));
	    તોડો;
	સ્થિતિ VAR_DICT:
	    emsg(_(e_using_dictionary_as_string));
	    તોડો;
	સ્થિતિ VAR_FLOAT:
	    જો (strict)
	    {
		emsg(_(e_using_float_as_string));
		તોડો;
	    }
	    vim_snprintf((અક્ષર *)buf, NUMBUFLEN, "%g", varp->vval.v_float);
	    પાછા buf;
	સ્થિતિ VAR_STRING:
	    જો (varp->vval.v_string != ખાલી)
		પાછા varp->vval.v_string;
	    પાછા (char_u *)"";
	સ્થિતિ VAR_BOOL:
	સ્થિતિ VAR_SPECIAL:
	    STRCPY(buf, get_var_special_name(varp->vval.v_સંખ્યા));
	    પાછા buf;
	સ્થિતિ VAR_BLOB:
	    emsg(_(e_using_blob_as_string));
	    તોડો;
	સ્થિતિ VAR_CLASS:
	સ્થિતિ VAR_TYPEALIAS:
	    check_typval_is_કિંમત(varp);
	    તોડો;
	સ્થિતિ VAR_OBJECT:
	    {
		જો (varp->vval.v_object == ખાલી)
		    emsg(_(e_using_object_as_string));
		નહીંતર
		{
		    class_T *cl = varp->vval.v_object->obj_class;
		    જો (cl != ખાલી && IS_ENUM(cl))
			semsg(_(e_using_enum_str_as_string), cl->class_name.string);
		    નહીંતર
			emsg(_(e_using_object_as_string));
		}
	    }
	    તોડો;
	સ્થિતિ VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    જો (in_vim9script())
	    {
		semsg(_(e_using_invalid_કિંમત_as_string_str), "job");
		તોડો;
	    }
	    પાછા job_to_string_buf(varp, buf);
#endif
	    તોડો;
	સ્થિતિ VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    જો (in_vim9script())
	    {
		semsg(_(e_using_invalid_કિંમત_as_string_str), "channel");
		તોડો;
	    }
	    પાછા channel_to_string_buf(varp, buf);
#endif
	    તોડો;
	સ્થિતિ VAR_VOID:
	    emsg(_(e_cannot_use_void_કિંમત));
	    તોડો;
	સ્થિતિ VAR_UNKNOWN:
	સ્થિતિ VAR_ANY:
	સ્થિતિ VAR_INSTR:
	    semsg(_(e_using_invalid_કિંમત_as_string_str),
						  vartype_name(varp->v_type));
	    તોડો;
    }
    પાછા ખાલી;
}

/*
 * Turn a typeval into a string.  Similar to tv_get_string_buf() but uses
 * string() on Dict, List, etc.
 */
    char_u *
tv_stringify(typval_T *varp, char_u *buf)
{
    જો (varp->v_type == VAR_LIST
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
	પાછા tmp.vval.v_string;
    }
    પાછા tv_get_string_buf(varp, buf);
}

/*
 * Return TRUE if typeval "tv" and its value are set to be locked (immutable).
 * Also give an error message, using "name" or _("name") when use_gettext is
 * TRUE.
 */
    પૂર્ણાંક
tv_check_lock(typval_T *tv, char_u *name, પૂર્ણાંક use_gettext)
{
    પૂર્ણાંક	lock = 0;

    પસંદગી (tv->v_type)
    {
	સ્થિતિ VAR_BLOB:
	    જો (tv->vval.v_blob != ખાલી)
		lock = tv->vval.v_blob->bv_lock;
	    તોડો;
	સ્થિતિ VAR_LIST:
	    જો (tv->vval.v_list != ખાલી)
		lock = tv->vval.v_list->lv_lock;
	    તોડો;
	સ્થિતિ VAR_TUPLE:
	    જો (tv->vval.v_tuple != ખાલી)
		lock = tv->vval.v_tuple->tv_lock;
	    તોડો;
	સ્થિતિ VAR_DICT:
	    જો (tv->vval.v_dict != ખાલી)
		lock = tv->vval.v_dict->dv_lock;
	    તોડો;
	મૂળભૂત:
	    તોડો;
    }
    પાછા કિંમત_check_lock(tv->v_lock, name, use_gettext)
		   || (lock != 0 && કિંમત_check_lock(lock, name, use_gettext));
}

/*
 * Copy the values from typval_T "from" to typval_T "to".
 * When needed allocates string or increases reference count.
 * Does not make a copy of a list, blob or dict but copies the reference!
 * It is OK for "from" and "to" to point to the same item.  This is used to
 * make a copy later.
 */
    પૂર્ણાંક
copy_tv(typval_T *from, typval_T *to)
{
    પૂર્ણાંક		ret = OK;

    to->v_type = from->v_type;
    to->v_lock = 0;
    પસંદગી (from->v_type)
    {
	સ્થિતિ VAR_સંખ્યા:
	સ્થિતિ VAR_BOOL:
	સ્થિતિ VAR_SPECIAL:
	    to->vval.v_સંખ્યા = from->vval.v_સંખ્યા;
	    તોડો;
	સ્થિતિ VAR_FLOAT:
	    to->vval.v_float = from->vval.v_float;
	    તોડો;
	સ્થિતિ VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    to->vval.v_job = from->vval.v_job;
	    જો (to->vval.v_job != ખાલી)
		++to->vval.v_job->jv_refcount;
	    તોડો;
#endif
	સ્થિતિ VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    to->vval.v_channel = from->vval.v_channel;
	    જો (to->vval.v_channel != ખાલી)
		++to->vval.v_channel->ch_refcount;
	    તોડો;
#endif
	સ્થિતિ VAR_INSTR:
	    to->vval.v_instr = from->vval.v_instr;
	    તોડો;

	સ્થિતિ VAR_CLASS:
	    copy_class(from, to);
	    તોડો;

	સ્થિતિ VAR_OBJECT:
	    copy_object(from, to);
	    તોડો;

	સ્થિતિ VAR_STRING:
	સ્થિતિ VAR_FUNC:
	    જો (from->vval.v_string == ખાલી)
		to->vval.v_string = ખાલી;
	    નહીંતર
	    {
		to->vval.v_string = vim_strsave(from->vval.v_string);
		જો (from->v_type == VAR_FUNC)
		    func_ref(to->vval.v_string);
	    }
	    તોડો;
	સ્થિતિ VAR_PARTIAL:
	    જો (from->vval.v_partial == ખાલી)
		to->vval.v_partial = ખાલી;
	    નહીંતર
	    {
		to->vval.v_partial = from->vval.v_partial;
		++to->vval.v_partial->pt_refcount;
	    }
	    તોડો;
	સ્થિતિ VAR_BLOB:
	    જો (from->vval.v_blob == ખાલી)
		to->vval.v_blob = ખાલી;
	    નહીંતર
	    {
		to->vval.v_blob = from->vval.v_blob;
		++to->vval.v_blob->bv_refcount;
	    }
	    તોડો;
	સ્થિતિ VAR_LIST:
	    જો (from->vval.v_list == ખાલી)
		to->vval.v_list = ખાલી;
	    નહીંતર
	    {
		to->vval.v_list = from->vval.v_list;
		++to->vval.v_list->lv_refcount;
	    }
	    તોડો;
	સ્થિતિ VAR_TUPLE:
	    જો (from->vval.v_tuple == ખાલી)
		to->vval.v_tuple = ખાલી;
	    નહીંતર
	    {
		to->vval.v_tuple = from->vval.v_tuple;
		++to->vval.v_tuple->tv_refcount;
	    }
	    તોડો;
	સ્થિતિ VAR_DICT:
	    જો (from->vval.v_dict == ખાલી)
		to->vval.v_dict = ખાલી;
	    નહીંતર
	    {
		to->vval.v_dict = from->vval.v_dict;
		++to->vval.v_dict->dv_refcount;
	    }
	    તોડો;
	સ્થિતિ VAR_TYPEALIAS:
	    જો (from->vval.v_typealias == ખાલી)
		to->vval.v_typealias = ખાલી;
	    નહીંતર
	    {
		to->vval.v_typealias = from->vval.v_typealias;
		++to->vval.v_typealias->ta_refcount;
	    }
	    તોડો;
	સ્થિતિ VAR_VOID:
	    emsg(_(e_cannot_use_void_કિંમત));
	    ret = FAIL;
	    તોડો;
	સ્થિતિ VAR_UNKNOWN:
	સ્થિતિ VAR_ANY:
	    internal_error_no_abort("copy_tv(UNKNOWN)");
	    ret = FAIL;
	    તોડો;
    }

    પાછા ret;
}

/*
 * Compare "tv1" and "tv2".
 */
    પૂર્ણાંક
typval_compare2(
    typval_T	*tv1,	// first operand
    typval_T	*tv2,	// second operand
    exprtype_T	type,	// operator
    પૂર્ણાંક		ic,	// ignore case
    પૂર્ણાંક		*res)	// comparison result
{
    varnumber_T	n1, n2;
    પૂર્ણાંક		type_is = type == પદાવલિ_IS || type == પદાવલિ_ISNOT;

    જો (check_typval_is_કિંમત(tv1) == FAIL
	|| check_typval_is_કિંમત(tv2) == FAIL)
	પાછા FAIL;
    નહીંતર જો (type_is && tv1->v_type != tv2->v_type)
    {
	// For "is" a different type always means FALSE, for "isnot"
	// it means TRUE.
	*res = (type == પદાવલિ_ISNOT);
    }
    નહીંતર જો (((tv1->v_type == VAR_SPECIAL && tv1->vval.v_સંખ્યા == VVAL_NULL)
		|| (tv2->v_type == VAR_SPECIAL
					   && tv2->vval.v_સંખ્યા == VVAL_NULL))
	    && tv1->v_type != tv2->v_type
	    && (type == પદાવલિ_EQUAL || type == પદાવલિ_NEQUAL))
    {
	n1 = typval_compare_null(tv1, tv2);
	જો (n1 == MAYBE)
	    પાછા FAIL;
	જો (type == પદાવલિ_NEQUAL)
	    n1 = !n1;
	*res = n1;
    }
    નહીંતર જો (tv1->v_type == VAR_BLOB || tv2->v_type == VAR_BLOB)
    {
	જો (typval_compare_blob(tv1, tv2, type, res) == FAIL)
	    પાછા FAIL;
    }
    નહીંતર જો (tv1->v_type == VAR_LIST || tv2->v_type == VAR_LIST)
    {
	જો (typval_compare_list(tv1, tv2, type, ic, res) == FAIL)
	    પાછા FAIL;
    }
    નહીંતર જો (tv1->v_type == VAR_TUPLE || tv2->v_type == VAR_TUPLE)
    {
	જો (typval_compare_tuple(tv1, tv2, type, ic, res) == FAIL)
	    પાછા FAIL;
    }
    નહીંતર જો (tv1->v_type == VAR_OBJECT || tv2->v_type == VAR_OBJECT)
    {
	જો (typval_compare_object(tv1, tv2, type, ic, res) == FAIL)
	    પાછા FAIL;
    }
    નહીંતર જો (tv1->v_type == VAR_DICT || tv2->v_type == VAR_DICT)
    {
	જો (typval_compare_dict(tv1, tv2, type, ic, res) == FAIL)
	    પાછા FAIL;
    }
    નહીંતર જો (tv1->v_type == VAR_FUNC || tv2->v_type == VAR_FUNC
	|| tv1->v_type == VAR_PARTIAL || tv2->v_type == VAR_PARTIAL)
    {
	જો (typval_compare_func(tv1, tv2, type, ic, res) == FAIL)
	    પાછા FAIL;
    }

    // If one of the two variables is a float, compare as a float.
    // When using "=~" or "!~", always compare as string.
    નહીંતર જો ((tv1->v_type == VAR_FLOAT || tv2->v_type == VAR_FLOAT)
	    && type != પદાવલિ_MATCH && type != પદાવલિ_NOMATCH)
    {
	float_T f1, f2;
	પૂર્ણાંક	error = FALSE;

	f1 = tv_get_float_chk(tv1, &error);
	જો (!error)
	    f2 = tv_get_float_chk(tv2, &error);
	જો (error)
	    પાછા FAIL;
	n1 = FALSE;
	પસંદગી (type)
	{
	    સ્થિતિ પદાવલિ_IS:
	    સ્થિતિ પદાવલિ_EQUAL:    n1 = (f1 == f2); તોડો;
	    સ્થિતિ પદાવલિ_ISNOT:
	    સ્થિતિ પદાવલિ_NEQUAL:   n1 = (f1 != f2); તોડો;
	    સ્થિતિ પદાવલિ_GREATER:  n1 = (f1 > f2); તોડો;
	    સ્થિતિ પદાવલિ_GEQUAL:   n1 = (f1 >= f2); તોડો;
	    સ્થિતિ પદાવલિ_SMALLER:  n1 = (f1 < f2); તોડો;
	    સ્થિતિ પદાવલિ_SEQUAL:   n1 = (f1 <= f2); તોડો;
	    સ્થિતિ પદાવલિ_UNKNOWN:
	    સ્થિતિ પદાવલિ_MATCH:
	    મૂળભૂત:  તોડો;  // avoid gcc warning
	}
	*res = n1;
    }

    // If one of the two variables is a number, compare as a number.
    // When using "=~" or "!~", always compare as string.
    નહીંતર જો ((tv1->v_type == VAR_સંખ્યા || tv2->v_type == VAR_સંખ્યા)
	    && type != પદાવલિ_MATCH && type != પદાવલિ_NOMATCH)
    {
	પૂર્ણાંક error = FALSE;

	n1 = tv_get_સંખ્યા_chk(tv1, &error);
	જો (!error)
	    n2 = tv_get_સંખ્યા_chk(tv2, &error);
	જો (error)
	    પાછા FAIL;
	પસંદગી (type)
	{
	    સ્થિતિ પદાવલિ_IS:
	    સ્થિતિ પદાવલિ_EQUAL:    n1 = (n1 == n2); તોડો;
	    સ્થિતિ પદાવલિ_ISNOT:
	    સ્થિતિ પદાવલિ_NEQUAL:   n1 = (n1 != n2); તોડો;
	    સ્થિતિ પદાવલિ_GREATER:  n1 = (n1 > n2); તોડો;
	    સ્થિતિ પદાવલિ_GEQUAL:   n1 = (n1 >= n2); તોડો;
	    સ્થિતિ પદાવલિ_SMALLER:  n1 = (n1 < n2); તોડો;
	    સ્થિતિ પદાવલિ_SEQUAL:   n1 = (n1 <= n2); તોડો;
	    સ્થિતિ પદાવલિ_UNKNOWN:
	    સ્થિતિ પદાવલિ_MATCH:
	    મૂળભૂત:  તોડો;  // avoid gcc warning
	}
	*res = n1;
    }
    નહીંતર જો (in_vim9script() && (tv1->v_type == VAR_BOOL
				    || tv2->v_type == VAR_BOOL
				    || (tv1->v_type == VAR_SPECIAL
					      && tv2->v_type == VAR_SPECIAL)))
    {
	જો (tv1->v_type != tv2->v_type)
	{
	    semsg(_(e_cannot_compare_str_with_str),
		       vartype_name(tv1->v_type), vartype_name(tv2->v_type));
	    પાછા FAIL;
	}
	n1 = tv1->vval.v_સંખ્યા;
	n2 = tv2->vval.v_સંખ્યા;
	પસંદગી (type)
	{
	    સ્થિતિ પદાવલિ_IS:
	    સ્થિતિ પદાવલિ_EQUAL:    n1 = (n1 == n2); તોડો;
	    સ્થિતિ પદાવલિ_ISNOT:
	    સ્થિતિ પદાવલિ_NEQUAL:   n1 = (n1 != n2); તોડો;
	    મૂળભૂત:
		semsg(_(e_invalid_operation_for_str),
						   vartype_name(tv1->v_type));
		પાછા FAIL;
	}
	*res = n1;
    }
#ifdef FEAT_JOB_CHANNEL
    નહીંતર જો (tv1->v_type == tv2->v_type
	    && (tv1->v_type == VAR_CHANNEL || tv1->v_type == VAR_JOB)
	    && (type == પદાવલિ_NEQUAL || type == પદાવલિ_EQUAL))
    {
	જો (tv1->v_type == VAR_CHANNEL)
	    n1 = tv1->vval.v_channel == tv2->vval.v_channel;
	નહીંતર
	    n1 = tv1->vval.v_job == tv2->vval.v_job;
	જો (type == પદાવલિ_NEQUAL)
	    n1 = !n1;
	*res = n1;
    }
#endif
    નહીંતર
    {
	જો (typval_compare_string(tv1, tv2, type, ic, res) == FAIL)
	    પાછા FAIL;
    }

    પાછા OK;
}

/*
 * Compare "tv1" and "tv2".
 * Put the result in "tv1".  Caller should clear "tv2".
 */
    પૂર્ણાંક
typval_compare(
    typval_T	*tv1,	// first operand
    typval_T	*tv2,	// second operand
    exprtype_T	type,   // operator
    પૂર્ણાંક		ic)     // ignore case
{
    પૂર્ણાંક res;

    જો (typval_compare2(tv1, tv2, type, ic, &res) == FAIL)
    {
	clear_tv(tv1);
	પાછા FAIL;
    }

    clear_tv(tv1);
    જો (in_vim9script())
    {
	tv1->v_type = VAR_BOOL;
	tv1->vval.v_સંખ્યા = res ? VVAL_TRUE : VVAL_FALSE;
    }
    નહીંતર
    {
	tv1->v_type = VAR_સંખ્યા;
	tv1->vval.v_સંખ્યા = res;
    }

    પાછા OK;
}

/*
 * Compare "tv1" to "tv2" as lists according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    પૂર્ણાંક
typval_compare_list(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	પૂર્ણાંક	    ic,
	પૂર્ણાંક	    *res)
{
    પૂર્ણાંક	    val = 0;

    જો (type == પદાવલિ_IS || type == પદાવલિ_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
				      && tv1->vval.v_list == tv2->vval.v_list);
	જો (type == પદાવલિ_ISNOT)
	    val = !val;
    }
    નહીંતર જો (tv1->v_type != tv2->v_type
	    || (type != પદાવલિ_EQUAL && type != પદાવલિ_NEQUAL))
    {
	જો (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_list_with_list));
	નહીંતર
	    emsg(_(e_invalid_operation_for_list));
	પાછા FAIL;
    }
    નહીંતર
    {
	val = list_equal(tv1->vval.v_list, tv2->vval.v_list, ic);
	જો (type == પદાવલિ_NEQUAL)
	    val = !val;
    }
    *res = val;
    પાછા OK;
}

/*
 * Compare "tv1" to "tv2" as tuples according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    પૂર્ણાંક
typval_compare_tuple(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	પૂર્ણાંક	    ic,
	પૂર્ણાંક	    *res)
{
    પૂર્ણાંક	    val = 0;

    જો (type == પદાવલિ_IS || type == પદાવલિ_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
			      && tv1->vval.v_tuple == tv2->vval.v_tuple);
	જો (type == પદાવલિ_ISNOT)
	    val = !val;
    }
    નહીંતર જો (tv1->v_type != tv2->v_type
	    || (type != પદાવલિ_EQUAL && type != પદાવલિ_NEQUAL))
    {
	જો (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_tuple_with_tuple));
	નહીંતર
	    emsg(_(e_invalid_operation_for_tuple));
	પાછા FAIL;
    }
    નહીંતર
    {
	val = tuple_equal(tv1->vval.v_tuple, tv2->vval.v_tuple, ic);
	જો (type == પદાવલિ_NEQUAL)
	    val = !val;
    }
    *res = val;
    પાછા OK;
}

/*
 * Compare v:null with another type.  Return TRUE if the value is NULL.
 */
    પૂર્ણાંક
typval_compare_null(typval_T *tv1, typval_T *tv2)
{
    જો ((tv1->v_type == VAR_SPECIAL && tv1->vval.v_સંખ્યા == VVAL_NULL)
	    || (tv2->v_type == VAR_SPECIAL && tv2->vval.v_સંખ્યા == VVAL_NULL))
    {
	typval_T	*tv = tv1->v_type == VAR_SPECIAL ? tv2 : tv1;

	પસંદગી (tv->v_type)
	{
	    સ્થિતિ VAR_BLOB: પાછા tv->vval.v_blob == ખાલી;
#ifdef FEAT_JOB_CHANNEL
	    સ્થિતિ VAR_CHANNEL: પાછા tv->vval.v_channel == ખાલી;
#endif
	    // TODO: null_class handling
	    // case VAR_CLASS: return tv->vval.v_class == NULL;
	    સ્થિતિ VAR_DICT: પાછા tv->vval.v_dict == ખાલી;
	    સ્થિતિ VAR_FUNC: પાછા tv->vval.v_string == ખાલી;
#ifdef FEAT_JOB_CHANNEL
	    સ્થિતિ VAR_JOB: પાછા tv->vval.v_job == ખાલી;
#endif
	    સ્થિતિ VAR_LIST: પાછા tv->vval.v_list == ખાલી;
	    સ્થિતિ VAR_TUPLE: પાછા tv->vval.v_tuple == ખાલી;
	    સ્થિતિ VAR_OBJECT: પાછા tv->vval.v_object == ખાલી;
	    સ્થિતિ VAR_PARTIAL: પાછા tv->vval.v_partial == ખાલી;
	    સ્થિતિ VAR_STRING: પાછા tv->vval.v_string == ખાલી;

	    સ્થિતિ VAR_સંખ્યા: જો (!in_vim9script())
				 પાછા tv->vval.v_સંખ્યા == 0;
			     તોડો;
	    સ્થિતિ VAR_FLOAT: જો (!in_vim9script())
				 પાછા tv->vval.v_float == 0.0;
			     તોડો;
	    સ્થિતિ VAR_TYPEALIAS: પાછા tv->vval.v_typealias == ખાલી;
	    મૂળભૂત: તોડો;
	}
    }
    // although comparing null with number, float or bool is not very useful
    // we won't give an error
    પાછા FALSE;
}

/*
 * Compare "tv1" to "tv2" as blobs according to "type".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    પૂર્ણાંક
typval_compare_blob(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	પૂર્ણાંક	    *res)
{
    પૂર્ણાંક	    val = 0;

    જો (type == પદાવલિ_IS || type == પદાવલિ_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
			&& tv1->vval.v_blob == tv2->vval.v_blob);
	જો (type == પદાવલિ_ISNOT)
	    val = !val;
    }
    નહીંતર જો (tv1->v_type != tv2->v_type
	    || (type != પદાવલિ_EQUAL && type != પદાવલિ_NEQUAL))
    {
	જો (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_blob_with_blob));
	નહીંતર
	    emsg(_(e_invalid_operation_for_blob));
	પાછા FAIL;
    }
    નહીંતર
    {
	val = blob_equal(tv1->vval.v_blob, tv2->vval.v_blob);
	જો (type == પદાવલિ_NEQUAL)
	    val = !val;
    }
    *res = val;
    પાછા OK;
}

/*
 * Compare "tv1" to "tv2" as objects according to "type".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    પૂર્ણાંક
typval_compare_object(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	પૂર્ણાંક	    ic,
	પૂર્ણાંક	    *res)
{
    પૂર્ણાંક res_match = type == પદાવલિ_EQUAL || type == પદાવલિ_IS ? TRUE : FALSE;

    જો (tv1->vval.v_object == ખાલી && tv2->vval.v_object == ખાલી)
    {
	*res = res_match;
	પાછા OK;
    }
    જો (tv1->vval.v_object == ખાલી || tv2->vval.v_object == ખાલી)
    {
	*res = !res_match;
	પાછા OK;
    }

    object_T *obj1 = tv1->vval.v_object;
    object_T *obj2 = tv2->vval.v_object;
    જો (type == પદાવલિ_IS || type == પદાવલિ_ISNOT)
    {
	*res = obj1 == obj2 ? res_match : !res_match;
	પાછા OK;
    }
    નહીંતર જો (tv1->v_type != tv2->v_type)
    {
	emsg(_(e_can_only_compare_object_with_object));
	પાછા FAIL;
    }

    *res = object_equal(obj1, obj2, ic) ? res_match : !res_match;
    પાછા OK;
}

/*
 * Compare "tv1" to "tv2" as dictionaries according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    પૂર્ણાંક
typval_compare_dict(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	પૂર્ણાંક	    ic,
	પૂર્ણાંક	    *res)
{
    પૂર્ણાંક	    val;

    જો (type == પદાવલિ_IS || type == પદાવલિ_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
			&& tv1->vval.v_dict == tv2->vval.v_dict);
	જો (type == પદાવલિ_ISNOT)
	    val = !val;
    }
    નહીંતર જો (tv1->v_type != tv2->v_type
		|| (type != પદાવલિ_EQUAL && type != પદાવલિ_NEQUAL))
    {
	જો (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_dictionary_with_dictionary));
	નહીંતર
	    emsg(_(e_invalid_operation_for_dictionary));
	પાછા FAIL;
    }
    નહીંતર
    {
	val = dict_equal(tv1->vval.v_dict, tv2->vval.v_dict, ic);
	જો (type == પદાવલિ_NEQUAL)
	    val = !val;
    }
    *res = val;
    પાછા OK;
}

/*
 * Compare "tv1" to "tv2" as funcrefs according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    પૂર્ણાંક
typval_compare_func(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	પૂર્ણાંક	    ic,
	પૂર્ણાંક	    *res)
{
    પૂર્ણાંક	    val = 0;

    જો (type != પદાવલિ_EQUAL && type != પદાવલિ_NEQUAL
	    && type != પદાવલિ_IS && type != પદાવલિ_ISNOT)
    {
	emsg(_(e_invalid_operation_for_funcrefs));
	પાછા FAIL;
    }
    જો ((tv1->v_type == VAR_PARTIAL && tv1->vval.v_partial == ખાલી)
	    || (tv2->v_type == VAR_PARTIAL && tv2->vval.v_partial == ખાલી))
	// When both partials are NULL, then they are equal.
	// Otherwise they are not equal.
	val = (tv1->vval.v_partial == tv2->vval.v_partial);
    નહીંતર જો (type == પદાવલિ_IS || type == પદાવલિ_ISNOT)
    {
	જો (tv1->v_type == VAR_FUNC && tv2->v_type == VAR_FUNC)
	    // strings are considered the same if their value is
	    // the same
	    val = tv_equal(tv1, tv2, ic);
	નહીંતર જો (tv1->v_type == VAR_PARTIAL && tv2->v_type == VAR_PARTIAL)
	    val = (tv1->vval.v_partial == tv2->vval.v_partial);
	નહીંતર
	    val = FALSE;
    }
    નહીંતર
	val = tv_equal(tv1, tv2, ic);
    જો (type == પદાવલિ_NEQUAL || type == પદાવલિ_ISNOT)
	val = !val;
    *res = val;
    પાછા OK;
}

/*
 * Compare "tv1" to "tv2" as strings according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    પૂર્ણાંક
typval_compare_string(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	પૂર્ણાંક	    ic,
	પૂર્ણાંક	    *res)
{
    પૂર્ણાંક		i = 0;
    પૂર્ણાંક		val = FALSE;
    char_u	*s1, *s2;
    char_u	buf1[NUMBUFLEN], buf2[NUMBUFLEN];

    જો (in_vim9script()
	  && ((tv1->v_type != VAR_STRING && tv1->v_type != VAR_SPECIAL)
	   || (tv2->v_type != VAR_STRING && tv2->v_type != VAR_SPECIAL)))
    {
	semsg(_(e_cannot_compare_str_with_str),
		   vartype_name(tv1->v_type), vartype_name(tv2->v_type));
	પાછા FAIL;
    }
    s1 = tv_get_string_buf(tv1, buf1);
    s2 = tv_get_string_buf(tv2, buf2);
    જો (type != પદાવલિ_MATCH && type != પદાવલિ_NOMATCH)
	i = ic ? MB_STRICMP(s1, s2) : STRCMP(s1, s2);
    પસંદગી (type)
    {
	સ્થિતિ પદાવલિ_IS:	    જો (in_vim9script())
			    {
				// Really check it is the same string, not just
				// the same value.
				val = tv1->vval.v_string == tv2->vval.v_string;
				તોડો;
			    }
			    // FALLTHROUGH
	સ્થિતિ પદાવલિ_EQUAL:    val = (i == 0); તોડો;
	સ્થિતિ પદાવલિ_ISNOT:    જો (in_vim9script())
			    {
				// Really check it is not the same string, not
				// just a different value.
				val = tv1->vval.v_string != tv2->vval.v_string;
				તોડો;
			    }
			    // FALLTHROUGH
	સ્થિતિ પદાવલિ_NEQUAL:   val = (i != 0); તોડો;
	સ્થિતિ પદાવલિ_GREATER:  val = (i > 0); તોડો;
	સ્થિતિ પદાવલિ_GEQUAL:   val = (i >= 0); તોડો;
	સ્થિતિ પદાવલિ_SMALLER:  val = (i < 0); તોડો;
	સ્થિતિ પદાવલિ_SEQUAL:   val = (i <= 0); તોડો;

	સ્થિતિ પદાવલિ_MATCH:
	સ્થિતિ પદાવલિ_NOMATCH:
		val = pattern_match(s2, s1, ic);
		જો (type == પદાવલિ_NOMATCH)
		    val = !val;
		તોડો;

	મૂળભૂત:  તોડો;  // avoid gcc warning
    }
    *res = val;
    પાછા OK;
}
/*
 * Convert any type to a string, never give an error.
 * When "quotes" is TRUE add quotes to a string.
 * Returns an allocated string.
 */
    char_u *
typval_tostring(typval_T *arg, પૂર્ણાંક quotes)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    char_u	*ret = ખાલી;

    જો (arg == ખાલી)
	પાછા vim_strsave((char_u *)"(does not exist)");
    જો (!quotes && arg->v_type == VAR_STRING)
    {
	ret = vim_strsave(arg->vval.v_string == ખાલી ? (char_u *)""
							 : arg->vval.v_string);
    }
    નહીંતર
    {
	ret = tv2string(arg, &tofree, numbuf, 0);
	// Make a copy if we have a value but it's not in allocated memory.
	જો (ret != ખાલી && tofree == ખાલી)
	    ret = vim_strsave(ret);
    }
    પાછા ret;
}

/*
 * Return TRUE if typeval "tv" is locked: Either that value is locked itself
 * or it refers to a List or Dictionary that is locked.
 */
    પૂર્ણાંક
tv_islocked(typval_T *tv)
{
    પાછા (tv->v_lock & VAR_LOCKED)
	|| (tv->v_type == VAR_LIST
		&& tv->vval.v_list != ખાલી
		&& (tv->vval.v_list->lv_lock & VAR_LOCKED))
	|| (tv->v_type == VAR_TUPLE
		&& tv->vval.v_tuple != ખાલી
		&& (tv->vval.v_tuple->tv_lock & VAR_LOCKED))
	|| (tv->v_type == VAR_DICT
		&& tv->vval.v_dict != ખાલી
		&& (tv->vval.v_dict->dv_lock & VAR_LOCKED));
}

    સ્થિર પૂર્ણાંક
func_equal(
    typval_T *tv1,
    typval_T *tv2,
    પૂર્ણાંક	     ic)	    // ignore case
{
    char_u	*s1, *s2;
    dict_T	*d1, *d2;
    પૂર્ણાંક		a1, a2;
    પૂર્ણાંક		i;

    // empty and NULL function name considered the same
    s1 = tv1->v_type == VAR_FUNC ? tv1->vval.v_string
					   : partial_name(tv1->vval.v_partial);
    જો (s1 != ખાલી && *s1 == NUL)
	s1 = ખાલી;
    s2 = tv2->v_type == VAR_FUNC ? tv2->vval.v_string
					   : partial_name(tv2->vval.v_partial);
    જો (s2 != ખાલી && *s2 == NUL)
	s2 = ખાલી;
    જો (s1 == ખાલી || s2 == ખાલી)
    {
	જો (s1 != s2)
	    પાછા FALSE;
    }
    નહીંતર જો (STRCMP(s1, s2) != 0)
	પાછા FALSE;

    // empty dict and NULL dict is different
    d1 = tv1->v_type == VAR_FUNC ? ખાલી : tv1->vval.v_partial->pt_dict;
    d2 = tv2->v_type == VAR_FUNC ? ખાલી : tv2->vval.v_partial->pt_dict;
    જો (d1 == ખાલી || d2 == ખાલી)
    {
	જો (d1 != d2)
	    પાછા FALSE;
    }
    નહીંતર જો (!dict_equal(d1, d2, ic))
	પાછા FALSE;

    // empty list and no list considered the same
    a1 = tv1->v_type == VAR_FUNC ? 0 : tv1->vval.v_partial->pt_argc;
    a2 = tv2->v_type == VAR_FUNC ? 0 : tv2->vval.v_partial->pt_argc;
    જો (a1 != a2)
	પાછા FALSE;
    માટે (i = 0; i < a1; ++i)
	જો (!tv_equal(tv1->vval.v_partial->pt_argv + i,
		      tv2->vval.v_partial->pt_argv + i, ic))
	    પાછા FALSE;

    પાછા TRUE;
}

/*
 * Return TRUE if "tv1" and "tv2" have the same value.
 * Compares the items just like "==" would compare them, but strings and
 * numbers are different.  Floats and numbers are also different.
 */
    પૂર્ણાંક
tv_equal(
    typval_T *tv1,
    typval_T *tv2,
    પૂર્ણાંક	     ic)	    // ignore case
{
    char_u	buf1[NUMBUFLEN], buf2[NUMBUFLEN];
    char_u	*s1, *s2;
    સ્થિર પૂર્ણાંક  recursive_cnt = 0;	    // catch recursive loops
    પૂર્ણાંક		r;
    સ્થિર પૂર્ણાંક	tv_equal_recurse_limit;

    // Catch lists and dicts that have an endless loop by limiting
    // recursiveness to a limit.  We guess they are equal then.
    // A fixed limit has the problem of still taking an awful long time.
    // Reduce the limit every time running into it. That should work fine for
    // deeply linked structures that are not recursively linked and catch
    // recursiveness quickly.
    જો (recursive_cnt == 0)
	tv_equal_recurse_limit = 1000;
    જો (recursive_cnt >= tv_equal_recurse_limit)
    {
	--tv_equal_recurse_limit;
	પાછા TRUE;
    }

    // For VAR_FUNC and VAR_PARTIAL compare the function name, bound dict and
    // arguments.
    જો ((tv1->v_type == VAR_FUNC
		|| (tv1->v_type == VAR_PARTIAL && tv1->vval.v_partial != ખાલી))
	    && (tv2->v_type == VAR_FUNC
		|| (tv2->v_type == VAR_PARTIAL && tv2->vval.v_partial != ખાલી)))
    {
	++recursive_cnt;
	r = func_equal(tv1, tv2, ic);
	--recursive_cnt;
	પાછા r;
    }

    જો (tv1->v_type != tv2->v_type
	    && ((tv1->v_type != VAR_BOOL && tv1->v_type != VAR_SPECIAL)
		|| (tv2->v_type != VAR_BOOL && tv2->v_type != VAR_SPECIAL)))
	પાછા FALSE;

    પસંદગી (tv1->v_type)
    {
	સ્થિતિ VAR_LIST:
	    ++recursive_cnt;
	    r = list_equal(tv1->vval.v_list, tv2->vval.v_list, ic);
	    --recursive_cnt;
	    પાછા r;

	સ્થિતિ VAR_TUPLE:
	    ++recursive_cnt;
	    r = tuple_equal(tv1->vval.v_tuple, tv2->vval.v_tuple, ic);
	    --recursive_cnt;
	    પાછા r;

	સ્થિતિ VAR_DICT:
	    ++recursive_cnt;
	    r = dict_equal(tv1->vval.v_dict, tv2->vval.v_dict, ic);
	    --recursive_cnt;
	    પાછા r;

	સ્થિતિ VAR_BLOB:
	    પાછા blob_equal(tv1->vval.v_blob, tv2->vval.v_blob);

	સ્થિતિ VAR_સંખ્યા:
	સ્થિતિ VAR_BOOL:
	સ્થિતિ VAR_SPECIAL:
	    પાછા tv1->vval.v_સંખ્યા == tv2->vval.v_સંખ્યા;

	સ્થિતિ VAR_STRING:
	    s1 = tv_get_string_buf(tv1, buf1);
	    s2 = tv_get_string_buf(tv2, buf2);
	    પાછા ((ic ? MB_STRICMP(s1, s2) : STRCMP(s1, s2)) == 0);

	સ્થિતિ VAR_FLOAT:
	    પાછા tv1->vval.v_float == tv2->vval.v_float;
	સ્થિતિ VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    પાછા tv1->vval.v_job == tv2->vval.v_job;
#endif
	સ્થિતિ VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    પાછા tv1->vval.v_channel == tv2->vval.v_channel;
#endif
	સ્થિતિ VAR_INSTR:
	    પાછા tv1->vval.v_instr == tv2->vval.v_instr;

	સ્થિતિ VAR_CLASS:
	    // A class only exists once, equality is identity.
	    પાછા tv1->vval.v_class == tv2->vval.v_class;

	સ્થિતિ VAR_OBJECT:
	    ++recursive_cnt;
	    r = object_equal(tv1->vval.v_object, tv2->vval.v_object, ic);
	    --recursive_cnt;
	    પાછા r;

	સ્થિતિ VAR_PARTIAL:
	    પાછા tv1->vval.v_partial == tv2->vval.v_partial;

	સ્થિતિ VAR_FUNC:
	    પાછા tv1->vval.v_string == tv2->vval.v_string;

	સ્થિતિ VAR_TYPEALIAS:
	    પાછા tv1->vval.v_typealias == tv2->vval.v_typealias;

	સ્થિતિ VAR_UNKNOWN:
	સ્થિતિ VAR_ANY:
	સ્થિતિ VAR_VOID:
	    તોડો;
    }

    // VAR_UNKNOWN can be the result of a invalid expression, let's say it
    // does not equal anything, not even itself.
    પાછા FALSE;
}

/*
 * Get an option value.
 * "arg" points to the '&' or '+' before the option name.
 * "arg" is advanced to character after the option name.
 * Return OK or FAIL.
 */
    પૂર્ણાંક
eval_option(
    char_u	**arg,
    typval_T	*rettv,	// when NULL, only check if option exists
    પૂર્ણાંક		evaluate)
{
    char_u	*option_end;
    long	numval;
    char_u	*stringval;
    getoption_T	opt_type;
    પૂર્ણાંક		c;
    પૂર્ણાંક		working = (**arg == '+');    // has("+option")
    પૂર્ણાંક		ret = OK;
    પૂર્ણાંક		scope;

    // Isolate the option name and find its value.
    option_end = find_option_end(arg, &scope);
    જો (option_end == ખાલી)
    {
	જો (rettv != ખાલી)
	    semsg(_(e_option_name_missing_str), *arg);
	પાછા FAIL;
    }

    જો (!evaluate)
    {
	*arg = option_end;
	પાછા OK;
    }

    c = *option_end;
    *option_end = NUL;
    opt_type = get_option_કિંમત(*arg, &numval,
			       rettv == ખાલી ? ખાલી : &stringval, ખાલી, scope);

    જો (opt_type == gov_unknown)
    {
	જો (rettv != ખાલી)
	    semsg(_(e_unknown_option_str), *arg);
	ret = FAIL;
    }
    નહીંતર જો (rettv != ખાલી)
    {
	rettv->v_lock = 0;
	જો (opt_type == gov_hidden_string)
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = ખાલી;
	}
	નહીંતર જો (opt_type == gov_hidden_bool || opt_type == gov_hidden_સંખ્યા)
	{
	    rettv->v_type = in_vim9script() && opt_type == gov_hidden_bool
						       ? VAR_BOOL : VAR_સંખ્યા;
	    rettv->vval.v_સંખ્યા = 0;
	}
	નહીંતર જો (opt_type == gov_bool || opt_type == gov_સંખ્યા)
	{
	    જો (in_vim9script() && opt_type == gov_bool)
	    {
		rettv->v_type = VAR_BOOL;
		rettv->vval.v_સંખ્યા = numval ? VVAL_TRUE : VVAL_FALSE;
	    }
	    નહીંતર
	    {
		rettv->v_type = VAR_સંખ્યા;
		rettv->vval.v_સંખ્યા = numval;
	    }
	}
	નહીંતર				// string option
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = stringval;
	}
    }
    નહીંતર જો (working && (opt_type == gov_hidden_bool
			|| opt_type == gov_hidden_સંખ્યા
			|| opt_type == gov_hidden_string))
	ret = FAIL;

    *option_end = c;		    // put back for error messages
    *arg = option_end;

    પાછા ret;
}

/*
 * Allocate a variable for a number constant.  Also deals with "0z" for blob.
 * Return OK or FAIL.
 */
    પૂર્ણાંક
eval_સંખ્યા(
	char_u	    **arg,
	typval_T    *rettv,
	પૂર્ણાંક	    evaluate,
	પૂર્ણાંક	    want_string)
{
    પૂર્ણાંક		len;
    પૂર્ણાંક		skip_quotes = !in_old_script(4);
    char_u	*p;
    પૂર્ણાંક		get_float = FALSE;

    // We accept a float when the format matches
    // "[0-9]\+\.[0-9]\+\([eE][+-]\?[0-9]\+\)\?".  This is very
    // strict to avoid backwards compatibility problems.
    // With script version 2 and later the leading digit can be
    // omitted.
    // Don't look for a float after the "." operator, so that
    // ":let vers = 1.2.3" doesn't fail.
    જો (**arg == '.')
	p = *arg;
    નહીંતર
    {
	p = *arg + 1;
	જો (skip_quotes)
	    માટે (;;)
	    {
		જો (*p == '\'')
		    ++p;
		જો (!vim_isdigit(*p))
		    તોડો;
		p = skipdigits(p);
	    }
	નહીંતર
	    p = skipdigits(p);
    }
    જો (!want_string && p[0] == '.' && vim_isdigit(p[1]))
    {
	get_float = TRUE;
	p = skipdigits(p + 2);
	જો (*p == 'e' || *p == 'E')
	{
	    ++p;
	    જો (*p == '-' || *p == '+')
		++p;
	    જો (!vim_isdigit(*p))
		get_float = FALSE;
	    નહીંતર
		p = skipdigits(p + 1);
	}
	જો (ASCII_ISALPHA(*p) || *p == '.')
	    get_float = FALSE;
    }
    જો (get_float)
    {
	float_T	f;

	*arg += string2float(*arg, &f, skip_quotes);
	જો (evaluate)
	{
	    rettv->v_type = VAR_FLOAT;
	    rettv->vval.v_float = f;
	}
    }
    નહીંતર
    જો (**arg == '0' && ((*arg)[1] == 'z' || (*arg)[1] == 'Z'))
    {
	char_u  *bp;
	blob_T  *blob = ખાલી;  // init for gcc

	// Blob constant: 0z0123456789abcdef
	જો (evaluate)
	    blob = blob_alloc();
	માટે (bp = *arg + 2; vim_isxdigit(bp[0]); bp += 2)
	{
	    જો (!vim_isxdigit(bp[1]))
	    {
		જો (blob != ખાલી)
		{
		    emsg(_(e_blob_literal_should_have_an_even_સંખ્યા_of_hex_characters));
		    ga_clear(&blob->bv_ga);
		    VIM_CLEAR(blob);
		}
		પાછા FAIL;
	    }
	    જો (blob != ખાલી)
		ga_append(&blob->bv_ga,
			     (hex2nr(*bp) << 4) + hex2nr(*(bp+1)));
	    જો (bp[2] == '.' && vim_isxdigit(bp[3]))
		++bp;
	}
	જો (blob != ખાલી)
	    rettv_blob_set(rettv, blob);
	*arg = bp;
    }
    નહીંતર
    {
	varnumber_T	n;

	// decimal, hex or octal number
	vim_str2nr(*arg, ખાલી, &len, skip_quotes
		      ? STR2NR_NO_OCT + STR2NR_QUOTE
		      : STR2NR_ALL, &n, ખાલી, 0, TRUE, ખાલી);
	જો (len == 0)
	{
	    જો (evaluate)
		semsg(_(e_invalid_expression_str), *arg);
	    પાછા FAIL;
	}
	*arg += len;
	જો (evaluate)
	{
	    rettv->v_type = VAR_સંખ્યા;
	    rettv->vval.v_સંખ્યા = n;
	}
    }
    પાછા OK;
}

/*
 * Evaluate a string constant and put the result in "rettv".
 * "*arg" points to the double quote or to after it when "interpolate" is TRUE.
 * When "interpolate" is TRUE reduce "{{" to "{", reduce "}}" to "}" and stop
 * at a single "{".
 * Return OK or FAIL.
 */
    પૂર્ણાંક
eval_string(char_u **arg, typval_T *rettv, પૂર્ણાંક evaluate, પૂર્ણાંક interpolate)
{
    char_u	*p;
    char_u	*end;
    પૂર્ણાંક		extra = interpolate ? 1 : 0;
    પૂર્ણાંક		off = interpolate ? 0 : 1;
    પૂર્ણાંક		len;

    // Find the end of the string, skipping backslashed characters.
    માટે (p = *arg + off; *p != NUL && *p != '"'; MB_PTR_ADV(p))
    {
	જો (*p == '\\' && p[1] != NUL)
	{
	    ++p;
	    // A "\<x>" form occupies at least 4 characters, and produces up
	    // to 9 characters (6 for the char and 3 for a modifier):
	    // reserve space for 5 extra.
	    જો (*p == '<')
	    {
		પૂર્ણાંક		modifiers = 0;
		પૂર્ણાંક		flags = FSK_KEYCODE | FSK_IN_STRING;

		extra += 5;

		// Skip to the '>' to avoid using '{' inside for string
		// interpolation.
		જો (p[1] != '*')
		    flags |= FSK_SIMPLIFY;
		જો (find_special_ચાવી(&p, &modifiers, flags, ખાલી) != 0)
		    --p;  // leave "p" on the ">"
	    }
	}
	નહીંતર જો (interpolate && (*p == '{' || *p == '}'))
	{
	    જો (*p == '{' && p[1] != '{') // start of expression
		તોડો;
	    ++p;
	    જો (p[-1] == '}' && *p != '}') // single '}' is an error
	    {
		semsg(_(e_stray_closing_curly_str), *arg);
		પાછા FAIL;
	    }
	    --extra;  // "{{" becomes "{", "}}" becomes "}"
	}
    }

    જો (*p != '"' && !(interpolate && *p == '{'))
    {
	semsg(_(e_missing_double_quote_str), *arg);
	પાછા FAIL;
    }

    // If only parsing, set *arg and return here
    જો (!evaluate)
    {
	*arg = p + off;
	પાછા OK;
    }

    // Copy the string into allocated memory, handling backslashed
    // characters.
    rettv->v_type = VAR_STRING;
    len = (પૂર્ણાંક)(p - *arg + extra);
    rettv->vval.v_string = alloc(len);
    જો (rettv->vval.v_string == ખાલી)
	પાછા FAIL;
    end = rettv->vval.v_string;

    માટે (p = *arg + off; *p != NUL && *p != '"'; )
    {
	જો (*p == '\\')
	{
	    પસંદગી (*++p)
	    {
		સ્થિતિ 'b': *end++ = BS; ++p; તોડો;
		સ્થિતિ 'e': *end++ = ESC; ++p; તોડો;
		સ્થિતિ 'f': *end++ = FF; ++p; તોડો;
		સ્થિતિ 'n': *end++ = NL; ++p; તોડો;
		સ્થિતિ 'r': *end++ = CAR; ++p; તોડો;
		સ્થિતિ 't': *end++ = TAB; ++p; તોડો;

		સ્થિતિ 'X': // hex: "\x1", "\x12"
		સ્થિતિ 'x':
		સ્થિતિ 'u': // Unicode: "\u0023"
		સ્થિતિ 'U':
			  જો (vim_isxdigit(p[1]))
			  {
			      પૂર્ણાંક	n, nr;
			      પૂર્ણાંક	c = SAFE_toupper(*p);

			      જો (c == 'X')
				  n = 2;
			      નહીંતર જો (*p == 'u')
				  n = 4;
			      નહીંતર
				  n = 8;
			      nr = 0;
			      જ્યારે (--n >= 0 && vim_isxdigit(p[1]))
			      {
				  ++p;
				  nr = (nr << 4) + hex2nr(*p);
			      }
			      ++p;
			      // For "\u" store the number according to
			      // 'encoding'.
			      જો (c != 'X')
				  end += (*mb_char2bytes)(nr, end);
			      નહીંતર
				  *end++ = nr;
			  }
			  તોડો;

			  // octal: "\1", "\12", "\123"
		સ્થિતિ '0':
		સ્થિતિ '1':
		સ્થિતિ '2':
		સ્થિતિ '3':
		સ્થિતિ '4':
		સ્થિતિ '5':
		સ્થિતિ '6':
		સ્થિતિ '7': *end = *p++ - '0';
			  જો (*p >= '0' && *p <= '7')
			  {
			      *end = (*end << 3) + *p++ - '0';
			      જો (*p >= '0' && *p <= '7')
				  *end = (*end << 3) + *p++ - '0';
			  }
			  ++end;
			  તોડો;

			  // Special key, e.g.: "\<C-W>"
		સ્થિતિ '<':
			  {
			      પૂર્ણાંક flags = FSK_KEYCODE | FSK_IN_STRING;

			      જો (p[1] != '*')
				  flags |= FSK_SIMPLIFY;
			      extra = trans_special(&p, end, flags, FALSE, ખાલી);
			      જો (extra != 0)
			      {
				  end += extra;
				  જો (end >= rettv->vval.v_string + len)
				      iemsg("eval_string() used more space than allocated");
				  તોડો;
			      }
			  }
			  // FALLTHROUGH

		મૂળભૂત: MB_COPY_CHAR(p, end);
			  તોડો;
	    }
	}
	નહીંતર
	{
	    જો (interpolate && (*p == '{' || *p == '}'))
	    {
		જો (*p == '{' && p[1] != '{') // start of expression
		    તોડો;
		++p;  // reduce "{{" to "{" and "}}" to "}"
	    }
	    MB_COPY_CHAR(p, end);
	}
    }
    *end = NUL;
    જો (*p == '"' && !interpolate)
	++p;
    *arg = p;

    પાછા OK;
}

/*
 * Allocate a variable for a 'str''ing' constant.
 * When "interpolate" is TRUE reduce "{{" to "{" and stop at a single "{".
 * Return OK when a "rettv" was set to the string.
 * Return FAIL on error, "rettv" is not set.
 */
    પૂર્ણાંક
eval_lit_string(char_u **arg, typval_T *rettv, પૂર્ણાંક evaluate, પૂર્ણાંક interpolate)
{
    char_u	*p;
    char_u	*str;
    પૂર્ણાંક		reduce = interpolate ? -1 : 0;
    પૂર્ણાંક		off = interpolate ? 0 : 1;

    // Find the end of the string, skipping ''.
    માટે (p = *arg + off; *p != NUL; MB_PTR_ADV(p))
    {
	જો (*p == '\'')
	{
	    જો (p[1] != '\'')
		તોડો;
	    ++reduce;
	    ++p;
	}
	નહીંતર જો (interpolate)
	{
	    જો (*p == '{')
	    {
		જો (p[1] != '{')
		    તોડો;
		++p;
		++reduce;
	    }
	    નહીંતર જો (*p == '}')
	    {
		++p;
		જો (*p != '}')
		{
		    semsg(_(e_stray_closing_curly_str), *arg);
		    પાછા FAIL;
		}
		++reduce;
	    }
	}
    }

    જો (*p != '\'' && !(interpolate && *p == '{'))
    {
	semsg(_(e_missing_single_quote_str), *arg);
	પાછા FAIL;
    }

    // If only parsing return after setting "*arg"
    જો (!evaluate)
    {
	*arg = p + off;
	પાછા OK;
    }

    // Copy the string into allocated memory, handling '' to ' reduction and
    // any expressions.
    str = alloc((p - *arg) - reduce);
    જો (str == ખાલી)
	પાછા FAIL;
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = str;

    માટે (p = *arg + off; *p != NUL; )
    {
	જો (*p == '\'')
	{
	    જો (p[1] != '\'')
		તોડો;
	    ++p;
	}
	નહીંતર જો (interpolate && (*p == '{' || *p == '}'))
	{
	    જો (*p == '{' && p[1] != '{')
		તોડો;
	    ++p;
	}
	MB_COPY_CHAR(p, str);
    }
    *str = NUL;
    *arg = p + off;

    પાછા OK;
}

/*
 * Evaluate a single or double quoted string possibly containing expressions.
 * "arg" points to the '$'.  The result is put in "rettv".
 * Returns OK or FAIL.
 */
    પૂર્ણાંક
eval_interp_string(char_u **arg, typval_T *rettv, પૂર્ણાંક evaluate)
{
    typval_T	tv;
    પૂર્ણાંક		ret = OK;
    પૂર્ણાંક		quote;
    garray_T	ga;
    char_u	*p;

    ga_init2(&ga, 1, 80);

    // *arg is on the '$' character, move it to the first string character.
    ++*arg;
    quote = **arg;
    ++*arg;

    માટે (;;)
    {
	// Get the string up to the matching quote or to a single '{'.
	// "arg" is advanced to either the quote or the '{'.
	જો (quote == '"')
	    ret = eval_string(arg, &tv, evaluate, TRUE);
	નહીંતર
	    ret = eval_lit_string(arg, &tv, evaluate, TRUE);
	જો (ret == FAIL)
	    તોડો;
	જો (evaluate)
	{
	    ga_concat(&ga, tv.vval.v_string);
	    clear_tv(&tv);
	}

	જો (**arg != '{')
	{
	    // found terminating quote
	    ++*arg;
	    તોડો;
	}
	p = eval_one_પદાવલિ_in_str(*arg, &ga, evaluate);
	જો (p == ખાલી)
	{
	    ret = FAIL;
	    તોડો;
	}
	*arg = p;
    }

    rettv->v_type = VAR_STRING;
    જો (ret == FAIL || !evaluate || ga_append(&ga, NUL) == FAIL)
    {
	ga_clear(&ga);
	rettv->vval.v_string = ખાલી;
	પાછા ret;
    }

    rettv->vval.v_string = ga.ga_data;
    પાછા OK;
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
    પૂર્ણાંક		copyID)
{
    પાછા echo_string_core(tv, tofree, numbuf, copyID, FALSE, TRUE, FALSE);
}

/*
 * Get the value of an environment variable.
 * "arg" is pointing to the '$'.  It is advanced to after the name.
 * If the environment variable was not set, silently assume it is empty.
 * Return FAIL if the name is invalid.
 */
    પૂર્ણાંક
eval_env_var(char_u **arg, typval_T *rettv, પૂર્ણાંક evaluate)
{
    char_u	*string = ખાલી;
    પૂર્ણાંક		len;
    પૂર્ણાંક		cc;
    char_u	*name;
    પૂર્ણાંક		mustfree = FALSE;

    ++*arg;
    name = *arg;
    len = get_env_len(arg);
    જો (evaluate)
    {
	જો (len == 0)
	    પાછા FAIL; // invalid empty name

	cc = name[len];
	name[len] = NUL;
	// first try vim_getenv(), fast for normal environment vars
	string = vim_getenv(name, &mustfree);
	જો (string != ખાલી && *string != NUL)
	{
	    જો (!mustfree)
		string = vim_strsave(string);
	}
	નહીંતર
	{
	    જો (mustfree)
		vim_free(string);

	    // next try expanding things like $VIM and ${HOME}
	    string = expand_env_save(name - 1);
	    જો (string != ખાલી && *string == '$')
		VIM_CLEAR(string);
	}
	name[len] = cc;

	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = string;
	rettv->v_lock = 0;
    }

    પાછા OK;
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
    પૂર્ણાંક		did_emsg_before = did_emsg;

    જો (argvars[0].v_type != VAR_STRING || !in_vim9script())
	lnum = (linenr_T)tv_get_સંખ્યા_chk(&argvars[0], ખાલી);
    જો (lnum <= 0 && did_emsg_before == did_emsg
					    && argvars[0].v_type != VAR_સંખ્યા)
    {
	પૂર્ણાંક	fnum;
	pos_T	*fp;

	// no valid number, try using arg like line()
	fp = var2fpos(&argvars[0], TRUE, &fnum, FALSE);
	જો (fp != ખાલી)
	    lnum = fp->lnum;
    }
    પાછા lnum;
}

/*
 * Get the lnum from the first argument.
 * Also accepts "$", then "buf" is used.
 * Returns 0 on error.
 */
    linenr_T
tv_get_lnum_buf(typval_T *argvars, buf_T *buf)
{
    જો (argvars[0].v_type == VAR_STRING
	    && argvars[0].vval.v_string != ખાલી
	    && argvars[0].vval.v_string[0] == '$'
	    && argvars[0].vval.v_string[1] == NUL
	    && buf != ખાલી)
	પાછા buf->b_ml.ml_લીટી_ગણતરી;
    પાછા (linenr_T)tv_get_સંખ્યા_chk(&argvars[0], ખાલી);
}

/*
 * Get buffer by number or pattern.
 */
    buf_T *
tv_get_buf(typval_T *tv, પૂર્ણાંક curtab_only)
{
    char_u	*name = tv->vval.v_string;
    buf_T	*buf;

    જો (tv->v_type == VAR_સંખ્યા)
	પાછા buflist_findnr((પૂર્ણાંક)tv->vval.v_સંખ્યા);
    જો (tv->v_type != VAR_STRING)
	પાછા ખાલી;
    જો (name == ખાલી || *name == NUL)
	પાછા curbuf;
    જો (name[0] == '$' && name[1] == NUL)
	પાછા lastbuf;

    buf = buflist_find_by_name(name, curtab_only);

    // If not found, try expanding the name, like done for bufexists().
    જો (buf == ખાલી)
	buf = find_buffer(tv);

    પાછા buf;
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
    જો (buf == ખાલી
	    && tv->v_type != VAR_સંખ્યા
	    && tv->v_type != VAR_STRING)
	// issue errmsg for type error
	(શૂન્ય)tv_get_સંખ્યા(tv);
    પાછા buf;
}

/*
 * Check that repeating "slen" items "n" times, does not
 * overflow the int used for the resulting length.
 */
    પૂર્ણાંક
check_repeat_ગણતરી(varnumber_T slen, varnumber_T n)
{
    // Use a division so that the multiplication cannot overflow.
    જો (n <= (varnumber_T)INT_MAX / slen)
	પાછા OK;

    અક્ષર	buf[NUMBUFLEN];

    vim_snprintf(buf, કદ(buf), "%lld", (long long)n);
    semsg(_(e_val_too_large), buf);
    પાછા FAIL;
}

#endif // FEAT_EVAL
