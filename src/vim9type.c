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

#تعریف_کرو USING_FLOAT_STUFF
#شامل "vim.h"

#اگر defined(FEAT_EVAL)

#اگر_تعریف_شدہ VMS
# include <float.h>
#اگر_ختم

/*
 * Allocate memory for a type_T and add the pointer to type_gap, so that it can
 * be easily freed later.
 */
    type_T *
get_type_ptr(garray_T *type_gap)
{
    type_T *type;

    اگر (ga_grow(type_gap, 1) == FAIL)
	واپس تہی;
    type = ALLOC_CLEAR_ONE(type_T);
    اگر (type == تہی)
	واپس تہی;

    ((type_T **)type_gap->ga_ڈیٹا)[type_gap->ga_len] = type;
    ++type_gap->ga_len;
    واپس type;
}

/*
 * Make a shallow copy of "type".
 * When allocation fails returns "type".
 */
    type_T *
copy_type(type_T *type, garray_T *type_gap)
{
    type_T *copy = get_type_ptr(type_gap);

    اگر (copy == تہی)
	واپس type;
    *copy = *type;
    copy->tt_flags &= ~TTFLAG_STATIC;

    اگر (type->tt_دلائل != تہی
	   && func_type_جوڑو_arg_types(copy, type->tt_argcount, type_gap) == OK)
	برائے (صحیح i = 0; i < type->tt_argcount; ++i)
	    copy->tt_دلائل[i] = type->tt_دلائل[i];

    واپس copy;
}

/*
 * Inner part of copy_type_deep().
 * When allocation fails returns "type".
 */
    ساکن type_T *
copy_type_deep_rec(type_T *type, garray_T *type_gap, garray_T *نظر_آیا_types)
{
    برائے (صحیح i = 0; i < نظر_آیا_types->ga_len; ++i)
	اگر (((type_T **)نظر_آیا_types->ga_ڈیٹا)[i * 2] == type)
	    // seen this type before, return the copy we made
	    واپس ((type_T **)نظر_آیا_types->ga_ڈیٹا)[i * 2 + 1];

    type_T *copy = copy_type(type, type_gap);
    اگر (ga_grow(نظر_آیا_types, 1) == FAIL)
	واپس copy;
    ((type_T **)نظر_آیا_types->ga_ڈیٹا)[نظر_آیا_types->ga_len * 2] = type;
    ((type_T **)نظر_آیا_types->ga_ڈیٹا)[نظر_آیا_types->ga_len * 2 + 1] = copy;
    ++نظر_آیا_types->ga_len;

    اگر (copy->tt_member != تہی)
	copy->tt_member = copy_type_deep_rec(copy->tt_member,
							 type_gap, نظر_آیا_types);
    اگر (type->tt_دلائل != تہی)
	برائے (صحیح i = 0; i < type->tt_argcount; ++i)
	    copy->tt_دلائل[i] = copy_type_deep_rec(copy->tt_دلائل[i],
							 type_gap, نظر_آیا_types);

    واپس copy;
}

/*
 * Make a deep copy of "type".
 * When allocation fails returns "type".
 */
    type_T *
copy_type_deep(type_T *type, garray_T *type_gap)
{
    garray_T نظر_آیا_types;
    // stores type pairs : a type we have seen and the copy used
    ga_init2(&نظر_آیا_types, حجم(type_T *) * 2, 20);

    type_T *res = copy_type_deep_rec(type, type_gap, &نظر_آیا_types);

    ga_clear(&نظر_آیا_types);
    واپس res;
}

    خالی
clear_type_list(garray_T *gap)
{
    جب (gap->ga_len > 0)
	vim_free(((type_T **)gap->ga_ڈیٹا)[--gap->ga_len]);
    ga_clear(gap);
}

    خالی
