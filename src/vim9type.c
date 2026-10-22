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

#အကယ်၍ defined(FEAT_EVAL)

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

    အကယ်၍ (ga_grow(type_gap, 1) == FAIL)
	ပြန်ပေး NULL;
    type = ALLOC_CLEAR_ONE(type_T);
    အကယ်၍ (type == NULL)
	ပြန်ပေး NULL;

    ((type_T **)type_gap->ga_data)[type_gap->ga_len] = type;
    ++type_gap->ga_len;
    ပြန်ပေး type;
}

/*
 * Make a shallow copy of "type".
 * When allocation fails returns "type".
 */
    type_T *
copy_type(type_T *type, garray_T *type_gap)
{
    type_T *copy = get_type_ptr(type_gap);

    အကယ်၍ (copy == NULL)
	ပြန်ပေး type;
    *copy = *type;
    copy->tt_flags &= ~TTFLAG_STATIC;

    အကယ်၍ (type->tt_args != NULL
	   && func_type_add_arg_types(copy, type->tt_argcount, type_gap) == OK)
	အတွက် (ကိန်းပြည့် i = 0; i < type->tt_argcount; ++i)
	    copy->tt_args[i] = type->tt_args[i];

    ပြန်ပေး copy;
}

/*
 * Inner part of copy_type_deep().
 * When allocation fails returns "type".
 */
    static type_T *
copy_type_deep_rec(type_T *type, garray_T *type_gap, garray_T *seen_types)
{
    အတွက် (ကိန်းပြည့် i = 0; i < seen_types->ga_len; ++i)
	အကယ်၍ (((type_T **)seen_types->ga_data)[i * 2] == type)
	    // seen this type before, return the copy we made
	    ပြန်ပေး ((type_T **)seen_types->ga_data)[i * 2 + 1];

    type_T *copy = copy_type(type, type_gap);
    အကယ်၍ (ga_grow(seen_types, 1) == FAIL)
	ပြန်ပေး copy;
    ((type_T **)seen_types->ga_data)[seen_types->ga_len * 2] = type;
    ((type_T **)seen_types->ga_data)[seen_types->ga_len * 2 + 1] = copy;
    ++seen_types->ga_len;

    အကယ်၍ (copy->tt_member != NULL)
	copy->tt_member = copy_type_deep_rec(copy->tt_member,
							 type_gap, seen_types);
    အကယ်၍ (type->tt_args != NULL)
	အတွက် (ကိန်းပြည့် i = 0; i < type->tt_argcount; ++i)
	    copy->tt_args[i] = copy_type_deep_rec(copy->tt_args[i],
							 type_gap, seen_types);

    ပြန်ပေး copy;
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
    ပြန်ပေး res;
}

    void
clear_type_list(garray_T *gap)
{
    နေစဉ် (gap->ga_len > 0)
	vim_free(((type_T **)gap->ga_data)[--gap->ga_len]);
    ga_clear(gap);
}

    void
