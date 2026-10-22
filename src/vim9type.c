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
#अन्तर्भाव "vim.h"

#सचे defined(FEAT_EVAL)

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

    सचे (ga_grow(type_gap, 1) == FAIL)
	पटिनिवत्त रिक्त;
    type = ALLOC_CLEAR_ONE(type_T);
    सचे (type == रिक्त)
	पटिनिवत्त रिक्त;

    ((type_T **)type_gap->ga_data)[type_gap->ga_len] = type;
    ++type_gap->ga_len;
    पटिनिवत्त type;
}

/*
 * Make a shallow copy of "type".
 * When allocation fails returns "type".
 */
    type_T *
copy_type(type_T *type, garray_T *type_gap)
{
    type_T *copy = get_type_ptr(type_gap);

    सचे (copy == रिक्त)
	पटिनिवत्त type;
    *copy = *type;
    copy->tt_flags &= ~TTFLAG_STATIC;

    सचे (type->tt_args != रिक्त
	   && func_type_add_arg_types(copy, type->tt_argcount, type_gap) == OK)
	अत्थाय (पूर्णाङ्क i = 0; i < type->tt_argcount; ++i)
	    copy->tt_args[i] = type->tt_args[i];

    पटिनिवत्त copy;
}

/*
 * Inner part of copy_type_deep().
 * When allocation fails returns "type".
 */
    स्थिर type_T *
copy_type_deep_rec(type_T *type, garray_T *type_gap, garray_T *seen_types)
{
    अत्थाय (पूर्णाङ्क i = 0; i < seen_types->ga_len; ++i)
	सचे (((type_T **)seen_types->ga_data)[i * 2] == type)
	    // seen this type before, return the copy we made
	    पटिनिवत्त ((type_T **)seen_types->ga_data)[i * 2 + 1];

    type_T *copy = copy_type(type, type_gap);
    सचे (ga_grow(seen_types, 1) == FAIL)
	पटिनिवत्त copy;
    ((type_T **)seen_types->ga_data)[seen_types->ga_len * 2] = type;
    ((type_T **)seen_types->ga_data)[seen_types->ga_len * 2 + 1] = copy;
    ++seen_types->ga_len;

    सचे (copy->tt_member != रिक्त)
	copy->tt_member = copy_type_deep_rec(copy->tt_member,
							 type_gap, seen_types);
    सचे (type->tt_args != रिक्त)
	अत्थाय (पूर्णाङ्क i = 0; i < type->tt_argcount; ++i)
	    copy->tt_args[i] = copy_type_deep_rec(copy->tt_args[i],
							 type_gap, seen_types);

    पटिनिवत्त copy;
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
    ga_init2(&seen_types, परिमाण(type_T *) * 2, 20);

    type_T *res = copy_type_deep_rec(type, type_gap, &seen_types);

    ga_clear(&seen_types);
    पटिनिवत्त res;
}

    शून्य
clear_type_list(garray_T *gap)
{
    याव (gap->ga_len > 0)
	vim_free(((type_T **)gap->ga_data)[--gap->ga_len]);
    ga_clear(gap);
}

    शून्य
