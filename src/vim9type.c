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

#ако defined(FEAT_EVAL)

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

    ако (ga_grow(type_gap, 1) == FAIL)
	врати NULL;
    type = ALLOC_CLEAR_ONE(type_T);
    ако (type == NULL)
	врати NULL;

    ((type_T **)type_gap->ga_data)[type_gap->ga_len] = type;
    ++type_gap->ga_len;
    врати type;
}

/*
 * Make a shallow copy of "type".
 * When allocation fails returns "type".
 */
    type_T *
copy_type(type_T *type, garray_T *type_gap)
{
    type_T *copy = get_type_ptr(type_gap);

    ако (copy == NULL)
	врати type;
    *copy = *type;
    copy->tt_flags &= ~TTFLAG_STATIC;

    ако (type->tt_args != NULL
	   && func_type_add_arg_types(copy, type->tt_argcount, type_gap) == OK)
	за (цео i = 0; i < type->tt_argcount; ++i)
	    copy->tt_args[i] = type->tt_args[i];

    врати copy;
}

/*
 * Inner part of copy_type_deep().
 * When allocation fails returns "type".
 */
    static type_T *
copy_type_deep_rec(type_T *type, garray_T *type_gap, garray_T *seen_types)
{
    за (цео i = 0; i < seen_types->ga_len; ++i)
	ако (((type_T **)seen_types->ga_data)[i * 2] == type)
	    // seen this type before, return the copy we made
	    врати ((type_T **)seen_types->ga_data)[i * 2 + 1];

    type_T *copy = copy_type(type, type_gap);
    ако (ga_grow(seen_types, 1) == FAIL)
	врати copy;
    ((type_T **)seen_types->ga_data)[seen_types->ga_len * 2] = type;
    ((type_T **)seen_types->ga_data)[seen_types->ga_len * 2 + 1] = copy;
    ++seen_types->ga_len;

    ако (copy->tt_member != NULL)
	copy->tt_member = copy_type_deep_rec(copy->tt_member,
							 type_gap, seen_types);
    ако (type->tt_args != NULL)
	за (цео i = 0; i < type->tt_argcount; ++i)
	    copy->tt_args[i] = copy_type_deep_rec(copy->tt_args[i],
							 type_gap, seen_types);

    врати copy;
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
    врати res;
}

    void
clear_type_list(garray_T *gap)
{
    док (gap->ga_len > 0)
	vim_free(((type_T **)gap->ga_data)[--gap->ga_len]);
    ga_clear(gap);
}

    void
