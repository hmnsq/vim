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

#nếu defined(FEAT_EVAL)

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

    nếu (ga_grow(type_gap, 1) == FAIL)
	trả_về NULL;
    type = ALLOC_CLEAR_ONE(type_T);
    nếu (type == NULL)
	trả_về NULL;

    ((type_T **)type_gap->ga_data)[type_gap->ga_len] = type;
    ++type_gap->ga_len;
    trả_về type;
}

/*
 * Make a shallow copy of "type".
 * When allocation fails returns "type".
 */
    type_T *
copy_type(type_T *type, garray_T *type_gap)
{
    type_T *copy = get_type_ptr(type_gap);

    nếu (copy == NULL)
	trả_về type;
    *copy = *type;
    copy->tt_flags &= ~TTFLAG_STATIC;

    nếu (type->tt_args != NULL
	   && func_type_add_arg_types(copy, type->tt_argcount, type_gap) == OK)
	cho (số_nguyên i = 0; i < type->tt_argcount; ++i)
	    copy->tt_args[i] = type->tt_args[i];

    trả_về copy;
}

/*
 * Inner part of copy_type_deep().
 * When allocation fails returns "type".
 */
    static type_T *
copy_type_deep_rec(type_T *type, garray_T *type_gap, garray_T *seen_types)
{
    cho (số_nguyên i = 0; i < seen_types->ga_len; ++i)
	nếu (((type_T **)seen_types->ga_data)[i * 2] == type)
	    // seen this type before, return the copy we made
	    trả_về ((type_T **)seen_types->ga_data)[i * 2 + 1];

    type_T *copy = copy_type(type, type_gap);
    nếu (ga_grow(seen_types, 1) == FAIL)
	trả_về copy;
    ((type_T **)seen_types->ga_data)[seen_types->ga_len * 2] = type;
    ((type_T **)seen_types->ga_data)[seen_types->ga_len * 2 + 1] = copy;
    ++seen_types->ga_len;

    nếu (copy->tt_member != NULL)
	copy->tt_member = copy_type_deep_rec(copy->tt_member,
							 type_gap, seen_types);
    nếu (type->tt_args != NULL)
	cho (số_nguyên i = 0; i < type->tt_argcount; ++i)
	    copy->tt_args[i] = copy_type_deep_rec(copy->tt_args[i],
							 type_gap, seen_types);

    trả_về copy;
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
    ga_init2(&seen_types, sizeof(type_T *) * 2, 20);

    type_T *res = copy_type_deep_rec(type, type_gap, &seen_types);

    ga_clear(&seen_types);
    trả_về res;
}

    void
clear_type_list(garray_T *gap)
{
    trong_khi (gap->ga_len > 0)
	vim_free(((type_T **)gap->ga_data)[--gap->ga_len]);
    ga_clear(gap);
}

    void
