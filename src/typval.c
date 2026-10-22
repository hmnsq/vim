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

#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_EVAL)

/*
 * Allocate memory for a variable type-value, and make it empty (0 or NULL
 * value).
 */
    typval_T *
alloc_tv(శూన్య)
{
    తిరిగి ALLOC_CLEAR_ONE(typval_T);
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
    ఒకవేళ (rettv != ఖాళీ)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = s;
    }
    లేకపోతే
	vim_free(s);
    తిరిగి rettv;
}

/*
 * Free the memory for a variable type-value.
 */
    శూన్య
free_tv(typval_T *varp)
{
    ఒకవేళ (varp == ఖాళీ)
	తిరిగి;

    ఎంపిక (varp->v_type)
    {
	స్థితి VAR_FUNC:
	    func_unref(varp->vval.v_string);
	    // FALLTHROUGH
	స్థితి VAR_STRING:
	    vim_free(varp->vval.v_string);
	    ఆపు;
	స్థితి VAR_PARTIAL:
	    partial_unref(varp->vval.v_partial);
	    ఆపు;
	స్థితి VAR_BLOB:
	    blob_unref(varp->vval.v_blob);
	    ఆపు;
	స్థితి VAR_LIST:
	    list_unref(varp->vval.v_list);
	    ఆపు;
	స్థితి VAR_TUPLE:
	    tuple_unref(varp->vval.v_tuple);
	    ఆపు;
	స్థితి VAR_DICT:
	    dict_unref(varp->vval.v_dict);
	    ఆపు;
	స్థితి VAR_పని:
#నిర్వచితమైతే FEAT_పని_CHANNEL
	    పని_unref(varp->vval.v_పని);
	    ఆపు;
#అయితే_ముగింపు
	స్థితి VAR_CHANNEL:
#నిర్వచితమైతే FEAT_పని_CHANNEL
	    channel_unref(varp->vval.v_channel);
	    ఆపు;
#అయితే_ముగింపు
	స్థితి VAR_CLASS:
	    class_unref(varp->vval.v_class);
	    ఆపు;
	స్థితి VAR_OBJECT:
	    object_unref(varp->vval.v_object);
	    ఆపు;

	స్థితి VAR_TYPEALIAS:
	    typealias_unref(varp->vval.v_typealias);
	    ఆపు;

	స్థితి VAR_సంఖ్య:
	స్థితి VAR_FLOAT:
	స్థితి VAR_ANY:
	స్థితి VAR_UNKNOWN:
	స్థితి VAR_VOID:
	స్థితి VAR_BOOL:
	స్థితి VAR_SPECIAL:
	స్థితి VAR_INSTR:
	    ఆపు;
    }
    vim_free(varp);
}

/*
 * Free the memory for a variable value and set the value to NULL or 0.
 */
    శూన్య
clear_tv(typval_T *varp)
{
    ఒకవేళ (varp == ఖాళీ)
	తిరిగి;

    ఎంపిక (varp->v_type)
    {
	స్థితి VAR_FUNC:
	    func_unref(varp->vval.v_string);
	    // FALLTHROUGH
	స్థితి VAR_STRING:
	    VIM_CLEAR(varp->vval.v_string);
	    ఆపు;
	స్థితి VAR_PARTIAL:
	    partial_unref(varp->vval.v_partial);
	    varp->vval.v_partial = ఖాళీ;
	    ఆపు;
	స్థితి VAR_BLOB:
	    blob_unref(varp->vval.v_blob);
	    varp->vval.v_blob = ఖాళీ;
	    ఆపు;
	స్థితి VAR_LIST:
	    list_unref(varp->vval.v_list);
	    varp->vval.v_list = ఖాళీ;
	    ఆపు;
	స్థితి VAR_TUPLE:
	    tuple_unref(varp->vval.v_tuple);
	    varp->vval.v_tuple = ఖాళీ;
	    ఆపు;
	స్థితి VAR_DICT:
	    dict_unref(varp->vval.v_dict);
	    varp->vval.v_dict = ఖాళీ;
	    ఆపు;
	స్థితి VAR_సంఖ్య:
	స్థితి VAR_BOOL:
	స్థితి VAR_SPECIAL:
	    varp->vval.v_సంఖ్య = 0;
	    ఆపు;
	స్థితి VAR_FLOAT:
	    varp->vval.v_float = 0.0;
	    ఆపు;
	స్థితి VAR_పని:
#నిర్వచితమైతే FEAT_పని_CHANNEL
	    పని_unref(varp->vval.v_పని);
	    varp->vval.v_పని = ఖాళీ;
#అయితే_ముగింపు
	    ఆపు;
	స్థితి VAR_CHANNEL:
#నిర్వచితమైతే FEAT_పని_CHANNEL
	    channel_unref(varp->vval.v_channel);
	    varp->vval.v_channel = ఖాళీ;
#అయితే_ముగింపు
	    ఆపు;
	స్థితి VAR_INSTR:
	    VIM_CLEAR(varp->vval.v_instr);
	    ఆపు;
	స్థితి VAR_CLASS:
	    class_unref(varp->vval.v_class);
	    varp->vval.v_class = ఖాళీ;
	    ఆపు;
	స్థితి VAR_OBJECT:
	    object_unref(varp->vval.v_object);
	    varp->vval.v_object = ఖాళీ;
	    ఆపు;
	స్థితి VAR_TYPEALIAS:
	    typealias_unref(varp->vval.v_typealias);
	    varp->vval.v_typealias = ఖాళీ;
	    ఆపు;
	స్థితి VAR_UNKNOWN:
	స్థితి VAR_ANY:
	స్థితి VAR_VOID:
	    ఆపు;
    }
    varp->v_తాళం = 0;
}

/*
 * Set the value of a variable to NULL without freeing items.
 */
    శూన్య
init_tv(typval_T *varp)
{
    ఒకవేళ (varp != ఖాళీ)
	CLEAR_సూచిక(varp);
}

    స్థిర varnumber_T
