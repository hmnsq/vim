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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಯದಿ defined(FEAT_EVAL)

/*
 * Allocate memory for a variable type-value, and make it empty (0 or NULL
 * value).
 */
    typval_T *
alloc_tv(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ ALLOC_CLEAR_ONE(typval_T);
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
    ಯದಿ (rettv != ರಿಕ್ತ)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = s;
    }
    ಅನ್ಯಥಾ
	vim_free(s);
    ಪ್ರತ್ಯಾಗಮ rettv;
}

/*
 * Free the memory for a variable type-value.
 */
    ಶೂನ್ಯ
free_tv(typval_T *varp)
{
    ಯದಿ (varp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ವಿಕಲ್ಪ (varp->v_type)
    {
	ಪಕ್ಷ VAR_FUNC:
	    func_unref(varp->vval.v_string);
	    // FALLTHROUGH
	ಪಕ್ಷ VAR_STRING:
	    vim_free(varp->vval.v_string);
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_PARTIAL:
	    partial_unref(varp->vval.v_partial);
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_BLOB:
	    blob_unref(varp->vval.v_blob);
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_LIST:
	    list_unref(varp->vval.v_list);
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_TUPLE:
	    tuple_unref(varp->vval.v_tuple);
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_DICT:
	    dict_unref(varp->vval.v_dict);
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    job_unref(varp->vval.v_job);
	    ಭಙ್ಗ;
#endif
	ಪಕ್ಷ VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    channel_unref(varp->vval.v_channel);
	    ಭಙ್ಗ;
#endif
	ಪಕ್ಷ VAR_CLASS:
	    class_unref(varp->vval.v_class);
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_OBJECT:
	    object_unref(varp->vval.v_object);
	    ಭಙ್ಗ;

	ಪಕ್ಷ VAR_TYPEALIAS:
	    typealias_unref(varp->vval.v_typealias);
	    ಭಙ್ಗ;

	ಪಕ್ಷ VAR_ಸಂಖ್ಯೆ:
	ಪಕ್ಷ VAR_FLOAT:
	ಪಕ್ಷ VAR_ANY:
	ಪಕ್ಷ VAR_UNKNOWN:
	ಪಕ್ಷ VAR_VOID:
	ಪಕ್ಷ VAR_BOOL:
	ಪಕ್ಷ VAR_SPECIAL:
	ಪಕ್ಷ VAR_INSTR:
	    ಭಙ್ಗ;
    }
    vim_free(varp);
}

/*
 * Free the memory for a variable value and set the value to NULL or 0.
 */
    ಶೂನ್ಯ
clear_tv(typval_T *varp)
{
    ಯದಿ (varp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ವಿಕಲ್ಪ (varp->v_type)
    {
	ಪಕ್ಷ VAR_FUNC:
	    func_unref(varp->vval.v_string);
	    // FALLTHROUGH
	ಪಕ್ಷ VAR_STRING:
	    VIM_CLEAR(varp->vval.v_string);
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_PARTIAL:
	    partial_unref(varp->vval.v_partial);
	    varp->vval.v_partial = ರಿಕ್ತ;
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_BLOB:
	    blob_unref(varp->vval.v_blob);
	    varp->vval.v_blob = ರಿಕ್ತ;
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_LIST:
	    list_unref(varp->vval.v_list);
	    varp->vval.v_list = ರಿಕ್ತ;
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_TUPLE:
	    tuple_unref(varp->vval.v_tuple);
	    varp->vval.v_tuple = ರಿಕ್ತ;
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_DICT:
	    dict_unref(varp->vval.v_dict);
	    varp->vval.v_dict = ರಿಕ್ತ;
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_ಸಂಖ್ಯೆ:
	ಪಕ್ಷ VAR_BOOL:
	ಪಕ್ಷ VAR_SPECIAL:
	    varp->vval.v_ಸಂಖ್ಯೆ = 0;
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_FLOAT:
	    varp->vval.v_float = 0.0;
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    job_unref(varp->vval.v_job);
	    varp->vval.v_job = ರಿಕ್ತ;
#endif
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    channel_unref(varp->vval.v_channel);
	    varp->vval.v_channel = ರಿಕ್ತ;
#endif
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_INSTR:
	    VIM_CLEAR(varp->vval.v_instr);
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_CLASS:
	    class_unref(varp->vval.v_class);
	    varp->vval.v_class = ರಿಕ್ತ;
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_OBJECT:
	    object_unref(varp->vval.v_object);
	    varp->vval.v_object = ರಿಕ್ತ;
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_TYPEALIAS:
	    typealias_unref(varp->vval.v_typealias);
	    varp->vval.v_typealias = ರಿಕ್ತ;
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_UNKNOWN:
	ಪಕ್ಷ VAR_ANY:
	ಪಕ್ಷ VAR_VOID:
	    ಭಙ್ಗ;
    }
    varp->v_lock = 0;
}

/*
 * Set the value of a variable to NULL without freeing items.
 */
    ಶೂನ್ಯ
init_tv(typval_T *varp)
{
    ಯದಿ (varp != ರಿಕ್ತ)
	CLEAR_POINTER(varp);
}

    ಸ್ಥಿರ varnumber_T
tv_get_bool_or_ಸಂಖ್ಯೆ_chk(
	typval_T    *varp,
	ಪೂರ್ಣಾಙ್ಕ	    *denote,
	ಪೂರ್ಣಾಙ್ಕ	    want_bool,
	ಪೂರ್ಣಾಙ್ಕ	    vim9_string_error) // in Vim9 using a string is an error
{
    varnumber_T	n = 0L;

    ವಿಕಲ್ಪ (varp->v_type)
    {
	ಪಕ್ಷ VAR_ಸಂಖ್ಯೆ:
	    ಯದಿ (in_vim9script() && want_bool && varp->vval.v_ಸಂಖ್ಯೆ != 0
						   && varp->vval.v_ಸಂಖ್ಯೆ != 1)
	    {
		semsg(_(e_using_ಸಂಖ್ಯೆ_as_bool_nr), varp->vval.v_ಸಂಖ್ಯೆ);
		ಭಙ್ಗ;
	    }
	    ಪ್ರತ್ಯಾಗಮ varp->vval.v_ಸಂಖ್ಯೆ;
	ಪಕ್ಷ VAR_FLOAT:
	    emsg(_(e_using_float_as_ಸಂಖ್ಯೆ));
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_FUNC:
	ಪಕ್ಷ VAR_PARTIAL:
	    emsg(_(e_using_funcref_as_ಸಂಖ್ಯೆ));
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_STRING:
	    ಯದಿ (vim9_string_error && in_vim9script())
	    {
		emsg_using_string_as(varp, !want_bool);
		ಭಙ್ಗ;
	    }
	    ಯದಿ (varp->vval.v_string != ರಿಕ್ತ)
		vim_str2nr(varp->vval.v_string, ರಿಕ್ತ, ರಿಕ್ತ,
					 STR2NR_ALL, &n, ರಿಕ್ತ, 0, FALSE, ರಿಕ್ತ);
	    ಪ್ರತ್ಯಾಗಮ n;
	ಪಕ್ಷ VAR_LIST:
	    emsg(_(e_using_list_as_ಸಂಖ್ಯೆ));
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_TUPLE:
	    emsg(_(e_using_tuple_as_ಸಂಖ್ಯೆ));
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_DICT:
	    emsg(_(e_using_dictionary_as_ಸಂಖ್ಯೆ));
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_BOOL:
	ಪಕ್ಷ VAR_SPECIAL:
	    ಯದಿ (!want_bool && in_vim9script())
	    {
		ಯದಿ (varp->v_type == VAR_BOOL)
		    emsg(_(e_using_bool_as_ಸಂಖ್ಯೆ));
		ಅನ್ಯಥಾ
		    emsg(_(e_using_special_as_ಸಂಖ್ಯೆ));
		ಭಙ್ಗ;
	    }
	    ಪ್ರತ್ಯಾಗಮ varp->vval.v_ಸಂಖ್ಯೆ == VVAL_TRUE ? 1 : 0;
	ಪಕ್ಷ VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    emsg(_(e_using_job_as_ಸಂಖ್ಯೆ));
	    ಭಙ್ಗ;
#endif
	ಪಕ್ಷ VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    emsg(_(e_using_channel_as_ಸಂಖ್ಯೆ));
	    ಭಙ್ಗ;
#endif
	ಪಕ್ಷ VAR_BLOB:
	    emsg(_(e_using_blob_as_ಸಂಖ್ಯೆ));
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_CLASS:
	ಪಕ್ಷ VAR_TYPEALIAS:
	    check_typval_is_ಮೌಲ್ಯ(varp);
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_OBJECT:
	    {
		ಯದಿ (varp->vval.v_object == ರಿಕ್ತ)
		    emsg(_(e_using_object_as_string));
		ಅನ್ಯಥಾ
		{
		    class_T *cl = varp->vval.v_object->obj_class;
		    ಯದಿ (cl != ರಿಕ್ತ && IS_ENUM(cl))
			semsg(_(e_using_enum_str_as_ಸಂಖ್ಯೆ), cl->class_name.string);
		    ಅನ್ಯಥಾ
			emsg(_(e_using_object_as_ಸಂಖ್ಯೆ));
		}
	    }
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_VOID:
	    emsg(_(e_cannot_use_void_ಮೌಲ್ಯ));
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_UNKNOWN:
	ಪಕ್ಷ VAR_ANY:
	ಪಕ್ಷ VAR_INSTR:
	    internal_error_no_abort("tv_get_number(UNKNOWN)");
	    ಭಙ್ಗ;
    }
    ಯದಿ (denote == ರಿಕ್ತ)		// useful for values that must be unsigned
	n = -1;
    ಅನ್ಯಥಾ
	*denote = TRUE;
    ಪ್ರತ್ಯಾಗಮ n;
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
tv_get_ಸಂಖ್ಯೆ(typval_T *varp)
{
    ಪೂರ್ಣಾಙ್ಕ		error = FALSE;

    ಪ್ರತ್ಯಾಗಮ tv_get_ಸಂಖ್ಯೆ_chk(varp, &error);	// return 0L on error
}

/*
 * Like tv_get_number() but in Vim9 script do convert a number in a string to a
 * number without giving an error.
 */
    varnumber_T
tv_to_ಸಂಖ್ಯೆ(typval_T *varp)
{
    ಪೂರ್ಣಾಙ್ಕ		error = FALSE;

    ಪ್ರತ್ಯಾಗಮ tv_get_bool_or_ಸಂಖ್ಯೆ_chk(varp, &error, FALSE, FALSE);
}

    varnumber_T
tv_get_ಸಂಖ್ಯೆ_chk(typval_T *varp, ಪೂರ್ಣಾಙ್ಕ *denote)
{
    ಪ್ರತ್ಯಾಗಮ tv_get_bool_or_ಸಂಖ್ಯೆ_chk(varp, denote, FALSE, TRUE);
}

/*
 * Get the boolean value of "varp".  This is like tv_get_number_chk(),
 * but in Vim9 script accepts Number (0 and 1) and Bool/Special.
 */
    varnumber_T
tv_get_bool(typval_T *varp)
{
    ಪ್ರತ್ಯಾಗಮ tv_get_bool_or_ಸಂಖ್ಯೆ_chk(varp, ರಿಕ್ತ, TRUE, TRUE);
}

/*
 * Get the boolean value of "varp".  This is like tv_get_number_chk(),
 * but in Vim9 script accepts Number and Bool.
 */
    varnumber_T
tv_get_bool_chk(typval_T *varp, ಪೂರ್ಣಾಙ್ಕ *denote)
{
    ಪ್ರತ್ಯಾಗಮ tv_get_bool_or_ಸಂಖ್ಯೆ_chk(varp, denote, TRUE, TRUE);
}

    ಸ್ಥಿರ float_T
tv_get_float_chk(typval_T *varp, ಪೂರ್ಣಾಙ್ಕ *error)
{
    ವಿಕಲ್ಪ (varp->v_type)
    {
	ಪಕ್ಷ VAR_ಸಂಖ್ಯೆ:
	    ಪ್ರತ್ಯಾಗಮ (float_T)(varp->vval.v_ಸಂಖ್ಯೆ);
	ಪಕ್ಷ VAR_FLOAT:
	    ಪ್ರತ್ಯಾಗಮ varp->vval.v_float;
	ಪಕ್ಷ VAR_FUNC:
	ಪಕ್ಷ VAR_PARTIAL:
	    emsg(_(e_using_funcref_as_float));
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_STRING:
	    emsg(_(e_using_string_as_float));
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_LIST:
	    emsg(_(e_using_list_as_float));
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_TUPLE:
	    emsg(_(e_using_tuple_as_float));
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_DICT:
	    emsg(_(e_using_dictionary_as_float));
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_BOOL:
	    emsg(_(e_using_boolean_ಮೌಲ್ಯ_as_float));
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_SPECIAL:
	    emsg(_(e_using_special_ಮೌಲ್ಯ_as_float));
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    emsg(_(e_using_job_as_float));
	    ಭಙ್ಗ;
#endif
	ಪಕ್ಷ VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    emsg(_(e_using_channel_as_float));
	    ಭಙ್ಗ;
#endif
	ಪಕ್ಷ VAR_BLOB:
	    emsg(_(e_using_blob_as_float));
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_CLASS:
	ಪಕ್ಷ VAR_TYPEALIAS:
	    check_typval_is_ಮೌಲ್ಯ(varp);
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_OBJECT:
	    emsg(_(e_using_object_as_float));
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_VOID:
	    emsg(_(e_cannot_use_void_ಮೌಲ್ಯ));
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_UNKNOWN:
	ಪಕ್ಷ VAR_ANY:
	ಪಕ್ಷ VAR_INSTR:
	    internal_error_no_abort("tv_get_float(UNKNOWN)");
	    ಭಙ್ಗ;
    }
    ಯದಿ (error != ರಿಕ್ತ)
	*error = TRUE;
    ಪ್ರತ್ಯಾಗಮ 0;
}

    float_T
tv_get_float(typval_T *varp)
{
    ಪ್ರತ್ಯಾಗಮ tv_get_float_chk(varp, ರಿಕ್ತ);
}

/*
 * Give an error and return FAIL unless "args[idx]" is unknown
 */
    ಪೂರ್ಣಾಙ್ಕ
check_for_unknown_arg(typval_T *args, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (args[idx].v_type != VAR_UNKNOWN)
    {
	semsg(_(e_too_many_arguments), idx + 1);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_for_string_arg(typval_T *args, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (args[idx].v_type != VAR_STRING)
    {
	semsg(_(e_string_required_for_argument_nr), idx + 1);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a non-empty string.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_for_nonempty_string_arg(typval_T *args, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (check_for_string_arg(args, idx) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಯದಿ (args[idx].vval.v_string == ರಿಕ್ತ || *args[idx].vval.v_string == NUL)
    {
	semsg(_(e_non_empty_string_required_for_argument_nr), idx + 1);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Check for an optional string argument at 'idx'
 */
    ಪೂರ್ಣಾಙ್ಕ
check_for_opt_string_arg(typval_T *args, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಪ್ರತ್ಯಾಗಮ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_string_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a number.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_for_ಸಂಖ್ಯೆ_arg(typval_T *args, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (args[idx].v_type != VAR_ಸಂಖ್ಯೆ)
    {
	semsg(_(e_ಸಂಖ್ಯೆ_required_for_argument_nr), idx + 1);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Check for an optional number argument at 'idx'
 */
    ಪೂರ್ಣಾಙ್ಕ
check_for_opt_ಸಂಖ್ಯೆ_arg(typval_T *args, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಪ್ರತ್ಯಾಗಮ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_ಸಂಖ್ಯೆ_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a float or a number.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_for_float_or_nr_arg(typval_T *args, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (args[idx].v_type != VAR_FLOAT && args[idx].v_type != VAR_ಸಂಖ್ಯೆ)
    {
	semsg(_(e_float_or_ಸಂಖ್ಯೆ_required_for_argument_nr), idx + 1);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a bool.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_for_bool_arg(typval_T *args, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (args[idx].v_type != VAR_BOOL
	    && !(args[idx].v_type == VAR_ಸಂಖ್ಯೆ
		&& (args[idx].vval.v_ಸಂಖ್ಯೆ == 0
		    || args[idx].vval.v_ಸಂಖ್ಯೆ == 1)))
    {
	semsg(_(e_bool_required_for_argument_nr), idx + 1);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a bool or a number.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
check_for_bool_or_ಸಂಖ್ಯೆ_arg(typval_T *args, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (args[idx].v_type != VAR_BOOL && args[idx].v_type != VAR_ಸಂಖ್ಯೆ)
    {
	semsg(_(e_bool_or_ಸಂಖ್ಯೆ_required_for_argument_nr), idx + 1);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Check for an optional bool argument at 'idx'.
 * Return FAIL if the type is wrong.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_for_opt_bool_arg(typval_T *args, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (args[idx].v_type == VAR_UNKNOWN)
	ಪ್ರತ್ಯಾಗಮ OK;
    ಪ್ರತ್ಯಾಗಮ check_for_bool_arg(args, idx);
}

/*
 * Check for an optional bool or number argument at 'idx'.
 * Return FAIL if the type is wrong.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_for_opt_bool_or_ಸಂಖ್ಯೆ_arg(typval_T *args, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (args[idx].v_type == VAR_UNKNOWN)
	ಪ್ರತ್ಯಾಗಮ OK;
    ಪ್ರತ್ಯಾಗಮ check_for_bool_or_ಸಂಖ್ಯೆ_arg(args, idx);
}

/*
 * Check for an optional bool or dict argument at 'idx'.
 * Return FAIL if the type is wrong.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_for_opt_bool_or_dict_arg(typval_T *args, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (args[idx].v_type == VAR_UNKNOWN || args[idx].v_type == VAR_DICT)
	ಪ್ರತ್ಯಾಗಮ OK;
    ಪ್ರತ್ಯಾಗಮ check_for_bool_arg(args, idx);
}

/*
 * Give an error and return FAIL unless "args[idx]" is a blob.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_for_blob_arg(typval_T *args, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_blob_required_for_argument_nr), idx + 1);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_for_list_arg(typval_T *args, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (args[idx].v_type != VAR_LIST)
    {
	semsg(_(e_list_required_for_argument_nr), idx + 1);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a non-NULL list.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_for_nonnull_list_arg(typval_T *args, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (check_for_list_arg(args, idx) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (args[idx].vval.v_list == ರಿಕ್ತ)
    {
	semsg(_(e_non_null_list_required_for_argument_nr), idx + 1);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Check for an optional list argument at 'idx'
 */
    ಪೂರ್ಣಾಙ್ಕ
check_for_opt_list_arg(typval_T *args, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಪ್ರತ್ಯಾಗಮ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_list_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a tuple.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_for_tuple_arg(typval_T *args, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (args[idx].v_type != VAR_TUPLE)
    {
	semsg(_(e_tuple_required_for_argument_nr), idx + 1);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a dict.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_for_dict_arg(typval_T *args, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_dict_required_for_argument_nr), idx + 1);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a non-NULL dict.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_for_nonnull_dict_arg(typval_T *args, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (check_for_dict_arg(args, idx) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (args[idx].vval.v_dict == ರಿಕ್ತ)
    {
	semsg(_(e_non_null_dict_required_for_argument_nr), idx + 1);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Check for an optional dict argument at 'idx'
 */
    ಪೂರ್ಣಾಙ್ಕ
check_for_opt_dict_arg(typval_T *args, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಪ್ರತ್ಯಾಗಮ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_dict_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Check for an optional non-NULL dict argument at 'idx'
 */
    ಪೂರ್ಣಾಙ್ಕ
check_for_opt_nonnull_dict_arg(typval_T *args, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಪ್ರತ್ಯಾಗಮ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_nonnull_dict_arg(args, idx) != FAIL) ? OK : FAIL;
}

#ಯದಿ defined(FEAT_JOB_CHANNEL)
/*
 * Give an error and return FAIL unless "args[idx]" is a channel or a job.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_for_chan_or_job_arg(typval_T *args, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (args[idx].v_type != VAR_CHANNEL && args[idx].v_type != VAR_JOB)
    {
	semsg(_(e_chan_or_job_required_for_argument_nr), idx + 1);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an optional channel or a
 * job.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_for_opt_chan_or_job_arg(typval_T *args, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಪ್ರತ್ಯಾಗಮ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_chan_or_job_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a job.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_for_job_arg(typval_T *args, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (args[idx].v_type != VAR_JOB)
    {
	semsg(_(e_job_required_for_argument_nr), idx + 1);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Check for an optional job argument at 'idx'.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_for_opt_job_arg(typval_T *args, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಪ್ರತ್ಯಾಗಮ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_job_arg(args, idx) != FAIL) ? OK : FAIL;
}
#ಅನ್ಯಥಾ
/*
 * Give an error and return FAIL unless "args[idx]" is an optional channel or a
 * job.  Used without the +channel feature, thus only VAR_UNKNOWN is accepted.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_for_opt_chan_or_job_arg(typval_T *args, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಪ್ರತ್ಯಾಗಮ args[idx].v_type == VAR_UNKNOWN ? OK : FAIL;
}
#endif

/*
 * Give an error and return FAIL unless "args[idx]" is a string or
 * a number.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_for_string_or_ಸಂಖ್ಯೆ_arg(typval_T *args, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (args[idx].v_type != VAR_STRING && args[idx].v_type != VAR_ಸಂಖ್ಯೆ)
    {
	semsg(_(e_string_or_ಸಂಖ್ಯೆ_required_for_argument_nr), idx + 1);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Check for an optional string or number argument at 'idx'.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_for_opt_string_or_ಸಂಖ್ಯೆ_arg(typval_T *args, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಪ್ರತ್ಯಾಗಮ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_string_or_ಸಂಖ್ಯೆ_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a buffer number.
 * Buffer number can be a number or a string.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_for_buffer_arg(typval_T *args, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಪ್ರತ್ಯಾಗಮ check_for_string_or_ಸಂಖ್ಯೆ_arg(args, idx);
}

/*
 * Check for an optional buffer argument at 'idx'
 */
    ಪೂರ್ಣಾಙ್ಕ
check_for_opt_buffer_arg(typval_T *args, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಪ್ರತ್ಯಾಗಮ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_buffer_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a line number.
 * Line number can be a number or a string.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_for_lnum_arg(typval_T *args, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಪ್ರತ್ಯಾಗಮ check_for_string_or_ಸಂಖ್ಯೆ_arg(args, idx);
}

/*
 * Check for an optional line number argument at 'idx'
 */
    ಪೂರ್ಣಾಙ್ಕ
check_for_opt_lnum_arg(typval_T *args, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಪ್ರತ್ಯಾಗಮ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_lnum_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a blob.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_for_string_or_blob_arg(typval_T *args, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (args[idx].v_type != VAR_STRING && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_string_or_blob_required_for_argument_nr), idx + 1);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a list.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_for_string_or_list_arg(typval_T *args, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (args[idx].v_type != VAR_STRING && args[idx].v_type != VAR_LIST)
    {
	semsg(_(e_string_or_list_required_for_argument_nr), idx + 1);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string, a list, a
 * tuple or a blob.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_for_string_or_list_or_tuple_or_blob_arg(typval_T *args, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_string_list_tuple_or_blob_required_for_argument_nr), idx + 1);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Check for an optional string or list argument at 'idx'
 */
    ಪೂರ್ಣಾಙ್ಕ
check_for_opt_string_or_list_arg(typval_T *args, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಪ್ರತ್ಯಾಗಮ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_string_or_list_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a dict.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_for_string_or_dict_arg(typval_T *args, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (args[idx].v_type != VAR_STRING && args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_string_or_dict_required_for_argument_nr), idx + 1);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a number
 * or a list.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_for_string_or_ಸಂಖ್ಯೆ_or_list_arg(typval_T *args, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_ಸಂಖ್ಯೆ
	    && args[idx].v_type != VAR_LIST)
    {
	semsg(_(e_string_ಸಂಖ್ಯೆ_or_list_required_for_argument_nr), idx + 1);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an optional string
 * or number or a list
 */
    ಪೂರ್ಣಾಙ್ಕ
check_for_opt_string_or_ಸಂಖ್ಯೆ_or_list_arg(typval_T *args, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಪ್ರತ್ಯಾಗಮ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_string_or_ಸಂಖ್ಯೆ_or_list_arg(args, idx)
							!= FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string, a number, a
 * list, a tuple or a blob.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_for_repeat_func_arg(typval_T *args, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_ಸಂಖ್ಯೆ
	    && args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_repeatable_type_required_for_argument_nr), idx + 1);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string, a list, a
 * tuple or a dict.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_for_string_list_tuple_or_dict_arg(typval_T *args, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_string_list_tuple_or_dict_required_for_argument_nr), idx + 1);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string
 * or a function reference.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_for_string_or_func_arg(typval_T *args, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (args[idx].v_type != VAR_PARTIAL
	    && args[idx].v_type != VAR_FUNC
	    && args[idx].v_type != VAR_STRING)
    {
	semsg(_(e_string_or_function_required_for_argument_nr), idx + 1);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list or a blob.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_for_list_or_blob_arg(typval_T *args, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (args[idx].v_type != VAR_LIST && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_list_or_blob_required_for_argument_nr), idx + 1);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list or a tuple.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_for_list_or_tuple_arg(typval_T *args, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (args[idx].v_type != VAR_LIST && args[idx].v_type != VAR_TUPLE)
    {
	semsg(_(e_list_or_tuple_required_for_argument_nr), idx + 1);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list, a tuple or a
 * blob.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_for_list_or_tuple_or_blob_arg(typval_T *args, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_list_or_tuple_or_blob_required_for_argument_nr), idx + 1);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list, a tuple or a
 * dict
 */
    ಪೂರ್ಣಾಙ್ಕ
check_for_list_or_tuple_or_dict_arg(typval_T *args, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_list_or_tuple_or_dict_required_for_argument_nr), idx + 1);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list or dict or a
 * blob.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_for_list_or_dict_or_blob_arg(typval_T *args, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_DICT
	    && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_list_dict_or_blob_required_for_argument_nr), idx + 1);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list, a tuple, a dict,
 * a blob or a string.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_for_list_tuple_dict_blob_or_string_arg(typval_T *args, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_DICT
	    && args[idx].v_type != VAR_BLOB
	    && args[idx].v_type != VAR_STRING)
    {
	semsg(_(e_list_tuple_dict_blob_or_string_required_for_argument_nr), idx + 1);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an optional buffer
 * number or a dict.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_for_opt_buffer_or_dict_arg(typval_T *args, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (args[idx].v_type != VAR_UNKNOWN
	    && args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_ಸಂಖ್ಯೆ
	    && args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_string_required_for_argument_nr), idx + 1);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an object.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_for_object_arg(typval_T *args, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (args[idx].v_type != VAR_OBJECT)
    {
	semsg(_(e_object_required_for_argument_nr), idx + 1);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Returns TRUE if "tv" is a type alias for a class
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
tv_class_alias(typval_T *tv)
{
    ಪ್ರತ್ಯಾಗಮ tv->v_type == VAR_TYPEALIAS &&
			tv->vval.v_typealias->ta_type->tt_type == VAR_OBJECT;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a class
 * or class typealias.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_for_class_or_typealias_args(typval_T *args, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = idx; args[i].v_type != VAR_UNKNOWN; ++i)
    {
	ಯದಿ (args[i].v_type != VAR_CLASS && !tv_class_alias(&args[idx]))
	{
	    semsg(_(e_class_or_typealias_required_for_argument_nr), i + 1);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
    }
    ಪ್ರತ್ಯಾಗಮ OK;
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
    ಸ್ಥಿರ char_u   mybuf[NUMBUFLEN];

    ಪ್ರತ್ಯಾಗಮ tv_get_string_buf(varp, mybuf);
}

/*
 * Like tv_get_string() but don't allow number to string conversion for Vim9.
 */
    char_u *
tv_get_string_strict(typval_T *varp)
{
    ಸ್ಥಿರ char_u   mybuf[NUMBUFLEN];
    char_u	    *res =  tv_get_string_buf_chk_strict(
						 varp, mybuf, in_vim9script());

    ಪ್ರತ್ಯಾಗಮ res != ರಿಕ್ತ ? res : (char_u *)"";
}

    char_u *
tv_get_string_buf(typval_T *varp, char_u *buf)
{
    char_u	*res = tv_get_string_buf_chk(varp, buf);

    ಪ್ರತ್ಯಾಗಮ res != ರಿಕ್ತ ? res : (char_u *)"";
}

/*
 * Careful: This uses a single, static buffer.  YOU CAN ONLY USE IT ONCE!
 */
    char_u *
tv_get_string_chk(typval_T *varp)
{
    ಸ್ಥಿರ char_u   mybuf[NUMBUFLEN];

    ಪ್ರತ್ಯಾಗಮ tv_get_string_buf_chk(varp, mybuf);
}

    char_u *
tv_get_string_buf_chk(typval_T *varp, char_u *buf)
{
    ಪ್ರತ್ಯಾಗಮ tv_get_string_buf_chk_strict(varp, buf, FALSE);
}

    char_u *
tv_get_string_buf_chk_strict(typval_T *varp, char_u *buf, ಪೂರ್ಣಾಙ್ಕ strict)
{
    ವಿಕಲ್ಪ (varp->v_type)
    {
	ಪಕ್ಷ VAR_ಸಂಖ್ಯೆ:
	    ಯದಿ (strict)
	    {
		emsg(_(e_using_ಸಂಖ್ಯೆ_as_string));
		ಭಙ್ಗ;
	    }
	    vim_snprintf((ವರ್ಣ *)buf, NUMBUFLEN, "%lld",
					    (varnumber_T)varp->vval.v_ಸಂಖ್ಯೆ);
	    ಪ್ರತ್ಯಾಗಮ buf;
	ಪಕ್ಷ VAR_FUNC:
	ಪಕ್ಷ VAR_PARTIAL:
	    emsg(_(e_using_funcref_as_string));
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_LIST:
	    emsg(_(e_using_list_as_string));
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_TUPLE:
	    emsg(_(e_using_tuple_as_string));
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_DICT:
	    emsg(_(e_using_dictionary_as_string));
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_FLOAT:
	    ಯದಿ (strict)
	    {
		emsg(_(e_using_float_as_string));
		ಭಙ್ಗ;
	    }
	    vim_snprintf((ವರ್ಣ *)buf, NUMBUFLEN, "%g", varp->vval.v_float);
	    ಪ್ರತ್ಯಾಗಮ buf;
	ಪಕ್ಷ VAR_STRING:
	    ಯದಿ (varp->vval.v_string != ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ varp->vval.v_string;
	    ಪ್ರತ್ಯಾಗಮ (char_u *)"";
	ಪಕ್ಷ VAR_BOOL:
	ಪಕ್ಷ VAR_SPECIAL:
	    STRCPY(buf, get_var_special_name(varp->vval.v_ಸಂಖ್ಯೆ));
	    ಪ್ರತ್ಯಾಗಮ buf;
	ಪಕ್ಷ VAR_BLOB:
	    emsg(_(e_using_blob_as_string));
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_CLASS:
	ಪಕ್ಷ VAR_TYPEALIAS:
	    check_typval_is_ಮೌಲ್ಯ(varp);
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_OBJECT:
	    {
		ಯದಿ (varp->vval.v_object == ರಿಕ್ತ)
		    emsg(_(e_using_object_as_string));
		ಅನ್ಯಥಾ
		{
		    class_T *cl = varp->vval.v_object->obj_class;
		    ಯದಿ (cl != ರಿಕ್ತ && IS_ENUM(cl))
			semsg(_(e_using_enum_str_as_string), cl->class_name.string);
		    ಅನ್ಯಥಾ
			emsg(_(e_using_object_as_string));
		}
	    }
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    ಯದಿ (in_vim9script())
	    {
		semsg(_(e_using_invalid_ಮೌಲ್ಯ_as_string_str), "job");
		ಭಙ್ಗ;
	    }
	    ಪ್ರತ್ಯಾಗಮ job_to_string_buf(varp, buf);
#endif
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    ಯದಿ (in_vim9script())
	    {
		semsg(_(e_using_invalid_ಮೌಲ್ಯ_as_string_str), "channel");
		ಭಙ್ಗ;
	    }
	    ಪ್ರತ್ಯಾಗಮ channel_to_string_buf(varp, buf);
#endif
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_VOID:
	    emsg(_(e_cannot_use_void_ಮೌಲ್ಯ));
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_UNKNOWN:
	ಪಕ್ಷ VAR_ANY:
	ಪಕ್ಷ VAR_INSTR:
	    semsg(_(e_using_invalid_ಮೌಲ್ಯ_as_string_str),
						  vartype_name(varp->v_type));
	    ಭಙ್ಗ;
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Turn a typeval into a string.  Similar to tv_get_string_buf() but uses
 * string() on Dict, List, etc.
 */
    char_u *
tv_stringify(typval_T *varp, char_u *buf)
{
    ಯದಿ (varp->v_type == VAR_LIST
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
	ಪ್ರತ್ಯಾಗಮ tmp.vval.v_string;
    }
    ಪ್ರತ್ಯಾಗಮ tv_get_string_buf(varp, buf);
}

/*
 * Return TRUE if typeval "tv" and its value are set to be locked (immutable).
 * Also give an error message, using "name" or _("name") when use_gettext is
 * TRUE.
 */
    ಪೂರ್ಣಾಙ್ಕ
tv_check_lock(typval_T *tv, char_u *name, ಪೂರ್ಣಾಙ್ಕ use_gettext)
{
    ಪೂರ್ಣಾಙ್ಕ	lock = 0;

    ವಿಕಲ್ಪ (tv->v_type)
    {
	ಪಕ್ಷ VAR_BLOB:
	    ಯದಿ (tv->vval.v_blob != ರಿಕ್ತ)
		lock = tv->vval.v_blob->bv_lock;
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_LIST:
	    ಯದಿ (tv->vval.v_list != ರಿಕ್ತ)
		lock = tv->vval.v_list->lv_lock;
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_TUPLE:
	    ಯದಿ (tv->vval.v_tuple != ರಿಕ್ತ)
		lock = tv->vval.v_tuple->tv_lock;
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_DICT:
	    ಯದಿ (tv->vval.v_dict != ರಿಕ್ತ)
		lock = tv->vval.v_dict->dv_lock;
	    ಭಙ್ಗ;
	ಸ್ವಭಾವ:
	    ಭಙ್ಗ;
    }
    ಪ್ರತ್ಯಾಗಮ ಮೌಲ್ಯ_check_lock(tv->v_lock, name, use_gettext)
		   || (lock != 0 && ಮೌಲ್ಯ_check_lock(lock, name, use_gettext));
}

/*
 * Copy the values from typval_T "from" to typval_T "to".
 * When needed allocates string or increases reference count.
 * Does not make a copy of a list, blob or dict but copies the reference!
 * It is OK for "from" and "to" to point to the same item.  This is used to
 * make a copy later.
 */
    ಪೂರ್ಣಾಙ್ಕ
copy_tv(typval_T *from, typval_T *to)
{
    ಪೂರ್ಣಾಙ್ಕ		ret = OK;

    to->v_type = from->v_type;
    to->v_lock = 0;
    ವಿಕಲ್ಪ (from->v_type)
    {
	ಪಕ್ಷ VAR_ಸಂಖ್ಯೆ:
	ಪಕ್ಷ VAR_BOOL:
	ಪಕ್ಷ VAR_SPECIAL:
	    to->vval.v_ಸಂಖ್ಯೆ = from->vval.v_ಸಂಖ್ಯೆ;
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_FLOAT:
	    to->vval.v_float = from->vval.v_float;
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    to->vval.v_job = from->vval.v_job;
	    ಯದಿ (to->vval.v_job != ರಿಕ್ತ)
		++to->vval.v_job->jv_refcount;
	    ಭಙ್ಗ;
#endif
	ಪಕ್ಷ VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    to->vval.v_channel = from->vval.v_channel;
	    ಯದಿ (to->vval.v_channel != ರಿಕ್ತ)
		++to->vval.v_channel->ch_refcount;
	    ಭಙ್ಗ;
#endif
	ಪಕ್ಷ VAR_INSTR:
	    to->vval.v_instr = from->vval.v_instr;
	    ಭಙ್ಗ;

	ಪಕ್ಷ VAR_CLASS:
	    copy_class(from, to);
	    ಭಙ್ಗ;

	ಪಕ್ಷ VAR_OBJECT:
	    copy_object(from, to);
	    ಭಙ್ಗ;

	ಪಕ್ಷ VAR_STRING:
	ಪಕ್ಷ VAR_FUNC:
	    ಯದಿ (from->vval.v_string == ರಿಕ್ತ)
		to->vval.v_string = ರಿಕ್ತ;
	    ಅನ್ಯಥಾ
	    {
		to->vval.v_string = vim_strsave(from->vval.v_string);
		ಯದಿ (from->v_type == VAR_FUNC)
		    func_ref(to->vval.v_string);
	    }
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_PARTIAL:
	    ಯದಿ (from->vval.v_partial == ರಿಕ್ತ)
		to->vval.v_partial = ರಿಕ್ತ;
	    ಅನ್ಯಥಾ
	    {
		to->vval.v_partial = from->vval.v_partial;
		++to->vval.v_partial->pt_refcount;
	    }
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_BLOB:
	    ಯದಿ (from->vval.v_blob == ರಿಕ್ತ)
		to->vval.v_blob = ರಿಕ್ತ;
	    ಅನ್ಯಥಾ
	    {
		to->vval.v_blob = from->vval.v_blob;
		++to->vval.v_blob->bv_refcount;
	    }
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_LIST:
	    ಯದಿ (from->vval.v_list == ರಿಕ್ತ)
		to->vval.v_list = ರಿಕ್ತ;
	    ಅನ್ಯಥಾ
	    {
		to->vval.v_list = from->vval.v_list;
		++to->vval.v_list->lv_refcount;
	    }
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_TUPLE:
	    ಯದಿ (from->vval.v_tuple == ರಿಕ್ತ)
		to->vval.v_tuple = ರಿಕ್ತ;
	    ಅನ್ಯಥಾ
	    {
		to->vval.v_tuple = from->vval.v_tuple;
		++to->vval.v_tuple->tv_refcount;
	    }
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_DICT:
	    ಯದಿ (from->vval.v_dict == ರಿಕ್ತ)
		to->vval.v_dict = ರಿಕ್ತ;
	    ಅನ್ಯಥಾ
	    {
		to->vval.v_dict = from->vval.v_dict;
		++to->vval.v_dict->dv_refcount;
	    }
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_TYPEALIAS:
	    ಯದಿ (from->vval.v_typealias == ರಿಕ್ತ)
		to->vval.v_typealias = ರಿಕ್ತ;
	    ಅನ್ಯಥಾ
	    {
		to->vval.v_typealias = from->vval.v_typealias;
		++to->vval.v_typealias->ta_refcount;
	    }
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_VOID:
	    emsg(_(e_cannot_use_void_ಮೌಲ್ಯ));
	    ret = FAIL;
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_UNKNOWN:
	ಪಕ್ಷ VAR_ANY:
	    internal_error_no_abort("copy_tv(UNKNOWN)");
	    ret = FAIL;
	    ಭಙ್ಗ;
    }

    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Compare "tv1" and "tv2".
 */
    ಪೂರ್ಣಾಙ್ಕ
typval_compare2(
    typval_T	*tv1,	// first operand
    typval_T	*tv2,	// second operand
    exprtype_T	type,	// operator
    ಪೂರ್ಣಾಙ್ಕ		ic,	// ignore case
    ಪೂರ್ಣಾಙ್ಕ		*res)	// comparison result
{
    varnumber_T	n1, n2;
    ಪೂರ್ಣಾಙ್ಕ		type_is = type == ಅಭಿವ್ಯಕ್ತಿ_IS || type == ಅಭಿವ್ಯಕ್ತಿ_ISNOT;

    ಯದಿ (check_typval_is_ಮೌಲ್ಯ(tv1) == FAIL
	|| check_typval_is_ಮೌಲ್ಯ(tv2) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಅನ್ಯಥಾ ಯದಿ (type_is && tv1->v_type != tv2->v_type)
    {
	// For "is" a different type always means FALSE, for "isnot"
	// it means TRUE.
	*res = (type == ಅಭಿವ್ಯಕ್ತಿ_ISNOT);
    }
    ಅನ್ಯಥಾ ಯದಿ (((tv1->v_type == VAR_SPECIAL && tv1->vval.v_ಸಂಖ್ಯೆ == VVAL_NULL)
		|| (tv2->v_type == VAR_SPECIAL
					   && tv2->vval.v_ಸಂಖ್ಯೆ == VVAL_NULL))
	    && tv1->v_type != tv2->v_type
	    && (type == ಅಭಿವ್ಯಕ್ತಿ_EQUAL || type == ಅಭಿವ್ಯಕ್ತಿ_NEQUAL))
    {
	n1 = typval_compare_null(tv1, tv2);
	ಯದಿ (n1 == MAYBE)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	ಯದಿ (type == ಅಭಿವ್ಯಕ್ತಿ_NEQUAL)
	    n1 = !n1;
	*res = n1;
    }
    ಅನ್ಯಥಾ ಯದಿ (tv1->v_type == VAR_BLOB || tv2->v_type == VAR_BLOB)
    {
	ಯದಿ (typval_compare_blob(tv1, tv2, type, res) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಅನ್ಯಥಾ ಯದಿ (tv1->v_type == VAR_LIST || tv2->v_type == VAR_LIST)
    {
	ಯದಿ (typval_compare_list(tv1, tv2, type, ic, res) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಅನ್ಯಥಾ ಯದಿ (tv1->v_type == VAR_TUPLE || tv2->v_type == VAR_TUPLE)
    {
	ಯದಿ (typval_compare_tuple(tv1, tv2, type, ic, res) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಅನ್ಯಥಾ ಯದಿ (tv1->v_type == VAR_OBJECT || tv2->v_type == VAR_OBJECT)
    {
	ಯದಿ (typval_compare_object(tv1, tv2, type, ic, res) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಅನ್ಯಥಾ ಯದಿ (tv1->v_type == VAR_DICT || tv2->v_type == VAR_DICT)
    {
	ಯದಿ (typval_compare_dict(tv1, tv2, type, ic, res) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಅನ್ಯಥಾ ಯದಿ (tv1->v_type == VAR_FUNC || tv2->v_type == VAR_FUNC
	|| tv1->v_type == VAR_PARTIAL || tv2->v_type == VAR_PARTIAL)
    {
	ಯದಿ (typval_compare_func(tv1, tv2, type, ic, res) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // If one of the two variables is a float, compare as a float.
    // When using "=~" or "!~", always compare as string.
    ಅನ್ಯಥಾ ಯದಿ ((tv1->v_type == VAR_FLOAT || tv2->v_type == VAR_FLOAT)
	    && type != ಅಭಿವ್ಯಕ್ತಿ_MATCH && type != ಅಭಿವ್ಯಕ್ತಿ_NOMATCH)
    {
	float_T f1, f2;
	ಪೂರ್ಣಾಙ್ಕ	error = FALSE;

	f1 = tv_get_float_chk(tv1, &error);
	ಯದಿ (!error)
	    f2 = tv_get_float_chk(tv2, &error);
	ಯದಿ (error)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	n1 = FALSE;
	ವಿಕಲ್ಪ (type)
	{
	    ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_IS:
	    ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_EQUAL:    n1 = (f1 == f2); ಭಙ್ಗ;
	    ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_ISNOT:
	    ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_NEQUAL:   n1 = (f1 != f2); ಭಙ್ಗ;
	    ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_GREATER:  n1 = (f1 > f2); ಭಙ್ಗ;
	    ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_GEQUAL:   n1 = (f1 >= f2); ಭಙ್ಗ;
	    ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_SMALLER:  n1 = (f1 < f2); ಭಙ್ಗ;
	    ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_SEQUAL:   n1 = (f1 <= f2); ಭಙ್ಗ;
	    ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_UNKNOWN:
	    ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_MATCH:
	    ಸ್ವಭಾವ:  ಭಙ್ಗ;  // avoid gcc warning
	}
	*res = n1;
    }

    // If one of the two variables is a number, compare as a number.
    // When using "=~" or "!~", always compare as string.
    ಅನ್ಯಥಾ ಯದಿ ((tv1->v_type == VAR_ಸಂಖ್ಯೆ || tv2->v_type == VAR_ಸಂಖ್ಯೆ)
	    && type != ಅಭಿವ್ಯಕ್ತಿ_MATCH && type != ಅಭಿವ್ಯಕ್ತಿ_NOMATCH)
    {
	ಪೂರ್ಣಾಙ್ಕ error = FALSE;

	n1 = tv_get_ಸಂಖ್ಯೆ_chk(tv1, &error);
	ಯದಿ (!error)
	    n2 = tv_get_ಸಂಖ್ಯೆ_chk(tv2, &error);
	ಯದಿ (error)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	ವಿಕಲ್ಪ (type)
	{
	    ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_IS:
	    ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_EQUAL:    n1 = (n1 == n2); ಭಙ್ಗ;
	    ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_ISNOT:
	    ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_NEQUAL:   n1 = (n1 != n2); ಭಙ್ಗ;
	    ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_GREATER:  n1 = (n1 > n2); ಭಙ್ಗ;
	    ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_GEQUAL:   n1 = (n1 >= n2); ಭಙ್ಗ;
	    ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_SMALLER:  n1 = (n1 < n2); ಭಙ್ಗ;
	    ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_SEQUAL:   n1 = (n1 <= n2); ಭಙ್ಗ;
	    ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_UNKNOWN:
	    ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_MATCH:
	    ಸ್ವಭಾವ:  ಭಙ್ಗ;  // avoid gcc warning
	}
	*res = n1;
    }
    ಅನ್ಯಥಾ ಯದಿ (in_vim9script() && (tv1->v_type == VAR_BOOL
				    || tv2->v_type == VAR_BOOL
				    || (tv1->v_type == VAR_SPECIAL
					      && tv2->v_type == VAR_SPECIAL)))
    {
	ಯದಿ (tv1->v_type != tv2->v_type)
	{
	    semsg(_(e_cannot_compare_str_with_str),
		       vartype_name(tv1->v_type), vartype_name(tv2->v_type));
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	n1 = tv1->vval.v_ಸಂಖ್ಯೆ;
	n2 = tv2->vval.v_ಸಂಖ್ಯೆ;
	ವಿಕಲ್ಪ (type)
	{
	    ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_IS:
	    ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_EQUAL:    n1 = (n1 == n2); ಭಙ್ಗ;
	    ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_ISNOT:
	    ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_NEQUAL:   n1 = (n1 != n2); ಭಙ್ಗ;
	    ಸ್ವಭಾವ:
		semsg(_(e_invalid_operation_for_str),
						   vartype_name(tv1->v_type));
		ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	*res = n1;
    }
#ifdef FEAT_JOB_CHANNEL
    ಅನ್ಯಥಾ ಯದಿ (tv1->v_type == tv2->v_type
	    && (tv1->v_type == VAR_CHANNEL || tv1->v_type == VAR_JOB)
	    && (type == ಅಭಿವ್ಯಕ್ತಿ_NEQUAL || type == ಅಭಿವ್ಯಕ್ತಿ_EQUAL))
    {
	ಯದಿ (tv1->v_type == VAR_CHANNEL)
	    n1 = tv1->vval.v_channel == tv2->vval.v_channel;
	ಅನ್ಯಥಾ
	    n1 = tv1->vval.v_job == tv2->vval.v_job;
	ಯದಿ (type == ಅಭಿವ್ಯಕ್ತಿ_NEQUAL)
	    n1 = !n1;
	*res = n1;
    }
#endif
    ಅನ್ಯಥಾ
    {
	ಯದಿ (typval_compare_string(tv1, tv2, type, ic, res) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Compare "tv1" and "tv2".
 * Put the result in "tv1".  Caller should clear "tv2".
 */
    ಪೂರ್ಣಾಙ್ಕ
typval_compare(
    typval_T	*tv1,	// first operand
    typval_T	*tv2,	// second operand
    exprtype_T	type,   // operator
    ಪೂರ್ಣಾಙ್ಕ		ic)     // ignore case
{
    ಪೂರ್ಣಾಙ್ಕ res;

    ಯದಿ (typval_compare2(tv1, tv2, type, ic, &res) == FAIL)
    {
	clear_tv(tv1);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    clear_tv(tv1);
    ಯದಿ (in_vim9script())
    {
	tv1->v_type = VAR_BOOL;
	tv1->vval.v_ಸಂಖ್ಯೆ = res ? VVAL_TRUE : VVAL_FALSE;
    }
    ಅನ್ಯಥಾ
    {
	tv1->v_type = VAR_ಸಂಖ್ಯೆ;
	tv1->vval.v_ಸಂಖ್ಯೆ = res;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Compare "tv1" to "tv2" as lists according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    ಪೂರ್ಣಾಙ್ಕ
typval_compare_list(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	ಪೂರ್ಣಾಙ್ಕ	    ic,
	ಪೂರ್ಣಾಙ್ಕ	    *res)
{
    ಪೂರ್ಣಾಙ್ಕ	    val = 0;

    ಯದಿ (type == ಅಭಿವ್ಯಕ್ತಿ_IS || type == ಅಭಿವ್ಯಕ್ತಿ_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
				      && tv1->vval.v_list == tv2->vval.v_list);
	ಯದಿ (type == ಅಭಿವ್ಯಕ್ತಿ_ISNOT)
	    val = !val;
    }
    ಅನ್ಯಥಾ ಯದಿ (tv1->v_type != tv2->v_type
	    || (type != ಅಭಿವ್ಯಕ್ತಿ_EQUAL && type != ಅಭಿವ್ಯಕ್ತಿ_NEQUAL))
    {
	ಯದಿ (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_list_with_list));
	ಅನ್ಯಥಾ
	    emsg(_(e_invalid_operation_for_list));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಅನ್ಯಥಾ
    {
	val = list_equal(tv1->vval.v_list, tv2->vval.v_list, ic);
	ಯದಿ (type == ಅಭಿವ್ಯಕ್ತಿ_NEQUAL)
	    val = !val;
    }
    *res = val;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Compare "tv1" to "tv2" as tuples according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    ಪೂರ್ಣಾಙ್ಕ
typval_compare_tuple(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	ಪೂರ್ಣಾಙ್ಕ	    ic,
	ಪೂರ್ಣಾಙ್ಕ	    *res)
{
    ಪೂರ್ಣಾಙ್ಕ	    val = 0;

    ಯದಿ (type == ಅಭಿವ್ಯಕ್ತಿ_IS || type == ಅಭಿವ್ಯಕ್ತಿ_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
			      && tv1->vval.v_tuple == tv2->vval.v_tuple);
	ಯದಿ (type == ಅಭಿವ್ಯಕ್ತಿ_ISNOT)
	    val = !val;
    }
    ಅನ್ಯಥಾ ಯದಿ (tv1->v_type != tv2->v_type
	    || (type != ಅಭಿವ್ಯಕ್ತಿ_EQUAL && type != ಅಭಿವ್ಯಕ್ತಿ_NEQUAL))
    {
	ಯದಿ (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_tuple_with_tuple));
	ಅನ್ಯಥಾ
	    emsg(_(e_invalid_operation_for_tuple));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಅನ್ಯಥಾ
    {
	val = tuple_equal(tv1->vval.v_tuple, tv2->vval.v_tuple, ic);
	ಯದಿ (type == ಅಭಿವ್ಯಕ್ತಿ_NEQUAL)
	    val = !val;
    }
    *res = val;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Compare v:null with another type.  Return TRUE if the value is NULL.
 */
    ಪೂರ್ಣಾಙ್ಕ
typval_compare_null(typval_T *tv1, typval_T *tv2)
{
    ಯದಿ ((tv1->v_type == VAR_SPECIAL && tv1->vval.v_ಸಂಖ್ಯೆ == VVAL_NULL)
	    || (tv2->v_type == VAR_SPECIAL && tv2->vval.v_ಸಂಖ್ಯೆ == VVAL_NULL))
    {
	typval_T	*tv = tv1->v_type == VAR_SPECIAL ? tv2 : tv1;

	ವಿಕಲ್ಪ (tv->v_type)
	{
	    ಪಕ್ಷ VAR_BLOB: ಪ್ರತ್ಯಾಗಮ tv->vval.v_blob == ರಿಕ್ತ;
#ifdef FEAT_JOB_CHANNEL
	    ಪಕ್ಷ VAR_CHANNEL: ಪ್ರತ್ಯಾಗಮ tv->vval.v_channel == ರಿಕ್ತ;
#endif
	    // TODO: null_class handling
	    // case VAR_CLASS: return tv->vval.v_class == NULL;
	    ಪಕ್ಷ VAR_DICT: ಪ್ರತ್ಯಾಗಮ tv->vval.v_dict == ರಿಕ್ತ;
	    ಪಕ್ಷ VAR_FUNC: ಪ್ರತ್ಯಾಗಮ tv->vval.v_string == ರಿಕ್ತ;
#ifdef FEAT_JOB_CHANNEL
	    ಪಕ್ಷ VAR_JOB: ಪ್ರತ್ಯಾಗಮ tv->vval.v_job == ರಿಕ್ತ;
#endif
	    ಪಕ್ಷ VAR_LIST: ಪ್ರತ್ಯಾಗಮ tv->vval.v_list == ರಿಕ್ತ;
	    ಪಕ್ಷ VAR_TUPLE: ಪ್ರತ್ಯಾಗಮ tv->vval.v_tuple == ರಿಕ್ತ;
	    ಪಕ್ಷ VAR_OBJECT: ಪ್ರತ್ಯಾಗಮ tv->vval.v_object == ರಿಕ್ತ;
	    ಪಕ್ಷ VAR_PARTIAL: ಪ್ರತ್ಯಾಗಮ tv->vval.v_partial == ರಿಕ್ತ;
	    ಪಕ್ಷ VAR_STRING: ಪ್ರತ್ಯಾಗಮ tv->vval.v_string == ರಿಕ್ತ;

	    ಪಕ್ಷ VAR_ಸಂಖ್ಯೆ: ಯದಿ (!in_vim9script())
				 ಪ್ರತ್ಯಾಗಮ tv->vval.v_ಸಂಖ್ಯೆ == 0;
			     ಭಙ್ಗ;
	    ಪಕ್ಷ VAR_FLOAT: ಯದಿ (!in_vim9script())
				 ಪ್ರತ್ಯಾಗಮ tv->vval.v_float == 0.0;
			     ಭಙ್ಗ;
	    ಪಕ್ಷ VAR_TYPEALIAS: ಪ್ರತ್ಯಾಗಮ tv->vval.v_typealias == ರಿಕ್ತ;
	    ಸ್ವಭಾವ: ಭಙ್ಗ;
	}
    }
    // although comparing null with number, float or bool is not very useful
    // we won't give an error
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Compare "tv1" to "tv2" as blobs according to "type".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    ಪೂರ್ಣಾಙ್ಕ
typval_compare_blob(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	ಪೂರ್ಣಾಙ್ಕ	    *res)
{
    ಪೂರ್ಣಾಙ್ಕ	    val = 0;

    ಯದಿ (type == ಅಭಿವ್ಯಕ್ತಿ_IS || type == ಅಭಿವ್ಯಕ್ತಿ_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
			&& tv1->vval.v_blob == tv2->vval.v_blob);
	ಯದಿ (type == ಅಭಿವ್ಯಕ್ತಿ_ISNOT)
	    val = !val;
    }
    ಅನ್ಯಥಾ ಯದಿ (tv1->v_type != tv2->v_type
	    || (type != ಅಭಿವ್ಯಕ್ತಿ_EQUAL && type != ಅಭಿವ್ಯಕ್ತಿ_NEQUAL))
    {
	ಯದಿ (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_blob_with_blob));
	ಅನ್ಯಥಾ
	    emsg(_(e_invalid_operation_for_blob));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಅನ್ಯಥಾ
    {
	val = blob_equal(tv1->vval.v_blob, tv2->vval.v_blob);
	ಯದಿ (type == ಅಭಿವ್ಯಕ್ತಿ_NEQUAL)
	    val = !val;
    }
    *res = val;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Compare "tv1" to "tv2" as objects according to "type".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    ಪೂರ್ಣಾಙ್ಕ
typval_compare_object(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	ಪೂರ್ಣಾಙ್ಕ	    ic,
	ಪೂರ್ಣಾಙ್ಕ	    *res)
{
    ಪೂರ್ಣಾಙ್ಕ res_match = type == ಅಭಿವ್ಯಕ್ತಿ_EQUAL || type == ಅಭಿವ್ಯಕ್ತಿ_IS ? TRUE : FALSE;

    ಯದಿ (tv1->vval.v_object == ರಿಕ್ತ && tv2->vval.v_object == ರಿಕ್ತ)
    {
	*res = res_match;
	ಪ್ರತ್ಯಾಗಮ OK;
    }
    ಯದಿ (tv1->vval.v_object == ರಿಕ್ತ || tv2->vval.v_object == ರಿಕ್ತ)
    {
	*res = !res_match;
	ಪ್ರತ್ಯಾಗಮ OK;
    }

    object_T *obj1 = tv1->vval.v_object;
    object_T *obj2 = tv2->vval.v_object;
    ಯದಿ (type == ಅಭಿವ್ಯಕ್ತಿ_IS || type == ಅಭಿವ್ಯಕ್ತಿ_ISNOT)
    {
	*res = obj1 == obj2 ? res_match : !res_match;
	ಪ್ರತ್ಯಾಗಮ OK;
    }
    ಅನ್ಯಥಾ ಯದಿ (tv1->v_type != tv2->v_type)
    {
	emsg(_(e_can_only_compare_object_with_object));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    *res = object_equal(obj1, obj2, ic) ? res_match : !res_match;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Compare "tv1" to "tv2" as dictionaries according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    ಪೂರ್ಣಾಙ್ಕ
typval_compare_dict(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	ಪೂರ್ಣಾಙ್ಕ	    ic,
	ಪೂರ್ಣಾಙ್ಕ	    *res)
{
    ಪೂರ್ಣಾಙ್ಕ	    val;

    ಯದಿ (type == ಅಭಿವ್ಯಕ್ತಿ_IS || type == ಅಭಿವ್ಯಕ್ತಿ_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
			&& tv1->vval.v_dict == tv2->vval.v_dict);
	ಯದಿ (type == ಅಭಿವ್ಯಕ್ತಿ_ISNOT)
	    val = !val;
    }
    ಅನ್ಯಥಾ ಯದಿ (tv1->v_type != tv2->v_type
		|| (type != ಅಭಿವ್ಯಕ್ತಿ_EQUAL && type != ಅಭಿವ್ಯಕ್ತಿ_NEQUAL))
    {
	ಯದಿ (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_dictionary_with_dictionary));
	ಅನ್ಯಥಾ
	    emsg(_(e_invalid_operation_for_dictionary));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಅನ್ಯಥಾ
    {
	val = dict_equal(tv1->vval.v_dict, tv2->vval.v_dict, ic);
	ಯದಿ (type == ಅಭಿವ್ಯಕ್ತಿ_NEQUAL)
	    val = !val;
    }
    *res = val;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Compare "tv1" to "tv2" as funcrefs according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    ಪೂರ್ಣಾಙ್ಕ
typval_compare_func(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	ಪೂರ್ಣಾಙ್ಕ	    ic,
	ಪೂರ್ಣಾಙ್ಕ	    *res)
{
    ಪೂರ್ಣಾಙ್ಕ	    val = 0;

    ಯದಿ (type != ಅಭಿವ್ಯಕ್ತಿ_EQUAL && type != ಅಭಿವ್ಯಕ್ತಿ_NEQUAL
	    && type != ಅಭಿವ್ಯಕ್ತಿ_IS && type != ಅಭಿವ್ಯಕ್ತಿ_ISNOT)
    {
	emsg(_(e_invalid_operation_for_funcrefs));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಯದಿ ((tv1->v_type == VAR_PARTIAL && tv1->vval.v_partial == ರಿಕ್ತ)
	    || (tv2->v_type == VAR_PARTIAL && tv2->vval.v_partial == ರಿಕ್ತ))
	// When both partials are NULL, then they are equal.
	// Otherwise they are not equal.
	val = (tv1->vval.v_partial == tv2->vval.v_partial);
    ಅನ್ಯಥಾ ಯದಿ (type == ಅಭಿವ್ಯಕ್ತಿ_IS || type == ಅಭಿವ್ಯಕ್ತಿ_ISNOT)
    {
	ಯದಿ (tv1->v_type == VAR_FUNC && tv2->v_type == VAR_FUNC)
	    // strings are considered the same if their value is
	    // the same
	    val = tv_equal(tv1, tv2, ic);
	ಅನ್ಯಥಾ ಯದಿ (tv1->v_type == VAR_PARTIAL && tv2->v_type == VAR_PARTIAL)
	    val = (tv1->vval.v_partial == tv2->vval.v_partial);
	ಅನ್ಯಥಾ
	    val = FALSE;
    }
    ಅನ್ಯಥಾ
	val = tv_equal(tv1, tv2, ic);
    ಯದಿ (type == ಅಭಿವ್ಯಕ್ತಿ_NEQUAL || type == ಅಭಿವ್ಯಕ್ತಿ_ISNOT)
	val = !val;
    *res = val;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Compare "tv1" to "tv2" as strings according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    ಪೂರ್ಣಾಙ್ಕ
typval_compare_string(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	ಪೂರ್ಣಾಙ್ಕ	    ic,
	ಪೂರ್ಣಾಙ್ಕ	    *res)
{
    ಪೂರ್ಣಾಙ್ಕ		i = 0;
    ಪೂರ್ಣಾಙ್ಕ		val = FALSE;
    char_u	*s1, *s2;
    char_u	buf1[NUMBUFLEN], buf2[NUMBUFLEN];

    ಯದಿ (in_vim9script()
	  && ((tv1->v_type != VAR_STRING && tv1->v_type != VAR_SPECIAL)
	   || (tv2->v_type != VAR_STRING && tv2->v_type != VAR_SPECIAL)))
    {
	semsg(_(e_cannot_compare_str_with_str),
		   vartype_name(tv1->v_type), vartype_name(tv2->v_type));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    s1 = tv_get_string_buf(tv1, buf1);
    s2 = tv_get_string_buf(tv2, buf2);
    ಯದಿ (type != ಅಭಿವ್ಯಕ್ತಿ_MATCH && type != ಅಭಿವ್ಯಕ್ತಿ_NOMATCH)
	i = ic ? MB_STRICMP(s1, s2) : STRCMP(s1, s2);
    ವಿಕಲ್ಪ (type)
    {
	ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_IS:	    ಯದಿ (in_vim9script())
			    {
				// Really check it is the same string, not just
				// the same value.
				val = tv1->vval.v_string == tv2->vval.v_string;
				ಭಙ್ಗ;
			    }
			    // FALLTHROUGH
	ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_EQUAL:    val = (i == 0); ಭಙ್ಗ;
	ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_ISNOT:    ಯದಿ (in_vim9script())
			    {
				// Really check it is not the same string, not
				// just a different value.
				val = tv1->vval.v_string != tv2->vval.v_string;
				ಭಙ್ಗ;
			    }
			    // FALLTHROUGH
	ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_NEQUAL:   val = (i != 0); ಭಙ್ಗ;
	ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_GREATER:  val = (i > 0); ಭಙ್ಗ;
	ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_GEQUAL:   val = (i >= 0); ಭಙ್ಗ;
	ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_SMALLER:  val = (i < 0); ಭಙ್ಗ;
	ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_SEQUAL:   val = (i <= 0); ಭಙ್ಗ;

	ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_MATCH:
	ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_NOMATCH:
		val = pattern_match(s2, s1, ic);
		ಯದಿ (type == ಅಭಿವ್ಯಕ್ತಿ_NOMATCH)
		    val = !val;
		ಭಙ್ಗ;

	ಸ್ವಭಾವ:  ಭಙ್ಗ;  // avoid gcc warning
    }
    *res = val;
    ಪ್ರತ್ಯಾಗಮ OK;
}
/*
 * Convert any type to a string, never give an error.
 * When "quotes" is TRUE add quotes to a string.
 * Returns an allocated string.
 */
    char_u *
typval_tostring(typval_T *arg, ಪೂರ್ಣಾಙ್ಕ quotes)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    char_u	*ret = ರಿಕ್ತ;

    ಯದಿ (arg == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ vim_strsave((char_u *)"(does not exist)");
    ಯದಿ (!quotes && arg->v_type == VAR_STRING)
    {
	ret = vim_strsave(arg->vval.v_string == ರಿಕ್ತ ? (char_u *)""
							 : arg->vval.v_string);
    }
    ಅನ್ಯಥಾ
    {
	ret = tv2string(arg, &tofree, numbuf, 0);
	// Make a copy if we have a value but it's not in allocated memory.
	ಯದಿ (ret != ರಿಕ್ತ && tofree == ರಿಕ್ತ)
	    ret = vim_strsave(ret);
    }
    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Return TRUE if typeval "tv" is locked: Either that value is locked itself
 * or it refers to a List or Dictionary that is locked.
 */
    ಪೂರ್ಣಾಙ್ಕ
tv_islocked(typval_T *tv)
{
    ಪ್ರತ್ಯಾಗಮ (tv->v_lock & VAR_LOCKED)
	|| (tv->v_type == VAR_LIST
		&& tv->vval.v_list != ರಿಕ್ತ
		&& (tv->vval.v_list->lv_lock & VAR_LOCKED))
	|| (tv->v_type == VAR_TUPLE
		&& tv->vval.v_tuple != ರಿಕ್ತ
		&& (tv->vval.v_tuple->tv_lock & VAR_LOCKED))
	|| (tv->v_type == VAR_DICT
		&& tv->vval.v_dict != ರಿಕ್ತ
		&& (tv->vval.v_dict->dv_lock & VAR_LOCKED));
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
func_equal(
    typval_T *tv1,
    typval_T *tv2,
    ಪೂರ್ಣಾಙ್ಕ	     ic)	    // ignore case
{
    char_u	*s1, *s2;
    dict_T	*d1, *d2;
    ಪೂರ್ಣಾಙ್ಕ		a1, a2;
    ಪೂರ್ಣಾಙ್ಕ		i;

    // empty and NULL function name considered the same
    s1 = tv1->v_type == VAR_FUNC ? tv1->vval.v_string
					   : partial_name(tv1->vval.v_partial);
    ಯದಿ (s1 != ರಿಕ್ತ && *s1 == NUL)
	s1 = ರಿಕ್ತ;
    s2 = tv2->v_type == VAR_FUNC ? tv2->vval.v_string
					   : partial_name(tv2->vval.v_partial);
    ಯದಿ (s2 != ರಿಕ್ತ && *s2 == NUL)
	s2 = ರಿಕ್ತ;
    ಯದಿ (s1 == ರಿಕ್ತ || s2 == ರಿಕ್ತ)
    {
	ಯದಿ (s1 != s2)
	    ಪ್ರತ್ಯಾಗಮ FALSE;
    }
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(s1, s2) != 0)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    // empty dict and NULL dict is different
    d1 = tv1->v_type == VAR_FUNC ? ರಿಕ್ತ : tv1->vval.v_partial->pt_dict;
    d2 = tv2->v_type == VAR_FUNC ? ರಿಕ್ತ : tv2->vval.v_partial->pt_dict;
    ಯದಿ (d1 == ರಿಕ್ತ || d2 == ರಿಕ್ತ)
    {
	ಯದಿ (d1 != d2)
	    ಪ್ರತ್ಯಾಗಮ FALSE;
    }
    ಅನ್ಯಥಾ ಯದಿ (!dict_equal(d1, d2, ic))
	ಪ್ರತ್ಯಾಗಮ FALSE;

    // empty list and no list considered the same
    a1 = tv1->v_type == VAR_FUNC ? 0 : tv1->vval.v_partial->pt_argc;
    a2 = tv2->v_type == VAR_FUNC ? 0 : tv2->vval.v_partial->pt_argc;
    ಯದಿ (a1 != a2)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಪ್ರತಿ (i = 0; i < a1; ++i)
	ಯದಿ (!tv_equal(tv1->vval.v_partial->pt_argv + i,
		      tv2->vval.v_partial->pt_argv + i, ic))
	    ಪ್ರತ್ಯಾಗಮ FALSE;

    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Return TRUE if "tv1" and "tv2" have the same value.
 * Compares the items just like "==" would compare them, but strings and
 * numbers are different.  Floats and numbers are also different.
 */
    ಪೂರ್ಣಾಙ್ಕ
tv_equal(
    typval_T *tv1,
    typval_T *tv2,
    ಪೂರ್ಣಾಙ್ಕ	     ic)	    // ignore case
{
    char_u	buf1[NUMBUFLEN], buf2[NUMBUFLEN];
    char_u	*s1, *s2;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ  recursive_cnt = 0;	    // catch recursive loops
    ಪೂರ್ಣಾಙ್ಕ		r;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	tv_equal_recurse_limit;

    // Catch lists and dicts that have an endless loop by limiting
    // recursiveness to a limit.  We guess they are equal then.
    // A fixed limit has the problem of still taking an awful long time.
    // Reduce the limit every time running into it. That should work fine for
    // deeply linked structures that are not recursively linked and catch
    // recursiveness quickly.
    ಯದಿ (recursive_cnt == 0)
	tv_equal_recurse_limit = 1000;
    ಯದಿ (recursive_cnt >= tv_equal_recurse_limit)
    {
	--tv_equal_recurse_limit;
	ಪ್ರತ್ಯಾಗಮ TRUE;
    }

    // For VAR_FUNC and VAR_PARTIAL compare the function name, bound dict and
    // arguments.
    ಯದಿ ((tv1->v_type == VAR_FUNC
		|| (tv1->v_type == VAR_PARTIAL && tv1->vval.v_partial != ರಿಕ್ತ))
	    && (tv2->v_type == VAR_FUNC
		|| (tv2->v_type == VAR_PARTIAL && tv2->vval.v_partial != ರಿಕ್ತ)))
    {
	++recursive_cnt;
	r = func_equal(tv1, tv2, ic);
	--recursive_cnt;
	ಪ್ರತ್ಯಾಗಮ r;
    }

    ಯದಿ (tv1->v_type != tv2->v_type
	    && ((tv1->v_type != VAR_BOOL && tv1->v_type != VAR_SPECIAL)
		|| (tv2->v_type != VAR_BOOL && tv2->v_type != VAR_SPECIAL)))
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ವಿಕಲ್ಪ (tv1->v_type)
    {
	ಪಕ್ಷ VAR_LIST:
	    ++recursive_cnt;
	    r = list_equal(tv1->vval.v_list, tv2->vval.v_list, ic);
	    --recursive_cnt;
	    ಪ್ರತ್ಯಾಗಮ r;

	ಪಕ್ಷ VAR_TUPLE:
	    ++recursive_cnt;
	    r = tuple_equal(tv1->vval.v_tuple, tv2->vval.v_tuple, ic);
	    --recursive_cnt;
	    ಪ್ರತ್ಯಾಗಮ r;

	ಪಕ್ಷ VAR_DICT:
	    ++recursive_cnt;
	    r = dict_equal(tv1->vval.v_dict, tv2->vval.v_dict, ic);
	    --recursive_cnt;
	    ಪ್ರತ್ಯಾಗಮ r;

	ಪಕ್ಷ VAR_BLOB:
	    ಪ್ರತ್ಯಾಗಮ blob_equal(tv1->vval.v_blob, tv2->vval.v_blob);

	ಪಕ್ಷ VAR_ಸಂಖ್ಯೆ:
	ಪಕ್ಷ VAR_BOOL:
	ಪಕ್ಷ VAR_SPECIAL:
	    ಪ್ರತ್ಯಾಗಮ tv1->vval.v_ಸಂಖ್ಯೆ == tv2->vval.v_ಸಂಖ್ಯೆ;

	ಪಕ್ಷ VAR_STRING:
	    s1 = tv_get_string_buf(tv1, buf1);
	    s2 = tv_get_string_buf(tv2, buf2);
	    ಪ್ರತ್ಯಾಗಮ ((ic ? MB_STRICMP(s1, s2) : STRCMP(s1, s2)) == 0);

	ಪಕ್ಷ VAR_FLOAT:
	    ಪ್ರತ್ಯಾಗಮ tv1->vval.v_float == tv2->vval.v_float;
	ಪಕ್ಷ VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    ಪ್ರತ್ಯಾಗಮ tv1->vval.v_job == tv2->vval.v_job;
#endif
	ಪಕ್ಷ VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    ಪ್ರತ್ಯಾಗಮ tv1->vval.v_channel == tv2->vval.v_channel;
#endif
	ಪಕ್ಷ VAR_INSTR:
	    ಪ್ರತ್ಯಾಗಮ tv1->vval.v_instr == tv2->vval.v_instr;

	ಪಕ್ಷ VAR_CLASS:
	    // A class only exists once, equality is identity.
	    ಪ್ರತ್ಯಾಗಮ tv1->vval.v_class == tv2->vval.v_class;

	ಪಕ್ಷ VAR_OBJECT:
	    ++recursive_cnt;
	    r = object_equal(tv1->vval.v_object, tv2->vval.v_object, ic);
	    --recursive_cnt;
	    ಪ್ರತ್ಯಾಗಮ r;

	ಪಕ್ಷ VAR_PARTIAL:
	    ಪ್ರತ್ಯಾಗಮ tv1->vval.v_partial == tv2->vval.v_partial;

	ಪಕ್ಷ VAR_FUNC:
	    ಪ್ರತ್ಯಾಗಮ tv1->vval.v_string == tv2->vval.v_string;

	ಪಕ್ಷ VAR_TYPEALIAS:
	    ಪ್ರತ್ಯಾಗಮ tv1->vval.v_typealias == tv2->vval.v_typealias;

	ಪಕ್ಷ VAR_UNKNOWN:
	ಪಕ್ಷ VAR_ANY:
	ಪಕ್ಷ VAR_VOID:
	    ಭಙ್ಗ;
    }

    // VAR_UNKNOWN can be the result of a invalid expression, let's say it
    // does not equal anything, not even itself.
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Get an option value.
 * "arg" points to the '&' or '+' before the option name.
 * "arg" is advanced to character after the option name.
 * Return OK or FAIL.
 */
    ಪೂರ್ಣಾಙ್ಕ
eval_option(
    char_u	**arg,
    typval_T	*rettv,	// when NULL, only check if option exists
    ಪೂರ್ಣಾಙ್ಕ		evaluate)
{
    char_u	*option_end;
    long	numval;
    char_u	*stringval;
    getoption_T	opt_type;
    ಪೂರ್ಣಾಙ್ಕ		c;
    ಪೂರ್ಣಾಙ್ಕ		working = (**arg == '+');    // has("+option")
    ಪೂರ್ಣಾಙ್ಕ		ret = OK;
    ಪೂರ್ಣಾಙ್ಕ		scope;

    // Isolate the option name and find its value.
    option_end = find_option_end(arg, &scope);
    ಯದಿ (option_end == ರಿಕ್ತ)
    {
	ಯದಿ (rettv != ರಿಕ್ತ)
	    semsg(_(e_option_name_missing_str), *arg);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಯದಿ (!evaluate)
    {
	*arg = option_end;
	ಪ್ರತ್ಯಾಗಮ OK;
    }

    c = *option_end;
    *option_end = NUL;
    opt_type = get_option_ಮೌಲ್ಯ(*arg, &numval,
			       rettv == ರಿಕ್ತ ? ರಿಕ್ತ : &stringval, ರಿಕ್ತ, scope);

    ಯದಿ (opt_type == gov_unknown)
    {
	ಯದಿ (rettv != ರಿಕ್ತ)
	    semsg(_(e_unknown_option_str), *arg);
	ret = FAIL;
    }
    ಅನ್ಯಥಾ ಯದಿ (rettv != ರಿಕ್ತ)
    {
	rettv->v_lock = 0;
	ಯದಿ (opt_type == gov_hidden_string)
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = ರಿಕ್ತ;
	}
	ಅನ್ಯಥಾ ಯದಿ (opt_type == gov_hidden_bool || opt_type == gov_hidden_ಸಂಖ್ಯೆ)
	{
	    rettv->v_type = in_vim9script() && opt_type == gov_hidden_bool
						       ? VAR_BOOL : VAR_ಸಂಖ್ಯೆ;
	    rettv->vval.v_ಸಂಖ್ಯೆ = 0;
	}
	ಅನ್ಯಥಾ ಯದಿ (opt_type == gov_bool || opt_type == gov_ಸಂಖ್ಯೆ)
	{
	    ಯದಿ (in_vim9script() && opt_type == gov_bool)
	    {
		rettv->v_type = VAR_BOOL;
		rettv->vval.v_ಸಂಖ್ಯೆ = numval ? VVAL_TRUE : VVAL_FALSE;
	    }
	    ಅನ್ಯಥಾ
	    {
		rettv->v_type = VAR_ಸಂಖ್ಯೆ;
		rettv->vval.v_ಸಂಖ್ಯೆ = numval;
	    }
	}
	ಅನ್ಯಥಾ				// string option
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = stringval;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (working && (opt_type == gov_hidden_bool
			|| opt_type == gov_hidden_ಸಂಖ್ಯೆ
			|| opt_type == gov_hidden_string))
	ret = FAIL;

    *option_end = c;		    // put back for error messages
    *arg = option_end;

    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Allocate a variable for a number constant.  Also deals with "0z" for blob.
 * Return OK or FAIL.
 */
    ಪೂರ್ಣಾಙ್ಕ
eval_ಸಂಖ್ಯೆ(
	char_u	    **arg,
	typval_T    *rettv,
	ಪೂರ್ಣಾಙ್ಕ	    evaluate,
	ಪೂರ್ಣಾಙ್ಕ	    want_string)
{
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		skip_quotes = !in_old_script(4);
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		get_float = FALSE;

    // We accept a float when the format matches
    // "[0-9]\+\.[0-9]\+\([eE][+-]\?[0-9]\+\)\?".  This is very
    // strict to avoid backwards compatibility problems.
    // With script version 2 and later the leading digit can be
    // omitted.
    // Don't look for a float after the "." operator, so that
    // ":let vers = 1.2.3" doesn't fail.
    ಯದಿ (**arg == '.')
	p = *arg;
    ಅನ್ಯಥಾ
    {
	p = *arg + 1;
	ಯದಿ (skip_quotes)
	    ಪ್ರತಿ (;;)
	    {
		ಯದಿ (*p == '\'')
		    ++p;
		ಯದಿ (!vim_isdigit(*p))
		    ಭಙ್ಗ;
		p = skipdigits(p);
	    }
	ಅನ್ಯಥಾ
	    p = skipdigits(p);
    }
    ಯದಿ (!want_string && p[0] == '.' && vim_isdigit(p[1]))
    {
	get_float = TRUE;
	p = skipdigits(p + 2);
	ಯದಿ (*p == 'e' || *p == 'E')
	{
	    ++p;
	    ಯದಿ (*p == '-' || *p == '+')
		++p;
	    ಯದಿ (!vim_isdigit(*p))
		get_float = FALSE;
	    ಅನ್ಯಥಾ
		p = skipdigits(p + 1);
	}
	ಯದಿ (ASCII_ISALPHA(*p) || *p == '.')
	    get_float = FALSE;
    }
    ಯದಿ (get_float)
    {
	float_T	f;

	*arg += string2float(*arg, &f, skip_quotes);
	ಯದಿ (evaluate)
	{
	    rettv->v_type = VAR_FLOAT;
	    rettv->vval.v_float = f;
	}
    }
    ಅನ್ಯಥಾ
    ಯದಿ (**arg == '0' && ((*arg)[1] == 'z' || (*arg)[1] == 'Z'))
    {
	char_u  *bp;
	blob_T  *blob = ರಿಕ್ತ;  // init for gcc

	// Blob constant: 0z0123456789abcdef
	ಯದಿ (evaluate)
	    blob = blob_alloc();
	ಪ್ರತಿ (bp = *arg + 2; vim_isxdigit(bp[0]); bp += 2)
	{
	    ಯದಿ (!vim_isxdigit(bp[1]))
	    {
		ಯದಿ (blob != ರಿಕ್ತ)
		{
		    emsg(_(e_blob_literal_should_have_an_even_ಸಂಖ್ಯೆ_of_hex_characters));
		    ga_clear(&blob->bv_ga);
		    VIM_CLEAR(blob);
		}
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    ಯದಿ (blob != ರಿಕ್ತ)
		ga_append(&blob->bv_ga,
			     (hex2nr(*bp) << 4) + hex2nr(*(bp+1)));
	    ಯದಿ (bp[2] == '.' && vim_isxdigit(bp[3]))
		++bp;
	}
	ಯದಿ (blob != ರಿಕ್ತ)
	    rettv_blob_set(rettv, blob);
	*arg = bp;
    }
    ಅನ್ಯಥಾ
    {
	varnumber_T	n;

	// decimal, hex or octal number
	vim_str2nr(*arg, ರಿಕ್ತ, &len, skip_quotes
		      ? STR2NR_NO_OCT + STR2NR_QUOTE
		      : STR2NR_ALL, &n, ರಿಕ್ತ, 0, TRUE, ರಿಕ್ತ);
	ಯದಿ (len == 0)
	{
	    ಯದಿ (evaluate)
		semsg(_(e_invalid_expression_str), *arg);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	*arg += len;
	ಯದಿ (evaluate)
	{
	    rettv->v_type = VAR_ಸಂಖ್ಯೆ;
	    rettv->vval.v_ಸಂಖ್ಯೆ = n;
	}
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Evaluate a string constant and put the result in "rettv".
 * "*arg" points to the double quote or to after it when "interpolate" is TRUE.
 * When "interpolate" is TRUE reduce "{{" to "{", reduce "}}" to "}" and stop
 * at a single "{".
 * Return OK or FAIL.
 */
    ಪೂರ್ಣಾಙ್ಕ
eval_string(char_u **arg, typval_T *rettv, ಪೂರ್ಣಾಙ್ಕ evaluate, ಪೂರ್ಣಾಙ್ಕ interpolate)
{
    char_u	*p;
    char_u	*end;
    ಪೂರ್ಣಾಙ್ಕ		extra = interpolate ? 1 : 0;
    ಪೂರ್ಣಾಙ್ಕ		off = interpolate ? 0 : 1;
    ಪೂರ್ಣಾಙ್ಕ		len;

    // Find the end of the string, skipping backslashed characters.
    ಪ್ರತಿ (p = *arg + off; *p != NUL && *p != '"'; MB_PTR_ADV(p))
    {
	ಯದಿ (*p == '\\' && p[1] != NUL)
	{
	    ++p;
	    // A "\<x>" form occupies at least 4 characters, and produces up
	    // to 9 characters (6 for the char and 3 for a modifier):
	    // reserve space for 5 extra.
	    ಯದಿ (*p == '<')
	    {
		ಪೂರ್ಣಾಙ್ಕ		modifiers = 0;
		ಪೂರ್ಣಾಙ್ಕ		flags = FSK_KEYCODE | FSK_IN_STRING;

		extra += 5;

		// Skip to the '>' to avoid using '{' inside for string
		// interpolation.
		ಯದಿ (p[1] != '*')
		    flags |= FSK_SIMPLIFY;
		ಯದಿ (find_special_ಕೀಲಿ(&p, &modifiers, flags, ರಿಕ್ತ) != 0)
		    --p;  // leave "p" on the ">"
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (interpolate && (*p == '{' || *p == '}'))
	{
	    ಯದಿ (*p == '{' && p[1] != '{') // start of expression
		ಭಙ್ಗ;
	    ++p;
	    ಯದಿ (p[-1] == '}' && *p != '}') // single '}' is an error
	    {
		semsg(_(e_stray_closing_curly_str), *arg);
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    --extra;  // "{{" becomes "{", "}}" becomes "}"
	}
    }

    ಯದಿ (*p != '"' && !(interpolate && *p == '{'))
    {
	semsg(_(e_missing_double_quote_str), *arg);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // If only parsing, set *arg and return here
    ಯದಿ (!evaluate)
    {
	*arg = p + off;
	ಪ್ರತ್ಯಾಗಮ OK;
    }

    // Copy the string into allocated memory, handling backslashed
    // characters.
    rettv->v_type = VAR_STRING;
    len = (ಪೂರ್ಣಾಙ್ಕ)(p - *arg + extra);
    rettv->vval.v_string = alloc(len);
    ಯದಿ (rettv->vval.v_string == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    end = rettv->vval.v_string;

    ಪ್ರತಿ (p = *arg + off; *p != NUL && *p != '"'; )
    {
	ಯದಿ (*p == '\\')
	{
	    ವಿಕಲ್ಪ (*++p)
	    {
		ಪಕ್ಷ 'b': *end++ = BS; ++p; ಭಙ್ಗ;
		ಪಕ್ಷ 'e': *end++ = ESC; ++p; ಭಙ್ಗ;
		ಪಕ್ಷ 'f': *end++ = FF; ++p; ಭಙ್ಗ;
		ಪಕ್ಷ 'n': *end++ = NL; ++p; ಭಙ್ಗ;
		ಪಕ್ಷ 'r': *end++ = CAR; ++p; ಭಙ್ಗ;
		ಪಕ್ಷ 't': *end++ = TAB; ++p; ಭಙ್ಗ;

		ಪಕ್ಷ 'X': // hex: "\x1", "\x12"
		ಪಕ್ಷ 'x':
		ಪಕ್ಷ 'u': // Unicode: "\u0023"
		ಪಕ್ಷ 'U':
			  ಯದಿ (vim_isxdigit(p[1]))
			  {
			      ಪೂರ್ಣಾಙ್ಕ	n, nr;
			      ಪೂರ್ಣಾಙ್ಕ	c = SAFE_toupper(*p);

			      ಯದಿ (c == 'X')
				  n = 2;
			      ಅನ್ಯಥಾ ಯದಿ (*p == 'u')
				  n = 4;
			      ಅನ್ಯಥಾ
				  n = 8;
			      nr = 0;
			      ಯಾವತ್ (--n >= 0 && vim_isxdigit(p[1]))
			      {
				  ++p;
				  nr = (nr << 4) + hex2nr(*p);
			      }
			      ++p;
			      // For "\u" store the number according to
			      // 'encoding'.
			      ಯದಿ (c != 'X')
				  end += (*mb_char2bytes)(nr, end);
			      ಅನ್ಯಥಾ
				  *end++ = nr;
			  }
			  ಭಙ್ಗ;

			  // octal: "\1", "\12", "\123"
		ಪಕ್ಷ '0':
		ಪಕ್ಷ '1':
		ಪಕ್ಷ '2':
		ಪಕ್ಷ '3':
		ಪಕ್ಷ '4':
		ಪಕ್ಷ '5':
		ಪಕ್ಷ '6':
		ಪಕ್ಷ '7': *end = *p++ - '0';
			  ಯದಿ (*p >= '0' && *p <= '7')
			  {
			      *end = (*end << 3) + *p++ - '0';
			      ಯದಿ (*p >= '0' && *p <= '7')
				  *end = (*end << 3) + *p++ - '0';
			  }
			  ++end;
			  ಭಙ್ಗ;

			  // Special key, e.g.: "\<C-W>"
		ಪಕ್ಷ '<':
			  {
			      ಪೂರ್ಣಾಙ್ಕ flags = FSK_KEYCODE | FSK_IN_STRING;

			      ಯದಿ (p[1] != '*')
				  flags |= FSK_SIMPLIFY;
			      extra = trans_special(&p, end, flags, FALSE, ರಿಕ್ತ);
			      ಯದಿ (extra != 0)
			      {
				  end += extra;
				  ಯದಿ (end >= rettv->vval.v_string + len)
				      iemsg("eval_string() used more space than allocated");
				  ಭಙ್ಗ;
			      }
			  }
			  // FALLTHROUGH

		ಸ್ವಭಾವ: MB_COPY_CHAR(p, end);
			  ಭಙ್ಗ;
	    }
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (interpolate && (*p == '{' || *p == '}'))
	    {
		ಯದಿ (*p == '{' && p[1] != '{') // start of expression
		    ಭಙ್ಗ;
		++p;  // reduce "{{" to "{" and "}}" to "}"
	    }
	    MB_COPY_CHAR(p, end);
	}
    }
    *end = NUL;
    ಯದಿ (*p == '"' && !interpolate)
	++p;
    *arg = p;

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Allocate a variable for a 'str''ing' constant.
 * When "interpolate" is TRUE reduce "{{" to "{" and stop at a single "{".
 * Return OK when a "rettv" was set to the string.
 * Return FAIL on error, "rettv" is not set.
 */
    ಪೂರ್ಣಾಙ್ಕ
eval_lit_string(char_u **arg, typval_T *rettv, ಪೂರ್ಣಾಙ್ಕ evaluate, ಪೂರ್ಣಾಙ್ಕ interpolate)
{
    char_u	*p;
    char_u	*str;
    ಪೂರ್ಣಾಙ್ಕ		reduce = interpolate ? -1 : 0;
    ಪೂರ್ಣಾಙ್ಕ		off = interpolate ? 0 : 1;

    // Find the end of the string, skipping ''.
    ಪ್ರತಿ (p = *arg + off; *p != NUL; MB_PTR_ADV(p))
    {
	ಯದಿ (*p == '\'')
	{
	    ಯದಿ (p[1] != '\'')
		ಭಙ್ಗ;
	    ++reduce;
	    ++p;
	}
	ಅನ್ಯಥಾ ಯದಿ (interpolate)
	{
	    ಯದಿ (*p == '{')
	    {
		ಯದಿ (p[1] != '{')
		    ಭಙ್ಗ;
		++p;
		++reduce;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (*p == '}')
	    {
		++p;
		ಯದಿ (*p != '}')
		{
		    semsg(_(e_stray_closing_curly_str), *arg);
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
		++reduce;
	    }
	}
    }

    ಯದಿ (*p != '\'' && !(interpolate && *p == '{'))
    {
	semsg(_(e_missing_single_quote_str), *arg);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // If only parsing return after setting "*arg"
    ಯದಿ (!evaluate)
    {
	*arg = p + off;
	ಪ್ರತ್ಯಾಗಮ OK;
    }

    // Copy the string into allocated memory, handling '' to ' reduction and
    // any expressions.
    str = alloc((p - *arg) - reduce);
    ಯದಿ (str == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = str;

    ಪ್ರತಿ (p = *arg + off; *p != NUL; )
    {
	ಯದಿ (*p == '\'')
	{
	    ಯದಿ (p[1] != '\'')
		ಭಙ್ಗ;
	    ++p;
	}
	ಅನ್ಯಥಾ ಯದಿ (interpolate && (*p == '{' || *p == '}'))
	{
	    ಯದಿ (*p == '{' && p[1] != '{')
		ಭಙ್ಗ;
	    ++p;
	}
	MB_COPY_CHAR(p, str);
    }
    *str = NUL;
    *arg = p + off;

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Evaluate a single or double quoted string possibly containing expressions.
 * "arg" points to the '$'.  The result is put in "rettv".
 * Returns OK or FAIL.
 */
    ಪೂರ್ಣಾಙ್ಕ
eval_interp_string(char_u **arg, typval_T *rettv, ಪೂರ್ಣಾಙ್ಕ evaluate)
{
    typval_T	tv;
    ಪೂರ್ಣಾಙ್ಕ		ret = OK;
    ಪೂರ್ಣಾಙ್ಕ		quote;
    garray_T	ga;
    char_u	*p;

    ga_init2(&ga, 1, 80);

    // *arg is on the '$' character, move it to the first string character.
    ++*arg;
    quote = **arg;
    ++*arg;

    ಪ್ರತಿ (;;)
    {
	// Get the string up to the matching quote or to a single '{'.
	// "arg" is advanced to either the quote or the '{'.
	ಯದಿ (quote == '"')
	    ret = eval_string(arg, &tv, evaluate, TRUE);
	ಅನ್ಯಥಾ
	    ret = eval_lit_string(arg, &tv, evaluate, TRUE);
	ಯದಿ (ret == FAIL)
	    ಭಙ್ಗ;
	ಯದಿ (evaluate)
	{
	    ga_concat(&ga, tv.vval.v_string);
	    clear_tv(&tv);
	}

	ಯದಿ (**arg != '{')
	{
	    // found terminating quote
	    ++*arg;
	    ಭಙ್ಗ;
	}
	p = eval_one_ಅಭಿವ್ಯಕ್ತಿ_in_str(*arg, &ga, evaluate);
	ಯದಿ (p == ರಿಕ್ತ)
	{
	    ret = FAIL;
	    ಭಙ್ಗ;
	}
	*arg = p;
    }

    rettv->v_type = VAR_STRING;
    ಯದಿ (ret == FAIL || !evaluate || ga_append(&ga, NUL) == FAIL)
    {
	ga_clear(&ga);
	rettv->vval.v_string = ರಿಕ್ತ;
	ಪ್ರತ್ಯಾಗಮ ret;
    }

    rettv->vval.v_string = ga.ga_data;
    ಪ್ರತ್ಯಾಗಮ OK;
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
    ಪೂರ್ಣಾಙ್ಕ		copyID)
{
    ಪ್ರತ್ಯಾಗಮ echo_string_core(tv, tofree, numbuf, copyID, FALSE, TRUE, FALSE);
}

/*
 * Get the value of an environment variable.
 * "arg" is pointing to the '$'.  It is advanced to after the name.
 * If the environment variable was not set, silently assume it is empty.
 * Return FAIL if the name is invalid.
 */
    ಪೂರ್ಣಾಙ್ಕ
eval_env_var(char_u **arg, typval_T *rettv, ಪೂರ್ಣಾಙ್ಕ evaluate)
{
    char_u	*string = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		cc;
    char_u	*name;
    ಪೂರ್ಣಾಙ್ಕ		mustfree = FALSE;

    ++*arg;
    name = *arg;
    len = get_env_len(arg);
    ಯದಿ (evaluate)
    {
	ಯದಿ (len == 0)
	    ಪ್ರತ್ಯಾಗಮ FAIL; // invalid empty name

	cc = name[len];
	name[len] = NUL;
	// first try vim_getenv(), fast for normal environment vars
	string = vim_getenv(name, &mustfree);
	ಯದಿ (string != ರಿಕ್ತ && *string != NUL)
	{
	    ಯದಿ (!mustfree)
		string = vim_strsave(string);
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (mustfree)
		vim_free(string);

	    // next try expanding things like $VIM and ${HOME}
	    string = expand_env_save(name - 1);
	    ಯದಿ (string != ರಿಕ್ತ && *string == '$')
		VIM_CLEAR(string);
	}
	name[len] = cc;

	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = string;
	rettv->v_lock = 0;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
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
    ಪೂರ್ಣಾಙ್ಕ		did_emsg_before = did_emsg;

    ಯದಿ (argvars[0].v_type != VAR_STRING || !in_vim9script())
	lnum = (linenr_T)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[0], ರಿಕ್ತ);
    ಯದಿ (lnum <= 0 && did_emsg_before == did_emsg
					    && argvars[0].v_type != VAR_ಸಂಖ್ಯೆ)
    {
	ಪೂರ್ಣಾಙ್ಕ	fnum;
	pos_T	*fp;

	// no valid number, try using arg like line()
	fp = var2fpos(&argvars[0], TRUE, &fnum, FALSE);
	ಯದಿ (fp != ರಿಕ್ತ)
	    lnum = fp->lnum;
    }
    ಪ್ರತ್ಯಾಗಮ lnum;
}

/*
 * Get the lnum from the first argument.
 * Also accepts "$", then "buf" is used.
 * Returns 0 on error.
 */
    linenr_T
tv_get_lnum_buf(typval_T *argvars, buf_T *buf)
{
    ಯದಿ (argvars[0].v_type == VAR_STRING
	    && argvars[0].vval.v_string != ರಿಕ್ತ
	    && argvars[0].vval.v_string[0] == '$'
	    && argvars[0].vval.v_string[1] == NUL
	    && buf != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    ಪ್ರತ್ಯಾಗಮ (linenr_T)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[0], ರಿಕ್ತ);
}

/*
 * Get buffer by number or pattern.
 */
    buf_T *
tv_get_buf(typval_T *tv, ಪೂರ್ಣಾಙ್ಕ curtab_only)
{
    char_u	*name = tv->vval.v_string;
    buf_T	*buf;

    ಯದಿ (tv->v_type == VAR_ಸಂಖ್ಯೆ)
	ಪ್ರತ್ಯಾಗಮ buflist_findnr((ಪೂರ್ಣಾಙ್ಕ)tv->vval.v_ಸಂಖ್ಯೆ);
    ಯದಿ (tv->v_type != VAR_STRING)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಯದಿ (name == ರಿಕ್ತ || *name == NUL)
	ಪ್ರತ್ಯಾಗಮ curbuf;
    ಯದಿ (name[0] == '$' && name[1] == NUL)
	ಪ್ರತ್ಯಾಗಮ lastbuf;

    buf = buflist_find_by_name(name, curtab_only);

    // If not found, try expanding the name, like done for bufexists().
    ಯದಿ (buf == ರಿಕ್ತ)
	buf = find_buffer(tv);

    ಪ್ರತ್ಯಾಗಮ buf;
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
    ಯದಿ (buf == ರಿಕ್ತ
	    && tv->v_type != VAR_ಸಂಖ್ಯೆ
	    && tv->v_type != VAR_STRING)
	// issue errmsg for type error
	(ಶೂನ್ಯ)tv_get_ಸಂಖ್ಯೆ(tv);
    ಪ್ರತ್ಯಾಗಮ buf;
}

/*
 * Check that repeating "slen" items "n" times, does not
 * overflow the int used for the resulting length.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_repeat_ಎಣಿಕೆ(varnumber_T slen, varnumber_T n)
{
    // Use a division so that the multiplication cannot overflow.
    ಯದಿ (n <= (varnumber_T)INT_MAX / slen)
	ಪ್ರತ್ಯಾಗಮ OK;

    ವರ್ಣ	buf[NUMBUFLEN];

    vim_snprintf(buf, ಪರಿಮಾಣ(buf), "%lld", (long long)n);
    semsg(_(e_val_too_large), buf);
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

#endif // FEAT_EVAL
