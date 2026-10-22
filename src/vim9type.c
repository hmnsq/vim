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
#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଯଦି defined(FEAT_EVAL)

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

    ଯଦି (ga_grow(type_gap, 1) == FAIL)
	ଫେରନ୍ତୁ ଖାଲି;
    type = ALLOC_CLEAR_ONE(type_T);
    ଯଦି (type == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ((type_T **)type_gap->ga_data)[type_gap->ga_len] = type;
    ++type_gap->ga_len;
    ଫେରନ୍ତୁ type;
}

/*
 * Make a shallow copy of "type".
 * When allocation fails returns "type".
 */
    type_T *
copy_type(type_T *type, garray_T *type_gap)
{
    type_T *copy = get_type_ptr(type_gap);

    ଯଦି (copy == ଖାଲି)
	ଫେରନ୍ତୁ type;
    *copy = *type;
    copy->tt_flags &= ~TTFLAG_STATIC;

    ଯଦି (type->tt_args != ଖାଲି
	   && func_type_add_arg_types(copy, type->tt_argcount, type_gap) == OK)
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < type->tt_argcount; ++i)
	    copy->tt_args[i] = type->tt_args[i];

    ଫେରନ୍ତୁ copy;
}

/*
 * Inner part of copy_type_deep().
 * When allocation fails returns "type".
 */
    ସ୍ଥିର type_T *
copy_type_deep_rec(type_T *type, garray_T *type_gap, garray_T *seen_types)
{
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < seen_types->ga_len; ++i)
	ଯଦି (((type_T **)seen_types->ga_data)[i * 2] == type)
	    // seen this type before, return the copy we made
	    ଫେରନ୍ତୁ ((type_T **)seen_types->ga_data)[i * 2 + 1];

    type_T *copy = copy_type(type, type_gap);
    ଯଦି (ga_grow(seen_types, 1) == FAIL)
	ଫେରନ୍ତୁ copy;
    ((type_T **)seen_types->ga_data)[seen_types->ga_len * 2] = type;
    ((type_T **)seen_types->ga_data)[seen_types->ga_len * 2 + 1] = copy;
    ++seen_types->ga_len;

    ଯଦି (copy->tt_member != ଖାଲି)
	copy->tt_member = copy_type_deep_rec(copy->tt_member,
							 type_gap, seen_types);
    ଯଦି (type->tt_args != ଖାଲି)
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < type->tt_argcount; ++i)
	    copy->tt_args[i] = copy_type_deep_rec(copy->tt_args[i],
							 type_gap, seen_types);

    ଫେରନ୍ତୁ copy;
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
    ga_init2(&seen_types, ଆକାର(type_T *) * 2, 20);

    type_T *res = copy_type_deep_rec(type, type_gap, &seen_types);

    ga_clear(&seen_types);
    ଫେରନ୍ତୁ res;
}

    ଶୂନ୍ୟ
clear_type_list(garray_T *gap)
{
    ଯେତେବେଳେ (gap->ga_len > 0)
	vim_free(((type_T **)gap->ga_data)[--gap->ga_len]);
    ga_clear(gap);
}

    ଶୂନ୍ୟ