clear_func_type_list(garray_T *gap, type_T **func_type)
{
    trong_khi (gap->ga_len > 0)
    {
	// func_type pointing to the uf_type_list, so reset pointer
	nếu (*func_type == ((type_T **)gap->ga_data)[--gap->ga_len])
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

    nếu (type == NULL)
	trả_về NULL;

    // A fixed type never contains allocated types, return as-is.
    nếu (type->tt_flags & TTFLAG_STATIC)
	trả_về type;

    ret = ALLOC_ONE(type_T);
    nếu (ret == NULL)
	trả_về NULL;
    *ret = *type;

    nếu (ret->tt_member != NULL)
	ret->tt_member = alloc_type(ret->tt_member);

    nếu (type->tt_argcount > 0 && type->tt_args != NULL)
    {
	số_nguyên i;

	ret->tt_args = ALLOC_MULT(type_T *, type->tt_argcount);
	nếu (ret->tt_args != NULL)
	    cho (i = 0; i < type->tt_argcount; ++i)
		ret->tt_args[i] = alloc_type(type->tt_args[i]);
    }
    không_thì
	ret->tt_args = NULL;

    trả_về ret;
}

/*
 * Free a type that was created with alloc_type().
 */
    void
free_type(type_T *type)
{
    số_nguyên i;

    nếu (type == NULL || (type->tt_flags & TTFLAG_STATIC))
	trả_về;
    nếu (type->tt_args != NULL)
    {
	cho (i = 0; i < type->tt_argcount; ++i)
	    free_type(type->tt_args[i]);
	vim_free(type->tt_args);
    }

    free_type(type->tt_member);

    vim_free(type);
}

/*
 * Return TRUE if "type" is to be recursed into for setting the type.
 */
    static số_nguyên
set_tv_type_recurse(type_T *type)
{
    trả_về type->tt_member != NULL
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
    nếu (d->dv_type == type)
	trả_về;

    free_type(d->dv_type);
    d->dv_type = alloc_type(type);

    // Need to recursively set the type of dict items?
    nếu (!set_tv_type_recurse(type))
	trả_về;

    số_nguyên		todo = (số_nguyên)d->dv_hashtab.ht_used;
    hashitem_T	*hi;
    dictitem_T	*di;

    FOR_ALL_HASHTAB_ITEMS(&d->dv_hashtab, hi, todo)
    {
	nếu (!HASHITEM_EMPTY(hi))
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
    nếu (l->lv_type == type)
	trả_về;

    free_type(l->lv_type);
    l->lv_type = alloc_type(type);

    // Need to recursively set the type of list items?
    nếu (l->lv_first == &range_list_item || !set_tv_type_recurse(type))
	trả_về;

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
    nếu (tuple->tv_type == type)
	trả_về;

    free_type(tuple->tv_type);
    tuple->tv_type = alloc_type(type);

    nếu (type->tt_argcount <= 0)
	trả_về;

    // recursively set the type of list items
    type_T	*item_type;
    cho (số_nguyên i = 0; i < tuple_len(tuple); i++)
    {
	nếu ((type->tt_flags & TTFLAG_VARARGS) && (i >= type->tt_argcount - 1))
	    // For a variadic tuple, the last type is a List.  So use the
	    // List member type.
	    item_type = type->tt_args[type->tt_argcount - 1]->tt_member;
	không_thì
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
    nếu (type->tt_type == VAR_ANY)
	// If the variable type is "any", then keep the value type.
	// e.g.  var x: any = [1, 2] or var y: any = {v: 1}
	trả_về;

    nếu (tv->v_type == VAR_DICT && tv->vval.v_dict != NULL)
	set_tv_type_dict(tv->vval.v_dict, type);
    không_thì nếu (tv->v_type == VAR_LIST && tv->vval.v_list != NULL)
	set_tv_type_list(tv->vval.v_list, type);
    không_thì nếu (tv->v_type == VAR_TUPLE && tv->vval.v_tuple != NULL)
	set_tv_type_tuple(tv->vval.v_tuple, type);
}

/*
 * For a tuple type, reserve space for "typecount" types (including the
 * repeated type).
 */
    static số_nguyên
tuple_type_add_types(
    type_T	*tupletype,
    số_nguyên		typecount,
    garray_T	*type_gap)
{
    // To make it easy to free the space needed for the types, add the
    // pointer to type_gap.
    nếu (ga_grow(type_gap, 1) == FAIL)
	trả_về FAIL;
    tupletype->tt_args = alloc_clear_id(sizeof(type_T *) * typecount,
							  aid_tuple_type_args);
    nếu (tupletype->tt_args == NULL)
	trả_về FAIL;
    ((type_T **)type_gap->ga_data)[type_gap->ga_len] =
						(void *)tupletype->tt_args;
    ++type_gap->ga_len;
    trả_về OK;
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
    nếu (member_type == NULL || (member_type->tt_type == VAR_ANY
					&& !IS_GENERIC_TYPE(member_type)))
	trả_về &t_list_any;
    nếu (member_type->tt_type == VAR_VOID
	    || member_type->tt_type == VAR_UNKNOWN)
	trả_về &t_list_empty;
    nếu (member_type->tt_type == VAR_BOOL)
	trả_về &t_list_bool;
    nếu (member_type->tt_type == VAR_NUMBER)
	trả_về &t_list_number;
    nếu (member_type->tt_type == VAR_STRING)
	trả_về &t_list_string;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    nếu (type == NULL)
	trả_về &t_any;
    type->tt_type = VAR_LIST;
    type->tt_member = member_type;
    type->tt_argcount = 0;
    type->tt_args = NULL;
    trả_về type;
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
    số_nguyên		typecount = tuple_types_gap->ga_len;

    // recognize commonly used types
    nếu (typecount == 0)
	trả_về &t_tuple_any;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    nếu (type == NULL)
	trả_về &t_any;
    type->tt_type = VAR_TUPLE;
    type->tt_member = NULL;
    nếu (typecount > 0)
    {
	nếu (tuple_type_add_types(type, typecount, type_gap) == FAIL)
	    trả_về &t_any;
	mch_memmove(type->tt_args, tuple_types, sizeof(type_T *) * typecount);
    }
    type->tt_argcount = typecount;
    type->tt_flags = 0;

    trả_về type;
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
    nếu (member_type == NULL || (member_type->tt_type == VAR_ANY
					&& !IS_GENERIC_TYPE(member_type)))
	trả_về &t_dict_any;
    nếu (member_type->tt_type == VAR_VOID
	    || member_type->tt_type == VAR_UNKNOWN)
	trả_về &t_dict_empty;
    nếu (member_type->tt_type == VAR_BOOL)
	trả_về &t_dict_bool;
    nếu (member_type->tt_type == VAR_NUMBER)
	trả_về &t_dict_number;
    nếu (member_type->tt_type == VAR_STRING)
	trả_về &t_dict_string;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    nếu (type == NULL)
	trả_về &t_any;
    type->tt_type = VAR_DICT;
    type->tt_member = member_type;
    type->tt_argcount = 0;
    type->tt_args = NULL;
    trả_về type;
}

/*
 * Allocate a new type for a function.
 */
    type_T *
alloc_func_type(type_T *ret_type, số_nguyên argcount, garray_T *type_gap)
{
    type_T *type = get_type_ptr(type_gap);

    nếu (type == NULL)
	trả_về &t_any;
    type->tt_type = VAR_FUNC;
    type->tt_member = ret_type == NULL ? &t_unknown : ret_type;
    type->tt_argcount = argcount;
    type->tt_args = NULL;
    trả_về type;
}

/*
 * Allocate a new type for a tuple.
 */
    static type_T *
alloc_tuple_type(số_nguyên typecount, garray_T *type_gap)
{
    type_T *type = get_type_ptr(type_gap);

    nếu (type == NULL)
	trả_về &t_any;
    type->tt_type = VAR_TUPLE;
    type->tt_member = NULL;
    type->tt_argcount = typecount;
    type->tt_args = NULL;
    trả_về type;
}

/*
 * Get a function type, based on the return type "ret_type".
 * "argcount" must be -1 or 0, a predefined type can be used.
 */
    type_T *
get_func_type(type_T *ret_type, số_nguyên argcount, garray_T *type_gap)
{
    // recognize commonly used types
    nếu (ret_type == &t_unknown || ret_type == NULL)
    {
	// (argcount == 0) is not possible
	trả_về &t_func_unknown;
    }
    nếu (ret_type == &t_void)
    {
	nếu (argcount == 0)
	    trả_về &t_func_0_void;
	không_thì
	    trả_về &t_func_void;
    }
    nếu (ret_type == &t_any)
    {
	nếu (argcount == 0)
	    trả_về &t_func_0_any;
	không_thì
	    trả_về &t_func_any;
    }
    nếu (ret_type == &t_number)
    {
	nếu (argcount == 0)
	    trả_về &t_func_0_number;
	không_thì
	    trả_về &t_func_number;
    }
    nếu (ret_type == &t_string)
    {
	nếu (argcount == 0)
	    trả_về &t_func_0_string;
	không_thì
	    trả_về &t_func_string;
    }

    trả_về alloc_func_type(ret_type, argcount, type_gap);
}

/*
 * For a function type, reserve space for "argcount" argument types (including
 * vararg).
 */
    số_nguyên
func_type_add_arg_types(
	type_T	    *functype,
	số_nguyên	    argcount,
	garray_T    *type_gap)
{
    // To make it easy to free the space needed for the argument types, add the
    // pointer to type_gap.
    nếu (ga_grow(type_gap, 1) == FAIL)
	trả_về FAIL;
    functype->tt_args = ALLOC_CLEAR_MULT(type_T *, argcount);
    nếu (functype->tt_args == NULL)
	trả_về FAIL;
    ((type_T **)type_gap->ga_data)[type_gap->ga_len] =
						     (void *)functype->tt_args;
    ++type_gap->ga_len;
    trả_về OK;
}

/*
 * Return TRUE if "type" is NULL, any or unknown.
 * This also works for const (comparing with &t_any and &t_unknown doesn't).
 */
    số_nguyên
type_any_or_unknown(type_T *type)
{
    trả_về type == NULL || type->tt_type == VAR_ANY
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
	case VVAL_NULL:
	    trả_về &t_null;

	case VVAL_NONE:
	    trả_về &t_none;

	case VVAL_TRUE:
	case VVAL_FALSE:
	    trả_về &t_bool;

	default:
	    trả_về &t_unknown;
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
list_typval2type(typval_T *tv, số_nguyên copyID, garray_T *type_gap, số_nguyên flags)
{
    list_T	*l = tv->vval.v_list;
    listitem_T	*li;
    type_T	*member_type = NULL;

    // An empty list has type list<unknown>, unless the type was specified
    // and is not list<any>.  This matters when assigning to a variable
    // with a specific list type.
    nếu (l == NULL || (l->lv_first == NULL
		&& (l->lv_type == NULL || l->lv_type->tt_member == &t_any)))
	trả_về &t_list_empty;

    nếu ((flags & TVTT_DO_MEMBER) == 0)
	trả_về &t_list_any;

    // If the type is list<any> go through the members, it may end up a
    // more specific type.
    nếu (l->lv_type != NULL && (l->lv_first == NULL
		|| (flags & TVTT_MORE_SPECIFIC) == 0
		|| l->lv_type->tt_member != &t_any))
	// make a copy, lv_type may be freed if the list is freed
	trả_về copy_type_deep(l->lv_type, type_gap);

    nếu (l->lv_first == &range_list_item)
	trả_về &t_list_number;

    nếu (l->lv_copyID == copyID)
	// avoid recursion
	trả_về &t_list_any;

    l->lv_copyID = copyID;

    // Use the common type of all members.
    member_type = typval2type(&l->lv_first->li_tv, copyID, type_gap,
							TVTT_DO_MEMBER);
    cho (li = l->lv_first->li_next; li != NULL; li = li->li_next)
	common_type(typval2type(&li->li_tv, copyID, type_gap, TVTT_DO_MEMBER),
					member_type, &member_type, type_gap);

    // Reset copyID so that a shared reference to this list (not a circular
    // reference) can be processed again to get the correct type.
    l->lv_copyID = 0;

    trả_về get_list_type(member_type, type_gap);
}

/*
 * Get a type_T for a Tuple typval in "tv".
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    static type_T *
tuple_typval2type(typval_T *tv, số_nguyên copyID, garray_T *type_gap, số_nguyên flags)
{
    tuple_T	*tuple = tv->vval.v_tuple;
    số_nguyên		len = tuple_len(tuple);
    type_T	*type = NULL;

    // An empty tuple has type tuple<unknown>, unless the type was specified
    // and is not tuple<any>.  This matters when assigning to a variable
    // with a specific tuple type.
    nếu (tuple == NULL || (len == 0 && (tuple->tv_type == NULL
				|| tuple->tv_type->tt_argcount == 0)))
	trả_về &t_tuple_empty;

    nếu ((flags & TVTT_DO_MEMBER) == 0)
	trả_về &t_tuple_any;

    // If the type is tuple<any> go through the members, it may end up a
    // more specific type.
    nếu (tuple->tv_type != NULL && (len == 0
					|| (flags & TVTT_MORE_SPECIFIC) == 0))
	// make a copy, tv_type may be freed if the tuple is freed
	trả_về copy_type_deep(tuple->tv_type, type_gap);

    nếu (tuple->tv_copyID == copyID)
	// avoid recursion
	trả_về &t_tuple_any;

    tuple->tv_copyID = copyID;

    garray_T	tuple_types_ga;
    ga_init2(&tuple_types_ga, sizeof(type_T *), 10);
    cho (số_nguyên i = 0; i < len; i++)
    {
	type = typval2type(TUPLE_ITEM(tuple, i), copyID, type_gap,
							TVTT_DO_MEMBER);
	nếu (ga_grow(&tuple_types_ga, 1) == FAIL)
	{
	    ga_clear(&tuple_types_ga);
	    // Reset copyID so that a shared reference to this tuple can be
	    // processed again.
	    tuple->tv_copyID = 0;
	    trả_về NULL;
	}
	((type_T **)tuple_types_ga.ga_data)[tuple_types_ga.ga_len] = type;
	tuple_types_ga.ga_len++;
    }

    type_T *tuple_type = get_tuple_type(&tuple_types_ga, type_gap);
    ga_clear(&tuple_types_ga);

    // Reset copyID so that a shared reference to this tuple (not a circular
    // reference) can be processed again to get the correct type.
    tuple->tv_copyID = 0;

    trả_về tuple_type;
}

/*
 * Get a type_T for a Dict typval in "tv".
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    static type_T *
dict_typval2type(typval_T *tv, số_nguyên copyID, garray_T *type_gap, số_nguyên flags)
{
    dict_iterator_T	iter;
    typval_T		*value;
    dict_T		*d = tv->vval.v_dict;
    type_T		*member_type = NULL;

    nếu (d == NULL || (d->dv_hashtab.ht_used == 0 && d->dv_type == NULL))
	trả_về &t_dict_empty;

    nếu ((flags & TVTT_DO_MEMBER) == 0)
	trả_về &t_dict_any;

    // If the type is dict<any> go through the members, it may end up a
    // more specific type.
    nếu (d->dv_type != NULL && (d->dv_hashtab.ht_used == 0
		|| (flags & TVTT_MORE_SPECIFIC) == 0
		|| d->dv_type->tt_member != &t_any))
	trả_về d->dv_type;

    nếu (d->dv_copyID == copyID)
	// avoid recursion
	trả_về &t_dict_any;

    d->dv_copyID = copyID;

    // Use the common type of all values.
    dict_iterate_start(tv, &iter);
    dict_iterate_next(&iter, &value);
    member_type = typval2type(value, copyID, type_gap, TVTT_DO_MEMBER);

    trong_khi (dict_iterate_next(&iter, &value) != NULL)
	common_type(typval2type(value, copyID, type_gap, TVTT_DO_MEMBER),
					member_type, &member_type, type_gap);

    // Reset copyID so that a shared reference to this dict (not a circular
    // reference) can be processed again to get the correct type.
    d->dv_copyID = 0;

    trả_về get_dict_type(member_type, type_gap);
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
    nếu (type == NULL)
	trả_về NULL;

    *type = *ufunc->uf_func_type;
    nếu (type->tt_argcount >= 0 && pt->pt_argc > 0)
    {
	type->tt_argcount -= pt->pt_argc;
	type->tt_min_argcount -= pt->pt_argc;
	nếu (type->tt_argcount > 0 && func_type_add_arg_types(type,
		    type->tt_argcount, type_gap) == OK)
	    cho (số_nguyên i = 0; i < type->tt_argcount; ++i)
		type->tt_args[i] =
		    ufunc->uf_func_type->tt_args[i + pt->pt_argc];
    }
    nếu (pt->pt_func != NULL)
	type->tt_member = pt->pt_func->uf_ret_type;

    trả_về type;
}

/*
 * Get a type_T for a "class" or an "object" typval in "tv".
 */
    static type_T *
oc_typval2type(typval_T *tv)
{
    nếu (tv->v_type == VAR_CLASS)
    {
	nếu (tv->vval.v_class == NULL)
	    trả_về &t_class;

	trả_về &tv->vval.v_class->class_type;
    }

    nếu (tv->vval.v_object != NULL)
	trả_về &tv->vval.v_object->obj_class->class_object_type;

    trả_về &t_object_any;
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
    số_nguyên		argcount = 0;
    số_nguyên		min_argcount = 0;

    nếu (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != NULL)
    {
	nếu (tv->vval.v_partial->pt_func != NULL)
	    ufunc = tv->vval.v_partial->pt_func;
	không_thì
	    name = tv->vval.v_partial->pt_name;
    }
    không_thì
	name = tv->vval.v_string;

    nếu (name == NULL && ufunc == NULL)
	trả_về &t_func_unknown;

    nếu (name != NULL)
    {
	số_nguyên idx = find_internal_func(name);

	nếu (idx >= 0)
	{
	    type_T *decl_type;  // unused

	    internal_func_get_argcount(idx, &argcount, &min_argcount);
	    member_type = internal_func_ret_type(idx, 0, NULL, &decl_type,
		    type_gap);
	}
	không_thì
	{
	    // Check if name contains "<".  If it does, then replace "<" with
	    // NUL and lookup the function and then look up the specific
	    // generic function using the supplied types.
	    char_u *p = generic_func_find_open_bracket(name);
	    nếu (p == NULL)
		ufunc = find_func(name, FALSE);
	    không_thì
	    {
		// generic function
		char_u	cc = *p;

		*p = NUL;
		ufunc = find_func(name, FALSE);
		*p = cc;
		nếu (ufunc != NULL && IS_GENERIC_FUNC(ufunc))
		{
		    ufunc = find_generic_func(ufunc, name, &p);
		    nếu (ufunc == NULL)
			trả_về NULL;
		}
	    }
	}
    }
    nếu (ufunc != NULL)
    {
	// May need to get the argument types from default values by
	// compiling the function.
	nếu (ufunc->uf_def_status == UF_TO_BE_COMPILED
		&& compile_def_function(ufunc, TRUE, CT_NONE, NULL)
		== FAIL)
	    trả_về NULL;
	nếu (ufunc->uf_func_type == NULL)
	    set_function_type(ufunc);
	nếu (ufunc->uf_func_type != NULL)
	{
	    nếu (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != NULL)
		trả_về partial_typval2type(tv, ufunc, type_gap);
	    trả_về ufunc->uf_func_type;
	}
    }

    type = get_type_ptr(type_gap);
    nếu (type == NULL)
	trả_về NULL;
    type->tt_type = tv->v_type;
    type->tt_argcount = argcount;
    type->tt_min_argcount = min_argcount;
    nếu (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != NULL
					&& tv->vval.v_partial->pt_argc > 0)
    {
	type->tt_argcount -= tv->vval.v_partial->pt_argc;
	type->tt_min_argcount -= tv->vval.v_partial->pt_argc;
    }
    type->tt_member = member_type;

    trả_về type;
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
typval2type_int(typval_T *tv, số_nguyên copyID, garray_T *type_gap, số_nguyên flags)
{
    switch (tv->v_type)
    {
	case VAR_UNKNOWN:
	    trả_về &t_unknown;

	case VAR_ANY:
	    trả_về &t_any;

	case VAR_VOID:
	    trả_về &t_void;

	case VAR_BOOL:
	    trả_về &t_bool;

	case VAR_SPECIAL:
	    trả_về special_typval2type(tv);

	case VAR_NUMBER:
	    trả_về &t_number;

	case VAR_FLOAT:
	    trả_về &t_float;

	case VAR_STRING:
	    trả_về &t_string;

	case VAR_BLOB:
	    nếu (tv->vval.v_blob == NULL)
		trả_về &t_blob_null;
	    trả_về &t_blob;

	case VAR_LIST:
	    trả_về list_typval2type(tv, copyID, type_gap, flags);

	case VAR_TUPLE:
	    trả_về tuple_typval2type(tv, copyID, type_gap, flags);

	case VAR_DICT:
	    trả_về dict_typval2type(tv, copyID, type_gap, flags);

	case VAR_JOB:
	    trả_về &t_job;

	case VAR_CHANNEL:
	    trả_về &t_channel;

	case VAR_CLASS:
	case VAR_OBJECT:
	    trả_về oc_typval2type(tv);

	case VAR_TYPEALIAS:
	    trả_về &t_typealias;

	case VAR_FUNC:
	case VAR_PARTIAL:
	    trả_về fp_typval2type(tv, type_gap);

	case VAR_INSTR:
	default:
	    break;
    }

    trả_về NULL;
}

/*
 * Return TRUE if "tv" is not a bool but should be converted to bool.
 */
    số_nguyên
need_convert_to_bool(type_T *type, typval_T *tv)
{
    trả_về type != NULL && type == &t_bool && tv->v_type != VAR_BOOL
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
typval2type(typval_T *tv, số_nguyên copyID, garray_T *type_gap, số_nguyên flags)
{
    type_T *type = typval2type_int(tv, copyID, type_gap, flags);

    nếu (type == NULL)
	trả_về NULL;

    nếu (type != &t_bool && (tv->v_type == VAR_NUMBER
		&& (tv->vval.v_number == 0 || tv->vval.v_number == 1)))
	// Number 0 and 1 and expression with "&&" or "||" can also be used
	// for bool.
	type = &t_number_bool;
    không_thì nếu (type != &t_float && tv->v_type == VAR_NUMBER)
	// A number can also be used for float.
	type = &t_number_float;
    trả_về type;
}

/*
 * Return TRUE if "type" can be used for a variable declaration.
 * Give an error and return FALSE if not.
 */
    số_nguyên
valid_declaration_type(type_T *type)
{
    nếu (type->tt_type == VAR_SPECIAL  // null, none
	    || type->tt_type == VAR_VOID)
    {
	char *tofree = NULL;
	char *name = type_name(type, &tofree);
	semsg(_(e_invalid_type_in_variable_declaration_str), name);
	vim_free(tofree);
	trả_về FALSE;
    }
    trả_về TRUE;
}

/*
 * Get a type_T for a typval_T, used for v: variables.
 * "type_list" is used to temporarily create types in.
 */
    type_T *
typval2type_vimvar(typval_T *tv, garray_T *type_gap)
{
    nếu (tv->v_type == VAR_LIST)  // e.g. for v:oldfiles
	trả_về &t_list_string;
    nếu (tv->v_type == VAR_DICT)  // e.g. for v:event
	trả_về &t_dict_any;
    trả_về typval2type(tv, get_copyID(), type_gap, TVTT_DO_MEMBER);
}

    số_nguyên
check_typval_arg_type(
	type_T	    *expected,
	typval_T    *actual_tv,
	char	    *func_name,
	số_nguyên	    arg_idx)
{
    where_T	where = WHERE_INIT;

    nếu (arg_idx > 0)
    {
	where.wt_index = arg_idx;
	where.wt_kind = WT_ARGUMENT;
    }
    where.wt_func_name = func_name;
    trả_về check_typval_type(expected, actual_tv, where);
}

/*
 * Return true if "expected" is list<any> or dict<any> and "actual_tv" is a
 * list or dict of that kind, which matches whatever its values are.
 */
    static bool
matches_listdict_any(type_T *expected, typval_T *actual_tv)
{
    trả_về (expected->tt_type == VAR_LIST || expected->tt_type == VAR_DICT)
	    && actual_tv->v_type == expected->tt_type
	    && expected->tt_member != NULL
	    && (expected->tt_member->tt_type == VAR_ANY
		|| expected->tt_member->tt_type == VAR_UNKNOWN);
}

/*
 * Return FAIL if "expected" and "actual" don't match.
 * When "argidx" > 0 it is included in the error message.
 */
    số_nguyên
check_typval_type(type_T *expected, typval_T *actual_tv, where_T where)
{
    garray_T	type_list;
    type_T	*actual_type;
    số_nguyên		res = FAIL;

    nếu (expected == NULL)
	trả_về OK;  // didn't expect anything.

    // No need to go through the values of a possibly big list or dict.
    nếu (matches_listdict_any(expected, actual_tv))
	trả_về OK;

    ga_init2(&type_list, sizeof(type_T *), 10);

    // A null_function and null_partial are special cases, they can be used to
    // clear a variable.
    nếu ((actual_tv->v_type == VAR_FUNC && actual_tv->vval.v_string == NULL)
	    || (actual_tv->v_type == VAR_PARTIAL
					 && actual_tv->vval.v_partial == NULL))
	actual_type = &t_func_unknown;
    không_thì
	// When the actual type is list<any> or dict<any> go through the values
	// to possibly get a more specific type.
	actual_type = typval2type(actual_tv, get_copyID(), &type_list,
					  TVTT_DO_MEMBER | TVTT_MORE_SPECIFIC);
    nếu (actual_type != NULL)
    {
	res = check_type_maybe(expected, actual_type, TRUE, where);
	nếu (res == MAYBE && !(actual_type->tt_type == VAR_FUNC
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
    trả_về res;
}

    void
arg_type_mismatch(type_T *expected, type_T *actual, số_nguyên arg_idx)
{
    where_T	where = WHERE_INIT;

    nếu (arg_idx > 0)
    {
	where.wt_index = arg_idx;
	where.wt_kind = WT_ARGUMENT;
    }
    type_mismatch_where(expected, actual, where);
}

    void
type_mismatch_where(type_T *expected, type_T *actual, where_T where)
{
    char *tofree1, *tofree2;
    char *typename1 = type_name(expected, &tofree1);
    char *typename2 = type_name(actual, &tofree2);

    switch (where.wt_kind)
    {
	case WT_MEMBER:
	    semsg(_(e_variable_str_type_mismatch_expected_str_but_got_str),
		    where.wt_func_name, typename1, typename2);
	    break;
	case WT_METHOD:
	case WT_METHOD_ARG:
	case WT_METHOD_RETURN:
	    semsg(_(e_method_str_type_mismatch_expected_str_but_got_str),
		    where.wt_func_name, typename1, typename2);
	    break;
	case WT_VARIABLE:
	    nếu (where.wt_func_name == NULL)
		semsg(_(e_variable_nr_type_mismatch_expected_str_but_got_str),
			where.wt_index, typename1, typename2);
	    không_thì
		semsg(_(e_variable_nr_type_mismatch_expected_str_but_got_str_in_str),
			where.wt_index, typename1, typename2, where.wt_func_name);
	    break;
	case WT_ARGUMENT:
	    nếu (where.wt_func_name == NULL)
		semsg(_(e_argument_nr_type_mismatch_expected_str_but_got_str),
			where.wt_index, typename1, typename2);
	    không_thì
		semsg(_(e_argument_nr_type_mismatch_expected_str_but_got_str_in_str),
			where.wt_index, typename1, typename2, where.wt_func_name);
	    break;
	case WT_CAST:
	case WT_UNKNOWN:
	    nếu (where.wt_func_name == NULL)
		semsg(_(e_type_mismatch_expected_str_but_got_str),
			typename1, typename2);
	    không_thì
		semsg(_(e_type_mismatch_expected_str_but_got_str_in_str),
			typename1, typename2, where.wt_func_name);
	    break;
    }

    vim_free(tofree1);
    vim_free(tofree2);
}

/*
 * Check if the expected and actual types match for a tuple
 */
    static số_nguyên
check_tuple_type_maybe(
    type_T	*expected,
    type_T	*actual,
    where_T	where)
{
    nếu (expected->tt_argcount == -1 || actual->tt_argcount == -1
		|| expected->tt_args == NULL || actual->tt_args == NULL)
	trả_về OK;

    // For a non-variadic tuple, the number of items must match
    nếu (!(expected->tt_flags & TTFLAG_VARARGS)
	    && expected->tt_argcount != actual->tt_argcount)
	trả_về FAIL;

    // compare the type of each tuple item
    cho (số_nguyên i = 0; i < actual->tt_argcount; ++i)
    {
	type_T	*exp_type;
	type_T	*actual_type;

	nếu (expected->tt_flags & TTFLAG_VARARGS)
	{
	    nếu (i < expected->tt_argcount - 1)
		exp_type = expected->tt_args[i];
	    không_thì
		// For a variadic tuple, the last type is a List.  So use the
		// List member type.
		exp_type = expected->tt_args[expected->tt_argcount - 1]->tt_member;
	}
	không_thì
	    exp_type = expected->tt_args[i];

	nếu (actual->tt_flags & TTFLAG_VARARGS)
	{
	    nếu (i < actual->tt_argcount - 1)
		actual_type = actual->tt_args[i];
	    không_thì
		// For a variadic tuple, the last type is a List.  So use the
		// List member type.
		actual_type = actual->tt_args[actual->tt_argcount - 1]->tt_member;
	}
	không_thì
	    actual_type = actual->tt_args[i];

	// Allow for using "any" type for a tuple item
	nếu (actual->tt_args[i] != &t_any && check_type(exp_type, actual_type,
						FALSE, where) == FAIL)
	    trả_về FAIL;
    }

    trả_về OK;
}

/*
 * Check if the expected and actual types match for a function
 * Returns OK if "expected" and "actual" are matching function types.
 * Returns FAIL if "expected" and "actual" are different types.
 * Returns MAYBE when a runtime type check is needed.
 */
    static số_nguyên
check_func_type_maybe(
    type_T	*expected,
    type_T	*actual,
    where_T	where)
{
    số_nguyên ret = OK;

    // If the return type is unknown it can be anything, including
    // nothing, thus there is no point in checking.
    nếu (expected->tt_member != &t_unknown)
    {
	nếu (actual->tt_member != NULL
		&& actual->tt_member != &t_unknown)
	{
	    where_T  func_where = where;

	    func_where.wt_kind = WT_METHOD_RETURN;
	    ret = check_type_maybe(expected->tt_member,
		    actual->tt_member, FALSE,
		    func_where);
	}
	không_thì
	    ret = MAYBE;
    }
    nếu (ret != FAIL
	    && ((expected->tt_flags & TTFLAG_VARARGS)
		!= (actual->tt_flags & TTFLAG_VARARGS))
	    && expected->tt_argcount != -1)
	ret = FAIL;
    nếu (ret != FAIL && expected->tt_argcount != -1
	    && actual->tt_min_argcount != -1
	    && (actual->tt_argcount == -1
		|| (actual->tt_argcount < expected->tt_min_argcount
		    || actual->tt_argcount > expected->tt_argcount)))
	ret = FAIL;
    nếu (ret != FAIL && expected->tt_args != NULL
	    && actual->tt_args != NULL)
    {
	số_nguyên i;

	cho (i = 0; i < expected->tt_argcount
		&& i < actual->tt_argcount; ++i)
	{
	    where_T  func_where = where;
	    func_where.wt_kind = WT_METHOD_ARG;

	    // Allow for using "any" argument type, lambda's have them.
	    nếu (actual->tt_args[i] != &t_any && check_type(
			expected->tt_args[i], actual->tt_args[i], FALSE,
			func_where) == FAIL)
	    {
		ret = FAIL;
		break;
	    }
	}
    }
    nếu (ret == OK && expected->tt_argcount >= 0
	    && actual->tt_argcount == -1)
	// check the argument count at runtime
	ret = MAYBE;

    trả_về ret;
}

/*
 * Check if the expected and actual types match for an object
 * Returns OK if "expected" and "actual" are matching object types.
 * Returns FAIL if "expected" and "actual" are different types.
 * Returns MAYBE when a runtime type check is needed.
 */
    static số_nguyên
check_object_type_maybe(
    type_T	*expected,
    type_T	*actual,
    where_T	where)
{
    số_nguyên ret = OK;

    nếu (actual->tt_type == VAR_ANY)
	trả_về MAYBE;	// use runtime type check
    nếu (actual->tt_type != VAR_OBJECT)
	trả_về FAIL;	// don't use tt_class
    nếu (actual->tt_class == NULL)    // null object
	trả_về OK;
    // t_object_any matches any object except for an enum item
    nếu (expected == &t_object_any && !IS_ENUM(actual->tt_class))
	trả_về OK;

    // For object method arguments, do a invariant type check in
    // an extended class.  For all others, do a covariance type check.
    nếu (where.wt_kind == WT_METHOD_ARG)
    {
	nếu (actual->tt_class != expected->tt_class)
	    ret = FAIL;
    }
    không_thì nếu (!class_instance_of(actual->tt_class, expected->tt_class))
    {
	// Check if this is an up-cast, if so we'll have to check the type at
	// runtime.
	nếu (where.wt_kind == WT_CAST &&
		class_instance_of(expected->tt_class, actual->tt_class))
	    ret = MAYBE;
	không_thì
	    ret = FAIL;
    }

    trả_về ret;
}

/*
 * Check if the expected and actual types match.
 * Does not allow for assigning "any" to a specific type.
 * When "argidx" > 0 it is included in the error message.
 * Return OK if types match.
 * Return FAIL if types do not match.
 */
    số_nguyên
check_type(
	type_T	*expected,
	type_T	*actual,
	số_nguyên	give_msg,
	where_T where)
{
    số_nguyên ret = check_type_maybe(expected, actual, give_msg, where);

    trả_về ret == MAYBE ? OK : ret;
}

/*
 * As check_type() but return MAYBE when a runtime type check should be used
 * when compiling.
 */
    số_nguyên
check_type_maybe(
	type_T	*expected,
	type_T	*actual,
	số_nguyên	give_msg,
	where_T where)
{
    số_nguyên ret = OK;

    // When expected is "unknown" we accept any actual type.
    // When expected is "any" we accept any actual type except "void".
    nếu (expected->tt_type != VAR_UNKNOWN
	    && !(expected->tt_type == VAR_ANY && actual->tt_type != VAR_VOID))

    {
	// tt_type should match, except that a "partial" can be assigned to a
	// variable with type "func".
	// And "unknown" (using global variable) and "any" need a runtime type
	// check.
	nếu (!(expected->tt_type == actual->tt_type
		    || actual->tt_type == VAR_UNKNOWN
		    || actual->tt_type == VAR_ANY
		    || (expected->tt_type == VAR_FUNC
					   && actual->tt_type == VAR_PARTIAL)))
	{
	    nếu (expected->tt_type == VAR_BOOL
					&& (actual->tt_flags & TTFLAG_BOOL_OK))
		// Using number 0 or 1 for bool is OK.
		trả_về OK;
	    nếu (expected->tt_type == VAR_FLOAT
		    && actual->tt_type == VAR_NUMBER
		    && ((expected->tt_flags & TTFLAG_NUMBER_OK)
			     || (actual->tt_flags & TTFLAG_FLOAT_OK)))
		// Using a number where a float is expected is OK here.
		trả_về OK;
	    nếu (expected->tt_type == VAR_LIST
		    && actual->tt_type == VAR_TUPLE
		    && (expected->tt_flags & TTFLAG_TUPLE_OK))
		// Using a tuple where a list is expected is OK here.
		trả_về OK;
	    nếu (give_msg)
		type_mismatch_where(expected, actual, where);
	    trả_về FAIL;
	}
	nếu (expected->tt_type == VAR_DICT || expected->tt_type == VAR_LIST)
	{
	    // "unknown" is used for an empty list or dict
	    nếu (actual->tt_member != NULL && actual->tt_member != &t_unknown)
		ret = check_type_maybe(expected->tt_member, actual->tt_member,
								 FALSE, where);
	}
	không_thì nếu (expected->tt_type == VAR_TUPLE && actual != &t_any)
	    ret =  check_tuple_type_maybe(expected, actual, where);
	không_thì nếu (expected->tt_type == VAR_FUNC && actual != &t_any)
	    ret = check_func_type_maybe(expected, actual, where);
	không_thì nếu (expected->tt_type == VAR_OBJECT)
	    ret = check_object_type_maybe(expected, actual, where);

	nếu (ret == FAIL && give_msg)
	    type_mismatch_where(expected, actual, where);
    }

    nếu (ret == OK && expected->tt_type != VAR_UNKNOWN
	    && expected->tt_type != VAR_ANY
	    && (actual->tt_type == VAR_UNKNOWN || actual->tt_type == VAR_ANY))
	// check the type at runtime
	ret = MAYBE;

    trả_về ret;
}

/*
 * Check that the arguments of "type" match "argvars[argcount]".
 * "base_tv" is from "expr->Func()".
 * Return OK/FAIL.
 */
    số_nguyên
check_argument_types(
	type_T	    *type,
	typval_T    *argvars,
	số_nguyên	    argcount,
	typval_T    *base_tv,
	char_u	    *name)
{
    số_nguyên	    varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;
    số_nguyên	    i;
    số_nguyên	    totcount = argcount + (base_tv == NULL ? 0 : 1);

    nếu (type->tt_type != VAR_FUNC && type->tt_type != VAR_PARTIAL)
	trả_về OK;  // just in case
    nếu (totcount < type->tt_min_argcount - varargs)
    {
	emsg_funcname(e_not_enough_arguments_for_function_str, name);
	trả_về FAIL;
    }
    nếu (!varargs && type->tt_argcount >= 0 && totcount > type->tt_argcount)
    {
	emsg_funcname(e_too_many_arguments_for_function_str, name);
	trả_về FAIL;
    }
    nếu (type->tt_args == NULL)
	trả_về OK;  // cannot check


    cho (i = 0; i < totcount; ++i)
    {
	type_T	    *expected;
	typval_T    *tv;

	nếu (base_tv != NULL)
	{
	    nếu (i == 0)
		tv = base_tv;
	    không_thì
		tv = &argvars[i - 1];
	}
	không_thì
	    tv = &argvars[i];
	nếu (varargs && i >= type->tt_argcount - 1)
	{
	    expected = type->tt_args[type->tt_argcount - 1];
	    nếu (expected != NULL && expected->tt_type == VAR_LIST)
		expected = expected->tt_member;
	    nếu (expected == NULL)
		expected = &t_any;
	}
	không_thì
	    expected = type->tt_args[i];

	// check the type, unless the value is v:none
	nếu ((tv->v_type != VAR_SPECIAL || tv->vval.v_number != VVAL_NONE)
		   && check_typval_arg_type(expected, tv, NULL, i + 1) == FAIL)
	    trả_về FAIL;
    }
    trả_về OK;
}

// Maximum nesting of types, to avoid running out of stack space.  E.g.
// "list<list<number>>" is nested twice.
#define MAX_TYPE_DEPTH	1000

// Current number of levels of parse_type() and the deepest level reached while
// parsing the current type.  The nesting is one less than the number of
// levels.
static số_nguyên	parse_depth = 0;
static số_nguyên	parse_max_depth = 0;

/*
 * Skip over type in list<type>, dict<type> or tuple<type>.
 * Returns a pointer to the character after the type.  "syn_error" is set to
 * TRUE on syntax error.
 */
    static char_u *
skip_member_type(char_u *start, char_u *p, số_nguyên *syn_error)
{
    nếu (STRNCMP("tuple", start, 5) == 0)
    {
	// handle tuple<{type1}, {type2}, ....<type>>
	p = skipwhite(p + 1);
	trong_khi (*p != '>' && *p != NUL)
	{
	    char_u *sp = p;

	    nếu (STRNCMP(p, "...", 3) == 0)
		p += 3;
	    p = skip_type(p, TRUE);
	    nếu (p == sp)
	    {
		*syn_error = TRUE;
		trả_về p;  // syntax error
	    }
	    nếu (*p == ',')
		p = skipwhite(p + 1);
	}
	nếu (*p == '>')
	    p++;
    }
    không_thì
    {
	p = skipwhite(p);
	p = skip_type(skipwhite(p + 1), FALSE);
	p = skipwhite(p);
	nếu (*p == '>')
	    ++p;
    }

    trả_về p;
}

/*
 * Skip over a function type.  Returns a pointer to the character after the
 * type.  "syn_error" is set to TRUE on syntax error.
 */
    static char_u *
skip_func_type(char_u *p, số_nguyên *syn_error)
{
    nếu (*p == '(')
    {
	// handle func(args): type
	++p;
	trong_khi (*p != ')' && *p != NUL)
	{
	    char_u *sp = p;

	    nếu (STRNCMP(p, "...", 3) == 0)
		p += 3;
	    p = skip_type(p, TRUE);
	    nếu (p == sp)
	    {
		*syn_error = TRUE;
		trả_về p;  // syntax error
	    }
	    nếu (*p == ',')
		p = skipwhite(p + 1);
	}
	nếu (*p == ')')
	{
	    nếu (p[1] == ':')
		p = skip_type(skipwhite(p + 2), FALSE);
	    không_thì
		++p;
	}
    }
    không_thì
    {
	// handle func: return_type
	p = skip_type(skipwhite(p + 1), FALSE);
    }

    trả_về p;
}

/*
 * Inner part of skip_type().
 */
    static char_u *
skip_type_inner(char_u *start, số_nguyên optional)
{
    char_u	*p = start;
    số_nguyên		syn_error = FALSE;

    nếu (optional && *p == '?')
	++p;

    // Also skip over "." for imported classes: "import.ClassName".
    trong_khi (ASCII_ISALNUM(*p) || *p == '_' || *p == '.')
	++p;

    // Skip over "<type>"; this is permissive about white space.
    nếu (*skipwhite(p) == '<')
    {
	p = skip_member_type(start, p, &syn_error);
	nếu (syn_error)
	    trả_về p;
    }
    không_thì nếu ((*p == '(' || (*p == ':' && VIM_ISWHITE(p[1])))
					     && STRNCMP("func", start, 4) == 0)
    {
	// skip over function type
	p = skip_func_type(p, &syn_error);
	nếu (syn_error)
	    trả_về p;
    }

    trả_về p;
}

/*
 * Skip over a type definition and return a pointer to just after it.
 * When "optional" is TRUE then a leading "?" is accepted.
 */
    char_u *
skip_type(char_u *start, số_nguyên optional)
{
    static số_nguyên	skip_depth = 0;
    char_u	*p = start;

    // Too deeply nested: skip to the matching ">" without recursing, an
    // error is given when parsing the type.
    nếu (skip_depth > MAX_TYPE_DEPTH)
    {
	số_nguyên	depth = 0;

	cho ( ; *p != NUL; ++p)
	{
	    nếu (*p == '<')
		++depth;
	    không_thì nếu (*p == '>')
	    {
		nếu (depth == 0)
		    break;
		nếu (--depth == 0)
		    trả_về p + 1;
	    }
	    không_thì nếu (depth == 0 && !ASCII_ISALNUM(*p) && *p != '_'
								&& *p != '.')
		break;
	}
	trả_về p;
    }

    ++skip_depth;
    p = skip_type_inner(start, optional);
    --skip_depth;
    trả_về p;
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
	số_nguyên	    give_error,
	char	    *info,
	ufunc_T	    *ufunc,
	cctx_T	    *cctx)
{
    char_u  *arg_start = *arg;
    type_T  *member_type;
    số_nguyên	    prev_called_emsg = called_emsg;

    nếu (**arg != '<')
    {
	nếu (give_error)
	{
	    nếu (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    không_thì
		semsg(_(e_missing_type_after_str), info);
	}
	trả_về NULL;
    }
    *arg = skipwhite(*arg + 1);

    member_type = parse_type(arg, type_gap, ufunc, cctx, give_error);
    nếu (member_type == NULL || !valid_declaration_type(member_type))
	trả_về NULL;

    *arg = skipwhite(*arg);
    nếu (**arg != '>' && called_emsg == prev_called_emsg)
    {
	nếu (give_error)
	    semsg(_(e_missing_gt_after_type_str), arg_start);
	trả_về NULL;
    }
    ++*arg;

    nếu (type->tt_type == VAR_LIST)
	trả_về get_list_type(member_type, type_gap);
    trả_về get_dict_type(member_type, type_gap);
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
    số_nguyên		give_error,
    ufunc_T	*ufunc,
    cctx_T	*cctx)
{
    char_u  *p;
    type_T  *type;
    type_T  *ret_type = &t_unknown;
    số_nguyên	argcount = -1;
    số_nguyên	flags = 0;
    số_nguyên	first_optional = -1;
    type_T	*arg_type[MAX_FUNC_ARGS + 1];

    // func({type}, ...{type}): {type}
    *arg += len;
    nếu (**arg == '(')
    {
	// "func" may or may not return a value, "func()" does
	// not return a value.
	ret_type = &t_void;

	p = ++*arg;
	argcount = 0;
	trong_khi (*p != NUL && *p != ')')
	{
	    nếu (*p == '?')
	    {
		nếu (first_optional == -1)
		    first_optional = argcount;
		++p;
	    }
	    không_thì nếu (STRNCMP(p, "...", 3) == 0)
	    {
		flags |= TTFLAG_VARARGS;
		p += 3;
	    }
	    không_thì nếu (first_optional != -1)
	    {
		nếu (give_error)
		    emsg(_(e_mandatory_argument_after_optional_argument));
		trả_về NULL;
	    }

	    type = parse_type(&p, type_gap, ufunc, cctx, give_error);
	    nếu (type == NULL || !valid_declaration_type(type))
		trả_về NULL;
	    nếu ((flags & TTFLAG_VARARGS) != 0 && type->tt_type != VAR_LIST)
	    {
		char *tofree;
		semsg(_(e_variable_arguments_type_must_be_list_str),
			type_name(type, &tofree));
		vim_free(tofree);
		trả_về NULL;
	    }
	    arg_type[argcount++] = type;

	    // Nothing comes after "...{type}".
	    nếu (flags & TTFLAG_VARARGS)
		break;

	    nếu (*p != ',' && *skipwhite(p) == ',')
	    {
		nếu (give_error)
		    semsg(_(e_no_white_space_allowed_before_str_str),
			    ",", p);
		trả_về NULL;
	    }
	    nếu (*p == ',')
	    {
		++p;
		nếu (!VIM_ISWHITE(*p))
		{
		    nếu (give_error)
			semsg(_(e_white_space_required_after_str_str),
				",", p - 1);
		    trả_về NULL;
		}
	    }
	    p = skipwhite(p);
	    nếu (argcount == MAX_FUNC_ARGS)
	    {
		nếu (give_error)
		    emsg(_(e_too_many_argument_types));
		trả_về NULL;
	    }
	}

	p = skipwhite(p);
	nếu (*p != ')')
	{
	    nếu (give_error)
		emsg(_(e_missing_closing_paren));
	    trả_về NULL;
	}
	*arg = p + 1;
    }
    nếu (**arg == ':')
    {
	// parse return type
	++*arg;
	nếu (!VIM_ISWHITE(**arg) && give_error)
	    semsg(_(e_white_space_required_after_str_str), ":", *arg - 1);
	*arg = skipwhite(*arg);
	ret_type = parse_type(arg, type_gap, ufunc, cctx, give_error);
	nếu (ret_type == NULL)
	    trả_về NULL;
    }
    nếu (flags == 0 && first_optional == -1 && argcount <= 0)
	type = get_func_type(ret_type, argcount, type_gap);
    không_thì
    {
	type = alloc_func_type(ret_type, argcount, type_gap);
	type->tt_flags = flags;
	nếu (argcount > 0)
	{
	    type->tt_argcount = argcount;
	    type->tt_min_argcount = first_optional == -1
						? argcount : first_optional;
	    nếu (func_type_add_arg_types(type, argcount, type_gap) == FAIL)
		trả_về NULL;
	    mch_memmove(type->tt_args, arg_type, sizeof(type_T *) * argcount);
	}
    }

    trả_về type;
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
    số_nguyên		give_error,
    ufunc_T	*ufunc,
    cctx_T	*cctx)
{
    char_u	*p;
    type_T	*type;
    type_T	*ret_type = NULL;
    số_nguyên		typecount = -1;
    số_nguyên		flags = 0;
    garray_T	tuple_types_ga;

    ga_init2(&tuple_types_ga, sizeof(type_T *), 10);

    // tuple<{type}, {type}>
    // tuple<{type}, ...{type}>
    nếu (**arg != '<')
    {
	nếu (give_error)
	{
	    nếu (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    không_thì
		semsg(_(e_missing_type_after_str), "tuple");
	}

	// only "tuple" is specified
	trả_về NULL;
    }

    p = ++*arg;
    typecount = 0;
    trong_khi (*p != NUL && *p != '>')
    {
	nếu (STRNCMP(p, "...", 3) == 0)
	{
	    flags |= TTFLAG_VARARGS;
	    p += 3;
	}

	type = parse_type(&p, type_gap, ufunc, cctx, give_error);
	nếu (type == NULL || !valid_declaration_type(type))
	    goto on_err;

	nếu ((flags & TTFLAG_VARARGS) != 0 && type->tt_type != VAR_LIST)
	{
	    char *tofree;
	    semsg(_(e_variadic_tuple_must_end_with_list_type_str),
						type_name(type, &tofree));
	    vim_free(tofree);
	    goto on_err;
	}

	// Add the item type
	nếu (ga_grow(&tuple_types_ga, 1) == FAIL)
	    goto on_err;
	((type_T **)tuple_types_ga.ga_data)[tuple_types_ga.ga_len] = type;
	tuple_types_ga.ga_len++;
	typecount++;

	// Nothing comes after "...{type}".
	nếu (flags & TTFLAG_VARARGS)
	    break;

	nếu (*p != ',' && *skipwhite(p) == ',')
	{
	    nếu (give_error)
		semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
	    goto on_err;
	}
	nếu (*p == ',')
	{
	    ++p;
	    nếu (!VIM_ISWHITE(*p))
	    {
		nếu (give_error)
		    semsg(_(e_white_space_required_after_str_str),
			    ",", p - 1);
		goto on_err;
	    }
	}
	p = skipwhite(p);
    }

    p = skipwhite(p);
    nếu (*p != '>' || typecount <= 0)
    {
	nếu (give_error)
	    semsg(_(e_missing_type_after_str), p);
	goto on_err;
    }
    *arg = p + 1;

    ret_type = alloc_tuple_type(typecount, type_gap);
    nếu (ret_type == &t_any
	    || tuple_type_add_types(ret_type, typecount, type_gap) == FAIL)
    {
	ret_type = NULL;
	goto on_err;
    }
    ret_type->tt_flags = flags;
    ret_type->tt_argcount = typecount;
    mch_memmove(ret_type->tt_args, tuple_types_ga.ga_data,
						sizeof(type_T *) * typecount);

on_err:
    ga_clear(&tuple_types_ga);

    trả_về ret_type;
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
    số_nguyên		give_error,
    cctx_T	*cctx)
{
    char_u	*arg_start = *arg;
    type_T	*object_type;
    số_nguyên		prev_called_emsg = called_emsg;

    // object<X> or object<any>
    nếu (**arg != '<')
    {
	nếu (give_error)
	{
	    nếu (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    không_thì
		semsg(_(e_missing_type_after_str), "object");
	}

	// only "object" is specified
	trả_về NULL;
    }

    // skip spaces following "object<"
    *arg = skipwhite(*arg + 1);

    object_type = parse_type(arg, type_gap, NULL, cctx, give_error);
    nếu (object_type == NULL)
	trả_về NULL;

    *arg = skipwhite(*arg);
    nếu (**arg != '>' && called_emsg == prev_called_emsg)
    {
	nếu (give_error)
	    semsg(_(e_missing_gt_after_type_str), arg_start);
	trả_về NULL;
    }
    ++*arg;

    nếu (object_type->tt_type == VAR_ANY)
	trả_về &t_object_any;

    nếu (object_type->tt_type != VAR_OBJECT)
    {
	// specified type is not a class
	nếu (give_error)
	    semsg(_(e_class_name_not_found_str), arg_start);
	trả_về NULL;
    }

    trả_về object_type;
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
    số_nguyên		give_error,
    ufunc_T	*ufunc,
    cctx_T	*cctx)
{
    số_nguyên		did_emsg_before = did_emsg;
    typval_T	tv;

    tv.v_type = VAR_UNKNOWN;
    nếu (eval_variable_import(*arg, &tv) == OK)
    {
	nếu (tv.v_type == VAR_CLASS && tv.vval.v_class != NULL)
	{
	    type_T *type = get_type_ptr(type_gap);
	    nếu (type != NULL)
	    {
		// Although the name is that of a class or interface, the type
		// uses will be an object.
		type->tt_type = VAR_OBJECT;
		type->tt_class = tv.vval.v_class;
		clear_tv(&tv);

		*arg += len;
		// Skip over ".ClassName".
		trong_khi (ASCII_ISALNUM(**arg) || **arg == '_' || **arg == '.')
		    ++*arg;

		trả_về type;
	    }
	}
	không_thì nếu (tv.v_type == VAR_TYPEALIAS)
	{
	    // user defined type
	    // The type alias may itself be a nested type, check the resulting
	    // nesting depth.
	    số_nguyên	depth = parse_depth - 1
				+ MAX(tv.vval.v_typealias->ta_depth, 1);
	    nếu (depth - 1 > MAX_TYPE_DEPTH)
	    {
		clear_tv(&tv);
		nếu (give_error)
		    emsg(_(e_type_nested_too_deep));
		trả_về NULL;
	    }
	    nếu (depth > parse_max_depth)
		parse_max_depth = depth;

	    type_T *type = copy_type(tv.vval.v_typealias->ta_type, type_gap);
	    *arg += len;
	    clear_tv(&tv);
	    // Skip over ".TypeName".
	    trong_khi (ASCII_ISALNUM(**arg) || **arg == '_' || **arg == '.')
		++*arg;
	    trả_về type;
	}

	clear_tv(&tv);
    }

    // Check whether it is a generic type
    type_T *type = find_generic_type(*arg, len, ufunc, cctx);
    nếu (type != NULL)
    {
	*arg += len;
	trả_về type;
    }

    nếu (give_error && (did_emsg == did_emsg_before))
    {
	char_u	*p = skip_type(*arg, FALSE);
	char	cc = *p;

	*p = NUL;
	semsg(_(e_type_not_recognized_str), *arg);
	*p = cc;
    }

    trả_về NULL;
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
    số_nguyên		give_error)
{
    char_u  *p = *arg;
    size_t  len;

    // Skip over the first word.
    trong_khi (ASCII_ISALNUM(*p) || *p == '_')
	++p;
    len = p - *arg;

    switch (**arg)
    {
	case 'a':
	    nếu (len == 3 && STRNCMP(*arg, "any", len) == 0)
	    {
		*arg += len;
		trả_về &t_any;
	    }
	    break;
	case 'b':
	    nếu (len == 4 && STRNCMP(*arg, "bool", len) == 0)
	    {
		*arg += len;
		trả_về &t_bool;
	    }
	    nếu (len == 4 && STRNCMP(*arg, "blob", len) == 0)
	    {
		*arg += len;
		trả_về &t_blob;
	    }
	    break;
	case 'c':
	    nếu (len == 7 && STRNCMP(*arg, "channel", len) == 0)
	    {
		*arg += len;
		trả_về &t_channel;
	    }
	    break;
	case 'd':
	    nếu (len == 4 && STRNCMP(*arg, "dict", len) == 0)
	    {
		*arg += len;
		trả_về parse_type_member(arg, &t_dict_any, type_gap,
						give_error, "dict", ufunc,
						cctx);
	    }
	    break;
	case 'f':
	    nếu (len == 5 && STRNCMP(*arg, "float", len) == 0)
	    {
		*arg += len;
		trả_về &t_float;
	    }
	    nếu (len == 4 && STRNCMP(*arg, "func", len) == 0)
		trả_về parse_type_func(arg, len, type_gap, give_error, ufunc,
									cctx);
	    break;
	case 'j':
	    nếu (len == 3 && STRNCMP(*arg, "job", len) == 0)
	    {
		*arg += len;
		trả_về &t_job;
	    }
	    break;
	case 'l':
	    nếu (len == 4 && STRNCMP(*arg, "list", len) == 0)
	    {
		*arg += len;
		trả_về parse_type_member(arg, &t_list_any, type_gap,
						give_error, "list", ufunc,
						cctx);
	    }
	    break;
	case 'n':
	    nếu (len == 6 && STRNCMP(*arg, "number", len) == 0)
	    {
		*arg += len;
		trả_về &t_number;
	    }
	    break;
	case 'o':
	    nếu (len == 6 && STRNCMP(*arg, "object", len) == 0)
	    {
		*arg += len;
		trả_về parse_type_object(arg, type_gap, give_error, cctx);
	    }
	    break;
	case 's':
	    nếu (len == 6 && STRNCMP(*arg, "string", len) == 0)
	    {
		*arg += len;
		trả_về &t_string;
	    }
	    break;
	case 't':
	    nếu (len == 5 && STRNCMP(*arg, "tuple", len) == 0)
	    {
		*arg += len;
		trả_về parse_type_tuple(arg, type_gap, give_error, ufunc,
									cctx);
	    }
	    break;
	case 'v':
	    nếu (len == 4 && STRNCMP(*arg, "void", len) == 0)
	    {
		*arg += len;
		trả_về &t_void;
	    }
	    break;
    }

    // User defined type
    trả_về parse_type_user_defined(arg, len, type_gap, give_error, ufunc,
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
    số_nguyên		give_error)
{
    type_T	*type;

    nếu (parse_depth > MAX_TYPE_DEPTH)
    {
	nếu (give_error)
	    emsg(_(e_type_nested_too_deep));
	trả_về NULL;
    }
    nếu (parse_depth == 0)
	parse_max_depth = 0;
    ++parse_depth;
    nếu (parse_depth > parse_max_depth)
	parse_max_depth = parse_depth;
    type = parse_type_inner(arg, type_gap, ufunc, cctx, give_error);
    --parse_depth;
    trả_về type;
}

/*
 * Return the number of levels of the type last parsed by parse_type().
 */
    số_nguyên
parse_type_depth(void)
{
    trả_về parse_max_depth;
}

/*
 * Check if "type1" and "type2" are exactly the same.
 * "flags" can have ETYPE_ARG_UNKNOWN, which means that an unknown argument
 * type in "type1" is accepted.
 */
    số_nguyên
equal_type(type_T *type1, type_T *type2, số_nguyên flags)
{
    số_nguyên i;

    nếu (type1 == NULL || type2 == NULL)
	trả_về FALSE;
    nếu (type1->tt_type != type2->tt_type)
	trả_về FALSE;
    switch (type1->tt_type)
    {
	case VAR_UNKNOWN:
	case VAR_ANY:
	case VAR_VOID:
	case VAR_SPECIAL:
	case VAR_BOOL:
	case VAR_NUMBER:
	case VAR_FLOAT:
	case VAR_STRING:
	case VAR_BLOB:
	case VAR_JOB:
	case VAR_CHANNEL:
	case VAR_INSTR:
	case VAR_TYPEALIAS:
	    break;  // not composite is always OK
	case VAR_OBJECT:
	case VAR_CLASS:
	    // Objects are considered equal if they are from the same class
	    trả_về type1->tt_class == type2->tt_class;
	case VAR_LIST:
	case VAR_DICT:
	    trả_về equal_type(type1->tt_member, type2->tt_member, flags);
	case VAR_TUPLE:
	    nếu (type1->tt_argcount != type2->tt_argcount)
		trả_về FALSE;
	    nếu (type1->tt_argcount < 0
			   || type1->tt_args == NULL || type2->tt_args == NULL)
		trả_về TRUE;
	    cho (i = 0; i < type1->tt_argcount; ++i)
		nếu ((flags & ETYPE_ARG_UNKNOWN) == 0
			&& !equal_type(type1->tt_args[i], type2->tt_args[i],
									flags))
		    trả_về FALSE;
	    trả_về TRUE;
	case VAR_FUNC:
	case VAR_PARTIAL:
	    nếu (!equal_type(type1->tt_member, type2->tt_member, flags)
		    || type1->tt_argcount != type2->tt_argcount)
		trả_về FALSE;
	    nếu (type1->tt_argcount < 0
			   || type1->tt_args == NULL || type2->tt_args == NULL)
		trả_về TRUE;
	    cho (i = 0; i < type1->tt_argcount; ++i)
		nếu ((flags & ETYPE_ARG_UNKNOWN) == 0
			&& !equal_type(type1->tt_args[i], type2->tt_args[i],
									flags))
		    trả_về FALSE;
	    trả_về TRUE;
    }
    trả_về TRUE;
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
    nếu (type1 == &t_func_unknown)
    {
	*dest = type2;
	trả_về;
    }
    nếu (type2 == &t_func_unknown)
    {
	*dest = type1;
	trả_về;
    }

    common_type(type1->tt_member, type2->tt_member, &common, type_gap);
    nếu (type1->tt_argcount == type2->tt_argcount
	    && type1->tt_argcount >= 0)
    {
	số_nguyên argcount = type1->tt_argcount;
	số_nguyên i;

	*dest = alloc_func_type(common, argcount, type_gap);
	nếu (type1->tt_args != NULL && type2->tt_args != NULL)
	{
	    nếu (func_type_add_arg_types(*dest, argcount,
			type_gap) == OK)
		cho (i = 0; i < argcount; ++i)
		    common_type(type1->tt_args[i], type2->tt_args[i],
			    &(*dest)->tt_args[i], type_gap);
	}
    }
    không_thì
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
    nếu (equal_type(type1, type2, 0))
    {
	*dest = type1;
	trả_về;
    }

    // If either is VAR_UNKNOWN use the other type.  An empty list/dict has no
    // specific type.
    nếu (type1 == NULL || type1->tt_type == VAR_UNKNOWN)
    {
	*dest = type2;
	trả_về;
    }
    nếu (type2 == NULL || type2->tt_type == VAR_UNKNOWN)
    {
	*dest = type1;
	trả_về;
    }

    nếu (type1->tt_type == type2->tt_type)
    {
	nếu (type1->tt_type == VAR_LIST
		|| type1->tt_type == VAR_DICT)
	{
	    type_T *common;

	    common_type(type1->tt_member, type2->tt_member, &common, type_gap);
	    nếu (type1->tt_type == VAR_LIST)
		*dest = get_list_type(common, type_gap);
	    không_thì
		*dest = get_dict_type(common, type_gap);
	    trả_về;
	}
	không_thì nếu (type1->tt_type == VAR_FUNC)
	{
	    common_type_var_func(type1, type2, dest, type_gap);
	    trả_về;
	}
	không_thì nếu (type1->tt_type == VAR_OBJECT)
	{
	    *dest = &t_object_any;
	    trả_về;
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
    nếu (type->tt_type == VAR_TUPLE)
    {
	nếu (type->tt_argcount != 1)
	    trả_về &t_any;

	nếu (type->tt_flags & TTFLAG_VARARGS)
	    trả_về type->tt_args[0]->tt_member;
	không_thì
	    trả_về type->tt_args[0];
    }

    // For "any" the item type is not known.
    trả_về type->tt_member == NULL ? &t_any : type->tt_member;
}

/*
 * Push an entry onto the type stack.  "type" used both for the current type
 * and the declared type.
 * Returns FAIL when out of memory.
 */
    số_nguyên
push_type_stack(cctx_T *cctx, type_T *type)
{
    trả_về push_type_stack2(cctx, type, type);
}

/*
 * Push an entry onto the type stack.  "type" is the current type, "decl_type"
 * is the declared type.
 * Returns FAIL when out of memory.
 */
    số_nguyên
push_type_stack2(cctx_T *cctx, type_T *type, type_T *decl_type)
{
    garray_T	*stack = &cctx->ctx_type_stack;
    type2_T	*typep;

    nếu (GA_GROW_FAILS(stack, 1))
	trả_về FAIL;
    typep = ((type2_T *)stack->ga_data) + stack->ga_len;
    typep->type_curr = type;
    typep->type_decl = decl_type;
    ++stack->ga_len;
    trả_về OK;
}

/*
 * Set the type of the top of the stack to "type".
 */
    void
set_type_on_stack(cctx_T *cctx, type_T *type, số_nguyên offset)
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
get_type_on_stack(cctx_T *cctx, số_nguyên offset)
{
    garray_T	*stack = &cctx->ctx_type_stack;

    nếu (offset + 1 > stack->ga_len)
	trả_về &t_unknown;
    trả_về (((type2_T *)stack->ga_data) + stack->ga_len - offset - 1)
								   ->type_curr;
}

/*
 * Get the declared type from the type stack.  If "offset" is zero the one at
 * the top,
 * if "offset" is one the type above that, etc.
 * Returns &t_unknown if there is no such stack entry.
 */
    type_T *
get_decl_type_on_stack(cctx_T *cctx, số_nguyên offset)
{
    garray_T	*stack = &cctx->ctx_type_stack;

    nếu (offset + 1 > stack->ga_len)
	trả_về &t_unknown;
    trả_về (((type2_T *)stack->ga_data) + stack->ga_len - offset - 1)
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
	số_nguyên	    count,
	số_nguyên	    skip,
	cctx_T	    *cctx)
{
    garray_T	*stack = &cctx->ctx_type_stack;
    type2_T	*typep;
    garray_T    *type_gap = cctx->ctx_type_list;
    số_nguyên		i;
    type_T	*result;
    type_T	*type;

    // Use "unknown" for an empty list or dict.
    nếu (count == 0)
	trả_về &t_unknown;
    // Find the common type from following items.
    typep = ((type2_T *)stack->ga_data) + stack->ga_len;
    result = &t_unknown;
    cho (i = 0; i < count; ++i)
    {
	type = (typep -((count - i) * skip) + skip - 1)->type_curr;
	nếu (check_type_is_value(type) == FAIL)
	    trả_về NULL;
	nếu (result != &t_any)
	    common_type(type, result, &result, type_gap);
    }

    trả_về result;
}

/*
 * Get the types of items in a tuple on the stack of "cctx".
 * Returns the number of types.  Returns -1 on failure.
 */
    số_nguyên
get_tuple_type_from_stack(
    số_nguyên		count,
    garray_T	*tuple_types_gap,
    cctx_T	*cctx)
{
    garray_T	*stack = &cctx->ctx_type_stack;
    type2_T	*typep;
    type_T	*type = NULL;

    // Use "unknown" for an empty tuple
    nếu (count == 0)
	trả_về 0;

    // Find the common type from following items.
    typep = ((type2_T *)stack->ga_data) + stack->ga_len;
    cho (số_nguyên i = 0; i < count; i++)
    {
	type = (typep - (count - i))->type_curr;
	nếu (check_type_is_value(type) == FAIL)
	    trả_về -1;
	nếu (ga_grow(tuple_types_gap, 1) == FAIL)
	    trả_về -1;
	((type_T **)tuple_types_gap->ga_data)[tuple_types_gap->ga_len] = type;
	tuple_types_gap->ga_len++;
    }

    trả_về tuple_types_gap->ga_len;
}

    char *
vartype_name(vartype_T type)
{
    switch (type)
    {
	case VAR_UNKNOWN: break;
	case VAR_ANY: trả_về "any";
	case VAR_VOID: trả_về "void";
	case VAR_SPECIAL: trả_về "special";
	case VAR_BOOL: trả_về "bool";
	case VAR_NUMBER: trả_về "number";
	case VAR_FLOAT: trả_về "float";
	case VAR_STRING: trả_về "string";
	case VAR_BLOB: trả_về "blob";
	case VAR_JOB: trả_về "job";
	case VAR_CHANNEL: trả_về "channel";
	case VAR_LIST: trả_về "list";
	case VAR_TUPLE: trả_về "tuple";
	case VAR_DICT: trả_về "dict";
	case VAR_INSTR: trả_về "instr";
	case VAR_CLASS: trả_về "class";
	case VAR_OBJECT: trả_về "object";
	case VAR_TYPEALIAS: trả_về "typealias";

	case VAR_FUNC:
	case VAR_PARTIAL: trả_về "func";
    }
    trả_về "unknown";
}

/*
 * Return the type name of a List (list<type>) or Dict (dict<type>).
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    static char *
type_name_list_or_dict(char *name, type_T *type, char **tofree)
{
    char *member_free;
    char *member_name;

    nếu (type->tt_member->tt_type == VAR_UNKNOWN)
	member_name = type_name(&t_any, &member_free);
    không_thì
	member_name = type_name(type->tt_member, &member_free);

    size_t len = STRLEN(name) + STRLEN(member_name) + 3;
    *tofree = alloc(len);
    nếu (*tofree == NULL)
    {
	vim_free(member_free);
	trả_về name;
    }

    vim_snprintf(*tofree, len, "%s<%s>", name, member_name);
    vim_free(member_free);
    trả_về *tofree;
}

/*
 * Return the type name of a tuple.
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    static char *
type_name_tuple(type_T *type, char **tofree)
{
    garray_T    ga;
    số_nguyên		i;
    số_nguyên		varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;
    char	*arg_free = NULL;

    ga_init2(&ga, 1, 100);
    nếu (ga_grow(&ga, 20) == FAIL)
	goto failed;
    STRCPY(ga.ga_data, "tuple<");
    ga.ga_len += 6;

    nếu (type->tt_argcount <= 0)
	// empty tuple
	GA_CONCAT_LITERAL(&ga, "any");
    không_thì
    {
	nếu (type->tt_args == NULL)
	    GA_CONCAT_LITERAL(&ga, "[unknown]");
	không_thì
	{
	    cho (i = 0; i < type->tt_argcount; ++i)
	    {
		string_T    arg_type;

		arg_type.string = (char_u *)type_name(type->tt_args[i], &arg_free);
		nếu (i > 0)
		{
		    STRCPY((char *)ga.ga_data + ga.ga_len, ", ");
		    ga.ga_len += 2;
		}
		arg_type.length = STRLEN(arg_type.string);
		nếu (ga_grow(&ga, (số_nguyên)arg_type.length + 8) == FAIL)
		    goto failed;
		nếu (varargs && i == type->tt_argcount - 1)
		    GA_CONCAT_LITERAL(&ga, "...");
		ga_concat_len(&ga, arg_type.string, arg_type.length);
		VIM_CLEAR(arg_free);
	    }
	}
    }

    STRCPY((char *)ga.ga_data + ga.ga_len, ">");
    *tofree = ga.ga_data;
    trả_về ga.ga_data;

failed:
    vim_free(arg_free);
    ga_clear(&ga);
    trả_về "[unknown]";
}

/*
 * Return the type name of a Class (class<name>) or Object (object<name>).
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    static char *
type_name_class_or_obj(char *name, type_T *type, char **tofree)
{
    string_T	class_name;

    nếu (type->tt_class != NULL)
    {
	class_name.string = type->tt_class->class_name.string;
	class_name.length = type->tt_class->class_name.length;
	nếu (IS_ENUM(type->tt_class))
	    name = "enum";
    }
    không_thì
	STR_LITERAL_SET(class_name, "any");

    size_t len = STRLEN(name) + class_name.length + 3;
    *tofree = alloc(len);
    nếu (*tofree == NULL)
	trả_về name;

    vim_snprintf(*tofree, len, "%s<%s>", name, class_name.string);
    trả_về *tofree;
}

/*
 * Return the type name of a function.
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    static char *
type_name_func(type_T *type, char **tofree)
{
    garray_T    ga;
    số_nguyên		i;
    số_nguyên		varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;
    char	*arg_free = NULL;

    ga_init2(&ga, 1, 100);
    nếu (ga_grow(&ga, 20) == FAIL)
	goto failed;
    STRCPY(ga.ga_data, "func(");
    ga.ga_len += 5;

    cho (i = 0; i < type->tt_argcount; ++i)
    {
	string_T    arg_type;

	nếu (type->tt_args == NULL)
	    STR_LITERAL_SET(arg_type, "[unknown]");
	không_thì
	{
	    arg_type.string = (char_u *)type_name(type->tt_args[i], &arg_free);
	    arg_type.length = STRLEN(arg_type.string);
	}
	nếu (i > 0)
	{
	    STRCPY((char *)ga.ga_data + ga.ga_len, ", ");
	    ga.ga_len += 2;
	}
	nếu (ga_grow(&ga, (số_nguyên)arg_type.length + 8) == FAIL)
	    goto failed;
	nếu (varargs && i == type->tt_argcount - 1)
	    GA_CONCAT_LITERAL(&ga, "...");
	không_thì nếu (i >= type->tt_min_argcount)
	    *((char *)ga.ga_data + ga.ga_len++) = '?';
	ga_concat_len(&ga, arg_type.string, arg_type.length);
	VIM_CLEAR(arg_free);
    }
    nếu (type->tt_argcount < 0)
	// any number of arguments
	GA_CONCAT_LITERAL(&ga, "...");

    nếu (type->tt_member == &t_void)
	STRCPY((char *)ga.ga_data + ga.ga_len, ")");
    không_thì
    {
	char *ret_free = NULL;
	char *ret_name = type_name(type->tt_member, &ret_free);
	số_nguyên  len;

	len = (số_nguyên)STRLEN(ret_name) + 4;
	nếu (ga_grow(&ga, len) == FAIL)
	{
	    vim_free(ret_free);
	    goto failed;
	}
	STRCPY((char *)ga.ga_data + ga.ga_len, "): ");
	STRCPY((char *)ga.ga_data + ga.ga_len + 3, ret_name);
	vim_free(ret_free);
    }
    *tofree = ga.ga_data;
    trả_về ga.ga_data;

failed:
    vim_free(arg_free);
    ga_clear(&ga);
    trả_về "[unknown]";
}

/*
 * Return the name of a type.
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    char *
type_name(type_T *type, char **tofree)
{
    char *name;

    *tofree = NULL;
    nếu (type == NULL)
	trả_về "[unknown]";
    name = vartype_name(type->tt_type);

    switch (type->tt_type)
    {
	case VAR_LIST:
	case VAR_DICT:
	    trả_về type_name_list_or_dict(name, type, tofree);

	case VAR_TUPLE:
	    trả_về type_name_tuple(type, tofree);

	case VAR_CLASS:
	case VAR_OBJECT:
	    trả_về type_name_class_or_obj(name, type, tofree);

	case VAR_FUNC:
	    trả_về type_name_func(type, tofree);

	default:
	    break;
    }

    trả_về name;
}

/*
 * "typename(expr)" function
 */
    void
f_typename(typval_T *argvars, typval_T *rettv)
{
    garray_T	type_list;
    type_T	*type;
    char	*tofree;
    char	*name;

    rettv->v_type = VAR_STRING;
    ga_init2(&type_list, sizeof(type_T *), 10);
    nếu (argvars[0].v_type == VAR_TYPEALIAS)
    {
	type = copy_type(argvars[0].vval.v_typealias->ta_type, &type_list);
	// A type alias for a class has the type set to VAR_OBJECT.  Change it
	// to VAR_CLASS, so that the name is "typealias<class<xxx>>"
	nếu (type->tt_type == VAR_OBJECT)
	    type->tt_type = VAR_CLASS;
    }
    không_thì
	type = typval2type(argvars, get_copyID(), &type_list, TVTT_DO_MEMBER);
    name = type_name(type, &tofree);
    nếu (argvars[0].v_type == VAR_TYPEALIAS)
    {
	vim_snprintf((char *)IObuff, IOSIZE, "typealias<%s>", name);
	rettv->vval.v_string = vim_strsave((char_u *)IObuff);
	nếu (tofree != NULL)
	    vim_free(tofree);
    }
    không_thì
    {
	nếu (tofree != NULL)
	    rettv->vval.v_string = (char_u *)tofree;
	không_thì
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
    số_nguyên
check_typval_is_value(typval_T *tv)
{
    nếu (tv == NULL)
	trả_về OK;

    switch (tv->v_type)
    {
	case VAR_CLASS:
	    {
		class_T *cl = tv->vval.v_class;
		char_u *class_name = (cl == NULL) ? (char_u *)""
							: cl->class_name.string;
		nếu (cl != NULL && IS_ENUM(cl))
		    semsg(_(e_using_enum_as_value_str), class_name);
		không_thì
		    semsg(_(e_using_class_as_value_str), class_name);
	    }
	    trả_về FAIL;

	case VAR_TYPEALIAS:
	    semsg(_(e_using_typealias_as_value_str),
		    tv->vval.v_typealias->ta_name);
	    trả_về FAIL;

	default:
	    break;
    }
    trả_về OK;
}

/*
 * Same as above, except check type_T.
 */
    số_nguyên
check_type_is_value(type_T *type)
{
    nếu (type == NULL)
	trả_về OK;
    switch (type->tt_type)
    {
	case VAR_CLASS:
	    nếu (type->tt_class != NULL && IS_ENUM(type->tt_class))
		semsg(_(e_using_enum_as_value_str),
			type->tt_class->class_name.string);
	    không_thì
		semsg(_(e_using_class_as_value_str),
			type->tt_class == NULL ? (char_u *)""
			: type->tt_class->class_name.string);
	    trả_về FAIL;

	case VAR_TYPEALIAS:
	    // TODO: Not sure what could be done here to get a name.
	    //       Maybe an optional argument?
	    emsg(_(e_using_typealias_as_var_val));
	    trả_về FAIL;

	default:
	    break;
    }
    trả_về OK;
}

#endif // FEAT_EVAL
