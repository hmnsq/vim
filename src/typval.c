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

#അന്തര്ഭാവ "vim.h"

#എങ്കിൽ defined(FEAT_EVAL)

/*
 * Allocate memory for a variable type-value, and make it empty (0 or NULL
 * value).
 */
    typval_T *
alloc_tv(ശൂന്യം)
{
    തിരികെ ALLOC_CLEAR_ONE(typval_T);
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
    എങ്കിൽ (rettv != ശൂന്യ)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = s;
    }
    അല്ലെങ്കിൽ
	vim_free(s);
    തിരികെ rettv;
}

/*
 * Free the memory for a variable type-value.
 */
    ശൂന്യം
free_tv(typval_T *varp)
{
    എങ്കിൽ (varp == ശൂന്യ)
	തിരികെ;

    തിരഞ്ഞെടുപ്പ് (varp->v_type)
    {
	അവസ്ഥ VAR_FUNC:
	    func_unref(varp->vval.v_string);
	    // FALLTHROUGH
	അവസ്ഥ VAR_STRING:
	    vim_free(varp->vval.v_string);
	    നിർത്തുക;
	അവസ്ഥ VAR_PARTIAL:
	    partial_unref(varp->vval.v_partial);
	    നിർത്തുക;
	അവസ്ഥ VAR_BLOB:
	    blob_unref(varp->vval.v_blob);
	    നിർത്തുക;
	അവസ്ഥ VAR_LIST:
	    list_unref(varp->vval.v_list);
	    നിർത്തുക;
	അവസ്ഥ VAR_TUPLE:
	    tuple_unref(varp->vval.v_tuple);
	    നിർത്തുക;
	അവസ്ഥ VAR_DICT:
	    dict_unref(varp->vval.v_dict);
	    നിർത്തുക;
	അവസ്ഥ VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    job_unref(varp->vval.v_job);
	    നിർത്തുക;
#endif
	അവസ്ഥ VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    channel_unref(varp->vval.v_channel);
	    നിർത്തുക;
#endif
	അവസ്ഥ VAR_CLASS:
	    class_unref(varp->vval.v_class);
	    നിർത്തുക;
	അവസ്ഥ VAR_OBJECT:
	    object_unref(varp->vval.v_object);
	    നിർത്തുക;

	അവസ്ഥ VAR_TYPEALIAS:
	    typealias_unref(varp->vval.v_typealias);
	    നിർത്തുക;

	അവസ്ഥ VAR_സംഖ്യ:
	അവസ്ഥ VAR_FLOAT:
	അവസ്ഥ VAR_ANY:
	അവസ്ഥ VAR_UNKNOWN:
	അവസ്ഥ VAR_VOID:
	അവസ്ഥ VAR_BOOL:
	അവസ്ഥ VAR_SPECIAL:
	അവസ്ഥ VAR_INSTR:
	    നിർത്തുക;
    }
    vim_free(varp);
}

/*
 * Free the memory for a variable value and set the value to NULL or 0.
 */
    ശൂന്യം
clear_tv(typval_T *varp)
{
    എങ്കിൽ (varp == ശൂന്യ)
	തിരികെ;

    തിരഞ്ഞെടുപ്പ് (varp->v_type)
    {
	അവസ്ഥ VAR_FUNC:
	    func_unref(varp->vval.v_string);
	    // FALLTHROUGH
	അവസ്ഥ VAR_STRING:
	    VIM_CLEAR(varp->vval.v_string);
	    നിർത്തുക;
	അവസ്ഥ VAR_PARTIAL:
	    partial_unref(varp->vval.v_partial);
	    varp->vval.v_partial = ശൂന്യ;
	    നിർത്തുക;
	അവസ്ഥ VAR_BLOB:
	    blob_unref(varp->vval.v_blob);
	    varp->vval.v_blob = ശൂന്യ;
	    നിർത്തുക;
	അവസ്ഥ VAR_LIST:
	    list_unref(varp->vval.v_list);
	    varp->vval.v_list = ശൂന്യ;
	    നിർത്തുക;
	അവസ്ഥ VAR_TUPLE:
	    tuple_unref(varp->vval.v_tuple);
	    varp->vval.v_tuple = ശൂന്യ;
	    നിർത്തുക;
	അവസ്ഥ VAR_DICT:
	    dict_unref(varp->vval.v_dict);
	    varp->vval.v_dict = ശൂന്യ;
	    നിർത്തുക;
	അവസ്ഥ VAR_സംഖ്യ:
	അവസ്ഥ VAR_BOOL:
	അവസ്ഥ VAR_SPECIAL:
	    varp->vval.v_സംഖ്യ = 0;
	    നിർത്തുക;
	അവസ്ഥ VAR_FLOAT:
	    varp->vval.v_float = 0.0;
	    നിർത്തുക;
	അവസ്ഥ VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    job_unref(varp->vval.v_job);
	    varp->vval.v_job = ശൂന്യ;
#endif
	    നിർത്തുക;
	അവസ്ഥ VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    channel_unref(varp->vval.v_channel);
	    varp->vval.v_channel = ശൂന്യ;
#endif
	    നിർത്തുക;
	അവസ്ഥ VAR_INSTR:
	    VIM_CLEAR(varp->vval.v_instr);
	    നിർത്തുക;
	അവസ്ഥ VAR_CLASS:
	    class_unref(varp->vval.v_class);
	    varp->vval.v_class = ശൂന്യ;
	    നിർത്തുക;
	അവസ്ഥ VAR_OBJECT:
	    object_unref(varp->vval.v_object);
	    varp->vval.v_object = ശൂന്യ;
	    നിർത്തുക;
	അവസ്ഥ VAR_TYPEALIAS:
	    typealias_unref(varp->vval.v_typealias);
	    varp->vval.v_typealias = ശൂന്യ;
	    നിർത്തുക;
	അവസ്ഥ VAR_UNKNOWN:
	അവസ്ഥ VAR_ANY:
	അവസ്ഥ VAR_VOID:
	    നിർത്തുക;
    }
    varp->v_lock = 0;
}

/*
 * Set the value of a variable to NULL without freeing items.
 */
    ശൂന്യം
init_tv(typval_T *varp)
{
    എങ്കിൽ (varp != ശൂന്യ)
	CLEAR_POINTER(varp);
}

    സ്ഥിതം varnumber_T