clear_func_type_list(garray_T *gap, type_T **func_type)
{
    ଯେତେବେଳେ (gap->ga_len > 0)
    {
	// func_type pointing to the uf_type_list, so reset pointer
	ଯଦି (*func_type == ((type_T **)gap->ga_data)[--gap->ga_len])
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

    ଯଦି (type == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    // A fixed type never contains allocated types, return as-is.
    ଯଦି (type->tt_flags & TTFLAG_STATIC)
	ଫେରନ୍ତୁ type;

    ret = ALLOC_ONE(type_T);
    ଯଦି (ret == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    *ret = *type;

    ଯଦି (ret->tt_member != ଖାଲି)
	ret->tt_member = alloc_type(ret->tt_member);

    ଯଦି (type->tt_argcount > 0 && type->tt_args != ଖାଲି)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

	ret->tt_args = ALLOC_MULT(type_T *, type->tt_argcount);
	ଯଦି (ret->tt_args != ଖାଲି)
	    ପାଇଁ (i = 0; i < type->tt_argcount; ++i)
		ret->tt_args[i] = alloc_type(type->tt_args[i]);
    }
    ଅନ୍ୟଥା
	ret->tt_args = ଖାଲି;

    ଫେରନ୍ତୁ ret;
}

/*
 * Free a type that was created with alloc_type().
 */
    ଶୂନ୍ୟ
free_type(type_T *type)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    ଯଦି (type == ଖାଲି || (type->tt_flags & TTFLAG_STATIC))
	ଫେରନ୍ତୁ;
    ଯଦି (type->tt_args != ଖାଲି)
    {
	ପାଇଁ (i = 0; i < type->tt_argcount; ++i)
	    free_type(type->tt_args[i]);
	vim_free(type->tt_args);
    }

    free_type(type->tt_member);

    vim_free(type);
}

/*
 * Return TRUE if "type" is to be recursed into for setting the type.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
set_tv_type_recurse(type_T *type)
{
    ଫେରନ୍ତୁ type->tt_member != ଖାଲି
		&& (type->tt_member->tt_type == VAR_DICT
				|| type->tt_member->tt_type == VAR_LIST)
		&& type->tt_member->tt_member != ଖାଲି
		&& type->tt_member->tt_member != &t_any
		&& type->tt_member->tt_member != &t_unknown;
}

/*
 * Set the type of Dict "d" to "type"
 */
    ସ୍ଥିର ଶୂନ୍ୟ
set_tv_type_dict(dict_T *d, type_T *type)
{
    ଯଦି (d->dv_type == type)
	ଫେରନ୍ତୁ;

    free_type(d->dv_type);
    d->dv_type = alloc_type(type);

    // Need to recursively set the type of dict items?
    ଯଦି (!set_tv_type_recurse(type))
	ଫେରନ୍ତୁ;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		todo = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)d->dv_hashtab.ht_used;
    hashitem_T	*hi;
    dictitem_T	*di;

    FOR_ALL_HASHTAB_ବସ୍ତୁଗୁଡ଼ିକ(&d->dv_hashtab, hi, todo)
    {
	ଯଦି (!HASHITEM_EMPTY(hi))
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
    ସ୍ଥିର ଶୂନ୍ୟ
set_tv_type_list(list_T *l, type_T *type)
{
    ଯଦି (l->lv_type == type)
	ଫେରନ୍ତୁ;

    free_type(l->lv_type);
    l->lv_type = alloc_type(type);

    // Need to recursively set the type of list items?
    ଯଦି (l->lv_first == &range_list_item || !set_tv_type_recurse(type))
	ଫେରନ୍ତୁ;

    listitem_T	*li;

    FOR_ALL_LIST_ବସ୍ତୁଗୁଡ଼ିକ(l, li)
	set_tv_type(&li->li_tv, type->tt_member);
}

/*
 * Set the type of Tuple "tuple" to "type"
 */
    ସ୍ଥିର ଶୂନ୍ୟ
set_tv_type_tuple(tuple_T *tuple, type_T *type)
{
    ଯଦି (tuple->tv_type == type)
	ଫେରନ୍ତୁ;

    free_type(tuple->tv_type);
    tuple->tv_type = alloc_type(type);

    ଯଦି (type->tt_argcount <= 0)
	ଫେରନ୍ତୁ;

    // recursively set the type of list items
    type_T	*item_type;
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < tuple_len(tuple); i++)
    {
	ଯଦି ((type->tt_flags & TTFLAG_VARARGS) && (i >= type->tt_argcount - 1))
	    // For a variadic tuple, the last type is a List.  So use the
	    // List member type.
	    item_type = type->tt_args[type->tt_argcount - 1]->tt_member;
	ଅନ୍ୟଥା
	    item_type = type->tt_args[i];

	set_tv_type(TUPLE_ITEM(tuple, i), item_type);
    }
}

/*
 * Set the type of "tv" to "type" if it is a list or tuple or dict.
 */
    ଶୂନ୍ୟ
set_tv_type(typval_T *tv, type_T *type)
{
    ଯଦି (type->tt_type == VAR_ANY)
	// If the variable type is "any", then keep the value type.
	// e.g.  var x: any = [1, 2] or var y: any = {v: 1}
	ଫେରନ୍ତୁ;

    ଯଦି (tv->v_type == VAR_DICT && tv->vval.v_dict != ଖାଲି)
	set_tv_type_dict(tv->vval.v_dict, type);
    ଅନ୍ୟଥା ଯଦି (tv->v_type == VAR_LIST && tv->vval.v_list != ଖାଲି)
	set_tv_type_list(tv->vval.v_list, type);
    ଅନ୍ୟଥା ଯଦି (tv->v_type == VAR_TUPLE && tv->vval.v_tuple != ଖାଲି)
	set_tv_type_tuple(tv->vval.v_tuple, type);
}

/*
 * For a tuple type, reserve space for "typecount" types (including the
 * repeated type).
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
tuple_type_add_types(
    type_T	*tupletype,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		typecount,
    garray_T	*type_gap)
{
    // To make it easy to free the space needed for the types, add the
    // pointer to type_gap.
    ଯଦି (ga_grow(type_gap, 1) == FAIL)
	ଫେରନ୍ତୁ FAIL;
    tupletype->tt_args = alloc_clear_id(ଆକାର(type_T *) * typecount,
							  aid_tuple_type_args);
    ଯଦି (tupletype->tt_args == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    ((type_T **)type_gap->ga_data)[type_gap->ga_len] =
						(ଶୂନ୍ୟ *)tupletype->tt_args;
    ++type_gap->ga_len;
    ଫେରନ୍ତୁ OK;
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
    ଯଦି (member_type == ଖାଲି || (member_type->tt_type == VAR_ANY
					&& !IS_GENERIC_TYPE(member_type)))
	ଫେରନ୍ତୁ &t_list_any;
    ଯଦି (member_type->tt_type == VAR_VOID
	    || member_type->tt_type == VAR_UNKNOWN)
	ଫେରନ୍ତୁ &t_list_empty;
    ଯଦି (member_type->tt_type == VAR_BOOL)
	ଫେରନ୍ତୁ &t_list_bool;
    ଯଦି (member_type->tt_type == VAR_ସଂଖ୍ୟା)
	ଫେରନ୍ତୁ &t_list_ସଂଖ୍ୟା;
    ଯଦି (member_type->tt_type == VAR_STRING)
	ଫେରନ୍ତୁ &t_list_string;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    ଯଦି (type == ଖାଲି)
	ଫେରନ୍ତୁ &t_any;
    type->tt_type = VAR_LIST;
    type->tt_member = member_type;
    type->tt_argcount = 0;
    type->tt_args = ଖାଲି;
    ଫେରନ୍ତୁ type;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		typecount = tuple_types_gap->ga_len;

    // recognize commonly used types
    ଯଦି (typecount == 0)
	ଫେରନ୍ତୁ &t_tuple_any;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    ଯଦି (type == ଖାଲି)
	ଫେରନ୍ତୁ &t_any;
    type->tt_type = VAR_TUPLE;
    type->tt_member = ଖାଲି;
    ଯଦି (typecount > 0)
    {
	ଯଦି (tuple_type_add_types(type, typecount, type_gap) == FAIL)
	    ଫେରନ୍ତୁ &t_any;
	mch_memmove(type->tt_args, tuple_types, ଆକାର(type_T *) * typecount);
    }
    type->tt_argcount = typecount;
    type->tt_flags = 0;

    ଫେରନ୍ତୁ type;
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
    ଯଦି (member_type == ଖାଲି || (member_type->tt_type == VAR_ANY
					&& !IS_GENERIC_TYPE(member_type)))
	ଫେରନ୍ତୁ &t_dict_any;
    ଯଦି (member_type->tt_type == VAR_VOID
	    || member_type->tt_type == VAR_UNKNOWN)
	ଫେରନ୍ତୁ &t_dict_empty;
    ଯଦି (member_type->tt_type == VAR_BOOL)
	ଫେରନ୍ତୁ &t_dict_bool;
    ଯଦି (member_type->tt_type == VAR_ସଂଖ୍ୟା)
	ଫେରନ୍ତୁ &t_dict_ସଂଖ୍ୟା;
    ଯଦି (member_type->tt_type == VAR_STRING)
	ଫେରନ୍ତୁ &t_dict_string;

    // Not a common type, create a new entry.
    type = get_type_ptr(type_gap);
    ଯଦି (type == ଖାଲି)
	ଫେରନ୍ତୁ &t_any;
    type->tt_type = VAR_DICT;
    type->tt_member = member_type;
    type->tt_argcount = 0;
    type->tt_args = ଖାଲି;
    ଫେରନ୍ତୁ type;
}

/*
 * Allocate a new type for a function.
 */
    type_T *
alloc_func_type(type_T *ret_type, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount, garray_T *type_gap)
{
    type_T *type = get_type_ptr(type_gap);

    ଯଦି (type == ଖାଲି)
	ଫେରନ୍ତୁ &t_any;
    type->tt_type = VAR_FUNC;
    type->tt_member = ret_type == ଖାଲି ? &t_unknown : ret_type;
    type->tt_argcount = argcount;
    type->tt_args = ଖାଲି;
    ଫେରନ୍ତୁ type;
}

/*
 * Allocate a new type for a tuple.
 */
    ସ୍ଥିର type_T *
alloc_tuple_type(ପୂର୍ଣ୍ଣସଂଖ୍ୟା typecount, garray_T *type_gap)
{
    type_T *type = get_type_ptr(type_gap);

    ଯଦି (type == ଖାଲି)
	ଫେରନ୍ତୁ &t_any;
    type->tt_type = VAR_TUPLE;
    type->tt_member = ଖାଲି;
    type->tt_argcount = typecount;
    type->tt_args = ଖାଲି;
    ଫେରନ୍ତୁ type;
}

/*
 * Get a function type, based on the return type "ret_type".
 * "argcount" must be -1 or 0, a predefined type can be used.
 */
    type_T *
get_func_type(type_T *ret_type, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount, garray_T *type_gap)
{
    // recognize commonly used types
    ଯଦି (ret_type == &t_unknown || ret_type == ଖାଲି)
    {
	// (argcount == 0) is not possible
	ଫେରନ୍ତୁ &t_func_unknown;
    }
    ଯଦି (ret_type == &t_void)
    {
	ଯଦି (argcount == 0)
	    ଫେରନ୍ତୁ &t_func_0_void;
	ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ &t_func_void;
    }
    ଯଦି (ret_type == &t_any)
    {
	ଯଦି (argcount == 0)
	    ଫେରନ୍ତୁ &t_func_0_any;
	ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ &t_func_any;
    }
    ଯଦି (ret_type == &t_ସଂଖ୍ୟା)
    {
	ଯଦି (argcount == 0)
	    ଫେରନ୍ତୁ &t_func_0_ସଂଖ୍ୟା;
	ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ &t_func_ସଂଖ୍ୟା;
    }
    ଯଦି (ret_type == &t_string)
    {
	ଯଦି (argcount == 0)
	    ଫେରନ୍ତୁ &t_func_0_string;
	ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ &t_func_string;
    }

    ଫେରନ୍ତୁ alloc_func_type(ret_type, argcount, type_gap);
}

/*
 * For a function type, reserve space for "argcount" argument types (including
 * vararg).
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
func_type_add_arg_types(
	type_T	    *functype,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    argcount,
	garray_T    *type_gap)
{
    // To make it easy to free the space needed for the argument types, add the
    // pointer to type_gap.
    ଯଦି (ga_grow(type_gap, 1) == FAIL)
	ଫେରନ୍ତୁ FAIL;
    functype->tt_args = ALLOC_CLEAR_MULT(type_T *, argcount);
    ଯଦି (functype->tt_args == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    ((type_T **)type_gap->ga_data)[type_gap->ga_len] =
						     (ଶୂନ୍ୟ *)functype->tt_args;
    ++type_gap->ga_len;
    ଫେରନ୍ତୁ OK;
}

/*
 * Return TRUE if "type" is NULL, any or unknown.
 * This also works for const (comparing with &t_any and &t_unknown doesn't).
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
type_any_or_unknown(type_T *type)
{
    ଫେରନ୍ତୁ type == ଖାଲି || type->tt_type == VAR_ANY
					|| type->tt_type == VAR_UNKNOWN;
}

/*
 * Get a type_T for a "special" typval in "tv".
 */
    ସ୍ଥିର type_T *
special_typval2type(typval_T *tv)
{
    ବାଛ (tv->vval.v_ସଂଖ୍ୟା)
    {
	ସ୍ଥିତି VVAL_NULL:
	    ଫେରନ୍ତୁ &t_null;

	ସ୍ଥିତି VVAL_NONE:
	    ଫେରନ୍ତୁ &t_none;

	ସ୍ଥିତି VVAL_TRUE:
	ସ୍ଥିତି VVAL_FALSE:
	    ଫେରନ୍ତୁ &t_bool;

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଫେରନ୍ତୁ &t_unknown;
    }
}

/*
 * Get a type_T for a List typval in "tv".
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    ସ୍ଥିର type_T *
list_typval2type(typval_T *tv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID, garray_T *type_gap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
    list_T	*l = tv->vval.v_list;
    listitem_T	*li;
    type_T	*member_type = ଖାଲି;

    // An empty list has type list<unknown>, unless the type was specified
    // and is not list<any>.  This matters when assigning to a variable
    // with a specific list type.
    ଯଦି (l == ଖାଲି || (l->lv_first == ଖାଲି
		&& (l->lv_type == ଖାଲି || l->lv_type->tt_member == &t_any)))
	ଫେରନ୍ତୁ &t_list_empty;

    ଯଦି ((flags & TVTT_DO_MEMBER) == 0)
	ଫେରନ୍ତୁ &t_list_any;

    // If the type is list<any> go through the members, it may end up a
    // more specific type.
    ଯଦି (l->lv_type != ଖାଲି && (l->lv_first == ଖାଲି
		|| (flags & TVTT_MORE_SPECIFIC) == 0
		|| l->lv_type->tt_member != &t_any))
	// make a copy, lv_type may be freed if the list is freed
	ଫେରନ୍ତୁ copy_type_deep(l->lv_type, type_gap);

    ଯଦି (l->lv_first == &range_list_item)
	ଫେରନ୍ତୁ &t_list_ସଂଖ୍ୟା;

    ଯଦି (l->lv_copyID == copyID)
	// avoid recursion
	ଫେରନ୍ତୁ &t_list_any;

    l->lv_copyID = copyID;

    // Use the common type of all members.
    member_type = typval2type(&l->lv_first->li_tv, copyID, type_gap,
							TVTT_DO_MEMBER);
    ପାଇଁ (li = l->lv_first->li_next; li != ଖାଲି; li = li->li_next)
	common_type(typval2type(&li->li_tv, copyID, type_gap, TVTT_DO_MEMBER),
					member_type, &member_type, type_gap);

    // Reset copyID so that a shared reference to this list (not a circular
    // reference) can be processed again to get the correct type.
    l->lv_copyID = 0;

    ଫେରନ୍ତୁ get_list_type(member_type, type_gap);
}

/*
 * Get a type_T for a Tuple typval in "tv".
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    ସ୍ଥିର type_T *
tuple_typval2type(typval_T *tv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID, garray_T *type_gap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
    tuple_T	*tuple = tv->vval.v_tuple;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = tuple_len(tuple);
    type_T	*type = ଖାଲି;

    // An empty tuple has type tuple<unknown>, unless the type was specified
    // and is not tuple<any>.  This matters when assigning to a variable
    // with a specific tuple type.
    ଯଦି (tuple == ଖାଲି || (len == 0 && (tuple->tv_type == ଖାଲି
				|| tuple->tv_type->tt_argcount == 0)))
	ଫେରନ୍ତୁ &t_tuple_empty;

    ଯଦି ((flags & TVTT_DO_MEMBER) == 0)
	ଫେରନ୍ତୁ &t_tuple_any;

    // If the type is tuple<any> go through the members, it may end up a
    // more specific type.
    ଯଦି (tuple->tv_type != ଖାଲି && (len == 0
					|| (flags & TVTT_MORE_SPECIFIC) == 0))
	// make a copy, tv_type may be freed if the tuple is freed
	ଫେରନ୍ତୁ copy_type_deep(tuple->tv_type, type_gap);

    ଯଦି (tuple->tv_copyID == copyID)
	// avoid recursion
	ଫେରନ୍ତୁ &t_tuple_any;

    tuple->tv_copyID = copyID;

    garray_T	tuple_types_ga;
    ga_init2(&tuple_types_ga, ଆକାର(type_T *), 10);
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < len; i++)
    {
	type = typval2type(TUPLE_ITEM(tuple, i), copyID, type_gap,
							TVTT_DO_MEMBER);
	ଯଦି (ga_grow(&tuple_types_ga, 1) == FAIL)
	{
	    ga_clear(&tuple_types_ga);
	    // Reset copyID so that a shared reference to this tuple can be
	    // processed again.
	    tuple->tv_copyID = 0;
	    ଫେରନ୍ତୁ ଖାଲି;
	}
	((type_T **)tuple_types_ga.ga_data)[tuple_types_ga.ga_len] = type;
	tuple_types_ga.ga_len++;
    }

    type_T *tuple_type = get_tuple_type(&tuple_types_ga, type_gap);
    ga_clear(&tuple_types_ga);

    // Reset copyID so that a shared reference to this tuple (not a circular
    // reference) can be processed again to get the correct type.
    tuple->tv_copyID = 0;

    ଫେରନ୍ତୁ tuple_type;
}

/*
 * Get a type_T for a Dict typval in "tv".
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    ସ୍ଥିର type_T *
dict_typval2type(typval_T *tv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID, garray_T *type_gap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
    dict_iterator_T	iter;
    typval_T		*ମୂଲ୍ୟ;
    dict_T		*d = tv->vval.v_dict;
    type_T		*member_type = ଖାଲି;

    ଯଦି (d == ଖାଲି || (d->dv_hashtab.ht_used == 0 && d->dv_type == ଖାଲି))
	ଫେରନ୍ତୁ &t_dict_empty;

    ଯଦି ((flags & TVTT_DO_MEMBER) == 0)
	ଫେରନ୍ତୁ &t_dict_any;

    // If the type is dict<any> go through the members, it may end up a
    // more specific type.
    ଯଦି (d->dv_type != ଖାଲି && (d->dv_hashtab.ht_used == 0
		|| (flags & TVTT_MORE_SPECIFIC) == 0
		|| d->dv_type->tt_member != &t_any))
	ଫେରନ୍ତୁ d->dv_type;

    ଯଦି (d->dv_copyID == copyID)
	// avoid recursion
	ଫେରନ୍ତୁ &t_dict_any;

    d->dv_copyID = copyID;

    // Use the common type of all values.
    dict_iterate_start(tv, &iter);
    dict_iterate_next(&iter, &ମୂଲ୍ୟ);
    member_type = typval2type(ମୂଲ୍ୟ, copyID, type_gap, TVTT_DO_MEMBER);

    ଯେତେବେଳେ (dict_iterate_next(&iter, &ମୂଲ୍ୟ) != ଖାଲି)
	common_type(typval2type(ମୂଲ୍ୟ, copyID, type_gap, TVTT_DO_MEMBER),
					member_type, &member_type, type_gap);

    // Reset copyID so that a shared reference to this dict (not a circular
    // reference) can be processed again to get the correct type.
    d->dv_copyID = 0;

    ଫେରନ୍ତୁ get_dict_type(member_type, type_gap);
}

/*
 * Get a type_T for a "partial" typval in "tv".
 */
    ସ୍ଥିର type_T *
partial_typval2type(typval_T *tv, ufunc_T *ufunc, garray_T *type_gap)
{
    partial_T *pt = tv->vval.v_partial;
    type_T  *type;

    type = get_type_ptr(type_gap);
    ଯଦି (type == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    *type = *ufunc->uf_func_type;
    ଯଦି (type->tt_argcount >= 0 && pt->pt_argc > 0)
    {
	type->tt_argcount -= pt->pt_argc;
	type->tt_min_argcount -= pt->pt_argc;
	ଯଦି (type->tt_argcount > 0 && func_type_add_arg_types(type,
		    type->tt_argcount, type_gap) == OK)
	    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < type->tt_argcount; ++i)
		type->tt_args[i] =
		    ufunc->uf_func_type->tt_args[i + pt->pt_argc];
    }
    ଯଦି (pt->pt_func != ଖାଲି)
	type->tt_member = pt->pt_func->uf_ret_type;

    ଫେରନ୍ତୁ type;
}

/*
 * Get a type_T for a "class" or an "object" typval in "tv".
 */
    ସ୍ଥିର type_T *
oc_typval2type(typval_T *tv)
{
    ଯଦି (tv->v_type == VAR_CLASS)
    {
	ଯଦି (tv->vval.v_class == ଖାଲି)
	    ଫେରନ୍ତୁ &t_class;

	ଫେରନ୍ତୁ &tv->vval.v_class->class_type;
    }

    ଯଦି (tv->vval.v_object != ଖାଲି)
	ଫେରନ୍ତୁ &tv->vval.v_object->obj_class->class_object_type;

    ଫେରନ୍ତୁ &t_object_any;
}

/*
 * Get a type_T for a "function" or a "partial"
 */
    ସ୍ଥିର type_T *
fp_typval2type(typval_T *tv, garray_T *type_gap)
{
    char_u	*name = ଖାଲି;
    ufunc_T	*ufunc = ଖାଲି;
    type_T	*type;
    type_T	*member_type = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		argcount = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		min_argcount = 0;

    ଯଦି (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != ଖାଲି)
    {
	ଯଦି (tv->vval.v_partial->pt_func != ଖାଲି)
	    ufunc = tv->vval.v_partial->pt_func;
	ଅନ୍ୟଥା
	    name = tv->vval.v_partial->pt_name;
    }
    ଅନ୍ୟଥା
	name = tv->vval.v_string;

    ଯଦି (name == ଖାଲି && ufunc == ଖାଲି)
	ଫେରନ୍ତୁ &t_func_unknown;

    ଯଦି (name != ଖାଲି)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = find_internal_func(name);

	ଯଦି (idx >= 0)
	{
	    type_T *decl_type;  // unused

	    internal_func_get_argcount(idx, &argcount, &min_argcount);
	    member_type = internal_func_ret_type(idx, 0, ଖାଲି, &decl_type,
		    type_gap);
	}
	ଅନ୍ୟଥା
	{
	    // Check if name contains "<".  If it does, then replace "<" with
	    // NUL and lookup the function and then look up the specific
	    // generic function using the supplied types.
	    char_u *p = generic_func_find_open_bracket(name);
	    ଯଦି (p == ଖାଲି)
		ufunc = find_func(name, FALSE);
	    ଅନ୍ୟଥା
	    {
		// generic function
		char_u	cc = *p;

		*p = NUL;
		ufunc = find_func(name, FALSE);
		*p = cc;
		ଯଦି (ufunc != ଖାଲି && IS_GENERIC_FUNC(ufunc))
		{
		    ufunc = find_generic_func(ufunc, name, &p);
		    ଯଦି (ufunc == ଖାଲି)
			ଫେରନ୍ତୁ ଖାଲି;
		}
	    }
	}
    }
    ଯଦି (ufunc != ଖାଲି)
    {
	// May need to get the argument types from default values by
	// compiling the function.
	ଯଦି (ufunc->uf_def_status == UF_TO_BE_COMPILED
		&& compile_def_function(ufunc, TRUE, CT_NONE, ଖାଲି)
		== FAIL)
	    ଫେରନ୍ତୁ ଖାଲି;
	ଯଦି (ufunc->uf_func_type == ଖାଲି)
	    set_function_type(ufunc);
	ଯଦି (ufunc->uf_func_type != ଖାଲି)
	{
	    ଯଦି (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != ଖାଲି)
		ଫେରନ୍ତୁ partial_typval2type(tv, ufunc, type_gap);
	    ଫେରନ୍ତୁ ufunc->uf_func_type;
	}
    }

    type = get_type_ptr(type_gap);
    ଯଦି (type == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    type->tt_type = tv->v_type;
    type->tt_argcount = argcount;
    type->tt_min_argcount = min_argcount;
    ଯଦି (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != ଖାଲି
					&& tv->vval.v_partial->pt_argc > 0)
    {
	type->tt_argcount -= tv->vval.v_partial->pt_argc;
	type->tt_min_argcount -= tv->vval.v_partial->pt_argc;
    }
    type->tt_member = member_type;

    ଫେରନ୍ତୁ type;
}

/*
 * Get a type_T for a typval_T.
 * "type_gap" is used to temporarily create types in.
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the more specific member type if it
 * is "any".
 */
    ସ୍ଥିର type_T *
typval2type_int(typval_T *tv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID, garray_T *type_gap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
    ବାଛ (tv->v_type)
    {
	ସ୍ଥିତି VAR_UNKNOWN:
	    ଫେରନ୍ତୁ &t_unknown;

	ସ୍ଥିତି VAR_ANY:
	    ଫେରନ୍ତୁ &t_any;

	ସ୍ଥିତି VAR_VOID:
	    ଫେରନ୍ତୁ &t_void;

	ସ୍ଥିତି VAR_BOOL:
	    ଫେରନ୍ତୁ &t_bool;

	ସ୍ଥିତି VAR_SPECIAL:
	    ଫେରନ୍ତୁ special_typval2type(tv);

	ସ୍ଥିତି VAR_ସଂଖ୍ୟା:
	    ଫେରନ୍ତୁ &t_ସଂଖ୍ୟା;

	ସ୍ଥିତି VAR_FLOAT:
	    ଫେରନ୍ତୁ &t_float;

	ସ୍ଥିତି VAR_STRING:
	    ଫେରନ୍ତୁ &t_string;

	ସ୍ଥିତି VAR_BLOB:
	    ଯଦି (tv->vval.v_blob == ଖାଲି)
		ଫେରନ୍ତୁ &t_blob_null;
	    ଫେରନ୍ତୁ &t_blob;

	ସ୍ଥିତି VAR_LIST:
	    ଫେରନ୍ତୁ list_typval2type(tv, copyID, type_gap, flags);

	ସ୍ଥିତି VAR_TUPLE:
	    ଫେରନ୍ତୁ tuple_typval2type(tv, copyID, type_gap, flags);

	ସ୍ଥିତି VAR_DICT:
	    ଫେରନ୍ତୁ dict_typval2type(tv, copyID, type_gap, flags);

	ସ୍ଥିତି VAR_JOB:
	    ଫେରନ୍ତୁ &t_job;

	ସ୍ଥିତି VAR_CHANNEL:
	    ଫେରନ୍ତୁ &t_channel;

	ସ୍ଥିତି VAR_CLASS:
	ସ୍ଥିତି VAR_OBJECT:
	    ଫେରନ୍ତୁ oc_typval2type(tv);

	ସ୍ଥିତି VAR_TYPEALIAS:
	    ଫେରନ୍ତୁ &t_typealias;

	ସ୍ଥିତି VAR_FUNC:
	ସ୍ଥିତି VAR_PARTIAL:
	    ଫେରନ୍ତୁ fp_typval2type(tv, type_gap);

	ସ୍ଥିତି VAR_INSTR:
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଭାଙ୍ଗ;
    }

    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Return TRUE if "tv" is not a bool but should be converted to bool.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
need_convert_to_bool(type_T *type, typval_T *tv)
{
    ଫେରନ୍ତୁ type != ଖାଲି && type == &t_bool && tv->v_type != VAR_BOOL
	    && (tv->v_type == VAR_ସଂଖ୍ୟା
		       && (tv->vval.v_ସଂଖ୍ୟା == 0 || tv->vval.v_ସଂଖ୍ୟା == 1));
}

/*
 * Get a type_T for a typval_T.
 * "type_list" is used to temporarily create types in.
 * When "flags" has TVTT_DO_MEMBER also get the member type, otherwise use
 * "any".
 * When "flags" has TVTT_MORE_SPECIFIC get the most specific member type.
 */
    type_T *
typval2type(typval_T *tv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID, garray_T *type_gap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
    type_T *type = typval2type_int(tv, copyID, type_gap, flags);

    ଯଦି (type == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (type != &t_bool && (tv->v_type == VAR_ସଂଖ୍ୟା
		&& (tv->vval.v_ସଂଖ୍ୟା == 0 || tv->vval.v_ସଂଖ୍ୟା == 1)))
	// Number 0 and 1 and expression with "&&" or "||" can also be used
	// for bool.
	type = &t_ସଂଖ୍ୟା_bool;
    ଅନ୍ୟଥା ଯଦି (type != &t_float && tv->v_type == VAR_ସଂଖ୍ୟା)
	// A number can also be used for float.
	type = &t_ସଂଖ୍ୟା_float;
    ଫେରନ୍ତୁ type;
}

/*
 * Return TRUE if "type" can be used for a variable declaration.
 * Give an error and return FALSE if not.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
valid_declaration_type(type_T *type)
{
    ଯଦି (type->tt_type == VAR_SPECIAL  // null, none
	    || type->tt_type == VAR_VOID)
    {
	ଅକ୍ଷର *tofree = ଖାଲି;
	ଅକ୍ଷର *name = type_name(type, &tofree);
	semsg(_(e_invalid_type_in_variable_declaration_str), name);
	vim_free(tofree);
	ଫେରନ୍ତୁ FALSE;
    }
    ଫେରନ୍ତୁ TRUE;
}

/*
 * Get a type_T for a typval_T, used for v: variables.
 * "type_list" is used to temporarily create types in.
 */
    type_T *
typval2type_vimvar(typval_T *tv, garray_T *type_gap)
{
    ଯଦି (tv->v_type == VAR_LIST)  // e.g. for v:oldfiles
	ଫେରନ୍ତୁ &t_list_string;
    ଯଦି (tv->v_type == VAR_DICT)  // e.g. for v:event
	ଫେରନ୍ତୁ &t_dict_any;
    ଫେରନ୍ତୁ typval2type(tv, get_copyID(), type_gap, TVTT_DO_MEMBER);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_typval_arg_type(
	type_T	    *expected,
	typval_T    *actual_tv,
	ଅକ୍ଷର	    *func_name,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    arg_idx)
{
    where_T	where = WHERE_INIT;

    ଯଦି (arg_idx > 0)
    {
	where.wt_index = arg_idx;
	where.wt_kind = WT_ARGUMENT;
    }
    where.wt_func_name = func_name;
    ଫେରନ୍ତୁ check_typval_type(expected, actual_tv, where);
}

/*
 * Return true if "expected" is list<any> or dict<any> and "actual_tv" is a
 * list or dict of that kind, which matches whatever its values are.
 */
    ସ୍ଥିର bool
matches_listdict_any(type_T *expected, typval_T *actual_tv)
{
    ଫେରନ୍ତୁ (expected->tt_type == VAR_LIST || expected->tt_type == VAR_DICT)
	    && actual_tv->v_type == expected->tt_type
	    && expected->tt_member != ଖାଲି
	    && (expected->tt_member->tt_type == VAR_ANY
		|| expected->tt_member->tt_type == VAR_UNKNOWN);
}

/*
 * Return FAIL if "expected" and "actual" don't match.
 * When "argidx" > 0 it is included in the error message.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_typval_type(type_T *expected, typval_T *actual_tv, where_T where)
{
    garray_T	type_list;
    type_T	*actual_type;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		res = FAIL;

    ଯଦି (expected == ଖାଲି)
	ଫେରନ୍ତୁ OK;  // didn't expect anything.

    // No need to go through the values of a possibly big list or dict.
    ଯଦି (matches_listdict_any(expected, actual_tv))
	ଫେରନ୍ତୁ OK;

    ga_init2(&type_list, ଆକାର(type_T *), 10);

    // A null_function and null_partial are special cases, they can be used to
    // clear a variable.
    ଯଦି ((actual_tv->v_type == VAR_FUNC && actual_tv->vval.v_string == ଖାଲି)
	    || (actual_tv->v_type == VAR_PARTIAL
					 && actual_tv->vval.v_partial == ଖାଲି))
	actual_type = &t_func_unknown;
    ଅନ୍ୟଥା
	// When the actual type is list<any> or dict<any> go through the values
	// to possibly get a more specific type.
	actual_type = typval2type(actual_tv, get_copyID(), &type_list,
					  TVTT_DO_MEMBER | TVTT_MORE_SPECIFIC);
    ଯଦି (actual_type != ଖାଲି)
    {
	res = check_type_maybe(expected, actual_type, TRUE, where);
	ଯଦି (res == MAYBE && !(actual_type->tt_type == VAR_FUNC
				      && (actual_type->tt_member == &t_unknown
					  || actual_type->tt_member == ଖାଲି)))
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
    ଫେରନ୍ତୁ res;
}

    ଶୂନ୍ୟ
arg_type_mismatch(type_T *expected, type_T *actual, ପୂର୍ଣ୍ଣସଂଖ୍ୟା arg_idx)
{
    where_T	where = WHERE_INIT;

    ଯଦି (arg_idx > 0)
    {
	where.wt_index = arg_idx;
	where.wt_kind = WT_ARGUMENT;
    }
    type_mismatch_where(expected, actual, where);
}

    ଶୂନ୍ୟ
type_mismatch_where(type_T *expected, type_T *actual, where_T where)
{
    ଅକ୍ଷର *tofree1, *tofree2;
    ଅକ୍ଷର *typename1 = type_name(expected, &tofree1);
    ଅକ୍ଷର *typename2 = type_name(actual, &tofree2);

    ବାଛ (where.wt_kind)
    {
	ସ୍ଥିତି WT_MEMBER:
	    semsg(_(e_variable_str_type_mismatch_expected_str_but_got_str),
		    where.wt_func_name, typename1, typename2);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି WT_METHOD:
	ସ୍ଥିତି WT_METHOD_ARG:
	ସ୍ଥିତି WT_METHOD_RETURN:
	    semsg(_(e_method_str_type_mismatch_expected_str_but_got_str),
		    where.wt_func_name, typename1, typename2);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି WT_VARIABLE:
	    ଯଦି (where.wt_func_name == ଖାଲି)
		semsg(_(e_variable_nr_type_mismatch_expected_str_but_got_str),
			where.wt_index, typename1, typename2);
	    ଅନ୍ୟଥା
		semsg(_(e_variable_nr_type_mismatch_expected_str_but_got_str_in_str),
			where.wt_index, typename1, typename2, where.wt_func_name);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି WT_ARGUMENT:
	    ଯଦି (where.wt_func_name == ଖାଲି)
		semsg(_(e_argument_nr_type_mismatch_expected_str_but_got_str),
			where.wt_index, typename1, typename2);
	    ଅନ୍ୟଥା
		semsg(_(e_argument_nr_type_mismatch_expected_str_but_got_str_in_str),
			where.wt_index, typename1, typename2, where.wt_func_name);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି WT_CAST:
	ସ୍ଥିତି WT_UNKNOWN:
	    ଯଦି (where.wt_func_name == ଖାଲି)
		semsg(_(e_type_mismatch_expected_str_but_got_str),
			typename1, typename2);
	    ଅନ୍ୟଥା
		semsg(_(e_type_mismatch_expected_str_but_got_str_in_str),
			typename1, typename2, where.wt_func_name);
	    ଭାଙ୍ଗ;
    }

    vim_free(tofree1);
    vim_free(tofree2);
}

/*
 * Check if the expected and actual types match for a tuple
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_tuple_type_maybe(
    type_T	*expected,
    type_T	*actual,
    where_T	where)
{
    ଯଦି (expected->tt_argcount == -1 || actual->tt_argcount == -1
		|| expected->tt_args == ଖାଲି || actual->tt_args == ଖାଲି)
	ଫେରନ୍ତୁ OK;

    // For a non-variadic tuple, the number of items must match
    ଯଦି (!(expected->tt_flags & TTFLAG_VARARGS)
	    && expected->tt_argcount != actual->tt_argcount)
	ଫେରନ୍ତୁ FAIL;

    // compare the type of each tuple item
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < actual->tt_argcount; ++i)
    {
	type_T	*exp_type;
	type_T	*actual_type;

	ଯଦି (expected->tt_flags & TTFLAG_VARARGS)
	{
	    ଯଦି (i < expected->tt_argcount - 1)
		exp_type = expected->tt_args[i];
	    ଅନ୍ୟଥା
		// For a variadic tuple, the last type is a List.  So use the
		// List member type.
		exp_type = expected->tt_args[expected->tt_argcount - 1]->tt_member;
	}
	ଅନ୍ୟଥା
	    exp_type = expected->tt_args[i];

	ଯଦି (actual->tt_flags & TTFLAG_VARARGS)
	{
	    ଯଦି (i < actual->tt_argcount - 1)
		actual_type = actual->tt_args[i];
	    ଅନ୍ୟଥା
		// For a variadic tuple, the last type is a List.  So use the
		// List member type.
		actual_type = actual->tt_args[actual->tt_argcount - 1]->tt_member;
	}
	ଅନ୍ୟଥା
	    actual_type = actual->tt_args[i];

	// Allow for using "any" type for a tuple item
	ଯଦି (actual->tt_args[i] != &t_any && check_type(exp_type, actual_type,
						FALSE, where) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Check if the expected and actual types match for a function
 * Returns OK if "expected" and "actual" are matching function types.
 * Returns FAIL if "expected" and "actual" are different types.
 * Returns MAYBE when a runtime type check is needed.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_func_type_maybe(
    type_T	*expected,
    type_T	*actual,
    where_T	where)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = OK;

    // If the return type is unknown it can be anything, including
    // nothing, thus there is no point in checking.
    ଯଦି (expected->tt_member != &t_unknown)
    {
	ଯଦି (actual->tt_member != ଖାଲି
		&& actual->tt_member != &t_unknown)
	{
	    where_T  func_where = where;

	    func_where.wt_kind = WT_METHOD_RETURN;
	    ret = check_type_maybe(expected->tt_member,
		    actual->tt_member, FALSE,
		    func_where);
	}
	ଅନ୍ୟଥା
	    ret = MAYBE;
    }
    ଯଦି (ret != FAIL
	    && ((expected->tt_flags & TTFLAG_VARARGS)
		!= (actual->tt_flags & TTFLAG_VARARGS))
	    && expected->tt_argcount != -1)
	ret = FAIL;
    ଯଦି (ret != FAIL && expected->tt_argcount != -1
	    && actual->tt_min_argcount != -1
	    && (actual->tt_argcount == -1
		|| (actual->tt_argcount < expected->tt_min_argcount
		    || actual->tt_argcount > expected->tt_argcount)))
	ret = FAIL;
    ଯଦି (ret != FAIL && expected->tt_args != ଖାଲି
	    && actual->tt_args != ଖାଲି)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

	ପାଇଁ (i = 0; i < expected->tt_argcount
		&& i < actual->tt_argcount; ++i)
	{
	    where_T  func_where = where;
	    func_where.wt_kind = WT_METHOD_ARG;

	    // Allow for using "any" argument type, lambda's have them.
	    ଯଦି (actual->tt_args[i] != &t_any && check_type(
			expected->tt_args[i], actual->tt_args[i], FALSE,
			func_where) == FAIL)
	    {
		ret = FAIL;
		ଭାଙ୍ଗ;
	    }
	}
    }
    ଯଦି (ret == OK && expected->tt_argcount >= 0
	    && actual->tt_argcount == -1)
	// check the argument count at runtime
	ret = MAYBE;

    ଫେରନ୍ତୁ ret;
}

/*
 * Check if the expected and actual types match for an object
 * Returns OK if "expected" and "actual" are matching object types.
 * Returns FAIL if "expected" and "actual" are different types.
 * Returns MAYBE when a runtime type check is needed.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_object_type_maybe(
    type_T	*expected,
    type_T	*actual,
    where_T	where)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = OK;

    ଯଦି (actual->tt_type == VAR_ANY)
	ଫେରନ୍ତୁ MAYBE;	// use runtime type check
    ଯଦି (actual->tt_type != VAR_OBJECT)
	ଫେରନ୍ତୁ FAIL;	// don't use tt_class
    ଯଦି (actual->tt_class == ଖାଲି)    // null object
	ଫେରନ୍ତୁ OK;
    // t_object_any matches any object except for an enum item
    ଯଦି (expected == &t_object_any && !IS_ENUM(actual->tt_class))
	ଫେରନ୍ତୁ OK;

    // For object method arguments, do a invariant type check in
    // an extended class.  For all others, do a covariance type check.
    ଯଦି (where.wt_kind == WT_METHOD_ARG)
    {
	ଯଦି (actual->tt_class != expected->tt_class)
	    ret = FAIL;
    }
    ଅନ୍ୟଥା ଯଦି (!class_instance_of(actual->tt_class, expected->tt_class))
    {
	// Check if this is an up-cast, if so we'll have to check the type at
	// runtime.
	ଯଦି (where.wt_kind == WT_CAST &&
		class_instance_of(expected->tt_class, actual->tt_class))
	    ret = MAYBE;
	ଅନ୍ୟଥା
	    ret = FAIL;
    }

    ଫେରନ୍ତୁ ret;
}

/*
 * Check if the expected and actual types match.
 * Does not allow for assigning "any" to a specific type.
 * When "argidx" > 0 it is included in the error message.
 * Return OK if types match.
 * Return FAIL if types do not match.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_type(
	type_T	*expected,
	type_T	*actual,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	give_ସନ୍ଦେଶ,
	where_T where)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = check_type_maybe(expected, actual, give_ସନ୍ଦେଶ, where);

    ଫେରନ୍ତୁ ret == MAYBE ? OK : ret;
}

/*
 * As check_type() but return MAYBE when a runtime type check should be used
 * when compiling.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_type_maybe(
	type_T	*expected,
	type_T	*actual,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	give_ସନ୍ଦେଶ,
	where_T where)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = OK;

    // When expected is "unknown" we accept any actual type.
    // When expected is "any" we accept any actual type except "void".
    ଯଦି (expected->tt_type != VAR_UNKNOWN
	    && !(expected->tt_type == VAR_ANY && actual->tt_type != VAR_VOID))

    {
	// tt_type should match, except that a "partial" can be assigned to a
	// variable with type "func".
	// And "unknown" (using global variable) and "any" need a runtime type
	// check.
	ଯଦି (!(expected->tt_type == actual->tt_type
		    || actual->tt_type == VAR_UNKNOWN
		    || actual->tt_type == VAR_ANY
		    || (expected->tt_type == VAR_FUNC
					   && actual->tt_type == VAR_PARTIAL)))
	{
	    ଯଦି (expected->tt_type == VAR_BOOL
					&& (actual->tt_flags & TTFLAG_BOOL_OK))
		// Using number 0 or 1 for bool is OK.
		ଫେରନ୍ତୁ OK;
	    ଯଦି (expected->tt_type == VAR_FLOAT
		    && actual->tt_type == VAR_ସଂଖ୍ୟା
		    && ((expected->tt_flags & TTFLAG_ସଂଖ୍ୟା_OK)
			     || (actual->tt_flags & TTFLAG_FLOAT_OK)))
		// Using a number where a float is expected is OK here.
		ଫେରନ୍ତୁ OK;
	    ଯଦି (expected->tt_type == VAR_LIST
		    && actual->tt_type == VAR_TUPLE
		    && (expected->tt_flags & TTFLAG_TUPLE_OK))
		// Using a tuple where a list is expected is OK here.
		ଫେରନ୍ତୁ OK;
	    ଯଦି (give_ସନ୍ଦେଶ)
		type_mismatch_where(expected, actual, where);
	    ଫେରନ୍ତୁ FAIL;
	}
	ଯଦି (expected->tt_type == VAR_DICT || expected->tt_type == VAR_LIST)
	{
	    // "unknown" is used for an empty list or dict
	    ଯଦି (actual->tt_member != ଖାଲି && actual->tt_member != &t_unknown)
		ret = check_type_maybe(expected->tt_member, actual->tt_member,
								 FALSE, where);
	}
	ଅନ୍ୟଥା ଯଦି (expected->tt_type == VAR_TUPLE && actual != &t_any)
	    ret =  check_tuple_type_maybe(expected, actual, where);
	ଅନ୍ୟଥା ଯଦି (expected->tt_type == VAR_FUNC && actual != &t_any)
	    ret = check_func_type_maybe(expected, actual, where);
	ଅନ୍ୟଥା ଯଦି (expected->tt_type == VAR_OBJECT)
	    ret = check_object_type_maybe(expected, actual, where);

	ଯଦି (ret == FAIL && give_ସନ୍ଦେଶ)
	    type_mismatch_where(expected, actual, where);
    }

    ଯଦି (ret == OK && expected->tt_type != VAR_UNKNOWN
	    && expected->tt_type != VAR_ANY
	    && (actual->tt_type == VAR_UNKNOWN || actual->tt_type == VAR_ANY))
	// check the type at runtime
	ret = MAYBE;

    ଫେରନ୍ତୁ ret;
}

/*
 * Check that the arguments of "type" match "argvars[argcount]".
 * "base_tv" is from "expr->Func()".
 * Return OK/FAIL.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_argument_types(
	type_T	    *type,
	typval_T    *argvars,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    argcount,
	typval_T    *base_tv,
	char_u	    *name)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    totcount = argcount + (base_tv == ଖାଲି ? 0 : 1);

    ଯଦି (type->tt_type != VAR_FUNC && type->tt_type != VAR_PARTIAL)
	ଫେରନ୍ତୁ OK;  // just in case
    ଯଦି (totcount < type->tt_min_argcount - varargs)
    {
	emsg_funcname(e_not_enough_arguments_for_function_str, name);
	ଫେରନ୍ତୁ FAIL;
    }
    ଯଦି (!varargs && type->tt_argcount >= 0 && totcount > type->tt_argcount)
    {
	emsg_funcname(e_too_many_arguments_for_function_str, name);
	ଫେରନ୍ତୁ FAIL;
    }
    ଯଦି (type->tt_args == ଖାଲି)
	ଫେରନ୍ତୁ OK;  // cannot check


    ପାଇଁ (i = 0; i < totcount; ++i)
    {
	type_T	    *expected;
	typval_T    *tv;

	ଯଦି (base_tv != ଖାଲି)
	{
	    ଯଦି (i == 0)
		tv = base_tv;
	    ଅନ୍ୟଥା
		tv = &argvars[i - 1];
	}
	ଅନ୍ୟଥା
	    tv = &argvars[i];
	ଯଦି (varargs && i >= type->tt_argcount - 1)
	{
	    expected = type->tt_args[type->tt_argcount - 1];
	    ଯଦି (expected != ଖାଲି && expected->tt_type == VAR_LIST)
		expected = expected->tt_member;
	    ଯଦି (expected == ଖାଲି)
		expected = &t_any;
	}
	ଅନ୍ୟଥା
	    expected = type->tt_args[i];

	// check the type, unless the value is v:none
	ଯଦି ((tv->v_type != VAR_SPECIAL || tv->vval.v_ସଂଖ୍ୟା != VVAL_NONE)
		   && check_typval_arg_type(expected, tv, ଖାଲି, i + 1) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

// Maximum nesting of types, to avoid running out of stack space.  E.g.
// "list<list<number>>" is nested twice.
#define MAX_TYPE_DEPTH	1000

// Current number of levels of parse_type() and the deepest level reached while
// parsing the current type.  The nesting is one less than the number of
// levels.
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	parse_depth = 0;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	parse_max_depth = 0;

/*
 * Skip over type in list<type>, dict<type> or tuple<type>.
 * Returns a pointer to the character after the type.  "syn_error" is set to
 * TRUE on syntax error.
 */
    ସ୍ଥିର char_u *
skip_member_type(char_u *start, char_u *p, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *syn_error)
{
    ଯଦି (STRNCMP("tuple", start, 5) == 0)
    {
	// handle tuple<{type1}, {type2}, ....<type>>
	p = skipwhite(p + 1);
	ଯେତେବେଳେ (*p != '>' && *p != NUL)
	{
	    char_u *sp = p;

	    ଯଦି (STRNCMP(p, "...", 3) == 0)
		p += 3;
	    p = skip_type(p, TRUE);
	    ଯଦି (p == sp)
	    {
		*syn_error = TRUE;
		ଫେରନ୍ତୁ p;  // syntax error
	    }
	    ଯଦି (*p == ',')
		p = skipwhite(p + 1);
	}
	ଯଦି (*p == '>')
	    p++;
    }
    ଅନ୍ୟଥା
    {
	p = skipwhite(p);
	p = skip_type(skipwhite(p + 1), FALSE);
	p = skipwhite(p);
	ଯଦି (*p == '>')
	    ++p;
    }

    ଫେରନ୍ତୁ p;
}

/*
 * Skip over a function type.  Returns a pointer to the character after the
 * type.  "syn_error" is set to TRUE on syntax error.
 */
    ସ୍ଥିର char_u *
skip_func_type(char_u *p, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *syn_error)
{
    ଯଦି (*p == '(')
    {
	// handle func(args): type
	++p;
	ଯେତେବେଳେ (*p != ')' && *p != NUL)
	{
	    char_u *sp = p;

	    ଯଦି (STRNCMP(p, "...", 3) == 0)
		p += 3;
	    p = skip_type(p, TRUE);
	    ଯଦି (p == sp)
	    {
		*syn_error = TRUE;
		ଫେରନ୍ତୁ p;  // syntax error
	    }
	    ଯଦି (*p == ',')
		p = skipwhite(p + 1);
	}
	ଯଦି (*p == ')')
	{
	    ଯଦି (p[1] == ':')
		p = skip_type(skipwhite(p + 2), FALSE);
	    ଅନ୍ୟଥା
		++p;
	}
    }
    ଅନ୍ୟଥା
    {
	// handle func: return_type
	p = skip_type(skipwhite(p + 1), FALSE);
    }

    ଫେରନ୍ତୁ p;
}

/*
 * Inner part of skip_type().
 */
    ସ୍ଥିର char_u *
skip_type_inner(char_u *start, ପୂର୍ଣ୍ଣସଂଖ୍ୟା optional)
{
    char_u	*p = start;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		syn_error = FALSE;

    ଯଦି (optional && *p == '?')
	++p;

    // Also skip over "." for imported classes: "import.ClassName".
    ଯେତେବେଳେ (ASCII_ISALNUM(*p) || *p == '_' || *p == '.')
	++p;

    // Skip over "<type>"; this is permissive about white space.
    ଯଦି (*skipwhite(p) == '<')
    {
	p = skip_member_type(start, p, &syn_error);
	ଯଦି (syn_error)
	    ଫେରନ୍ତୁ p;
    }
    ଅନ୍ୟଥା ଯଦି ((*p == '(' || (*p == ':' && VIM_ISWHITE(p[1])))
					     && STRNCMP("func", start, 4) == 0)
    {
	// skip over function type
	p = skip_func_type(p, &syn_error);
	ଯଦି (syn_error)
	    ଫେରନ୍ତୁ p;
    }

    ଫେରନ୍ତୁ p;
}

/*
 * Skip over a type definition and return a pointer to just after it.
 * When "optional" is TRUE then a leading "?" is accepted.
 */
    char_u *
skip_type(char_u *start, ପୂର୍ଣ୍ଣସଂଖ୍ୟା optional)
{
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	skip_depth = 0;
    char_u	*p = start;

    // Too deeply nested: skip to the matching ">" without recursing, an
    // error is given when parsing the type.
    ଯଦି (skip_depth > MAX_TYPE_DEPTH)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	depth = 0;

	ପାଇଁ ( ; *p != NUL; ++p)
	{
	    ଯଦି (*p == '<')
		++depth;
	    ଅନ୍ୟଥା ଯଦି (*p == '>')
	    {
		ଯଦି (depth == 0)
		    ଭାଙ୍ଗ;
		ଯଦି (--depth == 0)
		    ଫେରନ୍ତୁ p + 1;
	    }
	    ଅନ୍ୟଥା ଯଦି (depth == 0 && !ASCII_ISALNUM(*p) && *p != '_'
								&& *p != '.')
		ଭାଙ୍ଗ;
	}
	ଫେରନ୍ତୁ p;
    }

    ++skip_depth;
    p = skip_type_inner(start, optional);
    --skip_depth;
    ଫେରନ୍ତୁ p;
}

/*
 * Parse the member type: "<type>" and return "type" with the member set.
 * Use "type_gap" if a new type needs to be added.
 * "info" is extra information for an error message.
 * Returns NULL in case of failure.
 */
    ସ୍ଥିର type_T *
parse_type_member(
	char_u	    **arg,
	type_T	    *type,
	garray_T    *type_gap,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    give_error,
	ଅକ୍ଷର	    *info,
	ufunc_T	    *ufunc,
	cctx_T	    *cctx)
{
    char_u  *arg_start = *arg;
    type_T  *member_type;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    prev_called_emsg = called_emsg;

    ଯଦି (**arg != '<')
    {
	ଯଦି (give_error)
	{
	    ଯଦି (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    ଅନ୍ୟଥା
		semsg(_(e_missing_type_after_str), info);
	}
	ଫେରନ୍ତୁ ଖାଲି;
    }
    *arg = skipwhite(*arg + 1);

    member_type = parse_type(arg, type_gap, ufunc, cctx, give_error);
    ଯଦି (member_type == ଖାଲି || !valid_declaration_type(member_type))
	ଫେରନ୍ତୁ ଖାଲି;

    *arg = skipwhite(*arg);
    ଯଦି (**arg != '>' && called_emsg == prev_called_emsg)
    {
	ଯଦି (give_error)
	    semsg(_(e_missing_gt_after_type_str), arg_start);
	ଫେରନ୍ତୁ ଖାଲି;
    }
    ++*arg;

    ଯଦି (type->tt_type == VAR_LIST)
	ଫେରନ୍ତୁ get_list_type(member_type, type_gap);
    ଫେରନ୍ତୁ get_dict_type(member_type, type_gap);
}

/*
 * Parse a "func" type at "*arg" and advance over it.
 * When "give_error" is TRUE give error messages, otherwise be quiet.
 * Return NULL for failure.
 */
    ସ୍ଥିର type_T *
parse_type_func(
    char_u	**arg,
    size_t	len,
    garray_T	*type_gap,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		give_error,
    ufunc_T	*ufunc,
    cctx_T	*cctx)
{
    char_u  *p;
    type_T  *type;
    type_T  *ret_type = &t_unknown;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	argcount = -1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	flags = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	first_optional = -1;
    type_T	*arg_type[MAX_FUNC_ARGS + 1];

    // func({type}, ...{type}): {type}
    *arg += len;
    ଯଦି (**arg == '(')
    {
	// "func" may or may not return a value, "func()" does
	// not return a value.
	ret_type = &t_void;

	p = ++*arg;
	argcount = 0;
	ଯେତେବେଳେ (*p != NUL && *p != ')')
	{
	    ଯଦି (*p == '?')
	    {
		ଯଦି (first_optional == -1)
		    first_optional = argcount;
		++p;
	    }
	    ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "...", 3) == 0)
	    {
		flags |= TTFLAG_VARARGS;
		p += 3;
	    }
	    ଅନ୍ୟଥା ଯଦି (first_optional != -1)
	    {
		ଯଦି (give_error)
		    emsg(_(e_mandatory_argument_after_optional_argument));
		ଫେରନ୍ତୁ ଖାଲି;
	    }

	    type = parse_type(&p, type_gap, ufunc, cctx, give_error);
	    ଯଦି (type == ଖାଲି || !valid_declaration_type(type))
		ଫେରନ୍ତୁ ଖାଲି;
	    ଯଦି ((flags & TTFLAG_VARARGS) != 0 && type->tt_type != VAR_LIST)
	    {
		ଅକ୍ଷର *tofree;
		semsg(_(e_variable_arguments_type_must_be_list_str),
			type_name(type, &tofree));
		vim_free(tofree);
		ଫେରନ୍ତୁ ଖାଲି;
	    }
	    arg_type[argcount++] = type;

	    // Nothing comes after "...{type}".
	    ଯଦି (flags & TTFLAG_VARARGS)
		ଭାଙ୍ଗ;

	    ଯଦି (*p != ',' && *skipwhite(p) == ',')
	    {
		ଯଦି (give_error)
		    semsg(_(e_no_white_space_allowed_before_str_str),
			    ",", p);
		ଫେରନ୍ତୁ ଖାଲି;
	    }
	    ଯଦି (*p == ',')
	    {
		++p;
		ଯଦି (!VIM_ISWHITE(*p))
		{
		    ଯଦି (give_error)
			semsg(_(e_white_space_required_after_str_str),
				",", p - 1);
		    ଫେରନ୍ତୁ ଖାଲି;
		}
	    }
	    p = skipwhite(p);
	    ଯଦି (argcount == MAX_FUNC_ARGS)
	    {
		ଯଦି (give_error)
		    emsg(_(e_too_many_argument_types));
		ଫେରନ୍ତୁ ଖାଲି;
	    }
	}

	p = skipwhite(p);
	ଯଦି (*p != ')')
	{
	    ଯଦି (give_error)
		emsg(_(e_missing_closing_paren));
	    ଫେରନ୍ତୁ ଖାଲି;
	}
	*arg = p + 1;
    }
    ଯଦି (**arg == ':')
    {
	// parse return type
	++*arg;
	ଯଦି (!VIM_ISWHITE(**arg) && give_error)
	    semsg(_(e_white_space_required_after_str_str), ":", *arg - 1);
	*arg = skipwhite(*arg);
	ret_type = parse_type(arg, type_gap, ufunc, cctx, give_error);
	ଯଦି (ret_type == ଖାଲି)
	    ଫେରନ୍ତୁ ଖାଲି;
    }
    ଯଦି (flags == 0 && first_optional == -1 && argcount <= 0)
	type = get_func_type(ret_type, argcount, type_gap);
    ଅନ୍ୟଥା
    {
	type = alloc_func_type(ret_type, argcount, type_gap);
	type->tt_flags = flags;
	ଯଦି (argcount > 0)
	{
	    type->tt_argcount = argcount;
	    type->tt_min_argcount = first_optional == -1
						? argcount : first_optional;
	    ଯଦି (func_type_add_arg_types(type, argcount, type_gap) == FAIL)
		ଫେରନ୍ତୁ ଖାଲି;
	    mch_memmove(type->tt_args, arg_type, ଆକାର(type_T *) * argcount);
	}
    }

    ଫେରନ୍ତୁ type;
}

/*
 * Parse a "tuple" type at "*arg" and advance over it.
 * When "give_error" is TRUE give error messages, otherwise be quiet.
 * Return NULL for failure.
 */
    ସ୍ଥିର type_T *
parse_type_tuple(
    char_u	**arg,
    garray_T	*type_gap,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		give_error,
    ufunc_T	*ufunc,
    cctx_T	*cctx)
{
    char_u	*p;
    type_T	*type;
    type_T	*ret_type = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		typecount = -1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags = 0;
    garray_T	tuple_types_ga;

    ga_init2(&tuple_types_ga, ଆକାର(type_T *), 10);

    // tuple<{type}, {type}>
    // tuple<{type}, ...{type}>
    ଯଦି (**arg != '<')
    {
	ଯଦି (give_error)
	{
	    ଯଦି (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    ଅନ୍ୟଥା
		semsg(_(e_missing_type_after_str), "tuple");
	}

	// only "tuple" is specified
	ଫେରନ୍ତୁ ଖାଲି;
    }

    p = ++*arg;
    typecount = 0;
    ଯେତେବେଳେ (*p != NUL && *p != '>')
    {
	ଯଦି (STRNCMP(p, "...", 3) == 0)
	{
	    flags |= TTFLAG_VARARGS;
	    p += 3;
	}

	type = parse_type(&p, type_gap, ufunc, cctx, give_error);
	ଯଦି (type == ଖାଲି || !valid_declaration_type(type))
	    ଯାଅ on_err;

	ଯଦି ((flags & TTFLAG_VARARGS) != 0 && type->tt_type != VAR_LIST)
	{
	    ଅକ୍ଷର *tofree;
	    semsg(_(e_variadic_tuple_must_end_with_list_type_str),
						type_name(type, &tofree));
	    vim_free(tofree);
	    ଯାଅ on_err;
	}

	// Add the item type
	ଯଦି (ga_grow(&tuple_types_ga, 1) == FAIL)
	    ଯାଅ on_err;
	((type_T **)tuple_types_ga.ga_data)[tuple_types_ga.ga_len] = type;
	tuple_types_ga.ga_len++;
	typecount++;

	// Nothing comes after "...{type}".
	ଯଦି (flags & TTFLAG_VARARGS)
	    ଭାଙ୍ଗ;

	ଯଦି (*p != ',' && *skipwhite(p) == ',')
	{
	    ଯଦି (give_error)
		semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
	    ଯାଅ on_err;
	}
	ଯଦି (*p == ',')
	{
	    ++p;
	    ଯଦି (!VIM_ISWHITE(*p))
	    {
		ଯଦି (give_error)
		    semsg(_(e_white_space_required_after_str_str),
			    ",", p - 1);
		ଯାଅ on_err;
	    }
	}
	p = skipwhite(p);
    }

    p = skipwhite(p);
    ଯଦି (*p != '>' || typecount <= 0)
    {
	ଯଦି (give_error)
	    semsg(_(e_missing_type_after_str), p);
	ଯାଅ on_err;
    }
    *arg = p + 1;

    ret_type = alloc_tuple_type(typecount, type_gap);
    ଯଦି (ret_type == &t_any
	    || tuple_type_add_types(ret_type, typecount, type_gap) == FAIL)
    {
	ret_type = ଖାଲି;
	ଯାଅ on_err;
    }
    ret_type->tt_flags = flags;
    ret_type->tt_argcount = typecount;
    mch_memmove(ret_type->tt_args, tuple_types_ga.ga_data,
						ଆକାର(type_T *) * typecount);

on_err:
    ga_clear(&tuple_types_ga);

    ଫେରନ୍ତୁ ret_type;
}

/*
 * Parse a "object" type at "*arg" and advance over it.
 * When "give_error" is TRUE give error messages, otherwise be quiet.
 * Return NULL for failure.
 */
    ସ୍ଥିର type_T *
parse_type_object(
    char_u	**arg,
    garray_T	*type_gap,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		give_error,
    cctx_T	*cctx)
{
    char_u	*arg_start = *arg;
    type_T	*object_type;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev_called_emsg = called_emsg;

    // object<X> or object<any>
    ଯଦି (**arg != '<')
    {
	ଯଦି (give_error)
	{
	    ଯଦି (*skipwhite(*arg) == '<')
		semsg(_(e_no_white_space_allowed_before_str_str), "<", *arg);
	    ଅନ୍ୟଥା
		semsg(_(e_missing_type_after_str), "object");
	}

	// only "object" is specified
	ଫେରନ୍ତୁ ଖାଲି;
    }

    // skip spaces following "object<"
    *arg = skipwhite(*arg + 1);

    object_type = parse_type(arg, type_gap, ଖାଲି, cctx, give_error);
    ଯଦି (object_type == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    *arg = skipwhite(*arg);
    ଯଦି (**arg != '>' && called_emsg == prev_called_emsg)
    {
	ଯଦି (give_error)
	    semsg(_(e_missing_gt_after_type_str), arg_start);
	ଫେରନ୍ତୁ ଖାଲି;
    }
    ++*arg;

    ଯଦି (object_type->tt_type == VAR_ANY)
	ଫେରନ୍ତୁ &t_object_any;

    ଯଦି (object_type->tt_type != VAR_OBJECT)
    {
	// specified type is not a class
	ଯଦି (give_error)
	    semsg(_(e_class_name_not_found_str), arg_start);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ଫେରନ୍ତୁ object_type;
}

/*
 * Parse a user defined type at "*arg" and advance over it.
 * It can be a class or an interface or a typealias name, possibly imported.
 * Return NULL if a type is not found.
 */
    ସ୍ଥିର type_T *
parse_type_user_defined(
    char_u	**arg,
    size_t	len,
    garray_T	*type_gap,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		give_error,
    ufunc_T	*ufunc,
    cctx_T	*cctx)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_emsg_before = did_emsg;
    typval_T	tv;

    tv.v_type = VAR_UNKNOWN;
    ଯଦି (eval_variable_import(*arg, &tv) == OK)
    {
	ଯଦି (tv.v_type == VAR_CLASS && tv.vval.v_class != ଖାଲି)
	{
	    type_T *type = get_type_ptr(type_gap);
	    ଯଦି (type != ଖାଲି)
	    {
		// Although the name is that of a class or interface, the type
		// uses will be an object.
		type->tt_type = VAR_OBJECT;
		type->tt_class = tv.vval.v_class;
		clear_tv(&tv);

		*arg += len;
		// Skip over ".ClassName".
		ଯେତେବେଳେ (ASCII_ISALNUM(**arg) || **arg == '_' || **arg == '.')
		    ++*arg;

		ଫେରନ୍ତୁ type;
	    }
	}
	ଅନ୍ୟଥା ଯଦି (tv.v_type == VAR_TYPEALIAS)
	{
	    // user defined type
	    // The type alias may itself be a nested type, check the resulting
	    // nesting depth.
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	depth = parse_depth - 1
				+ MAX(tv.vval.v_typealias->ta_depth, 1);
	    ଯଦି (depth - 1 > MAX_TYPE_DEPTH)
	    {
		clear_tv(&tv);
		ଯଦି (give_error)
		    emsg(_(e_type_nested_too_deep));
		ଫେରନ୍ତୁ ଖାଲି;
	    }
	    ଯଦି (depth > parse_max_depth)
		parse_max_depth = depth;

	    type_T *type = copy_type(tv.vval.v_typealias->ta_type, type_gap);
	    *arg += len;
	    clear_tv(&tv);
	    // Skip over ".TypeName".
	    ଯେତେବେଳେ (ASCII_ISALNUM(**arg) || **arg == '_' || **arg == '.')
		++*arg;
	    ଫେରନ୍ତୁ type;
	}

	clear_tv(&tv);
    }

    // Check whether it is a generic type
    type_T *type = find_generic_type(*arg, len, ufunc, cctx);
    ଯଦି (type != ଖାଲି)
    {
	*arg += len;
	ଫେରନ୍ତୁ type;
    }

    ଯଦି (give_error && (did_emsg == did_emsg_before))
    {
	char_u	*p = skip_type(*arg, FALSE);
	ଅକ୍ଷର	cc = *p;

	*p = NUL;
	semsg(_(e_type_not_recognized_str), *arg);
	*p = cc;
    }

    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Inner part of parse_type().
 */
    ସ୍ଥିର type_T *
parse_type_inner(
    char_u	**arg,
    garray_T	*type_gap,
    ufunc_T	*ufunc,
    cctx_T	*cctx,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		give_error)
{
    char_u  *p = *arg;
    size_t  len;

    // Skip over the first word.
    ଯେତେବେଳେ (ASCII_ISALNUM(*p) || *p == '_')
	++p;
    len = p - *arg;

    ବାଛ (**arg)
    {
	ସ୍ଥିତି 'a':
	    ଯଦି (len == 3 && STRNCMP(*arg, "any", len) == 0)
	    {
		*arg += len;
		ଫେରନ୍ତୁ &t_any;
	    }
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି 'b':
	    ଯଦି (len == 4 && STRNCMP(*arg, "bool", len) == 0)
	    {
		*arg += len;
		ଫେରନ୍ତୁ &t_bool;
	    }
	    ଯଦି (len == 4 && STRNCMP(*arg, "blob", len) == 0)
	    {
		*arg += len;
		ଫେରନ୍ତୁ &t_blob;
	    }
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି 'c':
	    ଯଦି (len == 7 && STRNCMP(*arg, "channel", len) == 0)
	    {
		*arg += len;
		ଫେରନ୍ତୁ &t_channel;
	    }
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି 'd':
	    ଯଦି (len == 4 && STRNCMP(*arg, "dict", len) == 0)
	    {
		*arg += len;
		ଫେରନ୍ତୁ parse_type_member(arg, &t_dict_any, type_gap,
						give_error, "dict", ufunc,
						cctx);
	    }
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି 'f':
	    ଯଦି (len == 5 && STRNCMP(*arg, "float", len) == 0)
	    {
		*arg += len;
		ଫେରନ୍ତୁ &t_float;
	    }
	    ଯଦି (len == 4 && STRNCMP(*arg, "func", len) == 0)
		ଫେରନ୍ତୁ parse_type_func(arg, len, type_gap, give_error, ufunc,
									cctx);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି 'j':
	    ଯଦି (len == 3 && STRNCMP(*arg, "job", len) == 0)
	    {
		*arg += len;
		ଫେରନ୍ତୁ &t_job;
	    }
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି 'l':
	    ଯଦି (len == 4 && STRNCMP(*arg, "list", len) == 0)
	    {
		*arg += len;
		ଫେରନ୍ତୁ parse_type_member(arg, &t_list_any, type_gap,
						give_error, "list", ufunc,
						cctx);
	    }
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି 'n':
	    ଯଦି (len == 6 && STRNCMP(*arg, "number", len) == 0)
	    {
		*arg += len;
		ଫେରନ୍ତୁ &t_ସଂଖ୍ୟା;
	    }
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି 'o':
	    ଯଦି (len == 6 && STRNCMP(*arg, "object", len) == 0)
	    {
		*arg += len;
		ଫେରନ୍ତୁ parse_type_object(arg, type_gap, give_error, cctx);
	    }
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି 's':
	    ଯଦି (len == 6 && STRNCMP(*arg, "string", len) == 0)
	    {
		*arg += len;
		ଫେରନ୍ତୁ &t_string;
	    }
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି 't':
	    ଯଦି (len == 5 && STRNCMP(*arg, "tuple", len) == 0)
	    {
		*arg += len;
		ଫେରନ୍ତୁ parse_type_tuple(arg, type_gap, give_error, ufunc,
									cctx);
	    }
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି 'v':
	    ଯଦି (len == 4 && STRNCMP(*arg, "void", len) == 0)
	    {
		*arg += len;
		ଫେରନ୍ତୁ &t_void;
	    }
	    ଭାଙ୍ଗ;
    }

    // User defined type
    ଫେରନ୍ତୁ parse_type_user_defined(arg, len, type_gap, give_error, ufunc,
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		give_error)
{
    type_T	*type;

    ଯଦି (parse_depth > MAX_TYPE_DEPTH)
    {
	ଯଦି (give_error)
	    emsg(_(e_type_nested_too_deep));
	ଫେରନ୍ତୁ ଖାଲି;
    }
    ଯଦି (parse_depth == 0)
	parse_max_depth = 0;
    ++parse_depth;
    ଯଦି (parse_depth > parse_max_depth)
	parse_max_depth = parse_depth;
    type = parse_type_inner(arg, type_gap, ufunc, cctx, give_error);
    --parse_depth;
    ଫେରନ୍ତୁ type;
}

/*
 * Return the number of levels of the type last parsed by parse_type().
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
parse_type_depth(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ parse_max_depth;
}

/*
 * Check if "type1" and "type2" are exactly the same.
 * "flags" can have ETYPE_ARG_UNKNOWN, which means that an unknown argument
 * type in "type1" is accepted.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
equal_type(type_T *type1, type_T *type2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    ଯଦି (type1 == ଖାଲି || type2 == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;
    ଯଦି (type1->tt_type != type2->tt_type)
	ଫେରନ୍ତୁ FALSE;
    ବାଛ (type1->tt_type)
    {
	ସ୍ଥିତି VAR_UNKNOWN:
	ସ୍ଥିତି VAR_ANY:
	ସ୍ଥିତି VAR_VOID:
	ସ୍ଥିତି VAR_SPECIAL:
	ସ୍ଥିତି VAR_BOOL:
	ସ୍ଥିତି VAR_ସଂଖ୍ୟା:
	ସ୍ଥିତି VAR_FLOAT:
	ସ୍ଥିତି VAR_STRING:
	ସ୍ଥିତି VAR_BLOB:
	ସ୍ଥିତି VAR_JOB:
	ସ୍ଥିତି VAR_CHANNEL:
	ସ୍ଥିତି VAR_INSTR:
	ସ୍ଥିତି VAR_TYPEALIAS:
	    ଭାଙ୍ଗ;  // not composite is always OK
	ସ୍ଥିତି VAR_OBJECT:
	ସ୍ଥିତି VAR_CLASS:
	    // Objects are considered equal if they are from the same class
	    ଫେରନ୍ତୁ type1->tt_class == type2->tt_class;
	ସ୍ଥିତି VAR_LIST:
	ସ୍ଥିତି VAR_DICT:
	    ଫେରନ୍ତୁ equal_type(type1->tt_member, type2->tt_member, flags);
	ସ୍ଥିତି VAR_TUPLE:
	    ଯଦି (type1->tt_argcount != type2->tt_argcount)
		ଫେରନ୍ତୁ FALSE;
	    ଯଦି (type1->tt_argcount < 0
			   || type1->tt_args == ଖାଲି || type2->tt_args == ଖାଲି)
		ଫେରନ୍ତୁ TRUE;
	    ପାଇଁ (i = 0; i < type1->tt_argcount; ++i)
		ଯଦି ((flags & ETYPE_ARG_UNKNOWN) == 0
			&& !equal_type(type1->tt_args[i], type2->tt_args[i],
									flags))
		    ଫେରନ୍ତୁ FALSE;
	    ଫେରନ୍ତୁ TRUE;
	ସ୍ଥିତି VAR_FUNC:
	ସ୍ଥିତି VAR_PARTIAL:
	    ଯଦି (!equal_type(type1->tt_member, type2->tt_member, flags)
		    || type1->tt_argcount != type2->tt_argcount)
		ଫେରନ୍ତୁ FALSE;
	    ଯଦି (type1->tt_argcount < 0
			   || type1->tt_args == ଖାଲି || type2->tt_args == ଖାଲି)
		ଫେରନ୍ତୁ TRUE;
	    ପାଇଁ (i = 0; i < type1->tt_argcount; ++i)
		ଯଦି ((flags & ETYPE_ARG_UNKNOWN) == 0
			&& !equal_type(type1->tt_args[i], type2->tt_args[i],
									flags))
		    ଫେରନ୍ତୁ FALSE;
	    ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ TRUE;
}

/*
 * Find the common type of "type1" (VAR_FUNC) and "type2" (VAR_FUNC) and put it
 * in "dest".  "type2" and "dest" may be the same.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
common_type_var_func(
    type_T	*type1,
    type_T	*type2,
    type_T	**dest,
    garray_T	*type_gap)
{
    type_T *common;

    // When one of the types is t_func_unknown return the other one.
    // Useful if a list or dict item is null_func.
    ଯଦି (type1 == &t_func_unknown)
    {
	*dest = type2;
	ଫେରନ୍ତୁ;
    }
    ଯଦି (type2 == &t_func_unknown)
    {
	*dest = type1;
	ଫେରନ୍ତୁ;
    }

    common_type(type1->tt_member, type2->tt_member, &common, type_gap);
    ଯଦି (type1->tt_argcount == type2->tt_argcount
	    && type1->tt_argcount >= 0)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount = type1->tt_argcount;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

	*dest = alloc_func_type(common, argcount, type_gap);
	ଯଦି (type1->tt_args != ଖାଲି && type2->tt_args != ଖାଲି)
	{
	    ଯଦି (func_type_add_arg_types(*dest, argcount,
			type_gap) == OK)
		ପାଇଁ (i = 0; i < argcount; ++i)
		    common_type(type1->tt_args[i], type2->tt_args[i],
			    &(*dest)->tt_args[i], type_gap);
	}
    }
    ଅନ୍ୟଥା
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
    ଶୂନ୍ୟ
common_type(type_T *type1, type_T *type2, type_T **dest, garray_T *type_gap)
{
    ଯଦି (equal_type(type1, type2, 0))
    {
	*dest = type1;
	ଫେରନ୍ତୁ;
    }

    // If either is VAR_UNKNOWN use the other type.  An empty list/dict has no
    // specific type.
    ଯଦି (type1 == ଖାଲି || type1->tt_type == VAR_UNKNOWN)
    {
	*dest = type2;
	ଫେରନ୍ତୁ;
    }
    ଯଦି (type2 == ଖାଲି || type2->tt_type == VAR_UNKNOWN)
    {
	*dest = type1;
	ଫେରନ୍ତୁ;
    }

    ଯଦି (type1->tt_type == type2->tt_type)
    {
	ଯଦି (type1->tt_type == VAR_LIST
		|| type1->tt_type == VAR_DICT)
	{
	    type_T *common;

	    common_type(type1->tt_member, type2->tt_member, &common, type_gap);
	    ଯଦି (type1->tt_type == VAR_LIST)
		*dest = get_list_type(common, type_gap);
	    ଅନ୍ୟଥା
		*dest = get_dict_type(common, type_gap);
	    ଫେରନ୍ତୁ;
	}
	ଅନ୍ୟଥା ଯଦି (type1->tt_type == VAR_FUNC)
	{
	    common_type_var_func(type1, type2, dest, type_gap);
	    ଫେରନ୍ତୁ;
	}
	ଅନ୍ୟଥା ଯଦି (type1->tt_type == VAR_OBJECT)
	{
	    *dest = &t_object_any;
	    ଫେରନ୍ତୁ;
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
    ଯଦି (type->tt_type == VAR_TUPLE)
    {
	ଯଦି (type->tt_argcount != 1)
	    ଫେରନ୍ତୁ &t_any;

	ଯଦି (type->tt_flags & TTFLAG_VARARGS)
	    ଫେରନ୍ତୁ type->tt_args[0]->tt_member;
	ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ type->tt_args[0];
    }

    // For "any" the item type is not known.
    ଫେରନ୍ତୁ type->tt_member == ଖାଲି ? &t_any : type->tt_member;
}

/*
 * Push an entry onto the type stack.  "type" used both for the current type
 * and the declared type.
 * Returns FAIL when out of memory.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ଠେଲ_type_ଗଦା(cctx_T *cctx, type_T *type)
{
    ଫେରନ୍ତୁ ଠେଲ_type_stack2(cctx, type, type);
}

/*
 * Push an entry onto the type stack.  "type" is the current type, "decl_type"
 * is the declared type.
 * Returns FAIL when out of memory.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ଠେଲ_type_stack2(cctx_T *cctx, type_T *type, type_T *decl_type)
{
    garray_T	*ଗଦା = &cctx->ctx_type_ଗଦା;
    type2_T	*typep;

    ଯଦି (GA_GROW_FAILS(ଗଦା, 1))
	ଫେରନ୍ତୁ FAIL;
    typep = ((type2_T *)ଗଦା->ga_data) + ଗଦା->ga_len;
    typep->type_curr = type;
    typep->type_decl = decl_type;
    ++ଗଦା->ga_len;
    ଫେରନ୍ତୁ OK;
}

/*
 * Set the type of the top of the stack to "type".
 */
    ଶୂନ୍ୟ
set_type_on_ଗଦା(cctx_T *cctx, type_T *type, ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset)
{
    garray_T	*ଗଦା = &cctx->ctx_type_ଗଦା;
    type2_T	*typep = ((type2_T *)ଗଦା->ga_data)
						  + ଗଦା->ga_len - 1 - offset;

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
get_type_on_ଗଦା(cctx_T *cctx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset)
{
    garray_T	*ଗଦା = &cctx->ctx_type_ଗଦା;

    ଯଦି (offset + 1 > ଗଦା->ga_len)
	ଫେରନ୍ତୁ &t_unknown;
    ଫେରନ୍ତୁ (((type2_T *)ଗଦା->ga_data) + ଗଦା->ga_len - offset - 1)
								   ->type_curr;
}

/*
 * Get the declared type from the type stack.  If "offset" is zero the one at
 * the top,
 * if "offset" is one the type above that, etc.
 * Returns &t_unknown if there is no such stack entry.
 */
    type_T *
get_decl_type_on_ଗଦା(cctx_T *cctx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset)
{
    garray_T	*ଗଦା = &cctx->ctx_type_ଗଦା;

    ଯଦି (offset + 1 > ଗଦା->ga_len)
	ଫେରନ୍ତୁ &t_unknown;
    ଫେରନ୍ତୁ (((type2_T *)ଗଦା->ga_data) + ଗଦା->ga_len - offset - 1)
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
get_member_type_from_ଗଦା(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ଗଣନା_2,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    skip,
	cctx_T	    *cctx)
{
    garray_T	*ଗଦା = &cctx->ctx_type_ଗଦା;
    type2_T	*typep;
    garray_T    *type_gap = cctx->ctx_type_list;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    type_T	*result;
    type_T	*type;

    // Use "unknown" for an empty list or dict.
    ଯଦି (ଗଣନା_2 == 0)
	ଫେରନ୍ତୁ &t_unknown;
    // Find the common type from following items.
    typep = ((type2_T *)ଗଦା->ga_data) + ଗଦା->ga_len;
    result = &t_unknown;
    ପାଇଁ (i = 0; i < ଗଣନା_2; ++i)
    {
	type = (typep -((ଗଣନା_2 - i) * skip) + skip - 1)->type_curr;
	ଯଦି (check_type_is_ମୂଲ୍ୟ(type) == FAIL)
	    ଫେରନ୍ତୁ ଖାଲି;
	ଯଦି (result != &t_any)
	    common_type(type, result, &result, type_gap);
    }

    ଫେରନ୍ତୁ result;
}

/*
 * Get the types of items in a tuple on the stack of "cctx".
 * Returns the number of types.  Returns -1 on failure.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_tuple_type_from_ଗଦା(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2,
    garray_T	*tuple_types_gap,
    cctx_T	*cctx)
{
    garray_T	*ଗଦା = &cctx->ctx_type_ଗଦା;
    type2_T	*typep;
    type_T	*type = ଖାଲି;

    // Use "unknown" for an empty tuple
    ଯଦି (ଗଣନା_2 == 0)
	ଫେରନ୍ତୁ 0;

    // Find the common type from following items.
    typep = ((type2_T *)ଗଦା->ga_data) + ଗଦା->ga_len;
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < ଗଣନା_2; i++)
    {
	type = (typep - (ଗଣନା_2 - i))->type_curr;
	ଯଦି (check_type_is_ମୂଲ୍ୟ(type) == FAIL)
	    ଫେରନ୍ତୁ -1;
	ଯଦି (ga_grow(tuple_types_gap, 1) == FAIL)
	    ଫେରନ୍ତୁ -1;
	((type_T **)tuple_types_gap->ga_data)[tuple_types_gap->ga_len] = type;
	tuple_types_gap->ga_len++;
    }

    ଫେରନ୍ତୁ tuple_types_gap->ga_len;
}

    ଅକ୍ଷର *
vartype_name(vartype_T type)
{
    ବାଛ (type)
    {
	ସ୍ଥିତି VAR_UNKNOWN: ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_ANY: ଫେରନ୍ତୁ "any";
	ସ୍ଥିତି VAR_VOID: ଫେରନ୍ତୁ "void";
	ସ୍ଥିତି VAR_SPECIAL: ଫେରନ୍ତୁ "special";
	ସ୍ଥିତି VAR_BOOL: ଫେରନ୍ତୁ "bool";
	ସ୍ଥିତି VAR_ସଂଖ୍ୟା: ଫେରନ୍ତୁ "number";
	ସ୍ଥିତି VAR_FLOAT: ଫେରନ୍ତୁ "float";
	ସ୍ଥିତି VAR_STRING: ଫେରନ୍ତୁ "string";
	ସ୍ଥିତି VAR_BLOB: ଫେରନ୍ତୁ "blob";
	ସ୍ଥିତି VAR_JOB: ଫେରନ୍ତୁ "job";
	ସ୍ଥିତି VAR_CHANNEL: ଫେରନ୍ତୁ "channel";
	ସ୍ଥିତି VAR_LIST: ଫେରନ୍ତୁ "list";
	ସ୍ଥିତି VAR_TUPLE: ଫେରନ୍ତୁ "tuple";
	ସ୍ଥିତି VAR_DICT: ଫେରନ୍ତୁ "dict";
	ସ୍ଥିତି VAR_INSTR: ଫେରନ୍ତୁ "instr";
	ସ୍ଥିତି VAR_CLASS: ଫେରନ୍ତୁ "class";
	ସ୍ଥିତି VAR_OBJECT: ଫେରନ୍ତୁ "object";
	ସ୍ଥିତି VAR_TYPEALIAS: ଫେରନ୍ତୁ "typealias";

	ସ୍ଥିତି VAR_FUNC:
	ସ୍ଥିତି VAR_PARTIAL: ଫେରନ୍ତୁ "func";
    }
    ଫେରନ୍ତୁ "unknown";
}

/*
 * Return the type name of a List (list<type>) or Dict (dict<type>).
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    ସ୍ଥିର ଅକ୍ଷର *
type_name_list_or_dict(ଅକ୍ଷର *name, type_T *type, ଅକ୍ଷର **tofree)
{
    ଅକ୍ଷର *member_free;
    ଅକ୍ଷର *member_name;

    ଯଦି (type->tt_member->tt_type == VAR_UNKNOWN)
	member_name = type_name(&t_any, &member_free);
    ଅନ୍ୟଥା
	member_name = type_name(type->tt_member, &member_free);

    size_t len = STRLEN(name) + STRLEN(member_name) + 3;
    *tofree = alloc(len);
    ଯଦି (*tofree == ଖାଲି)
    {
	vim_free(member_free);
	ଫେରନ୍ତୁ name;
    }

    vim_snprintf(*tofree, len, "%s<%s>", name, member_name);
    vim_free(member_free);
    ଫେରନ୍ତୁ *tofree;
}

/*
 * Return the type name of a tuple.
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    ସ୍ଥିର ଅକ୍ଷର *
type_name_tuple(type_T *type, ଅକ୍ଷର **tofree)
{
    garray_T    ga;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;
    ଅକ୍ଷର	*arg_free = ଖାଲି;

    ga_init2(&ga, 1, 100);
    ଯଦି (ga_grow(&ga, 20) == FAIL)
	ଯାଅ failed;
    STRCPY(ga.ga_data, "tuple<");
    ga.ga_len += 6;

    ଯଦି (type->tt_argcount <= 0)
	// empty tuple
	GA_CONCAT_LITERAL(&ga, "any");
    ଅନ୍ୟଥା
    {
	ଯଦି (type->tt_args == ଖାଲି)
	    GA_CONCAT_LITERAL(&ga, "[unknown]");
	ଅନ୍ୟଥା
	{
	    ପାଇଁ (i = 0; i < type->tt_argcount; ++i)
	    {
		string_T    arg_type;

		arg_type.string = (char_u *)type_name(type->tt_args[i], &arg_free);
		ଯଦି (i > 0)
		{
		    STRCPY((ଅକ୍ଷର *)ga.ga_data + ga.ga_len, ", ");
		    ga.ga_len += 2;
		}
		arg_type.length = STRLEN(arg_type.string);
		ଯଦି (ga_grow(&ga, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)arg_type.length + 8) == FAIL)
		    ଯାଅ failed;
		ଯଦି (varargs && i == type->tt_argcount - 1)
		    GA_CONCAT_LITERAL(&ga, "...");
		ga_concat_len(&ga, arg_type.string, arg_type.length);
		VIM_CLEAR(arg_free);
	    }
	}
    }

    STRCPY((ଅକ୍ଷର *)ga.ga_data + ga.ga_len, ">");
    *tofree = ga.ga_data;
    ଫେରନ୍ତୁ ga.ga_data;

failed:
    vim_free(arg_free);
    ga_clear(&ga);
    ଫେରନ୍ତୁ "[unknown]";
}

/*
 * Return the type name of a Class (class<name>) or Object (object<name>).
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    ସ୍ଥିର ଅକ୍ଷର *
type_name_class_or_obj(ଅକ୍ଷର *name, type_T *type, ଅକ୍ଷର **tofree)
{
    string_T	class_name;

    ଯଦି (type->tt_class != ଖାଲି)
    {
	class_name.string = type->tt_class->class_name.string;
	class_name.length = type->tt_class->class_name.length;
	ଯଦି (IS_ENUM(type->tt_class))
	    name = "enum";
    }
    ଅନ୍ୟଥା
	STR_LITERAL_SET(class_name, "any");

    size_t len = STRLEN(name) + class_name.length + 3;
    *tofree = alloc(len);
    ଯଦି (*tofree == ଖାଲି)
	ଫେରନ୍ତୁ name;

    vim_snprintf(*tofree, len, "%s<%s>", name, class_name.string);
    ଫେରନ୍ତୁ *tofree;
}

/*
 * Return the type name of a function.
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    ସ୍ଥିର ଅକ୍ଷର *
type_name_func(type_T *type, ଅକ୍ଷର **tofree)
{
    garray_T    ga;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;
    ଅକ୍ଷର	*arg_free = ଖାଲି;

    ga_init2(&ga, 1, 100);
    ଯଦି (ga_grow(&ga, 20) == FAIL)
	ଯାଅ failed;
    STRCPY(ga.ga_data, "func(");
    ga.ga_len += 5;

    ପାଇଁ (i = 0; i < type->tt_argcount; ++i)
    {
	string_T    arg_type;

	ଯଦି (type->tt_args == ଖାଲି)
	    STR_LITERAL_SET(arg_type, "[unknown]");
	ଅନ୍ୟଥା
	{
	    arg_type.string = (char_u *)type_name(type->tt_args[i], &arg_free);
	    arg_type.length = STRLEN(arg_type.string);
	}
	ଯଦି (i > 0)
	{
	    STRCPY((ଅକ୍ଷର *)ga.ga_data + ga.ga_len, ", ");
	    ga.ga_len += 2;
	}
	ଯଦି (ga_grow(&ga, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)arg_type.length + 8) == FAIL)
	    ଯାଅ failed;
	ଯଦି (varargs && i == type->tt_argcount - 1)
	    GA_CONCAT_LITERAL(&ga, "...");
	ଅନ୍ୟଥା ଯଦି (i >= type->tt_min_argcount)
	    *((ଅକ୍ଷର *)ga.ga_data + ga.ga_len++) = '?';
	ga_concat_len(&ga, arg_type.string, arg_type.length);
	VIM_CLEAR(arg_free);
    }
    ଯଦି (type->tt_argcount < 0)
	// any number of arguments
	GA_CONCAT_LITERAL(&ga, "...");

    ଯଦି (type->tt_member == &t_void)
	STRCPY((ଅକ୍ଷର *)ga.ga_data + ga.ga_len, ")");
    ଅନ୍ୟଥା
    {
	ଅକ୍ଷର *ret_free = ଖାଲି;
	ଅକ୍ଷର *ret_name = type_name(type->tt_member, &ret_free);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା  len;

	len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(ret_name) + 4;
	ଯଦି (ga_grow(&ga, len) == FAIL)
	{
	    vim_free(ret_free);
	    ଯାଅ failed;
	}
	STRCPY((ଅକ୍ଷର *)ga.ga_data + ga.ga_len, "): ");
	STRCPY((ଅକ୍ଷର *)ga.ga_data + ga.ga_len + 3, ret_name);
	vim_free(ret_free);
    }
    *tofree = ga.ga_data;
    ଫେରନ୍ତୁ ga.ga_data;

failed:
    vim_free(arg_free);
    ga_clear(&ga);
    ଫେରନ୍ତୁ "[unknown]";
}

/*
 * Return the name of a type.
 * The result may be in allocated memory, in which case "tofree" is set.
 */
    ଅକ୍ଷର *
type_name(type_T *type, ଅକ୍ଷର **tofree)
{
    ଅକ୍ଷର *name;

    *tofree = ଖାଲି;
    ଯଦି (type == ଖାଲି)
	ଫେରନ୍ତୁ "[unknown]";
    name = vartype_name(type->tt_type);

    ବାଛ (type->tt_type)
    {
	ସ୍ଥିତି VAR_LIST:
	ସ୍ଥିତି VAR_DICT:
	    ଫେରନ୍ତୁ type_name_list_or_dict(name, type, tofree);

	ସ୍ଥିତି VAR_TUPLE:
	    ଫେରନ୍ତୁ type_name_tuple(type, tofree);

	ସ୍ଥିତି VAR_CLASS:
	ସ୍ଥିତି VAR_OBJECT:
	    ଫେରନ୍ତୁ type_name_class_or_obj(name, type, tofree);

	ସ୍ଥିତି VAR_FUNC:
	    ଫେରନ୍ତୁ type_name_func(type, tofree);

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଭାଙ୍ଗ;
    }

    ଫେରନ୍ତୁ name;
}

/*
 * "typename(expr)" function
 */
    ଶୂନ୍ୟ
f_typename(typval_T *argvars, typval_T *rettv)
{
    garray_T	type_list;
    type_T	*type;
    ଅକ୍ଷର	*tofree;
    ଅକ୍ଷର	*name;

    rettv->v_type = VAR_STRING;
    ga_init2(&type_list, ଆକାର(type_T *), 10);
    ଯଦି (argvars[0].v_type == VAR_TYPEALIAS)
    {
	type = copy_type(argvars[0].vval.v_typealias->ta_type, &type_list);
	// A type alias for a class has the type set to VAR_OBJECT.  Change it
	// to VAR_CLASS, so that the name is "typealias<class<xxx>>"
	ଯଦି (type->tt_type == VAR_OBJECT)
	    type->tt_type = VAR_CLASS;
    }
    ଅନ୍ୟଥା
	type = typval2type(argvars, get_copyID(), &type_list, TVTT_DO_MEMBER);
    name = type_name(type, &tofree);
    ଯଦି (argvars[0].v_type == VAR_TYPEALIAS)
    {
	vim_snprintf((ଅକ୍ଷର *)IObuff, IOSIZE, "typealias<%s>", name);
	rettv->vval.v_string = vim_strsave((char_u *)IObuff);
	ଯଦି (tofree != ଖାଲି)
	    vim_free(tofree);
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (tofree != ଖାଲି)
	    rettv->vval.v_string = (char_u *)tofree;
	ଅନ୍ୟଥା
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_typval_is_ମୂଲ୍ୟ(typval_T *tv)
{
    ଯଦି (tv == ଖାଲି)
	ଫେରନ୍ତୁ OK;

    ବାଛ (tv->v_type)
    {
	ସ୍ଥିତି VAR_CLASS:
	    {
		class_T *cl = tv->vval.v_class;
		char_u *class_name = (cl == ଖାଲି) ? (char_u *)""
							: cl->class_name.string;
		ଯଦି (cl != ଖାଲି && IS_ENUM(cl))
		    semsg(_(e_using_enum_as_ମୂଲ୍ୟ_str), class_name);
		ଅନ୍ୟଥା
		    semsg(_(e_using_class_as_ମୂଲ୍ୟ_str), class_name);
	    }
	    ଫେରନ୍ତୁ FAIL;

	ସ୍ଥିତି VAR_TYPEALIAS:
	    semsg(_(e_using_typealias_as_ମୂଲ୍ୟ_str),
		    tv->vval.v_typealias->ta_name);
	    ଫେରନ୍ତୁ FAIL;

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଭାଙ୍ଗ;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Same as above, except check type_T.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_type_is_ମୂଲ୍ୟ(type_T *type)
{
    ଯଦି (type == ଖାଲି)
	ଫେରନ୍ତୁ OK;
    ବାଛ (type->tt_type)
    {
	ସ୍ଥିତି VAR_CLASS:
	    ଯଦି (type->tt_class != ଖାଲି && IS_ENUM(type->tt_class))
		semsg(_(e_using_enum_as_ମୂଲ୍ୟ_str),
			type->tt_class->class_name.string);
	    ଅନ୍ୟଥା
		semsg(_(e_using_class_as_ମୂଲ୍ୟ_str),
			type->tt_class == ଖାଲି ? (char_u *)""
			: type->tt_class->class_name.string);
	    ଫେରନ୍ତୁ FAIL;

	ସ୍ଥିତି VAR_TYPEALIAS:
	    // TODO: Not sure what could be done here to get a name.
	    //       Maybe an optional argument?
	    emsg(_(e_using_typealias_as_var_val));
	    ଫେରନ୍ତୁ FAIL;

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଭାଙ୍ଗ;
    }
    ଫେରନ୍ତୁ OK;
}

#endif // FEAT_EVAL
