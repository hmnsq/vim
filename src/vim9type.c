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
#அந்தர்பாவ "vim.h"

#எனில் defined(FEAT_EVAL)

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

    எனில் (ga_grow(type_gap, 1) == FAIL)
	திரும்பு வெற்று;
    type = ALLOC_CLEAR_ONE(type_T);
    எனில் (type == வெற்று)
	திரும்பு வெற்று;

    ((type_T **)type_gap->ga_data)[type_gap->ga_len] = type;
    ++type_gap->ga_len;
    திரும்பு type;
}

/*
 * Make a shallow copy of "type".
 * When allocation fails returns "type".
 */
    type_T *
copy_type(type_T *type, garray_T *type_gap)
{
    type_T *copy = get_type_ptr(type_gap);

    எனில் (copy == வெற்று)
	திரும்பு type;
    *copy = *type;
    copy->tt_flags &= ~TTFLAG_STATIC;

    எனில் (type->tt_args != வெற்று
	   && func_type_add_arg_types(copy, type->tt_argcount, type_gap) == OK)
	க்கு (முழு i = 0; i < type->tt_argcount; ++i)
	    copy->tt_args[i] = type->tt_args[i];

    திரும்பு copy;
}

/*
 * Inner part of copy_type_deep().
 * When allocation fails returns "type".
 */
    நிலையான type_T *
copy_type_deep_rec(type_T *type, garray_T *type_gap, garray_T *seen_types)
{
    க்கு (முழு i = 0; i < seen_types->ga_len; ++i)
	எனில் (((type_T **)seen_types->ga_data)[i * 2] == type)
	    // seen this type before, return the copy we made
	    திரும்பு ((type_T **)seen_types->ga_data)[i * 2 + 1];

    type_T *copy = copy_type(type, type_gap);
    எனில் (ga_grow(seen_types, 1) == FAIL)
	திரும்பு copy;
    ((type_T **)seen_types->ga_data)[seen_types->ga_len * 2] = type;
    ((type_T **)seen_types->ga_data)[seen_types->ga_len * 2 + 1] = copy;
    ++seen_types->ga_len;

    எனில் (copy->tt_member != வெற்று)
	copy->tt_member = copy_type_deep_rec(copy->tt_member,
							 type_gap, seen_types);
    எனில் (type->tt_args != வெற்று)
	க்கு (முழு i = 0; i < type->tt_argcount; ++i)
	    copy->tt_args[i] = copy_type_deep_rec(copy->tt_args[i],
							 type_gap, seen_types);

    திரும்பு copy;
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
    ga_init2(&seen_types, அளவு(type_T *) * 2, 20);

    type_T *res = copy_type_deep_rec(type, type_gap, &seen_types);

    ga_clear(&seen_types);
    திரும்பு res;
}

    வெறுமை
clear_type_list(garray_T *gap)
{
    போது (gap->ga_len > 0)
	vim_free(((type_T **)gap->ga_data)[--gap->ga_len]);
    ga_clear(gap);
}

    வெறுமை
