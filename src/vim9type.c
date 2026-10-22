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

#ຖ້າ defined(FEAT_EVAL)

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

    ຖ້າ (ga_grow(type_gap, 1) == FAIL)
	ສົ່ງຄືນ NULL;
    type = ALLOC_CLEAR_ONE(type_T);
    ຖ້າ (type == NULL)
	ສົ່ງຄືນ NULL;

    ((type_T **)type_gap->ga_data)[type_gap->ga_len] = type;
    ++type_gap->ga_len;
    ສົ່ງຄືນ type;
}

/*
 * Make a shallow copy of "type".
 * When allocation fails returns "type".
 */
    type_T *
copy_type(type_T *type, garray_T *type_gap)
{
    type_T *copy = get_type_ptr(type_gap);

    ຖ້າ (copy == NULL)
	ສົ່ງຄືນ type;
    *copy = *type;
    copy->tt_flags &= ~TTFLAG_STATIC;

    ຖ້າ (type->tt_args != NULL
	   && func_type_add_arg_types(copy, type->tt_argcount, type_gap) == OK)
	ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < type->tt_argcount; ++i)
	    copy->tt_args[i] = type->tt_args[i];

    ສົ່ງຄືນ copy;
}

/*
 * Inner part of copy_type_deep().
 * When allocation fails returns "type".
 */
    static type_T *
copy_type_deep_rec(type_T *type, garray_T *type_gap, garray_T *seen_types)
{
    ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < seen_types->ga_len; ++i)
	ຖ້າ (((type_T **)seen_types->ga_data)[i * 2] == type)
	    // seen this type before, return the copy we made
	    ສົ່ງຄືນ ((type_T **)seen_types->ga_data)[i * 2 + 1];

    type_T *copy = copy_type(type, type_gap);
    ຖ້າ (ga_grow(seen_types, 1) == FAIL)
	ສົ່ງຄືນ copy;
    ((type_T **)seen_types->ga_data)[seen_types->ga_len * 2] = type;
    ((type_T **)seen_types->ga_data)[seen_types->ga_len * 2 + 1] = copy;
    ++seen_types->ga_len;

    ຖ້າ (copy->tt_member != NULL)
	copy->tt_member = copy_type_deep_rec(copy->tt_member,
							 type_gap, seen_types);
    ຖ້າ (type->tt_args != NULL)
	ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < type->tt_argcount; ++i)
	    copy->tt_args[i] = copy_type_deep_rec(copy->tt_args[i],
							 type_gap, seen_types);

    ສົ່ງຄືນ copy;
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
    ສົ່ງຄືນ res;
}

    void
clear_type_list(garray_T *gap)
{
    ໃນຂະນະທີ່ (gap->ga_len > 0)
	vim_free(((type_T **)gap->ga_data)[--gap->ga_len]);
    ga_clear(gap);
}

    void
