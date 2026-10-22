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

#यदि defined(FEAT_EVAL)

/*
 * Allocate memory for a variable type-value, and make it empty (0 or NULL
 * value).
 */
    typval_T *
alloc_tv(शून्य)
{
    फर्काऊ ALLOC_CLEAR_ONE(typval_T);
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
    यदि (rettv != रिक्त)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = s;
    }
    अन्यथा
	vim_free(s);
    फर्काऊ rettv;
}

/*
 * Free the memory for a variable type-value.
 */
    शून्य
free_tv(typval_T *varp)
{
    यदि (varp == रिक्त)
	फर्काऊ;

    विकल्प (varp->v_type)
    {
	पक्ष VAR_FUNC:
	    func_unref(varp->vval.v_string);
	    // FALLTHROUGH
	पक्ष VAR_STRING:
	    vim_free(varp->vval.v_string);
	    रोक;
	पक्ष VAR_PARTIAL:
	    partial_unref(varp->vval.v_partial);
	    रोक;
	पक्ष VAR_BLOB:
	    blob_unref(varp->vval.v_blob);
	    रोक;
	पक्ष VAR_LIST:
	    list_unref(varp->vval.v_list);
	    रोक;
	पक्ष VAR_TUPLE:
	    tuple_unref(varp->vval.v_tuple);
	    रोक;
	पक्ष VAR_DICT:
	    dict_unref(varp->vval.v_dict);
	    रोक;
	पक्ष VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    job_unref(varp->vval.v_job);
	    रोक;
#endif
	पक्ष VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    channel_unref(varp->vval.v_channel);
	    रोक;
#endif
	पक्ष VAR_CLASS:
	    class_unref(varp->vval.v_class);
	    रोक;
	पक्ष VAR_OBJECT:
	    object_unref(varp->vval.v_object);
	    रोक;

	पक्ष VAR_TYPEALIAS:
	    typealias_unref(varp->vval.v_typealias);
	    रोक;

	पक्ष VAR_सङ्ख्या:
	पक्ष VAR_FLOAT:
	पक्ष VAR_ANY:
	पक्ष VAR_UNKNOWN:
	पक्ष VAR_VOID:
	पक्ष VAR_BOOL:
	पक्ष VAR_SPECIAL:
	पक्ष VAR_INSTR:
	    रोक;
    }
    vim_free(varp);
}

/*
 * Free the memory for a variable value and set the value to NULL or 0.
 */
    शून्य
clear_tv(typval_T *varp)
{
    यदि (varp == रिक्त)
	फर्काऊ;

    विकल्प (varp->v_type)
    {
	पक्ष VAR_FUNC:
	    func_unref(varp->vval.v_string);
	    // FALLTHROUGH
	पक्ष VAR_STRING:
	    VIM_CLEAR(varp->vval.v_string);
	    रोक;
	पक्ष VAR_PARTIAL:
	    partial_unref(varp->vval.v_partial);
	    varp->vval.v_partial = रिक्त;
	    रोक;
	पक्ष VAR_BLOB:
	    blob_unref(varp->vval.v_blob);
	    varp->vval.v_blob = रिक्त;
	    रोक;
	पक्ष VAR_LIST:
	    list_unref(varp->vval.v_list);
	    varp->vval.v_list = रिक्त;
	    रोक;
	पक्ष VAR_TUPLE:
	    tuple_unref(varp->vval.v_tuple);
	    varp->vval.v_tuple = रिक्त;
	    रोक;
	पक्ष VAR_DICT:
	    dict_unref(varp->vval.v_dict);
	    varp->vval.v_dict = रिक्त;
	    रोक;
	पक्ष VAR_सङ्ख्या:
	पक्ष VAR_BOOL:
	पक्ष VAR_SPECIAL:
	    varp->vval.v_सङ्ख्या = 0;
	    रोक;
	पक्ष VAR_FLOAT:
	    varp->vval.v_float = 0.0;
	    रोक;
	पक्ष VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    job_unref(varp->vval.v_job);
	    varp->vval.v_job = रिक्त;
#endif
	    रोक;
	पक्ष VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    channel_unref(varp->vval.v_channel);
	    varp->vval.v_channel = रिक्त;
#endif
	    रोक;
	पक्ष VAR_INSTR:
	    VIM_CLEAR(varp->vval.v_instr);
	    रोक;
	पक्ष VAR_CLASS:
	    class_unref(varp->vval.v_class);
	    varp->vval.v_class = रिक्त;
	    रोक;
	पक्ष VAR_OBJECT:
	    object_unref(varp->vval.v_object);
	    varp->vval.v_object = रिक्त;
	    रोक;
	पक्ष VAR_TYPEALIAS:
	    typealias_unref(varp->vval.v_typealias);
	    varp->vval.v_typealias = रिक्त;
	    रोक;
	पक्ष VAR_UNKNOWN:
	पक्ष VAR_ANY:
	पक्ष VAR_VOID:
	    रोक;
    }
    varp->v_lock = 0;
}

/*
 * Set the value of a variable to NULL without freeing items.
 */
    शून्य
init_tv(typval_T *varp)
{
    यदि (varp != रिक्त)
	CLEAR_POINTER(varp);
}

    स्थिर varnumber_T
