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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଯଦି defined(FEAT_EVAL)

/*
 * Allocate memory for a variable type-value, and make it empty (0 or NULL
 * value).
 */
    typval_T *
alloc_tv(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ ALLOC_CLEAR_ONE(typval_T);
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
    ଯଦି (rettv != ଖାଲି)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = s;
    }
    ଅନ୍ୟଥା
	vim_free(s);
    ଫେରନ୍ତୁ rettv;
}

/*
 * Free the memory for a variable type-value.
 */
    ଶୂନ୍ୟ
free_tv(typval_T *varp)
{
    ଯଦି (varp == ଖାଲି)
	ଫେରନ୍ତୁ;

    ବାଛ (varp->v_type)
    {
	ସ୍ଥିତି VAR_FUNC:
	    func_unref(varp->vval.v_string);
	    // FALLTHROUGH
	ସ୍ଥିତି VAR_STRING:
	    vim_free(varp->vval.v_string);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_PARTIAL:
	    partial_unref(varp->vval.v_partial);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_BLOB:
	    blob_unref(varp->vval.v_blob);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_LIST:
	    list_unref(varp->vval.v_list);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_TUPLE:
	    tuple_unref(varp->vval.v_tuple);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_DICT:
	    dict_unref(varp->vval.v_dict);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    job_unref(varp->vval.v_job);
	    ଭାଙ୍ଗ;
#endif
	ସ୍ଥିତି VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    channel_unref(varp->vval.v_channel);
	    ଭାଙ୍ଗ;
#endif
	ସ୍ଥିତି VAR_CLASS:
	    class_unref(varp->vval.v_class);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_OBJECT:
	    object_unref(varp->vval.v_object);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି VAR_TYPEALIAS:
	    typealias_unref(varp->vval.v_typealias);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି VAR_ସଂଖ୍ୟା:
	ସ୍ଥିତି VAR_FLOAT:
	ସ୍ଥିତି VAR_ANY:
	ସ୍ଥିତି VAR_UNKNOWN:
	ସ୍ଥିତି VAR_VOID:
	ସ୍ଥିତି VAR_BOOL:
	ସ୍ଥିତି VAR_SPECIAL:
	ସ୍ଥିତି VAR_INSTR:
	    ଭାଙ୍ଗ;
    }
    vim_free(varp);
}

/*
 * Free the memory for a variable value and set the value to NULL or 0.
 */
    ଶୂନ୍ୟ
clear_tv(typval_T *varp)
{
    ଯଦି (varp == ଖାଲି)
	ଫେରନ୍ତୁ;

    ବାଛ (varp->v_type)
    {
	ସ୍ଥିତି VAR_FUNC:
	    func_unref(varp->vval.v_string);
	    // FALLTHROUGH
	ସ୍ଥିତି VAR_STRING:
	    VIM_CLEAR(varp->vval.v_string);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_PARTIAL:
	    partial_unref(varp->vval.v_partial);
	    varp->vval.v_partial = ଖାଲି;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_BLOB:
	    blob_unref(varp->vval.v_blob);
	    varp->vval.v_blob = ଖାଲି;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_LIST:
	    list_unref(varp->vval.v_list);
	    varp->vval.v_list = ଖାଲି;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_TUPLE:
	    tuple_unref(varp->vval.v_tuple);
	    varp->vval.v_tuple = ଖାଲି;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_DICT:
	    dict_unref(varp->vval.v_dict);
	    varp->vval.v_dict = ଖାଲି;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_ସଂଖ୍ୟା:
	ସ୍ଥିତି VAR_BOOL:
	ସ୍ଥିତି VAR_SPECIAL:
	    varp->vval.v_ସଂଖ୍ୟା = 0;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_FLOAT:
	    varp->vval.v_float = 0.0;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    job_unref(varp->vval.v_job);
	    varp->vval.v_job = ଖାଲି;
#endif
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    channel_unref(varp->vval.v_channel);
	    varp->vval.v_channel = ଖାଲି;
#endif
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_INSTR:
	    VIM_CLEAR(varp->vval.v_instr);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_CLASS:
	    class_unref(varp->vval.v_class);
	    varp->vval.v_class = ଖାଲି;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_OBJECT:
	    object_unref(varp->vval.v_object);
	    varp->vval.v_object = ଖାଲି;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_TYPEALIAS:
	    typealias_unref(varp->vval.v_typealias);
	    varp->vval.v_typealias = ଖାଲି;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_UNKNOWN:
	ସ୍ଥିତି VAR_ANY:
	ସ୍ଥିତି VAR_VOID:
	    ଭାଙ୍ଗ;
    }
    varp->v_lock = 0;
}

/*
 * Set the value of a variable to NULL without freeing items.
 */
    ଶୂନ୍ୟ
init_tv(typval_T *varp)
{
    ଯଦି (varp != ଖାଲି)
	CLEAR_POINTER(varp);
}

    ସ୍ଥିର varnumber_T
