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
#شامل "vim.h"

#اگر defined(FEAT_EVAL)

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

    اگر (ga_grow(type_gap, 1) == FAIL)
	بازگشت پوچ;
    type = ALLOC_CLEAR_ONE(type_T);
    اگر (type == پوچ)
	بازگشت پوچ;

    ((type_T **)type_gap->ga_data)[type_gap->ga_len] = type;
    ++type_gap->ga_len;
    بازگشت type;
}

/*
 * Make a shallow copy of "type".
 * When allocation fails returns "type".
 */
    type_T *
copy_type(type_T *type, garray_T *type_gap)
{
    type_T *copy = get_type_ptr(type_gap);

    اگر (copy == پوچ)
	بازگشت type;
    *copy = *type;
    copy->tt_flags &= ~TTFLAG_STATIC;

    اگر (type->tt_args != پوچ
	   && func_type_add_arg_types(copy, type->tt_argcount, type_gap) == OK)
	برای (صحیح i = 0; i < type->tt_argcount; ++i)
	    copy->tt_args[i] = type->tt_args[i];

    بازگشت copy;
}

/*
 * Inner part of copy_type_deep().
 * When allocation fails returns "type".
 */
    ایستا type_T *
copy_type_deep_rec(type_T *type, garray_T *type_gap, garray_T *seen_types)
{
    برای (صحیح i = 0; i < seen_types->ga_len; ++i)
	اگر (((type_T **)seen_types->ga_data)[i * 2] == type)
	    // seen this type before, return the copy we made
	    بازگشت ((type_T **)seen_types->ga_data)[i * 2 + 1];

    type_T *copy = copy_type(type, type_gap);
    اگر (ga_grow(seen_types, 1) == FAIL)
	بازگشت copy;
    ((type_T **)seen_types->ga_data)[seen_types->ga_len * 2] = type;
    ((type_T **)seen_types->ga_data)[seen_types->ga_len * 2 + 1] = copy;
    ++seen_types->ga_len;

    اگر (copy->tt_member != پوچ)
	copy->tt_member = copy_type_deep_rec(copy->tt_member,
							 type_gap, seen_types);
    اگر (type->tt_args != پوچ)
	برای (صحیح i = 0; i < type->tt_argcount; ++i)
	    copy->tt_args[i] = copy_type_deep_rec(copy->tt_args[i],
							 type_gap, seen_types);

    بازگشت copy;
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
    ga_init2(&seen_types, اندازه(type_T *) * 2, 20);

    type_T *res = copy_type_deep_rec(type, type_gap, &seen_types);

    ga_clear(&seen_types);
    بازگشت res;
}

    تهی
clear_type_list(garray_T *gap)
{
    وقتی (gap->ga_len > 0)
	vim_free(((type_T **)gap->ga_data)[--gap->ga_len]);
    ga_clear(gap);
}

    تهی
