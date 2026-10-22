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
#include "vim.h"

#ပေႃး defined(FEAT_EVAL)

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

    ပေႃး (ga_grow(type_gap, 1) == FAIL)
	ပွၵ်ႈ NULL;
    type = ALLOC_CLEAR_ONE(type_T);
    ပေႃး (type == NULL)
	ပွၵ်ႈ NULL;

    ((type_T **)type_gap->ga_data)[type_gap->ga_len] = type;
    ++type_gap->ga_len;
    ပွၵ်ႈ type;
}

/*
 * Make a shallow copy of "type".
 * When allocation fails returns "type".
 */
    type_T *
copy_type(type_T *type, garray_T *type_gap)
{
    type_T *copy = get_type_ptr(type_gap);

    ပေႃး (copy == NULL)
	ပွၵ်ႈ type;
    *copy = *type;
    copy->tt_flags &= ~TTFLAG_STATIC;

    ပေႃး (type->tt_args != NULL
	   && func_type_add_arg_types(copy, type->tt_argcount, type_gap) == OK)
	for (puur_naa_nka i = 0; i < type->tt_argcount; ++i)
	    copy->tt_args[i] = type->tt_args[i];

    ပွၵ်ႈ copy;
}

/*
 * Inner part of copy_type_deep().
 * When allocation fails returns "type".
 */
    static type_T *
copy_type_deep_rec(type_T *type, garray_T *type_gap, garray_T *seen_types)
{
    for (puur_naa_nka i = 0; i < seen_types->ga_len; ++i)
	ပေႃး (((type_T **)seen_types->ga_data)[i * 2] == type)
	    // seen this type before, return the copy we made
	    ပွၵ်ႈ ((type_T **)seen_types->ga_data)[i * 2 + 1];

    type_T *copy = copy_type(type, type_gap);
    ပေႃး (ga_grow(seen_types, 1) == FAIL)
	ပွၵ်ႈ copy;
    ((type_T **)seen_types->ga_data)[seen_types->ga_len * 2] = type;
    ((type_T **)seen_types->ga_data)[seen_types->ga_len * 2 + 1] = copy;
    ++seen_types->ga_len;

    ပေႃး (copy->tt_member != NULL)
	copy->tt_member = copy_type_deep_rec(copy->tt_member,
							 type_gap, seen_types);
    ပေႃး (type->tt_args != NULL)
	for (puur_naa_nka i = 0; i < type->tt_argcount; ++i)
	    copy->tt_args[i] = copy_type_deep_rec(copy->tt_args[i],
							 type_gap, seen_types);

    ပွၵ်ႈ copy;
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
    ga_init2(&seen_types, parimaa_na(type_T *) * 2, 20);

    type_T *res = copy_type_deep_rec(type, type_gap, &seen_types);

    ga_clear(&seen_types);
    ပွၵ်ႈ res;
}

    void
clear_type_list(garray_T *gap)
{
    ၶၢဝ်း (gap->ga_len > 0)
	vim_free(((type_T **)gap->ga_data)[--gap->ga_len]);
    ga_clear(gap);
}

    void
