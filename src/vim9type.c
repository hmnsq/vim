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
	ফেরত ফাঁকা;
    type = ALLOC_CLEAR_ONE(type_T);
    যদি (type == ফাঁকা)
	ফেরত ফাঁকা;

    ((type_T **)type_gap->ga_তথ্য)[type_gap->ga_len] = type;
    ++type_gap->ga_len;
    ফেরত type;
}

/*
 * Make a shallow copy of "type".
 * When allocation fails returns "type".
 */
    type_T *
copy_type(type_T *type, garray_T *type_gap)
{
    type_T *copy = get_type_ptr(type_gap);

    যদি (copy == ফাঁকা)
	ফেরত type;
    *copy = *type;
    copy->tt_flags &= ~TTFLAG_STATIC;

    যদি (type->tt_args != ফাঁকা
	   && func_type_add_arg_types(copy, type->tt_argcount, type_gap) == OK)
	জন্য (পূর্ণসংখ্যা i = 0; i < type->tt_argcount; ++i)
	    copy->tt_args[i] = type->tt_args[i];

    ফেরত copy;
}

/*
 * Inner part of copy_type_deep().
 * When allocation fails returns "type".
 */
    স্থির type_T *
copy_type_deep_rec(type_T *type, garray_T *type_gap, garray_T *seen_types)
{
    জন্য (পূর্ণসংখ্যা i = 0; i < seen_types->ga_len; ++i)
	যদি (((type_T **)seen_types->ga_তথ্য)[i * 2] == type)
	    // seen this type before, return the copy we made
	    ফেরত ((type_T **)seen_types->ga_তথ্য)[i * 2 + 1];

    type_T *copy = copy_type(type, type_gap);
    যদি (ga_grow(seen_types, 1) == FAIL)
	ফেরত copy;
    ((type_T **)seen_types->ga_তথ্য)[seen_types->ga_len * 2] = type;
    ((type_T **)seen_types->ga_তথ্য)[seen_types->ga_len * 2 + 1] = copy;
    ++seen_types->ga_len;

    যদি (copy->tt_member != ফাঁকা)
	copy->tt_member = copy_type_deep_rec(copy->tt_member,
							 type_gap, seen_types);
    যদি (type->tt_args != ফাঁকা)
	জন্য (পূর্ণসংখ্যা i = 0; i < type->tt_argcount; ++i)
	    copy->tt_args[i] = copy_type_deep_rec(copy->tt_args[i],
							 type_gap, seen_types);

    ফেরত copy;
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
    ga_init2(&seen_types, আকার(type_T *) * 2, 20);

    type_T *res = copy_type_deep_rec(type, type_gap, &seen_types);

    ga_clear(&seen_types);
    ফেরত res;
}

    শূন্য
clear_type_list(garray_T *gap)
{
    যখন (gap->ga_len > 0)
	vim_free(((type_T **)gap->ga_তথ্য)[--gap->ga_len]);
    ga_clear(gap);
}

    শূন্য