tv_get_bool_or_सङ्ख्या_chk(
	typval_T    *varp,
	पूर्णाङ्क	    *denote,
	पूर्णाङ्क	    want_bool,
	पूर्णाङ्क	    vim9_string_error) // in Vim9 using a string is an error
{
    varnumber_T	n = 0L;

    विकल्प (varp->v_type)
    {
	पक्ष VAR_सङ्ख्या:
	    यदि (in_vim9script() && want_bool && varp->vval.v_सङ्ख्या != 0
						   && varp->vval.v_सङ्ख्या != 1)
	    {
		semsg(_(e_using_सङ्ख्या_as_bool_nr), varp->vval.v_सङ्ख्या);
		रोक;
	    }
	    फर्काऊ varp->vval.v_सङ्ख्या;
	पक्ष VAR_FLOAT:
	    emsg(_(e_using_float_as_सङ्ख्या));
	    रोक;
	पक्ष VAR_FUNC:
	पक्ष VAR_PARTIAL:
	    emsg(_(e_using_funcref_as_सङ्ख्या));
	    रोक;
	पक्ष VAR_STRING:
	    यदि (vim9_string_error && in_vim9script())
	    {
		emsg_using_string_as(varp, !want_bool);
		रोक;
	    }
	    यदि (varp->vval.v_string != रिक्त)
		vim_str2nr(varp->vval.v_string, रिक्त, रिक्त,
					 STR2NR_ALL, &n, रिक्त, 0, FALSE, रिक्त);
	    फर्काऊ n;
	पक्ष VAR_LIST:
	    emsg(_(e_using_list_as_सङ्ख्या));
	    रोक;
	पक्ष VAR_TUPLE:
	    emsg(_(e_using_tuple_as_सङ्ख्या));
	    रोक;
	पक्ष VAR_DICT:
	    emsg(_(e_using_dictionary_as_सङ्ख्या));
	    रोक;
	पक्ष VAR_BOOL:
	पक्ष VAR_SPECIAL:
	    यदि (!want_bool && in_vim9script())
	    {
		यदि (varp->v_type == VAR_BOOL)
		    emsg(_(e_using_bool_as_सङ्ख्या));
		अन्यथा
		    emsg(_(e_using_special_as_सङ्ख्या));
		रोक;
	    }
	    फर्काऊ varp->vval.v_सङ्ख्या == VVAL_TRUE ? 1 : 0;
	पक्ष VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    emsg(_(e_using_job_as_सङ्ख्या));
	    रोक;
#endif
	पक्ष VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    emsg(_(e_using_channel_as_सङ्ख्या));
	    रोक;
#endif
	पक्ष VAR_BLOB:
	    emsg(_(e_using_blob_as_सङ्ख्या));
	    रोक;
	पक्ष VAR_CLASS:
	पक्ष VAR_TYPEALIAS:
	    check_typval_is_मान(varp);
	    रोक;
	पक्ष VAR_OBJECT:
	    {
		यदि (varp->vval.v_object == रिक्त)
		    emsg(_(e_using_object_as_string));
		अन्यथा
		{
		    class_T *cl = varp->vval.v_object->obj_class;
		    यदि (cl != रिक्त && IS_ENUM(cl))
			semsg(_(e_using_enum_str_as_सङ्ख्या), cl->class_name.string);
		    अन्यथा
			emsg(_(e_using_object_as_सङ्ख्या));
		}
	    }
	    रोक;
	पक्ष VAR_VOID:
	    emsg(_(e_cannot_use_void_मान));
	    रोक;
	पक्ष VAR_UNKNOWN:
	पक्ष VAR_ANY:
	पक्ष VAR_INSTR:
	    internal_error_no_abort("tv_get_number(UNKNOWN)");
	    रोक;
    }
    यदि (denote == रिक्त)		// useful for values that must be unsigned
	n = -1;
    अन्यथा
	*denote = TRUE;
    फर्काऊ n;
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
tv_get_सङ्ख्या(typval_T *varp)
{
    पूर्णाङ्क		error = FALSE;

    फर्काऊ tv_get_सङ्ख्या_chk(varp, &error);	// return 0L on error
}

/*
 * Like tv_get_number() but in Vim9 script do convert a number in a string to a
 * number without giving an error.
 */
    varnumber_T
tv_to_सङ्ख्या(typval_T *varp)
{
    पूर्णाङ्क		error = FALSE;

    फर्काऊ tv_get_bool_or_सङ्ख्या_chk(varp, &error, FALSE, FALSE);
}

    varnumber_T
tv_get_सङ्ख्या_chk(typval_T *varp, पूर्णाङ्क *denote)
{
    फर्काऊ tv_get_bool_or_सङ्ख्या_chk(varp, denote, FALSE, TRUE);
}

/*
 * Get the boolean value of "varp".  This is like tv_get_number_chk(),
 * but in Vim9 script accepts Number (0 and 1) and Bool/Special.
 */
    varnumber_T
tv_get_bool(typval_T *varp)
{
    फर्काऊ tv_get_bool_or_सङ्ख्या_chk(varp, रिक्त, TRUE, TRUE);
}

/*
 * Get the boolean value of "varp".  This is like tv_get_number_chk(),
 * but in Vim9 script accepts Number and Bool.
 */
    varnumber_T
tv_get_bool_chk(typval_T *varp, पूर्णाङ्क *denote)
{
    फर्काऊ tv_get_bool_or_सङ्ख्या_chk(varp, denote, TRUE, TRUE);
}

    स्थिर float_T
tv_get_float_chk(typval_T *varp, पूर्णाङ्क *error)
{
    विकल्प (varp->v_type)
    {
	पक्ष VAR_सङ्ख्या:
	    फर्काऊ (float_T)(varp->vval.v_सङ्ख्या);
	पक्ष VAR_FLOAT:
	    फर्काऊ varp->vval.v_float;
	पक्ष VAR_FUNC:
	पक्ष VAR_PARTIAL:
	    emsg(_(e_using_funcref_as_float));
	    रोक;
	पक्ष VAR_STRING:
	    emsg(_(e_using_string_as_float));
	    रोक;
	पक्ष VAR_LIST:
	    emsg(_(e_using_list_as_float));
	    रोक;
	पक्ष VAR_TUPLE:
	    emsg(_(e_using_tuple_as_float));
	    रोक;
	पक्ष VAR_DICT:
	    emsg(_(e_using_dictionary_as_float));
	    रोक;
	पक्ष VAR_BOOL:
	    emsg(_(e_using_boolean_मान_as_float));
	    रोक;
	पक्ष VAR_SPECIAL:
	    emsg(_(e_using_special_मान_as_float));
	    रोक;
	पक्ष VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    emsg(_(e_using_job_as_float));
	    रोक;
#endif
	पक्ष VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    emsg(_(e_using_channel_as_float));
	    रोक;
#endif
	पक्ष VAR_BLOB:
	    emsg(_(e_using_blob_as_float));
	    रोक;
	पक्ष VAR_CLASS:
	पक्ष VAR_TYPEALIAS:
	    check_typval_is_मान(varp);
	    रोक;
	पक्ष VAR_OBJECT:
	    emsg(_(e_using_object_as_float));
	    रोक;
	पक्ष VAR_VOID:
	    emsg(_(e_cannot_use_void_मान));
	    रोक;
	पक्ष VAR_UNKNOWN:
	पक्ष VAR_ANY:
	पक्ष VAR_INSTR:
	    internal_error_no_abort("tv_get_float(UNKNOWN)");
	    रोक;
    }
    यदि (error != रिक्त)
	*error = TRUE;
    फर्काऊ 0;
}

    float_T
tv_get_float(typval_T *varp)
{
    फर्काऊ tv_get_float_chk(varp, रिक्त);
}

/*
 * Give an error and return FAIL unless "args[idx]" is unknown
 */
    पूर्णाङ्क
check_for_unknown_arg(typval_T *args, पूर्णाङ्क idx)
{
    यदि (args[idx].v_type != VAR_UNKNOWN)
    {
	semsg(_(e_too_many_arguments), idx + 1);
	फर्काऊ FAIL;
    }
    फर्काऊ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string.
 */
    पूर्णाङ्क
check_for_string_arg(typval_T *args, पूर्णाङ्क idx)
{
    यदि (args[idx].v_type != VAR_STRING)
    {
	semsg(_(e_string_required_for_argument_nr), idx + 1);
	फर्काऊ FAIL;
    }
    फर्काऊ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a non-empty string.
 */
    पूर्णाङ्क
check_for_nonempty_string_arg(typval_T *args, पूर्णाङ्क idx)
{
    यदि (check_for_string_arg(args, idx) == FAIL)
	फर्काऊ FAIL;
    यदि (args[idx].vval.v_string == रिक्त || *args[idx].vval.v_string == NUL)
    {
	semsg(_(e_non_empty_string_required_for_argument_nr), idx + 1);
	फर्काऊ FAIL;
    }
    फर्काऊ OK;
}

/*
 * Check for an optional string argument at 'idx'
 */
    पूर्णाङ्क
check_for_opt_string_arg(typval_T *args, पूर्णाङ्क idx)
{
    फर्काऊ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_string_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a number.
 */
    पूर्णाङ्क
check_for_सङ्ख्या_arg(typval_T *args, पूर्णाङ्क idx)
{
    यदि (args[idx].v_type != VAR_सङ्ख्या)
    {
	semsg(_(e_सङ्ख्या_required_for_argument_nr), idx + 1);
	फर्काऊ FAIL;
    }
    फर्काऊ OK;
}

/*
 * Check for an optional number argument at 'idx'
 */
    पूर्णाङ्क
check_for_opt_सङ्ख्या_arg(typval_T *args, पूर्णाङ्क idx)
{
    फर्काऊ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_सङ्ख्या_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a float or a number.
 */
    पूर्णाङ्क
check_for_float_or_nr_arg(typval_T *args, पूर्णाङ्क idx)
{
    यदि (args[idx].v_type != VAR_FLOAT && args[idx].v_type != VAR_सङ्ख्या)
    {
	semsg(_(e_float_or_सङ्ख्या_required_for_argument_nr), idx + 1);
	फर्काऊ FAIL;
    }
    फर्काऊ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a bool.
 */
    पूर्णाङ्क
check_for_bool_arg(typval_T *args, पूर्णाङ्क idx)
{
    यदि (args[idx].v_type != VAR_BOOL
	    && !(args[idx].v_type == VAR_सङ्ख्या
		&& (args[idx].vval.v_सङ्ख्या == 0
		    || args[idx].vval.v_सङ्ख्या == 1)))
    {
	semsg(_(e_bool_required_for_argument_nr), idx + 1);
	फर्काऊ FAIL;
    }
    फर्काऊ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a bool or a number.
 */
    स्थिर पूर्णाङ्क
check_for_bool_or_सङ्ख्या_arg(typval_T *args, पूर्णाङ्क idx)
{
    यदि (args[idx].v_type != VAR_BOOL && args[idx].v_type != VAR_सङ्ख्या)
    {
	semsg(_(e_bool_or_सङ्ख्या_required_for_argument_nr), idx + 1);
	फर्काऊ FAIL;
    }
    फर्काऊ OK;
}

/*
 * Check for an optional bool argument at 'idx'.
 * Return FAIL if the type is wrong.
 */
    पूर्णाङ्क
check_for_opt_bool_arg(typval_T *args, पूर्णाङ्क idx)
{
    यदि (args[idx].v_type == VAR_UNKNOWN)
	फर्काऊ OK;
    फर्काऊ check_for_bool_arg(args, idx);
}

/*
 * Check for an optional bool or number argument at 'idx'.
 * Return FAIL if the type is wrong.
 */
    पूर्णाङ्क
check_for_opt_bool_or_सङ्ख्या_arg(typval_T *args, पूर्णाङ्क idx)
{
    यदि (args[idx].v_type == VAR_UNKNOWN)
	फर्काऊ OK;
    फर्काऊ check_for_bool_or_सङ्ख्या_arg(args, idx);
}

/*
 * Check for an optional bool or dict argument at 'idx'.
 * Return FAIL if the type is wrong.
 */
    पूर्णाङ्क
check_for_opt_bool_or_dict_arg(typval_T *args, पूर्णाङ्क idx)
{
    यदि (args[idx].v_type == VAR_UNKNOWN || args[idx].v_type == VAR_DICT)
	फर्काऊ OK;
    फर्काऊ check_for_bool_arg(args, idx);
}

/*
 * Give an error and return FAIL unless "args[idx]" is a blob.
 */
    पूर्णाङ्क
check_for_blob_arg(typval_T *args, पूर्णाङ्क idx)
{
    यदि (args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_blob_required_for_argument_nr), idx + 1);
	फर्काऊ FAIL;
    }
    फर्काऊ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list.
 */
    पूर्णाङ्क
check_for_list_arg(typval_T *args, पूर्णाङ्क idx)
{
    यदि (args[idx].v_type != VAR_LIST)
    {
	semsg(_(e_list_required_for_argument_nr), idx + 1);
	फर्काऊ FAIL;
    }
    फर्काऊ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a non-NULL list.
 */
    पूर्णाङ्क
check_for_nonnull_list_arg(typval_T *args, पूर्णाङ्क idx)
{
    यदि (check_for_list_arg(args, idx) == FAIL)
	फर्काऊ FAIL;

    यदि (args[idx].vval.v_list == रिक्त)
    {
	semsg(_(e_non_null_list_required_for_argument_nr), idx + 1);
	फर्काऊ FAIL;
    }
    फर्काऊ OK;
}

/*
 * Check for an optional list argument at 'idx'
 */
    पूर्णाङ्क
check_for_opt_list_arg(typval_T *args, पूर्णाङ्क idx)
{
    फर्काऊ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_list_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a tuple.
 */
    पूर्णाङ्क
check_for_tuple_arg(typval_T *args, पूर्णाङ्क idx)
{
    यदि (args[idx].v_type != VAR_TUPLE)
    {
	semsg(_(e_tuple_required_for_argument_nr), idx + 1);
	फर्काऊ FAIL;
    }
    फर्काऊ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a dict.
 */
    पूर्णाङ्क
check_for_dict_arg(typval_T *args, पूर्णाङ्क idx)
{
    यदि (args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_dict_required_for_argument_nr), idx + 1);
	फर्काऊ FAIL;
    }
    फर्काऊ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a non-NULL dict.
 */
    पूर्णाङ्क
check_for_nonnull_dict_arg(typval_T *args, पूर्णाङ्क idx)
{
    यदि (check_for_dict_arg(args, idx) == FAIL)
	फर्काऊ FAIL;

    यदि (args[idx].vval.v_dict == रिक्त)
    {
	semsg(_(e_non_null_dict_required_for_argument_nr), idx + 1);
	फर्काऊ FAIL;
    }
    फर्काऊ OK;
}

/*
 * Check for an optional dict argument at 'idx'
 */
    पूर्णाङ्क
check_for_opt_dict_arg(typval_T *args, पूर्णाङ्क idx)
{
    फर्काऊ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_dict_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Check for an optional non-NULL dict argument at 'idx'
 */
    पूर्णाङ्क
check_for_opt_nonnull_dict_arg(typval_T *args, पूर्णाङ्क idx)
{
    फर्काऊ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_nonnull_dict_arg(args, idx) != FAIL) ? OK : FAIL;
}

#यदि defined(FEAT_JOB_CHANNEL)
/*
 * Give an error and return FAIL unless "args[idx]" is a channel or a job.
 */
    पूर्णाङ्क
check_for_chan_or_job_arg(typval_T *args, पूर्णाङ्क idx)
{
    यदि (args[idx].v_type != VAR_CHANNEL && args[idx].v_type != VAR_JOB)
    {
	semsg(_(e_chan_or_job_required_for_argument_nr), idx + 1);
	फर्काऊ FAIL;
    }
    फर्काऊ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an optional channel or a
 * job.
 */
    पूर्णाङ्क
check_for_opt_chan_or_job_arg(typval_T *args, पूर्णाङ्क idx)
{
    फर्काऊ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_chan_or_job_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a job.
 */
    पूर्णाङ्क
check_for_job_arg(typval_T *args, पूर्णाङ्क idx)
{
    यदि (args[idx].v_type != VAR_JOB)
    {
	semsg(_(e_job_required_for_argument_nr), idx + 1);
	फर्काऊ FAIL;
    }
    फर्काऊ OK;
}

/*
 * Check for an optional job argument at 'idx'.
 */
    पूर्णाङ्क
check_for_opt_job_arg(typval_T *args, पूर्णाङ्क idx)
{
    फर्काऊ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_job_arg(args, idx) != FAIL) ? OK : FAIL;
}
#अन्यथा
/*
 * Give an error and return FAIL unless "args[idx]" is an optional channel or a
 * job.  Used without the +channel feature, thus only VAR_UNKNOWN is accepted.
 */
    पूर्णाङ्क
check_for_opt_chan_or_job_arg(typval_T *args, पूर्णाङ्क idx)
{
    फर्काऊ args[idx].v_type == VAR_UNKNOWN ? OK : FAIL;
}
#endif

/*
 * Give an error and return FAIL unless "args[idx]" is a string or
 * a number.
 */
    पूर्णाङ्क
check_for_string_or_सङ्ख्या_arg(typval_T *args, पूर्णाङ्क idx)
{
    यदि (args[idx].v_type != VAR_STRING && args[idx].v_type != VAR_सङ्ख्या)
    {
	semsg(_(e_string_or_सङ्ख्या_required_for_argument_nr), idx + 1);
	फर्काऊ FAIL;
    }
    फर्काऊ OK;
}

/*
 * Check for an optional string or number argument at 'idx'.
 */
    पूर्णाङ्क
check_for_opt_string_or_सङ्ख्या_arg(typval_T *args, पूर्णाङ्क idx)
{
    फर्काऊ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_string_or_सङ्ख्या_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a buffer number.
 * Buffer number can be a number or a string.
 */
    पूर्णाङ्क
check_for_buffer_arg(typval_T *args, पूर्णाङ्क idx)
{
    फर्काऊ check_for_string_or_सङ्ख्या_arg(args, idx);
}

/*
 * Check for an optional buffer argument at 'idx'
 */
    पूर्णाङ्क
check_for_opt_buffer_arg(typval_T *args, पूर्णाङ्क idx)
{
    फर्काऊ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_buffer_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a line number.
 * Line number can be a number or a string.
 */
    पूर्णाङ्क
check_for_lnum_arg(typval_T *args, पूर्णाङ्क idx)
{
    फर्काऊ check_for_string_or_सङ्ख्या_arg(args, idx);
}

/*
 * Check for an optional line number argument at 'idx'
 */
    पूर्णाङ्क
check_for_opt_lnum_arg(typval_T *args, पूर्णाङ्क idx)
{
    फर्काऊ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_lnum_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a blob.
 */
    पूर्णाङ्क
check_for_string_or_blob_arg(typval_T *args, पूर्णाङ्क idx)
{
    यदि (args[idx].v_type != VAR_STRING && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_string_or_blob_required_for_argument_nr), idx + 1);
	फर्काऊ FAIL;
    }
    फर्काऊ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a list.
 */
    पूर्णाङ्क
check_for_string_or_list_arg(typval_T *args, पूर्णाङ्क idx)
{
    यदि (args[idx].v_type != VAR_STRING && args[idx].v_type != VAR_LIST)
    {
	semsg(_(e_string_or_list_required_for_argument_nr), idx + 1);
	फर्काऊ FAIL;
    }
    फर्काऊ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string, a list, a
 * tuple or a blob.
 */
    पूर्णाङ्क
check_for_string_or_list_or_tuple_or_blob_arg(typval_T *args, पूर्णाङ्क idx)
{
    यदि (args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_string_list_tuple_or_blob_required_for_argument_nr), idx + 1);
	फर्काऊ FAIL;
    }
    फर्काऊ OK;
}

/*
 * Check for an optional string or list argument at 'idx'
 */
    पूर्णाङ्क
check_for_opt_string_or_list_arg(typval_T *args, पूर्णाङ्क idx)
{
    फर्काऊ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_string_or_list_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a dict.
 */
    पूर्णाङ्क
check_for_string_or_dict_arg(typval_T *args, पूर्णाङ्क idx)
{
    यदि (args[idx].v_type != VAR_STRING && args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_string_or_dict_required_for_argument_nr), idx + 1);
	फर्काऊ FAIL;
    }
    फर्काऊ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a number
 * or a list.
 */
    पूर्णाङ्क
check_for_string_or_सङ्ख्या_or_list_arg(typval_T *args, पूर्णाङ्क idx)
{
    यदि (args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_सङ्ख्या
	    && args[idx].v_type != VAR_LIST)
    {
	semsg(_(e_string_सङ्ख्या_or_list_required_for_argument_nr), idx + 1);
	फर्काऊ FAIL;
    }
    फर्काऊ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an optional string
 * or number or a list
 */
    पूर्णाङ्क
check_for_opt_string_or_सङ्ख्या_or_list_arg(typval_T *args, पूर्णाङ्क idx)
{
    फर्काऊ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_string_or_सङ्ख्या_or_list_arg(args, idx)
							!= FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string, a number, a
 * list, a tuple or a blob.
 */
    पूर्णाङ्क
check_for_repeat_func_arg(typval_T *args, पूर्णाङ्क idx)
{
    यदि (args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_सङ्ख्या
	    && args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_repeatable_type_required_for_argument_nr), idx + 1);
	फर्काऊ FAIL;
    }
    फर्काऊ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string, a list, a
 * tuple or a dict.
 */
    पूर्णाङ्क
check_for_string_list_tuple_or_dict_arg(typval_T *args, पूर्णाङ्क idx)
{
    यदि (args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_string_list_tuple_or_dict_required_for_argument_nr), idx + 1);
	फर्काऊ FAIL;
    }
    फर्काऊ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string
 * or a function reference.
 */
    पूर्णाङ्क
check_for_string_or_func_arg(typval_T *args, पूर्णाङ्क idx)
{
    यदि (args[idx].v_type != VAR_PARTIAL
	    && args[idx].v_type != VAR_FUNC
	    && args[idx].v_type != VAR_STRING)
    {
	semsg(_(e_string_or_function_required_for_argument_nr), idx + 1);
	फर्काऊ FAIL;
    }
    फर्काऊ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list or a blob.
 */
    पूर्णाङ्क
check_for_list_or_blob_arg(typval_T *args, पूर्णाङ्क idx)
{
    यदि (args[idx].v_type != VAR_LIST && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_list_or_blob_required_for_argument_nr), idx + 1);
	फर्काऊ FAIL;
    }
    फर्काऊ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list or a tuple.
 */
    पूर्णाङ्क
check_for_list_or_tuple_arg(typval_T *args, पूर्णाङ्क idx)
{
    यदि (args[idx].v_type != VAR_LIST && args[idx].v_type != VAR_TUPLE)
    {
	semsg(_(e_list_or_tuple_required_for_argument_nr), idx + 1);
	फर्काऊ FAIL;
    }
    फर्काऊ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list, a tuple or a
 * blob.
 */
    पूर्णाङ्क
check_for_list_or_tuple_or_blob_arg(typval_T *args, पूर्णाङ्क idx)
{
    यदि (args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_list_or_tuple_or_blob_required_for_argument_nr), idx + 1);
	फर्काऊ FAIL;
    }
    फर्काऊ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list, a tuple or a
 * dict
 */
    पूर्णाङ्क
check_for_list_or_tuple_or_dict_arg(typval_T *args, पूर्णाङ्क idx)
{
    यदि (args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_list_or_tuple_or_dict_required_for_argument_nr), idx + 1);
	फर्काऊ FAIL;
    }
    फर्काऊ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list or dict or a
 * blob.
 */
    पूर्णाङ्क
check_for_list_or_dict_or_blob_arg(typval_T *args, पूर्णाङ्क idx)
{
    यदि (args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_DICT
	    && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_list_dict_or_blob_required_for_argument_nr), idx + 1);
	फर्काऊ FAIL;
    }
    फर्काऊ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list, a tuple, a dict,
 * a blob or a string.
 */
    पूर्णाङ्क
check_for_list_tuple_dict_blob_or_string_arg(typval_T *args, पूर्णाङ्क idx)
{
    यदि (args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_DICT
	    && args[idx].v_type != VAR_BLOB
	    && args[idx].v_type != VAR_STRING)
    {
	semsg(_(e_list_tuple_dict_blob_or_string_required_for_argument_nr), idx + 1);
	फर्काऊ FAIL;
    }
    फर्काऊ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an optional buffer
 * number or a dict.
 */
    पूर्णाङ्क
check_for_opt_buffer_or_dict_arg(typval_T *args, पूर्णाङ्क idx)
{
    यदि (args[idx].v_type != VAR_UNKNOWN
	    && args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_सङ्ख्या
	    && args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_string_required_for_argument_nr), idx + 1);
	फर्काऊ FAIL;
    }
    फर्काऊ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an object.
 */
    पूर्णाङ्क
check_for_object_arg(typval_T *args, पूर्णाङ्क idx)
{
    यदि (args[idx].v_type != VAR_OBJECT)
    {
	semsg(_(e_object_required_for_argument_nr), idx + 1);
	फर्काऊ FAIL;
    }
    फर्काऊ OK;
}

/*
 * Returns TRUE if "tv" is a type alias for a class
 */
    स्थिर पूर्णाङ्क
tv_class_alias(typval_T *tv)
{
    फर्काऊ tv->v_type == VAR_TYPEALIAS &&
			tv->vval.v_typealias->ta_type->tt_type == VAR_OBJECT;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a class
 * or class typealias.
 */
    पूर्णाङ्क
check_for_class_or_typealias_args(typval_T *args, पूर्णाङ्क idx)
{
    लागि (पूर्णाङ्क i = idx; args[i].v_type != VAR_UNKNOWN; ++i)
    {
	यदि (args[i].v_type != VAR_CLASS && !tv_class_alias(&args[idx]))
	{
	    semsg(_(e_class_or_typealias_required_for_argument_nr), i + 1);
	    फर्काऊ FAIL;
	}
    }
    फर्काऊ OK;
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

    फर्काऊ tv_get_string_buf(varp, mybuf);
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

    फर्काऊ res != रिक्त ? res : (char_u *)"";
}

    char_u *
tv_get_string_buf(typval_T *varp, char_u *buf)
{
    char_u	*res = tv_get_string_buf_chk(varp, buf);

    फर्काऊ res != रिक्त ? res : (char_u *)"";
}

/*
 * Careful: This uses a single, static buffer.  YOU CAN ONLY USE IT ONCE!
 */
    char_u *
tv_get_string_chk(typval_T *varp)
{
    स्थिर char_u   mybuf[NUMBUFLEN];

    फर्काऊ tv_get_string_buf_chk(varp, mybuf);
}

    char_u *
tv_get_string_buf_chk(typval_T *varp, char_u *buf)
{
    फर्काऊ tv_get_string_buf_chk_strict(varp, buf, FALSE);
}

    char_u *
tv_get_string_buf_chk_strict(typval_T *varp, char_u *buf, पूर्णाङ्क strict)
{
    विकल्प (varp->v_type)
    {
	पक्ष VAR_सङ्ख्या:
	    यदि (strict)
	    {
		emsg(_(e_using_सङ्ख्या_as_string));
		रोक;
	    }
	    vim_snprintf((वर्ण *)buf, NUMBUFLEN, "%lld",
					    (varnumber_T)varp->vval.v_सङ्ख्या);
	    फर्काऊ buf;
	पक्ष VAR_FUNC:
	पक्ष VAR_PARTIAL:
	    emsg(_(e_using_funcref_as_string));
	    रोक;
	पक्ष VAR_LIST:
	    emsg(_(e_using_list_as_string));
	    रोक;
	पक्ष VAR_TUPLE:
	    emsg(_(e_using_tuple_as_string));
	    रोक;
	पक्ष VAR_DICT:
	    emsg(_(e_using_dictionary_as_string));
	    रोक;
	पक्ष VAR_FLOAT:
	    यदि (strict)
	    {
		emsg(_(e_using_float_as_string));
		रोक;
	    }
	    vim_snprintf((वर्ण *)buf, NUMBUFLEN, "%g", varp->vval.v_float);
	    फर्काऊ buf;
	पक्ष VAR_STRING:
	    यदि (varp->vval.v_string != रिक्त)
		फर्काऊ varp->vval.v_string;
	    फर्काऊ (char_u *)"";
	पक्ष VAR_BOOL:
	पक्ष VAR_SPECIAL:
	    STRCPY(buf, get_var_special_name(varp->vval.v_सङ्ख्या));
	    फर्काऊ buf;
	पक्ष VAR_BLOB:
	    emsg(_(e_using_blob_as_string));
	    रोक;
	पक्ष VAR_CLASS:
	पक्ष VAR_TYPEALIAS:
	    check_typval_is_मान(varp);
	    रोक;
	पक्ष VAR_OBJECT:
	    {
		यदि (varp->vval.v_object == रिक्त)
		    emsg(_(e_using_object_as_string));
		अन्यथा
		{
		    class_T *cl = varp->vval.v_object->obj_class;
		    यदि (cl != रिक्त && IS_ENUM(cl))
			semsg(_(e_using_enum_str_as_string), cl->class_name.string);
		    अन्यथा
			emsg(_(e_using_object_as_string));
		}
	    }
	    रोक;
	पक्ष VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    यदि (in_vim9script())
	    {
		semsg(_(e_using_invalid_मान_as_string_str), "job");
		रोक;
	    }
	    फर्काऊ job_to_string_buf(varp, buf);
#endif
	    रोक;
	पक्ष VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    यदि (in_vim9script())
	    {
		semsg(_(e_using_invalid_मान_as_string_str), "channel");
		रोक;
	    }
	    फर्काऊ channel_to_string_buf(varp, buf);
#endif
	    रोक;
	पक्ष VAR_VOID:
	    emsg(_(e_cannot_use_void_मान));
	    रोक;
	पक्ष VAR_UNKNOWN:
	पक्ष VAR_ANY:
	पक्ष VAR_INSTR:
	    semsg(_(e_using_invalid_मान_as_string_str),
						  vartype_name(varp->v_type));
	    रोक;
    }
    फर्काऊ रिक्त;
}

/*
 * Turn a typeval into a string.  Similar to tv_get_string_buf() but uses
 * string() on Dict, List, etc.
 */
    char_u *
tv_stringify(typval_T *varp, char_u *buf)
{
    यदि (varp->v_type == VAR_LIST
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
	फर्काऊ tmp.vval.v_string;
    }
    फर्काऊ tv_get_string_buf(varp, buf);
}

/*
 * Return TRUE if typeval "tv" and its value are set to be locked (immutable).
 * Also give an error message, using "name" or _("name") when use_gettext is
 * TRUE.
 */
    पूर्णाङ्क
tv_check_lock(typval_T *tv, char_u *name, पूर्णाङ्क use_gettext)
{
    पूर्णाङ्क	lock = 0;

    विकल्प (tv->v_type)
    {
	पक्ष VAR_BLOB:
	    यदि (tv->vval.v_blob != रिक्त)
		lock = tv->vval.v_blob->bv_lock;
	    रोक;
	पक्ष VAR_LIST:
	    यदि (tv->vval.v_list != रिक्त)
		lock = tv->vval.v_list->lv_lock;
	    रोक;
	पक्ष VAR_TUPLE:
	    यदि (tv->vval.v_tuple != रिक्त)
		lock = tv->vval.v_tuple->tv_lock;
	    रोक;
	पक्ष VAR_DICT:
	    यदि (tv->vval.v_dict != रिक्त)
		lock = tv->vval.v_dict->dv_lock;
	    रोक;
	स्वभाव:
	    रोक;
    }
    फर्काऊ मान_check_lock(tv->v_lock, name, use_gettext)
		   || (lock != 0 && मान_check_lock(lock, name, use_gettext));
}

/*
 * Copy the values from typval_T "from" to typval_T "to".
 * When needed allocates string or increases reference count.
 * Does not make a copy of a list, blob or dict but copies the reference!
 * It is OK for "from" and "to" to point to the same item.  This is used to
 * make a copy later.
 */
    पूर्णाङ्क
copy_tv(typval_T *from, typval_T *to)
{
    पूर्णाङ्क		ret = OK;

    to->v_type = from->v_type;
    to->v_lock = 0;
    विकल्प (from->v_type)
    {
	पक्ष VAR_सङ्ख्या:
	पक्ष VAR_BOOL:
	पक्ष VAR_SPECIAL:
	    to->vval.v_सङ्ख्या = from->vval.v_सङ्ख्या;
	    रोक;
	पक्ष VAR_FLOAT:
	    to->vval.v_float = from->vval.v_float;
	    रोक;
	पक्ष VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    to->vval.v_job = from->vval.v_job;
	    यदि (to->vval.v_job != रिक्त)
		++to->vval.v_job->jv_refcount;
	    रोक;
#endif
	पक्ष VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    to->vval.v_channel = from->vval.v_channel;
	    यदि (to->vval.v_channel != रिक्त)
		++to->vval.v_channel->ch_refcount;
	    रोक;
#endif
	पक्ष VAR_INSTR:
	    to->vval.v_instr = from->vval.v_instr;
	    रोक;

	पक्ष VAR_CLASS:
	    copy_class(from, to);
	    रोक;

	पक्ष VAR_OBJECT:
	    copy_object(from, to);
	    रोक;

	पक्ष VAR_STRING:
	पक्ष VAR_FUNC:
	    यदि (from->vval.v_string == रिक्त)
		to->vval.v_string = रिक्त;
	    अन्यथा
	    {
		to->vval.v_string = vim_strsave(from->vval.v_string);
		यदि (from->v_type == VAR_FUNC)
		    func_ref(to->vval.v_string);
	    }
	    रोक;
	पक्ष VAR_PARTIAL:
	    यदि (from->vval.v_partial == रिक्त)
		to->vval.v_partial = रिक्त;
	    अन्यथा
	    {
		to->vval.v_partial = from->vval.v_partial;
		++to->vval.v_partial->pt_refcount;
	    }
	    रोक;
	पक्ष VAR_BLOB:
	    यदि (from->vval.v_blob == रिक्त)
		to->vval.v_blob = रिक्त;
	    अन्यथा
	    {
		to->vval.v_blob = from->vval.v_blob;
		++to->vval.v_blob->bv_refcount;
	    }
	    रोक;
	पक्ष VAR_LIST:
	    यदि (from->vval.v_list == रिक्त)
		to->vval.v_list = रिक्त;
	    अन्यथा
	    {
		to->vval.v_list = from->vval.v_list;
		++to->vval.v_list->lv_refcount;
	    }
	    रोक;
	पक्ष VAR_TUPLE:
	    यदि (from->vval.v_tuple == रिक्त)
		to->vval.v_tuple = रिक्त;
	    अन्यथा
	    {
		to->vval.v_tuple = from->vval.v_tuple;
		++to->vval.v_tuple->tv_refcount;
	    }
	    रोक;
	पक्ष VAR_DICT:
	    यदि (from->vval.v_dict == रिक्त)
		to->vval.v_dict = रिक्त;
	    अन्यथा
	    {
		to->vval.v_dict = from->vval.v_dict;
		++to->vval.v_dict->dv_refcount;
	    }
	    रोक;
	पक्ष VAR_TYPEALIAS:
	    यदि (from->vval.v_typealias == रिक्त)
		to->vval.v_typealias = रिक्त;
	    अन्यथा
	    {
		to->vval.v_typealias = from->vval.v_typealias;
		++to->vval.v_typealias->ta_refcount;
	    }
	    रोक;
	पक्ष VAR_VOID:
	    emsg(_(e_cannot_use_void_मान));
	    ret = FAIL;
	    रोक;
	पक्ष VAR_UNKNOWN:
	पक्ष VAR_ANY:
	    internal_error_no_abort("copy_tv(UNKNOWN)");
	    ret = FAIL;
	    रोक;
    }

    फर्काऊ ret;
}

/*
 * Compare "tv1" and "tv2".
 */
    पूर्णाङ्क
typval_compare2(
    typval_T	*tv1,	// first operand
    typval_T	*tv2,	// second operand
    exprtype_T	type,	// operator
    पूर्णाङ्क		ic,	// ignore case
    पूर्णाङ्क		*res)	// comparison result
{
    varnumber_T	n1, n2;
    पूर्णाङ्क		type_is = type == अभिव्यक्ति_IS || type == अभिव्यक्ति_ISNOT;

    यदि (check_typval_is_मान(tv1) == FAIL
	|| check_typval_is_मान(tv2) == FAIL)
	फर्काऊ FAIL;
    अन्यथा यदि (type_is && tv1->v_type != tv2->v_type)
    {
	// For "is" a different type always means FALSE, for "isnot"
	// it means TRUE.
	*res = (type == अभिव्यक्ति_ISNOT);
    }
    अन्यथा यदि (((tv1->v_type == VAR_SPECIAL && tv1->vval.v_सङ्ख्या == VVAL_NULL)
		|| (tv2->v_type == VAR_SPECIAL
					   && tv2->vval.v_सङ्ख्या == VVAL_NULL))
	    && tv1->v_type != tv2->v_type
	    && (type == अभिव्यक्ति_EQUAL || type == अभिव्यक्ति_NEQUAL))
    {
	n1 = typval_compare_null(tv1, tv2);
	यदि (n1 == MAYBE)
	    फर्काऊ FAIL;
	यदि (type == अभिव्यक्ति_NEQUAL)
	    n1 = !n1;
	*res = n1;
    }
    अन्यथा यदि (tv1->v_type == VAR_BLOB || tv2->v_type == VAR_BLOB)
    {
	यदि (typval_compare_blob(tv1, tv2, type, res) == FAIL)
	    फर्काऊ FAIL;
    }
    अन्यथा यदि (tv1->v_type == VAR_LIST || tv2->v_type == VAR_LIST)
    {
	यदि (typval_compare_list(tv1, tv2, type, ic, res) == FAIL)
	    फर्काऊ FAIL;
    }
    अन्यथा यदि (tv1->v_type == VAR_TUPLE || tv2->v_type == VAR_TUPLE)
    {
	यदि (typval_compare_tuple(tv1, tv2, type, ic, res) == FAIL)
	    फर्काऊ FAIL;
    }
    अन्यथा यदि (tv1->v_type == VAR_OBJECT || tv2->v_type == VAR_OBJECT)
    {
	यदि (typval_compare_object(tv1, tv2, type, ic, res) == FAIL)
	    फर्काऊ FAIL;
    }
    अन्यथा यदि (tv1->v_type == VAR_DICT || tv2->v_type == VAR_DICT)
    {
	यदि (typval_compare_dict(tv1, tv2, type, ic, res) == FAIL)
	    फर्काऊ FAIL;
    }
    अन्यथा यदि (tv1->v_type == VAR_FUNC || tv2->v_type == VAR_FUNC
	|| tv1->v_type == VAR_PARTIAL || tv2->v_type == VAR_PARTIAL)
    {
	यदि (typval_compare_func(tv1, tv2, type, ic, res) == FAIL)
	    फर्काऊ FAIL;
    }

    // If one of the two variables is a float, compare as a float.
    // When using "=~" or "!~", always compare as string.
    अन्यथा यदि ((tv1->v_type == VAR_FLOAT || tv2->v_type == VAR_FLOAT)
	    && type != अभिव्यक्ति_MATCH && type != अभिव्यक्ति_NOMATCH)
    {
	float_T f1, f2;
	पूर्णाङ्क	error = FALSE;

	f1 = tv_get_float_chk(tv1, &error);
	यदि (!error)
	    f2 = tv_get_float_chk(tv2, &error);
	यदि (error)
	    फर्काऊ FAIL;
	n1 = FALSE;
	विकल्प (type)
	{
	    पक्ष अभिव्यक्ति_IS:
	    पक्ष अभिव्यक्ति_EQUAL:    n1 = (f1 == f2); रोक;
	    पक्ष अभिव्यक्ति_ISNOT:
	    पक्ष अभिव्यक्ति_NEQUAL:   n1 = (f1 != f2); रोक;
	    पक्ष अभिव्यक्ति_GREATER:  n1 = (f1 > f2); रोक;
	    पक्ष अभिव्यक्ति_GEQUAL:   n1 = (f1 >= f2); रोक;
	    पक्ष अभिव्यक्ति_SMALLER:  n1 = (f1 < f2); रोक;
	    पक्ष अभिव्यक्ति_SEQUAL:   n1 = (f1 <= f2); रोक;
	    पक्ष अभिव्यक्ति_UNKNOWN:
	    पक्ष अभिव्यक्ति_MATCH:
	    स्वभाव:  रोक;  // avoid gcc warning
	}
	*res = n1;
    }

    // If one of the two variables is a number, compare as a number.
    // When using "=~" or "!~", always compare as string.
    अन्यथा यदि ((tv1->v_type == VAR_सङ्ख्या || tv2->v_type == VAR_सङ्ख्या)
	    && type != अभिव्यक्ति_MATCH && type != अभिव्यक्ति_NOMATCH)
    {
	पूर्णाङ्क error = FALSE;

	n1 = tv_get_सङ्ख्या_chk(tv1, &error);
	यदि (!error)
	    n2 = tv_get_सङ्ख्या_chk(tv2, &error);
	यदि (error)
	    फर्काऊ FAIL;
	विकल्प (type)
	{
	    पक्ष अभिव्यक्ति_IS:
	    पक्ष अभिव्यक्ति_EQUAL:    n1 = (n1 == n2); रोक;
	    पक्ष अभिव्यक्ति_ISNOT:
	    पक्ष अभिव्यक्ति_NEQUAL:   n1 = (n1 != n2); रोक;
	    पक्ष अभिव्यक्ति_GREATER:  n1 = (n1 > n2); रोक;
	    पक्ष अभिव्यक्ति_GEQUAL:   n1 = (n1 >= n2); रोक;
	    पक्ष अभिव्यक्ति_SMALLER:  n1 = (n1 < n2); रोक;
	    पक्ष अभिव्यक्ति_SEQUAL:   n1 = (n1 <= n2); रोक;
	    पक्ष अभिव्यक्ति_UNKNOWN:
	    पक्ष अभिव्यक्ति_MATCH:
	    स्वभाव:  रोक;  // avoid gcc warning
	}
	*res = n1;
    }
    अन्यथा यदि (in_vim9script() && (tv1->v_type == VAR_BOOL
				    || tv2->v_type == VAR_BOOL
				    || (tv1->v_type == VAR_SPECIAL
					      && tv2->v_type == VAR_SPECIAL)))
    {
	यदि (tv1->v_type != tv2->v_type)
	{
	    semsg(_(e_cannot_compare_str_with_str),
		       vartype_name(tv1->v_type), vartype_name(tv2->v_type));
	    फर्काऊ FAIL;
	}
	n1 = tv1->vval.v_सङ्ख्या;
	n2 = tv2->vval.v_सङ्ख्या;
	विकल्प (type)
	{
	    पक्ष अभिव्यक्ति_IS:
	    पक्ष अभिव्यक्ति_EQUAL:    n1 = (n1 == n2); रोक;
	    पक्ष अभिव्यक्ति_ISNOT:
	    पक्ष अभिव्यक्ति_NEQUAL:   n1 = (n1 != n2); रोक;
	    स्वभाव:
		semsg(_(e_invalid_operation_for_str),
						   vartype_name(tv1->v_type));
		फर्काऊ FAIL;
	}
	*res = n1;
    }
#ifdef FEAT_JOB_CHANNEL
    अन्यथा यदि (tv1->v_type == tv2->v_type
	    && (tv1->v_type == VAR_CHANNEL || tv1->v_type == VAR_JOB)
	    && (type == अभिव्यक्ति_NEQUAL || type == अभिव्यक्ति_EQUAL))
    {
	यदि (tv1->v_type == VAR_CHANNEL)
	    n1 = tv1->vval.v_channel == tv2->vval.v_channel;
	अन्यथा
	    n1 = tv1->vval.v_job == tv2->vval.v_job;
	यदि (type == अभिव्यक्ति_NEQUAL)
	    n1 = !n1;
	*res = n1;
    }
#endif
    अन्यथा
    {
	यदि (typval_compare_string(tv1, tv2, type, ic, res) == FAIL)
	    फर्काऊ FAIL;
    }

    फर्काऊ OK;
}

/*
 * Compare "tv1" and "tv2".
 * Put the result in "tv1".  Caller should clear "tv2".
 */
    पूर्णाङ्क
typval_compare(
    typval_T	*tv1,	// first operand
    typval_T	*tv2,	// second operand
    exprtype_T	type,   // operator
    पूर्णाङ्क		ic)     // ignore case
{
    पूर्णाङ्क res;

    यदि (typval_compare2(tv1, tv2, type, ic, &res) == FAIL)
    {
	clear_tv(tv1);
	फर्काऊ FAIL;
    }

    clear_tv(tv1);
    यदि (in_vim9script())
    {
	tv1->v_type = VAR_BOOL;
	tv1->vval.v_सङ्ख्या = res ? VVAL_TRUE : VVAL_FALSE;
    }
    अन्यथा
    {
	tv1->v_type = VAR_सङ्ख्या;
	tv1->vval.v_सङ्ख्या = res;
    }

    फर्काऊ OK;
}

/*
 * Compare "tv1" to "tv2" as lists according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    पूर्णाङ्क
typval_compare_list(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	पूर्णाङ्क	    ic,
	पूर्णाङ्क	    *res)
{
    पूर्णाङ्क	    val = 0;

    यदि (type == अभिव्यक्ति_IS || type == अभिव्यक्ति_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
				      && tv1->vval.v_list == tv2->vval.v_list);
	यदि (type == अभिव्यक्ति_ISNOT)
	    val = !val;
    }
    अन्यथा यदि (tv1->v_type != tv2->v_type
	    || (type != अभिव्यक्ति_EQUAL && type != अभिव्यक्ति_NEQUAL))
    {
	यदि (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_list_with_list));
	अन्यथा
	    emsg(_(e_invalid_operation_for_list));
	फर्काऊ FAIL;
    }
    अन्यथा
    {
	val = list_equal(tv1->vval.v_list, tv2->vval.v_list, ic);
	यदि (type == अभिव्यक्ति_NEQUAL)
	    val = !val;
    }
    *res = val;
    फर्काऊ OK;
}

/*
 * Compare "tv1" to "tv2" as tuples according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    पूर्णाङ्क
typval_compare_tuple(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	पूर्णाङ्क	    ic,
	पूर्णाङ्क	    *res)
{
    पूर्णाङ्क	    val = 0;

    यदि (type == अभिव्यक्ति_IS || type == अभिव्यक्ति_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
			      && tv1->vval.v_tuple == tv2->vval.v_tuple);
	यदि (type == अभिव्यक्ति_ISNOT)
	    val = !val;
    }
    अन्यथा यदि (tv1->v_type != tv2->v_type
	    || (type != अभिव्यक्ति_EQUAL && type != अभिव्यक्ति_NEQUAL))
    {
	यदि (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_tuple_with_tuple));
	अन्यथा
	    emsg(_(e_invalid_operation_for_tuple));
	फर्काऊ FAIL;
    }
    अन्यथा
    {
	val = tuple_equal(tv1->vval.v_tuple, tv2->vval.v_tuple, ic);
	यदि (type == अभिव्यक्ति_NEQUAL)
	    val = !val;
    }
    *res = val;
    फर्काऊ OK;
}

/*
 * Compare v:null with another type.  Return TRUE if the value is NULL.
 */
    पूर्णाङ्क
typval_compare_null(typval_T *tv1, typval_T *tv2)
{
    यदि ((tv1->v_type == VAR_SPECIAL && tv1->vval.v_सङ्ख्या == VVAL_NULL)
	    || (tv2->v_type == VAR_SPECIAL && tv2->vval.v_सङ्ख्या == VVAL_NULL))
    {
	typval_T	*tv = tv1->v_type == VAR_SPECIAL ? tv2 : tv1;

	विकल्प (tv->v_type)
	{
	    पक्ष VAR_BLOB: फर्काऊ tv->vval.v_blob == रिक्त;
#ifdef FEAT_JOB_CHANNEL
	    पक्ष VAR_CHANNEL: फर्काऊ tv->vval.v_channel == रिक्त;
#endif
	    // TODO: null_class handling
	    // case VAR_CLASS: return tv->vval.v_class == NULL;
	    पक्ष VAR_DICT: फर्काऊ tv->vval.v_dict == रिक्त;
	    पक्ष VAR_FUNC: फर्काऊ tv->vval.v_string == रिक्त;
#ifdef FEAT_JOB_CHANNEL
	    पक्ष VAR_JOB: फर्काऊ tv->vval.v_job == रिक्त;
#endif
	    पक्ष VAR_LIST: फर्काऊ tv->vval.v_list == रिक्त;
	    पक्ष VAR_TUPLE: फर्काऊ tv->vval.v_tuple == रिक्त;
	    पक्ष VAR_OBJECT: फर्काऊ tv->vval.v_object == रिक्त;
	    पक्ष VAR_PARTIAL: फर्काऊ tv->vval.v_partial == रिक्त;
	    पक्ष VAR_STRING: फर्काऊ tv->vval.v_string == रिक्त;

	    पक्ष VAR_सङ्ख्या: यदि (!in_vim9script())
				 फर्काऊ tv->vval.v_सङ्ख्या == 0;
			     रोक;
	    पक्ष VAR_FLOAT: यदि (!in_vim9script())
				 फर्काऊ tv->vval.v_float == 0.0;
			     रोक;
	    पक्ष VAR_TYPEALIAS: फर्काऊ tv->vval.v_typealias == रिक्त;
	    स्वभाव: रोक;
	}
    }
    // although comparing null with number, float or bool is not very useful
    // we won't give an error
    फर्काऊ FALSE;
}

/*
 * Compare "tv1" to "tv2" as blobs according to "type".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    पूर्णाङ्क
typval_compare_blob(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	पूर्णाङ्क	    *res)
{
    पूर्णाङ्क	    val = 0;

    यदि (type == अभिव्यक्ति_IS || type == अभिव्यक्ति_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
			&& tv1->vval.v_blob == tv2->vval.v_blob);
	यदि (type == अभिव्यक्ति_ISNOT)
	    val = !val;
    }
    अन्यथा यदि (tv1->v_type != tv2->v_type
	    || (type != अभिव्यक्ति_EQUAL && type != अभिव्यक्ति_NEQUAL))
    {
	यदि (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_blob_with_blob));
	अन्यथा
	    emsg(_(e_invalid_operation_for_blob));
	फर्काऊ FAIL;
    }
    अन्यथा
    {
	val = blob_equal(tv1->vval.v_blob, tv2->vval.v_blob);
	यदि (type == अभिव्यक्ति_NEQUAL)
	    val = !val;
    }
    *res = val;
    फर्काऊ OK;
}

/*
 * Compare "tv1" to "tv2" as objects according to "type".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    पूर्णाङ्क
typval_compare_object(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	पूर्णाङ्क	    ic,
	पूर्णाङ्क	    *res)
{
    पूर्णाङ्क res_match = type == अभिव्यक्ति_EQUAL || type == अभिव्यक्ति_IS ? TRUE : FALSE;

    यदि (tv1->vval.v_object == रिक्त && tv2->vval.v_object == रिक्त)
    {
	*res = res_match;
	फर्काऊ OK;
    }
    यदि (tv1->vval.v_object == रिक्त || tv2->vval.v_object == रिक्त)
    {
	*res = !res_match;
	फर्काऊ OK;
    }

    object_T *obj1 = tv1->vval.v_object;
    object_T *obj2 = tv2->vval.v_object;
    यदि (type == अभिव्यक्ति_IS || type == अभिव्यक्ति_ISNOT)
    {
	*res = obj1 == obj2 ? res_match : !res_match;
	फर्काऊ OK;
    }
    अन्यथा यदि (tv1->v_type != tv2->v_type)
    {
	emsg(_(e_can_only_compare_object_with_object));
	फर्काऊ FAIL;
    }

    *res = object_equal(obj1, obj2, ic) ? res_match : !res_match;
    फर्काऊ OK;
}

/*
 * Compare "tv1" to "tv2" as dictionaries according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    पूर्णाङ्क
typval_compare_dict(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	पूर्णाङ्क	    ic,
	पूर्णाङ्क	    *res)
{
    पूर्णाङ्क	    val;

    यदि (type == अभिव्यक्ति_IS || type == अभिव्यक्ति_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
			&& tv1->vval.v_dict == tv2->vval.v_dict);
	यदि (type == अभिव्यक्ति_ISNOT)
	    val = !val;
    }
    अन्यथा यदि (tv1->v_type != tv2->v_type
		|| (type != अभिव्यक्ति_EQUAL && type != अभिव्यक्ति_NEQUAL))
    {
	यदि (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_dictionary_with_dictionary));
	अन्यथा
	    emsg(_(e_invalid_operation_for_dictionary));
	फर्काऊ FAIL;
    }
    अन्यथा
    {
	val = dict_equal(tv1->vval.v_dict, tv2->vval.v_dict, ic);
	यदि (type == अभिव्यक्ति_NEQUAL)
	    val = !val;
    }
    *res = val;
    फर्काऊ OK;
}

/*
 * Compare "tv1" to "tv2" as funcrefs according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    पूर्णाङ्क
typval_compare_func(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	पूर्णाङ्क	    ic,
	पूर्णाङ्क	    *res)
{
    पूर्णाङ्क	    val = 0;

    यदि (type != अभिव्यक्ति_EQUAL && type != अभिव्यक्ति_NEQUAL
	    && type != अभिव्यक्ति_IS && type != अभिव्यक्ति_ISNOT)
    {
	emsg(_(e_invalid_operation_for_funcrefs));
	फर्काऊ FAIL;
    }
    यदि ((tv1->v_type == VAR_PARTIAL && tv1->vval.v_partial == रिक्त)
	    || (tv2->v_type == VAR_PARTIAL && tv2->vval.v_partial == रिक्त))
	// When both partials are NULL, then they are equal.
	// Otherwise they are not equal.
	val = (tv1->vval.v_partial == tv2->vval.v_partial);
    अन्यथा यदि (type == अभिव्यक्ति_IS || type == अभिव्यक्ति_ISNOT)
    {
	यदि (tv1->v_type == VAR_FUNC && tv2->v_type == VAR_FUNC)
	    // strings are considered the same if their value is
	    // the same
	    val = tv_equal(tv1, tv2, ic);
	अन्यथा यदि (tv1->v_type == VAR_PARTIAL && tv2->v_type == VAR_PARTIAL)
	    val = (tv1->vval.v_partial == tv2->vval.v_partial);
	अन्यथा
	    val = FALSE;
    }
    अन्यथा
	val = tv_equal(tv1, tv2, ic);
    यदि (type == अभिव्यक्ति_NEQUAL || type == अभिव्यक्ति_ISNOT)
	val = !val;
    *res = val;
    फर्काऊ OK;
}

/*
 * Compare "tv1" to "tv2" as strings according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    पूर्णाङ्क
typval_compare_string(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	पूर्णाङ्क	    ic,
	पूर्णाङ्क	    *res)
{
    पूर्णाङ्क		i = 0;
    पूर्णाङ्क		val = FALSE;
    char_u	*s1, *s2;
    char_u	buf1[NUMBUFLEN], buf2[NUMBUFLEN];

    यदि (in_vim9script()
	  && ((tv1->v_type != VAR_STRING && tv1->v_type != VAR_SPECIAL)
	   || (tv2->v_type != VAR_STRING && tv2->v_type != VAR_SPECIAL)))
    {
	semsg(_(e_cannot_compare_str_with_str),
		   vartype_name(tv1->v_type), vartype_name(tv2->v_type));
	फर्काऊ FAIL;
    }
    s1 = tv_get_string_buf(tv1, buf1);
    s2 = tv_get_string_buf(tv2, buf2);
    यदि (type != अभिव्यक्ति_MATCH && type != अभिव्यक्ति_NOMATCH)
	i = ic ? MB_STRICMP(s1, s2) : STRCMP(s1, s2);
    विकल्प (type)
    {
	पक्ष अभिव्यक्ति_IS:	    यदि (in_vim9script())
			    {
				// Really check it is the same string, not just
				// the same value.
				val = tv1->vval.v_string == tv2->vval.v_string;
				रोक;
			    }
			    // FALLTHROUGH
	पक्ष अभिव्यक्ति_EQUAL:    val = (i == 0); रोक;
	पक्ष अभिव्यक्ति_ISNOT:    यदि (in_vim9script())
			    {
				// Really check it is not the same string, not
				// just a different value.
				val = tv1->vval.v_string != tv2->vval.v_string;
				रोक;
			    }
			    // FALLTHROUGH
	पक्ष अभिव्यक्ति_NEQUAL:   val = (i != 0); रोक;
	पक्ष अभिव्यक्ति_GREATER:  val = (i > 0); रोक;
	पक्ष अभिव्यक्ति_GEQUAL:   val = (i >= 0); रोक;
	पक्ष अभिव्यक्ति_SMALLER:  val = (i < 0); रोक;
	पक्ष अभिव्यक्ति_SEQUAL:   val = (i <= 0); रोक;

	पक्ष अभिव्यक्ति_MATCH:
	पक्ष अभिव्यक्ति_NOMATCH:
		val = pattern_match(s2, s1, ic);
		यदि (type == अभिव्यक्ति_NOMATCH)
		    val = !val;
		रोक;

	स्वभाव:  रोक;  // avoid gcc warning
    }
    *res = val;
    फर्काऊ OK;
}
/*
 * Convert any type to a string, never give an error.
 * When "quotes" is TRUE add quotes to a string.
 * Returns an allocated string.
 */
    char_u *
typval_tostring(typval_T *arg, पूर्णाङ्क quotes)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    char_u	*ret = रिक्त;

    यदि (arg == रिक्त)
	फर्काऊ vim_strsave((char_u *)"(does not exist)");
    यदि (!quotes && arg->v_type == VAR_STRING)
    {
	ret = vim_strsave(arg->vval.v_string == रिक्त ? (char_u *)""
							 : arg->vval.v_string);
    }
    अन्यथा
    {
	ret = tv2string(arg, &tofree, numbuf, 0);
	// Make a copy if we have a value but it's not in allocated memory.
	यदि (ret != रिक्त && tofree == रिक्त)
	    ret = vim_strsave(ret);
    }
    फर्काऊ ret;
}

/*
 * Return TRUE if typeval "tv" is locked: Either that value is locked itself
 * or it refers to a List or Dictionary that is locked.
 */
    पूर्णाङ्क
tv_islocked(typval_T *tv)
{
    फर्काऊ (tv->v_lock & VAR_LOCKED)
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

    स्थिर पूर्णाङ्क
func_equal(
    typval_T *tv1,
    typval_T *tv2,
    पूर्णाङ्क	     ic)	    // ignore case
{
    char_u	*s1, *s2;
    dict_T	*d1, *d2;
    पूर्णाङ्क		a1, a2;
    पूर्णाङ्क		i;

    // empty and NULL function name considered the same
    s1 = tv1->v_type == VAR_FUNC ? tv1->vval.v_string
					   : partial_name(tv1->vval.v_partial);
    यदि (s1 != रिक्त && *s1 == NUL)
	s1 = रिक्त;
    s2 = tv2->v_type == VAR_FUNC ? tv2->vval.v_string
					   : partial_name(tv2->vval.v_partial);
    यदि (s2 != रिक्त && *s2 == NUL)
	s2 = रिक्त;
    यदि (s1 == रिक्त || s2 == रिक्त)
    {
	यदि (s1 != s2)
	    फर्काऊ FALSE;
    }
    अन्यथा यदि (STRCMP(s1, s2) != 0)
	फर्काऊ FALSE;

    // empty dict and NULL dict is different
    d1 = tv1->v_type == VAR_FUNC ? रिक्त : tv1->vval.v_partial->pt_dict;
    d2 = tv2->v_type == VAR_FUNC ? रिक्त : tv2->vval.v_partial->pt_dict;
    यदि (d1 == रिक्त || d2 == रिक्त)
    {
	यदि (d1 != d2)
	    फर्काऊ FALSE;
    }
    अन्यथा यदि (!dict_equal(d1, d2, ic))
	फर्काऊ FALSE;

    // empty list and no list considered the same
    a1 = tv1->v_type == VAR_FUNC ? 0 : tv1->vval.v_partial->pt_argc;
    a2 = tv2->v_type == VAR_FUNC ? 0 : tv2->vval.v_partial->pt_argc;
    यदि (a1 != a2)
	फर्काऊ FALSE;
    लागि (i = 0; i < a1; ++i)
	यदि (!tv_equal(tv1->vval.v_partial->pt_argv + i,
		      tv2->vval.v_partial->pt_argv + i, ic))
	    फर्काऊ FALSE;

    फर्काऊ TRUE;
}

/*
 * Return TRUE if "tv1" and "tv2" have the same value.
 * Compares the items just like "==" would compare them, but strings and
 * numbers are different.  Floats and numbers are also different.
 */
    पूर्णाङ्क
tv_equal(
    typval_T *tv1,
    typval_T *tv2,
    पूर्णाङ्क	     ic)	    // ignore case
{
    char_u	buf1[NUMBUFLEN], buf2[NUMBUFLEN];
    char_u	*s1, *s2;
    स्थिर पूर्णाङ्क  recursive_cnt = 0;	    // catch recursive loops
    पूर्णाङ्क		r;
    स्थिर पूर्णाङ्क	tv_equal_recurse_limit;

    // Catch lists and dicts that have an endless loop by limiting
    // recursiveness to a limit.  We guess they are equal then.
    // A fixed limit has the problem of still taking an awful long time.
    // Reduce the limit every time running into it. That should work fine for
    // deeply linked structures that are not recursively linked and catch
    // recursiveness quickly.
    यदि (recursive_cnt == 0)
	tv_equal_recurse_limit = 1000;
    यदि (recursive_cnt >= tv_equal_recurse_limit)
    {
	--tv_equal_recurse_limit;
	फर्काऊ TRUE;
    }

    // For VAR_FUNC and VAR_PARTIAL compare the function name, bound dict and
    // arguments.
    यदि ((tv1->v_type == VAR_FUNC
		|| (tv1->v_type == VAR_PARTIAL && tv1->vval.v_partial != रिक्त))
	    && (tv2->v_type == VAR_FUNC
		|| (tv2->v_type == VAR_PARTIAL && tv2->vval.v_partial != रिक्त)))
    {
	++recursive_cnt;
	r = func_equal(tv1, tv2, ic);
	--recursive_cnt;
	फर्काऊ r;
    }

    यदि (tv1->v_type != tv2->v_type
	    && ((tv1->v_type != VAR_BOOL && tv1->v_type != VAR_SPECIAL)
		|| (tv2->v_type != VAR_BOOL && tv2->v_type != VAR_SPECIAL)))
	फर्काऊ FALSE;

    विकल्प (tv1->v_type)
    {
	पक्ष VAR_LIST:
	    ++recursive_cnt;
	    r = list_equal(tv1->vval.v_list, tv2->vval.v_list, ic);
	    --recursive_cnt;
	    फर्काऊ r;

	पक्ष VAR_TUPLE:
	    ++recursive_cnt;
	    r = tuple_equal(tv1->vval.v_tuple, tv2->vval.v_tuple, ic);
	    --recursive_cnt;
	    फर्काऊ r;

	पक्ष VAR_DICT:
	    ++recursive_cnt;
	    r = dict_equal(tv1->vval.v_dict, tv2->vval.v_dict, ic);
	    --recursive_cnt;
	    फर्काऊ r;

	पक्ष VAR_BLOB:
	    फर्काऊ blob_equal(tv1->vval.v_blob, tv2->vval.v_blob);

	पक्ष VAR_सङ्ख्या:
	पक्ष VAR_BOOL:
	पक्ष VAR_SPECIAL:
	    फर्काऊ tv1->vval.v_सङ्ख्या == tv2->vval.v_सङ्ख्या;

	पक्ष VAR_STRING:
	    s1 = tv_get_string_buf(tv1, buf1);
	    s2 = tv_get_string_buf(tv2, buf2);
	    फर्काऊ ((ic ? MB_STRICMP(s1, s2) : STRCMP(s1, s2)) == 0);

	पक्ष VAR_FLOAT:
	    फर्काऊ tv1->vval.v_float == tv2->vval.v_float;
	पक्ष VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    फर्काऊ tv1->vval.v_job == tv2->vval.v_job;
#endif
	पक्ष VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    फर्काऊ tv1->vval.v_channel == tv2->vval.v_channel;
#endif
	पक्ष VAR_INSTR:
	    फर्काऊ tv1->vval.v_instr == tv2->vval.v_instr;

	पक्ष VAR_CLASS:
	    // A class only exists once, equality is identity.
	    फर्काऊ tv1->vval.v_class == tv2->vval.v_class;

	पक्ष VAR_OBJECT:
	    ++recursive_cnt;
	    r = object_equal(tv1->vval.v_object, tv2->vval.v_object, ic);
	    --recursive_cnt;
	    फर्काऊ r;

	पक्ष VAR_PARTIAL:
	    फर्काऊ tv1->vval.v_partial == tv2->vval.v_partial;

	पक्ष VAR_FUNC:
	    फर्काऊ tv1->vval.v_string == tv2->vval.v_string;

	पक्ष VAR_TYPEALIAS:
	    फर्काऊ tv1->vval.v_typealias == tv2->vval.v_typealias;

	पक्ष VAR_UNKNOWN:
	पक्ष VAR_ANY:
	पक्ष VAR_VOID:
	    रोक;
    }

    // VAR_UNKNOWN can be the result of a invalid expression, let's say it
    // does not equal anything, not even itself.
    फर्काऊ FALSE;
}

/*
 * Get an option value.
 * "arg" points to the '&' or '+' before the option name.
 * "arg" is advanced to character after the option name.
 * Return OK or FAIL.
 */
    पूर्णाङ्क
eval_option(
    char_u	**arg,
    typval_T	*rettv,	// when NULL, only check if option exists
    पूर्णाङ्क		evaluate)
{
    char_u	*option_end;
    दीर्घ	numval;
    char_u	*stringval;
    getoption_T	opt_type;
    पूर्णाङ्क		c;
    पूर्णाङ्क		working = (**arg == '+');    // has("+option")
    पूर्णाङ्क		ret = OK;
    पूर्णाङ्क		scope;

    // Isolate the option name and find its value.
    option_end = find_option_end(arg, &scope);
    यदि (option_end == रिक्त)
    {
	यदि (rettv != रिक्त)
	    semsg(_(e_option_name_missing_str), *arg);
	फर्काऊ FAIL;
    }

    यदि (!evaluate)
    {
	*arg = option_end;
	फर्काऊ OK;
    }

    c = *option_end;
    *option_end = NUL;
    opt_type = get_option_मान(*arg, &numval,
			       rettv == रिक्त ? रिक्त : &stringval, रिक्त, scope);

    यदि (opt_type == gov_unknown)
    {
	यदि (rettv != रिक्त)
	    semsg(_(e_unknown_option_str), *arg);
	ret = FAIL;
    }
    अन्यथा यदि (rettv != रिक्त)
    {
	rettv->v_lock = 0;
	यदि (opt_type == gov_hidden_string)
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = रिक्त;
	}
	अन्यथा यदि (opt_type == gov_hidden_bool || opt_type == gov_hidden_सङ्ख्या)
	{
	    rettv->v_type = in_vim9script() && opt_type == gov_hidden_bool
						       ? VAR_BOOL : VAR_सङ्ख्या;
	    rettv->vval.v_सङ्ख्या = 0;
	}
	अन्यथा यदि (opt_type == gov_bool || opt_type == gov_सङ्ख्या)
	{
	    यदि (in_vim9script() && opt_type == gov_bool)
	    {
		rettv->v_type = VAR_BOOL;
		rettv->vval.v_सङ्ख्या = numval ? VVAL_TRUE : VVAL_FALSE;
	    }
	    अन्यथा
	    {
		rettv->v_type = VAR_सङ्ख्या;
		rettv->vval.v_सङ्ख्या = numval;
	    }
	}
	अन्यथा				// string option
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = stringval;
	}
    }
    अन्यथा यदि (working && (opt_type == gov_hidden_bool
			|| opt_type == gov_hidden_सङ्ख्या
			|| opt_type == gov_hidden_string))
	ret = FAIL;

    *option_end = c;		    // put back for error messages
    *arg = option_end;

    फर्काऊ ret;
}

/*
 * Allocate a variable for a number constant.  Also deals with "0z" for blob.
 * Return OK or FAIL.
 */
    पूर्णाङ्क
eval_सङ्ख्या(
	char_u	    **arg,
	typval_T    *rettv,
	पूर्णाङ्क	    evaluate,
	पूर्णाङ्क	    want_string)
{
    पूर्णाङ्क		len;
    पूर्णाङ्क		skip_quotes = !in_old_script(4);
    char_u	*p;
    पूर्णाङ्क		get_float = FALSE;

    // We accept a float when the format matches
    // "[0-9]\+\.[0-9]\+\([eE][+-]\?[0-9]\+\)\?".  This is very
    // strict to avoid backwards compatibility problems.
    // With script version 2 and later the leading digit can be
    // omitted.
    // Don't look for a float after the "." operator, so that
    // ":let vers = 1.2.3" doesn't fail.
    यदि (**arg == '.')
	p = *arg;
    अन्यथा
    {
	p = *arg + 1;
	यदि (skip_quotes)
	    लागि (;;)
	    {
		यदि (*p == '\'')
		    ++p;
		यदि (!vim_isdigit(*p))
		    रोक;
		p = skipdigits(p);
	    }
	अन्यथा
	    p = skipdigits(p);
    }
    यदि (!want_string && p[0] == '.' && vim_isdigit(p[1]))
    {
	get_float = TRUE;
	p = skipdigits(p + 2);
	यदि (*p == 'e' || *p == 'E')
	{
	    ++p;
	    यदि (*p == '-' || *p == '+')
		++p;
	    यदि (!vim_isdigit(*p))
		get_float = FALSE;
	    अन्यथा
		p = skipdigits(p + 1);
	}
	यदि (ASCII_ISALPHA(*p) || *p == '.')
	    get_float = FALSE;
    }
    यदि (get_float)
    {
	float_T	f;

	*arg += string2float(*arg, &f, skip_quotes);
	यदि (evaluate)
	{
	    rettv->v_type = VAR_FLOAT;
	    rettv->vval.v_float = f;
	}
    }
    अन्यथा
    यदि (**arg == '0' && ((*arg)[1] == 'z' || (*arg)[1] == 'Z'))
    {
	char_u  *bp;
	blob_T  *blob = रिक्त;  // init for gcc

	// Blob constant: 0z0123456789abcdef
	यदि (evaluate)
	    blob = blob_alloc();
	लागि (bp = *arg + 2; vim_isxdigit(bp[0]); bp += 2)
	{
	    यदि (!vim_isxdigit(bp[1]))
	    {
		यदि (blob != रिक्त)
		{
		    emsg(_(e_blob_literal_should_have_an_even_सङ्ख्या_of_hex_characters));
		    ga_clear(&blob->bv_ga);
		    VIM_CLEAR(blob);
		}
		फर्काऊ FAIL;
	    }
	    यदि (blob != रिक्त)
		ga_append(&blob->bv_ga,
			     (hex2nr(*bp) << 4) + hex2nr(*(bp+1)));
	    यदि (bp[2] == '.' && vim_isxdigit(bp[3]))
		++bp;
	}
	यदि (blob != रिक्त)
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
	यदि (len == 0)
	{
	    यदि (evaluate)
		semsg(_(e_invalid_expression_str), *arg);
	    फर्काऊ FAIL;
	}
	*arg += len;
	यदि (evaluate)
	{
	    rettv->v_type = VAR_सङ्ख्या;
	    rettv->vval.v_सङ्ख्या = n;
	}
    }
    फर्काऊ OK;
}

/*
 * Evaluate a string constant and put the result in "rettv".
 * "*arg" points to the double quote or to after it when "interpolate" is TRUE.
 * When "interpolate" is TRUE reduce "{{" to "{", reduce "}}" to "}" and stop
 * at a single "{".
 * Return OK or FAIL.
 */
    पूर्णाङ्क
eval_string(char_u **arg, typval_T *rettv, पूर्णाङ्क evaluate, पूर्णाङ्क interpolate)
{
    char_u	*p;
    char_u	*end;
    पूर्णाङ्क		extra = interpolate ? 1 : 0;
    पूर्णाङ्क		off = interpolate ? 0 : 1;
    पूर्णाङ्क		len;

    // Find the end of the string, skipping backslashed characters.
    लागि (p = *arg + off; *p != NUL && *p != '"'; MB_PTR_ADV(p))
    {
	यदि (*p == '\\' && p[1] != NUL)
	{
	    ++p;
	    // A "\<x>" form occupies at least 4 characters, and produces up
	    // to 9 characters (6 for the char and 3 for a modifier):
	    // reserve space for 5 extra.
	    यदि (*p == '<')
	    {
		पूर्णाङ्क		modifiers = 0;
		पूर्णाङ्क		flags = FSK_KEYCODE | FSK_IN_STRING;

		extra += 5;

		// Skip to the '>' to avoid using '{' inside for string
		// interpolation.
		यदि (p[1] != '*')
		    flags |= FSK_SIMPLIFY;
		यदि (find_special_साँचो(&p, &modifiers, flags, रिक्त) != 0)
		    --p;  // leave "p" on the ">"
	    }
	}
	अन्यथा यदि (interpolate && (*p == '{' || *p == '}'))
	{
	    यदि (*p == '{' && p[1] != '{') // start of expression
		रोक;
	    ++p;
	    यदि (p[-1] == '}' && *p != '}') // single '}' is an error
	    {
		semsg(_(e_stray_closing_curly_str), *arg);
		फर्काऊ FAIL;
	    }
	    --extra;  // "{{" becomes "{", "}}" becomes "}"
	}
    }

    यदि (*p != '"' && !(interpolate && *p == '{'))
    {
	semsg(_(e_missing_double_quote_str), *arg);
	फर्काऊ FAIL;
    }

    // If only parsing, set *arg and return here
    यदि (!evaluate)
    {
	*arg = p + off;
	फर्काऊ OK;
    }

    // Copy the string into allocated memory, handling backslashed
    // characters.
    rettv->v_type = VAR_STRING;
    len = (पूर्णाङ्क)(p - *arg + extra);
    rettv->vval.v_string = alloc(len);
    यदि (rettv->vval.v_string == रिक्त)
	फर्काऊ FAIL;
    end = rettv->vval.v_string;

    लागि (p = *arg + off; *p != NUL && *p != '"'; )
    {
	यदि (*p == '\\')
	{
	    विकल्प (*++p)
	    {
		पक्ष 'b': *end++ = BS; ++p; रोक;
		पक्ष 'e': *end++ = ESC; ++p; रोक;
		पक्ष 'f': *end++ = FF; ++p; रोक;
		पक्ष 'n': *end++ = NL; ++p; रोक;
		पक्ष 'r': *end++ = CAR; ++p; रोक;
		पक्ष 't': *end++ = TAB; ++p; रोक;

		पक्ष 'X': // hex: "\x1", "\x12"
		पक्ष 'x':
		पक्ष 'u': // Unicode: "\u0023"
		पक्ष 'U':
			  यदि (vim_isxdigit(p[1]))
			  {
			      पूर्णाङ्क	n, nr;
			      पूर्णाङ्क	c = SAFE_toupper(*p);

			      यदि (c == 'X')
				  n = 2;
			      अन्यथा यदि (*p == 'u')
				  n = 4;
			      अन्यथा
				  n = 8;
			      nr = 0;
			      जबसम्म (--n >= 0 && vim_isxdigit(p[1]))
			      {
				  ++p;
				  nr = (nr << 4) + hex2nr(*p);
			      }
			      ++p;
			      // For "\u" store the number according to
			      // 'encoding'.
			      यदि (c != 'X')
				  end += (*mb_char2bytes)(nr, end);
			      अन्यथा
				  *end++ = nr;
			  }
			  रोक;

			  // octal: "\1", "\12", "\123"
		पक्ष '0':
		पक्ष '1':
		पक्ष '2':
		पक्ष '3':
		पक्ष '4':
		पक्ष '5':
		पक्ष '6':
		पक्ष '7': *end = *p++ - '0';
			  यदि (*p >= '0' && *p <= '7')
			  {
			      *end = (*end << 3) + *p++ - '0';
			      यदि (*p >= '0' && *p <= '7')
				  *end = (*end << 3) + *p++ - '0';
			  }
			  ++end;
			  रोक;

			  // Special key, e.g.: "\<C-W>"
		पक्ष '<':
			  {
			      पूर्णाङ्क flags = FSK_KEYCODE | FSK_IN_STRING;

			      यदि (p[1] != '*')
				  flags |= FSK_SIMPLIFY;
			      extra = trans_special(&p, end, flags, FALSE, रिक्त);
			      यदि (extra != 0)
			      {
				  end += extra;
				  यदि (end >= rettv->vval.v_string + len)
				      iemsg("eval_string() used more space than allocated");
				  रोक;
			      }
			  }
			  // FALLTHROUGH

		स्वभाव: MB_COPY_CHAR(p, end);
			  रोक;
	    }
	}
	अन्यथा
	{
	    यदि (interpolate && (*p == '{' || *p == '}'))
	    {
		यदि (*p == '{' && p[1] != '{') // start of expression
		    रोक;
		++p;  // reduce "{{" to "{" and "}}" to "}"
	    }
	    MB_COPY_CHAR(p, end);
	}
    }
    *end = NUL;
    यदि (*p == '"' && !interpolate)
	++p;
    *arg = p;

    फर्काऊ OK;
}

/*
 * Allocate a variable for a 'str''ing' constant.
 * When "interpolate" is TRUE reduce "{{" to "{" and stop at a single "{".
 * Return OK when a "rettv" was set to the string.
 * Return FAIL on error, "rettv" is not set.
 */
    पूर्णाङ्क
eval_lit_string(char_u **arg, typval_T *rettv, पूर्णाङ्क evaluate, पूर्णाङ्क interpolate)
{
    char_u	*p;
    char_u	*str;
    पूर्णाङ्क		reduce = interpolate ? -1 : 0;
    पूर्णाङ्क		off = interpolate ? 0 : 1;

    // Find the end of the string, skipping ''.
    लागि (p = *arg + off; *p != NUL; MB_PTR_ADV(p))
    {
	यदि (*p == '\'')
	{
	    यदि (p[1] != '\'')
		रोक;
	    ++reduce;
	    ++p;
	}
	अन्यथा यदि (interpolate)
	{
	    यदि (*p == '{')
	    {
		यदि (p[1] != '{')
		    रोक;
		++p;
		++reduce;
	    }
	    अन्यथा यदि (*p == '}')
	    {
		++p;
		यदि (*p != '}')
		{
		    semsg(_(e_stray_closing_curly_str), *arg);
		    फर्काऊ FAIL;
		}
		++reduce;
	    }
	}
    }

    यदि (*p != '\'' && !(interpolate && *p == '{'))
    {
	semsg(_(e_missing_single_quote_str), *arg);
	फर्काऊ FAIL;
    }

    // If only parsing return after setting "*arg"
    यदि (!evaluate)
    {
	*arg = p + off;
	फर्काऊ OK;
    }

    // Copy the string into allocated memory, handling '' to ' reduction and
    // any expressions.
    str = alloc((p - *arg) - reduce);
    यदि (str == रिक्त)
	फर्काऊ FAIL;
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = str;

    लागि (p = *arg + off; *p != NUL; )
    {
	यदि (*p == '\'')
	{
	    यदि (p[1] != '\'')
		रोक;
	    ++p;
	}
	अन्यथा यदि (interpolate && (*p == '{' || *p == '}'))
	{
	    यदि (*p == '{' && p[1] != '{')
		रोक;
	    ++p;
	}
	MB_COPY_CHAR(p, str);
    }
    *str = NUL;
    *arg = p + off;

    फर्काऊ OK;
}

/*
 * Evaluate a single or double quoted string possibly containing expressions.
 * "arg" points to the '$'.  The result is put in "rettv".
 * Returns OK or FAIL.
 */
    पूर्णाङ्क
eval_interp_string(char_u **arg, typval_T *rettv, पूर्णाङ्क evaluate)
{
    typval_T	tv;
    पूर्णाङ्क		ret = OK;
    पूर्णाङ्क		quote;
    garray_T	ga;
    char_u	*p;

    ga_init2(&ga, 1, 80);

    // *arg is on the '$' character, move it to the first string character.
    ++*arg;
    quote = **arg;
    ++*arg;

    लागि (;;)
    {
	// Get the string up to the matching quote or to a single '{'.
	// "arg" is advanced to either the quote or the '{'.
	यदि (quote == '"')
	    ret = eval_string(arg, &tv, evaluate, TRUE);
	अन्यथा
	    ret = eval_lit_string(arg, &tv, evaluate, TRUE);
	यदि (ret == FAIL)
	    रोक;
	यदि (evaluate)
	{
	    ga_concat(&ga, tv.vval.v_string);
	    clear_tv(&tv);
	}

	यदि (**arg != '{')
	{
	    // found terminating quote
	    ++*arg;
	    रोक;
	}
	p = eval_one_अभिव्यक्ति_in_str(*arg, &ga, evaluate);
	यदि (p == रिक्त)
	{
	    ret = FAIL;
	    रोक;
	}
	*arg = p;
    }

    rettv->v_type = VAR_STRING;
    यदि (ret == FAIL || !evaluate || ga_append(&ga, NUL) == FAIL)
    {
	ga_clear(&ga);
	rettv->vval.v_string = रिक्त;
	फर्काऊ ret;
    }

    rettv->vval.v_string = ga.ga_data;
    फर्काऊ OK;
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
    पूर्णाङ्क		copyID)
{
    फर्काऊ echo_string_core(tv, tofree, numbuf, copyID, FALSE, TRUE, FALSE);
}

/*
 * Get the value of an environment variable.
 * "arg" is pointing to the '$'.  It is advanced to after the name.
 * If the environment variable was not set, silently assume it is empty.
 * Return FAIL if the name is invalid.
 */
    पूर्णाङ्क
eval_env_var(char_u **arg, typval_T *rettv, पूर्णाङ्क evaluate)
{
    char_u	*string = रिक्त;
    पूर्णाङ्क		len;
    पूर्णाङ्क		cc;
    char_u	*name;
    पूर्णाङ्क		mustfree = FALSE;

    ++*arg;
    name = *arg;
    len = get_env_len(arg);
    यदि (evaluate)
    {
	यदि (len == 0)
	    फर्काऊ FAIL; // invalid empty name

	cc = name[len];
	name[len] = NUL;
	// first try vim_getenv(), fast for normal environment vars
	string = vim_getenv(name, &mustfree);
	यदि (string != रिक्त && *string != NUL)
	{
	    यदि (!mustfree)
		string = vim_strsave(string);
	}
	अन्यथा
	{
	    यदि (mustfree)
		vim_free(string);

	    // next try expanding things like $VIM and ${HOME}
	    string = expand_env_save(name - 1);
	    यदि (string != रिक्त && *string == '$')
		VIM_CLEAR(string);
	}
	name[len] = cc;

	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = string;
	rettv->v_lock = 0;
    }

    फर्काऊ OK;
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
    पूर्णाङ्क		did_emsg_before = did_emsg;

    यदि (argvars[0].v_type != VAR_STRING || !in_vim9script())
	lnum = (linenr_T)tv_get_सङ्ख्या_chk(&argvars[0], रिक्त);
    यदि (lnum <= 0 && did_emsg_before == did_emsg
					    && argvars[0].v_type != VAR_सङ्ख्या)
    {
	पूर्णाङ्क	fnum;
	pos_T	*fp;

	// no valid number, try using arg like line()
	fp = var2fpos(&argvars[0], TRUE, &fnum, FALSE);
	यदि (fp != रिक्त)
	    lnum = fp->lnum;
    }
    फर्काऊ lnum;
}

/*
 * Get the lnum from the first argument.
 * Also accepts "$", then "buf" is used.
 * Returns 0 on error.
 */
    linenr_T
tv_get_lnum_buf(typval_T *argvars, buf_T *buf)
{
    यदि (argvars[0].v_type == VAR_STRING
	    && argvars[0].vval.v_string != रिक्त
	    && argvars[0].vval.v_string[0] == '$'
	    && argvars[0].vval.v_string[1] == NUL
	    && buf != रिक्त)
	फर्काऊ buf->b_ml.ml_पङ्क्ति_गन्ती;
    फर्काऊ (linenr_T)tv_get_सङ्ख्या_chk(&argvars[0], रिक्त);
}

/*
 * Get buffer by number or pattern.
 */
    buf_T *
tv_get_buf(typval_T *tv, पूर्णाङ्क curtab_only)
{
    char_u	*name = tv->vval.v_string;
    buf_T	*buf;

    यदि (tv->v_type == VAR_सङ्ख्या)
	फर्काऊ buflist_findnr((पूर्णाङ्क)tv->vval.v_सङ्ख्या);
    यदि (tv->v_type != VAR_STRING)
	फर्काऊ रिक्त;
    यदि (name == रिक्त || *name == NUL)
	फर्काऊ curbuf;
    यदि (name[0] == '$' && name[1] == NUL)
	फर्काऊ lastbuf;

    buf = buflist_find_by_name(name, curtab_only);

    // If not found, try expanding the name, like done for bufexists().
    यदि (buf == रिक्त)
	buf = find_buffer(tv);

    फर्काऊ buf;
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
    यदि (buf == रिक्त
	    && tv->v_type != VAR_सङ्ख्या
	    && tv->v_type != VAR_STRING)
	// issue errmsg for type error
	(शून्य)tv_get_सङ्ख्या(tv);
    फर्काऊ buf;
}

/*
 * Check that repeating "slen" items "n" times, does not
 * overflow the int used for the resulting length.
 */
    पूर्णाङ्क
check_repeat_गन्ती(varnumber_T slen, varnumber_T n)
{
    // Use a division so that the multiplication cannot overflow.
    यदि (n <= (varnumber_T)पूर्णांक_उच्च / slen)
	फर्काऊ OK;

    वर्ण	buf[NUMBUFLEN];

    vim_snprintf(buf, परिमाण(buf), "%lld", (दीर्घ दीर्घ)n);
    semsg(_(e_val_too_large), buf);
    फर्काऊ FAIL;
}

#endif // FEAT_EVAL
