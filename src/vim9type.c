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

#నిర్వచించు USING_FLOAT_STUFF
#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_EVAL)

#నిర్వచితమైతే VMS
# include <float.h>
#అయితే_ముగింపు

/*
 * Allocate memory for a type_T and add the pointer to type_gap, so that it can
 * be easily freed later.
 */
    type_T *
get_type_ptr(garray_T *type_gap)
{
    type_T *type;

    ఒకవేళ (ga_grow(type_gap, 1) == FAIL)
	తిరిగి ఖాళీ;
    type = ALLOC_CLEAR_ONE(type_T);
    ఒకవేళ (type == ఖాళీ)
	తిరిగి ఖాళీ;

    ((type_T **)type_gap->ga_దత్తాంశం)[type_gap->ga_len] = type;
    ++type_gap->ga_len;
    తిరిగి type;
}

/*
 * Make a shallow copy of "type".
 * When allocation fails returns "type".
 */
    type_T *
copy_type(type_T *type, garray_T *type_gap)
{
    type_T *copy = get_type_ptr(type_gap);

    ఒకవేళ (copy == ఖాళీ)
	తిరిగి type;
    *copy = *type;
    copy->tt_flags &= ~TTFLAG_STATIC;

    ఒకవేళ (type->tt_వాదనలు != ఖాళీ
	   && func_type_కూడు_arg_types(copy, type->tt_argcount, type_gap) == OK)
	కొరకు (పూర్ణాంకం i = 0; i < type->tt_argcount; ++i)
	    copy->tt_వాదనలు[i] = type->tt_వాదనలు[i];

    తిరిగి copy;
}

/*
 * Inner part of copy_type_deep().
 * When allocation fails returns "type".
 */
    స్థిర type_T *
copy_type_deep_rec(type_T *type, garray_T *type_gap, garray_T *చూసిన_types)
{
    కొరకు (పూర్ణాంకం i = 0; i < చూసిన_types->ga_len; ++i)
	ఒకవేళ (((type_T **)చూసిన_types->ga_దత్తాంశం)[i * 2] == type)
	    // seen this type before, return the copy we made
	    తిరిగి ((type_T **)చూసిన_types->ga_దత్తాంశం)[i * 2 + 1];

    type_T *copy = copy_type(type, type_gap);
    ఒకవేళ (ga_grow(చూసిన_types, 1) == FAIL)
	తిరిగి copy;
    ((type_T **)చూసిన_types->ga_దత్తాంశం)[చూసిన_types->ga_len * 2] = type;
    ((type_T **)చూసిన_types->ga_దత్తాంశం)[చూసిన_types->ga_len * 2 + 1] = copy;
    ++చూసిన_types->ga_len;

    ఒకవేళ (copy->tt_member != ఖాళీ)
	copy->tt_member = copy_type_deep_rec(copy->tt_member,
							 type_gap, చూసిన_types);
    ఒకవేళ (type->tt_వాదనలు != ఖాళీ)
	కొరకు (పూర్ణాంకం i = 0; i < type->tt_argcount; ++i)
	    copy->tt_వాదనలు[i] = copy_type_deep_rec(copy->tt_వాదనలు[i],
							 type_gap, చూసిన_types);

    తిరిగి copy;
}

/*
 * Make a deep copy of "type".
 * When allocation fails returns "type".
 */
    type_T *
copy_type_deep(type_T *type, garray_T *type_gap)
{
    garray_T చూసిన_types;
    // stores type pairs : a type we have seen and the copy used
    ga_init2(&చూసిన_types, పరిమాణం(type_T *) * 2, 20);

    type_T *res = copy_type_deep_rec(type, type_gap, &చూసిన_types);

    ga_clear(&చూసిన_types);
    తిరిగి res;
}

    శూన్య
clear_type_list(garray_T *gap)
{
    అయితే (gap->ga_len > 0)
	vim_free(((type_T **)gap->ga_దత్తాంశం)[--gap->ga_len]);
    ga_clear(gap);
}

    శూన్య
