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
#ضمن "vim.h"

#אם defined(FEAT_EVAL)

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

    אם (ga_grow(type_gap, 1) == FAIL)
	החזר عدم;
    type = ALLOC_CLEAR_ONE(type_T);
    אם (type == عدم)
	החזר عدم;

    ((type_T **)type_gap->ga_data)[type_gap->ga_len] = type;
    ++type_gap->ga_len;
    החזר type;
}

/*
 * Make a shallow copy of "type".
 * When allocation fails returns "type".
 */
    type_T *
copy_type(type_T *type, garray_T *type_gap)
{
    type_T *copy = get_type_ptr(type_gap);

    אם (copy == عدم)
	החזר type;
    *copy = *type;
    copy->tt_flags &= ~TTFLAG_STATIC;

    אם (type->tt_args != عدم
	   && func_type_add_arg_types(copy, type->tt_argcount, type_gap) == OK)
	עבור (صحيح i = 0; i < type->tt_argcount; ++i)
	    copy->tt_args[i] = type->tt_args[i];

    החזר copy;
}

/*
 * Inner part of copy_type_deep().
 * When allocation fails returns "type".
 */
    ساكن type_T *
copy_type_deep_rec(type_T *type, garray_T *type_gap, garray_T *seen_types)
{
    עבור (صحيح i = 0; i < seen_types->ga_len; ++i)
	אם (((type_T **)seen_types->ga_data)[i * 2] == type)
	    // seen this type before, return the copy we made
	    החזר ((type_T **)seen_types->ga_data)[i * 2 + 1];

    type_T *copy = copy_type(type, type_gap);
    אם (ga_grow(seen_types, 1) == FAIL)
	החזר copy;
    ((type_T **)seen_types->ga_data)[seen_types->ga_len * 2] = type;
    ((type_T **)seen_types->ga_data)[seen_types->ga_len * 2 + 1] = copy;
    ++seen_types->ga_len;

    אם (copy->tt_member != عدم)
	copy->tt_member = copy_type_deep_rec(copy->tt_member,
							 type_gap, seen_types);
    אם (type->tt_args != عدم)
	עבור (صحيح i = 0; i < type->tt_argcount; ++i)
	    copy->tt_args[i] = copy_type_deep_rec(copy->tt_args[i],
							 type_gap, seen_types);

    החזר copy;
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
    ga_init2(&seen_types, حجم(type_T *) * 2, 20);

    type_T *res = copy_type_deep_rec(type, type_gap, &seen_types);

    ga_clear(&seen_types);
    החזר res;
}

    فراغ
clear_type_list(garray_T *gap)
{
    כלעוד (gap->ga_len > 0)
	vim_free(((type_T **)gap->ga_data)[--gap->ga_len]);
    ga_clear(gap);
}

    فراغ
