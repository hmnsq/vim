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

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USING_FLOAT_STUFF
#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_EVAL)

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VMS
# include <float.h>
#ਜੇ_ਅੰਤ

/*
 * Allocate memory for a type_T and add the pointer to type_gap, so that it can
 * be easily freed later.
 */
    type_T *
get_type_ptr(garray_T *type_gap)
{
    type_T *type;

    ਜੇ (ga_grow(type_gap, 1) == FAIL)
	ਵਾਪਸ ਨਿਰਾ;
    type = ALLOC_CLEAR_ONE(type_T);
    ਜੇ (type == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ((type_T **)type_gap->ga_ਅੰਕੜੇ)[type_gap->ga_len] = type;
    ++type_gap->ga_len;
    ਵਾਪਸ type;
}

/*
 * Make a shallow copy of "type".
 * When allocation fails returns "type".
 */
    type_T *
copy_type(type_T *type, garray_T *type_gap)
{
    type_T *copy = get_type_ptr(type_gap);

    ਜੇ (copy == ਨਿਰਾ)
	ਵਾਪਸ type;
    *copy = *type;
    copy->tt_flags &= ~TTFLAG_STATIC;

    ਜੇ (type->tt_ਦਲੀਲਾਂ != ਨਿਰਾ
	   && func_type_ਜੋੜੋ_arg_types(copy, type->tt_argcount, type_gap) == OK)
	ਲਈ (ਪੂਰਨ i = 0; i < type->tt_argcount; ++i)
	    copy->tt_ਦਲੀਲਾਂ[i] = type->tt_ਦਲੀਲਾਂ[i];

    ਵਾਪਸ copy;
}

/*
 * Inner part of copy_type_deep().
 * When allocation fails returns "type".
 */
    ਸਥਿਰ type_T *
copy_type_deep_rec(type_T *type, garray_T *type_gap, garray_T *ਵੇਖਿਆ_types)
{
    ਲਈ (ਪੂਰਨ i = 0; i < ਵੇਖਿਆ_types->ga_len; ++i)
	ਜੇ (((type_T **)ਵੇਖਿਆ_types->ga_ਅੰਕੜੇ)[i * 2] == type)
	    // seen this type before, return the copy we made
	    ਵਾਪਸ ((type_T **)ਵੇਖਿਆ_types->ga_ਅੰਕੜੇ)[i * 2 + 1];

    type_T *copy = copy_type(type, type_gap);
    ਜੇ (ga_grow(ਵੇਖਿਆ_types, 1) == FAIL)
	ਵਾਪਸ copy;
    ((type_T **)ਵੇਖਿਆ_types->ga_ਅੰਕੜੇ)[ਵੇਖਿਆ_types->ga_len * 2] = type;
    ((type_T **)ਵੇਖਿਆ_types->ga_ਅੰਕੜੇ)[ਵੇਖਿਆ_types->ga_len * 2 + 1] = copy;
    ++ਵੇਖਿਆ_types->ga_len;

    ਜੇ (copy->tt_member != ਨਿਰਾ)
	copy->tt_member = copy_type_deep_rec(copy->tt_member,
							 type_gap, ਵੇਖਿਆ_types);
    ਜੇ (type->tt_ਦਲੀਲਾਂ != ਨਿਰਾ)
	ਲਈ (ਪੂਰਨ i = 0; i < type->tt_argcount; ++i)
	    copy->tt_ਦਲੀਲਾਂ[i] = copy_type_deep_rec(copy->tt_ਦਲੀਲਾਂ[i],
							 type_gap, ਵੇਖਿਆ_types);

    ਵਾਪਸ copy;
}

/*
 * Make a deep copy of "type".
 * When allocation fails returns "type".
 */
    type_T *
copy_type_deep(type_T *type, garray_T *type_gap)
{
    garray_T ਵੇਖਿਆ_types;
    // stores type pairs : a type we have seen and the copy used
    ga_init2(&ਵੇਖਿਆ_types, ਆਕਾਰ(type_T *) * 2, 20);

    type_T *res = copy_type_deep_rec(type, type_gap, &ਵੇਖਿਆ_types);

    ga_clear(&ਵੇਖਿਆ_types);
    ਵਾਪਸ res;
}

    ਖਾਲੀ
clear_type_list(garray_T *gap)
{
    ਜਦੋਂ (gap->ga_len > 0)
	vim_free(((type_T **)gap->ga_ਅੰਕੜੇ)[--gap->ga_len]);
    ga_clear(gap);
}

    ਖਾਲੀ
clear_func_type_list(garray_T *gap, type_T **func_type)
{
    ਜਦੋਂ (gap->ga_len > 0)
    {
	// func_type pointing to the uf_type_list, so reset pointer
	ਜੇ (*func_type == ((type_T **)gap->ga_ਅੰਕੜੇ)[--gap->ga_len])
	    *func_type = &t_func_any;
	vim_free(((type_T **)gap->ga_ਅੰਕੜੇ)[gap->ga_len]);
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

    ਜੇ (type == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    // A fixed type never contains allocated types, return as-is.
    ਜੇ (type->tt_flags & TTFLAG_STATIC)
	ਵਾਪਸ type;

    ret = ALLOC_ONE(type_T);
    ਜੇ (ret == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    *ret = *type;

    ਜੇ (ret->tt_member != ਨਿਰਾ)
	ret->tt_member = alloc_type(ret->tt_member);

    ਜੇ (type->tt_argcount > 0 && type->tt_ਦਲੀਲਾਂ != ਨਿਰਾ)
    {
	ਪੂਰਨ i;

	ret->tt_ਦਲੀਲਾਂ = ALLOC_MULT(type_T *, type->tt_argcount);
	ਜੇ (ret->tt_ਦਲੀਲਾਂ != ਨਿਰਾ)
	    ਲਈ (i = 0; i < type->tt_argcount; ++i)
		ret->tt_ਦਲੀਲਾਂ[i] = alloc_type(type->tt_ਦਲੀਲਾਂ[i]);
    }
    ਨਹੀਂਤਾਂ
	ret->tt_ਦਲੀਲਾਂ = ਨਿਰਾ;

    ਵਾਪਸ ret;
}

/*
 * Free a type that was created with alloc_type().
 */
    ਖਾਲੀ
free_type(type_T *type)
{
    ਪੂਰਨ i;

    ਜੇ (type == ਨਿਰਾ || (type->tt_flags & TTFLAG_STATIC))
	ਵਾਪਸ;
    ਜੇ (type->tt_ਦਲੀਲਾਂ != ਨਿਰਾ)
    {
	ਲਈ (i = 0; i < type->tt_argcount; ++i)
	    free_type(type->tt_ਦਲੀਲਾਂ[i]);
	vim_free(type->tt_ਦਲੀਲਾਂ);
    }

    free_type(type->tt_member);

    vim_free(type);
}

/*
 * Return TRUE if "type" is to be recursed into for setting the type.
 */
    ਸਥਿਰ ਪੂਰਨ
ਸੈੱਟ_tv_type_recurse(type_T *type)
{
    ਵਾਪਸ type->tt_member != ਨਿਰਾ
		&& (type->tt_member->tt_type == VAR_DICT
				|| type->tt_member->tt_type == VAR_LIST)
		&& type->tt_member->tt_member != ਨਿਰਾ
		&& type->tt_member->tt_member != &t_any
		&& type->tt_member->tt_member != &t_unknown;
}

/*
 * Set the type of Dict "d" to "type"
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_tv_type_dict(dict_T *d, type_T *type)
{
    ਜੇ (d->dv_type == type)
	ਵਾਪਸ;

    free_type(d->dv_type);
    d->dv_type = alloc_type(type);

    // Need to recursively set the type of dict items?
    ਜੇ (!ਸੈੱਟ_tv_type_recurse(type))
	ਵਾਪਸ;

    ਪੂਰਨ		todo = (ਪੂਰਨ)d->dv_hashtab.ht_ਵਰਤਿਆ;
    hashitem_T	*hi;
    dictitem_T	*di;

    FOR_ALL_HASHTAB_ਚੀਜ਼ਾਂ(&d->dv_hashtab, hi, todo)
    {
	ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	{
	    --todo;
	    di = HI2DI(hi);
	    ਸੈੱਟ_tv_type(&di->di_tv, type->tt_member);
	}
    }
}

/*
 * Set the type of List "l" to "type"
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_tv_type_list(list_T *l, type_T *type)
{
    ਜੇ (l->lv_type == type)
	ਵਾਪਸ;

    free_type(l->lv_type);
    l->lv_type = alloc_type(type);

    // Need to recursively set the type of list items?
    ਜੇ (l->lv_ਪਹਿਲਾ == &range_list_ਵਸਤੂ || !ਸੈੱਟ_tv_type_recurse(type))
	ਵਾਪਸ;

    listitem_T	*li;

    FOR_ALL_LIST_ਚੀਜ਼ਾਂ(l, li)
	ਸੈੱਟ_tv_type(&li->li_tv, type->tt_member);
}

/*
 * Set the type of Tuple "tuple" to "type"
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_tv_type_tuple(tuple_T *tuple, type_T *type)
{
    ਜੇ (tuple->tv_type == type)
	ਵਾਪਸ;

    free_type(tuple->tv_type);
    tuple->tv_type = alloc_type(type);

    ਜੇ (type->tt_argcount <= 0)
	ਵਾਪਸ;

    // recursively set the type of list items
    type_T	*ਵਸਤੂ_type;
    ਲਈ (ਪੂਰਨ i = 0; i < tuple_len(tuple); i++)
    {
	ਜੇ ((type->tt_flags & TTFLAG_VARARGS) && (i >= type->tt_argcount - 1))
	    // For a variadic tuple, the last type is a List.  So use the
	    // List member type.
	    ਵਸਤੂ_type = type->tt_ਦਲੀਲਾਂ[type->tt_argcount - 1]->tt_member;
	ਨਹੀਂਤਾਂ
	    ਵਸਤੂ_type = type->tt_ਦਲੀਲਾਂ[i];

	ਸੈੱਟ_tv_type(TUPLE_ਵਸਤੂ(tuple, i), ਵਸਤੂ_type);
    }
}

/*
 * Set the type of "tv" to "type" if it is a list or tuple or dict.
 */
    ਖਾਲੀ
ਸੈੱਟ_tv_type(typval_T *tv, type_T *type)
{
    ਜੇ (type->tt_type == VAR_ANY)
	// If the variable type is "any", then keep the value type.
	// e.g.  var x: any = [1, 2] or var y: any = {v: 1}
	ਵਾਪਸ;

    ਜੇ (tv->v_type == VAR_DICT && tv->vval.v_dict != ਨਿਰਾ)
	ਸੈੱਟ_tv_type_dict(tv->vval.v_dict, type);
    ਨਹੀਂਤਾਂ ਜੇ (tv->v_type == VAR_LIST && tv->vval.v_list != ਨਿਰਾ)
	ਸੈੱਟ_tv_type_list(tv->vval.v_list, type);
    ਨਹੀਂਤਾਂ ਜੇ (tv->v_type == VAR_TUPLE && tv->vval.v_tuple != ਨਿਰਾ)
	ਸੈੱਟ_tv_type_tuple(tv->vval.v_tuple, type);
}

/*
 * For a tuple type, reserve space for "typecount" types (including the
 * repeated type).
 */
    ਸਥਿਰ ਪੂਰਨ
tuple_type_ਜੋੜੋ_types(
    type_T	*tupletype,
    ਪੂਰਨ		typecount,
    garray_T	*type_gap)
{
    // To make it easy to free the space needed for the types, add the
    // pointer to type_gap.
    ਜੇ (ga_grow(type_gap, 1) == FAIL)
	ਵਾਪਸ FAIL;
    tupletype->tt_ਦਲੀਲਾਂ = alloc_clear_id(ਆਕਾਰ(type_T *) * typecount,
							  aid_tuple_type_ਦਲੀਲਾਂ);
    ਜੇ (tupletype->tt_ਦਲੀਲਾਂ == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    ((type_T **)type_gap->ga_ਅੰਕੜੇ)[type_gap->ga_len] =
						(ਖਾਲੀ *)tupletype->tt_ਦਲੀਲਾਂ;
    ++type_gap->ga_len;
    ਵਾਪਸ OK;
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
    ਜੇ (member_type == ਨਿਰਾ || (member_type->tt_type == VAR_ANY
					&& !IS_GENERIC_TYPE(member_type)))
	ਵਾਪਸ &t_list_any;
    ਜੇ (member_type->tt_type == VAR_VOID
	    || member_type->tt_type == VAR_UNKNOWN)
	ਵਾਪਸ &t_list_ਖਾਲੀ_ਥਾਵਾਂ;
    ਜੇ (member_type->tt_type == VAR_BOOL)
	ਵਾਪਸ &t_list_bool;
    ਜੇ (member_type->tt_type == VAR_ਸੰਖਿਆ)
	ਵਾਪਸ &t_list_ਸੰਖਿਆ;
    ਜੇ (member_type->tt_type == VAR_STRING)
	ਵਾਪਸ &t_list_string;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    ਜੇ (type == ਨਿਰਾ)
	ਵਾਪਸ &t_any;
    type->tt_type = VAR_LIST;
    type->tt_member = member_type;
    type->tt_argcount = 0;
    type->tt_ਦਲੀਲਾਂ = ਨਿਰਾ;
    ਵਾਪਸ type;
}

/*
 * Create and return a tuple type from the tuple item types in
 * "tuple_types_ga".
 */
    type_T *
get_tuple_type(garray_T *tuple_types_gap, garray_T *type_gap)
{
    type_T	*type;
    type_T	**tuple_types = tuple_types_gap->ga_ਅੰਕੜੇ;
    ਪੂਰਨ		typecount = tuple_types_gap->ga_len;

    // recognize commonly used types
    ਜੇ (typecount == 0)
	ਵਾਪਸ &t_tuple_any;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    ਜੇ (type == ਨਿਰਾ)
	ਵਾਪਸ &t_any;
    type->tt_type = VAR_TUPLE;
    type->tt_member = ਨਿਰਾ;
    ਜੇ (typecount > 0)
    {
	ਜੇ (tuple_type_ਜੋੜੋ_types(type, typecount, type_gap) == FAIL)
	    ਵਾਪਸ &t_any;
	mch_memmove(type->tt_ਦਲੀਲਾਂ, tuple_types, ਆਕਾਰ(type_T *) * typecount);
    }
    type->tt_argcount = typecount;
    type->tt_flags = 0;

    ਵਾਪਸ type;
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
    ਜੇ (member_type == ਨਿਰਾ || (member_type->tt_type == VAR_ANY
					&& !IS_GENERIC_TYPE(member_type)))
	ਵਾਪਸ &t_dict_any;
    ਜੇ (member_type->tt_type == VAR_VOID
	    || member_type->tt_type == VAR_UNKNOWN)
	ਵਾਪਸ &t_dict_ਖਾਲੀ_ਥਾਵਾਂ;
    ਜੇ (member_type->tt_type == VAR_BOOL)
	ਵਾਪਸ &t_dict_bool;
    ਜੇ (member_type->tt_type == VAR_ਸੰਖਿਆ)
	ਵਾਪਸ &t_dict_ਸੰਖਿਆ;
    ਜੇ (member_type->tt_type == VAR_STRING)
	ਵਾਪਸ &t_dict_string;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    ਜੇ (type == ਨਿਰਾ)
	ਵਾਪਸ &t_any;
    type->tt_type = VAR_DICT;
    type->tt_member = member_type;
    type->tt_argcount = 0;
    type->tt_ਦਲੀਲਾਂ = ਨਿਰਾ;
    ਵਾਪਸ type;
}

/*
 * Allocate a new type for a function.
 */
    type_T *
alloc_func_type(type_T *ret_type, ਪੂਰਨ argcount, garray_T *type_gap)
{
    type_T *type = get_type_ptr(type_gap);

    ਜੇ (type == ਨਿਰਾ)
	ਵਾਪਸ &t_any;
    type->tt_type = VAR_FUNC;
    type->tt_member = ret_type == ਨਿਰਾ ? &t_unknown : ret_type;
    type->tt_argcount = argcount;
    type->tt_ਦਲੀਲਾਂ = ਨਿਰਾ;
    ਵਾਪਸ type;
}

/*
 * Allocate a new type for a tuple.
 */
    ਸਥਿਰ type_T *
alloc_tuple_type(ਪੂਰਨ typecount, garray_T *type_gap)
{
    type_T *type = get_type_ptr(type_gap);

    ਜੇ (type == ਨਿਰਾ)
	ਵਾਪਸ &t_any;
    type->tt_type = VAR_TUPLE;
    type->tt_member = ਨਿਰਾ;
    type->tt_argcount = typecount;
    type->tt_ਦਲੀਲਾਂ = ਨਿਰਾ;
    ਵਾਪਸ type;
}

/*
 * Get a function type, based on the return type "ret_type".
 * "argcount" must be -1 or 0, a predefined type can be used.
 */
    type_T *
get_func_type(type_T *ret_type, ਪੂਰਨ argcount, garray_T *type_gap)
{
    // recognize commonly used types
    ਜੇ (ret_type == &t_unknown || ret_type == ਨਿਰਾ)
    {
	// (argcount == 0) is not possible
	ਵਾਪਸ &t_func_unknown;
    }
    ਜੇ (ret_type == &t_void)
    {
	ਜੇ (argcount == 0)
	    ਵਾਪਸ &t_func_0_void;
	ਨਹੀਂਤਾਂ
	    ਵਾਪਸ &t_func_void;
    }
    ਜੇ (ret_type == &t_any)
    {
	ਜੇ (argcount == 0)
	    ਵਾਪਸ &t_func_0_any;
	ਨਹੀਂਤਾਂ
	    ਵਾਪਸ &t_func_any;
    }
    ਜੇ (ret_type == &t_ਸੰਖਿਆ)
    {
	ਜੇ (argcount == 0)
	    ਵਾਪਸ &t_func_0_ਸੰਖਿਆ;
	ਨਹੀਂਤਾਂ
	    ਵਾਪਸ &t_func_ਸੰਖਿਆ;
    }
    ਜੇ (ret_type == &t_string)
    {
	ਜੇ (argcount == 0)
	    ਵਾਪਸ &t_func_0_string;
	ਨਹੀਂਤਾਂ
	    ਵਾਪਸ &t_func_string;
    }

    ਵਾਪਸ alloc_func_type(ret_type, argcount, type_gap);
}

/*
 * For a function type, reserve space for "argcount" argument types (including
 * vararg).
 */
    ਪੂਰਨ
func_type_ਜੋੜੋ_arg_types(
	type_T	    *functype,
	ਪੂਰਨ	    argcount,
	garray_T    *type_gap)
{
    // To make it easy to free the space needed for the argument types, add the
    // pointer to type_gap.
    ਜੇ (ga_grow(type_gap, 1) == FAIL)
	ਵਾਪਸ FAIL;
    functype->tt_ਦਲੀਲਾਂ = ALLOC_CLEAR_MULT(type_T *, argcount);
    ਜੇ (functype->tt_ਦਲੀਲਾਂ == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    ((type_T **)type_gap->ga_ਅੰਕੜੇ)[type_gap->ga_len] =
						     (ਖਾਲੀ *)functype->tt_ਦਲੀਲਾਂ;
    ++type_gap->ga_len;
    ਵਾਪਸ OK;
}

/*
 * Return TRUE if "type" is NULL, any or unknown.
 * This also works for const (comparing with &t_any and &t_unknown doesn't).
 */
    ਪੂਰਨ
type_any_or_unknown(type_T *type)
{
    ਵਾਪਸ type == ਨਿਰਾ || type->tt_type == VAR_ANY
					|| type->tt_type == VAR_UNKNOWN;
}

/*
 * Get a type_T for a "special" typval in "tv".
 */
    ਸਥਿਰ type_T *
special_typval2type(typval_T *tv)
{
    ਚੋਣ (tv->vval.v_ਸੰਖਿਆ)
    {
	ਹਾਲਤ VVAL_NULL:
	    ਵਾਪਸ &t_null;

	ਹਾਲਤ VVAL_NONE:
	    ਵਾਪਸ &t_none;

	ਹਾਲਤ VVAL_TRUE:
	ਹਾਲਤ VVAL_FALSE:
	    ਵਾਪਸ &t_bool;

	ਮੂਲ:
	    ਵਾਪਸ &t_unknown;
    }
}

/*
 * Get a type_T for a List typval in "tv".
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    ਸਥਿਰ type_T *
list_typval2type(typval_T *tv, ਪੂਰਨ copyID, garray_T *type_gap, ਪੂਰਨ flags)
{
    list_T	*l = tv->vval.v_list;
    listitem_T	*li;
    type_T	*member_type = ਨਿਰਾ;

    // An empty list has type list<unknown>, unless the type was specified
    // and is not list<any>.  This matters when assigning to a variable
    // with a specific list type.
    ਜੇ (l == ਨਿਰਾ || (l->lv_ਪਹਿਲਾ == ਨਿਰਾ
		&& (l->lv_type == ਨਿਰਾ || l->lv_type->tt_member == &t_any)))
	ਵਾਪਸ &t_list_ਖਾਲੀ_ਥਾਵਾਂ;

    ਜੇ ((flags & TVTT_DO_MEMBER) == 0)
	ਵਾਪਸ &t_list_any;

    // If the type is list<any> go through the members, it may end up a
    // more specific type.
    ਜੇ (l->lv_type != ਨਿਰਾ && (l->lv_ਪਹਿਲਾ == ਨਿਰਾ
		|| (flags & TVTT_ਹੋਰ_SPECIFIC) == 0
		|| l->lv_type->tt_member != &t_any))
	// make a copy, lv_type may be freed if the list is freed
	ਵਾਪਸ copy_type_deep(l->lv_type, type_gap);

    ਜੇ (l->lv_ਪਹਿਲਾ == &range_list_ਵਸਤੂ)
	ਵਾਪਸ &t_list_ਸੰਖਿਆ;

    ਜੇ (l->lv_copyID == copyID)
	// avoid recursion
	ਵਾਪਸ &t_list_any;

    l->lv_copyID = copyID;

    // Use the common type of all members.
    member_type = typval2type(&l->lv_ਪਹਿਲਾ->li_tv, copyID, type_gap,
							TVTT_DO_MEMBER);
    ਲਈ (li = l->lv_ਪਹਿਲਾ->li_ਅਗਲਾ; li != ਨਿਰਾ; li = li->li_ਅਗਲਾ)
	common_type(typval2type(&li->li_tv, copyID, type_gap, TVTT_DO_MEMBER),
					member_type, &member_type, type_gap);

    // Reset copyID so that a shared reference to this list (not a circular
    // reference) can be processed again to get the correct type.
    l->lv_copyID = 0;

    ਵਾਪਸ get_list_type(member_type, type_gap);
}

/*
 * Get a type_T for a Tuple typval in "tv".
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    ਸਥਿਰ type_T *
tuple_typval2type(typval_T *tv, ਪੂਰਨ copyID, garray_T *type_gap, ਪੂਰਨ flags)
{
    tuple_T	*tuple = tv->vval.v_tuple;
    ਪੂਰਨ		len = tuple_len(tuple);
    type_T	*type = ਨਿਰਾ;

    // An empty tuple has type tuple<unknown>, unless the type was specified
    // and is not tuple<any>.  This matters when assigning to a variable
    // with a specific tuple type.
    ਜੇ (tuple == ਨਿਰਾ || (len == 0 && (tuple->tv_type == ਨਿਰਾ
				|| tuple->tv_type->tt_argcount == 0)))
	ਵਾਪਸ &t_tuple_ਖਾਲੀ_ਥਾਵਾਂ;

    ਜੇ ((flags & TVTT_DO_MEMBER) == 0)
	ਵਾਪਸ &t_tuple_any;

    // If the type is tuple<any> go through the members, it may end up a
    // more specific type.
    ਜੇ (tuple->tv_type != ਨਿਰਾ && (len == 0
					|| (flags & TVTT_ਹੋਰ_SPECIFIC) == 0))
	// make a copy, tv_type may be freed if the tuple is freed
	ਵਾਪਸ copy_type_deep(tuple->tv_type, type_gap);

    ਜੇ (tuple->tv_copyID == copyID)
	// avoid recursion
	ਵਾਪਸ &t_tuple_any;

    tuple->tv_copyID = copyID;

    garray_T	tuple_types_ga;
    ga_init2(&tuple_types_ga, ਆਕਾਰ(type_T *), 10);
    ਲਈ (ਪੂਰਨ i = 0; i < len; i++)
    {
	type = typval2type(TUPLE_ਵਸਤੂ(tuple, i), copyID, type_gap,
							TVTT_DO_MEMBER);
	ਜੇ (ga_grow(&tuple_types_ga, 1) == FAIL)
	{
	    ga_clear(&tuple_types_ga);
	    // Reset copyID so that a shared reference to this tuple can be
	    // processed again.
	    tuple->tv_copyID = 0;
	    ਵਾਪਸ ਨਿਰਾ;
	}
	((type_T **)tuple_types_ga.ga_ਅੰਕੜੇ)[tuple_types_ga.ga_len] = type;
	tuple_types_ga.ga_len++;
    }

    type_T *tuple_type = get_tuple_type(&tuple_types_ga, type_gap);
    ga_clear(&tuple_types_ga);

    // Reset copyID so that a shared reference to this tuple (not a circular
    // reference) can be processed again to get the correct type.
    tuple->tv_copyID = 0;

    ਵਾਪਸ tuple_type;
}

/*
 * Get a type_T for a Dict typval in "tv".
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    ਸਥਿਰ type_T *
dict_typval2type(typval_T *tv, ਪੂਰਨ copyID, garray_T *type_gap, ਪੂਰਨ flags)
{
    dict_iterator_T	iter;
    typval_T		*ਮੁੱਲ;
    dict_T		*d = tv->vval.v_dict;
    type_T		*member_type = ਨਿਰਾ;

    ਜੇ (d == ਨਿਰਾ || (d->dv_hashtab.ht_ਵਰਤਿਆ == 0 && d->dv_type == ਨਿਰਾ))
	ਵਾਪਸ &t_dict_ਖਾਲੀ_ਥਾਵਾਂ;

    ਜੇ ((flags & TVTT_DO_MEMBER) == 0)
	ਵਾਪਸ &t_dict_any;

    // If the type is dict<any> go through the members, it may end up a
    // more specific type.
    ਜੇ (d->dv_type != ਨਿਰਾ && (d->dv_hashtab.ht_ਵਰਤਿਆ == 0
		|| (flags & TVTT_ਹੋਰ_SPECIFIC) == 0
		|| d->dv_type->tt_member != &t_any))
	ਵਾਪਸ d->dv_type;

    ਜੇ (d->dv_copyID == copyID)
	// avoid recursion
	ਵਾਪਸ &t_dict_any;

    d->dv_copyID = copyID;

    // Use the common type of all values.
    dict_iterate_ਸ਼ੁਰੂ(tv, &iter);
    dict_iterate_ਅਗਲਾ(&iter, &ਮੁੱਲ);
    member_type = typval2type(ਮੁੱਲ, copyID, type_gap, TVTT_DO_MEMBER);

    ਜਦੋਂ (dict_iterate_ਅਗਲਾ(&iter, &ਮੁੱਲ) != ਨਿਰਾ)
	common_type(typval2type(ਮੁੱਲ, copyID, type_gap, TVTT_DO_MEMBER),
					member_type, &member_type, type_gap);

    // Reset copyID so that a shared reference to this dict (not a circular
    // reference) can be processed again to get the correct type.
    d->dv_copyID = 0;

    ਵਾਪਸ get_dict_type(member_type, type_gap);
}

/*
 * Get a type_T for a "partial" typval in "tv".
 */
    ਸਥਿਰ type_T *
partial_typval2type(typval_T *tv, ufunc_T *ufunc, garray_T *type_gap)
{
    partial_T *pt = tv->vval.v_partial;
    type_T  *type;

    type = get_type_ptr(type_gap);
    ਜੇ (type == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    *type = *ufunc->uf_func_type;
    ਜੇ (type->tt_argcount >= 0 && pt->pt_argc > 0)
    {
	type->tt_argcount -= pt->pt_argc;
	type->tt_min_argcount -= pt->pt_argc;
	ਜੇ (type->tt_argcount > 0 && func_type_ਜੋੜੋ_arg_types(type,
		    type->tt_argcount, type_gap) == OK)
	    ਲਈ (ਪੂਰਨ i = 0; i < type->tt_argcount; ++i)
		type->tt_ਦਲੀਲਾਂ[i] =
		    ufunc->uf_func_type->tt_ਦਲੀਲਾਂ[i + pt->pt_argc];
    }
    ਜੇ (pt->pt_func != ਨਿਰਾ)
	type->tt_member = pt->pt_func->uf_ret_type;

    ਵਾਪਸ type;
}

/*
 * Get a type_T for a "class" or an "object" typval in "tv".
 */
    ਸਥਿਰ type_T *
oc_typval2type(typval_T *tv)
{
    ਜੇ (tv->v_type == VAR_CLASS)
    {
	ਜੇ (tv->vval.v_class == ਨਿਰਾ)
	    ਵਾਪਸ &t_class;

	ਵਾਪਸ &tv->vval.v_class->class_type;
    }

    ਜੇ (tv->vval.v_object != ਨਿਰਾ)
	ਵਾਪਸ &tv->vval.v_object->obj_class->class_object_type;

    ਵਾਪਸ &t_object_any;
}

/*
 * Get a type_T for a "function" or a "partial"
 */
    ਸਥਿਰ type_T *
fp_typval2type(typval_T *tv, garray_T *type_gap)
{
    char_u	*name = ਨਿਰਾ;
    ufunc_T	*ufunc = ਨਿਰਾ;
    type_T	*type;
    type_T	*member_type = ਨਿਰਾ;
    ਪੂਰਨ		argcount = 0;
    ਪੂਰਨ		min_argcount = 0;

    ਜੇ (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != ਨਿਰਾ)
    {
	ਜੇ (tv->vval.v_partial->pt_func != ਨਿਰਾ)
	    ufunc = tv->vval.v_partial->pt_func;
	ਨਹੀਂਤਾਂ
	    name = tv->vval.v_partial->pt_name;
    }
    ਨਹੀਂਤਾਂ
	name = tv->vval.v_string;

    ਜੇ (name == ਨਿਰਾ && ufunc == ਨਿਰਾ)
	ਵਾਪਸ &t_func_unknown;

    ਜੇ (name != ਨਿਰਾ)
    {
	ਪੂਰਨ idx = ਲੱਭੋ_ਅੰਦਰੂਨੀ_func(name);

	ਜੇ (idx >= 0)
	{
	    type_T *decl_type;  //ਅਣਵਰਤਿਆ

	    ਅੰਦਰੂਨੀ_func_get_argcount(idx, &argcount, &min_argcount);
	    member_type = ਅੰਦਰੂਨੀ_func_ret_type(idx, 0, ਨਿਰਾ, &decl_type,
		    type_gap);
	}
	ਨਹੀਂਤਾਂ
	{
	    // Check if name contains "<".  If it does, then replace "<" with
	    // NUL and lookup the function and then look up the specific
	    // generic function using the supplied types.
	    char_u *p = generic_func_ਲੱਭੋ_open_bracket(name);
	    ਜੇ (p == ਨਿਰਾ)
		ufunc = ਲੱਭੋ_func(name, FALSE);
	    ਨਹੀਂਤਾਂ
	    {
		// generic function
		char_u	cc = *p;

		*p = NUL;
		ufunc = ਲੱਭੋ_func(name, FALSE);
		*p = cc;
		ਜੇ (ufunc != ਨਿਰਾ && IS_GENERIC_FUNC(ufunc))
		{
		    ufunc = ਲੱਭੋ_generic_func(ufunc, name, &p);
		    ਜੇ (ufunc == ਨਿਰਾ)
			ਵਾਪਸ ਨਿਰਾ;
		}
	    }
	}
    }
    ਜੇ (ufunc != ਨਿਰਾ)
    {
	// May need to get the argument types from default values by
	// compiling the function.
	ਜੇ (ufunc->uf_def_ਸਥਿਤੀ == UF_ਮੰਜ਼ਿਲ_BE_COMPILED
		&& compile_def_function(ufunc, TRUE, CT_NONE, ਨਿਰਾ)
		== FAIL)
	    ਵਾਪਸ ਨਿਰਾ;
	ਜੇ (ufunc->uf_func_type == ਨਿਰਾ)
	    ਸੈੱਟ_function_type(ufunc);
	ਜੇ (ufunc->uf_func_type != ਨਿਰਾ)
	{
	    ਜੇ (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != ਨਿਰਾ)
		ਵਾਪਸ partial_typval2type(tv, ufunc, type_gap);
	    ਵਾਪਸ ufunc->uf_func_type;
	}
    }

    type = get_type_ptr(type_gap);
    ਜੇ (type == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    type->tt_type = tv->v_type;
    type->tt_argcount = argcount;
    type->tt_min_argcount = min_argcount;
    ਜੇ (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != ਨਿਰਾ
					&& tv->vval.v_partial->pt_argc > 0)
    {
	type->tt_argcount -= tv->vval.v_partial->pt_argc;
	type->tt_min_argcount -= tv->vval.v_partial->pt_argc;
    }
    type->tt_member = member_type;

    ਵਾਪਸ type;
}

/*
 * Get a type_T for a typval_T.
 * "type_gap" is used to temporarily create types in.
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    ਸਥਿਰ type_T *
typval2type_int(typval_T *tv, ਪੂਰਨ copyID, garray_T *type_gap, ਪੂਰਨ flags)
{
    ਚੋਣ (tv->v_type)
    {
	ਹਾਲਤ VAR_UNKNOWN:
	    ਵਾਪਸ &t_unknown;

	ਹਾਲਤ VAR_ANY:
	    ਵਾਪਸ &t_any;

	ਹਾਲਤ VAR_VOID:
	    ਵਾਪਸ &t_void;

	ਹਾਲਤ VAR_BOOL:
	    ਵਾਪਸ &t_bool;

	ਹਾਲਤ VAR_SPECIAL:
	    ਵਾਪਸ special_typval2type(tv);

	ਹਾਲਤ VAR_ਸੰਖਿਆ:
	    ਵਾਪਸ &t_ਸੰਖਿਆ;

	ਹਾਲਤ VAR_FLOAT:
	    ਵਾਪਸ &t_float;

	ਹਾਲਤ VAR_STRING:
	    ਵਾਪਸ &t_string;

	ਹਾਲਤ VAR_BLOB:
	    ਜੇ (tv->vval.v_blob == ਨਿਰਾ)
		ਵਾਪਸ &t_blob_null;
	    ਵਾਪਸ &t_blob;

	ਹਾਲਤ VAR_LIST:
	    ਵਾਪਸ list_typval2type(tv, copyID, type_gap, flags);

	ਹਾਲਤ VAR_TUPLE:
	    ਵਾਪਸ tuple_typval2type(tv, copyID, type_gap, flags);

	ਹਾਲਤ VAR_DICT:
	    ਵਾਪਸ dict_typval2type(tv, copyID, type_gap, flags);

	ਹਾਲਤ VAR_ਕੰਮ:
	    ਵਾਪਸ &t_ਕੰਮ;

	ਹਾਲਤ VAR_CHANNEL:
	    ਵਾਪਸ &t_channel;

	ਹਾਲਤ VAR_CLASS:
	ਹਾਲਤ VAR_OBJECT:
	    ਵਾਪਸ oc_typval2type(tv);

	ਹਾਲਤ VAR_TYPEALIAS:
	    ਵਾਪਸ &t_typealias;

	ਹਾਲਤ VAR_FUNC:
	ਹਾਲਤ VAR_PARTIAL:
	    ਵਾਪਸ fp_typval2type(tv, type_gap);

	ਹਾਲਤ VAR_INSTR:
	ਮੂਲ:
	    ਰੋਕੋ;
    }

    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Return TRUE if "tv" is not a bool but should be converted to bool.
 */
    ਪੂਰਨ
need_convert_ਮੰਜ਼ਿਲ_bool(type_T *type, typval_T *tv)
{
    ਵਾਪਸ type != ਨਿਰਾ && type == &t_bool && tv->v_type != VAR_BOOL
	    && (tv->v_type == VAR_ਸੰਖਿਆ
		       && (tv->vval.v_ਸੰਖਿਆ == 0 || tv->vval.v_ਸੰਖਿਆ == 1));
}

/*
 * Get a type_T for a typval_T.
 * "type_list" is used to temporarily create types in.
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the most specific member type.
 */
    type_T *
typval2type(typval_T *tv, ਪੂਰਨ copyID, garray_T *type_gap, ਪੂਰਨ flags)
{
    type_T *type = typval2type_int(tv, copyID, type_gap, flags);

    ਜੇ (type == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (type != &t_bool && (tv->v_type == VAR_ਸੰਖਿਆ
		&& (tv->vval.v_ਸੰਖਿਆ == 0 || tv->vval.v_ਸੰਖਿਆ == 1)))
	// Number 0 and 1 and expression with "&&" or "||" can also be used
	// for bool.
	type = &t_ਸੰਖਿਆ_bool;
    ਨਹੀਂਤਾਂ ਜੇ (type != &t_float && tv->v_type == VAR_ਸੰਖਿਆ)
	// A number can also be used for float.
	type = &t_ਸੰਖਿਆ_float;
    ਵਾਪਸ type;
}

/*
 * Return TRUE if "type" can be used for a variable declaration.
 * Give an error and return FALSE if not.
 */
    ਪੂਰਨ
valid_declaration_type(type_T *type)
{
    ਜੇ (type->tt_type == VAR_SPECIAL  // null, none
	    || type->tt_type == VAR_VOID)
    {
	ਅੱਖਰ *tofree = ਨਿਰਾ;
	ਅੱਖਰ *name = type_name(type, &tofree);
	semsg(_(e_invalid_type_ਵਿੱਚ_variable_declaration_str), name);
	vim_free(tofree);
	ਵਾਪਸ FALSE;
    }
    ਵਾਪਸ TRUE;
}

/*
 * Get a type_T for a typval_T, used for v: variables.
 * "type_list" is used to temporarily create types in.
 */
    type_T *
typval2type_vimvar(typval_T *tv, garray_T *type_gap)
{
    ਜੇ (tv->v_type == VAR_LIST)  // e.g. for v:oldfiles
	ਵਾਪਸ &t_list_string;
    ਜੇ (tv->v_type == VAR_DICT)  // e.g. for v:event
	ਵਾਪਸ &t_dict_any;
    ਵਾਪਸ typval2type(tv, get_copyID(), type_gap, TVTT_DO_MEMBER);
}

    ਪੂਰਨ
check_typval_arg_type(
	type_T	    *expected,
	typval_T    *actual_tv,
	ਅੱਖਰ	    *func_name,
	ਪੂਰਨ	    arg_idx)
{
    where_T	where = WHERE_INIT;

    ਜੇ (arg_idx > 0)
    {
	where.wt_ਸੂਚਕਾਂਕ = arg_idx;
	where.wt_ਕਿਸਮ = WT_ਦਲੀਲ;
    }
    where.wt_func_name = func_name;
    ਵਾਪਸ check_typval_type(expected, actual_tv, where);
}

/*
 * Return true if "expected" is list<any> or dict<any> and "actual_tv" is a
 * list or dict of that kind, which matches whatever its values are.
 */
    ਸਥਿਰ bool
matches_listdict_any(type_T *expected, typval_T *actual_tv)
{
    ਵਾਪਸ (expected->tt_type == VAR_LIST || expected->tt_type == VAR_DICT)
	    && actual_tv->v_type == expected->tt_type
	    && expected->tt_member != ਨਿਰਾ
	    && (expected->tt_member->tt_type == VAR_ANY
		|| expected->tt_member->tt_type == VAR_UNKNOWN);
}

/*
 * Return FAIL if "expected" and "actual" don't match.
 * When "argidx" > 0 it is included in the error message.
 */
    ਪੂਰਨ
check_typval_type(type_T *expected, typval_T *actual_tv, where_T where)
{
    garray_T	type_list;
    type_T	*actual_type;
    ਪੂਰਨ		res = FAIL;

    ਜੇ (expected == ਨਿਰਾ)
	ਵਾਪਸ OK;  // didn't expect anything.

    // No need to go through the values of a possibly big list or dict.
    ਜੇ (matches_listdict_any(expected, actual_tv))
	ਵਾਪਸ OK;

    ga_init2(&type_list, ਆਕਾਰ(type_T *), 10);

    // A null_function and null_partial are special cases, they can be used to
    // clear a variable.
    ਜੇ ((actual_tv->v_type == VAR_FUNC && actual_tv->vval.v_string == ਨਿਰਾ)
	    || (actual_tv->v_type == VAR_PARTIAL
					 && actual_tv->vval.v_partial == ਨਿਰਾ))
	actual_type = &t_func_unknown;
    ਨਹੀਂਤਾਂ
	// When the actual type is list<any> or dict<any> go through the values
	// to possibly get a more specific type.
	actual_type = typval2type(actual_tv, get_copyID(), &type_list,
					  TVTT_DO_MEMBER | TVTT_ਹੋਰ_SPECIFIC);
    ਜੇ (actual_type != ਨਿਰਾ)
    {
	res = check_type_maybe(expected, actual_type, TRUE, where);
	ਜੇ (res == MAYBE && !(actual_type->tt_type == VAR_FUNC
				      && (actual_type->tt_member == &t_unknown
					  || actual_type->tt_member == ਨਿਰਾ)))
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
    ਵਾਪਸ res;
}

    ਖਾਲੀ
arg_type_mismatch(type_T *expected, type_T *actual, ਪੂਰਨ arg_idx)
{
    where_T	where = WHERE_INIT;

    ਜੇ (arg_idx > 0)
    {
	where.wt_ਸੂਚਕਾਂਕ = arg_idx;
	where.wt_ਕਿਸਮ = WT_ਦਲੀਲ;
    }
    type_mismatch_where(expected, actual, where);
}

    ਖਾਲੀ
type_mismatch_where(type_T *expected, type_T *actual, where_T where)
{
    ਅੱਖਰ *tofree1, *tofree2;
    ਅੱਖਰ *typename1 = type_name(expected, &tofree1);
    ਅੱਖਰ *typename2 = type_name(actual, &tofree2);

    ਚੋਣ (where.wt_ਕਿਸਮ)
    {
	ਹਾਲਤ WT_MEMBER:
	    semsg(_(e_variable_str_type_mismatch_expected_str_but_got_str),
		    where.wt_func_name, typename1, typename2);
	    ਰੋਕੋ;
	ਹਾਲਤ WT_ਵਿਧੀ:
	ਹਾਲਤ WT_ਵਿਧੀ_ARG:
	ਹਾਲਤ WT_ਵਿਧੀ_RETURN:
	    semsg(_(e_ਵਿਧੀ_str_type_mismatch_expected_str_but_got_str),
		    where.wt_func_name, typename1, typename2);
	    ਰੋਕੋ;
	ਹਾਲਤ WT_VARIABLE:
	    ਜੇ (where.wt_func_name == ਨਿਰਾ)
		semsg(_(e_variable_nr_type_mismatch_expected_str_but_got_str),
			where.wt_ਸੂਚਕਾਂਕ, typename1, typename2);
	    ਨਹੀਂਤਾਂ
		semsg(_(e_variable_nr_type_mismatch_expected_str_but_got_str_ਵਿੱਚ_str),
			where.wt_ਸੂਚਕਾਂਕ, typename1, typename2, where.wt_func_name);
	    ਰੋਕੋ;
	ਹਾਲਤ WT_ਦਲੀਲ:
	    ਜੇ (where.wt_func_name == ਨਿਰਾ)
		semsg(_(e_ਦਲੀਲ_nr_type_mismatch_expected_str_but_got_str),
			where.wt_ਸੂਚਕਾਂਕ, typename1, typename2);
	    ਨਹੀਂਤਾਂ
		semsg(_(e_ਦਲੀਲ_nr_type_mismatch_expected_str_but_got_str_ਵਿੱਚ_str),
			where.wt_ਸੂਚਕਾਂਕ, typename1, typename2, where.wt_func_name);
	    ਰੋਕੋ;
	ਹਾਲਤ WT_CAST:
	ਹਾਲਤ WT_UNKNOWN:
	    ਜੇ (where.wt_func_name == ਨਿਰਾ)
		semsg(_(e_type_mismatch_expected_str_but_got_str),
			typename1, typename2);
	    ਨਹੀਂਤਾਂ
		semsg(_(e_type_mismatch_expected_str_but_got_str_ਵਿੱਚ_str),
			typename1, typename2, where.wt_func_name);
	    ਰੋਕੋ;
    }

    vim_free(tofree1);
    vim_free(tofree2);
}

/*
 * Check if the expected and actual types match for a tuple
 */
    ਸਥਿਰ ਪੂਰਨ
check_tuple_type_maybe(
    type_T	*expected,
    type_T	*actual,
    where_T	where)
{
    ਜੇ (expected->tt_argcount == -1 || actual->tt_argcount == -1
		|| expected->tt_ਦਲੀਲਾਂ == ਨਿਰਾ || actual->tt_ਦਲੀਲਾਂ == ਨਿਰਾ)
	ਵਾਪਸ OK;

    // For a non-variadic tuple, the number of items must match
    ਜੇ (!(expected->tt_flags & TTFLAG_VARARGS)
	    && expected->tt_argcount != actual->tt_argcount)
	ਵਾਪਸ FAIL;

    // compare the type of each tuple item
    ਲਈ (ਪੂਰਨ i = 0; i < actual->tt_argcount; ++i)
    {
	type_T	*exp_type;
	type_T	*actual_type;

	ਜੇ (expected->tt_flags & TTFLAG_VARARGS)
	{
	    ਜੇ (i < expected->tt_argcount - 1)
		exp_type = expected->tt_ਦਲੀਲਾਂ[i];
	    ਨਹੀਂਤਾਂ
		// For a variadic tuple, the last type is a List.  So use the
		// List member type.
		exp_type = expected->tt_ਦਲੀਲਾਂ[expected->tt_argcount - 1]->tt_member;
	}
	ਨਹੀਂਤਾਂ
	    exp_type = expected->tt_ਦਲੀਲਾਂ[i];

	ਜੇ (actual->tt_flags & TTFLAG_VARARGS)
	{
	    ਜੇ (i < actual->tt_argcount - 1)
		actual_type = actual->tt_ਦਲੀਲਾਂ[i];
	    ਨਹੀਂਤਾਂ
		// For a variadic tuple, the last type is a List.  So use the
		// List member type.
		actual_type = actual->tt_ਦਲੀਲਾਂ[actual->tt_argcount - 1]->tt_member;
	}
	ਨਹੀਂਤਾਂ
	    actual_type = actual->tt_ਦਲੀਲਾਂ[i];

	// Allow for using "any" type for a tuple item
	ਜੇ (actual->tt_ਦਲੀਲਾਂ[i] != &t_any && check_type(exp_type, actual_type,
						FALSE, where) == FAIL)
	    ਵਾਪਸ FAIL;
    }

    ਵਾਪਸ OK;
}

/*
 * Check if the expected and actual types match for a function
 * Returns OK if "expected" and "actual" are matching function types.
 * Returns FAIL if "expected" and "actual" are different types.
 * Returns MAYBE when a runtime type check is needed.
 */
    ਸਥਿਰ ਪੂਰਨ
check_func_type_maybe(
    type_T	*expected,
    type_T	*actual,
    where_T	where)
{
    ਪੂਰਨ ret = OK;

    // If the return type is unknown it can be anything, including
    // nothing, thus there is no point in checking.
    ਜੇ (expected->tt_member != &t_unknown)
    {
	ਜੇ (actual->tt_member != ਨਿਰਾ
		&& actual->tt_member != &t_unknown)
	{
	    where_T  func_where = where;

	    func_where.wt_ਕਿਸਮ = WT_ਵਿਧੀ_RETURN;
	    ret = check_type_maybe(expected->tt_member,
		    actual->tt_member, FALSE,
		    func_where);
	}
	ਨਹੀਂਤਾਂ
	    ret = MAYBE;
    }
    ਜੇ (ret != FAIL
	    && ((expected->tt_flags & TTFLAG_VARARGS)
		!= (actual->tt_flags & TTFLAG_VARARGS))
	    && expected->tt_argcount != -1)
	ret = FAIL;
    ਜੇ (ret != FAIL && expected->tt_argcount != -1
	    && actual->tt_min_argcount != -1
	    && (actual->tt_argcount == -1
		|| (actual->tt_argcount < expected->tt_min_argcount
		    || actual->tt_argcount > expected->tt_argcount)))
	ret = FAIL;
    ਜੇ (ret != FAIL && expected->tt_ਦਲੀਲਾਂ != ਨਿਰਾ
	    && actual->tt_ਦਲੀਲਾਂ != ਨਿਰਾ)
    {
	ਪੂਰਨ i;

	ਲਈ (i = 0; i < expected->tt_argcount
		&& i < actual->tt_argcount; ++i)
	{
	    where_T  func_where = where;
	    func_where.wt_ਕਿਸਮ = WT_ਵਿਧੀ_ARG;

	    // Allow for using "any" argument type, lambda's have them.
	    ਜੇ (actual->tt_ਦਲੀਲਾਂ[i] != &t_any && check_type(
			expected->tt_ਦਲੀਲਾਂ[i], actual->tt_ਦਲੀਲਾਂ[i], FALSE,
			func_where) == FAIL)
	    {
		ret = FAIL;
		ਰੋਕੋ;
	    }
	}
    }
    ਜੇ (ret == OK && expected->tt_argcount >= 0
	    && actual->tt_argcount == -1)
	// check the argument count at runtime
	ret = MAYBE;

    ਵਾਪਸ ret;
}

/*
 * Check if the expected and actual types match for an object
 * Returns OK if "expected" and "actual" are matching object types.
 * Returns FAIL if "expected" and "actual" are different types.
 * Returns MAYBE when a runtime type check is needed.
 */
    ਸਥਿਰ ਪੂਰਨ
check_object_type_maybe(
    type_T	*expected,
    type_T	*actual,
    where_T	where)
{
    ਪੂਰਨ ret = OK;

    ਜੇ (actual->tt_type == VAR_ANY)
	ਵਾਪਸ MAYBE;	// use runtime type check
    ਜੇ (actual->tt_type != VAR_OBJECT)
	ਵਾਪਸ FAIL;	// don't use tt_class
    ਜੇ (actual->tt_class == ਨਿਰਾ)    // null object
	ਵਾਪਸ OK;
    // t_object_any matches any object except for an enum item
    ਜੇ (expected == &t_object_any && !IS_ENUM(actual->tt_class))
	ਵਾਪਸ OK;

    // For object method arguments, do a invariant type check in
    // an extended class.  For all others, do a covariance type check.
    ਜੇ (where.wt_ਕਿਸਮ == WT_ਵਿਧੀ_ARG)
    {
	ਜੇ (actual->tt_class != expected->tt_class)
	    ret = FAIL;
    }
    ਨਹੀਂਤਾਂ ਜੇ (!class_instance_ਦਾ(actual->tt_class, expected->tt_class))
    {
	// Check if this is an up-cast, if so we'll have to check the type at
	// runtime.
	ਜੇ (where.wt_ਕਿਸਮ == WT_CAST &&
		class_instance_ਦਾ(expected->tt_class, actual->tt_class))
	    ret = MAYBE;
	ਨਹੀਂਤਾਂ
	    ret = FAIL;
    }

    ਵਾਪਸ ret;
}

/*
 * Check if the expected and actual types match.
 * Does not allow for assigning "any" to a specific type.
 * When "argidx" > 0 it is included in the error message.
 * Return OK if types match.
 * Return FAIL if types do not match.
 */
    ਪੂਰਨ
check_type(
	type_T	*expected,
	type_T	*actual,
	ਪੂਰਨ	give_ਸੁਨੇਹਾ_2,
	where_T where)
{
    ਪੂਰਨ ret = check_type_maybe(expected, actual, give_ਸੁਨੇਹਾ_2, where);

    ਵਾਪਸ ret == MAYBE ? OK : ret;
}

/*
 * As check_type() but return MAYBE when a runtime type check should be used
 * when compiling.
 */
    ਪੂਰਨ
check_type_maybe(
	type_T	*expected,
	type_T	*actual,
	ਪੂਰਨ	give_ਸੁਨੇਹਾ_2,
	where_T where)
{
    ਪੂਰਨ ret = OK;

    // When expected is "unknown" we accept any actual type.
    // When expected is "any" we accept any actual type except "void".
    ਜੇ (expected->tt_type != VAR_UNKNOWN
	    && !(expected->tt_type == VAR_ANY && actual->tt_type != VAR_VOID))

    {
	// tt_type should match, except that a "partial" can be assigned to a
	// variable with type "func".
	// And "unknown" (using global variable) and "any" need a runtime type
	// check.
	ਜੇ (!(expected->tt_type == actual->tt_type
		    || actual->tt_type == VAR_UNKNOWN
		    || actual->tt_type == VAR_ANY
		    || (expected->tt_type == VAR_FUNC
					   && actual->tt_type == VAR_PARTIAL)))
	{
	    ਜੇ (expected->tt_type == VAR_BOOL
					&& (actual->tt_flags & TTFLAG_BOOL_OK))
		// Using number 0 or 1 for bool is OK.
		ਵਾਪਸ OK;
	    ਜੇ (expected->tt_type == VAR_FLOAT
		    && actual->tt_type == VAR_ਸੰਖਿਆ
		    && ((expected->tt_flags & TTFLAG_ਸੰਖਿਆ_OK)
			     || (actual->tt_flags & TTFLAG_FLOAT_OK)))
		// Using a number where a float is expected is OK here.
		ਵਾਪਸ OK;
	    ਜੇ (expected->tt_type == VAR_LIST
		    && actual->tt_type == VAR_TUPLE
		    && (expected->tt_flags & TTFLAG_TUPLE_OK))
		// Using a tuple where a list is expected is OK here.
		ਵਾਪਸ OK;
	    ਜੇ (give_ਸੁਨੇਹਾ_2)
		type_mismatch_where(expected, actual, where);
	    ਵਾਪਸ FAIL;
	}
	ਜੇ (expected->tt_type == VAR_DICT || expected->tt_type == VAR_LIST)
	{
	    // "unknown" is used for an empty list or dict
	    ਜੇ (actual->tt_member != ਨਿਰਾ && actual->tt_member != &t_unknown)
		ret = check_type_maybe(expected->tt_member, actual->tt_member,
								 FALSE, where);
	}
	ਨਹੀਂਤਾਂ ਜੇ (expected->tt_type == VAR_TUPLE && actual != &t_any)
	    ret =  check_tuple_type_maybe(expected, actual, where);
	ਨਹੀਂਤਾਂ ਜੇ (expected->tt_type == VAR_FUNC && actual != &t_any)
	    ret = check_func_type_maybe(expected, actual, where);
	ਨਹੀਂਤਾਂ ਜੇ (expected->tt_type == VAR_OBJECT)
	    ret = check_object_type_maybe(expected, actual, where);

	ਜੇ (ret == FAIL && give_ਸੁਨੇਹਾ_2)
	    type_mismatch_where(expected, actual, where);
    }

    ਜੇ (ret == OK && expected->tt_type != VAR_UNKNOWN
	    && expected->tt_type != VAR_ANY
	    && (actual->tt_type == VAR_UNKNOWN || actual->tt_type == VAR_ANY))
	// check the type at runtime
	ret = MAYBE;

    ਵਾਪਸ ret;
}

/*
 * Check that the arguments of "type" match "argvars[argcount]".
 * "base_tv" is from "expr->Func()".
 * Return OK/FAIL.
 */
    ਪੂਰਨ
check_ਦਲੀਲ_types(
	type_T	    *type,
	typval_T    *argvars,
	ਪੂਰਨ	    argcount,
	typval_T    *ਅਧਾਰ_tv,
	char_u	    *name)
{
    ਪੂਰਨ	    varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;
    ਪੂਰਨ	    i;
    ਪੂਰਨ	    totcount = argcount + (ਅਧਾਰ_tv == ਨਿਰਾ ? 0 : 1);

    ਜੇ (type->tt_type != VAR_FUNC && type->tt_type != VAR_PARTIAL)
	ਵਾਪਸ OK;  // just in case
    ਜੇ (totcount < type->tt_min_argcount - varargs)
    {
	emsg_funcname(e_not_enough_arguments_for_function_str, name);
	ਵਾਪਸ FAIL;
    }
    ਜੇ (!varargs && type->tt_argcount >= 0 && totcount > type->tt_argcount)
    {
	emsg_funcname(e_too_many_arguments_for_function_str, name);
	ਵਾਪਸ FAIL;
    }
    ਜੇ (type->tt_ਦਲੀਲਾਂ == ਨਿਰਾ)
	ਵਾਪਸ OK;  // cannot check


    ਲਈ (i = 0; i < totcount; ++i)
    {
	type_T	    *expected;
	typval_T    *tv;

	ਜੇ (ਅਧਾਰ_tv != ਨਿਰਾ)
	{
	    ਜੇ (i == 0)
		tv = ਅਧਾਰ_tv;
	    ਨਹੀਂਤਾਂ
		tv = &argvars[i - 1];
	}
	ਨਹੀਂਤਾਂ
	    tv = &argvars[i];
	ਜੇ (varargs && i >= type->tt_argcount - 1)
	{
	    expected = type->tt_ਦਲੀਲਾਂ[type->tt_argcount - 1];
	    ਜੇ (expected != ਨਿਰਾ && expected->tt_type == VAR_LIST)
		expected = expected->tt_member;
	    ਜੇ (expected == ਨਿਰਾ)
		expected = &t_any;
	}
	ਨਹੀਂਤਾਂ
	    expected = type->tt_ਦਲੀਲਾਂ[i];

	// check the type, unless the value is v:none
	ਜੇ ((tv->v_type != VAR_SPECIAL || tv->vval.v_ਸੰਖਿਆ != VVAL_NONE)
		   && check_typval_arg_type(expected, tv, ਨਿਰਾ, i + 1) == FAIL)
	    ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

// Maximum nesting of types, to avoid running out of stack space.  E.g.
// "list<list<number>>" is nested twice.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_TYPE_ਡੂੰਘਾਈ	1000

// Current number of levels of parse_type() and the deepest level reached while
// parsing the current type.  The nesting is one less than the number of
// levels.
ਸਥਿਰ ਪੂਰਨ	parse_ਡੂੰਘਾਈ = 0;
ਸਥਿਰ ਪੂਰਨ	parse_max_ਡੂੰਘਾਈ = 0;

/*
 * Skip over type in list<type>, dict<type> or tuple<type>.
 * Returns a pointer to the character after the type.  "syn_error" is set to
 * TRUE on syntax error.
 */
    ਸਥਿਰ char_u *
skip_member_type(char_u *ਸ਼ੁਰੂ, char_u *p, ਪੂਰਨ *syn_error)
{
    ਜੇ (STRNCMP("tuple", ਸ਼ੁਰੂ, 5) == 0)
    {
	// handle tuple<{type1}, {type2}, ....<type>>
	p = skipwhite(p + 1);
	ਜਦੋਂ (*p != '>' && *p != NUL)
	{
	    char_u *sp = p;

	    ਜੇ (STRNCMP(p, "...", 3) == 0)
		p += 3;
	    p = skip_type(p, TRUE);
	    ਜੇ (p == sp)
	    {
		*syn_error = TRUE;
		ਵਾਪਸ p;  // syntax error
	    }
	    ਜੇ (*p == ',')
		p = skipwhite(p + 1);
	}
	ਜੇ (*p == '>')
	    p++;
    }
    ਨਹੀਂਤਾਂ
    {
	p = skipwhite(p);
	p = skip_type(skipwhite(p + 1), FALSE);
	p = skipwhite(p);
	ਜੇ (*p == '>')
	    ++p;
    }

    ਵਾਪਸ p;
}

/*
 * Skip over a function type.  Returns a pointer to the character after the
 * type.  "syn_error" is set to TRUE on syntax error.
 */
    ਸਥਿਰ char_u *
skip_func_type(char_u *p, ਪੂਰਨ *syn_error)
{
    ਜੇ (*p == '(')
    {
	// handle func(args): type
	++p;
	ਜਦੋਂ (*p != ')' && *p != NUL)
	{
	    char_u *sp = p;

	    ਜੇ (STRNCMP(p, "...", 3) == 0)
		p += 3;
	    p = skip_type(p, TRUE);
	    ਜੇ (p == sp)
	    {
		*syn_error = TRUE;
		ਵਾਪਸ p;  // syntax error
	    }
	    ਜੇ (*p == ',')
		p = skipwhite(p + 1);
	}
	ਜੇ (*p == ')')
	{
	    ਜੇ (p[1] == ':')
		p = skip_type(skipwhite(p + 2), FALSE);
	    ਨਹੀਂਤਾਂ
		++p;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	// handle func: return_type
	p = skip_type(skipwhite(p + 1), FALSE);
    }

    ਵਾਪਸ p;
}

/*
 * Inner part of skip_type().
 */
    ਸਥਿਰ char_u *
skip_type_inner(char_u *ਸ਼ੁਰੂ, ਪੂਰਨ optional)
{
    char_u	*p = ਸ਼ੁਰੂ;
    ਪੂਰਨ		syn_error = FALSE;

    ਜੇ (optional && *p == '?')
	++p;

    // Also skip over "." for imported classes: "import.ClassName".
    ਜਦੋਂ (ASCII_ISALNUM(*p) || *p == '_' || *p == '.')
	++p;

    // Skip over "<type>"; this is permissive about white space.
    ਜੇ (*skipwhite(p) == '<')
    {
	p = skip_member_type(ਸ਼ੁਰੂ, p, &syn_error);
	ਜੇ (syn_error)
	    ਵਾਪਸ p;
    }
    ਨਹੀਂਤਾਂ ਜੇ ((*p == '(' || (*p == ':' && VIM_ISWHITE(p[1])))
					     && STRNCMP("func", ਸ਼ੁਰੂ, 4) == 0)
    {
	// skip over function type
	p = skip_func_type(p, &syn_error);
	ਜੇ (syn_error)
	    ਵਾਪਸ p;
    }

    ਵਾਪਸ p;
}

/*
 * Skip over a type definition and return a pointer to just after it.
 * When "optional" is TRUE then a leading "?" is accepted.
 */
    char_u *
skip_type(char_u *ਸ਼ੁਰੂ, ਪੂਰਨ optional)
{
    ਸਥਿਰ ਪੂਰਨ	skip_ਡੂੰਘਾਈ = 0;
    char_u	*p = ਸ਼ੁਰੂ;

    // Too deeply nested: skip to the matching ">" without recursing, an
    // error is given when parsing the type.
    ਜੇ (skip_ਡੂੰਘਾਈ > MAX_TYPE_ਡੂੰਘਾਈ)
    {
	ਪੂਰਨ	ਡੂੰਘਾਈ = 0;

	ਲਈ ( ; *p != NUL; ++p)
	{
	    ਜੇ (*p == '<')
		++ਡੂੰਘਾਈ;
	    ਨਹੀਂਤਾਂ ਜੇ (*p == '>')
	    {
		ਜੇ (ਡੂੰਘਾਈ == 0)
		    ਰੋਕੋ;
		ਜੇ (--ਡੂੰਘਾਈ == 0)
		    ਵਾਪਸ p + 1;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (ਡੂੰਘਾਈ == 0 && !ASCII_ISALNUM(*p) && *p != '_'
								&& *p != '.')
		ਰੋਕੋ;
	}
	ਵਾਪਸ p;
    }

    ++skip_ਡੂੰਘਾਈ;
    p = skip_type_inner(ਸ਼ੁਰੂ, optional);
    --skip_ਡੂੰਘਾਈ;
    ਵਾਪਸ p;
}

/*
 * Parse the member type: "<type>" and return "type" with the member set.
 * Use "type_gap" if a new type needs to be added.
 * "info" is extra information for an error message.
 * Returns NULL in case of failure.
 */
    ਸਥਿਰ type_T *
parse_type_member(
	char_u	    **arg,
	type_T	    *type,
	garray_T    *type_gap,
	ਪੂਰਨ	    give_error,
	ਅੱਖਰ	    *info,
	ufunc_T	    *ufunc,
	cctx_T	    *cctx)
{
    char_u  *arg_ਸ਼ੁਰੂ = *arg;
    type_T  *member_type;
    ਪੂਰਨ	    prev_called_emsg = called_emsg;

    ਜੇ (**arg != '<')
    {
	ਜੇ (give_error)
	{
	    ਜੇ (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    ਨਹੀਂਤਾਂ
		semsg(_(e_missing_type_after_str), info);
	}
	ਵਾਪਸ ਨਿਰਾ;
    }
    *arg = skipwhite(*arg + 1);

    member_type = parse_type(arg, type_gap, ufunc, cctx, give_error);
    ਜੇ (member_type == ਨਿਰਾ || !valid_declaration_type(member_type))
	ਵਾਪਸ ਨਿਰਾ;

    *arg = skipwhite(*arg);
    ਜੇ (**arg != '>' && called_emsg == prev_called_emsg)
    {
	ਜੇ (give_error)
	    semsg(_(e_missing_gt_after_type_str), arg_ਸ਼ੁਰੂ);
	ਵਾਪਸ ਨਿਰਾ;
    }
    ++*arg;

    ਜੇ (type->tt_type == VAR_LIST)
	ਵਾਪਸ get_list_type(member_type, type_gap);
    ਵਾਪਸ get_dict_type(member_type, type_gap);
}

/*
 * Parse a "func" type at "*arg" and advance over it.
 * When "give_error" is TRUE give error messages, otherwise be quiet.
 * Return NULL for failure.
 */
    ਸਥਿਰ type_T *
parse_type_func(
    char_u	**arg,
    size_t	len,
    garray_T	*type_gap,
    ਪੂਰਨ		give_error,
    ufunc_T	*ufunc,
    cctx_T	*cctx)
{
    char_u  *p;
    type_T  *type;
    type_T  *ret_type = &t_unknown;
    ਪੂਰਨ	argcount = -1;
    ਪੂਰਨ	flags = 0;
    ਪੂਰਨ	ਪਹਿਲਾ_optional = -1;
    type_T	*arg_type[MAX_FUNC_ਦਲੀਲਾਂ + 1];

    // func({type}, ...{type}): {type}
    *arg += len;
    ਜੇ (**arg == '(')
    {
	// "func" may or may not return a value, "func()" does
	// not return a value.
	ret_type = &t_void;

	p = ++*arg;
	argcount = 0;
	ਜਦੋਂ (*p != NUL && *p != ')')
	{
	    ਜੇ (*p == '?')
	    {
		ਜੇ (ਪਹਿਲਾ_optional == -1)
		    ਪਹਿਲਾ_optional = argcount;
		++p;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "...", 3) == 0)
	    {
		flags |= TTFLAG_VARARGS;
		p += 3;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (ਪਹਿਲਾ_optional != -1)
	    {
		ਜੇ (give_error)
		    emsg(_(e_mandatory_ਦਲੀਲ_after_optional_ਦਲੀਲ));
		ਵਾਪਸ ਨਿਰਾ;
	    }

	    type = parse_type(&p, type_gap, ufunc, cctx, give_error);
	    ਜੇ (type == ਨਿਰਾ || !valid_declaration_type(type))
		ਵਾਪਸ ਨਿਰਾ;
	    ਜੇ ((flags & TTFLAG_VARARGS) != 0 && type->tt_type != VAR_LIST)
	    {
		ਅੱਖਰ *tofree;
		semsg(_(e_variable_arguments_type_must_be_list_str),
			type_name(type, &tofree));
		vim_free(tofree);
		ਵਾਪਸ ਨਿਰਾ;
	    }
	    arg_type[argcount++] = type;

	    // Nothing comes after "...{type}".
	    ਜੇ (flags & TTFLAG_VARARGS)
		ਰੋਕੋ;

	    ਜੇ (*p != ',' && *skipwhite(p) == ',')
	    {
		ਜੇ (give_error)
		    semsg(_(e_no_white_space_allowed_before_str_str),
			    ",", p);
		ਵਾਪਸ ਨਿਰਾ;
	    }
	    ਜੇ (*p == ',')
	    {
		++p;
		ਜੇ (!VIM_ISWHITE(*p))
		{
		    ਜੇ (give_error)
			semsg(_(e_white_space_required_after_str_str),
				",", p - 1);
		    ਵਾਪਸ ਨਿਰਾ;
		}
	    }
	    p = skipwhite(p);
	    ਜੇ (argcount == MAX_FUNC_ਦਲੀਲਾਂ)
	    {
		ਜੇ (give_error)
		    emsg(_(e_too_many_ਦਲੀਲ_types));
		ਵਾਪਸ ਨਿਰਾ;
	    }
	}

	p = skipwhite(p);
	ਜੇ (*p != ')')
	{
	    ਜੇ (give_error)
		emsg(_(e_missing_closing_paren));
	    ਵਾਪਸ ਨਿਰਾ;
	}
	*arg = p + 1;
    }
    ਜੇ (**arg == ':')
    {
	// parse return type
	++*arg;
	ਜੇ (!VIM_ISWHITE(**arg) && give_error)
	    semsg(_(e_white_space_required_after_str_str), ":", *arg - 1);
	*arg = skipwhite(*arg);
	ret_type = parse_type(arg, type_gap, ufunc, cctx, give_error);
	ਜੇ (ret_type == ਨਿਰਾ)
	    ਵਾਪਸ ਨਿਰਾ;
    }
    ਜੇ (flags == 0 && ਪਹਿਲਾ_optional == -1 && argcount <= 0)
	type = get_func_type(ret_type, argcount, type_gap);
    ਨਹੀਂਤਾਂ
    {
	type = alloc_func_type(ret_type, argcount, type_gap);
	type->tt_flags = flags;
	ਜੇ (argcount > 0)
	{
	    type->tt_argcount = argcount;
	    type->tt_min_argcount = ਪਹਿਲਾ_optional == -1
						? argcount : ਪਹਿਲਾ_optional;
	    ਜੇ (func_type_ਜੋੜੋ_arg_types(type, argcount, type_gap) == FAIL)
		ਵਾਪਸ ਨਿਰਾ;
	    mch_memmove(type->tt_ਦਲੀਲਾਂ, arg_type, ਆਕਾਰ(type_T *) * argcount);
	}
    }

    ਵਾਪਸ type;
}

/*
 * Parse a "tuple" type at "*arg" and advance over it.
 * When "give_error" is TRUE give error messages, otherwise be quiet.
 * Return NULL for failure.
 */
    ਸਥਿਰ type_T *
parse_type_tuple(
    char_u	**arg,
    garray_T	*type_gap,
    ਪੂਰਨ		give_error,
    ufunc_T	*ufunc,
    cctx_T	*cctx)
{
    char_u	*p;
    type_T	*type;
    type_T	*ret_type = ਨਿਰਾ;
    ਪੂਰਨ		typecount = -1;
    ਪੂਰਨ		flags = 0;
    garray_T	tuple_types_ga;

    ga_init2(&tuple_types_ga, ਆਕਾਰ(type_T *), 10);

    // tuple<{type}, {type}>
    // tuple<{type}, ...{type}>
    ਜੇ (**arg != '<')
    {
	ਜੇ (give_error)
	{
	    ਜੇ (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    ਨਹੀਂਤਾਂ
		semsg(_(e_missing_type_after_str), "tuple");
	}

	// only "tuple" is specified
	ਵਾਪਸ ਨਿਰਾ;
    }

    p = ++*arg;
    typecount = 0;
    ਜਦੋਂ (*p != NUL && *p != '>')
    {
	ਜੇ (STRNCMP(p, "...", 3) == 0)
	{
	    flags |= TTFLAG_VARARGS;
	    p += 3;
	}

	type = parse_type(&p, type_gap, ufunc, cctx, give_error);
	ਜੇ (type == ਨਿਰਾ || !valid_declaration_type(type))
	    ਜਾਓ on_err;

	ਜੇ ((flags & TTFLAG_VARARGS) != 0 && type->tt_type != VAR_LIST)
	{
	    ਅੱਖਰ *tofree;
	    semsg(_(e_variadic_tuple_must_ਸਿਰਾ_with_list_type_str),
						type_name(type, &tofree));
	    vim_free(tofree);
	    ਜਾਓ on_err;
	}

	// Add the item type
	ਜੇ (ga_grow(&tuple_types_ga, 1) == FAIL)
	    ਜਾਓ on_err;
	((type_T **)tuple_types_ga.ga_ਅੰਕੜੇ)[tuple_types_ga.ga_len] = type;
	tuple_types_ga.ga_len++;
	typecount++;

	// Nothing comes after "...{type}".
	ਜੇ (flags & TTFLAG_VARARGS)
	    ਰੋਕੋ;

	ਜੇ (*p != ',' && *skipwhite(p) == ',')
	{
	    ਜੇ (give_error)
		semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
	    ਜਾਓ on_err;
	}
	ਜੇ (*p == ',')
	{
	    ++p;
	    ਜੇ (!VIM_ISWHITE(*p))
	    {
		ਜੇ (give_error)
		    semsg(_(e_white_space_required_after_str_str),
			    ",", p - 1);
		ਜਾਓ on_err;
	    }
	}
	p = skipwhite(p);
    }

    p = skipwhite(p);
    ਜੇ (*p != '>' || typecount <= 0)
    {
	ਜੇ (give_error)
	    semsg(_(e_missing_type_after_str), p);
	ਜਾਓ on_err;
    }
    *arg = p + 1;

    ret_type = alloc_tuple_type(typecount, type_gap);
    ਜੇ (ret_type == &t_any
	    || tuple_type_ਜੋੜੋ_types(ret_type, typecount, type_gap) == FAIL)
    {
	ret_type = ਨਿਰਾ;
	ਜਾਓ on_err;
    }
    ret_type->tt_flags = flags;
    ret_type->tt_argcount = typecount;
    mch_memmove(ret_type->tt_ਦਲੀਲਾਂ, tuple_types_ga.ga_ਅੰਕੜੇ,
						ਆਕਾਰ(type_T *) * typecount);

on_err:
    ga_clear(&tuple_types_ga);

    ਵਾਪਸ ret_type;
}

/*
 * Parse a "object" type at "*arg" and advance over it.
 * When "give_error" is TRUE give error messages, otherwise be quiet.
 * Return NULL for failure.
 */
    ਸਥਿਰ type_T *
parse_type_object(
    char_u	**arg,
    garray_T	*type_gap,
    ਪੂਰਨ		give_error,
    cctx_T	*cctx)
{
    char_u	*arg_ਸ਼ੁਰੂ = *arg;
    type_T	*object_type;
    ਪੂਰਨ		prev_called_emsg = called_emsg;

    // object<X> or object<any>
    ਜੇ (**arg != '<')
    {
	ਜੇ (give_error)
	{
	    ਜੇ (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    ਨਹੀਂਤਾਂ
		semsg(_(e_missing_type_after_str), "object");
	}

	// only "object" is specified
	ਵਾਪਸ ਨਿਰਾ;
    }

    // skip spaces following "object<"
    *arg = skipwhite(*arg + 1);

    object_type = parse_type(arg, type_gap, ਨਿਰਾ, cctx, give_error);
    ਜੇ (object_type == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    *arg = skipwhite(*arg);
    ਜੇ (**arg != '>' && called_emsg == prev_called_emsg)
    {
	ਜੇ (give_error)
	    semsg(_(e_missing_gt_after_type_str), arg_ਸ਼ੁਰੂ);
	ਵਾਪਸ ਨਿਰਾ;
    }
    ++*arg;

    ਜੇ (object_type->tt_type == VAR_ANY)
	ਵਾਪਸ &t_object_any;

    ਜੇ (object_type->tt_type != VAR_OBJECT)
    {
	// specified type is not a class
	ਜੇ (give_error)
	    semsg(_(e_class_name_not_ਮਿਲਿਆ_str), arg_ਸ਼ੁਰੂ);
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਵਾਪਸ object_type;
}

/*
 * Parse a user defined type at "*arg" and advance over it.
 * It can be a class or an interface or a typealias name, possibly imported.
 * Return NULL if a type is not found.
 */
    ਸਥਿਰ type_T *
parse_type_user_defined(
    char_u	**arg,
    size_t	len,
    garray_T	*type_gap,
    ਪੂਰਨ		give_error,
    ufunc_T	*ufunc,
    cctx_T	*cctx)
{
    ਪੂਰਨ		did_emsg_before = did_emsg;
    typval_T	tv;

    tv.v_type = VAR_UNKNOWN;
    ਜੇ (eval_variable_import(*arg, &tv) == OK)
    {
	ਜੇ (tv.v_type == VAR_CLASS && tv.vval.v_class != ਨਿਰਾ)
	{
	    type_T *type = get_type_ptr(type_gap);
	    ਜੇ (type != ਨਿਰਾ)
	    {
		// Although the name is that of a class or interface, the type
		// uses will be an object.
		type->tt_type = VAR_OBJECT;
		type->tt_class = tv.vval.v_class;
		clear_tv(&tv);

		*arg += len;
		// Skip over ".ClassName".
		ਜਦੋਂ (ASCII_ISALNUM(**arg) || **arg == '_' || **arg == '.')
		    ++*arg;

		ਵਾਪਸ type;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (tv.v_type == VAR_TYPEALIAS)
	{
	    // user defined type
	    // The type alias may itself be a nested type, check the resulting
	    // nesting depth.
	    ਪੂਰਨ	ਡੂੰਘਾਈ = parse_ਡੂੰਘਾਈ - 1
				+ MAX(tv.vval.v_typealias->ta_ਡੂੰਘਾਈ, 1);
	    ਜੇ (ਡੂੰਘਾਈ - 1 > MAX_TYPE_ਡੂੰਘਾਈ)
	    {
		clear_tv(&tv);
		ਜੇ (give_error)
		    emsg(_(e_type_nested_too_deep));
		ਵਾਪਸ ਨਿਰਾ;
	    }
	    ਜੇ (ਡੂੰਘਾਈ > parse_max_ਡੂੰਘਾਈ)
		parse_max_ਡੂੰਘਾਈ = ਡੂੰਘਾਈ;

	    type_T *type = copy_type(tv.vval.v_typealias->ta_type, type_gap);
	    *arg += len;
	    clear_tv(&tv);
	    // Skip over ".TypeName".
	    ਜਦੋਂ (ASCII_ISALNUM(**arg) || **arg == '_' || **arg == '.')
		++*arg;
	    ਵਾਪਸ type;
	}

	clear_tv(&tv);
    }

    // Check whether it is a generic type
    type_T *type = ਲੱਭੋ_generic_type(*arg, len, ufunc, cctx);
    ਜੇ (type != ਨਿਰਾ)
    {
	*arg += len;
	ਵਾਪਸ type;
    }

    ਜੇ (give_error && (did_emsg == did_emsg_before))
    {
	char_u	*p = skip_type(*arg, FALSE);
	ਅੱਖਰ	cc = *p;

	*p = NUL;
	semsg(_(e_type_not_recognized_str), *arg);
	*p = cc;
    }

    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Inner part of parse_type().
 */
    ਸਥਿਰ type_T *
parse_type_inner(
    char_u	**arg,
    garray_T	*type_gap,
    ufunc_T	*ufunc,
    cctx_T	*cctx,
    ਪੂਰਨ		give_error)
{
    char_u  *p = *arg;
    size_t  len;

    // Skip over the first word.
    ਜਦੋਂ (ASCII_ISALNUM(*p) || *p == '_')
	++p;
    len = p - *arg;

    ਚੋਣ (**arg)
    {
	ਹਾਲਤ 'a':
	    ਜੇ (len == 3 && STRNCMP(*arg, "any", len) == 0)
	    {
		*arg += len;
		ਵਾਪਸ &t_any;
	    }
	    ਰੋਕੋ;
	ਹਾਲਤ 'b':
	    ਜੇ (len == 4 && STRNCMP(*arg, "bool", len) == 0)
	    {
		*arg += len;
		ਵਾਪਸ &t_bool;
	    }
	    ਜੇ (len == 4 && STRNCMP(*arg, "blob", len) == 0)
	    {
		*arg += len;
		ਵਾਪਸ &t_blob;
	    }
	    ਰੋਕੋ;
	ਹਾਲਤ 'c':
	    ਜੇ (len == 7 && STRNCMP(*arg, "channel", len) == 0)
	    {
		*arg += len;
		ਵਾਪਸ &t_channel;
	    }
	    ਰੋਕੋ;
	ਹਾਲਤ 'd':
	    ਜੇ (len == 4 && STRNCMP(*arg, "dict", len) == 0)
	    {
		*arg += len;
		ਵਾਪਸ parse_type_member(arg, &t_dict_any, type_gap,
						give_error, "dict", ufunc,
						cctx);
	    }
	    ਰੋਕੋ;
	ਹਾਲਤ 'f':
	    ਜੇ (len == 5 && STRNCMP(*arg, "float", len) == 0)
	    {
		*arg += len;
		ਵਾਪਸ &t_float;
	    }
	    ਜੇ (len == 4 && STRNCMP(*arg, "func", len) == 0)
		ਵਾਪਸ parse_type_func(arg, len, type_gap, give_error, ufunc,
									cctx);
	    ਰੋਕੋ;
	ਹਾਲਤ 'j':
	    ਜੇ (len == 3 && STRNCMP(*arg, "job", len) == 0)
	    {
		*arg += len;
		ਵਾਪਸ &t_ਕੰਮ;
	    }
	    ਰੋਕੋ;
	ਹਾਲਤ 'l':
	    ਜੇ (len == 4 && STRNCMP(*arg, "list", len) == 0)
	    {
		*arg += len;
		ਵਾਪਸ parse_type_member(arg, &t_list_any, type_gap,
						give_error, "list", ufunc,
						cctx);
	    }
	    ਰੋਕੋ;
	ਹਾਲਤ 'n':
	    ਜੇ (len == 6 && STRNCMP(*arg, "number", len) == 0)
	    {
		*arg += len;
		ਵਾਪਸ &t_ਸੰਖਿਆ;
	    }
	    ਰੋਕੋ;
	ਹਾਲਤ 'o':
	    ਜੇ (len == 6 && STRNCMP(*arg, "object", len) == 0)
	    {
		*arg += len;
		ਵਾਪਸ parse_type_object(arg, type_gap, give_error, cctx);
	    }
	    ਰੋਕੋ;
	ਹਾਲਤ 's':
	    ਜੇ (len == 6 && STRNCMP(*arg, "string", len) == 0)
	    {
		*arg += len;
		ਵਾਪਸ &t_string;
	    }
	    ਰੋਕੋ;
	ਹਾਲਤ 't':
	    ਜੇ (len == 5 && STRNCMP(*arg, "tuple", len) == 0)
	    {
		*arg += len;
		ਵਾਪਸ parse_type_tuple(arg, type_gap, give_error, ufunc,
									cctx);
	    }
	    ਰੋਕੋ;
	ਹਾਲਤ 'v':
	    ਜੇ (len == 4 && STRNCMP(*arg, "void", len) == 0)
	    {
		*arg += len;
		ਵਾਪਸ &t_void;
	    }
	    ਰੋਕੋ;
    }

    // User defined type
    ਵਾਪਸ parse_type_user_defined(arg, len, type_gap, give_error, ufunc,
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
    ਪੂਰਨ		give_error)
{
    type_T	*type;

    ਜੇ (parse_ਡੂੰਘਾਈ > MAX_TYPE_ਡੂੰਘਾਈ)
    {
	ਜੇ (give_error)
	    emsg(_(e_type_nested_too_deep));
	ਵਾਪਸ ਨਿਰਾ;
    }
    ਜੇ (parse_ਡੂੰਘਾਈ == 0)
	parse_max_ਡੂੰਘਾਈ = 0;
    ++parse_ਡੂੰਘਾਈ;
    ਜੇ (parse_ਡੂੰਘਾਈ > parse_max_ਡੂੰਘਾਈ)
	parse_max_ਡੂੰਘਾਈ = parse_ਡੂੰਘਾਈ;
    type = parse_type_inner(arg, type_gap, ufunc, cctx, give_error);
    --parse_ਡੂੰਘਾਈ;
    ਵਾਪਸ type;
}

/*
 * Return the number of levels of the type last parsed by parse_type().
 */
    ਪੂਰਨ
parse_type_ਡੂੰਘਾਈ(ਖਾਲੀ)
{
    ਵਾਪਸ parse_max_ਡੂੰਘਾਈ;
}

/*
 * Check if "type1" and "type2" are exactly the same.
 * "flags" can have ETYPE_ARG_UNKNOWN, which means that an unknown argument
 * type in "type1" is accepted.
 */
    ਪੂਰਨ
equal_type(type_T *type1, type_T *type2, ਪੂਰਨ flags)
{
    ਪੂਰਨ i;

    ਜੇ (type1 == ਨਿਰਾ || type2 == ਨਿਰਾ)
	ਵਾਪਸ FALSE;
    ਜੇ (type1->tt_type != type2->tt_type)
	ਵਾਪਸ FALSE;
    ਚੋਣ (type1->tt_type)
    {
	ਹਾਲਤ VAR_UNKNOWN:
	ਹਾਲਤ VAR_ANY:
	ਹਾਲਤ VAR_VOID:
	ਹਾਲਤ VAR_SPECIAL:
	ਹਾਲਤ VAR_BOOL:
	ਹਾਲਤ VAR_ਸੰਖਿਆ:
	ਹਾਲਤ VAR_FLOAT:
	ਹਾਲਤ VAR_STRING:
	ਹਾਲਤ VAR_BLOB:
	ਹਾਲਤ VAR_ਕੰਮ:
	ਹਾਲਤ VAR_CHANNEL:
	ਹਾਲਤ VAR_INSTR:
	ਹਾਲਤ VAR_TYPEALIAS:
	    ਰੋਕੋ;  // not composite is always OK
	ਹਾਲਤ VAR_OBJECT:
	ਹਾਲਤ VAR_CLASS:
	    // Objects are considered equal if they are from the same class
	    ਵਾਪਸ type1->tt_class == type2->tt_class;
	ਹਾਲਤ VAR_LIST:
	ਹਾਲਤ VAR_DICT:
	    ਵਾਪਸ equal_type(type1->tt_member, type2->tt_member, flags);
	ਹਾਲਤ VAR_TUPLE:
	    ਜੇ (type1->tt_argcount != type2->tt_argcount)
		ਵਾਪਸ FALSE;
	    ਜੇ (type1->tt_argcount < 0
			   || type1->tt_ਦਲੀਲਾਂ == ਨਿਰਾ || type2->tt_ਦਲੀਲਾਂ == ਨਿਰਾ)
		ਵਾਪਸ TRUE;
	    ਲਈ (i = 0; i < type1->tt_argcount; ++i)
		ਜੇ ((flags & ETYPE_ARG_UNKNOWN) == 0
			&& !equal_type(type1->tt_ਦਲੀਲਾਂ[i], type2->tt_ਦਲੀਲਾਂ[i],
									flags))
		    ਵਾਪਸ FALSE;
	    ਵਾਪਸ TRUE;
	ਹਾਲਤ VAR_FUNC:
	ਹਾਲਤ VAR_PARTIAL:
	    ਜੇ (!equal_type(type1->tt_member, type2->tt_member, flags)
		    || type1->tt_argcount != type2->tt_argcount)
		ਵਾਪਸ FALSE;
	    ਜੇ (type1->tt_argcount < 0
			   || type1->tt_ਦਲੀਲਾਂ == ਨਿਰਾ || type2->tt_ਦਲੀਲਾਂ == ਨਿਰਾ)
		ਵਾਪਸ TRUE;
	    ਲਈ (i = 0; i < type1->tt_argcount; ++i)
		ਜੇ ((flags & ETYPE_ARG_UNKNOWN) == 0
			&& !equal_type(type1->tt_ਦਲੀਲਾਂ[i], type2->tt_ਦਲੀਲਾਂ[i],
									flags))
		    ਵਾਪਸ FALSE;
	    ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ TRUE;
}

/*
 * Find the common type of "type1" (VAR_FUNC) and "type2" (VAR_FUNC) and put it
 * in "dest".  "type2" and "dest" may be the same.
 */
    ਸਥਿਰ ਖਾਲੀ
common_type_var_func(
    type_T	*type1,
    type_T	*type2,
    type_T	**dest,
    garray_T	*type_gap)
{
    type_T *common;

    // When one of the types is t_func_unknown return the other one.
    // Useful if a list or dict item is null_func.
    ਜੇ (type1 == &t_func_unknown)
    {
	*dest = type2;
	ਵਾਪਸ;
    }
    ਜੇ (type2 == &t_func_unknown)
    {
	*dest = type1;
	ਵਾਪਸ;
    }

    common_type(type1->tt_member, type2->tt_member, &common, type_gap);
    ਜੇ (type1->tt_argcount == type2->tt_argcount
	    && type1->tt_argcount >= 0)
    {
	ਪੂਰਨ argcount = type1->tt_argcount;
	ਪੂਰਨ i;

	*dest = alloc_func_type(common, argcount, type_gap);
	ਜੇ (type1->tt_ਦਲੀਲਾਂ != ਨਿਰਾ && type2->tt_ਦਲੀਲਾਂ != ਨਿਰਾ)
	{
	    ਜੇ (func_type_ਜੋੜੋ_arg_types(*dest, argcount,
			type_gap) == OK)
		ਲਈ (i = 0; i < argcount; ++i)
		    common_type(type1->tt_ਦਲੀਲਾਂ[i], type2->tt_ਦਲੀਲਾਂ[i],
			    &(*dest)->tt_ਦਲੀਲਾਂ[i], type_gap);
	}
    }
    ਨਹੀਂਤਾਂ
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
    ਖਾਲੀ
common_type(type_T *type1, type_T *type2, type_T **dest, garray_T *type_gap)
{
    ਜੇ (equal_type(type1, type2, 0))
    {
	*dest = type1;
	ਵਾਪਸ;
    }

    // If either is VAR_UNKNOWN use the other type.  An empty list/dict has no
    // specific type.
    ਜੇ (type1 == ਨਿਰਾ || type1->tt_type == VAR_UNKNOWN)
    {
	*dest = type2;
	ਵਾਪਸ;
    }
    ਜੇ (type2 == ਨਿਰਾ || type2->tt_type == VAR_UNKNOWN)
    {
	*dest = type1;
	ਵਾਪਸ;
    }

    ਜੇ (type1->tt_type == type2->tt_type)
    {
	ਜੇ (type1->tt_type == VAR_LIST
		|| type1->tt_type == VAR_DICT)
	{
	    type_T *common;

	    common_type(type1->tt_member, type2->tt_member, &common, type_gap);
	    ਜੇ (type1->tt_type == VAR_LIST)
		*dest = get_list_type(common, type_gap);
	    ਨਹੀਂਤਾਂ
		*dest = get_dict_type(common, type_gap);
	    ਵਾਪਸ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (type1->tt_type == VAR_FUNC)
	{
	    common_type_var_func(type1, type2, dest, type_gap);
	    ਵਾਪਸ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (type1->tt_type == VAR_OBJECT)
	{
	    *dest = &t_object_any;
	    ਵਾਪਸ;
	}
    }

    *dest = &t_any;
}

/*
 * Return the item type of a List, Dict or a Tuple
 */
    type_T *
get_ਵਸਤੂ_type(type_T *type)
{
    ਜੇ (type->tt_type == VAR_TUPLE)
    {
	ਜੇ (type->tt_argcount != 1)
	    ਵਾਪਸ &t_any;

	ਜੇ (type->tt_flags & TTFLAG_VARARGS)
	    ਵਾਪਸ type->tt_ਦਲੀਲਾਂ[0]->tt_member;
	ਨਹੀਂਤਾਂ
	    ਵਾਪਸ type->tt_ਦਲੀਲਾਂ[0];
    }

    // For "any" the item type is not known.
    ਵਾਪਸ type->tt_member == ਨਿਰਾ ? &t_any : type->tt_member;
}

/*
 * Push an entry onto the type stack.  "type" used both for the current type
 * and the declared type.
 * Returns FAIL when out of memory.
 */
    ਪੂਰਨ
ਧੱਕੋ_type_ਢੇਰ(cctx_T *cctx, type_T *type)
{
    ਵਾਪਸ ਧੱਕੋ_type_stack2(cctx, type, type);
}

/*
 * Push an entry onto the type stack.  "type" is the current type, "decl_type"
 * is the declared type.
 * Returns FAIL when out of memory.
 */
    ਪੂਰਨ
ਧੱਕੋ_type_stack2(cctx_T *cctx, type_T *type, type_T *decl_type)
{
    garray_T	*ਢੇਰ = &cctx->ctx_type_ਢੇਰ;
    type2_T	*typep;

    ਜੇ (GA_GROW_FAILS(ਢੇਰ, 1))
	ਵਾਪਸ FAIL;
    typep = ((type2_T *)ਢੇਰ->ga_ਅੰਕੜੇ) + ਢੇਰ->ga_len;
    typep->type_curr = type;
    typep->type_decl = decl_type;
    ++ਢੇਰ->ga_len;
    ਵਾਪਸ OK;
}

/*
 * Set the type of the top of the stack to "type".
 */
    ਖਾਲੀ
ਸੈੱਟ_type_on_ਢੇਰ(cctx_T *cctx, type_T *type, ਪੂਰਨ ਵਿੱਥ)
{
    garray_T	*ਢੇਰ = &cctx->ctx_type_ਢੇਰ;
    type2_T	*typep = ((type2_T *)ਢੇਰ->ga_ਅੰਕੜੇ)
						  + ਢੇਰ->ga_len - 1 - ਵਿੱਥ;

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
get_type_on_ਢੇਰ(cctx_T *cctx, ਪੂਰਨ ਵਿੱਥ)
{
    garray_T	*ਢੇਰ = &cctx->ctx_type_ਢੇਰ;

    ਜੇ (ਵਿੱਥ + 1 > ਢੇਰ->ga_len)
	ਵਾਪਸ &t_unknown;
    ਵਾਪਸ (((type2_T *)ਢੇਰ->ga_ਅੰਕੜੇ) + ਢੇਰ->ga_len - ਵਿੱਥ - 1)
								   ->type_curr;
}

/*
 * Get the declared type from the type stack.  If "offset" is zero the one at
 * the top,
 * if "offset" is one the type above that, etc.
 * Returns &t_unknown if there is no such stack entry.
 */
    type_T *
get_decl_type_on_ਢੇਰ(cctx_T *cctx, ਪੂਰਨ ਵਿੱਥ)
{
    garray_T	*ਢੇਰ = &cctx->ctx_type_ਢੇਰ;

    ਜੇ (ਵਿੱਥ + 1 > ਢੇਰ->ga_len)
	ਵਾਪਸ &t_unknown;
    ਵਾਪਸ (((type2_T *)ਢੇਰ->ga_ਅੰਕੜੇ) + ਢੇਰ->ga_len - ਵਿੱਥ - 1)
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
get_member_type_from_ਢੇਰ(
	ਪੂਰਨ	    ਗਿਣਤੀ_2,
	ਪੂਰਨ	    skip,
	cctx_T	    *cctx)
{
    garray_T	*ਢੇਰ = &cctx->ctx_type_ਢੇਰ;
    type2_T	*typep;
    garray_T    *type_gap = cctx->ctx_type_list;
    ਪੂਰਨ		i;
    type_T	*ਨਤੀਜਾ;
    type_T	*type;

    // Use "unknown" for an empty list or dict.
    ਜੇ (ਗਿਣਤੀ_2 == 0)
	ਵਾਪਸ &t_unknown;
    // Find the common type from following items.
    typep = ((type2_T *)ਢੇਰ->ga_ਅੰਕੜੇ) + ਢੇਰ->ga_len;
    ਨਤੀਜਾ = &t_unknown;
    ਲਈ (i = 0; i < ਗਿਣਤੀ_2; ++i)
    {
	type = (typep -((ਗਿਣਤੀ_2 - i) * skip) + skip - 1)->type_curr;
	ਜੇ (check_type_is_ਮੁੱਲ(type) == FAIL)
	    ਵਾਪਸ ਨਿਰਾ;
	ਜੇ (ਨਤੀਜਾ != &t_any)
	    common_type(type, ਨਤੀਜਾ, &ਨਤੀਜਾ, type_gap);
    }

    ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Get the types of items in a tuple on the stack of "cctx".
 * Returns the number of types.  Returns -1 on failure.
 */
    ਪੂਰਨ
get_tuple_type_from_ਢੇਰ(
    ਪੂਰਨ		ਗਿਣਤੀ_2,
    garray_T	*tuple_types_gap,
    cctx_T	*cctx)
{
    garray_T	*ਢੇਰ = &cctx->ctx_type_ਢੇਰ;
    type2_T	*typep;
    type_T	*type = ਨਿਰਾ;

    // Use "unknown" for an empty tuple
    ਜੇ (ਗਿਣਤੀ_2 == 0)
	ਵਾਪਸ 0;

    // Find the common type from following items.
    typep = ((type2_T *)ਢੇਰ->ga_ਅੰਕੜੇ) + ਢੇਰ->ga_len;
    ਲਈ (ਪੂਰਨ i = 0; i < ਗਿਣਤੀ_2; i++)
    {
	type = (typep - (ਗਿਣਤੀ_2 - i))->type_curr;
	ਜੇ (check_type_is_ਮੁੱਲ(type) == FAIL)
	    ਵਾਪਸ -1;
	ਜੇ (ga_grow(tuple_types_gap, 1) == FAIL)
	    ਵਾਪਸ -1;
	((type_T **)tuple_types_gap->ga_ਅੰਕੜੇ)[tuple_types_gap->ga_len] = type;
	tuple_types_gap->ga_len++;
    }

    ਵਾਪਸ tuple_types_gap->ga_len;
}

    ਅੱਖਰ *
vartype_name(vartype_T type)
{
    ਚੋਣ (type)
    {
	ਹਾਲਤ VAR_UNKNOWN: ਰੋਕੋ;
	ਹਾਲਤ VAR_ANY: ਵਾਪਸ "any";
	ਹਾਲਤ VAR_VOID: ਵਾਪਸ "void";
	ਹਾਲਤ VAR_SPECIAL: ਵਾਪਸ "special";
	ਹਾਲਤ VAR_BOOL: ਵਾਪਸ "bool";
	ਹਾਲਤ VAR_ਸੰਖਿਆ: ਵਾਪਸ "number";
	ਹਾਲਤ VAR_FLOAT: ਵਾਪਸ "float";
	ਹਾਲਤ VAR_STRING: ਵਾਪਸ "string";
	ਹਾਲਤ VAR_BLOB: ਵਾਪਸ "blob";
	ਹਾਲਤ VAR_ਕੰਮ: ਵਾਪਸ "job";
	ਹਾਲਤ VAR_CHANNEL: ਵਾਪਸ "channel";
	ਹਾਲਤ VAR_LIST: ਵਾਪਸ "list";
	ਹਾਲਤ VAR_TUPLE: ਵਾਪਸ "tuple";
	ਹਾਲਤ VAR_DICT: ਵਾਪਸ "dict";
	ਹਾਲਤ VAR_INSTR: ਵਾਪਸ "instr";
	ਹਾਲਤ VAR_CLASS: ਵਾਪਸ "class";
	ਹਾਲਤ VAR_OBJECT: ਵਾਪਸ "object";
	ਹਾਲਤ VAR_TYPEALIAS: ਵਾਪਸ "typealias";

	ਹਾਲਤ VAR_FUNC:
	ਹਾਲਤ VAR_PARTIAL: ਵਾਪਸ "func";
    }
    ਵਾਪਸ "unknown";
}

/*
 * Return the type name of a List (list<type>) or Dict (dict<type>).
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    ਸਥਿਰ ਅੱਖਰ *
type_name_list_or_dict(ਅੱਖਰ *name, type_T *type, ਅੱਖਰ **tofree)
{
    ਅੱਖਰ *member_free;
    ਅੱਖਰ *member_name;

    ਜੇ (type->tt_member->tt_type == VAR_UNKNOWN)
	member_name = type_name(&t_any, &member_free);
    ਨਹੀਂਤਾਂ
	member_name = type_name(type->tt_member, &member_free);

    size_t len = STRLEN(name) + STRLEN(member_name) + 3;
    *tofree = alloc(len);
    ਜੇ (*tofree == ਨਿਰਾ)
    {
	vim_free(member_free);
	ਵਾਪਸ name;
    }

    vim_snprintf(*tofree, len, "%s<%s>", name, member_name);
    vim_free(member_free);
    ਵਾਪਸ *tofree;
}

/*
 * Return the type name of a tuple.
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    ਸਥਿਰ ਅੱਖਰ *
type_name_tuple(type_T *type, ਅੱਖਰ **tofree)
{
    garray_T    ga;
    ਪੂਰਨ		i;
    ਪੂਰਨ		varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;
    ਅੱਖਰ	*arg_free = ਨਿਰਾ;

    ga_init2(&ga, 1, 100);
    ਜੇ (ga_grow(&ga, 20) == FAIL)
	ਜਾਓ failed;
    STRCPY(ga.ga_ਅੰਕੜੇ, "tuple<");
    ga.ga_len += 6;

    ਜੇ (type->tt_argcount <= 0)
	// empty tuple
	GA_CONCAT_LITERAL(&ga, "any");
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (type->tt_ਦਲੀਲਾਂ == ਨਿਰਾ)
	    GA_CONCAT_LITERAL(&ga, "[unknown]");
	ਨਹੀਂਤਾਂ
	{
	    ਲਈ (i = 0; i < type->tt_argcount; ++i)
	    {
		string_T    arg_type;

		arg_type.string = (char_u *)type_name(type->tt_ਦਲੀਲਾਂ[i], &arg_free);
		ਜੇ (i > 0)
		{
		    STRCPY((ਅੱਖਰ *)ga.ga_ਅੰਕੜੇ + ga.ga_len, ", ");
		    ga.ga_len += 2;
		}
		arg_type.ਲੰਮਾਈ = STRLEN(arg_type.string);
		ਜੇ (ga_grow(&ga, (ਪੂਰਨ)arg_type.ਲੰਮਾਈ + 8) == FAIL)
		    ਜਾਓ failed;
		ਜੇ (varargs && i == type->tt_argcount - 1)
		    GA_CONCAT_LITERAL(&ga, "...");
		ga_concat_len(&ga, arg_type.string, arg_type.ਲੰਮਾਈ);
		VIM_CLEAR(arg_free);
	    }
	}
    }

    STRCPY((ਅੱਖਰ *)ga.ga_ਅੰਕੜੇ + ga.ga_len, ">");
    *tofree = ga.ga_ਅੰਕੜੇ;
    ਵਾਪਸ ga.ga_ਅੰਕੜੇ;

failed:
    vim_free(arg_free);
    ga_clear(&ga);
    ਵਾਪਸ "[unknown]";
}

/*
 * Return the type name of a Class (class<name>) or Object (object<name>).
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    ਸਥਿਰ ਅੱਖਰ *
type_name_class_or_obj(ਅੱਖਰ *name, type_T *type, ਅੱਖਰ **tofree)
{
    string_T	class_name;

    ਜੇ (type->tt_class != ਨਿਰਾ)
    {
	class_name.string = type->tt_class->class_name.string;
	class_name.ਲੰਮਾਈ = type->tt_class->class_name.ਲੰਮਾਈ;
	ਜੇ (IS_ENUM(type->tt_class))
	    name = "enum";
    }
    ਨਹੀਂਤਾਂ
	STR_LITERAL_ਸੈੱਟ(class_name, "any");

    size_t len = STRLEN(name) + class_name.ਲੰਮਾਈ + 3;
    *tofree = alloc(len);
    ਜੇ (*tofree == ਨਿਰਾ)
	ਵਾਪਸ name;

    vim_snprintf(*tofree, len, "%s<%s>", name, class_name.string);
    ਵਾਪਸ *tofree;
}

/*
 * Return the type name of a function.
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    ਸਥਿਰ ਅੱਖਰ *
type_name_func(type_T *type, ਅੱਖਰ **tofree)
{
    garray_T    ga;
    ਪੂਰਨ		i;
    ਪੂਰਨ		varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;
    ਅੱਖਰ	*arg_free = ਨਿਰਾ;

    ga_init2(&ga, 1, 100);
    ਜੇ (ga_grow(&ga, 20) == FAIL)
	ਜਾਓ failed;
    STRCPY(ga.ga_ਅੰਕੜੇ, "func(");
    ga.ga_len += 5;

    ਲਈ (i = 0; i < type->tt_argcount; ++i)
    {
	string_T    arg_type;

	ਜੇ (type->tt_ਦਲੀਲਾਂ == ਨਿਰਾ)
	    STR_LITERAL_ਸੈੱਟ(arg_type, "[unknown]");
	ਨਹੀਂਤਾਂ
	{
	    arg_type.string = (char_u *)type_name(type->tt_ਦਲੀਲਾਂ[i], &arg_free);
	    arg_type.ਲੰਮਾਈ = STRLEN(arg_type.string);
	}
	ਜੇ (i > 0)
	{
	    STRCPY((ਅੱਖਰ *)ga.ga_ਅੰਕੜੇ + ga.ga_len, ", ");
	    ga.ga_len += 2;
	}
	ਜੇ (ga_grow(&ga, (ਪੂਰਨ)arg_type.ਲੰਮਾਈ + 8) == FAIL)
	    ਜਾਓ failed;
	ਜੇ (varargs && i == type->tt_argcount - 1)
	    GA_CONCAT_LITERAL(&ga, "...");
	ਨਹੀਂਤਾਂ ਜੇ (i >= type->tt_min_argcount)
	    *((ਅੱਖਰ *)ga.ga_ਅੰਕੜੇ + ga.ga_len++) = '?';
	ga_concat_len(&ga, arg_type.string, arg_type.ਲੰਮਾਈ);
	VIM_CLEAR(arg_free);
    }
    ਜੇ (type->tt_argcount < 0)
	// any number of arguments
	GA_CONCAT_LITERAL(&ga, "...");

    ਜੇ (type->tt_member == &t_void)
	STRCPY((ਅੱਖਰ *)ga.ga_ਅੰਕੜੇ + ga.ga_len, ")");
    ਨਹੀਂਤਾਂ
    {
	ਅੱਖਰ *ret_free = ਨਿਰਾ;
	ਅੱਖਰ *ret_name = type_name(type->tt_member, &ret_free);
	ਪੂਰਨ  len;

	len = (ਪੂਰਨ)STRLEN(ret_name) + 4;
	ਜੇ (ga_grow(&ga, len) == FAIL)
	{
	    vim_free(ret_free);
	    ਜਾਓ failed;
	}
	STRCPY((ਅੱਖਰ *)ga.ga_ਅੰਕੜੇ + ga.ga_len, "): ");
	STRCPY((ਅੱਖਰ *)ga.ga_ਅੰਕੜੇ + ga.ga_len + 3, ret_name);
	vim_free(ret_free);
    }
    *tofree = ga.ga_ਅੰਕੜੇ;
    ਵਾਪਸ ga.ga_ਅੰਕੜੇ;

failed:
    vim_free(arg_free);
    ga_clear(&ga);
    ਵਾਪਸ "[unknown]";
}

/*
 * Return the name of a type.
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    ਅੱਖਰ *
type_name(type_T *type, ਅੱਖਰ **tofree)
{
    ਅੱਖਰ *name;

    *tofree = ਨਿਰਾ;
    ਜੇ (type == ਨਿਰਾ)
	ਵਾਪਸ "[unknown]";
    name = vartype_name(type->tt_type);

    ਚੋਣ (type->tt_type)
    {
	ਹਾਲਤ VAR_LIST:
	ਹਾਲਤ VAR_DICT:
	    ਵਾਪਸ type_name_list_or_dict(name, type, tofree);

	ਹਾਲਤ VAR_TUPLE:
	    ਵਾਪਸ type_name_tuple(type, tofree);

	ਹਾਲਤ VAR_CLASS:
	ਹਾਲਤ VAR_OBJECT:
	    ਵਾਪਸ type_name_class_or_obj(name, type, tofree);

	ਹਾਲਤ VAR_FUNC:
	    ਵਾਪਸ type_name_func(type, tofree);

	ਮੂਲ:
	    ਰੋਕੋ;
    }

    ਵਾਪਸ name;
}

/*
 * "typename(expr)" function
 */
    ਖਾਲੀ
f_typename(typval_T *argvars, typval_T *rettv)
{
    garray_T	type_list;
    type_T	*type;
    ਅੱਖਰ	*tofree;
    ਅੱਖਰ	*name;

    rettv->v_type = VAR_STRING;
    ga_init2(&type_list, ਆਕਾਰ(type_T *), 10);
    ਜੇ (argvars[0].v_type == VAR_TYPEALIAS)
    {
	type = copy_type(argvars[0].vval.v_typealias->ta_type, &type_list);
	// A type alias for a class has the type set to VAR_OBJECT.  Change it
	// to VAR_CLASS, so that the name is "typealias<class<xxx>>"
	ਜੇ (type->tt_type == VAR_OBJECT)
	    type->tt_type = VAR_CLASS;
    }
    ਨਹੀਂਤਾਂ
	type = typval2type(argvars, get_copyID(), &type_list, TVTT_DO_MEMBER);
    name = type_name(type, &tofree);
    ਜੇ (argvars[0].v_type == VAR_TYPEALIAS)
    {
	vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE, "typealias<%s>", name);
	rettv->vval.v_string = vim_strsave((char_u *)IObuff);
	ਜੇ (tofree != ਨਿਰਾ)
	    vim_free(tofree);
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (tofree != ਨਿਰਾ)
	    rettv->vval.v_string = (char_u *)tofree;
	ਨਹੀਂਤਾਂ
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
    ਪੂਰਨ
check_typval_is_ਮੁੱਲ(typval_T *tv)
{
    ਜੇ (tv == ਨਿਰਾ)
	ਵਾਪਸ OK;

    ਚੋਣ (tv->v_type)
    {
	ਹਾਲਤ VAR_CLASS:
	    {
		class_T *cl = tv->vval.v_class;
		char_u *class_name = (cl == ਨਿਰਾ) ? (char_u *)""
							: cl->class_name.string;
		ਜੇ (cl != ਨਿਰਾ && IS_ENUM(cl))
		    semsg(_(e_using_enum_as_ਮੁੱਲ_str), class_name);
		ਨਹੀਂਤਾਂ
		    semsg(_(e_using_class_as_ਮੁੱਲ_str), class_name);
	    }
	    ਵਾਪਸ FAIL;

	ਹਾਲਤ VAR_TYPEALIAS:
	    semsg(_(e_using_typealias_as_ਮੁੱਲ_str),
		    tv->vval.v_typealias->ta_name);
	    ਵਾਪਸ FAIL;

	ਮੂਲ:
	    ਰੋਕੋ;
    }
    ਵਾਪਸ OK;
}

/*
 * Same as above, except check type_T.
 */
    ਪੂਰਨ
check_type_is_ਮੁੱਲ(type_T *type)
{
    ਜੇ (type == ਨਿਰਾ)
	ਵਾਪਸ OK;
    ਚੋਣ (type->tt_type)
    {
	ਹਾਲਤ VAR_CLASS:
	    ਜੇ (type->tt_class != ਨਿਰਾ && IS_ENUM(type->tt_class))
		semsg(_(e_using_enum_as_ਮੁੱਲ_str),
			type->tt_class->class_name.string);
	    ਨਹੀਂਤਾਂ
		semsg(_(e_using_class_as_ਮੁੱਲ_str),
			type->tt_class == ਨਿਰਾ ? (char_u *)""
			: type->tt_class->class_name.string);
	    ਵਾਪਸ FAIL;

	ਹਾਲਤ VAR_TYPEALIAS:
	    // TODO: Not sure what could be done here to get a name.
	    //       Maybe an optional argument?
	    emsg(_(e_using_typealias_as_var_val));
	    ਵਾਪਸ FAIL;

	ਮੂਲ:
	    ਰੋਕੋ;
    }
    ਵਾਪਸ OK;
}

#ਜੇ_ਅੰਤ // FEAT_EVAL
