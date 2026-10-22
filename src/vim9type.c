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

#ប្រសិនបើ defined(FEAT_EVAL)

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

    ប្រសិនបើ (ga_grow(type_gap, 1) == FAIL)
	ត្រឡប់ NULL;
    type = ALLOC_CLEAR_ONE(type_T);
    ប្រសិនបើ (type == NULL)
	ត្រឡប់ NULL;

    ((type_T **)type_gap->ga_data)[type_gap->ga_len] = type;
    ++type_gap->ga_len;
    ត្រឡប់ type;
}

/*
 * Make a shallow copy of "type".
 * When allocation fails returns "type".
 */
    type_T *
copy_type(type_T *type, garray_T *type_gap)
{
    type_T *copy = get_type_ptr(type_gap);

    ប្រសិនបើ (copy == NULL)
	ត្រឡប់ type;
    *copy = *type;
    copy->tt_flags &= ~TTFLAG_STATIC;

    ប្រសិនបើ (type->tt_args != NULL
	   && func_type_add_arg_types(copy, type->tt_argcount, type_gap) == OK)
	សម្រាប់ (ចំនួនគត់ i = 0; i < type->tt_argcount; ++i)
	    copy->tt_args[i] = type->tt_args[i];

    ត្រឡប់ copy;
}

/*
 * Inner part of copy_type_deep().
 * When allocation fails returns "type".
 */
    static type_T *
copy_type_deep_rec(type_T *type, garray_T *type_gap, garray_T *seen_types)
{
    សម្រាប់ (ចំនួនគត់ i = 0; i < seen_types->ga_len; ++i)
	ប្រសិនបើ (((type_T **)seen_types->ga_data)[i * 2] == type)
	    // seen this type before, return the copy we made
	    ត្រឡប់ ((type_T **)seen_types->ga_data)[i * 2 + 1];

    type_T *copy = copy_type(type, type_gap);
    ប្រសិនបើ (ga_grow(seen_types, 1) == FAIL)
	ត្រឡប់ copy;
    ((type_T **)seen_types->ga_data)[seen_types->ga_len * 2] = type;
    ((type_T **)seen_types->ga_data)[seen_types->ga_len * 2 + 1] = copy;
    ++seen_types->ga_len;

    ប្រសិនបើ (copy->tt_member != NULL)
	copy->tt_member = copy_type_deep_rec(copy->tt_member,
							 type_gap, seen_types);
    ប្រសិនបើ (type->tt_args != NULL)
	សម្រាប់ (ចំនួនគត់ i = 0; i < type->tt_argcount; ++i)
	    copy->tt_args[i] = copy_type_deep_rec(copy->tt_args[i],
							 type_gap, seen_types);

    ត្រឡប់ copy;
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
    ត្រឡប់ res;
}

    void
clear_type_list(garray_T *gap)
{
    ខណៈពេល (gap->ga_len > 0)
	vim_free(((type_T **)gap->ga_data)[--gap->ga_len]);
    ga_clear(gap);
}

    void