clear_func_type_list(garray_T *gap, type_T **func_type)
{
    याव (gap->ga_len > 0)
    {
	// func_type pointing to the uf_type_list, so reset pointer
	सचे (*func_type == ((type_T **)gap->ga_data)[--gap->ga_len])
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

    सचे (type == रिक्त)
	पटिनिवत्त रिक्त;

    // A fixed type never contains allocated types, return as-is.
    सचे (type->tt_flags & TTFLAG_STATIC)
	पटिनिवत्त type;

    ret = ALLOC_ONE(type_T);
    सचे (ret == रिक्त)
	पटिनिवत्त रिक्त;
    *ret = *type;

    सचे (ret->tt_member != रिक्त)
	ret->tt_member = alloc_type(ret->tt_member);

    सचे (type->tt_argcount > 0 && type->tt_args != रिक्त)
    {
	पूर्णाङ्क i;

	ret->tt_args = ALLOC_MULT(type_T *, type->tt_argcount);
	सचे (ret->tt_args != रिक्त)
	    अत्थाय (i = 0; i < type->tt_argcount; ++i)
		ret->tt_args[i] = alloc_type(type->tt_args[i]);
    }
    अञ्ञथा
	ret->tt_args = रिक्त;

    पटिनिवत्त ret;
}

/*
 * Free a type that was created with alloc_type().
 */
    शून्य
free_type(type_T *type)
{
    पूर्णाङ्क i;

    सचे (type == रिक्त || (type->tt_flags & TTFLAG_STATIC))
	पटिनिवत्त;
    सचे (type->tt_args != रिक्त)
    {
	अत्थाय (i = 0; i < type->tt_argcount; ++i)
	    free_type(type->tt_args[i]);
	vim_free(type->tt_args);
    }

    free_type(type->tt_member);

    vim_free(type);
}

/*
 * Return TRUE if "type" is to be recursed into for setting the type.
 */
    स्थिर पूर्णाङ्क
set_tv_type_recurse(type_T *type)
{
    पटिनिवत्त type->tt_member != रिक्त
		&& (type->tt_member->tt_type == VAR_DICT
				|| type->tt_member->tt_type == VAR_LIST)
		&& type->tt_member->tt_member != रिक्त
		&& type->tt_member->tt_member != &t_any
		&& type->tt_member->tt_member != &t_unknown;
}

/*
 * Set the type of Dict "d" to "type"
 */
    स्थिर शून्य
set_tv_type_dict(dict_T *d, type_T *type)
{
    सचे (d->dv_type == type)
	पटिनिवत्त;

    free_type(d->dv_type);
    d->dv_type = alloc_type(type);

    // Need to recursively set the type of dict items?
    सचे (!set_tv_type_recurse(type))
	पटिनिवत्त;

    पूर्णाङ्क		todo = (पूर्णाङ्क)d->dv_hashtab.ht_used;
    hashitem_T	*hi;
    dictitem_T	*di;

    FOR_ALL_HASHTAB_वस्तूनि(&d->dv_hashtab, hi, todo)
    {
	सचे (!HASHITEM_EMPTY(hi))
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
    स्थिर शून्य
set_tv_type_list(list_T *l, type_T *type)
{
    सचे (l->lv_type == type)
	पटिनिवत्त;

    free_type(l->lv_type);
    l->lv_type = alloc_type(type);

    // Need to recursively set the type of list items?
    सचे (l->lv_first == &range_list_item || !set_tv_type_recurse(type))
	पटिनिवत्त;

    listitem_T	*li;

    FOR_ALL_LIST_वस्तूनि(l, li)
	set_tv_type(&li->li_tv, type->tt_member);
}

/*
 * Set the type of Tuple "tuple" to "type"
 */
    स्थिर शून्य
set_tv_type_tuple(tuple_T *tuple, type_T *type)
{
    सचे (tuple->tv_type == type)
	पटिनिवत्त;

    free_type(tuple->tv_type);
    tuple->tv_type = alloc_type(type);

    सचे (type->tt_argcount <= 0)
	पटिनिवत्त;

    // recursively set the type of list items
    type_T	*item_type;
    अत्थाय (पूर्णाङ्क i = 0; i < tuple_len(tuple); i++)
    {
	सचे ((type->tt_flags & TTFLAG_VARARGS) && (i >= type->tt_argcount - 1))
	    // For a variadic tuple, the last type is a List.  So use the
	    // List member type.
	    item_type = type->tt_args[type->tt_argcount - 1]->tt_member;
	अञ्ञथा
	    item_type = type->tt_args[i];

	set_tv_type(TUPLE_ITEM(tuple, i), item_type);
    }
}

/*
 * Set the type of "tv" to "type" if it is a list or tuple or dict.
 */
    शून्य
set_tv_type(typval_T *tv, type_T *type)
{
    सचे (type->tt_type == VAR_ANY)
	// If the variable type is "any", then keep the value type.
	// e.g.  var x: any = [1, 2] or var y: any = {v: 1}
	पटिनिवत्त;

    सचे (tv->v_type == VAR_DICT && tv->vval.v_dict != रिक्त)
	set_tv_type_dict(tv->vval.v_dict, type);
    अञ्ञथा सचे (tv->v_type == VAR_LIST && tv->vval.v_list != रिक्त)
	set_tv_type_list(tv->vval.v_list, type);
    अञ्ञथा सचे (tv->v_type == VAR_TUPLE && tv->vval.v_tuple != रिक्त)
	set_tv_type_tuple(tv->vval.v_tuple, type);
}

/*
 * For a tuple type, reserve space for "typecount" types (including the
 * repeated type).
 */
    स्थिर पूर्णाङ्क
tuple_type_add_types(
    type_T	*tupletype,
    पूर्णाङ्क		typecount,
    garray_T	*type_gap)
{
    // To make it easy to free the space needed for the types, add the
    // pointer to type_gap.
    सचे (ga_grow(type_gap, 1) == FAIL)
	पटिनिवत्त FAIL;
    tupletype->tt_args = alloc_clear_id(परिमाण(type_T *) * typecount,
							  aid_tuple_type_args);
    सचे (tupletype->tt_args == रिक्त)
	पटिनिवत्त FAIL;
    ((type_T **)type_gap->ga_data)[type_gap->ga_len] =
						(शून्य *)tupletype->tt_args;
    ++type_gap->ga_len;
    पटिनिवत्त OK;
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
    सचे (member_type == रिक्त || (member_type->tt_type == VAR_ANY
					&& !IS_GENERIC_TYPE(member_type)))
	पटिनिवत्त &t_list_any;
    सचे (member_type->tt_type == VAR_VOID
	    || member_type->tt_type == VAR_UNKNOWN)
	पटिनिवत्त &t_list_empty;
    सचे (member_type->tt_type == VAR_BOOL)
	पटिनिवत्त &t_list_bool;
    सचे (member_type->tt_type == VAR_सङ्ख्या)
	पटिनिवत्त &t_list_सङ्ख्या;
    सचे (member_type->tt_type == VAR_STRING)
	पटिनिवत्त &t_list_string;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    सचे (type == रिक्त)
	पटिनिवत्त &t_any;
    type->tt_type = VAR_LIST;
    type->tt_member = member_type;
    type->tt_argcount = 0;
    type->tt_args = रिक्त;
    पटिनिवत्त type;
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
    पूर्णाङ्क		typecount = tuple_types_gap->ga_len;

    // recognize commonly used types
    सचे (typecount == 0)
	पटिनिवत्त &t_tuple_any;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    सचे (type == रिक्त)
	पटिनिवत्त &t_any;
    type->tt_type = VAR_TUPLE;
    type->tt_member = रिक्त;
    सचे (typecount > 0)
    {
	सचे (tuple_type_add_types(type, typecount, type_gap) == FAIL)
	    पटिनिवत्त &t_any;
	mch_memmove(type->tt_args, tuple_types, परिमाण(type_T *) * typecount);
    }
    type->tt_argcount = typecount;
    type->tt_flags = 0;

    पटिनिवत्त type;
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
    सचे (member_type == रिक्त || (member_type->tt_type == VAR_ANY
					&& !IS_GENERIC_TYPE(member_type)))
	पटिनिवत्त &t_dict_any;
    सचे (member_type->tt_type == VAR_VOID
	    || member_type->tt_type == VAR_UNKNOWN)
	पटिनिवत्त &t_dict_empty;
    सचे (member_type->tt_type == VAR_BOOL)
	पटिनिवत्त &t_dict_bool;
    सचे (member_type->tt_type == VAR_सङ्ख्या)
	पटिनिवत्त &t_dict_सङ्ख्या;
    सचे (member_type->tt_type == VAR_STRING)
	पटिनिवत्त &t_dict_string;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    सचे (type == रिक्त)
	पटिनिवत्त &t_any;
    type->tt_type = VAR_DICT;
    type->tt_member = member_type;
    type->tt_argcount = 0;
    type->tt_args = रिक्त;
    पटिनिवत्त type;
}

/*
 * Allocate a new type for a function.
 */
    type_T *
alloc_func_type(type_T *ret_type, पूर्णाङ्क argcount, garray_T *type_gap)
{
    type_T *type = get_type_ptr(type_gap);

    सचे (type == रिक्त)
	पटिनिवत्त &t_any;
    type->tt_type = VAR_FUNC;
    type->tt_member = ret_type == रिक्त ? &t_unknown : ret_type;
    type->tt_argcount = argcount;
    type->tt_args = रिक्त;
    पटिनिवत्त type;
}

/*
 * Allocate a new type for a tuple.
 */
    स्थिर type_T *
alloc_tuple_type(पूर्णाङ्क typecount, garray_T *type_gap)
{
    type_T *type = get_type_ptr(type_gap);

    सचे (type == रिक्त)
	पटिनिवत्त &t_any;
    type->tt_type = VAR_TUPLE;
    type->tt_member = रिक्त;
    type->tt_argcount = typecount;
    type->tt_args = रिक्त;
    पटिनिवत्त type;
}

/*
 * Get a function type, based on the return type "ret_type".
 * "argcount" must be -1 or 0, a predefined type can be used.
 */
    type_T *
get_func_type(type_T *ret_type, पूर्णाङ्क argcount, garray_T *type_gap)
{
    // recognize commonly used types
    सचे (ret_type == &t_unknown || ret_type == रिक्त)
    {
	// (argcount == 0) is not possible
	पटिनिवत्त &t_func_unknown;
    }
    सचे (ret_type == &t_void)
    {
	सचे (argcount == 0)
	    पटिनिवत्त &t_func_0_void;
	अञ्ञथा
	    पटिनिवत्त &t_func_void;
    }
    सचे (ret_type == &t_any)
    {
	सचे (argcount == 0)
	    पटिनिवत्त &t_func_0_any;
	अञ्ञथा
	    पटिनिवत्त &t_func_any;
    }
    सचे (ret_type == &t_सङ्ख्या)
    {
	सचे (argcount == 0)
	    पटिनिवत्त &t_func_0_सङ्ख्या;
	अञ्ञथा
	    पटिनिवत्त &t_func_सङ्ख्या;
    }
    सचे (ret_type == &t_string)
    {
	सचे (argcount == 0)
	    पटिनिवत्त &t_func_0_string;
	अञ्ञथा
	    पटिनिवत्त &t_func_string;
    }

    पटिनिवत्त alloc_func_type(ret_type, argcount, type_gap);
}

/*
 * For a function type, reserve space for "argcount" argument types (including
 * vararg).
 */
    पूर्णाङ्क
func_type_add_arg_types(
	type_T	    *functype,
	पूर्णाङ्क	    argcount,
	garray_T    *type_gap)
{
    // To make it easy to free the space needed for the argument types, add the
    // pointer to type_gap.
    सचे (ga_grow(type_gap, 1) == FAIL)
	पटिनिवत्त FAIL;
    functype->tt_args = ALLOC_CLEAR_MULT(type_T *, argcount);
    सचे (functype->tt_args == रिक्त)
	पटिनिवत्त FAIL;
    ((type_T **)type_gap->ga_data)[type_gap->ga_len] =
						     (शून्य *)functype->tt_args;
    ++type_gap->ga_len;
    पटिनिवत्त OK;
}

/*
 * Return TRUE if "type" is NULL, any or unknown.
 * This also works for const (comparing with &t_any and &t_unknown doesn't).
 */
    पूर्णाङ्क
type_any_or_unknown(type_T *type)
{
    पटिनिवत्त type == रिक्त || type->tt_type == VAR_ANY
					|| type->tt_type == VAR_UNKNOWN;
}

/*
 * Get a type_T for a "special" typval in "tv".
 */
    स्थिर type_T *
special_typval2type(typval_T *tv)
{
    विकल्प (tv->vval.v_सङ्ख्या)
    {
	पक्ष VVAL_NULL:
	    पटिनिवत्त &t_null;

	पक्ष VVAL_NONE:
	    पटिनिवत्त &t_none;

	पक्ष VVAL_TRUE:
	पक्ष VVAL_FALSE:
	    पटिनिवत्त &t_bool;

	स्वभाव:
	    पटिनिवत्त &t_unknown;
    }
}

/*
 * Get a type_T for a List typval in "tv".
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    स्थिर type_T *
list_typval2type(typval_T *tv, पूर्णाङ्क copyID, garray_T *type_gap, पूर्णाङ्क flags)
{
    list_T	*l = tv->vval.v_list;
    listitem_T	*li;
    type_T	*member_type = रिक्त;

    // An empty list has type list<unknown>, unless the type was specified
    // and is not list<any>.  This matters when assigning to a variable
    // with a specific list type.
    सचे (l == रिक्त || (l->lv_first == रिक्त
		&& (l->lv_type == रिक्त || l->lv_type->tt_member == &t_any)))
	पटिनिवत्त &t_list_empty;

    सचे ((flags & TVTT_DO_MEMBER) == 0)
	पटिनिवत्त &t_list_any;

    // If the type is list<any> go through the members, it may end up a
    // more specific type.
    सचे (l->lv_type != रिक्त && (l->lv_first == रिक्त
		|| (flags & TVTT_MORE_SPECIFIC) == 0
		|| l->lv_type->tt_member != &t_any))
	// make a copy, lv_type may be freed if the list is freed
	पटिनिवत्त copy_type_deep(l->lv_type, type_gap);

    सचे (l->lv_first == &range_list_item)
	पटिनिवत्त &t_list_सङ्ख्या;

    सचे (l->lv_copyID == copyID)
	// avoid recursion
	पटिनिवत्त &t_list_any;

    l->lv_copyID = copyID;

    // Use the common type of all members.
    member_type = typval2type(&l->lv_first->li_tv, copyID, type_gap,
							TVTT_DO_MEMBER);
    अत्थाय (li = l->lv_first->li_next; li != रिक्त; li = li->li_next)
	common_type(typval2type(&li->li_tv, copyID, type_gap, TVTT_DO_MEMBER),
					member_type, &member_type, type_gap);

    // Reset copyID so that a shared reference to this list (not a circular
    // reference) can be processed again to get the correct type.
    l->lv_copyID = 0;

    पटिनिवत्त get_list_type(member_type, type_gap);
}

/*
 * Get a type_T for a Tuple typval in "tv".
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    स्थिर type_T *
tuple_typval2type(typval_T *tv, पूर्णाङ्क copyID, garray_T *type_gap, पूर्णाङ्क flags)
{
    tuple_T	*tuple = tv->vval.v_tuple;
    पूर्णाङ्क		len = tuple_len(tuple);
    type_T	*type = रिक्त;

    // An empty tuple has type tuple<unknown>, unless the type was specified
    // and is not tuple<any>.  This matters when assigning to a variable
    // with a specific tuple type.
    सचे (tuple == रिक्त || (len == 0 && (tuple->tv_type == रिक्त
				|| tuple->tv_type->tt_argcount == 0)))
	पटिनिवत्त &t_tuple_empty;

    सचे ((flags & TVTT_DO_MEMBER) == 0)
	पटिनिवत्त &t_tuple_any;

    // If the type is tuple<any> go through the members, it may end up a
    // more specific type.
    सचे (tuple->tv_type != रिक्त && (len == 0
					|| (flags & TVTT_MORE_SPECIFIC) == 0))
	// make a copy, tv_type may be freed if the tuple is freed
	पटिनिवत्त copy_type_deep(tuple->tv_type, type_gap);

    सचे (tuple->tv_copyID == copyID)
	// avoid recursion
	पटिनिवत्त &t_tuple_any;

    tuple->tv_copyID = copyID;

    garray_T	tuple_types_ga;
    ga_init2(&tuple_types_ga, परिमाण(type_T *), 10);
    अत्थाय (पूर्णाङ्क i = 0; i < len; i++)
    {
	type = typval2type(TUPLE_ITEM(tuple, i), copyID, type_gap,
							TVTT_DO_MEMBER);
	सचे (ga_grow(&tuple_types_ga, 1) == FAIL)
	{
	    ga_clear(&tuple_types_ga);
	    // Reset copyID so that a shared reference to this tuple can be
	    // processed again.
	    tuple->tv_copyID = 0;
	    पटिनिवत्त रिक्त;
	}
	((type_T **)tuple_types_ga.ga_data)[tuple_types_ga.ga_len] = type;
	tuple_types_ga.ga_len++;
    }

    type_T *tuple_type = get_tuple_type(&tuple_types_ga, type_gap);
    ga_clear(&tuple_types_ga);

    // Reset copyID so that a shared reference to this tuple (not a circular
    // reference) can be processed again to get the correct type.
    tuple->tv_copyID = 0;

    पटिनिवत्त tuple_type;
}

/*
 * Get a type_T for a Dict typval in "tv".
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    स्थिर type_T *
dict_typval2type(typval_T *tv, पूर्णाङ्क copyID, garray_T *type_gap, पूर्णाङ्क flags)
{
    dict_iterator_T	iter;
    typval_T		*मानम्;
    dict_T		*d = tv->vval.v_dict;
    type_T		*member_type = रिक्त;

    सचे (d == रिक्त || (d->dv_hashtab.ht_used == 0 && d->dv_type == रिक्त))
	पटिनिवत्त &t_dict_empty;

    सचे ((flags & TVTT_DO_MEMBER) == 0)
	पटिनिवत्त &t_dict_any;

    // If the type is dict<any> go through the members, it may end up a
    // more specific type.
    सचे (d->dv_type != रिक्त && (d->dv_hashtab.ht_used == 0
		|| (flags & TVTT_MORE_SPECIFIC) == 0
		|| d->dv_type->tt_member != &t_any))
	पटिनिवत्त d->dv_type;

    सचे (d->dv_copyID == copyID)
	// avoid recursion
	पटिनिवत्त &t_dict_any;

    d->dv_copyID = copyID;

    // Use the common type of all values.
    dict_iterate_start(tv, &iter);
    dict_iterate_next(&iter, &मानम्);
    member_type = typval2type(मानम्, copyID, type_gap, TVTT_DO_MEMBER);

    याव (dict_iterate_next(&iter, &मानम्) != रिक्त)
	common_type(typval2type(मानम्, copyID, type_gap, TVTT_DO_MEMBER),
					member_type, &member_type, type_gap);

    // Reset copyID so that a shared reference to this dict (not a circular
    // reference) can be processed again to get the correct type.
    d->dv_copyID = 0;

    पटिनिवत्त get_dict_type(member_type, type_gap);
}

/*
 * Get a type_T for a "partial" typval in "tv".
 */
    स्थिर type_T *
partial_typval2type(typval_T *tv, ufunc_T *ufunc, garray_T *type_gap)
{
    partial_T *pt = tv->vval.v_partial;
    type_T  *type;

    type = get_type_ptr(type_gap);
    सचे (type == रिक्त)
	पटिनिवत्त रिक्त;

    *type = *ufunc->uf_func_type;
    सचे (type->tt_argcount >= 0 && pt->pt_argc > 0)
    {
	type->tt_argcount -= pt->pt_argc;
	type->tt_min_argcount -= pt->pt_argc;
	सचे (type->tt_argcount > 0 && func_type_add_arg_types(type,
		    type->tt_argcount, type_gap) == OK)
	    अत्थाय (पूर्णाङ्क i = 0; i < type->tt_argcount; ++i)
		type->tt_args[i] =
		    ufunc->uf_func_type->tt_args[i + pt->pt_argc];
    }
    सचे (pt->pt_func != रिक्त)
	type->tt_member = pt->pt_func->uf_ret_type;

    पटिनिवत्त type;
}

/*
 * Get a type_T for a "class" or an "object" typval in "tv".
 */
    स्थिर type_T *
oc_typval2type(typval_T *tv)
{
    सचे (tv->v_type == VAR_CLASS)
    {
	सचे (tv->vval.v_class == रिक्त)
	    पटिनिवत्त &t_class;

	पटिनिवत्त &tv->vval.v_class->class_type;
    }

    सचे (tv->vval.v_object != रिक्त)
	पटिनिवत्त &tv->vval.v_object->obj_class->class_object_type;

    पटिनिवत्त &t_object_any;
}

/*
 * Get a type_T for a "function" or a "partial"
 */
    स्थिर type_T *
fp_typval2type(typval_T *tv, garray_T *type_gap)
{
    char_u	*name = रिक्त;
    ufunc_T	*ufunc = रिक्त;
    type_T	*type;
    type_T	*member_type = रिक्त;
    पूर्णाङ्क		argcount = 0;
    पूर्णाङ्क		min_argcount = 0;

    सचे (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != रिक्त)
    {
	सचे (tv->vval.v_partial->pt_func != रिक्त)
	    ufunc = tv->vval.v_partial->pt_func;
	अञ्ञथा
	    name = tv->vval.v_partial->pt_name;
    }
    अञ्ञथा
	name = tv->vval.v_string;

    सचे (name == रिक्त && ufunc == रिक्त)
	पटिनिवत्त &t_func_unknown;

    सचे (name != रिक्त)
    {
	पूर्णाङ्क idx = find_internal_func(name);

	सचे (idx >= 0)
	{
	    type_T *decl_type;  // unused

	    internal_func_get_argcount(idx, &argcount, &min_argcount);
	    member_type = internal_func_ret_type(idx, 0, रिक्त, &decl_type,
		    type_gap);
	}
	अञ्ञथा
	{
	    // Check if name contains "<".  If it does, then replace "<" with
	    // NUL and lookup the function and then look up the specific
	    // generic function using the supplied types.
	    char_u *p = generic_func_find_open_bracket(name);
	    सचे (p == रिक्त)
		ufunc = find_func(name, FALSE);
	    अञ्ञथा
	    {
		// generic function
		char_u	cc = *p;

		*p = NUL;
		ufunc = find_func(name, FALSE);
		*p = cc;
		सचे (ufunc != रिक्त && IS_GENERIC_FUNC(ufunc))
		{
		    ufunc = find_generic_func(ufunc, name, &p);
		    सचे (ufunc == रिक्त)
			पटिनिवत्त रिक्त;
		}
	    }
	}
    }
    सचे (ufunc != रिक्त)
    {
	// May need to get the argument types from default values by
	// compiling the function.
	सचे (ufunc->uf_def_status == UF_TO_BE_COMPILED
		&& compile_def_function(ufunc, TRUE, CT_NONE, रिक्त)
		== FAIL)
	    पटिनिवत्त रिक्त;
	सचे (ufunc->uf_func_type == रिक्त)
	    set_function_type(ufunc);
	सचे (ufunc->uf_func_type != रिक्त)
	{
	    सचे (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != रिक्त)
		पटिनिवत्त partial_typval2type(tv, ufunc, type_gap);
	    पटिनिवत्त ufunc->uf_func_type;
	}
    }

    type = get_type_ptr(type_gap);
    सचे (type == रिक्त)
	पटिनिवत्त रिक्त;
    type->tt_type = tv->v_type;
    type->tt_argcount = argcount;
    type->tt_min_argcount = min_argcount;
    सचे (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != रिक्त
					&& tv->vval.v_partial->pt_argc > 0)
    {
	type->tt_argcount -= tv->vval.v_partial->pt_argc;
	type->tt_min_argcount -= tv->vval.v_partial->pt_argc;
    }
    type->tt_member = member_type;

    पटिनिवत्त type;
}

/*
 * Get a type_T for a typval_T.
 * "type_gap" is used to temporarily create types in.
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    स्थिर type_T *
typval2type_int(typval_T *tv, पूर्णाङ्क copyID, garray_T *type_gap, पूर्णाङ्क flags)
{
    विकल्प (tv->v_type)
    {
	पक्ष VAR_UNKNOWN:
	    पटिनिवत्त &t_unknown;

	पक्ष VAR_ANY:
	    पटिनिवत्त &t_any;

	पक्ष VAR_VOID:
	    पटिनिवत्त &t_void;

	पक्ष VAR_BOOL:
	    पटिनिवत्त &t_bool;

	पक्ष VAR_SPECIAL:
	    पटिनिवत्त special_typval2type(tv);

	पक्ष VAR_सङ्ख्या:
	    पटिनिवत्त &t_सङ्ख्या;

	पक्ष VAR_FLOAT:
	    पटिनिवत्त &t_float;

	पक्ष VAR_STRING:
	    पटिनिवत्त &t_string;

	पक्ष VAR_BLOB:
	    सचे (tv->vval.v_blob == रिक्त)
		पटिनिवत्त &t_blob_null;
	    पटिनिवत्त &t_blob;

	पक्ष VAR_LIST:
	    पटिनिवत्त list_typval2type(tv, copyID, type_gap, flags);

	पक्ष VAR_TUPLE:
	    पटिनिवत्त tuple_typval2type(tv, copyID, type_gap, flags);

	पक्ष VAR_DICT:
	    पटिनिवत्त dict_typval2type(tv, copyID, type_gap, flags);

	पक्ष VAR_JOB:
	    पटिनिवत्त &t_job;

	पक्ष VAR_CHANNEL:
	    पटिनिवत्त &t_channel;

	पक्ष VAR_CLASS:
	पक्ष VAR_OBJECT:
	    पटिनिवत्त oc_typval2type(tv);

	पक्ष VAR_TYPEALIAS:
	    पटिनिवत्त &t_typealias;

	पक्ष VAR_FUNC:
	पक्ष VAR_PARTIAL:
	    पटिनिवत्त fp_typval2type(tv, type_gap);

	पक्ष VAR_INSTR:
	स्वभाव:
	    भङ्ग;
    }

    पटिनिवत्त रिक्त;
}

/*
 * Return TRUE if "tv" is not a bool but should be converted to bool.
 */
    पूर्णाङ्क
need_convert_to_bool(type_T *type, typval_T *tv)
{
    पटिनिवत्त type != रिक्त && type == &t_bool && tv->v_type != VAR_BOOL
	    && (tv->v_type == VAR_सङ्ख्या
		       && (tv->vval.v_सङ्ख्या == 0 || tv->vval.v_सङ्ख्या == 1));
}

/*
 * Get a type_T for a typval_T.
 * "type_list" is used to temporarily create types in.
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the most specific member type.
 */
    type_T *
typval2type(typval_T *tv, पूर्णाङ्क copyID, garray_T *type_gap, पूर्णाङ्क flags)
{
    type_T *type = typval2type_int(tv, copyID, type_gap, flags);

    सचे (type == रिक्त)
	पटिनिवत्त रिक्त;

    सचे (type != &t_bool && (tv->v_type == VAR_सङ्ख्या
		&& (tv->vval.v_सङ्ख्या == 0 || tv->vval.v_सङ्ख्या == 1)))
	// Number 0 and 1 and expression with "&&" or "||" can also be used
	// for bool.
	type = &t_सङ्ख्या_bool;
    अञ्ञथा सचे (type != &t_float && tv->v_type == VAR_सङ्ख्या)
	// A number can also be used for float.
	type = &t_सङ्ख्या_float;
    पटिनिवत्त type;
}

/*
 * Return TRUE if "type" can be used for a variable declaration.
 * Give an error and return FALSE if not.
 */
    पूर्णाङ्क
valid_declaration_type(type_T *type)
{
    सचे (type->tt_type == VAR_SPECIAL  // null, none
	    || type->tt_type == VAR_VOID)
    {
	वर्ण *tofree = रिक्त;
	वर्ण *name = type_name(type, &tofree);
	semsg(_(e_invalid_type_in_variable_declaration_str), name);
	vim_free(tofree);
	पटिनिवत्त FALSE;
    }
    पटिनिवत्त TRUE;
}

/*
 * Get a type_T for a typval_T, used for v: variables.
 * "type_list" is used to temporarily create types in.
 */
    type_T *
typval2type_vimvar(typval_T *tv, garray_T *type_gap)
{
    सचे (tv->v_type == VAR_LIST)  // e.g. for v:oldfiles
	पटिनिवत्त &t_list_string;
    सचे (tv->v_type == VAR_DICT)  // e.g. for v:event
	पटिनिवत्त &t_dict_any;
    पटिनिवत्त typval2type(tv, get_copyID(), type_gap, TVTT_DO_MEMBER);
}

    पूर्णाङ्क
check_typval_arg_type(
	type_T	    *expected,
	typval_T    *actual_tv,
	वर्ण	    *func_name,
	पूर्णाङ्क	    arg_idx)
{
    where_T	where = WHERE_INIT;

    सचे (arg_idx > 0)
    {
	where.wt_index = arg_idx;
	where.wt_kind = WT_ARGUMENT;
    }
    where.wt_func_name = func_name;
    पटिनिवत्त check_typval_type(expected, actual_tv, where);
}

/*
 * Return true if "expected" is list<any> or dict<any> and "actual_tv" is a
 * list or dict of that kind, which matches whatever its values are.
 */
    स्थिर bool
matches_listdict_any(type_T *expected, typval_T *actual_tv)
{
    पटिनिवत्त (expected->tt_type == VAR_LIST || expected->tt_type == VAR_DICT)
	    && actual_tv->v_type == expected->tt_type
	    && expected->tt_member != रिक्त
	    && (expected->tt_member->tt_type == VAR_ANY
		|| expected->tt_member->tt_type == VAR_UNKNOWN);
}

/*
 * Return FAIL if "expected" and "actual" don't match.
 * When "argidx" > 0 it is included in the error message.
 */
    पूर्णाङ्क
check_typval_type(type_T *expected, typval_T *actual_tv, where_T where)
{
    garray_T	type_list;
    type_T	*actual_type;
    पूर्णाङ्क		res = FAIL;

    सचे (expected == रिक्त)
	पटिनिवत्त OK;  // didn't expect anything.

    // No need to go through the values of a possibly big list or dict.
    सचे (matches_listdict_any(expected, actual_tv))
	पटिनिवत्त OK;

    ga_init2(&type_list, परिमाण(type_T *), 10);

    // A null_function and null_partial are special cases, they can be used to
    // clear a variable.
    सचे ((actual_tv->v_type == VAR_FUNC && actual_tv->vval.v_string == रिक्त)
	    || (actual_tv->v_type == VAR_PARTIAL
					 && actual_tv->vval.v_partial == रिक्त))
	actual_type = &t_func_unknown;
    अञ्ञथा
	// When the actual type is list<any> or dict<any> go through the values
	// to possibly get a more specific type.
	actual_type = typval2type(actual_tv, get_copyID(), &type_list,
					  TVTT_DO_MEMBER | TVTT_MORE_SPECIFIC);
    सचे (actual_type != रिक्त)
    {
	res = check_type_maybe(expected, actual_type, TRUE, where);
	सचे (res == MAYBE && !(actual_type->tt_type == VAR_FUNC
				      && (actual_type->tt_member == &t_unknown
					  || actual_type->tt_member == रिक्त)))
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
    पटिनिवत्त res;
}

    शून्य
arg_type_mismatch(type_T *expected, type_T *actual, पूर्णाङ्क arg_idx)
{
    where_T	where = WHERE_INIT;

    सचे (arg_idx > 0)
    {
	where.wt_index = arg_idx;
	where.wt_kind = WT_ARGUMENT;
    }
    type_mismatch_where(expected, actual, where);
}

    शून्य
type_mismatch_where(type_T *expected, type_T *actual, where_T where)
{
    वर्ण *tofree1, *tofree2;
    वर्ण *typename1 = type_name(expected, &tofree1);
    वर्ण *typename2 = type_name(actual, &tofree2);

    विकल्प (where.wt_kind)
    {
	पक्ष WT_MEMBER:
	    semsg(_(e_variable_str_type_mismatch_expected_str_but_got_str),
		    where.wt_func_name, typename1, typename2);
	    भङ्ग;
	पक्ष WT_METHOD:
	पक्ष WT_METHOD_ARG:
	पक्ष WT_METHOD_RETURN:
	    semsg(_(e_method_str_type_mismatch_expected_str_but_got_str),
		    where.wt_func_name, typename1, typename2);
	    भङ्ग;
	पक्ष WT_VARIABLE:
	    सचे (where.wt_func_name == रिक्त)
		semsg(_(e_variable_nr_type_mismatch_expected_str_but_got_str),
			where.wt_index, typename1, typename2);
	    अञ्ञथा
		semsg(_(e_variable_nr_type_mismatch_expected_str_but_got_str_in_str),
			where.wt_index, typename1, typename2, where.wt_func_name);
	    भङ्ग;
	पक्ष WT_ARGUMENT:
	    सचे (where.wt_func_name == रिक्त)
		semsg(_(e_argument_nr_type_mismatch_expected_str_but_got_str),
			where.wt_index, typename1, typename2);
	    अञ्ञथा
		semsg(_(e_argument_nr_type_mismatch_expected_str_but_got_str_in_str),
			where.wt_index, typename1, typename2, where.wt_func_name);
	    भङ्ग;
	पक्ष WT_CAST:
	पक्ष WT_UNKNOWN:
	    सचे (where.wt_func_name == रिक्त)
		semsg(_(e_type_mismatch_expected_str_but_got_str),
			typename1, typename2);
	    अञ्ञथा
		semsg(_(e_type_mismatch_expected_str_but_got_str_in_str),
			typename1, typename2, where.wt_func_name);
	    भङ्ग;
    }

    vim_free(tofree1);
    vim_free(tofree2);
}

/*
 * Check if the expected and actual types match for a tuple
 */
    स्थिर पूर्णाङ्क
check_tuple_type_maybe(
    type_T	*expected,
    type_T	*actual,
    where_T	where)
{
    सचे (expected->tt_argcount == -1 || actual->tt_argcount == -1
		|| expected->tt_args == रिक्त || actual->tt_args == रिक्त)
	पटिनिवत्त OK;

    // For a non-variadic tuple, the number of items must match
    सचे (!(expected->tt_flags & TTFLAG_VARARGS)
	    && expected->tt_argcount != actual->tt_argcount)
	पटिनिवत्त FAIL;

    // compare the type of each tuple item
    अत्थाय (पूर्णाङ्क i = 0; i < actual->tt_argcount; ++i)
    {
	type_T	*exp_type;
	type_T	*actual_type;

	सचे (expected->tt_flags & TTFLAG_VARARGS)
	{
	    सचे (i < expected->tt_argcount - 1)
		exp_type = expected->tt_args[i];
	    अञ्ञथा
		// For a variadic tuple, the last type is a List.  So use the
		// List member type.
		exp_type = expected->tt_args[expected->tt_argcount - 1]->tt_member;
	}
	अञ्ञथा
	    exp_type = expected->tt_args[i];

	सचे (actual->tt_flags & TTFLAG_VARARGS)
	{
	    सचे (i < actual->tt_argcount - 1)
		actual_type = actual->tt_args[i];
	    अञ्ञथा
		// For a variadic tuple, the last type is a List.  So use the
		// List member type.
		actual_type = actual->tt_args[actual->tt_argcount - 1]->tt_member;
	}
	अञ्ञथा
	    actual_type = actual->tt_args[i];

	// Allow for using "any" type for a tuple item
	सचे (actual->tt_args[i] != &t_any && check_type(exp_type, actual_type,
						FALSE, where) == FAIL)
	    पटिनिवत्त FAIL;
    }

    पटिनिवत्त OK;
}

/*
 * Check if the expected and actual types match for a function
 * Returns OK if "expected" and "actual" are matching function types.
 * Returns FAIL if "expected" and "actual" are different types.
 * Returns MAYBE when a runtime type check is needed.
 */
    स्थिर पूर्णाङ्क
check_func_type_maybe(
    type_T	*expected,
    type_T	*actual,
    where_T	where)
{
    पूर्णाङ्क ret = OK;

    // If the return type is unknown it can be anything, including
    // nothing, thus there is no point in checking.
    सचे (expected->tt_member != &t_unknown)
    {
	सचे (actual->tt_member != रिक्त
		&& actual->tt_member != &t_unknown)
	{
	    where_T  func_where = where;

	    func_where.wt_kind = WT_METHOD_RETURN;
	    ret = check_type_maybe(expected->tt_member,
		    actual->tt_member, FALSE,
		    func_where);
	}
	अञ्ञथा
	    ret = MAYBE;
    }
    सचे (ret != FAIL
	    && ((expected->tt_flags & TTFLAG_VARARGS)
		!= (actual->tt_flags & TTFLAG_VARARGS))
	    && expected->tt_argcount != -1)
	ret = FAIL;
    सचे (ret != FAIL && expected->tt_argcount != -1
	    && actual->tt_min_argcount != -1
	    && (actual->tt_argcount == -1
		|| (actual->tt_argcount < expected->tt_min_argcount
		    || actual->tt_argcount > expected->tt_argcount)))
	ret = FAIL;
    सचे (ret != FAIL && expected->tt_args != रिक्त
	    && actual->tt_args != रिक्त)
    {
	पूर्णाङ्क i;

	अत्थाय (i = 0; i < expected->tt_argcount
		&& i < actual->tt_argcount; ++i)
	{
	    where_T  func_where = where;
	    func_where.wt_kind = WT_METHOD_ARG;

	    // Allow for using "any" argument type, lambda's have them.
	    सचे (actual->tt_args[i] != &t_any && check_type(
			expected->tt_args[i], actual->tt_args[i], FALSE,
			func_where) == FAIL)
	    {
		ret = FAIL;
		भङ्ग;
	    }
	}
    }
    सचे (ret == OK && expected->tt_argcount >= 0
	    && actual->tt_argcount == -1)
	// check the argument count at runtime
	ret = MAYBE;

    पटिनिवत्त ret;
}

/*
 * Check if the expected and actual types match for an object
 * Returns OK if "expected" and "actual" are matching object types.
 * Returns FAIL if "expected" and "actual" are different types.
 * Returns MAYBE when a runtime type check is needed.
 */
    स्थिर पूर्णाङ्क
check_object_type_maybe(
    type_T	*expected,
    type_T	*actual,
    where_T	where)
{
    पूर्णाङ्क ret = OK;

    सचे (actual->tt_type == VAR_ANY)
	पटिनिवत्त MAYBE;	// use runtime type check
    सचे (actual->tt_type != VAR_OBJECT)
	पटिनिवत्त FAIL;	// don't use tt_class
    सचे (actual->tt_class == रिक्त)    // null object
	पटिनिवत्त OK;
    // t_object_any matches any object except for an enum item
    सचे (expected == &t_object_any && !IS_ENUM(actual->tt_class))
	पटिनिवत्त OK;

    // For object method arguments, do a invariant type check in
    // an extended class.  For all others, do a covariance type check.
    सचे (where.wt_kind == WT_METHOD_ARG)
    {
	सचे (actual->tt_class != expected->tt_class)
	    ret = FAIL;
    }
    अञ्ञथा सचे (!class_instance_of(actual->tt_class, expected->tt_class))
    {
	// Check if this is an up-cast, if so we'll have to check the type at
	// runtime.
	सचे (where.wt_kind == WT_CAST &&
		class_instance_of(expected->tt_class, actual->tt_class))
	    ret = MAYBE;
	अञ्ञथा
	    ret = FAIL;
    }

    पटिनिवत्त ret;
}

/*
 * Check if the expected and actual types match.
 * Does not allow for assigning "any" to a specific type.
 * When "argidx" > 0 it is included in the error message.
 * Return OK if types match.
 * Return FAIL if types do not match.
 */
    पूर्णाङ्क
check_type(
	type_T	*expected,
	type_T	*actual,
	पूर्णाङ्क	give_सन्देशः,
	where_T where)
{
    पूर्णाङ्क ret = check_type_maybe(expected, actual, give_सन्देशः, where);

    पटिनिवत्त ret == MAYBE ? OK : ret;
}

/*
 * As check_type() but return MAYBE when a runtime type check should be used
 * when compiling.
 */
    पूर्णाङ्क
check_type_maybe(
	type_T	*expected,
	type_T	*actual,
	पूर्णाङ्क	give_सन्देशः,
	where_T where)
{
    पूर्णाङ्क ret = OK;

    // When expected is "unknown" we accept any actual type.
    // When expected is "any" we accept any actual type except "void".
    सचे (expected->tt_type != VAR_UNKNOWN
	    && !(expected->tt_type == VAR_ANY && actual->tt_type != VAR_VOID))

    {
	// tt_type should match, except that a "partial" can be assigned to a
	// variable with type "func".
	// And "unknown" (using global variable) and "any" need a runtime type
	// check.
	सचे (!(expected->tt_type == actual->tt_type
		    || actual->tt_type == VAR_UNKNOWN
		    || actual->tt_type == VAR_ANY
		    || (expected->tt_type == VAR_FUNC
					   && actual->tt_type == VAR_PARTIAL)))
	{
	    सचे (expected->tt_type == VAR_BOOL
					&& (actual->tt_flags & TTFLAG_BOOL_OK))
		// Using number 0 or 1 for bool is OK.
		पटिनिवत्त OK;
	    सचे (expected->tt_type == VAR_FLOAT
		    && actual->tt_type == VAR_सङ्ख्या
		    && ((expected->tt_flags & TTFLAG_सङ्ख्या_OK)
			     || (actual->tt_flags & TTFLAG_FLOAT_OK)))
		// Using a number where a float is expected is OK here.
		पटिनिवत्त OK;
	    सचे (expected->tt_type == VAR_LIST
		    && actual->tt_type == VAR_TUPLE
		    && (expected->tt_flags & TTFLAG_TUPLE_OK))
		// Using a tuple where a list is expected is OK here.
		पटिनिवत्त OK;
	    सचे (give_सन्देशः)
		type_mismatch_where(expected, actual, where);
	    पटिनिवत्त FAIL;
	}
	सचे (expected->tt_type == VAR_DICT || expected->tt_type == VAR_LIST)
	{
	    // "unknown" is used for an empty list or dict
	    सचे (actual->tt_member != रिक्त && actual->tt_member != &t_unknown)
		ret = check_type_maybe(expected->tt_member, actual->tt_member,
								 FALSE, where);
	}
	अञ्ञथा सचे (expected->tt_type == VAR_TUPLE && actual != &t_any)
	    ret =  check_tuple_type_maybe(expected, actual, where);
	अञ्ञथा सचे (expected->tt_type == VAR_FUNC && actual != &t_any)
	    ret = check_func_type_maybe(expected, actual, where);
	अञ्ञथा सचे (expected->tt_type == VAR_OBJECT)
	    ret = check_object_type_maybe(expected, actual, where);

	सचे (ret == FAIL && give_सन्देशः)
	    type_mismatch_where(expected, actual, where);
    }

    सचे (ret == OK && expected->tt_type != VAR_UNKNOWN
	    && expected->tt_type != VAR_ANY
	    && (actual->tt_type == VAR_UNKNOWN || actual->tt_type == VAR_ANY))
	// check the type at runtime
	ret = MAYBE;

    पटिनिवत्त ret;
}

/*
 * Check that the arguments of "type" match "argvars[argcount]".
 * "base_tv" is from "expr->Func()".
 * Return OK/FAIL.
 */
    पूर्णाङ्क
check_argument_types(
	type_T	    *type,
	typval_T    *argvars,
	पूर्णाङ्क	    argcount,
	typval_T    *base_tv,
	char_u	    *name)
{
    पूर्णाङ्क	    varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;
    पूर्णाङ्क	    i;
    पूर्णाङ्क	    totcount = argcount + (base_tv == रिक्त ? 0 : 1);

    सचे (type->tt_type != VAR_FUNC && type->tt_type != VAR_PARTIAL)
	पटिनिवत्त OK;  // just in case
    सचे (totcount < type->tt_min_argcount - varargs)
    {
	emsg_funcname(e_not_enough_arguments_for_function_str, name);
	पटिनिवत्त FAIL;
    }
    सचे (!varargs && type->tt_argcount >= 0 && totcount > type->tt_argcount)
    {
	emsg_funcname(e_too_many_arguments_for_function_str, name);
	पटिनिवत्त FAIL;
    }
    सचे (type->tt_args == रिक्त)
	पटिनिवत्त OK;  // cannot check


    अत्थाय (i = 0; i < totcount; ++i)
    {
	type_T	    *expected;
	typval_T    *tv;

	सचे (base_tv != रिक्त)
	{
	    सचे (i == 0)
		tv = base_tv;
	    अञ्ञथा
		tv = &argvars[i - 1];
	}
	अञ्ञथा
	    tv = &argvars[i];
	सचे (varargs && i >= type->tt_argcount - 1)
	{
	    expected = type->tt_args[type->tt_argcount - 1];
	    सचे (expected != रिक्त && expected->tt_type == VAR_LIST)
		expected = expected->tt_member;
	    सचे (expected == रिक्त)
		expected = &t_any;
	}
	अञ्ञथा
	    expected = type->tt_args[i];

	// check the type, unless the value is v:none
	सचे ((tv->v_type != VAR_SPECIAL || tv->vval.v_सङ्ख्या != VVAL_NONE)
		   && check_typval_arg_type(expected, tv, रिक्त, i + 1) == FAIL)
	    पटिनिवत्त FAIL;
    }
    पटिनिवत्त OK;
}

// Maximum nesting of types, to avoid running out of stack space.  E.g.
// "list<list<number>>" is nested twice.
#define MAX_TYPE_DEPTH	1000

// Current number of levels of parse_type() and the deepest level reached while
// parsing the current type.  The nesting is one less than the number of
// levels.
स्थिर पूर्णाङ्क	parse_depth = 0;
स्थिर पूर्णाङ्क	parse_max_depth = 0;

/*
 * Skip over type in list<type>, dict<type> or tuple<type>.
 * Returns a pointer to the character after the type.  "syn_error" is set to
 * TRUE on syntax error.
 */
    स्थिर char_u *
skip_member_type(char_u *start, char_u *p, पूर्णाङ्क *syn_error)
{
    सचे (STRNCMP("tuple", start, 5) == 0)
    {
	// handle tuple<{type1}, {type2}, ....<type>>
	p = skipwhite(p + 1);
	याव (*p != '>' && *p != NUL)
	{
	    char_u *sp = p;

	    सचे (STRNCMP(p, "...", 3) == 0)
		p += 3;
	    p = skip_type(p, TRUE);
	    सचे (p == sp)
	    {
		*syn_error = TRUE;
		पटिनिवत्त p;  // syntax error
	    }
	    सचे (*p == ',')
		p = skipwhite(p + 1);
	}
	सचे (*p == '>')
	    p++;
    }
    अञ्ञथा
    {
	p = skipwhite(p);
	p = skip_type(skipwhite(p + 1), FALSE);
	p = skipwhite(p);
	सचे (*p == '>')
	    ++p;
    }

    पटिनिवत्त p;
}

/*
 * Skip over a function type.  Returns a pointer to the character after the
 * type.  "syn_error" is set to TRUE on syntax error.
 */
    स्थिर char_u *
skip_func_type(char_u *p, पूर्णाङ्क *syn_error)
{
    सचे (*p == '(')
    {
	// handle func(args): type
	++p;
	याव (*p != ')' && *p != NUL)
	{
	    char_u *sp = p;

	    सचे (STRNCMP(p, "...", 3) == 0)
		p += 3;
	    p = skip_type(p, TRUE);
	    सचे (p == sp)
	    {
		*syn_error = TRUE;
		पटिनिवत्त p;  // syntax error
	    }
	    सचे (*p == ',')
		p = skipwhite(p + 1);
	}
	सचे (*p == ')')
	{
	    सचे (p[1] == ':')
		p = skip_type(skipwhite(p + 2), FALSE);
	    अञ्ञथा
		++p;
	}
    }
    अञ्ञथा
    {
	// handle func: return_type
	p = skip_type(skipwhite(p + 1), FALSE);
    }

    पटिनिवत्त p;
}

/*
 * Inner part of skip_type().
 */
    स्थिर char_u *
skip_type_inner(char_u *start, पूर्णाङ्क optional)
{
    char_u	*p = start;
    पूर्णाङ्क		syn_error = FALSE;

    सचे (optional && *p == '?')
	++p;

    // Also skip over "." for imported classes: "import.ClassName".
    याव (ASCII_ISALNUM(*p) || *p == '_' || *p == '.')
	++p;

    // Skip over "<type>"; this is permissive about white space.
    सचे (*skipwhite(p) == '<')
    {
	p = skip_member_type(start, p, &syn_error);
	सचे (syn_error)
	    पटिनिवत्त p;
    }
    अञ्ञथा सचे ((*p == '(' || (*p == ':' && VIM_ISWHITE(p[1])))
					     && STRNCMP("func", start, 4) == 0)
    {
	// skip over function type
	p = skip_func_type(p, &syn_error);
	सचे (syn_error)
	    पटिनिवत्त p;
    }

    पटिनिवत्त p;
}

/*
 * Skip over a type definition and return a pointer to just after it.
 * When "optional" is TRUE then a leading "?" is accepted.
 */
    char_u *
skip_type(char_u *start, पूर्णाङ्क optional)
{
    स्थिर पूर्णाङ्क	skip_depth = 0;
    char_u	*p = start;

    // Too deeply nested: skip to the matching ">" without recursing, an
    // error is given when parsing the type.
    सचे (skip_depth > MAX_TYPE_DEPTH)
    {
	पूर्णाङ्क	depth = 0;

	अत्थाय ( ; *p != NUL; ++p)
	{
	    सचे (*p == '<')
		++depth;
	    अञ्ञथा सचे (*p == '>')
	    {
		सचे (depth == 0)
		    भङ्ग;
		सचे (--depth == 0)
		    पटिनिवत्त p + 1;
	    }
	    अञ्ञथा सचे (depth == 0 && !ASCII_ISALNUM(*p) && *p != '_'
								&& *p != '.')
		भङ्ग;
	}
	पटिनिवत्त p;
    }

    ++skip_depth;
    p = skip_type_inner(start, optional);
    --skip_depth;
    पटिनिवत्त p;
}

/*
 * Parse the member type: "<type>" and return "type" with the member set.
 * Use "type_gap" if a new type needs to be added.
 * "info" is extra information for an error message.
 * Returns NULL in case of failure.
 */
    स्थिर type_T *
parse_type_member(
	char_u	    **arg,
	type_T	    *type,
	garray_T    *type_gap,
	पूर्णाङ्क	    give_error,
	वर्ण	    *info,
	ufunc_T	    *ufunc,
	cctx_T	    *cctx)
{
    char_u  *arg_start = *arg;
    type_T  *member_type;
    पूर्णाङ्क	    prev_called_emsg = called_emsg;

    सचे (**arg != '<')
    {
	सचे (give_error)
	{
	    सचे (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    अञ्ञथा
		semsg(_(e_missing_type_after_str), info);
	}
	पटिनिवत्त रिक्त;
    }
    *arg = skipwhite(*arg + 1);

    member_type = parse_type(arg, type_gap, ufunc, cctx, give_error);
    सचे (member_type == रिक्त || !valid_declaration_type(member_type))
	पटिनिवत्त रिक्त;

    *arg = skipwhite(*arg);
    सचे (**arg != '>' && called_emsg == prev_called_emsg)
    {
	सचे (give_error)
	    semsg(_(e_missing_gt_after_type_str), arg_start);
	पटिनिवत्त रिक्त;
    }
    ++*arg;

    सचे (type->tt_type == VAR_LIST)
	पटिनिवत्त get_list_type(member_type, type_gap);
    पटिनिवत्त get_dict_type(member_type, type_gap);
}

/*
 * Parse a "func" type at "*arg" and advance over it.
 * When "give_error" is TRUE give error messages, otherwise be quiet.
 * Return NULL for failure.
 */
    स्थिर type_T *
parse_type_func(
    char_u	**arg,
    size_t	len,
    garray_T	*type_gap,
    पूर्णाङ्क		give_error,
    ufunc_T	*ufunc,
    cctx_T	*cctx)
{
    char_u  *p;
    type_T  *type;
    type_T  *ret_type = &t_unknown;
    पूर्णाङ्क	argcount = -1;
    पूर्णाङ्क	flags = 0;
    पूर्णाङ्क	first_optional = -1;
    type_T	*arg_type[MAX_FUNC_ARGS + 1];

    // func({type}, ...{type}): {type}
    *arg += len;
    सचे (**arg == '(')
    {
	// "func" may or may not return a value, "func()" does
	// not return a value.
	ret_type = &t_void;

	p = ++*arg;
	argcount = 0;
	याव (*p != NUL && *p != ')')
	{
	    सचे (*p == '?')
	    {
		सचे (first_optional == -1)
		    first_optional = argcount;
		++p;
	    }
	    अञ्ञथा सचे (STRNCMP(p, "...", 3) == 0)
	    {
		flags |= TTFLAG_VARARGS;
		p += 3;
	    }
	    अञ्ञथा सचे (first_optional != -1)
	    {
		सचे (give_error)
		    emsg(_(e_mandatory_argument_after_optional_argument));
		पटिनिवत्त रिक्त;
	    }

	    type = parse_type(&p, type_gap, ufunc, cctx, give_error);
	    सचे (type == रिक्त || !valid_declaration_type(type))
		पटिनिवत्त रिक्त;
	    सचे ((flags & TTFLAG_VARARGS) != 0 && type->tt_type != VAR_LIST)
	    {
		वर्ण *tofree;
		semsg(_(e_variable_arguments_type_must_be_list_str),
			type_name(type, &tofree));
		vim_free(tofree);
		पटिनिवत्त रिक्त;
	    }
	    arg_type[argcount++] = type;

	    // Nothing comes after "...{type}".
	    सचे (flags & TTFLAG_VARARGS)
		भङ्ग;

	    सचे (*p != ',' && *skipwhite(p) == ',')
	    {
		सचे (give_error)
		    semsg(_(e_no_white_space_allowed_before_str_str),
			    ",", p);
		पटिनिवत्त रिक्त;
	    }
	    सचे (*p == ',')
	    {
		++p;
		सचे (!VIM_ISWHITE(*p))
		{
		    सचे (give_error)
			semsg(_(e_white_space_required_after_str_str),
				",", p - 1);
		    पटिनिवत्त रिक्त;
		}
	    }
	    p = skipwhite(p);
	    सचे (argcount == MAX_FUNC_ARGS)
	    {
		सचे (give_error)
		    emsg(_(e_too_many_argument_types));
		पटिनिवत्त रिक्त;
	    }
	}

	p = skipwhite(p);
	सचे (*p != ')')
	{
	    सचे (give_error)
		emsg(_(e_missing_closing_paren));
	    पटिनिवत्त रिक्त;
	}
	*arg = p + 1;
    }
    सचे (**arg == ':')
    {
	// parse return type
	++*arg;
	सचे (!VIM_ISWHITE(**arg) && give_error)
	    semsg(_(e_white_space_required_after_str_str), ":", *arg - 1);
	*arg = skipwhite(*arg);
	ret_type = parse_type(arg, type_gap, ufunc, cctx, give_error);
	सचे (ret_type == रिक्त)
	    पटिनिवत्त रिक्त;
    }
    सचे (flags == 0 && first_optional == -1 && argcount <= 0)
	type = get_func_type(ret_type, argcount, type_gap);
    अञ्ञथा
    {
	type = alloc_func_type(ret_type, argcount, type_gap);
	type->tt_flags = flags;
	सचे (argcount > 0)
	{
	    type->tt_argcount = argcount;
	    type->tt_min_argcount = first_optional == -1
						? argcount : first_optional;
	    सचे (func_type_add_arg_types(type, argcount, type_gap) == FAIL)
		पटिनिवत्त रिक्त;
	    mch_memmove(type->tt_args, arg_type, परिमाण(type_T *) * argcount);
	}
    }

    पटिनिवत्त type;
}

/*
 * Parse a "tuple" type at "*arg" and advance over it.
 * When "give_error" is TRUE give error messages, otherwise be quiet.
 * Return NULL for failure.
 */
    स्थिर type_T *
parse_type_tuple(
    char_u	**arg,
    garray_T	*type_gap,
    पूर्णाङ्क		give_error,
    ufunc_T	*ufunc,
    cctx_T	*cctx)
{
    char_u	*p;
    type_T	*type;
    type_T	*ret_type = रिक्त;
    पूर्णाङ्क		typecount = -1;
    पूर्णाङ्क		flags = 0;
    garray_T	tuple_types_ga;

    ga_init2(&tuple_types_ga, परिमाण(type_T *), 10);

    // tuple<{type}, {type}>
    // tuple<{type}, ...{type}>
    सचे (**arg != '<')
    {
	सचे (give_error)
	{
	    सचे (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    अञ्ञथा
		semsg(_(e_missing_type_after_str), "tuple");
	}

	// only "tuple" is specified
	पटिनिवत्त रिक्त;
    }

    p = ++*arg;
    typecount = 0;
    याव (*p != NUL && *p != '>')
    {
	सचे (STRNCMP(p, "...", 3) == 0)
	{
	    flags |= TTFLAG_VARARGS;
	    p += 3;
	}

	type = parse_type(&p, type_gap, ufunc, cctx, give_error);
	सचे (type == रिक्त || !valid_declaration_type(type))
	    गच्छ on_err;

	सचे ((flags & TTFLAG_VARARGS) != 0 && type->tt_type != VAR_LIST)
	{
	    वर्ण *tofree;
	    semsg(_(e_variadic_tuple_must_end_with_list_type_str),
						type_name(type, &tofree));
	    vim_free(tofree);
	    गच्छ on_err;
	}

	// Add the item type
	सचे (ga_grow(&tuple_types_ga, 1) == FAIL)
	    गच्छ on_err;
	((type_T **)tuple_types_ga.ga_data)[tuple_types_ga.ga_len] = type;
	tuple_types_ga.ga_len++;
	typecount++;

	// Nothing comes after "...{type}".
	सचे (flags & TTFLAG_VARARGS)
	    भङ्ग;

	सचे (*p != ',' && *skipwhite(p) == ',')
	{
	    सचे (give_error)
		semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
	    गच्छ on_err;
	}
	सचे (*p == ',')
	{
	    ++p;
	    सचे (!VIM_ISWHITE(*p))
	    {
		सचे (give_error)
		    semsg(_(e_white_space_required_after_str_str),
			    ",", p - 1);
		गच्छ on_err;
	    }
	}
	p = skipwhite(p);
    }

    p = skipwhite(p);
    सचे (*p != '>' || typecount <= 0)
    {
	सचे (give_error)
	    semsg(_(e_missing_type_after_str), p);
	गच्छ on_err;
    }
    *arg = p + 1;

    ret_type = alloc_tuple_type(typecount, type_gap);
    सचे (ret_type == &t_any
	    || tuple_type_add_types(ret_type, typecount, type_gap) == FAIL)
    {
	ret_type = रिक्त;
	गच्छ on_err;
    }
    ret_type->tt_flags = flags;
    ret_type->tt_argcount = typecount;
    mch_memmove(ret_type->tt_args, tuple_types_ga.ga_data,
						परिमाण(type_T *) * typecount);

on_err:
    ga_clear(&tuple_types_ga);

    पटिनिवत्त ret_type;
}

/*
 * Parse a "object" type at "*arg" and advance over it.
 * When "give_error" is TRUE give error messages, otherwise be quiet.
 * Return NULL for failure.
 */
    स्थिर type_T *
parse_type_object(
    char_u	**arg,
    garray_T	*type_gap,
    पूर्णाङ्क		give_error,
    cctx_T	*cctx)
{
    char_u	*arg_start = *arg;
    type_T	*object_type;
    पूर्णाङ्क		prev_called_emsg = called_emsg;

    // object<X> or object<any>
    सचे (**arg != '<')
    {
	सचे (give_error)
	{
	    सचे (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    अञ्ञथा
		semsg(_(e_missing_type_after_str), "object");
	}

	// only "object" is specified
	पटिनिवत्त रिक्त;
    }

    // skip spaces following "object<"
    *arg = skipwhite(*arg + 1);

    object_type = parse_type(arg, type_gap, रिक्त, cctx, give_error);
    सचे (object_type == रिक्त)
	पटिनिवत्त रिक्त;

    *arg = skipwhite(*arg);
    सचे (**arg != '>' && called_emsg == prev_called_emsg)
    {
	सचे (give_error)
	    semsg(_(e_missing_gt_after_type_str), arg_start);
	पटिनिवत्त रिक्त;
    }
    ++*arg;

    सचे (object_type->tt_type == VAR_ANY)
	पटिनिवत्त &t_object_any;

    सचे (object_type->tt_type != VAR_OBJECT)
    {
	// specified type is not a class
	सचे (give_error)
	    semsg(_(e_class_name_not_found_str), arg_start);
	पटिनिवत्त रिक्त;
    }

    पटिनिवत्त object_type;
}

/*
 * Parse a user defined type at "*arg" and advance over it.
 * It can be a class or an interface or a typealias name, possibly imported.
 * Return NULL if a type is not found.
 */
    स्थिर type_T *
parse_type_user_defined(
    char_u	**arg,
    size_t	len,
    garray_T	*type_gap,
    पूर्णाङ्क		give_error,
    ufunc_T	*ufunc,
    cctx_T	*cctx)
{
    पूर्णाङ्क		did_emsg_before = did_emsg;
    typval_T	tv;

    tv.v_type = VAR_UNKNOWN;
    सचे (eval_variable_import(*arg, &tv) == OK)
    {
	सचे (tv.v_type == VAR_CLASS && tv.vval.v_class != रिक्त)
	{
	    type_T *type = get_type_ptr(type_gap);
	    सचे (type != रिक्त)
	    {
		// Although the name is that of a class or interface, the type
		// uses will be an object.
		type->tt_type = VAR_OBJECT;
		type->tt_class = tv.vval.v_class;
		clear_tv(&tv);

		*arg += len;
		// Skip over ".ClassName".
		याव (ASCII_ISALNUM(**arg) || **arg == '_' || **arg == '.')
		    ++*arg;

		पटिनिवत्त type;
	    }
	}
	अञ्ञथा सचे (tv.v_type == VAR_TYPEALIAS)
	{
	    // user defined type
	    // The type alias may itself be a nested type, check the resulting
	    // nesting depth.
	    पूर्णाङ्क	depth = parse_depth - 1
				+ MAX(tv.vval.v_typealias->ta_depth, 1);
	    सचे (depth - 1 > MAX_TYPE_DEPTH)
	    {
		clear_tv(&tv);
		सचे (give_error)
		    emsg(_(e_type_nested_too_deep));
		पटिनिवत्त रिक्त;
	    }
	    सचे (depth > parse_max_depth)
		parse_max_depth = depth;

	    type_T *type = copy_type(tv.vval.v_typealias->ta_type, type_gap);
	    *arg += len;
	    clear_tv(&tv);
	    // Skip over ".TypeName".
	    याव (ASCII_ISALNUM(**arg) || **arg == '_' || **arg == '.')
		++*arg;
	    पटिनिवत्त type;
	}

	clear_tv(&tv);
    }

    // Check whether it is a generic type
    type_T *type = find_generic_type(*arg, len, ufunc, cctx);
    सचे (type != रिक्त)
    {
	*arg += len;
	पटिनिवत्त type;
    }

    सचे (give_error && (did_emsg == did_emsg_before))
    {
	char_u	*p = skip_type(*arg, FALSE);
	वर्ण	cc = *p;

	*p = NUL;
	semsg(_(e_type_not_recognized_str), *arg);
	*p = cc;
    }

    पटिनिवत्त रिक्त;
}

/*
 * Inner part of parse_type().
 */
    स्थिर type_T *
parse_type_inner(
    char_u	**arg,
    garray_T	*type_gap,
    ufunc_T	*ufunc,
    cctx_T	*cctx,
    पूर्णाङ्क		give_error)
{
    char_u  *p = *arg;
    size_t  len;

    // Skip over the first word.
    याव (ASCII_ISALNUM(*p) || *p == '_')
	++p;
    len = p - *arg;

    विकल्प (**arg)
    {
	पक्ष 'a':
	    सचे (len == 3 && STRNCMP(*arg, "any", len) == 0)
	    {
		*arg += len;
		पटिनिवत्त &t_any;
	    }
	    भङ्ग;
	पक्ष 'b':
	    सचे (len == 4 && STRNCMP(*arg, "bool", len) == 0)
	    {
		*arg += len;
		पटिनिवत्त &t_bool;
	    }
	    सचे (len == 4 && STRNCMP(*arg, "blob", len) == 0)
	    {
		*arg += len;
		पटिनिवत्त &t_blob;
	    }
	    भङ्ग;
	पक्ष 'c':
	    सचे (len == 7 && STRNCMP(*arg, "channel", len) == 0)
	    {
		*arg += len;
		पटिनिवत्त &t_channel;
	    }
	    भङ्ग;
	पक्ष 'd':
	    सचे (len == 4 && STRNCMP(*arg, "dict", len) == 0)
	    {
		*arg += len;
		पटिनिवत्त parse_type_member(arg, &t_dict_any, type_gap,
						give_error, "dict", ufunc,
						cctx);
	    }
	    भङ्ग;
	पक्ष 'f':
	    सचे (len == 5 && STRNCMP(*arg, "float", len) == 0)
	    {
		*arg += len;
		पटिनिवत्त &t_float;
	    }
	    सचे (len == 4 && STRNCMP(*arg, "func", len) == 0)
		पटिनिवत्त parse_type_func(arg, len, type_gap, give_error, ufunc,
									cctx);
	    भङ्ग;
	पक्ष 'j':
	    सचे (len == 3 && STRNCMP(*arg, "job", len) == 0)
	    {
		*arg += len;
		पटिनिवत्त &t_job;
	    }
	    भङ्ग;
	पक्ष 'l':
	    सचे (len == 4 && STRNCMP(*arg, "list", len) == 0)
	    {
		*arg += len;
		पटिनिवत्त parse_type_member(arg, &t_list_any, type_gap,
						give_error, "list", ufunc,
						cctx);
	    }
	    भङ्ग;
	पक्ष 'n':
	    सचे (len == 6 && STRNCMP(*arg, "number", len) == 0)
	    {
		*arg += len;
		पटिनिवत्त &t_सङ्ख्या;
	    }
	    भङ्ग;
	पक्ष 'o':
	    सचे (len == 6 && STRNCMP(*arg, "object", len) == 0)
	    {
		*arg += len;
		पटिनिवत्त parse_type_object(arg, type_gap, give_error, cctx);
	    }
	    भङ्ग;
	पक्ष 's':
	    सचे (len == 6 && STRNCMP(*arg, "string", len) == 0)
	    {
		*arg += len;
		पटिनिवत्त &t_string;
	    }
	    भङ्ग;
	पक्ष 't':
	    सचे (len == 5 && STRNCMP(*arg, "tuple", len) == 0)
	    {
		*arg += len;
		पटिनिवत्त parse_type_tuple(arg, type_gap, give_error, ufunc,
									cctx);
	    }
	    भङ्ग;
	पक्ष 'v':
	    सचे (len == 4 && STRNCMP(*arg, "void", len) == 0)
	    {
		*arg += len;
		पटिनिवत्त &t_void;
	    }
	    भङ्ग;
    }

    // User defined type
    पटिनिवत्त parse_type_user_defined(arg, len, type_gap, give_error, ufunc,
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
    पूर्णाङ्क		give_error)
{
    type_T	*type;

    सचे (parse_depth > MAX_TYPE_DEPTH)
    {
	सचे (give_error)
	    emsg(_(e_type_nested_too_deep));
	पटिनिवत्त रिक्त;
    }
    सचे (parse_depth == 0)
	parse_max_depth = 0;
    ++parse_depth;
    सचे (parse_depth > parse_max_depth)
	parse_max_depth = parse_depth;
    type = parse_type_inner(arg, type_gap, ufunc, cctx, give_error);
    --parse_depth;
    पटिनिवत्त type;
}

/*
 * Return the number of levels of the type last parsed by parse_type().
 */
    पूर्णाङ्क
parse_type_depth(शून्य)
{
    पटिनिवत्त parse_max_depth;
}

/*
 * Check if "type1" and "type2" are exactly the same.
 * "flags" can have ETYPE_ARG_UNKNOWN, which means that an unknown argument
 * type in "type1" is accepted.
 */
    पूर्णाङ्क
equal_type(type_T *type1, type_T *type2, पूर्णाङ्क flags)
{
    पूर्णाङ्क i;

    सचे (type1 == रिक्त || type2 == रिक्त)
	पटिनिवत्त FALSE;
    सचे (type1->tt_type != type2->tt_type)
	पटिनिवत्त FALSE;
    विकल्प (type1->tt_type)
    {
	पक्ष VAR_UNKNOWN:
	पक्ष VAR_ANY:
	पक्ष VAR_VOID:
	पक्ष VAR_SPECIAL:
	पक्ष VAR_BOOL:
	पक्ष VAR_सङ्ख्या:
	पक्ष VAR_FLOAT:
	पक्ष VAR_STRING:
	पक्ष VAR_BLOB:
	पक्ष VAR_JOB:
	पक्ष VAR_CHANNEL:
	पक्ष VAR_INSTR:
	पक्ष VAR_TYPEALIAS:
	    भङ्ग;  // not composite is always OK
	पक्ष VAR_OBJECT:
	पक्ष VAR_CLASS:
	    // Objects are considered equal if they are from the same class
	    पटिनिवत्त type1->tt_class == type2->tt_class;
	पक्ष VAR_LIST:
	पक्ष VAR_DICT:
	    पटिनिवत्त equal_type(type1->tt_member, type2->tt_member, flags);
	पक्ष VAR_TUPLE:
	    सचे (type1->tt_argcount != type2->tt_argcount)
		पटिनिवत्त FALSE;
	    सचे (type1->tt_argcount < 0
			   || type1->tt_args == रिक्त || type2->tt_args == रिक्त)
		पटिनिवत्त TRUE;
	    अत्थाय (i = 0; i < type1->tt_argcount; ++i)
		सचे ((flags & ETYPE_ARG_UNKNOWN) == 0
			&& !equal_type(type1->tt_args[i], type2->tt_args[i],
									flags))
		    पटिनिवत्त FALSE;
	    पटिनिवत्त TRUE;
	पक्ष VAR_FUNC:
	पक्ष VAR_PARTIAL:
	    सचे (!equal_type(type1->tt_member, type2->tt_member, flags)
		    || type1->tt_argcount != type2->tt_argcount)
		पटिनिवत्त FALSE;
	    सचे (type1->tt_argcount < 0
			   || type1->tt_args == रिक्त || type2->tt_args == रिक्त)
		पटिनिवत्त TRUE;
	    अत्थाय (i = 0; i < type1->tt_argcount; ++i)
		सचे ((flags & ETYPE_ARG_UNKNOWN) == 0
			&& !equal_type(type1->tt_args[i], type2->tt_args[i],
									flags))
		    पटिनिवत्त FALSE;
	    पटिनिवत्त TRUE;
    }
    पटिनिवत्त TRUE;
}

/*
 * Find the common type of "type1" (VAR_FUNC) and "type2" (VAR_FUNC) and put it
 * in "dest".  "type2" and "dest" may be the same.
 */
    स्थिर शून्य
common_type_var_func(
    type_T	*type1,
    type_T	*type2,
    type_T	**dest,
    garray_T	*type_gap)
{
    type_T *common;

    // When one of the types is t_func_unknown return the other one.
    // Useful if a list or dict item is null_func.
    सचे (type1 == &t_func_unknown)
    {
	*dest = type2;
	पटिनिवत्त;
    }
    सचे (type2 == &t_func_unknown)
    {
	*dest = type1;
	पटिनिवत्त;
    }

    common_type(type1->tt_member, type2->tt_member, &common, type_gap);
    सचे (type1->tt_argcount == type2->tt_argcount
	    && type1->tt_argcount >= 0)
    {
	पूर्णाङ्क argcount = type1->tt_argcount;
	पूर्णाङ्क i;

	*dest = alloc_func_type(common, argcount, type_gap);
	सचे (type1->tt_args != रिक्त && type2->tt_args != रिक्त)
	{
	    सचे (func_type_add_arg_types(*dest, argcount,
			type_gap) == OK)
		अत्थाय (i = 0; i < argcount; ++i)
		    common_type(type1->tt_args[i], type2->tt_args[i],
			    &(*dest)->tt_args[i], type_gap);
	}
    }
    अञ्ञथा
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
    शून्य
common_type(type_T *type1, type_T *type2, type_T **dest, garray_T *type_gap)
{
    सचे (equal_type(type1, type2, 0))
    {
	*dest = type1;
	पटिनिवत्त;
    }

    // If either is VAR_UNKNOWN use the other type.  An empty list/dict has no
    // specific type.
    सचे (type1 == रिक्त || type1->tt_type == VAR_UNKNOWN)
    {
	*dest = type2;
	पटिनिवत्त;
    }
    सचे (type2 == रिक्त || type2->tt_type == VAR_UNKNOWN)
    {
	*dest = type1;
	पटिनिवत्त;
    }

    सचे (type1->tt_type == type2->tt_type)
    {
	सचे (type1->tt_type == VAR_LIST
		|| type1->tt_type == VAR_DICT)
	{
	    type_T *common;

	    common_type(type1->tt_member, type2->tt_member, &common, type_gap);
	    सचे (type1->tt_type == VAR_LIST)
		*dest = get_list_type(common, type_gap);
	    अञ्ञथा
		*dest = get_dict_type(common, type_gap);
	    पटिनिवत्त;
	}
	अञ्ञथा सचे (type1->tt_type == VAR_FUNC)
	{
	    common_type_var_func(type1, type2, dest, type_gap);
	    पटिनिवत्त;
	}
	अञ्ञथा सचे (type1->tt_type == VAR_OBJECT)
	{
	    *dest = &t_object_any;
	    पटिनिवत्त;
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
    सचे (type->tt_type == VAR_TUPLE)
    {
	सचे (type->tt_argcount != 1)
	    पटिनिवत्त &t_any;

	सचे (type->tt_flags & TTFLAG_VARARGS)
	    पटिनिवत्त type->tt_args[0]->tt_member;
	अञ्ञथा
	    पटिनिवत्त type->tt_args[0];
    }

    // For "any" the item type is not known.
    पटिनिवत्त type->tt_member == रिक्त ? &t_any : type->tt_member;
}

/*
 * Push an entry onto the type stack.  "type" used both for the current type
 * and the declared type.
 * Returns FAIL when out of memory.
 */
    पूर्णाङ्क
प्रक्षिप_type_राशिः(cctx_T *cctx, type_T *type)
{
    पटिनिवत्त प्रक्षिप_type_stack2(cctx, type, type);
}

/*
 * Push an entry onto the type stack.  "type" is the current type, "decl_type"
 * is the declared type.
 * Returns FAIL when out of memory.
 */
    पूर्णाङ्क
प्रक्षिप_type_stack2(cctx_T *cctx, type_T *type, type_T *decl_type)
{
    garray_T	*राशिः = &cctx->ctx_type_राशिः;
    type2_T	*typep;

    सचे (GA_GROW_FAILS(राशिः, 1))
	पटिनिवत्त FAIL;
    typep = ((type2_T *)राशिः->ga_data) + राशिः->ga_len;
    typep->type_curr = type;
    typep->type_decl = decl_type;
    ++राशिः->ga_len;
    पटिनिवत्त OK;
}

/*
 * Set the type of the top of the stack to "type".
 */
    शून्य
set_type_on_राशिः(cctx_T *cctx, type_T *type, पूर्णाङ्क offset)
{
    garray_T	*राशिः = &cctx->ctx_type_राशिः;
    type2_T	*typep = ((type2_T *)राशिः->ga_data)
						  + राशिः->ga_len - 1 - offset;

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
get_type_on_राशिः(cctx_T *cctx, पूर्णाङ्क offset)
{
    garray_T	*राशिः = &cctx->ctx_type_राशिः;

    सचे (offset + 1 > राशिः->ga_len)
	पटिनिवत्त &t_unknown;
    पटिनिवत्त (((type2_T *)राशिः->ga_data) + राशिः->ga_len - offset - 1)
								   ->type_curr;
}

/*
 * Get the declared type from the type stack.  If "offset" is zero the one at
 * the top,
 * if "offset" is one the type above that, etc.
 * Returns &t_unknown if there is no such stack entry.
 */
    type_T *
get_decl_type_on_राशिः(cctx_T *cctx, पूर्णाङ्क offset)
{
    garray_T	*राशिः = &cctx->ctx_type_राशिः;

    सचे (offset + 1 > राशिः->ga_len)
	पटिनिवत्त &t_unknown;
    पटिनिवत्त (((type2_T *)राशिः->ga_data) + राशिः->ga_len - offset - 1)
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
get_member_type_from_राशिः(
	पूर्णाङ्क	    गणनम्,
	पूर्णाङ्क	    skip,
	cctx_T	    *cctx)
{
    garray_T	*राशिः = &cctx->ctx_type_राशिः;
    type2_T	*typep;
    garray_T    *type_gap = cctx->ctx_type_list;
    पूर्णाङ्क		i;
    type_T	*result;
    type_T	*type;

    // Use "unknown" for an empty list or dict.
    सचे (गणनम् == 0)
	पटिनिवत्त &t_unknown;
    // Find the common type from following items.
    typep = ((type2_T *)राशिः->ga_data) + राशिः->ga_len;
    result = &t_unknown;
    अत्थाय (i = 0; i < गणनम्; ++i)
    {
	type = (typep -((गणनम् - i) * skip) + skip - 1)->type_curr;
	सचे (check_type_is_मानम्(type) == FAIL)
	    पटिनिवत्त रिक्त;
	सचे (result != &t_any)
	    common_type(type, result, &result, type_gap);
    }

    पटिनिवत्त result;
}

/*
 * Get the types of items in a tuple on the stack of "cctx".
 * Returns the number of types.  Returns -1 on failure.
 */
    पूर्णाङ्क
get_tuple_type_from_राशिः(
    पूर्णाङ्क		गणनम्,
    garray_T	*tuple_types_gap,
    cctx_T	*cctx)
{
    garray_T	*राशिः = &cctx->ctx_type_राशिः;
    type2_T	*typep;
    type_T	*type = रिक्त;

    // Use "unknown" for an empty tuple
    सचे (गणनम् == 0)
	पटिनिवत्त 0;

    // Find the common type from following items.
    typep = ((type2_T *)राशिः->ga_data) + राशिः->ga_len;
    अत्थाय (पूर्णाङ्क i = 0; i < गणनम्; i++)
    {
	type = (typep - (गणनम् - i))->type_curr;
	सचे (check_type_is_मानम्(type) == FAIL)
	    पटिनिवत्त -1;
	सचे (ga_grow(tuple_types_gap, 1) == FAIL)
	    पटिनिवत्त -1;
	((type_T **)tuple_types_gap->ga_data)[tuple_types_gap->ga_len] = type;
	tuple_types_gap->ga_len++;
    }

    पटिनिवत्त tuple_types_gap->ga_len;
}

    वर्ण *
vartype_name(vartype_T type)
{
    विकल्प (type)
    {
	पक्ष VAR_UNKNOWN: भङ्ग;
	पक्ष VAR_ANY: पटिनिवत्त "any";
	पक्ष VAR_VOID: पटिनिवत्त "void";
	पक्ष VAR_SPECIAL: पटिनिवत्त "special";
	पक्ष VAR_BOOL: पटिनिवत्त "bool";
	पक्ष VAR_सङ्ख्या: पटिनिवत्त "number";
	पक्ष VAR_FLOAT: पटिनिवत्त "float";
	पक्ष VAR_STRING: पटिनिवत्त "string";
	पक्ष VAR_BLOB: पटिनिवत्त "blob";
	पक्ष VAR_JOB: पटिनिवत्त "job";
	पक्ष VAR_CHANNEL: पटिनिवत्त "channel";
	पक्ष VAR_LIST: पटिनिवत्त "list";
	पक्ष VAR_TUPLE: पटिनिवत्त "tuple";
	पक्ष VAR_DICT: पटिनिवत्त "dict";
	पक्ष VAR_INSTR: पटिनिवत्त "instr";
	पक्ष VAR_CLASS: पटिनिवत्त "class";
	पक्ष VAR_OBJECT: पटिनिवत्त "object";
	पक्ष VAR_TYPEALIAS: पटिनिवत्त "typealias";

	पक्ष VAR_FUNC:
	पक्ष VAR_PARTIAL: पटिनिवत्त "func";
    }
    पटिनिवत्त "unknown";
}

/*
 * Return the type name of a List (list<type>) or Dict (dict<type>).
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    स्थिर वर्ण *
type_name_list_or_dict(वर्ण *name, type_T *type, वर्ण **tofree)
{
    वर्ण *member_free;
    वर्ण *member_name;

    सचे (type->tt_member->tt_type == VAR_UNKNOWN)
	member_name = type_name(&t_any, &member_free);
    अञ्ञथा
	member_name = type_name(type->tt_member, &member_free);

    size_t len = STRLEN(name) + STRLEN(member_name) + 3;
    *tofree = alloc(len);
    सचे (*tofree == रिक्त)
    {
	vim_free(member_free);
	पटिनिवत्त name;
    }

    vim_snprintf(*tofree, len, "%s<%s>", name, member_name);
    vim_free(member_free);
    पटिनिवत्त *tofree;
}

/*
 * Return the type name of a tuple.
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    स्थिर वर्ण *
type_name_tuple(type_T *type, वर्ण **tofree)
{
    garray_T    ga;
    पूर्णाङ्क		i;
    पूर्णाङ्क		varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;
    वर्ण	*arg_free = रिक्त;

    ga_init2(&ga, 1, 100);
    सचे (ga_grow(&ga, 20) == FAIL)
	गच्छ failed;
    STRCPY(ga.ga_data, "tuple<");
    ga.ga_len += 6;

    सचे (type->tt_argcount <= 0)
	// empty tuple
	GA_CONCAT_LITERAL(&ga, "any");
    अञ्ञथा
    {
	सचे (type->tt_args == रिक्त)
	    GA_CONCAT_LITERAL(&ga, "[unknown]");
	अञ्ञथा
	{
	    अत्थाय (i = 0; i < type->tt_argcount; ++i)
	    {
		string_T    arg_type;

		arg_type.string = (char_u *)type_name(type->tt_args[i], &arg_free);
		सचे (i > 0)
		{
		    STRCPY((वर्ण *)ga.ga_data + ga.ga_len, ", ");
		    ga.ga_len += 2;
		}
		arg_type.length = STRLEN(arg_type.string);
		सचे (ga_grow(&ga, (पूर्णाङ्क)arg_type.length + 8) == FAIL)
		    गच्छ failed;
		सचे (varargs && i == type->tt_argcount - 1)
		    GA_CONCAT_LITERAL(&ga, "...");
		ga_concat_len(&ga, arg_type.string, arg_type.length);
		VIM_CLEAR(arg_free);
	    }
	}
    }

    STRCPY((वर्ण *)ga.ga_data + ga.ga_len, ">");
    *tofree = ga.ga_data;
    पटिनिवत्त ga.ga_data;

failed:
    vim_free(arg_free);
    ga_clear(&ga);
    पटिनिवत्त "[unknown]";
}

/*
 * Return the type name of a Class (class<name>) or Object (object<name>).
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    स्थिर वर्ण *
type_name_class_or_obj(वर्ण *name, type_T *type, वर्ण **tofree)
{
    string_T	class_name;

    सचे (type->tt_class != रिक्त)
    {
	class_name.string = type->tt_class->class_name.string;
	class_name.length = type->tt_class->class_name.length;
	सचे (IS_ENUM(type->tt_class))
	    name = "enum";
    }
    अञ्ञथा
	STR_LITERAL_SET(class_name, "any");

    size_t len = STRLEN(name) + class_name.length + 3;
    *tofree = alloc(len);
    सचे (*tofree == रिक्त)
	पटिनिवत्त name;

    vim_snprintf(*tofree, len, "%s<%s>", name, class_name.string);
    पटिनिवत्त *tofree;
}

/*
 * Return the type name of a function.
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    स्थिर वर्ण *
type_name_func(type_T *type, वर्ण **tofree)
{
    garray_T    ga;
    पूर्णाङ्क		i;
    पूर्णाङ्क		varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;
    वर्ण	*arg_free = रिक्त;

    ga_init2(&ga, 1, 100);
    सचे (ga_grow(&ga, 20) == FAIL)
	गच्छ failed;
    STRCPY(ga.ga_data, "func(");
    ga.ga_len += 5;

    अत्थाय (i = 0; i < type->tt_argcount; ++i)
    {
	string_T    arg_type;

	सचे (type->tt_args == रिक्त)
	    STR_LITERAL_SET(arg_type, "[unknown]");
	अञ्ञथा
	{
	    arg_type.string = (char_u *)type_name(type->tt_args[i], &arg_free);
	    arg_type.length = STRLEN(arg_type.string);
	}
	सचे (i > 0)
	{
	    STRCPY((वर्ण *)ga.ga_data + ga.ga_len, ", ");
	    ga.ga_len += 2;
	}
	सचे (ga_grow(&ga, (पूर्णाङ्क)arg_type.length + 8) == FAIL)
	    गच्छ failed;
	सचे (varargs && i == type->tt_argcount - 1)
	    GA_CONCAT_LITERAL(&ga, "...");
	अञ्ञथा सचे (i >= type->tt_min_argcount)
	    *((वर्ण *)ga.ga_data + ga.ga_len++) = '?';
	ga_concat_len(&ga, arg_type.string, arg_type.length);
	VIM_CLEAR(arg_free);
    }
    सचे (type->tt_argcount < 0)
	// any number of arguments
	GA_CONCAT_LITERAL(&ga, "...");

    सचे (type->tt_member == &t_void)
	STRCPY((वर्ण *)ga.ga_data + ga.ga_len, ")");
    अञ्ञथा
    {
	वर्ण *ret_free = रिक्त;
	वर्ण *ret_name = type_name(type->tt_member, &ret_free);
	पूर्णाङ्क  len;

	len = (पूर्णाङ्क)STRLEN(ret_name) + 4;
	सचे (ga_grow(&ga, len) == FAIL)
	{
	    vim_free(ret_free);
	    गच्छ failed;
	}
	STRCPY((वर्ण *)ga.ga_data + ga.ga_len, "): ");
	STRCPY((वर्ण *)ga.ga_data + ga.ga_len + 3, ret_name);
	vim_free(ret_free);
    }
    *tofree = ga.ga_data;
    पटिनिवत्त ga.ga_data;

failed:
    vim_free(arg_free);
    ga_clear(&ga);
    पटिनिवत्त "[unknown]";
}

/*
 * Return the name of a type.
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    वर्ण *
type_name(type_T *type, वर्ण **tofree)
{
    वर्ण *name;

    *tofree = रिक्त;
    सचे (type == रिक्त)
	पटिनिवत्त "[unknown]";
    name = vartype_name(type->tt_type);

    विकल्प (type->tt_type)
    {
	पक्ष VAR_LIST:
	पक्ष VAR_DICT:
	    पटिनिवत्त type_name_list_or_dict(name, type, tofree);

	पक्ष VAR_TUPLE:
	    पटिनिवत्त type_name_tuple(type, tofree);

	पक्ष VAR_CLASS:
	पक्ष VAR_OBJECT:
	    पटिनिवत्त type_name_class_or_obj(name, type, tofree);

	पक्ष VAR_FUNC:
	    पटिनिवत्त type_name_func(type, tofree);

	स्वभाव:
	    भङ्ग;
    }

    पटिनिवत्त name;
}

/*
 * "typename(expr)" function
 */
    शून्य
f_typename(typval_T *argvars, typval_T *rettv)
{
    garray_T	type_list;
    type_T	*type;
    वर्ण	*tofree;
    वर्ण	*name;

    rettv->v_type = VAR_STRING;
    ga_init2(&type_list, परिमाण(type_T *), 10);
    सचे (argvars[0].v_type == VAR_TYPEALIAS)
    {
	type = copy_type(argvars[0].vval.v_typealias->ta_type, &type_list);
	// A type alias for a class has the type set to VAR_OBJECT.  Change it
	// to VAR_CLASS, so that the name is "typealias<class<xxx>>"
	सचे (type->tt_type == VAR_OBJECT)
	    type->tt_type = VAR_CLASS;
    }
    अञ्ञथा
	type = typval2type(argvars, get_copyID(), &type_list, TVTT_DO_MEMBER);
    name = type_name(type, &tofree);
    सचे (argvars[0].v_type == VAR_TYPEALIAS)
    {
	vim_snprintf((वर्ण *)IObuff, IOSIZE, "typealias<%s>", name);
	rettv->vval.v_string = vim_strsave((char_u *)IObuff);
	सचे (tofree != रिक्त)
	    vim_free(tofree);
    }
    अञ्ञथा
    {
	सचे (tofree != रिक्त)
	    rettv->vval.v_string = (char_u *)tofree;
	अञ्ञथा
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
    पूर्णाङ्क
check_typval_is_मानम्(typval_T *tv)
{
    सचे (tv == रिक्त)
	पटिनिवत्त OK;

    विकल्प (tv->v_type)
    {
	पक्ष VAR_CLASS:
	    {
		class_T *cl = tv->vval.v_class;
		char_u *class_name = (cl == रिक्त) ? (char_u *)""
							: cl->class_name.string;
		सचे (cl != रिक्त && IS_ENUM(cl))
		    semsg(_(e_using_enum_as_मानम्_str), class_name);
		अञ्ञथा
		    semsg(_(e_using_class_as_मानम्_str), class_name);
	    }
	    पटिनिवत्त FAIL;

	पक्ष VAR_TYPEALIAS:
	    semsg(_(e_using_typealias_as_मानम्_str),
		    tv->vval.v_typealias->ta_name);
	    पटिनिवत्त FAIL;

	स्वभाव:
	    भङ्ग;
    }
    पटिनिवत्त OK;
}

/*
 * Same as above, except check type_T.
 */
    पूर्णाङ्क
check_type_is_मानम्(type_T *type)
{
    सचे (type == रिक्त)
	पटिनिवत्त OK;
    विकल्प (type->tt_type)
    {
	पक्ष VAR_CLASS:
	    सचे (type->tt_class != रिक्त && IS_ENUM(type->tt_class))
		semsg(_(e_using_enum_as_मानम्_str),
			type->tt_class->class_name.string);
	    अञ्ञथा
		semsg(_(e_using_class_as_मानम्_str),
			type->tt_class == रिक्त ? (char_u *)""
			: type->tt_class->class_name.string);
	    पटिनिवत्त FAIL;

	पक्ष VAR_TYPEALIAS:
	    // TODO: Not sure what could be done here to get a name.
	    //       Maybe an optional argument?
	    emsg(_(e_using_typealias_as_var_val));
	    पटिनिवत्त FAIL;

	स्वभाव:
	    भङ्ग;
    }
    पटिनिवत्त OK;
}

#endif // FEAT_EVAL