clear_func_type_list(garray_T *gap, type_T **func_type)
{
    နေစဉ် (gap->ga_len > 0)
    {
	// func_type pointing to the uf_type_list, so reset pointer
	အကယ်၍ (*func_type == ((type_T **)gap->ga_data)[--gap->ga_len])
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

    အကယ်၍ (type == NULL)
	ပြန်ပေး NULL;

    // A fixed type never contains allocated types, return as-is.
    အကယ်၍ (type->tt_flags & TTFLAG_STATIC)
	ပြန်ပေး type;

    ret = ALLOC_ONE(type_T);
    အကယ်၍ (ret == NULL)
	ပြန်ပေး NULL;
    *ret = *type;

    အကယ်၍ (ret->tt_member != NULL)
	ret->tt_member = alloc_type(ret->tt_member);

    အကယ်၍ (type->tt_argcount > 0 && type->tt_args != NULL)
    {
	ကိန်းပြည့် i;

	ret->tt_args = ALLOC_MULT(type_T *, type->tt_argcount);
	အကယ်၍ (ret->tt_args != NULL)
	    အတွက် (i = 0; i < type->tt_argcount; ++i)
		ret->tt_args[i] = alloc_type(type->tt_args[i]);
    }
    မဟုတ်ပါက
	ret->tt_args = NULL;

    ပြန်ပေး ret;
}

/*
 * Free a type that was created with alloc_type().
 */
    void
free_type(type_T *type)
{
    ကိန်းပြည့် i;

    အကယ်၍ (type == NULL || (type->tt_flags & TTFLAG_STATIC))
	ပြန်ပေး;
    အကယ်၍ (type->tt_args != NULL)
    {
	အတွက် (i = 0; i < type->tt_argcount; ++i)
	    free_type(type->tt_args[i]);
	vim_free(type->tt_args);
    }

    free_type(type->tt_member);

    vim_free(type);
}

/*
 * Return TRUE if "type" is to be recursed into for setting the type.
 */
    static ကိန်းပြည့်
set_tv_type_recurse(type_T *type)
{
    ပြန်ပေး type->tt_member != NULL
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
    အကယ်၍ (d->dv_type == type)
	ပြန်ပေး;

    free_type(d->dv_type);
    d->dv_type = alloc_type(type);

    // Need to recursively set the type of dict items?
    အကယ်၍ (!set_tv_type_recurse(type))
	ပြန်ပေး;

    ကိန်းပြည့်		todo = (ကိန်းပြည့်)d->dv_hashtab.ht_used;
    hashitem_T	*hi;
    dictitem_T	*di;

    FOR_ALL_HASHTAB_ITEMS(&d->dv_hashtab, hi, todo)
    {
	အကယ်၍ (!HASHITEM_EMPTY(hi))
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
    အကယ်၍ (l->lv_type == type)
	ပြန်ပေး;

    free_type(l->lv_type);
    l->lv_type = alloc_type(type);

    // Need to recursively set the type of list items?
    အကယ်၍ (l->lv_first == &range_list_item || !set_tv_type_recurse(type))
	ပြန်ပေး;

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
    အကယ်၍ (tuple->tv_type == type)
	ပြန်ပေး;

    free_type(tuple->tv_type);
    tuple->tv_type = alloc_type(type);

    အကယ်၍ (type->tt_argcount <= 0)
	ပြန်ပေး;

    // recursively set the type of list items
    type_T	*item_type;
    အတွက် (ကိန်းပြည့် i = 0; i < tuple_len(tuple); i++)
    {
	အကယ်၍ ((type->tt_flags & TTFLAG_VARARGS) && (i >= type->tt_argcount - 1))
	    // For a variadic tuple, the last type is a List.  So use the
	    // List member type.
	    item_type = type->tt_args[type->tt_argcount - 1]->tt_member;
	မဟုတ်ပါက
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
    အကယ်၍ (type->tt_type == VAR_ANY)
	// If the variable type is "any", then keep the value type.
	// e.g.  var x: any = [1, 2] or var y: any = {v: 1}
	ပြန်ပေး;

    အကယ်၍ (tv->v_type == VAR_DICT && tv->vval.v_dict != NULL)
	set_tv_type_dict(tv->vval.v_dict, type);
    မဟုတ်ပါက အကယ်၍ (tv->v_type == VAR_LIST && tv->vval.v_list != NULL)
	set_tv_type_list(tv->vval.v_list, type);
    မဟုတ်ပါက အကယ်၍ (tv->v_type == VAR_TUPLE && tv->vval.v_tuple != NULL)
	set_tv_type_tuple(tv->vval.v_tuple, type);
}

/*
 * For a tuple type, reserve space for "typecount" types (including the
 * repeated type).
 */
    static ကိန်းပြည့်
tuple_type_add_types(
    type_T	*tupletype,
    ကိန်းပြည့်		typecount,
    garray_T	*type_gap)
{
    // To make it easy to free the space needed for the types, add the
    // pointer to type_gap.
    အကယ်၍ (ga_grow(type_gap, 1) == FAIL)
	ပြန်ပေး FAIL;
    tupletype->tt_args = alloc_clear_id(parimaa_na(type_T *) * typecount,
							  aid_tuple_type_args);
    အကယ်၍ (tupletype->tt_args == NULL)
	ပြန်ပေး FAIL;
    ((type_T **)type_gap->ga_data)[type_gap->ga_len] =
						(void *)tupletype->tt_args;
    ++type_gap->ga_len;
    ပြန်ပေး OK;
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
    အကယ်၍ (member_type == NULL || (member_type->tt_type == VAR_ANY
					&& !IS_GENERIC_TYPE(member_type)))
	ပြန်ပေး &t_list_any;
    အကယ်၍ (member_type->tt_type == VAR_VOID
	    || member_type->tt_type == VAR_UNKNOWN)
	ပြန်ပေး &t_list_empty;
    အကယ်၍ (member_type->tt_type == VAR_BOOL)
	ပြန်ပေး &t_list_bool;
    အကယ်၍ (member_type->tt_type == VAR_NUMBER)
	ပြန်ပေး &t_list_number;
    အကယ်၍ (member_type->tt_type == VAR_STRING)
	ပြန်ပေး &t_list_string;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    အကယ်၍ (type == NULL)
	ပြန်ပေး &t_any;
    type->tt_type = VAR_LIST;
    type->tt_member = member_type;
    type->tt_argcount = 0;
    type->tt_args = NULL;
    ပြန်ပေး type;
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
    ကိန်းပြည့်		typecount = tuple_types_gap->ga_len;

    // recognize commonly used types
    အကယ်၍ (typecount == 0)
	ပြန်ပေး &t_tuple_any;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    အကယ်၍ (type == NULL)
	ပြန်ပေး &t_any;
    type->tt_type = VAR_TUPLE;
    type->tt_member = NULL;
    အကယ်၍ (typecount > 0)
    {
	အကယ်၍ (tuple_type_add_types(type, typecount, type_gap) == FAIL)
	    ပြန်ပေး &t_any;
	mch_memmove(type->tt_args, tuple_types, parimaa_na(type_T *) * typecount);
    }
    type->tt_argcount = typecount;
    type->tt_flags = 0;

    ပြန်ပေး type;
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
    အကယ်၍ (member_type == NULL || (member_type->tt_type == VAR_ANY
					&& !IS_GENERIC_TYPE(member_type)))
	ပြန်ပေး &t_dict_any;
    အကယ်၍ (member_type->tt_type == VAR_VOID
	    || member_type->tt_type == VAR_UNKNOWN)
	ပြန်ပေး &t_dict_empty;
    အကယ်၍ (member_type->tt_type == VAR_BOOL)
	ပြန်ပေး &t_dict_bool;
    အကယ်၍ (member_type->tt_type == VAR_NUMBER)
	ပြန်ပေး &t_dict_number;
    အကယ်၍ (member_type->tt_type == VAR_STRING)
	ပြန်ပေး &t_dict_string;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    အကယ်၍ (type == NULL)
	ပြန်ပေး &t_any;
    type->tt_type = VAR_DICT;
    type->tt_member = member_type;
    type->tt_argcount = 0;
    type->tt_args = NULL;
    ပြန်ပေး type;
}

/*
 * Allocate a new type for a function.
 */
    type_T *
alloc_func_type(type_T *ret_type, ကိန်းပြည့် argcount, garray_T *type_gap)
{
    type_T *type = get_type_ptr(type_gap);

    အကယ်၍ (type == NULL)
	ပြန်ပေး &t_any;
    type->tt_type = VAR_FUNC;
    type->tt_member = ret_type == NULL ? &t_unknown : ret_type;
    type->tt_argcount = argcount;
    type->tt_args = NULL;
    ပြန်ပေး type;
}

/*
 * Allocate a new type for a tuple.
 */
    static type_T *
alloc_tuple_type(ကိန်းပြည့် typecount, garray_T *type_gap)
{
    type_T *type = get_type_ptr(type_gap);

    အကယ်၍ (type == NULL)
	ပြန်ပေး &t_any;
    type->tt_type = VAR_TUPLE;
    type->tt_member = NULL;
    type->tt_argcount = typecount;
    type->tt_args = NULL;
    ပြန်ပေး type;
}

/*
 * Get a function type, based on the return type "ret_type".
 * "argcount" must be -1 or 0, a predefined type can be used.
 */
    type_T *
get_func_type(type_T *ret_type, ကိန်းပြည့် argcount, garray_T *type_gap)
{
    // recognize commonly used types
    အကယ်၍ (ret_type == &t_unknown || ret_type == NULL)
    {
	// (argcount == 0) is not possible
	ပြန်ပေး &t_func_unknown;
    }
    အကယ်၍ (ret_type == &t_void)
    {
	အကယ်၍ (argcount == 0)
	    ပြန်ပေး &t_func_0_void;
	မဟုတ်ပါက
	    ပြန်ပေး &t_func_void;
    }
    အကယ်၍ (ret_type == &t_any)
    {
	အကယ်၍ (argcount == 0)
	    ပြန်ပေး &t_func_0_any;
	မဟုတ်ပါက
	    ပြန်ပေး &t_func_any;
    }
    အကယ်၍ (ret_type == &t_number)
    {
	အကယ်၍ (argcount == 0)
	    ပြန်ပေး &t_func_0_number;
	မဟုတ်ပါက
	    ပြန်ပေး &t_func_number;
    }
    အကယ်၍ (ret_type == &t_string)
    {
	အကယ်၍ (argcount == 0)
	    ပြန်ပေး &t_func_0_string;
	မဟုတ်ပါက
	    ပြန်ပေး &t_func_string;
    }

    ပြန်ပေး alloc_func_type(ret_type, argcount, type_gap);
}

/*
 * For a function type, reserve space for "argcount" argument types (including
 * vararg).
 */
    ကိန်းပြည့်
func_type_add_arg_types(
	type_T	    *functype,
	ကိန်းပြည့်	    argcount,
	garray_T    *type_gap)
{
    // To make it easy to free the space needed for the argument types, add the
    // pointer to type_gap.
    အကယ်၍ (ga_grow(type_gap, 1) == FAIL)
	ပြန်ပေး FAIL;
    functype->tt_args = ALLOC_CLEAR_MULT(type_T *, argcount);
    အကယ်၍ (functype->tt_args == NULL)
	ပြန်ပေး FAIL;
    ((type_T **)type_gap->ga_data)[type_gap->ga_len] =
						     (void *)functype->tt_args;
    ++type_gap->ga_len;
    ပြန်ပေး OK;
}

/*
 * Return TRUE if "type" is NULL, any or unknown.
 * This also works for const (comparing with &t_any and &t_unknown doesn't).
 */
    ကိန်းပြည့်
type_any_or_unknown(type_T *type)
{
    ပြန်ပေး type == NULL || type->tt_type == VAR_ANY
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
	    ပြန်ပေး &t_null;

	pak_sa VVAL_NONE:
	    ပြန်ပေး &t_none;

	pak_sa VVAL_TRUE:
	pak_sa VVAL_FALSE:
	    ပြန်ပေး &t_bool;

	default:
	    ပြန်ပေး &t_unknown;
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
list_typval2type(typval_T *tv, ကိန်းပြည့် copyID, garray_T *type_gap, ကိန်းပြည့် flags)
{
    list_T	*l = tv->vval.v_list;
    listitem_T	*li;
    type_T	*member_type = NULL;

    // An empty list has type list<unknown>, unless the type was specified
    // and is not list<any>.  This matters when assigning to a variable
    // with a specific list type.
    အကယ်၍ (l == NULL || (l->lv_first == NULL
		&& (l->lv_type == NULL || l->lv_type->tt_member == &t_any)))
	ပြန်ပေး &t_list_empty;

    အကယ်၍ ((flags & TVTT_DO_MEMBER) == 0)
	ပြန်ပေး &t_list_any;

    // If the type is list<any> go through the members, it may end up a
    // more specific type.
    အကယ်၍ (l->lv_type != NULL && (l->lv_first == NULL
		|| (flags & TVTT_MORE_SPECIFIC) == 0
		|| l->lv_type->tt_member != &t_any))
	// make a copy, lv_type may be freed if the list is freed
	ပြန်ပေး copy_type_deep(l->lv_type, type_gap);

    အကယ်၍ (l->lv_first == &range_list_item)
	ပြန်ပေး &t_list_number;

    အကယ်၍ (l->lv_copyID == copyID)
	// avoid recursion
	ပြန်ပေး &t_list_any;

    l->lv_copyID = copyID;

    // Use the common type of all members.
    member_type = typval2type(&l->lv_first->li_tv, copyID, type_gap,
							TVTT_DO_MEMBER);
    အတွက် (li = l->lv_first->li_next; li != NULL; li = li->li_next)
	common_type(typval2type(&li->li_tv, copyID, type_gap, TVTT_DO_MEMBER),
					member_type, &member_type, type_gap);

    // Reset copyID so that a shared reference to this list (not a circular
    // reference) can be processed again to get the correct type.
    l->lv_copyID = 0;

    ပြန်ပေး get_list_type(member_type, type_gap);
}

/*
 * Get a type_T for a Tuple typval in "tv".
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    static type_T *
tuple_typval2type(typval_T *tv, ကိန်းပြည့် copyID, garray_T *type_gap, ကိန်းပြည့် flags)
{
    tuple_T	*tuple = tv->vval.v_tuple;
    ကိန်းပြည့်		len = tuple_len(tuple);
    type_T	*type = NULL;

    // An empty tuple has type tuple<unknown>, unless the type was specified
    // and is not tuple<any>.  This matters when assigning to a variable
    // with a specific tuple type.
    အကယ်၍ (tuple == NULL || (len == 0 && (tuple->tv_type == NULL
				|| tuple->tv_type->tt_argcount == 0)))
	ပြန်ပေး &t_tuple_empty;

    အကယ်၍ ((flags & TVTT_DO_MEMBER) == 0)
	ပြန်ပေး &t_tuple_any;

    // If the type is tuple<any> go through the members, it may end up a
    // more specific type.
    အကယ်၍ (tuple->tv_type != NULL && (len == 0
					|| (flags & TVTT_MORE_SPECIFIC) == 0))
	// make a copy, tv_type may be freed if the tuple is freed
	ပြန်ပေး copy_type_deep(tuple->tv_type, type_gap);

    အကယ်၍ (tuple->tv_copyID == copyID)
	// avoid recursion
	ပြန်ပေး &t_tuple_any;

    tuple->tv_copyID = copyID;

    garray_T	tuple_types_ga;
    ga_init2(&tuple_types_ga, parimaa_na(type_T *), 10);
    အတွက် (ကိန်းပြည့် i = 0; i < len; i++)
    {
	type = typval2type(TUPLE_ITEM(tuple, i), copyID, type_gap,
							TVTT_DO_MEMBER);
	အကယ်၍ (ga_grow(&tuple_types_ga, 1) == FAIL)
	{
	    ga_clear(&tuple_types_ga);
	    // Reset copyID so that a shared reference to this tuple can be
	    // processed again.
	    tuple->tv_copyID = 0;
	    ပြန်ပေး NULL;
	}
	((type_T **)tuple_types_ga.ga_data)[tuple_types_ga.ga_len] = type;
	tuple_types_ga.ga_len++;
    }

    type_T *tuple_type = get_tuple_type(&tuple_types_ga, type_gap);
    ga_clear(&tuple_types_ga);

    // Reset copyID so that a shared reference to this tuple (not a circular
    // reference) can be processed again to get the correct type.
    tuple->tv_copyID = 0;

    ပြန်ပေး tuple_type;
}

/*
 * Get a type_T for a Dict typval in "tv".
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    static type_T *
dict_typval2type(typval_T *tv, ကိန်းပြည့် copyID, garray_T *type_gap, ကိန်းပြည့် flags)
{
    dict_iterator_T	iter;
    typval_T		*value;
    dict_T		*d = tv->vval.v_dict;
    type_T		*member_type = NULL;

    အကယ်၍ (d == NULL || (d->dv_hashtab.ht_used == 0 && d->dv_type == NULL))
	ပြန်ပေး &t_dict_empty;

    အကယ်၍ ((flags & TVTT_DO_MEMBER) == 0)
	ပြန်ပေး &t_dict_any;

    // If the type is dict<any> go through the members, it may end up a
    // more specific type.
    အကယ်၍ (d->dv_type != NULL && (d->dv_hashtab.ht_used == 0
		|| (flags & TVTT_MORE_SPECIFIC) == 0
		|| d->dv_type->tt_member != &t_any))
	ပြန်ပေး d->dv_type;

    အကယ်၍ (d->dv_copyID == copyID)
	// avoid recursion
	ပြန်ပေး &t_dict_any;

    d->dv_copyID = copyID;

    // Use the common type of all values.
    dict_iterate_start(tv, &iter);
    dict_iterate_next(&iter, &value);
    member_type = typval2type(value, copyID, type_gap, TVTT_DO_MEMBER);

    နေစဉ် (dict_iterate_next(&iter, &value) != NULL)
	common_type(typval2type(value, copyID, type_gap, TVTT_DO_MEMBER),
					member_type, &member_type, type_gap);

    // Reset copyID so that a shared reference to this dict (not a circular
    // reference) can be processed again to get the correct type.
    d->dv_copyID = 0;

    ပြန်ပေး get_dict_type(member_type, type_gap);
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
    အကယ်၍ (type == NULL)
	ပြန်ပေး NULL;

    *type = *ufunc->uf_func_type;
    အကယ်၍ (type->tt_argcount >= 0 && pt->pt_argc > 0)
    {
	type->tt_argcount -= pt->pt_argc;
	type->tt_min_argcount -= pt->pt_argc;
	အကယ်၍ (type->tt_argcount > 0 && func_type_add_arg_types(type,
		    type->tt_argcount, type_gap) == OK)
	    အတွက် (ကိန်းပြည့် i = 0; i < type->tt_argcount; ++i)
		type->tt_args[i] =
		    ufunc->uf_func_type->tt_args[i + pt->pt_argc];
    }
    အကယ်၍ (pt->pt_func != NULL)
	type->tt_member = pt->pt_func->uf_ret_type;

    ပြန်ပေး type;
}

/*
 * Get a type_T for a "class" or an "object" typval in "tv".
 */
    static type_T *
oc_typval2type(typval_T *tv)
{
    အကယ်၍ (tv->v_type == VAR_CLASS)
    {
	အကယ်၍ (tv->vval.v_class == NULL)
	    ပြန်ပေး &t_class;

	ပြန်ပေး &tv->vval.v_class->class_type;
    }

    အကယ်၍ (tv->vval.v_object != NULL)
	ပြန်ပေး &tv->vval.v_object->obj_class->class_object_type;

    ပြန်ပေး &t_object_any;
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
    ကိန်းပြည့်		argcount = 0;
    ကိန်းပြည့်		min_argcount = 0;

    အကယ်၍ (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != NULL)
    {
	အကယ်၍ (tv->vval.v_partial->pt_func != NULL)
	    ufunc = tv->vval.v_partial->pt_func;
	မဟုတ်ပါက
	    name = tv->vval.v_partial->pt_name;
    }
    မဟုတ်ပါက
	name = tv->vval.v_string;

    အကယ်၍ (name == NULL && ufunc == NULL)
	ပြန်ပေး &t_func_unknown;

    အကယ်၍ (name != NULL)
    {
	ကိန်းပြည့် idx = find_internal_func(name);

	အကယ်၍ (idx >= 0)
	{
	    type_T *decl_type;  // unused

	    internal_func_get_argcount(idx, &argcount, &min_argcount);
	    member_type = internal_func_ret_type(idx, 0, NULL, &decl_type,
		    type_gap);
	}
	မဟုတ်ပါက
	{
	    // Check if name contains "<".  If it does, then replace "<" with
	    // NUL and lookup the function and then look up the specific
	    // generic function using the supplied types.
	    char_u *p = generic_func_find_open_bracket(name);
	    အကယ်၍ (p == NULL)
		ufunc = find_func(name, FALSE);
	    မဟုတ်ပါက
	    {
		// generic function
		char_u	cc = *p;

		*p = NUL;
		ufunc = find_func(name, FALSE);
		*p = cc;
		အကယ်၍ (ufunc != NULL && IS_GENERIC_FUNC(ufunc))
		{
		    ufunc = find_generic_func(ufunc, name, &p);
		    အကယ်၍ (ufunc == NULL)
			ပြန်ပေး NULL;
		}
	    }
	}
    }
    အကယ်၍ (ufunc != NULL)
    {
	// May need to get the argument types from default values by
	// compiling the function.
	အကယ်၍ (ufunc->uf_def_status == UF_TO_BE_COMPILED
		&& compile_def_function(ufunc, TRUE, CT_NONE, NULL)
		== FAIL)
	    ပြန်ပေး NULL;
	အကယ်၍ (ufunc->uf_func_type == NULL)
	    set_function_type(ufunc);
	အကယ်၍ (ufunc->uf_func_type != NULL)
	{
	    အကယ်၍ (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != NULL)
		ပြန်ပေး partial_typval2type(tv, ufunc, type_gap);
	    ပြန်ပေး ufunc->uf_func_type;
	}
    }

    type = get_type_ptr(type_gap);
    အကယ်၍ (type == NULL)
	ပြန်ပေး NULL;
    type->tt_type = tv->v_type;
    type->tt_argcount = argcount;
    type->tt_min_argcount = min_argcount;
    အကယ်၍ (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != NULL
					&& tv->vval.v_partial->pt_argc > 0)
    {
	type->tt_argcount -= tv->vval.v_partial->pt_argc;
	type->tt_min_argcount -= tv->vval.v_partial->pt_argc;
    }
    type->tt_member = member_type;

    ပြန်ပေး type;
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
typval2type_int(typval_T *tv, ကိန်းပြည့် copyID, garray_T *type_gap, ကိန်းပြည့် flags)
{
    switch (tv->v_type)
    {
	pak_sa VAR_UNKNOWN:
	    ပြန်ပေး &t_unknown;

	pak_sa VAR_ANY:
	    ပြန်ပေး &t_any;

	pak_sa VAR_VOID:
	    ပြန်ပေး &t_void;

	pak_sa VAR_BOOL:
	    ပြန်ပေး &t_bool;

	pak_sa VAR_SPECIAL:
	    ပြန်ပေး special_typval2type(tv);

	pak_sa VAR_NUMBER:
	    ပြန်ပေး &t_number;

	pak_sa VAR_FLOAT:
	    ပြန်ပေး &t_float;

	pak_sa VAR_STRING:
	    ပြန်ပေး &t_string;

	pak_sa VAR_BLOB:
	    အကယ်၍ (tv->vval.v_blob == NULL)
		ပြန်ပေး &t_blob_null;
	    ပြန်ပေး &t_blob;

	pak_sa VAR_LIST:
	    ပြန်ပေး list_typval2type(tv, copyID, type_gap, flags);

	pak_sa VAR_TUPLE:
	    ပြန်ပေး tuple_typval2type(tv, copyID, type_gap, flags);

	pak_sa VAR_DICT:
	    ပြန်ပေး dict_typval2type(tv, copyID, type_gap, flags);

	pak_sa VAR_JOB:
	    ပြန်ပေး &t_job;

	pak_sa VAR_CHANNEL:
	    ပြန်ပေး &t_channel;

	pak_sa VAR_CLASS:
	pak_sa VAR_OBJECT:
	    ပြန်ပေး oc_typval2type(tv);

	pak_sa VAR_TYPEALIAS:
	    ပြန်ပေး &t_typealias;

	pak_sa VAR_FUNC:
	pak_sa VAR_PARTIAL:
	    ပြန်ပေး fp_typval2type(tv, type_gap);

	pak_sa VAR_INSTR:
	default:
	    bha_nga;
    }

    ပြန်ပေး NULL;
}

/*
 * Return TRUE if "tv" is not a bool but should be converted to bool.
 */
    ကိန်းပြည့်
need_convert_to_bool(type_T *type, typval_T *tv)
{
    ပြန်ပေး type != NULL && type == &t_bool && tv->v_type != VAR_BOOL
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
typval2type(typval_T *tv, ကိန်းပြည့် copyID, garray_T *type_gap, ကိန်းပြည့် flags)
{
    type_T *type = typval2type_int(tv, copyID, type_gap, flags);

    အကယ်၍ (type == NULL)
	ပြန်ပေး NULL;

    အကယ်၍ (type != &t_bool && (tv->v_type == VAR_NUMBER
		&& (tv->vval.v_number == 0 || tv->vval.v_number == 1)))
	// Number 0 and 1 and expression with "&&" or "||" can also be used
	// for bool.
	type = &t_number_bool;
    မဟုတ်ပါက အကယ်၍ (type != &t_float && tv->v_type == VAR_NUMBER)
	// A number can also be used for float.
	type = &t_number_float;
    ပြန်ပေး type;
}

/*
 * Return TRUE if "type" can be used for a variable declaration.
 * Give an error and return FALSE if not.
 */
    ကိန်းပြည့်
valid_declaration_type(type_T *type)
{
    အကယ်၍ (type->tt_type == VAR_SPECIAL  // null, none
	    || type->tt_type == VAR_VOID)
    {
	var_na *tofree = NULL;
	var_na *name = type_name(type, &tofree);
	semsg(_(e_invalid_type_in_variable_declaration_str), name);
	vim_free(tofree);
	ပြန်ပေး FALSE;
    }
    ပြန်ပေး TRUE;
}

/*
 * Get a type_T for a typval_T, used for v: variables.
 * "type_list" is used to temporarily create types in.
 */
    type_T *
typval2type_vimvar(typval_T *tv, garray_T *type_gap)
{
    အကယ်၍ (tv->v_type == VAR_LIST)  // e.g. for v:oldfiles
	ပြန်ပေး &t_list_string;
    အကယ်၍ (tv->v_type == VAR_DICT)  // e.g. for v:event
	ပြန်ပေး &t_dict_any;
    ပြန်ပေး typval2type(tv, get_copyID(), type_gap, TVTT_DO_MEMBER);
}

    ကိန်းပြည့်
check_typval_arg_type(
	type_T	    *expected,
	typval_T    *actual_tv,
	var_na	    *func_name,
	ကိန်းပြည့်	    arg_idx)
{
    where_T	where = WHERE_INIT;

    အကယ်၍ (arg_idx > 0)
    {
	where.wt_index = arg_idx;
	where.wt_kind = WT_ARGUMENT;
    }
    where.wt_func_name = func_name;
    ပြန်ပေး check_typval_type(expected, actual_tv, where);
}

/*
 * Return true if "expected" is list<any> or dict<any> and "actual_tv" is a
 * list or dict of that kind, which matches whatever its values are.
 */
    static bool
matches_listdict_any(type_T *expected, typval_T *actual_tv)
{
    ပြန်ပေး (expected->tt_type == VAR_LIST || expected->tt_type == VAR_DICT)
	    && actual_tv->v_type == expected->tt_type
	    && expected->tt_member != NULL
	    && (expected->tt_member->tt_type == VAR_ANY
		|| expected->tt_member->tt_type == VAR_UNKNOWN);
}

/*
 * Return FAIL if "expected" and "actual" don't match.
 * When "argidx" > 0 it is included in the error message.
 */
    ကိန်းပြည့်
check_typval_type(type_T *expected, typval_T *actual_tv, where_T where)
{
    garray_T	type_list;
    type_T	*actual_type;
    ကိန်းပြည့်		res = FAIL;

    အကယ်၍ (expected == NULL)
	ပြန်ပေး OK;  // didn't expect anything.

    // No need to go through the values of a possibly big list or dict.
    အကယ်၍ (matches_listdict_any(expected, actual_tv))
	ပြန်ပေး OK;

    ga_init2(&type_list, parimaa_na(type_T *), 10);

    // A null_function and null_partial are special cases, they can be used to
    // clear a variable.
    အကယ်၍ ((actual_tv->v_type == VAR_FUNC && actual_tv->vval.v_string == NULL)
	    || (actual_tv->v_type == VAR_PARTIAL
					 && actual_tv->vval.v_partial == NULL))
	actual_type = &t_func_unknown;
    မဟုတ်ပါက
	// When the actual type is list<any> or dict<any> go through the values
	// to possibly get a more specific type.
	actual_type = typval2type(actual_tv, get_copyID(), &type_list,
					  TVTT_DO_MEMBER | TVTT_MORE_SPECIFIC);
    အကယ်၍ (actual_type != NULL)
    {
	res = check_type_maybe(expected, actual_type, TRUE, where);
	အကယ်၍ (res == MAYBE && !(actual_type->tt_type == VAR_FUNC
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
    ပြန်ပေး res;
}

    void
arg_type_mismatch(type_T *expected, type_T *actual, ကိန်းပြည့် arg_idx)
{
    where_T	where = WHERE_INIT;

    အကယ်၍ (arg_idx > 0)
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
	    အကယ်၍ (where.wt_func_name == NULL)
		semsg(_(e_variable_nr_type_mismatch_expected_str_but_got_str),
			where.wt_index, typename1, typename2);
	    မဟုတ်ပါက
		semsg(_(e_variable_nr_type_mismatch_expected_str_but_got_str_in_str),
			where.wt_index, typename1, typename2, where.wt_func_name);
	    bha_nga;
	pak_sa WT_ARGUMENT:
	    အကယ်၍ (where.wt_func_name == NULL)
		semsg(_(e_argument_nr_type_mismatch_expected_str_but_got_str),
			where.wt_index, typename1, typename2);
	    မဟုတ်ပါက
		semsg(_(e_argument_nr_type_mismatch_expected_str_but_got_str_in_str),
			where.wt_index, typename1, typename2, where.wt_func_name);
	    bha_nga;
	pak_sa WT_CAST:
	pak_sa WT_UNKNOWN:
	    အကယ်၍ (where.wt_func_name == NULL)
		semsg(_(e_type_mismatch_expected_str_but_got_str),
			typename1, typename2);
	    မဟုတ်ပါက
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
    static ကိန်းပြည့်
check_tuple_type_maybe(
    type_T	*expected,
    type_T	*actual,
    where_T	where)
{
    အကယ်၍ (expected->tt_argcount == -1 || actual->tt_argcount == -1
		|| expected->tt_args == NULL || actual->tt_args == NULL)
	ပြန်ပေး OK;

    // For a non-variadic tuple, the number of items must match
    အကယ်၍ (!(expected->tt_flags & TTFLAG_VARARGS)
	    && expected->tt_argcount != actual->tt_argcount)
	ပြန်ပေး FAIL;

    // compare the type of each tuple item
    အတွက် (ကိန်းပြည့် i = 0; i < actual->tt_argcount; ++i)
    {
	type_T	*exp_type;
	type_T	*actual_type;

	အကယ်၍ (expected->tt_flags & TTFLAG_VARARGS)
	{
	    အကယ်၍ (i < expected->tt_argcount - 1)
		exp_type = expected->tt_args[i];
	    မဟုတ်ပါက
		// For a variadic tuple, the last type is a List.  So use the
		// List member type.
		exp_type = expected->tt_args[expected->tt_argcount - 1]->tt_member;
	}
	မဟုတ်ပါက
	    exp_type = expected->tt_args[i];

	အကယ်၍ (actual->tt_flags & TTFLAG_VARARGS)
	{
	    အကယ်၍ (i < actual->tt_argcount - 1)
		actual_type = actual->tt_args[i];
	    မဟုတ်ပါက
		// For a variadic tuple, the last type is a List.  So use the
		// List member type.
		actual_type = actual->tt_args[actual->tt_argcount - 1]->tt_member;
	}
	မဟုတ်ပါက
	    actual_type = actual->tt_args[i];

	// Allow for using "any" type for a tuple item
	အကယ်၍ (actual->tt_args[i] != &t_any && check_type(exp_type, actual_type,
						FALSE, where) == FAIL)
	    ပြန်ပေး FAIL;
    }

    ပြန်ပေး OK;
}

/*
 * Check if the expected and actual types match for a function
 * Returns OK if "expected" and "actual" are matching function types.
 * Returns FAIL if "expected" and "actual" are different types.
 * Returns MAYBE when a runtime type check is needed.
 */
    static ကိန်းပြည့်
check_func_type_maybe(
    type_T	*expected,
    type_T	*actual,
    where_T	where)
{
    ကိန်းပြည့် ret = OK;

    // If the return type is unknown it can be anything, including
    // nothing, thus there is no point in checking.
    အကယ်၍ (expected->tt_member != &t_unknown)
    {
	အကယ်၍ (actual->tt_member != NULL
		&& actual->tt_member != &t_unknown)
	{
	    where_T  func_where = where;

	    func_where.wt_kind = WT_METHOD_RETURN;
	    ret = check_type_maybe(expected->tt_member,
		    actual->tt_member, FALSE,
		    func_where);
	}
	မဟုတ်ပါက
	    ret = MAYBE;
    }
    အကယ်၍ (ret != FAIL
	    && ((expected->tt_flags & TTFLAG_VARARGS)
		!= (actual->tt_flags & TTFLAG_VARARGS))
	    && expected->tt_argcount != -1)
	ret = FAIL;
    အကယ်၍ (ret != FAIL && expected->tt_argcount != -1
	    && actual->tt_min_argcount != -1
	    && (actual->tt_argcount == -1
		|| (actual->tt_argcount < expected->tt_min_argcount
		    || actual->tt_argcount > expected->tt_argcount)))
	ret = FAIL;
    အကယ်၍ (ret != FAIL && expected->tt_args != NULL
	    && actual->tt_args != NULL)
    {
	ကိန်းပြည့် i;

	အတွက် (i = 0; i < expected->tt_argcount
		&& i < actual->tt_argcount; ++i)
	{
	    where_T  func_where = where;
	    func_where.wt_kind = WT_METHOD_ARG;

	    // Allow for using "any" argument type, lambda's have them.
	    အကယ်၍ (actual->tt_args[i] != &t_any && check_type(
			expected->tt_args[i], actual->tt_args[i], FALSE,
			func_where) == FAIL)
	    {
		ret = FAIL;
		bha_nga;
	    }
	}
    }
    အကယ်၍ (ret == OK && expected->tt_argcount >= 0
	    && actual->tt_argcount == -1)
	// check the argument count at runtime
	ret = MAYBE;

    ပြန်ပေး ret;
}

/*
 * Check if the expected and actual types match for an object
 * Returns OK if "expected" and "actual" are matching object types.
 * Returns FAIL if "expected" and "actual" are different types.
 * Returns MAYBE when a runtime type check is needed.
 */
    static ကိန်းပြည့်
check_object_type_maybe(
    type_T	*expected,
    type_T	*actual,
    where_T	where)
{
    ကိန်းပြည့် ret = OK;

    အကယ်၍ (actual->tt_type == VAR_ANY)
	ပြန်ပေး MAYBE;	// use runtime type check
    အကယ်၍ (actual->tt_type != VAR_OBJECT)
	ပြန်ပေး FAIL;	// don't use tt_class
    အကယ်၍ (actual->tt_class == NULL)    // null object
	ပြန်ပေး OK;
    // t_object_any matches any object except for an enum item
    အကယ်၍ (expected == &t_object_any && !IS_ENUM(actual->tt_class))
	ပြန်ပေး OK;

    // For object method arguments, do a invariant type check in
    // an extended class.  For all others, do a covariance type check.
    အကယ်၍ (where.wt_kind == WT_METHOD_ARG)
    {
	အကယ်၍ (actual->tt_class != expected->tt_class)
	    ret = FAIL;
    }
    မဟုတ်ပါက အကယ်၍ (!class_instance_of(actual->tt_class, expected->tt_class))
    {
	// Check if this is an up-cast, if so we'll have to check the type at
	// runtime.
	အကယ်၍ (where.wt_kind == WT_CAST &&
		class_instance_of(expected->tt_class, actual->tt_class))
	    ret = MAYBE;
	မဟုတ်ပါက
	    ret = FAIL;
    }

    ပြန်ပေး ret;
}

/*
 * Check if the expected and actual types match.
 * Does not allow for assigning "any" to a specific type.
 * When "argidx" > 0 it is included in the error message.
 * Return OK if types match.
 * Return FAIL if types do not match.
 */
    ကိန်းပြည့်
check_type(
	type_T	*expected,
	type_T	*actual,
	ကိန်းပြည့်	give_msg,
	where_T where)
{
    ကိန်းပြည့် ret = check_type_maybe(expected, actual, give_msg, where);

    ပြန်ပေး ret == MAYBE ? OK : ret;
}

/*
 * As check_type() but return MAYBE when a runtime type check should be used
 * when compiling.
 */
    ကိန်းပြည့်
check_type_maybe(
	type_T	*expected,
	type_T	*actual,
	ကိန်းပြည့်	give_msg,
	where_T where)
{
    ကိန်းပြည့် ret = OK;

    // When expected is "unknown" we accept any actual type.
    // When expected is "any" we accept any actual type except "void".
    အကယ်၍ (expected->tt_type != VAR_UNKNOWN
	    && !(expected->tt_type == VAR_ANY && actual->tt_type != VAR_VOID))

    {
	// tt_type should match, except that a "partial" can be assigned to a
	// variable with type "func".
	// And "unknown" (using global variable) and "any" need a runtime type
	// check.
	အကယ်၍ (!(expected->tt_type == actual->tt_type
		    || actual->tt_type == VAR_UNKNOWN
		    || actual->tt_type == VAR_ANY
		    || (expected->tt_type == VAR_FUNC
					   && actual->tt_type == VAR_PARTIAL)))
	{
	    အကယ်၍ (expected->tt_type == VAR_BOOL
					&& (actual->tt_flags & TTFLAG_BOOL_OK))
		// Using number 0 or 1 for bool is OK.
		ပြန်ပေး OK;
	    အကယ်၍ (expected->tt_type == VAR_FLOAT
		    && actual->tt_type == VAR_NUMBER
		    && ((expected->tt_flags & TTFLAG_NUMBER_OK)
			     || (actual->tt_flags & TTFLAG_FLOAT_OK)))
		// Using a number where a float is expected is OK here.
		ပြန်ပေး OK;
	    အကယ်၍ (expected->tt_type == VAR_LIST
		    && actual->tt_type == VAR_TUPLE
		    && (expected->tt_flags & TTFLAG_TUPLE_OK))
		// Using a tuple where a list is expected is OK here.
		ပြန်ပေး OK;
	    အကယ်၍ (give_msg)
		type_mismatch_where(expected, actual, where);
	    ပြန်ပေး FAIL;
	}
	အကယ်၍ (expected->tt_type == VAR_DICT || expected->tt_type == VAR_LIST)
	{
	    // "unknown" is used for an empty list or dict
	    အကယ်၍ (actual->tt_member != NULL && actual->tt_member != &t_unknown)
		ret = check_type_maybe(expected->tt_member, actual->tt_member,
								 FALSE, where);
	}
	မဟုတ်ပါက အကယ်၍ (expected->tt_type == VAR_TUPLE && actual != &t_any)
	    ret =  check_tuple_type_maybe(expected, actual, where);
	မဟုတ်ပါက အကယ်၍ (expected->tt_type == VAR_FUNC && actual != &t_any)
	    ret = check_func_type_maybe(expected, actual, where);
	မဟုတ်ပါက အကယ်၍ (expected->tt_type == VAR_OBJECT)
	    ret = check_object_type_maybe(expected, actual, where);

	အကယ်၍ (ret == FAIL && give_msg)
	    type_mismatch_where(expected, actual, where);
    }

    အကယ်၍ (ret == OK && expected->tt_type != VAR_UNKNOWN
	    && expected->tt_type != VAR_ANY
	    && (actual->tt_type == VAR_UNKNOWN || actual->tt_type == VAR_ANY))
	// check the type at runtime
	ret = MAYBE;

    ပြန်ပေး ret;
}

/*
 * Check that the arguments of "type" match "argvars[argcount]".
 * "base_tv" is from "expr->Func()".
 * Return OK/FAIL.
 */
    ကိန်းပြည့်
check_argument_types(
	type_T	    *type,
	typval_T    *argvars,
	ကိန်းပြည့်	    argcount,
	typval_T    *base_tv,
	char_u	    *name)
{
    ကိန်းပြည့်	    varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;
    ကိန်းပြည့်	    i;
    ကိန်းပြည့်	    totcount = argcount + (base_tv == NULL ? 0 : 1);

    အကယ်၍ (type->tt_type != VAR_FUNC && type->tt_type != VAR_PARTIAL)
	ပြန်ပေး OK;  // just in case
    အကယ်၍ (totcount < type->tt_min_argcount - varargs)
    {
	emsg_funcname(e_not_enough_arguments_for_function_str, name);
	ပြန်ပေး FAIL;
    }
    အကယ်၍ (!varargs && type->tt_argcount >= 0 && totcount > type->tt_argcount)
    {
	emsg_funcname(e_too_many_arguments_for_function_str, name);
	ပြန်ပေး FAIL;
    }
    အကယ်၍ (type->tt_args == NULL)
	ပြန်ပေး OK;  // cannot check


    အတွက် (i = 0; i < totcount; ++i)
    {
	type_T	    *expected;
	typval_T    *tv;

	အကယ်၍ (base_tv != NULL)
	{
	    အကယ်၍ (i == 0)
		tv = base_tv;
	    မဟုတ်ပါက
		tv = &argvars[i - 1];
	}
	မဟုတ်ပါက
	    tv = &argvars[i];
	အကယ်၍ (varargs && i >= type->tt_argcount - 1)
	{
	    expected = type->tt_args[type->tt_argcount - 1];
	    အကယ်၍ (expected != NULL && expected->tt_type == VAR_LIST)
		expected = expected->tt_member;
	    အကယ်၍ (expected == NULL)
		expected = &t_any;
	}
	မဟုတ်ပါက
	    expected = type->tt_args[i];

	// check the type, unless the value is v:none
	အကယ်၍ ((tv->v_type != VAR_SPECIAL || tv->vval.v_number != VVAL_NONE)
		   && check_typval_arg_type(expected, tv, NULL, i + 1) == FAIL)
	    ပြန်ပေး FAIL;
    }
    ပြန်ပေး OK;
}

// Maximum nesting of types, to avoid running out of stack space.  E.g.
// "list<list<number>>" is nested twice.
#define MAX_TYPE_DEPTH	1000

// Current number of levels of parse_type() and the deepest level reached while
// parsing the current type.  The nesting is one less than the number of
// levels.
static ကိန်းပြည့်	parse_depth = 0;
static ကိန်းပြည့်	parse_max_depth = 0;

/*
 * Skip over type in list<type>, dict<type> or tuple<type>.
 * Returns a pointer to the character after the type.  "syn_error" is set to
 * TRUE on syntax error.
 */
    static char_u *
skip_member_type(char_u *start, char_u *p, ကိန်းပြည့် *syn_error)
{
    အကယ်၍ (STRNCMP("tuple", start, 5) == 0)
    {
	// handle tuple<{type1}, {type2}, ....<type>>
	p = skipwhite(p + 1);
	နေစဉ် (*p != '>' && *p != NUL)
	{
	    char_u *sp = p;

	    အကယ်၍ (STRNCMP(p, "...", 3) == 0)
		p += 3;
	    p = skip_type(p, TRUE);
	    အကယ်၍ (p == sp)
	    {
		*syn_error = TRUE;
		ပြန်ပေး p;  // syntax error
	    }
	    အကယ်၍ (*p == ',')
		p = skipwhite(p + 1);
	}
	အကယ်၍ (*p == '>')
	    p++;
    }
    မဟုတ်ပါက
    {
	p = skipwhite(p);
	p = skip_type(skipwhite(p + 1), FALSE);
	p = skipwhite(p);
	အကယ်၍ (*p == '>')
	    ++p;
    }

    ပြန်ပေး p;
}

/*
 * Skip over a function type.  Returns a pointer to the character after the
 * type.  "syn_error" is set to TRUE on syntax error.
 */
    static char_u *
skip_func_type(char_u *p, ကိန်းပြည့် *syn_error)
{
    အကယ်၍ (*p == '(')
    {
	// handle func(args): type
	++p;
	နေစဉ် (*p != ')' && *p != NUL)
	{
	    char_u *sp = p;

	    အကယ်၍ (STRNCMP(p, "...", 3) == 0)
		p += 3;
	    p = skip_type(p, TRUE);
	    အကယ်၍ (p == sp)
	    {
		*syn_error = TRUE;
		ပြန်ပေး p;  // syntax error
	    }
	    အကယ်၍ (*p == ',')
		p = skipwhite(p + 1);
	}
	အကယ်၍ (*p == ')')
	{
	    အကယ်၍ (p[1] == ':')
		p = skip_type(skipwhite(p + 2), FALSE);
	    မဟုတ်ပါက
		++p;
	}
    }
    မဟုတ်ပါက
    {
	// handle func: return_type
	p = skip_type(skipwhite(p + 1), FALSE);
    }

    ပြန်ပေး p;
}

/*
 * Inner part of skip_type().
 */
    static char_u *
skip_type_inner(char_u *start, ကိန်းပြည့် optional)
{
    char_u	*p = start;
    ကိန်းပြည့်		syn_error = FALSE;

    အကယ်၍ (optional && *p == '?')
	++p;

    // Also skip over "." for imported classes: "import.ClassName".
    နေစဉ် (ASCII_ISALNUM(*p) || *p == '_' || *p == '.')
	++p;

    // Skip over "<type>"; this is permissive about white space.
    အကယ်၍ (*skipwhite(p) == '<')
    {
	p = skip_member_type(start, p, &syn_error);
	အကယ်၍ (syn_error)
	    ပြန်ပေး p;
    }
    မဟုတ်ပါက အကယ်၍ ((*p == '(' || (*p == ':' && VIM_ISWHITE(p[1])))
					     && STRNCMP("func", start, 4) == 0)
    {
	// skip over function type
	p = skip_func_type(p, &syn_error);
	အကယ်၍ (syn_error)
	    ပြန်ပေး p;
    }

    ပြန်ပေး p;
}

/*
 * Skip over a type definition and return a pointer to just after it.
 * When "optional" is TRUE then a leading "?" is accepted.
 */
    char_u *
skip_type(char_u *start, ကိန်းပြည့် optional)
{
    static ကိန်းပြည့်	skip_depth = 0;
    char_u	*p = start;

    // Too deeply nested: skip to the matching ">" without recursing, an
    // error is given when parsing the type.
    အကယ်၍ (skip_depth > MAX_TYPE_DEPTH)
    {
	ကိန်းပြည့်	depth = 0;

	အတွက် ( ; *p != NUL; ++p)
	{
	    အကယ်၍ (*p == '<')
		++depth;
	    မဟုတ်ပါက အကယ်၍ (*p == '>')
	    {
		အကယ်၍ (depth == 0)
		    bha_nga;
		အကယ်၍ (--depth == 0)
		    ပြန်ပေး p + 1;
	    }
	    မဟုတ်ပါက အကယ်၍ (depth == 0 && !ASCII_ISALNUM(*p) && *p != '_'
								&& *p != '.')
		bha_nga;
	}
	ပြန်ပေး p;
    }

    ++skip_depth;
    p = skip_type_inner(start, optional);
    --skip_depth;
    ပြန်ပေး p;
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
	ကိန်းပြည့်	    give_error,
	var_na	    *info,
	ufunc_T	    *ufunc,
	cctx_T	    *cctx)
{
    char_u  *arg_start = *arg;
    type_T  *member_type;
    ကိန်းပြည့်	    prev_called_emsg = called_emsg;

    အကယ်၍ (**arg != '<')
    {
	အကယ်၍ (give_error)
	{
	    အကယ်၍ (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    မဟုတ်ပါက
		semsg(_(e_missing_type_after_str), info);
	}
	ပြန်ပေး NULL;
    }
    *arg = skipwhite(*arg + 1);

    member_type = parse_type(arg, type_gap, ufunc, cctx, give_error);
    အကယ်၍ (member_type == NULL || !valid_declaration_type(member_type))
	ပြန်ပေး NULL;

    *arg = skipwhite(*arg);
    အကယ်၍ (**arg != '>' && called_emsg == prev_called_emsg)
    {
	အကယ်၍ (give_error)
	    semsg(_(e_missing_gt_after_type_str), arg_start);
	ပြန်ပေး NULL;
    }
    ++*arg;

    အကယ်၍ (type->tt_type == VAR_LIST)
	ပြန်ပေး get_list_type(member_type, type_gap);
    ပြန်ပေး get_dict_type(member_type, type_gap);
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
    ကိန်းပြည့်		give_error,
    ufunc_T	*ufunc,
    cctx_T	*cctx)
{
    char_u  *p;
    type_T  *type;
    type_T  *ret_type = &t_unknown;
    ကိန်းပြည့်	argcount = -1;
    ကိန်းပြည့်	flags = 0;
    ကိန်းပြည့်	first_optional = -1;
    type_T	*arg_type[MAX_FUNC_ARGS + 1];

    // func({type}, ...{type}): {type}
    *arg += len;
    အကယ်၍ (**arg == '(')
    {
	// "func" may or may not return a value, "func()" does
	// not return a value.
	ret_type = &t_void;

	p = ++*arg;
	argcount = 0;
	နေစဉ် (*p != NUL && *p != ')')
	{
	    အကယ်၍ (*p == '?')
	    {
		အကယ်၍ (first_optional == -1)
		    first_optional = argcount;
		++p;
	    }
	    မဟုတ်ပါက အကယ်၍ (STRNCMP(p, "...", 3) == 0)
	    {
		flags |= TTFLAG_VARARGS;
		p += 3;
	    }
	    မဟုတ်ပါက အကယ်၍ (first_optional != -1)
	    {
		အကယ်၍ (give_error)
		    emsg(_(e_mandatory_argument_after_optional_argument));
		ပြန်ပေး NULL;
	    }

	    type = parse_type(&p, type_gap, ufunc, cctx, give_error);
	    အကယ်၍ (type == NULL || !valid_declaration_type(type))
		ပြန်ပေး NULL;
	    အကယ်၍ ((flags & TTFLAG_VARARGS) != 0 && type->tt_type != VAR_LIST)
	    {
		var_na *tofree;
		semsg(_(e_variable_arguments_type_must_be_list_str),
			type_name(type, &tofree));
		vim_free(tofree);
		ပြန်ပေး NULL;
	    }
	    arg_type[argcount++] = type;

	    // Nothing comes after "...{type}".
	    အကယ်၍ (flags & TTFLAG_VARARGS)
		bha_nga;

	    အကယ်၍ (*p != ',' && *skipwhite(p) == ',')
	    {
		အကယ်၍ (give_error)
		    semsg(_(e_no_white_space_allowed_before_str_str),
			    ",", p);
		ပြန်ပေး NULL;
	    }
	    အကယ်၍ (*p == ',')
	    {
		++p;
		အကယ်၍ (!VIM_ISWHITE(*p))
		{
		    အကယ်၍ (give_error)
			semsg(_(e_white_space_required_after_str_str),
				",", p - 1);
		    ပြန်ပေး NULL;
		}
	    }
	    p = skipwhite(p);
	    အကယ်၍ (argcount == MAX_FUNC_ARGS)
	    {
		အကယ်၍ (give_error)
		    emsg(_(e_too_many_argument_types));
		ပြန်ပေး NULL;
	    }
	}

	p = skipwhite(p);
	အကယ်၍ (*p != ')')
	{
	    အကယ်၍ (give_error)
		emsg(_(e_missing_closing_paren));
	    ပြန်ပေး NULL;
	}
	*arg = p + 1;
    }
    အကယ်၍ (**arg == ':')
    {
	// parse return type
	++*arg;
	အကယ်၍ (!VIM_ISWHITE(**arg) && give_error)
	    semsg(_(e_white_space_required_after_str_str), ":", *arg - 1);
	*arg = skipwhite(*arg);
	ret_type = parse_type(arg, type_gap, ufunc, cctx, give_error);
	အကယ်၍ (ret_type == NULL)
	    ပြန်ပေး NULL;
    }
    အကယ်၍ (flags == 0 && first_optional == -1 && argcount <= 0)
	type = get_func_type(ret_type, argcount, type_gap);
    မဟုတ်ပါက
    {
	type = alloc_func_type(ret_type, argcount, type_gap);
	type->tt_flags = flags;
	အကယ်၍ (argcount > 0)
	{
	    type->tt_argcount = argcount;
	    type->tt_min_argcount = first_optional == -1
						? argcount : first_optional;
	    အကယ်၍ (func_type_add_arg_types(type, argcount, type_gap) == FAIL)
		ပြန်ပေး NULL;
	    mch_memmove(type->tt_args, arg_type, parimaa_na(type_T *) * argcount);
	}
    }

    ပြန်ပေး type;
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
    ကိန်းပြည့်		give_error,
    ufunc_T	*ufunc,
    cctx_T	*cctx)
{
    char_u	*p;
    type_T	*type;
    type_T	*ret_type = NULL;
    ကိန်းပြည့်		typecount = -1;
    ကိန်းပြည့်		flags = 0;
    garray_T	tuple_types_ga;

    ga_init2(&tuple_types_ga, parimaa_na(type_T *), 10);

    // tuple<{type}, {type}>
    // tuple<{type}, ...{type}>
    အကယ်၍ (**arg != '<')
    {
	အကယ်၍ (give_error)
	{
	    အကယ်၍ (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    မဟုတ်ပါက
		semsg(_(e_missing_type_after_str), "tuple");
	}

	// only "tuple" is specified
	ပြန်ပေး NULL;
    }

    p = ++*arg;
    typecount = 0;
    နေစဉ် (*p != NUL && *p != '>')
    {
	အကယ်၍ (STRNCMP(p, "...", 3) == 0)
	{
	    flags |= TTFLAG_VARARGS;
	    p += 3;
	}

	type = parse_type(&p, type_gap, ufunc, cctx, give_error);
	အကယ်၍ (type == NULL || !valid_declaration_type(type))
	    goto on_err;

	အကယ်၍ ((flags & TTFLAG_VARARGS) != 0 && type->tt_type != VAR_LIST)
	{
	    var_na *tofree;
	    semsg(_(e_variadic_tuple_must_end_with_list_type_str),
						type_name(type, &tofree));
	    vim_free(tofree);
	    goto on_err;
	}

	// Add the item type
	အကယ်၍ (ga_grow(&tuple_types_ga, 1) == FAIL)
	    goto on_err;
	((type_T **)tuple_types_ga.ga_data)[tuple_types_ga.ga_len] = type;
	tuple_types_ga.ga_len++;
	typecount++;

	// Nothing comes after "...{type}".
	အကယ်၍ (flags & TTFLAG_VARARGS)
	    bha_nga;

	အကယ်၍ (*p != ',' && *skipwhite(p) == ',')
	{
	    အကယ်၍ (give_error)
		semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
	    goto on_err;
	}
	အကယ်၍ (*p == ',')
	{
	    ++p;
	    အကယ်၍ (!VIM_ISWHITE(*p))
	    {
		အကယ်၍ (give_error)
		    semsg(_(e_white_space_required_after_str_str),
			    ",", p - 1);
		goto on_err;
	    }
	}
	p = skipwhite(p);
    }

    p = skipwhite(p);
    အကယ်၍ (*p != '>' || typecount <= 0)
    {
	အကယ်၍ (give_error)
	    semsg(_(e_missing_type_after_str), p);
	goto on_err;
    }
    *arg = p + 1;

    ret_type = alloc_tuple_type(typecount, type_gap);
    အကယ်၍ (ret_type == &t_any
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

    ပြန်ပေး ret_type;
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
    ကိန်းပြည့်		give_error,
    cctx_T	*cctx)
{
    char_u	*arg_start = *arg;
    type_T	*object_type;
    ကိန်းပြည့်		prev_called_emsg = called_emsg;

    // object<X> or object<any>
    အကယ်၍ (**arg != '<')
    {
	အကယ်၍ (give_error)
	{
	    အကယ်၍ (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    မဟုတ်ပါက
		semsg(_(e_missing_type_after_str), "object");
	}

	// only "object" is specified
	ပြန်ပေး NULL;
    }

    // skip spaces following "object<"
    *arg = skipwhite(*arg + 1);

    object_type = parse_type(arg, type_gap, NULL, cctx, give_error);
    အကယ်၍ (object_type == NULL)
	ပြန်ပေး NULL;

    *arg = skipwhite(*arg);
    အကယ်၍ (**arg != '>' && called_emsg == prev_called_emsg)
    {
	အကယ်၍ (give_error)
	    semsg(_(e_missing_gt_after_type_str), arg_start);
	ပြန်ပေး NULL;
    }
    ++*arg;

    အကယ်၍ (object_type->tt_type == VAR_ANY)
	ပြန်ပေး &t_object_any;

    အကယ်၍ (object_type->tt_type != VAR_OBJECT)
    {
	// specified type is not a class
	အကယ်၍ (give_error)
	    semsg(_(e_class_name_not_found_str), arg_start);
	ပြန်ပေး NULL;
    }

    ပြန်ပေး object_type;
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
    ကိန်းပြည့်		give_error,
    ufunc_T	*ufunc,
    cctx_T	*cctx)
{
    ကိန်းပြည့်		did_emsg_before = did_emsg;
    typval_T	tv;

    tv.v_type = VAR_UNKNOWN;
    အကယ်၍ (eval_variable_import(*arg, &tv) == OK)
    {
	အကယ်၍ (tv.v_type == VAR_CLASS && tv.vval.v_class != NULL)
	{
	    type_T *type = get_type_ptr(type_gap);
	    အကယ်၍ (type != NULL)
	    {
		// Although the name is that of a class or interface, the type
		// uses will be an object.
		type->tt_type = VAR_OBJECT;
		type->tt_class = tv.vval.v_class;
		clear_tv(&tv);

		*arg += len;
		// Skip over ".ClassName".
		နေစဉ် (ASCII_ISALNUM(**arg) || **arg == '_' || **arg == '.')
		    ++*arg;

		ပြန်ပေး type;
	    }
	}
	မဟုတ်ပါက အကယ်၍ (tv.v_type == VAR_TYPEALIAS)
	{
	    // user defined type
	    // The type alias may itself be a nested type, check the resulting
	    // nesting depth.
	    ကိန်းပြည့်	depth = parse_depth - 1
				+ MAX(tv.vval.v_typealias->ta_depth, 1);
	    အကယ်၍ (depth - 1 > MAX_TYPE_DEPTH)
	    {
		clear_tv(&tv);
		အကယ်၍ (give_error)
		    emsg(_(e_type_nested_too_deep));
		ပြန်ပေး NULL;
	    }
	    အကယ်၍ (depth > parse_max_depth)
		parse_max_depth = depth;

	    type_T *type = copy_type(tv.vval.v_typealias->ta_type, type_gap);
	    *arg += len;
	    clear_tv(&tv);
	    // Skip over ".TypeName".
	    နေစဉ် (ASCII_ISALNUM(**arg) || **arg == '_' || **arg == '.')
		++*arg;
	    ပြန်ပေး type;
	}

	clear_tv(&tv);
    }

    // Check whether it is a generic type
    type_T *type = find_generic_type(*arg, len, ufunc, cctx);
    အကယ်၍ (type != NULL)
    {
	*arg += len;
	ပြန်ပေး type;
    }

    အကယ်၍ (give_error && (did_emsg == did_emsg_before))
    {
	char_u	*p = skip_type(*arg, FALSE);
	var_na	cc = *p;

	*p = NUL;
	semsg(_(e_type_not_recognized_str), *arg);
	*p = cc;
    }

    ပြန်ပေး NULL;
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
    ကိန်းပြည့်		give_error)
{
    char_u  *p = *arg;
    size_t  len;

    // Skip over the first word.
    နေစဉ် (ASCII_ISALNUM(*p) || *p == '_')
	++p;
    len = p - *arg;

    switch (**arg)
    {
	pak_sa 'a':
	    အကယ်၍ (len == 3 && STRNCMP(*arg, "any", len) == 0)
	    {
		*arg += len;
		ပြန်ပေး &t_any;
	    }
	    bha_nga;
	pak_sa 'b':
	    အကယ်၍ (len == 4 && STRNCMP(*arg, "bool", len) == 0)
	    {
		*arg += len;
		ပြန်ပေး &t_bool;
	    }
	    အကယ်၍ (len == 4 && STRNCMP(*arg, "blob", len) == 0)
	    {
		*arg += len;
		ပြန်ပေး &t_blob;
	    }
	    bha_nga;
	pak_sa 'c':
	    အကယ်၍ (len == 7 && STRNCMP(*arg, "channel", len) == 0)
	    {
		*arg += len;
		ပြန်ပေး &t_channel;
	    }
	    bha_nga;
	pak_sa 'd':
	    အကယ်၍ (len == 4 && STRNCMP(*arg, "dict", len) == 0)
	    {
		*arg += len;
		ပြန်ပေး parse_type_member(arg, &t_dict_any, type_gap,
						give_error, "dict", ufunc,
						cctx);
	    }
	    bha_nga;
	pak_sa 'f':
	    အကယ်၍ (len == 5 && STRNCMP(*arg, "float", len) == 0)
	    {
		*arg += len;
		ပြန်ပေး &t_float;
	    }
	    အကယ်၍ (len == 4 && STRNCMP(*arg, "func", len) == 0)
		ပြန်ပေး parse_type_func(arg, len, type_gap, give_error, ufunc,
									cctx);
	    bha_nga;
	pak_sa 'j':
	    အကယ်၍ (len == 3 && STRNCMP(*arg, "job", len) == 0)
	    {
		*arg += len;
		ပြန်ပေး &t_job;
	    }
	    bha_nga;
	pak_sa 'l':
	    အကယ်၍ (len == 4 && STRNCMP(*arg, "list", len) == 0)
	    {
		*arg += len;
		ပြန်ပေး parse_type_member(arg, &t_list_any, type_gap,
						give_error, "list", ufunc,
						cctx);
	    }
	    bha_nga;
	pak_sa 'n':
	    အကယ်၍ (len == 6 && STRNCMP(*arg, "number", len) == 0)
	    {
		*arg += len;
		ပြန်ပေး &t_number;
	    }
	    bha_nga;
	pak_sa 'o':
	    အကယ်၍ (len == 6 && STRNCMP(*arg, "object", len) == 0)
	    {
		*arg += len;
		ပြန်ပေး parse_type_object(arg, type_gap, give_error, cctx);
	    }
	    bha_nga;
	pak_sa 's':
	    အကယ်၍ (len == 6 && STRNCMP(*arg, "string", len) == 0)
	    {
		*arg += len;
		ပြန်ပေး &t_string;
	    }
	    bha_nga;
	pak_sa 't':
	    အကယ်၍ (len == 5 && STRNCMP(*arg, "tuple", len) == 0)
	    {
		*arg += len;
		ပြန်ပေး parse_type_tuple(arg, type_gap, give_error, ufunc,
									cctx);
	    }
	    bha_nga;
	pak_sa 'v':
	    အကယ်၍ (len == 4 && STRNCMP(*arg, "void", len) == 0)
	    {
		*arg += len;
		ပြန်ပေး &t_void;
	    }
	    bha_nga;
    }

    // User defined type
    ပြန်ပေး parse_type_user_defined(arg, len, type_gap, give_error, ufunc,
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
    ကိန်းပြည့်		give_error)
{
    type_T	*type;

    အကယ်၍ (parse_depth > MAX_TYPE_DEPTH)
    {
	အကယ်၍ (give_error)
	    emsg(_(e_type_nested_too_deep));
	ပြန်ပေး NULL;
    }
    အကယ်၍ (parse_depth == 0)
	parse_max_depth = 0;
    ++parse_depth;
    အကယ်၍ (parse_depth > parse_max_depth)
	parse_max_depth = parse_depth;
    type = parse_type_inner(arg, type_gap, ufunc, cctx, give_error);
    --parse_depth;
    ပြန်ပေး type;
}

/*
 * Return the number of levels of the type last parsed by parse_type().
 */
    ကိန်းပြည့်
parse_type_depth(void)
{
    ပြန်ပေး parse_max_depth;
}

/*
 * Check if "type1" and "type2" are exactly the same.
 * "flags" can have ETYPE_ARG_UNKNOWN, which means that an unknown argument
 * type in "type1" is accepted.
 */
    ကိန်းပြည့်
equal_type(type_T *type1, type_T *type2, ကိန်းပြည့် flags)
{
    ကိန်းပြည့် i;

    အကယ်၍ (type1 == NULL || type2 == NULL)
	ပြန်ပေး FALSE;
    အကယ်၍ (type1->tt_type != type2->tt_type)
	ပြန်ပေး FALSE;
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
	    ပြန်ပေး type1->tt_class == type2->tt_class;
	pak_sa VAR_LIST:
	pak_sa VAR_DICT:
	    ပြန်ပေး equal_type(type1->tt_member, type2->tt_member, flags);
	pak_sa VAR_TUPLE:
	    အကယ်၍ (type1->tt_argcount != type2->tt_argcount)
		ပြန်ပေး FALSE;
	    အကယ်၍ (type1->tt_argcount < 0
			   || type1->tt_args == NULL || type2->tt_args == NULL)
		ပြန်ပေး TRUE;
	    အတွက် (i = 0; i < type1->tt_argcount; ++i)
		အကယ်၍ ((flags & ETYPE_ARG_UNKNOWN) == 0
			&& !equal_type(type1->tt_args[i], type2->tt_args[i],
									flags))
		    ပြန်ပေး FALSE;
	    ပြန်ပေး TRUE;
	pak_sa VAR_FUNC:
	pak_sa VAR_PARTIAL:
	    အကယ်၍ (!equal_type(type1->tt_member, type2->tt_member, flags)
		    || type1->tt_argcount != type2->tt_argcount)
		ပြန်ပေး FALSE;
	    အကယ်၍ (type1->tt_argcount < 0
			   || type1->tt_args == NULL || type2->tt_args == NULL)
		ပြန်ပေး TRUE;
	    အတွက် (i = 0; i < type1->tt_argcount; ++i)
		အကယ်၍ ((flags & ETYPE_ARG_UNKNOWN) == 0
			&& !equal_type(type1->tt_args[i], type2->tt_args[i],
									flags))
		    ပြန်ပေး FALSE;
	    ပြန်ပေး TRUE;
    }
    ပြန်ပေး TRUE;
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
    အကယ်၍ (type1 == &t_func_unknown)
    {
	*dest = type2;
	ပြန်ပေး;
    }
    အကယ်၍ (type2 == &t_func_unknown)
    {
	*dest = type1;
	ပြန်ပေး;
    }

    common_type(type1->tt_member, type2->tt_member, &common, type_gap);
    အကယ်၍ (type1->tt_argcount == type2->tt_argcount
	    && type1->tt_argcount >= 0)
    {
	ကိန်းပြည့် argcount = type1->tt_argcount;
	ကိန်းပြည့် i;

	*dest = alloc_func_type(common, argcount, type_gap);
	အကယ်၍ (type1->tt_args != NULL && type2->tt_args != NULL)
	{
	    အကယ်၍ (func_type_add_arg_types(*dest, argcount,
			type_gap) == OK)
		အတွက် (i = 0; i < argcount; ++i)
		    common_type(type1->tt_args[i], type2->tt_args[i],
			    &(*dest)->tt_args[i], type_gap);
	}
    }
    မဟုတ်ပါက
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
    အကယ်၍ (equal_type(type1, type2, 0))
    {
	*dest = type1;
	ပြန်ပေး;
    }

    // If either is VAR_UNKNOWN use the other type.  An empty list/dict has no
    // specific type.
    အကယ်၍ (type1 == NULL || type1->tt_type == VAR_UNKNOWN)
    {
	*dest = type2;
	ပြန်ပေး;
    }
    အကယ်၍ (type2 == NULL || type2->tt_type == VAR_UNKNOWN)
    {
	*dest = type1;
	ပြန်ပေး;
    }

    အကယ်၍ (type1->tt_type == type2->tt_type)
    {
	အကယ်၍ (type1->tt_type == VAR_LIST
		|| type1->tt_type == VAR_DICT)
	{
	    type_T *common;

	    common_type(type1->tt_member, type2->tt_member, &common, type_gap);
	    အကယ်၍ (type1->tt_type == VAR_LIST)
		*dest = get_list_type(common, type_gap);
	    မဟုတ်ပါက
		*dest = get_dict_type(common, type_gap);
	    ပြန်ပေး;
	}
	မဟုတ်ပါက အကယ်၍ (type1->tt_type == VAR_FUNC)
	{
	    common_type_var_func(type1, type2, dest, type_gap);
	    ပြန်ပေး;
	}
	မဟုတ်ပါက အကယ်၍ (type1->tt_type == VAR_OBJECT)
	{
	    *dest = &t_object_any;
	    ပြန်ပေး;
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
    အကယ်၍ (type->tt_type == VAR_TUPLE)
    {
	အကယ်၍ (type->tt_argcount != 1)
	    ပြန်ပေး &t_any;

	အကယ်၍ (type->tt_flags & TTFLAG_VARARGS)
	    ပြန်ပေး type->tt_args[0]->tt_member;
	မဟုတ်ပါက
	    ပြန်ပေး type->tt_args[0];
    }

    // For "any" the item type is not known.
    ပြန်ပေး type->tt_member == NULL ? &t_any : type->tt_member;
}

/*
 * Push an entry onto the type stack.  "type" used both for the current type
 * and the declared type.
 * Returns FAIL when out of memory.
 */
    ကိန်းပြည့်
push_type_stack(cctx_T *cctx, type_T *type)
{
    ပြန်ပေး push_type_stack2(cctx, type, type);
}

/*
 * Push an entry onto the type stack.  "type" is the current type, "decl_type"
 * is the declared type.
 * Returns FAIL when out of memory.
 */
    ကိန်းပြည့်
push_type_stack2(cctx_T *cctx, type_T *type, type_T *decl_type)
{
    garray_T	*stack = &cctx->ctx_type_stack;
    type2_T	*typep;

    အကယ်၍ (GA_GROW_FAILS(stack, 1))
	ပြန်ပေး FAIL;
    typep = ((type2_T *)stack->ga_data) + stack->ga_len;
    typep->type_curr = type;
    typep->type_decl = decl_type;
    ++stack->ga_len;
    ပြန်ပေး OK;
}

/*
 * Set the type of the top of the stack to "type".
 */
    void
set_type_on_stack(cctx_T *cctx, type_T *type, ကိန်းပြည့် offset)
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
get_type_on_stack(cctx_T *cctx, ကိန်းပြည့် offset)
{
    garray_T	*stack = &cctx->ctx_type_stack;

    အကယ်၍ (offset + 1 > stack->ga_len)
	ပြန်ပေး &t_unknown;
    ပြန်ပေး (((type2_T *)stack->ga_data) + stack->ga_len - offset - 1)
								   ->type_curr;
}

/*
 * Get the declared type from the type stack.  If "offset" is zero the one at
 * the top,
 * if "offset" is one the type above that, etc.
 * Returns &t_unknown if there is no such stack entry.
 */
    type_T *
get_decl_type_on_stack(cctx_T *cctx, ကိန်းပြည့် offset)
{
    garray_T	*stack = &cctx->ctx_type_stack;

    အကယ်၍ (offset + 1 > stack->ga_len)
	ပြန်ပေး &t_unknown;
    ပြန်ပေး (((type2_T *)stack->ga_data) + stack->ga_len - offset - 1)
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
	ကိန်းပြည့်	    count,
	ကိန်းပြည့်	    skip,
	cctx_T	    *cctx)
{
    garray_T	*stack = &cctx->ctx_type_stack;
    type2_T	*typep;
    garray_T    *type_gap = cctx->ctx_type_list;
    ကိန်းပြည့်		i;
    type_T	*result;
    type_T	*type;

    // Use "unknown" for an empty list or dict.
    အကယ်၍ (count == 0)
	ပြန်ပေး &t_unknown;
    // Find the common type from following items.
    typep = ((type2_T *)stack->ga_data) + stack->ga_len;
    result = &t_unknown;
    အတွက် (i = 0; i < count; ++i)
    {
	type = (typep -((count - i) * skip) + skip - 1)->type_curr;
	အကယ်၍ (check_type_is_value(type) == FAIL)
	    ပြန်ပေး NULL;
	အကယ်၍ (result != &t_any)
	    common_type(type, result, &result, type_gap);
    }

    ပြန်ပေး result;
}

/*
 * Get the types of items in a tuple on the stack of "cctx".
 * Returns the number of types.  Returns -1 on failure.
 */
    ကိန်းပြည့်
get_tuple_type_from_stack(
    ကိန်းပြည့်		count,
    garray_T	*tuple_types_gap,
    cctx_T	*cctx)
{
    garray_T	*stack = &cctx->ctx_type_stack;
    type2_T	*typep;
    type_T	*type = NULL;

    // Use "unknown" for an empty tuple
    အကယ်၍ (count == 0)
	ပြန်ပေး 0;

    // Find the common type from following items.
    typep = ((type2_T *)stack->ga_data) + stack->ga_len;
    အတွက် (ကိန်းပြည့် i = 0; i < count; i++)
    {
	type = (typep - (count - i))->type_curr;
	အကယ်၍ (check_type_is_value(type) == FAIL)
	    ပြန်ပေး -1;
	အကယ်၍ (ga_grow(tuple_types_gap, 1) == FAIL)
	    ပြန်ပေး -1;
	((type_T **)tuple_types_gap->ga_data)[tuple_types_gap->ga_len] = type;
	tuple_types_gap->ga_len++;
    }

    ပြန်ပေး tuple_types_gap->ga_len;
}

    var_na *
vartype_name(vartype_T type)
{
    switch (type)
    {
	pak_sa VAR_UNKNOWN: bha_nga;
	pak_sa VAR_ANY: ပြန်ပေး "any";
	pak_sa VAR_VOID: ပြန်ပေး "void";
	pak_sa VAR_SPECIAL: ပြန်ပေး "special";
	pak_sa VAR_BOOL: ပြန်ပေး "bool";
	pak_sa VAR_NUMBER: ပြန်ပေး "number";
	pak_sa VAR_FLOAT: ပြန်ပေး "float";
	pak_sa VAR_STRING: ပြန်ပေး "string";
	pak_sa VAR_BLOB: ပြန်ပေး "blob";
	pak_sa VAR_JOB: ပြန်ပေး "job";
	pak_sa VAR_CHANNEL: ပြန်ပေး "channel";
	pak_sa VAR_LIST: ပြန်ပေး "list";
	pak_sa VAR_TUPLE: ပြန်ပေး "tuple";
	pak_sa VAR_DICT: ပြန်ပေး "dict";
	pak_sa VAR_INSTR: ပြန်ပေး "instr";
	pak_sa VAR_CLASS: ပြန်ပေး "class";
	pak_sa VAR_OBJECT: ပြန်ပေး "object";
	pak_sa VAR_TYPEALIAS: ပြန်ပေး "typealias";

	pak_sa VAR_FUNC:
	pak_sa VAR_PARTIAL: ပြန်ပေး "func";
    }
    ပြန်ပေး "unknown";
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

    အကယ်၍ (type->tt_member->tt_type == VAR_UNKNOWN)
	member_name = type_name(&t_any, &member_free);
    မဟုတ်ပါက
	member_name = type_name(type->tt_member, &member_free);

    size_t len = STRLEN(name) + STRLEN(member_name) + 3;
    *tofree = alloc(len);
    အကယ်၍ (*tofree == NULL)
    {
	vim_free(member_free);
	ပြန်ပေး name;
    }

    vim_snprintf(*tofree, len, "%s<%s>", name, member_name);
    vim_free(member_free);
    ပြန်ပေး *tofree;
}

/*
 * Return the type name of a tuple.
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    static var_na *
type_name_tuple(type_T *type, var_na **tofree)
{
    garray_T    ga;
    ကိန်းပြည့်		i;
    ကိန်းပြည့်		varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;
    var_na	*arg_free = NULL;

    ga_init2(&ga, 1, 100);
    အကယ်၍ (ga_grow(&ga, 20) == FAIL)
	goto failed;
    STRCPY(ga.ga_data, "tuple<");
    ga.ga_len += 6;

    အကယ်၍ (type->tt_argcount <= 0)
	// empty tuple
	GA_CONCAT_LITERAL(&ga, "any");
    မဟုတ်ပါက
    {
	အကယ်၍ (type->tt_args == NULL)
	    GA_CONCAT_LITERAL(&ga, "[unknown]");
	မဟုတ်ပါက
	{
	    အတွက် (i = 0; i < type->tt_argcount; ++i)
	    {
		string_T    arg_type;

		arg_type.string = (char_u *)type_name(type->tt_args[i], &arg_free);
		အကယ်၍ (i > 0)
		{
		    STRCPY((var_na *)ga.ga_data + ga.ga_len, ", ");
		    ga.ga_len += 2;
		}
		arg_type.length = STRLEN(arg_type.string);
		အကယ်၍ (ga_grow(&ga, (ကိန်းပြည့်)arg_type.length + 8) == FAIL)
		    goto failed;
		အကယ်၍ (varargs && i == type->tt_argcount - 1)
		    GA_CONCAT_LITERAL(&ga, "...");
		ga_concat_len(&ga, arg_type.string, arg_type.length);
		VIM_CLEAR(arg_free);
	    }
	}
    }

    STRCPY((var_na *)ga.ga_data + ga.ga_len, ">");
    *tofree = ga.ga_data;
    ပြန်ပေး ga.ga_data;

failed:
    vim_free(arg_free);
    ga_clear(&ga);
    ပြန်ပေး "[unknown]";
}

/*
 * Return the type name of a Class (class<name>) or Object (object<name>).
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    static var_na *
type_name_class_or_obj(var_na *name, type_T *type, var_na **tofree)
{
    string_T	class_name;

    အကယ်၍ (type->tt_class != NULL)
    {
	class_name.string = type->tt_class->class_name.string;
	class_name.length = type->tt_class->class_name.length;
	အကယ်၍ (IS_ENUM(type->tt_class))
	    name = "enum";
    }
    မဟုတ်ပါက
	STR_LITERAL_SET(class_name, "any");

    size_t len = STRLEN(name) + class_name.length + 3;
    *tofree = alloc(len);
    အကယ်၍ (*tofree == NULL)
	ပြန်ပေး name;

    vim_snprintf(*tofree, len, "%s<%s>", name, class_name.string);
    ပြန်ပေး *tofree;
}

/*
 * Return the type name of a function.
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    static var_na *
type_name_func(type_T *type, var_na **tofree)
{
    garray_T    ga;
    ကိန်းပြည့်		i;
    ကိန်းပြည့်		varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;
    var_na	*arg_free = NULL;

    ga_init2(&ga, 1, 100);
    အကယ်၍ (ga_grow(&ga, 20) == FAIL)
	goto failed;
    STRCPY(ga.ga_data, "func(");
    ga.ga_len += 5;

    အတွက် (i = 0; i < type->tt_argcount; ++i)
    {
	string_T    arg_type;

	အကယ်၍ (type->tt_args == NULL)
	    STR_LITERAL_SET(arg_type, "[unknown]");
	မဟုတ်ပါက
	{
	    arg_type.string = (char_u *)type_name(type->tt_args[i], &arg_free);
	    arg_type.length = STRLEN(arg_type.string);
	}
	အကယ်၍ (i > 0)
	{
	    STRCPY((var_na *)ga.ga_data + ga.ga_len, ", ");
	    ga.ga_len += 2;
	}
	အကယ်၍ (ga_grow(&ga, (ကိန်းပြည့်)arg_type.length + 8) == FAIL)
	    goto failed;
	အကယ်၍ (varargs && i == type->tt_argcount - 1)
	    GA_CONCAT_LITERAL(&ga, "...");
	မဟုတ်ပါက အကယ်၍ (i >= type->tt_min_argcount)
	    *((var_na *)ga.ga_data + ga.ga_len++) = '?';
	ga_concat_len(&ga, arg_type.string, arg_type.length);
	VIM_CLEAR(arg_free);
    }
    အကယ်၍ (type->tt_argcount < 0)
	// any number of arguments
	GA_CONCAT_LITERAL(&ga, "...");

    အကယ်၍ (type->tt_member == &t_void)
	STRCPY((var_na *)ga.ga_data + ga.ga_len, ")");
    မဟုတ်ပါက
    {
	var_na *ret_free = NULL;
	var_na *ret_name = type_name(type->tt_member, &ret_free);
	ကိန်းပြည့်  len;

	len = (ကိန်းပြည့်)STRLEN(ret_name) + 4;
	အကယ်၍ (ga_grow(&ga, len) == FAIL)
	{
	    vim_free(ret_free);
	    goto failed;
	}
	STRCPY((var_na *)ga.ga_data + ga.ga_len, "): ");
	STRCPY((var_na *)ga.ga_data + ga.ga_len + 3, ret_name);
	vim_free(ret_free);
    }
    *tofree = ga.ga_data;
    ပြန်ပေး ga.ga_data;

failed:
    vim_free(arg_free);
    ga_clear(&ga);
    ပြန်ပေး "[unknown]";
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
    အကယ်၍ (type == NULL)
	ပြန်ပေး "[unknown]";
    name = vartype_name(type->tt_type);

    switch (type->tt_type)
    {
	pak_sa VAR_LIST:
	pak_sa VAR_DICT:
	    ပြန်ပေး type_name_list_or_dict(name, type, tofree);

	pak_sa VAR_TUPLE:
	    ပြန်ပေး type_name_tuple(type, tofree);

	pak_sa VAR_CLASS:
	pak_sa VAR_OBJECT:
	    ပြန်ပေး type_name_class_or_obj(name, type, tofree);

	pak_sa VAR_FUNC:
	    ပြန်ပေး type_name_func(type, tofree);

	default:
	    bha_nga;
    }

    ပြန်ပေး name;
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
    အကယ်၍ (argvars[0].v_type == VAR_TYPEALIAS)
    {
	type = copy_type(argvars[0].vval.v_typealias->ta_type, &type_list);
	// A type alias for a class has the type set to VAR_OBJECT.  Change it
	// to VAR_CLASS, so that the name is "typealias<class<xxx>>"
	အကယ်၍ (type->tt_type == VAR_OBJECT)
	    type->tt_type = VAR_CLASS;
    }
    မဟုတ်ပါက
	type = typval2type(argvars, get_copyID(), &type_list, TVTT_DO_MEMBER);
    name = type_name(type, &tofree);
    အကယ်၍ (argvars[0].v_type == VAR_TYPEALIAS)
    {
	vim_snprintf((var_na *)IObuff, IOSIZE, "typealias<%s>", name);
	rettv->vval.v_string = vim_strsave((char_u *)IObuff);
	အကယ်၍ (tofree != NULL)
	    vim_free(tofree);
    }
    မဟုတ်ပါက
    {
	အကယ်၍ (tofree != NULL)
	    rettv->vval.v_string = (char_u *)tofree;
	မဟုတ်ပါက
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
    ကိန်းပြည့်
check_typval_is_value(typval_T *tv)
{
    အကယ်၍ (tv == NULL)
	ပြန်ပေး OK;

    switch (tv->v_type)
    {
	pak_sa VAR_CLASS:
	    {
		class_T *cl = tv->vval.v_class;
		char_u *class_name = (cl == NULL) ? (char_u *)""
							: cl->class_name.string;
		အကယ်၍ (cl != NULL && IS_ENUM(cl))
		    semsg(_(e_using_enum_as_value_str), class_name);
		မဟုတ်ပါက
		    semsg(_(e_using_class_as_value_str), class_name);
	    }
	    ပြန်ပေး FAIL;

	pak_sa VAR_TYPEALIAS:
	    semsg(_(e_using_typealias_as_value_str),
		    tv->vval.v_typealias->ta_name);
	    ပြန်ပေး FAIL;

	default:
	    bha_nga;
    }
    ပြန်ပေး OK;
}

/*
 * Same as above, except check type_T.
 */
    ကိန်းပြည့်
check_type_is_value(type_T *type)
{
    အကယ်၍ (type == NULL)
	ပြန်ပေး OK;
    switch (type->tt_type)
    {
	pak_sa VAR_CLASS:
	    အကယ်၍ (type->tt_class != NULL && IS_ENUM(type->tt_class))
		semsg(_(e_using_enum_as_value_str),
			type->tt_class->class_name.string);
	    မဟုတ်ပါက
		semsg(_(e_using_class_as_value_str),
			type->tt_class == NULL ? (char_u *)""
			: type->tt_class->class_name.string);
	    ပြန်ပေး FAIL;

	pak_sa VAR_TYPEALIAS:
	    // TODO: Not sure what could be done here to get a name.
	    //       Maybe an optional argument?
	    emsg(_(e_using_typealias_as_var_val));
	    ပြန်ပေး FAIL;

	default:
	    bha_nga;
    }
    ပြန်ပေး OK;
}

#endif // FEAT_EVAL