clear_func_type_list(garray_T *gap, type_T **func_type)
{
    جب (gap->ga_len > 0)
    {
	// func_type pointing to the uf_type_list, so reset pointer
	اگر (*func_type == ((type_T **)gap->ga_ڈیٹا)[--gap->ga_len])
	    *func_type = &t_func_any;
	vim_free(((type_T **)gap->ga_ڈیٹا)[gap->ga_len]);
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

    اگر (type == تہی)
	واپس تہی;

    // A fixed type never contains allocated types, return as-is.
    اگر (type->tt_flags & TTFLAG_STATIC)
	واپس type;

    ret = ALLOC_ONE(type_T);
    اگر (ret == تہی)
	واپس تہی;
    *ret = *type;

    اگر (ret->tt_member != تہی)
	ret->tt_member = alloc_type(ret->tt_member);

    اگر (type->tt_argcount > 0 && type->tt_دلائل != تہی)
    {
	صحیح i;

	ret->tt_دلائل = ALLOC_MULT(type_T *, type->tt_argcount);
	اگر (ret->tt_دلائل != تہی)
	    برائے (i = 0; i < type->tt_argcount; ++i)
		ret->tt_دلائل[i] = alloc_type(type->tt_دلائل[i]);
    }
    ورنہ
	ret->tt_دلائل = تہی;

    واپس ret;
}

/*
 * Free a type that was created with alloc_type().
 */
    خالی
free_type(type_T *type)
{
    صحیح i;

    اگر (type == تہی || (type->tt_flags & TTFLAG_STATIC))
	واپس;
    اگر (type->tt_دلائل != تہی)
    {
	برائے (i = 0; i < type->tt_argcount; ++i)
	    free_type(type->tt_دلائل[i]);
	vim_free(type->tt_دلائل);
    }

    free_type(type->tt_member);

    vim_free(type);
}

/*
 * Return TRUE if "type" is to be recursed into for setting the type.
 */
    ساکن صحیح
سیٹ_tv_type_recurse(type_T *type)
{
    واپس type->tt_member != تہی
		&& (type->tt_member->tt_type == VAR_DICT
				|| type->tt_member->tt_type == VAR_LIST)
		&& type->tt_member->tt_member != تہی
		&& type->tt_member->tt_member != &t_any
		&& type->tt_member->tt_member != &t_unknown;
}

/*
 * Set the type of Dict "d" to "type"
 */
    ساکن خالی
سیٹ_tv_type_dict(dict_T *d, type_T *type)
{
    اگر (d->dv_type == type)
	واپس;

    free_type(d->dv_type);
    d->dv_type = alloc_type(type);

    // Need to recursively set the type of dict items?
    اگر (!سیٹ_tv_type_recurse(type))
	واپس;

    صحیح		todo = (صحیح)d->dv_hashtab.ht_مستعمل;
    hashitem_T	*hi;
    dictitem_T	*di;

    FOR_ALL_HASHTAB_اشیاء(&d->dv_hashtab, hi, todo)
    {
	اگر (!HASHITEM_خالی_خانے(hi))
	{
	    --todo;
	    di = HI2DI(hi);
	    سیٹ_tv_type(&di->di_tv, type->tt_member);
	}
    }
}

/*
 * Set the type of List "l" to "type"
 */
    ساکن خالی
سیٹ_tv_type_list(list_T *l, type_T *type)
{
    اگر (l->lv_type == type)
	واپس;

    free_type(l->lv_type);
    l->lv_type = alloc_type(type);

    // Need to recursively set the type of list items?
    اگر (l->lv_پہلا == &range_list_شے || !سیٹ_tv_type_recurse(type))
	واپس;

    listitem_T	*li;

    FOR_ALL_LIST_اشیاء(l, li)
	سیٹ_tv_type(&li->li_tv, type->tt_member);
}

/*
 * Set the type of Tuple "tuple" to "type"
 */
    ساکن خالی
سیٹ_tv_type_tuple(tuple_T *tuple, type_T *type)
{
    اگر (tuple->tv_type == type)
	واپس;

    free_type(tuple->tv_type);
    tuple->tv_type = alloc_type(type);

    اگر (type->tt_argcount <= 0)
	واپس;

    // recursively set the type of list items
    type_T	*شے_type;
    برائے (صحیح i = 0; i < tuple_len(tuple); i++)
    {
	اگر ((type->tt_flags & TTFLAG_VARARGS) && (i >= type->tt_argcount - 1))
	    // For a variadic tuple, the last type is a List.  So use the
	    // List member type.
	    شے_type = type->tt_دلائل[type->tt_argcount - 1]->tt_member;
	ورنہ
	    شے_type = type->tt_دلائل[i];

	سیٹ_tv_type(TUPLE_شے(tuple, i), شے_type);
    }
}

/*
 * Set the type of "tv" to "type" if it is a list or tuple or dict.
 */
    خالی
سیٹ_tv_type(typval_T *tv, type_T *type)
{
    اگر (type->tt_type == VAR_ANY)
	// If the variable type is "any", then keep the value type.
	// e.g.  var x: any = [1, 2] or var y: any = {v: 1}
	واپس;

    اگر (tv->v_type == VAR_DICT && tv->vval.v_dict != تہی)
	سیٹ_tv_type_dict(tv->vval.v_dict, type);
    ورنہ اگر (tv->v_type == VAR_LIST && tv->vval.v_list != تہی)
	سیٹ_tv_type_list(tv->vval.v_list, type);
    ورنہ اگر (tv->v_type == VAR_TUPLE && tv->vval.v_tuple != تہی)
	سیٹ_tv_type_tuple(tv->vval.v_tuple, type);
}

/*
 * For a tuple type, reserve space for "typecount" types (including the
 * repeated type).
 */
    ساکن صحیح
tuple_type_جوڑو_types(
    type_T	*tupletype,
    صحیح		typecount,
    garray_T	*type_gap)
{
    // To make it easy to free the space needed for the types, add the
    // pointer to type_gap.
    اگر (ga_grow(type_gap, 1) == FAIL)
	واپس FAIL;
    tupletype->tt_دلائل = alloc_clear_id(حجم(type_T *) * typecount,
							  aid_tuple_type_دلائل);
    اگر (tupletype->tt_دلائل == تہی)
	واپس FAIL;
    ((type_T **)type_gap->ga_ڈیٹا)[type_gap->ga_len] =
						(خالی *)tupletype->tt_دلائل;
    ++type_gap->ga_len;
    واپس OK;
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
    اگر (member_type == تہی || (member_type->tt_type == VAR_ANY
					&& !IS_GENERIC_TYPE(member_type)))
	واپس &t_list_any;
    اگر (member_type->tt_type == VAR_VOID
	    || member_type->tt_type == VAR_UNKNOWN)
	واپس &t_list_خالی_خانے;
    اگر (member_type->tt_type == VAR_BOOL)
	واپس &t_list_bool;
    اگر (member_type->tt_type == VAR_نمبر)
	واپس &t_list_نمبر;
    اگر (member_type->tt_type == VAR_STRING)
	واپس &t_list_string;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    اگر (type == تہی)
	واپس &t_any;
    type->tt_type = VAR_LIST;
    type->tt_member = member_type;
    type->tt_argcount = 0;
    type->tt_دلائل = تہی;
    واپس type;
}

/*
 * Create and return a tuple type from the tuple item types in
 * "tuple_types_ga".
 */
    type_T *
get_tuple_type(garray_T *tuple_types_gap, garray_T *type_gap)
{
    type_T	*type;
    type_T	**tuple_types = tuple_types_gap->ga_ڈیٹا;
    صحیح		typecount = tuple_types_gap->ga_len;

    // recognize commonly used types
    اگر (typecount == 0)
	واپس &t_tuple_any;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    اگر (type == تہی)
	واپس &t_any;
    type->tt_type = VAR_TUPLE;
    type->tt_member = تہی;
    اگر (typecount > 0)
    {
	اگر (tuple_type_جوڑو_types(type, typecount, type_gap) == FAIL)
	    واپس &t_any;
	mch_memmove(type->tt_دلائل, tuple_types, حجم(type_T *) * typecount);
    }
    type->tt_argcount = typecount;
    type->tt_flags = 0;

    واپس type;
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
    اگر (member_type == تہی || (member_type->tt_type == VAR_ANY
					&& !IS_GENERIC_TYPE(member_type)))
	واپس &t_dict_any;
    اگر (member_type->tt_type == VAR_VOID
	    || member_type->tt_type == VAR_UNKNOWN)
	واپس &t_dict_خالی_خانے;
    اگر (member_type->tt_type == VAR_BOOL)
	واپس &t_dict_bool;
    اگر (member_type->tt_type == VAR_نمبر)
	واپس &t_dict_نمبر;
    اگر (member_type->tt_type == VAR_STRING)
	واپس &t_dict_string;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    اگر (type == تہی)
	واپس &t_any;
    type->tt_type = VAR_DICT;
    type->tt_member = member_type;
    type->tt_argcount = 0;
    type->tt_دلائل = تہی;
    واپس type;
}

/*
 * Allocate a new type for a function.
 */
    type_T *
alloc_func_type(type_T *ret_type, صحیح argcount, garray_T *type_gap)
{
    type_T *type = get_type_ptr(type_gap);

    اگر (type == تہی)
	واپس &t_any;
    type->tt_type = VAR_FUNC;
    type->tt_member = ret_type == تہی ? &t_unknown : ret_type;
    type->tt_argcount = argcount;
    type->tt_دلائل = تہی;
    واپس type;
}

/*
 * Allocate a new type for a tuple.
 */
    ساکن type_T *
alloc_tuple_type(صحیح typecount, garray_T *type_gap)
{
    type_T *type = get_type_ptr(type_gap);

    اگر (type == تہی)
	واپس &t_any;
    type->tt_type = VAR_TUPLE;
    type->tt_member = تہی;
    type->tt_argcount = typecount;
    type->tt_دلائل = تہی;
    واپس type;
}

/*
 * Get a function type, based on the return type "ret_type".
 * "argcount" must be -1 or 0, a predefined type can be used.
 */
    type_T *
get_func_type(type_T *ret_type, صحیح argcount, garray_T *type_gap)
{
    // recognize commonly used types
    اگر (ret_type == &t_unknown || ret_type == تہی)
    {
	// (argcount == 0) is not possible
	واپس &t_func_unknown;
    }
    اگر (ret_type == &t_void)
    {
	اگر (argcount == 0)
	    واپس &t_func_0_void;
	ورنہ
	    واپس &t_func_void;
    }
    اگر (ret_type == &t_any)
    {
	اگر (argcount == 0)
	    واپس &t_func_0_any;
	ورنہ
	    واپس &t_func_any;
    }
    اگر (ret_type == &t_نمبر)
    {
	اگر (argcount == 0)
	    واپس &t_func_0_نمبر;
	ورنہ
	    واپس &t_func_نمبر;
    }
    اگر (ret_type == &t_string)
    {
	اگر (argcount == 0)
	    واپس &t_func_0_string;
	ورنہ
	    واپس &t_func_string;
    }

    واپس alloc_func_type(ret_type, argcount, type_gap);
}

/*
 * For a function type, reserve space for "argcount" argument types (including
 * vararg).
 */
    صحیح
func_type_جوڑو_arg_types(
	type_T	    *functype,
	صحیح	    argcount,
	garray_T    *type_gap)
{
    // To make it easy to free the space needed for the argument types, add the
    // pointer to type_gap.
    اگر (ga_grow(type_gap, 1) == FAIL)
	واپس FAIL;
    functype->tt_دلائل = ALLOC_CLEAR_MULT(type_T *, argcount);
    اگر (functype->tt_دلائل == تہی)
	واپس FAIL;
    ((type_T **)type_gap->ga_ڈیٹا)[type_gap->ga_len] =
						     (خالی *)functype->tt_دلائل;
    ++type_gap->ga_len;
    واپس OK;
}

/*
 * Return TRUE if "type" is NULL, any or unknown.
 * This also works for const (comparing with &t_any and &t_unknown doesn't).
 */
    صحیح
type_any_or_unknown(type_T *type)
{
    واپس type == تہی || type->tt_type == VAR_ANY
					|| type->tt_type == VAR_UNKNOWN;
}

/*
 * Get a type_T for a "special" typval in "tv".
 */
    ساکن type_T *
special_typval2type(typval_T *tv)
{
    انتخاب (tv->vval.v_نمبر)
    {
	حالت VVAL_NULL:
	    واپس &t_null;

	حالت VVAL_NONE:
	    واپس &t_none;

	حالت VVAL_TRUE:
	حالت VVAL_FALSE:
	    واپس &t_bool;

	طےشدہ:
	    واپس &t_unknown;
    }
}

/*
 * Get a type_T for a List typval in "tv".
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    ساکن type_T *
list_typval2type(typval_T *tv, صحیح copyID, garray_T *type_gap, صحیح flags)
{
    list_T	*l = tv->vval.v_list;
    listitem_T	*li;
    type_T	*member_type = تہی;

    // An empty list has type list<unknown>, unless the type was specified
    // and is not list<any>.  This matters when assigning to a variable
    // with a specific list type.
    اگر (l == تہی || (l->lv_پہلا == تہی
		&& (l->lv_type == تہی || l->lv_type->tt_member == &t_any)))
	واپس &t_list_خالی_خانے;

    اگر ((flags & TVTT_DO_MEMBER) == 0)
	واپس &t_list_any;

    // If the type is list<any> go through the members, it may end up a
    // more specific type.
    اگر (l->lv_type != تہی && (l->lv_پہلا == تہی
		|| (flags & TVTT_مزید_SPECIFIC) == 0
		|| l->lv_type->tt_member != &t_any))
	// make a copy, lv_type may be freed if the list is freed
	واپس copy_type_deep(l->lv_type, type_gap);

    اگر (l->lv_پہلا == &range_list_شے)
	واپس &t_list_نمبر;

    اگر (l->lv_copyID == copyID)
	// avoid recursion
	واپس &t_list_any;

    l->lv_copyID = copyID;

    // Use the common type of all members.
    member_type = typval2type(&l->lv_پہلا->li_tv, copyID, type_gap,
							TVTT_DO_MEMBER);
    برائے (li = l->lv_پہلا->li_اگلا; li != تہی; li = li->li_اگلا)
	common_type(typval2type(&li->li_tv, copyID, type_gap, TVTT_DO_MEMBER),
					member_type, &member_type, type_gap);

    // Reset copyID so that a shared reference to this list (not a circular
    // reference) can be processed again to get the correct type.
    l->lv_copyID = 0;

    واپس get_list_type(member_type, type_gap);
}

/*
 * Get a type_T for a Tuple typval in "tv".
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    ساکن type_T *
tuple_typval2type(typval_T *tv, صحیح copyID, garray_T *type_gap, صحیح flags)
{
    tuple_T	*tuple = tv->vval.v_tuple;
    صحیح		len = tuple_len(tuple);
    type_T	*type = تہی;

    // An empty tuple has type tuple<unknown>, unless the type was specified
    // and is not tuple<any>.  This matters when assigning to a variable
    // with a specific tuple type.
    اگر (tuple == تہی || (len == 0 && (tuple->tv_type == تہی
				|| tuple->tv_type->tt_argcount == 0)))
	واپس &t_tuple_خالی_خانے;

    اگر ((flags & TVTT_DO_MEMBER) == 0)
	واپس &t_tuple_any;

    // If the type is tuple<any> go through the members, it may end up a
    // more specific type.
    اگر (tuple->tv_type != تہی && (len == 0
					|| (flags & TVTT_مزید_SPECIFIC) == 0))
	// make a copy, tv_type may be freed if the tuple is freed
	واپس copy_type_deep(tuple->tv_type, type_gap);

    اگر (tuple->tv_copyID == copyID)
	// avoid recursion
	واپس &t_tuple_any;

    tuple->tv_copyID = copyID;

    garray_T	tuple_types_ga;
    ga_init2(&tuple_types_ga, حجم(type_T *), 10);
    برائے (صحیح i = 0; i < len; i++)
    {
	type = typval2type(TUPLE_شے(tuple, i), copyID, type_gap,
							TVTT_DO_MEMBER);
	اگر (ga_grow(&tuple_types_ga, 1) == FAIL)
	{
	    ga_clear(&tuple_types_ga);
	    // Reset copyID so that a shared reference to this tuple can be
	    // processed again.
	    tuple->tv_copyID = 0;
	    واپس تہی;
	}
	((type_T **)tuple_types_ga.ga_ڈیٹا)[tuple_types_ga.ga_len] = type;
	tuple_types_ga.ga_len++;
    }

    type_T *tuple_type = get_tuple_type(&tuple_types_ga, type_gap);
    ga_clear(&tuple_types_ga);

    // Reset copyID so that a shared reference to this tuple (not a circular
    // reference) can be processed again to get the correct type.
    tuple->tv_copyID = 0;

    واپس tuple_type;
}

/*
 * Get a type_T for a Dict typval in "tv".
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    ساکن type_T *
dict_typval2type(typval_T *tv, صحیح copyID, garray_T *type_gap, صحیح flags)
{
    dict_iterator_T	iter;
    typval_T		*قدر;
    dict_T		*d = tv->vval.v_dict;
    type_T		*member_type = تہی;

    اگر (d == تہی || (d->dv_hashtab.ht_مستعمل == 0 && d->dv_type == تہی))
	واپس &t_dict_خالی_خانے;

    اگر ((flags & TVTT_DO_MEMBER) == 0)
	واپس &t_dict_any;

    // If the type is dict<any> go through the members, it may end up a
    // more specific type.
    اگر (d->dv_type != تہی && (d->dv_hashtab.ht_مستعمل == 0
		|| (flags & TVTT_مزید_SPECIFIC) == 0
		|| d->dv_type->tt_member != &t_any))
	واپس d->dv_type;

    اگر (d->dv_copyID == copyID)
	// avoid recursion
	واپس &t_dict_any;

    d->dv_copyID = copyID;

    // Use the common type of all values.
    dict_iterate_آغاز(tv, &iter);
    dict_iterate_اگلا(&iter, &قدر);
    member_type = typval2type(قدر, copyID, type_gap, TVTT_DO_MEMBER);

    جب (dict_iterate_اگلا(&iter, &قدر) != تہی)
	common_type(typval2type(قدر, copyID, type_gap, TVTT_DO_MEMBER),
					member_type, &member_type, type_gap);

    // Reset copyID so that a shared reference to this dict (not a circular
    // reference) can be processed again to get the correct type.
    d->dv_copyID = 0;

    واپس get_dict_type(member_type, type_gap);
}

/*
 * Get a type_T for a "partial" typval in "tv".
 */
    ساکن type_T *
partial_typval2type(typval_T *tv, ufunc_T *ufunc, garray_T *type_gap)
{
    partial_T *pt = tv->vval.v_partial;
    type_T  *type;

    type = get_type_ptr(type_gap);
    اگر (type == تہی)
	واپس تہی;

    *type = *ufunc->uf_func_type;
    اگر (type->tt_argcount >= 0 && pt->pt_argc > 0)
    {
	type->tt_argcount -= pt->pt_argc;
	type->tt_min_argcount -= pt->pt_argc;
	اگر (type->tt_argcount > 0 && func_type_جوڑو_arg_types(type,
		    type->tt_argcount, type_gap) == OK)
	    برائے (صحیح i = 0; i < type->tt_argcount; ++i)
		type->tt_دلائل[i] =
		    ufunc->uf_func_type->tt_دلائل[i + pt->pt_argc];
    }
    اگر (pt->pt_func != تہی)
	type->tt_member = pt->pt_func->uf_ret_type;

    واپس type;
}

/*
 * Get a type_T for a "class" or an "object" typval in "tv".
 */
    ساکن type_T *
oc_typval2type(typval_T *tv)
{
    اگر (tv->v_type == VAR_CLASS)
    {
	اگر (tv->vval.v_class == تہی)
	    واپس &t_class;

	واپس &tv->vval.v_class->class_type;
    }

    اگر (tv->vval.v_object != تہی)
	واپس &tv->vval.v_object->obj_class->class_object_type;

    واپس &t_object_any;
}

/*
 * Get a type_T for a "function" or a "partial"
 */
    ساکن type_T *
fp_typval2type(typval_T *tv, garray_T *type_gap)
{
    char_u	*name = تہی;
    ufunc_T	*ufunc = تہی;
    type_T	*type;
    type_T	*member_type = تہی;
    صحیح		argcount = 0;
    صحیح		min_argcount = 0;

    اگر (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != تہی)
    {
	اگر (tv->vval.v_partial->pt_func != تہی)
	    ufunc = tv->vval.v_partial->pt_func;
	ورنہ
	    name = tv->vval.v_partial->pt_name;
    }
    ورنہ
	name = tv->vval.v_string;

    اگر (name == تہی && ufunc == تہی)
	واپس &t_func_unknown;

    اگر (name != تہی)
    {
	صحیح idx = ڈھونڈو_اندرونی_func(name);

	اگر (idx >= 0)
	{
	    type_T *decl_type;  //غیر_مستعمل

	    اندرونی_func_get_argcount(idx, &argcount, &min_argcount);
	    member_type = اندرونی_func_ret_type(idx, 0, تہی, &decl_type,
		    type_gap);
	}
	ورنہ
	{
	    // Check if name contains "<".  If it does, then replace "<" with
	    // NUL and lookup the function and then look up the specific
	    // generic function using the supplied types.
	    char_u *p = generic_func_ڈھونڈو_open_bracket(name);
	    اگر (p == تہی)
		ufunc = ڈھونڈو_func(name, FALSE);
	    ورنہ
	    {
		// generic function
		char_u	cc = *p;

		*p = NUL;
		ufunc = ڈھونڈو_func(name, FALSE);
		*p = cc;
		اگر (ufunc != تہی && IS_GENERIC_FUNC(ufunc))
		{
		    ufunc = ڈھونڈو_generic_func(ufunc, name, &p);
		    اگر (ufunc == تہی)
			واپس تہی;
		}
	    }
	}
    }
    اگر (ufunc != تہی)
    {
	// May need to get the argument types from default values by
	// compiling the function.
	اگر (ufunc->uf_def_حالت == UF_مقصد_BE_COMPILED
		&& compile_def_function(ufunc, TRUE, CT_NONE, تہی)
		== FAIL)
	    واپس تہی;
	اگر (ufunc->uf_func_type == تہی)
	    سیٹ_function_type(ufunc);
	اگر (ufunc->uf_func_type != تہی)
	{
	    اگر (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != تہی)
		واپس partial_typval2type(tv, ufunc, type_gap);
	    واپس ufunc->uf_func_type;
	}
    }

    type = get_type_ptr(type_gap);
    اگر (type == تہی)
	واپس تہی;
    type->tt_type = tv->v_type;
    type->tt_argcount = argcount;
    type->tt_min_argcount = min_argcount;
    اگر (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != تہی
					&& tv->vval.v_partial->pt_argc > 0)
    {
	type->tt_argcount -= tv->vval.v_partial->pt_argc;
	type->tt_min_argcount -= tv->vval.v_partial->pt_argc;
    }
    type->tt_member = member_type;

    واپس type;
}

/*
 * Get a type_T for a typval_T.
 * "type_gap" is used to temporarily create types in.
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    ساکن type_T *
typval2type_int(typval_T *tv, صحیح copyID, garray_T *type_gap, صحیح flags)
{
    انتخاب (tv->v_type)
    {
	حالت VAR_UNKNOWN:
	    واپس &t_unknown;

	حالت VAR_ANY:
	    واپس &t_any;

	حالت VAR_VOID:
	    واپس &t_void;

	حالت VAR_BOOL:
	    واپس &t_bool;

	حالت VAR_SPECIAL:
	    واپس special_typval2type(tv);

	حالت VAR_نمبر:
	    واپس &t_نمبر;

	حالت VAR_FLOAT:
	    واپس &t_float;

	حالت VAR_STRING:
	    واپس &t_string;

	حالت VAR_BLOB:
	    اگر (tv->vval.v_blob == تہی)
		واپس &t_blob_null;
	    واپس &t_blob;

	حالت VAR_LIST:
	    واپس list_typval2type(tv, copyID, type_gap, flags);

	حالت VAR_TUPLE:
	    واپس tuple_typval2type(tv, copyID, type_gap, flags);

	حالت VAR_DICT:
	    واپس dict_typval2type(tv, copyID, type_gap, flags);

	حالت VAR_کام:
	    واپس &t_کام;

	حالت VAR_CHANNEL:
	    واپس &t_channel;

	حالت VAR_CLASS:
	حالت VAR_OBJECT:
	    واپس oc_typval2type(tv);

	حالت VAR_TYPEALIAS:
	    واپس &t_typealias;

	حالت VAR_FUNC:
	حالت VAR_PARTIAL:
	    واپس fp_typval2type(tv, type_gap);

	حالت VAR_INSTR:
	طےشدہ:
	    توڑو;
    }

    واپس تہی;
}

/*
 * Return TRUE if "tv" is not a bool but should be converted to bool.
 */
    صحیح
need_convert_مقصد_bool(type_T *type, typval_T *tv)
{
    واپس type != تہی && type == &t_bool && tv->v_type != VAR_BOOL
	    && (tv->v_type == VAR_نمبر
		       && (tv->vval.v_نمبر == 0 || tv->vval.v_نمبر == 1));
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

    اگر (type == تہی)
	واپس تہی;

    اگر (type != &t_bool && (tv->v_type == VAR_نمبر
		&& (tv->vval.v_نمبر == 0 || tv->vval.v_نمبر == 1)))
	// Number 0 and 1 and expression with "&&" or "||" can also be used
	// for bool.
	type = &t_نمبر_bool;
    ورنہ اگر (type != &t_float && tv->v_type == VAR_نمبر)
	// A number can also be used for float.
	type = &t_نمبر_float;
    واپس type;
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
	حرف *tofree = تہی;
	حرف *name = type_name(type, &tofree);
	semsg(_(e_invalid_type_اندر_variable_declaration_str), name);
	vim_free(tofree);
	واپس FALSE;
    }
    واپس TRUE;
}

