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

#𐤀𐤌 defined(FEAT_EVAL)

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

    𐤀𐤌 (ga_grow(type_gap, 1) == FAIL)
	أرجع عدم;
    type = ALLOC_CLEAR_ONE(type_T);
    𐤀𐤌 (type == عدم)
	أرجع عدم;

    ((type_T **)type_gap->ga_data)[type_gap->ga_len] = type;
    ++type_gap->ga_len;
    أرجع type;
}

/*
 * Make a shallow copy of "type".
 * When allocation fails returns "type".
 */
    type_T *
copy_type(type_T *type, garray_T *type_gap)
{
    type_T *copy = get_type_ptr(type_gap);

    𐤀𐤌 (copy == عدم)
	أرجع type;
    *copy = *type;
    copy->tt_flags &= ~TTFLAG_STATIC;

    𐤀𐤌 (type->tt_args != عدم
	   && func_type_add_arg_types(copy, type->tt_argcount, type_gap) == OK)
	لكل (صحيح i = 0; i < type->tt_argcount; ++i)
	    copy->tt_args[i] = type->tt_args[i];

    أرجع copy;
}

/*
 * Inner part of copy_type_deep().
 * When allocation fails returns "type".
 */
    ساكن type_T *
copy_type_deep_rec(type_T *type, garray_T *type_gap, garray_T *seen_types)
{
    لكل (صحيح i = 0; i < seen_types->ga_len; ++i)
	𐤀𐤌 (((type_T **)seen_types->ga_data)[i * 2] == type)
	    // seen this type before, return the copy we made
	    أرجع ((type_T **)seen_types->ga_data)[i * 2 + 1];

    type_T *copy = copy_type(type, type_gap);
    𐤀𐤌 (ga_grow(seen_types, 1) == FAIL)
	أرجع copy;
    ((type_T **)seen_types->ga_data)[seen_types->ga_len * 2] = type;
    ((type_T **)seen_types->ga_data)[seen_types->ga_len * 2 + 1] = copy;
    ++seen_types->ga_len;

    𐤀𐤌 (copy->tt_member != عدم)
	copy->tt_member = copy_type_deep_rec(copy->tt_member,
							 type_gap, seen_types);
    𐤀𐤌 (type->tt_args != عدم)
	لكل (صحيح i = 0; i < type->tt_argcount; ++i)
	    copy->tt_args[i] = copy_type_deep_rec(copy->tt_args[i],
							 type_gap, seen_types);

    أرجع copy;
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
    أرجع res;
}

    فراغ
clear_type_list(garray_T *gap)
{
    طالما (gap->ga_len > 0)
	vim_free(((type_T **)gap->ga_data)[--gap->ga_len]);
    ga_clear(gap);
}

    فراغ
