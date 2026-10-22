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

#define USING_FLOAT_STUFF
#ಅನ್ತರ್ಭಾವ "vim.h"

#ಯದಿ defined(FEAT_EVAL)

#ifdef VMS
# include <float.h>
#endif

/*
 * Allocate memory for a type_T and add the pointer to type_gap, so that it can
 * be easily freed later.
 */
    type_T *
get_type_ptr(garray_T *type_gap)
{
    type_T *type;

    ಯದಿ (ga_grow(type_gap, 1) == FAIL)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    type = ALLOC_CLEAR_ONE(type_T);
    ಯದಿ (type == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ((type_T **)type_gap->ga_data)[type_gap->ga_len] = type;
    ++type_gap->ga_len;
    ಪ್ರತ್ಯಾಗಮ type;
}

/*
 * Make a shallow copy of "type".
 * When allocation fails returns "type".
 */
    type_T *
copy_type(type_T *type, garray_T *type_gap)
{
    type_T *copy = get_type_ptr(type_gap);

    ಯದಿ (copy == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ type;
    *copy = *type;
    copy->tt_flags &= ~TTFLAG_STATIC;

    ಯದಿ (type->tt_args != ರಿಕ್ತ
	   && func_type_add_arg_types(copy, type->tt_argcount, type_gap) == OK)
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < type->tt_argcount; ++i)
	    copy->tt_args[i] = type->tt_args[i];

    ಪ್ರತ್ಯಾಗಮ copy;
}

/*
 * Inner part of copy_type_deep().
 * When allocation fails returns "type".
 */
    ಸ್ಥಿರ type_T *
copy_type_deep_rec(type_T *type, garray_T *type_gap, garray_T *seen_types)
{
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < seen_types->ga_len; ++i)
	ಯದಿ (((type_T **)seen_types->ga_data)[i * 2] == type)
	    // seen this type before, return the copy we made
	    ಪ್ರತ್ಯಾಗಮ ((type_T **)seen_types->ga_data)[i * 2 + 1];

    type_T *copy = copy_type(type, type_gap);
    ಯದಿ (ga_grow(seen_types, 1) == FAIL)
	ಪ್ರತ್ಯಾಗಮ copy;
    ((type_T **)seen_types->ga_data)[seen_types->ga_len * 2] = type;
    ((type_T **)seen_types->ga_data)[seen_types->ga_len * 2 + 1] = copy;
    ++seen_types->ga_len;

    ಯದಿ (copy->tt_member != ರಿಕ್ತ)
	copy->tt_member = copy_type_deep_rec(copy->tt_member,
							 type_gap, seen_types);
    ಯದಿ (type->tt_args != ರಿಕ್ತ)
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < type->tt_argcount; ++i)
	    copy->tt_args[i] = copy_type_deep_rec(copy->tt_args[i],
							 type_gap, seen_types);

    ಪ್ರತ್ಯಾಗಮ copy;
}

/*
 * Make a deep copy of "type".
 * When allocation fails returns "type".
 */
    type_T *
copy_type_deep(type_T *type, garray_T *type_gap)
{
    garray_T seen_types;
    // stores type pairs : a type we have seen and the copy used
    ga_init2(&seen_types, ಪರಿಮಾಣ(type_T *) * 2, 20);

    type_T *res = copy_type_deep_rec(type, type_gap, &seen_types);

    ga_clear(&seen_types);
    ಪ್ರತ್ಯಾಗಮ res;
}

    ಶೂನ್ಯ
clear_type_list(garray_T *gap)
{
    ಯಾವತ್ (gap->ga_len > 0)
	vim_free(((type_T **)gap->ga_data)[--gap->ga_len]);
    ga_clear(gap);
}

    ಶೂನ್ಯ