/*
 * Get a type_T for a typval_T, used for v: variables.
 * "type_list" is used to temporarily create types in.
 */
    type_T *
typval2type_vimvar(typval_T *tv, garray_T *type_gap)
{
    اگر (tv->v_type == VAR_LIST)  // e.g. for v:oldfiles
	واپس &t_list_string;
    اگر (tv->v_type == VAR_DICT)  // e.g. for v:event
	واپس &t_dict_any;
    واپس typval2type(tv, get_copyID(), type_gap, TVTT_DO_MEMBER);
}

    صحیح
check_typval_arg_type(
	type_T	    *expected,
	typval_T    *actual_tv,
	حرف	    *func_name,
	صحیح	    arg_idx)
{
    where_T	where = WHERE_INIT;

    اگر (arg_idx > 0)
    {
	where.wt_اشاریہ = arg_idx;
	where.wt_قسم = WT_دلیل;
    }
    where.wt_func_name = func_name;
    واپس check_typval_type(expected, actual_tv, where);
}

/*
 * Return true if "expected" is list<any> or dict<any> and "actual_tv" is a
 * list or dict of that kind, which matches whatever its values are.
 */
    ساکن bool
matches_listdict_any(type_T *expected, typval_T *actual_tv)
{
    واپس (expected->tt_type == VAR_LIST || expected->tt_type == VAR_DICT)
	    && actual_tv->v_type == expected->tt_type
	    && expected->tt_member != تہی
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

    اگر (expected == تہی)
	واپس OK;  // didn't expect anything.

    // No need to go through the values of a possibly big list or dict.
    اگر (matches_listdict_any(expected, actual_tv))
	واپس OK;

    ga_init2(&type_list, حجم(type_T *), 10);

    // A null_function and null_partial are special cases, they can be used to
    // clear a variable.
    اگر ((actual_tv->v_type == VAR_FUNC && actual_tv->vval.v_string == تہی)
	    || (actual_tv->v_type == VAR_PARTIAL
					 && actual_tv->vval.v_partial == تہی))
	actual_type = &t_func_unknown;
    ورنہ
	// When the actual type is list<any> or dict<any> go through the values
	// to possibly get a more specific type.
	actual_type = typval2type(actual_tv, get_copyID(), &type_list,
					  TVTT_DO_MEMBER | TVTT_مزید_SPECIFIC);
    اگر (actual_type != تہی)
    {
	res = check_type_maybe(expected, actual_type, TRUE, where);
	اگر (res == MAYBE && !(actual_type->tt_type == VAR_FUNC
				      && (actual_type->tt_member == &t_unknown
					  || actual_type->tt_member == تہی)))
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
    واپس res;
}

    خالی
arg_type_mismatch(type_T *expected, type_T *actual, صحیح arg_idx)
{
    where_T	where = WHERE_INIT;

    اگر (arg_idx > 0)
    {
	where.wt_اشاریہ = arg_idx;
	where.wt_قسم = WT_دلیل;
    }
    type_mismatch_where(expected, actual, where);
}

    خالی
type_mismatch_where(type_T *expected, type_T *actual, where_T where)
{
    حرف *tofree1, *tofree2;
    حرف *typename1 = type_name(expected, &tofree1);
    حرف *typename2 = type_name(actual, &tofree2);

    انتخاب (where.wt_قسم)
    {
	حالت WT_MEMBER:
	    semsg(_(e_variable_str_type_mismatch_expected_str_but_got_str),
		    where.wt_func_name, typename1, typename2);
	    توڑو;
	حالت WT_طریقہ:
	حالت WT_طریقہ_ARG:
	حالت WT_طریقہ_RETURN:
	    semsg(_(e_طریقہ_str_type_mismatch_expected_str_but_got_str),
		    where.wt_func_name, typename1, typename2);
	    توڑو;
	حالت WT_VARIABLE:
	    اگر (where.wt_func_name == تہی)
		semsg(_(e_variable_nr_type_mismatch_expected_str_but_got_str),
			where.wt_اشاریہ, typename1, typename2);
	    ورنہ
		semsg(_(e_variable_nr_type_mismatch_expected_str_but_got_str_اندر_str),
			where.wt_اشاریہ, typename1, typename2, where.wt_func_name);
	    توڑو;
	حالت WT_دلیل:
	    اگر (where.wt_func_name == تہی)
		semsg(_(e_دلیل_nr_type_mismatch_expected_str_but_got_str),
			where.wt_اشاریہ, typename1, typename2);
	    ورنہ
		semsg(_(e_دلیل_nr_type_mismatch_expected_str_but_got_str_اندر_str),
			where.wt_اشاریہ, typename1, typename2, where.wt_func_name);
	    توڑو;
	حالت WT_CAST:
	حالت WT_UNKNOWN:
	    اگر (where.wt_func_name == تہی)
		semsg(_(e_type_mismatch_expected_str_but_got_str),
			typename1, typename2);
	    ورنہ
		semsg(_(e_type_mismatch_expected_str_but_got_str_اندر_str),
			typename1, typename2, where.wt_func_name);
	    توڑو;
    }

    vim_free(tofree1);
    vim_free(tofree2);
}

/*
 * Check if the expected and actual types match for a tuple
 */
    ساکن صحیح
check_tuple_type_maybe(
    type_T	*expected,
    type_T	*actual,
    where_T	where)
{
    اگر (expected->tt_argcount == -1 || actual->tt_argcount == -1
		|| expected->tt_دلائل == تہی || actual->tt_دلائل == تہی)
	واپس OK;

    // For a non-variadic tuple, the number of items must match
    اگر (!(expected->tt_flags & TTFLAG_VARARGS)
	    && expected->tt_argcount != actual->tt_argcount)
	واپس FAIL;

    // compare the type of each tuple item
    برائے (صحیح i = 0; i < actual->tt_argcount; ++i)
    {
	type_T	*exp_type;
	type_T	*actual_type;

	اگر (expected->tt_flags & TTFLAG_VARARGS)
	{
	    اگر (i < expected->tt_argcount - 1)
		exp_type = expected->tt_دلائل[i];
	    ورنہ
		// For a variadic tuple, the last type is a List.  So use the
		// List member type.
		exp_type = expected->tt_دلائل[expected->tt_argcount - 1]->tt_member;
	}
	ورنہ
	    exp_type = expected->tt_دلائل[i];

	اگر (actual->tt_flags & TTFLAG_VARARGS)
	{
	    اگر (i < actual->tt_argcount - 1)
		actual_type = actual->tt_دلائل[i];
	    ورنہ
		// For a variadic tuple, the last type is a List.  So use the
		// List member type.
		actual_type = actual->tt_دلائل[actual->tt_argcount - 1]->tt_member;
	}
	ورنہ
	    actual_type = actual->tt_دلائل[i];

	// Allow for using "any" type for a tuple item
	اگر (actual->tt_دلائل[i] != &t_any && check_type(exp_type, actual_type,
						FALSE, where) == FAIL)
	    واپس FAIL;
    }

    واپس OK;
}

/*
 * Check if the expected and actual types match for a function
 * Returns OK if "expected" and "actual" are matching function types.
 * Returns FAIL if "expected" and "actual" are different types.
 * Returns MAYBE when a runtime type check is needed.
 */
    ساکن صحیح
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
	اگر (actual->tt_member != تہی
		&& actual->tt_member != &t_unknown)
	{
	    where_T  func_where = where;

	    func_where.wt_قسم = WT_طریقہ_RETURN;
	    ret = check_type_maybe(expected->tt_member,
		    actual->tt_member, FALSE,
		    func_where);
	}
	ورنہ
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
    اگر (ret != FAIL && expected->tt_دلائل != تہی
	    && actual->tt_دلائل != تہی)
    {
	صحیح i;

	برائے (i = 0; i < expected->tt_argcount
		&& i < actual->tt_argcount; ++i)
	{
	    where_T  func_where = where;
	    func_where.wt_قسم = WT_طریقہ_ARG;

	    // Allow for using "any" argument type, lambda's have them.
	    اگر (actual->tt_دلائل[i] != &t_any && check_type(
			expected->tt_دلائل[i], actual->tt_دلائل[i], FALSE,
			func_where) == FAIL)
	    {
		ret = FAIL;
		توڑو;
	    }
	}
    }
    اگر (ret == OK && expected->tt_argcount >= 0
	    && actual->tt_argcount == -1)
	// check the argument count at runtime
	ret = MAYBE;

    واپس ret;
}