clear_func_type_list(garray_T *gap, type_T **func_type)
{
    যখন (gap->ga_len > 0)
    {
	// func_type pointing to the uf_type_list, so reset pointer
	যদি (*func_type == ((type_T **)gap->ga_তথ্য)[--gap->ga_len])
	    *func_type = &t_func_any;
	vim_free(((type_T **)gap->ga_তথ্য)[gap->ga_len]);
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

    যদি (type == ফাঁকা)
	ফেরত ফাঁকা;

    // A fixed type never contains allocated types, return as-is.
    যদি (type->tt_flags & TTFLAG_STATIC)
	ফেরত type;

    ret = ALLOC_ONE(type_T);
    যদি (ret == ফাঁকা)
	ফেরত ফাঁকা;
    *ret = *type;

    যদি (ret->tt_member != ফাঁকা)
	ret->tt_member = alloc_type(ret->tt_member);

    যদি (type->tt_argcount > 0 && type->tt_args != ফাঁকা)
    {
	পূর্ণসংখ্যা i;

	ret->tt_args = ALLOC_MULT(type_T *, type->tt_argcount);
	যদি (ret->tt_args != ফাঁকা)
	    জন্য (i = 0; i < type->tt_argcount; ++i)
		ret->tt_args[i] = alloc_type(type->tt_args[i]);
    }
    অন্যথা
	ret->tt_args = ফাঁকা;

    ফেরত ret;
}

/*
 * Free a type that was created with alloc_type().
 */
    শূন্য
free_type(type_T *type)
{
    পূর্ণসংখ্যা i;

    যদি (type == ফাঁকা || (type->tt_flags & TTFLAG_STATIC))
	ফেরত;
    যদি (type->tt_args != ফাঁকা)
    {
	জন্য (i = 0; i < type->tt_argcount; ++i)
	    free_type(type->tt_args[i]);
	vim_free(type->tt_args);
    }

    free_type(type->tt_member);

    vim_free(type);
}

/*
 * Return TRUE if "type" is to be recursed into for setting the type.
 */
    স্থির পূর্ণসংখ্যা
set_tv_type_recurse(type_T *type)
{
    ফেরত type->tt_member != ফাঁকা
		&& (type->tt_member->tt_type == VAR_DICT
				|| type->tt_member->tt_type == VAR_LIST)
		&& type->tt_member->tt_member != ফাঁকা
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
	ফেরত;

    free_type(d->dv_type);
    d->dv_type = alloc_type(type);

    // Need to recursively set the type of dict items?
    যদি (!set_tv_type_recurse(type))
	ফেরত;

    পূর্ণসংখ্যা		todo = (পূর্ণসংখ্যা)d->dv_hashtab.ht_used;
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
	ফেরত;

    free_type(l->lv_type);
    l->lv_type = alloc_type(type);

    // Need to recursively set the type of list items?
    যদি (l->lv_first == &range_list_item || !set_tv_type_recurse(type))
	ফেরত;

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
	ফেরত;

    free_type(tuple->tv_type);
    tuple->tv_type = alloc_type(type);

    যদি (type->tt_argcount <= 0)
	ফেরত;

    // recursively set the type of list items
    type_T	*item_type;
    জন্য (পূর্ণসংখ্যা i = 0; i < tuple_len(tuple); i++)
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
	ফেরত;

    যদি (tv->v_type == VAR_DICT && tv->vval.v_dict != ফাঁকা)
	set_tv_type_dict(tv->vval.v_dict, type);
    অন্যথা যদি (tv->v_type == VAR_LIST && tv->vval.v_list != ফাঁকা)
	set_tv_type_list(tv->vval.v_list, type);
    অন্যথা যদি (tv->v_type == VAR_TUPLE && tv->vval.v_tuple != ফাঁকা)
	set_tv_type_tuple(tv->vval.v_tuple, type);
}

/*
 * For a tuple type, reserve space for "typecount" types (including the
 * repeated type).
 */
    স্থির পূর্ণসংখ্যা
tuple_type_add_types(
    type_T	*tupletype,
    পূর্ণসংখ্যা		typecount,
    garray_T	*type_gap)
{
    // To make it easy to free the space needed for the types, add the
    // pointer to type_gap.
    যদি (ga_grow(type_gap, 1) == FAIL)
	ফেরত FAIL;
    tupletype->tt_args = alloc_clear_id(আকার(type_T *) * typecount,
							  aid_tuple_type_args);
    যদি (tupletype->tt_args == ফাঁকা)
	ফেরত FAIL;
    ((type_T **)type_gap->ga_তথ্য)[type_gap->ga_len] =
						(শূন্য *)tupletype->tt_args;
    ++type_gap->ga_len;
    ফেরত OK;
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
    যদি (member_type == ফাঁকা || (member_type->tt_type == VAR_ANY
					&& !IS_GENERIC_TYPE(member_type)))
	ফেরত &t_list_any;
    যদি (member_type->tt_type == VAR_VOID
	    || member_type->tt_type == VAR_UNKNOWN)
	ফেরত &t_list_empty;
    যদি (member_type->tt_type == VAR_BOOL)
	ফেরত &t_list_bool;
    যদি (member_type->tt_type == VAR_সংখ্যা)
	ফেরত &t_list_সংখ্যা;
    যদি (member_type->tt_type == VAR_STRING)
	ফেরত &t_list_string;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    যদি (type == ফাঁকা)
	ফেরত &t_any;
    type->tt_type = VAR_LIST;
    type->tt_member = member_type;
    type->tt_argcount = 0;
    type->tt_args = ফাঁকা;
    ফেরত type;
}

/*
 * Create and return a tuple type from the tuple item types in
 * "tuple_types_ga".
 */
    type_T *
get_tuple_type(garray_T *tuple_types_gap, garray_T *type_gap)
{
    type_T	*type;
    type_T	**tuple_types = tuple_types_gap->ga_তথ্য;
    পূর্ণসংখ্যা		typecount = tuple_types_gap->ga_len;

    // recognize commonly used types
    যদি (typecount == 0)
	ফেরত &t_tuple_any;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    যদি (type == ফাঁকা)
	ফেরত &t_any;
    type->tt_type = VAR_TUPLE;
    type->tt_member = ফাঁকা;
    যদি (typecount > 0)
    {
	যদি (tuple_type_add_types(type, typecount, type_gap) == FAIL)
	    ফেরত &t_any;
	mch_memmove(type->tt_args, tuple_types, আকার(type_T *) * typecount);
    }
    type->tt_argcount = typecount;
    type->tt_flags = 0;

    ফেরত type;
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
    যদি (member_type == ফাঁকা || (member_type->tt_type == VAR_ANY
					&& !IS_GENERIC_TYPE(member_type)))
	ফেরত &t_dict_any;
    যদি (member_type->tt_type == VAR_VOID
	    || member_type->tt_type == VAR_UNKNOWN)
	ফেরত &t_dict_empty;
    যদি (member_type->tt_type == VAR_BOOL)
	ফেরত &t_dict_bool;
    যদি (member_type->tt_type == VAR_সংখ্যা)
	ফেরত &t_dict_সংখ্যা;
    যদি (member_type->tt_type == VAR_STRING)
	ফেরত &t_dict_string;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    যদি (type == ফাঁকা)
	ফেরত &t_any;
    type->tt_type = VAR_DICT;
    type->tt_member = member_type;
    type->tt_argcount = 0;
    type->tt_args = ফাঁকা;
    ফেরত type;
}

/*
 * Allocate a new type for a function.
 */
    type_T *
alloc_func_type(type_T *ret_type, পূর্ণসংখ্যা argcount, garray_T *type_gap)
{
    type_T *type = get_type_ptr(type_gap);

    যদি (type == ফাঁকা)
	ফেরত &t_any;
    type->tt_type = VAR_FUNC;
    type->tt_member = ret_type == ফাঁকা ? &t_unknown : ret_type;
    type->tt_argcount = argcount;
    type->tt_args = ফাঁকা;
    ফেরত type;
}

/*
 * Allocate a new type for a tuple.
 */
    স্থির type_T *
alloc_tuple_type(পূর্ণসংখ্যা typecount, garray_T *type_gap)
{
    type_T *type = get_type_ptr(type_gap);

    যদি (type == ফাঁকা)
	ফেরত &t_any;
    type->tt_type = VAR_TUPLE;
    type->tt_member = ফাঁকা;
    type->tt_argcount = typecount;
    type->tt_args = ফাঁকা;
    ফেরত type;
}

/*
 * Get a function type, based on the return type "ret_type".
 * "argcount" must be -1 or 0, a predefined type can be used.
 */
    type_T *
get_func_type(type_T *ret_type, পূর্ণসংখ্যা argcount, garray_T *type_gap)
{
    // recognize commonly used types
    যদি (ret_type == &t_unknown || ret_type == ফাঁকা)
    {
	// (argcount == 0) is not possible
	ফেরত &t_func_unknown;
    }
    যদি (ret_type == &t_void)
    {
	যদি (argcount == 0)
	    ফেরত &t_func_0_void;
	অন্যথা
	    ফেরত &t_func_void;
    }
    যদি (ret_type == &t_any)
    {
	যদি (argcount == 0)
	    ফেরত &t_func_0_any;
	অন্যথা
	    ফেরত &t_func_any;
    }
    যদি (ret_type == &t_সংখ্যা)
    {
	যদি (argcount == 0)
	    ফেরত &t_func_0_সংখ্যা;
	অন্যথা
	    ফেরত &t_func_সংখ্যা;
    }
    যদি (ret_type == &t_string)
    {
	যদি (argcount == 0)
	    ফেরত &t_func_0_string;
	অন্যথা
	    ফেরত &t_func_string;
    }

    ফেরত alloc_func_type(ret_type, argcount, type_gap);
}

/*
 * For a function type, reserve space for "argcount" argument types (including
 * vararg).
 */
    পূর্ণসংখ্যা
func_type_add_arg_types(
	type_T	    *functype,
	পূর্ণসংখ্যা	    argcount,
	garray_T    *type_gap)
{
    // To make it easy to free the space needed for the argument types, add the
    // pointer to type_gap.
    যদি (ga_grow(type_gap, 1) == FAIL)
	ফেরত FAIL;
    functype->tt_args = ALLOC_CLEAR_MULT(type_T *, argcount);
    যদি (functype->tt_args == ফাঁকা)
	ফেরত FAIL;
    ((type_T **)type_gap->ga_তথ্য)[type_gap->ga_len] =
						     (শূন্য *)functype->tt_args;
    ++type_gap->ga_len;
    ফেরত OK;
}

/*
 * Return TRUE if "type" is NULL, any or unknown.
 * This also works for const (comparing with &t_any and &t_unknown doesn't).
 */
    পূর্ণসংখ্যা
type_any_or_unknown(type_T *type)
{
    ফেরত type == ফাঁকা || type->tt_type == VAR_ANY
					|| type->tt_type == VAR_UNKNOWN;
}

/*
 * Get a type_T for a "special" typval in "tv".
 */
    স্থির type_T *
special_typval2type(typval_T *tv)
{
    নির্বাচন (tv->vval.v_সংখ্যা)
    {
	ক্ষেত্র VVAL_NULL:
	    ফেরত &t_null;

	ক্ষেত্র VVAL_NONE:
	    ফেরত &t_none;

	ক্ষেত্র VVAL_TRUE:
	ক্ষেত্র VVAL_FALSE:
	    ফেরত &t_bool;

	পূর্বনির্ধারিত:
	    ফেরত &t_unknown;
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
list_typval2type(typval_T *tv, পূর্ণসংখ্যা copyID, garray_T *type_gap, পূর্ণসংখ্যা flags)
{
    list_T	*l = tv->vval.v_list;
    listitem_T	*li;
    type_T	*member_type = ফাঁকা;

    // An empty list has type list<unknown>, unless the type was specified
    // and is not list<any>.  This matters when assigning to a variable
    // with a specific list type.
    যদি (l == ফাঁকা || (l->lv_first == ফাঁকা
		&& (l->lv_type == ফাঁকা || l->lv_type->tt_member == &t_any)))
	ফেরত &t_list_empty;

    যদি ((flags & TVTT_DO_MEMBER) == 0)
	ফেরত &t_list_any;

    // If the type is list<any> go through the members, it may end up a
    // more specific type.
    যদি (l->lv_type != ফাঁকা && (l->lv_first == ফাঁকা
		|| (flags & TVTT_MORE_SPECIFIC) == 0
		|| l->lv_type->tt_member != &t_any))
	// make a copy, lv_type may be freed if the list is freed
	ফেরত copy_type_deep(l->lv_type, type_gap);

    যদি (l->lv_first == &range_list_item)
	ফেরত &t_list_সংখ্যা;

    যদি (l->lv_copyID == copyID)
	// avoid recursion
	ফেরত &t_list_any;

    l->lv_copyID = copyID;

    // Use the common type of all members.
    member_type = typval2type(&l->lv_first->li_tv, copyID, type_gap,
							TVTT_DO_MEMBER);
    জন্য (li = l->lv_first->li_পরবর্তী; li != ফাঁকা; li = li->li_পরবর্তী)
	common_type(typval2type(&li->li_tv, copyID, type_gap, TVTT_DO_MEMBER),
					member_type, &member_type, type_gap);

    // Reset copyID so that a shared reference to this list (not a circular
    // reference) can be processed again to get the correct type.
    l->lv_copyID = 0;

    ফেরত get_list_type(member_type, type_gap);
}

/*
 * Get a type_T for a Tuple typval in "tv".
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    স্থির type_T *
tuple_typval2type(typval_T *tv, পূর্ণসংখ্যা copyID, garray_T *type_gap, পূর্ণসংখ্যা flags)
{
    tuple_T	*tuple = tv->vval.v_tuple;
    পূর্ণসংখ্যা		len = tuple_len(tuple);
    type_T	*type = ফাঁকা;

    // An empty tuple has type tuple<unknown>, unless the type was specified
    // and is not tuple<any>.  This matters when assigning to a variable
    // with a specific tuple type.
    যদি (tuple == ফাঁকা || (len == 0 && (tuple->tv_type == ফাঁকা
				|| tuple->tv_type->tt_argcount == 0)))
	ফেরত &t_tuple_empty;

    যদি ((flags & TVTT_DO_MEMBER) == 0)
	ফেরত &t_tuple_any;

    // If the type is tuple<any> go through the members, it may end up a
    // more specific type.
    যদি (tuple->tv_type != ফাঁকা && (len == 0
					|| (flags & TVTT_MORE_SPECIFIC) == 0))
	// make a copy, tv_type may be freed if the tuple is freed
	ফেরত copy_type_deep(tuple->tv_type, type_gap);

    যদি (tuple->tv_copyID == copyID)
	// avoid recursion
	ফেরত &t_tuple_any;

    tuple->tv_copyID = copyID;

    garray_T	tuple_types_ga;
    ga_init2(&tuple_types_ga, আকার(type_T *), 10);
    জন্য (পূর্ণসংখ্যা i = 0; i < len; i++)
    {
	type = typval2type(TUPLE_ITEM(tuple, i), copyID, type_gap,
							TVTT_DO_MEMBER);
	যদি (ga_grow(&tuple_types_ga, 1) == FAIL)
	{
	    ga_clear(&tuple_types_ga);
	    // Reset copyID so that a shared reference to this tuple can be
	    // processed again.
	    tuple->tv_copyID = 0;
	    ফেরত ফাঁকা;
	}
	((type_T **)tuple_types_ga.ga_তথ্য)[tuple_types_ga.ga_len] = type;
	tuple_types_ga.ga_len++;
    }

    type_T *tuple_type = get_tuple_type(&tuple_types_ga, type_gap);
    ga_clear(&tuple_types_ga);

    // Reset copyID so that a shared reference to this tuple (not a circular
    // reference) can be processed again to get the correct type.
    tuple->tv_copyID = 0;

    ফেরত tuple_type;
}

/*
 * Get a type_T for a Dict typval in "tv".
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    স্থির type_T *
dict_typval2type(typval_T *tv, পূর্ণসংখ্যা copyID, garray_T *type_gap, পূর্ণসংখ্যা flags)
{
    dict_iterator_T	iter;
    typval_T		*মান;
    dict_T		*d = tv->vval.v_dict;
    type_T		*member_type = ফাঁকা;

    যদি (d == ফাঁকা || (d->dv_hashtab.ht_used == 0 && d->dv_type == ফাঁকা))
	ফেরত &t_dict_empty;

    যদি ((flags & TVTT_DO_MEMBER) == 0)
	ফেরত &t_dict_any;

    // If the type is dict<any> go through the members, it may end up a
    // more specific type.
    যদি (d->dv_type != ফাঁকা && (d->dv_hashtab.ht_used == 0
		|| (flags & TVTT_MORE_SPECIFIC) == 0
		|| d->dv_type->tt_member != &t_any))
	ফেরত d->dv_type;

    যদি (d->dv_copyID == copyID)
	// avoid recursion
	ফেরত &t_dict_any;

    d->dv_copyID = copyID;

    // Use the common type of all values.
    dict_iterate_start(tv, &iter);
    dict_iterate_পরবর্তী(&iter, &মান);
    member_type = typval2type(মান, copyID, type_gap, TVTT_DO_MEMBER);

    যখন (dict_iterate_পরবর্তী(&iter, &মান) != ফাঁকা)
	common_type(typval2type(মান, copyID, type_gap, TVTT_DO_MEMBER),
					member_type, &member_type, type_gap);

    // Reset copyID so that a shared reference to this dict (not a circular
    // reference) can be processed again to get the correct type.
    d->dv_copyID = 0;

    ফেরত get_dict_type(member_type, type_gap);
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
    যদি (type == ফাঁকা)
	ফেরত ফাঁকা;

    *type = *ufunc->uf_func_type;
    যদি (type->tt_argcount >= 0 && pt->pt_argc > 0)
    {
	type->tt_argcount -= pt->pt_argc;
	type->tt_min_argcount -= pt->pt_argc;
	যদি (type->tt_argcount > 0 && func_type_add_arg_types(type,
		    type->tt_argcount, type_gap) == OK)
	    জন্য (পূর্ণসংখ্যা i = 0; i < type->tt_argcount; ++i)
		type->tt_args[i] =
		    ufunc->uf_func_type->tt_args[i + pt->pt_argc];
    }
    যদি (pt->pt_func != ফাঁকা)
	type->tt_member = pt->pt_func->uf_ret_type;

    ফেরত type;
}

/*
 * Get a type_T for a "class" or an "object" typval in "tv".
 */
    স্থির type_T *
oc_typval2type(typval_T *tv)
{
    যদি (tv->v_type == VAR_CLASS)
    {
	যদি (tv->vval.v_class == ফাঁকা)
	    ফেরত &t_class;

	ফেরত &tv->vval.v_class->class_type;
    }

    যদি (tv->vval.v_object != ফাঁকা)
	ফেরত &tv->vval.v_object->obj_class->class_object_type;

    ফেরত &t_object_any;
}

/*
 * Get a type_T for a "function" or a "partial"
 */
    স্থির type_T *
fp_typval2type(typval_T *tv, garray_T *type_gap)
{
    char_u	*name = ফাঁকা;
    ufunc_T	*ufunc = ফাঁকা;
    type_T	*type;
    type_T	*member_type = ফাঁকা;
    পূর্ণসংখ্যা		argcount = 0;
    পূর্ণসংখ্যা		min_argcount = 0;

    যদি (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != ফাঁকা)
    {
	যদি (tv->vval.v_partial->pt_func != ফাঁকা)
	    ufunc = tv->vval.v_partial->pt_func;
	অন্যথা
	    name = tv->vval.v_partial->pt_name;
    }
    অন্যথা
	name = tv->vval.v_string;

    যদি (name == ফাঁকা && ufunc == ফাঁকা)
	ফেরত &t_func_unknown;

    যদি (name != ফাঁকা)
    {
	পূর্ণসংখ্যা idx = find_internal_func(name);

	যদি (idx >= 0)
	{
	    type_T *decl_type;  // unused

	    internal_func_get_argcount(idx, &argcount, &min_argcount);
	    member_type = internal_func_ret_type(idx, 0, ফাঁকা, &decl_type,
		    type_gap);
	}
	অন্যথা
	{
	    // Check if name contains "<".  If it does, then replace "<" with
	    // NUL and lookup the function and then look up the specific
	    // generic function using the supplied types.
	    char_u *p = generic_func_find_open_bracket(name);
	    যদি (p == ফাঁকা)
		ufunc = find_func(name, FALSE);
	    অন্যথা
	    {
		// generic function
		char_u	cc = *p;

		*p = NUL;
		ufunc = find_func(name, FALSE);
		*p = cc;
		যদি (ufunc != ফাঁকা && IS_GENERIC_FUNC(ufunc))
		{
		    ufunc = find_generic_func(ufunc, name, &p);
		    যদি (ufunc == ফাঁকা)
			ফেরত ফাঁকা;
		}
	    }
	}
    }
    যদি (ufunc != ফাঁকা)
    {
	// May need to get the argument types from default values by
	// compiling the function.
	যদি (ufunc->uf_def_status == UF_TO_BE_COMPILED
		&& compile_def_function(ufunc, TRUE, CT_NONE, ফাঁকা)
		== FAIL)
	    ফেরত ফাঁকা;
	যদি (ufunc->uf_func_type == ফাঁকা)
	    set_function_type(ufunc);
	যদি (ufunc->uf_func_type != ফাঁকা)
	{
	    যদি (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != ফাঁকা)
		ফেরত partial_typval2type(tv, ufunc, type_gap);
	    ফেরত ufunc->uf_func_type;
	}
    }

    type = get_type_ptr(type_gap);
    যদি (type == ফাঁকা)
	ফেরত ফাঁকা;
    type->tt_type = tv->v_type;
    type->tt_argcount = argcount;
    type->tt_min_argcount = min_argcount;
    যদি (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != ফাঁকা
					&& tv->vval.v_partial->pt_argc > 0)
    {
	type->tt_argcount -= tv->vval.v_partial->pt_argc;
	type->tt_min_argcount -= tv->vval.v_partial->pt_argc;
    }
    type->tt_member = member_type;

    ফেরত type;
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
typval2type_int(typval_T *tv, পূর্ণসংখ্যা copyID, garray_T *type_gap, পূর্ণসংখ্যা flags)
{
    নির্বাচন (tv->v_type)
    {
	ক্ষেত্র VAR_UNKNOWN:
	    ফেরত &t_unknown;

	ক্ষেত্র VAR_ANY:
	    ফেরত &t_any;

	ক্ষেত্র VAR_VOID:
	    ফেরত &t_void;

	ক্ষেত্র VAR_BOOL:
	    ফেরত &t_bool;

	ক্ষেত্র VAR_SPECIAL:
	    ফেরত special_typval2type(tv);

	ক্ষেত্র VAR_সংখ্যা:
	    ফেরত &t_সংখ্যা;

	ক্ষেত্র VAR_FLOAT:
	    ফেরত &t_float;

	ক্ষেত্র VAR_STRING:
	    ফেরত &t_string;

	ক্ষেত্র VAR_BLOB:
	    যদি (tv->vval.v_blob == ফাঁকা)
		ফেরত &t_blob_null;
	    ফেরত &t_blob;

	ক্ষেত্র VAR_LIST:
	    ফেরত list_typval2type(tv, copyID, type_gap, flags);

	ক্ষেত্র VAR_TUPLE:
	    ফেরত tuple_typval2type(tv, copyID, type_gap, flags);

	ক্ষেত্র VAR_DICT:
	    ফেরত dict_typval2type(tv, copyID, type_gap, flags);

	ক্ষেত্র VAR_JOB:
	    ফেরত &t_job;

	ক্ষেত্র VAR_CHANNEL:
	    ফেরত &t_channel;

	ক্ষেত্র VAR_CLASS:
	ক্ষেত্র VAR_OBJECT:
	    ফেরত oc_typval2type(tv);

	ক্ষেত্র VAR_TYPEALIAS:
	    ফেরত &t_typealias;

	ক্ষেত্র VAR_FUNC:
	ক্ষেত্র VAR_PARTIAL:
	    ফেরত fp_typval2type(tv, type_gap);

	ক্ষেত্র VAR_INSTR:
	পূর্বনির্ধারিত:
	    বিরতি;
    }

    ফেরত ফাঁকা;
}

/*
 * Return TRUE if "tv" is not a bool but should be converted to bool.
 */
    পূর্ণসংখ্যা
need_convert_to_bool(type_T *type, typval_T *tv)
{
    ফেরত type != ফাঁকা && type == &t_bool && tv->v_type != VAR_BOOL
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
typval2type(typval_T *tv, পূর্ণসংখ্যা copyID, garray_T *type_gap, পূর্ণসংখ্যা flags)
{
    type_T *type = typval2type_int(tv, copyID, type_gap, flags);

    যদি (type == ফাঁকা)
	ফেরত ফাঁকা;

    যদি (type != &t_bool && (tv->v_type == VAR_সংখ্যা
		&& (tv->vval.v_সংখ্যা == 0 || tv->vval.v_সংখ্যা == 1)))
	// Number 0 and 1 and expression with "&&" or "||" can also be used
	// for bool.
	type = &t_সংখ্যা_bool;
    অন্যথা যদি (type != &t_float && tv->v_type == VAR_সংখ্যা)
	// A number can also be used for float.
	type = &t_সংখ্যা_float;
    ফেরত type;
}

/*
 * Return TRUE if "type" can be used for a variable declaration.
 * Give an error and return FALSE if not.
 */
    পূর্ণসংখ্যা
valid_declaration_type(type_T *type)
{
    যদি (type->tt_type == VAR_SPECIAL  // null, none
	    || type->tt_type == VAR_VOID)
    {
	অক্ষর *tofree = ফাঁকা;
	অক্ষর *name = type_name(type, &tofree);
	semsg(_(e_invalid_type_in_variable_declaration_str), name);
	vim_free(tofree);
	ফেরত FALSE;
    }
    ফেরত TRUE;
}

/*
 * Get a type_T for a typval_T, used for v: variables.
 * "type_list" is used to temporarily create types in.
 */
    type_T *
typval2type_vimvar(typval_T *tv, garray_T *type_gap)
{
    যদি (tv->v_type == VAR_LIST)  // e.g. for v:oldfiles
	ফেরত &t_list_string;
    যদি (tv->v_type == VAR_DICT)  // e.g. for v:event
	ফেরত &t_dict_any;
    ফেরত typval2type(tv, get_copyID(), type_gap, TVTT_DO_MEMBER);
}

    পূর্ণসংখ্যা
check_typval_arg_type(
	type_T	    *expected,
	typval_T    *actual_tv,
	অক্ষর	    *func_name,
	পূর্ণসংখ্যা	    arg_idx)
{
    where_T	where = WHERE_INIT;

    যদি (arg_idx > 0)
    {
	where.wt_সূচক = arg_idx;
	where.wt_kind = WT_ARGUMENT;
    }
    where.wt_func_name = func_name;
    ফেরত check_typval_type(expected, actual_tv, where);
}

/*
 * Return true if "expected" is list<any> or dict<any> and "actual_tv" is a
 * list or dict of that kind, which matches whatever its values are.
 */
    স্থির bool
matches_listdict_any(type_T *expected, typval_T *actual_tv)
{
    ফেরত (expected->tt_type == VAR_LIST || expected->tt_type == VAR_DICT)
	    && actual_tv->v_type == expected->tt_type
	    && expected->tt_member != ফাঁকা
	    && (expected->tt_member->tt_type == VAR_ANY
		|| expected->tt_member->tt_type == VAR_UNKNOWN);
}

/*
 * Return FAIL if "expected" and "actual" don't match.
 * When "argidx" > 0 it is included in the error message.
 */
    পূর্ণসংখ্যা
check_typval_type(type_T *expected, typval_T *actual_tv, where_T where)
{
    garray_T	type_list;
    type_T	*actual_type;
    পূর্ণসংখ্যা		res = FAIL;

    যদি (expected == ফাঁকা)
	ফেরত OK;  // didn't expect anything.

    // No need to go through the values of a possibly big list or dict.
    যদি (matches_listdict_any(expected, actual_tv))
	ফেরত OK;

    ga_init2(&type_list, আকার(type_T *), 10);

    // A null_function and null_partial are special cases, they can be used to
    // clear a variable.
    যদি ((actual_tv->v_type == VAR_FUNC && actual_tv->vval.v_string == ফাঁকা)
	    || (actual_tv->v_type == VAR_PARTIAL
					 && actual_tv->vval.v_partial == ফাঁকা))
	actual_type = &t_func_unknown;
    অন্যথা
	// When the actual type is list<any> or dict<any> go through the values
	// to possibly get a more specific type.
	actual_type = typval2type(actual_tv, get_copyID(), &type_list,
					  TVTT_DO_MEMBER | TVTT_MORE_SPECIFIC);
    যদি (actual_type != ফাঁকা)
    {
	res = check_type_maybe(expected, actual_type, TRUE, where);
	যদি (res == MAYBE && !(actual_type->tt_type == VAR_FUNC
				      && (actual_type->tt_member == &t_unknown
					  || actual_type->tt_member == ফাঁকা)))
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
    ফেরত res;
}

    শূন্য
arg_type_mismatch(type_T *expected, type_T *actual, পূর্ণসংখ্যা arg_idx)
{
    where_T	where = WHERE_INIT;

    যদি (arg_idx > 0)
    {
	where.wt_সূচক = arg_idx;
	where.wt_kind = WT_ARGUMENT;
    }
    type_mismatch_where(expected, actual, where);
}

    শূন্য
type_mismatch_where(type_T *expected, type_T *actual, where_T where)
{
    অক্ষর *tofree1, *tofree2;
    অক্ষর *typename1 = type_name(expected, &tofree1);
    অক্ষর *typename2 = type_name(actual, &tofree2);

    নির্বাচন (where.wt_kind)
    {
	ক্ষেত্র WT_MEMBER:
	    semsg(_(e_variable_str_type_mismatch_expected_str_but_got_str),
		    where.wt_func_name, typename1, typename2);
	    বিরতি;
	ক্ষেত্র WT_METHOD:
	ক্ষেত্র WT_METHOD_ARG:
	ক্ষেত্র WT_METHOD_RETURN:
	    semsg(_(e_method_str_type_mismatch_expected_str_but_got_str),
		    where.wt_func_name, typename1, typename2);
	    বিরতি;
	ক্ষেত্র WT_VARIABLE:
	    যদি (where.wt_func_name == ফাঁকা)
		semsg(_(e_variable_nr_type_mismatch_expected_str_but_got_str),
			where.wt_সূচক, typename1, typename2);
	    অন্যথা
		semsg(_(e_variable_nr_type_mismatch_expected_str_but_got_str_in_str),
			where.wt_সূচক, typename1, typename2, where.wt_func_name);
	    বিরতি;
	ক্ষেত্র WT_ARGUMENT:
	    যদি (where.wt_func_name == ফাঁকা)
		semsg(_(e_argument_nr_type_mismatch_expected_str_but_got_str),
			where.wt_সূচক, typename1, typename2);
	    অন্যথা
		semsg(_(e_argument_nr_type_mismatch_expected_str_but_got_str_in_str),
			where.wt_সূচক, typename1, typename2, where.wt_func_name);
	    বিরতি;
	ক্ষেত্র WT_CAST:
	ক্ষেত্র WT_UNKNOWN:
	    যদি (where.wt_func_name == ফাঁকা)
		semsg(_(e_type_mismatch_expected_str_but_got_str),
			typename1, typename2);
	    অন্যথা
		semsg(_(e_type_mismatch_expected_str_but_got_str_in_str),
			typename1, typename2, where.wt_func_name);
	    বিরতি;
    }

    vim_free(tofree1);
    vim_free(tofree2);
}

/*
 * Check if the expected and actual types match for a tuple
 */
    স্থির পূর্ণসংখ্যা
check_tuple_type_maybe(
    type_T	*expected,
    type_T	*actual,
    where_T	where)
{
    যদি (expected->tt_argcount == -1 || actual->tt_argcount == -1
		|| expected->tt_args == ফাঁকা || actual->tt_args == ফাঁকা)
	ফেরত OK;

    // For a non-variadic tuple, the number of items must match
    যদি (!(expected->tt_flags & TTFLAG_VARARGS)
	    && expected->tt_argcount != actual->tt_argcount)
	ফেরত FAIL;

    // compare the type of each tuple item
    জন্য (পূর্ণসংখ্যা i = 0; i < actual->tt_argcount; ++i)
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
	    ফেরত FAIL;
    }

    ফেরত OK;
}

/*
 * Check if the expected and actual types match for a function
 * Returns OK if "expected" and "actual" are matching function types.
 * Returns FAIL if "expected" and "actual" are different types.
 * Returns MAYBE when a runtime type check is needed.
 */
    স্থির পূর্ণসংখ্যা
check_func_type_maybe(
    type_T	*expected,
    type_T	*actual,
    where_T	where)
{
    পূর্ণসংখ্যা ret = OK;

    // If the return type is unknown it can be anything, including
    // nothing, thus there is no point in checking.
    যদি (expected->tt_member != &t_unknown)
    {
	যদি (actual->tt_member != ফাঁকা
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
    যদি (ret != FAIL && expected->tt_args != ফাঁকা
	    && actual->tt_args != ফাঁকা)
    {
	পূর্ণসংখ্যা i;

	জন্য (i = 0; i < expected->tt_argcount
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
		বিরতি;
	    }
	}
    }
    যদি (ret == OK && expected->tt_argcount >= 0
	    && actual->tt_argcount == -1)
	// check the argument count at runtime
	ret = MAYBE;

    ফেরত ret;
}

/*
 * Check if the expected and actual types match for an object
 * Returns OK if "expected" and "actual" are matching object types.
 * Returns FAIL if "expected" and "actual" are different types.
 * Returns MAYBE when a runtime type check is needed.
 */
    স্থির পূর্ণসংখ্যা
check_object_type_maybe(
    type_T	*expected,
    type_T	*actual,
    where_T	where)
{
    পূর্ণসংখ্যা ret = OK;

    যদি (actual->tt_type == VAR_ANY)
	ফেরত MAYBE;	// use runtime type check
    যদি (actual->tt_type != VAR_OBJECT)
	ফেরত FAIL;	// don't use tt_class
    যদি (actual->tt_class == ফাঁকা)    // null object
	ফেরত OK;
    // t_object_any matches any object except for an enum item
    যদি (expected == &t_object_any && !IS_ENUM(actual->tt_class))
	ফেরত OK;

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

    ফেরত ret;
}

/*
 * Check if the expected and actual types match.
 * Does not allow for assigning "any" to a specific type.
 * When "argidx" > 0 it is included in the error message.
 * Return OK if types match.
 * Return FAIL if types do not match.
 */
    পূর্ণসংখ্যা
check_type(
	type_T	*expected,
	type_T	*actual,
	পূর্ণসংখ্যা	give_বার্তা,
	where_T where)
{
    পূর্ণসংখ্যা ret = check_type_maybe(expected, actual, give_বার্তা, where);

    ফেরত ret == MAYBE ? OK : ret;
}

/*
 * As check_type() but return MAYBE when a runtime type check should be used
 * when compiling.
 */
    পূর্ণসংখ্যা
check_type_maybe(
	type_T	*expected,
	type_T	*actual,
	পূর্ণসংখ্যা	give_বার্তা,
	where_T where)
{
    পূর্ণসংখ্যা ret = OK;

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
		ফেরত OK;
	    যদি (expected->tt_type == VAR_FLOAT
		    && actual->tt_type == VAR_সংখ্যা
		    && ((expected->tt_flags & TTFLAG_সংখ্যা_OK)
			     || (actual->tt_flags & TTFLAG_FLOAT_OK)))
		// Using a number where a float is expected is OK here.
		ফেরত OK;
	    যদি (expected->tt_type == VAR_LIST
		    && actual->tt_type == VAR_TUPLE
		    && (expected->tt_flags & TTFLAG_TUPLE_OK))
		// Using a tuple where a list is expected is OK here.
		ফেরত OK;
	    যদি (give_বার্তা)
		type_mismatch_where(expected, actual, where);
	    ফেরত FAIL;
	}
	যদি (expected->tt_type == VAR_DICT || expected->tt_type == VAR_LIST)
	{
	    // "unknown" is used for an empty list or dict
	    যদি (actual->tt_member != ফাঁকা && actual->tt_member != &t_unknown)
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

    ফেরত ret;
}

/*
 * Check that the arguments of "type" match "argvars[argcount]".
 * "base_tv" is from "expr->Func()".
 * Return OK/FAIL.
 */
    পূর্ণসংখ্যা
check_argument_types(
	type_T	    *type,
	typval_T    *argvars,
	পূর্ণসংখ্যা	    argcount,
	typval_T    *base_tv,
	char_u	    *name)
{
    পূর্ণসংখ্যা	    varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;
    পূর্ণসংখ্যা	    i;
    পূর্ণসংখ্যা	    totcount = argcount + (base_tv == ফাঁকা ? 0 : 1);

    যদি (type->tt_type != VAR_FUNC && type->tt_type != VAR_PARTIAL)
	ফেরত OK;  // just in case
    যদি (totcount < type->tt_min_argcount - varargs)
    {
	emsg_funcname(e_not_enough_arguments_for_function_str, name);
	ফেরত FAIL;
    }
    যদি (!varargs && type->tt_argcount >= 0 && totcount > type->tt_argcount)
    {
	emsg_funcname(e_too_many_arguments_for_function_str, name);
	ফেরত FAIL;
    }
    যদি (type->tt_args == ফাঁকা)
	ফেরত OK;  // cannot check


    জন্য (i = 0; i < totcount; ++i)
    {
	type_T	    *expected;
	typval_T    *tv;

	যদি (base_tv != ফাঁকা)
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
	    যদি (expected != ফাঁকা && expected->tt_type == VAR_LIST)
		expected = expected->tt_member;
	    যদি (expected == ফাঁকা)
		expected = &t_any;
	}
	অন্যথা
	    expected = type->tt_args[i];

	// check the type, unless the value is v:none
	যদি ((tv->v_type != VAR_SPECIAL || tv->vval.v_সংখ্যা != VVAL_NONE)
		   && check_typval_arg_type(expected, tv, ফাঁকা, i + 1) == FAIL)
	    ফেরত FAIL;
    }
    ফেরত OK;
}

// Maximum nesting of types, to avoid running out of stack space.  E.g.
// "list<list<number>>" is nested twice.
#define MAX_TYPE_DEPTH	1000

// Current number of levels of parse_type() and the deepest level reached while
// parsing the current type.  The nesting is one less than the number of
// levels.
স্থির পূর্ণসংখ্যা	parse_depth = 0;
স্থির পূর্ণসংখ্যা	parse_max_depth = 0;

/*
 * Skip over type in list<type>, dict<type> or tuple<type>.
 * Returns a pointer to the character after the type.  "syn_error" is set to
 * TRUE on syntax error.
 */
    স্থির char_u *
skip_member_type(char_u *start, char_u *p, পূর্ণসংখ্যা *syn_error)
{
    যদি (STRNCMP("tuple", start, 5) == 0)
    {
	// handle tuple<{type1}, {type2}, ....<type>>
	p = skipwhite(p + 1);
	যখন (*p != '>' && *p != NUL)
	{
	    char_u *sp = p;

	    যদি (STRNCMP(p, "...", 3) == 0)
		p += 3;
	    p = skip_type(p, TRUE);
	    যদি (p == sp)
	    {
		*syn_error = TRUE;
		ফেরত p;  // syntax error
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

    ফেরত p;
}

/*
 * Skip over a function type.  Returns a pointer to the character after the
 * type.  "syn_error" is set to TRUE on syntax error.
 */
    স্থির char_u *
skip_func_type(char_u *p, পূর্ণসংখ্যা *syn_error)
{
    যদি (*p == '(')
    {
	// handle func(args): type
	++p;
	যখন (*p != ')' && *p != NUL)
	{
	    char_u *sp = p;

	    যদি (STRNCMP(p, "...", 3) == 0)
		p += 3;
	    p = skip_type(p, TRUE);
	    যদি (p == sp)
	    {
		*syn_error = TRUE;
		ফেরত p;  // syntax error
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

    ফেরত p;
}

/*
 * Inner part of skip_type().
 */
    স্থির char_u *
skip_type_inner(char_u *start, পূর্ণসংখ্যা optional)
{
    char_u	*p = start;
    পূর্ণসংখ্যা		syn_error = FALSE;

    যদি (optional && *p == '?')
	++p;

    // Also skip over "." for imported classes: "import.ClassName".
    যখন (ASCII_ISALNUM(*p) || *p == '_' || *p == '.')
	++p;

    // Skip over "<type>"; this is permissive about white space.
    যদি (*skipwhite(p) == '<')
    {
	p = skip_member_type(start, p, &syn_error);
	যদি (syn_error)
	    ফেরত p;
    }
    অন্যথা যদি ((*p == '(' || (*p == ':' && VIM_ISWHITE(p[1])))
					     && STRNCMP("func", start, 4) == 0)
    {
	// skip over function type
	p = skip_func_type(p, &syn_error);
	যদি (syn_error)
	    ফেরত p;
    }

    ফেরত p;
}

/*
 * Skip over a type definition and return a pointer to just after it.
 * When "optional" is TRUE then a leading "?" is accepted.
 */
    char_u *
skip_type(char_u *start, পূর্ণসংখ্যা optional)
{
    স্থির পূর্ণসংখ্যা	skip_depth = 0;
    char_u	*p = start;

    // Too deeply nested: skip to the matching ">" without recursing, an
    // error is given when parsing the type.
    যদি (skip_depth > MAX_TYPE_DEPTH)
    {
	পূর্ণসংখ্যা	depth = 0;

	জন্য ( ; *p != NUL; ++p)
	{
	    যদি (*p == '<')
		++depth;
	    অন্যথা যদি (*p == '>')
	    {
		যদি (depth == 0)
		    বিরতি;
		যদি (--depth == 0)
		    ফেরত p + 1;
	    }
	    অন্যথা যদি (depth == 0 && !ASCII_ISALNUM(*p) && *p != '_'
								&& *p != '.')
		বিরতি;
	}
	ফেরত p;
    }

    ++skip_depth;
    p = skip_type_inner(start, optional);
    --skip_depth;
    ফেরত p;
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
	পূর্ণসংখ্যা	    give_error,
	অক্ষর	    *info,
	ufunc_T	    *ufunc,
	cctx_T	    *cctx)
{
    char_u  *arg_start = *arg;
    type_T  *member_type;
    পূর্ণসংখ্যা	    পূর্ব_called_emsg = called_emsg;

    যদি (**arg != '<')
    {
	যদি (give_error)
	{
	    যদি (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    অন্যথা
		semsg(_(e_missing_type_after_str), info);
	}
	ফেরত ফাঁকা;
    }
    *arg = skipwhite(*arg + 1);

    member_type = parse_type(arg, type_gap, ufunc, cctx, give_error);
    যদি (member_type == ফাঁকা || !valid_declaration_type(member_type))
	ফেরত ফাঁকা;

    *arg = skipwhite(*arg);
    যদি (**arg != '>' && called_emsg == পূর্ব_called_emsg)
    {
	যদি (give_error)
	    semsg(_(e_missing_gt_after_type_str), arg_start);
	ফেরত ফাঁকা;
    }
    ++*arg;

    যদি (type->tt_type == VAR_LIST)
	ফেরত get_list_type(member_type, type_gap);
    ফেরত get_dict_type(member_type, type_gap);
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
    পূর্ণসংখ্যা		give_error,
    ufunc_T	*ufunc,
    cctx_T	*cctx)
{
    char_u  *p;
    type_T  *type;
    type_T  *ret_type = &t_unknown;
    পূর্ণসংখ্যা	argcount = -1;
    পূর্ণসংখ্যা	flags = 0;
    পূর্ণসংখ্যা	first_optional = -1;
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
	যখন (*p != NUL && *p != ')')
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
		ফেরত ফাঁকা;
	    }

	    type = parse_type(&p, type_gap, ufunc, cctx, give_error);
	    যদি (type == ফাঁকা || !valid_declaration_type(type))
		ফেরত ফাঁকা;
	    যদি ((flags & TTFLAG_VARARGS) != 0 && type->tt_type != VAR_LIST)
	    {
		অক্ষর *tofree;
		semsg(_(e_variable_arguments_type_must_be_list_str),
			type_name(type, &tofree));
		vim_free(tofree);
		ফেরত ফাঁকা;
	    }
	    arg_type[argcount++] = type;

	    // Nothing comes after "...{type}".
	    যদি (flags & TTFLAG_VARARGS)
		বিরতি;

	    যদি (*p != ',' && *skipwhite(p) == ',')
	    {
		যদি (give_error)
		    semsg(_(e_no_white_space_allowed_before_str_str),
			    ",", p);
		ফেরত ফাঁকা;
	    }
	    যদি (*p == ',')
	    {
		++p;
		যদি (!VIM_ISWHITE(*p))
		{
		    যদি (give_error)
			semsg(_(e_white_space_required_after_str_str),
				",", p - 1);
		    ফেরত ফাঁকা;
		}
	    }
	    p = skipwhite(p);
	    যদি (argcount == MAX_FUNC_ARGS)
	    {
		যদি (give_error)
		    emsg(_(e_too_many_argument_types));
		ফেরত ফাঁকা;
	    }
	}

	p = skipwhite(p);
	যদি (*p != ')')
	{
	    যদি (give_error)
		emsg(_(e_missing_closing_paren));
	    ফেরত ফাঁকা;
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
	যদি (ret_type == ফাঁকা)
	    ফেরত ফাঁকা;
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
		ফেরত ফাঁকা;
	    mch_memmove(type->tt_args, arg_type, আকার(type_T *) * argcount);
	}
    }

    ফেরত type;
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
    পূর্ণসংখ্যা		give_error,
    ufunc_T	*ufunc,
    cctx_T	*cctx)
{
    char_u	*p;
    type_T	*type;
    type_T	*ret_type = ফাঁকা;
    পূর্ণসংখ্যা		typecount = -1;
    পূর্ণসংখ্যা		flags = 0;
    garray_T	tuple_types_ga;

    ga_init2(&tuple_types_ga, আকার(type_T *), 10);

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
	ফেরত ফাঁকা;
    }

    p = ++*arg;
    typecount = 0;
    যখন (*p != NUL && *p != '>')
    {
	যদি (STRNCMP(p, "...", 3) == 0)
	{
	    flags |= TTFLAG_VARARGS;
	    p += 3;
	}

	type = parse_type(&p, type_gap, ufunc, cctx, give_error);
	যদি (type == ফাঁকা || !valid_declaration_type(type))
	    যাও on_err;

	যদি ((flags & TTFLAG_VARARGS) != 0 && type->tt_type != VAR_LIST)
	{
	    অক্ষর *tofree;
	    semsg(_(e_variadic_tuple_must_end_with_list_type_str),
						type_name(type, &tofree));
	    vim_free(tofree);
	    যাও on_err;
	}

	// Add the item type
	যদি (ga_grow(&tuple_types_ga, 1) == FAIL)
	    যাও on_err;
	((type_T **)tuple_types_ga.ga_তথ্য)[tuple_types_ga.ga_len] = type;
	tuple_types_ga.ga_len++;
	typecount++;

	// Nothing comes after "...{type}".
	যদি (flags & TTFLAG_VARARGS)
	    বিরতি;

	যদি (*p != ',' && *skipwhite(p) == ',')
	{
	    যদি (give_error)
		semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
	    যাও on_err;
	}
	যদি (*p == ',')
	{
	    ++p;
	    যদি (!VIM_ISWHITE(*p))
	    {
		যদি (give_error)
		    semsg(_(e_white_space_required_after_str_str),
			    ",", p - 1);
		যাও on_err;
	    }
	}
	p = skipwhite(p);
    }

    p = skipwhite(p);
    যদি (*p != '>' || typecount <= 0)
    {
	যদি (give_error)
	    semsg(_(e_missing_type_after_str), p);
	যাও on_err;
    }
    *arg = p + 1;

    ret_type = alloc_tuple_type(typecount, type_gap);
    যদি (ret_type == &t_any
	    || tuple_type_add_types(ret_type, typecount, type_gap) == FAIL)
    {
	ret_type = ফাঁকা;
	যাও on_err;
    }
    ret_type->tt_flags = flags;
    ret_type->tt_argcount = typecount;
    mch_memmove(ret_type->tt_args, tuple_types_ga.ga_তথ্য,
						আকার(type_T *) * typecount);

on_err:
    ga_clear(&tuple_types_ga);

    ফেরত ret_type;
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
    পূর্ণসংখ্যা		give_error,
    cctx_T	*cctx)
{
    char_u	*arg_start = *arg;
    type_T	*object_type;
    পূর্ণসংখ্যা		পূর্ব_called_emsg = called_emsg;

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
	ফেরত ফাঁকা;
    }

    // skip spaces following "object<"
    *arg = skipwhite(*arg + 1);

    object_type = parse_type(arg, type_gap, ফাঁকা, cctx, give_error);
    যদি (object_type == ফাঁকা)
	ফেরত ফাঁকা;

    *arg = skipwhite(*arg);
    যদি (**arg != '>' && called_emsg == পূর্ব_called_emsg)
    {
	যদি (give_error)
	    semsg(_(e_missing_gt_after_type_str), arg_start);
	ফেরত ফাঁকা;
    }
    ++*arg;

    যদি (object_type->tt_type == VAR_ANY)
	ফেরত &t_object_any;

    যদি (object_type->tt_type != VAR_OBJECT)
    {
	// specified type is not a class
	যদি (give_error)
	    semsg(_(e_class_name_not_found_str), arg_start);
	ফেরত ফাঁকা;
    }

    ফেরত object_type;
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
    পূর্ণসংখ্যা		give_error,
    ufunc_T	*ufunc,
    cctx_T	*cctx)
{
    পূর্ণসংখ্যা		did_emsg_before = did_emsg;
    typval_T	tv;

    tv.v_type = VAR_UNKNOWN;
    যদি (eval_variable_import(*arg, &tv) == OK)
    {
	যদি (tv.v_type == VAR_CLASS && tv.vval.v_class != ফাঁকা)
	{
	    type_T *type = get_type_ptr(type_gap);
	    যদি (type != ফাঁকা)
	    {
		// Although the name is that of a class or interface, the type
		// uses will be an object.
		type->tt_type = VAR_OBJECT;
		type->tt_class = tv.vval.v_class;
		clear_tv(&tv);

		*arg += len;
		// Skip over ".ClassName".
		যখন (ASCII_ISALNUM(**arg) || **arg == '_' || **arg == '.')
		    ++*arg;

		ফেরত type;
	    }
	}
	অন্যথা যদি (tv.v_type == VAR_TYPEALIAS)
	{
	    // user defined type
	    // The type alias may itself be a nested type, check the resulting
	    // nesting depth.
	    পূর্ণসংখ্যা	depth = parse_depth - 1
				+ MAX(tv.vval.v_typealias->ta_depth, 1);
	    যদি (depth - 1 > MAX_TYPE_DEPTH)
	    {
		clear_tv(&tv);
		যদি (give_error)
		    emsg(_(e_type_nested_too_deep));
		ফেরত ফাঁকা;
	    }
	    যদি (depth > parse_max_depth)
		parse_max_depth = depth;

	    type_T *type = copy_type(tv.vval.v_typealias->ta_type, type_gap);
	    *arg += len;
	    clear_tv(&tv);
	    // Skip over ".TypeName".
	    যখন (ASCII_ISALNUM(**arg) || **arg == '_' || **arg == '.')
		++*arg;
	    ফেরত type;
	}

	clear_tv(&tv);
    }

    // Check whether it is a generic type
    type_T *type = find_generic_type(*arg, len, ufunc, cctx);
    যদি (type != ফাঁকা)
    {
	*arg += len;
	ফেরত type;
    }

    যদি (give_error && (did_emsg == did_emsg_before))
    {
	char_u	*p = skip_type(*arg, FALSE);
	অক্ষর	cc = *p;

	*p = NUL;
	semsg(_(e_type_not_recognized_str), *arg);
	*p = cc;
    }

    ফেরত ফাঁকা;
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
    পূর্ণসংখ্যা		give_error)
{
    char_u  *p = *arg;
    size_t  len;

    // Skip over the first word.
    যখন (ASCII_ISALNUM(*p) || *p == '_')
	++p;
    len = p - *arg;

    নির্বাচন (**arg)
    {
	ক্ষেত্র 'a':
	    যদি (len == 3 && STRNCMP(*arg, "any", len) == 0)
	    {
		*arg += len;
		ফেরত &t_any;
	    }
	    বিরতি;
	ক্ষেত্র 'b':
	    যদি (len == 4 && STRNCMP(*arg, "bool", len) == 0)
	    {
		*arg += len;
		ফেরত &t_bool;
	    }
	    যদি (len == 4 && STRNCMP(*arg, "blob", len) == 0)
	    {
		*arg += len;
		ফেরত &t_blob;
	    }
	    বিরতি;
	ক্ষেত্র 'c':
	    যদি (len == 7 && STRNCMP(*arg, "channel", len) == 0)
	    {
		*arg += len;
		ফেরত &t_channel;
	    }
	    বিরতি;
	ক্ষেত্র 'd':
	    যদি (len == 4 && STRNCMP(*arg, "dict", len) == 0)
	    {
		*arg += len;
		ফেরত parse_type_member(arg, &t_dict_any, type_gap,
						give_error, "dict", ufunc,
						cctx);
	    }
	    বিরতি;
	ক্ষেত্র 'f':
	    যদি (len == 5 && STRNCMP(*arg, "float", len) == 0)
	    {
		*arg += len;
		ফেরত &t_float;
	    }
	    যদি (len == 4 && STRNCMP(*arg, "func", len) == 0)
		ফেরত parse_type_func(arg, len, type_gap, give_error, ufunc,
									cctx);
	    বিরতি;
	ক্ষেত্র 'j':
	    যদি (len == 3 && STRNCMP(*arg, "job", len) == 0)
	    {
		*arg += len;
		ফেরত &t_job;
	    }
	    বিরতি;
	ক্ষেত্র 'l':
	    যদি (len == 4 && STRNCMP(*arg, "list", len) == 0)
	    {
		*arg += len;
		ফেরত parse_type_member(arg, &t_list_any, type_gap,
						give_error, "list", ufunc,
						cctx);
	    }
	    বিরতি;
	ক্ষেত্র 'n':
	    যদি (len == 6 && STRNCMP(*arg, "number", len) == 0)
	    {
		*arg += len;
		ফেরত &t_সংখ্যা;
	    }
	    বিরতি;
	ক্ষেত্র 'o':
	    যদি (len == 6 && STRNCMP(*arg, "object", len) == 0)
	    {
		*arg += len;
		ফেরত parse_type_object(arg, type_gap, give_error, cctx);
	    }
	    বিরতি;
	ক্ষেত্র 's':
	    যদি (len == 6 && STRNCMP(*arg, "string", len) == 0)
	    {
		*arg += len;
		ফেরত &t_string;
	    }
	    বিরতি;
	ক্ষেত্র 't':
	    যদি (len == 5 && STRNCMP(*arg, "tuple", len) == 0)
	    {
		*arg += len;
		ফেরত parse_type_tuple(arg, type_gap, give_error, ufunc,
									cctx);
	    }
	    বিরতি;
	ক্ষেত্র 'v':
	    যদি (len == 4 && STRNCMP(*arg, "void", len) == 0)
	    {
		*arg += len;
		ফেরত &t_void;
	    }
	    বিরতি;
    }

    // User defined type
    ফেরত parse_type_user_defined(arg, len, type_gap, give_error, ufunc,
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
    পূর্ণসংখ্যা		give_error)
{
    type_T	*type;

    যদি (parse_depth > MAX_TYPE_DEPTH)
    {
	যদি (give_error)
	    emsg(_(e_type_nested_too_deep));
	ফেরত ফাঁকা;
    }
    যদি (parse_depth == 0)
	parse_max_depth = 0;
    ++parse_depth;
    যদি (parse_depth > parse_max_depth)
	parse_max_depth = parse_depth;
    type = parse_type_inner(arg, type_gap, ufunc, cctx, give_error);
    --parse_depth;
    ফেরত type;
}

/*
 * Return the number of levels of the type last parsed by parse_type().
 */
    পূর্ণসংখ্যা
parse_type_depth(শূন্য)
{
    ফেরত parse_max_depth;
}

/*
 * Check if "type1" and "type2" are exactly the same.
 * "flags" can have ETYPE_ARG_UNKNOWN, which means that an unknown argument
 * type in "type1" is accepted.
 */
    পূর্ণসংখ্যা
equal_type(type_T *type1, type_T *type2, পূর্ণসংখ্যা flags)
{
    পূর্ণসংখ্যা i;

    যদি (type1 == ফাঁকা || type2 == ফাঁকা)
	ফেরত FALSE;
    যদি (type1->tt_type != type2->tt_type)
	ফেরত FALSE;
    নির্বাচন (type1->tt_type)
    {
	ক্ষেত্র VAR_UNKNOWN:
	ক্ষেত্র VAR_ANY:
	ক্ষেত্র VAR_VOID:
	ক্ষেত্র VAR_SPECIAL:
	ক্ষেত্র VAR_BOOL:
	ক্ষেত্র VAR_সংখ্যা:
	ক্ষেত্র VAR_FLOAT:
	ক্ষেত্র VAR_STRING:
	ক্ষেত্র VAR_BLOB:
	ক্ষেত্র VAR_JOB:
	ক্ষেত্র VAR_CHANNEL:
	ক্ষেত্র VAR_INSTR:
	ক্ষেত্র VAR_TYPEALIAS:
	    বিরতি;  // not composite is always OK
	ক্ষেত্র VAR_OBJECT:
	ক্ষেত্র VAR_CLASS:
	    // Objects are considered equal if they are from the same class
	    ফেরত type1->tt_class == type2->tt_class;
	ক্ষেত্র VAR_LIST:
	ক্ষেত্র VAR_DICT:
	    ফেরত equal_type(type1->tt_member, type2->tt_member, flags);
	ক্ষেত্র VAR_TUPLE:
	    যদি (type1->tt_argcount != type2->tt_argcount)
		ফেরত FALSE;
	    যদি (type1->tt_argcount < 0
			   || type1->tt_args == ফাঁকা || type2->tt_args == ফাঁকা)
		ফেরত TRUE;
	    জন্য (i = 0; i < type1->tt_argcount; ++i)
		যদি ((flags & ETYPE_ARG_UNKNOWN) == 0
			&& !equal_type(type1->tt_args[i], type2->tt_args[i],
									flags))
		    ফেরত FALSE;
	    ফেরত TRUE;
	ক্ষেত্র VAR_FUNC:
	ক্ষেত্র VAR_PARTIAL:
	    যদি (!equal_type(type1->tt_member, type2->tt_member, flags)
		    || type1->tt_argcount != type2->tt_argcount)
		ফেরত FALSE;
	    যদি (type1->tt_argcount < 0
			   || type1->tt_args == ফাঁকা || type2->tt_args == ফাঁকা)
		ফেরত TRUE;
	    জন্য (i = 0; i < type1->tt_argcount; ++i)
		যদি ((flags & ETYPE_ARG_UNKNOWN) == 0
			&& !equal_type(type1->tt_args[i], type2->tt_args[i],
									flags))
		    ফেরত FALSE;
	    ফেরত TRUE;
    }
    ফেরত TRUE;
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
	ফেরত;
    }
    যদি (type2 == &t_func_unknown)
    {
	*dest = type1;
	ফেরত;
    }

    common_type(type1->tt_member, type2->tt_member, &common, type_gap);
    যদি (type1->tt_argcount == type2->tt_argcount
	    && type1->tt_argcount >= 0)
    {
	পূর্ণসংখ্যা argcount = type1->tt_argcount;
	পূর্ণসংখ্যা i;

	*dest = alloc_func_type(common, argcount, type_gap);
	যদি (type1->tt_args != ফাঁকা && type2->tt_args != ফাঁকা)
	{
	    যদি (func_type_add_arg_types(*dest, argcount,
			type_gap) == OK)
		জন্য (i = 0; i < argcount; ++i)
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
	ফেরত;
    }

    // If either is VAR_UNKNOWN use the other type.  An empty list/dict has no
    // specific type.
    যদি (type1 == ফাঁকা || type1->tt_type == VAR_UNKNOWN)
    {
	*dest = type2;
	ফেরত;
    }
    যদি (type2 == ফাঁকা || type2->tt_type == VAR_UNKNOWN)
    {
	*dest = type1;
	ফেরত;
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
	    ফেরত;
	}
	অন্যথা যদি (type1->tt_type == VAR_FUNC)
	{
	    common_type_var_func(type1, type2, dest, type_gap);
	    ফেরত;
	}
	অন্যথা যদি (type1->tt_type == VAR_OBJECT)
	{
	    *dest = &t_object_any;
	    ফেরত;
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
	    ফেরত &t_any;

	যদি (type->tt_flags & TTFLAG_VARARGS)
	    ফেরত type->tt_args[0]->tt_member;
	অন্যথা
	    ফেরত type->tt_args[0];
    }

    // For "any" the item type is not known.
    ফেরত type->tt_member == ফাঁকা ? &t_any : type->tt_member;
}

/*
 * Push an entry onto the type stack.  "type" used both for the current type
 * and the declared type.
 * Returns FAIL when out of memory.
 */
    পূর্ণসংখ্যা
ঠেলো_type_স্তূপ(cctx_T *cctx, type_T *type)
{
    ফেরত ঠেলো_type_stack2(cctx, type, type);
}

/*
 * Push an entry onto the type stack.  "type" is the current type, "decl_type"
 * is the declared type.
 * Returns FAIL when out of memory.
 */
    পূর্ণসংখ্যা
ঠেলো_type_stack2(cctx_T *cctx, type_T *type, type_T *decl_type)
{
    garray_T	*স্তূপ = &cctx->ctx_type_স্তূপ;
    type2_T	*typep;

    যদি (GA_GROW_FAILS(স্তূপ, 1))
	ফেরত FAIL;
    typep = ((type2_T *)স্তূপ->ga_তথ্য) + স্তূপ->ga_len;
    typep->type_curr = type;
    typep->type_decl = decl_type;
    ++স্তূপ->ga_len;
    ফেরত OK;
}

/*
 * Set the type of the top of the stack to "type".
 */
    শূন্য
set_type_on_স্তূপ(cctx_T *cctx, type_T *type, পূর্ণসংখ্যা offset)
{
    garray_T	*স্তূপ = &cctx->ctx_type_স্তূপ;
    type2_T	*typep = ((type2_T *)স্তূপ->ga_তথ্য)
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
get_type_on_স্তূপ(cctx_T *cctx, পূর্ণসংখ্যা offset)
{
    garray_T	*স্তূপ = &cctx->ctx_type_স্তূপ;

    যদি (offset + 1 > স্তূপ->ga_len)
	ফেরত &t_unknown;
    ফেরত (((type2_T *)স্তূপ->ga_তথ্য) + স্তূপ->ga_len - offset - 1)
								   ->type_curr;
}

/*
 * Get the declared type from the type stack.  If "offset" is zero the one at
 * the top,
 * if "offset" is one the type above that, etc.
 * Returns &t_unknown if there is no such stack entry.
 */
    type_T *
get_decl_type_on_স্তূপ(cctx_T *cctx, পূর্ণসংখ্যা offset)
{
    garray_T	*স্তূপ = &cctx->ctx_type_স্তূপ;

    যদি (offset + 1 > স্তূপ->ga_len)
	ফেরত &t_unknown;
    ফেরত (((type2_T *)স্তূপ->ga_তথ্য) + স্তূপ->ga_len - offset - 1)
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
	পূর্ণসংখ্যা	    গুনতি,
	পূর্ণসংখ্যা	    skip,
	cctx_T	    *cctx)
{
    garray_T	*স্তূপ = &cctx->ctx_type_স্তূপ;
    type2_T	*typep;
    garray_T    *type_gap = cctx->ctx_type_list;
    পূর্ণসংখ্যা		i;
    type_T	*ফলাফল;
    type_T	*type;

    // Use "unknown" for an empty list or dict.
    যদি (গুনতি == 0)
	ফেরত &t_unknown;
    // Find the common type from following items.
    typep = ((type2_T *)স্তূপ->ga_তথ্য) + স্তূপ->ga_len;
    ফলাফল = &t_unknown;
    জন্য (i = 0; i < গুনতি; ++i)
    {
	type = (typep -((গুনতি - i) * skip) + skip - 1)->type_curr;
	যদি (check_type_is_মান(type) == FAIL)
	    ফেরত ফাঁকা;
	যদি (ফলাফল != &t_any)
	    common_type(type, ফলাফল, &ফলাফল, type_gap);
    }

    ফেরত ফলাফল;
}

/*
 * Get the types of items in a tuple on the stack of "cctx".
 * Returns the number of types.  Returns -1 on failure.
 */
    পূর্ণসংখ্যা
get_tuple_type_from_স্তূপ(
    পূর্ণসংখ্যা		গুনতি,
    garray_T	*tuple_types_gap,
    cctx_T	*cctx)
{
    garray_T	*স্তূপ = &cctx->ctx_type_স্তূপ;
    type2_T	*typep;
    type_T	*type = ফাঁকা;

    // Use "unknown" for an empty tuple
    যদি (গুনতি == 0)
	ফেরত 0;

    // Find the common type from following items.
    typep = ((type2_T *)স্তূপ->ga_তথ্য) + স্তূপ->ga_len;
    জন্য (পূর্ণসংখ্যা i = 0; i < গুনতি; i++)
    {
	type = (typep - (গুনতি - i))->type_curr;
	যদি (check_type_is_মান(type) == FAIL)
	    ফেরত -1;
	যদি (ga_grow(tuple_types_gap, 1) == FAIL)
	    ফেরত -1;
	((type_T **)tuple_types_gap->ga_তথ্য)[tuple_types_gap->ga_len] = type;
	tuple_types_gap->ga_len++;
    }

    ফেরত tuple_types_gap->ga_len;
}

    অক্ষর *
vartype_name(vartype_T type)
{
    নির্বাচন (type)
    {
	ক্ষেত্র VAR_UNKNOWN: বিরতি;
	ক্ষেত্র VAR_ANY: ফেরত "any";
	ক্ষেত্র VAR_VOID: ফেরত "void";
	ক্ষেত্র VAR_SPECIAL: ফেরত "special";
	ক্ষেত্র VAR_BOOL: ফেরত "bool";
	ক্ষেত্র VAR_সংখ্যা: ফেরত "number";
	ক্ষেত্র VAR_FLOAT: ফেরত "float";
	ক্ষেত্র VAR_STRING: ফেরত "string";
	ক্ষেত্র VAR_BLOB: ফেরত "blob";
	ক্ষেত্র VAR_JOB: ফেরত "job";
	ক্ষেত্র VAR_CHANNEL: ফেরত "channel";
	ক্ষেত্র VAR_LIST: ফেরত "list";
	ক্ষেত্র VAR_TUPLE: ফেরত "tuple";
	ক্ষেত্র VAR_DICT: ফেরত "dict";
	ক্ষেত্র VAR_INSTR: ফেরত "instr";
	ক্ষেত্র VAR_CLASS: ফেরত "class";
	ক্ষেত্র VAR_OBJECT: ফেরত "object";
	ক্ষেত্র VAR_TYPEALIAS: ফেরত "typealias";

	ক্ষেত্র VAR_FUNC:
	ক্ষেত্র VAR_PARTIAL: ফেরত "func";
    }
    ফেরত "unknown";
}

/*
 * Return the type name of a List (list<type>) or Dict (dict<type>).
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    স্থির অক্ষর *
type_name_list_or_dict(অক্ষর *name, type_T *type, অক্ষর **tofree)
{
    অক্ষর *member_free;
    অক্ষর *member_name;

    যদি (type->tt_member->tt_type == VAR_UNKNOWN)
	member_name = type_name(&t_any, &member_free);
    অন্যথা
	member_name = type_name(type->tt_member, &member_free);

    size_t len = STRLEN(name) + STRLEN(member_name) + 3;
    *tofree = alloc(len);
    যদি (*tofree == ফাঁকা)
    {
	vim_free(member_free);
	ফেরত name;
    }

    vim_snprintf(*tofree, len, "%s<%s>", name, member_name);
    vim_free(member_free);
    ফেরত *tofree;
}

/*
 * Return the type name of a tuple.
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    স্থির অক্ষর *
type_name_tuple(type_T *type, অক্ষর **tofree)
{
    garray_T    ga;
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;
    অক্ষর	*arg_free = ফাঁকা;

    ga_init2(&ga, 1, 100);
    যদি (ga_grow(&ga, 20) == FAIL)
	যাও failed;
    STRCPY(ga.ga_তথ্য, "tuple<");
    ga.ga_len += 6;

    যদি (type->tt_argcount <= 0)
	// empty tuple
	GA_CONCAT_LITERAL(&ga, "any");
    অন্যথা
    {
	যদি (type->tt_args == ফাঁকা)
	    GA_CONCAT_LITERAL(&ga, "[unknown]");
	অন্যথা
	{
	    জন্য (i = 0; i < type->tt_argcount; ++i)
	    {
		string_T    arg_type;

		arg_type.string = (char_u *)type_name(type->tt_args[i], &arg_free);
		যদি (i > 0)
		{
		    STRCPY((অক্ষর *)ga.ga_তথ্য + ga.ga_len, ", ");
		    ga.ga_len += 2;
		}
		arg_type.দৈর্ঘ্য = STRLEN(arg_type.string);
		যদি (ga_grow(&ga, (পূর্ণসংখ্যা)arg_type.দৈর্ঘ্য + 8) == FAIL)
		    যাও failed;
		যদি (varargs && i == type->tt_argcount - 1)
		    GA_CONCAT_LITERAL(&ga, "...");
		ga_concat_len(&ga, arg_type.string, arg_type.দৈর্ঘ্য);
		VIM_CLEAR(arg_free);
	    }
	}
    }

    STRCPY((অক্ষর *)ga.ga_তথ্য + ga.ga_len, ">");
    *tofree = ga.ga_তথ্য;
    ফেরত ga.ga_তথ্য;

failed:
    vim_free(arg_free);
    ga_clear(&ga);
    ফেরত "[unknown]";
}

/*
 * Return the type name of a Class (class<name>) or Object (object<name>).
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    স্থির অক্ষর *
type_name_class_or_obj(অক্ষর *name, type_T *type, অক্ষর **tofree)
{
    string_T	class_name;

    যদি (type->tt_class != ফাঁকা)
    {
	class_name.string = type->tt_class->class_name.string;
	class_name.দৈর্ঘ্য = type->tt_class->class_name.দৈর্ঘ্য;
	যদি (IS_ENUM(type->tt_class))
	    name = "enum";
    }
    অন্যথা
	STR_LITERAL_SET(class_name, "any");

    size_t len = STRLEN(name) + class_name.দৈর্ঘ্য + 3;
    *tofree = alloc(len);
    যদি (*tofree == ফাঁকা)
	ফেরত name;

    vim_snprintf(*tofree, len, "%s<%s>", name, class_name.string);
    ফেরত *tofree;
}

/*
 * Return the type name of a function.
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    স্থির অক্ষর *
type_name_func(type_T *type, অক্ষর **tofree)
{
    garray_T    ga;
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;
    অক্ষর	*arg_free = ফাঁকা;

    ga_init2(&ga, 1, 100);
    যদি (ga_grow(&ga, 20) == FAIL)
	যাও failed;
    STRCPY(ga.ga_তথ্য, "func(");
    ga.ga_len += 5;

    জন্য (i = 0; i < type->tt_argcount; ++i)
    {
	string_T    arg_type;

	যদি (type->tt_args == ফাঁকা)
	    STR_LITERAL_SET(arg_type, "[unknown]");
	অন্যথা
	{
	    arg_type.string = (char_u *)type_name(type->tt_args[i], &arg_free);
	    arg_type.দৈর্ঘ্য = STRLEN(arg_type.string);
	}
	যদি (i > 0)
	{
	    STRCPY((অক্ষর *)ga.ga_তথ্য + ga.ga_len, ", ");
	    ga.ga_len += 2;
	}
	যদি (ga_grow(&ga, (পূর্ণসংখ্যা)arg_type.দৈর্ঘ্য + 8) == FAIL)
	    যাও failed;
	যদি (varargs && i == type->tt_argcount - 1)
	    GA_CONCAT_LITERAL(&ga, "...");
	অন্যথা যদি (i >= type->tt_min_argcount)
	    *((অক্ষর *)ga.ga_তথ্য + ga.ga_len++) = '?';
	ga_concat_len(&ga, arg_type.string, arg_type.দৈর্ঘ্য);
	VIM_CLEAR(arg_free);
    }
    যদি (type->tt_argcount < 0)
	// any number of arguments
	GA_CONCAT_LITERAL(&ga, "...");

    যদি (type->tt_member == &t_void)
	STRCPY((অক্ষর *)ga.ga_তথ্য + ga.ga_len, ")");
    অন্যথা
    {
	অক্ষর *ret_free = ফাঁকা;
	অক্ষর *ret_name = type_name(type->tt_member, &ret_free);
	পূর্ণসংখ্যা  len;

	len = (পূর্ণসংখ্যা)STRLEN(ret_name) + 4;
	যদি (ga_grow(&ga, len) == FAIL)
	{
	    vim_free(ret_free);
	    যাও failed;
	}
	STRCPY((অক্ষর *)ga.ga_তথ্য + ga.ga_len, "): ");
	STRCPY((অক্ষর *)ga.ga_তথ্য + ga.ga_len + 3, ret_name);
	vim_free(ret_free);
    }
    *tofree = ga.ga_তথ্য;
    ফেরত ga.ga_তথ্য;

failed:
    vim_free(arg_free);
    ga_clear(&ga);
    ফেরত "[unknown]";
}

/*
 * Return the name of a type.
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    অক্ষর *
type_name(type_T *type, অক্ষর **tofree)
{
    অক্ষর *name;

    *tofree = ফাঁকা;
    যদি (type == ফাঁকা)
	ফেরত "[unknown]";
    name = vartype_name(type->tt_type);

    নির্বাচন (type->tt_type)
    {
	ক্ষেত্র VAR_LIST:
	ক্ষেত্র VAR_DICT:
	    ফেরত type_name_list_or_dict(name, type, tofree);

	ক্ষেত্র VAR_TUPLE:
	    ফেরত type_name_tuple(type, tofree);

	ক্ষেত্র VAR_CLASS:
	ক্ষেত্র VAR_OBJECT:
	    ফেরত type_name_class_or_obj(name, type, tofree);

	ক্ষেত্র VAR_FUNC:
	    ফেরত type_name_func(type, tofree);

	পূর্বনির্ধারিত:
	    বিরতি;
    }

    ফেরত name;
}

/*
 * "typename(expr)" function
 */
    শূন্য
f_typename(typval_T *argvars, typval_T *rettv)
{
    garray_T	type_list;
    type_T	*type;
    অক্ষর	*tofree;
    অক্ষর	*name;

    rettv->v_type = VAR_STRING;
    ga_init2(&type_list, আকার(type_T *), 10);
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
	vim_snprintf((অক্ষর *)IObuff, IOSIZE, "typealias<%s>", name);
	rettv->vval.v_string = vim_strsave((char_u *)IObuff);
	যদি (tofree != ফাঁকা)
	    vim_free(tofree);
    }
    অন্যথা
    {
	যদি (tofree != ফাঁকা)
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
    পূর্ণসংখ্যা
check_typval_is_মান(typval_T *tv)
{
    যদি (tv == ফাঁকা)
	ফেরত OK;

    নির্বাচন (tv->v_type)
    {
	ক্ষেত্র VAR_CLASS:
	    {
		class_T *cl = tv->vval.v_class;
		char_u *class_name = (cl == ফাঁকা) ? (char_u *)""
							: cl->class_name.string;
		যদি (cl != ফাঁকা && IS_ENUM(cl))
		    semsg(_(e_using_enum_as_মান_str), class_name);
		অন্যথা
		    semsg(_(e_using_class_as_মান_str), class_name);
	    }
	    ফেরত FAIL;

	ক্ষেত্র VAR_TYPEALIAS:
	    semsg(_(e_using_typealias_as_মান_str),
		    tv->vval.v_typealias->ta_name);
	    ফেরত FAIL;

	পূর্বনির্ধারিত:
	    বিরতি;
    }
    ফেরত OK;
}

/*
 * Same as above, except check type_T.
 */
    পূর্ণসংখ্যা
check_type_is_মান(type_T *type)
{
    যদি (type == ফাঁকা)
	ফেরত OK;
    নির্বাচন (type->tt_type)
    {
	ক্ষেত্র VAR_CLASS:
	    যদি (type->tt_class != ফাঁকা && IS_ENUM(type->tt_class))
		semsg(_(e_using_enum_as_মান_str),
			type->tt_class->class_name.string);
	    অন্যথা
		semsg(_(e_using_class_as_মান_str),
			type->tt_class == ফাঁকা ? (char_u *)""
			: type->tt_class->class_name.string);
	    ফেরত FAIL;

	ক্ষেত্র VAR_TYPEALIAS:
	    // TODO: Not sure what could be done here to get a name.
	    //       Maybe an optional argument?
	    emsg(_(e_using_typealias_as_var_val));
	    ফেরত FAIL;

	পূর্বনির্ধারিত:
	    বিরতি;
    }
    ফেরত OK;
}

#endif // FEAT_EVAL