clear_func_type_list(garray_T *gap, type_T **func_type)
{
    ໃນຂະນະທີ່ (gap->ga_len > 0)
    {
	// func_type pointing to the uf_type_list, so reset pointer
	ຖ້າ (*func_type == ((type_T **)gap->ga_data)[--gap->ga_len])
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

    ຖ້າ (type == NULL)
	ສົ່ງຄືນ NULL;

    // A fixed type never contains allocated types, return as-is.
    ຖ້າ (type->tt_flags & TTFLAG_STATIC)
	ສົ່ງຄືນ type;

    ret = ALLOC_ONE(type_T);
    ຖ້າ (ret == NULL)
	ສົ່ງຄືນ NULL;
    *ret = *type;

    ຖ້າ (ret->tt_member != NULL)
	ret->tt_member = alloc_type(ret->tt_member);

    ຖ້າ (type->tt_argcount > 0 && type->tt_args != NULL)
    {
	ຈຳນວນເຕັມ i;

	ret->tt_args = ALLOC_MULT(type_T *, type->tt_argcount);
	ຖ້າ (ret->tt_args != NULL)
	    ສຳລັບ (i = 0; i < type->tt_argcount; ++i)
		ret->tt_args[i] = alloc_type(type->tt_args[i]);
    }
    ບໍ່ດັ່ງນັ້ນ
	ret->tt_args = NULL;

    ສົ່ງຄືນ ret;
}

/*
 * Free a type that was created with alloc_type().
 */
    void
free_type(type_T *type)
{
    ຈຳນວນເຕັມ i;

    ຖ້າ (type == NULL || (type->tt_flags & TTFLAG_STATIC))
	ສົ່ງຄືນ;
    ຖ້າ (type->tt_args != NULL)
    {
	ສຳລັບ (i = 0; i < type->tt_argcount; ++i)
	    free_type(type->tt_args[i]);
	vim_free(type->tt_args);
    }

    free_type(type->tt_member);

    vim_free(type);
}

/*
 * Return TRUE if "type" is to be recursed into for setting the type.
 */
    static ຈຳນວນເຕັມ
set_tv_type_recurse(type_T *type)
{
    ສົ່ງຄືນ type->tt_member != NULL
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
    ຖ້າ (d->dv_type == type)
	ສົ່ງຄືນ;

    free_type(d->dv_type);
    d->dv_type = alloc_type(type);

    // Need to recursively set the type of dict items?
    ຖ້າ (!set_tv_type_recurse(type))
	ສົ່ງຄືນ;

    ຈຳນວນເຕັມ		todo = (ຈຳນວນເຕັມ)d->dv_hashtab.ht_used;
    hashitem_T	*hi;
    dictitem_T	*di;

    FOR_ALL_HASHTAB_ITEMS(&d->dv_hashtab, hi, todo)
    {
	ຖ້າ (!HASHITEM_EMPTY(hi))
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
    ຖ້າ (l->lv_type == type)
	ສົ່ງຄືນ;

    free_type(l->lv_type);
    l->lv_type = alloc_type(type);

    // Need to recursively set the type of list items?
    ຖ້າ (l->lv_first == &range_list_item || !set_tv_type_recurse(type))
	ສົ່ງຄືນ;

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
    ຖ້າ (tuple->tv_type == type)
	ສົ່ງຄືນ;

    free_type(tuple->tv_type);
    tuple->tv_type = alloc_type(type);

    ຖ້າ (type->tt_argcount <= 0)
	ສົ່ງຄືນ;

    // recursively set the type of list items
    type_T	*item_type;
    ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < tuple_len(tuple); i++)
    {
	ຖ້າ ((type->tt_flags & TTFLAG_VARARGS) && (i >= type->tt_argcount - 1))
	    // For a variadic tuple, the last type is a List.  So use the
	    // List member type.
	    item_type = type->tt_args[type->tt_argcount - 1]->tt_member;
	ບໍ່ດັ່ງນັ້ນ
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
    ຖ້າ (type->tt_type == VAR_ANY)
	// If the variable type is "any", then keep the value type.
	// e.g.  var x: any = [1, 2] or var y: any = {v: 1}
	ສົ່ງຄືນ;

    ຖ້າ (tv->v_type == VAR_DICT && tv->vval.v_dict != NULL)
	set_tv_type_dict(tv->vval.v_dict, type);
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (tv->v_type == VAR_LIST && tv->vval.v_list != NULL)
	set_tv_type_list(tv->vval.v_list, type);
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (tv->v_type == VAR_TUPLE && tv->vval.v_tuple != NULL)
	set_tv_type_tuple(tv->vval.v_tuple, type);
}

/*
 * For a tuple type, reserve space for "typecount" types (including the
 * repeated type).
 */
    static ຈຳນວນເຕັມ
tuple_type_add_types(
    type_T	*tupletype,
    ຈຳນວນເຕັມ		typecount,
    garray_T	*type_gap)
{
    // To make it easy to free the space needed for the types, add the
    // pointer to type_gap.
    ຖ້າ (ga_grow(type_gap, 1) == FAIL)
	ສົ່ງຄືນ FAIL;
    tupletype->tt_args = alloc_clear_id(parimaa_na(type_T *) * typecount,
							  aid_tuple_type_args);
    ຖ້າ (tupletype->tt_args == NULL)
	ສົ່ງຄືນ FAIL;
    ((type_T **)type_gap->ga_data)[type_gap->ga_len] =
						(void *)tupletype->tt_args;
    ++type_gap->ga_len;
    ສົ່ງຄືນ OK;
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
    ຖ້າ (member_type == NULL || (member_type->tt_type == VAR_ANY
					&& !IS_GENERIC_TYPE(member_type)))
	ສົ່ງຄືນ &t_list_any;
    ຖ້າ (member_type->tt_type == VAR_VOID
	    || member_type->tt_type == VAR_UNKNOWN)
	ສົ່ງຄືນ &t_list_empty;
    ຖ້າ (member_type->tt_type == VAR_BOOL)
	ສົ່ງຄືນ &t_list_bool;
    ຖ້າ (member_type->tt_type == VAR_NUMBER)
	ສົ່ງຄືນ &t_list_number;
    ຖ້າ (member_type->tt_type == VAR_STRING)
	ສົ່ງຄືນ &t_list_string;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    ຖ້າ (type == NULL)
	ສົ່ງຄືນ &t_any;
    type->tt_type = VAR_LIST;
    type->tt_member = member_type;
    type->tt_argcount = 0;
    type->tt_args = NULL;
    ສົ່ງຄືນ type;
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
    ຈຳນວນເຕັມ		typecount = tuple_types_gap->ga_len;

    // recognize commonly used types
    ຖ້າ (typecount == 0)
	ສົ່ງຄືນ &t_tuple_any;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    ຖ້າ (type == NULL)
	ສົ່ງຄືນ &t_any;
    type->tt_type = VAR_TUPLE;
    type->tt_member = NULL;
    ຖ້າ (typecount > 0)
    {
	ຖ້າ (tuple_type_add_types(type, typecount, type_gap) == FAIL)
	    ສົ່ງຄືນ &t_any;
	mch_memmove(type->tt_args, tuple_types, parimaa_na(type_T *) * typecount);
    }
    type->tt_argcount = typecount;
    type->tt_flags = 0;

    ສົ່ງຄືນ type;
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
    ຖ້າ (member_type == NULL || (member_type->tt_type == VAR_ANY
					&& !IS_GENERIC_TYPE(member_type)))
	ສົ່ງຄືນ &t_dict_any;
    ຖ້າ (member_type->tt_type == VAR_VOID
	    || member_type->tt_type == VAR_UNKNOWN)
	ສົ່ງຄືນ &t_dict_empty;
    ຖ້າ (member_type->tt_type == VAR_BOOL)
	ສົ່ງຄືນ &t_dict_bool;
    ຖ້າ (member_type->tt_type == VAR_NUMBER)
	ສົ່ງຄືນ &t_dict_number;
    ຖ້າ (member_type->tt_type == VAR_STRING)
	ສົ່ງຄືນ &t_dict_string;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    ຖ້າ (type == NULL)
	ສົ່ງຄືນ &t_any;
    type->tt_type = VAR_DICT;
    type->tt_member = member_type;
    type->tt_argcount = 0;
    type->tt_args = NULL;
    ສົ່ງຄືນ type;
}

/*
 * Allocate a new type for a function.
 */
    type_T *
alloc_func_type(type_T *ret_type, ຈຳນວນເຕັມ argcount, garray_T *type_gap)
{
    type_T *type = get_type_ptr(type_gap);

    ຖ້າ (type == NULL)
	ສົ່ງຄືນ &t_any;
    type->tt_type = VAR_FUNC;
    type->tt_member = ret_type == NULL ? &t_unknown : ret_type;
    type->tt_argcount = argcount;
    type->tt_args = NULL;
    ສົ່ງຄືນ type;
}

/*
 * Allocate a new type for a tuple.
 */
    static type_T *
alloc_tuple_type(ຈຳນວນເຕັມ typecount, garray_T *type_gap)
{
    type_T *type = get_type_ptr(type_gap);

    ຖ້າ (type == NULL)
	ສົ່ງຄືນ &t_any;
    type->tt_type = VAR_TUPLE;
    type->tt_member = NULL;
    type->tt_argcount = typecount;
    type->tt_args = NULL;
    ສົ່ງຄືນ type;
}

/*
 * Get a function type, based on the return type "ret_type".
 * "argcount" must be -1 or 0, a predefined type can be used.
 */
    type_T *
get_func_type(type_T *ret_type, ຈຳນວນເຕັມ argcount, garray_T *type_gap)
{
    // recognize commonly used types
    ຖ້າ (ret_type == &t_unknown || ret_type == NULL)
    {
	// (argcount == 0) is not possible
	ສົ່ງຄືນ &t_func_unknown;
    }
    ຖ້າ (ret_type == &t_void)
    {
	ຖ້າ (argcount == 0)
	    ສົ່ງຄືນ &t_func_0_void;
	ບໍ່ດັ່ງນັ້ນ
	    ສົ່ງຄືນ &t_func_void;
    }
    ຖ້າ (ret_type == &t_any)
    {
	ຖ້າ (argcount == 0)
	    ສົ່ງຄືນ &t_func_0_any;
	ບໍ່ດັ່ງນັ້ນ
	    ສົ່ງຄືນ &t_func_any;
    }
    ຖ້າ (ret_type == &t_number)
    {
	ຖ້າ (argcount == 0)
	    ສົ່ງຄືນ &t_func_0_number;
	ບໍ່ດັ່ງນັ້ນ
	    ສົ່ງຄືນ &t_func_number;
    }
    ຖ້າ (ret_type == &t_string)
    {
	ຖ້າ (argcount == 0)
	    ສົ່ງຄືນ &t_func_0_string;
	ບໍ່ດັ່ງນັ້ນ
	    ສົ່ງຄືນ &t_func_string;
    }

    ສົ່ງຄືນ alloc_func_type(ret_type, argcount, type_gap);
}

/*
 * For a function type, reserve space for "argcount" argument types (including
 * vararg).
 */
    ຈຳນວນເຕັມ
func_type_add_arg_types(
	type_T	    *functype,
	ຈຳນວນເຕັມ	    argcount,
	garray_T    *type_gap)
{
    // To make it easy to free the space needed for the argument types, add the
    // pointer to type_gap.
    ຖ້າ (ga_grow(type_gap, 1) == FAIL)
	ສົ່ງຄືນ FAIL;
    functype->tt_args = ALLOC_CLEAR_MULT(type_T *, argcount);
    ຖ້າ (functype->tt_args == NULL)
	ສົ່ງຄືນ FAIL;
    ((type_T **)type_gap->ga_data)[type_gap->ga_len] =
						     (void *)functype->tt_args;
    ++type_gap->ga_len;
    ສົ່ງຄືນ OK;
}

/*
 * Return TRUE if "type" is NULL, any or unknown.
 * This also works for const (comparing with &t_any and &t_unknown doesn't).
 */
    ຈຳນວນເຕັມ
type_any_or_unknown(type_T *type)
{
    ສົ່ງຄືນ type == NULL || type->tt_type == VAR_ANY
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
	    ສົ່ງຄືນ &t_null;

	pak_sa VVAL_NONE:
	    ສົ່ງຄືນ &t_none;

	pak_sa VVAL_TRUE:
	pak_sa VVAL_FALSE:
	    ສົ່ງຄືນ &t_bool;

	default:
	    ສົ່ງຄືນ &t_unknown;
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
list_typval2type(typval_T *tv, ຈຳນວນເຕັມ copyID, garray_T *type_gap, ຈຳນວນເຕັມ flags)
{
    list_T	*l = tv->vval.v_list;
    listitem_T	*li;
    type_T	*member_type = NULL;

    // An empty list has type list<unknown>, unless the type was specified
    // and is not list<any>.  This matters when assigning to a variable
    // with a specific list type.
    ຖ້າ (l == NULL || (l->lv_first == NULL
		&& (l->lv_type == NULL || l->lv_type->tt_member == &t_any)))
	ສົ່ງຄືນ &t_list_empty;

    ຖ້າ ((flags & TVTT_DO_MEMBER) == 0)
	ສົ່ງຄືນ &t_list_any;

    // If the type is list<any> go through the members, it may end up a
    // more specific type.
    ຖ້າ (l->lv_type != NULL && (l->lv_first == NULL
		|| (flags & TVTT_MORE_SPECIFIC) == 0
		|| l->lv_type->tt_member != &t_any))
	// make a copy, lv_type may be freed if the list is freed
	ສົ່ງຄືນ copy_type_deep(l->lv_type, type_gap);

    ຖ້າ (l->lv_first == &range_list_item)
	ສົ່ງຄືນ &t_list_number;

    ຖ້າ (l->lv_copyID == copyID)
	// avoid recursion
	ສົ່ງຄືນ &t_list_any;

    l->lv_copyID = copyID;

    // Use the common type of all members.
    member_type = typval2type(&l->lv_first->li_tv, copyID, type_gap,
							TVTT_DO_MEMBER);
    ສຳລັບ (li = l->lv_first->li_next; li != NULL; li = li->li_next)
	common_type(typval2type(&li->li_tv, copyID, type_gap, TVTT_DO_MEMBER),
					member_type, &member_type, type_gap);

    // Reset copyID so that a shared reference to this list (not a circular
    // reference) can be processed again to get the correct type.
    l->lv_copyID = 0;

    ສົ່ງຄືນ get_list_type(member_type, type_gap);
}

/*
 * Get a type_T for a Tuple typval in "tv".
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    static type_T *
tuple_typval2type(typval_T *tv, ຈຳນວນເຕັມ copyID, garray_T *type_gap, ຈຳນວນເຕັມ flags)
{
    tuple_T	*tuple = tv->vval.v_tuple;
    ຈຳນວນເຕັມ		len = tuple_len(tuple);
    type_T	*type = NULL;

    // An empty tuple has type tuple<unknown>, unless the type was specified
    // and is not tuple<any>.  This matters when assigning to a variable
    // with a specific tuple type.
    ຖ້າ (tuple == NULL || (len == 0 && (tuple->tv_type == NULL
				|| tuple->tv_type->tt_argcount == 0)))
	ສົ່ງຄືນ &t_tuple_empty;

    ຖ້າ ((flags & TVTT_DO_MEMBER) == 0)
	ສົ່ງຄືນ &t_tuple_any;

    // If the type is tuple<any> go through the members, it may end up a
    // more specific type.
    ຖ້າ (tuple->tv_type != NULL && (len == 0
					|| (flags & TVTT_MORE_SPECIFIC) == 0))
	// make a copy, tv_type may be freed if the tuple is freed
	ສົ່ງຄືນ copy_type_deep(tuple->tv_type, type_gap);

    ຖ້າ (tuple->tv_copyID == copyID)
	// avoid recursion
	ສົ່ງຄືນ &t_tuple_any;

    tuple->tv_copyID = copyID;

    garray_T	tuple_types_ga;
    ga_init2(&tuple_types_ga, parimaa_na(type_T *), 10);
    ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < len; i++)
    {
	type = typval2type(TUPLE_ITEM(tuple, i), copyID, type_gap,
							TVTT_DO_MEMBER);
	ຖ້າ (ga_grow(&tuple_types_ga, 1) == FAIL)
	{
	    ga_clear(&tuple_types_ga);
	    // Reset copyID so that a shared reference to this tuple can be
	    // processed again.
	    tuple->tv_copyID = 0;
	    ສົ່ງຄືນ NULL;
	}
	((type_T **)tuple_types_ga.ga_data)[tuple_types_ga.ga_len] = type;
	tuple_types_ga.ga_len++;
    }

    type_T *tuple_type = get_tuple_type(&tuple_types_ga, type_gap);
    ga_clear(&tuple_types_ga);

    // Reset copyID so that a shared reference to this tuple (not a circular
    // reference) can be processed again to get the correct type.
    tuple->tv_copyID = 0;

    ສົ່ງຄືນ tuple_type;
}

/*
 * Get a type_T for a Dict typval in "tv".
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    static type_T *
dict_typval2type(typval_T *tv, ຈຳນວນເຕັມ copyID, garray_T *type_gap, ຈຳນວນເຕັມ flags)
{
    dict_iterator_T	iter;
    typval_T		*value;
    dict_T		*d = tv->vval.v_dict;
    type_T		*member_type = NULL;

    ຖ້າ (d == NULL || (d->dv_hashtab.ht_used == 0 && d->dv_type == NULL))
	ສົ່ງຄືນ &t_dict_empty;

    ຖ້າ ((flags & TVTT_DO_MEMBER) == 0)
	ສົ່ງຄືນ &t_dict_any;

    // If the type is dict<any> go through the members, it may end up a
    // more specific type.
    ຖ້າ (d->dv_type != NULL && (d->dv_hashtab.ht_used == 0
		|| (flags & TVTT_MORE_SPECIFIC) == 0
		|| d->dv_type->tt_member != &t_any))
	ສົ່ງຄືນ d->dv_type;

    ຖ້າ (d->dv_copyID == copyID)
	// avoid recursion
	ສົ່ງຄືນ &t_dict_any;

    d->dv_copyID = copyID;

    // Use the common type of all values.
    dict_iterate_start(tv, &iter);
    dict_iterate_next(&iter, &value);
    member_type = typval2type(value, copyID, type_gap, TVTT_DO_MEMBER);

    ໃນຂະນະທີ່ (dict_iterate_next(&iter, &value) != NULL)
	common_type(typval2type(value, copyID, type_gap, TVTT_DO_MEMBER),
					member_type, &member_type, type_gap);

    // Reset copyID so that a shared reference to this dict (not a circular
    // reference) can be processed again to get the correct type.
    d->dv_copyID = 0;

    ສົ່ງຄືນ get_dict_type(member_type, type_gap);
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
    ຖ້າ (type == NULL)
	ສົ່ງຄືນ NULL;

    *type = *ufunc->uf_func_type;
    ຖ້າ (type->tt_argcount >= 0 && pt->pt_argc > 0)
    {
	type->tt_argcount -= pt->pt_argc;
	type->tt_min_argcount -= pt->pt_argc;
	ຖ້າ (type->tt_argcount > 0 && func_type_add_arg_types(type,
		    type->tt_argcount, type_gap) == OK)
	    ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < type->tt_argcount; ++i)
		type->tt_args[i] =
		    ufunc->uf_func_type->tt_args[i + pt->pt_argc];
    }
    ຖ້າ (pt->pt_func != NULL)
	type->tt_member = pt->pt_func->uf_ret_type;

    ສົ່ງຄືນ type;
}

/*
 * Get a type_T for a "class" or an "object" typval in "tv".
 */
    static type_T *
oc_typval2type(typval_T *tv)
{
    ຖ້າ (tv->v_type == VAR_CLASS)
    {
	ຖ້າ (tv->vval.v_class == NULL)
	    ສົ່ງຄືນ &t_class;

	ສົ່ງຄືນ &tv->vval.v_class->class_type;
    }

    ຖ້າ (tv->vval.v_object != NULL)
	ສົ່ງຄືນ &tv->vval.v_object->obj_class->class_object_type;

    ສົ່ງຄືນ &t_object_any;
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
    ຈຳນວນເຕັມ		argcount = 0;
    ຈຳນວນເຕັມ		min_argcount = 0;

    ຖ້າ (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != NULL)
    {
	ຖ້າ (tv->vval.v_partial->pt_func != NULL)
	    ufunc = tv->vval.v_partial->pt_func;
	ບໍ່ດັ່ງນັ້ນ
	    name = tv->vval.v_partial->pt_name;
    }
    ບໍ່ດັ່ງນັ້ນ
	name = tv->vval.v_string;

    ຖ້າ (name == NULL && ufunc == NULL)
	ສົ່ງຄືນ &t_func_unknown;

    ຖ້າ (name != NULL)
    {
	ຈຳນວນເຕັມ idx = find_internal_func(name);

	ຖ້າ (idx >= 0)
	{
	    type_T *decl_type;  // unused

	    internal_func_get_argcount(idx, &argcount, &min_argcount);
	    member_type = internal_func_ret_type(idx, 0, NULL, &decl_type,
		    type_gap);
	}
	ບໍ່ດັ່ງນັ້ນ
	{
	    // Check if name contains "<".  If it does, then replace "<" with
	    // NUL and lookup the function and then look up the specific
	    // generic function using the supplied types.
	    char_u *p = generic_func_find_open_bracket(name);
	    ຖ້າ (p == NULL)
		ufunc = find_func(name, FALSE);
	    ບໍ່ດັ່ງນັ້ນ
	    {
		// generic function
		char_u	cc = *p;

		*p = NUL;
		ufunc = find_func(name, FALSE);
		*p = cc;
		ຖ້າ (ufunc != NULL && IS_GENERIC_FUNC(ufunc))
		{
		    ufunc = find_generic_func(ufunc, name, &p);
		    ຖ້າ (ufunc == NULL)
			ສົ່ງຄືນ NULL;
		}
	    }
	}
    }
    ຖ້າ (ufunc != NULL)
    {
	// May need to get the argument types from default values by
	// compiling the function.
	ຖ້າ (ufunc->uf_def_status == UF_TO_BE_COMPILED
		&& compile_def_function(ufunc, TRUE, CT_NONE, NULL)
		== FAIL)
	    ສົ່ງຄືນ NULL;
	ຖ້າ (ufunc->uf_func_type == NULL)
	    set_function_type(ufunc);
	ຖ້າ (ufunc->uf_func_type != NULL)
	{
	    ຖ້າ (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != NULL)
		ສົ່ງຄືນ partial_typval2type(tv, ufunc, type_gap);
	    ສົ່ງຄືນ ufunc->uf_func_type;
	}
    }

    type = get_type_ptr(type_gap);
    ຖ້າ (type == NULL)
	ສົ່ງຄືນ NULL;
    type->tt_type = tv->v_type;
    type->tt_argcount = argcount;
    type->tt_min_argcount = min_argcount;
    ຖ້າ (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != NULL
					&& tv->vval.v_partial->pt_argc > 0)
    {
	type->tt_argcount -= tv->vval.v_partial->pt_argc;
	type->tt_min_argcount -= tv->vval.v_partial->pt_argc;
    }
    type->tt_member = member_type;

    ສົ່ງຄືນ type;
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
typval2type_int(typval_T *tv, ຈຳນວນເຕັມ copyID, garray_T *type_gap, ຈຳນວນເຕັມ flags)
{
    switch (tv->v_type)
    {
	pak_sa VAR_UNKNOWN:
	    ສົ່ງຄືນ &t_unknown;

	pak_sa VAR_ANY:
	    ສົ່ງຄືນ &t_any;

	pak_sa VAR_VOID:
	    ສົ່ງຄືນ &t_void;

	pak_sa VAR_BOOL:
	    ສົ່ງຄືນ &t_bool;

	pak_sa VAR_SPECIAL:
	    ສົ່ງຄືນ special_typval2type(tv);

	pak_sa VAR_NUMBER:
	    ສົ່ງຄືນ &t_number;

	pak_sa VAR_FLOAT:
	    ສົ່ງຄືນ &t_float;

	pak_sa VAR_STRING:
	    ສົ່ງຄືນ &t_string;

	pak_sa VAR_BLOB:
	    ຖ້າ (tv->vval.v_blob == NULL)
		ສົ່ງຄືນ &t_blob_null;
	    ສົ່ງຄືນ &t_blob;

	pak_sa VAR_LIST:
	    ສົ່ງຄືນ list_typval2type(tv, copyID, type_gap, flags);

	pak_sa VAR_TUPLE:
	    ສົ່ງຄືນ tuple_typval2type(tv, copyID, type_gap, flags);

	pak_sa VAR_DICT:
	    ສົ່ງຄືນ dict_typval2type(tv, copyID, type_gap, flags);

	pak_sa VAR_JOB:
	    ສົ່ງຄືນ &t_job;

	pak_sa VAR_CHANNEL:
	    ສົ່ງຄືນ &t_channel;

	pak_sa VAR_CLASS:
	pak_sa VAR_OBJECT:
	    ສົ່ງຄືນ oc_typval2type(tv);

	pak_sa VAR_TYPEALIAS:
	    ສົ່ງຄືນ &t_typealias;

	pak_sa VAR_FUNC:
	pak_sa VAR_PARTIAL:
	    ສົ່ງຄືນ fp_typval2type(tv, type_gap);

	pak_sa VAR_INSTR:
	default:
	    bha_nga;
    }

    ສົ່ງຄືນ NULL;
}

/*
 * Return TRUE if "tv" is not a bool but should be converted to bool.
 */
    ຈຳນວນເຕັມ
need_convert_to_bool(type_T *type, typval_T *tv)
{
    ສົ່ງຄືນ type != NULL && type == &t_bool && tv->v_type != VAR_BOOL
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
typval2type(typval_T *tv, ຈຳນວນເຕັມ copyID, garray_T *type_gap, ຈຳນວນເຕັມ flags)
{
    type_T *type = typval2type_int(tv, copyID, type_gap, flags);

    ຖ້າ (type == NULL)
	ສົ່ງຄືນ NULL;

    ຖ້າ (type != &t_bool && (tv->v_type == VAR_NUMBER
		&& (tv->vval.v_number == 0 || tv->vval.v_number == 1)))
	// Number 0 and 1 and expression with "&&" or "||" can also be used
	// for bool.
	type = &t_number_bool;
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (type != &t_float && tv->v_type == VAR_NUMBER)
	// A number can also be used for float.
	type = &t_number_float;
    ສົ່ງຄືນ type;
}

/*
 * Return TRUE if "type" can be used for a variable declaration.
 * Give an error and return FALSE if not.
 */
    ຈຳນວນເຕັມ
valid_declaration_type(type_T *type)
{
    ຖ້າ (type->tt_type == VAR_SPECIAL  // null, none
	    || type->tt_type == VAR_VOID)
    {
	var_na *tofree = NULL;
	var_na *name = type_name(type, &tofree);
	semsg(_(e_invalid_type_in_variable_declaration_str), name);
	vim_free(tofree);
	ສົ່ງຄືນ FALSE;
    }
    ສົ່ງຄືນ TRUE;
}

/*
 * Get a type_T for a typval_T, used for v: variables.
 * "type_list" is used to temporarily create types in.
 */
    type_T *
typval2type_vimvar(typval_T *tv, garray_T *type_gap)
{
    ຖ້າ (tv->v_type == VAR_LIST)  // e.g. for v:oldfiles
	ສົ່ງຄືນ &t_list_string;
    ຖ້າ (tv->v_type == VAR_DICT)  // e.g. for v:event
	ສົ່ງຄືນ &t_dict_any;
    ສົ່ງຄືນ typval2type(tv, get_copyID(), type_gap, TVTT_DO_MEMBER);
}

    ຈຳນວນເຕັມ
check_typval_arg_type(
	type_T	    *expected,
	typval_T    *actual_tv,
	var_na	    *func_name,
	ຈຳນວນເຕັມ	    arg_idx)
{
    where_T	where = WHERE_INIT;

    ຖ້າ (arg_idx > 0)
    {
	where.wt_index = arg_idx;
	where.wt_kind = WT_ARGUMENT;
    }
    where.wt_func_name = func_name;
    ສົ່ງຄືນ check_typval_type(expected, actual_tv, where);
}

/*
 * Return true if "expected" is list<any> or dict<any> and "actual_tv" is a
 * list or dict of that kind, which matches whatever its values are.
 */
    static bool
matches_listdict_any(type_T *expected, typval_T *actual_tv)
{
    ສົ່ງຄືນ (expected->tt_type == VAR_LIST || expected->tt_type == VAR_DICT)
	    && actual_tv->v_type == expected->tt_type
	    && expected->tt_member != NULL
	    && (expected->tt_member->tt_type == VAR_ANY
		|| expected->tt_member->tt_type == VAR_UNKNOWN);
}

/*
 * Return FAIL if "expected" and "actual" don't match.
 * When "argidx" > 0 it is included in the error message.
 */
    ຈຳນວນເຕັມ
check_typval_type(type_T *expected, typval_T *actual_tv, where_T where)
{
    garray_T	type_list;
    type_T	*actual_type;
    ຈຳນວນເຕັມ		res = FAIL;

    ຖ້າ (expected == NULL)
	ສົ່ງຄືນ OK;  // didn't expect anything.

    // No need to go through the values of a possibly big list or dict.
    ຖ້າ (matches_listdict_any(expected, actual_tv))
	ສົ່ງຄືນ OK;

    ga_init2(&type_list, parimaa_na(type_T *), 10);

    // A null_function and null_partial are special cases, they can be used to
    // clear a variable.
    ຖ້າ ((actual_tv->v_type == VAR_FUNC && actual_tv->vval.v_string == NULL)
	    || (actual_tv->v_type == VAR_PARTIAL
					 && actual_tv->vval.v_partial == NULL))
	actual_type = &t_func_unknown;
    ບໍ່ດັ່ງນັ້ນ
	// When the actual type is list<any> or dict<any> go through the values
	// to possibly get a more specific type.
	actual_type = typval2type(actual_tv, get_copyID(), &type_list,
					  TVTT_DO_MEMBER | TVTT_MORE_SPECIFIC);
    ຖ້າ (actual_type != NULL)
    {
	res = check_type_maybe(expected, actual_type, TRUE, where);
	ຖ້າ (res == MAYBE && !(actual_type->tt_type == VAR_FUNC
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
    ສົ່ງຄືນ res;
}

    void
arg_type_mismatch(type_T *expected, type_T *actual, ຈຳນວນເຕັມ arg_idx)
{
    where_T	where = WHERE_INIT;

    ຖ້າ (arg_idx > 0)
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
	    ຖ້າ (where.wt_func_name == NULL)
		semsg(_(e_variable_nr_type_mismatch_expected_str_but_got_str),
			where.wt_index, typename1, typename2);
	    ບໍ່ດັ່ງນັ້ນ
		semsg(_(e_variable_nr_type_mismatch_expected_str_but_got_str_in_str),
			where.wt_index, typename1, typename2, where.wt_func_name);
	    bha_nga;
	pak_sa WT_ARGUMENT:
	    ຖ້າ (where.wt_func_name == NULL)
		semsg(_(e_argument_nr_type_mismatch_expected_str_but_got_str),
			where.wt_index, typename1, typename2);
	    ບໍ່ດັ່ງນັ້ນ
		semsg(_(e_argument_nr_type_mismatch_expected_str_but_got_str_in_str),
			where.wt_index, typename1, typename2, where.wt_func_name);
	    bha_nga;
	pak_sa WT_CAST:
	pak_sa WT_UNKNOWN:
	    ຖ້າ (where.wt_func_name == NULL)
		semsg(_(e_type_mismatch_expected_str_but_got_str),
			typename1, typename2);
	    ບໍ່ດັ່ງນັ້ນ
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
    static ຈຳນວນເຕັມ
check_tuple_type_maybe(
    type_T	*expected,
    type_T	*actual,
    where_T	where)
{
    ຖ້າ (expected->tt_argcount == -1 || actual->tt_argcount == -1
		|| expected->tt_args == NULL || actual->tt_args == NULL)
	ສົ່ງຄືນ OK;

    // For a non-variadic tuple, the number of items must match
    ຖ້າ (!(expected->tt_flags & TTFLAG_VARARGS)
	    && expected->tt_argcount != actual->tt_argcount)
	ສົ່ງຄືນ FAIL;

    // compare the type of each tuple item
    ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < actual->tt_argcount; ++i)
    {
	type_T	*exp_type;
	type_T	*actual_type;

	ຖ້າ (expected->tt_flags & TTFLAG_VARARGS)
	{
	    ຖ້າ (i < expected->tt_argcount - 1)
		exp_type = expected->tt_args[i];
	    ບໍ່ດັ່ງນັ້ນ
		// For a variadic tuple, the last type is a List.  So use the
		// List member type.
		exp_type = expected->tt_args[expected->tt_argcount - 1]->tt_member;
	}
	ບໍ່ດັ່ງນັ້ນ
	    exp_type = expected->tt_args[i];

	ຖ້າ (actual->tt_flags & TTFLAG_VARARGS)
	{
	    ຖ້າ (i < actual->tt_argcount - 1)
		actual_type = actual->tt_args[i];
	    ບໍ່ດັ່ງນັ້ນ
		// For a variadic tuple, the last type is a List.  So use the
		// List member type.
		actual_type = actual->tt_args[actual->tt_argcount - 1]->tt_member;
	}
	ບໍ່ດັ່ງນັ້ນ
	    actual_type = actual->tt_args[i];

	// Allow for using "any" type for a tuple item
	ຖ້າ (actual->tt_args[i] != &t_any && check_type(exp_type, actual_type,
						FALSE, where) == FAIL)
	    ສົ່ງຄືນ FAIL;
    }

    ສົ່ງຄືນ OK;
}

/*
 * Check if the expected and actual types match for a function
 * Returns OK if "expected" and "actual" are matching function types.
 * Returns FAIL if "expected" and "actual" are different types.
 * Returns MAYBE when a runtime type check is needed.
 */
    static ຈຳນວນເຕັມ
check_func_type_maybe(
    type_T	*expected,
    type_T	*actual,
    where_T	where)
{
    ຈຳນວນເຕັມ ret = OK;

    // If the return type is unknown it can be anything, including
    // nothing, thus there is no point in checking.
    ຖ້າ (expected->tt_member != &t_unknown)
    {
	ຖ້າ (actual->tt_member != NULL
		&& actual->tt_member != &t_unknown)
	{
	    where_T  func_where = where;

	    func_where.wt_kind = WT_METHOD_RETURN;
	    ret = check_type_maybe(expected->tt_member,
		    actual->tt_member, FALSE,
		    func_where);
	}
	ບໍ່ດັ່ງນັ້ນ
	    ret = MAYBE;
    }
    ຖ້າ (ret != FAIL
	    && ((expected->tt_flags & TTFLAG_VARARGS)
		!= (actual->tt_flags & TTFLAG_VARARGS))
	    && expected->tt_argcount != -1)
	ret = FAIL;
    ຖ້າ (ret != FAIL && expected->tt_argcount != -1
	    && actual->tt_min_argcount != -1
	    && (actual->tt_argcount == -1
		|| (actual->tt_argcount < expected->tt_min_argcount
		    || actual->tt_argcount > expected->tt_argcount)))
	ret = FAIL;
    ຖ້າ (ret != FAIL && expected->tt_args != NULL
	    && actual->tt_args != NULL)
    {
	ຈຳນວນເຕັມ i;

	ສຳລັບ (i = 0; i < expected->tt_argcount
		&& i < actual->tt_argcount; ++i)
	{
	    where_T  func_where = where;
	    func_where.wt_kind = WT_METHOD_ARG;

	    // Allow for using "any" argument type, lambda's have them.
	    ຖ້າ (actual->tt_args[i] != &t_any && check_type(
			expected->tt_args[i], actual->tt_args[i], FALSE,
			func_where) == FAIL)
	    {
		ret = FAIL;
		bha_nga;
	    }
	}
    }
    ຖ້າ (ret == OK && expected->tt_argcount >= 0
	    && actual->tt_argcount == -1)
	// check the argument count at runtime
	ret = MAYBE;

    ສົ່ງຄືນ ret;
}

/*
 * Check if the expected and actual types match for an object
 * Returns OK if "expected" and "actual" are matching object types.
 * Returns FAIL if "expected" and "actual" are different types.
 * Returns MAYBE when a runtime type check is needed.
 */
    static ຈຳນວນເຕັມ
check_object_type_maybe(
    type_T	*expected,
    type_T	*actual,
    where_T	where)
{
    ຈຳນວນເຕັມ ret = OK;

    ຖ້າ (actual->tt_type == VAR_ANY)
	ສົ່ງຄືນ MAYBE;	// use runtime type check
    ຖ້າ (actual->tt_type != VAR_OBJECT)
	ສົ່ງຄືນ FAIL;	// don't use tt_class
    ຖ້າ (actual->tt_class == NULL)    // null object
	ສົ່ງຄືນ OK;
    // t_object_any matches any object except for an enum item
    ຖ້າ (expected == &t_object_any && !IS_ENUM(actual->tt_class))
	ສົ່ງຄືນ OK;

    // For object method arguments, do a invariant type check in
    // an extended class.  For all others, do a covariance type check.
    ຖ້າ (where.wt_kind == WT_METHOD_ARG)
    {
	ຖ້າ (actual->tt_class != expected->tt_class)
	    ret = FAIL;
    }
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (!class_instance_of(actual->tt_class, expected->tt_class))
    {
	// Check if this is an up-cast, if so we'll have to check the type at
	// runtime.
	ຖ້າ (where.wt_kind == WT_CAST &&
		class_instance_of(expected->tt_class, actual->tt_class))
	    ret = MAYBE;
	ບໍ່ດັ່ງນັ້ນ
	    ret = FAIL;
    }

    ສົ່ງຄືນ ret;
}

/*
 * Check if the expected and actual types match.
 * Does not allow for assigning "any" to a specific type.
 * When "argidx" > 0 it is included in the error message.
 * Return OK if types match.
 * Return FAIL if types do not match.
 */
    ຈຳນວນເຕັມ
check_type(
	type_T	*expected,
	type_T	*actual,
	ຈຳນວນເຕັມ	give_msg,
	where_T where)
{
    ຈຳນວນເຕັມ ret = check_type_maybe(expected, actual, give_msg, where);

    ສົ່ງຄືນ ret == MAYBE ? OK : ret;
}

/*
 * As check_type() but return MAYBE when a runtime type check should be used
 * when compiling.
 */
    ຈຳນວນເຕັມ
check_type_maybe(
	type_T	*expected,
	type_T	*actual,
	ຈຳນວນເຕັມ	give_msg,
	where_T where)
{
    ຈຳນວນເຕັມ ret = OK;

    // When expected is "unknown" we accept any actual type.
    // When expected is "any" we accept any actual type except "void".
    ຖ້າ (expected->tt_type != VAR_UNKNOWN
	    && !(expected->tt_type == VAR_ANY && actual->tt_type != VAR_VOID))

    {
	// tt_type should match, except that a "partial" can be assigned to a
	// variable with type "func".
	// And "unknown" (using global variable) and "any" need a runtime type
	// check.
	ຖ້າ (!(expected->tt_type == actual->tt_type
		    || actual->tt_type == VAR_UNKNOWN
		    || actual->tt_type == VAR_ANY
		    || (expected->tt_type == VAR_FUNC
					   && actual->tt_type == VAR_PARTIAL)))
	{
	    ຖ້າ (expected->tt_type == VAR_BOOL
					&& (actual->tt_flags & TTFLAG_BOOL_OK))
		// Using number 0 or 1 for bool is OK.
		ສົ່ງຄືນ OK;
	    ຖ້າ (expected->tt_type == VAR_FLOAT
		    && actual->tt_type == VAR_NUMBER
		    && ((expected->tt_flags & TTFLAG_NUMBER_OK)
			     || (actual->tt_flags & TTFLAG_FLOAT_OK)))
		// Using a number where a float is expected is OK here.
		ສົ່ງຄືນ OK;
	    ຖ້າ (expected->tt_type == VAR_LIST
		    && actual->tt_type == VAR_TUPLE
		    && (expected->tt_flags & TTFLAG_TUPLE_OK))
		// Using a tuple where a list is expected is OK here.
		ສົ່ງຄືນ OK;
	    ຖ້າ (give_msg)
		type_mismatch_where(expected, actual, where);
	    ສົ່ງຄືນ FAIL;
	}
	ຖ້າ (expected->tt_type == VAR_DICT || expected->tt_type == VAR_LIST)
	{
	    // "unknown" is used for an empty list or dict
	    ຖ້າ (actual->tt_member != NULL && actual->tt_member != &t_unknown)
		ret = check_type_maybe(expected->tt_member, actual->tt_member,
								 FALSE, where);
	}
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (expected->tt_type == VAR_TUPLE && actual != &t_any)
	    ret =  check_tuple_type_maybe(expected, actual, where);
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (expected->tt_type == VAR_FUNC && actual != &t_any)
	    ret = check_func_type_maybe(expected, actual, where);
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (expected->tt_type == VAR_OBJECT)
	    ret = check_object_type_maybe(expected, actual, where);

	ຖ້າ (ret == FAIL && give_msg)
	    type_mismatch_where(expected, actual, where);
    }

    ຖ້າ (ret == OK && expected->tt_type != VAR_UNKNOWN
	    && expected->tt_type != VAR_ANY
	    && (actual->tt_type == VAR_UNKNOWN || actual->tt_type == VAR_ANY))
	// check the type at runtime
	ret = MAYBE;

    ສົ່ງຄືນ ret;
}

/*
 * Check that the arguments of "type" match "argvars[argcount]".
 * "base_tv" is from "expr->Func()".
 * Return OK/FAIL.
 */
    ຈຳນວນເຕັມ
check_argument_types(
	type_T	    *type,
	typval_T    *argvars,
	ຈຳນວນເຕັມ	    argcount,
	typval_T    *base_tv,
	char_u	    *name)
{
    ຈຳນວນເຕັມ	    varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;
    ຈຳນວນເຕັມ	    i;
    ຈຳນວນເຕັມ	    totcount = argcount + (base_tv == NULL ? 0 : 1);

    ຖ້າ (type->tt_type != VAR_FUNC && type->tt_type != VAR_PARTIAL)
	ສົ່ງຄືນ OK;  // just in case
    ຖ້າ (totcount < type->tt_min_argcount - varargs)
    {
	emsg_funcname(e_not_enough_arguments_for_function_str, name);
	ສົ່ງຄືນ FAIL;
    }
    ຖ້າ (!varargs && type->tt_argcount >= 0 && totcount > type->tt_argcount)
    {
	emsg_funcname(e_too_many_arguments_for_function_str, name);
	ສົ່ງຄືນ FAIL;
    }
    ຖ້າ (type->tt_args == NULL)
	ສົ່ງຄືນ OK;  // cannot check


    ສຳລັບ (i = 0; i < totcount; ++i)
    {
	type_T	    *expected;
	typval_T    *tv;

	ຖ້າ (base_tv != NULL)
	{
	    ຖ້າ (i == 0)
		tv = base_tv;
	    ບໍ່ດັ່ງນັ້ນ
		tv = &argvars[i - 1];
	}
	ບໍ່ດັ່ງນັ້ນ
	    tv = &argvars[i];
	ຖ້າ (varargs && i >= type->tt_argcount - 1)
	{
	    expected = type->tt_args[type->tt_argcount - 1];
	    ຖ້າ (expected != NULL && expected->tt_type == VAR_LIST)
		expected = expected->tt_member;
	    ຖ້າ (expected == NULL)
		expected = &t_any;
	}
	ບໍ່ດັ່ງນັ້ນ
	    expected = type->tt_args[i];

	// check the type, unless the value is v:none
	ຖ້າ ((tv->v_type != VAR_SPECIAL || tv->vval.v_number != VVAL_NONE)
		   && check_typval_arg_type(expected, tv, NULL, i + 1) == FAIL)
	    ສົ່ງຄືນ FAIL;
    }
    ສົ່ງຄືນ OK;
}

// Maximum nesting of types, to avoid running out of stack space.  E.g.
// "list<list<number>>" is nested twice.
#define MAX_TYPE_DEPTH	1000

// Current number of levels of parse_type() and the deepest level reached while
// parsing the current type.  The nesting is one less than the number of
// levels.
static ຈຳນວນເຕັມ	parse_depth = 0;
static ຈຳນວນເຕັມ	parse_max_depth = 0;

/*
 * Skip over type in list<type>, dict<type> or tuple<type>.
 * Returns a pointer to the character after the type.  "syn_error" is set to
 * TRUE on syntax error.
 */
    static char_u *
skip_member_type(char_u *start, char_u *p, ຈຳນວນເຕັມ *syn_error)
{
    ຖ້າ (STRNCMP("tuple", start, 5) == 0)
    {
	// handle tuple<{type1}, {type2}, ....<type>>
	p = skipwhite(p + 1);
	ໃນຂະນະທີ່ (*p != '>' && *p != NUL)
	{
	    char_u *sp = p;

	    ຖ້າ (STRNCMP(p, "...", 3) == 0)
		p += 3;
	    p = skip_type(p, TRUE);
	    ຖ້າ (p == sp)
	    {
		*syn_error = TRUE;
		ສົ່ງຄືນ p;  // syntax error
	    }
	    ຖ້າ (*p == ',')
		p = skipwhite(p + 1);
	}
	ຖ້າ (*p == '>')
	    p++;
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	p = skipwhite(p);
	p = skip_type(skipwhite(p + 1), FALSE);
	p = skipwhite(p);
	ຖ້າ (*p == '>')
	    ++p;
    }

    ສົ່ງຄືນ p;
}

/*
 * Skip over a function type.  Returns a pointer to the character after the
 * type.  "syn_error" is set to TRUE on syntax error.
 */
    static char_u *
skip_func_type(char_u *p, ຈຳນວນເຕັມ *syn_error)
{
    ຖ້າ (*p == '(')
    {
	// handle func(args): type
	++p;
	ໃນຂະນະທີ່ (*p != ')' && *p != NUL)
	{
	    char_u *sp = p;

	    ຖ້າ (STRNCMP(p, "...", 3) == 0)
		p += 3;
	    p = skip_type(p, TRUE);
	    ຖ້າ (p == sp)
	    {
		*syn_error = TRUE;
		ສົ່ງຄືນ p;  // syntax error
	    }
	    ຖ້າ (*p == ',')
		p = skipwhite(p + 1);
	}
	ຖ້າ (*p == ')')
	{
	    ຖ້າ (p[1] == ':')
		p = skip_type(skipwhite(p + 2), FALSE);
	    ບໍ່ດັ່ງນັ້ນ
		++p;
	}
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	// handle func: return_type
	p = skip_type(skipwhite(p + 1), FALSE);
    }

    ສົ່ງຄືນ p;
}

/*
 * Inner part of skip_type().
 */
    static char_u *
skip_type_inner(char_u *start, ຈຳນວນເຕັມ optional)
{
    char_u	*p = start;
    ຈຳນວນເຕັມ		syn_error = FALSE;

    ຖ້າ (optional && *p == '?')
	++p;

    // Also skip over "." for imported classes: "import.ClassName".
    ໃນຂະນະທີ່ (ASCII_ISALNUM(*p) || *p == '_' || *p == '.')
	++p;

    // Skip over "<type>"; this is permissive about white space.
    ຖ້າ (*skipwhite(p) == '<')
    {
	p = skip_member_type(start, p, &syn_error);
	ຖ້າ (syn_error)
	    ສົ່ງຄືນ p;
    }
    ບໍ່ດັ່ງນັ້ນ ຖ້າ ((*p == '(' || (*p == ':' && VIM_ISWHITE(p[1])))
					     && STRNCMP("func", start, 4) == 0)
    {
	// skip over function type
	p = skip_func_type(p, &syn_error);
	ຖ້າ (syn_error)
	    ສົ່ງຄືນ p;
    }

    ສົ່ງຄືນ p;
}

/*
 * Skip over a type definition and return a pointer to just after it.
 * When "optional" is TRUE then a leading "?" is accepted.
 */
    char_u *
skip_type(char_u *start, ຈຳນວນເຕັມ optional)
{
    static ຈຳນວນເຕັມ	skip_depth = 0;
    char_u	*p = start;

    // Too deeply nested: skip to the matching ">" without recursing, an
    // error is given when parsing the type.
    ຖ້າ (skip_depth > MAX_TYPE_DEPTH)
    {
	ຈຳນວນເຕັມ	depth = 0;

	ສຳລັບ ( ; *p != NUL; ++p)
	{
	    ຖ້າ (*p == '<')
		++depth;
	    ບໍ່ດັ່ງນັ້ນ ຖ້າ (*p == '>')
	    {
		ຖ້າ (depth == 0)
		    bha_nga;
		ຖ້າ (--depth == 0)
		    ສົ່ງຄືນ p + 1;
	    }
	    ບໍ່ດັ່ງນັ້ນ ຖ້າ (depth == 0 && !ASCII_ISALNUM(*p) && *p != '_'
								&& *p != '.')
		bha_nga;
	}
	ສົ່ງຄືນ p;
    }

    ++skip_depth;
    p = skip_type_inner(start, optional);
    --skip_depth;
    ສົ່ງຄືນ p;
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
	ຈຳນວນເຕັມ	    give_error,
	var_na	    *info,
	ufunc_T	    *ufunc,
	cctx_T	    *cctx)
{
    char_u  *arg_start = *arg;
    type_T  *member_type;
    ຈຳນວນເຕັມ	    prev_called_emsg = called_emsg;

    ຖ້າ (**arg != '<')
    {
	ຖ້າ (give_error)
	{
	    ຖ້າ (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    ບໍ່ດັ່ງນັ້ນ
		semsg(_(e_missing_type_after_str), info);
	}
	ສົ່ງຄືນ NULL;
    }
    *arg = skipwhite(*arg + 1);

    member_type = parse_type(arg, type_gap, ufunc, cctx, give_error);
    ຖ້າ (member_type == NULL || !valid_declaration_type(member_type))
	ສົ່ງຄືນ NULL;

    *arg = skipwhite(*arg);
    ຖ້າ (**arg != '>' && called_emsg == prev_called_emsg)
    {
	ຖ້າ (give_error)
	    semsg(_(e_missing_gt_after_type_str), arg_start);
	ສົ່ງຄືນ NULL;
    }
    ++*arg;

    ຖ້າ (type->tt_type == VAR_LIST)
	ສົ່ງຄືນ get_list_type(member_type, type_gap);
    ສົ່ງຄືນ get_dict_type(member_type, type_gap);
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
    ຈຳນວນເຕັມ		give_error,
    ufunc_T	*ufunc,
    cctx_T	*cctx)
{
    char_u  *p;
    type_T  *type;
    type_T  *ret_type = &t_unknown;
    ຈຳນວນເຕັມ	argcount = -1;
    ຈຳນວນເຕັມ	flags = 0;
    ຈຳນວນເຕັມ	first_optional = -1;
    type_T	*arg_type[MAX_FUNC_ARGS + 1];

    // func({type}, ...{type}): {type}
    *arg += len;
    ຖ້າ (**arg == '(')
    {
	// "func" may or may not return a value, "func()" does
	// not return a value.
	ret_type = &t_void;

	p = ++*arg;
	argcount = 0;
	ໃນຂະນະທີ່ (*p != NUL && *p != ')')
	{
	    ຖ້າ (*p == '?')
	    {
		ຖ້າ (first_optional == -1)
		    first_optional = argcount;
		++p;
	    }
	    ບໍ່ດັ່ງນັ້ນ ຖ້າ (STRNCMP(p, "...", 3) == 0)
	    {
		flags |= TTFLAG_VARARGS;
		p += 3;
	    }
	    ບໍ່ດັ່ງນັ້ນ ຖ້າ (first_optional != -1)
	    {
		ຖ້າ (give_error)
		    emsg(_(e_mandatory_argument_after_optional_argument));
		ສົ່ງຄືນ NULL;
	    }

	    type = parse_type(&p, type_gap, ufunc, cctx, give_error);
	    ຖ້າ (type == NULL || !valid_declaration_type(type))
		ສົ່ງຄືນ NULL;
	    ຖ້າ ((flags & TTFLAG_VARARGS) != 0 && type->tt_type != VAR_LIST)
	    {
		var_na *tofree;
		semsg(_(e_variable_arguments_type_must_be_list_str),
			type_name(type, &tofree));
		vim_free(tofree);
		ສົ່ງຄືນ NULL;
	    }
	    arg_type[argcount++] = type;

	    // Nothing comes after "...{type}".
	    ຖ້າ (flags & TTFLAG_VARARGS)
		bha_nga;

	    ຖ້າ (*p != ',' && *skipwhite(p) == ',')
	    {
		ຖ້າ (give_error)
		    semsg(_(e_no_white_space_allowed_before_str_str),
			    ",", p);
		ສົ່ງຄືນ NULL;
	    }
	    ຖ້າ (*p == ',')
	    {
		++p;
		ຖ້າ (!VIM_ISWHITE(*p))
		{
		    ຖ້າ (give_error)
			semsg(_(e_white_space_required_after_str_str),
				",", p - 1);
		    ສົ່ງຄືນ NULL;
		}
	    }
	    p = skipwhite(p);
	    ຖ້າ (argcount == MAX_FUNC_ARGS)
	    {
		ຖ້າ (give_error)
		    emsg(_(e_too_many_argument_types));
		ສົ່ງຄືນ NULL;
	    }
	}

	p = skipwhite(p);
	ຖ້າ (*p != ')')
	{
	    ຖ້າ (give_error)
		emsg(_(e_missing_closing_paren));
	    ສົ່ງຄືນ NULL;
	}
	*arg = p + 1;
    }
    ຖ້າ (**arg == ':')
    {
	// parse return type
	++*arg;
	ຖ້າ (!VIM_ISWHITE(**arg) && give_error)
	    semsg(_(e_white_space_required_after_str_str), ":", *arg - 1);
	*arg = skipwhite(*arg);
	ret_type = parse_type(arg, type_gap, ufunc, cctx, give_error);
	ຖ້າ (ret_type == NULL)
	    ສົ່ງຄືນ NULL;
    }
    ຖ້າ (flags == 0 && first_optional == -1 && argcount <= 0)
	type = get_func_type(ret_type, argcount, type_gap);
    ບໍ່ດັ່ງນັ້ນ
    {
	type = alloc_func_type(ret_type, argcount, type_gap);
	type->tt_flags = flags;
	ຖ້າ (argcount > 0)
	{
	    type->tt_argcount = argcount;
	    type->tt_min_argcount = first_optional == -1
						? argcount : first_optional;
	    ຖ້າ (func_type_add_arg_types(type, argcount, type_gap) == FAIL)
		ສົ່ງຄືນ NULL;
	    mch_memmove(type->tt_args, arg_type, parimaa_na(type_T *) * argcount);
	}
    }

    ສົ່ງຄືນ type;
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
    ຈຳນວນເຕັມ		give_error,
    ufunc_T	*ufunc,
    cctx_T	*cctx)
{
    char_u	*p;
    type_T	*type;
    type_T	*ret_type = NULL;
    ຈຳນວນເຕັມ		typecount = -1;
    ຈຳນວນເຕັມ		flags = 0;
    garray_T	tuple_types_ga;

    ga_init2(&tuple_types_ga, parimaa_na(type_T *), 10);

    // tuple<{type}, {type}>
    // tuple<{type}, ...{type}>
    ຖ້າ (**arg != '<')
    {
	ຖ້າ (give_error)
	{
	    ຖ້າ (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    ບໍ່ດັ່ງນັ້ນ
		semsg(_(e_missing_type_after_str), "tuple");
	}

	// only "tuple" is specified
	ສົ່ງຄືນ NULL;
    }

    p = ++*arg;
    typecount = 0;
    ໃນຂະນະທີ່ (*p != NUL && *p != '>')
    {
	ຖ້າ (STRNCMP(p, "...", 3) == 0)
	{
	    flags |= TTFLAG_VARARGS;
	    p += 3;
	}

	type = parse_type(&p, type_gap, ufunc, cctx, give_error);
	ຖ້າ (type == NULL || !valid_declaration_type(type))
	    goto on_err;

	ຖ້າ ((flags & TTFLAG_VARARGS) != 0 && type->tt_type != VAR_LIST)
	{
	    var_na *tofree;
	    semsg(_(e_variadic_tuple_must_end_with_list_type_str),
						type_name(type, &tofree));
	    vim_free(tofree);
	    goto on_err;
	}

	// Add the item type
	ຖ້າ (ga_grow(&tuple_types_ga, 1) == FAIL)
	    goto on_err;
	((type_T **)tuple_types_ga.ga_data)[tuple_types_ga.ga_len] = type;
	tuple_types_ga.ga_len++;
	typecount++;

	// Nothing comes after "...{type}".
	ຖ້າ (flags & TTFLAG_VARARGS)
	    bha_nga;

	ຖ້າ (*p != ',' && *skipwhite(p) == ',')
	{
	    ຖ້າ (give_error)
		semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
	    goto on_err;
	}
	ຖ້າ (*p == ',')
	{
	    ++p;
	    ຖ້າ (!VIM_ISWHITE(*p))
	    {
		ຖ້າ (give_error)
		    semsg(_(e_white_space_required_after_str_str),
			    ",", p - 1);
		goto on_err;
	    }
	}
	p = skipwhite(p);
    }

    p = skipwhite(p);
    ຖ້າ (*p != '>' || typecount <= 0)
    {
	ຖ້າ (give_error)
	    semsg(_(e_missing_type_after_str), p);
	goto on_err;
    }
    *arg = p + 1;

    ret_type = alloc_tuple_type(typecount, type_gap);
    ຖ້າ (ret_type == &t_any
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

    ສົ່ງຄືນ ret_type;
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
    ຈຳນວນເຕັມ		give_error,
    cctx_T	*cctx)
{
    char_u	*arg_start = *arg;
    type_T	*object_type;
    ຈຳນວນເຕັມ		prev_called_emsg = called_emsg;

    // object<X> or object<any>
    ຖ້າ (**arg != '<')
    {
	ຖ້າ (give_error)
	{
	    ຖ້າ (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    ບໍ່ດັ່ງນັ້ນ
		semsg(_(e_missing_type_after_str), "object");
	}

	// only "object" is specified
	ສົ່ງຄືນ NULL;
    }

    // skip spaces following "object<"
    *arg = skipwhite(*arg + 1);

    object_type = parse_type(arg, type_gap, NULL, cctx, give_error);
    ຖ້າ (object_type == NULL)
	ສົ່ງຄືນ NULL;

    *arg = skipwhite(*arg);
    ຖ້າ (**arg != '>' && called_emsg == prev_called_emsg)
    {
	ຖ້າ (give_error)
	    semsg(_(e_missing_gt_after_type_str), arg_start);
	ສົ່ງຄືນ NULL;
    }
    ++*arg;

    ຖ້າ (object_type->tt_type == VAR_ANY)
	ສົ່ງຄືນ &t_object_any;

    ຖ້າ (object_type->tt_type != VAR_OBJECT)
    {
	// specified type is not a class
	ຖ້າ (give_error)
	    semsg(_(e_class_name_not_found_str), arg_start);
	ສົ່ງຄືນ NULL;
    }

    ສົ່ງຄືນ object_type;
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
    ຈຳນວນເຕັມ		give_error,
    ufunc_T	*ufunc,
    cctx_T	*cctx)
{
    ຈຳນວນເຕັມ		did_emsg_before = did_emsg;
    typval_T	tv;

    tv.v_type = VAR_UNKNOWN;
    ຖ້າ (eval_variable_import(*arg, &tv) == OK)
    {
	ຖ້າ (tv.v_type == VAR_CLASS && tv.vval.v_class != NULL)
	{
	    type_T *type = get_type_ptr(type_gap);
	    ຖ້າ (type != NULL)
	    {
		// Although the name is that of a class or interface, the type
		// uses will be an object.
		type->tt_type = VAR_OBJECT;
		type->tt_class = tv.vval.v_class;
		clear_tv(&tv);

		*arg += len;
		// Skip over ".ClassName".
		ໃນຂະນະທີ່ (ASCII_ISALNUM(**arg) || **arg == '_' || **arg == '.')
		    ++*arg;

		ສົ່ງຄືນ type;
	    }
	}
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (tv.v_type == VAR_TYPEALIAS)
	{
	    // user defined type
	    // The type alias may itself be a nested type, check the resulting
	    // nesting depth.
	    ຈຳນວນເຕັມ	depth = parse_depth - 1
				+ MAX(tv.vval.v_typealias->ta_depth, 1);
	    ຖ້າ (depth - 1 > MAX_TYPE_DEPTH)
	    {
		clear_tv(&tv);
		ຖ້າ (give_error)
		    emsg(_(e_type_nested_too_deep));
		ສົ່ງຄືນ NULL;
	    }
	    ຖ້າ (depth > parse_max_depth)
		parse_max_depth = depth;

	    type_T *type = copy_type(tv.vval.v_typealias->ta_type, type_gap);
	    *arg += len;
	    clear_tv(&tv);
	    // Skip over ".TypeName".
	    ໃນຂະນະທີ່ (ASCII_ISALNUM(**arg) || **arg == '_' || **arg == '.')
		++*arg;
	    ສົ່ງຄືນ type;
	}

	clear_tv(&tv);
    }

    // Check whether it is a generic type
    type_T *type = find_generic_type(*arg, len, ufunc, cctx);
    ຖ້າ (type != NULL)
    {
	*arg += len;
	ສົ່ງຄືນ type;
    }

    ຖ້າ (give_error && (did_emsg == did_emsg_before))
    {
	char_u	*p = skip_type(*arg, FALSE);
	var_na	cc = *p;

	*p = NUL;
	semsg(_(e_type_not_recognized_str), *arg);
	*p = cc;
    }

    ສົ່ງຄືນ NULL;
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
    ຈຳນວນເຕັມ		give_error)
{
    char_u  *p = *arg;
    size_t  len;

    // Skip over the first word.
    ໃນຂະນະທີ່ (ASCII_ISALNUM(*p) || *p == '_')
	++p;
    len = p - *arg;

    switch (**arg)
    {
	pak_sa 'a':
	    ຖ້າ (len == 3 && STRNCMP(*arg, "any", len) == 0)
	    {
		*arg += len;
		ສົ່ງຄືນ &t_any;
	    }
	    bha_nga;
	pak_sa 'b':
	    ຖ້າ (len == 4 && STRNCMP(*arg, "bool", len) == 0)
	    {
		*arg += len;
		ສົ່ງຄືນ &t_bool;
	    }
	    ຖ້າ (len == 4 && STRNCMP(*arg, "blob", len) == 0)
	    {
		*arg += len;
		ສົ່ງຄືນ &t_blob;
	    }
	    bha_nga;
	pak_sa 'c':
	    ຖ້າ (len == 7 && STRNCMP(*arg, "channel", len) == 0)
	    {
		*arg += len;
		ສົ່ງຄືນ &t_channel;
	    }
	    bha_nga;
	pak_sa 'd':
	    ຖ້າ (len == 4 && STRNCMP(*arg, "dict", len) == 0)
	    {
		*arg += len;
		ສົ່ງຄືນ parse_type_member(arg, &t_dict_any, type_gap,
						give_error, "dict", ufunc,
						cctx);
	    }
	    bha_nga;
	pak_sa 'f':
	    ຖ້າ (len == 5 && STRNCMP(*arg, "float", len) == 0)
	    {
		*arg += len;
		ສົ່ງຄືນ &t_float;
	    }
	    ຖ້າ (len == 4 && STRNCMP(*arg, "func", len) == 0)
		ສົ່ງຄືນ parse_type_func(arg, len, type_gap, give_error, ufunc,
									cctx);
	    bha_nga;
	pak_sa 'j':
	    ຖ້າ (len == 3 && STRNCMP(*arg, "job", len) == 0)
	    {
		*arg += len;
		ສົ່ງຄືນ &t_job;
	    }
	    bha_nga;
	pak_sa 'l':
	    ຖ້າ (len == 4 && STRNCMP(*arg, "list", len) == 0)
	    {
		*arg += len;
		ສົ່ງຄືນ parse_type_member(arg, &t_list_any, type_gap,
						give_error, "list", ufunc,
						cctx);
	    }
	    bha_nga;
	pak_sa 'n':
	    ຖ້າ (len == 6 && STRNCMP(*arg, "number", len) == 0)
	    {
		*arg += len;
		ສົ່ງຄືນ &t_number;
	    }
	    bha_nga;
	pak_sa 'o':
	    ຖ້າ (len == 6 && STRNCMP(*arg, "object", len) == 0)
	    {
		*arg += len;
		ສົ່ງຄືນ parse_type_object(arg, type_gap, give_error, cctx);
	    }
	    bha_nga;
	pak_sa 's':
	    ຖ້າ (len == 6 && STRNCMP(*arg, "string", len) == 0)
	    {
		*arg += len;
		ສົ່ງຄືນ &t_string;
	    }
	    bha_nga;
	pak_sa 't':
	    ຖ້າ (len == 5 && STRNCMP(*arg, "tuple", len) == 0)
	    {
		*arg += len;
		ສົ່ງຄືນ parse_type_tuple(arg, type_gap, give_error, ufunc,
									cctx);
	    }
	    bha_nga;
	pak_sa 'v':
	    ຖ້າ (len == 4 && STRNCMP(*arg, "void", len) == 0)
	    {
		*arg += len;
		ສົ່ງຄືນ &t_void;
	    }
	    bha_nga;
    }

    // User defined type
    ສົ່ງຄືນ parse_type_user_defined(arg, len, type_gap, give_error, ufunc,
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
    ຈຳນວນເຕັມ		give_error)
{
    type_T	*type;

    ຖ້າ (parse_depth > MAX_TYPE_DEPTH)
    {
	ຖ້າ (give_error)
	    emsg(_(e_type_nested_too_deep));
	ສົ່ງຄືນ NULL;
    }
    ຖ້າ (parse_depth == 0)
	parse_max_depth = 0;
    ++parse_depth;
    ຖ້າ (parse_depth > parse_max_depth)
	parse_max_depth = parse_depth;
    type = parse_type_inner(arg, type_gap, ufunc, cctx, give_error);
    --parse_depth;
    ສົ່ງຄືນ type;
}

/*
 * Return the number of levels of the type last parsed by parse_type().
 */
    ຈຳນວນເຕັມ
parse_type_depth(void)
{
    ສົ່ງຄືນ parse_max_depth;
}

/*
 * Check if "type1" and "type2" are exactly the same.
 * "flags" can have ETYPE_ARG_UNKNOWN, which means that an unknown argument
 * type in "type1" is accepted.
 */
    ຈຳນວນເຕັມ
equal_type(type_T *type1, type_T *type2, ຈຳນວນເຕັມ flags)
{
    ຈຳນວນເຕັມ i;

    ຖ້າ (type1 == NULL || type2 == NULL)
	ສົ່ງຄືນ FALSE;
    ຖ້າ (type1->tt_type != type2->tt_type)
	ສົ່ງຄືນ FALSE;
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
	    ສົ່ງຄືນ type1->tt_class == type2->tt_class;
	pak_sa VAR_LIST:
	pak_sa VAR_DICT:
	    ສົ່ງຄືນ equal_type(type1->tt_member, type2->tt_member, flags);
	pak_sa VAR_TUPLE:
	    ຖ້າ (type1->tt_argcount != type2->tt_argcount)
		ສົ່ງຄືນ FALSE;
	    ຖ້າ (type1->tt_argcount < 0
			   || type1->tt_args == NULL || type2->tt_args == NULL)
		ສົ່ງຄືນ TRUE;
	    ສຳລັບ (i = 0; i < type1->tt_argcount; ++i)
		ຖ້າ ((flags & ETYPE_ARG_UNKNOWN) == 0
			&& !equal_type(type1->tt_args[i], type2->tt_args[i],
									flags))
		    ສົ່ງຄືນ FALSE;
	    ສົ່ງຄືນ TRUE;
	pak_sa VAR_FUNC:
	pak_sa VAR_PARTIAL:
	    ຖ້າ (!equal_type(type1->tt_member, type2->tt_member, flags)
		    || type1->tt_argcount != type2->tt_argcount)
		ສົ່ງຄືນ FALSE;
	    ຖ້າ (type1->tt_argcount < 0
			   || type1->tt_args == NULL || type2->tt_args == NULL)
		ສົ່ງຄືນ TRUE;
	    ສຳລັບ (i = 0; i < type1->tt_argcount; ++i)
		ຖ້າ ((flags & ETYPE_ARG_UNKNOWN) == 0
			&& !equal_type(type1->tt_args[i], type2->tt_args[i],
									flags))
		    ສົ່ງຄືນ FALSE;
	    ສົ່ງຄືນ TRUE;
    }
    ສົ່ງຄືນ TRUE;
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
    ຖ້າ (type1 == &t_func_unknown)
    {
	*dest = type2;
	ສົ່ງຄືນ;
    }
    ຖ້າ (type2 == &t_func_unknown)
    {
	*dest = type1;
	ສົ່ງຄືນ;
    }

    common_type(type1->tt_member, type2->tt_member, &common, type_gap);
    ຖ້າ (type1->tt_argcount == type2->tt_argcount
	    && type1->tt_argcount >= 0)
    {
	ຈຳນວນເຕັມ argcount = type1->tt_argcount;
	ຈຳນວນເຕັມ i;

	*dest = alloc_func_type(common, argcount, type_gap);
	ຖ້າ (type1->tt_args != NULL && type2->tt_args != NULL)
	{
	    ຖ້າ (func_type_add_arg_types(*dest, argcount,
			type_gap) == OK)
		ສຳລັບ (i = 0; i < argcount; ++i)
		    common_type(type1->tt_args[i], type2->tt_args[i],
			    &(*dest)->tt_args[i], type_gap);
	}
    }
    ບໍ່ດັ່ງນັ້ນ
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
    ຖ້າ (equal_type(type1, type2, 0))
    {
	*dest = type1;
	ສົ່ງຄືນ;
    }

    // If either is VAR_UNKNOWN use the other type.  An empty list/dict has no
    // specific type.
    ຖ້າ (type1 == NULL || type1->tt_type == VAR_UNKNOWN)
    {
	*dest = type2;
	ສົ່ງຄືນ;
    }
    ຖ້າ (type2 == NULL || type2->tt_type == VAR_UNKNOWN)
    {
	*dest = type1;
	ສົ່ງຄືນ;
    }

    ຖ້າ (type1->tt_type == type2->tt_type)
    {
	ຖ້າ (type1->tt_type == VAR_LIST
		|| type1->tt_type == VAR_DICT)
	{
	    type_T *common;

	    common_type(type1->tt_member, type2->tt_member, &common, type_gap);
	    ຖ້າ (type1->tt_type == VAR_LIST)
		*dest = get_list_type(common, type_gap);
	    ບໍ່ດັ່ງນັ້ນ
		*dest = get_dict_type(common, type_gap);
	    ສົ່ງຄືນ;
	}
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (type1->tt_type == VAR_FUNC)
	{
	    common_type_var_func(type1, type2, dest, type_gap);
	    ສົ່ງຄືນ;
	}
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (type1->tt_type == VAR_OBJECT)
	{
	    *dest = &t_object_any;
	    ສົ່ງຄືນ;
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
    ຖ້າ (type->tt_type == VAR_TUPLE)
    {
	ຖ້າ (type->tt_argcount != 1)
	    ສົ່ງຄືນ &t_any;

	ຖ້າ (type->tt_flags & TTFLAG_VARARGS)
	    ສົ່ງຄືນ type->tt_args[0]->tt_member;
	ບໍ່ດັ່ງນັ້ນ
	    ສົ່ງຄືນ type->tt_args[0];
    }

    // For "any" the item type is not known.
    ສົ່ງຄືນ type->tt_member == NULL ? &t_any : type->tt_member;
}

/*
 * Push an entry onto the type stack.  "type" used both for the current type
 * and the declared type.
 * Returns FAIL when out of memory.
 */
    ຈຳນວນເຕັມ
push_type_stack(cctx_T *cctx, type_T *type)
{
    ສົ່ງຄືນ push_type_stack2(cctx, type, type);
}

/*
 * Push an entry onto the type stack.  "type" is the current type, "decl_type"
 * is the declared type.
 * Returns FAIL when out of memory.
 */
    ຈຳນວນເຕັມ
push_type_stack2(cctx_T *cctx, type_T *type, type_T *decl_type)
{
    garray_T	*stack = &cctx->ctx_type_stack;
    type2_T	*typep;

    ຖ້າ (GA_GROW_FAILS(stack, 1))
	ສົ່ງຄືນ FAIL;
    typep = ((type2_T *)stack->ga_data) + stack->ga_len;
    typep->type_curr = type;
    typep->type_decl = decl_type;
    ++stack->ga_len;
    ສົ່ງຄືນ OK;
}

/*
 * Set the type of the top of the stack to "type".
 */
    void
set_type_on_stack(cctx_T *cctx, type_T *type, ຈຳນວນເຕັມ offset)
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
get_type_on_stack(cctx_T *cctx, ຈຳນວນເຕັມ offset)
{
    garray_T	*stack = &cctx->ctx_type_stack;

    ຖ້າ (offset + 1 > stack->ga_len)
	ສົ່ງຄືນ &t_unknown;
    ສົ່ງຄືນ (((type2_T *)stack->ga_data) + stack->ga_len - offset - 1)
								   ->type_curr;
}

/*
 * Get the declared type from the type stack.  If "offset" is zero the one at
 * the top,
 * if "offset" is one the type above that, etc.
 * Returns &t_unknown if there is no such stack entry.
 */
    type_T *
get_decl_type_on_stack(cctx_T *cctx, ຈຳນວນເຕັມ offset)
{
    garray_T	*stack = &cctx->ctx_type_stack;

    ຖ້າ (offset + 1 > stack->ga_len)
	ສົ່ງຄືນ &t_unknown;
    ສົ່ງຄືນ (((type2_T *)stack->ga_data) + stack->ga_len - offset - 1)
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
	ຈຳນວນເຕັມ	    count,
	ຈຳນວນເຕັມ	    skip,
	cctx_T	    *cctx)
{
    garray_T	*stack = &cctx->ctx_type_stack;
    type2_T	*typep;
    garray_T    *type_gap = cctx->ctx_type_list;
    ຈຳນວນເຕັມ		i;
    type_T	*result;
    type_T	*type;

    // Use "unknown" for an empty list or dict.
    ຖ້າ (count == 0)
	ສົ່ງຄືນ &t_unknown;
    // Find the common type from following items.
    typep = ((type2_T *)stack->ga_data) + stack->ga_len;
    result = &t_unknown;
    ສຳລັບ (i = 0; i < count; ++i)
    {
	type = (typep -((count - i) * skip) + skip - 1)->type_curr;
	ຖ້າ (check_type_is_value(type) == FAIL)
	    ສົ່ງຄືນ NULL;
	ຖ້າ (result != &t_any)
	    common_type(type, result, &result, type_gap);
    }

    ສົ່ງຄືນ result;
}

/*
 * Get the types of items in a tuple on the stack of "cctx".
 * Returns the number of types.  Returns -1 on failure.
 */
    ຈຳນວນເຕັມ
get_tuple_type_from_stack(
    ຈຳນວນເຕັມ		count,
    garray_T	*tuple_types_gap,
    cctx_T	*cctx)
{
    garray_T	*stack = &cctx->ctx_type_stack;
    type2_T	*typep;
    type_T	*type = NULL;

    // Use "unknown" for an empty tuple
    ຖ້າ (count == 0)
	ສົ່ງຄືນ 0;

    // Find the common type from following items.
    typep = ((type2_T *)stack->ga_data) + stack->ga_len;
    ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < count; i++)
    {
	type = (typep - (count - i))->type_curr;
	ຖ້າ (check_type_is_value(type) == FAIL)
	    ສົ່ງຄືນ -1;
	ຖ້າ (ga_grow(tuple_types_gap, 1) == FAIL)
	    ສົ່ງຄືນ -1;
	((type_T **)tuple_types_gap->ga_data)[tuple_types_gap->ga_len] = type;
	tuple_types_gap->ga_len++;
    }

    ສົ່ງຄືນ tuple_types_gap->ga_len;
}

    var_na *
vartype_name(vartype_T type)
{
    switch (type)
    {
	pak_sa VAR_UNKNOWN: bha_nga;
	pak_sa VAR_ANY: ສົ່ງຄືນ "any";
	pak_sa VAR_VOID: ສົ່ງຄືນ "void";
	pak_sa VAR_SPECIAL: ສົ່ງຄືນ "special";
	pak_sa VAR_BOOL: ສົ່ງຄືນ "bool";
	pak_sa VAR_NUMBER: ສົ່ງຄືນ "number";
	pak_sa VAR_FLOAT: ສົ່ງຄືນ "float";
	pak_sa VAR_STRING: ສົ່ງຄືນ "string";
	pak_sa VAR_BLOB: ສົ່ງຄືນ "blob";
	pak_sa VAR_JOB: ສົ່ງຄືນ "job";
	pak_sa VAR_CHANNEL: ສົ່ງຄືນ "channel";
	pak_sa VAR_LIST: ສົ່ງຄືນ "list";
	pak_sa VAR_TUPLE: ສົ່ງຄືນ "tuple";
	pak_sa VAR_DICT: ສົ່ງຄືນ "dict";
	pak_sa VAR_INSTR: ສົ່ງຄືນ "instr";
	pak_sa VAR_CLASS: ສົ່ງຄືນ "class";
	pak_sa VAR_OBJECT: ສົ່ງຄືນ "object";
	pak_sa VAR_TYPEALIAS: ສົ່ງຄືນ "typealias";

	pak_sa VAR_FUNC:
	pak_sa VAR_PARTIAL: ສົ່ງຄືນ "func";
    }
    ສົ່ງຄືນ "unknown";
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

    ຖ້າ (type->tt_member->tt_type == VAR_UNKNOWN)
	member_name = type_name(&t_any, &member_free);
    ບໍ່ດັ່ງນັ້ນ
	member_name = type_name(type->tt_member, &member_free);

    size_t len = STRLEN(name) + STRLEN(member_name) + 3;
    *tofree = alloc(len);
    ຖ້າ (*tofree == NULL)
    {
	vim_free(member_free);
	ສົ່ງຄືນ name;
    }

    vim_snprintf(*tofree, len, "%s<%s>", name, member_name);
    vim_free(member_free);
    ສົ່ງຄືນ *tofree;
}

/*
 * Return the type name of a tuple.
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    static var_na *
type_name_tuple(type_T *type, var_na **tofree)
{
    garray_T    ga;
    ຈຳນວນເຕັມ		i;
    ຈຳນວນເຕັມ		varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;
    var_na	*arg_free = NULL;

    ga_init2(&ga, 1, 100);
    ຖ້າ (ga_grow(&ga, 20) == FAIL)
	goto failed;
    STRCPY(ga.ga_data, "tuple<");
    ga.ga_len += 6;

    ຖ້າ (type->tt_argcount <= 0)
	// empty tuple
	GA_CONCAT_LITERAL(&ga, "any");
    ບໍ່ດັ່ງນັ້ນ
    {
	ຖ້າ (type->tt_args == NULL)
	    GA_CONCAT_LITERAL(&ga, "[unknown]");
	ບໍ່ດັ່ງນັ້ນ
	{
	    ສຳລັບ (i = 0; i < type->tt_argcount; ++i)
	    {
		string_T    arg_type;

		arg_type.string = (char_u *)type_name(type->tt_args[i], &arg_free);
		ຖ້າ (i > 0)
		{
		    STRCPY((var_na *)ga.ga_data + ga.ga_len, ", ");
		    ga.ga_len += 2;
		}
		arg_type.length = STRLEN(arg_type.string);
		ຖ້າ (ga_grow(&ga, (ຈຳນວນເຕັມ)arg_type.length + 8) == FAIL)
		    goto failed;
		ຖ້າ (varargs && i == type->tt_argcount - 1)
		    GA_CONCAT_LITERAL(&ga, "...");
		ga_concat_len(&ga, arg_type.string, arg_type.length);
		VIM_CLEAR(arg_free);
	    }
	}
    }

    STRCPY((var_na *)ga.ga_data + ga.ga_len, ">");
    *tofree = ga.ga_data;
    ສົ່ງຄືນ ga.ga_data;

failed:
    vim_free(arg_free);
    ga_clear(&ga);
    ສົ່ງຄືນ "[unknown]";
}

/*
 * Return the type name of a Class (class<name>) or Object (object<name>).
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    static var_na *
type_name_class_or_obj(var_na *name, type_T *type, var_na **tofree)
{
    string_T	class_name;

    ຖ້າ (type->tt_class != NULL)
    {
	class_name.string = type->tt_class->class_name.string;
	class_name.length = type->tt_class->class_name.length;
	ຖ້າ (IS_ENUM(type->tt_class))
	    name = "enum";
    }
    ບໍ່ດັ່ງນັ້ນ
	STR_LITERAL_SET(class_name, "any");

    size_t len = STRLEN(name) + class_name.length + 3;
    *tofree = alloc(len);
    ຖ້າ (*tofree == NULL)
	ສົ່ງຄືນ name;

    vim_snprintf(*tofree, len, "%s<%s>", name, class_name.string);
    ສົ່ງຄືນ *tofree;
}

/*
 * Return the type name of a function.
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    static var_na *
type_name_func(type_T *type, var_na **tofree)
{
    garray_T    ga;
    ຈຳນວນເຕັມ		i;
    ຈຳນວນເຕັມ		varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;
    var_na	*arg_free = NULL;

    ga_init2(&ga, 1, 100);
    ຖ້າ (ga_grow(&ga, 20) == FAIL)
	goto failed;
    STRCPY(ga.ga_data, "func(");
    ga.ga_len += 5;

    ສຳລັບ (i = 0; i < type->tt_argcount; ++i)
    {
	string_T    arg_type;

	ຖ້າ (type->tt_args == NULL)
	    STR_LITERAL_SET(arg_type, "[unknown]");
	ບໍ່ດັ່ງນັ້ນ
	{
	    arg_type.string = (char_u *)type_name(type->tt_args[i], &arg_free);
	    arg_type.length = STRLEN(arg_type.string);
	}
	ຖ້າ (i > 0)
	{
	    STRCPY((var_na *)ga.ga_data + ga.ga_len, ", ");
	    ga.ga_len += 2;
	}
	ຖ້າ (ga_grow(&ga, (ຈຳນວນເຕັມ)arg_type.length + 8) == FAIL)
	    goto failed;
	ຖ້າ (varargs && i == type->tt_argcount - 1)
	    GA_CONCAT_LITERAL(&ga, "...");
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (i >= type->tt_min_argcount)
	    *((var_na *)ga.ga_data + ga.ga_len++) = '?';
	ga_concat_len(&ga, arg_type.string, arg_type.length);
	VIM_CLEAR(arg_free);
    }
    ຖ້າ (type->tt_argcount < 0)
	// any number of arguments
	GA_CONCAT_LITERAL(&ga, "...");

    ຖ້າ (type->tt_member == &t_void)
	STRCPY((var_na *)ga.ga_data + ga.ga_len, ")");
    ບໍ່ດັ່ງນັ້ນ
    {
	var_na *ret_free = NULL;
	var_na *ret_name = type_name(type->tt_member, &ret_free);
	ຈຳນວນເຕັມ  len;

	len = (ຈຳນວນເຕັມ)STRLEN(ret_name) + 4;
	ຖ້າ (ga_grow(&ga, len) == FAIL)
	{
	    vim_free(ret_free);
	    goto failed;
	}
	STRCPY((var_na *)ga.ga_data + ga.ga_len, "): ");
	STRCPY((var_na *)ga.ga_data + ga.ga_len + 3, ret_name);
	vim_free(ret_free);
    }
    *tofree = ga.ga_data;
    ສົ່ງຄືນ ga.ga_data;

failed:
    vim_free(arg_free);
    ga_clear(&ga);
    ສົ່ງຄືນ "[unknown]";
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
    ຖ້າ (type == NULL)
	ສົ່ງຄືນ "[unknown]";
    name = vartype_name(type->tt_type);

    switch (type->tt_type)
    {
	pak_sa VAR_LIST:
	pak_sa VAR_DICT:
	    ສົ່ງຄືນ type_name_list_or_dict(name, type, tofree);

	pak_sa VAR_TUPLE:
	    ສົ່ງຄືນ type_name_tuple(type, tofree);

	pak_sa VAR_CLASS:
	pak_sa VAR_OBJECT:
	    ສົ່ງຄືນ type_name_class_or_obj(name, type, tofree);

	pak_sa VAR_FUNC:
	    ສົ່ງຄືນ type_name_func(type, tofree);

	default:
	    bha_nga;
    }

    ສົ່ງຄືນ name;
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
    ຖ້າ (argvars[0].v_type == VAR_TYPEALIAS)
    {
	type = copy_type(argvars[0].vval.v_typealias->ta_type, &type_list);
	// A type alias for a class has the type set to VAR_OBJECT.  Change it
	// to VAR_CLASS, so that the name is "typealias<class<xxx>>"
	ຖ້າ (type->tt_type == VAR_OBJECT)
	    type->tt_type = VAR_CLASS;
    }
    ບໍ່ດັ່ງນັ້ນ
	type = typval2type(argvars, get_copyID(), &type_list, TVTT_DO_MEMBER);
    name = type_name(type, &tofree);
    ຖ້າ (argvars[0].v_type == VAR_TYPEALIAS)
    {
	vim_snprintf((var_na *)IObuff, IOSIZE, "typealias<%s>", name);
	rettv->vval.v_string = vim_strsave((char_u *)IObuff);
	ຖ້າ (tofree != NULL)
	    vim_free(tofree);
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	ຖ້າ (tofree != NULL)
	    rettv->vval.v_string = (char_u *)tofree;
	ບໍ່ດັ່ງນັ້ນ
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
    ຈຳນວນເຕັມ
check_typval_is_value(typval_T *tv)
{
    ຖ້າ (tv == NULL)
	ສົ່ງຄືນ OK;

    switch (tv->v_type)
    {
	pak_sa VAR_CLASS:
	    {
		class_T *cl = tv->vval.v_class;
		char_u *class_name = (cl == NULL) ? (char_u *)""
							: cl->class_name.string;
		ຖ້າ (cl != NULL && IS_ENUM(cl))
		    semsg(_(e_using_enum_as_value_str), class_name);
		ບໍ່ດັ່ງນັ້ນ
		    semsg(_(e_using_class_as_value_str), class_name);
	    }
	    ສົ່ງຄືນ FAIL;

	pak_sa VAR_TYPEALIAS:
	    semsg(_(e_using_typealias_as_value_str),
		    tv->vval.v_typealias->ta_name);
	    ສົ່ງຄືນ FAIL;

	default:
	    bha_nga;
    }
    ສົ່ງຄືນ OK;
}

/*
 * Same as above, except check type_T.
 */
    ຈຳນວນເຕັມ
check_type_is_value(type_T *type)
{
    ຖ້າ (type == NULL)
	ສົ່ງຄືນ OK;
    switch (type->tt_type)
    {
	pak_sa VAR_CLASS:
	    ຖ້າ (type->tt_class != NULL && IS_ENUM(type->tt_class))
		semsg(_(e_using_enum_as_value_str),
			type->tt_class->class_name.string);
	    ບໍ່ດັ່ງນັ້ນ
		semsg(_(e_using_class_as_value_str),
			type->tt_class == NULL ? (char_u *)""
			: type->tt_class->class_name.string);
	    ສົ່ງຄືນ FAIL;

	pak_sa VAR_TYPEALIAS:
	    // TODO: Not sure what could be done here to get a name.
	    //       Maybe an optional argument?
	    emsg(_(e_using_typealias_as_var_val));
	    ສົ່ງຄືນ FAIL;

	default:
	    bha_nga;
    }
    ສົ່ງຄືນ OK;
}

#endif // FEAT_EVAL