tv_get_bool_or_ସଂଖ୍ୟା_chk(
	typval_T    *varp,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    *denote,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    want_bool,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    vim9_string_error) // in Vim9 using a string is an error
{
    varnumber_T	n = 0L;

    ବାଛ (varp->v_type)
    {
	ସ୍ଥିତି VAR_ସଂଖ୍ୟା:
	    ଯଦି (in_vim9script() && want_bool && varp->vval.v_ସଂଖ୍ୟା != 0
						   && varp->vval.v_ସଂଖ୍ୟା != 1)
	    {
		semsg(_(e_using_ସଂଖ୍ୟା_as_bool_nr), varp->vval.v_ସଂଖ୍ୟା);
		ଭାଙ୍ଗ;
	    }
	    ଫେରନ୍ତୁ varp->vval.v_ସଂଖ୍ୟା;
	ସ୍ଥିତି VAR_FLOAT:
	    emsg(_(e_using_float_as_ସଂଖ୍ୟା));
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_FUNC:
	ସ୍ଥିତି VAR_PARTIAL:
	    emsg(_(e_using_funcref_as_ସଂଖ୍ୟା));
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_STRING:
	    ଯଦି (vim9_string_error && in_vim9script())
	    {
		emsg_using_string_as(varp, !want_bool);
		ଭାଙ୍ଗ;
	    }
	    ଯଦି (varp->vval.v_string != ଖାଲି)
		vim_str2nr(varp->vval.v_string, ଖାଲି, ଖାଲି,
					 STR2NR_ALL, &n, ଖାଲି, 0, FALSE, ଖାଲି);
	    ଫେରନ୍ତୁ n;
	ସ୍ଥିତି VAR_LIST:
	    emsg(_(e_using_list_as_ସଂଖ୍ୟା));
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_TUPLE:
	    emsg(_(e_using_tuple_as_ସଂଖ୍ୟା));
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_DICT:
	    emsg(_(e_using_dictionary_as_ସଂଖ୍ୟା));
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_BOOL:
	ସ୍ଥିତି VAR_SPECIAL:
	    ଯଦି (!want_bool && in_vim9script())
	    {
		ଯଦି (varp->v_type == VAR_BOOL)
		    emsg(_(e_using_bool_as_ସଂଖ୍ୟା));
		ଅନ୍ୟଥା
		    emsg(_(e_using_special_as_ସଂଖ୍ୟା));
		ଭାଙ୍ଗ;
	    }
	    ଫେରନ୍ତୁ varp->vval.v_ସଂଖ୍ୟା == VVAL_TRUE ? 1 : 0;
	ସ୍ଥିତି VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    emsg(_(e_using_job_as_ସଂଖ୍ୟା));
	    ଭାଙ୍ଗ;
#endif
	ସ୍ଥିତି VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    emsg(_(e_using_channel_as_ସଂଖ୍ୟା));
	    ଭାଙ୍ଗ;
#endif
	ସ୍ଥିତି VAR_BLOB:
	    emsg(_(e_using_blob_as_ସଂଖ୍ୟା));
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_CLASS:
	ସ୍ଥିତି VAR_TYPEALIAS:
	    check_typval_is_ମୂଲ୍ୟ(varp);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_OBJECT:
	    {
		ଯଦି (varp->vval.v_object == ଖାଲି)
		    emsg(_(e_using_object_as_string));
		ଅନ୍ୟଥା
		{
		    class_T *cl = varp->vval.v_object->obj_class;
		    ଯଦି (cl != ଖାଲି && IS_ENUM(cl))
			semsg(_(e_using_enum_str_as_ସଂଖ୍ୟା), cl->class_name.string);
		    ଅନ୍ୟଥା
			emsg(_(e_using_object_as_ସଂଖ୍ୟା));
		}
	    }
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_VOID:
	    emsg(_(e_cannot_use_void_ମୂଲ୍ୟ));
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_UNKNOWN:
	ସ୍ଥିତି VAR_ANY:
	ସ୍ଥିତି VAR_INSTR:
	    internal_error_no_abort("tv_get_number(UNKNOWN)");
	    ଭାଙ୍ଗ;
    }
    ଯଦି (denote == ଖାଲି)		// useful for values that must be unsigned
	n = -1;
    ଅନ୍ୟଥା
	*denote = TRUE;
    ଫେରନ୍ତୁ n;
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
tv_get_ସଂଖ୍ୟା(typval_T *varp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = FALSE;

    ଫେରନ୍ତୁ tv_get_ସଂଖ୍ୟା_chk(varp, &error);	// return 0L on error
}

/*
 * Like tv_get_number() but in Vim9 script do convert a number in a string to a
 * number without giving an error.
 */
    varnumber_T
tv_to_ସଂଖ୍ୟା(typval_T *varp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = FALSE;

    ଫେରନ୍ତୁ tv_get_bool_or_ସଂଖ୍ୟା_chk(varp, &error, FALSE, FALSE);
}

    varnumber_T
tv_get_ସଂଖ୍ୟା_chk(typval_T *varp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *denote)
{
    ଫେରନ୍ତୁ tv_get_bool_or_ସଂଖ୍ୟା_chk(varp, denote, FALSE, TRUE);
}

/*
 * Get the boolean value of "varp".  This is like tv_get_number_chk(),
 * but in Vim9 script accepts Number (0 and 1) and Bool/Special.
 */
    varnumber_T
tv_get_bool(typval_T *varp)
{
    ଫେରନ୍ତୁ tv_get_bool_or_ସଂଖ୍ୟା_chk(varp, ଖାଲି, TRUE, TRUE);
}

/*
 * Get the boolean value of "varp".  This is like tv_get_number_chk(),
 * but in Vim9 script accepts Number and Bool.
 */
    varnumber_T
tv_get_bool_chk(typval_T *varp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *denote)
{
    ଫେରନ୍ତୁ tv_get_bool_or_ସଂଖ୍ୟା_chk(varp, denote, TRUE, TRUE);
}

    ସ୍ଥିର float_T
tv_get_float_chk(typval_T *varp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *error)
{
    ବାଛ (varp->v_type)
    {
	ସ୍ଥିତି VAR_ସଂଖ୍ୟା:
	    ଫେରନ୍ତୁ (float_T)(varp->vval.v_ସଂଖ୍ୟା);
	ସ୍ଥିତି VAR_FLOAT:
	    ଫେରନ୍ତୁ varp->vval.v_float;
	ସ୍ଥିତି VAR_FUNC:
	ସ୍ଥିତି VAR_PARTIAL:
	    emsg(_(e_using_funcref_as_float));
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_STRING:
	    emsg(_(e_using_string_as_float));
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_LIST:
	    emsg(_(e_using_list_as_float));
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_TUPLE:
	    emsg(_(e_using_tuple_as_float));
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_DICT:
	    emsg(_(e_using_dictionary_as_float));
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_BOOL:
	    emsg(_(e_using_boolean_ମୂଲ୍ୟ_as_float));
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_SPECIAL:
	    emsg(_(e_using_special_ମୂଲ୍ୟ_as_float));
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    emsg(_(e_using_job_as_float));
	    ଭାଙ୍ଗ;
#endif
	ସ୍ଥିତି VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    emsg(_(e_using_channel_as_float));
	    ଭାଙ୍ଗ;
#endif
	ସ୍ଥିତି VAR_BLOB:
	    emsg(_(e_using_blob_as_float));
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_CLASS:
	ସ୍ଥିତି VAR_TYPEALIAS:
	    check_typval_is_ମୂଲ୍ୟ(varp);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_OBJECT:
	    emsg(_(e_using_object_as_float));
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_VOID:
	    emsg(_(e_cannot_use_void_ମୂଲ୍ୟ));
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_UNKNOWN:
	ସ୍ଥିତି VAR_ANY:
	ସ୍ଥିତି VAR_INSTR:
	    internal_error_no_abort("tv_get_float(UNKNOWN)");
	    ଭାଙ୍ଗ;
    }
    ଯଦି (error != ଖାଲି)
	*error = TRUE;
    ଫେରନ୍ତୁ 0;
}

    float_T
tv_get_float(typval_T *varp)
{
    ଫେରନ୍ତୁ tv_get_float_chk(varp, ଖାଲି);
}

/*
 * Give an error and return FAIL unless "args[idx]" is unknown
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_for_unknown_arg(typval_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (args[idx].v_type != VAR_UNKNOWN)
    {
	semsg(_(e_too_many_arguments), idx + 1);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_for_string_arg(typval_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (args[idx].v_type != VAR_STRING)
    {
	semsg(_(e_string_required_for_argument_nr), idx + 1);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a non-empty string.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_for_nonempty_string_arg(typval_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (check_for_string_arg(args, idx) == FAIL)
	ଫେରନ୍ତୁ FAIL;
    ଯଦି (args[idx].vval.v_string == ଖାଲି || *args[idx].vval.v_string == NUL)
    {
	semsg(_(e_non_empty_string_required_for_argument_nr), idx + 1);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Check for an optional string argument at 'idx'
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_for_opt_string_arg(typval_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଫେରନ୍ତୁ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_string_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a number.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_for_ସଂଖ୍ୟା_arg(typval_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (args[idx].v_type != VAR_ସଂଖ୍ୟା)
    {
	semsg(_(e_ସଂଖ୍ୟା_required_for_argument_nr), idx + 1);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Check for an optional number argument at 'idx'
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_for_opt_ସଂଖ୍ୟା_arg(typval_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଫେରନ୍ତୁ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_ସଂଖ୍ୟା_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a float or a number.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_for_float_or_nr_arg(typval_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (args[idx].v_type != VAR_FLOAT && args[idx].v_type != VAR_ସଂଖ୍ୟା)
    {
	semsg(_(e_float_or_ସଂଖ୍ୟା_required_for_argument_nr), idx + 1);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a bool.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_for_bool_arg(typval_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (args[idx].v_type != VAR_BOOL
	    && !(args[idx].v_type == VAR_ସଂଖ୍ୟା
		&& (args[idx].vval.v_ସଂଖ୍ୟା == 0
		    || args[idx].vval.v_ସଂଖ୍ୟା == 1)))
    {
	semsg(_(e_bool_required_for_argument_nr), idx + 1);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a bool or a number.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_for_bool_or_ସଂଖ୍ୟା_arg(typval_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (args[idx].v_type != VAR_BOOL && args[idx].v_type != VAR_ସଂଖ୍ୟା)
    {
	semsg(_(e_bool_or_ସଂଖ୍ୟା_required_for_argument_nr), idx + 1);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Check for an optional bool argument at 'idx'.
 * Return FAIL if the type is wrong.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_for_opt_bool_arg(typval_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (args[idx].v_type == VAR_UNKNOWN)
	ଫେରନ୍ତୁ OK;
    ଫେରନ୍ତୁ check_for_bool_arg(args, idx);
}

/*
 * Check for an optional bool or number argument at 'idx'.
 * Return FAIL if the type is wrong.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_for_opt_bool_or_ସଂଖ୍ୟା_arg(typval_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (args[idx].v_type == VAR_UNKNOWN)
	ଫେରନ୍ତୁ OK;
    ଫେରନ୍ତୁ check_for_bool_or_ସଂଖ୍ୟା_arg(args, idx);
}

/*
 * Check for an optional bool or dict argument at 'idx'.
 * Return FAIL if the type is wrong.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_for_opt_bool_or_dict_arg(typval_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (args[idx].v_type == VAR_UNKNOWN || args[idx].v_type == VAR_DICT)
	ଫେରନ୍ତୁ OK;
    ଫେରନ୍ତୁ check_for_bool_arg(args, idx);
}

/*
 * Give an error and return FAIL unless "args[idx]" is a blob.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_for_blob_arg(typval_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_blob_required_for_argument_nr), idx + 1);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_for_list_arg(typval_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (args[idx].v_type != VAR_LIST)
    {
	semsg(_(e_list_required_for_argument_nr), idx + 1);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a non-NULL list.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_for_nonnull_list_arg(typval_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (check_for_list_arg(args, idx) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (args[idx].vval.v_list == ଖାଲି)
    {
	semsg(_(e_non_null_list_required_for_argument_nr), idx + 1);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Check for an optional list argument at 'idx'
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_for_opt_list_arg(typval_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଫେରନ୍ତୁ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_list_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a tuple.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_for_tuple_arg(typval_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (args[idx].v_type != VAR_TUPLE)
    {
	semsg(_(e_tuple_required_for_argument_nr), idx + 1);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a dict.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_for_dict_arg(typval_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_dict_required_for_argument_nr), idx + 1);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a non-NULL dict.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_for_nonnull_dict_arg(typval_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (check_for_dict_arg(args, idx) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (args[idx].vval.v_dict == ଖାଲି)
    {
	semsg(_(e_non_null_dict_required_for_argument_nr), idx + 1);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Check for an optional dict argument at 'idx'
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_for_opt_dict_arg(typval_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଫେରନ୍ତୁ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_dict_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Check for an optional non-NULL dict argument at 'idx'
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_for_opt_nonnull_dict_arg(typval_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଫେରନ୍ତୁ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_nonnull_dict_arg(args, idx) != FAIL) ? OK : FAIL;
}

#ଯଦି defined(FEAT_JOB_CHANNEL)
/*
 * Give an error and return FAIL unless "args[idx]" is a channel or a job.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_for_chan_or_job_arg(typval_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (args[idx].v_type != VAR_CHANNEL && args[idx].v_type != VAR_JOB)
    {
	semsg(_(e_chan_or_job_required_for_argument_nr), idx + 1);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an optional channel or a
 * job.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_for_opt_chan_or_job_arg(typval_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଫେରନ୍ତୁ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_chan_or_job_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a job.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_for_job_arg(typval_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (args[idx].v_type != VAR_JOB)
    {
	semsg(_(e_job_required_for_argument_nr), idx + 1);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Check for an optional job argument at 'idx'.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_for_opt_job_arg(typval_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଫେରନ୍ତୁ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_job_arg(args, idx) != FAIL) ? OK : FAIL;
}
#ଅନ୍ୟଥା
/*
 * Give an error and return FAIL unless "args[idx]" is an optional channel or a
 * job.  Used without the +channel feature, thus only VAR_UNKNOWN is accepted.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_for_opt_chan_or_job_arg(typval_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଫେରନ୍ତୁ args[idx].v_type == VAR_UNKNOWN ? OK : FAIL;
}
#endif

/*
 * Give an error and return FAIL unless "args[idx]" is a string or
 * a number.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_for_string_or_ସଂଖ୍ୟା_arg(typval_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (args[idx].v_type != VAR_STRING && args[idx].v_type != VAR_ସଂଖ୍ୟା)
    {
	semsg(_(e_string_or_ସଂଖ୍ୟା_required_for_argument_nr), idx + 1);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Check for an optional string or number argument at 'idx'.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_for_opt_string_or_ସଂଖ୍ୟା_arg(typval_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଫେରନ୍ତୁ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_string_or_ସଂଖ୍ୟା_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a buffer number.
 * Buffer number can be a number or a string.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_for_buffer_arg(typval_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଫେରନ୍ତୁ check_for_string_or_ସଂଖ୍ୟା_arg(args, idx);
}

/*
 * Check for an optional buffer argument at 'idx'
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_for_opt_buffer_arg(typval_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଫେରନ୍ତୁ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_buffer_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a line number.
 * Line number can be a number or a string.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_for_lnum_arg(typval_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଫେରନ୍ତୁ check_for_string_or_ସଂଖ୍ୟା_arg(args, idx);
}

/*
 * Check for an optional line number argument at 'idx'
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_for_opt_lnum_arg(typval_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଫେରନ୍ତୁ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_lnum_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a blob.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_for_string_or_blob_arg(typval_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (args[idx].v_type != VAR_STRING && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_string_or_blob_required_for_argument_nr), idx + 1);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a list.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_for_string_or_list_arg(typval_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (args[idx].v_type != VAR_STRING && args[idx].v_type != VAR_LIST)
    {
	semsg(_(e_string_or_list_required_for_argument_nr), idx + 1);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string, a list, a
 * tuple or a blob.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_for_string_or_list_or_tuple_or_blob_arg(typval_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_string_list_tuple_or_blob_required_for_argument_nr), idx + 1);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Check for an optional string or list argument at 'idx'
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_for_opt_string_or_list_arg(typval_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଫେରନ୍ତୁ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_string_or_list_arg(args, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a dict.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_for_string_or_dict_arg(typval_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (args[idx].v_type != VAR_STRING && args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_string_or_dict_required_for_argument_nr), idx + 1);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a number
 * or a list.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_for_string_or_ସଂଖ୍ୟା_or_list_arg(typval_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_ସଂଖ୍ୟା
	    && args[idx].v_type != VAR_LIST)
    {
	semsg(_(e_string_ସଂଖ୍ୟା_or_list_required_for_argument_nr), idx + 1);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an optional string
 * or number or a list
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_for_opt_string_or_ସଂଖ୍ୟା_or_list_arg(typval_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଫେରନ୍ତୁ (args[idx].v_type == VAR_UNKNOWN
	    || check_for_string_or_ସଂଖ୍ୟା_or_list_arg(args, idx)
							!= FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string, a number, a
 * list, a tuple or a blob.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_for_repeat_func_arg(typval_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_ସଂଖ୍ୟା
	    && args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_repeatable_type_required_for_argument_nr), idx + 1);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string, a list, a
 * tuple or a dict.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_for_string_list_tuple_or_dict_arg(typval_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_string_list_tuple_or_dict_required_for_argument_nr), idx + 1);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string
 * or a function reference.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_for_string_or_func_arg(typval_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (args[idx].v_type != VAR_PARTIAL
	    && args[idx].v_type != VAR_FUNC
	    && args[idx].v_type != VAR_STRING)
    {
	semsg(_(e_string_or_function_required_for_argument_nr), idx + 1);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list or a blob.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_for_list_or_blob_arg(typval_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (args[idx].v_type != VAR_LIST && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_list_or_blob_required_for_argument_nr), idx + 1);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list or a tuple.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_for_list_or_tuple_arg(typval_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (args[idx].v_type != VAR_LIST && args[idx].v_type != VAR_TUPLE)
    {
	semsg(_(e_list_or_tuple_required_for_argument_nr), idx + 1);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list, a tuple or a
 * blob.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_for_list_or_tuple_or_blob_arg(typval_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_list_or_tuple_or_blob_required_for_argument_nr), idx + 1);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list, a tuple or a
 * dict
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_for_list_or_tuple_or_dict_arg(typval_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_list_or_tuple_or_dict_required_for_argument_nr), idx + 1);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list or dict or a
 * blob.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_for_list_or_dict_or_blob_arg(typval_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_DICT
	    && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_list_dict_or_blob_required_for_argument_nr), idx + 1);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list, a tuple, a dict,
 * a blob or a string.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_for_list_tuple_dict_blob_or_string_arg(typval_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (args[idx].v_type != VAR_LIST
	    && args[idx].v_type != VAR_TUPLE
	    && args[idx].v_type != VAR_DICT
	    && args[idx].v_type != VAR_BLOB
	    && args[idx].v_type != VAR_STRING)
    {
	semsg(_(e_list_tuple_dict_blob_or_string_required_for_argument_nr), idx + 1);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an optional buffer
 * number or a dict.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_for_opt_buffer_or_dict_arg(typval_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (args[idx].v_type != VAR_UNKNOWN
	    && args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_ସଂଖ୍ୟା
	    && args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_string_required_for_argument_nr), idx + 1);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an object.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_for_object_arg(typval_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (args[idx].v_type != VAR_OBJECT)
    {
	semsg(_(e_object_required_for_argument_nr), idx + 1);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Returns TRUE if "tv" is a type alias for a class
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
tv_class_alias(typval_T *tv)
{
    ଫେରନ୍ତୁ tv->v_type == VAR_TYPEALIAS &&
			tv->vval.v_typealias->ta_type->tt_type == VAR_OBJECT;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a class
 * or class typealias.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_for_class_or_typealias_args(typval_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = idx; args[i].v_type != VAR_UNKNOWN; ++i)
    {
	ଯଦି (args[i].v_type != VAR_CLASS && !tv_class_alias(&args[idx]))
	{
	    semsg(_(e_class_or_typealias_required_for_argument_nr), i + 1);
	    ଫେରନ୍ତୁ FAIL;
	}
    }
    ଫେରନ୍ତୁ OK;
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
    ସ୍ଥିର char_u   mybuf[NUMBUFLEN];

    ଫେରନ୍ତୁ tv_get_string_buf(varp, mybuf);
}

/*
 * Like tv_get_string() but don't allow number to string conversion for Vim9.
 */
    char_u *
tv_get_string_strict(typval_T *varp)
{
    ସ୍ଥିର char_u   mybuf[NUMBUFLEN];
    char_u	    *res =  tv_get_string_buf_chk_strict(
						 varp, mybuf, in_vim9script());

    ଫେରନ୍ତୁ res != ଖାଲି ? res : (char_u *)"";
}

    char_u *
tv_get_string_buf(typval_T *varp, char_u *buf)
{
    char_u	*res = tv_get_string_buf_chk(varp, buf);

    ଫେରନ୍ତୁ res != ଖାଲି ? res : (char_u *)"";
}

/*
 * Careful: This uses a single, static buffer.  YOU CAN ONLY USE IT ONCE!
 */
    char_u *
tv_get_string_chk(typval_T *varp)
{
    ସ୍ଥିର char_u   mybuf[NUMBUFLEN];

    ଫେରନ୍ତୁ tv_get_string_buf_chk(varp, mybuf);
}

    char_u *
tv_get_string_buf_chk(typval_T *varp, char_u *buf)
{
    ଫେରନ୍ତୁ tv_get_string_buf_chk_strict(varp, buf, FALSE);
}

    char_u *
tv_get_string_buf_chk_strict(typval_T *varp, char_u *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା strict)
{
    ବାଛ (varp->v_type)
    {
	ସ୍ଥିତି VAR_ସଂଖ୍ୟା:
	    ଯଦି (strict)
	    {
		emsg(_(e_using_ସଂଖ୍ୟା_as_string));
		ଭାଙ୍ଗ;
	    }
	    vim_snprintf((ଅକ୍ଷର *)buf, NUMBUFLEN, "%lld",
					    (varnumber_T)varp->vval.v_ସଂଖ୍ୟା);
	    ଫେରନ୍ତୁ buf;
	ସ୍ଥିତି VAR_FUNC:
	ସ୍ଥିତି VAR_PARTIAL:
	    emsg(_(e_using_funcref_as_string));
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_LIST:
	    emsg(_(e_using_list_as_string));
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_TUPLE:
	    emsg(_(e_using_tuple_as_string));
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_DICT:
	    emsg(_(e_using_dictionary_as_string));
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_FLOAT:
	    ଯଦି (strict)
	    {
		emsg(_(e_using_float_as_string));
		ଭାଙ୍ଗ;
	    }
	    vim_snprintf((ଅକ୍ଷର *)buf, NUMBUFLEN, "%g", varp->vval.v_float);
	    ଫେରନ୍ତୁ buf;
	ସ୍ଥିତି VAR_STRING:
	    ଯଦି (varp->vval.v_string != ଖାଲି)
		ଫେରନ୍ତୁ varp->vval.v_string;
	    ଫେରନ୍ତୁ (char_u *)"";
	ସ୍ଥିତି VAR_BOOL:
	ସ୍ଥିତି VAR_SPECIAL:
	    STRCPY(buf, get_var_special_name(varp->vval.v_ସଂଖ୍ୟା));
	    ଫେରନ୍ତୁ buf;
	ସ୍ଥିତି VAR_BLOB:
	    emsg(_(e_using_blob_as_string));
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_CLASS:
	ସ୍ଥିତି VAR_TYPEALIAS:
	    check_typval_is_ମୂଲ୍ୟ(varp);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_OBJECT:
	    {
		ଯଦି (varp->vval.v_object == ଖାଲି)
		    emsg(_(e_using_object_as_string));
		ଅନ୍ୟଥା
		{
		    class_T *cl = varp->vval.v_object->obj_class;
		    ଯଦି (cl != ଖାଲି && IS_ENUM(cl))
			semsg(_(e_using_enum_str_as_string), cl->class_name.string);
		    ଅନ୍ୟଥା
			emsg(_(e_using_object_as_string));
		}
	    }
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    ଯଦି (in_vim9script())
	    {
		semsg(_(e_using_invalid_ମୂଲ୍ୟ_as_string_str), "job");
		ଭାଙ୍ଗ;
	    }
	    ଫେରନ୍ତୁ job_to_string_buf(varp, buf);
#endif
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    ଯଦି (in_vim9script())
	    {
		semsg(_(e_using_invalid_ମୂଲ୍ୟ_as_string_str), "channel");
		ଭାଙ୍ଗ;
	    }
	    ଫେରନ୍ତୁ channel_to_string_buf(varp, buf);
#endif
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_VOID:
	    emsg(_(e_cannot_use_void_ମୂଲ୍ୟ));
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_UNKNOWN:
	ସ୍ଥିତି VAR_ANY:
	ସ୍ଥିତି VAR_INSTR:
	    semsg(_(e_using_invalid_ମୂଲ୍ୟ_as_string_str),
						  vartype_name(varp->v_type));
	    ଭାଙ୍ଗ;
    }
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Turn a typeval into a string.  Similar to tv_get_string_buf() but uses
 * string() on Dict, List, etc.
 */
    char_u *
tv_stringify(typval_T *varp, char_u *buf)
{
    ଯଦି (varp->v_type == VAR_LIST
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
	ଫେରନ୍ତୁ tmp.vval.v_string;
    }
    ଫେରନ୍ତୁ tv_get_string_buf(varp, buf);
}

/*
 * Return TRUE if typeval "tv" and its value are set to be locked (immutable).
 * Also give an error message, using "name" or _("name") when use_gettext is
 * TRUE.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
tv_check_lock(typval_T *tv, char_u *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା use_gettext)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	lock = 0;

    ବାଛ (tv->v_type)
    {
	ସ୍ଥିତି VAR_BLOB:
	    ଯଦି (tv->vval.v_blob != ଖାଲି)
		lock = tv->vval.v_blob->bv_lock;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_LIST:
	    ଯଦି (tv->vval.v_list != ଖାଲି)
		lock = tv->vval.v_list->lv_lock;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_TUPLE:
	    ଯଦି (tv->vval.v_tuple != ଖାଲି)
		lock = tv->vval.v_tuple->tv_lock;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_DICT:
	    ଯଦି (tv->vval.v_dict != ଖାଲି)
		lock = tv->vval.v_dict->dv_lock;
	    ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଭାଙ୍ଗ;
    }
    ଫେରନ୍ତୁ ମୂଲ୍ୟ_check_lock(tv->v_lock, name, use_gettext)
		   || (lock != 0 && ମୂଲ୍ୟ_check_lock(lock, name, use_gettext));
}

/*
 * Copy the values from typval_T "from" to typval_T "to".
 * When needed allocates string or increases reference count.
 * Does not make a copy of a list, blob or dict but copies the reference!
 * It is OK for "from" and "to" to point to the same item.  This is used to
 * make a copy later.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
copy_tv(typval_T *from, typval_T *to)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = OK;

    to->v_type = from->v_type;
    to->v_lock = 0;
    ବାଛ (from->v_type)
    {
	ସ୍ଥିତି VAR_ସଂଖ୍ୟା:
	ସ୍ଥିତି VAR_BOOL:
	ସ୍ଥିତି VAR_SPECIAL:
	    to->vval.v_ସଂଖ୍ୟା = from->vval.v_ସଂଖ୍ୟା;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_FLOAT:
	    to->vval.v_float = from->vval.v_float;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    to->vval.v_job = from->vval.v_job;
	    ଯଦି (to->vval.v_job != ଖାଲି)
		++to->vval.v_job->jv_refcount;
	    ଭାଙ୍ଗ;
#endif
	ସ୍ଥିତି VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    to->vval.v_channel = from->vval.v_channel;
	    ଯଦି (to->vval.v_channel != ଖାଲି)
		++to->vval.v_channel->ch_refcount;
	    ଭାଙ୍ଗ;
#endif
	ସ୍ଥିତି VAR_INSTR:
	    to->vval.v_instr = from->vval.v_instr;
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି VAR_CLASS:
	    copy_class(from, to);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି VAR_OBJECT:
	    copy_object(from, to);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି VAR_STRING:
	ସ୍ଥିତି VAR_FUNC:
	    ଯଦି (from->vval.v_string == ଖାଲି)
		to->vval.v_string = ଖାଲି;
	    ଅନ୍ୟଥା
	    {
		to->vval.v_string = vim_strsave(from->vval.v_string);
		ଯଦି (from->v_type == VAR_FUNC)
		    func_ref(to->vval.v_string);
	    }
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_PARTIAL:
	    ଯଦି (from->vval.v_partial == ଖାଲି)
		to->vval.v_partial = ଖାଲି;
	    ଅନ୍ୟଥା
	    {
		to->vval.v_partial = from->vval.v_partial;
		++to->vval.v_partial->pt_refcount;
	    }
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_BLOB:
	    ଯଦି (from->vval.v_blob == ଖାଲି)
		to->vval.v_blob = ଖାଲି;
	    ଅନ୍ୟଥା
	    {
		to->vval.v_blob = from->vval.v_blob;
		++to->vval.v_blob->bv_refcount;
	    }
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_LIST:
	    ଯଦି (from->vval.v_list == ଖାଲି)
		to->vval.v_list = ଖାଲି;
	    ଅନ୍ୟଥା
	    {
		to->vval.v_list = from->vval.v_list;
		++to->vval.v_list->lv_refcount;
	    }
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_TUPLE:
	    ଯଦି (from->vval.v_tuple == ଖାଲି)
		to->vval.v_tuple = ଖାଲି;
	    ଅନ୍ୟଥା
	    {
		to->vval.v_tuple = from->vval.v_tuple;
		++to->vval.v_tuple->tv_refcount;
	    }
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_DICT:
	    ଯଦି (from->vval.v_dict == ଖାଲି)
		to->vval.v_dict = ଖାଲି;
	    ଅନ୍ୟଥା
	    {
		to->vval.v_dict = from->vval.v_dict;
		++to->vval.v_dict->dv_refcount;
	    }
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_TYPEALIAS:
	    ଯଦି (from->vval.v_typealias == ଖାଲି)
		to->vval.v_typealias = ଖାଲି;
	    ଅନ୍ୟଥା
	    {
		to->vval.v_typealias = from->vval.v_typealias;
		++to->vval.v_typealias->ta_refcount;
	    }
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_VOID:
	    emsg(_(e_cannot_use_void_ମୂଲ୍ୟ));
	    ret = FAIL;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_UNKNOWN:
	ସ୍ଥିତି VAR_ANY:
	    internal_error_no_abort("copy_tv(UNKNOWN)");
	    ret = FAIL;
	    ଭାଙ୍ଗ;
    }

    ଫେରନ୍ତୁ ret;
}

/*
 * Compare "tv1" and "tv2".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
typval_compare2(
    typval_T	*tv1,	// first operand
    typval_T	*tv2,	// second operand
    exprtype_T	type,	// operator
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ic,	// ignore case
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*res)	// comparison result
{
    varnumber_T	n1, n2;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		type_is = type == ପଦସମଷ୍ଟି_IS || type == ପଦସମଷ୍ଟି_ISNOT;

    ଯଦି (check_typval_is_ମୂଲ୍ୟ(tv1) == FAIL
	|| check_typval_is_ମୂଲ୍ୟ(tv2) == FAIL)
	ଫେରନ୍ତୁ FAIL;
    ଅନ୍ୟଥା ଯଦି (type_is && tv1->v_type != tv2->v_type)
    {
	// For "is" a different type always means FALSE, for "isnot"
	// it means TRUE.
	*res = (type == ପଦସମଷ୍ଟି_ISNOT);
    }
    ଅନ୍ୟଥା ଯଦି (((tv1->v_type == VAR_SPECIAL && tv1->vval.v_ସଂଖ୍ୟା == VVAL_NULL)
		|| (tv2->v_type == VAR_SPECIAL
					   && tv2->vval.v_ସଂଖ୍ୟା == VVAL_NULL))
	    && tv1->v_type != tv2->v_type
	    && (type == ପଦସମଷ୍ଟି_EQUAL || type == ପଦସମଷ୍ଟି_NEQUAL))
    {
	n1 = typval_compare_null(tv1, tv2);
	ଯଦି (n1 == MAYBE)
	    ଫେରନ୍ତୁ FAIL;
	ଯଦି (type == ପଦସମଷ୍ଟି_NEQUAL)
	    n1 = !n1;
	*res = n1;
    }
    ଅନ୍ୟଥା ଯଦି (tv1->v_type == VAR_BLOB || tv2->v_type == VAR_BLOB)
    {
	ଯଦି (typval_compare_blob(tv1, tv2, type, res) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }
    ଅନ୍ୟଥା ଯଦି (tv1->v_type == VAR_LIST || tv2->v_type == VAR_LIST)
    {
	ଯଦି (typval_compare_list(tv1, tv2, type, ic, res) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }
    ଅନ୍ୟଥା ଯଦି (tv1->v_type == VAR_TUPLE || tv2->v_type == VAR_TUPLE)
    {
	ଯଦି (typval_compare_tuple(tv1, tv2, type, ic, res) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }
    ଅନ୍ୟଥା ଯଦି (tv1->v_type == VAR_OBJECT || tv2->v_type == VAR_OBJECT)
    {
	ଯଦି (typval_compare_object(tv1, tv2, type, ic, res) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }
    ଅନ୍ୟଥା ଯଦି (tv1->v_type == VAR_DICT || tv2->v_type == VAR_DICT)
    {
	ଯଦି (typval_compare_dict(tv1, tv2, type, ic, res) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }
    ଅନ୍ୟଥା ଯଦି (tv1->v_type == VAR_FUNC || tv2->v_type == VAR_FUNC
	|| tv1->v_type == VAR_PARTIAL || tv2->v_type == VAR_PARTIAL)
    {
	ଯଦି (typval_compare_func(tv1, tv2, type, ic, res) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }

    // If one of the two variables is a float, compare as a float.
    // When using "=~" or "!~", always compare as string.
    ଅନ୍ୟଥା ଯଦି ((tv1->v_type == VAR_FLOAT || tv2->v_type == VAR_FLOAT)
	    && type != ପଦସମଷ୍ଟି_MATCH && type != ପଦସମଷ୍ଟି_NOMATCH)
    {
	float_T f1, f2;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	error = FALSE;

	f1 = tv_get_float_chk(tv1, &error);
	ଯଦି (!error)
	    f2 = tv_get_float_chk(tv2, &error);
	ଯଦି (error)
	    ଫେରନ୍ତୁ FAIL;
	n1 = FALSE;
	ବାଛ (type)
	{
	    ସ୍ଥିତି ପଦସମଷ୍ଟି_IS:
	    ସ୍ଥିତି ପଦସମଷ୍ଟି_EQUAL:    n1 = (f1 == f2); ଭାଙ୍ଗ;
	    ସ୍ଥିତି ପଦସମଷ୍ଟି_ISNOT:
	    ସ୍ଥିତି ପଦସମଷ୍ଟି_NEQUAL:   n1 = (f1 != f2); ଭାଙ୍ଗ;
	    ସ୍ଥିତି ପଦସମଷ୍ଟି_GREATER:  n1 = (f1 > f2); ଭାଙ୍ଗ;
	    ସ୍ଥିତି ପଦସମଷ୍ଟି_GEQUAL:   n1 = (f1 >= f2); ଭାଙ୍ଗ;
	    ସ୍ଥିତି ପଦସମଷ୍ଟି_SMALLER:  n1 = (f1 < f2); ଭାଙ୍ଗ;
	    ସ୍ଥିତି ପଦସମଷ୍ଟି_SEQUAL:   n1 = (f1 <= f2); ଭାଙ୍ଗ;
	    ସ୍ଥିତି ପଦସମଷ୍ଟି_UNKNOWN:
	    ସ୍ଥିତି ପଦସମଷ୍ଟି_MATCH:
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:  ଭାଙ୍ଗ;  // avoid gcc warning
	}
	*res = n1;
    }

    // If one of the two variables is a number, compare as a number.
    // When using "=~" or "!~", always compare as string.
    ଅନ୍ୟଥା ଯଦି ((tv1->v_type == VAR_ସଂଖ୍ୟା || tv2->v_type == VAR_ସଂଖ୍ୟା)
	    && type != ପଦସମଷ୍ଟି_MATCH && type != ପଦସମଷ୍ଟି_NOMATCH)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା error = FALSE;

	n1 = tv_get_ସଂଖ୍ୟା_chk(tv1, &error);
	ଯଦି (!error)
	    n2 = tv_get_ସଂଖ୍ୟା_chk(tv2, &error);
	ଯଦି (error)
	    ଫେରନ୍ତୁ FAIL;
	ବାଛ (type)
	{
	    ସ୍ଥିତି ପଦସମଷ୍ଟି_IS:
	    ସ୍ଥିତି ପଦସମଷ୍ଟି_EQUAL:    n1 = (n1 == n2); ଭାଙ୍ଗ;
	    ସ୍ଥିତି ପଦସମଷ୍ଟି_ISNOT:
	    ସ୍ଥିତି ପଦସମଷ୍ଟି_NEQUAL:   n1 = (n1 != n2); ଭାଙ୍ଗ;
	    ସ୍ଥିତି ପଦସମଷ୍ଟି_GREATER:  n1 = (n1 > n2); ଭାଙ୍ଗ;
	    ସ୍ଥିତି ପଦସମଷ୍ଟି_GEQUAL:   n1 = (n1 >= n2); ଭାଙ୍ଗ;
	    ସ୍ଥିତି ପଦସମଷ୍ଟି_SMALLER:  n1 = (n1 < n2); ଭାଙ୍ଗ;
	    ସ୍ଥିତି ପଦସମଷ୍ଟି_SEQUAL:   n1 = (n1 <= n2); ଭାଙ୍ଗ;
	    ସ୍ଥିତି ପଦସମଷ୍ଟି_UNKNOWN:
	    ସ୍ଥିତି ପଦସମଷ୍ଟି_MATCH:
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:  ଭାଙ୍ଗ;  // avoid gcc warning
	}
	*res = n1;
    }
    ଅନ୍ୟଥା ଯଦି (in_vim9script() && (tv1->v_type == VAR_BOOL
				    || tv2->v_type == VAR_BOOL
				    || (tv1->v_type == VAR_SPECIAL
					      && tv2->v_type == VAR_SPECIAL)))
    {
	ଯଦି (tv1->v_type != tv2->v_type)
	{
	    semsg(_(e_cannot_compare_str_with_str),
		       vartype_name(tv1->v_type), vartype_name(tv2->v_type));
	    ଫେରନ୍ତୁ FAIL;
	}
	n1 = tv1->vval.v_ସଂଖ୍ୟା;
	n2 = tv2->vval.v_ସଂଖ୍ୟା;
	ବାଛ (type)
	{
	    ସ୍ଥିତି ପଦସମଷ୍ଟି_IS:
	    ସ୍ଥିତି ପଦସମଷ୍ଟି_EQUAL:    n1 = (n1 == n2); ଭାଙ୍ଗ;
	    ସ୍ଥିତି ପଦସମଷ୍ଟି_ISNOT:
	    ସ୍ଥିତି ପଦସମଷ୍ଟି_NEQUAL:   n1 = (n1 != n2); ଭାଙ୍ଗ;
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		semsg(_(e_invalid_operation_for_str),
						   vartype_name(tv1->v_type));
		ଫେରନ୍ତୁ FAIL;
	}
	*res = n1;
    }
#ifdef FEAT_JOB_CHANNEL
    ଅନ୍ୟଥା ଯଦି (tv1->v_type == tv2->v_type
	    && (tv1->v_type == VAR_CHANNEL || tv1->v_type == VAR_JOB)
	    && (type == ପଦସମଷ୍ଟି_NEQUAL || type == ପଦସମଷ୍ଟି_EQUAL))
    {
	ଯଦି (tv1->v_type == VAR_CHANNEL)
	    n1 = tv1->vval.v_channel == tv2->vval.v_channel;
	ଅନ୍ୟଥା
	    n1 = tv1->vval.v_job == tv2->vval.v_job;
	ଯଦି (type == ପଦସମଷ୍ଟି_NEQUAL)
	    n1 = !n1;
	*res = n1;
    }
#endif
    ଅନ୍ୟଥା
    {
	ଯଦି (typval_compare_string(tv1, tv2, type, ic, res) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Compare "tv1" and "tv2".
 * Put the result in "tv1".  Caller should clear "tv2".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
typval_compare(
    typval_T	*tv1,	// first operand
    typval_T	*tv2,	// second operand
    exprtype_T	type,   // operator
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ic)     // ignore case
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା res;

    ଯଦି (typval_compare2(tv1, tv2, type, ic, &res) == FAIL)
    {
	clear_tv(tv1);
	ଫେରନ୍ତୁ FAIL;
    }

    clear_tv(tv1);
    ଯଦି (in_vim9script())
    {
	tv1->v_type = VAR_BOOL;
	tv1->vval.v_ସଂଖ୍ୟା = res ? VVAL_TRUE : VVAL_FALSE;
    }
    ଅନ୍ୟଥା
    {
	tv1->v_type = VAR_ସଂଖ୍ୟା;
	tv1->vval.v_ସଂଖ୍ୟା = res;
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Compare "tv1" to "tv2" as lists according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
typval_compare_list(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ic,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    *res)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    val = 0;

    ଯଦି (type == ପଦସମଷ୍ଟି_IS || type == ପଦସମଷ୍ଟି_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
				      && tv1->vval.v_list == tv2->vval.v_list);
	ଯଦି (type == ପଦସମଷ୍ଟି_ISNOT)
	    val = !val;
    }
    ଅନ୍ୟଥା ଯଦି (tv1->v_type != tv2->v_type
	    || (type != ପଦସମଷ୍ଟି_EQUAL && type != ପଦସମଷ୍ଟି_NEQUAL))
    {
	ଯଦି (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_list_with_list));
	ଅନ୍ୟଥା
	    emsg(_(e_invalid_operation_for_list));
	ଫେରନ୍ତୁ FAIL;
    }
    ଅନ୍ୟଥା
    {
	val = list_equal(tv1->vval.v_list, tv2->vval.v_list, ic);
	ଯଦି (type == ପଦସମଷ୍ଟି_NEQUAL)
	    val = !val;
    }
    *res = val;
    ଫେରନ୍ତୁ OK;
}

/*
 * Compare "tv1" to "tv2" as tuples according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
typval_compare_tuple(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ic,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    *res)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    val = 0;

    ଯଦି (type == ପଦସମଷ୍ଟି_IS || type == ପଦସମଷ୍ଟି_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
			      && tv1->vval.v_tuple == tv2->vval.v_tuple);
	ଯଦି (type == ପଦସମଷ୍ଟି_ISNOT)
	    val = !val;
    }
    ଅନ୍ୟଥା ଯଦି (tv1->v_type != tv2->v_type
	    || (type != ପଦସମଷ୍ଟି_EQUAL && type != ପଦସମଷ୍ଟି_NEQUAL))
    {
	ଯଦି (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_tuple_with_tuple));
	ଅନ୍ୟଥା
	    emsg(_(e_invalid_operation_for_tuple));
	ଫେରନ୍ତୁ FAIL;
    }
    ଅନ୍ୟଥା
    {
	val = tuple_equal(tv1->vval.v_tuple, tv2->vval.v_tuple, ic);
	ଯଦି (type == ପଦସମଷ୍ଟି_NEQUAL)
	    val = !val;
    }
    *res = val;
    ଫେରନ୍ତୁ OK;
}

/*
 * Compare v:null with another type.  Return TRUE if the value is NULL.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
typval_compare_null(typval_T *tv1, typval_T *tv2)
{
    ଯଦି ((tv1->v_type == VAR_SPECIAL && tv1->vval.v_ସଂଖ୍ୟା == VVAL_NULL)
	    || (tv2->v_type == VAR_SPECIAL && tv2->vval.v_ସଂଖ୍ୟା == VVAL_NULL))
    {
	typval_T	*tv = tv1->v_type == VAR_SPECIAL ? tv2 : tv1;

	ବାଛ (tv->v_type)
	{
	    ସ୍ଥିତି VAR_BLOB: ଫେରନ୍ତୁ tv->vval.v_blob == ଖାଲି;
#ifdef FEAT_JOB_CHANNEL
	    ସ୍ଥିତି VAR_CHANNEL: ଫେରନ୍ତୁ tv->vval.v_channel == ଖାଲି;
#endif
	    // TODO: null_class handling
	    // case VAR_CLASS: return tv->vval.v_class == NULL;
	    ସ୍ଥିତି VAR_DICT: ଫେରନ୍ତୁ tv->vval.v_dict == ଖାଲି;
	    ସ୍ଥିତି VAR_FUNC: ଫେରନ୍ତୁ tv->vval.v_string == ଖାଲି;
#ifdef FEAT_JOB_CHANNEL
	    ସ୍ଥିତି VAR_JOB: ଫେରନ୍ତୁ tv->vval.v_job == ଖାଲି;
#endif
	    ସ୍ଥିତି VAR_LIST: ଫେରନ୍ତୁ tv->vval.v_list == ଖାଲି;
	    ସ୍ଥିତି VAR_TUPLE: ଫେରନ୍ତୁ tv->vval.v_tuple == ଖାଲି;
	    ସ୍ଥିତି VAR_OBJECT: ଫେରନ୍ତୁ tv->vval.v_object == ଖାଲି;
	    ସ୍ଥିତି VAR_PARTIAL: ଫେରନ୍ତୁ tv->vval.v_partial == ଖାଲି;
	    ସ୍ଥିତି VAR_STRING: ଫେରନ୍ତୁ tv->vval.v_string == ଖାଲି;

	    ସ୍ଥିତି VAR_ସଂଖ୍ୟା: ଯଦି (!in_vim9script())
				 ଫେରନ୍ତୁ tv->vval.v_ସଂଖ୍ୟା == 0;
			     ଭାଙ୍ଗ;
	    ସ୍ଥିତି VAR_FLOAT: ଯଦି (!in_vim9script())
				 ଫେରନ୍ତୁ tv->vval.v_float == 0.0;
			     ଭାଙ୍ଗ;
	    ସ୍ଥିତି VAR_TYPEALIAS: ଫେରନ୍ତୁ tv->vval.v_typealias == ଖାଲି;
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ: ଭାଙ୍ଗ;
	}
    }
    // although comparing null with number, float or bool is not very useful
    // we won't give an error
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Compare "tv1" to "tv2" as blobs according to "type".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
typval_compare_blob(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    *res)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    val = 0;

    ଯଦି (type == ପଦସମଷ୍ଟି_IS || type == ପଦସମଷ୍ଟି_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
			&& tv1->vval.v_blob == tv2->vval.v_blob);
	ଯଦି (type == ପଦସମଷ୍ଟି_ISNOT)
	    val = !val;
    }
    ଅନ୍ୟଥା ଯଦି (tv1->v_type != tv2->v_type
	    || (type != ପଦସମଷ୍ଟି_EQUAL && type != ପଦସମଷ୍ଟି_NEQUAL))
    {
	ଯଦି (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_blob_with_blob));
	ଅନ୍ୟଥା
	    emsg(_(e_invalid_operation_for_blob));
	ଫେରନ୍ତୁ FAIL;
    }
    ଅନ୍ୟଥା
    {
	val = blob_equal(tv1->vval.v_blob, tv2->vval.v_blob);
	ଯଦି (type == ପଦସମଷ୍ଟି_NEQUAL)
	    val = !val;
    }
    *res = val;
    ଫେରନ୍ତୁ OK;
}

/*
 * Compare "tv1" to "tv2" as objects according to "type".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
typval_compare_object(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ic,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    *res)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା res_match = type == ପଦସମଷ୍ଟି_EQUAL || type == ପଦସମଷ୍ଟି_IS ? TRUE : FALSE;

    ଯଦି (tv1->vval.v_object == ଖାଲି && tv2->vval.v_object == ଖାଲି)
    {
	*res = res_match;
	ଫେରନ୍ତୁ OK;
    }
    ଯଦି (tv1->vval.v_object == ଖାଲି || tv2->vval.v_object == ଖାଲି)
    {
	*res = !res_match;
	ଫେରନ୍ତୁ OK;
    }

    object_T *obj1 = tv1->vval.v_object;
    object_T *obj2 = tv2->vval.v_object;
    ଯଦି (type == ପଦସମଷ୍ଟି_IS || type == ପଦସମଷ୍ଟି_ISNOT)
    {
	*res = obj1 == obj2 ? res_match : !res_match;
	ଫେରନ୍ତୁ OK;
    }
    ଅନ୍ୟଥା ଯଦି (tv1->v_type != tv2->v_type)
    {
	emsg(_(e_can_only_compare_object_with_object));
	ଫେରନ୍ତୁ FAIL;
    }

    *res = object_equal(obj1, obj2, ic) ? res_match : !res_match;
    ଫେରନ୍ତୁ OK;
}

/*
 * Compare "tv1" to "tv2" as dictionaries according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
typval_compare_dict(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ic,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    *res)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    val;

    ଯଦି (type == ପଦସମଷ୍ଟି_IS || type == ପଦସମଷ୍ଟି_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
			&& tv1->vval.v_dict == tv2->vval.v_dict);
	ଯଦି (type == ପଦସମଷ୍ଟି_ISNOT)
	    val = !val;
    }
    ଅନ୍ୟଥା ଯଦି (tv1->v_type != tv2->v_type
		|| (type != ପଦସମଷ୍ଟି_EQUAL && type != ପଦସମଷ୍ଟି_NEQUAL))
    {
	ଯଦି (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_dictionary_with_dictionary));
	ଅନ୍ୟଥା
	    emsg(_(e_invalid_operation_for_dictionary));
	ଫେରନ୍ତୁ FAIL;
    }
    ଅନ୍ୟଥା
    {
	val = dict_equal(tv1->vval.v_dict, tv2->vval.v_dict, ic);
	ଯଦି (type == ପଦସମଷ୍ଟି_NEQUAL)
	    val = !val;
    }
    *res = val;
    ଫେରନ୍ତୁ OK;
}

/*
 * Compare "tv1" to "tv2" as funcrefs according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
typval_compare_func(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ic,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    *res)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    val = 0;

    ଯଦି (type != ପଦସମଷ୍ଟି_EQUAL && type != ପଦସମଷ୍ଟି_NEQUAL
	    && type != ପଦସମଷ୍ଟି_IS && type != ପଦସମଷ୍ଟି_ISNOT)
    {
	emsg(_(e_invalid_operation_for_funcrefs));
	ଫେରନ୍ତୁ FAIL;
    }
    ଯଦି ((tv1->v_type == VAR_PARTIAL && tv1->vval.v_partial == ଖାଲି)
	    || (tv2->v_type == VAR_PARTIAL && tv2->vval.v_partial == ଖାଲି))
	// When both partials are NULL, then they are equal.
	// Otherwise they are not equal.
	val = (tv1->vval.v_partial == tv2->vval.v_partial);
    ଅନ୍ୟଥା ଯଦି (type == ପଦସମଷ୍ଟି_IS || type == ପଦସମଷ୍ଟି_ISNOT)
    {
	ଯଦି (tv1->v_type == VAR_FUNC && tv2->v_type == VAR_FUNC)
	    // strings are considered the same if their value is
	    // the same
	    val = tv_equal(tv1, tv2, ic);
	ଅନ୍ୟଥା ଯଦି (tv1->v_type == VAR_PARTIAL && tv2->v_type == VAR_PARTIAL)
	    val = (tv1->vval.v_partial == tv2->vval.v_partial);
	ଅନ୍ୟଥା
	    val = FALSE;
    }
    ଅନ୍ୟଥା
	val = tv_equal(tv1, tv2, ic);
    ଯଦି (type == ପଦସମଷ୍ଟି_NEQUAL || type == ପଦସମଷ୍ଟି_ISNOT)
	val = !val;
    *res = val;
    ଫେରନ୍ତୁ OK;
}

/*
 * Compare "tv1" to "tv2" as strings according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
typval_compare_string(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ic,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    *res)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		val = FALSE;
    char_u	*s1, *s2;
    char_u	buf1[NUMBUFLEN], buf2[NUMBUFLEN];

    ଯଦି (in_vim9script()
	  && ((tv1->v_type != VAR_STRING && tv1->v_type != VAR_SPECIAL)
	   || (tv2->v_type != VAR_STRING && tv2->v_type != VAR_SPECIAL)))
    {
	semsg(_(e_cannot_compare_str_with_str),
		   vartype_name(tv1->v_type), vartype_name(tv2->v_type));
	ଫେରନ୍ତୁ FAIL;
    }
    s1 = tv_get_string_buf(tv1, buf1);
    s2 = tv_get_string_buf(tv2, buf2);
    ଯଦି (type != ପଦସମଷ୍ଟି_MATCH && type != ପଦସମଷ୍ଟି_NOMATCH)
	i = ic ? MB_STRICMP(s1, s2) : STRCMP(s1, s2);
    ବାଛ (type)
    {
	ସ୍ଥିତି ପଦସମଷ୍ଟି_IS:	    ଯଦି (in_vim9script())
			    {
				// Really check it is the same string, not just
				// the same value.
				val = tv1->vval.v_string == tv2->vval.v_string;
				ଭାଙ୍ଗ;
			    }
			    // FALLTHROUGH
	ସ୍ଥିତି ପଦସମଷ୍ଟି_EQUAL:    val = (i == 0); ଭାଙ୍ଗ;
	ସ୍ଥିତି ପଦସମଷ୍ଟି_ISNOT:    ଯଦି (in_vim9script())
			    {
				// Really check it is not the same string, not
				// just a different value.
				val = tv1->vval.v_string != tv2->vval.v_string;
				ଭାଙ୍ଗ;
			    }
			    // FALLTHROUGH
	ସ୍ଥିତି ପଦସମଷ୍ଟି_NEQUAL:   val = (i != 0); ଭାଙ୍ଗ;
	ସ୍ଥିତି ପଦସମଷ୍ଟି_GREATER:  val = (i > 0); ଭାଙ୍ଗ;
	ସ୍ଥିତି ପଦସମଷ୍ଟି_GEQUAL:   val = (i >= 0); ଭାଙ୍ଗ;
	ସ୍ଥିତି ପଦସମଷ୍ଟି_SMALLER:  val = (i < 0); ଭାଙ୍ଗ;
	ସ୍ଥିତି ପଦସମଷ୍ଟି_SEQUAL:   val = (i <= 0); ଭାଙ୍ଗ;

	ସ୍ଥିତି ପଦସମଷ୍ଟି_MATCH:
	ସ୍ଥିତି ପଦସମଷ୍ଟି_NOMATCH:
		val = pattern_match(s2, s1, ic);
		ଯଦି (type == ପଦସମଷ୍ଟି_NOMATCH)
		    val = !val;
		ଭାଙ୍ଗ;

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:  ଭାଙ୍ଗ;  // avoid gcc warning
    }
    *res = val;
    ଫେରନ୍ତୁ OK;
}
/*
 * Convert any type to a string, never give an error.
 * When "quotes" is TRUE add quotes to a string.
 * Returns an allocated string.
 */
    char_u *
typval_tostring(typval_T *arg, ପୂର୍ଣ୍ଣସଂଖ୍ୟା quotes)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    char_u	*ret = ଖାଲି;

    ଯଦି (arg == ଖାଲି)
	ଫେରନ୍ତୁ vim_strsave((char_u *)"(does not exist)");
    ଯଦି (!quotes && arg->v_type == VAR_STRING)
    {
	ret = vim_strsave(arg->vval.v_string == ଖାଲି ? (char_u *)""
							 : arg->vval.v_string);
    }
    ଅନ୍ୟଥା
    {
	ret = tv2string(arg, &tofree, numbuf, 0);
	// Make a copy if we have a value but it's not in allocated memory.
	ଯଦି (ret != ଖାଲି && tofree == ଖାଲି)
	    ret = vim_strsave(ret);
    }
    ଫେରନ୍ତୁ ret;
}

/*
 * Return TRUE if typeval "tv" is locked: Either that value is locked itself
 * or it refers to a List or Dictionary that is locked.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
tv_islocked(typval_T *tv)
{
    ଫେରନ୍ତୁ (tv->v_lock & VAR_LOCKED)
	|| (tv->v_type == VAR_LIST
		&& tv->vval.v_list != ଖାଲି
		&& (tv->vval.v_list->lv_lock & VAR_LOCKED))
	|| (tv->v_type == VAR_TUPLE
		&& tv->vval.v_tuple != ଖାଲି
		&& (tv->vval.v_tuple->tv_lock & VAR_LOCKED))
	|| (tv->v_type == VAR_DICT
		&& tv->vval.v_dict != ଖାଲି
		&& (tv->vval.v_dict->dv_lock & VAR_LOCKED));
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
func_equal(
    typval_T *tv1,
    typval_T *tv2,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	     ic)	    // ignore case
{
    char_u	*s1, *s2;
    dict_T	*d1, *d2;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		a1, a2;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    // empty and NULL function name considered the same
    s1 = tv1->v_type == VAR_FUNC ? tv1->vval.v_string
					   : partial_name(tv1->vval.v_partial);
    ଯଦି (s1 != ଖାଲି && *s1 == NUL)
	s1 = ଖାଲି;
    s2 = tv2->v_type == VAR_FUNC ? tv2->vval.v_string
					   : partial_name(tv2->vval.v_partial);
    ଯଦି (s2 != ଖାଲି && *s2 == NUL)
	s2 = ଖାଲି;
    ଯଦି (s1 == ଖାଲି || s2 == ଖାଲି)
    {
	ଯଦି (s1 != s2)
	    ଫେରନ୍ତୁ FALSE;
    }
    ଅନ୍ୟଥା ଯଦି (STRCMP(s1, s2) != 0)
	ଫେରନ୍ତୁ FALSE;

    // empty dict and NULL dict is different
    d1 = tv1->v_type == VAR_FUNC ? ଖାଲି : tv1->vval.v_partial->pt_dict;
    d2 = tv2->v_type == VAR_FUNC ? ଖାଲି : tv2->vval.v_partial->pt_dict;
    ଯଦି (d1 == ଖାଲି || d2 == ଖାଲି)
    {
	ଯଦି (d1 != d2)
	    ଫେରନ୍ତୁ FALSE;
    }
    ଅନ୍ୟଥା ଯଦି (!dict_equal(d1, d2, ic))
	ଫେରନ୍ତୁ FALSE;

    // empty list and no list considered the same
    a1 = tv1->v_type == VAR_FUNC ? 0 : tv1->vval.v_partial->pt_argc;
    a2 = tv2->v_type == VAR_FUNC ? 0 : tv2->vval.v_partial->pt_argc;
    ଯଦି (a1 != a2)
	ଫେରନ୍ତୁ FALSE;
    ପାଇଁ (i = 0; i < a1; ++i)
	ଯଦି (!tv_equal(tv1->vval.v_partial->pt_argv + i,
		      tv2->vval.v_partial->pt_argv + i, ic))
	    ଫେରନ୍ତୁ FALSE;

    ଫେରନ୍ତୁ TRUE;
}

/*
 * Return TRUE if "tv1" and "tv2" have the same value.
 * Compares the items just like "==" would compare them, but strings and
 * numbers are different.  Floats and numbers are also different.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
tv_equal(
    typval_T *tv1,
    typval_T *tv2,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	     ic)	    // ignore case
{
    char_u	buf1[NUMBUFLEN], buf2[NUMBUFLEN];
    char_u	*s1, *s2;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା  recursive_cnt = 0;	    // catch recursive loops
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		r;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	tv_equal_recurse_limit;

    // Catch lists and dicts that have an endless loop by limiting
    // recursiveness to a limit.  We guess they are equal then.
    // A fixed limit has the problem of still taking an awful long time.
    // Reduce the limit every time running into it. That should work fine for
    // deeply linked structures that are not recursively linked and catch
    // recursiveness quickly.
    ଯଦି (recursive_cnt == 0)
	tv_equal_recurse_limit = 1000;
    ଯଦି (recursive_cnt >= tv_equal_recurse_limit)
    {
	--tv_equal_recurse_limit;
	ଫେରନ୍ତୁ TRUE;
    }

    // For VAR_FUNC and VAR_PARTIAL compare the function name, bound dict and
    // arguments.
    ଯଦି ((tv1->v_type == VAR_FUNC
		|| (tv1->v_type == VAR_PARTIAL && tv1->vval.v_partial != ଖାଲି))
	    && (tv2->v_type == VAR_FUNC
		|| (tv2->v_type == VAR_PARTIAL && tv2->vval.v_partial != ଖାଲି)))
    {
	++recursive_cnt;
	r = func_equal(tv1, tv2, ic);
	--recursive_cnt;
	ଫେରନ୍ତୁ r;
    }

    ଯଦି (tv1->v_type != tv2->v_type
	    && ((tv1->v_type != VAR_BOOL && tv1->v_type != VAR_SPECIAL)
		|| (tv2->v_type != VAR_BOOL && tv2->v_type != VAR_SPECIAL)))
	ଫେରନ୍ତୁ FALSE;

    ବାଛ (tv1->v_type)
    {
	ସ୍ଥିତି VAR_LIST:
	    ++recursive_cnt;
	    r = list_equal(tv1->vval.v_list, tv2->vval.v_list, ic);
	    --recursive_cnt;
	    ଫେରନ୍ତୁ r;

	ସ୍ଥିତି VAR_TUPLE:
	    ++recursive_cnt;
	    r = tuple_equal(tv1->vval.v_tuple, tv2->vval.v_tuple, ic);
	    --recursive_cnt;
	    ଫେରନ୍ତୁ r;

	ସ୍ଥିତି VAR_DICT:
	    ++recursive_cnt;
	    r = dict_equal(tv1->vval.v_dict, tv2->vval.v_dict, ic);
	    --recursive_cnt;
	    ଫେରନ୍ତୁ r;

	ସ୍ଥିତି VAR_BLOB:
	    ଫେରନ୍ତୁ blob_equal(tv1->vval.v_blob, tv2->vval.v_blob);

	ସ୍ଥିତି VAR_ସଂଖ୍ୟା:
	ସ୍ଥିତି VAR_BOOL:
	ସ୍ଥିତି VAR_SPECIAL:
	    ଫେରନ୍ତୁ tv1->vval.v_ସଂଖ୍ୟା == tv2->vval.v_ସଂଖ୍ୟା;

	ସ୍ଥିତି VAR_STRING:
	    s1 = tv_get_string_buf(tv1, buf1);
	    s2 = tv_get_string_buf(tv2, buf2);
	    ଫେରନ୍ତୁ ((ic ? MB_STRICMP(s1, s2) : STRCMP(s1, s2)) == 0);

	ସ୍ଥିତି VAR_FLOAT:
	    ଫେରନ୍ତୁ tv1->vval.v_float == tv2->vval.v_float;
	ସ୍ଥିତି VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    ଫେରନ୍ତୁ tv1->vval.v_job == tv2->vval.v_job;
#endif
	ସ୍ଥିତି VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    ଫେରନ୍ତୁ tv1->vval.v_channel == tv2->vval.v_channel;
#endif
	ସ୍ଥିତି VAR_INSTR:
	    ଫେରନ୍ତୁ tv1->vval.v_instr == tv2->vval.v_instr;

	ସ୍ଥିତି VAR_CLASS:
	    // A class only exists once, equality is identity.
	    ଫେରନ୍ତୁ tv1->vval.v_class == tv2->vval.v_class;

	ସ୍ଥିତି VAR_OBJECT:
	    ++recursive_cnt;
	    r = object_equal(tv1->vval.v_object, tv2->vval.v_object, ic);
	    --recursive_cnt;
	    ଫେରନ୍ତୁ r;

	ସ୍ଥିତି VAR_PARTIAL:
	    ଫେରନ୍ତୁ tv1->vval.v_partial == tv2->vval.v_partial;

	ସ୍ଥିତି VAR_FUNC:
	    ଫେରନ୍ତୁ tv1->vval.v_string == tv2->vval.v_string;

	ସ୍ଥିତି VAR_TYPEALIAS:
	    ଫେରନ୍ତୁ tv1->vval.v_typealias == tv2->vval.v_typealias;

	ସ୍ଥିତି VAR_UNKNOWN:
	ସ୍ଥିତି VAR_ANY:
	ସ୍ଥିତି VAR_VOID:
	    ଭାଙ୍ଗ;
    }

    // VAR_UNKNOWN can be the result of a invalid expression, let's say it
    // does not equal anything, not even itself.
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Get an option value.
 * "arg" points to the '&' or '+' before the option name.
 * "arg" is advanced to character after the option name.
 * Return OK or FAIL.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
eval_option(
    char_u	**arg,
    typval_T	*rettv,	// when NULL, only check if option exists
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		evaluate)
{
    char_u	*option_end;
    long	numval;
    char_u	*stringval;
    getoption_T	opt_type;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		working = (**arg == '+');    // has("+option")
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = OK;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		scope;

    // Isolate the option name and find its value.
    option_end = find_option_end(arg, &scope);
    ଯଦି (option_end == ଖାଲି)
    {
	ଯଦି (rettv != ଖାଲି)
	    semsg(_(e_option_name_missing_str), *arg);
	ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି (!evaluate)
    {
	*arg = option_end;
	ଫେରନ୍ତୁ OK;
    }

    c = *option_end;
    *option_end = NUL;
    opt_type = get_option_ମୂଲ୍ୟ(*arg, &numval,
			       rettv == ଖାଲି ? ଖାଲି : &stringval, ଖାଲି, scope);

    ଯଦି (opt_type == gov_unknown)
    {
	ଯଦି (rettv != ଖାଲି)
	    semsg(_(e_unknown_option_str), *arg);
	ret = FAIL;
    }
    ଅନ୍ୟଥା ଯଦି (rettv != ଖାଲି)
    {
	rettv->v_lock = 0;
	ଯଦି (opt_type == gov_hidden_string)
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = ଖାଲି;
	}
	ଅନ୍ୟଥା ଯଦି (opt_type == gov_hidden_bool || opt_type == gov_hidden_ସଂଖ୍ୟା)
	{
	    rettv->v_type = in_vim9script() && opt_type == gov_hidden_bool
						       ? VAR_BOOL : VAR_ସଂଖ୍ୟା;
	    rettv->vval.v_ସଂଖ୍ୟା = 0;
	}
	ଅନ୍ୟଥା ଯଦି (opt_type == gov_bool || opt_type == gov_ସଂଖ୍ୟା)
	{
	    ଯଦି (in_vim9script() && opt_type == gov_bool)
	    {
		rettv->v_type = VAR_BOOL;
		rettv->vval.v_ସଂଖ୍ୟା = numval ? VVAL_TRUE : VVAL_FALSE;
	    }
	    ଅନ୍ୟଥା
	    {
		rettv->v_type = VAR_ସଂଖ୍ୟା;
		rettv->vval.v_ସଂଖ୍ୟା = numval;
	    }
	}
	ଅନ୍ୟଥା				// string option
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = stringval;
	}
    }
    ଅନ୍ୟଥା ଯଦି (working && (opt_type == gov_hidden_bool
			|| opt_type == gov_hidden_ସଂଖ୍ୟା
			|| opt_type == gov_hidden_string))
	ret = FAIL;

    *option_end = c;		    // put back for error messages
    *arg = option_end;

    ଫେରନ୍ତୁ ret;
}

/*
 * Allocate a variable for a number constant.  Also deals with "0z" for blob.
 * Return OK or FAIL.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
eval_ସଂଖ୍ୟା(
	char_u	    **arg,
	typval_T    *rettv,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    evaluate,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    want_string)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		skip_quotes = !in_old_script(4);
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		get_float = FALSE;

    // We accept a float when the format matches
    // "[0-9]\+\.[0-9]\+\([eE][+-]\?[0-9]\+\)\?".  This is very
    // strict to avoid backwards compatibility problems.
    // With script version 2 and later the leading digit can be
    // omitted.
    // Don't look for a float after the "." operator, so that
    // ":let vers = 1.2.3" doesn't fail.
    ଯଦି (**arg == '.')
	p = *arg;
    ଅନ୍ୟଥା
    {
	p = *arg + 1;
	ଯଦି (skip_quotes)
	    ପାଇଁ (;;)
	    {
		ଯଦି (*p == '\'')
		    ++p;
		ଯଦି (!vim_isdigit(*p))
		    ଭାଙ୍ଗ;
		p = skipdigits(p);
	    }
	ଅନ୍ୟଥା
	    p = skipdigits(p);
    }
    ଯଦି (!want_string && p[0] == '.' && vim_isdigit(p[1]))
    {
	get_float = TRUE;
	p = skipdigits(p + 2);
	ଯଦି (*p == 'e' || *p == 'E')
	{
	    ++p;
	    ଯଦି (*p == '-' || *p == '+')
		++p;
	    ଯଦି (!vim_isdigit(*p))
		get_float = FALSE;
	    ଅନ୍ୟଥା
		p = skipdigits(p + 1);
	}
	ଯଦି (ASCII_ISALPHA(*p) || *p == '.')
	    get_float = FALSE;
    }
    ଯଦି (get_float)
    {
	float_T	f;

	*arg += string2float(*arg, &f, skip_quotes);
	ଯଦି (evaluate)
	{
	    rettv->v_type = VAR_FLOAT;
	    rettv->vval.v_float = f;
	}
    }
    ଅନ୍ୟଥା
    ଯଦି (**arg == '0' && ((*arg)[1] == 'z' || (*arg)[1] == 'Z'))
    {
	char_u  *bp;
	blob_T  *blob = ଖାଲି;  // init for gcc

	// Blob constant: 0z0123456789abcdef
	ଯଦି (evaluate)
	    blob = blob_alloc();
	ପାଇଁ (bp = *arg + 2; vim_isxdigit(bp[0]); bp += 2)
	{
	    ଯଦି (!vim_isxdigit(bp[1]))
	    {
		ଯଦି (blob != ଖାଲି)
		{
		    emsg(_(e_blob_literal_should_have_an_even_ସଂଖ୍ୟା_of_hex_characters));
		    ga_clear(&blob->bv_ga);
		    VIM_CLEAR(blob);
		}
		ଫେରନ୍ତୁ FAIL;
	    }
	    ଯଦି (blob != ଖାଲି)
		ga_append(&blob->bv_ga,
			     (hex2nr(*bp) << 4) + hex2nr(*(bp+1)));
	    ଯଦି (bp[2] == '.' && vim_isxdigit(bp[3]))
		++bp;
	}
	ଯଦି (blob != ଖାଲି)
	    rettv_blob_set(rettv, blob);
	*arg = bp;
    }
    ଅନ୍ୟଥା
    {
	varnumber_T	n;

	// decimal, hex or octal number
	vim_str2nr(*arg, ଖାଲି, &len, skip_quotes
		      ? STR2NR_NO_OCT + STR2NR_QUOTE
		      : STR2NR_ALL, &n, ଖାଲି, 0, TRUE, ଖାଲି);
	ଯଦି (len == 0)
	{
	    ଯଦି (evaluate)
		semsg(_(e_invalid_expression_str), *arg);
	    ଫେରନ୍ତୁ FAIL;
	}
	*arg += len;
	ଯଦି (evaluate)
	{
	    rettv->v_type = VAR_ସଂଖ୍ୟା;
	    rettv->vval.v_ସଂଖ୍ୟା = n;
	}
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Evaluate a string constant and put the result in "rettv".
 * "*arg" points to the double quote or to after it when "interpolate" is TRUE.
 * When "interpolate" is TRUE reduce "{{" to "{", reduce "}}" to "}" and stop
 * at a single "{".
 * Return OK or FAIL.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
eval_string(char_u **arg, typval_T *rettv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା evaluate, ପୂର୍ଣ୍ଣସଂଖ୍ୟା interpolate)
{
    char_u	*p;
    char_u	*end;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		extra = interpolate ? 1 : 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		off = interpolate ? 0 : 1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;

    // Find the end of the string, skipping backslashed characters.
    ପାଇଁ (p = *arg + off; *p != NUL && *p != '"'; MB_PTR_ADV(p))
    {
	ଯଦି (*p == '\\' && p[1] != NUL)
	{
	    ++p;
	    // A "\<x>" form occupies at least 4 characters, and produces up
	    // to 9 characters (6 for the char and 3 for a modifier):
	    // reserve space for 5 extra.
	    ଯଦି (*p == '<')
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା		modifiers = 0;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags = FSK_KEYCODE | FSK_IN_STRING;

		extra += 5;

		// Skip to the '>' to avoid using '{' inside for string
		// interpolation.
		ଯଦି (p[1] != '*')
		    flags |= FSK_SIMPLIFY;
		ଯଦି (find_special_ଚାବି(&p, &modifiers, flags, ଖାଲି) != 0)
		    --p;  // leave "p" on the ">"
	    }
	}
	ଅନ୍ୟଥା ଯଦି (interpolate && (*p == '{' || *p == '}'))
	{
	    ଯଦି (*p == '{' && p[1] != '{') // start of expression
		ଭାଙ୍ଗ;
	    ++p;
	    ଯଦି (p[-1] == '}' && *p != '}') // single '}' is an error
	    {
		semsg(_(e_stray_closing_curly_str), *arg);
		ଫେରନ୍ତୁ FAIL;
	    }
	    --extra;  // "{{" becomes "{", "}}" becomes "}"
	}
    }

    ଯଦି (*p != '"' && !(interpolate && *p == '{'))
    {
	semsg(_(e_missing_double_quote_str), *arg);
	ଫେରନ୍ତୁ FAIL;
    }

    // If only parsing, set *arg and return here
    ଯଦି (!evaluate)
    {
	*arg = p + off;
	ଫେରନ୍ତୁ OK;
    }

    // Copy the string into allocated memory, handling backslashed
    // characters.
    rettv->v_type = VAR_STRING;
    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - *arg + extra);
    rettv->vval.v_string = alloc(len);
    ଯଦି (rettv->vval.v_string == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    end = rettv->vval.v_string;

    ପାଇଁ (p = *arg + off; *p != NUL && *p != '"'; )
    {
	ଯଦି (*p == '\\')
	{
	    ବାଛ (*++p)
	    {
		ସ୍ଥିତି 'b': *end++ = BS; ++p; ଭାଙ୍ଗ;
		ସ୍ଥିତି 'e': *end++ = ESC; ++p; ଭାଙ୍ଗ;
		ସ୍ଥିତି 'f': *end++ = FF; ++p; ଭାଙ୍ଗ;
		ସ୍ଥିତି 'n': *end++ = NL; ++p; ଭାଙ୍ଗ;
		ସ୍ଥିତି 'r': *end++ = CAR; ++p; ଭାଙ୍ଗ;
		ସ୍ଥିତି 't': *end++ = TAB; ++p; ଭାଙ୍ଗ;

		ସ୍ଥିତି 'X': // hex: "\x1", "\x12"
		ସ୍ଥିତି 'x':
		ସ୍ଥିତି 'u': // Unicode: "\u0023"
		ସ୍ଥିତି 'U':
			  ଯଦି (vim_isxdigit(p[1]))
			  {
			      ପୂର୍ଣ୍ଣସଂଖ୍ୟା	n, nr;
			      ପୂର୍ଣ୍ଣସଂଖ୍ୟା	c = SAFE_toupper(*p);

			      ଯଦି (c == 'X')
				  n = 2;
			      ଅନ୍ୟଥା ଯଦି (*p == 'u')
				  n = 4;
			      ଅନ୍ୟଥା
				  n = 8;
			      nr = 0;
			      ଯେତେବେଳେ (--n >= 0 && vim_isxdigit(p[1]))
			      {
				  ++p;
				  nr = (nr << 4) + hex2nr(*p);
			      }
			      ++p;
			      // For "\u" store the number according to
			      // 'encoding'.
			      ଯଦି (c != 'X')
				  end += (*mb_char2bytes)(nr, end);
			      ଅନ୍ୟଥା
				  *end++ = nr;
			  }
			  ଭାଙ୍ଗ;

			  // octal: "\1", "\12", "\123"
		ସ୍ଥିତି '0':
		ସ୍ଥିତି '1':
		ସ୍ଥିତି '2':
		ସ୍ଥିତି '3':
		ସ୍ଥିତି '4':
		ସ୍ଥିତି '5':
		ସ୍ଥିତି '6':
		ସ୍ଥିତି '7': *end = *p++ - '0';
			  ଯଦି (*p >= '0' && *p <= '7')
			  {
			      *end = (*end << 3) + *p++ - '0';
			      ଯଦି (*p >= '0' && *p <= '7')
				  *end = (*end << 3) + *p++ - '0';
			  }
			  ++end;
			  ଭାଙ୍ଗ;

			  // Special key, e.g.: "\<C-W>"
		ସ୍ଥିତି '<':
			  {
			      ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags = FSK_KEYCODE | FSK_IN_STRING;

			      ଯଦି (p[1] != '*')
				  flags |= FSK_SIMPLIFY;
			      extra = trans_special(&p, end, flags, FALSE, ଖାଲି);
			      ଯଦି (extra != 0)
			      {
				  end += extra;
				  ଯଦି (end >= rettv->vval.v_string + len)
				      iemsg("eval_string() used more space than allocated");
				  ଭାଙ୍ଗ;
			      }
			  }
			  // FALLTHROUGH

		ପୂର୍ବନିର୍ଦ୍ଧାରିତ: MB_COPY_CHAR(p, end);
			  ଭାଙ୍ଗ;
	    }
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (interpolate && (*p == '{' || *p == '}'))
	    {
		ଯଦି (*p == '{' && p[1] != '{') // start of expression
		    ଭାଙ୍ଗ;
		++p;  // reduce "{{" to "{" and "}}" to "}"
	    }
	    MB_COPY_CHAR(p, end);
	}
    }
    *end = NUL;
    ଯଦି (*p == '"' && !interpolate)
	++p;
    *arg = p;

    ଫେରନ୍ତୁ OK;
}

/*
 * Allocate a variable for a 'str''ing' constant.
 * When "interpolate" is TRUE reduce "{{" to "{" and stop at a single "{".
 * Return OK when a "rettv" was set to the string.
 * Return FAIL on error, "rettv" is not set.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
eval_lit_string(char_u **arg, typval_T *rettv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା evaluate, ପୂର୍ଣ୍ଣସଂଖ୍ୟା interpolate)
{
    char_u	*p;
    char_u	*str;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		reduce = interpolate ? -1 : 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		off = interpolate ? 0 : 1;

    // Find the end of the string, skipping ''.
    ପାଇଁ (p = *arg + off; *p != NUL; MB_PTR_ADV(p))
    {
	ଯଦି (*p == '\'')
	{
	    ଯଦି (p[1] != '\'')
		ଭାଙ୍ଗ;
	    ++reduce;
	    ++p;
	}
	ଅନ୍ୟଥା ଯଦି (interpolate)
	{
	    ଯଦି (*p == '{')
	    {
		ଯଦି (p[1] != '{')
		    ଭାଙ୍ଗ;
		++p;
		++reduce;
	    }
	    ଅନ୍ୟଥା ଯଦି (*p == '}')
	    {
		++p;
		ଯଦି (*p != '}')
		{
		    semsg(_(e_stray_closing_curly_str), *arg);
		    ଫେରନ୍ତୁ FAIL;
		}
		++reduce;
	    }
	}
    }

    ଯଦି (*p != '\'' && !(interpolate && *p == '{'))
    {
	semsg(_(e_missing_single_quote_str), *arg);
	ଫେରନ୍ତୁ FAIL;
    }

    // If only parsing return after setting "*arg"
    ଯଦି (!evaluate)
    {
	*arg = p + off;
	ଫେରନ୍ତୁ OK;
    }

    // Copy the string into allocated memory, handling '' to ' reduction and
    // any expressions.
    str = alloc((p - *arg) - reduce);
    ଯଦି (str == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = str;

    ପାଇଁ (p = *arg + off; *p != NUL; )
    {
	ଯଦି (*p == '\'')
	{
	    ଯଦି (p[1] != '\'')
		ଭାଙ୍ଗ;
	    ++p;
	}
	ଅନ୍ୟଥା ଯଦି (interpolate && (*p == '{' || *p == '}'))
	{
	    ଯଦି (*p == '{' && p[1] != '{')
		ଭାଙ୍ଗ;
	    ++p;
	}
	MB_COPY_CHAR(p, str);
    }
    *str = NUL;
    *arg = p + off;

    ଫେରନ୍ତୁ OK;
}

/*
 * Evaluate a single or double quoted string possibly containing expressions.
 * "arg" points to the '$'.  The result is put in "rettv".
 * Returns OK or FAIL.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
eval_interp_string(char_u **arg, typval_T *rettv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା evaluate)
{
    typval_T	tv;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = OK;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		quote;
    garray_T	ga;
    char_u	*p;

    ga_init2(&ga, 1, 80);

    // *arg is on the '$' character, move it to the first string character.
    ++*arg;
    quote = **arg;
    ++*arg;

    ପାଇଁ (;;)
    {
	// Get the string up to the matching quote or to a single '{'.
	// "arg" is advanced to either the quote or the '{'.
	ଯଦି (quote == '"')
	    ret = eval_string(arg, &tv, evaluate, TRUE);
	ଅନ୍ୟଥା
	    ret = eval_lit_string(arg, &tv, evaluate, TRUE);
	ଯଦି (ret == FAIL)
	    ଭାଙ୍ଗ;
	ଯଦି (evaluate)
	{
	    ga_concat(&ga, tv.vval.v_string);
	    clear_tv(&tv);
	}

	ଯଦି (**arg != '{')
	{
	    // found terminating quote
	    ++*arg;
	    ଭାଙ୍ଗ;
	}
	p = eval_one_ପଦସମଷ୍ଟି_in_str(*arg, &ga, evaluate);
	ଯଦି (p == ଖାଲି)
	{
	    ret = FAIL;
	    ଭାଙ୍ଗ;
	}
	*arg = p;
    }

    rettv->v_type = VAR_STRING;
    ଯଦି (ret == FAIL || !evaluate || ga_append(&ga, NUL) == FAIL)
    {
	ga_clear(&ga);
	rettv->vval.v_string = ଖାଲି;
	ଫେରନ୍ତୁ ret;
    }

    rettv->vval.v_string = ga.ga_data;
    ଫେରନ୍ତୁ OK;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		copyID)
{
    ଫେରନ୍ତୁ echo_string_core(tv, tofree, numbuf, copyID, FALSE, TRUE, FALSE);
}

/*
 * Get the value of an environment variable.
 * "arg" is pointing to the '$'.  It is advanced to after the name.
 * If the environment variable was not set, silently assume it is empty.
 * Return FAIL if the name is invalid.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
eval_env_var(char_u **arg, typval_T *rettv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା evaluate)
{
    char_u	*string = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cc;
    char_u	*name;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mustfree = FALSE;

    ++*arg;
    name = *arg;
    len = get_env_len(arg);
    ଯଦି (evaluate)
    {
	ଯଦି (len == 0)
	    ଫେରନ୍ତୁ FAIL; // invalid empty name

	cc = name[len];
	name[len] = NUL;
	// first try vim_getenv(), fast for normal environment vars
	string = vim_getenv(name, &mustfree);
	ଯଦି (string != ଖାଲି && *string != NUL)
	{
	    ଯଦି (!mustfree)
		string = vim_strsave(string);
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (mustfree)
		vim_free(string);

	    // next try expanding things like $VIM and ${HOME}
	    string = expand_env_save(name - 1);
	    ଯଦି (string != ଖାଲି && *string == '$')
		VIM_CLEAR(string);
	}
	name[len] = cc;

	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = string;
	rettv->v_lock = 0;
    }

    ଫେରନ୍ତୁ OK;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_emsg_before = did_emsg;

    ଯଦି (argvars[0].v_type != VAR_STRING || !in_vim9script())
	lnum = (linenr_T)tv_get_ସଂଖ୍ୟା_chk(&argvars[0], ଖାଲି);
    ଯଦି (lnum <= 0 && did_emsg_before == did_emsg
					    && argvars[0].v_type != VAR_ସଂଖ୍ୟା)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	fnum;
	pos_T	*fp;

	// no valid number, try using arg like line()
	fp = var2fpos(&argvars[0], TRUE, &fnum, FALSE);
	ଯଦି (fp != ଖାଲି)
	    lnum = fp->lnum;
    }
    ଫେରନ୍ତୁ lnum;
}

/*
 * Get the lnum from the first argument.
 * Also accepts "$", then "buf" is used.
 * Returns 0 on error.
 */
    linenr_T
tv_get_lnum_buf(typval_T *argvars, buf_T *buf)
{
    ଯଦି (argvars[0].v_type == VAR_STRING
	    && argvars[0].vval.v_string != ଖାଲି
	    && argvars[0].vval.v_string[0] == '$'
	    && argvars[0].vval.v_string[1] == NUL
	    && buf != ଖାଲି)
	ଫେରନ୍ତୁ buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
    ଫେରନ୍ତୁ (linenr_T)tv_get_ସଂଖ୍ୟା_chk(&argvars[0], ଖାଲି);
}

/*
 * Get buffer by number or pattern.
 */
    buf_T *
tv_get_buf(typval_T *tv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା curtab_only)
{
    char_u	*name = tv->vval.v_string;
    buf_T	*buf;

    ଯଦି (tv->v_type == VAR_ସଂଖ୍ୟା)
	ଫେରନ୍ତୁ buflist_findnr((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv->vval.v_ସଂଖ୍ୟା);
    ଯଦି (tv->v_type != VAR_STRING)
	ଫେରନ୍ତୁ ଖାଲି;
    ଯଦି (name == ଖାଲି || *name == NUL)
	ଫେରନ୍ତୁ curbuf;
    ଯଦି (name[0] == '$' && name[1] == NUL)
	ଫେରନ୍ତୁ lastbuf;

    buf = buflist_find_by_name(name, curtab_only);

    // If not found, try expanding the name, like done for bufexists().
    ଯଦି (buf == ଖାଲି)
	buf = find_buffer(tv);

    ଫେରନ୍ତୁ buf;
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
    ଯଦି (buf == ଖାଲି
	    && tv->v_type != VAR_ସଂଖ୍ୟା
	    && tv->v_type != VAR_STRING)
	// issue errmsg for type error
	(ଶୂନ୍ୟ)tv_get_ସଂଖ୍ୟା(tv);
    ଫେରନ୍ତୁ buf;
}

/*
 * Check that repeating "slen" items "n" times, does not
 * overflow the int used for the resulting length.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_repeat_ଗଣନା(varnumber_T slen, varnumber_T n)
{
    // Use a division so that the multiplication cannot overflow.
    ଯଦି (n <= (varnumber_T)INT_MAX / slen)
	ଫେରନ୍ତୁ OK;

    ଅକ୍ଷର	buf[NUMBUFLEN];

    vim_snprintf(buf, ଆକାର(buf), "%lld", (long long)n);
    semsg(_(e_val_too_large), buf);
    ଫେରନ୍ତୁ FAIL;
}

#endif // FEAT_EVAL