clear_func_type_list(garray_T *gap, type_T **func_type)
{
    ၶၢဝ်း (gap->ga_len > 0)
    {
	// func_type pointing to the uf_type_list, so reset pointer
	ပေႃး (*func_type == ((type_T **)gap->ga_data)[--gap->ga_len])
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

    ပေႃး (type == NULL)
	ပွၵ်ႈ NULL;

    // A fixed type never contains allocated types, return as-is.
    ပေႃး (type->tt_flags & TTFLAG_STATIC)
	ပွၵ်ႈ type;

    ret = ALLOC_ONE(type_T);
    ပေႃး (ret == NULL)
	ပွၵ်ႈ NULL;
    *ret = *type;

    ပေႃး (ret->tt_member != NULL)
	ret->tt_member = alloc_type(ret->tt_member);

    ပေႃး (type->tt_argcount > 0 && type->tt_args != NULL)
    {
	puur_naa_nka i;

	ret->tt_args = ALLOC_MULT(type_T *, type->tt_argcount);
	ပေႃး (ret->tt_args != NULL)
	    for (i = 0; i < type->tt_argcount; ++i)
		ret->tt_args[i] = alloc_type(type->tt_args[i]);
    }
    ပေႃးဢမ်ႇ
	ret->tt_args = NULL;

    ပွၵ်ႈ ret;
}

/*
 * Free a type that was created with alloc_type().
 */
    void
free_type(type_T *type)
{
    puur_naa_nka i;

    ပေႃး (type == NULL || (type->tt_flags & TTFLAG_STATIC))
	ပွၵ်ႈ;
    ပေႃး (type->tt_args != NULL)
    {
	for (i = 0; i < type->tt_argcount; ++i)
	    free_type(type->tt_args[i]);
	vim_free(type->tt_args);
    }

    free_type(type->tt_member);

    vim_free(type);
}

/*
 * Return TRUE if "type" is to be recursed into for setting the type.
 */
    static puur_naa_nka
set_tv_type_recurse(type_T *type)
{
    ပွၵ်ႈ type->tt_member != NULL
		&& (type->tt_member->tt_type == VAR_DICT
				|| type->tt_member->tt_type == VAR_LIST)
		&& type->tt_member->tt_member != NULL
		&& type->tt_member->tt_member != &t_any
		&& type->tt_member->tt_member != &t_unknown;
}

/*
 * Set the type of Dict "d" to "type"
 */
    static void
set_tv_type_dict(dict_T *d, type_T *type)
{
    ပေႃး (d->dv_type == type)
	ပွၵ်ႈ;

    free_type(d->dv_type);
    d->dv_type = alloc_type(type);

    // Need to recursively set the type of dict items?
    ပေႃး (!set_tv_type_recurse(type))
	ပွၵ်ႈ;

    puur_naa_nka		todo = (puur_naa_nka)d->dv_hashtab.ht_used;
    hashitem_T	*hi;
    dictitem_T	*di;

    FOR_ALL_HASHTAB_ITEMS(&d->dv_hashtab, hi, todo)
    {
	ပေႃး (!HASHITEM_EMPTY(hi))
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
    static void
set_tv_type_list(list_T *l, type_T *type)
{
    ပေႃး (l->lv_type == type)
	ပွၵ်ႈ;

    free_type(l->lv_type);
    l->lv_type = alloc_type(type);

    // Need to recursively set the type of list items?
    ပေႃး (l->lv_first == &range_list_item || !set_tv_type_recurse(type))
	ပွၵ်ႈ;

    listitem_T	*li;

    FOR_ALL_LIST_ITEMS(l, li)
	set_tv_type(&li->li_tv, type->tt_member);
}

/*
 * Set the type of Tuple "tuple" to "type"
 */
    static void
set_tv_type_tuple(tuple_T *tuple, type_T *type)
{
    ပေႃး (tuple->tv_type == type)
	ပွၵ်ႈ;

    free_type(tuple->tv_type);
    tuple->tv_type = alloc_type(type);

    ပေႃး (type->tt_argcount <= 0)
	ပွၵ်ႈ;

    // recursively set the type of list items
    type_T	*item_type;
    for (puur_naa_nka i = 0; i < tuple_len(tuple); i++)
    {
	ပေႃး ((type->tt_flags & TTFLAG_VARARGS) && (i >= type->tt_argcount - 1))
	    // For a variadic tuple, the last type is a List.  So use the
	    // List member type.
	    item_type = type->tt_args[type->tt_argcount - 1]->tt_member;
	ပေႃးဢမ်ႇ
	    item_type = type->tt_args[i];

	set_tv_type(TUPLE_ITEM(tuple, i), item_type);
    }
}

/*
 * Set the type of "tv" to "type" if it is a list or tuple or dict.
 */
    void
set_tv_type(typval_T *tv, type_T *type)
{
    ပေႃး (type->tt_type == VAR_ANY)
	// If the variable type is "any", then keep the value type.
	// e.g.  var x: any = [1, 2] or var y: any = {v: 1}
	ပွၵ်ႈ;

    ပေႃး (tv->v_type == VAR_DICT && tv->vval.v_dict != NULL)
	set_tv_type_dict(tv->vval.v_dict, type);
    ပေႃးဢမ်ႇ ပေႃး (tv->v_type == VAR_LIST && tv->vval.v_list != NULL)
	set_tv_type_list(tv->vval.v_list, type);
    ပေႃးဢမ်ႇ ပေႃး (tv->v_type == VAR_TUPLE && tv->vval.v_tuple != NULL)
	set_tv_type_tuple(tv->vval.v_tuple, type);
}

/*
 * For a tuple type, reserve space for "typecount" types (including the
 * repeated type).
 */
    static puur_naa_nka
tuple_type_add_types(
    type_T	*tupletype,
    puur_naa_nka		typecount,
    garray_T	*type_gap)
{
    // To make it easy to free the space needed for the types, add the
    // pointer to type_gap.
    ပေႃး (ga_grow(type_gap, 1) == FAIL)
	ပွၵ်ႈ FAIL;
    tupletype->tt_args = alloc_clear_id(parimaa_na(type_T *) * typecount,
							  aid_tuple_type_args);
    ပေႃး (tupletype->tt_args == NULL)
	ပွၵ်ႈ FAIL;
    ((type_T **)type_gap->ga_data)[type_gap->ga_len] =
						(void *)tupletype->tt_args;
    ++type_gap->ga_len;
    ပွၵ်ႈ OK;
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
    ပေႃး (member_type == NULL || (member_type->tt_type == VAR_ANY
					&& !IS_GENERIC_TYPE(member_type)))
	ပွၵ်ႈ &t_list_any;
    ပေႃး (member_type->tt_type == VAR_VOID
	    || member_type->tt_type == VAR_UNKNOWN)
	ပွၵ်ႈ &t_list_empty;
    ပေႃး (member_type->tt_type == VAR_BOOL)
	ပွၵ်ႈ &t_list_bool;
    ပေႃး (member_type->tt_type == VAR_NUMBER)
	ပွၵ်ႈ &t_list_number;
    ပေႃး (member_type->tt_type == VAR_STRING)
	ပွၵ်ႈ &t_list_string;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    ပေႃး (type == NULL)
	ပွၵ်ႈ &t_any;
    type->tt_type = VAR_LIST;
    type->tt_member = member_type;
    type->tt_argcount = 0;
    type->tt_args = NULL;
    ပွၵ်ႈ type;
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
    puur_naa_nka		typecount = tuple_types_gap->ga_len;

    // recognize commonly used types
    ပေႃး (typecount == 0)
	ပွၵ်ႈ &t_tuple_any;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    ပေႃး (type == NULL)
	ပွၵ်ႈ &t_any;
    type->tt_type = VAR_TUPLE;
    type->tt_member = NULL;
    ပေႃး (typecount > 0)
    {
	ပေႃး (tuple_type_add_types(type, typecount, type_gap) == FAIL)
	    ပွၵ်ႈ &t_any;
	mch_memmove(type->tt_args, tuple_types, parimaa_na(type_T *) * typecount);
    }
    type->tt_argcount = typecount;
    type->tt_flags = 0;

    ပွၵ်ႈ type;
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
    ပေႃး (member_type == NULL || (member_type->tt_type == VAR_ANY
					&& !IS_GENERIC_TYPE(member_type)))
	ပွၵ်ႈ &t_dict_any;
    ပေႃး (member_type->tt_type == VAR_VOID
	    || member_type->tt_type == VAR_UNKNOWN)
	ပွၵ်ႈ &t_dict_empty;
    ပေႃး (member_type->tt_type == VAR_BOOL)
	ပွၵ်ႈ &t_dict_bool;
    ပေႃး (member_type->tt_type == VAR_NUMBER)
	ပွၵ်ႈ &t_dict_number;
    ပေႃး (member_type->tt_type == VAR_STRING)
	ပွၵ်ႈ &t_dict_string;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    ပေႃး (type == NULL)
	ပွၵ်ႈ &t_any;
    type->tt_type = VAR_DICT;
    type->tt_member = member_type;
    type->tt_argcount = 0;
    type->tt_args = NULL;
    ပွၵ်ႈ type;
}

/*
 * Allocate a new type for a function.
 */
    type_T *
alloc_func_type(type_T *ret_type, puur_naa_nka argcount, garray_T *type_gap)
{
    type_T *type = get_type_ptr(type_gap);

    ပေႃး (type == NULL)
	ပွၵ်ႈ &t_any;
    type->tt_type = VAR_FUNC;
    type->tt_member = ret_type == NULL ? &t_unknown : ret_type;
    type->tt_argcount = argcount;
    type->tt_args = NULL;
    ပွၵ်ႈ type;
}

/*
 * Allocate a new type for a tuple.
 */
    static type_T *
alloc_tuple_type(puur_naa_nka typecount, garray_T *type_gap)
{
    type_T *type = get_type_ptr(type_gap);

    ပေႃး (type == NULL)
	ပွၵ်ႈ &t_any;
    type->tt_type = VAR_TUPLE;
    type->tt_member = NULL;
    type->tt_argcount = typecount;
    type->tt_args = NULL;
    ပွၵ်ႈ type;
}

/*
 * Get a function type, based on the return type "ret_type".
 * "argcount" must be -1 or 0, a predefined type can be used.
 */
    type_T *
get_func_type(type_T *ret_type, puur_naa_nka argcount, garray_T *type_gap)
{
    // recognize commonly used types
    ပေႃး (ret_type == &t_unknown || ret_type == NULL)
    {
	// (argcount == 0) is not possible
	ပွၵ်ႈ &t_func_unknown;
    }
    ပေႃး (ret_type == &t_void)
    {
	ပေႃး (argcount == 0)
	    ပွၵ်ႈ &t_func_0_void;
	ပေႃးဢမ်ႇ
	    ပွၵ်ႈ &t_func_void;
    }
    ပေႃး (ret_type == &t_any)
    {
	ပေႃး (argcount == 0)
	    ပွၵ်ႈ &t_func_0_any;
	ပေႃးဢမ်ႇ
	    ပွၵ်ႈ &t_func_any;
    }
    ပေႃး (ret_type == &t_number)
    {
	ပေႃး (argcount == 0)
	    ပွၵ်ႈ &t_func_0_number;
	ပေႃးဢမ်ႇ
	    ပွၵ်ႈ &t_func_number;
    }
    ပေႃး (ret_type == &t_string)
    {
	ပေႃး (argcount == 0)
	    ပွၵ်ႈ &t_func_0_string;
	ပေႃးဢမ်ႇ
	    ပွၵ်ႈ &t_func_string;
    }

    ပွၵ်ႈ alloc_func_type(ret_type, argcount, type_gap);
}

/*
 * For a function type, reserve space for "argcount" argument types (including
 * vararg).
 */
    puur_naa_nka
func_type_add_arg_types(
	type_T	    *functype,
	puur_naa_nka	    argcount,
	garray_T    *type_gap)
{
    // To make it easy to free the space needed for the argument types, add the
    // pointer to type_gap.
    ပေႃး (ga_grow(type_gap, 1) == FAIL)
	ပွၵ်ႈ FAIL;
    functype->tt_args = ALLOC_CLEAR_MULT(type_T *, argcount);
    ပေႃး (functype->tt_args == NULL)
	ပွၵ်ႈ FAIL;
    ((type_T **)type_gap->ga_data)[type_gap->ga_len] =
						     (void *)functype->tt_args;
    ++type_gap->ga_len;
    ပွၵ်ႈ OK;
}

/*
 * Return TRUE if "type" is NULL, any or unknown.
 * This also works for const (comparing with &t_any and &t_unknown doesn't).
 */
    puur_naa_nka
type_any_or_unknown(type_T *type)
{
    ပွၵ်ႈ type == NULL || type->tt_type == VAR_ANY
					|| type->tt_type == VAR_UNKNOWN;
}

/*
 * Get a type_T for a "special" typval in "tv".
 */
    static type_T *
special_typval2type(typval_T *tv)
{
    switch (tv->vval.v_number)
    {
	pak_sa VVAL_NULL:
	    ပွၵ်ႈ &t_null;

	pak_sa VVAL_NONE:
	    ပွၵ်ႈ &t_none;

	pak_sa VVAL_TRUE:
	pak_sa VVAL_FALSE:
	    ပွၵ်ႈ &t_bool;

	default:
	    ပွၵ်ႈ &t_unknown;
    }
}

/*
 * Get a type_T for a List typval in "tv".
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    static type_T *
list_typval2type(typval_T *tv, puur_naa_nka copyID, garray_T *type_gap, puur_naa_nka flags)
{
    list_T	*l = tv->vval.v_list;
    listitem_T	*li;
    type_T	*member_type = NULL;

    // An empty list has type list<unknown>, unless the type was specified
    // and is not list<any>.  This matters when assigning to a variable
    // with a specific list type.
    ပေႃး (l == NULL || (l->lv_first == NULL
		&& (l->lv_type == NULL || l->lv_type->tt_member == &t_any)))
	ပွၵ်ႈ &t_list_empty;

    ပေႃး ((flags & TVTT_DO_MEMBER) == 0)
	ပွၵ်ႈ &t_list_any;

    // If the type is list<any> go through the members, it may end up a
    // more specific type.
    ပေႃး (l->lv_type != NULL && (l->lv_first == NULL
		|| (flags & TVTT_MORE_SPECIFIC) == 0
		|| l->lv_type->tt_member != &t_any))
	// make a copy, lv_type may be freed if the list is freed
	ပွၵ်ႈ copy_type_deep(l->lv_type, type_gap);

    ပေႃး (l->lv_first == &range_list_item)
	ပွၵ်ႈ &t_list_number;

    ပေႃး (l->lv_copyID == copyID)
	// avoid recursion
	ပွၵ်ႈ &t_list_any;

    l->lv_copyID = copyID;

    // Use the common type of all members.
    member_type = typval2type(&l->lv_first->li_tv, copyID, type_gap,
							TVTT_DO_MEMBER);
    for (li = l->lv_first->li_next; li != NULL; li = li->li_next)
	common_type(typval2type(&li->li_tv, copyID, type_gap, TVTT_DO_MEMBER),
					member_type, &member_type, type_gap);

    // Reset copyID so that a shared reference to this list (not a circular
    // reference) can be processed again to get the correct type.
    l->lv_copyID = 0;

    ပွၵ်ႈ get_list_type(member_type, type_gap);
}

/*
 * Get a type_T for a Tuple typval in "tv".
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    static type_T *
tuple_typval2type(typval_T *tv, puur_naa_nka copyID, garray_T *type_gap, puur_naa_nka flags)
{
    tuple_T	*tuple = tv->vval.v_tuple;
    puur_naa_nka		len = tuple_len(tuple);
    type_T	*type = NULL;

    // An empty tuple has type tuple<unknown>, unless the type was specified
    // and is not tuple<any>.  This matters when assigning to a variable
    // with a specific tuple type.
    ပေႃး (tuple == NULL || (len == 0 && (tuple->tv_type == NULL
				|| tuple->tv_type->tt_argcount == 0)))
	ပွၵ်ႈ &t_tuple_empty;

    ပေႃး ((flags & TVTT_DO_MEMBER) == 0)
	ပွၵ်ႈ &t_tuple_any;

    // If the type is tuple<any> go through the members, it may end up a
    // more specific type.
    ပေႃး (tuple->tv_type != NULL && (len == 0
					|| (flags & TVTT_MORE_SPECIFIC) == 0))
	// make a copy, tv_type may be freed if the tuple is freed
	ပွၵ်ႈ copy_type_deep(tuple->tv_type, type_gap);

    ပေႃး (tuple->tv_copyID == copyID)
	// avoid recursion
	ပွၵ်ႈ &t_tuple_any;

    tuple->tv_copyID = copyID;

    garray_T	tuple_types_ga;
    ga_init2(&tuple_types_ga, parimaa_na(type_T *), 10);
    for (puur_naa_nka i = 0; i < len; i++)
    {
	type = typval2type(TUPLE_ITEM(tuple, i), copyID, type_gap,
							TVTT_DO_MEMBER);
	ပေႃး (ga_grow(&tuple_types_ga, 1) == FAIL)
	{
	    ga_clear(&tuple_types_ga);
	    // Reset copyID so that a shared reference to this tuple can be
	    // processed again.
	    tuple->tv_copyID = 0;
	    ပွၵ်ႈ NULL;
	}
	((type_T **)tuple_types_ga.ga_data)[tuple_types_ga.ga_len] = type;
	tuple_types_ga.ga_len++;
    }

    type_T *tuple_type = get_tuple_type(&tuple_types_ga, type_gap);
    ga_clear(&tuple_types_ga);

    // Reset copyID so that a shared reference to this tuple (not a circular
    // reference) can be processed again to get the correct type.
    tuple->tv_copyID = 0;

    ပွၵ်ႈ tuple_type;
}

/*
 * Get a type_T for a Dict typval in "tv".
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    static type_T *
dict_typval2type(typval_T *tv, puur_naa_nka copyID, garray_T *type_gap, puur_naa_nka flags)
{
    dict_iterator_T	iter;
    typval_T		*value;
    dict_T		*d = tv->vval.v_dict;
    type_T		*member_type = NULL;

    ပေႃး (d == NULL || (d->dv_hashtab.ht_used == 0 && d->dv_type == NULL))
	ပွၵ်ႈ &t_dict_empty;

    ပေႃး ((flags & TVTT_DO_MEMBER) == 0)
	ပွၵ်ႈ &t_dict_any;

    // If the type is dict<any> go through the members, it may end up a
    // more specific type.
    ပေႃး (d->dv_type != NULL && (d->dv_hashtab.ht_used == 0
		|| (flags & TVTT_MORE_SPECIFIC) == 0
		|| d->dv_type->tt_member != &t_any))
	ပွၵ်ႈ d->dv_type;

    ပေႃး (d->dv_copyID == copyID)
	// avoid recursion
	ပွၵ်ႈ &t_dict_any;

    d->dv_copyID = copyID;

    // Use the common type of all values.
    dict_iterate_start(tv, &iter);
    dict_iterate_next(&iter, &value);
    member_type = typval2type(value, copyID, type_gap, TVTT_DO_MEMBER);

    ၶၢဝ်း (dict_iterate_next(&iter, &value) != NULL)
	common_type(typval2type(value, copyID, type_gap, TVTT_DO_MEMBER),
					member_type, &member_type, type_gap);

    // Reset copyID so that a shared reference to this dict (not a circular
    // reference) can be processed again to get the correct type.
    d->dv_copyID = 0;

    ပွၵ်ႈ get_dict_type(member_type, type_gap);
}

/*
 * Get a type_T for a "partial" typval in "tv".
 */
    static type_T *
partial_typval2type(typval_T *tv, ufunc_T *ufunc, garray_T *type_gap)
{
    partial_T *pt = tv->vval.v_partial;
    type_T  *type;

    type = get_type_ptr(type_gap);
    ပေႃး (type == NULL)
	ပွၵ်ႈ NULL;

    *type = *ufunc->uf_func_type;
    ပေႃး (type->tt_argcount >= 0 && pt->pt_argc > 0)
    {
	type->tt_argcount -= pt->pt_argc;
	type->tt_min_argcount -= pt->pt_argc;
	ပေႃး (type->tt_argcount > 0 && func_type_add_arg_types(type,
		    type->tt_argcount, type_gap) == OK)
	    for (puur_naa_nka i = 0; i < type->tt_argcount; ++i)
		type->tt_args[i] =
		    ufunc->uf_func_type->tt_args[i + pt->pt_argc];
    }
    ပေႃး (pt->pt_func != NULL)
	type->tt_member = pt->pt_func->uf_ret_type;

    ပွၵ်ႈ type;
}

/*
 * Get a type_T for a "class" or an "object" typval in "tv".
 */
    static type_T *
oc_typval2type(typval_T *tv)
{
    ပေႃး (tv->v_type == VAR_CLASS)
    {
	ပေႃး (tv->vval.v_class == NULL)
	    ပွၵ်ႈ &t_class;

	ပွၵ်ႈ &tv->vval.v_class->class_type;
    }

    ပေႃး (tv->vval.v_object != NULL)
	ပွၵ်ႈ &tv->vval.v_object->obj_class->class_object_type;

    ပွၵ်ႈ &t_object_any;
}

/*
 * Get a type_T for a "function" or a "partial"
 */
    static type_T *
fp_typval2type(typval_T *tv, garray_T *type_gap)
{
    char_u	*name = NULL;
    ufunc_T	*ufunc = NULL;
    type_T	*type;
    type_T	*member_type = NULL;
    puur_naa_nka		argcount = 0;
    puur_naa_nka		min_argcount = 0;

    ပေႃး (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != NULL)
    {
	ပေႃး (tv->vval.v_partial->pt_func != NULL)
	    ufunc = tv->vval.v_partial->pt_func;
	ပေႃးဢမ်ႇ
	    name = tv->vval.v_partial->pt_name;
    }
    ပေႃးဢမ်ႇ
	name = tv->vval.v_string;

    ပေႃး (name == NULL && ufunc == NULL)
	ပွၵ်ႈ &t_func_unknown;

    ပေႃး (name != NULL)
    {
	puur_naa_nka idx = find_internal_func(name);

	ပေႃး (idx >= 0)
	{
	    type_T *decl_type;  // unused

	    internal_func_get_argcount(idx, &argcount, &min_argcount);
	    member_type = internal_func_ret_type(idx, 0, NULL, &decl_type,
		    type_gap);
	}
	ပေႃးဢမ်ႇ
	{
	    // Check if name contains "<".  If it does, then replace "<" with
	    // NUL and lookup the function and then look up the specific
	    // generic function using the supplied types.
	    char_u *p = generic_func_find_open_bracket(name);
	    ပေႃး (p == NULL)
		ufunc = find_func(name, FALSE);
	    ပေႃးဢမ်ႇ
	    {
		// generic function
		char_u	cc = *p;

		*p = NUL;
		ufunc = find_func(name, FALSE);
		*p = cc;
		ပေႃး (ufunc != NULL && IS_GENERIC_FUNC(ufunc))
		{
		    ufunc = find_generic_func(ufunc, name, &p);
		    ပေႃး (ufunc == NULL)
			ပွၵ်ႈ NULL;
		}
	    }
	}
    }
    ပေႃး (ufunc != NULL)
    {
	// May need to get the argument types from default values by
	// compiling the function.
	ပေႃး (ufunc->uf_def_status == UF_TO_BE_COMPILED
		&& compile_def_function(ufunc, TRUE, CT_NONE, NULL)
		== FAIL)
	    ပွၵ်ႈ NULL;
	ပေႃး (ufunc->uf_func_type == NULL)
	    set_function_type(ufunc);
	ပေႃး (ufunc->uf_func_type != NULL)
	{
	    ပေႃး (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != NULL)
		ပွၵ်ႈ partial_typval2type(tv, ufunc, type_gap);
	    ပွၵ်ႈ ufunc->uf_func_type;
	}
    }

    type = get_type_ptr(type_gap);
    ပေႃး (type == NULL)
	ပွၵ်ႈ NULL;
    type->tt_type = tv->v_type;
    type->tt_argcount = argcount;
    type->tt_min_argcount = min_argcount;
    ပေႃး (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != NULL
					&& tv->vval.v_partial->pt_argc > 0)
    {
	type->tt_argcount -= tv->vval.v_partial->pt_argc;
	type->tt_min_argcount -= tv->vval.v_partial->pt_argc;
    }
    type->tt_member = member_type;

    ပွၵ်ႈ type;
}

/*
 * Get a type_T for a typval_T.
 * "type_gap" is used to temporarily create types in.
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    static type_T *
typval2type_int(typval_T *tv, puur_naa_nka copyID, garray_T *type_gap, puur_naa_nka flags)
{
    switch (tv->v_type)
    {
	pak_sa VAR_UNKNOWN:
	    ပွၵ်ႈ &t_unknown;

	pak_sa VAR_ANY:
	    ပွၵ်ႈ &t_any;

	pak_sa VAR_VOID:
	    ပွၵ်ႈ &t_void;

	pak_sa VAR_BOOL:
	    ပွၵ်ႈ &t_bool;

	pak_sa VAR_SPECIAL:
	    ပွၵ်ႈ special_typval2type(tv);

	pak_sa VAR_NUMBER:
	    ပွၵ်ႈ &t_number;

	pak_sa VAR_FLOAT:
	    ပွၵ်ႈ &t_float;

	pak_sa VAR_STRING:
	    ပွၵ်ႈ &t_string;

	pak_sa VAR_BLOB:
	    ပေႃး (tv->vval.v_blob == NULL)
		ပွၵ်ႈ &t_blob_null;
	    ပွၵ်ႈ &t_blob;

	pak_sa VAR_LIST:
	    ပွၵ်ႈ list_typval2type(tv, copyID, type_gap, flags);

	pak_sa VAR_TUPLE:
	    ပွၵ်ႈ tuple_typval2type(tv, copyID, type_gap, flags);

	pak_sa VAR_DICT:
	    ပွၵ်ႈ dict_typval2type(tv, copyID, type_gap, flags);

	pak_sa VAR_JOB:
	    ပွၵ်ႈ &t_job;

	pak_sa VAR_CHANNEL:
	    ပွၵ်ႈ &t_channel;

	pak_sa VAR_CLASS:
	pak_sa VAR_OBJECT:
	    ပွၵ်ႈ oc_typval2type(tv);

	pak_sa VAR_TYPEALIAS:
	    ပွၵ်ႈ &t_typealias;

	pak_sa VAR_FUNC:
	pak_sa VAR_PARTIAL:
	    ပွၵ်ႈ fp_typval2type(tv, type_gap);

	pak_sa VAR_INSTR:
	default:
	    bha_nga;
    }

    ပွၵ်ႈ NULL;
}

/*
 * Return TRUE if "tv" is not a bool but should be converted to bool.
 */
    puur_naa_nka
need_convert_to_bool(type_T *type, typval_T *tv)
{
    ပွၵ်ႈ type != NULL && type == &t_bool && tv->v_type != VAR_BOOL
	    && (tv->v_type == VAR_NUMBER
		       && (tv->vval.v_number == 0 || tv->vval.v_number == 1));
}

/*
 * Get a type_T for a typval_T.
 * "type_list" is used to temporarily create types in.
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the most specific member type.
 */
    type_T *
typval2type(typval_T *tv, puur_naa_nka copyID, garray_T *type_gap, puur_naa_nka flags)
{
    type_T *type = typval2type_int(tv, copyID, type_gap, flags);

    ပေႃး (type == NULL)
	ပွၵ်ႈ NULL;

    ပေႃး (type != &t_bool && (tv->v_type == VAR_NUMBER
		&& (tv->vval.v_number == 0 || tv->vval.v_number == 1)))
	// Number 0 and 1 and expression with "&&" or "||" can also be used
	// for bool.
	type = &t_number_bool;
    ပေႃးဢမ်ႇ ပေႃး (type != &t_float && tv->v_type == VAR_NUMBER)
	// A number can also be used for float.
	type = &t_number_float;
    ပွၵ်ႈ type;
}

/*
 * Return TRUE if "type" can be used for a variable declaration.
 * Give an error and return FALSE if not.
 */
    puur_naa_nka
valid_declaration_type(type_T *type)
{
    ပေႃး (type->tt_type == VAR_SPECIAL  // null, none
	    || type->tt_type == VAR_VOID)
    {
	var_na *tofree = NULL;
	var_na *name = type_name(type, &tofree);
	semsg(_(e_invalid_type_in_variable_declaration_str), name);
	vim_free(tofree);
	ပွၵ်ႈ FALSE;
    }
    ပွၵ်ႈ TRUE;
}

/*
 * Get a type_T for a typval_T, used for v: variables.
 * "type_list" is used to temporarily create types in.
 */
    type_T *
typval2type_vimvar(typval_T *tv, garray_T *type_gap)
{
    ပေႃး (tv->v_type == VAR_LIST)  // e.g. for v:oldfiles
	ပွၵ်ႈ &t_list_string;
    ပေႃး (tv->v_type == VAR_DICT)  // e.g. for v:event
	ပွၵ်ႈ &t_dict_any;
    ပွၵ်ႈ typval2type(tv, get_copyID(), type_gap, TVTT_DO_MEMBER);
}

    puur_naa_nka
check_typval_arg_type(
	type_T	    *expected,
	typval_T    *actual_tv,
	var_na	    *func_name,
	puur_naa_nka	    arg_idx)
{
    where_T	where = WHERE_INIT;

    ပေႃး (arg_idx > 0)
    {
	where.wt_index = arg_idx;
	where.wt_kind = WT_ARGUMENT;
    }
    where.wt_func_name = func_name;
    ပွၵ်ႈ check_typval_type(expected, actual_tv, where);
}

/*
 * Return true if "expected" is list<any> or dict<any> and "actual_tv" is a
 * list or dict of that kind, which matches whatever its values are.
 */
    static bool
matches_listdict_any(type_T *expected, typval_T *actual_tv)
{
    ပွၵ်ႈ (expected->tt_type == VAR_LIST || expected->tt_type == VAR_DICT)
	    && actual_tv->v_type == expected->tt_type
	    && expected->tt_member != NULL
	    && (expected->tt_member->tt_type == VAR_ANY
		|| expected->tt_member->tt_type == VAR_UNKNOWN);
}

/*
 * Return FAIL if "expected" and "actual" don't match.
 * When "argidx" > 0 it is included in the error message.
 */
    puur_naa_nka
check_typval_type(type_T *expected, typval_T *actual_tv, where_T where)
{
    garray_T	type_list;
    type_T	*actual_type;
    puur_naa_nka		res = FAIL;

    ပေႃး (expected == NULL)
	ပွၵ်ႈ OK;  // didn't expect anything.

    // No need to go through the values of a possibly big list or dict.
    ပေႃး (matches_listdict_any(expected, actual_tv))
	ပွၵ်ႈ OK;

    ga_init2(&type_list, parimaa_na(type_T *), 10);

    // A null_function and null_partial are special cases, they can be used to
    // clear a variable.
    ပေႃး ((actual_tv->v_type == VAR_FUNC && actual_tv->vval.v_string == NULL)
	    || (actual_tv->v_type == VAR_PARTIAL
					 && actual_tv->vval.v_partial == NULL))
	actual_type = &t_func_unknown;
    ပေႃးဢမ်ႇ
	// When the actual type is list<any> or dict<any> go through the values
	// to possibly get a more specific type.
	actual_type = typval2type(actual_tv, get_copyID(), &type_list,
					  TVTT_DO_MEMBER | TVTT_MORE_SPECIFIC);
    ပေႃး (actual_type != NULL)
    {
	res = check_type_maybe(expected, actual_type, TRUE, where);
	ပေႃး (res == MAYBE && !(actual_type->tt_type == VAR_FUNC
				      && (actual_type->tt_member == &t_unknown
					  || actual_type->tt_member == NULL)))
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
    ပွၵ်ႈ res;
}

    void
arg_type_mismatch(type_T *expected, type_T *actual, puur_naa_nka arg_idx)
{
    where_T	where = WHERE_INIT;

    ပေႃး (arg_idx > 0)
    {
	where.wt_index = arg_idx;
	where.wt_kind = WT_ARGUMENT;
    }
    type_mismatch_where(expected, actual, where);
}

    void
type_mismatch_where(type_T *expected, type_T *actual, where_T where)
{
    var_na *tofree1, *tofree2;
    var_na *typename1 = type_name(expected, &tofree1);
    var_na *typename2 = type_name(actual, &tofree2);

    switch (where.wt_kind)
    {
	pak_sa WT_MEMBER:
	    semsg(_(e_variable_str_type_mismatch_expected_str_but_got_str),
		    where.wt_func_name, typename1, typename2);
	    bha_nga;
	pak_sa WT_METHOD:
	pak_sa WT_METHOD_ARG:
	pak_sa WT_METHOD_RETURN:
	    semsg(_(e_method_str_type_mismatch_expected_str_but_got_str),
		    where.wt_func_name, typename1, typename2);
	    bha_nga;
	pak_sa WT_VARIABLE:
	    ပေႃး (where.wt_func_name == NULL)
		semsg(_(e_variable_nr_type_mismatch_expected_str_but_got_str),
			where.wt_index, typename1, typename2);
	    ပေႃးဢမ်ႇ
		semsg(_(e_variable_nr_type_mismatch_expected_str_but_got_str_in_str),
			where.wt_index, typename1, typename2, where.wt_func_name);
	    bha_nga;
	pak_sa WT_ARGUMENT:
	    ပေႃး (where.wt_func_name == NULL)
		semsg(_(e_argument_nr_type_mismatch_expected_str_but_got_str),
			where.wt_index, typename1, typename2);
	    ပေႃးဢမ်ႇ
		semsg(_(e_argument_nr_type_mismatch_expected_str_but_got_str_in_str),
			where.wt_index, typename1, typename2, where.wt_func_name);
	    bha_nga;
	pak_sa WT_CAST:
	pak_sa WT_UNKNOWN:
	    ပေႃး (where.wt_func_name == NULL)
		semsg(_(e_type_mismatch_expected_str_but_got_str),
			typename1, typename2);
	    ပေႃးဢမ်ႇ
		semsg(_(e_type_mismatch_expected_str_but_got_str_in_str),
			typename1, typename2, where.wt_func_name);
	    bha_nga;
    }

    vim_free(tofree1);
    vim_free(tofree2);
}

/*
 * Check if the expected and actual types match for a tuple
 */
    static puur_naa_nka
check_tuple_type_maybe(
    type_T	*expected,
    type_T	*actual,
    where_T	where)
{
    ပေႃး (expected->tt_argcount == -1 || actual->tt_argcount == -1
		|| expected->tt_args == NULL || actual->tt_args == NULL)
	ပွၵ်ႈ OK;

    // For a non-variadic tuple, the number of items must match
    ပေႃး (!(expected->tt_flags & TTFLAG_VARARGS)
	    && expected->tt_argcount != actual->tt_argcount)
	ပွၵ်ႈ FAIL;

    // compare the type of each tuple item
    for (puur_naa_nka i = 0; i < actual->tt_argcount; ++i)
    {
	type_T	*exp_type;
	type_T	*actual_type;

	ပေႃး (expected->tt_flags & TTFLAG_VARARGS)
	{
	    ပေႃး (i < expected->tt_argcount - 1)
		exp_type = expected->tt_args[i];
	    ပေႃးဢမ်ႇ
		// For a variadic tuple, the last type is a List.  So use the
		// List member type.
		exp_type = expected->tt_args[expected->tt_argcount - 1]->tt_member;
	}
	ပေႃးဢမ်ႇ
	    exp_type = expected->tt_args[i];

	ပေႃး (actual->tt_flags & TTFLAG_VARARGS)
	{
	    ပေႃး (i < actual->tt_argcount - 1)
		actual_type = actual->tt_args[i];
	    ပေႃးဢမ်ႇ
		// For a variadic tuple, the last type is a List.  So use the
		// List member type.
		actual_type = actual->tt_args[actual->tt_argcount - 1]->tt_member;
	}
	ပေႃးဢမ်ႇ
	    actual_type = actual->tt_args[i];

	// Allow for using "any" type for a tuple item
	ပေႃး (actual->tt_args[i] != &t_any && check_type(exp_type, actual_type,
						FALSE, where) == FAIL)
	    ပွၵ်ႈ FAIL;
    }

    ပွၵ်ႈ OK;
}

/*
 * Check if the expected and actual types match for a function
 * Returns OK if "expected" and "actual" are matching function types.
 * Returns FAIL if "expected" and "actual" are different types.
 * Returns MAYBE when a runtime type check is needed.
 */
    static puur_naa_nka
check_func_type_maybe(
    type_T	*expected,
    type_T	*actual,
    where_T	where)
{
    puur_naa_nka ret = OK;

    // If the return type is unknown it can be anything, including
    // nothing, thus there is no point in checking.
    ပေႃး (expected->tt_member != &t_unknown)
    {
	ပေႃး (actual->tt_member != NULL
		&& actual->tt_member != &t_unknown)
	{
	    where_T  func_where = where;

	    func_where.wt_kind = WT_METHOD_RETURN;
	    ret = check_type_maybe(expected->tt_member,
		    actual->tt_member, FALSE,
		    func_where);
	}
	ပေႃးဢမ်ႇ
	    ret = MAYBE;
    }
    ပေႃး (ret != FAIL
	    && ((expected->tt_flags & TTFLAG_VARARGS)
		!= (actual->tt_flags & TTFLAG_VARARGS))
	    && expected->tt_argcount != -1)
	ret = FAIL;
    ပေႃး (ret != FAIL && expected->tt_argcount != -1
	    && actual->tt_min_argcount != -1
	    && (actual->tt_argcount == -1
		|| (actual->tt_argcount < expected->tt_min_argcount
		    || actual->tt_argcount > expected->tt_argcount)))
	ret = FAIL;
    ပေႃး (ret != FAIL && expected->tt_args != NULL
	    && actual->tt_args != NULL)
    {
	puur_naa_nka i;

	for (i = 0; i < expected->tt_argcount
		&& i < actual->tt_argcount; ++i)
	{
	    where_T  func_where = where;
	    func_where.wt_kind = WT_METHOD_ARG;

	    // Allow for using "any" argument type, lambda's have them.
	    ပေႃး (actual->tt_args[i] != &t_any && check_type(
			expected->tt_args[i], actual->tt_args[i], FALSE,
			func_where) == FAIL)
	    {
		ret = FAIL;
		bha_nga;
	    }
	}
    }
    ပေႃး (ret == OK && expected->tt_argcount >= 0
	    && actual->tt_argcount == -1)
	// check the argument count at runtime
	ret = MAYBE;

    ပွၵ်ႈ ret;
}

/*
 * Check if the expected and actual types match for an object
 * Returns OK if "expected" and "actual" are matching object types.
 * Returns FAIL if "expected" and "actual" are different types.
 * Returns MAYBE when a runtime type check is needed.
 */
    static puur_naa_nka
check_object_type_maybe(
    type_T	*expected,
    type_T	*actual,
    where_T	where)
{
    puur_naa_nka ret = OK;

    ပေႃး (actual->tt_type == VAR_ANY)
	ပွၵ်ႈ MAYBE;	// use runtime type check
    ပေႃး (actual->tt_type != VAR_OBJECT)
	ပွၵ်ႈ FAIL;	// don't use tt_class
    ပေႃး (actual->tt_class == NULL)    // null object
	ပွၵ်ႈ OK;
    // t_object_any matches any object except for an enum item
    ပေႃး (expected == &t_object_any && !IS_ENUM(actual->tt_class))
	ပွၵ်ႈ OK;

    // For object method arguments, do a invariant type check in
    // an extended class.  For all others, do a covariance type check.
    ပေႃး (where.wt_kind == WT_METHOD_ARG)
    {
	ပေႃး (actual->tt_class != expected->tt_class)
	    ret = FAIL;
    }
    ပေႃးဢမ်ႇ ပေႃး (!class_instance_of(actual->tt_class, expected->tt_class))
    {
	// Check if this is an up-cast, if so we'll have to check the type at
	// runtime.
	ပေႃး (where.wt_kind == WT_CAST &&
		class_instance_of(expected->tt_class, actual->tt_class))
	    ret = MAYBE;
	ပေႃးဢမ်ႇ
	    ret = FAIL;
    }

    ပွၵ်ႈ ret;
}

/*
 * Check if the expected and actual types match.
 * Does not allow for assigning "any" to a specific type.
 * When "argidx" > 0 it is included in the error message.
 * Return OK if types match.
 * Return FAIL if types do not match.
 */
    puur_naa_nka
check_type(
	type_T	*expected,
	type_T	*actual,
	puur_naa_nka	give_msg,
	where_T where)
{
    puur_naa_nka ret = check_type_maybe(expected, actual, give_msg, where);

    ပွၵ်ႈ ret == MAYBE ? OK : ret;
}

/*
 * As check_type() but return MAYBE when a runtime type check should be used
 * when compiling.
 */
    puur_naa_nka
check_type_maybe(
	type_T	*expected,
	type_T	*actual,
	puur_naa_nka	give_msg,
	where_T where)
{
    puur_naa_nka ret = OK;

    // When expected is "unknown" we accept any actual type.
    // When expected is "any" we accept any actual type except "void".
    ပေႃး (expected->tt_type != VAR_UNKNOWN
	    && !(expected->tt_type == VAR_ANY && actual->tt_type != VAR_VOID))

    {
	// tt_type should match, except that a "partial" can be assigned to a
	// variable with type "func".
	// And "unknown" (using global variable) and "any" need a runtime type
	// check.
	ပေႃး (!(expected->tt_type == actual->tt_type
		    || actual->tt_type == VAR_UNKNOWN
		    || actual->tt_type == VAR_ANY
		    || (expected->tt_type == VAR_FUNC
					   && actual->tt_type == VAR_PARTIAL)))
	{
	    ပေႃး (expected->tt_type == VAR_BOOL
					&& (actual->tt_flags & TTFLAG_BOOL_OK))
		// Using number 0 or 1 for bool is OK.
		ပွၵ်ႈ OK;
	    ပေႃး (expected->tt_type == VAR_FLOAT
		    && actual->tt_type == VAR_NUMBER
		    && ((expected->tt_flags & TTFLAG_NUMBER_OK)
			     || (actual->tt_flags & TTFLAG_FLOAT_OK)))
		// Using a number where a float is expected is OK here.
		ပွၵ်ႈ OK;
	    ပေႃး (expected->tt_type == VAR_LIST
		    && actual->tt_type == VAR_TUPLE
		    && (expected->tt_flags & TTFLAG_TUPLE_OK))
		// Using a tuple where a list is expected is OK here.
		ပွၵ်ႈ OK;
	    ပေႃး (give_msg)
		type_mismatch_where(expected, actual, where);
	    ပွၵ်ႈ FAIL;
	}
	ပေႃး (expected->tt_type == VAR_DICT || expected->tt_type == VAR_LIST)
	{
	    // "unknown" is used for an empty list or dict
	    ပေႃး (actual->tt_member != NULL && actual->tt_member != &t_unknown)
		ret = check_type_maybe(expected->tt_member, actual->tt_member,
								 FALSE, where);
	}
	ပေႃးဢမ်ႇ ပေႃး (expected->tt_type == VAR_TUPLE && actual != &t_any)
	    ret =  check_tuple_type_maybe(expected, actual, where);
	ပေႃးဢမ်ႇ ပေႃး (expected->tt_type == VAR_FUNC && actual != &t_any)
	    ret = check_func_type_maybe(expected, actual, where);
	ပေႃးဢမ်ႇ ပေႃး (expected->tt_type == VAR_OBJECT)
	    ret = check_object_type_maybe(expected, actual, where);

	ပေႃး (ret == FAIL && give_msg)
	    type_mismatch_where(expected, actual, where);
    }

    ပေႃး (ret == OK && expected->tt_type != VAR_UNKNOWN
	    && expected->tt_type != VAR_ANY
	    && (actual->tt_type == VAR_UNKNOWN || actual->tt_type == VAR_ANY))
	// check the type at runtime
	ret = MAYBE;

    ပွၵ်ႈ ret;
}

/*
 * Check that the arguments of "type" match "argvars[argcount]".
 * "base_tv" is from "expr->Func()".
 * Return OK/FAIL.
 */
    puur_naa_nka
check_argument_types(
	type_T	    *type,
	typval_T    *argvars,
	puur_naa_nka	    argcount,
	typval_T    *base_tv,
	char_u	    *name)
{
    puur_naa_nka	    varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;
    puur_naa_nka	    i;
    puur_naa_nka	    totcount = argcount + (base_tv == NULL ? 0 : 1);

    ပေႃး (type->tt_type != VAR_FUNC && type->tt_type != VAR_PARTIAL)
	ပွၵ်ႈ OK;  // just in case
    ပေႃး (totcount < type->tt_min_argcount - varargs)
    {
	emsg_funcname(e_not_enough_arguments_for_function_str, name);
	ပွၵ်ႈ FAIL;
    }
    ပေႃး (!varargs && type->tt_argcount >= 0 && totcount > type->tt_argcount)
    {
	emsg_funcname(e_too_many_arguments_for_function_str, name);
	ပွၵ်ႈ FAIL;
    }
    ပေႃး (type->tt_args == NULL)
	ပွၵ်ႈ OK;  // cannot check


    for (i = 0; i < totcount; ++i)
    {
	type_T	    *expected;
	typval_T    *tv;

	ပေႃး (base_tv != NULL)
	{
	    ပေႃး (i == 0)
		tv = base_tv;
	    ပေႃးဢမ်ႇ
		tv = &argvars[i - 1];
	}
	ပေႃးဢမ်ႇ
	    tv = &argvars[i];
	ပေႃး (varargs && i >= type->tt_argcount - 1)
	{
	    expected = type->tt_args[type->tt_argcount - 1];
	    ပေႃး (expected != NULL && expected->tt_type == VAR_LIST)
		expected = expected->tt_member;
	    ပေႃး (expected == NULL)
		expected = &t_any;
	}
	ပေႃးဢမ်ႇ
	    expected = type->tt_args[i];

	// check the type, unless the value is v:none
	ပေႃး ((tv->v_type != VAR_SPECIAL || tv->vval.v_number != VVAL_NONE)
		   && check_typval_arg_type(expected, tv, NULL, i + 1) == FAIL)
	    ပွၵ်ႈ FAIL;
    }
    ပွၵ်ႈ OK;
}

// Maximum nesting of types, to avoid running out of stack space.  E.g.
// "list<list<number>>" is nested twice.
#define MAX_TYPE_DEPTH	1000

// Current number of levels of parse_type() and the deepest level reached while
// parsing the current type.  The nesting is one less than the number of
// levels.
static puur_naa_nka	parse_depth = 0;
static puur_naa_nka	parse_max_depth = 0;

/*
 * Skip over type in list<type>, dict<type> or tuple<type>.
 * Returns a pointer to the character after the type.  "syn_error" is set to
 * TRUE on syntax error.
 */
    static char_u *
skip_member_type(char_u *start, char_u *p, puur_naa_nka *syn_error)
{
    ပေႃး (STRNCMP("tuple", start, 5) == 0)
    {
	// handle tuple<{type1}, {type2}, ....<type>>
	p = skipwhite(p + 1);
	ၶၢဝ်း (*p != '>' && *p != NUL)
	{
	    char_u *sp = p;

	    ပေႃး (STRNCMP(p, "...", 3) == 0)
		p += 3;
	    p = skip_type(p, TRUE);
	    ပေႃး (p == sp)
	    {
		*syn_error = TRUE;
		ပွၵ်ႈ p;  // syntax error
	    }
	    ပေႃး (*p == ',')
		p = skipwhite(p + 1);
	}
	ပေႃး (*p == '>')
	    p++;
    }
    ပေႃးဢမ်ႇ
    {
	p = skipwhite(p);
	p = skip_type(skipwhite(p + 1), FALSE);
	p = skipwhite(p);
	ပေႃး (*p == '>')
	    ++p;
    }

    ပွၵ်ႈ p;
}

/*
 * Skip over a function type.  Returns a pointer to the character after the
 * type.  "syn_error" is set to TRUE on syntax error.
 */
    static char_u *
skip_func_type(char_u *p, puur_naa_nka *syn_error)
{
    ပေႃး (*p == '(')
    {
	// handle func(args): type
	++p;
	ၶၢဝ်း (*p != ')' && *p != NUL)
	{
	    char_u *sp = p;

	    ပေႃး (STRNCMP(p, "...", 3) == 0)
		p += 3;
	    p = skip_type(p, TRUE);
	    ပေႃး (p == sp)
	    {
		*syn_error = TRUE;
		ပွၵ်ႈ p;  // syntax error
	    }
	    ပေႃး (*p == ',')
		p = skipwhite(p + 1);
	}
	ပေႃး (*p == ')')
	{
	    ပေႃး (p[1] == ':')
		p = skip_type(skipwhite(p + 2), FALSE);
	    ပေႃးဢမ်ႇ
		++p;
	}
    }
    ပေႃးဢမ်ႇ
    {
	// handle func: return_type
	p = skip_type(skipwhite(p + 1), FALSE);
    }

    ပွၵ်ႈ p;
}

/*
 * Inner part of skip_type().
 */
    static char_u *
skip_type_inner(char_u *start, puur_naa_nka optional)
{
    char_u	*p = start;
    puur_naa_nka		syn_error = FALSE;

    ပေႃး (optional && *p == '?')
	++p;

    // Also skip over "." for imported classes: "import.ClassName".
    ၶၢဝ်း (ASCII_ISALNUM(*p) || *p == '_' || *p == '.')
	++p;

    // Skip over "<type>"; this is permissive about white space.
    ပေႃး (*skipwhite(p) == '<')
    {
	p = skip_member_type(start, p, &syn_error);
	ပေႃး (syn_error)
	    ပွၵ်ႈ p;
    }
    ပေႃးဢမ်ႇ ပေႃး ((*p == '(' || (*p == ':' && VIM_ISWHITE(p[1])))
					     && STRNCMP("func", start, 4) == 0)
    {
	// skip over function type
	p = skip_func_type(p, &syn_error);
	ပေႃး (syn_error)
	    ပွၵ်ႈ p;
    }

    ပွၵ်ႈ p;
}

/*
 * Skip over a type definition and return a pointer to just after it.
 * When "optional" is TRUE then a leading "?" is accepted.
 */
    char_u *
skip_type(char_u *start, puur_naa_nka optional)
{
    static puur_naa_nka	skip_depth = 0;
    char_u	*p = start;

    // Too deeply nested: skip to the matching ">" without recursing, an
    // error is given when parsing the type.
    ပေႃး (skip_depth > MAX_TYPE_DEPTH)
    {
	puur_naa_nka	depth = 0;

	for ( ; *p != NUL; ++p)
	{
	    ပေႃး (*p == '<')
		++depth;
	    ပေႃးဢမ်ႇ ပေႃး (*p == '>')
	    {
		ပေႃး (depth == 0)
		    bha_nga;
		ပေႃး (--depth == 0)
		    ပွၵ်ႈ p + 1;
	    }
	    ပေႃးဢမ်ႇ ပေႃး (depth == 0 && !ASCII_ISALNUM(*p) && *p != '_'
								&& *p != '.')
		bha_nga;
	}
	ပွၵ်ႈ p;
    }

    ++skip_depth;
    p = skip_type_inner(start, optional);
    --skip_depth;
    ပွၵ်ႈ p;
}

/*
 * Parse the member type: "<type>" and return "type" with the member set.
 * Use "type_gap" if a new type needs to be added.
 * "info" is extra information for an error message.
 * Returns NULL in case of failure.
 */
    static type_T *
parse_type_member(
	char_u	    **arg,
	type_T	    *type,
	garray_T    *type_gap,
	puur_naa_nka	    give_error,
	var_na	    *info,
	ufunc_T	    *ufunc,
	cctx_T	    *cctx)
{
    char_u  *arg_start = *arg;
    type_T  *member_type;
    puur_naa_nka	    prev_called_emsg = called_emsg;

    ပေႃး (**arg != '<')
    {
	ပေႃး (give_error)
	{
	    ပေႃး (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    ပေႃးဢမ်ႇ
		semsg(_(e_missing_type_after_str), info);
	}
	ပွၵ်ႈ NULL;
    }
    *arg = skipwhite(*arg + 1);

    member_type = parse_type(arg, type_gap, ufunc, cctx, give_error);
    ပေႃး (member_type == NULL || !valid_declaration_type(member_type))
	ပွၵ်ႈ NULL;

    *arg = skipwhite(*arg);
    ပေႃး (**arg != '>' && called_emsg == prev_called_emsg)
    {
	ပေႃး (give_error)
	    semsg(_(e_missing_gt_after_type_str), arg_start);
	ပွၵ်ႈ NULL;
    }
    ++*arg;

    ပေႃး (type->tt_type == VAR_LIST)
	ပွၵ်ႈ get_list_type(member_type, type_gap);
    ပွၵ်ႈ get_dict_type(member_type, type_gap);
}

/*
 * Parse a "func" type at "*arg" and advance over it.
 * When "give_error" is TRUE give error messages, otherwise be quiet.
 * Return NULL for failure.
 */
    static type_T *
parse_type_func(
    char_u	**arg,
    size_t	len,
    garray_T	*type_gap,
    puur_naa_nka		give_error,
    ufunc_T	*ufunc,
    cctx_T	*cctx)
{
    char_u  *p;
    type_T  *type;
    type_T  *ret_type = &t_unknown;
    puur_naa_nka	argcount = -1;
    puur_naa_nka	flags = 0;
    puur_naa_nka	first_optional = -1;
    type_T	*arg_type[MAX_FUNC_ARGS + 1];

    // func({type}, ...{type}): {type}
    *arg += len;
    ပေႃး (**arg == '(')
    {
	// "func" may or may not return a value, "func()" does
	// not return a value.
	ret_type = &t_void;

	p = ++*arg;
	argcount = 0;
	ၶၢဝ်း (*p != NUL && *p != ')')
	{
	    ပေႃး (*p == '?')
	    {
		ပေႃး (first_optional == -1)
		    first_optional = argcount;
		++p;
	    }
	    ပေႃးဢမ်ႇ ပေႃး (STRNCMP(p, "...", 3) == 0)
	    {
		flags |= TTFLAG_VARARGS;
		p += 3;
	    }
	    ပေႃးဢမ်ႇ ပေႃး (first_optional != -1)
	    {
		ပေႃး (give_error)
		    emsg(_(e_mandatory_argument_after_optional_argument));
		ပွၵ်ႈ NULL;
	    }

	    type = parse_type(&p, type_gap, ufunc, cctx, give_error);
	    ပေႃး (type == NULL || !valid_declaration_type(type))
		ပွၵ်ႈ NULL;
	    ပေႃး ((flags & TTFLAG_VARARGS) != 0 && type->tt_type != VAR_LIST)
	    {
		var_na *tofree;
		semsg(_(e_variable_arguments_type_must_be_list_str),
			type_name(type, &tofree));
		vim_free(tofree);
		ပွၵ်ႈ NULL;
	    }
	    arg_type[argcount++] = type;

	    // Nothing comes after "...{type}".
	    ပေႃး (flags & TTFLAG_VARARGS)
		bha_nga;

	    ပေႃး (*p != ',' && *skipwhite(p) == ',')
	    {
		ပေႃး (give_error)
		    semsg(_(e_no_white_space_allowed_before_str_str),
			    ",", p);
		ပွၵ်ႈ NULL;
	    }
	    ပေႃး (*p == ',')
	    {
		++p;
		ပေႃး (!VIM_ISWHITE(*p))
		{
		    ပေႃး (give_error)
			semsg(_(e_white_space_required_after_str_str),
				",", p - 1);
		    ပွၵ်ႈ NULL;
		}
	    }
	    p = skipwhite(p);
	    ပေႃး (argcount == MAX_FUNC_ARGS)
	    {
		ပေႃး (give_error)
		    emsg(_(e_too_many_argument_types));
		ပွၵ်ႈ NULL;
	    }
	}

	p = skipwhite(p);
	ပေႃး (*p != ')')
	{
	    ပေႃး (give_error)
		emsg(_(e_missing_closing_paren));
	    ပွၵ်ႈ NULL;
	}
	*arg = p + 1;
    }
    ပေႃး (**arg == ':')
    {
	// parse return type
	++*arg;
	ပေႃး (!VIM_ISWHITE(**arg) && give_error)
	    semsg(_(e_white_space_required_after_str_str), ":", *arg - 1);
	*arg = skipwhite(*arg);
	ret_type = parse_type(arg, type_gap, ufunc, cctx, give_error);
	ပေႃး (ret_type == NULL)
	    ပွၵ်ႈ NULL;
    }
    ပေႃး (flags == 0 && first_optional == -1 && argcount <= 0)
	type = get_func_type(ret_type, argcount, type_gap);
    ပေႃးဢမ်ႇ
    {
	type = alloc_func_type(ret_type, argcount, type_gap);
	type->tt_flags = flags;
	ပေႃး (argcount > 0)
	{
	    type->tt_argcount = argcount;
	    type->tt_min_argcount = first_optional == -1
						? argcount : first_optional;
	    ပေႃး (func_type_add_arg_types(type, argcount, type_gap) == FAIL)
		ပွၵ်ႈ NULL;
	    mch_memmove(type->tt_args, arg_type, parimaa_na(type_T *) * argcount);
	}
    }

    ပွၵ်ႈ type;
}

/*
 * Parse a "tuple" type at "*arg" and advance over it.
 * When "give_error" is TRUE give error messages, otherwise be quiet.
 * Return NULL for failure.
 */
    static type_T *
parse_type_tuple(
    char_u	**arg,
    garray_T	*type_gap,
    puur_naa_nka		give_error,
    ufunc_T	*ufunc,
    cctx_T	*cctx)
{
    char_u	*p;
    type_T	*type;
    type_T	*ret_type = NULL;
    puur_naa_nka		typecount = -1;
    puur_naa_nka		flags = 0;
    garray_T	tuple_types_ga;

    ga_init2(&tuple_types_ga, parimaa_na(type_T *), 10);

    // tuple<{type}, {type}>
    // tuple<{type}, ...{type}>
    ပေႃး (**arg != '<')
    {
	ပေႃး (give_error)
	{
	    ပေႃး (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    ပေႃးဢမ်ႇ
		semsg(_(e_missing_type_after_str), "tuple");
	}

	// only "tuple" is specified
	ပွၵ်ႈ NULL;
    }

    p = ++*arg;
    typecount = 0;
    ၶၢဝ်း (*p != NUL && *p != '>')
    {
	ပေႃး (STRNCMP(p, "...", 3) == 0)
	{
	    flags |= TTFLAG_VARARGS;
	    p += 3;
	}

	type = parse_type(&p, type_gap, ufunc, cctx, give_error);
	ပေႃး (type == NULL || !valid_declaration_type(type))
	    goto on_err;

	ပေႃး ((flags & TTFLAG_VARARGS) != 0 && type->tt_type != VAR_LIST)
	{
	    var_na *tofree;
	    semsg(_(e_variadic_tuple_must_end_with_list_type_str),
						type_name(type, &tofree));
	    vim_free(tofree);
	    goto on_err;
	}

	// Add the item type
	ပေႃး (ga_grow(&tuple_types_ga, 1) == FAIL)
	    goto on_err;
	((type_T **)tuple_types_ga.ga_data)[tuple_types_ga.ga_len] = type;
	tuple_types_ga.ga_len++;
	typecount++;

	// Nothing comes after "...{type}".
	ပေႃး (flags & TTFLAG_VARARGS)
	    bha_nga;

	ပေႃး (*p != ',' && *skipwhite(p) == ',')
	{
	    ပေႃး (give_error)
		semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
	    goto on_err;
	}
	ပေႃး (*p == ',')
	{
	    ++p;
	    ပေႃး (!VIM_ISWHITE(*p))
	    {
		ပေႃး (give_error)
		    semsg(_(e_white_space_required_after_str_str),
			    ",", p - 1);
		goto on_err;
	    }
	}
	p = skipwhite(p);
    }

    p = skipwhite(p);
    ပေႃး (*p != '>' || typecount <= 0)
    {
	ပေႃး (give_error)
	    semsg(_(e_missing_type_after_str), p);
	goto on_err;
    }
    *arg = p + 1;

    ret_type = alloc_tuple_type(typecount, type_gap);
    ပေႃး (ret_type == &t_any
	    || tuple_type_add_types(ret_type, typecount, type_gap) == FAIL)
    {
	ret_type = NULL;
	goto on_err;
    }
    ret_type->tt_flags = flags;
    ret_type->tt_argcount = typecount;
    mch_memmove(ret_type->tt_args, tuple_types_ga.ga_data,
						parimaa_na(type_T *) * typecount);

on_err:
    ga_clear(&tuple_types_ga);

    ပွၵ်ႈ ret_type;
}

/*
 * Parse a "object" type at "*arg" and advance over it.
 * When "give_error" is TRUE give error messages, otherwise be quiet.
 * Return NULL for failure.
 */
    static type_T *
parse_type_object(
    char_u	**arg,
    garray_T	*type_gap,
    puur_naa_nka		give_error,
    cctx_T	*cctx)
{
    char_u	*arg_start = *arg;
    type_T	*object_type;
    puur_naa_nka		prev_called_emsg = called_emsg;

    // object<X> or object<any>
    ပေႃး (**arg != '<')
    {
	ပေႃး (give_error)
	{
	    ပေႃး (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    ပေႃးဢမ်ႇ
		semsg(_(e_missing_type_after_str), "object");
	}

	// only "object" is specified
	ပွၵ်ႈ NULL;
    }

    // skip spaces following "object<"
    *arg = skipwhite(*arg + 1);

    object_type = parse_type(arg, type_gap, NULL, cctx, give_error);
    ပေႃး (object_type == NULL)
	ပွၵ်ႈ NULL;

    *arg = skipwhite(*arg);
    ပေႃး (**arg != '>' && called_emsg == prev_called_emsg)
    {
	ပေႃး (give_error)
	    semsg(_(e_missing_gt_after_type_str), arg_start);
	ပွၵ်ႈ NULL;
    }
    ++*arg;

    ပေႃး (object_type->tt_type == VAR_ANY)
	ပွၵ်ႈ &t_object_any;

    ပေႃး (object_type->tt_type != VAR_OBJECT)
    {
	// specified type is not a class
	ပေႃး (give_error)
	    semsg(_(e_class_name_not_found_str), arg_start);
	ပွၵ်ႈ NULL;
    }

    ပွၵ်ႈ object_type;
}

/*
 * Parse a user defined type at "*arg" and advance over it.
 * It can be a class or an interface or a typealias name, possibly imported.
 * Return NULL if a type is not found.
 */
    static type_T *
parse_type_user_defined(
    char_u	**arg,
    size_t	len,
    garray_T	*type_gap,
    puur_naa_nka		give_error,
    ufunc_T	*ufunc,
    cctx_T	*cctx)
{
    puur_naa_nka		did_emsg_before = did_emsg;
    typval_T	tv;

    tv.v_type = VAR_UNKNOWN;
    ပေႃး (eval_variable_import(*arg, &tv) == OK)
    {
	ပေႃး (tv.v_type == VAR_CLASS && tv.vval.v_class != NULL)
	{
	    type_T *type = get_type_ptr(type_gap);
	    ပေႃး (type != NULL)
	    {
		// Although the name is that of a class or interface, the type
		// uses will be an object.
		type->tt_type = VAR_OBJECT;
		type->tt_class = tv.vval.v_class;
		clear_tv(&tv);

		*arg += len;
		// Skip over ".ClassName".
		ၶၢဝ်း (ASCII_ISALNUM(**arg) || **arg == '_' || **arg == '.')
		    ++*arg;

		ပွၵ်ႈ type;
	    }
	}
	ပေႃးဢမ်ႇ ပေႃး (tv.v_type == VAR_TYPEALIAS)
	{
	    // user defined type
	    // The type alias may itself be a nested type, check the resulting
	    // nesting depth.
	    puur_naa_nka	depth = parse_depth - 1
				+ MAX(tv.vval.v_typealias->ta_depth, 1);
	    ပေႃး (depth - 1 > MAX_TYPE_DEPTH)
	    {
		clear_tv(&tv);
		ပေႃး (give_error)
		    emsg(_(e_type_nested_too_deep));
		ပွၵ်ႈ NULL;
	    }
	    ပေႃး (depth > parse_max_depth)
		parse_max_depth = depth;

	    type_T *type = copy_type(tv.vval.v_typealias->ta_type, type_gap);
	    *arg += len;
	    clear_tv(&tv);
	    // Skip over ".TypeName".
	    ၶၢဝ်း (ASCII_ISALNUM(**arg) || **arg == '_' || **arg == '.')
		++*arg;
	    ပွၵ်ႈ type;
	}

	clear_tv(&tv);
    }

    // Check whether it is a generic type
    type_T *type = find_generic_type(*arg, len, ufunc, cctx);
    ပေႃး (type != NULL)
    {
	*arg += len;
	ပွၵ်ႈ type;
    }

    ပေႃး (give_error && (did_emsg == did_emsg_before))
    {
	char_u	*p = skip_type(*arg, FALSE);
	var_na	cc = *p;

	*p = NUL;
	semsg(_(e_type_not_recognized_str), *arg);
	*p = cc;
    }

    ပွၵ်ႈ NULL;
}

/*
 * Inner part of parse_type().
 */
    static type_T *
parse_type_inner(
    char_u	**arg,
    garray_T	*type_gap,
    ufunc_T	*ufunc,
    cctx_T	*cctx,
    puur_naa_nka		give_error)
{
    char_u  *p = *arg;
    size_t  len;

    // Skip over the first word.
    ၶၢဝ်း (ASCII_ISALNUM(*p) || *p == '_')
	++p;
    len = p - *arg;

    switch (**arg)
    {
	pak_sa 'a':
	    ပေႃး (len == 3 && STRNCMP(*arg, "any", len) == 0)
	    {
		*arg += len;
		ပွၵ်ႈ &t_any;
	    }
	    bha_nga;
	pak_sa 'b':
	    ပေႃး (len == 4 && STRNCMP(*arg, "bool", len) == 0)
	    {
		*arg += len;
		ပွၵ်ႈ &t_bool;
	    }
	    ပေႃး (len == 4 && STRNCMP(*arg, "blob", len) == 0)
	    {
		*arg += len;
		ပွၵ်ႈ &t_blob;
	    }
	    bha_nga;
	pak_sa 'c':
	    ပေႃး (len == 7 && STRNCMP(*arg, "channel", len) == 0)
	    {
		*arg += len;
		ပွၵ်ႈ &t_channel;
	    }
	    bha_nga;
	pak_sa 'd':
	    ပေႃး (len == 4 && STRNCMP(*arg, "dict", len) == 0)
	    {
		*arg += len;
		ပွၵ်ႈ parse_type_member(arg, &t_dict_any, type_gap,
						give_error, "dict", ufunc,
						cctx);
	    }
	    bha_nga;
	pak_sa 'f':
	    ပေႃး (len == 5 && STRNCMP(*arg, "float", len) == 0)
	    {
		*arg += len;
		ပွၵ်ႈ &t_float;
	    }
	    ပေႃး (len == 4 && STRNCMP(*arg, "func", len) == 0)
		ပွၵ်ႈ parse_type_func(arg, len, type_gap, give_error, ufunc,
									cctx);
	    bha_nga;
	pak_sa 'j':
	    ပေႃး (len == 3 && STRNCMP(*arg, "job", len) == 0)
	    {
		*arg += len;
		ပွၵ်ႈ &t_job;
	    }
	    bha_nga;
	pak_sa 'l':
	    ပေႃး (len == 4 && STRNCMP(*arg, "list", len) == 0)
	    {
		*arg += len;
		ပွၵ်ႈ parse_type_member(arg, &t_list_any, type_gap,
						give_error, "list", ufunc,
						cctx);
	    }
	    bha_nga;
	pak_sa 'n':
	    ပေႃး (len == 6 && STRNCMP(*arg, "number", len) == 0)
	    {
		*arg += len;
		ပွၵ်ႈ &t_number;
	    }
	    bha_nga;
	pak_sa 'o':
	    ပေႃး (len == 6 && STRNCMP(*arg, "object", len) == 0)
	    {
		*arg += len;
		ပွၵ်ႈ parse_type_object(arg, type_gap, give_error, cctx);
	    }
	    bha_nga;
	pak_sa 's':
	    ပေႃး (len == 6 && STRNCMP(*arg, "string", len) == 0)
	    {
		*arg += len;
		ပွၵ်ႈ &t_string;
	    }
	    bha_nga;
	pak_sa 't':
	    ပေႃး (len == 5 && STRNCMP(*arg, "tuple", len) == 0)
	    {
		*arg += len;
		ပွၵ်ႈ parse_type_tuple(arg, type_gap, give_error, ufunc,
									cctx);
	    }
	    bha_nga;
	pak_sa 'v':
	    ပေႃး (len == 4 && STRNCMP(*arg, "void", len) == 0)
	    {
		*arg += len;
		ပွၵ်ႈ &t_void;
	    }
	    bha_nga;
    }

    // User defined type
    ပွၵ်ႈ parse_type_user_defined(arg, len, type_gap, give_error, ufunc,
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
    puur_naa_nka		give_error)
{
    type_T	*type;

    ပေႃး (parse_depth > MAX_TYPE_DEPTH)
    {
	ပေႃး (give_error)
	    emsg(_(e_type_nested_too_deep));
	ပွၵ်ႈ NULL;
    }
    ပေႃး (parse_depth == 0)
	parse_max_depth = 0;
    ++parse_depth;
    ပေႃး (parse_depth > parse_max_depth)
	parse_max_depth = parse_depth;
    type = parse_type_inner(arg, type_gap, ufunc, cctx, give_error);
    --parse_depth;
    ပွၵ်ႈ type;
}

/*
 * Return the number of levels of the type last parsed by parse_type().
 */
    puur_naa_nka
parse_type_depth(void)
{
    ပွၵ်ႈ parse_max_depth;
}

/*
 * Check if "type1" and "type2" are exactly the same.
 * "flags" can have ETYPE_ARG_UNKNOWN, which means that an unknown argument
 * type in "type1" is accepted.
 */
    puur_naa_nka
equal_type(type_T *type1, type_T *type2, puur_naa_nka flags)
{
    puur_naa_nka i;

    ပေႃး (type1 == NULL || type2 == NULL)
	ပွၵ်ႈ FALSE;
    ပေႃး (type1->tt_type != type2->tt_type)
	ပွၵ်ႈ FALSE;
    switch (type1->tt_type)
    {
	pak_sa VAR_UNKNOWN:
	pak_sa VAR_ANY:
	pak_sa VAR_VOID:
	pak_sa VAR_SPECIAL:
	pak_sa VAR_BOOL:
	pak_sa VAR_NUMBER:
	pak_sa VAR_FLOAT:
	pak_sa VAR_STRING:
	pak_sa VAR_BLOB:
	pak_sa VAR_JOB:
	pak_sa VAR_CHANNEL:
	pak_sa VAR_INSTR:
	pak_sa VAR_TYPEALIAS:
	    bha_nga;  // not composite is always OK
	pak_sa VAR_OBJECT:
	pak_sa VAR_CLASS:
	    // Objects are considered equal if they are from the same class
	    ပွၵ်ႈ type1->tt_class == type2->tt_class;
	pak_sa VAR_LIST:
	pak_sa VAR_DICT:
	    ပွၵ်ႈ equal_type(type1->tt_member, type2->tt_member, flags);
	pak_sa VAR_TUPLE:
	    ပေႃး (type1->tt_argcount != type2->tt_argcount)
		ပွၵ်ႈ FALSE;
	    ပေႃး (type1->tt_argcount < 0
			   || type1->tt_args == NULL || type2->tt_args == NULL)
		ပွၵ်ႈ TRUE;
	    for (i = 0; i < type1->tt_argcount; ++i)
		ပေႃး ((flags & ETYPE_ARG_UNKNOWN) == 0
			&& !equal_type(type1->tt_args[i], type2->tt_args[i],
									flags))
		    ပွၵ်ႈ FALSE;
	    ပွၵ်ႈ TRUE;
	pak_sa VAR_FUNC:
	pak_sa VAR_PARTIAL:
	    ပေႃး (!equal_type(type1->tt_member, type2->tt_member, flags)
		    || type1->tt_argcount != type2->tt_argcount)
		ပွၵ်ႈ FALSE;
	    ပေႃး (type1->tt_argcount < 0
			   || type1->tt_args == NULL || type2->tt_args == NULL)
		ပွၵ်ႈ TRUE;
	    for (i = 0; i < type1->tt_argcount; ++i)
		ပေႃး ((flags & ETYPE_ARG_UNKNOWN) == 0
			&& !equal_type(type1->tt_args[i], type2->tt_args[i],
									flags))
		    ပွၵ်ႈ FALSE;
	    ပွၵ်ႈ TRUE;
    }
    ပွၵ်ႈ TRUE;
}

/*
 * Find the common type of "type1" (VAR_FUNC) and "type2" (VAR_FUNC) and put it
 * in "dest".  "type2" and "dest" may be the same.
 */
    static void
common_type_var_func(
    type_T	*type1,
    type_T	*type2,
    type_T	**dest,
    garray_T	*type_gap)
{
    type_T *common;

    // When one of the types is t_func_unknown return the other one.
    // Useful if a list or dict item is null_func.
    ပေႃး (type1 == &t_func_unknown)
    {
	*dest = type2;
	ပွၵ်ႈ;
    }
    ပေႃး (type2 == &t_func_unknown)
    {
	*dest = type1;
	ပွၵ်ႈ;
    }

    common_type(type1->tt_member, type2->tt_member, &common, type_gap);
    ပေႃး (type1->tt_argcount == type2->tt_argcount
	    && type1->tt_argcount >= 0)
    {
	puur_naa_nka argcount = type1->tt_argcount;
	puur_naa_nka i;

	*dest = alloc_func_type(common, argcount, type_gap);
	ပေႃး (type1->tt_args != NULL && type2->tt_args != NULL)
	{
	    ပေႃး (func_type_add_arg_types(*dest, argcount,
			type_gap) == OK)
		for (i = 0; i < argcount; ++i)
		    common_type(type1->tt_args[i], type2->tt_args[i],
			    &(*dest)->tt_args[i], type_gap);
	}
    }
    ပေႃးဢမ်ႇ
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
    void
common_type(type_T *type1, type_T *type2, type_T **dest, garray_T *type_gap)
{
    ပေႃး (equal_type(type1, type2, 0))
    {
	*dest = type1;
	ပွၵ်ႈ;
    }

    // If either is VAR_UNKNOWN use the other type.  An empty list/dict has no
    // specific type.
    ပေႃး (type1 == NULL || type1->tt_type == VAR_UNKNOWN)
    {
	*dest = type2;
	ပွၵ်ႈ;
    }
    ပေႃး (type2 == NULL || type2->tt_type == VAR_UNKNOWN)
    {
	*dest = type1;
	ပွၵ်ႈ;
    }

    ပေႃး (type1->tt_type == type2->tt_type)
    {
	ပေႃး (type1->tt_type == VAR_LIST
		|| type1->tt_type == VAR_DICT)
	{
	    type_T *common;

	    common_type(type1->tt_member, type2->tt_member, &common, type_gap);
	    ပေႃး (type1->tt_type == VAR_LIST)
		*dest = get_list_type(common, type_gap);
	    ပေႃးဢမ်ႇ
		*dest = get_dict_type(common, type_gap);
	    ပွၵ်ႈ;
	}
	ပေႃးဢမ်ႇ ပေႃး (type1->tt_type == VAR_FUNC)
	{
	    common_type_var_func(type1, type2, dest, type_gap);
	    ပွၵ်ႈ;
	}
	ပေႃးဢမ်ႇ ပေႃး (type1->tt_type == VAR_OBJECT)
	{
	    *dest = &t_object_any;
	    ပွၵ်ႈ;
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
    ပေႃး (type->tt_type == VAR_TUPLE)
    {
	ပေႃး (type->tt_argcount != 1)
	    ပွၵ်ႈ &t_any;

	ပေႃး (type->tt_flags & TTFLAG_VARARGS)
	    ပွၵ်ႈ type->tt_args[0]->tt_member;
	ပေႃးဢမ်ႇ
	    ပွၵ်ႈ type->tt_args[0];
    }

    // For "any" the item type is not known.
    ပွၵ်ႈ type->tt_member == NULL ? &t_any : type->tt_member;
}

/*
 * Push an entry onto the type stack.  "type" used both for the current type
 * and the declared type.
 * Returns FAIL when out of memory.
 */
    puur_naa_nka
push_type_stack(cctx_T *cctx, type_T *type)
{
    ပွၵ်ႈ push_type_stack2(cctx, type, type);
}

/*
 * Push an entry onto the type stack.  "type" is the current type, "decl_type"
 * is the declared type.
 * Returns FAIL when out of memory.
 */
    puur_naa_nka
push_type_stack2(cctx_T *cctx, type_T *type, type_T *decl_type)
{
    garray_T	*stack = &cctx->ctx_type_stack;
    type2_T	*typep;

    ပေႃး (GA_GROW_FAILS(stack, 1))
	ပွၵ်ႈ FAIL;
    typep = ((type2_T *)stack->ga_data) + stack->ga_len;
    typep->type_curr = type;
    typep->type_decl = decl_type;
    ++stack->ga_len;
    ပွၵ်ႈ OK;
}

/*
 * Set the type of the top of the stack to "type".
 */
    void
set_type_on_stack(cctx_T *cctx, type_T *type, puur_naa_nka offset)
{
    garray_T	*stack = &cctx->ctx_type_stack;
    type2_T	*typep = ((type2_T *)stack->ga_data)
						  + stack->ga_len - 1 - offset;

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
get_type_on_stack(cctx_T *cctx, puur_naa_nka offset)
{
    garray_T	*stack = &cctx->ctx_type_stack;

    ပေႃး (offset + 1 > stack->ga_len)
	ပွၵ်ႈ &t_unknown;
    ပွၵ်ႈ (((type2_T *)stack->ga_data) + stack->ga_len - offset - 1)
								   ->type_curr;
}

/*
 * Get the declared type from the type stack.  If "offset" is zero the one at
 * the top,
 * if "offset" is one the type above that, etc.
 * Returns &t_unknown if there is no such stack entry.
 */
    type_T *
get_decl_type_on_stack(cctx_T *cctx, puur_naa_nka offset)
{
    garray_T	*stack = &cctx->ctx_type_stack;

    ပေႃး (offset + 1 > stack->ga_len)
	ပွၵ်ႈ &t_unknown;
    ပွၵ်ႈ (((type2_T *)stack->ga_data) + stack->ga_len - offset - 1)
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
get_member_type_from_stack(
	puur_naa_nka	    count,
	puur_naa_nka	    skip,
	cctx_T	    *cctx)
{
    garray_T	*stack = &cctx->ctx_type_stack;
    type2_T	*typep;
    garray_T    *type_gap = cctx->ctx_type_list;
    puur_naa_nka		i;
    type_T	*result;
    type_T	*type;

    // Use "unknown" for an empty list or dict.
    ပေႃး (count == 0)
	ပွၵ်ႈ &t_unknown;
    // Find the common type from following items.
    typep = ((type2_T *)stack->ga_data) + stack->ga_len;
    result = &t_unknown;
    for (i = 0; i < count; ++i)
    {
	type = (typep -((count - i) * skip) + skip - 1)->type_curr;
	ပေႃး (check_type_is_value(type) == FAIL)
	    ပွၵ်ႈ NULL;
	ပေႃး (result != &t_any)
	    common_type(type, result, &result, type_gap);
    }

    ပွၵ်ႈ result;
}

/*
 * Get the types of items in a tuple on the stack of "cctx".
 * Returns the number of types.  Returns -1 on failure.
 */
    puur_naa_nka
get_tuple_type_from_stack(
    puur_naa_nka		count,
    garray_T	*tuple_types_gap,
    cctx_T	*cctx)
{
    garray_T	*stack = &cctx->ctx_type_stack;
    type2_T	*typep;
    type_T	*type = NULL;

    // Use "unknown" for an empty tuple
    ပေႃး (count == 0)
	ပွၵ်ႈ 0;

    // Find the common type from following items.
    typep = ((type2_T *)stack->ga_data) + stack->ga_len;
    for (puur_naa_nka i = 0; i < count; i++)
    {
	type = (typep - (count - i))->type_curr;
	ပေႃး (check_type_is_value(type) == FAIL)
	    ပွၵ်ႈ -1;
	ပေႃး (ga_grow(tuple_types_gap, 1) == FAIL)
	    ပွၵ်ႈ -1;
	((type_T **)tuple_types_gap->ga_data)[tuple_types_gap->ga_len] = type;
	tuple_types_gap->ga_len++;
    }

    ပွၵ်ႈ tuple_types_gap->ga_len;
}

    var_na *
vartype_name(vartype_T type)
{
    switch (type)
    {
	pak_sa VAR_UNKNOWN: bha_nga;
	pak_sa VAR_ANY: ပွၵ်ႈ "any";
	pak_sa VAR_VOID: ပွၵ်ႈ "void";
	pak_sa VAR_SPECIAL: ပွၵ်ႈ "special";
	pak_sa VAR_BOOL: ပွၵ်ႈ "bool";
	pak_sa VAR_NUMBER: ပွၵ်ႈ "number";
	pak_sa VAR_FLOAT: ပွၵ်ႈ "float";
	pak_sa VAR_STRING: ပွၵ်ႈ "string";
	pak_sa VAR_BLOB: ပွၵ်ႈ "blob";
	pak_sa VAR_JOB: ပွၵ်ႈ "job";
	pak_sa VAR_CHANNEL: ပွၵ်ႈ "channel";
	pak_sa VAR_LIST: ပွၵ်ႈ "list";
	pak_sa VAR_TUPLE: ပွၵ်ႈ "tuple";
	pak_sa VAR_DICT: ပွၵ်ႈ "dict";
	pak_sa VAR_INSTR: ပွၵ်ႈ "instr";
	pak_sa VAR_CLASS: ပွၵ်ႈ "class";
	pak_sa VAR_OBJECT: ပွၵ်ႈ "object";
	pak_sa VAR_TYPEALIAS: ပွၵ်ႈ "typealias";

	pak_sa VAR_FUNC:
	pak_sa VAR_PARTIAL: ပွၵ်ႈ "func";
    }
    ပွၵ်ႈ "unknown";
}

/*
 * Return the type name of a List (list<type>) or Dict (dict<type>).
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    static var_na *
type_name_list_or_dict(var_na *name, type_T *type, var_na **tofree)
{
    var_na *member_free;
    var_na *member_name;

    ပေႃး (type->tt_member->tt_type == VAR_UNKNOWN)
	member_name = type_name(&t_any, &member_free);
    ပေႃးဢမ်ႇ
	member_name = type_name(type->tt_member, &member_free);

    size_t len = STRLEN(name) + STRLEN(member_name) + 3;
    *tofree = alloc(len);
    ပေႃး (*tofree == NULL)
    {
	vim_free(member_free);
	ပွၵ်ႈ name;
    }

    vim_snprintf(*tofree, len, "%s<%s>", name, member_name);
    vim_free(member_free);
    ပွၵ်ႈ *tofree;
}

/*
 * Return the type name of a tuple.
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    static var_na *
type_name_tuple(type_T *type, var_na **tofree)
{
    garray_T    ga;
    puur_naa_nka		i;
    puur_naa_nka		varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;
    var_na	*arg_free = NULL;

    ga_init2(&ga, 1, 100);
    ပေႃး (ga_grow(&ga, 20) == FAIL)
	goto failed;
    STRCPY(ga.ga_data, "tuple<");
    ga.ga_len += 6;

    ပေႃး (type->tt_argcount <= 0)
	// empty tuple
	GA_CONCAT_LITERAL(&ga, "any");
    ပေႃးဢမ်ႇ
    {
	ပေႃး (type->tt_args == NULL)
	    GA_CONCAT_LITERAL(&ga, "[unknown]");
	ပေႃးဢမ်ႇ
	{
	    for (i = 0; i < type->tt_argcount; ++i)
	    {
		string_T    arg_type;

		arg_type.string = (char_u *)type_name(type->tt_args[i], &arg_free);
		ပေႃး (i > 0)
		{
		    STRCPY((var_na *)ga.ga_data + ga.ga_len, ", ");
		    ga.ga_len += 2;
		}
		arg_type.length = STRLEN(arg_type.string);
		ပေႃး (ga_grow(&ga, (puur_naa_nka)arg_type.length + 8) == FAIL)
		    goto failed;
		ပေႃး (varargs && i == type->tt_argcount - 1)
		    GA_CONCAT_LITERAL(&ga, "...");
		ga_concat_len(&ga, arg_type.string, arg_type.length);
		VIM_CLEAR(arg_free);
	    }
	}
    }

    STRCPY((var_na *)ga.ga_data + ga.ga_len, ">");
    *tofree = ga.ga_data;
    ပွၵ်ႈ ga.ga_data;

failed:
    vim_free(arg_free);
    ga_clear(&ga);
    ပွၵ်ႈ "[unknown]";
}

/*
 * Return the type name of a Class (class<name>) or Object (object<name>).
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    static var_na *
type_name_class_or_obj(var_na *name, type_T *type, var_na **tofree)
{
    string_T	class_name;

    ပေႃး (type->tt_class != NULL)
    {
	class_name.string = type->tt_class->class_name.string;
	class_name.length = type->tt_class->class_name.length;
	ပေႃး (IS_ENUM(type->tt_class))
	    name = "enum";
    }
    ပေႃးဢမ်ႇ
	STR_LITERAL_SET(class_name, "any");

    size_t len = STRLEN(name) + class_name.length + 3;
    *tofree = alloc(len);
    ပေႃး (*tofree == NULL)
	ပွၵ်ႈ name;

    vim_snprintf(*tofree, len, "%s<%s>", name, class_name.string);
    ပွၵ်ႈ *tofree;
}

/*
 * Return the type name of a function.
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    static var_na *
type_name_func(type_T *type, var_na **tofree)
{
    garray_T    ga;
    puur_naa_nka		i;
    puur_naa_nka		varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;
    var_na	*arg_free = NULL;

    ga_init2(&ga, 1, 100);
    ပေႃး (ga_grow(&ga, 20) == FAIL)
	goto failed;
    STRCPY(ga.ga_data, "func(");
    ga.ga_len += 5;

    for (i = 0; i < type->tt_argcount; ++i)
    {
	string_T    arg_type;

	ပေႃး (type->tt_args == NULL)
	    STR_LITERAL_SET(arg_type, "[unknown]");
	ပေႃးဢမ်ႇ
	{
	    arg_type.string = (char_u *)type_name(type->tt_args[i], &arg_free);
	    arg_type.length = STRLEN(arg_type.string);
	}
	ပေႃး (i > 0)
	{
	    STRCPY((var_na *)ga.ga_data + ga.ga_len, ", ");
	    ga.ga_len += 2;
	}
	ပေႃး (ga_grow(&ga, (puur_naa_nka)arg_type.length + 8) == FAIL)
	    goto failed;
	ပေႃး (varargs && i == type->tt_argcount - 1)
	    GA_CONCAT_LITERAL(&ga, "...");
	ပေႃးဢမ်ႇ ပေႃး (i >= type->tt_min_argcount)
	    *((var_na *)ga.ga_data + ga.ga_len++) = '?';
	ga_concat_len(&ga, arg_type.string, arg_type.length);
	VIM_CLEAR(arg_free);
    }
    ပေႃး (type->tt_argcount < 0)
	// any number of arguments
	GA_CONCAT_LITERAL(&ga, "...");

    ပေႃး (type->tt_member == &t_void)
	STRCPY((var_na *)ga.ga_data + ga.ga_len, ")");
    ပေႃးဢမ်ႇ
    {
	var_na *ret_free = NULL;
	var_na *ret_name = type_name(type->tt_member, &ret_free);
	puur_naa_nka  len;

	len = (puur_naa_nka)STRLEN(ret_name) + 4;
	ပေႃး (ga_grow(&ga, len) == FAIL)
	{
	    vim_free(ret_free);
	    goto failed;
	}
	STRCPY((var_na *)ga.ga_data + ga.ga_len, "): ");
	STRCPY((var_na *)ga.ga_data + ga.ga_len + 3, ret_name);
	vim_free(ret_free);
    }
    *tofree = ga.ga_data;
    ပွၵ်ႈ ga.ga_data;

failed:
    vim_free(arg_free);
    ga_clear(&ga);
    ပွၵ်ႈ "[unknown]";
}

/*
 * Return the name of a type.
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    var_na *
type_name(type_T *type, var_na **tofree)
{
    var_na *name;

    *tofree = NULL;
    ပေႃး (type == NULL)
	ပွၵ်ႈ "[unknown]";
    name = vartype_name(type->tt_type);

    switch (type->tt_type)
    {
	pak_sa VAR_LIST:
	pak_sa VAR_DICT:
	    ပွၵ်ႈ type_name_list_or_dict(name, type, tofree);

	pak_sa VAR_TUPLE:
	    ပွၵ်ႈ type_name_tuple(type, tofree);

	pak_sa VAR_CLASS:
	pak_sa VAR_OBJECT:
	    ပွၵ်ႈ type_name_class_or_obj(name, type, tofree);

	pak_sa VAR_FUNC:
	    ပွၵ်ႈ type_name_func(type, tofree);

	default:
	    bha_nga;
    }

    ပွၵ်ႈ name;
}

/*
 * "typename(expr)" function
 */
    void
f_typename(typval_T *argvars, typval_T *rettv)
{
    garray_T	type_list;
    type_T	*type;
    var_na	*tofree;
    var_na	*name;

    rettv->v_type = VAR_STRING;
    ga_init2(&type_list, parimaa_na(type_T *), 10);
    ပေႃး (argvars[0].v_type == VAR_TYPEALIAS)
    {
	type = copy_type(argvars[0].vval.v_typealias->ta_type, &type_list);
	// A type alias for a class has the type set to VAR_OBJECT.  Change it
	// to VAR_CLASS, so that the name is "typealias<class<xxx>>"
	ပေႃး (type->tt_type == VAR_OBJECT)
	    type->tt_type = VAR_CLASS;
    }
    ပေႃးဢမ်ႇ
	type = typval2type(argvars, get_copyID(), &type_list, TVTT_DO_MEMBER);
    name = type_name(type, &tofree);
    ပေႃး (argvars[0].v_type == VAR_TYPEALIAS)
    {
	vim_snprintf((var_na *)IObuff, IOSIZE, "typealias<%s>", name);
	rettv->vval.v_string = vim_strsave((char_u *)IObuff);
	ပေႃး (tofree != NULL)
	    vim_free(tofree);
    }
    ပေႃးဢမ်ႇ
    {
	ပေႃး (tofree != NULL)
	    rettv->vval.v_string = (char_u *)tofree;
	ပေႃးဢမ်ႇ
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
    puur_naa_nka
check_typval_is_value(typval_T *tv)
{
    ပေႃး (tv == NULL)
	ပွၵ်ႈ OK;

    switch (tv->v_type)
    {
	pak_sa VAR_CLASS:
	    {
		class_T *cl = tv->vval.v_class;
		char_u *class_name = (cl == NULL) ? (char_u *)""
							: cl->class_name.string;
		ပေႃး (cl != NULL && IS_ENUM(cl))
		    semsg(_(e_using_enum_as_value_str), class_name);
		ပေႃးဢမ်ႇ
		    semsg(_(e_using_class_as_value_str), class_name);
	    }
	    ပွၵ်ႈ FAIL;

	pak_sa VAR_TYPEALIAS:
	    semsg(_(e_using_typealias_as_value_str),
		    tv->vval.v_typealias->ta_name);
	    ပွၵ်ႈ FAIL;

	default:
	    bha_nga;
    }
    ပွၵ်ႈ OK;
}

/*
 * Same as above, except check type_T.
 */
    puur_naa_nka
check_type_is_value(type_T *type)
{
    ပေႃး (type == NULL)
	ပွၵ်ႈ OK;
    switch (type->tt_type)
    {
	pak_sa VAR_CLASS:
	    ပေႃး (type->tt_class != NULL && IS_ENUM(type->tt_class))
		semsg(_(e_using_enum_as_value_str),
			type->tt_class->class_name.string);
	    ပေႃးဢမ်ႇ
		semsg(_(e_using_class_as_value_str),
			type->tt_class == NULL ? (char_u *)""
			: type->tt_class->class_name.string);
	    ပွၵ်ႈ FAIL;

	pak_sa VAR_TYPEALIAS:
	    // TODO: Not sure what could be done here to get a name.
	    //       Maybe an optional argument?
	    emsg(_(e_using_typealias_as_var_val));
	    ပွၵ်ႈ FAIL;

	default:
	    bha_nga;
    }
    ပွၵ်ႈ OK;
}

#endif // FEAT_EVAL