/*
 * Check if the expected and actual types match for an object
 * Returns OK if "expected" and "actual" are matching object types.
 * Returns FAIL if "expected" and "actual" are different types.
 * Returns MAYBE when a runtime type check is needed.
 */
    ساکن صحیح
check_object_type_maybe(
    type_T	*expected,
    type_T	*actual,
    where_T	where)
{
    صحیح ret = OK;

    اگر (actual->tt_type == VAR_ANY)
	واپس MAYBE;	// use runtime type check
    اگر (actual->tt_type != VAR_OBJECT)
	واپس FAIL;	// don't use tt_class
    اگر (actual->tt_class == تہی)    // null object
	واپس OK;
    // t_object_any matches any object except for an enum item
    اگر (expected == &t_object_any && !IS_ENUM(actual->tt_class))
	واپس OK;

    // For object method arguments, do a invariant type check in
    // an extended class.  For all others, do a covariance type check.
    اگر (where.wt_قسم == WT_طریقہ_ARG)
    {
	اگر (actual->tt_class != expected->tt_class)
	    ret = FAIL;
    }
    ورنہ اگر (!class_instance_کا(actual->tt_class, expected->tt_class))
    {
	// Check if this is an up-cast, if so we'll have to check the type at
	// runtime.
	اگر (where.wt_قسم == WT_CAST &&
		class_instance_کا(expected->tt_class, actual->tt_class))
	    ret = MAYBE;
	ورنہ
	    ret = FAIL;
    }

    واپس ret;
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
	صحیح	give_پیغام_2,
	where_T where)
{
    صحیح ret = check_type_maybe(expected, actual, give_پیغام_2, where);

    واپس ret == MAYBE ? OK : ret;
}

/*
 * As check_type() but return MAYBE when a runtime type check should be used
 * when compiling.
 */
    صحیح
check_type_maybe(
	type_T	*expected,
	type_T	*actual,
	صحیح	give_پیغام_2,
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
		واپس OK;
	    اگر (expected->tt_type == VAR_FLOAT
		    && actual->tt_type == VAR_نمبر
		    && ((expected->tt_flags & TTFLAG_نمبر_OK)
			     || (actual->tt_flags & TTFLAG_FLOAT_OK)))
		// Using a number where a float is expected is OK here.
		واپس OK;
	    اگر (expected->tt_type == VAR_LIST
		    && actual->tt_type == VAR_TUPLE
		    && (expected->tt_flags & TTFLAG_TUPLE_OK))
		// Using a tuple where a list is expected is OK here.
		واپس OK;
	    اگر (give_پیغام_2)
		type_mismatch_where(expected, actual, where);
	    واپس FAIL;
	}
	اگر (expected->tt_type == VAR_DICT || expected->tt_type == VAR_LIST)
	{
	    // "unknown" is used for an empty list or dict
	    اگر (actual->tt_member != تہی && actual->tt_member != &t_unknown)
		ret = check_type_maybe(expected->tt_member, actual->tt_member,
								 FALSE, where);
	}
	ورنہ اگر (expected->tt_type == VAR_TUPLE && actual != &t_any)
	    ret =  check_tuple_type_maybe(expected, actual, where);
	ورنہ اگر (expected->tt_type == VAR_FUNC && actual != &t_any)
	    ret = check_func_type_maybe(expected, actual, where);
	ورنہ اگر (expected->tt_type == VAR_OBJECT)
	    ret = check_object_type_maybe(expected, actual, where);

	اگر (ret == FAIL && give_پیغام_2)
	    type_mismatch_where(expected, actual, where);
    }

    اگر (ret == OK && expected->tt_type != VAR_UNKNOWN
	    && expected->tt_type != VAR_ANY
	    && (actual->tt_type == VAR_UNKNOWN || actual->tt_type == VAR_ANY))
	// check the type at runtime
	ret = MAYBE;

    واپس ret;
}

/*
 * Check that the arguments of "type" match "argvars[argcount]".
 * "base_tv" is from "expr->Func()".
 * Return OK/FAIL.
 */
    صحیح
