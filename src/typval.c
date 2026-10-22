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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_EVAL)

/*
 * Allocate memory for a variable type-value, and make it empty (0 or NULL
 * value).
 */
    typval_T *
alloc_tv(ਖਾਲੀ)
{
    ਵਾਪਸ ALLOC_CLEAR_ONE(typval_T);
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
    ਜੇ (rettv != ਨਿਰਾ)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = s;
    }
    ਨਹੀਂਤਾਂ
	vim_free(s);
    ਵਾਪਸ rettv;
}

/*
 * Free the memory for a variable type-value.
 */
    ਖਾਲੀ
free_tv(typval_T *varp)
{
    ਜੇ (varp == ਨਿਰਾ)
	ਵਾਪਸ;

    ਚੋਣ (varp->v_type)
    {
	ਹਾਲਤ VAR_FUNC:
	    func_unref(varp->vval.v_string);
	    // FALLTHROUGH
	ਹਾਲਤ VAR_STRING:
	    vim_free(varp->vval.v_string);
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_PARTIAL:
	    partial_unref(varp->vval.v_partial);
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_BLOB:
	    blob_unref(varp->vval.v_blob);
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_LIST:
	    list_unref(varp->vval.v_list);
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_TUPLE:
	    tuple_unref(varp->vval.v_tuple);
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_DICT:
	    dict_unref(varp->vval.v_dict);
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_ਕੰਮ:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
	    ਕੰਮ_unref(varp->vval.v_ਕੰਮ);
	    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
	ਹਾਲਤ VAR_CHANNEL:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
	    channel_unref(varp->vval.v_channel);
	    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
	ਹਾਲਤ VAR_CLASS:
	    class_unref(varp->vval.v_class);
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_OBJECT:
	    object_unref(varp->vval.v_object);
	    ਰੋਕੋ;

	ਹਾਲਤ VAR_TYPEALIAS:
	    typealias_unref(varp->vval.v_typealias);
	    ਰੋਕੋ;

	ਹਾਲਤ VAR_ਸੰਖਿਆ:
	ਹਾਲਤ VAR_FLOAT:
	ਹਾਲਤ VAR_ANY:
	ਹਾਲਤ VAR_UNKNOWN:
	ਹਾਲਤ VAR_VOID:
	ਹਾਲਤ VAR_BOOL:
	ਹਾਲਤ VAR_SPECIAL:
	ਹਾਲਤ VAR_INSTR:
	    ਰੋਕੋ;
    }
    vim_free(varp);
}

/*
 * Free the memory for a variable value and set the value to NULL or 0.
 */
    ਖਾਲੀ
clear_tv(typval_T *varp)
{
    ਜੇ (varp == ਨਿਰਾ)
	ਵਾਪਸ;

    ਚੋਣ (varp->v_type)
    {
	ਹਾਲਤ VAR_FUNC:
	    func_unref(varp->vval.v_string);
	    // FALLTHROUGH
	ਹਾਲਤ VAR_STRING:
	    VIM_CLEAR(varp->vval.v_string);
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_PARTIAL:
	    partial_unref(varp->vval.v_partial);
	    varp->vval.v_partial = ਨਿਰਾ;
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_BLOB:
	    blob_unref(varp->vval.v_blob);
	    varp->vval.v_blob = ਨਿਰਾ;
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_LIST:
	    list_unref(varp->vval.v_list);
	    varp->vval.v_list = ਨਿਰਾ;
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_TUPLE:
	    tuple_unref(varp->vval.v_tuple);
	    varp->vval.v_tuple = ਨਿਰਾ;
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_DICT:
	    dict_unref(varp->vval.v_dict);
	    varp->vval.v_dict = ਨਿਰਾ;
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_ਸੰਖਿਆ:
	ਹਾਲਤ VAR_BOOL:
	ਹਾਲਤ VAR_SPECIAL:
	    varp->vval.v_ਸੰਖਿਆ = 0;
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_FLOAT:
	    varp->vval.v_float = 0.0;
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_ਕੰਮ:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
	    ਕੰਮ_unref(varp->vval.v_ਕੰਮ);
	    varp->vval.v_ਕੰਮ = ਨਿਰਾ;
#ਜੇ_ਅੰਤ
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_CHANNEL:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
	    channel_unref(varp->vval.v_channel);
	    varp->vval.v_channel = ਨਿਰਾ;
#ਜੇ_ਅੰਤ
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_INSTR:
	    VIM_CLEAR(varp->vval.v_instr);
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_CLASS:
	    class_unref(varp->vval.v_class);
	    varp->vval.v_class = ਨਿਰਾ;
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_OBJECT:
	    object_unref(varp->vval.v_object);
	    varp->vval.v_object = ਨਿਰਾ;
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_TYPEALIAS:
	    typealias_unref(varp->vval.v_typealias);
	    varp->vval.v_typealias = ਨਿਰਾ;
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_UNKNOWN:
	ਹਾਲਤ VAR_ANY:
	ਹਾਲਤ VAR_VOID:
	    ਰੋਕੋ;
    }
    varp->v_ਤਾਲਾ = 0;
}

/*
 * Set the value of a variable to NULL without freeing items.
 */
    ਖਾਲੀ
init_tv(typval_T *varp)
{
    ਜੇ (varp != ਨਿਰਾ)
	CLEAR_ਸੂਚਕ(varp);
}

    ਸਥਿਰ varnumber_T
