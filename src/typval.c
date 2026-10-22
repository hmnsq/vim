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

#ಆದರೆ defined(FEAT_EVAL)

/*
 * Allocate memory for a variable type-value, and make it empty (0 or NULL
 * value).
 */
    typval_T *
alloc_tv(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ ALLOC_CLEAR_ONE(typval_T);
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
    ಆದರೆ (rettv != ಖಾಲಿ)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = s;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	vim_free(s);
    ಹಿಂತಿರುಗಿ rettv;
}

/*
 * Free the memory for a variable type-value.
 */
    ಶೂನ್ಯ
free_tv(typval_T *varp)
{
    ಆದರೆ (varp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆಯ್ಕೆ (varp->v_type)
    {
	ಸ್ಥಿತಿ VAR_FUNC:
	    func_unref(varp->vval.v_string);
	    // FALLTHROUGH
	ಸ್ಥಿತಿ VAR_STRING:
	    vim_free(varp->vval.v_string);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_PARTIAL:
	    partial_unref(varp->vval.v_partial);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_BLOB:
	    blob_unref(varp->vval.v_blob);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_LIST:
	    list_unref(varp->vval.v_list);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_TUPLE:
	    tuple_unref(varp->vval.v_tuple);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_DICT:
	    dict_unref(varp->vval.v_dict);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_ಕೆಲಸ:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
	    ಕೆಲಸ_unref(varp->vval.v_ಕೆಲಸ);
	    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
	ಸ್ಥಿತಿ VAR_CHANNEL:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
	    channel_unref(varp->vval.v_channel);
	    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
	ಸ್ಥಿತಿ VAR_CLASS:
	    class_unref(varp->vval.v_class);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_OBJECT:
	    object_unref(varp->vval.v_object);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ VAR_TYPEALIAS:
	    typealias_unref(varp->vval.v_typealias);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ VAR_ಸಂಖ್ಯೆ:
	ಸ್ಥಿತಿ VAR_FLOAT:
	ಸ್ಥಿತಿ VAR_ANY:
	ಸ್ಥಿತಿ VAR_UNKNOWN:
	ಸ್ಥಿತಿ VAR_VOID:
	ಸ್ಥಿತಿ VAR_BOOL:
	ಸ್ಥಿತಿ VAR_SPECIAL:
	ಸ್ಥಿತಿ VAR_INSTR:
	    ನಿಲ್ಲಿಸು;
    }
    vim_free(varp);
}

/*
 * Free the memory for a variable value and set the value to NULL or 0.
 */
    ಶೂನ್ಯ
clear_tv(typval_T *varp)
{
    ಆದರೆ (varp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆಯ್ಕೆ (varp->v_type)
    {
	ಸ್ಥಿತಿ VAR_FUNC:
	    func_unref(varp->vval.v_string);
	    // FALLTHROUGH
	ಸ್ಥಿತಿ VAR_STRING:
	    VIM_CLEAR(varp->vval.v_string);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_PARTIAL:
	    partial_unref(varp->vval.v_partial);
	    varp->vval.v_partial = ಖಾಲಿ;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_BLOB:
	    blob_unref(varp->vval.v_blob);
	    varp->vval.v_blob = ಖಾಲಿ;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_LIST:
	    list_unref(varp->vval.v_list);
	    varp->vval.v_list = ಖಾಲಿ;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_TUPLE:
	    tuple_unref(varp->vval.v_tuple);
	    varp->vval.v_tuple = ಖಾಲಿ;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_DICT:
	    dict_unref(varp->vval.v_dict);
	    varp->vval.v_dict = ಖಾಲಿ;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_ಸಂಖ್ಯೆ:
	ಸ್ಥಿತಿ VAR_BOOL:
	ಸ್ಥಿತಿ VAR_SPECIAL:
	    varp->vval.v_ಸಂಖ್ಯೆ = 0;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_FLOAT:
	    varp->vval.v_float = 0.0;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_ಕೆಲಸ:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
	    ಕೆಲಸ_unref(varp->vval.v_ಕೆಲಸ);
	    varp->vval.v_ಕೆಲಸ = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_CHANNEL:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
	    channel_unref(varp->vval.v_channel);
	    varp->vval.v_channel = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_INSTR:
	    VIM_CLEAR(varp->vval.v_instr);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_CLASS:
	    class_unref(varp->vval.v_class);
	    varp->vval.v_class = ಖಾಲಿ;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_OBJECT:
	    object_unref(varp->vval.v_object);
	    varp->vval.v_object = ಖಾಲಿ;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_TYPEALIAS:
	    typealias_unref(varp->vval.v_typealias);
	    varp->vval.v_typealias = ಖಾಲಿ;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_UNKNOWN:
	ಸ್ಥಿತಿ VAR_ANY:
	ಸ್ಥಿತಿ VAR_VOID:
	    ನಿಲ್ಲಿಸು;
    }
    varp->v_ಬೀಗ = 0;
}

/*
 * Set the value of a variable to NULL without freeing items.
 */
    ಶೂನ್ಯ
init_tv(typval_T *varp)
{
    ಆದರೆ (varp != ಖಾಲಿ)
	CLEAR_ಸೂಚಕ(varp);
}

    ಸ್ಥಿರ varnumber_T
tv_get_bool_or_ಸಂಖ್ಯೆ_chk(
	typval_T    *varp,
	ಪೂರ್ಣಾಂಕ	    *denote,
	ಪೂರ್ಣಾಂಕ	    want_bool,
	ಪೂರ್ಣಾಂಕ	    vim9_string_error) // in Vim9 using a string is an error
{
    varnumber_T	n = 0L;

    ಆಯ್ಕೆ (varp->v_type)
    {
	ಸ್ಥಿತಿ VAR_ಸಂಖ್ಯೆ:
	    ಆದರೆ (ಒಳ_vim9script() && want_bool && varp->vval.v_ಸಂಖ್ಯೆ != 0
						   && varp->vval.v_ಸಂಖ್ಯೆ != 1)
	    {
		semsg(_(e_using_ಸಂಖ್ಯೆ_as_bool_nr), varp->vval.v_ಸಂಖ್ಯೆ);
		ನಿಲ್ಲಿಸು;
	    }
	    ಹಿಂತಿರುಗಿ varp->vval.v_ಸಂಖ್ಯೆ;
	ಸ್ಥಿತಿ VAR_FLOAT:
	    emsg(_(e_using_float_as_ಸಂಖ್ಯೆ));
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_FUNC:
	ಸ್ಥಿತಿ VAR_PARTIAL:
	    emsg(_(e_using_funcref_as_ಸಂಖ್ಯೆ));
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_STRING:
	    ಆದರೆ (vim9_string_error && ಒಳ_vim9script())
	    {
		emsg_using_string_as(varp, !want_bool);
		ನಿಲ್ಲಿಸು;
	    }
	    ಆದರೆ (varp->vval.v_string != ಖಾಲಿ)
		vim_str2nr(varp->vval.v_string, ಖಾಲಿ, ಖಾಲಿ,
					 STR2NR_ALL, &n, ಖಾಲಿ, 0, FALSE, ಖಾಲಿ);
	    ಹಿಂತಿರುಗಿ n;
	ಸ್ಥಿತಿ VAR_LIST:
	    emsg(_(e_using_list_as_ಸಂಖ್ಯೆ));
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_TUPLE:
	    emsg(_(e_using_tuple_as_ಸಂಖ್ಯೆ));
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_DICT:
	    emsg(_(e_using_dictionary_as_ಸಂಖ್ಯೆ));
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_BOOL:
	ಸ್ಥಿತಿ VAR_SPECIAL:
	    ಆದರೆ (!want_bool && ಒಳ_vim9script())
	    {
		ಆದರೆ (varp->v_type == VAR_BOOL)
		    emsg(_(e_using_bool_as_ಸಂಖ್ಯೆ));
		ಇಲ್ಲದಿದ್ದರೆ
		    emsg(_(e_using_special_as_ಸಂಖ್ಯೆ));
		ನಿಲ್ಲಿಸು;
	    }
	    ಹಿಂತಿರುಗಿ varp->vval.v_ಸಂಖ್ಯೆ == VVAL_TRUE ? 1 : 0;
	ಸ್ಥಿತಿ VAR_ಕೆಲಸ:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
	    emsg(_(e_using_ಕೆಲಸ_as_ಸಂಖ್ಯೆ));
	    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
	ಸ್ಥಿತಿ VAR_CHANNEL:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
	    emsg(_(e_using_channel_as_ಸಂಖ್ಯೆ));
	    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
	ಸ್ಥಿತಿ VAR_BLOB:
	    emsg(_(e_using_blob_as_ಸಂಖ್ಯೆ));
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_CLASS:
	ಸ್ಥಿತಿ VAR_TYPEALIAS:
	    check_typval_is_ಮೌಲ್ಯ(varp);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_OBJECT:
	    {
		ಆದರೆ (varp->vval.v_object == ಖಾಲಿ)
		    emsg(_(e_using_object_as_string));
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    class_T *cl = varp->vval.v_object->obj_class;
		    ಆದರೆ (cl != ಖಾಲಿ && IS_ENUM(cl))
			semsg(_(e_using_enum_str_as_ಸಂಖ್ಯೆ), cl->class_name.string);
		    ಇಲ್ಲದಿದ್ದರೆ
			emsg(_(e_using_object_as_ಸಂಖ್ಯೆ));
		}
	    }
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_VOID:
	    emsg(_(e_cannot_use_void_ಮೌಲ್ಯ));
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_UNKNOWN:
	ಸ್ಥಿತಿ VAR_ANY:
	ಸ್ಥಿತಿ VAR_INSTR:
	    ಆಂತರಿಕ_error_no_abort("tv_get_number(UNKNOWN)");
	    ನಿಲ್ಲಿಸು;
    }
    ಆದರೆ (denote == ಖಾಲಿ)		// useful for values that must be unsigned
	n = -1;
    ಇಲ್ಲದಿದ್ದರೆ
	*denote = TRUE;
    ಹಿಂತಿರುಗಿ n;
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
    ಪೂರ್ಣಾಂಕ		error = FALSE;

    ಹಿಂತಿರುಗಿ tv_get_ಸಂಖ್ಯೆ_chk(varp, &error);	// return 0L on error
}

/*
 * Like tv_get_number() but in Vim9 script do convert a number in a string to a
 * number without giving an error.
 */
    varnumber_T
tv_ಗಮ್ಯ_ಸಂಖ್ಯೆ(typval_T *varp)
{
    ಪೂರ್ಣಾಂಕ		error = FALSE;

    ಹಿಂತಿರುಗಿ tv_get_bool_or_ಸಂಖ್ಯೆ_chk(varp, &error, FALSE, FALSE);
}

    varnumber_T
tv_get_ಸಂಖ್ಯೆ_chk(typval_T *varp, ಪೂರ್ಣಾಂಕ *denote)
{
    ಹಿಂತಿರುಗಿ tv_get_bool_or_ಸಂಖ್ಯೆ_chk(varp, denote, FALSE, TRUE);
}

/*
 * Get the boolean value of "varp".  This is like tv_get_number_chk(),
 * but in Vim9 script accepts Number (0 and 1) and Bool/Special.
 */
    varnumber_T
tv_get_bool(typval_T *varp)
{
    ಹಿಂತಿರುಗಿ tv_get_bool_or_ಸಂಖ್ಯೆ_chk(varp, ಖಾಲಿ, TRUE, TRUE);
}

/*
 * Get the boolean value of "varp".  This is like tv_get_number_chk(),
 * but in Vim9 script accepts Number and Bool.
 */
    varnumber_T
tv_get_bool_chk(typval_T *varp, ಪೂರ್ಣಾಂಕ *denote)
{
    ಹಿಂತಿರುಗಿ tv_get_bool_or_ಸಂಖ್ಯೆ_chk(varp, denote, TRUE, TRUE);
}

    ಸ್ಥಿರ float_T
tv_get_float_chk(typval_T *varp, ಪೂರ್ಣಾಂಕ *error)
{
    ಆಯ್ಕೆ (varp->v_type)
    {
	ಸ್ಥಿತಿ VAR_ಸಂಖ್ಯೆ:
	    ಹಿಂತಿರುಗಿ (float_T)(varp->vval.v_ಸಂಖ್ಯೆ);
	ಸ್ಥಿತಿ VAR_FLOAT:
	    ಹಿಂತಿರುಗಿ varp->vval.v_float;
	ಸ್ಥಿತಿ VAR_FUNC:
	ಸ್ಥಿತಿ VAR_PARTIAL:
	    emsg(_(e_using_funcref_as_float));
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_STRING:
	    emsg(_(e_using_string_as_float));
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_LIST:
	    emsg(_(e_using_list_as_float));
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_TUPLE:
	    emsg(_(e_using_tuple_as_float));
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_DICT:
	    emsg(_(e_using_dictionary_as_float));
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_BOOL:
	    emsg(_(e_using_boolean_ಮೌಲ್ಯ_as_float));
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_SPECIAL:
	    emsg(_(e_using_special_ಮೌಲ್ಯ_as_float));
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_ಕೆಲಸ:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
	    emsg(_(e_using_ಕೆಲಸ_as_float));
	    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
	ಸ್ಥಿತಿ VAR_CHANNEL:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
	    emsg(_(e_using_channel_as_float));
	    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
	ಸ್ಥಿತಿ VAR_BLOB:
	    emsg(_(e_using_blob_as_float));
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_CLASS:
	ಸ್ಥಿತಿ VAR_TYPEALIAS:
	    check_typval_is_ಮೌಲ್ಯ(varp);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_OBJECT:
	    emsg(_(e_using_object_as_float));
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_VOID:
	    emsg(_(e_cannot_use_void_ಮೌಲ್ಯ));
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_UNKNOWN:
	ಸ್ಥಿತಿ VAR_ANY:
	ಸ್ಥಿತಿ VAR_INSTR:
	    ಆಂತರಿಕ_error_no_abort("tv_get_float(UNKNOWN)");
	    ನಿಲ್ಲಿಸು;
    }
    ಆದರೆ (error != ಖಾಲಿ)
	*error = TRUE;
    ಹಿಂತಿರುಗಿ 0;
}

    float_T
tv_get_float(typval_T *varp)
{
    ಹಿಂತಿರುಗಿ tv_get_float_chk(varp, ಖಾಲಿ);
}

/*
 * Give an error and return FAIL unless "args[idx]" is unknown
 */
    ಪೂರ್ಣಾಂಕ
check_for_unknown_arg(typval_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (ವಾದಗಳು[idx].v_type != VAR_UNKNOWN)
    {
	semsg(_(e_too_many_arguments), idx + 1);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string.
 */
    ಪೂರ್ಣಾಂಕ
check_for_string_arg(typval_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (ವಾದಗಳು[idx].v_type != VAR_STRING)
    {
	semsg(_(e_string_required_for_ವಾದ_nr), idx + 1);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a non-empty string.
 */
    ಪೂರ್ಣಾಂಕ
check_for_nonempty_string_arg(typval_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (check_for_string_arg(ವಾದಗಳು, idx) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;
    ಆದರೆ (ವಾದಗಳು[idx].vval.v_string == ಖಾಲಿ || *ವಾದಗಳು[idx].vval.v_string == NUL)
    {
	semsg(_(e_non_ಖಾಲಿಗಳು_string_required_for_ವಾದ_nr), idx + 1);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Check for an optional string argument at 'idx'
 */
    ಪೂರ್ಣಾಂಕ
check_for_opt_string_arg(typval_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ idx)
{
    ಹಿಂತಿರುಗಿ (ವಾದಗಳು[idx].v_type == VAR_UNKNOWN
	    || check_for_string_arg(ವಾದಗಳು, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a number.
 */
    ಪೂರ್ಣಾಂಕ
check_for_ಸಂಖ್ಯೆ_arg(typval_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (ವಾದಗಳು[idx].v_type != VAR_ಸಂಖ್ಯೆ)
    {
	semsg(_(e_ಸಂಖ್ಯೆ_required_for_ವಾದ_nr), idx + 1);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Check for an optional number argument at 'idx'
 */
    ಪೂರ್ಣಾಂಕ
check_for_opt_ಸಂಖ್ಯೆ_arg(typval_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ idx)
{
    ಹಿಂತಿರುಗಿ (ವಾದಗಳು[idx].v_type == VAR_UNKNOWN
	    || check_for_ಸಂಖ್ಯೆ_arg(ವಾದಗಳು, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a float or a number.
 */
    ಪೂರ್ಣಾಂಕ
check_for_float_or_nr_arg(typval_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (ವಾದಗಳು[idx].v_type != VAR_FLOAT && ವಾದಗಳು[idx].v_type != VAR_ಸಂಖ್ಯೆ)
    {
	semsg(_(e_float_or_ಸಂಖ್ಯೆ_required_for_ವಾದ_nr), idx + 1);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a bool.
 */
    ಪೂರ್ಣಾಂಕ
check_for_bool_arg(typval_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (ವಾದಗಳು[idx].v_type != VAR_BOOL
	    && !(ವಾದಗಳು[idx].v_type == VAR_ಸಂಖ್ಯೆ
		&& (ವಾದಗಳು[idx].vval.v_ಸಂಖ್ಯೆ == 0
		    || ವಾದಗಳು[idx].vval.v_ಸಂಖ್ಯೆ == 1)))
    {
	semsg(_(e_bool_required_for_ವಾದ_nr), idx + 1);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a bool or a number.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
check_for_bool_or_ಸಂಖ್ಯೆ_arg(typval_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (ವಾದಗಳು[idx].v_type != VAR_BOOL && ವಾದಗಳು[idx].v_type != VAR_ಸಂಖ್ಯೆ)
    {
	semsg(_(e_bool_or_ಸಂಖ್ಯೆ_required_for_ವಾದ_nr), idx + 1);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Check for an optional bool argument at 'idx'.
 * Return FAIL if the type is wrong.
 */
    ಪೂರ್ಣಾಂಕ
check_for_opt_bool_arg(typval_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (ವಾದಗಳು[idx].v_type == VAR_UNKNOWN)
	ಹಿಂತಿರುಗಿ OK;
    ಹಿಂತಿರುಗಿ check_for_bool_arg(ವಾದಗಳು, idx);
}

/*
 * Check for an optional bool or number argument at 'idx'.
 * Return FAIL if the type is wrong.
 */
    ಪೂರ್ಣಾಂಕ
check_for_opt_bool_or_ಸಂಖ್ಯೆ_arg(typval_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (ವಾದಗಳು[idx].v_type == VAR_UNKNOWN)
	ಹಿಂತಿರುಗಿ OK;
    ಹಿಂತಿರುಗಿ check_for_bool_or_ಸಂಖ್ಯೆ_arg(ವಾದಗಳು, idx);
}

/*
 * Check for an optional bool or dict argument at 'idx'.
 * Return FAIL if the type is wrong.
 */
    ಪೂರ್ಣಾಂಕ
check_for_opt_bool_or_dict_arg(typval_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (ವಾದಗಳು[idx].v_type == VAR_UNKNOWN || ವಾದಗಳು[idx].v_type == VAR_DICT)
	ಹಿಂತಿರುಗಿ OK;
    ಹಿಂತಿರುಗಿ check_for_bool_arg(ವಾದಗಳು, idx);
}

/*
 * Give an error and return FAIL unless "args[idx]" is a blob.
 */
    ಪೂರ್ಣಾಂಕ
check_for_blob_arg(typval_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (ವಾದಗಳು[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_blob_required_for_ವಾದ_nr), idx + 1);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list.
 */
    ಪೂರ್ಣಾಂಕ
check_for_list_arg(typval_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (ವಾದಗಳು[idx].v_type != VAR_LIST)
    {
	semsg(_(e_list_required_for_ವಾದ_nr), idx + 1);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a non-NULL list.
 */
    ಪೂರ್ಣಾಂಕ
check_for_nonnull_list_arg(typval_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (check_for_list_arg(ವಾದಗಳು, idx) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (ವಾದಗಳು[idx].vval.v_list == ಖಾಲಿ)
    {
	semsg(_(e_non_null_list_required_for_ವಾದ_nr), idx + 1);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Check for an optional list argument at 'idx'
 */
    ಪೂರ್ಣಾಂಕ
check_for_opt_list_arg(typval_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ idx)
{
    ಹಿಂತಿರುಗಿ (ವಾದಗಳು[idx].v_type == VAR_UNKNOWN
	    || check_for_list_arg(ವಾದಗಳು, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a tuple.
 */
    ಪೂರ್ಣಾಂಕ
check_for_tuple_arg(typval_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (ವಾದಗಳು[idx].v_type != VAR_TUPLE)
    {
	semsg(_(e_tuple_required_for_ವಾದ_nr), idx + 1);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a dict.
 */
    ಪೂರ್ಣಾಂಕ
check_for_dict_arg(typval_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (ವಾದಗಳು[idx].v_type != VAR_DICT)
    {
	semsg(_(e_dict_required_for_ವಾದ_nr), idx + 1);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a non-NULL dict.
 */
    ಪೂರ್ಣಾಂಕ
check_for_nonnull_dict_arg(typval_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (check_for_dict_arg(ವಾದಗಳು, idx) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (ವಾದಗಳು[idx].vval.v_dict == ಖಾಲಿ)
    {
	semsg(_(e_non_null_dict_required_for_ವಾದ_nr), idx + 1);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Check for an optional dict argument at 'idx'
 */
    ಪೂರ್ಣಾಂಕ
check_for_opt_dict_arg(typval_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ idx)
{
    ಹಿಂತಿರುಗಿ (ವಾದಗಳು[idx].v_type == VAR_UNKNOWN
	    || check_for_dict_arg(ವಾದಗಳು, idx) != FAIL) ? OK : FAIL;
}

/*
 * Check for an optional non-NULL dict argument at 'idx'
 */
    ಪೂರ್ಣಾಂಕ
check_for_opt_nonnull_dict_arg(typval_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ idx)
{
    ಹಿಂತಿರುಗಿ (ವಾದಗಳು[idx].v_type == VAR_UNKNOWN
	    || check_for_nonnull_dict_arg(ವಾದಗಳು, idx) != FAIL) ? OK : FAIL;
}

#ಆದರೆ defined(FEAT_ಕೆಲಸ_CHANNEL)
/*
 * Give an error and return FAIL unless "args[idx]" is a channel or a job.
 */
    ಪೂರ್ಣಾಂಕ
check_for_chan_or_ಕೆಲಸ_arg(typval_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (ವಾದಗಳು[idx].v_type != VAR_CHANNEL && ವಾದಗಳು[idx].v_type != VAR_ಕೆಲಸ)
    {
	semsg(_(e_chan_or_ಕೆಲಸ_required_for_ವಾದ_nr), idx + 1);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an optional channel or a
 * job.
 */
    ಪೂರ್ಣಾಂಕ
check_for_opt_chan_or_ಕೆಲಸ_arg(typval_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ idx)
{
    ಹಿಂತಿರುಗಿ (ವಾದಗಳು[idx].v_type == VAR_UNKNOWN
	    || check_for_chan_or_ಕೆಲಸ_arg(ವಾದಗಳು, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a job.
 */
    ಪೂರ್ಣಾಂಕ
check_for_ಕೆಲಸ_arg(typval_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (ವಾದಗಳು[idx].v_type != VAR_ಕೆಲಸ)
    {
	semsg(_(e_ಕೆಲಸ_required_for_ವಾದ_nr), idx + 1);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Check for an optional job argument at 'idx'.
 */
    ಪೂರ್ಣಾಂಕ
check_for_opt_ಕೆಲಸ_arg(typval_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ idx)
{
    ಹಿಂತಿರುಗಿ (ವಾದಗಳು[idx].v_type == VAR_UNKNOWN
	    || check_for_ಕೆಲಸ_arg(ವಾದಗಳು, idx) != FAIL) ? OK : FAIL;
}
#ಇಲ್ಲದಿದ್ದರೆ
/*
 * Give an error and return FAIL unless "args[idx]" is an optional channel or a
 * job.  Used without the +channel feature, thus only VAR_UNKNOWN is accepted.
 */
    ಪೂರ್ಣಾಂಕ
check_for_opt_chan_or_ಕೆಲಸ_arg(typval_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ idx)
{
    ಹಿಂತಿರುಗಿ ವಾದಗಳು[idx].v_type == VAR_UNKNOWN ? OK : FAIL;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Give an error and return FAIL unless "args[idx]" is a string or
 * a number.
 */
    ಪೂರ್ಣಾಂಕ
check_for_string_or_ಸಂಖ್ಯೆ_arg(typval_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (ವಾದಗಳು[idx].v_type != VAR_STRING && ವಾದಗಳು[idx].v_type != VAR_ಸಂಖ್ಯೆ)
    {
	semsg(_(e_string_or_ಸಂಖ್ಯೆ_required_for_ವಾದ_nr), idx + 1);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Check for an optional string or number argument at 'idx'.
 */
    ಪೂರ್ಣಾಂಕ
check_for_opt_string_or_ಸಂಖ್ಯೆ_arg(typval_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ idx)
{
    ಹಿಂತಿರುಗಿ (ವಾದಗಳು[idx].v_type == VAR_UNKNOWN
	    || check_for_string_or_ಸಂಖ್ಯೆ_arg(ವಾದಗಳು, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a buffer number.
 * Buffer number can be a number or a string.
 */
    ಪೂರ್ಣಾಂಕ
check_for_ಬಫರ್_arg(typval_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ idx)
{
    ಹಿಂತಿರುಗಿ check_for_string_or_ಸಂಖ್ಯೆ_arg(ವಾದಗಳು, idx);
}

/*
 * Check for an optional buffer argument at 'idx'
 */
    ಪೂರ್ಣಾಂಕ
check_for_opt_ಬಫರ್_arg(typval_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ idx)
{
    ಹಿಂತಿರುಗಿ (ವಾದಗಳು[idx].v_type == VAR_UNKNOWN
	    || check_for_ಬಫರ್_arg(ವಾದಗಳು, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a line number.
 * Line number can be a number or a string.
 */
    ಪೂರ್ಣಾಂಕ
check_for_lnum_arg(typval_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ idx)
{
    ಹಿಂತಿರುಗಿ check_for_string_or_ಸಂಖ್ಯೆ_arg(ವಾದಗಳು, idx);
}

/*
 * Check for an optional line number argument at 'idx'
 */
    ಪೂರ್ಣಾಂಕ
check_for_opt_lnum_arg(typval_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ idx)
{
    ಹಿಂತಿರುಗಿ (ವಾದಗಳು[idx].v_type == VAR_UNKNOWN
	    || check_for_lnum_arg(ವಾದಗಳು, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a blob.
 */
    ಪೂರ್ಣಾಂಕ
check_for_string_or_blob_arg(typval_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (ವಾದಗಳು[idx].v_type != VAR_STRING && ವಾದಗಳು[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_string_or_blob_required_for_ವಾದ_nr), idx + 1);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a list.
 */
    ಪೂರ್ಣಾಂಕ
check_for_string_or_list_arg(typval_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (ವಾದಗಳು[idx].v_type != VAR_STRING && ವಾದಗಳು[idx].v_type != VAR_LIST)
    {
	semsg(_(e_string_or_list_required_for_ವಾದ_nr), idx + 1);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string, a list, a
 * tuple or a blob.
 */
    ಪೂರ್ಣಾಂಕ
check_for_string_or_list_or_tuple_or_blob_arg(typval_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (ವಾದಗಳು[idx].v_type != VAR_STRING
	    && ವಾದಗಳು[idx].v_type != VAR_LIST
	    && ವಾದಗಳು[idx].v_type != VAR_TUPLE
	    && ವಾದಗಳು[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_string_list_tuple_or_blob_required_for_ವಾದ_nr), idx + 1);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Check for an optional string or list argument at 'idx'
 */
    ಪೂರ್ಣಾಂಕ
check_for_opt_string_or_list_arg(typval_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ idx)
{
    ಹಿಂತಿರುಗಿ (ವಾದಗಳು[idx].v_type == VAR_UNKNOWN
	    || check_for_string_or_list_arg(ವಾದಗಳು, idx) != FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a dict.
 */
    ಪೂರ್ಣಾಂಕ
check_for_string_or_dict_arg(typval_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (ವಾದಗಳು[idx].v_type != VAR_STRING && ವಾದಗಳು[idx].v_type != VAR_DICT)
    {
	semsg(_(e_string_or_dict_required_for_ವಾದ_nr), idx + 1);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string or a number
 * or a list.
 */
    ಪೂರ್ಣಾಂಕ
check_for_string_or_ಸಂಖ್ಯೆ_or_list_arg(typval_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (ವಾದಗಳು[idx].v_type != VAR_STRING
	    && ವಾದಗಳು[idx].v_type != VAR_ಸಂಖ್ಯೆ
	    && ವಾದಗಳು[idx].v_type != VAR_LIST)
    {
	semsg(_(e_string_ಸಂಖ್ಯೆ_or_list_required_for_ವಾದ_nr), idx + 1);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an optional string
 * or number or a list
 */
    ಪೂರ್ಣಾಂಕ
check_for_opt_string_or_ಸಂಖ್ಯೆ_or_list_arg(typval_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ idx)
{
    ಹಿಂತಿರುಗಿ (ವಾದಗಳು[idx].v_type == VAR_UNKNOWN
	    || check_for_string_or_ಸಂಖ್ಯೆ_or_list_arg(ವಾದಗಳು, idx)
							!= FAIL) ? OK : FAIL;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string, a number, a
 * list, a tuple or a blob.
 */
    ಪೂರ್ಣಾಂಕ
check_for_repeat_func_arg(typval_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (ವಾದಗಳು[idx].v_type != VAR_STRING
	    && ವಾದಗಳು[idx].v_type != VAR_ಸಂಖ್ಯೆ
	    && ವಾದಗಳು[idx].v_type != VAR_LIST
	    && ವಾದಗಳು[idx].v_type != VAR_TUPLE
	    && ವಾದಗಳು[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_repeatable_type_required_for_ವಾದ_nr), idx + 1);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string, a list, a
 * tuple or a dict.
 */
    ಪೂರ್ಣಾಂಕ
check_for_string_list_tuple_or_dict_arg(typval_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (ವಾದಗಳು[idx].v_type != VAR_STRING
	    && ವಾದಗಳು[idx].v_type != VAR_LIST
	    && ವಾದಗಳು[idx].v_type != VAR_TUPLE
	    && ವಾದಗಳು[idx].v_type != VAR_DICT)
    {
	semsg(_(e_string_list_tuple_or_dict_required_for_ವಾದ_nr), idx + 1);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a string
 * or a function reference.
 */
    ಪೂರ್ಣಾಂಕ
check_for_string_or_func_arg(typval_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (ವಾದಗಳು[idx].v_type != VAR_PARTIAL
	    && ವಾದಗಳು[idx].v_type != VAR_FUNC
	    && ವಾದಗಳು[idx].v_type != VAR_STRING)
    {
	semsg(_(e_string_or_function_required_for_ವಾದ_nr), idx + 1);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list or a blob.
 */
    ಪೂರ್ಣಾಂಕ
check_for_list_or_blob_arg(typval_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (ವಾದಗಳು[idx].v_type != VAR_LIST && ವಾದಗಳು[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_list_or_blob_required_for_ವಾದ_nr), idx + 1);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list or a tuple.
 */
    ಪೂರ್ಣಾಂಕ
check_for_list_or_tuple_arg(typval_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (ವಾದಗಳು[idx].v_type != VAR_LIST && ವಾದಗಳು[idx].v_type != VAR_TUPLE)
    {
	semsg(_(e_list_or_tuple_required_for_ವಾದ_nr), idx + 1);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list, a tuple or a
 * blob.
 */
    ಪೂರ್ಣಾಂಕ
check_for_list_or_tuple_or_blob_arg(typval_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (ವಾದಗಳು[idx].v_type != VAR_LIST
	    && ವಾದಗಳು[idx].v_type != VAR_TUPLE
	    && ವಾದಗಳು[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_list_or_tuple_or_blob_required_for_ವಾದ_nr), idx + 1);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list, a tuple or a
 * dict
 */
    ಪೂರ್ಣಾಂಕ
check_for_list_or_tuple_or_dict_arg(typval_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (ವಾದಗಳು[idx].v_type != VAR_LIST
	    && ವಾದಗಳು[idx].v_type != VAR_TUPLE
	    && ವಾದಗಳು[idx].v_type != VAR_DICT)
    {
	semsg(_(e_list_or_tuple_or_dict_required_for_ವಾದ_nr), idx + 1);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list or dict or a
 * blob.
 */
    ಪೂರ್ಣಾಂಕ
check_for_list_or_dict_or_blob_arg(typval_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (ವಾದಗಳು[idx].v_type != VAR_LIST
	    && ವಾದಗಳು[idx].v_type != VAR_DICT
	    && ವಾದಗಳು[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_list_dict_or_blob_required_for_ವಾದ_nr), idx + 1);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a list, a tuple, a dict,
 * a blob or a string.
 */
    ಪೂರ್ಣಾಂಕ
check_for_list_tuple_dict_blob_or_string_arg(typval_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (ವಾದಗಳು[idx].v_type != VAR_LIST
	    && ವಾದಗಳು[idx].v_type != VAR_TUPLE
	    && ವಾದಗಳು[idx].v_type != VAR_DICT
	    && ವಾದಗಳು[idx].v_type != VAR_BLOB
	    && ವಾದಗಳು[idx].v_type != VAR_STRING)
    {
	semsg(_(e_list_tuple_dict_blob_or_string_required_for_ವಾದ_nr), idx + 1);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an optional buffer
 * number or a dict.
 */
    ಪೂರ್ಣಾಂಕ
check_for_opt_ಬಫರ್_or_dict_arg(typval_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (ವಾದಗಳು[idx].v_type != VAR_UNKNOWN
	    && ವಾದಗಳು[idx].v_type != VAR_STRING
	    && ವಾದಗಳು[idx].v_type != VAR_ಸಂಖ್ಯೆ
	    && ವಾದಗಳು[idx].v_type != VAR_DICT)
    {
	semsg(_(e_string_required_for_ವಾದ_nr), idx + 1);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Give an error and return FAIL unless "args[idx]" is an object.
 */
    ಪೂರ್ಣಾಂಕ
check_for_object_arg(typval_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (ವಾದಗಳು[idx].v_type != VAR_OBJECT)
    {
	semsg(_(e_object_required_for_ವಾದ_nr), idx + 1);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Returns TRUE if "tv" is a type alias for a class
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
tv_class_alias(typval_T *tv)
{
    ಹಿಂತಿರುಗಿ tv->v_type == VAR_TYPEALIAS &&
			tv->vval.v_typealias->ta_type->tt_type == VAR_OBJECT;
}

/*
 * Give an error and return FAIL unless "args[idx]" is a class
 * or class typealias.
 */
    ಪೂರ್ಣಾಂಕ
check_for_class_or_typealias_ವಾದಗಳು(typval_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ idx)
{
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = idx; ವಾದಗಳು[i].v_type != VAR_UNKNOWN; ++i)
    {
	ಆದರೆ (ವಾದಗಳು[i].v_type != VAR_CLASS && !tv_class_alias(&ವಾದಗಳು[idx]))
	{
	    semsg(_(e_class_or_typealias_required_for_ವಾದ_nr), i + 1);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
    }
    ಹಿಂತಿರುಗಿ OK;
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

    ಹಿಂತಿರುಗಿ tv_get_string_buf(varp, mybuf);
}

/*
 * Like tv_get_string() but don't allow number to string conversion for Vim9.
 */
    char_u *
tv_get_string_strict(typval_T *varp)
{
    ಸ್ಥಿರ char_u   mybuf[NUMBUFLEN];
    char_u	    *res =  tv_get_string_buf_chk_strict(
						 varp, mybuf, ಒಳ_vim9script());

    ಹಿಂತಿರುಗಿ res != ಖಾಲಿ ? res : (char_u *)"";
}

    char_u *
tv_get_string_buf(typval_T *varp, char_u *buf)
{
    char_u	*res = tv_get_string_buf_chk(varp, buf);

    ಹಿಂತಿರುಗಿ res != ಖಾಲಿ ? res : (char_u *)"";
}

/*
 * Careful: This uses a single, static buffer.  YOU CAN ONLY USE IT ONCE!
 */
    char_u *
tv_get_string_chk(typval_T *varp)
{
    ಸ್ಥಿರ char_u   mybuf[NUMBUFLEN];

    ಹಿಂತಿರುಗಿ tv_get_string_buf_chk(varp, mybuf);
}

    char_u *
tv_get_string_buf_chk(typval_T *varp, char_u *buf)
{
    ಹಿಂತಿರುಗಿ tv_get_string_buf_chk_strict(varp, buf, FALSE);
}

    char_u *
tv_get_string_buf_chk_strict(typval_T *varp, char_u *buf, ಪೂರ್ಣಾಂಕ strict)
{
    ಆಯ್ಕೆ (varp->v_type)
    {
	ಸ್ಥಿತಿ VAR_ಸಂಖ್ಯೆ:
	    ಆದರೆ (strict)
	    {
		emsg(_(e_using_ಸಂಖ್ಯೆ_as_string));
		ನಿಲ್ಲಿಸು;
	    }
	    vim_snprintf((ಅಕ್ಷರ *)buf, NUMBUFLEN, "%lld",
					    (varnumber_T)varp->vval.v_ಸಂಖ್ಯೆ);
	    ಹಿಂತಿರುಗಿ buf;
	ಸ್ಥಿತಿ VAR_FUNC:
	ಸ್ಥಿತಿ VAR_PARTIAL:
	    emsg(_(e_using_funcref_as_string));
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_LIST:
	    emsg(_(e_using_list_as_string));
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_TUPLE:
	    emsg(_(e_using_tuple_as_string));
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_DICT:
	    emsg(_(e_using_dictionary_as_string));
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_FLOAT:
	    ಆದರೆ (strict)
	    {
		emsg(_(e_using_float_as_string));
		ನಿಲ್ಲಿಸು;
	    }
	    vim_snprintf((ಅಕ್ಷರ *)buf, NUMBUFLEN, "%g", varp->vval.v_float);
	    ಹಿಂತಿರುಗಿ buf;
	ಸ್ಥಿತಿ VAR_STRING:
	    ಆದರೆ (varp->vval.v_string != ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ varp->vval.v_string;
	    ಹಿಂತಿರುಗಿ (char_u *)"";
	ಸ್ಥಿತಿ VAR_BOOL:
	ಸ್ಥಿತಿ VAR_SPECIAL:
	    STRCPY(buf, get_var_special_name(varp->vval.v_ಸಂಖ್ಯೆ));
	    ಹಿಂತಿರುಗಿ buf;
	ಸ್ಥಿತಿ VAR_BLOB:
	    emsg(_(e_using_blob_as_string));
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_CLASS:
	ಸ್ಥಿತಿ VAR_TYPEALIAS:
	    check_typval_is_ಮೌಲ್ಯ(varp);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_OBJECT:
	    {
		ಆದರೆ (varp->vval.v_object == ಖಾಲಿ)
		    emsg(_(e_using_object_as_string));
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    class_T *cl = varp->vval.v_object->obj_class;
		    ಆದರೆ (cl != ಖಾಲಿ && IS_ENUM(cl))
			semsg(_(e_using_enum_str_as_string), cl->class_name.string);
		    ಇಲ್ಲದಿದ್ದರೆ
			emsg(_(e_using_object_as_string));
		}
	    }
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_ಕೆಲಸ:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
	    ಆದರೆ (ಒಳ_vim9script())
	    {
		semsg(_(e_using_invalid_ಮೌಲ್ಯ_as_string_str), "job");
		ನಿಲ್ಲಿಸು;
	    }
	    ಹಿಂತಿರುಗಿ ಕೆಲಸ_ಗಮ್ಯ_string_buf(varp, buf);
#ಆದರೆ_ಅಂತ್ಯ
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_CHANNEL:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
	    ಆದರೆ (ಒಳ_vim9script())
	    {
		semsg(_(e_using_invalid_ಮೌಲ್ಯ_as_string_str), "channel");
		ನಿಲ್ಲಿಸು;
	    }
	    ಹಿಂತಿರುಗಿ channel_ಗಮ್ಯ_string_buf(varp, buf);
#ಆದರೆ_ಅಂತ್ಯ
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_VOID:
	    emsg(_(e_cannot_use_void_ಮೌಲ್ಯ));
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_UNKNOWN:
	ಸ್ಥಿತಿ VAR_ANY:
	ಸ್ಥಿತಿ VAR_INSTR:
	    semsg(_(e_using_invalid_ಮೌಲ್ಯ_as_string_str),
						  vartype_name(varp->v_type));
	    ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Turn a typeval into a string.  Similar to tv_get_string_buf() but uses
 * string() on Dict, List, etc.
 */
    char_u *
tv_stringify(typval_T *varp, char_u *buf)
{
    ಆದರೆ (varp->v_type == VAR_LIST
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
	ಹಿಂತಿರುಗಿ tmp.vval.v_string;
    }
    ಹಿಂತಿರುಗಿ tv_get_string_buf(varp, buf);
}

/*
 * Return TRUE if typeval "tv" and its value are set to be locked (immutable).
 * Also give an error message, using "name" or _("name") when use_gettext is
 * TRUE.
 */
    ಪೂರ್ಣಾಂಕ
tv_check_ಬೀಗ(typval_T *tv, char_u *name, ಪೂರ್ಣಾಂಕ use_gettext)
{
    ಪೂರ್ಣಾಂಕ	ಬೀಗ = 0;

    ಆಯ್ಕೆ (tv->v_type)
    {
	ಸ್ಥಿತಿ VAR_BLOB:
	    ಆದರೆ (tv->vval.v_blob != ಖಾಲಿ)
		ಬೀಗ = tv->vval.v_blob->bv_ಬೀಗ;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_LIST:
	    ಆದರೆ (tv->vval.v_list != ಖಾಲಿ)
		ಬೀಗ = tv->vval.v_list->lv_ಬೀಗ;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_TUPLE:
	    ಆದರೆ (tv->vval.v_tuple != ಖಾಲಿ)
		ಬೀಗ = tv->vval.v_tuple->tv_ಬೀಗ;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_DICT:
	    ಆದರೆ (tv->vval.v_dict != ಖಾಲಿ)
		ಬೀಗ = tv->vval.v_dict->dv_ಬೀಗ;
	    ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
	    ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ ಮೌಲ್ಯ_check_ಬೀಗ(tv->v_ಬೀಗ, name, use_gettext)
		   || (ಬೀಗ != 0 && ಮೌಲ್ಯ_check_ಬೀಗ(ಬೀಗ, name, use_gettext));
}

/*
 * Copy the values from typval_T "from" to typval_T "to".
 * When needed allocates string or increases reference count.
 * Does not make a copy of a list, blob or dict but copies the reference!
 * It is OK for "from" and "to" to point to the same item.  This is used to
 * make a copy later.
 */
    ಪೂರ್ಣಾಂಕ
copy_tv(typval_T *from, typval_T *ಗಮ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		ret = OK;

    ಗಮ್ಯ->v_type = from->v_type;
    ಗಮ್ಯ->v_ಬೀಗ = 0;
    ಆಯ್ಕೆ (from->v_type)
    {
	ಸ್ಥಿತಿ VAR_ಸಂಖ್ಯೆ:
	ಸ್ಥಿತಿ VAR_BOOL:
	ಸ್ಥಿತಿ VAR_SPECIAL:
	    ಗಮ್ಯ->vval.v_ಸಂಖ್ಯೆ = from->vval.v_ಸಂಖ್ಯೆ;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_FLOAT:
	    ಗಮ್ಯ->vval.v_float = from->vval.v_float;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_ಕೆಲಸ:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
	    ಗಮ್ಯ->vval.v_ಕೆಲಸ = from->vval.v_ಕೆಲಸ;
	    ಆದರೆ (ಗಮ್ಯ->vval.v_ಕೆಲಸ != ಖಾಲಿ)
		++ಗಮ್ಯ->vval.v_ಕೆಲಸ->jv_refcount;
	    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
	ಸ್ಥಿತಿ VAR_CHANNEL:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
	    ಗಮ್ಯ->vval.v_channel = from->vval.v_channel;
	    ಆದರೆ (ಗಮ್ಯ->vval.v_channel != ಖಾಲಿ)
		++ಗಮ್ಯ->vval.v_channel->ch_refcount;
	    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
	ಸ್ಥಿತಿ VAR_INSTR:
	    ಗಮ್ಯ->vval.v_instr = from->vval.v_instr;
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ VAR_CLASS:
	    copy_class(from, ಗಮ್ಯ);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ VAR_OBJECT:
	    copy_object(from, ಗಮ್ಯ);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ VAR_STRING:
	ಸ್ಥಿತಿ VAR_FUNC:
	    ಆದರೆ (from->vval.v_string == ಖಾಲಿ)
		ಗಮ್ಯ->vval.v_string = ಖಾಲಿ;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಗಮ್ಯ->vval.v_string = vim_strsave(from->vval.v_string);
		ಆದರೆ (from->v_type == VAR_FUNC)
		    func_ref(ಗಮ್ಯ->vval.v_string);
	    }
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_PARTIAL:
	    ಆದರೆ (from->vval.v_partial == ಖಾಲಿ)
		ಗಮ್ಯ->vval.v_partial = ಖಾಲಿ;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಗಮ್ಯ->vval.v_partial = from->vval.v_partial;
		++ಗಮ್ಯ->vval.v_partial->pt_refcount;
	    }
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_BLOB:
	    ಆದರೆ (from->vval.v_blob == ಖಾಲಿ)
		ಗಮ್ಯ->vval.v_blob = ಖಾಲಿ;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಗಮ್ಯ->vval.v_blob = from->vval.v_blob;
		++ಗಮ್ಯ->vval.v_blob->bv_refcount;
	    }
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_LIST:
	    ಆದರೆ (from->vval.v_list == ಖಾಲಿ)
		ಗಮ್ಯ->vval.v_list = ಖಾಲಿ;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಗಮ್ಯ->vval.v_list = from->vval.v_list;
		++ಗಮ್ಯ->vval.v_list->lv_refcount;
	    }
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_TUPLE:
	    ಆದರೆ (from->vval.v_tuple == ಖಾಲಿ)
		ಗಮ್ಯ->vval.v_tuple = ಖಾಲಿ;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಗಮ್ಯ->vval.v_tuple = from->vval.v_tuple;
		++ಗಮ್ಯ->vval.v_tuple->tv_refcount;
	    }
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_DICT:
	    ಆದರೆ (from->vval.v_dict == ಖಾಲಿ)
		ಗಮ್ಯ->vval.v_dict = ಖಾಲಿ;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಗಮ್ಯ->vval.v_dict = from->vval.v_dict;
		++ಗಮ್ಯ->vval.v_dict->dv_refcount;
	    }
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_TYPEALIAS:
	    ಆದರೆ (from->vval.v_typealias == ಖಾಲಿ)
		ಗಮ್ಯ->vval.v_typealias = ಖಾಲಿ;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಗಮ್ಯ->vval.v_typealias = from->vval.v_typealias;
		++ಗಮ್ಯ->vval.v_typealias->ta_refcount;
	    }
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_VOID:
	    emsg(_(e_cannot_use_void_ಮೌಲ್ಯ));
	    ret = FAIL;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_UNKNOWN:
	ಸ್ಥಿತಿ VAR_ANY:
	    ಆಂತರಿಕ_error_no_abort("copy_tv(UNKNOWN)");
	    ret = FAIL;
	    ನಿಲ್ಲಿಸು;
    }

    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Compare "tv1" and "tv2".
 */
    ಪೂರ್ಣಾಂಕ
typval_compare2(
    typval_T	*tv1,	// first operand
    typval_T	*tv2,	// second operand
    exprtype_T	type,	// operator
    ಪೂರ್ಣಾಂಕ		ic,	// ignore case
    ಪೂರ್ಣಾಂಕ		*res)	// comparison result
{
    varnumber_T	n1, n2;
    ಪೂರ್ಣಾಂಕ		type_is = type == ಅಭಿವ್ಯಕ್ತಿ_IS || type == ಅಭಿವ್ಯಕ್ತಿ_ISNOT;

    ಆದರೆ (check_typval_is_ಮೌಲ್ಯ(tv1) == FAIL
	|| check_typval_is_ಮೌಲ್ಯ(tv2) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type_is && tv1->v_type != tv2->v_type)
    {
	// For "is" a different type always means FALSE, for "isnot"
	// it means TRUE.
	*res = (type == ಅಭಿವ್ಯಕ್ತಿ_ISNOT);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (((tv1->v_type == VAR_SPECIAL && tv1->vval.v_ಸಂಖ್ಯೆ == VVAL_NULL)
		|| (tv2->v_type == VAR_SPECIAL
					   && tv2->vval.v_ಸಂಖ್ಯೆ == VVAL_NULL))
	    && tv1->v_type != tv2->v_type
	    && (type == ಅಭಿವ್ಯಕ್ತಿ_EQUAL || type == ಅಭಿವ್ಯಕ್ತಿ_NEQUAL))
    {
	n1 = typval_compare_null(tv1, tv2);
	ಆದರೆ (n1 == MAYBE)
	    ಹಿಂತಿರುಗಿ FAIL;
	ಆದರೆ (type == ಅಭಿವ್ಯಕ್ತಿ_NEQUAL)
	    n1 = !n1;
	*res = n1;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tv1->v_type == VAR_BLOB || tv2->v_type == VAR_BLOB)
    {
	ಆದರೆ (typval_compare_blob(tv1, tv2, type, res) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tv1->v_type == VAR_LIST || tv2->v_type == VAR_LIST)
    {
	ಆದರೆ (typval_compare_list(tv1, tv2, type, ic, res) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tv1->v_type == VAR_TUPLE || tv2->v_type == VAR_TUPLE)
    {
	ಆದರೆ (typval_compare_tuple(tv1, tv2, type, ic, res) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tv1->v_type == VAR_OBJECT || tv2->v_type == VAR_OBJECT)
    {
	ಆದರೆ (typval_compare_object(tv1, tv2, type, ic, res) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tv1->v_type == VAR_DICT || tv2->v_type == VAR_DICT)
    {
	ಆದರೆ (typval_compare_dict(tv1, tv2, type, ic, res) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tv1->v_type == VAR_FUNC || tv2->v_type == VAR_FUNC
	|| tv1->v_type == VAR_PARTIAL || tv2->v_type == VAR_PARTIAL)
    {
	ಆದರೆ (typval_compare_func(tv1, tv2, type, ic, res) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }

    // If one of the two variables is a float, compare as a float.
    // When using "=~" or "!~", always compare as string.
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((tv1->v_type == VAR_FLOAT || tv2->v_type == VAR_FLOAT)
	    && type != ಅಭಿವ್ಯಕ್ತಿ_ಹೊಂದಿಕೆ && type != ಅಭಿವ್ಯಕ್ತಿ_NOMATCH)
    {
	float_T f1, f2;
	ಪೂರ್ಣಾಂಕ	error = FALSE;

	f1 = tv_get_float_chk(tv1, &error);
	ಆದರೆ (!error)
	    f2 = tv_get_float_chk(tv2, &error);
	ಆದರೆ (error)
	    ಹಿಂತಿರುಗಿ FAIL;
	n1 = FALSE;
	ಆಯ್ಕೆ (type)
	{
	    ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_IS:
	    ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_EQUAL:    n1 = (f1 == f2); ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_ISNOT:
	    ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_NEQUAL:   n1 = (f1 != f2); ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_GREATER:  n1 = (f1 > f2); ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_GEQUAL:   n1 = (f1 >= f2); ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_SMALLER:  n1 = (f1 < f2); ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_SEQUAL:   n1 = (f1 <= f2); ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_UNKNOWN:
	    ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_ಹೊಂದಿಕೆ:
	    ಪೂರ್ವನಿಯೋಜಿತ:  ನಿಲ್ಲಿಸು;  // avoid gcc warning
	}
	*res = n1;
    }

    // If one of the two variables is a number, compare as a number.
    // When using "=~" or "!~", always compare as string.
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((tv1->v_type == VAR_ಸಂಖ್ಯೆ || tv2->v_type == VAR_ಸಂಖ್ಯೆ)
	    && type != ಅಭಿವ್ಯಕ್ತಿ_ಹೊಂದಿಕೆ && type != ಅಭಿವ್ಯಕ್ತಿ_NOMATCH)
    {
	ಪೂರ್ಣಾಂಕ error = FALSE;

	n1 = tv_get_ಸಂಖ್ಯೆ_chk(tv1, &error);
	ಆದರೆ (!error)
	    n2 = tv_get_ಸಂಖ್ಯೆ_chk(tv2, &error);
	ಆದರೆ (error)
	    ಹಿಂತಿರುಗಿ FAIL;
	ಆಯ್ಕೆ (type)
	{
	    ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_IS:
	    ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_EQUAL:    n1 = (n1 == n2); ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_ISNOT:
	    ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_NEQUAL:   n1 = (n1 != n2); ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_GREATER:  n1 = (n1 > n2); ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_GEQUAL:   n1 = (n1 >= n2); ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_SMALLER:  n1 = (n1 < n2); ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_SEQUAL:   n1 = (n1 <= n2); ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_UNKNOWN:
	    ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_ಹೊಂದಿಕೆ:
	    ಪೂರ್ವನಿಯೋಜಿತ:  ನಿಲ್ಲಿಸು;  // avoid gcc warning
	}
	*res = n1;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಒಳ_vim9script() && (tv1->v_type == VAR_BOOL
				    || tv2->v_type == VAR_BOOL
				    || (tv1->v_type == VAR_SPECIAL
					      && tv2->v_type == VAR_SPECIAL)))
    {
	ಆದರೆ (tv1->v_type != tv2->v_type)
	{
	    semsg(_(e_cannot_compare_str_with_str),
		       vartype_name(tv1->v_type), vartype_name(tv2->v_type));
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	n1 = tv1->vval.v_ಸಂಖ್ಯೆ;
	n2 = tv2->vval.v_ಸಂಖ್ಯೆ;
	ಆಯ್ಕೆ (type)
	{
	    ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_IS:
	    ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_EQUAL:    n1 = (n1 == n2); ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_ISNOT:
	    ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_NEQUAL:   n1 = (n1 != n2); ನಿಲ್ಲಿಸು;
	    ಪೂರ್ವನಿಯೋಜಿತ:
		semsg(_(e_invalid_operation_for_str),
						   vartype_name(tv1->v_type));
		ಹಿಂತಿರುಗಿ FAIL;
	}
	*res = n1;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tv1->v_type == tv2->v_type
	    && (tv1->v_type == VAR_CHANNEL || tv1->v_type == VAR_ಕೆಲಸ)
	    && (type == ಅಭಿವ್ಯಕ್ತಿ_NEQUAL || type == ಅಭಿವ್ಯಕ್ತಿ_EQUAL))
    {
	ಆದರೆ (tv1->v_type == VAR_CHANNEL)
	    n1 = tv1->vval.v_channel == tv2->vval.v_channel;
	ಇಲ್ಲದಿದ್ದರೆ
	    n1 = tv1->vval.v_ಕೆಲಸ == tv2->vval.v_ಕೆಲಸ;
	ಆದರೆ (type == ಅಭಿವ್ಯಕ್ತಿ_NEQUAL)
	    n1 = !n1;
	*res = n1;
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (typval_compare_string(tv1, tv2, type, ic, res) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Compare "tv1" and "tv2".
 * Put the result in "tv1".  Caller should clear "tv2".
 */
    ಪೂರ್ಣಾಂಕ
typval_compare(
    typval_T	*tv1,	// first operand
    typval_T	*tv2,	// second operand
    exprtype_T	type,   // operator
    ಪೂರ್ಣಾಂಕ		ic)     // ignore case
{
    ಪೂರ್ಣಾಂಕ res;

    ಆದರೆ (typval_compare2(tv1, tv2, type, ic, &res) == FAIL)
    {
	clear_tv(tv1);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    clear_tv(tv1);
    ಆದರೆ (ಒಳ_vim9script())
    {
	tv1->v_type = VAR_BOOL;
	tv1->vval.v_ಸಂಖ್ಯೆ = res ? VVAL_TRUE : VVAL_FALSE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	tv1->v_type = VAR_ಸಂಖ್ಯೆ;
	tv1->vval.v_ಸಂಖ್ಯೆ = res;
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Compare "tv1" to "tv2" as lists according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    ಪೂರ್ಣಾಂಕ
typval_compare_list(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	ಪೂರ್ಣಾಂಕ	    ic,
	ಪೂರ್ಣಾಂಕ	    *res)
{
    ಪೂರ್ಣಾಂಕ	    val = 0;

    ಆದರೆ (type == ಅಭಿವ್ಯಕ್ತಿ_IS || type == ಅಭಿವ್ಯಕ್ತಿ_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
				      && tv1->vval.v_list == tv2->vval.v_list);
	ಆದರೆ (type == ಅಭಿವ್ಯಕ್ತಿ_ISNOT)
	    val = !val;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tv1->v_type != tv2->v_type
	    || (type != ಅಭಿವ್ಯಕ್ತಿ_EQUAL && type != ಅಭಿವ್ಯಕ್ತಿ_NEQUAL))
    {
	ಆದರೆ (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_list_with_list));
	ಇಲ್ಲದಿದ್ದರೆ
	    emsg(_(e_invalid_operation_for_list));
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	val = list_equal(tv1->vval.v_list, tv2->vval.v_list, ic);
	ಆದರೆ (type == ಅಭಿವ್ಯಕ್ತಿ_NEQUAL)
	    val = !val;
    }
    *res = val;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Compare "tv1" to "tv2" as tuples according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    ಪೂರ್ಣಾಂಕ
typval_compare_tuple(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	ಪೂರ್ಣಾಂಕ	    ic,
	ಪೂರ್ಣಾಂಕ	    *res)
{
    ಪೂರ್ಣಾಂಕ	    val = 0;

    ಆದರೆ (type == ಅಭಿವ್ಯಕ್ತಿ_IS || type == ಅಭಿವ್ಯಕ್ತಿ_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
			      && tv1->vval.v_tuple == tv2->vval.v_tuple);
	ಆದರೆ (type == ಅಭಿವ್ಯಕ್ತಿ_ISNOT)
	    val = !val;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tv1->v_type != tv2->v_type
	    || (type != ಅಭಿವ್ಯಕ್ತಿ_EQUAL && type != ಅಭಿವ್ಯಕ್ತಿ_NEQUAL))
    {
	ಆದರೆ (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_tuple_with_tuple));
	ಇಲ್ಲದಿದ್ದರೆ
	    emsg(_(e_invalid_operation_for_tuple));
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	val = tuple_equal(tv1->vval.v_tuple, tv2->vval.v_tuple, ic);
	ಆದರೆ (type == ಅಭಿವ್ಯಕ್ತಿ_NEQUAL)
	    val = !val;
    }
    *res = val;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Compare v:null with another type.  Return TRUE if the value is NULL.
 */
    ಪೂರ್ಣಾಂಕ
typval_compare_null(typval_T *tv1, typval_T *tv2)
{
    ಆದರೆ ((tv1->v_type == VAR_SPECIAL && tv1->vval.v_ಸಂಖ್ಯೆ == VVAL_NULL)
	    || (tv2->v_type == VAR_SPECIAL && tv2->vval.v_ಸಂಖ್ಯೆ == VVAL_NULL))
    {
	typval_T	*tv = tv1->v_type == VAR_SPECIAL ? tv2 : tv1;

	ಆಯ್ಕೆ (tv->v_type)
	{
	    ಸ್ಥಿತಿ VAR_BLOB: ಹಿಂತಿರುಗಿ tv->vval.v_blob == ಖಾಲಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
	    ಸ್ಥಿತಿ VAR_CHANNEL: ಹಿಂತಿರುಗಿ tv->vval.v_channel == ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
	    // TODO: null_class handling
	    // case VAR_CLASS: return tv->vval.v_class == NULL;
	    ಸ್ಥಿತಿ VAR_DICT: ಹಿಂತಿರುಗಿ tv->vval.v_dict == ಖಾಲಿ;
	    ಸ್ಥಿತಿ VAR_FUNC: ಹಿಂತಿರುಗಿ tv->vval.v_string == ಖಾಲಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
	    ಸ್ಥಿತಿ VAR_ಕೆಲಸ: ಹಿಂತಿರುಗಿ tv->vval.v_ಕೆಲಸ == ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
	    ಸ್ಥಿತಿ VAR_LIST: ಹಿಂತಿರುಗಿ tv->vval.v_list == ಖಾಲಿ;
	    ಸ್ಥಿತಿ VAR_TUPLE: ಹಿಂತಿರುಗಿ tv->vval.v_tuple == ಖಾಲಿ;
	    ಸ್ಥಿತಿ VAR_OBJECT: ಹಿಂತಿರುಗಿ tv->vval.v_object == ಖಾಲಿ;
	    ಸ್ಥಿತಿ VAR_PARTIAL: ಹಿಂತಿರುಗಿ tv->vval.v_partial == ಖಾಲಿ;
	    ಸ್ಥಿತಿ VAR_STRING: ಹಿಂತಿರುಗಿ tv->vval.v_string == ಖಾಲಿ;

	    ಸ್ಥಿತಿ VAR_ಸಂಖ್ಯೆ: ಆದರೆ (!ಒಳ_vim9script())
				 ಹಿಂತಿರುಗಿ tv->vval.v_ಸಂಖ್ಯೆ == 0;
			     ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ VAR_FLOAT: ಆದರೆ (!ಒಳ_vim9script())
				 ಹಿಂತಿರುಗಿ tv->vval.v_float == 0.0;
			     ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ VAR_TYPEALIAS: ಹಿಂತಿರುಗಿ tv->vval.v_typealias == ಖಾಲಿ;
	    ಪೂರ್ವನಿಯೋಜಿತ: ನಿಲ್ಲಿಸು;
	}
    }
    // although comparing null with number, float or bool is not very useful
    // we won't give an error
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Compare "tv1" to "tv2" as blobs according to "type".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    ಪೂರ್ಣಾಂಕ
typval_compare_blob(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	ಪೂರ್ಣಾಂಕ	    *res)
{
    ಪೂರ್ಣಾಂಕ	    val = 0;

    ಆದರೆ (type == ಅಭಿವ್ಯಕ್ತಿ_IS || type == ಅಭಿವ್ಯಕ್ತಿ_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
			&& tv1->vval.v_blob == tv2->vval.v_blob);
	ಆದರೆ (type == ಅಭಿವ್ಯಕ್ತಿ_ISNOT)
	    val = !val;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tv1->v_type != tv2->v_type
	    || (type != ಅಭಿವ್ಯಕ್ತಿ_EQUAL && type != ಅಭಿವ್ಯಕ್ತಿ_NEQUAL))
    {
	ಆದರೆ (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_blob_with_blob));
	ಇಲ್ಲದಿದ್ದರೆ
	    emsg(_(e_invalid_operation_for_blob));
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	val = blob_equal(tv1->vval.v_blob, tv2->vval.v_blob);
	ಆದರೆ (type == ಅಭಿವ್ಯಕ್ತಿ_NEQUAL)
	    val = !val;
    }
    *res = val;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Compare "tv1" to "tv2" as objects according to "type".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    ಪೂರ್ಣಾಂಕ
typval_compare_object(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	ಪೂರ್ಣಾಂಕ	    ic,
	ಪೂರ್ಣಾಂಕ	    *res)
{
    ಪೂರ್ಣಾಂಕ res_ಹೊಂದಿಕೆ = type == ಅಭಿವ್ಯಕ್ತಿ_EQUAL || type == ಅಭಿವ್ಯಕ್ತಿ_IS ? TRUE : FALSE;

    ಆದರೆ (tv1->vval.v_object == ಖಾಲಿ && tv2->vval.v_object == ಖಾಲಿ)
    {
	*res = res_ಹೊಂದಿಕೆ;
	ಹಿಂತಿರುಗಿ OK;
    }
    ಆದರೆ (tv1->vval.v_object == ಖಾಲಿ || tv2->vval.v_object == ಖಾಲಿ)
    {
	*res = !res_ಹೊಂದಿಕೆ;
	ಹಿಂತಿರುಗಿ OK;
    }

    object_T *obj1 = tv1->vval.v_object;
    object_T *obj2 = tv2->vval.v_object;
    ಆದರೆ (type == ಅಭಿವ್ಯಕ್ತಿ_IS || type == ಅಭಿವ್ಯಕ್ತಿ_ISNOT)
    {
	*res = obj1 == obj2 ? res_ಹೊಂದಿಕೆ : !res_ಹೊಂದಿಕೆ;
	ಹಿಂತಿರುಗಿ OK;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tv1->v_type != tv2->v_type)
    {
	emsg(_(e_can_only_compare_object_with_object));
	ಹಿಂತಿರುಗಿ FAIL;
    }

    *res = object_equal(obj1, obj2, ic) ? res_ಹೊಂದಿಕೆ : !res_ಹೊಂದಿಕೆ;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Compare "tv1" to "tv2" as dictionaries according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    ಪೂರ್ಣಾಂಕ
typval_compare_dict(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	ಪೂರ್ಣಾಂಕ	    ic,
	ಪೂರ್ಣಾಂಕ	    *res)
{
    ಪೂರ್ಣಾಂಕ	    val;

    ಆದರೆ (type == ಅಭಿವ್ಯಕ್ತಿ_IS || type == ಅಭಿವ್ಯಕ್ತಿ_ISNOT)
    {
	val = (tv1->v_type == tv2->v_type
			&& tv1->vval.v_dict == tv2->vval.v_dict);
	ಆದರೆ (type == ಅಭಿವ್ಯಕ್ತಿ_ISNOT)
	    val = !val;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tv1->v_type != tv2->v_type
		|| (type != ಅಭಿವ್ಯಕ್ತಿ_EQUAL && type != ಅಭಿವ್ಯಕ್ತಿ_NEQUAL))
    {
	ಆದರೆ (tv1->v_type != tv2->v_type)
	    emsg(_(e_can_only_compare_dictionary_with_dictionary));
	ಇಲ್ಲದಿದ್ದರೆ
	    emsg(_(e_invalid_operation_for_dictionary));
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	val = dict_equal(tv1->vval.v_dict, tv2->vval.v_dict, ic);
	ಆದರೆ (type == ಅಭಿವ್ಯಕ್ತಿ_NEQUAL)
	    val = !val;
    }
    *res = val;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Compare "tv1" to "tv2" as funcrefs according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    ಪೂರ್ಣಾಂಕ
typval_compare_func(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	ಪೂರ್ಣಾಂಕ	    ic,
	ಪೂರ್ಣಾಂಕ	    *res)
{
    ಪೂರ್ಣಾಂಕ	    val = 0;

    ಆದರೆ (type != ಅಭಿವ್ಯಕ್ತಿ_EQUAL && type != ಅಭಿವ್ಯಕ್ತಿ_NEQUAL
	    && type != ಅಭಿವ್ಯಕ್ತಿ_IS && type != ಅಭಿವ್ಯಕ್ತಿ_ISNOT)
    {
	emsg(_(e_invalid_operation_for_funcrefs));
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಆದರೆ ((tv1->v_type == VAR_PARTIAL && tv1->vval.v_partial == ಖಾಲಿ)
	    || (tv2->v_type == VAR_PARTIAL && tv2->vval.v_partial == ಖಾಲಿ))
	// When both partials are NULL, then they are equal.
	// Otherwise they are not equal.
	val = (tv1->vval.v_partial == tv2->vval.v_partial);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type == ಅಭಿವ್ಯಕ್ತಿ_IS || type == ಅಭಿವ್ಯಕ್ತಿ_ISNOT)
    {
	ಆದರೆ (tv1->v_type == VAR_FUNC && tv2->v_type == VAR_FUNC)
	    // strings are considered the same if their value is
	    // the same
	    val = tv_equal(tv1, tv2, ic);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tv1->v_type == VAR_PARTIAL && tv2->v_type == VAR_PARTIAL)
	    val = (tv1->vval.v_partial == tv2->vval.v_partial);
	ಇಲ್ಲದಿದ್ದರೆ
	    val = FALSE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	val = tv_equal(tv1, tv2, ic);
    ಆದರೆ (type == ಅಭಿವ್ಯಕ್ತಿ_NEQUAL || type == ಅಭಿವ್ಯಕ್ತಿ_ISNOT)
	val = !val;
    *res = val;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Compare "tv1" to "tv2" as strings according to "type" and "ic".
 * Put the result, false or true, in "res".
 * Return FAIL and give an error message when the comparison can't be done.
 */
    ಪೂರ್ಣಾಂಕ
typval_compare_string(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	ಪೂರ್ಣಾಂಕ	    ic,
	ಪೂರ್ಣಾಂಕ	    *res)
{
    ಪೂರ್ಣಾಂಕ		i = 0;
    ಪೂರ್ಣಾಂಕ		val = FALSE;
    char_u	*s1, *s2;
    char_u	buf1[NUMBUFLEN], buf2[NUMBUFLEN];

    ಆದರೆ (ಒಳ_vim9script()
	  && ((tv1->v_type != VAR_STRING && tv1->v_type != VAR_SPECIAL)
	   || (tv2->v_type != VAR_STRING && tv2->v_type != VAR_SPECIAL)))
    {
	semsg(_(e_cannot_compare_str_with_str),
		   vartype_name(tv1->v_type), vartype_name(tv2->v_type));
	ಹಿಂತಿರುಗಿ FAIL;
    }
    s1 = tv_get_string_buf(tv1, buf1);
    s2 = tv_get_string_buf(tv2, buf2);
    ಆದರೆ (type != ಅಭಿವ್ಯಕ್ತಿ_ಹೊಂದಿಕೆ && type != ಅಭಿವ್ಯಕ್ತಿ_NOMATCH)
	i = ic ? MB_STRICMP(s1, s2) : STRCMP(s1, s2);
    ಆಯ್ಕೆ (type)
    {
	ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_IS:	    ಆದರೆ (ಒಳ_vim9script())
			    {
				// Really check it is the same string, not just
				// the same value.
				val = tv1->vval.v_string == tv2->vval.v_string;
				ನಿಲ್ಲಿಸು;
			    }
			    // FALLTHROUGH
	ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_EQUAL:    val = (i == 0); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_ISNOT:    ಆದರೆ (ಒಳ_vim9script())
			    {
				// Really check it is not the same string, not
				// just a different value.
				val = tv1->vval.v_string != tv2->vval.v_string;
				ನಿಲ್ಲಿಸು;
			    }
			    // FALLTHROUGH
	ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_NEQUAL:   val = (i != 0); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_GREATER:  val = (i > 0); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_GEQUAL:   val = (i >= 0); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_SMALLER:  val = (i < 0); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_SEQUAL:   val = (i <= 0); ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_ಹೊಂದಿಕೆ:
	ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_NOMATCH:
		val = ನಮೂನೆ_ಹೊಂದಿಕೆ(s2, s1, ic);
		ಆದರೆ (type == ಅಭಿವ್ಯಕ್ತಿ_NOMATCH)
		    val = !val;
		ನಿಲ್ಲಿಸು;

	ಪೂರ್ವನಿಯೋಜಿತ:  ನಿಲ್ಲಿಸು;  // avoid gcc warning
    }
    *res = val;
    ಹಿಂತಿರುಗಿ OK;
}
/*
 * Convert any type to a string, never give an error.
 * When "quotes" is TRUE add quotes to a string.
 * Returns an allocated string.
 */
    char_u *
typval_tostring(typval_T *arg, ಪೂರ್ಣಾಂಕ quotes)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    char_u	*ret = ಖಾಲಿ;

    ಆದರೆ (arg == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ vim_strsave((char_u *)"(does not exist)");
    ಆದರೆ (!quotes && arg->v_type == VAR_STRING)
    {
	ret = vim_strsave(arg->vval.v_string == ಖಾಲಿ ? (char_u *)""
							 : arg->vval.v_string);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ret = tv2string(arg, &tofree, numbuf, 0);
	// Make a copy if we have a value but it's not in allocated memory.
	ಆದರೆ (ret != ಖಾಲಿ && tofree == ಖಾಲಿ)
	    ret = vim_strsave(ret);
    }
    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Return TRUE if typeval "tv" is locked: Either that value is locked itself
 * or it refers to a List or Dictionary that is locked.
 */
    ಪೂರ್ಣಾಂಕ
tv_islocked(typval_T *tv)
{
    ಹಿಂತಿರುಗಿ (tv->v_ಬೀಗ & VAR_LOCKED)
	|| (tv->v_type == VAR_LIST
		&& tv->vval.v_list != ಖಾಲಿ
		&& (tv->vval.v_list->lv_ಬೀಗ & VAR_LOCKED))
	|| (tv->v_type == VAR_TUPLE
		&& tv->vval.v_tuple != ಖಾಲಿ
		&& (tv->vval.v_tuple->tv_ಬೀಗ & VAR_LOCKED))
	|| (tv->v_type == VAR_DICT
		&& tv->vval.v_dict != ಖಾಲಿ
		&& (tv->vval.v_dict->dv_ಬೀಗ & VAR_LOCKED));
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
func_equal(
    typval_T *tv1,
    typval_T *tv2,
    ಪೂರ್ಣಾಂಕ	     ic)	    // ignore case
{
    char_u	*s1, *s2;
    dict_T	*d1, *d2;
    ಪೂರ್ಣಾಂಕ		a1, a2;
    ಪೂರ್ಣಾಂಕ		i;

    // empty and NULL function name considered the same
    s1 = tv1->v_type == VAR_FUNC ? tv1->vval.v_string
					   : partial_name(tv1->vval.v_partial);
    ಆದರೆ (s1 != ಖಾಲಿ && *s1 == NUL)
	s1 = ಖಾಲಿ;
    s2 = tv2->v_type == VAR_FUNC ? tv2->vval.v_string
					   : partial_name(tv2->vval.v_partial);
    ಆದರೆ (s2 != ಖಾಲಿ && *s2 == NUL)
	s2 = ಖಾಲಿ;
    ಆದರೆ (s1 == ಖಾಲಿ || s2 == ಖಾಲಿ)
    {
	ಆದರೆ (s1 != s2)
	    ಹಿಂತಿರುಗಿ FALSE;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(s1, s2) != 0)
	ಹಿಂತಿರುಗಿ FALSE;

    // empty dict and NULL dict is different
    d1 = tv1->v_type == VAR_FUNC ? ಖಾಲಿ : tv1->vval.v_partial->pt_dict;
    d2 = tv2->v_type == VAR_FUNC ? ಖಾಲಿ : tv2->vval.v_partial->pt_dict;
    ಆದರೆ (d1 == ಖಾಲಿ || d2 == ಖಾಲಿ)
    {
	ಆದರೆ (d1 != d2)
	    ಹಿಂತಿರುಗಿ FALSE;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!dict_equal(d1, d2, ic))
	ಹಿಂತಿರುಗಿ FALSE;

    // empty list and no list considered the same
    a1 = tv1->v_type == VAR_FUNC ? 0 : tv1->vval.v_partial->pt_argc;
    a2 = tv2->v_type == VAR_FUNC ? 0 : tv2->vval.v_partial->pt_argc;
    ಆದರೆ (a1 != a2)
	ಹಿಂತಿರುಗಿ FALSE;
    ಗಾಗಿ (i = 0; i < a1; ++i)
	ಆದರೆ (!tv_equal(tv1->vval.v_partial->pt_argv + i,
		      tv2->vval.v_partial->pt_argv + i, ic))
	    ಹಿಂತಿರುಗಿ FALSE;

    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Return TRUE if "tv1" and "tv2" have the same value.
 * Compares the items just like "==" would compare them, but strings and
 * numbers are different.  Floats and numbers are also different.
 */
    ಪೂರ್ಣಾಂಕ
tv_equal(
    typval_T *tv1,
    typval_T *tv2,
    ಪೂರ್ಣಾಂಕ	     ic)	    // ignore case
{
    char_u	buf1[NUMBUFLEN], buf2[NUMBUFLEN];
    char_u	*s1, *s2;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  recursive_cnt = 0;	    // catch recursive loops
    ಪೂರ್ಣಾಂಕ		r;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	tv_equal_recurse_ಎಲ್ಲೆ;

    // Catch lists and dicts that have an endless loop by limiting
    // recursiveness to a limit.  We guess they are equal then.
    // A fixed limit has the problem of still taking an awful long time.
    // Reduce the limit every time running into it. That should work fine for
    // deeply linked structures that are not recursively linked and catch
    // recursiveness quickly.
    ಆದರೆ (recursive_cnt == 0)
	tv_equal_recurse_ಎಲ್ಲೆ = 1000;
    ಆದರೆ (recursive_cnt >= tv_equal_recurse_ಎಲ್ಲೆ)
    {
	--tv_equal_recurse_ಎಲ್ಲೆ;
	ಹಿಂತಿರುಗಿ TRUE;
    }

    // For VAR_FUNC and VAR_PARTIAL compare the function name, bound dict and
    // arguments.
    ಆದರೆ ((tv1->v_type == VAR_FUNC
		|| (tv1->v_type == VAR_PARTIAL && tv1->vval.v_partial != ಖಾಲಿ))
	    && (tv2->v_type == VAR_FUNC
		|| (tv2->v_type == VAR_PARTIAL && tv2->vval.v_partial != ಖಾಲಿ)))
    {
	++recursive_cnt;
	r = func_equal(tv1, tv2, ic);
	--recursive_cnt;
	ಹಿಂತಿರುಗಿ r;
    }

    ಆದರೆ (tv1->v_type != tv2->v_type
	    && ((tv1->v_type != VAR_BOOL && tv1->v_type != VAR_SPECIAL)
		|| (tv2->v_type != VAR_BOOL && tv2->v_type != VAR_SPECIAL)))
	ಹಿಂತಿರುಗಿ FALSE;

    ಆಯ್ಕೆ (tv1->v_type)
    {
	ಸ್ಥಿತಿ VAR_LIST:
	    ++recursive_cnt;
	    r = list_equal(tv1->vval.v_list, tv2->vval.v_list, ic);
	    --recursive_cnt;
	    ಹಿಂತಿರುಗಿ r;

	ಸ್ಥಿತಿ VAR_TUPLE:
	    ++recursive_cnt;
	    r = tuple_equal(tv1->vval.v_tuple, tv2->vval.v_tuple, ic);
	    --recursive_cnt;
	    ಹಿಂತಿರುಗಿ r;

	ಸ್ಥಿತಿ VAR_DICT:
	    ++recursive_cnt;
	    r = dict_equal(tv1->vval.v_dict, tv2->vval.v_dict, ic);
	    --recursive_cnt;
	    ಹಿಂತಿರುಗಿ r;

	ಸ್ಥಿತಿ VAR_BLOB:
	    ಹಿಂತಿರುಗಿ blob_equal(tv1->vval.v_blob, tv2->vval.v_blob);

	ಸ್ಥಿತಿ VAR_ಸಂಖ್ಯೆ:
	ಸ್ಥಿತಿ VAR_BOOL:
	ಸ್ಥಿತಿ VAR_SPECIAL:
	    ಹಿಂತಿರುಗಿ tv1->vval.v_ಸಂಖ್ಯೆ == tv2->vval.v_ಸಂಖ್ಯೆ;

	ಸ್ಥಿತಿ VAR_STRING:
	    s1 = tv_get_string_buf(tv1, buf1);
	    s2 = tv_get_string_buf(tv2, buf2);
	    ಹಿಂತಿರುಗಿ ((ic ? MB_STRICMP(s1, s2) : STRCMP(s1, s2)) == 0);

	ಸ್ಥಿತಿ VAR_FLOAT:
	    ಹಿಂತಿರುಗಿ tv1->vval.v_float == tv2->vval.v_float;
	ಸ್ಥಿತಿ VAR_ಕೆಲಸ:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
	    ಹಿಂತಿರುಗಿ tv1->vval.v_ಕೆಲಸ == tv2->vval.v_ಕೆಲಸ;
#ಆದರೆ_ಅಂತ್ಯ
	ಸ್ಥಿತಿ VAR_CHANNEL:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
	    ಹಿಂತಿರುಗಿ tv1->vval.v_channel == tv2->vval.v_channel;
#ಆದರೆ_ಅಂತ್ಯ
	ಸ್ಥಿತಿ VAR_INSTR:
	    ಹಿಂತಿರುಗಿ tv1->vval.v_instr == tv2->vval.v_instr;

	ಸ್ಥಿತಿ VAR_CLASS:
	    // A class only exists once, equality is identity.
	    ಹಿಂತಿರುಗಿ tv1->vval.v_class == tv2->vval.v_class;

	ಸ್ಥಿತಿ VAR_OBJECT:
	    ++recursive_cnt;
	    r = object_equal(tv1->vval.v_object, tv2->vval.v_object, ic);
	    --recursive_cnt;
	    ಹಿಂತಿರುಗಿ r;

	ಸ್ಥಿತಿ VAR_PARTIAL:
	    ಹಿಂತಿರುಗಿ tv1->vval.v_partial == tv2->vval.v_partial;

	ಸ್ಥಿತಿ VAR_FUNC:
	    ಹಿಂತಿರುಗಿ tv1->vval.v_string == tv2->vval.v_string;

	ಸ್ಥಿತಿ VAR_TYPEALIAS:
	    ಹಿಂತಿರುಗಿ tv1->vval.v_typealias == tv2->vval.v_typealias;

	ಸ್ಥಿತಿ VAR_UNKNOWN:
	ಸ್ಥಿತಿ VAR_ANY:
	ಸ್ಥಿತಿ VAR_VOID:
	    ನಿಲ್ಲಿಸು;
    }

    // VAR_UNKNOWN can be the result of a invalid expression, let's say it
    // does not equal anything, not even itself.
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Get an option value.
 * "arg" points to the '&' or '+' before the option name.
 * "arg" is advanced to character after the option name.
 * Return OK or FAIL.
 */
    ಪೂರ್ಣಾಂಕ
eval_option(
    char_u	**arg,
    typval_T	*rettv,	// when NULL, only check if option exists
    ಪೂರ್ಣಾಂಕ		evaluate)
{
    char_u	*option_ಕೊನೆ;
    ದೀರ್ಘ	numval;
    char_u	*stringval;
    getoption_T	opt_type;
    ಪೂರ್ಣಾಂಕ		c;
    ಪೂರ್ಣಾಂಕ		working = (**arg == '+');    // has("+option")
    ಪೂರ್ಣಾಂಕ		ret = OK;
    ಪೂರ್ಣಾಂಕ		scope;

    // Isolate the option name and find its value.
    option_ಕೊನೆ = ಕಂಡುಹಿಡಿ_option_ಕೊನೆ(arg, &scope);
    ಆದರೆ (option_ಕೊನೆ == ಖಾಲಿ)
    {
	ಆದರೆ (rettv != ಖಾಲಿ)
	    semsg(_(e_option_name_missing_str), *arg);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (!evaluate)
    {
	*arg = option_ಕೊನೆ;
	ಹಿಂತಿರುಗಿ OK;
    }

    c = *option_ಕೊನೆ;
    *option_ಕೊನೆ = NUL;
    opt_type = get_option_ಮೌಲ್ಯ(*arg, &numval,
			       rettv == ಖಾಲಿ ? ಖಾಲಿ : &stringval, ಖಾಲಿ, scope);

    ಆದರೆ (opt_type == gov_unknown)
    {
	ಆದರೆ (rettv != ಖಾಲಿ)
	    semsg(_(e_unknown_option_str), *arg);
	ret = FAIL;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (rettv != ಖಾಲಿ)
    {
	rettv->v_ಬೀಗ = 0;
	ಆದರೆ (opt_type == gov_hidden_string)
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = ಖಾಲಿ;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (opt_type == gov_hidden_bool || opt_type == gov_hidden_ಸಂಖ್ಯೆ)
	{
	    rettv->v_type = ಒಳ_vim9script() && opt_type == gov_hidden_bool
						       ? VAR_BOOL : VAR_ಸಂಖ್ಯೆ;
	    rettv->vval.v_ಸಂಖ್ಯೆ = 0;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (opt_type == gov_bool || opt_type == gov_ಸಂಖ್ಯೆ)
	{
	    ಆದರೆ (ಒಳ_vim9script() && opt_type == gov_bool)
	    {
		rettv->v_type = VAR_BOOL;
		rettv->vval.v_ಸಂಖ್ಯೆ = numval ? VVAL_TRUE : VVAL_FALSE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		rettv->v_type = VAR_ಸಂಖ್ಯೆ;
		rettv->vval.v_ಸಂಖ್ಯೆ = numval;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ				// string option
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = stringval;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (working && (opt_type == gov_hidden_bool
			|| opt_type == gov_hidden_ಸಂಖ್ಯೆ
			|| opt_type == gov_hidden_string))
	ret = FAIL;

    *option_ಕೊನೆ = c;		    // put back for error messages
    *arg = option_ಕೊನೆ;

    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Allocate a variable for a number constant.  Also deals with "0z" for blob.
 * Return OK or FAIL.
 */
    ಪೂರ್ಣಾಂಕ
eval_ಸಂಖ್ಯೆ(
	char_u	    **arg,
	typval_T    *rettv,
	ಪೂರ್ಣಾಂಕ	    evaluate,
	ಪೂರ್ಣಾಂಕ	    want_string)
{
    ಪೂರ್ಣಾಂಕ		len;
    ಪೂರ್ಣಾಂಕ		skip_quotes = !ಒಳ_old_script(4);
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		get_float = FALSE;

    // We accept a float when the format matches
    // "[0-9]\+\.[0-9]\+\([eE][+-]\?[0-9]\+\)\?".  This is very
    // strict to avoid backwards compatibility problems.
    // With script version 2 and later the leading digit can be
    // omitted.
    // Don't look for a float after the "." operator, so that
    // ":let vers = 1.2.3" doesn't fail.
    ಆದರೆ (**arg == '.')
	p = *arg;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	p = *arg + 1;
	ಆದರೆ (skip_quotes)
	    ಗಾಗಿ (;;)
	    {
		ಆದರೆ (*p == '\'')
		    ++p;
		ಆದರೆ (!vim_isdigit(*p))
		    ನಿಲ್ಲಿಸು;
		p = skipdigits(p);
	    }
	ಇಲ್ಲದಿದ್ದರೆ
	    p = skipdigits(p);
    }
    ಆದರೆ (!want_string && p[0] == '.' && vim_isdigit(p[1]))
    {
	get_float = TRUE;
	p = skipdigits(p + 2);
	ಆದರೆ (*p == 'e' || *p == 'E')
	{
	    ++p;
	    ಆದರೆ (*p == '-' || *p == '+')
		++p;
	    ಆದರೆ (!vim_isdigit(*p))
		get_float = FALSE;
	    ಇಲ್ಲದಿದ್ದರೆ
		p = skipdigits(p + 1);
	}
	ಆದರೆ (ASCII_ISALPHA(*p) || *p == '.')
	    get_float = FALSE;
    }
    ಆದರೆ (get_float)
    {
	float_T	f;

	*arg += string2float(*arg, &f, skip_quotes);
	ಆದರೆ (evaluate)
	{
	    rettv->v_type = VAR_FLOAT;
	    rettv->vval.v_float = f;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    ಆದರೆ (**arg == '0' && ((*arg)[1] == 'z' || (*arg)[1] == 'Z'))
    {
	char_u  *bp;
	blob_T  *blob = ಖಾಲಿ;  // init for gcc

	// Blob constant: 0z0123456789abcdef
	ಆದರೆ (evaluate)
	    blob = blob_alloc();
	ಗಾಗಿ (bp = *arg + 2; vim_isxdigit(bp[0]); bp += 2)
	{
	    ಆದರೆ (!vim_isxdigit(bp[1]))
	    {
		ಆದರೆ (blob != ಖಾಲಿ)
		{
		    emsg(_(e_blob_literal_should_have_an_even_ಸಂಖ್ಯೆ_ದ_hex_characters));
		    ga_clear(&blob->bv_ga);
		    VIM_CLEAR(blob);
		}
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    ಆದರೆ (blob != ಖಾಲಿ)
		ga_append(&blob->bv_ga,
			     (hex2nr(*bp) << 4) + hex2nr(*(bp+1)));
	    ಆದರೆ (bp[2] == '.' && vim_isxdigit(bp[3]))
		++bp;
	}
	ಆದರೆ (blob != ಖಾಲಿ)
	    rettv_blob_ಗಣ(rettv, blob);
	*arg = bp;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	varnumber_T	n;

	// decimal, hex or octal number
	vim_str2nr(*arg, ಖಾಲಿ, &len, skip_quotes
		      ? STR2NR_NO_OCT + STR2NR_QUOTE
		      : STR2NR_ALL, &n, ಖಾಲಿ, 0, TRUE, ಖಾಲಿ);
	ಆದರೆ (len == 0)
	{
	    ಆದರೆ (evaluate)
		semsg(_(e_invalid_ಅಭಿವ್ಯಕ್ತಿ_str), *arg);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	*arg += len;
	ಆದರೆ (evaluate)
	{
	    rettv->v_type = VAR_ಸಂಖ್ಯೆ;
	    rettv->vval.v_ಸಂಖ್ಯೆ = n;
	}
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Evaluate a string constant and put the result in "rettv".
 * "*arg" points to the double quote or to after it when "interpolate" is TRUE.
 * When "interpolate" is TRUE reduce "{{" to "{", reduce "}}" to "}" and stop
 * at a single "{".
 * Return OK or FAIL.
 */
    ಪೂರ್ಣಾಂಕ
eval_string(char_u **arg, typval_T *rettv, ಪೂರ್ಣಾಂಕ evaluate, ಪೂರ್ಣಾಂಕ interpolate)
{
    char_u	*p;
    char_u	*ಕೊನೆ;
    ಪೂರ್ಣಾಂಕ		extra = interpolate ? 1 : 0;
    ಪೂರ್ಣಾಂಕ		off = interpolate ? 0 : 1;
    ಪೂರ್ಣಾಂಕ		len;

    // Find the end of the string, skipping backslashed characters.
    ಗಾಗಿ (p = *arg + off; *p != NUL && *p != '"'; MB_PTR_ADV(p))
    {
	ಆದರೆ (*p == '\\' && p[1] != NUL)
	{
	    ++p;
	    // A "\<x>" form occupies at least 4 characters, and produces up
	    // to 9 characters (6 for the char and 3 for a modifier):
	    // reserve space for 5 extra.
	    ಆದರೆ (*p == '<')
	    {
		ಪೂರ್ಣಾಂಕ		modifiers = 0;
		ಪೂರ್ಣಾಂಕ		flags = FSK_KEYCODE | FSK_ಒಳ_STRING;

		extra += 5;

		// Skip to the '>' to avoid using '{' inside for string
		// interpolation.
		ಆದರೆ (p[1] != '*')
		    flags |= FSK_SIMPLIFY;
		ಆದರೆ (ಕಂಡುಹಿಡಿ_special_ಕೀಲಿ(&p, &modifiers, flags, ಖಾಲಿ) != 0)
		    --p;  // leave "p" on the ">"
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (interpolate && (*p == '{' || *p == '}'))
	{
	    ಆದರೆ (*p == '{' && p[1] != '{') // start of expression
		ನಿಲ್ಲಿಸು;
	    ++p;
	    ಆದರೆ (p[-1] == '}' && *p != '}') // single '}' is an error
	    {
		semsg(_(e_stray_closing_curly_str), *arg);
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    --extra;  // "{{" becomes "{", "}}" becomes "}"
	}
    }

    ಆದರೆ (*p != '"' && !(interpolate && *p == '{'))
    {
	semsg(_(e_missing_double_quote_str), *arg);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    // If only parsing, set *arg and return here
    ಆದರೆ (!evaluate)
    {
	*arg = p + off;
	ಹಿಂತಿರುಗಿ OK;
    }

    // Copy the string into allocated memory, handling backslashed
    // characters.
    rettv->v_type = VAR_STRING;
    len = (ಪೂರ್ಣಾಂಕ)(p - *arg + extra);
    rettv->vval.v_string = alloc(len);
    ಆದರೆ (rettv->vval.v_string == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    ಕೊನೆ = rettv->vval.v_string;

    ಗಾಗಿ (p = *arg + off; *p != NUL && *p != '"'; )
    {
	ಆದರೆ (*p == '\\')
	{
	    ಆಯ್ಕೆ (*++p)
	    {
		ಸ್ಥಿತಿ 'b': *ಕೊನೆ++ = BS; ++p; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 'e': *ಕೊನೆ++ = ESC; ++p; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 'f': *ಕೊನೆ++ = FF; ++p; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 'n': *ಕೊನೆ++ = NL; ++p; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 'r': *ಕೊನೆ++ = CAR; ++p; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 't': *ಕೊನೆ++ = TAB; ++p; ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ 'X': // hex: "\x1", "\x12"
		ಸ್ಥಿತಿ 'x':
		ಸ್ಥಿತಿ 'u': // Unicode: "\u0023"
		ಸ್ಥಿತಿ 'U':
			  ಆದರೆ (vim_isxdigit(p[1]))
			  {
			      ಪೂರ್ಣಾಂಕ	n, nr;
			      ಪೂರ್ಣಾಂಕ	c = ಸುರಕ್ಷಿತ_toupper(*p);

			      ಆದರೆ (c == 'X')
				  n = 2;
			      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == 'u')
				  n = 4;
			      ಇಲ್ಲದಿದ್ದರೆ
				  n = 8;
			      nr = 0;
			      ಆಗ (--n >= 0 && vim_isxdigit(p[1]))
			      {
				  ++p;
				  nr = (nr << 4) + hex2nr(*p);
			      }
			      ++p;
			      // For "\u" store the number according to
			      // 'encoding'.
			      ಆದರೆ (c != 'X')
				  ಕೊನೆ += (*mb_char2bytes)(nr, ಕೊನೆ);
			      ಇಲ್ಲದಿದ್ದರೆ
				  *ಕೊನೆ++ = nr;
			  }
			  ನಿಲ್ಲಿಸು;

			  // octal: "\1", "\12", "\123"
		ಸ್ಥಿತಿ '0':
		ಸ್ಥಿತಿ '1':
		ಸ್ಥಿತಿ '2':
		ಸ್ಥಿತಿ '3':
		ಸ್ಥಿತಿ '4':
		ಸ್ಥಿತಿ '5':
		ಸ್ಥಿತಿ '6':
		ಸ್ಥಿತಿ '7': *ಕೊನೆ = *p++ - '0';
			  ಆದರೆ (*p >= '0' && *p <= '7')
			  {
			      *ಕೊನೆ = (*ಕೊನೆ << 3) + *p++ - '0';
			      ಆದರೆ (*p >= '0' && *p <= '7')
				  *ಕೊನೆ = (*ಕೊನೆ << 3) + *p++ - '0';
			  }
			  ++ಕೊನೆ;
			  ನಿಲ್ಲಿಸು;

			  // Special key, e.g.: "\<C-W>"
		ಸ್ಥಿತಿ '<':
			  {
			      ಪೂರ್ಣಾಂಕ flags = FSK_KEYCODE | FSK_ಒಳ_STRING;

			      ಆದರೆ (p[1] != '*')
				  flags |= FSK_SIMPLIFY;
			      extra = trans_special(&p, ಕೊನೆ, flags, FALSE, ಖಾಲಿ);
			      ಆದರೆ (extra != 0)
			      {
				  ಕೊನೆ += extra;
				  ಆದರೆ (ಕೊನೆ >= rettv->vval.v_string + len)
				      iemsg("eval_string() used more space than allocated");
				  ನಿಲ್ಲಿಸು;
			      }
			  }
			  // FALLTHROUGH

		ಪೂರ್ವನಿಯೋಜಿತ: MB_COPY_CHAR(p, ಕೊನೆ);
			  ನಿಲ್ಲಿಸು;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (interpolate && (*p == '{' || *p == '}'))
	    {
		ಆದರೆ (*p == '{' && p[1] != '{') // start of expression
		    ನಿಲ್ಲಿಸು;
		++p;  // reduce "{{" to "{" and "}}" to "}"
	    }
	    MB_COPY_CHAR(p, ಕೊನೆ);
	}
    }
    *ಕೊನೆ = NUL;
    ಆದರೆ (*p == '"' && !interpolate)
	++p;
    *arg = p;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Allocate a variable for a 'str''ing' constant.
 * When "interpolate" is TRUE reduce "{{" to "{" and stop at a single "{".
 * Return OK when a "rettv" was set to the string.
 * Return FAIL on error, "rettv" is not set.
 */
    ಪೂರ್ಣಾಂಕ
eval_lit_string(char_u **arg, typval_T *rettv, ಪೂರ್ಣಾಂಕ evaluate, ಪೂರ್ಣಾಂಕ interpolate)
{
    char_u	*p;
    char_u	*str;
    ಪೂರ್ಣಾಂಕ		reduce = interpolate ? -1 : 0;
    ಪೂರ್ಣಾಂಕ		off = interpolate ? 0 : 1;

    // Find the end of the string, skipping ''.
    ಗಾಗಿ (p = *arg + off; *p != NUL; MB_PTR_ADV(p))
    {
	ಆದರೆ (*p == '\'')
	{
	    ಆದರೆ (p[1] != '\'')
		ನಿಲ್ಲಿಸು;
	    ++reduce;
	    ++p;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (interpolate)
	{
	    ಆದರೆ (*p == '{')
	    {
		ಆದರೆ (p[1] != '{')
		    ನಿಲ್ಲಿಸು;
		++p;
		++reduce;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == '}')
	    {
		++p;
		ಆದರೆ (*p != '}')
		{
		    semsg(_(e_stray_closing_curly_str), *arg);
		    ಹಿಂತಿರುಗಿ FAIL;
		}
		++reduce;
	    }
	}
    }

    ಆದರೆ (*p != '\'' && !(interpolate && *p == '{'))
    {
	semsg(_(e_missing_single_quote_str), *arg);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    // If only parsing return after setting "*arg"
    ಆದರೆ (!evaluate)
    {
	*arg = p + off;
	ಹಿಂತಿರುಗಿ OK;
    }

    // Copy the string into allocated memory, handling '' to ' reduction and
    // any expressions.
    str = alloc((p - *arg) - reduce);
    ಆದರೆ (str == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = str;

    ಗಾಗಿ (p = *arg + off; *p != NUL; )
    {
	ಆದರೆ (*p == '\'')
	{
	    ಆದರೆ (p[1] != '\'')
		ನಿಲ್ಲಿಸು;
	    ++p;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (interpolate && (*p == '{' || *p == '}'))
	{
	    ಆದರೆ (*p == '{' && p[1] != '{')
		ನಿಲ್ಲಿಸು;
	    ++p;
	}
	MB_COPY_CHAR(p, str);
    }
    *str = NUL;
    *arg = p + off;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Evaluate a single or double quoted string possibly containing expressions.
 * "arg" points to the '$'.  The result is put in "rettv".
 * Returns OK or FAIL.
 */
    ಪೂರ್ಣಾಂಕ
eval_interp_string(char_u **arg, typval_T *rettv, ಪೂರ್ಣಾಂಕ evaluate)
{
    typval_T	tv;
    ಪೂರ್ಣಾಂಕ		ret = OK;
    ಪೂರ್ಣಾಂಕ		quote;
    garray_T	ga;
    char_u	*p;

    ga_init2(&ga, 1, 80);

    // *arg is on the '$' character, move it to the first string character.
    ++*arg;
    quote = **arg;
    ++*arg;

    ಗಾಗಿ (;;)
    {
	// Get the string up to the matching quote or to a single '{'.
	// "arg" is advanced to either the quote or the '{'.
	ಆದರೆ (quote == '"')
	    ret = eval_string(arg, &tv, evaluate, TRUE);
	ಇಲ್ಲದಿದ್ದರೆ
	    ret = eval_lit_string(arg, &tv, evaluate, TRUE);
	ಆದರೆ (ret == FAIL)
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (evaluate)
	{
	    ga_concat(&ga, tv.vval.v_string);
	    clear_tv(&tv);
	}

	ಆದರೆ (**arg != '{')
	{
	    // found terminating quote
	    ++*arg;
	    ನಿಲ್ಲಿಸು;
	}
	p = eval_one_ಅಭಿವ್ಯಕ್ತಿ_ಒಳ_str(*arg, &ga, evaluate);
	ಆದರೆ (p == ಖಾಲಿ)
	{
	    ret = FAIL;
	    ನಿಲ್ಲಿಸು;
	}
	*arg = p;
    }

    rettv->v_type = VAR_STRING;
    ಆದರೆ (ret == FAIL || !evaluate || ga_append(&ga, NUL) == FAIL)
    {
	ga_clear(&ga);
	rettv->vval.v_string = ಖಾಲಿ;
	ಹಿಂತಿರುಗಿ ret;
    }

    rettv->vval.v_string = ga.ga_ದತ್ತಾಂಶ;
    ಹಿಂತಿರುಗಿ OK;
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
    ಪೂರ್ಣಾಂಕ		copyID)
{
    ಹಿಂತಿರುಗಿ echo_string_core(tv, tofree, numbuf, copyID, FALSE, TRUE, FALSE);
}

/*
 * Get the value of an environment variable.
 * "arg" is pointing to the '$'.  It is advanced to after the name.
 * If the environment variable was not set, silently assume it is empty.
 * Return FAIL if the name is invalid.
 */
    ಪೂರ್ಣಾಂಕ
eval_env_var(char_u **arg, typval_T *rettv, ಪೂರ್ಣಾಂಕ evaluate)
{
    char_u	*string = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		len;
    ಪೂರ್ಣಾಂಕ		cc;
    char_u	*name;
    ಪೂರ್ಣಾಂಕ		mustfree = FALSE;

    ++*arg;
    name = *arg;
    len = get_env_len(arg);
    ಆದರೆ (evaluate)
    {
	ಆದರೆ (len == 0)
	    ಹಿಂತಿರುಗಿ FAIL; // invalid empty name

	cc = name[len];
	name[len] = NUL;
	// first try vim_getenv(), fast for normal environment vars
	string = vim_getenv(name, &mustfree);
	ಆದರೆ (string != ಖಾಲಿ && *string != NUL)
	{
	    ಆದರೆ (!mustfree)
		string = vim_strsave(string);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (mustfree)
		vim_free(string);

	    // next try expanding things like $VIM and ${HOME}
	    string = expand_env_save(name - 1);
	    ಆದರೆ (string != ಖಾಲಿ && *string == '$')
		VIM_CLEAR(string);
	}
	name[len] = cc;

	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = string;
	rettv->v_ಬೀಗ = 0;
    }

    ಹಿಂತಿರುಗಿ OK;
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
    ಪೂರ್ಣಾಂಕ		did_emsg_before = did_emsg;

    ಆದರೆ (argvars[0].v_type != VAR_STRING || !ಒಳ_vim9script())
	lnum = (linenr_T)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[0], ಖಾಲಿ);
    ಆದರೆ (lnum <= 0 && did_emsg_before == did_emsg
					    && argvars[0].v_type != VAR_ಸಂಖ್ಯೆ)
    {
	ಪೂರ್ಣಾಂಕ	fnum;
	pos_T	*fp;

	// no valid number, try using arg like line()
	fp = var2fpos(&argvars[0], TRUE, &fnum, FALSE);
	ಆದರೆ (fp != ಖಾಲಿ)
	    lnum = fp->lnum;
    }
    ಹಿಂತಿರುಗಿ lnum;
}

/*
 * Get the lnum from the first argument.
 * Also accepts "$", then "buf" is used.
 * Returns 0 on error.
 */
    linenr_T
tv_get_lnum_buf(typval_T *argvars, buf_T *buf)
{
    ಆದರೆ (argvars[0].v_type == VAR_STRING
	    && argvars[0].vval.v_string != ಖಾಲಿ
	    && argvars[0].vval.v_string[0] == '$'
	    && argvars[0].vval.v_string[1] == NUL
	    && buf != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    ಹಿಂತಿರುಗಿ (linenr_T)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[0], ಖಾಲಿ);
}

/*
 * Get buffer by number or pattern.
 */
    buf_T *
tv_get_buf(typval_T *tv, ಪೂರ್ಣಾಂಕ curtab_only)
{
    char_u	*name = tv->vval.v_string;
    buf_T	*buf;

    ಆದರೆ (tv->v_type == VAR_ಸಂಖ್ಯೆ)
	ಹಿಂತಿರುಗಿ buflist_findnr((ಪೂರ್ಣಾಂಕ)tv->vval.v_ಸಂಖ್ಯೆ);
    ಆದರೆ (tv->v_type != VAR_STRING)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಆದರೆ (name == ಖಾಲಿ || *name == NUL)
	ಹಿಂತಿರುಗಿ curbuf;
    ಆದರೆ (name[0] == '$' && name[1] == NUL)
	ಹಿಂತಿರುಗಿ lastbuf;

    buf = buflist_ಕಂಡುಹಿಡಿ_by_name(name, curtab_only);

    // If not found, try expanding the name, like done for bufexists().
    ಆದರೆ (buf == ಖಾಲಿ)
	buf = ಕಂಡುಹಿಡಿ_ಬಫರ್(tv);

    ಹಿಂತಿರುಗಿ buf;
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
    ಆದರೆ (buf == ಖಾಲಿ
	    && tv->v_type != VAR_ಸಂಖ್ಯೆ
	    && tv->v_type != VAR_STRING)
	// issue errmsg for type error
	(ಶೂನ್ಯ)tv_get_ಸಂಖ್ಯೆ(tv);
    ಹಿಂತಿರುಗಿ buf;
}

/*
 * Check that repeating "slen" items "n" times, does not
 * overflow the int used for the resulting length.
 */
    ಪೂರ್ಣಾಂಕ
check_repeat_ಎಣಿಕೆ(varnumber_T slen, varnumber_T n)
{
    // Use a division so that the multiplication cannot overflow.
    ಆದರೆ (n <= (varnumber_T)ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ / slen)
	ಹಿಂತಿರುಗಿ OK;

    ಅಕ್ಷರ	buf[NUMBUFLEN];

    vim_snprintf(buf, ಗಾತ್ರ(buf), "%lld", (ದೀರ್ಘ ದೀರ್ಘ)n);
    semsg(_(e_val_too_large), buf);
    ಹಿಂತಿರುಗಿ FAIL;
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_EVAL