check_دلیل_types(
	type_T	    *type,
	typval_T    *argvars,
	صحیح	    argcount,
	typval_T    *بنیاد_tv,
	char_u	    *name)
{
    صحیح	    varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;
    صحیح	    i;
    صحیح	    totcount = argcount + (بنیاد_tv == تہی ? 0 : 1);

    اگر (type->tt_type != VAR_FUNC && type->tt_type != VAR_PARTIAL)
	واپس OK;  // just in case
    اگر (totcount < type->tt_min_argcount - varargs)
    {
	emsg_funcname(e_not_enough_arguments_for_function_str, name);
	واپس FAIL;
    }
    اگر (!varargs && type->tt_argcount >= 0 && totcount > type->tt_argcount)
    {
	emsg_funcname(e_too_many_arguments_for_function_str, name);
	واپس FAIL;
    }
    اگر (type->tt_دلائل == تہی)
	واپس OK;  // cannot check


    برائے (i = 0; i < totcount; ++i)
    {
	type_T	    *expected;
	typval_T    *tv;

	اگر (بنیاد_tv != تہی)
	{
	    اگر (i == 0)
		tv = بنیاد_tv;
	    ورنہ
		tv = &argvars[i - 1];
	}
	ورنہ
	    tv = &argvars[i];
	اگر (varargs && i >= type->tt_argcount - 1)
	{
	    expected = type->tt_دلائل[type->tt_argcount - 1];
	    اگر (expected != تہی && expected->tt_type == VAR_LIST)
		expected = expected->tt_member;
	    اگر (expected == تہی)
		expected = &t_any;
	}
	ورنہ
	    expected = type->tt_دلائل[i];

	// check the type, unless the value is v:none
	اگر ((tv->v_type != VAR_SPECIAL || tv->vval.v_نمبر != VVAL_NONE)
		   && check_typval_arg_type(expected, tv, تہی, i + 1) == FAIL)
	    واپس FAIL;
    }
    واپس OK;
}

// Maximum nesting of types, to avoid running out of stack space.  E.g.
// "list<list<number>>" is nested twice.
#تعریف_کرو MAX_TYPE_گہرائی	1000

// Current number of levels of parse_type() and the deepest level reached while
// parsing the current type.  The nesting is one less than the number of
// levels.
ساکن صحیح	parse_گہرائی = 0;
ساکن صحیح	parse_max_گہرائی = 0;

/*
 * Skip over type in list<type>, dict<type> or tuple<type>.
 * Returns a pointer to the character after the type.  "syn_error" is set to
 * TRUE on syntax error.
 */
    ساکن char_u *
skip_member_type(char_u *آغاز, char_u *p, صحیح *syn_error)
{
    اگر (STRNCMP("tuple", آغاز, 5) == 0)
    {
	// handle tuple<{type1}, {type2}, ....<type>>
	p = skipwhite(p + 1);
	جب (*p != '>' && *p != NUL)
	{
	    char_u *sp = p;

	    اگر (STRNCMP(p, "...", 3) == 0)
		p += 3;
	    p = skip_type(p, TRUE);
	    اگر (p == sp)
	    {
		*syn_error = TRUE;
		واپس p;  // syntax error
	    }
	    اگر (*p == ',')
		p = skipwhite(p + 1);
	}
	اگر (*p == '>')
	    p++;
    }
    ورنہ
    {
	p = skipwhite(p);
	p = skip_type(skipwhite(p + 1), FALSE);
	p = skipwhite(p);
	اگر (*p == '>')
	    ++p;
    }

    واپس p;
}

/*
 * Skip over a function type.  Returns a pointer to the character after the
 * type.  "syn_error" is set to TRUE on syntax error.
 */
    ساکن char_u *
skip_func_type(char_u *p, صحیح *syn_error)
{
    اگر (*p == '(')
    {
	// handle func(args): type
	++p;
	جب (*p != ')' && *p != NUL)
	{
	    char_u *sp = p;

	    اگر (STRNCMP(p, "...", 3) == 0)
		p += 3;
	    p = skip_type(p, TRUE);
	    اگر (p == sp)
	    {
		*syn_error = TRUE;
		واپس p;  // syntax error
	    }
	    اگر (*p == ',')
		p = skipwhite(p + 1);
	}
	اگر (*p == ')')
	{
	    اگر (p[1] == ':')
		p = skip_type(skipwhite(p + 2), FALSE);
	    ورنہ
		++p;
	}
    }
    ورنہ
    {
	// handle func: return_type
	p = skip_type(skipwhite(p + 1), FALSE);
    }

    واپس p;
}

/*
 * Inner part of skip_type().
 */
    ساکن char_u *
skip_type_inner(char_u *آغاز, صحیح optional)
{
    char_u	*p = آغاز;
    صحیح		syn_error = FALSE;

    اگر (optional && *p == '?')
	++p;

    // Also skip over "." for imported classes: "import.ClassName".
    جب (ASCII_ISALNUM(*p) || *p == '_' || *p == '.')
	++p;

    // Skip over "<type>"; this is permissive about white space.
    اگر (*skipwhite(p) == '<')
    {
	p = skip_member_type(آغاز, p, &syn_error);
	اگر (syn_error)
	    واپس p;
    }
    ورنہ اگر ((*p == '(' || (*p == ':' && VIM_ISWHITE(p[1])))
					     && STRNCMP("func", آغاز, 4) == 0)
    {
	// skip over function type
	p = skip_func_type(p, &syn_error);
	اگر (syn_error)
	    واپس p;
    }

    واپس p;
}

/*
 * Skip over a type definition and return a pointer to just after it.
 * When "optional" is TRUE then a leading "?" is accepted.
 */
    char_u *
skip_type(char_u *آغاز, صحیح optional)
{
    ساکن صحیح	skip_گہرائی = 0;
    char_u	*p = آغاز;

    // Too deeply nested: skip to the matching ">" without recursing, an
    // error is given when parsing the type.
    اگر (skip_گہرائی > MAX_TYPE_گہرائی)
    {
	صحیح	گہرائی = 0;

	برائے ( ; *p != NUL; ++p)
	{
	    اگر (*p == '<')
		++گہرائی;
	    ورنہ اگر (*p == '>')
	    {
		اگر (گہرائی == 0)
		    توڑو;
		اگر (--گہرائی == 0)
		    واپس p + 1;
	    }
	    ورنہ اگر (گہرائی == 0 && !ASCII_ISALNUM(*p) && *p != '_'
								&& *p != '.')
		توڑو;
	}
	واپس p;
    }

    ++skip_گہرائی;
    p = skip_type_inner(آغاز, optional);
    --skip_گہرائی;
    واپس p;
}

/*
 * Parse the member type: "<type>" and return "type" with the member set.
 * Use "type_gap" if a new type needs to be added.
 * "info" is extra information for an error message.
 * Returns NULL in case of failure.
 */
    ساکن type_T *