clear_func_type_list(garray_T *gap, type_T **func_type)
{
    док (gap->ga_len > 0)
    {
	// func_type pointing to the uf_type_list, so reset pointer
	ако (*func_type == ((type_T **)gap->ga_data)[--gap->ga_len])
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

    ако (type == NULL)
	врати NULL;

    // A fixed type never contains allocated types, return as-is.
    ако (type->tt_flags & TTFLAG_STATIC)
	врати type;

    ret = ALLOC_ONE(type_T);
    ако (ret == NULL)
	врати NULL;
    *ret = *type;

    ако (ret->tt_member != NULL)
	ret->tt_member = alloc_type(ret->tt_member);

    ако (type->tt_argcount > 0 && type->tt_args != NULL)
    {
	цео i;

	ret->tt_args = ALLOC_MULT(type_T *, type->tt_argcount);
	ако (ret->tt_args != NULL)
	    за (i = 0; i < type->tt_argcount; ++i)
		ret->tt_args[i] = alloc_type(type->tt_args[i]);
    }
    инако
	ret->tt_args = NULL;

    врати ret;
}

/*
 * Free a type that was created with alloc_type().
 */
    void
free_type(type_T *type)
{
    цео i;

    ако (type == NULL || (type->tt_flags & TTFLAG_STATIC))
	врати;
    ако (type->tt_args != NULL)
    {
	за (i = 0; i < type->tt_argcount; ++i)
	    free_type(type->tt_args[i]);
	vim_free(type->tt_args);
    }

    free_type(type->tt_member);

    vim_free(type);
}

/*
 * Return TRUE if "type" is to be recursed into for setting the type.
 */
    static цео
set_tv_type_recurse(type_T *type)
{
    врати type->tt_member != NULL
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
    ако (d->dv_type == type)
	врати;

    free_type(d->dv_type);
    d->dv_type = alloc_type(type);

    // Need to recursively set the type of dict items?
    ако (!set_tv_type_recurse(type))
	врати;

    цео		todo = (цео)d->dv_hashtab.ht_used;
    hashitem_T	*hi;
    dictitem_T	*di;

    FOR_ALL_HASHTAB_ITEMS(&d->dv_hashtab, hi, todo)
    {
	ако (!HASHITEM_EMPTY(hi))
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
    ако (l->lv_type == type)
	врати;

    free_type(l->lv_type);
    l->lv_type = alloc_type(type);

    // Need to recursively set the type of list items?
    ако (l->lv_first == &range_list_item || !set_tv_type_recurse(type))
	врати;

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
    ако (tuple->tv_type == type)
	врати;

    free_type(tuple->tv_type);
    tuple->tv_type = alloc_type(type);

    ако (type->tt_argcount <= 0)
	врати;

    // recursively set the type of list items
    type_T	*item_type;
    за (цео i = 0; i < tuple_len(tuple); i++)
    {
	ако ((type->tt_flags & TTFLAG_VARARGS) && (i >= type->tt_argcount - 1))
	    // For a variadic tuple, the last type is a List.  So use the
	    // List member type.
	    item_type = type->tt_args[type->tt_argcount - 1]->tt_member;
	инако
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
    ако (type->tt_type == VAR_ANY)
	// If the variable type is "any", then keep the value type.
	// e.g.  var x: any = [1, 2] or var y: any = {v: 1}
	врати;

    ако (tv->v_type == VAR_DICT && tv->vval.v_dict != NULL)
	set_tv_type_dict(tv->vval.v_dict, type);
    инако ако (tv->v_type == VAR_LIST && tv->vval.v_list != NULL)
	set_tv_type_list(tv->vval.v_list, type);
    инако ако (tv->v_type == VAR_TUPLE && tv->vval.v_tuple != NULL)
	set_tv_type_tuple(tv->vval.v_tuple, type);
}

/*
 * For a tuple type, reserve space for "typecount" types (including the
 * repeated type).
 */
    static цео
tuple_type_add_types(
    type_T	*tupletype,
    цео		typecount,
    garray_T	*type_gap)
{
    // To make it easy to free the space needed for the types, add the
    // pointer to type_gap.
    ако (ga_grow(type_gap, 1) == FAIL)
	врати FAIL;
    tupletype->tt_args = alloc_clear_id(sizeof(type_T *) * typecount,
							  aid_tuple_type_args);
    ако (tupletype->tt_args == NULL)
	врати FAIL;
    ((type_T **)type_gap->ga_data)[type_gap->ga_len] =
						(void *)tupletype->tt_args;
    ++type_gap->ga_len;
    врати OK;
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
    ако (member_type == NULL || (member_type->tt_type == VAR_ANY
					&& !IS_GENERIC_TYPE(member_type)))
	врати &t_list_any;
    ако (member_type->tt_type == VAR_VOID
	    || member_type->tt_type == VAR_UNKNOWN)
	врати &t_list_empty;
    ако (member_type->tt_type == VAR_BOOL)
	врати &t_list_bool;
    ако (member_type->tt_type == VAR_NUMBER)
	врати &t_list_number;
    ако (member_type->tt_type == VAR_STRING)
	врати &t_list_string;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    ако (type == NULL)
	врати &t_any;
    type->tt_type = VAR_LIST;
    type->tt_member = member_type;
    type->tt_argcount = 0;
    type->tt_args = NULL;
    врати type;
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
    цео		typecount = tuple_types_gap->ga_len;

    // recognize commonly used types
    ако (typecount == 0)
	врати &t_tuple_any;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    ако (type == NULL)
	врати &t_any;
    type->tt_type = VAR_TUPLE;
    type->tt_member = NULL;
    ако (typecount > 0)
    {
	ако (tuple_type_add_types(type, typecount, type_gap) == FAIL)
	    врати &t_any;
	mch_memmove(type->tt_args, tuple_types, sizeof(type_T *) * typecount);
    }
    type->tt_argcount = typecount;
    type->tt_flags = 0;

    врати type;
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
    ако (member_type == NULL || (member_type->tt_type == VAR_ANY
					&& !IS_GENERIC_TYPE(member_type)))
	врати &t_dict_any;
    ако (member_type->tt_type == VAR_VOID
	    || member_type->tt_type == VAR_UNKNOWN)
	врати &t_dict_empty;
    ако (member_type->tt_type == VAR_BOOL)
	врати &t_dict_bool;
    ако (member_type->tt_type == VAR_NUMBER)
	врати &t_dict_number;
    ако (member_type->tt_type == VAR_STRING)
	врати &t_dict_string;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    ако (type == NULL)
	врати &t_any;
    type->tt_type = VAR_DICT;
    type->tt_member = member_type;
    type->tt_argcount = 0;
    type->tt_args = NULL;
    врати type;
}

/*
 * Allocate a new type for a function.
 */
    type_T *
alloc_func_type(type_T *ret_type, цео argcount, garray_T *type_gap)
{
    type_T *type = get_type_ptr(type_gap);

    ако (type == NULL)
	врати &t_any;
    type->tt_type = VAR_FUNC;
    type->tt_member = ret_type == NULL ? &t_unknown : ret_type;
    type->tt_argcount = argcount;
    type->tt_args = NULL;
    врати type;
}

/*
 * Allocate a new type for a tuple.
 */
    static type_T *
alloc_tuple_type(цео typecount, garray_T *type_gap)
{
    type_T *type = get_type_ptr(type_gap);

    ако (type == NULL)
	врати &t_any;
    type->tt_type = VAR_TUPLE;
    type->tt_member = NULL;
    type->tt_argcount = typecount;
    type->tt_args = NULL;
    врати type;
}

/*
 * Get a function type, based on the return type "ret_type".
 * "argcount" must be -1 or 0, a predefined type can be used.
 */
    type_T *
get_func_type(type_T *ret_type, цео argcount, garray_T *type_gap)
{
    // recognize commonly used types
    ако (ret_type == &t_unknown || ret_type == NULL)
    {
	// (argcount == 0) is not possible
	врати &t_func_unknown;
    }
    ако (ret_type == &t_void)
    {
	ако (argcount == 0)
	    врати &t_func_0_void;
	инако
	    врати &t_func_void;
    }
    ако (ret_type == &t_any)
    {
	ако (argcount == 0)
	    врати &t_func_0_any;
	инако
	    врати &t_func_any;
    }
    ако (ret_type == &t_number)
    {
	ако (argcount == 0)
	    врати &t_func_0_number;
	инако
	    врати &t_func_number;
    }
    ако (ret_type == &t_string)
    {
	ако (argcount == 0)
	    врати &t_func_0_string;
	инако
	    врати &t_func_string;
    }

    врати alloc_func_type(ret_type, argcount, type_gap);
}

/*
 * For a function type, reserve space for "argcount" argument types (including
 * vararg).
 */
    цео
func_type_add_arg_types(
	type_T	    *functype,
	цео	    argcount,
	garray_T    *type_gap)
{
    // To make it easy to free the space needed for the argument types, add the
    // pointer to type_gap.
    ако (ga_grow(type_gap, 1) == FAIL)
	врати FAIL;
    functype->tt_args = ALLOC_CLEAR_MULT(type_T *, argcount);
    ако (functype->tt_args == NULL)
	врати FAIL;
    ((type_T **)type_gap->ga_data)[type_gap->ga_len] =
						     (void *)functype->tt_args;
    ++type_gap->ga_len;
    врати OK;
}

/*
 * Return TRUE if "type" is NULL, any or unknown.
 * This also works for const (comparing with &t_any and &t_unknown doesn't).
 */
    цео
type_any_or_unknown(type_T *type)
{
    врати type == NULL || type->tt_type == VAR_ANY
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
	    врати &t_null;

	case VVAL_NONE:
	    врати &t_none;

	case VVAL_TRUE:
	case VVAL_FALSE:
	    врати &t_bool;

	default:
	    врати &t_unknown;
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
list_typval2type(typval_T *tv, цео copyID, garray_T *type_gap, цео flags)
{
    list_T	*l = tv->vval.v_list;
    listitem_T	*li;
    type_T	*member_type = NULL;

    // An empty list has type list<unknown>, unless the type was specified
    // and is not list<any>.  This matters when assigning to a variable
    // with a specific list type.
    ако (l == NULL || (l->lv_first == NULL
		&& (l->lv_type == NULL || l->lv_type->tt_member == &t_any)))
	врати &t_list_empty;

    ако ((flags & TVTT_DO_MEMBER) == 0)
	врати &t_list_any;

    // If the type is list<any> go through the members, it may end up a
    // more specific type.
    ако (l->lv_type != NULL && (l->lv_first == NULL
		|| (flags & TVTT_MORE_SPECIFIC) == 0
		|| l->lv_type->tt_member != &t_any))
	// make a copy, lv_type may be freed if the list is freed
	врати copy_type_deep(l->lv_type, type_gap);

    ако (l->lv_first == &range_list_item)
	врати &t_list_number;

    ако (l->lv_copyID == copyID)
	// avoid recursion
	врати &t_list_any;

    l->lv_copyID = copyID;

    // Use the common type of all members.
    member_type = typval2type(&l->lv_first->li_tv, copyID, type_gap,
							TVTT_DO_MEMBER);
    за (li = l->lv_first->li_next; li != NULL; li = li->li_next)
	common_type(typval2type(&li->li_tv, copyID, type_gap, TVTT_DO_MEMBER),
					member_type, &member_type, type_gap);

    // Reset copyID so that a shared reference to this list (not a circular
    // reference) can be processed again to get the correct type.
    l->lv_copyID = 0;

    врати get_list_type(member_type, type_gap);
}

/*
 * Get a type_T for a Tuple typval in "tv".
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    static type_T *
tuple_typval2type(typval_T *tv, цео copyID, garray_T *type_gap, цео flags)
{
    tuple_T	*tuple = tv->vval.v_tuple;
    цео		len = tuple_len(tuple);
    type_T	*type = NULL;

    // An empty tuple has type tuple<unknown>, unless the type was specified
    // and is not tuple<any>.  This matters when assigning to a variable
    // with a specific tuple type.
    ако (tuple == NULL || (len == 0 && (tuple->tv_type == NULL
				|| tuple->tv_type->tt_argcount == 0)))
	врати &t_tuple_empty;

    ако ((flags & TVTT_DO_MEMBER) == 0)
	врати &t_tuple_any;

    // If the type is tuple<any> go through the members, it may end up a
    // more specific type.
    ако (tuple->tv_type != NULL && (len == 0
					|| (flags & TVTT_MORE_SPECIFIC) == 0))
	// make a copy, tv_type may be freed if the tuple is freed
	врати copy_type_deep(tuple->tv_type, type_gap);

    ако (tuple->tv_copyID == copyID)
	// avoid recursion
	врати &t_tuple_any;

    tuple->tv_copyID = copyID;

    garray_T	tuple_types_ga;
    ga_init2(&tuple_types_ga, sizeof(type_T *), 10);
    за (цео i = 0; i < len; i++)
    {
	type = typval2type(TUPLE_ITEM(tuple, i), copyID, type_gap,
							TVTT_DO_MEMBER);
	ако (ga_grow(&tuple_types_ga, 1) == FAIL)
	{
	    ga_clear(&tuple_types_ga);
	    // Reset copyID so that a shared reference to this tuple can be
	    // processed again.
	    tuple->tv_copyID = 0;
	    врати NULL;
	}
	((type_T **)tuple_types_ga.ga_data)[tuple_types_ga.ga_len] = type;
	tuple_types_ga.ga_len++;
    }

    type_T *tuple_type = get_tuple_type(&tuple_types_ga, type_gap);
    ga_clear(&tuple_types_ga);

    // Reset copyID so that a shared reference to this tuple (not a circular
    // reference) can be processed again to get the correct type.
    tuple->tv_copyID = 0;

    врати tuple_type;
}

/*
 * Get a type_T for a Dict typval in "tv".
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    static type_T *
dict_typval2type(typval_T *tv, цео copyID, garray_T *type_gap, цео flags)
{
    dict_iterator_T	iter;
    typval_T		*value;
    dict_T		*d = tv->vval.v_dict;
    type_T		*member_type = NULL;

    ако (d == NULL || (d->dv_hashtab.ht_used == 0 && d->dv_type == NULL))
	врати &t_dict_empty;

    ако ((flags & TVTT_DO_MEMBER) == 0)
	врати &t_dict_any;

    // If the type is dict<any> go through the members, it may end up a
    // more specific type.
    ако (d->dv_type != NULL && (d->dv_hashtab.ht_used == 0
		|| (flags & TVTT_MORE_SPECIFIC) == 0
		|| d->dv_type->tt_member != &t_any))
	врати d->dv_type;

    ако (d->dv_copyID == copyID)
	// avoid recursion
	врати &t_dict_any;

    d->dv_copyID = copyID;

    // Use the common type of all values.
    dict_iterate_start(tv, &iter);
    dict_iterate_next(&iter, &value);
    member_type = typval2type(value, copyID, type_gap, TVTT_DO_MEMBER);

    док (dict_iterate_next(&iter, &value) != NULL)
	common_type(typval2type(value, copyID, type_gap, TVTT_DO_MEMBER),
					member_type, &member_type, type_gap);

    // Reset copyID so that a shared reference to this dict (not a circular
    // reference) can be processed again to get the correct type.
    d->dv_copyID = 0;

    врати get_dict_type(member_type, type_gap);
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
    ако (type == NULL)
	врати NULL;

    *type = *ufunc->uf_func_type;
    ако (type->tt_argcount >= 0 && pt->pt_argc > 0)
    {
	type->tt_argcount -= pt->pt_argc;
	type->tt_min_argcount -= pt->pt_argc;
	ако (type->tt_argcount > 0 && func_type_add_arg_types(type,
		    type->tt_argcount, type_gap) == OK)
	    за (цео i = 0; i < type->tt_argcount; ++i)
		type->tt_args[i] =
		    ufunc->uf_func_type->tt_args[i + pt->pt_argc];
    }
    ако (pt->pt_func != NULL)
	type->tt_member = pt->pt_func->uf_ret_type;

    врати type;
}

/*
 * Get a type_T for a "class" or an "object" typval in "tv".
 */
    static type_T *
oc_typval2type(typval_T *tv)
{
    ако (tv->v_type == VAR_CLASS)
    {
	ако (tv->vval.v_class == NULL)
	    врати &t_class;

	врати &tv->vval.v_class->class_type;
    }

    ако (tv->vval.v_object != NULL)
	врати &tv->vval.v_object->obj_class->class_object_type;

    врати &t_object_any;
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
    цео		argcount = 0;
    цео		min_argcount = 0;

    ако (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != NULL)
    {
	ако (tv->vval.v_partial->pt_func != NULL)
	    ufunc = tv->vval.v_partial->pt_func;
	инако
	    name = tv->vval.v_partial->pt_name;
    }
    инако
	name = tv->vval.v_string;

    ако (name == NULL && ufunc == NULL)
	врати &t_func_unknown;

    ако (name != NULL)
    {
	цео idx = find_internal_func(name);

	ако (idx >= 0)
	{
	    type_T *decl_type;  // unused

	    internal_func_get_argcount(idx, &argcount, &min_argcount);
	    member_type = internal_func_ret_type(idx, 0, NULL, &decl_type,
		    type_gap);
	}
	инако
	{
	    // Check if name contains "<".  If it does, then replace "<" with
	    // NUL and lookup the function and then look up the specific
	    // generic function using the supplied types.
	    char_u *p = generic_func_find_open_bracket(name);
	    ако (p == NULL)
		ufunc = find_func(name, FALSE);
	    инако
	    {
		// generic function
		char_u	cc = *p;

		*p = NUL;
		ufunc = find_func(name, FALSE);
		*p = cc;
		ако (ufunc != NULL && IS_GENERIC_FUNC(ufunc))
		{
		    ufunc = find_generic_func(ufunc, name, &p);
		    ако (ufunc == NULL)
			врати NULL;
		}
	    }
	}
    }
    ако (ufunc != NULL)
    {
	// May need to get the argument types from default values by
	// compiling the function.
	ако (ufunc->uf_def_status == UF_TO_BE_COMPILED
		&& compile_def_function(ufunc, TRUE, CT_NONE, NULL)
		== FAIL)
	    врати NULL;
	ако (ufunc->uf_func_type == NULL)
	    set_function_type(ufunc);
	ако (ufunc->uf_func_type != NULL)
	{
	    ако (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != NULL)
		врати partial_typval2type(tv, ufunc, type_gap);
	    врати ufunc->uf_func_type;
	}
    }

    type = get_type_ptr(type_gap);
    ако (type == NULL)
	врати NULL;
    type->tt_type = tv->v_type;
    type->tt_argcount = argcount;
    type->tt_min_argcount = min_argcount;
    ако (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != NULL
					&& tv->vval.v_partial->pt_argc > 0)
    {
	type->tt_argcount -= tv->vval.v_partial->pt_argc;
	type->tt_min_argcount -= tv->vval.v_partial->pt_argc;
    }
    type->tt_member = member_type;

    врати type;
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
typval2type_int(typval_T *tv, цео copyID, garray_T *type_gap, цео flags)
{
    switch (tv->v_type)
    {
	case VAR_UNKNOWN:
	    врати &t_unknown;

	case VAR_ANY:
	    врати &t_any;

	case VAR_VOID:
	    врати &t_void;

	case VAR_BOOL:
	    врати &t_bool;

	case VAR_SPECIAL:
	    врати special_typval2type(tv);

	case VAR_NUMBER:
	    врати &t_number;

	case VAR_FLOAT:
	    врати &t_float;

	case VAR_STRING:
	    врати &t_string;

	case VAR_BLOB:
	    ако (tv->vval.v_blob == NULL)
		врати &t_blob_null;
	    врати &t_blob;

	case VAR_LIST:
	    врати list_typval2type(tv, copyID, type_gap, flags);

	case VAR_TUPLE:
	    врати tuple_typval2type(tv, copyID, type_gap, flags);

	case VAR_DICT:
	    врати dict_typval2type(tv, copyID, type_gap, flags);

	case VAR_JOB:
	    врати &t_job;

	case VAR_CHANNEL:
	    врати &t_channel;

	case VAR_CLASS:
	case VAR_OBJECT:
	    врати oc_typval2type(tv);

	case VAR_TYPEALIAS:
	    врати &t_typealias;

	case VAR_FUNC:
	case VAR_PARTIAL:
	    врати fp_typval2type(tv, type_gap);

	case VAR_INSTR:
	default:
	    break;
    }

    врати NULL;
}

/*
 * Return TRUE if "tv" is not a bool but should be converted to bool.
 */
    цео
need_convert_to_bool(type_T *type, typval_T *tv)
{
    врати type != NULL && type == &t_bool && tv->v_type != VAR_BOOL
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
typval2type(typval_T *tv, цео copyID, garray_T *type_gap, цео flags)
{
    type_T *type = typval2type_int(tv, copyID, type_gap, flags);

    ако (type == NULL)
	врати NULL;

    ако (type != &t_bool && (tv->v_type == VAR_NUMBER
		&& (tv->vval.v_number == 0 || tv->vval.v_number == 1)))
	// Number 0 and 1 and expression with "&&" or "||" can also be used
	// for bool.
	type = &t_number_bool;
    инако ако (type != &t_float && tv->v_type == VAR_NUMBER)
	// A number can also be used for float.
	type = &t_number_float;
    врати type;
}

/*
 * Return TRUE if "type" can be used for a variable declaration.
 * Give an error and return FALSE if not.
 */
    цео
valid_declaration_type(type_T *type)
{
    ако (type->tt_type == VAR_SPECIAL  // null, none
	    || type->tt_type == VAR_VOID)
    {
	char *tofree = NULL;
	char *name = type_name(type, &tofree);
	semsg(_(e_invalid_type_in_variable_declaration_str), name);
	vim_free(tofree);
	врати FALSE;
    }
    врати TRUE;
}

/*
 * Get a type_T for a typval_T, used for v: variables.
 * "type_list" is used to temporarily create types in.
 */
    type_T *
typval2type_vimvar(typval_T *tv, garray_T *type_gap)
{
    ако (tv->v_type == VAR_LIST)  // e.g. for v:oldfiles
	врати &t_list_string;
    ако (tv->v_type == VAR_DICT)  // e.g. for v:event
	врати &t_dict_any;
    врати typval2type(tv, get_copyID(), type_gap, TVTT_DO_MEMBER);
}

    цео
check_typval_arg_type(
	type_T	    *expected,
	typval_T    *actual_tv,
	char	    *func_name,
	цео	    arg_idx)
{
    where_T	where = WHERE_INIT;

    ако (arg_idx > 0)
    {
	where.wt_index = arg_idx;
	where.wt_kind = WT_ARGUMENT;
    }
    where.wt_func_name = func_name;
    врати check_typval_type(expected, actual_tv, where);
}

/*
 * Return true if "expected" is list<any> or dict<any> and "actual_tv" is a
 * list or dict of that kind, which matches whatever its values are.
 */
    static bool
matches_listdict_any(type_T *expected, typval_T *actual_tv)
{
    врати (expected->tt_type == VAR_LIST || expected->tt_type == VAR_DICT)
	    && actual_tv->v_type == expected->tt_type
	    && expected->tt_member != NULL
	    && (expected->tt_member->tt_type == VAR_ANY
		|| expected->tt_member->tt_type == VAR_UNKNOWN);
}

/*
 * Return FAIL if "expected" and "actual" don't match.
 * When "argidx" > 0 it is included in the error message.
 */
    цео
check_typval_type(type_T *expected, typval_T *actual_tv, where_T where)
{
    garray_T	type_list;
    type_T	*actual_type;
    цео		res = FAIL;

    ако (expected == NULL)
	врати OK;  // didn't expect anything.

    // No need to go through the values of a possibly big list or dict.
    ако (matches_listdict_any(expected, actual_tv))
	врати OK;

    ga_init2(&type_list, sizeof(type_T *), 10);

    // A null_function and null_partial are special cases, they can be used to
    // clear a variable.
    ако ((actual_tv->v_type == VAR_FUNC && actual_tv->vval.v_string == NULL)
	    || (actual_tv->v_type == VAR_PARTIAL
					 && actual_tv->vval.v_partial == NULL))
	actual_type = &t_func_unknown;
    инако
	// When the actual type is list<any> or dict<any> go through the values
	// to possibly get a more specific type.
	actual_type = typval2type(actual_tv, get_copyID(), &type_list,
					  TVTT_DO_MEMBER | TVTT_MORE_SPECIFIC);
    ако (actual_type != NULL)
    {
	res = check_type_maybe(expected, actual_type, TRUE, where);
	ако (res == MAYBE && !(actual_type->tt_type == VAR_FUNC
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
    врати res;
}

    void
arg_type_mismatch(type_T *expected, type_T *actual, цео arg_idx)
{
    where_T	where = WHERE_INIT;

    ако (arg_idx > 0)
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
	    ако (where.wt_func_name == NULL)
		semsg(_(e_variable_nr_type_mismatch_expected_str_but_got_str),
			where.wt_index, typename1, typename2);
	    инако
		semsg(_(e_variable_nr_type_mismatch_expected_str_but_got_str_in_str),
			where.wt_index, typename1, typename2, where.wt_func_name);
	    break;
	case WT_ARGUMENT:
	    ако (where.wt_func_name == NULL)
		semsg(_(e_argument_nr_type_mismatch_expected_str_but_got_str),
			where.wt_index, typename1, typename2);
	    инако
		semsg(_(e_argument_nr_type_mismatch_expected_str_but_got_str_in_str),
			where.wt_index, typename1, typename2, where.wt_func_name);
	    break;
	case WT_CAST:
	case WT_UNKNOWN:
	    ако (where.wt_func_name == NULL)
		semsg(_(e_type_mismatch_expected_str_but_got_str),
			typename1, typename2);
	    инако
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
    static цео
check_tuple_type_maybe(
    type_T	*expected,
    type_T	*actual,
    where_T	where)
{
    ако (expected->tt_argcount == -1 || actual->tt_argcount == -1
		|| expected->tt_args == NULL || actual->tt_args == NULL)
	врати OK;

    // For a non-variadic tuple, the number of items must match
    ако (!(expected->tt_flags & TTFLAG_VARARGS)
	    && expected->tt_argcount != actual->tt_argcount)
	врати FAIL;

    // compare the type of each tuple item
    за (цео i = 0; i < actual->tt_argcount; ++i)
    {
	type_T	*exp_type;
	type_T	*actual_type;

	ако (expected->tt_flags & TTFLAG_VARARGS)
	{
	    ако (i < expected->tt_argcount - 1)
		exp_type = expected->tt_args[i];
	    инако
		// For a variadic tuple, the last type is a List.  So use the
		// List member type.
		exp_type = expected->tt_args[expected->tt_argcount - 1]->tt_member;
	}
	инако
	    exp_type = expected->tt_args[i];

	ако (actual->tt_flags & TTFLAG_VARARGS)
	{
	    ако (i < actual->tt_argcount - 1)
		actual_type = actual->tt_args[i];
	    инако
		// For a variadic tuple, the last type is a List.  So use the
		// List member type.
		actual_type = actual->tt_args[actual->tt_argcount - 1]->tt_member;
	}
	инако
	    actual_type = actual->tt_args[i];

	// Allow for using "any" type for a tuple item
	ако (actual->tt_args[i] != &t_any && check_type(exp_type, actual_type,
						FALSE, where) == FAIL)
	    врати FAIL;
    }

    врати OK;
}

/*
 * Check if the expected and actual types match for a function
 * Returns OK if "expected" and "actual" are matching function types.
 * Returns FAIL if "expected" and "actual" are different types.
 * Returns MAYBE when a runtime type check is needed.
 */
    static цео
check_func_type_maybe(
    type_T	*expected,
    type_T	*actual,
    where_T	where)
{
    цео ret = OK;

    // If the return type is unknown it can be anything, including
    // nothing, thus there is no point in checking.
    ако (expected->tt_member != &t_unknown)
    {
	ако (actual->tt_member != NULL
		&& actual->tt_member != &t_unknown)
	{
	    where_T  func_where = where;

	    func_where.wt_kind = WT_METHOD_RETURN;
	    ret = check_type_maybe(expected->tt_member,
		    actual->tt_member, FALSE,
		    func_where);
	}
	инако
	    ret = MAYBE;
    }
    ако (ret != FAIL
	    && ((expected->tt_flags & TTFLAG_VARARGS)
		!= (actual->tt_flags & TTFLAG_VARARGS))
	    && expected->tt_argcount != -1)
	ret = FAIL;
    ако (ret != FAIL && expected->tt_argcount != -1
	    && actual->tt_min_argcount != -1
	    && (actual->tt_argcount == -1
		|| (actual->tt_argcount < expected->tt_min_argcount
		    || actual->tt_argcount > expected->tt_argcount)))
	ret = FAIL;
    ако (ret != FAIL && expected->tt_args != NULL
	    && actual->tt_args != NULL)
    {
	цео i;

	за (i = 0; i < expected->tt_argcount
		&& i < actual->tt_argcount; ++i)
	{
	    where_T  func_where = where;
	    func_where.wt_kind = WT_METHOD_ARG;

	    // Allow for using "any" argument type, lambda's have them.
	    ако (actual->tt_args[i] != &t_any && check_type(
			expected->tt_args[i], actual->tt_args[i], FALSE,
			func_where) == FAIL)
	    {
		ret = FAIL;
		break;
	    }
	}
    }
    ако (ret == OK && expected->tt_argcount >= 0
	    && actual->tt_argcount == -1)
	// check the argument count at runtime
	ret = MAYBE;

    врати ret;
}

/*
 * Check if the expected and actual types match for an object
 * Returns OK if "expected" and "actual" are matching object types.
 * Returns FAIL if "expected" and "actual" are different types.
 * Returns MAYBE when a runtime type check is needed.
 */
    static цео
check_object_type_maybe(
    type_T	*expected,
    type_T	*actual,
    where_T	where)
{
    цео ret = OK;

    ако (actual->tt_type == VAR_ANY)
	врати MAYBE;	// use runtime type check
    ако (actual->tt_type != VAR_OBJECT)
	врати FAIL;	// don't use tt_class
    ако (actual->tt_class == NULL)    // null object
	врати OK;
    // t_object_any matches any object except for an enum item
    ако (expected == &t_object_any && !IS_ENUM(actual->tt_class))
	врати OK;

    // For object method arguments, do a invariant type check in
    // an extended class.  For all others, do a covariance type check.
    ако (where.wt_kind == WT_METHOD_ARG)
    {
	ако (actual->tt_class != expected->tt_class)
	    ret = FAIL;
    }
    инако ако (!class_instance_of(actual->tt_class, expected->tt_class))
    {
	// Check if this is an up-cast, if so we'll have to check the type at
	// runtime.
	ако (where.wt_kind == WT_CAST &&
		class_instance_of(expected->tt_class, actual->tt_class))
	    ret = MAYBE;
	инако
	    ret = FAIL;
    }

    врати ret;
}

/*
 * Check if the expected and actual types match.
 * Does not allow for assigning "any" to a specific type.
 * When "argidx" > 0 it is included in the error message.
 * Return OK if types match.
 * Return FAIL if types do not match.
 */
    цео
check_type(
	type_T	*expected,
	type_T	*actual,
	цео	give_msg,
	where_T where)
{
    цео ret = check_type_maybe(expected, actual, give_msg, where);

    врати ret == MAYBE ? OK : ret;
}

/*
 * As check_type() but return MAYBE when a runtime type check should be used
 * when compiling.
 */
    цео
check_type_maybe(
	type_T	*expected,
	type_T	*actual,
	цео	give_msg,
	where_T where)
{
    цео ret = OK;

    // When expected is "unknown" we accept any actual type.
    // When expected is "any" we accept any actual type except "void".
    ако (expected->tt_type != VAR_UNKNOWN
	    && !(expected->tt_type == VAR_ANY && actual->tt_type != VAR_VOID))

    {
	// tt_type should match, except that a "partial" can be assigned to a
	// variable with type "func".
	// And "unknown" (using global variable) and "any" need a runtime type
	// check.
	ако (!(expected->tt_type == actual->tt_type
		    || actual->tt_type == VAR_UNKNOWN
		    || actual->tt_type == VAR_ANY
		    || (expected->tt_type == VAR_FUNC
					   && actual->tt_type == VAR_PARTIAL)))
	{
	    ако (expected->tt_type == VAR_BOOL
					&& (actual->tt_flags & TTFLAG_BOOL_OK))
		// Using number 0 or 1 for bool is OK.
		врати OK;
	    ако (expected->tt_type == VAR_FLOAT
		    && actual->tt_type == VAR_NUMBER
		    && ((expected->tt_flags & TTFLAG_NUMBER_OK)
			     || (actual->tt_flags & TTFLAG_FLOAT_OK)))
		// Using a number where a float is expected is OK here.
		врати OK;
	    ако (expected->tt_type == VAR_LIST
		    && actual->tt_type == VAR_TUPLE
		    && (expected->tt_flags & TTFLAG_TUPLE_OK))
		// Using a tuple where a list is expected is OK here.
		врати OK;
	    ако (give_msg)
		type_mismatch_where(expected, actual, where);
	    врати FAIL;
	}
	ако (expected->tt_type == VAR_DICT || expected->tt_type == VAR_LIST)
	{
	    // "unknown" is used for an empty list or dict
	    ако (actual->tt_member != NULL && actual->tt_member != &t_unknown)
		ret = check_type_maybe(expected->tt_member, actual->tt_member,
								 FALSE, where);
	}
	инако ако (expected->tt_type == VAR_TUPLE && actual != &t_any)
	    ret =  check_tuple_type_maybe(expected, actual, where);
	инако ако (expected->tt_type == VAR_FUNC && actual != &t_any)
	    ret = check_func_type_maybe(expected, actual, where);
	инако ако (expected->tt_type == VAR_OBJECT)
	    ret = check_object_type_maybe(expected, actual, where);

	ако (ret == FAIL && give_msg)
	    type_mismatch_where(expected, actual, where);
    }

    ако (ret == OK && expected->tt_type != VAR_UNKNOWN
	    && expected->tt_type != VAR_ANY
	    && (actual->tt_type == VAR_UNKNOWN || actual->tt_type == VAR_ANY))
	// check the type at runtime
	ret = MAYBE;

    врати ret;
}

/*
 * Check that the arguments of "type" match "argvars[argcount]".
 * "base_tv" is from "expr->Func()".
 * Return OK/FAIL.
 */
    цео
check_argument_types(
	type_T	    *type,
	typval_T    *argvars,
	цео	    argcount,
	typval_T    *base_tv,
	char_u	    *name)
{
    цео	    varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;
    цео	    i;
    цео	    totcount = argcount + (base_tv == NULL ? 0 : 1);

    ако (type->tt_type != VAR_FUNC && type->tt_type != VAR_PARTIAL)
	врати OK;  // just in case
    ако (totcount < type->tt_min_argcount - varargs)
    {
	emsg_funcname(e_not_enough_arguments_for_function_str, name);
	врати FAIL;
    }
    ако (!varargs && type->tt_argcount >= 0 && totcount > type->tt_argcount)
    {
	emsg_funcname(e_too_many_arguments_for_function_str, name);
	врати FAIL;
    }
    ако (type->tt_args == NULL)
	врати OK;  // cannot check


    за (i = 0; i < totcount; ++i)
    {
	type_T	    *expected;
	typval_T    *tv;

	ако (base_tv != NULL)
	{
	    ако (i == 0)
		tv = base_tv;
	    инако
		tv = &argvars[i - 1];
	}
	инако
	    tv = &argvars[i];
	ако (varargs && i >= type->tt_argcount - 1)
	{
	    expected = type->tt_args[type->tt_argcount - 1];
	    ако (expected != NULL && expected->tt_type == VAR_LIST)
		expected = expected->tt_member;
	    ако (expected == NULL)
		expected = &t_any;
	}
	инако
	    expected = type->tt_args[i];

	// check the type, unless the value is v:none
	ако ((tv->v_type != VAR_SPECIAL || tv->vval.v_number != VVAL_NONE)
		   && check_typval_arg_type(expected, tv, NULL, i + 1) == FAIL)
	    врати FAIL;
    }
    врати OK;
}

// Maximum nesting of types, to avoid running out of stack space.  E.g.
// "list<list<number>>" is nested twice.
#define MAX_TYPE_DEPTH	1000

// Current number of levels of parse_type() and the deepest level reached while
// parsing the current type.  The nesting is one less than the number of
// levels.
static цео	parse_depth = 0;
static цео	parse_max_depth = 0;

/*
 * Skip over type in list<type>, dict<type> or tuple<type>.
 * Returns a pointer to the character after the type.  "syn_error" is set to
 * TRUE on syntax error.
 */
    static char_u *
skip_member_type(char_u *start, char_u *p, цео *syn_error)
{
    ако (STRNCMP("tuple", start, 5) == 0)
    {
	// handle tuple<{type1}, {type2}, ....<type>>
	p = skipwhite(p + 1);
	док (*p != '>' && *p != NUL)
	{
	    char_u *sp = p;

	    ако (STRNCMP(p, "...", 3) == 0)
		p += 3;
	    p = skip_type(p, TRUE);
	    ако (p == sp)
	    {
		*syn_error = TRUE;
		врати p;  // syntax error
	    }
	    ако (*p == ',')
		p = skipwhite(p + 1);
	}
	ако (*p == '>')
	    p++;
    }
    инако
    {
	p = skipwhite(p);
	p = skip_type(skipwhite(p + 1), FALSE);
	p = skipwhite(p);
	ако (*p == '>')
	    ++p;
    }

    врати p;
}

/*
 * Skip over a function type.  Returns a pointer to the character after the
 * type.  "syn_error" is set to TRUE on syntax error.
 */
    static char_u *
skip_func_type(char_u *p, цео *syn_error)
{
    ако (*p == '(')
    {
	// handle func(args): type
	++p;
	док (*p != ')' && *p != NUL)
	{
	    char_u *sp = p;

	    ако (STRNCMP(p, "...", 3) == 0)
		p += 3;
	    p = skip_type(p, TRUE);
	    ако (p == sp)
	    {
		*syn_error = TRUE;
		врати p;  // syntax error
	    }
	    ако (*p == ',')
		p = skipwhite(p + 1);
	}
	ако (*p == ')')
	{
	    ако (p[1] == ':')
		p = skip_type(skipwhite(p + 2), FALSE);
	    инако
		++p;
	}
    }
    инако
    {
	// handle func: return_type
	p = skip_type(skipwhite(p + 1), FALSE);
    }

    врати p;
}

/*
 * Inner part of skip_type().
 */
    static char_u *
skip_type_inner(char_u *start, цео optional)
{
    char_u	*p = start;
    цео		syn_error = FALSE;

    ако (optional && *p == '?')
	++p;

    // Also skip over "." for imported classes: "import.ClassName".
    док (ASCII_ISALNUM(*p) || *p == '_' || *p == '.')
	++p;

    // Skip over "<type>"; this is permissive about white space.
    ако (*skipwhite(p) == '<')
    {
	p = skip_member_type(start, p, &syn_error);
	ако (syn_error)
	    врати p;
    }
    инако ако ((*p == '(' || (*p == ':' && VIM_ISWHITE(p[1])))
					     && STRNCMP("func", start, 4) == 0)
    {
	// skip over function type
	p = skip_func_type(p, &syn_error);
	ако (syn_error)
	    врати p;
    }

    врати p;
}

/*
 * Skip over a type definition and return a pointer to just after it.
 * When "optional" is TRUE then a leading "?" is accepted.
 */
    char_u *
skip_type(char_u *start, цео optional)
{
    static цео	skip_depth = 0;
    char_u	*p = start;

    // Too deeply nested: skip to the matching ">" without recursing, an
    // error is given when parsing the type.
    ако (skip_depth > MAX_TYPE_DEPTH)
    {
	цео	depth = 0;

	за ( ; *p != NUL; ++p)
	{
	    ако (*p == '<')
		++depth;
	    инако ако (*p == '>')
	    {
		ако (depth == 0)
		    break;
		ако (--depth == 0)
		    врати p + 1;
	    }
	    инако ако (depth == 0 && !ASCII_ISALNUM(*p) && *p != '_'
								&& *p != '.')
		break;
	}
	врати p;
    }

    ++skip_depth;
    p = skip_type_inner(start, optional);
    --skip_depth;
    врати p;
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
	цео	    give_error,
	char	    *info,
	ufunc_T	    *ufunc,
	cctx_T	    *cctx)
{
    char_u  *arg_start = *arg;
    type_T  *member_type;
    цео	    prev_called_emsg = called_emsg;

    ако (**arg != '<')
    {
	ако (give_error)
	{
	    ако (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    инако
		semsg(_(e_missing_type_after_str), info);
	}
	врати NULL;
    }
    *arg = skipwhite(*arg + 1);

    member_type = parse_type(arg, type_gap, ufunc, cctx, give_error);
    ако (member_type == NULL || !valid_declaration_type(member_type))
	врати NULL;

    *arg = skipwhite(*arg);
    ако (**arg != '>' && called_emsg == prev_called_emsg)
    {
	ако (give_error)
	    semsg(_(e_missing_gt_after_type_str), arg_start);
	врати NULL;
    }
    ++*arg;

    ако (type->tt_type == VAR_LIST)
	врати get_list_type(member_type, type_gap);
    врати get_dict_type(member_type, type_gap);
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
    цео		give_error,
    ufunc_T	*ufunc,
    cctx_T	*cctx)
{
    char_u  *p;
    type_T  *type;
    type_T  *ret_type = &t_unknown;
    цео	argcount = -1;
    цео	flags = 0;
    цео	first_optional = -1;
    type_T	*arg_type[MAX_FUNC_ARGS + 1];

    // func({type}, ...{type}): {type}
    *arg += len;
    ако (**arg == '(')
    {
	// "func" may or may not return a value, "func()" does
	// not return a value.
	ret_type = &t_void;

	p = ++*arg;
	argcount = 0;
	док (*p != NUL && *p != ')')
	{
	    ако (*p == '?')
	    {
		ако (first_optional == -1)
		    first_optional = argcount;
		++p;
	    }
	    инако ако (STRNCMP(p, "...", 3) == 0)
	    {
		flags |= TTFLAG_VARARGS;
		p += 3;
	    }
	    инако ако (first_optional != -1)
	    {
		ако (give_error)
		    emsg(_(e_mandatory_argument_after_optional_argument));
		врати NULL;
	    }

	    type = parse_type(&p, type_gap, ufunc, cctx, give_error);
	    ако (type == NULL || !valid_declaration_type(type))
		врати NULL;
	    ако ((flags & TTFLAG_VARARGS) != 0 && type->tt_type != VAR_LIST)
	    {
		char *tofree;
		semsg(_(e_variable_arguments_type_must_be_list_str),
			type_name(type, &tofree));
		vim_free(tofree);
		врати NULL;
	    }
	    arg_type[argcount++] = type;

	    // Nothing comes after "...{type}".
	    ако (flags & TTFLAG_VARARGS)
		break;

	    ако (*p != ',' && *skipwhite(p) == ',')
	    {
		ако (give_error)
		    semsg(_(e_no_white_space_allowed_before_str_str),
			    ",", p);
		врати NULL;
	    }
	    ако (*p == ',')
	    {
		++p;
		ако (!VIM_ISWHITE(*p))
		{
		    ако (give_error)
			semsg(_(e_white_space_required_after_str_str),
				",", p - 1);
		    врати NULL;
		}
	    }
	    p = skipwhite(p);
	    ако (argcount == MAX_FUNC_ARGS)
	    {
		ако (give_error)
		    emsg(_(e_too_many_argument_types));
		врати NULL;
	    }
	}

	p = skipwhite(p);
	ако (*p != ')')
	{
	    ако (give_error)
		emsg(_(e_missing_closing_paren));
	    врати NULL;
	}
	*arg = p + 1;
    }
    ако (**arg == ':')
    {
	// parse return type
	++*arg;
	ако (!VIM_ISWHITE(**arg) && give_error)
	    semsg(_(e_white_space_required_after_str_str), ":", *arg - 1);
	*arg = skipwhite(*arg);
	ret_type = parse_type(arg, type_gap, ufunc, cctx, give_error);
	ако (ret_type == NULL)
	    врати NULL;
    }
    ако (flags == 0 && first_optional == -1 && argcount <= 0)
	type = get_func_type(ret_type, argcount, type_gap);
    инако
    {
	type = alloc_func_type(ret_type, argcount, type_gap);
	type->tt_flags = flags;
	ако (argcount > 0)
	{
	    type->tt_argcount = argcount;
	    type->tt_min_argcount = first_optional == -1
						? argcount : first_optional;
	    ако (func_type_add_arg_types(type, argcount, type_gap) == FAIL)
		врати NULL;
	    mch_memmove(type->tt_args, arg_type, sizeof(type_T *) * argcount);
	}
    }

    врати type;
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
    цео		give_error,
    ufunc_T	*ufunc,
    cctx_T	*cctx)
{
    char_u	*p;
    type_T	*type;
    type_T	*ret_type = NULL;
    цео		typecount = -1;
    цео		flags = 0;
    garray_T	tuple_types_ga;

    ga_init2(&tuple_types_ga, sizeof(type_T *), 10);

    // tuple<{type}, {type}>
    // tuple<{type}, ...{type}>
    ако (**arg != '<')
    {
	ако (give_error)
	{
	    ако (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    инако
		semsg(_(e_missing_type_after_str), "tuple");
	}

	// only "tuple" is specified
	врати NULL;
    }

    p = ++*arg;
    typecount = 0;
    док (*p != NUL && *p != '>')
    {
	ако (STRNCMP(p, "...", 3) == 0)
	{
	    flags |= TTFLAG_VARARGS;
	    p += 3;
	}

	type = parse_type(&p, type_gap, ufunc, cctx, give_error);
	ако (type == NULL || !valid_declaration_type(type))
	    goto on_err;

	ако ((flags & TTFLAG_VARARGS) != 0 && type->tt_type != VAR_LIST)
	{
	    char *tofree;
	    semsg(_(e_variadic_tuple_must_end_with_list_type_str),
						type_name(type, &tofree));
	    vim_free(tofree);
	    goto on_err;
	}

	// Add the item type
	ако (ga_grow(&tuple_types_ga, 1) == FAIL)
	    goto on_err;
	((type_T **)tuple_types_ga.ga_data)[tuple_types_ga.ga_len] = type;
	tuple_types_ga.ga_len++;
	typecount++;

	// Nothing comes after "...{type}".
	ако (flags & TTFLAG_VARARGS)
	    break;

	ако (*p != ',' && *skipwhite(p) == ',')
	{
	    ако (give_error)
		semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
	    goto on_err;
	}
	ако (*p == ',')
	{
	    ++p;
	    ако (!VIM_ISWHITE(*p))
	    {
		ако (give_error)
		    semsg(_(e_white_space_required_after_str_str),
			    ",", p - 1);
		goto on_err;
	    }
	}
	p = skipwhite(p);
    }

    p = skipwhite(p);
    ако (*p != '>' || typecount <= 0)
    {
	ако (give_error)
	    semsg(_(e_missing_type_after_str), p);
	goto on_err;
    }
    *arg = p + 1;

    ret_type = alloc_tuple_type(typecount, type_gap);
    ако (ret_type == &t_any
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

    врати ret_type;
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
    цео		give_error,
    cctx_T	*cctx)
{
    char_u	*arg_start = *arg;
    type_T	*object_type;
    цео		prev_called_emsg = called_emsg;

    // object<X> or object<any>
    ако (**arg != '<')
    {
	ако (give_error)
	{
	    ако (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    инако
		semsg(_(e_missing_type_after_str), "object");
	}

	// only "object" is specified
	врати NULL;
    }

    // skip spaces following "object<"
    *arg = skipwhite(*arg + 1);

    object_type = parse_type(arg, type_gap, NULL, cctx, give_error);
    ако (object_type == NULL)
	врати NULL;

    *arg = skipwhite(*arg);
    ако (**arg != '>' && called_emsg == prev_called_emsg)
    {
	ако (give_error)
	    semsg(_(e_missing_gt_after_type_str), arg_start);
	врати NULL;
    }
    ++*arg;

    ако (object_type->tt_type == VAR_ANY)
	врати &t_object_any;

    ако (object_type->tt_type != VAR_OBJECT)
    {
	// specified type is not a class
	ако (give_error)
	    semsg(_(e_class_name_not_found_str), arg_start);
	врати NULL;
    }

    врати object_type;
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
    цео		give_error,
    ufunc_T	*ufunc,
    cctx_T	*cctx)
{
    цео		did_emsg_before = did_emsg;
    typval_T	tv;

    tv.v_type = VAR_UNKNOWN;
    ако (eval_variable_import(*arg, &tv) == OK)
    {
	ако (tv.v_type == VAR_CLASS && tv.vval.v_class != NULL)
	{
	    type_T *type = get_type_ptr(type_gap);
	    ако (type != NULL)
	    {
		// Although the name is that of a class or interface, the type
		// uses will be an object.
		type->tt_type = VAR_OBJECT;
		type->tt_class = tv.vval.v_class;
		clear_tv(&tv);

		*arg += len;
		// Skip over ".ClassName".
		док (ASCII_ISALNUM(**arg) || **arg == '_' || **arg == '.')
		    ++*arg;

		врати type;
	    }
	}
	инако ако (tv.v_type == VAR_TYPEALIAS)
	{
	    // user defined type
	    // The type alias may itself be a nested type, check the resulting
	    // nesting depth.
	    цео	depth = parse_depth - 1
				+ MAX(tv.vval.v_typealias->ta_depth, 1);
	    ако (depth - 1 > MAX_TYPE_DEPTH)
	    {
		clear_tv(&tv);
		ако (give_error)
		    emsg(_(e_type_nested_too_deep));
		врати NULL;
	    }
	    ако (depth > parse_max_depth)
		parse_max_depth = depth;

	    type_T *type = copy_type(tv.vval.v_typealias->ta_type, type_gap);
	    *arg += len;
	    clear_tv(&tv);
	    // Skip over ".TypeName".
	    док (ASCII_ISALNUM(**arg) || **arg == '_' || **arg == '.')
		++*arg;
	    врати type;
	}

	clear_tv(&tv);
    }

    // Check whether it is a generic type
    type_T *type = find_generic_type(*arg, len, ufunc, cctx);
    ако (type != NULL)
    {
	*arg += len;
	врати type;
    }

    ако (give_error && (did_emsg == did_emsg_before))
    {
	char_u	*p = skip_type(*arg, FALSE);
	char	cc = *p;

	*p = NUL;
	semsg(_(e_type_not_recognized_str), *arg);
	*p = cc;
    }

    врати NULL;
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
    цео		give_error)
{
    char_u  *p = *arg;
    size_t  len;

    // Skip over the first word.
    док (ASCII_ISALNUM(*p) || *p == '_')
	++p;
    len = p - *arg;

    switch (**arg)
    {
	case 'a':
	    ако (len == 3 && STRNCMP(*arg, "any", len) == 0)
	    {
		*arg += len;
		врати &t_any;
	    }
	    break;
	case 'b':
	    ако (len == 4 && STRNCMP(*arg, "bool", len) == 0)
	    {
		*arg += len;
		врати &t_bool;
	    }
	    ако (len == 4 && STRNCMP(*arg, "blob", len) == 0)
	    {
		*arg += len;
		врати &t_blob;
	    }
	    break;
	case 'c':
	    ако (len == 7 && STRNCMP(*arg, "channel", len) == 0)
	    {
		*arg += len;
		врати &t_channel;
	    }
	    break;
	case 'd':
	    ако (len == 4 && STRNCMP(*arg, "dict", len) == 0)
	    {
		*arg += len;
		врати parse_type_member(arg, &t_dict_any, type_gap,
						give_error, "dict", ufunc,
						cctx);
	    }
	    break;
	case 'f':
	    ако (len == 5 && STRNCMP(*arg, "float", len) == 0)
	    {
		*arg += len;
		врати &t_float;
	    }
	    ако (len == 4 && STRNCMP(*arg, "func", len) == 0)
		врати parse_type_func(arg, len, type_gap, give_error, ufunc,
									cctx);
	    break;
	case 'j':
	    ако (len == 3 && STRNCMP(*arg, "job", len) == 0)
	    {
		*arg += len;
		врати &t_job;
	    }
	    break;
	case 'l':
	    ако (len == 4 && STRNCMP(*arg, "list", len) == 0)
	    {
		*arg += len;
		врати parse_type_member(arg, &t_list_any, type_gap,
						give_error, "list", ufunc,
						cctx);
	    }
	    break;
	case 'n':
	    ако (len == 6 && STRNCMP(*arg, "number", len) == 0)
	    {
		*arg += len;
		врати &t_number;
	    }
	    break;
	case 'o':
	    ако (len == 6 && STRNCMP(*arg, "object", len) == 0)
	    {
		*arg += len;
		врати parse_type_object(arg, type_gap, give_error, cctx);
	    }
	    break;
	case 's':
	    ако (len == 6 && STRNCMP(*arg, "string", len) == 0)
	    {
		*arg += len;
		врати &t_string;
	    }
	    break;
	case 't':
	    ако (len == 5 && STRNCMP(*arg, "tuple", len) == 0)
	    {
		*arg += len;
		врати parse_type_tuple(arg, type_gap, give_error, ufunc,
									cctx);
	    }
	    break;
	case 'v':
	    ако (len == 4 && STRNCMP(*arg, "void", len) == 0)
	    {
		*arg += len;
		врати &t_void;
	    }
	    break;
    }

    // User defined type
    врати parse_type_user_defined(arg, len, type_gap, give_error, ufunc,
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
    цео		give_error)
{
    type_T	*type;

    ако (parse_depth > MAX_TYPE_DEPTH)
    {
	ако (give_error)
	    emsg(_(e_type_nested_too_deep));
	врати NULL;
    }
    ако (parse_depth == 0)
	parse_max_depth = 0;
    ++parse_depth;
    ако (parse_depth > parse_max_depth)
	parse_max_depth = parse_depth;
    type = parse_type_inner(arg, type_gap, ufunc, cctx, give_error);
    --parse_depth;
    врати type;
}

/*
 * Return the number of levels of the type last parsed by parse_type().
 */
    цео
parse_type_depth(void)
{
    врати parse_max_depth;
}

/*
 * Check if "type1" and "type2" are exactly the same.
 * "flags" can have ETYPE_ARG_UNKNOWN, which means that an unknown argument
 * type in "type1" is accepted.
 */
    цео
equal_type(type_T *type1, type_T *type2, цео flags)
{
    цео i;

    ако (type1 == NULL || type2 == NULL)
	врати FALSE;
    ако (type1->tt_type != type2->tt_type)
	врати FALSE;
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
	    врати type1->tt_class == type2->tt_class;
	case VAR_LIST:
	case VAR_DICT:
	    врати equal_type(type1->tt_member, type2->tt_member, flags);
	case VAR_TUPLE:
	    ако (type1->tt_argcount != type2->tt_argcount)
		врати FALSE;
	    ако (type1->tt_argcount < 0
			   || type1->tt_args == NULL || type2->tt_args == NULL)
		врати TRUE;
	    за (i = 0; i < type1->tt_argcount; ++i)
		ако ((flags & ETYPE_ARG_UNKNOWN) == 0
			&& !equal_type(type1->tt_args[i], type2->tt_args[i],
									flags))
		    врати FALSE;
	    врати TRUE;
	case VAR_FUNC:
	case VAR_PARTIAL:
	    ако (!equal_type(type1->tt_member, type2->tt_member, flags)
		    || type1->tt_argcount != type2->tt_argcount)
		врати FALSE;
	    ако (type1->tt_argcount < 0
			   || type1->tt_args == NULL || type2->tt_args == NULL)
		врати TRUE;
	    за (i = 0; i < type1->tt_argcount; ++i)
		ако ((flags & ETYPE_ARG_UNKNOWN) == 0
			&& !equal_type(type1->tt_args[i], type2->tt_args[i],
									flags))
		    врати FALSE;
	    врати TRUE;
    }
    врати TRUE;
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
    ако (type1 == &t_func_unknown)
    {
	*dest = type2;
	врати;
    }
    ако (type2 == &t_func_unknown)
    {
	*dest = type1;
	врати;
    }

    common_type(type1->tt_member, type2->tt_member, &common, type_gap);
    ако (type1->tt_argcount == type2->tt_argcount
	    && type1->tt_argcount >= 0)
    {
	цео argcount = type1->tt_argcount;
	цео i;

	*dest = alloc_func_type(common, argcount, type_gap);
	ако (type1->tt_args != NULL && type2->tt_args != NULL)
	{
	    ако (func_type_add_arg_types(*dest, argcount,
			type_gap) == OK)
		за (i = 0; i < argcount; ++i)
		    common_type(type1->tt_args[i], type2->tt_args[i],
			    &(*dest)->tt_args[i], type_gap);
	}
    }
    инако
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
    ако (equal_type(type1, type2, 0))
    {
	*dest = type1;
	врати;
    }

    // If either is VAR_UNKNOWN use the other type.  An empty list/dict has no
    // specific type.
    ако (type1 == NULL || type1->tt_type == VAR_UNKNOWN)
    {
	*dest = type2;
	врати;
    }
    ако (type2 == NULL || type2->tt_type == VAR_UNKNOWN)
    {
	*dest = type1;
	врати;
    }

    ако (type1->tt_type == type2->tt_type)
    {
	ако (type1->tt_type == VAR_LIST
		|| type1->tt_type == VAR_DICT)
	{
	    type_T *common;

	    common_type(type1->tt_member, type2->tt_member, &common, type_gap);
	    ако (type1->tt_type == VAR_LIST)
		*dest = get_list_type(common, type_gap);
	    инако
		*dest = get_dict_type(common, type_gap);
	    врати;
	}
	инако ако (type1->tt_type == VAR_FUNC)
	{
	    common_type_var_func(type1, type2, dest, type_gap);
	    врати;
	}
	инако ако (type1->tt_type == VAR_OBJECT)
	{
	    *dest = &t_object_any;
	    врати;
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
    ако (type->tt_type == VAR_TUPLE)
    {
	ако (type->tt_argcount != 1)
	    врати &t_any;

	ако (type->tt_flags & TTFLAG_VARARGS)
	    врати type->tt_args[0]->tt_member;
	инако
	    врати type->tt_args[0];
    }

    // For "any" the item type is not known.
    врати type->tt_member == NULL ? &t_any : type->tt_member;
}

/*
 * Push an entry onto the type stack.  "type" used both for the current type
 * and the declared type.
 * Returns FAIL when out of memory.
 */
    цео
push_type_stack(cctx_T *cctx, type_T *type)
{
    врати push_type_stack2(cctx, type, type);
}

/*
 * Push an entry onto the type stack.  "type" is the current type, "decl_type"
 * is the declared type.
 * Returns FAIL when out of memory.
 */
    цео
push_type_stack2(cctx_T *cctx, type_T *type, type_T *decl_type)
{
    garray_T	*stack = &cctx->ctx_type_stack;
    type2_T	*typep;

    ако (GA_GROW_FAILS(stack, 1))
	врати FAIL;
    typep = ((type2_T *)stack->ga_data) + stack->ga_len;
    typep->type_curr = type;
    typep->type_decl = decl_type;
    ++stack->ga_len;
    врати OK;
}

/*
 * Set the type of the top of the stack to "type".
 */
    void
set_type_on_stack(cctx_T *cctx, type_T *type, цео offset)
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
get_type_on_stack(cctx_T *cctx, цео offset)
{
    garray_T	*stack = &cctx->ctx_type_stack;

    ако (offset + 1 > stack->ga_len)
	врати &t_unknown;
    врати (((type2_T *)stack->ga_data) + stack->ga_len - offset - 1)
								   ->type_curr;
}

/*
 * Get the declared type from the type stack.  If "offset" is zero the one at
 * the top,
 * if "offset" is one the type above that, etc.
 * Returns &t_unknown if there is no such stack entry.
 */
    type_T *
get_decl_type_on_stack(cctx_T *cctx, цео offset)
{
    garray_T	*stack = &cctx->ctx_type_stack;

    ако (offset + 1 > stack->ga_len)
	врати &t_unknown;
    врати (((type2_T *)stack->ga_data) + stack->ga_len - offset - 1)
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
	цео	    count,
	цео	    skip,
	cctx_T	    *cctx)
{
    garray_T	*stack = &cctx->ctx_type_stack;
    type2_T	*typep;
    garray_T    *type_gap = cctx->ctx_type_list;
    цео		i;
    type_T	*result;
    type_T	*type;

    // Use "unknown" for an empty list or dict.
    ако (count == 0)
	врати &t_unknown;
    // Find the common type from following items.
    typep = ((type2_T *)stack->ga_data) + stack->ga_len;
    result = &t_unknown;
    за (i = 0; i < count; ++i)
    {
	type = (typep -((count - i) * skip) + skip - 1)->type_curr;
	ако (check_type_is_value(type) == FAIL)
	    врати NULL;
	ако (result != &t_any)
	    common_type(type, result, &result, type_gap);
    }

    врати result;
}

/*
 * Get the types of items in a tuple on the stack of "cctx".
 * Returns the number of types.  Returns -1 on failure.
 */
    цео
get_tuple_type_from_stack(
    цео		count,
    garray_T	*tuple_types_gap,
    cctx_T	*cctx)
{
    garray_T	*stack = &cctx->ctx_type_stack;
    type2_T	*typep;
    type_T	*type = NULL;

    // Use "unknown" for an empty tuple
    ако (count == 0)
	врати 0;

    // Find the common type from following items.
    typep = ((type2_T *)stack->ga_data) + stack->ga_len;
    за (цео i = 0; i < count; i++)
    {
	type = (typep - (count - i))->type_curr;
	ако (check_type_is_value(type) == FAIL)
	    врати -1;
	ако (ga_grow(tuple_types_gap, 1) == FAIL)
	    врати -1;
	((type_T **)tuple_types_gap->ga_data)[tuple_types_gap->ga_len] = type;
	tuple_types_gap->ga_len++;
    }

    врати tuple_types_gap->ga_len;
}

    char *
vartype_name(vartype_T type)
{
    switch (type)
    {
	case VAR_UNKNOWN: break;
	case VAR_ANY: врати "any";
	case VAR_VOID: врати "void";
	case VAR_SPECIAL: врати "special";
	case VAR_BOOL: врати "bool";
	case VAR_NUMBER: врати "number";
	case VAR_FLOAT: врати "float";
	case VAR_STRING: врати "string";
	case VAR_BLOB: врати "blob";
	case VAR_JOB: врати "job";
	case VAR_CHANNEL: врати "channel";
	case VAR_LIST: врати "list";
	case VAR_TUPLE: врати "tuple";
	case VAR_DICT: врати "dict";
	case VAR_INSTR: врати "instr";
	case VAR_CLASS: врати "class";
	case VAR_OBJECT: врати "object";
	case VAR_TYPEALIAS: врати "typealias";

	case VAR_FUNC:
	case VAR_PARTIAL: врати "func";
    }
    врати "unknown";
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

    ако (type->tt_member->tt_type == VAR_UNKNOWN)
	member_name = type_name(&t_any, &member_free);
    инако
	member_name = type_name(type->tt_member, &member_free);

    size_t len = STRLEN(name) + STRLEN(member_name) + 3;
    *tofree = alloc(len);
    ако (*tofree == NULL)
    {
	vim_free(member_free);
	врати name;
    }

    vim_snprintf(*tofree, len, "%s<%s>", name, member_name);
    vim_free(member_free);
    врати *tofree;
}

/*
 * Return the type name of a tuple.
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    static char *
type_name_tuple(type_T *type, char **tofree)
{
    garray_T    ga;
    цео		i;
    цео		varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;
    char	*arg_free = NULL;

    ga_init2(&ga, 1, 100);
    ако (ga_grow(&ga, 20) == FAIL)
	goto failed;
    STRCPY(ga.ga_data, "tuple<");
    ga.ga_len += 6;

    ако (type->tt_argcount <= 0)
	// empty tuple
	GA_CONCAT_LITERAL(&ga, "any");
    инако
    {
	ако (type->tt_args == NULL)
	    GA_CONCAT_LITERAL(&ga, "[unknown]");
	инако
	{
	    за (i = 0; i < type->tt_argcount; ++i)
	    {
		string_T    arg_type;

		arg_type.string = (char_u *)type_name(type->tt_args[i], &arg_free);
		ако (i > 0)
		{
		    STRCPY((char *)ga.ga_data + ga.ga_len, ", ");
		    ga.ga_len += 2;
		}
		arg_type.length = STRLEN(arg_type.string);
		ако (ga_grow(&ga, (цео)arg_type.length + 8) == FAIL)
		    goto failed;
		ако (varargs && i == type->tt_argcount - 1)
		    GA_CONCAT_LITERAL(&ga, "...");
		ga_concat_len(&ga, arg_type.string, arg_type.length);
		VIM_CLEAR(arg_free);
	    }
	}
    }

    STRCPY((char *)ga.ga_data + ga.ga_len, ">");
    *tofree = ga.ga_data;
    врати ga.ga_data;

failed:
    vim_free(arg_free);
    ga_clear(&ga);
    врати "[unknown]";
}

/*
 * Return the type name of a Class (class<name>) or Object (object<name>).
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    static char *
type_name_class_or_obj(char *name, type_T *type, char **tofree)
{
    string_T	class_name;

    ако (type->tt_class != NULL)
    {
	class_name.string = type->tt_class->class_name.string;
	class_name.length = type->tt_class->class_name.length;
	ако (IS_ENUM(type->tt_class))
	    name = "enum";
    }
    инако
	STR_LITERAL_SET(class_name, "any");

    size_t len = STRLEN(name) + class_name.length + 3;
    *tofree = alloc(len);
    ако (*tofree == NULL)
	врати name;

    vim_snprintf(*tofree, len, "%s<%s>", name, class_name.string);
    врати *tofree;
}

/*
 * Return the type name of a function.
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    static char *
type_name_func(type_T *type, char **tofree)
{
    garray_T    ga;
    цео		i;
    цео		varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;
    char	*arg_free = NULL;

    ga_init2(&ga, 1, 100);
    ако (ga_grow(&ga, 20) == FAIL)
	goto failed;
    STRCPY(ga.ga_data, "func(");
    ga.ga_len += 5;

    за (i = 0; i < type->tt_argcount; ++i)
    {
	string_T    arg_type;

	ако (type->tt_args == NULL)
	    STR_LITERAL_SET(arg_type, "[unknown]");
	инако
	{
	    arg_type.string = (char_u *)type_name(type->tt_args[i], &arg_free);
	    arg_type.length = STRLEN(arg_type.string);
	}
	ако (i > 0)
	{
	    STRCPY((char *)ga.ga_data + ga.ga_len, ", ");
	    ga.ga_len += 2;
	}
	ако (ga_grow(&ga, (цео)arg_type.length + 8) == FAIL)
	    goto failed;
	ако (varargs && i == type->tt_argcount - 1)
	    GA_CONCAT_LITERAL(&ga, "...");
	инако ако (i >= type->tt_min_argcount)
	    *((char *)ga.ga_data + ga.ga_len++) = '?';
	ga_concat_len(&ga, arg_type.string, arg_type.length);
	VIM_CLEAR(arg_free);
    }
    ако (type->tt_argcount < 0)
	// any number of arguments
	GA_CONCAT_LITERAL(&ga, "...");

    ако (type->tt_member == &t_void)
	STRCPY((char *)ga.ga_data + ga.ga_len, ")");
    инако
    {
	char *ret_free = NULL;
	char *ret_name = type_name(type->tt_member, &ret_free);
	цео  len;

	len = (цео)STRLEN(ret_name) + 4;
	ако (ga_grow(&ga, len) == FAIL)
	{
	    vim_free(ret_free);
	    goto failed;
	}
	STRCPY((char *)ga.ga_data + ga.ga_len, "): ");
	STRCPY((char *)ga.ga_data + ga.ga_len + 3, ret_name);
	vim_free(ret_free);
    }
    *tofree = ga.ga_data;
    врати ga.ga_data;

failed:
    vim_free(arg_free);
    ga_clear(&ga);
    врати "[unknown]";
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
    ако (type == NULL)
	врати "[unknown]";
    name = vartype_name(type->tt_type);

    switch (type->tt_type)
    {
	case VAR_LIST:
	case VAR_DICT:
	    врати type_name_list_or_dict(name, type, tofree);

	case VAR_TUPLE:
	    врати type_name_tuple(type, tofree);

	case VAR_CLASS:
	case VAR_OBJECT:
	    врати type_name_class_or_obj(name, type, tofree);

	case VAR_FUNC:
	    врати type_name_func(type, tofree);

	default:
	    break;
    }

    врати name;
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
    ако (argvars[0].v_type == VAR_TYPEALIAS)
    {
	type = copy_type(argvars[0].vval.v_typealias->ta_type, &type_list);
	// A type alias for a class has the type set to VAR_OBJECT.  Change it
	// to VAR_CLASS, so that the name is "typealias<class<xxx>>"
	ако (type->tt_type == VAR_OBJECT)
	    type->tt_type = VAR_CLASS;
    }
    инако
	type = typval2type(argvars, get_copyID(), &type_list, TVTT_DO_MEMBER);
    name = type_name(type, &tofree);
    ако (argvars[0].v_type == VAR_TYPEALIAS)
    {
	vim_snprintf((char *)IObuff, IOSIZE, "typealias<%s>", name);
	rettv->vval.v_string = vim_strsave((char_u *)IObuff);
	ако (tofree != NULL)
	    vim_free(tofree);
    }
    инако
    {
	ако (tofree != NULL)
	    rettv->vval.v_string = (char_u *)tofree;
	инако
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
    цео
check_typval_is_value(typval_T *tv)
{
    ако (tv == NULL)
	врати OK;

    switch (tv->v_type)
    {
	case VAR_CLASS:
	    {
		class_T *cl = tv->vval.v_class;
		char_u *class_name = (cl == NULL) ? (char_u *)""
							: cl->class_name.string;
		ако (cl != NULL && IS_ENUM(cl))
		    semsg(_(e_using_enum_as_value_str), class_name);
		инако
		    semsg(_(e_using_class_as_value_str), class_name);
	    }
	    врати FAIL;

	case VAR_TYPEALIAS:
	    semsg(_(e_using_typealias_as_value_str),
		    tv->vval.v_typealias->ta_name);
	    врати FAIL;

	default:
	    break;
    }
    врати OK;
}

/*
 * Same as above, except check type_T.
 */
    цео
check_type_is_value(type_T *type)
{
    ако (type == NULL)
	врати OK;
    switch (type->tt_type)
    {
	case VAR_CLASS:
	    ако (type->tt_class != NULL && IS_ENUM(type->tt_class))
		semsg(_(e_using_enum_as_value_str),
			type->tt_class->class_name.string);
	    инако
		semsg(_(e_using_class_as_value_str),
			type->tt_class == NULL ? (char_u *)""
			: type->tt_class->class_name.string);
	    врати FAIL;

	case VAR_TYPEALIAS:
	    // TODO: Not sure what could be done here to get a name.
	    //       Maybe an optional argument?
	    emsg(_(e_using_typealias_as_var_val));
	    врати FAIL;

	default:
	    break;
    }
    врати OK;
}

#endif // FEAT_EVAL
