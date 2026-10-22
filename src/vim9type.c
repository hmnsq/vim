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
#অন্তর্ভাব "vim.h"

#যদি defined(FEAT_EVAL)

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

    যদি (ga_grow(type_gap, 1) == FAIL)
	প্রত্যাগম রিক্ত;
    type = ALLOC_CLEAR_ONE(type_T);
    যদি (type == রিক্ত)
	প্রত্যাগম রিক্ত;

    ((type_T **)type_gap->ga_data)[type_gap->ga_len] = type;
    ++type_gap->ga_len;
    প্রত্যাগম type;
}

/*
 * Make a shallow copy of "type".
 * When allocation fails returns "type".
 */
    type_T *
copy_type(type_T *type, garray_T *type_gap)
{
    type_T *copy = get_type_ptr(type_gap);

    যদি (copy == রিক্ত)
	প্রত্যাগম type;
    *copy = *type;
    copy->tt_flags &= ~TTFLAG_STATIC;

    যদি (type->tt_args != রিক্ত
	   && func_type_add_arg_types(copy, type->tt_argcount, type_gap) == OK)
	প্রতি (পূর্ণাঙ্ক i = 0; i < type->tt_argcount; ++i)
	    copy->tt_args[i] = type->tt_args[i];

    প্রত্যাগম copy;
}

/*
 * Inner part of copy_type_deep().
 * When allocation fails returns "type".
 */
    স্থির type_T *
copy_type_deep_rec(type_T *type, garray_T *type_gap, garray_T *seen_types)
{
    প্রতি (পূর্ণাঙ্ক i = 0; i < seen_types->ga_len; ++i)
	যদি (((type_T **)seen_types->ga_data)[i * 2] == type)
	    // seen this type before, return the copy we made
	    প্রত্যাগম ((type_T **)seen_types->ga_data)[i * 2 + 1];

    type_T *copy = copy_type(type, type_gap);
    যদি (ga_grow(seen_types, 1) == FAIL)
	প্রত্যাগম copy;
    ((type_T **)seen_types->ga_data)[seen_types->ga_len * 2] = type;
    ((type_T **)seen_types->ga_data)[seen_types->ga_len * 2 + 1] = copy;
    ++seen_types->ga_len;

    যদি (copy->tt_member != রিক্ত)
	copy->tt_member = copy_type_deep_rec(copy->tt_member,
							 type_gap, seen_types);
    যদি (type->tt_args != রিক্ত)
	প্রতি (পূর্ণাঙ্ক i = 0; i < type->tt_argcount; ++i)
	    copy->tt_args[i] = copy_type_deep_rec(copy->tt_args[i],
							 type_gap, seen_types);

    প্রত্যাগম copy;
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
    ga_init2(&seen_types, পরিমাণ(type_T *) * 2, 20);

    type_T *res = copy_type_deep_rec(type, type_gap, &seen_types);

    ga_clear(&seen_types);
    প্রত্যাগম res;
}

    শূন্য
clear_type_list(garray_T *gap)
{
    যাবত্ (gap->ga_len > 0)
	vim_free(((type_T **)gap->ga_data)[--gap->ga_len]);
    ga_clear(gap);
}

    শূন্য