clear_func_type_list(garray_T *gap, type_T **func_type)
{
    כלעוד (gap->ga_len > 0)
    {
	// func_type pointing to the uf_type_list, so reset pointer
	אם (*func_type == ((type_T **)gap->ga_data)[--gap->ga_len])
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

    אם (type == عدم)
	החזר عدم;

    // A fixed type never contains allocated types, return as-is.
    אם (type->tt_flags & TTFLAG_STATIC)
	החזר type;

    ret = ALLOC_ONE(type_T);
    אם (ret == عدم)
	החזר عدم;
    *ret = *type;

    אם (ret->tt_member != عدم)
	ret->tt_member = alloc_type(ret->tt_member);

    אם (type->tt_argcount > 0 && type->tt_args != عدم)
    {
	صحيح i;

	ret->tt_args = ALLOC_MULT(type_T *, type->tt_argcount);
	אם (ret->tt_args != عدم)
	    עבור (i = 0; i < type->tt_argcount; ++i)
		ret->tt_args[i] = alloc_type(type->tt_args[i]);
    }
    אחרת
	ret->tt_args = عدم;

    החזר ret;
}

/*
 * Free a type that was created with alloc_type().
 */
    فراغ
free_type(type_T *type)
{
    صحيح i;

    אם (type == عدم || (type->tt_flags & TTFLAG_STATIC))
	החזר;
    אם (type->tt_args != عدم)
    {
	עבור (i = 0; i < type->tt_argcount; ++i)
	    free_type(type->tt_args[i]);
	vim_free(type->tt_args);
    }

    free_type(type->tt_member);

    vim_free(type);
}

/*
 * Return TRUE if "type" is to be recursed into for setting the type.
 */
    ساكن صحيح
set_tv_type_recurse(type_T *type)
{
    החזר type->tt_member != عدم
		&& (type->tt_member->tt_type == VAR_DICT
				|| type->tt_member->tt_type == VAR_LIST)
		&& type->tt_member->tt_member != عدم
		&& type->tt_member->tt_member != &t_any
		&& type->tt_member->tt_member != &t_unknown;
}

/*
 * Set the type of Dict "d" to "type"
 */
    ساكن فراغ
set_tv_type_dict(dict_T *d, type_T *type)
{
    אם (d->dv_type == type)
	החזר;

    free_type(d->dv_type);
    d->dv_type = alloc_type(type);

    // Need to recursively set the type of dict items?
    אם (!set_tv_type_recurse(type))
	החזר;

    صحيح		todo = (صحيح)d->dv_hashtab.ht_used;
    hashitem_T	*hi;
    dictitem_T	*di;

    FOR_ALL_HASHTAB_ITEMS(&d->dv_hashtab, hi, todo)
    {
	אם (!HASHITEM_EMPTY(hi))
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
    ساكن فراغ
set_tv_type_list(list_T *l, type_T *type)
{
    אם (l->lv_type == type)
	החזר;

    free_type(l->lv_type);
    l->lv_type = alloc_type(type);

    // Need to recursively set the type of list items?
    אם (l->lv_first == &range_list_item || !set_tv_type_recurse(type))
	החזר;

    listitem_T	*li;

    FOR_ALL_LIST_ITEMS(l, li)
	set_tv_type(&li->li_tv, type->tt_member);
}

/*
 * Set the type of Tuple "tuple" to "type"
 */
    ساكن فراغ
set_tv_type_tuple(tuple_T *tuple, type_T *type)
{
    אם (tuple->tv_type == type)
	החזר;

    free_type(tuple->tv_type);
    tuple->tv_type = alloc_type(type);

    אם (type->tt_argcount <= 0)
	החזר;

    // recursively set the type of list items
    type_T	*item_type;
    עבור (صحيح i = 0; i < tuple_len(tuple); i++)
    {
	אם ((type->tt_flags & TTFLAG_VARARGS) && (i >= type->tt_argcount - 1))
	    // For a variadic tuple, the last type is a List.  So use the
	    // List member type.
	    item_type = type->tt_args[type->tt_argcount - 1]->tt_member;
	אחרת
	    item_type = type->tt_args[i];

	set_tv_type(TUPLE_ITEM(tuple, i), item_type);
    }
}

/*
 * Set the type of "tv" to "type" if it is a list or tuple or dict.
 */
    فراغ
set_tv_type(typval_T *tv, type_T *type)
{
    אם (type->tt_type == VAR_ANY)
	// If the variable type is "any", then keep the value type.
	// e.g.  var x: any = [1, 2] or var y: any = {v: 1}
	החזר;

    אם (tv->v_type == VAR_DICT && tv->vval.v_dict != عدم)
	set_tv_type_dict(tv->vval.v_dict, type);
    אחרת אם (tv->v_type == VAR_LIST && tv->vval.v_list != عدم)
	set_tv_type_list(tv->vval.v_list, type);
    אחרת אם (tv->v_type == VAR_TUPLE && tv->vval.v_tuple != عدم)
	set_tv_type_tuple(tv->vval.v_tuple, type);
}

/*
 * For a tuple type, reserve space for "typecount" types (including the
 * repeated type).
 */
    ساكن صحيح
tuple_type_add_types(
    type_T	*tupletype,
    صحيح		typecount,
    garray_T	*type_gap)
{
    // To make it easy to free the space needed for the types, add the
    // pointer to type_gap.
    אם (ga_grow(type_gap, 1) == FAIL)
	החזר FAIL;
    tupletype->tt_args = alloc_clear_id(حجم(type_T *) * typecount,
							  aid_tuple_type_args);
    אם (tupletype->tt_args == عدم)
	החזר FAIL;
    ((type_T **)type_gap->ga_data)[type_gap->ga_len] =
						(فراغ *)tupletype->tt_args;
    ++type_gap->ga_len;
    החזר OK;
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
    אם (member_type == عدم || (member_type->tt_type == VAR_ANY
					&& !IS_GENERIC_TYPE(member_type)))
	החזר &t_list_any;
    אם (member_type->tt_type == VAR_VOID
	    || member_type->tt_type == VAR_UNKNOWN)
	החזר &t_list_empty;
    אם (member_type->tt_type == VAR_BOOL)
	החזר &t_list_bool;
    אם (member_type->tt_type == VAR_NUMBER)
	החזר &t_list_number;
    אם (member_type->tt_type == VAR_STRING)
	החזר &t_list_string;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    אם (type == عدم)
	החזר &t_any;
    type->tt_type = VAR_LIST;
    type->tt_member = member_type;
    type->tt_argcount = 0;
    type->tt_args = عدم;
    החזר type;
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
    صحيح		typecount = tuple_types_gap->ga_len;

    // recognize commonly used types
    אם (typecount == 0)
	החזר &t_tuple_any;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    אם (type == عدم)
	החזר &t_any;
    type->tt_type = VAR_TUPLE;
    type->tt_member = عدم;
    אם (typecount > 0)
    {
	אם (tuple_type_add_types(type, typecount, type_gap) == FAIL)
	    החזר &t_any;
	mch_memmove(type->tt_args, tuple_types, حجم(type_T *) * typecount);
    }
    type->tt_argcount = typecount;
    type->tt_flags = 0;

    החזר type;
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
    אם (member_type == عدم || (member_type->tt_type == VAR_ANY
					&& !IS_GENERIC_TYPE(member_type)))
	החזר &t_dict_any;
    אם (member_type->tt_type == VAR_VOID
	    || member_type->tt_type == VAR_UNKNOWN)
	החזר &t_dict_empty;
    אם (member_type->tt_type == VAR_BOOL)
	החזר &t_dict_bool;
    אם (member_type->tt_type == VAR_NUMBER)
	החזר &t_dict_number;
    אם (member_type->tt_type == VAR_STRING)
	החזר &t_dict_string;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    אם (type == عدم)
	החזר &t_any;
    type->tt_type = VAR_DICT;
    type->tt_member = member_type;
    type->tt_argcount = 0;
    type->tt_args = عدم;
    החזר type;
}

/*
 * Allocate a new type for a function.
 */
    type_T *
alloc_func_type(type_T *ret_type, صحيح argcount, garray_T *type_gap)
{
    type_T *type = get_type_ptr(type_gap);

    אם (type == عدم)
	החזר &t_any;
    type->tt_type = VAR_FUNC;
    type->tt_member = ret_type == عدم ? &t_unknown : ret_type;
    type->tt_argcount = argcount;
    type->tt_args = عدم;
    החזר type;
}

/*
 * Allocate a new type for a tuple.
 */
    ساكن type_T *
alloc_tuple_type(صحيح typecount, garray_T *type_gap)
{
    type_T *type = get_type_ptr(type_gap);

    אם (type == عدم)
	החזר &t_any;
    type->tt_type = VAR_TUPLE;
    type->tt_member = عدم;
    type->tt_argcount = typecount;
    type->tt_args = عدم;
    החזר type;
}

/*
 * Get a function type, based on the return type "ret_type".
 * "argcount" must be -1 or 0, a predefined type can be used.
 */
    type_T *
get_func_type(type_T *ret_type, صحيح argcount, garray_T *type_gap)
{
    // recognize commonly used types
    אם (ret_type == &t_unknown || ret_type == عدم)
    {
	// (argcount == 0) is not possible
	החזר &t_func_unknown;
    }
    אם (ret_type == &t_void)
    {
	אם (argcount == 0)
	    החזר &t_func_0_void;
	אחרת
	    החזר &t_func_void;
    }
    אם (ret_type == &t_any)
    {
	אם (argcount == 0)
	    החזר &t_func_0_any;
	אחרת
	    החזר &t_func_any;
    }
    אם (ret_type == &t_number)
    {
	אם (argcount == 0)
	    החזר &t_func_0_number;
	אחרת
	    החזר &t_func_number;
    }
    אם (ret_type == &t_string)
    {
	אם (argcount == 0)
	    החזר &t_func_0_string;
	אחרת
	    החזר &t_func_string;
    }

    החזר alloc_func_type(ret_type, argcount, type_gap);
}

/*
 * For a function type, reserve space for "argcount" argument types (including
 * vararg).
 */
    صحيح
func_type_add_arg_types(
	type_T	    *functype,
	صحيح	    argcount,
	garray_T    *type_gap)
{
    // To make it easy to free the space needed for the argument types, add the
    // pointer to type_gap.
    אם (ga_grow(type_gap, 1) == FAIL)
	החזר FAIL;
    functype->tt_args = ALLOC_CLEAR_MULT(type_T *, argcount);
    אם (functype->tt_args == عدم)
	החזר FAIL;
    ((type_T **)type_gap->ga_data)[type_gap->ga_len] =
						     (فراغ *)functype->tt_args;
    ++type_gap->ga_len;
    החזר OK;
}

/*
 * Return TRUE if "type" is NULL, any or unknown.
 * This also works for const (comparing with &t_any and &t_unknown doesn't).
 */
    صحيح
type_any_or_unknown(type_T *type)
{
    החזר type == عدم || type->tt_type == VAR_ANY
					|| type->tt_type == VAR_UNKNOWN;
}

/*
 * Get a type_T for a "special" typval in "tv".
 */
    ساكن type_T *
special_typval2type(typval_T *tv)
{
    بدل (tv->vval.v_number)
    {
	حالة VVAL_NULL:
	    החזר &t_null;

	حالة VVAL_NONE:
	    החזר &t_none;

	حالة VVAL_TRUE:
	حالة VVAL_FALSE:
	    החזר &t_bool;

	افتراضي:
	    החזר &t_unknown;
    }
}

/*
 * Get a type_T for a List typval in "tv".
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    ساكن type_T *
list_typval2type(typval_T *tv, صحيح copyID, garray_T *type_gap, صحيح flags)
{
    list_T	*l = tv->vval.v_list;
    listitem_T	*li;
    type_T	*member_type = عدم;

    // An empty list has type list<unknown>, unless the type was specified
    // and is not list<any>.  This matters when assigning to a variable
    // with a specific list type.
    אם (l == عدم || (l->lv_first == عدم
		&& (l->lv_type == عدم || l->lv_type->tt_member == &t_any)))
	החזר &t_list_empty;

    אם ((flags & TVTT_DO_MEMBER) == 0)
	החזר &t_list_any;

    // If the type is list<any> go through the members, it may end up a
    // more specific type.
    אם (l->lv_type != عدم && (l->lv_first == عدم
		|| (flags & TVTT_MORE_SPECIFIC) == 0
		|| l->lv_type->tt_member != &t_any))
	// make a copy, lv_type may be freed if the list is freed
	החזר copy_type_deep(l->lv_type, type_gap);

    אם (l->lv_first == &range_list_item)
	החזר &t_list_number;

    אם (l->lv_copyID == copyID)
	// avoid recursion
	החזר &t_list_any;

    l->lv_copyID = copyID;

    // Use the common type of all members.
    member_type = typval2type(&l->lv_first->li_tv, copyID, type_gap,
							TVTT_DO_MEMBER);
    עבור (li = l->lv_first->li_next; li != عدم; li = li->li_next)
	common_type(typval2type(&li->li_tv, copyID, type_gap, TVTT_DO_MEMBER),
					member_type, &member_type, type_gap);

    // Reset copyID so that a shared reference to this list (not a circular
    // reference) can be processed again to get the correct type.
    l->lv_copyID = 0;

    החזר get_list_type(member_type, type_gap);
}

/*
 * Get a type_T for a Tuple typval in "tv".
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    ساكن type_T *
tuple_typval2type(typval_T *tv, صحيح copyID, garray_T *type_gap, صحيح flags)
{
    tuple_T	*tuple = tv->vval.v_tuple;
    صحيح		len = tuple_len(tuple);
    type_T	*type = عدم;

    // An empty tuple has type tuple<unknown>, unless the type was specified
    // and is not tuple<any>.  This matters when assigning to a variable
    // with a specific tuple type.
    אם (tuple == عدم || (len == 0 && (tuple->tv_type == عدم
				|| tuple->tv_type->tt_argcount == 0)))
	החזר &t_tuple_empty;

    אם ((flags & TVTT_DO_MEMBER) == 0)
	החזר &t_tuple_any;

    // If the type is tuple<any> go through the members, it may end up a
    // more specific type.
    אם (tuple->tv_type != عدم && (len == 0
					|| (flags & TVTT_MORE_SPECIFIC) == 0))
	// make a copy, tv_type may be freed if the tuple is freed
	החזר copy_type_deep(tuple->tv_type, type_gap);

    אם (tuple->tv_copyID == copyID)
	// avoid recursion
	החזר &t_tuple_any;

    tuple->tv_copyID = copyID;

    garray_T	tuple_types_ga;
    ga_init2(&tuple_types_ga, حجم(type_T *), 10);
    עבור (صحيح i = 0; i < len; i++)
    {
	type = typval2type(TUPLE_ITEM(tuple, i), copyID, type_gap,
							TVTT_DO_MEMBER);
	אם (ga_grow(&tuple_types_ga, 1) == FAIL)
	{
	    ga_clear(&tuple_types_ga);
	    // Reset copyID so that a shared reference to this tuple can be
	    // processed again.
	    tuple->tv_copyID = 0;
	    החזר عدم;
	}
	((type_T **)tuple_types_ga.ga_data)[tuple_types_ga.ga_len] = type;
	tuple_types_ga.ga_len++;
    }

    type_T *tuple_type = get_tuple_type(&tuple_types_ga, type_gap);
    ga_clear(&tuple_types_ga);

    // Reset copyID so that a shared reference to this tuple (not a circular
    // reference) can be processed again to get the correct type.
    tuple->tv_copyID = 0;

    החזר tuple_type;
}

/*
 * Get a type_T for a Dict typval in "tv".
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    ساكن type_T *
dict_typval2type(typval_T *tv, صحيح copyID, garray_T *type_gap, صحيح flags)
{
    dict_iterator_T	iter;
    typval_T		*value;
    dict_T		*d = tv->vval.v_dict;
    type_T		*member_type = عدم;

    אם (d == عدم || (d->dv_hashtab.ht_used == 0 && d->dv_type == عدم))
	החזר &t_dict_empty;

    אם ((flags & TVTT_DO_MEMBER) == 0)
	החזר &t_dict_any;

    // If the type is dict<any> go through the members, it may end up a
    // more specific type.
    אם (d->dv_type != عدم && (d->dv_hashtab.ht_used == 0
		|| (flags & TVTT_MORE_SPECIFIC) == 0
		|| d->dv_type->tt_member != &t_any))
	החזר d->dv_type;

    אם (d->dv_copyID == copyID)
	// avoid recursion
	החזר &t_dict_any;

    d->dv_copyID = copyID;

    // Use the common type of all values.
    dict_iterate_start(tv, &iter);
    dict_iterate_next(&iter, &value);
    member_type = typval2type(value, copyID, type_gap, TVTT_DO_MEMBER);

    כלעוד (dict_iterate_next(&iter, &value) != عدم)
	common_type(typval2type(value, copyID, type_gap, TVTT_DO_MEMBER),
					member_type, &member_type, type_gap);

    // Reset copyID so that a shared reference to this dict (not a circular
    // reference) can be processed again to get the correct type.
    d->dv_copyID = 0;

    החזר get_dict_type(member_type, type_gap);
}

/*
 * Get a type_T for a "partial" typval in "tv".
 */
    ساكن type_T *
partial_typval2type(typval_T *tv, ufunc_T *ufunc, garray_T *type_gap)
{
    partial_T *pt = tv->vval.v_partial;
    type_T  *type;

    type = get_type_ptr(type_gap);
    אם (type == عدم)
	החזר عدم;

    *type = *ufunc->uf_func_type;
    אם (type->tt_argcount >= 0 && pt->pt_argc > 0)
    {
	type->tt_argcount -= pt->pt_argc;
	type->tt_min_argcount -= pt->pt_argc;
	אם (type->tt_argcount > 0 && func_type_add_arg_types(type,
		    type->tt_argcount, type_gap) == OK)
	    עבור (صحيح i = 0; i < type->tt_argcount; ++i)
		type->tt_args[i] =
		    ufunc->uf_func_type->tt_args[i + pt->pt_argc];
    }
    אם (pt->pt_func != عدم)
	type->tt_member = pt->pt_func->uf_ret_type;

    החזר type;
}

/*
 * Get a type_T for a "class" or an "object" typval in "tv".
 */
    ساكن type_T *
oc_typval2type(typval_T *tv)
{
    אם (tv->v_type == VAR_CLASS)
    {
	אם (tv->vval.v_class == عدم)
	    החזר &t_class;

	החזר &tv->vval.v_class->class_type;
    }

    אם (tv->vval.v_object != عدم)
	החזר &tv->vval.v_object->obj_class->class_object_type;

    החזר &t_object_any;
}

/*
 * Get a type_T for a "function" or a "partial"
 */
    ساكن type_T *
fp_typval2type(typval_T *tv, garray_T *type_gap)
{
    char_u	*name = عدم;
    ufunc_T	*ufunc = عدم;
    type_T	*type;
    type_T	*member_type = عدم;
    صحيح		argcount = 0;
    صحيح		min_argcount = 0;

    אם (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != عدم)
    {
	אם (tv->vval.v_partial->pt_func != عدم)
	    ufunc = tv->vval.v_partial->pt_func;
	אחרת
	    name = tv->vval.v_partial->pt_name;
    }
    אחרת
	name = tv->vval.v_string;

    אם (name == عدم && ufunc == عدم)
	החזר &t_func_unknown;

    אם (name != عدم)
    {
	صحيح idx = find_internal_func(name);

	אם (idx >= 0)
	{
	    type_T *decl_type;  // unused

	    internal_func_get_argcount(idx, &argcount, &min_argcount);
	    member_type = internal_func_ret_type(idx, 0, عدم, &decl_type,
		    type_gap);
	}
	אחרת
	{
	    // Check if name contains "<".  If it does, then replace "<" with
	    // NUL and lookup the function and then look up the specific
	    // generic function using the supplied types.
	    char_u *p = generic_func_find_open_bracket(name);
	    אם (p == عدم)
		ufunc = find_func(name, FALSE);
	    אחרת
	    {
		// generic function
		char_u	cc = *p;

		*p = NUL;
		ufunc = find_func(name, FALSE);
		*p = cc;
		אם (ufunc != عدم && IS_GENERIC_FUNC(ufunc))
		{
		    ufunc = find_generic_func(ufunc, name, &p);
		    אם (ufunc == عدم)
			החזר عدم;
		}
	    }
	}
    }
    אם (ufunc != عدم)
    {
	// May need to get the argument types from default values by
	// compiling the function.
	אם (ufunc->uf_def_status == UF_TO_BE_COMPILED
		&& compile_def_function(ufunc, TRUE, CT_NONE, عدم)
		== FAIL)
	    החזר عدم;
	אם (ufunc->uf_func_type == عدم)
	    set_function_type(ufunc);
	אם (ufunc->uf_func_type != عدم)
	{
	    אם (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != عدم)
		החזר partial_typval2type(tv, ufunc, type_gap);
	    החזר ufunc->uf_func_type;
	}
    }

    type = get_type_ptr(type_gap);
    אם (type == عدم)
	החזר عدم;
    type->tt_type = tv->v_type;
    type->tt_argcount = argcount;
    type->tt_min_argcount = min_argcount;
    אם (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != عدم
					&& tv->vval.v_partial->pt_argc > 0)
    {
	type->tt_argcount -= tv->vval.v_partial->pt_argc;
	type->tt_min_argcount -= tv->vval.v_partial->pt_argc;
    }
    type->tt_member = member_type;

    החזר type;
}

/*
 * Get a type_T for a typval_T.
 * "type_gap" is used to temporarily create types in.
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    ساكن type_T *
typval2type_int(typval_T *tv, صحيح copyID, garray_T *type_gap, صحيح flags)
{
    بدل (tv->v_type)
    {
	حالة VAR_UNKNOWN:
	    החזר &t_unknown;

	حالة VAR_ANY:
	    החזר &t_any;

	حالة VAR_VOID:
	    החזר &t_void;

	حالة VAR_BOOL:
	    החזר &t_bool;

	حالة VAR_SPECIAL:
	    החזר special_typval2type(tv);

	حالة VAR_NUMBER:
	    החזר &t_number;

	حالة VAR_FLOAT:
	    החזר &t_float;

	حالة VAR_STRING:
	    החזר &t_string;

	حالة VAR_BLOB:
	    אם (tv->vval.v_blob == عدم)
		החזר &t_blob_null;
	    החזר &t_blob;

	حالة VAR_LIST:
	    החזר list_typval2type(tv, copyID, type_gap, flags);

	حالة VAR_TUPLE:
	    החזר tuple_typval2type(tv, copyID, type_gap, flags);

	حالة VAR_DICT:
	    החזר dict_typval2type(tv, copyID, type_gap, flags);

	حالة VAR_JOB:
	    החזר &t_job;

	حالة VAR_CHANNEL:
	    החזר &t_channel;

	حالة VAR_CLASS:
	حالة VAR_OBJECT:
	    החזר oc_typval2type(tv);

	حالة VAR_TYPEALIAS:
	    החזר &t_typealias;

	حالة VAR_FUNC:
	حالة VAR_PARTIAL:
	    החזר fp_typval2type(tv, type_gap);

	حالة VAR_INSTR:
	افتراضي:
	    اكسر;
    }

    החזר عدم;
}

/*
 * Return TRUE if "tv" is not a bool but should be converted to bool.
 */
    صحيح
need_convert_to_bool(type_T *type, typval_T *tv)
{
    החזר type != عدم && type == &t_bool && tv->v_type != VAR_BOOL
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
typval2type(typval_T *tv, صحيح copyID, garray_T *type_gap, صحيح flags)
{
    type_T *type = typval2type_int(tv, copyID, type_gap, flags);

    אם (type == عدم)
	החזר عدم;

    אם (type != &t_bool && (tv->v_type == VAR_NUMBER
		&& (tv->vval.v_number == 0 || tv->vval.v_number == 1)))
	// Number 0 and 1 and expression with "&&" or "||" can also be used
	// for bool.
	type = &t_number_bool;
    אחרת אם (type != &t_float && tv->v_type == VAR_NUMBER)
	// A number can also be used for float.
	type = &t_number_float;
    החזר type;
}

/*
 * Return TRUE if "type" can be used for a variable declaration.
 * Give an error and return FALSE if not.
 */
    صحيح
valid_declaration_type(type_T *type)
{
    אם (type->tt_type == VAR_SPECIAL  // null, none
	    || type->tt_type == VAR_VOID)
    {
	حرف *tofree = عدم;
	حرف *name = type_name(type, &tofree);
	semsg(_(e_invalid_type_in_variable_declaration_str), name);
	vim_free(tofree);
	החזר FALSE;
    }
    החזר TRUE;
}

/*
 * Get a type_T for a typval_T, used for v: variables.
 * "type_list" is used to temporarily create types in.
 */
    type_T *
typval2type_vimvar(typval_T *tv, garray_T *type_gap)
{
    אם (tv->v_type == VAR_LIST)  // e.g. for v:oldfiles
	החזר &t_list_string;
    אם (tv->v_type == VAR_DICT)  // e.g. for v:event
	החזר &t_dict_any;
    החזר typval2type(tv, get_copyID(), type_gap, TVTT_DO_MEMBER);
}

    صحيح
check_typval_arg_type(
	type_T	    *expected,
	typval_T    *actual_tv,
	حرف	    *func_name,
	صحيح	    arg_idx)
{
    where_T	where = WHERE_INIT;

    אם (arg_idx > 0)
    {
	where.wt_index = arg_idx;
	where.wt_kind = WT_ARGUMENT;
    }
    where.wt_func_name = func_name;
    החזר check_typval_type(expected, actual_tv, where);
}

/*
 * Return true if "expected" is list<any> or dict<any> and "actual_tv" is a
 * list or dict of that kind, which matches whatever its values are.
 */
    ساكن bool
matches_listdict_any(type_T *expected, typval_T *actual_tv)
{
    החזר (expected->tt_type == VAR_LIST || expected->tt_type == VAR_DICT)
	    && actual_tv->v_type == expected->tt_type
	    && expected->tt_member != عدم
	    && (expected->tt_member->tt_type == VAR_ANY
		|| expected->tt_member->tt_type == VAR_UNKNOWN);
}

/*
 * Return FAIL if "expected" and "actual" don't match.
 * When "argidx" > 0 it is included in the error message.
 */
    صحيح
check_typval_type(type_T *expected, typval_T *actual_tv, where_T where)
{
    garray_T	type_list;
    type_T	*actual_type;
    صحيح		res = FAIL;

    אם (expected == عدم)
	החזר OK;  // didn't expect anything.

    // No need to go through the values of a possibly big list or dict.
    אם (matches_listdict_any(expected, actual_tv))
	החזר OK;

    ga_init2(&type_list, حجم(type_T *), 10);

    // A null_function and null_partial are special cases, they can be used to
    // clear a variable.
    אם ((actual_tv->v_type == VAR_FUNC && actual_tv->vval.v_string == عدم)
	    || (actual_tv->v_type == VAR_PARTIAL
					 && actual_tv->vval.v_partial == عدم))
	actual_type = &t_func_unknown;
    אחרת
	// When the actual type is list<any> or dict<any> go through the values
	// to possibly get a more specific type.
	actual_type = typval2type(actual_tv, get_copyID(), &type_list,
					  TVTT_DO_MEMBER | TVTT_MORE_SPECIFIC);
    אם (actual_type != عدم)
    {
	res = check_type_maybe(expected, actual_type, TRUE, where);
	אם (res == MAYBE && !(actual_type->tt_type == VAR_FUNC
				      && (actual_type->tt_member == &t_unknown
					  || actual_type->tt_member == عدم)))
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
    החזר res;
}

    فراغ
arg_type_mismatch(type_T *expected, type_T *actual, صحيح arg_idx)
{
    where_T	where = WHERE_INIT;

    אם (arg_idx > 0)
    {
	where.wt_index = arg_idx;
	where.wt_kind = WT_ARGUMENT;
    }
    type_mismatch_where(expected, actual, where);
}

    فراغ
type_mismatch_where(type_T *expected, type_T *actual, where_T where)
{
    حرف *tofree1, *tofree2;
    حرف *typename1 = type_name(expected, &tofree1);
    حرف *typename2 = type_name(actual, &tofree2);

    بدل (where.wt_kind)
    {
	حالة WT_MEMBER:
	    semsg(_(e_variable_str_type_mismatch_expected_str_but_got_str),
		    where.wt_func_name, typename1, typename2);
	    اكسر;
	حالة WT_METHOD:
	حالة WT_METHOD_ARG:
	حالة WT_METHOD_RETURN:
	    semsg(_(e_method_str_type_mismatch_expected_str_but_got_str),
		    where.wt_func_name, typename1, typename2);
	    اكسر;
	حالة WT_VARIABLE:
	    אם (where.wt_func_name == عدم)
		semsg(_(e_variable_nr_type_mismatch_expected_str_but_got_str),
			where.wt_index, typename1, typename2);
	    אחרת
		semsg(_(e_variable_nr_type_mismatch_expected_str_but_got_str_in_str),
			where.wt_index, typename1, typename2, where.wt_func_name);
	    اكسر;
	حالة WT_ARGUMENT:
	    אם (where.wt_func_name == عدم)
		semsg(_(e_argument_nr_type_mismatch_expected_str_but_got_str),
			where.wt_index, typename1, typename2);
	    אחרת
		semsg(_(e_argument_nr_type_mismatch_expected_str_but_got_str_in_str),
			where.wt_index, typename1, typename2, where.wt_func_name);
	    اكسر;
	حالة WT_CAST:
	حالة WT_UNKNOWN:
	    אם (where.wt_func_name == عدم)
		semsg(_(e_type_mismatch_expected_str_but_got_str),
			typename1, typename2);
	    אחרת
		semsg(_(e_type_mismatch_expected_str_but_got_str_in_str),
			typename1, typename2, where.wt_func_name);
	    اكسر;
    }

    vim_free(tofree1);
    vim_free(tofree2);
}

/*
 * Check if the expected and actual types match for a tuple
 */
    ساكن صحيح
check_tuple_type_maybe(
    type_T	*expected,
    type_T	*actual,
    where_T	where)
{
    אם (expected->tt_argcount == -1 || actual->tt_argcount == -1
		|| expected->tt_args == عدم || actual->tt_args == عدم)
	החזר OK;

    // For a non-variadic tuple, the number of items must match
    אם (!(expected->tt_flags & TTFLAG_VARARGS)
	    && expected->tt_argcount != actual->tt_argcount)
	החזר FAIL;

    // compare the type of each tuple item
    עבור (صحيح i = 0; i < actual->tt_argcount; ++i)
    {
	type_T	*exp_type;
	type_T	*actual_type;

	אם (expected->tt_flags & TTFLAG_VARARGS)
	{
	    אם (i < expected->tt_argcount - 1)
		exp_type = expected->tt_args[i];
	    אחרת
		// For a variadic tuple, the last type is a List.  So use the
		// List member type.
		exp_type = expected->tt_args[expected->tt_argcount - 1]->tt_member;
	}
	אחרת
	    exp_type = expected->tt_args[i];

	אם (actual->tt_flags & TTFLAG_VARARGS)
	{
	    אם (i < actual->tt_argcount - 1)
		actual_type = actual->tt_args[i];
	    אחרת
		// For a variadic tuple, the last type is a List.  So use the
		// List member type.
		actual_type = actual->tt_args[actual->tt_argcount - 1]->tt_member;
	}
	אחרת
	    actual_type = actual->tt_args[i];

	// Allow for using "any" type for a tuple item
	אם (actual->tt_args[i] != &t_any && check_type(exp_type, actual_type,
						FALSE, where) == FAIL)
	    החזר FAIL;
    }

    החזר OK;
}

/*
 * Check if the expected and actual types match for a function
 * Returns OK if "expected" and "actual" are matching function types.
 * Returns FAIL if "expected" and "actual" are different types.
 * Returns MAYBE when a runtime type check is needed.
 */
    ساكن صحيح
check_func_type_maybe(
    type_T	*expected,
    type_T	*actual,
    where_T	where)
{
    صحيح ret = OK;

    // If the return type is unknown it can be anything, including
    // nothing, thus there is no point in checking.
    אם (expected->tt_member != &t_unknown)
    {
	אם (actual->tt_member != عدم
		&& actual->tt_member != &t_unknown)
	{
	    where_T  func_where = where;

	    func_where.wt_kind = WT_METHOD_RETURN;
	    ret = check_type_maybe(expected->tt_member,
		    actual->tt_member, FALSE,
		    func_where);
	}
	אחרת
	    ret = MAYBE;
    }
    אם (ret != FAIL
	    && ((expected->tt_flags & TTFLAG_VARARGS)
		!= (actual->tt_flags & TTFLAG_VARARGS))
	    && expected->tt_argcount != -1)
	ret = FAIL;
    אם (ret != FAIL && expected->tt_argcount != -1
	    && actual->tt_min_argcount != -1
	    && (actual->tt_argcount == -1
		|| (actual->tt_argcount < expected->tt_min_argcount
		    || actual->tt_argcount > expected->tt_argcount)))
	ret = FAIL;
    אם (ret != FAIL && expected->tt_args != عدم
	    && actual->tt_args != عدم)
    {
	صحيح i;

	עבור (i = 0; i < expected->tt_argcount
		&& i < actual->tt_argcount; ++i)
	{
	    where_T  func_where = where;
	    func_where.wt_kind = WT_METHOD_ARG;

	    // Allow for using "any" argument type, lambda's have them.
	    אם (actual->tt_args[i] != &t_any && check_type(
			expected->tt_args[i], actual->tt_args[i], FALSE,
			func_where) == FAIL)
	    {
		ret = FAIL;
		اكسر;
	    }
	}
    }
    אם (ret == OK && expected->tt_argcount >= 0
	    && actual->tt_argcount == -1)
	// check the argument count at runtime
	ret = MAYBE;

    החזר ret;
}

/*
 * Check if the expected and actual types match for an object
 * Returns OK if "expected" and "actual" are matching object types.
 * Returns FAIL if "expected" and "actual" are different types.
 * Returns MAYBE when a runtime type check is needed.
 */
    ساكن صحيح
check_object_type_maybe(
    type_T	*expected,
    type_T	*actual,
    where_T	where)
{
    صحيح ret = OK;

    אם (actual->tt_type == VAR_ANY)
	החזר MAYBE;	// use runtime type check
    אם (actual->tt_type != VAR_OBJECT)
	החזר FAIL;	// don't use tt_class
    אם (actual->tt_class == عدم)    // null object
	החזר OK;
    // t_object_any matches any object except for an enum item
    אם (expected == &t_object_any && !IS_ENUM(actual->tt_class))
	החזר OK;

    // For object method arguments, do a invariant type check in
    // an extended class.  For all others, do a covariance type check.
    אם (where.wt_kind == WT_METHOD_ARG)
    {
	אם (actual->tt_class != expected->tt_class)
	    ret = FAIL;
    }
    אחרת אם (!class_instance_of(actual->tt_class, expected->tt_class))
    {
	// Check if this is an up-cast, if so we'll have to check the type at
	// runtime.
	אם (where.wt_kind == WT_CAST &&
		class_instance_of(expected->tt_class, actual->tt_class))
	    ret = MAYBE;
	אחרת
	    ret = FAIL;
    }

    החזר ret;
}

/*
 * Check if the expected and actual types match.
 * Does not allow for assigning "any" to a specific type.
 * When "argidx" > 0 it is included in the error message.
 * Return OK if types match.
 * Return FAIL if types do not match.
 */
    صحيح
check_type(
	type_T	*expected,
	type_T	*actual,
	صحيح	give_msg,
	where_T where)
{
    صحيح ret = check_type_maybe(expected, actual, give_msg, where);

    החזר ret == MAYBE ? OK : ret;
}

/*
 * As check_type() but return MAYBE when a runtime type check should be used
 * when compiling.
 */
    صحيح
check_type_maybe(
	type_T	*expected,
	type_T	*actual,
	صحيح	give_msg,
	where_T where)
{
    صحيح ret = OK;

    // When expected is "unknown" we accept any actual type.
    // When expected is "any" we accept any actual type except "void".
    אם (expected->tt_type != VAR_UNKNOWN
	    && !(expected->tt_type == VAR_ANY && actual->tt_type != VAR_VOID))

    {
	// tt_type should match, except that a "partial" can be assigned to a
	// variable with type "func".
	// And "unknown" (using global variable) and "any" need a runtime type
	// check.
	אם (!(expected->tt_type == actual->tt_type
		    || actual->tt_type == VAR_UNKNOWN
		    || actual->tt_type == VAR_ANY
		    || (expected->tt_type == VAR_FUNC
					   && actual->tt_type == VAR_PARTIAL)))
	{
	    אם (expected->tt_type == VAR_BOOL
					&& (actual->tt_flags & TTFLAG_BOOL_OK))
		// Using number 0 or 1 for bool is OK.
		החזר OK;
	    אם (expected->tt_type == VAR_FLOAT
		    && actual->tt_type == VAR_NUMBER
		    && ((expected->tt_flags & TTFLAG_NUMBER_OK)
			     || (actual->tt_flags & TTFLAG_FLOAT_OK)))
		// Using a number where a float is expected is OK here.
		החזר OK;
	    אם (expected->tt_type == VAR_LIST
		    && actual->tt_type == VAR_TUPLE
		    && (expected->tt_flags & TTFLAG_TUPLE_OK))
		// Using a tuple where a list is expected is OK here.
		החזר OK;
	    אם (give_msg)
		type_mismatch_where(expected, actual, where);
	    החזר FAIL;
	}
	אם (expected->tt_type == VAR_DICT || expected->tt_type == VAR_LIST)
	{
	    // "unknown" is used for an empty list or dict
	    אם (actual->tt_member != عدم && actual->tt_member != &t_unknown)
		ret = check_type_maybe(expected->tt_member, actual->tt_member,
								 FALSE, where);
	}
	אחרת אם (expected->tt_type == VAR_TUPLE && actual != &t_any)
	    ret =  check_tuple_type_maybe(expected, actual, where);
	אחרת אם (expected->tt_type == VAR_FUNC && actual != &t_any)
	    ret = check_func_type_maybe(expected, actual, where);
	אחרת אם (expected->tt_type == VAR_OBJECT)
	    ret = check_object_type_maybe(expected, actual, where);

	אם (ret == FAIL && give_msg)
	    type_mismatch_where(expected, actual, where);
    }

    אם (ret == OK && expected->tt_type != VAR_UNKNOWN
	    && expected->tt_type != VAR_ANY
	    && (actual->tt_type == VAR_UNKNOWN || actual->tt_type == VAR_ANY))
	// check the type at runtime
	ret = MAYBE;

    החזר ret;
}

/*
 * Check that the arguments of "type" match "argvars[argcount]".
 * "base_tv" is from "expr->Func()".
 * Return OK/FAIL.
 */
    صحيح
check_argument_types(
	type_T	    *type,
	typval_T    *argvars,
	صحيح	    argcount,
	typval_T    *base_tv,
	char_u	    *name)
{
    صحيح	    varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;
    صحيح	    i;
    صحيح	    totcount = argcount + (base_tv == عدم ? 0 : 1);

    אם (type->tt_type != VAR_FUNC && type->tt_type != VAR_PARTIAL)
	החזר OK;  // just in case
    אם (totcount < type->tt_min_argcount - varargs)
    {
	emsg_funcname(e_not_enough_arguments_for_function_str, name);
	החזר FAIL;
    }
    אם (!varargs && type->tt_argcount >= 0 && totcount > type->tt_argcount)
    {
	emsg_funcname(e_too_many_arguments_for_function_str, name);
	החזר FAIL;
    }
    אם (type->tt_args == عدم)
	החזר OK;  // cannot check


    עבור (i = 0; i < totcount; ++i)
    {
	type_T	    *expected;
	typval_T    *tv;

	אם (base_tv != عدم)
	{
	    אם (i == 0)
		tv = base_tv;
	    אחרת
		tv = &argvars[i - 1];
	}
	אחרת
	    tv = &argvars[i];
	אם (varargs && i >= type->tt_argcount - 1)
	{
	    expected = type->tt_args[type->tt_argcount - 1];
	    אם (expected != عدم && expected->tt_type == VAR_LIST)
		expected = expected->tt_member;
	    אם (expected == عدم)
		expected = &t_any;
	}
	אחרת
	    expected = type->tt_args[i];

	// check the type, unless the value is v:none
	אם ((tv->v_type != VAR_SPECIAL || tv->vval.v_number != VVAL_NONE)
		   && check_typval_arg_type(expected, tv, عدم, i + 1) == FAIL)
	    החזר FAIL;
    }
    החזר OK;
}

// Maximum nesting of types, to avoid running out of stack space.  E.g.
// "list<list<number>>" is nested twice.
#define MAX_TYPE_DEPTH	1000

// Current number of levels of parse_type() and the deepest level reached while
// parsing the current type.  The nesting is one less than the number of
// levels.
ساكن صحيح	parse_depth = 0;
ساكن صحيح	parse_max_depth = 0;

/*
 * Skip over type in list<type>, dict<type> or tuple<type>.
 * Returns a pointer to the character after the type.  "syn_error" is set to
 * TRUE on syntax error.
 */
    ساكن char_u *
skip_member_type(char_u *start, char_u *p, صحيح *syn_error)
{
    אם (STRNCMP("tuple", start, 5) == 0)
    {
	// handle tuple<{type1}, {type2}, ....<type>>
	p = skipwhite(p + 1);
	כלעוד (*p != '>' && *p != NUL)
	{
	    char_u *sp = p;

	    אם (STRNCMP(p, "...", 3) == 0)
		p += 3;
	    p = skip_type(p, TRUE);
	    אם (p == sp)
	    {
		*syn_error = TRUE;
		החזר p;  // syntax error
	    }
	    אם (*p == ',')
		p = skipwhite(p + 1);
	}
	אם (*p == '>')
	    p++;
    }
    אחרת
    {
	p = skipwhite(p);
	p = skip_type(skipwhite(p + 1), FALSE);
	p = skipwhite(p);
	אם (*p == '>')
	    ++p;
    }

    החזר p;
}

/*
 * Skip over a function type.  Returns a pointer to the character after the
 * type.  "syn_error" is set to TRUE on syntax error.
 */
    ساكن char_u *
skip_func_type(char_u *p, صحيح *syn_error)
{
    אם (*p == '(')
    {
	// handle func(args): type
	++p;
	כלעוד (*p != ')' && *p != NUL)
	{
	    char_u *sp = p;

	    אם (STRNCMP(p, "...", 3) == 0)
		p += 3;
	    p = skip_type(p, TRUE);
	    אם (p == sp)
	    {
		*syn_error = TRUE;
		החזר p;  // syntax error
	    }
	    אם (*p == ',')
		p = skipwhite(p + 1);
	}
	אם (*p == ')')
	{
	    אם (p[1] == ':')
		p = skip_type(skipwhite(p + 2), FALSE);
	    אחרת
		++p;
	}
    }
    אחרת
    {
	// handle func: return_type
	p = skip_type(skipwhite(p + 1), FALSE);
    }

    החזר p;
}

/*
 * Inner part of skip_type().
 */
    ساكن char_u *
skip_type_inner(char_u *start, صحيح optional)
{
    char_u	*p = start;
    صحيح		syn_error = FALSE;

    אם (optional && *p == '?')
	++p;

    // Also skip over "." for imported classes: "import.ClassName".
    כלעוד (ASCII_ISALNUM(*p) || *p == '_' || *p == '.')
	++p;

    // Skip over "<type>"; this is permissive about white space.
    אם (*skipwhite(p) == '<')
    {
	p = skip_member_type(start, p, &syn_error);
	אם (syn_error)
	    החזר p;
    }
    אחרת אם ((*p == '(' || (*p == ':' && VIM_ISWHITE(p[1])))
					     && STRNCMP("func", start, 4) == 0)
    {
	// skip over function type
	p = skip_func_type(p, &syn_error);
	אם (syn_error)
	    החזר p;
    }

    החזר p;
}

/*
 * Skip over a type definition and return a pointer to just after it.
 * When "optional" is TRUE then a leading "?" is accepted.
 */
    char_u *
skip_type(char_u *start, صحيح optional)
{
    ساكن صحيح	skip_depth = 0;
    char_u	*p = start;

    // Too deeply nested: skip to the matching ">" without recursing, an
    // error is given when parsing the type.
    אם (skip_depth > MAX_TYPE_DEPTH)
    {
	صحيح	depth = 0;

	עבור ( ; *p != NUL; ++p)
	{
	    אם (*p == '<')
		++depth;
	    אחרת אם (*p == '>')
	    {
		אם (depth == 0)
		    اكسر;
		אם (--depth == 0)
		    החזר p + 1;
	    }
	    אחרת אם (depth == 0 && !ASCII_ISALNUM(*p) && *p != '_'
								&& *p != '.')
		اكسر;
	}
	החזר p;
    }

    ++skip_depth;
    p = skip_type_inner(start, optional);
    --skip_depth;
    החזר p;
}

/*
 * Parse the member type: "<type>" and return "type" with the member set.
 * Use "type_gap" if a new type needs to be added.
 * "info" is extra information for an error message.
 * Returns NULL in case of failure.
 */
    ساكن type_T *
parse_type_member(
	char_u	    **arg,
	type_T	    *type,
	garray_T    *type_gap,
	صحيح	    give_error,
	حرف	    *info,
	ufunc_T	    *ufunc,
	cctx_T	    *cctx)
{
    char_u  *arg_start = *arg;
    type_T  *member_type;
    صحيح	    prev_called_emsg = called_emsg;

    אם (**arg != '<')
    {
	אם (give_error)
	{
	    אם (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    אחרת
		semsg(_(e_missing_type_after_str), info);
	}
	החזר عدم;
    }
    *arg = skipwhite(*arg + 1);

    member_type = parse_type(arg, type_gap, ufunc, cctx, give_error);
    אם (member_type == عدم || !valid_declaration_type(member_type))
	החזר عدم;

    *arg = skipwhite(*arg);
    אם (**arg != '>' && called_emsg == prev_called_emsg)
    {
	אם (give_error)
	    semsg(_(e_missing_gt_after_type_str), arg_start);
	החזר عدم;
    }
    ++*arg;

    אם (type->tt_type == VAR_LIST)
	החזר get_list_type(member_type, type_gap);
    החזר get_dict_type(member_type, type_gap);
}

/*
 * Parse a "func" type at "*arg" and advance over it.
 * When "give_error" is TRUE give error messages, otherwise be quiet.
 * Return NULL for failure.
 */
    ساكن type_T *
parse_type_func(
    char_u	**arg,
    size_t	len,
    garray_T	*type_gap,
    صحيح		give_error,
    ufunc_T	*ufunc,
    cctx_T	*cctx)
{
    char_u  *p;
    type_T  *type;
    type_T  *ret_type = &t_unknown;
    صحيح	argcount = -1;
    صحيح	flags = 0;
    صحيح	first_optional = -1;
    type_T	*arg_type[MAX_FUNC_ARGS + 1];

    // func({type}, ...{type}): {type}
    *arg += len;
    אם (**arg == '(')
    {
	// "func" may or may not return a value, "func()" does
	// not return a value.
	ret_type = &t_void;

	p = ++*arg;
	argcount = 0;
	כלעוד (*p != NUL && *p != ')')
	{
	    אם (*p == '?')
	    {
		אם (first_optional == -1)
		    first_optional = argcount;
		++p;
	    }
	    אחרת אם (STRNCMP(p, "...", 3) == 0)
	    {
		flags |= TTFLAG_VARARGS;
		p += 3;
	    }
	    אחרת אם (first_optional != -1)
	    {
		אם (give_error)
		    emsg(_(e_mandatory_argument_after_optional_argument));
		החזר عدم;
	    }

	    type = parse_type(&p, type_gap, ufunc, cctx, give_error);
	    אם (type == عدم || !valid_declaration_type(type))
		החזר عدم;
	    אם ((flags & TTFLAG_VARARGS) != 0 && type->tt_type != VAR_LIST)
	    {
		حرف *tofree;
		semsg(_(e_variable_arguments_type_must_be_list_str),
			type_name(type, &tofree));
		vim_free(tofree);
		החזר عدم;
	    }
	    arg_type[argcount++] = type;

	    // Nothing comes after "...{type}".
	    אם (flags & TTFLAG_VARARGS)
		اكسر;

	    אם (*p != ',' && *skipwhite(p) == ',')
	    {
		אם (give_error)
		    semsg(_(e_no_white_space_allowed_before_str_str),
			    ",", p);
		החזר عدم;
	    }
	    אם (*p == ',')
	    {
		++p;
		אם (!VIM_ISWHITE(*p))
		{
		    אם (give_error)
			semsg(_(e_white_space_required_after_str_str),
				",", p - 1);
		    החזר عدم;
		}
	    }
	    p = skipwhite(p);
	    אם (argcount == MAX_FUNC_ARGS)
	    {
		אם (give_error)
		    emsg(_(e_too_many_argument_types));
		החזר عدم;
	    }
	}

	p = skipwhite(p);
	אם (*p != ')')
	{
	    אם (give_error)
		emsg(_(e_missing_closing_paren));
	    החזר عدم;
	}
	*arg = p + 1;
    }
    אם (**arg == ':')
    {
	// parse return type
	++*arg;
	אם (!VIM_ISWHITE(**arg) && give_error)
	    semsg(_(e_white_space_required_after_str_str), ":", *arg - 1);
	*arg = skipwhite(*arg);
	ret_type = parse_type(arg, type_gap, ufunc, cctx, give_error);
	אם (ret_type == عدم)
	    החזר عدم;
    }
    אם (flags == 0 && first_optional == -1 && argcount <= 0)
	type = get_func_type(ret_type, argcount, type_gap);
    אחרת
    {
	type = alloc_func_type(ret_type, argcount, type_gap);
	type->tt_flags = flags;
	אם (argcount > 0)
	{
	    type->tt_argcount = argcount;
	    type->tt_min_argcount = first_optional == -1
						? argcount : first_optional;
	    אם (func_type_add_arg_types(type, argcount, type_gap) == FAIL)
		החזר عدم;
	    mch_memmove(type->tt_args, arg_type, حجم(type_T *) * argcount);
	}
    }

    החזר type;
}

/*
 * Parse a "tuple" type at "*arg" and advance over it.
 * When "give_error" is TRUE give error messages, otherwise be quiet.
 * Return NULL for failure.
 */
    ساكن type_T *
parse_type_tuple(
    char_u	**arg,
    garray_T	*type_gap,
    صحيح		give_error,
    ufunc_T	*ufunc,
    cctx_T	*cctx)
{
    char_u	*p;
    type_T	*type;
    type_T	*ret_type = عدم;
    صحيح		typecount = -1;
    صحيح		flags = 0;
    garray_T	tuple_types_ga;

    ga_init2(&tuple_types_ga, حجم(type_T *), 10);

    // tuple<{type}, {type}>
    // tuple<{type}, ...{type}>
    אם (**arg != '<')
    {
	אם (give_error)
	{
	    אם (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    אחרת
		semsg(_(e_missing_type_after_str), "tuple");
	}

	// only "tuple" is specified
	החזר عدم;
    }

    p = ++*arg;
    typecount = 0;
    כלעוד (*p != NUL && *p != '>')
    {
	אם (STRNCMP(p, "...", 3) == 0)
	{
	    flags |= TTFLAG_VARARGS;
	    p += 3;
	}

	type = parse_type(&p, type_gap, ufunc, cctx, give_error);
	אם (type == عدم || !valid_declaration_type(type))
	    اذهب on_err;

	אם ((flags & TTFLAG_VARARGS) != 0 && type->tt_type != VAR_LIST)
	{
	    حرف *tofree;
	    semsg(_(e_variadic_tuple_must_end_with_list_type_str),
						type_name(type, &tofree));
	    vim_free(tofree);
	    اذهب on_err;
	}

	// Add the item type
	אם (ga_grow(&tuple_types_ga, 1) == FAIL)
	    اذهب on_err;
	((type_T **)tuple_types_ga.ga_data)[tuple_types_ga.ga_len] = type;
	tuple_types_ga.ga_len++;
	typecount++;

	// Nothing comes after "...{type}".
	אם (flags & TTFLAG_VARARGS)
	    اكسر;

	אם (*p != ',' && *skipwhite(p) == ',')
	{
	    אם (give_error)
		semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
	    اذهب on_err;
	}
	אם (*p == ',')
	{
	    ++p;
	    אם (!VIM_ISWHITE(*p))
	    {
		אם (give_error)
		    semsg(_(e_white_space_required_after_str_str),
			    ",", p - 1);
		اذهب on_err;
	    }
	}
	p = skipwhite(p);
    }

    p = skipwhite(p);
    אם (*p != '>' || typecount <= 0)
    {
	אם (give_error)
	    semsg(_(e_missing_type_after_str), p);
	اذهب on_err;
    }
    *arg = p + 1;

    ret_type = alloc_tuple_type(typecount, type_gap);
    אם (ret_type == &t_any
	    || tuple_type_add_types(ret_type, typecount, type_gap) == FAIL)
    {
	ret_type = عدم;
	اذهب on_err;
    }
    ret_type->tt_flags = flags;
    ret_type->tt_argcount = typecount;
    mch_memmove(ret_type->tt_args, tuple_types_ga.ga_data,
						حجم(type_T *) * typecount);

on_err:
    ga_clear(&tuple_types_ga);

    החזר ret_type;
}

/*
 * Parse a "object" type at "*arg" and advance over it.
 * When "give_error" is TRUE give error messages, otherwise be quiet.
 * Return NULL for failure.
 */
    ساكن type_T *
parse_type_object(
    char_u	**arg,
    garray_T	*type_gap,
    صحيح		give_error,
    cctx_T	*cctx)
{
    char_u	*arg_start = *arg;
    type_T	*object_type;
    صحيح		prev_called_emsg = called_emsg;

    // object<X> or object<any>
    אם (**arg != '<')
    {
	אם (give_error)
	{
	    אם (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    אחרת
		semsg(_(e_missing_type_after_str), "object");
	}

	// only "object" is specified
	החזר عدم;
    }

    // skip spaces following "object<"
    *arg = skipwhite(*arg + 1);

    object_type = parse_type(arg, type_gap, عدم, cctx, give_error);
    אם (object_type == عدم)
	החזר عدم;

    *arg = skipwhite(*arg);
    אם (**arg != '>' && called_emsg == prev_called_emsg)
    {
	אם (give_error)
	    semsg(_(e_missing_gt_after_type_str), arg_start);
	החזר عدم;
    }
    ++*arg;

    אם (object_type->tt_type == VAR_ANY)
	החזר &t_object_any;

    אם (object_type->tt_type != VAR_OBJECT)
    {
	// specified type is not a class
	אם (give_error)
	    semsg(_(e_class_name_not_found_str), arg_start);
	החזר عدم;
    }

    החזר object_type;
}

/*
 * Parse a user defined type at "*arg" and advance over it.
 * It can be a class or an interface or a typealias name, possibly imported.
 * Return NULL if a type is not found.
 */
    ساكن type_T *
parse_type_user_defined(
    char_u	**arg,
    size_t	len,
    garray_T	*type_gap,
    صحيح		give_error,
    ufunc_T	*ufunc,
    cctx_T	*cctx)
{
    صحيح		did_emsg_before = did_emsg;
    typval_T	tv;

    tv.v_type = VAR_UNKNOWN;
    אם (eval_variable_import(*arg, &tv) == OK)
    {
	אם (tv.v_type == VAR_CLASS && tv.vval.v_class != عدم)
	{
	    type_T *type = get_type_ptr(type_gap);
	    אם (type != عدم)
	    {
		// Although the name is that of a class or interface, the type
		// uses will be an object.
		type->tt_type = VAR_OBJECT;
		type->tt_class = tv.vval.v_class;
		clear_tv(&tv);

		*arg += len;
		// Skip over ".ClassName".
		כלעוד (ASCII_ISALNUM(**arg) || **arg == '_' || **arg == '.')
		    ++*arg;

		החזר type;
	    }
	}
	אחרת אם (tv.v_type == VAR_TYPEALIAS)
	{
	    // user defined type
	    // The type alias may itself be a nested type, check the resulting
	    // nesting depth.
	    صحيح	depth = parse_depth - 1
				+ MAX(tv.vval.v_typealias->ta_depth, 1);
	    אם (depth - 1 > MAX_TYPE_DEPTH)
	    {
		clear_tv(&tv);
		אם (give_error)
		    emsg(_(e_type_nested_too_deep));
		החזר عدم;
	    }
	    אם (depth > parse_max_depth)
		parse_max_depth = depth;

	    type_T *type = copy_type(tv.vval.v_typealias->ta_type, type_gap);
	    *arg += len;
	    clear_tv(&tv);
	    // Skip over ".TypeName".
	    כלעוד (ASCII_ISALNUM(**arg) || **arg == '_' || **arg == '.')
		++*arg;
	    החזר type;
	}

	clear_tv(&tv);
    }

    // Check whether it is a generic type
    type_T *type = find_generic_type(*arg, len, ufunc, cctx);
    אם (type != عدم)
    {
	*arg += len;
	החזר type;
    }

    אם (give_error && (did_emsg == did_emsg_before))
    {
	char_u	*p = skip_type(*arg, FALSE);
	حرف	cc = *p;

	*p = NUL;
	semsg(_(e_type_not_recognized_str), *arg);
	*p = cc;
    }

    החזר عدم;
}

/*
 * Inner part of parse_type().
 */
    ساكن type_T *
parse_type_inner(
    char_u	**arg,
    garray_T	*type_gap,
    ufunc_T	*ufunc,
    cctx_T	*cctx,
    صحيح		give_error)
{
    char_u  *p = *arg;
    size_t  len;

    // Skip over the first word.
    כלעוד (ASCII_ISALNUM(*p) || *p == '_')
	++p;
    len = p - *arg;

    بدل (**arg)
    {
	حالة 'a':
	    אם (len == 3 && STRNCMP(*arg, "any", len) == 0)
	    {
		*arg += len;
		החזר &t_any;
	    }
	    اكسر;
	حالة 'b':
	    אם (len == 4 && STRNCMP(*arg, "bool", len) == 0)
	    {
		*arg += len;
		החזר &t_bool;
	    }
	    אם (len == 4 && STRNCMP(*arg, "blob", len) == 0)
	    {
		*arg += len;
		החזר &t_blob;
	    }
	    اكسر;
	حالة 'c':
	    אם (len == 7 && STRNCMP(*arg, "channel", len) == 0)
	    {
		*arg += len;
		החזר &t_channel;
	    }
	    اكسر;
	حالة 'd':
	    אם (len == 4 && STRNCMP(*arg, "dict", len) == 0)
	    {
		*arg += len;
		החזר parse_type_member(arg, &t_dict_any, type_gap,
						give_error, "dict", ufunc,
						cctx);
	    }
	    اكسر;
	حالة 'f':
	    אם (len == 5 && STRNCMP(*arg, "float", len) == 0)
	    {
		*arg += len;
		החזר &t_float;
	    }
	    אם (len == 4 && STRNCMP(*arg, "func", len) == 0)
		החזר parse_type_func(arg, len, type_gap, give_error, ufunc,
									cctx);
	    اكسر;
	حالة 'j':
	    אם (len == 3 && STRNCMP(*arg, "job", len) == 0)
	    {
		*arg += len;
		החזר &t_job;
	    }
	    اكسر;
	حالة 'l':
	    אם (len == 4 && STRNCMP(*arg, "list", len) == 0)
	    {
		*arg += len;
		החזר parse_type_member(arg, &t_list_any, type_gap,
						give_error, "list", ufunc,
						cctx);
	    }
	    اكسر;
	حالة 'n':
	    אם (len == 6 && STRNCMP(*arg, "number", len) == 0)
	    {
		*arg += len;
		החזר &t_number;
	    }
	    اكسر;
	حالة 'o':
	    אם (len == 6 && STRNCMP(*arg, "object", len) == 0)
	    {
		*arg += len;
		החזר parse_type_object(arg, type_gap, give_error, cctx);
	    }
	    اكسر;
	حالة 's':
	    אם (len == 6 && STRNCMP(*arg, "string", len) == 0)
	    {
		*arg += len;
		החזר &t_string;
	    }
	    اكسر;
	حالة 't':
	    אם (len == 5 && STRNCMP(*arg, "tuple", len) == 0)
	    {
		*arg += len;
		החזר parse_type_tuple(arg, type_gap, give_error, ufunc,
									cctx);
	    }
	    اكسر;
	حالة 'v':
	    אם (len == 4 && STRNCMP(*arg, "void", len) == 0)
	    {
		*arg += len;
		החזר &t_void;
	    }
	    اكسر;
    }

    // User defined type
    החזר parse_type_user_defined(arg, len, type_gap, give_error, ufunc,
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
    صحيح		give_error)
{
    type_T	*type;

    אם (parse_depth > MAX_TYPE_DEPTH)
    {
	אם (give_error)
	    emsg(_(e_type_nested_too_deep));
	החזר عدم;
    }
    אם (parse_depth == 0)
	parse_max_depth = 0;
    ++parse_depth;
    אם (parse_depth > parse_max_depth)
	parse_max_depth = parse_depth;
    type = parse_type_inner(arg, type_gap, ufunc, cctx, give_error);
    --parse_depth;
    החזר type;
}

/*
 * Return the number of levels of the type last parsed by parse_type().
 */
    صحيح
parse_type_depth(فراغ)
{
    החזר parse_max_depth;
}

/*
 * Check if "type1" and "type2" are exactly the same.
 * "flags" can have ETYPE_ARG_UNKNOWN, which means that an unknown argument
 * type in "type1" is accepted.
 */
    صحيح
equal_type(type_T *type1, type_T *type2, صحيح flags)
{
    صحيح i;

    אם (type1 == عدم || type2 == عدم)
	החזר FALSE;
    אם (type1->tt_type != type2->tt_type)
	החזר FALSE;
    بدل (type1->tt_type)
    {
	حالة VAR_UNKNOWN:
	حالة VAR_ANY:
	حالة VAR_VOID:
	حالة VAR_SPECIAL:
	حالة VAR_BOOL:
	حالة VAR_NUMBER:
	حالة VAR_FLOAT:
	حالة VAR_STRING:
	حالة VAR_BLOB:
	حالة VAR_JOB:
	حالة VAR_CHANNEL:
	حالة VAR_INSTR:
	حالة VAR_TYPEALIAS:
	    اكسر;  // not composite is always OK
	حالة VAR_OBJECT:
	حالة VAR_CLASS:
	    // Objects are considered equal if they are from the same class
	    החזר type1->tt_class == type2->tt_class;
	حالة VAR_LIST:
	حالة VAR_DICT:
	    החזר equal_type(type1->tt_member, type2->tt_member, flags);
	حالة VAR_TUPLE:
	    אם (type1->tt_argcount != type2->tt_argcount)
		החזר FALSE;
	    אם (type1->tt_argcount < 0
			   || type1->tt_args == عدم || type2->tt_args == عدم)
		החזר TRUE;
	    עבור (i = 0; i < type1->tt_argcount; ++i)
		אם ((flags & ETYPE_ARG_UNKNOWN) == 0
			&& !equal_type(type1->tt_args[i], type2->tt_args[i],
									flags))
		    החזר FALSE;
	    החזר TRUE;
	حالة VAR_FUNC:
	حالة VAR_PARTIAL:
	    אם (!equal_type(type1->tt_member, type2->tt_member, flags)
		    || type1->tt_argcount != type2->tt_argcount)
		החזר FALSE;
	    אם (type1->tt_argcount < 0
			   || type1->tt_args == عدم || type2->tt_args == عدم)
		החזר TRUE;
	    עבור (i = 0; i < type1->tt_argcount; ++i)
		אם ((flags & ETYPE_ARG_UNKNOWN) == 0
			&& !equal_type(type1->tt_args[i], type2->tt_args[i],
									flags))
		    החזר FALSE;
	    החזר TRUE;
    }
    החזר TRUE;
}

/*
 * Find the common type of "type1" (VAR_FUNC) and "type2" (VAR_FUNC) and put it
 * in "dest".  "type2" and "dest" may be the same.
 */
    ساكن فراغ
common_type_var_func(
    type_T	*type1,
    type_T	*type2,
    type_T	**dest,
    garray_T	*type_gap)
{
    type_T *common;

    // When one of the types is t_func_unknown return the other one.
    // Useful if a list or dict item is null_func.
    אם (type1 == &t_func_unknown)
    {
	*dest = type2;
	החזר;
    }
    אם (type2 == &t_func_unknown)
    {
	*dest = type1;
	החזר;
    }

    common_type(type1->tt_member, type2->tt_member, &common, type_gap);
    אם (type1->tt_argcount == type2->tt_argcount
	    && type1->tt_argcount >= 0)
    {
	صحيح argcount = type1->tt_argcount;
	صحيح i;

	*dest = alloc_func_type(common, argcount, type_gap);
	אם (type1->tt_args != عدم && type2->tt_args != عدم)
	{
	    אם (func_type_add_arg_types(*dest, argcount,
			type_gap) == OK)
		עבור (i = 0; i < argcount; ++i)
		    common_type(type1->tt_args[i], type2->tt_args[i],
			    &(*dest)->tt_args[i], type_gap);
	}
    }
    אחרת
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
    فراغ
common_type(type_T *type1, type_T *type2, type_T **dest, garray_T *type_gap)
{
    אם (equal_type(type1, type2, 0))
    {
	*dest = type1;
	החזר;
    }

    // If either is VAR_UNKNOWN use the other type.  An empty list/dict has no
    // specific type.
    אם (type1 == عدم || type1->tt_type == VAR_UNKNOWN)
    {
	*dest = type2;
	החזר;
    }
    אם (type2 == عدم || type2->tt_type == VAR_UNKNOWN)
    {
	*dest = type1;
	החזר;
    }

    אם (type1->tt_type == type2->tt_type)
    {
	אם (type1->tt_type == VAR_LIST
		|| type1->tt_type == VAR_DICT)
	{
	    type_T *common;

	    common_type(type1->tt_member, type2->tt_member, &common, type_gap);
	    אם (type1->tt_type == VAR_LIST)
		*dest = get_list_type(common, type_gap);
	    אחרת
		*dest = get_dict_type(common, type_gap);
	    החזר;
	}
	אחרת אם (type1->tt_type == VAR_FUNC)
	{
	    common_type_var_func(type1, type2, dest, type_gap);
	    החזר;
	}
	אחרת אם (type1->tt_type == VAR_OBJECT)
	{
	    *dest = &t_object_any;
	    החזר;
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
    אם (type->tt_type == VAR_TUPLE)
    {
	אם (type->tt_argcount != 1)
	    החזר &t_any;

	אם (type->tt_flags & TTFLAG_VARARGS)
	    החזר type->tt_args[0]->tt_member;
	אחרת
	    החזר type->tt_args[0];
    }

    // For "any" the item type is not known.
    החזר type->tt_member == عدم ? &t_any : type->tt_member;
}

/*
 * Push an entry onto the type stack.  "type" used both for the current type
 * and the declared type.
 * Returns FAIL when out of memory.
 */
    صحيح
push_type_stack(cctx_T *cctx, type_T *type)
{
    החזר push_type_stack2(cctx, type, type);
}

/*
 * Push an entry onto the type stack.  "type" is the current type, "decl_type"
 * is the declared type.
 * Returns FAIL when out of memory.
 */
    صحيح
push_type_stack2(cctx_T *cctx, type_T *type, type_T *decl_type)
{
    garray_T	*stack = &cctx->ctx_type_stack;
    type2_T	*typep;

    אם (GA_GROW_FAILS(stack, 1))
	החזר FAIL;
    typep = ((type2_T *)stack->ga_data) + stack->ga_len;
    typep->type_curr = type;
    typep->type_decl = decl_type;
    ++stack->ga_len;
    החזר OK;
}

/*
 * Set the type of the top of the stack to "type".
 */
    فراغ
set_type_on_stack(cctx_T *cctx, type_T *type, صحيح offset)
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
get_type_on_stack(cctx_T *cctx, صحيح offset)
{
    garray_T	*stack = &cctx->ctx_type_stack;

    אם (offset + 1 > stack->ga_len)
	החזר &t_unknown;
    החזר (((type2_T *)stack->ga_data) + stack->ga_len - offset - 1)
								   ->type_curr;
}

/*
 * Get the declared type from the type stack.  If "offset" is zero the one at
 * the top,
 * if "offset" is one the type above that, etc.
 * Returns &t_unknown if there is no such stack entry.
 */
    type_T *
get_decl_type_on_stack(cctx_T *cctx, صحيح offset)
{
    garray_T	*stack = &cctx->ctx_type_stack;

    אם (offset + 1 > stack->ga_len)
	החזר &t_unknown;
    החזר (((type2_T *)stack->ga_data) + stack->ga_len - offset - 1)
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
	صحيح	    count,
	صحيح	    skip,
	cctx_T	    *cctx)
{
    garray_T	*stack = &cctx->ctx_type_stack;
    type2_T	*typep;
    garray_T    *type_gap = cctx->ctx_type_list;
    صحيح		i;
    type_T	*result;
    type_T	*type;

    // Use "unknown" for an empty list or dict.
    אם (count == 0)
	החזר &t_unknown;
    // Find the common type from following items.
    typep = ((type2_T *)stack->ga_data) + stack->ga_len;
    result = &t_unknown;
    עבור (i = 0; i < count; ++i)
    {
	type = (typep -((count - i) * skip) + skip - 1)->type_curr;
	אם (check_type_is_value(type) == FAIL)
	    החזר عدم;
	אם (result != &t_any)
	    common_type(type, result, &result, type_gap);
    }

    החזר result;
}

/*
 * Get the types of items in a tuple on the stack of "cctx".
 * Returns the number of types.  Returns -1 on failure.
 */
    صحيح
get_tuple_type_from_stack(
    صحيح		count,
    garray_T	*tuple_types_gap,
    cctx_T	*cctx)
{
    garray_T	*stack = &cctx->ctx_type_stack;
    type2_T	*typep;
    type_T	*type = عدم;

    // Use "unknown" for an empty tuple
    אם (count == 0)
	החזר 0;

    // Find the common type from following items.
    typep = ((type2_T *)stack->ga_data) + stack->ga_len;
    עבור (صحيح i = 0; i < count; i++)
    {
	type = (typep - (count - i))->type_curr;
	אם (check_type_is_value(type) == FAIL)
	    החזר -1;
	אם (ga_grow(tuple_types_gap, 1) == FAIL)
	    החזר -1;
	((type_T **)tuple_types_gap->ga_data)[tuple_types_gap->ga_len] = type;
	tuple_types_gap->ga_len++;
    }

    החזר tuple_types_gap->ga_len;
}

    حرف *
vartype_name(vartype_T type)
{
    بدل (type)
    {
	حالة VAR_UNKNOWN: اكسر;
	حالة VAR_ANY: החזר "any";
	حالة VAR_VOID: החזר "void";
	حالة VAR_SPECIAL: החזר "special";
	حالة VAR_BOOL: החזר "bool";
	حالة VAR_NUMBER: החזר "number";
	حالة VAR_FLOAT: החזר "float";
	حالة VAR_STRING: החזר "string";
	حالة VAR_BLOB: החזר "blob";
	حالة VAR_JOB: החזר "job";
	حالة VAR_CHANNEL: החזר "channel";
	حالة VAR_LIST: החזר "list";
	حالة VAR_TUPLE: החזר "tuple";
	حالة VAR_DICT: החזר "dict";
	حالة VAR_INSTR: החזר "instr";
	حالة VAR_CLASS: החזר "class";
	حالة VAR_OBJECT: החזר "object";
	حالة VAR_TYPEALIAS: החזר "typealias";

	حالة VAR_FUNC:
	حالة VAR_PARTIAL: החזר "func";
    }
    החזר "unknown";
}

/*
 * Return the type name of a List (list<type>) or Dict (dict<type>).
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    ساكن حرف *
type_name_list_or_dict(حرف *name, type_T *type, حرف **tofree)
{
    حرف *member_free;
    حرف *member_name;

    אם (type->tt_member->tt_type == VAR_UNKNOWN)
	member_name = type_name(&t_any, &member_free);
    אחרת
	member_name = type_name(type->tt_member, &member_free);

    size_t len = STRLEN(name) + STRLEN(member_name) + 3;
    *tofree = alloc(len);
    אם (*tofree == عدم)
    {
	vim_free(member_free);
	החזר name;
    }

    vim_snprintf(*tofree, len, "%s<%s>", name, member_name);
    vim_free(member_free);
    החזר *tofree;
}

/*
 * Return the type name of a tuple.
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    ساكن حرف *
type_name_tuple(type_T *type, حرف **tofree)
{
    garray_T    ga;
    صحيح		i;
    صحيح		varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;
    حرف	*arg_free = عدم;

    ga_init2(&ga, 1, 100);
    אם (ga_grow(&ga, 20) == FAIL)
	اذهب failed;
    STRCPY(ga.ga_data, "tuple<");
    ga.ga_len += 6;

    אם (type->tt_argcount <= 0)
	// empty tuple
	GA_CONCAT_LITERAL(&ga, "any");
    אחרת
    {
	אם (type->tt_args == عدم)
	    GA_CONCAT_LITERAL(&ga, "[unknown]");
	אחרת
	{
	    עבור (i = 0; i < type->tt_argcount; ++i)
	    {
		string_T    arg_type;

		arg_type.string = (char_u *)type_name(type->tt_args[i], &arg_free);
		אם (i > 0)
		{
		    STRCPY((حرف *)ga.ga_data + ga.ga_len, ", ");
		    ga.ga_len += 2;
		}
		arg_type.length = STRLEN(arg_type.string);
		אם (ga_grow(&ga, (صحيح)arg_type.length + 8) == FAIL)
		    اذهب failed;
		אם (varargs && i == type->tt_argcount - 1)
		    GA_CONCAT_LITERAL(&ga, "...");
		ga_concat_len(&ga, arg_type.string, arg_type.length);
		VIM_CLEAR(arg_free);
	    }
	}
    }

    STRCPY((حرف *)ga.ga_data + ga.ga_len, ">");
    *tofree = ga.ga_data;
    החזר ga.ga_data;

failed:
    vim_free(arg_free);
    ga_clear(&ga);
    החזר "[unknown]";
}

/*
 * Return the type name of a Class (class<name>) or Object (object<name>).
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    ساكن حرف *
type_name_class_or_obj(حرف *name, type_T *type, حرف **tofree)
{
    string_T	class_name;

    אם (type->tt_class != عدم)
    {
	class_name.string = type->tt_class->class_name.string;
	class_name.length = type->tt_class->class_name.length;
	אם (IS_ENUM(type->tt_class))
	    name = "enum";
    }
    אחרת
	STR_LITERAL_SET(class_name, "any");

    size_t len = STRLEN(name) + class_name.length + 3;
    *tofree = alloc(len);
    אם (*tofree == عدم)
	החזר name;

    vim_snprintf(*tofree, len, "%s<%s>", name, class_name.string);
    החזר *tofree;
}

/*
 * Return the type name of a function.
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    ساكن حرف *
type_name_func(type_T *type, حرف **tofree)
{
    garray_T    ga;
    صحيح		i;
    صحيح		varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;
    حرف	*arg_free = عدم;

    ga_init2(&ga, 1, 100);
    אם (ga_grow(&ga, 20) == FAIL)
	اذهب failed;
    STRCPY(ga.ga_data, "func(");
    ga.ga_len += 5;

    עבור (i = 0; i < type->tt_argcount; ++i)
    {
	string_T    arg_type;

	אם (type->tt_args == عدم)
	    STR_LITERAL_SET(arg_type, "[unknown]");
	אחרת
	{
	    arg_type.string = (char_u *)type_name(type->tt_args[i], &arg_free);
	    arg_type.length = STRLEN(arg_type.string);
	}
	אם (i > 0)
	{
	    STRCPY((حرف *)ga.ga_data + ga.ga_len, ", ");
	    ga.ga_len += 2;
	}
	אם (ga_grow(&ga, (صحيح)arg_type.length + 8) == FAIL)
	    اذهب failed;
	אם (varargs && i == type->tt_argcount - 1)
	    GA_CONCAT_LITERAL(&ga, "...");
	אחרת אם (i >= type->tt_min_argcount)
	    *((حرف *)ga.ga_data + ga.ga_len++) = '?';
	ga_concat_len(&ga, arg_type.string, arg_type.length);
	VIM_CLEAR(arg_free);
    }
    אם (type->tt_argcount < 0)
	// any number of arguments
	GA_CONCAT_LITERAL(&ga, "...");

    אם (type->tt_member == &t_void)
	STRCPY((حرف *)ga.ga_data + ga.ga_len, ")");
    אחרת
    {
	حرف *ret_free = عدم;
	حرف *ret_name = type_name(type->tt_member, &ret_free);
	صحيح  len;

	len = (صحيح)STRLEN(ret_name) + 4;
	אם (ga_grow(&ga, len) == FAIL)
	{
	    vim_free(ret_free);
	    اذهب failed;
	}
	STRCPY((حرف *)ga.ga_data + ga.ga_len, "): ");
	STRCPY((حرف *)ga.ga_data + ga.ga_len + 3, ret_name);
	vim_free(ret_free);
    }
    *tofree = ga.ga_data;
    החזר ga.ga_data;

failed:
    vim_free(arg_free);
    ga_clear(&ga);
    החזר "[unknown]";
}

/*
 * Return the name of a type.
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    حرف *
type_name(type_T *type, حرف **tofree)
{
    حرف *name;

    *tofree = عدم;
    אם (type == عدم)
	החזר "[unknown]";
    name = vartype_name(type->tt_type);

    بدل (type->tt_type)
    {
	حالة VAR_LIST:
	حالة VAR_DICT:
	    החזר type_name_list_or_dict(name, type, tofree);

	حالة VAR_TUPLE:
	    החזר type_name_tuple(type, tofree);

	حالة VAR_CLASS:
	حالة VAR_OBJECT:
	    החזר type_name_class_or_obj(name, type, tofree);

	حالة VAR_FUNC:
	    החזר type_name_func(type, tofree);

	افتراضي:
	    اكسر;
    }

    החזר name;
}

/*
 * "typename(expr)" function
 */
    فراغ
f_typename(typval_T *argvars, typval_T *rettv)
{
    garray_T	type_list;
    type_T	*type;
    حرف	*tofree;
    حرف	*name;

    rettv->v_type = VAR_STRING;
    ga_init2(&type_list, حجم(type_T *), 10);
    אם (argvars[0].v_type == VAR_TYPEALIAS)
    {
	type = copy_type(argvars[0].vval.v_typealias->ta_type, &type_list);
	// A type alias for a class has the type set to VAR_OBJECT.  Change it
	// to VAR_CLASS, so that the name is "typealias<class<xxx>>"
	אם (type->tt_type == VAR_OBJECT)
	    type->tt_type = VAR_CLASS;
    }
    אחרת
	type = typval2type(argvars, get_copyID(), &type_list, TVTT_DO_MEMBER);
    name = type_name(type, &tofree);
    אם (argvars[0].v_type == VAR_TYPEALIAS)
    {
	vim_snprintf((حرف *)IObuff, IOSIZE, "typealias<%s>", name);
	rettv->vval.v_string = vim_strsave((char_u *)IObuff);
	אם (tofree != عدم)
	    vim_free(tofree);
    }
    אחרת
    {
	אם (tofree != عدم)
	    rettv->vval.v_string = (char_u *)tofree;
	אחרת
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
    صحيح
check_typval_is_value(typval_T *tv)
{
    אם (tv == عدم)
	החזר OK;

    بدل (tv->v_type)
    {
	حالة VAR_CLASS:
	    {
		class_T *cl = tv->vval.v_class;
		char_u *class_name = (cl == عدم) ? (char_u *)""
							: cl->class_name.string;
		אם (cl != عدم && IS_ENUM(cl))
		    semsg(_(e_using_enum_as_value_str), class_name);
		אחרת
		    semsg(_(e_using_class_as_value_str), class_name);
	    }
	    החזר FAIL;

	حالة VAR_TYPEALIAS:
	    semsg(_(e_using_typealias_as_value_str),
		    tv->vval.v_typealias->ta_name);
	    החזר FAIL;

	افتراضي:
	    اكسر;
    }
    החזר OK;
}

/*
 * Same as above, except check type_T.
 */
    صحيح
check_type_is_value(type_T *type)
{
    אם (type == عدم)
	החזר OK;
    بدل (type->tt_type)
    {
	حالة VAR_CLASS:
	    אם (type->tt_class != عدم && IS_ENUM(type->tt_class))
		semsg(_(e_using_enum_as_value_str),
			type->tt_class->class_name.string);
	    אחרת
		semsg(_(e_using_class_as_value_str),
			type->tt_class == عدم ? (char_u *)""
			: type->tt_class->class_name.string);
	    החזר FAIL;

	حالة VAR_TYPEALIAS:
	    // TODO: Not sure what could be done here to get a name.
	    //       Maybe an optional argument?
	    emsg(_(e_using_typealias_as_var_val));
	    החזר FAIL;

	افتراضي:
	    اكسر;
    }
    החזר OK;
}

#endif // FEAT_EVAL