clear_func_type_list(garray_T *gap, type_T **func_type)
{
    وقتی (gap->ga_len > 0)
    {
	// func_type pointing to the uf_type_list, so reset pointer
	اگر (*func_type == ((type_T **)gap->ga_data)[--gap->ga_len])
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

    اگر (type == پوچ)
	بازگشت پوچ;

    // A fixed type never contains allocated types, return as-is.
    اگر (type->tt_flags & TTFLAG_STATIC)
	بازگشت type;

    ret = ALLOC_ONE(type_T);
    اگر (ret == پوچ)
	بازگشت پوچ;
    *ret = *type;

    اگر (ret->tt_member != پوچ)
	ret->tt_member = alloc_type(ret->tt_member);

    اگر (type->tt_argcount > 0 && type->tt_args != پوچ)
    {
	صحیح i;

	ret->tt_args = ALLOC_MULT(type_T *, type->tt_argcount);
	اگر (ret->tt_args != پوچ)
	    برای (i = 0; i < type->tt_argcount; ++i)
		ret->tt_args[i] = alloc_type(type->tt_args[i]);
    }
    وگرنه
	ret->tt_args = پوچ;

    بازگشت ret;
}

/*
 * Free a type that was created with alloc_type().
 */
    تهی
free_type(type_T *type)
{
    صحیح i;

    اگر (type == پوچ || (type->tt_flags & TTFLAG_STATIC))
	بازگشت;
    اگر (type->tt_args != پوچ)
    {
	برای (i = 0; i < type->tt_argcount; ++i)
	    free_type(type->tt_args[i]);
	vim_free(type->tt_args);
    }

    free_type(type->tt_member);

    vim_free(type);
}

/*
 * Return TRUE if "type" is to be recursed into for setting the type.
 */
    ایستا صحیح
set_tv_type_recurse(type_T *type)
{
    بازگشت type->tt_member != پوچ
		&& (type->tt_member->tt_type == VAR_DICT
				|| type->tt_member->tt_type == VAR_LIST)
		&& type->tt_member->tt_member != پوچ
		&& type->tt_member->tt_member != &t_any
		&& type->tt_member->tt_member != &t_unknown;
}

/*
 * Set the type of Dict "d" to "type"
 */
    ایستا تهی
set_tv_type_dict(dict_T *d, type_T *type)
{
    اگر (d->dv_type == type)
	بازگشت;

    free_type(d->dv_type);
    d->dv_type = alloc_type(type);

    // Need to recursively set the type of dict items?
    اگر (!set_tv_type_recurse(type))
	بازگشت;

    صحیح		todo = (صحیح)d->dv_hashtab.ht_used;
    hashitem_T	*hi;
    dictitem_T	*di;

    FOR_ALL_HASHTAB_اقلام(&d->dv_hashtab, hi, todo)
    {
	اگر (!HASHITEM_EMPTY(hi))
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
    ایستا تهی
set_tv_type_list(list_T *l, type_T *type)
{
    اگر (l->lv_type == type)
	بازگشت;

    free_type(l->lv_type);
    l->lv_type = alloc_type(type);

    // Need to recursively set the type of list items?
    اگر (l->lv_first == &range_list_item || !set_tv_type_recurse(type))
	بازگشت;

    listitem_T	*li;

    FOR_ALL_LIST_اقلام(l, li)
	set_tv_type(&li->li_tv, type->tt_member);
}

/*
 * Set the type of Tuple "tuple" to "type"
 */
    ایستا تهی
set_tv_type_tuple(tuple_T *tuple, type_T *type)
{
    اگر (tuple->tv_type == type)
	بازگشت;

    free_type(tuple->tv_type);
    tuple->tv_type = alloc_type(type);

    اگر (type->tt_argcount <= 0)
	بازگشت;

    // recursively set the type of list items
    type_T	*item_type;
    برای (صحیح i = 0; i < tuple_len(tuple); i++)
    {
	اگر ((type->tt_flags & TTFLAG_VARARGS) && (i >= type->tt_argcount - 1))
	    // For a variadic tuple, the last type is a List.  So use the
	    // List member type.
	    item_type = type->tt_args[type->tt_argcount - 1]->tt_member;
	وگرنه
	    item_type = type->tt_args[i];

	set_tv_type(TUPLE_ITEM(tuple, i), item_type);
    }
}

/*
 * Set the type of "tv" to "type" if it is a list or tuple or dict.
 */
    تهی
set_tv_type(typval_T *tv, type_T *type)
{
    اگر (type->tt_type == VAR_ANY)
	// If the variable type is "any", then keep the value type.
	// e.g.  var x: any = [1, 2] or var y: any = {v: 1}
	بازگشت;

    اگر (tv->v_type == VAR_DICT && tv->vval.v_dict != پوچ)
	set_tv_type_dict(tv->vval.v_dict, type);
    وگرنه اگر (tv->v_type == VAR_LIST && tv->vval.v_list != پوچ)
	set_tv_type_list(tv->vval.v_list, type);
    وگرنه اگر (tv->v_type == VAR_TUPLE && tv->vval.v_tuple != پوچ)
	set_tv_type_tuple(tv->vval.v_tuple, type);
}

/*
 * For a tuple type, reserve space for "typecount" types (including the
 * repeated type).
 */
    ایستا صحیح
tuple_type_add_types(
    type_T	*tupletype,
    صحیح		typecount,
    garray_T	*type_gap)
{
    // To make it easy to free the space needed for the types, add the
    // pointer to type_gap.
    اگر (ga_grow(type_gap, 1) == FAIL)
	بازگشت FAIL;
    tupletype->tt_args = alloc_clear_id(اندازه(type_T *) * typecount,
							  aid_tuple_type_args);
    اگر (tupletype->tt_args == پوچ)
	بازگشت FAIL;
    ((type_T **)type_gap->ga_data)[type_gap->ga_len] =
						(تهی *)tupletype->tt_args;
    ++type_gap->ga_len;
    بازگشت OK;
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
    اگر (member_type == پوچ || (member_type->tt_type == VAR_ANY
					&& !IS_GENERIC_TYPE(member_type)))
	بازگشت &t_list_any;
    اگر (member_type->tt_type == VAR_VOID
	    || member_type->tt_type == VAR_UNKNOWN)
	بازگشت &t_list_empty;
    اگر (member_type->tt_type == VAR_BOOL)
	بازگشت &t_list_bool;
    اگر (member_type->tt_type == VAR_عدد)
	بازگشت &t_list_عدد;
    اگر (member_type->tt_type == VAR_STRING)
	بازگشت &t_list_string;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    اگر (type == پوچ)
	بازگشت &t_any;
    type->tt_type = VAR_LIST;
    type->tt_member = member_type;
    type->tt_argcount = 0;
    type->tt_args = پوچ;
    بازگشت type;
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
    صحیح		typecount = tuple_types_gap->ga_len;

    // recognize commonly used types
    اگر (typecount == 0)
	بازگشت &t_tuple_any;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    اگر (type == پوچ)
	بازگشت &t_any;
    type->tt_type = VAR_TUPLE;
    type->tt_member = پوچ;
    اگر (typecount > 0)
    {
	اگر (tuple_type_add_types(type, typecount, type_gap) == FAIL)
	    بازگشت &t_any;
	mch_memmove(type->tt_args, tuple_types, اندازه(type_T *) * typecount);
    }
    type->tt_argcount = typecount;
    type->tt_flags = 0;

    بازگشت type;
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
    اگر (member_type == پوچ || (member_type->tt_type == VAR_ANY
					&& !IS_GENERIC_TYPE(member_type)))
	بازگشت &t_dict_any;
    اگر (member_type->tt_type == VAR_VOID
	    || member_type->tt_type == VAR_UNKNOWN)
	بازگشت &t_dict_empty;
    اگر (member_type->tt_type == VAR_BOOL)
	بازگشت &t_dict_bool;
    اگر (member_type->tt_type == VAR_عدد)
	بازگشت &t_dict_عدد;
    اگر (member_type->tt_type == VAR_STRING)
	بازگشت &t_dict_string;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    اگر (type == پوچ)
	بازگشت &t_any;
    type->tt_type = VAR_DICT;
    type->tt_member = member_type;
    type->tt_argcount = 0;
    type->tt_args = پوچ;
    بازگشت type;
}

/*
 * Allocate a new type for a function.
 */
    type_T *
alloc_func_type(type_T *ret_type, صحیح argcount, garray_T *type_gap)
{
    type_T *type = get_type_ptr(type_gap);

    اگر (type == پوچ)
	بازگشت &t_any;
    type->tt_type = VAR_FUNC;
    type->tt_member = ret_type == پوچ ? &t_unknown : ret_type;
    type->tt_argcount = argcount;
    type->tt_args = پوچ;
    بازگشت type;
}

/*
 * Allocate a new type for a tuple.
 */
    ایستا type_T *
alloc_tuple_type(صحیح typecount, garray_T *type_gap)
{
    type_T *type = get_type_ptr(type_gap);

    اگر (type == پوچ)
	بازگشت &t_any;
    type->tt_type = VAR_TUPLE;
    type->tt_member = پوچ;
    type->tt_argcount = typecount;
    type->tt_args = پوچ;
    بازگشت type;
}

/*
 * Get a function type, based on the return type "ret_type".
 * "argcount" must be -1 or 0, a predefined type can be used.
 */
    type_T *
get_func_type(type_T *ret_type, صحیح argcount, garray_T *type_gap)
{
    // recognize commonly used types
    اگر (ret_type == &t_unknown || ret_type == پوچ)
    {
	// (argcount == 0) is not possible
	بازگشت &t_func_unknown;
    }
    اگر (ret_type == &t_void)
    {
	اگر (argcount == 0)
	    بازگشت &t_func_0_void;
	وگرنه
	    بازگشت &t_func_void;
    }
    اگر (ret_type == &t_any)
    {
	اگر (argcount == 0)
	    بازگشت &t_func_0_any;
	وگرنه
	    بازگشت &t_func_any;
    }
    اگر (ret_type == &t_عدد)
    {
	اگر (argcount == 0)
	    بازگشت &t_func_0_عدد;
	وگرنه
	    بازگشت &t_func_عدد;
    }
    اگر (ret_type == &t_string)
    {
	اگر (argcount == 0)
	    بازگشت &t_func_0_string;
	وگرنه
	    بازگشت &t_func_string;
    }

    بازگشت alloc_func_type(ret_type, argcount, type_gap);
}

/*
 * For a function type, reserve space for "argcount" argument types (including
 * vararg).
 */
    صحیح
func_type_add_arg_types(
	type_T	    *functype,
	صحیح	    argcount,
	garray_T    *type_gap)
{
    // To make it easy to free the space needed for the argument types, add the
    // pointer to type_gap.
    اگر (ga_grow(type_gap, 1) == FAIL)
	بازگشت FAIL;
    functype->tt_args = ALLOC_CLEAR_MULT(type_T *, argcount);
    اگر (functype->tt_args == پوچ)
	بازگشت FAIL;
    ((type_T **)type_gap->ga_data)[type_gap->ga_len] =
						     (تهی *)functype->tt_args;
    ++type_gap->ga_len;
    بازگشت OK;
}

/*
 * Return TRUE if "type" is NULL, any or unknown.
 * This also works for const (comparing with &t_any and &t_unknown doesn't).
 */
    صحیح
type_any_or_unknown(type_T *type)
{
    بازگشت type == پوچ || type->tt_type == VAR_ANY
					|| type->tt_type == VAR_UNKNOWN;
}

/*
 * Get a type_T for a "special" typval in "tv".
 */
    ایستا type_T *
special_typval2type(typval_T *tv)
{
    انتخاب (tv->vval.v_عدد)
    {
	حالت VVAL_NULL:
	    بازگشت &t_null;

	حالت VVAL_NONE:
	    بازگشت &t_none;

	حالت VVAL_TRUE:
	حالت VVAL_FALSE:
	    بازگشت &t_bool;

	پیشفرض:
	    بازگشت &t_unknown;
    }
}

/*
 * Get a type_T for a List typval in "tv".
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    ایستا type_T *
list_typval2type(typval_T *tv, صحیح copyID, garray_T *type_gap, صحیح flags)
{
    list_T	*l = tv->vval.v_list;
    listitem_T	*li;
    type_T	*member_type = پوچ;

    // An empty list has type list<unknown>, unless the type was specified
    // and is not list<any>.  This matters when assigning to a variable
    // with a specific list type.
    اگر (l == پوچ || (l->lv_first == پوچ
		&& (l->lv_type == پوچ || l->lv_type->tt_member == &t_any)))
	بازگشت &t_list_empty;

    اگر ((flags & TVTT_DO_MEMBER) == 0)
	بازگشت &t_list_any;

    // If the type is list<any> go through the members, it may end up a
    // more specific type.
    اگر (l->lv_type != پوچ && (l->lv_first == پوچ
		|| (flags & TVTT_MORE_SPECIFIC) == 0
		|| l->lv_type->tt_member != &t_any))
	// make a copy, lv_type may be freed if the list is freed
	بازگشت copy_type_deep(l->lv_type, type_gap);

    اگر (l->lv_first == &range_list_item)
	بازگشت &t_list_عدد;

    اگر (l->lv_copyID == copyID)
	// avoid recursion
	بازگشت &t_list_any;

    l->lv_copyID = copyID;

    // Use the common type of all members.
    member_type = typval2type(&l->lv_first->li_tv, copyID, type_gap,
							TVTT_DO_MEMBER);
    برای (li = l->lv_first->li_next; li != پوچ; li = li->li_next)
	common_type(typval2type(&li->li_tv, copyID, type_gap, TVTT_DO_MEMBER),
					member_type, &member_type, type_gap);

    // Reset copyID so that a shared reference to this list (not a circular
    // reference) can be processed again to get the correct type.
    l->lv_copyID = 0;

    بازگشت get_list_type(member_type, type_gap);
}

/*
 * Get a type_T for a Tuple typval in "tv".
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    ایستا type_T *
tuple_typval2type(typval_T *tv, صحیح copyID, garray_T *type_gap, صحیح flags)
{
    tuple_T	*tuple = tv->vval.v_tuple;
    صحیح		len = tuple_len(tuple);
    type_T	*type = پوچ;

    // An empty tuple has type tuple<unknown>, unless the type was specified
    // and is not tuple<any>.  This matters when assigning to a variable
    // with a specific tuple type.
    اگر (tuple == پوچ || (len == 0 && (tuple->tv_type == پوچ
				|| tuple->tv_type->tt_argcount == 0)))
	بازگشت &t_tuple_empty;

    اگر ((flags & TVTT_DO_MEMBER) == 0)
	بازگشت &t_tuple_any;

    // If the type is tuple<any> go through the members, it may end up a
    // more specific type.
    اگر (tuple->tv_type != پوچ && (len == 0
					|| (flags & TVTT_MORE_SPECIFIC) == 0))
	// make a copy, tv_type may be freed if the tuple is freed
	بازگشت copy_type_deep(tuple->tv_type, type_gap);

    اگر (tuple->tv_copyID == copyID)
	// avoid recursion
	بازگشت &t_tuple_any;

    tuple->tv_copyID = copyID;

    garray_T	tuple_types_ga;
    ga_init2(&tuple_types_ga, اندازه(type_T *), 10);
    برای (صحیح i = 0; i < len; i++)
    {
	type = typval2type(TUPLE_ITEM(tuple, i), copyID, type_gap,
							TVTT_DO_MEMBER);
	اگر (ga_grow(&tuple_types_ga, 1) == FAIL)
	{
	    ga_clear(&tuple_types_ga);
	    // Reset copyID so that a shared reference to this tuple can be
	    // processed again.
	    tuple->tv_copyID = 0;
	    بازگشت پوچ;
	}
	((type_T **)tuple_types_ga.ga_data)[tuple_types_ga.ga_len] = type;
	tuple_types_ga.ga_len++;
    }

    type_T *tuple_type = get_tuple_type(&tuple_types_ga, type_gap);
    ga_clear(&tuple_types_ga);

    // Reset copyID so that a shared reference to this tuple (not a circular
    // reference) can be processed again to get the correct type.
    tuple->tv_copyID = 0;

    بازگشت tuple_type;
}

/*
 * Get a type_T for a Dict typval in "tv".
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    ایستا type_T *
dict_typval2type(typval_T *tv, صحیح copyID, garray_T *type_gap, صحیح flags)
{
    dict_iterator_T	iter;
    typval_T		*مقدار;
    dict_T		*d = tv->vval.v_dict;
    type_T		*member_type = پوچ;

    اگر (d == پوچ || (d->dv_hashtab.ht_used == 0 && d->dv_type == پوچ))
	بازگشت &t_dict_empty;

    اگر ((flags & TVTT_DO_MEMBER) == 0)
	بازگشت &t_dict_any;

    // If the type is dict<any> go through the members, it may end up a
    // more specific type.
    اگر (d->dv_type != پوچ && (d->dv_hashtab.ht_used == 0
		|| (flags & TVTT_MORE_SPECIFIC) == 0
		|| d->dv_type->tt_member != &t_any))
	بازگشت d->dv_type;

    اگر (d->dv_copyID == copyID)
	// avoid recursion
	بازگشت &t_dict_any;

    d->dv_copyID = copyID;

    // Use the common type of all values.
    dict_iterate_start(tv, &iter);
    dict_iterate_next(&iter, &مقدار);
    member_type = typval2type(مقدار, copyID, type_gap, TVTT_DO_MEMBER);

    وقتی (dict_iterate_next(&iter, &مقدار) != پوچ)
	common_type(typval2type(مقدار, copyID, type_gap, TVTT_DO_MEMBER),
					member_type, &member_type, type_gap);

    // Reset copyID so that a shared reference to this dict (not a circular
    // reference) can be processed again to get the correct type.
    d->dv_copyID = 0;

    بازگشت get_dict_type(member_type, type_gap);
}

/*
 * Get a type_T for a "partial" typval in "tv".
 */
    ایستا type_T *
partial_typval2type(typval_T *tv, ufunc_T *ufunc, garray_T *type_gap)
{
    partial_T *pt = tv->vval.v_partial;
    type_T  *type;

    type = get_type_ptr(type_gap);
    اگر (type == پوچ)
	بازگشت پوچ;

    *type = *ufunc->uf_func_type;
    اگر (type->tt_argcount >= 0 && pt->pt_argc > 0)
    {
	type->tt_argcount -= pt->pt_argc;
	type->tt_min_argcount -= pt->pt_argc;
	اگر (type->tt_argcount > 0 && func_type_add_arg_types(type,
		    type->tt_argcount, type_gap) == OK)
	    برای (صحیح i = 0; i < type->tt_argcount; ++i)
		type->tt_args[i] =
		    ufunc->uf_func_type->tt_args[i + pt->pt_argc];
    }
    اگر (pt->pt_func != پوچ)
	type->tt_member = pt->pt_func->uf_ret_type;

    بازگشت type;
}

/*
 * Get a type_T for a "class" or an "object" typval in "tv".
 */
    ایستا type_T *
oc_typval2type(typval_T *tv)
{
    اگر (tv->v_type == VAR_CLASS)
    {
	اگر (tv->vval.v_class == پوچ)
	    بازگشت &t_class;

	بازگشت &tv->vval.v_class->class_type;
    }

    اگر (tv->vval.v_object != پوچ)
	بازگشت &tv->vval.v_object->obj_class->class_object_type;

    بازگشت &t_object_any;
}

/*
 * Get a type_T for a "function" or a "partial"
 */
    ایستا type_T *
fp_typval2type(typval_T *tv, garray_T *type_gap)
{
    char_u	*name = پوچ;
    ufunc_T	*ufunc = پوچ;
    type_T	*type;
    type_T	*member_type = پوچ;
    صحیح		argcount = 0;
    صحیح		min_argcount = 0;

    اگر (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != پوچ)
    {
	اگر (tv->vval.v_partial->pt_func != پوچ)
	    ufunc = tv->vval.v_partial->pt_func;
	وگرنه
	    name = tv->vval.v_partial->pt_name;
    }
    وگرنه
	name = tv->vval.v_string;

    اگر (name == پوچ && ufunc == پوچ)
	بازگشت &t_func_unknown;

    اگر (name != پوچ)
    {
	صحیح idx = find_internal_func(name);

	اگر (idx >= 0)
	{
	    type_T *decl_type;  // unused

	    internal_func_get_argcount(idx, &argcount, &min_argcount);
	    member_type = internal_func_ret_type(idx, 0, پوچ, &decl_type,
		    type_gap);
	}
	وگرنه
	{
	    // Check if name contains "<".  If it does, then replace "<" with
	    // NUL and lookup the function and then look up the specific
	    // generic function using the supplied types.
	    char_u *p = generic_func_find_open_bracket(name);
	    اگر (p == پوچ)
		ufunc = find_func(name, FALSE);
	    وگرنه
	    {
		// generic function
		char_u	cc = *p;

		*p = NUL;
		ufunc = find_func(name, FALSE);
		*p = cc;
		اگر (ufunc != پوچ && IS_GENERIC_FUNC(ufunc))
		{
		    ufunc = find_generic_func(ufunc, name, &p);
		    اگر (ufunc == پوچ)
			بازگشت پوچ;
		}
	    }
	}
    }
    اگر (ufunc != پوچ)
    {
	// May need to get the argument types from default values by
	// compiling the function.
	اگر (ufunc->uf_def_status == UF_TO_BE_COMPILED
		&& compile_def_function(ufunc, TRUE, CT_NONE, پوچ)
		== FAIL)
	    بازگشت پوچ;
	اگر (ufunc->uf_func_type == پوچ)
	    set_function_type(ufunc);
	اگر (ufunc->uf_func_type != پوچ)
	{
	    اگر (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != پوچ)
		بازگشت partial_typval2type(tv, ufunc, type_gap);
	    بازگشت ufunc->uf_func_type;
	}
    }

    type = get_type_ptr(type_gap);
    اگر (type == پوچ)
	بازگشت پوچ;
    type->tt_type = tv->v_type;
    type->tt_argcount = argcount;
    type->tt_min_argcount = min_argcount;
    اگر (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != پوچ
					&& tv->vval.v_partial->pt_argc > 0)
    {
	type->tt_argcount -= tv->vval.v_partial->pt_argc;
	type->tt_min_argcount -= tv->vval.v_partial->pt_argc;
    }
    type->tt_member = member_type;

    بازگشت type;
}

/*
 * Get a type_T for a typval_T.
 * "type_gap" is used to temporarily create types in.
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    ایستا type_T *
typval2type_int(typval_T *tv, صحیح copyID, garray_T *type_gap, صحیح flags)
{
    انتخاب (tv->v_type)
    {
	حالت VAR_UNKNOWN:
	    بازگشت &t_unknown;

	حالت VAR_ANY:
	    بازگشت &t_any;

	حالت VAR_VOID:
	    بازگشت &t_void;

	حالت VAR_BOOL:
	    بازگشت &t_bool;

	حالت VAR_SPECIAL:
	    بازگشت special_typval2type(tv);

	حالت VAR_عدد:
	    بازگشت &t_عدد;

	حالت VAR_FLOAT:
	    بازگشت &t_float;

	حالت VAR_STRING:
	    بازگشت &t_string;

	حالت VAR_BLOB:
	    اگر (tv->vval.v_blob == پوچ)
		بازگشت &t_blob_null;
	    بازگشت &t_blob;

	حالت VAR_LIST:
	    بازگشت list_typval2type(tv, copyID, type_gap, flags);

	حالت VAR_TUPLE:
	    بازگشت tuple_typval2type(tv, copyID, type_gap, flags);

	حالت VAR_DICT:
	    بازگشت dict_typval2type(tv, copyID, type_gap, flags);

	حالت VAR_JOB:
	    بازگشت &t_job;

	حالت VAR_CHANNEL:
	    بازگشت &t_channel;

	حالت VAR_CLASS:
	حالت VAR_OBJECT:
	    بازگشت oc_typval2type(tv);

	حالت VAR_TYPEALIAS:
	    بازگشت &t_typealias;

	حالت VAR_FUNC:
	حالت VAR_PARTIAL:
	    بازگشت fp_typval2type(tv, type_gap);

	حالت VAR_INSTR:
	پیشفرض:
	    بشکن;
    }

    بازگشت پوچ;
}

/*
 * Return TRUE if "tv" is not a bool but should be converted to bool.
 */
    صحیح
need_convert_to_bool(type_T *type, typval_T *tv)
{
    بازگشت type != پوچ && type == &t_bool && tv->v_type != VAR_BOOL
	    && (tv->v_type == VAR_عدد
		       && (tv->vval.v_عدد == 0 || tv->vval.v_عدد == 1));
}

/*
 * Get a type_T for a typval_T.
 * "type_list" is used to temporarily create types in.
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the most specific member type.
 */
    type_T *
typval2type(typval_T *tv, صحیح copyID, garray_T *type_gap, صحیح flags)
{
    type_T *type = typval2type_int(tv, copyID, type_gap, flags);

    اگر (type == پوچ)
	بازگشت پوچ;

    اگر (type != &t_bool && (tv->v_type == VAR_عدد
		&& (tv->vval.v_عدد == 0 || tv->vval.v_عدد == 1)))
	// Number 0 and 1 and expression with "&&" or "||" can also be used
	// for bool.
	type = &t_عدد_bool;
    وگرنه اگر (type != &t_float && tv->v_type == VAR_عدد)
	// A number can also be used for float.
	type = &t_عدد_float;
    بازگشت type;
}

/*
 * Return TRUE if "type" can be used for a variable declaration.
 * Give an error and return FALSE if not.
 */
    صحیح
valid_declaration_type(type_T *type)
{
    اگر (type->tt_type == VAR_SPECIAL  // null, none
	    || type->tt_type == VAR_VOID)
    {
	نویسه *tofree = پوچ;
	نویسه *name = type_name(type, &tofree);
	semsg(_(e_invalid_type_in_variable_declaration_str), name);
	vim_free(tofree);
	بازگشت FALSE;
    }
    بازگشت TRUE;
}

/*
 * Get a type_T for a typval_T, used for v: variables.
 * "type_list" is used to temporarily create types in.
 */
    type_T *
typval2type_vimvar(typval_T *tv, garray_T *type_gap)
{
    اگر (tv->v_type == VAR_LIST)  // e.g. for v:oldfiles
	بازگشت &t_list_string;
    اگر (tv->v_type == VAR_DICT)  // e.g. for v:event
	بازگشت &t_dict_any;
    بازگشت typval2type(tv, get_copyID(), type_gap, TVTT_DO_MEMBER);
}

    صحیح
check_typval_arg_type(
	type_T	    *expected,
	typval_T    *actual_tv,
	نویسه	    *func_name,
	صحیح	    arg_idx)
{
    where_T	where = WHERE_INIT;

    اگر (arg_idx > 0)
    {
	where.wt_index = arg_idx;
	where.wt_kind = WT_ARGUMENT;
    }
    where.wt_func_name = func_name;
    بازگشت check_typval_type(expected, actual_tv, where);
}

/*
 * Return true if "expected" is list<any> or dict<any> and "actual_tv" is a
 * list or dict of that kind, which matches whatever its values are.
 */
    ایستا bool
matches_listdict_any(type_T *expected, typval_T *actual_tv)
{
    بازگشت (expected->tt_type == VAR_LIST || expected->tt_type == VAR_DICT)
	    && actual_tv->v_type == expected->tt_type
	    && expected->tt_member != پوچ
	    && (expected->tt_member->tt_type == VAR_ANY
		|| expected->tt_member->tt_type == VAR_UNKNOWN);
}

/*
 * Return FAIL if "expected" and "actual" don't match.
 * When "argidx" > 0 it is included in the error message.
 */
    صحیح
check_typval_type(type_T *expected, typval_T *actual_tv, where_T where)
{
    garray_T	type_list;
    type_T	*actual_type;
    صحیح		res = FAIL;

    اگر (expected == پوچ)
	بازگشت OK;  // didn't expect anything.

    // No need to go through the values of a possibly big list or dict.
    اگر (matches_listdict_any(expected, actual_tv))
	بازگشت OK;

    ga_init2(&type_list, اندازه(type_T *), 10);

    // A null_function and null_partial are special cases, they can be used to
    // clear a variable.
    اگر ((actual_tv->v_type == VAR_FUNC && actual_tv->vval.v_string == پوچ)
	    || (actual_tv->v_type == VAR_PARTIAL
					 && actual_tv->vval.v_partial == پوچ))
	actual_type = &t_func_unknown;
    وگرنه
	// When the actual type is list<any> or dict<any> go through the values
	// to possibly get a more specific type.
	actual_type = typval2type(actual_tv, get_copyID(), &type_list,
					  TVTT_DO_MEMBER | TVTT_MORE_SPECIFIC);
    اگر (actual_type != پوچ)
    {
	res = check_type_maybe(expected, actual_type, TRUE, where);
	اگر (res == MAYBE && !(actual_type->tt_type == VAR_FUNC
				      && (actual_type->tt_member == &t_unknown
					  || actual_type->tt_member == پوچ)))
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
    بازگشت res;
}

    تهی
arg_type_mismatch(type_T *expected, type_T *actual, صحیح arg_idx)
{
    where_T	where = WHERE_INIT;

    اگر (arg_idx > 0)
    {
	where.wt_index = arg_idx;
	where.wt_kind = WT_ARGUMENT;
    }
    type_mismatch_where(expected, actual, where);
}

    تهی
type_mismatch_where(type_T *expected, type_T *actual, where_T where)
{
    نویسه *tofree1, *tofree2;
    نویسه *typename1 = type_name(expected, &tofree1);
    نویسه *typename2 = type_name(actual, &tofree2);

    انتخاب (where.wt_kind)
    {
	حالت WT_MEMBER:
	    semsg(_(e_variable_str_type_mismatch_expected_str_but_got_str),
		    where.wt_func_name, typename1, typename2);
	    بشکن;
	حالت WT_METHOD:
	حالت WT_METHOD_ARG:
	حالت WT_METHOD_RETURN:
	    semsg(_(e_method_str_type_mismatch_expected_str_but_got_str),
		    where.wt_func_name, typename1, typename2);
	    بشکن;
	حالت WT_VARIABLE:
	    اگر (where.wt_func_name == پوچ)
		semsg(_(e_variable_nr_type_mismatch_expected_str_but_got_str),
			where.wt_index, typename1, typename2);
	    وگرنه
		semsg(_(e_variable_nr_type_mismatch_expected_str_but_got_str_in_str),
			where.wt_index, typename1, typename2, where.wt_func_name);
	    بشکن;
	حالت WT_ARGUMENT:
	    اگر (where.wt_func_name == پوچ)
		semsg(_(e_argument_nr_type_mismatch_expected_str_but_got_str),
			where.wt_index, typename1, typename2);
	    وگرنه
		semsg(_(e_argument_nr_type_mismatch_expected_str_but_got_str_in_str),
			where.wt_index, typename1, typename2, where.wt_func_name);
	    بشکن;
	حالت WT_CAST:
	حالت WT_UNKNOWN:
	    اگر (where.wt_func_name == پوچ)
		semsg(_(e_type_mismatch_expected_str_but_got_str),
			typename1, typename2);
	    وگرنه
		semsg(_(e_type_mismatch_expected_str_but_got_str_in_str),
			typename1, typename2, where.wt_func_name);
	    بشکن;
    }

    vim_free(tofree1);
    vim_free(tofree2);
}

/*
 * Check if the expected and actual types match for a tuple
 */
    ایستا صحیح
check_tuple_type_maybe(
    type_T	*expected,
    type_T	*actual,
    where_T	where)
{
    اگر (expected->tt_argcount == -1 || actual->tt_argcount == -1
		|| expected->tt_args == پوچ || actual->tt_args == پوچ)
	بازگشت OK;

    // For a non-variadic tuple, the number of items must match
    اگر (!(expected->tt_flags & TTFLAG_VARARGS)
	    && expected->tt_argcount != actual->tt_argcount)
	بازگشت FAIL;

    // compare the type of each tuple item
    برای (صحیح i = 0; i < actual->tt_argcount; ++i)
    {
	type_T	*exp_type;
	type_T	*actual_type;

	اگر (expected->tt_flags & TTFLAG_VARARGS)
	{
	    اگر (i < expected->tt_argcount - 1)
		exp_type = expected->tt_args[i];
	    وگرنه
		// For a variadic tuple, the last type is a List.  So use the
		// List member type.
		exp_type = expected->tt_args[expected->tt_argcount - 1]->tt_member;
	}
	وگرنه
	    exp_type = expected->tt_args[i];

	اگر (actual->tt_flags & TTFLAG_VARARGS)
	{
	    اگر (i < actual->tt_argcount - 1)
		actual_type = actual->tt_args[i];
	    وگرنه
		// For a variadic tuple, the last type is a List.  So use the
		// List member type.
		actual_type = actual->tt_args[actual->tt_argcount - 1]->tt_member;
	}
	وگرنه
	    actual_type = actual->tt_args[i];

	// Allow for using "any" type for a tuple item
	اگر (actual->tt_args[i] != &t_any && check_type(exp_type, actual_type,
						FALSE, where) == FAIL)
	    بازگشت FAIL;
    }

    بازگشت OK;
}

/*
 * Check if the expected and actual types match for a function
 * Returns OK if "expected" and "actual" are matching function types.
 * Returns FAIL if "expected" and "actual" are different types.
 * Returns MAYBE when a runtime type check is needed.
 */
    ایستا صحیح
check_func_type_maybe(
    type_T	*expected,
    type_T	*actual,
    where_T	where)
{
    صحیح ret = OK;

    // If the return type is unknown it can be anything, including
    // nothing, thus there is no point in checking.
    اگر (expected->tt_member != &t_unknown)
    {
	اگر (actual->tt_member != پوچ
		&& actual->tt_member != &t_unknown)
	{
	    where_T  func_where = where;

	    func_where.wt_kind = WT_METHOD_RETURN;
	    ret = check_type_maybe(expected->tt_member,
		    actual->tt_member, FALSE,
		    func_where);
	}
	وگرنه
	    ret = MAYBE;
    }
    اگر (ret != FAIL
	    && ((expected->tt_flags & TTFLAG_VARARGS)
		!= (actual->tt_flags & TTFLAG_VARARGS))
	    && expected->tt_argcount != -1)
	ret = FAIL;
    اگر (ret != FAIL && expected->tt_argcount != -1
	    && actual->tt_min_argcount != -1
	    && (actual->tt_argcount == -1
		|| (actual->tt_argcount < expected->tt_min_argcount
		    || actual->tt_argcount > expected->tt_argcount)))
	ret = FAIL;
    اگر (ret != FAIL && expected->tt_args != پوچ
	    && actual->tt_args != پوچ)
    {
	صحیح i;

	برای (i = 0; i < expected->tt_argcount
		&& i < actual->tt_argcount; ++i)
	{
	    where_T  func_where = where;
	    func_where.wt_kind = WT_METHOD_ARG;

	    // Allow for using "any" argument type, lambda's have them.
	    اگر (actual->tt_args[i] != &t_any && check_type(
			expected->tt_args[i], actual->tt_args[i], FALSE,
			func_where) == FAIL)
	    {
		ret = FAIL;
		بشکن;
	    }
	}
    }
    اگر (ret == OK && expected->tt_argcount >= 0
	    && actual->tt_argcount == -1)
	// check the argument count at runtime
	ret = MAYBE;

    بازگشت ret;
}

/*
 * Check if the expected and actual types match for an object
 * Returns OK if "expected" and "actual" are matching object types.
 * Returns FAIL if "expected" and "actual" are different types.
 * Returns MAYBE when a runtime type check is needed.
 */
    ایستا صحیح
check_object_type_maybe(
    type_T	*expected,
    type_T	*actual,
    where_T	where)
{
    صحیح ret = OK;

    اگر (actual->tt_type == VAR_ANY)
	بازگشت MAYBE;	// use runtime type check
    اگر (actual->tt_type != VAR_OBJECT)
	بازگشت FAIL;	// don't use tt_class
    اگر (actual->tt_class == پوچ)    // null object
	بازگشت OK;
    // t_object_any matches any object except for an enum item
    اگر (expected == &t_object_any && !IS_ENUM(actual->tt_class))
	بازگشت OK;

    // For object method arguments, do a invariant type check in
    // an extended class.  For all others, do a covariance type check.
    اگر (where.wt_kind == WT_METHOD_ARG)
    {
	اگر (actual->tt_class != expected->tt_class)
	    ret = FAIL;
    }
    وگرنه اگر (!class_instance_of(actual->tt_class, expected->tt_class))
    {
	// Check if this is an up-cast, if so we'll have to check the type at
	// runtime.
	اگر (where.wt_kind == WT_CAST &&
		class_instance_of(expected->tt_class, actual->tt_class))
	    ret = MAYBE;
	وگرنه
	    ret = FAIL;
    }

    بازگشت ret;
}

/*
 * Check if the expected and actual types match.
 * Does not allow for assigning "any" to a specific type.
 * When "argidx" > 0 it is included in the error message.
 * Return OK if types match.
 * Return FAIL if types do not match.
 */
    صحیح
check_type(
	type_T	*expected,
	type_T	*actual,
	صحیح	give_پیام,
	where_T where)
{
    صحیح ret = check_type_maybe(expected, actual, give_پیام, where);

    بازگشت ret == MAYBE ? OK : ret;
}

/*
 * As check_type() but return MAYBE when a runtime type check should be used
 * when compiling.
 */
    صحیح
check_type_maybe(
	type_T	*expected,
	type_T	*actual,
	صحیح	give_پیام,
	where_T where)
{
    صحیح ret = OK;

    // When expected is "unknown" we accept any actual type.
    // When expected is "any" we accept any actual type except "void".
    اگر (expected->tt_type != VAR_UNKNOWN
	    && !(expected->tt_type == VAR_ANY && actual->tt_type != VAR_VOID))

    {
	// tt_type should match, except that a "partial" can be assigned to a
	// variable with type "func".
	// And "unknown" (using global variable) and "any" need a runtime type
	// check.
	اگر (!(expected->tt_type == actual->tt_type
		    || actual->tt_type == VAR_UNKNOWN
		    || actual->tt_type == VAR_ANY
		    || (expected->tt_type == VAR_FUNC
					   && actual->tt_type == VAR_PARTIAL)))
	{
	    اگر (expected->tt_type == VAR_BOOL
					&& (actual->tt_flags & TTFLAG_BOOL_OK))
		// Using number 0 or 1 for bool is OK.
		بازگشت OK;
	    اگر (expected->tt_type == VAR_FLOAT
		    && actual->tt_type == VAR_عدد
		    && ((expected->tt_flags & TTFLAG_عدد_OK)
			     || (actual->tt_flags & TTFLAG_FLOAT_OK)))
		// Using a number where a float is expected is OK here.
		بازگشت OK;
	    اگر (expected->tt_type == VAR_LIST
		    && actual->tt_type == VAR_TUPLE
		    && (expected->tt_flags & TTFLAG_TUPLE_OK))
		// Using a tuple where a list is expected is OK here.
		بازگشت OK;
	    اگر (give_پیام)
		type_mismatch_where(expected, actual, where);
	    بازگشت FAIL;
	}
	اگر (expected->tt_type == VAR_DICT || expected->tt_type == VAR_LIST)
	{
	    // "unknown" is used for an empty list or dict
	    اگر (actual->tt_member != پوچ && actual->tt_member != &t_unknown)
		ret = check_type_maybe(expected->tt_member, actual->tt_member,
								 FALSE, where);
	}
	وگرنه اگر (expected->tt_type == VAR_TUPLE && actual != &t_any)
	    ret =  check_tuple_type_maybe(expected, actual, where);
	وگرنه اگر (expected->tt_type == VAR_FUNC && actual != &t_any)
	    ret = check_func_type_maybe(expected, actual, where);
	وگرنه اگر (expected->tt_type == VAR_OBJECT)
	    ret = check_object_type_maybe(expected, actual, where);

	اگر (ret == FAIL && give_پیام)
	    type_mismatch_where(expected, actual, where);
    }

    اگر (ret == OK && expected->tt_type != VAR_UNKNOWN
	    && expected->tt_type != VAR_ANY
	    && (actual->tt_type == VAR_UNKNOWN || actual->tt_type == VAR_ANY))
	// check the type at runtime
	ret = MAYBE;

    بازگشت ret;
}

/*
 * Check that the arguments of "type" match "argvars[argcount]".
 * "base_tv" is from "expr->Func()".
 * Return OK/FAIL.
 */
    صحیح
check_argument_types(
	type_T	    *type,
	typval_T    *argvars,
	صحیح	    argcount,
	typval_T    *base_tv,
	char_u	    *name)
{
    صحیح	    varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;
    صحیح	    i;
    صحیح	    totcount = argcount + (base_tv == پوچ ? 0 : 1);

    اگر (type->tt_type != VAR_FUNC && type->tt_type != VAR_PARTIAL)
	بازگشت OK;  // just in case
    اگر (totcount < type->tt_min_argcount - varargs)
    {
	emsg_funcname(e_not_enough_arguments_for_function_str, name);
	بازگشت FAIL;
    }
    اگر (!varargs && type->tt_argcount >= 0 && totcount > type->tt_argcount)
    {
	emsg_funcname(e_too_many_arguments_for_function_str, name);
	بازگشت FAIL;
    }
    اگر (type->tt_args == پوچ)
	بازگشت OK;  // cannot check


    برای (i = 0; i < totcount; ++i)
    {
	type_T	    *expected;
	typval_T    *tv;

	اگر (base_tv != پوچ)
	{
	    اگر (i == 0)
		tv = base_tv;
	    وگرنه
		tv = &argvars[i - 1];
	}
	وگرنه
	    tv = &argvars[i];
	اگر (varargs && i >= type->tt_argcount - 1)
	{
	    expected = type->tt_args[type->tt_argcount - 1];
	    اگر (expected != پوچ && expected->tt_type == VAR_LIST)
		expected = expected->tt_member;
	    اگر (expected == پوچ)
		expected = &t_any;
	}
	وگرنه
	    expected = type->tt_args[i];

	// check the type, unless the value is v:none
	اگر ((tv->v_type != VAR_SPECIAL || tv->vval.v_عدد != VVAL_NONE)
		   && check_typval_arg_type(expected, tv, پوچ, i + 1) == FAIL)
	    بازگشت FAIL;
    }
    بازگشت OK;
}

// Maximum nesting of types, to avoid running out of stack space.  E.g.
// "list<list<number>>" is nested twice.
#define MAX_TYPE_DEPTH	1000

// Current number of levels of parse_type() and the deepest level reached while
// parsing the current type.  The nesting is one less than the number of
// levels.
ایستا صحیح	parse_depth = 0;
ایستا صحیح	parse_max_depth = 0;

/*
 * Skip over type in list<type>, dict<type> or tuple<type>.
 * Returns a pointer to the character after the type.  "syn_error" is set to
 * TRUE on syntax error.
 */
    ایستا char_u *
skip_member_type(char_u *start, char_u *p, صحیح *syn_error)
{
    اگر (STRNCMP("tuple", start, 5) == 0)
    {
	// handle tuple<{type1}, {type2}, ....<type>>
	p = skipwhite(p + 1);
	وقتی (*p != '>' && *p != NUL)
	{
	    char_u *sp = p;

	    اگر (STRNCMP(p, "...", 3) == 0)
		p += 3;
	    p = skip_type(p, TRUE);
	    اگر (p == sp)
	    {
		*syn_error = TRUE;
		بازگشت p;  // syntax error
	    }
	    اگر (*p == ',')
		p = skipwhite(p + 1);
	}
	اگر (*p == '>')
	    p++;
    }
    وگرنه
    {
	p = skipwhite(p);
	p = skip_type(skipwhite(p + 1), FALSE);
	p = skipwhite(p);
	اگر (*p == '>')
	    ++p;
    }

    بازگشت p;
}

/*
 * Skip over a function type.  Returns a pointer to the character after the
 * type.  "syn_error" is set to TRUE on syntax error.
 */
    ایستا char_u *
skip_func_type(char_u *p, صحیح *syn_error)
{
    اگر (*p == '(')
    {
	// handle func(args): type
	++p;
	وقتی (*p != ')' && *p != NUL)
	{
	    char_u *sp = p;

	    اگر (STRNCMP(p, "...", 3) == 0)
		p += 3;
	    p = skip_type(p, TRUE);
	    اگر (p == sp)
	    {
		*syn_error = TRUE;
		بازگشت p;  // syntax error
	    }
	    اگر (*p == ',')
		p = skipwhite(p + 1);
	}
	اگر (*p == ')')
	{
	    اگر (p[1] == ':')
		p = skip_type(skipwhite(p + 2), FALSE);
	    وگرنه
		++p;
	}
    }
    وگرنه
    {
	// handle func: return_type
	p = skip_type(skipwhite(p + 1), FALSE);
    }

    بازگشت p;
}

/*
 * Inner part of skip_type().
 */
    ایستا char_u *
skip_type_inner(char_u *start, صحیح optional)
{
    char_u	*p = start;
    صحیح		syn_error = FALSE;

    اگر (optional && *p == '?')
	++p;

    // Also skip over "." for imported classes: "import.ClassName".
    وقتی (ASCII_ISALNUM(*p) || *p == '_' || *p == '.')
	++p;

    // Skip over "<type>"; this is permissive about white space.
    اگر (*skipwhite(p) == '<')
    {
	p = skip_member_type(start, p, &syn_error);
	اگر (syn_error)
	    بازگشت p;
    }
    وگرنه اگر ((*p == '(' || (*p == ':' && VIM_ISWHITE(p[1])))
					     && STRNCMP("func", start, 4) == 0)
    {
	// skip over function type
	p = skip_func_type(p, &syn_error);
	اگر (syn_error)
	    بازگشت p;
    }

    بازگشت p;
}

/*
 * Skip over a type definition and return a pointer to just after it.
 * When "optional" is TRUE then a leading "?" is accepted.
 */
    char_u *
skip_type(char_u *start, صحیح optional)
{
    ایستا صحیح	skip_depth = 0;
    char_u	*p = start;

    // Too deeply nested: skip to the matching ">" without recursing, an
    // error is given when parsing the type.
    اگر (skip_depth > MAX_TYPE_DEPTH)
    {
	صحیح	depth = 0;

	برای ( ; *p != NUL; ++p)
	{
	    اگر (*p == '<')
		++depth;
	    وگرنه اگر (*p == '>')
	    {
		اگر (depth == 0)
		    بشکن;
		اگر (--depth == 0)
		    بازگشت p + 1;
	    }
	    وگرنه اگر (depth == 0 && !ASCII_ISALNUM(*p) && *p != '_'
								&& *p != '.')
		بشکن;
	}
	بازگشت p;
    }

    ++skip_depth;
    p = skip_type_inner(start, optional);
    --skip_depth;
    بازگشت p;
}

/*
 * Parse the member type: "<type>" and return "type" with the member set.
 * Use "type_gap" if a new type needs to be added.
 * "info" is extra information for an error message.
 * Returns NULL in case of failure.
 */
    ایستا type_T *
parse_type_member(
	char_u	    **arg,
	type_T	    *type,
	garray_T    *type_gap,
	صحیح	    give_error,
	نویسه	    *info,
	ufunc_T	    *ufunc,
	cctx_T	    *cctx)
{
    char_u  *arg_start = *arg;
    type_T  *member_type;
    صحیح	    prev_called_emsg = called_emsg;

    اگر (**arg != '<')
    {
	اگر (give_error)
	{
	    اگر (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    وگرنه
		semsg(_(e_missing_type_after_str), info);
	}
	بازگشت پوچ;
    }
    *arg = skipwhite(*arg + 1);

    member_type = parse_type(arg, type_gap, ufunc, cctx, give_error);
    اگر (member_type == پوچ || !valid_declaration_type(member_type))
	بازگشت پوچ;

    *arg = skipwhite(*arg);
    اگر (**arg != '>' && called_emsg == prev_called_emsg)
    {
	اگر (give_error)
	    semsg(_(e_missing_gt_after_type_str), arg_start);
	بازگشت پوچ;
    }
    ++*arg;

    اگر (type->tt_type == VAR_LIST)
	بازگشت get_list_type(member_type, type_gap);
    بازگشت get_dict_type(member_type, type_gap);
}

/*
 * Parse a "func" type at "*arg" and advance over it.
 * When "give_error" is TRUE give error messages, otherwise be quiet.
 * Return NULL for failure.
 */
    ایستا type_T *
parse_type_func(
    char_u	**arg,
    size_t	len,
    garray_T	*type_gap,
    صحیح		give_error,
    ufunc_T	*ufunc,
    cctx_T	*cctx)
{
    char_u  *p;
    type_T  *type;
    type_T  *ret_type = &t_unknown;
    صحیح	argcount = -1;
    صحیح	flags = 0;
    صحیح	first_optional = -1;
    type_T	*arg_type[MAX_FUNC_ARGS + 1];

    // func({type}, ...{type}): {type}
    *arg += len;
    اگر (**arg == '(')
    {
	// "func" may or may not return a value, "func()" does
	// not return a value.
	ret_type = &t_void;

	p = ++*arg;
	argcount = 0;
	وقتی (*p != NUL && *p != ')')
	{
	    اگر (*p == '?')
	    {
		اگر (first_optional == -1)
		    first_optional = argcount;
		++p;
	    }
	    وگرنه اگر (STRNCMP(p, "...", 3) == 0)
	    {
		flags |= TTFLAG_VARARGS;
		p += 3;
	    }
	    وگرنه اگر (first_optional != -1)
	    {
		اگر (give_error)
		    emsg(_(e_mandatory_argument_after_optional_argument));
		بازگشت پوچ;
	    }

	    type = parse_type(&p, type_gap, ufunc, cctx, give_error);
	    اگر (type == پوچ || !valid_declaration_type(type))
		بازگشت پوچ;
	    اگر ((flags & TTFLAG_VARARGS) != 0 && type->tt_type != VAR_LIST)
	    {
		نویسه *tofree;
		semsg(_(e_variable_arguments_type_must_be_list_str),
			type_name(type, &tofree));
		vim_free(tofree);
		بازگشت پوچ;
	    }
	    arg_type[argcount++] = type;

	    // Nothing comes after "...{type}".
	    اگر (flags & TTFLAG_VARARGS)
		بشکن;

	    اگر (*p != ',' && *skipwhite(p) == ',')
	    {
		اگر (give_error)
		    semsg(_(e_no_white_space_allowed_before_str_str),
			    ",", p);
		بازگشت پوچ;
	    }
	    اگر (*p == ',')
	    {
		++p;
		اگر (!VIM_ISWHITE(*p))
		{
		    اگر (give_error)
			semsg(_(e_white_space_required_after_str_str),
				",", p - 1);
		    بازگشت پوچ;
		}
	    }
	    p = skipwhite(p);
	    اگر (argcount == MAX_FUNC_ARGS)
	    {
		اگر (give_error)
		    emsg(_(e_too_many_argument_types));
		بازگشت پوچ;
	    }
	}

	p = skipwhite(p);
	اگر (*p != ')')
	{
	    اگر (give_error)
		emsg(_(e_missing_closing_paren));
	    بازگشت پوچ;
	}
	*arg = p + 1;
    }
    اگر (**arg == ':')
    {
	// parse return type
	++*arg;
	اگر (!VIM_ISWHITE(**arg) && give_error)
	    semsg(_(e_white_space_required_after_str_str), ":", *arg - 1);
	*arg = skipwhite(*arg);
	ret_type = parse_type(arg, type_gap, ufunc, cctx, give_error);
	اگر (ret_type == پوچ)
	    بازگشت پوچ;
    }
    اگر (flags == 0 && first_optional == -1 && argcount <= 0)
	type = get_func_type(ret_type, argcount, type_gap);
    وگرنه
    {
	type = alloc_func_type(ret_type, argcount, type_gap);
	type->tt_flags = flags;
	اگر (argcount > 0)
	{
	    type->tt_argcount = argcount;
	    type->tt_min_argcount = first_optional == -1
						? argcount : first_optional;
	    اگر (func_type_add_arg_types(type, argcount, type_gap) == FAIL)
		بازگشت پوچ;
	    mch_memmove(type->tt_args, arg_type, اندازه(type_T *) * argcount);
	}
    }

    بازگشت type;
}

/*
 * Parse a "tuple" type at "*arg" and advance over it.
 * When "give_error" is TRUE give error messages, otherwise be quiet.
 * Return NULL for failure.
 */
    ایستا type_T *
parse_type_tuple(
    char_u	**arg,
    garray_T	*type_gap,
    صحیح		give_error,
    ufunc_T	*ufunc,
    cctx_T	*cctx)
{
    char_u	*p;
    type_T	*type;
    type_T	*ret_type = پوچ;
    صحیح		typecount = -1;
    صحیح		flags = 0;
    garray_T	tuple_types_ga;

    ga_init2(&tuple_types_ga, اندازه(type_T *), 10);

    // tuple<{type}, {type}>
    // tuple<{type}, ...{type}>
    اگر (**arg != '<')
    {
	اگر (give_error)
	{
	    اگر (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    وگرنه
		semsg(_(e_missing_type_after_str), "tuple");
	}

	// only "tuple" is specified
	بازگشت پوچ;
    }

    p = ++*arg;
    typecount = 0;
    وقتی (*p != NUL && *p != '>')
    {
	اگر (STRNCMP(p, "...", 3) == 0)
	{
	    flags |= TTFLAG_VARARGS;
	    p += 3;
	}

	type = parse_type(&p, type_gap, ufunc, cctx, give_error);
	اگر (type == پوچ || !valid_declaration_type(type))
	    برو on_err;

	اگر ((flags & TTFLAG_VARARGS) != 0 && type->tt_type != VAR_LIST)
	{
	    نویسه *tofree;
	    semsg(_(e_variadic_tuple_must_end_with_list_type_str),
						type_name(type, &tofree));
	    vim_free(tofree);
	    برو on_err;
	}

	// Add the item type
	اگر (ga_grow(&tuple_types_ga, 1) == FAIL)
	    برو on_err;
	((type_T **)tuple_types_ga.ga_data)[tuple_types_ga.ga_len] = type;
	tuple_types_ga.ga_len++;
	typecount++;

	// Nothing comes after "...{type}".
	اگر (flags & TTFLAG_VARARGS)
	    بشکن;

	اگر (*p != ',' && *skipwhite(p) == ',')
	{
	    اگر (give_error)
		semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
	    برو on_err;
	}
	اگر (*p == ',')
	{
	    ++p;
	    اگر (!VIM_ISWHITE(*p))
	    {
		اگر (give_error)
		    semsg(_(e_white_space_required_after_str_str),
			    ",", p - 1);
		برو on_err;
	    }
	}
	p = skipwhite(p);
    }

    p = skipwhite(p);
    اگر (*p != '>' || typecount <= 0)
    {
	اگر (give_error)
	    semsg(_(e_missing_type_after_str), p);
	برو on_err;
    }
    *arg = p + 1;

    ret_type = alloc_tuple_type(typecount, type_gap);
    اگر (ret_type == &t_any
	    || tuple_type_add_types(ret_type, typecount, type_gap) == FAIL)
    {
	ret_type = پوچ;
	برو on_err;
    }
    ret_type->tt_flags = flags;
    ret_type->tt_argcount = typecount;
    mch_memmove(ret_type->tt_args, tuple_types_ga.ga_data,
						اندازه(type_T *) * typecount);

on_err:
    ga_clear(&tuple_types_ga);

    بازگشت ret_type;
}

/*
 * Parse a "object" type at "*arg" and advance over it.
 * When "give_error" is TRUE give error messages, otherwise be quiet.
 * Return NULL for failure.
 */
    ایستا type_T *
parse_type_object(
    char_u	**arg,
    garray_T	*type_gap,
    صحیح		give_error,
    cctx_T	*cctx)
{
    char_u	*arg_start = *arg;
    type_T	*object_type;
    صحیح		prev_called_emsg = called_emsg;

    // object<X> or object<any>
    اگر (**arg != '<')
    {
	اگر (give_error)
	{
	    اگر (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    وگرنه
		semsg(_(e_missing_type_after_str), "object");
	}

	// only "object" is specified
	بازگشت پوچ;
    }

    // skip spaces following "object<"
    *arg = skipwhite(*arg + 1);

    object_type = parse_type(arg, type_gap, پوچ, cctx, give_error);
    اگر (object_type == پوچ)
	بازگشت پوچ;

    *arg = skipwhite(*arg);
    اگر (**arg != '>' && called_emsg == prev_called_emsg)
    {
	اگر (give_error)
	    semsg(_(e_missing_gt_after_type_str), arg_start);
	بازگشت پوچ;
    }
    ++*arg;

    اگر (object_type->tt_type == VAR_ANY)
	بازگشت &t_object_any;

    اگر (object_type->tt_type != VAR_OBJECT)
    {
	// specified type is not a class
	اگر (give_error)
	    semsg(_(e_class_name_not_found_str), arg_start);
	بازگشت پوچ;
    }

    بازگشت object_type;
}

/*
 * Parse a user defined type at "*arg" and advance over it.
 * It can be a class or an interface or a typealias name, possibly imported.
 * Return NULL if a type is not found.
 */
    ایستا type_T *
parse_type_user_defined(
    char_u	**arg,
    size_t	len,
    garray_T	*type_gap,
    صحیح		give_error,
    ufunc_T	*ufunc,
    cctx_T	*cctx)
{
    صحیح		did_emsg_before = did_emsg;
    typval_T	tv;

    tv.v_type = VAR_UNKNOWN;
    اگر (eval_variable_import(*arg, &tv) == OK)
    {
	اگر (tv.v_type == VAR_CLASS && tv.vval.v_class != پوچ)
	{
	    type_T *type = get_type_ptr(type_gap);
	    اگر (type != پوچ)
	    {
		// Although the name is that of a class or interface, the type
		// uses will be an object.
		type->tt_type = VAR_OBJECT;
		type->tt_class = tv.vval.v_class;
		clear_tv(&tv);

		*arg += len;
		// Skip over ".ClassName".
		وقتی (ASCII_ISALNUM(**arg) || **arg == '_' || **arg == '.')
		    ++*arg;

		بازگشت type;
	    }
	}
	وگرنه اگر (tv.v_type == VAR_TYPEALIAS)
	{
	    // user defined type
	    // The type alias may itself be a nested type, check the resulting
	    // nesting depth.
	    صحیح	depth = parse_depth - 1
				+ MAX(tv.vval.v_typealias->ta_depth, 1);
	    اگر (depth - 1 > MAX_TYPE_DEPTH)
	    {
		clear_tv(&tv);
		اگر (give_error)
		    emsg(_(e_type_nested_too_deep));
		بازگشت پوچ;
	    }
	    اگر (depth > parse_max_depth)
		parse_max_depth = depth;

	    type_T *type = copy_type(tv.vval.v_typealias->ta_type, type_gap);
	    *arg += len;
	    clear_tv(&tv);
	    // Skip over ".TypeName".
	    وقتی (ASCII_ISALNUM(**arg) || **arg == '_' || **arg == '.')
		++*arg;
	    بازگشت type;
	}

	clear_tv(&tv);
    }

    // Check whether it is a generic type
    type_T *type = find_generic_type(*arg, len, ufunc, cctx);
    اگر (type != پوچ)
    {
	*arg += len;
	بازگشت type;
    }

    اگر (give_error && (did_emsg == did_emsg_before))
    {
	char_u	*p = skip_type(*arg, FALSE);
	نویسه	cc = *p;

	*p = NUL;
	semsg(_(e_type_not_recognized_str), *arg);
	*p = cc;
    }

    بازگشت پوچ;
}

/*
 * Inner part of parse_type().
 */
    ایستا type_T *
parse_type_inner(
    char_u	**arg,
    garray_T	*type_gap,
    ufunc_T	*ufunc,
    cctx_T	*cctx,
    صحیح		give_error)
{
    char_u  *p = *arg;
    size_t  len;

    // Skip over the first word.
    وقتی (ASCII_ISALNUM(*p) || *p == '_')
	++p;
    len = p - *arg;

    انتخاب (**arg)
    {
	حالت 'a':
	    اگر (len == 3 && STRNCMP(*arg, "any", len) == 0)
	    {
		*arg += len;
		بازگشت &t_any;
	    }
	    بشکن;
	حالت 'b':
	    اگر (len == 4 && STRNCMP(*arg, "bool", len) == 0)
	    {
		*arg += len;
		بازگشت &t_bool;
	    }
	    اگر (len == 4 && STRNCMP(*arg, "blob", len) == 0)
	    {
		*arg += len;
		بازگشت &t_blob;
	    }
	    بشکن;
	حالت 'c':
	    اگر (len == 7 && STRNCMP(*arg, "channel", len) == 0)
	    {
		*arg += len;
		بازگشت &t_channel;
	    }
	    بشکن;
	حالت 'd':
	    اگر (len == 4 && STRNCMP(*arg, "dict", len) == 0)
	    {
		*arg += len;
		بازگشت parse_type_member(arg, &t_dict_any, type_gap,
						give_error, "dict", ufunc,
						cctx);
	    }
	    بشکن;
	حالت 'f':
	    اگر (len == 5 && STRNCMP(*arg, "float", len) == 0)
	    {
		*arg += len;
		بازگشت &t_float;
	    }
	    اگر (len == 4 && STRNCMP(*arg, "func", len) == 0)
		بازگشت parse_type_func(arg, len, type_gap, give_error, ufunc,
									cctx);
	    بشکن;
	حالت 'j':
	    اگر (len == 3 && STRNCMP(*arg, "job", len) == 0)
	    {
		*arg += len;
		بازگشت &t_job;
	    }
	    بشکن;
	حالت 'l':
	    اگر (len == 4 && STRNCMP(*arg, "list", len) == 0)
	    {
		*arg += len;
		بازگشت parse_type_member(arg, &t_list_any, type_gap,
						give_error, "list", ufunc,
						cctx);
	    }
	    بشکن;
	حالت 'n':
	    اگر (len == 6 && STRNCMP(*arg, "number", len) == 0)
	    {
		*arg += len;
		بازگشت &t_عدد;
	    }
	    بشکن;
	حالت 'o':
	    اگر (len == 6 && STRNCMP(*arg, "object", len) == 0)
	    {
		*arg += len;
		بازگشت parse_type_object(arg, type_gap, give_error, cctx);
	    }
	    بشکن;
	حالت 's':
	    اگر (len == 6 && STRNCMP(*arg, "string", len) == 0)
	    {
		*arg += len;
		بازگشت &t_string;
	    }
	    بشکن;
	حالت 't':
	    اگر (len == 5 && STRNCMP(*arg, "tuple", len) == 0)
	    {
		*arg += len;
		بازگشت parse_type_tuple(arg, type_gap, give_error, ufunc,
									cctx);
	    }
	    بشکن;
	حالت 'v':
	    اگر (len == 4 && STRNCMP(*arg, "void", len) == 0)
	    {
		*arg += len;
		بازگشت &t_void;
	    }
	    بشکن;
    }

    // User defined type
    بازگشت parse_type_user_defined(arg, len, type_gap, give_error, ufunc,
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
    صحیح		give_error)
{
    type_T	*type;

    اگر (parse_depth > MAX_TYPE_DEPTH)
    {
	اگر (give_error)
	    emsg(_(e_type_nested_too_deep));
	بازگشت پوچ;
    }
    اگر (parse_depth == 0)
	parse_max_depth = 0;
    ++parse_depth;
    اگر (parse_depth > parse_max_depth)
	parse_max_depth = parse_depth;
    type = parse_type_inner(arg, type_gap, ufunc, cctx, give_error);
    --parse_depth;
    بازگشت type;
}

/*
 * Return the number of levels of the type last parsed by parse_type().
 */
    صحیح
parse_type_depth(تهی)
{
    بازگشت parse_max_depth;
}

/*
 * Check if "type1" and "type2" are exactly the same.
 * "flags" can have ETYPE_ARG_UNKNOWN, which means that an unknown argument
 * type in "type1" is accepted.
 */
    صحیح
equal_type(type_T *type1, type_T *type2, صحیح flags)
{
    صحیح i;

    اگر (type1 == پوچ || type2 == پوچ)
	بازگشت FALSE;
    اگر (type1->tt_type != type2->tt_type)
	بازگشت FALSE;
    انتخاب (type1->tt_type)
    {
	حالت VAR_UNKNOWN:
	حالت VAR_ANY:
	حالت VAR_VOID:
	حالت VAR_SPECIAL:
	حالت VAR_BOOL:
	حالت VAR_عدد:
	حالت VAR_FLOAT:
	حالت VAR_STRING:
	حالت VAR_BLOB:
	حالت VAR_JOB:
	حالت VAR_CHANNEL:
	حالت VAR_INSTR:
	حالت VAR_TYPEALIAS:
	    بشکن;  // not composite is always OK
	حالت VAR_OBJECT:
	حالت VAR_CLASS:
	    // Objects are considered equal if they are from the same class
	    بازگشت type1->tt_class == type2->tt_class;
	حالت VAR_LIST:
	حالت VAR_DICT:
	    بازگشت equal_type(type1->tt_member, type2->tt_member, flags);
	حالت VAR_TUPLE:
	    اگر (type1->tt_argcount != type2->tt_argcount)
		بازگشت FALSE;
	    اگر (type1->tt_argcount < 0
			   || type1->tt_args == پوچ || type2->tt_args == پوچ)
		بازگشت TRUE;
	    برای (i = 0; i < type1->tt_argcount; ++i)
		اگر ((flags & ETYPE_ARG_UNKNOWN) == 0
			&& !equal_type(type1->tt_args[i], type2->tt_args[i],
									flags))
		    بازگشت FALSE;
	    بازگشت TRUE;
	حالت VAR_FUNC:
	حالت VAR_PARTIAL:
	    اگر (!equal_type(type1->tt_member, type2->tt_member, flags)
		    || type1->tt_argcount != type2->tt_argcount)
		بازگشت FALSE;
	    اگر (type1->tt_argcount < 0
			   || type1->tt_args == پوچ || type2->tt_args == پوچ)
		بازگشت TRUE;
	    برای (i = 0; i < type1->tt_argcount; ++i)
		اگر ((flags & ETYPE_ARG_UNKNOWN) == 0
			&& !equal_type(type1->tt_args[i], type2->tt_args[i],
									flags))
		    بازگشت FALSE;
	    بازگشت TRUE;
    }
    بازگشت TRUE;
}

/*
 * Find the common type of "type1" (VAR_FUNC) and "type2" (VAR_FUNC) and put it
 * in "dest".  "type2" and "dest" may be the same.
 */
    ایستا تهی
common_type_var_func(
    type_T	*type1,
    type_T	*type2,
    type_T	**dest,
    garray_T	*type_gap)
{
    type_T *common;

    // When one of the types is t_func_unknown return the other one.
    // Useful if a list or dict item is null_func.
    اگر (type1 == &t_func_unknown)
    {
	*dest = type2;
	بازگشت;
    }
    اگر (type2 == &t_func_unknown)
    {
	*dest = type1;
	بازگشت;
    }

    common_type(type1->tt_member, type2->tt_member, &common, type_gap);
    اگر (type1->tt_argcount == type2->tt_argcount
	    && type1->tt_argcount >= 0)
    {
	صحیح argcount = type1->tt_argcount;
	صحیح i;

	*dest = alloc_func_type(common, argcount, type_gap);
	اگر (type1->tt_args != پوچ && type2->tt_args != پوچ)
	{
	    اگر (func_type_add_arg_types(*dest, argcount,
			type_gap) == OK)
		برای (i = 0; i < argcount; ++i)
		    common_type(type1->tt_args[i], type2->tt_args[i],
			    &(*dest)->tt_args[i], type_gap);
	}
    }
    وگرنه
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
    تهی
common_type(type_T *type1, type_T *type2, type_T **dest, garray_T *type_gap)
{
    اگر (equal_type(type1, type2, 0))
    {
	*dest = type1;
	بازگشت;
    }

    // If either is VAR_UNKNOWN use the other type.  An empty list/dict has no
    // specific type.
    اگر (type1 == پوچ || type1->tt_type == VAR_UNKNOWN)
    {
	*dest = type2;
	بازگشت;
    }
    اگر (type2 == پوچ || type2->tt_type == VAR_UNKNOWN)
    {
	*dest = type1;
	بازگشت;
    }

    اگر (type1->tt_type == type2->tt_type)
    {
	اگر (type1->tt_type == VAR_LIST
		|| type1->tt_type == VAR_DICT)
	{
	    type_T *common;

	    common_type(type1->tt_member, type2->tt_member, &common, type_gap);
	    اگر (type1->tt_type == VAR_LIST)
		*dest = get_list_type(common, type_gap);
	    وگرنه
		*dest = get_dict_type(common, type_gap);
	    بازگشت;
	}
	وگرنه اگر (type1->tt_type == VAR_FUNC)
	{
	    common_type_var_func(type1, type2, dest, type_gap);
	    بازگشت;
	}
	وگرنه اگر (type1->tt_type == VAR_OBJECT)
	{
	    *dest = &t_object_any;
	    بازگشت;
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
    اگر (type->tt_type == VAR_TUPLE)
    {
	اگر (type->tt_argcount != 1)
	    بازگشت &t_any;

	اگر (type->tt_flags & TTFLAG_VARARGS)
	    بازگشت type->tt_args[0]->tt_member;
	وگرنه
	    بازگشت type->tt_args[0];
    }

    // For "any" the item type is not known.
    بازگشت type->tt_member == پوچ ? &t_any : type->tt_member;
}

/*
 * Push an entry onto the type stack.  "type" used both for the current type
 * and the declared type.
 * Returns FAIL when out of memory.
 */
    صحیح
افزودن_type_پشته(cctx_T *cctx, type_T *type)
{
    بازگشت افزودن_type_stack2(cctx, type, type);
}

/*
 * Push an entry onto the type stack.  "type" is the current type, "decl_type"
 * is the declared type.
 * Returns FAIL when out of memory.
 */
    صحیح
افزودن_type_stack2(cctx_T *cctx, type_T *type, type_T *decl_type)
{
    garray_T	*پشته = &cctx->ctx_type_پشته;
    type2_T	*typep;

    اگر (GA_GROW_FAILS(پشته, 1))
	بازگشت FAIL;
    typep = ((type2_T *)پشته->ga_data) + پشته->ga_len;
    typep->type_curr = type;
    typep->type_decl = decl_type;
    ++پشته->ga_len;
    بازگشت OK;
}

/*
 * Set the type of the top of the stack to "type".
 */
    تهی
set_type_on_پشته(cctx_T *cctx, type_T *type, صحیح offset)
{
    garray_T	*پشته = &cctx->ctx_type_پشته;
    type2_T	*typep = ((type2_T *)پشته->ga_data)
						  + پشته->ga_len - 1 - offset;

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
get_type_on_پشته(cctx_T *cctx, صحیح offset)
{
    garray_T	*پشته = &cctx->ctx_type_پشته;

    اگر (offset + 1 > پشته->ga_len)
	بازگشت &t_unknown;
    بازگشت (((type2_T *)پشته->ga_data) + پشته->ga_len - offset - 1)
								   ->type_curr;
}

/*
 * Get the declared type from the type stack.  If "offset" is zero the one at
 * the top,
 * if "offset" is one the type above that, etc.
 * Returns &t_unknown if there is no such stack entry.
 */
    type_T *
get_decl_type_on_پشته(cctx_T *cctx, صحیح offset)
{
    garray_T	*پشته = &cctx->ctx_type_پشته;

    اگر (offset + 1 > پشته->ga_len)
	بازگشت &t_unknown;
    بازگشت (((type2_T *)پشته->ga_data) + پشته->ga_len - offset - 1)
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
get_member_type_from_پشته(
	صحیح	    شمار,
	صحیح	    skip,
	cctx_T	    *cctx)
{
    garray_T	*پشته = &cctx->ctx_type_پشته;
    type2_T	*typep;
    garray_T    *type_gap = cctx->ctx_type_list;
    صحیح		i;
    type_T	*result;
    type_T	*type;

    // Use "unknown" for an empty list or dict.
    اگر (شمار == 0)
	بازگشت &t_unknown;
    // Find the common type from following items.
    typep = ((type2_T *)پشته->ga_data) + پشته->ga_len;
    result = &t_unknown;
    برای (i = 0; i < شمار; ++i)
    {
	type = (typep -((شمار - i) * skip) + skip - 1)->type_curr;
	اگر (check_type_is_مقدار(type) == FAIL)
	    بازگشت پوچ;
	اگر (result != &t_any)
	    common_type(type, result, &result, type_gap);
    }

    بازگشت result;
}

/*
 * Get the types of items in a tuple on the stack of "cctx".
 * Returns the number of types.  Returns -1 on failure.
 */
    صحیح
get_tuple_type_from_پشته(
    صحیح		شمار,
    garray_T	*tuple_types_gap,
    cctx_T	*cctx)
{
    garray_T	*پشته = &cctx->ctx_type_پشته;
    type2_T	*typep;
    type_T	*type = پوچ;

    // Use "unknown" for an empty tuple
    اگر (شمار == 0)
	بازگشت 0;

    // Find the common type from following items.
    typep = ((type2_T *)پشته->ga_data) + پشته->ga_len;
    برای (صحیح i = 0; i < شمار; i++)
    {
	type = (typep - (شمار - i))->type_curr;
	اگر (check_type_is_مقدار(type) == FAIL)
	    بازگشت -1;
	اگر (ga_grow(tuple_types_gap, 1) == FAIL)
	    بازگشت -1;
	((type_T **)tuple_types_gap->ga_data)[tuple_types_gap->ga_len] = type;
	tuple_types_gap->ga_len++;
    }

    بازگشت tuple_types_gap->ga_len;
}

    نویسه *
vartype_name(vartype_T type)
{
    انتخاب (type)
    {
	حالت VAR_UNKNOWN: بشکن;
	حالت VAR_ANY: بازگشت "any";
	حالت VAR_VOID: بازگشت "void";
	حالت VAR_SPECIAL: بازگشت "special";
	حالت VAR_BOOL: بازگشت "bool";
	حالت VAR_عدد: بازگشت "number";
	حالت VAR_FLOAT: بازگشت "float";
	حالت VAR_STRING: بازگشت "string";
	حالت VAR_BLOB: بازگشت "blob";
	حالت VAR_JOB: بازگشت "job";
	حالت VAR_CHANNEL: بازگشت "channel";
	حالت VAR_LIST: بازگشت "list";
	حالت VAR_TUPLE: بازگشت "tuple";
	حالت VAR_DICT: بازگشت "dict";
	حالت VAR_INSTR: بازگشت "instr";
	حالت VAR_CLASS: بازگشت "class";
	حالت VAR_OBJECT: بازگشت "object";
	حالت VAR_TYPEALIAS: بازگشت "typealias";

	حالت VAR_FUNC:
	حالت VAR_PARTIAL: بازگشت "func";
    }
    بازگشت "unknown";
}

/*
 * Return the type name of a List (list<type>) or Dict (dict<type>).
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    ایستا نویسه *
type_name_list_or_dict(نویسه *name, type_T *type, نویسه **tofree)
{
    نویسه *member_free;
    نویسه *member_name;

    اگر (type->tt_member->tt_type == VAR_UNKNOWN)
	member_name = type_name(&t_any, &member_free);
    وگرنه
	member_name = type_name(type->tt_member, &member_free);

    size_t len = STRLEN(name) + STRLEN(member_name) + 3;
    *tofree = alloc(len);
    اگر (*tofree == پوچ)
    {
	vim_free(member_free);
	بازگشت name;
    }

    vim_snprintf(*tofree, len, "%s<%s>", name, member_name);
    vim_free(member_free);
    بازگشت *tofree;
}

/*
 * Return the type name of a tuple.
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    ایستا نویسه *
type_name_tuple(type_T *type, نویسه **tofree)
{
    garray_T    ga;
    صحیح		i;
    صحیح		varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;
    نویسه	*arg_free = پوچ;

    ga_init2(&ga, 1, 100);
    اگر (ga_grow(&ga, 20) == FAIL)
	برو failed;
    STRCPY(ga.ga_data, "tuple<");
    ga.ga_len += 6;

    اگر (type->tt_argcount <= 0)
	// empty tuple
	GA_CONCAT_LITERAL(&ga, "any");
    وگرنه
    {
	اگر (type->tt_args == پوچ)
	    GA_CONCAT_LITERAL(&ga, "[unknown]");
	وگرنه
	{
	    برای (i = 0; i < type->tt_argcount; ++i)
	    {
		string_T    arg_type;

		arg_type.string = (char_u *)type_name(type->tt_args[i], &arg_free);
		اگر (i > 0)
		{
		    STRCPY((نویسه *)ga.ga_data + ga.ga_len, ", ");
		    ga.ga_len += 2;
		}
		arg_type.length = STRLEN(arg_type.string);
		اگر (ga_grow(&ga, (صحیح)arg_type.length + 8) == FAIL)
		    برو failed;
		اگر (varargs && i == type->tt_argcount - 1)
		    GA_CONCAT_LITERAL(&ga, "...");
		ga_concat_len(&ga, arg_type.string, arg_type.length);
		VIM_CLEAR(arg_free);
	    }
	}
    }

    STRCPY((نویسه *)ga.ga_data + ga.ga_len, ">");
    *tofree = ga.ga_data;
    بازگشت ga.ga_data;

failed:
    vim_free(arg_free);
    ga_clear(&ga);
    بازگشت "[unknown]";
}

/*
 * Return the type name of a Class (class<name>) or Object (object<name>).
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    ایستا نویسه *
type_name_class_or_obj(نویسه *name, type_T *type, نویسه **tofree)
{
    string_T	class_name;

    اگر (type->tt_class != پوچ)
    {
	class_name.string = type->tt_class->class_name.string;
	class_name.length = type->tt_class->class_name.length;
	اگر (IS_ENUM(type->tt_class))
	    name = "enum";
    }
    وگرنه
	STR_LITERAL_SET(class_name, "any");

    size_t len = STRLEN(name) + class_name.length + 3;
    *tofree = alloc(len);
    اگر (*tofree == پوچ)
	بازگشت name;

    vim_snprintf(*tofree, len, "%s<%s>", name, class_name.string);
    بازگشت *tofree;
}

/*
 * Return the type name of a function.
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    ایستا نویسه *
type_name_func(type_T *type, نویسه **tofree)
{
    garray_T    ga;
    صحیح		i;
    صحیح		varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;
    نویسه	*arg_free = پوچ;

    ga_init2(&ga, 1, 100);
    اگر (ga_grow(&ga, 20) == FAIL)
	برو failed;
    STRCPY(ga.ga_data, "func(");
    ga.ga_len += 5;

    برای (i = 0; i < type->tt_argcount; ++i)
    {
	string_T    arg_type;

	اگر (type->tt_args == پوچ)
	    STR_LITERAL_SET(arg_type, "[unknown]");
	وگرنه
	{
	    arg_type.string = (char_u *)type_name(type->tt_args[i], &arg_free);
	    arg_type.length = STRLEN(arg_type.string);
	}
	اگر (i > 0)
	{
	    STRCPY((نویسه *)ga.ga_data + ga.ga_len, ", ");
	    ga.ga_len += 2;
	}
	اگر (ga_grow(&ga, (صحیح)arg_type.length + 8) == FAIL)
	    برو failed;
	اگر (varargs && i == type->tt_argcount - 1)
	    GA_CONCAT_LITERAL(&ga, "...");
	وگرنه اگر (i >= type->tt_min_argcount)
	    *((نویسه *)ga.ga_data + ga.ga_len++) = '?';
	ga_concat_len(&ga, arg_type.string, arg_type.length);
	VIM_CLEAR(arg_free);
    }
    اگر (type->tt_argcount < 0)
	// any number of arguments
	GA_CONCAT_LITERAL(&ga, "...");

    اگر (type->tt_member == &t_void)
	STRCPY((نویسه *)ga.ga_data + ga.ga_len, ")");
    وگرنه
    {
	نویسه *ret_free = پوچ;
	نویسه *ret_name = type_name(type->tt_member, &ret_free);
	صحیح  len;

	len = (صحیح)STRLEN(ret_name) + 4;
	اگر (ga_grow(&ga, len) == FAIL)
	{
	    vim_free(ret_free);
	    برو failed;
	}
	STRCPY((نویسه *)ga.ga_data + ga.ga_len, "): ");
	STRCPY((نویسه *)ga.ga_data + ga.ga_len + 3, ret_name);
	vim_free(ret_free);
    }
    *tofree = ga.ga_data;
    بازگشت ga.ga_data;

failed:
    vim_free(arg_free);
    ga_clear(&ga);
    بازگشت "[unknown]";
}

/*
 * Return the name of a type.
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    نویسه *
type_name(type_T *type, نویسه **tofree)
{
    نویسه *name;

    *tofree = پوچ;
    اگر (type == پوچ)
	بازگشت "[unknown]";
    name = vartype_name(type->tt_type);

    انتخاب (type->tt_type)
    {
	حالت VAR_LIST:
	حالت VAR_DICT:
	    بازگشت type_name_list_or_dict(name, type, tofree);

	حالت VAR_TUPLE:
	    بازگشت type_name_tuple(type, tofree);

	حالت VAR_CLASS:
	حالت VAR_OBJECT:
	    بازگشت type_name_class_or_obj(name, type, tofree);

	حالت VAR_FUNC:
	    بازگشت type_name_func(type, tofree);

	پیشفرض:
	    بشکن;
    }

    بازگشت name;
}

/*
 * "typename(expr)" function
 */
    تهی
f_typename(typval_T *argvars, typval_T *rettv)
{
    garray_T	type_list;
    type_T	*type;
    نویسه	*tofree;
    نویسه	*name;

    rettv->v_type = VAR_STRING;
    ga_init2(&type_list, اندازه(type_T *), 10);
    اگر (argvars[0].v_type == VAR_TYPEALIAS)
    {
	type = copy_type(argvars[0].vval.v_typealias->ta_type, &type_list);
	// A type alias for a class has the type set to VAR_OBJECT.  Change it
	// to VAR_CLASS, so that the name is "typealias<class<xxx>>"
	اگر (type->tt_type == VAR_OBJECT)
	    type->tt_type = VAR_CLASS;
    }
    وگرنه
	type = typval2type(argvars, get_copyID(), &type_list, TVTT_DO_MEMBER);
    name = type_name(type, &tofree);
    اگر (argvars[0].v_type == VAR_TYPEALIAS)
    {
	vim_snprintf((نویسه *)IObuff, IOSIZE, "typealias<%s>", name);
	rettv->vval.v_string = vim_strsave((char_u *)IObuff);
	اگر (tofree != پوچ)
	    vim_free(tofree);
    }
    وگرنه
    {
	اگر (tofree != پوچ)
	    rettv->vval.v_string = (char_u *)tofree;
	وگرنه
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
    صحیح
check_typval_is_مقدار(typval_T *tv)
{
    اگر (tv == پوچ)
	بازگشت OK;

    انتخاب (tv->v_type)
    {
	حالت VAR_CLASS:
	    {
		class_T *cl = tv->vval.v_class;
		char_u *class_name = (cl == پوچ) ? (char_u *)""
							: cl->class_name.string;
		اگر (cl != پوچ && IS_ENUM(cl))
		    semsg(_(e_using_enum_as_مقدار_str), class_name);
		وگرنه
		    semsg(_(e_using_class_as_مقدار_str), class_name);
	    }
	    بازگشت FAIL;

	حالت VAR_TYPEALIAS:
	    semsg(_(e_using_typealias_as_مقدار_str),
		    tv->vval.v_typealias->ta_name);
	    بازگشت FAIL;

	پیشفرض:
	    بشکن;
    }
    بازگشت OK;
}

/*
 * Same as above, except check type_T.
 */
    صحیح
check_type_is_مقدار(type_T *type)
{
    اگر (type == پوچ)
	بازگشت OK;
    انتخاب (type->tt_type)
    {
	حالت VAR_CLASS:
	    اگر (type->tt_class != پوچ && IS_ENUM(type->tt_class))
		semsg(_(e_using_enum_as_مقدار_str),
			type->tt_class->class_name.string);
	    وگرنه
		semsg(_(e_using_class_as_مقدار_str),
			type->tt_class == پوچ ? (char_u *)""
			: type->tt_class->class_name.string);
	    بازگشت FAIL;

	حالت VAR_TYPEALIAS:
	    // TODO: Not sure what could be done here to get a name.
	    //       Maybe an optional argument?
	    emsg(_(e_using_typealias_as_var_val));
	    بازگشت FAIL;

	پیشفرض:
	    بشکن;
    }
    بازگشت OK;
}

#endif // FEAT_EVAL