tv_get_bool_or_സംഖ്യ_chk(
	typval_T    *varp,
	പൂർണ്ണസംഖ്യ	    *denote,
	പൂർണ്ണസംഖ്യ	    want_bool,
	പൂർണ്ണസംഖ്യ	    vim9_string_error) // in Vim9 using a string is an error
{
    varnumber_T	n = 0L;

    തിരഞ്ഞെടുപ്പ് (varp->v_type)
    {
	അവസ്ഥ VAR_സംഖ്യ:
	    എങ്കിൽ (in_vim9script() && want_bool && varp->vval.v_സംഖ്യ != 0
						   && varp->vval.v_സംഖ്യ != 1)
	    {
		semsg(_(e_using_സംഖ്യ_as_bool_nr), varp->vval.v_സംഖ്യ);
		നിർത്തുക;
	    }
	    തിരികെ varp->vval.v_സംഖ്യ;
	അവസ്ഥ VAR_FLOAT:
	    emsg(_(e_using_float_as_സംഖ്യ));
	    നിർത്തുക;
	അവസ്ഥ VAR_FUNC:
	അവസ്ഥ VAR_PARTIAL:
	    emsg(_(e_using_funcref_as_സംഖ്യ));
	    നിർത്തുക;
	അവസ്ഥ VAR_STRING:
	    എങ്കിൽ (vim9_string_error && in_vim9script())
	    {
		emsg_using_string_as(varp, !want_bool);
		നിർത്തുക;
	    }
	    എങ്കിൽ (varp->vval.v_string != ശൂന്യ)
		vim_str2nr(varp->vval.v_string, ശൂന്യ, ശൂന്യ,
					 STR2NR_ALL, &n, ശൂന്യ, 0, FALSE, ശൂന്യ);
	    തിരികെ n;
	അവസ്ഥ VAR_LIST:
	    emsg(_(e_using_list_as_സംഖ്യ));
	    നിർത്തുക;
	അവസ്ഥ VAR_TUPLE:
	    emsg(_(e_using_tuple_as_സംഖ്യ));
	    നിർത്തുക;
	അവസ്ഥ VAR_DICT:
	    emsg(_(e_using_dictionary_as_സംഖ്യ));
	    നിർത്തുക;
	അവസ്ഥ VAR_BOOL:
	അവസ്ഥ VAR_SPECIAL:
	    എങ്കിൽ (!want_bool && in_vim9script())
	    {
		എങ്കിൽ (varp->v_type == VAR_BOOL)
		    emsg(_(e_using_bool_as_സംഖ്യ));
		അല്ലെങ്കിൽ
		    emsg(_(e_using_special_as_സംഖ്യ));
		നിർത്തുക;
	    }
	    തിരികെ varp->vval.v_സംഖ്യ == VVAL_TRUE ? 1 : 0;
	അവസ്ഥ VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    emsg(_(e_using_job_as_സംഖ്യ));
	    നിർത്തുക;
#endif
	അവസ്ഥ VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    emsg(_(e_using_channel_as_സംഖ്യ));
	    നിർത്തുക;
#endif
	അവസ്ഥ VAR_BLOB:
	    emsg(_(e_using_blob_as_സംഖ്യ));
	    നിർത്തുക;
	അവസ്ഥ VAR_CLASS:
	അവസ്ഥ VAR_TYPEALIAS:
	    check_typval_is_മൂല്യം(varp);
	    നിർത്തുക;
	അവസ്ഥ VAR_OBJECT:
	    {
		എങ്കിൽ (varp->vval.v_object == ശൂന്യ)
		    emsg(_(e_using_object_as_string));
		അല്ലെങ്കിൽ
		{
		    class_T *cl = varp->vval.v_object->obj_class;
		    എങ്കിൽ (cl != ശൂന്യ && IS_ENUM(cl))
			semsg(_(e_using_enum_str_as_സംഖ്യ), cl->class_name.string);
		    അല്ലെങ്കിൽ
			emsg(_(e_using_object_as_സംഖ്യ));
		}
	    }
	    നിർത്തുക;
	അവസ്ഥ VAR_VOID:
	    emsg(_(e_cannot_use_void_മൂല്യം));
	    നിർത്തുക;
	അവസ്ഥ VAR_UNKNOWN:
	അവസ്ഥ VAR_ANY:
	അവസ്ഥ VAR_INSTR:
	    internal_error_no_abort("tv_get_number(UNKNOWN)");
	    നിർത്തുക;
    }
    എങ്കിൽ (denote == ശൂന്യ)		// useful for values that must be unsigned
	n = -1;
    അല്ലെങ്കിൽ
	*denote = TRUE;
    തിരികെ n;
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
tv_get_സംഖ്യ(typval_T *varp)
{
    പൂർണ്ണസംഖ്യ		error = FALSE;

    തിരികെ tv_get_സംഖ്യ_chk(varp, &error);	// return 0L on error
}

/*
 * Like tv_get_number() but in Vim9 script do convert a number in a string to a
 * number without giving an error.
 */
    varnumber_T
tv_to_സംഖ്യ(typval_T *varp)
{
    പൂർണ്ണസംഖ്യ		error = FALSE;

    തിരികെ tv_get_bool_or_സംഖ്യ_chk(varp, &error, FALSE, FALSE);
}

    varnumber_T
tv_get_സംഖ്യ_chk(typval_T *varp, പൂർണ്ണസംഖ്യ *denote)
{
    തിരികെ tv_get_bool_or_സംഖ്യ_chk(varp, denote, FALSE, TRUE);
}

/*
 * Get the boolean value of "varp".  This is like tv_get_number_chk(),
 * but in Vim9 script accepts Number (0 and 1) and Bool/Special.
 */
    varnumber_T
tv_get_bool(typval_T *varp)
{
    തിരികെ tv_get_bool_or_സംഖ്യ_chk(varp, ശൂന്യ, TRUE, TRUE);
}

/*
 * Get the boolean value of "varp".  This is like tv_get_number_chk(),
 * but in Vim9 script accepts Number and Bool.
 */
    varnumber_T
tv_get_bool_chk(typval_T *varp, പൂർണ്ണസംഖ്യ *denote)
{
    തിരികെ tv_get_bool_or_സംഖ്യ_chk(varp, denote, TRUE, TRUE);
}

    സ്ഥിതം float_T
tv_get_float_chk(typval_T *varp, പൂർണ്ണസംഖ്യ *error)
{
    തിരഞ്ഞെടുപ്പ് (varp->v_type)
    {
	അവസ്ഥ VAR_സംഖ്യ:
	    തിരികെ (float_T)(varp->vval.v_സംഖ്യ);
	അവസ്ഥ VAR_FLOAT:
	    തിരികെ varp->vval.v_float;
	അവസ്ഥ VAR_FUNC:
	അവസ്ഥ VAR_PARTIAL:
	    emsg(_(e_using_funcref_as_float));
	    നിർത്തുക;
	അവസ്ഥ VAR_STRING:
	    emsg(_(e_using_string_as_float));
	    നിർത്തുക;
	അവസ്ഥ VAR_LIST:
	    emsg(_(e_using_list_as_float));
	    നിർത്തുക;
	അവസ്ഥ VAR_TUPLE:
	    emsg(_(e_using_tuple_as_float));
	    നിർത്തുക;
	അവസ്ഥ VAR_DICT:
	    emsg(_(e_using_dictionary_as_float));
	    നിർത്തുക;
	അവസ്ഥ VAR_BOOL:
	    emsg(_(e_using_boolean_മൂല്യം_as_float));
	    നിർത്തുക;
	അവസ്ഥ VAR_SPECIAL:
	    emsg(_(e_using_special_മൂല്യം_as_float));
	    നിർത്തുക;
	അവസ്ഥ VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    emsg(_(e_using_job_as_float));
	    നിർത്തുക;
#endif
	അവസ്ഥ VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    emsg(_(e_using_channel_as_float));
	    നിർത്തുക;
#endif
	അവസ്ഥ VAR_BLOB:
	    emsg(_(e_using_blob_as_float));
	    നിർത്തുക;
	അവസ്ഥ VAR_CLASS:
	അവസ്ഥ VAR_TYPEALIAS:
	    check_typval_is_മൂല്യം(varp);
	    നിർത്തുക;
	അവസ്ഥ VAR_OBJECT:
	    emsg(_(e_using_object_as_float));
	    നിർത്തുക;
	അവസ്ഥ VAR_VOID:
	    emsg(_(e_cannot_use_void_മൂല്യം));
	    നിർത്തുക;
	അവസ്ഥ VAR_UNKNOWN:
	അവസ്ഥ VAR_ANY:
	അവസ്ഥ VAR_INSTR:
	    internal_error_no_abort("tv_get_float(UNKNOWN)");
	    നിർത്തുക;
    }
    എങ്കിൽ (error != ശൂന്യ)
	*error = TRUE;
    തിരികെ 0;
}

    float_T
tv_get_float(typval_T *varp)
{
    തിരികെ tv_get_float_chk(varp, ശൂന്യ);
}

/*
 * Give an error and return FAIL unless "args[idx]" is unknown
 */
    പൂർണ്ണസംഖ്യ
check_for_unknown_arg(typval_T *args, പൂർണ്ണസംഖ്യ idx)
{
    എങ്കിൽ (args[idx].v_type != VAR_UNKNOWN)
    {
	semsg(_(e_too_many_arguments), idx + 1);
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string.
 */
    പൂർണ്ണസംഖ്യ
check_for_string_arg(typval_T *args, പൂർണ്ണസംഖ്യ idx)
{
    എങ്കിൽ (args[idx].v_type != VAR_STRING)
    {
	semsg(_(e_string_required_for_argument_nr), idx + 1);
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a non-empty string.
 */
    പൂർണ്ണസംഖ്യ
check_for_nonempty_string_arg(typval_T *args, പൂർണ്ണസംഖ്യ idx)
{
    എങ്കിൽ (check_for_string_arg(args, idx) == FAIL)
	തിരികെ FAIL;
    എങ്കിൽ (args[idx].vval.v_string == ശൂന്യ || *args[idx].vval.v_string == NUL)
    {
	semsg(_(e_non_empty_string_required_for_argument_nr), idx + 1);
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Check for an optional string argument at 'idx'
 */
    പൂർണ്ണസംഖ്യ
check_for_opt_string_arg(typval_T *args, പൂർണ്ണസംഖ്യ idx)
{
    തിരികെ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_string_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a number.
 */
    പൂർണ്ണസംഖ്യ
check_for_സംഖ്യ_arg(typval_T *args, പൂർണ്ണസംഖ്യ idx)
{
    എങ്കിൽ (args[idx].v_type != VAR_സംഖ്യ)
    {
	semsg(_(e_സംഖ്യ_required_for_argument_nr), idx + 1);
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Check for an optional number argument at 'idx'
 */
    പൂർണ്ണസംഖ്യ
check_for_opt_സംഖ്യ_arg(typval_T *args, പൂർണ്ണസംഖ്യ idx)
{
    തിരികെ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_സംഖ്യ_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a float or a number.
 */
    പൂർണ്ണസംഖ്യ
check_for_float_or_nr_arg(typval_T *args, പൂർണ്ണസംഖ്യ idx)
{
    എങ്കിൽ (args[idx].v_type != VAR_FLOAT && args[idx].v_type != VAR_സംഖ്യ)
    {
	semsg(_(e_float_or_സംഖ്യ_required_for_argument_nr), idx + 1);
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a bool.
 */
    പൂർണ്ണസംഖ്യ
check_for_bool_arg(typval_T *args, പൂർണ്ണസംഖ്യ idx)
{
    എങ്കിൽ (args[idx].v_type != VAR_BOOL
	    && !(args[idx].v_type == VAR_സംഖ്യ
		&& (args[idx].vval.v_സംഖ്യ == 0
		    || args[idx].vval.v_സംഖ്യ == 1)))
    {
	semsg(_(e_bool_required_for_argument_nr), idx + 1);
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a bool or a number.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
check_for_bool_or_സംഖ്യ_arg(typval_T *args, പൂർണ്ണസംഖ്യ idx)
{
    എങ്കിൽ (args[idx].v_type != VAR_BOOL && args[idx].v_type != VAR_സംഖ്യ)
    {
	semsg(_(e_bool_or_സംഖ്യ_required_for_argument_nr), idx + 1);
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Check for an optional bool argument at 'idx'.
 * Return FAIL if the type is wrong.
 */
    പൂർണ്ണസംഖ്യ
check_for_opt_bool_arg(typval_T *args, പൂർണ്ണസംഖ്യ idx)
{
    എങ്കിൽ (args[idx].v_type == VAR_UNKNOWN)
	തിരികെ OK;
    തിരികെ check_for_bool_arg(args, idx);
}

/*
 * Check for an optional bool or number argument at 'idx'.
 * Return FAIL if the type is wrong.
 */
    പൂർണ്ണസംഖ്യ
check_for_opt_bool_or_സംഖ്യ_arg(typval_T *args, പൂർണ്ണസംഖ്യ idx)
{
    എങ്കിൽ (args[idx].v_type == VAR_UNKNOWN)
	തിരികെ OK;
    തിരികെ check_for_bool_or_സംഖ്യ_arg(args, idx);
}

/*
 * Check for an optional bool or dict argument at 'idx'.
 * Return FAIL if the type is wrong.
 */
    പൂർണ്ണസംഖ്യ
check_for_opt_bool_or_dict_arg(typval_T *args, പൂർണ്ണസംഖ്യ idx)
{
    എങ്കിൽ (args[idx].v_type == VAR_UNKNOWN || args[idx].v_type == VAR_DICT)
	തിരികെ OK;
    തിരികെ check_for_bool_arg(args, idx);
}

/*
 * Give an error and return FAIL unless "args[idx]" is a blob.
 */
    പൂർണ്ണസംഖ്യ
check_for_blob_arg(typval_T *args, പൂർണ്ണസംഖ്യ idx)
{
    എങ്കിൽ (args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_blob_required_for_argument_nr), idx + 1);
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list.
 */
    പൂർണ്ണസംഖ്യ
check_for_list_arg(typval_T *args, പൂർണ്ണസംഖ്യ idx)
{
    എങ്കിൽ (args[idx].v_type != VAR_LIST)
    {
	semsg(_(e_list_required_for_argument_nr), idx + 1);
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a non-NULL list.
 */
    പൂർണ്ണസംഖ്യ
check_for_nonnull_list_arg(typval_T *args, പൂർണ്ണസംഖ്യ idx)
{
    എങ്കിൽ (check_for_list_arg(args, idx) == FAIL)
	തിരികെ FAIL;

    എങ്കിൽ (args[idx].vval.v_list == ശൂന്യ)
    {
	semsg(_(e_non_null_list_required_for_argument_nr), idx + 1);
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Check for an optional list argument at 'idx'
 */
    പൂർണ്ണസംഖ്യ
check_for_opt_list_arg(typval_T *args, പൂർണ്ണസംഖ്യ idx)
{
    തിരികെ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_list_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a tuple.
 */
    പൂർണ്ണസംഖ്യ
check_for_tuple_arg(typval_T *args, പൂർണ്ണസംഖ്യ idx)
{
    എങ്കിൽ (args[idx].v_type != VAR_TUPLE)
    {
	semsg(_(e_tuple_required_for_argument_nr), idx + 1);
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a dict.
 */
    പൂർണ്ണസംഖ്യ
check_for_dict_arg(typval_T *args, പൂർണ്ണസംഖ്യ idx)
{
    എങ്കിൽ (args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_dict_required_for_argument_nr), idx + 1);
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a non-NULL dict.
 */
    പൂർണ്ണസംഖ്യ
check_for_nonnull_dict_arg(typval_T *args, പൂർണ്ണസംഖ്യ idx)
{
    എങ്കിൽ (check_for_dict_arg(args, idx) == FAIL)
	തിരികെ FAIL;

    എങ്കിൽ (args[idx].vval.v_dict == ശൂന്യ)
    {
	semsg(_(e_non_null_dict_required_for_argument_nr), idx + 1);
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Check for an optional dict argument at 'idx'
 */
    പൂർണ്ണസംഖ്യ
check_for_opt_dict_arg(typval_T *args, പൂർണ്ണസംഖ്യ idx)
{
    തിരികെ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_dict_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Check for an optional non-NULL dict argument at 'idx'
 */
    പൂർണ്ണസംഖ്യ
check_for_opt_nonnull_dict_arg(typval_T *args, പൂർണ്ണസംഖ്യ idx)
{
    തിരികെ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_nonnull_dict_arg(args, idx) != FAIL) ? OK : FAIL;
}

#എങ്കിൽ defined(FEAT_JOB_CHANNEL)
/*
 * Give an error and return FAIL unless "args[idx]" is a channel or a job.
 */
    പൂർണ്ണസംഖ്യ
check_for_chan_or_job_arg(typval_T *args, പൂർണ്ണസംഖ്യ idx)
{
    എങ്കിൽ (args[idx].v_type != VAR_CHANNEL && args[idx].v_type != VAR_JOB)
    {
	semsg(_(e_chan_or_job_required_for_argument_nr), idx + 1);
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an optional channel or a
 * job.
 */
    പൂർണ്ണസംഖ്യ
check_for_opt_chan_or_job_arg(typval_T *args, പൂർണ്ണസംഖ്യ idx)
{
    തിരികെ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_chan_or_job_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a job.
 */
    പൂർണ്ണസംഖ്യ
check_for_job_arg(typval_T *args, പൂർണ്ണസംഖ്യ idx)
{
    എങ്കിൽ (args[idx].v_type != VAR_JOB)
    {
	semsg(_(e_job_required_for_argument_nr), idx + 1);
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Check for an optional job argument at 'idx'.
 */
    പൂർണ്ണസംഖ്യ
check_for_opt_job_arg(typval_T *args, പൂർണ്ണസംഖ്യ idx)
{
    തിരികെ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_job_arg(args, idx) != FAIL) ? OK : FAIL;
}
#അല്ലെങ്കിൽ
/*
 * Give an error and return FAIL unless "args[idx]" is an optional channel or a
 * job.  Used without the +channel feature, thus only VAR_UNKNOWN is accepted.
 */
    പൂർണ്ണസംഖ്യ
check_for_opt_chan_or_job_arg(typval_T *args, പൂർണ്ണസംഖ്യ idx)
{
    തിരികെ args[idx].v_type == VAR_UNKNOWN ? OK : FAIL;
}
#endif

/*
 * Give an error and return FAIL unless "args[idx]" is a string or
 * a number.
 */
    പൂർണ്ണസംഖ്യ
check_for_string_or_സംഖ്യ_arg(typval_T *args, പൂർണ്ണസംഖ്യ idx)
{
    എങ്കിൽ (args[idx].v_type != VAR_STRING && args[idx].v_type != VAR_സംഖ്യ)
    {
	semsg(_(e_string_or_സംഖ്യ_required_for_argument_nr), idx + 1);
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Check for an optional string or number argument at 'idx'.
 */
    പൂർണ്ണസംഖ്യ
check_for_opt_string_or_സംഖ്യ_arg(typval_T *args, പൂർണ്ണസംഖ്യ idx)
{
    തിരികെ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_string_or_സംഖ്യ_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a buffer number.
 * Buffer number can be a number or a string.
 */
    പൂർണ്ണസംഖ്യ
check_for_buffer_arg(typval_T *args, പൂർണ്ണസംഖ്യ idx)
{
    തിരികെ check_for_string_or_സംഖ്യ_arg(args, idx);
}

/*
 * Check for an optional buffer argument at 'idx'
 */
    പൂർണ്ണസംഖ്യ
check_for_opt_buffer_arg(typval_T *args, പൂർണ്ണസംഖ്യ idx)
{
    തിരികെ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_buffer_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a line number.
 * Line number can be a number or a string.
 */
    പൂർണ്ണസംഖ്യ
check_for_lnum_arg(typval_T *args, പൂർണ്ണസംഖ്യ idx)
{
    തിരികെ check_for_string_or_സംഖ്യ_arg(args, idx);
}

/*
 * Check for an optional line number argument at 'idx'
 */
    പൂർണ്ണസംഖ്യ
check_for_opt_lnum_arg(typval_T *args, പൂർണ്ണസംഖ്യ idx)
{
    തിരികെ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_lnum_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a blob.
 */
    പൂർണ്ണസംഖ്യ
check_for_string_or_blob_arg(typval_T *args, പൂർണ്ണസംഖ്യ idx)
{
    എങ്കിൽ (args[idx].v_type != VAR_STRING && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_string_or_blob_required_for_argument_nr), idx + 1);
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a list.
 */
    പൂർണ്ണസംഖ്യ
check_for_string_or_list_arg(typval_T *args, പൂർണ്ണസംഖ്യ idx)
{
    എങ്കിൽ (args[idx].v_type != VAR_STRING && args[idx].v_type != VAR_LIST)
    {
	semsg(_(e_string_or_list_required_for_argument_nr), idx + 1);
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string, a list, a
 * tuple or a blob.
 */
    പൂർണ്ണസംഖ്യ
check_for_string_or_list_or_tuple_or_blob_arg(typval_T *args, പൂർണ്ണസംഖ്യ idx)
{
    എങ്കിൽ (args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_string_list_tuple_or_blob_required_for_argument_nr), idx + 1);
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Check for an optional string or list argument at 'idx'
 */
    പൂർണ്ണസംഖ്യ
check_for_opt_string_or_list_arg(typval_T *args, പൂർണ്ണസംഖ്യ idx)
{
    തിരികെ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_string_or_list_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a dict.
 */
    പൂർണ്ണസംഖ്യ
check_for_string_or_dict_arg(typval_T *args, പൂർണ്ണസംഖ്യ idx)
{
    എങ്കിൽ (args[idx].v_type != VAR_STRING && args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_string_or_dict_required_for_argument_nr), idx + 1);
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a number
 * or a list.
 */
    പൂർണ്ണസംഖ്യ
check_for_string_or_സംഖ്യ_or_list_arg(typval_T *args, പൂർണ്ണസംഖ്യ idx)
{
    എങ്കിൽ (args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_സംഖ്യ
	    && args[idx].v_type != VAR_LIST)
    {
	semsg(_(e_string_സംഖ്യ_or_list_required_for_argument_nr), idx + 1);
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an optional string
 * or number or a list
 */
    പൂർണ്ണസംഖ്യ
check_for_opt_string_or_സംഖ്യ_or_list_arg(typval_T *args, പൂർണ്ണസംഖ്യ idx)
{
    തിരികെ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_string_or_സംഖ്യ_or_list_arg(args, idx)
							!= FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string, a number, a
 * list, a tuple or a blob.
 */
    പൂർണ്ണസംഖ്യ
check_for_repeat_func_arg(typval_T *args, പൂർണ്ണസംഖ്യ idx)
{
    എങ്കിൽ (args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_സംഖ്യ
	    && args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_repeatable_type_required_for_argument_nr), idx + 1);
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string, a list, a
 * tuple or a dict.
 */
    പൂർണ്ണസംഖ്യ
check_for_string_list_tuple_or_dict_arg(typval_T *args, പൂർണ്ണസംഖ്യ idx)
{
    എങ്കിൽ (args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_string_list_tuple_or_dict_required_for_argument_nr), idx + 1);
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string
 * or a function reference.
 */
    പൂർണ്ണസംഖ്യ
check_for_string_or_func_arg(typval_T *args, പൂർണ്ണസംഖ്യ idx)
{
    എങ്കിൽ (args[idx].v_type != VAR_PARTIAL
	    && args[idx].v_type != VAR_FUNC
	    && args[idx].v_type != VAR_STRING)
    {
	semsg(_(e_string_or_function_required_for_argument_nr), idx + 1);
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list or a blob.
 */
    പൂർണ്ണസംഖ്യ
check_for_list_or_blob_arg(typval_T *args, പൂർണ്ണസംഖ്യ idx)
{
    എങ്കിൽ (args[idx].v_type != VAR_LIST && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_list_or_blob_required_for_argument_nr), idx + 1);
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list or a tuple.
 */
    പൂർണ്ണസംഖ്യ
check_for_list_or_tuple_arg(typval_T *args, പൂർണ്ണസംഖ്യ idx)
{
    എങ്കിൽ (args[idx].v_type != VAR_LIST && args[idx].v_type != VAR_TUPLE)
    {
	semsg(_(e_list_or_tuple_required_for_argument_nr), idx + 1);
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list, a tuple or a
 * blob.
 */
    പൂർണ്ണസംഖ്യ
check_for_list_or_tuple_or_blob_arg(typval_T *args, പൂർണ്ണസംഖ്യ idx)
{
    എങ്കിൽ (args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_list_or_tuple_or_blob_required_for_argument_nr), idx + 1);
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list, a tuple or a
 * dict
 */
    പൂർണ്ണസംഖ്യ
check_for_list_or_tuple_or_dict_arg(typval_T *args, പൂർണ്ണസംഖ്യ idx)
{
    എങ്കിൽ (args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_list_or_tuple_or_dict_required_for_argument_nr), idx + 1);
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list or dict or a
 * blob.
 */
    പൂർണ്ണസംഖ്യ
check_for_list_or_dict_or_blob_arg(typval_T *args, പൂർണ്ണസംഖ്യ idx)
{
    എങ്കിൽ (args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_DICT
	    && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_list_dict_or_blob_required_for_argument_nr), idx + 1);
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list, a tuple, a dict,
 * a blob or a string.
 */
    പൂർണ്ണസംഖ്യ
check_for_list_tuple_dict_blob_or_string_arg(typval_T *args, പൂർണ്ണസംഖ്യ idx)
{
    എങ്കിൽ (args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_DICT
	    && args[idx].v_type != VAR_BLOB
	    && args[idx].v_type != VAR_STRING)
    {
	semsg(_(e_list_tuple_dict_blob_or_string_required_for_argument_nr), idx + 1);
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an optional buffer
 * number or a dict.
 */
    പൂർണ്ണസംഖ്യ
check_for_opt_buffer_or_dict_arg(typval_T *args, പൂർണ്ണസംഖ്യ idx)
{
    എങ്കിൽ (args[idx].v_type != VAR_UNKNOWN
	    && args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_സംഖ്യ
	    && args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_string_required_for_argument_nr), idx + 1);
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an object.
 */
    പൂർണ്ണസംഖ്യ
check_for_object_arg(typval_T *args, പൂർണ്ണസംഖ്യ idx)
{
    എങ്കിൽ (args[idx].v_type != VAR_OBJECT)
    {
	semsg(_(e_object_required_for_argument_nr), idx + 1);
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Returns TRUE if "tv" is a type alias for a class
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
tv_class_alias(typval_T *tv)
{
    തിരികെ tv->v_type == VAR_TYPEALIAS &&
			tv->vval.v_typealias->ta_type->tt_type == VAR_OBJECT;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a class
 * or class typealias.
 */
    പൂർണ്ണസംഖ്യ
check_for_class_or_typealias_args(typval_T *args, പൂർണ്ണസംഖ്യ idx)
{
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = idx; args[i].v_type != VAR_UNKNOWN; ++i)
    {
	എങ്കിൽ (args[i].v_type != VAR_CLASS && !tv_class_alias(&args[idx]))
	{
	    semsg(_(e_class_or_typealias_required_for_argument_nr), i + 1);
	    തിരികെ FAIL;
	}
    }
    തിരികെ OK;
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
    സ്ഥിതം char_u   mybuf[NUMBUFLEN];

    തിരികെ tv_get_string_buf(varp, mybuf);
}

/*
 * Like tv_get_string() but don't allow number to string conversion for Vim9.
 */
    char_u *
tv_get_string_strict(typval_T *varp)
{
    സ്ഥിതം char_u   mybuf[NUMBUFLEN];
    char_u	    *res =  tv_get_string_buf_chk_strict(
						 varp, mybuf, in_vim9script());

    തിരികെ res != ശൂന്യ ? res : (char_u *)"";
}

    char_u *
tv_get_string_buf(typval_T *varp, char_u *buf)
{
    char_u	*res = tv_get_string_buf_chk(varp, buf);

    തിരികെ res != ശൂന്യ ? res : (char_u *)"";
}

/*
 * Careful: This uses a single, static buffer.  YOU CAN ONLY USE IT ONCE!
 */
    char_u *
tv_get_string_chk(typval_T *varp)
{
    സ്ഥിതം char_u   mybuf[NUMBUFLEN];

    തിരികെ tv_get_string_buf_chk(varp, mybuf);
}

    char_u *
tv_get_string_buf_chk(typval_T *varp, char_u *buf)
{
    തിരികെ tv_get_string_buf_chk_strict(varp, buf, FALSE);
}

    char_u *
tv_get_string_buf_chk_strict(typval_T *varp, char_u *buf, പൂർണ്ണസംഖ്യ strict)
{
    തിരഞ്ഞെടുപ്പ് (varp->v_type)
    {
	അവസ്ഥ VAR_സംഖ്യ:
	    എങ്കിൽ (strict)
	    {
		emsg(_(e_using_സംഖ്യ_as_string));
		നിർത്തുക;
	    }
	    vim_snprintf((അക്ഷരം *)buf, NUMBUFLEN, "%lld",
					    (varnumber_T)varp->vval.v_സംഖ്യ);
	    തിരികെ buf;
	അവസ്ഥ VAR_FUNC:
	അവസ്ഥ VAR_PARTIAL:
	    emsg(_(e_using_funcref_as_string));
	    നിർത്തുക;
	അവസ്ഥ VAR_LIST:
	    emsg(_(e_using_list_as_string));
	    നിർത്തുക;
	അവസ്ഥ VAR_TUPLE:
	    emsg(_(e_using_tuple_as_string));
	    നിർത്തുക;
	അവസ്ഥ VAR_DICT:
	    emsg(_(e_using_dictionary_as_string));
	    നിർത്തുക;
	അവസ്ഥ VAR_FLOAT:
	    എങ്കിൽ (strict)
	    {
		emsg(_(e_using_float_as_string));
		നിർത്തുക;
	    }
	    vim_snprintf((അക്ഷരം *)buf, NUMBUFLEN, "%g", varp->vval.v_float);
	    തിരികെ buf;
	അവസ്ഥ VAR_STRING:
	    എങ്കിൽ (varp->vval.v_string != ശൂന്യ)
		തിരികെ varp->vval.v_string;
	    തിരികെ (char_u *)"";
	അവസ്ഥ VAR_BOOL:
	അവസ്ഥ VAR_SPECIAL:
	    STRCPY(buf, get_var_special_name(varp->vval.v_സംഖ്യ));
	    തിരികെ buf;
	അവസ്ഥ VAR_BLOB:
	    emsg(_(e_using_blob_as_string));
	    നിർത്തുക;
	അവസ്ഥ VAR_CLASS:
	അവസ്ഥ VAR_TYPEALIAS:
	    check_typval_is_മൂല്യം(varp);
	    നിർത്തുക;
	അവസ്ഥ VAR_OBJECT:
	    {
		എങ്കിൽ (varp->vval.v_object == ശൂന്യ)
		    emsg(_(e_using_object_as_string));
		അല്ലെങ്കിൽ
		{
		    class_T *cl = varp->vval.v_object->obj_class;
		    എങ്കിൽ (cl != ശൂന്യ && IS_ENUM(cl))
			semsg(_(e_using_enum_str_as_string), cl->class_name.string);
		    അല്ലെങ്കിൽ
			emsg(_(e_using_object_as_string));
		}
	    }
	    നിർത്തുക;
	അവസ്ഥ VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    എങ്കിൽ (in_vim9script())
	    {
		semsg(_(e_using_invalid_മൂല്യം_as_string_str), "job");
		നിർത്തുക;
	    }
	    തിരികെ job_to_string_buf(varp, buf);
#endif
	    നിർത്തുക;
	അവസ്ഥ VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    എങ്കിൽ (in_vim9script())
	    {
		semsg(_(e_using_invalid_മൂല്യം_as_string_str), "channel");
		നിർത്തുക;
	    }
	    തിരികെ channel_to_string_buf(varp, buf);
#endif
	    നിർത്തുക;
	അവസ്ഥ VAR_VOID:
	    emsg(_(e_cannot_use_void_മൂല്യം));
	    നിർത്തുക;
	അവസ്ഥ VAR_UNKNOWN:
	അവസ്ഥ VAR_ANY:
	അവസ്ഥ VAR_INSTR:
	    semsg(_(e_using_invalid_മൂല്യം_as_string_str),
						  vartype_name(varp->v_type));
	    നിർത്തുക;
    }
    തിരികെ ശൂന്യ;
}

/*
 * Turn a typeval into a string.  Similar to tv_get_string_buf() but uses
 * string() on Dict, List, etc.
 */
    char_u *
tv_stringify(typval_T *varp, char_u *buf)
{
    എങ്കിൽ (varp->v_type == VAR_LIST
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
	തിരികെ tmp.vval.v_string;
    }
    തിരികെ tv_get_string_buf(varp, buf);
}

/*
 * Return TRUE if typeval "tv" and its value are set to be locked (immutable).
 * Also give an error message, using "name" or _("name") when use_gettext is
 * TRUE.
 */
    പൂർണ്ണസംഖ്യ
tv_check_lock(typval_T *tv, char_u *name, പൂർണ്ണസംഖ്യ use_gettext)
{
    പൂർണ്ണസംഖ്യ	lock = 0;

    തിരഞ്ഞെടുപ്പ് (tv->v_type)
    {
	അവസ്ഥ VAR_BLOB:
	    എങ്കിൽ (tv->vval.v_blob != ശൂന്യ)
		lock = tv->vval.v_blob->bv_lock;
	    നിർത്തുക;
	അവസ്ഥ VAR_LIST:
	    എങ്കിൽ (tv->vval.v_list != ശൂന്യ)
		lock = tv->vval.v_list->lv_lock;
	    നിർത്തുക;
	അവസ്ഥ VAR_TUPLE:
	    എങ്കിൽ (tv->vval.v_tuple != ശൂന്യ)
		lock = tv->vval.v_tuple->tv_lock;
	    നിർത്തുക;
	അവസ്ഥ VAR_DICT:
	    എങ്കിൽ (tv->vval.v_dict != ശൂന്യ)
		lock = tv->vval.v_dict->dv_lock;
	    നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
	    നിർത്തുക;
    }
    തിരികെ മൂല്യം_check_lock(tv->v_lock, name, use_gettext)
		   || (lock != 0 && മൂല്യം_check_lock(lock, name, use_gettext));
}

/*
 * Copy the values from typval_T "from" to typval_T "to".
 * When needed allocates string or increases reference count.
 * Does not make a copy of a list, blob or dict but copies the reference!
 * It is OK for "from" and "to" to point to the same item.  This is used to
 * make a copy later.
 */
    പൂർണ്ണസംഖ്യ
copy_tv(typval_T *from, typval_T *to)
{
    പൂർണ്ണസംഖ്യ		ret = OK;

    to->v_type = from->v_type;
    to->v_lock = 0;
    തിരഞ്ഞെടുപ്പ് (from->v_type)
    {
	അവസ്ഥ VAR_സംഖ്യ:
	അവസ്ഥ VAR_BOOL:
	അവസ്ഥ VAR_SPECIAL:
	    to->vval.v_സംഖ്യ = from->vval.v_സംഖ്യ;
	    നിർത്തുക;
	അവസ്ഥ VAR_FLOAT:
	    to->vval.v_float = from->vval.v_float;
	    നിർത്തുക;
	അവസ്ഥ VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    to->vval.v_job = from->vval.v_job;
	    എങ്കിൽ (to->vval.v_job != ശൂന്യ)
		++to->vval.v_job->jv_refcount;
	    നിർത്തുക;
#endif
	അവസ്ഥ VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    to->vval.v_channel = from->vval.v_channel;
	    എങ്കിൽ (to->vval.v_channel != ശൂന്യ)
		++to->vval.v_channel->ch_refcount;
	    നിർത്തുക;
#endif
	അവസ്ഥ VAR_INSTR:
	    to->vval.v_instr = from->vval.v_instr;
	    നിർത്തുക;

	അവസ്ഥ VAR_CLASS:
	    copy_class(from, to);
	    നിർത്തുക;

	അവസ്ഥ VAR_OBJECT:
	    copy_object(from, to);
	    നിർത്തുക;

	അവസ്ഥ VAR_STRING:
	അവസ്ഥ VAR_FUNC:
	    എങ്കിൽ (from->vval.v_string == ശൂന്യ)
		to->vval.v_string = ശൂന്യ;
	    അല്ലെങ്കിൽ
	    {
		to->vval.v_string = vim_strsave(from->vval.v_string);
		എങ്കിൽ (from->v_type == VAR_FUNC)
		    func_ref(to->vval.v_string);
	    }
	    നിർത്തുക;
	അവസ്ഥ VAR_PARTIAL:
	    എങ്കിൽ (from->vval.v_partial == ശൂന്യ)
		to->vval.v_partial = ശൂന്യ;
	    അല്ലെങ്കിൽ
	    {
		to->vval.v_partial = from->vval.v_partial;
		++to->vval.v_partial->pt_refcount;
	    }
	    നിർത്തുക;
	അവസ്ഥ VAR_BLOB:
	    എങ്കിൽ (from->vval.v_blob == ശൂന്യ)
		to->vval.v_blob = ശൂന്യ;
	    അല്ലെങ്കിൽ
	    {
		to->vval.v_blob = from->vval.v_blob;
		++to->vval.v_blob->bv_refcount;
	    }
	    നിർത്തുക;
	അവസ്ഥ VAR_LIST:
	    എങ്കിൽ (from->vval.v_list == ശൂന്യ)
		to->vval.v_list = ശൂന്യ;
	    അല്ലെങ്കിൽ
	    {
		to->vval.v_list = from->vval.v_list;
		++to->vval.v_list->lv_refcount;
	    }
	    നിർത്തുക;
	അവസ്ഥ VAR_TUPLE:
	    എങ്കിൽ (from->vval.v_tuple == ശൂന്യ)
		to->vval.v_tuple = ശൂന്യ;
	    അല്ലെങ്കിൽ
	    {
		to->vval.v_tuple = from->vval.v_tuple;
		++to->vval.v_tuple->tv_refcount;
	    }
	    നിർത്തുക;
	അവസ്ഥ VAR_DICT:
	    എങ്കിൽ (from->vval.v_dict == ശൂന്യ)
		to->vval.v_dict = ശൂന്യ;
	    അല്ലെങ്കിൽ
	    {
		to->vval.v_dict = from->vval.v_dict;
		++to->vval.v_dict->dv_refcount;
	    }
	    നിർത്തുക;
	അവസ്ഥ VAR_TYPEALIAS:
	    എങ്കിൽ (from->vval.v_typealias == ശൂന്യ)
		to->vval.v_typealias = ശൂന്യ;
	    അല്ലെങ്കിൽ
	    {
		to->vval.v_typealias = from->vval.v_typealias;
		++to->vval.v_typealias->ta_refcount;
	    }
	    നിർത്തുക;
	അവസ്ഥ VAR_VOID:
	    emsg(_(e_cannot_use_void_മൂല്യം));
	    ret = FAIL;
	    നിർത്തുക;
	അവസ്ഥ VAR_UNKNOWN:
	അവസ്ഥ VAR_ANY:
	    internal_error_no_abort("copy_tv(UNKNOWN)");
	    ret = FAIL;
	    നിർത്തുക;
    }

    തിരികെ ret;
}

/*
 * Compare "tv1" and "tv2".
 */
    പൂർണ്ണസംഖ്യ
typval_compare2(
    typval_T	*tv1,	// first operand
    typval_T	*tv2,	// second operand
    exprtype_T	type,	// operator
    പൂർണ്ണസംഖ്യ		ic,	// ignore case
    പൂർണ്ണസംഖ്യ		*res)	// comparison result
{
    varnumber_T	n1, n2;
    പൂർണ്ണസംഖ്യ		type_is = type == വ്യഞ്ജകം_IS || type == വ്യഞ്ജകം_ISNOT;

    എങ്കിൽ (check_typval_is_മൂല്യം(tv1) == FAIL
	|| check_typval_is_മൂല്യം(tv2) == FAIL)
	തിരികെ FAIL;
    അല്ലെങ്കിൽ എങ്കിൽ (type_is && tv1->v_type != tv2->v_type)
    {
	// For "is" a different type always means FALSE, for "isnot"
	// it means TRUE.
	*res = (type == വ്യഞ്ജകം_ISNOT);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (((tv1->v_type == VAR_SPECIAL && tv1->vval.v_സംഖ്യ == VVAL_NULL)
		|| (tv2->v_type == VAR_SPECIAL
					   && tv2->vval.v_സംഖ്യ == VVAL_NULL))
	    && tv1->v_type != tv2->v_type
	    && (type == വ്യഞ്ജകം_EQUAL || type == വ്യഞ്ജകം_NEQUAL))
    {
	n1 = typval_compare_null(tv1, tv2);
	എങ്കിൽ (n1 == MAYBE)
	    തിരികെ FAIL;
	എങ്കിൽ (type == വ്യഞ്ജകം_NEQUAL)
	    n1 = !n1;
	*res = n1;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (tv1->v_type == VAR_BLOB || tv2->v_type == VAR_BLOB)
    {
	എങ്കിൽ (typval_compare_blob(tv1, tv2, type, res) == FAIL)
	    തിരികെ FAIL;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (tv1->v_type == VAR_LIST || tv2->v_type == VAR_LIST)
    {
	എങ്കിൽ (typval_compare_list(tv1, tv2, type, ic, res) == FAIL)
	    തിരികെ FAIL;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (tv1->v_type == VAR_TUPLE || tv2->v_type == VAR_TUPLE)
    {
	എങ്കിൽ (typval_compare_tuple(tv1, tv2, type, ic, res) == FAIL)
	    തിരികെ FAIL;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (tv1->v_type == VAR_OBJECT || tv2->v_type == VAR_OBJECT)
    {
	എങ്കിൽ (typval_compare_object(tv1, tv2, type, ic, res) == FAIL)
	    തിരികെ FAIL;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (tv1->v_type == VAR_DICT || tv2->v_type == VAR_DICT)
    {
	എങ്കിൽ (typval_compare_dict(tv1, tv2, type, ic, res) == FAIL)
	    തിരികെ FAIL;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (tv1->v_type == VAR_FUNC || tv2->v_type == VAR_FUNC
	|| tv1->v_type == VAR_PARTIAL || tv2->v_type == VAR_PARTIAL)
    {
	എങ്കിൽ (typval_compare_func(tv1, tv2, type, ic, res) == FAIL)
	    തിരികെ FAIL;
    }

    // If one of the two variables is a float, compare as a float.
    // When using "=~" or "!~", always compare as string.
    അല്ലെങ്കിൽ എങ്കിൽ ((tv1->v_type == VAR_FLOAT || tv2->v_type == VAR_FLOAT)
	    && type != വ്യഞ്ജകം_MATCH && type != വ്യഞ്ജകം_NOMATCH)
    {
	float_T f1, f2;
	പൂർണ്ണസംഖ്യ	error = FALSE;

	f1 = tv_get_float_chk(tv1, &error);
	എങ്കിൽ (!error)
	    f2 = tv_get_float_chk(tv2, &error);
	എങ്കിൽ (error)
	    തിരികെ FAIL;
	n1 = FALSE;
	തിരഞ്ഞെടുപ്പ് (type)
	{
	    അവസ്ഥ വ്യഞ്ജകം_IS:
	    അവസ്ഥ വ്യഞ്ജകം_EQUAL:    n1 = (f1 == f2); നിർത്തുക;
	    അവസ്ഥ വ്യഞ്ജകം_ISNOT:
	    അവസ്ഥ വ്യഞ്ജകം_NEQUAL:   n1 = (f1 != f2); നിർത്തുക;
	    അവസ്ഥ വ്യഞ്ജകം_GREATER:  n1 = (f1 > f2); നിർത്തുക;
	    അവസ്ഥ വ്യഞ്ജകം_GEQUAL:   n1 = (f1 >= f2); നിർത്തുക;
	    അവസ്ഥ വ്യഞ്ജകം_SMALLER:  n1 = (f1 < f2); നിർത്തുക;
	    അവസ്ഥ വ്യഞ്ജകം_SEQUAL:   n1 = (f1 <= f2); നിർത്തുക;
	    അവസ്ഥ വ്യഞ്ജകം_UNKNOWN:
	    അവസ്ഥ വ്യഞ്ജകം_MATCH:
	    സ്ഥിരസ്ഥിതി:  നിർത്തുക;  // avoid gcc warning
	}
	*res = n1;
    }

    // If one of the two variables is a number, compare as a number.
    // When using "=~" or "!~", always compare as string.
    അല്ലെങ്കിൽ എങ്കിൽ ((tv1->v_type == VAR_സംഖ്യ || tv2->v_type == VAR_സംഖ്യ)
	    && type != വ്യഞ്ജകം_MATCH && type != വ്യഞ്ജകം_NOMATCH)
    {
	പൂർണ്ണസംഖ്യ error = FALSE;

	n1 = tv_get_സംഖ്യ_chk(tv1, &error);
	എങ്കിൽ (!error)
	    n2 = tv_get_സംഖ്യ_chk(tv2, &error);
	എങ്കിൽ (error)
	    തിരികെ FAIL;
	തിരഞ്ഞെടുപ്പ് (type)
	{
	    അവസ്ഥ വ്യഞ്ജകം_IS:
	    അവസ്ഥ വ്യഞ്ജകം_EQUAL:    n1 = (n1 == n2); നിർത്തുക;
	    അവസ്ഥ വ്യഞ്ജകം_ISNOT:
	    അവസ്ഥ വ്യഞ്ജകം_NEQUAL:   n1 = (n1 != n2); നിർത്തുക;
	    അവസ്ഥ വ്യഞ്ജകം_GREATER:  n1 = (n1 > n2); നിർത്തുക;
	    അവസ്ഥ വ്യഞ്ജകം_GEQUAL:   n1 = (n1 >= n2); നിർത്തുക;
	    അവസ്ഥ വ്യഞ്ജകം_SMALLER:  n1 = (n1 < n2); നിർത്തുക;
	    അവസ്ഥ വ്യഞ്ജകം_SEQUAL:   n1 = (n1 <= n2); നിർത്തുക;
	    അവസ്ഥ വ്യഞ്ജകം_UNKNOWN:
	    അവസ്ഥ വ്യഞ്ജകം_MATCH:
	    സ്ഥിരസ്ഥിതി:  നിർത്തുക;  // avoid gcc warning
	}
	*res = n1;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (in_vim9script() && (tv1->v_type == VAR_BOOL
				    || tv2->v_type == VAR_BOOL
				    || (tv1->v_type == VAR_SPECIAL
					      && tv2->v_type == VAR_SPECIAL)))
    {
	എങ്കിൽ (tv1->v_type != tv2->v_type)
	{
	    semsg(_(e_cannot_compare_str_with_str),
		       vartype_name(tv1->v_type), vartype_name(tv2->v_type));
	    തിരികെ FAIL;
	}
	n1 = tv1->vval.v_സംഖ്യ;
	n2 = tv2->vval.v_സംഖ്യ;
	തിരഞ്ഞെടുപ്പ് (type)
	{
	    അവസ്ഥ വ്യഞ്ജകം_IS:
	    അവസ്ഥ വ്യഞ്ജകം_EQUAL:    n1 = (n1 == n2); നിർത്തുക;
	    അവസ്ഥ വ്യഞ്ജകം_ISNOT:
	    അവസ്ഥ വ്യഞ്ജകം_NEQUAL:   n1 = (n1 != n2); നിർത്തുക;
	    സ്ഥിരസ്ഥിതി:
		semsg(_(e_invalid_operation_for_str),
						   vartype_name(tv1->v_type));
		തിരികെ FAIL;
	}
	*res = n1;
    }
#ifdef FEAT_JOB_CHANNEL
    അല്ലെങ്കിൽ എങ്കിൽ (tv1->v_type == tv2->v_type
	    && (tv1->v_type == VAR_CHANNEL || tv1->v_type == VAR_JOB)
	    && (type == വ്യഞ്ജകം_NEQUAL || type == വ്യഞ്ജകം_EQUAL))
    {
	എങ്കിൽ (tv1->v_type == VAR_CHANNEL)
	    n1 = tv1->vval.v_channel == tv2->vval.v_channel;
	അല്ലെങ്കിൽ
	    n1 = tv1->vval.v_job == tv2->vval.v_job;
	എങ്കിൽ (type == വ്യഞ്ജകം_NEQUAL)
	    n1 = !n1;
	*res = n1;
    }
#endif
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (typval_compare_string(tv1, tv2, type, ic, res) == FAIL)
	    തിരികെ FAIL;
    }

    തിരികെ OK;
}

/*
 * Compare "tv1" and "tv2".
 * Put the result in "tv1".  Caller should clear "tv2".
 */
    പൂർണ്ണസംഖ്യ
typval_compare(
    typval_T	*tv1,	// first operand
    typval_T	*tv2,	// second operand
    exprtype_T	type,   // operator
    പൂർണ്ണസംഖ്യ		ic)     // ignore case
{
    പൂർണ്ണസംഖ്യ res;

    എങ്കിൽ (typval_compare2(tv1, tv2, type, ic, &res) == FAIL)
    {
	clear_tv(tv1);
	തിരികെ FAIL;
    }

    clear_tv(tv1);
    എങ്കിൽ (in_vim9script())
    {
	tv1->v_type = VAR_BOOL;
	tv1->vval.v_സംഖ്യ = res ? VVAL_TRUE : VVAL_FALSE;
    }
    അല്ലെങ്കിൽ
    {
	tv1->v_type = VAR_സംഖ്യ;
	tv1->vval.v_സംഖ്യ = res;
    }

    തിരികെ OK;
}

/*
 * Compare "tv1" to "tv2" as lists according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    പൂർണ്ണസംഖ്യ
typval_compare_list(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	പൂർണ്ണസംഖ്യ	    ic,
	പൂർണ്ണസംഖ്യ	    *res)
{
    പൂർണ്ണസംഖ്യ	    val = 0;

    എങ്കിൽ (type == വ്യഞ്ജകം_IS || type == വ്യഞ്ജകം_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
				      && tv1->vval.v_list == tv2->vval.v_list);
	എങ്കിൽ (type == വ്യഞ്ജകം_ISNOT)
	    val = !val;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (tv1->v_type != tv2->v_type
	    || (type != വ്യഞ്ജകം_EQUAL && type != വ്യഞ്ജകം_NEQUAL))
    {
	എങ്കിൽ (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_list_with_list));
	അല്ലെങ്കിൽ
	    emsg(_(e_invalid_operation_for_list));
	തിരികെ FAIL;
    }
    അല്ലെങ്കിൽ
    {
	val = list_equal(tv1->vval.v_list, tv2->vval.v_list, ic);
	എങ്കിൽ (type == വ്യഞ്ജകം_NEQUAL)
	    val = !val;
    }
    *res = val;
    തിരികെ OK;
}

/*
 * Compare "tv1" to "tv2" as tuples according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    പൂർണ്ണസംഖ്യ
typval_compare_tuple(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	പൂർണ്ണസംഖ്യ	    ic,
	പൂർണ്ണസംഖ്യ	    *res)
{
    പൂർണ്ണസംഖ്യ	    val = 0;

    എങ്കിൽ (type == വ്യഞ്ജകം_IS || type == വ്യഞ്ജകം_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
			      && tv1->vval.v_tuple == tv2->vval.v_tuple);
	എങ്കിൽ (type == വ്യഞ്ജകം_ISNOT)
	    val = !val;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (tv1->v_type != tv2->v_type
	    || (type != വ്യഞ്ജകം_EQUAL && type != വ്യഞ്ജകം_NEQUAL))
    {
	എങ്കിൽ (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_tuple_with_tuple));
	അല്ലെങ്കിൽ
	    emsg(_(e_invalid_operation_for_tuple));
	തിരികെ FAIL;
    }
    അല്ലെങ്കിൽ
    {
	val = tuple_equal(tv1->vval.v_tuple, tv2->vval.v_tuple, ic);
	എങ്കിൽ (type == വ്യഞ്ജകം_NEQUAL)
	    val = !val;
    }
    *res = val;
    തിരികെ OK;
}

/*
 * Compare v:null with another type.  Return TRUE if the value is NULL.
 */
    പൂർണ്ണസംഖ്യ
typval_compare_null(typval_T *tv1, typval_T *tv2)
{
    എങ്കിൽ ((tv1->v_type == VAR_SPECIAL && tv1->vval.v_സംഖ്യ == VVAL_NULL)
	    || (tv2->v_type == VAR_SPECIAL && tv2->vval.v_സംഖ്യ == VVAL_NULL))
    {
	typval_T	*tv = tv1->v_type == VAR_SPECIAL ? tv2 : tv1;

	തിരഞ്ഞെടുപ്പ് (tv->v_type)
	{
	    അവസ്ഥ VAR_BLOB: തിരികെ tv->vval.v_blob == ശൂന്യ;
#ifdef FEAT_JOB_CHANNEL
	    അവസ്ഥ VAR_CHANNEL: തിരികെ tv->vval.v_channel == ശൂന്യ;
#endif
	    // TODO: null_class handling
	    // case VAR_CLASS: return tv->vval.v_class == NULL;
	    അവസ്ഥ VAR_DICT: തിരികെ tv->vval.v_dict == ശൂന്യ;
	    അവസ്ഥ VAR_FUNC: തിരികെ tv->vval.v_string == ശൂന്യ;
#ifdef FEAT_JOB_CHANNEL
	    അവസ്ഥ VAR_JOB: തിരികെ tv->vval.v_job == ശൂന്യ;
#endif
	    അവസ്ഥ VAR_LIST: തിരികെ tv->vval.v_list == ശൂന്യ;
	    അവസ്ഥ VAR_TUPLE: തിരികെ tv->vval.v_tuple == ശൂന്യ;
	    അവസ്ഥ VAR_OBJECT: തിരികെ tv->vval.v_object == ശൂന്യ;
	    അവസ്ഥ VAR_PARTIAL: തിരികെ tv->vval.v_partial == ശൂന്യ;
	    അവസ്ഥ VAR_STRING: തിരികെ tv->vval.v_string == ശൂന്യ;

	    അവസ്ഥ VAR_സംഖ്യ: എങ്കിൽ (!in_vim9script())
				 തിരികെ tv->vval.v_സംഖ്യ == 0;
			     നിർത്തുക;
	    അവസ്ഥ VAR_FLOAT: എങ്കിൽ (!in_vim9script())
				 തിരികെ tv->vval.v_float == 0.0;
			     നിർത്തുക;
	    അവസ്ഥ VAR_TYPEALIAS: തിരികെ tv->vval.v_typealias == ശൂന്യ;
	    സ്ഥിരസ്ഥിതി: നിർത്തുക;
	}
    }
    // although comparing null with number, float or bool is not very useful
    // we won't give an error
    തിരികെ FALSE;
}

/*
 * Compare "tv1" to "tv2" as blobs according to "type".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    പൂർണ്ണസംഖ്യ
typval_compare_blob(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	പൂർണ്ണസംഖ്യ	    *res)
{
    പൂർണ്ണസംഖ്യ	    val = 0;

    എങ്കിൽ (type == വ്യഞ്ജകം_IS || type == വ്യഞ്ജകം_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
			&& tv1->vval.v_blob == tv2->vval.v_blob);
	എങ്കിൽ (type == വ്യഞ്ജകം_ISNOT)
	    val = !val;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (tv1->v_type != tv2->v_type
	    || (type != വ്യഞ്ജകം_EQUAL && type != വ്യഞ്ജകം_NEQUAL))
    {
	എങ്കിൽ (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_blob_with_blob));
	അല്ലെങ്കിൽ
	    emsg(_(e_invalid_operation_for_blob));
	തിരികെ FAIL;
    }
    അല്ലെങ്കിൽ
    {
	val = blob_equal(tv1->vval.v_blob, tv2->vval.v_blob);
	എങ്കിൽ (type == വ്യഞ്ജകം_NEQUAL)
	    val = !val;
    }
    *res = val;
    തിരികെ OK;
}

/*
 * Compare "tv1" to "tv2" as objects according to "type".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    പൂർണ്ണസംഖ്യ
typval_compare_object(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	പൂർണ്ണസംഖ്യ	    ic,
	പൂർണ്ണസംഖ്യ	    *res)
{
    പൂർണ്ണസംഖ്യ res_match = type == വ്യഞ്ജകം_EQUAL || type == വ്യഞ്ജകം_IS ? TRUE : FALSE;

    എങ്കിൽ (tv1->vval.v_object == ശൂന്യ && tv2->vval.v_object == ശൂന്യ)
    {
	*res = res_match;
	തിരികെ OK;
    }
    എങ്കിൽ (tv1->vval.v_object == ശൂന്യ || tv2->vval.v_object == ശൂന്യ)
    {
	*res = !res_match;
	തിരികെ OK;
    }

    object_T *obj1 = tv1->vval.v_object;
    object_T *obj2 = tv2->vval.v_object;
    എങ്കിൽ (type == വ്യഞ്ജകം_IS || type == വ്യഞ്ജകം_ISNOT)
    {
	*res = obj1 == obj2 ? res_match : !res_match;
	തിരികെ OK;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (tv1->v_type != tv2->v_type)
    {
	emsg(_(e_can_only_compare_object_with_object));
	തിരികെ FAIL;
    }

    *res = object_equal(obj1, obj2, ic) ? res_match : !res_match;
    തിരികെ OK;
}

/*
 * Compare "tv1" to "tv2" as dictionaries according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    പൂർണ്ണസംഖ്യ
typval_compare_dict(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	പൂർണ്ണസംഖ്യ	    ic,
	പൂർണ്ണസംഖ്യ	    *res)
{
    പൂർണ്ണസംഖ്യ	    val;

    എങ്കിൽ (type == വ്യഞ്ജകം_IS || type == വ്യഞ്ജകം_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
			&& tv1->vval.v_dict == tv2->vval.v_dict);
	എങ്കിൽ (type == വ്യഞ്ജകം_ISNOT)
	    val = !val;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (tv1->v_type != tv2->v_type
		|| (type != വ്യഞ്ജകം_EQUAL && type != വ്യഞ്ജകം_NEQUAL))
    {
	എങ്കിൽ (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_dictionary_with_dictionary));
	അല്ലെങ്കിൽ
	    emsg(_(e_invalid_operation_for_dictionary));
	തിരികെ FAIL;
    }
    അല്ലെങ്കിൽ
    {
	val = dict_equal(tv1->vval.v_dict, tv2->vval.v_dict, ic);
	എങ്കിൽ (type == വ്യഞ്ജകം_NEQUAL)
	    val = !val;
    }
    *res = val;
    തിരികെ OK;
}

/*
 * Compare "tv1" to "tv2" as funcrefs according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    പൂർണ്ണസംഖ്യ
typval_compare_func(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	പൂർണ്ണസംഖ്യ	    ic,
	പൂർണ്ണസംഖ്യ	    *res)
{
    പൂർണ്ണസംഖ്യ	    val = 0;

    എങ്കിൽ (type != വ്യഞ്ജകം_EQUAL && type != വ്യഞ്ജകം_NEQUAL
	    && type != വ്യഞ്ജകം_IS && type != വ്യഞ്ജകം_ISNOT)
    {
	emsg(_(e_invalid_operation_for_funcrefs));
	തിരികെ FAIL;
    }
    എങ്കിൽ ((tv1->v_type == VAR_PARTIAL && tv1->vval.v_partial == ശൂന്യ)
	    || (tv2->v_type == VAR_PARTIAL && tv2->vval.v_partial == ശൂന്യ))
	// When both partials are NULL, then they are equal.
	// Otherwise they are not equal.
	val = (tv1->vval.v_partial == tv2->vval.v_partial);
    അല്ലെങ്കിൽ എങ്കിൽ (type == വ്യഞ്ജകം_IS || type == വ്യഞ്ജകം_ISNOT)
    {
	എങ്കിൽ (tv1->v_type == VAR_FUNC && tv2->v_type == VAR_FUNC)
	    // strings are considered the same if their value is
	    // the same
	    val = tv_equal(tv1, tv2, ic);
	അല്ലെങ്കിൽ എങ്കിൽ (tv1->v_type == VAR_PARTIAL && tv2->v_type == VAR_PARTIAL)
	    val = (tv1->vval.v_partial == tv2->vval.v_partial);
	അല്ലെങ്കിൽ
	    val = FALSE;
    }
    അല്ലെങ്കിൽ
	val = tv_equal(tv1, tv2, ic);
    എങ്കിൽ (type == വ്യഞ്ജകം_NEQUAL || type == വ്യഞ്ജകം_ISNOT)
	val = !val;
    *res = val;
    തിരികെ OK;
}

/*
 * Compare "tv1" to "tv2" as strings according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    പൂർണ്ണസംഖ്യ
typval_compare_string(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	പൂർണ്ണസംഖ്യ	    ic,
	പൂർണ്ണസംഖ്യ	    *res)
{
    പൂർണ്ണസംഖ്യ		i = 0;
    പൂർണ്ണസംഖ്യ		val = FALSE;
    char_u	*s1, *s2;
    char_u	buf1[NUMBUFLEN], buf2[NUMBUFLEN];

    എങ്കിൽ (in_vim9script()
	  && ((tv1->v_type != VAR_STRING && tv1->v_type != VAR_SPECIAL)
	   || (tv2->v_type != VAR_STRING && tv2->v_type != VAR_SPECIAL)))
    {
	semsg(_(e_cannot_compare_str_with_str),
		   vartype_name(tv1->v_type), vartype_name(tv2->v_type));
	തിരികെ FAIL;
    }
    s1 = tv_get_string_buf(tv1, buf1);
    s2 = tv_get_string_buf(tv2, buf2);
    എങ്കിൽ (type != വ്യഞ്ജകം_MATCH && type != വ്യഞ്ജകം_NOMATCH)
	i = ic ? MB_STRICMP(s1, s2) : STRCMP(s1, s2);
    തിരഞ്ഞെടുപ്പ് (type)
    {
	അവസ്ഥ വ്യഞ്ജകം_IS:	    എങ്കിൽ (in_vim9script())
			    {
				// Really check it is the same string, not just
				// the same value.
				val = tv1->vval.v_string == tv2->vval.v_string;
				നിർത്തുക;
			    }
			    // FALLTHROUGH
	അവസ്ഥ വ്യഞ്ജകം_EQUAL:    val = (i == 0); നിർത്തുക;
	അവസ്ഥ വ്യഞ്ജകം_ISNOT:    എങ്കിൽ (in_vim9script())
			    {
				// Really check it is not the same string, not
				// just a different value.
				val = tv1->vval.v_string != tv2->vval.v_string;
				നിർത്തുക;
			    }
			    // FALLTHROUGH
	അവസ്ഥ വ്യഞ്ജകം_NEQUAL:   val = (i != 0); നിർത്തുക;
	അവസ്ഥ വ്യഞ്ജകം_GREATER:  val = (i > 0); നിർത്തുക;
	അവസ്ഥ വ്യഞ്ജകം_GEQUAL:   val = (i >= 0); നിർത്തുക;
	അവസ്ഥ വ്യഞ്ജകം_SMALLER:  val = (i < 0); നിർത്തുക;
	അവസ്ഥ വ്യഞ്ജകം_SEQUAL:   val = (i <= 0); നിർത്തുക;

	അവസ്ഥ വ്യഞ്ജകം_MATCH:
	അവസ്ഥ വ്യഞ്ജകം_NOMATCH:
		val = pattern_match(s2, s1, ic);
		എങ്കിൽ (type == വ്യഞ്ജകം_NOMATCH)
		    val = !val;
		നിർത്തുക;

	സ്ഥിരസ്ഥിതി:  നിർത്തുക;  // avoid gcc warning
    }
    *res = val;
    തിരികെ OK;
}
/*
 * Convert any type to a string, never give an error.
 * When "quotes" is TRUE add quotes to a string.
 * Returns an allocated string.
 */
    char_u *
typval_tostring(typval_T *arg, പൂർണ്ണസംഖ്യ quotes)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    char_u	*ret = ശൂന്യ;

    എങ്കിൽ (arg == ശൂന്യ)
	തിരികെ vim_strsave((char_u *)"(does not exist)");
    എങ്കിൽ (!quotes && arg->v_type == VAR_STRING)
    {
	ret = vim_strsave(arg->vval.v_string == ശൂന്യ ? (char_u *)""
							 : arg->vval.v_string);
    }
    അല്ലെങ്കിൽ
    {
	ret = tv2string(arg, &tofree, numbuf, 0);
	// Make a copy if we have a value but it's not in allocated memory.
	എങ്കിൽ (ret != ശൂന്യ && tofree == ശൂന്യ)
	    ret = vim_strsave(ret);
    }
    തിരികെ ret;
}

/*
 * Return TRUE if typeval "tv" is locked: Either that value is locked itself
 * or it refers to a List or Dictionary that is locked.
 */
    പൂർണ്ണസംഖ്യ
tv_islocked(typval_T *tv)
{
    തിരികെ (tv->v_lock & VAR_LOCKED)
	|| (tv->v_type == VAR_LIST
		&& tv->vval.v_list != ശൂന്യ
		&& (tv->vval.v_list->lv_lock & VAR_LOCKED))
	|| (tv->v_type == VAR_TUPLE
		&& tv->vval.v_tuple != ശൂന്യ
		&& (tv->vval.v_tuple->tv_lock & VAR_LOCKED))
	|| (tv->v_type == VAR_DICT
		&& tv->vval.v_dict != ശൂന്യ
		&& (tv->vval.v_dict->dv_lock & VAR_LOCKED));
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
func_equal(
    typval_T *tv1,
    typval_T *tv2,
    പൂർണ്ണസംഖ്യ	     ic)	    // ignore case
{
    char_u	*s1, *s2;
    dict_T	*d1, *d2;
    പൂർണ്ണസംഖ്യ		a1, a2;
    പൂർണ്ണസംഖ്യ		i;

    // empty and NULL function name considered the same
    s1 = tv1->v_type == VAR_FUNC ? tv1->vval.v_string
					   : partial_name(tv1->vval.v_partial);
    എങ്കിൽ (s1 != ശൂന്യ && *s1 == NUL)
	s1 = ശൂന്യ;
    s2 = tv2->v_type == VAR_FUNC ? tv2->vval.v_string
					   : partial_name(tv2->vval.v_partial);
    എങ്കിൽ (s2 != ശൂന്യ && *s2 == NUL)
	s2 = ശൂന്യ;
    എങ്കിൽ (s1 == ശൂന്യ || s2 == ശൂന്യ)
    {
	എങ്കിൽ (s1 != s2)
	    തിരികെ FALSE;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(s1, s2) != 0)
	തിരികെ FALSE;

    // empty dict and NULL dict is different
    d1 = tv1->v_type == VAR_FUNC ? ശൂന്യ : tv1->vval.v_partial->pt_dict;
    d2 = tv2->v_type == VAR_FUNC ? ശൂന്യ : tv2->vval.v_partial->pt_dict;
    എങ്കിൽ (d1 == ശൂന്യ || d2 == ശൂന്യ)
    {
	എങ്കിൽ (d1 != d2)
	    തിരികെ FALSE;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (!dict_equal(d1, d2, ic))
	തിരികെ FALSE;

    // empty list and no list considered the same
    a1 = tv1->v_type == VAR_FUNC ? 0 : tv1->vval.v_partial->pt_argc;
    a2 = tv2->v_type == VAR_FUNC ? 0 : tv2->vval.v_partial->pt_argc;
    എങ്കിൽ (a1 != a2)
	തിരികെ FALSE;
    വേണ്ടി (i = 0; i < a1; ++i)
	എങ്കിൽ (!tv_equal(tv1->vval.v_partial->pt_argv + i,
		      tv2->vval.v_partial->pt_argv + i, ic))
	    തിരികെ FALSE;

    തിരികെ TRUE;
}

/*
 * Return TRUE if "tv1" and "tv2" have the same value.
 * Compares the items just like "==" would compare them, but strings and
 * numbers are different.  Floats and numbers are also different.
 */
    പൂർണ്ണസംഖ്യ
tv_equal(
    typval_T *tv1,
    typval_T *tv2,
    പൂർണ്ണസംഖ്യ	     ic)	    // ignore case
{
    char_u	buf1[NUMBUFLEN], buf2[NUMBUFLEN];
    char_u	*s1, *s2;
    സ്ഥിതം പൂർണ്ണസംഖ്യ  recursive_cnt = 0;	    // catch recursive loops
    പൂർണ്ണസംഖ്യ		r;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	tv_equal_recurse_limit;

    // Catch lists and dicts that have an endless loop by limiting
    // recursiveness to a limit.  We guess they are equal then.
    // A fixed limit has the problem of still taking an awful long time.
    // Reduce the limit every time running into it. That should work fine for
    // deeply linked structures that are not recursively linked and catch
    // recursiveness quickly.
    എങ്കിൽ (recursive_cnt == 0)
	tv_equal_recurse_limit = 1000;
    എങ്കിൽ (recursive_cnt >= tv_equal_recurse_limit)
    {
	--tv_equal_recurse_limit;
	തിരികെ TRUE;
    }

    // For VAR_FUNC and VAR_PARTIAL compare the function name, bound dict and
    // arguments.
    എങ്കിൽ ((tv1->v_type == VAR_FUNC
		|| (tv1->v_type == VAR_PARTIAL && tv1->vval.v_partial != ശൂന്യ))
	    && (tv2->v_type == VAR_FUNC
		|| (tv2->v_type == VAR_PARTIAL && tv2->vval.v_partial != ശൂന്യ)))
    {
	++recursive_cnt;
	r = func_equal(tv1, tv2, ic);
	--recursive_cnt;
	തിരികെ r;
    }

    എങ്കിൽ (tv1->v_type != tv2->v_type
	    && ((tv1->v_type != VAR_BOOL && tv1->v_type != VAR_SPECIAL)
		|| (tv2->v_type != VAR_BOOL && tv2->v_type != VAR_SPECIAL)))
	തിരികെ FALSE;

    തിരഞ്ഞെടുപ്പ് (tv1->v_type)
    {
	അവസ്ഥ VAR_LIST:
	    ++recursive_cnt;
	    r = list_equal(tv1->vval.v_list, tv2->vval.v_list, ic);
	    --recursive_cnt;
	    തിരികെ r;

	അവസ്ഥ VAR_TUPLE:
	    ++recursive_cnt;
	    r = tuple_equal(tv1->vval.v_tuple, tv2->vval.v_tuple, ic);
	    --recursive_cnt;
	    തിരികെ r;

	അവസ്ഥ VAR_DICT:
	    ++recursive_cnt;
	    r = dict_equal(tv1->vval.v_dict, tv2->vval.v_dict, ic);
	    --recursive_cnt;
	    തിരികെ r;

	അവസ്ഥ VAR_BLOB:
	    തിരികെ blob_equal(tv1->vval.v_blob, tv2->vval.v_blob);

	അവസ്ഥ VAR_സംഖ്യ:
	അവസ്ഥ VAR_BOOL:
	അവസ്ഥ VAR_SPECIAL:
	    തിരികെ tv1->vval.v_സംഖ്യ == tv2->vval.v_സംഖ്യ;

	അവസ്ഥ VAR_STRING:
	    s1 = tv_get_string_buf(tv1, buf1);
	    s2 = tv_get_string_buf(tv2, buf2);
	    തിരികെ ((ic ? MB_STRICMP(s1, s2) : STRCMP(s1, s2)) == 0);

	അവസ്ഥ VAR_FLOAT:
	    തിരികെ tv1->vval.v_float == tv2->vval.v_float;
	അവസ്ഥ VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    തിരികെ tv1->vval.v_job == tv2->vval.v_job;
#endif
	അവസ്ഥ VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    തിരികെ tv1->vval.v_channel == tv2->vval.v_channel;
#endif
	അവസ്ഥ VAR_INSTR:
	    തിരികെ tv1->vval.v_instr == tv2->vval.v_instr;

	അവസ്ഥ VAR_CLASS:
	    // A class only exists once, equality is identity.
	    തിരികെ tv1->vval.v_class == tv2->vval.v_class;

	അവസ്ഥ VAR_OBJECT:
	    ++recursive_cnt;
	    r = object_equal(tv1->vval.v_object, tv2->vval.v_object, ic);
	    --recursive_cnt;
	    തിരികെ r;

	അവസ്ഥ VAR_PARTIAL:
	    തിരികെ tv1->vval.v_partial == tv2->vval.v_partial;

	അവസ്ഥ VAR_FUNC:
	    തിരികെ tv1->vval.v_string == tv2->vval.v_string;

	അവസ്ഥ VAR_TYPEALIAS:
	    തിരികെ tv1->vval.v_typealias == tv2->vval.v_typealias;

	അവസ്ഥ VAR_UNKNOWN:
	അവസ്ഥ VAR_ANY:
	അവസ്ഥ VAR_VOID:
	    നിർത്തുക;
    }

    // VAR_UNKNOWN can be the result of a invalid expression, let's say it
    // does not equal anything, not even itself.
    തിരികെ FALSE;
}

/*
 * Get an option value.
 * "arg" points to the '&' or '+' before the option name.
 * "arg" is advanced to character after the option name.
 * Return OK or FAIL.
 */
    പൂർണ്ണസംഖ്യ
eval_option(
    char_u	**arg,
    typval_T	*rettv,	// when NULL, only check if option exists
    പൂർണ്ണസംഖ്യ		evaluate)
{
    char_u	*option_end;
    long	numval;
    char_u	*stringval;
    getoption_T	opt_type;
    പൂർണ്ണസംഖ്യ		c;
    പൂർണ്ണസംഖ്യ		working = (**arg == '+');    // has("+option")
    പൂർണ്ണസംഖ്യ		ret = OK;
    പൂർണ്ണസംഖ്യ		scope;

    // Isolate the option name and find its value.
    option_end = find_option_end(arg, &scope);
    എങ്കിൽ (option_end == ശൂന്യ)
    {
	എങ്കിൽ (rettv != ശൂന്യ)
	    semsg(_(e_option_name_missing_str), *arg);
	തിരികെ FAIL;
    }

    എങ്കിൽ (!evaluate)
    {
	*arg = option_end;
	തിരികെ OK;
    }

    c = *option_end;
    *option_end = NUL;
    opt_type = get_option_മൂല്യം(*arg, &numval,
			       rettv == ശൂന്യ ? ശൂന്യ : &stringval, ശൂന്യ, scope);

    എങ്കിൽ (opt_type == gov_unknown)
    {
	എങ്കിൽ (rettv != ശൂന്യ)
	    semsg(_(e_unknown_option_str), *arg);
	ret = FAIL;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (rettv != ശൂന്യ)
    {
	rettv->v_lock = 0;
	എങ്കിൽ (opt_type == gov_hidden_string)
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = ശൂന്യ;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (opt_type == gov_hidden_bool || opt_type == gov_hidden_സംഖ്യ)
	{
	    rettv->v_type = in_vim9script() && opt_type == gov_hidden_bool
						       ? VAR_BOOL : VAR_സംഖ്യ;
	    rettv->vval.v_സംഖ്യ = 0;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (opt_type == gov_bool || opt_type == gov_സംഖ്യ)
	{
	    എങ്കിൽ (in_vim9script() && opt_type == gov_bool)
	    {
		rettv->v_type = VAR_BOOL;
		rettv->vval.v_സംഖ്യ = numval ? VVAL_TRUE : VVAL_FALSE;
	    }
	    അല്ലെങ്കിൽ
	    {
		rettv->v_type = VAR_സംഖ്യ;
		rettv->vval.v_സംഖ്യ = numval;
	    }
	}
	അല്ലെങ്കിൽ				// string option
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = stringval;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (working && (opt_type == gov_hidden_bool
			|| opt_type == gov_hidden_സംഖ്യ
			|| opt_type == gov_hidden_string))
	ret = FAIL;

    *option_end = c;		    // put back for error messages
    *arg = option_end;

    തിരികെ ret;
}

/*
 * Allocate a variable for a number constant.  Also deals with "0z" for blob.
 * Return OK or FAIL.
 */
    പൂർണ്ണസംഖ്യ
eval_സംഖ്യ(
	char_u	    **arg,
	typval_T    *rettv,
	പൂർണ്ണസംഖ്യ	    evaluate,
	പൂർണ്ണസംഖ്യ	    want_string)
{
    പൂർണ്ണസംഖ്യ		len;
    പൂർണ്ണസംഖ്യ		skip_quotes = !in_old_script(4);
    char_u	*p;
    പൂർണ്ണസംഖ്യ		get_float = FALSE;

    // We accept a float when the format matches
    // "[0-9]\+\.[0-9]\+\([eE][+-]\?[0-9]\+\)\?".  This is very
    // strict to avoid backwards compatibility problems.
    // With script version 2 and later the leading digit can be
    // omitted.
    // Don't look for a float after the "." operator, so that
    // ":let vers = 1.2.3" doesn't fail.
    എങ്കിൽ (**arg == '.')
	p = *arg;
    അല്ലെങ്കിൽ
    {
	p = *arg + 1;
	എങ്കിൽ (skip_quotes)
	    വേണ്ടി (;;)
	    {
		എങ്കിൽ (*p == '\'')
		    ++p;
		എങ്കിൽ (!vim_isdigit(*p))
		    നിർത്തുക;
		p = skipdigits(p);
	    }
	അല്ലെങ്കിൽ
	    p = skipdigits(p);
    }
    എങ്കിൽ (!want_string && p[0] == '.' && vim_isdigit(p[1]))
    {
	get_float = TRUE;
	p = skipdigits(p + 2);
	എങ്കിൽ (*p == 'e' || *p == 'E')
	{
	    ++p;
	    എങ്കിൽ (*p == '-' || *p == '+')
		++p;
	    എങ്കിൽ (!vim_isdigit(*p))
		get_float = FALSE;
	    അല്ലെങ്കിൽ
		p = skipdigits(p + 1);
	}
	എങ്കിൽ (ASCII_ISALPHA(*p) || *p == '.')
	    get_float = FALSE;
    }
    എങ്കിൽ (get_float)
    {
	float_T	f;

	*arg += string2float(*arg, &f, skip_quotes);
	എങ്കിൽ (evaluate)
	{
	    rettv->v_type = VAR_FLOAT;
	    rettv->vval.v_float = f;
	}
    }
    അല്ലെങ്കിൽ
    എങ്കിൽ (**arg == '0' && ((*arg)[1] == 'z' || (*arg)[1] == 'Z'))
    {
	char_u  *bp;
	blob_T  *blob = ശൂന്യ;  // init for gcc

	// Blob constant: 0z0123456789abcdef
	എങ്കിൽ (evaluate)
	    blob = blob_alloc();
	വേണ്ടി (bp = *arg + 2; vim_isxdigit(bp[0]); bp += 2)
	{
	    എങ്കിൽ (!vim_isxdigit(bp[1]))
	    {
		എങ്കിൽ (blob != ശൂന്യ)
		{
		    emsg(_(e_blob_literal_should_have_an_even_സംഖ്യ_of_hex_characters));
		    ga_clear(&blob->bv_ga);
		    VIM_CLEAR(blob);
		}
		തിരികെ FAIL;
	    }
	    എങ്കിൽ (blob != ശൂന്യ)
		ga_append(&blob->bv_ga,
			     (hex2nr(*bp) << 4) + hex2nr(*(bp+1)));
	    എങ്കിൽ (bp[2] == '.' && vim_isxdigit(bp[3]))
		++bp;
	}
	എങ്കിൽ (blob != ശൂന്യ)
	    rettv_blob_set(rettv, blob);
	*arg = bp;
    }
    അല്ലെങ്കിൽ
    {
	varnumber_T	n;

	// decimal, hex or octal number
	vim_str2nr(*arg, ശൂന്യ, &len, skip_quotes
		      ? STR2NR_NO_OCT + STR2NR_QUOTE
		      : STR2NR_ALL, &n, ശൂന്യ, 0, TRUE, ശൂന്യ);
	എങ്കിൽ (len == 0)
	{
	    എങ്കിൽ (evaluate)
		semsg(_(e_invalid_expression_str), *arg);
	    തിരികെ FAIL;
	}
	*arg += len;
	എങ്കിൽ (evaluate)
	{
	    rettv->v_type = VAR_സംഖ്യ;
	    rettv->vval.v_സംഖ്യ = n;
	}
    }
    തിരികെ OK;
}

/*
 * Evaluate a string constant and put the result in "rettv".
 * "*arg" points to the double quote or to after it when "interpolate" is TRUE.
 * When "interpolate" is TRUE reduce "{{" to "{", reduce "}}" to "}" and stop
 * at a single "{".
 * Return OK or FAIL.
 */
    പൂർണ്ണസംഖ്യ
eval_string(char_u **arg, typval_T *rettv, പൂർണ്ണസംഖ്യ evaluate, പൂർണ്ണസംഖ്യ interpolate)
{
    char_u	*p;
    char_u	*end;
    പൂർണ്ണസംഖ്യ		extra = interpolate ? 1 : 0;
    പൂർണ്ണസംഖ്യ		off = interpolate ? 0 : 1;
    പൂർണ്ണസംഖ്യ		len;

    // Find the end of the string, skipping backslashed characters.
    വേണ്ടി (p = *arg + off; *p != NUL && *p != '"'; MB_PTR_ADV(p))
    {
	എങ്കിൽ (*p == '\\' && p[1] != NUL)
	{
	    ++p;
	    // A "\<x>" form occupies at least 4 characters, and produces up
	    // to 9 characters (6 for the char and 3 for a modifier):
	    // reserve space for 5 extra.
	    എങ്കിൽ (*p == '<')
	    {
		പൂർണ്ണസംഖ്യ		modifiers = 0;
		പൂർണ്ണസംഖ്യ		flags = FSK_KEYCODE | FSK_IN_STRING;

		extra += 5;

		// Skip to the '>' to avoid using '{' inside for string
		// interpolation.
		എങ്കിൽ (p[1] != '*')
		    flags |= FSK_SIMPLIFY;
		എങ്കിൽ (find_special_താക്കോൽ(&p, &modifiers, flags, ശൂന്യ) != 0)
		    --p;  // leave "p" on the ">"
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (interpolate && (*p == '{' || *p == '}'))
	{
	    എങ്കിൽ (*p == '{' && p[1] != '{') // start of expression
		നിർത്തുക;
	    ++p;
	    എങ്കിൽ (p[-1] == '}' && *p != '}') // single '}' is an error
	    {
		semsg(_(e_stray_closing_curly_str), *arg);
		തിരികെ FAIL;
	    }
	    --extra;  // "{{" becomes "{", "}}" becomes "}"
	}
    }

    എങ്കിൽ (*p != '"' && !(interpolate && *p == '{'))
    {
	semsg(_(e_missing_double_quote_str), *arg);
	തിരികെ FAIL;
    }

    // If only parsing, set *arg and return here
    എങ്കിൽ (!evaluate)
    {
	*arg = p + off;
	തിരികെ OK;
    }

    // Copy the string into allocated memory, handling backslashed
    // characters.
    rettv->v_type = VAR_STRING;
    len = (പൂർണ്ണസംഖ്യ)(p - *arg + extra);
    rettv->vval.v_string = alloc(len);
    എങ്കിൽ (rettv->vval.v_string == ശൂന്യ)
	തിരികെ FAIL;
    end = rettv->vval.v_string;

    വേണ്ടി (p = *arg + off; *p != NUL && *p != '"'; )
    {
	എങ്കിൽ (*p == '\\')
	{
	    തിരഞ്ഞെടുപ്പ് (*++p)
	    {
		അവസ്ഥ 'b': *end++ = BS; ++p; നിർത്തുക;
		അവസ്ഥ 'e': *end++ = ESC; ++p; നിർത്തുക;
		അവസ്ഥ 'f': *end++ = FF; ++p; നിർത്തുക;
		അവസ്ഥ 'n': *end++ = NL; ++p; നിർത്തുക;
		അവസ്ഥ 'r': *end++ = CAR; ++p; നിർത്തുക;
		അവസ്ഥ 't': *end++ = TAB; ++p; നിർത്തുക;

		അവസ്ഥ 'X': // hex: "\x1", "\x12"
		അവസ്ഥ 'x':
		അവസ്ഥ 'u': // Unicode: "\u0023"
		അവസ്ഥ 'U':
			  എങ്കിൽ (vim_isxdigit(p[1]))
			  {
			      പൂർണ്ണസംഖ്യ	n, nr;
			      പൂർണ്ണസംഖ്യ	c = SAFE_toupper(*p);

			      എങ്കിൽ (c == 'X')
				  n = 2;
			      അല്ലെങ്കിൽ എങ്കിൽ (*p == 'u')
				  n = 4;
			      അല്ലെങ്കിൽ
				  n = 8;
			      nr = 0;
			      ആകുമ്പോൾ (--n >= 0 && vim_isxdigit(p[1]))
			      {
				  ++p;
				  nr = (nr << 4) + hex2nr(*p);
			      }
			      ++p;
			      // For "\u" store the number according to
			      // 'encoding'.
			      എങ്കിൽ (c != 'X')
				  end += (*mb_char2bytes)(nr, end);
			      അല്ലെങ്കിൽ
				  *end++ = nr;
			  }
			  നിർത്തുക;

			  // octal: "\1", "\12", "\123"
		അവസ്ഥ '0':
		അവസ്ഥ '1':
		അവസ്ഥ '2':
		അവസ്ഥ '3':
		അവസ്ഥ '4':
		അവസ്ഥ '5':
		അവസ്ഥ '6':
		അവസ്ഥ '7': *end = *p++ - '0';
			  എങ്കിൽ (*p >= '0' && *p <= '7')
			  {
			      *end = (*end << 3) + *p++ - '0';
			      എങ്കിൽ (*p >= '0' && *p <= '7')
				  *end = (*end << 3) + *p++ - '0';
			  }
			  ++end;
			  നിർത്തുക;

			  // Special key, e.g.: "\<C-W>"
		അവസ്ഥ '<':
			  {
			      പൂർണ്ണസംഖ്യ flags = FSK_KEYCODE | FSK_IN_STRING;

			      എങ്കിൽ (p[1] != '*')
				  flags |= FSK_SIMPLIFY;
			      extra = trans_special(&p, end, flags, FALSE, ശൂന്യ);
			      എങ്കിൽ (extra != 0)
			      {
				  end += extra;
				  എങ്കിൽ (end >= rettv->vval.v_string + len)
				      iemsg("eval_string() used more space than allocated");
				  നിർത്തുക;
			      }
			  }
			  // FALLTHROUGH

		സ്ഥിരസ്ഥിതി: MB_COPY_CHAR(p, end);
			  നിർത്തുക;
	    }
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (interpolate && (*p == '{' || *p == '}'))
	    {
		എങ്കിൽ (*p == '{' && p[1] != '{') // start of expression
		    നിർത്തുക;
		++p;  // reduce "{{" to "{" and "}}" to "}"
	    }
	    MB_COPY_CHAR(p, end);
	}
    }
    *end = NUL;
    എങ്കിൽ (*p == '"' && !interpolate)
	++p;
    *arg = p;

    തിരികെ OK;
}

/*
 * Allocate a variable for a 'str''ing' constant.
 * When "interpolate" is TRUE reduce "{{" to "{" and stop at a single "{".
 * Return OK when a "rettv" was set to the string.
 * Return FAIL on error, "rettv" is not set.
 */
    പൂർണ്ണസംഖ്യ
eval_lit_string(char_u **arg, typval_T *rettv, പൂർണ്ണസംഖ്യ evaluate, പൂർണ്ണസംഖ്യ interpolate)
{
    char_u	*p;
    char_u	*str;
    പൂർണ്ണസംഖ്യ		reduce = interpolate ? -1 : 0;
    പൂർണ്ണസംഖ്യ		off = interpolate ? 0 : 1;

    // Find the end of the string, skipping ''.
    വേണ്ടി (p = *arg + off; *p != NUL; MB_PTR_ADV(p))
    {
	എങ്കിൽ (*p == '\'')
	{
	    എങ്കിൽ (p[1] != '\'')
		നിർത്തുക;
	    ++reduce;
	    ++p;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (interpolate)
	{
	    എങ്കിൽ (*p == '{')
	    {
		എങ്കിൽ (p[1] != '{')
		    നിർത്തുക;
		++p;
		++reduce;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (*p == '}')
	    {
		++p;
		എങ്കിൽ (*p != '}')
		{
		    semsg(_(e_stray_closing_curly_str), *arg);
		    തിരികെ FAIL;
		}
		++reduce;
	    }
	}
    }

    എങ്കിൽ (*p != '\'' && !(interpolate && *p == '{'))
    {
	semsg(_(e_missing_single_quote_str), *arg);
	തിരികെ FAIL;
    }

    // If only parsing return after setting "*arg"
    എങ്കിൽ (!evaluate)
    {
	*arg = p + off;
	തിരികെ OK;
    }

    // Copy the string into allocated memory, handling '' to ' reduction and
    // any expressions.
    str = alloc((p - *arg) - reduce);
    എങ്കിൽ (str == ശൂന്യ)
	തിരികെ FAIL;
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = str;

    വേണ്ടി (p = *arg + off; *p != NUL; )
    {
	എങ്കിൽ (*p == '\'')
	{
	    എങ്കിൽ (p[1] != '\'')
		നിർത്തുക;
	    ++p;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (interpolate && (*p == '{' || *p == '}'))
	{
	    എങ്കിൽ (*p == '{' && p[1] != '{')
		നിർത്തുക;
	    ++p;
	}
	MB_COPY_CHAR(p, str);
    }
    *str = NUL;
    *arg = p + off;

    തിരികെ OK;
}

/*
 * Evaluate a single or double quoted string possibly containing expressions.
 * "arg" points to the '$'.  The result is put in "rettv".
 * Returns OK or FAIL.
 */
    പൂർണ്ണസംഖ്യ
eval_interp_string(char_u **arg, typval_T *rettv, പൂർണ്ണസംഖ്യ evaluate)
{
    typval_T	tv;
    പൂർണ്ണസംഖ്യ		ret = OK;
    പൂർണ്ണസംഖ്യ		quote;
    garray_T	ga;
    char_u	*p;

    ga_init2(&ga, 1, 80);

    // *arg is on the '$' character, move it to the first string character.
    ++*arg;
    quote = **arg;
    ++*arg;

    വേണ്ടി (;;)
    {
	// Get the string up to the matching quote or to a single '{'.
	// "arg" is advanced to either the quote or the '{'.
	എങ്കിൽ (quote == '"')
	    ret = eval_string(arg, &tv, evaluate, TRUE);
	അല്ലെങ്കിൽ
	    ret = eval_lit_string(arg, &tv, evaluate, TRUE);
	എങ്കിൽ (ret == FAIL)
	    നിർത്തുക;
	എങ്കിൽ (evaluate)
	{
	    ga_concat(&ga, tv.vval.v_string);
	    clear_tv(&tv);
	}

	എങ്കിൽ (**arg != '{')
	{
	    // found terminating quote
	    ++*arg;
	    നിർത്തുക;
	}
	p = eval_one_വ്യഞ്ജകം_in_str(*arg, &ga, evaluate);
	എങ്കിൽ (p == ശൂന്യ)
	{
	    ret = FAIL;
	    നിർത്തുക;
	}
	*arg = p;
    }

    rettv->v_type = VAR_STRING;
    എങ്കിൽ (ret == FAIL || !evaluate || ga_append(&ga, NUL) == FAIL)
    {
	ga_clear(&ga);
	rettv->vval.v_string = ശൂന്യ;
	തിരികെ ret;
    }

    rettv->vval.v_string = ga.ga_data;
    തിരികെ OK;
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
    പൂർണ്ണസംഖ്യ		copyID)
{
    തിരികെ echo_string_core(tv, tofree, numbuf, copyID, FALSE, TRUE, FALSE);
}

/*
 * Get the value of an environment variable.
 * "arg" is pointing to the '$'.  It is advanced to after the name.
 * If the environment variable was not set, silently assume it is empty.
 * Return FAIL if the name is invalid.
 */
    പൂർണ്ണസംഖ്യ
eval_env_var(char_u **arg, typval_T *rettv, പൂർണ്ണസംഖ്യ evaluate)
{
    char_u	*string = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		len;
    പൂർണ്ണസംഖ്യ		cc;
    char_u	*name;
    പൂർണ്ണസംഖ്യ		mustfree = FALSE;

    ++*arg;
    name = *arg;
    len = get_env_len(arg);
    എങ്കിൽ (evaluate)
    {
	എങ്കിൽ (len == 0)
	    തിരികെ FAIL; // invalid empty name

	cc = name[len];
	name[len] = NUL;
	// first try vim_getenv(), fast for normal environment vars
	string = vim_getenv(name, &mustfree);
	എങ്കിൽ (string != ശൂന്യ && *string != NUL)
	{
	    എങ്കിൽ (!mustfree)
		string = vim_strsave(string);
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (mustfree)
		vim_free(string);

	    // next try expanding things like $VIM and ${HOME}
	    string = expand_env_save(name - 1);
	    എങ്കിൽ (string != ശൂന്യ && *string == '$')
		VIM_CLEAR(string);
	}
	name[len] = cc;

	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = string;
	rettv->v_lock = 0;
    }

    തിരികെ OK;
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
    പൂർണ്ണസംഖ്യ		did_emsg_before = did_emsg;

    എങ്കിൽ (argvars[0].v_type != VAR_STRING || !in_vim9script())
	lnum = (linenr_T)tv_get_സംഖ്യ_chk(&argvars[0], ശൂന്യ);
    എങ്കിൽ (lnum <= 0 && did_emsg_before == did_emsg
					    && argvars[0].v_type != VAR_സംഖ്യ)
    {
	പൂർണ്ണസംഖ്യ	fnum;
	pos_T	*fp;

	// no valid number, try using arg like line()
	fp = var2fpos(&argvars[0], TRUE, &fnum, FALSE);
	എങ്കിൽ (fp != ശൂന്യ)
	    lnum = fp->lnum;
    }
    തിരികെ lnum;
}

/*
 * Get the lnum from the first argument.
 * Also accepts "$", then "buf" is used.
 * Returns 0 on error.
 */
    linenr_T
tv_get_lnum_buf(typval_T *argvars, buf_T *buf)
{
    എങ്കിൽ (argvars[0].v_type == VAR_STRING
	    && argvars[0].vval.v_string != ശൂന്യ
	    && argvars[0].vval.v_string[0] == '$'
	    && argvars[0].vval.v_string[1] == NUL
	    && buf != ശൂന്യ)
	തിരികെ buf->b_ml.ml_വരി_എണ്ണം;
    തിരികെ (linenr_T)tv_get_സംഖ്യ_chk(&argvars[0], ശൂന്യ);
}

/*
 * Get buffer by number or pattern.
 */
    buf_T *
tv_get_buf(typval_T *tv, പൂർണ്ണസംഖ്യ curtab_only)
{
    char_u	*name = tv->vval.v_string;
    buf_T	*buf;

    എങ്കിൽ (tv->v_type == VAR_സംഖ്യ)
	തിരികെ buflist_findnr((പൂർണ്ണസംഖ്യ)tv->vval.v_സംഖ്യ);
    എങ്കിൽ (tv->v_type != VAR_STRING)
	തിരികെ ശൂന്യ;
    എങ്കിൽ (name == ശൂന്യ || *name == NUL)
	തിരികെ curbuf;
    എങ്കിൽ (name[0] == '$' && name[1] == NUL)
	തിരികെ lastbuf;

    buf = buflist_find_by_name(name, curtab_only);

    // If not found, try expanding the name, like done for bufexists().
    എങ്കിൽ (buf == ശൂന്യ)
	buf = find_buffer(tv);

    തിരികെ buf;
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
    എങ്കിൽ (buf == ശൂന്യ
	    && tv->v_type != VAR_സംഖ്യ
	    && tv->v_type != VAR_STRING)
	// issue errmsg for type error
	(ശൂന്യം)tv_get_സംഖ്യ(tv);
    തിരികെ buf;
}

/*
 * Check that repeating "slen" items "n" times, does not
 * overflow the int used for the resulting length.
 */
    പൂർണ്ണസംഖ്യ
check_repeat_എണ്ണം(varnumber_T slen, varnumber_T n)
{
    // Use a division so that the multiplication cannot overflow.
    എങ്കിൽ (n <= (varnumber_T)INT_MAX / slen)
	തിരികെ OK;

    അക്ഷരം	buf[NUMBUFLEN];

    vim_snprintf(buf, വലുപ്പം(buf), "%lld", (long long)n);
    semsg(_(e_val_too_large), buf);
    തിരികെ FAIL;
}

#endif // FEAT_EVAL