clear_func_type_list(garray_T *gap, type_T **func_type)
{
    போது (gap->ga_len > 0)
    {
	// func_type pointing to the uf_type_list, so reset pointer
	எனில் (*func_type == ((type_T **)gap->ga_data)[--gap->ga_len])
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

    எனில் (type == வெற்று)
	திரும்பு வெற்று;

    // A fixed type never contains allocated types, return as-is.
    எனில் (type->tt_flags & TTFLAG_STATIC)
	திரும்பு type;

    ret = ALLOC_ONE(type_T);
    எனில் (ret == வெற்று)
	திரும்பு வெற்று;
    *ret = *type;

    எனில் (ret->tt_member != வெற்று)
	ret->tt_member = alloc_type(ret->tt_member);

    எனில் (type->tt_argcount > 0 && type->tt_args != வெற்று)
    {
	முழு i;

	ret->tt_args = ALLOC_MULT(type_T *, type->tt_argcount);
	எனில் (ret->tt_args != வெற்று)
	    க்கு (i = 0; i < type->tt_argcount; ++i)
		ret->tt_args[i] = alloc_type(type->tt_args[i]);
    }
    இல்லையெனில்
	ret->tt_args = வெற்று;

    திரும்பு ret;
}

/*
 * Free a type that was created with alloc_type().
 */
    வெறுமை
free_type(type_T *type)
{
    முழு i;

    எனில் (type == வெற்று || (type->tt_flags & TTFLAG_STATIC))
	திரும்பு;
    எனில் (type->tt_args != வெற்று)
    {
	க்கு (i = 0; i < type->tt_argcount; ++i)
	    free_type(type->tt_args[i]);
	vim_free(type->tt_args);
    }

    free_type(type->tt_member);

    vim_free(type);
}

/*
 * Return TRUE if "type" is to be recursed into for setting the type.
 */
    நிலையான முழு
set_tv_type_recurse(type_T *type)
{
    திரும்பு type->tt_member != வெற்று
		&& (type->tt_member->tt_type == VAR_DICT
				|| type->tt_member->tt_type == VAR_LIST)
		&& type->tt_member->tt_member != வெற்று
		&& type->tt_member->tt_member != &t_any
		&& type->tt_member->tt_member != &t_unknown;
}

/*
 * Set the type of Dict "d" to "type"
 */
    நிலையான வெறுமை
set_tv_type_dict(dict_T *d, type_T *type)
{
    எனில் (d->dv_type == type)
	திரும்பு;

    free_type(d->dv_type);
    d->dv_type = alloc_type(type);

    // Need to recursively set the type of dict items?
    எனில் (!set_tv_type_recurse(type))
	திரும்பு;

    முழு		todo = (முழு)d->dv_hashtab.ht_used;
    hashitem_T	*hi;
    dictitem_T	*di;

    FOR_ALL_HASHTAB_பொருட்கள்(&d->dv_hashtab, hi, todo)
    {
	எனில் (!HASHITEM_EMPTY(hi))
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
    நிலையான வெறுமை
set_tv_type_list(list_T *l, type_T *type)
{
    எனில் (l->lv_type == type)
	திரும்பு;

    free_type(l->lv_type);
    l->lv_type = alloc_type(type);

    // Need to recursively set the type of list items?
    எனில் (l->lv_first == &range_list_item || !set_tv_type_recurse(type))
	திரும்பு;

    listitem_T	*li;

    FOR_ALL_LIST_பொருட்கள்(l, li)
	set_tv_type(&li->li_tv, type->tt_member);
}

/*
 * Set the type of Tuple "tuple" to "type"
 */
    நிலையான வெறுமை
set_tv_type_tuple(tuple_T *tuple, type_T *type)
{
    எனில் (tuple->tv_type == type)
	திரும்பு;

    free_type(tuple->tv_type);
    tuple->tv_type = alloc_type(type);

    எனில் (type->tt_argcount <= 0)
	திரும்பு;

    // recursively set the type of list items
    type_T	*item_type;
    க்கு (முழு i = 0; i < tuple_len(tuple); i++)
    {
	எனில் ((type->tt_flags & TTFLAG_VARARGS) && (i >= type->tt_argcount - 1))
	    // For a variadic tuple, the last type is a List.  So use the
	    // List member type.
	    item_type = type->tt_args[type->tt_argcount - 1]->tt_member;
	இல்லையெனில்
	    item_type = type->tt_args[i];

	set_tv_type(TUPLE_ITEM(tuple, i), item_type);
    }
}

/*
 * Set the type of "tv" to "type" if it is a list or tuple or dict.
 */
    வெறுமை
set_tv_type(typval_T *tv, type_T *type)
{
    எனில் (type->tt_type == VAR_ANY)
	// If the variable type is "any", then keep the value type.
	// e.g.  var x: any = [1, 2] or var y: any = {v: 1}
	திரும்பு;

    எனில் (tv->v_type == VAR_DICT && tv->vval.v_dict != வெற்று)
	set_tv_type_dict(tv->vval.v_dict, type);
    இல்லையெனில் எனில் (tv->v_type == VAR_LIST && tv->vval.v_list != வெற்று)
	set_tv_type_list(tv->vval.v_list, type);
    இல்லையெனில் எனில் (tv->v_type == VAR_TUPLE && tv->vval.v_tuple != வெற்று)
	set_tv_type_tuple(tv->vval.v_tuple, type);
}

/*
 * For a tuple type, reserve space for "typecount" types (including the
 * repeated type).
 */
    நிலையான முழு
tuple_type_add_types(
    type_T	*tupletype,
    முழு		typecount,
    garray_T	*type_gap)
{
    // To make it easy to free the space needed for the types, add the
    // pointer to type_gap.
    எனில் (ga_grow(type_gap, 1) == FAIL)
	திரும்பு FAIL;
    tupletype->tt_args = alloc_clear_id(அளவு(type_T *) * typecount,
							  aid_tuple_type_args);
    எனில் (tupletype->tt_args == வெற்று)
	திரும்பு FAIL;
    ((type_T **)type_gap->ga_data)[type_gap->ga_len] =
						(வெறுமை *)tupletype->tt_args;
    ++type_gap->ga_len;
    திரும்பு OK;
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
    எனில் (member_type == வெற்று || (member_type->tt_type == VAR_ANY
					&& !IS_GENERIC_TYPE(member_type)))
	திரும்பு &t_list_any;
    எனில் (member_type->tt_type == VAR_VOID
	    || member_type->tt_type == VAR_UNKNOWN)
	திரும்பு &t_list_empty;
    எனில் (member_type->tt_type == VAR_BOOL)
	திரும்பு &t_list_bool;
    எனில் (member_type->tt_type == VAR_எண்)
	திரும்பு &t_list_எண்;
    எனில் (member_type->tt_type == VAR_STRING)
	திரும்பு &t_list_string;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    எனில் (type == வெற்று)
	திரும்பு &t_any;
    type->tt_type = VAR_LIST;
    type->tt_member = member_type;
    type->tt_argcount = 0;
    type->tt_args = வெற்று;
    திரும்பு type;
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
    முழு		typecount = tuple_types_gap->ga_len;

    // recognize commonly used types
    எனில் (typecount == 0)
	திரும்பு &t_tuple_any;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    எனில் (type == வெற்று)
	திரும்பு &t_any;
    type->tt_type = VAR_TUPLE;
    type->tt_member = வெற்று;
    எனில் (typecount > 0)
    {
	எனில் (tuple_type_add_types(type, typecount, type_gap) == FAIL)
	    திரும்பு &t_any;
	mch_memmove(type->tt_args, tuple_types, அளவு(type_T *) * typecount);
    }
    type->tt_argcount = typecount;
    type->tt_flags = 0;

    திரும்பு type;
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
    எனில் (member_type == வெற்று || (member_type->tt_type == VAR_ANY
					&& !IS_GENERIC_TYPE(member_type)))
	திரும்பு &t_dict_any;
    எனில் (member_type->tt_type == VAR_VOID
	    || member_type->tt_type == VAR_UNKNOWN)
	திரும்பு &t_dict_empty;
    எனில் (member_type->tt_type == VAR_BOOL)
	திரும்பு &t_dict_bool;
    எனில் (member_type->tt_type == VAR_எண்)
	திரும்பு &t_dict_எண்;
    எனில் (member_type->tt_type == VAR_STRING)
	திரும்பு &t_dict_string;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    எனில் (type == வெற்று)
	திரும்பு &t_any;
    type->tt_type = VAR_DICT;
    type->tt_member = member_type;
    type->tt_argcount = 0;
    type->tt_args = வெற்று;
    திரும்பு type;
}

/*
 * Allocate a new type for a function.
 */
    type_T *
alloc_func_type(type_T *ret_type, முழு argcount, garray_T *type_gap)
{
    type_T *type = get_type_ptr(type_gap);

    எனில் (type == வெற்று)
	திரும்பு &t_any;
    type->tt_type = VAR_FUNC;
    type->tt_member = ret_type == வெற்று ? &t_unknown : ret_type;
    type->tt_argcount = argcount;
    type->tt_args = வெற்று;
    திரும்பு type;
}

/*
 * Allocate a new type for a tuple.
 */
    நிலையான type_T *
alloc_tuple_type(முழு typecount, garray_T *type_gap)
{
    type_T *type = get_type_ptr(type_gap);

    எனில் (type == வெற்று)
	திரும்பு &t_any;
    type->tt_type = VAR_TUPLE;
    type->tt_member = வெற்று;
    type->tt_argcount = typecount;
    type->tt_args = வெற்று;
    திரும்பு type;
}

/*
 * Get a function type, based on the return type "ret_type".
 * "argcount" must be -1 or 0, a predefined type can be used.
 */
    type_T *
get_func_type(type_T *ret_type, முழு argcount, garray_T *type_gap)
{
    // recognize commonly used types
    எனில் (ret_type == &t_unknown || ret_type == வெற்று)
    {
	// (argcount == 0) is not possible
	திரும்பு &t_func_unknown;
    }
    எனில் (ret_type == &t_void)
    {
	எனில் (argcount == 0)
	    திரும்பு &t_func_0_void;
	இல்லையெனில்
	    திரும்பு &t_func_void;
    }
    எனில் (ret_type == &t_any)
    {
	எனில் (argcount == 0)
	    திரும்பு &t_func_0_any;
	இல்லையெனில்
	    திரும்பு &t_func_any;
    }
    எனில் (ret_type == &t_எண்)
    {
	எனில் (argcount == 0)
	    திரும்பு &t_func_0_எண்;
	இல்லையெனில்
	    திரும்பு &t_func_எண்;
    }
    எனில் (ret_type == &t_string)
    {
	எனில் (argcount == 0)
	    திரும்பு &t_func_0_string;
	இல்லையெனில்
	    திரும்பு &t_func_string;
    }

    திரும்பு alloc_func_type(ret_type, argcount, type_gap);
}

/*
 * For a function type, reserve space for "argcount" argument types (including
 * vararg).
 */
    முழு
func_type_add_arg_types(
	type_T	    *functype,
	முழு	    argcount,
	garray_T    *type_gap)
{
    // To make it easy to free the space needed for the argument types, add the
    // pointer to type_gap.
    எனில் (ga_grow(type_gap, 1) == FAIL)
	திரும்பு FAIL;
    functype->tt_args = ALLOC_CLEAR_MULT(type_T *, argcount);
    எனில் (functype->tt_args == வெற்று)
	திரும்பு FAIL;
    ((type_T **)type_gap->ga_data)[type_gap->ga_len] =
						     (வெறுமை *)functype->tt_args;
    ++type_gap->ga_len;
    திரும்பு OK;
}

/*
 * Return TRUE if "type" is NULL, any or unknown.
 * This also works for const (comparing with &t_any and &t_unknown doesn't).
 */
    முழு
type_any_or_unknown(type_T *type)
{
    திரும்பு type == வெற்று || type->tt_type == VAR_ANY
					|| type->tt_type == VAR_UNKNOWN;
}

/*
 * Get a type_T for a "special" typval in "tv".
 */
    நிலையான type_T *
special_typval2type(typval_T *tv)
{
    தேர்வு (tv->vval.v_எண்)
    {
	நிலை VVAL_NULL:
	    திரும்பு &t_null;

	நிலை VVAL_NONE:
	    திரும்பு &t_none;

	நிலை VVAL_TRUE:
	நிலை VVAL_FALSE:
	    திரும்பு &t_bool;

	இயல்பு:
	    திரும்பு &t_unknown;
    }
}

/*
 * Get a type_T for a List typval in "tv".
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    நிலையான type_T *
list_typval2type(typval_T *tv, முழு copyID, garray_T *type_gap, முழு flags)
{
    list_T	*l = tv->vval.v_list;
    listitem_T	*li;
    type_T	*member_type = வெற்று;

    // An empty list has type list<unknown>, unless the type was specified
    // and is not list<any>.  This matters when assigning to a variable
    // with a specific list type.
    எனில் (l == வெற்று || (l->lv_first == வெற்று
		&& (l->lv_type == வெற்று || l->lv_type->tt_member == &t_any)))
	திரும்பு &t_list_empty;

    எனில் ((flags & TVTT_DO_MEMBER) == 0)
	திரும்பு &t_list_any;

    // If the type is list<any> go through the members, it may end up a
    // more specific type.
    எனில் (l->lv_type != வெற்று && (l->lv_first == வெற்று
		|| (flags & TVTT_MORE_SPECIFIC) == 0
		|| l->lv_type->tt_member != &t_any))
	// make a copy, lv_type may be freed if the list is freed
	திரும்பு copy_type_deep(l->lv_type, type_gap);

    எனில் (l->lv_first == &range_list_item)
	திரும்பு &t_list_எண்;

    எனில் (l->lv_copyID == copyID)
	// avoid recursion
	திரும்பு &t_list_any;

    l->lv_copyID = copyID;

    // Use the common type of all members.
    member_type = typval2type(&l->lv_first->li_tv, copyID, type_gap,
							TVTT_DO_MEMBER);
    க்கு (li = l->lv_first->li_next; li != வெற்று; li = li->li_next)
	common_type(typval2type(&li->li_tv, copyID, type_gap, TVTT_DO_MEMBER),
					member_type, &member_type, type_gap);

    // Reset copyID so that a shared reference to this list (not a circular
    // reference) can be processed again to get the correct type.
    l->lv_copyID = 0;

    திரும்பு get_list_type(member_type, type_gap);
}

/*
 * Get a type_T for a Tuple typval in "tv".
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    நிலையான type_T *
tuple_typval2type(typval_T *tv, முழு copyID, garray_T *type_gap, முழு flags)
{
    tuple_T	*tuple = tv->vval.v_tuple;
    முழு		len = tuple_len(tuple);
    type_T	*type = வெற்று;

    // An empty tuple has type tuple<unknown>, unless the type was specified
    // and is not tuple<any>.  This matters when assigning to a variable
    // with a specific tuple type.
    எனில் (tuple == வெற்று || (len == 0 && (tuple->tv_type == வெற்று
				|| tuple->tv_type->tt_argcount == 0)))
	திரும்பு &t_tuple_empty;

    எனில் ((flags & TVTT_DO_MEMBER) == 0)
	திரும்பு &t_tuple_any;

    // If the type is tuple<any> go through the members, it may end up a
    // more specific type.
    எனில் (tuple->tv_type != வெற்று && (len == 0
					|| (flags & TVTT_MORE_SPECIFIC) == 0))
	// make a copy, tv_type may be freed if the tuple is freed
	திரும்பு copy_type_deep(tuple->tv_type, type_gap);

    எனில் (tuple->tv_copyID == copyID)
	// avoid recursion
	திரும்பு &t_tuple_any;

    tuple->tv_copyID = copyID;

    garray_T	tuple_types_ga;
    ga_init2(&tuple_types_ga, அளவு(type_T *), 10);
    க்கு (முழு i = 0; i < len; i++)
    {
	type = typval2type(TUPLE_ITEM(tuple, i), copyID, type_gap,
							TVTT_DO_MEMBER);
	எனில் (ga_grow(&tuple_types_ga, 1) == FAIL)
	{
	    ga_clear(&tuple_types_ga);
	    // Reset copyID so that a shared reference to this tuple can be
	    // processed again.
	    tuple->tv_copyID = 0;
	    திரும்பு வெற்று;
	}
	((type_T **)tuple_types_ga.ga_data)[tuple_types_ga.ga_len] = type;
	tuple_types_ga.ga_len++;
    }

    type_T *tuple_type = get_tuple_type(&tuple_types_ga, type_gap);
    ga_clear(&tuple_types_ga);

    // Reset copyID so that a shared reference to this tuple (not a circular
    // reference) can be processed again to get the correct type.
    tuple->tv_copyID = 0;

    திரும்பு tuple_type;
}

/*
 * Get a type_T for a Dict typval in "tv".
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    நிலையான type_T *
dict_typval2type(typval_T *tv, முழு copyID, garray_T *type_gap, முழு flags)
{
    dict_iterator_T	iter;
    typval_T		*மதிப்பு;
    dict_T		*d = tv->vval.v_dict;
    type_T		*member_type = வெற்று;

    எனில் (d == வெற்று || (d->dv_hashtab.ht_used == 0 && d->dv_type == வெற்று))
	திரும்பு &t_dict_empty;

    எனில் ((flags & TVTT_DO_MEMBER) == 0)
	திரும்பு &t_dict_any;

    // If the type is dict<any> go through the members, it may end up a
    // more specific type.
    எனில் (d->dv_type != வெற்று && (d->dv_hashtab.ht_used == 0
		|| (flags & TVTT_MORE_SPECIFIC) == 0
		|| d->dv_type->tt_member != &t_any))
	திரும்பு d->dv_type;

    எனில் (d->dv_copyID == copyID)
	// avoid recursion
	திரும்பு &t_dict_any;

    d->dv_copyID = copyID;

    // Use the common type of all values.
    dict_iterate_start(tv, &iter);
    dict_iterate_next(&iter, &மதிப்பு);
    member_type = typval2type(மதிப்பு, copyID, type_gap, TVTT_DO_MEMBER);

    போது (dict_iterate_next(&iter, &மதிப்பு) != வெற்று)
	common_type(typval2type(மதிப்பு, copyID, type_gap, TVTT_DO_MEMBER),
					member_type, &member_type, type_gap);

    // Reset copyID so that a shared reference to this dict (not a circular
    // reference) can be processed again to get the correct type.
    d->dv_copyID = 0;

    திரும்பு get_dict_type(member_type, type_gap);
}

/*
 * Get a type_T for a "partial" typval in "tv".
 */
    நிலையான type_T *
partial_typval2type(typval_T *tv, ufunc_T *ufunc, garray_T *type_gap)
{
    partial_T *pt = tv->vval.v_partial;
    type_T  *type;

    type = get_type_ptr(type_gap);
    எனில் (type == வெற்று)
	திரும்பு வெற்று;

    *type = *ufunc->uf_func_type;
    எனில் (type->tt_argcount >= 0 && pt->pt_argc > 0)
    {
	type->tt_argcount -= pt->pt_argc;
	type->tt_min_argcount -= pt->pt_argc;
	எனில் (type->tt_argcount > 0 && func_type_add_arg_types(type,
		    type->tt_argcount, type_gap) == OK)
	    க்கு (முழு i = 0; i < type->tt_argcount; ++i)
		type->tt_args[i] =
		    ufunc->uf_func_type->tt_args[i + pt->pt_argc];
    }
    எனில் (pt->pt_func != வெற்று)
	type->tt_member = pt->pt_func->uf_ret_type;

    திரும்பு type;
}

/*
 * Get a type_T for a "class" or an "object" typval in "tv".
 */
    நிலையான type_T *
oc_typval2type(typval_T *tv)
{
    எனில் (tv->v_type == VAR_CLASS)
    {
	எனில் (tv->vval.v_class == வெற்று)
	    திரும்பு &t_class;

	திரும்பு &tv->vval.v_class->class_type;
    }

    எனில் (tv->vval.v_object != வெற்று)
	திரும்பு &tv->vval.v_object->obj_class->class_object_type;

    திரும்பு &t_object_any;
}

/*
 * Get a type_T for a "function" or a "partial"
 */
    நிலையான type_T *
fp_typval2type(typval_T *tv, garray_T *type_gap)
{
    char_u	*name = வெற்று;
    ufunc_T	*ufunc = வெற்று;
    type_T	*type;
    type_T	*member_type = வெற்று;
    முழு		argcount = 0;
    முழு		min_argcount = 0;

    எனில் (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != வெற்று)
    {
	எனில் (tv->vval.v_partial->pt_func != வெற்று)
	    ufunc = tv->vval.v_partial->pt_func;
	இல்லையெனில்
	    name = tv->vval.v_partial->pt_name;
    }
    இல்லையெனில்
	name = tv->vval.v_string;

    எனில் (name == வெற்று && ufunc == வெற்று)
	திரும்பு &t_func_unknown;

    எனில் (name != வெற்று)
    {
	முழு idx = find_internal_func(name);

	எனில் (idx >= 0)
	{
	    type_T *decl_type;  // unused

	    internal_func_get_argcount(idx, &argcount, &min_argcount);
	    member_type = internal_func_ret_type(idx, 0, வெற்று, &decl_type,
		    type_gap);
	}
	இல்லையெனில்
	{
	    // Check if name contains "<".  If it does, then replace "<" with
	    // NUL and lookup the function and then look up the specific
	    // generic function using the supplied types.
	    char_u *p = generic_func_find_open_bracket(name);
	    எனில் (p == வெற்று)
		ufunc = find_func(name, FALSE);
	    இல்லையெனில்
	    {
		// generic function
		char_u	cc = *p;

		*p = NUL;
		ufunc = find_func(name, FALSE);
		*p = cc;
		எனில் (ufunc != வெற்று && IS_GENERIC_FUNC(ufunc))
		{
		    ufunc = find_generic_func(ufunc, name, &p);
		    எனில் (ufunc == வெற்று)
			திரும்பு வெற்று;
		}
	    }
	}
    }
    எனில் (ufunc != வெற்று)
    {
	// May need to get the argument types from default values by
	// compiling the function.
	எனில் (ufunc->uf_def_status == UF_TO_BE_COMPILED
		&& compile_def_function(ufunc, TRUE, CT_NONE, வெற்று)
		== FAIL)
	    திரும்பு வெற்று;
	எனில் (ufunc->uf_func_type == வெற்று)
	    set_function_type(ufunc);
	எனில் (ufunc->uf_func_type != வெற்று)
	{
	    எனில் (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != வெற்று)
		திரும்பு partial_typval2type(tv, ufunc, type_gap);
	    திரும்பு ufunc->uf_func_type;
	}
    }

    type = get_type_ptr(type_gap);
    எனில் (type == வெற்று)
	திரும்பு வெற்று;
    type->tt_type = tv->v_type;
    type->tt_argcount = argcount;
    type->tt_min_argcount = min_argcount;
    எனில் (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != வெற்று
					&& tv->vval.v_partial->pt_argc > 0)
    {
	type->tt_argcount -= tv->vval.v_partial->pt_argc;
	type->tt_min_argcount -= tv->vval.v_partial->pt_argc;
    }
    type->tt_member = member_type;

    திரும்பு type;
}

/*
 * Get a type_T for a typval_T.
 * "type_gap" is used to temporarily create types in.
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    நிலையான type_T *
typval2type_int(typval_T *tv, முழு copyID, garray_T *type_gap, முழு flags)
{
    தேர்வு (tv->v_type)
    {
	நிலை VAR_UNKNOWN:
	    திரும்பு &t_unknown;

	நிலை VAR_ANY:
	    திரும்பு &t_any;

	நிலை VAR_VOID:
	    திரும்பு &t_void;

	நிலை VAR_BOOL:
	    திரும்பு &t_bool;

	நிலை VAR_SPECIAL:
	    திரும்பு special_typval2type(tv);

	நிலை VAR_எண்:
	    திரும்பு &t_எண்;

	நிலை VAR_FLOAT:
	    திரும்பு &t_float;

	நிலை VAR_STRING:
	    திரும்பு &t_string;

	நிலை VAR_BLOB:
	    எனில் (tv->vval.v_blob == வெற்று)
		திரும்பு &t_blob_null;
	    திரும்பு &t_blob;

	நிலை VAR_LIST:
	    திரும்பு list_typval2type(tv, copyID, type_gap, flags);

	நிலை VAR_TUPLE:
	    திரும்பு tuple_typval2type(tv, copyID, type_gap, flags);

	நிலை VAR_DICT:
	    திரும்பு dict_typval2type(tv, copyID, type_gap, flags);

	நிலை VAR_JOB:
	    திரும்பு &t_job;

	நிலை VAR_CHANNEL:
	    திரும்பு &t_channel;

	நிலை VAR_CLASS:
	நிலை VAR_OBJECT:
	    திரும்பு oc_typval2type(tv);

	நிலை VAR_TYPEALIAS:
	    திரும்பு &t_typealias;

	நிலை VAR_FUNC:
	நிலை VAR_PARTIAL:
	    திரும்பு fp_typval2type(tv, type_gap);

	நிலை VAR_INSTR:
	இயல்பு:
	    நிறுத்து;
    }

    திரும்பு வெற்று;
}

/*
 * Return TRUE if "tv" is not a bool but should be converted to bool.
 */
    முழு
need_convert_to_bool(type_T *type, typval_T *tv)
{
    திரும்பு type != வெற்று && type == &t_bool && tv->v_type != VAR_BOOL
	    && (tv->v_type == VAR_எண்
		       && (tv->vval.v_எண் == 0 || tv->vval.v_எண் == 1));
}

/*
 * Get a type_T for a typval_T.
 * "type_list" is used to temporarily create types in.
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the most specific member type.
 */
    type_T *
typval2type(typval_T *tv, முழு copyID, garray_T *type_gap, முழு flags)
{
    type_T *type = typval2type_int(tv, copyID, type_gap, flags);

    எனில் (type == வெற்று)
	திரும்பு வெற்று;

    எனில் (type != &t_bool && (tv->v_type == VAR_எண்
		&& (tv->vval.v_எண் == 0 || tv->vval.v_எண் == 1)))
	// Number 0 and 1 and expression with "&&" or "||" can also be used
	// for bool.
	type = &t_எண்_bool;
    இல்லையெனில் எனில் (type != &t_float && tv->v_type == VAR_எண்)
	// A number can also be used for float.
	type = &t_எண்_float;
    திரும்பு type;
}

/*
 * Return TRUE if "type" can be used for a variable declaration.
 * Give an error and return FALSE if not.
 */
    முழு
valid_declaration_type(type_T *type)
{
    எனில் (type->tt_type == VAR_SPECIAL  // null, none
	    || type->tt_type == VAR_VOID)
    {
	எழுத்து *tofree = வெற்று;
	எழுத்து *name = type_name(type, &tofree);
	semsg(_(e_invalid_type_in_variable_declaration_str), name);
	vim_free(tofree);
	திரும்பு FALSE;
    }
    திரும்பு TRUE;
}

/*
 * Get a type_T for a typval_T, used for v: variables.
 * "type_list" is used to temporarily create types in.
 */
    type_T *
typval2type_vimvar(typval_T *tv, garray_T *type_gap)
{
    எனில் (tv->v_type == VAR_LIST)  // e.g. for v:oldfiles
	திரும்பு &t_list_string;
    எனில் (tv->v_type == VAR_DICT)  // e.g. for v:event
	திரும்பு &t_dict_any;
    திரும்பு typval2type(tv, get_copyID(), type_gap, TVTT_DO_MEMBER);
}

    முழு
check_typval_arg_type(
	type_T	    *expected,
	typval_T    *actual_tv,
	எழுத்து	    *func_name,
	முழு	    arg_idx)
{
    where_T	where = WHERE_INIT;

    எனில் (arg_idx > 0)
    {
	where.wt_index = arg_idx;
	where.wt_kind = WT_ARGUMENT;
    }
    where.wt_func_name = func_name;
    திரும்பு check_typval_type(expected, actual_tv, where);
}

/*
 * Return true if "expected" is list<any> or dict<any> and "actual_tv" is a
 * list or dict of that kind, which matches whatever its values are.
 */
    நிலையான bool
matches_listdict_any(type_T *expected, typval_T *actual_tv)
{
    திரும்பு (expected->tt_type == VAR_LIST || expected->tt_type == VAR_DICT)
	    && actual_tv->v_type == expected->tt_type
	    && expected->tt_member != வெற்று
	    && (expected->tt_member->tt_type == VAR_ANY
		|| expected->tt_member->tt_type == VAR_UNKNOWN);
}

/*
 * Return FAIL if "expected" and "actual" don't match.
 * When "argidx" > 0 it is included in the error message.
 */
    முழு
check_typval_type(type_T *expected, typval_T *actual_tv, where_T where)
{
    garray_T	type_list;
    type_T	*actual_type;
    முழு		res = FAIL;

    எனில் (expected == வெற்று)
	திரும்பு OK;  // didn't expect anything.

    // No need to go through the values of a possibly big list or dict.
    எனில் (matches_listdict_any(expected, actual_tv))
	திரும்பு OK;

    ga_init2(&type_list, அளவு(type_T *), 10);

    // A null_function and null_partial are special cases, they can be used to
    // clear a variable.
    எனில் ((actual_tv->v_type == VAR_FUNC && actual_tv->vval.v_string == வெற்று)
	    || (actual_tv->v_type == VAR_PARTIAL
					 && actual_tv->vval.v_partial == வெற்று))
	actual_type = &t_func_unknown;
    இல்லையெனில்
	// When the actual type is list<any> or dict<any> go through the values
	// to possibly get a more specific type.
	actual_type = typval2type(actual_tv, get_copyID(), &type_list,
					  TVTT_DO_MEMBER | TVTT_MORE_SPECIFIC);
    எனில் (actual_type != வெற்று)
    {
	res = check_type_maybe(expected, actual_type, TRUE, where);
	எனில் (res == MAYBE && !(actual_type->tt_type == VAR_FUNC
				      && (actual_type->tt_member == &t_unknown
					  || actual_type->tt_member == வெற்று)))
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
    திரும்பு res;
}

    வெறுமை
arg_type_mismatch(type_T *expected, type_T *actual, முழு arg_idx)
{
    where_T	where = WHERE_INIT;

    எனில் (arg_idx > 0)
    {
	where.wt_index = arg_idx;
	where.wt_kind = WT_ARGUMENT;
    }
    type_mismatch_where(expected, actual, where);
}

    வெறுமை
type_mismatch_where(type_T *expected, type_T *actual, where_T where)
{
    எழுத்து *tofree1, *tofree2;
    எழுத்து *typename1 = type_name(expected, &tofree1);
    எழுத்து *typename2 = type_name(actual, &tofree2);

    தேர்வு (where.wt_kind)
    {
	நிலை WT_MEMBER:
	    semsg(_(e_variable_str_type_mismatch_expected_str_but_got_str),
		    where.wt_func_name, typename1, typename2);
	    நிறுத்து;
	நிலை WT_METHOD:
	நிலை WT_METHOD_ARG:
	நிலை WT_METHOD_RETURN:
	    semsg(_(e_method_str_type_mismatch_expected_str_but_got_str),
		    where.wt_func_name, typename1, typename2);
	    நிறுத்து;
	நிலை WT_VARIABLE:
	    எனில் (where.wt_func_name == வெற்று)
		semsg(_(e_variable_nr_type_mismatch_expected_str_but_got_str),
			where.wt_index, typename1, typename2);
	    இல்லையெனில்
		semsg(_(e_variable_nr_type_mismatch_expected_str_but_got_str_in_str),
			where.wt_index, typename1, typename2, where.wt_func_name);
	    நிறுத்து;
	நிலை WT_ARGUMENT:
	    எனில் (where.wt_func_name == வெற்று)
		semsg(_(e_argument_nr_type_mismatch_expected_str_but_got_str),
			where.wt_index, typename1, typename2);
	    இல்லையெனில்
		semsg(_(e_argument_nr_type_mismatch_expected_str_but_got_str_in_str),
			where.wt_index, typename1, typename2, where.wt_func_name);
	    நிறுத்து;
	நிலை WT_CAST:
	நிலை WT_UNKNOWN:
	    எனில் (where.wt_func_name == வெற்று)
		semsg(_(e_type_mismatch_expected_str_but_got_str),
			typename1, typename2);
	    இல்லையெனில்
		semsg(_(e_type_mismatch_expected_str_but_got_str_in_str),
			typename1, typename2, where.wt_func_name);
	    நிறுத்து;
    }

    vim_free(tofree1);
    vim_free(tofree2);
}

/*
 * Check if the expected and actual types match for a tuple
 */
    நிலையான முழு
check_tuple_type_maybe(
    type_T	*expected,
    type_T	*actual,
    where_T	where)
{
    எனில் (expected->tt_argcount == -1 || actual->tt_argcount == -1
		|| expected->tt_args == வெற்று || actual->tt_args == வெற்று)
	திரும்பு OK;

    // For a non-variadic tuple, the number of items must match
    எனில் (!(expected->tt_flags & TTFLAG_VARARGS)
	    && expected->tt_argcount != actual->tt_argcount)
	திரும்பு FAIL;

    // compare the type of each tuple item
    க்கு (முழு i = 0; i < actual->tt_argcount; ++i)
    {
	type_T	*exp_type;
	type_T	*actual_type;

	எனில் (expected->tt_flags & TTFLAG_VARARGS)
	{
	    எனில் (i < expected->tt_argcount - 1)
		exp_type = expected->tt_args[i];
	    இல்லையெனில்
		// For a variadic tuple, the last type is a List.  So use the
		// List member type.
		exp_type = expected->tt_args[expected->tt_argcount - 1]->tt_member;
	}
	இல்லையெனில்
	    exp_type = expected->tt_args[i];

	எனில் (actual->tt_flags & TTFLAG_VARARGS)
	{
	    எனில் (i < actual->tt_argcount - 1)
		actual_type = actual->tt_args[i];
	    இல்லையெனில்
		// For a variadic tuple, the last type is a List.  So use the
		// List member type.
		actual_type = actual->tt_args[actual->tt_argcount - 1]->tt_member;
	}
	இல்லையெனில்
	    actual_type = actual->tt_args[i];

	// Allow for using "any" type for a tuple item
	எனில் (actual->tt_args[i] != &t_any && check_type(exp_type, actual_type,
						FALSE, where) == FAIL)
	    திரும்பு FAIL;
    }

    திரும்பு OK;
}

/*
 * Check if the expected and actual types match for a function
 * Returns OK if "expected" and "actual" are matching function types.
 * Returns FAIL if "expected" and "actual" are different types.
 * Returns MAYBE when a runtime type check is needed.
 */
    நிலையான முழு
check_func_type_maybe(
    type_T	*expected,
    type_T	*actual,
    where_T	where)
{
    முழு ret = OK;

    // If the return type is unknown it can be anything, including
    // nothing, thus there is no point in checking.
    எனில் (expected->tt_member != &t_unknown)
    {
	எனில் (actual->tt_member != வெற்று
		&& actual->tt_member != &t_unknown)
	{
	    where_T  func_where = where;

	    func_where.wt_kind = WT_METHOD_RETURN;
	    ret = check_type_maybe(expected->tt_member,
		    actual->tt_member, FALSE,
		    func_where);
	}
	இல்லையெனில்
	    ret = MAYBE;
    }
    எனில் (ret != FAIL
	    && ((expected->tt_flags & TTFLAG_VARARGS)
		!= (actual->tt_flags & TTFLAG_VARARGS))
	    && expected->tt_argcount != -1)
	ret = FAIL;
    எனில் (ret != FAIL && expected->tt_argcount != -1
	    && actual->tt_min_argcount != -1
	    && (actual->tt_argcount == -1
		|| (actual->tt_argcount < expected->tt_min_argcount
		    || actual->tt_argcount > expected->tt_argcount)))
	ret = FAIL;
    எனில் (ret != FAIL && expected->tt_args != வெற்று
	    && actual->tt_args != வெற்று)
    {
	முழு i;

	க்கு (i = 0; i < expected->tt_argcount
		&& i < actual->tt_argcount; ++i)
	{
	    where_T  func_where = where;
	    func_where.wt_kind = WT_METHOD_ARG;

	    // Allow for using "any" argument type, lambda's have them.
	    எனில் (actual->tt_args[i] != &t_any && check_type(
			expected->tt_args[i], actual->tt_args[i], FALSE,
			func_where) == FAIL)
	    {
		ret = FAIL;
		நிறுத்து;
	    }
	}
    }
    எனில் (ret == OK && expected->tt_argcount >= 0
	    && actual->tt_argcount == -1)
	// check the argument count at runtime
	ret = MAYBE;

    திரும்பு ret;
}

/*
 * Check if the expected and actual types match for an object
 * Returns OK if "expected" and "actual" are matching object types.
 * Returns FAIL if "expected" and "actual" are different types.
 * Returns MAYBE when a runtime type check is needed.
 */
    நிலையான முழு
check_object_type_maybe(
    type_T	*expected,
    type_T	*actual,
    where_T	where)
{
    முழு ret = OK;

    எனில் (actual->tt_type == VAR_ANY)
	திரும்பு MAYBE;	// use runtime type check
    எனில் (actual->tt_type != VAR_OBJECT)
	திரும்பு FAIL;	// don't use tt_class
    எனில் (actual->tt_class == வெற்று)    // null object
	திரும்பு OK;
    // t_object_any matches any object except for an enum item
    எனில் (expected == &t_object_any && !IS_ENUM(actual->tt_class))
	திரும்பு OK;

    // For object method arguments, do a invariant type check in
    // an extended class.  For all others, do a covariance type check.
    எனில் (where.wt_kind == WT_METHOD_ARG)
    {
	எனில் (actual->tt_class != expected->tt_class)
	    ret = FAIL;
    }
    இல்லையெனில் எனில் (!class_instance_of(actual->tt_class, expected->tt_class))
    {
	// Check if this is an up-cast, if so we'll have to check the type at
	// runtime.
	எனில் (where.wt_kind == WT_CAST &&
		class_instance_of(expected->tt_class, actual->tt_class))
	    ret = MAYBE;
	இல்லையெனில்
	    ret = FAIL;
    }

    திரும்பு ret;
}

/*
 * Check if the expected and actual types match.
 * Does not allow for assigning "any" to a specific type.
 * When "argidx" > 0 it is included in the error message.
 * Return OK if types match.
 * Return FAIL if types do not match.
 */
    முழு
check_type(
	type_T	*expected,
	type_T	*actual,
	முழு	give_செய்தி,
	where_T where)
{
    முழு ret = check_type_maybe(expected, actual, give_செய்தி, where);

    திரும்பு ret == MAYBE ? OK : ret;
}

/*
 * As check_type() but return MAYBE when a runtime type check should be used
 * when compiling.
 */
    முழு
check_type_maybe(
	type_T	*expected,
	type_T	*actual,
	முழு	give_செய்தி,
	where_T where)
{
    முழு ret = OK;

    // When expected is "unknown" we accept any actual type.
    // When expected is "any" we accept any actual type except "void".
    எனில் (expected->tt_type != VAR_UNKNOWN
	    && !(expected->tt_type == VAR_ANY && actual->tt_type != VAR_VOID))

    {
	// tt_type should match, except that a "partial" can be assigned to a
	// variable with type "func".
	// And "unknown" (using global variable) and "any" need a runtime type
	// check.
	எனில் (!(expected->tt_type == actual->tt_type
		    || actual->tt_type == VAR_UNKNOWN
		    || actual->tt_type == VAR_ANY
		    || (expected->tt_type == VAR_FUNC
					   && actual->tt_type == VAR_PARTIAL)))
	{
	    எனில் (expected->tt_type == VAR_BOOL
					&& (actual->tt_flags & TTFLAG_BOOL_OK))
		// Using number 0 or 1 for bool is OK.
		திரும்பு OK;
	    எனில் (expected->tt_type == VAR_FLOAT
		    && actual->tt_type == VAR_எண்
		    && ((expected->tt_flags & TTFLAG_எண்_OK)
			     || (actual->tt_flags & TTFLAG_FLOAT_OK)))
		// Using a number where a float is expected is OK here.
		திரும்பு OK;
	    எனில் (expected->tt_type == VAR_LIST
		    && actual->tt_type == VAR_TUPLE
		    && (expected->tt_flags & TTFLAG_TUPLE_OK))
		// Using a tuple where a list is expected is OK here.
		திரும்பு OK;
	    எனில் (give_செய்தி)
		type_mismatch_where(expected, actual, where);
	    திரும்பு FAIL;
	}
	எனில் (expected->tt_type == VAR_DICT || expected->tt_type == VAR_LIST)
	{
	    // "unknown" is used for an empty list or dict
	    எனில் (actual->tt_member != வெற்று && actual->tt_member != &t_unknown)
		ret = check_type_maybe(expected->tt_member, actual->tt_member,
								 FALSE, where);
	}
	இல்லையெனில் எனில் (expected->tt_type == VAR_TUPLE && actual != &t_any)
	    ret =  check_tuple_type_maybe(expected, actual, where);
	இல்லையெனில் எனில் (expected->tt_type == VAR_FUNC && actual != &t_any)
	    ret = check_func_type_maybe(expected, actual, where);
	இல்லையெனில் எனில் (expected->tt_type == VAR_OBJECT)
	    ret = check_object_type_maybe(expected, actual, where);

	எனில் (ret == FAIL && give_செய்தி)
	    type_mismatch_where(expected, actual, where);
    }

    எனில் (ret == OK && expected->tt_type != VAR_UNKNOWN
	    && expected->tt_type != VAR_ANY
	    && (actual->tt_type == VAR_UNKNOWN || actual->tt_type == VAR_ANY))
	// check the type at runtime
	ret = MAYBE;

    திரும்பு ret;
}

/*
 * Check that the arguments of "type" match "argvars[argcount]".
 * "base_tv" is from "expr->Func()".
 * Return OK/FAIL.
 */
    முழு
check_argument_types(
	type_T	    *type,
	typval_T    *argvars,
	முழு	    argcount,
	typval_T    *base_tv,
	char_u	    *name)
{
    முழு	    varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;
    முழு	    i;
    முழு	    totcount = argcount + (base_tv == வெற்று ? 0 : 1);

    எனில் (type->tt_type != VAR_FUNC && type->tt_type != VAR_PARTIAL)
	திரும்பு OK;  // just in case
    எனில் (totcount < type->tt_min_argcount - varargs)
    {
	emsg_funcname(e_not_enough_arguments_for_function_str, name);
	திரும்பு FAIL;
    }
    எனில் (!varargs && type->tt_argcount >= 0 && totcount > type->tt_argcount)
    {
	emsg_funcname(e_too_many_arguments_for_function_str, name);
	திரும்பு FAIL;
    }
    எனில் (type->tt_args == வெற்று)
	திரும்பு OK;  // cannot check


    க்கு (i = 0; i < totcount; ++i)
    {
	type_T	    *expected;
	typval_T    *tv;

	எனில் (base_tv != வெற்று)
	{
	    எனில் (i == 0)
		tv = base_tv;
	    இல்லையெனில்
		tv = &argvars[i - 1];
	}
	இல்லையெனில்
	    tv = &argvars[i];
	எனில் (varargs && i >= type->tt_argcount - 1)
	{
	    expected = type->tt_args[type->tt_argcount - 1];
	    எனில் (expected != வெற்று && expected->tt_type == VAR_LIST)
		expected = expected->tt_member;
	    எனில் (expected == வெற்று)
		expected = &t_any;
	}
	இல்லையெனில்
	    expected = type->tt_args[i];

	// check the type, unless the value is v:none
	எனில் ((tv->v_type != VAR_SPECIAL || tv->vval.v_எண் != VVAL_NONE)
		   && check_typval_arg_type(expected, tv, வெற்று, i + 1) == FAIL)
	    திரும்பு FAIL;
    }
    திரும்பு OK;
}

// Maximum nesting of types, to avoid running out of stack space.  E.g.
// "list<list<number>>" is nested twice.
#define MAX_TYPE_DEPTH	1000

// Current number of levels of parse_type() and the deepest level reached while
// parsing the current type.  The nesting is one less than the number of
// levels.
நிலையான முழு	parse_depth = 0;
நிலையான முழு	parse_max_depth = 0;

/*
 * Skip over type in list<type>, dict<type> or tuple<type>.
 * Returns a pointer to the character after the type.  "syn_error" is set to
 * TRUE on syntax error.
 */
    நிலையான char_u *
skip_member_type(char_u *start, char_u *p, முழு *syn_error)
{
    எனில் (STRNCMP("tuple", start, 5) == 0)
    {
	// handle tuple<{type1}, {type2}, ....<type>>
	p = skipwhite(p + 1);
	போது (*p != '>' && *p != NUL)
	{
	    char_u *sp = p;

	    எனில் (STRNCMP(p, "...", 3) == 0)
		p += 3;
	    p = skip_type(p, TRUE);
	    எனில் (p == sp)
	    {
		*syn_error = TRUE;
		திரும்பு p;  // syntax error
	    }
	    எனில் (*p == ',')
		p = skipwhite(p + 1);
	}
	எனில் (*p == '>')
	    p++;
    }
    இல்லையெனில்
    {
	p = skipwhite(p);
	p = skip_type(skipwhite(p + 1), FALSE);
	p = skipwhite(p);
	எனில் (*p == '>')
	    ++p;
    }

    திரும்பு p;
}

/*
 * Skip over a function type.  Returns a pointer to the character after the
 * type.  "syn_error" is set to TRUE on syntax error.
 */
    நிலையான char_u *
skip_func_type(char_u *p, முழு *syn_error)
{
    எனில் (*p == '(')
    {
	// handle func(args): type
	++p;
	போது (*p != ')' && *p != NUL)
	{
	    char_u *sp = p;

	    எனில் (STRNCMP(p, "...", 3) == 0)
		p += 3;
	    p = skip_type(p, TRUE);
	    எனில் (p == sp)
	    {
		*syn_error = TRUE;
		திரும்பு p;  // syntax error
	    }
	    எனில் (*p == ',')
		p = skipwhite(p + 1);
	}
	எனில் (*p == ')')
	{
	    எனில் (p[1] == ':')
		p = skip_type(skipwhite(p + 2), FALSE);
	    இல்லையெனில்
		++p;
	}
    }
    இல்லையெனில்
    {
	// handle func: return_type
	p = skip_type(skipwhite(p + 1), FALSE);
    }

    திரும்பு p;
}

/*
 * Inner part of skip_type().
 */
    நிலையான char_u *
skip_type_inner(char_u *start, முழு optional)
{
    char_u	*p = start;
    முழு		syn_error = FALSE;

    எனில் (optional && *p == '?')
	++p;

    // Also skip over "." for imported classes: "import.ClassName".
    போது (ASCII_ISALNUM(*p) || *p == '_' || *p == '.')
	++p;

    // Skip over "<type>"; this is permissive about white space.
    எனில் (*skipwhite(p) == '<')
    {
	p = skip_member_type(start, p, &syn_error);
	எனில் (syn_error)
	    திரும்பு p;
    }
    இல்லையெனில் எனில் ((*p == '(' || (*p == ':' && VIM_ISWHITE(p[1])))
					     && STRNCMP("func", start, 4) == 0)
    {
	// skip over function type
	p = skip_func_type(p, &syn_error);
	எனில் (syn_error)
	    திரும்பு p;
    }

    திரும்பு p;
}

/*
 * Skip over a type definition and return a pointer to just after it.
 * When "optional" is TRUE then a leading "?" is accepted.
 */
    char_u *
skip_type(char_u *start, முழு optional)
{
    நிலையான முழு	skip_depth = 0;
    char_u	*p = start;

    // Too deeply nested: skip to the matching ">" without recursing, an
    // error is given when parsing the type.
    எனில் (skip_depth > MAX_TYPE_DEPTH)
    {
	முழு	depth = 0;

	க்கு ( ; *p != NUL; ++p)
	{
	    எனில் (*p == '<')
		++depth;
	    இல்லையெனில் எனில் (*p == '>')
	    {
		எனில் (depth == 0)
		    நிறுத்து;
		எனில் (--depth == 0)
		    திரும்பு p + 1;
	    }
	    இல்லையெனில் எனில் (depth == 0 && !ASCII_ISALNUM(*p) && *p != '_'
								&& *p != '.')
		நிறுத்து;
	}
	திரும்பு p;
    }

    ++skip_depth;
    p = skip_type_inner(start, optional);
    --skip_depth;
    திரும்பு p;
}

/*
 * Parse the member type: "<type>" and return "type" with the member set.
 * Use "type_gap" if a new type needs to be added.
 * "info" is extra information for an error message.
 * Returns NULL in case of failure.
 */
    நிலையான type_T *
parse_type_member(
	char_u	    **arg,
	type_T	    *type,
	garray_T    *type_gap,
	முழு	    give_error,
	எழுத்து	    *info,
	ufunc_T	    *ufunc,
	cctx_T	    *cctx)
{
    char_u  *arg_start = *arg;
    type_T  *member_type;
    முழு	    prev_called_emsg = called_emsg;

    எனில் (**arg != '<')
    {
	எனில் (give_error)
	{
	    எனில் (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    இல்லையெனில்
		semsg(_(e_missing_type_after_str), info);
	}
	திரும்பு வெற்று;
    }
    *arg = skipwhite(*arg + 1);

    member_type = parse_type(arg, type_gap, ufunc, cctx, give_error);
    எனில் (member_type == வெற்று || !valid_declaration_type(member_type))
	திரும்பு வெற்று;

    *arg = skipwhite(*arg);
    எனில் (**arg != '>' && called_emsg == prev_called_emsg)
    {
	எனில் (give_error)
	    semsg(_(e_missing_gt_after_type_str), arg_start);
	திரும்பு வெற்று;
    }
    ++*arg;

    எனில் (type->tt_type == VAR_LIST)
	திரும்பு get_list_type(member_type, type_gap);
    திரும்பு get_dict_type(member_type, type_gap);
}

/*
 * Parse a "func" type at "*arg" and advance over it.
 * When "give_error" is TRUE give error messages, otherwise be quiet.
 * Return NULL for failure.
 */
    நிலையான type_T *
parse_type_func(
    char_u	**arg,
    size_t	len,
    garray_T	*type_gap,
    முழு		give_error,
    ufunc_T	*ufunc,
    cctx_T	*cctx)
{
    char_u  *p;
    type_T  *type;
    type_T  *ret_type = &t_unknown;
    முழு	argcount = -1;
    முழு	flags = 0;
    முழு	first_optional = -1;
    type_T	*arg_type[MAX_FUNC_ARGS + 1];

    // func({type}, ...{type}): {type}
    *arg += len;
    எனில் (**arg == '(')
    {
	// "func" may or may not return a value, "func()" does
	// not return a value.
	ret_type = &t_void;

	p = ++*arg;
	argcount = 0;
	போது (*p != NUL && *p != ')')
	{
	    எனில் (*p == '?')
	    {
		எனில் (first_optional == -1)
		    first_optional = argcount;
		++p;
	    }
	    இல்லையெனில் எனில் (STRNCMP(p, "...", 3) == 0)
	    {
		flags |= TTFLAG_VARARGS;
		p += 3;
	    }
	    இல்லையெனில் எனில் (first_optional != -1)
	    {
		எனில் (give_error)
		    emsg(_(e_mandatory_argument_after_optional_argument));
		திரும்பு வெற்று;
	    }

	    type = parse_type(&p, type_gap, ufunc, cctx, give_error);
	    எனில் (type == வெற்று || !valid_declaration_type(type))
		திரும்பு வெற்று;
	    எனில் ((flags & TTFLAG_VARARGS) != 0 && type->tt_type != VAR_LIST)
	    {
		எழுத்து *tofree;
		semsg(_(e_variable_arguments_type_must_be_list_str),
			type_name(type, &tofree));
		vim_free(tofree);
		திரும்பு வெற்று;
	    }
	    arg_type[argcount++] = type;

	    // Nothing comes after "...{type}".
	    எனில் (flags & TTFLAG_VARARGS)
		நிறுத்து;

	    எனில் (*p != ',' && *skipwhite(p) == ',')
	    {
		எனில் (give_error)
		    semsg(_(e_no_white_space_allowed_before_str_str),
			    ",", p);
		திரும்பு வெற்று;
	    }
	    எனில் (*p == ',')
	    {
		++p;
		எனில் (!VIM_ISWHITE(*p))
		{
		    எனில் (give_error)
			semsg(_(e_white_space_required_after_str_str),
				",", p - 1);
		    திரும்பு வெற்று;
		}
	    }
	    p = skipwhite(p);
	    எனில் (argcount == MAX_FUNC_ARGS)
	    {
		எனில் (give_error)
		    emsg(_(e_too_many_argument_types));
		திரும்பு வெற்று;
	    }
	}

	p = skipwhite(p);
	எனில் (*p != ')')
	{
	    எனில் (give_error)
		emsg(_(e_missing_closing_paren));
	    திரும்பு வெற்று;
	}
	*arg = p + 1;
    }
    எனில் (**arg == ':')
    {
	// parse return type
	++*arg;
	எனில் (!VIM_ISWHITE(**arg) && give_error)
	    semsg(_(e_white_space_required_after_str_str), ":", *arg - 1);
	*arg = skipwhite(*arg);
	ret_type = parse_type(arg, type_gap, ufunc, cctx, give_error);
	எனில் (ret_type == வெற்று)
	    திரும்பு வெற்று;
    }
    எனில் (flags == 0 && first_optional == -1 && argcount <= 0)
	type = get_func_type(ret_type, argcount, type_gap);
    இல்லையெனில்
    {
	type = alloc_func_type(ret_type, argcount, type_gap);
	type->tt_flags = flags;
	எனில் (argcount > 0)
	{
	    type->tt_argcount = argcount;
	    type->tt_min_argcount = first_optional == -1
						? argcount : first_optional;
	    எனில் (func_type_add_arg_types(type, argcount, type_gap) == FAIL)
		திரும்பு வெற்று;
	    mch_memmove(type->tt_args, arg_type, அளவு(type_T *) * argcount);
	}
    }

    திரும்பு type;
}

/*
 * Parse a "tuple" type at "*arg" and advance over it.
 * When "give_error" is TRUE give error messages, otherwise be quiet.
 * Return NULL for failure.
 */
    நிலையான type_T *
parse_type_tuple(
    char_u	**arg,
    garray_T	*type_gap,
    முழு		give_error,
    ufunc_T	*ufunc,
    cctx_T	*cctx)
{
    char_u	*p;
    type_T	*type;
    type_T	*ret_type = வெற்று;
    முழு		typecount = -1;
    முழு		flags = 0;
    garray_T	tuple_types_ga;

    ga_init2(&tuple_types_ga, அளவு(type_T *), 10);

    // tuple<{type}, {type}>
    // tuple<{type}, ...{type}>
    எனில் (**arg != '<')
    {
	எனில் (give_error)
	{
	    எனில் (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    இல்லையெனில்
		semsg(_(e_missing_type_after_str), "tuple");
	}

	// only "tuple" is specified
	திரும்பு வெற்று;
    }

    p = ++*arg;
    typecount = 0;
    போது (*p != NUL && *p != '>')
    {
	எனில் (STRNCMP(p, "...", 3) == 0)
	{
	    flags |= TTFLAG_VARARGS;
	    p += 3;
	}

	type = parse_type(&p, type_gap, ufunc, cctx, give_error);
	எனில் (type == வெற்று || !valid_declaration_type(type))
	    செல் on_err;

	எனில் ((flags & TTFLAG_VARARGS) != 0 && type->tt_type != VAR_LIST)
	{
	    எழுத்து *tofree;
	    semsg(_(e_variadic_tuple_must_end_with_list_type_str),
						type_name(type, &tofree));
	    vim_free(tofree);
	    செல் on_err;
	}

	// Add the item type
	எனில் (ga_grow(&tuple_types_ga, 1) == FAIL)
	    செல் on_err;
	((type_T **)tuple_types_ga.ga_data)[tuple_types_ga.ga_len] = type;
	tuple_types_ga.ga_len++;
	typecount++;

	// Nothing comes after "...{type}".
	எனில் (flags & TTFLAG_VARARGS)
	    நிறுத்து;

	எனில் (*p != ',' && *skipwhite(p) == ',')
	{
	    எனில் (give_error)
		semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
	    செல் on_err;
	}
	எனில் (*p == ',')
	{
	    ++p;
	    எனில் (!VIM_ISWHITE(*p))
	    {
		எனில் (give_error)
		    semsg(_(e_white_space_required_after_str_str),
			    ",", p - 1);
		செல் on_err;
	    }
	}
	p = skipwhite(p);
    }

    p = skipwhite(p);
    எனில் (*p != '>' || typecount <= 0)
    {
	எனில் (give_error)
	    semsg(_(e_missing_type_after_str), p);
	செல் on_err;
    }
    *arg = p + 1;

    ret_type = alloc_tuple_type(typecount, type_gap);
    எனில் (ret_type == &t_any
	    || tuple_type_add_types(ret_type, typecount, type_gap) == FAIL)
    {
	ret_type = வெற்று;
	செல் on_err;
    }
    ret_type->tt_flags = flags;
    ret_type->tt_argcount = typecount;
    mch_memmove(ret_type->tt_args, tuple_types_ga.ga_data,
						அளவு(type_T *) * typecount);

on_err:
    ga_clear(&tuple_types_ga);

    திரும்பு ret_type;
}

/*
 * Parse a "object" type at "*arg" and advance over it.
 * When "give_error" is TRUE give error messages, otherwise be quiet.
 * Return NULL for failure.
 */
    நிலையான type_T *
parse_type_object(
    char_u	**arg,
    garray_T	*type_gap,
    முழு		give_error,
    cctx_T	*cctx)
{
    char_u	*arg_start = *arg;
    type_T	*object_type;
    முழு		prev_called_emsg = called_emsg;

    // object<X> or object<any>
    எனில் (**arg != '<')
    {
	எனில் (give_error)
	{
	    எனில் (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    இல்லையெனில்
		semsg(_(e_missing_type_after_str), "object");
	}

	// only "object" is specified
	திரும்பு வெற்று;
    }

    // skip spaces following "object<"
    *arg = skipwhite(*arg + 1);

    object_type = parse_type(arg, type_gap, வெற்று, cctx, give_error);
    எனில் (object_type == வெற்று)
	திரும்பு வெற்று;

    *arg = skipwhite(*arg);
    எனில் (**arg != '>' && called_emsg == prev_called_emsg)
    {
	எனில் (give_error)
	    semsg(_(e_missing_gt_after_type_str), arg_start);
	திரும்பு வெற்று;
    }
    ++*arg;

    எனில் (object_type->tt_type == VAR_ANY)
	திரும்பு &t_object_any;

    எனில் (object_type->tt_type != VAR_OBJECT)
    {
	// specified type is not a class
	எனில் (give_error)
	    semsg(_(e_class_name_not_found_str), arg_start);
	திரும்பு வெற்று;
    }

    திரும்பு object_type;
}

/*
 * Parse a user defined type at "*arg" and advance over it.
 * It can be a class or an interface or a typealias name, possibly imported.
 * Return NULL if a type is not found.
 */
    நிலையான type_T *
parse_type_user_defined(
    char_u	**arg,
    size_t	len,
    garray_T	*type_gap,
    முழு		give_error,
    ufunc_T	*ufunc,
    cctx_T	*cctx)
{
    முழு		did_emsg_before = did_emsg;
    typval_T	tv;

    tv.v_type = VAR_UNKNOWN;
    எனில் (eval_variable_import(*arg, &tv) == OK)
    {
	எனில் (tv.v_type == VAR_CLASS && tv.vval.v_class != வெற்று)
	{
	    type_T *type = get_type_ptr(type_gap);
	    எனில் (type != வெற்று)
	    {
		// Although the name is that of a class or interface, the type
		// uses will be an object.
		type->tt_type = VAR_OBJECT;
		type->tt_class = tv.vval.v_class;
		clear_tv(&tv);

		*arg += len;
		// Skip over ".ClassName".
		போது (ASCII_ISALNUM(**arg) || **arg == '_' || **arg == '.')
		    ++*arg;

		திரும்பு type;
	    }
	}
	இல்லையெனில் எனில் (tv.v_type == VAR_TYPEALIAS)
	{
	    // user defined type
	    // The type alias may itself be a nested type, check the resulting
	    // nesting depth.
	    முழு	depth = parse_depth - 1
				+ MAX(tv.vval.v_typealias->ta_depth, 1);
	    எனில் (depth - 1 > MAX_TYPE_DEPTH)
	    {
		clear_tv(&tv);
		எனில் (give_error)
		    emsg(_(e_type_nested_too_deep));
		திரும்பு வெற்று;
	    }
	    எனில் (depth > parse_max_depth)
		parse_max_depth = depth;

	    type_T *type = copy_type(tv.vval.v_typealias->ta_type, type_gap);
	    *arg += len;
	    clear_tv(&tv);
	    // Skip over ".TypeName".
	    போது (ASCII_ISALNUM(**arg) || **arg == '_' || **arg == '.')
		++*arg;
	    திரும்பு type;
	}

	clear_tv(&tv);
    }

    // Check whether it is a generic type
    type_T *type = find_generic_type(*arg, len, ufunc, cctx);
    எனில் (type != வெற்று)
    {
	*arg += len;
	திரும்பு type;
    }

    எனில் (give_error && (did_emsg == did_emsg_before))
    {
	char_u	*p = skip_type(*arg, FALSE);
	எழுத்து	cc = *p;

	*p = NUL;
	semsg(_(e_type_not_recognized_str), *arg);
	*p = cc;
    }

    திரும்பு வெற்று;
}

/*
 * Inner part of parse_type().
 */
    நிலையான type_T *
parse_type_inner(
    char_u	**arg,
    garray_T	*type_gap,
    ufunc_T	*ufunc,
    cctx_T	*cctx,
    முழு		give_error)
{
    char_u  *p = *arg;
    size_t  len;

    // Skip over the first word.
    போது (ASCII_ISALNUM(*p) || *p == '_')
	++p;
    len = p - *arg;

    தேர்வு (**arg)
    {
	நிலை 'a':
	    எனில் (len == 3 && STRNCMP(*arg, "any", len) == 0)
	    {
		*arg += len;
		திரும்பு &t_any;
	    }
	    நிறுத்து;
	நிலை 'b':
	    எனில் (len == 4 && STRNCMP(*arg, "bool", len) == 0)
	    {
		*arg += len;
		திரும்பு &t_bool;
	    }
	    எனில் (len == 4 && STRNCMP(*arg, "blob", len) == 0)
	    {
		*arg += len;
		திரும்பு &t_blob;
	    }
	    நிறுத்து;
	நிலை 'c':
	    எனில் (len == 7 && STRNCMP(*arg, "channel", len) == 0)
	    {
		*arg += len;
		திரும்பு &t_channel;
	    }
	    நிறுத்து;
	நிலை 'd':
	    எனில் (len == 4 && STRNCMP(*arg, "dict", len) == 0)
	    {
		*arg += len;
		திரும்பு parse_type_member(arg, &t_dict_any, type_gap,
						give_error, "dict", ufunc,
						cctx);
	    }
	    நிறுத்து;
	நிலை 'f':
	    எனில் (len == 5 && STRNCMP(*arg, "float", len) == 0)
	    {
		*arg += len;
		திரும்பு &t_float;
	    }
	    எனில் (len == 4 && STRNCMP(*arg, "func", len) == 0)
		திரும்பு parse_type_func(arg, len, type_gap, give_error, ufunc,
									cctx);
	    நிறுத்து;
	நிலை 'j':
	    எனில் (len == 3 && STRNCMP(*arg, "job", len) == 0)
	    {
		*arg += len;
		திரும்பு &t_job;
	    }
	    நிறுத்து;
	நிலை 'l':
	    எனில் (len == 4 && STRNCMP(*arg, "list", len) == 0)
	    {
		*arg += len;
		திரும்பு parse_type_member(arg, &t_list_any, type_gap,
						give_error, "list", ufunc,
						cctx);
	    }
	    நிறுத்து;
	நிலை 'n':
	    எனில் (len == 6 && STRNCMP(*arg, "number", len) == 0)
	    {
		*arg += len;
		திரும்பு &t_எண்;
	    }
	    நிறுத்து;
	நிலை 'o':
	    எனில் (len == 6 && STRNCMP(*arg, "object", len) == 0)
	    {
		*arg += len;
		திரும்பு parse_type_object(arg, type_gap, give_error, cctx);
	    }
	    நிறுத்து;
	நிலை 's':
	    எனில் (len == 6 && STRNCMP(*arg, "string", len) == 0)
	    {
		*arg += len;
		திரும்பு &t_string;
	    }
	    நிறுத்து;
	நிலை 't':
	    எனில் (len == 5 && STRNCMP(*arg, "tuple", len) == 0)
	    {
		*arg += len;
		திரும்பு parse_type_tuple(arg, type_gap, give_error, ufunc,
									cctx);
	    }
	    நிறுத்து;
	நிலை 'v':
	    எனில் (len == 4 && STRNCMP(*arg, "void", len) == 0)
	    {
		*arg += len;
		திரும்பு &t_void;
	    }
	    நிறுத்து;
    }

    // User defined type
    திரும்பு parse_type_user_defined(arg, len, type_gap, give_error, ufunc,
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
    முழு		give_error)
{
    type_T	*type;

    எனில் (parse_depth > MAX_TYPE_DEPTH)
    {
	எனில் (give_error)
	    emsg(_(e_type_nested_too_deep));
	திரும்பு வெற்று;
    }
    எனில் (parse_depth == 0)
	parse_max_depth = 0;
    ++parse_depth;
    எனில் (parse_depth > parse_max_depth)
	parse_max_depth = parse_depth;
    type = parse_type_inner(arg, type_gap, ufunc, cctx, give_error);
    --parse_depth;
    திரும்பு type;
}

/*
 * Return the number of levels of the type last parsed by parse_type().
 */
    முழு
parse_type_depth(வெறுமை)
{
    திரும்பு parse_max_depth;
}

/*
 * Check if "type1" and "type2" are exactly the same.
 * "flags" can have ETYPE_ARG_UNKNOWN, which means that an unknown argument
 * type in "type1" is accepted.
 */
    முழு
equal_type(type_T *type1, type_T *type2, முழு flags)
{
    முழு i;

    எனில் (type1 == வெற்று || type2 == வெற்று)
	திரும்பு FALSE;
    எனில் (type1->tt_type != type2->tt_type)
	திரும்பு FALSE;
    தேர்வு (type1->tt_type)
    {
	நிலை VAR_UNKNOWN:
	நிலை VAR_ANY:
	நிலை VAR_VOID:
	நிலை VAR_SPECIAL:
	நிலை VAR_BOOL:
	நிலை VAR_எண்:
	நிலை VAR_FLOAT:
	நிலை VAR_STRING:
	நிலை VAR_BLOB:
	நிலை VAR_JOB:
	நிலை VAR_CHANNEL:
	நிலை VAR_INSTR:
	நிலை VAR_TYPEALIAS:
	    நிறுத்து;  // not composite is always OK
	நிலை VAR_OBJECT:
	நிலை VAR_CLASS:
	    // Objects are considered equal if they are from the same class
	    திரும்பு type1->tt_class == type2->tt_class;
	நிலை VAR_LIST:
	நிலை VAR_DICT:
	    திரும்பு equal_type(type1->tt_member, type2->tt_member, flags);
	நிலை VAR_TUPLE:
	    எனில் (type1->tt_argcount != type2->tt_argcount)
		திரும்பு FALSE;
	    எனில் (type1->tt_argcount < 0
			   || type1->tt_args == வெற்று || type2->tt_args == வெற்று)
		திரும்பு TRUE;
	    க்கு (i = 0; i < type1->tt_argcount; ++i)
		எனில் ((flags & ETYPE_ARG_UNKNOWN) == 0
			&& !equal_type(type1->tt_args[i], type2->tt_args[i],
									flags))
		    திரும்பு FALSE;
	    திரும்பு TRUE;
	நிலை VAR_FUNC:
	நிலை VAR_PARTIAL:
	    எனில் (!equal_type(type1->tt_member, type2->tt_member, flags)
		    || type1->tt_argcount != type2->tt_argcount)
		திரும்பு FALSE;
	    எனில் (type1->tt_argcount < 0
			   || type1->tt_args == வெற்று || type2->tt_args == வெற்று)
		திரும்பு TRUE;
	    க்கு (i = 0; i < type1->tt_argcount; ++i)
		எனில் ((flags & ETYPE_ARG_UNKNOWN) == 0
			&& !equal_type(type1->tt_args[i], type2->tt_args[i],
									flags))
		    திரும்பு FALSE;
	    திரும்பு TRUE;
    }
    திரும்பு TRUE;
}

/*
 * Find the common type of "type1" (VAR_FUNC) and "type2" (VAR_FUNC) and put it
 * in "dest".  "type2" and "dest" may be the same.
 */
    நிலையான வெறுமை
common_type_var_func(
    type_T	*type1,
    type_T	*type2,
    type_T	**dest,
    garray_T	*type_gap)
{
    type_T *common;

    // When one of the types is t_func_unknown return the other one.
    // Useful if a list or dict item is null_func.
    எனில் (type1 == &t_func_unknown)
    {
	*dest = type2;
	திரும்பு;
    }
    எனில் (type2 == &t_func_unknown)
    {
	*dest = type1;
	திரும்பு;
    }

    common_type(type1->tt_member, type2->tt_member, &common, type_gap);
    எனில் (type1->tt_argcount == type2->tt_argcount
	    && type1->tt_argcount >= 0)
    {
	முழு argcount = type1->tt_argcount;
	முழு i;

	*dest = alloc_func_type(common, argcount, type_gap);
	எனில் (type1->tt_args != வெற்று && type2->tt_args != வெற்று)
	{
	    எனில் (func_type_add_arg_types(*dest, argcount,
			type_gap) == OK)
		க்கு (i = 0; i < argcount; ++i)
		    common_type(type1->tt_args[i], type2->tt_args[i],
			    &(*dest)->tt_args[i], type_gap);
	}
    }
    இல்லையெனில்
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
    வெறுமை
common_type(type_T *type1, type_T *type2, type_T **dest, garray_T *type_gap)
{
    எனில் (equal_type(type1, type2, 0))
    {
	*dest = type1;
	திரும்பு;
    }

    // If either is VAR_UNKNOWN use the other type.  An empty list/dict has no
    // specific type.
    எனில் (type1 == வெற்று || type1->tt_type == VAR_UNKNOWN)
    {
	*dest = type2;
	திரும்பு;
    }
    எனில் (type2 == வெற்று || type2->tt_type == VAR_UNKNOWN)
    {
	*dest = type1;
	திரும்பு;
    }

    எனில் (type1->tt_type == type2->tt_type)
    {
	எனில் (type1->tt_type == VAR_LIST
		|| type1->tt_type == VAR_DICT)
	{
	    type_T *common;

	    common_type(type1->tt_member, type2->tt_member, &common, type_gap);
	    எனில் (type1->tt_type == VAR_LIST)
		*dest = get_list_type(common, type_gap);
	    இல்லையெனில்
		*dest = get_dict_type(common, type_gap);
	    திரும்பு;
	}
	இல்லையெனில் எனில் (type1->tt_type == VAR_FUNC)
	{
	    common_type_var_func(type1, type2, dest, type_gap);
	    திரும்பு;
	}
	இல்லையெனில் எனில் (type1->tt_type == VAR_OBJECT)
	{
	    *dest = &t_object_any;
	    திரும்பு;
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
    எனில் (type->tt_type == VAR_TUPLE)
    {
	எனில் (type->tt_argcount != 1)
	    திரும்பு &t_any;

	எனில் (type->tt_flags & TTFLAG_VARARGS)
	    திரும்பு type->tt_args[0]->tt_member;
	இல்லையெனில்
	    திரும்பு type->tt_args[0];
    }

    // For "any" the item type is not known.
    திரும்பு type->tt_member == வெற்று ? &t_any : type->tt_member;
}

/*
 * Push an entry onto the type stack.  "type" used both for the current type
 * and the declared type.
 * Returns FAIL when out of memory.
 */
    முழு
தள்ளு_type_அடுக்கு(cctx_T *cctx, type_T *type)
{
    திரும்பு தள்ளு_type_stack2(cctx, type, type);
}

/*
 * Push an entry onto the type stack.  "type" is the current type, "decl_type"
 * is the declared type.
 * Returns FAIL when out of memory.
 */
    முழு
தள்ளு_type_stack2(cctx_T *cctx, type_T *type, type_T *decl_type)
{
    garray_T	*அடுக்கு = &cctx->ctx_type_அடுக்கு;
    type2_T	*typep;

    எனில் (GA_GROW_FAILS(அடுக்கு, 1))
	திரும்பு FAIL;
    typep = ((type2_T *)அடுக்கு->ga_data) + அடுக்கு->ga_len;
    typep->type_curr = type;
    typep->type_decl = decl_type;
    ++அடுக்கு->ga_len;
    திரும்பு OK;
}

/*
 * Set the type of the top of the stack to "type".
 */
    வெறுமை
set_type_on_அடுக்கு(cctx_T *cctx, type_T *type, முழு offset)
{
    garray_T	*அடுக்கு = &cctx->ctx_type_அடுக்கு;
    type2_T	*typep = ((type2_T *)அடுக்கு->ga_data)
						  + அடுக்கு->ga_len - 1 - offset;

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
get_type_on_அடுக்கு(cctx_T *cctx, முழு offset)
{
    garray_T	*அடுக்கு = &cctx->ctx_type_அடுக்கு;

    எனில் (offset + 1 > அடுக்கு->ga_len)
	திரும்பு &t_unknown;
    திரும்பு (((type2_T *)அடுக்கு->ga_data) + அடுக்கு->ga_len - offset - 1)
								   ->type_curr;
}

/*
 * Get the declared type from the type stack.  If "offset" is zero the one at
 * the top,
 * if "offset" is one the type above that, etc.
 * Returns &t_unknown if there is no such stack entry.
 */
    type_T *
get_decl_type_on_அடுக்கு(cctx_T *cctx, முழு offset)
{
    garray_T	*அடுக்கு = &cctx->ctx_type_அடுக்கு;

    எனில் (offset + 1 > அடுக்கு->ga_len)
	திரும்பு &t_unknown;
    திரும்பு (((type2_T *)அடுக்கு->ga_data) + அடுக்கு->ga_len - offset - 1)
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
get_member_type_from_அடுக்கு(
	முழு	    மொத்தம்,
	முழு	    skip,
	cctx_T	    *cctx)
{
    garray_T	*அடுக்கு = &cctx->ctx_type_அடுக்கு;
    type2_T	*typep;
    garray_T    *type_gap = cctx->ctx_type_list;
    முழு		i;
    type_T	*result;
    type_T	*type;

    // Use "unknown" for an empty list or dict.
    எனில் (மொத்தம் == 0)
	திரும்பு &t_unknown;
    // Find the common type from following items.
    typep = ((type2_T *)அடுக்கு->ga_data) + அடுக்கு->ga_len;
    result = &t_unknown;
    க்கு (i = 0; i < மொத்தம்; ++i)
    {
	type = (typep -((மொத்தம் - i) * skip) + skip - 1)->type_curr;
	எனில் (check_type_is_மதிப்பு(type) == FAIL)
	    திரும்பு வெற்று;
	எனில் (result != &t_any)
	    common_type(type, result, &result, type_gap);
    }

    திரும்பு result;
}

/*
 * Get the types of items in a tuple on the stack of "cctx".
 * Returns the number of types.  Returns -1 on failure.
 */
    முழு
get_tuple_type_from_அடுக்கு(
    முழு		மொத்தம்,
    garray_T	*tuple_types_gap,
    cctx_T	*cctx)
{
    garray_T	*அடுக்கு = &cctx->ctx_type_அடுக்கு;
    type2_T	*typep;
    type_T	*type = வெற்று;

    // Use "unknown" for an empty tuple
    எனில் (மொத்தம் == 0)
	திரும்பு 0;

    // Find the common type from following items.
    typep = ((type2_T *)அடுக்கு->ga_data) + அடுக்கு->ga_len;
    க்கு (முழு i = 0; i < மொத்தம்; i++)
    {
	type = (typep - (மொத்தம் - i))->type_curr;
	எனில் (check_type_is_மதிப்பு(type) == FAIL)
	    திரும்பு -1;
	எனில் (ga_grow(tuple_types_gap, 1) == FAIL)
	    திரும்பு -1;
	((type_T **)tuple_types_gap->ga_data)[tuple_types_gap->ga_len] = type;
	tuple_types_gap->ga_len++;
    }

    திரும்பு tuple_types_gap->ga_len;
}

    எழுத்து *
vartype_name(vartype_T type)
{
    தேர்வு (type)
    {
	நிலை VAR_UNKNOWN: நிறுத்து;
	நிலை VAR_ANY: திரும்பு "any";
	நிலை VAR_VOID: திரும்பு "void";
	நிலை VAR_SPECIAL: திரும்பு "special";
	நிலை VAR_BOOL: திரும்பு "bool";
	நிலை VAR_எண்: திரும்பு "number";
	நிலை VAR_FLOAT: திரும்பு "float";
	நிலை VAR_STRING: திரும்பு "string";
	நிலை VAR_BLOB: திரும்பு "blob";
	நிலை VAR_JOB: திரும்பு "job";
	நிலை VAR_CHANNEL: திரும்பு "channel";
	நிலை VAR_LIST: திரும்பு "list";
	நிலை VAR_TUPLE: திரும்பு "tuple";
	நிலை VAR_DICT: திரும்பு "dict";
	நிலை VAR_INSTR: திரும்பு "instr";
	நிலை VAR_CLASS: திரும்பு "class";
	நிலை VAR_OBJECT: திரும்பு "object";
	நிலை VAR_TYPEALIAS: திரும்பு "typealias";

	நிலை VAR_FUNC:
	நிலை VAR_PARTIAL: திரும்பு "func";
    }
    திரும்பு "unknown";
}

/*
 * Return the type name of a List (list<type>) or Dict (dict<type>).
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    நிலையான எழுத்து *
type_name_list_or_dict(எழுத்து *name, type_T *type, எழுத்து **tofree)
{
    எழுத்து *member_free;
    எழுத்து *member_name;

    எனில் (type->tt_member->tt_type == VAR_UNKNOWN)
	member_name = type_name(&t_any, &member_free);
    இல்லையெனில்
	member_name = type_name(type->tt_member, &member_free);

    size_t len = STRLEN(name) + STRLEN(member_name) + 3;
    *tofree = alloc(len);
    எனில் (*tofree == வெற்று)
    {
	vim_free(member_free);
	திரும்பு name;
    }

    vim_snprintf(*tofree, len, "%s<%s>", name, member_name);
    vim_free(member_free);
    திரும்பு *tofree;
}

/*
 * Return the type name of a tuple.
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    நிலையான எழுத்து *
type_name_tuple(type_T *type, எழுத்து **tofree)
{
    garray_T    ga;
    முழு		i;
    முழு		varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;
    எழுத்து	*arg_free = வெற்று;

    ga_init2(&ga, 1, 100);
    எனில் (ga_grow(&ga, 20) == FAIL)
	செல் failed;
    STRCPY(ga.ga_data, "tuple<");
    ga.ga_len += 6;

    எனில் (type->tt_argcount <= 0)
	// empty tuple
	GA_CONCAT_LITERAL(&ga, "any");
    இல்லையெனில்
    {
	எனில் (type->tt_args == வெற்று)
	    GA_CONCAT_LITERAL(&ga, "[unknown]");
	இல்லையெனில்
	{
	    க்கு (i = 0; i < type->tt_argcount; ++i)
	    {
		string_T    arg_type;

		arg_type.string = (char_u *)type_name(type->tt_args[i], &arg_free);
		எனில் (i > 0)
		{
		    STRCPY((எழுத்து *)ga.ga_data + ga.ga_len, ", ");
		    ga.ga_len += 2;
		}
		arg_type.length = STRLEN(arg_type.string);
		எனில் (ga_grow(&ga, (முழு)arg_type.length + 8) == FAIL)
		    செல் failed;
		எனில் (varargs && i == type->tt_argcount - 1)
		    GA_CONCAT_LITERAL(&ga, "...");
		ga_concat_len(&ga, arg_type.string, arg_type.length);
		VIM_CLEAR(arg_free);
	    }
	}
    }

    STRCPY((எழுத்து *)ga.ga_data + ga.ga_len, ">");
    *tofree = ga.ga_data;
    திரும்பு ga.ga_data;

failed:
    vim_free(arg_free);
    ga_clear(&ga);
    திரும்பு "[unknown]";
}

/*
 * Return the type name of a Class (class<name>) or Object (object<name>).
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    நிலையான எழுத்து *
type_name_class_or_obj(எழுத்து *name, type_T *type, எழுத்து **tofree)
{
    string_T	class_name;

    எனில் (type->tt_class != வெற்று)
    {
	class_name.string = type->tt_class->class_name.string;
	class_name.length = type->tt_class->class_name.length;
	எனில் (IS_ENUM(type->tt_class))
	    name = "enum";
    }
    இல்லையெனில்
	STR_LITERAL_SET(class_name, "any");

    size_t len = STRLEN(name) + class_name.length + 3;
    *tofree = alloc(len);
    எனில் (*tofree == வெற்று)
	திரும்பு name;

    vim_snprintf(*tofree, len, "%s<%s>", name, class_name.string);
    திரும்பு *tofree;
}

/*
 * Return the type name of a function.
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    நிலையான எழுத்து *
type_name_func(type_T *type, எழுத்து **tofree)
{
    garray_T    ga;
    முழு		i;
    முழு		varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;
    எழுத்து	*arg_free = வெற்று;

    ga_init2(&ga, 1, 100);
    எனில் (ga_grow(&ga, 20) == FAIL)
	செல் failed;
    STRCPY(ga.ga_data, "func(");
    ga.ga_len += 5;

    க்கு (i = 0; i < type->tt_argcount; ++i)
    {
	string_T    arg_type;

	எனில் (type->tt_args == வெற்று)
	    STR_LITERAL_SET(arg_type, "[unknown]");
	இல்லையெனில்
	{
	    arg_type.string = (char_u *)type_name(type->tt_args[i], &arg_free);
	    arg_type.length = STRLEN(arg_type.string);
	}
	எனில் (i > 0)
	{
	    STRCPY((எழுத்து *)ga.ga_data + ga.ga_len, ", ");
	    ga.ga_len += 2;
	}
	எனில் (ga_grow(&ga, (முழு)arg_type.length + 8) == FAIL)
	    செல் failed;
	எனில் (varargs && i == type->tt_argcount - 1)
	    GA_CONCAT_LITERAL(&ga, "...");
	இல்லையெனில் எனில் (i >= type->tt_min_argcount)
	    *((எழுத்து *)ga.ga_data + ga.ga_len++) = '?';
	ga_concat_len(&ga, arg_type.string, arg_type.length);
	VIM_CLEAR(arg_free);
    }
    எனில் (type->tt_argcount < 0)
	// any number of arguments
	GA_CONCAT_LITERAL(&ga, "...");

    எனில் (type->tt_member == &t_void)
	STRCPY((எழுத்து *)ga.ga_data + ga.ga_len, ")");
    இல்லையெனில்
    {
	எழுத்து *ret_free = வெற்று;
	எழுத்து *ret_name = type_name(type->tt_member, &ret_free);
	முழு  len;

	len = (முழு)STRLEN(ret_name) + 4;
	எனில் (ga_grow(&ga, len) == FAIL)
	{
	    vim_free(ret_free);
	    செல் failed;
	}
	STRCPY((எழுத்து *)ga.ga_data + ga.ga_len, "): ");
	STRCPY((எழுத்து *)ga.ga_data + ga.ga_len + 3, ret_name);
	vim_free(ret_free);
    }
    *tofree = ga.ga_data;
    திரும்பு ga.ga_data;

failed:
    vim_free(arg_free);
    ga_clear(&ga);
    திரும்பு "[unknown]";
}

/*
 * Return the name of a type.
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    எழுத்து *
type_name(type_T *type, எழுத்து **tofree)
{
    எழுத்து *name;

    *tofree = வெற்று;
    எனில் (type == வெற்று)
	திரும்பு "[unknown]";
    name = vartype_name(type->tt_type);

    தேர்வு (type->tt_type)
    {
	நிலை VAR_LIST:
	நிலை VAR_DICT:
	    திரும்பு type_name_list_or_dict(name, type, tofree);

	நிலை VAR_TUPLE:
	    திரும்பு type_name_tuple(type, tofree);

	நிலை VAR_CLASS:
	நிலை VAR_OBJECT:
	    திரும்பு type_name_class_or_obj(name, type, tofree);

	நிலை VAR_FUNC:
	    திரும்பு type_name_func(type, tofree);

	இயல்பு:
	    நிறுத்து;
    }

    திரும்பு name;
}

/*
 * "typename(expr)" function
 */
    வெறுமை
f_typename(typval_T *argvars, typval_T *rettv)
{
    garray_T	type_list;
    type_T	*type;
    எழுத்து	*tofree;
    எழுத்து	*name;

    rettv->v_type = VAR_STRING;
    ga_init2(&type_list, அளவு(type_T *), 10);
    எனில் (argvars[0].v_type == VAR_TYPEALIAS)
    {
	type = copy_type(argvars[0].vval.v_typealias->ta_type, &type_list);
	// A type alias for a class has the type set to VAR_OBJECT.  Change it
	// to VAR_CLASS, so that the name is "typealias<class<xxx>>"
	எனில் (type->tt_type == VAR_OBJECT)
	    type->tt_type = VAR_CLASS;
    }
    இல்லையெனில்
	type = typval2type(argvars, get_copyID(), &type_list, TVTT_DO_MEMBER);
    name = type_name(type, &tofree);
    எனில் (argvars[0].v_type == VAR_TYPEALIAS)
    {
	vim_snprintf((எழுத்து *)IObuff, IOSIZE, "typealias<%s>", name);
	rettv->vval.v_string = vim_strsave((char_u *)IObuff);
	எனில் (tofree != வெற்று)
	    vim_free(tofree);
    }
    இல்லையெனில்
    {
	எனில் (tofree != வெற்று)
	    rettv->vval.v_string = (char_u *)tofree;
	இல்லையெனில்
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
    முழு
check_typval_is_மதிப்பு(typval_T *tv)
{
    எனில் (tv == வெற்று)
	திரும்பு OK;

    தேர்வு (tv->v_type)
    {
	நிலை VAR_CLASS:
	    {
		class_T *cl = tv->vval.v_class;
		char_u *class_name = (cl == வெற்று) ? (char_u *)""
							: cl->class_name.string;
		எனில் (cl != வெற்று && IS_ENUM(cl))
		    semsg(_(e_using_enum_as_மதிப்பு_str), class_name);
		இல்லையெனில்
		    semsg(_(e_using_class_as_மதிப்பு_str), class_name);
	    }
	    திரும்பு FAIL;

	நிலை VAR_TYPEALIAS:
	    semsg(_(e_using_typealias_as_மதிப்பு_str),
		    tv->vval.v_typealias->ta_name);
	    திரும்பு FAIL;

	இயல்பு:
	    நிறுத்து;
    }
    திரும்பு OK;
}

/*
 * Same as above, except check type_T.
 */
    முழு
check_type_is_மதிப்பு(type_T *type)
{
    எனில் (type == வெற்று)
	திரும்பு OK;
    தேர்வு (type->tt_type)
    {
	நிலை VAR_CLASS:
	    எனில் (type->tt_class != வெற்று && IS_ENUM(type->tt_class))
		semsg(_(e_using_enum_as_மதிப்பு_str),
			type->tt_class->class_name.string);
	    இல்லையெனில்
		semsg(_(e_using_class_as_மதிப்பு_str),
			type->tt_class == வெற்று ? (char_u *)""
			: type->tt_class->class_name.string);
	    திரும்பு FAIL;

	நிலை VAR_TYPEALIAS:
	    // TODO: Not sure what could be done here to get a name.
	    //       Maybe an optional argument?
	    emsg(_(e_using_typealias_as_var_val));
	    திரும்பு FAIL;

	இயல்பு:
	    நிறுத்து;
    }
    திரும்பு OK;
}

#endif // FEAT_EVAL