tv_get_bool_or_ਸੰਖਿਆ_chk(
	typval_T    *varp,
	ਪੂਰਨ	    *denote,
	ਪੂਰਨ	    want_bool,
	ਪੂਰਨ	    vim9_string_error) // in Vim9 using a string is an error
{
    varnumber_T	n = 0L;

    ਚੋਣ (varp->v_type)
    {
	ਹਾਲਤ VAR_ਸੰਖਿਆ:
	    ਜੇ (ਵਿੱਚ_vim9script() && want_bool && varp->vval.v_ਸੰਖਿਆ != 0
						   && varp->vval.v_ਸੰਖਿਆ != 1)
	    {
		semsg(_(e_using_ਸੰਖਿਆ_as_bool_nr), varp->vval.v_ਸੰਖਿਆ);
		ਰੋਕੋ;
	    }
	    ਵਾਪਸ varp->vval.v_ਸੰਖਿਆ;
	ਹਾਲਤ VAR_FLOAT:
	    emsg(_(e_using_float_as_ਸੰਖਿਆ));
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_FUNC:
	ਹਾਲਤ VAR_PARTIAL:
	    emsg(_(e_using_funcref_as_ਸੰਖਿਆ));
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_STRING:
	    ਜੇ (vim9_string_error && ਵਿੱਚ_vim9script())
	    {
		emsg_using_string_as(varp, !want_bool);
		ਰੋਕੋ;
	    }
	    ਜੇ (varp->vval.v_string != ਨਿਰਾ)
		vim_str2nr(varp->vval.v_string, ਨਿਰਾ, ਨਿਰਾ,
					 STR2NR_ALL, &n, ਨਿਰਾ, 0, FALSE, ਨਿਰਾ);
	    ਵਾਪਸ n;
	ਹਾਲਤ VAR_LIST:
	    emsg(_(e_using_list_as_ਸੰਖਿਆ));
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_TUPLE:
	    emsg(_(e_using_tuple_as_ਸੰਖਿਆ));
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_DICT:
	    emsg(_(e_using_dictionary_as_ਸੰਖਿਆ));
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_BOOL:
	ਹਾਲਤ VAR_SPECIAL:
	    ਜੇ (!want_bool && ਵਿੱਚ_vim9script())
	    {
		ਜੇ (varp->v_type == VAR_BOOL)
		    emsg(_(e_using_bool_as_ਸੰਖਿਆ));
		ਨਹੀਂਤਾਂ
		    emsg(_(e_using_special_as_ਸੰਖਿਆ));
		ਰੋਕੋ;
	    }
	    ਵਾਪਸ varp->vval.v_ਸੰਖਿਆ == VVAL_TRUE ? 1 : 0;
	ਹਾਲਤ VAR_ਕੰਮ:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
	    emsg(_(e_using_ਕੰਮ_as_ਸੰਖਿਆ));
	    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
	ਹਾਲਤ VAR_CHANNEL:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
	    emsg(_(e_using_channel_as_ਸੰਖਿਆ));
	    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
	ਹਾਲਤ VAR_BLOB:
	    emsg(_(e_using_blob_as_ਸੰਖਿਆ));
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_CLASS:
	ਹਾਲਤ VAR_TYPEALIAS:
	    check_typval_is_ਮੁੱਲ(varp);
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_OBJECT:
	    {
		ਜੇ (varp->vval.v_object == ਨਿਰਾ)
		    emsg(_(e_using_object_as_string));
		ਨਹੀਂਤਾਂ
		{
		    class_T *cl = varp->vval.v_object->obj_class;
		    ਜੇ (cl != ਨਿਰਾ && IS_ENUM(cl))
			semsg(_(e_using_enum_str_as_ਸੰਖਿਆ), cl->class_name.string);
		    ਨਹੀਂਤਾਂ
			emsg(_(e_using_object_as_ਸੰਖਿਆ));
		}
	    }
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_VOID:
	    emsg(_(e_cannot_use_void_ਮੁੱਲ));
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_UNKNOWN:
	ਹਾਲਤ VAR_ANY:
	ਹਾਲਤ VAR_INSTR:
	    ਅੰਦਰੂਨੀ_error_no_abort("tv_get_number(UNKNOWN)");
	    ਰੋਕੋ;
    }
    ਜੇ (denote == ਨਿਰਾ)		// useful for values that must be unsigned
	n = -1;
    ਨਹੀਂਤਾਂ
	*denote = TRUE;
    ਵਾਪਸ n;
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
tv_get_ਸੰਖਿਆ(typval_T *varp)
{
    ਪੂਰਨ		error = FALSE;

    ਵਾਪਸ tv_get_ਸੰਖਿਆ_chk(varp, &error);	// return 0L on error
}

/*
 * Like tv_get_number() but in Vim9 script do convert a number in a string to a
 * number without giving an error.
 */
    varnumber_T
tv_ਮੰਜ਼ਿਲ_ਸੰਖਿਆ(typval_T *varp)
{
    ਪੂਰਨ		error = FALSE;

    ਵਾਪਸ tv_get_bool_or_ਸੰਖਿਆ_chk(varp, &error, FALSE, FALSE);
}

    varnumber_T
tv_get_ਸੰਖਿਆ_chk(typval_T *varp, ਪੂਰਨ *denote)
{
    ਵਾਪਸ tv_get_bool_or_ਸੰਖਿਆ_chk(varp, denote, FALSE, TRUE);
}

/*
 * Get the boolean value of "varp".  This is like tv_get_number_chk(),
 * but in Vim9 script accepts Number (0 and 1) and Bool/Special.
 */
    varnumber_T
tv_get_bool(typval_T *varp)
{
    ਵਾਪਸ tv_get_bool_or_ਸੰਖਿਆ_chk(varp, ਨਿਰਾ, TRUE, TRUE);
}

/*
 * Get the boolean value of "varp".  This is like tv_get_number_chk(),
 * but in Vim9 script accepts Number and Bool.
 */
    varnumber_T
tv_get_bool_chk(typval_T *varp, ਪੂਰਨ *denote)
{
    ਵਾਪਸ tv_get_bool_or_ਸੰਖਿਆ_chk(varp, denote, TRUE, TRUE);
}

    ਸਥਿਰ float_T
tv_get_float_chk(typval_T *varp, ਪੂਰਨ *error)
{
    ਚੋਣ (varp->v_type)
    {
	ਹਾਲਤ VAR_ਸੰਖਿਆ:
	    ਵਾਪਸ (float_T)(varp->vval.v_ਸੰਖਿਆ);
	ਹਾਲਤ VAR_FLOAT:
	    ਵਾਪਸ varp->vval.v_float;
	ਹਾਲਤ VAR_FUNC:
	ਹਾਲਤ VAR_PARTIAL:
	    emsg(_(e_using_funcref_as_float));
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_STRING:
	    emsg(_(e_using_string_as_float));
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_LIST:
	    emsg(_(e_using_list_as_float));
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_TUPLE:
	    emsg(_(e_using_tuple_as_float));
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_DICT:
	    emsg(_(e_using_dictionary_as_float));
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_BOOL:
	    emsg(_(e_using_boolean_ਮੁੱਲ_as_float));
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_SPECIAL:
	    emsg(_(e_using_special_ਮੁੱਲ_as_float));
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_ਕੰਮ:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
	    emsg(_(e_using_ਕੰਮ_as_float));
	    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
	ਹਾਲਤ VAR_CHANNEL:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
	    emsg(_(e_using_channel_as_float));
	    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
	ਹਾਲਤ VAR_BLOB:
	    emsg(_(e_using_blob_as_float));
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_CLASS:
	ਹਾਲਤ VAR_TYPEALIAS:
	    check_typval_is_ਮੁੱਲ(varp);
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_OBJECT:
	    emsg(_(e_using_object_as_float));
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_VOID:
	    emsg(_(e_cannot_use_void_ਮੁੱਲ));
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_UNKNOWN:
	ਹਾਲਤ VAR_ANY:
	ਹਾਲਤ VAR_INSTR:
	    ਅੰਦਰੂਨੀ_error_no_abort("tv_get_float(UNKNOWN)");
	    ਰੋਕੋ;
    }
    ਜੇ (error != ਨਿਰਾ)
	*error = TRUE;
    ਵਾਪਸ 0;
}

    float_T
tv_get_float(typval_T *varp)
{
    ਵਾਪਸ tv_get_float_chk(varp, ਨਿਰਾ);
}

/*
 * Give an error and return FAIL unless "args[idx]" is unknown
 */
    ਪੂਰਨ
check_for_unknown_arg(typval_T *ਦਲੀਲਾਂ, ਪੂਰਨ idx)
{
    ਜੇ (ਦਲੀਲਾਂ[idx].v_type != VAR_UNKNOWN)
    {
	semsg(_(e_too_many_arguments), idx + 1);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string.
 */
    ਪੂਰਨ
check_for_string_arg(typval_T *ਦਲੀਲਾਂ, ਪੂਰਨ idx)
{
    ਜੇ (ਦਲੀਲਾਂ[idx].v_type != VAR_STRING)
    {
	semsg(_(e_string_required_for_ਦਲੀਲ_nr), idx + 1);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a non-empty string.
 */
    ਪੂਰਨ
check_for_nonempty_string_arg(typval_T *ਦਲੀਲਾਂ, ਪੂਰਨ idx)
{
    ਜੇ (check_for_string_arg(ਦਲੀਲਾਂ, idx) == FAIL)
	ਵਾਪਸ FAIL;
    ਜੇ (ਦਲੀਲਾਂ[idx].vval.v_string == ਨਿਰਾ || *ਦਲੀਲਾਂ[idx].vval.v_string == NUL)
    {
	semsg(_(e_non_ਖਾਲੀ_ਥਾਵਾਂ_string_required_for_ਦਲੀਲ_nr), idx + 1);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Check for an optional string argument at 'idx'
 */
    ਪੂਰਨ
check_for_opt_string_arg(typval_T *ਦਲੀਲਾਂ, ਪੂਰਨ idx)
{
    ਵਾਪਸ (ਦਲੀਲਾਂ[idx].v_type == VAR_UNKNOWN
	    || check_for_string_arg(ਦਲੀਲਾਂ, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a number.
 */
    ਪੂਰਨ
check_for_ਸੰਖਿਆ_arg(typval_T *ਦਲੀਲਾਂ, ਪੂਰਨ idx)
{
    ਜੇ (ਦਲੀਲਾਂ[idx].v_type != VAR_ਸੰਖਿਆ)
    {
	semsg(_(e_ਸੰਖਿਆ_required_for_ਦਲੀਲ_nr), idx + 1);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Check for an optional number argument at 'idx'
 */
    ਪੂਰਨ
check_for_opt_ਸੰਖਿਆ_arg(typval_T *ਦਲੀਲਾਂ, ਪੂਰਨ idx)
{
    ਵਾਪਸ (ਦਲੀਲਾਂ[idx].v_type == VAR_UNKNOWN
	    || check_for_ਸੰਖਿਆ_arg(ਦਲੀਲਾਂ, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a float or a number.
 */
    ਪੂਰਨ
check_for_float_or_nr_arg(typval_T *ਦਲੀਲਾਂ, ਪੂਰਨ idx)
{
    ਜੇ (ਦਲੀਲਾਂ[idx].v_type != VAR_FLOAT && ਦਲੀਲਾਂ[idx].v_type != VAR_ਸੰਖਿਆ)
    {
	semsg(_(e_float_or_ਸੰਖਿਆ_required_for_ਦਲੀਲ_nr), idx + 1);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a bool.
 */
    ਪੂਰਨ
check_for_bool_arg(typval_T *ਦਲੀਲਾਂ, ਪੂਰਨ idx)
{
    ਜੇ (ਦਲੀਲਾਂ[idx].v_type != VAR_BOOL
	    && !(ਦਲੀਲਾਂ[idx].v_type == VAR_ਸੰਖਿਆ
		&& (ਦਲੀਲਾਂ[idx].vval.v_ਸੰਖਿਆ == 0
		    || ਦਲੀਲਾਂ[idx].vval.v_ਸੰਖਿਆ == 1)))
    {
	semsg(_(e_bool_required_for_ਦਲੀਲ_nr), idx + 1);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a bool or a number.
 */
    ਸਥਿਰ ਪੂਰਨ
check_for_bool_or_ਸੰਖਿਆ_arg(typval_T *ਦਲੀਲਾਂ, ਪੂਰਨ idx)
{
    ਜੇ (ਦਲੀਲਾਂ[idx].v_type != VAR_BOOL && ਦਲੀਲਾਂ[idx].v_type != VAR_ਸੰਖਿਆ)
    {
	semsg(_(e_bool_or_ਸੰਖਿਆ_required_for_ਦਲੀਲ_nr), idx + 1);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Check for an optional bool argument at 'idx'.
 * Return FAIL if the type is wrong.
 */
    ਪੂਰਨ
check_for_opt_bool_arg(typval_T *ਦਲੀਲਾਂ, ਪੂਰਨ idx)
{
    ਜੇ (ਦਲੀਲਾਂ[idx].v_type == VAR_UNKNOWN)
	ਵਾਪਸ OK;
    ਵਾਪਸ check_for_bool_arg(ਦਲੀਲਾਂ, idx);
}

/*
 * Check for an optional bool or number argument at 'idx'.
 * Return FAIL if the type is wrong.
 */
    ਪੂਰਨ
check_for_opt_bool_or_ਸੰਖਿਆ_arg(typval_T *ਦਲੀਲਾਂ, ਪੂਰਨ idx)
{
    ਜੇ (ਦਲੀਲਾਂ[idx].v_type == VAR_UNKNOWN)
	ਵਾਪਸ OK;
    ਵਾਪਸ check_for_bool_or_ਸੰਖਿਆ_arg(ਦਲੀਲਾਂ, idx);
}

/*
 * Check for an optional bool or dict argument at 'idx'.
 * Return FAIL if the type is wrong.
 */
    ਪੂਰਨ
check_for_opt_bool_or_dict_arg(typval_T *ਦਲੀਲਾਂ, ਪੂਰਨ idx)
{
    ਜੇ (ਦਲੀਲਾਂ[idx].v_type == VAR_UNKNOWN || ਦਲੀਲਾਂ[idx].v_type == VAR_DICT)
	ਵਾਪਸ OK;
    ਵਾਪਸ check_for_bool_arg(ਦਲੀਲਾਂ, idx);
}

/*
 * Give an error and return FAIL unless "args[idx]" is a blob.
 */
    ਪੂਰਨ
check_for_blob_arg(typval_T *ਦਲੀਲਾਂ, ਪੂਰਨ idx)
{
    ਜੇ (ਦਲੀਲਾਂ[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_blob_required_for_ਦਲੀਲ_nr), idx + 1);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list.
 */
    ਪੂਰਨ
check_for_list_arg(typval_T *ਦਲੀਲਾਂ, ਪੂਰਨ idx)
{
    ਜੇ (ਦਲੀਲਾਂ[idx].v_type != VAR_LIST)
    {
	semsg(_(e_list_required_for_ਦਲੀਲ_nr), idx + 1);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a non-NULL list.
 */
    ਪੂਰਨ
check_for_nonnull_list_arg(typval_T *ਦਲੀਲਾਂ, ਪੂਰਨ idx)
{
    ਜੇ (check_for_list_arg(ਦਲੀਲਾਂ, idx) == FAIL)
	ਵਾਪਸ FAIL;

    ਜੇ (ਦਲੀਲਾਂ[idx].vval.v_list == ਨਿਰਾ)
    {
	semsg(_(e_non_null_list_required_for_ਦਲੀਲ_nr), idx + 1);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Check for an optional list argument at 'idx'
 */
    ਪੂਰਨ
check_for_opt_list_arg(typval_T *ਦਲੀਲਾਂ, ਪੂਰਨ idx)
{
    ਵਾਪਸ (ਦਲੀਲਾਂ[idx].v_type == VAR_UNKNOWN
	    || check_for_list_arg(ਦਲੀਲਾਂ, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a tuple.
 */
    ਪੂਰਨ
check_for_tuple_arg(typval_T *ਦਲੀਲਾਂ, ਪੂਰਨ idx)
{
    ਜੇ (ਦਲੀਲਾਂ[idx].v_type != VAR_TUPLE)
    {
	semsg(_(e_tuple_required_for_ਦਲੀਲ_nr), idx + 1);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a dict.
 */
    ਪੂਰਨ
check_for_dict_arg(typval_T *ਦਲੀਲਾਂ, ਪੂਰਨ idx)
{
    ਜੇ (ਦਲੀਲਾਂ[idx].v_type != VAR_DICT)
    {
	semsg(_(e_dict_required_for_ਦਲੀਲ_nr), idx + 1);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a non-NULL dict.
 */
    ਪੂਰਨ
check_for_nonnull_dict_arg(typval_T *ਦਲੀਲਾਂ, ਪੂਰਨ idx)
{
    ਜੇ (check_for_dict_arg(ਦਲੀਲਾਂ, idx) == FAIL)
	ਵਾਪਸ FAIL;

    ਜੇ (ਦਲੀਲਾਂ[idx].vval.v_dict == ਨਿਰਾ)
    {
	semsg(_(e_non_null_dict_required_for_ਦਲੀਲ_nr), idx + 1);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Check for an optional dict argument at 'idx'
 */
    ਪੂਰਨ
check_for_opt_dict_arg(typval_T *ਦਲੀਲਾਂ, ਪੂਰਨ idx)
{
    ਵਾਪਸ (ਦਲੀਲਾਂ[idx].v_type == VAR_UNKNOWN
	    || check_for_dict_arg(ਦਲੀਲਾਂ, idx) != FAIL) ? OK : FAIL;
}

/*
 * Check for an optional non-NULL dict argument at 'idx'
 */
    ਪੂਰਨ
check_for_opt_nonnull_dict_arg(typval_T *ਦਲੀਲਾਂ, ਪੂਰਨ idx)
{
    ਵਾਪਸ (ਦਲੀਲਾਂ[idx].v_type == VAR_UNKNOWN
	    || check_for_nonnull_dict_arg(ਦਲੀਲਾਂ, idx) != FAIL) ? OK : FAIL;
}

#ਜੇ defined(FEAT_ਕੰਮ_CHANNEL)
/*
 * Give an error and return FAIL unless "args[idx]" is a channel or a job.
 */
    ਪੂਰਨ
check_for_chan_or_ਕੰਮ_arg(typval_T *ਦਲੀਲਾਂ, ਪੂਰਨ idx)
{
    ਜੇ (ਦਲੀਲਾਂ[idx].v_type != VAR_CHANNEL && ਦਲੀਲਾਂ[idx].v_type != VAR_ਕੰਮ)
    {
	semsg(_(e_chan_or_ਕੰਮ_required_for_ਦਲੀਲ_nr), idx + 1);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an optional channel or a
 * job.
 */
    ਪੂਰਨ
check_for_opt_chan_or_ਕੰਮ_arg(typval_T *ਦਲੀਲਾਂ, ਪੂਰਨ idx)
{
    ਵਾਪਸ (ਦਲੀਲਾਂ[idx].v_type == VAR_UNKNOWN
	    || check_for_chan_or_ਕੰਮ_arg(ਦਲੀਲਾਂ, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a job.
 */
    ਪੂਰਨ
check_for_ਕੰਮ_arg(typval_T *ਦਲੀਲਾਂ, ਪੂਰਨ idx)
{
    ਜੇ (ਦਲੀਲਾਂ[idx].v_type != VAR_ਕੰਮ)
    {
	semsg(_(e_ਕੰਮ_required_for_ਦਲੀਲ_nr), idx + 1);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Check for an optional job argument at 'idx'.
 */
    ਪੂਰਨ
check_for_opt_ਕੰਮ_arg(typval_T *ਦਲੀਲਾਂ, ਪੂਰਨ idx)
{
    ਵਾਪਸ (ਦਲੀਲਾਂ[idx].v_type == VAR_UNKNOWN
	    || check_for_ਕੰਮ_arg(ਦਲੀਲਾਂ, idx) != FAIL) ? OK : FAIL;
}
#ਨਹੀਂਤਾਂ
/*
 * Give an error and return FAIL unless "args[idx]" is an optional channel or a
 * job.  Used without the +channel feature, thus only VAR_UNKNOWN is accepted.
 */
    ਪੂਰਨ
check_for_opt_chan_or_ਕੰਮ_arg(typval_T *ਦਲੀਲਾਂ, ਪੂਰਨ idx)
{
    ਵਾਪਸ ਦਲੀਲਾਂ[idx].v_type == VAR_UNKNOWN ? OK : FAIL;
}
#ਜੇ_ਅੰਤ

/*
 * Give an error and return FAIL unless "args[idx]" is a string or
 * a number.
 */
    ਪੂਰਨ
check_for_string_or_ਸੰਖਿਆ_arg(typval_T *ਦਲੀਲਾਂ, ਪੂਰਨ idx)
{
    ਜੇ (ਦਲੀਲਾਂ[idx].v_type != VAR_STRING && ਦਲੀਲਾਂ[idx].v_type != VAR_ਸੰਖਿਆ)
    {
	semsg(_(e_string_or_ਸੰਖਿਆ_required_for_ਦਲੀਲ_nr), idx + 1);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Check for an optional string or number argument at 'idx'.
 */
    ਪੂਰਨ
check_for_opt_string_or_ਸੰਖਿਆ_arg(typval_T *ਦਲੀਲਾਂ, ਪੂਰਨ idx)
{
    ਵਾਪਸ (ਦਲੀਲਾਂ[idx].v_type == VAR_UNKNOWN
	    || check_for_string_or_ਸੰਖਿਆ_arg(ਦਲੀਲਾਂ, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a buffer number.
 * Buffer number can be a number or a string.
 */
    ਪੂਰਨ
check_for_ਬਫਰ_arg(typval_T *ਦਲੀਲਾਂ, ਪੂਰਨ idx)
{
    ਵਾਪਸ check_for_string_or_ਸੰਖਿਆ_arg(ਦਲੀਲਾਂ, idx);
}

/*
 * Check for an optional buffer argument at 'idx'
 */
    ਪੂਰਨ
check_for_opt_ਬਫਰ_arg(typval_T *ਦਲੀਲਾਂ, ਪੂਰਨ idx)
{
    ਵਾਪਸ (ਦਲੀਲਾਂ[idx].v_type == VAR_UNKNOWN
	    || check_for_ਬਫਰ_arg(ਦਲੀਲਾਂ, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a line number.
 * Line number can be a number or a string.
 */
    ਪੂਰਨ
check_for_lnum_arg(typval_T *ਦਲੀਲਾਂ, ਪੂਰਨ idx)
{
    ਵਾਪਸ check_for_string_or_ਸੰਖਿਆ_arg(ਦਲੀਲਾਂ, idx);
}

/*
 * Check for an optional line number argument at 'idx'
 */
    ਪੂਰਨ
check_for_opt_lnum_arg(typval_T *ਦਲੀਲਾਂ, ਪੂਰਨ idx)
{
    ਵਾਪਸ (ਦਲੀਲਾਂ[idx].v_type == VAR_UNKNOWN
	    || check_for_lnum_arg(ਦਲੀਲਾਂ, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a blob.
 */
    ਪੂਰਨ
check_for_string_or_blob_arg(typval_T *ਦਲੀਲਾਂ, ਪੂਰਨ idx)
{
    ਜੇ (ਦਲੀਲਾਂ[idx].v_type != VAR_STRING && ਦਲੀਲਾਂ[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_string_or_blob_required_for_ਦਲੀਲ_nr), idx + 1);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a list.
 */
    ਪੂਰਨ
check_for_string_or_list_arg(typval_T *ਦਲੀਲਾਂ, ਪੂਰਨ idx)
{
    ਜੇ (ਦਲੀਲਾਂ[idx].v_type != VAR_STRING && ਦਲੀਲਾਂ[idx].v_type != VAR_LIST)
    {
	semsg(_(e_string_or_list_required_for_ਦਲੀਲ_nr), idx + 1);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string, a list, a
 * tuple or a blob.
 */
    ਪੂਰਨ
check_for_string_or_list_or_tuple_or_blob_arg(typval_T *ਦਲੀਲਾਂ, ਪੂਰਨ idx)
{
    ਜੇ (ਦਲੀਲਾਂ[idx].v_type != VAR_STRING
	    && ਦਲੀਲਾਂ[idx].v_type != VAR_LIST
	    && ਦਲੀਲਾਂ[idx].v_type != VAR_TUPLE
	    && ਦਲੀਲਾਂ[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_string_list_tuple_or_blob_required_for_ਦਲੀਲ_nr), idx + 1);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Check for an optional string or list argument at 'idx'
 */
    ਪੂਰਨ
check_for_opt_string_or_list_arg(typval_T *ਦਲੀਲਾਂ, ਪੂਰਨ idx)
{
    ਵਾਪਸ (ਦਲੀਲਾਂ[idx].v_type == VAR_UNKNOWN
	    || check_for_string_or_list_arg(ਦਲੀਲਾਂ, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a dict.
 */
    ਪੂਰਨ
check_for_string_or_dict_arg(typval_T *ਦਲੀਲਾਂ, ਪੂਰਨ idx)
{
    ਜੇ (ਦਲੀਲਾਂ[idx].v_type != VAR_STRING && ਦਲੀਲਾਂ[idx].v_type != VAR_DICT)
    {
	semsg(_(e_string_or_dict_required_for_ਦਲੀਲ_nr), idx + 1);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a number
 * or a list.
 */
    ਪੂਰਨ
check_for_string_or_ਸੰਖਿਆ_or_list_arg(typval_T *ਦਲੀਲਾਂ, ਪੂਰਨ idx)
{
    ਜੇ (ਦਲੀਲਾਂ[idx].v_type != VAR_STRING
	    && ਦਲੀਲਾਂ[idx].v_type != VAR_ਸੰਖਿਆ
	    && ਦਲੀਲਾਂ[idx].v_type != VAR_LIST)
    {
	semsg(_(e_string_ਸੰਖਿਆ_or_list_required_for_ਦਲੀਲ_nr), idx + 1);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an optional string
 * or number or a list
 */
    ਪੂਰਨ
check_for_opt_string_or_ਸੰਖਿਆ_or_list_arg(typval_T *ਦਲੀਲਾਂ, ਪੂਰਨ idx)
{
    ਵਾਪਸ (ਦਲੀਲਾਂ[idx].v_type == VAR_UNKNOWN
	    || check_for_string_or_ਸੰਖਿਆ_or_list_arg(ਦਲੀਲਾਂ, idx)
							!= FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string, a number, a
 * list, a tuple or a blob.
 */
    ਪੂਰਨ
check_for_repeat_func_arg(typval_T *ਦਲੀਲਾਂ, ਪੂਰਨ idx)
{
    ਜੇ (ਦਲੀਲਾਂ[idx].v_type != VAR_STRING
	    && ਦਲੀਲਾਂ[idx].v_type != VAR_ਸੰਖਿਆ
	    && ਦਲੀਲਾਂ[idx].v_type != VAR_LIST
	    && ਦਲੀਲਾਂ[idx].v_type != VAR_TUPLE
	    && ਦਲੀਲਾਂ[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_repeatable_type_required_for_ਦਲੀਲ_nr), idx + 1);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string, a list, a
 * tuple or a dict.
 */
    ਪੂਰਨ
check_for_string_list_tuple_or_dict_arg(typval_T *ਦਲੀਲਾਂ, ਪੂਰਨ idx)
{
    ਜੇ (ਦਲੀਲਾਂ[idx].v_type != VAR_STRING
	    && ਦਲੀਲਾਂ[idx].v_type != VAR_LIST
	    && ਦਲੀਲਾਂ[idx].v_type != VAR_TUPLE
	    && ਦਲੀਲਾਂ[idx].v_type != VAR_DICT)
    {
	semsg(_(e_string_list_tuple_or_dict_required_for_ਦਲੀਲ_nr), idx + 1);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string
 * or a function reference.
 */
    ਪੂਰਨ
check_for_string_or_func_arg(typval_T *ਦਲੀਲਾਂ, ਪੂਰਨ idx)
{
    ਜੇ (ਦਲੀਲਾਂ[idx].v_type != VAR_PARTIAL
	    && ਦਲੀਲਾਂ[idx].v_type != VAR_FUNC
	    && ਦਲੀਲਾਂ[idx].v_type != VAR_STRING)
    {
	semsg(_(e_string_or_function_required_for_ਦਲੀਲ_nr), idx + 1);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list or a blob.
 */
    ਪੂਰਨ
check_for_list_or_blob_arg(typval_T *ਦਲੀਲਾਂ, ਪੂਰਨ idx)
{
    ਜੇ (ਦਲੀਲਾਂ[idx].v_type != VAR_LIST && ਦਲੀਲਾਂ[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_list_or_blob_required_for_ਦਲੀਲ_nr), idx + 1);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list or a tuple.
 */
    ਪੂਰਨ
check_for_list_or_tuple_arg(typval_T *ਦਲੀਲਾਂ, ਪੂਰਨ idx)
{
    ਜੇ (ਦਲੀਲਾਂ[idx].v_type != VAR_LIST && ਦਲੀਲਾਂ[idx].v_type != VAR_TUPLE)
    {
	semsg(_(e_list_or_tuple_required_for_ਦਲੀਲ_nr), idx + 1);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list, a tuple or a
 * blob.
 */
    ਪੂਰਨ
check_for_list_or_tuple_or_blob_arg(typval_T *ਦਲੀਲਾਂ, ਪੂਰਨ idx)
{
    ਜੇ (ਦਲੀਲਾਂ[idx].v_type != VAR_LIST
	    && ਦਲੀਲਾਂ[idx].v_type != VAR_TUPLE
	    && ਦਲੀਲਾਂ[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_list_or_tuple_or_blob_required_for_ਦਲੀਲ_nr), idx + 1);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list, a tuple or a
 * dict
 */
    ਪੂਰਨ
check_for_list_or_tuple_or_dict_arg(typval_T *ਦਲੀਲਾਂ, ਪੂਰਨ idx)
{
    ਜੇ (ਦਲੀਲਾਂ[idx].v_type != VAR_LIST
	    && ਦਲੀਲਾਂ[idx].v_type != VAR_TUPLE
	    && ਦਲੀਲਾਂ[idx].v_type != VAR_DICT)
    {
	semsg(_(e_list_or_tuple_or_dict_required_for_ਦਲੀਲ_nr), idx + 1);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list or dict or a
 * blob.
 */
    ਪੂਰਨ
check_for_list_or_dict_or_blob_arg(typval_T *ਦਲੀਲਾਂ, ਪੂਰਨ idx)
{
    ਜੇ (ਦਲੀਲਾਂ[idx].v_type != VAR_LIST
	    && ਦਲੀਲਾਂ[idx].v_type != VAR_DICT
	    && ਦਲੀਲਾਂ[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_list_dict_or_blob_required_for_ਦਲੀਲ_nr), idx + 1);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list, a tuple, a dict,
 * a blob or a string.
 */
    ਪੂਰਨ
check_for_list_tuple_dict_blob_or_string_arg(typval_T *ਦਲੀਲਾਂ, ਪੂਰਨ idx)
{
    ਜੇ (ਦਲੀਲਾਂ[idx].v_type != VAR_LIST
	    && ਦਲੀਲਾਂ[idx].v_type != VAR_TUPLE
	    && ਦਲੀਲਾਂ[idx].v_type != VAR_DICT
	    && ਦਲੀਲਾਂ[idx].v_type != VAR_BLOB
	    && ਦਲੀਲਾਂ[idx].v_type != VAR_STRING)
    {
	semsg(_(e_list_tuple_dict_blob_or_string_required_for_ਦਲੀਲ_nr), idx + 1);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an optional buffer
 * number or a dict.
 */
    ਪੂਰਨ
check_for_opt_ਬਫਰ_or_dict_arg(typval_T *ਦਲੀਲਾਂ, ਪੂਰਨ idx)
{
    ਜੇ (ਦਲੀਲਾਂ[idx].v_type != VAR_UNKNOWN
	    && ਦਲੀਲਾਂ[idx].v_type != VAR_STRING
	    && ਦਲੀਲਾਂ[idx].v_type != VAR_ਸੰਖਿਆ
	    && ਦਲੀਲਾਂ[idx].v_type != VAR_DICT)
    {
	semsg(_(e_string_required_for_ਦਲੀਲ_nr), idx + 1);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an object.
 */
    ਪੂਰਨ
check_for_object_arg(typval_T *ਦਲੀਲਾਂ, ਪੂਰਨ idx)
{
    ਜੇ (ਦਲੀਲਾਂ[idx].v_type != VAR_OBJECT)
    {
	semsg(_(e_object_required_for_ਦਲੀਲ_nr), idx + 1);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Returns TRUE if "tv" is a type alias for a class
 */
    ਸਥਿਰ ਪੂਰਨ
tv_class_alias(typval_T *tv)
{
    ਵਾਪਸ tv->v_type == VAR_TYPEALIAS &&
			tv->vval.v_typealias->ta_type->tt_type == VAR_OBJECT;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a class
 * or class typealias.
 */
    ਪੂਰਨ
check_for_class_or_typealias_ਦਲੀਲਾਂ(typval_T *ਦਲੀਲਾਂ, ਪੂਰਨ idx)
{
    ਲਈ (ਪੂਰਨ i = idx; ਦਲੀਲਾਂ[i].v_type != VAR_UNKNOWN; ++i)
    {
	ਜੇ (ਦਲੀਲਾਂ[i].v_type != VAR_CLASS && !tv_class_alias(&ਦਲੀਲਾਂ[idx]))
	{
	    semsg(_(e_class_or_typealias_required_for_ਦਲੀਲ_nr), i + 1);
	    ਵਾਪਸ FAIL;
	}
    }
    ਵਾਪਸ OK;
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
    ਸਥਿਰ char_u   mybuf[NUMBUFLEN];

    ਵਾਪਸ tv_get_string_buf(varp, mybuf);
}

/*
 * Like tv_get_string() but don't allow number to string conversion for Vim9.
 */
    char_u *
tv_get_string_strict(typval_T *varp)
{
    ਸਥਿਰ char_u   mybuf[NUMBUFLEN];
    char_u	    *res =  tv_get_string_buf_chk_strict(
						 varp, mybuf, ਵਿੱਚ_vim9script());

    ਵਾਪਸ res != ਨਿਰਾ ? res : (char_u *)"";
}

    char_u *
tv_get_string_buf(typval_T *varp, char_u *buf)
{
    char_u	*res = tv_get_string_buf_chk(varp, buf);

    ਵਾਪਸ res != ਨਿਰਾ ? res : (char_u *)"";
}

/*
 * Careful: This uses a single, static buffer.  YOU CAN ONLY USE IT ONCE!
 */
    char_u *
tv_get_string_chk(typval_T *varp)
{
    ਸਥਿਰ char_u   mybuf[NUMBUFLEN];

    ਵਾਪਸ tv_get_string_buf_chk(varp, mybuf);
}

    char_u *
tv_get_string_buf_chk(typval_T *varp, char_u *buf)
{
    ਵਾਪਸ tv_get_string_buf_chk_strict(varp, buf, FALSE);
}

    char_u *
tv_get_string_buf_chk_strict(typval_T *varp, char_u *buf, ਪੂਰਨ strict)
{
    ਚੋਣ (varp->v_type)
    {
	ਹਾਲਤ VAR_ਸੰਖਿਆ:
	    ਜੇ (strict)
	    {
		emsg(_(e_using_ਸੰਖਿਆ_as_string));
		ਰੋਕੋ;
	    }
	    vim_snprintf((ਅੱਖਰ *)buf, NUMBUFLEN, "%lld",
					    (varnumber_T)varp->vval.v_ਸੰਖਿਆ);
	    ਵਾਪਸ buf;
	ਹਾਲਤ VAR_FUNC:
	ਹਾਲਤ VAR_PARTIAL:
	    emsg(_(e_using_funcref_as_string));
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_LIST:
	    emsg(_(e_using_list_as_string));
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_TUPLE:
	    emsg(_(e_using_tuple_as_string));
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_DICT:
	    emsg(_(e_using_dictionary_as_string));
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_FLOAT:
	    ਜੇ (strict)
	    {
		emsg(_(e_using_float_as_string));
		ਰੋਕੋ;
	    }
	    vim_snprintf((ਅੱਖਰ *)buf, NUMBUFLEN, "%g", varp->vval.v_float);
	    ਵਾਪਸ buf;
	ਹਾਲਤ VAR_STRING:
	    ਜੇ (varp->vval.v_string != ਨਿਰਾ)
		ਵਾਪਸ varp->vval.v_string;
	    ਵਾਪਸ (char_u *)"";
	ਹਾਲਤ VAR_BOOL:
	ਹਾਲਤ VAR_SPECIAL:
	    STRCPY(buf, get_var_special_name(varp->vval.v_ਸੰਖਿਆ));
	    ਵਾਪਸ buf;
	ਹਾਲਤ VAR_BLOB:
	    emsg(_(e_using_blob_as_string));
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_CLASS:
	ਹਾਲਤ VAR_TYPEALIAS:
	    check_typval_is_ਮੁੱਲ(varp);
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_OBJECT:
	    {
		ਜੇ (varp->vval.v_object == ਨਿਰਾ)
		    emsg(_(e_using_object_as_string));
		ਨਹੀਂਤਾਂ
		{
		    class_T *cl = varp->vval.v_object->obj_class;
		    ਜੇ (cl != ਨਿਰਾ && IS_ENUM(cl))
			semsg(_(e_using_enum_str_as_string), cl->class_name.string);
		    ਨਹੀਂਤਾਂ
			emsg(_(e_using_object_as_string));
		}
	    }
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_ਕੰਮ:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
	    ਜੇ (ਵਿੱਚ_vim9script())
	    {
		semsg(_(e_using_invalid_ਮੁੱਲ_as_string_str), "job");
		ਰੋਕੋ;
	    }
	    ਵਾਪਸ ਕੰਮ_ਮੰਜ਼ਿਲ_string_buf(varp, buf);
#ਜੇ_ਅੰਤ
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_CHANNEL:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
	    ਜੇ (ਵਿੱਚ_vim9script())
	    {
		semsg(_(e_using_invalid_ਮੁੱਲ_as_string_str), "channel");
		ਰੋਕੋ;
	    }
	    ਵਾਪਸ channel_ਮੰਜ਼ਿਲ_string_buf(varp, buf);
#ਜੇ_ਅੰਤ
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_VOID:
	    emsg(_(e_cannot_use_void_ਮੁੱਲ));
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_UNKNOWN:
	ਹਾਲਤ VAR_ANY:
	ਹਾਲਤ VAR_INSTR:
	    semsg(_(e_using_invalid_ਮੁੱਲ_as_string_str),
						  vartype_name(varp->v_type));
	    ਰੋਕੋ;
    }
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Turn a typeval into a string.  Similar to tv_get_string_buf() but uses
 * string() on Dict, List, etc.
 */
    char_u *
tv_stringify(typval_T *varp, char_u *buf)
{
    ਜੇ (varp->v_type == VAR_LIST
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
	ਵਾਪਸ tmp.vval.v_string;
    }
    ਵਾਪਸ tv_get_string_buf(varp, buf);
}

/*
 * Return TRUE if typeval "tv" and its value are set to be locked (immutable).
 * Also give an error message, using "name" or _("name") when use_gettext is
 * TRUE.
 */
    ਪੂਰਨ
tv_check_ਤਾਲਾ(typval_T *tv, char_u *name, ਪੂਰਨ use_gettext)
{
    ਪੂਰਨ	ਤਾਲਾ = 0;

    ਚੋਣ (tv->v_type)
    {
	ਹਾਲਤ VAR_BLOB:
	    ਜੇ (tv->vval.v_blob != ਨਿਰਾ)
		ਤਾਲਾ = tv->vval.v_blob->bv_ਤਾਲਾ;
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_LIST:
	    ਜੇ (tv->vval.v_list != ਨਿਰਾ)
		ਤਾਲਾ = tv->vval.v_list->lv_ਤਾਲਾ;
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_TUPLE:
	    ਜੇ (tv->vval.v_tuple != ਨਿਰਾ)
		ਤਾਲਾ = tv->vval.v_tuple->tv_ਤਾਲਾ;
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_DICT:
	    ਜੇ (tv->vval.v_dict != ਨਿਰਾ)
		ਤਾਲਾ = tv->vval.v_dict->dv_ਤਾਲਾ;
	    ਰੋਕੋ;
	ਮੂਲ:
	    ਰੋਕੋ;
    }
    ਵਾਪਸ ਮੁੱਲ_check_ਤਾਲਾ(tv->v_ਤਾਲਾ, name, use_gettext)
		   || (ਤਾਲਾ != 0 && ਮੁੱਲ_check_ਤਾਲਾ(ਤਾਲਾ, name, use_gettext));
}

/*
 * Copy the values from typval_T "from" to typval_T "to".
 * When needed allocates string or increases reference count.
 * Does not make a copy of a list, blob or dict but copies the reference!
 * It is OK for "from" and "to" to point to the same item.  This is used to
 * make a copy later.
 */
    ਪੂਰਨ
copy_tv(typval_T *from, typval_T *ਮੰਜ਼ਿਲ)
{
    ਪੂਰਨ		ret = OK;

    ਮੰਜ਼ਿਲ->v_type = from->v_type;
    ਮੰਜ਼ਿਲ->v_ਤਾਲਾ = 0;
    ਚੋਣ (from->v_type)
    {
	ਹਾਲਤ VAR_ਸੰਖਿਆ:
	ਹਾਲਤ VAR_BOOL:
	ਹਾਲਤ VAR_SPECIAL:
	    ਮੰਜ਼ਿਲ->vval.v_ਸੰਖਿਆ = from->vval.v_ਸੰਖਿਆ;
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_FLOAT:
	    ਮੰਜ਼ਿਲ->vval.v_float = from->vval.v_float;
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_ਕੰਮ:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
	    ਮੰਜ਼ਿਲ->vval.v_ਕੰਮ = from->vval.v_ਕੰਮ;
	    ਜੇ (ਮੰਜ਼ਿਲ->vval.v_ਕੰਮ != ਨਿਰਾ)
		++ਮੰਜ਼ਿਲ->vval.v_ਕੰਮ->jv_refcount;
	    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
	ਹਾਲਤ VAR_CHANNEL:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
	    ਮੰਜ਼ਿਲ->vval.v_channel = from->vval.v_channel;
	    ਜੇ (ਮੰਜ਼ਿਲ->vval.v_channel != ਨਿਰਾ)
		++ਮੰਜ਼ਿਲ->vval.v_channel->ch_refcount;
	    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
	ਹਾਲਤ VAR_INSTR:
	    ਮੰਜ਼ਿਲ->vval.v_instr = from->vval.v_instr;
	    ਰੋਕੋ;

	ਹਾਲਤ VAR_CLASS:
	    copy_class(from, ਮੰਜ਼ਿਲ);
	    ਰੋਕੋ;

	ਹਾਲਤ VAR_OBJECT:
	    copy_object(from, ਮੰਜ਼ਿਲ);
	    ਰੋਕੋ;

	ਹਾਲਤ VAR_STRING:
	ਹਾਲਤ VAR_FUNC:
	    ਜੇ (from->vval.v_string == ਨਿਰਾ)
		ਮੰਜ਼ਿਲ->vval.v_string = ਨਿਰਾ;
	    ਨਹੀਂਤਾਂ
	    {
		ਮੰਜ਼ਿਲ->vval.v_string = vim_strsave(from->vval.v_string);
		ਜੇ (from->v_type == VAR_FUNC)
		    func_ref(ਮੰਜ਼ਿਲ->vval.v_string);
	    }
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_PARTIAL:
	    ਜੇ (from->vval.v_partial == ਨਿਰਾ)
		ਮੰਜ਼ਿਲ->vval.v_partial = ਨਿਰਾ;
	    ਨਹੀਂਤਾਂ
	    {
		ਮੰਜ਼ਿਲ->vval.v_partial = from->vval.v_partial;
		++ਮੰਜ਼ਿਲ->vval.v_partial->pt_refcount;
	    }
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_BLOB:
	    ਜੇ (from->vval.v_blob == ਨਿਰਾ)
		ਮੰਜ਼ਿਲ->vval.v_blob = ਨਿਰਾ;
	    ਨਹੀਂਤਾਂ
	    {
		ਮੰਜ਼ਿਲ->vval.v_blob = from->vval.v_blob;
		++ਮੰਜ਼ਿਲ->vval.v_blob->bv_refcount;
	    }
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_LIST:
	    ਜੇ (from->vval.v_list == ਨਿਰਾ)
		ਮੰਜ਼ਿਲ->vval.v_list = ਨਿਰਾ;
	    ਨਹੀਂਤਾਂ
	    {
		ਮੰਜ਼ਿਲ->vval.v_list = from->vval.v_list;
		++ਮੰਜ਼ਿਲ->vval.v_list->lv_refcount;
	    }
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_TUPLE:
	    ਜੇ (from->vval.v_tuple == ਨਿਰਾ)
		ਮੰਜ਼ਿਲ->vval.v_tuple = ਨਿਰਾ;
	    ਨਹੀਂਤਾਂ
	    {
		ਮੰਜ਼ਿਲ->vval.v_tuple = from->vval.v_tuple;
		++ਮੰਜ਼ਿਲ->vval.v_tuple->tv_refcount;
	    }
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_DICT:
	    ਜੇ (from->vval.v_dict == ਨਿਰਾ)
		ਮੰਜ਼ਿਲ->vval.v_dict = ਨਿਰਾ;
	    ਨਹੀਂਤਾਂ
	    {
		ਮੰਜ਼ਿਲ->vval.v_dict = from->vval.v_dict;
		++ਮੰਜ਼ਿਲ->vval.v_dict->dv_refcount;
	    }
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_TYPEALIAS:
	    ਜੇ (from->vval.v_typealias == ਨਿਰਾ)
		ਮੰਜ਼ਿਲ->vval.v_typealias = ਨਿਰਾ;
	    ਨਹੀਂਤਾਂ
	    {
		ਮੰਜ਼ਿਲ->vval.v_typealias = from->vval.v_typealias;
		++ਮੰਜ਼ਿਲ->vval.v_typealias->ta_refcount;
	    }
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_VOID:
	    emsg(_(e_cannot_use_void_ਮੁੱਲ));
	    ret = FAIL;
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_UNKNOWN:
	ਹਾਲਤ VAR_ANY:
	    ਅੰਦਰੂਨੀ_error_no_abort("copy_tv(UNKNOWN)");
	    ret = FAIL;
	    ਰੋਕੋ;
    }

    ਵਾਪਸ ret;
}

/*
 * Compare "tv1" and "tv2".
 */
    ਪੂਰਨ
typval_compare2(
    typval_T	*tv1,	// first operand
    typval_T	*tv2,	// second operand
    exprtype_T	type,	// operator
    ਪੂਰਨ		ic,	// ignore case
    ਪੂਰਨ		*res)	// comparison result
{
    varnumber_T	n1, n2;
    ਪੂਰਨ		type_is = type == ਵਿਅੰਜਕ_IS || type == ਵਿਅੰਜਕ_ISNOT;

    ਜੇ (check_typval_is_ਮੁੱਲ(tv1) == FAIL
	|| check_typval_is_ਮੁੱਲ(tv2) == FAIL)
	ਵਾਪਸ FAIL;
    ਨਹੀਂਤਾਂ ਜੇ (type_is && tv1->v_type != tv2->v_type)
    {
	// For "is" a different type always means FALSE, for "isnot"
	// it means TRUE.
	*res = (type == ਵਿਅੰਜਕ_ISNOT);
    }
    ਨਹੀਂਤਾਂ ਜੇ (((tv1->v_type == VAR_SPECIAL && tv1->vval.v_ਸੰਖਿਆ == VVAL_NULL)
		|| (tv2->v_type == VAR_SPECIAL
					   && tv2->vval.v_ਸੰਖਿਆ == VVAL_NULL))
	    && tv1->v_type != tv2->v_type
	    && (type == ਵਿਅੰਜਕ_EQUAL || type == ਵਿਅੰਜਕ_NEQUAL))
    {
	n1 = typval_compare_null(tv1, tv2);
	ਜੇ (n1 == MAYBE)
	    ਵਾਪਸ FAIL;
	ਜੇ (type == ਵਿਅੰਜਕ_NEQUAL)
	    n1 = !n1;
	*res = n1;
    }
    ਨਹੀਂਤਾਂ ਜੇ (tv1->v_type == VAR_BLOB || tv2->v_type == VAR_BLOB)
    {
	ਜੇ (typval_compare_blob(tv1, tv2, type, res) == FAIL)
	    ਵਾਪਸ FAIL;
    }
    ਨਹੀਂਤਾਂ ਜੇ (tv1->v_type == VAR_LIST || tv2->v_type == VAR_LIST)
    {
	ਜੇ (typval_compare_list(tv1, tv2, type, ic, res) == FAIL)
	    ਵਾਪਸ FAIL;
    }
    ਨਹੀਂਤਾਂ ਜੇ (tv1->v_type == VAR_TUPLE || tv2->v_type == VAR_TUPLE)
    {
	ਜੇ (typval_compare_tuple(tv1, tv2, type, ic, res) == FAIL)
	    ਵਾਪਸ FAIL;
    }
    ਨਹੀਂਤਾਂ ਜੇ (tv1->v_type == VAR_OBJECT || tv2->v_type == VAR_OBJECT)
    {
	ਜੇ (typval_compare_object(tv1, tv2, type, ic, res) == FAIL)
	    ਵਾਪਸ FAIL;
    }
    ਨਹੀਂਤਾਂ ਜੇ (tv1->v_type == VAR_DICT || tv2->v_type == VAR_DICT)
    {
	ਜੇ (typval_compare_dict(tv1, tv2, type, ic, res) == FAIL)
	    ਵਾਪਸ FAIL;
    }
    ਨਹੀਂਤਾਂ ਜੇ (tv1->v_type == VAR_FUNC || tv2->v_type == VAR_FUNC
	|| tv1->v_type == VAR_PARTIAL || tv2->v_type == VAR_PARTIAL)
    {
	ਜੇ (typval_compare_func(tv1, tv2, type, ic, res) == FAIL)
	    ਵਾਪਸ FAIL;
    }

    // If one of the two variables is a float, compare as a float.
    // When using "=~" or "!~", always compare as string.
    ਨਹੀਂਤਾਂ ਜੇ ((tv1->v_type == VAR_FLOAT || tv2->v_type == VAR_FLOAT)
	    && type != ਵਿਅੰਜਕ_ਮੇਲ && type != ਵਿਅੰਜਕ_NOMATCH)
    {
	float_T f1, f2;
	ਪੂਰਨ	error = FALSE;

	f1 = tv_get_float_chk(tv1, &error);
	ਜੇ (!error)
	    f2 = tv_get_float_chk(tv2, &error);
	ਜੇ (error)
	    ਵਾਪਸ FAIL;
	n1 = FALSE;
	ਚੋਣ (type)
	{
	    ਹਾਲਤ ਵਿਅੰਜਕ_IS:
	    ਹਾਲਤ ਵਿਅੰਜਕ_EQUAL:    n1 = (f1 == f2); ਰੋਕੋ;
	    ਹਾਲਤ ਵਿਅੰਜਕ_ISNOT:
	    ਹਾਲਤ ਵਿਅੰਜਕ_NEQUAL:   n1 = (f1 != f2); ਰੋਕੋ;
	    ਹਾਲਤ ਵਿਅੰਜਕ_GREATER:  n1 = (f1 > f2); ਰੋਕੋ;
	    ਹਾਲਤ ਵਿਅੰਜਕ_GEQUAL:   n1 = (f1 >= f2); ਰੋਕੋ;
	    ਹਾਲਤ ਵਿਅੰਜਕ_SMALLER:  n1 = (f1 < f2); ਰੋਕੋ;
	    ਹਾਲਤ ਵਿਅੰਜਕ_SEQUAL:   n1 = (f1 <= f2); ਰੋਕੋ;
	    ਹਾਲਤ ਵਿਅੰਜਕ_UNKNOWN:
	    ਹਾਲਤ ਵਿਅੰਜਕ_ਮੇਲ:
	    ਮੂਲ:  ਰੋਕੋ;  // avoid gcc warning
	}
	*res = n1;
    }

    // If one of the two variables is a number, compare as a number.
    // When using "=~" or "!~", always compare as string.
    ਨਹੀਂਤਾਂ ਜੇ ((tv1->v_type == VAR_ਸੰਖਿਆ || tv2->v_type == VAR_ਸੰਖਿਆ)
	    && type != ਵਿਅੰਜਕ_ਮੇਲ && type != ਵਿਅੰਜਕ_NOMATCH)
    {
	ਪੂਰਨ error = FALSE;

	n1 = tv_get_ਸੰਖਿਆ_chk(tv1, &error);
	ਜੇ (!error)
	    n2 = tv_get_ਸੰਖਿਆ_chk(tv2, &error);
	ਜੇ (error)
	    ਵਾਪਸ FAIL;
	ਚੋਣ (type)
	{
	    ਹਾਲਤ ਵਿਅੰਜਕ_IS:
	    ਹਾਲਤ ਵਿਅੰਜਕ_EQUAL:    n1 = (n1 == n2); ਰੋਕੋ;
	    ਹਾਲਤ ਵਿਅੰਜਕ_ISNOT:
	    ਹਾਲਤ ਵਿਅੰਜਕ_NEQUAL:   n1 = (n1 != n2); ਰੋਕੋ;
	    ਹਾਲਤ ਵਿਅੰਜਕ_GREATER:  n1 = (n1 > n2); ਰੋਕੋ;
	    ਹਾਲਤ ਵਿਅੰਜਕ_GEQUAL:   n1 = (n1 >= n2); ਰੋਕੋ;
	    ਹਾਲਤ ਵਿਅੰਜਕ_SMALLER:  n1 = (n1 < n2); ਰੋਕੋ;
	    ਹਾਲਤ ਵਿਅੰਜਕ_SEQUAL:   n1 = (n1 <= n2); ਰੋਕੋ;
	    ਹਾਲਤ ਵਿਅੰਜਕ_UNKNOWN:
	    ਹਾਲਤ ਵਿਅੰਜਕ_ਮੇਲ:
	    ਮੂਲ:  ਰੋਕੋ;  // avoid gcc warning
	}
	*res = n1;
    }
    ਨਹੀਂਤਾਂ ਜੇ (ਵਿੱਚ_vim9script() && (tv1->v_type == VAR_BOOL
				    || tv2->v_type == VAR_BOOL
				    || (tv1->v_type == VAR_SPECIAL
					      && tv2->v_type == VAR_SPECIAL)))
    {
	ਜੇ (tv1->v_type != tv2->v_type)
	{
	    semsg(_(e_cannot_compare_str_with_str),
		       vartype_name(tv1->v_type), vartype_name(tv2->v_type));
	    ਵਾਪਸ FAIL;
	}
	n1 = tv1->vval.v_ਸੰਖਿਆ;
	n2 = tv2->vval.v_ਸੰਖਿਆ;
	ਚੋਣ (type)
	{
	    ਹਾਲਤ ਵਿਅੰਜਕ_IS:
	    ਹਾਲਤ ਵਿਅੰਜਕ_EQUAL:    n1 = (n1 == n2); ਰੋਕੋ;
	    ਹਾਲਤ ਵਿਅੰਜਕ_ISNOT:
	    ਹਾਲਤ ਵਿਅੰਜਕ_NEQUAL:   n1 = (n1 != n2); ਰੋਕੋ;
	    ਮੂਲ:
		semsg(_(e_invalid_operation_for_str),
						   vartype_name(tv1->v_type));
		ਵਾਪਸ FAIL;
	}
	*res = n1;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
    ਨਹੀਂਤਾਂ ਜੇ (tv1->v_type == tv2->v_type
	    && (tv1->v_type == VAR_CHANNEL || tv1->v_type == VAR_ਕੰਮ)
	    && (type == ਵਿਅੰਜਕ_NEQUAL || type == ਵਿਅੰਜਕ_EQUAL))
    {
	ਜੇ (tv1->v_type == VAR_CHANNEL)
	    n1 = tv1->vval.v_channel == tv2->vval.v_channel;
	ਨਹੀਂਤਾਂ
	    n1 = tv1->vval.v_ਕੰਮ == tv2->vval.v_ਕੰਮ;
	ਜੇ (type == ਵਿਅੰਜਕ_NEQUAL)
	    n1 = !n1;
	*res = n1;
    }
#ਜੇ_ਅੰਤ
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (typval_compare_string(tv1, tv2, type, ic, res) == FAIL)
	    ਵਾਪਸ FAIL;
    }

    ਵਾਪਸ OK;
}

/*
 * Compare "tv1" and "tv2".
 * Put the result in "tv1".  Caller should clear "tv2".
 */
    ਪੂਰਨ
typval_compare(
    typval_T	*tv1,	// first operand
    typval_T	*tv2,	// second operand
    exprtype_T	type,   // operator
    ਪੂਰਨ		ic)     // ignore case
{
    ਪੂਰਨ res;

    ਜੇ (typval_compare2(tv1, tv2, type, ic, &res) == FAIL)
    {
	clear_tv(tv1);
	ਵਾਪਸ FAIL;
    }

    clear_tv(tv1);
    ਜੇ (ਵਿੱਚ_vim9script())
    {
	tv1->v_type = VAR_BOOL;
	tv1->vval.v_ਸੰਖਿਆ = res ? VVAL_TRUE : VVAL_FALSE;
    }
    ਨਹੀਂਤਾਂ
    {
	tv1->v_type = VAR_ਸੰਖਿਆ;
	tv1->vval.v_ਸੰਖਿਆ = res;
    }

    ਵਾਪਸ OK;
}

/*
 * Compare "tv1" to "tv2" as lists according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    ਪੂਰਨ
typval_compare_list(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	ਪੂਰਨ	    ic,
	ਪੂਰਨ	    *res)
{
    ਪੂਰਨ	    val = 0;

    ਜੇ (type == ਵਿਅੰਜਕ_IS || type == ਵਿਅੰਜਕ_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
				      && tv1->vval.v_list == tv2->vval.v_list);
	ਜੇ (type == ਵਿਅੰਜਕ_ISNOT)
	    val = !val;
    }
    ਨਹੀਂਤਾਂ ਜੇ (tv1->v_type != tv2->v_type
	    || (type != ਵਿਅੰਜਕ_EQUAL && type != ਵਿਅੰਜਕ_NEQUAL))
    {
	ਜੇ (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_list_with_list));
	ਨਹੀਂਤਾਂ
	    emsg(_(e_invalid_operation_for_list));
	ਵਾਪਸ FAIL;
    }
    ਨਹੀਂਤਾਂ
    {
	val = list_equal(tv1->vval.v_list, tv2->vval.v_list, ic);
	ਜੇ (type == ਵਿਅੰਜਕ_NEQUAL)
	    val = !val;
    }
    *res = val;
    ਵਾਪਸ OK;
}

/*
 * Compare "tv1" to "tv2" as tuples according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    ਪੂਰਨ
typval_compare_tuple(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	ਪੂਰਨ	    ic,
	ਪੂਰਨ	    *res)
{
    ਪੂਰਨ	    val = 0;

    ਜੇ (type == ਵਿਅੰਜਕ_IS || type == ਵਿਅੰਜਕ_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
			      && tv1->vval.v_tuple == tv2->vval.v_tuple);
	ਜੇ (type == ਵਿਅੰਜਕ_ISNOT)
	    val = !val;
    }
    ਨਹੀਂਤਾਂ ਜੇ (tv1->v_type != tv2->v_type
	    || (type != ਵਿਅੰਜਕ_EQUAL && type != ਵਿਅੰਜਕ_NEQUAL))
    {
	ਜੇ (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_tuple_with_tuple));
	ਨਹੀਂਤਾਂ
	    emsg(_(e_invalid_operation_for_tuple));
	ਵਾਪਸ FAIL;
    }
    ਨਹੀਂਤਾਂ
    {
	val = tuple_equal(tv1->vval.v_tuple, tv2->vval.v_tuple, ic);
	ਜੇ (type == ਵਿਅੰਜਕ_NEQUAL)
	    val = !val;
    }
    *res = val;
    ਵਾਪਸ OK;
}

/*
 * Compare v:null with another type.  Return TRUE if the value is NULL.
 */
    ਪੂਰਨ
typval_compare_null(typval_T *tv1, typval_T *tv2)
{
    ਜੇ ((tv1->v_type == VAR_SPECIAL && tv1->vval.v_ਸੰਖਿਆ == VVAL_NULL)
	    || (tv2->v_type == VAR_SPECIAL && tv2->vval.v_ਸੰਖਿਆ == VVAL_NULL))
    {
	typval_T	*tv = tv1->v_type == VAR_SPECIAL ? tv2 : tv1;

	ਚੋਣ (tv->v_type)
	{
	    ਹਾਲਤ VAR_BLOB: ਵਾਪਸ tv->vval.v_blob == ਨਿਰਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
	    ਹਾਲਤ VAR_CHANNEL: ਵਾਪਸ tv->vval.v_channel == ਨਿਰਾ;
#ਜੇ_ਅੰਤ
	    // TODO: null_class handling
	    // case VAR_CLASS: return tv->vval.v_class == NULL;
	    ਹਾਲਤ VAR_DICT: ਵਾਪਸ tv->vval.v_dict == ਨਿਰਾ;
	    ਹਾਲਤ VAR_FUNC: ਵਾਪਸ tv->vval.v_string == ਨਿਰਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
	    ਹਾਲਤ VAR_ਕੰਮ: ਵਾਪਸ tv->vval.v_ਕੰਮ == ਨਿਰਾ;
#ਜੇ_ਅੰਤ
	    ਹਾਲਤ VAR_LIST: ਵਾਪਸ tv->vval.v_list == ਨਿਰਾ;
	    ਹਾਲਤ VAR_TUPLE: ਵਾਪਸ tv->vval.v_tuple == ਨਿਰਾ;
	    ਹਾਲਤ VAR_OBJECT: ਵਾਪਸ tv->vval.v_object == ਨਿਰਾ;
	    ਹਾਲਤ VAR_PARTIAL: ਵਾਪਸ tv->vval.v_partial == ਨਿਰਾ;
	    ਹਾਲਤ VAR_STRING: ਵਾਪਸ tv->vval.v_string == ਨਿਰਾ;

	    ਹਾਲਤ VAR_ਸੰਖਿਆ: ਜੇ (!ਵਿੱਚ_vim9script())
				 ਵਾਪਸ tv->vval.v_ਸੰਖਿਆ == 0;
			     ਰੋਕੋ;
	    ਹਾਲਤ VAR_FLOAT: ਜੇ (!ਵਿੱਚ_vim9script())
				 ਵਾਪਸ tv->vval.v_float == 0.0;
			     ਰੋਕੋ;
	    ਹਾਲਤ VAR_TYPEALIAS: ਵਾਪਸ tv->vval.v_typealias == ਨਿਰਾ;
	    ਮੂਲ: ਰੋਕੋ;
	}
    }
    // although comparing null with number, float or bool is not very useful
    // we won't give an error
    ਵਾਪਸ FALSE;
}

/*
 * Compare "tv1" to "tv2" as blobs according to "type".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    ਪੂਰਨ
typval_compare_blob(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	ਪੂਰਨ	    *res)
{
    ਪੂਰਨ	    val = 0;

    ਜੇ (type == ਵਿਅੰਜਕ_IS || type == ਵਿਅੰਜਕ_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
			&& tv1->vval.v_blob == tv2->vval.v_blob);
	ਜੇ (type == ਵਿਅੰਜਕ_ISNOT)
	    val = !val;
    }
    ਨਹੀਂਤਾਂ ਜੇ (tv1->v_type != tv2->v_type
	    || (type != ਵਿਅੰਜਕ_EQUAL && type != ਵਿਅੰਜਕ_NEQUAL))
    {
	ਜੇ (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_blob_with_blob));
	ਨਹੀਂਤਾਂ
	    emsg(_(e_invalid_operation_for_blob));
	ਵਾਪਸ FAIL;
    }
    ਨਹੀਂਤਾਂ
    {
	val = blob_equal(tv1->vval.v_blob, tv2->vval.v_blob);
	ਜੇ (type == ਵਿਅੰਜਕ_NEQUAL)
	    val = !val;
    }
    *res = val;
    ਵਾਪਸ OK;
}

/*
 * Compare "tv1" to "tv2" as objects according to "type".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    ਪੂਰਨ
typval_compare_object(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	ਪੂਰਨ	    ic,
	ਪੂਰਨ	    *res)
{
    ਪੂਰਨ res_ਮੇਲ = type == ਵਿਅੰਜਕ_EQUAL || type == ਵਿਅੰਜਕ_IS ? TRUE : FALSE;

    ਜੇ (tv1->vval.v_object == ਨਿਰਾ && tv2->vval.v_object == ਨਿਰਾ)
    {
	*res = res_ਮੇਲ;
	ਵਾਪਸ OK;
    }
    ਜੇ (tv1->vval.v_object == ਨਿਰਾ || tv2->vval.v_object == ਨਿਰਾ)
    {
	*res = !res_ਮੇਲ;
	ਵਾਪਸ OK;
    }

    object_T *obj1 = tv1->vval.v_object;
    object_T *obj2 = tv2->vval.v_object;
    ਜੇ (type == ਵਿਅੰਜਕ_IS || type == ਵਿਅੰਜਕ_ISNOT)
    {
	*res = obj1 == obj2 ? res_ਮੇਲ : !res_ਮੇਲ;
	ਵਾਪਸ OK;
    }
    ਨਹੀਂਤਾਂ ਜੇ (tv1->v_type != tv2->v_type)
    {
	emsg(_(e_can_only_compare_object_with_object));
	ਵਾਪਸ FAIL;
    }

    *res = object_equal(obj1, obj2, ic) ? res_ਮੇਲ : !res_ਮੇਲ;
    ਵਾਪਸ OK;
}

/*
 * Compare "tv1" to "tv2" as dictionaries according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    ਪੂਰਨ
typval_compare_dict(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	ਪੂਰਨ	    ic,
	ਪੂਰਨ	    *res)
{
    ਪੂਰਨ	    val;

    ਜੇ (type == ਵਿਅੰਜਕ_IS || type == ਵਿਅੰਜਕ_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
			&& tv1->vval.v_dict == tv2->vval.v_dict);
	ਜੇ (type == ਵਿਅੰਜਕ_ISNOT)
	    val = !val;
    }
    ਨਹੀਂਤਾਂ ਜੇ (tv1->v_type != tv2->v_type
		|| (type != ਵਿਅੰਜਕ_EQUAL && type != ਵਿਅੰਜਕ_NEQUAL))
    {
	ਜੇ (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_dictionary_with_dictionary));
	ਨਹੀਂਤਾਂ
	    emsg(_(e_invalid_operation_for_dictionary));
	ਵਾਪਸ FAIL;
    }
    ਨਹੀਂਤਾਂ
    {
	val = dict_equal(tv1->vval.v_dict, tv2->vval.v_dict, ic);
	ਜੇ (type == ਵਿਅੰਜਕ_NEQUAL)
	    val = !val;
    }
    *res = val;
    ਵਾਪਸ OK;
}

/*
 * Compare "tv1" to "tv2" as funcrefs according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    ਪੂਰਨ
typval_compare_func(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	ਪੂਰਨ	    ic,
	ਪੂਰਨ	    *res)
{
    ਪੂਰਨ	    val = 0;

    ਜੇ (type != ਵਿਅੰਜਕ_EQUAL && type != ਵਿਅੰਜਕ_NEQUAL
	    && type != ਵਿਅੰਜਕ_IS && type != ਵਿਅੰਜਕ_ISNOT)
    {
	emsg(_(e_invalid_operation_for_funcrefs));
	ਵਾਪਸ FAIL;
    }
    ਜੇ ((tv1->v_type == VAR_PARTIAL && tv1->vval.v_partial == ਨਿਰਾ)
	    || (tv2->v_type == VAR_PARTIAL && tv2->vval.v_partial == ਨਿਰਾ))
	// When both partials are NULL, then they are equal.
	// Otherwise they are not equal.
	val = (tv1->vval.v_partial == tv2->vval.v_partial);
    ਨਹੀਂਤਾਂ ਜੇ (type == ਵਿਅੰਜਕ_IS || type == ਵਿਅੰਜਕ_ISNOT)
    {
	ਜੇ (tv1->v_type == VAR_FUNC && tv2->v_type == VAR_FUNC)
	    // strings are considered the same if their value is
	    // the same
	    val = tv_equal(tv1, tv2, ic);
	ਨਹੀਂਤਾਂ ਜੇ (tv1->v_type == VAR_PARTIAL && tv2->v_type == VAR_PARTIAL)
	    val = (tv1->vval.v_partial == tv2->vval.v_partial);
	ਨਹੀਂਤਾਂ
	    val = FALSE;
    }
    ਨਹੀਂਤਾਂ
	val = tv_equal(tv1, tv2, ic);
    ਜੇ (type == ਵਿਅੰਜਕ_NEQUAL || type == ਵਿਅੰਜਕ_ISNOT)
	val = !val;
    *res = val;
    ਵਾਪਸ OK;
}

/*
 * Compare "tv1" to "tv2" as strings according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    ਪੂਰਨ
typval_compare_string(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	ਪੂਰਨ	    ic,
	ਪੂਰਨ	    *res)
{
    ਪੂਰਨ		i = 0;
    ਪੂਰਨ		val = FALSE;
    char_u	*s1, *s2;
    char_u	buf1[NUMBUFLEN], buf2[NUMBUFLEN];

    ਜੇ (ਵਿੱਚ_vim9script()
	  && ((tv1->v_type != VAR_STRING && tv1->v_type != VAR_SPECIAL)
	   || (tv2->v_type != VAR_STRING && tv2->v_type != VAR_SPECIAL)))
    {
	semsg(_(e_cannot_compare_str_with_str),
		   vartype_name(tv1->v_type), vartype_name(tv2->v_type));
	ਵਾਪਸ FAIL;
    }
    s1 = tv_get_string_buf(tv1, buf1);
    s2 = tv_get_string_buf(tv2, buf2);
    ਜੇ (type != ਵਿਅੰਜਕ_ਮੇਲ && type != ਵਿਅੰਜਕ_NOMATCH)
	i = ic ? MB_STRICMP(s1, s2) : STRCMP(s1, s2);
    ਚੋਣ (type)
    {
	ਹਾਲਤ ਵਿਅੰਜਕ_IS:	    ਜੇ (ਵਿੱਚ_vim9script())
			    {
				// Really check it is the same string, not just
				// the same value.
				val = tv1->vval.v_string == tv2->vval.v_string;
				ਰੋਕੋ;
			    }
			    // FALLTHROUGH
	ਹਾਲਤ ਵਿਅੰਜਕ_EQUAL:    val = (i == 0); ਰੋਕੋ;
	ਹਾਲਤ ਵਿਅੰਜਕ_ISNOT:    ਜੇ (ਵਿੱਚ_vim9script())
			    {
				// Really check it is not the same string, not
				// just a different value.
				val = tv1->vval.v_string != tv2->vval.v_string;
				ਰੋਕੋ;
			    }
			    // FALLTHROUGH
	ਹਾਲਤ ਵਿਅੰਜਕ_NEQUAL:   val = (i != 0); ਰੋਕੋ;
	ਹਾਲਤ ਵਿਅੰਜਕ_GREATER:  val = (i > 0); ਰੋਕੋ;
	ਹਾਲਤ ਵਿਅੰਜਕ_GEQUAL:   val = (i >= 0); ਰੋਕੋ;
	ਹਾਲਤ ਵਿਅੰਜਕ_SMALLER:  val = (i < 0); ਰੋਕੋ;
	ਹਾਲਤ ਵਿਅੰਜਕ_SEQUAL:   val = (i <= 0); ਰੋਕੋ;

	ਹਾਲਤ ਵਿਅੰਜਕ_ਮੇਲ:
	ਹਾਲਤ ਵਿਅੰਜਕ_NOMATCH:
		val = ਨਮੂਨਾ_ਮੇਲ(s2, s1, ic);
		ਜੇ (type == ਵਿਅੰਜਕ_NOMATCH)
		    val = !val;
		ਰੋਕੋ;

	ਮੂਲ:  ਰੋਕੋ;  // avoid gcc warning
    }
    *res = val;
    ਵਾਪਸ OK;
}
/*
 * Convert any type to a string, never give an error.
 * When "quotes" is TRUE add quotes to a string.
 * Returns an allocated string.
 */
    char_u *
typval_tostring(typval_T *arg, ਪੂਰਨ quotes)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    char_u	*ret = ਨਿਰਾ;

    ਜੇ (arg == ਨਿਰਾ)
	ਵਾਪਸ vim_strsave((char_u *)"(does not exist)");
    ਜੇ (!quotes && arg->v_type == VAR_STRING)
    {
	ret = vim_strsave(arg->vval.v_string == ਨਿਰਾ ? (char_u *)""
							 : arg->vval.v_string);
    }
    ਨਹੀਂਤਾਂ
    {
	ret = tv2string(arg, &tofree, numbuf, 0);
	// Make a copy if we have a value but it's not in allocated memory.
	ਜੇ (ret != ਨਿਰਾ && tofree == ਨਿਰਾ)
	    ret = vim_strsave(ret);
    }
    ਵਾਪਸ ret;
}

/*
 * Return TRUE if typeval "tv" is locked: Either that value is locked itself
 * or it refers to a List or Dictionary that is locked.
 */
    ਪੂਰਨ
tv_islocked(typval_T *tv)
{
    ਵਾਪਸ (tv->v_ਤਾਲਾ & VAR_LOCKED)
	|| (tv->v_type == VAR_LIST
		&& tv->vval.v_list != ਨਿਰਾ
		&& (tv->vval.v_list->lv_ਤਾਲਾ & VAR_LOCKED))
	|| (tv->v_type == VAR_TUPLE
		&& tv->vval.v_tuple != ਨਿਰਾ
		&& (tv->vval.v_tuple->tv_ਤਾਲਾ & VAR_LOCKED))
	|| (tv->v_type == VAR_DICT
		&& tv->vval.v_dict != ਨਿਰਾ
		&& (tv->vval.v_dict->dv_ਤਾਲਾ & VAR_LOCKED));
}

    ਸਥਿਰ ਪੂਰਨ
func_equal(
    typval_T *tv1,
    typval_T *tv2,
    ਪੂਰਨ	     ic)	    // ignore case
{
    char_u	*s1, *s2;
    dict_T	*d1, *d2;
    ਪੂਰਨ		a1, a2;
    ਪੂਰਨ		i;

    // empty and NULL function name considered the same
    s1 = tv1->v_type == VAR_FUNC ? tv1->vval.v_string
					   : partial_name(tv1->vval.v_partial);
    ਜੇ (s1 != ਨਿਰਾ && *s1 == NUL)
	s1 = ਨਿਰਾ;
    s2 = tv2->v_type == VAR_FUNC ? tv2->vval.v_string
					   : partial_name(tv2->vval.v_partial);
    ਜੇ (s2 != ਨਿਰਾ && *s2 == NUL)
	s2 = ਨਿਰਾ;
    ਜੇ (s1 == ਨਿਰਾ || s2 == ਨਿਰਾ)
    {
	ਜੇ (s1 != s2)
	    ਵਾਪਸ FALSE;
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(s1, s2) != 0)
	ਵਾਪਸ FALSE;

    // empty dict and NULL dict is different
    d1 = tv1->v_type == VAR_FUNC ? ਨਿਰਾ : tv1->vval.v_partial->pt_dict;
    d2 = tv2->v_type == VAR_FUNC ? ਨਿਰਾ : tv2->vval.v_partial->pt_dict;
    ਜੇ (d1 == ਨਿਰਾ || d2 == ਨਿਰਾ)
    {
	ਜੇ (d1 != d2)
	    ਵਾਪਸ FALSE;
    }
    ਨਹੀਂਤਾਂ ਜੇ (!dict_equal(d1, d2, ic))
	ਵਾਪਸ FALSE;

    // empty list and no list considered the same
    a1 = tv1->v_type == VAR_FUNC ? 0 : tv1->vval.v_partial->pt_argc;
    a2 = tv2->v_type == VAR_FUNC ? 0 : tv2->vval.v_partial->pt_argc;
    ਜੇ (a1 != a2)
	ਵਾਪਸ FALSE;
    ਲਈ (i = 0; i < a1; ++i)
	ਜੇ (!tv_equal(tv1->vval.v_partial->pt_argv + i,
		      tv2->vval.v_partial->pt_argv + i, ic))
	    ਵਾਪਸ FALSE;

    ਵਾਪਸ TRUE;
}

/*
 * Return TRUE if "tv1" and "tv2" have the same value.
 * Compares the items just like "==" would compare them, but strings and
 * numbers are different.  Floats and numbers are also different.
 */
    ਪੂਰਨ
tv_equal(
    typval_T *tv1,
    typval_T *tv2,
    ਪੂਰਨ	     ic)	    // ignore case
{
    char_u	buf1[NUMBUFLEN], buf2[NUMBUFLEN];
    char_u	*s1, *s2;
    ਸਥਿਰ ਪੂਰਨ  recursive_cnt = 0;	    // catch recursive loops
    ਪੂਰਨ		r;
    ਸਥਿਰ ਪੂਰਨ	tv_equal_recurse_ਹੱਦਬੰਦੀ;

    // Catch lists and dicts that have an endless loop by limiting
    // recursiveness to a limit.  We guess they are equal then.
    // A fixed limit has the problem of still taking an awful long time.
    // Reduce the limit every time running into it. That should work fine for
    // deeply linked structures that are not recursively linked and catch
    // recursiveness quickly.
    ਜੇ (recursive_cnt == 0)
	tv_equal_recurse_ਹੱਦਬੰਦੀ = 1000;
    ਜੇ (recursive_cnt >= tv_equal_recurse_ਹੱਦਬੰਦੀ)
    {
	--tv_equal_recurse_ਹੱਦਬੰਦੀ;
	ਵਾਪਸ TRUE;
    }

    // For VAR_FUNC and VAR_PARTIAL compare the function name, bound dict and
    // arguments.
    ਜੇ ((tv1->v_type == VAR_FUNC
		|| (tv1->v_type == VAR_PARTIAL && tv1->vval.v_partial != ਨਿਰਾ))
	    && (tv2->v_type == VAR_FUNC
		|| (tv2->v_type == VAR_PARTIAL && tv2->vval.v_partial != ਨਿਰਾ)))
    {
	++recursive_cnt;
	r = func_equal(tv1, tv2, ic);
	--recursive_cnt;
	ਵਾਪਸ r;
    }

    ਜੇ (tv1->v_type != tv2->v_type
	    && ((tv1->v_type != VAR_BOOL && tv1->v_type != VAR_SPECIAL)
		|| (tv2->v_type != VAR_BOOL && tv2->v_type != VAR_SPECIAL)))
	ਵਾਪਸ FALSE;

    ਚੋਣ (tv1->v_type)
    {
	ਹਾਲਤ VAR_LIST:
	    ++recursive_cnt;
	    r = list_equal(tv1->vval.v_list, tv2->vval.v_list, ic);
	    --recursive_cnt;
	    ਵਾਪਸ r;

	ਹਾਲਤ VAR_TUPLE:
	    ++recursive_cnt;
	    r = tuple_equal(tv1->vval.v_tuple, tv2->vval.v_tuple, ic);
	    --recursive_cnt;
	    ਵਾਪਸ r;

	ਹਾਲਤ VAR_DICT:
	    ++recursive_cnt;
	    r = dict_equal(tv1->vval.v_dict, tv2->vval.v_dict, ic);
	    --recursive_cnt;
	    ਵਾਪਸ r;

	ਹਾਲਤ VAR_BLOB:
	    ਵਾਪਸ blob_equal(tv1->vval.v_blob, tv2->vval.v_blob);

	ਹਾਲਤ VAR_ਸੰਖਿਆ:
	ਹਾਲਤ VAR_BOOL:
	ਹਾਲਤ VAR_SPECIAL:
	    ਵਾਪਸ tv1->vval.v_ਸੰਖਿਆ == tv2->vval.v_ਸੰਖਿਆ;

	ਹਾਲਤ VAR_STRING:
	    s1 = tv_get_string_buf(tv1, buf1);
	    s2 = tv_get_string_buf(tv2, buf2);
	    ਵਾਪਸ ((ic ? MB_STRICMP(s1, s2) : STRCMP(s1, s2)) == 0);

	ਹਾਲਤ VAR_FLOAT:
	    ਵਾਪਸ tv1->vval.v_float == tv2->vval.v_float;
	ਹਾਲਤ VAR_ਕੰਮ:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
	    ਵਾਪਸ tv1->vval.v_ਕੰਮ == tv2->vval.v_ਕੰਮ;
#ਜੇ_ਅੰਤ
	ਹਾਲਤ VAR_CHANNEL:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
	    ਵਾਪਸ tv1->vval.v_channel == tv2->vval.v_channel;
#ਜੇ_ਅੰਤ
	ਹਾਲਤ VAR_INSTR:
	    ਵਾਪਸ tv1->vval.v_instr == tv2->vval.v_instr;

	ਹਾਲਤ VAR_CLASS:
	    // A class only exists once, equality is identity.
	    ਵਾਪਸ tv1->vval.v_class == tv2->vval.v_class;

	ਹਾਲਤ VAR_OBJECT:
	    ++recursive_cnt;
	    r = object_equal(tv1->vval.v_object, tv2->vval.v_object, ic);
	    --recursive_cnt;
	    ਵਾਪਸ r;

	ਹਾਲਤ VAR_PARTIAL:
	    ਵਾਪਸ tv1->vval.v_partial == tv2->vval.v_partial;

	ਹਾਲਤ VAR_FUNC:
	    ਵਾਪਸ tv1->vval.v_string == tv2->vval.v_string;

	ਹਾਲਤ VAR_TYPEALIAS:
	    ਵਾਪਸ tv1->vval.v_typealias == tv2->vval.v_typealias;

	ਹਾਲਤ VAR_UNKNOWN:
	ਹਾਲਤ VAR_ANY:
	ਹਾਲਤ VAR_VOID:
	    ਰੋਕੋ;
    }

    // VAR_UNKNOWN can be the result of a invalid expression, let's say it
    // does not equal anything, not even itself.
    ਵਾਪਸ FALSE;
}

/*
 * Get an option value.
 * "arg" points to the '&' or '+' before the option name.
 * "arg" is advanced to character after the option name.
 * Return OK or FAIL.
 */
    ਪੂਰਨ
eval_option(
    char_u	**arg,
    typval_T	*rettv,	// when NULL, only check if option exists
    ਪੂਰਨ		evaluate)
{
    char_u	*option_ਸਿਰਾ;
    ਲੰਮਾ	numval;
    char_u	*stringval;
    getoption_T	opt_type;
    ਪੂਰਨ		c;
    ਪੂਰਨ		working = (**arg == '+');    // has("+option")
    ਪੂਰਨ		ret = OK;
    ਪੂਰਨ		scope;

    // Isolate the option name and find its value.
    option_ਸਿਰਾ = ਲੱਭੋ_option_ਸਿਰਾ(arg, &scope);
    ਜੇ (option_ਸਿਰਾ == ਨਿਰਾ)
    {
	ਜੇ (rettv != ਨਿਰਾ)
	    semsg(_(e_option_name_missing_str), *arg);
	ਵਾਪਸ FAIL;
    }

    ਜੇ (!evaluate)
    {
	*arg = option_ਸਿਰਾ;
	ਵਾਪਸ OK;
    }

    c = *option_ਸਿਰਾ;
    *option_ਸਿਰਾ = NUL;
    opt_type = get_option_ਮੁੱਲ(*arg, &numval,
			       rettv == ਨਿਰਾ ? ਨਿਰਾ : &stringval, ਨਿਰਾ, scope);

    ਜੇ (opt_type == gov_unknown)
    {
	ਜੇ (rettv != ਨਿਰਾ)
	    semsg(_(e_unknown_option_str), *arg);
	ret = FAIL;
    }
    ਨਹੀਂਤਾਂ ਜੇ (rettv != ਨਿਰਾ)
    {
	rettv->v_ਤਾਲਾ = 0;
	ਜੇ (opt_type == gov_hidden_string)
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = ਨਿਰਾ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (opt_type == gov_hidden_bool || opt_type == gov_hidden_ਸੰਖਿਆ)
	{
	    rettv->v_type = ਵਿੱਚ_vim9script() && opt_type == gov_hidden_bool
						       ? VAR_BOOL : VAR_ਸੰਖਿਆ;
	    rettv->vval.v_ਸੰਖਿਆ = 0;
	}
	ਨਹੀਂਤਾਂ ਜੇ (opt_type == gov_bool || opt_type == gov_ਸੰਖਿਆ)
	{
	    ਜੇ (ਵਿੱਚ_vim9script() && opt_type == gov_bool)
	    {
		rettv->v_type = VAR_BOOL;
		rettv->vval.v_ਸੰਖਿਆ = numval ? VVAL_TRUE : VVAL_FALSE;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		rettv->v_type = VAR_ਸੰਖਿਆ;
		rettv->vval.v_ਸੰਖਿਆ = numval;
	    }
	}
	ਨਹੀਂਤਾਂ				// string option
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = stringval;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (working && (opt_type == gov_hidden_bool
			|| opt_type == gov_hidden_ਸੰਖਿਆ
			|| opt_type == gov_hidden_string))
	ret = FAIL;

    *option_ਸਿਰਾ = c;		    // put back for error messages
    *arg = option_ਸਿਰਾ;

    ਵਾਪਸ ret;
}

/*
 * Allocate a variable for a number constant.  Also deals with "0z" for blob.
 * Return OK or FAIL.
 */
    ਪੂਰਨ
eval_ਸੰਖਿਆ(
	char_u	    **arg,
	typval_T    *rettv,
	ਪੂਰਨ	    evaluate,
	ਪੂਰਨ	    want_string)
{
    ਪੂਰਨ		len;
    ਪੂਰਨ		skip_quotes = !ਵਿੱਚ_old_script(4);
    char_u	*p;
    ਪੂਰਨ		get_float = FALSE;

    // We accept a float when the format matches
    // "[0-9]\+\.[0-9]\+\([eE][+-]\?[0-9]\+\)\?".  This is very
    // strict to avoid backwards compatibility problems.
    // With script version 2 and later the leading digit can be
    // omitted.
    // Don't look for a float after the "." operator, so that
    // ":let vers = 1.2.3" doesn't fail.
    ਜੇ (**arg == '.')
	p = *arg;
    ਨਹੀਂਤਾਂ
    {
	p = *arg + 1;
	ਜੇ (skip_quotes)
	    ਲਈ (;;)
	    {
		ਜੇ (*p == '\'')
		    ++p;
		ਜੇ (!vim_isdigit(*p))
		    ਰੋਕੋ;
		p = skipdigits(p);
	    }
	ਨਹੀਂਤਾਂ
	    p = skipdigits(p);
    }
    ਜੇ (!want_string && p[0] == '.' && vim_isdigit(p[1]))
    {
	get_float = TRUE;
	p = skipdigits(p + 2);
	ਜੇ (*p == 'e' || *p == 'E')
	{
	    ++p;
	    ਜੇ (*p == '-' || *p == '+')
		++p;
	    ਜੇ (!vim_isdigit(*p))
		get_float = FALSE;
	    ਨਹੀਂਤਾਂ
		p = skipdigits(p + 1);
	}
	ਜੇ (ASCII_ISALPHA(*p) || *p == '.')
	    get_float = FALSE;
    }
    ਜੇ (get_float)
    {
	float_T	f;

	*arg += string2float(*arg, &f, skip_quotes);
	ਜੇ (evaluate)
	{
	    rettv->v_type = VAR_FLOAT;
	    rettv->vval.v_float = f;
	}
    }
    ਨਹੀਂਤਾਂ
    ਜੇ (**arg == '0' && ((*arg)[1] == 'z' || (*arg)[1] == 'Z'))
    {
	char_u  *bp;
	blob_T  *blob = ਨਿਰਾ;  // init for gcc

	// Blob constant: 0z0123456789abcdef
	ਜੇ (evaluate)
	    blob = blob_alloc();
	ਲਈ (bp = *arg + 2; vim_isxdigit(bp[0]); bp += 2)
	{
	    ਜੇ (!vim_isxdigit(bp[1]))
	    {
		ਜੇ (blob != ਨਿਰਾ)
		{
		    emsg(_(e_blob_literal_should_have_an_even_ਸੰਖਿਆ_ਦਾ_hex_characters));
		    ga_clear(&blob->bv_ga);
		    VIM_CLEAR(blob);
		}
		ਵਾਪਸ FAIL;
	    }
	    ਜੇ (blob != ਨਿਰਾ)
		ga_append(&blob->bv_ga,
			     (hex2nr(*bp) << 4) + hex2nr(*(bp+1)));
	    ਜੇ (bp[2] == '.' && vim_isxdigit(bp[3]))
		++bp;
	}
	ਜੇ (blob != ਨਿਰਾ)
	    rettv_blob_ਸੈੱਟ(rettv, blob);
	*arg = bp;
    }
    ਨਹੀਂਤਾਂ
    {
	varnumber_T	n;

	// decimal, hex or octal number
	vim_str2nr(*arg, ਨਿਰਾ, &len, skip_quotes
		      ? STR2NR_NO_OCT + STR2NR_QUOTE
		      : STR2NR_ALL, &n, ਨਿਰਾ, 0, TRUE, ਨਿਰਾ);
	ਜੇ (len == 0)
	{
	    ਜੇ (evaluate)
		semsg(_(e_invalid_ਵਿਅੰਜਕ_str), *arg);
	    ਵਾਪਸ FAIL;
	}
	*arg += len;
	ਜੇ (evaluate)
	{
	    rettv->v_type = VAR_ਸੰਖਿਆ;
	    rettv->vval.v_ਸੰਖਿਆ = n;
	}
    }
    ਵਾਪਸ OK;
}

/*
 * Evaluate a string constant and put the result in "rettv".
 * "*arg" points to the double quote or to after it when "interpolate" is TRUE.
 * When "interpolate" is TRUE reduce "{{" to "{", reduce "}}" to "}" and stop
 * at a single "{".
 * Return OK or FAIL.
 */
    ਪੂਰਨ
eval_string(char_u **arg, typval_T *rettv, ਪੂਰਨ evaluate, ਪੂਰਨ interpolate)
{
    char_u	*p;
    char_u	*ਸਿਰਾ;
    ਪੂਰਨ		extra = interpolate ? 1 : 0;
    ਪੂਰਨ		off = interpolate ? 0 : 1;
    ਪੂਰਨ		len;

    // Find the end of the string, skipping backslashed characters.
    ਲਈ (p = *arg + off; *p != NUL && *p != '"'; MB_PTR_ADV(p))
    {
	ਜੇ (*p == '\\' && p[1] != NUL)
	{
	    ++p;
	    // A "\<x>" form occupies at least 4 characters, and produces up
	    // to 9 characters (6 for the char and 3 for a modifier):
	    // reserve space for 5 extra.
	    ਜੇ (*p == '<')
	    {
		ਪੂਰਨ		modifiers = 0;
		ਪੂਰਨ		flags = FSK_KEYCODE | FSK_ਵਿੱਚ_STRING;

		extra += 5;

		// Skip to the '>' to avoid using '{' inside for string
		// interpolation.
		ਜੇ (p[1] != '*')
		    flags |= FSK_SIMPLIFY;
		ਜੇ (ਲੱਭੋ_special_ਕੁੰਜੀ(&p, &modifiers, flags, ਨਿਰਾ) != 0)
		    --p;  // leave "p" on the ">"
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (interpolate && (*p == '{' || *p == '}'))
	{
	    ਜੇ (*p == '{' && p[1] != '{') // start of expression
		ਰੋਕੋ;
	    ++p;
	    ਜੇ (p[-1] == '}' && *p != '}') // single '}' is an error
	    {
		semsg(_(e_stray_closing_curly_str), *arg);
		ਵਾਪਸ FAIL;
	    }
	    --extra;  // "{{" becomes "{", "}}" becomes "}"
	}
    }

    ਜੇ (*p != '"' && !(interpolate && *p == '{'))
    {
	semsg(_(e_missing_double_quote_str), *arg);
	ਵਾਪਸ FAIL;
    }

    // If only parsing, set *arg and return here
    ਜੇ (!evaluate)
    {
	*arg = p + off;
	ਵਾਪਸ OK;
    }

    // Copy the string into allocated memory, handling backslashed
    // characters.
    rettv->v_type = VAR_STRING;
    len = (ਪੂਰਨ)(p - *arg + extra);
    rettv->vval.v_string = alloc(len);
    ਜੇ (rettv->vval.v_string == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    ਸਿਰਾ = rettv->vval.v_string;

    ਲਈ (p = *arg + off; *p != NUL && *p != '"'; )
    {
	ਜੇ (*p == '\\')
	{
	    ਚੋਣ (*++p)
	    {
		ਹਾਲਤ 'b': *ਸਿਰਾ++ = BS; ++p; ਰੋਕੋ;
		ਹਾਲਤ 'e': *ਸਿਰਾ++ = ESC; ++p; ਰੋਕੋ;
		ਹਾਲਤ 'f': *ਸਿਰਾ++ = FF; ++p; ਰੋਕੋ;
		ਹਾਲਤ 'n': *ਸਿਰਾ++ = NL; ++p; ਰੋਕੋ;
		ਹਾਲਤ 'r': *ਸਿਰਾ++ = CAR; ++p; ਰੋਕੋ;
		ਹਾਲਤ 't': *ਸਿਰਾ++ = TAB; ++p; ਰੋਕੋ;

		ਹਾਲਤ 'X': // hex: "\x1", "\x12"
		ਹਾਲਤ 'x':
		ਹਾਲਤ 'u': // Unicode: "\u0023"
		ਹਾਲਤ 'U':
			  ਜੇ (vim_isxdigit(p[1]))
			  {
			      ਪੂਰਨ	n, nr;
			      ਪੂਰਨ	c = ਸੁਰੱਖਿਅਤ_toupper(*p);

			      ਜੇ (c == 'X')
				  n = 2;
			      ਨਹੀਂਤਾਂ ਜੇ (*p == 'u')
				  n = 4;
			      ਨਹੀਂਤਾਂ
				  n = 8;
			      nr = 0;
			      ਜਦੋਂ (--n >= 0 && vim_isxdigit(p[1]))
			      {
				  ++p;
				  nr = (nr << 4) + hex2nr(*p);
			      }
			      ++p;
			      // For "\u" store the number according to
			      // 'encoding'.
			      ਜੇ (c != 'X')
				  ਸਿਰਾ += (*mb_char2bytes)(nr, ਸਿਰਾ);
			      ਨਹੀਂਤਾਂ
				  *ਸਿਰਾ++ = nr;
			  }
			  ਰੋਕੋ;

			  // octal: "\1", "\12", "\123"
		ਹਾਲਤ '0':
		ਹਾਲਤ '1':
		ਹਾਲਤ '2':
		ਹਾਲਤ '3':
		ਹਾਲਤ '4':
		ਹਾਲਤ '5':
		ਹਾਲਤ '6':
		ਹਾਲਤ '7': *ਸਿਰਾ = *p++ - '0';
			  ਜੇ (*p >= '0' && *p <= '7')
			  {
			      *ਸਿਰਾ = (*ਸਿਰਾ << 3) + *p++ - '0';
			      ਜੇ (*p >= '0' && *p <= '7')
				  *ਸਿਰਾ = (*ਸਿਰਾ << 3) + *p++ - '0';
			  }
			  ++ਸਿਰਾ;
			  ਰੋਕੋ;

			  // Special key, e.g.: "\<C-W>"
		ਹਾਲਤ '<':
			  {
			      ਪੂਰਨ flags = FSK_KEYCODE | FSK_ਵਿੱਚ_STRING;

			      ਜੇ (p[1] != '*')
				  flags |= FSK_SIMPLIFY;
			      extra = trans_special(&p, ਸਿਰਾ, flags, FALSE, ਨਿਰਾ);
			      ਜੇ (extra != 0)
			      {
				  ਸਿਰਾ += extra;
				  ਜੇ (ਸਿਰਾ >= rettv->vval.v_string + len)
				      iemsg("eval_string() used more space than allocated");
				  ਰੋਕੋ;
			      }
			  }
			  // FALLTHROUGH

		ਮੂਲ: MB_COPY_CHAR(p, ਸਿਰਾ);
			  ਰੋਕੋ;
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (interpolate && (*p == '{' || *p == '}'))
	    {
		ਜੇ (*p == '{' && p[1] != '{') // start of expression
		    ਰੋਕੋ;
		++p;  // reduce "{{" to "{" and "}}" to "}"
	    }
	    MB_COPY_CHAR(p, ਸਿਰਾ);
	}
    }
    *ਸਿਰਾ = NUL;
    ਜੇ (*p == '"' && !interpolate)
	++p;
    *arg = p;

    ਵਾਪਸ OK;
}

/*
 * Allocate a variable for a 'str''ing' constant.
 * When "interpolate" is TRUE reduce "{{" to "{" and stop at a single "{".
 * Return OK when a "rettv" was set to the string.
 * Return FAIL on error, "rettv" is not set.
 */
    ਪੂਰਨ
eval_lit_string(char_u **arg, typval_T *rettv, ਪੂਰਨ evaluate, ਪੂਰਨ interpolate)
{
    char_u	*p;
    char_u	*str;
    ਪੂਰਨ		reduce = interpolate ? -1 : 0;
    ਪੂਰਨ		off = interpolate ? 0 : 1;

    // Find the end of the string, skipping ''.
    ਲਈ (p = *arg + off; *p != NUL; MB_PTR_ADV(p))
    {
	ਜੇ (*p == '\'')
	{
	    ਜੇ (p[1] != '\'')
		ਰੋਕੋ;
	    ++reduce;
	    ++p;
	}
	ਨਹੀਂਤਾਂ ਜੇ (interpolate)
	{
	    ਜੇ (*p == '{')
	    {
		ਜੇ (p[1] != '{')
		    ਰੋਕੋ;
		++p;
		++reduce;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (*p == '}')
	    {
		++p;
		ਜੇ (*p != '}')
		{
		    semsg(_(e_stray_closing_curly_str), *arg);
		    ਵਾਪਸ FAIL;
		}
		++reduce;
	    }
	}
    }

    ਜੇ (*p != '\'' && !(interpolate && *p == '{'))
    {
	semsg(_(e_missing_single_quote_str), *arg);
	ਵਾਪਸ FAIL;
    }

    // If only parsing return after setting "*arg"
    ਜੇ (!evaluate)
    {
	*arg = p + off;
	ਵਾਪਸ OK;
    }

    // Copy the string into allocated memory, handling '' to ' reduction and
    // any expressions.
    str = alloc((p - *arg) - reduce);
    ਜੇ (str == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = str;

    ਲਈ (p = *arg + off; *p != NUL; )
    {
	ਜੇ (*p == '\'')
	{
	    ਜੇ (p[1] != '\'')
		ਰੋਕੋ;
	    ++p;
	}
	ਨਹੀਂਤਾਂ ਜੇ (interpolate && (*p == '{' || *p == '}'))
	{
	    ਜੇ (*p == '{' && p[1] != '{')
		ਰੋਕੋ;
	    ++p;
	}
	MB_COPY_CHAR(p, str);
    }
    *str = NUL;
    *arg = p + off;

    ਵਾਪਸ OK;
}

/*
 * Evaluate a single or double quoted string possibly containing expressions.
 * "arg" points to the '$'.  The result is put in "rettv".
 * Returns OK or FAIL.
 */
    ਪੂਰਨ
eval_interp_string(char_u **arg, typval_T *rettv, ਪੂਰਨ evaluate)
{
    typval_T	tv;
    ਪੂਰਨ		ret = OK;
    ਪੂਰਨ		quote;
    garray_T	ga;
    char_u	*p;

    ga_init2(&ga, 1, 80);

    // *arg is on the '$' character, move it to the first string character.
    ++*arg;
    quote = **arg;
    ++*arg;

    ਲਈ (;;)
    {
	// Get the string up to the matching quote or to a single '{'.
	// "arg" is advanced to either the quote or the '{'.
	ਜੇ (quote == '"')
	    ret = eval_string(arg, &tv, evaluate, TRUE);
	ਨਹੀਂਤਾਂ
	    ret = eval_lit_string(arg, &tv, evaluate, TRUE);
	ਜੇ (ret == FAIL)
	    ਰੋਕੋ;
	ਜੇ (evaluate)
	{
	    ga_concat(&ga, tv.vval.v_string);
	    clear_tv(&tv);
	}

	ਜੇ (**arg != '{')
	{
	    // found terminating quote
	    ++*arg;
	    ਰੋਕੋ;
	}
	p = eval_one_ਵਿਅੰਜਕ_ਵਿੱਚ_str(*arg, &ga, evaluate);
	ਜੇ (p == ਨਿਰਾ)
	{
	    ret = FAIL;
	    ਰੋਕੋ;
	}
	*arg = p;
    }

    rettv->v_type = VAR_STRING;
    ਜੇ (ret == FAIL || !evaluate || ga_append(&ga, NUL) == FAIL)
    {
	ga_clear(&ga);
	rettv->vval.v_string = ਨਿਰਾ;
	ਵਾਪਸ ret;
    }

    rettv->vval.v_string = ga.ga_ਅੰਕੜੇ;
    ਵਾਪਸ OK;
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
    ਪੂਰਨ		copyID)
{
    ਵਾਪਸ echo_string_core(tv, tofree, numbuf, copyID, FALSE, TRUE, FALSE);
}

/*
 * Get the value of an environment variable.
 * "arg" is pointing to the '$'.  It is advanced to after the name.
 * If the environment variable was not set, silently assume it is empty.
 * Return FAIL if the name is invalid.
 */
    ਪੂਰਨ
eval_env_var(char_u **arg, typval_T *rettv, ਪੂਰਨ evaluate)
{
    char_u	*string = ਨਿਰਾ;
    ਪੂਰਨ		len;
    ਪੂਰਨ		cc;
    char_u	*name;
    ਪੂਰਨ		mustfree = FALSE;

    ++*arg;
    name = *arg;
    len = get_env_len(arg);
    ਜੇ (evaluate)
    {
	ਜੇ (len == 0)
	    ਵਾਪਸ FAIL; // invalid empty name

	cc = name[len];
	name[len] = NUL;
	// first try vim_getenv(), fast for normal environment vars
	string = vim_getenv(name, &mustfree);
	ਜੇ (string != ਨਿਰਾ && *string != NUL)
	{
	    ਜੇ (!mustfree)
		string = vim_strsave(string);
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (mustfree)
		vim_free(string);

	    // next try expanding things like $VIM and ${HOME}
	    string = expand_env_save(name - 1);
	    ਜੇ (string != ਨਿਰਾ && *string == '$')
		VIM_CLEAR(string);
	}
	name[len] = cc;

	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = string;
	rettv->v_ਤਾਲਾ = 0;
    }

    ਵਾਪਸ OK;
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
    ਪੂਰਨ		did_emsg_before = did_emsg;

    ਜੇ (argvars[0].v_type != VAR_STRING || !ਵਿੱਚ_vim9script())
	lnum = (linenr_T)tv_get_ਸੰਖਿਆ_chk(&argvars[0], ਨਿਰਾ);
    ਜੇ (lnum <= 0 && did_emsg_before == did_emsg
					    && argvars[0].v_type != VAR_ਸੰਖਿਆ)
    {
	ਪੂਰਨ	fnum;
	pos_T	*fp;

	// no valid number, try using arg like line()
	fp = var2fpos(&argvars[0], TRUE, &fnum, FALSE);
	ਜੇ (fp != ਨਿਰਾ)
	    lnum = fp->lnum;
    }
    ਵਾਪਸ lnum;
}

/*
 * Get the lnum from the first argument.
 * Also accepts "$", then "buf" is used.
 * Returns 0 on error.
 */
    linenr_T
tv_get_lnum_buf(typval_T *argvars, buf_T *buf)
{
    ਜੇ (argvars[0].v_type == VAR_STRING
	    && argvars[0].vval.v_string != ਨਿਰਾ
	    && argvars[0].vval.v_string[0] == '$'
	    && argvars[0].vval.v_string[1] == NUL
	    && buf != ਨਿਰਾ)
	ਵਾਪਸ buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
    ਵਾਪਸ (linenr_T)tv_get_ਸੰਖਿਆ_chk(&argvars[0], ਨਿਰਾ);
}

/*
 * Get buffer by number or pattern.
 */
    buf_T *
tv_get_buf(typval_T *tv, ਪੂਰਨ curtab_only)
{
    char_u	*name = tv->vval.v_string;
    buf_T	*buf;

    ਜੇ (tv->v_type == VAR_ਸੰਖਿਆ)
	ਵਾਪਸ buflist_findnr((ਪੂਰਨ)tv->vval.v_ਸੰਖਿਆ);
    ਜੇ (tv->v_type != VAR_STRING)
	ਵਾਪਸ ਨਿਰਾ;
    ਜੇ (name == ਨਿਰਾ || *name == NUL)
	ਵਾਪਸ curbuf;
    ਜੇ (name[0] == '$' && name[1] == NUL)
	ਵਾਪਸ lastbuf;

    buf = buflist_ਲੱਭੋ_by_name(name, curtab_only);

    // If not found, try expanding the name, like done for bufexists().
    ਜੇ (buf == ਨਿਰਾ)
	buf = ਲੱਭੋ_ਬਫਰ(tv);

    ਵਾਪਸ buf;
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
    ਜੇ (buf == ਨਿਰਾ
	    && tv->v_type != VAR_ਸੰਖਿਆ
	    && tv->v_type != VAR_STRING)
	// issue errmsg for type error
	(ਖਾਲੀ)tv_get_ਸੰਖਿਆ(tv);
    ਵਾਪਸ buf;
}

/*
 * Check that repeating "slen" items "n" times, does not
 * overflow the int used for the resulting length.
 */
    ਪੂਰਨ
check_repeat_ਗਿਣਤੀ(varnumber_T slen, varnumber_T n)
{
    // Use a division so that the multiplication cannot overflow.
    ਜੇ (n <= (varnumber_T)ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ / slen)
	ਵਾਪਸ OK;

    ਅੱਖਰ	buf[NUMBUFLEN];

    vim_snprintf(buf, ਆਕਾਰ(buf), "%lld", (ਲੰਮਾ ਲੰਮਾ)n);
    semsg(_(e_val_too_large), buf);
    ਵਾਪਸ FAIL;
}

#ਜੇ_ਅੰਤ // FEAT_EVAL
