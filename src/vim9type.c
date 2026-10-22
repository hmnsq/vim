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
#അന്തര്ഭാവ "vim.h"

#എങ്കിൽ defined(FEAT_EVAL)

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

    എങ്കിൽ (ga_grow(type_gap, 1) == FAIL)
	തിരികെ ശൂന്യ;
    type = ALLOC_CLEAR_ONE(type_T);
    എങ്കിൽ (type == ശൂന്യ)
	തിരികെ ശൂന്യ;

    ((type_T **)type_gap->ga_data)[type_gap->ga_len] = type;
    ++type_gap->ga_len;
    തിരികെ type;
}

/*
 * Make a shallow copy of "type".
 * When allocation fails returns "type".
 */
    type_T *
copy_type(type_T *type, garray_T *type_gap)
{
    type_T *copy = get_type_ptr(type_gap);

    എങ്കിൽ (copy == ശൂന്യ)
	തിരികെ type;
    *copy = *type;
    copy->tt_flags &= ~TTFLAG_STATIC;

    എങ്കിൽ (type->tt_args != ശൂന്യ
	   && func_type_add_arg_types(copy, type->tt_argcount, type_gap) == OK)
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < type->tt_argcount; ++i)
	    copy->tt_args[i] = type->tt_args[i];

    തിരികെ copy;
}

/*
 * Inner part of copy_type_deep().
 * When allocation fails returns "type".
 */
    സ്ഥിതം type_T *
copy_type_deep_rec(type_T *type, garray_T *type_gap, garray_T *seen_types)
{
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < seen_types->ga_len; ++i)
	എങ്കിൽ (((type_T **)seen_types->ga_data)[i * 2] == type)
	    // seen this type before, return the copy we made
	    തിരികെ ((type_T **)seen_types->ga_data)[i * 2 + 1];

    type_T *copy = copy_type(type, type_gap);
    എങ്കിൽ (ga_grow(seen_types, 1) == FAIL)
	തിരികെ copy;
    ((type_T **)seen_types->ga_data)[seen_types->ga_len * 2] = type;
    ((type_T **)seen_types->ga_data)[seen_types->ga_len * 2 + 1] = copy;
    ++seen_types->ga_len;

    എങ്കിൽ (copy->tt_member != ശൂന്യ)
	copy->tt_member = copy_type_deep_rec(copy->tt_member,
							 type_gap, seen_types);
    എങ്കിൽ (type->tt_args != ശൂന്യ)
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < type->tt_argcount; ++i)
	    copy->tt_args[i] = copy_type_deep_rec(copy->tt_args[i],
							 type_gap, seen_types);

    തിരികെ copy;
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
    ga_init2(&seen_types, വലുപ്പം(type_T *) * 2, 20);

    type_T *res = copy_type_deep_rec(type, type_gap, &seen_types);

    ga_clear(&seen_types);
    തിരികെ res;
}

    ശൂന്യം
clear_type_list(garray_T *gap)
{
    ആകുമ്പോൾ (gap->ga_len > 0)
	vim_free(((type_T **)gap->ga_data)[--gap->ga_len]);
    ga_clear(gap);
}

    ശൂന്യം