clear_func_type_list(garray_T *gap, type_T **func_type)
{
    ಯಾವತ್ (gap->ga_len > 0)
    {
	// func_type pointing to the uf_type_list, so reset pointer
	ಯದಿ (*func_type == ((type_T **)gap->ga_data)[--gap->ga_len])
	    *func_type = &t_func_any;
	vim_free(((type_T **)gap->ga_data)[gap->ga_len]);
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

    ಯದಿ (type == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // A fixed type never contains allocated types, return as-is.
    ಯದಿ (type->tt_flags & TTFLAG_STATIC)
	ಪ್ರತ್ಯಾಗಮ type;

    ret = ALLOC_ONE(type_T);
    ಯದಿ (ret == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    *ret = *type;

    ಯದಿ (ret->tt_member != ರಿಕ್ತ)
	ret->tt_member = alloc_type(ret->tt_member);

    ಯದಿ (type->tt_argcount > 0 && type->tt_args != ರಿಕ್ತ)
    {
	ಪೂರ್ಣಾಙ್ಕ i;

	ret->tt_args = ALLOC_MULT(type_T *, type->tt_argcount);
	ಯದಿ (ret->tt_args != ರಿಕ್ತ)
	    ಪ್ರತಿ (i = 0; i < type->tt_argcount; ++i)
		ret->tt_args[i] = alloc_type(type->tt_args[i]);
    }
    ಅನ್ಯಥಾ
	ret->tt_args = ರಿಕ್ತ;

    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Free a type that was created with alloc_type().
 */
    ಶೂನ್ಯ
free_type(type_T *type)
{
    ಪೂರ್ಣಾಙ್ಕ i;

    ಯದಿ (type == ರಿಕ್ತ || (type->tt_flags & TTFLAG_STATIC))
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (type->tt_args != ರಿಕ್ತ)
    {
	ಪ್ರತಿ (i = 0; i < type->tt_argcount; ++i)
	    free_type(type->tt_args[i]);
	vim_free(type->tt_args);
    }

    free_type(type->tt_member);

    vim_free(type);
}

/*
 * Return TRUE if "type" is to be recursed into for setting the type.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
set_tv_type_recurse(type_T *type)
{
    ಪ್ರತ್ಯಾಗಮ type->tt_member != ರಿಕ್ತ
		&& (type->tt_member->tt_type == VAR_DICT
				|| type->tt_member->tt_type == VAR_LIST)
		&& type->tt_member->tt_member != ರಿಕ್ತ
		&& type->tt_member->tt_member != &t_any
		&& type->tt_member->tt_member != &t_unknown;
}

/*
 * Set the type of Dict "d" to "type"
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
set_tv_type_dict(dict_T *d, type_T *type)
{
    ಯದಿ (d->dv_type == type)
	ಪ್ರತ್ಯಾಗಮ;

    free_type(d->dv_type);
    d->dv_type = alloc_type(type);

    // Need to recursively set the type of dict items?
    ಯದಿ (!set_tv_type_recurse(type))
	ಪ್ರತ್ಯಾಗಮ;

    ಪೂರ್ಣಾಙ್ಕ		todo = (ಪೂರ್ಣಾಙ್ಕ)d->dv_hashtab.ht_used;
    hashitem_T	*hi;
    dictitem_T	*di;

    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(&d->dv_hashtab, hi, todo)
    {
	ಯದಿ (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    di = HI2DI(hi);
	    set_tv_type(&di->di_tv, type->tt_member);
	}
    }
}

/*
 * Set the type of List "l" to "type"
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
set_tv_type_list(list_T *l, type_T *type)
{
    ಯದಿ (l->lv_type == type)
	ಪ್ರತ್ಯಾಗಮ;

    free_type(l->lv_type);
    l->lv_type = alloc_type(type);

    // Need to recursively set the type of list items?
    ಯದಿ (l->lv_first == &range_list_item || !set_tv_type_recurse(type))
	ಪ್ರತ್ಯಾಗಮ;

    listitem_T	*li;

    FOR_ALL_LIST_ವಸ್ತುಗಳು(l, li)
	set_tv_type(&li->li_tv, type->tt_member);
}

/*
 * Set the type of Tuple "tuple" to "type"
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
set_tv_type_tuple(tuple_T *tuple, type_T *type)
{
    ಯದಿ (tuple->tv_type == type)
	ಪ್ರತ್ಯಾಗಮ;

    free_type(tuple->tv_type);
    tuple->tv_type = alloc_type(type);

    ಯದಿ (type->tt_argcount <= 0)
	ಪ್ರತ್ಯಾಗಮ;

    // recursively set the type of list items
    type_T	*item_type;
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < tuple_len(tuple); i++)
    {
	ಯದಿ ((type->tt_flags & TTFLAG_VARARGS) && (i >= type->tt_argcount - 1))
	    // For a variadic tuple, the last type is a List.  So use the
	    // List member type.
	    item_type = type->tt_args[type->tt_argcount - 1]->tt_member;
	ಅನ್ಯಥಾ
	    item_type = type->tt_args[i];

	set_tv_type(TUPLE_ITEM(tuple, i), item_type);
    }
}

/*
 * Set the type of "tv" to "type" if it is a list or tuple or dict.
 */
    ಶೂನ್ಯ
set_tv_type(typval_T *tv, type_T *type)
{
    ಯದಿ (type->tt_type == VAR_ANY)
	// If the variable type is "any", then keep the value type.
	// e.g.  var x: any = [1, 2] or var y: any = {v: 1}
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (tv->v_type == VAR_DICT && tv->vval.v_dict != ರಿಕ್ತ)
	set_tv_type_dict(tv->vval.v_dict, type);
    ಅನ್ಯಥಾ ಯದಿ (tv->v_type == VAR_LIST && tv->vval.v_list != ರಿಕ್ತ)
	set_tv_type_list(tv->vval.v_list, type);
    ಅನ್ಯಥಾ ಯದಿ (tv->v_type == VAR_TUPLE && tv->vval.v_tuple != ರಿಕ್ತ)
	set_tv_type_tuple(tv->vval.v_tuple, type);
}

/*
 * For a tuple type, reserve space for "typecount" types (including the
 * repeated type).
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
tuple_type_add_types(
    type_T	*tupletype,
    ಪೂರ್ಣಾಙ್ಕ		typecount,
    garray_T	*type_gap)
{
    // To make it easy to free the space needed for the types, add the
    // pointer to type_gap.
    ಯದಿ (ga_grow(type_gap, 1) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    tupletype->tt_args = alloc_clear_id(ಪರಿಮಾಣ(type_T *) * typecount,
							  aid_tuple_type_args);
    ಯದಿ (tupletype->tt_args == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ((type_T **)type_gap->ga_data)[type_gap->ga_len] =
						(ಶೂನ್ಯ *)tupletype->tt_args;
    ++type_gap->ga_len;
    ಪ್ರತ್ಯಾಗಮ OK;
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
    ಯದಿ (member_type == ರಿಕ್ತ || (member_type->tt_type == VAR_ANY
					&& !IS_GENERIC_TYPE(member_type)))
	ಪ್ರತ್ಯಾಗಮ &t_list_any;
    ಯದಿ (member_type->tt_type == VAR_VOID
	    || member_type->tt_type == VAR_UNKNOWN)
	ಪ್ರತ್ಯಾಗಮ &t_list_empty;
    ಯದಿ (member_type->tt_type == VAR_BOOL)
	ಪ್ರತ್ಯಾಗಮ &t_list_bool;
    ಯದಿ (member_type->tt_type == VAR_ಸಂಖ್ಯೆ)
	ಪ್ರತ್ಯಾಗಮ &t_list_ಸಂಖ್ಯೆ;
    ಯದಿ (member_type->tt_type == VAR_STRING)
	ಪ್ರತ್ಯಾಗಮ &t_list_string;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    ಯದಿ (type == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ &t_any;
    type->tt_type = VAR_LIST;
    type->tt_member = member_type;
    type->tt_argcount = 0;
    type->tt_args = ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ type;
}

/*
 * Create and return a tuple type from the tuple item types in
 * "tuple_types_ga".
 */
    type_T *
get_tuple_type(garray_T *tuple_types_gap, garray_T *type_gap)
{
    type_T	*type;
    type_T	**tuple_types = tuple_types_gap->ga_data;
    ಪೂರ್ಣಾಙ್ಕ		typecount = tuple_types_gap->ga_len;

    // recognize commonly used types
    ಯದಿ (typecount == 0)
	ಪ್ರತ್ಯಾಗಮ &t_tuple_any;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    ಯದಿ (type == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ &t_any;
    type->tt_type = VAR_TUPLE;
    type->tt_member = ರಿಕ್ತ;
    ಯದಿ (typecount > 0)
    {
	ಯದಿ (tuple_type_add_types(type, typecount, type_gap) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ &t_any;
	mch_memmove(type->tt_args, tuple_types, ಪರಿಮಾಣ(type_T *) * typecount);
    }
    type->tt_argcount = typecount;
    type->tt_flags = 0;

    ಪ್ರತ್ಯಾಗಮ type;
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
    ಯದಿ (member_type == ರಿಕ್ತ || (member_type->tt_type == VAR_ANY
					&& !IS_GENERIC_TYPE(member_type)))
	ಪ್ರತ್ಯಾಗಮ &t_dict_any;
    ಯದಿ (member_type->tt_type == VAR_VOID
	    || member_type->tt_type == VAR_UNKNOWN)
	ಪ್ರತ್ಯಾಗಮ &t_dict_empty;
    ಯದಿ (member_type->tt_type == VAR_BOOL)
	ಪ್ರತ್ಯಾಗಮ &t_dict_bool;
    ಯದಿ (member_type->tt_type == VAR_ಸಂಖ್ಯೆ)
	ಪ್ರತ್ಯಾಗಮ &t_dict_ಸಂಖ್ಯೆ;
    ಯದಿ (member_type->tt_type == VAR_STRING)
	ಪ್ರತ್ಯಾಗಮ &t_dict_string;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    ಯದಿ (type == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ &t_any;
    type->tt_type = VAR_DICT;
    type->tt_member = member_type;
    type->tt_argcount = 0;
    type->tt_args = ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ type;
}

/*
 * Allocate a new type for a function.
 */
    type_T *
alloc_func_type(type_T *ret_type, ಪೂರ್ಣಾಙ್ಕ argcount, garray_T *type_gap)
{
    type_T *type = get_type_ptr(type_gap);

    ಯದಿ (type == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ &t_any;
    type->tt_type = VAR_FUNC;
    type->tt_member = ret_type == ರಿಕ್ತ ? &t_unknown : ret_type;
    type->tt_argcount = argcount;
    type->tt_args = ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ type;
}

/*
 * Allocate a new type for a tuple.
 */
    ಸ್ಥಿರ type_T *
alloc_tuple_type(ಪೂರ್ಣಾಙ್ಕ typecount, garray_T *type_gap)
{
    type_T *type = get_type_ptr(type_gap);

    ಯದಿ (type == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ &t_any;
    type->tt_type = VAR_TUPLE;
    type->tt_member = ರಿಕ್ತ;
    type->tt_argcount = typecount;
    type->tt_args = ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ type;
}

/*
 * Get a function type, based on the return type "ret_type".
 * "argcount" must be -1 or 0, a predefined type can be used.
 */
    type_T *
get_func_type(type_T *ret_type, ಪೂರ್ಣಾಙ್ಕ argcount, garray_T *type_gap)
{
    // recognize commonly used types
    ಯದಿ (ret_type == &t_unknown || ret_type == ರಿಕ್ತ)
    {
	// (argcount == 0) is not possible
	ಪ್ರತ್ಯಾಗಮ &t_func_unknown;
    }
    ಯದಿ (ret_type == &t_void)
    {
	ಯದಿ (argcount == 0)
	    ಪ್ರತ್ಯಾಗಮ &t_func_0_void;
	ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ &t_func_void;
    }
    ಯದಿ (ret_type == &t_any)
    {
	ಯದಿ (argcount == 0)
	    ಪ್ರತ್ಯಾಗಮ &t_func_0_any;
	ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ &t_func_any;
    }
    ಯದಿ (ret_type == &t_ಸಂಖ್ಯೆ)
    {
	ಯದಿ (argcount == 0)
	    ಪ್ರತ್ಯಾಗಮ &t_func_0_ಸಂಖ್ಯೆ;
	ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ &t_func_ಸಂಖ್ಯೆ;
    }
    ಯದಿ (ret_type == &t_string)
    {
	ಯದಿ (argcount == 0)
	    ಪ್ರತ್ಯಾಗಮ &t_func_0_string;
	ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ &t_func_string;
    }

    ಪ್ರತ್ಯಾಗಮ alloc_func_type(ret_type, argcount, type_gap);
}

/*
 * For a function type, reserve space for "argcount" argument types (including
 * vararg).
 */
    ಪೂರ್ಣಾಙ್ಕ
func_type_add_arg_types(
	type_T	    *functype,
	ಪೂರ್ಣಾಙ್ಕ	    argcount,
	garray_T    *type_gap)
{
    // To make it easy to free the space needed for the argument types, add the
    // pointer to type_gap.
    ಯದಿ (ga_grow(type_gap, 1) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    functype->tt_args = ALLOC_CLEAR_MULT(type_T *, argcount);
    ಯದಿ (functype->tt_args == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ((type_T **)type_gap->ga_data)[type_gap->ga_len] =
						     (ಶೂನ್ಯ *)functype->tt_args;
    ++type_gap->ga_len;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Return TRUE if "type" is NULL, any or unknown.
 * This also works for const (comparing with &t_any and &t_unknown doesn't).
 */
    ಪೂರ್ಣಾಙ್ಕ
type_any_or_unknown(type_T *type)
{
    ಪ್ರತ್ಯಾಗಮ type == ರಿಕ್ತ || type->tt_type == VAR_ANY
					|| type->tt_type == VAR_UNKNOWN;
}

/*
 * Get a type_T for a "special" typval in "tv".
 */
    ಸ್ಥಿರ type_T *
special_typval2type(typval_T *tv)
{
    ವಿಕಲ್ಪ (tv->vval.v_ಸಂಖ್ಯೆ)
    {
	ಪಕ್ಷ VVAL_NULL:
	    ಪ್ರತ್ಯಾಗಮ &t_null;

	ಪಕ್ಷ VVAL_NONE:
	    ಪ್ರತ್ಯಾಗಮ &t_none;

	ಪಕ್ಷ VVAL_TRUE:
	ಪಕ್ಷ VVAL_FALSE:
	    ಪ್ರತ್ಯಾಗಮ &t_bool;

	ಸ್ವಭಾವ:
	    ಪ್ರತ್ಯಾಗಮ &t_unknown;
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
list_typval2type(typval_T *tv, ಪೂರ್ಣಾಙ್ಕ copyID, garray_T *type_gap, ಪೂರ್ಣಾಙ್ಕ flags)
{
    list_T	*l = tv->vval.v_list;
    listitem_T	*li;
    type_T	*member_type = ರಿಕ್ತ;

    // An empty list has type list<unknown>, unless the type was specified
    // and is not list<any>.  This matters when assigning to a variable
    // with a specific list type.
    ಯದಿ (l == ರಿಕ್ತ || (l->lv_first == ರಿಕ್ತ
		&& (l->lv_type == ರಿಕ್ತ || l->lv_type->tt_member == &t_any)))
	ಪ್ರತ್ಯಾಗಮ &t_list_empty;

    ಯದಿ ((flags & TVTT_DO_MEMBER) == 0)
	ಪ್ರತ್ಯಾಗಮ &t_list_any;

    // If the type is list<any> go through the members, it may end up a
    // more specific type.
    ಯದಿ (l->lv_type != ರಿಕ್ತ && (l->lv_first == ರಿಕ್ತ
		|| (flags & TVTT_MORE_SPECIFIC) == 0
		|| l->lv_type->tt_member != &t_any))
	// make a copy, lv_type may be freed if the list is freed
	ಪ್ರತ್ಯಾಗಮ copy_type_deep(l->lv_type, type_gap);

    ಯದಿ (l->lv_first == &range_list_item)
	ಪ್ರತ್ಯಾಗಮ &t_list_ಸಂಖ್ಯೆ;

    ಯದಿ (l->lv_copyID == copyID)
	// avoid recursion
	ಪ್ರತ್ಯಾಗಮ &t_list_any;

    l->lv_copyID = copyID;

    // Use the common type of all members.
    member_type = typval2type(&l->lv_first->li_tv, copyID, type_gap,
							TVTT_DO_MEMBER);
    ಪ್ರತಿ (li = l->lv_first->li_next; li != ರಿಕ್ತ; li = li->li_next)
	common_type(typval2type(&li->li_tv, copyID, type_gap, TVTT_DO_MEMBER),
					member_type, &member_type, type_gap);

    // Reset copyID so that a shared reference to this list (not a circular
    // reference) can be processed again to get the correct type.
    l->lv_copyID = 0;

    ಪ್ರತ್ಯಾಗಮ get_list_type(member_type, type_gap);
}

/*
 * Get a type_T for a Tuple typval in "tv".
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    ಸ್ಥಿರ type_T *
tuple_typval2type(typval_T *tv, ಪೂರ್ಣಾಙ್ಕ copyID, garray_T *type_gap, ಪೂರ್ಣಾಙ್ಕ flags)
{
    tuple_T	*tuple = tv->vval.v_tuple;
    ಪೂರ್ಣಾಙ್ಕ		len = tuple_len(tuple);
    type_T	*type = ರಿಕ್ತ;

    // An empty tuple has type tuple<unknown>, unless the type was specified
    // and is not tuple<any>.  This matters when assigning to a variable
    // with a specific tuple type.
    ಯದಿ (tuple == ರಿಕ್ತ || (len == 0 && (tuple->tv_type == ರಿಕ್ತ
				|| tuple->tv_type->tt_argcount == 0)))
	ಪ್ರತ್ಯಾಗಮ &t_tuple_empty;

    ಯದಿ ((flags & TVTT_DO_MEMBER) == 0)
	ಪ್ರತ್ಯಾಗಮ &t_tuple_any;

    // If the type is tuple<any> go through the members, it may end up a
    // more specific type.
    ಯದಿ (tuple->tv_type != ರಿಕ್ತ && (len == 0
					|| (flags & TVTT_MORE_SPECIFIC) == 0))
	// make a copy, tv_type may be freed if the tuple is freed
	ಪ್ರತ್ಯಾಗಮ copy_type_deep(tuple->tv_type, type_gap);

    ಯದಿ (tuple->tv_copyID == copyID)
	// avoid recursion
	ಪ್ರತ್ಯಾಗಮ &t_tuple_any;

    tuple->tv_copyID = copyID;

    garray_T	tuple_types_ga;
    ga_init2(&tuple_types_ga, ಪರಿಮಾಣ(type_T *), 10);
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < len; i++)
    {
	type = typval2type(TUPLE_ITEM(tuple, i), copyID, type_gap,
							TVTT_DO_MEMBER);
	ಯದಿ (ga_grow(&tuple_types_ga, 1) == FAIL)
	{
	    ga_clear(&tuple_types_ga);
	    // Reset copyID so that a shared reference to this tuple can be
	    // processed again.
	    tuple->tv_copyID = 0;
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
	((type_T **)tuple_types_ga.ga_data)[tuple_types_ga.ga_len] = type;
	tuple_types_ga.ga_len++;
    }

    type_T *tuple_type = get_tuple_type(&tuple_types_ga, type_gap);
    ga_clear(&tuple_types_ga);

    // Reset copyID so that a shared reference to this tuple (not a circular
    // reference) can be processed again to get the correct type.
    tuple->tv_copyID = 0;

    ಪ್ರತ್ಯಾಗಮ tuple_type;
}

/*
 * Get a type_T for a Dict typval in "tv".
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    ಸ್ಥಿರ type_T *
dict_typval2type(typval_T *tv, ಪೂರ್ಣಾಙ್ಕ copyID, garray_T *type_gap, ಪೂರ್ಣಾಙ್ಕ flags)
{
    dict_iterator_T	iter;
    typval_T		*ಮೌಲ್ಯ;
    dict_T		*d = tv->vval.v_dict;
    type_T		*member_type = ರಿಕ್ತ;

    ಯದಿ (d == ರಿಕ್ತ || (d->dv_hashtab.ht_used == 0 && d->dv_type == ರಿಕ್ತ))
	ಪ್ರತ್ಯಾಗಮ &t_dict_empty;

    ಯದಿ ((flags & TVTT_DO_MEMBER) == 0)
	ಪ್ರತ್ಯಾಗಮ &t_dict_any;

    // If the type is dict<any> go through the members, it may end up a
    // more specific type.
    ಯದಿ (d->dv_type != ರಿಕ್ತ && (d->dv_hashtab.ht_used == 0
		|| (flags & TVTT_MORE_SPECIFIC) == 0
		|| d->dv_type->tt_member != &t_any))
	ಪ್ರತ್ಯಾಗಮ d->dv_type;

    ಯದಿ (d->dv_copyID == copyID)
	// avoid recursion
	ಪ್ರತ್ಯಾಗಮ &t_dict_any;

    d->dv_copyID = copyID;

    // Use the common type of all values.
    dict_iterate_start(tv, &iter);
    dict_iterate_next(&iter, &ಮೌಲ್ಯ);
    member_type = typval2type(ಮೌಲ್ಯ, copyID, type_gap, TVTT_DO_MEMBER);

    ಯಾವತ್ (dict_iterate_next(&iter, &ಮೌಲ್ಯ) != ರಿಕ್ತ)
	common_type(typval2type(ಮೌಲ್ಯ, copyID, type_gap, TVTT_DO_MEMBER),
					member_type, &member_type, type_gap);

    // Reset copyID so that a shared reference to this dict (not a circular
    // reference) can be processed again to get the correct type.
    d->dv_copyID = 0;

    ಪ್ರತ್ಯಾಗಮ get_dict_type(member_type, type_gap);
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
    ಯದಿ (type == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    *type = *ufunc->uf_func_type;
    ಯದಿ (type->tt_argcount >= 0 && pt->pt_argc > 0)
    {
	type->tt_argcount -= pt->pt_argc;
	type->tt_min_argcount -= pt->pt_argc;
	ಯದಿ (type->tt_argcount > 0 && func_type_add_arg_types(type,
		    type->tt_argcount, type_gap) == OK)
	    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < type->tt_argcount; ++i)
		type->tt_args[i] =
		    ufunc->uf_func_type->tt_args[i + pt->pt_argc];
    }
    ಯದಿ (pt->pt_func != ರಿಕ್ತ)
	type->tt_member = pt->pt_func->uf_ret_type;

    ಪ್ರತ್ಯಾಗಮ type;
}

/*
 * Get a type_T for a "class" or an "object" typval in "tv".
 */
    ಸ್ಥಿರ type_T *
oc_typval2type(typval_T *tv)
{
    ಯದಿ (tv->v_type == VAR_CLASS)
    {
	ಯದಿ (tv->vval.v_class == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ &t_class;

	ಪ್ರತ್ಯಾಗಮ &tv->vval.v_class->class_type;
    }

    ಯದಿ (tv->vval.v_object != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ &tv->vval.v_object->obj_class->class_object_type;

    ಪ್ರತ್ಯಾಗಮ &t_object_any;
}

/*
 * Get a type_T for a "function" or a "partial"
 */
    ಸ್ಥಿರ type_T *
fp_typval2type(typval_T *tv, garray_T *type_gap)
{
    char_u	*name = ರಿಕ್ತ;
    ufunc_T	*ufunc = ರಿಕ್ತ;
    type_T	*type;
    type_T	*member_type = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		argcount = 0;
    ಪೂರ್ಣಾಙ್ಕ		min_argcount = 0;

    ಯದಿ (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != ರಿಕ್ತ)
    {
	ಯದಿ (tv->vval.v_partial->pt_func != ರಿಕ್ತ)
	    ufunc = tv->vval.v_partial->pt_func;
	ಅನ್ಯಥಾ
	    name = tv->vval.v_partial->pt_name;
    }
    ಅನ್ಯಥಾ
	name = tv->vval.v_string;

    ಯದಿ (name == ರಿಕ್ತ && ufunc == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ &t_func_unknown;

    ಯದಿ (name != ರಿಕ್ತ)
    {
	ಪೂರ್ಣಾಙ್ಕ idx = find_internal_func(name);

	ಯದಿ (idx >= 0)
	{
	    type_T *decl_type;  // unused

	    internal_func_get_argcount(idx, &argcount, &min_argcount);
	    member_type = internal_func_ret_type(idx, 0, ರಿಕ್ತ, &decl_type,
		    type_gap);
	}
	ಅನ್ಯಥಾ
	{
	    // Check if name contains "<".  If it does, then replace "<" with
	    // NUL and lookup the function and then look up the specific
	    // generic function using the supplied types.
	    char_u *p = generic_func_find_open_bracket(name);
	    ಯದಿ (p == ರಿಕ್ತ)
		ufunc = find_func(name, FALSE);
	    ಅನ್ಯಥಾ
	    {
		// generic function
		char_u	cc = *p;

		*p = NUL;
		ufunc = find_func(name, FALSE);
		*p = cc;
		ಯದಿ (ufunc != ರಿಕ್ತ && IS_GENERIC_FUNC(ufunc))
		{
		    ufunc = find_generic_func(ufunc, name, &p);
		    ಯದಿ (ufunc == ರಿಕ್ತ)
			ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		}
	    }
	}
    }
    ಯದಿ (ufunc != ರಿಕ್ತ)
    {
	// May need to get the argument types from default values by
	// compiling the function.
	ಯದಿ (ufunc->uf_def_status == UF_TO_BE_COMPILED
		&& compile_def_function(ufunc, TRUE, CT_NONE, ರಿಕ್ತ)
		== FAIL)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	ಯದಿ (ufunc->uf_func_type == ರಿಕ್ತ)
	    set_function_type(ufunc);
	ಯದಿ (ufunc->uf_func_type != ರಿಕ್ತ)
	{
	    ಯದಿ (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ partial_typval2type(tv, ufunc, type_gap);
	    ಪ್ರತ್ಯಾಗಮ ufunc->uf_func_type;
	}
    }

    type = get_type_ptr(type_gap);
    ಯದಿ (type == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    type->tt_type = tv->v_type;
    type->tt_argcount = argcount;
    type->tt_min_argcount = min_argcount;
    ಯದಿ (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != ರಿಕ್ತ
					&& tv->vval.v_partial->pt_argc > 0)
    {
	type->tt_argcount -= tv->vval.v_partial->pt_argc;
	type->tt_min_argcount -= tv->vval.v_partial->pt_argc;
    }
    type->tt_member = member_type;

    ಪ್ರತ್ಯಾಗಮ type;
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
typval2type_int(typval_T *tv, ಪೂರ್ಣಾಙ್ಕ copyID, garray_T *type_gap, ಪೂರ್ಣಾಙ್ಕ flags)
{
    ವಿಕಲ್ಪ (tv->v_type)
    {
	ಪಕ್ಷ VAR_UNKNOWN:
	    ಪ್ರತ್ಯಾಗಮ &t_unknown;

	ಪಕ್ಷ VAR_ANY:
	    ಪ್ರತ್ಯಾಗಮ &t_any;

	ಪಕ್ಷ VAR_VOID:
	    ಪ್ರತ್ಯಾಗಮ &t_void;

	ಪಕ್ಷ VAR_BOOL:
	    ಪ್ರತ್ಯಾಗಮ &t_bool;

	ಪಕ್ಷ VAR_SPECIAL:
	    ಪ್ರತ್ಯಾಗಮ special_typval2type(tv);

	ಪಕ್ಷ VAR_ಸಂಖ್ಯೆ:
	    ಪ್ರತ್ಯಾಗಮ &t_ಸಂಖ್ಯೆ;

	ಪಕ್ಷ VAR_FLOAT:
	    ಪ್ರತ್ಯಾಗಮ &t_float;

	ಪಕ್ಷ VAR_STRING:
	    ಪ್ರತ್ಯಾಗಮ &t_string;

	ಪಕ್ಷ VAR_BLOB:
	    ಯದಿ (tv->vval.v_blob == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ &t_blob_null;
	    ಪ್ರತ್ಯಾಗಮ &t_blob;

	ಪಕ್ಷ VAR_LIST:
	    ಪ್ರತ್ಯಾಗಮ list_typval2type(tv, copyID, type_gap, flags);

	ಪಕ್ಷ VAR_TUPLE:
	    ಪ್ರತ್ಯಾಗಮ tuple_typval2type(tv, copyID, type_gap, flags);

	ಪಕ್ಷ VAR_DICT:
	    ಪ್ರತ್ಯಾಗಮ dict_typval2type(tv, copyID, type_gap, flags);

	ಪಕ್ಷ VAR_JOB:
	    ಪ್ರತ್ಯಾಗಮ &t_job;

	ಪಕ್ಷ VAR_CHANNEL:
	    ಪ್ರತ್ಯಾಗಮ &t_channel;

	ಪಕ್ಷ VAR_CLASS:
	ಪಕ್ಷ VAR_OBJECT:
	    ಪ್ರತ್ಯಾಗಮ oc_typval2type(tv);

	ಪಕ್ಷ VAR_TYPEALIAS:
	    ಪ್ರತ್ಯಾಗಮ &t_typealias;

	ಪಕ್ಷ VAR_FUNC:
	ಪಕ್ಷ VAR_PARTIAL:
	    ಪ್ರತ್ಯಾಗಮ fp_typval2type(tv, type_gap);

	ಪಕ್ಷ VAR_INSTR:
	ಸ್ವಭಾವ:
	    ಭಙ್ಗ;
    }

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Return TRUE if "tv" is not a bool but should be converted to bool.
 */
    ಪೂರ್ಣಾಙ್ಕ
need_convert_to_bool(type_T *type, typval_T *tv)
{
    ಪ್ರತ್ಯಾಗಮ type != ರಿಕ್ತ && type == &t_bool && tv->v_type != VAR_BOOL
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
typval2type(typval_T *tv, ಪೂರ್ಣಾಙ್ಕ copyID, garray_T *type_gap, ಪೂರ್ಣಾಙ್ಕ flags)
{
    type_T *type = typval2type_int(tv, copyID, type_gap, flags);

    ಯದಿ (type == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (type != &t_bool && (tv->v_type == VAR_ಸಂಖ್ಯೆ
		&& (tv->vval.v_ಸಂಖ್ಯೆ == 0 || tv->vval.v_ಸಂಖ್ಯೆ == 1)))
	// Number 0 and 1 and expression with "&&" or "||" can also be used
	// for bool.
	type = &t_ಸಂಖ್ಯೆ_bool;
    ಅನ್ಯಥಾ ಯದಿ (type != &t_float && tv->v_type == VAR_ಸಂಖ್ಯೆ)
	// A number can also be used for float.
	type = &t_ಸಂಖ್ಯೆ_float;
    ಪ್ರತ್ಯಾಗಮ type;
}

/*
 * Return TRUE if "type" can be used for a variable declaration.
 * Give an error and return FALSE if not.
 */
    ಪೂರ್ಣಾಙ್ಕ
valid_declaration_type(type_T *type)
{
    ಯದಿ (type->tt_type == VAR_SPECIAL  // null, none
	    || type->tt_type == VAR_VOID)
    {
	ವರ್ಣ *tofree = ರಿಕ್ತ;
	ವರ್ಣ *name = type_name(type, &tofree);
	semsg(_(e_invalid_type_in_variable_declaration_str), name);
	vim_free(tofree);
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Get a type_T for a typval_T, used for v: variables.
 * "type_list" is used to temporarily create types in.
 */
    type_T *
typval2type_vimvar(typval_T *tv, garray_T *type_gap)
{
    ಯದಿ (tv->v_type == VAR_LIST)  // e.g. for v:oldfiles
	ಪ್ರತ್ಯಾಗಮ &t_list_string;
    ಯದಿ (tv->v_type == VAR_DICT)  // e.g. for v:event
	ಪ್ರತ್ಯಾಗಮ &t_dict_any;
    ಪ್ರತ್ಯಾಗಮ typval2type(tv, get_copyID(), type_gap, TVTT_DO_MEMBER);
}

    ಪೂರ್ಣಾಙ್ಕ
check_typval_arg_type(
	type_T	    *expected,
	typval_T    *actual_tv,
	ವರ್ಣ	    *func_name,
	ಪೂರ್ಣಾಙ್ಕ	    arg_idx)
{
    where_T	where = WHERE_INIT;

    ಯದಿ (arg_idx > 0)
    {
	where.wt_index = arg_idx;
	where.wt_kind = WT_ARGUMENT;
    }
    where.wt_func_name = func_name;
    ಪ್ರತ್ಯಾಗಮ check_typval_type(expected, actual_tv, where);
}

/*
 * Return true if "expected" is list<any> or dict<any> and "actual_tv" is a
 * list or dict of that kind, which matches whatever its values are.
 */
    ಸ್ಥಿರ bool
matches_listdict_any(type_T *expected, typval_T *actual_tv)
{
    ಪ್ರತ್ಯಾಗಮ (expected->tt_type == VAR_LIST || expected->tt_type == VAR_DICT)
	    && actual_tv->v_type == expected->tt_type
	    && expected->tt_member != ರಿಕ್ತ
	    && (expected->tt_member->tt_type == VAR_ANY
		|| expected->tt_member->tt_type == VAR_UNKNOWN);
}

/*
 * Return FAIL if "expected" and "actual" don't match.
 * When "argidx" > 0 it is included in the error message.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_typval_type(type_T *expected, typval_T *actual_tv, where_T where)
{
    garray_T	type_list;
    type_T	*actual_type;
    ಪೂರ್ಣಾಙ್ಕ		res = FAIL;

    ಯದಿ (expected == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ OK;  // didn't expect anything.

    // No need to go through the values of a possibly big list or dict.
    ಯದಿ (matches_listdict_any(expected, actual_tv))
	ಪ್ರತ್ಯಾಗಮ OK;

    ga_init2(&type_list, ಪರಿಮಾಣ(type_T *), 10);

    // A null_function and null_partial are special cases, they can be used to
    // clear a variable.
    ಯದಿ ((actual_tv->v_type == VAR_FUNC && actual_tv->vval.v_string == ರಿಕ್ತ)
	    || (actual_tv->v_type == VAR_PARTIAL
					 && actual_tv->vval.v_partial == ರಿಕ್ತ))
	actual_type = &t_func_unknown;
    ಅನ್ಯಥಾ
	// When the actual type is list<any> or dict<any> go through the values
	// to possibly get a more specific type.
	actual_type = typval2type(actual_tv, get_copyID(), &type_list,
					  TVTT_DO_MEMBER | TVTT_MORE_SPECIFIC);
    ಯದಿ (actual_type != ರಿಕ್ತ)
    {
	res = check_type_maybe(expected, actual_type, TRUE, where);
	ಯದಿ (res == MAYBE && !(actual_type->tt_type == VAR_FUNC
				      && (actual_type->tt_member == &t_unknown
					  || actual_type->tt_member == ರಿಕ್ತ)))
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
    ಪ್ರತ್ಯಾಗಮ res;
}

    ಶೂನ್ಯ
arg_type_mismatch(type_T *expected, type_T *actual, ಪೂರ್ಣಾಙ್ಕ arg_idx)
{
    where_T	where = WHERE_INIT;

    ಯದಿ (arg_idx > 0)
    {
	where.wt_index = arg_idx;
	where.wt_kind = WT_ARGUMENT;
    }
    type_mismatch_where(expected, actual, where);
}

    ಶೂನ್ಯ
type_mismatch_where(type_T *expected, type_T *actual, where_T where)
{
    ವರ್ಣ *tofree1, *tofree2;
    ವರ್ಣ *typename1 = type_name(expected, &tofree1);
    ವರ್ಣ *typename2 = type_name(actual, &tofree2);

    ವಿಕಲ್ಪ (where.wt_kind)
    {
	ಪಕ್ಷ WT_MEMBER:
	    semsg(_(e_variable_str_type_mismatch_expected_str_but_got_str),
		    where.wt_func_name, typename1, typename2);
	    ಭಙ್ಗ;
	ಪಕ್ಷ WT_METHOD:
	ಪಕ್ಷ WT_METHOD_ARG:
	ಪಕ್ಷ WT_METHOD_RETURN:
	    semsg(_(e_method_str_type_mismatch_expected_str_but_got_str),
		    where.wt_func_name, typename1, typename2);
	    ಭಙ್ಗ;
	ಪಕ್ಷ WT_VARIABLE:
	    ಯದಿ (where.wt_func_name == ರಿಕ್ತ)
		semsg(_(e_variable_nr_type_mismatch_expected_str_but_got_str),
			where.wt_index, typename1, typename2);
	    ಅನ್ಯಥಾ
		semsg(_(e_variable_nr_type_mismatch_expected_str_but_got_str_in_str),
			where.wt_index, typename1, typename2, where.wt_func_name);
	    ಭಙ್ಗ;
	ಪಕ್ಷ WT_ARGUMENT:
	    ಯದಿ (where.wt_func_name == ರಿಕ್ತ)
		semsg(_(e_argument_nr_type_mismatch_expected_str_but_got_str),
			where.wt_index, typename1, typename2);
	    ಅನ್ಯಥಾ
		semsg(_(e_argument_nr_type_mismatch_expected_str_but_got_str_in_str),
			where.wt_index, typename1, typename2, where.wt_func_name);
	    ಭಙ್ಗ;
	ಪಕ್ಷ WT_CAST:
	ಪಕ್ಷ WT_UNKNOWN:
	    ಯದಿ (where.wt_func_name == ರಿಕ್ತ)
		semsg(_(e_type_mismatch_expected_str_but_got_str),
			typename1, typename2);
	    ಅನ್ಯಥಾ
		semsg(_(e_type_mismatch_expected_str_but_got_str_in_str),
			typename1, typename2, where.wt_func_name);
	    ಭಙ್ಗ;
    }

    vim_free(tofree1);
    vim_free(tofree2);
}

/*
 * Check if the expected and actual types match for a tuple
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
check_tuple_type_maybe(
    type_T	*expected,
    type_T	*actual,
    where_T	where)
{
    ಯದಿ (expected->tt_argcount == -1 || actual->tt_argcount == -1
		|| expected->tt_args == ರಿಕ್ತ || actual->tt_args == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ OK;

    // For a non-variadic tuple, the number of items must match
    ಯದಿ (!(expected->tt_flags & TTFLAG_VARARGS)
	    && expected->tt_argcount != actual->tt_argcount)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // compare the type of each tuple item
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < actual->tt_argcount; ++i)
    {
	type_T	*exp_type;
	type_T	*actual_type;

	ಯದಿ (expected->tt_flags & TTFLAG_VARARGS)
	{
	    ಯದಿ (i < expected->tt_argcount - 1)
		exp_type = expected->tt_args[i];
	    ಅನ್ಯಥಾ
		// For a variadic tuple, the last type is a List.  So use the
		// List member type.
		exp_type = expected->tt_args[expected->tt_argcount - 1]->tt_member;
	}
	ಅನ್ಯಥಾ
	    exp_type = expected->tt_args[i];

	ಯದಿ (actual->tt_flags & TTFLAG_VARARGS)
	{
	    ಯದಿ (i < actual->tt_argcount - 1)
		actual_type = actual->tt_args[i];
	    ಅನ್ಯಥಾ
		// For a variadic tuple, the last type is a List.  So use the
		// List member type.
		actual_type = actual->tt_args[actual->tt_argcount - 1]->tt_member;
	}
	ಅನ್ಯಥಾ
	    actual_type = actual->tt_args[i];

	// Allow for using "any" type for a tuple item
	ಯದಿ (actual->tt_args[i] != &t_any && check_type(exp_type, actual_type,
						FALSE, where) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Check if the expected and actual types match for a function
 * Returns OK if "expected" and "actual" are matching function types.
 * Returns FAIL if "expected" and "actual" are different types.
 * Returns MAYBE when a runtime type check is needed.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
check_func_type_maybe(
    type_T	*expected,
    type_T	*actual,
    where_T	where)
{
    ಪೂರ್ಣಾಙ್ಕ ret = OK;

    // If the return type is unknown it can be anything, including
    // nothing, thus there is no point in checking.
    ಯದಿ (expected->tt_member != &t_unknown)
    {
	ಯದಿ (actual->tt_member != ರಿಕ್ತ
		&& actual->tt_member != &t_unknown)
	{
	    where_T  func_where = where;

	    func_where.wt_kind = WT_METHOD_RETURN;
	    ret = check_type_maybe(expected->tt_member,
		    actual->tt_member, FALSE,
		    func_where);
	}
	ಅನ್ಯಥಾ
	    ret = MAYBE;
    }
    ಯದಿ (ret != FAIL
	    && ((expected->tt_flags & TTFLAG_VARARGS)
		!= (actual->tt_flags & TTFLAG_VARARGS))
	    && expected->tt_argcount != -1)
	ret = FAIL;
    ಯದಿ (ret != FAIL && expected->tt_argcount != -1
	    && actual->tt_min_argcount != -1
	    && (actual->tt_argcount == -1
		|| (actual->tt_argcount < expected->tt_min_argcount
		    || actual->tt_argcount > expected->tt_argcount)))
	ret = FAIL;
    ಯದಿ (ret != FAIL && expected->tt_args != ರಿಕ್ತ
	    && actual->tt_args != ರಿಕ್ತ)
    {
	ಪೂರ್ಣಾಙ್ಕ i;

	ಪ್ರತಿ (i = 0; i < expected->tt_argcount
		&& i < actual->tt_argcount; ++i)
	{
	    where_T  func_where = where;
	    func_where.wt_kind = WT_METHOD_ARG;

	    // Allow for using "any" argument type, lambda's have them.
	    ಯದಿ (actual->tt_args[i] != &t_any && check_type(
			expected->tt_args[i], actual->tt_args[i], FALSE,
			func_where) == FAIL)
	    {
		ret = FAIL;
		ಭಙ್ಗ;
	    }
	}
    }
    ಯದಿ (ret == OK && expected->tt_argcount >= 0
	    && actual->tt_argcount == -1)
	// check the argument count at runtime
	ret = MAYBE;

    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Check if the expected and actual types match for an object
 * Returns OK if "expected" and "actual" are matching object types.
 * Returns FAIL if "expected" and "actual" are different types.
 * Returns MAYBE when a runtime type check is needed.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
check_object_type_maybe(
    type_T	*expected,
    type_T	*actual,
    where_T	where)
{
    ಪೂರ್ಣಾಙ್ಕ ret = OK;

    ಯದಿ (actual->tt_type == VAR_ANY)
	ಪ್ರತ್ಯಾಗಮ MAYBE;	// use runtime type check
    ಯದಿ (actual->tt_type != VAR_OBJECT)
	ಪ್ರತ್ಯಾಗಮ FAIL;	// don't use tt_class
    ಯದಿ (actual->tt_class == ರಿಕ್ತ)    // null object
	ಪ್ರತ್ಯಾಗಮ OK;
    // t_object_any matches any object except for an enum item
    ಯದಿ (expected == &t_object_any && !IS_ENUM(actual->tt_class))
	ಪ್ರತ್ಯಾಗಮ OK;

    // For object method arguments, do a invariant type check in
    // an extended class.  For all others, do a covariance type check.
    ಯದಿ (where.wt_kind == WT_METHOD_ARG)
    {
	ಯದಿ (actual->tt_class != expected->tt_class)
	    ret = FAIL;
    }
    ಅನ್ಯಥಾ ಯದಿ (!class_instance_of(actual->tt_class, expected->tt_class))
    {
	// Check if this is an up-cast, if so we'll have to check the type at
	// runtime.
	ಯದಿ (where.wt_kind == WT_CAST &&
		class_instance_of(expected->tt_class, actual->tt_class))
	    ret = MAYBE;
	ಅನ್ಯಥಾ
	    ret = FAIL;
    }

    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Check if the expected and actual types match.
 * Does not allow for assigning "any" to a specific type.
 * When "argidx" > 0 it is included in the error message.
 * Return OK if types match.
 * Return FAIL if types do not match.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_type(
	type_T	*expected,
	type_T	*actual,
	ಪೂರ್ಣಾಙ್ಕ	give_ಸಂದೇಶ,
	where_T where)
{
    ಪೂರ್ಣಾಙ್ಕ ret = check_type_maybe(expected, actual, give_ಸಂದೇಶ, where);

    ಪ್ರತ್ಯಾಗಮ ret == MAYBE ? OK : ret;
}

/*
 * As check_type() but return MAYBE when a runtime type check should be used
 * when compiling.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_type_maybe(
	type_T	*expected,
	type_T	*actual,
	ಪೂರ್ಣಾಙ್ಕ	give_ಸಂದೇಶ,
	where_T where)
{
    ಪೂರ್ಣಾಙ್ಕ ret = OK;

    // When expected is "unknown" we accept any actual type.
    // When expected is "any" we accept any actual type except "void".
    ಯದಿ (expected->tt_type != VAR_UNKNOWN
	    && !(expected->tt_type == VAR_ANY && actual->tt_type != VAR_VOID))

    {
	// tt_type should match, except that a "partial" can be assigned to a
	// variable with type "func".
	// And "unknown" (using global variable) and "any" need a runtime type
	// check.
	ಯದಿ (!(expected->tt_type == actual->tt_type
		    || actual->tt_type == VAR_UNKNOWN
		    || actual->tt_type == VAR_ANY
		    || (expected->tt_type == VAR_FUNC
					   && actual->tt_type == VAR_PARTIAL)))
	{
	    ಯದಿ (expected->tt_type == VAR_BOOL
					&& (actual->tt_flags & TTFLAG_BOOL_OK))
		// Using number 0 or 1 for bool is OK.
		ಪ್ರತ್ಯಾಗಮ OK;
	    ಯದಿ (expected->tt_type == VAR_FLOAT
		    && actual->tt_type == VAR_ಸಂಖ್ಯೆ
		    && ((expected->tt_flags & TTFLAG_ಸಂಖ್ಯೆ_OK)
			     || (actual->tt_flags & TTFLAG_FLOAT_OK)))
		// Using a number where a float is expected is OK here.
		ಪ್ರತ್ಯಾಗಮ OK;
	    ಯದಿ (expected->tt_type == VAR_LIST
		    && actual->tt_type == VAR_TUPLE
		    && (expected->tt_flags & TTFLAG_TUPLE_OK))
		// Using a tuple where a list is expected is OK here.
		ಪ್ರತ್ಯಾಗಮ OK;
	    ಯದಿ (give_ಸಂದೇಶ)
		type_mismatch_where(expected, actual, where);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಯದಿ (expected->tt_type == VAR_DICT || expected->tt_type == VAR_LIST)
	{
	    // "unknown" is used for an empty list or dict
	    ಯದಿ (actual->tt_member != ರಿಕ್ತ && actual->tt_member != &t_unknown)
		ret = check_type_maybe(expected->tt_member, actual->tt_member,
								 FALSE, where);
	}
	ಅನ್ಯಥಾ ಯದಿ (expected->tt_type == VAR_TUPLE && actual != &t_any)
	    ret =  check_tuple_type_maybe(expected, actual, where);
	ಅನ್ಯಥಾ ಯದಿ (expected->tt_type == VAR_FUNC && actual != &t_any)
	    ret = check_func_type_maybe(expected, actual, where);
	ಅನ್ಯಥಾ ಯದಿ (expected->tt_type == VAR_OBJECT)
	    ret = check_object_type_maybe(expected, actual, where);

	ಯದಿ (ret == FAIL && give_ಸಂದೇಶ)
	    type_mismatch_where(expected, actual, where);
    }

    ಯದಿ (ret == OK && expected->tt_type != VAR_UNKNOWN
	    && expected->tt_type != VAR_ANY
	    && (actual->tt_type == VAR_UNKNOWN || actual->tt_type == VAR_ANY))
	// check the type at runtime
	ret = MAYBE;

    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Check that the arguments of "type" match "argvars[argcount]".
 * "base_tv" is from "expr->Func()".
 * Return OK/FAIL.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_argument_types(
	type_T	    *type,
	typval_T    *argvars,
	ಪೂರ್ಣಾಙ್ಕ	    argcount,
	typval_T    *base_tv,
	char_u	    *name)
{
    ಪೂರ್ಣಾಙ್ಕ	    varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;
    ಪೂರ್ಣಾಙ್ಕ	    i;
    ಪೂರ್ಣಾಙ್ಕ	    totcount = argcount + (base_tv == ರಿಕ್ತ ? 0 : 1);

    ಯದಿ (type->tt_type != VAR_FUNC && type->tt_type != VAR_PARTIAL)
	ಪ್ರತ್ಯಾಗಮ OK;  // just in case
    ಯದಿ (totcount < type->tt_min_argcount - varargs)
    {
	emsg_funcname(e_not_enough_arguments_for_function_str, name);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಯದಿ (!varargs && type->tt_argcount >= 0 && totcount > type->tt_argcount)
    {
	emsg_funcname(e_too_many_arguments_for_function_str, name);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಯದಿ (type->tt_args == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ OK;  // cannot check


    ಪ್ರತಿ (i = 0; i < totcount; ++i)
    {
	type_T	    *expected;
	typval_T    *tv;

	ಯದಿ (base_tv != ರಿಕ್ತ)
	{
	    ಯದಿ (i == 0)
		tv = base_tv;
	    ಅನ್ಯಥಾ
		tv = &argvars[i - 1];
	}
	ಅನ್ಯಥಾ
	    tv = &argvars[i];
	ಯದಿ (varargs && i >= type->tt_argcount - 1)
	{
	    expected = type->tt_args[type->tt_argcount - 1];
	    ಯದಿ (expected != ರಿಕ್ತ && expected->tt_type == VAR_LIST)
		expected = expected->tt_member;
	    ಯದಿ (expected == ರಿಕ್ತ)
		expected = &t_any;
	}
	ಅನ್ಯಥಾ
	    expected = type->tt_args[i];

	// check the type, unless the value is v:none
	ಯದಿ ((tv->v_type != VAR_SPECIAL || tv->vval.v_ಸಂಖ್ಯೆ != VVAL_NONE)
		   && check_typval_arg_type(expected, tv, ರಿಕ್ತ, i + 1) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

// Maximum nesting of types, to avoid running out of stack space.  E.g.
// "list<list<number>>" is nested twice.
#define MAX_TYPE_DEPTH	1000

// Current number of levels of parse_type() and the deepest level reached while
// parsing the current type.  The nesting is one less than the number of
// levels.
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	parse_depth = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	parse_max_depth = 0;

/*
 * Skip over type in list<type>, dict<type> or tuple<type>.
 * Returns a pointer to the character after the type.  "syn_error" is set to
 * TRUE on syntax error.
 */
    ಸ್ಥಿರ char_u *
skip_member_type(char_u *start, char_u *p, ಪೂರ್ಣಾಙ್ಕ *syn_error)
{
    ಯದಿ (STRNCMP("tuple", start, 5) == 0)
    {
	// handle tuple<{type1}, {type2}, ....<type>>
	p = skipwhite(p + 1);
	ಯಾವತ್ (*p != '>' && *p != NUL)
	{
	    char_u *sp = p;

	    ಯದಿ (STRNCMP(p, "...", 3) == 0)
		p += 3;
	    p = skip_type(p, TRUE);
	    ಯದಿ (p == sp)
	    {
		*syn_error = TRUE;
		ಪ್ರತ್ಯಾಗಮ p;  // syntax error
	    }
	    ಯದಿ (*p == ',')
		p = skipwhite(p + 1);
	}
	ಯದಿ (*p == '>')
	    p++;
    }
    ಅನ್ಯಥಾ
    {
	p = skipwhite(p);
	p = skip_type(skipwhite(p + 1), FALSE);
	p = skipwhite(p);
	ಯದಿ (*p == '>')
	    ++p;
    }

    ಪ್ರತ್ಯಾಗಮ p;
}

/*
 * Skip over a function type.  Returns a pointer to the character after the
 * type.  "syn_error" is set to TRUE on syntax error.
 */
    ಸ್ಥಿರ char_u *
skip_func_type(char_u *p, ಪೂರ್ಣಾಙ್ಕ *syn_error)
{
    ಯದಿ (*p == '(')
    {
	// handle func(args): type
	++p;
	ಯಾವತ್ (*p != ')' && *p != NUL)
	{
	    char_u *sp = p;

	    ಯದಿ (STRNCMP(p, "...", 3) == 0)
		p += 3;
	    p = skip_type(p, TRUE);
	    ಯದಿ (p == sp)
	    {
		*syn_error = TRUE;
		ಪ್ರತ್ಯಾಗಮ p;  // syntax error
	    }
	    ಯದಿ (*p == ',')
		p = skipwhite(p + 1);
	}
	ಯದಿ (*p == ')')
	{
	    ಯದಿ (p[1] == ':')
		p = skip_type(skipwhite(p + 2), FALSE);
	    ಅನ್ಯಥಾ
		++p;
	}
    }
    ಅನ್ಯಥಾ
    {
	// handle func: return_type
	p = skip_type(skipwhite(p + 1), FALSE);
    }

    ಪ್ರತ್ಯಾಗಮ p;
}

/*
 * Inner part of skip_type().
 */
    ಸ್ಥಿರ char_u *
skip_type_inner(char_u *start, ಪೂರ್ಣಾಙ್ಕ optional)
{
    char_u	*p = start;
    ಪೂರ್ಣಾಙ್ಕ		syn_error = FALSE;

    ಯದಿ (optional && *p == '?')
	++p;

    // Also skip over "." for imported classes: "import.ClassName".
    ಯಾವತ್ (ASCII_ISALNUM(*p) || *p == '_' || *p == '.')
	++p;

    // Skip over "<type>"; this is permissive about white space.
    ಯದಿ (*skipwhite(p) == '<')
    {
	p = skip_member_type(start, p, &syn_error);
	ಯದಿ (syn_error)
	    ಪ್ರತ್ಯಾಗಮ p;
    }
    ಅನ್ಯಥಾ ಯದಿ ((*p == '(' || (*p == ':' && VIM_ISWHITE(p[1])))
					     && STRNCMP("func", start, 4) == 0)
    {
	// skip over function type
	p = skip_func_type(p, &syn_error);
	ಯದಿ (syn_error)
	    ಪ್ರತ್ಯಾಗಮ p;
    }

    ಪ್ರತ್ಯಾಗಮ p;
}

/*
 * Skip over a type definition and return a pointer to just after it.
 * When "optional" is TRUE then a leading "?" is accepted.
 */
    char_u *
skip_type(char_u *start, ಪೂರ್ಣಾಙ್ಕ optional)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	skip_depth = 0;
    char_u	*p = start;

    // Too deeply nested: skip to the matching ">" without recursing, an
    // error is given when parsing the type.
    ಯದಿ (skip_depth > MAX_TYPE_DEPTH)
    {
	ಪೂರ್ಣಾಙ್ಕ	depth = 0;

	ಪ್ರತಿ ( ; *p != NUL; ++p)
	{
	    ಯದಿ (*p == '<')
		++depth;
	    ಅನ್ಯಥಾ ಯದಿ (*p == '>')
	    {
		ಯದಿ (depth == 0)
		    ಭಙ್ಗ;
		ಯದಿ (--depth == 0)
		    ಪ್ರತ್ಯಾಗಮ p + 1;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (depth == 0 && !ASCII_ISALNUM(*p) && *p != '_'
								&& *p != '.')
		ಭಙ್ಗ;
	}
	ಪ್ರತ್ಯಾಗಮ p;
    }

    ++skip_depth;
    p = skip_type_inner(start, optional);
    --skip_depth;
    ಪ್ರತ್ಯಾಗಮ p;
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
	ಪೂರ್ಣಾಙ್ಕ	    give_error,
	ವರ್ಣ	    *info,
	ufunc_T	    *ufunc,
	cctx_T	    *cctx)
{
    char_u  *arg_start = *arg;
    type_T  *member_type;
    ಪೂರ್ಣಾಙ್ಕ	    prev_called_emsg = called_emsg;

    ಯದಿ (**arg != '<')
    {
	ಯದಿ (give_error)
	{
	    ಯದಿ (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    ಅನ್ಯಥಾ
		semsg(_(e_missing_type_after_str), info);
	}
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    *arg = skipwhite(*arg + 1);

    member_type = parse_type(arg, type_gap, ufunc, cctx, give_error);
    ಯದಿ (member_type == ರಿಕ್ತ || !valid_declaration_type(member_type))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    *arg = skipwhite(*arg);
    ಯದಿ (**arg != '>' && called_emsg == prev_called_emsg)
    {
	ಯದಿ (give_error)
	    semsg(_(e_missing_gt_after_type_str), arg_start);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ++*arg;

    ಯದಿ (type->tt_type == VAR_LIST)
	ಪ್ರತ್ಯಾಗಮ get_list_type(member_type, type_gap);
    ಪ್ರತ್ಯಾಗಮ get_dict_type(member_type, type_gap);
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
    ಪೂರ್ಣಾಙ್ಕ		give_error,
    ufunc_T	*ufunc,
    cctx_T	*cctx)
{
    char_u  *p;
    type_T  *type;
    type_T  *ret_type = &t_unknown;
    ಪೂರ್ಣಾಙ್ಕ	argcount = -1;
    ಪೂರ್ಣಾಙ್ಕ	flags = 0;
    ಪೂರ್ಣಾಙ್ಕ	first_optional = -1;
    type_T	*arg_type[MAX_FUNC_ARGS + 1];

    // func({type}, ...{type}): {type}
    *arg += len;
    ಯದಿ (**arg == '(')
    {
	// "func" may or may not return a value, "func()" does
	// not return a value.
	ret_type = &t_void;

	p = ++*arg;
	argcount = 0;
	ಯಾವತ್ (*p != NUL && *p != ')')
	{
	    ಯದಿ (*p == '?')
	    {
		ಯದಿ (first_optional == -1)
		    first_optional = argcount;
		++p;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "...", 3) == 0)
	    {
		flags |= TTFLAG_VARARGS;
		p += 3;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (first_optional != -1)
	    {
		ಯದಿ (give_error)
		    emsg(_(e_mandatory_argument_after_optional_argument));
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    }

	    type = parse_type(&p, type_gap, ufunc, cctx, give_error);
	    ಯದಿ (type == ರಿಕ್ತ || !valid_declaration_type(type))
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    ಯದಿ ((flags & TTFLAG_VARARGS) != 0 && type->tt_type != VAR_LIST)
	    {
		ವರ್ಣ *tofree;
		semsg(_(e_variable_arguments_type_must_be_list_str),
			type_name(type, &tofree));
		vim_free(tofree);
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    }
	    arg_type[argcount++] = type;

	    // Nothing comes after "...{type}".
	    ಯದಿ (flags & TTFLAG_VARARGS)
		ಭಙ್ಗ;

	    ಯದಿ (*p != ',' && *skipwhite(p) == ',')
	    {
		ಯದಿ (give_error)
		    semsg(_(e_no_white_space_allowed_before_str_str),
			    ",", p);
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    }
	    ಯದಿ (*p == ',')
	    {
		++p;
		ಯದಿ (!VIM_ISWHITE(*p))
		{
		    ಯದಿ (give_error)
			semsg(_(e_white_space_required_after_str_str),
				",", p - 1);
		    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		}
	    }
	    p = skipwhite(p);
	    ಯದಿ (argcount == MAX_FUNC_ARGS)
	    {
		ಯದಿ (give_error)
		    emsg(_(e_too_many_argument_types));
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    }
	}

	p = skipwhite(p);
	ಯದಿ (*p != ')')
	{
	    ಯದಿ (give_error)
		emsg(_(e_missing_closing_paren));
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
	*arg = p + 1;
    }
    ಯದಿ (**arg == ':')
    {
	// parse return type
	++*arg;
	ಯದಿ (!VIM_ISWHITE(**arg) && give_error)
	    semsg(_(e_white_space_required_after_str_str), ":", *arg - 1);
	*arg = skipwhite(*arg);
	ret_type = parse_type(arg, type_gap, ufunc, cctx, give_error);
	ಯದಿ (ret_type == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಯದಿ (flags == 0 && first_optional == -1 && argcount <= 0)
	type = get_func_type(ret_type, argcount, type_gap);
    ಅನ್ಯಥಾ
    {
	type = alloc_func_type(ret_type, argcount, type_gap);
	type->tt_flags = flags;
	ಯದಿ (argcount > 0)
	{
	    type->tt_argcount = argcount;
	    type->tt_min_argcount = first_optional == -1
						? argcount : first_optional;
	    ಯದಿ (func_type_add_arg_types(type, argcount, type_gap) == FAIL)
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    mch_memmove(type->tt_args, arg_type, ಪರಿಮಾಣ(type_T *) * argcount);
	}
    }

    ಪ್ರತ್ಯಾಗಮ type;
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
    ಪೂರ್ಣಾಙ್ಕ		give_error,
    ufunc_T	*ufunc,
    cctx_T	*cctx)
{
    char_u	*p;
    type_T	*type;
    type_T	*ret_type = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		typecount = -1;
    ಪೂರ್ಣಾಙ್ಕ		flags = 0;
    garray_T	tuple_types_ga;

    ga_init2(&tuple_types_ga, ಪರಿಮಾಣ(type_T *), 10);

    // tuple<{type}, {type}>
    // tuple<{type}, ...{type}>
    ಯದಿ (**arg != '<')
    {
	ಯದಿ (give_error)
	{
	    ಯದಿ (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    ಅನ್ಯಥಾ
		semsg(_(e_missing_type_after_str), "tuple");
	}

	// only "tuple" is specified
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    p = ++*arg;
    typecount = 0;
    ಯಾವತ್ (*p != NUL && *p != '>')
    {
	ಯದಿ (STRNCMP(p, "...", 3) == 0)
	{
	    flags |= TTFLAG_VARARGS;
	    p += 3;
	}

	type = parse_type(&p, type_gap, ufunc, cctx, give_error);
	ಯದಿ (type == ರಿಕ್ತ || !valid_declaration_type(type))
	    ಗಚ್ಛ on_err;

	ಯದಿ ((flags & TTFLAG_VARARGS) != 0 && type->tt_type != VAR_LIST)
	{
	    ವರ್ಣ *tofree;
	    semsg(_(e_variadic_tuple_must_end_with_list_type_str),
						type_name(type, &tofree));
	    vim_free(tofree);
	    ಗಚ್ಛ on_err;
	}

	// Add the item type
	ಯದಿ (ga_grow(&tuple_types_ga, 1) == FAIL)
	    ಗಚ್ಛ on_err;
	((type_T **)tuple_types_ga.ga_data)[tuple_types_ga.ga_len] = type;
	tuple_types_ga.ga_len++;
	typecount++;

	// Nothing comes after "...{type}".
	ಯದಿ (flags & TTFLAG_VARARGS)
	    ಭಙ್ಗ;

	ಯದಿ (*p != ',' && *skipwhite(p) == ',')
	{
	    ಯದಿ (give_error)
		semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
	    ಗಚ್ಛ on_err;
	}
	ಯದಿ (*p == ',')
	{
	    ++p;
	    ಯದಿ (!VIM_ISWHITE(*p))
	    {
		ಯದಿ (give_error)
		    semsg(_(e_white_space_required_after_str_str),
			    ",", p - 1);
		ಗಚ್ಛ on_err;
	    }
	}
	p = skipwhite(p);
    }

    p = skipwhite(p);
    ಯದಿ (*p != '>' || typecount <= 0)
    {
	ಯದಿ (give_error)
	    semsg(_(e_missing_type_after_str), p);
	ಗಚ್ಛ on_err;
    }
    *arg = p + 1;

    ret_type = alloc_tuple_type(typecount, type_gap);
    ಯದಿ (ret_type == &t_any
	    || tuple_type_add_types(ret_type, typecount, type_gap) == FAIL)
    {
	ret_type = ರಿಕ್ತ;
	ಗಚ್ಛ on_err;
    }
    ret_type->tt_flags = flags;
    ret_type->tt_argcount = typecount;
    mch_memmove(ret_type->tt_args, tuple_types_ga.ga_data,
						ಪರಿಮಾಣ(type_T *) * typecount);

on_err:
    ga_clear(&tuple_types_ga);

    ಪ್ರತ್ಯಾಗಮ ret_type;
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
    ಪೂರ್ಣಾಙ್ಕ		give_error,
    cctx_T	*cctx)
{
    char_u	*arg_start = *arg;
    type_T	*object_type;
    ಪೂರ್ಣಾಙ್ಕ		prev_called_emsg = called_emsg;

    // object<X> or object<any>
    ಯದಿ (**arg != '<')
    {
	ಯದಿ (give_error)
	{
	    ಯದಿ (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    ಅನ್ಯಥಾ
		semsg(_(e_missing_type_after_str), "object");
	}

	// only "object" is specified
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    // skip spaces following "object<"
    *arg = skipwhite(*arg + 1);

    object_type = parse_type(arg, type_gap, ರಿಕ್ತ, cctx, give_error);
    ಯದಿ (object_type == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    *arg = skipwhite(*arg);
    ಯದಿ (**arg != '>' && called_emsg == prev_called_emsg)
    {
	ಯದಿ (give_error)
	    semsg(_(e_missing_gt_after_type_str), arg_start);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ++*arg;

    ಯದಿ (object_type->tt_type == VAR_ANY)
	ಪ್ರತ್ಯಾಗಮ &t_object_any;

    ಯದಿ (object_type->tt_type != VAR_OBJECT)
    {
	// specified type is not a class
	ಯದಿ (give_error)
	    semsg(_(e_class_name_not_found_str), arg_start);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಪ್ರತ್ಯಾಗಮ object_type;
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
    ಪೂರ್ಣಾಙ್ಕ		give_error,
    ufunc_T	*ufunc,
    cctx_T	*cctx)
{
    ಪೂರ್ಣಾಙ್ಕ		did_emsg_before = did_emsg;
    typval_T	tv;

    tv.v_type = VAR_UNKNOWN;
    ಯದಿ (eval_variable_import(*arg, &tv) == OK)
    {
	ಯದಿ (tv.v_type == VAR_CLASS && tv.vval.v_class != ರಿಕ್ತ)
	{
	    type_T *type = get_type_ptr(type_gap);
	    ಯದಿ (type != ರಿಕ್ತ)
	    {
		// Although the name is that of a class or interface, the type
		// uses will be an object.
		type->tt_type = VAR_OBJECT;
		type->tt_class = tv.vval.v_class;
		clear_tv(&tv);

		*arg += len;
		// Skip over ".ClassName".
		ಯಾವತ್ (ASCII_ISALNUM(**arg) || **arg == '_' || **arg == '.')
		    ++*arg;

		ಪ್ರತ್ಯಾಗಮ type;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (tv.v_type == VAR_TYPEALIAS)
	{
	    // user defined type
	    // The type alias may itself be a nested type, check the resulting
	    // nesting depth.
	    ಪೂರ್ಣಾಙ್ಕ	depth = parse_depth - 1
				+ MAX(tv.vval.v_typealias->ta_depth, 1);
	    ಯದಿ (depth - 1 > MAX_TYPE_DEPTH)
	    {
		clear_tv(&tv);
		ಯದಿ (give_error)
		    emsg(_(e_type_nested_too_deep));
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    }
	    ಯದಿ (depth > parse_max_depth)
		parse_max_depth = depth;

	    type_T *type = copy_type(tv.vval.v_typealias->ta_type, type_gap);
	    *arg += len;
	    clear_tv(&tv);
	    // Skip over ".TypeName".
	    ಯಾವತ್ (ASCII_ISALNUM(**arg) || **arg == '_' || **arg == '.')
		++*arg;
	    ಪ್ರತ್ಯಾಗಮ type;
	}

	clear_tv(&tv);
    }

    // Check whether it is a generic type
    type_T *type = find_generic_type(*arg, len, ufunc, cctx);
    ಯದಿ (type != ರಿಕ್ತ)
    {
	*arg += len;
	ಪ್ರತ್ಯಾಗಮ type;
    }

    ಯದಿ (give_error && (did_emsg == did_emsg_before))
    {
	char_u	*p = skip_type(*arg, FALSE);
	ವರ್ಣ	cc = *p;

	*p = NUL;
	semsg(_(e_type_not_recognized_str), *arg);
	*p = cc;
    }

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
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
    ಪೂರ್ಣಾಙ್ಕ		give_error)
{
    char_u  *p = *arg;
    size_t  len;

    // Skip over the first word.
    ಯಾವತ್ (ASCII_ISALNUM(*p) || *p == '_')
	++p;
    len = p - *arg;

    ವಿಕಲ್ಪ (**arg)
    {
	ಪಕ್ಷ 'a':
	    ಯದಿ (len == 3 && STRNCMP(*arg, "any", len) == 0)
	    {
		*arg += len;
		ಪ್ರತ್ಯಾಗಮ &t_any;
	    }
	    ಭಙ್ಗ;
	ಪಕ್ಷ 'b':
	    ಯದಿ (len == 4 && STRNCMP(*arg, "bool", len) == 0)
	    {
		*arg += len;
		ಪ್ರತ್ಯಾಗಮ &t_bool;
	    }
	    ಯದಿ (len == 4 && STRNCMP(*arg, "blob", len) == 0)
	    {
		*arg += len;
		ಪ್ರತ್ಯಾಗಮ &t_blob;
	    }
	    ಭಙ್ಗ;
	ಪಕ್ಷ 'c':
	    ಯದಿ (len == 7 && STRNCMP(*arg, "channel", len) == 0)
	    {
		*arg += len;
		ಪ್ರತ್ಯಾಗಮ &t_channel;
	    }
	    ಭಙ್ಗ;
	ಪಕ್ಷ 'd':
	    ಯದಿ (len == 4 && STRNCMP(*arg, "dict", len) == 0)
	    {
		*arg += len;
		ಪ್ರತ್ಯಾಗಮ parse_type_member(arg, &t_dict_any, type_gap,
						give_error, "dict", ufunc,
						cctx);
	    }
	    ಭಙ್ಗ;
	ಪಕ್ಷ 'f':
	    ಯದಿ (len == 5 && STRNCMP(*arg, "float", len) == 0)
	    {
		*arg += len;
		ಪ್ರತ್ಯಾಗಮ &t_float;
	    }
	    ಯದಿ (len == 4 && STRNCMP(*arg, "func", len) == 0)
		ಪ್ರತ್ಯಾಗಮ parse_type_func(arg, len, type_gap, give_error, ufunc,
									cctx);
	    ಭಙ್ಗ;
	ಪಕ್ಷ 'j':
	    ಯದಿ (len == 3 && STRNCMP(*arg, "job", len) == 0)
	    {
		*arg += len;
		ಪ್ರತ್ಯಾಗಮ &t_job;
	    }
	    ಭಙ್ಗ;
	ಪಕ್ಷ 'l':
	    ಯದಿ (len == 4 && STRNCMP(*arg, "list", len) == 0)
	    {
		*arg += len;
		ಪ್ರತ್ಯಾಗಮ parse_type_member(arg, &t_list_any, type_gap,
						give_error, "list", ufunc,
						cctx);
	    }
	    ಭಙ್ಗ;
	ಪಕ್ಷ 'n':
	    ಯದಿ (len == 6 && STRNCMP(*arg, "number", len) == 0)
	    {
		*arg += len;
		ಪ್ರತ್ಯಾಗಮ &t_ಸಂಖ್ಯೆ;
	    }
	    ಭಙ್ಗ;
	ಪಕ್ಷ 'o':
	    ಯದಿ (len == 6 && STRNCMP(*arg, "object", len) == 0)
	    {
		*arg += len;
		ಪ್ರತ್ಯಾಗಮ parse_type_object(arg, type_gap, give_error, cctx);
	    }
	    ಭಙ್ಗ;
	ಪಕ್ಷ 's':
	    ಯದಿ (len == 6 && STRNCMP(*arg, "string", len) == 0)
	    {
		*arg += len;
		ಪ್ರತ್ಯಾಗಮ &t_string;
	    }
	    ಭಙ್ಗ;
	ಪಕ್ಷ 't':
	    ಯದಿ (len == 5 && STRNCMP(*arg, "tuple", len) == 0)
	    {
		*arg += len;
		ಪ್ರತ್ಯಾಗಮ parse_type_tuple(arg, type_gap, give_error, ufunc,
									cctx);
	    }
	    ಭಙ್ಗ;
	ಪಕ್ಷ 'v':
	    ಯದಿ (len == 4 && STRNCMP(*arg, "void", len) == 0)
	    {
		*arg += len;
		ಪ್ರತ್ಯಾಗಮ &t_void;
	    }
	    ಭಙ್ಗ;
    }

    // User defined type
    ಪ್ರತ್ಯಾಗಮ parse_type_user_defined(arg, len, type_gap, give_error, ufunc,
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
    ಪೂರ್ಣಾಙ್ಕ		give_error)
{
    type_T	*type;

    ಯದಿ (parse_depth > MAX_TYPE_DEPTH)
    {
	ಯದಿ (give_error)
	    emsg(_(e_type_nested_too_deep));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಯದಿ (parse_depth == 0)
	parse_max_depth = 0;
    ++parse_depth;
    ಯದಿ (parse_depth > parse_max_depth)
	parse_max_depth = parse_depth;
    type = parse_type_inner(arg, type_gap, ufunc, cctx, give_error);
    --parse_depth;
    ಪ್ರತ್ಯಾಗಮ type;
}

/*
 * Return the number of levels of the type last parsed by parse_type().
 */
    ಪೂರ್ಣಾಙ್ಕ
parse_type_depth(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ parse_max_depth;
}

/*
 * Check if "type1" and "type2" are exactly the same.
 * "flags" can have ETYPE_ARG_UNKNOWN, which means that an unknown argument
 * type in "type1" is accepted.
 */
    ಪೂರ್ಣಾಙ್ಕ
equal_type(type_T *type1, type_T *type2, ಪೂರ್ಣಾಙ್ಕ flags)
{
    ಪೂರ್ಣಾಙ್ಕ i;

    ಯದಿ (type1 == ರಿಕ್ತ || type2 == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಯದಿ (type1->tt_type != type2->tt_type)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ವಿಕಲ್ಪ (type1->tt_type)
    {
	ಪಕ್ಷ VAR_UNKNOWN:
	ಪಕ್ಷ VAR_ANY:
	ಪಕ್ಷ VAR_VOID:
	ಪಕ್ಷ VAR_SPECIAL:
	ಪಕ್ಷ VAR_BOOL:
	ಪಕ್ಷ VAR_ಸಂಖ್ಯೆ:
	ಪಕ್ಷ VAR_FLOAT:
	ಪಕ್ಷ VAR_STRING:
	ಪಕ್ಷ VAR_BLOB:
	ಪಕ್ಷ VAR_JOB:
	ಪಕ್ಷ VAR_CHANNEL:
	ಪಕ್ಷ VAR_INSTR:
	ಪಕ್ಷ VAR_TYPEALIAS:
	    ಭಙ್ಗ;  // not composite is always OK
	ಪಕ್ಷ VAR_OBJECT:
	ಪಕ್ಷ VAR_CLASS:
	    // Objects are considered equal if they are from the same class
	    ಪ್ರತ್ಯಾಗಮ type1->tt_class == type2->tt_class;
	ಪಕ್ಷ VAR_LIST:
	ಪಕ್ಷ VAR_DICT:
	    ಪ್ರತ್ಯಾಗಮ equal_type(type1->tt_member, type2->tt_member, flags);
	ಪಕ್ಷ VAR_TUPLE:
	    ಯದಿ (type1->tt_argcount != type2->tt_argcount)
		ಪ್ರತ್ಯಾಗಮ FALSE;
	    ಯದಿ (type1->tt_argcount < 0
			   || type1->tt_args == ರಿಕ್ತ || type2->tt_args == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ TRUE;
	    ಪ್ರತಿ (i = 0; i < type1->tt_argcount; ++i)
		ಯದಿ ((flags & ETYPE_ARG_UNKNOWN) == 0
			&& !equal_type(type1->tt_args[i], type2->tt_args[i],
									flags))
		    ಪ್ರತ್ಯಾಗಮ FALSE;
	    ಪ್ರತ್ಯಾಗಮ TRUE;
	ಪಕ್ಷ VAR_FUNC:
	ಪಕ್ಷ VAR_PARTIAL:
	    ಯದಿ (!equal_type(type1->tt_member, type2->tt_member, flags)
		    || type1->tt_argcount != type2->tt_argcount)
		ಪ್ರತ್ಯಾಗಮ FALSE;
	    ಯದಿ (type1->tt_argcount < 0
			   || type1->tt_args == ರಿಕ್ತ || type2->tt_args == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ TRUE;
	    ಪ್ರತಿ (i = 0; i < type1->tt_argcount; ++i)
		ಯದಿ ((flags & ETYPE_ARG_UNKNOWN) == 0
			&& !equal_type(type1->tt_args[i], type2->tt_args[i],
									flags))
		    ಪ್ರತ್ಯಾಗಮ FALSE;
	    ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ TRUE;
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
    ಯದಿ (type1 == &t_func_unknown)
    {
	*dest = type2;
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (type2 == &t_func_unknown)
    {
	*dest = type1;
	ಪ್ರತ್ಯಾಗಮ;
    }

    common_type(type1->tt_member, type2->tt_member, &common, type_gap);
    ಯದಿ (type1->tt_argcount == type2->tt_argcount
	    && type1->tt_argcount >= 0)
    {
	ಪೂರ್ಣಾಙ್ಕ argcount = type1->tt_argcount;
	ಪೂರ್ಣಾಙ್ಕ i;

	*dest = alloc_func_type(common, argcount, type_gap);
	ಯದಿ (type1->tt_args != ರಿಕ್ತ && type2->tt_args != ರಿಕ್ತ)
	{
	    ಯದಿ (func_type_add_arg_types(*dest, argcount,
			type_gap) == OK)
		ಪ್ರತಿ (i = 0; i < argcount; ++i)
		    common_type(type1->tt_args[i], type2->tt_args[i],
			    &(*dest)->tt_args[i], type_gap);
	}
    }
    ಅನ್ಯಥಾ
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
    ಯದಿ (equal_type(type1, type2, 0))
    {
	*dest = type1;
	ಪ್ರತ್ಯಾಗಮ;
    }

    // If either is VAR_UNKNOWN use the other type.  An empty list/dict has no
    // specific type.
    ಯದಿ (type1 == ರಿಕ್ತ || type1->tt_type == VAR_UNKNOWN)
    {
	*dest = type2;
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (type2 == ರಿಕ್ತ || type2->tt_type == VAR_UNKNOWN)
    {
	*dest = type1;
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (type1->tt_type == type2->tt_type)
    {
	ಯದಿ (type1->tt_type == VAR_LIST
		|| type1->tt_type == VAR_DICT)
	{
	    type_T *common;

	    common_type(type1->tt_member, type2->tt_member, &common, type_gap);
	    ಯದಿ (type1->tt_type == VAR_LIST)
		*dest = get_list_type(common, type_gap);
	    ಅನ್ಯಥಾ
		*dest = get_dict_type(common, type_gap);
	    ಪ್ರತ್ಯಾಗಮ;
	}
	ಅನ್ಯಥಾ ಯದಿ (type1->tt_type == VAR_FUNC)
	{
	    common_type_var_func(type1, type2, dest, type_gap);
	    ಪ್ರತ್ಯಾಗಮ;
	}
	ಅನ್ಯಥಾ ಯದಿ (type1->tt_type == VAR_OBJECT)
	{
	    *dest = &t_object_any;
	    ಪ್ರತ್ಯಾಗಮ;
	}
    }

    *dest = &t_any;
}

/*
 * Return the item type of a List, Dict or a Tuple
 */
    type_T *
get_item_type(type_T *type)
{
    ಯದಿ (type->tt_type == VAR_TUPLE)
    {
	ಯದಿ (type->tt_argcount != 1)
	    ಪ್ರತ್ಯಾಗಮ &t_any;

	ಯದಿ (type->tt_flags & TTFLAG_VARARGS)
	    ಪ್ರತ್ಯಾಗಮ type->tt_args[0]->tt_member;
	ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ type->tt_args[0];
    }

    // For "any" the item type is not known.
    ಪ್ರತ್ಯಾಗಮ type->tt_member == ರಿಕ್ತ ? &t_any : type->tt_member;
}

/*
 * Push an entry onto the type stack.  "type" used both for the current type
 * and the declared type.
 * Returns FAIL when out of memory.
 */
    ಪೂರ್ಣಾಙ್ಕ
ತಳ್ಳು_type_ರಾಶಿ(cctx_T *cctx, type_T *type)
{
    ಪ್ರತ್ಯಾಗಮ ತಳ್ಳು_type_stack2(cctx, type, type);
}

/*
 * Push an entry onto the type stack.  "type" is the current type, "decl_type"
 * is the declared type.
 * Returns FAIL when out of memory.
 */
    ಪೂರ್ಣಾಙ್ಕ
ತಳ್ಳು_type_stack2(cctx_T *cctx, type_T *type, type_T *decl_type)
{
    garray_T	*ರಾಶಿ = &cctx->ctx_type_ರಾಶಿ;
    type2_T	*typep;

    ಯದಿ (GA_GROW_FAILS(ರಾಶಿ, 1))
	ಪ್ರತ್ಯಾಗಮ FAIL;
    typep = ((type2_T *)ರಾಶಿ->ga_data) + ರಾಶಿ->ga_len;
    typep->type_curr = type;
    typep->type_decl = decl_type;
    ++ರಾಶಿ->ga_len;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Set the type of the top of the stack to "type".
 */
    ಶೂನ್ಯ
set_type_on_ರಾಶಿ(cctx_T *cctx, type_T *type, ಪೂರ್ಣಾಙ್ಕ offset)
{
    garray_T	*ರಾಶಿ = &cctx->ctx_type_ರಾಶಿ;
    type2_T	*typep = ((type2_T *)ರಾಶಿ->ga_data)
						  + ರಾಶಿ->ga_len - 1 - offset;

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
get_type_on_ರಾಶಿ(cctx_T *cctx, ಪೂರ್ಣಾಙ್ಕ offset)
{
    garray_T	*ರಾಶಿ = &cctx->ctx_type_ರಾಶಿ;

    ಯದಿ (offset + 1 > ರಾಶಿ->ga_len)
	ಪ್ರತ್ಯಾಗಮ &t_unknown;
    ಪ್ರತ್ಯಾಗಮ (((type2_T *)ರಾಶಿ->ga_data) + ರಾಶಿ->ga_len - offset - 1)
								   ->type_curr;
}

/*
 * Get the declared type from the type stack.  If "offset" is zero the one at
 * the top,
 * if "offset" is one the type above that, etc.
 * Returns &t_unknown if there is no such stack entry.
 */
    type_T *
get_decl_type_on_ರಾಶಿ(cctx_T *cctx, ಪೂರ್ಣಾಙ್ಕ offset)
{
    garray_T	*ರಾಶಿ = &cctx->ctx_type_ರಾಶಿ;

    ಯದಿ (offset + 1 > ರಾಶಿ->ga_len)
	ಪ್ರತ್ಯಾಗಮ &t_unknown;
    ಪ್ರತ್ಯಾಗಮ (((type2_T *)ರಾಶಿ->ga_data) + ರಾಶಿ->ga_len - offset - 1)
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
	ಪೂರ್ಣಾಙ್ಕ	    ಎಣಿಕೆ,
	ಪೂರ್ಣಾಙ್ಕ	    skip,
	cctx_T	    *cctx)
{
    garray_T	*ರಾಶಿ = &cctx->ctx_type_ರಾಶಿ;
    type2_T	*typep;
    garray_T    *type_gap = cctx->ctx_type_list;
    ಪೂರ್ಣಾಙ್ಕ		i;
    type_T	*result;
    type_T	*type;

    // Use "unknown" for an empty list or dict.
    ಯದಿ (ಎಣಿಕೆ == 0)
	ಪ್ರತ್ಯಾಗಮ &t_unknown;
    // Find the common type from following items.
    typep = ((type2_T *)ರಾಶಿ->ga_data) + ರಾಶಿ->ga_len;
    result = &t_unknown;
    ಪ್ರತಿ (i = 0; i < ಎಣಿಕೆ; ++i)
    {
	type = (typep -((ಎಣಿಕೆ - i) * skip) + skip - 1)->type_curr;
	ಯದಿ (check_type_is_ಮೌಲ್ಯ(type) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	ಯದಿ (result != &t_any)
	    common_type(type, result, &result, type_gap);
    }

    ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * Get the types of items in a tuple on the stack of "cctx".
 * Returns the number of types.  Returns -1 on failure.
 */
    ಪೂರ್ಣಾಙ್ಕ
get_tuple_type_from_ರಾಶಿ(
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ,
    garray_T	*tuple_types_gap,
    cctx_T	*cctx)
{
    garray_T	*ರಾಶಿ = &cctx->ctx_type_ರಾಶಿ;
    type2_T	*typep;
    type_T	*type = ರಿಕ್ತ;

    // Use "unknown" for an empty tuple
    ಯದಿ (ಎಣಿಕೆ == 0)
	ಪ್ರತ್ಯಾಗಮ 0;

    // Find the common type from following items.
    typep = ((type2_T *)ರಾಶಿ->ga_data) + ರಾಶಿ->ga_len;
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < ಎಣಿಕೆ; i++)
    {
	type = (typep - (ಎಣಿಕೆ - i))->type_curr;
	ಯದಿ (check_type_is_ಮೌಲ್ಯ(type) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ -1;
	ಯದಿ (ga_grow(tuple_types_gap, 1) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ -1;
	((type_T **)tuple_types_gap->ga_data)[tuple_types_gap->ga_len] = type;
	tuple_types_gap->ga_len++;
    }

    ಪ್ರತ್ಯಾಗಮ tuple_types_gap->ga_len;
}

    ವರ್ಣ *
vartype_name(vartype_T type)
{
    ವಿಕಲ್ಪ (type)
    {
	ಪಕ್ಷ VAR_UNKNOWN: ಭಙ್ಗ;
	ಪಕ್ಷ VAR_ANY: ಪ್ರತ್ಯಾಗಮ "any";
	ಪಕ್ಷ VAR_VOID: ಪ್ರತ್ಯಾಗಮ "void";
	ಪಕ್ಷ VAR_SPECIAL: ಪ್ರತ್ಯಾಗಮ "special";
	ಪಕ್ಷ VAR_BOOL: ಪ್ರತ್ಯಾಗಮ "bool";
	ಪಕ್ಷ VAR_ಸಂಖ್ಯೆ: ಪ್ರತ್ಯಾಗಮ "number";
	ಪಕ್ಷ VAR_FLOAT: ಪ್ರತ್ಯಾಗಮ "float";
	ಪಕ್ಷ VAR_STRING: ಪ್ರತ್ಯಾಗಮ "string";
	ಪಕ್ಷ VAR_BLOB: ಪ್ರತ್ಯಾಗಮ "blob";
	ಪಕ್ಷ VAR_JOB: ಪ್ರತ್ಯಾಗಮ "job";
	ಪಕ್ಷ VAR_CHANNEL: ಪ್ರತ್ಯಾಗಮ "channel";
	ಪಕ್ಷ VAR_LIST: ಪ್ರತ್ಯಾಗಮ "list";
	ಪಕ್ಷ VAR_TUPLE: ಪ್ರತ್ಯಾಗಮ "tuple";
	ಪಕ್ಷ VAR_DICT: ಪ್ರತ್ಯಾಗಮ "dict";
	ಪಕ್ಷ VAR_INSTR: ಪ್ರತ್ಯಾಗಮ "instr";
	ಪಕ್ಷ VAR_CLASS: ಪ್ರತ್ಯಾಗಮ "class";
	ಪಕ್ಷ VAR_OBJECT: ಪ್ರತ್ಯಾಗಮ "object";
	ಪಕ್ಷ VAR_TYPEALIAS: ಪ್ರತ್ಯಾಗಮ "typealias";

	ಪಕ್ಷ VAR_FUNC:
	ಪಕ್ಷ VAR_PARTIAL: ಪ್ರತ್ಯಾಗಮ "func";
    }
    ಪ್ರತ್ಯಾಗಮ "unknown";
}

/*
 * Return the type name of a List (list<type>) or Dict (dict<type>).
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    ಸ್ಥಿರ ವರ್ಣ *
type_name_list_or_dict(ವರ್ಣ *name, type_T *type, ವರ್ಣ **tofree)
{
    ವರ್ಣ *member_free;
    ವರ್ಣ *member_name;

    ಯದಿ (type->tt_member->tt_type == VAR_UNKNOWN)
	member_name = type_name(&t_any, &member_free);
    ಅನ್ಯಥಾ
	member_name = type_name(type->tt_member, &member_free);

    size_t len = STRLEN(name) + STRLEN(member_name) + 3;
    *tofree = alloc(len);
    ಯದಿ (*tofree == ರಿಕ್ತ)
    {
	vim_free(member_free);
	ಪ್ರತ್ಯಾಗಮ name;
    }

    vim_snprintf(*tofree, len, "%s<%s>", name, member_name);
    vim_free(member_free);
    ಪ್ರತ್ಯಾಗಮ *tofree;
}

/*
 * Return the type name of a tuple.
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    ಸ್ಥಿರ ವರ್ಣ *
type_name_tuple(type_T *type, ವರ್ಣ **tofree)
{
    garray_T    ga;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;
    ವರ್ಣ	*arg_free = ರಿಕ್ತ;

    ga_init2(&ga, 1, 100);
    ಯದಿ (ga_grow(&ga, 20) == FAIL)
	ಗಚ್ಛ failed;
    STRCPY(ga.ga_data, "tuple<");
    ga.ga_len += 6;

    ಯದಿ (type->tt_argcount <= 0)
	// empty tuple
	GA_CONCAT_LITERAL(&ga, "any");
    ಅನ್ಯಥಾ
    {
	ಯದಿ (type->tt_args == ರಿಕ್ತ)
	    GA_CONCAT_LITERAL(&ga, "[unknown]");
	ಅನ್ಯಥಾ
	{
	    ಪ್ರತಿ (i = 0; i < type->tt_argcount; ++i)
	    {
		string_T    arg_type;

		arg_type.string = (char_u *)type_name(type->tt_args[i], &arg_free);
		ಯದಿ (i > 0)
		{
		    STRCPY((ವರ್ಣ *)ga.ga_data + ga.ga_len, ", ");
		    ga.ga_len += 2;
		}
		arg_type.length = STRLEN(arg_type.string);
		ಯದಿ (ga_grow(&ga, (ಪೂರ್ಣಾಙ್ಕ)arg_type.length + 8) == FAIL)
		    ಗಚ್ಛ failed;
		ಯದಿ (varargs && i == type->tt_argcount - 1)
		    GA_CONCAT_LITERAL(&ga, "...");
		ga_concat_len(&ga, arg_type.string, arg_type.length);
		VIM_CLEAR(arg_free);
	    }
	}
    }

    STRCPY((ವರ್ಣ *)ga.ga_data + ga.ga_len, ">");
    *tofree = ga.ga_data;
    ಪ್ರತ್ಯಾಗಮ ga.ga_data;

failed:
    vim_free(arg_free);
    ga_clear(&ga);
    ಪ್ರತ್ಯಾಗಮ "[unknown]";
}

/*
 * Return the type name of a Class (class<name>) or Object (object<name>).
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    ಸ್ಥಿರ ವರ್ಣ *
type_name_class_or_obj(ವರ್ಣ *name, type_T *type, ವರ್ಣ **tofree)
{
    string_T	class_name;

    ಯದಿ (type->tt_class != ರಿಕ್ತ)
    {
	class_name.string = type->tt_class->class_name.string;
	class_name.length = type->tt_class->class_name.length;
	ಯದಿ (IS_ENUM(type->tt_class))
	    name = "enum";
    }
    ಅನ್ಯಥಾ
	STR_LITERAL_SET(class_name, "any");

    size_t len = STRLEN(name) + class_name.length + 3;
    *tofree = alloc(len);
    ಯದಿ (*tofree == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ name;

    vim_snprintf(*tofree, len, "%s<%s>", name, class_name.string);
    ಪ್ರತ್ಯಾಗಮ *tofree;
}

/*
 * Return the type name of a function.
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    ಸ್ಥಿರ ವರ್ಣ *
type_name_func(type_T *type, ವರ್ಣ **tofree)
{
    garray_T    ga;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;
    ವರ್ಣ	*arg_free = ರಿಕ್ತ;

    ga_init2(&ga, 1, 100);
    ಯದಿ (ga_grow(&ga, 20) == FAIL)
	ಗಚ್ಛ failed;
    STRCPY(ga.ga_data, "func(");
    ga.ga_len += 5;

    ಪ್ರತಿ (i = 0; i < type->tt_argcount; ++i)
    {
	string_T    arg_type;

	ಯದಿ (type->tt_args == ರಿಕ್ತ)
	    STR_LITERAL_SET(arg_type, "[unknown]");
	ಅನ್ಯಥಾ
	{
	    arg_type.string = (char_u *)type_name(type->tt_args[i], &arg_free);
	    arg_type.length = STRLEN(arg_type.string);
	}
	ಯದಿ (i > 0)
	{
	    STRCPY((ವರ್ಣ *)ga.ga_data + ga.ga_len, ", ");
	    ga.ga_len += 2;
	}
	ಯದಿ (ga_grow(&ga, (ಪೂರ್ಣಾಙ್ಕ)arg_type.length + 8) == FAIL)
	    ಗಚ್ಛ failed;
	ಯದಿ (varargs && i == type->tt_argcount - 1)
	    GA_CONCAT_LITERAL(&ga, "...");
	ಅನ್ಯಥಾ ಯದಿ (i >= type->tt_min_argcount)
	    *((ವರ್ಣ *)ga.ga_data + ga.ga_len++) = '?';
	ga_concat_len(&ga, arg_type.string, arg_type.length);
	VIM_CLEAR(arg_free);
    }
    ಯದಿ (type->tt_argcount < 0)
	// any number of arguments
	GA_CONCAT_LITERAL(&ga, "...");

    ಯದಿ (type->tt_member == &t_void)
	STRCPY((ವರ್ಣ *)ga.ga_data + ga.ga_len, ")");
    ಅನ್ಯಥಾ
    {
	ವರ್ಣ *ret_free = ರಿಕ್ತ;
	ವರ್ಣ *ret_name = type_name(type->tt_member, &ret_free);
	ಪೂರ್ಣಾಙ್ಕ  len;

	len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(ret_name) + 4;
	ಯದಿ (ga_grow(&ga, len) == FAIL)
	{
	    vim_free(ret_free);
	    ಗಚ್ಛ failed;
	}
	STRCPY((ವರ್ಣ *)ga.ga_data + ga.ga_len, "): ");
	STRCPY((ವರ್ಣ *)ga.ga_data + ga.ga_len + 3, ret_name);
	vim_free(ret_free);
    }
    *tofree = ga.ga_data;
    ಪ್ರತ್ಯಾಗಮ ga.ga_data;

failed:
    vim_free(arg_free);
    ga_clear(&ga);
    ಪ್ರತ್ಯಾಗಮ "[unknown]";
}

/*
 * Return the name of a type.
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    ವರ್ಣ *
type_name(type_T *type, ವರ್ಣ **tofree)
{
    ವರ್ಣ *name;

    *tofree = ರಿಕ್ತ;
    ಯದಿ (type == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ "[unknown]";
    name = vartype_name(type->tt_type);

    ವಿಕಲ್ಪ (type->tt_type)
    {
	ಪಕ್ಷ VAR_LIST:
	ಪಕ್ಷ VAR_DICT:
	    ಪ್ರತ್ಯಾಗಮ type_name_list_or_dict(name, type, tofree);

	ಪಕ್ಷ VAR_TUPLE:
	    ಪ್ರತ್ಯಾಗಮ type_name_tuple(type, tofree);

	ಪಕ್ಷ VAR_CLASS:
	ಪಕ್ಷ VAR_OBJECT:
	    ಪ್ರತ್ಯಾಗಮ type_name_class_or_obj(name, type, tofree);

	ಪಕ್ಷ VAR_FUNC:
	    ಪ್ರತ್ಯಾಗಮ type_name_func(type, tofree);

	ಸ್ವಭಾವ:
	    ಭಙ್ಗ;
    }

    ಪ್ರತ್ಯಾಗಮ name;
}

/*
 * "typename(expr)" function
 */
    ಶೂನ್ಯ
f_typename(typval_T *argvars, typval_T *rettv)
{
    garray_T	type_list;
    type_T	*type;
    ವರ್ಣ	*tofree;
    ವರ್ಣ	*name;

    rettv->v_type = VAR_STRING;
    ga_init2(&type_list, ಪರಿಮಾಣ(type_T *), 10);
    ಯದಿ (argvars[0].v_type == VAR_TYPEALIAS)
    {
	type = copy_type(argvars[0].vval.v_typealias->ta_type, &type_list);
	// A type alias for a class has the type set to VAR_OBJECT.  Change it
	// to VAR_CLASS, so that the name is "typealias<class<xxx>>"
	ಯದಿ (type->tt_type == VAR_OBJECT)
	    type->tt_type = VAR_CLASS;
    }
    ಅನ್ಯಥಾ
	type = typval2type(argvars, get_copyID(), &type_list, TVTT_DO_MEMBER);
    name = type_name(type, &tofree);
    ಯದಿ (argvars[0].v_type == VAR_TYPEALIAS)
    {
	vim_snprintf((ವರ್ಣ *)IObuff, IOSIZE, "typealias<%s>", name);
	rettv->vval.v_string = vim_strsave((char_u *)IObuff);
	ಯದಿ (tofree != ರಿಕ್ತ)
	    vim_free(tofree);
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (tofree != ರಿಕ್ತ)
	    rettv->vval.v_string = (char_u *)tofree;
	ಅನ್ಯಥಾ
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
    ಪೂರ್ಣಾಙ್ಕ
check_typval_is_ಮೌಲ್ಯ(typval_T *tv)
{
    ಯದಿ (tv == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ OK;

    ವಿಕಲ್ಪ (tv->v_type)
    {
	ಪಕ್ಷ VAR_CLASS:
	    {
		class_T *cl = tv->vval.v_class;
		char_u *class_name = (cl == ರಿಕ್ತ) ? (char_u *)""
							: cl->class_name.string;
		ಯದಿ (cl != ರಿಕ್ತ && IS_ENUM(cl))
		    semsg(_(e_using_enum_as_ಮೌಲ್ಯ_str), class_name);
		ಅನ್ಯಥಾ
		    semsg(_(e_using_class_as_ಮೌಲ್ಯ_str), class_name);
	    }
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	ಪಕ್ಷ VAR_TYPEALIAS:
	    semsg(_(e_using_typealias_as_ಮೌಲ್ಯ_str),
		    tv->vval.v_typealias->ta_name);
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	ಸ್ವಭಾವ:
	    ಭಙ್ಗ;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Same as above, except check type_T.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_type_is_ಮೌಲ್ಯ(type_T *type)
{
    ಯದಿ (type == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ OK;
    ವಿಕಲ್ಪ (type->tt_type)
    {
	ಪಕ್ಷ VAR_CLASS:
	    ಯದಿ (type->tt_class != ರಿಕ್ತ && IS_ENUM(type->tt_class))
		semsg(_(e_using_enum_as_ಮೌಲ್ಯ_str),
			type->tt_class->class_name.string);
	    ಅನ್ಯಥಾ
		semsg(_(e_using_class_as_ಮೌಲ್ಯ_str),
			type->tt_class == ರಿಕ್ತ ? (char_u *)""
			: type->tt_class->class_name.string);
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	ಪಕ್ಷ VAR_TYPEALIAS:
	    // TODO: Not sure what could be done here to get a name.
	    //       Maybe an optional argument?
	    emsg(_(e_using_typealias_as_var_val));
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	ಸ್ವಭಾವ:
	    ಭಙ್ಗ;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

#endif // FEAT_EVAL