tv_get_bool_or_సంఖ్య_chk(
	typval_T    *varp,
	పూర్ణాంకం	    *denote,
	పూర్ణాంకం	    want_bool,
	పూర్ణాంకం	    vim9_string_error) // in Vim9 using a string is an error
{
    varnumber_T	n = 0L;

    ఎంపిక (varp->v_type)
    {
	స్థితి VAR_సంఖ్య:
	    ఒకవేళ (లోపల_vim9script() && want_bool && varp->vval.v_సంఖ్య != 0
						   && varp->vval.v_సంఖ్య != 1)
	    {
		semsg(_(e_using_సంఖ్య_as_bool_nr), varp->vval.v_సంఖ్య);
		ఆపు;
	    }
	    తిరిగి varp->vval.v_సంఖ్య;
	స్థితి VAR_FLOAT:
	    emsg(_(e_using_float_as_సంఖ్య));
	    ఆపు;
	స్థితి VAR_FUNC:
	స్థితి VAR_PARTIAL:
	    emsg(_(e_using_funcref_as_సంఖ్య));
	    ఆపు;
	స్థితి VAR_STRING:
	    ఒకవేళ (vim9_string_error && లోపల_vim9script())
	    {
		emsg_using_string_as(varp, !want_bool);
		ఆపు;
	    }
	    ఒకవేళ (varp->vval.v_string != ఖాళీ)
		vim_str2nr(varp->vval.v_string, ఖాళీ, ఖాళీ,
					 STR2NR_ALL, &n, ఖాళీ, 0, FALSE, ఖాళీ);
	    తిరిగి n;
	స్థితి VAR_LIST:
	    emsg(_(e_using_list_as_సంఖ్య));
	    ఆపు;
	స్థితి VAR_TUPLE:
	    emsg(_(e_using_tuple_as_సంఖ్య));
	    ఆపు;
	స్థితి VAR_DICT:
	    emsg(_(e_using_dictionary_as_సంఖ్య));
	    ఆపు;
	స్థితి VAR_BOOL:
	స్థితి VAR_SPECIAL:
	    ఒకవేళ (!want_bool && లోపల_vim9script())
	    {
		ఒకవేళ (varp->v_type == VAR_BOOL)
		    emsg(_(e_using_bool_as_సంఖ్య));
		లేకపోతే
		    emsg(_(e_using_special_as_సంఖ్య));
		ఆపు;
	    }
	    తిరిగి varp->vval.v_సంఖ్య == VVAL_TRUE ? 1 : 0;
	స్థితి VAR_పని:
#నిర్వచితమైతే FEAT_పని_CHANNEL
	    emsg(_(e_using_పని_as_సంఖ్య));
	    ఆపు;
#అయితే_ముగింపు
	స్థితి VAR_CHANNEL:
#నిర్వచితమైతే FEAT_పని_CHANNEL
	    emsg(_(e_using_channel_as_సంఖ్య));
	    ఆపు;
#అయితే_ముగింపు
	స్థితి VAR_BLOB:
	    emsg(_(e_using_blob_as_సంఖ్య));
	    ఆపు;
	స్థితి VAR_CLASS:
	స్థితి VAR_TYPEALIAS:
	    check_typval_is_విలువ(varp);
	    ఆపు;
	స్థితి VAR_OBJECT:
	    {
		ఒకవేళ (varp->vval.v_object == ఖాళీ)
		    emsg(_(e_using_object_as_string));
		లేకపోతే
		{
		    class_T *cl = varp->vval.v_object->obj_class;
		    ఒకవేళ (cl != ఖాళీ && IS_ENUM(cl))
			semsg(_(e_using_enum_str_as_సంఖ్య), cl->class_name.string);
		    లేకపోతే
			emsg(_(e_using_object_as_సంఖ్య));
		}
	    }
	    ఆపు;
	స్థితి VAR_VOID:
	    emsg(_(e_cannot_use_void_విలువ));
	    ఆపు;
	స్థితి VAR_UNKNOWN:
	స్థితి VAR_ANY:
	స్థితి VAR_INSTR:
	    అంతర్గత_error_no_abort("tv_get_number(UNKNOWN)");
	    ఆపు;
    }
    ఒకవేళ (denote == ఖాళీ)		// useful for values that must be unsigned
	n = -1;
    లేకపోతే
	*denote = TRUE;
    తిరిగి n;
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
tv_get_సంఖ్య(typval_T *varp)
{
    పూర్ణాంకం		error = FALSE;

    తిరిగి tv_get_సంఖ్య_chk(varp, &error);	// return 0L on error
}

/*
 * Like tv_get_number() but in Vim9 script do convert a number in a string to a
 * number without giving an error.
 */
    varnumber_T
tv_గమ్యం_సంఖ్య(typval_T *varp)
{
    పూర్ణాంకం		error = FALSE;

    తిరిగి tv_get_bool_or_సంఖ్య_chk(varp, &error, FALSE, FALSE);
}

    varnumber_T
tv_get_సంఖ్య_chk(typval_T *varp, పూర్ణాంకం *denote)
{
    తిరిగి tv_get_bool_or_సంఖ్య_chk(varp, denote, FALSE, TRUE);
}

/*
 * Get the boolean value of "varp".  This is like tv_get_number_chk(),
 * but in Vim9 script accepts Number (0 and 1) and Bool/Special.
 */
    varnumber_T
tv_get_bool(typval_T *varp)
{
    తిరిగి tv_get_bool_or_సంఖ్య_chk(varp, ఖాళీ, TRUE, TRUE);
}

/*
 * Get the boolean value of "varp".  This is like tv_get_number_chk(),
 * but in Vim9 script accepts Number and Bool.
 */
    varnumber_T
tv_get_bool_chk(typval_T *varp, పూర్ణాంకం *denote)
{
    తిరిగి tv_get_bool_or_సంఖ్య_chk(varp, denote, TRUE, TRUE);
}

    స్థిర float_T
tv_get_float_chk(typval_T *varp, పూర్ణాంకం *error)
{
    ఎంపిక (varp->v_type)
    {
	స్థితి VAR_సంఖ్య:
	    తిరిగి (float_T)(varp->vval.v_సంఖ్య);
	స్థితి VAR_FLOAT:
	    తిరిగి varp->vval.v_float;
	స్థితి VAR_FUNC:
	స్థితి VAR_PARTIAL:
	    emsg(_(e_using_funcref_as_float));
	    ఆపు;
	స్థితి VAR_STRING:
	    emsg(_(e_using_string_as_float));
	    ఆపు;
	స్థితి VAR_LIST:
	    emsg(_(e_using_list_as_float));
	    ఆపు;
	స్థితి VAR_TUPLE:
	    emsg(_(e_using_tuple_as_float));
	    ఆపు;
	స్థితి VAR_DICT:
	    emsg(_(e_using_dictionary_as_float));
	    ఆపు;
	స్థితి VAR_BOOL:
	    emsg(_(e_using_boolean_విలువ_as_float));
	    ఆపు;
	స్థితి VAR_SPECIAL:
	    emsg(_(e_using_special_విలువ_as_float));
	    ఆపు;
	స్థితి VAR_పని:
#నిర్వచితమైతే FEAT_పని_CHANNEL
	    emsg(_(e_using_పని_as_float));
	    ఆపు;
#అయితే_ముగింపు
	స్థితి VAR_CHANNEL:
#నిర్వచితమైతే FEAT_పని_CHANNEL
	    emsg(_(e_using_channel_as_float));
	    ఆపు;
#అయితే_ముగింపు
	స్థితి VAR_BLOB:
	    emsg(_(e_using_blob_as_float));
	    ఆపు;
	స్థితి VAR_CLASS:
	స్థితి VAR_TYPEALIAS:
	    check_typval_is_విలువ(varp);
	    ఆపు;
	స్థితి VAR_OBJECT:
	    emsg(_(e_using_object_as_float));
	    ఆపు;
	స్థితి VAR_VOID:
	    emsg(_(e_cannot_use_void_విలువ));
	    ఆపు;
	స్థితి VAR_UNKNOWN:
	స్థితి VAR_ANY:
	స్థితి VAR_INSTR:
	    అంతర్గత_error_no_abort("tv_get_float(UNKNOWN)");
	    ఆపు;
    }
    ఒకవేళ (error != ఖాళీ)
	*error = TRUE;
    తిరిగి 0;
}

    float_T
tv_get_float(typval_T *varp)
{
    తిరిగి tv_get_float_chk(varp, ఖాళీ);
}

/*
 * Give an error and return FAIL unless "args[idx]" is unknown
 */
    పూర్ణాంకం
check_for_unknown_arg(typval_T *వాదనలు, పూర్ణాంకం idx)
{
    ఒకవేళ (వాదనలు[idx].v_type != VAR_UNKNOWN)
    {
	semsg(_(e_too_many_arguments), idx + 1);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string.
 */
    పూర్ణాంకం
check_for_string_arg(typval_T *వాదనలు, పూర్ణాంకం idx)
{
    ఒకవేళ (వాదనలు[idx].v_type != VAR_STRING)
    {
	semsg(_(e_string_required_for_వాదం_nr), idx + 1);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a non-empty string.
 */
    పూర్ణాంకం
check_for_nonempty_string_arg(typval_T *వాదనలు, పూర్ణాంకం idx)
{
    ఒకవేళ (check_for_string_arg(వాదనలు, idx) == FAIL)
	తిరిగి FAIL;
    ఒకవేళ (వాదనలు[idx].vval.v_string == ఖాళీ || *వాదనలు[idx].vval.v_string == NUL)
    {
	semsg(_(e_non_ఖాళీలు_string_required_for_వాదం_nr), idx + 1);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Check for an optional string argument at 'idx'
 */
    పూర్ణాంకం
check_for_opt_string_arg(typval_T *వాదనలు, పూర్ణాంకం idx)
{
    తిరిగి (వాదనలు[idx].v_type == VAR_UNKNOWN
	    || check_for_string_arg(వాదనలు, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a number.
 */
    పూర్ణాంకం
check_for_సంఖ్య_arg(typval_T *వాదనలు, పూర్ణాంకం idx)
{
    ఒకవేళ (వాదనలు[idx].v_type != VAR_సంఖ్య)
    {
	semsg(_(e_సంఖ్య_required_for_వాదం_nr), idx + 1);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Check for an optional number argument at 'idx'
 */
    పూర్ణాంకం
check_for_opt_సంఖ్య_arg(typval_T *వాదనలు, పూర్ణాంకం idx)
{
    తిరిగి (వాదనలు[idx].v_type == VAR_UNKNOWN
	    || check_for_సంఖ్య_arg(వాదనలు, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a float or a number.
 */
    పూర్ణాంకం
check_for_float_or_nr_arg(typval_T *వాదనలు, పూర్ణాంకం idx)
{
    ఒకవేళ (వాదనలు[idx].v_type != VAR_FLOAT && వాదనలు[idx].v_type != VAR_సంఖ్య)
    {
	semsg(_(e_float_or_సంఖ్య_required_for_వాదం_nr), idx + 1);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a bool.
 */
    పూర్ణాంకం
check_for_bool_arg(typval_T *వాదనలు, పూర్ణాంకం idx)
{
    ఒకవేళ (వాదనలు[idx].v_type != VAR_BOOL
	    && !(వాదనలు[idx].v_type == VAR_సంఖ్య
		&& (వాదనలు[idx].vval.v_సంఖ్య == 0
		    || వాదనలు[idx].vval.v_సంఖ్య == 1)))
    {
	semsg(_(e_bool_required_for_వాదం_nr), idx + 1);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a bool or a number.
 */
    స్థిర పూర్ణాంకం
check_for_bool_or_సంఖ్య_arg(typval_T *వాదనలు, పూర్ణాంకం idx)
{
    ఒకవేళ (వాదనలు[idx].v_type != VAR_BOOL && వాదనలు[idx].v_type != VAR_సంఖ్య)
    {
	semsg(_(e_bool_or_సంఖ్య_required_for_వాదం_nr), idx + 1);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Check for an optional bool argument at 'idx'.
 * Return FAIL if the type is wrong.
 */
    పూర్ణాంకం
check_for_opt_bool_arg(typval_T *వాదనలు, పూర్ణాంకం idx)
{
    ఒకవేళ (వాదనలు[idx].v_type == VAR_UNKNOWN)
	తిరిగి OK;
    తిరిగి check_for_bool_arg(వాదనలు, idx);
}

/*
 * Check for an optional bool or number argument at 'idx'.
 * Return FAIL if the type is wrong.
 */
    పూర్ణాంకం
check_for_opt_bool_or_సంఖ్య_arg(typval_T *వాదనలు, పూర్ణాంకం idx)
{
    ఒకవేళ (వాదనలు[idx].v_type == VAR_UNKNOWN)
	తిరిగి OK;
    తిరిగి check_for_bool_or_సంఖ్య_arg(వాదనలు, idx);
}

/*
 * Check for an optional bool or dict argument at 'idx'.
 * Return FAIL if the type is wrong.
 */
    పూర్ణాంకం
check_for_opt_bool_or_dict_arg(typval_T *వాదనలు, పూర్ణాంకం idx)
{
    ఒకవేళ (వాదనలు[idx].v_type == VAR_UNKNOWN || వాదనలు[idx].v_type == VAR_DICT)
	తిరిగి OK;
    తిరిగి check_for_bool_arg(వాదనలు, idx);
}

/*
 * Give an error and return FAIL unless "args[idx]" is a blob.
 */
    పూర్ణాంకం
check_for_blob_arg(typval_T *వాదనలు, పూర్ణాంకం idx)
{
    ఒకవేళ (వాదనలు[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_blob_required_for_వాదం_nr), idx + 1);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list.
 */
    పూర్ణాంకం
check_for_list_arg(typval_T *వాదనలు, పూర్ణాంకం idx)
{
    ఒకవేళ (వాదనలు[idx].v_type != VAR_LIST)
    {
	semsg(_(e_list_required_for_వాదం_nr), idx + 1);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a non-NULL list.
 */
    పూర్ణాంకం
check_for_nonnull_list_arg(typval_T *వాదనలు, పూర్ణాంకం idx)
{
    ఒకవేళ (check_for_list_arg(వాదనలు, idx) == FAIL)
	తిరిగి FAIL;

    ఒకవేళ (వాదనలు[idx].vval.v_list == ఖాళీ)
    {
	semsg(_(e_non_null_list_required_for_వాదం_nr), idx + 1);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Check for an optional list argument at 'idx'
 */
    పూర్ణాంకం
check_for_opt_list_arg(typval_T *వాదనలు, పూర్ణాంకం idx)
{
    తిరిగి (వాదనలు[idx].v_type == VAR_UNKNOWN
	    || check_for_list_arg(వాదనలు, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a tuple.
 */
    పూర్ణాంకం
check_for_tuple_arg(typval_T *వాదనలు, పూర్ణాంకం idx)
{
    ఒకవేళ (వాదనలు[idx].v_type != VAR_TUPLE)
    {
	semsg(_(e_tuple_required_for_వాదం_nr), idx + 1);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a dict.
 */
    పూర్ణాంకం
check_for_dict_arg(typval_T *వాదనలు, పూర్ణాంకం idx)
{
    ఒకవేళ (వాదనలు[idx].v_type != VAR_DICT)
    {
	semsg(_(e_dict_required_for_వాదం_nr), idx + 1);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a non-NULL dict.
 */
    పూర్ణాంకం
check_for_nonnull_dict_arg(typval_T *వాదనలు, పూర్ణాంకం idx)
{
    ఒకవేళ (check_for_dict_arg(వాదనలు, idx) == FAIL)
	తిరిగి FAIL;

    ఒకవేళ (వాదనలు[idx].vval.v_dict == ఖాళీ)
    {
	semsg(_(e_non_null_dict_required_for_వాదం_nr), idx + 1);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Check for an optional dict argument at 'idx'
 */
    పూర్ణాంకం
check_for_opt_dict_arg(typval_T *వాదనలు, పూర్ణాంకం idx)
{
    తిరిగి (వాదనలు[idx].v_type == VAR_UNKNOWN
	    || check_for_dict_arg(వాదనలు, idx) != FAIL) ? OK : FAIL;
}

/*
 * Check for an optional non-NULL dict argument at 'idx'
 */
    పూర్ణాంకం
check_for_opt_nonnull_dict_arg(typval_T *వాదనలు, పూర్ణాంకం idx)
{
    తిరిగి (వాదనలు[idx].v_type == VAR_UNKNOWN
	    || check_for_nonnull_dict_arg(వాదనలు, idx) != FAIL) ? OK : FAIL;
}

#ఒకవేళ defined(FEAT_పని_CHANNEL)
/*
 * Give an error and return FAIL unless "args[idx]" is a channel or a job.
 */
    పూర్ణాంకం
check_for_chan_or_పని_arg(typval_T *వాదనలు, పూర్ణాంకం idx)
{
    ఒకవేళ (వాదనలు[idx].v_type != VAR_CHANNEL && వాదనలు[idx].v_type != VAR_పని)
    {
	semsg(_(e_chan_or_పని_required_for_వాదం_nr), idx + 1);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an optional channel or a
 * job.
 */
    పూర్ణాంకం
check_for_opt_chan_or_పని_arg(typval_T *వాదనలు, పూర్ణాంకం idx)
{
    తిరిగి (వాదనలు[idx].v_type == VAR_UNKNOWN
	    || check_for_chan_or_పని_arg(వాదనలు, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a job.
 */
    పూర్ణాంకం
check_for_పని_arg(typval_T *వాదనలు, పూర్ణాంకం idx)
{
    ఒకవేళ (వాదనలు[idx].v_type != VAR_పని)
    {
	semsg(_(e_పని_required_for_వాదం_nr), idx + 1);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Check for an optional job argument at 'idx'.
 */
    పూర్ణాంకం
check_for_opt_పని_arg(typval_T *వాదనలు, పూర్ణాంకం idx)
{
    తిరిగి (వాదనలు[idx].v_type == VAR_UNKNOWN
	    || check_for_పని_arg(వాదనలు, idx) != FAIL) ? OK : FAIL;
}
#లేకపోతే
/*
 * Give an error and return FAIL unless "args[idx]" is an optional channel or a
 * job.  Used without the +channel feature, thus only VAR_UNKNOWN is accepted.
 */
    పూర్ణాంకం
check_for_opt_chan_or_పని_arg(typval_T *వాదనలు, పూర్ణాంకం idx)
{
    తిరిగి వాదనలు[idx].v_type == VAR_UNKNOWN ? OK : FAIL;
}
#అయితే_ముగింపు

/*
 * Give an error and return FAIL unless "args[idx]" is a string or
 * a number.
 */
    పూర్ణాంకం
check_for_string_or_సంఖ్య_arg(typval_T *వాదనలు, పూర్ణాంకం idx)
{
    ఒకవేళ (వాదనలు[idx].v_type != VAR_STRING && వాదనలు[idx].v_type != VAR_సంఖ్య)
    {
	semsg(_(e_string_or_సంఖ్య_required_for_వాదం_nr), idx + 1);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Check for an optional string or number argument at 'idx'.
 */
    పూర్ణాంకం
check_for_opt_string_or_సంఖ్య_arg(typval_T *వాదనలు, పూర్ణాంకం idx)
{
    తిరిగి (వాదనలు[idx].v_type == VAR_UNKNOWN
	    || check_for_string_or_సంఖ్య_arg(వాదనలు, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a buffer number.
 * Buffer number can be a number or a string.
 */
    పూర్ణాంకం
check_for_బఫరు_arg(typval_T *వాదనలు, పూర్ణాంకం idx)
{
    తిరిగి check_for_string_or_సంఖ్య_arg(వాదనలు, idx);
}

/*
 * Check for an optional buffer argument at 'idx'
 */
    పూర్ణాంకం
check_for_opt_బఫరు_arg(typval_T *వాదనలు, పూర్ణాంకం idx)
{
    తిరిగి (వాదనలు[idx].v_type == VAR_UNKNOWN
	    || check_for_బఫరు_arg(వాదనలు, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a line number.
 * Line number can be a number or a string.
 */
    పూర్ణాంకం
check_for_lnum_arg(typval_T *వాదనలు, పూర్ణాంకం idx)
{
    తిరిగి check_for_string_or_సంఖ్య_arg(వాదనలు, idx);
}

/*
 * Check for an optional line number argument at 'idx'
 */
    పూర్ణాంకం
check_for_opt_lnum_arg(typval_T *వాదనలు, పూర్ణాంకం idx)
{
    తిరిగి (వాదనలు[idx].v_type == VAR_UNKNOWN
	    || check_for_lnum_arg(వాదనలు, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a blob.
 */
    పూర్ణాంకం
check_for_string_or_blob_arg(typval_T *వాదనలు, పూర్ణాంకం idx)
{
    ఒకవేళ (వాదనలు[idx].v_type != VAR_STRING && వాదనలు[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_string_or_blob_required_for_వాదం_nr), idx + 1);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a list.
 */
    పూర్ణాంకం
check_for_string_or_list_arg(typval_T *వాదనలు, పూర్ణాంకం idx)
{
    ఒకవేళ (వాదనలు[idx].v_type != VAR_STRING && వాదనలు[idx].v_type != VAR_LIST)
    {
	semsg(_(e_string_or_list_required_for_వాదం_nr), idx + 1);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string, a list, a
 * tuple or a blob.
 */
    పూర్ణాంకం
check_for_string_or_list_or_tuple_or_blob_arg(typval_T *వాదనలు, పూర్ణాంకం idx)
{
    ఒకవేళ (వాదనలు[idx].v_type != VAR_STRING
	    && వాదనలు[idx].v_type != VAR_LIST
	    && వాదనలు[idx].v_type != VAR_TUPLE
	    && వాదనలు[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_string_list_tuple_or_blob_required_for_వాదం_nr), idx + 1);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Check for an optional string or list argument at 'idx'
 */
    పూర్ణాంకం
check_for_opt_string_or_list_arg(typval_T *వాదనలు, పూర్ణాంకం idx)
{
    తిరిగి (వాదనలు[idx].v_type == VAR_UNKNOWN
	    || check_for_string_or_list_arg(వాదనలు, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a dict.
 */
    పూర్ణాంకం
check_for_string_or_dict_arg(typval_T *వాదనలు, పూర్ణాంకం idx)
{
    ఒకవేళ (వాదనలు[idx].v_type != VAR_STRING && వాదనలు[idx].v_type != VAR_DICT)
    {
	semsg(_(e_string_or_dict_required_for_వాదం_nr), idx + 1);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a number
 * or a list.
 */
    పూర్ణాంకం
check_for_string_or_సంఖ్య_or_list_arg(typval_T *వాదనలు, పూర్ణాంకం idx)
{
    ఒకవేళ (వాదనలు[idx].v_type != VAR_STRING
	    && వాదనలు[idx].v_type != VAR_సంఖ్య
	    && వాదనలు[idx].v_type != VAR_LIST)
    {
	semsg(_(e_string_సంఖ్య_or_list_required_for_వాదం_nr), idx + 1);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an optional string
 * or number or a list
 */
    పూర్ణాంకం
check_for_opt_string_or_సంఖ్య_or_list_arg(typval_T *వాదనలు, పూర్ణాంకం idx)
{
    తిరిగి (వాదనలు[idx].v_type == VAR_UNKNOWN
	    || check_for_string_or_సంఖ్య_or_list_arg(వాదనలు, idx)
							!= FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string, a number, a
 * list, a tuple or a blob.
 */
    పూర్ణాంకం
check_for_repeat_func_arg(typval_T *వాదనలు, పూర్ణాంకం idx)
{
    ఒకవేళ (వాదనలు[idx].v_type != VAR_STRING
	    && వాదనలు[idx].v_type != VAR_సంఖ్య
	    && వాదనలు[idx].v_type != VAR_LIST
	    && వాదనలు[idx].v_type != VAR_TUPLE
	    && వాదనలు[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_repeatable_type_required_for_వాదం_nr), idx + 1);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string, a list, a
 * tuple or a dict.
 */
    పూర్ణాంకం
check_for_string_list_tuple_or_dict_arg(typval_T *వాదనలు, పూర్ణాంకం idx)
{
    ఒకవేళ (వాదనలు[idx].v_type != VAR_STRING
	    && వాదనలు[idx].v_type != VAR_LIST
	    && వాదనలు[idx].v_type != VAR_TUPLE
	    && వాదనలు[idx].v_type != VAR_DICT)
    {
	semsg(_(e_string_list_tuple_or_dict_required_for_వాదం_nr), idx + 1);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string
 * or a function reference.
 */
    పూర్ణాంకం
check_for_string_or_func_arg(typval_T *వాదనలు, పూర్ణాంకం idx)
{
    ఒకవేళ (వాదనలు[idx].v_type != VAR_PARTIAL
	    && వాదనలు[idx].v_type != VAR_FUNC
	    && వాదనలు[idx].v_type != VAR_STRING)
    {
	semsg(_(e_string_or_function_required_for_వాదం_nr), idx + 1);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list or a blob.
 */
    పూర్ణాంకం
check_for_list_or_blob_arg(typval_T *వాదనలు, పూర్ణాంకం idx)
{
    ఒకవేళ (వాదనలు[idx].v_type != VAR_LIST && వాదనలు[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_list_or_blob_required_for_వాదం_nr), idx + 1);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list or a tuple.
 */
    పూర్ణాంకం
check_for_list_or_tuple_arg(typval_T *వాదనలు, పూర్ణాంకం idx)
{
    ఒకవేళ (వాదనలు[idx].v_type != VAR_LIST && వాదనలు[idx].v_type != VAR_TUPLE)
    {
	semsg(_(e_list_or_tuple_required_for_వాదం_nr), idx + 1);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list, a tuple or a
 * blob.
 */
    పూర్ణాంకం
check_for_list_or_tuple_or_blob_arg(typval_T *వాదనలు, పూర్ణాంకం idx)
{
    ఒకవేళ (వాదనలు[idx].v_type != VAR_LIST
	    && వాదనలు[idx].v_type != VAR_TUPLE
	    && వాదనలు[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_list_or_tuple_or_blob_required_for_వాదం_nr), idx + 1);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list, a tuple or a
 * dict
 */
    పూర్ణాంకం
check_for_list_or_tuple_or_dict_arg(typval_T *వాదనలు, పూర్ణాంకం idx)
{
    ఒకవేళ (వాదనలు[idx].v_type != VAR_LIST
	    && వాదనలు[idx].v_type != VAR_TUPLE
	    && వాదనలు[idx].v_type != VAR_DICT)
    {
	semsg(_(e_list_or_tuple_or_dict_required_for_వాదం_nr), idx + 1);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list or dict or a
 * blob.
 */
    పూర్ణాంకం
check_for_list_or_dict_or_blob_arg(typval_T *వాదనలు, పూర్ణాంకం idx)
{
    ఒకవేళ (వాదనలు[idx].v_type != VAR_LIST
	    && వాదనలు[idx].v_type != VAR_DICT
	    && వాదనలు[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_list_dict_or_blob_required_for_వాదం_nr), idx + 1);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list, a tuple, a dict,
 * a blob or a string.
 */
    పూర్ణాంకం
check_for_list_tuple_dict_blob_or_string_arg(typval_T *వాదనలు, పూర్ణాంకం idx)
{
    ఒకవేళ (వాదనలు[idx].v_type != VAR_LIST
	    && వాదనలు[idx].v_type != VAR_TUPLE
	    && వాదనలు[idx].v_type != VAR_DICT
	    && వాదనలు[idx].v_type != VAR_BLOB
	    && వాదనలు[idx].v_type != VAR_STRING)
    {
	semsg(_(e_list_tuple_dict_blob_or_string_required_for_వాదం_nr), idx + 1);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an optional buffer
 * number or a dict.
 */
    పూర్ణాంకం
check_for_opt_బఫరు_or_dict_arg(typval_T *వాదనలు, పూర్ణాంకం idx)
{
    ఒకవేళ (వాదనలు[idx].v_type != VAR_UNKNOWN
	    && వాదనలు[idx].v_type != VAR_STRING
	    && వాదనలు[idx].v_type != VAR_సంఖ్య
	    && వాదనలు[idx].v_type != VAR_DICT)
    {
	semsg(_(e_string_required_for_వాదం_nr), idx + 1);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an object.
 */
    పూర్ణాంకం
check_for_object_arg(typval_T *వాదనలు, పూర్ణాంకం idx)
{
    ఒకవేళ (వాదనలు[idx].v_type != VAR_OBJECT)
    {
	semsg(_(e_object_required_for_వాదం_nr), idx + 1);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Returns TRUE if "tv" is a type alias for a class
 */
    స్థిర పూర్ణాంకం
tv_class_alias(typval_T *tv)
{
    తిరిగి tv->v_type == VAR_TYPEALIAS &&
			tv->vval.v_typealias->ta_type->tt_type == VAR_OBJECT;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a class
 * or class typealias.
 */
    పూర్ణాంకం
check_for_class_or_typealias_వాదనలు(typval_T *వాదనలు, పూర్ణాంకం idx)
{
    కొరకు (పూర్ణాంకం i = idx; వాదనలు[i].v_type != VAR_UNKNOWN; ++i)
    {
	ఒకవేళ (వాదనలు[i].v_type != VAR_CLASS && !tv_class_alias(&వాదనలు[idx]))
	{
	    semsg(_(e_class_or_typealias_required_for_వాదం_nr), i + 1);
	    తిరిగి FAIL;
	}
    }
    తిరిగి OK;
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
    స్థిర char_u   mybuf[NUMBUFLEN];

    తిరిగి tv_get_string_buf(varp, mybuf);
}

/*
 * Like tv_get_string() but don't allow number to string conversion for Vim9.
 */
    char_u *
tv_get_string_strict(typval_T *varp)
{
    స్థిర char_u   mybuf[NUMBUFLEN];
    char_u	    *res =  tv_get_string_buf_chk_strict(
						 varp, mybuf, లోపల_vim9script());

    తిరిగి res != ఖాళీ ? res : (char_u *)"";
}

    char_u *
tv_get_string_buf(typval_T *varp, char_u *buf)
{
    char_u	*res = tv_get_string_buf_chk(varp, buf);

    తిరిగి res != ఖాళీ ? res : (char_u *)"";
}

/*
 * Careful: This uses a single, static buffer.  YOU CAN ONLY USE IT ONCE!
 */
    char_u *
tv_get_string_chk(typval_T *varp)
{
    స్థిర char_u   mybuf[NUMBUFLEN];

    తిరిగి tv_get_string_buf_chk(varp, mybuf);
}

    char_u *
tv_get_string_buf_chk(typval_T *varp, char_u *buf)
{
    తిరిగి tv_get_string_buf_chk_strict(varp, buf, FALSE);
}

    char_u *
tv_get_string_buf_chk_strict(typval_T *varp, char_u *buf, పూర్ణాంకం strict)
{
    ఎంపిక (varp->v_type)
    {
	స్థితి VAR_సంఖ్య:
	    ఒకవేళ (strict)
	    {
		emsg(_(e_using_సంఖ్య_as_string));
		ఆపు;
	    }
	    vim_snprintf((అక్షరం *)buf, NUMBUFLEN, "%lld",
					    (varnumber_T)varp->vval.v_సంఖ్య);
	    తిరిగి buf;
	స్థితి VAR_FUNC:
	స్థితి VAR_PARTIAL:
	    emsg(_(e_using_funcref_as_string));
	    ఆపు;
	స్థితి VAR_LIST:
	    emsg(_(e_using_list_as_string));
	    ఆపు;
	స్థితి VAR_TUPLE:
	    emsg(_(e_using_tuple_as_string));
	    ఆపు;
	స్థితి VAR_DICT:
	    emsg(_(e_using_dictionary_as_string));
	    ఆపు;
	స్థితి VAR_FLOAT:
	    ఒకవేళ (strict)
	    {
		emsg(_(e_using_float_as_string));
		ఆపు;
	    }
	    vim_snprintf((అక్షరం *)buf, NUMBUFLEN, "%g", varp->vval.v_float);
	    తిరిగి buf;
	స్థితి VAR_STRING:
	    ఒకవేళ (varp->vval.v_string != ఖాళీ)
		తిరిగి varp->vval.v_string;
	    తిరిగి (char_u *)"";
	స్థితి VAR_BOOL:
	స్థితి VAR_SPECIAL:
	    STRCPY(buf, get_var_special_name(varp->vval.v_సంఖ్య));
	    తిరిగి buf;
	స్థితి VAR_BLOB:
	    emsg(_(e_using_blob_as_string));
	    ఆపు;
	స్థితి VAR_CLASS:
	స్థితి VAR_TYPEALIAS:
	    check_typval_is_విలువ(varp);
	    ఆపు;
	స్థితి VAR_OBJECT:
	    {
		ఒకవేళ (varp->vval.v_object == ఖాళీ)
		    emsg(_(e_using_object_as_string));
		లేకపోతే
		{
		    class_T *cl = varp->vval.v_object->obj_class;
		    ఒకవేళ (cl != ఖాళీ && IS_ENUM(cl))
			semsg(_(e_using_enum_str_as_string), cl->class_name.string);
		    లేకపోతే
			emsg(_(e_using_object_as_string));
		}
	    }
	    ఆపు;
	స్థితి VAR_పని:
#నిర్వచితమైతే FEAT_పని_CHANNEL
	    ఒకవేళ (లోపల_vim9script())
	    {
		semsg(_(e_using_invalid_విలువ_as_string_str), "job");
		ఆపు;
	    }
	    తిరిగి పని_గమ్యం_string_buf(varp, buf);
#అయితే_ముగింపు
	    ఆపు;
	స్థితి VAR_CHANNEL:
#నిర్వచితమైతే FEAT_పని_CHANNEL
	    ఒకవేళ (లోపల_vim9script())
	    {
		semsg(_(e_using_invalid_విలువ_as_string_str), "channel");
		ఆపు;
	    }
	    తిరిగి channel_గమ్యం_string_buf(varp, buf);
#అయితే_ముగింపు
	    ఆపు;
	స్థితి VAR_VOID:
	    emsg(_(e_cannot_use_void_విలువ));
	    ఆపు;
	స్థితి VAR_UNKNOWN:
	స్థితి VAR_ANY:
	స్థితి VAR_INSTR:
	    semsg(_(e_using_invalid_విలువ_as_string_str),
						  vartype_name(varp->v_type));
	    ఆపు;
    }
    తిరిగి ఖాళీ;
}

/*
 * Turn a typeval into a string.  Similar to tv_get_string_buf() but uses
 * string() on Dict, List, etc.
 */
    char_u *
tv_stringify(typval_T *varp, char_u *buf)
{
    ఒకవేళ (varp->v_type == VAR_LIST
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
	తిరిగి tmp.vval.v_string;
    }
    తిరిగి tv_get_string_buf(varp, buf);
}

/*
 * Return TRUE if typeval "tv" and its value are set to be locked (immutable).
 * Also give an error message, using "name" or _("name") when use_gettext is
 * TRUE.
 */
    పూర్ణాంకం
tv_check_తాళం(typval_T *tv, char_u *name, పూర్ణాంకం use_gettext)
{
    పూర్ణాంకం	తాళం = 0;

    ఎంపిక (tv->v_type)
    {
	స్థితి VAR_BLOB:
	    ఒకవేళ (tv->vval.v_blob != ఖాళీ)
		తాళం = tv->vval.v_blob->bv_తాళం;
	    ఆపు;
	స్థితి VAR_LIST:
	    ఒకవేళ (tv->vval.v_list != ఖాళీ)
		తాళం = tv->vval.v_list->lv_తాళం;
	    ఆపు;
	స్థితి VAR_TUPLE:
	    ఒకవేళ (tv->vval.v_tuple != ఖాళీ)
		తాళం = tv->vval.v_tuple->tv_తాళం;
	    ఆపు;
	స్థితి VAR_DICT:
	    ఒకవేళ (tv->vval.v_dict != ఖాళీ)
		తాళం = tv->vval.v_dict->dv_తాళం;
	    ఆపు;
	అప్రమేయ:
	    ఆపు;
    }
    తిరిగి విలువ_check_తాళం(tv->v_తాళం, name, use_gettext)
		   || (తాళం != 0 && విలువ_check_తాళం(తాళం, name, use_gettext));
}

/*
 * Copy the values from typval_T "from" to typval_T "to".
 * When needed allocates string or increases reference count.
 * Does not make a copy of a list, blob or dict but copies the reference!
 * It is OK for "from" and "to" to point to the same item.  This is used to
 * make a copy later.
 */
    పూర్ణాంకం
copy_tv(typval_T *from, typval_T *గమ్యం)
{
    పూర్ణాంకం		ret = OK;

    గమ్యం->v_type = from->v_type;
    గమ్యం->v_తాళం = 0;
    ఎంపిక (from->v_type)
    {
	స్థితి VAR_సంఖ్య:
	స్థితి VAR_BOOL:
	స్థితి VAR_SPECIAL:
	    గమ్యం->vval.v_సంఖ్య = from->vval.v_సంఖ్య;
	    ఆపు;
	స్థితి VAR_FLOAT:
	    గమ్యం->vval.v_float = from->vval.v_float;
	    ఆపు;
	స్థితి VAR_పని:
#నిర్వచితమైతే FEAT_పని_CHANNEL
	    గమ్యం->vval.v_పని = from->vval.v_పని;
	    ఒకవేళ (గమ్యం->vval.v_పని != ఖాళీ)
		++గమ్యం->vval.v_పని->jv_refcount;
	    ఆపు;
#అయితే_ముగింపు
	స్థితి VAR_CHANNEL:
#నిర్వచితమైతే FEAT_పని_CHANNEL
	    గమ్యం->vval.v_channel = from->vval.v_channel;
	    ఒకవేళ (గమ్యం->vval.v_channel != ఖాళీ)
		++గమ్యం->vval.v_channel->ch_refcount;
	    ఆపు;
#అయితే_ముగింపు
	స్థితి VAR_INSTR:
	    గమ్యం->vval.v_instr = from->vval.v_instr;
	    ఆపు;

	స్థితి VAR_CLASS:
	    copy_class(from, గమ్యం);
	    ఆపు;

	స్థితి VAR_OBJECT:
	    copy_object(from, గమ్యం);
	    ఆపు;

	స్థితి VAR_STRING:
	స్థితి VAR_FUNC:
	    ఒకవేళ (from->vval.v_string == ఖాళీ)
		గమ్యం->vval.v_string = ఖాళీ;
	    లేకపోతే
	    {
		గమ్యం->vval.v_string = vim_strsave(from->vval.v_string);
		ఒకవేళ (from->v_type == VAR_FUNC)
		    func_ref(గమ్యం->vval.v_string);
	    }
	    ఆపు;
	స్థితి VAR_PARTIAL:
	    ఒకవేళ (from->vval.v_partial == ఖాళీ)
		గమ్యం->vval.v_partial = ఖాళీ;
	    లేకపోతే
	    {
		గమ్యం->vval.v_partial = from->vval.v_partial;
		++గమ్యం->vval.v_partial->pt_refcount;
	    }
	    ఆపు;
	స్థితి VAR_BLOB:
	    ఒకవేళ (from->vval.v_blob == ఖాళీ)
		గమ్యం->vval.v_blob = ఖాళీ;
	    లేకపోతే
	    {
		గమ్యం->vval.v_blob = from->vval.v_blob;
		++గమ్యం->vval.v_blob->bv_refcount;
	    }
	    ఆపు;
	స్థితి VAR_LIST:
	    ఒకవేళ (from->vval.v_list == ఖాళీ)
		గమ్యం->vval.v_list = ఖాళీ;
	    లేకపోతే
	    {
		గమ్యం->vval.v_list = from->vval.v_list;
		++గమ్యం->vval.v_list->lv_refcount;
	    }
	    ఆపు;
	స్థితి VAR_TUPLE:
	    ఒకవేళ (from->vval.v_tuple == ఖాళీ)
		గమ్యం->vval.v_tuple = ఖాళీ;
	    లేకపోతే
	    {
		గమ్యం->vval.v_tuple = from->vval.v_tuple;
		++గమ్యం->vval.v_tuple->tv_refcount;
	    }
	    ఆపు;
	స్థితి VAR_DICT:
	    ఒకవేళ (from->vval.v_dict == ఖాళీ)
		గమ్యం->vval.v_dict = ఖాళీ;
	    లేకపోతే
	    {
		గమ్యం->vval.v_dict = from->vval.v_dict;
		++గమ్యం->vval.v_dict->dv_refcount;
	    }
	    ఆపు;
	స్థితి VAR_TYPEALIAS:
	    ఒకవేళ (from->vval.v_typealias == ఖాళీ)
		గమ్యం->vval.v_typealias = ఖాళీ;
	    లేకపోతే
	    {
		గమ్యం->vval.v_typealias = from->vval.v_typealias;
		++గమ్యం->vval.v_typealias->ta_refcount;
	    }
	    ఆపు;
	స్థితి VAR_VOID:
	    emsg(_(e_cannot_use_void_విలువ));
	    ret = FAIL;
	    ఆపు;
	స్థితి VAR_UNKNOWN:
	స్థితి VAR_ANY:
	    అంతర్గత_error_no_abort("copy_tv(UNKNOWN)");
	    ret = FAIL;
	    ఆపు;
    }

    తిరిగి ret;
}

/*
 * Compare "tv1" and "tv2".
 */
    పూర్ణాంకం
typval_compare2(
    typval_T	*tv1,	// first operand
    typval_T	*tv2,	// second operand
    exprtype_T	type,	// operator
    పూర్ణాంకం		ic,	// ignore case
    పూర్ణాంకం		*res)	// comparison result
{
    varnumber_T	n1, n2;
    పూర్ణాంకం		type_is = type == సమాసం_IS || type == సమాసం_ISNOT;

    ఒకవేళ (check_typval_is_విలువ(tv1) == FAIL
	|| check_typval_is_విలువ(tv2) == FAIL)
	తిరిగి FAIL;
    లేకపోతే ఒకవేళ (type_is && tv1->v_type != tv2->v_type)
    {
	// For "is" a different type always means FALSE, for "isnot"
	// it means TRUE.
	*res = (type == సమాసం_ISNOT);
    }
    లేకపోతే ఒకవేళ (((tv1->v_type == VAR_SPECIAL && tv1->vval.v_సంఖ్య == VVAL_NULL)
		|| (tv2->v_type == VAR_SPECIAL
					   && tv2->vval.v_సంఖ్య == VVAL_NULL))
	    && tv1->v_type != tv2->v_type
	    && (type == సమాసం_EQUAL || type == సమాసం_NEQUAL))
    {
	n1 = typval_compare_null(tv1, tv2);
	ఒకవేళ (n1 == MAYBE)
	    తిరిగి FAIL;
	ఒకవేళ (type == సమాసం_NEQUAL)
	    n1 = !n1;
	*res = n1;
    }
    లేకపోతే ఒకవేళ (tv1->v_type == VAR_BLOB || tv2->v_type == VAR_BLOB)
    {
	ఒకవేళ (typval_compare_blob(tv1, tv2, type, res) == FAIL)
	    తిరిగి FAIL;
    }
    లేకపోతే ఒకవేళ (tv1->v_type == VAR_LIST || tv2->v_type == VAR_LIST)
    {
	ఒకవేళ (typval_compare_list(tv1, tv2, type, ic, res) == FAIL)
	    తిరిగి FAIL;
    }
    లేకపోతే ఒకవేళ (tv1->v_type == VAR_TUPLE || tv2->v_type == VAR_TUPLE)
    {
	ఒకవేళ (typval_compare_tuple(tv1, tv2, type, ic, res) == FAIL)
	    తిరిగి FAIL;
    }
    లేకపోతే ఒకవేళ (tv1->v_type == VAR_OBJECT || tv2->v_type == VAR_OBJECT)
    {
	ఒకవేళ (typval_compare_object(tv1, tv2, type, ic, res) == FAIL)
	    తిరిగి FAIL;
    }
    లేకపోతే ఒకవేళ (tv1->v_type == VAR_DICT || tv2->v_type == VAR_DICT)
    {
	ఒకవేళ (typval_compare_dict(tv1, tv2, type, ic, res) == FAIL)
	    తిరిగి FAIL;
    }
    లేకపోతే ఒకవేళ (tv1->v_type == VAR_FUNC || tv2->v_type == VAR_FUNC
	|| tv1->v_type == VAR_PARTIAL || tv2->v_type == VAR_PARTIAL)
    {
	ఒకవేళ (typval_compare_func(tv1, tv2, type, ic, res) == FAIL)
	    తిరిగి FAIL;
    }

    // If one of the two variables is a float, compare as a float.
    // When using "=~" or "!~", always compare as string.
    లేకపోతే ఒకవేళ ((tv1->v_type == VAR_FLOAT || tv2->v_type == VAR_FLOAT)
	    && type != సమాసం_సరిపోలిక && type != సమాసం_NOMATCH)
    {
	float_T f1, f2;
	పూర్ణాంకం	error = FALSE;

	f1 = tv_get_float_chk(tv1, &error);
	ఒకవేళ (!error)
	    f2 = tv_get_float_chk(tv2, &error);
	ఒకవేళ (error)
	    తిరిగి FAIL;
	n1 = FALSE;
	ఎంపిక (type)
	{
	    స్థితి సమాసం_IS:
	    స్థితి సమాసం_EQUAL:    n1 = (f1 == f2); ఆపు;
	    స్థితి సమాసం_ISNOT:
	    స్థితి సమాసం_NEQUAL:   n1 = (f1 != f2); ఆపు;
	    స్థితి సమాసం_GREATER:  n1 = (f1 > f2); ఆపు;
	    స్థితి సమాసం_GEQUAL:   n1 = (f1 >= f2); ఆపు;
	    స్థితి సమాసం_SMALLER:  n1 = (f1 < f2); ఆపు;
	    స్థితి సమాసం_SEQUAL:   n1 = (f1 <= f2); ఆపు;
	    స్థితి సమాసం_UNKNOWN:
	    స్థితి సమాసం_సరిపోలిక:
	    అప్రమేయ:  ఆపు;  // avoid gcc warning
	}
	*res = n1;
    }

    // If one of the two variables is a number, compare as a number.
    // When using "=~" or "!~", always compare as string.
    లేకపోతే ఒకవేళ ((tv1->v_type == VAR_సంఖ్య || tv2->v_type == VAR_సంఖ్య)
	    && type != సమాసం_సరిపోలిక && type != సమాసం_NOMATCH)
    {
	పూర్ణాంకం error = FALSE;

	n1 = tv_get_సంఖ్య_chk(tv1, &error);
	ఒకవేళ (!error)
	    n2 = tv_get_సంఖ్య_chk(tv2, &error);
	ఒకవేళ (error)
	    తిరిగి FAIL;
	ఎంపిక (type)
	{
	    స్థితి సమాసం_IS:
	    స్థితి సమాసం_EQUAL:    n1 = (n1 == n2); ఆపు;
	    స్థితి సమాసం_ISNOT:
	    స్థితి సమాసం_NEQUAL:   n1 = (n1 != n2); ఆపు;
	    స్థితి సమాసం_GREATER:  n1 = (n1 > n2); ఆపు;
	    స్థితి సమాసం_GEQUAL:   n1 = (n1 >= n2); ఆపు;
	    స్థితి సమాసం_SMALLER:  n1 = (n1 < n2); ఆపు;
	    స్థితి సమాసం_SEQUAL:   n1 = (n1 <= n2); ఆపు;
	    స్థితి సమాసం_UNKNOWN:
	    స్థితి సమాసం_సరిపోలిక:
	    అప్రమేయ:  ఆపు;  // avoid gcc warning
	}
	*res = n1;
    }
    లేకపోతే ఒకవేళ (లోపల_vim9script() && (tv1->v_type == VAR_BOOL
				    || tv2->v_type == VAR_BOOL
				    || (tv1->v_type == VAR_SPECIAL
					      && tv2->v_type == VAR_SPECIAL)))
    {
	ఒకవేళ (tv1->v_type != tv2->v_type)
	{
	    semsg(_(e_cannot_compare_str_with_str),
		       vartype_name(tv1->v_type), vartype_name(tv2->v_type));
	    తిరిగి FAIL;
	}
	n1 = tv1->vval.v_సంఖ్య;
	n2 = tv2->vval.v_సంఖ్య;
	ఎంపిక (type)
	{
	    స్థితి సమాసం_IS:
	    స్థితి సమాసం_EQUAL:    n1 = (n1 == n2); ఆపు;
	    స్థితి సమాసం_ISNOT:
	    స్థితి సమాసం_NEQUAL:   n1 = (n1 != n2); ఆపు;
	    అప్రమేయ:
		semsg(_(e_invalid_operation_for_str),
						   vartype_name(tv1->v_type));
		తిరిగి FAIL;
	}
	*res = n1;
    }
#నిర్వచితమైతే FEAT_పని_CHANNEL
    లేకపోతే ఒకవేళ (tv1->v_type == tv2->v_type
	    && (tv1->v_type == VAR_CHANNEL || tv1->v_type == VAR_పని)
	    && (type == సమాసం_NEQUAL || type == సమాసం_EQUAL))
    {
	ఒకవేళ (tv1->v_type == VAR_CHANNEL)
	    n1 = tv1->vval.v_channel == tv2->vval.v_channel;
	లేకపోతే
	    n1 = tv1->vval.v_పని == tv2->vval.v_పని;
	ఒకవేళ (type == సమాసం_NEQUAL)
	    n1 = !n1;
	*res = n1;
    }
#అయితే_ముగింపు
    లేకపోతే
    {
	ఒకవేళ (typval_compare_string(tv1, tv2, type, ic, res) == FAIL)
	    తిరిగి FAIL;
    }

    తిరిగి OK;
}

/*
 * Compare "tv1" and "tv2".
 * Put the result in "tv1".  Caller should clear "tv2".
 */
    పూర్ణాంకం
typval_compare(
    typval_T	*tv1,	// first operand
    typval_T	*tv2,	// second operand
    exprtype_T	type,   // operator
    పూర్ణాంకం		ic)     // ignore case
{
    పూర్ణాంకం res;

    ఒకవేళ (typval_compare2(tv1, tv2, type, ic, &res) == FAIL)
    {
	clear_tv(tv1);
	తిరిగి FAIL;
    }

    clear_tv(tv1);
    ఒకవేళ (లోపల_vim9script())
    {
	tv1->v_type = VAR_BOOL;
	tv1->vval.v_సంఖ్య = res ? VVAL_TRUE : VVAL_FALSE;
    }
    లేకపోతే
    {
	tv1->v_type = VAR_సంఖ్య;
	tv1->vval.v_సంఖ్య = res;
    }

    తిరిగి OK;
}

/*
 * Compare "tv1" to "tv2" as lists according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    పూర్ణాంకం
typval_compare_list(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	పూర్ణాంకం	    ic,
	పూర్ణాంకం	    *res)
{
    పూర్ణాంకం	    val = 0;

    ఒకవేళ (type == సమాసం_IS || type == సమాసం_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
				      && tv1->vval.v_list == tv2->vval.v_list);
	ఒకవేళ (type == సమాసం_ISNOT)
	    val = !val;
    }
    లేకపోతే ఒకవేళ (tv1->v_type != tv2->v_type
	    || (type != సమాసం_EQUAL && type != సమాసం_NEQUAL))
    {
	ఒకవేళ (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_list_with_list));
	లేకపోతే
	    emsg(_(e_invalid_operation_for_list));
	తిరిగి FAIL;
    }
    లేకపోతే
    {
	val = list_equal(tv1->vval.v_list, tv2->vval.v_list, ic);
	ఒకవేళ (type == సమాసం_NEQUAL)
	    val = !val;
    }
    *res = val;
    తిరిగి OK;
}

/*
 * Compare "tv1" to "tv2" as tuples according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    పూర్ణాంకం
typval_compare_tuple(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	పూర్ణాంకం	    ic,
	పూర్ణాంకం	    *res)
{
    పూర్ణాంకం	    val = 0;

    ఒకవేళ (type == సమాసం_IS || type == సమాసం_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
			      && tv1->vval.v_tuple == tv2->vval.v_tuple);
	ఒకవేళ (type == సమాసం_ISNOT)
	    val = !val;
    }
    లేకపోతే ఒకవేళ (tv1->v_type != tv2->v_type
	    || (type != సమాసం_EQUAL && type != సమాసం_NEQUAL))
    {
	ఒకవేళ (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_tuple_with_tuple));
	లేకపోతే
	    emsg(_(e_invalid_operation_for_tuple));
	తిరిగి FAIL;
    }
    లేకపోతే
    {
	val = tuple_equal(tv1->vval.v_tuple, tv2->vval.v_tuple, ic);
	ఒకవేళ (type == సమాసం_NEQUAL)
	    val = !val;
    }
    *res = val;
    తిరిగి OK;
}

/*
 * Compare v:null with another type.  Return TRUE if the value is NULL.
 */
    పూర్ణాంకం
typval_compare_null(typval_T *tv1, typval_T *tv2)
{
    ఒకవేళ ((tv1->v_type == VAR_SPECIAL && tv1->vval.v_సంఖ్య == VVAL_NULL)
	    || (tv2->v_type == VAR_SPECIAL && tv2->vval.v_సంఖ్య == VVAL_NULL))
    {
	typval_T	*tv = tv1->v_type == VAR_SPECIAL ? tv2 : tv1;

	ఎంపిక (tv->v_type)
	{
	    స్థితి VAR_BLOB: తిరిగి tv->vval.v_blob == ఖాళీ;
#నిర్వచితమైతే FEAT_పని_CHANNEL
	    స్థితి VAR_CHANNEL: తిరిగి tv->vval.v_channel == ఖాళీ;
#అయితే_ముగింపు
	    // TODO: null_class handling
	    // case VAR_CLASS: return tv->vval.v_class == NULL;
	    స్థితి VAR_DICT: తిరిగి tv->vval.v_dict == ఖాళీ;
	    స్థితి VAR_FUNC: తిరిగి tv->vval.v_string == ఖాళీ;
#నిర్వచితమైతే FEAT_పని_CHANNEL
	    స్థితి VAR_పని: తిరిగి tv->vval.v_పని == ఖాళీ;
#అయితే_ముగింపు
	    స్థితి VAR_LIST: తిరిగి tv->vval.v_list == ఖాళీ;
	    స్థితి VAR_TUPLE: తిరిగి tv->vval.v_tuple == ఖాళీ;
	    స్థితి VAR_OBJECT: తిరిగి tv->vval.v_object == ఖాళీ;
	    స్థితి VAR_PARTIAL: తిరిగి tv->vval.v_partial == ఖాళీ;
	    స్థితి VAR_STRING: తిరిగి tv->vval.v_string == ఖాళీ;

	    స్థితి VAR_సంఖ్య: ఒకవేళ (!లోపల_vim9script())
				 తిరిగి tv->vval.v_సంఖ్య == 0;
			     ఆపు;
	    స్థితి VAR_FLOAT: ఒకవేళ (!లోపల_vim9script())
				 తిరిగి tv->vval.v_float == 0.0;
			     ఆపు;
	    స్థితి VAR_TYPEALIAS: తిరిగి tv->vval.v_typealias == ఖాళీ;
	    అప్రమేయ: ఆపు;
	}
    }
    // although comparing null with number, float or bool is not very useful
    // we won't give an error
    తిరిగి FALSE;
}

/*
 * Compare "tv1" to "tv2" as blobs according to "type".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    పూర్ణాంకం
typval_compare_blob(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	పూర్ణాంకం	    *res)
{
    పూర్ణాంకం	    val = 0;

    ఒకవేళ (type == సమాసం_IS || type == సమాసం_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
			&& tv1->vval.v_blob == tv2->vval.v_blob);
	ఒకవేళ (type == సమాసం_ISNOT)
	    val = !val;
    }
    లేకపోతే ఒకవేళ (tv1->v_type != tv2->v_type
	    || (type != సమాసం_EQUAL && type != సమాసం_NEQUAL))
    {
	ఒకవేళ (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_blob_with_blob));
	లేకపోతే
	    emsg(_(e_invalid_operation_for_blob));
	తిరిగి FAIL;
    }
    లేకపోతే
    {
	val = blob_equal(tv1->vval.v_blob, tv2->vval.v_blob);
	ఒకవేళ (type == సమాసం_NEQUAL)
	    val = !val;
    }
    *res = val;
    తిరిగి OK;
}

/*
 * Compare "tv1" to "tv2" as objects according to "type".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    పూర్ణాంకం
typval_compare_object(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	పూర్ణాంకం	    ic,
	పూర్ణాంకం	    *res)
{
    పూర్ణాంకం res_సరిపోలిక = type == సమాసం_EQUAL || type == సమాసం_IS ? TRUE : FALSE;

    ఒకవేళ (tv1->vval.v_object == ఖాళీ && tv2->vval.v_object == ఖాళీ)
    {
	*res = res_సరిపోలిక;
	తిరిగి OK;
    }
    ఒకవేళ (tv1->vval.v_object == ఖాళీ || tv2->vval.v_object == ఖాళీ)
    {
	*res = !res_సరిపోలిక;
	తిరిగి OK;
    }

    object_T *obj1 = tv1->vval.v_object;
    object_T *obj2 = tv2->vval.v_object;
    ఒకవేళ (type == సమాసం_IS || type == సమాసం_ISNOT)
    {
	*res = obj1 == obj2 ? res_సరిపోలిక : !res_సరిపోలిక;
	తిరిగి OK;
    }
    లేకపోతే ఒకవేళ (tv1->v_type != tv2->v_type)
    {
	emsg(_(e_can_only_compare_object_with_object));
	తిరిగి FAIL;
    }

    *res = object_equal(obj1, obj2, ic) ? res_సరిపోలిక : !res_సరిపోలిక;
    తిరిగి OK;
}

/*
 * Compare "tv1" to "tv2" as dictionaries according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    పూర్ణాంకం
typval_compare_dict(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	పూర్ణాంకం	    ic,
	పూర్ణాంకం	    *res)
{
    పూర్ణాంకం	    val;

    ఒకవేళ (type == సమాసం_IS || type == సమాసం_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
			&& tv1->vval.v_dict == tv2->vval.v_dict);
	ఒకవేళ (type == సమాసం_ISNOT)
	    val = !val;
    }
    లేకపోతే ఒకవేళ (tv1->v_type != tv2->v_type
		|| (type != సమాసం_EQUAL && type != సమాసం_NEQUAL))
    {
	ఒకవేళ (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_dictionary_with_dictionary));
	లేకపోతే
	    emsg(_(e_invalid_operation_for_dictionary));
	తిరిగి FAIL;
    }
    లేకపోతే
    {
	val = dict_equal(tv1->vval.v_dict, tv2->vval.v_dict, ic);
	ఒకవేళ (type == సమాసం_NEQUAL)
	    val = !val;
    }
    *res = val;
    తిరిగి OK;
}

/*
 * Compare "tv1" to "tv2" as funcrefs according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    పూర్ణాంకం
typval_compare_func(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	పూర్ణాంకం	    ic,
	పూర్ణాంకం	    *res)
{
    పూర్ణాంకం	    val = 0;

    ఒకవేళ (type != సమాసం_EQUAL && type != సమాసం_NEQUAL
	    && type != సమాసం_IS && type != సమాసం_ISNOT)
    {
	emsg(_(e_invalid_operation_for_funcrefs));
	తిరిగి FAIL;
    }
    ఒకవేళ ((tv1->v_type == VAR_PARTIAL && tv1->vval.v_partial == ఖాళీ)
	    || (tv2->v_type == VAR_PARTIAL && tv2->vval.v_partial == ఖాళీ))
	// When both partials are NULL, then they are equal.
	// Otherwise they are not equal.
	val = (tv1->vval.v_partial == tv2->vval.v_partial);
    లేకపోతే ఒకవేళ (type == సమాసం_IS || type == సమాసం_ISNOT)
    {
	ఒకవేళ (tv1->v_type == VAR_FUNC && tv2->v_type == VAR_FUNC)
	    // strings are considered the same if their value is
	    // the same
	    val = tv_equal(tv1, tv2, ic);
	లేకపోతే ఒకవేళ (tv1->v_type == VAR_PARTIAL && tv2->v_type == VAR_PARTIAL)
	    val = (tv1->vval.v_partial == tv2->vval.v_partial);
	లేకపోతే
	    val = FALSE;
    }
    లేకపోతే
	val = tv_equal(tv1, tv2, ic);
    ఒకవేళ (type == సమాసం_NEQUAL || type == సమాసం_ISNOT)
	val = !val;
    *res = val;
    తిరిగి OK;
}

/*
 * Compare "tv1" to "tv2" as strings according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    పూర్ణాంకం
typval_compare_string(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	పూర్ణాంకం	    ic,
	పూర్ణాంకం	    *res)
{
    పూర్ణాంకం		i = 0;
    పూర్ణాంకం		val = FALSE;
    char_u	*s1, *s2;
    char_u	buf1[NUMBUFLEN], buf2[NUMBUFLEN];

    ఒకవేళ (లోపల_vim9script()
	  && ((tv1->v_type != VAR_STRING && tv1->v_type != VAR_SPECIAL)
	   || (tv2->v_type != VAR_STRING && tv2->v_type != VAR_SPECIAL)))
    {
	semsg(_(e_cannot_compare_str_with_str),
		   vartype_name(tv1->v_type), vartype_name(tv2->v_type));
	తిరిగి FAIL;
    }
    s1 = tv_get_string_buf(tv1, buf1);
    s2 = tv_get_string_buf(tv2, buf2);
    ఒకవేళ (type != సమాసం_సరిపోలిక && type != సమాసం_NOMATCH)
	i = ic ? MB_STRICMP(s1, s2) : STRCMP(s1, s2);
    ఎంపిక (type)
    {
	స్థితి సమాసం_IS:	    ఒకవేళ (లోపల_vim9script())
			    {
				// Really check it is the same string, not just
				// the same value.
				val = tv1->vval.v_string == tv2->vval.v_string;
				ఆపు;
			    }
			    // FALLTHROUGH
	స్థితి సమాసం_EQUAL:    val = (i == 0); ఆపు;
	స్థితి సమాసం_ISNOT:    ఒకవేళ (లోపల_vim9script())
			    {
				// Really check it is not the same string, not
				// just a different value.
				val = tv1->vval.v_string != tv2->vval.v_string;
				ఆపు;
			    }
			    // FALLTHROUGH
	స్థితి సమాసం_NEQUAL:   val = (i != 0); ఆపు;
	స్థితి సమాసం_GREATER:  val = (i > 0); ఆపు;
	స్థితి సమాసం_GEQUAL:   val = (i >= 0); ఆపు;
	స్థితి సమాసం_SMALLER:  val = (i < 0); ఆపు;
	స్థితి సమాసం_SEQUAL:   val = (i <= 0); ఆపు;

	స్థితి సమాసం_సరిపోలిక:
	స్థితి సమాసం_NOMATCH:
		val = నమూనా_సరిపోలిక(s2, s1, ic);
		ఒకవేళ (type == సమాసం_NOMATCH)
		    val = !val;
		ఆపు;

	అప్రమేయ:  ఆపు;  // avoid gcc warning
    }
    *res = val;
    తిరిగి OK;
}
/*
 * Convert any type to a string, never give an error.
 * When "quotes" is TRUE add quotes to a string.
 * Returns an allocated string.
 */
    char_u *
typval_tostring(typval_T *arg, పూర్ణాంకం quotes)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    char_u	*ret = ఖాళీ;

    ఒకవేళ (arg == ఖాళీ)
	తిరిగి vim_strsave((char_u *)"(does not exist)");
    ఒకవేళ (!quotes && arg->v_type == VAR_STRING)
    {
	ret = vim_strsave(arg->vval.v_string == ఖాళీ ? (char_u *)""
							 : arg->vval.v_string);
    }
    లేకపోతే
    {
	ret = tv2string(arg, &tofree, numbuf, 0);
	// Make a copy if we have a value but it's not in allocated memory.
	ఒకవేళ (ret != ఖాళీ && tofree == ఖాళీ)
	    ret = vim_strsave(ret);
    }
    తిరిగి ret;
}

/*
 * Return TRUE if typeval "tv" is locked: Either that value is locked itself
 * or it refers to a List or Dictionary that is locked.
 */
    పూర్ణాంకం
tv_islocked(typval_T *tv)
{
    తిరిగి (tv->v_తాళం & VAR_LOCKED)
	|| (tv->v_type == VAR_LIST
		&& tv->vval.v_list != ఖాళీ
		&& (tv->vval.v_list->lv_తాళం & VAR_LOCKED))
	|| (tv->v_type == VAR_TUPLE
		&& tv->vval.v_tuple != ఖాళీ
		&& (tv->vval.v_tuple->tv_తాళం & VAR_LOCKED))
	|| (tv->v_type == VAR_DICT
		&& tv->vval.v_dict != ఖాళీ
		&& (tv->vval.v_dict->dv_తాళం & VAR_LOCKED));
}

    స్థిర పూర్ణాంకం
func_equal(
    typval_T *tv1,
    typval_T *tv2,
    పూర్ణాంకం	     ic)	    // ignore case
{
    char_u	*s1, *s2;
    dict_T	*d1, *d2;
    పూర్ణాంకం		a1, a2;
    పూర్ణాంకం		i;

    // empty and NULL function name considered the same
    s1 = tv1->v_type == VAR_FUNC ? tv1->vval.v_string
					   : partial_name(tv1->vval.v_partial);
    ఒకవేళ (s1 != ఖాళీ && *s1 == NUL)
	s1 = ఖాళీ;
    s2 = tv2->v_type == VAR_FUNC ? tv2->vval.v_string
					   : partial_name(tv2->vval.v_partial);
    ఒకవేళ (s2 != ఖాళీ && *s2 == NUL)
	s2 = ఖాళీ;
    ఒకవేళ (s1 == ఖాళీ || s2 == ఖాళీ)
    {
	ఒకవేళ (s1 != s2)
	    తిరిగి FALSE;
    }
    లేకపోతే ఒకవేళ (STRCMP(s1, s2) != 0)
	తిరిగి FALSE;

    // empty dict and NULL dict is different
    d1 = tv1->v_type == VAR_FUNC ? ఖాళీ : tv1->vval.v_partial->pt_dict;
    d2 = tv2->v_type == VAR_FUNC ? ఖాళీ : tv2->vval.v_partial->pt_dict;
    ఒకవేళ (d1 == ఖాళీ || d2 == ఖాళీ)
    {
	ఒకవేళ (d1 != d2)
	    తిరిగి FALSE;
    }
    లేకపోతే ఒకవేళ (!dict_equal(d1, d2, ic))
	తిరిగి FALSE;

    // empty list and no list considered the same
    a1 = tv1->v_type == VAR_FUNC ? 0 : tv1->vval.v_partial->pt_argc;
    a2 = tv2->v_type == VAR_FUNC ? 0 : tv2->vval.v_partial->pt_argc;
    ఒకవేళ (a1 != a2)
	తిరిగి FALSE;
    కొరకు (i = 0; i < a1; ++i)
	ఒకవేళ (!tv_equal(tv1->vval.v_partial->pt_argv + i,
		      tv2->vval.v_partial->pt_argv + i, ic))
	    తిరిగి FALSE;

    తిరిగి TRUE;
}

/*
 * Return TRUE if "tv1" and "tv2" have the same value.
 * Compares the items just like "==" would compare them, but strings and
 * numbers are different.  Floats and numbers are also different.
 */
    పూర్ణాంకం
tv_equal(
    typval_T *tv1,
    typval_T *tv2,
    పూర్ణాంకం	     ic)	    // ignore case
{
    char_u	buf1[NUMBUFLEN], buf2[NUMBUFLEN];
    char_u	*s1, *s2;
    స్థిర పూర్ణాంకం  recursive_cnt = 0;	    // catch recursive loops
    పూర్ణాంకం		r;
    స్థిర పూర్ణాంకం	tv_equal_recurse_హద్దు;

    // Catch lists and dicts that have an endless loop by limiting
    // recursiveness to a limit.  We guess they are equal then.
    // A fixed limit has the problem of still taking an awful long time.
    // Reduce the limit every time running into it. That should work fine for
    // deeply linked structures that are not recursively linked and catch
    // recursiveness quickly.
    ఒకవేళ (recursive_cnt == 0)
	tv_equal_recurse_హద్దు = 1000;
    ఒకవేళ (recursive_cnt >= tv_equal_recurse_హద్దు)
    {
	--tv_equal_recurse_హద్దు;
	తిరిగి TRUE;
    }

    // For VAR_FUNC and VAR_PARTIAL compare the function name, bound dict and
    // arguments.
    ఒకవేళ ((tv1->v_type == VAR_FUNC
		|| (tv1->v_type == VAR_PARTIAL && tv1->vval.v_partial != ఖాళీ))
	    && (tv2->v_type == VAR_FUNC
		|| (tv2->v_type == VAR_PARTIAL && tv2->vval.v_partial != ఖాళీ)))
    {
	++recursive_cnt;
	r = func_equal(tv1, tv2, ic);
	--recursive_cnt;
	తిరిగి r;
    }

    ఒకవేళ (tv1->v_type != tv2->v_type
	    && ((tv1->v_type != VAR_BOOL && tv1->v_type != VAR_SPECIAL)
		|| (tv2->v_type != VAR_BOOL && tv2->v_type != VAR_SPECIAL)))
	తిరిగి FALSE;

    ఎంపిక (tv1->v_type)
    {
	స్థితి VAR_LIST:
	    ++recursive_cnt;
	    r = list_equal(tv1->vval.v_list, tv2->vval.v_list, ic);
	    --recursive_cnt;
	    తిరిగి r;

	స్థితి VAR_TUPLE:
	    ++recursive_cnt;
	    r = tuple_equal(tv1->vval.v_tuple, tv2->vval.v_tuple, ic);
	    --recursive_cnt;
	    తిరిగి r;

	స్థితి VAR_DICT:
	    ++recursive_cnt;
	    r = dict_equal(tv1->vval.v_dict, tv2->vval.v_dict, ic);
	    --recursive_cnt;
	    తిరిగి r;

	స్థితి VAR_BLOB:
	    తిరిగి blob_equal(tv1->vval.v_blob, tv2->vval.v_blob);

	స్థితి VAR_సంఖ్య:
	స్థితి VAR_BOOL:
	స్థితి VAR_SPECIAL:
	    తిరిగి tv1->vval.v_సంఖ్య == tv2->vval.v_సంఖ్య;

	స్థితి VAR_STRING:
	    s1 = tv_get_string_buf(tv1, buf1);
	    s2 = tv_get_string_buf(tv2, buf2);
	    తిరిగి ((ic ? MB_STRICMP(s1, s2) : STRCMP(s1, s2)) == 0);

	స్థితి VAR_FLOAT:
	    తిరిగి tv1->vval.v_float == tv2->vval.v_float;
	స్థితి VAR_పని:
#నిర్వచితమైతే FEAT_పని_CHANNEL
	    తిరిగి tv1->vval.v_పని == tv2->vval.v_పని;
#అయితే_ముగింపు
	స్థితి VAR_CHANNEL:
#నిర్వచితమైతే FEAT_పని_CHANNEL
	    తిరిగి tv1->vval.v_channel == tv2->vval.v_channel;
#అయితే_ముగింపు
	స్థితి VAR_INSTR:
	    తిరిగి tv1->vval.v_instr == tv2->vval.v_instr;

	స్థితి VAR_CLASS:
	    // A class only exists once, equality is identity.
	    తిరిగి tv1->vval.v_class == tv2->vval.v_class;

	స్థితి VAR_OBJECT:
	    ++recursive_cnt;
	    r = object_equal(tv1->vval.v_object, tv2->vval.v_object, ic);
	    --recursive_cnt;
	    తిరిగి r;

	స్థితి VAR_PARTIAL:
	    తిరిగి tv1->vval.v_partial == tv2->vval.v_partial;

	స్థితి VAR_FUNC:
	    తిరిగి tv1->vval.v_string == tv2->vval.v_string;

	స్థితి VAR_TYPEALIAS:
	    తిరిగి tv1->vval.v_typealias == tv2->vval.v_typealias;

	స్థితి VAR_UNKNOWN:
	స్థితి VAR_ANY:
	స్థితి VAR_VOID:
	    ఆపు;
    }

    // VAR_UNKNOWN can be the result of a invalid expression, let's say it
    // does not equal anything, not even itself.
    తిరిగి FALSE;
}

/*
 * Get an option value.
 * "arg" points to the '&' or '+' before the option name.
 * "arg" is advanced to character after the option name.
 * Return OK or FAIL.
 */
    పూర్ణాంకం
eval_option(
    char_u	**arg,
    typval_T	*rettv,	// when NULL, only check if option exists
    పూర్ణాంకం		evaluate)
{
    char_u	*option_చివర;
    దీర్ఘ	numval;
    char_u	*stringval;
    getoption_T	opt_type;
    పూర్ణాంకం		c;
    పూర్ణాంకం		working = (**arg == '+');    // has("+option")
    పూర్ణాంకం		ret = OK;
    పూర్ణాంకం		scope;

    // Isolate the option name and find its value.
    option_చివర = కనుగొను_option_చివర(arg, &scope);
    ఒకవేళ (option_చివర == ఖాళీ)
    {
	ఒకవేళ (rettv != ఖాళీ)
	    semsg(_(e_option_name_missing_str), *arg);
	తిరిగి FAIL;
    }

    ఒకవేళ (!evaluate)
    {
	*arg = option_చివర;
	తిరిగి OK;
    }

    c = *option_చివర;
    *option_చివర = NUL;
    opt_type = get_option_విలువ(*arg, &numval,
			       rettv == ఖాళీ ? ఖాళీ : &stringval, ఖాళీ, scope);

    ఒకవేళ (opt_type == gov_unknown)
    {
	ఒకవేళ (rettv != ఖాళీ)
	    semsg(_(e_unknown_option_str), *arg);
	ret = FAIL;
    }
    లేకపోతే ఒకవేళ (rettv != ఖాళీ)
    {
	rettv->v_తాళం = 0;
	ఒకవేళ (opt_type == gov_hidden_string)
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = ఖాళీ;
	}
	లేకపోతే ఒకవేళ (opt_type == gov_hidden_bool || opt_type == gov_hidden_సంఖ్య)
	{
	    rettv->v_type = లోపల_vim9script() && opt_type == gov_hidden_bool
						       ? VAR_BOOL : VAR_సంఖ్య;
	    rettv->vval.v_సంఖ్య = 0;
	}
	లేకపోతే ఒకవేళ (opt_type == gov_bool || opt_type == gov_సంఖ్య)
	{
	    ఒకవేళ (లోపల_vim9script() && opt_type == gov_bool)
	    {
		rettv->v_type = VAR_BOOL;
		rettv->vval.v_సంఖ్య = numval ? VVAL_TRUE : VVAL_FALSE;
	    }
	    లేకపోతే
	    {
		rettv->v_type = VAR_సంఖ్య;
		rettv->vval.v_సంఖ్య = numval;
	    }
	}
	లేకపోతే				// string option
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = stringval;
	}
    }
    లేకపోతే ఒకవేళ (working && (opt_type == gov_hidden_bool
			|| opt_type == gov_hidden_సంఖ్య
			|| opt_type == gov_hidden_string))
	ret = FAIL;

    *option_చివర = c;		    // put back for error messages
    *arg = option_చివర;

    తిరిగి ret;
}

/*
 * Allocate a variable for a number constant.  Also deals with "0z" for blob.
 * Return OK or FAIL.
 */
    పూర్ణాంకం
eval_సంఖ్య(
	char_u	    **arg,
	typval_T    *rettv,
	పూర్ణాంకం	    evaluate,
	పూర్ణాంకం	    want_string)
{
    పూర్ణాంకం		len;
    పూర్ణాంకం		skip_quotes = !లోపల_old_script(4);
    char_u	*p;
    పూర్ణాంకం		get_float = FALSE;

    // We accept a float when the format matches
    // "[0-9]\+\.[0-9]\+\([eE][+-]\?[0-9]\+\)\?".  This is very
    // strict to avoid backwards compatibility problems.
    // With script version 2 and later the leading digit can be
    // omitted.
    // Don't look for a float after the "." operator, so that
    // ":let vers = 1.2.3" doesn't fail.
    ఒకవేళ (**arg == '.')
	p = *arg;
    లేకపోతే
    {
	p = *arg + 1;
	ఒకవేళ (skip_quotes)
	    కొరకు (;;)
	    {
		ఒకవేళ (*p == '\'')
		    ++p;
		ఒకవేళ (!vim_isdigit(*p))
		    ఆపు;
		p = skipdigits(p);
	    }
	లేకపోతే
	    p = skipdigits(p);
    }
    ఒకవేళ (!want_string && p[0] == '.' && vim_isdigit(p[1]))
    {
	get_float = TRUE;
	p = skipdigits(p + 2);
	ఒకవేళ (*p == 'e' || *p == 'E')
	{
	    ++p;
	    ఒకవేళ (*p == '-' || *p == '+')
		++p;
	    ఒకవేళ (!vim_isdigit(*p))
		get_float = FALSE;
	    లేకపోతే
		p = skipdigits(p + 1);
	}
	ఒకవేళ (ASCII_ISALPHA(*p) || *p == '.')
	    get_float = FALSE;
    }
    ఒకవేళ (get_float)
    {
	float_T	f;

	*arg += string2float(*arg, &f, skip_quotes);
	ఒకవేళ (evaluate)
	{
	    rettv->v_type = VAR_FLOAT;
	    rettv->vval.v_float = f;
	}
    }
    లేకపోతే
    ఒకవేళ (**arg == '0' && ((*arg)[1] == 'z' || (*arg)[1] == 'Z'))
    {
	char_u  *bp;
	blob_T  *blob = ఖాళీ;  // init for gcc

	// Blob constant: 0z0123456789abcdef
	ఒకవేళ (evaluate)
	    blob = blob_alloc();
	కొరకు (bp = *arg + 2; vim_isxdigit(bp[0]); bp += 2)
	{
	    ఒకవేళ (!vim_isxdigit(bp[1]))
	    {
		ఒకవేళ (blob != ఖాళీ)
		{
		    emsg(_(e_blob_literal_should_have_an_even_సంఖ్య_యొక్క_hex_characters));
		    ga_clear(&blob->bv_ga);
		    VIM_CLEAR(blob);
		}
		తిరిగి FAIL;
	    }
	    ఒకవేళ (blob != ఖాళీ)
		ga_append(&blob->bv_ga,
			     (hex2nr(*bp) << 4) + hex2nr(*(bp+1)));
	    ఒకవేళ (bp[2] == '.' && vim_isxdigit(bp[3]))
		++bp;
	}
	ఒకవేళ (blob != ఖాళీ)
	    rettv_blob_సమితి(rettv, blob);
	*arg = bp;
    }
    లేకపోతే
    {
	varnumber_T	n;

	// decimal, hex or octal number
	vim_str2nr(*arg, ఖాళీ, &len, skip_quotes
		      ? STR2NR_NO_OCT + STR2NR_QUOTE
		      : STR2NR_ALL, &n, ఖాళీ, 0, TRUE, ఖాళీ);
	ఒకవేళ (len == 0)
	{
	    ఒకవేళ (evaluate)
		semsg(_(e_invalid_వ్యక్తీకరణ_str), *arg);
	    తిరిగి FAIL;
	}
	*arg += len;
	ఒకవేళ (evaluate)
	{
	    rettv->v_type = VAR_సంఖ్య;
	    rettv->vval.v_సంఖ్య = n;
	}
    }
    తిరిగి OK;
}

/*
 * Evaluate a string constant and put the result in "rettv".
 * "*arg" points to the double quote or to after it when "interpolate" is TRUE.
 * When "interpolate" is TRUE reduce "{{" to "{", reduce "}}" to "}" and stop
 * at a single "{".
 * Return OK or FAIL.
 */
    పూర్ణాంకం
eval_string(char_u **arg, typval_T *rettv, పూర్ణాంకం evaluate, పూర్ణాంకం interpolate)
{
    char_u	*p;
    char_u	*చివర;
    పూర్ణాంకం		extra = interpolate ? 1 : 0;
    పూర్ణాంకం		off = interpolate ? 0 : 1;
    పూర్ణాంకం		len;

    // Find the end of the string, skipping backslashed characters.
    కొరకు (p = *arg + off; *p != NUL && *p != '"'; MB_PTR_ADV(p))
    {
	ఒకవేళ (*p == '\\' && p[1] != NUL)
	{
	    ++p;
	    // A "\<x>" form occupies at least 4 characters, and produces up
	    // to 9 characters (6 for the char and 3 for a modifier):
	    // reserve space for 5 extra.
	    ఒకవేళ (*p == '<')
	    {
		పూర్ణాంకం		modifiers = 0;
		పూర్ణాంకం		flags = FSK_KEYCODE | FSK_లోపల_STRING;

		extra += 5;

		// Skip to the '>' to avoid using '{' inside for string
		// interpolation.
		ఒకవేళ (p[1] != '*')
		    flags |= FSK_SIMPLIFY;
		ఒకవేళ (కనుగొను_special_కీలకం(&p, &modifiers, flags, ఖాళీ) != 0)
		    --p;  // leave "p" on the ">"
	    }
	}
	లేకపోతే ఒకవేళ (interpolate && (*p == '{' || *p == '}'))
	{
	    ఒకవేళ (*p == '{' && p[1] != '{') // start of expression
		ఆపు;
	    ++p;
	    ఒకవేళ (p[-1] == '}' && *p != '}') // single '}' is an error
	    {
		semsg(_(e_stray_closing_curly_str), *arg);
		తిరిగి FAIL;
	    }
	    --extra;  // "{{" becomes "{", "}}" becomes "}"
	}
    }

    ఒకవేళ (*p != '"' && !(interpolate && *p == '{'))
    {
	semsg(_(e_missing_double_quote_str), *arg);
	తిరిగి FAIL;
    }

    // If only parsing, set *arg and return here
    ఒకవేళ (!evaluate)
    {
	*arg = p + off;
	తిరిగి OK;
    }

    // Copy the string into allocated memory, handling backslashed
    // characters.
    rettv->v_type = VAR_STRING;
    len = (పూర్ణాంకం)(p - *arg + extra);
    rettv->vval.v_string = alloc(len);
    ఒకవేళ (rettv->vval.v_string == ఖాళీ)
	తిరిగి FAIL;
    చివర = rettv->vval.v_string;

    కొరకు (p = *arg + off; *p != NUL && *p != '"'; )
    {
	ఒకవేళ (*p == '\\')
	{
	    ఎంపిక (*++p)
	    {
		స్థితి 'b': *చివర++ = BS; ++p; ఆపు;
		స్థితి 'e': *చివర++ = ESC; ++p; ఆపు;
		స్థితి 'f': *చివర++ = FF; ++p; ఆపు;
		స్థితి 'n': *చివర++ = NL; ++p; ఆపు;
		స్థితి 'r': *చివర++ = CAR; ++p; ఆపు;
		స్థితి 't': *చివర++ = TAB; ++p; ఆపు;

		స్థితి 'X': // hex: "\x1", "\x12"
		స్థితి 'x':
		స్థితి 'u': // Unicode: "\u0023"
		స్థితి 'U':
			  ఒకవేళ (vim_isxdigit(p[1]))
			  {
			      పూర్ణాంకం	n, nr;
			      పూర్ణాంకం	c = సురక్షిత_toupper(*p);

			      ఒకవేళ (c == 'X')
				  n = 2;
			      లేకపోతే ఒకవేళ (*p == 'u')
				  n = 4;
			      లేకపోతే
				  n = 8;
			      nr = 0;
			      అయితే (--n >= 0 && vim_isxdigit(p[1]))
			      {
				  ++p;
				  nr = (nr << 4) + hex2nr(*p);
			      }
			      ++p;
			      // For "\u" store the number according to
			      // 'encoding'.
			      ఒకవేళ (c != 'X')
				  చివర += (*mb_char2bytes)(nr, చివర);
			      లేకపోతే
				  *చివర++ = nr;
			  }
			  ఆపు;

			  // octal: "\1", "\12", "\123"
		స్థితి '0':
		స్థితి '1':
		స్థితి '2':
		స్థితి '3':
		స్థితి '4':
		స్థితి '5':
		స్థితి '6':
		స్థితి '7': *చివర = *p++ - '0';
			  ఒకవేళ (*p >= '0' && *p <= '7')
			  {
			      *చివర = (*చివర << 3) + *p++ - '0';
			      ఒకవేళ (*p >= '0' && *p <= '7')
				  *చివర = (*చివర << 3) + *p++ - '0';
			  }
			  ++చివర;
			  ఆపు;

			  // Special key, e.g.: "\<C-W>"
		స్థితి '<':
			  {
			      పూర్ణాంకం flags = FSK_KEYCODE | FSK_లోపల_STRING;

			      ఒకవేళ (p[1] != '*')
				  flags |= FSK_SIMPLIFY;
			      extra = trans_special(&p, చివర, flags, FALSE, ఖాళీ);
			      ఒకవేళ (extra != 0)
			      {
				  చివర += extra;
				  ఒకవేళ (చివర >= rettv->vval.v_string + len)
				      iemsg("eval_string() used more space than allocated");
				  ఆపు;
			      }
			  }
			  // FALLTHROUGH

		అప్రమేయ: MB_COPY_CHAR(p, చివర);
			  ఆపు;
	    }
	}
	లేకపోతే
	{
	    ఒకవేళ (interpolate && (*p == '{' || *p == '}'))
	    {
		ఒకవేళ (*p == '{' && p[1] != '{') // start of expression
		    ఆపు;
		++p;  // reduce "{{" to "{" and "}}" to "}"
	    }
	    MB_COPY_CHAR(p, చివర);
	}
    }
    *చివర = NUL;
    ఒకవేళ (*p == '"' && !interpolate)
	++p;
    *arg = p;

    తిరిగి OK;
}

/*
 * Allocate a variable for a 'str''ing' constant.
 * When "interpolate" is TRUE reduce "{{" to "{" and stop at a single "{".
 * Return OK when a "rettv" was set to the string.
 * Return FAIL on error, "rettv" is not set.
 */
    పూర్ణాంకం
eval_lit_string(char_u **arg, typval_T *rettv, పూర్ణాంకం evaluate, పూర్ణాంకం interpolate)
{
    char_u	*p;
    char_u	*str;
    పూర్ణాంకం		reduce = interpolate ? -1 : 0;
    పూర్ణాంకం		off = interpolate ? 0 : 1;

    // Find the end of the string, skipping ''.
    కొరకు (p = *arg + off; *p != NUL; MB_PTR_ADV(p))
    {
	ఒకవేళ (*p == '\'')
	{
	    ఒకవేళ (p[1] != '\'')
		ఆపు;
	    ++reduce;
	    ++p;
	}
	లేకపోతే ఒకవేళ (interpolate)
	{
	    ఒకవేళ (*p == '{')
	    {
		ఒకవేళ (p[1] != '{')
		    ఆపు;
		++p;
		++reduce;
	    }
	    లేకపోతే ఒకవేళ (*p == '}')
	    {
		++p;
		ఒకవేళ (*p != '}')
		{
		    semsg(_(e_stray_closing_curly_str), *arg);
		    తిరిగి FAIL;
		}
		++reduce;
	    }
	}
    }

    ఒకవేళ (*p != '\'' && !(interpolate && *p == '{'))
    {
	semsg(_(e_missing_single_quote_str), *arg);
	తిరిగి FAIL;
    }

    // If only parsing return after setting "*arg"
    ఒకవేళ (!evaluate)
    {
	*arg = p + off;
	తిరిగి OK;
    }

    // Copy the string into allocated memory, handling '' to ' reduction and
    // any expressions.
    str = alloc((p - *arg) - reduce);
    ఒకవేళ (str == ఖాళీ)
	తిరిగి FAIL;
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = str;

    కొరకు (p = *arg + off; *p != NUL; )
    {
	ఒకవేళ (*p == '\'')
	{
	    ఒకవేళ (p[1] != '\'')
		ఆపు;
	    ++p;
	}
	లేకపోతే ఒకవేళ (interpolate && (*p == '{' || *p == '}'))
	{
	    ఒకవేళ (*p == '{' && p[1] != '{')
		ఆపు;
	    ++p;
	}
	MB_COPY_CHAR(p, str);
    }
    *str = NUL;
    *arg = p + off;

    తిరిగి OK;
}

/*
 * Evaluate a single or double quoted string possibly containing expressions.
 * "arg" points to the '$'.  The result is put in "rettv".
 * Returns OK or FAIL.
 */
    పూర్ణాంకం
eval_interp_string(char_u **arg, typval_T *rettv, పూర్ణాంకం evaluate)
{
    typval_T	tv;
    పూర్ణాంకం		ret = OK;
    పూర్ణాంకం		quote;
    garray_T	ga;
    char_u	*p;

    ga_init2(&ga, 1, 80);

    // *arg is on the '$' character, move it to the first string character.
    ++*arg;
    quote = **arg;
    ++*arg;

    కొరకు (;;)
    {
	// Get the string up to the matching quote or to a single '{'.
	// "arg" is advanced to either the quote or the '{'.
	ఒకవేళ (quote == '"')
	    ret = eval_string(arg, &tv, evaluate, TRUE);
	లేకపోతే
	    ret = eval_lit_string(arg, &tv, evaluate, TRUE);
	ఒకవేళ (ret == FAIL)
	    ఆపు;
	ఒకవేళ (evaluate)
	{
	    ga_concat(&ga, tv.vval.v_string);
	    clear_tv(&tv);
	}

	ఒకవేళ (**arg != '{')
	{
	    // found terminating quote
	    ++*arg;
	    ఆపు;
	}
	p = eval_one_సమాసం_లోపల_str(*arg, &ga, evaluate);
	ఒకవేళ (p == ఖాళీ)
	{
	    ret = FAIL;
	    ఆపు;
	}
	*arg = p;
    }

    rettv->v_type = VAR_STRING;
    ఒకవేళ (ret == FAIL || !evaluate || ga_append(&ga, NUL) == FAIL)
    {
	ga_clear(&ga);
	rettv->vval.v_string = ఖాళీ;
	తిరిగి ret;
    }

    rettv->vval.v_string = ga.ga_దత్తాంశం;
    తిరిగి OK;
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
    పూర్ణాంకం		copyID)
{
    తిరిగి echo_string_core(tv, tofree, numbuf, copyID, FALSE, TRUE, FALSE);
}

/*
 * Get the value of an environment variable.
 * "arg" is pointing to the '$'.  It is advanced to after the name.
 * If the environment variable was not set, silently assume it is empty.
 * Return FAIL if the name is invalid.
 */
    పూర్ణాంకం
eval_env_var(char_u **arg, typval_T *rettv, పూర్ణాంకం evaluate)
{
    char_u	*string = ఖాళీ;
    పూర్ణాంకం		len;
    పూర్ణాంకం		cc;
    char_u	*name;
    పూర్ణాంకం		mustfree = FALSE;

    ++*arg;
    name = *arg;
    len = get_env_len(arg);
    ఒకవేళ (evaluate)
    {
	ఒకవేళ (len == 0)
	    తిరిగి FAIL; // invalid empty name

	cc = name[len];
	name[len] = NUL;
	// first try vim_getenv(), fast for normal environment vars
	string = vim_getenv(name, &mustfree);
	ఒకవేళ (string != ఖాళీ && *string != NUL)
	{
	    ఒకవేళ (!mustfree)
		string = vim_strsave(string);
	}
	లేకపోతే
	{
	    ఒకవేళ (mustfree)
		vim_free(string);

	    // next try expanding things like $VIM and ${HOME}
	    string = expand_env_save(name - 1);
	    ఒకవేళ (string != ఖాళీ && *string == '$')
		VIM_CLEAR(string);
	}
	name[len] = cc;

	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = string;
	rettv->v_తాళం = 0;
    }

    తిరిగి OK;
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
    పూర్ణాంకం		did_emsg_before = did_emsg;

    ఒకవేళ (argvars[0].v_type != VAR_STRING || !లోపల_vim9script())
	lnum = (linenr_T)tv_get_సంఖ్య_chk(&argvars[0], ఖాళీ);
    ఒకవేళ (lnum <= 0 && did_emsg_before == did_emsg
					    && argvars[0].v_type != VAR_సంఖ్య)
    {
	పూర్ణాంకం	fnum;
	pos_T	*fp;

	// no valid number, try using arg like line()
	fp = var2fpos(&argvars[0], TRUE, &fnum, FALSE);
	ఒకవేళ (fp != ఖాళీ)
	    lnum = fp->lnum;
    }
    తిరిగి lnum;
}

/*
 * Get the lnum from the first argument.
 * Also accepts "$", then "buf" is used.
 * Returns 0 on error.
 */
    linenr_T
tv_get_lnum_buf(typval_T *argvars, buf_T *buf)
{
    ఒకవేళ (argvars[0].v_type == VAR_STRING
	    && argvars[0].vval.v_string != ఖాళీ
	    && argvars[0].vval.v_string[0] == '$'
	    && argvars[0].vval.v_string[1] == NUL
	    && buf != ఖాళీ)
	తిరిగి buf->b_ml.ml_పంక్తి_లెక్క;
    తిరిగి (linenr_T)tv_get_సంఖ్య_chk(&argvars[0], ఖాళీ);
}

/*
 * Get buffer by number or pattern.
 */
    buf_T *
tv_get_buf(typval_T *tv, పూర్ణాంకం curtab_only)
{
    char_u	*name = tv->vval.v_string;
    buf_T	*buf;

    ఒకవేళ (tv->v_type == VAR_సంఖ్య)
	తిరిగి buflist_findnr((పూర్ణాంకం)tv->vval.v_సంఖ్య);
    ఒకవేళ (tv->v_type != VAR_STRING)
	తిరిగి ఖాళీ;
    ఒకవేళ (name == ఖాళీ || *name == NUL)
	తిరిగి curbuf;
    ఒకవేళ (name[0] == '$' && name[1] == NUL)
	తిరిగి lastbuf;

    buf = buflist_కనుగొను_by_name(name, curtab_only);

    // If not found, try expanding the name, like done for bufexists().
    ఒకవేళ (buf == ఖాళీ)
	buf = కనుగొను_బఫరు(tv);

    తిరిగి buf;
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
    ఒకవేళ (buf == ఖాళీ
	    && tv->v_type != VAR_సంఖ్య
	    && tv->v_type != VAR_STRING)
	// issue errmsg for type error
	(శూన్య)tv_get_సంఖ్య(tv);
    తిరిగి buf;
}

/*
 * Check that repeating "slen" items "n" times, does not
 * overflow the int used for the resulting length.
 */
    పూర్ణాంకం
check_repeat_లెక్క(varnumber_T slen, varnumber_T n)
{
    // Use a division so that the multiplication cannot overflow.
    ఒకవేళ (n <= (varnumber_T)గరిష్ఠ_పూర్ణాంకం / slen)
	తిరిగి OK;

    అక్షరం	buf[NUMBUFLEN];

    vim_snprintf(buf, పరిమాణం(buf), "%lld", (దీర్ఘ దీర్ఘ)n);
    semsg(_(e_val_too_large), buf);
    తిరిగి FAIL;
}

#అయితే_ముగింపు // FEAT_EVAL