clear_func_type_list(garray_T *gap, type_T **func_type)
{
    అయితే (gap->ga_len > 0)
    {
	// func_type pointing to the uf_type_list, so reset pointer
	ఒకవేళ (*func_type == ((type_T **)gap->ga_దత్తాంశం)[--gap->ga_len])
	    *func_type = &t_func_any;
	vim_free(((type_T **)gap->ga_దత్తాంశం)[gap->ga_len]);
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

    ఒకవేళ (type == ఖాళీ)
	తిరిగి ఖాళీ;

    // A fixed type never contains allocated types, return as-is.
    ఒకవేళ (type->tt_flags & TTFLAG_STATIC)
	తిరిగి type;

    ret = ALLOC_ONE(type_T);
    ఒకవేళ (ret == ఖాళీ)
	తిరిగి ఖాళీ;
    *ret = *type;

    ఒకవేళ (ret->tt_member != ఖాళీ)
	ret->tt_member = alloc_type(ret->tt_member);

    ఒకవేళ (type->tt_argcount > 0 && type->tt_వాదనలు != ఖాళీ)
    {
	పూర్ణాంకం i;

	ret->tt_వాదనలు = ALLOC_MULT(type_T *, type->tt_argcount);
	ఒకవేళ (ret->tt_వాదనలు != ఖాళీ)
	    కొరకు (i = 0; i < type->tt_argcount; ++i)
		ret->tt_వాదనలు[i] = alloc_type(type->tt_వాదనలు[i]);
    }
    లేకపోతే
	ret->tt_వాదనలు = ఖాళీ;

    తిరిగి ret;
}

/*
 * Free a type that was created with alloc_type().
 */
    శూన్య
free_type(type_T *type)
{
    పూర్ణాంకం i;

    ఒకవేళ (type == ఖాళీ || (type->tt_flags & TTFLAG_STATIC))
	తిరిగి;
    ఒకవేళ (type->tt_వాదనలు != ఖాళీ)
    {
	కొరకు (i = 0; i < type->tt_argcount; ++i)
	    free_type(type->tt_వాదనలు[i]);
	vim_free(type->tt_వాదనలు);
    }

    free_type(type->tt_member);

    vim_free(type);
}

/*
 * Return TRUE if "type" is to be recursed into for setting the type.
 */
    స్థిర పూర్ణాంకం
సమితి_tv_type_recurse(type_T *type)
{
    తిరిగి type->tt_member != ఖాళీ
		&& (type->tt_member->tt_type == VAR_DICT
				|| type->tt_member->tt_type == VAR_LIST)
		&& type->tt_member->tt_member != ఖాళీ
		&& type->tt_member->tt_member != &t_any
		&& type->tt_member->tt_member != &t_unknown;
}

/*
 * Set the type of Dict "d" to "type"
 */
    స్థిర శూన్య
సమితి_tv_type_dict(dict_T *d, type_T *type)
{
    ఒకవేళ (d->dv_type == type)
	తిరిగి;

    free_type(d->dv_type);
    d->dv_type = alloc_type(type);

    // Need to recursively set the type of dict items?
    ఒకవేళ (!సమితి_tv_type_recurse(type))
	తిరిగి;

    పూర్ణాంకం		todo = (పూర్ణాంకం)d->dv_hashtab.ht_వాడిన;
    hashitem_T	*hi;
    dictitem_T	*di;

    FOR_ALL_HASHTAB_వస్తువులు(&d->dv_hashtab, hi, todo)
    {
	ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
	{
	    --todo;
	    di = HI2DI(hi);
	    సమితి_tv_type(&di->di_tv, type->tt_member);
	}
    }
}

/*
 * Set the type of List "l" to "type"
 */
    స్థిర శూన్య
సమితి_tv_type_list(list_T *l, type_T *type)
{
    ఒకవేళ (l->lv_type == type)
	తిరిగి;

    free_type(l->lv_type);
    l->lv_type = alloc_type(type);

    // Need to recursively set the type of list items?
    ఒకవేళ (l->lv_మొదటి == &range_list_అంశం || !సమితి_tv_type_recurse(type))
	తిరిగి;

    listitem_T	*li;

    FOR_ALL_LIST_వస్తువులు(l, li)
	సమితి_tv_type(&li->li_tv, type->tt_member);
}

/*
 * Set the type of Tuple "tuple" to "type"
 */
    స్థిర శూన్య
సమితి_tv_type_tuple(tuple_T *tuple, type_T *type)
{
    ఒకవేళ (tuple->tv_type == type)
	తిరిగి;

    free_type(tuple->tv_type);
    tuple->tv_type = alloc_type(type);

    ఒకవేళ (type->tt_argcount <= 0)
	తిరిగి;

    // recursively set the type of list items
    type_T	*అంశం_type;
    కొరకు (పూర్ణాంకం i = 0; i < tuple_len(tuple); i++)
    {
	ఒకవేళ ((type->tt_flags & TTFLAG_VARARGS) && (i >= type->tt_argcount - 1))
	    // For a variadic tuple, the last type is a List.  So use the
	    // List member type.
	    అంశం_type = type->tt_వాదనలు[type->tt_argcount - 1]->tt_member;
	లేకపోతే
	    అంశం_type = type->tt_వాదనలు[i];

	సమితి_tv_type(TUPLE_అంశం(tuple, i), అంశం_type);
    }
}

/*
 * Set the type of "tv" to "type" if it is a list or tuple or dict.
 */
    శూన్య
సమితి_tv_type(typval_T *tv, type_T *type)
{
    ఒకవేళ (type->tt_type == VAR_ANY)
	// If the variable type is "any", then keep the value type.
	// e.g.  var x: any = [1, 2] or var y: any = {v: 1}
	తిరిగి;

    ఒకవేళ (tv->v_type == VAR_DICT && tv->vval.v_dict != ఖాళీ)
	సమితి_tv_type_dict(tv->vval.v_dict, type);
    లేకపోతే ఒకవేళ (tv->v_type == VAR_LIST && tv->vval.v_list != ఖాళీ)
	సమితి_tv_type_list(tv->vval.v_list, type);
    లేకపోతే ఒకవేళ (tv->v_type == VAR_TUPLE && tv->vval.v_tuple != ఖాళీ)
	సమితి_tv_type_tuple(tv->vval.v_tuple, type);
}

/*
 * For a tuple type, reserve space for "typecount" types (including the
 * repeated type).
 */
    స్థిర పూర్ణాంకం
tuple_type_కూడు_types(
    type_T	*tupletype,
    పూర్ణాంకం		typecount,
    garray_T	*type_gap)
{
    // To make it easy to free the space needed for the types, add the
    // pointer to type_gap.
    ఒకవేళ (ga_grow(type_gap, 1) == FAIL)
	తిరిగి FAIL;
    tupletype->tt_వాదనలు = alloc_clear_id(పరిమాణం(type_T *) * typecount,
							  aid_tuple_type_వాదనలు);
    ఒకవేళ (tupletype->tt_వాదనలు == ఖాళీ)
	తిరిగి FAIL;
    ((type_T **)type_gap->ga_దత్తాంశం)[type_gap->ga_len] =
						(శూన్య *)tupletype->tt_వాదనలు;
    ++type_gap->ga_len;
    తిరిగి OK;
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
    ఒకవేళ (member_type == ఖాళీ || (member_type->tt_type == VAR_ANY
					&& !IS_GENERIC_TYPE(member_type)))
	తిరిగి &t_list_any;
    ఒకవేళ (member_type->tt_type == VAR_VOID
	    || member_type->tt_type == VAR_UNKNOWN)
	తిరిగి &t_list_ఖాళీలు;
    ఒకవేళ (member_type->tt_type == VAR_BOOL)
	తిరిగి &t_list_bool;
    ఒకవేళ (member_type->tt_type == VAR_సంఖ్య)
	తిరిగి &t_list_సంఖ్య;
    ఒకవేళ (member_type->tt_type == VAR_STRING)
	తిరిగి &t_list_string;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    ఒకవేళ (type == ఖాళీ)
	తిరిగి &t_any;
    type->tt_type = VAR_LIST;
    type->tt_member = member_type;
    type->tt_argcount = 0;
    type->tt_వాదనలు = ఖాళీ;
    తిరిగి type;
}

/*
 * Create and return a tuple type from the tuple item types in
 * "tuple_types_ga".
 */
    type_T *
get_tuple_type(garray_T *tuple_types_gap, garray_T *type_gap)
{
    type_T	*type;
    type_T	**tuple_types = tuple_types_gap->ga_దత్తాంశం;
    పూర్ణాంకం		typecount = tuple_types_gap->ga_len;

    // recognize commonly used types
    ఒకవేళ (typecount == 0)
	తిరిగి &t_tuple_any;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    ఒకవేళ (type == ఖాళీ)
	తిరిగి &t_any;
    type->tt_type = VAR_TUPLE;
    type->tt_member = ఖాళీ;
    ఒకవేళ (typecount > 0)
    {
	ఒకవేళ (tuple_type_కూడు_types(type, typecount, type_gap) == FAIL)
	    తిరిగి &t_any;
	mch_memmove(type->tt_వాదనలు, tuple_types, పరిమాణం(type_T *) * typecount);
    }
    type->tt_argcount = typecount;
    type->tt_flags = 0;

    తిరిగి type;
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
    ఒకవేళ (member_type == ఖాళీ || (member_type->tt_type == VAR_ANY
					&& !IS_GENERIC_TYPE(member_type)))
	తిరిగి &t_dict_any;
    ఒకవేళ (member_type->tt_type == VAR_VOID
	    || member_type->tt_type == VAR_UNKNOWN)
	తిరిగి &t_dict_ఖాళీలు;
    ఒకవేళ (member_type->tt_type == VAR_BOOL)
	తిరిగి &t_dict_bool;
    ఒకవేళ (member_type->tt_type == VAR_సంఖ్య)
	తిరిగి &t_dict_సంఖ్య;
    ఒకవేళ (member_type->tt_type == VAR_STRING)
	తిరిగి &t_dict_string;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    ఒకవేళ (type == ఖాళీ)
	తిరిగి &t_any;
    type->tt_type = VAR_DICT;
    type->tt_member = member_type;
    type->tt_argcount = 0;
    type->tt_వాదనలు = ఖాళీ;
    తిరిగి type;
}

/*
 * Allocate a new type for a function.
 */
    type_T *
alloc_func_type(type_T *ret_type, పూర్ణాంకం argcount, garray_T *type_gap)
{
    type_T *type = get_type_ptr(type_gap);

    ఒకవేళ (type == ఖాళీ)
	తిరిగి &t_any;
    type->tt_type = VAR_FUNC;
    type->tt_member = ret_type == ఖాళీ ? &t_unknown : ret_type;
    type->tt_argcount = argcount;
    type->tt_వాదనలు = ఖాళీ;
    తిరిగి type;
}

/*
 * Allocate a new type for a tuple.
 */
    స్థిర type_T *
alloc_tuple_type(పూర్ణాంకం typecount, garray_T *type_gap)
{
    type_T *type = get_type_ptr(type_gap);

    ఒకవేళ (type == ఖాళీ)
	తిరిగి &t_any;
    type->tt_type = VAR_TUPLE;
    type->tt_member = ఖాళీ;
    type->tt_argcount = typecount;
    type->tt_వాదనలు = ఖాళీ;
    తిరిగి type;
}

/*
 * Get a function type, based on the return type "ret_type".
 * "argcount" must be -1 or 0, a predefined type can be used.
 */
    type_T *
get_func_type(type_T *ret_type, పూర్ణాంకం argcount, garray_T *type_gap)
{
    // recognize commonly used types
    ఒకవేళ (ret_type == &t_unknown || ret_type == ఖాళీ)
    {
	// (argcount == 0) is not possible
	తిరిగి &t_func_unknown;
    }
    ఒకవేళ (ret_type == &t_void)
    {
	ఒకవేళ (argcount == 0)
	    తిరిగి &t_func_0_void;
	లేకపోతే
	    తిరిగి &t_func_void;
    }
    ఒకవేళ (ret_type == &t_any)
    {
	ఒకవేళ (argcount == 0)
	    తిరిగి &t_func_0_any;
	లేకపోతే
	    తిరిగి &t_func_any;
    }
    ఒకవేళ (ret_type == &t_సంఖ్య)
    {
	ఒకవేళ (argcount == 0)
	    తిరిగి &t_func_0_సంఖ్య;
	లేకపోతే
	    తిరిగి &t_func_సంఖ్య;
    }
    ఒకవేళ (ret_type == &t_string)
    {
	ఒకవేళ (argcount == 0)
	    తిరిగి &t_func_0_string;
	లేకపోతే
	    తిరిగి &t_func_string;
    }

    తిరిగి alloc_func_type(ret_type, argcount, type_gap);
}

/*
 * For a function type, reserve space for "argcount" argument types (including
 * vararg).
 */
    పూర్ణాంకం
func_type_కూడు_arg_types(
	type_T	    *functype,
	పూర్ణాంకం	    argcount,
	garray_T    *type_gap)
{
    // To make it easy to free the space needed for the argument types, add the
    // pointer to type_gap.
    ఒకవేళ (ga_grow(type_gap, 1) == FAIL)
	తిరిగి FAIL;
    functype->tt_వాదనలు = ALLOC_CLEAR_MULT(type_T *, argcount);
    ఒకవేళ (functype->tt_వాదనలు == ఖాళీ)
	తిరిగి FAIL;
    ((type_T **)type_gap->ga_దత్తాంశం)[type_gap->ga_len] =
						     (శూన్య *)functype->tt_వాదనలు;
    ++type_gap->ga_len;
    తిరిగి OK;
}

/*
 * Return TRUE if "type" is NULL, any or unknown.
 * This also works for const (comparing with &t_any and &t_unknown doesn't).
 */
    పూర్ణాంకం
type_any_or_unknown(type_T *type)
{
    తిరిగి type == ఖాళీ || type->tt_type == VAR_ANY
					|| type->tt_type == VAR_UNKNOWN;
}

/*
 * Get a type_T for a "special" typval in "tv".
 */
    స్థిర type_T *
special_typval2type(typval_T *tv)
{
    ఎంపిక (tv->vval.v_సంఖ్య)
    {
	స్థితి VVAL_NULL:
	    తిరిగి &t_null;

	స్థితి VVAL_NONE:
	    తిరిగి &t_none;

	స్థితి VVAL_TRUE:
	స్థితి VVAL_FALSE:
	    తిరిగి &t_bool;

	అప్రమేయ:
	    తిరిగి &t_unknown;
    }
}

/*
 * Get a type_T for a List typval in "tv".
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    స్థిర type_T *
list_typval2type(typval_T *tv, పూర్ణాంకం copyID, garray_T *type_gap, పూర్ణాంకం flags)
{
    list_T	*l = tv->vval.v_list;
    listitem_T	*li;
    type_T	*member_type = ఖాళీ;

    // An empty list has type list<unknown>, unless the type was specified
    // and is not list<any>.  This matters when assigning to a variable
    // with a specific list type.
    ఒకవేళ (l == ఖాళీ || (l->lv_మొదటి == ఖాళీ
		&& (l->lv_type == ఖాళీ || l->lv_type->tt_member == &t_any)))
	తిరిగి &t_list_ఖాళీలు;

    ఒకవేళ ((flags & TVTT_DO_MEMBER) == 0)
	తిరిగి &t_list_any;

    // If the type is list<any> go through the members, it may end up a
    // more specific type.
    ఒకవేళ (l->lv_type != ఖాళీ && (l->lv_మొదటి == ఖాళీ
		|| (flags & TVTT_మరిన్ని_SPECIFIC) == 0
		|| l->lv_type->tt_member != &t_any))
	// make a copy, lv_type may be freed if the list is freed
	తిరిగి copy_type_deep(l->lv_type, type_gap);

    ఒకవేళ (l->lv_మొదటి == &range_list_అంశం)
	తిరిగి &t_list_సంఖ్య;

    ఒకవేళ (l->lv_copyID == copyID)
	// avoid recursion
	తిరిగి &t_list_any;

    l->lv_copyID = copyID;

    // Use the common type of all members.
    member_type = typval2type(&l->lv_మొదటి->li_tv, copyID, type_gap,
							TVTT_DO_MEMBER);
    కొరకు (li = l->lv_మొదటి->li_తదుపరి; li != ఖాళీ; li = li->li_తదుపరి)
	common_type(typval2type(&li->li_tv, copyID, type_gap, TVTT_DO_MEMBER),
					member_type, &member_type, type_gap);

    // Reset copyID so that a shared reference to this list (not a circular
    // reference) can be processed again to get the correct type.
    l->lv_copyID = 0;

    తిరిగి get_list_type(member_type, type_gap);
}

/*
 * Get a type_T for a Tuple typval in "tv".
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    స్థిర type_T *
tuple_typval2type(typval_T *tv, పూర్ణాంకం copyID, garray_T *type_gap, పూర్ణాంకం flags)
{
    tuple_T	*tuple = tv->vval.v_tuple;
    పూర్ణాంకం		len = tuple_len(tuple);
    type_T	*type = ఖాళీ;

    // An empty tuple has type tuple<unknown>, unless the type was specified
    // and is not tuple<any>.  This matters when assigning to a variable
    // with a specific tuple type.
    ఒకవేళ (tuple == ఖాళీ || (len == 0 && (tuple->tv_type == ఖాళీ
				|| tuple->tv_type->tt_argcount == 0)))
	తిరిగి &t_tuple_ఖాళీలు;

    ఒకవేళ ((flags & TVTT_DO_MEMBER) == 0)
	తిరిగి &t_tuple_any;

    // If the type is tuple<any> go through the members, it may end up a
    // more specific type.
    ఒకవేళ (tuple->tv_type != ఖాళీ && (len == 0
					|| (flags & TVTT_మరిన్ని_SPECIFIC) == 0))
	// make a copy, tv_type may be freed if the tuple is freed
	తిరిగి copy_type_deep(tuple->tv_type, type_gap);

    ఒకవేళ (tuple->tv_copyID == copyID)
	// avoid recursion
	తిరిగి &t_tuple_any;

    tuple->tv_copyID = copyID;

    garray_T	tuple_types_ga;
    ga_init2(&tuple_types_ga, పరిమాణం(type_T *), 10);
    కొరకు (పూర్ణాంకం i = 0; i < len; i++)
    {
	type = typval2type(TUPLE_అంశం(tuple, i), copyID, type_gap,
							TVTT_DO_MEMBER);
	ఒకవేళ (ga_grow(&tuple_types_ga, 1) == FAIL)
	{
	    ga_clear(&tuple_types_ga);
	    // Reset copyID so that a shared reference to this tuple can be
	    // processed again.
	    tuple->tv_copyID = 0;
	    తిరిగి ఖాళీ;
	}
	((type_T **)tuple_types_ga.ga_దత్తాంశం)[tuple_types_ga.ga_len] = type;
	tuple_types_ga.ga_len++;
    }

    type_T *tuple_type = get_tuple_type(&tuple_types_ga, type_gap);
    ga_clear(&tuple_types_ga);

    // Reset copyID so that a shared reference to this tuple (not a circular
    // reference) can be processed again to get the correct type.
    tuple->tv_copyID = 0;

    తిరిగి tuple_type;
}

/*
 * Get a type_T for a Dict typval in "tv".
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    స్థిర type_T *
dict_typval2type(typval_T *tv, పూర్ణాంకం copyID, garray_T *type_gap, పూర్ణాంకం flags)
{
    dict_iterator_T	iter;
    typval_T		*విలువ;
    dict_T		*d = tv->vval.v_dict;
    type_T		*member_type = ఖాళీ;

    ఒకవేళ (d == ఖాళీ || (d->dv_hashtab.ht_వాడిన == 0 && d->dv_type == ఖాళీ))
	తిరిగి &t_dict_ఖాళీలు;

    ఒకవేళ ((flags & TVTT_DO_MEMBER) == 0)
	తిరిగి &t_dict_any;

    // If the type is dict<any> go through the members, it may end up a
    // more specific type.
    ఒకవేళ (d->dv_type != ఖాళీ && (d->dv_hashtab.ht_వాడిన == 0
		|| (flags & TVTT_మరిన్ని_SPECIFIC) == 0
		|| d->dv_type->tt_member != &t_any))
	తిరిగి d->dv_type;

    ఒకవేళ (d->dv_copyID == copyID)
	// avoid recursion
	తిరిగి &t_dict_any;

    d->dv_copyID = copyID;

    // Use the common type of all values.
    dict_iterate_ప్రారంభం(tv, &iter);
    dict_iterate_తదుపరి(&iter, &విలువ);
    member_type = typval2type(విలువ, copyID, type_gap, TVTT_DO_MEMBER);

    అయితే (dict_iterate_తదుపరి(&iter, &విలువ) != ఖాళీ)
	common_type(typval2type(విలువ, copyID, type_gap, TVTT_DO_MEMBER),
					member_type, &member_type, type_gap);

    // Reset copyID so that a shared reference to this dict (not a circular
    // reference) can be processed again to get the correct type.
    d->dv_copyID = 0;

    తిరిగి get_dict_type(member_type, type_gap);
}

/*
 * Get a type_T for a "partial" typval in "tv".
 */
    స్థిర type_T *
partial_typval2type(typval_T *tv, ufunc_T *ufunc, garray_T *type_gap)
{
    partial_T *pt = tv->vval.v_partial;
    type_T  *type;

    type = get_type_ptr(type_gap);
    ఒకవేళ (type == ఖాళీ)
	తిరిగి ఖాళీ;

    *type = *ufunc->uf_func_type;
    ఒకవేళ (type->tt_argcount >= 0 && pt->pt_argc > 0)
    {
	type->tt_argcount -= pt->pt_argc;
	type->tt_min_argcount -= pt->pt_argc;
	ఒకవేళ (type->tt_argcount > 0 && func_type_కూడు_arg_types(type,
		    type->tt_argcount, type_gap) == OK)
	    కొరకు (పూర్ణాంకం i = 0; i < type->tt_argcount; ++i)
		type->tt_వాదనలు[i] =
		    ufunc->uf_func_type->tt_వాదనలు[i + pt->pt_argc];
    }
    ఒకవేళ (pt->pt_func != ఖాళీ)
	type->tt_member = pt->pt_func->uf_ret_type;

    తిరిగి type;
}

/*
 * Get a type_T for a "class" or an "object" typval in "tv".
 */
    స్థిర type_T *
oc_typval2type(typval_T *tv)
{
    ఒకవేళ (tv->v_type == VAR_CLASS)
    {
	ఒకవేళ (tv->vval.v_class == ఖాళీ)
	    తిరిగి &t_class;

	తిరిగి &tv->vval.v_class->class_type;
    }

    ఒకవేళ (tv->vval.v_object != ఖాళీ)
	తిరిగి &tv->vval.v_object->obj_class->class_object_type;

    తిరిగి &t_object_any;
}

/*
 * Get a type_T for a "function" or a "partial"
 */
    స్థిర type_T *
fp_typval2type(typval_T *tv, garray_T *type_gap)
{
    char_u	*name = ఖాళీ;
    ufunc_T	*ufunc = ఖాళీ;
    type_T	*type;
    type_T	*member_type = ఖాళీ;
    పూర్ణాంకం		argcount = 0;
    పూర్ణాంకం		min_argcount = 0;

    ఒకవేళ (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != ఖాళీ)
    {
	ఒకవేళ (tv->vval.v_partial->pt_func != ఖాళీ)
	    ufunc = tv->vval.v_partial->pt_func;
	లేకపోతే
	    name = tv->vval.v_partial->pt_name;
    }
    లేకపోతే
	name = tv->vval.v_string;

    ఒకవేళ (name == ఖాళీ && ufunc == ఖాళీ)
	తిరిగి &t_func_unknown;

    ఒకవేళ (name != ఖాళీ)
    {
	పూర్ణాంకం idx = కనుగొను_అంతర్గత_func(name);

	ఒకవేళ (idx >= 0)
	{
	    type_T *decl_type;  //వాడని

	    అంతర్గత_func_get_argcount(idx, &argcount, &min_argcount);
	    member_type = అంతర్గత_func_ret_type(idx, 0, ఖాళీ, &decl_type,
		    type_gap);
	}
	లేకపోతే
	{
	    // Check if name contains "<".  If it does, then replace "<" with
	    // NUL and lookup the function and then look up the specific
	    // generic function using the supplied types.
	    char_u *p = generic_func_కనుగొను_open_bracket(name);
	    ఒకవేళ (p == ఖాళీ)
		ufunc = కనుగొను_func(name, FALSE);
	    లేకపోతే
	    {
		// generic function
		char_u	cc = *p;

		*p = NUL;
		ufunc = కనుగొను_func(name, FALSE);
		*p = cc;
		ఒకవేళ (ufunc != ఖాళీ && IS_GENERIC_FUNC(ufunc))
		{
		    ufunc = కనుగొను_generic_func(ufunc, name, &p);
		    ఒకవేళ (ufunc == ఖాళీ)
			తిరిగి ఖాళీ;
		}
	    }
	}
    }
    ఒకవేళ (ufunc != ఖాళీ)
    {
	// May need to get the argument types from default values by
	// compiling the function.
	ఒకవేళ (ufunc->uf_def_స్థితి == UF_గమ్యం_BE_COMPILED
		&& compile_def_function(ufunc, TRUE, CT_NONE, ఖాళీ)
		== FAIL)
	    తిరిగి ఖాళీ;
	ఒకవేళ (ufunc->uf_func_type == ఖాళీ)
	    సమితి_function_type(ufunc);
	ఒకవేళ (ufunc->uf_func_type != ఖాళీ)
	{
	    ఒకవేళ (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != ఖాళీ)
		తిరిగి partial_typval2type(tv, ufunc, type_gap);
	    తిరిగి ufunc->uf_func_type;
	}
    }

    type = get_type_ptr(type_gap);
    ఒకవేళ (type == ఖాళీ)
	తిరిగి ఖాళీ;
    type->tt_type = tv->v_type;
    type->tt_argcount = argcount;
    type->tt_min_argcount = min_argcount;
    ఒకవేళ (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != ఖాళీ
					&& tv->vval.v_partial->pt_argc > 0)
    {
	type->tt_argcount -= tv->vval.v_partial->pt_argc;
	type->tt_min_argcount -= tv->vval.v_partial->pt_argc;
    }
    type->tt_member = member_type;

    తిరిగి type;
}

/*
 * Get a type_T for a typval_T.
 * "type_gap" is used to temporarily create types in.
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    స్థిర type_T *
typval2type_int(typval_T *tv, పూర్ణాంకం copyID, garray_T *type_gap, పూర్ణాంకం flags)
{
    ఎంపిక (tv->v_type)
    {
	స్థితి VAR_UNKNOWN:
	    తిరిగి &t_unknown;

	స్థితి VAR_ANY:
	    తిరిగి &t_any;

	స్థితి VAR_VOID:
	    తిరిగి &t_void;

	స్థితి VAR_BOOL:
	    తిరిగి &t_bool;

	స్థితి VAR_SPECIAL:
	    తిరిగి special_typval2type(tv);

	స్థితి VAR_సంఖ్య:
	    తిరిగి &t_సంఖ్య;

	స్థితి VAR_FLOAT:
	    తిరిగి &t_float;

	స్థితి VAR_STRING:
	    తిరిగి &t_string;

	స్థితి VAR_BLOB:
	    ఒకవేళ (tv->vval.v_blob == ఖాళీ)
		తిరిగి &t_blob_null;
	    తిరిగి &t_blob;

	స్థితి VAR_LIST:
	    తిరిగి list_typval2type(tv, copyID, type_gap, flags);

	స్థితి VAR_TUPLE:
	    తిరిగి tuple_typval2type(tv, copyID, type_gap, flags);

	స్థితి VAR_DICT:
	    తిరిగి dict_typval2type(tv, copyID, type_gap, flags);

	స్థితి VAR_పని:
	    తిరిగి &t_పని;

	స్థితి VAR_CHANNEL:
	    తిరిగి &t_channel;

	స్థితి VAR_CLASS:
	స్థితి VAR_OBJECT:
	    తిరిగి oc_typval2type(tv);

	స్థితి VAR_TYPEALIAS:
	    తిరిగి &t_typealias;

	స్థితి VAR_FUNC:
	స్థితి VAR_PARTIAL:
	    తిరిగి fp_typval2type(tv, type_gap);

	స్థితి VAR_INSTR:
	అప్రమేయ:
	    ఆపు;
    }

    తిరిగి ఖాళీ;
}

/*
 * Return TRUE if "tv" is not a bool but should be converted to bool.
 */
    పూర్ణాంకం
need_convert_గమ్యం_bool(type_T *type, typval_T *tv)
{
    తిరిగి type != ఖాళీ && type == &t_bool && tv->v_type != VAR_BOOL
	    && (tv->v_type == VAR_సంఖ్య
		       && (tv->vval.v_సంఖ్య == 0 || tv->vval.v_సంఖ్య == 1));
}

/*
 * Get a type_T for a typval_T.
 * "type_list" is used to temporarily create types in.
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the most specific member type.
 */
    type_T *
typval2type(typval_T *tv, పూర్ణాంకం copyID, garray_T *type_gap, పూర్ణాంకం flags)
{
    type_T *type = typval2type_int(tv, copyID, type_gap, flags);

    ఒకవేళ (type == ఖాళీ)
	తిరిగి ఖాళీ;

    ఒకవేళ (type != &t_bool && (tv->v_type == VAR_సంఖ్య
		&& (tv->vval.v_సంఖ్య == 0 || tv->vval.v_సంఖ్య == 1)))
	// Number 0 and 1 and expression with "&&" or "||" can also be used
	// for bool.
	type = &t_సంఖ్య_bool;
    లేకపోతే ఒకవేళ (type != &t_float && tv->v_type == VAR_సంఖ్య)
	// A number can also be used for float.
	type = &t_సంఖ్య_float;
    తిరిగి type;
}

/*
 * Return TRUE if "type" can be used for a variable declaration.
 * Give an error and return FALSE if not.
 */
    పూర్ణాంకం
valid_declaration_type(type_T *type)
{
    ఒకవేళ (type->tt_type == VAR_SPECIAL  // null, none
	    || type->tt_type == VAR_VOID)
    {
	అక్షరం *tofree = ఖాళీ;
	అక్షరం *name = type_name(type, &tofree);
	semsg(_(e_invalid_type_లోపల_variable_declaration_str), name);
	vim_free(tofree);
	తిరిగి FALSE;
    }
    తిరిగి TRUE;
}

/*
 * Get a type_T for a typval_T, used for v: variables.
 * "type_list" is used to temporarily create types in.
 */
    type_T *
typval2type_vimvar(typval_T *tv, garray_T *type_gap)
{
    ఒకవేళ (tv->v_type == VAR_LIST)  // e.g. for v:oldfiles
	తిరిగి &t_list_string;
    ఒకవేళ (tv->v_type == VAR_DICT)  // e.g. for v:event
	తిరిగి &t_dict_any;
    తిరిగి typval2type(tv, get_copyID(), type_gap, TVTT_DO_MEMBER);
}

    పూర్ణాంకం
check_typval_arg_type(
	type_T	    *expected,
	typval_T    *actual_tv,
	అక్షరం	    *func_name,
	పూర్ణాంకం	    arg_idx)
{
    where_T	where = WHERE_INIT;

    ఒకవేళ (arg_idx > 0)
    {
	where.wt_సూచీ = arg_idx;
	where.wt_రకం = WT_వాదం;
    }
    where.wt_func_name = func_name;
    తిరిగి check_typval_type(expected, actual_tv, where);
}

/*
 * Return true if "expected" is list<any> or dict<any> and "actual_tv" is a
 * list or dict of that kind, which matches whatever its values are.
 */
    స్థిర bool
matches_listdict_any(type_T *expected, typval_T *actual_tv)
{
    తిరిగి (expected->tt_type == VAR_LIST || expected->tt_type == VAR_DICT)
	    && actual_tv->v_type == expected->tt_type
	    && expected->tt_member != ఖాళీ
	    && (expected->tt_member->tt_type == VAR_ANY
		|| expected->tt_member->tt_type == VAR_UNKNOWN);
}

/*
 * Return FAIL if "expected" and "actual" don't match.
 * When "argidx" > 0 it is included in the error message.
 */
    పూర్ణాంకం
check_typval_type(type_T *expected, typval_T *actual_tv, where_T where)
{
    garray_T	type_list;
    type_T	*actual_type;
    పూర్ణాంకం		res = FAIL;

    ఒకవేళ (expected == ఖాళీ)
	తిరిగి OK;  // didn't expect anything.

    // No need to go through the values of a possibly big list or dict.
    ఒకవేళ (matches_listdict_any(expected, actual_tv))
	తిరిగి OK;

    ga_init2(&type_list, పరిమాణం(type_T *), 10);

    // A null_function and null_partial are special cases, they can be used to
    // clear a variable.
    ఒకవేళ ((actual_tv->v_type == VAR_FUNC && actual_tv->vval.v_string == ఖాళీ)
	    || (actual_tv->v_type == VAR_PARTIAL
					 && actual_tv->vval.v_partial == ఖాళీ))
	actual_type = &t_func_unknown;
    లేకపోతే
	// When the actual type is list<any> or dict<any> go through the values
	// to possibly get a more specific type.
	actual_type = typval2type(actual_tv, get_copyID(), &type_list,
					  TVTT_DO_MEMBER | TVTT_మరిన్ని_SPECIFIC);
    ఒకవేళ (actual_type != ఖాళీ)
    {
	res = check_type_maybe(expected, actual_type, TRUE, where);
	ఒకవేళ (res == MAYBE && !(actual_type->tt_type == VAR_FUNC
				      && (actual_type->tt_member == &t_unknown
					  || actual_type->tt_member == ఖాళీ)))
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
    తిరిగి res;
}

    శూన్య
arg_type_mismatch(type_T *expected, type_T *actual, పూర్ణాంకం arg_idx)
{
    where_T	where = WHERE_INIT;

    ఒకవేళ (arg_idx > 0)
    {
	where.wt_సూచీ = arg_idx;
	where.wt_రకం = WT_వాదం;
    }
    type_mismatch_where(expected, actual, where);
}

    శూన్య
type_mismatch_where(type_T *expected, type_T *actual, where_T where)
{
    అక్షరం *tofree1, *tofree2;
    అక్షరం *typename1 = type_name(expected, &tofree1);
    అక్షరం *typename2 = type_name(actual, &tofree2);

    ఎంపిక (where.wt_రకం)
    {
	స్థితి WT_MEMBER:
	    semsg(_(e_variable_str_type_mismatch_expected_str_but_got_str),
		    where.wt_func_name, typename1, typename2);
	    ఆపు;
	స్థితి WT_పద్ధతి:
	స్థితి WT_పద్ధతి_ARG:
	స్థితి WT_పద్ధతి_RETURN:
	    semsg(_(e_పద్ధతి_str_type_mismatch_expected_str_but_got_str),
		    where.wt_func_name, typename1, typename2);
	    ఆపు;
	స్థితి WT_VARIABLE:
	    ఒకవేళ (where.wt_func_name == ఖాళీ)
		semsg(_(e_variable_nr_type_mismatch_expected_str_but_got_str),
			where.wt_సూచీ, typename1, typename2);
	    లేకపోతే
		semsg(_(e_variable_nr_type_mismatch_expected_str_but_got_str_లోపల_str),
			where.wt_సూచీ, typename1, typename2, where.wt_func_name);
	    ఆపు;
	స్థితి WT_వాదం:
	    ఒకవేళ (where.wt_func_name == ఖాళీ)
		semsg(_(e_వాదం_nr_type_mismatch_expected_str_but_got_str),
			where.wt_సూచీ, typename1, typename2);
	    లేకపోతే
		semsg(_(e_వాదం_nr_type_mismatch_expected_str_but_got_str_లోపల_str),
			where.wt_సూచీ, typename1, typename2, where.wt_func_name);
	    ఆపు;
	స్థితి WT_CAST:
	స్థితి WT_UNKNOWN:
	    ఒకవేళ (where.wt_func_name == ఖాళీ)
		semsg(_(e_type_mismatch_expected_str_but_got_str),
			typename1, typename2);
	    లేకపోతే
		semsg(_(e_type_mismatch_expected_str_but_got_str_లోపల_str),
			typename1, typename2, where.wt_func_name);
	    ఆపు;
    }

    vim_free(tofree1);
    vim_free(tofree2);
}

/*
 * Check if the expected and actual types match for a tuple
 */
    స్థిర పూర్ణాంకం
check_tuple_type_maybe(
    type_T	*expected,
    type_T	*actual,
    where_T	where)
{
    ఒకవేళ (expected->tt_argcount == -1 || actual->tt_argcount == -1
		|| expected->tt_వాదనలు == ఖాళీ || actual->tt_వాదనలు == ఖాళీ)
	తిరిగి OK;

    // For a non-variadic tuple, the number of items must match
    ఒకవేళ (!(expected->tt_flags & TTFLAG_VARARGS)
	    && expected->tt_argcount != actual->tt_argcount)
	తిరిగి FAIL;

    // compare the type of each tuple item
    కొరకు (పూర్ణాంకం i = 0; i < actual->tt_argcount; ++i)
    {
	type_T	*exp_type;
	type_T	*actual_type;

	ఒకవేళ (expected->tt_flags & TTFLAG_VARARGS)
	{
	    ఒకవేళ (i < expected->tt_argcount - 1)
		exp_type = expected->tt_వాదనలు[i];
	    లేకపోతే
		// For a variadic tuple, the last type is a List.  So use the
		// List member type.
		exp_type = expected->tt_వాదనలు[expected->tt_argcount - 1]->tt_member;
	}
	లేకపోతే
	    exp_type = expected->tt_వాదనలు[i];

	ఒకవేళ (actual->tt_flags & TTFLAG_VARARGS)
	{
	    ఒకవేళ (i < actual->tt_argcount - 1)
		actual_type = actual->tt_వాదనలు[i];
	    లేకపోతే
		// For a variadic tuple, the last type is a List.  So use the
		// List member type.
		actual_type = actual->tt_వాదనలు[actual->tt_argcount - 1]->tt_member;
	}
	లేకపోతే
	    actual_type = actual->tt_వాదనలు[i];

	// Allow for using "any" type for a tuple item
	ఒకవేళ (actual->tt_వాదనలు[i] != &t_any && check_type(exp_type, actual_type,
						FALSE, where) == FAIL)
	    తిరిగి FAIL;
    }

    తిరిగి OK;
}

/*
 * Check if the expected and actual types match for a function
 * Returns OK if "expected" and "actual" are matching function types.
 * Returns FAIL if "expected" and "actual" are different types.
 * Returns MAYBE when a runtime type check is needed.
 */
    స్థిర పూర్ణాంకం
check_func_type_maybe(
    type_T	*expected,
    type_T	*actual,
    where_T	where)
{
    పూర్ణాంకం ret = OK;

    // If the return type is unknown it can be anything, including
    // nothing, thus there is no point in checking.
    ఒకవేళ (expected->tt_member != &t_unknown)
    {
	ఒకవేళ (actual->tt_member != ఖాళీ
		&& actual->tt_member != &t_unknown)
	{
	    where_T  func_where = where;

	    func_where.wt_రకం = WT_పద్ధతి_RETURN;
	    ret = check_type_maybe(expected->tt_member,
		    actual->tt_member, FALSE,
		    func_where);
	}
	లేకపోతే
	    ret = MAYBE;
    }
    ఒకవేళ (ret != FAIL
	    && ((expected->tt_flags & TTFLAG_VARARGS)
		!= (actual->tt_flags & TTFLAG_VARARGS))
	    && expected->tt_argcount != -1)
	ret = FAIL;
    ఒకవేళ (ret != FAIL && expected->tt_argcount != -1
	    && actual->tt_min_argcount != -1
	    && (actual->tt_argcount == -1
		|| (actual->tt_argcount < expected->tt_min_argcount
		    || actual->tt_argcount > expected->tt_argcount)))
	ret = FAIL;
    ఒకవేళ (ret != FAIL && expected->tt_వాదనలు != ఖాళీ
	    && actual->tt_వాదనలు != ఖాళీ)
    {
	పూర్ణాంకం i;

	కొరకు (i = 0; i < expected->tt_argcount
		&& i < actual->tt_argcount; ++i)
	{
	    where_T  func_where = where;
	    func_where.wt_రకం = WT_పద్ధతి_ARG;

	    // Allow for using "any" argument type, lambda's have them.
	    ఒకవేళ (actual->tt_వాదనలు[i] != &t_any && check_type(
			expected->tt_వాదనలు[i], actual->tt_వాదనలు[i], FALSE,
			func_where) == FAIL)
	    {
		ret = FAIL;
		ఆపు;
	    }
	}
    }
    ఒకవేళ (ret == OK && expected->tt_argcount >= 0
	    && actual->tt_argcount == -1)
	// check the argument count at runtime
	ret = MAYBE;

    తిరిగి ret;
}

/*
 * Check if the expected and actual types match for an object
 * Returns OK if "expected" and "actual" are matching object types.
 * Returns FAIL if "expected" and "actual" are different types.
 * Returns MAYBE when a runtime type check is needed.
 */
    స్థిర పూర్ణాంకం
check_object_type_maybe(
    type_T	*expected,
    type_T	*actual,
    where_T	where)
{
    పూర్ణాంకం ret = OK;

    ఒకవేళ (actual->tt_type == VAR_ANY)
	తిరిగి MAYBE;	// use runtime type check
    ఒకవేళ (actual->tt_type != VAR_OBJECT)
	తిరిగి FAIL;	// don't use tt_class
    ఒకవేళ (actual->tt_class == ఖాళీ)    // null object
	తిరిగి OK;
    // t_object_any matches any object except for an enum item
    ఒకవేళ (expected == &t_object_any && !IS_ENUM(actual->tt_class))
	తిరిగి OK;

    // For object method arguments, do a invariant type check in
    // an extended class.  For all others, do a covariance type check.
    ఒకవేళ (where.wt_రకం == WT_పద్ధతి_ARG)
    {
	ఒకవేళ (actual->tt_class != expected->tt_class)
	    ret = FAIL;
    }
    లేకపోతే ఒకవేళ (!class_instance_యొక్క(actual->tt_class, expected->tt_class))
    {
	// Check if this is an up-cast, if so we'll have to check the type at
	// runtime.
	ఒకవేళ (where.wt_రకం == WT_CAST &&
		class_instance_యొక్క(expected->tt_class, actual->tt_class))
	    ret = MAYBE;
	లేకపోతే
	    ret = FAIL;
    }

    తిరిగి ret;
}

/*
 * Check if the expected and actual types match.
 * Does not allow for assigning "any" to a specific type.
 * When "argidx" > 0 it is included in the error message.
 * Return OK if types match.
 * Return FAIL if types do not match.
 */
    పూర్ణాంకం
check_type(
	type_T	*expected,
	type_T	*actual,
	పూర్ణాంకం	give_సందేశం_2,
	where_T where)
{
    పూర్ణాంకం ret = check_type_maybe(expected, actual, give_సందేశం_2, where);

    తిరిగి ret == MAYBE ? OK : ret;
}

/*
 * As check_type() but return MAYBE when a runtime type check should be used
 * when compiling.
 */
    పూర్ణాంకం
check_type_maybe(
	type_T	*expected,
	type_T	*actual,
	పూర్ణాంకం	give_సందేశం_2,
	where_T where)
{
    పూర్ణాంకం ret = OK;

    // When expected is "unknown" we accept any actual type.
    // When expected is "any" we accept any actual type except "void".
    ఒకవేళ (expected->tt_type != VAR_UNKNOWN
	    && !(expected->tt_type == VAR_ANY && actual->tt_type != VAR_VOID))

    {
	// tt_type should match, except that a "partial" can be assigned to a
	// variable with type "func".
	// And "unknown" (using global variable) and "any" need a runtime type
	// check.
	ఒకవేళ (!(expected->tt_type == actual->tt_type
		    || actual->tt_type == VAR_UNKNOWN
		    || actual->tt_type == VAR_ANY
		    || (expected->tt_type == VAR_FUNC
					   && actual->tt_type == VAR_PARTIAL)))
	{
	    ఒకవేళ (expected->tt_type == VAR_BOOL
					&& (actual->tt_flags & TTFLAG_BOOL_OK))
		// Using number 0 or 1 for bool is OK.
		తిరిగి OK;
	    ఒకవేళ (expected->tt_type == VAR_FLOAT
		    && actual->tt_type == VAR_సంఖ్య
		    && ((expected->tt_flags & TTFLAG_సంఖ్య_OK)
			     || (actual->tt_flags & TTFLAG_FLOAT_OK)))
		// Using a number where a float is expected is OK here.
		తిరిగి OK;
	    ఒకవేళ (expected->tt_type == VAR_LIST
		    && actual->tt_type == VAR_TUPLE
		    && (expected->tt_flags & TTFLAG_TUPLE_OK))
		// Using a tuple where a list is expected is OK here.
		తిరిగి OK;
	    ఒకవేళ (give_సందేశం_2)
		type_mismatch_where(expected, actual, where);
	    తిరిగి FAIL;
	}
	ఒకవేళ (expected->tt_type == VAR_DICT || expected->tt_type == VAR_LIST)
	{
	    // "unknown" is used for an empty list or dict
	    ఒకవేళ (actual->tt_member != ఖాళీ && actual->tt_member != &t_unknown)
		ret = check_type_maybe(expected->tt_member, actual->tt_member,
								 FALSE, where);
	}
	లేకపోతే ఒకవేళ (expected->tt_type == VAR_TUPLE && actual != &t_any)
	    ret =  check_tuple_type_maybe(expected, actual, where);
	లేకపోతే ఒకవేళ (expected->tt_type == VAR_FUNC && actual != &t_any)
	    ret = check_func_type_maybe(expected, actual, where);
	లేకపోతే ఒకవేళ (expected->tt_type == VAR_OBJECT)
	    ret = check_object_type_maybe(expected, actual, where);

	ఒకవేళ (ret == FAIL && give_సందేశం_2)
	    type_mismatch_where(expected, actual, where);
    }

    ఒకవేళ (ret == OK && expected->tt_type != VAR_UNKNOWN
	    && expected->tt_type != VAR_ANY
	    && (actual->tt_type == VAR_UNKNOWN || actual->tt_type == VAR_ANY))
	// check the type at runtime
	ret = MAYBE;

    తిరిగి ret;
}

/*
 * Check that the arguments of "type" match "argvars[argcount]".
 * "base_tv" is from "expr->Func()".
 * Return OK/FAIL.
 */
    పూర్ణాంకం
check_వాదం_types(
	type_T	    *type,
	typval_T    *argvars,
	పూర్ణాంకం	    argcount,
	typval_T    *పునాది_tv,
	char_u	    *name)
{
    పూర్ణాంకం	    varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;
    పూర్ణాంకం	    i;
    పూర్ణాంకం	    totcount = argcount + (పునాది_tv == ఖాళీ ? 0 : 1);

    ఒకవేళ (type->tt_type != VAR_FUNC && type->tt_type != VAR_PARTIAL)
	తిరిగి OK;  // just in case
    ఒకవేళ (totcount < type->tt_min_argcount - varargs)
    {
	emsg_funcname(e_not_enough_arguments_for_function_str, name);
	తిరిగి FAIL;
    }
    ఒకవేళ (!varargs && type->tt_argcount >= 0 && totcount > type->tt_argcount)
    {
	emsg_funcname(e_too_many_arguments_for_function_str, name);
	తిరిగి FAIL;
    }
    ఒకవేళ (type->tt_వాదనలు == ఖాళీ)
	తిరిగి OK;  // cannot check


    కొరకు (i = 0; i < totcount; ++i)
    {
	type_T	    *expected;
	typval_T    *tv;

	ఒకవేళ (పునాది_tv != ఖాళీ)
	{
	    ఒకవేళ (i == 0)
		tv = పునాది_tv;
	    లేకపోతే
		tv = &argvars[i - 1];
	}
	లేకపోతే
	    tv = &argvars[i];
	ఒకవేళ (varargs && i >= type->tt_argcount - 1)
	{
	    expected = type->tt_వాదనలు[type->tt_argcount - 1];
	    ఒకవేళ (expected != ఖాళీ && expected->tt_type == VAR_LIST)
		expected = expected->tt_member;
	    ఒకవేళ (expected == ఖాళీ)
		expected = &t_any;
	}
	లేకపోతే
	    expected = type->tt_వాదనలు[i];

	// check the type, unless the value is v:none
	ఒకవేళ ((tv->v_type != VAR_SPECIAL || tv->vval.v_సంఖ్య != VVAL_NONE)
		   && check_typval_arg_type(expected, tv, ఖాళీ, i + 1) == FAIL)
	    తిరిగి FAIL;
    }
    తిరిగి OK;
}

// Maximum nesting of types, to avoid running out of stack space.  E.g.
// "list<list<number>>" is nested twice.
#నిర్వచించు MAX_TYPE_లోతు	1000

// Current number of levels of parse_type() and the deepest level reached while
// parsing the current type.  The nesting is one less than the number of
// levels.
స్థిర పూర్ణాంకం	parse_లోతు = 0;
స్థిర పూర్ణాంకం	parse_max_లోతు = 0;

/*
 * Skip over type in list<type>, dict<type> or tuple<type>.
 * Returns a pointer to the character after the type.  "syn_error" is set to
 * TRUE on syntax error.
 */
    స్థిర char_u *
skip_member_type(char_u *ప్రారంభం, char_u *p, పూర్ణాంకం *syn_error)
{
    ఒకవేళ (STRNCMP("tuple", ప్రారంభం, 5) == 0)
    {
	// handle tuple<{type1}, {type2}, ....<type>>
	p = skipwhite(p + 1);
	అయితే (*p != '>' && *p != NUL)
	{
	    char_u *sp = p;

	    ఒకవేళ (STRNCMP(p, "...", 3) == 0)
		p += 3;
	    p = skip_type(p, TRUE);
	    ఒకవేళ (p == sp)
	    {
		*syn_error = TRUE;
		తిరిగి p;  // syntax error
	    }
	    ఒకవేళ (*p == ',')
		p = skipwhite(p + 1);
	}
	ఒకవేళ (*p == '>')
	    p++;
    }
    లేకపోతే
    {
	p = skipwhite(p);
	p = skip_type(skipwhite(p + 1), FALSE);
	p = skipwhite(p);
	ఒకవేళ (*p == '>')
	    ++p;
    }

    తిరిగి p;
}

/*
 * Skip over a function type.  Returns a pointer to the character after the
 * type.  "syn_error" is set to TRUE on syntax error.
 */
    స్థిర char_u *
skip_func_type(char_u *p, పూర్ణాంకం *syn_error)
{
    ఒకవేళ (*p == '(')
    {
	// handle func(args): type
	++p;
	అయితే (*p != ')' && *p != NUL)
	{
	    char_u *sp = p;

	    ఒకవేళ (STRNCMP(p, "...", 3) == 0)
		p += 3;
	    p = skip_type(p, TRUE);
	    ఒకవేళ (p == sp)
	    {
		*syn_error = TRUE;
		తిరిగి p;  // syntax error
	    }
	    ఒకవేళ (*p == ',')
		p = skipwhite(p + 1);
	}
	ఒకవేళ (*p == ')')
	{
	    ఒకవేళ (p[1] == ':')
		p = skip_type(skipwhite(p + 2), FALSE);
	    లేకపోతే
		++p;
	}
    }
    లేకపోతే
    {
	// handle func: return_type
	p = skip_type(skipwhite(p + 1), FALSE);
    }

    తిరిగి p;
}

/*
 * Inner part of skip_type().
 */
    స్థిర char_u *
skip_type_inner(char_u *ప్రారంభం, పూర్ణాంకం optional)
{
    char_u	*p = ప్రారంభం;
    పూర్ణాంకం		syn_error = FALSE;

    ఒకవేళ (optional && *p == '?')
	++p;

    // Also skip over "." for imported classes: "import.ClassName".
    అయితే (ASCII_ISALNUM(*p) || *p == '_' || *p == '.')
	++p;

    // Skip over "<type>"; this is permissive about white space.
    ఒకవేళ (*skipwhite(p) == '<')
    {
	p = skip_member_type(ప్రారంభం, p, &syn_error);
	ఒకవేళ (syn_error)
	    తిరిగి p;
    }
    లేకపోతే ఒకవేళ ((*p == '(' || (*p == ':' && VIM_ISWHITE(p[1])))
					     && STRNCMP("func", ప్రారంభం, 4) == 0)
    {
	// skip over function type
	p = skip_func_type(p, &syn_error);
	ఒకవేళ (syn_error)
	    తిరిగి p;
    }

    తిరిగి p;
}

/*
 * Skip over a type definition and return a pointer to just after it.
 * When "optional" is TRUE then a leading "?" is accepted.
 */
    char_u *
skip_type(char_u *ప్రారంభం, పూర్ణాంకం optional)
{
    స్థిర పూర్ణాంకం	skip_లోతు = 0;
    char_u	*p = ప్రారంభం;

    // Too deeply nested: skip to the matching ">" without recursing, an
    // error is given when parsing the type.
    ఒకవేళ (skip_లోతు > MAX_TYPE_లోతు)
    {
	పూర్ణాంకం	లోతు = 0;

	కొరకు ( ; *p != NUL; ++p)
	{
	    ఒకవేళ (*p == '<')
		++లోతు;
	    లేకపోతే ఒకవేళ (*p == '>')
	    {
		ఒకవేళ (లోతు == 0)
		    ఆపు;
		ఒకవేళ (--లోతు == 0)
		    తిరిగి p + 1;
	    }
	    లేకపోతే ఒకవేళ (లోతు == 0 && !ASCII_ISALNUM(*p) && *p != '_'
								&& *p != '.')
		ఆపు;
	}
	తిరిగి p;
    }

    ++skip_లోతు;
    p = skip_type_inner(ప్రారంభం, optional);
    --skip_లోతు;
    తిరిగి p;
}

/*
 * Parse the member type: "<type>" and return "type" with the member set.
 * Use "type_gap" if a new type needs to be added.
 * "info" is extra information for an error message.
 * Returns NULL in case of failure.
 */
    స్థిర type_T *
parse_type_member(
	char_u	    **arg,
	type_T	    *type,
	garray_T    *type_gap,
	పూర్ణాంకం	    give_error,
	అక్షరం	    *info,
	ufunc_T	    *ufunc,
	cctx_T	    *cctx)
{
    char_u  *arg_ప్రారంభం = *arg;
    type_T  *member_type;
    పూర్ణాంకం	    prev_called_emsg = called_emsg;

    ఒకవేళ (**arg != '<')
    {
	ఒకవేళ (give_error)
	{
	    ఒకవేళ (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    లేకపోతే
		semsg(_(e_missing_type_after_str), info);
	}
	తిరిగి ఖాళీ;
    }
    *arg = skipwhite(*arg + 1);

    member_type = parse_type(arg, type_gap, ufunc, cctx, give_error);
    ఒకవేళ (member_type == ఖాళీ || !valid_declaration_type(member_type))
	తిరిగి ఖాళీ;

    *arg = skipwhite(*arg);
    ఒకవేళ (**arg != '>' && called_emsg == prev_called_emsg)
    {
	ఒకవేళ (give_error)
	    semsg(_(e_missing_gt_after_type_str), arg_ప్రారంభం);
	తిరిగి ఖాళీ;
    }
    ++*arg;

    ఒకవేళ (type->tt_type == VAR_LIST)
	తిరిగి get_list_type(member_type, type_gap);
    తిరిగి get_dict_type(member_type, type_gap);
}

/*
 * Parse a "func" type at "*arg" and advance over it.
 * When "give_error" is TRUE give error messages, otherwise be quiet.
 * Return NULL for failure.
 */
    స్థిర type_T *
parse_type_func(
    char_u	**arg,
    size_t	len,
    garray_T	*type_gap,
    పూర్ణాంకం		give_error,
    ufunc_T	*ufunc,
    cctx_T	*cctx)
{
    char_u  *p;
    type_T  *type;
    type_T  *ret_type = &t_unknown;
    పూర్ణాంకం	argcount = -1;
    పూర్ణాంకం	flags = 0;
    పూర్ణాంకం	మొదటి_optional = -1;
    type_T	*arg_type[MAX_FUNC_వాదనలు + 1];

    // func({type}, ...{type}): {type}
    *arg += len;
    ఒకవేళ (**arg == '(')
    {
	// "func" may or may not return a value, "func()" does
	// not return a value.
	ret_type = &t_void;

	p = ++*arg;
	argcount = 0;
	అయితే (*p != NUL && *p != ')')
	{
	    ఒకవేళ (*p == '?')
	    {
		ఒకవేళ (మొదటి_optional == -1)
		    మొదటి_optional = argcount;
		++p;
	    }
	    లేకపోతే ఒకవేళ (STRNCMP(p, "...", 3) == 0)
	    {
		flags |= TTFLAG_VARARGS;
		p += 3;
	    }
	    లేకపోతే ఒకవేళ (మొదటి_optional != -1)
	    {
		ఒకవేళ (give_error)
		    emsg(_(e_mandatory_వాదం_after_optional_వాదం));
		తిరిగి ఖాళీ;
	    }

	    type = parse_type(&p, type_gap, ufunc, cctx, give_error);
	    ఒకవేళ (type == ఖాళీ || !valid_declaration_type(type))
		తిరిగి ఖాళీ;
	    ఒకవేళ ((flags & TTFLAG_VARARGS) != 0 && type->tt_type != VAR_LIST)
	    {
		అక్షరం *tofree;
		semsg(_(e_variable_arguments_type_must_be_list_str),
			type_name(type, &tofree));
		vim_free(tofree);
		తిరిగి ఖాళీ;
	    }
	    arg_type[argcount++] = type;

	    // Nothing comes after "...{type}".
	    ఒకవేళ (flags & TTFLAG_VARARGS)
		ఆపు;

	    ఒకవేళ (*p != ',' && *skipwhite(p) == ',')
	    {
		ఒకవేళ (give_error)
		    semsg(_(e_no_white_space_allowed_before_str_str),
			    ",", p);
		తిరిగి ఖాళీ;
	    }
	    ఒకవేళ (*p == ',')
	    {
		++p;
		ఒకవేళ (!VIM_ISWHITE(*p))
		{
		    ఒకవేళ (give_error)
			semsg(_(e_white_space_required_after_str_str),
				",", p - 1);
		    తిరిగి ఖాళీ;
		}
	    }
	    p = skipwhite(p);
	    ఒకవేళ (argcount == MAX_FUNC_వాదనలు)
	    {
		ఒకవేళ (give_error)
		    emsg(_(e_too_many_వాదం_types));
		తిరిగి ఖాళీ;
	    }
	}

	p = skipwhite(p);
	ఒకవేళ (*p != ')')
	{
	    ఒకవేళ (give_error)
		emsg(_(e_missing_closing_paren));
	    తిరిగి ఖాళీ;
	}
	*arg = p + 1;
    }
    ఒకవేళ (**arg == ':')
    {
	// parse return type
	++*arg;
	ఒకవేళ (!VIM_ISWHITE(**arg) && give_error)
	    semsg(_(e_white_space_required_after_str_str), ":", *arg - 1);
	*arg = skipwhite(*arg);
	ret_type = parse_type(arg, type_gap, ufunc, cctx, give_error);
	ఒకవేళ (ret_type == ఖాళీ)
	    తిరిగి ఖాళీ;
    }
    ఒకవేళ (flags == 0 && మొదటి_optional == -1 && argcount <= 0)
	type = get_func_type(ret_type, argcount, type_gap);
    లేకపోతే
    {
	type = alloc_func_type(ret_type, argcount, type_gap);
	type->tt_flags = flags;
	ఒకవేళ (argcount > 0)
	{
	    type->tt_argcount = argcount;
	    type->tt_min_argcount = మొదటి_optional == -1
						? argcount : మొదటి_optional;
	    ఒకవేళ (func_type_కూడు_arg_types(type, argcount, type_gap) == FAIL)
		తిరిగి ఖాళీ;
	    mch_memmove(type->tt_వాదనలు, arg_type, పరిమాణం(type_T *) * argcount);
	}
    }

    తిరిగి type;
}

/*
 * Parse a "tuple" type at "*arg" and advance over it.
 * When "give_error" is TRUE give error messages, otherwise be quiet.
 * Return NULL for failure.
 */
    స్థిర type_T *
parse_type_tuple(
    char_u	**arg,
    garray_T	*type_gap,
    పూర్ణాంకం		give_error,
    ufunc_T	*ufunc,
    cctx_T	*cctx)
{
    char_u	*p;
    type_T	*type;
    type_T	*ret_type = ఖాళీ;
    పూర్ణాంకం		typecount = -1;
    పూర్ణాంకం		flags = 0;
    garray_T	tuple_types_ga;

    ga_init2(&tuple_types_ga, పరిమాణం(type_T *), 10);

    // tuple<{type}, {type}>
    // tuple<{type}, ...{type}>
    ఒకవేళ (**arg != '<')
    {
	ఒకవేళ (give_error)
	{
	    ఒకవేళ (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    లేకపోతే
		semsg(_(e_missing_type_after_str), "tuple");
	}

	// only "tuple" is specified
	తిరిగి ఖాళీ;
    }

    p = ++*arg;
    typecount = 0;
    అయితే (*p != NUL && *p != '>')
    {
	ఒకవేళ (STRNCMP(p, "...", 3) == 0)
	{
	    flags |= TTFLAG_VARARGS;
	    p += 3;
	}

	type = parse_type(&p, type_gap, ufunc, cctx, give_error);
	ఒకవేళ (type == ఖాళీ || !valid_declaration_type(type))
	    వెళ్ళు on_err;

	ఒకవేళ ((flags & TTFLAG_VARARGS) != 0 && type->tt_type != VAR_LIST)
	{
	    అక్షరం *tofree;
	    semsg(_(e_variadic_tuple_must_చివర_with_list_type_str),
						type_name(type, &tofree));
	    vim_free(tofree);
	    వెళ్ళు on_err;
	}

	// Add the item type
	ఒకవేళ (ga_grow(&tuple_types_ga, 1) == FAIL)
	    వెళ్ళు on_err;
	((type_T **)tuple_types_ga.ga_దత్తాంశం)[tuple_types_ga.ga_len] = type;
	tuple_types_ga.ga_len++;
	typecount++;

	// Nothing comes after "...{type}".
	ఒకవేళ (flags & TTFLAG_VARARGS)
	    ఆపు;

	ఒకవేళ (*p != ',' && *skipwhite(p) == ',')
	{
	    ఒకవేళ (give_error)
		semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
	    వెళ్ళు on_err;
	}
	ఒకవేళ (*p == ',')
	{
	    ++p;
	    ఒకవేళ (!VIM_ISWHITE(*p))
	    {
		ఒకవేళ (give_error)
		    semsg(_(e_white_space_required_after_str_str),
			    ",", p - 1);
		వెళ్ళు on_err;
	    }
	}
	p = skipwhite(p);
    }

    p = skipwhite(p);
    ఒకవేళ (*p != '>' || typecount <= 0)
    {
	ఒకవేళ (give_error)
	    semsg(_(e_missing_type_after_str), p);
	వెళ్ళు on_err;
    }
    *arg = p + 1;

    ret_type = alloc_tuple_type(typecount, type_gap);
    ఒకవేళ (ret_type == &t_any
	    || tuple_type_కూడు_types(ret_type, typecount, type_gap) == FAIL)
    {
	ret_type = ఖాళీ;
	వెళ్ళు on_err;
    }
    ret_type->tt_flags = flags;
    ret_type->tt_argcount = typecount;
    mch_memmove(ret_type->tt_వాదనలు, tuple_types_ga.ga_దత్తాంశం,
						పరిమాణం(type_T *) * typecount);

on_err:
    ga_clear(&tuple_types_ga);

    తిరిగి ret_type;
}

/*
 * Parse a "object" type at "*arg" and advance over it.
 * When "give_error" is TRUE give error messages, otherwise be quiet.
 * Return NULL for failure.
 */
    స్థిర type_T *
parse_type_object(
    char_u	**arg,
    garray_T	*type_gap,
    పూర్ణాంకం		give_error,
    cctx_T	*cctx)
{
    char_u	*arg_ప్రారంభం = *arg;
    type_T	*object_type;
    పూర్ణాంకం		prev_called_emsg = called_emsg;

    // object<X> or object<any>
    ఒకవేళ (**arg != '<')
    {
	ఒకవేళ (give_error)
	{
	    ఒకవేళ (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    లేకపోతే
		semsg(_(e_missing_type_after_str), "object");
	}

	// only "object" is specified
	తిరిగి ఖాళీ;
    }

    // skip spaces following "object<"
    *arg = skipwhite(*arg + 1);

    object_type = parse_type(arg, type_gap, ఖాళీ, cctx, give_error);
    ఒకవేళ (object_type == ఖాళీ)
	తిరిగి ఖాళీ;

    *arg = skipwhite(*arg);
    ఒకవేళ (**arg != '>' && called_emsg == prev_called_emsg)
    {
	ఒకవేళ (give_error)
	    semsg(_(e_missing_gt_after_type_str), arg_ప్రారంభం);
	తిరిగి ఖాళీ;
    }
    ++*arg;

    ఒకవేళ (object_type->tt_type == VAR_ANY)
	తిరిగి &t_object_any;

    ఒకవేళ (object_type->tt_type != VAR_OBJECT)
    {
	// specified type is not a class
	ఒకవేళ (give_error)
	    semsg(_(e_class_name_not_దొరికింది_str), arg_ప్రారంభం);
	తిరిగి ఖాళీ;
    }

    తిరిగి object_type;
}

/*
 * Parse a user defined type at "*arg" and advance over it.
 * It can be a class or an interface or a typealias name, possibly imported.
 * Return NULL if a type is not found.
 */
    స్థిర type_T *
parse_type_user_defined(
    char_u	**arg,
    size_t	len,
    garray_T	*type_gap,
    పూర్ణాంకం		give_error,
    ufunc_T	*ufunc,
    cctx_T	*cctx)
{
    పూర్ణాంకం		did_emsg_before = did_emsg;
    typval_T	tv;

    tv.v_type = VAR_UNKNOWN;
    ఒకవేళ (eval_variable_import(*arg, &tv) == OK)
    {
	ఒకవేళ (tv.v_type == VAR_CLASS && tv.vval.v_class != ఖాళీ)
	{
	    type_T *type = get_type_ptr(type_gap);
	    ఒకవేళ (type != ఖాళీ)
	    {
		// Although the name is that of a class or interface, the type
		// uses will be an object.
		type->tt_type = VAR_OBJECT;
		type->tt_class = tv.vval.v_class;
		clear_tv(&tv);

		*arg += len;
		// Skip over ".ClassName".
		అయితే (ASCII_ISALNUM(**arg) || **arg == '_' || **arg == '.')
		    ++*arg;

		తిరిగి type;
	    }
	}
	లేకపోతే ఒకవేళ (tv.v_type == VAR_TYPEALIAS)
	{
	    // user defined type
	    // The type alias may itself be a nested type, check the resulting
	    // nesting depth.
	    పూర్ణాంకం	లోతు = parse_లోతు - 1
				+ MAX(tv.vval.v_typealias->ta_లోతు, 1);
	    ఒకవేళ (లోతు - 1 > MAX_TYPE_లోతు)
	    {
		clear_tv(&tv);
		ఒకవేళ (give_error)
		    emsg(_(e_type_nested_too_deep));
		తిరిగి ఖాళీ;
	    }
	    ఒకవేళ (లోతు > parse_max_లోతు)
		parse_max_లోతు = లోతు;

	    type_T *type = copy_type(tv.vval.v_typealias->ta_type, type_gap);
	    *arg += len;
	    clear_tv(&tv);
	    // Skip over ".TypeName".
	    అయితే (ASCII_ISALNUM(**arg) || **arg == '_' || **arg == '.')
		++*arg;
	    తిరిగి type;
	}

	clear_tv(&tv);
    }

    // Check whether it is a generic type
    type_T *type = కనుగొను_generic_type(*arg, len, ufunc, cctx);
    ఒకవేళ (type != ఖాళీ)
    {
	*arg += len;
	తిరిగి type;
    }

    ఒకవేళ (give_error && (did_emsg == did_emsg_before))
    {
	char_u	*p = skip_type(*arg, FALSE);
	అక్షరం	cc = *p;

	*p = NUL;
	semsg(_(e_type_not_recognized_str), *arg);
	*p = cc;
    }

    తిరిగి ఖాళీ;
}

/*
 * Inner part of parse_type().
 */
    స్థిర type_T *
parse_type_inner(
    char_u	**arg,
    garray_T	*type_gap,
    ufunc_T	*ufunc,
    cctx_T	*cctx,
    పూర్ణాంకం		give_error)
{
    char_u  *p = *arg;
    size_t  len;

    // Skip over the first word.
    అయితే (ASCII_ISALNUM(*p) || *p == '_')
	++p;
    len = p - *arg;

    ఎంపిక (**arg)
    {
	స్థితి 'a':
	    ఒకవేళ (len == 3 && STRNCMP(*arg, "any", len) == 0)
	    {
		*arg += len;
		తిరిగి &t_any;
	    }
	    ఆపు;
	స్థితి 'b':
	    ఒకవేళ (len == 4 && STRNCMP(*arg, "bool", len) == 0)
	    {
		*arg += len;
		తిరిగి &t_bool;
	    }
	    ఒకవేళ (len == 4 && STRNCMP(*arg, "blob", len) == 0)
	    {
		*arg += len;
		తిరిగి &t_blob;
	    }
	    ఆపు;
	స్థితి 'c':
	    ఒకవేళ (len == 7 && STRNCMP(*arg, "channel", len) == 0)
	    {
		*arg += len;
		తిరిగి &t_channel;
	    }
	    ఆపు;
	స్థితి 'd':
	    ఒకవేళ (len == 4 && STRNCMP(*arg, "dict", len) == 0)
	    {
		*arg += len;
		తిరిగి parse_type_member(arg, &t_dict_any, type_gap,
						give_error, "dict", ufunc,
						cctx);
	    }
	    ఆపు;
	స్థితి 'f':
	    ఒకవేళ (len == 5 && STRNCMP(*arg, "float", len) == 0)
	    {
		*arg += len;
		తిరిగి &t_float;
	    }
	    ఒకవేళ (len == 4 && STRNCMP(*arg, "func", len) == 0)
		తిరిగి parse_type_func(arg, len, type_gap, give_error, ufunc,
									cctx);
	    ఆపు;
	స్థితి 'j':
	    ఒకవేళ (len == 3 && STRNCMP(*arg, "job", len) == 0)
	    {
		*arg += len;
		తిరిగి &t_పని;
	    }
	    ఆపు;
	స్థితి 'l':
	    ఒకవేళ (len == 4 && STRNCMP(*arg, "list", len) == 0)
	    {
		*arg += len;
		తిరిగి parse_type_member(arg, &t_list_any, type_gap,
						give_error, "list", ufunc,
						cctx);
	    }
	    ఆపు;
	స్థితి 'n':
	    ఒకవేళ (len == 6 && STRNCMP(*arg, "number", len) == 0)
	    {
		*arg += len;
		తిరిగి &t_సంఖ్య;
	    }
	    ఆపు;
	స్థితి 'o':
	    ఒకవేళ (len == 6 && STRNCMP(*arg, "object", len) == 0)
	    {
		*arg += len;
		తిరిగి parse_type_object(arg, type_gap, give_error, cctx);
	    }
	    ఆపు;
	స్థితి 's':
	    ఒకవేళ (len == 6 && STRNCMP(*arg, "string", len) == 0)
	    {
		*arg += len;
		తిరిగి &t_string;
	    }
	    ఆపు;
	స్థితి 't':
	    ఒకవేళ (len == 5 && STRNCMP(*arg, "tuple", len) == 0)
	    {
		*arg += len;
		తిరిగి parse_type_tuple(arg, type_gap, give_error, ufunc,
									cctx);
	    }
	    ఆపు;
	స్థితి 'v':
	    ఒకవేళ (len == 4 && STRNCMP(*arg, "void", len) == 0)
	    {
		*arg += len;
		తిరిగి &t_void;
	    }
	    ఆపు;
    }

    // User defined type
    తిరిగి parse_type_user_defined(arg, len, type_gap, give_error, ufunc,
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
    పూర్ణాంకం		give_error)
{
    type_T	*type;

    ఒకవేళ (parse_లోతు > MAX_TYPE_లోతు)
    {
	ఒకవేళ (give_error)
	    emsg(_(e_type_nested_too_deep));
	తిరిగి ఖాళీ;
    }
    ఒకవేళ (parse_లోతు == 0)
	parse_max_లోతు = 0;
    ++parse_లోతు;
    ఒకవేళ (parse_లోతు > parse_max_లోతు)
	parse_max_లోతు = parse_లోతు;
    type = parse_type_inner(arg, type_gap, ufunc, cctx, give_error);
    --parse_లోతు;
    తిరిగి type;
}

/*
 * Return the number of levels of the type last parsed by parse_type().
 */
    పూర్ణాంకం
parse_type_లోతు(శూన్య)
{
    తిరిగి parse_max_లోతు;
}

/*
 * Check if "type1" and "type2" are exactly the same.
 * "flags" can have ETYPE_ARG_UNKNOWN, which means that an unknown argument
 * type in "type1" is accepted.
 */
    పూర్ణాంకం
equal_type(type_T *type1, type_T *type2, పూర్ణాంకం flags)
{
    పూర్ణాంకం i;

    ఒకవేళ (type1 == ఖాళీ || type2 == ఖాళీ)
	తిరిగి FALSE;
    ఒకవేళ (type1->tt_type != type2->tt_type)
	తిరిగి FALSE;
    ఎంపిక (type1->tt_type)
    {
	స్థితి VAR_UNKNOWN:
	స్థితి VAR_ANY:
	స్థితి VAR_VOID:
	స్థితి VAR_SPECIAL:
	స్థితి VAR_BOOL:
	స్థితి VAR_సంఖ్య:
	స్థితి VAR_FLOAT:
	స్థితి VAR_STRING:
	స్థితి VAR_BLOB:
	స్థితి VAR_పని:
	స్థితి VAR_CHANNEL:
	స్థితి VAR_INSTR:
	స్థితి VAR_TYPEALIAS:
	    ఆపు;  // not composite is always OK
	స్థితి VAR_OBJECT:
	స్థితి VAR_CLASS:
	    // Objects are considered equal if they are from the same class
	    తిరిగి type1->tt_class == type2->tt_class;
	స్థితి VAR_LIST:
	స్థితి VAR_DICT:
	    తిరిగి equal_type(type1->tt_member, type2->tt_member, flags);
	స్థితి VAR_TUPLE:
	    ఒకవేళ (type1->tt_argcount != type2->tt_argcount)
		తిరిగి FALSE;
	    ఒకవేళ (type1->tt_argcount < 0
			   || type1->tt_వాదనలు == ఖాళీ || type2->tt_వాదనలు == ఖాళీ)
		తిరిగి TRUE;
	    కొరకు (i = 0; i < type1->tt_argcount; ++i)
		ఒకవేళ ((flags & ETYPE_ARG_UNKNOWN) == 0
			&& !equal_type(type1->tt_వాదనలు[i], type2->tt_వాదనలు[i],
									flags))
		    తిరిగి FALSE;
	    తిరిగి TRUE;
	స్థితి VAR_FUNC:
	స్థితి VAR_PARTIAL:
	    ఒకవేళ (!equal_type(type1->tt_member, type2->tt_member, flags)
		    || type1->tt_argcount != type2->tt_argcount)
		తిరిగి FALSE;
	    ఒకవేళ (type1->tt_argcount < 0
			   || type1->tt_వాదనలు == ఖాళీ || type2->tt_వాదనలు == ఖాళీ)
		తిరిగి TRUE;
	    కొరకు (i = 0; i < type1->tt_argcount; ++i)
		ఒకవేళ ((flags & ETYPE_ARG_UNKNOWN) == 0
			&& !equal_type(type1->tt_వాదనలు[i], type2->tt_వాదనలు[i],
									flags))
		    తిరిగి FALSE;
	    తిరిగి TRUE;
    }
    తిరిగి TRUE;
}

/*
 * Find the common type of "type1" (VAR_FUNC) and "type2" (VAR_FUNC) and put it
 * in "dest".  "type2" and "dest" may be the same.
 */
    స్థిర శూన్య
common_type_var_func(
    type_T	*type1,
    type_T	*type2,
    type_T	**dest,
    garray_T	*type_gap)
{
    type_T *common;

    // When one of the types is t_func_unknown return the other one.
    // Useful if a list or dict item is null_func.
    ఒకవేళ (type1 == &t_func_unknown)
    {
	*dest = type2;
	తిరిగి;
    }
    ఒకవేళ (type2 == &t_func_unknown)
    {
	*dest = type1;
	తిరిగి;
    }

    common_type(type1->tt_member, type2->tt_member, &common, type_gap);
    ఒకవేళ (type1->tt_argcount == type2->tt_argcount
	    && type1->tt_argcount >= 0)
    {
	పూర్ణాంకం argcount = type1->tt_argcount;
	పూర్ణాంకం i;

	*dest = alloc_func_type(common, argcount, type_gap);
	ఒకవేళ (type1->tt_వాదనలు != ఖాళీ && type2->tt_వాదనలు != ఖాళీ)
	{
	    ఒకవేళ (func_type_కూడు_arg_types(*dest, argcount,
			type_gap) == OK)
		కొరకు (i = 0; i < argcount; ++i)
		    common_type(type1->tt_వాదనలు[i], type2->tt_వాదనలు[i],
			    &(*dest)->tt_వాదనలు[i], type_gap);
	}
    }
    లేకపోతే
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
    శూన్య
common_type(type_T *type1, type_T *type2, type_T **dest, garray_T *type_gap)
{
    ఒకవేళ (equal_type(type1, type2, 0))
    {
	*dest = type1;
	తిరిగి;
    }

    // If either is VAR_UNKNOWN use the other type.  An empty list/dict has no
    // specific type.
    ఒకవేళ (type1 == ఖాళీ || type1->tt_type == VAR_UNKNOWN)
    {
	*dest = type2;
	తిరిగి;
    }
    ఒకవేళ (type2 == ఖాళీ || type2->tt_type == VAR_UNKNOWN)
    {
	*dest = type1;
	తిరిగి;
    }

    ఒకవేళ (type1->tt_type == type2->tt_type)
    {
	ఒకవేళ (type1->tt_type == VAR_LIST
		|| type1->tt_type == VAR_DICT)
	{
	    type_T *common;

	    common_type(type1->tt_member, type2->tt_member, &common, type_gap);
	    ఒకవేళ (type1->tt_type == VAR_LIST)
		*dest = get_list_type(common, type_gap);
	    లేకపోతే
		*dest = get_dict_type(common, type_gap);
	    తిరిగి;
	}
	లేకపోతే ఒకవేళ (type1->tt_type == VAR_FUNC)
	{
	    common_type_var_func(type1, type2, dest, type_gap);
	    తిరిగి;
	}
	లేకపోతే ఒకవేళ (type1->tt_type == VAR_OBJECT)
	{
	    *dest = &t_object_any;
	    తిరిగి;
	}
    }

    *dest = &t_any;
}

/*
 * Return the item type of a List, Dict or a Tuple
 */
    type_T *
get_అంశం_type(type_T *type)
{
    ఒకవేళ (type->tt_type == VAR_TUPLE)
    {
	ఒకవేళ (type->tt_argcount != 1)
	    తిరిగి &t_any;

	ఒకవేళ (type->tt_flags & TTFLAG_VARARGS)
	    తిరిగి type->tt_వాదనలు[0]->tt_member;
	లేకపోతే
	    తిరిగి type->tt_వాదనలు[0];
    }

    // For "any" the item type is not known.
    తిరిగి type->tt_member == ఖాళీ ? &t_any : type->tt_member;
}

/*
 * Push an entry onto the type stack.  "type" used both for the current type
 * and the declared type.
 * Returns FAIL when out of memory.
 */
    పూర్ణాంకం
నెట్టు_type_దొంతర(cctx_T *cctx, type_T *type)
{
    తిరిగి నెట్టు_type_stack2(cctx, type, type);
}

/*
 * Push an entry onto the type stack.  "type" is the current type, "decl_type"
 * is the declared type.
 * Returns FAIL when out of memory.
 */
    పూర్ణాంకం
నెట్టు_type_stack2(cctx_T *cctx, type_T *type, type_T *decl_type)
{
    garray_T	*దొంతర = &cctx->ctx_type_దొంతర;
    type2_T	*typep;

    ఒకవేళ (GA_GROW_FAILS(దొంతర, 1))
	తిరిగి FAIL;
    typep = ((type2_T *)దొంతర->ga_దత్తాంశం) + దొంతర->ga_len;
    typep->type_curr = type;
    typep->type_decl = decl_type;
    ++దొంతర->ga_len;
    తిరిగి OK;
}

/*
 * Set the type of the top of the stack to "type".
 */
    శూన్య
సమితి_type_on_దొంతర(cctx_T *cctx, type_T *type, పూర్ణాంకం స్థానభ్రంశం)
{
    garray_T	*దొంతర = &cctx->ctx_type_దొంతర;
    type2_T	*typep = ((type2_T *)దొంతర->ga_దత్తాంశం)
						  + దొంతర->ga_len - 1 - స్థానభ్రంశం;

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
get_type_on_దొంతర(cctx_T *cctx, పూర్ణాంకం స్థానభ్రంశం)
{
    garray_T	*దొంతర = &cctx->ctx_type_దొంతర;

    ఒకవేళ (స్థానభ్రంశం + 1 > దొంతర->ga_len)
	తిరిగి &t_unknown;
    తిరిగి (((type2_T *)దొంతర->ga_దత్తాంశం) + దొంతర->ga_len - స్థానభ్రంశం - 1)
								   ->type_curr;
}

/*
 * Get the declared type from the type stack.  If "offset" is zero the one at
 * the top,
 * if "offset" is one the type above that, etc.
 * Returns &t_unknown if there is no such stack entry.
 */
    type_T *
get_decl_type_on_దొంతర(cctx_T *cctx, పూర్ణాంకం స్థానభ్రంశం)
{
    garray_T	*దొంతర = &cctx->ctx_type_దొంతర;

    ఒకవేళ (స్థానభ్రంశం + 1 > దొంతర->ga_len)
	తిరిగి &t_unknown;
    తిరిగి (((type2_T *)దొంతర->ga_దత్తాంశం) + దొంతర->ga_len - స్థానభ్రంశం - 1)
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
get_member_type_from_దొంతర(
	పూర్ణాంకం	    లెక్క,
	పూర్ణాంకం	    skip,
	cctx_T	    *cctx)
{
    garray_T	*దొంతర = &cctx->ctx_type_దొంతర;
    type2_T	*typep;
    garray_T    *type_gap = cctx->ctx_type_list;
    పూర్ణాంకం		i;
    type_T	*ఫలితం;
    type_T	*type;

    // Use "unknown" for an empty list or dict.
    ఒకవేళ (లెక్క == 0)
	తిరిగి &t_unknown;
    // Find the common type from following items.
    typep = ((type2_T *)దొంతర->ga_దత్తాంశం) + దొంతర->ga_len;
    ఫలితం = &t_unknown;
    కొరకు (i = 0; i < లెక్క; ++i)
    {
	type = (typep -((లెక్క - i) * skip) + skip - 1)->type_curr;
	ఒకవేళ (check_type_is_విలువ(type) == FAIL)
	    తిరిగి ఖాళీ;
	ఒకవేళ (ఫలితం != &t_any)
	    common_type(type, ఫలితం, &ఫలితం, type_gap);
    }

    తిరిగి ఫలితం;
}

/*
 * Get the types of items in a tuple on the stack of "cctx".
 * Returns the number of types.  Returns -1 on failure.
 */
    పూర్ణాంకం
get_tuple_type_from_దొంతర(
    పూర్ణాంకం		లెక్క,
    garray_T	*tuple_types_gap,
    cctx_T	*cctx)
{
    garray_T	*దొంతర = &cctx->ctx_type_దొంతర;
    type2_T	*typep;
    type_T	*type = ఖాళీ;

    // Use "unknown" for an empty tuple
    ఒకవేళ (లెక్క == 0)
	తిరిగి 0;

    // Find the common type from following items.
    typep = ((type2_T *)దొంతర->ga_దత్తాంశం) + దొంతర->ga_len;
    కొరకు (పూర్ణాంకం i = 0; i < లెక్క; i++)
    {
	type = (typep - (లెక్క - i))->type_curr;
	ఒకవేళ (check_type_is_విలువ(type) == FAIL)
	    తిరిగి -1;
	ఒకవేళ (ga_grow(tuple_types_gap, 1) == FAIL)
	    తిరిగి -1;
	((type_T **)tuple_types_gap->ga_దత్తాంశం)[tuple_types_gap->ga_len] = type;
	tuple_types_gap->ga_len++;
    }

    తిరిగి tuple_types_gap->ga_len;
}

    అక్షరం *
vartype_name(vartype_T type)
{
    ఎంపిక (type)
    {
	స్థితి VAR_UNKNOWN: ఆపు;
	స్థితి VAR_ANY: తిరిగి "any";
	స్థితి VAR_VOID: తిరిగి "void";
	స్థితి VAR_SPECIAL: తిరిగి "special";
	స్థితి VAR_BOOL: తిరిగి "bool";
	స్థితి VAR_సంఖ్య: తిరిగి "number";
	స్థితి VAR_FLOAT: తిరిగి "float";
	స్థితి VAR_STRING: తిరిగి "string";
	స్థితి VAR_BLOB: తిరిగి "blob";
	స్థితి VAR_పని: తిరిగి "job";
	స్థితి VAR_CHANNEL: తిరిగి "channel";
	స్థితి VAR_LIST: తిరిగి "list";
	స్థితి VAR_TUPLE: తిరిగి "tuple";
	స్థితి VAR_DICT: తిరిగి "dict";
	స్థితి VAR_INSTR: తిరిగి "instr";
	స్థితి VAR_CLASS: తిరిగి "class";
	స్థితి VAR_OBJECT: తిరిగి "object";
	స్థితి VAR_TYPEALIAS: తిరిగి "typealias";

	స్థితి VAR_FUNC:
	స్థితి VAR_PARTIAL: తిరిగి "func";
    }
    తిరిగి "unknown";
}

/*
 * Return the type name of a List (list<type>) or Dict (dict<type>).
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    స్థిర అక్షరం *
type_name_list_or_dict(అక్షరం *name, type_T *type, అక్షరం **tofree)
{
    అక్షరం *member_free;
    అక్షరం *member_name;

    ఒకవేళ (type->tt_member->tt_type == VAR_UNKNOWN)
	member_name = type_name(&t_any, &member_free);
    లేకపోతే
	member_name = type_name(type->tt_member, &member_free);

    size_t len = STRLEN(name) + STRLEN(member_name) + 3;
    *tofree = alloc(len);
    ఒకవేళ (*tofree == ఖాళీ)
    {
	vim_free(member_free);
	తిరిగి name;
    }

    vim_snprintf(*tofree, len, "%s<%s>", name, member_name);
    vim_free(member_free);
    తిరిగి *tofree;
}

/*
 * Return the type name of a tuple.
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    స్థిర అక్షరం *
type_name_tuple(type_T *type, అక్షరం **tofree)
{
    garray_T    ga;
    పూర్ణాంకం		i;
    పూర్ణాంకం		varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;
    అక్షరం	*arg_free = ఖాళీ;

    ga_init2(&ga, 1, 100);
    ఒకవేళ (ga_grow(&ga, 20) == FAIL)
	వెళ్ళు failed;
    STRCPY(ga.ga_దత్తాంశం, "tuple<");
    ga.ga_len += 6;

    ఒకవేళ (type->tt_argcount <= 0)
	// empty tuple
	GA_CONCAT_LITERAL(&ga, "any");
    లేకపోతే
    {
	ఒకవేళ (type->tt_వాదనలు == ఖాళీ)
	    GA_CONCAT_LITERAL(&ga, "[unknown]");
	లేకపోతే
	{
	    కొరకు (i = 0; i < type->tt_argcount; ++i)
	    {
		string_T    arg_type;

		arg_type.string = (char_u *)type_name(type->tt_వాదనలు[i], &arg_free);
		ఒకవేళ (i > 0)
		{
		    STRCPY((అక్షరం *)ga.ga_దత్తాంశం + ga.ga_len, ", ");
		    ga.ga_len += 2;
		}
		arg_type.నిడివి = STRLEN(arg_type.string);
		ఒకవేళ (ga_grow(&ga, (పూర్ణాంకం)arg_type.నిడివి + 8) == FAIL)
		    వెళ్ళు failed;
		ఒకవేళ (varargs && i == type->tt_argcount - 1)
		    GA_CONCAT_LITERAL(&ga, "...");
		ga_concat_len(&ga, arg_type.string, arg_type.నిడివి);
		VIM_CLEAR(arg_free);
	    }
	}
    }

    STRCPY((అక్షరం *)ga.ga_దత్తాంశం + ga.ga_len, ">");
    *tofree = ga.ga_దత్తాంశం;
    తిరిగి ga.ga_దత్తాంశం;

failed:
    vim_free(arg_free);
    ga_clear(&ga);
    తిరిగి "[unknown]";
}

/*
 * Return the type name of a Class (class<name>) or Object (object<name>).
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    స్థిర అక్షరం *
type_name_class_or_obj(అక్షరం *name, type_T *type, అక్షరం **tofree)
{
    string_T	class_name;

    ఒకవేళ (type->tt_class != ఖాళీ)
    {
	class_name.string = type->tt_class->class_name.string;
	class_name.నిడివి = type->tt_class->class_name.నిడివి;
	ఒకవేళ (IS_ENUM(type->tt_class))
	    name = "enum";
    }
    లేకపోతే
	STR_LITERAL_సమితి(class_name, "any");

    size_t len = STRLEN(name) + class_name.నిడివి + 3;
    *tofree = alloc(len);
    ఒకవేళ (*tofree == ఖాళీ)
	తిరిగి name;

    vim_snprintf(*tofree, len, "%s<%s>", name, class_name.string);
    తిరిగి *tofree;
}

/*
 * Return the type name of a function.
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    స్థిర అక్షరం *
type_name_func(type_T *type, అక్షరం **tofree)
{
    garray_T    ga;
    పూర్ణాంకం		i;
    పూర్ణాంకం		varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;
    అక్షరం	*arg_free = ఖాళీ;

    ga_init2(&ga, 1, 100);
    ఒకవేళ (ga_grow(&ga, 20) == FAIL)
	వెళ్ళు failed;
    STRCPY(ga.ga_దత్తాంశం, "func(");
    ga.ga_len += 5;

    కొరకు (i = 0; i < type->tt_argcount; ++i)
    {
	string_T    arg_type;

	ఒకవేళ (type->tt_వాదనలు == ఖాళీ)
	    STR_LITERAL_సమితి(arg_type, "[unknown]");
	లేకపోతే
	{
	    arg_type.string = (char_u *)type_name(type->tt_వాదనలు[i], &arg_free);
	    arg_type.నిడివి = STRLEN(arg_type.string);
	}
	ఒకవేళ (i > 0)
	{
	    STRCPY((అక్షరం *)ga.ga_దత్తాంశం + ga.ga_len, ", ");
	    ga.ga_len += 2;
	}
	ఒకవేళ (ga_grow(&ga, (పూర్ణాంకం)arg_type.నిడివి + 8) == FAIL)
	    వెళ్ళు failed;
	ఒకవేళ (varargs && i == type->tt_argcount - 1)
	    GA_CONCAT_LITERAL(&ga, "...");
	లేకపోతే ఒకవేళ (i >= type->tt_min_argcount)
	    *((అక్షరం *)ga.ga_దత్తాంశం + ga.ga_len++) = '?';
	ga_concat_len(&ga, arg_type.string, arg_type.నిడివి);
	VIM_CLEAR(arg_free);
    }
    ఒకవేళ (type->tt_argcount < 0)
	// any number of arguments
	GA_CONCAT_LITERAL(&ga, "...");

    ఒకవేళ (type->tt_member == &t_void)
	STRCPY((అక్షరం *)ga.ga_దత్తాంశం + ga.ga_len, ")");
    లేకపోతే
    {
	అక్షరం *ret_free = ఖాళీ;
	అక్షరం *ret_name = type_name(type->tt_member, &ret_free);
	పూర్ణాంకం  len;

	len = (పూర్ణాంకం)STRLEN(ret_name) + 4;
	ఒకవేళ (ga_grow(&ga, len) == FAIL)
	{
	    vim_free(ret_free);
	    వెళ్ళు failed;
	}
	STRCPY((అక్షరం *)ga.ga_దత్తాంశం + ga.ga_len, "): ");
	STRCPY((అక్షరం *)ga.ga_దత్తాంశం + ga.ga_len + 3, ret_name);
	vim_free(ret_free);
    }
    *tofree = ga.ga_దత్తాంశం;
    తిరిగి ga.ga_దత్తాంశం;

failed:
    vim_free(arg_free);
    ga_clear(&ga);
    తిరిగి "[unknown]";
}

/*
 * Return the name of a type.
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    అక్షరం *
type_name(type_T *type, అక్షరం **tofree)
{
    అక్షరం *name;

    *tofree = ఖాళీ;
    ఒకవేళ (type == ఖాళీ)
	తిరిగి "[unknown]";
    name = vartype_name(type->tt_type);

    ఎంపిక (type->tt_type)
    {
	స్థితి VAR_LIST:
	స్థితి VAR_DICT:
	    తిరిగి type_name_list_or_dict(name, type, tofree);

	స్థితి VAR_TUPLE:
	    తిరిగి type_name_tuple(type, tofree);

	స్థితి VAR_CLASS:
	స్థితి VAR_OBJECT:
	    తిరిగి type_name_class_or_obj(name, type, tofree);

	స్థితి VAR_FUNC:
	    తిరిగి type_name_func(type, tofree);

	అప్రమేయ:
	    ఆపు;
    }

    తిరిగి name;
}

/*
 * "typename(expr)" function
 */
    శూన్య
f_typename(typval_T *argvars, typval_T *rettv)
{
    garray_T	type_list;
    type_T	*type;
    అక్షరం	*tofree;
    అక్షరం	*name;

    rettv->v_type = VAR_STRING;
    ga_init2(&type_list, పరిమాణం(type_T *), 10);
    ఒకవేళ (argvars[0].v_type == VAR_TYPEALIAS)
    {
	type = copy_type(argvars[0].vval.v_typealias->ta_type, &type_list);
	// A type alias for a class has the type set to VAR_OBJECT.  Change it
	// to VAR_CLASS, so that the name is "typealias<class<xxx>>"
	ఒకవేళ (type->tt_type == VAR_OBJECT)
	    type->tt_type = VAR_CLASS;
    }
    లేకపోతే
	type = typval2type(argvars, get_copyID(), &type_list, TVTT_DO_MEMBER);
    name = type_name(type, &tofree);
    ఒకవేళ (argvars[0].v_type == VAR_TYPEALIAS)
    {
	vim_snprintf((అక్షరం *)IObuff, IOSIZE, "typealias<%s>", name);
	rettv->vval.v_string = vim_strsave((char_u *)IObuff);
	ఒకవేళ (tofree != ఖాళీ)
	    vim_free(tofree);
    }
    లేకపోతే
    {
	ఒకవేళ (tofree != ఖాళీ)
	    rettv->vval.v_string = (char_u *)tofree;
	లేకపోతే
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
    పూర్ణాంకం
check_typval_is_విలువ(typval_T *tv)
{
    ఒకవేళ (tv == ఖాళీ)
	తిరిగి OK;

    ఎంపిక (tv->v_type)
    {
	స్థితి VAR_CLASS:
	    {
		class_T *cl = tv->vval.v_class;
		char_u *class_name = (cl == ఖాళీ) ? (char_u *)""
							: cl->class_name.string;
		ఒకవేళ (cl != ఖాళీ && IS_ENUM(cl))
		    semsg(_(e_using_enum_as_విలువ_str), class_name);
		లేకపోతే
		    semsg(_(e_using_class_as_విలువ_str), class_name);
	    }
	    తిరిగి FAIL;

	స్థితి VAR_TYPEALIAS:
	    semsg(_(e_using_typealias_as_విలువ_str),
		    tv->vval.v_typealias->ta_name);
	    తిరిగి FAIL;

	అప్రమేయ:
	    ఆపు;
    }
    తిరిగి OK;
}

/*
 * Same as above, except check type_T.
 */
    పూర్ణాంకం
check_type_is_విలువ(type_T *type)
{
    ఒకవేళ (type == ఖాళీ)
	తిరిగి OK;
    ఎంపిక (type->tt_type)
    {
	స్థితి VAR_CLASS:
	    ఒకవేళ (type->tt_class != ఖాళీ && IS_ENUM(type->tt_class))
		semsg(_(e_using_enum_as_విలువ_str),
			type->tt_class->class_name.string);
	    లేకపోతే
		semsg(_(e_using_class_as_విలువ_str),
			type->tt_class == ఖాళీ ? (char_u *)""
			: type->tt_class->class_name.string);
	    తిరిగి FAIL;

	స్థితి VAR_TYPEALIAS:
	    // TODO: Not sure what could be done here to get a name.
	    //       Maybe an optional argument?
	    emsg(_(e_using_typealias_as_var_val));
	    తిరిగి FAIL;

	అప్రమేయ:
	    ఆపు;
    }
    తిరిగి OK;
}

#అయితే_ముగింపు // FEAT_EVAL