clear_func_type_list(garray_T *gap, type_T **func_type)
{
    ខណៈពេល (gap->ga_len > 0)
    {
	// func_type pointing to the uf_type_list, so reset pointer
	ប្រសិនបើ (*func_type == ((type_T **)gap->ga_data)[--gap->ga_len])
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

    ប្រសិនបើ (type == NULL)
	ត្រឡប់ NULL;

    // A fixed type never contains allocated types, return as-is.
    ប្រសិនបើ (type->tt_flags & TTFLAG_STATIC)
	ត្រឡប់ type;

    ret = ALLOC_ONE(type_T);
    ប្រសិនបើ (ret == NULL)
	ត្រឡប់ NULL;
    *ret = *type;

    ប្រសិនបើ (ret->tt_member != NULL)
	ret->tt_member = alloc_type(ret->tt_member);

    ប្រសិនបើ (type->tt_argcount > 0 && type->tt_args != NULL)
    {
	ចំនួនគត់ i;

	ret->tt_args = ALLOC_MULT(type_T *, type->tt_argcount);
	ប្រសិនបើ (ret->tt_args != NULL)
	    សម្រាប់ (i = 0; i < type->tt_argcount; ++i)
		ret->tt_args[i] = alloc_type(type->tt_args[i]);
    }
    បើមិនដូច្នេះ
	ret->tt_args = NULL;

    ត្រឡប់ ret;
}

/*
 * Free a type that was created with alloc_type().
 */
    void
free_type(type_T *type)
{
    ចំនួនគត់ i;

    ប្រសិនបើ (type == NULL || (type->tt_flags & TTFLAG_STATIC))
	ត្រឡប់;
    ប្រសិនបើ (type->tt_args != NULL)
    {
	សម្រាប់ (i = 0; i < type->tt_argcount; ++i)
	    free_type(type->tt_args[i]);
	vim_free(type->tt_args);
    }

    free_type(type->tt_member);

    vim_free(type);
}

/*
 * Return TRUE if "type" is to be recursed into for setting the type.
 */
    static ចំនួនគត់
set_tv_type_recurse(type_T *type)
{
    ត្រឡប់ type->tt_member != NULL
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
    ប្រសិនបើ (d->dv_type == type)
	ត្រឡប់;

    free_type(d->dv_type);
    d->dv_type = alloc_type(type);

    // Need to recursively set the type of dict items?
    ប្រសិនបើ (!set_tv_type_recurse(type))
	ត្រឡប់;

    ចំនួនគត់		todo = (ចំនួនគត់)d->dv_hashtab.ht_used;
    hashitem_T	*hi;
    dictitem_T	*di;

    FOR_ALL_HASHTAB_ITEMS(&d->dv_hashtab, hi, todo)
    {
	ប្រសិនបើ (!HASHITEM_EMPTY(hi))
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
    ប្រសិនបើ (l->lv_type == type)
	ត្រឡប់;

    free_type(l->lv_type);
    l->lv_type = alloc_type(type);

    // Need to recursively set the type of list items?
    ប្រសិនបើ (l->lv_first == &range_list_item || !set_tv_type_recurse(type))
	ត្រឡប់;

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
    ប្រសិនបើ (tuple->tv_type == type)
	ត្រឡប់;

    free_type(tuple->tv_type);
    tuple->tv_type = alloc_type(type);

    ប្រសិនបើ (type->tt_argcount <= 0)
	ត្រឡប់;

    // recursively set the type of list items
    type_T	*item_type;
    សម្រាប់ (ចំនួនគត់ i = 0; i < tuple_len(tuple); i++)
    {
	ប្រសិនបើ ((type->tt_flags & TTFLAG_VARARGS) && (i >= type->tt_argcount - 1))
	    // For a variadic tuple, the last type is a List.  So use the
	    // List member type.
	    item_type = type->tt_args[type->tt_argcount - 1]->tt_member;
	បើមិនដូច្នេះ
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
    ប្រសិនបើ (type->tt_type == VAR_ANY)
	// If the variable type is "any", then keep the value type.
	// e.g.  var x: any = [1, 2] or var y: any = {v: 1}
	ត្រឡប់;

    ប្រសិនបើ (tv->v_type == VAR_DICT && tv->vval.v_dict != NULL)
	set_tv_type_dict(tv->vval.v_dict, type);
    បើមិនដូច្នេះ ប្រសិនបើ (tv->v_type == VAR_LIST && tv->vval.v_list != NULL)
	set_tv_type_list(tv->vval.v_list, type);
    បើមិនដូច្នេះ ប្រសិនបើ (tv->v_type == VAR_TUPLE && tv->vval.v_tuple != NULL)
	set_tv_type_tuple(tv->vval.v_tuple, type);
}

/*
 * For a tuple type, reserve space for "typecount" types (including the
 * repeated type).
 */
    static ចំនួនគត់
tuple_type_add_types(
    type_T	*tupletype,
    ចំនួនគត់		typecount,
    garray_T	*type_gap)
{
    // To make it easy to free the space needed for the types, add the
    // pointer to type_gap.
    ប្រសិនបើ (ga_grow(type_gap, 1) == FAIL)
	ត្រឡប់ FAIL;
    tupletype->tt_args = alloc_clear_id(parimaa_na(type_T *) * typecount,
							  aid_tuple_type_args);
    ប្រសិនបើ (tupletype->tt_args == NULL)
	ត្រឡប់ FAIL;
    ((type_T **)type_gap->ga_data)[type_gap->ga_len] =
						(void *)tupletype->tt_args;
    ++type_gap->ga_len;
    ត្រឡប់ OK;
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
    ប្រសិនបើ (member_type == NULL || (member_type->tt_type == VAR_ANY
					&& !IS_GENERIC_TYPE(member_type)))
	ត្រឡប់ &t_list_any;
    ប្រសិនបើ (member_type->tt_type == VAR_VOID
	    || member_type->tt_type == VAR_UNKNOWN)
	ត្រឡប់ &t_list_empty;
    ប្រសិនបើ (member_type->tt_type == VAR_BOOL)
	ត្រឡប់ &t_list_bool;
    ប្រសិនបើ (member_type->tt_type == VAR_NUMBER)
	ត្រឡប់ &t_list_number;
    ប្រសិនបើ (member_type->tt_type == VAR_STRING)
	ត្រឡប់ &t_list_string;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    ប្រសិនបើ (type == NULL)
	ត្រឡប់ &t_any;
    type->tt_type = VAR_LIST;
    type->tt_member = member_type;
    type->tt_argcount = 0;
    type->tt_args = NULL;
    ត្រឡប់ type;
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
    ចំនួនគត់		typecount = tuple_types_gap->ga_len;

    // recognize commonly used types
    ប្រសិនបើ (typecount == 0)
	ត្រឡប់ &t_tuple_any;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    ប្រសិនបើ (type == NULL)
	ត្រឡប់ &t_any;
    type->tt_type = VAR_TUPLE;
    type->tt_member = NULL;
    ប្រសិនបើ (typecount > 0)
    {
	ប្រសិនបើ (tuple_type_add_types(type, typecount, type_gap) == FAIL)
	    ត្រឡប់ &t_any;
	mch_memmove(type->tt_args, tuple_types, parimaa_na(type_T *) * typecount);
    }
    type->tt_argcount = typecount;
    type->tt_flags = 0;

    ត្រឡប់ type;
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
    ប្រសិនបើ (member_type == NULL || (member_type->tt_type == VAR_ANY
					&& !IS_GENERIC_TYPE(member_type)))
	ត្រឡប់ &t_dict_any;
    ប្រសិនបើ (member_type->tt_type == VAR_VOID
	    || member_type->tt_type == VAR_UNKNOWN)
	ត្រឡប់ &t_dict_empty;
    ប្រសិនបើ (member_type->tt_type == VAR_BOOL)
	ត្រឡប់ &t_dict_bool;
    ប្រសិនបើ (member_type->tt_type == VAR_NUMBER)
	ត្រឡប់ &t_dict_number;
    ប្រសិនបើ (member_type->tt_type == VAR_STRING)
	ត្រឡប់ &t_dict_string;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    ប្រសិនបើ (type == NULL)
	ត្រឡប់ &t_any;
    type->tt_type = VAR_DICT;
    type->tt_member = member_type;
    type->tt_argcount = 0;
    type->tt_args = NULL;
    ត្រឡប់ type;
}

/*
 * Allocate a new type for a function.
 */
    type_T *
alloc_func_type(type_T *ret_type, ចំនួនគត់ argcount, garray_T *type_gap)
{
    type_T *type = get_type_ptr(type_gap);

    ប្រសិនបើ (type == NULL)
	ត្រឡប់ &t_any;
    type->tt_type = VAR_FUNC;
    type->tt_member = ret_type == NULL ? &t_unknown : ret_type;
    type->tt_argcount = argcount;
    type->tt_args = NULL;
    ត្រឡប់ type;
}

/*
 * Allocate a new type for a tuple.
 */
    static type_T *
alloc_tuple_type(ចំនួនគត់ typecount, garray_T *type_gap)
{
    type_T *type = get_type_ptr(type_gap);

    ប្រសិនបើ (type == NULL)
	ត្រឡប់ &t_any;
    type->tt_type = VAR_TUPLE;
    type->tt_member = NULL;
    type->tt_argcount = typecount;
    type->tt_args = NULL;
    ត្រឡប់ type;
}

/*
 * Get a function type, based on the return type "ret_type".
 * "argcount" must be -1 or 0, a predefined type can be used.
 */
    type_T *
get_func_type(type_T *ret_type, ចំនួនគត់ argcount, garray_T *type_gap)
{
    // recognize commonly used types
    ប្រសិនបើ (ret_type == &t_unknown || ret_type == NULL)
    {
	// (argcount == 0) is not possible
	ត្រឡប់ &t_func_unknown;
    }
    ប្រសិនបើ (ret_type == &t_void)
    {
	ប្រសិនបើ (argcount == 0)
	    ត្រឡប់ &t_func_0_void;
	បើមិនដូច្នេះ
	    ត្រឡប់ &t_func_void;
    }
    ប្រសិនបើ (ret_type == &t_any)
    {
	ប្រសិនបើ (argcount == 0)
	    ត្រឡប់ &t_func_0_any;
	បើមិនដូច្នេះ
	    ត្រឡប់ &t_func_any;
    }
    ប្រសិនបើ (ret_type == &t_number)
    {
	ប្រសិនបើ (argcount == 0)
	    ត្រឡប់ &t_func_0_number;
	បើមិនដូច្នេះ
	    ត្រឡប់ &t_func_number;
    }
    ប្រសិនបើ (ret_type == &t_string)
    {
	ប្រសិនបើ (argcount == 0)
	    ត្រឡប់ &t_func_0_string;
	បើមិនដូច្នេះ
	    ត្រឡប់ &t_func_string;
    }

    ត្រឡប់ alloc_func_type(ret_type, argcount, type_gap);
}

/*
 * For a function type, reserve space for "argcount" argument types (including
 * vararg).
 */
    ចំនួនគត់
func_type_add_arg_types(
	type_T	    *functype,
	ចំនួនគត់	    argcount,
	garray_T    *type_gap)
{
    // To make it easy to free the space needed for the argument types, add the
    // pointer to type_gap.
    ប្រសិនបើ (ga_grow(type_gap, 1) == FAIL)
	ត្រឡប់ FAIL;
    functype->tt_args = ALLOC_CLEAR_MULT(type_T *, argcount);
    ប្រសិនបើ (functype->tt_args == NULL)
	ត្រឡប់ FAIL;
    ((type_T **)type_gap->ga_data)[type_gap->ga_len] =
						     (void *)functype->tt_args;
    ++type_gap->ga_len;
    ត្រឡប់ OK;
}

/*
 * Return TRUE if "type" is NULL, any or unknown.
 * This also works for const (comparing with &t_any and &t_unknown doesn't).
 */
    ចំនួនគត់
type_any_or_unknown(type_T *type)
{
    ត្រឡប់ type == NULL || type->tt_type == VAR_ANY
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
	    ត្រឡប់ &t_null;

	pak_sa VVAL_NONE:
	    ត្រឡប់ &t_none;

	pak_sa VVAL_TRUE:
	pak_sa VVAL_FALSE:
	    ត្រឡប់ &t_bool;

	default:
	    ត្រឡប់ &t_unknown;
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
list_typval2type(typval_T *tv, ចំនួនគត់ copyID, garray_T *type_gap, ចំនួនគត់ flags)
{
    list_T	*l = tv->vval.v_list;
    listitem_T	*li;
    type_T	*member_type = NULL;

    // An empty list has type list<unknown>, unless the type was specified
    // and is not list<any>.  This matters when assigning to a variable
    // with a specific list type.
    ប្រសិនបើ (l == NULL || (l->lv_first == NULL
		&& (l->lv_type == NULL || l->lv_type->tt_member == &t_any)))
	ត្រឡប់ &t_list_empty;

    ប្រសិនបើ ((flags & TVTT_DO_MEMBER) == 0)
	ត្រឡប់ &t_list_any;

    // If the type is list<any> go through the members, it may end up a
    // more specific type.
    ប្រសិនបើ (l->lv_type != NULL && (l->lv_first == NULL
		|| (flags & TVTT_MORE_SPECIFIC) == 0
		|| l->lv_type->tt_member != &t_any))
	// make a copy, lv_type may be freed if the list is freed
	ត្រឡប់ copy_type_deep(l->lv_type, type_gap);

    ប្រសិនបើ (l->lv_first == &range_list_item)
	ត្រឡប់ &t_list_number;

    ប្រសិនបើ (l->lv_copyID == copyID)
	// avoid recursion
	ត្រឡប់ &t_list_any;

    l->lv_copyID = copyID;

    // Use the common type of all members.
    member_type = typval2type(&l->lv_first->li_tv, copyID, type_gap,
							TVTT_DO_MEMBER);
    សម្រាប់ (li = l->lv_first->li_next; li != NULL; li = li->li_next)
	common_type(typval2type(&li->li_tv, copyID, type_gap, TVTT_DO_MEMBER),
					member_type, &member_type, type_gap);

    // Reset copyID so that a shared reference to this list (not a circular
    // reference) can be processed again to get the correct type.
    l->lv_copyID = 0;

    ត្រឡប់ get_list_type(member_type, type_gap);
}

/*
 * Get a type_T for a Tuple typval in "tv".
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    static type_T *
tuple_typval2type(typval_T *tv, ចំនួនគត់ copyID, garray_T *type_gap, ចំនួនគត់ flags)
{
    tuple_T	*tuple = tv->vval.v_tuple;
    ចំនួនគត់		len = tuple_len(tuple);
    type_T	*type = NULL;

    // An empty tuple has type tuple<unknown>, unless the type was specified
    // and is not tuple<any>.  This matters when assigning to a variable
    // with a specific tuple type.
    ប្រសិនបើ (tuple == NULL || (len == 0 && (tuple->tv_type == NULL
				|| tuple->tv_type->tt_argcount == 0)))
	ត្រឡប់ &t_tuple_empty;

    ប្រសិនបើ ((flags & TVTT_DO_MEMBER) == 0)
	ត្រឡប់ &t_tuple_any;

    // If the type is tuple<any> go through the members, it may end up a
    // more specific type.
    ប្រសិនបើ (tuple->tv_type != NULL && (len == 0
					|| (flags & TVTT_MORE_SPECIFIC) == 0))
	// make a copy, tv_type may be freed if the tuple is freed
	ត្រឡប់ copy_type_deep(tuple->tv_type, type_gap);

    ប្រសិនបើ (tuple->tv_copyID == copyID)
	// avoid recursion
	ត្រឡប់ &t_tuple_any;

    tuple->tv_copyID = copyID;

    garray_T	tuple_types_ga;
    ga_init2(&tuple_types_ga, parimaa_na(type_T *), 10);
    សម្រាប់ (ចំនួនគត់ i = 0; i < len; i++)
    {
	type = typval2type(TUPLE_ITEM(tuple, i), copyID, type_gap,
							TVTT_DO_MEMBER);
	ប្រសិនបើ (ga_grow(&tuple_types_ga, 1) == FAIL)
	{
	    ga_clear(&tuple_types_ga);
	    // Reset copyID so that a shared reference to this tuple can be
	    // processed again.
	    tuple->tv_copyID = 0;
	    ត្រឡប់ NULL;
	}
	((type_T **)tuple_types_ga.ga_data)[tuple_types_ga.ga_len] = type;
	tuple_types_ga.ga_len++;
    }

    type_T *tuple_type = get_tuple_type(&tuple_types_ga, type_gap);
    ga_clear(&tuple_types_ga);

    // Reset copyID so that a shared reference to this tuple (not a circular
    // reference) can be processed again to get the correct type.
    tuple->tv_copyID = 0;

    ត្រឡប់ tuple_type;
}

/*
 * Get a type_T for a Dict typval in "tv".
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    static type_T *
dict_typval2type(typval_T *tv, ចំនួនគត់ copyID, garray_T *type_gap, ចំនួនគត់ flags)
{
    dict_iterator_T	iter;
    typval_T		*value;
    dict_T		*d = tv->vval.v_dict;
    type_T		*member_type = NULL;

    ប្រសិនបើ (d == NULL || (d->dv_hashtab.ht_used == 0 && d->dv_type == NULL))
	ត្រឡប់ &t_dict_empty;

    ប្រសិនបើ ((flags & TVTT_DO_MEMBER) == 0)
	ត្រឡប់ &t_dict_any;

    // If the type is dict<any> go through the members, it may end up a
    // more specific type.
    ប្រសិនបើ (d->dv_type != NULL && (d->dv_hashtab.ht_used == 0
		|| (flags & TVTT_MORE_SPECIFIC) == 0
		|| d->dv_type->tt_member != &t_any))
	ត្រឡប់ d->dv_type;

    ប្រសិនបើ (d->dv_copyID == copyID)
	// avoid recursion
	ត្រឡប់ &t_dict_any;

    d->dv_copyID = copyID;

    // Use the common type of all values.
    dict_iterate_start(tv, &iter);
    dict_iterate_next(&iter, &value);
    member_type = typval2type(value, copyID, type_gap, TVTT_DO_MEMBER);

    ខណៈពេល (dict_iterate_next(&iter, &value) != NULL)
	common_type(typval2type(value, copyID, type_gap, TVTT_DO_MEMBER),
					member_type, &member_type, type_gap);

    // Reset copyID so that a shared reference to this dict (not a circular
    // reference) can be processed again to get the correct type.
    d->dv_copyID = 0;

    ត្រឡប់ get_dict_type(member_type, type_gap);
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
    ប្រសិនបើ (type == NULL)
	ត្រឡប់ NULL;

    *type = *ufunc->uf_func_type;
    ប្រសិនបើ (type->tt_argcount >= 0 && pt->pt_argc > 0)
    {
	type->tt_argcount -= pt->pt_argc;
	type->tt_min_argcount -= pt->pt_argc;
	ប្រសិនបើ (type->tt_argcount > 0 && func_type_add_arg_types(type,
		    type->tt_argcount, type_gap) == OK)
	    សម្រាប់ (ចំនួនគត់ i = 0; i < type->tt_argcount; ++i)
		type->tt_args[i] =
		    ufunc->uf_func_type->tt_args[i + pt->pt_argc];
    }
    ប្រសិនបើ (pt->pt_func != NULL)
	type->tt_member = pt->pt_func->uf_ret_type;

    ត្រឡប់ type;
}

/*
 * Get a type_T for a "class" or an "object" typval in "tv".
 */
    static type_T *
oc_typval2type(typval_T *tv)
{
    ប្រសិនបើ (tv->v_type == VAR_CLASS)
    {
	ប្រសិនបើ (tv->vval.v_class == NULL)
	    ត្រឡប់ &t_class;

	ត្រឡប់ &tv->vval.v_class->class_type;
    }

    ប្រសិនបើ (tv->vval.v_object != NULL)
	ត្រឡប់ &tv->vval.v_object->obj_class->class_object_type;

    ត្រឡប់ &t_object_any;
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
    ចំនួនគត់		argcount = 0;
    ចំនួនគត់		min_argcount = 0;

    ប្រសិនបើ (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != NULL)
    {
	ប្រសិនបើ (tv->vval.v_partial->pt_func != NULL)
	    ufunc = tv->vval.v_partial->pt_func;
	បើមិនដូច្នេះ
	    name = tv->vval.v_partial->pt_name;
    }
    បើមិនដូច្នេះ
	name = tv->vval.v_string;

    ប្រសិនបើ (name == NULL && ufunc == NULL)
	ត្រឡប់ &t_func_unknown;

    ប្រសិនបើ (name != NULL)
    {
	ចំនួនគត់ idx = find_internal_func(name);

	ប្រសិនបើ (idx >= 0)
	{
	    type_T *decl_type;  // unused

	    internal_func_get_argcount(idx, &argcount, &min_argcount);
	    member_type = internal_func_ret_type(idx, 0, NULL, &decl_type,
		    type_gap);
	}
	បើមិនដូច្នេះ
	{
	    // Check if name contains "<".  If it does, then replace "<" with
	    // NUL and lookup the function and then look up the specific
	    // generic function using the supplied types.
	    char_u *p = generic_func_find_open_bracket(name);
	    ប្រសិនបើ (p == NULL)
		ufunc = find_func(name, FALSE);
	    បើមិនដូច្នេះ
	    {
		// generic function
		char_u	cc = *p;

		*p = NUL;
		ufunc = find_func(name, FALSE);
		*p = cc;
		ប្រសិនបើ (ufunc != NULL && IS_GENERIC_FUNC(ufunc))
		{
		    ufunc = find_generic_func(ufunc, name, &p);
		    ប្រសិនបើ (ufunc == NULL)
			ត្រឡប់ NULL;
		}
	    }
	}
    }
    ប្រសិនបើ (ufunc != NULL)
    {
	// May need to get the argument types from default values by
	// compiling the function.
	ប្រសិនបើ (ufunc->uf_def_status == UF_TO_BE_COMPILED
		&& compile_def_function(ufunc, TRUE, CT_NONE, NULL)
		== FAIL)
	    ត្រឡប់ NULL;
	ប្រសិនបើ (ufunc->uf_func_type == NULL)
	    set_function_type(ufunc);
	ប្រសិនបើ (ufunc->uf_func_type != NULL)
	{
	    ប្រសិនបើ (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != NULL)
		ត្រឡប់ partial_typval2type(tv, ufunc, type_gap);
	    ត្រឡប់ ufunc->uf_func_type;
	}
    }

    type = get_type_ptr(type_gap);
    ប្រសិនបើ (type == NULL)
	ត្រឡប់ NULL;
    type->tt_type = tv->v_type;
    type->tt_argcount = argcount;
    type->tt_min_argcount = min_argcount;
    ប្រសិនបើ (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != NULL
					&& tv->vval.v_partial->pt_argc > 0)
    {
	type->tt_argcount -= tv->vval.v_partial->pt_argc;
	type->tt_min_argcount -= tv->vval.v_partial->pt_argc;
    }
    type->tt_member = member_type;

    ត្រឡប់ type;
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
typval2type_int(typval_T *tv, ចំនួនគត់ copyID, garray_T *type_gap, ចំនួនគត់ flags)
{
    switch (tv->v_type)
    {
	pak_sa VAR_UNKNOWN:
	    ត្រឡប់ &t_unknown;

	pak_sa VAR_ANY:
	    ត្រឡប់ &t_any;

	pak_sa VAR_VOID:
	    ត្រឡប់ &t_void;

	pak_sa VAR_BOOL:
	    ត្រឡប់ &t_bool;

	pak_sa VAR_SPECIAL:
	    ត្រឡប់ special_typval2type(tv);

	pak_sa VAR_NUMBER:
	    ត្រឡប់ &t_number;

	pak_sa VAR_FLOAT:
	    ត្រឡប់ &t_float;

	pak_sa VAR_STRING:
	    ត្រឡប់ &t_string;

	pak_sa VAR_BLOB:
	    ប្រសិនបើ (tv->vval.v_blob == NULL)
		ត្រឡប់ &t_blob_null;
	    ត្រឡប់ &t_blob;

	pak_sa VAR_LIST:
	    ត្រឡប់ list_typval2type(tv, copyID, type_gap, flags);

	pak_sa VAR_TUPLE:
	    ត្រឡប់ tuple_typval2type(tv, copyID, type_gap, flags);

	pak_sa VAR_DICT:
	    ត្រឡប់ dict_typval2type(tv, copyID, type_gap, flags);

	pak_sa VAR_JOB:
	    ត្រឡប់ &t_job;

	pak_sa VAR_CHANNEL:
	    ត្រឡប់ &t_channel;

	pak_sa VAR_CLASS:
	pak_sa VAR_OBJECT:
	    ត្រឡប់ oc_typval2type(tv);

	pak_sa VAR_TYPEALIAS:
	    ត្រឡប់ &t_typealias;

	pak_sa VAR_FUNC:
	pak_sa VAR_PARTIAL:
	    ត្រឡប់ fp_typval2type(tv, type_gap);

	pak_sa VAR_INSTR:
	default:
	    bha_nga;
    }

    ត្រឡប់ NULL;
}

/*
 * Return TRUE if "tv" is not a bool but should be converted to bool.
 */
    ចំនួនគត់
need_convert_to_bool(type_T *type, typval_T *tv)
{
    ត្រឡប់ type != NULL && type == &t_bool && tv->v_type != VAR_BOOL
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
typval2type(typval_T *tv, ចំនួនគត់ copyID, garray_T *type_gap, ចំនួនគត់ flags)
{
    type_T *type = typval2type_int(tv, copyID, type_gap, flags);

    ប្រសិនបើ (type == NULL)
	ត្រឡប់ NULL;

    ប្រសិនបើ (type != &t_bool && (tv->v_type == VAR_NUMBER
		&& (tv->vval.v_number == 0 || tv->vval.v_number == 1)))
	// Number 0 and 1 and expression with "&&" or "||" can also be used
	// for bool.
	type = &t_number_bool;
    បើមិនដូច្នេះ ប្រសិនបើ (type != &t_float && tv->v_type == VAR_NUMBER)
	// A number can also be used for float.
	type = &t_number_float;
    ត្រឡប់ type;
}

/*
 * Return TRUE if "type" can be used for a variable declaration.
 * Give an error and return FALSE if not.
 */
    ចំនួនគត់
valid_declaration_type(type_T *type)
{
    ប្រសិនបើ (type->tt_type == VAR_SPECIAL  // null, none
	    || type->tt_type == VAR_VOID)
    {
	var_na *tofree = NULL;
	var_na *name = type_name(type, &tofree);
	semsg(_(e_invalid_type_in_variable_declaration_str), name);
	vim_free(tofree);
	ត្រឡប់ FALSE;
    }
    ត្រឡប់ TRUE;
}

/*
 * Get a type_T for a typval_T, used for v: variables.
 * "type_list" is used to temporarily create types in.
 */
    type_T *
typval2type_vimvar(typval_T *tv, garray_T *type_gap)
{
    ប្រសិនបើ (tv->v_type == VAR_LIST)  // e.g. for v:oldfiles
	ត្រឡប់ &t_list_string;
    ប្រសិនបើ (tv->v_type == VAR_DICT)  // e.g. for v:event
	ត្រឡប់ &t_dict_any;
    ត្រឡប់ typval2type(tv, get_copyID(), type_gap, TVTT_DO_MEMBER);
}

    ចំនួនគត់
check_typval_arg_type(
	type_T	    *expected,
	typval_T    *actual_tv,
	var_na	    *func_name,
	ចំនួនគត់	    arg_idx)
{
    where_T	where = WHERE_INIT;

    ប្រសិនបើ (arg_idx > 0)
    {
	where.wt_index = arg_idx;
	where.wt_kind = WT_ARGUMENT;
    }
    where.wt_func_name = func_name;
    ត្រឡប់ check_typval_type(expected, actual_tv, where);
}

/*
 * Return true if "expected" is list<any> or dict<any> and "actual_tv" is a
 * list or dict of that kind, which matches whatever its values are.
 */
    static bool
matches_listdict_any(type_T *expected, typval_T *actual_tv)
{
    ត្រឡប់ (expected->tt_type == VAR_LIST || expected->tt_type == VAR_DICT)
	    && actual_tv->v_type == expected->tt_type
	    && expected->tt_member != NULL
	    && (expected->tt_member->tt_type == VAR_ANY
		|| expected->tt_member->tt_type == VAR_UNKNOWN);
}

/*
 * Return FAIL if "expected" and "actual" don't match.
 * When "argidx" > 0 it is included in the error message.
 */
    ចំនួនគត់
check_typval_type(type_T *expected, typval_T *actual_tv, where_T where)
{
    garray_T	type_list;
    type_T	*actual_type;
    ចំនួនគត់		res = FAIL;

    ប្រសិនបើ (expected == NULL)
	ត្រឡប់ OK;  // didn't expect anything.

    // No need to go through the values of a possibly big list or dict.
    ប្រសិនបើ (matches_listdict_any(expected, actual_tv))
	ត្រឡប់ OK;

    ga_init2(&type_list, parimaa_na(type_T *), 10);

    // A null_function and null_partial are special cases, they can be used to
    // clear a variable.
    ប្រសិនបើ ((actual_tv->v_type == VAR_FUNC && actual_tv->vval.v_string == NULL)
	    || (actual_tv->v_type == VAR_PARTIAL
					 && actual_tv->vval.v_partial == NULL))
	actual_type = &t_func_unknown;
    បើមិនដូច្នេះ
	// When the actual type is list<any> or dict<any> go through the values
	// to possibly get a more specific type.
	actual_type = typval2type(actual_tv, get_copyID(), &type_list,
					  TVTT_DO_MEMBER | TVTT_MORE_SPECIFIC);
    ប្រសិនបើ (actual_type != NULL)
    {
	res = check_type_maybe(expected, actual_type, TRUE, where);
	ប្រសិនបើ (res == MAYBE && !(actual_type->tt_type == VAR_FUNC
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
    ត្រឡប់ res;
}

    void
arg_type_mismatch(type_T *expected, type_T *actual, ចំនួនគត់ arg_idx)
{
    where_T	where = WHERE_INIT;

    ប្រសិនបើ (arg_idx > 0)
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
	    ប្រសិនបើ (where.wt_func_name == NULL)
		semsg(_(e_variable_nr_type_mismatch_expected_str_but_got_str),
			where.wt_index, typename1, typename2);
	    បើមិនដូច្នេះ
		semsg(_(e_variable_nr_type_mismatch_expected_str_but_got_str_in_str),
			where.wt_index, typename1, typename2, where.wt_func_name);
	    bha_nga;
	pak_sa WT_ARGUMENT:
	    ប្រសិនបើ (where.wt_func_name == NULL)
		semsg(_(e_argument_nr_type_mismatch_expected_str_but_got_str),
			where.wt_index, typename1, typename2);
	    បើមិនដូច្នេះ
		semsg(_(e_argument_nr_type_mismatch_expected_str_but_got_str_in_str),
			where.wt_index, typename1, typename2, where.wt_func_name);
	    bha_nga;
	pak_sa WT_CAST:
	pak_sa WT_UNKNOWN:
	    ប្រសិនបើ (where.wt_func_name == NULL)
		semsg(_(e_type_mismatch_expected_str_but_got_str),
			typename1, typename2);
	    បើមិនដូច្នេះ
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
    static ចំនួនគត់
check_tuple_type_maybe(
    type_T	*expected,
    type_T	*actual,
    where_T	where)
{
    ប្រសិនបើ (expected->tt_argcount == -1 || actual->tt_argcount == -1
		|| expected->tt_args == NULL || actual->tt_args == NULL)
	ត្រឡប់ OK;

    // For a non-variadic tuple, the number of items must match
    ប្រសិនបើ (!(expected->tt_flags & TTFLAG_VARARGS)
	    && expected->tt_argcount != actual->tt_argcount)
	ត្រឡប់ FAIL;

    // compare the type of each tuple item
    សម្រាប់ (ចំនួនគត់ i = 0; i < actual->tt_argcount; ++i)
    {
	type_T	*exp_type;
	type_T	*actual_type;

	ប្រសិនបើ (expected->tt_flags & TTFLAG_VARARGS)
	{
	    ប្រសិនបើ (i < expected->tt_argcount - 1)
		exp_type = expected->tt_args[i];
	    បើមិនដូច្នេះ
		// For a variadic tuple, the last type is a List.  So use the
		// List member type.
		exp_type = expected->tt_args[expected->tt_argcount - 1]->tt_member;
	}
	បើមិនដូច្នេះ
	    exp_type = expected->tt_args[i];

	ប្រសិនបើ (actual->tt_flags & TTFLAG_VARARGS)
	{
	    ប្រសិនបើ (i < actual->tt_argcount - 1)
		actual_type = actual->tt_args[i];
	    បើមិនដូច្នេះ
		// For a variadic tuple, the last type is a List.  So use the
		// List member type.
		actual_type = actual->tt_args[actual->tt_argcount - 1]->tt_member;
	}
	បើមិនដូច្នេះ
	    actual_type = actual->tt_args[i];

	// Allow for using "any" type for a tuple item
	ប្រសិនបើ (actual->tt_args[i] != &t_any && check_type(exp_type, actual_type,
						FALSE, where) == FAIL)
	    ត្រឡប់ FAIL;
    }

    ត្រឡប់ OK;
}

/*
 * Check if the expected and actual types match for a function
 * Returns OK if "expected" and "actual" are matching function types.
 * Returns FAIL if "expected" and "actual" are different types.
 * Returns MAYBE when a runtime type check is needed.
 */
    static ចំនួនគត់
check_func_type_maybe(
    type_T	*expected,
    type_T	*actual,
    where_T	where)
{
    ចំនួនគត់ ret = OK;

    // If the return type is unknown it can be anything, including
    // nothing, thus there is no point in checking.
    ប្រសិនបើ (expected->tt_member != &t_unknown)
    {
	ប្រសិនបើ (actual->tt_member != NULL
		&& actual->tt_member != &t_unknown)
	{
	    where_T  func_where = where;

	    func_where.wt_kind = WT_METHOD_RETURN;
	    ret = check_type_maybe(expected->tt_member,
		    actual->tt_member, FALSE,
		    func_where);
	}
	បើមិនដូច្នេះ
	    ret = MAYBE;
    }
    ប្រសិនបើ (ret != FAIL
	    && ((expected->tt_flags & TTFLAG_VARARGS)
		!= (actual->tt_flags & TTFLAG_VARARGS))
	    && expected->tt_argcount != -1)
	ret = FAIL;
    ប្រសិនបើ (ret != FAIL && expected->tt_argcount != -1
	    && actual->tt_min_argcount != -1
	    && (actual->tt_argcount == -1
		|| (actual->tt_argcount < expected->tt_min_argcount
		    || actual->tt_argcount > expected->tt_argcount)))
	ret = FAIL;
    ប្រសិនបើ (ret != FAIL && expected->tt_args != NULL
	    && actual->tt_args != NULL)
    {
	ចំនួនគត់ i;

	សម្រាប់ (i = 0; i < expected->tt_argcount
		&& i < actual->tt_argcount; ++i)
	{
	    where_T  func_where = where;
	    func_where.wt_kind = WT_METHOD_ARG;

	    // Allow for using "any" argument type, lambda's have them.
	    ប្រសិនបើ (actual->tt_args[i] != &t_any && check_type(
			expected->tt_args[i], actual->tt_args[i], FALSE,
			func_where) == FAIL)
	    {
		ret = FAIL;
		bha_nga;
	    }
	}
    }
    ប្រសិនបើ (ret == OK && expected->tt_argcount >= 0
	    && actual->tt_argcount == -1)
	// check the argument count at runtime
	ret = MAYBE;

    ត្រឡប់ ret;
}

/*
 * Check if the expected and actual types match for an object
 * Returns OK if "expected" and "actual" are matching object types.
 * Returns FAIL if "expected" and "actual" are different types.
 * Returns MAYBE when a runtime type check is needed.
 */
    static ចំនួនគត់
check_object_type_maybe(
    type_T	*expected,
    type_T	*actual,
    where_T	where)
{
    ចំនួនគត់ ret = OK;

    ប្រសិនបើ (actual->tt_type == VAR_ANY)
	ត្រឡប់ MAYBE;	// use runtime type check
    ប្រសិនបើ (actual->tt_type != VAR_OBJECT)
	ត្រឡប់ FAIL;	// don't use tt_class
    ប្រសិនបើ (actual->tt_class == NULL)    // null object
	ត្រឡប់ OK;
    // t_object_any matches any object except for an enum item
    ប្រសិនបើ (expected == &t_object_any && !IS_ENUM(actual->tt_class))
	ត្រឡប់ OK;

    // For object method arguments, do a invariant type check in
    // an extended class.  For all others, do a covariance type check.
    ប្រសិនបើ (where.wt_kind == WT_METHOD_ARG)
    {
	ប្រសិនបើ (actual->tt_class != expected->tt_class)
	    ret = FAIL;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (!class_instance_of(actual->tt_class, expected->tt_class))
    {
	// Check if this is an up-cast, if so we'll have to check the type at
	// runtime.
	ប្រសិនបើ (where.wt_kind == WT_CAST &&
		class_instance_of(expected->tt_class, actual->tt_class))
	    ret = MAYBE;
	បើមិនដូច្នេះ
	    ret = FAIL;
    }

    ត្រឡប់ ret;
}

/*
 * Check if the expected and actual types match.
 * Does not allow for assigning "any" to a specific type.
 * When "argidx" > 0 it is included in the error message.
 * Return OK if types match.
 * Return FAIL if types do not match.
 */
    ចំនួនគត់
check_type(
	type_T	*expected,
	type_T	*actual,
	ចំនួនគត់	give_msg,
	where_T where)
{
    ចំនួនគត់ ret = check_type_maybe(expected, actual, give_msg, where);

    ត្រឡប់ ret == MAYBE ? OK : ret;
}

/*
 * As check_type() but return MAYBE when a runtime type check should be used
 * when compiling.
 */
    ចំនួនគត់
check_type_maybe(
	type_T	*expected,
	type_T	*actual,
	ចំនួនគត់	give_msg,
	where_T where)
{
    ចំនួនគត់ ret = OK;

    // When expected is "unknown" we accept any actual type.
    // When expected is "any" we accept any actual type except "void".
    ប្រសិនបើ (expected->tt_type != VAR_UNKNOWN
	    && !(expected->tt_type == VAR_ANY && actual->tt_type != VAR_VOID))

    {
	// tt_type should match, except that a "partial" can be assigned to a
	// variable with type "func".
	// And "unknown" (using global variable) and "any" need a runtime type
	// check.
	ប្រសិនបើ (!(expected->tt_type == actual->tt_type
		    || actual->tt_type == VAR_UNKNOWN
		    || actual->tt_type == VAR_ANY
		    || (expected->tt_type == VAR_FUNC
					   && actual->tt_type == VAR_PARTIAL)))
	{
	    ប្រសិនបើ (expected->tt_type == VAR_BOOL
					&& (actual->tt_flags & TTFLAG_BOOL_OK))
		// Using number 0 or 1 for bool is OK.
		ត្រឡប់ OK;
	    ប្រសិនបើ (expected->tt_type == VAR_FLOAT
		    && actual->tt_type == VAR_NUMBER
		    && ((expected->tt_flags & TTFLAG_NUMBER_OK)
			     || (actual->tt_flags & TTFLAG_FLOAT_OK)))
		// Using a number where a float is expected is OK here.
		ត្រឡប់ OK;
	    ប្រសិនបើ (expected->tt_type == VAR_LIST
		    && actual->tt_type == VAR_TUPLE
		    && (expected->tt_flags & TTFLAG_TUPLE_OK))
		// Using a tuple where a list is expected is OK here.
		ត្រឡប់ OK;
	    ប្រសិនបើ (give_msg)
		type_mismatch_where(expected, actual, where);
	    ត្រឡប់ FAIL;
	}
	ប្រសិនបើ (expected->tt_type == VAR_DICT || expected->tt_type == VAR_LIST)
	{
	    // "unknown" is used for an empty list or dict
	    ប្រសិនបើ (actual->tt_member != NULL && actual->tt_member != &t_unknown)
		ret = check_type_maybe(expected->tt_member, actual->tt_member,
								 FALSE, where);
	}
	បើមិនដូច្នេះ ប្រសិនបើ (expected->tt_type == VAR_TUPLE && actual != &t_any)
	    ret =  check_tuple_type_maybe(expected, actual, where);
	បើមិនដូច្នេះ ប្រសិនបើ (expected->tt_type == VAR_FUNC && actual != &t_any)
	    ret = check_func_type_maybe(expected, actual, where);
	បើមិនដូច្នេះ ប្រសិនបើ (expected->tt_type == VAR_OBJECT)
	    ret = check_object_type_maybe(expected, actual, where);

	ប្រសិនបើ (ret == FAIL && give_msg)
	    type_mismatch_where(expected, actual, where);
    }

    ប្រសិនបើ (ret == OK && expected->tt_type != VAR_UNKNOWN
	    && expected->tt_type != VAR_ANY
	    && (actual->tt_type == VAR_UNKNOWN || actual->tt_type == VAR_ANY))
	// check the type at runtime
	ret = MAYBE;

    ត្រឡប់ ret;
}

/*
 * Check that the arguments of "type" match "argvars[argcount]".
 * "base_tv" is from "expr->Func()".
 * Return OK/FAIL.
 */
    ចំនួនគត់
check_argument_types(
	type_T	    *type,
	typval_T    *argvars,
	ចំនួនគត់	    argcount,
	typval_T    *base_tv,
	char_u	    *name)
{
    ចំនួនគត់	    varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;
    ចំនួនគត់	    i;
    ចំនួនគត់	    totcount = argcount + (base_tv == NULL ? 0 : 1);

    ប្រសិនបើ (type->tt_type != VAR_FUNC && type->tt_type != VAR_PARTIAL)
	ត្រឡប់ OK;  // just in case
    ប្រសិនបើ (totcount < type->tt_min_argcount - varargs)
    {
	emsg_funcname(e_not_enough_arguments_for_function_str, name);
	ត្រឡប់ FAIL;
    }
    ប្រសិនបើ (!varargs && type->tt_argcount >= 0 && totcount > type->tt_argcount)
    {
	emsg_funcname(e_too_many_arguments_for_function_str, name);
	ត្រឡប់ FAIL;
    }
    ប្រសិនបើ (type->tt_args == NULL)
	ត្រឡប់ OK;  // cannot check


    សម្រាប់ (i = 0; i < totcount; ++i)
    {
	type_T	    *expected;
	typval_T    *tv;

	ប្រសិនបើ (base_tv != NULL)
	{
	    ប្រសិនបើ (i == 0)
		tv = base_tv;
	    បើមិនដូច្នេះ
		tv = &argvars[i - 1];
	}
	បើមិនដូច្នេះ
	    tv = &argvars[i];
	ប្រសិនបើ (varargs && i >= type->tt_argcount - 1)
	{
	    expected = type->tt_args[type->tt_argcount - 1];
	    ប្រសិនបើ (expected != NULL && expected->tt_type == VAR_LIST)
		expected = expected->tt_member;
	    ប្រសិនបើ (expected == NULL)
		expected = &t_any;
	}
	បើមិនដូច្នេះ
	    expected = type->tt_args[i];

	// check the type, unless the value is v:none
	ប្រសិនបើ ((tv->v_type != VAR_SPECIAL || tv->vval.v_number != VVAL_NONE)
		   && check_typval_arg_type(expected, tv, NULL, i + 1) == FAIL)
	    ត្រឡប់ FAIL;
    }
    ត្រឡប់ OK;
}

// Maximum nesting of types, to avoid running out of stack space.  E.g.
// "list<list<number>>" is nested twice.
#define MAX_TYPE_DEPTH	1000

// Current number of levels of parse_type() and the deepest level reached while
// parsing the current type.  The nesting is one less than the number of
// levels.
static ចំនួនគត់	parse_depth = 0;
static ចំនួនគត់	parse_max_depth = 0;

/*
 * Skip over type in list<type>, dict<type> or tuple<type>.
 * Returns a pointer to the character after the type.  "syn_error" is set to
 * TRUE on syntax error.
 */
    static char_u *
skip_member_type(char_u *start, char_u *p, ចំនួនគត់ *syn_error)
{
    ប្រសិនបើ (STRNCMP("tuple", start, 5) == 0)
    {
	// handle tuple<{type1}, {type2}, ....<type>>
	p = skipwhite(p + 1);
	ខណៈពេល (*p != '>' && *p != NUL)
	{
	    char_u *sp = p;

	    ប្រសិនបើ (STRNCMP(p, "...", 3) == 0)
		p += 3;
	    p = skip_type(p, TRUE);
	    ប្រសិនបើ (p == sp)
	    {
		*syn_error = TRUE;
		ត្រឡប់ p;  // syntax error
	    }
	    ប្រសិនបើ (*p == ',')
		p = skipwhite(p + 1);
	}
	ប្រសិនបើ (*p == '>')
	    p++;
    }
    បើមិនដូច្នេះ
    {
	p = skipwhite(p);
	p = skip_type(skipwhite(p + 1), FALSE);
	p = skipwhite(p);
	ប្រសិនបើ (*p == '>')
	    ++p;
    }

    ត្រឡប់ p;
}

/*
 * Skip over a function type.  Returns a pointer to the character after the
 * type.  "syn_error" is set to TRUE on syntax error.
 */
    static char_u *
skip_func_type(char_u *p, ចំនួនគត់ *syn_error)
{
    ប្រសិនបើ (*p == '(')
    {
	// handle func(args): type
	++p;
	ខណៈពេល (*p != ')' && *p != NUL)
	{
	    char_u *sp = p;

	    ប្រសិនបើ (STRNCMP(p, "...", 3) == 0)
		p += 3;
	    p = skip_type(p, TRUE);
	    ប្រសិនបើ (p == sp)
	    {
		*syn_error = TRUE;
		ត្រឡប់ p;  // syntax error
	    }
	    ប្រសិនបើ (*p == ',')
		p = skipwhite(p + 1);
	}
	ប្រសិនបើ (*p == ')')
	{
	    ប្រសិនបើ (p[1] == ':')
		p = skip_type(skipwhite(p + 2), FALSE);
	    បើមិនដូច្នេះ
		++p;
	}
    }
    បើមិនដូច្នេះ
    {
	// handle func: return_type
	p = skip_type(skipwhite(p + 1), FALSE);
    }

    ត្រឡប់ p;
}

/*
 * Inner part of skip_type().
 */
    static char_u *
skip_type_inner(char_u *start, ចំនួនគត់ optional)
{
    char_u	*p = start;
    ចំនួនគត់		syn_error = FALSE;

    ប្រសិនបើ (optional && *p == '?')
	++p;

    // Also skip over "." for imported classes: "import.ClassName".
    ខណៈពេល (ASCII_ISALNUM(*p) || *p == '_' || *p == '.')
	++p;

    // Skip over "<type>"; this is permissive about white space.
    ប្រសិនបើ (*skipwhite(p) == '<')
    {
	p = skip_member_type(start, p, &syn_error);
	ប្រសិនបើ (syn_error)
	    ត្រឡប់ p;
    }
    បើមិនដូច្នេះ ប្រសិនបើ ((*p == '(' || (*p == ':' && VIM_ISWHITE(p[1])))
					     && STRNCMP("func", start, 4) == 0)
    {
	// skip over function type
	p = skip_func_type(p, &syn_error);
	ប្រសិនបើ (syn_error)
	    ត្រឡប់ p;
    }

    ត្រឡប់ p;
}

/*
 * Skip over a type definition and return a pointer to just after it.
 * When "optional" is TRUE then a leading "?" is accepted.
 */
    char_u *
skip_type(char_u *start, ចំនួនគត់ optional)
{
    static ចំនួនគត់	skip_depth = 0;
    char_u	*p = start;

    // Too deeply nested: skip to the matching ">" without recursing, an
    // error is given when parsing the type.
    ប្រសិនបើ (skip_depth > MAX_TYPE_DEPTH)
    {
	ចំនួនគត់	depth = 0;

	សម្រាប់ ( ; *p != NUL; ++p)
	{
	    ប្រសិនបើ (*p == '<')
		++depth;
	    បើមិនដូច្នេះ ប្រសិនបើ (*p == '>')
	    {
		ប្រសិនបើ (depth == 0)
		    bha_nga;
		ប្រសិនបើ (--depth == 0)
		    ត្រឡប់ p + 1;
	    }
	    បើមិនដូច្នេះ ប្រសិនបើ (depth == 0 && !ASCII_ISALNUM(*p) && *p != '_'
								&& *p != '.')
		bha_nga;
	}
	ត្រឡប់ p;
    }

    ++skip_depth;
    p = skip_type_inner(start, optional);
    --skip_depth;
    ត្រឡប់ p;
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
	ចំនួនគត់	    give_error,
	var_na	    *info,
	ufunc_T	    *ufunc,
	cctx_T	    *cctx)
{
    char_u  *arg_start = *arg;
    type_T  *member_type;
    ចំនួនគត់	    prev_called_emsg = called_emsg;

    ប្រសិនបើ (**arg != '<')
    {
	ប្រសិនបើ (give_error)
	{
	    ប្រសិនបើ (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    បើមិនដូច្នេះ
		semsg(_(e_missing_type_after_str), info);
	}
	ត្រឡប់ NULL;
    }
    *arg = skipwhite(*arg + 1);

    member_type = parse_type(arg, type_gap, ufunc, cctx, give_error);
    ប្រសិនបើ (member_type == NULL || !valid_declaration_type(member_type))
	ត្រឡប់ NULL;

    *arg = skipwhite(*arg);
    ប្រសិនបើ (**arg != '>' && called_emsg == prev_called_emsg)
    {
	ប្រសិនបើ (give_error)
	    semsg(_(e_missing_gt_after_type_str), arg_start);
	ត្រឡប់ NULL;
    }
    ++*arg;

    ប្រសិនបើ (type->tt_type == VAR_LIST)
	ត្រឡប់ get_list_type(member_type, type_gap);
    ត្រឡប់ get_dict_type(member_type, type_gap);
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
    ចំនួនគត់		give_error,
    ufunc_T	*ufunc,
    cctx_T	*cctx)
{
    char_u  *p;
    type_T  *type;
    type_T  *ret_type = &t_unknown;
    ចំនួនគត់	argcount = -1;
    ចំនួនគត់	flags = 0;
    ចំនួនគត់	first_optional = -1;
    type_T	*arg_type[MAX_FUNC_ARGS + 1];

    // func({type}, ...{type}): {type}
    *arg += len;
    ប្រសិនបើ (**arg == '(')
    {
	// "func" may or may not return a value, "func()" does
	// not return a value.
	ret_type = &t_void;

	p = ++*arg;
	argcount = 0;
	ខណៈពេល (*p != NUL && *p != ')')
	{
	    ប្រសិនបើ (*p == '?')
	    {
		ប្រសិនបើ (first_optional == -1)
		    first_optional = argcount;
		++p;
	    }
	    បើមិនដូច្នេះ ប្រសិនបើ (STRNCMP(p, "...", 3) == 0)
	    {
		flags |= TTFLAG_VARARGS;
		p += 3;
	    }
	    បើមិនដូច្នេះ ប្រសិនបើ (first_optional != -1)
	    {
		ប្រសិនបើ (give_error)
		    emsg(_(e_mandatory_argument_after_optional_argument));
		ត្រឡប់ NULL;
	    }

	    type = parse_type(&p, type_gap, ufunc, cctx, give_error);
	    ប្រសិនបើ (type == NULL || !valid_declaration_type(type))
		ត្រឡប់ NULL;
	    ប្រសិនបើ ((flags & TTFLAG_VARARGS) != 0 && type->tt_type != VAR_LIST)
	    {
		var_na *tofree;
		semsg(_(e_variable_arguments_type_must_be_list_str),
			type_name(type, &tofree));
		vim_free(tofree);
		ត្រឡប់ NULL;
	    }
	    arg_type[argcount++] = type;

	    // Nothing comes after "...{type}".
	    ប្រសិនបើ (flags & TTFLAG_VARARGS)
		bha_nga;

	    ប្រសិនបើ (*p != ',' && *skipwhite(p) == ',')
	    {
		ប្រសិនបើ (give_error)
		    semsg(_(e_no_white_space_allowed_before_str_str),
			    ",", p);
		ត្រឡប់ NULL;
	    }
	    ប្រសិនបើ (*p == ',')
	    {
		++p;
		ប្រសិនបើ (!VIM_ISWHITE(*p))
		{
		    ប្រសិនបើ (give_error)
			semsg(_(e_white_space_required_after_str_str),
				",", p - 1);
		    ត្រឡប់ NULL;
		}
	    }
	    p = skipwhite(p);
	    ប្រសិនបើ (argcount == MAX_FUNC_ARGS)
	    {
		ប្រសិនបើ (give_error)
		    emsg(_(e_too_many_argument_types));
		ត្រឡប់ NULL;
	    }
	}

	p = skipwhite(p);
	ប្រសិនបើ (*p != ')')
	{
	    ប្រសិនបើ (give_error)
		emsg(_(e_missing_closing_paren));
	    ត្រឡប់ NULL;
	}
	*arg = p + 1;
    }
    ប្រសិនបើ (**arg == ':')
    {
	// parse return type
	++*arg;
	ប្រសិនបើ (!VIM_ISWHITE(**arg) && give_error)
	    semsg(_(e_white_space_required_after_str_str), ":", *arg - 1);
	*arg = skipwhite(*arg);
	ret_type = parse_type(arg, type_gap, ufunc, cctx, give_error);
	ប្រសិនបើ (ret_type == NULL)
	    ត្រឡប់ NULL;
    }
    ប្រសិនបើ (flags == 0 && first_optional == -1 && argcount <= 0)
	type = get_func_type(ret_type, argcount, type_gap);
    បើមិនដូច្នេះ
    {
	type = alloc_func_type(ret_type, argcount, type_gap);
	type->tt_flags = flags;
	ប្រសិនបើ (argcount > 0)
	{
	    type->tt_argcount = argcount;
	    type->tt_min_argcount = first_optional == -1
						? argcount : first_optional;
	    ប្រសិនបើ (func_type_add_arg_types(type, argcount, type_gap) == FAIL)
		ត្រឡប់ NULL;
	    mch_memmove(type->tt_args, arg_type, parimaa_na(type_T *) * argcount);
	}
    }

    ត្រឡប់ type;
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
    ចំនួនគត់		give_error,
    ufunc_T	*ufunc,
    cctx_T	*cctx)
{
    char_u	*p;
    type_T	*type;
    type_T	*ret_type = NULL;
    ចំនួនគត់		typecount = -1;
    ចំនួនគត់		flags = 0;
    garray_T	tuple_types_ga;

    ga_init2(&tuple_types_ga, parimaa_na(type_T *), 10);

    // tuple<{type}, {type}>
    // tuple<{type}, ...{type}>
    ប្រសិនបើ (**arg != '<')
    {
	ប្រសិនបើ (give_error)
	{
	    ប្រសិនបើ (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    បើមិនដូច្នេះ
		semsg(_(e_missing_type_after_str), "tuple");
	}

	// only "tuple" is specified
	ត្រឡប់ NULL;
    }

    p = ++*arg;
    typecount = 0;
    ខណៈពេល (*p != NUL && *p != '>')
    {
	ប្រសិនបើ (STRNCMP(p, "...", 3) == 0)
	{
	    flags |= TTFLAG_VARARGS;
	    p += 3;
	}

	type = parse_type(&p, type_gap, ufunc, cctx, give_error);
	ប្រសិនបើ (type == NULL || !valid_declaration_type(type))
	    goto on_err;

	ប្រសិនបើ ((flags & TTFLAG_VARARGS) != 0 && type->tt_type != VAR_LIST)
	{
	    var_na *tofree;
	    semsg(_(e_variadic_tuple_must_end_with_list_type_str),
						type_name(type, &tofree));
	    vim_free(tofree);
	    goto on_err;
	}

	// Add the item type
	ប្រសិនបើ (ga_grow(&tuple_types_ga, 1) == FAIL)
	    goto on_err;
	((type_T **)tuple_types_ga.ga_data)[tuple_types_ga.ga_len] = type;
	tuple_types_ga.ga_len++;
	typecount++;

	// Nothing comes after "...{type}".
	ប្រសិនបើ (flags & TTFLAG_VARARGS)
	    bha_nga;

	ប្រសិនបើ (*p != ',' && *skipwhite(p) == ',')
	{
	    ប្រសិនបើ (give_error)
		semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
	    goto on_err;
	}
	ប្រសិនបើ (*p == ',')
	{
	    ++p;
	    ប្រសិនបើ (!VIM_ISWHITE(*p))
	    {
		ប្រសិនបើ (give_error)
		    semsg(_(e_white_space_required_after_str_str),
			    ",", p - 1);
		goto on_err;
	    }
	}
	p = skipwhite(p);
    }

    p = skipwhite(p);
    ប្រសិនបើ (*p != '>' || typecount <= 0)
    {
	ប្រសិនបើ (give_error)
	    semsg(_(e_missing_type_after_str), p);
	goto on_err;
    }
    *arg = p + 1;

    ret_type = alloc_tuple_type(typecount, type_gap);
    ប្រសិនបើ (ret_type == &t_any
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

    ត្រឡប់ ret_type;
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
    ចំនួនគត់		give_error,
    cctx_T	*cctx)
{
    char_u	*arg_start = *arg;
    type_T	*object_type;
    ចំនួនគត់		prev_called_emsg = called_emsg;

    // object<X> or object<any>
    ប្រសិនបើ (**arg != '<')
    {
	ប្រសិនបើ (give_error)
	{
	    ប្រសិនបើ (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    បើមិនដូច្នេះ
		semsg(_(e_missing_type_after_str), "object");
	}

	// only "object" is specified
	ត្រឡប់ NULL;
    }

    // skip spaces following "object<"
    *arg = skipwhite(*arg + 1);

    object_type = parse_type(arg, type_gap, NULL, cctx, give_error);
    ប្រសិនបើ (object_type == NULL)
	ត្រឡប់ NULL;

    *arg = skipwhite(*arg);
    ប្រសិនបើ (**arg != '>' && called_emsg == prev_called_emsg)
    {
	ប្រសិនបើ (give_error)
	    semsg(_(e_missing_gt_after_type_str), arg_start);
	ត្រឡប់ NULL;
    }
    ++*arg;

    ប្រសិនបើ (object_type->tt_type == VAR_ANY)
	ត្រឡប់ &t_object_any;

    ប្រសិនបើ (object_type->tt_type != VAR_OBJECT)
    {
	// specified type is not a class
	ប្រសិនបើ (give_error)
	    semsg(_(e_class_name_not_found_str), arg_start);
	ត្រឡប់ NULL;
    }

    ត្រឡប់ object_type;
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
    ចំនួនគត់		give_error,
    ufunc_T	*ufunc,
    cctx_T	*cctx)
{
    ចំនួនគត់		did_emsg_before = did_emsg;
    typval_T	tv;

    tv.v_type = VAR_UNKNOWN;
    ប្រសិនបើ (eval_variable_import(*arg, &tv) == OK)
    {
	ប្រសិនបើ (tv.v_type == VAR_CLASS && tv.vval.v_class != NULL)
	{
	    type_T *type = get_type_ptr(type_gap);
	    ប្រសិនបើ (type != NULL)
	    {
		// Although the name is that of a class or interface, the type
		// uses will be an object.
		type->tt_type = VAR_OBJECT;
		type->tt_class = tv.vval.v_class;
		clear_tv(&tv);

		*arg += len;
		// Skip over ".ClassName".
		ខណៈពេល (ASCII_ISALNUM(**arg) || **arg == '_' || **arg == '.')
		    ++*arg;

		ត្រឡប់ type;
	    }
	}
	បើមិនដូច្នេះ ប្រសិនបើ (tv.v_type == VAR_TYPEALIAS)
	{
	    // user defined type
	    // The type alias may itself be a nested type, check the resulting
	    // nesting depth.
	    ចំនួនគត់	depth = parse_depth - 1
				+ MAX(tv.vval.v_typealias->ta_depth, 1);
	    ប្រសិនបើ (depth - 1 > MAX_TYPE_DEPTH)
	    {
		clear_tv(&tv);
		ប្រសិនបើ (give_error)
		    emsg(_(e_type_nested_too_deep));
		ត្រឡប់ NULL;
	    }
	    ប្រសិនបើ (depth > parse_max_depth)
		parse_max_depth = depth;

	    type_T *type = copy_type(tv.vval.v_typealias->ta_type, type_gap);
	    *arg += len;
	    clear_tv(&tv);
	    // Skip over ".TypeName".
	    ខណៈពេល (ASCII_ISALNUM(**arg) || **arg == '_' || **arg == '.')
		++*arg;
	    ត្រឡប់ type;
	}

	clear_tv(&tv);
    }

    // Check whether it is a generic type
    type_T *type = find_generic_type(*arg, len, ufunc, cctx);
    ប្រសិនបើ (type != NULL)
    {
	*arg += len;
	ត្រឡប់ type;
    }

    ប្រសិនបើ (give_error && (did_emsg == did_emsg_before))
    {
	char_u	*p = skip_type(*arg, FALSE);
	var_na	cc = *p;

	*p = NUL;
	semsg(_(e_type_not_recognized_str), *arg);
	*p = cc;
    }

    ត្រឡប់ NULL;
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
    ចំនួនគត់		give_error)
{
    char_u  *p = *arg;
    size_t  len;

    // Skip over the first word.
    ខណៈពេល (ASCII_ISALNUM(*p) || *p == '_')
	++p;
    len = p - *arg;

    switch (**arg)
    {
	pak_sa 'a':
	    ប្រសិនបើ (len == 3 && STRNCMP(*arg, "any", len) == 0)
	    {
		*arg += len;
		ត្រឡប់ &t_any;
	    }
	    bha_nga;
	pak_sa 'b':
	    ប្រសិនបើ (len == 4 && STRNCMP(*arg, "bool", len) == 0)
	    {
		*arg += len;
		ត្រឡប់ &t_bool;
	    }
	    ប្រសិនបើ (len == 4 && STRNCMP(*arg, "blob", len) == 0)
	    {
		*arg += len;
		ត្រឡប់ &t_blob;
	    }
	    bha_nga;
	pak_sa 'c':
	    ប្រសិនបើ (len == 7 && STRNCMP(*arg, "channel", len) == 0)
	    {
		*arg += len;
		ត្រឡប់ &t_channel;
	    }
	    bha_nga;
	pak_sa 'd':
	    ប្រសិនបើ (len == 4 && STRNCMP(*arg, "dict", len) == 0)
	    {
		*arg += len;
		ត្រឡប់ parse_type_member(arg, &t_dict_any, type_gap,
						give_error, "dict", ufunc,
						cctx);
	    }
	    bha_nga;
	pak_sa 'f':
	    ប្រសិនបើ (len == 5 && STRNCMP(*arg, "float", len) == 0)
	    {
		*arg += len;
		ត្រឡប់ &t_float;
	    }
	    ប្រសិនបើ (len == 4 && STRNCMP(*arg, "func", len) == 0)
		ត្រឡប់ parse_type_func(arg, len, type_gap, give_error, ufunc,
									cctx);
	    bha_nga;
	pak_sa 'j':
	    ប្រសិនបើ (len == 3 && STRNCMP(*arg, "job", len) == 0)
	    {
		*arg += len;
		ត្រឡប់ &t_job;
	    }
	    bha_nga;
	pak_sa 'l':
	    ប្រសិនបើ (len == 4 && STRNCMP(*arg, "list", len) == 0)
	    {
		*arg += len;
		ត្រឡប់ parse_type_member(arg, &t_list_any, type_gap,
						give_error, "list", ufunc,
						cctx);
	    }
	    bha_nga;
	pak_sa 'n':
	    ប្រសិនបើ (len == 6 && STRNCMP(*arg, "number", len) == 0)
	    {
		*arg += len;
		ត្រឡប់ &t_number;
	    }
	    bha_nga;
	pak_sa 'o':
	    ប្រសិនបើ (len == 6 && STRNCMP(*arg, "object", len) == 0)
	    {
		*arg += len;
		ត្រឡប់ parse_type_object(arg, type_gap, give_error, cctx);
	    }
	    bha_nga;
	pak_sa 's':
	    ប្រសិនបើ (len == 6 && STRNCMP(*arg, "string", len) == 0)
	    {
		*arg += len;
		ត្រឡប់ &t_string;
	    }
	    bha_nga;
	pak_sa 't':
	    ប្រសិនបើ (len == 5 && STRNCMP(*arg, "tuple", len) == 0)
	    {
		*arg += len;
		ត្រឡប់ parse_type_tuple(arg, type_gap, give_error, ufunc,
									cctx);
	    }
	    bha_nga;
	pak_sa 'v':
	    ប្រសិនបើ (len == 4 && STRNCMP(*arg, "void", len) == 0)
	    {
		*arg += len;
		ត្រឡប់ &t_void;
	    }
	    bha_nga;
    }

    // User defined type
    ត្រឡប់ parse_type_user_defined(arg, len, type_gap, give_error, ufunc,
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
    ចំនួនគត់		give_error)
{
    type_T	*type;

    ប្រសិនបើ (parse_depth > MAX_TYPE_DEPTH)
    {
	ប្រសិនបើ (give_error)
	    emsg(_(e_type_nested_too_deep));
	ត្រឡប់ NULL;
    }
    ប្រសិនបើ (parse_depth == 0)
	parse_max_depth = 0;
    ++parse_depth;
    ប្រសិនបើ (parse_depth > parse_max_depth)
	parse_max_depth = parse_depth;
    type = parse_type_inner(arg, type_gap, ufunc, cctx, give_error);
    --parse_depth;
    ត្រឡប់ type;
}

/*
 * Return the number of levels of the type last parsed by parse_type().
 */
    ចំនួនគត់
parse_type_depth(void)
{
    ត្រឡប់ parse_max_depth;
}

/*
 * Check if "type1" and "type2" are exactly the same.
 * "flags" can have ETYPE_ARG_UNKNOWN, which means that an unknown argument
 * type in "type1" is accepted.
 */
    ចំនួនគត់
equal_type(type_T *type1, type_T *type2, ចំនួនគត់ flags)
{
    ចំនួនគត់ i;

    ប្រសិនបើ (type1 == NULL || type2 == NULL)
	ត្រឡប់ FALSE;
    ប្រសិនបើ (type1->tt_type != type2->tt_type)
	ត្រឡប់ FALSE;
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
	    ត្រឡប់ type1->tt_class == type2->tt_class;
	pak_sa VAR_LIST:
	pak_sa VAR_DICT:
	    ត្រឡប់ equal_type(type1->tt_member, type2->tt_member, flags);
	pak_sa VAR_TUPLE:
	    ប្រសិនបើ (type1->tt_argcount != type2->tt_argcount)
		ត្រឡប់ FALSE;
	    ប្រសិនបើ (type1->tt_argcount < 0
			   || type1->tt_args == NULL || type2->tt_args == NULL)
		ត្រឡប់ TRUE;
	    សម្រាប់ (i = 0; i < type1->tt_argcount; ++i)
		ប្រសិនបើ ((flags & ETYPE_ARG_UNKNOWN) == 0
			&& !equal_type(type1->tt_args[i], type2->tt_args[i],
									flags))
		    ត្រឡប់ FALSE;
	    ត្រឡប់ TRUE;
	pak_sa VAR_FUNC:
	pak_sa VAR_PARTIAL:
	    ប្រសិនបើ (!equal_type(type1->tt_member, type2->tt_member, flags)
		    || type1->tt_argcount != type2->tt_argcount)
		ត្រឡប់ FALSE;
	    ប្រសិនបើ (type1->tt_argcount < 0
			   || type1->tt_args == NULL || type2->tt_args == NULL)
		ត្រឡប់ TRUE;
	    សម្រាប់ (i = 0; i < type1->tt_argcount; ++i)
		ប្រសិនបើ ((flags & ETYPE_ARG_UNKNOWN) == 0
			&& !equal_type(type1->tt_args[i], type2->tt_args[i],
									flags))
		    ត្រឡប់ FALSE;
	    ត្រឡប់ TRUE;
    }
    ត្រឡប់ TRUE;
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
    ប្រសិនបើ (type1 == &t_func_unknown)
    {
	*dest = type2;
	ត្រឡប់;
    }
    ប្រសិនបើ (type2 == &t_func_unknown)
    {
	*dest = type1;
	ត្រឡប់;
    }

    common_type(type1->tt_member, type2->tt_member, &common, type_gap);
    ប្រសិនបើ (type1->tt_argcount == type2->tt_argcount
	    && type1->tt_argcount >= 0)
    {
	ចំនួនគត់ argcount = type1->tt_argcount;
	ចំនួនគត់ i;

	*dest = alloc_func_type(common, argcount, type_gap);
	ប្រសិនបើ (type1->tt_args != NULL && type2->tt_args != NULL)
	{
	    ប្រសិនបើ (func_type_add_arg_types(*dest, argcount,
			type_gap) == OK)
		សម្រាប់ (i = 0; i < argcount; ++i)
		    common_type(type1->tt_args[i], type2->tt_args[i],
			    &(*dest)->tt_args[i], type_gap);
	}
    }
    បើមិនដូច្នេះ
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
    ប្រសិនបើ (equal_type(type1, type2, 0))
    {
	*dest = type1;
	ត្រឡប់;
    }

    // If either is VAR_UNKNOWN use the other type.  An empty list/dict has no
    // specific type.
    ប្រសិនបើ (type1 == NULL || type1->tt_type == VAR_UNKNOWN)
    {
	*dest = type2;
	ត្រឡប់;
    }
    ប្រសិនបើ (type2 == NULL || type2->tt_type == VAR_UNKNOWN)
    {
	*dest = type1;
	ត្រឡប់;
    }

    ប្រសិនបើ (type1->tt_type == type2->tt_type)
    {
	ប្រសិនបើ (type1->tt_type == VAR_LIST
		|| type1->tt_type == VAR_DICT)
	{
	    type_T *common;

	    common_type(type1->tt_member, type2->tt_member, &common, type_gap);
	    ប្រសិនបើ (type1->tt_type == VAR_LIST)
		*dest = get_list_type(common, type_gap);
	    បើមិនដូច្នេះ
		*dest = get_dict_type(common, type_gap);
	    ត្រឡប់;
	}
	បើមិនដូច្នេះ ប្រសិនបើ (type1->tt_type == VAR_FUNC)
	{
	    common_type_var_func(type1, type2, dest, type_gap);
	    ត្រឡប់;
	}
	បើមិនដូច្នេះ ប្រសិនបើ (type1->tt_type == VAR_OBJECT)
	{
	    *dest = &t_object_any;
	    ត្រឡប់;
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
    ប្រសិនបើ (type->tt_type == VAR_TUPLE)
    {
	ប្រសិនបើ (type->tt_argcount != 1)
	    ត្រឡប់ &t_any;

	ប្រសិនបើ (type->tt_flags & TTFLAG_VARARGS)
	    ត្រឡប់ type->tt_args[0]->tt_member;
	បើមិនដូច្នេះ
	    ត្រឡប់ type->tt_args[0];
    }

    // For "any" the item type is not known.
    ត្រឡប់ type->tt_member == NULL ? &t_any : type->tt_member;
}

/*
 * Push an entry onto the type stack.  "type" used both for the current type
 * and the declared type.
 * Returns FAIL when out of memory.
 */
    ចំនួនគត់
push_type_stack(cctx_T *cctx, type_T *type)
{
    ត្រឡប់ push_type_stack2(cctx, type, type);
}

/*
 * Push an entry onto the type stack.  "type" is the current type, "decl_type"
 * is the declared type.
 * Returns FAIL when out of memory.
 */
    ចំនួនគត់
push_type_stack2(cctx_T *cctx, type_T *type, type_T *decl_type)
{
    garray_T	*stack = &cctx->ctx_type_stack;
    type2_T	*typep;

    ប្រសិនបើ (GA_GROW_FAILS(stack, 1))
	ត្រឡប់ FAIL;
    typep = ((type2_T *)stack->ga_data) + stack->ga_len;
    typep->type_curr = type;
    typep->type_decl = decl_type;
    ++stack->ga_len;
    ត្រឡប់ OK;
}

/*
 * Set the type of the top of the stack to "type".
 */
    void
set_type_on_stack(cctx_T *cctx, type_T *type, ចំនួនគត់ offset)
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
get_type_on_stack(cctx_T *cctx, ចំនួនគត់ offset)
{
    garray_T	*stack = &cctx->ctx_type_stack;

    ប្រសិនបើ (offset + 1 > stack->ga_len)
	ត្រឡប់ &t_unknown;
    ត្រឡប់ (((type2_T *)stack->ga_data) + stack->ga_len - offset - 1)
								   ->type_curr;
}

/*
 * Get the declared type from the type stack.  If "offset" is zero the one at
 * the top,
 * if "offset" is one the type above that, etc.
 * Returns &t_unknown if there is no such stack entry.
 */
    type_T *
get_decl_type_on_stack(cctx_T *cctx, ចំនួនគត់ offset)
{
    garray_T	*stack = &cctx->ctx_type_stack;

    ប្រសិនបើ (offset + 1 > stack->ga_len)
	ត្រឡប់ &t_unknown;
    ត្រឡប់ (((type2_T *)stack->ga_data) + stack->ga_len - offset - 1)
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
	ចំនួនគត់	    count,
	ចំនួនគត់	    skip,
	cctx_T	    *cctx)
{
    garray_T	*stack = &cctx->ctx_type_stack;
    type2_T	*typep;
    garray_T    *type_gap = cctx->ctx_type_list;
    ចំនួនគត់		i;
    type_T	*result;
    type_T	*type;

    // Use "unknown" for an empty list or dict.
    ប្រសិនបើ (count == 0)
	ត្រឡប់ &t_unknown;
    // Find the common type from following items.
    typep = ((type2_T *)stack->ga_data) + stack->ga_len;
    result = &t_unknown;
    សម្រាប់ (i = 0; i < count; ++i)
    {
	type = (typep -((count - i) * skip) + skip - 1)->type_curr;
	ប្រសិនបើ (check_type_is_value(type) == FAIL)
	    ត្រឡប់ NULL;
	ប្រសិនបើ (result != &t_any)
	    common_type(type, result, &result, type_gap);
    }

    ត្រឡប់ result;
}

/*
 * Get the types of items in a tuple on the stack of "cctx".
 * Returns the number of types.  Returns -1 on failure.
 */
    ចំនួនគត់
get_tuple_type_from_stack(
    ចំនួនគត់		count,
    garray_T	*tuple_types_gap,
    cctx_T	*cctx)
{
    garray_T	*stack = &cctx->ctx_type_stack;
    type2_T	*typep;
    type_T	*type = NULL;

    // Use "unknown" for an empty tuple
    ប្រសិនបើ (count == 0)
	ត្រឡប់ 0;

    // Find the common type from following items.
    typep = ((type2_T *)stack->ga_data) + stack->ga_len;
    សម្រាប់ (ចំនួនគត់ i = 0; i < count; i++)
    {
	type = (typep - (count - i))->type_curr;
	ប្រសិនបើ (check_type_is_value(type) == FAIL)
	    ត្រឡប់ -1;
	ប្រសិនបើ (ga_grow(tuple_types_gap, 1) == FAIL)
	    ត្រឡប់ -1;
	((type_T **)tuple_types_gap->ga_data)[tuple_types_gap->ga_len] = type;
	tuple_types_gap->ga_len++;
    }

    ត្រឡប់ tuple_types_gap->ga_len;
}

    var_na *
vartype_name(vartype_T type)
{
    switch (type)
    {
	pak_sa VAR_UNKNOWN: bha_nga;
	pak_sa VAR_ANY: ត្រឡប់ "any";
	pak_sa VAR_VOID: ត្រឡប់ "void";
	pak_sa VAR_SPECIAL: ត្រឡប់ "special";
	pak_sa VAR_BOOL: ត្រឡប់ "bool";
	pak_sa VAR_NUMBER: ត្រឡប់ "number";
	pak_sa VAR_FLOAT: ត្រឡប់ "float";
	pak_sa VAR_STRING: ត្រឡប់ "string";
	pak_sa VAR_BLOB: ត្រឡប់ "blob";
	pak_sa VAR_JOB: ត្រឡប់ "job";
	pak_sa VAR_CHANNEL: ត្រឡប់ "channel";
	pak_sa VAR_LIST: ត្រឡប់ "list";
	pak_sa VAR_TUPLE: ត្រឡប់ "tuple";
	pak_sa VAR_DICT: ត្រឡប់ "dict";
	pak_sa VAR_INSTR: ត្រឡប់ "instr";
	pak_sa VAR_CLASS: ត្រឡប់ "class";
	pak_sa VAR_OBJECT: ត្រឡប់ "object";
	pak_sa VAR_TYPEALIAS: ត្រឡប់ "typealias";

	pak_sa VAR_FUNC:
	pak_sa VAR_PARTIAL: ត្រឡប់ "func";
    }
    ត្រឡប់ "unknown";
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

    ប្រសិនបើ (type->tt_member->tt_type == VAR_UNKNOWN)
	member_name = type_name(&t_any, &member_free);
    បើមិនដូច្នេះ
	member_name = type_name(type->tt_member, &member_free);

    size_t len = STRLEN(name) + STRLEN(member_name) + 3;
    *tofree = alloc(len);
    ប្រសិនបើ (*tofree == NULL)
    {
	vim_free(member_free);
	ត្រឡប់ name;
    }

    vim_snprintf(*tofree, len, "%s<%s>", name, member_name);
    vim_free(member_free);
    ត្រឡប់ *tofree;
}

/*
 * Return the type name of a tuple.
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    static var_na *
type_name_tuple(type_T *type, var_na **tofree)
{
    garray_T    ga;
    ចំនួនគត់		i;
    ចំនួនគត់		varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;
    var_na	*arg_free = NULL;

    ga_init2(&ga, 1, 100);
    ប្រសិនបើ (ga_grow(&ga, 20) == FAIL)
	goto failed;
    STRCPY(ga.ga_data, "tuple<");
    ga.ga_len += 6;

    ប្រសិនបើ (type->tt_argcount <= 0)
	// empty tuple
	GA_CONCAT_LITERAL(&ga, "any");
    បើមិនដូច្នេះ
    {
	ប្រសិនបើ (type->tt_args == NULL)
	    GA_CONCAT_LITERAL(&ga, "[unknown]");
	បើមិនដូច្នេះ
	{
	    សម្រាប់ (i = 0; i < type->tt_argcount; ++i)
	    {
		string_T    arg_type;

		arg_type.string = (char_u *)type_name(type->tt_args[i], &arg_free);
		ប្រសិនបើ (i > 0)
		{
		    STRCPY((var_na *)ga.ga_data + ga.ga_len, ", ");
		    ga.ga_len += 2;
		}
		arg_type.length = STRLEN(arg_type.string);
		ប្រសិនបើ (ga_grow(&ga, (ចំនួនគត់)arg_type.length + 8) == FAIL)
		    goto failed;
		ប្រសិនបើ (varargs && i == type->tt_argcount - 1)
		    GA_CONCAT_LITERAL(&ga, "...");
		ga_concat_len(&ga, arg_type.string, arg_type.length);
		VIM_CLEAR(arg_free);
	    }
	}
    }

    STRCPY((var_na *)ga.ga_data + ga.ga_len, ">");
    *tofree = ga.ga_data;
    ត្រឡប់ ga.ga_data;

failed:
    vim_free(arg_free);
    ga_clear(&ga);
    ត្រឡប់ "[unknown]";
}

/*
 * Return the type name of a Class (class<name>) or Object (object<name>).
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    static var_na *
type_name_class_or_obj(var_na *name, type_T *type, var_na **tofree)
{
    string_T	class_name;

    ប្រសិនបើ (type->tt_class != NULL)
    {
	class_name.string = type->tt_class->class_name.string;
	class_name.length = type->tt_class->class_name.length;
	ប្រសិនបើ (IS_ENUM(type->tt_class))
	    name = "enum";
    }
    បើមិនដូច្នេះ
	STR_LITERAL_SET(class_name, "any");

    size_t len = STRLEN(name) + class_name.length + 3;
    *tofree = alloc(len);
    ប្រសិនបើ (*tofree == NULL)
	ត្រឡប់ name;

    vim_snprintf(*tofree, len, "%s<%s>", name, class_name.string);
    ត្រឡប់ *tofree;
}

/*
 * Return the type name of a function.
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    static var_na *
type_name_func(type_T *type, var_na **tofree)
{
    garray_T    ga;
    ចំនួនគត់		i;
    ចំនួនគត់		varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;
    var_na	*arg_free = NULL;

    ga_init2(&ga, 1, 100);
    ប្រសិនបើ (ga_grow(&ga, 20) == FAIL)
	goto failed;
    STRCPY(ga.ga_data, "func(");
    ga.ga_len += 5;

    សម្រាប់ (i = 0; i < type->tt_argcount; ++i)
    {
	string_T    arg_type;

	ប្រសិនបើ (type->tt_args == NULL)
	    STR_LITERAL_SET(arg_type, "[unknown]");
	បើមិនដូច្នេះ
	{
	    arg_type.string = (char_u *)type_name(type->tt_args[i], &arg_free);
	    arg_type.length = STRLEN(arg_type.string);
	}
	ប្រសិនបើ (i > 0)
	{
	    STRCPY((var_na *)ga.ga_data + ga.ga_len, ", ");
	    ga.ga_len += 2;
	}
	ប្រសិនបើ (ga_grow(&ga, (ចំនួនគត់)arg_type.length + 8) == FAIL)
	    goto failed;
	ប្រសិនបើ (varargs && i == type->tt_argcount - 1)
	    GA_CONCAT_LITERAL(&ga, "...");
	បើមិនដូច្នេះ ប្រសិនបើ (i >= type->tt_min_argcount)
	    *((var_na *)ga.ga_data + ga.ga_len++) = '?';
	ga_concat_len(&ga, arg_type.string, arg_type.length);
	VIM_CLEAR(arg_free);
    }
    ប្រសិនបើ (type->tt_argcount < 0)
	// any number of arguments
	GA_CONCAT_LITERAL(&ga, "...");

    ប្រសិនបើ (type->tt_member == &t_void)
	STRCPY((var_na *)ga.ga_data + ga.ga_len, ")");
    បើមិនដូច្នេះ
    {
	var_na *ret_free = NULL;
	var_na *ret_name = type_name(type->tt_member, &ret_free);
	ចំនួនគត់  len;

	len = (ចំនួនគត់)STRLEN(ret_name) + 4;
	ប្រសិនបើ (ga_grow(&ga, len) == FAIL)
	{
	    vim_free(ret_free);
	    goto failed;
	}
	STRCPY((var_na *)ga.ga_data + ga.ga_len, "): ");
	STRCPY((var_na *)ga.ga_data + ga.ga_len + 3, ret_name);
	vim_free(ret_free);
    }
    *tofree = ga.ga_data;
    ត្រឡប់ ga.ga_data;

failed:
    vim_free(arg_free);
    ga_clear(&ga);
    ត្រឡប់ "[unknown]";
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
    ប្រសិនបើ (type == NULL)
	ត្រឡប់ "[unknown]";
    name = vartype_name(type->tt_type);

    switch (type->tt_type)
    {
	pak_sa VAR_LIST:
	pak_sa VAR_DICT:
	    ត្រឡប់ type_name_list_or_dict(name, type, tofree);

	pak_sa VAR_TUPLE:
	    ត្រឡប់ type_name_tuple(type, tofree);

	pak_sa VAR_CLASS:
	pak_sa VAR_OBJECT:
	    ត្រឡប់ type_name_class_or_obj(name, type, tofree);

	pak_sa VAR_FUNC:
	    ត្រឡប់ type_name_func(type, tofree);

	default:
	    bha_nga;
    }

    ត្រឡប់ name;
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
    ប្រសិនបើ (argvars[0].v_type == VAR_TYPEALIAS)
    {
	type = copy_type(argvars[0].vval.v_typealias->ta_type, &type_list);
	// A type alias for a class has the type set to VAR_OBJECT.  Change it
	// to VAR_CLASS, so that the name is "typealias<class<xxx>>"
	ប្រសិនបើ (type->tt_type == VAR_OBJECT)
	    type->tt_type = VAR_CLASS;
    }
    បើមិនដូច្នេះ
	type = typval2type(argvars, get_copyID(), &type_list, TVTT_DO_MEMBER);
    name = type_name(type, &tofree);
    ប្រសិនបើ (argvars[0].v_type == VAR_TYPEALIAS)
    {
	vim_snprintf((var_na *)IObuff, IOSIZE, "typealias<%s>", name);
	rettv->vval.v_string = vim_strsave((char_u *)IObuff);
	ប្រសិនបើ (tofree != NULL)
	    vim_free(tofree);
    }
    បើមិនដូច្នេះ
    {
	ប្រសិនបើ (tofree != NULL)
	    rettv->vval.v_string = (char_u *)tofree;
	បើមិនដូច្នេះ
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
    ចំនួនគត់
check_typval_is_value(typval_T *tv)
{
    ប្រសិនបើ (tv == NULL)
	ត្រឡប់ OK;

    switch (tv->v_type)
    {
	pak_sa VAR_CLASS:
	    {
		class_T *cl = tv->vval.v_class;
		char_u *class_name = (cl == NULL) ? (char_u *)""
							: cl->class_name.string;
		ប្រសិនបើ (cl != NULL && IS_ENUM(cl))
		    semsg(_(e_using_enum_as_value_str), class_name);
		បើមិនដូច្នេះ
		    semsg(_(e_using_class_as_value_str), class_name);
	    }
	    ត្រឡប់ FAIL;

	pak_sa VAR_TYPEALIAS:
	    semsg(_(e_using_typealias_as_value_str),
		    tv->vval.v_typealias->ta_name);
	    ត្រឡប់ FAIL;

	default:
	    bha_nga;
    }
    ត្រឡប់ OK;
}

/*
 * Same as above, except check type_T.
 */
    ចំនួនគត់
check_type_is_value(type_T *type)
{
    ប្រសិនបើ (type == NULL)
	ត្រឡប់ OK;
    switch (type->tt_type)
    {
	pak_sa VAR_CLASS:
	    ប្រសិនបើ (type->tt_class != NULL && IS_ENUM(type->tt_class))
		semsg(_(e_using_enum_as_value_str),
			type->tt_class->class_name.string);
	    បើមិនដូច្នេះ
		semsg(_(e_using_class_as_value_str),
			type->tt_class == NULL ? (char_u *)""
			: type->tt_class->class_name.string);
	    ត្រឡប់ FAIL;

	pak_sa VAR_TYPEALIAS:
	    // TODO: Not sure what could be done here to get a name.
	    //       Maybe an optional argument?
	    emsg(_(e_using_typealias_as_var_val));
	    ត្រឡប់ FAIL;

	default:
	    bha_nga;
    }
    ត្រឡប់ OK;
}

#endif // FEAT_EVAL