parse_type_member(
	char_u	    **arg,
	type_T	    *type,
	garray_T    *type_gap,
	صحیح	    give_error,
	حرف	    *info,
	ufunc_T	    *ufunc,
	cctx_T	    *cctx)
{
    char_u  *arg_آغاز = *arg;
    type_T  *member_type;
    صحیح	    پچھلا_called_emsg = called_emsg;

    اگر (**arg != '<')
    {
	اگر (give_error)
	{
	    اگر (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    ورنہ
		semsg(_(e_missing_type_after_str), info);
	}
	واپس تہی;
    }
    *arg = skipwhite(*arg + 1);

    member_type = parse_type(arg, type_gap, ufunc, cctx, give_error);
    اگر (member_type == تہی || !valid_declaration_type(member_type))
	واپس تہی;

    *arg = skipwhite(*arg);
    اگر (**arg != '>' && called_emsg == پچھلا_called_emsg)
    {
	اگر (give_error)
	    semsg(_(e_missing_gt_after_type_str), arg_آغاز);
	واپس تہی;
    }
    ++*arg;

    اگر (type->tt_type == VAR_LIST)
	واپس get_list_type(member_type, type_gap);
    واپس get_dict_type(member_type, type_gap);
}

/*
 * Parse a "func" type at "*arg" and advance over it.
 * When "give_error" is TRUE give error messages, otherwise be quiet.
 * Return NULL for failure.
 */
    ساکن type_T *
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
    صحیح	پہلا_optional = -1;
    type_T	*arg_type[MAX_FUNC_دلائل + 1];

    // func({type}, ...{type}): {type}
    *arg += len;
    اگر (**arg == '(')
    {
	// "func" may or may not return a value, "func()" does
	// not return a value.
	ret_type = &t_void;

	p = ++*arg;
	argcount = 0;
	جب (*p != NUL && *p != ')')
	{
	    اگر (*p == '?')
	    {
		اگر (پہلا_optional == -1)
		    پہلا_optional = argcount;
		++p;
	    }
	    ورنہ اگر (STRNCMP(p, "...", 3) == 0)
	    {
		flags |= TTFLAG_VARARGS;
		p += 3;
	    }
	    ورنہ اگر (پہلا_optional != -1)
	    {
		اگر (give_error)
		    emsg(_(e_mandatory_دلیل_after_optional_دلیل));
		واپس تہی;
	    }

	    type = parse_type(&p, type_gap, ufunc, cctx, give_error);
	    اگر (type == تہی || !valid_declaration_type(type))
		واپس تہی;
	    اگر ((flags & TTFLAG_VARARGS) != 0 && type->tt_type != VAR_LIST)
	    {
		حرف *tofree;
		semsg(_(e_variable_arguments_type_must_be_list_str),
			type_name(type, &tofree));
		vim_free(tofree);
		واپس تہی;
	    }
	    arg_type[argcount++] = type;

	    // Nothing comes after "...{type}".
	    اگر (flags & TTFLAG_VARARGS)
		توڑو;

	    اگر (*p != ',' && *skipwhite(p) == ',')
	    {
		اگر (give_error)
		    semsg(_(e_no_white_space_allowed_before_str_str),
			    ",", p);
		واپس تہی;
	    }
	    اگر (*p == ',')
	    {
		++p;
		اگر (!VIM_ISWHITE(*p))
		{
		    اگر (give_error)
			semsg(_(e_white_space_required_after_str_str),
				",", p - 1);
		    واپس تہی;
		}
	    }
	    p = skipwhite(p);
	    اگر (argcount == MAX_FUNC_دلائل)
	    {
		اگر (give_error)
		    emsg(_(e_too_many_دلیل_types));
		واپس تہی;
	    }
	}

	p = skipwhite(p);
	اگر (*p != ')')
	{
	    اگر (give_error)
		emsg(_(e_missing_closing_paren));
	    واپس تہی;
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
	اگر (ret_type == تہی)
	    واپس تہی;
    }
    اگر (flags == 0 && پہلا_optional == -1 && argcount <= 0)
	type = get_func_type(ret_type, argcount, type_gap);
    ورنہ
    {
	type = alloc_func_type(ret_type, argcount, type_gap);
	type->tt_flags = flags;
	اگر (argcount > 0)
	{
	    type->tt_argcount = argcount;
	    type->tt_min_argcount = پہلا_optional == -1
						? argcount : پہلا_optional;
	    اگر (func_type_جوڑو_arg_types(type, argcount, type_gap) == FAIL)
		واپس تہی;
	    mch_memmove(type->tt_دلائل, arg_type, حجم(type_T *) * argcount);
	}
    }

    واپس type;
}

/*
 * Parse a "tuple" type at "*arg" and advance over it.
 * When "give_error" is TRUE give error messages, otherwise be quiet.
 * Return NULL for failure.
 */
    ساکن type_T *
parse_type_tuple(
    char_u	**arg,
    garray_T	*type_gap,
    صحیح		give_error,
    ufunc_T	*ufunc,
    cctx_T	*cctx)
{
    char_u	*p;
    type_T	*type;
    type_T	*ret_type = تہی;
    صحیح		typecount = -1;
    صحیح		flags = 0;
    garray_T	tuple_types_ga;

    ga_init2(&tuple_types_ga, حجم(type_T *), 10);

    // tuple<{type}, {type}>
    // tuple<{type}, ...{type}>
    اگر (**arg != '<')
    {
	اگر (give_error)
	{
	    اگر (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    ورنہ
		semsg(_(e_missing_type_after_str), "tuple");
	}

	// only "tuple" is specified
	واپس تہی;
    }

    p = ++*arg;
    typecount = 0;
    جب (*p != NUL && *p != '>')
    {
	اگر (STRNCMP(p, "...", 3) == 0)
	{
	    flags |= TTFLAG_VARARGS;
	    p += 3;
	}

	type = parse_type(&p, type_gap, ufunc, cctx, give_error);
	اگر (type == تہی || !valid_declaration_type(type))
	    جاؤ on_err;

	اگر ((flags & TTFLAG_VARARGS) != 0 && type->tt_type != VAR_LIST)
	{
	    حرف *tofree;
	    semsg(_(e_variadic_tuple_must_انتہا_with_list_type_str),
						type_name(type, &tofree));
	    vim_free(tofree);
	    جاؤ on_err;
	}

	// Add the item type
	اگر (ga_grow(&tuple_types_ga, 1) == FAIL)
	    جاؤ on_err;
	((type_T **)tuple_types_ga.ga_ڈیٹا)[tuple_types_ga.ga_len] = type;
	tuple_types_ga.ga_len++;
	typecount++;

	// Nothing comes after "...{type}".
	اگر (flags & TTFLAG_VARARGS)
	    توڑو;

	اگر (*p != ',' && *skipwhite(p) == ',')
	{
	    اگر (give_error)
		semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
	    جاؤ on_err;
	}
	اگر (*p == ',')
	{
	    ++p;
	    اگر (!VIM_ISWHITE(*p))
	    {
		اگر (give_error)
		    semsg(_(e_white_space_required_after_str_str),
			    ",", p - 1);
		جاؤ on_err;
	    }
	}
	p = skipwhite(p);
    }

    p = skipwhite(p);
    اگر (*p != '>' || typecount <= 0)
    {
	اگر (give_error)
	    semsg(_(e_missing_type_after_str), p);
	جاؤ on_err;
    }
    *arg = p + 1;

    ret_type = alloc_tuple_type(typecount, type_gap);
    اگر (ret_type == &t_any
	    || tuple_type_جوڑو_types(ret_type, typecount, type_gap) == FAIL)
    {
	ret_type = تہی;
	جاؤ on_err;
    }
    ret_type->tt_flags = flags;
    ret_type->tt_argcount = typecount;
    mch_memmove(ret_type->tt_دلائل, tuple_types_ga.ga_ڈیٹا,
						حجم(type_T *) * typecount);

on_err:
    ga_clear(&tuple_types_ga);

    واپس ret_type;
}

/*
 * Parse a "object" type at "*arg" and advance over it.
 * When "give_error" is TRUE give error messages, otherwise be quiet.
 * Return NULL for failure.
 */
    ساکن type_T *
parse_type_object(
    char_u	**arg,
    garray_T	*type_gap,
    صحیح		give_error,
    cctx_T	*cctx)
{
    char_u	*arg_آغاز = *arg;
    type_T	*object_type;
    صحیح		پچھلا_called_emsg = called_emsg;

    // object<X> or object<any>
    اگر (**arg != '<')
    {
	اگر (give_error)
	{
	    اگر (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    ورنہ
		semsg(_(e_missing_type_after_str), "object");
	}

	// only "object" is specified
	واپس تہی;
    }

    // skip spaces following "object<"
    *arg = skipwhite(*arg + 1);

    object_type = parse_type(arg, type_gap, تہی, cctx, give_error);
    اگر (object_type == تہی)
	واپس تہی;

    *arg = skipwhite(*arg);
    اگر (**arg != '>' && called_emsg == پچھلا_called_emsg)
    {
	اگر (give_error)
	    semsg(_(e_missing_gt_after_type_str), arg_آغاز);
	واپس تہی;
    }
    ++*arg;

    اگر (object_type->tt_type == VAR_ANY)
	واپس &t_object_any;

    اگر (object_type->tt_type != VAR_OBJECT)
    {
	// specified type is not a class
	اگر (give_error)
	    semsg(_(e_class_name_not_ملا_str), arg_آغاز);
	واپس تہی;
    }

    واپس object_type;
}

/*
 * Parse a user defined type at "*arg" and advance over it.
 * It can be a class or an interface or a typealias name, possibly imported.
 * Return NULL if a type is not found.
 */
    ساکن type_T *
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
	اگر (tv.v_type == VAR_CLASS && tv.vval.v_class != تہی)
	{
	    type_T *type = get_type_ptr(type_gap);
	    اگر (type != تہی)
	    {
		// Although the name is that of a class or interface, the type
		// uses will be an object.
		type->tt_type = VAR_OBJECT;
		type->tt_class = tv.vval.v_class;
		clear_tv(&tv);

		*arg += len;
		// Skip over ".ClassName".
		جب (ASCII_ISALNUM(**arg) || **arg == '_' || **arg == '.')
		    ++*arg;

		واپس type;
	    }
	}
	ورنہ اگر (tv.v_type == VAR_TYPEALIAS)
	{
	    // user defined type
	    // The type alias may itself be a nested type, check the resulting
	    // nesting depth.
	    صحیح	گہرائی = parse_گہرائی - 1
				+ MAX(tv.vval.v_typealias->ta_گہرائی, 1);
	    اگر (گہرائی - 1 > MAX_TYPE_گہرائی)
	    {
		clear_tv(&tv);
		اگر (give_error)
		    emsg(_(e_type_nested_too_deep));
		واپس تہی;
	    }
	    اگر (گہرائی > parse_max_گہرائی)
		parse_max_گہرائی = گہرائی;

	    type_T *type = copy_type(tv.vval.v_typealias->ta_type, type_gap);
	    *arg += len;
	    clear_tv(&tv);
	    // Skip over ".TypeName".
	    جب (ASCII_ISALNUM(**arg) || **arg == '_' || **arg == '.')
		++*arg;
	    واپس type;
	}

	clear_tv(&tv);
    }

    // Check whether it is a generic type
    type_T *type = ڈھونڈو_generic_type(*arg, len, ufunc, cctx);
    اگر (type != تہی)
    {
	*arg += len;
	واپس type;
    }

    اگر (give_error && (did_emsg == did_emsg_before))
    {
	char_u	*p = skip_type(*arg, FALSE);
	حرف	cc = *p;

	*p = NUL;
	semsg(_(e_type_not_recognized_str), *arg);
	*p = cc;
    }

    واپس تہی;
}

/*
 * Inner part of parse_type().
 */
    ساکن type_T *
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
    جب (ASCII_ISALNUM(*p) || *p == '_')
	++p;
    len = p - *arg;

    انتخاب (**arg)
    {
	حالت 'a':
	    اگر (len == 3 && STRNCMP(*arg, "any", len) == 0)
	    {
		*arg += len;
		واپس &t_any;
	    }
	    توڑو;
	حالت 'b':
	    اگر (len == 4 && STRNCMP(*arg, "bool", len) == 0)
	    {
		*arg += len;
		واپس &t_bool;
	    }
	    اگر (len == 4 && STRNCMP(*arg, "blob", len) == 0)
	    {
		*arg += len;
		واپس &t_blob;
	    }
	    توڑو;
	حالت 'c':
	    اگر (len == 7 && STRNCMP(*arg, "channel", len) == 0)
	    {
		*arg += len;
		واپس &t_channel;
	    }
	    توڑو;
	حالت 'd':
	    اگر (len == 4 && STRNCMP(*arg, "dict", len) == 0)
	    {
		*arg += len;
		واپس parse_type_member(arg, &t_dict_any, type_gap,
						give_error, "dict", ufunc,
						cctx);
	    }
	    توڑو;
	حالت 'f':
	    اگر (len == 5 && STRNCMP(*arg, "float", len) == 0)
	    {
		*arg += len;
		واپس &t_float;
	    }
	    اگر (len == 4 && STRNCMP(*arg, "func", len) == 0)
		واپس parse_type_func(arg, len, type_gap, give_error, ufunc,
									cctx);
	    توڑو;
	حالت 'j':
	    اگر (len == 3 && STRNCMP(*arg, "job", len) == 0)
	    {
		*arg += len;
		واپس &t_کام;
	    }
	    توڑو;
	حالت 'l':
	    اگر (len == 4 && STRNCMP(*arg, "list", len) == 0)
	    {
		*arg += len;
		واپس parse_type_member(arg, &t_list_any, type_gap,
						give_error, "list", ufunc,
						cctx);
	    }
	    توڑو;
	حالت 'n':
	    اگر (len == 6 && STRNCMP(*arg, "number", len) == 0)
	    {
		*arg += len;
		واپس &t_نمبر;
	    }
	    توڑو;
	حالت 'o':
	    اگر (len == 6 && STRNCMP(*arg, "object", len) == 0)
	    {
		*arg += len;
		واپس parse_type_object(arg, type_gap, give_error, cctx);
	    }
	    توڑو;
	حالت 's':
	    اگر (len == 6 && STRNCMP(*arg, "string", len) == 0)
	    {
		*arg += len;
		واپس &t_string;
	    }
	    توڑو;
	حالت 't':
	    اگر (len == 5 && STRNCMP(*arg, "tuple", len) == 0)
	    {
		*arg += len;
		واپس parse_type_tuple(arg, type_gap, give_error, ufunc,
									cctx);
	    }
	    توڑو;
	حالت 'v':
	    اگر (len == 4 && STRNCMP(*arg, "void", len) == 0)
	    {
		*arg += len;
		واپس &t_void;
	    }
	    توڑو;
    }

    // User defined type
    واپس parse_type_user_defined(arg, len, type_gap, give_error, ufunc,
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

    اگر (parse_گہرائی > MAX_TYPE_گہرائی)
    {
	اگر (give_error)
	    emsg(_(e_type_nested_too_deep));
	واپس تہی;
    }
    اگر (parse_گہرائی == 0)
	parse_max_گہرائی = 0;
    ++parse_گہرائی;
    اگر (parse_گہرائی > parse_max_گہرائی)
	parse_max_گہرائی = parse_گہرائی;
    type = parse_type_inner(arg, type_gap, ufunc, cctx, give_error);
    --parse_گہرائی;
    واپس type;
}

/*
 * Return the number of levels of the type last parsed by parse_type().
 */
    صحیح
parse_type_گہرائی(خالی)
{
    واپس parse_max_گہرائی;
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

    اگر (type1 == تہی || type2 == تہی)
	واپس FALSE;
    اگر (type1->tt_type != type2->tt_type)
	واپس FALSE;
    انتخاب (type1->tt_type)
    {
	حالت VAR_UNKNOWN:
	حالت VAR_ANY:
	حالت VAR_VOID:
	حالت VAR_SPECIAL:
	حالت VAR_BOOL:
	حالت VAR_نمبر:
	حالت VAR_FLOAT:
	حالت VAR_STRING:
	حالت VAR_BLOB:
	حالت VAR_کام:
	حالت VAR_CHANNEL:
	حالت VAR_INSTR:
	حالت VAR_TYPEALIAS:
	    توڑو;  // not composite is always OK
	حالت VAR_OBJECT:
	حالت VAR_CLASS:
	    // Objects are considered equal if they are from the same class
	    واپس type1->tt_class == type2->tt_class;
	حالت VAR_LIST:
	حالت VAR_DICT:
	    واپس equal_type(type1->tt_member, type2->tt_member, flags);
	حالت VAR_TUPLE:
	    اگر (type1->tt_argcount != type2->tt_argcount)
		واپس FALSE;
	    اگر (type1->tt_argcount < 0
			   || type1->tt_دلائل == تہی || type2->tt_دلائل == تہی)
		واپس TRUE;
	    برائے (i = 0; i < type1->tt_argcount; ++i)
		اگر ((flags & ETYPE_ARG_UNKNOWN) == 0
			&& !equal_type(type1->tt_دلائل[i], type2->tt_دلائل[i],
									flags))
		    واپس FALSE;
	    واپس TRUE;
	حالت VAR_FUNC:
	حالت VAR_PARTIAL:
	    اگر (!equal_type(type1->tt_member, type2->tt_member, flags)
		    || type1->tt_argcount != type2->tt_argcount)
		واپس FALSE;
	    اگر (type1->tt_argcount < 0
			   || type1->tt_دلائل == تہی || type2->tt_دلائل == تہی)
		واپس TRUE;
	    برائے (i = 0; i < type1->tt_argcount; ++i)
		اگر ((flags & ETYPE_ARG_UNKNOWN) == 0
			&& !equal_type(type1->tt_دلائل[i], type2->tt_دلائل[i],
									flags))
		    واپس FALSE;
	    واپس TRUE;
    }
    واپس TRUE;
}

/*
 * Find the common type of "type1" (VAR_FUNC) and "type2" (VAR_FUNC) and put it
 * in "dest".  "type2" and "dest" may be the same.
 */
    ساکن خالی
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
	واپس;
    }
    اگر (type2 == &t_func_unknown)
    {
	*dest = type1;
	واپس;
    }

    common_type(type1->tt_member, type2->tt_member, &common, type_gap);
    اگر (type1->tt_argcount == type2->tt_argcount
	    && type1->tt_argcount >= 0)
    {
	صحیح argcount = type1->tt_argcount;
	صحیح i;

	*dest = alloc_func_type(common, argcount, type_gap);
	اگر (type1->tt_دلائل != تہی && type2->tt_دلائل != تہی)
	{
	    اگر (func_type_جوڑو_arg_types(*dest, argcount,
			type_gap) == OK)
		برائے (i = 0; i < argcount; ++i)
		    common_type(type1->tt_دلائل[i], type2->tt_دلائل[i],
			    &(*dest)->tt_دلائل[i], type_gap);
	}
    }
    ورنہ
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
    خالی