clear_func_type_list(garray_T *gap, type_T **func_type)
{
    ആകുമ്പോൾ (gap->ga_len > 0)
    {
	// func_type pointing to the uf_type_list, so reset pointer
	എങ്കിൽ (*func_type == ((type_T **)gap->ga_data)[--gap->ga_len])
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

    എങ്കിൽ (type == ശൂന്യ)
	തിരികെ ശൂന്യ;

    // A fixed type never contains allocated types, return as-is.
    എങ്കിൽ (type->tt_flags & TTFLAG_STATIC)
	തിരികെ type;

    ret = ALLOC_ONE(type_T);
    എങ്കിൽ (ret == ശൂന്യ)
	തിരികെ ശൂന്യ;
    *ret = *type;

    എങ്കിൽ (ret->tt_member != ശൂന്യ)
	ret->tt_member = alloc_type(ret->tt_member);

    എങ്കിൽ (type->tt_argcount > 0 && type->tt_args != ശൂന്യ)
    {
	പൂർണ്ണസംഖ്യ i;

	ret->tt_args = ALLOC_MULT(type_T *, type->tt_argcount);
	എങ്കിൽ (ret->tt_args != ശൂന്യ)
	    വേണ്ടി (i = 0; i < type->tt_argcount; ++i)
		ret->tt_args[i] = alloc_type(type->tt_args[i]);
    }
    അല്ലെങ്കിൽ
	ret->tt_args = ശൂന്യ;

    തിരികെ ret;
}

/*
 * Free a type that was created with alloc_type().
 */
    ശൂന്യം
free_type(type_T *type)
{
    പൂർണ്ണസംഖ്യ i;

    എങ്കിൽ (type == ശൂന്യ || (type->tt_flags & TTFLAG_STATIC))
	തിരികെ;
    എങ്കിൽ (type->tt_args != ശൂന്യ)
    {
	വേണ്ടി (i = 0; i < type->tt_argcount; ++i)
	    free_type(type->tt_args[i]);
	vim_free(type->tt_args);
    }

    free_type(type->tt_member);

    vim_free(type);
}

/*
 * Return TRUE if "type" is to be recursed into for setting the type.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
set_tv_type_recurse(type_T *type)
{
    തിരികെ type->tt_member != ശൂന്യ
		&& (type->tt_member->tt_type == VAR_DICT
				|| type->tt_member->tt_type == VAR_LIST)
		&& type->tt_member->tt_member != ശൂന്യ
		&& type->tt_member->tt_member != &t_any
		&& type->tt_member->tt_member != &t_unknown;
}

/*
 * Set the type of Dict "d" to "type"
 */
    സ്ഥിതം ശൂന്യം
set_tv_type_dict(dict_T *d, type_T *type)
{
    എങ്കിൽ (d->dv_type == type)
	തിരികെ;

    free_type(d->dv_type);
    d->dv_type = alloc_type(type);

    // Need to recursively set the type of dict items?
    എങ്കിൽ (!set_tv_type_recurse(type))
	തിരികെ;

    പൂർണ്ണസംഖ്യ		todo = (പൂർണ്ണസംഖ്യ)d->dv_hashtab.ht_used;
    hashitem_T	*hi;
    dictitem_T	*di;

    FOR_ALL_HASHTAB_ഇനങ്ങൾ(&d->dv_hashtab, hi, todo)
    {
	എങ്കിൽ (!HASHITEM_EMPTY(hi))
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
    സ്ഥിതം ശൂന്യം
set_tv_type_list(list_T *l, type_T *type)
{
    എങ്കിൽ (l->lv_type == type)
	തിരികെ;

    free_type(l->lv_type);
    l->lv_type = alloc_type(type);

    // Need to recursively set the type of list items?
    എങ്കിൽ (l->lv_first == &range_list_item || !set_tv_type_recurse(type))
	തിരികെ;

    listitem_T	*li;

    FOR_ALL_LIST_ഇനങ്ങൾ(l, li)
	set_tv_type(&li->li_tv, type->tt_member);
}

/*
 * Set the type of Tuple "tuple" to "type"
 */
    സ്ഥിതം ശൂന്യം
set_tv_type_tuple(tuple_T *tuple, type_T *type)
{
    എങ്കിൽ (tuple->tv_type == type)
	തിരികെ;

    free_type(tuple->tv_type);
    tuple->tv_type = alloc_type(type);

    എങ്കിൽ (type->tt_argcount <= 0)
	തിരികെ;

    // recursively set the type of list items
    type_T	*item_type;
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < tuple_len(tuple); i++)
    {
	എങ്കിൽ ((type->tt_flags & TTFLAG_VARARGS) && (i >= type->tt_argcount - 1))
	    // For a variadic tuple, the last type is a List.  So use the
	    // List member type.
	    item_type = type->tt_args[type->tt_argcount - 1]->tt_member;
	അല്ലെങ്കിൽ
	    item_type = type->tt_args[i];

	set_tv_type(TUPLE_ITEM(tuple, i), item_type);
    }
}

/*
 * Set the type of "tv" to "type" if it is a list or tuple or dict.
 */
    ശൂന്യം
set_tv_type(typval_T *tv, type_T *type)
{
    എങ്കിൽ (type->tt_type == VAR_ANY)
	// If the variable type is "any", then keep the value type.
	// e.g.  var x: any = [1, 2] or var y: any = {v: 1}
	തിരികെ;

    എങ്കിൽ (tv->v_type == VAR_DICT && tv->vval.v_dict != ശൂന്യ)
	set_tv_type_dict(tv->vval.v_dict, type);
    അല്ലെങ്കിൽ എങ്കിൽ (tv->v_type == VAR_LIST && tv->vval.v_list != ശൂന്യ)
	set_tv_type_list(tv->vval.v_list, type);
    അല്ലെങ്കിൽ എങ്കിൽ (tv->v_type == VAR_TUPLE && tv->vval.v_tuple != ശൂന്യ)
	set_tv_type_tuple(tv->vval.v_tuple, type);
}

/*
 * For a tuple type, reserve space for "typecount" types (including the
 * repeated type).
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
tuple_type_add_types(
    type_T	*tupletype,
    പൂർണ്ണസംഖ്യ		typecount,
    garray_T	*type_gap)
{
    // To make it easy to free the space needed for the types, add the
    // pointer to type_gap.
    എങ്കിൽ (ga_grow(type_gap, 1) == FAIL)
	തിരികെ FAIL;
    tupletype->tt_args = alloc_clear_id(വലുപ്പം(type_T *) * typecount,
							  aid_tuple_type_args);
    എങ്കിൽ (tupletype->tt_args == ശൂന്യ)
	തിരികെ FAIL;
    ((type_T **)type_gap->ga_data)[type_gap->ga_len] =
						(ശൂന്യം *)tupletype->tt_args;
    ++type_gap->ga_len;
    തിരികെ OK;
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
    എങ്കിൽ (member_type == ശൂന്യ || (member_type->tt_type == VAR_ANY
					&& !IS_GENERIC_TYPE(member_type)))
	തിരികെ &t_list_any;
    എങ്കിൽ (member_type->tt_type == VAR_VOID
	    || member_type->tt_type == VAR_UNKNOWN)
	തിരികെ &t_list_empty;
    എങ്കിൽ (member_type->tt_type == VAR_BOOL)
	തിരികെ &t_list_bool;
    എങ്കിൽ (member_type->tt_type == VAR_സംഖ്യ)
	തിരികെ &t_list_സംഖ്യ;
    എങ്കിൽ (member_type->tt_type == VAR_STRING)
	തിരികെ &t_list_string;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    എങ്കിൽ (type == ശൂന്യ)
	തിരികെ &t_any;
    type->tt_type = VAR_LIST;
    type->tt_member = member_type;
    type->tt_argcount = 0;
    type->tt_args = ശൂന്യ;
    തിരികെ type;
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
    പൂർണ്ണസംഖ്യ		typecount = tuple_types_gap->ga_len;

    // recognize commonly used types
    എങ്കിൽ (typecount == 0)
	തിരികെ &t_tuple_any;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    എങ്കിൽ (type == ശൂന്യ)
	തിരികെ &t_any;
    type->tt_type = VAR_TUPLE;
    type->tt_member = ശൂന്യ;
    എങ്കിൽ (typecount > 0)
    {
	എങ്കിൽ (tuple_type_add_types(type, typecount, type_gap) == FAIL)
	    തിരികെ &t_any;
	mch_memmove(type->tt_args, tuple_types, വലുപ്പം(type_T *) * typecount);
    }
    type->tt_argcount = typecount;
    type->tt_flags = 0;

    തിരികെ type;
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
    എങ്കിൽ (member_type == ശൂന്യ || (member_type->tt_type == VAR_ANY
					&& !IS_GENERIC_TYPE(member_type)))
	തിരികെ &t_dict_any;
    എങ്കിൽ (member_type->tt_type == VAR_VOID
	    || member_type->tt_type == VAR_UNKNOWN)
	തിരികെ &t_dict_empty;
    എങ്കിൽ (member_type->tt_type == VAR_BOOL)
	തിരികെ &t_dict_bool;
    എങ്കിൽ (member_type->tt_type == VAR_സംഖ്യ)
	തിരികെ &t_dict_സംഖ്യ;
    എങ്കിൽ (member_type->tt_type == VAR_STRING)
	തിരികെ &t_dict_string;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    എങ്കിൽ (type == ശൂന്യ)
	തിരികെ &t_any;
    type->tt_type = VAR_DICT;
    type->tt_member = member_type;
    type->tt_argcount = 0;
    type->tt_args = ശൂന്യ;
    തിരികെ type;
}

/*
 * Allocate a new type for a function.
 */
    type_T *
alloc_func_type(type_T *ret_type, പൂർണ്ണസംഖ്യ argcount, garray_T *type_gap)
{
    type_T *type = get_type_ptr(type_gap);

    എങ്കിൽ (type == ശൂന്യ)
	തിരികെ &t_any;
    type->tt_type = VAR_FUNC;
    type->tt_member = ret_type == ശൂന്യ ? &t_unknown : ret_type;
    type->tt_argcount = argcount;
    type->tt_args = ശൂന്യ;
    തിരികെ type;
}

/*
 * Allocate a new type for a tuple.
 */
    സ്ഥിതം type_T *
alloc_tuple_type(പൂർണ്ണസംഖ്യ typecount, garray_T *type_gap)
{
    type_T *type = get_type_ptr(type_gap);

    എങ്കിൽ (type == ശൂന്യ)
	തിരികെ &t_any;
    type->tt_type = VAR_TUPLE;
    type->tt_member = ശൂന്യ;
    type->tt_argcount = typecount;
    type->tt_args = ശൂന്യ;
    തിരികെ type;
}

/*
 * Get a function type, based on the return type "ret_type".
 * "argcount" must be -1 or 0, a predefined type can be used.
 */
    type_T *
get_func_type(type_T *ret_type, പൂർണ്ണസംഖ്യ argcount, garray_T *type_gap)
{
    // recognize commonly used types
    എങ്കിൽ (ret_type == &t_unknown || ret_type == ശൂന്യ)
    {
	// (argcount == 0) is not possible
	തിരികെ &t_func_unknown;
    }
    എങ്കിൽ (ret_type == &t_void)
    {
	എങ്കിൽ (argcount == 0)
	    തിരികെ &t_func_0_void;
	അല്ലെങ്കിൽ
	    തിരികെ &t_func_void;
    }
    എങ്കിൽ (ret_type == &t_any)
    {
	എങ്കിൽ (argcount == 0)
	    തിരികെ &t_func_0_any;
	അല്ലെങ്കിൽ
	    തിരികെ &t_func_any;
    }
    എങ്കിൽ (ret_type == &t_സംഖ്യ)
    {
	എങ്കിൽ (argcount == 0)
	    തിരികെ &t_func_0_സംഖ്യ;
	അല്ലെങ്കിൽ
	    തിരികെ &t_func_സംഖ്യ;
    }
    എങ്കിൽ (ret_type == &t_string)
    {
	എങ്കിൽ (argcount == 0)
	    തിരികെ &t_func_0_string;
	അല്ലെങ്കിൽ
	    തിരികെ &t_func_string;
    }

    തിരികെ alloc_func_type(ret_type, argcount, type_gap);
}

/*
 * For a function type, reserve space for "argcount" argument types (including
 * vararg).
 */
    പൂർണ്ണസംഖ്യ
func_type_add_arg_types(
	type_T	    *functype,
	പൂർണ്ണസംഖ്യ	    argcount,
	garray_T    *type_gap)
{
    // To make it easy to free the space needed for the argument types, add the
    // pointer to type_gap.
    എങ്കിൽ (ga_grow(type_gap, 1) == FAIL)
	തിരികെ FAIL;
    functype->tt_args = ALLOC_CLEAR_MULT(type_T *, argcount);
    എങ്കിൽ (functype->tt_args == ശൂന്യ)
	തിരികെ FAIL;
    ((type_T **)type_gap->ga_data)[type_gap->ga_len] =
						     (ശൂന്യം *)functype->tt_args;
    ++type_gap->ga_len;
    തിരികെ OK;
}

/*
 * Return TRUE if "type" is NULL, any or unknown.
 * This also works for const (comparing with &t_any and &t_unknown doesn't).
 */
    പൂർണ്ണസംഖ്യ
type_any_or_unknown(type_T *type)
{
    തിരികെ type == ശൂന്യ || type->tt_type == VAR_ANY
					|| type->tt_type == VAR_UNKNOWN;
}

/*
 * Get a type_T for a "special" typval in "tv".
 */
    സ്ഥിതം type_T *
special_typval2type(typval_T *tv)
{
    തിരഞ്ഞെടുപ്പ് (tv->vval.v_സംഖ്യ)
    {
	അവസ്ഥ VVAL_NULL:
	    തിരികെ &t_null;

	അവസ്ഥ VVAL_NONE:
	    തിരികെ &t_none;

	അവസ്ഥ VVAL_TRUE:
	അവസ്ഥ VVAL_FALSE:
	    തിരികെ &t_bool;

	സ്ഥിരസ്ഥിതി:
	    തിരികെ &t_unknown;
    }
}

/*
 * Get a type_T for a List typval in "tv".
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    സ്ഥിതം type_T *
list_typval2type(typval_T *tv, പൂർണ്ണസംഖ്യ copyID, garray_T *type_gap, പൂർണ്ണസംഖ്യ flags)
{
    list_T	*l = tv->vval.v_list;
    listitem_T	*li;
    type_T	*member_type = ശൂന്യ;

    // An empty list has type list<unknown>, unless the type was specified
    // and is not list<any>.  This matters when assigning to a variable
    // with a specific list type.
    എങ്കിൽ (l == ശൂന്യ || (l->lv_first == ശൂന്യ
		&& (l->lv_type == ശൂന്യ || l->lv_type->tt_member == &t_any)))
	തിരികെ &t_list_empty;

    എങ്കിൽ ((flags & TVTT_DO_MEMBER) == 0)
	തിരികെ &t_list_any;

    // If the type is list<any> go through the members, it may end up a
    // more specific type.
    എങ്കിൽ (l->lv_type != ശൂന്യ && (l->lv_first == ശൂന്യ
		|| (flags & TVTT_MORE_SPECIFIC) == 0
		|| l->lv_type->tt_member != &t_any))
	// make a copy, lv_type may be freed if the list is freed
	തിരികെ copy_type_deep(l->lv_type, type_gap);

    എങ്കിൽ (l->lv_first == &range_list_item)
	തിരികെ &t_list_സംഖ്യ;

    എങ്കിൽ (l->lv_copyID == copyID)
	// avoid recursion
	തിരികെ &t_list_any;

    l->lv_copyID = copyID;

    // Use the common type of all members.
    member_type = typval2type(&l->lv_first->li_tv, copyID, type_gap,
							TVTT_DO_MEMBER);
    വേണ്ടി (li = l->lv_first->li_next; li != ശൂന്യ; li = li->li_next)
	common_type(typval2type(&li->li_tv, copyID, type_gap, TVTT_DO_MEMBER),
					member_type, &member_type, type_gap);

    // Reset copyID so that a shared reference to this list (not a circular
    // reference) can be processed again to get the correct type.
    l->lv_copyID = 0;

    തിരികെ get_list_type(member_type, type_gap);
}

/*
 * Get a type_T for a Tuple typval in "tv".
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    സ്ഥിതം type_T *
tuple_typval2type(typval_T *tv, പൂർണ്ണസംഖ്യ copyID, garray_T *type_gap, പൂർണ്ണസംഖ്യ flags)
{
    tuple_T	*tuple = tv->vval.v_tuple;
    പൂർണ്ണസംഖ്യ		len = tuple_len(tuple);
    type_T	*type = ശൂന്യ;

    // An empty tuple has type tuple<unknown>, unless the type was specified
    // and is not tuple<any>.  This matters when assigning to a variable
    // with a specific tuple type.
    എങ്കിൽ (tuple == ശൂന്യ || (len == 0 && (tuple->tv_type == ശൂന്യ
				|| tuple->tv_type->tt_argcount == 0)))
	തിരികെ &t_tuple_empty;

    എങ്കിൽ ((flags & TVTT_DO_MEMBER) == 0)
	തിരികെ &t_tuple_any;

    // If the type is tuple<any> go through the members, it may end up a
    // more specific type.
    എങ്കിൽ (tuple->tv_type != ശൂന്യ && (len == 0
					|| (flags & TVTT_MORE_SPECIFIC) == 0))
	// make a copy, tv_type may be freed if the tuple is freed
	തിരികെ copy_type_deep(tuple->tv_type, type_gap);

    എങ്കിൽ (tuple->tv_copyID == copyID)
	// avoid recursion
	തിരികെ &t_tuple_any;

    tuple->tv_copyID = copyID;

    garray_T	tuple_types_ga;
    ga_init2(&tuple_types_ga, വലുപ്പം(type_T *), 10);
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < len; i++)
    {
	type = typval2type(TUPLE_ITEM(tuple, i), copyID, type_gap,
							TVTT_DO_MEMBER);
	എങ്കിൽ (ga_grow(&tuple_types_ga, 1) == FAIL)
	{
	    ga_clear(&tuple_types_ga);
	    // Reset copyID so that a shared reference to this tuple can be
	    // processed again.
	    tuple->tv_copyID = 0;
	    തിരികെ ശൂന്യ;
	}
	((type_T **)tuple_types_ga.ga_data)[tuple_types_ga.ga_len] = type;
	tuple_types_ga.ga_len++;
    }

    type_T *tuple_type = get_tuple_type(&tuple_types_ga, type_gap);
    ga_clear(&tuple_types_ga);

    // Reset copyID so that a shared reference to this tuple (not a circular
    // reference) can be processed again to get the correct type.
    tuple->tv_copyID = 0;

    തിരികെ tuple_type;
}

/*
 * Get a type_T for a Dict typval in "tv".
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    സ്ഥിതം type_T *
dict_typval2type(typval_T *tv, പൂർണ്ണസംഖ്യ copyID, garray_T *type_gap, പൂർണ്ണസംഖ്യ flags)
{
    dict_iterator_T	iter;
    typval_T		*മൂല്യം;
    dict_T		*d = tv->vval.v_dict;
    type_T		*member_type = ശൂന്യ;

    എങ്കിൽ (d == ശൂന്യ || (d->dv_hashtab.ht_used == 0 && d->dv_type == ശൂന്യ))
	തിരികെ &t_dict_empty;

    എങ്കിൽ ((flags & TVTT_DO_MEMBER) == 0)
	തിരികെ &t_dict_any;

    // If the type is dict<any> go through the members, it may end up a
    // more specific type.
    എങ്കിൽ (d->dv_type != ശൂന്യ && (d->dv_hashtab.ht_used == 0
		|| (flags & TVTT_MORE_SPECIFIC) == 0
		|| d->dv_type->tt_member != &t_any))
	തിരികെ d->dv_type;

    എങ്കിൽ (d->dv_copyID == copyID)
	// avoid recursion
	തിരികെ &t_dict_any;

    d->dv_copyID = copyID;

    // Use the common type of all values.
    dict_iterate_start(tv, &iter);
    dict_iterate_next(&iter, &മൂല്യം);
    member_type = typval2type(മൂല്യം, copyID, type_gap, TVTT_DO_MEMBER);

    ആകുമ്പോൾ (dict_iterate_next(&iter, &മൂല്യം) != ശൂന്യ)
	common_type(typval2type(മൂല്യം, copyID, type_gap, TVTT_DO_MEMBER),
					member_type, &member_type, type_gap);

    // Reset copyID so that a shared reference to this dict (not a circular
    // reference) can be processed again to get the correct type.
    d->dv_copyID = 0;

    തിരികെ get_dict_type(member_type, type_gap);
}

/*
 * Get a type_T for a "partial" typval in "tv".
 */
    സ്ഥിതം type_T *
partial_typval2type(typval_T *tv, ufunc_T *ufunc, garray_T *type_gap)
{
    partial_T *pt = tv->vval.v_partial;
    type_T  *type;

    type = get_type_ptr(type_gap);
    എങ്കിൽ (type == ശൂന്യ)
	തിരികെ ശൂന്യ;

    *type = *ufunc->uf_func_type;
    എങ്കിൽ (type->tt_argcount >= 0 && pt->pt_argc > 0)
    {
	type->tt_argcount -= pt->pt_argc;
	type->tt_min_argcount -= pt->pt_argc;
	എങ്കിൽ (type->tt_argcount > 0 && func_type_add_arg_types(type,
		    type->tt_argcount, type_gap) == OK)
	    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < type->tt_argcount; ++i)
		type->tt_args[i] =
		    ufunc->uf_func_type->tt_args[i + pt->pt_argc];
    }
    എങ്കിൽ (pt->pt_func != ശൂന്യ)
	type->tt_member = pt->pt_func->uf_ret_type;

    തിരികെ type;
}

/*
 * Get a type_T for a "class" or an "object" typval in "tv".
 */
    സ്ഥിതം type_T *
oc_typval2type(typval_T *tv)
{
    എങ്കിൽ (tv->v_type == VAR_CLASS)
    {
	എങ്കിൽ (tv->vval.v_class == ശൂന്യ)
	    തിരികെ &t_class;

	തിരികെ &tv->vval.v_class->class_type;
    }

    എങ്കിൽ (tv->vval.v_object != ശൂന്യ)
	തിരികെ &tv->vval.v_object->obj_class->class_object_type;

    തിരികെ &t_object_any;
}

/*
 * Get a type_T for a "function" or a "partial"
 */
    സ്ഥിതം type_T *
fp_typval2type(typval_T *tv, garray_T *type_gap)
{
    char_u	*name = ശൂന്യ;
    ufunc_T	*ufunc = ശൂന്യ;
    type_T	*type;
    type_T	*member_type = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		argcount = 0;
    പൂർണ്ണസംഖ്യ		min_argcount = 0;

    എങ്കിൽ (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != ശൂന്യ)
    {
	എങ്കിൽ (tv->vval.v_partial->pt_func != ശൂന്യ)
	    ufunc = tv->vval.v_partial->pt_func;
	അല്ലെങ്കിൽ
	    name = tv->vval.v_partial->pt_name;
    }
    അല്ലെങ്കിൽ
	name = tv->vval.v_string;

    എങ്കിൽ (name == ശൂന്യ && ufunc == ശൂന്യ)
	തിരികെ &t_func_unknown;

    എങ്കിൽ (name != ശൂന്യ)
    {
	പൂർണ്ണസംഖ്യ idx = find_internal_func(name);

	എങ്കിൽ (idx >= 0)
	{
	    type_T *decl_type;  // unused

	    internal_func_get_argcount(idx, &argcount, &min_argcount);
	    member_type = internal_func_ret_type(idx, 0, ശൂന്യ, &decl_type,
		    type_gap);
	}
	അല്ലെങ്കിൽ
	{
	    // Check if name contains "<".  If it does, then replace "<" with
	    // NUL and lookup the function and then look up the specific
	    // generic function using the supplied types.
	    char_u *p = generic_func_find_open_bracket(name);
	    എങ്കിൽ (p == ശൂന്യ)
		ufunc = find_func(name, FALSE);
	    അല്ലെങ്കിൽ
	    {
		// generic function
		char_u	cc = *p;

		*p = NUL;
		ufunc = find_func(name, FALSE);
		*p = cc;
		എങ്കിൽ (ufunc != ശൂന്യ && IS_GENERIC_FUNC(ufunc))
		{
		    ufunc = find_generic_func(ufunc, name, &p);
		    എങ്കിൽ (ufunc == ശൂന്യ)
			തിരികെ ശൂന്യ;
		}
	    }
	}
    }
    എങ്കിൽ (ufunc != ശൂന്യ)
    {
	// May need to get the argument types from default values by
	// compiling the function.
	എങ്കിൽ (ufunc->uf_def_status == UF_TO_BE_COMPILED
		&& compile_def_function(ufunc, TRUE, CT_NONE, ശൂന്യ)
		== FAIL)
	    തിരികെ ശൂന്യ;
	എങ്കിൽ (ufunc->uf_func_type == ശൂന്യ)
	    set_function_type(ufunc);
	എങ്കിൽ (ufunc->uf_func_type != ശൂന്യ)
	{
	    എങ്കിൽ (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != ശൂന്യ)
		തിരികെ partial_typval2type(tv, ufunc, type_gap);
	    തിരികെ ufunc->uf_func_type;
	}
    }

    type = get_type_ptr(type_gap);
    എങ്കിൽ (type == ശൂന്യ)
	തിരികെ ശൂന്യ;
    type->tt_type = tv->v_type;
    type->tt_argcount = argcount;
    type->tt_min_argcount = min_argcount;
    എങ്കിൽ (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != ശൂന്യ
					&& tv->vval.v_partial->pt_argc > 0)
    {
	type->tt_argcount -= tv->vval.v_partial->pt_argc;
	type->tt_min_argcount -= tv->vval.v_partial->pt_argc;
    }
    type->tt_member = member_type;

    തിരികെ type;
}

/*
 * Get a type_T for a typval_T.
 * "type_gap" is used to temporarily create types in.
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    സ്ഥിതം type_T *
typval2type_int(typval_T *tv, പൂർണ്ണസംഖ്യ copyID, garray_T *type_gap, പൂർണ്ണസംഖ്യ flags)
{
    തിരഞ്ഞെടുപ്പ് (tv->v_type)
    {
	അവസ്ഥ VAR_UNKNOWN:
	    തിരികെ &t_unknown;

	അവസ്ഥ VAR_ANY:
	    തിരികെ &t_any;

	അവസ്ഥ VAR_VOID:
	    തിരികെ &t_void;

	അവസ്ഥ VAR_BOOL:
	    തിരികെ &t_bool;

	അവസ്ഥ VAR_SPECIAL:
	    തിരികെ special_typval2type(tv);

	അവസ്ഥ VAR_സംഖ്യ:
	    തിരികെ &t_സംഖ്യ;

	അവസ്ഥ VAR_FLOAT:
	    തിരികെ &t_float;

	അവസ്ഥ VAR_STRING:
	    തിരികെ &t_string;

	അവസ്ഥ VAR_BLOB:
	    എങ്കിൽ (tv->vval.v_blob == ശൂന്യ)
		തിരികെ &t_blob_null;
	    തിരികെ &t_blob;

	അവസ്ഥ VAR_LIST:
	    തിരികെ list_typval2type(tv, copyID, type_gap, flags);

	അവസ്ഥ VAR_TUPLE:
	    തിരികെ tuple_typval2type(tv, copyID, type_gap, flags);

	അവസ്ഥ VAR_DICT:
	    തിരികെ dict_typval2type(tv, copyID, type_gap, flags);

	അവസ്ഥ VAR_JOB:
	    തിരികെ &t_job;

	അവസ്ഥ VAR_CHANNEL:
	    തിരികെ &t_channel;

	അവസ്ഥ VAR_CLASS:
	അവസ്ഥ VAR_OBJECT:
	    തിരികെ oc_typval2type(tv);

	അവസ്ഥ VAR_TYPEALIAS:
	    തിരികെ &t_typealias;

	അവസ്ഥ VAR_FUNC:
	അവസ്ഥ VAR_PARTIAL:
	    തിരികെ fp_typval2type(tv, type_gap);

	അവസ്ഥ VAR_INSTR:
	സ്ഥിരസ്ഥിതി:
	    നിർത്തുക;
    }

    തിരികെ ശൂന്യ;
}

/*
 * Return TRUE if "tv" is not a bool but should be converted to bool.
 */
    പൂർണ്ണസംഖ്യ
need_convert_to_bool(type_T *type, typval_T *tv)
{
    തിരികെ type != ശൂന്യ && type == &t_bool && tv->v_type != VAR_BOOL
	    && (tv->v_type == VAR_സംഖ്യ
		       && (tv->vval.v_സംഖ്യ == 0 || tv->vval.v_സംഖ്യ == 1));
}

/*
 * Get a type_T for a typval_T.
 * "type_list" is used to temporarily create types in.
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the most specific member type.
 */
    type_T *
typval2type(typval_T *tv, പൂർണ്ണസംഖ്യ copyID, garray_T *type_gap, പൂർണ്ണസംഖ്യ flags)
{
    type_T *type = typval2type_int(tv, copyID, type_gap, flags);

    എങ്കിൽ (type == ശൂന്യ)
	തിരികെ ശൂന്യ;

    എങ്കിൽ (type != &t_bool && (tv->v_type == VAR_സംഖ്യ
		&& (tv->vval.v_സംഖ്യ == 0 || tv->vval.v_സംഖ്യ == 1)))
	// Number 0 and 1 and expression with "&&" or "||" can also be used
	// for bool.
	type = &t_സംഖ്യ_bool;
    അല്ലെങ്കിൽ എങ്കിൽ (type != &t_float && tv->v_type == VAR_സംഖ്യ)
	// A number can also be used for float.
	type = &t_സംഖ്യ_float;
    തിരികെ type;
}

/*
 * Return TRUE if "type" can be used for a variable declaration.
 * Give an error and return FALSE if not.
 */
    പൂർണ്ണസംഖ്യ
valid_declaration_type(type_T *type)
{
    എങ്കിൽ (type->tt_type == VAR_SPECIAL  // null, none
	    || type->tt_type == VAR_VOID)
    {
	അക്ഷരം *tofree = ശൂന്യ;
	അക്ഷരം *name = type_name(type, &tofree);
	semsg(_(e_invalid_type_in_variable_declaration_str), name);
	vim_free(tofree);
	തിരികെ FALSE;
    }
    തിരികെ TRUE;
}

/*
 * Get a type_T for a typval_T, used for v: variables.
 * "type_list" is used to temporarily create types in.
 */
    type_T *
typval2type_vimvar(typval_T *tv, garray_T *type_gap)
{
    എങ്കിൽ (tv->v_type == VAR_LIST)  // e.g. for v:oldfiles
	തിരികെ &t_list_string;
    എങ്കിൽ (tv->v_type == VAR_DICT)  // e.g. for v:event
	തിരികെ &t_dict_any;
    തിരികെ typval2type(tv, get_copyID(), type_gap, TVTT_DO_MEMBER);
}

    പൂർണ്ണസംഖ്യ
check_typval_arg_type(
	type_T	    *expected,
	typval_T    *actual_tv,
	അക്ഷരം	    *func_name,
	പൂർണ്ണസംഖ്യ	    arg_idx)
{
    where_T	where = WHERE_INIT;

    എങ്കിൽ (arg_idx > 0)
    {
	where.wt_index = arg_idx;
	where.wt_kind = WT_ARGUMENT;
    }
    where.wt_func_name = func_name;
    തിരികെ check_typval_type(expected, actual_tv, where);
}

/*
 * Return true if "expected" is list<any> or dict<any> and "actual_tv" is a
 * list or dict of that kind, which matches whatever its values are.
 */
    സ്ഥിതം bool
matches_listdict_any(type_T *expected, typval_T *actual_tv)
{
    തിരികെ (expected->tt_type == VAR_LIST || expected->tt_type == VAR_DICT)
	    && actual_tv->v_type == expected->tt_type
	    && expected->tt_member != ശൂന്യ
	    && (expected->tt_member->tt_type == VAR_ANY
		|| expected->tt_member->tt_type == VAR_UNKNOWN);
}

/*
 * Return FAIL if "expected" and "actual" don't match.
 * When "argidx" > 0 it is included in the error message.
 */
    പൂർണ്ണസംഖ്യ
check_typval_type(type_T *expected, typval_T *actual_tv, where_T where)
{
    garray_T	type_list;
    type_T	*actual_type;
    പൂർണ്ണസംഖ്യ		res = FAIL;

    എങ്കിൽ (expected == ശൂന്യ)
	തിരികെ OK;  // didn't expect anything.

    // No need to go through the values of a possibly big list or dict.
    എങ്കിൽ (matches_listdict_any(expected, actual_tv))
	തിരികെ OK;

    ga_init2(&type_list, വലുപ്പം(type_T *), 10);

    // A null_function and null_partial are special cases, they can be used to
    // clear a variable.
    എങ്കിൽ ((actual_tv->v_type == VAR_FUNC && actual_tv->vval.v_string == ശൂന്യ)
	    || (actual_tv->v_type == VAR_PARTIAL
					 && actual_tv->vval.v_partial == ശൂന്യ))
	actual_type = &t_func_unknown;
    അല്ലെങ്കിൽ
	// When the actual type is list<any> or dict<any> go through the values
	// to possibly get a more specific type.
	actual_type = typval2type(actual_tv, get_copyID(), &type_list,
					  TVTT_DO_MEMBER | TVTT_MORE_SPECIFIC);
    എങ്കിൽ (actual_type != ശൂന്യ)
    {
	res = check_type_maybe(expected, actual_type, TRUE, where);
	എങ്കിൽ (res == MAYBE && !(actual_type->tt_type == VAR_FUNC
				      && (actual_type->tt_member == &t_unknown
					  || actual_type->tt_member == ശൂന്യ)))
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
    തിരികെ res;
}

    ശൂന്യം
arg_type_mismatch(type_T *expected, type_T *actual, പൂർണ്ണസംഖ്യ arg_idx)
{
    where_T	where = WHERE_INIT;

    എങ്കിൽ (arg_idx > 0)
    {
	where.wt_index = arg_idx;
	where.wt_kind = WT_ARGUMENT;
    }
    type_mismatch_where(expected, actual, where);
}

    ശൂന്യം
type_mismatch_where(type_T *expected, type_T *actual, where_T where)
{
    അക്ഷരം *tofree1, *tofree2;
    അക്ഷരം *typename1 = type_name(expected, &tofree1);
    അക്ഷരം *typename2 = type_name(actual, &tofree2);

    തിരഞ്ഞെടുപ്പ് (where.wt_kind)
    {
	അവസ്ഥ WT_MEMBER:
	    semsg(_(e_variable_str_type_mismatch_expected_str_but_got_str),
		    where.wt_func_name, typename1, typename2);
	    നിർത്തുക;
	അവസ്ഥ WT_METHOD:
	അവസ്ഥ WT_METHOD_ARG:
	അവസ്ഥ WT_METHOD_RETURN:
	    semsg(_(e_method_str_type_mismatch_expected_str_but_got_str),
		    where.wt_func_name, typename1, typename2);
	    നിർത്തുക;
	അവസ്ഥ WT_VARIABLE:
	    എങ്കിൽ (where.wt_func_name == ശൂന്യ)
		semsg(_(e_variable_nr_type_mismatch_expected_str_but_got_str),
			where.wt_index, typename1, typename2);
	    അല്ലെങ്കിൽ
		semsg(_(e_variable_nr_type_mismatch_expected_str_but_got_str_in_str),
			where.wt_index, typename1, typename2, where.wt_func_name);
	    നിർത്തുക;
	അവസ്ഥ WT_ARGUMENT:
	    എങ്കിൽ (where.wt_func_name == ശൂന്യ)
		semsg(_(e_argument_nr_type_mismatch_expected_str_but_got_str),
			where.wt_index, typename1, typename2);
	    അല്ലെങ്കിൽ
		semsg(_(e_argument_nr_type_mismatch_expected_str_but_got_str_in_str),
			where.wt_index, typename1, typename2, where.wt_func_name);
	    നിർത്തുക;
	അവസ്ഥ WT_CAST:
	അവസ്ഥ WT_UNKNOWN:
	    എങ്കിൽ (where.wt_func_name == ശൂന്യ)
		semsg(_(e_type_mismatch_expected_str_but_got_str),
			typename1, typename2);
	    അല്ലെങ്കിൽ
		semsg(_(e_type_mismatch_expected_str_but_got_str_in_str),
			typename1, typename2, where.wt_func_name);
	    നിർത്തുക;
    }

    vim_free(tofree1);
    vim_free(tofree2);
}

/*
 * Check if the expected and actual types match for a tuple
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
check_tuple_type_maybe(
    type_T	*expected,
    type_T	*actual,
    where_T	where)
{
    എങ്കിൽ (expected->tt_argcount == -1 || actual->tt_argcount == -1
		|| expected->tt_args == ശൂന്യ || actual->tt_args == ശൂന്യ)
	തിരികെ OK;

    // For a non-variadic tuple, the number of items must match
    എങ്കിൽ (!(expected->tt_flags & TTFLAG_VARARGS)
	    && expected->tt_argcount != actual->tt_argcount)
	തിരികെ FAIL;

    // compare the type of each tuple item
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < actual->tt_argcount; ++i)
    {
	type_T	*exp_type;
	type_T	*actual_type;

	എങ്കിൽ (expected->tt_flags & TTFLAG_VARARGS)
	{
	    എങ്കിൽ (i < expected->tt_argcount - 1)
		exp_type = expected->tt_args[i];
	    അല്ലെങ്കിൽ
		// For a variadic tuple, the last type is a List.  So use the
		// List member type.
		exp_type = expected->tt_args[expected->tt_argcount - 1]->tt_member;
	}
	അല്ലെങ്കിൽ
	    exp_type = expected->tt_args[i];

	എങ്കിൽ (actual->tt_flags & TTFLAG_VARARGS)
	{
	    എങ്കിൽ (i < actual->tt_argcount - 1)
		actual_type = actual->tt_args[i];
	    അല്ലെങ്കിൽ
		// For a variadic tuple, the last type is a List.  So use the
		// List member type.
		actual_type = actual->tt_args[actual->tt_argcount - 1]->tt_member;
	}
	അല്ലെങ്കിൽ
	    actual_type = actual->tt_args[i];

	// Allow for using "any" type for a tuple item
	എങ്കിൽ (actual->tt_args[i] != &t_any && check_type(exp_type, actual_type,
						FALSE, where) == FAIL)
	    തിരികെ FAIL;
    }

    തിരികെ OK;
}

/*
 * Check if the expected and actual types match for a function
 * Returns OK if "expected" and "actual" are matching function types.
 * Returns FAIL if "expected" and "actual" are different types.
 * Returns MAYBE when a runtime type check is needed.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
check_func_type_maybe(
    type_T	*expected,
    type_T	*actual,
    where_T	where)
{
    പൂർണ്ണസംഖ്യ ret = OK;

    // If the return type is unknown it can be anything, including
    // nothing, thus there is no point in checking.
    എങ്കിൽ (expected->tt_member != &t_unknown)
    {
	എങ്കിൽ (actual->tt_member != ശൂന്യ
		&& actual->tt_member != &t_unknown)
	{
	    where_T  func_where = where;

	    func_where.wt_kind = WT_METHOD_RETURN;
	    ret = check_type_maybe(expected->tt_member,
		    actual->tt_member, FALSE,
		    func_where);
	}
	അല്ലെങ്കിൽ
	    ret = MAYBE;
    }
    എങ്കിൽ (ret != FAIL
	    && ((expected->tt_flags & TTFLAG_VARARGS)
		!= (actual->tt_flags & TTFLAG_VARARGS))
	    && expected->tt_argcount != -1)
	ret = FAIL;
    എങ്കിൽ (ret != FAIL && expected->tt_argcount != -1
	    && actual->tt_min_argcount != -1
	    && (actual->tt_argcount == -1
		|| (actual->tt_argcount < expected->tt_min_argcount
		    || actual->tt_argcount > expected->tt_argcount)))
	ret = FAIL;
    എങ്കിൽ (ret != FAIL && expected->tt_args != ശൂന്യ
	    && actual->tt_args != ശൂന്യ)
    {
	പൂർണ്ണസംഖ്യ i;

	വേണ്ടി (i = 0; i < expected->tt_argcount
		&& i < actual->tt_argcount; ++i)
	{
	    where_T  func_where = where;
	    func_where.wt_kind = WT_METHOD_ARG;

	    // Allow for using "any" argument type, lambda's have them.
	    എങ്കിൽ (actual->tt_args[i] != &t_any && check_type(
			expected->tt_args[i], actual->tt_args[i], FALSE,
			func_where) == FAIL)
	    {
		ret = FAIL;
		നിർത്തുക;
	    }
	}
    }
    എങ്കിൽ (ret == OK && expected->tt_argcount >= 0
	    && actual->tt_argcount == -1)
	// check the argument count at runtime
	ret = MAYBE;

    തിരികെ ret;
}

/*
 * Check if the expected and actual types match for an object
 * Returns OK if "expected" and "actual" are matching object types.
 * Returns FAIL if "expected" and "actual" are different types.
 * Returns MAYBE when a runtime type check is needed.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
check_object_type_maybe(
    type_T	*expected,
    type_T	*actual,
    where_T	where)
{
    പൂർണ്ണസംഖ്യ ret = OK;

    എങ്കിൽ (actual->tt_type == VAR_ANY)
	തിരികെ MAYBE;	// use runtime type check
    എങ്കിൽ (actual->tt_type != VAR_OBJECT)
	തിരികെ FAIL;	// don't use tt_class
    എങ്കിൽ (actual->tt_class == ശൂന്യ)    // null object
	തിരികെ OK;
    // t_object_any matches any object except for an enum item
    എങ്കിൽ (expected == &t_object_any && !IS_ENUM(actual->tt_class))
	തിരികെ OK;

    // For object method arguments, do a invariant type check in
    // an extended class.  For all others, do a covariance type check.
    എങ്കിൽ (where.wt_kind == WT_METHOD_ARG)
    {
	എങ്കിൽ (actual->tt_class != expected->tt_class)
	    ret = FAIL;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (!class_instance_of(actual->tt_class, expected->tt_class))
    {
	// Check if this is an up-cast, if so we'll have to check the type at
	// runtime.
	എങ്കിൽ (where.wt_kind == WT_CAST &&
		class_instance_of(expected->tt_class, actual->tt_class))
	    ret = MAYBE;
	അല്ലെങ്കിൽ
	    ret = FAIL;
    }

    തിരികെ ret;
}

/*
 * Check if the expected and actual types match.
 * Does not allow for assigning "any" to a specific type.
 * When "argidx" > 0 it is included in the error message.
 * Return OK if types match.
 * Return FAIL if types do not match.
 */
    പൂർണ്ണസംഖ്യ
check_type(
	type_T	*expected,
	type_T	*actual,
	പൂർണ്ണസംഖ്യ	give_സന്ദേശം,
	where_T where)
{
    പൂർണ്ണസംഖ്യ ret = check_type_maybe(expected, actual, give_സന്ദേശം, where);

    തിരികെ ret == MAYBE ? OK : ret;
}

/*
 * As check_type() but return MAYBE when a runtime type check should be used
 * when compiling.
 */
    പൂർണ്ണസംഖ്യ
check_type_maybe(
	type_T	*expected,
	type_T	*actual,
	പൂർണ്ണസംഖ്യ	give_സന്ദേശം,
	where_T where)
{
    പൂർണ്ണസംഖ്യ ret = OK;

    // When expected is "unknown" we accept any actual type.
    // When expected is "any" we accept any actual type except "void".
    എങ്കിൽ (expected->tt_type != VAR_UNKNOWN
	    && !(expected->tt_type == VAR_ANY && actual->tt_type != VAR_VOID))

    {
	// tt_type should match, except that a "partial" can be assigned to a
	// variable with type "func".
	// And "unknown" (using global variable) and "any" need a runtime type
	// check.
	എങ്കിൽ (!(expected->tt_type == actual->tt_type
		    || actual->tt_type == VAR_UNKNOWN
		    || actual->tt_type == VAR_ANY
		    || (expected->tt_type == VAR_FUNC
					   && actual->tt_type == VAR_PARTIAL)))
	{
	    എങ്കിൽ (expected->tt_type == VAR_BOOL
					&& (actual->tt_flags & TTFLAG_BOOL_OK))
		// Using number 0 or 1 for bool is OK.
		തിരികെ OK;
	    എങ്കിൽ (expected->tt_type == VAR_FLOAT
		    && actual->tt_type == VAR_സംഖ്യ
		    && ((expected->tt_flags & TTFLAG_സംഖ്യ_OK)
			     || (actual->tt_flags & TTFLAG_FLOAT_OK)))
		// Using a number where a float is expected is OK here.
		തിരികെ OK;
	    എങ്കിൽ (expected->tt_type == VAR_LIST
		    && actual->tt_type == VAR_TUPLE
		    && (expected->tt_flags & TTFLAG_TUPLE_OK))
		// Using a tuple where a list is expected is OK here.
		തിരികെ OK;
	    എങ്കിൽ (give_സന്ദേശം)
		type_mismatch_where(expected, actual, where);
	    തിരികെ FAIL;
	}
	എങ്കിൽ (expected->tt_type == VAR_DICT || expected->tt_type == VAR_LIST)
	{
	    // "unknown" is used for an empty list or dict
	    എങ്കിൽ (actual->tt_member != ശൂന്യ && actual->tt_member != &t_unknown)
		ret = check_type_maybe(expected->tt_member, actual->tt_member,
								 FALSE, where);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (expected->tt_type == VAR_TUPLE && actual != &t_any)
	    ret =  check_tuple_type_maybe(expected, actual, where);
	അല്ലെങ്കിൽ എങ്കിൽ (expected->tt_type == VAR_FUNC && actual != &t_any)
	    ret = check_func_type_maybe(expected, actual, where);
	അല്ലെങ്കിൽ എങ്കിൽ (expected->tt_type == VAR_OBJECT)
	    ret = check_object_type_maybe(expected, actual, where);

	എങ്കിൽ (ret == FAIL && give_സന്ദേശം)
	    type_mismatch_where(expected, actual, where);
    }

    എങ്കിൽ (ret == OK && expected->tt_type != VAR_UNKNOWN
	    && expected->tt_type != VAR_ANY
	    && (actual->tt_type == VAR_UNKNOWN || actual->tt_type == VAR_ANY))
	// check the type at runtime
	ret = MAYBE;

    തിരികെ ret;
}

/*
 * Check that the arguments of "type" match "argvars[argcount]".
 * "base_tv" is from "expr->Func()".
 * Return OK/FAIL.
 */
    പൂർണ്ണസംഖ്യ
check_argument_types(
	type_T	    *type,
	typval_T    *argvars,
	പൂർണ്ണസംഖ്യ	    argcount,
	typval_T    *base_tv,
	char_u	    *name)
{
    പൂർണ്ണസംഖ്യ	    varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;
    പൂർണ്ണസംഖ്യ	    i;
    പൂർണ്ണസംഖ്യ	    totcount = argcount + (base_tv == ശൂന്യ ? 0 : 1);

    എങ്കിൽ (type->tt_type != VAR_FUNC && type->tt_type != VAR_PARTIAL)
	തിരികെ OK;  // just in case
    എങ്കിൽ (totcount < type->tt_min_argcount - varargs)
    {
	emsg_funcname(e_not_enough_arguments_for_function_str, name);
	തിരികെ FAIL;
    }
    എങ്കിൽ (!varargs && type->tt_argcount >= 0 && totcount > type->tt_argcount)
    {
	emsg_funcname(e_too_many_arguments_for_function_str, name);
	തിരികെ FAIL;
    }
    എങ്കിൽ (type->tt_args == ശൂന്യ)
	തിരികെ OK;  // cannot check


    വേണ്ടി (i = 0; i < totcount; ++i)
    {
	type_T	    *expected;
	typval_T    *tv;

	എങ്കിൽ (base_tv != ശൂന്യ)
	{
	    എങ്കിൽ (i == 0)
		tv = base_tv;
	    അല്ലെങ്കിൽ
		tv = &argvars[i - 1];
	}
	അല്ലെങ്കിൽ
	    tv = &argvars[i];
	എങ്കിൽ (varargs && i >= type->tt_argcount - 1)
	{
	    expected = type->tt_args[type->tt_argcount - 1];
	    എങ്കിൽ (expected != ശൂന്യ && expected->tt_type == VAR_LIST)
		expected = expected->tt_member;
	    എങ്കിൽ (expected == ശൂന്യ)
		expected = &t_any;
	}
	അല്ലെങ്കിൽ
	    expected = type->tt_args[i];

	// check the type, unless the value is v:none
	എങ്കിൽ ((tv->v_type != VAR_SPECIAL || tv->vval.v_സംഖ്യ != VVAL_NONE)
		   && check_typval_arg_type(expected, tv, ശൂന്യ, i + 1) == FAIL)
	    തിരികെ FAIL;
    }
    തിരികെ OK;
}

// Maximum nesting of types, to avoid running out of stack space.  E.g.
// "list<list<number>>" is nested twice.
#define MAX_TYPE_DEPTH	1000

// Current number of levels of parse_type() and the deepest level reached while
// parsing the current type.  The nesting is one less than the number of
// levels.
സ്ഥിതം പൂർണ്ണസംഖ്യ	parse_depth = 0;
സ്ഥിതം പൂർണ്ണസംഖ്യ	parse_max_depth = 0;

/*
 * Skip over type in list<type>, dict<type> or tuple<type>.
 * Returns a pointer to the character after the type.  "syn_error" is set to
 * TRUE on syntax error.
 */
    സ്ഥിതം char_u *
skip_member_type(char_u *start, char_u *p, പൂർണ്ണസംഖ്യ *syn_error)
{
    എങ്കിൽ (STRNCMP("tuple", start, 5) == 0)
    {
	// handle tuple<{type1}, {type2}, ....<type>>
	p = skipwhite(p + 1);
	ആകുമ്പോൾ (*p != '>' && *p != NUL)
	{
	    char_u *sp = p;

	    എങ്കിൽ (STRNCMP(p, "...", 3) == 0)
		p += 3;
	    p = skip_type(p, TRUE);
	    എങ്കിൽ (p == sp)
	    {
		*syn_error = TRUE;
		തിരികെ p;  // syntax error
	    }
	    എങ്കിൽ (*p == ',')
		p = skipwhite(p + 1);
	}
	എങ്കിൽ (*p == '>')
	    p++;
    }
    അല്ലെങ്കിൽ
    {
	p = skipwhite(p);
	p = skip_type(skipwhite(p + 1), FALSE);
	p = skipwhite(p);
	എങ്കിൽ (*p == '>')
	    ++p;
    }

    തിരികെ p;
}

/*
 * Skip over a function type.  Returns a pointer to the character after the
 * type.  "syn_error" is set to TRUE on syntax error.
 */
    സ്ഥിതം char_u *
skip_func_type(char_u *p, പൂർണ്ണസംഖ്യ *syn_error)
{
    എങ്കിൽ (*p == '(')
    {
	// handle func(args): type
	++p;
	ആകുമ്പോൾ (*p != ')' && *p != NUL)
	{
	    char_u *sp = p;

	    എങ്കിൽ (STRNCMP(p, "...", 3) == 0)
		p += 3;
	    p = skip_type(p, TRUE);
	    എങ്കിൽ (p == sp)
	    {
		*syn_error = TRUE;
		തിരികെ p;  // syntax error
	    }
	    എങ്കിൽ (*p == ',')
		p = skipwhite(p + 1);
	}
	എങ്കിൽ (*p == ')')
	{
	    എങ്കിൽ (p[1] == ':')
		p = skip_type(skipwhite(p + 2), FALSE);
	    അല്ലെങ്കിൽ
		++p;
	}
    }
    അല്ലെങ്കിൽ
    {
	// handle func: return_type
	p = skip_type(skipwhite(p + 1), FALSE);
    }

    തിരികെ p;
}

/*
 * Inner part of skip_type().
 */
    സ്ഥിതം char_u *
skip_type_inner(char_u *start, പൂർണ്ണസംഖ്യ optional)
{
    char_u	*p = start;
    പൂർണ്ണസംഖ്യ		syn_error = FALSE;

    എങ്കിൽ (optional && *p == '?')
	++p;

    // Also skip over "." for imported classes: "import.ClassName".
    ആകുമ്പോൾ (ASCII_ISALNUM(*p) || *p == '_' || *p == '.')
	++p;

    // Skip over "<type>"; this is permissive about white space.
    എങ്കിൽ (*skipwhite(p) == '<')
    {
	p = skip_member_type(start, p, &syn_error);
	എങ്കിൽ (syn_error)
	    തിരികെ p;
    }
    അല്ലെങ്കിൽ എങ്കിൽ ((*p == '(' || (*p == ':' && VIM_ISWHITE(p[1])))
					     && STRNCMP("func", start, 4) == 0)
    {
	// skip over function type
	p = skip_func_type(p, &syn_error);
	എങ്കിൽ (syn_error)
	    തിരികെ p;
    }

    തിരികെ p;
}

/*
 * Skip over a type definition and return a pointer to just after it.
 * When "optional" is TRUE then a leading "?" is accepted.
 */
    char_u *
skip_type(char_u *start, പൂർണ്ണസംഖ്യ optional)
{
    സ്ഥിതം പൂർണ്ണസംഖ്യ	skip_depth = 0;
    char_u	*p = start;

    // Too deeply nested: skip to the matching ">" without recursing, an
    // error is given when parsing the type.
    എങ്കിൽ (skip_depth > MAX_TYPE_DEPTH)
    {
	പൂർണ്ണസംഖ്യ	depth = 0;

	വേണ്ടി ( ; *p != NUL; ++p)
	{
	    എങ്കിൽ (*p == '<')
		++depth;
	    അല്ലെങ്കിൽ എങ്കിൽ (*p == '>')
	    {
		എങ്കിൽ (depth == 0)
		    നിർത്തുക;
		എങ്കിൽ (--depth == 0)
		    തിരികെ p + 1;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (depth == 0 && !ASCII_ISALNUM(*p) && *p != '_'
								&& *p != '.')
		നിർത്തുക;
	}
	തിരികെ p;
    }

    ++skip_depth;
    p = skip_type_inner(start, optional);
    --skip_depth;
    തിരികെ p;
}

/*
 * Parse the member type: "<type>" and return "type" with the member set.
 * Use "type_gap" if a new type needs to be added.
 * "info" is extra information for an error message.
 * Returns NULL in case of failure.
 */
    സ്ഥിതം type_T *
parse_type_member(
	char_u	    **arg,
	type_T	    *type,
	garray_T    *type_gap,
	പൂർണ്ണസംഖ്യ	    give_error,
	അക്ഷരം	    *info,
	ufunc_T	    *ufunc,
	cctx_T	    *cctx)
{
    char_u  *arg_start = *arg;
    type_T  *member_type;
    പൂർണ്ണസംഖ്യ	    prev_called_emsg = called_emsg;

    എങ്കിൽ (**arg != '<')
    {
	എങ്കിൽ (give_error)
	{
	    എങ്കിൽ (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    അല്ലെങ്കിൽ
		semsg(_(e_missing_type_after_str), info);
	}
	തിരികെ ശൂന്യ;
    }
    *arg = skipwhite(*arg + 1);

    member_type = parse_type(arg, type_gap, ufunc, cctx, give_error);
    എങ്കിൽ (member_type == ശൂന്യ || !valid_declaration_type(member_type))
	തിരികെ ശൂന്യ;

    *arg = skipwhite(*arg);
    എങ്കിൽ (**arg != '>' && called_emsg == prev_called_emsg)
    {
	എങ്കിൽ (give_error)
	    semsg(_(e_missing_gt_after_type_str), arg_start);
	തിരികെ ശൂന്യ;
    }
    ++*arg;

    എങ്കിൽ (type->tt_type == VAR_LIST)
	തിരികെ get_list_type(member_type, type_gap);
    തിരികെ get_dict_type(member_type, type_gap);
}

/*
 * Parse a "func" type at "*arg" and advance over it.
 * When "give_error" is TRUE give error messages, otherwise be quiet.
 * Return NULL for failure.
 */
    സ്ഥിതം type_T *
parse_type_func(
    char_u	**arg,
    size_t	len,
    garray_T	*type_gap,
    പൂർണ്ണസംഖ്യ		give_error,
    ufunc_T	*ufunc,
    cctx_T	*cctx)
{
    char_u  *p;
    type_T  *type;
    type_T  *ret_type = &t_unknown;
    പൂർണ്ണസംഖ്യ	argcount = -1;
    പൂർണ്ണസംഖ്യ	flags = 0;
    പൂർണ്ണസംഖ്യ	first_optional = -1;
    type_T	*arg_type[MAX_FUNC_ARGS + 1];

    // func({type}, ...{type}): {type}
    *arg += len;
    എങ്കിൽ (**arg == '(')
    {
	// "func" may or may not return a value, "func()" does
	// not return a value.
	ret_type = &t_void;

	p = ++*arg;
	argcount = 0;
	ആകുമ്പോൾ (*p != NUL && *p != ')')
	{
	    എങ്കിൽ (*p == '?')
	    {
		എങ്കിൽ (first_optional == -1)
		    first_optional = argcount;
		++p;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "...", 3) == 0)
	    {
		flags |= TTFLAG_VARARGS;
		p += 3;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (first_optional != -1)
	    {
		എങ്കിൽ (give_error)
		    emsg(_(e_mandatory_argument_after_optional_argument));
		തിരികെ ശൂന്യ;
	    }

	    type = parse_type(&p, type_gap, ufunc, cctx, give_error);
	    എങ്കിൽ (type == ശൂന്യ || !valid_declaration_type(type))
		തിരികെ ശൂന്യ;
	    എങ്കിൽ ((flags & TTFLAG_VARARGS) != 0 && type->tt_type != VAR_LIST)
	    {
		അക്ഷരം *tofree;
		semsg(_(e_variable_arguments_type_must_be_list_str),
			type_name(type, &tofree));
		vim_free(tofree);
		തിരികെ ശൂന്യ;
	    }
	    arg_type[argcount++] = type;

	    // Nothing comes after "...{type}".
	    എങ്കിൽ (flags & TTFLAG_VARARGS)
		നിർത്തുക;

	    എങ്കിൽ (*p != ',' && *skipwhite(p) == ',')
	    {
		എങ്കിൽ (give_error)
		    semsg(_(e_no_white_space_allowed_before_str_str),
			    ",", p);
		തിരികെ ശൂന്യ;
	    }
	    എങ്കിൽ (*p == ',')
	    {
		++p;
		എങ്കിൽ (!VIM_ISWHITE(*p))
		{
		    എങ്കിൽ (give_error)
			semsg(_(e_white_space_required_after_str_str),
				",", p - 1);
		    തിരികെ ശൂന്യ;
		}
	    }
	    p = skipwhite(p);
	    എങ്കിൽ (argcount == MAX_FUNC_ARGS)
	    {
		എങ്കിൽ (give_error)
		    emsg(_(e_too_many_argument_types));
		തിരികെ ശൂന്യ;
	    }
	}

	p = skipwhite(p);
	എങ്കിൽ (*p != ')')
	{
	    എങ്കിൽ (give_error)
		emsg(_(e_missing_closing_paren));
	    തിരികെ ശൂന്യ;
	}
	*arg = p + 1;
    }
    എങ്കിൽ (**arg == ':')
    {
	// parse return type
	++*arg;
	എങ്കിൽ (!VIM_ISWHITE(**arg) && give_error)
	    semsg(_(e_white_space_required_after_str_str), ":", *arg - 1);
	*arg = skipwhite(*arg);
	ret_type = parse_type(arg, type_gap, ufunc, cctx, give_error);
	എങ്കിൽ (ret_type == ശൂന്യ)
	    തിരികെ ശൂന്യ;
    }
    എങ്കിൽ (flags == 0 && first_optional == -1 && argcount <= 0)
	type = get_func_type(ret_type, argcount, type_gap);
    അല്ലെങ്കിൽ
    {
	type = alloc_func_type(ret_type, argcount, type_gap);
	type->tt_flags = flags;
	എങ്കിൽ (argcount > 0)
	{
	    type->tt_argcount = argcount;
	    type->tt_min_argcount = first_optional == -1
						? argcount : first_optional;
	    എങ്കിൽ (func_type_add_arg_types(type, argcount, type_gap) == FAIL)
		തിരികെ ശൂന്യ;
	    mch_memmove(type->tt_args, arg_type, വലുപ്പം(type_T *) * argcount);
	}
    }

    തിരികെ type;
}

/*
 * Parse a "tuple" type at "*arg" and advance over it.
 * When "give_error" is TRUE give error messages, otherwise be quiet.
 * Return NULL for failure.
 */
    സ്ഥിതം type_T *
parse_type_tuple(
    char_u	**arg,
    garray_T	*type_gap,
    പൂർണ്ണസംഖ്യ		give_error,
    ufunc_T	*ufunc,
    cctx_T	*cctx)
{
    char_u	*p;
    type_T	*type;
    type_T	*ret_type = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		typecount = -1;
    പൂർണ്ണസംഖ്യ		flags = 0;
    garray_T	tuple_types_ga;

    ga_init2(&tuple_types_ga, വലുപ്പം(type_T *), 10);

    // tuple<{type}, {type}>
    // tuple<{type}, ...{type}>
    എങ്കിൽ (**arg != '<')
    {
	എങ്കിൽ (give_error)
	{
	    എങ്കിൽ (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    അല്ലെങ്കിൽ
		semsg(_(e_missing_type_after_str), "tuple");
	}

	// only "tuple" is specified
	തിരികെ ശൂന്യ;
    }

    p = ++*arg;
    typecount = 0;
    ആകുമ്പോൾ (*p != NUL && *p != '>')
    {
	എങ്കിൽ (STRNCMP(p, "...", 3) == 0)
	{
	    flags |= TTFLAG_VARARGS;
	    p += 3;
	}

	type = parse_type(&p, type_gap, ufunc, cctx, give_error);
	എങ്കിൽ (type == ശൂന്യ || !valid_declaration_type(type))
	    പോകുക on_err;

	എങ്കിൽ ((flags & TTFLAG_VARARGS) != 0 && type->tt_type != VAR_LIST)
	{
	    അക്ഷരം *tofree;
	    semsg(_(e_variadic_tuple_must_end_with_list_type_str),
						type_name(type, &tofree));
	    vim_free(tofree);
	    പോകുക on_err;
	}

	// Add the item type
	എങ്കിൽ (ga_grow(&tuple_types_ga, 1) == FAIL)
	    പോകുക on_err;
	((type_T **)tuple_types_ga.ga_data)[tuple_types_ga.ga_len] = type;
	tuple_types_ga.ga_len++;
	typecount++;

	// Nothing comes after "...{type}".
	എങ്കിൽ (flags & TTFLAG_VARARGS)
	    നിർത്തുക;

	എങ്കിൽ (*p != ',' && *skipwhite(p) == ',')
	{
	    എങ്കിൽ (give_error)
		semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
	    പോകുക on_err;
	}
	എങ്കിൽ (*p == ',')
	{
	    ++p;
	    എങ്കിൽ (!VIM_ISWHITE(*p))
	    {
		എങ്കിൽ (give_error)
		    semsg(_(e_white_space_required_after_str_str),
			    ",", p - 1);
		പോകുക on_err;
	    }
	}
	p = skipwhite(p);
    }

    p = skipwhite(p);
    എങ്കിൽ (*p != '>' || typecount <= 0)
    {
	എങ്കിൽ (give_error)
	    semsg(_(e_missing_type_after_str), p);
	പോകുക on_err;
    }
    *arg = p + 1;

    ret_type = alloc_tuple_type(typecount, type_gap);
    എങ്കിൽ (ret_type == &t_any
	    || tuple_type_add_types(ret_type, typecount, type_gap) == FAIL)
    {
	ret_type = ശൂന്യ;
	പോകുക on_err;
    }
    ret_type->tt_flags = flags;
    ret_type->tt_argcount = typecount;
    mch_memmove(ret_type->tt_args, tuple_types_ga.ga_data,
						വലുപ്പം(type_T *) * typecount);

on_err:
    ga_clear(&tuple_types_ga);

    തിരികെ ret_type;
}

/*
 * Parse a "object" type at "*arg" and advance over it.
 * When "give_error" is TRUE give error messages, otherwise be quiet.
 * Return NULL for failure.
 */
    സ്ഥിതം type_T *
parse_type_object(
    char_u	**arg,
    garray_T	*type_gap,
    പൂർണ്ണസംഖ്യ		give_error,
    cctx_T	*cctx)
{
    char_u	*arg_start = *arg;
    type_T	*object_type;
    പൂർണ്ണസംഖ്യ		prev_called_emsg = called_emsg;

    // object<X> or object<any>
    എങ്കിൽ (**arg != '<')
    {
	എങ്കിൽ (give_error)
	{
	    എങ്കിൽ (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    അല്ലെങ്കിൽ
		semsg(_(e_missing_type_after_str), "object");
	}

	// only "object" is specified
	തിരികെ ശൂന്യ;
    }

    // skip spaces following "object<"
    *arg = skipwhite(*arg + 1);

    object_type = parse_type(arg, type_gap, ശൂന്യ, cctx, give_error);
    എങ്കിൽ (object_type == ശൂന്യ)
	തിരികെ ശൂന്യ;

    *arg = skipwhite(*arg);
    എങ്കിൽ (**arg != '>' && called_emsg == prev_called_emsg)
    {
	എങ്കിൽ (give_error)
	    semsg(_(e_missing_gt_after_type_str), arg_start);
	തിരികെ ശൂന്യ;
    }
    ++*arg;

    എങ്കിൽ (object_type->tt_type == VAR_ANY)
	തിരികെ &t_object_any;

    എങ്കിൽ (object_type->tt_type != VAR_OBJECT)
    {
	// specified type is not a class
	എങ്കിൽ (give_error)
	    semsg(_(e_class_name_not_found_str), arg_start);
	തിരികെ ശൂന്യ;
    }

    തിരികെ object_type;
}

/*
 * Parse a user defined type at "*arg" and advance over it.
 * It can be a class or an interface or a typealias name, possibly imported.
 * Return NULL if a type is not found.
 */
    സ്ഥിതം type_T *
parse_type_user_defined(
    char_u	**arg,
    size_t	len,
    garray_T	*type_gap,
    പൂർണ്ണസംഖ്യ		give_error,
    ufunc_T	*ufunc,
    cctx_T	*cctx)
{
    പൂർണ്ണസംഖ്യ		did_emsg_before = did_emsg;
    typval_T	tv;

    tv.v_type = VAR_UNKNOWN;
    എങ്കിൽ (eval_variable_import(*arg, &tv) == OK)
    {
	എങ്കിൽ (tv.v_type == VAR_CLASS && tv.vval.v_class != ശൂന്യ)
	{
	    type_T *type = get_type_ptr(type_gap);
	    എങ്കിൽ (type != ശൂന്യ)
	    {
		// Although the name is that of a class or interface, the type
		// uses will be an object.
		type->tt_type = VAR_OBJECT;
		type->tt_class = tv.vval.v_class;
		clear_tv(&tv);

		*arg += len;
		// Skip over ".ClassName".
		ആകുമ്പോൾ (ASCII_ISALNUM(**arg) || **arg == '_' || **arg == '.')
		    ++*arg;

		തിരികെ type;
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (tv.v_type == VAR_TYPEALIAS)
	{
	    // user defined type
	    // The type alias may itself be a nested type, check the resulting
	    // nesting depth.
	    പൂർണ്ണസംഖ്യ	depth = parse_depth - 1
				+ MAX(tv.vval.v_typealias->ta_depth, 1);
	    എങ്കിൽ (depth - 1 > MAX_TYPE_DEPTH)
	    {
		clear_tv(&tv);
		എങ്കിൽ (give_error)
		    emsg(_(e_type_nested_too_deep));
		തിരികെ ശൂന്യ;
	    }
	    എങ്കിൽ (depth > parse_max_depth)
		parse_max_depth = depth;

	    type_T *type = copy_type(tv.vval.v_typealias->ta_type, type_gap);
	    *arg += len;
	    clear_tv(&tv);
	    // Skip over ".TypeName".
	    ആകുമ്പോൾ (ASCII_ISALNUM(**arg) || **arg == '_' || **arg == '.')
		++*arg;
	    തിരികെ type;
	}

	clear_tv(&tv);
    }

    // Check whether it is a generic type
    type_T *type = find_generic_type(*arg, len, ufunc, cctx);
    എങ്കിൽ (type != ശൂന്യ)
    {
	*arg += len;
	തിരികെ type;
    }

    എങ്കിൽ (give_error && (did_emsg == did_emsg_before))
    {
	char_u	*p = skip_type(*arg, FALSE);
	അക്ഷരം	cc = *p;

	*p = NUL;
	semsg(_(e_type_not_recognized_str), *arg);
	*p = cc;
    }

    തിരികെ ശൂന്യ;
}

/*
 * Inner part of parse_type().
 */
    സ്ഥിതം type_T *
parse_type_inner(
    char_u	**arg,
    garray_T	*type_gap,
    ufunc_T	*ufunc,
    cctx_T	*cctx,
    പൂർണ്ണസംഖ്യ		give_error)
{
    char_u  *p = *arg;
    size_t  len;

    // Skip over the first word.
    ആകുമ്പോൾ (ASCII_ISALNUM(*p) || *p == '_')
	++p;
    len = p - *arg;

    തിരഞ്ഞെടുപ്പ് (**arg)
    {
	അവസ്ഥ 'a':
	    എങ്കിൽ (len == 3 && STRNCMP(*arg, "any", len) == 0)
	    {
		*arg += len;
		തിരികെ &t_any;
	    }
	    നിർത്തുക;
	അവസ്ഥ 'b':
	    എങ്കിൽ (len == 4 && STRNCMP(*arg, "bool", len) == 0)
	    {
		*arg += len;
		തിരികെ &t_bool;
	    }
	    എങ്കിൽ (len == 4 && STRNCMP(*arg, "blob", len) == 0)
	    {
		*arg += len;
		തിരികെ &t_blob;
	    }
	    നിർത്തുക;
	അവസ്ഥ 'c':
	    എങ്കിൽ (len == 7 && STRNCMP(*arg, "channel", len) == 0)
	    {
		*arg += len;
		തിരികെ &t_channel;
	    }
	    നിർത്തുക;
	അവസ്ഥ 'd':
	    എങ്കിൽ (len == 4 && STRNCMP(*arg, "dict", len) == 0)
	    {
		*arg += len;
		തിരികെ parse_type_member(arg, &t_dict_any, type_gap,
						give_error, "dict", ufunc,
						cctx);
	    }
	    നിർത്തുക;
	അവസ്ഥ 'f':
	    എങ്കിൽ (len == 5 && STRNCMP(*arg, "float", len) == 0)
	    {
		*arg += len;
		തിരികെ &t_float;
	    }
	    എങ്കിൽ (len == 4 && STRNCMP(*arg, "func", len) == 0)
		തിരികെ parse_type_func(arg, len, type_gap, give_error, ufunc,
									cctx);
	    നിർത്തുക;
	അവസ്ഥ 'j':
	    എങ്കിൽ (len == 3 && STRNCMP(*arg, "job", len) == 0)
	    {
		*arg += len;
		തിരികെ &t_job;
	    }
	    നിർത്തുക;
	അവസ്ഥ 'l':
	    എങ്കിൽ (len == 4 && STRNCMP(*arg, "list", len) == 0)
	    {
		*arg += len;
		തിരികെ parse_type_member(arg, &t_list_any, type_gap,
						give_error, "list", ufunc,
						cctx);
	    }
	    നിർത്തുക;
	അവസ്ഥ 'n':
	    എങ്കിൽ (len == 6 && STRNCMP(*arg, "number", len) == 0)
	    {
		*arg += len;
		തിരികെ &t_സംഖ്യ;
	    }
	    നിർത്തുക;
	അവസ്ഥ 'o':
	    എങ്കിൽ (len == 6 && STRNCMP(*arg, "object", len) == 0)
	    {
		*arg += len;
		തിരികെ parse_type_object(arg, type_gap, give_error, cctx);
	    }
	    നിർത്തുക;
	അവസ്ഥ 's':
	    എങ്കിൽ (len == 6 && STRNCMP(*arg, "string", len) == 0)
	    {
		*arg += len;
		തിരികെ &t_string;
	    }
	    നിർത്തുക;
	അവസ്ഥ 't':
	    എങ്കിൽ (len == 5 && STRNCMP(*arg, "tuple", len) == 0)
	    {
		*arg += len;
		തിരികെ parse_type_tuple(arg, type_gap, give_error, ufunc,
									cctx);
	    }
	    നിർത്തുക;
	അവസ്ഥ 'v':
	    എങ്കിൽ (len == 4 && STRNCMP(*arg, "void", len) == 0)
	    {
		*arg += len;
		തിരികെ &t_void;
	    }
	    നിർത്തുക;
    }

    // User defined type
    തിരികെ parse_type_user_defined(arg, len, type_gap, give_error, ufunc,
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
    പൂർണ്ണസംഖ്യ		give_error)
{
    type_T	*type;

    എങ്കിൽ (parse_depth > MAX_TYPE_DEPTH)
    {
	എങ്കിൽ (give_error)
	    emsg(_(e_type_nested_too_deep));
	തിരികെ ശൂന്യ;
    }
    എങ്കിൽ (parse_depth == 0)
	parse_max_depth = 0;
    ++parse_depth;
    എങ്കിൽ (parse_depth > parse_max_depth)
	parse_max_depth = parse_depth;
    type = parse_type_inner(arg, type_gap, ufunc, cctx, give_error);
    --parse_depth;
    തിരികെ type;
}

/*
 * Return the number of levels of the type last parsed by parse_type().
 */
    പൂർണ്ണസംഖ്യ
parse_type_depth(ശൂന്യം)
{
    തിരികെ parse_max_depth;
}

/*
 * Check if "type1" and "type2" are exactly the same.
 * "flags" can have ETYPE_ARG_UNKNOWN, which means that an unknown argument
 * type in "type1" is accepted.
 */
    പൂർണ്ണസംഖ്യ
equal_type(type_T *type1, type_T *type2, പൂർണ്ണസംഖ്യ flags)
{
    പൂർണ്ണസംഖ്യ i;

    എങ്കിൽ (type1 == ശൂന്യ || type2 == ശൂന്യ)
	തിരികെ FALSE;
    എങ്കിൽ (type1->tt_type != type2->tt_type)
	തിരികെ FALSE;
    തിരഞ്ഞെടുപ്പ് (type1->tt_type)
    {
	അവസ്ഥ VAR_UNKNOWN:
	അവസ്ഥ VAR_ANY:
	അവസ്ഥ VAR_VOID:
	അവസ്ഥ VAR_SPECIAL:
	അവസ്ഥ VAR_BOOL:
	അവസ്ഥ VAR_സംഖ്യ:
	അവസ്ഥ VAR_FLOAT:
	അവസ്ഥ VAR_STRING:
	അവസ്ഥ VAR_BLOB:
	അവസ്ഥ VAR_JOB:
	അവസ്ഥ VAR_CHANNEL:
	അവസ്ഥ VAR_INSTR:
	അവസ്ഥ VAR_TYPEALIAS:
	    നിർത്തുക;  // not composite is always OK
	അവസ്ഥ VAR_OBJECT:
	അവസ്ഥ VAR_CLASS:
	    // Objects are considered equal if they are from the same class
	    തിരികെ type1->tt_class == type2->tt_class;
	അവസ്ഥ VAR_LIST:
	അവസ്ഥ VAR_DICT:
	    തിരികെ equal_type(type1->tt_member, type2->tt_member, flags);
	അവസ്ഥ VAR_TUPLE:
	    എങ്കിൽ (type1->tt_argcount != type2->tt_argcount)
		തിരികെ FALSE;
	    എങ്കിൽ (type1->tt_argcount < 0
			   || type1->tt_args == ശൂന്യ || type2->tt_args == ശൂന്യ)
		തിരികെ TRUE;
	    വേണ്ടി (i = 0; i < type1->tt_argcount; ++i)
		എങ്കിൽ ((flags & ETYPE_ARG_UNKNOWN) == 0
			&& !equal_type(type1->tt_args[i], type2->tt_args[i],
									flags))
		    തിരികെ FALSE;
	    തിരികെ TRUE;
	അവസ്ഥ VAR_FUNC:
	അവസ്ഥ VAR_PARTIAL:
	    എങ്കിൽ (!equal_type(type1->tt_member, type2->tt_member, flags)
		    || type1->tt_argcount != type2->tt_argcount)
		തിരികെ FALSE;
	    എങ്കിൽ (type1->tt_argcount < 0
			   || type1->tt_args == ശൂന്യ || type2->tt_args == ശൂന്യ)
		തിരികെ TRUE;
	    വേണ്ടി (i = 0; i < type1->tt_argcount; ++i)
		എങ്കിൽ ((flags & ETYPE_ARG_UNKNOWN) == 0
			&& !equal_type(type1->tt_args[i], type2->tt_args[i],
									flags))
		    തിരികെ FALSE;
	    തിരികെ TRUE;
    }
    തിരികെ TRUE;
}

/*
 * Find the common type of "type1" (VAR_FUNC) and "type2" (VAR_FUNC) and put it
 * in "dest".  "type2" and "dest" may be the same.
 */
    സ്ഥിതം ശൂന്യം
common_type_var_func(
    type_T	*type1,
    type_T	*type2,
    type_T	**dest,
    garray_T	*type_gap)
{
    type_T *common;

    // When one of the types is t_func_unknown return the other one.
    // Useful if a list or dict item is null_func.
    എങ്കിൽ (type1 == &t_func_unknown)
    {
	*dest = type2;
	തിരികെ;
    }
    എങ്കിൽ (type2 == &t_func_unknown)
    {
	*dest = type1;
	തിരികെ;
    }

    common_type(type1->tt_member, type2->tt_member, &common, type_gap);
    എങ്കിൽ (type1->tt_argcount == type2->tt_argcount
	    && type1->tt_argcount >= 0)
    {
	പൂർണ്ണസംഖ്യ argcount = type1->tt_argcount;
	പൂർണ്ണസംഖ്യ i;

	*dest = alloc_func_type(common, argcount, type_gap);
	എങ്കിൽ (type1->tt_args != ശൂന്യ && type2->tt_args != ശൂന്യ)
	{
	    എങ്കിൽ (func_type_add_arg_types(*dest, argcount,
			type_gap) == OK)
		വേണ്ടി (i = 0; i < argcount; ++i)
		    common_type(type1->tt_args[i], type2->tt_args[i],
			    &(*dest)->tt_args[i], type_gap);
	}
    }
    അല്ലെങ്കിൽ
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
    ശൂന്യം
common_type(type_T *type1, type_T *type2, type_T **dest, garray_T *type_gap)
{
    എങ്കിൽ (equal_type(type1, type2, 0))
    {
	*dest = type1;
	തിരികെ;
    }

    // If either is VAR_UNKNOWN use the other type.  An empty list/dict has no
    // specific type.
    എങ്കിൽ (type1 == ശൂന്യ || type1->tt_type == VAR_UNKNOWN)
    {
	*dest = type2;
	തിരികെ;
    }
    എങ്കിൽ (type2 == ശൂന്യ || type2->tt_type == VAR_UNKNOWN)
    {
	*dest = type1;
	തിരികെ;
    }

    എങ്കിൽ (type1->tt_type == type2->tt_type)
    {
	എങ്കിൽ (type1->tt_type == VAR_LIST
		|| type1->tt_type == VAR_DICT)
	{
	    type_T *common;

	    common_type(type1->tt_member, type2->tt_member, &common, type_gap);
	    എങ്കിൽ (type1->tt_type == VAR_LIST)
		*dest = get_list_type(common, type_gap);
	    അല്ലെങ്കിൽ
		*dest = get_dict_type(common, type_gap);
	    തിരികെ;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (type1->tt_type == VAR_FUNC)
	{
	    common_type_var_func(type1, type2, dest, type_gap);
	    തിരികെ;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (type1->tt_type == VAR_OBJECT)
	{
	    *dest = &t_object_any;
	    തിരികെ;
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
    എങ്കിൽ (type->tt_type == VAR_TUPLE)
    {
	എങ്കിൽ (type->tt_argcount != 1)
	    തിരികെ &t_any;

	എങ്കിൽ (type->tt_flags & TTFLAG_VARARGS)
	    തിരികെ type->tt_args[0]->tt_member;
	അല്ലെങ്കിൽ
	    തിരികെ type->tt_args[0];
    }

    // For "any" the item type is not known.
    തിരികെ type->tt_member == ശൂന്യ ? &t_any : type->tt_member;
}

/*
 * Push an entry onto the type stack.  "type" used both for the current type
 * and the declared type.
 * Returns FAIL when out of memory.
 */
    പൂർണ്ണസംഖ്യ
തള്ളുക_type_അട്ടി(cctx_T *cctx, type_T *type)
{
    തിരികെ തള്ളുക_type_stack2(cctx, type, type);
}

/*
 * Push an entry onto the type stack.  "type" is the current type, "decl_type"
 * is the declared type.
 * Returns FAIL when out of memory.
 */
    പൂർണ്ണസംഖ്യ
തള്ളുക_type_stack2(cctx_T *cctx, type_T *type, type_T *decl_type)
{
    garray_T	*അട്ടി = &cctx->ctx_type_അട്ടി;
    type2_T	*typep;

    എങ്കിൽ (GA_GROW_FAILS(അട്ടി, 1))
	തിരികെ FAIL;
    typep = ((type2_T *)അട്ടി->ga_data) + അട്ടി->ga_len;
    typep->type_curr = type;
    typep->type_decl = decl_type;
    ++അട്ടി->ga_len;
    തിരികെ OK;
}

/*
 * Set the type of the top of the stack to "type".
 */
    ശൂന്യം
set_type_on_അട്ടി(cctx_T *cctx, type_T *type, പൂർണ്ണസംഖ്യ offset)
{
    garray_T	*അട്ടി = &cctx->ctx_type_അട്ടി;
    type2_T	*typep = ((type2_T *)അട്ടി->ga_data)
						  + അട്ടി->ga_len - 1 - offset;

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
get_type_on_അട്ടി(cctx_T *cctx, പൂർണ്ണസംഖ്യ offset)
{
    garray_T	*അട്ടി = &cctx->ctx_type_അട്ടി;

    എങ്കിൽ (offset + 1 > അട്ടി->ga_len)
	തിരികെ &t_unknown;
    തിരികെ (((type2_T *)അട്ടി->ga_data) + അട്ടി->ga_len - offset - 1)
								   ->type_curr;
}

/*
 * Get the declared type from the type stack.  If "offset" is zero the one at
 * the top,
 * if "offset" is one the type above that, etc.
 * Returns &t_unknown if there is no such stack entry.
 */
    type_T *
get_decl_type_on_അട്ടി(cctx_T *cctx, പൂർണ്ണസംഖ്യ offset)
{
    garray_T	*അട്ടി = &cctx->ctx_type_അട്ടി;

    എങ്കിൽ (offset + 1 > അട്ടി->ga_len)
	തിരികെ &t_unknown;
    തിരികെ (((type2_T *)അട്ടി->ga_data) + അട്ടി->ga_len - offset - 1)
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
get_member_type_from_അട്ടി(
	പൂർണ്ണസംഖ്യ	    എണ്ണം,
	പൂർണ്ണസംഖ്യ	    skip,
	cctx_T	    *cctx)
{
    garray_T	*അട്ടി = &cctx->ctx_type_അട്ടി;
    type2_T	*typep;
    garray_T    *type_gap = cctx->ctx_type_list;
    പൂർണ്ണസംഖ്യ		i;
    type_T	*result;
    type_T	*type;

    // Use "unknown" for an empty list or dict.
    എങ്കിൽ (എണ്ണം == 0)
	തിരികെ &t_unknown;
    // Find the common type from following items.
    typep = ((type2_T *)അട്ടി->ga_data) + അട്ടി->ga_len;
    result = &t_unknown;
    വേണ്ടി (i = 0; i < എണ്ണം; ++i)
    {
	type = (typep -((എണ്ണം - i) * skip) + skip - 1)->type_curr;
	എങ്കിൽ (check_type_is_മൂല്യം(type) == FAIL)
	    തിരികെ ശൂന്യ;
	എങ്കിൽ (result != &t_any)
	    common_type(type, result, &result, type_gap);
    }

    തിരികെ result;
}

/*
 * Get the types of items in a tuple on the stack of "cctx".
 * Returns the number of types.  Returns -1 on failure.
 */
    പൂർണ്ണസംഖ്യ
get_tuple_type_from_അട്ടി(
    പൂർണ്ണസംഖ്യ		എണ്ണം,
    garray_T	*tuple_types_gap,
    cctx_T	*cctx)
{
    garray_T	*അട്ടി = &cctx->ctx_type_അട്ടി;
    type2_T	*typep;
    type_T	*type = ശൂന്യ;

    // Use "unknown" for an empty tuple
    എങ്കിൽ (എണ്ണം == 0)
	തിരികെ 0;

    // Find the common type from following items.
    typep = ((type2_T *)അട്ടി->ga_data) + അട്ടി->ga_len;
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < എണ്ണം; i++)
    {
	type = (typep - (എണ്ണം - i))->type_curr;
	എങ്കിൽ (check_type_is_മൂല്യം(type) == FAIL)
	    തിരികെ -1;
	എങ്കിൽ (ga_grow(tuple_types_gap, 1) == FAIL)
	    തിരികെ -1;
	((type_T **)tuple_types_gap->ga_data)[tuple_types_gap->ga_len] = type;
	tuple_types_gap->ga_len++;
    }

    തിരികെ tuple_types_gap->ga_len;
}

    അക്ഷരം *
vartype_name(vartype_T type)
{
    തിരഞ്ഞെടുപ്പ് (type)
    {
	അവസ്ഥ VAR_UNKNOWN: നിർത്തുക;
	അവസ്ഥ VAR_ANY: തിരികെ "any";
	അവസ്ഥ VAR_VOID: തിരികെ "void";
	അവസ്ഥ VAR_SPECIAL: തിരികെ "special";
	അവസ്ഥ VAR_BOOL: തിരികെ "bool";
	അവസ്ഥ VAR_സംഖ്യ: തിരികെ "number";
	അവസ്ഥ VAR_FLOAT: തിരികെ "float";
	അവസ്ഥ VAR_STRING: തിരികെ "string";
	അവസ്ഥ VAR_BLOB: തിരികെ "blob";
	അവസ്ഥ VAR_JOB: തിരികെ "job";
	അവസ്ഥ VAR_CHANNEL: തിരികെ "channel";
	അവസ്ഥ VAR_LIST: തിരികെ "list";
	അവസ്ഥ VAR_TUPLE: തിരികെ "tuple";
	അവസ്ഥ VAR_DICT: തിരികെ "dict";
	അവസ്ഥ VAR_INSTR: തിരികെ "instr";
	അവസ്ഥ VAR_CLASS: തിരികെ "class";
	അവസ്ഥ VAR_OBJECT: തിരികെ "object";
	അവസ്ഥ VAR_TYPEALIAS: തിരികെ "typealias";

	അവസ്ഥ VAR_FUNC:
	അവസ്ഥ VAR_PARTIAL: തിരികെ "func";
    }
    തിരികെ "unknown";
}

/*
 * Return the type name of a List (list<type>) or Dict (dict<type>).
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    സ്ഥിതം അക്ഷരം *
type_name_list_or_dict(അക്ഷരം *name, type_T *type, അക്ഷരം **tofree)
{
    അക്ഷരം *member_free;
    അക്ഷരം *member_name;

    എങ്കിൽ (type->tt_member->tt_type == VAR_UNKNOWN)
	member_name = type_name(&t_any, &member_free);
    അല്ലെങ്കിൽ
	member_name = type_name(type->tt_member, &member_free);

    size_t len = STRLEN(name) + STRLEN(member_name) + 3;
    *tofree = alloc(len);
    എങ്കിൽ (*tofree == ശൂന്യ)
    {
	vim_free(member_free);
	തിരികെ name;
    }

    vim_snprintf(*tofree, len, "%s<%s>", name, member_name);
    vim_free(member_free);
    തിരികെ *tofree;
}

/*
 * Return the type name of a tuple.
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    സ്ഥിതം അക്ഷരം *
type_name_tuple(type_T *type, അക്ഷരം **tofree)
{
    garray_T    ga;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;
    അക്ഷരം	*arg_free = ശൂന്യ;

    ga_init2(&ga, 1, 100);
    എങ്കിൽ (ga_grow(&ga, 20) == FAIL)
	പോകുക failed;
    STRCPY(ga.ga_data, "tuple<");
    ga.ga_len += 6;

    എങ്കിൽ (type->tt_argcount <= 0)
	// empty tuple
	GA_CONCAT_LITERAL(&ga, "any");
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (type->tt_args == ശൂന്യ)
	    GA_CONCAT_LITERAL(&ga, "[unknown]");
	അല്ലെങ്കിൽ
	{
	    വേണ്ടി (i = 0; i < type->tt_argcount; ++i)
	    {
		string_T    arg_type;

		arg_type.string = (char_u *)type_name(type->tt_args[i], &arg_free);
		എങ്കിൽ (i > 0)
		{
		    STRCPY((അക്ഷരം *)ga.ga_data + ga.ga_len, ", ");
		    ga.ga_len += 2;
		}
		arg_type.length = STRLEN(arg_type.string);
		എങ്കിൽ (ga_grow(&ga, (പൂർണ്ണസംഖ്യ)arg_type.length + 8) == FAIL)
		    പോകുക failed;
		എങ്കിൽ (varargs && i == type->tt_argcount - 1)
		    GA_CONCAT_LITERAL(&ga, "...");
		ga_concat_len(&ga, arg_type.string, arg_type.length);
		VIM_CLEAR(arg_free);
	    }
	}
    }

    STRCPY((അക്ഷരം *)ga.ga_data + ga.ga_len, ">");
    *tofree = ga.ga_data;
    തിരികെ ga.ga_data;

failed:
    vim_free(arg_free);
    ga_clear(&ga);
    തിരികെ "[unknown]";
}

/*
 * Return the type name of a Class (class<name>) or Object (object<name>).
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    സ്ഥിതം അക്ഷരം *
type_name_class_or_obj(അക്ഷരം *name, type_T *type, അക്ഷരം **tofree)
{
    string_T	class_name;

    എങ്കിൽ (type->tt_class != ശൂന്യ)
    {
	class_name.string = type->tt_class->class_name.string;
	class_name.length = type->tt_class->class_name.length;
	എങ്കിൽ (IS_ENUM(type->tt_class))
	    name = "enum";
    }
    അല്ലെങ്കിൽ
	STR_LITERAL_SET(class_name, "any");

    size_t len = STRLEN(name) + class_name.length + 3;
    *tofree = alloc(len);
    എങ്കിൽ (*tofree == ശൂന്യ)
	തിരികെ name;

    vim_snprintf(*tofree, len, "%s<%s>", name, class_name.string);
    തിരികെ *tofree;
}

/*
 * Return the type name of a function.
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    സ്ഥിതം അക്ഷരം *
type_name_func(type_T *type, അക്ഷരം **tofree)
{
    garray_T    ga;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;
    അക്ഷരം	*arg_free = ശൂന്യ;

    ga_init2(&ga, 1, 100);
    എങ്കിൽ (ga_grow(&ga, 20) == FAIL)
	പോകുക failed;
    STRCPY(ga.ga_data, "func(");
    ga.ga_len += 5;

    വേണ്ടി (i = 0; i < type->tt_argcount; ++i)
    {
	string_T    arg_type;

	എങ്കിൽ (type->tt_args == ശൂന്യ)
	    STR_LITERAL_SET(arg_type, "[unknown]");
	അല്ലെങ്കിൽ
	{
	    arg_type.string = (char_u *)type_name(type->tt_args[i], &arg_free);
	    arg_type.length = STRLEN(arg_type.string);
	}
	എങ്കിൽ (i > 0)
	{
	    STRCPY((അക്ഷരം *)ga.ga_data + ga.ga_len, ", ");
	    ga.ga_len += 2;
	}
	എങ്കിൽ (ga_grow(&ga, (പൂർണ്ണസംഖ്യ)arg_type.length + 8) == FAIL)
	    പോകുക failed;
	എങ്കിൽ (varargs && i == type->tt_argcount - 1)
	    GA_CONCAT_LITERAL(&ga, "...");
	അല്ലെങ്കിൽ എങ്കിൽ (i >= type->tt_min_argcount)
	    *((അക്ഷരം *)ga.ga_data + ga.ga_len++) = '?';
	ga_concat_len(&ga, arg_type.string, arg_type.length);
	VIM_CLEAR(arg_free);
    }
    എങ്കിൽ (type->tt_argcount < 0)
	// any number of arguments
	GA_CONCAT_LITERAL(&ga, "...");

    എങ്കിൽ (type->tt_member == &t_void)
	STRCPY((അക്ഷരം *)ga.ga_data + ga.ga_len, ")");
    അല്ലെങ്കിൽ
    {
	അക്ഷരം *ret_free = ശൂന്യ;
	അക്ഷരം *ret_name = type_name(type->tt_member, &ret_free);
	പൂർണ്ണസംഖ്യ  len;

	len = (പൂർണ്ണസംഖ്യ)STRLEN(ret_name) + 4;
	എങ്കിൽ (ga_grow(&ga, len) == FAIL)
	{
	    vim_free(ret_free);
	    പോകുക failed;
	}
	STRCPY((അക്ഷരം *)ga.ga_data + ga.ga_len, "): ");
	STRCPY((അക്ഷരം *)ga.ga_data + ga.ga_len + 3, ret_name);
	vim_free(ret_free);
    }
    *tofree = ga.ga_data;
    തിരികെ ga.ga_data;

failed:
    vim_free(arg_free);
    ga_clear(&ga);
    തിരികെ "[unknown]";
}

/*
 * Return the name of a type.
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    അക്ഷരം *
type_name(type_T *type, അക്ഷരം **tofree)
{
    അക്ഷരം *name;

    *tofree = ശൂന്യ;
    എങ്കിൽ (type == ശൂന്യ)
	തിരികെ "[unknown]";
    name = vartype_name(type->tt_type);

    തിരഞ്ഞെടുപ്പ് (type->tt_type)
    {
	അവസ്ഥ VAR_LIST:
	അവസ്ഥ VAR_DICT:
	    തിരികെ type_name_list_or_dict(name, type, tofree);

	അവസ്ഥ VAR_TUPLE:
	    തിരികെ type_name_tuple(type, tofree);

	അവസ്ഥ VAR_CLASS:
	അവസ്ഥ VAR_OBJECT:
	    തിരികെ type_name_class_or_obj(name, type, tofree);

	അവസ്ഥ VAR_FUNC:
	    തിരികെ type_name_func(type, tofree);

	സ്ഥിരസ്ഥിതി:
	    നിർത്തുക;
    }

    തിരികെ name;
}

/*
 * "typename(expr)" function
 */
    ശൂന്യം
f_typename(typval_T *argvars, typval_T *rettv)
{
    garray_T	type_list;
    type_T	*type;
    അക്ഷരം	*tofree;
    അക്ഷരം	*name;

    rettv->v_type = VAR_STRING;
    ga_init2(&type_list, വലുപ്പം(type_T *), 10);
    എങ്കിൽ (argvars[0].v_type == VAR_TYPEALIAS)
    {
	type = copy_type(argvars[0].vval.v_typealias->ta_type, &type_list);
	// A type alias for a class has the type set to VAR_OBJECT.  Change it
	// to VAR_CLASS, so that the name is "typealias<class<xxx>>"
	എങ്കിൽ (type->tt_type == VAR_OBJECT)
	    type->tt_type = VAR_CLASS;
    }
    അല്ലെങ്കിൽ
	type = typval2type(argvars, get_copyID(), &type_list, TVTT_DO_MEMBER);
    name = type_name(type, &tofree);
    എങ്കിൽ (argvars[0].v_type == VAR_TYPEALIAS)
    {
	vim_snprintf((അക്ഷരം *)IObuff, IOSIZE, "typealias<%s>", name);
	rettv->vval.v_string = vim_strsave((char_u *)IObuff);
	എങ്കിൽ (tofree != ശൂന്യ)
	    vim_free(tofree);
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (tofree != ശൂന്യ)
	    rettv->vval.v_string = (char_u *)tofree;
	അല്ലെങ്കിൽ
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
    പൂർണ്ണസംഖ്യ
check_typval_is_മൂല്യം(typval_T *tv)
{
    എങ്കിൽ (tv == ശൂന്യ)
	തിരികെ OK;

    തിരഞ്ഞെടുപ്പ് (tv->v_type)
    {
	അവസ്ഥ VAR_CLASS:
	    {
		class_T *cl = tv->vval.v_class;
		char_u *class_name = (cl == ശൂന്യ) ? (char_u *)""
							: cl->class_name.string;
		എങ്കിൽ (cl != ശൂന്യ && IS_ENUM(cl))
		    semsg(_(e_using_enum_as_മൂല്യം_str), class_name);
		അല്ലെങ്കിൽ
		    semsg(_(e_using_class_as_മൂല്യം_str), class_name);
	    }
	    തിരികെ FAIL;

	അവസ്ഥ VAR_TYPEALIAS:
	    semsg(_(e_using_typealias_as_മൂല്യം_str),
		    tv->vval.v_typealias->ta_name);
	    തിരികെ FAIL;

	സ്ഥിരസ്ഥിതി:
	    നിർത്തുക;
    }
    തിരികെ OK;
}

/*
 * Same as above, except check type_T.
 */
    പൂർണ്ണസംഖ്യ
check_type_is_മൂല്യം(type_T *type)
{
    എങ്കിൽ (type == ശൂന്യ)
	തിരികെ OK;
    തിരഞ്ഞെടുപ്പ് (type->tt_type)
    {
	അവസ്ഥ VAR_CLASS:
	    എങ്കിൽ (type->tt_class != ശൂന്യ && IS_ENUM(type->tt_class))
		semsg(_(e_using_enum_as_മൂല്യം_str),
			type->tt_class->class_name.string);
	    അല്ലെങ്കിൽ
		semsg(_(e_using_class_as_മൂല്യം_str),
			type->tt_class == ശൂന്യ ? (char_u *)""
			: type->tt_class->class_name.string);
	    തിരികെ FAIL;

	അവസ്ഥ VAR_TYPEALIAS:
	    // TODO: Not sure what could be done here to get a name.
	    //       Maybe an optional argument?
	    emsg(_(e_using_typealias_as_var_val));
	    തിരികെ FAIL;

	സ്ഥിരസ്ഥിതി:
	    നിർത്തുക;
    }
    തിരികെ OK;
}

#endif // FEAT_EVAL
