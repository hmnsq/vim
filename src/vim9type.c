/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * vim9type.c: handling of types
 */

#ವ್ಯಾಖ್ಯಾನಿಸು USING_FLOAT_STUFF
#ಅನ್ತರ್ಭಾವ "vim.h"

#ಆದರೆ defined(FEAT_EVAL)

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VMS
# include <float.h>
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Allocate memory for a type_T and add the pointer to type_gap, so that it can
 * be easily freed later.
 */
    type_T *
get_type_ptr(garray_T *type_gap)
{
    type_T *type;

    ಆದರೆ (ga_grow(type_gap, 1) == FAIL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    type = ALLOC_CLEAR_ONE(type_T);
    ಆದರೆ (type == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ((type_T **)type_gap->ga_ದತ್ತಾಂಶ)[type_gap->ga_len] = type;
    ++type_gap->ga_len;
    ಹಿಂತಿರುಗಿ type;
}

/*
 * Make a shallow copy of "type".
 * When allocation fails returns "type".
 */
    type_T *
copy_type(type_T *type, garray_T *type_gap)
{
    type_T *copy = get_type_ptr(type_gap);

    ಆದರೆ (copy == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ type;
    *copy = *type;
    copy->tt_flags &= ~TTFLAG_STATIC;

    ಆದರೆ (type->tt_ವಾದಗಳು != ಖಾಲಿ
	   && func_type_ಕೂಡಿಸು_arg_types(copy, type->tt_argcount, type_gap) == OK)
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < type->tt_argcount; ++i)
	    copy->tt_ವಾದಗಳು[i] = type->tt_ವಾದಗಳು[i];

    ಹಿಂತಿರುಗಿ copy;
}

/*
 * Inner part of copy_type_deep().
 * When allocation fails returns "type".
 */
    ಸ್ಥಿರ type_T *
copy_type_deep_rec(type_T *type, garray_T *type_gap, garray_T *ನೋಡಿದ_types)
{
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ನೋಡಿದ_types->ga_len; ++i)
	ಆದರೆ (((type_T **)ನೋಡಿದ_types->ga_ದತ್ತಾಂಶ)[i * 2] == type)
	    // seen this type before, return the copy we made
	    ಹಿಂತಿರುಗಿ ((type_T **)ನೋಡಿದ_types->ga_ದತ್ತಾಂಶ)[i * 2 + 1];

    type_T *copy = copy_type(type, type_gap);
    ಆದರೆ (ga_grow(ನೋಡಿದ_types, 1) == FAIL)
	ಹಿಂತಿರುಗಿ copy;
    ((type_T **)ನೋಡಿದ_types->ga_ದತ್ತಾಂಶ)[ನೋಡಿದ_types->ga_len * 2] = type;
    ((type_T **)ನೋಡಿದ_types->ga_ದತ್ತಾಂಶ)[ನೋಡಿದ_types->ga_len * 2 + 1] = copy;
    ++ನೋಡಿದ_types->ga_len;

    ಆದರೆ (copy->tt_member != ಖಾಲಿ)
	copy->tt_member = copy_type_deep_rec(copy->tt_member,
							 type_gap, ನೋಡಿದ_types);
    ಆದರೆ (type->tt_ವಾದಗಳು != ಖಾಲಿ)
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < type->tt_argcount; ++i)
	    copy->tt_ವಾದಗಳು[i] = copy_type_deep_rec(copy->tt_ವಾದಗಳು[i],
							 type_gap, ನೋಡಿದ_types);

    ಹಿಂತಿರುಗಿ copy;
}

/*
 * Make a deep copy of "type".
 * When allocation fails returns "type".
 */
    type_T *
copy_type_deep(type_T *type, garray_T *type_gap)
{
    garray_T ನೋಡಿದ_types;
    // stores type pairs : a type we have seen and the copy used
    ga_init2(&ನೋಡಿದ_types, ಗಾತ್ರ(type_T *) * 2, 20);

    type_T *res = copy_type_deep_rec(type, type_gap, &ನೋಡಿದ_types);

    ga_clear(&ನೋಡಿದ_types);
    ಹಿಂತಿರುಗಿ res;
}

    ಶೂನ್ಯ
clear_type_list(garray_T *gap)
{
    ಆಗ (gap->ga_len > 0)
	vim_free(((type_T **)gap->ga_ದತ್ತಾಂಶ)[--gap->ga_len]);
    ga_clear(gap);
}

    ಶೂನ್ಯ
clear_func_type_list(garray_T *gap, type_T **func_type)
{
    ಆಗ (gap->ga_len > 0)
    {
	// func_type pointing to the uf_type_list, so reset pointer
	ಆದರೆ (*func_type == ((type_T **)gap->ga_ದತ್ತಾಂಶ)[--gap->ga_len])
	    *func_type = &t_func_any;
	vim_free(((type_T **)gap->ga_ದತ್ತಾಂಶ)[gap->ga_len]);
    }
    ga_clear(gap);
}

/*
 * Take a type that is using entries in a growarray and turn it into a type
 * with allocated entries.
 */
    type_T *