common_type(type_T *type1, type_T *type2, type_T **dest, garray_T *type_gap)
{
    اگر (equal_type(type1, type2, 0))
    {
	*dest = type1;
	واپس;
    }

    // If either is VAR_UNKNOWN use the other type.  An empty list/dict has no
    // specific type.
    اگر (type1 == تہی || type1->tt_type == VAR_UNKNOWN)
    {
	*dest = type2;
	واپس;
    }
    اگر (type2 == تہی || type2->tt_type == VAR_UNKNOWN)
    {
	*dest = type1;
	واپس;
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
	    ورنہ
		*dest = get_dict_type(common, type_gap);
	    واپس;
	}
	ورنہ اگر (type1->tt_type == VAR_FUNC)
	{
	    common_type_var_func(type1, type2, dest, type_gap);
	    واپس;
	}
	ورنہ اگر (type1->tt_type == VAR_OBJECT)
	{
	    *dest = &t_object_any;
	    واپس;
	}
    }

    *dest = &t_any;
}

/*
 * Return the item type of a List, Dict or a Tuple
 */
    type_T *
get_شے_type(type_T *type)
{
    اگر (type->tt_type == VAR_TUPLE)
    {
	اگر (type->tt_argcount != 1)
	    واپس &t_any;

	اگر (type->tt_flags & TTFLAG_VARARGS)
	    واپس type->tt_دلائل[0]->tt_member;
	ورنہ
	    واپس type->tt_دلائل[0];
    }

    // For "any" the item type is not known.
    واپس type->tt_member == تہی ? &t_any : type->tt_member;
}

/*
 * Push an entry onto the type stack.  "type" used both for the current type
 * and the declared type.
 * Returns FAIL when out of memory.
 */
    صحیح
ڈالو_type_ڈھیر(cctx_T *cctx, type_T *type)
{
    واپس ڈالو_type_stack2(cctx, type, type);
}

/*
 * Push an entry onto the type stack.  "type" is the current type, "decl_type"
 * is the declared type.
 * Returns FAIL when out of memory.
 */
    صحیح
ڈالو_type_stack2(cctx_T *cctx, type_T *type, type_T *decl_type)
{
    garray_T	*ڈھیر = &cctx->ctx_type_ڈھیر;
    type2_T	*typep;

    اگر (GA_GROW_FAILS(ڈھیر, 1))
	واپس FAIL;
    typep = ((type2_T *)ڈھیر->ga_ڈیٹا) + ڈھیر->ga_len;
    typep->type_curr = type;
    typep->type_decl = decl_type;
    ++ڈھیر->ga_len;
    واپس OK;
}

/*
 * Set the type of the top of the stack to "type".
 */
    خالی