clear_func_type_list(garray_T *gap, type_T **func_type)
{
    যাবত্ (gap->ga_len > 0)
    {
	// func_type pointing to the uf_type_list, so reset pointer
	যদি (*func_type == ((type_T **)gap->ga_data)[--gap->ga_len])
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

    যদি (type == রিক্ত)
	প্রত্যাগম রিক্ত;

    // A fixed type never contains allocated types, return as-is.
    যদি (type->tt_flags & TTFLAG_STATIC)
	প্রত্যাগম type;

    ret = ALLOC_ONE(type_T);
    যদি (ret == রিক্ত)
	প্রত্যাগম রিক্ত;
    *ret = *type;

    যদি (ret->tt_member != রিক্ত)
	ret->tt_member = alloc_type(ret->tt_member);

    যদি (type->tt_argcount > 0 && type->tt_args != রিক্ত)
    {
	পূর্ণাঙ্ক i;

	ret->tt_args = ALLOC_MULT(type_T *, type->tt_argcount);
	যদি (ret->tt_args != রিক্ত)
	    প্রতি (i = 0; i < type->tt_argcount; ++i)
		ret->tt_args[i] = alloc_type(type->tt_args[i]);
    }
    অন্যথা
	ret->tt_args = রিক্ত;

    প্রত্যাগম ret;
}

/*
 * Free a type that was created with alloc_type().
 */
    শূন্য
free_type(type_T *type)
{
    পূর্ণাঙ্ক i;

    যদি (type == রিক্ত || (type->tt_flags & TTFLAG_STATIC))
	প্রত্যাগম;
    যদি (type->tt_args != রিক্ত)
    {
	প্রতি (i = 0; i < type->tt_argcount; ++i)
	    free_type(type->tt_args[i]);
	vim_free(type->tt_args);
    }

    free_type(type->tt_member);

    vim_free(type);
}

/*
 * Return TRUE if "type" is to be recursed into for setting the type.
 */
    স্থির পূর্ণাঙ্ক
set_tv_type_recurse(type_T *type)
{
    প্রত্যাগম type->tt_member != রিক্ত
		&& (type->tt_member->tt_type == VAR_DICT
				|| type->tt_member->tt_type == VAR_LIST)
		&& type->tt_member->tt_member != রিক্ত
		&& type->tt_member->tt_member != &t_any
		&& type->tt_member->tt_member != &t_unknown;
}

/*
 * Set the type of Dict "d" to "type"
 */
    স্থির শূন্য
set_tv_type_dict(dict_T *d, type_T *type)
{
    যদি (d->dv_type == type)
	প্রত্যাগম;

    free_type(d->dv_type);
    d->dv_type = alloc_type(type);

    // Need to recursively set the type of dict items?
    যদি (!set_tv_type_recurse(type))
	প্রত্যাগম;

    পূর্ণাঙ্ক		todo = (পূর্ণাঙ্ক)d->dv_hashtab.ht_used;
    hashitem_T	*hi;
    dictitem_T	*di;

    FOR_ALL_HASHTAB_বস্তুসমূহ(&d->dv_hashtab, hi, todo)
    {
	যদি (!HASHITEM_EMPTY(hi))
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
    স্থির শূন্য
set_tv_type_list(list_T *l, type_T *type)
{
    যদি (l->lv_type == type)
	প্রত্যাগম;

    free_type(l->lv_type);
    l->lv_type = alloc_type(type);

    // Need to recursively set the type of list items?
    যদি (l->lv_first == &range_list_item || !set_tv_type_recurse(type))
	প্রত্যাগম;

    listitem_T	*li;

    FOR_ALL_LIST_বস্তুসমূহ(l, li)
	set_tv_type(&li->li_tv, type->tt_member);
}

/*
 * Set the type of Tuple "tuple" to "type"
 */
    স্থির শূন্য
set_tv_type_tuple(tuple_T *tuple, type_T *type)
{
    যদি (tuple->tv_type == type)
	প্রত্যাগম;

    free_type(tuple->tv_type);
    tuple->tv_type = alloc_type(type);

    যদি (type->tt_argcount <= 0)
	প্রত্যাগম;

    // recursively set the type of list items
    type_T	*item_type;
    প্রতি (পূর্ণাঙ্ক i = 0; i < tuple_len(tuple); i++)
    {
	যদি ((type->tt_flags & TTFLAG_VARARGS) && (i >= type->tt_argcount - 1))
	    // For a variadic tuple, the last type is a List.  So use the
	    // List member type.
	    item_type = type->tt_args[type->tt_argcount - 1]->tt_member;
	অন্যথা
	    item_type = type->tt_args[i];

	set_tv_type(TUPLE_ITEM(tuple, i), item_type);
    }
}

/*
 * Set the type of "tv" to "type" if it is a list or tuple or dict.
 */
    শূন্য
set_tv_type(typval_T *tv, type_T *type)
{
    যদি (type->tt_type == VAR_ANY)
	// If the variable type is "any", then keep the value type.
	// e.g.  var x: any = [1, 2] or var y: any = {v: 1}
	প্রত্যাগম;

    যদি (tv->v_type == VAR_DICT && tv->vval.v_dict != রিক্ত)
	set_tv_type_dict(tv->vval.v_dict, type);
    অন্যথা যদি (tv->v_type == VAR_LIST && tv->vval.v_list != রিক্ত)
	set_tv_type_list(tv->vval.v_list, type);
    অন্যথা যদি (tv->v_type == VAR_TUPLE && tv->vval.v_tuple != রিক্ত)
	set_tv_type_tuple(tv->vval.v_tuple, type);
}

/*
 * For a tuple type, reserve space for "typecount" types (including the
 * repeated type).
 */
    স্থির পূর্ণাঙ্ক
tuple_type_add_types(
    type_T	*tupletype,
    পূর্ণাঙ্ক		typecount,
    garray_T	*type_gap)
{
    // To make it easy to free the space needed for the types, add the
    // pointer to type_gap.
    যদি (ga_grow(type_gap, 1) == FAIL)
	প্রত্যাগম FAIL;
    tupletype->tt_args = alloc_clear_id(পরিমাণ(type_T *) * typecount,
							  aid_tuple_type_args);
    যদি (tupletype->tt_args == রিক্ত)
	প্রত্যাগম FAIL;
    ((type_T **)type_gap->ga_data)[type_gap->ga_len] =
						(শূন্য *)tupletype->tt_args;
    ++type_gap->ga_len;
    প্রত্যাগম OK;
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
    যদি (member_type == রিক্ত || (member_type->tt_type == VAR_ANY
					&& !IS_GENERIC_TYPE(member_type)))
	প্রত্যাগম &t_list_any;
    যদি (member_type->tt_type == VAR_VOID
	    || member_type->tt_type == VAR_UNKNOWN)
	প্রত্যাগম &t_list_empty;
    যদি (member_type->tt_type == VAR_BOOL)
	প্রত্যাগম &t_list_bool;
    যদি (member_type->tt_type == VAR_সংখ্যা)
	প্রত্যাগম &t_list_সংখ্যা;
    যদি (member_type->tt_type == VAR_STRING)
	প্রত্যাগম &t_list_string;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    যদি (type == রিক্ত)
	প্রত্যাগম &t_any;
    type->tt_type = VAR_LIST;
    type->tt_member = member_type;
    type->tt_argcount = 0;
    type->tt_args = রিক্ত;
    প্রত্যাগম type;
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
    পূর্ণাঙ্ক		typecount = tuple_types_gap->ga_len;

    // recognize commonly used types
    যদি (typecount == 0)
	প্রত্যাগম &t_tuple_any;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    যদি (type == রিক্ত)
	প্রত্যাগম &t_any;
    type->tt_type = VAR_TUPLE;
    type->tt_member = রিক্ত;
    যদি (typecount > 0)
    {
	যদি (tuple_type_add_types(type, typecount, type_gap) == FAIL)
	    প্রত্যাগম &t_any;
	mch_memmove(type->tt_args, tuple_types, পরিমাণ(type_T *) * typecount);
    }
    type->tt_argcount = typecount;
    type->tt_flags = 0;

    প্রত্যাগম type;
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
    যদি (member_type == রিক্ত || (member_type->tt_type == VAR_ANY
					&& !IS_GENERIC_TYPE(member_type)))
	প্রত্যাগম &t_dict_any;
    যদি (member_type->tt_type == VAR_VOID
	    || member_type->tt_type == VAR_UNKNOWN)
	প্রত্যাগম &t_dict_empty;
    যদি (member_type->tt_type == VAR_BOOL)
	প্রত্যাগম &t_dict_bool;
    যদি (member_type->tt_type == VAR_সংখ্যা)
	প্রত্যাগম &t_dict_সংখ্যা;
    যদি (member_type->tt_type == VAR_STRING)
	প্রত্যাগম &t_dict_string;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    যদি (type == রিক্ত)
	প্রত্যাগম &t_any;
    type->tt_type = VAR_DICT;
    type->tt_member = member_type;
    type->tt_argcount = 0;
    type->tt_args = রিক্ত;
    প্রত্যাগম type;
}

/*
 * Allocate a new type for a function.
 */
    type_T *
alloc_func_type(type_T *ret_type, পূর্ণাঙ্ক argcount, garray_T *type_gap)
{
    type_T *type = get_type_ptr(type_gap);

    যদি (type == রিক্ত)
	প্রত্যাগম &t_any;
    type->tt_type = VAR_FUNC;
    type->tt_member = ret_type == রিক্ত ? &t_unknown : ret_type;
    type->tt_argcount = argcount;
    type->tt_args = রিক্ত;
    প্রত্যাগম type;
}

/*
 * Allocate a new type for a tuple.
 */
    স্থির type_T *
alloc_tuple_type(পূর্ণাঙ্ক typecount, garray_T *type_gap)
{
    type_T *type = get_type_ptr(type_gap);

    যদি (type == রিক্ত)
	প্রত্যাগম &t_any;
    type->tt_type = VAR_TUPLE;
    type->tt_member = রিক্ত;
    type->tt_argcount = typecount;
    type->tt_args = রিক্ত;
    প্রত্যাগম type;
}

/*
 * Get a function type, based on the return type "ret_type".
 * "argcount" must be -1 or 0, a predefined type can be used.
 */
    type_T *
get_func_type(type_T *ret_type, পূর্ণাঙ্ক argcount, garray_T *type_gap)
{
    // recognize commonly used types
    যদি (ret_type == &t_unknown || ret_type == রিক্ত)
    {
	// (argcount == 0) is not possible
	প্রত্যাগম &t_func_unknown;
    }
    যদি (ret_type == &t_void)
    {
	যদি (argcount == 0)
	    প্রত্যাগম &t_func_0_void;
	অন্যথা
	    প্রত্যাগম &t_func_void;
    }
    যদি (ret_type == &t_any)
    {
	যদি (argcount == 0)
	    প্রত্যাগম &t_func_0_any;
	অন্যথা
	    প্রত্যাগম &t_func_any;
    }
    যদি (ret_type == &t_সংখ্যা)
    {
	যদি (argcount == 0)
	    প্রত্যাগম &t_func_0_সংখ্যা;
	অন্যথা
	    প্রত্যাগম &t_func_সংখ্যা;
    }
    যদি (ret_type == &t_string)
    {
	যদি (argcount == 0)
	    প্রত্যাগম &t_func_0_string;
	অন্যথা
	    প্রত্যাগম &t_func_string;
    }

    প্রত্যাগম alloc_func_type(ret_type, argcount, type_gap);
}

/*
 * For a function type, reserve space for "argcount" argument types (including
 * vararg).
 */
    পূর্ণাঙ্ক
func_type_add_arg_types(
	type_T	    *functype,
	পূর্ণাঙ্ক	    argcount,
	garray_T    *type_gap)
{
    // To make it easy to free the space needed for the argument types, add the
    // pointer to type_gap.
    যদি (ga_grow(type_gap, 1) == FAIL)
	প্রত্যাগম FAIL;
    functype->tt_args = ALLOC_CLEAR_MULT(type_T *, argcount);
    যদি (functype->tt_args == রিক্ত)
	প্রত্যাগম FAIL;
    ((type_T **)type_gap->ga_data)[type_gap->ga_len] =
						     (শূন্য *)functype->tt_args;
    ++type_gap->ga_len;
    প্রত্যাগম OK;
}

/*
 * Return TRUE if "type" is NULL, any or unknown.
 * This also works for const (comparing with &t_any and &t_unknown doesn't).
 */
    পূর্ণাঙ্ক
type_any_or_unknown(type_T *type)
{
    প্রত্যাগম type == রিক্ত || type->tt_type == VAR_ANY
					|| type->tt_type == VAR_UNKNOWN;
}

/*
 * Get a type_T for a "special" typval in "tv".
 */
    স্থির type_T *
special_typval2type(typval_T *tv)
{
    বিকল্প (tv->vval.v_সংখ্যা)
    {
	পক্ষ VVAL_NULL:
	    প্রত্যাগম &t_null;

	পক্ষ VVAL_NONE:
	    প্রত্যাগম &t_none;

	পক্ষ VVAL_TRUE:
	পক্ষ VVAL_FALSE:
	    প্রত্যাগম &t_bool;

	স্বভাব:
	    প্রত্যাগম &t_unknown;
    }
}

/*
 * Get a type_T for a List typval in "tv".
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    স্থির type_T *
list_typval2type(typval_T *tv, পূর্ণাঙ্ক copyID, garray_T *type_gap, পূর্ণাঙ্ক flags)
{
    list_T	*l = tv->vval.v_list;
    listitem_T	*li;
    type_T	*member_type = রিক্ত;

    // An empty list has type list<unknown>, unless the type was specified
    // and is not list<any>.  This matters when assigning to a variable
    // with a specific list type.
    যদি (l == রিক্ত || (l->lv_first == রিক্ত
		&& (l->lv_type == রিক্ত || l->lv_type->tt_member == &t_any)))
	প্রত্যাগম &t_list_empty;

    যদি ((flags & TVTT_DO_MEMBER) == 0)
	প্রত্যাগম &t_list_any;

    // If the type is list<any> go through the members, it may end up a
    // more specific type.
    যদি (l->lv_type != রিক্ত && (l->lv_first == রিক্ত
		|| (flags & TVTT_MORE_SPECIFIC) == 0
		|| l->lv_type->tt_member != &t_any))
	// make a copy, lv_type may be freed if the list is freed
	প্রত্যাগম copy_type_deep(l->lv_type, type_gap);

    যদি (l->lv_first == &range_list_item)
	প্রত্যাগম &t_list_সংখ্যা;

    যদি (l->lv_copyID == copyID)
	// avoid recursion
	প্রত্যাগম &t_list_any;

    l->lv_copyID = copyID;

    // Use the common type of all members.
    member_type = typval2type(&l->lv_first->li_tv, copyID, type_gap,
							TVTT_DO_MEMBER);
    প্রতি (li = l->lv_first->li_next; li != রিক্ত; li = li->li_next)
	common_type(typval2type(&li->li_tv, copyID, type_gap, TVTT_DO_MEMBER),
					member_type, &member_type, type_gap);

    // Reset copyID so that a shared reference to this list (not a circular
    // reference) can be processed again to get the correct type.
    l->lv_copyID = 0;

    প্রত্যাগম get_list_type(member_type, type_gap);
}

/*
 * Get a type_T for a Tuple typval in "tv".
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    স্থির type_T *
tuple_typval2type(typval_T *tv, পূর্ণাঙ্ক copyID, garray_T *type_gap, পূর্ণাঙ্ক flags)
{
    tuple_T	*tuple = tv->vval.v_tuple;
    পূর্ণাঙ্ক		len = tuple_len(tuple);
    type_T	*type = রিক্ত;

    // An empty tuple has type tuple<unknown>, unless the type was specified
    // and is not tuple<any>.  This matters when assigning to a variable
    // with a specific tuple type.
    যদি (tuple == রিক্ত || (len == 0 && (tuple->tv_type == রিক্ত
				|| tuple->tv_type->tt_argcount == 0)))
	প্রত্যাগম &t_tuple_empty;

    যদি ((flags & TVTT_DO_MEMBER) == 0)
	প্রত্যাগম &t_tuple_any;

    // If the type is tuple<any> go through the members, it may end up a
    // more specific type.
    যদি (tuple->tv_type != রিক্ত && (len == 0
					|| (flags & TVTT_MORE_SPECIFIC) == 0))
	// make a copy, tv_type may be freed if the tuple is freed
	প্রত্যাগম copy_type_deep(tuple->tv_type, type_gap);

    যদি (tuple->tv_copyID == copyID)
	// avoid recursion
	প্রত্যাগম &t_tuple_any;

    tuple->tv_copyID = copyID;

    garray_T	tuple_types_ga;
    ga_init2(&tuple_types_ga, পরিমাণ(type_T *), 10);
    প্রতি (পূর্ণাঙ্ক i = 0; i < len; i++)
    {
	type = typval2type(TUPLE_ITEM(tuple, i), copyID, type_gap,
							TVTT_DO_MEMBER);
	যদি (ga_grow(&tuple_types_ga, 1) == FAIL)
	{
	    ga_clear(&tuple_types_ga);
	    // Reset copyID so that a shared reference to this tuple can be
	    // processed again.
	    tuple->tv_copyID = 0;
	    প্রত্যাগম রিক্ত;
	}
	((type_T **)tuple_types_ga.ga_data)[tuple_types_ga.ga_len] = type;
	tuple_types_ga.ga_len++;
    }

    type_T *tuple_type = get_tuple_type(&tuple_types_ga, type_gap);
    ga_clear(&tuple_types_ga);

    // Reset copyID so that a shared reference to this tuple (not a circular
    // reference) can be processed again to get the correct type.
    tuple->tv_copyID = 0;

    প্রত্যাগম tuple_type;
}

/*
 * Get a type_T for a Dict typval in "tv".
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    স্থির type_T *
dict_typval2type(typval_T *tv, পূর্ণাঙ্ক copyID, garray_T *type_gap, পূর্ণাঙ্ক flags)
{
    dict_iterator_T	iter;
    typval_T		*মান;
    dict_T		*d = tv->vval.v_dict;
    type_T		*member_type = রিক্ত;

    যদি (d == রিক্ত || (d->dv_hashtab.ht_used == 0 && d->dv_type == রিক্ত))
	প্রত্যাগম &t_dict_empty;

    যদি ((flags & TVTT_DO_MEMBER) == 0)
	প্রত্যাগম &t_dict_any;

    // If the type is dict<any> go through the members, it may end up a
    // more specific type.
    যদি (d->dv_type != রিক্ত && (d->dv_hashtab.ht_used == 0
		|| (flags & TVTT_MORE_SPECIFIC) == 0
		|| d->dv_type->tt_member != &t_any))
	প্রত্যাগম d->dv_type;

    যদি (d->dv_copyID == copyID)
	// avoid recursion
	প্রত্যাগম &t_dict_any;

    d->dv_copyID = copyID;

    // Use the common type of all values.
    dict_iterate_start(tv, &iter);
    dict_iterate_next(&iter, &মান);
    member_type = typval2type(মান, copyID, type_gap, TVTT_DO_MEMBER);

    যাবত্ (dict_iterate_next(&iter, &মান) != রিক্ত)
	common_type(typval2type(মান, copyID, type_gap, TVTT_DO_MEMBER),
					member_type, &member_type, type_gap);

    // Reset copyID so that a shared reference to this dict (not a circular
    // reference) can be processed again to get the correct type.
    d->dv_copyID = 0;

    প্রত্যাগম get_dict_type(member_type, type_gap);
}

/*
 * Get a type_T for a "partial" typval in "tv".
 */
    স্থির type_T *
partial_typval2type(typval_T *tv, ufunc_T *ufunc, garray_T *type_gap)
{
    partial_T *pt = tv->vval.v_partial;
    type_T  *type;

    type = get_type_ptr(type_gap);
    যদি (type == রিক্ত)
	প্রত্যাগম রিক্ত;

    *type = *ufunc->uf_func_type;
    যদি (type->tt_argcount >= 0 && pt->pt_argc > 0)
    {
	type->tt_argcount -= pt->pt_argc;
	type->tt_min_argcount -= pt->pt_argc;
	যদি (type->tt_argcount > 0 && func_type_add_arg_types(type,
		    type->tt_argcount, type_gap) == OK)
	    প্রতি (পূর্ণাঙ্ক i = 0; i < type->tt_argcount; ++i)
		type->tt_args[i] =
		    ufunc->uf_func_type->tt_args[i + pt->pt_argc];
    }
    যদি (pt->pt_func != রিক্ত)
	type->tt_member = pt->pt_func->uf_ret_type;

    প্রত্যাগম type;
}

/*
 * Get a type_T for a "class" or an "object" typval in "tv".
 */
    স্থির type_T *
oc_typval2type(typval_T *tv)
{
    যদি (tv->v_type == VAR_CLASS)
    {
	যদি (tv->vval.v_class == রিক্ত)
	    প্রত্যাগম &t_class;

	প্রত্যাগম &tv->vval.v_class->class_type;
    }

    যদি (tv->vval.v_object != রিক্ত)
	প্রত্যাগম &tv->vval.v_object->obj_class->class_object_type;

    প্রত্যাগম &t_object_any;
}

/*
 * Get a type_T for a "function" or a "partial"
 */
    স্থির type_T *
fp_typval2type(typval_T *tv, garray_T *type_gap)
{
    char_u	*name = রিক্ত;
    ufunc_T	*ufunc = রিক্ত;
    type_T	*type;
    type_T	*member_type = রিক্ত;
    পূর্ণাঙ্ক		argcount = 0;
    পূর্ণাঙ্ক		min_argcount = 0;

    যদি (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != রিক্ত)
    {
	যদি (tv->vval.v_partial->pt_func != রিক্ত)
	    ufunc = tv->vval.v_partial->pt_func;
	অন্যথা
	    name = tv->vval.v_partial->pt_name;
    }
    অন্যথা
	name = tv->vval.v_string;

    যদি (name == রিক্ত && ufunc == রিক্ত)
	প্রত্যাগম &t_func_unknown;

    যদি (name != রিক্ত)
    {
	পূর্ণাঙ্ক idx = find_internal_func(name);

	যদি (idx >= 0)
	{
	    type_T *decl_type;  // unused

	    internal_func_get_argcount(idx, &argcount, &min_argcount);
	    member_type = internal_func_ret_type(idx, 0, রিক্ত, &decl_type,
		    type_gap);
	}
	অন্যথা
	{
	    // Check if name contains "<".  If it does, then replace "<" with
	    // NUL and lookup the function and then look up the specific
	    // generic function using the supplied types.
	    char_u *p = generic_func_find_open_bracket(name);
	    যদি (p == রিক্ত)
		ufunc = find_func(name, FALSE);
	    অন্যথা
	    {
		// generic function
		char_u	cc = *p;

		*p = NUL;
		ufunc = find_func(name, FALSE);
		*p = cc;
		যদি (ufunc != রিক্ত && IS_GENERIC_FUNC(ufunc))
		{
		    ufunc = find_generic_func(ufunc, name, &p);
		    যদি (ufunc == রিক্ত)
			প্রত্যাগম রিক্ত;
		}
	    }
	}
    }
    যদি (ufunc != রিক্ত)
    {
	// May need to get the argument types from default values by
	// compiling the function.
	যদি (ufunc->uf_def_status == UF_TO_BE_COMPILED
		&& compile_def_function(ufunc, TRUE, CT_NONE, রিক্ত)
		== FAIL)
	    প্রত্যাগম রিক্ত;
	যদি (ufunc->uf_func_type == রিক্ত)
	    set_function_type(ufunc);
	যদি (ufunc->uf_func_type != রিক্ত)
	{
	    যদি (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != রিক্ত)
		প্রত্যাগম partial_typval2type(tv, ufunc, type_gap);
	    প্রত্যাগম ufunc->uf_func_type;
	}
    }

    type = get_type_ptr(type_gap);
    যদি (type == রিক্ত)
	প্রত্যাগম রিক্ত;
    type->tt_type = tv->v_type;
    type->tt_argcount = argcount;
    type->tt_min_argcount = min_argcount;
    যদি (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != রিক্ত
					&& tv->vval.v_partial->pt_argc > 0)
    {
	type->tt_argcount -= tv->vval.v_partial->pt_argc;
	type->tt_min_argcount -= tv->vval.v_partial->pt_argc;
    }
    type->tt_member = member_type;

    প্রত্যাগম type;
}

/*
 * Get a type_T for a typval_T.
 * "type_gap" is used to temporarily create types in.
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    স্থির type_T *
typval2type_int(typval_T *tv, পূর্ণাঙ্ক copyID, garray_T *type_gap, পূর্ণাঙ্ক flags)
{
    বিকল্প (tv->v_type)
    {
	পক্ষ VAR_UNKNOWN:
	    প্রত্যাগম &t_unknown;

	পক্ষ VAR_ANY:
	    প্রত্যাগম &t_any;

	পক্ষ VAR_VOID:
	    প্রত্যাগম &t_void;

	পক্ষ VAR_BOOL:
	    প্রত্যাগম &t_bool;

	পক্ষ VAR_SPECIAL:
	    প্রত্যাগম special_typval2type(tv);

	পক্ষ VAR_সংখ্যা:
	    প্রত্যাগম &t_সংখ্যা;

	পক্ষ VAR_FLOAT:
	    প্রত্যাগম &t_float;

	পক্ষ VAR_STRING:
	    প্রত্যাগম &t_string;

	পক্ষ VAR_BLOB:
	    যদি (tv->vval.v_blob == রিক্ত)
		প্রত্যাগম &t_blob_null;
	    প্রত্যাগম &t_blob;

	পক্ষ VAR_LIST:
	    প্রত্যাগম list_typval2type(tv, copyID, type_gap, flags);

	পক্ষ VAR_TUPLE:
	    প্রত্যাগম tuple_typval2type(tv, copyID, type_gap, flags);

	পক্ষ VAR_DICT:
	    প্রত্যাগম dict_typval2type(tv, copyID, type_gap, flags);

	পক্ষ VAR_JOB:
	    প্রত্যাগম &t_job;

	পক্ষ VAR_CHANNEL:
	    প্রত্যাগম &t_channel;

	পক্ষ VAR_CLASS:
	পক্ষ VAR_OBJECT:
	    প্রত্যাগম oc_typval2type(tv);

	পক্ষ VAR_TYPEALIAS:
	    প্রত্যাগম &t_typealias;

	পক্ষ VAR_FUNC:
	পক্ষ VAR_PARTIAL:
	    প্রত্যাগম fp_typval2type(tv, type_gap);

	পক্ষ VAR_INSTR:
	স্বভাব:
	    ভঙ্গ;
    }

    প্রত্যাগম রিক্ত;
}

/*
 * Return TRUE if "tv" is not a bool but should be converted to bool.
 */
    পূর্ণাঙ্ক
need_convert_to_bool(type_T *type, typval_T *tv)
{
    প্রত্যাগম type != রিক্ত && type == &t_bool && tv->v_type != VAR_BOOL
	    && (tv->v_type == VAR_সংখ্যা
		       && (tv->vval.v_সংখ্যা == 0 || tv->vval.v_সংখ্যা == 1));
}

/*
 * Get a type_T for a typval_T.
 * "type_list" is used to temporarily create types in.
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the most specific member type.
 */
    type_T *
typval2type(typval_T *tv, পূর্ণাঙ্ক copyID, garray_T *type_gap, পূর্ণাঙ্ক flags)
{
    type_T *type = typval2type_int(tv, copyID, type_gap, flags);

    যদি (type == রিক্ত)
	প্রত্যাগম রিক্ত;

    যদি (type != &t_bool && (tv->v_type == VAR_সংখ্যা
		&& (tv->vval.v_সংখ্যা == 0 || tv->vval.v_সংখ্যা == 1)))
	// Number 0 and 1 and expression with "&&" or "||" can also be used
	// for bool.
	type = &t_সংখ্যা_bool;
    অন্যথা যদি (type != &t_float && tv->v_type == VAR_সংখ্যা)
	// A number can also be used for float.
	type = &t_সংখ্যা_float;
    প্রত্যাগম type;
}

/*
 * Return TRUE if "type" can be used for a variable declaration.
 * Give an error and return FALSE if not.
 */
    পূর্ণাঙ্ক
valid_declaration_type(type_T *type)
{
    যদি (type->tt_type == VAR_SPECIAL  // null, none
	    || type->tt_type == VAR_VOID)
    {
	বর্ণ *tofree = রিক্ত;
	বর্ণ *name = type_name(type, &tofree);
	semsg(_(e_invalid_type_in_variable_declaration_str), name);
	vim_free(tofree);
	প্রত্যাগম FALSE;
    }
    প্রত্যাগম TRUE;
}

/*
 * Get a type_T for a typval_T, used for v: variables.
 * "type_list" is used to temporarily create types in.
 */
    type_T *
typval2type_vimvar(typval_T *tv, garray_T *type_gap)
{
    যদি (tv->v_type == VAR_LIST)  // e.g. for v:oldfiles
	প্রত্যাগম &t_list_string;
    যদি (tv->v_type == VAR_DICT)  // e.g. for v:event
	প্রত্যাগম &t_dict_any;
    প্রত্যাগম typval2type(tv, get_copyID(), type_gap, TVTT_DO_MEMBER);
}

    পূর্ণাঙ্ক
check_typval_arg_type(
	type_T	    *expected,
	typval_T    *actual_tv,
	বর্ণ	    *func_name,
	পূর্ণাঙ্ক	    arg_idx)
{
    where_T	where = WHERE_INIT;

    যদি (arg_idx > 0)
    {
	where.wt_index = arg_idx;
	where.wt_kind = WT_ARGUMENT;
    }
    where.wt_func_name = func_name;
    প্রত্যাগম check_typval_type(expected, actual_tv, where);
}

/*
 * Return true if "expected" is list<any> or dict<any> and "actual_tv" is a
 * list or dict of that kind, which matches whatever its values are.
 */
    স্থির bool
matches_listdict_any(type_T *expected, typval_T *actual_tv)
{
    প্রত্যাগম (expected->tt_type == VAR_LIST || expected->tt_type == VAR_DICT)
	    && actual_tv->v_type == expected->tt_type
	    && expected->tt_member != রিক্ত
	    && (expected->tt_member->tt_type == VAR_ANY
		|| expected->tt_member->tt_type == VAR_UNKNOWN);
}

/*
 * Return FAIL if "expected" and "actual" don't match.
 * When "argidx" > 0 it is included in the error message.
 */
    পূর্ণাঙ্ক
check_typval_type(type_T *expected, typval_T *actual_tv, where_T where)
{
    garray_T	type_list;
    type_T	*actual_type;
    পূর্ণাঙ্ক		res = FAIL;

    যদি (expected == রিক্ত)
	প্রত্যাগম OK;  // didn't expect anything.

    // No need to go through the values of a possibly big list or dict.
    যদি (matches_listdict_any(expected, actual_tv))
	প্রত্যাগম OK;

    ga_init2(&type_list, পরিমাণ(type_T *), 10);

    // A null_function and null_partial are special cases, they can be used to
    // clear a variable.
    যদি ((actual_tv->v_type == VAR_FUNC && actual_tv->vval.v_string == রিক্ত)
	    || (actual_tv->v_type == VAR_PARTIAL
					 && actual_tv->vval.v_partial == রিক্ত))
	actual_type = &t_func_unknown;
    অন্যথা
	// When the actual type is list<any> or dict<any> go through the values
	// to possibly get a more specific type.
	actual_type = typval2type(actual_tv, get_copyID(), &type_list,
					  TVTT_DO_MEMBER | TVTT_MORE_SPECIFIC);
    যদি (actual_type != রিক্ত)
    {
	res = check_type_maybe(expected, actual_type, TRUE, where);
	যদি (res == MAYBE && !(actual_type->tt_type == VAR_FUNC
				      && (actual_type->tt_member == &t_unknown
					  || actual_type->tt_member == রিক্ত)))
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
    প্রত্যাগম res;
}

    শূন্য
arg_type_mismatch(type_T *expected, type_T *actual, পূর্ণাঙ্ক arg_idx)
{
    where_T	where = WHERE_INIT;

    যদি (arg_idx > 0)
    {
	where.wt_index = arg_idx;
	where.wt_kind = WT_ARGUMENT;
    }
    type_mismatch_where(expected, actual, where);
}

    শূন্য
type_mismatch_where(type_T *expected, type_T *actual, where_T where)
{
    বর্ণ *tofree1, *tofree2;
    বর্ণ *typename1 = type_name(expected, &tofree1);
    বর্ণ *typename2 = type_name(actual, &tofree2);

    বিকল্প (where.wt_kind)
    {
	পক্ষ WT_MEMBER:
	    semsg(_(e_variable_str_type_mismatch_expected_str_but_got_str),
		    where.wt_func_name, typename1, typename2);
	    ভঙ্গ;
	পক্ষ WT_METHOD:
	পক্ষ WT_METHOD_ARG:
	পক্ষ WT_METHOD_RETURN:
	    semsg(_(e_method_str_type_mismatch_expected_str_but_got_str),
		    where.wt_func_name, typename1, typename2);
	    ভঙ্গ;
	পক্ষ WT_VARIABLE:
	    যদি (where.wt_func_name == রিক্ত)
		semsg(_(e_variable_nr_type_mismatch_expected_str_but_got_str),
			where.wt_index, typename1, typename2);
	    অন্যথা
		semsg(_(e_variable_nr_type_mismatch_expected_str_but_got_str_in_str),
			where.wt_index, typename1, typename2, where.wt_func_name);
	    ভঙ্গ;
	পক্ষ WT_ARGUMENT:
	    যদি (where.wt_func_name == রিক্ত)
		semsg(_(e_argument_nr_type_mismatch_expected_str_but_got_str),
			where.wt_index, typename1, typename2);
	    অন্যথা
		semsg(_(e_argument_nr_type_mismatch_expected_str_but_got_str_in_str),
			where.wt_index, typename1, typename2, where.wt_func_name);
	    ভঙ্গ;
	পক্ষ WT_CAST:
	পক্ষ WT_UNKNOWN:
	    যদি (where.wt_func_name == রিক্ত)
		semsg(_(e_type_mismatch_expected_str_but_got_str),
			typename1, typename2);
	    অন্যথা
		semsg(_(e_type_mismatch_expected_str_but_got_str_in_str),
			typename1, typename2, where.wt_func_name);
	    ভঙ্গ;
    }

    vim_free(tofree1);
    vim_free(tofree2);
}

/*
 * Check if the expected and actual types match for a tuple
 */
    স্থির পূর্ণাঙ্ক
check_tuple_type_maybe(
    type_T	*expected,
    type_T	*actual,
    where_T	where)
{
    যদি (expected->tt_argcount == -1 || actual->tt_argcount == -1
		|| expected->tt_args == রিক্ত || actual->tt_args == রিক্ত)
	প্রত্যাগম OK;

    // For a non-variadic tuple, the number of items must match
    যদি (!(expected->tt_flags & TTFLAG_VARARGS)
	    && expected->tt_argcount != actual->tt_argcount)
	প্রত্যাগম FAIL;

    // compare the type of each tuple item
    প্রতি (পূর্ণাঙ্ক i = 0; i < actual->tt_argcount; ++i)
    {
	type_T	*exp_type;
	type_T	*actual_type;

	যদি (expected->tt_flags & TTFLAG_VARARGS)
	{
	    যদি (i < expected->tt_argcount - 1)
		exp_type = expected->tt_args[i];
	    অন্যথা
		// For a variadic tuple, the last type is a List.  So use the
		// List member type.
		exp_type = expected->tt_args[expected->tt_argcount - 1]->tt_member;
	}
	অন্যথা
	    exp_type = expected->tt_args[i];

	যদি (actual->tt_flags & TTFLAG_VARARGS)
	{
	    যদি (i < actual->tt_argcount - 1)
		actual_type = actual->tt_args[i];
	    অন্যথা
		// For a variadic tuple, the last type is a List.  So use the
		// List member type.
		actual_type = actual->tt_args[actual->tt_argcount - 1]->tt_member;
	}
	অন্যথা
	    actual_type = actual->tt_args[i];

	// Allow for using "any" type for a tuple item
	যদি (actual->tt_args[i] != &t_any && check_type(exp_type, actual_type,
						FALSE, where) == FAIL)
	    প্রত্যাগম FAIL;
    }

    প্রত্যাগম OK;
}

/*
 * Check if the expected and actual types match for a function
 * Returns OK if "expected" and "actual" are matching function types.
 * Returns FAIL if "expected" and "actual" are different types.
 * Returns MAYBE when a runtime type check is needed.
 */
    স্থির পূর্ণাঙ্ক
check_func_type_maybe(
    type_T	*expected,
    type_T	*actual,
    where_T	where)
{
    পূর্ণাঙ্ক ret = OK;

    // If the return type is unknown it can be anything, including
    // nothing, thus there is no point in checking.
    যদি (expected->tt_member != &t_unknown)
    {
	যদি (actual->tt_member != রিক্ত
		&& actual->tt_member != &t_unknown)
	{
	    where_T  func_where = where;

	    func_where.wt_kind = WT_METHOD_RETURN;
	    ret = check_type_maybe(expected->tt_member,
		    actual->tt_member, FALSE,
		    func_where);
	}
	অন্যথা
	    ret = MAYBE;
    }
    যদি (ret != FAIL
	    && ((expected->tt_flags & TTFLAG_VARARGS)
		!= (actual->tt_flags & TTFLAG_VARARGS))
	    && expected->tt_argcount != -1)
	ret = FAIL;
    যদি (ret != FAIL && expected->tt_argcount != -1
	    && actual->tt_min_argcount != -1
	    && (actual->tt_argcount == -1
		|| (actual->tt_argcount < expected->tt_min_argcount
		    || actual->tt_argcount > expected->tt_argcount)))
	ret = FAIL;
    যদি (ret != FAIL && expected->tt_args != রিক্ত
	    && actual->tt_args != রিক্ত)
    {
	পূর্ণাঙ্ক i;

	প্রতি (i = 0; i < expected->tt_argcount
		&& i < actual->tt_argcount; ++i)
	{
	    where_T  func_where = where;
	    func_where.wt_kind = WT_METHOD_ARG;

	    // Allow for using "any" argument type, lambda's have them.
	    যদি (actual->tt_args[i] != &t_any && check_type(
			expected->tt_args[i], actual->tt_args[i], FALSE,
			func_where) == FAIL)
	    {
		ret = FAIL;
		ভঙ্গ;
	    }
	}
    }
    যদি (ret == OK && expected->tt_argcount >= 0
	    && actual->tt_argcount == -1)
	// check the argument count at runtime
	ret = MAYBE;

    প্রত্যাগম ret;
}

/*
 * Check if the expected and actual types match for an object
 * Returns OK if "expected" and "actual" are matching object types.
 * Returns FAIL if "expected" and "actual" are different types.
 * Returns MAYBE when a runtime type check is needed.
 */
    স্থির পূর্ণাঙ্ক
check_object_type_maybe(
    type_T	*expected,
    type_T	*actual,
    where_T	where)
{
    পূর্ণাঙ্ক ret = OK;

    যদি (actual->tt_type == VAR_ANY)
	প্রত্যাগম MAYBE;	// use runtime type check
    যদি (actual->tt_type != VAR_OBJECT)
	প্রত্যাগম FAIL;	// don't use tt_class
    যদি (actual->tt_class == রিক্ত)    // null object
	প্রত্যাগম OK;
    // t_object_any matches any object except for an enum item
    যদি (expected == &t_object_any && !IS_ENUM(actual->tt_class))
	প্রত্যাগম OK;

    // For object method arguments, do a invariant type check in
    // an extended class.  For all others, do a covariance type check.
    যদি (where.wt_kind == WT_METHOD_ARG)
    {
	যদি (actual->tt_class != expected->tt_class)
	    ret = FAIL;
    }
    অন্যথা যদি (!class_instance_of(actual->tt_class, expected->tt_class))
    {
	// Check if this is an up-cast, if so we'll have to check the type at
	// runtime.
	যদি (where.wt_kind == WT_CAST &&
		class_instance_of(expected->tt_class, actual->tt_class))
	    ret = MAYBE;
	অন্যথা
	    ret = FAIL;
    }

    প্রত্যাগম ret;
}

/*
 * Check if the expected and actual types match.
 * Does not allow for assigning "any" to a specific type.
 * When "argidx" > 0 it is included in the error message.
 * Return OK if types match.
 * Return FAIL if types do not match.
 */
    পূর্ণাঙ্ক
check_type(
	type_T	*expected,
	type_T	*actual,
	পূর্ণাঙ্ক	give_বার্তা,
	where_T where)
{
    পূর্ণাঙ্ক ret = check_type_maybe(expected, actual, give_বার্তা, where);

    প্রত্যাগম ret == MAYBE ? OK : ret;
}

/*
 * As check_type() but return MAYBE when a runtime type check should be used
 * when compiling.
 */
    পূর্ণাঙ্ক
check_type_maybe(
	type_T	*expected,
	type_T	*actual,
	পূর্ণাঙ্ক	give_বার্তা,
	where_T where)
{
    পূর্ণাঙ্ক ret = OK;

    // When expected is "unknown" we accept any actual type.
    // When expected is "any" we accept any actual type except "void".
    যদি (expected->tt_type != VAR_UNKNOWN
	    && !(expected->tt_type == VAR_ANY && actual->tt_type != VAR_VOID))

    {
	// tt_type should match, except that a "partial" can be assigned to a
	// variable with type "func".
	// And "unknown" (using global variable) and "any" need a runtime type
	// check.
	যদি (!(expected->tt_type == actual->tt_type
		    || actual->tt_type == VAR_UNKNOWN
		    || actual->tt_type == VAR_ANY
		    || (expected->tt_type == VAR_FUNC
					   && actual->tt_type == VAR_PARTIAL)))
	{
	    যদি (expected->tt_type == VAR_BOOL
					&& (actual->tt_flags & TTFLAG_BOOL_OK))
		// Using number 0 or 1 for bool is OK.
		প্রত্যাগম OK;
	    যদি (expected->tt_type == VAR_FLOAT
		    && actual->tt_type == VAR_সংখ্যা
		    && ((expected->tt_flags & TTFLAG_সংখ্যা_OK)
			     || (actual->tt_flags & TTFLAG_FLOAT_OK)))
		// Using a number where a float is expected is OK here.
		প্রত্যাগম OK;
	    যদি (expected->tt_type == VAR_LIST
		    && actual->tt_type == VAR_TUPLE
		    && (expected->tt_flags & TTFLAG_TUPLE_OK))
		// Using a tuple where a list is expected is OK here.
		প্রত্যাগম OK;
	    যদি (give_বার্তা)
		type_mismatch_where(expected, actual, where);
	    প্রত্যাগম FAIL;
	}
	যদি (expected->tt_type == VAR_DICT || expected->tt_type == VAR_LIST)
	{
	    // "unknown" is used for an empty list or dict
	    যদি (actual->tt_member != রিক্ত && actual->tt_member != &t_unknown)
		ret = check_type_maybe(expected->tt_member, actual->tt_member,
								 FALSE, where);
	}
	অন্যথা যদি (expected->tt_type == VAR_TUPLE && actual != &t_any)
	    ret =  check_tuple_type_maybe(expected, actual, where);
	অন্যথা যদি (expected->tt_type == VAR_FUNC && actual != &t_any)
	    ret = check_func_type_maybe(expected, actual, where);
	অন্যথা যদি (expected->tt_type == VAR_OBJECT)
	    ret = check_object_type_maybe(expected, actual, where);

	যদি (ret == FAIL && give_বার্তা)
	    type_mismatch_where(expected, actual, where);
    }

    যদি (ret == OK && expected->tt_type != VAR_UNKNOWN
	    && expected->tt_type != VAR_ANY
	    && (actual->tt_type == VAR_UNKNOWN || actual->tt_type == VAR_ANY))
	// check the type at runtime
	ret = MAYBE;

    প্রত্যাগম ret;
}

/*
 * Check that the arguments of "type" match "argvars[argcount]".
 * "base_tv" is from "expr->Func()".
 * Return OK/FAIL.
 */
    পূর্ণাঙ্ক
check_argument_types(
	type_T	    *type,
	typval_T    *argvars,
	পূর্ণাঙ্ক	    argcount,
	typval_T    *base_tv,
	char_u	    *name)
{
    পূর্ণাঙ্ক	    varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;
    পূর্ণাঙ্ক	    i;
    পূর্ণাঙ্ক	    totcount = argcount + (base_tv == রিক্ত ? 0 : 1);

    যদি (type->tt_type != VAR_FUNC && type->tt_type != VAR_PARTIAL)
	প্রত্যাগম OK;  // just in case
    যদি (totcount < type->tt_min_argcount - varargs)
    {
	emsg_funcname(e_not_enough_arguments_for_function_str, name);
	প্রত্যাগম FAIL;
    }
    যদি (!varargs && type->tt_argcount >= 0 && totcount > type->tt_argcount)
    {
	emsg_funcname(e_too_many_arguments_for_function_str, name);
	প্রত্যাগম FAIL;
    }
    যদি (type->tt_args == রিক্ত)
	প্রত্যাগম OK;  // cannot check


    প্রতি (i = 0; i < totcount; ++i)
    {
	type_T	    *expected;
	typval_T    *tv;

	যদি (base_tv != রিক্ত)
	{
	    যদি (i == 0)
		tv = base_tv;
	    অন্যথা
		tv = &argvars[i - 1];
	}
	অন্যথা
	    tv = &argvars[i];
	যদি (varargs && i >= type->tt_argcount - 1)
	{
	    expected = type->tt_args[type->tt_argcount - 1];
	    যদি (expected != রিক্ত && expected->tt_type == VAR_LIST)
		expected = expected->tt_member;
	    যদি (expected == রিক্ত)
		expected = &t_any;
	}
	অন্যথা
	    expected = type->tt_args[i];

	// check the type, unless the value is v:none
	যদি ((tv->v_type != VAR_SPECIAL || tv->vval.v_সংখ্যা != VVAL_NONE)
		   && check_typval_arg_type(expected, tv, রিক্ত, i + 1) == FAIL)
	    প্রত্যাগম FAIL;
    }
    প্রত্যাগম OK;
}

// Maximum nesting of types, to avoid running out of stack space.  E.g.
// "list<list<number>>" is nested twice.
#define MAX_TYPE_DEPTH	1000

// Current number of levels of parse_type() and the deepest level reached while
// parsing the current type.  The nesting is one less than the number of
// levels.
স্থির পূর্ণাঙ্ক	parse_depth = 0;
স্থির পূর্ণাঙ্ক	parse_max_depth = 0;

/*
 * Skip over type in list<type>, dict<type> or tuple<type>.
 * Returns a pointer to the character after the type.  "syn_error" is set to
 * TRUE on syntax error.
 */
    স্থির char_u *
skip_member_type(char_u *start, char_u *p, পূর্ণাঙ্ক *syn_error)
{
    যদি (STRNCMP("tuple", start, 5) == 0)
    {
	// handle tuple<{type1}, {type2}, ....<type>>
	p = skipwhite(p + 1);
	যাবত্ (*p != '>' && *p != NUL)
	{
	    char_u *sp = p;

	    যদি (STRNCMP(p, "...", 3) == 0)
		p += 3;
	    p = skip_type(p, TRUE);
	    যদি (p == sp)
	    {
		*syn_error = TRUE;
		প্রত্যাগম p;  // syntax error
	    }
	    যদি (*p == ',')
		p = skipwhite(p + 1);
	}
	যদি (*p == '>')
	    p++;
    }
    অন্যথা
    {
	p = skipwhite(p);
	p = skip_type(skipwhite(p + 1), FALSE);
	p = skipwhite(p);
	যদি (*p == '>')
	    ++p;
    }

    প্রত্যাগম p;
}

/*
 * Skip over a function type.  Returns a pointer to the character after the
 * type.  "syn_error" is set to TRUE on syntax error.
 */
    স্থির char_u *
skip_func_type(char_u *p, পূর্ণাঙ্ক *syn_error)
{
    যদি (*p == '(')
    {
	// handle func(args): type
	++p;
	যাবত্ (*p != ')' && *p != NUL)
	{
	    char_u *sp = p;

	    যদি (STRNCMP(p, "...", 3) == 0)
		p += 3;
	    p = skip_type(p, TRUE);
	    যদি (p == sp)
	    {
		*syn_error = TRUE;
		প্রত্যাগম p;  // syntax error
	    }
	    যদি (*p == ',')
		p = skipwhite(p + 1);
	}
	যদি (*p == ')')
	{
	    যদি (p[1] == ':')
		p = skip_type(skipwhite(p + 2), FALSE);
	    অন্যথা
		++p;
	}
    }
    অন্যথা
    {
	// handle func: return_type
	p = skip_type(skipwhite(p + 1), FALSE);
    }

    প্রত্যাগম p;
}

/*
 * Inner part of skip_type().
 */
    স্থির char_u *
skip_type_inner(char_u *start, পূর্ণাঙ্ক optional)
{
    char_u	*p = start;
    পূর্ণাঙ্ক		syn_error = FALSE;

    যদি (optional && *p == '?')
	++p;

    // Also skip over "." for imported classes: "import.ClassName".
    যাবত্ (ASCII_ISALNUM(*p) || *p == '_' || *p == '.')
	++p;

    // Skip over "<type>"; this is permissive about white space.
    যদি (*skipwhite(p) == '<')
    {
	p = skip_member_type(start, p, &syn_error);
	যদি (syn_error)
	    প্রত্যাগম p;
    }
    অন্যথা যদি ((*p == '(' || (*p == ':' && VIM_ISWHITE(p[1])))
					     && STRNCMP("func", start, 4) == 0)
    {
	// skip over function type
	p = skip_func_type(p, &syn_error);
	যদি (syn_error)
	    প্রত্যাগম p;
    }

    প্রত্যাগম p;
}

/*
 * Skip over a type definition and return a pointer to just after it.
 * When "optional" is TRUE then a leading "?" is accepted.
 */
    char_u *
skip_type(char_u *start, পূর্ণাঙ্ক optional)
{
    স্থির পূর্ণাঙ্ক	skip_depth = 0;
    char_u	*p = start;

    // Too deeply nested: skip to the matching ">" without recursing, an
    // error is given when parsing the type.
    যদি (skip_depth > MAX_TYPE_DEPTH)
    {
	পূর্ণাঙ্ক	depth = 0;

	প্রতি ( ; *p != NUL; ++p)
	{
	    যদি (*p == '<')
		++depth;
	    অন্যথা যদি (*p == '>')
	    {
		যদি (depth == 0)
		    ভঙ্গ;
		যদি (--depth == 0)
		    প্রত্যাগম p + 1;
	    }
	    অন্যথা যদি (depth == 0 && !ASCII_ISALNUM(*p) && *p != '_'
								&& *p != '.')
		ভঙ্গ;
	}
	প্রত্যাগম p;
    }

    ++skip_depth;
    p = skip_type_inner(start, optional);
    --skip_depth;
    প্রত্যাগম p;
}

/*
 * Parse the member type: "<type>" and return "type" with the member set.
 * Use "type_gap" if a new type needs to be added.
 * "info" is extra information for an error message.
 * Returns NULL in case of failure.
 */
    স্থির type_T *
parse_type_member(
	char_u	    **arg,
	type_T	    *type,
	garray_T    *type_gap,
	পূর্ণাঙ্ক	    give_error,
	বর্ণ	    *info,
	ufunc_T	    *ufunc,
	cctx_T	    *cctx)
{
    char_u  *arg_start = *arg;
    type_T  *member_type;
    পূর্ণাঙ্ক	    prev_called_emsg = called_emsg;

    যদি (**arg != '<')
    {
	যদি (give_error)
	{
	    যদি (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    অন্যথা
		semsg(_(e_missing_type_after_str), info);
	}
	প্রত্যাগম রিক্ত;
    }
    *arg = skipwhite(*arg + 1);

    member_type = parse_type(arg, type_gap, ufunc, cctx, give_error);
    যদি (member_type == রিক্ত || !valid_declaration_type(member_type))
	প্রত্যাগম রিক্ত;

    *arg = skipwhite(*arg);
    যদি (**arg != '>' && called_emsg == prev_called_emsg)
    {
	যদি (give_error)
	    semsg(_(e_missing_gt_after_type_str), arg_start);
	প্রত্যাগম রিক্ত;
    }
    ++*arg;

    যদি (type->tt_type == VAR_LIST)
	প্রত্যাগম get_list_type(member_type, type_gap);
    প্রত্যাগম get_dict_type(member_type, type_gap);
}

/*
 * Parse a "func" type at "*arg" and advance over it.
 * When "give_error" is TRUE give error messages, otherwise be quiet.
 * Return NULL for failure.
 */
    স্থির type_T *
parse_type_func(
    char_u	**arg,
    size_t	len,
    garray_T	*type_gap,
    পূর্ণাঙ্ক		give_error,
    ufunc_T	*ufunc,
    cctx_T	*cctx)
{
    char_u  *p;
    type_T  *type;
    type_T  *ret_type = &t_unknown;
    পূর্ণাঙ্ক	argcount = -1;
    পূর্ণাঙ্ক	flags = 0;
    পূর্ণাঙ্ক	first_optional = -1;
    type_T	*arg_type[MAX_FUNC_ARGS + 1];

    // func({type}, ...{type}): {type}
    *arg += len;
    যদি (**arg == '(')
    {
	// "func" may or may not return a value, "func()" does
	// not return a value.
	ret_type = &t_void;

	p = ++*arg;
	argcount = 0;
	যাবত্ (*p != NUL && *p != ')')
	{
	    যদি (*p == '?')
	    {
		যদি (first_optional == -1)
		    first_optional = argcount;
		++p;
	    }
	    অন্যথা যদি (STRNCMP(p, "...", 3) == 0)
	    {
		flags |= TTFLAG_VARARGS;
		p += 3;
	    }
	    অন্যথা যদি (first_optional != -1)
	    {
		যদি (give_error)
		    emsg(_(e_mandatory_argument_after_optional_argument));
		প্রত্যাগম রিক্ত;
	    }

	    type = parse_type(&p, type_gap, ufunc, cctx, give_error);
	    যদি (type == রিক্ত || !valid_declaration_type(type))
		প্রত্যাগম রিক্ত;
	    যদি ((flags & TTFLAG_VARARGS) != 0 && type->tt_type != VAR_LIST)
	    {
		বর্ণ *tofree;
		semsg(_(e_variable_arguments_type_must_be_list_str),
			type_name(type, &tofree));
		vim_free(tofree);
		প্রত্যাগম রিক্ত;
	    }
	    arg_type[argcount++] = type;

	    // Nothing comes after "...{type}".
	    যদি (flags & TTFLAG_VARARGS)
		ভঙ্গ;

	    যদি (*p != ',' && *skipwhite(p) == ',')
	    {
		যদি (give_error)
		    semsg(_(e_no_white_space_allowed_before_str_str),
			    ",", p);
		প্রত্যাগম রিক্ত;
	    }
	    যদি (*p == ',')
	    {
		++p;
		যদি (!VIM_ISWHITE(*p))
		{
		    যদি (give_error)
			semsg(_(e_white_space_required_after_str_str),
				",", p - 1);
		    প্রত্যাগম রিক্ত;
		}
	    }
	    p = skipwhite(p);
	    যদি (argcount == MAX_FUNC_ARGS)
	    {
		যদি (give_error)
		    emsg(_(e_too_many_argument_types));
		প্রত্যাগম রিক্ত;
	    }
	}

	p = skipwhite(p);
	যদি (*p != ')')
	{
	    যদি (give_error)
		emsg(_(e_missing_closing_paren));
	    প্রত্যাগম রিক্ত;
	}
	*arg = p + 1;
    }
    যদি (**arg == ':')
    {
	// parse return type
	++*arg;
	যদি (!VIM_ISWHITE(**arg) && give_error)
	    semsg(_(e_white_space_required_after_str_str), ":", *arg - 1);
	*arg = skipwhite(*arg);
	ret_type = parse_type(arg, type_gap, ufunc, cctx, give_error);
	যদি (ret_type == রিক্ত)
	    প্রত্যাগম রিক্ত;
    }
    যদি (flags == 0 && first_optional == -1 && argcount <= 0)
	type = get_func_type(ret_type, argcount, type_gap);
    অন্যথা
    {
	type = alloc_func_type(ret_type, argcount, type_gap);
	type->tt_flags = flags;
	যদি (argcount > 0)
	{
	    type->tt_argcount = argcount;
	    type->tt_min_argcount = first_optional == -1
						? argcount : first_optional;
	    যদি (func_type_add_arg_types(type, argcount, type_gap) == FAIL)
		প্রত্যাগম রিক্ত;
	    mch_memmove(type->tt_args, arg_type, পরিমাণ(type_T *) * argcount);
	}
    }

    প্রত্যাগম type;
}

/*
 * Parse a "tuple" type at "*arg" and advance over it.
 * When "give_error" is TRUE give error messages, otherwise be quiet.
 * Return NULL for failure.
 */
    স্থির type_T *
parse_type_tuple(
    char_u	**arg,
    garray_T	*type_gap,
    পূর্ণাঙ্ক		give_error,
    ufunc_T	*ufunc,
    cctx_T	*cctx)
{
    char_u	*p;
    type_T	*type;
    type_T	*ret_type = রিক্ত;
    পূর্ণাঙ্ক		typecount = -1;
    পূর্ণাঙ্ক		flags = 0;
    garray_T	tuple_types_ga;

    ga_init2(&tuple_types_ga, পরিমাণ(type_T *), 10);

    // tuple<{type}, {type}>
    // tuple<{type}, ...{type}>
    যদি (**arg != '<')
    {
	যদি (give_error)
	{
	    যদি (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    অন্যথা
		semsg(_(e_missing_type_after_str), "tuple");
	}

	// only "tuple" is specified
	প্রত্যাগম রিক্ত;
    }

    p = ++*arg;
    typecount = 0;
    যাবত্ (*p != NUL && *p != '>')
    {
	যদি (STRNCMP(p, "...", 3) == 0)
	{
	    flags |= TTFLAG_VARARGS;
	    p += 3;
	}

	type = parse_type(&p, type_gap, ufunc, cctx, give_error);
	যদি (type == রিক্ত || !valid_declaration_type(type))
	    গচ্ছ on_err;

	যদি ((flags & TTFLAG_VARARGS) != 0 && type->tt_type != VAR_LIST)
	{
	    বর্ণ *tofree;
	    semsg(_(e_variadic_tuple_must_end_with_list_type_str),
						type_name(type, &tofree));
	    vim_free(tofree);
	    গচ্ছ on_err;
	}

	// Add the item type
	যদি (ga_grow(&tuple_types_ga, 1) == FAIL)
	    গচ্ছ on_err;
	((type_T **)tuple_types_ga.ga_data)[tuple_types_ga.ga_len] = type;
	tuple_types_ga.ga_len++;
	typecount++;

	// Nothing comes after "...{type}".
	যদি (flags & TTFLAG_VARARGS)
	    ভঙ্গ;

	যদি (*p != ',' && *skipwhite(p) == ',')
	{
	    যদি (give_error)
		semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
	    গচ্ছ on_err;
	}
	যদি (*p == ',')
	{
	    ++p;
	    যদি (!VIM_ISWHITE(*p))
	    {
		যদি (give_error)
		    semsg(_(e_white_space_required_after_str_str),
			    ",", p - 1);
		গচ্ছ on_err;
	    }
	}
	p = skipwhite(p);
    }

    p = skipwhite(p);
    যদি (*p != '>' || typecount <= 0)
    {
	যদি (give_error)
	    semsg(_(e_missing_type_after_str), p);
	গচ্ছ on_err;
    }
    *arg = p + 1;

    ret_type = alloc_tuple_type(typecount, type_gap);
    যদি (ret_type == &t_any
	    || tuple_type_add_types(ret_type, typecount, type_gap) == FAIL)
    {
	ret_type = রিক্ত;
	গচ্ছ on_err;
    }
    ret_type->tt_flags = flags;
    ret_type->tt_argcount = typecount;
    mch_memmove(ret_type->tt_args, tuple_types_ga.ga_data,
						পরিমাণ(type_T *) * typecount);

on_err:
    ga_clear(&tuple_types_ga);

    প্রত্যাগম ret_type;
}

/*
 * Parse a "object" type at "*arg" and advance over it.
 * When "give_error" is TRUE give error messages, otherwise be quiet.
 * Return NULL for failure.
 */
    স্থির type_T *
parse_type_object(
    char_u	**arg,
    garray_T	*type_gap,
    পূর্ণাঙ্ক		give_error,
    cctx_T	*cctx)
{
    char_u	*arg_start = *arg;
    type_T	*object_type;
    পূর্ণাঙ্ক		prev_called_emsg = called_emsg;

    // object<X> or object<any>
    যদি (**arg != '<')
    {
	যদি (give_error)
	{
	    যদি (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    অন্যথা
		semsg(_(e_missing_type_after_str), "object");
	}

	// only "object" is specified
	প্রত্যাগম রিক্ত;
    }

    // skip spaces following "object<"
    *arg = skipwhite(*arg + 1);

    object_type = parse_type(arg, type_gap, রিক্ত, cctx, give_error);
    যদি (object_type == রিক্ত)
	প্রত্যাগম রিক্ত;

    *arg = skipwhite(*arg);
    যদি (**arg != '>' && called_emsg == prev_called_emsg)
    {
	যদি (give_error)
	    semsg(_(e_missing_gt_after_type_str), arg_start);
	প্রত্যাগম রিক্ত;
    }
    ++*arg;

    যদি (object_type->tt_type == VAR_ANY)
	প্রত্যাগম &t_object_any;

    যদি (object_type->tt_type != VAR_OBJECT)
    {
	// specified type is not a class
	যদি (give_error)
	    semsg(_(e_class_name_not_found_str), arg_start);
	প্রত্যাগম রিক্ত;
    }

    প্রত্যাগম object_type;
}

/*
 * Parse a user defined type at "*arg" and advance over it.
 * It can be a class or an interface or a typealias name, possibly imported.
 * Return NULL if a type is not found.
 */
    স্থির type_T *
parse_type_user_defined(
    char_u	**arg,
    size_t	len,
    garray_T	*type_gap,
    পূর্ণাঙ্ক		give_error,
    ufunc_T	*ufunc,
    cctx_T	*cctx)
{
    পূর্ণাঙ্ক		did_emsg_before = did_emsg;
    typval_T	tv;

    tv.v_type = VAR_UNKNOWN;
    যদি (eval_variable_import(*arg, &tv) == OK)
    {
	যদি (tv.v_type == VAR_CLASS && tv.vval.v_class != রিক্ত)
	{
	    type_T *type = get_type_ptr(type_gap);
	    যদি (type != রিক্ত)
	    {
		// Although the name is that of a class or interface, the type
		// uses will be an object.
		type->tt_type = VAR_OBJECT;
		type->tt_class = tv.vval.v_class;
		clear_tv(&tv);

		*arg += len;
		// Skip over ".ClassName".
		যাবত্ (ASCII_ISALNUM(**arg) || **arg == '_' || **arg == '.')
		    ++*arg;

		প্রত্যাগম type;
	    }
	}
	অন্যথা যদি (tv.v_type == VAR_TYPEALIAS)
	{
	    // user defined type
	    // The type alias may itself be a nested type, check the resulting
	    // nesting depth.
	    পূর্ণাঙ্ক	depth = parse_depth - 1
				+ MAX(tv.vval.v_typealias->ta_depth, 1);
	    যদি (depth - 1 > MAX_TYPE_DEPTH)
	    {
		clear_tv(&tv);
		যদি (give_error)
		    emsg(_(e_type_nested_too_deep));
		প্রত্যাগম রিক্ত;
	    }
	    যদি (depth > parse_max_depth)
		parse_max_depth = depth;

	    type_T *type = copy_type(tv.vval.v_typealias->ta_type, type_gap);
	    *arg += len;
	    clear_tv(&tv);
	    // Skip over ".TypeName".
	    যাবত্ (ASCII_ISALNUM(**arg) || **arg == '_' || **arg == '.')
		++*arg;
	    প্রত্যাগম type;
	}

	clear_tv(&tv);
    }

    // Check whether it is a generic type
    type_T *type = find_generic_type(*arg, len, ufunc, cctx);
    যদি (type != রিক্ত)
    {
	*arg += len;
	প্রত্যাগম type;
    }

    যদি (give_error && (did_emsg == did_emsg_before))
    {
	char_u	*p = skip_type(*arg, FALSE);
	বর্ণ	cc = *p;

	*p = NUL;
	semsg(_(e_type_not_recognized_str), *arg);
	*p = cc;
    }

    প্রত্যাগম রিক্ত;
}

/*
 * Inner part of parse_type().
 */
    স্থির type_T *
parse_type_inner(
    char_u	**arg,
    garray_T	*type_gap,
    ufunc_T	*ufunc,
    cctx_T	*cctx,
    পূর্ণাঙ্ক		give_error)
{
    char_u  *p = *arg;
    size_t  len;

    // Skip over the first word.
    যাবত্ (ASCII_ISALNUM(*p) || *p == '_')
	++p;
    len = p - *arg;

    বিকল্প (**arg)
    {
	পক্ষ 'a':
	    যদি (len == 3 && STRNCMP(*arg, "any", len) == 0)
	    {
		*arg += len;
		প্রত্যাগম &t_any;
	    }
	    ভঙ্গ;
	পক্ষ 'b':
	    যদি (len == 4 && STRNCMP(*arg, "bool", len) == 0)
	    {
		*arg += len;
		প্রত্যাগম &t_bool;
	    }
	    যদি (len == 4 && STRNCMP(*arg, "blob", len) == 0)
	    {
		*arg += len;
		প্রত্যাগম &t_blob;
	    }
	    ভঙ্গ;
	পক্ষ 'c':
	    যদি (len == 7 && STRNCMP(*arg, "channel", len) == 0)
	    {
		*arg += len;
		প্রত্যাগম &t_channel;
	    }
	    ভঙ্গ;
	পক্ষ 'd':
	    যদি (len == 4 && STRNCMP(*arg, "dict", len) == 0)
	    {
		*arg += len;
		প্রত্যাগম parse_type_member(arg, &t_dict_any, type_gap,
						give_error, "dict", ufunc,
						cctx);
	    }
	    ভঙ্গ;
	পক্ষ 'f':
	    যদি (len == 5 && STRNCMP(*arg, "float", len) == 0)
	    {
		*arg += len;
		প্রত্যাগম &t_float;
	    }
	    যদি (len == 4 && STRNCMP(*arg, "func", len) == 0)
		প্রত্যাগম parse_type_func(arg, len, type_gap, give_error, ufunc,
									cctx);
	    ভঙ্গ;
	পক্ষ 'j':
	    যদি (len == 3 && STRNCMP(*arg, "job", len) == 0)
	    {
		*arg += len;
		প্রত্যাগম &t_job;
	    }
	    ভঙ্গ;
	পক্ষ 'l':
	    যদি (len == 4 && STRNCMP(*arg, "list", len) == 0)
	    {
		*arg += len;
		প্রত্যাগম parse_type_member(arg, &t_list_any, type_gap,
						give_error, "list", ufunc,
						cctx);
	    }
	    ভঙ্গ;
	পক্ষ 'n':
	    যদি (len == 6 && STRNCMP(*arg, "number", len) == 0)
	    {
		*arg += len;
		প্রত্যাগম &t_সংখ্যা;
	    }
	    ভঙ্গ;
	পক্ষ 'o':
	    যদি (len == 6 && STRNCMP(*arg, "object", len) == 0)
	    {
		*arg += len;
		প্রত্যাগম parse_type_object(arg, type_gap, give_error, cctx);
	    }
	    ভঙ্গ;
	পক্ষ 's':
	    যদি (len == 6 && STRNCMP(*arg, "string", len) == 0)
	    {
		*arg += len;
		প্রত্যাগম &t_string;
	    }
	    ভঙ্গ;
	পক্ষ 't':
	    যদি (len == 5 && STRNCMP(*arg, "tuple", len) == 0)
	    {
		*arg += len;
		প্রত্যাগম parse_type_tuple(arg, type_gap, give_error, ufunc,
									cctx);
	    }
	    ভঙ্গ;
	পক্ষ 'v':
	    যদি (len == 4 && STRNCMP(*arg, "void", len) == 0)
	    {
		*arg += len;
		প্রত্যাগম &t_void;
	    }
	    ভঙ্গ;
    }

    // User defined type
    প্রত্যাগম parse_type_user_defined(arg, len, type_gap, give_error, ufunc,
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
    পূর্ণাঙ্ক		give_error)
{
    type_T	*type;

    যদি (parse_depth > MAX_TYPE_DEPTH)
    {
	যদি (give_error)
	    emsg(_(e_type_nested_too_deep));
	প্রত্যাগম রিক্ত;
    }
    যদি (parse_depth == 0)
	parse_max_depth = 0;
    ++parse_depth;
    যদি (parse_depth > parse_max_depth)
	parse_max_depth = parse_depth;
    type = parse_type_inner(arg, type_gap, ufunc, cctx, give_error);
    --parse_depth;
    প্রত্যাগম type;
}

/*
 * Return the number of levels of the type last parsed by parse_type().
 */
    পূর্ণাঙ্ক
parse_type_depth(শূন্য)
{
    প্রত্যাগম parse_max_depth;
}

/*
 * Check if "type1" and "type2" are exactly the same.
 * "flags" can have ETYPE_ARG_UNKNOWN, which means that an unknown argument
 * type in "type1" is accepted.
 */
    পূর্ণাঙ্ক
equal_type(type_T *type1, type_T *type2, পূর্ণাঙ্ক flags)
{
    পূর্ণাঙ্ক i;

    যদি (type1 == রিক্ত || type2 == রিক্ত)
	প্রত্যাগম FALSE;
    যদি (type1->tt_type != type2->tt_type)
	প্রত্যাগম FALSE;
    বিকল্প (type1->tt_type)
    {
	পক্ষ VAR_UNKNOWN:
	পক্ষ VAR_ANY:
	পক্ষ VAR_VOID:
	পক্ষ VAR_SPECIAL:
	পক্ষ VAR_BOOL:
	পক্ষ VAR_সংখ্যা:
	পক্ষ VAR_FLOAT:
	পক্ষ VAR_STRING:
	পক্ষ VAR_BLOB:
	পক্ষ VAR_JOB:
	পক্ষ VAR_CHANNEL:
	পক্ষ VAR_INSTR:
	পক্ষ VAR_TYPEALIAS:
	    ভঙ্গ;  // not composite is always OK
	পক্ষ VAR_OBJECT:
	পক্ষ VAR_CLASS:
	    // Objects are considered equal if they are from the same class
	    প্রত্যাগম type1->tt_class == type2->tt_class;
	পক্ষ VAR_LIST:
	পক্ষ VAR_DICT:
	    প্রত্যাগম equal_type(type1->tt_member, type2->tt_member, flags);
	পক্ষ VAR_TUPLE:
	    যদি (type1->tt_argcount != type2->tt_argcount)
		প্রত্যাগম FALSE;
	    যদি (type1->tt_argcount < 0
			   || type1->tt_args == রিক্ত || type2->tt_args == রিক্ত)
		প্রত্যাগম TRUE;
	    প্রতি (i = 0; i < type1->tt_argcount; ++i)
		যদি ((flags & ETYPE_ARG_UNKNOWN) == 0
			&& !equal_type(type1->tt_args[i], type2->tt_args[i],
									flags))
		    প্রত্যাগম FALSE;
	    প্রত্যাগম TRUE;
	পক্ষ VAR_FUNC:
	পক্ষ VAR_PARTIAL:
	    যদি (!equal_type(type1->tt_member, type2->tt_member, flags)
		    || type1->tt_argcount != type2->tt_argcount)
		প্রত্যাগম FALSE;
	    যদি (type1->tt_argcount < 0
			   || type1->tt_args == রিক্ত || type2->tt_args == রিক্ত)
		প্রত্যাগম TRUE;
	    প্রতি (i = 0; i < type1->tt_argcount; ++i)
		যদি ((flags & ETYPE_ARG_UNKNOWN) == 0
			&& !equal_type(type1->tt_args[i], type2->tt_args[i],
									flags))
		    প্রত্যাগম FALSE;
	    প্রত্যাগম TRUE;
    }
    প্রত্যাগম TRUE;
}

/*
 * Find the common type of "type1" (VAR_FUNC) and "type2" (VAR_FUNC) and put it
 * in "dest".  "type2" and "dest" may be the same.
 */
    স্থির শূন্য
common_type_var_func(
    type_T	*type1,
    type_T	*type2,
    type_T	**dest,
    garray_T	*type_gap)
{
    type_T *common;

    // When one of the types is t_func_unknown return the other one.
    // Useful if a list or dict item is null_func.
    যদি (type1 == &t_func_unknown)
    {
	*dest = type2;
	প্রত্যাগম;
    }
    যদি (type2 == &t_func_unknown)
    {
	*dest = type1;
	প্রত্যাগম;
    }

    common_type(type1->tt_member, type2->tt_member, &common, type_gap);
    যদি (type1->tt_argcount == type2->tt_argcount
	    && type1->tt_argcount >= 0)
    {
	পূর্ণাঙ্ক argcount = type1->tt_argcount;
	পূর্ণাঙ্ক i;

	*dest = alloc_func_type(common, argcount, type_gap);
	যদি (type1->tt_args != রিক্ত && type2->tt_args != রিক্ত)
	{
	    যদি (func_type_add_arg_types(*dest, argcount,
			type_gap) == OK)
		প্রতি (i = 0; i < argcount; ++i)
		    common_type(type1->tt_args[i], type2->tt_args[i],
			    &(*dest)->tt_args[i], type_gap);
	}
    }
    অন্যথা
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
    শূন্য
common_type(type_T *type1, type_T *type2, type_T **dest, garray_T *type_gap)
{
    যদি (equal_type(type1, type2, 0))
    {
	*dest = type1;
	প্রত্যাগম;
    }

    // If either is VAR_UNKNOWN use the other type.  An empty list/dict has no
    // specific type.
    যদি (type1 == রিক্ত || type1->tt_type == VAR_UNKNOWN)
    {
	*dest = type2;
	প্রত্যাগম;
    }
    যদি (type2 == রিক্ত || type2->tt_type == VAR_UNKNOWN)
    {
	*dest = type1;
	প্রত্যাগম;
    }

    যদি (type1->tt_type == type2->tt_type)
    {
	যদি (type1->tt_type == VAR_LIST
		|| type1->tt_type == VAR_DICT)
	{
	    type_T *common;

	    common_type(type1->tt_member, type2->tt_member, &common, type_gap);
	    যদি (type1->tt_type == VAR_LIST)
		*dest = get_list_type(common, type_gap);
	    অন্যথা
		*dest = get_dict_type(common, type_gap);
	    প্রত্যাগম;
	}
	অন্যথা যদি (type1->tt_type == VAR_FUNC)
	{
	    common_type_var_func(type1, type2, dest, type_gap);
	    প্রত্যাগম;
	}
	অন্যথা যদি (type1->tt_type == VAR_OBJECT)
	{
	    *dest = &t_object_any;
	    প্রত্যাগম;
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
    যদি (type->tt_type == VAR_TUPLE)
    {
	যদি (type->tt_argcount != 1)
	    প্রত্যাগম &t_any;

	যদি (type->tt_flags & TTFLAG_VARARGS)
	    প্রত্যাগম type->tt_args[0]->tt_member;
	অন্যথা
	    প্রত্যাগম type->tt_args[0];
    }

    // For "any" the item type is not known.
    প্রত্যাগম type->tt_member == রিক্ত ? &t_any : type->tt_member;
}

/*
 * Push an entry onto the type stack.  "type" used both for the current type
 * and the declared type.
 * Returns FAIL when out of memory.
 */
    পূর্ণাঙ্ক
ঠেলো_type_স্তূপ(cctx_T *cctx, type_T *type)
{
    প্রত্যাগম ঠেলো_type_stack2(cctx, type, type);
}

/*
 * Push an entry onto the type stack.  "type" is the current type, "decl_type"
 * is the declared type.
 * Returns FAIL when out of memory.
 */
    পূর্ণাঙ্ক
ঠেলো_type_stack2(cctx_T *cctx, type_T *type, type_T *decl_type)
{
    garray_T	*স্তূপ = &cctx->ctx_type_স্তূপ;
    type2_T	*typep;

    যদি (GA_GROW_FAILS(স্তূপ, 1))
	প্রত্যাগম FAIL;
    typep = ((type2_T *)স্তূপ->ga_data) + স্তূপ->ga_len;
    typep->type_curr = type;
    typep->type_decl = decl_type;
    ++স্তূপ->ga_len;
    প্রত্যাগম OK;
}

/*
 * Set the type of the top of the stack to "type".
 */
    শূন্য
set_type_on_স্তূপ(cctx_T *cctx, type_T *type, পূর্ণাঙ্ক offset)
{
    garray_T	*স্তূপ = &cctx->ctx_type_স্তূপ;
    type2_T	*typep = ((type2_T *)স্তূপ->ga_data)
						  + স্তূপ->ga_len - 1 - offset;

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
get_type_on_স্তূপ(cctx_T *cctx, পূর্ণাঙ্ক offset)
{
    garray_T	*স্তূপ = &cctx->ctx_type_স্তূপ;

    যদি (offset + 1 > স্তূপ->ga_len)
	প্রত্যাগম &t_unknown;
    প্রত্যাগম (((type2_T *)স্তূপ->ga_data) + স্তূপ->ga_len - offset - 1)
								   ->type_curr;
}

/*
 * Get the declared type from the type stack.  If "offset" is zero the one at
 * the top,
 * if "offset" is one the type above that, etc.
 * Returns &t_unknown if there is no such stack entry.
 */
    type_T *
get_decl_type_on_স্তূপ(cctx_T *cctx, পূর্ণাঙ্ক offset)
{
    garray_T	*স্তূপ = &cctx->ctx_type_স্তূপ;

    যদি (offset + 1 > স্তূপ->ga_len)
	প্রত্যাগম &t_unknown;
    প্রত্যাগম (((type2_T *)স্তূপ->ga_data) + স্তূপ->ga_len - offset - 1)
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
get_member_type_from_স্তূপ(
	পূর্ণাঙ্ক	    গুনতি,
	পূর্ণাঙ্ক	    skip,
	cctx_T	    *cctx)
{
    garray_T	*স্তূপ = &cctx->ctx_type_স্তূপ;
    type2_T	*typep;
    garray_T    *type_gap = cctx->ctx_type_list;
    পূর্ণাঙ্ক		i;
    type_T	*result;
    type_T	*type;

    // Use "unknown" for an empty list or dict.
    যদি (গুনতি == 0)
	প্রত্যাগম &t_unknown;
    // Find the common type from following items.
    typep = ((type2_T *)স্তূপ->ga_data) + স্তূপ->ga_len;
    result = &t_unknown;
    প্রতি (i = 0; i < গুনতি; ++i)
    {
	type = (typep -((গুনতি - i) * skip) + skip - 1)->type_curr;
	যদি (check_type_is_মান(type) == FAIL)
	    প্রত্যাগম রিক্ত;
	যদি (result != &t_any)
	    common_type(type, result, &result, type_gap);
    }

    প্রত্যাগম result;
}

/*
 * Get the types of items in a tuple on the stack of "cctx".
 * Returns the number of types.  Returns -1 on failure.
 */
    পূর্ণাঙ্ক
get_tuple_type_from_স্তূপ(
    পূর্ণাঙ্ক		গুনতি,
    garray_T	*tuple_types_gap,
    cctx_T	*cctx)
{
    garray_T	*স্তূপ = &cctx->ctx_type_স্তূপ;
    type2_T	*typep;
    type_T	*type = রিক্ত;

    // Use "unknown" for an empty tuple
    যদি (গুনতি == 0)
	প্রত্যাগম 0;

    // Find the common type from following items.
    typep = ((type2_T *)স্তূপ->ga_data) + স্তূপ->ga_len;
    প্রতি (পূর্ণাঙ্ক i = 0; i < গুনতি; i++)
    {
	type = (typep - (গুনতি - i))->type_curr;
	যদি (check_type_is_মান(type) == FAIL)
	    প্রত্যাগম -1;
	যদি (ga_grow(tuple_types_gap, 1) == FAIL)
	    প্রত্যাগম -1;
	((type_T **)tuple_types_gap->ga_data)[tuple_types_gap->ga_len] = type;
	tuple_types_gap->ga_len++;
    }

    প্রত্যাগম tuple_types_gap->ga_len;
}

    বর্ণ *
vartype_name(vartype_T type)
{
    বিকল্প (type)
    {
	পক্ষ VAR_UNKNOWN: ভঙ্গ;
	পক্ষ VAR_ANY: প্রত্যাগম "any";
	পক্ষ VAR_VOID: প্রত্যাগম "void";
	পক্ষ VAR_SPECIAL: প্রত্যাগম "special";
	পক্ষ VAR_BOOL: প্রত্যাগম "bool";
	পক্ষ VAR_সংখ্যা: প্রত্যাগম "number";
	পক্ষ VAR_FLOAT: প্রত্যাগম "float";
	পক্ষ VAR_STRING: প্রত্যাগম "string";
	পক্ষ VAR_BLOB: প্রত্যাগম "blob";
	পক্ষ VAR_JOB: প্রত্যাগম "job";
	পক্ষ VAR_CHANNEL: প্রত্যাগম "channel";
	পক্ষ VAR_LIST: প্রত্যাগম "list";
	পক্ষ VAR_TUPLE: প্রত্যাগম "tuple";
	পক্ষ VAR_DICT: প্রত্যাগম "dict";
	পক্ষ VAR_INSTR: প্রত্যাগম "instr";
	পক্ষ VAR_CLASS: প্রত্যাগম "class";
	পক্ষ VAR_OBJECT: প্রত্যাগম "object";
	পক্ষ VAR_TYPEALIAS: প্রত্যাগম "typealias";

	পক্ষ VAR_FUNC:
	পক্ষ VAR_PARTIAL: প্রত্যাগম "func";
    }
    প্রত্যাগম "unknown";
}

/*
 * Return the type name of a List (list<type>) or Dict (dict<type>).
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    স্থির বর্ণ *
type_name_list_or_dict(বর্ণ *name, type_T *type, বর্ণ **tofree)
{
    বর্ণ *member_free;
    বর্ণ *member_name;

    যদি (type->tt_member->tt_type == VAR_UNKNOWN)
	member_name = type_name(&t_any, &member_free);
    অন্যথা
	member_name = type_name(type->tt_member, &member_free);

    size_t len = STRLEN(name) + STRLEN(member_name) + 3;
    *tofree = alloc(len);
    যদি (*tofree == রিক্ত)
    {
	vim_free(member_free);
	প্রত্যাগম name;
    }

    vim_snprintf(*tofree, len, "%s<%s>", name, member_name);
    vim_free(member_free);
    প্রত্যাগম *tofree;
}

/*
 * Return the type name of a tuple.
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    স্থির বর্ণ *
type_name_tuple(type_T *type, বর্ণ **tofree)
{
    garray_T    ga;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;
    বর্ণ	*arg_free = রিক্ত;

    ga_init2(&ga, 1, 100);
    যদি (ga_grow(&ga, 20) == FAIL)
	গচ্ছ failed;
    STRCPY(ga.ga_data, "tuple<");
    ga.ga_len += 6;

    যদি (type->tt_argcount <= 0)
	// empty tuple
	GA_CONCAT_LITERAL(&ga, "any");
    অন্যথা
    {
	যদি (type->tt_args == রিক্ত)
	    GA_CONCAT_LITERAL(&ga, "[unknown]");
	অন্যথা
	{
	    প্রতি (i = 0; i < type->tt_argcount; ++i)
	    {
		string_T    arg_type;

		arg_type.string = (char_u *)type_name(type->tt_args[i], &arg_free);
		যদি (i > 0)
		{
		    STRCPY((বর্ণ *)ga.ga_data + ga.ga_len, ", ");
		    ga.ga_len += 2;
		}
		arg_type.length = STRLEN(arg_type.string);
		যদি (ga_grow(&ga, (পূর্ণাঙ্ক)arg_type.length + 8) == FAIL)
		    গচ্ছ failed;
		যদি (varargs && i == type->tt_argcount - 1)
		    GA_CONCAT_LITERAL(&ga, "...");
		ga_concat_len(&ga, arg_type.string, arg_type.length);
		VIM_CLEAR(arg_free);
	    }
	}
    }

    STRCPY((বর্ণ *)ga.ga_data + ga.ga_len, ">");
    *tofree = ga.ga_data;
    প্রত্যাগম ga.ga_data;

failed:
    vim_free(arg_free);
    ga_clear(&ga);
    প্রত্যাগম "[unknown]";
}

/*
 * Return the type name of a Class (class<name>) or Object (object<name>).
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    স্থির বর্ণ *
type_name_class_or_obj(বর্ণ *name, type_T *type, বর্ণ **tofree)
{
    string_T	class_name;

    যদি (type->tt_class != রিক্ত)
    {
	class_name.string = type->tt_class->class_name.string;
	class_name.length = type->tt_class->class_name.length;
	যদি (IS_ENUM(type->tt_class))
	    name = "enum";
    }
    অন্যথা
	STR_LITERAL_SET(class_name, "any");

    size_t len = STRLEN(name) + class_name.length + 3;
    *tofree = alloc(len);
    যদি (*tofree == রিক্ত)
	প্রত্যাগম name;

    vim_snprintf(*tofree, len, "%s<%s>", name, class_name.string);
    প্রত্যাগম *tofree;
}

/*
 * Return the type name of a function.
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    স্থির বর্ণ *
type_name_func(type_T *type, বর্ণ **tofree)
{
    garray_T    ga;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;
    বর্ণ	*arg_free = রিক্ত;

    ga_init2(&ga, 1, 100);
    যদি (ga_grow(&ga, 20) == FAIL)
	গচ্ছ failed;
    STRCPY(ga.ga_data, "func(");
    ga.ga_len += 5;

    প্রতি (i = 0; i < type->tt_argcount; ++i)
    {
	string_T    arg_type;

	যদি (type->tt_args == রিক্ত)
	    STR_LITERAL_SET(arg_type, "[unknown]");
	অন্যথা
	{
	    arg_type.string = (char_u *)type_name(type->tt_args[i], &arg_free);
	    arg_type.length = STRLEN(arg_type.string);
	}
	যদি (i > 0)
	{
	    STRCPY((বর্ণ *)ga.ga_data + ga.ga_len, ", ");
	    ga.ga_len += 2;
	}
	যদি (ga_grow(&ga, (পূর্ণাঙ্ক)arg_type.length + 8) == FAIL)
	    গচ্ছ failed;
	যদি (varargs && i == type->tt_argcount - 1)
	    GA_CONCAT_LITERAL(&ga, "...");
	অন্যথা যদি (i >= type->tt_min_argcount)
	    *((বর্ণ *)ga.ga_data + ga.ga_len++) = '?';
	ga_concat_len(&ga, arg_type.string, arg_type.length);
	VIM_CLEAR(arg_free);
    }
    যদি (type->tt_argcount < 0)
	// any number of arguments
	GA_CONCAT_LITERAL(&ga, "...");

    যদি (type->tt_member == &t_void)
	STRCPY((বর্ণ *)ga.ga_data + ga.ga_len, ")");
    অন্যথা
    {
	বর্ণ *ret_free = রিক্ত;
	বর্ণ *ret_name = type_name(type->tt_member, &ret_free);
	পূর্ণাঙ্ক  len;

	len = (পূর্ণাঙ্ক)STRLEN(ret_name) + 4;
	যদি (ga_grow(&ga, len) == FAIL)
	{
	    vim_free(ret_free);
	    গচ্ছ failed;
	}
	STRCPY((বর্ণ *)ga.ga_data + ga.ga_len, "): ");
	STRCPY((বর্ণ *)ga.ga_data + ga.ga_len + 3, ret_name);
	vim_free(ret_free);
    }
    *tofree = ga.ga_data;
    প্রত্যাগম ga.ga_data;

failed:
    vim_free(arg_free);
    ga_clear(&ga);
    প্রত্যাগম "[unknown]";
}

/*
 * Return the name of a type.
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    বর্ণ *
type_name(type_T *type, বর্ণ **tofree)
{
    বর্ণ *name;

    *tofree = রিক্ত;
    যদি (type == রিক্ত)
	প্রত্যাগম "[unknown]";
    name = vartype_name(type->tt_type);

    বিকল্প (type->tt_type)
    {
	পক্ষ VAR_LIST:
	পক্ষ VAR_DICT:
	    প্রত্যাগম type_name_list_or_dict(name, type, tofree);

	পক্ষ VAR_TUPLE:
	    প্রত্যাগম type_name_tuple(type, tofree);

	পক্ষ VAR_CLASS:
	পক্ষ VAR_OBJECT:
	    প্রত্যাগম type_name_class_or_obj(name, type, tofree);

	পক্ষ VAR_FUNC:
	    প্রত্যাগম type_name_func(type, tofree);

	স্বভাব:
	    ভঙ্গ;
    }

    প্রত্যাগম name;
}

/*
 * "typename(expr)" function
 */
    শূন্য
f_typename(typval_T *argvars, typval_T *rettv)
{
    garray_T	type_list;
    type_T	*type;
    বর্ণ	*tofree;
    বর্ণ	*name;

    rettv->v_type = VAR_STRING;
    ga_init2(&type_list, পরিমাণ(type_T *), 10);
    যদি (argvars[0].v_type == VAR_TYPEALIAS)
    {
	type = copy_type(argvars[0].vval.v_typealias->ta_type, &type_list);
	// A type alias for a class has the type set to VAR_OBJECT.  Change it
	// to VAR_CLASS, so that the name is "typealias<class<xxx>>"
	যদি (type->tt_type == VAR_OBJECT)
	    type->tt_type = VAR_CLASS;
    }
    অন্যথা
	type = typval2type(argvars, get_copyID(), &type_list, TVTT_DO_MEMBER);
    name = type_name(type, &tofree);
    যদি (argvars[0].v_type == VAR_TYPEALIAS)
    {
	vim_snprintf((বর্ণ *)IObuff, IOSIZE, "typealias<%s>", name);
	rettv->vval.v_string = vim_strsave((char_u *)IObuff);
	যদি (tofree != রিক্ত)
	    vim_free(tofree);
    }
    অন্যথা
    {
	যদি (tofree != রিক্ত)
	    rettv->vval.v_string = (char_u *)tofree;
	অন্যথা
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
    পূর্ণাঙ্ক
check_typval_is_মান(typval_T *tv)
{
    যদি (tv == রিক্ত)
	প্রত্যাগম OK;

    বিকল্প (tv->v_type)
    {
	পক্ষ VAR_CLASS:
	    {
		class_T *cl = tv->vval.v_class;
		char_u *class_name = (cl == রিক্ত) ? (char_u *)""
							: cl->class_name.string;
		যদি (cl != রিক্ত && IS_ENUM(cl))
		    semsg(_(e_using_enum_as_মান_str), class_name);
		অন্যথা
		    semsg(_(e_using_class_as_মান_str), class_name);
	    }
	    প্রত্যাগম FAIL;

	পক্ষ VAR_TYPEALIAS:
	    semsg(_(e_using_typealias_as_মান_str),
		    tv->vval.v_typealias->ta_name);
	    প্রত্যাগম FAIL;

	স্বভাব:
	    ভঙ্গ;
    }
    প্রত্যাগম OK;
}

/*
 * Same as above, except check type_T.
 */
    পূর্ণাঙ্ক
check_type_is_মান(type_T *type)
{
    যদি (type == রিক্ত)
	প্রত্যাগম OK;
    বিকল্প (type->tt_type)
    {
	পক্ষ VAR_CLASS:
	    যদি (type->tt_class != রিক্ত && IS_ENUM(type->tt_class))
		semsg(_(e_using_enum_as_মান_str),
			type->tt_class->class_name.string);
	    অন্যথা
		semsg(_(e_using_class_as_মান_str),
			type->tt_class == রিক্ত ? (char_u *)""
			: type->tt_class->class_name.string);
	    প্রত্যাগম FAIL;

	পক্ষ VAR_TYPEALIAS:
	    // TODO: Not sure what could be done here to get a name.
	    //       Maybe an optional argument?
	    emsg(_(e_using_typealias_as_var_val));
	    প্রত্যাগম FAIL;

	স্বভাব:
	    ভঙ্গ;
    }
    প্রত্যাগম OK;
}

#endif // FEAT_EVAL