clear_func_type_list(garray_T *gap, type_T **func_type)
{
    طالما (gap->ga_len > 0)
    {
	// func_type pointing to the uf_type_list, so reset pointer
	𐤀𐤌 (*func_type == ((type_T **)gap->ga_data)[--gap->ga_len])
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

    𐤀𐤌 (type == عدم)
	أرجع عدم;

    // A fixed type never contains allocated types, return as-is.
    𐤀𐤌 (type->tt_flags & TTFLAG_STATIC)
	أرجع type;

    ret = ALLOC_ONE(type_T);
    𐤀𐤌 (ret == عدم)
	أرجع عدم;
    *ret = *type;

    𐤀𐤌 (ret->tt_member != عدم)
	ret->tt_member = alloc_type(ret->tt_member);

    𐤀𐤌 (type->tt_argcount > 0 && type->tt_args != عدم)
    {
	صحيح i;

	ret->tt_args = ALLOC_MULT(type_T *, type->tt_argcount);
	𐤀𐤌 (ret->tt_args != عدم)
	    لكل (i = 0; i < type->tt_argcount; ++i)
		ret->tt_args[i] = alloc_type(type->tt_args[i]);
    }
    وإلا
	ret->tt_args = عدم;

    أرجع ret;
}

/*
 * Free a type that was created with alloc_type().
 */
    فراغ
free_type(type_T *type)
{
    صحيح i;

    𐤀𐤌 (type == عدم || (type->tt_flags & TTFLAG_STATIC))
	أرجع;
    𐤀𐤌 (type->tt_args != عدم)
    {
	لكل (i = 0; i < type->tt_argcount; ++i)
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
    أرجع type->tt_member != عدم
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
    𐤀𐤌 (d->dv_type == type)
	أرجع;

    free_type(d->dv_type);
    d->dv_type = alloc_type(type);

    // Need to recursively set the type of dict items?
    𐤀𐤌 (!set_tv_type_recurse(type))
	أرجع;

    صحيح		todo = (صحيح)d->dv_hashtab.ht_used;
    hashitem_T	*hi;
    dictitem_T	*di;

    FOR_ALL_HASHTAB_ITEMS(&d->dv_hashtab, hi, todo)
    {
	𐤀𐤌 (!HASHITEM_EMPTY(hi))
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
    𐤀𐤌 (l->lv_type == type)
	أرجع;

    free_type(l->lv_type);
    l->lv_type = alloc_type(type);

    // Need to recursively set the type of list items?
    𐤀𐤌 (l->lv_first == &range_list_item || !set_tv_type_recurse(type))
	أرجع;

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
    𐤀𐤌 (tuple->tv_type == type)
	أرجع;

    free_type(tuple->tv_type);
    tuple->tv_type = alloc_type(type);

    𐤀𐤌 (type->tt_argcount <= 0)
	أرجع;

    // recursively set the type of list items
    type_T	*item_type;
    لكل (صحيح i = 0; i < tuple_len(tuple); i++)
    {
	𐤀𐤌 ((type->tt_flags & TTFLAG_VARARGS) && (i >= type->tt_argcount - 1))
	    // For a variadic tuple, the last type is a List.  So use the
	    // List member type.
	    item_type = type->tt_args[type->tt_argcount - 1]->tt_member;
	وإلا
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
    𐤀𐤌 (type->tt_type == VAR_ANY)
	// If the variable type is "any", then keep the value type.
	// e.g.  var x: any = [1, 2] or var y: any = {v: 1}
	أرجع;

    𐤀𐤌 (tv->v_type == VAR_DICT && tv->vval.v_dict != عدم)
	set_tv_type_dict(tv->vval.v_dict, type);
    وإلا 𐤀𐤌 (tv->v_type == VAR_LIST && tv->vval.v_list != عدم)
	set_tv_type_list(tv->vval.v_list, type);
    وإلا 𐤀𐤌 (tv->v_type == VAR_TUPLE && tv->vval.v_tuple != عدم)
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
    𐤀𐤌 (ga_grow(type_gap, 1) == FAIL)
	أرجع FAIL;
    tupletype->tt_args = alloc_clear_id(حجم(type_T *) * typecount,
							  aid_tuple_type_args);
    𐤀𐤌 (tupletype->tt_args == عدم)
	أرجع FAIL;
    ((type_T **)type_gap->ga_data)[type_gap->ga_len] =
						(فراغ *)tupletype->tt_args;
    ++type_gap->ga_len;
    أرجع OK;
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
    𐤀𐤌 (member_type == عدم || (member_type->tt_type == VAR_ANY
					&& !IS_GENERIC_TYPE(member_type)))
	أرجع &t_list_any;
    𐤀𐤌 (member_type->tt_type == VAR_VOID
	    || member_type->tt_type == VAR_UNKNOWN)
	أرجع &t_list_empty;
    𐤀𐤌 (member_type->tt_type == VAR_BOOL)
	أرجع &t_list_bool;
    𐤀𐤌 (member_type->tt_type == VAR_NUMBER)
	أرجع &t_list_number;
    𐤀𐤌 (member_type->tt_type == VAR_STRING)
	أرجع &t_list_string;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    𐤀𐤌 (type == عدم)
	أرجع &t_any;
    type->tt_type = VAR_LIST;
    type->tt_member = member_type;
    type->tt_argcount = 0;
    type->tt_args = عدم;
    أرجع type;
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
    𐤀𐤌 (typecount == 0)
	أرجع &t_tuple_any;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    𐤀𐤌 (type == عدم)
	أرجع &t_any;
    type->tt_type = VAR_TUPLE;
    type->tt_member = عدم;
    𐤀𐤌 (typecount > 0)
    {
	𐤀𐤌 (tuple_type_add_types(type, typecount, type_gap) == FAIL)
	    أرجع &t_any;
	mch_memmove(type->tt_args, tuple_types, حجم(type_T *) * typecount);
    }
    type->tt_argcount = typecount;
    type->tt_flags = 0;

    أرجع type;
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
    𐤀𐤌 (member_type == عدم || (member_type->tt_type == VAR_ANY
					&& !IS_GENERIC_TYPE(member_type)))
	أرجع &t_dict_any;
    𐤀𐤌 (member_type->tt_type == VAR_VOID
	    || member_type->tt_type == VAR_UNKNOWN)
	أرجع &t_dict_empty;
    𐤀𐤌 (member_type->tt_type == VAR_BOOL)
	أرجع &t_dict_bool;
    𐤀𐤌 (member_type->tt_type == VAR_NUMBER)
	أرجع &t_dict_number;
    𐤀𐤌 (member_type->tt_type == VAR_STRING)
	أرجع &t_dict_string;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    𐤀𐤌 (type == عدم)
	أرجع &t_any;
    type->tt_type = VAR_DICT;
    type->tt_member = member_type;
    type->tt_argcount = 0;
    type->tt_args = عدم;
    أرجع type;
}

/*
 * Allocate a new type for a function.
 */
    type_T *
alloc_func_type(type_T *ret_type, صحيح argcount, garray_T *type_gap)
{
    type_T *type = get_type_ptr(type_gap);

    𐤀𐤌 (type == عدم)
	أرجع &t_any;
    type->tt_type = VAR_FUNC;
    type->tt_member = ret_type == عدم ? &t_unknown : ret_type;
    type->tt_argcount = argcount;
    type->tt_args = عدم;
    أرجع type;
}

/*
 * Allocate a new type for a tuple.
 */
    ساكن type_T *
alloc_tuple_type(صحيح typecount, garray_T *type_gap)
{
    type_T *type = get_type_ptr(type_gap);

    𐤀𐤌 (type == عدم)
	أرجع &t_any;
    type->tt_type = VAR_TUPLE;
    type->tt_member = عدم;
    type->tt_argcount = typecount;
    type->tt_args = عدم;
    أرجع type;
}

/*
 * Get a function type, based on the return type "ret_type".
 * "argcount" must be -1 or 0, a predefined type can be used.
 */
    type_T *
get_func_type(type_T *ret_type, صحيح argcount, garray_T *type_gap)
{
    // recognize commonly used types
    𐤀𐤌 (ret_type == &t_unknown || ret_type == عدم)
    {
	// (argcount == 0) is not possible
	أرجع &t_func_unknown;
    }
    𐤀𐤌 (ret_type == &t_void)
    {
	𐤀𐤌 (argcount == 0)
	    أرجع &t_func_0_void;
	وإلا
	    أرجع &t_func_void;
    }
    𐤀𐤌 (ret_type == &t_any)
    {
	𐤀𐤌 (argcount == 0)
	    أرجع &t_func_0_any;
	وإلا
	    أرجع &t_func_any;
    }
    𐤀𐤌 (ret_type == &t_number)
    {
	𐤀𐤌 (argcount == 0)
	    أرجع &t_func_0_number;
	وإلا
	    أرجع &t_func_number;
    }
    𐤀𐤌 (ret_type == &t_string)
    {
	𐤀𐤌 (argcount == 0)
	    أرجع &t_func_0_string;
	وإلا
	    أرجع &t_func_string;
    }

    أرجع alloc_func_type(ret_type, argcount, type_gap);
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
    𐤀𐤌 (ga_grow(type_gap, 1) == FAIL)
	أرجع FAIL;
    functype->tt_args = ALLOC_CLEAR_MULT(type_T *, argcount);
    𐤀𐤌 (functype->tt_args == عدم)
	أرجع FAIL;
    ((type_T **)type_gap->ga_data)[type_gap->ga_len] =
						     (فراغ *)functype->tt_args;
    ++type_gap->ga_len;
    أرجع OK;
}

/*
 * Return TRUE if "type" is NULL, any or unknown.
 * This also works for const (comparing with &t_any and &t_unknown doesn't).
 */
    صحيح
type_any_or_unknown(type_T *type)
{
    أرجع type == عدم || type->tt_type == VAR_ANY
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
	    أرجع &t_null;

	حالة VVAL_NONE:
	    أرجع &t_none;

	حالة VVAL_TRUE:
	حالة VVAL_FALSE:
	    أرجع &t_bool;

	افتراضي:
	    أرجع &t_unknown;
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
    𐤀𐤌 (l == عدم || (l->lv_first == عدم
		&& (l->lv_type == عدم || l->lv_type->tt_member == &t_any)))
	أرجع &t_list_empty;

    𐤀𐤌 ((flags & TVTT_DO_MEMBER) == 0)
	أرجع &t_list_any;

    // If the type is list<any> go through the members, it may end up a
    // more specific type.
    𐤀𐤌 (l->lv_type != عدم && (l->lv_first == عدم
		|| (flags & TVTT_MORE_SPECIFIC) == 0
		|| l->lv_type->tt_member != &t_any))
	// make a copy, lv_type may be freed if the list is freed
	أرجع copy_type_deep(l->lv_type, type_gap);

    𐤀𐤌 (l->lv_first == &range_list_item)
	أرجع &t_list_number;

    𐤀𐤌 (l->lv_copyID == copyID)
	// avoid recursion
	أرجع &t_list_any;

    l->lv_copyID = copyID;

    // Use the common type of all members.
    member_type = typval2type(&l->lv_first->li_tv, copyID, type_gap,
							TVTT_DO_MEMBER);
    لكل (li = l->lv_first->li_next; li != عدم; li = li->li_next)
	common_type(typval2type(&li->li_tv, copyID, type_gap, TVTT_DO_MEMBER),
					member_type, &member_type, type_gap);

    // Reset copyID so that a shared reference to this list (not a circular
    // reference) can be processed again to get the correct type.
    l->lv_copyID = 0;

    أرجع get_list_type(member_type, type_gap);
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
    𐤀𐤌 (tuple == عدم || (len == 0 && (tuple->tv_type == عدم
				|| tuple->tv_type->tt_argcount == 0)))
	أرجع &t_tuple_empty;

    𐤀𐤌 ((flags & TVTT_DO_MEMBER) == 0)
	أرجع &t_tuple_any;

    // If the type is tuple<any> go through the members, it may end up a
    // more specific type.
    𐤀𐤌 (tuple->tv_type != عدم && (len == 0
					|| (flags & TVTT_MORE_SPECIFIC) == 0))
	// make a copy, tv_type may be freed if the tuple is freed
	أرجع copy_type_deep(tuple->tv_type, type_gap);

    𐤀𐤌 (tuple->tv_copyID == copyID)
	// avoid recursion
	أرجع &t_tuple_any;

    tuple->tv_copyID = copyID;

    garray_T	tuple_types_ga;
    ga_init2(&tuple_types_ga, حجم(type_T *), 10);
    لكل (صحيح i = 0; i < len; i++)
    {
	type = typval2type(TUPLE_ITEM(tuple, i), copyID, type_gap,
							TVTT_DO_MEMBER);
	𐤀𐤌 (ga_grow(&tuple_types_ga, 1) == FAIL)
	{
	    ga_clear(&tuple_types_ga);
	    // Reset copyID so that a shared reference to this tuple can be
	    // processed again.
	    tuple->tv_copyID = 0;
	    أرجع عدم;
	}
	((type_T **)tuple_types_ga.ga_data)[tuple_types_ga.ga_len] = type;
	tuple_types_ga.ga_len++;
    }

    type_T *tuple_type = get_tuple_type(&tuple_types_ga, type_gap);
    ga_clear(&tuple_types_ga);

    // Reset copyID so that a shared reference to this tuple (not a circular
    // reference) can be processed again to get the correct type.
    tuple->tv_copyID = 0;

    أرجع tuple_type;
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

    𐤀𐤌 (d == عدم || (d->dv_hashtab.ht_used == 0 && d->dv_type == عدم))
	أرجع &t_dict_empty;

    𐤀𐤌 ((flags & TVTT_DO_MEMBER) == 0)
	أرجع &t_dict_any;

    // If the type is dict<any> go through the members, it may end up a
    // more specific type.
    𐤀𐤌 (d->dv_type != عدم && (d->dv_hashtab.ht_used == 0
		|| (flags & TVTT_MORE_SPECIFIC) == 0
		|| d->dv_type->tt_member != &t_any))
	أرجع d->dv_type;

    𐤀𐤌 (d->dv_copyID == copyID)
	// avoid recursion
	أرجع &t_dict_any;

    d->dv_copyID = copyID;

    // Use the common type of all values.
    dict_iterate_start(tv, &iter);
    dict_iterate_next(&iter, &value);
    member_type = typval2type(value, copyID, type_gap, TVTT_DO_MEMBER);

    طالما (dict_iterate_next(&iter, &value) != عدم)
	common_type(typval2type(value, copyID, type_gap, TVTT_DO_MEMBER),
					member_type, &member_type, type_gap);

    // Reset copyID so that a shared reference to this dict (not a circular
    // reference) can be processed again to get the correct type.
    d->dv_copyID = 0;

    أرجع get_dict_type(member_type, type_gap);
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
    𐤀𐤌 (type == عدم)
	أرجع عدم;

    *type = *ufunc->uf_func_type;
    𐤀𐤌 (type->tt_argcount >= 0 && pt->pt_argc > 0)
    {
	type->tt_argcount -= pt->pt_argc;
	type->tt_min_argcount -= pt->pt_argc;
	𐤀𐤌 (type->tt_argcount > 0 && func_type_add_arg_types(type,
		    type->tt_argcount, type_gap) == OK)
	    لكل (صحيح i = 0; i < type->tt_argcount; ++i)
		type->tt_args[i] =
		    ufunc->uf_func_type->tt_args[i + pt->pt_argc];
    }
    𐤀𐤌 (pt->pt_func != عدم)
	type->tt_member = pt->pt_func->uf_ret_type;

    أرجع type;
}

/*
 * Get a type_T for a "class" or an "object" typval in "tv".
 */
    ساكن type_T *
oc_typval2type(typval_T *tv)
{
    𐤀𐤌 (tv->v_type == VAR_CLASS)
    {
	𐤀𐤌 (tv->vval.v_class == عدم)
	    أرجع &t_class;

	أرجع &tv->vval.v_class->class_type;
    }

    𐤀𐤌 (tv->vval.v_object != عدم)
	أرجع &tv->vval.v_object->obj_class->class_object_type;

    أرجع &t_object_any;
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

    𐤀𐤌 (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != عدم)
    {
	𐤀𐤌 (tv->vval.v_partial->pt_func != عدم)
	    ufunc = tv->vval.v_partial->pt_func;
	وإلا
	    name = tv->vval.v_partial->pt_name;
    }
    وإلا
	name = tv->vval.v_string;

    𐤀𐤌 (name == عدم && ufunc == عدم)
	أرجع &t_func_unknown;

    𐤀𐤌 (name != عدم)
    {
	صحيح idx = find_internal_func(name);

	𐤀𐤌 (idx >= 0)
	{
	    type_T *decl_type;  // unused

	    internal_func_get_argcount(idx, &argcount, &min_argcount);
	    member_type = internal_func_ret_type(idx, 0, عدم, &decl_type,
		    type_gap);
	}
	وإلا
	{
	    // Check if name contains "<".  If it does, then replace "<" with
	    // NUL and lookup the function and then look up the specific
	    // generic function using the supplied types.
	    char_u *p = generic_func_find_open_bracket(name);
	    𐤀𐤌 (p == عدم)
		ufunc = find_func(name, FALSE);
	    وإلا
	    {
		// generic function
		char_u	cc = *p;

		*p = NUL;
		ufunc = find_func(name, FALSE);
		*p = cc;
		𐤀𐤌 (ufunc != عدم && IS_GENERIC_FUNC(ufunc))
		{
		    ufunc = find_generic_func(ufunc, name, &p);
		    𐤀𐤌 (ufunc == عدم)
			أرجع عدم;
		}
	    }
	}
    }
    𐤀𐤌 (ufunc != عدم)
    {
	// May need to get the argument types from default values by
	// compiling the function.
	𐤀𐤌 (ufunc->uf_def_status == UF_TO_BE_COMPILED
		&& compile_def_function(ufunc, TRUE, CT_NONE, عدم)
		== FAIL)
	    أرجع عدم;
	𐤀𐤌 (ufunc->uf_func_type == عدم)
	    set_function_type(ufunc);
	𐤀𐤌 (ufunc->uf_func_type != عدم)
	{
	    𐤀𐤌 (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != عدم)
		أرجع partial_typval2type(tv, ufunc, type_gap);
	    أرجع ufunc->uf_func_type;
	}
    }

    type = get_type_ptr(type_gap);
    𐤀𐤌 (type == عدم)
	أرجع عدم;
    type->tt_type = tv->v_type;
    type->tt_argcount = argcount;
    type->tt_min_argcount = min_argcount;
    𐤀𐤌 (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != عدم
					&& tv->vval.v_partial->pt_argc > 0)
    {
	type->tt_argcount -= tv->vval.v_partial->pt_argc;
	type->tt_min_argcount -= tv->vval.v_partial->pt_argc;
    }
    type->tt_member = member_type;

    أرجع type;
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
	    أرجع &t_unknown;

	حالة VAR_ANY:
	    أرجع &t_any;

	حالة VAR_VOID:
	    أرجع &t_void;

	حالة VAR_BOOL:
	    أرجع &t_bool;

	حالة VAR_SPECIAL:
	    أرجع special_typval2type(tv);

	حالة VAR_NUMBER:
	    أرجع &t_number;

	حالة VAR_FLOAT:
	    أرجع &t_float;

	حالة VAR_STRING:
	    أرجع &t_string;

	حالة VAR_BLOB:
	    𐤀𐤌 (tv->vval.v_blob == عدم)
		أرجع &t_blob_null;
	    أرجع &t_blob;

	حالة VAR_LIST:
	    أرجع list_typval2type(tv, copyID, type_gap, flags);

	حالة VAR_TUPLE:
	    أرجع tuple_typval2type(tv, copyID, type_gap, flags);

	حالة VAR_DICT:
	    أرجع dict_typval2type(tv, copyID, type_gap, flags);

	حالة VAR_JOB:
	    أرجع &t_job;

	حالة VAR_CHANNEL:
	    أرجع &t_channel;

	حالة VAR_CLASS:
	حالة VAR_OBJECT:
	    أرجع oc_typval2type(tv);

	حالة VAR_TYPEALIAS:
	    أرجع &t_typealias;

	حالة VAR_FUNC:
	حالة VAR_PARTIAL:
	    أرجع fp_typval2type(tv, type_gap);

	حالة VAR_INSTR:
	افتراضي:
	    اكسر;
    }

    أرجع عدم;
}

/*
 * Return TRUE if "tv" is not a bool but should be converted to bool.
 */
    صحيح
need_convert_to_bool(type_T *type, typval_T *tv)
{
    أرجع type != عدم && type == &t_bool && tv->v_type != VAR_BOOL
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

    𐤀𐤌 (type == عدم)
	أرجع عدم;

    𐤀𐤌 (type != &t_bool && (tv->v_type == VAR_NUMBER
		&& (tv->vval.v_number == 0 || tv->vval.v_number == 1)))
	// Number 0 and 1 and expression with "&&" or "||" can also be used
	// for bool.
	type = &t_number_bool;
    وإلا 𐤀𐤌 (type != &t_float && tv->v_type == VAR_NUMBER)
	// A number can also be used for float.
	type = &t_number_float;
    أرجع type;
}

/*
 * Return TRUE if "type" can be used for a variable declaration.
 * Give an error and return FALSE if not.
 */
    صحيح
valid_declaration_type(type_T *type)
{
    𐤀𐤌 (type->tt_type == VAR_SPECIAL  // null, none
	    || type->tt_type == VAR_VOID)
    {
	حرف *tofree = عدم;
	حرف *name = type_name(type, &tofree);
	semsg(_(e_invalid_type_in_variable_declaration_str), name);
	vim_free(tofree);
	أرجع FALSE;
    }
    أرجع TRUE;
}

/*
 * Get a type_T for a typval_T, used for v: variables.
 * "type_list" is used to temporarily create types in.
 */
    type_T *
typval2type_vimvar(typval_T *tv, garray_T *type_gap)
{
    𐤀𐤌 (tv->v_type == VAR_LIST)  // e.g. for v:oldfiles
	أرجع &t_list_string;
    𐤀𐤌 (tv->v_type == VAR_DICT)  // e.g. for v:event
	أرجع &t_dict_any;
    أرجع typval2type(tv, get_copyID(), type_gap, TVTT_DO_MEMBER);
}

    صحيح
check_typval_arg_type(
	type_T	    *expected,
	typval_T    *actual_tv,
	حرف	    *func_name,
	صحيح	    arg_idx)
{
    where_T	where = WHERE_INIT;

    𐤀𐤌 (arg_idx > 0)
    {
	where.wt_index = arg_idx;
	where.wt_kind = WT_ARGUMENT;
    }
    where.wt_func_name = func_name;
    أرجع check_typval_type(expected, actual_tv, where);
}

/*
 * Return true if "expected" is list<any> or dict<any> and "actual_tv" is a
 * list or dict of that kind, which matches whatever its values are.
 */
    ساكن bool
matches_listdict_any(type_T *expected, typval_T *actual_tv)
{
    أرجع (expected->tt_type == VAR_LIST || expected->tt_type == VAR_DICT)
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

    𐤀𐤌 (expected == عدم)
	أرجع OK;  // didn't expect anything.

    // No need to go through the values of a possibly big list or dict.
    𐤀𐤌 (matches_listdict_any(expected, actual_tv))
	أرجع OK;

    ga_init2(&type_list, حجم(type_T *), 10);

    // A null_function and null_partial are special cases, they can be used to
    // clear a variable.
    𐤀𐤌 ((actual_tv->v_type == VAR_FUNC && actual_tv->vval.v_string == عدم)
	    || (actual_tv->v_type == VAR_PARTIAL
					 && actual_tv->vval.v_partial == عدم))
	actual_type = &t_func_unknown;
    وإلا
	// When the actual type is list<any> or dict<any> go through the values
	// to possibly get a more specific type.
	actual_type = typval2type(actual_tv, get_copyID(), &type_list,
					  TVTT_DO_MEMBER | TVTT_MORE_SPECIFIC);
    𐤀𐤌 (actual_type != عدم)
    {
	res = check_type_maybe(expected, actual_type, TRUE, where);
	𐤀𐤌 (res == MAYBE && !(actual_type->tt_type == VAR_FUNC
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
    أرجع res;
}

    فراغ
arg_type_mismatch(type_T *expected, type_T *actual, صحيح arg_idx)
{
    where_T	where = WHERE_INIT;

    𐤀𐤌 (arg_idx > 0)
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
	    𐤀𐤌 (where.wt_func_name == عدم)
		semsg(_(e_variable_nr_type_mismatch_expected_str_but_got_str),
			where.wt_index, typename1, typename2);
	    وإلا
		semsg(_(e_variable_nr_type_mismatch_expected_str_but_got_str_in_str),
			where.wt_index, typename1, typename2, where.wt_func_name);
	    اكسر;
	حالة WT_ARGUMENT:
	    𐤀𐤌 (where.wt_func_name == عدم)
		semsg(_(e_argument_nr_type_mismatch_expected_str_but_got_str),
			where.wt_index, typename1, typename2);
	    وإلا
		semsg(_(e_argument_nr_type_mismatch_expected_str_but_got_str_in_str),
			where.wt_index, typename1, typename2, where.wt_func_name);
	    اكسر;
	حالة WT_CAST:
	حالة WT_UNKNOWN:
	    𐤀𐤌 (where.wt_func_name == عدم)
		semsg(_(e_type_mismatch_expected_str_but_got_str),
			typename1, typename2);
	    وإلا
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
    𐤀𐤌 (expected->tt_argcount == -1 || actual->tt_argcount == -1
		|| expected->tt_args == عدم || actual->tt_args == عدم)
	أرجع OK;

    // For a non-variadic tuple, the number of items must match
    𐤀𐤌 (!(expected->tt_flags & TTFLAG_VARARGS)
	    && expected->tt_argcount != actual->tt_argcount)
	أرجع FAIL;

    // compare the type of each tuple item
    لكل (صحيح i = 0; i < actual->tt_argcount; ++i)
    {
	type_T	*exp_type;
	type_T	*actual_type;

	𐤀𐤌 (expected->tt_flags & TTFLAG_VARARGS)
	{
	    𐤀𐤌 (i < expected->tt_argcount - 1)
		exp_type = expected->tt_args[i];
	    وإلا
		// For a variadic tuple, the last type is a List.  So use the
		// List member type.
		exp_type = expected->tt_args[expected->tt_argcount - 1]->tt_member;
	}
	وإلا
	    exp_type = expected->tt_args[i];

	𐤀𐤌 (actual->tt_flags & TTFLAG_VARARGS)
	{
	    𐤀𐤌 (i < actual->tt_argcount - 1)
		actual_type = actual->tt_args[i];
	    وإلا
		// For a variadic tuple, the last type is a List.  So use the
		// List member type.
		actual_type = actual->tt_args[actual->tt_argcount - 1]->tt_member;
	}
	وإلا
	    actual_type = actual->tt_args[i];

	// Allow for using "any" type for a tuple item
	𐤀𐤌 (actual->tt_args[i] != &t_any && check_type(exp_type, actual_type,
						FALSE, where) == FAIL)
	    أرجع FAIL;
    }

    أرجع OK;
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
    𐤀𐤌 (expected->tt_member != &t_unknown)
    {
	𐤀𐤌 (actual->tt_member != عدم
		&& actual->tt_member != &t_unknown)
	{
	    where_T  func_where = where;

	    func_where.wt_kind = WT_METHOD_RETURN;
	    ret = check_type_maybe(expected->tt_member,
		    actual->tt_member, FALSE,
		    func_where);
	}
	وإلا
	    ret = MAYBE;
    }
    𐤀𐤌 (ret != FAIL
	    && ((expected->tt_flags & TTFLAG_VARARGS)
		!= (actual->tt_flags & TTFLAG_VARARGS))
	    && expected->tt_argcount != -1)
	ret = FAIL;
    𐤀𐤌 (ret != FAIL && expected->tt_argcount != -1
	    && actual->tt_min_argcount != -1
	    && (actual->tt_argcount == -1
		|| (actual->tt_argcount < expected->tt_min_argcount
		    || actual->tt_argcount > expected->tt_argcount)))
	ret = FAIL;
    𐤀𐤌 (ret != FAIL && expected->tt_args != عدم
	    && actual->tt_args != عدم)
    {
	صحيح i;

	لكل (i = 0; i < expected->tt_argcount
		&& i < actual->tt_argcount; ++i)
	{
	    where_T  func_where = where;
	    func_where.wt_kind = WT_METHOD_ARG;

	    // Allow for using "any" argument type, lambda's have them.
	    𐤀𐤌 (actual->tt_args[i] != &t_any && check_type(
			expected->tt_args[i], actual->tt_args[i], FALSE,
			func_where) == FAIL)
	    {
		ret = FAIL;
		اكسر;
	    }
	}
    }
    𐤀𐤌 (ret == OK && expected->tt_argcount >= 0
	    && actual->tt_argcount == -1)
	// check the argument count at runtime
	ret = MAYBE;

    أرجع ret;
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

    𐤀𐤌 (actual->tt_type == VAR_ANY)
	أرجع MAYBE;	// use runtime type check
    𐤀𐤌 (actual->tt_type != VAR_OBJECT)
	أرجع FAIL;	// don't use tt_class
    𐤀𐤌 (actual->tt_class == عدم)    // null object
	أرجع OK;
    // t_object_any matches any object except for an enum item
    𐤀𐤌 (expected == &t_object_any && !IS_ENUM(actual->tt_class))
	أرجع OK;

    // For object method arguments, do a invariant type check in
    // an extended class.  For all others, do a covariance type check.
    𐤀𐤌 (where.wt_kind == WT_METHOD_ARG)
    {
	𐤀𐤌 (actual->tt_class != expected->tt_class)
	    ret = FAIL;
    }
    وإلا 𐤀𐤌 (!class_instance_of(actual->tt_class, expected->tt_class))
    {
	// Check if this is an up-cast, if so we'll have to check the type at
	// runtime.
	𐤀𐤌 (where.wt_kind == WT_CAST &&
		class_instance_of(expected->tt_class, actual->tt_class))
	    ret = MAYBE;
	وإلا
	    ret = FAIL;
    }

    أرجع ret;
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

    أرجع ret == MAYBE ? OK : ret;
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
    𐤀𐤌 (expected->tt_type != VAR_UNKNOWN
	    && !(expected->tt_type == VAR_ANY && actual->tt_type != VAR_VOID))

    {
	// tt_type should match, except that a "partial" can be assigned to a
	// variable with type "func".
	// And "unknown" (using global variable) and "any" need a runtime type
	// check.
	𐤀𐤌 (!(expected->tt_type == actual->tt_type
		    || actual->tt_type == VAR_UNKNOWN
		    || actual->tt_type == VAR_ANY
		    || (expected->tt_type == VAR_FUNC
					   && actual->tt_type == VAR_PARTIAL)))
	{
	    𐤀𐤌 (expected->tt_type == VAR_BOOL
					&& (actual->tt_flags & TTFLAG_BOOL_OK))
		// Using number 0 or 1 for bool is OK.
		أرجع OK;
	    𐤀𐤌 (expected->tt_type == VAR_FLOAT
		    && actual->tt_type == VAR_NUMBER
		    && ((expected->tt_flags & TTFLAG_NUMBER_OK)
			     || (actual->tt_flags & TTFLAG_FLOAT_OK)))
		// Using a number where a float is expected is OK here.
		أرجع OK;
	    𐤀𐤌 (expected->tt_type == VAR_LIST
		    && actual->tt_type == VAR_TUPLE
		    && (expected->tt_flags & TTFLAG_TUPLE_OK))
		// Using a tuple where a list is expected is OK here.
		أرجع OK;
	    𐤀𐤌 (give_msg)
		type_mismatch_where(expected, actual, where);
	    أرجع FAIL;
	}
	𐤀𐤌 (expected->tt_type == VAR_DICT || expected->tt_type == VAR_LIST)
	{
	    // "unknown" is used for an empty list or dict
	    𐤀𐤌 (actual->tt_member != عدم && actual->tt_member != &t_unknown)
		ret = check_type_maybe(expected->tt_member, actual->tt_member,
								 FALSE, where);
	}
	وإلا 𐤀𐤌 (expected->tt_type == VAR_TUPLE && actual != &t_any)
	    ret =  check_tuple_type_maybe(expected, actual, where);
	وإلا 𐤀𐤌 (expected->tt_type == VAR_FUNC && actual != &t_any)
	    ret = check_func_type_maybe(expected, actual, where);
	وإلا 𐤀𐤌 (expected->tt_type == VAR_OBJECT)
	    ret = check_object_type_maybe(expected, actual, where);

	𐤀𐤌 (ret == FAIL && give_msg)
	    type_mismatch_where(expected, actual, where);
    }

    𐤀𐤌 (ret == OK && expected->tt_type != VAR_UNKNOWN
	    && expected->tt_type != VAR_ANY
	    && (actual->tt_type == VAR_UNKNOWN || actual->tt_type == VAR_ANY))
	// check the type at runtime
	ret = MAYBE;

    أرجع ret;
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

    𐤀𐤌 (type->tt_type != VAR_FUNC && type->tt_type != VAR_PARTIAL)
	أرجع OK;  // just in case
    𐤀𐤌 (totcount < type->tt_min_argcount - varargs)
    {
	emsg_funcname(e_not_enough_arguments_for_function_str, name);
	أرجع FAIL;
    }
    𐤀𐤌 (!varargs && type->tt_argcount >= 0 && totcount > type->tt_argcount)
    {
	emsg_funcname(e_too_many_arguments_for_function_str, name);
	أرجع FAIL;
    }
    𐤀𐤌 (type->tt_args == عدم)
	أرجع OK;  // cannot check


    لكل (i = 0; i < totcount; ++i)
    {
	type_T	    *expected;
	typval_T    *tv;

	𐤀𐤌 (base_tv != عدم)
	{
	    𐤀𐤌 (i == 0)
		tv = base_tv;
	    وإلا
		tv = &argvars[i - 1];
	}
	وإلا
	    tv = &argvars[i];
	𐤀𐤌 (varargs && i >= type->tt_argcount - 1)
	{
	    expected = type->tt_args[type->tt_argcount - 1];
	    𐤀𐤌 (expected != عدم && expected->tt_type == VAR_LIST)
		expected = expected->tt_member;
	    𐤀𐤌 (expected == عدم)
		expected = &t_any;
	}
	وإلا
	    expected = type->tt_args[i];

	// check the type, unless the value is v:none
	𐤀𐤌 ((tv->v_type != VAR_SPECIAL || tv->vval.v_number != VVAL_NONE)
		   && check_typval_arg_type(expected, tv, عدم, i + 1) == FAIL)
	    أرجع FAIL;
    }
    أرجع OK;
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
    𐤀𐤌 (STRNCMP("tuple", start, 5) == 0)
    {
	// handle tuple<{type1}, {type2}, ....<type>>
	p = skipwhite(p + 1);
	طالما (*p != '>' && *p != NUL)
	{
	    char_u *sp = p;

	    𐤀𐤌 (STRNCMP(p, "...", 3) == 0)
		p += 3;
	    p = skip_type(p, TRUE);
	    𐤀𐤌 (p == sp)
	    {
		*syn_error = TRUE;
		أرجع p;  // syntax error
	    }
	    𐤀𐤌 (*p == ',')
		p = skipwhite(p + 1);
	}
	𐤀𐤌 (*p == '>')
	    p++;
    }
    وإلا
    {
	p = skipwhite(p);
	p = skip_type(skipwhite(p + 1), FALSE);
	p = skipwhite(p);
	𐤀𐤌 (*p == '>')
	    ++p;
    }

    أرجع p;
}

/*
 * Skip over a function type.  Returns a pointer to the character after the
 * type.  "syn_error" is set to TRUE on syntax error.
 */
    ساكن char_u *
skip_func_type(char_u *p, صحيح *syn_error)
{
    𐤀𐤌 (*p == '(')
    {
	// handle func(args): type
	++p;
	طالما (*p != ')' && *p != NUL)
	{
	    char_u *sp = p;

	    𐤀𐤌 (STRNCMP(p, "...", 3) == 0)
		p += 3;
	    p = skip_type(p, TRUE);
	    𐤀𐤌 (p == sp)
	    {
		*syn_error = TRUE;
		أرجع p;  // syntax error
	    }
	    𐤀𐤌 (*p == ',')
		p = skipwhite(p + 1);
	}
	𐤀𐤌 (*p == ')')
	{
	    𐤀𐤌 (p[1] == ':')
		p = skip_type(skipwhite(p + 2), FALSE);
	    وإلا
		++p;
	}
    }
    وإلا
    {
	// handle func: return_type
	p = skip_type(skipwhite(p + 1), FALSE);
    }

    أرجع p;
}

/*
 * Inner part of skip_type().
 */
    ساكن char_u *
skip_type_inner(char_u *start, صحيح optional)
{
    char_u	*p = start;
    صحيح		syn_error = FALSE;

    𐤀𐤌 (optional && *p == '?')
	++p;

    // Also skip over "." for imported classes: "import.ClassName".
    طالما (ASCII_ISALNUM(*p) || *p == '_' || *p == '.')
	++p;

    // Skip over "<type>"; this is permissive about white space.
    𐤀𐤌 (*skipwhite(p) == '<')
    {
	p = skip_member_type(start, p, &syn_error);
	𐤀𐤌 (syn_error)
	    أرجع p;
    }
    وإلا 𐤀𐤌 ((*p == '(' || (*p == ':' && VIM_ISWHITE(p[1])))
					     && STRNCMP("func", start, 4) == 0)
    {
	// skip over function type
	p = skip_func_type(p, &syn_error);
	𐤀𐤌 (syn_error)
	    أرجع p;
    }

    أرجع p;
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
    𐤀𐤌 (skip_depth > MAX_TYPE_DEPTH)
    {
	صحيح	depth = 0;

	لكل ( ; *p != NUL; ++p)
	{
	    𐤀𐤌 (*p == '<')
		++depth;
	    وإلا 𐤀𐤌 (*p == '>')
	    {
		𐤀𐤌 (depth == 0)
		    اكسر;
		𐤀𐤌 (--depth == 0)
		    أرجع p + 1;
	    }
	    وإلا 𐤀𐤌 (depth == 0 && !ASCII_ISALNUM(*p) && *p != '_'
								&& *p != '.')
		اكسر;
	}
	أرجع p;
    }

    ++skip_depth;
    p = skip_type_inner(start, optional);
    --skip_depth;
    أرجع p;
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

    𐤀𐤌 (**arg != '<')
    {
	𐤀𐤌 (give_error)
	{
	    𐤀𐤌 (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    وإلا
		semsg(_(e_missing_type_after_str), info);
	}
	أرجع عدم;
    }
    *arg = skipwhite(*arg + 1);

    member_type = parse_type(arg, type_gap, ufunc, cctx, give_error);
    𐤀𐤌 (member_type == عدم || !valid_declaration_type(member_type))
	أرجع عدم;

    *arg = skipwhite(*arg);
    𐤀𐤌 (**arg != '>' && called_emsg == prev_called_emsg)
    {
	𐤀𐤌 (give_error)
	    semsg(_(e_missing_gt_after_type_str), arg_start);
	أرجع عدم;
    }
    ++*arg;

    𐤀𐤌 (type->tt_type == VAR_LIST)
	أرجع get_list_type(member_type, type_gap);
    أرجع get_dict_type(member_type, type_gap);
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
    𐤀𐤌 (**arg == '(')
    {
	// "func" may or may not return a value, "func()" does
	// not return a value.
	ret_type = &t_void;

	p = ++*arg;
	argcount = 0;
	طالما (*p != NUL && *p != ')')
	{
	    𐤀𐤌 (*p == '?')
	    {
		𐤀𐤌 (first_optional == -1)
		    first_optional = argcount;
		++p;
	    }
	    وإلا 𐤀𐤌 (STRNCMP(p, "...", 3) == 0)
	    {
		flags |= TTFLAG_VARARGS;
		p += 3;
	    }
	    وإلا 𐤀𐤌 (first_optional != -1)
	    {
		𐤀𐤌 (give_error)
		    emsg(_(e_mandatory_argument_after_optional_argument));
		أرجع عدم;
	    }

	    type = parse_type(&p, type_gap, ufunc, cctx, give_error);
	    𐤀𐤌 (type == عدم || !valid_declaration_type(type))
		أرجع عدم;
	    𐤀𐤌 ((flags & TTFLAG_VARARGS) != 0 && type->tt_type != VAR_LIST)
	    {
		حرف *tofree;
		semsg(_(e_variable_arguments_type_must_be_list_str),
			type_name(type, &tofree));
		vim_free(tofree);
		أرجع عدم;
	    }
	    arg_type[argcount++] = type;

	    // Nothing comes after "...{type}".
	    𐤀𐤌 (flags & TTFLAG_VARARGS)
		اكسر;

	    𐤀𐤌 (*p != ',' && *skipwhite(p) == ',')
	    {
		𐤀𐤌 (give_error)
		    semsg(_(e_no_white_space_allowed_before_str_str),
			    ",", p);
		أرجع عدم;
	    }
	    𐤀𐤌 (*p == ',')
	    {
		++p;
		𐤀𐤌 (!VIM_ISWHITE(*p))
		{
		    𐤀𐤌 (give_error)
			semsg(_(e_white_space_required_after_str_str),
				",", p - 1);
		    أرجع عدم;
		}
	    }
	    p = skipwhite(p);
	    𐤀𐤌 (argcount == MAX_FUNC_ARGS)
	    {
		𐤀𐤌 (give_error)
		    emsg(_(e_too_many_argument_types));
		أرجع عدم;
	    }
	}

	p = skipwhite(p);
	𐤀𐤌 (*p != ')')
	{
	    𐤀𐤌 (give_error)
		emsg(_(e_missing_closing_paren));
	    أرجع عدم;
	}
	*arg = p + 1;
    }
    𐤀𐤌 (**arg == ':')
    {
	// parse return type
	++*arg;
	𐤀𐤌 (!VIM_ISWHITE(**arg) && give_error)
	    semsg(_(e_white_space_required_after_str_str), ":", *arg - 1);
	*arg = skipwhite(*arg);
	ret_type = parse_type(arg, type_gap, ufunc, cctx, give_error);
	𐤀𐤌 (ret_type == عدم)
	    أرجع عدم;
    }
    𐤀𐤌 (flags == 0 && first_optional == -1 && argcount <= 0)
	type = get_func_type(ret_type, argcount, type_gap);
    وإلا
    {
	type = alloc_func_type(ret_type, argcount, type_gap);
	type->tt_flags = flags;
	𐤀𐤌 (argcount > 0)
	{
	    type->tt_argcount = argcount;
	    type->tt_min_argcount = first_optional == -1
						? argcount : first_optional;
	    𐤀𐤌 (func_type_add_arg_types(type, argcount, type_gap) == FAIL)
		أرجع عدم;
	    mch_memmove(type->tt_args, arg_type, حجم(type_T *) * argcount);
	}
    }

    أرجع type;
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
    𐤀𐤌 (**arg != '<')
    {
	𐤀𐤌 (give_error)
	{
	    𐤀𐤌 (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    وإلا
		semsg(_(e_missing_type_after_str), "tuple");
	}

	// only "tuple" is specified
	أرجع عدم;
    }

    p = ++*arg;
    typecount = 0;
    طالما (*p != NUL && *p != '>')
    {
	𐤀𐤌 (STRNCMP(p, "...", 3) == 0)
	{
	    flags |= TTFLAG_VARARGS;
	    p += 3;
	}

	type = parse_type(&p, type_gap, ufunc, cctx, give_error);
	𐤀𐤌 (type == عدم || !valid_declaration_type(type))
	    اذهب on_err;

	𐤀𐤌 ((flags & TTFLAG_VARARGS) != 0 && type->tt_type != VAR_LIST)
	{
	    حرف *tofree;
	    semsg(_(e_variadic_tuple_must_end_with_list_type_str),
						type_name(type, &tofree));
	    vim_free(tofree);
	    اذهب on_err;
	}

	// Add the item type
	𐤀𐤌 (ga_grow(&tuple_types_ga, 1) == FAIL)
	    اذهب on_err;
	((type_T **)tuple_types_ga.ga_data)[tuple_types_ga.ga_len] = type;
	tuple_types_ga.ga_len++;
	typecount++;

	// Nothing comes after "...{type}".
	𐤀𐤌 (flags & TTFLAG_VARARGS)
	    اكسر;

	𐤀𐤌 (*p != ',' && *skipwhite(p) == ',')
	{
	    𐤀𐤌 (give_error)
		semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
	    اذهب on_err;
	}
	𐤀𐤌 (*p == ',')
	{
	    ++p;
	    𐤀𐤌 (!VIM_ISWHITE(*p))
	    {
		𐤀𐤌 (give_error)
		    semsg(_(e_white_space_required_after_str_str),
			    ",", p - 1);
		اذهب on_err;
	    }
	}
	p = skipwhite(p);
    }

    p = skipwhite(p);
    𐤀𐤌 (*p != '>' || typecount <= 0)
    {
	𐤀𐤌 (give_error)
	    semsg(_(e_missing_type_after_str), p);
	اذهب on_err;
    }
    *arg = p + 1;

    ret_type = alloc_tuple_type(typecount, type_gap);
    𐤀𐤌 (ret_type == &t_any
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

    أرجع ret_type;
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
    𐤀𐤌 (**arg != '<')
    {
	𐤀𐤌 (give_error)
	{
	    𐤀𐤌 (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    وإلا
		semsg(_(e_missing_type_after_str), "object");
	}

	// only "object" is specified
	أرجع عدم;
    }

    // skip spaces following "object<"
    *arg = skipwhite(*arg + 1);

    object_type = parse_type(arg, type_gap, عدم, cctx, give_error);
    𐤀𐤌 (object_type == عدم)
	أرجع عدم;

    *arg = skipwhite(*arg);
    𐤀𐤌 (**arg != '>' && called_emsg == prev_called_emsg)
    {
	𐤀𐤌 (give_error)
	    semsg(_(e_missing_gt_after_type_str), arg_start);
	أرجع عدم;
    }
    ++*arg;

    𐤀𐤌 (object_type->tt_type == VAR_ANY)
	أرجع &t_object_any;

    𐤀𐤌 (object_type->tt_type != VAR_OBJECT)
    {
	// specified type is not a class
	𐤀𐤌 (give_error)
	    semsg(_(e_class_name_not_found_str), arg_start);
	أرجع عدم;
    }

    أرجع object_type;
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
    𐤀𐤌 (eval_variable_import(*arg, &tv) == OK)
    {
	𐤀𐤌 (tv.v_type == VAR_CLASS && tv.vval.v_class != عدم)
	{
	    type_T *type = get_type_ptr(type_gap);
	    𐤀𐤌 (type != عدم)
	    {
		// Although the name is that of a class or interface, the type
		// uses will be an object.
		type->tt_type = VAR_OBJECT;
		type->tt_class = tv.vval.v_class;
		clear_tv(&tv);

		*arg += len;
		// Skip over ".ClassName".
		طالما (ASCII_ISALNUM(**arg) || **arg == '_' || **arg == '.')
		    ++*arg;

		أرجع type;
	    }
	}
	وإلا 𐤀𐤌 (tv.v_type == VAR_TYPEALIAS)
	{
	    // user defined type
	    // The type alias may itself be a nested type, check the resulting
	    // nesting depth.
	    صحيح	depth = parse_depth - 1
				+ MAX(tv.vval.v_typealias->ta_depth, 1);
	    𐤀𐤌 (depth - 1 > MAX_TYPE_DEPTH)
	    {
		clear_tv(&tv);
		𐤀𐤌 (give_error)
		    emsg(_(e_type_nested_too_deep));
		أرجع عدم;
	    }
	    𐤀𐤌 (depth > parse_max_depth)
		parse_max_depth = depth;

	    type_T *type = copy_type(tv.vval.v_typealias->ta_type, type_gap);
	    *arg += len;
	    clear_tv(&tv);
	    // Skip over ".TypeName".
	    طالما (ASCII_ISALNUM(**arg) || **arg == '_' || **arg == '.')
		++*arg;
	    أرجع type;
	}

	clear_tv(&tv);
    }

    // Check whether it is a generic type
    type_T *type = find_generic_type(*arg, len, ufunc, cctx);
    𐤀𐤌 (type != عدم)
    {
	*arg += len;
	أرجع type;
    }

    𐤀𐤌 (give_error && (did_emsg == did_emsg_before))
    {
	char_u	*p = skip_type(*arg, FALSE);
	حرف	cc = *p;

	*p = NUL;
	semsg(_(e_type_not_recognized_str), *arg);
	*p = cc;
    }

    أرجع عدم;
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
    طالما (ASCII_ISALNUM(*p) || *p == '_')
	++p;
    len = p - *arg;

    بدل (**arg)
    {
	حالة 'a':
	    𐤀𐤌 (len == 3 && STRNCMP(*arg, "any", len) == 0)
	    {
		*arg += len;
		أرجع &t_any;
	    }
	    اكسر;
	حالة 'b':
	    𐤀𐤌 (len == 4 && STRNCMP(*arg, "bool", len) == 0)
	    {
		*arg += len;
		أرجع &t_bool;
	    }
	    𐤀𐤌 (len == 4 && STRNCMP(*arg, "blob", len) == 0)
	    {
		*arg += len;
		أرجع &t_blob;
	    }
	    اكسر;
	حالة 'c':
	    𐤀𐤌 (len == 7 && STRNCMP(*arg, "channel", len) == 0)
	    {
		*arg += len;
		أرجع &t_channel;
	    }
	    اكسر;
	حالة 'd':
	    𐤀𐤌 (len == 4 && STRNCMP(*arg, "dict", len) == 0)
	    {
		*arg += len;
		أرجع parse_type_member(arg, &t_dict_any, type_gap,
						give_error, "dict", ufunc,
						cctx);
	    }
	    اكسر;
	حالة 'f':
	    𐤀𐤌 (len == 5 && STRNCMP(*arg, "float", len) == 0)
	    {
		*arg += len;
		أرجع &t_float;
	    }
	    𐤀𐤌 (len == 4 && STRNCMP(*arg, "func", len) == 0)
		أرجع parse_type_func(arg, len, type_gap, give_error, ufunc,
									cctx);
	    اكسر;
	حالة 'j':
	    𐤀𐤌 (len == 3 && STRNCMP(*arg, "job", len) == 0)
	    {
		*arg += len;
		أرجع &t_job;
	    }
	    اكسر;
	حالة 'l':
	    𐤀𐤌 (len == 4 && STRNCMP(*arg, "list", len) == 0)
	    {
		*arg += len;
		أرجع parse_type_member(arg, &t_list_any, type_gap,
						give_error, "list", ufunc,
						cctx);
	    }
	    اكسر;
	حالة 'n':
	    𐤀𐤌 (len == 6 && STRNCMP(*arg, "number", len) == 0)
	    {
		*arg += len;
		أرجع &t_number;
	    }
	    اكسر;
	حالة 'o':
	    𐤀𐤌 (len == 6 && STRNCMP(*arg, "object", len) == 0)
	    {
		*arg += len;
		أرجع parse_type_object(arg, type_gap, give_error, cctx);
	    }
	    اكسر;
	حالة 's':
	    𐤀𐤌 (len == 6 && STRNCMP(*arg, "string", len) == 0)
	    {
		*arg += len;
		أرجع &t_string;
	    }
	    اكسر;
	حالة 't':
	    𐤀𐤌 (len == 5 && STRNCMP(*arg, "tuple", len) == 0)
	    {
		*arg += len;
		أرجع parse_type_tuple(arg, type_gap, give_error, ufunc,
									cctx);
	    }
	    اكسر;
	حالة 'v':
	    𐤀𐤌 (len == 4 && STRNCMP(*arg, "void", len) == 0)
	    {
		*arg += len;
		أرجع &t_void;
	    }
	    اكسر;
    }

    // User defined type
    أرجع parse_type_user_defined(arg, len, type_gap, give_error, ufunc,
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

    𐤀𐤌 (parse_depth > MAX_TYPE_DEPTH)
    {
	𐤀𐤌 (give_error)
	    emsg(_(e_type_nested_too_deep));
	أرجع عدم;
    }
    𐤀𐤌 (parse_depth == 0)
	parse_max_depth = 0;
    ++parse_depth;
    𐤀𐤌 (parse_depth > parse_max_depth)
	parse_max_depth = parse_depth;
    type = parse_type_inner(arg, type_gap, ufunc, cctx, give_error);
    --parse_depth;
    أرجع type;
}

/*
 * Return the number of levels of the type last parsed by parse_type().
 */
    صحيح
parse_type_depth(فراغ)
{
    أرجع parse_max_depth;
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

    𐤀𐤌 (type1 == عدم || type2 == عدم)
	أرجع FALSE;
    𐤀𐤌 (type1->tt_type != type2->tt_type)
	أرجع FALSE;
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
	    أرجع type1->tt_class == type2->tt_class;
	حالة VAR_LIST:
	حالة VAR_DICT:
	    أرجع equal_type(type1->tt_member, type2->tt_member, flags);
	حالة VAR_TUPLE:
	    𐤀𐤌 (type1->tt_argcount != type2->tt_argcount)
		أرجع FALSE;
	    𐤀𐤌 (type1->tt_argcount < 0
			   || type1->tt_args == عدم || type2->tt_args == عدم)
		أرجع TRUE;
	    لكل (i = 0; i < type1->tt_argcount; ++i)
		𐤀𐤌 ((flags & ETYPE_ARG_UNKNOWN) == 0
			&& !equal_type(type1->tt_args[i], type2->tt_args[i],
									flags))
		    أرجع FALSE;
	    أرجع TRUE;
	حالة VAR_FUNC:
	حالة VAR_PARTIAL:
	    𐤀𐤌 (!equal_type(type1->tt_member, type2->tt_member, flags)
		    || type1->tt_argcount != type2->tt_argcount)
		أرجع FALSE;
	    𐤀𐤌 (type1->tt_argcount < 0
			   || type1->tt_args == عدم || type2->tt_args == عدم)
		أرجع TRUE;
	    لكل (i = 0; i < type1->tt_argcount; ++i)
		𐤀𐤌 ((flags & ETYPE_ARG_UNKNOWN) == 0
			&& !equal_type(type1->tt_args[i], type2->tt_args[i],
									flags))
		    أرجع FALSE;
	    أرجع TRUE;
    }
    أرجع TRUE;
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
    𐤀𐤌 (type1 == &t_func_unknown)
    {
	*dest = type2;
	أرجع;
    }
    𐤀𐤌 (type2 == &t_func_unknown)
    {
	*dest = type1;
	أرجع;
    }

    common_type(type1->tt_member, type2->tt_member, &common, type_gap);
    𐤀𐤌 (type1->tt_argcount == type2->tt_argcount
	    && type1->tt_argcount >= 0)
    {
	صحيح argcount = type1->tt_argcount;
	صحيح i;

	*dest = alloc_func_type(common, argcount, type_gap);
	𐤀𐤌 (type1->tt_args != عدم && type2->tt_args != عدم)
	{
	    𐤀𐤌 (func_type_add_arg_types(*dest, argcount,
			type_gap) == OK)
		لكل (i = 0; i < argcount; ++i)
		    common_type(type1->tt_args[i], type2->tt_args[i],
			    &(*dest)->tt_args[i], type_gap);
	}
    }
    وإلا
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
    𐤀𐤌 (equal_type(type1, type2, 0))
    {
	*dest = type1;
	أرجع;
    }

    // If either is VAR_UNKNOWN use the other type.  An empty list/dict has no
    // specific type.
    𐤀𐤌 (type1 == عدم || type1->tt_type == VAR_UNKNOWN)
    {
	*dest = type2;
	أرجع;
    }
    𐤀𐤌 (type2 == عدم || type2->tt_type == VAR_UNKNOWN)
    {
	*dest = type1;
	أرجع;
    }

    𐤀𐤌 (type1->tt_type == type2->tt_type)
    {
	𐤀𐤌 (type1->tt_type == VAR_LIST
		|| type1->tt_type == VAR_DICT)
	{
	    type_T *common;

	    common_type(type1->tt_member, type2->tt_member, &common, type_gap);
	    𐤀𐤌 (type1->tt_type == VAR_LIST)
		*dest = get_list_type(common, type_gap);
	    وإلا
		*dest = get_dict_type(common, type_gap);
	    أرجع;
	}
	وإلا 𐤀𐤌 (type1->tt_type == VAR_FUNC)
	{
	    common_type_var_func(type1, type2, dest, type_gap);
	    أرجع;
	}
	وإلا 𐤀𐤌 (type1->tt_type == VAR_OBJECT)
	{
	    *dest = &t_object_any;
	    أرجع;
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
    𐤀𐤌 (type->tt_type == VAR_TUPLE)
    {
	𐤀𐤌 (type->tt_argcount != 1)
	    أرجع &t_any;

	𐤀𐤌 (type->tt_flags & TTFLAG_VARARGS)
	    أرجع type->tt_args[0]->tt_member;
	وإلا
	    أرجع type->tt_args[0];
    }

    // For "any" the item type is not known.
    أرجع type->tt_member == عدم ? &t_any : type->tt_member;
}

/*
 * Push an entry onto the type stack.  "type" used both for the current type
 * and the declared type.
 * Returns FAIL when out of memory.
 */
    صحيح
push_type_stack(cctx_T *cctx, type_T *type)
{
    أرجع push_type_stack2(cctx, type, type);
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

    𐤀𐤌 (GA_GROW_FAILS(stack, 1))
	أرجع FAIL;
    typep = ((type2_T *)stack->ga_data) + stack->ga_len;
    typep->type_curr = type;
    typep->type_decl = decl_type;
    ++stack->ga_len;
    أرجع OK;
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

    𐤀𐤌 (offset + 1 > stack->ga_len)
	أرجع &t_unknown;
    أرجع (((type2_T *)stack->ga_data) + stack->ga_len - offset - 1)
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

    𐤀𐤌 (offset + 1 > stack->ga_len)
	أرجع &t_unknown;
    أرجع (((type2_T *)stack->ga_data) + stack->ga_len - offset - 1)
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
    𐤀𐤌 (count == 0)
	أرجع &t_unknown;
    // Find the common type from following items.
    typep = ((type2_T *)stack->ga_data) + stack->ga_len;
    result = &t_unknown;
    لكل (i = 0; i < count; ++i)
    {
	type = (typep -((count - i) * skip) + skip - 1)->type_curr;
	𐤀𐤌 (check_type_is_value(type) == FAIL)
	    أرجع عدم;
	𐤀𐤌 (result != &t_any)
	    common_type(type, result, &result, type_gap);
    }

    أرجع result;
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
    𐤀𐤌 (count == 0)
	أرجع 0;

    // Find the common type from following items.
    typep = ((type2_T *)stack->ga_data) + stack->ga_len;
    لكل (صحيح i = 0; i < count; i++)
    {
	type = (typep - (count - i))->type_curr;
	𐤀𐤌 (check_type_is_value(type) == FAIL)
	    أرجع -1;
	𐤀𐤌 (ga_grow(tuple_types_gap, 1) == FAIL)
	    أرجع -1;
	((type_T **)tuple_types_gap->ga_data)[tuple_types_gap->ga_len] = type;
	tuple_types_gap->ga_len++;
    }

    أرجع tuple_types_gap->ga_len;
}

    حرف *
vartype_name(vartype_T type)
{
    بدل (type)
    {
	حالة VAR_UNKNOWN: اكسر;
	حالة VAR_ANY: أرجع "any";
	حالة VAR_VOID: أرجع "void";
	حالة VAR_SPECIAL: أرجع "special";
	حالة VAR_BOOL: أرجع "bool";
	حالة VAR_NUMBER: أرجع "number";
	حالة VAR_FLOAT: أرجع "float";
	حالة VAR_STRING: أرجع "string";
	حالة VAR_BLOB: أرجع "blob";
	حالة VAR_JOB: أرجع "job";
	حالة VAR_CHANNEL: أرجع "channel";
	حالة VAR_LIST: أرجع "list";
	حالة VAR_TUPLE: أرجع "tuple";
	حالة VAR_DICT: أرجع "dict";
	حالة VAR_INSTR: أرجع "instr";
	حالة VAR_CLASS: أرجع "class";
	حالة VAR_OBJECT: أرجع "object";
	حالة VAR_TYPEALIAS: أرجع "typealias";

	حالة VAR_FUNC:
	حالة VAR_PARTIAL: أرجع "func";
    }
    أرجع "unknown";
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

    𐤀𐤌 (type->tt_member->tt_type == VAR_UNKNOWN)
	member_name = type_name(&t_any, &member_free);
    وإلا
	member_name = type_name(type->tt_member, &member_free);

    size_t len = STRLEN(name) + STRLEN(member_name) + 3;
    *tofree = alloc(len);
    𐤀𐤌 (*tofree == عدم)
    {
	vim_free(member_free);
	أرجع name;
    }

    vim_snprintf(*tofree, len, "%s<%s>", name, member_name);
    vim_free(member_free);
    أرجع *tofree;
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
    𐤀𐤌 (ga_grow(&ga, 20) == FAIL)
	اذهب failed;
    STRCPY(ga.ga_data, "tuple<");
    ga.ga_len += 6;

    𐤀𐤌 (type->tt_argcount <= 0)
	// empty tuple
	GA_CONCAT_LITERAL(&ga, "any");
    وإلا
    {
	𐤀𐤌 (type->tt_args == عدم)
	    GA_CONCAT_LITERAL(&ga, "[unknown]");
	وإلا
	{
	    لكل (i = 0; i < type->tt_argcount; ++i)
	    {
		string_T    arg_type;

		arg_type.string = (char_u *)type_name(type->tt_args[i], &arg_free);
		𐤀𐤌 (i > 0)
		{
		    STRCPY((حرف *)ga.ga_data + ga.ga_len, ", ");
		    ga.ga_len += 2;
		}
		arg_type.length = STRLEN(arg_type.string);
		𐤀𐤌 (ga_grow(&ga, (صحيح)arg_type.length + 8) == FAIL)
		    اذهب failed;
		𐤀𐤌 (varargs && i == type->tt_argcount - 1)
		    GA_CONCAT_LITERAL(&ga, "...");
		ga_concat_len(&ga, arg_type.string, arg_type.length);
		VIM_CLEAR(arg_free);
	    }
	}
    }

    STRCPY((حرف *)ga.ga_data + ga.ga_len, ">");
    *tofree = ga.ga_data;
    أرجع ga.ga_data;

failed:
    vim_free(arg_free);
    ga_clear(&ga);
    أرجع "[unknown]";
}

/*
 * Return the type name of a Class (class<name>) or Object (object<name>).
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    ساكن حرف *
type_name_class_or_obj(حرف *name, type_T *type, حرف **tofree)
{
    string_T	class_name;

    𐤀𐤌 (type->tt_class != عدم)
    {
	class_name.string = type->tt_class->class_name.string;
	class_name.length = type->tt_class->class_name.length;
	𐤀𐤌 (IS_ENUM(type->tt_class))
	    name = "enum";
    }
    وإلا
	STR_LITERAL_SET(class_name, "any");

    size_t len = STRLEN(name) + class_name.length + 3;
    *tofree = alloc(len);
    𐤀𐤌 (*tofree == عدم)
	أرجع name;

    vim_snprintf(*tofree, len, "%s<%s>", name, class_name.string);
    أرجع *tofree;
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
    𐤀𐤌 (ga_grow(&ga, 20) == FAIL)
	اذهب failed;
    STRCPY(ga.ga_data, "func(");
    ga.ga_len += 5;

    لكل (i = 0; i < type->tt_argcount; ++i)
    {
	string_T    arg_type;

	𐤀𐤌 (type->tt_args == عدم)
	    STR_LITERAL_SET(arg_type, "[unknown]");
	وإلا
	{
	    arg_type.string = (char_u *)type_name(type->tt_args[i], &arg_free);
	    arg_type.length = STRLEN(arg_type.string);
	}
	𐤀𐤌 (i > 0)
	{
	    STRCPY((حرف *)ga.ga_data + ga.ga_len, ", ");
	    ga.ga_len += 2;
	}
	𐤀𐤌 (ga_grow(&ga, (صحيح)arg_type.length + 8) == FAIL)
	    اذهب failed;
	𐤀𐤌 (varargs && i == type->tt_argcount - 1)
	    GA_CONCAT_LITERAL(&ga, "...");
	وإلا 𐤀𐤌 (i >= type->tt_min_argcount)
	    *((حرف *)ga.ga_data + ga.ga_len++) = '?';
	ga_concat_len(&ga, arg_type.string, arg_type.length);
	VIM_CLEAR(arg_free);
    }
    𐤀𐤌 (type->tt_argcount < 0)
	// any number of arguments
	GA_CONCAT_LITERAL(&ga, "...");

    𐤀𐤌 (type->tt_member == &t_void)
	STRCPY((حرف *)ga.ga_data + ga.ga_len, ")");
    وإلا
    {
	حرف *ret_free = عدم;
	حرف *ret_name = type_name(type->tt_member, &ret_free);
	صحيح  len;

	len = (صحيح)STRLEN(ret_name) + 4;
	𐤀𐤌 (ga_grow(&ga, len) == FAIL)
	{
	    vim_free(ret_free);
	    اذهب failed;
	}
	STRCPY((حرف *)ga.ga_data + ga.ga_len, "): ");
	STRCPY((حرف *)ga.ga_data + ga.ga_len + 3, ret_name);
	vim_free(ret_free);
    }
    *tofree = ga.ga_data;
    أرجع ga.ga_data;

failed:
    vim_free(arg_free);
    ga_clear(&ga);
    أرجع "[unknown]";
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
    𐤀𐤌 (type == عدم)
	أرجع "[unknown]";
    name = vartype_name(type->tt_type);

    بدل (type->tt_type)
    {
	حالة VAR_LIST:
	حالة VAR_DICT:
	    أرجع type_name_list_or_dict(name, type, tofree);

	حالة VAR_TUPLE:
	    أرجع type_name_tuple(type, tofree);

	حالة VAR_CLASS:
	حالة VAR_OBJECT:
	    أرجع type_name_class_or_obj(name, type, tofree);

	حالة VAR_FUNC:
	    أرجع type_name_func(type, tofree);

	افتراضي:
	    اكسر;
    }

    أرجع name;
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
    𐤀𐤌 (argvars[0].v_type == VAR_TYPEALIAS)
    {
	type = copy_type(argvars[0].vval.v_typealias->ta_type, &type_list);
	// A type alias for a class has the type set to VAR_OBJECT.  Change it
	// to VAR_CLASS, so that the name is "typealias<class<xxx>>"
	𐤀𐤌 (type->tt_type == VAR_OBJECT)
	    type->tt_type = VAR_CLASS;
    }
    وإلا
	type = typval2type(argvars, get_copyID(), &type_list, TVTT_DO_MEMBER);
    name = type_name(type, &tofree);
    𐤀𐤌 (argvars[0].v_type == VAR_TYPEALIAS)
    {
	vim_snprintf((حرف *)IObuff, IOSIZE, "typealias<%s>", name);
	rettv->vval.v_string = vim_strsave((char_u *)IObuff);
	𐤀𐤌 (tofree != عدم)
	    vim_free(tofree);
    }
    وإلا
    {
	𐤀𐤌 (tofree != عدم)
	    rettv->vval.v_string = (char_u *)tofree;
	وإلا
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
    𐤀𐤌 (tv == عدم)
	أرجع OK;

    بدل (tv->v_type)
    {
	حالة VAR_CLASS:
	    {
		class_T *cl = tv->vval.v_class;
		char_u *class_name = (cl == عدم) ? (char_u *)""
							: cl->class_name.string;
		𐤀𐤌 (cl != عدم && IS_ENUM(cl))
		    semsg(_(e_using_enum_as_value_str), class_name);
		وإلا
		    semsg(_(e_using_class_as_value_str), class_name);
	    }
	    أرجع FAIL;

	حالة VAR_TYPEALIAS:
	    semsg(_(e_using_typealias_as_value_str),
		    tv->vval.v_typealias->ta_name);
	    أرجع FAIL;

	افتراضي:
	    اكسر;
    }
    أرجع OK;
}

/*
 * Same as above, except check type_T.
 */
    صحيح
check_type_is_value(type_T *type)
{
    𐤀𐤌 (type == عدم)
	أرجع OK;
    بدل (type->tt_type)
    {
	حالة VAR_CLASS:
	    𐤀𐤌 (type->tt_class != عدم && IS_ENUM(type->tt_class))
		semsg(_(e_using_enum_as_value_str),
			type->tt_class->class_name.string);
	    وإلا
		semsg(_(e_using_class_as_value_str),
			type->tt_class == عدم ? (char_u *)""
			: type->tt_class->class_name.string);
	    أرجع FAIL;

	حالة VAR_TYPEALIAS:
	    // TODO: Not sure what could be done here to get a name.
	    //       Maybe an optional argument?
	    emsg(_(e_using_typealias_as_var_val));
	    أرجع FAIL;

	افتراضي:
	    اكسر;
    }
    أرجع OK;
}

#endif // FEAT_EVAL