سیٹ_type_on_ڈھیر(cctx_T *cctx, type_T *type, صحیح انحراف)
{
    garray_T	*ڈھیر = &cctx->ctx_type_ڈھیر;
    type2_T	*typep = ((type2_T *)ڈھیر->ga_ڈیٹا)
						  + ڈھیر->ga_len - 1 - انحراف;

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
get_type_on_ڈھیر(cctx_T *cctx, صحیح انحراف)
{
    garray_T	*ڈھیر = &cctx->ctx_type_ڈھیر;

    اگر (انحراف + 1 > ڈھیر->ga_len)
	واپس &t_unknown;
    واپس (((type2_T *)ڈھیر->ga_ڈیٹا) + ڈھیر->ga_len - انحراف - 1)
								   ->type_curr;
}

/*
 * Get the declared type from the type stack.  If "offset" is zero the one at
 * the top,
 * if "offset" is one the type above that, etc.
 * Returns &t_unknown if there is no such stack entry.
 */
    type_T *
get_decl_type_on_ڈھیر(cctx_T *cctx, صحیح انحراف)
{
    garray_T	*ڈھیر = &cctx->ctx_type_ڈھیر;

    اگر (انحراف + 1 > ڈھیر->ga_len)
	واپس &t_unknown;
    واپس (((type2_T *)ڈھیر->ga_ڈیٹا) + ڈھیر->ga_len - انحراف - 1)
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
get_member_type_from_ڈھیر(
	صحیح	    گنتی,
	صحیح	    skip,
	cctx_T	    *cctx)
{
    garray_T	*ڈھیر = &cctx->ctx_type_ڈھیر;
    type2_T	*typep;
    garray_T    *type_gap = cctx->ctx_type_list;
    صحیح		i;
    type_T	*نتیجہ;
    type_T	*type;

    // Use "unknown" for an empty list or dict.
    اگر (گنتی == 0)
	واپس &t_unknown;
    // Find the common type from following items.
    typep = ((type2_T *)ڈھیر->ga_ڈیٹا) + ڈھیر->ga_len;
    نتیجہ = &t_unknown;
    برائے (i = 0; i < گنتی; ++i)
    {
	type = (typep -((گنتی - i) * skip) + skip - 1)->type_curr;
	اگر (check_type_is_قدر(type) == FAIL)
	    واپس تہی;
	اگر (نتیجہ != &t_any)
	    common_type(type, نتیجہ, &نتیجہ, type_gap);
    }

    واپس نتیجہ;
}

/*
 * Get the types of items in a tuple on the stack of "cctx".
 * Returns the number of types.  Returns -1 on failure.
 */
    صحیح
get_tuple_type_from_ڈھیر(
    صحیح		گنتی,
    garray_T	*tuple_types_gap,
    cctx_T	*cctx)
{
    garray_T	*ڈھیر = &cctx->ctx_type_ڈھیر;
    type2_T	*typep;
    type_T	*type = تہی;

    // Use "unknown" for an empty tuple
    اگر (گنتی == 0)
	واپس 0;

    // Find the common type from following items.
    typep = ((type2_T *)ڈھیر->ga_ڈیٹا) + ڈھیر->ga_len;
    برائے (صحیح i = 0; i < گنتی; i++)
    {
	type = (typep - (گنتی - i))->type_curr;
	اگر (check_type_is_قدر(type) == FAIL)
	    واپس -1;
	اگر (ga_grow(tuple_types_gap, 1) == FAIL)
	    واپس -1;
	((type_T **)tuple_types_gap->ga_ڈیٹا)[tuple_types_gap->ga_len] = type;
	tuple_types_gap->ga_len++;
    }

    واپس tuple_types_gap->ga_len;
}

    حرف *
vartype_name(vartype_T type)
{
    انتخاب (type)
    {
	حالت VAR_UNKNOWN: توڑو;
	حالت VAR_ANY: واپس "any";
	حالت VAR_VOID: واپس "void";
	حالت VAR_SPECIAL: واپس "special";
	حالت VAR_BOOL: واپس "bool";
	حالت VAR_نمبر: واپس "number";
	حالت VAR_FLOAT: واپس "float";
	حالت VAR_STRING: واپس "string";
	حالت VAR_BLOB: واپس "blob";
	حالت VAR_کام: واپس "job";
	حالت VAR_CHANNEL: واپس "channel";
	حالت VAR_LIST: واپس "list";
	حالت VAR_TUPLE: واپس "tuple";
	حالت VAR_DICT: واپس "dict";
	حالت VAR_INSTR: واپس "instr";
	حالت VAR_CLASS: واپس "class";
	حالت VAR_OBJECT: واپس "object";
	حالت VAR_TYPEALIAS: واپس "typealias";

	حالت VAR_FUNC:
	حالت VAR_PARTIAL: واپس "func";
    }
    واپس "unknown";
}

/*
 * Return the type name of a List (list<type>) or Dict (dict<type>).
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    ساکن حرف *
type_name_list_or_dict(حرف *name, type_T *type, حرف **tofree)
{
    حرف *member_free;
    حرف *member_name;

    اگر (type->tt_member->tt_type == VAR_UNKNOWN)
	member_name = type_name(&t_any, &member_free);
    ورنہ
	member_name = type_name(type->tt_member, &member_free);

    size_t len = STRLEN(name) + STRLEN(member_name) + 3;
    *tofree = alloc(len);
    اگر (*tofree == تہی)
    {
	vim_free(member_free);
	واپس name;
    }

    vim_snprintf(*tofree, len, "%s<%s>", name, member_name);
    vim_free(member_free);
    واپس *tofree;
}

/*
 * Return the type name of a tuple.
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    ساکن حرف *
type_name_tuple(type_T *type, حرف **tofree)
{
    garray_T    ga;
    صحیح		i;
    صحیح		varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;
    حرف	*arg_free = تہی;

    ga_init2(&ga, 1, 100);
    اگر (ga_grow(&ga, 20) == FAIL)
	جاؤ failed;
    STRCPY(ga.ga_ڈیٹا, "tuple<");
    ga.ga_len += 6;

    اگر (type->tt_argcount <= 0)
	// empty tuple
	GA_CONCAT_LITERAL(&ga, "any");
    ورنہ
    {
	اگر (type->tt_دلائل == تہی)
	    GA_CONCAT_LITERAL(&ga, "[unknown]");
	ورنہ
	{
	    برائے (i = 0; i < type->tt_argcount; ++i)
	    {
		string_T    arg_type;

		arg_type.string = (char_u *)type_name(type->tt_دلائل[i], &arg_free);
		اگر (i > 0)
		{
		    STRCPY((حرف *)ga.ga_ڈیٹا + ga.ga_len, ", ");
		    ga.ga_len += 2;
		}
		arg_type.لمبائی = STRLEN(arg_type.string);
		اگر (ga_grow(&ga, (صحیح)arg_type.لمبائی + 8) == FAIL)
		    جاؤ failed;
		اگر (varargs && i == type->tt_argcount - 1)
		    GA_CONCAT_LITERAL(&ga, "...");
		ga_concat_len(&ga, arg_type.string, arg_type.لمبائی);
		VIM_CLEAR(arg_free);
	    }
	}
    }

    STRCPY((حرف *)ga.ga_ڈیٹا + ga.ga_len, ">");
    *tofree = ga.ga_ڈیٹا;
    واپس ga.ga_ڈیٹا;

failed:
    vim_free(arg_free);
    ga_clear(&ga);
    واپس "[unknown]";
}

/*
 * Return the type name of a Class (class<name>) or Object (object<name>).
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    ساکن حرف *
type_name_class_or_obj(حرف *name, type_T *type, حرف **tofree)
{
    string_T	class_name;

    اگر (type->tt_class != تہی)
    {
	class_name.string = type->tt_class->class_name.string;
	class_name.لمبائی = type->tt_class->class_name.لمبائی;
	اگر (IS_ENUM(type->tt_class))
	    name = "enum";
    }
    ورنہ
	STR_LITERAL_سیٹ(class_name, "any");

    size_t len = STRLEN(name) + class_name.لمبائی + 3;
    *tofree = alloc(len);
    اگر (*tofree == تہی)
	واپس name;

    vim_snprintf(*tofree, len, "%s<%s>", name, class_name.string);
    واپس *tofree;
}

/*
 * Return the type name of a function.
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    ساکن حرف *
type_name_func(type_T *type, حرف **tofree)
{
    garray_T    ga;
    صحیح		i;
    صحیح		varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;
    حرف	*arg_free = تہی;

    ga_init2(&ga, 1, 100);
    اگر (ga_grow(&ga, 20) == FAIL)
	جاؤ failed;
    STRCPY(ga.ga_ڈیٹا, "func(");
    ga.ga_len += 5;

    برائے (i = 0; i < type->tt_argcount; ++i)
    {
	string_T    arg_type;

	اگر (type->tt_دلائل == تہی)
	    STR_LITERAL_سیٹ(arg_type, "[unknown]");
	ورنہ
	{
	    arg_type.string = (char_u *)type_name(type->tt_دلائل[i], &arg_free);
	    arg_type.لمبائی = STRLEN(arg_type.string);
	}
	اگر (i > 0)
	{
	    STRCPY((حرف *)ga.ga_ڈیٹا + ga.ga_len, ", ");
	    ga.ga_len += 2;
	}
	اگر (ga_grow(&ga, (صحیح)arg_type.لمبائی + 8) == FAIL)
	    جاؤ failed;
	اگر (varargs && i == type->tt_argcount - 1)
	    GA_CONCAT_LITERAL(&ga, "...");
	ورنہ اگر (i >= type->tt_min_argcount)
	    *((حرف *)ga.ga_ڈیٹا + ga.ga_len++) = '?';
	ga_concat_len(&ga, arg_type.string, arg_type.لمبائی);
	VIM_CLEAR(arg_free);
    }
    اگر (type->tt_argcount < 0)
	// any number of arguments
	GA_CONCAT_LITERAL(&ga, "...");

    اگر (type->tt_member == &t_void)
	STRCPY((حرف *)ga.ga_ڈیٹا + ga.ga_len, ")");
    ورنہ
    {
	حرف *ret_free = تہی;
	حرف *ret_name = type_name(type->tt_member, &ret_free);
	صحیح  len;

	len = (صحیح)STRLEN(ret_name) + 4;
	اگر (ga_grow(&ga, len) == FAIL)
	{
	    vim_free(ret_free);
	    جاؤ failed;
	}
	STRCPY((حرف *)ga.ga_ڈیٹا + ga.ga_len, "): ");
	STRCPY((حرف *)ga.ga_ڈیٹا + ga.ga_len + 3, ret_name);
	vim_free(ret_free);
    }
    *tofree = ga.ga_ڈیٹا;
    واپس ga.ga_ڈیٹا;

failed:
    vim_free(arg_free);
    ga_clear(&ga);
    واپس "[unknown]";
}

/*
 * Return the name of a type.
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    حرف *
type_name(type_T *type, حرف **tofree)
{
    حرف *name;

    *tofree = تہی;
    اگر (type == تہی)
	واپس "[unknown]";
    name = vartype_name(type->tt_type);

    انتخاب (type->tt_type)
    {
	حالت VAR_LIST:
	حالت VAR_DICT:
	    واپس type_name_list_or_dict(name, type, tofree);

	حالت VAR_TUPLE:
	    واپس type_name_tuple(type, tofree);

	حالت VAR_CLASS:
	حالت VAR_OBJECT:
	    واپس type_name_class_or_obj(name, type, tofree);

	حالت VAR_FUNC:
	    واپس type_name_func(type, tofree);

	طےشدہ:
	    توڑو;
    }

    واپس name;
}

/*
 * "typename(expr)" function
 */
    خالی
f_typename(typval_T *argvars, typval_T *rettv)
{
    garray_T	type_list;
    type_T	*type;
    حرف	*tofree;
    حرف	*name;

    rettv->v_type = VAR_STRING;
    ga_init2(&type_list, حجم(type_T *), 10);
    اگر (argvars[0].v_type == VAR_TYPEALIAS)
    {
	type = copy_type(argvars[0].vval.v_typealias->ta_type, &type_list);
	// A type alias for a class has the type set to VAR_OBJECT.  Change it
	// to VAR_CLASS, so that the name is "typealias<class<xxx>>"
	اگر (type->tt_type == VAR_OBJECT)
	    type->tt_type = VAR_CLASS;
    }
    ورنہ
	type = typval2type(argvars, get_copyID(), &type_list, TVTT_DO_MEMBER);
    name = type_name(type, &tofree);
    اگر (argvars[0].v_type == VAR_TYPEALIAS)
    {
	vim_snprintf((حرف *)IObuff, IOSIZE, "typealias<%s>", name);
	rettv->vval.v_string = vim_strsave((char_u *)IObuff);
	اگر (tofree != تہی)
	    vim_free(tofree);
    }
    ورنہ
    {
	اگر (tofree != تہی)
	    rettv->vval.v_string = (char_u *)tofree;
	ورنہ
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
check_typval_is_قدر(typval_T *tv)
{
    اگر (tv == تہی)
	واپس OK;

    انتخاب (tv->v_type)
    {
	حالت VAR_CLASS:
	    {
		class_T *cl = tv->vval.v_class;
		char_u *class_name = (cl == تہی) ? (char_u *)""
							: cl->class_name.string;
		اگر (cl != تہی && IS_ENUM(cl))
		    semsg(_(e_using_enum_as_قدر_str), class_name);
		ورنہ
		    semsg(_(e_using_class_as_قدر_str), class_name);
	    }
	    واپس FAIL;

	حالت VAR_TYPEALIAS:
	    semsg(_(e_using_typealias_as_قدر_str),
		    tv->vval.v_typealias->ta_name);
	    واپس FAIL;

	طےشدہ:
	    توڑو;
    }
    واپس OK;
}

/*
 * Same as above, except check type_T.
 */
    صحیح
check_type_is_قدر(type_T *type)
{
    اگر (type == تہی)
	واپس OK;
    انتخاب (type->tt_type)
    {
	حالت VAR_CLASS:
	    اگر (type->tt_class != تہی && IS_ENUM(type->tt_class))
		semsg(_(e_using_enum_as_قدر_str),
			type->tt_class->class_name.string);
	    ورنہ
		semsg(_(e_using_class_as_قدر_str),
			type->tt_class == تہی ? (char_u *)""
			: type->tt_class->class_name.string);
	    واپس FAIL;

	حالت VAR_TYPEALIAS:
	    // TODO: Not sure what could be done here to get a name.
	    //       Maybe an optional argument?
	    emsg(_(e_using_typealias_as_var_val));
	    واپس FAIL;

	طےشدہ:
	    توڑو;
    }
    واپس OK;
}

#اگر_ختم // FEAT_EVAL