alloc_type(type_T *type)
{
    type_T *ret;

    ಆದರೆ (type == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // A fixed type never contains allocated types, return as-is.
    ಆದರೆ (type->tt_flags & TTFLAG_STATIC)
	ಹಿಂತಿರುಗಿ type;

    ret = ALLOC_ONE(type_T);
    ಆದರೆ (ret == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    *ret = *type;

    ಆದರೆ (ret->tt_member != ಖಾಲಿ)
	ret->tt_member = alloc_type(ret->tt_member);

    ಆದರೆ (type->tt_argcount > 0 && type->tt_ವಾದಗಳು != ಖಾಲಿ)
    {
	ಪೂರ್ಣಾಂಕ i;

	ret->tt_ವಾದಗಳು = ALLOC_MULT(type_T *, type->tt_argcount);
	ಆದರೆ (ret->tt_ವಾದಗಳು != ಖಾಲಿ)
	    ಗಾಗಿ (i = 0; i < type->tt_argcount; ++i)
		ret->tt_ವಾದಗಳು[i] = alloc_type(type->tt_ವಾದಗಳು[i]);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ret->tt_ವಾದಗಳು = ಖಾಲಿ;

    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Free a type that was created with alloc_type().
 */
    ಶೂನ್ಯ
free_type(type_T *type)
{
    ಪೂರ್ಣಾಂಕ i;

    ಆದರೆ (type == ಖಾಲಿ || (type->tt_flags & TTFLAG_STATIC))
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (type->tt_ವಾದಗಳು != ಖಾಲಿ)
    {
	ಗಾಗಿ (i = 0; i < type->tt_argcount; ++i)
	    free_type(type->tt_ವಾದಗಳು[i]);
	vim_free(type->tt_ವಾದಗಳು);
    }

    free_type(type->tt_member);

    vim_free(type);
}

/*
 * Return TRUE if "type" is to be recursed into for setting the type.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಗಣ_tv_type_recurse(type_T *type)
{
    ಹಿಂತಿರುಗಿ type->tt_member != ಖಾಲಿ
		&& (type->tt_member->tt_type == VAR_DICT
				|| type->tt_member->tt_type == VAR_LIST)
		&& type->tt_member->tt_member != ಖಾಲಿ
		&& type->tt_member->tt_member != &t_any
		&& type->tt_member->tt_member != &t_unknown;
}

/*
 * Set the type of Dict "d" to "type"
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_tv_type_dict(dict_T *d, type_T *type)
{
    ಆದರೆ (d->dv_type == type)
	ಹಿಂತಿರುಗಿ;

    free_type(d->dv_type);
    d->dv_type = alloc_type(type);

    // Need to recursively set the type of dict items?
    ಆದರೆ (!ಗಣ_tv_type_recurse(type))
	ಹಿಂತಿರುಗಿ;

    ಪೂರ್ಣಾಂಕ		todo = (ಪೂರ್ಣಾಂಕ)d->dv_hashtab.ht_ಬಳಸಿದ;
    hashitem_T	*hi;
    dictitem_T	*di;

    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(&d->dv_hashtab, hi, todo)
    {
	ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
	{
	    --todo;
	    di = HI2DI(hi);
	    ಗಣ_tv_type(&di->di_tv, type->tt_member);
	}
    }
}

/*
 * Set the type of List "l" to "type"
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_tv_type_list(list_T *l, type_T *type)
{
    ಆದರೆ (l->lv_type == type)
	ಹಿಂತಿರುಗಿ;

    free_type(l->lv_type);
    l->lv_type = alloc_type(type);

    // Need to recursively set the type of list items?
    ಆದರೆ (l->lv_ಮೊದಲ == &range_list_ಅಂಶ || !ಗಣ_tv_type_recurse(type))
	ಹಿಂತಿರುಗಿ;

    listitem_T	*li;

    FOR_ALL_LIST_ವಸ್ತುಗಳು(l, li)
	ಗಣ_tv_type(&li->li_tv, type->tt_member);
}

/*
 * Set the type of Tuple "tuple" to "type"
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_tv_type_tuple(tuple_T *tuple, type_T *type)
{
    ಆದರೆ (tuple->tv_type == type)
	ಹಿಂತಿರುಗಿ;

    free_type(tuple->tv_type);
    tuple->tv_type = alloc_type(type);

    ಆದರೆ (type->tt_argcount <= 0)
	ಹಿಂತಿರುಗಿ;

    // recursively set the type of list items
    type_T	*ಅಂಶ_type;
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < tuple_len(tuple); i++)
    {
	ಆದರೆ ((type->tt_flags & TTFLAG_VARARGS) && (i >= type->tt_argcount - 1))
	    // For a variadic tuple, the last type is a List.  So use the
	    // List member type.
	    ಅಂಶ_type = type->tt_ವಾದಗಳು[type->tt_argcount - 1]->tt_member;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಅಂಶ_type = type->tt_ವಾದಗಳು[i];

	ಗಣ_tv_type(TUPLE_ಅಂಶ(tuple, i), ಅಂಶ_type);
    }
}

/*
 * Set the type of "tv" to "type" if it is a list or tuple or dict.
 */
    ಶೂನ್ಯ
ಗಣ_tv_type(typval_T *tv, type_T *type)
{
    ಆದರೆ (type->tt_type == VAR_ANY)
	// If the variable type is "any", then keep the value type.
	// e.g.  var x: any = [1, 2] or var y: any = {v: 1}
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (tv->v_type == VAR_DICT && tv->vval.v_dict != ಖಾಲಿ)
	ಗಣ_tv_type_dict(tv->vval.v_dict, type);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tv->v_type == VAR_LIST && tv->vval.v_list != ಖಾಲಿ)
	ಗಣ_tv_type_list(tv->vval.v_list, type);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tv->v_type == VAR_TUPLE && tv->vval.v_tuple != ಖಾಲಿ)
	ಗಣ_tv_type_tuple(tv->vval.v_tuple, type);
}

/*
 * For a tuple type, reserve space for "typecount" types (including the
 * repeated type).
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
tuple_type_ಕೂಡಿಸು_types(
    type_T	*tupletype,
    ಪೂರ್ಣಾಂಕ		typecount,
    garray_T	*type_gap)
{
    // To make it easy to free the space needed for the types, add the
    // pointer to type_gap.
    ಆದರೆ (ga_grow(type_gap, 1) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;
    tupletype->tt_ವಾದಗಳು = alloc_clear_id(ಗಾತ್ರ(type_T *) * typecount,
							  aid_tuple_type_ವಾದಗಳು);
    ಆದರೆ (tupletype->tt_ವಾದಗಳು == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    ((type_T **)type_gap->ga_ದತ್ತಾಂಶ)[type_gap->ga_len] =
						(ಶೂನ್ಯ *)tupletype->tt_ವಾದಗಳು;
    ++type_gap->ga_len;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Get a list type, based on the member item type in "member_type".
 */
    type_T *
get_list_type(type_T *member_type, garray_T *type_gap)
{
    type_T *type;

    // recognize commonly used types
    // A generic type is t_any initially before being set to a concrete type
    // later.  So don't use the static t_list_any for a generic type.
    ಆದರೆ (member_type == ಖಾಲಿ || (member_type->tt_type == VAR_ANY
					&& !IS_GENERIC_TYPE(member_type)))
	ಹಿಂತಿರುಗಿ &t_list_any;
    ಆದರೆ (member_type->tt_type == VAR_VOID
	    || member_type->tt_type == VAR_UNKNOWN)
	ಹಿಂತಿರುಗಿ &t_list_ಖಾಲಿಗಳು;
    ಆದರೆ (member_type->tt_type == VAR_BOOL)
	ಹಿಂತಿರುಗಿ &t_list_bool;
    ಆದರೆ (member_type->tt_type == VAR_ಸಂಖ್ಯೆ)
	ಹಿಂತಿರುಗಿ &t_list_ಸಂಖ್ಯೆ;
    ಆದರೆ (member_type->tt_type == VAR_STRING)
	ಹಿಂತಿರುಗಿ &t_list_string;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    ಆದರೆ (type == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ &t_any;
    type->tt_type = VAR_LIST;
    type->tt_member = member_type;
    type->tt_argcount = 0;
    type->tt_ವಾದಗಳು = ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ type;
}

/*
 * Create and return a tuple type from the tuple item types in
 * "tuple_types_ga".
 */
    type_T *
get_tuple_type(garray_T *tuple_types_gap, garray_T *type_gap)
{
    type_T	*type;
    type_T	**tuple_types = tuple_types_gap->ga_ದತ್ತಾಂಶ;
    ಪೂರ್ಣಾಂಕ		typecount = tuple_types_gap->ga_len;

    // recognize commonly used types
    ಆದರೆ (typecount == 0)
	ಹಿಂತಿರುಗಿ &t_tuple_any;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    ಆದರೆ (type == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ &t_any;
    type->tt_type = VAR_TUPLE;
    type->tt_member = ಖಾಲಿ;
    ಆದರೆ (typecount > 0)
    {
	ಆದರೆ (tuple_type_ಕೂಡಿಸು_types(type, typecount, type_gap) == FAIL)
	    ಹಿಂತಿರುಗಿ &t_any;
	mch_memmove(type->tt_ವಾದಗಳು, tuple_types, ಗಾತ್ರ(type_T *) * typecount);
    }
    type->tt_argcount = typecount;
    type->tt_flags = 0;

    ಹಿಂತಿರುಗಿ type;
}

/*
 * Get a dict type, based on the member item type in "member_type".
 */
    type_T *
get_dict_type(type_T *member_type, garray_T *type_gap)
{
    type_T *type;

    // recognize commonly used types
    // A generic type is t_any initially before being set to a concrete type
    // later.  So don't use the static t_dict_any for a generic type.
    ಆದರೆ (member_type == ಖಾಲಿ || (member_type->tt_type == VAR_ANY
					&& !IS_GENERIC_TYPE(member_type)))
	ಹಿಂತಿರುಗಿ &t_dict_any;
    ಆದರೆ (member_type->tt_type == VAR_VOID
	    || member_type->tt_type == VAR_UNKNOWN)
	ಹಿಂತಿರುಗಿ &t_dict_ಖಾಲಿಗಳು;
    ಆದರೆ (member_type->tt_type == VAR_BOOL)
	ಹಿಂತಿರುಗಿ &t_dict_bool;
    ಆದರೆ (member_type->tt_type == VAR_ಸಂಖ್ಯೆ)
	ಹಿಂತಿರುಗಿ &t_dict_ಸಂಖ್ಯೆ;
    ಆದರೆ (member_type->tt_type == VAR_STRING)
	ಹಿಂತಿರುಗಿ &t_dict_string;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    ಆದರೆ (type == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ &t_any;
    type->tt_type = VAR_DICT;
    type->tt_member = member_type;
    type->tt_argcount = 0;
    type->tt_ವಾದಗಳು = ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ type;
}

/*
 * Allocate a new type for a function.
 */
    type_T *
alloc_func_type(type_T *ret_type, ಪೂರ್ಣಾಂಕ argcount, garray_T *type_gap)
{
    type_T *type = get_type_ptr(type_gap);

    ಆದರೆ (type == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ &t_any;
    type->tt_type = VAR_FUNC;
    type->tt_member = ret_type == ಖಾಲಿ ? &t_unknown : ret_type;
    type->tt_argcount = argcount;
    type->tt_ವಾದಗಳು = ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ type;
}

/*
 * Allocate a new type for a tuple.
 */
    ಸ್ಥಿರ type_T *
alloc_tuple_type(ಪೂರ್ಣಾಂಕ typecount, garray_T *type_gap)
{
    type_T *type = get_type_ptr(type_gap);

    ಆದರೆ (type == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ &t_any;
    type->tt_type = VAR_TUPLE;
    type->tt_member = ಖಾಲಿ;
    type->tt_argcount = typecount;
    type->tt_ವಾದಗಳು = ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ type;
}

/*
 * Get a function type, based on the return type "ret_type".
 * "argcount" must be -1 or 0, a predefined type can be used.
 */
    type_T *
get_func_type(type_T *ret_type, ಪೂರ್ಣಾಂಕ argcount, garray_T *type_gap)
{
    // recognize commonly used types
    ಆದರೆ (ret_type == &t_unknown || ret_type == ಖಾಲಿ)
    {
	// (argcount == 0) is not possible
	ಹಿಂತಿರುಗಿ &t_func_unknown;
    }
    ಆದರೆ (ret_type == &t_void)
    {
	ಆದರೆ (argcount == 0)
	    ಹಿಂತಿರುಗಿ &t_func_0_void;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ &t_func_void;
    }
    ಆದರೆ (ret_type == &t_any)
    {
	ಆದರೆ (argcount == 0)
	    ಹಿಂತಿರುಗಿ &t_func_0_any;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ &t_func_any;
    }
    ಆದರೆ (ret_type == &t_ಸಂಖ್ಯೆ)
    {
	ಆದರೆ (argcount == 0)
	    ಹಿಂತಿರುಗಿ &t_func_0_ಸಂಖ್ಯೆ;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ &t_func_ಸಂಖ್ಯೆ;
    }
    ಆದರೆ (ret_type == &t_string)
    {
	ಆದರೆ (argcount == 0)
	    ಹಿಂತಿರುಗಿ &t_func_0_string;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ &t_func_string;
    }

    ಹಿಂತಿರುಗಿ alloc_func_type(ret_type, argcount, type_gap);
}

/*
 * For a function type, reserve space for "argcount" argument types (including
 * vararg).
 */
    ಪೂರ್ಣಾಂಕ
func_type_ಕೂಡಿಸು_arg_types(
	type_T	    *functype,
	ಪೂರ್ಣಾಂಕ	    argcount,
	garray_T    *type_gap)
{
    // To make it easy to free the space needed for the argument types, add the
    // pointer to type_gap.
    ಆದರೆ (ga_grow(type_gap, 1) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;
    functype->tt_ವಾದಗಳು = ALLOC_CLEAR_MULT(type_T *, argcount);
    ಆದರೆ (functype->tt_ವಾದಗಳು == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    ((type_T **)type_gap->ga_ದತ್ತಾಂಶ)[type_gap->ga_len] =
						     (ಶೂನ್ಯ *)functype->tt_ವಾದಗಳು;
    ++type_gap->ga_len;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Return TRUE if "type" is NULL, any or unknown.
 * This also works for const (comparing with &t_any and &t_unknown doesn't).
 */
    ಪೂರ್ಣಾಂಕ
type_any_or_unknown(type_T *type)
{
    ಹಿಂತಿರುಗಿ type == ಖಾಲಿ || type->tt_type == VAR_ANY
					|| type->tt_type == VAR_UNKNOWN;
}

/*
 * Get a type_T for a "special" typval in "tv".
 */
    ಸ್ಥಿರ type_T *
special_typval2type(typval_T *tv)
{
    ಆಯ್ಕೆ (tv->vval.v_ಸಂಖ್ಯೆ)
    {
	ಸ್ಥಿತಿ VVAL_NULL:
	    ಹಿಂತಿರುಗಿ &t_null;

	ಸ್ಥಿತಿ VVAL_NONE:
	    ಹಿಂತಿರುಗಿ &t_none;

	ಸ್ಥಿತಿ VVAL_TRUE:
	ಸ್ಥಿತಿ VVAL_FALSE:
	    ಹಿಂತಿರುಗಿ &t_bool;

	ಪೂರ್ವನಿಯೋಜಿತ:
	    ಹಿಂತಿರುಗಿ &t_unknown;
    }
}

/*
 * Get a type_T for a List typval in "tv".
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    ಸ್ಥಿರ type_T *
list_typval2type(typval_T *tv, ಪೂರ್ಣಾಂಕ copyID, garray_T *type_gap, ಪೂರ್ಣಾಂಕ flags)
{
    list_T	*l = tv->vval.v_list;
    listitem_T	*li;
    type_T	*member_type = ಖಾಲಿ;

    // An empty list has type list<unknown>, unless the type was specified
    // and is not list<any>.  This matters when assigning to a variable
    // with a specific list type.
    ಆದರೆ (l == ಖಾಲಿ || (l->lv_ಮೊದಲ == ಖಾಲಿ
		&& (l->lv_type == ಖಾಲಿ || l->lv_type->tt_member == &t_any)))
	ಹಿಂತಿರುಗಿ &t_list_ಖಾಲಿಗಳು;

    ಆದರೆ ((flags & TVTT_DO_MEMBER) == 0)
	ಹಿಂತಿರುಗಿ &t_list_any;

    // If the type is list<any> go through the members, it may end up a
    // more specific type.
    ಆದರೆ (l->lv_type != ಖಾಲಿ && (l->lv_ಮೊದಲ == ಖಾಲಿ
		|| (flags & TVTT_ಇನ್ನಷ್ಟು_SPECIFIC) == 0
		|| l->lv_type->tt_member != &t_any))
	// make a copy, lv_type may be freed if the list is freed
	ಹಿಂತಿರುಗಿ copy_type_deep(l->lv_type, type_gap);

    ಆದರೆ (l->lv_ಮೊದಲ == &range_list_ಅಂಶ)
	ಹಿಂತಿರುಗಿ &t_list_ಸಂಖ್ಯೆ;

    ಆದರೆ (l->lv_copyID == copyID)
	// avoid recursion
	ಹಿಂತಿರುಗಿ &t_list_any;

    l->lv_copyID = copyID;

    // Use the common type of all members.
    member_type = typval2type(&l->lv_ಮೊದಲ->li_tv, copyID, type_gap,
							TVTT_DO_MEMBER);
    ಗಾಗಿ (li = l->lv_ಮೊದಲ->li_ಮುಂದಿನ; li != ಖಾಲಿ; li = li->li_ಮುಂದಿನ)
	common_type(typval2type(&li->li_tv, copyID, type_gap, TVTT_DO_MEMBER),
					member_type, &member_type, type_gap);

    // Reset copyID so that a shared reference to this list (not a circular
    // reference) can be processed again to get the correct type.
    l->lv_copyID = 0;

    ಹಿಂತಿರುಗಿ get_list_type(member_type, type_gap);
}

/*
 * Get a type_T for a Tuple typval in "tv".
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    ಸ್ಥಿರ type_T *
tuple_typval2type(typval_T *tv, ಪೂರ್ಣಾಂಕ copyID, garray_T *type_gap, ಪೂರ್ಣಾಂಕ flags)
{
    tuple_T	*tuple = tv->vval.v_tuple;
    ಪೂರ್ಣಾಂಕ		len = tuple_len(tuple);
    type_T	*type = ಖಾಲಿ;

    // An empty tuple has type tuple<unknown>, unless the type was specified
    // and is not tuple<any>.  This matters when assigning to a variable
    // with a specific tuple type.
    ಆದರೆ (tuple == ಖಾಲಿ || (len == 0 && (tuple->tv_type == ಖಾಲಿ
				|| tuple->tv_type->tt_argcount == 0)))
	ಹಿಂತಿರುಗಿ &t_tuple_ಖಾಲಿಗಳು;

    ಆದರೆ ((flags & TVTT_DO_MEMBER) == 0)
	ಹಿಂತಿರುಗಿ &t_tuple_any;

    // If the type is tuple<any> go through the members, it may end up a
    // more specific type.
    ಆದರೆ (tuple->tv_type != ಖಾಲಿ && (len == 0
					|| (flags & TVTT_ಇನ್ನಷ್ಟು_SPECIFIC) == 0))
	// make a copy, tv_type may be freed if the tuple is freed
	ಹಿಂತಿರುಗಿ copy_type_deep(tuple->tv_type, type_gap);

    ಆದರೆ (tuple->tv_copyID == copyID)
	// avoid recursion
	ಹಿಂತಿರುಗಿ &t_tuple_any;

    tuple->tv_copyID = copyID;

    garray_T	tuple_types_ga;
    ga_init2(&tuple_types_ga, ಗಾತ್ರ(type_T *), 10);
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < len; i++)
    {
	type = typval2type(TUPLE_ಅಂಶ(tuple, i), copyID, type_gap,
							TVTT_DO_MEMBER);
	ಆದರೆ (ga_grow(&tuple_types_ga, 1) == FAIL)
	{
	    ga_clear(&tuple_types_ga);
	    // Reset copyID so that a shared reference to this tuple can be
	    // processed again.
	    tuple->tv_copyID = 0;
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
	((type_T **)tuple_types_ga.ga_ದತ್ತಾಂಶ)[tuple_types_ga.ga_len] = type;
	tuple_types_ga.ga_len++;
    }

    type_T *tuple_type = get_tuple_type(&tuple_types_ga, type_gap);
    ga_clear(&tuple_types_ga);

    // Reset copyID so that a shared reference to this tuple (not a circular
    // reference) can be processed again to get the correct type.
    tuple->tv_copyID = 0;

    ಹಿಂತಿರುಗಿ tuple_type;
}

/*
 * Get a type_T for a Dict typval in "tv".
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    ಸ್ಥಿರ type_T *
dict_typval2type(typval_T *tv, ಪೂರ್ಣಾಂಕ copyID, garray_T *type_gap, ಪೂರ್ಣಾಂಕ flags)
{
    dict_iterator_T	iter;
    typval_T		*ಮೌಲ್ಯ;
    dict_T		*d = tv->vval.v_dict;
    type_T		*member_type = ಖಾಲಿ;

    ಆದರೆ (d == ಖಾಲಿ || (d->dv_hashtab.ht_ಬಳಸಿದ == 0 && d->dv_type == ಖಾಲಿ))
	ಹಿಂತಿರುಗಿ &t_dict_ಖಾಲಿಗಳು;

    ಆದರೆ ((flags & TVTT_DO_MEMBER) == 0)
	ಹಿಂತಿರುಗಿ &t_dict_any;

    // If the type is dict<any> go through the members, it may end up a
    // more specific type.
    ಆದರೆ (d->dv_type != ಖಾಲಿ && (d->dv_hashtab.ht_ಬಳಸಿದ == 0
		|| (flags & TVTT_ಇನ್ನಷ್ಟು_SPECIFIC) == 0
		|| d->dv_type->tt_member != &t_any))
	ಹಿಂತಿರುಗಿ d->dv_type;

    ಆದರೆ (d->dv_copyID == copyID)
	// avoid recursion
	ಹಿಂತಿರುಗಿ &t_dict_any;

    d->dv_copyID = copyID;

    // Use the common type of all values.
    dict_iterate_ಆರಂಭ(tv, &iter);
    dict_iterate_ಮುಂದಿನ(&iter, &ಮೌಲ್ಯ);
    member_type = typval2type(ಮೌಲ್ಯ, copyID, type_gap, TVTT_DO_MEMBER);

    ಆಗ (dict_iterate_ಮುಂದಿನ(&iter, &ಮೌಲ್ಯ) != ಖಾಲಿ)
	common_type(typval2type(ಮೌಲ್ಯ, copyID, type_gap, TVTT_DO_MEMBER),
					member_type, &member_type, type_gap);

    // Reset copyID so that a shared reference to this dict (not a circular
    // reference) can be processed again to get the correct type.
    d->dv_copyID = 0;

    ಹಿಂತಿರುಗಿ get_dict_type(member_type, type_gap);
}

/*
 * Get a type_T for a "partial" typval in "tv".
 */
    ಸ್ಥಿರ type_T *
partial_typval2type(typval_T *tv, ufunc_T *ufunc, garray_T *type_gap)
{
    partial_T *pt = tv->vval.v_partial;
    type_T  *type;

    type = get_type_ptr(type_gap);
    ಆದರೆ (type == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    *type = *ufunc->uf_func_type;
    ಆದರೆ (type->tt_argcount >= 0 && pt->pt_argc > 0)
    {
	type->tt_argcount -= pt->pt_argc;
	type->tt_min_argcount -= pt->pt_argc;
	ಆದರೆ (type->tt_argcount > 0 && func_type_ಕೂಡಿಸು_arg_types(type,
		    type->tt_argcount, type_gap) == OK)
	    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < type->tt_argcount; ++i)
		type->tt_ವಾದಗಳು[i] =
		    ufunc->uf_func_type->tt_ವಾದಗಳು[i + pt->pt_argc];
    }
    ಆದರೆ (pt->pt_func != ಖಾಲಿ)
	type->tt_member = pt->pt_func->uf_ret_type;

    ಹಿಂತಿರುಗಿ type;
}

/*
 * Get a type_T for a "class" or an "object" typval in "tv".
 */
    ಸ್ಥಿರ type_T *
oc_typval2type(typval_T *tv)
{
    ಆದರೆ (tv->v_type == VAR_CLASS)
    {
	ಆದರೆ (tv->vval.v_class == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ &t_class;

	ಹಿಂತಿರುಗಿ &tv->vval.v_class->class_type;
    }

    ಆದರೆ (tv->vval.v_object != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ &tv->vval.v_object->obj_class->class_object_type;

    ಹಿಂತಿರುಗಿ &t_object_any;
}

/*
 * Get a type_T for a "function" or a "partial"
 */
    ಸ್ಥಿರ type_T *
fp_typval2type(typval_T *tv, garray_T *type_gap)
{
    char_u	*name = ಖಾಲಿ;
    ufunc_T	*ufunc = ಖಾಲಿ;
    type_T	*type;
    type_T	*member_type = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		argcount = 0;
    ಪೂರ್ಣಾಂಕ		min_argcount = 0;

    ಆದರೆ (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != ಖಾಲಿ)
    {
	ಆದರೆ (tv->vval.v_partial->pt_func != ಖಾಲಿ)
	    ufunc = tv->vval.v_partial->pt_func;
	ಇಲ್ಲದಿದ್ದರೆ
	    name = tv->vval.v_partial->pt_name;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	name = tv->vval.v_string;

    ಆದರೆ (name == ಖಾಲಿ && ufunc == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ &t_func_unknown;

    ಆದರೆ (name != ಖಾಲಿ)
    {
	ಪೂರ್ಣಾಂಕ idx = ಕಂಡುಹಿಡಿ_ಆಂತರಿಕ_func(name);

	ಆದರೆ (idx >= 0)
	{
	    type_T *decl_type;  //ಬಳಸದ

	    ಆಂತರಿಕ_func_get_argcount(idx, &argcount, &min_argcount);
	    member_type = ಆಂತರಿಕ_func_ret_type(idx, 0, ಖಾಲಿ, &decl_type,
		    type_gap);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Check if name contains "<".  If it does, then replace "<" with
	    // NUL and lookup the function and then look up the specific
	    // generic function using the supplied types.
	    char_u *p = generic_func_ಕಂಡುಹಿಡಿ_open_bracket(name);
	    ಆದರೆ (p == ಖಾಲಿ)
		ufunc = ಕಂಡುಹಿಡಿ_func(name, FALSE);
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// generic function
		char_u	cc = *p;

		*p = NUL;
		ufunc = ಕಂಡುಹಿಡಿ_func(name, FALSE);
		*p = cc;
		ಆದರೆ (ufunc != ಖಾಲಿ && IS_GENERIC_FUNC(ufunc))
		{
		    ufunc = ಕಂಡುಹಿಡಿ_generic_func(ufunc, name, &p);
		    ಆದರೆ (ufunc == ಖಾಲಿ)
			ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		}
	    }
	}
    }
    ಆದರೆ (ufunc != ಖಾಲಿ)
    {
	// May need to get the argument types from default values by
	// compiling the function.
	ಆದರೆ (ufunc->uf_def_ಸ್ಥಿತಿ == UF_ಗಮ್ಯ_BE_COMPILED
		&& compile_def_function(ufunc, TRUE, CT_NONE, ಖಾಲಿ)
		== FAIL)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	ಆದರೆ (ufunc->uf_func_type == ಖಾಲಿ)
	    ಗಣ_function_type(ufunc);
	ಆದರೆ (ufunc->uf_func_type != ಖಾಲಿ)
	{
	    ಆದರೆ (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ partial_typval2type(tv, ufunc, type_gap);
	    ಹಿಂತಿರುಗಿ ufunc->uf_func_type;
	}
    }

    type = get_type_ptr(type_gap);
    ಆದರೆ (type == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    type->tt_type = tv->v_type;
    type->tt_argcount = argcount;
    type->tt_min_argcount = min_argcount;
    ಆದರೆ (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != ಖಾಲಿ
					&& tv->vval.v_partial->pt_argc > 0)
    {
	type->tt_argcount -= tv->vval.v_partial->pt_argc;
	type->tt_min_argcount -= tv->vval.v_partial->pt_argc;
    }
    type->tt_member = member_type;

    ಹಿಂತಿರುಗಿ type;
}

/*
 * Get a type_T for a typval_T.
 * "type_gap" is used to temporarily create types in.
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    ಸ್ಥಿರ type_T *
typval2type_int(typval_T *tv, ಪೂರ್ಣಾಂಕ copyID, garray_T *type_gap, ಪೂರ್ಣಾಂಕ flags)
{
    ಆಯ್ಕೆ (tv->v_type)
    {
	ಸ್ಥಿತಿ VAR_UNKNOWN:
	    ಹಿಂತಿರುಗಿ &t_unknown;

	ಸ್ಥಿತಿ VAR_ANY:
	    ಹಿಂತಿರುಗಿ &t_any;

	ಸ್ಥಿತಿ VAR_VOID:
	    ಹಿಂತಿರುಗಿ &t_void;

	ಸ್ಥಿತಿ VAR_BOOL:
	    ಹಿಂತಿರುಗಿ &t_bool;

	ಸ್ಥಿತಿ VAR_SPECIAL:
	    ಹಿಂತಿರುಗಿ special_typval2type(tv);

	ಸ್ಥಿತಿ VAR_ಸಂಖ್ಯೆ:
	    ಹಿಂತಿರುಗಿ &t_ಸಂಖ್ಯೆ;

	ಸ್ಥಿತಿ VAR_FLOAT:
	    ಹಿಂತಿರುಗಿ &t_float;

	ಸ್ಥಿತಿ VAR_STRING:
	    ಹಿಂತಿರುಗಿ &t_string;

	ಸ್ಥಿತಿ VAR_BLOB:
	    ಆದರೆ (tv->vval.v_blob == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ &t_blob_null;
	    ಹಿಂತಿರುಗಿ &t_blob;

	ಸ್ಥಿತಿ VAR_LIST:
	    ಹಿಂತಿರುಗಿ list_typval2type(tv, copyID, type_gap, flags);

	ಸ್ಥಿತಿ VAR_TUPLE:
	    ಹಿಂತಿರುಗಿ tuple_typval2type(tv, copyID, type_gap, flags);

	ಸ್ಥಿತಿ VAR_DICT:
	    ಹಿಂತಿರುಗಿ dict_typval2type(tv, copyID, type_gap, flags);

	ಸ್ಥಿತಿ VAR_ಕೆಲಸ:
	    ಹಿಂತಿರುಗಿ &t_ಕೆಲಸ;

	ಸ್ಥಿತಿ VAR_CHANNEL:
	    ಹಿಂತಿರುಗಿ &t_channel;

	ಸ್ಥಿತಿ VAR_CLASS:
	ಸ್ಥಿತಿ VAR_OBJECT:
	    ಹಿಂತಿರುಗಿ oc_typval2type(tv);

	ಸ್ಥಿತಿ VAR_TYPEALIAS:
	    ಹಿಂತಿರುಗಿ &t_typealias;

	ಸ್ಥಿತಿ VAR_FUNC:
	ಸ್ಥಿತಿ VAR_PARTIAL:
	    ಹಿಂತಿರುಗಿ fp_typval2type(tv, type_gap);

	ಸ್ಥಿತಿ VAR_INSTR:
	ಪೂರ್ವನಿಯೋಜಿತ:
	    ನಿಲ್ಲಿಸು;
    }

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Return TRUE if "tv" is not a bool but should be converted to bool.
 */
    ಪೂರ್ಣಾಂಕ
need_convert_ಗಮ್ಯ_bool(type_T *type, typval_T *tv)
{
    ಹಿಂತಿರುಗಿ type != ಖಾಲಿ && type == &t_bool && tv->v_type != VAR_BOOL
	    && (tv->v_type == VAR_ಸಂಖ್ಯೆ
		       && (tv->vval.v_ಸಂಖ್ಯೆ == 0 || tv->vval.v_ಸಂಖ್ಯೆ == 1));
}

/*
 * Get a type_T for a typval_T.
 * "type_list" is used to temporarily create types in.
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the most specific member type.
 */
    type_T *
typval2type(typval_T *tv, ಪೂರ್ಣಾಂಕ copyID, garray_T *type_gap, ಪೂರ್ಣಾಂಕ flags)
{
    type_T *type = typval2type_int(tv, copyID, type_gap, flags);

    ಆದರೆ (type == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (type != &t_bool && (tv->v_type == VAR_ಸಂಖ್ಯೆ
		&& (tv->vval.v_ಸಂಖ್ಯೆ == 0 || tv->vval.v_ಸಂಖ್ಯೆ == 1)))
	// Number 0 and 1 and expression with "&&" or "||" can also be used
	// for bool.
	type = &t_ಸಂಖ್ಯೆ_bool;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type != &t_float && tv->v_type == VAR_ಸಂಖ್ಯೆ)
	// A number can also be used for float.
	type = &t_ಸಂಖ್ಯೆ_float;
    ಹಿಂತಿರುಗಿ type;
}

/*
 * Return TRUE if "type" can be used for a variable declaration.
 * Give an error and return FALSE if not.
 */
    ಪೂರ್ಣಾಂಕ
valid_declaration_type(type_T *type)
{
    ಆದರೆ (type->tt_type == VAR_SPECIAL  // null, none
	    || type->tt_type == VAR_VOID)
    {
	ಅಕ್ಷರ *tofree = ಖಾಲಿ;
	ಅಕ್ಷರ *name = type_name(type, &tofree);
	semsg(_(e_invalid_type_ಒಳ_variable_declaration_str), name);
	vim_free(tofree);
	ಹಿಂತಿರುಗಿ FALSE;
    }
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Get a type_T for a typval_T, used for v: variables.
 * "type_list" is used to temporarily create types in.
 */
    type_T *
typval2type_vimvar(typval_T *tv, garray_T *type_gap)
{
    ಆದರೆ (tv->v_type == VAR_LIST)  // e.g. for v:oldfiles
	ಹಿಂತಿರುಗಿ &t_list_string;
    ಆದರೆ (tv->v_type == VAR_DICT)  // e.g. for v:event
	ಹಿಂತಿರುಗಿ &t_dict_any;
    ಹಿಂತಿರುಗಿ typval2type(tv, get_copyID(), type_gap, TVTT_DO_MEMBER);
}

    ಪೂರ್ಣಾಂಕ
check_typval_arg_type(
	type_T	    *expected,
	typval_T    *actual_tv,
	ಅಕ್ಷರ	    *func_name,
	ಪೂರ್ಣಾಂಕ	    arg_idx)
{
    where_T	where = WHERE_INIT;

    ಆದರೆ (arg_idx > 0)
    {
	where.wt_ಸೂಚ್ಯಂಕ = arg_idx;
	where.wt_ಬಗೆ = WT_ವಾದ;
    }
    where.wt_func_name = func_name;
    ಹಿಂತಿರುಗಿ check_typval_type(expected, actual_tv, where);
}

/*
 * Return true if "expected" is list<any> or dict<any> and "actual_tv" is a
 * list or dict of that kind, which matches whatever its values are.
 */
    ಸ್ಥಿರ bool
matches_listdict_any(type_T *expected, typval_T *actual_tv)
{
    ಹಿಂತಿರುಗಿ (expected->tt_type == VAR_LIST || expected->tt_type == VAR_DICT)
	    && actual_tv->v_type == expected->tt_type
	    && expected->tt_member != ಖಾಲಿ
	    && (expected->tt_member->tt_type == VAR_ANY
		|| expected->tt_member->tt_type == VAR_UNKNOWN);
}

/*
 * Return FAIL if "expected" and "actual" don't match.
 * When "argidx" > 0 it is included in the error message.
 */
    ಪೂರ್ಣಾಂಕ
check_typval_type(type_T *expected, typval_T *actual_tv, where_T where)
{
    garray_T	type_list;
    type_T	*actual_type;
    ಪೂರ್ಣಾಂಕ		res = FAIL;

    ಆದರೆ (expected == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ OK;  // didn't expect anything.

    // No need to go through the values of a possibly big list or dict.
    ಆದರೆ (matches_listdict_any(expected, actual_tv))
	ಹಿಂತಿರುಗಿ OK;

    ga_init2(&type_list, ಗಾತ್ರ(type_T *), 10);

    // A null_function and null_partial are special cases, they can be used to
    // clear a variable.
    ಆದರೆ ((actual_tv->v_type == VAR_FUNC && actual_tv->vval.v_string == ಖಾಲಿ)
	    || (actual_tv->v_type == VAR_PARTIAL
					 && actual_tv->vval.v_partial == ಖಾಲಿ))
	actual_type = &t_func_unknown;
    ಇಲ್ಲದಿದ್ದರೆ
	// When the actual type is list<any> or dict<any> go through the values
	// to possibly get a more specific type.
	actual_type = typval2type(actual_tv, get_copyID(), &type_list,
					  TVTT_DO_MEMBER | TVTT_ಇನ್ನಷ್ಟು_SPECIFIC);
    ಆದರೆ (actual_type != ಖಾಲಿ)
    {
	res = check_type_maybe(expected, actual_type, TRUE, where);
	ಆದರೆ (res == MAYBE && !(actual_type->tt_type == VAR_FUNC
				      && (actual_type->tt_member == &t_unknown
					  || actual_type->tt_member == ಖಾಲಿ)))
	{
	    // If a type check is needed that means assigning "any" or
	    // "unknown" to a more specific type, which fails here.
	    // Except when it looks like a lambda, since they have an
	    // incomplete type.  A legacy lambda function has a NULL return
	    // type.
	    type_mismatch_where(expected, actual_type, where);
	    res = FAIL;
	}
    }
    clear_type_list(&type_list);
    ಹಿಂತಿರುಗಿ res;
}

    ಶೂನ್ಯ
arg_type_mismatch(type_T *expected, type_T *actual, ಪೂರ್ಣಾಂಕ arg_idx)
{
    where_T	where = WHERE_INIT;

    ಆದರೆ (arg_idx > 0)
    {
	where.wt_ಸೂಚ್ಯಂಕ = arg_idx;
	where.wt_ಬಗೆ = WT_ವಾದ;
    }
    type_mismatch_where(expected, actual, where);
}

    ಶೂನ್ಯ
type_mismatch_where(type_T *expected, type_T *actual, where_T where)
{
    ಅಕ್ಷರ *tofree1, *tofree2;
    ಅಕ್ಷರ *typename1 = type_name(expected, &tofree1);
    ಅಕ್ಷರ *typename2 = type_name(actual, &tofree2);

    ಆಯ್ಕೆ (where.wt_ಬಗೆ)
    {
	ಸ್ಥಿತಿ WT_MEMBER:
	    semsg(_(e_variable_str_type_mismatch_expected_str_but_got_str),
		    where.wt_func_name, typename1, typename2);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ WT_ವಿಧಾನ:
	ಸ್ಥಿತಿ WT_ವಿಧಾನ_ARG:
	ಸ್ಥಿತಿ WT_ವಿಧಾನ_RETURN:
	    semsg(_(e_ವಿಧಾನ_str_type_mismatch_expected_str_but_got_str),
		    where.wt_func_name, typename1, typename2);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ WT_VARIABLE:
	    ಆದರೆ (where.wt_func_name == ಖಾಲಿ)
		semsg(_(e_variable_nr_type_mismatch_expected_str_but_got_str),
			where.wt_ಸೂಚ್ಯಂಕ, typename1, typename2);
	    ಇಲ್ಲದಿದ್ದರೆ
		semsg(_(e_variable_nr_type_mismatch_expected_str_but_got_str_ಒಳ_str),
			where.wt_ಸೂಚ್ಯಂಕ, typename1, typename2, where.wt_func_name);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ WT_ವಾದ:
	    ಆದರೆ (where.wt_func_name == ಖಾಲಿ)
		semsg(_(e_ವಾದ_nr_type_mismatch_expected_str_but_got_str),
			where.wt_ಸೂಚ್ಯಂಕ, typename1, typename2);
	    ಇಲ್ಲದಿದ್ದರೆ
		semsg(_(e_ವಾದ_nr_type_mismatch_expected_str_but_got_str_ಒಳ_str),
			where.wt_ಸೂಚ್ಯಂಕ, typename1, typename2, where.wt_func_name);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ WT_CAST:
	ಸ್ಥಿತಿ WT_UNKNOWN:
	    ಆದರೆ (where.wt_func_name == ಖಾಲಿ)
		semsg(_(e_type_mismatch_expected_str_but_got_str),
			typename1, typename2);
	    ಇಲ್ಲದಿದ್ದರೆ
		semsg(_(e_type_mismatch_expected_str_but_got_str_ಒಳ_str),
			typename1, typename2, where.wt_func_name);
	    ನಿಲ್ಲಿಸು;
    }

    vim_free(tofree1);
    vim_free(tofree2);
}

/*
 * Check if the expected and actual types match for a tuple
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
check_tuple_type_maybe(
    type_T	*expected,
    type_T	*actual,
    where_T	where)
{
    ಆದರೆ (expected->tt_argcount == -1 || actual->tt_argcount == -1
		|| expected->tt_ವಾದಗಳು == ಖಾಲಿ || actual->tt_ವಾದಗಳು == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ OK;

    // For a non-variadic tuple, the number of items must match
    ಆದರೆ (!(expected->tt_flags & TTFLAG_VARARGS)
	    && expected->tt_argcount != actual->tt_argcount)
	ಹಿಂತಿರುಗಿ FAIL;

    // compare the type of each tuple item
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < actual->tt_argcount; ++i)
    {
	type_T	*exp_type;
	type_T	*actual_type;

	ಆದರೆ (expected->tt_flags & TTFLAG_VARARGS)
	{
	    ಆದರೆ (i < expected->tt_argcount - 1)
		exp_type = expected->tt_ವಾದಗಳು[i];
	    ಇಲ್ಲದಿದ್ದರೆ
		// For a variadic tuple, the last type is a List.  So use the
		// List member type.
		exp_type = expected->tt_ವಾದಗಳು[expected->tt_argcount - 1]->tt_member;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    exp_type = expected->tt_ವಾದಗಳು[i];

	ಆದರೆ (actual->tt_flags & TTFLAG_VARARGS)
	{
	    ಆದರೆ (i < actual->tt_argcount - 1)
		actual_type = actual->tt_ವಾದಗಳು[i];
	    ಇಲ್ಲದಿದ್ದರೆ
		// For a variadic tuple, the last type is a List.  So use the
		// List member type.
		actual_type = actual->tt_ವಾದಗಳು[actual->tt_argcount - 1]->tt_member;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    actual_type = actual->tt_ವಾದಗಳು[i];

	// Allow for using "any" type for a tuple item
	ಆದರೆ (actual->tt_ವಾದಗಳು[i] != &t_any && check_type(exp_type, actual_type,
						FALSE, where) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Check if the expected and actual types match for a function
 * Returns OK if "expected" and "actual" are matching function types.
 * Returns FAIL if "expected" and "actual" are different types.
 * Returns MAYBE when a runtime type check is needed.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
check_func_type_maybe(
    type_T	*expected,
    type_T	*actual,
    where_T	where)
{
    ಪೂರ್ಣಾಂಕ ret = OK;

    // If the return type is unknown it can be anything, including
    // nothing, thus there is no point in checking.
    ಆದರೆ (expected->tt_member != &t_unknown)
    {
	ಆದರೆ (actual->tt_member != ಖಾಲಿ
		&& actual->tt_member != &t_unknown)
	{
	    where_T  func_where = where;

	    func_where.wt_ಬಗೆ = WT_ವಿಧಾನ_RETURN;
	    ret = check_type_maybe(expected->tt_member,
		    actual->tt_member, FALSE,
		    func_where);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ret = MAYBE;
    }
    ಆದರೆ (ret != FAIL
	    && ((expected->tt_flags & TTFLAG_VARARGS)
		!= (actual->tt_flags & TTFLAG_VARARGS))
	    && expected->tt_argcount != -1)
	ret = FAIL;
    ಆದರೆ (ret != FAIL && expected->tt_argcount != -1
	    && actual->tt_min_argcount != -1
	    && (actual->tt_argcount == -1
		|| (actual->tt_argcount < expected->tt_min_argcount
		    || actual->tt_argcount > expected->tt_argcount)))
	ret = FAIL;
    ಆದರೆ (ret != FAIL && expected->tt_ವಾದಗಳು != ಖಾಲಿ
	    && actual->tt_ವಾದಗಳು != ಖಾಲಿ)
    {
	ಪೂರ್ಣಾಂಕ i;

	ಗಾಗಿ (i = 0; i < expected->tt_argcount
		&& i < actual->tt_argcount; ++i)
	{
	    where_T  func_where = where;
	    func_where.wt_ಬಗೆ = WT_ವಿಧಾನ_ARG;

	    // Allow for using "any" argument type, lambda's have them.
	    ಆದರೆ (actual->tt_ವಾದಗಳು[i] != &t_any && check_type(
			expected->tt_ವಾದಗಳು[i], actual->tt_ವಾದಗಳು[i], FALSE,
			func_where) == FAIL)
	    {
		ret = FAIL;
		ನಿಲ್ಲಿಸು;
	    }
	}
    }
    ಆದರೆ (ret == OK && expected->tt_argcount >= 0
	    && actual->tt_argcount == -1)
	// check the argument count at runtime
	ret = MAYBE;

    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Check if the expected and actual types match for an object
 * Returns OK if "expected" and "actual" are matching object types.
 * Returns FAIL if "expected" and "actual" are different types.
 * Returns MAYBE when a runtime type check is needed.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
check_object_type_maybe(
    type_T	*expected,
    type_T	*actual,
    where_T	where)
{
    ಪೂರ್ಣಾಂಕ ret = OK;

    ಆದರೆ (actual->tt_type == VAR_ANY)
	ಹಿಂತಿರುಗಿ MAYBE;	// use runtime type check
    ಆದರೆ (actual->tt_type != VAR_OBJECT)
	ಹಿಂತಿರುಗಿ FAIL;	// don't use tt_class
    ಆದರೆ (actual->tt_class == ಖಾಲಿ)    // null object
	ಹಿಂತಿರುಗಿ OK;
    // t_object_any matches any object except for an enum item
    ಆದರೆ (expected == &t_object_any && !IS_ENUM(actual->tt_class))
	ಹಿಂತಿರುಗಿ OK;

    // For object method arguments, do a invariant type check in
    // an extended class.  For all others, do a covariance type check.
    ಆದರೆ (where.wt_ಬಗೆ == WT_ವಿಧಾನ_ARG)
    {
	ಆದರೆ (actual->tt_class != expected->tt_class)
	    ret = FAIL;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!class_instance_ದ(actual->tt_class, expected->tt_class))
    {
	// Check if this is an up-cast, if so we'll have to check the type at
	// runtime.
	ಆದರೆ (where.wt_ಬಗೆ == WT_CAST &&
		class_instance_ದ(expected->tt_class, actual->tt_class))
	    ret = MAYBE;
	ಇಲ್ಲದಿದ್ದರೆ
	    ret = FAIL;
    }

    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Check if the expected and actual types match.
 * Does not allow for assigning "any" to a specific type.
 * When "argidx" > 0 it is included in the error message.
 * Return OK if types match.
 * Return FAIL if types do not match.
 */
    ಪೂರ್ಣಾಂಕ
check_type(
	type_T	*expected,
	type_T	*actual,
	ಪೂರ್ಣಾಂಕ	give_ಸಂದೇಶ_2,
	where_T where)
{
    ಪೂರ್ಣಾಂಕ ret = check_type_maybe(expected, actual, give_ಸಂದೇಶ_2, where);

    ಹಿಂತಿರುಗಿ ret == MAYBE ? OK : ret;
}

/*
 * As check_type() but return MAYBE when a runtime type check should be used
 * when compiling.
 */
    ಪೂರ್ಣಾಂಕ
check_type_maybe(
	type_T	*expected,
	type_T	*actual,
	ಪೂರ್ಣಾಂಕ	give_ಸಂದೇಶ_2,
	where_T where)
{
    ಪೂರ್ಣಾಂಕ ret = OK;

    // When expected is "unknown" we accept any actual type.
    // When expected is "any" we accept any actual type except "void".
    ಆದರೆ (expected->tt_type != VAR_UNKNOWN
	    && !(expected->tt_type == VAR_ANY && actual->tt_type != VAR_VOID))

    {
	// tt_type should match, except that a "partial" can be assigned to a
	// variable with type "func".
	// And "unknown" (using global variable) and "any" need a runtime type
	// check.
	ಆದರೆ (!(expected->tt_type == actual->tt_type
		    || actual->tt_type == VAR_UNKNOWN
		    || actual->tt_type == VAR_ANY
		    || (expected->tt_type == VAR_FUNC
					   && actual->tt_type == VAR_PARTIAL)))
	{
	    ಆದರೆ (expected->tt_type == VAR_BOOL
					&& (actual->tt_flags & TTFLAG_BOOL_OK))
		// Using number 0 or 1 for bool is OK.
		ಹಿಂತಿರುಗಿ OK;
	    ಆದರೆ (expected->tt_type == VAR_FLOAT
		    && actual->tt_type == VAR_ಸಂಖ್ಯೆ
		    && ((expected->tt_flags & TTFLAG_ಸಂಖ್ಯೆ_OK)
			     || (actual->tt_flags & TTFLAG_FLOAT_OK)))
		// Using a number where a float is expected is OK here.
		ಹಿಂತಿರುಗಿ OK;
	    ಆದರೆ (expected->tt_type == VAR_LIST
		    && actual->tt_type == VAR_TUPLE
		    && (expected->tt_flags & TTFLAG_TUPLE_OK))
		// Using a tuple where a list is expected is OK here.
		ಹಿಂತಿರುಗಿ OK;
	    ಆದರೆ (give_ಸಂದೇಶ_2)
		type_mismatch_where(expected, actual, where);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	ಆದರೆ (expected->tt_type == VAR_DICT || expected->tt_type == VAR_LIST)
	{
	    // "unknown" is used for an empty list or dict
	    ಆದರೆ (actual->tt_member != ಖಾಲಿ && actual->tt_member != &t_unknown)
		ret = check_type_maybe(expected->tt_member, actual->tt_member,
								 FALSE, where);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (expected->tt_type == VAR_TUPLE && actual != &t_any)
	    ret =  check_tuple_type_maybe(expected, actual, where);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (expected->tt_type == VAR_FUNC && actual != &t_any)
	    ret = check_func_type_maybe(expected, actual, where);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (expected->tt_type == VAR_OBJECT)
	    ret = check_object_type_maybe(expected, actual, where);

	ಆದರೆ (ret == FAIL && give_ಸಂದೇಶ_2)
	    type_mismatch_where(expected, actual, where);
    }

    ಆದರೆ (ret == OK && expected->tt_type != VAR_UNKNOWN
	    && expected->tt_type != VAR_ANY
	    && (actual->tt_type == VAR_UNKNOWN || actual->tt_type == VAR_ANY))
	// check the type at runtime
	ret = MAYBE;

    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Check that the arguments of "type" match "argvars[argcount]".
 * "base_tv" is from "expr->Func()".
 * Return OK/FAIL.
 */
    ಪೂರ್ಣಾಂಕ
check_ವಾದ_types(
	type_T	    *type,
	typval_T    *argvars,
	ಪೂರ್ಣಾಂಕ	    argcount,
	typval_T    *ತಳ_tv,
	char_u	    *name)
{
    ಪೂರ್ಣಾಂಕ	    varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;
    ಪೂರ್ಣಾಂಕ	    i;
    ಪೂರ್ಣಾಂಕ	    totcount = argcount + (ತಳ_tv == ಖಾಲಿ ? 0 : 1);

    ಆದರೆ (type->tt_type != VAR_FUNC && type->tt_type != VAR_PARTIAL)
	ಹಿಂತಿರುಗಿ OK;  // just in case
    ಆದರೆ (totcount < type->tt_min_argcount - varargs)
    {
	emsg_funcname(e_not_enough_arguments_for_function_str, name);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಆದರೆ (!varargs && type->tt_argcount >= 0 && totcount > type->tt_argcount)
    {
	emsg_funcname(e_too_many_arguments_for_function_str, name);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಆದರೆ (type->tt_ವಾದಗಳು == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ OK;  // cannot check


    ಗಾಗಿ (i = 0; i < totcount; ++i)
    {
	type_T	    *expected;
	typval_T    *tv;

	ಆದರೆ (ತಳ_tv != ಖಾಲಿ)
	{
	    ಆದರೆ (i == 0)
		tv = ತಳ_tv;
	    ಇಲ್ಲದಿದ್ದರೆ
		tv = &argvars[i - 1];
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    tv = &argvars[i];
	ಆದರೆ (varargs && i >= type->tt_argcount - 1)
	{
	    expected = type->tt_ವಾದಗಳು[type->tt_argcount - 1];
	    ಆದರೆ (expected != ಖಾಲಿ && expected->tt_type == VAR_LIST)
		expected = expected->tt_member;
	    ಆದರೆ (expected == ಖಾಲಿ)
		expected = &t_any;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    expected = type->tt_ವಾದಗಳು[i];

	// check the type, unless the value is v:none
	ಆದರೆ ((tv->v_type != VAR_SPECIAL || tv->vval.v_ಸಂಖ್ಯೆ != VVAL_NONE)
		   && check_typval_arg_type(expected, tv, ಖಾಲಿ, i + 1) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

// Maximum nesting of types, to avoid running out of stack space.  E.g.
// "list<list<number>>" is nested twice.
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_TYPE_ಆಳ	1000

// Current number of levels of parse_type() and the deepest level reached while
// parsing the current type.  The nesting is one less than the number of
// levels.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	parse_ಆಳ = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	parse_max_ಆಳ = 0;

/*
 * Skip over type in list<type>, dict<type> or tuple<type>.
 * Returns a pointer to the character after the type.  "syn_error" is set to
 * TRUE on syntax error.
 */
    ಸ್ಥಿರ char_u *
skip_member_type(char_u *ಆರಂಭ, char_u *p, ಪೂರ್ಣಾಂಕ *syn_error)
{
    ಆದರೆ (STRNCMP("tuple", ಆರಂಭ, 5) == 0)
    {
	// handle tuple<{type1}, {type2}, ....<type>>
	p = skipwhite(p + 1);
	ಆಗ (*p != '>' && *p != NUL)
	{
	    char_u *sp = p;

	    ಆದರೆ (STRNCMP(p, "...", 3) == 0)
		p += 3;
	    p = skip_type(p, TRUE);
	    ಆದರೆ (p == sp)
	    {
		*syn_error = TRUE;
		ಹಿಂತಿರುಗಿ p;  // syntax error
	    }
	    ಆದರೆ (*p == ',')
		p = skipwhite(p + 1);
	}
	ಆದರೆ (*p == '>')
	    p++;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	p = skipwhite(p);
	p = skip_type(skipwhite(p + 1), FALSE);
	p = skipwhite(p);
	ಆದರೆ (*p == '>')
	    ++p;
    }

    ಹಿಂತಿರುಗಿ p;
}

/*
 * Skip over a function type.  Returns a pointer to the character after the
 * type.  "syn_error" is set to TRUE on syntax error.
 */
    ಸ್ಥಿರ char_u *
skip_func_type(char_u *p, ಪೂರ್ಣಾಂಕ *syn_error)
{
    ಆದರೆ (*p == '(')
    {
	// handle func(args): type
	++p;
	ಆಗ (*p != ')' && *p != NUL)
	{
	    char_u *sp = p;

	    ಆದರೆ (STRNCMP(p, "...", 3) == 0)
		p += 3;
	    p = skip_type(p, TRUE);
	    ಆದರೆ (p == sp)
	    {
		*syn_error = TRUE;
		ಹಿಂತಿರುಗಿ p;  // syntax error
	    }
	    ಆದರೆ (*p == ',')
		p = skipwhite(p + 1);
	}
	ಆದರೆ (*p == ')')
	{
	    ಆದರೆ (p[1] == ':')
		p = skip_type(skipwhite(p + 2), FALSE);
	    ಇಲ್ಲದಿದ್ದರೆ
		++p;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// handle func: return_type
	p = skip_type(skipwhite(p + 1), FALSE);
    }

    ಹಿಂತಿರುಗಿ p;
}

/*
 * Inner part of skip_type().
 */
    ಸ್ಥಿರ char_u *
skip_type_inner(char_u *ಆರಂಭ, ಪೂರ್ಣಾಂಕ optional)
{
    char_u	*p = ಆರಂಭ;
    ಪೂರ್ಣಾಂಕ		syn_error = FALSE;

    ಆದರೆ (optional && *p == '?')
	++p;

    // Also skip over "." for imported classes: "import.ClassName".
    ಆಗ (ASCII_ISALNUM(*p) || *p == '_' || *p == '.')
	++p;

    // Skip over "<type>"; this is permissive about white space.
    ಆದರೆ (*skipwhite(p) == '<')
    {
	p = skip_member_type(ಆರಂಭ, p, &syn_error);
	ಆದರೆ (syn_error)
	    ಹಿಂತಿರುಗಿ p;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((*p == '(' || (*p == ':' && VIM_ISWHITE(p[1])))
					     && STRNCMP("func", ಆರಂಭ, 4) == 0)
    {
	// skip over function type
	p = skip_func_type(p, &syn_error);
	ಆದರೆ (syn_error)
	    ಹಿಂತಿರುಗಿ p;
    }

    ಹಿಂತಿರುಗಿ p;
}

/*
 * Skip over a type definition and return a pointer to just after it.
 * When "optional" is TRUE then a leading "?" is accepted.
 */
    char_u *
skip_type(char_u *ಆರಂಭ, ಪೂರ್ಣಾಂಕ optional)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	skip_ಆಳ = 0;
    char_u	*p = ಆರಂಭ;

    // Too deeply nested: skip to the matching ">" without recursing, an
    // error is given when parsing the type.
    ಆದರೆ (skip_ಆಳ > MAX_TYPE_ಆಳ)
    {
	ಪೂರ್ಣಾಂಕ	ಆಳ = 0;

	ಗಾಗಿ ( ; *p != NUL; ++p)
	{
	    ಆದರೆ (*p == '<')
		++ಆಳ;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == '>')
	    {
		ಆದರೆ (ಆಳ == 0)
		    ನಿಲ್ಲಿಸು;
		ಆದರೆ (--ಆಳ == 0)
		    ಹಿಂತಿರುಗಿ p + 1;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಆಳ == 0 && !ASCII_ISALNUM(*p) && *p != '_'
								&& *p != '.')
		ನಿಲ್ಲಿಸು;
	}
	ಹಿಂತಿರುಗಿ p;
    }

    ++skip_ಆಳ;
    p = skip_type_inner(ಆರಂಭ, optional);
    --skip_ಆಳ;
    ಹಿಂತಿರುಗಿ p;
}

/*
 * Parse the member type: "<type>" and return "type" with the member set.
 * Use "type_gap" if a new type needs to be added.
 * "info" is extra information for an error message.
 * Returns NULL in case of failure.
 */
    ಸ್ಥಿರ type_T *
parse_type_member(
	char_u	    **arg,
	type_T	    *type,
	garray_T    *type_gap,
	ಪೂರ್ಣಾಂಕ	    give_error,
	ಅಕ್ಷರ	    *info,
	ufunc_T	    *ufunc,
	cctx_T	    *cctx)
{
    char_u  *arg_ಆರಂಭ = *arg;
    type_T  *member_type;
    ಪೂರ್ಣಾಂಕ	    prev_called_emsg = called_emsg;

    ಆದರೆ (**arg != '<')
    {
	ಆದರೆ (give_error)
	{
	    ಆದರೆ (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    ಇಲ್ಲದಿದ್ದರೆ
		semsg(_(e_missing_type_after_str), info);
	}
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    *arg = skipwhite(*arg + 1);

    member_type = parse_type(arg, type_gap, ufunc, cctx, give_error);
    ಆದರೆ (member_type == ಖಾಲಿ || !valid_declaration_type(member_type))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    *arg = skipwhite(*arg);
    ಆದರೆ (**arg != '>' && called_emsg == prev_called_emsg)
    {
	ಆದರೆ (give_error)
	    semsg(_(e_missing_gt_after_type_str), arg_ಆರಂಭ);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ++*arg;

    ಆದರೆ (type->tt_type == VAR_LIST)
	ಹಿಂತಿರುಗಿ get_list_type(member_type, type_gap);
    ಹಿಂತಿರುಗಿ get_dict_type(member_type, type_gap);
}

/*
 * Parse a "func" type at "*arg" and advance over it.
 * When "give_error" is TRUE give error messages, otherwise be quiet.
 * Return NULL for failure.
 */
    ಸ್ಥಿರ type_T *
parse_type_func(
    char_u	**arg,
    size_t	len,
    garray_T	*type_gap,
    ಪೂರ್ಣಾಂಕ		give_error,
    ufunc_T	*ufunc,
    cctx_T	*cctx)
{
    char_u  *p;
    type_T  *type;
    type_T  *ret_type = &t_unknown;
    ಪೂರ್ಣಾಂಕ	argcount = -1;
    ಪೂರ್ಣಾಂಕ	flags = 0;
    ಪೂರ್ಣಾಂಕ	ಮೊದಲ_optional = -1;
    type_T	*arg_type[MAX_FUNC_ವಾದಗಳು + 1];

    // func({type}, ...{type}): {type}
    *arg += len;
    ಆದರೆ (**arg == '(')
    {
	// "func" may or may not return a value, "func()" does
	// not return a value.
	ret_type = &t_void;

	p = ++*arg;
	argcount = 0;
	ಆಗ (*p != NUL && *p != ')')
	{
	    ಆದರೆ (*p == '?')
	    {
		ಆದರೆ (ಮೊದಲ_optional == -1)
		    ಮೊದಲ_optional = argcount;
		++p;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "...", 3) == 0)
	    {
		flags |= TTFLAG_VARARGS;
		p += 3;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಮೊದಲ_optional != -1)
	    {
		ಆದರೆ (give_error)
		    emsg(_(e_mandatory_ವಾದ_after_optional_ವಾದ));
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    }

	    type = parse_type(&p, type_gap, ufunc, cctx, give_error);
	    ಆದರೆ (type == ಖಾಲಿ || !valid_declaration_type(type))
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    ಆದರೆ ((flags & TTFLAG_VARARGS) != 0 && type->tt_type != VAR_LIST)
	    {
		ಅಕ್ಷರ *tofree;
		semsg(_(e_variable_arguments_type_must_be_list_str),
			type_name(type, &tofree));
		vim_free(tofree);
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    }
	    arg_type[argcount++] = type;

	    // Nothing comes after "...{type}".
	    ಆದರೆ (flags & TTFLAG_VARARGS)
		ನಿಲ್ಲಿಸು;

	    ಆದರೆ (*p != ',' && *skipwhite(p) == ',')
	    {
		ಆದರೆ (give_error)
		    semsg(_(e_no_white_space_allowed_before_str_str),
			    ",", p);
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    }
	    ಆದರೆ (*p == ',')
	    {
		++p;
		ಆದರೆ (!VIM_ISWHITE(*p))
		{
		    ಆದರೆ (give_error)
			semsg(_(e_white_space_required_after_str_str),
				",", p - 1);
		    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		}
	    }
	    p = skipwhite(p);
	    ಆದರೆ (argcount == MAX_FUNC_ವಾದಗಳು)
	    {
		ಆದರೆ (give_error)
		    emsg(_(e_too_many_ವಾದ_types));
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    }
	}

	p = skipwhite(p);
	ಆದರೆ (*p != ')')
	{
	    ಆದರೆ (give_error)
		emsg(_(e_missing_closing_paren));
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
	*arg = p + 1;
    }
    ಆದರೆ (**arg == ':')
    {
	// parse return type
	++*arg;
	ಆದರೆ (!VIM_ISWHITE(**arg) && give_error)
	    semsg(_(e_white_space_required_after_str_str), ":", *arg - 1);
	*arg = skipwhite(*arg);
	ret_type = parse_type(arg, type_gap, ufunc, cctx, give_error);
	ಆದರೆ (ret_type == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಆದರೆ (flags == 0 && ಮೊದಲ_optional == -1 && argcount <= 0)
	type = get_func_type(ret_type, argcount, type_gap);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	type = alloc_func_type(ret_type, argcount, type_gap);
	type->tt_flags = flags;
	ಆದರೆ (argcount > 0)
	{
	    type->tt_argcount = argcount;
	    type->tt_min_argcount = ಮೊದಲ_optional == -1
						? argcount : ಮೊದಲ_optional;
	    ಆದರೆ (func_type_ಕೂಡಿಸು_arg_types(type, argcount, type_gap) == FAIL)
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    mch_memmove(type->tt_ವಾದಗಳು, arg_type, ಗಾತ್ರ(type_T *) * argcount);
	}
    }

    ಹಿಂತಿರುಗಿ type;
}

/*
 * Parse a "tuple" type at "*arg" and advance over it.
 * When "give_error" is TRUE give error messages, otherwise be quiet.
 * Return NULL for failure.
 */
    ಸ್ಥಿರ type_T *
parse_type_tuple(
    char_u	**arg,
    garray_T	*type_gap,
    ಪೂರ್ಣಾಂಕ		give_error,
    ufunc_T	*ufunc,
    cctx_T	*cctx)
{
    char_u	*p;
    type_T	*type;
    type_T	*ret_type = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		typecount = -1;
    ಪೂರ್ಣಾಂಕ		flags = 0;
    garray_T	tuple_types_ga;

    ga_init2(&tuple_types_ga, ಗಾತ್ರ(type_T *), 10);

    // tuple<{type}, {type}>
    // tuple<{type}, ...{type}>
    ಆದರೆ (**arg != '<')
    {
	ಆದರೆ (give_error)
	{
	    ಆದರೆ (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    ಇಲ್ಲದಿದ್ದರೆ
		semsg(_(e_missing_type_after_str), "tuple");
	}

	// only "tuple" is specified
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    p = ++*arg;
    typecount = 0;
    ಆಗ (*p != NUL && *p != '>')
    {
	ಆದರೆ (STRNCMP(p, "...", 3) == 0)
	{
	    flags |= TTFLAG_VARARGS;
	    p += 3;
	}

	type = parse_type(&p, type_gap, ufunc, cctx, give_error);
	ಆದರೆ (type == ಖಾಲಿ || !valid_declaration_type(type))
	    ಹೋಗು on_err;

	ಆದರೆ ((flags & TTFLAG_VARARGS) != 0 && type->tt_type != VAR_LIST)
	{
	    ಅಕ್ಷರ *tofree;
	    semsg(_(e_variadic_tuple_must_ಕೊನೆ_with_list_type_str),
						type_name(type, &tofree));
	    vim_free(tofree);
	    ಹೋಗು on_err;
	}

	// Add the item type
	ಆದರೆ (ga_grow(&tuple_types_ga, 1) == FAIL)
	    ಹೋಗು on_err;
	((type_T **)tuple_types_ga.ga_ದತ್ತಾಂಶ)[tuple_types_ga.ga_len] = type;
	tuple_types_ga.ga_len++;
	typecount++;

	// Nothing comes after "...{type}".
	ಆದರೆ (flags & TTFLAG_VARARGS)
	    ನಿಲ್ಲಿಸು;

	ಆದರೆ (*p != ',' && *skipwhite(p) == ',')
	{
	    ಆದರೆ (give_error)
		semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
	    ಹೋಗು on_err;
	}
	ಆದರೆ (*p == ',')
	{
	    ++p;
	    ಆದರೆ (!VIM_ISWHITE(*p))
	    {
		ಆದರೆ (give_error)
		    semsg(_(e_white_space_required_after_str_str),
			    ",", p - 1);
		ಹೋಗು on_err;
	    }
	}
	p = skipwhite(p);
    }

    p = skipwhite(p);
    ಆದರೆ (*p != '>' || typecount <= 0)
    {
	ಆದರೆ (give_error)
	    semsg(_(e_missing_type_after_str), p);
	ಹೋಗು on_err;
    }
    *arg = p + 1;

    ret_type = alloc_tuple_type(typecount, type_gap);
    ಆದರೆ (ret_type == &t_any
	    || tuple_type_ಕೂಡಿಸು_types(ret_type, typecount, type_gap) == FAIL)
    {
	ret_type = ಖಾಲಿ;
	ಹೋಗು on_err;
    }
    ret_type->tt_flags = flags;
    ret_type->tt_argcount = typecount;
    mch_memmove(ret_type->tt_ವಾದಗಳು, tuple_types_ga.ga_ದತ್ತಾಂಶ,
						ಗಾತ್ರ(type_T *) * typecount);

on_err:
    ga_clear(&tuple_types_ga);

    ಹಿಂತಿರುಗಿ ret_type;
}

/*
 * Parse a "object" type at "*arg" and advance over it.
 * When "give_error" is TRUE give error messages, otherwise be quiet.
 * Return NULL for failure.
 */
    ಸ್ಥಿರ type_T *
parse_type_object(
    char_u	**arg,
    garray_T	*type_gap,
    ಪೂರ್ಣಾಂಕ		give_error,
    cctx_T	*cctx)
{
    char_u	*arg_ಆರಂಭ = *arg;
    type_T	*object_type;
    ಪೂರ್ಣಾಂಕ		prev_called_emsg = called_emsg;

    // object<X> or object<any>
    ಆದರೆ (**arg != '<')
    {
	ಆದರೆ (give_error)
	{
	    ಆದರೆ (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    ಇಲ್ಲದಿದ್ದರೆ
		semsg(_(e_missing_type_after_str), "object");
	}

	// only "object" is specified
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    // skip spaces following "object<"
    *arg = skipwhite(*arg + 1);

    object_type = parse_type(arg, type_gap, ಖಾಲಿ, cctx, give_error);
    ಆದರೆ (object_type == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    *arg = skipwhite(*arg);
    ಆದರೆ (**arg != '>' && called_emsg == prev_called_emsg)
    {
	ಆದರೆ (give_error)
	    semsg(_(e_missing_gt_after_type_str), arg_ಆರಂಭ);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ++*arg;

    ಆದರೆ (object_type->tt_type == VAR_ANY)
	ಹಿಂತಿರುಗಿ &t_object_any;

    ಆದರೆ (object_type->tt_type != VAR_OBJECT)
    {
	// specified type is not a class
	ಆದರೆ (give_error)
	    semsg(_(e_class_name_not_ಸಿಕ್ಕಿತು_str), arg_ಆರಂಭ);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಹಿಂತಿರುಗಿ object_type;
}

/*
 * Parse a user defined type at "*arg" and advance over it.
 * It can be a class or an interface or a typealias name, possibly imported.
 * Return NULL if a type is not found.
 */
    ಸ್ಥಿರ type_T *
parse_type_user_defined(
    char_u	**arg,
    size_t	len,
    garray_T	*type_gap,
    ಪೂರ್ಣಾಂಕ		give_error,
    ufunc_T	*ufunc,
    cctx_T	*cctx)
{
    ಪೂರ್ಣಾಂಕ		did_emsg_before = did_emsg;
    typval_T	tv;

    tv.v_type = VAR_UNKNOWN;
    ಆದರೆ (eval_variable_import(*arg, &tv) == OK)
    {
	ಆದರೆ (tv.v_type == VAR_CLASS && tv.vval.v_class != ಖಾಲಿ)
	{
	    type_T *type = get_type_ptr(type_gap);
	    ಆದರೆ (type != ಖಾಲಿ)
	    {
		// Although the name is that of a class or interface, the type
		// uses will be an object.
		type->tt_type = VAR_OBJECT;
		type->tt_class = tv.vval.v_class;
		clear_tv(&tv);

		*arg += len;
		// Skip over ".ClassName".
		ಆಗ (ASCII_ISALNUM(**arg) || **arg == '_' || **arg == '.')
		    ++*arg;

		ಹಿಂತಿರುಗಿ type;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tv.v_type == VAR_TYPEALIAS)
	{
	    // user defined type
	    // The type alias may itself be a nested type, check the resulting
	    // nesting depth.
	    ಪೂರ್ಣಾಂಕ	ಆಳ = parse_ಆಳ - 1
				+ MAX(tv.vval.v_typealias->ta_ಆಳ, 1);
	    ಆದರೆ (ಆಳ - 1 > MAX_TYPE_ಆಳ)
	    {
		clear_tv(&tv);
		ಆದರೆ (give_error)
		    emsg(_(e_type_nested_too_deep));
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    }
	    ಆದರೆ (ಆಳ > parse_max_ಆಳ)
		parse_max_ಆಳ = ಆಳ;

	    type_T *type = copy_type(tv.vval.v_typealias->ta_type, type_gap);
	    *arg += len;
	    clear_tv(&tv);
	    // Skip over ".TypeName".
	    ಆಗ (ASCII_ISALNUM(**arg) || **arg == '_' || **arg == '.')
		++*arg;
	    ಹಿಂತಿರುಗಿ type;
	}

	clear_tv(&tv);
    }

    // Check whether it is a generic type
    type_T *type = ಕಂಡುಹಿಡಿ_generic_type(*arg, len, ufunc, cctx);
    ಆದರೆ (type != ಖಾಲಿ)
    {
	*arg += len;
	ಹಿಂತಿರುಗಿ type;
    }

    ಆದರೆ (give_error && (did_emsg == did_emsg_before))
    {
	char_u	*p = skip_type(*arg, FALSE);
	ಅಕ್ಷರ	cc = *p;

	*p = NUL;
	semsg(_(e_type_not_recognized_str), *arg);
	*p = cc;
    }

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Inner part of parse_type().
 */
    ಸ್ಥಿರ type_T *
parse_type_inner(
    char_u	**arg,
    garray_T	*type_gap,
    ufunc_T	*ufunc,
    cctx_T	*cctx,
    ಪೂರ್ಣಾಂಕ		give_error)
{
    char_u  *p = *arg;
    size_t  len;

    // Skip over the first word.
    ಆಗ (ASCII_ISALNUM(*p) || *p == '_')
	++p;
    len = p - *arg;

    ಆಯ್ಕೆ (**arg)
    {
	ಸ್ಥಿತಿ 'a':
	    ಆದರೆ (len == 3 && STRNCMP(*arg, "any", len) == 0)
	    {
		*arg += len;
		ಹಿಂತಿರುಗಿ &t_any;
	    }
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 'b':
	    ಆದರೆ (len == 4 && STRNCMP(*arg, "bool", len) == 0)
	    {
		*arg += len;
		ಹಿಂತಿರುಗಿ &t_bool;
	    }
	    ಆದರೆ (len == 4 && STRNCMP(*arg, "blob", len) == 0)
	    {
		*arg += len;
		ಹಿಂತಿರುಗಿ &t_blob;
	    }
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 'c':
	    ಆದರೆ (len == 7 && STRNCMP(*arg, "channel", len) == 0)
	    {
		*arg += len;
		ಹಿಂತಿರುಗಿ &t_channel;
	    }
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 'd':
	    ಆದರೆ (len == 4 && STRNCMP(*arg, "dict", len) == 0)
	    {
		*arg += len;
		ಹಿಂತಿರುಗಿ parse_type_member(arg, &t_dict_any, type_gap,
						give_error, "dict", ufunc,
						cctx);
	    }
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 'f':
	    ಆದರೆ (len == 5 && STRNCMP(*arg, "float", len) == 0)
	    {
		*arg += len;
		ಹಿಂತಿರುಗಿ &t_float;
	    }
	    ಆದರೆ (len == 4 && STRNCMP(*arg, "func", len) == 0)
		ಹಿಂತಿರುಗಿ parse_type_func(arg, len, type_gap, give_error, ufunc,
									cctx);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 'j':
	    ಆದರೆ (len == 3 && STRNCMP(*arg, "job", len) == 0)
	    {
		*arg += len;
		ಹಿಂತಿರುಗಿ &t_ಕೆಲಸ;
	    }
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 'l':
	    ಆದರೆ (len == 4 && STRNCMP(*arg, "list", len) == 0)
	    {
		*arg += len;
		ಹಿಂತಿರುಗಿ parse_type_member(arg, &t_list_any, type_gap,
						give_error, "list", ufunc,
						cctx);
	    }
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 'n':
	    ಆದರೆ (len == 6 && STRNCMP(*arg, "number", len) == 0)
	    {
		*arg += len;
		ಹಿಂತಿರುಗಿ &t_ಸಂಖ್ಯೆ;
	    }
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 'o':
	    ಆದರೆ (len == 6 && STRNCMP(*arg, "object", len) == 0)
	    {
		*arg += len;
		ಹಿಂತಿರುಗಿ parse_type_object(arg, type_gap, give_error, cctx);
	    }
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 's':
	    ಆದರೆ (len == 6 && STRNCMP(*arg, "string", len) == 0)
	    {
		*arg += len;
		ಹಿಂತಿರುಗಿ &t_string;
	    }
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 't':
	    ಆದರೆ (len == 5 && STRNCMP(*arg, "tuple", len) == 0)
	    {
		*arg += len;
		ಹಿಂತಿರುಗಿ parse_type_tuple(arg, type_gap, give_error, ufunc,
									cctx);
	    }
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 'v':
	    ಆದರೆ (len == 4 && STRNCMP(*arg, "void", len) == 0)
	    {
		*arg += len;
		ಹಿಂತಿರುಗಿ &t_void;
	    }
	    ನಿಲ್ಲಿಸು;
    }

    // User defined type
    ಹಿಂತಿರುಗಿ parse_type_user_defined(arg, len, type_gap, give_error, ufunc,
									cctx);
}

/*
 * Parse a type at "arg" and advance over it.
 * When "give_error" is TRUE give error messages, otherwise be quiet.
 * Return NULL for failure.
 */
    type_T *
parse_type(
    char_u	**arg,
    garray_T	*type_gap,
    ufunc_T	*ufunc,
    cctx_T	*cctx,
    ಪೂರ್ಣಾಂಕ		give_error)
{
    type_T	*type;

    ಆದರೆ (parse_ಆಳ > MAX_TYPE_ಆಳ)
    {
	ಆದರೆ (give_error)
	    emsg(_(e_type_nested_too_deep));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಆದರೆ (parse_ಆಳ == 0)
	parse_max_ಆಳ = 0;
    ++parse_ಆಳ;
    ಆದರೆ (parse_ಆಳ > parse_max_ಆಳ)
	parse_max_ಆಳ = parse_ಆಳ;
    type = parse_type_inner(arg, type_gap, ufunc, cctx, give_error);
    --parse_ಆಳ;
    ಹಿಂತಿರುಗಿ type;
}

/*
 * Return the number of levels of the type last parsed by parse_type().
 */
    ಪೂರ್ಣಾಂಕ
parse_type_ಆಳ(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ parse_max_ಆಳ;
}

/*
 * Check if "type1" and "type2" are exactly the same.
 * "flags" can have ETYPE_ARG_UNKNOWN, which means that an unknown argument
 * type in "type1" is accepted.
 */
    ಪೂರ್ಣಾಂಕ
equal_type(type_T *type1, type_T *type2, ಪೂರ್ಣಾಂಕ flags)
{
    ಪೂರ್ಣಾಂಕ i;

    ಆದರೆ (type1 == ಖಾಲಿ || type2 == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;
    ಆದರೆ (type1->tt_type != type2->tt_type)
	ಹಿಂತಿರುಗಿ FALSE;
    ಆಯ್ಕೆ (type1->tt_type)
    {
	ಸ್ಥಿತಿ VAR_UNKNOWN:
	ಸ್ಥಿತಿ VAR_ANY:
	ಸ್ಥಿತಿ VAR_VOID:
	ಸ್ಥಿತಿ VAR_SPECIAL:
	ಸ್ಥಿತಿ VAR_BOOL:
	ಸ್ಥಿತಿ VAR_ಸಂಖ್ಯೆ:
	ಸ್ಥಿತಿ VAR_FLOAT:
	ಸ್ಥಿತಿ VAR_STRING:
	ಸ್ಥಿತಿ VAR_BLOB:
	ಸ್ಥಿತಿ VAR_ಕೆಲಸ:
	ಸ್ಥಿತಿ VAR_CHANNEL:
	ಸ್ಥಿತಿ VAR_INSTR:
	ಸ್ಥಿತಿ VAR_TYPEALIAS:
	    ನಿಲ್ಲಿಸು;  // not composite is always OK
	ಸ್ಥಿತಿ VAR_OBJECT:
	ಸ್ಥಿತಿ VAR_CLASS:
	    // Objects are considered equal if they are from the same class
	    ಹಿಂತಿರುಗಿ type1->tt_class == type2->tt_class;
	ಸ್ಥಿತಿ VAR_LIST:
	ಸ್ಥಿತಿ VAR_DICT:
	    ಹಿಂತಿರುಗಿ equal_type(type1->tt_member, type2->tt_member, flags);
	ಸ್ಥಿತಿ VAR_TUPLE:
	    ಆದರೆ (type1->tt_argcount != type2->tt_argcount)
		ಹಿಂತಿರುಗಿ FALSE;
	    ಆದರೆ (type1->tt_argcount < 0
			   || type1->tt_ವಾದಗಳು == ಖಾಲಿ || type2->tt_ವಾದಗಳು == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ TRUE;
	    ಗಾಗಿ (i = 0; i < type1->tt_argcount; ++i)
		ಆದರೆ ((flags & ETYPE_ARG_UNKNOWN) == 0
			&& !equal_type(type1->tt_ವಾದಗಳು[i], type2->tt_ವಾದಗಳು[i],
									flags))
		    ಹಿಂತಿರುಗಿ FALSE;
	    ಹಿಂತಿರುಗಿ TRUE;
	ಸ್ಥಿತಿ VAR_FUNC:
	ಸ್ಥಿತಿ VAR_PARTIAL:
	    ಆದರೆ (!equal_type(type1->tt_member, type2->tt_member, flags)
		    || type1->tt_argcount != type2->tt_argcount)
		ಹಿಂತಿರುಗಿ FALSE;
	    ಆದರೆ (type1->tt_argcount < 0
			   || type1->tt_ವಾದಗಳು == ಖಾಲಿ || type2->tt_ವಾದಗಳು == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ TRUE;
	    ಗಾಗಿ (i = 0; i < type1->tt_argcount; ++i)
		ಆದರೆ ((flags & ETYPE_ARG_UNKNOWN) == 0
			&& !equal_type(type1->tt_ವಾದಗಳು[i], type2->tt_ವಾದಗಳು[i],
									flags))
		    ಹಿಂತಿರುಗಿ FALSE;
	    ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Find the common type of "type1" (VAR_FUNC) and "type2" (VAR_FUNC) and put it
 * in "dest".  "type2" and "dest" may be the same.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
common_type_var_func(
    type_T	*type1,
    type_T	*type2,
    type_T	**dest,
    garray_T	*type_gap)
{
    type_T *common;

    // When one of the types is t_func_unknown return the other one.
    // Useful if a list or dict item is null_func.
    ಆದರೆ (type1 == &t_func_unknown)
    {
	*dest = type2;
	ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (type2 == &t_func_unknown)
    {
	*dest = type1;
	ಹಿಂತಿರುಗಿ;
    }

    common_type(type1->tt_member, type2->tt_member, &common, type_gap);
    ಆದರೆ (type1->tt_argcount == type2->tt_argcount
	    && type1->tt_argcount >= 0)
    {
	ಪೂರ್ಣಾಂಕ argcount = type1->tt_argcount;
	ಪೂರ್ಣಾಂಕ i;

	*dest = alloc_func_type(common, argcount, type_gap);
	ಆದರೆ (type1->tt_ವಾದಗಳು != ಖಾಲಿ && type2->tt_ವಾದಗಳು != ಖಾಲಿ)
	{
	    ಆದರೆ (func_type_ಕೂಡಿಸು_arg_types(*dest, argcount,
			type_gap) == OK)
		ಗಾಗಿ (i = 0; i < argcount; ++i)
		    common_type(type1->tt_ವಾದಗಳು[i], type2->tt_ವಾದಗಳು[i],
			    &(*dest)->tt_ವಾದಗಳು[i], type_gap);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	// Use -1 for "tt_argcount" to indicate an unknown number of
	// arguments.
	*dest = alloc_func_type(common, -1, type_gap);

    // Use the minimum of min_argcount.
    (*dest)->tt_min_argcount =
	type1->tt_min_argcount < type2->tt_min_argcount
	? type1->tt_min_argcount : type2->tt_min_argcount;
}

/*
 * Find the common type of "type1" and "type2" and put it in "dest".
 * "type2" and "dest" may be the same.
 */
    ಶೂನ್ಯ
common_type(type_T *type1, type_T *type2, type_T **dest, garray_T *type_gap)
{
    ಆದರೆ (equal_type(type1, type2, 0))
    {
	*dest = type1;
	ಹಿಂತಿರುಗಿ;
    }

    // If either is VAR_UNKNOWN use the other type.  An empty list/dict has no
    // specific type.
    ಆದರೆ (type1 == ಖಾಲಿ || type1->tt_type == VAR_UNKNOWN)
    {
	*dest = type2;
	ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (type2 == ಖಾಲಿ || type2->tt_type == VAR_UNKNOWN)
    {
	*dest = type1;
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (type1->tt_type == type2->tt_type)
    {
	ಆದರೆ (type1->tt_type == VAR_LIST
		|| type1->tt_type == VAR_DICT)
	{
	    type_T *common;

	    common_type(type1->tt_member, type2->tt_member, &common, type_gap);
	    ಆದರೆ (type1->tt_type == VAR_LIST)
		*dest = get_list_type(common, type_gap);
	    ಇಲ್ಲದಿದ್ದರೆ
		*dest = get_dict_type(common, type_gap);
	    ಹಿಂತಿರುಗಿ;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type1->tt_type == VAR_FUNC)
	{
	    common_type_var_func(type1, type2, dest, type_gap);
	    ಹಿಂತಿರುಗಿ;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type1->tt_type == VAR_OBJECT)
	{
	    *dest = &t_object_any;
	    ಹಿಂತಿರುಗಿ;
	}
    }

    *dest = &t_any;
}

/*
 * Return the item type of a List, Dict or a Tuple
 */
    type_T *
get_ಅಂಶ_type(type_T *type)
{
    ಆದರೆ (type->tt_type == VAR_TUPLE)
    {
	ಆದರೆ (type->tt_argcount != 1)
	    ಹಿಂತಿರುಗಿ &t_any;

	ಆದರೆ (type->tt_flags & TTFLAG_VARARGS)
	    ಹಿಂತಿರುಗಿ type->tt_ವಾದಗಳು[0]->tt_member;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ type->tt_ವಾದಗಳು[0];
    }

    // For "any" the item type is not known.
    ಹಿಂತಿರುಗಿ type->tt_member == ಖಾಲಿ ? &t_any : type->tt_member;
}

/*
 * Push an entry onto the type stack.  "type" used both for the current type
 * and the declared type.
 * Returns FAIL when out of memory.
 */
    ಪೂರ್ಣಾಂಕ
ತಳ್ಳು_type_ರಾಶಿ(cctx_T *cctx, type_T *type)
{
    ಹಿಂತಿರುಗಿ ತಳ್ಳು_type_stack2(cctx, type, type);
}

/*
 * Push an entry onto the type stack.  "type" is the current type, "decl_type"
 * is the declared type.
 * Returns FAIL when out of memory.
 */
    ಪೂರ್ಣಾಂಕ
ತಳ್ಳು_type_stack2(cctx_T *cctx, type_T *type, type_T *decl_type)
{
    garray_T	*ರಾಶಿ = &cctx->ctx_type_ರಾಶಿ;
    type2_T	*typep;

    ಆದರೆ (GA_GROW_FAILS(ರಾಶಿ, 1))
	ಹಿಂತಿರುಗಿ FAIL;
    typep = ((type2_T *)ರಾಶಿ->ga_ದತ್ತಾಂಶ) + ರಾಶಿ->ga_len;
    typep->type_curr = type;
    typep->type_decl = decl_type;
    ++ರಾಶಿ->ga_len;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Set the type of the top of the stack to "type".
 */
    ಶೂನ್ಯ
ಗಣ_type_on_ರಾಶಿ(cctx_T *cctx, type_T *type, ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ)
{
    garray_T	*ರಾಶಿ = &cctx->ctx_type_ರಾಶಿ;
    type2_T	*typep = ((type2_T *)ರಾಶಿ->ga_ದತ್ತಾಂಶ)
						  + ರಾಶಿ->ga_len - 1 - ಸ್ಥಾನಾಂತರ;

    typep->type_curr = type;
    typep->type_decl = &t_any;
}

/*
 * Get the current type from the type stack.  If "offset" is zero the one at
 * the top,
 * if "offset" is one the type above that, etc.
 * Returns &t_unknown if there is no such stack entry.
 */
    type_T *
get_type_on_ರಾಶಿ(cctx_T *cctx, ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ)
{
    garray_T	*ರಾಶಿ = &cctx->ctx_type_ರಾಶಿ;

    ಆದರೆ (ಸ್ಥಾನಾಂತರ + 1 > ರಾಶಿ->ga_len)
	ಹಿಂತಿರುಗಿ &t_unknown;
    ಹಿಂತಿರುಗಿ (((type2_T *)ರಾಶಿ->ga_ದತ್ತಾಂಶ) + ರಾಶಿ->ga_len - ಸ್ಥಾನಾಂತರ - 1)
								   ->type_curr;
}

/*
 * Get the declared type from the type stack.  If "offset" is zero the one at
 * the top,
 * if "offset" is one the type above that, etc.
 * Returns &t_unknown if there is no such stack entry.
 */
    type_T *
get_decl_type_on_ರಾಶಿ(cctx_T *cctx, ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ)
{
    garray_T	*ರಾಶಿ = &cctx->ctx_type_ರಾಶಿ;

    ಆದರೆ (ಸ್ಥಾನಾಂತರ + 1 > ರಾಶಿ->ga_len)
	ಹಿಂತಿರುಗಿ &t_unknown;
    ಹಿಂತಿರುಗಿ (((type2_T *)ರಾಶಿ->ga_ದತ್ತಾಂಶ) + ರಾಶಿ->ga_len - ಸ್ಥಾನಾಂತರ - 1)
								   ->type_decl;
}

/*
 * Get the member type of a dict or list from the items on the stack of "cctx".
 * The declared type is stored in "decl_type".
 * For a list "skip" is 1, for a dict "skip" is 2, keys are skipped.
 * Returns &t_void for an empty list or dict.
 * Otherwise finds the common type of all items.
 */
    type_T *
get_member_type_from_ರಾಶಿ(
	ಪೂರ್ಣಾಂಕ	    ಎಣಿಕೆ_2,
	ಪೂರ್ಣಾಂಕ	    skip,
	cctx_T	    *cctx)
{
    garray_T	*ರಾಶಿ = &cctx->ctx_type_ರಾಶಿ;
    type2_T	*typep;
    garray_T    *type_gap = cctx->ctx_type_list;
    ಪೂರ್ಣಾಂಕ		i;
    type_T	*ಫಲಿತಾಂಶ;
    type_T	*type;

    // Use "unknown" for an empty list or dict.
    ಆದರೆ (ಎಣಿಕೆ_2 == 0)
	ಹಿಂತಿರುಗಿ &t_unknown;
    // Find the common type from following items.
    typep = ((type2_T *)ರಾಶಿ->ga_ದತ್ತಾಂಶ) + ರಾಶಿ->ga_len;
    ಫಲಿತಾಂಶ = &t_unknown;
    ಗಾಗಿ (i = 0; i < ಎಣಿಕೆ_2; ++i)
    {
	type = (typep -((ಎಣಿಕೆ_2 - i) * skip) + skip - 1)->type_curr;
	ಆದರೆ (check_type_is_ಮೌಲ್ಯ(type) == FAIL)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	ಆದರೆ (ಫಲಿತಾಂಶ != &t_any)
	    common_type(type, ಫಲಿತಾಂಶ, &ಫಲಿತಾಂಶ, type_gap);
    }

    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * Get the types of items in a tuple on the stack of "cctx".
 * Returns the number of types.  Returns -1 on failure.
 */
    ಪೂರ್ಣಾಂಕ
get_tuple_type_from_ರಾಶಿ(
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2,
    garray_T	*tuple_types_gap,
    cctx_T	*cctx)
{
    garray_T	*ರಾಶಿ = &cctx->ctx_type_ರಾಶಿ;
    type2_T	*typep;
    type_T	*type = ಖಾಲಿ;

    // Use "unknown" for an empty tuple
    ಆದರೆ (ಎಣಿಕೆ_2 == 0)
	ಹಿಂತಿರುಗಿ 0;

    // Find the common type from following items.
    typep = ((type2_T *)ರಾಶಿ->ga_ದತ್ತಾಂಶ) + ರಾಶಿ->ga_len;
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಎಣಿಕೆ_2; i++)
    {
	type = (typep - (ಎಣಿಕೆ_2 - i))->type_curr;
	ಆದರೆ (check_type_is_ಮೌಲ್ಯ(type) == FAIL)
	    ಹಿಂತಿರುಗಿ -1;
	ಆದರೆ (ga_grow(tuple_types_gap, 1) == FAIL)
	    ಹಿಂತಿರುಗಿ -1;
	((type_T **)tuple_types_gap->ga_ದತ್ತಾಂಶ)[tuple_types_gap->ga_len] = type;
	tuple_types_gap->ga_len++;
    }

    ಹಿಂತಿರುಗಿ tuple_types_gap->ga_len;
}

    ಅಕ್ಷರ *
vartype_name(vartype_T type)
{
    ಆಯ್ಕೆ (type)
    {
	ಸ್ಥಿತಿ VAR_UNKNOWN: ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_ANY: ಹಿಂತಿರುಗಿ "any";
	ಸ್ಥಿತಿ VAR_VOID: ಹಿಂತಿರುಗಿ "void";
	ಸ್ಥಿತಿ VAR_SPECIAL: ಹಿಂತಿರುಗಿ "special";
	ಸ್ಥಿತಿ VAR_BOOL: ಹಿಂತಿರುಗಿ "bool";
	ಸ್ಥಿತಿ VAR_ಸಂಖ್ಯೆ: ಹಿಂತಿರುಗಿ "number";
	ಸ್ಥಿತಿ VAR_FLOAT: ಹಿಂತಿರುಗಿ "float";
	ಸ್ಥಿತಿ VAR_STRING: ಹಿಂತಿರುಗಿ "string";
	ಸ್ಥಿತಿ VAR_BLOB: ಹಿಂತಿರುಗಿ "blob";
	ಸ್ಥಿತಿ VAR_ಕೆಲಸ: ಹಿಂತಿರುಗಿ "job";
	ಸ್ಥಿತಿ VAR_CHANNEL: ಹಿಂತಿರುಗಿ "channel";
	ಸ್ಥಿತಿ VAR_LIST: ಹಿಂತಿರುಗಿ "list";
	ಸ್ಥಿತಿ VAR_TUPLE: ಹಿಂತಿರುಗಿ "tuple";
	ಸ್ಥಿತಿ VAR_DICT: ಹಿಂತಿರುಗಿ "dict";
	ಸ್ಥಿತಿ VAR_INSTR: ಹಿಂತಿರುಗಿ "instr";
	ಸ್ಥಿತಿ VAR_CLASS: ಹಿಂತಿರುಗಿ "class";
	ಸ್ಥಿತಿ VAR_OBJECT: ಹಿಂತಿರುಗಿ "object";
	ಸ್ಥಿತಿ VAR_TYPEALIAS: ಹಿಂತಿರುಗಿ "typealias";

	ಸ್ಥಿತಿ VAR_FUNC:
	ಸ್ಥಿತಿ VAR_PARTIAL: ಹಿಂತಿರುಗಿ "func";
    }
    ಹಿಂತಿರುಗಿ "unknown";
}

/*
 * Return the type name of a List (list<type>) or Dict (dict<type>).
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    ಸ್ಥಿರ ಅಕ್ಷರ *
type_name_list_or_dict(ಅಕ್ಷರ *name, type_T *type, ಅಕ್ಷರ **tofree)
{
    ಅಕ್ಷರ *member_free;
    ಅಕ್ಷರ *member_name;

    ಆದರೆ (type->tt_member->tt_type == VAR_UNKNOWN)
	member_name = type_name(&t_any, &member_free);
    ಇಲ್ಲದಿದ್ದರೆ
	member_name = type_name(type->tt_member, &member_free);

    size_t len = STRLEN(name) + STRLEN(member_name) + 3;
    *tofree = alloc(len);
    ಆದರೆ (*tofree == ಖಾಲಿ)
    {
	vim_free(member_free);
	ಹಿಂತಿರುಗಿ name;
    }

    vim_snprintf(*tofree, len, "%s<%s>", name, member_name);
    vim_free(member_free);
    ಹಿಂತಿರುಗಿ *tofree;
}

/*
 * Return the type name of a tuple.
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    ಸ್ಥಿರ ಅಕ್ಷರ *
type_name_tuple(type_T *type, ಅಕ್ಷರ **tofree)
{
    garray_T    ga;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;
    ಅಕ್ಷರ	*arg_free = ಖಾಲಿ;

    ga_init2(&ga, 1, 100);
    ಆದರೆ (ga_grow(&ga, 20) == FAIL)
	ಹೋಗು failed;
    STRCPY(ga.ga_ದತ್ತಾಂಶ, "tuple<");
    ga.ga_len += 6;

    ಆದರೆ (type->tt_argcount <= 0)
	// empty tuple
	GA_CONCAT_LITERAL(&ga, "any");
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (type->tt_ವಾದಗಳು == ಖಾಲಿ)
	    GA_CONCAT_LITERAL(&ga, "[unknown]");
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಗಾಗಿ (i = 0; i < type->tt_argcount; ++i)
	    {
		string_T    arg_type;

		arg_type.string = (char_u *)type_name(type->tt_ವಾದಗಳು[i], &arg_free);
		ಆದರೆ (i > 0)
		{
		    STRCPY((ಅಕ್ಷರ *)ga.ga_ದತ್ತಾಂಶ + ga.ga_len, ", ");
		    ga.ga_len += 2;
		}
		arg_type.ಉದ್ದಳತೆ = STRLEN(arg_type.string);
		ಆದರೆ (ga_grow(&ga, (ಪೂರ್ಣಾಂಕ)arg_type.ಉದ್ದಳತೆ + 8) == FAIL)
		    ಹೋಗು failed;
		ಆದರೆ (varargs && i == type->tt_argcount - 1)
		    GA_CONCAT_LITERAL(&ga, "...");
		ga_concat_len(&ga, arg_type.string, arg_type.ಉದ್ದಳತೆ);
		VIM_CLEAR(arg_free);
	    }
	}
    }

    STRCPY((ಅಕ್ಷರ *)ga.ga_ದತ್ತಾಂಶ + ga.ga_len, ">");
    *tofree = ga.ga_ದತ್ತಾಂಶ;
    ಹಿಂತಿರುಗಿ ga.ga_ದತ್ತಾಂಶ;

failed:
    vim_free(arg_free);
    ga_clear(&ga);
    ಹಿಂತಿರುಗಿ "[unknown]";
}

/*
 * Return the type name of a Class (class<name>) or Object (object<name>).
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    ಸ್ಥಿರ ಅಕ್ಷರ *
type_name_class_or_obj(ಅಕ್ಷರ *name, type_T *type, ಅಕ್ಷರ **tofree)
{
    string_T	class_name;

    ಆದರೆ (type->tt_class != ಖಾಲಿ)
    {
	class_name.string = type->tt_class->class_name.string;
	class_name.ಉದ್ದಳತೆ = type->tt_class->class_name.ಉದ್ದಳತೆ;
	ಆದರೆ (IS_ENUM(type->tt_class))
	    name = "enum";
    }
    ಇಲ್ಲದಿದ್ದರೆ
	STR_LITERAL_ಗಣ(class_name, "any");

    size_t len = STRLEN(name) + class_name.ಉದ್ದಳತೆ + 3;
    *tofree = alloc(len);
    ಆದರೆ (*tofree == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ name;

    vim_snprintf(*tofree, len, "%s<%s>", name, class_name.string);
    ಹಿಂತಿರುಗಿ *tofree;
}

/*
 * Return the type name of a function.
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    ಸ್ಥಿರ ಅಕ್ಷರ *
type_name_func(type_T *type, ಅಕ್ಷರ **tofree)
{
    garray_T    ga;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;
    ಅಕ್ಷರ	*arg_free = ಖಾಲಿ;

    ga_init2(&ga, 1, 100);
    ಆದರೆ (ga_grow(&ga, 20) == FAIL)
	ಹೋಗು failed;
    STRCPY(ga.ga_ದತ್ತಾಂಶ, "func(");
    ga.ga_len += 5;

    ಗಾಗಿ (i = 0; i < type->tt_argcount; ++i)
    {
	string_T    arg_type;

	ಆದರೆ (type->tt_ವಾದಗಳು == ಖಾಲಿ)
	    STR_LITERAL_ಗಣ(arg_type, "[unknown]");
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    arg_type.string = (char_u *)type_name(type->tt_ವಾದಗಳು[i], &arg_free);
	    arg_type.ಉದ್ದಳತೆ = STRLEN(arg_type.string);
	}
	ಆದರೆ (i > 0)
	{
	    STRCPY((ಅಕ್ಷರ *)ga.ga_ದತ್ತಾಂಶ + ga.ga_len, ", ");
	    ga.ga_len += 2;
	}
	ಆದರೆ (ga_grow(&ga, (ಪೂರ್ಣಾಂಕ)arg_type.ಉದ್ದಳತೆ + 8) == FAIL)
	    ಹೋಗು failed;
	ಆದರೆ (varargs && i == type->tt_argcount - 1)
	    GA_CONCAT_LITERAL(&ga, "...");
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (i >= type->tt_min_argcount)
	    *((ಅಕ್ಷರ *)ga.ga_ದತ್ತಾಂಶ + ga.ga_len++) = '?';
	ga_concat_len(&ga, arg_type.string, arg_type.ಉದ್ದಳತೆ);
	VIM_CLEAR(arg_free);
    }
    ಆದರೆ (type->tt_argcount < 0)
	// any number of arguments
	GA_CONCAT_LITERAL(&ga, "...");

    ಆದರೆ (type->tt_member == &t_void)
	STRCPY((ಅಕ್ಷರ *)ga.ga_ದತ್ತಾಂಶ + ga.ga_len, ")");
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಅಕ್ಷರ *ret_free = ಖಾಲಿ;
	ಅಕ್ಷರ *ret_name = type_name(type->tt_member, &ret_free);
	ಪೂರ್ಣಾಂಕ  len;

	len = (ಪೂರ್ಣಾಂಕ)STRLEN(ret_name) + 4;
	ಆದರೆ (ga_grow(&ga, len) == FAIL)
	{
	    vim_free(ret_free);
	    ಹೋಗು failed;
	}
	STRCPY((ಅಕ್ಷರ *)ga.ga_ದತ್ತಾಂಶ + ga.ga_len, "): ");
	STRCPY((ಅಕ್ಷರ *)ga.ga_ದತ್ತಾಂಶ + ga.ga_len + 3, ret_name);
	vim_free(ret_free);
    }
    *tofree = ga.ga_ದತ್ತಾಂಶ;
    ಹಿಂತಿರುಗಿ ga.ga_ದತ್ತಾಂಶ;

failed:
    vim_free(arg_free);
    ga_clear(&ga);
    ಹಿಂತಿರುಗಿ "[unknown]";
}

/*
 * Return the name of a type.
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    ಅಕ್ಷರ *
type_name(type_T *type, ಅಕ್ಷರ **tofree)
{
    ಅಕ್ಷರ *name;

    *tofree = ಖಾಲಿ;
    ಆದರೆ (type == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ "[unknown]";
    name = vartype_name(type->tt_type);

    ಆಯ್ಕೆ (type->tt_type)
    {
	ಸ್ಥಿತಿ VAR_LIST:
	ಸ್ಥಿತಿ VAR_DICT:
	    ಹಿಂತಿರುಗಿ type_name_list_or_dict(name, type, tofree);

	ಸ್ಥಿತಿ VAR_TUPLE:
	    ಹಿಂತಿರುಗಿ type_name_tuple(type, tofree);

	ಸ್ಥಿತಿ VAR_CLASS:
	ಸ್ಥಿತಿ VAR_OBJECT:
	    ಹಿಂತಿರುಗಿ type_name_class_or_obj(name, type, tofree);

	ಸ್ಥಿತಿ VAR_FUNC:
	    ಹಿಂತಿರುಗಿ type_name_func(type, tofree);

	ಪೂರ್ವನಿಯೋಜಿತ:
	    ನಿಲ್ಲಿಸು;
    }

    ಹಿಂತಿರುಗಿ name;
}

/*
 * "typename(expr)" function
 */
    ಶೂನ್ಯ
f_typename(typval_T *argvars, typval_T *rettv)
{
    garray_T	type_list;
    type_T	*type;
    ಅಕ್ಷರ	*tofree;
    ಅಕ್ಷರ	*name;

    rettv->v_type = VAR_STRING;
    ga_init2(&type_list, ಗಾತ್ರ(type_T *), 10);
    ಆದರೆ (argvars[0].v_type == VAR_TYPEALIAS)
    {
	type = copy_type(argvars[0].vval.v_typealias->ta_type, &type_list);
	// A type alias for a class has the type set to VAR_OBJECT.  Change it
	// to VAR_CLASS, so that the name is "typealias<class<xxx>>"
	ಆದರೆ (type->tt_type == VAR_OBJECT)
	    type->tt_type = VAR_CLASS;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	type = typval2type(argvars, get_copyID(), &type_list, TVTT_DO_MEMBER);
    name = type_name(type, &tofree);
    ಆದರೆ (argvars[0].v_type == VAR_TYPEALIAS)
    {
	vim_snprintf((ಅಕ್ಷರ *)IObuff, IOSIZE, "typealias<%s>", name);
	rettv->vval.v_string = vim_strsave((char_u *)IObuff);
	ಆದರೆ (tofree != ಖಾಲಿ)
	    vim_free(tofree);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (tofree != ಖಾಲಿ)
	    rettv->vval.v_string = (char_u *)tofree;
	ಇಲ್ಲದಿದ್ದರೆ
	    rettv->vval.v_string = vim_strsave((char_u *)name);
    }
    clear_type_list(&type_list);
}

/*
 * Check if the typval_T is a value type; report an error if it is not.
 * Note: a type, user defined or typealias, is not a value type.
 *
 * Return OK if it's a value type, else FAIL
 */
    ಪೂರ್ಣಾಂಕ
check_typval_is_ಮೌಲ್ಯ(typval_T *tv)
{
    ಆದರೆ (tv == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ OK;

    ಆಯ್ಕೆ (tv->v_type)
    {
	ಸ್ಥಿತಿ VAR_CLASS:
	    {
		class_T *cl = tv->vval.v_class;
		char_u *class_name = (cl == ಖಾಲಿ) ? (char_u *)""
							: cl->class_name.string;
		ಆದರೆ (cl != ಖಾಲಿ && IS_ENUM(cl))
		    semsg(_(e_using_enum_as_ಮೌಲ್ಯ_str), class_name);
		ಇಲ್ಲದಿದ್ದರೆ
		    semsg(_(e_using_class_as_ಮೌಲ್ಯ_str), class_name);
	    }
	    ಹಿಂತಿರುಗಿ FAIL;

	ಸ್ಥಿತಿ VAR_TYPEALIAS:
	    semsg(_(e_using_typealias_as_ಮೌಲ್ಯ_str),
		    tv->vval.v_typealias->ta_name);
	    ಹಿಂತಿರುಗಿ FAIL;

	ಪೂರ್ವನಿಯೋಜಿತ:
	    ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Same as above, except check type_T.
 */
    ಪೂರ್ಣಾಂಕ
check_type_is_ಮೌಲ್ಯ(type_T *type)
{
    ಆದರೆ (type == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ OK;
    ಆಯ್ಕೆ (type->tt_type)
    {
	ಸ್ಥಿತಿ VAR_CLASS:
	    ಆದರೆ (type->tt_class != ಖಾಲಿ && IS_ENUM(type->tt_class))
		semsg(_(e_using_enum_as_ಮೌಲ್ಯ_str),
			type->tt_class->class_name.string);
	    ಇಲ್ಲದಿದ್ದರೆ
		semsg(_(e_using_class_as_ಮೌಲ್ಯ_str),
			type->tt_class == ಖಾಲಿ ? (char_u *)""
			: type->tt_class->class_name.string);
	    ಹಿಂತಿರುಗಿ FAIL;

	ಸ್ಥಿತಿ VAR_TYPEALIAS:
	    // TODO: Not sure what could be done here to get a name.
	    //       Maybe an optional argument?
	    emsg(_(e_using_typealias_as_var_val));
	    ಹಿಂತಿರುಗಿ FAIL;

	ಪೂರ್ವನಿಯೋಜಿತ:
	    ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ OK;
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_EVAL
