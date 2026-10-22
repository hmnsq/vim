/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * vim9execute.c: execute Vim9 script instructions
 */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USING_FLOAT_STUFF
#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_EVAL)

// Structure put on ec_trystack when ISN_TRY is encountered.
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਪੂਰਨ	    tcd_ਚੌਖਟਾ_idx;	// ec_frame_idx at ISN_TRY
    ਪੂਰਨ	    tcd_ਢੇਰ_len;	// size of ectx.ec_stack at ISN_TRY
    ਪੂਰਨ	    tcd_ਵਿੱਚ_catch;	// in catch or finally block
    ਪੂਰਨ	    tcd_did_throw;	// set did_throw in :endtry
    ਪੂਰਨ	    tcd_catch_idx;	// instruction of the first :catch or :finally
    ਪੂਰਨ	    tcd_finally_idx;	// instruction of the :finally block or zero
    ਪੂਰਨ	    tcd_endtry_idx;	// instruction of the :endtry
    ਪੂਰਨ	    tcd_caught;		// catch block entered
    ਪੂਰਨ	    tcd_cont;		// :continue encountered, jump here (minus one)
    ਪੂਰਨ	    tcd_return;		// when TRUE return from end of :finally
} trycmd_T;

// Data local to a function.
// On a function call, if not empty, is saved on the stack and restored when
// returning.
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਪੂਰਨ		floc_restore_cmdmod;
    cmdmod_T	floc_save_cmdmod;
    ਪੂਰਨ		floc_restore_cmdmod_stacklen;
} funclocal_T;

// Structure to hold a reference to an outer_T, with information of whether it
// was allocated.
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    outer_T	*or_outer;
    partial_T	*or_partial;	// decrement "or_partial->pt_refcount" later
    ਪੂਰਨ		or_outer_allocated;  // free "or_outer" later
} outer_ref_T;

// A stack is used to store:
// - arguments passed to a :def function
// - info about the calling function, to use when returning
// - local variables
// - temporary values
//
// In detail (FP == Frame Pointer):
//	  arg1		first argument from caller (if present)
//	  arg2		second argument from caller (if present)
//	  extra_arg1	any missing optional argument default value
// FP ->  cur_func	calling function
//	  current	previous instruction pointer
//	  frame_ptr	previous Frame Pointer
//	  var1		space for local variable
//	  var2		space for local variable
//	  ....		fixed space for max. number of local variables
//	  temp		temporary values
//	  ....		flexible space for temporary values (can grow big)

/*
 * Execution context.
 */
ਬਣਤਰ ectx_S {
    garray_T	ec_ਢੇਰ;	// stack of typval_T values
    ਪੂਰਨ		ec_ਚੌਖਟਾ_idx;	// index in ec_stack: context of ec_dfunc_idx
    ਪੂਰਨ		ec_initial_ਚੌਖਟਾ_idx;	// frame index when called

    outer_ref_T	*ec_outer_ref;	// outer scope used for closures, allocated
    funclocal_T ec_funclocal;

    garray_T	ec_trystack;	// stack of trycmd_T values

    isn_T	*ec_instr;	// array with instructions
    ਪੂਰਨ		ec_dfunc_idx;	// current function index
    ਪੂਰਨ		ec_iidx;	// index in ec_instr: instruction to execute

    garray_T	ec_funcrefs;	// partials that might be a closure

    ਪੂਰਨ		ec_did_emsg_before;
    ਪੂਰਨ		ec_trylevel_at_ਸ਼ੁਰੂ;
    where_T	ec_where;
};

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
// stack of profinfo_T used when profiling.
ਸਥਿਰ garray_T profile_info_ga = {0, 0, ਆਕਾਰ(profinfo_T), 20, ਨਿਰਾ};
#ਜੇ_ਅੰਤ

// Get pointer to item in the stack.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਢੇਰ_TV(idx) (((typval_T *)ectx->ec_ਢੇਰ.ga_ਅੰਕੜੇ) + idx)

// Get pointer to item relative to the bottom of the stack, -1 is the last one.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਢੇਰ_TV_BOT(idx) (((typval_T *)ectx->ec_ਢੇਰ.ga_ਅੰਕੜੇ) + ectx->ec_ਢੇਰ.ga_len + (idx))

// Get pointer to a local variable on the stack.  Negative for arguments.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਢੇਰ_TV_VAR(idx) (((typval_T *)ectx->ec_ਢੇਰ.ga_ਅੰਕੜੇ) + ectx->ec_ਚੌਖਟਾ_idx + ਢੇਰ_ਚੌਖਟਾ_ਮਾਪ + idx)

// Return value for functions used to execute instructions
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXEC_FAIL	0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXEC_OK		1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXEC_ਪੂਰਾ	2

    ਖਾਲੀ
ਮੰਜ਼ਿਲ_string_error(vartype_T vartype)
{
    semsg(_(e_cannot_convert_str_ਮੰਜ਼ਿਲ_string), vartype_name(vartype));
}

/*
 * Return the number of arguments, including optional arguments and any vararg.
 */
    ਸਥਿਰ ਪੂਰਨ
ufunc_argcount(ufunc_T *ufunc)
{
    ਵਾਪਸ ufunc->uf_ਦਲੀਲਾਂ.ga_len + (ufunc->uf_va_name != ਨਿਰਾ ? 1 : 0);
}

/*
 * Create a new string from "count" items at the bottom of the stack.
 * A trailing NUL is appended.
 * When "count" is zero an empty string is added to the stack.
 */
    ਸਥਿਰ ਪੂਰਨ
exe_concat(ਪੂਰਨ ਗਿਣਤੀ_2, ectx_T *ectx)
{
    ਪੂਰਨ		idx;
    size_t	len = 0;
    garray_T	ga;
    ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
    {
	typval_T    *tv;
	size_t	    ਲੰਮਾਈ;
    } string_segment_T;
    ਗਿਣਤੀ
    {
	STRING_SEGMENT_CACHE_ਮਾਪ = 10
    };
    string_segment_T	fixed_string_segment_tab[STRING_SEGMENT_CACHE_ਮਾਪ];
    string_segment_T	*string_segment_tab = &fixed_string_segment_tab[0];	// an array of cached typevals and lengths
    string_segment_T    *segment;

    ਜੇ (ਗਿਣਤੀ_2 > (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(fixed_string_segment_tab))
    {
	// make an array big enough to store the length of each string segment
	string_segment_tab = ALLOC_MULT(string_segment_T, ਗਿਣਤੀ_2);
	ਜੇ (string_segment_tab == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
    }

    ga_init2(&ga, ਆਕਾਰ(ਅੱਖਰ), 1);
    // Preallocate enough space for the whole string to avoid having to grow
    // and copy.
    ਲਈ (idx = 0; idx < ਗਿਣਤੀ_2; ++idx)
    {
	segment = &string_segment_tab[idx];
	segment->tv = ਢੇਰ_TV_BOT(idx - ਗਿਣਤੀ_2);
	ਜੇ (segment->tv->vval.v_string != ਨਿਰਾ)
	{
	    segment->ਲੰਮਾਈ = STRLEN(segment->tv->vval.v_string);
	    len += segment->ਲੰਮਾਈ;
	}
	ਨਹੀਂਤਾਂ
	    segment->ਲੰਮਾਈ = 0;    // Ensure clean state for the second pass
    }

    ਜੇ (ga_grow(&ga, (ਪੂਰਨ)len + 1) == FAIL)
    {
	ਜੇ (string_segment_tab != fixed_string_segment_tab)
	    vim_free(string_segment_tab);
	ਵਾਪਸ FAIL;
    }

    ਲਈ (idx = 0; idx < ਗਿਣਤੀ_2; ++idx)
    {
	segment = &string_segment_tab[idx];
	ga_concat_len(&ga, segment->tv->vval.v_string, segment->ਲੰਮਾਈ);
	clear_tv(segment->tv);
    }

    // add a terminating NUL
    ga_append(&ga, NUL);

    ectx->ec_ਢੇਰ.ga_len -= ਗਿਣਤੀ_2 - 1;
    ਢੇਰ_TV_BOT(-1)->vval.v_string = ga.ga_ਅੰਕੜੇ;

    ਜੇ (string_segment_tab != fixed_string_segment_tab)
	vim_free(string_segment_tab);

    ਵਾਪਸ OK;
}

/*
 * Create a new list from "count" items at the bottom of the stack.
 * When "count" is zero an empty list is added to the stack.
 * When "count" is -1 a NULL list is added to the stack.
 */
    ਸਥਿਰ ਪੂਰਨ
exe_newlist(ਪੂਰਨ ਗਿਣਤੀ_2, ectx_T *ectx)
{
    list_T	*list = ਨਿਰਾ;
    ਪੂਰਨ		idx;
    typval_T	*tv;

    ਜੇ (ਗਿਣਤੀ_2 >= 0)
    {
	list = list_alloc_with_ਚੀਜ਼ਾਂ(ਗਿਣਤੀ_2);
	ਜੇ (list == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
	ਲਈ (idx = 0; idx < ਗਿਣਤੀ_2; ++idx)
	    list_ਸੈੱਟ_ਵਸਤੂ(list, idx, ਢੇਰ_TV_BOT(idx - ਗਿਣਤੀ_2));
    }

    ਜੇ (ਗਿਣਤੀ_2 > 0)
	ectx->ec_ਢੇਰ.ga_len -= ਗਿਣਤੀ_2 - 1;
    ਨਹੀਂਤਾਂ ਜੇ (GA_GROW_FAILS(&ectx->ec_ਢੇਰ, 1))
    {
	list_unref(list);
	ਵਾਪਸ FAIL;
    }
    ਨਹੀਂਤਾਂ
	++ectx->ec_ਢੇਰ.ga_len;
    tv = ਢੇਰ_TV_BOT(-1);
    tv->v_type = VAR_LIST;
    tv->vval.v_list = list;
    tv->v_ਤਾਲਾ = 0;
    ਜੇ (list != ਨਿਰਾ)
	++list->lv_refcount;
    ਵਾਪਸ OK;
}

/*
 * Create a new tuple from "count" items at the bottom of the stack.
 * When "count" is zero an empty tuple is added to the stack.
 * When "count" is -1 a NULL tuple is added to the stack.
 */
    ਸਥਿਰ ਪੂਰਨ
exe_newtuple(ਪੂਰਨ ਗਿਣਤੀ_2, ectx_T *ectx)
{
    tuple_T	*tuple = ਨਿਰਾ;
    ਪੂਰਨ		idx;
    typval_T	*tv;

    ਜੇ (ਗਿਣਤੀ_2 >= 0)
    {
	tuple = tuple_alloc_with_ਚੀਜ਼ਾਂ(ਗਿਣਤੀ_2);
	ਜੇ (tuple == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
	ਲਈ (idx = 0; idx < ਗਿਣਤੀ_2; ++idx)
	    tuple_ਸੈੱਟ_ਵਸਤੂ(tuple, idx, ਢੇਰ_TV_BOT(idx - ਗਿਣਤੀ_2));
    }

    ਜੇ (ਗਿਣਤੀ_2 > 0)
	ectx->ec_ਢੇਰ.ga_len -= ਗਿਣਤੀ_2 - 1;
    ਨਹੀਂਤਾਂ ਜੇ (GA_GROW_FAILS(&ectx->ec_ਢੇਰ, 1))
    {
	tuple_unref(tuple);
	ਵਾਪਸ FAIL;
    }
    ਨਹੀਂਤਾਂ
	++ectx->ec_ਢੇਰ.ga_len;
    tv = ਢੇਰ_TV_BOT(-1);
    tv->v_type = VAR_TUPLE;
    tv->vval.v_tuple = tuple;
    tv->v_ਤਾਲਾ = 0;
    ਜੇ (tuple != ਨਿਰਾ)
	++tuple->tv_refcount;

    ਵਾਪਸ OK;
}

/*
 * Implementation of ISN_NEWDICT.
 * Returns FAIL on total failure, MAYBE on error.
 */
    ਸਥਿਰ ਪੂਰਨ
exe_newdict(ਪੂਰਨ ਗਿਣਤੀ_2, ectx_T *ectx)
{
    dict_T	*dict = ਨਿਰਾ;
    dictitem_T	*ਵਸਤੂ;
    char_u	*ਕੁੰਜੀ;
    ਪੂਰਨ		idx;
    typval_T	*tv;

    ਜੇ (ਗਿਣਤੀ_2 >= 0)
    {
	dict = dict_alloc();
	ਜੇ (unlikely(dict == ਨਿਰਾ))
	    ਵਾਪਸ FAIL;
	ਲਈ (idx = 0; idx < ਗਿਣਤੀ_2; ++idx)
	{
	    // have already checked key type is VAR_STRING
	    tv = ਢੇਰ_TV_BOT(2 * (idx - ਗਿਣਤੀ_2));
	    // check key is unique
	    ਕੁੰਜੀ = tv->vval.v_string == ਨਿਰਾ ? (char_u *)"" : tv->vval.v_string;
	    ਵਸਤੂ = dict_ਲੱਭੋ(dict, ਕੁੰਜੀ, -1);
	    ਜੇ (ਵਸਤੂ != ਨਿਰਾ)
	    {
		semsg(_(e_duplicate_ਕੁੰਜੀ_ਵਿੱਚ_dictionary_str), ਕੁੰਜੀ);
		dict_unref(dict);
		ਵਾਪਸ MAYBE;
	    }
	    ਵਸਤੂ = dictitem_alloc(ਕੁੰਜੀ);
	    clear_tv(tv);
	    ਜੇ (unlikely(ਵਸਤੂ == ਨਿਰਾ))
	    {
		dict_unref(dict);
		ਵਾਪਸ FAIL;
	    }
	    tv = ਢੇਰ_TV_BOT(2 * (idx - ਗਿਣਤੀ_2) + 1);
	    ਵਸਤੂ->di_tv = *tv;
	    ਵਸਤੂ->di_tv.v_ਤਾਲਾ = 0;
	    tv->v_type = VAR_UNKNOWN;
	    ਜੇ (dict_ਜੋੜੋ(dict, ਵਸਤੂ) == FAIL)
	    {
		// can this ever happen?
		dictitem_free(ਵਸਤੂ);
		dict_unref(dict);
		ਵਾਪਸ FAIL;
	    }
	}
    }

    ਜੇ (ਗਿਣਤੀ_2 > 0)
	ectx->ec_ਢੇਰ.ga_len -= 2 * ਗਿਣਤੀ_2 - 1;
    ਨਹੀਂਤਾਂ ਜੇ (GA_GROW_FAILS(&ectx->ec_ਢੇਰ, 1))
    {
	dict_unref(dict);
	ਵਾਪਸ FAIL;
    }
    ਨਹੀਂਤਾਂ
	++ectx->ec_ਢੇਰ.ga_len;
    tv = ਢੇਰ_TV_BOT(-1);
    tv->v_type = VAR_DICT;
    tv->v_ਤਾਲਾ = 0;
    tv->vval.v_dict = dict;
    ਜੇ (dict != ਨਿਰਾ)
	++dict->dv_refcount;
    ਵਾਪਸ OK;
}

/*
 * If debug_tick changed check if "ufunc" has a breakpoint and update
 * "uf_has_breakpoint".
 */
    ਖਾਲੀ
update_has_breakpoint(ufunc_T *ufunc)
{
    ਜੇ (ufunc->uf_debug_tick == debug_tick)
	ਵਾਪਸ;

    linenr_T breakpoint;

    ufunc->uf_debug_tick = debug_tick;
    breakpoint = dbg_ਲੱਭੋ_breakpoint(FALSE, ufunc->uf_name, 0);
    ufunc->uf_has_breakpoint = breakpoint > 0;
}

ਸਥਿਰ garray_T dict_ਢੇਰ = GA_ਖਾਲੀ_ਥਾਵਾਂ;

/*
 * Put a value on the dict stack.  This consumes "tv".
 */
    ਸਥਿਰ ਪੂਰਨ
dict_ਢੇਰ_save(typval_T *tv)
{
    ਜੇ (dict_ਢੇਰ.ga_growsize == 0)
	ga_init2(&dict_ਢੇਰ, ਆਕਾਰ(typval_T), 10);
    ਜੇ (ga_grow(&dict_ਢੇਰ, 1) == FAIL)
	ਵਾਪਸ FAIL;
    ((typval_T *)dict_ਢੇਰ.ga_ਅੰਕੜੇ)[dict_ਢੇਰ.ga_len] = *tv;
    ++dict_ਢੇਰ.ga_len;
    ਵਾਪਸ OK;
}

/*
 * Get the typval at top of the dict stack.
 */
    ਸਥਿਰ typval_T *
dict_ਢੇਰ_get_tv(ਖਾਲੀ)
{
    ਜੇ (dict_ਢੇਰ.ga_len == 0)
	ਵਾਪਸ ਨਿਰਾ;
    ਵਾਪਸ ((typval_T *)dict_ਢੇਰ.ga_ਅੰਕੜੇ) + dict_ਢੇਰ.ga_len - 1;
}

/*
 * Get the dict at top of the dict stack.
 */
    ਸਥਿਰ dict_T *
dict_ਢੇਰ_get_dict(ਖਾਲੀ)
{
    typval_T *tv;

    ਜੇ (dict_ਢੇਰ.ga_len == 0)
	ਵਾਪਸ ਨਿਰਾ;
    tv = ((typval_T *)dict_ਢੇਰ.ga_ਅੰਕੜੇ) + dict_ਢੇਰ.ga_len - 1;
    ਜੇ (tv->v_type == VAR_DICT)
	ਵਾਪਸ tv->vval.v_dict;
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Drop an item from the dict stack.
 */
    ਸਥਿਰ ਖਾਲੀ
dict_ਢੇਰ_drop(ਖਾਲੀ)
{
    ਜੇ (dict_ਢੇਰ.ga_len == 0)
    {
	iemsg("Dict stack underflow");
	ਵਾਪਸ;
    }
    --dict_ਢੇਰ.ga_len;
    clear_tv(((typval_T *)dict_ਢੇਰ.ga_ਅੰਕੜੇ) + dict_ਢੇਰ.ga_len);
}

/*
 * Drop items from the dict stack until the length is equal to "len".
 */
    ਸਥਿਰ ਖਾਲੀ
dict_ਢੇਰ_clear(ਪੂਰਨ len)
{
    ਜਦੋਂ (dict_ਢੇਰ.ga_len > len)
	dict_ਢੇਰ_drop();
}

/*
 * Get a pointer to useful "pt_outer" of "pt".
 */
    ਸਥਿਰ outer_T *
get_pt_outer(partial_T *pt)
{
    partial_T *ptref = pt->pt_outer_partial;

    ਜੇ (ptref == ਨਿਰਾ)
	ਵਾਪਸ &pt->pt_outer;

    // partial using partial (recursively)
    ਜਦੋਂ (ptref->pt_outer_partial != ਨਿਰਾ)
	ptref = ptref->pt_outer_partial;
    ਵਾਪਸ &ptref->pt_outer;
}

/*
 * Check "argcount" arguments on the stack against what "ufunc" expects.
 * "off" is the offset of arguments on the stack.
 * Return OK or FAIL.
 */
    ਸਥਿਰ ਪੂਰਨ
check_ufunc_arg_types(ufunc_T *ufunc, ਪੂਰਨ argcount, ਪੂਰਨ off, ectx_T *ectx)
{
    ਜੇ (ufunc->uf_arg_types == ਨਿਰਾ && ufunc->uf_va_type == ਨਿਰਾ)
	ਵਾਪਸ OK;

    typval_T	*argv = ਢੇਰ_TV_BOT(0) - argcount - off;

    // The function can change at runtime, check that the argument
    // types are correct.
    ਲਈ (ਪੂਰਨ i = 0; i < argcount; ++i)
    {
	type_T *type = ਨਿਰਾ;

	// assume a v:none argument, using the default value, is always OK
	ਜੇ (argv[i].v_type == VAR_SPECIAL
		&& argv[i].vval.v_ਸੰਖਿਆ == VVAL_NONE)
	    ਜਾਰੀ;
	// only pass values to user functions, never types
	ਜੇ (check_typval_is_ਮੁੱਲ(&argv[i]) == FAIL)
	    ਵਾਪਸ FAIL;

	ਜੇ (i < ufunc->uf_ਦਲੀਲਾਂ.ga_len && ufunc->uf_arg_types != ਨਿਰਾ)
	    type = ufunc->uf_arg_types[i];
	ਨਹੀਂਤਾਂ ਜੇ (ufunc->uf_va_type != ਨਿਰਾ)
	    type = ufunc->uf_va_type->tt_member;
	ਜੇ (type != ਨਿਰਾ && check_typval_arg_type(type,
		    &argv[i], ਨਿਰਾ, i + 1) == FAIL)
	    ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Call compiled function "cdf_idx" from compiled code.
 * This adds a stack frame and sets the instruction pointer to the start of the
 * called function.
 * If "pt_arg" is not NULL use "pt_arg->pt_outer" for ec_outer_ref->or_outer.
 *
 * Stack has:
 * - current arguments (already there)
 * - omitted optional argument (default values) added here
 * - stack frame:
 *	- pointer to calling function
 *	- Index of next instruction in calling function
 *	- previous frame pointer
 * - reserved space for local variables
 */
    ਸਥਿਰ ਪੂਰਨ
call_dfunc(
	ਪੂਰਨ		cdf_idx,
	partial_T	*pt_arg,
	ਪੂਰਨ		argcount_arg,
	ectx_T		*ectx)
{
    ਪੂਰਨ		argcount = argcount_arg;
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_ਅੰਕੜੇ) + cdf_idx;
    ufunc_T	*ufunc = dfunc->df_ufunc;
    ਪੂਰਨ		did_emsg_before = did_emsg_cumul + did_emsg;
    ਪੂਰਨ		arg_ਮੰਜ਼ਿਲ_ਜੋੜੋ;
    ਪੂਰਨ		vararg_ਗਿਣਤੀ = 0;
    ਪੂਰਨ		varcount;
    ਪੂਰਨ		idx;
    estack_T	*ਇੰਦਰਾਜ;
    funclocal_T	*floc = ਨਿਰਾ;
    ਪੂਰਨ		res = OK;
    compiletype_T compile_type;

    ਜੇ (dfunc->df_deleted)
    {
	// don't use ufunc->uf_name, it may have been freed
	emsg_funcname(e_function_was_deleted_str,
		dfunc->df_name == ਨਿਰਾ ? (char_u *)"unknown" : dfunc->df_name);
	ਵਾਪਸ FAIL;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
    ਜੇ (do_profiling == PROF_YES)
    {
	ਜੇ (GA_GROW_OK(&profile_info_ga, 1))
	{
	    profinfo_T *info = ((profinfo_T *)profile_info_ga.ga_ਅੰਕੜੇ)
						      + profile_info_ga.ga_len;
	    ++profile_info_ga.ga_len;
	    CLEAR_ਸੂਚਕ(info);
	    profile_may_ਸ਼ੁਰੂ_func(info, ufunc,
			(((dfunc_T *)def_functions.ga_ਅੰਕੜੇ)
					      + ectx->ec_dfunc_idx)->df_ufunc);
	}
    }
#ਜੇ_ਅੰਤ

    // When debugging and using "cont" switches to the not-debugged
    // instructions, may need to still compile them.
    compile_type = get_compile_type(ufunc);
    ਜੇ (func_needs_compiling(ufunc, compile_type))
    {
	res = compile_def_function(ufunc, FALSE, compile_type, ਨਿਰਾ);

	// compile_def_function() may cause def_functions.ga_data to change
	dfunc = ((dfunc_T *)def_functions.ga_ਅੰਕੜੇ) + cdf_idx;
    }
    ਜੇ (res == FAIL || INSTRUCTIONS(dfunc) == ਨਿਰਾ)
    {
	ਜੇ (did_emsg_cumul + did_emsg == did_emsg_before)
	    semsg(_(e_function_is_not_compiled_str),
						   printable_func_name(ufunc));
	ਵਾਪਸ FAIL;
    }

    ਜੇ (ufunc->uf_va_name != ਨਿਰਾ)
    {
	// Need to make a list out of the vararg arguments.
	// Stack at time of call with 2 varargs:
	//   normal_arg
	//   optional_arg
	//   vararg_1
	//   vararg_2
	// After creating the list:
	//   normal_arg
	//   optional_arg
	//   vararg-list
	// With missing optional arguments we get:
	//    normal_arg
	// After creating the list
	//    normal_arg
	//    (space for optional_arg)
	//    vararg-list
	vararg_ਗਿਣਤੀ = argcount - ufunc->uf_ਦਲੀਲਾਂ.ga_len;
	ਜੇ (vararg_ਗਿਣਤੀ < 0)
	    vararg_ਗਿਣਤੀ = 0;
	ਨਹੀਂਤਾਂ
	    argcount -= vararg_ਗਿਣਤੀ;
	ਜੇ (exe_newlist(vararg_ਗਿਣਤੀ, ectx) == FAIL)
	    ਵਾਪਸ FAIL;

	vararg_ਗਿਣਤੀ = 1;
    }

    arg_ਮੰਜ਼ਿਲ_ਜੋੜੋ = ufunc->uf_ਦਲੀਲਾਂ.ga_len - argcount;
    ਜੇ (arg_ਮੰਜ਼ਿਲ_ਜੋੜੋ < 0)
    {
	semsg(NGETTEXT(e_one_ਦਲੀਲ_too_many, e_nr_arguments_too_many,
			-arg_ਮੰਜ਼ਿਲ_ਜੋੜੋ), -arg_ਮੰਜ਼ਿਲ_ਜੋੜੋ);
	ਵਾਪਸ FAIL;
    }
    ਨਹੀਂਤਾਂ ਜੇ (arg_ਮੰਜ਼ਿਲ_ਜੋੜੋ > ufunc->uf_def_ਦਲੀਲਾਂ.ga_len)
    {
	ਪੂਰਨ missing = arg_ਮੰਜ਼ਿਲ_ਜੋੜੋ - ufunc->uf_def_ਦਲੀਲਾਂ.ga_len;

	semsg(NGETTEXT(e_one_ਦਲੀਲ_too_few, e_nr_arguments_too_few,
			missing), missing);
	ਵਾਪਸ FAIL;
    }

    // If this is an object method, the object is just before the arguments.
    typval_T	*obj = ਢੇਰ_TV_BOT(0) - argcount - vararg_ਗਿਣਤੀ - 1;

    ਜੇ (IS_OBJECT_ਵਿਧੀ(ufunc) && !IS_CONSTRUCTOR_ਵਿਧੀ(ufunc)
	    && obj->v_type == VAR_OBJECT && obj->vval.v_object == ਨਿਰਾ)
    {
	// If this is not a constructor method, then a valid object is
	// needed.
	emsg(_(e_using_null_object));
	ਵਾਪਸ FAIL;
    }

    // Check the argument types.
    ਜੇ (check_ufunc_arg_types(ufunc, argcount, vararg_ਗਿਣਤੀ, ectx) == FAIL)
	ਵਾਪਸ FAIL;

    // While check_ufunc_arg_types call, def function compilation process may
    // run.  If so many def functions are compiled, def_functions array may be
    // reallocated and dfunc may no longer have valid pointer.  Get the object
    // pointer from def_functions again here.
    dfunc = ((dfunc_T *)def_functions.ga_ਅੰਕੜੇ) + cdf_idx;

    // Reserve space for:
    // - missing arguments
    // - stack frame
    // - local variables
    // - if needed: a counter for number of closures created in
    //   ectx->ec_funcrefs.
    varcount = dfunc->df_varcount + dfunc->df_has_ਘੇਰਾ;
    ਜੇ (GA_GROW_FAILS(&ectx->ec_ਢੇਰ,
				     arg_ਮੰਜ਼ਿਲ_ਜੋੜੋ + ਢੇਰ_ਚੌਖਟਾ_ਮਾਪ + varcount))
	ਵਾਪਸ FAIL;

    // The object pointer is in the execution typval stack.  The GA_GROW call
    // above may have reallocated the execution typval stack.  So the object
    // pointer may not be valid anymore.  Get the object pointer again from the
    // execution stack.
    obj = ਢੇਰ_TV_BOT(0) - argcount - vararg_ਗਿਣਤੀ - 1;

    // If depth of calling is getting too high, don't execute the function.
    ਜੇ (funcdepth_increment() == FAIL)
	ਵਾਪਸ FAIL;
    ++ex_nesting_level;

    // Only make a copy of funclocal if it contains something to restore.
    ਜੇ (ectx->ec_funclocal.floc_restore_cmdmod)
    {
	floc = ALLOC_ONE(funclocal_T);
	ਜੇ (floc == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
	*floc = ectx->ec_funclocal;
	ectx->ec_funclocal.floc_restore_cmdmod = FALSE;
    }

    // Move the vararg-list to below the missing optional arguments.
    ਜੇ (vararg_ਗਿਣਤੀ > 0 && arg_ਮੰਜ਼ਿਲ_ਜੋੜੋ > 0)
	*ਢੇਰ_TV_BOT(arg_ਮੰਜ਼ਿਲ_ਜੋੜੋ - 1) = *ਢੇਰ_TV_BOT(-1);

    // Reserve space for omitted optional arguments, filled in soon.
    ਲਈ (idx = 0; idx < arg_ਮੰਜ਼ਿਲ_ਜੋੜੋ; ++idx)
	ਢੇਰ_TV_BOT(idx - vararg_ਗਿਣਤੀ)->v_type = VAR_UNKNOWN;
    ectx->ec_ਢੇਰ.ga_len += arg_ਮੰਜ਼ਿਲ_ਜੋੜੋ;

    // Store current execution state in stack frame for ISN_RETURN.
    ਢੇਰ_TV_BOT(ਢੇਰ_ਚੌਖਟਾ_FUNC_OFF)->vval.v_ਸੰਖਿਆ = ectx->ec_dfunc_idx;
    ਢੇਰ_TV_BOT(ਢੇਰ_ਚੌਖਟਾ_IIDX_OFF)->vval.v_ਸੰਖਿਆ = ectx->ec_iidx;
    ਢੇਰ_TV_BOT(ਢੇਰ_ਚੌਖਟਾ_INSTR_OFF)->vval.v_string = (ਖਾਲੀ *)ectx->ec_instr;
    ਢੇਰ_TV_BOT(ਢੇਰ_ਚੌਖਟਾ_OUTER_OFF)->vval.v_string =
						    (ਖਾਲੀ *)ectx->ec_outer_ref;
    ਢੇਰ_TV_BOT(ਢੇਰ_ਚੌਖਟਾ_FUNCLOCAL_OFF)->vval.v_string = (ਖਾਲੀ *)floc;
    ਢੇਰ_TV_BOT(ਢੇਰ_ਚੌਖਟਾ_IDX_OFF)->vval.v_ਸੰਖਿਆ = ectx->ec_ਚੌਖਟਾ_idx;
    ectx->ec_ਚੌਖਟਾ_idx = ectx->ec_ਢੇਰ.ga_len;

    // Initialize all local variables to number zero.  Also initialize the
    // variable that counts how many closures were created.  This is used in
    // handle_closure_in_use().
    ਪੂਰਨ initcount = dfunc->df_varcount + (dfunc->df_has_ਘੇਰਾ ? 1 : 0);
    ਲਈ (idx = 0; idx < initcount; ++idx)
    {
	typval_T *tv = ਢੇਰ_TV_BOT(ਢੇਰ_ਚੌਖਟਾ_ਮਾਪ + idx);

	tv->v_type = VAR_ਸੰਖਿਆ;
	tv->vval.v_ਸੰਖਿਆ = 0;
    }
    ectx->ec_ਢੇਰ.ga_len += ਢੇਰ_ਚੌਖਟਾ_ਮਾਪ + varcount;

    // For an object method move the object from just before the arguments to
    // the first local variable.
    ਜੇ (IS_OBJECT_ਵਿਧੀ(ufunc))
    {
	ਜੇ (obj->v_type != VAR_OBJECT)
	{
	    semsg(_(e_ਅੰਦਰੂਨੀ_error_str), "type in stack is not an object");
	    ਵਾਪਸ FAIL;
	}

	*ਢੇਰ_TV_VAR(0) = *obj;
	obj->v_type = VAR_UNKNOWN;
    }

    partial_T *pt = pt_arg != ਨਿਰਾ ? pt_arg : ufunc->uf_partial;
    ਜੇ (pt != ਨਿਰਾ || (ufunc->uf_flags & FC_ਘੇਰਾ))
    {
	outer_ref_T *ref = ALLOC_CLEAR_ONE(outer_ref_T);

	ਜੇ (ref == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
	ਜੇ (pt != ਨਿਰਾ)
	{
	    ref->or_outer = get_pt_outer(pt);
	    ++pt->pt_refcount;
	    ref->or_partial = pt;
	}
	ਨਹੀਂਤਾਂ
	{
	    ref->or_outer = ALLOC_CLEAR_ONE(outer_T);
	    ਜੇ (unlikely(ref->or_outer == ਨਿਰਾ))
	    {
		vim_free(ref);
		ਵਾਪਸ FAIL;
	    }
	    ref->or_outer_allocated = TRUE;
	    ref->or_outer->ਬਾਹਰਲਾ_ਢੇਰ = &ectx->ec_ਢੇਰ;
	    ref->or_outer->ਬਾਹਰਲਾ_ਚੌਖਟਾ_idx = ectx->ec_ਚੌਖਟਾ_idx;
	    ਜੇ (ectx->ec_outer_ref != ਨਿਰਾ)
		ref->or_outer->ਬਾਹਰਲਾ_ਉੱਪਰ = ectx->ec_outer_ref->or_outer;
	}
	ectx->ec_outer_ref = ref;
    }
    ਨਹੀਂਤਾਂ
	ectx->ec_outer_ref = ਨਿਰਾ;

    ਜੇ (ufunc->uf_flags & FC_SANDBOX)
	++sandbox;

    ++ufunc->uf_calls;

    // Set execution state to the start of the called function.
    ectx->ec_dfunc_idx = cdf_idx;
    ectx->ec_instr = INSTRUCTIONS(dfunc);
    ਇੰਦਰਾਜ = estack_ਧੱਕੋ_ufunc(ufunc, 1);
    ਜੇ (ਇੰਦਰਾਜ != ਨਿਰਾ)
    {
	// Set the script context to the script where the function was defined.
	// Save the current context so it can be restored on return.
	ਇੰਦਰਾਜ->es_save_sctx = current_sctx;
	current_sctx = ufunc->uf_script_ctx;
    }

    // Start execution at the first instruction.
    ectx->ec_iidx = 0;

    ਵਾਪਸ OK;
}

// Double linked list of funcstack_T in use.
ਸਥਿਰ funcstack_T *ਪਹਿਲਾ_funcstack = ਨਿਰਾ;

    ਸਥਿਰ ਖਾਲੀ
ਜੋੜੋ_funcstack_ਮੰਜ਼ਿਲ_list(funcstack_T *funcstack)
{
	// Link in list of funcstacks.
    ਜੇ (ਪਹਿਲਾ_funcstack != ਨਿਰਾ)
	ਪਹਿਲਾ_funcstack->fs_prev = funcstack;
    funcstack->fs_ਅਗਲਾ = ਪਹਿਲਾ_funcstack;
    funcstack->fs_prev = ਨਿਰਾ;
    ਪਹਿਲਾ_funcstack = funcstack;
}

    ਸਥਿਰ ਖਾਲੀ
remove_funcstack_from_list(funcstack_T *funcstack)
{
    ਜੇ (funcstack->fs_prev == ਨਿਰਾ)
	ਪਹਿਲਾ_funcstack = funcstack->fs_ਅਗਲਾ;
    ਨਹੀਂਤਾਂ
	funcstack->fs_prev->fs_ਅਗਲਾ = funcstack->fs_ਅਗਲਾ;
    ਜੇ (funcstack->fs_ਅਗਲਾ != ਨਿਰਾ)
	funcstack->fs_ਅਗਲਾ->fs_prev = funcstack->fs_prev;
}

/*
 * Used when returning from a function: Check if any closure is still
 * referenced.  If so then move the arguments and variables to a separate piece
 * of stack to be used when the closure is called.
 * When "free_arguments" is TRUE the arguments are to be freed.
 * Returns FAIL when out of memory.
 */
    ਸਥਿਰ ਪੂਰਨ
handle_ਘੇਰਾ_ਵਿੱਚ_use(ectx_T *ectx, ਪੂਰਨ free_arguments)
{
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_ਅੰਕੜੇ)
							  + ectx->ec_dfunc_idx;
    ਪੂਰਨ		argcount;
    ਪੂਰਨ		ਸਿਖਰ;
    ਪੂਰਨ		idx;
    typval_T	*tv;
    ਪੂਰਨ		ਘੇਰਾ_ਵਿੱਚ_use = FALSE;
    garray_T	*gap = &ectx->ec_funcrefs;
    varnumber_T	ਘੇਰਾ_ਗਿਣਤੀ;

    ਜੇ (dfunc->df_ufunc == ਨਿਰਾ)
	ਵਾਪਸ OK;  // function was freed
    ਜੇ (dfunc->df_has_ਘੇਰਾ == 0)
	ਵਾਪਸ OK;  // no closures
    tv = ਢੇਰ_TV(ectx->ec_ਚੌਖਟਾ_idx + ਢੇਰ_ਚੌਖਟਾ_ਮਾਪ + dfunc->df_varcount);
    ਘੇਰਾ_ਗਿਣਤੀ = tv->vval.v_ਸੰਖਿਆ;
    ਜੇ (ਘੇਰਾ_ਗਿਣਤੀ == 0)
	ਵਾਪਸ OK;  // no funcrefs created

    // Compute "top": the first entry in the stack used by the function.
    // This is the first argument (after that comes the stack frame and then
    // the local variables).
    argcount = ufunc_argcount(dfunc->df_ufunc);
    ਸਿਖਰ = ectx->ec_ਚੌਖਟਾ_idx - argcount;

    // Check if any created closure is still in use.
    ਲਈ (idx = 0; idx < ਘੇਰਾ_ਗਿਣਤੀ; ++idx)
    {
	partial_T   *pt;
	ਪੂਰਨ	    off = gap->ga_len - ਘੇਰਾ_ਗਿਣਤੀ + idx;

	ਜੇ (off < 0)
	    ਜਾਰੀ;  // count is off or already done
	pt = ((partial_T **)gap->ga_ਅੰਕੜੇ)[off];
	ਜੇ (pt->pt_refcount > 1)
	{
	    ਪੂਰਨ refcount = pt->pt_refcount;
	    ਪੂਰਨ i;

	    // A Reference in a local variable doesn't count, it gets
	    // unreferenced on return.
	    ਲਈ (i = 0; i < dfunc->df_varcount; ++i)
	    {
		typval_T *stv = ਢੇਰ_TV(ectx->ec_ਚੌਖਟਾ_idx
						       + ਢੇਰ_ਚੌਖਟਾ_ਮਾਪ + i);
		ਜੇ (stv->v_type == VAR_PARTIAL && pt == stv->vval.v_partial)
		    --refcount;
	    }
	    ਜੇ (refcount > 1)
	    {
		ਘੇਰਾ_ਵਿੱਚ_use = TRUE;
		ਰੋਕੋ;
	    }
	}
    }

    ਜੇ (ਘੇਰਾ_ਵਿੱਚ_use)
    {
	funcstack_T *funcstack = ALLOC_CLEAR_ONE(funcstack_T);
	typval_T    *ਢੇਰ;

	// A closure is using the arguments and/or local variables.
	// Move them to the called function.
	ਜੇ (funcstack == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;

	funcstack->fs_var_ਵਿੱਥ = argcount + ਢੇਰ_ਚੌਖਟਾ_ਮਾਪ;
	funcstack->fs_ga.ga_len = funcstack->fs_var_ਵਿੱਥ
							  + dfunc->df_varcount;
	ਢੇਰ = ALLOC_CLEAR_MULT(typval_T, funcstack->fs_ga.ga_len);
	funcstack->fs_ga.ga_ਅੰਕੜੇ = ਢੇਰ;
	ਜੇ (ਢੇਰ == ਨਿਰਾ)
	{
	    vim_free(funcstack);
	    ਵਾਪਸ FAIL;
	}
	ਜੋੜੋ_funcstack_ਮੰਜ਼ਿਲ_list(funcstack);

	// Move or copy the arguments.
	ਲਈ (idx = 0; idx < argcount; ++idx)
	{
	    tv = ਢੇਰ_TV(ਸਿਖਰ + idx);
	    ਜੇ (free_arguments)
	    {
		*(ਢੇਰ + idx) = *tv;
		tv->v_type = VAR_UNKNOWN;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (tv->v_type != VAR_UNKNOWN)
		// Skip an argument that was not set, the stack was cleared.
		copy_tv(tv, ਢੇਰ + idx);
	}
	// Skip the stack frame.
	// Move the local variables.
	ਲਈ (idx = 0; idx < dfunc->df_varcount; ++idx)
	{
	    tv = ਢੇਰ_TV(ectx->ec_ਚੌਖਟਾ_idx + ਢੇਰ_ਚੌਖਟਾ_ਮਾਪ + idx);

	    // A partial created for a local function, that is also used as a
	    // local variable, has a reference count for the variable, thus
	    // will never go down to zero.  When all these refcounts are one
	    // then the funcstack is unused.  We need to count how many we have
	    // so we know when to check.
	    ਜੇ (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != ਨਿਰਾ)
	    {
		ਪੂਰਨ	    i;

		ਲਈ (i = 0; i < ਘੇਰਾ_ਗਿਣਤੀ; ++i)
		    ਜੇ (tv->vval.v_partial == ((partial_T **)gap->ga_ਅੰਕੜੇ)[
					      gap->ga_len - ਘੇਰਾ_ਗਿਣਤੀ + i])
			++funcstack->fs_min_refcount;
	    }

	    *(ਢੇਰ + funcstack->fs_var_ਵਿੱਥ + idx) = *tv;
	    tv->v_type = VAR_UNKNOWN;
	}

	ਲਈ (idx = 0; idx < ਘੇਰਾ_ਗਿਣਤੀ; ++idx)
	{
	    partial_T *pt = ((partial_T **)gap->ga_ਅੰਕੜੇ)[gap->ga_len
							- ਘੇਰਾ_ਗਿਣਤੀ + idx];
	    ਜੇ (pt->pt_refcount > 1)
	    {
		++funcstack->fs_refcount;
		pt->pt_funcstack = funcstack;
		pt->pt_outer.ਬਾਹਰਲਾ_ਢੇਰ = &funcstack->fs_ga;
		pt->pt_outer.ਬਾਹਰਲਾ_ਚੌਖਟਾ_idx = ectx->ec_ਚੌਖਟਾ_idx - ਸਿਖਰ;
	    }
	}
    }

    ਲਈ (idx = 0; idx < ਘੇਰਾ_ਗਿਣਤੀ; ++idx)
	partial_unref(((partial_T **)gap->ga_ਅੰਕੜੇ)[gap->ga_len
						       - ਘੇਰਾ_ਗਿਣਤੀ + idx]);
    gap->ga_len -= ਘੇਰਾ_ਗਿਣਤੀ;
    ਜੇ (gap->ga_len == 0)
	ga_clear(gap);

    ਵਾਪਸ OK;
}

/*
 * Called when a partial is freed or its reference count goes down to one.  The
 * funcstack may be the only reference to the partials in the local variables.
 * Go over all of them, the funcref and can be freed if all partials
 * referencing the funcstack have a reference count of one.
 * Returns TRUE if the funcstack is freed, the partial referencing it will then
 * also have been freed.
 */
    ਪੂਰਨ
funcstack_check_refcount(funcstack_T *funcstack)
{
    ਪੂਰਨ		i;
    garray_T	*gap = &funcstack->fs_ga;
    ਪੂਰਨ		ਪੂਰਾ = 0;
    typval_T	*ਢੇਰ;

    ਜੇ (funcstack->fs_refcount > funcstack->fs_min_refcount)
	ਵਾਪਸ FALSE;
    ਲਈ (i = funcstack->fs_var_ਵਿੱਥ; i < gap->ga_len; ++i)
    {
	typval_T *tv = ((typval_T *)gap->ga_ਅੰਕੜੇ) + i;

	ਜੇ (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != ਨਿਰਾ
		&& tv->vval.v_partial->pt_funcstack == funcstack
		&& tv->vval.v_partial->pt_refcount == 1)
	    ++ਪੂਰਾ;
    }
    ਜੇ (ਪੂਰਾ != funcstack->fs_min_refcount)
	ਵਾਪਸ FALSE;

    ਢੇਰ = gap->ga_ਅੰਕੜੇ;

    // All partials referencing the funcstack have a reference count of
    // one, thus the funcstack is no longer of use.
    ਲਈ (i = 0; i < gap->ga_len; ++i)
	clear_tv(ਢੇਰ + i);
    vim_free(ਢੇਰ);
    remove_funcstack_from_list(funcstack);
    vim_free(funcstack);

    ਵਾਪਸ TRUE;
}

/*
 * For garbage collecting: set references in all variables referenced by
 * all funcstacks.
 */
    ਪੂਰਨ
ਸੈੱਟ_ref_ਵਿੱਚ_funcstacks(ਪੂਰਨ copyID)
{
    funcstack_T *funcstack;

    ਲਈ (funcstack = ਪਹਿਲਾ_funcstack; funcstack != ਨਿਰਾ;
						funcstack = funcstack->fs_ਅਗਲਾ)
    {
	typval_T    *ਢੇਰ = funcstack->fs_ga.ga_ਅੰਕੜੇ;
	ਪੂਰਨ	    i;

	ਲਈ (i = 0; i < funcstack->fs_ga.ga_len; ++i)
	    ਜੇ (ਸੈੱਟ_ref_ਵਿੱਚ_ਵਸਤੂ(ਢੇਰ + i, copyID, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ))
		ਵਾਪਸ TRUE;  // abort
    }
    ਵਾਪਸ FALSE;
}

// Ugly static to avoid passing the execution context around through many
// layers.
ਸਥਿਰ ectx_T *current_ectx = ਨਿਰਾ;

/*
 * Return TRUE if currently executing a :def function.
 * Can be used by builtin functions only.
 */
    ਪੂਰਨ
ਵਿੱਚ_def_function(ਖਾਲੀ)
{
    ਵਾਪਸ current_ectx != ਨਿਰਾ;
}

/*
 * If executing a class/object method, then fill in the lval_T.
 * Set lr_tv to the executing item, and lr_exec_class to the executing class;
 * use free_tv and class_unref when finished with the lval_root.
 * For use by builtin functions.
 *
 * Return FAIL and do nothing if not executing in a class; otherwise OK.
 */
    ਪੂਰਨ
fill_exec_lval_ਜੜ੍ਹ(lval_ਜੜ੍ਹ_T *ਜੜ੍ਹ)
{
    ectx_T *ectx = current_ectx;
    ਜੇ (ectx != ਨਿਰਾ)
    {
	dfunc_T	    *dfunc = ((dfunc_T *)def_functions.ga_ਅੰਕੜੇ)
						  + current_ectx->ec_dfunc_idx;
	ufunc_T    *ufunc = dfunc->df_ufunc;
	ਜੇ (ufunc->uf_class != ਨਿਰਾ)	// executing a method?
	{
	    typval_T *tv = alloc_tv();
	    ਜੇ (tv != ਨਿਰਾ)
	    {
		CLEAR_ਸੂਚਕ(ਜੜ੍ਹ);
		ਜੜ੍ਹ->lr_tv = tv;
		copy_tv(ਢੇਰ_TV_VAR(0), ਜੜ੍ਹ->lr_tv);
		ਜੜ੍ਹ->lr_cl_exec = ufunc->uf_class;
		++ਜੜ੍ਹ->lr_cl_exec->class_refcount;
		ਵਾਪਸ OK;
	    }
	}
    }

    ਵਾਪਸ FAIL;
}

/*
 * Clear "current_ectx" and return the previous value.  To be used when calling
 * a user function.
 */
    ectx_T *
clear_current_ectx(ਖਾਲੀ)
{
    ectx_T *r = current_ectx;

    current_ectx = ਨਿਰਾ;
    ਵਾਪਸ r;
}

    ਖਾਲੀ
restore_current_ectx(ectx_T *ectx)
{
    ਜੇ (current_ectx != ਨਿਰਾ)
	iemsg("Restoring current_ectx while it is not NULL");
    current_ectx = ectx;
}

/*
 * Add an entry for a deferred function call to the currently executing
 * function.
 * Return the list or NULL when failed.
 */
    ਸਥਿਰ list_T *
ਜੋੜੋ_defer_ਵਸਤੂ(ਪੂਰਨ var_idx, ਪੂਰਨ argcount, ectx_T *ectx)
{
    typval_T	*defer_tv = ਢੇਰ_TV_VAR(var_idx);
    list_T	*defer_l;
    list_T	*l;
    typval_T	listval;

    ਜੇ (defer_tv->v_type != VAR_LIST)
    {
	// first time, allocate the list
	ਜੇ (rettv_list_alloc(defer_tv) == FAIL)
	    ਵਾਪਸ ਨਿਰਾ;
    }
    defer_l = defer_tv->vval.v_list;

    l = list_alloc_with_ਚੀਜ਼ਾਂ(argcount + 1);
    ਜੇ (l == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    listval.v_type = VAR_LIST;
    listval.vval.v_list = l;
    listval.v_ਤਾਲਾ = 0;
    ਜੇ (list_ਪਾਓ_tv(defer_l, &listval, defer_l->lv_ਪਹਿਲਾ) == FAIL)
    {
	list_free(l);
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਵਾਪਸ l;
}

/*
 * Handle ISN_DEFER.  Stack has a function reference and "argcount" arguments.
 * The local variable that lists deferred functions is "var_idx".
 * Returns OK or FAIL.
 */
    ਸਥਿਰ ਪੂਰਨ
defer_command(ਪੂਰਨ var_idx, ਪੂਰਨ argcount, ectx_T *ectx)
{
    list_T	*l = ਜੋੜੋ_defer_ਵਸਤੂ(var_idx, argcount, ectx);
    ਪੂਰਨ		i;
    typval_T	*func_tv;

    ਜੇ (l == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    func_tv = ਢੇਰ_TV_BOT(-argcount - 1);
    ਜੇ (func_tv->v_type != VAR_PARTIAL && func_tv->v_type != VAR_FUNC)
    {
	semsg(_(e_expected_str_but_got_str),
		"function or partial",
		vartype_name(func_tv->v_type));
	ਵਾਪਸ FAIL;
    }
    list_ਸੈੱਟ_ਵਸਤੂ(l, 0, func_tv);

    ਲਈ (i = 0; i < argcount; ++i)
	list_ਸੈੱਟ_ਵਸਤੂ(l, i + 1, ਢੇਰ_TV_BOT(-argcount + i));
    ectx->ec_ਢੇਰ.ga_len -= argcount + 1;
    ਵਾਪਸ OK;
}

/*
 * Add a deferred call for "name" with arguments "argvars[argcount]".
 * Consumes "name", also on failure.
 * Only to be called when in_def_function() returns TRUE.
 */
    ਪੂਰਨ
ਜੋੜੋ_defer_function(char_u *name, ਪੂਰਨ argcount, typval_T *argvars)
{
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_ਅੰਕੜੇ)
						  + current_ectx->ec_dfunc_idx;
    list_T	*l;
    typval_T	func_tv;
    ਪੂਰਨ		i;

    ਜੇ (dfunc->df_defer_var_idx == 0)
    {
	iemsg("df_defer_var_idx is zero");
	vim_free(name);
	ਵਾਪਸ FAIL;
    }

    l = ਜੋੜੋ_defer_ਵਸਤੂ(dfunc->df_defer_var_idx - 1, argcount, current_ectx);
    ਜੇ (l == ਨਿਰਾ)
    {
	vim_free(name);
	ਵਾਪਸ FAIL;
    }

    func_tv.v_type = VAR_FUNC;
    func_tv.v_ਤਾਲਾ = 0;
    func_tv.vval.v_string = name;
    list_ਸੈੱਟ_ਵਸਤੂ(l, 0, &func_tv);

    ਲਈ (i = 0; i < argcount; ++i)
	list_ਸੈੱਟ_ਵਸਤੂ(l, i + 1, argvars + i);
    ਵਾਪਸ OK;
}

/*
 * Invoked when returning from a function: Invoke any deferred calls.
 */
    ਸਥਿਰ ਖਾਲੀ
invoke_defer_funcs(ectx_T *ectx)
{
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_ਅੰਕੜੇ)
							  + ectx->ec_dfunc_idx;
    typval_T	*defer_tv = ਢੇਰ_TV_VAR(dfunc->df_defer_var_idx - 1);
    listitem_T	*li;

    ਜੇ (defer_tv->v_type != VAR_LIST)
	ਵਾਪਸ;	 // no function added
    FOR_ALL_LIST_ਚੀਜ਼ਾਂ(defer_tv->vval.v_list, li)
    {
	list_T	    *l = li->li_tv.vval.v_list;
	typval_T    rettv;
	typval_T    argvars[MAX_FUNC_ਦਲੀਲਾਂ];
	ਪੂਰਨ	    i;
	listitem_T  *arg_li = l->lv_ਪਹਿਲਾ;
	typval_T    *functv = &l->lv_ਪਹਿਲਾ->li_tv;
	ਪੂਰਨ	    argcount = l->lv_len - 1;

	ਜੇ (functv->vval.v_string == ਨਿਰਾ)
	    // already being called, can happen if function does ":qa"
	    ਜਾਰੀ;

	ਲਈ (i = 0; i < argcount; ++i)
	{
	    arg_li = arg_li->li_ਅਗਲਾ;
	    argvars[i] = arg_li->li_tv;
	}

	funcexe_T	funcexe;
	char_u		*name = ਨਿਰਾ;
	partial_T	*pt = ਨਿਰਾ;

	CLEAR_ਖੇਤਰ(funcexe);
	funcexe.fe_evaluate = TRUE;
	rettv.v_type = VAR_UNKNOWN;

	ਜੇ (functv->v_type == VAR_PARTIAL)
	{
	    pt = functv->vval.v_partial;
	    functv->vval.v_partial = ਨਿਰਾ;

	    name = pt->pt_func->uf_name;
	    funcexe.fe_partial = pt;
	    funcexe.fe_object = pt->pt_obj;
	    ਜੇ (funcexe.fe_object != ਨਿਰਾ)
		++funcexe.fe_object->obj_refcount;
	}
	ਨਹੀਂਤਾਂ
	{
	    name = functv->vval.v_string;
	    functv->vval.v_string = ਨਿਰਾ;
	}

	// If the deferred function is called after an exception, then only the
	// first statement in the function will be executed (because of the
	// exception).  So save and restore the try/catch/throw exception
	// state.
	exception_state_T estate;
	exception_state_save(&estate);
	exception_state_clear();

	(ਖਾਲੀ)call_func(name, -1, &rettv, argcount, argvars, &funcexe);

	exception_state_restore(&estate);

	clear_tv(&rettv);
	ਜੇ (functv->v_type == VAR_PARTIAL)
	    partial_unref(pt);
	ਨਹੀਂਤਾਂ
	    vim_free(name);
    }
}

/*
 * Return from the current function.
 */
    ਸਥਿਰ ਪੂਰਨ
func_return(ectx_T *ectx)
{
    ਪੂਰਨ		idx;
    ਪੂਰਨ		ret_idx;
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_ਅੰਕੜੇ)
							  + ectx->ec_dfunc_idx;
    ਪੂਰਨ		argcount = ufunc_argcount(dfunc->df_ufunc);
    estack_T	*ਇੰਦਰਾਜ;
    ਪੂਰਨ		prev_dfunc_idx = ਢੇਰ_TV(ectx->ec_ਚੌਖਟਾ_idx
					+ ਢੇਰ_ਚੌਖਟਾ_FUNC_OFF)->vval.v_ਸੰਖਿਆ;
    funclocal_T	*floc;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
    dfunc_T	*prev_dfunc = ((dfunc_T *)def_functions.ga_ਅੰਕੜੇ)
							      + prev_dfunc_idx;

    ਜੇ (do_profiling == PROF_YES)
    {
	ufunc_T *caller = prev_dfunc->df_ufunc;

	ਜੇ (dfunc->df_ufunc->uf_profiling
				   || (caller != ਨਿਰਾ && caller->uf_profiling))
	{
	    profile_may_ਸਿਰਾ_func(((profinfo_T *)profile_info_ga.ga_ਅੰਕੜੇ)
			+ profile_info_ga.ga_len - 1, dfunc->df_ufunc, caller);
	    --profile_info_ga.ga_len;
	}
    }
#ਜੇ_ਅੰਤ

    ਜੇ (dfunc->df_defer_var_idx > 0)
	invoke_defer_funcs(ectx);

    ਜੇ (dfunc->df_ufunc->uf_flags & FC_SANDBOX)
	--sandbox;

    // No check for uf_refcount being zero, cannot think of a way that would
    // happen.
    --dfunc->df_ufunc->uf_calls;

    // execution context goes one level up
    ਇੰਦਰਾਜ = estack_ਕੱਢੋ();
    ਜੇ (ਇੰਦਰਾਜ != ਨਿਰਾ)
	current_sctx = ਇੰਦਰਾਜ->es_save_sctx;

    ਜੇ (handle_ਘੇਰਾ_ਵਿੱਚ_use(ectx, TRUE) == FAIL)
	ਵਾਪਸ FAIL;

    // Clear the arguments.  If this was an object method also clear the
    // object, it is just before the arguments.
    ਪੂਰਨ ਸਿਖਰ = ectx->ec_ਚੌਖਟਾ_idx - argcount;
    ਜੇ (IS_OBJECT_ਵਿਧੀ(dfunc->df_ufunc))
	--ਸਿਖਰ;
    ਲਈ (idx = ਸਿਖਰ; idx < ectx->ec_ਚੌਖਟਾ_idx; ++idx)
	clear_tv(ਢੇਰ_TV(idx));

    // Clear local variables and temp values, but not the return value.
    ਲਈ (idx = ectx->ec_ਚੌਖਟਾ_idx + ਢੇਰ_ਚੌਖਟਾ_ਮਾਪ;
					idx < ectx->ec_ਢੇਰ.ga_len - 1; ++idx)
	clear_tv(ਢੇਰ_TV(idx));

    // The return value should be on top of the stack.  However, when aborting
    // it may not be there and ec_frame_idx is the top of the stack.
    ret_idx = ectx->ec_ਢੇਰ.ga_len - 1;
    ਜੇ (ret_idx == ectx->ec_ਚੌਖਟਾ_idx + ਢੇਰ_ਚੌਖਟਾ_IDX_OFF)
	ret_idx = 0;

    ਜੇ (ectx->ec_outer_ref != ਨਿਰਾ)
    {
	ਜੇ (ectx->ec_outer_ref->or_outer_allocated)
	    vim_free(ectx->ec_outer_ref->or_outer);
	partial_unref(ectx->ec_outer_ref->or_partial);
	vim_free(ectx->ec_outer_ref);
    }

    // Restore the previous frame.
    ectx->ec_dfunc_idx = prev_dfunc_idx;
    ectx->ec_iidx = ਢੇਰ_TV(ectx->ec_ਚੌਖਟਾ_idx
					+ ਢੇਰ_ਚੌਖਟਾ_IIDX_OFF)->vval.v_ਸੰਖਿਆ;
    ectx->ec_instr = (ਖਾਲੀ *)ਢੇਰ_TV(ectx->ec_ਚੌਖਟਾ_idx
				       + ਢੇਰ_ਚੌਖਟਾ_INSTR_OFF)->vval.v_string;
    ectx->ec_outer_ref = (ਖਾਲੀ *)ਢੇਰ_TV(ectx->ec_ਚੌਖਟਾ_idx
				       + ਢੇਰ_ਚੌਖਟਾ_OUTER_OFF)->vval.v_string;
    floc = (ਖਾਲੀ *)ਢੇਰ_TV(ectx->ec_ਚੌਖਟਾ_idx
				   + ਢੇਰ_ਚੌਖਟਾ_FUNCLOCAL_OFF)->vval.v_string;
    // restoring ec_frame_idx must be last
    ectx->ec_ਚੌਖਟਾ_idx = ਢੇਰ_TV(ectx->ec_ਚੌਖਟਾ_idx
				       + ਢੇਰ_ਚੌਖਟਾ_IDX_OFF)->vval.v_ਸੰਖਿਆ;

    ਜੇ (floc == ਨਿਰਾ)
	ectx->ec_funclocal.floc_restore_cmdmod = FALSE;
    ਨਹੀਂਤਾਂ
    {
	ectx->ec_funclocal = *floc;
	vim_free(floc);
    }

    ਜੇ (ret_idx > 0)
    {
	// Reset the stack to the position before the call, with a spot for the
	// return value, moved there from above the frame.
	ectx->ec_ਢੇਰ.ga_len = ਸਿਖਰ + 1;
	*ਢੇਰ_TV_BOT(-1) = *ਢੇਰ_TV(ret_idx);
    }
    ਨਹੀਂਤਾਂ
	// Reset the stack to the position before the call.
	ectx->ec_ਢੇਰ.ga_len = ਸਿਖਰ;

    funcdepth_decrement();
    --ex_nesting_level;
    ਵਾਪਸ OK;
}

/*
 * Prepare arguments and rettv for calling a builtin or user function.
 */
    ਸਥਿਰ ਪੂਰਨ
call_prepare(ਪੂਰਨ argcount, typval_T *argvars, ectx_T *ectx)
{
    ਪੂਰਨ		idx;
    typval_T	*tv;

    // Move arguments from bottom of the stack to argvars[] and add terminator.
    ਲਈ (idx = 0; idx < argcount; ++idx)
	argvars[idx] = *ਢੇਰ_TV_BOT(idx - argcount);
    argvars[argcount].v_type = VAR_UNKNOWN;

    // Result replaces the arguments on the stack.
    ਜੇ (argcount > 0)
	ectx->ec_ਢੇਰ.ga_len -= argcount - 1;
    ਨਹੀਂਤਾਂ ਜੇ (GA_GROW_FAILS(&ectx->ec_ਢੇਰ, 1))
	ਵਾਪਸ FAIL;
    ਨਹੀਂਤਾਂ
	++ectx->ec_ਢੇਰ.ga_len;

    // Default return value is zero.
    tv = ਢੇਰ_TV_BOT(-1);
    tv->v_type = VAR_ਸੰਖਿਆ;
    tv->vval.v_ਸੰਖਿਆ = 0;
    tv->v_ਤਾਲਾ = 0;

    ਵਾਪਸ OK;
}

/*
 * Call a builtin function by index.
 */
    ਸਥਿਰ ਪੂਰਨ
call_bfunc(ਪੂਰਨ func_idx, ਪੂਰਨ argcount, ectx_T *ectx)
{
    typval_T	argvars[MAX_FUNC_ਦਲੀਲਾਂ];
    ਪੂਰਨ		idx;
    ਪੂਰਨ		did_emsg_before = did_emsg;
    ectx_T	*prev_ectx = current_ectx;
    ਅੱਖਰ	*save_func_name = ectx->ec_where.wt_func_name;

    ਜੇ (call_prepare(argcount, argvars, ectx) == FAIL)
	ਵਾਪਸ FAIL;

    // Check for void value being passed as an argument.
    ਲਈ (idx = 0; idx < argcount; ++idx)
	ਜੇ (argvars[idx].v_type == VAR_VOID)
	{
	    emsg(_(e_cannot_use_void_ਮੁੱਲ));
	    ਲਈ (idx = 0; idx < argcount; ++idx)
		clear_tv(&argvars[idx]);
	    ਵਾਪਸ FAIL;
	}

    ectx->ec_where.wt_func_name = ਅੰਦਰੂਨੀ_func_name(func_idx);

    // Call the builtin function.  Set "current_ectx" so that when it
    // recursively invokes call_def_function() a closure context can be set.
    current_ectx = ectx;
    call_ਅੰਦਰੂਨੀ_func_by_idx(func_idx, argvars, ਢੇਰ_TV_BOT(-1));
    current_ectx = prev_ectx;
    ectx->ec_where.wt_func_name = save_func_name;

    // Clear the arguments.
    ਲਈ (idx = 0; idx < argcount; ++idx)
	clear_tv(&argvars[idx]);

    ਜੇ (did_emsg > did_emsg_before)
	ਵਾਪਸ FAIL;
    ਵਾਪਸ OK;
}

/*
 * Execute a user defined function.
 * If the function is compiled this will add a stack frame and set the
 * instruction pointer at the start of the function.
 * Otherwise the function is called here.
 * If "pt" is not null use "pt->pt_outer" for ec_outer_ref->or_outer.
 * "iptr" can be used to replace the instruction with a more efficient one.
 */
    ਸਥਿਰ ਪੂਰਨ
call_ufunc(
	ufunc_T	    *ufunc,
	partial_T   *pt,
	ਪੂਰਨ	    argcount,
	ectx_T	    *ectx,
	isn_T	    *iptr,
	dict_T	    *selfdict)
{
    typval_T	argvars[MAX_FUNC_ਦਲੀਲਾਂ];
    funcexe_T   funcexe;
    funcerror_T	error;
    ਪੂਰਨ		idx;
    ਪੂਰਨ		did_emsg_before = did_emsg;
    compiletype_T compile_type = get_compile_type(ufunc);

    ਜੇ (func_needs_compiling(ufunc, compile_type)
		&& compile_def_function(ufunc, FALSE, compile_type, ਨਿਰਾ)
								       == FAIL)
	ਵਾਪਸ FAIL;
    ਜੇ (ufunc->uf_def_ਸਥਿਤੀ == UF_COMPILED)
    {
	error = check_user_func_argcount(ufunc, argcount);
	ਜੇ (error != FCERR_UNKNOWN)
	{
	    ਜੇ (error == FCERR_TOOMANY)
		semsg(_(e_too_many_arguments_for_function_str),
						   printable_func_name(ufunc));
	    ਨਹੀਂਤਾਂ
		semsg(_(e_not_enough_arguments_for_function_str),
						   printable_func_name(ufunc));
	    ਵਾਪਸ FAIL;
	}

	// The function has been compiled, can call it quickly.  For a function
	// that was defined later: we can call it directly next time.
	ਜੇ (iptr != ਨਿਰਾ)
	{
	    delete_instr(iptr);
	    iptr->isn_type = ISN_DCALL;
	    iptr->isn_arg.dfunc.cdf_idx = ufunc->uf_dfunc_idx;
	    iptr->isn_arg.dfunc.cdf_argcount = argcount;
	}
	ਵਾਪਸ call_dfunc(ufunc->uf_dfunc_idx, pt, argcount, ectx);
    }

    ਜੇ (call_prepare(argcount, argvars, ectx) == FAIL)
	ਵਾਪਸ FAIL;
    CLEAR_ਖੇਤਰ(funcexe);
    funcexe.fe_evaluate = TRUE;
    funcexe.fe_selfdict = selfdict != ਨਿਰਾ ? selfdict : dict_ਢੇਰ_get_dict();

    // Call the user function.  Result goes in last position on the stack.
    error = call_user_func_check(ufunc, argcount, argvars,
			      ਢੇਰ_TV_BOT(-1), &funcexe, funcexe.fe_selfdict);

    // Clear the arguments.
    ਲਈ (idx = 0; idx < argcount; ++idx)
	clear_tv(&argvars[idx]);

    ਜੇ (error != FCERR_NONE)
    {
	user_func_error(error, printable_func_name(ufunc),
							 funcexe.fe_ਮਿਲਿਆ_var);
	ਵਾਪਸ FAIL;
    }
    ਜੇ (did_emsg > did_emsg_before)
	// Error other than from calling the function itself.
	ਵਾਪਸ FAIL;
    ਵਾਪਸ OK;
}

/*
 * If command modifiers were applied restore them.
 */
    ਸਥਿਰ ਖਾਲੀ
may_restore_cmdmod(funclocal_T *funclocal)
{
    ਜੇ (funclocal->floc_restore_cmdmod)
    {
	cmdmod.cmod_filter_regmatch.regprog = ਨਿਰਾ;
	undo_cmdmod(&cmdmod);
	cmdmod = funclocal->floc_save_cmdmod;
	funclocal->floc_restore_cmdmod = FALSE;
    }
}

/*
 * Return TRUE if an error was given (not caught in try/catch) or CTRL-C was
 * pressed.
 */
    ਸਥਿਰ ਪੂਰਨ
vim9_aborting(ਪੂਰਨ prev_uncaught_emsg)
{
    ਵਾਪਸ uncaught_emsg > prev_uncaught_emsg || got_int || did_throw;
}

/*
 * Execute a function by "name".
 * This can be a builtin function or a user function.
 * "iptr" can be used to replace the instruction with a more efficient one.
 * Returns FAIL if not found without an error message.
 */
    ਸਥਿਰ ਪੂਰਨ
call_by_name(
	char_u	    *name,
	ਪੂਰਨ	    argcount,
	ectx_T	    *ectx,
	isn_T	    *iptr,
	dict_T	    *selfdict)
{
    ufunc_T *ufunc;

    ਜੇ (builtin_function(name, -1))
    {
	ਪੂਰਨ func_idx = ਲੱਭੋ_ਅੰਦਰੂਨੀ_func(name);

	ਜੇ (func_idx < 0)  // Impossible?
	    ਵਾਪਸ FAIL;
	ਜੇ (check_ਅੰਦਰੂਨੀ_func(func_idx, argcount) < 0)
	    ਵਾਪਸ FAIL;
	ਵਾਪਸ call_bfunc(func_idx, argcount, ectx);
    }

    char_u	cc = NUL;
    char_u	*ਸ਼ੁਰੂ_bracket = generic_func_ਲੱਭੋ_open_bracket(name);
    ਜੇ (ਸ਼ੁਰੂ_bracket != ਨਿਰਾ)
    {
	cc = *ਸ਼ੁਰੂ_bracket;
	*ਸ਼ੁਰੂ_bracket = NUL;
    }

    ufunc = ਲੱਭੋ_func(name, FALSE);

    ਜੇ (ufunc == ਨਿਰਾ)
    {
	ਪੂਰਨ prev_uncaught_emsg = uncaught_emsg;

	ਜੇ (script_autoload(name, TRUE))
	    // loaded a package, search for the function again
	    ufunc = ਲੱਭੋ_func(name, FALSE);

	ਜੇ (vim9_aborting(prev_uncaught_emsg))
	{
	    ਜੇ (ਸ਼ੁਰੂ_bracket != ਨਿਰਾ)
		*ਸ਼ੁਰੂ_bracket = cc;
	    ਵਾਪਸ FAIL;  // bail out if loading the script caused an error
	}
    }

    ਜੇ (ਸ਼ੁਰੂ_bracket != ਨਿਰਾ)
	*ਸ਼ੁਰੂ_bracket = cc;

    ਜੇ (ufunc != ਨਿਰਾ)
    {
	ਜੇ (IS_GENERIC_FUNC(ufunc))
	{
	    ਜੇ (ਸ਼ੁਰੂ_bracket != ਨਿਰਾ)
		ufunc = ਲੱਭੋ_generic_func(ufunc, name, &ਸ਼ੁਰੂ_bracket);
	    ਨਹੀਂਤਾਂ
	    {
		emsg_funcname(e_generic_func_missing_type_ਦਲੀਲਾਂ_str, name);
		ufunc = ਨਿਰਾ;
	    }
	    ਜੇ (ufunc == ਨਿਰਾ)
		ਵਾਪਸ FAIL;
	}
	ਨਹੀਂਤਾਂ ਜੇ (ਸ਼ੁਰੂ_bracket != ਨਿਰਾ)
	{
	    emsg_funcname(e_not_a_generic_function_str, name);
	    ਵਾਪਸ FAIL;
	}

	ਜੇ (check_ufunc_arg_types(ufunc, argcount, 0, ectx) == FAIL)
	    ਵਾਪਸ FAIL;

	ਵਾਪਸ call_ufunc(ufunc, ਨਿਰਾ, argcount, ectx, iptr, selfdict);
    }

    ਵਾਪਸ FAIL;
}

    ਸਥਿਰ ਪੂਰਨ
call_partial(
	typval_T    *tv,
	ਪੂਰਨ	    argcount_arg,
	ectx_T	    *ectx)
{
    ਪੂਰਨ		argcount = argcount_arg;
    char_u	*name = ਨਿਰਾ;
    ਪੂਰਨ		called_emsg_before = called_emsg;
    ਪੂਰਨ		res = FAIL;
    dict_T	*selfdict = ਨਿਰਾ;

    ਜੇ (tv->v_type == VAR_PARTIAL)
    {
	partial_T   *pt = tv->vval.v_partial;
	ਪੂਰਨ	    i;

	ਜੇ (pt->pt_obj != ਨਿਰਾ)
	{
	    // partial with an object method.  Push the object before the
	    // function arguments.
	    ਜੇ (GA_GROW_FAILS(&ectx->ec_ਢੇਰ, 1))
		ਵਾਪਸ FAIL;
	    ਲਈ (i = 1; i <= argcount; ++i)
		*ਢੇਰ_TV_BOT(-i + 1) = *ਢੇਰ_TV_BOT(-i);

	    typval_T *obj_tv = ਢੇਰ_TV_BOT(-argcount);
	    obj_tv->v_type = VAR_OBJECT;
	    obj_tv->v_ਤਾਲਾ = 0;
	    obj_tv->vval.v_object = pt->pt_obj;
	    ++pt->pt_obj->obj_refcount;
	    ++ectx->ec_ਢੇਰ.ga_len;
	}

	ਜੇ (pt->pt_argc > 0)
	{
	    // Make space for arguments from the partial, shift the "argcount"
	    // arguments up.
	    ਜੇ (GA_GROW_FAILS(&ectx->ec_ਢੇਰ, pt->pt_argc))
		ਵਾਪਸ FAIL;
	    ਲਈ (i = 1; i <= argcount; ++i)
		*ਢੇਰ_TV_BOT(-i + pt->pt_argc) = *ਢੇਰ_TV_BOT(-i);
	    ectx->ec_ਢੇਰ.ga_len += pt->pt_argc;
	    argcount += pt->pt_argc;

	    // copy the arguments from the partial onto the stack
	    ਲਈ (i = 0; i < pt->pt_argc; ++i)
		copy_tv(&pt->pt_argv[i], ਢੇਰ_TV_BOT(-argcount + i));
	}
	selfdict = pt->pt_dict;

	ਜੇ (pt->pt_func != ਨਿਰਾ)
	    ਵਾਪਸ call_ufunc(pt->pt_func, pt, argcount, ectx, ਨਿਰਾ, selfdict);

	name = pt->pt_name;
    }
    ਨਹੀਂਤਾਂ ਜੇ (tv->v_type == VAR_FUNC)
	name = tv->vval.v_string;
    ਜੇ (name != ਨਿਰਾ)
    {
	char_u	    fname_buf[FLEN_FIXED + 1];
	char_u	    *tofree = ਨਿਰਾ;
	funcerror_T error = FCERR_NONE;
	char_u	    *fname;

	// May need to translate <SNR>123_ to K_SNR.
	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
	ਜੇ (error != FCERR_NONE)
	    res = FAIL;
	ਨਹੀਂਤਾਂ
	    res = call_by_name(fname, argcount, ectx, ਨਿਰਾ, selfdict);
	vim_free(tofree);
    }

    ਜੇ (res == FAIL)
    {
	ਜੇ (called_emsg == called_emsg_before)
	    emsg_funcname(e_unknown_function_str,
				  name == ਨਿਰਾ ? (char_u *)"[unknown]" : name);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Check if "lock" is VAR_LOCKED or VAR_FIXED.  If so give an error and return
 * TRUE.
 */
    ਸਥਿਰ ਪੂਰਨ
error_if_locked(ਪੂਰਨ ਤਾਲਾ, ਅੱਖਰ *error)
{
    ਜੇ (ਤਾਲਾ & (VAR_LOCKED | VAR_FIXED))
    {
	emsg(_(error));
	ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}

/*
 * Give an error if "tv" is not a number and return FAIL.
 */
    ਸਥਿਰ ਪੂਰਨ
check_for_ਸੰਖਿਆ(typval_T *tv)
{
    ਜੇ (tv->v_type != VAR_ਸੰਖਿਆ)
    {
	semsg(_(e_expected_str_but_got_str),
		vartype_name(VAR_ਸੰਖਿਆ), vartype_name(tv->v_type));
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Store "tv" in variable "name".
 * This is for s: and g: variables.
 */
    ਸਥਿਰ ਖਾਲੀ
store_var(char_u *name, typval_T *tv)
{
    funccal_ਇੰਦਰਾਜ_T ਇੰਦਰਾਜ;
    ਪੂਰਨ		    flags = ਸੌਂਪੋ_DECL;

    ਜੇ (tv->v_ਤਾਲਾ)
	flags |= ਸੌਂਪੋ_CONST;
    save_funccal(&ਇੰਦਰਾਜ);
    ਸੈੱਟ_var_const(name, 0, ਨਿਰਾ, tv, FALSE, flags, 0);
    restore_funccal();
}

/*
 * Convert "tv" to a string.
 * Return FAIL if not allowed.
 */
    ਸਥਿਰ ਪੂਰਨ
do_2string(typval_T *tv, ਪੂਰਨ is_2string_any, ਪੂਰਨ tostring_flags)
{
    ਜੇ (tv->v_type == VAR_STRING)
	ਵਾਪਸ OK;

    char_u *str;

    ਜੇ (is_2string_any)
    {
	ਚੋਣ (tv->v_type)
	{
	    ਹਾਲਤ VAR_SPECIAL:
	    ਹਾਲਤ VAR_BOOL:
	    ਹਾਲਤ VAR_ਸੰਖਿਆ:
	    ਹਾਲਤ VAR_FLOAT:
	    ਹਾਲਤ VAR_DICT:
	    ਹਾਲਤ VAR_BLOB:	ਰੋਕੋ;

	    ਹਾਲਤ VAR_LIST:
				ਜੇ (tostring_flags & TOSTRING_TOLERANT)
				{
				    char_u	*s, *e, *p;
				    garray_T	ga;

				    ga_init2(&ga, ਆਕਾਰ(char_u *), 1);

				    // Convert to NL separated items, then
				    // escape the items and replace the NL with
				    // a space.
				    str = typval2string(tv, TRUE);
				    ਜੇ (str == ਨਿਰਾ)
					ਵਾਪਸ FAIL;
				    s = str;
				    ਜਦੋਂ ((e = vim_strchr(s, '\n')) != ਨਿਰਾ)
				    {
					*e = NUL;
					p = vim_strsave_fnameescape(s,
						VSE_NONE);
					ਜੇ (p != ਨਿਰਾ)
					{
					    ga_concat(&ga, p);
					    GA_CONCAT_LITERAL(&ga, " ");
					    vim_free(p);
					}
					s = e + 1;
				    }
				    vim_free(str);
				    clear_tv(tv);
				    tv->v_type = VAR_STRING;
				    tv->vval.v_string = ga.ga_ਅੰਕੜੇ;
				    ਵਾਪਸ OK;
				}
				ਜੇ (tostring_flags & TOSTRING_INTERPOLATE)
				    ਰੋਕੋ;
				// FALLTHROUGH
	    ਮੂਲ:	ਮੰਜ਼ਿਲ_string_error(tv->v_type);
			ਵਾਪਸ FAIL;
	}
    }
    str = typval_tostring(tv, TRUE);
    clear_tv(tv);
    tv->v_type = VAR_STRING;
    tv->vval.v_string = str;
    ਵਾਪਸ OK;
}

/*
 * When the value of "sv" is a null list of dict, allocate it.
 */
    ਸਥਿਰ ਖਾਲੀ
allocate_if_null(svar_T *sv)
{
    typval_T *tv = sv->sv_tv;

    ਚੋਣ (tv->v_type)
    {
	ਹਾਲਤ VAR_LIST:
	    ਜੇ (tv->vval.v_list == ਨਿਰਾ && sv->sv_type != &t_list_ਖਾਲੀ_ਥਾਵਾਂ)
		(ਖਾਲੀ)rettv_list_alloc(tv);
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_TUPLE:
	    ਜੇ (tv->vval.v_tuple == ਨਿਰਾ && sv->sv_type != &t_tuple_ਖਾਲੀ_ਥਾਵਾਂ)
		(ਖਾਲੀ)rettv_tuple_alloc(tv);
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_DICT:
	    ਜੇ (tv->vval.v_dict == ਨਿਰਾ && sv->sv_type != &t_dict_ਖਾਲੀ_ਥਾਵਾਂ)
		(ਖਾਲੀ)rettv_dict_alloc(tv);
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_BLOB:
	    ਜੇ (tv->vval.v_blob == ਨਿਰਾ && sv->sv_type != &t_blob_null)
		(ਖਾਲੀ)rettv_blob_alloc(tv);
	    ਰੋਕੋ;
	ਮੂਲ:
	    ਰੋਕੋ;
    }
}

/*
 * Return the character "str[index]" where "index" is the character index,
 * including composing characters.
 * If "index" is out of range NULL is returned.
 */
    char_u *
char_from_string(char_u *str, varnumber_T ਸੂਚਕਾਂਕ)
{
    size_t	    nbyte = 0;
    varnumber_T	    nchar = ਸੂਚਕਾਂਕ;
    size_t	    slen;

    ਜੇ (str == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    slen = STRLEN(str);

    // Do the same as for a list: a negative index counts from the end.
    // Optimization to check the first byte to be below 0x80 (and no composing
    // character follows) makes this a lot faster.
    ਜੇ (ਸੂਚਕਾਂਕ < 0)
    {
	ਪੂਰਨ	clen = 0;

	ਲਈ (nbyte = 0; nbyte < slen; ++clen)
	{
	    ਜੇ (str[nbyte] < 0x80 && str[nbyte + 1] < 0x80)
		++nbyte;
	    ਨਹੀਂਤਾਂ ਜੇ (enc_utf8)
		nbyte += utfc_ptr2len(str + nbyte);
	    ਨਹੀਂਤਾਂ
		nbyte += mb_ptr2len(str + nbyte);
	}
	nchar = clen + ਸੂਚਕਾਂਕ;
	ਜੇ (nchar < 0)
	    // unlike list: index out of range results in empty string
	    ਵਾਪਸ ਨਿਰਾ;
    }

    ਲਈ (nbyte = 0; nchar > 0 && nbyte < slen; --nchar)
    {
	ਜੇ (str[nbyte] < 0x80 && str[nbyte + 1] < 0x80)
	    ++nbyte;
	ਨਹੀਂਤਾਂ ਜੇ (enc_utf8)
	    nbyte += utfc_ptr2len(str + nbyte);
	ਨਹੀਂਤਾਂ
	    nbyte += mb_ptr2len(str + nbyte);
    }
    ਜੇ (nbyte >= slen)
	ਵਾਪਸ ਨਿਰਾ;
    ਵਾਪਸ vim_strnsave(str + nbyte, mb_ptr2len(str + nbyte));
}

/*
 * Get the byte index for character index "idx" in string "str" with length
 * "str_len".  Composing characters are included.
 * If going over the end return "str_len".
 * If "idx" is negative count from the end, -1 is the last character.
 * When going over the start return -1.
 */
    ਸਥਿਰ ਲੰਮਾ
char_idx2byte(char_u *str, size_t str_len, varnumber_T idx)
{
    varnumber_T nchar = idx;
    size_t	nbyte = 0;

    ਜੇ (nchar >= 0)
    {
	ਜਦੋਂ (nchar > 0 && nbyte < str_len)
	{
	    nbyte += mb_ptr2len(str + nbyte);
	    --nchar;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	nbyte = str_len;
	ਜਦੋਂ (nchar < 0 && nbyte > 0)
	{
	    --nbyte;
	    nbyte -= mb_ਸਿਰ_off(str, str + nbyte);
	    ++nchar;
	}
	ਜੇ (nchar < 0)
	    ਵਾਪਸ -1;
    }
    ਵਾਪਸ (ਲੰਮਾ)nbyte;
}

/*
 * Return the slice "str[first : last]" using character indexes.  Composing
 * characters are included.
 * "exclusive" is TRUE for slice().
 * Return NULL when the result is empty.
 */
    char_u *
string_ਟੁਕੜਾ(char_u *str, varnumber_T ਪਹਿਲਾ, varnumber_T ਆਖਰੀ, ਪੂਰਨ exclusive)
{
    ਲੰਮਾ	ਸ਼ੁਰੂ_byte, ਸਿਰਾ_byte;
    size_t	slen;

    ਜੇ (str == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    slen = STRLEN(str);
    ਸ਼ੁਰੂ_byte = char_idx2byte(str, slen, ਪਹਿਲਾ);
    ਜੇ (ਸ਼ੁਰੂ_byte < 0)
	ਸ਼ੁਰੂ_byte = 0; // first index very negative: use zero
    ਜੇ ((ਆਖਰੀ == -1 && !exclusive) || ਆਖਰੀ == VARNUM_MAX)
	ਸਿਰਾ_byte = (ਲੰਮਾ)slen;
    ਨਹੀਂਤਾਂ
    {
	ਸਿਰਾ_byte = char_idx2byte(str, slen, ਆਖਰੀ);
	ਜੇ (!exclusive && ਸਿਰਾ_byte >= 0 && ਸਿਰਾ_byte < (ਲੰਮਾ)slen)
	    // end index is inclusive
	    ਸਿਰਾ_byte += mb_ptr2len(str + ਸਿਰਾ_byte);
    }

    ਜੇ (ਸ਼ੁਰੂ_byte >= (ਲੰਮਾ)slen || ਸਿਰਾ_byte <= ਸ਼ੁਰੂ_byte)
	ਵਾਪਸ ਨਿਰਾ;
    ਵਾਪਸ vim_strnsave(str + ਸ਼ੁਰੂ_byte, ਸਿਰਾ_byte - ਸ਼ੁਰੂ_byte);
}

/*
 * Get a script variable for ISN_STORESCRIPT and ISN_LOADSCRIPT.
 * When "dfunc_idx" is negative don't give an error.
 * Returns NULL for an error.
 */
    ਸਥਿਰ svar_T *
get_script_svar(scriptref_T *sref, ਪੂਰਨ dfunc_idx)
{
    scriptitem_T    *si = SCRIPT_ਵਸਤੂ(sref->sref_sid);
    dfunc_T	    *dfunc = dfunc_idx < 0 ? ਨਿਰਾ
			      : ((dfunc_T *)def_functions.ga_ਅੰਕੜੇ) + dfunc_idx;
    svar_T	    *sv;

    ਜੇ (sref->sref_seq != si->sn_script_seq)
    {
	// The script was reloaded after the function was compiled, the
	// script_idx may not be valid.
	ਜੇ (dfunc != ਨਿਰਾ)
	    semsg(_(e_script_variable_invalid_after_reload_ਵਿੱਚ_function_str),
					 printable_func_name(dfunc->df_ufunc));
	ਵਾਪਸ ਨਿਰਾ;
    }
    sv = ((svar_T *)si->sn_var_vals.ga_ਅੰਕੜੇ) + sref->sref_idx;
    ਜੇ (sv->sv_name == ਨਿਰਾ)
    {
	ਜੇ (dfunc != ਨਿਰਾ)
	    emsg(_(e_script_variable_was_deleted));
	ਵਾਪਸ ਨਿਰਾ;
    }
    ਜੇ (!equal_type(sv->sv_type, sref->sref_type, 0))
    {
	ਜੇ (dfunc != ਨਿਰਾ)
	    emsg(_(e_script_variable_type_ਬਦਲਿਆ));
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਜੇ ((sv->sv_flags & SVFLAG_EXPORTED) == 0
				      && sref->sref_sid != current_sctx.sc_sid)
    {
	ਜੇ (dfunc != ਨਿਰਾ)
	    semsg(_(e_ਵਸਤੂ_not_exported_ਵਿੱਚ_script_str), sv->sv_name);
	ਵਾਪਸ ਨਿਰਾ;
    }
    ਵਾਪਸ sv;
}

/*
 * Function passed to do_cmdline() for splitting a script joined by NL
 * characters.
 */
    ਸਥਿਰ char_u *
get_ਵੰਡੋ_sourceline(
	ਪੂਰਨ c ਅਣਵਰਤਿਆ,
	ਖਾਲੀ *cookie,
	ਪੂਰਨ indent ਅਣਵਰਤਿਆ,
	getline_opt_T options ਅਣਵਰਤਿਆ)
{
    ਸਰੋਤ_cookie_T	*sp = (ਸਰੋਤ_cookie_T *)cookie;
    char_u		*p;
    char_u		*ਸਤਰ;

    p = vim_strchr(sp->nextline, '\n');
    ਜੇ (p == ਨਿਰਾ)
    {
	ਸਤਰ = vim_strsave(sp->nextline);
	sp->nextline += STRLEN(sp->nextline);
    }
    ਨਹੀਂਤਾਂ
    {
	ਸਤਰ = vim_strnsave(sp->nextline, p - sp->nextline);
	sp->nextline = p + 1;
    }
    ਵਾਪਸ ਸਤਰ;
}

/*
 * Execute a function by "name".
 * This can be a builtin function, user function or a funcref.
 * "iptr" can be used to replace the instruction with a more efficient one.
 */
    ਸਥਿਰ ਪੂਰਨ
call_eval_func(
	char_u	    *name,
	ਪੂਰਨ	    argcount,
	ectx_T	    *ectx,
	isn_T	    *iptr)
{
    ਪੂਰਨ	    called_emsg_before = called_emsg;
    ਪੂਰਨ	    res;

    res = call_by_name(name, argcount, ectx, iptr, ਨਿਰਾ);
    ਜੇ (res == FAIL && called_emsg == called_emsg_before)
    {
	dictitem_T	*v;

	v = ਲੱਭੋ_var(name, ਨਿਰਾ, FALSE);
	ਜੇ (v == ਨਿਰਾ || (v->di_tv.v_type != VAR_PARTIAL
					       && v->di_tv.v_type != VAR_FUNC))
	{
	    emsg_funcname(e_unknown_function_str, name);
	    ਵਾਪਸ FAIL;
	}
	ਵਾਪਸ call_partial(&v->di_tv, argcount, ectx);
    }
    ਵਾਪਸ res;
}

/*
 * When a function reference is used, fill a partial with the information
 * needed, especially when it is used as a closure.
 */
    ਪੂਰਨ
fill_partial_and_ਘੇਰਾ(
	partial_T	*pt,
	ufunc_T		*ufunc,
	loopvarinfo_T	*lvi,
	ectx_T		*ectx)
{
    pt->pt_func = ufunc;
    pt->pt_refcount = 1;

    ਜੇ (ufunc->uf_flags & FC_ਘੇਰਾ)
    {
	dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_ਅੰਕੜੇ)
							  + ectx->ec_dfunc_idx;

	// The closure may need to find arguments and local variables of the
	// current function in the stack.
	pt->pt_outer.ਬਾਹਰਲਾ_ਢੇਰ = &ectx->ec_ਢੇਰ;
	pt->pt_outer.ਬਾਹਰਲਾ_ਚੌਖਟਾ_idx = ectx->ec_ਚੌਖਟਾ_idx;
	ਜੇ (ectx->ec_outer_ref != ਨਿਰਾ)
	{
	    // The current context already has a context, link to that one.
	    pt->pt_outer.ਬਾਹਰਲਾ_ਉੱਪਰ = ectx->ec_outer_ref->or_outer;
	    ਜੇ (ectx->ec_outer_ref->or_partial != ਨਿਰਾ)
	    {
		pt->pt_outer.ਬਾਹਰਲਾ_ਉੱਪਰ_partial = ectx->ec_outer_ref->or_partial;
		++pt->pt_outer.ਬਾਹਰਲਾ_ਉੱਪਰ_partial->pt_refcount;
	    }
	}

	ਜੇ (lvi != ਨਿਰਾ)
	{
	    ਪੂਰਨ	ਡੂੰਘਾਈ;

	    // The closure may need to find variables defined inside a loop,
	    // for every nested loop.  A new reference is made every time,
	    // ISN_ENDLOOP will check if they are actually used.
	    ਲਈ (ਡੂੰਘਾਈ = 0; ਡੂੰਘਾਈ < lvi->lvi_ਡੂੰਘਾਈ; ++ਡੂੰਘਾਈ)
	    {
		pt->pt_outer.ਬਾਹਰਲਾ_loop[ਡੂੰਘਾਈ].ਢੇਰ = &ectx->ec_ਢੇਰ;
		pt->pt_outer.ਬਾਹਰਲਾ_loop[ਡੂੰਘਾਈ].var_idx = ectx->ec_ਚੌਖਟਾ_idx
			 + ਢੇਰ_ਚੌਖਟਾ_ਮਾਪ + lvi->lvi_loop[ਡੂੰਘਾਈ].var_idx;
		pt->pt_outer.ਬਾਹਰਲਾ_loop[ਡੂੰਘਾਈ].var_ਗਿਣਤੀ =
					    lvi->lvi_loop[ਡੂੰਘਾਈ].var_ਗਿਣਤੀ;
	    }
	    pt->pt_outer.ਬਾਹਰਲਾ_loop_ਮਾਪ = lvi->lvi_ਡੂੰਘਾਈ;
	}
	ਨਹੀਂਤਾਂ
	    pt->pt_outer.ਬਾਹਰਲਾ_loop_ਮਾਪ = 0;

	// If the function currently executing returns and the closure is still
	// being referenced, we need to make a copy of the context (arguments
	// and local variables) so that the closure can use it later.
	// Store a reference to the partial so we can handle that.
	ਜੇ (GA_GROW_FAILS(&ectx->ec_funcrefs, 1))
	    // caller needs to free pt
	    ਵਾਪਸ FAIL;
	// Extra variable keeps the count of closures created in the current
	// function call.
	++(((typval_T *)ectx->ec_ਢੇਰ.ga_ਅੰਕੜੇ) + ectx->ec_ਚੌਖਟਾ_idx
		       + ਢੇਰ_ਚੌਖਟਾ_ਮਾਪ + dfunc->df_varcount)->vval.v_ਸੰਖਿਆ;

	((partial_T **)ectx->ec_funcrefs.ga_ਅੰਕੜੇ)[ectx->ec_funcrefs.ga_len]
									  = pt;
	++pt->pt_refcount;
	++ectx->ec_funcrefs.ga_len;
    }
    ++ufunc->uf_refcount;
    ਵਾਪਸ OK;
}

/*
 * Execute iptr->isn_arg.string as an Ex command.
 */
    ਸਥਿਰ ਪੂਰਨ
exec_command(isn_T *iptr, char_u *cmd_string)
{
    ਸਰੋਤ_cookie_T cookie;

    SOURCING_LNUM = iptr->isn_lnum;
    // Pass getsourceline to get an error for a missing ":end" command.
    CLEAR_ਖੇਤਰ(cookie);
    cookie.sourcing_lnum = iptr->isn_lnum - 1;
    ਜੇ (do_cmdline(cmd_string, getsourceline, &cookie,
			     DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED) == FAIL
		|| did_emsg)
	ਵਾਪਸ FAIL;
    ਵਾਪਸ OK;
}

/*
 * If script "sid" is not loaded yet then load it now.
 * Caller must make sure "sid" is a valid script ID.
 * "loaded" is set to TRUE if the script had to be loaded.
 * Returns FAIL if loading fails, OK if already loaded or loaded now.
 */
    ਪੂਰਨ
may_ਲੱਦੋ_script(ਪੂਰਨ sid, ਪੂਰਨ *loaded)
{
    scriptitem_T *si = SCRIPT_ਵਸਤੂ(sid);

    ਜੇ (si->sn_state == SN_STATE_NOT_LOADED)
    {
	*loaded = TRUE;
	ਜੇ (do_ਸਰੋਤ(si->sn_name, FALSE, DOSO_NONE, ਨਿਰਾ) == FAIL)
	{
	    semsg(_(e_cant_open_file_str), si->sn_name);
	    ਵਾਪਸ FAIL;
	}
    }
    ਵਾਪਸ OK;
}

// used for v_instr of typval of VAR_INSTR
ਬਣਤਰ instr_S {
    ectx_T	*instr_ectx;
    isn_T	*instr_instr;
};

// used for substitute_instr
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ subs_ਵਿਅੰਜਕ_S {
    ectx_T	*subs_ectx;
    isn_T	*subs_instr;
    ਪੂਰਨ		subs_ਸਥਿਤੀ;
} subs_ਵਿਅੰਜਕ_T;

// Set when calling do_debug().
ਸਥਿਰ ectx_T	*debug_context = ਨਿਰਾ;
ਸਥਿਰ ਪੂਰਨ	debug_var_ਗਿਣਤੀ;

/*
 * When debugging lookup "name" and return the typeval.
 * When not found return NULL.
 */
    typval_T *
lookup_debug_var(char_u *name)
{
    ਪੂਰਨ		    idx;
    dfunc_T	    *dfunc;
    ufunc_T	    *ufunc;
    ectx_T	    *ectx = debug_context;
    ਪੂਰਨ		    varargs_off;

    ਜੇ (ectx == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    dfunc = ((dfunc_T *)def_functions.ga_ਅੰਕੜੇ) + ectx->ec_dfunc_idx;

    // Go through the local variable names, from last to first.
    ਲਈ (idx = debug_var_ਗਿਣਤੀ - 1; idx >= 0; --idx)
    {
	char_u *varname = ((char_u **)dfunc->df_var_ਨਾਂ.ga_ਅੰਕੜੇ)[idx];

	// the variable name may be NULL when not available in this block
	ਜੇ (varname != ਨਿਰਾ && STRCMP(varname, name) == 0)
	    ਵਾਪਸ ਢੇਰ_TV_VAR(idx);
    }

    // Go through argument names.
    ufunc = dfunc->df_ufunc;
    varargs_off = ufunc->uf_va_name == ਨਿਰਾ ? 0 : 1;
    ਲਈ (idx = 0; idx < ufunc->uf_ਦਲੀਲਾਂ.ga_len; ++idx)
	ਜੇ (STRCMP(((char_u **)(ufunc->uf_ਦਲੀਲਾਂ.ga_ਅੰਕੜੇ))[idx], name) == 0)
	    ਵਾਪਸ ਢੇਰ_TV(ectx->ec_ਚੌਖਟਾ_idx - ufunc->uf_ਦਲੀਲਾਂ.ga_len
							  - varargs_off + idx);
    ਜੇ (ufunc->uf_va_name != ਨਿਰਾ && STRCMP(ufunc->uf_va_name, name) == 0)
	ਵਾਪਸ ਢੇਰ_TV(ectx->ec_ਚੌਖਟਾ_idx - 1);

    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Return TRUE if there might be a breakpoint in "ufunc", which is when a
 * breakpoint was set in that function or when there is any expression.
 */
    ਪੂਰਨ
may_break_ਵਿੱਚ_function(ufunc_T *ufunc)
{
    ਵਾਪਸ ufunc->uf_has_breakpoint || debug_has_ਵਿਅੰਜਕ_breakpoint();
}

    ਸਥਿਰ ਖਾਲੀ
handle_debug(isn_T *iptr, ectx_T *ectx)
{
    char_u	*ਸਤਰ;
    ufunc_T	*ufunc = (((dfunc_T *)def_functions.ga_ਅੰਕੜੇ)
					       + ectx->ec_dfunc_idx)->df_ufunc;
    isn_T	*ni;
    ਪੂਰਨ		ਸਿਰਾ_lnum = iptr->isn_lnum;
    garray_T	ga;
    ਪੂਰਨ		lnum;

    ਜੇ (ex_nesting_level > debug_break_level)
    {
	linenr_T breakpoint;

	ਜੇ (!may_break_ਵਿੱਚ_function(ufunc))
	    ਵਾਪਸ;

	// check for the next breakpoint if needed
	breakpoint = dbg_ਲੱਭੋ_breakpoint(FALSE, ufunc->uf_name,
					   iptr->isn_arg.debug.dbg_break_lnum);
	ਜੇ (breakpoint <= 0 || breakpoint > iptr->isn_lnum)
	    ਵਾਪਸ;
    }

    SOURCING_LNUM = iptr->isn_lnum;
    debug_context = ectx;
    debug_var_ਗਿਣਤੀ = iptr->isn_arg.debug.dbg_var_ਨਾਂ_len;

    ਲਈ (ni = iptr + 1; ni->isn_type != ISN_ਅੰਤ; ++ni)
	ਜੇ (ni->isn_type == ISN_DEBUG
		  || ni->isn_type == ISN_RETURN
		  || ni->isn_type == ISN_RETURN_OBJECT
		  || ni->isn_type == ISN_RETURN_VOID)
	{
	    ਸਿਰਾ_lnum = ni->isn_lnum + (ni->isn_type == ISN_DEBUG ? 0 : 1);
	    ਰੋਕੋ;
	}

    ਜੇ (ਸਿਰਾ_lnum > iptr->isn_lnum)
    {
	ga_init2(&ga, ਆਕਾਰ(char_u *), 10);
	ਲਈ (lnum = iptr->isn_lnum; lnum < ਸਿਰਾ_lnum
				     && lnum <= ufunc->uf_ਸਤਰਾਂ.ga_len; ++lnum)
	{
	    char_u *p = ((char_u **)ufunc->uf_ਸਤਰਾਂ.ga_ਅੰਕੜੇ)[lnum - 1];

	    ਜੇ (p == ਨਿਰਾ)
		ਜਾਰੀ;  // left over from continuation line
	    p = skipwhite(p);
	    ਜੇ (*p == '#')
		ਰੋਕੋ;
	    ਜੇ (GA_GROW_OK(&ga, 1))
		((char_u **)(ga.ga_ਅੰਕੜੇ))[ga.ga_len++] = p;
	    ਜੇ (STRNCMP(p, "def ", 4) == 0)
		ਰੋਕੋ;
	}
	ਸਤਰ = ga_concat_ਲੜੀਆਂ(&ga, "  ");
	vim_free(ga.ga_ਅੰਕੜੇ);
    }
    ਨਹੀਂਤਾਂ
	ਸਤਰ = ((char_u **)ufunc->uf_ਸਤਰਾਂ.ga_ਅੰਕੜੇ)[iptr->isn_lnum - 1];

    do_debug(ਸਤਰ == ਨਿਰਾ ? (char_u *)"[empty]" : ਸਤਰ);
    debug_context = ਨਿਰਾ;

    ਜੇ (ਸਿਰਾ_lnum > iptr->isn_lnum)
	vim_free(ਸਤਰ);
}

/*
 * Do a runtime check of the RHS value against the LHS List member type.
 * This is used by the STOREINDEX instruction to perform a type check
 * at runtime if compile time type check cannot be performed (VAR_ANY).
 * Returns FAIL if there is a type mismatch.
 */
    ਸਥਿਰ ਪੂਰਨ
storeindex_check_list_member_type(
    list_T	*lhs_list,
    typval_T	*rhs_tv,
    ectx_T	*ectx)
{
    ਜੇ (lhs_list->lv_type == ਨਿਰਾ || lhs_list->lv_type->tt_member == ਨਿਰਾ)
	ਵਾਪਸ OK;

    ਵਾਪਸ check_typval_type(lhs_list->lv_type->tt_member, rhs_tv,
			     ectx->ec_where);
}

/*
 * Store a value in a list, dict, blob or object variable.
 * Returns OK, FAIL or NOTDONE (uncatchable error).
 */
    ਸਥਿਰ ਪੂਰਨ
execute_storeindex(isn_T *iptr, ectx_T *ectx)
{
    vartype_T	dest_type = iptr->isn_arg.storeindex.si_vartype;
    typval_T	*tv;
    typval_T	*tv_idx = ਢੇਰ_TV_BOT(-2);
    ਲੰਮਾ	lidx = 0;
    typval_T	*tv_dest = ਢੇਰ_TV_BOT(-1);
    ਪੂਰਨ		ਸਥਿਤੀ = OK;
    ਪੂਰਨ		check_rhs_type = FALSE;

    ਜੇ (tv_idx->v_type == VAR_ਸੰਖਿਆ)
	lidx = (ਲੰਮਾ)tv_idx->vval.v_ਸੰਖਿਆ;

    // Stack contains:
    // -3 value to be stored
    // -2 index
    // -1 dict, list, blob, object or class
    tv = ਢੇਰ_TV_BOT(-3);
    SOURCING_LNUM = iptr->isn_lnum;

    // Make sure an object has been initialized
    ਜੇ (dest_type == VAR_OBJECT && tv_dest->vval.v_object == ਨਿਰਾ)
    {
	emsg(_(e_using_null_object));
	ਸਥਿਤੀ = FAIL;
    }
    ਨਹੀਂਤਾਂ ਜੇ (dest_type == VAR_ANY)
    {
	check_rhs_type = TRUE;
	dest_type = tv_dest->v_type;
	ਜੇ (dest_type == VAR_DICT)
	    ਸਥਿਤੀ = do_2string(tv_idx, TRUE, FALSE);
	ਨਹੀਂਤਾਂ ਜੇ (dest_type == VAR_OBJECT && tv_idx->v_type == VAR_STRING)
	{
	    // Need to get the member index now that the class is known.
	    object_T *obj = tv_dest->vval.v_object;
	    ਜੇ (obj == ਨਿਰਾ)
	    {
		emsg(_(e_using_null_object));
		ਸਥਿਤੀ = FAIL;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		class_T *cl = obj->obj_class;
		char_u  *member = tv_idx->vval.v_string;

		ਪੂਰਨ		m_idx;
		ocmember_T *m = object_member_lookup(cl, member, 0, &m_idx);
		ਜੇ (m != ਨਿਰਾ)
		{
		    // Get the current function
		    ufunc_T *ufunc = (((dfunc_T *)def_functions.ga_ਅੰਕੜੇ)
					+ ectx->ec_dfunc_idx)->df_ufunc;
		    where_T where = WHERE_INIT;

		    // Check whether the member variable is writeable
		    ਜੇ ((m->ocm_access != VIM_ACCESS_ALL) &&
			    (ufunc->uf_class == ਨਿਰਾ ||
			     !class_instance_ਦਾ(ufunc->uf_class, cl)))
		    {
			ਅੱਖਰ *ਸੁਨੇਹਾ = (m->ocm_access == VIM_ACCESS_ਨਿੱਜੀ)
			    ? e_cannot_access_protected_variable_str
			    : e_variable_is_not_writable_str;
			emsg_var_cl_define(ਸੁਨੇਹਾ, m->ocm_name.string, 0, cl);
			ਸਥਿਤੀ = FAIL;
		    }
		    // Fail if the variable is a const or final or the type
		    // is not compatible
		    ਨਹੀਂਤਾਂ ਜੇ (oc_var_check_ro(cl, m) ||
			     check_typval_type(m->ocm_type, tv, where)
								== FAIL)
			ਸਥਿਤੀ = FAIL;
		    ਨਹੀਂਤਾਂ
			lidx = m_idx;
		}
		ਨਹੀਂਤਾਂ
		{
		    member_not_ਮਿਲਿਆ_ਸੁਨੇਹਾ(cl, VAR_OBJECT, member, 0);
		    ਸਥਿਤੀ = FAIL;
		}
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (dest_type == VAR_OBJECT)
	{
	    emsg(_(e_string_required));
	    ਸਥਿਤੀ = FAIL;
	}
	ਨਹੀਂਤਾਂ ਜੇ (dest_type == VAR_LIST && tv_idx->v_type != VAR_ਸੰਖਿਆ)
	{
	    emsg(_(e_ਸੰਖਿਆ_expected));
	    ਸਥਿਤੀ = FAIL;
	}
    }

    ਜੇ (ਸਥਿਤੀ == OK)
    {
	ਜੇ (dest_type == VAR_LIST)
	{
	    list_T	    *list = tv_dest->vval.v_list;

	    ਜੇ (list == ਨਿਰਾ)
	    {
		emsg(_(e_list_not_ਸੈੱਟ));
		ਵਾਪਸ FAIL;
	    }
	    ਜੇ (lidx < 0 && list->lv_len + lidx >= 0)
		// negative index is relative to the end
		lidx = list->lv_len + lidx;
	    ਜੇ (lidx < 0 || lidx > list->lv_len)
	    {
		semsg(_(e_list_ਸੂਚਕਾਂਕ_ਬਾਹਰਲਾ_ਦਾ_range_nr), lidx);
		ਵਾਪਸ FAIL;
	    }

	    // Do a runtime type check for VAR_ANY
	    ਜੇ (check_rhs_type &&
		    storeindex_check_list_member_type(list, tv, ectx) == FAIL)
		ਵਾਪਸ FAIL;

	    ਜੇ (lidx < list->lv_len)
	    {
		listitem_T *li = list_ਲੱਭੋ(list, lidx);
		ਜੇ (li == ਨਿਰਾ)
		    ਵਾਪਸ FAIL;
		ਜੇ (error_if_locked(li->li_tv.v_ਤਾਲਾ,
					     e_cannot_change_locked_list_ਵਸਤੂ))
		    ਵਾਪਸ FAIL;
		// overwrite existing list item
		clear_tv(&li->li_tv);
		li->li_tv = *tv;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (error_if_locked(list->lv_ਤਾਲਾ, e_cannot_change_locked_list))
		    ਵਾਪਸ FAIL;
		// append to list, only fails when out of memory
		ਜੇ (list_append_tv(list, tv) == FAIL)
		    ਵਾਪਸ NOTDONE;
		clear_tv(tv);
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (dest_type == VAR_DICT)
	{
	    char_u		*ਕੁੰਜੀ = tv_idx->vval.v_string;
	    dict_T		*dict = tv_dest->vval.v_dict;
	    dictitem_T	*di;

	    SOURCING_LNUM = iptr->isn_lnum;
	    ਜੇ (dict == ਨਿਰਾ)
	    {
		emsg(_(e_dictionary_not_ਸੈੱਟ));
		ਵਾਪਸ FAIL;
	    }
	    ਜੇ (ਕੁੰਜੀ == ਨਿਰਾ)
		ਕੁੰਜੀ = (char_u *)"";
	    di = dict_ਲੱਭੋ(dict, ਕੁੰਜੀ, -1);
	    ਜੇ (di != ਨਿਰਾ)
	    {
		ਜੇ (error_if_locked(di->di_tv.v_ਤਾਲਾ,
						    e_cannot_change_dict_ਵਸਤੂ))
		    ਵਾਪਸ FAIL;
		// overwrite existing value
		clear_tv(&di->di_tv);
		di->di_tv = *tv;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (error_if_locked(dict->dv_ਤਾਲਾ, e_cannot_change_dict))
		    ਵਾਪਸ FAIL;
		// add to dict, only fails when out of memory
		ਜੇ (dict_ਜੋੜੋ_tv(dict, (ਅੱਖਰ *)ਕੁੰਜੀ, tv) == FAIL)
		    ਵਾਪਸ NOTDONE;
		clear_tv(tv);
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (dest_type == VAR_BLOB)
	{
	    blob_T	    *blob = tv_dest->vval.v_blob;
	    varnumber_T	    nr;
	    ਪੂਰਨ		    error = FALSE;
	    ਪੂਰਨ		    len;

	    ਜੇ (blob == ਨਿਰਾ)
	    {
		emsg(_(e_blob_not_ਸੈੱਟ));
		ਵਾਪਸ FAIL;
	    }
	    len = blob_len(blob);
	    ਜੇ (lidx < 0 && len + lidx >= 0)
		// negative index is relative to the end
		lidx = len + lidx;

	    // Can add one byte at the end.
	    ਜੇ (lidx < 0 || lidx > len)
	    {
		semsg(_(e_blob_ਸੂਚਕਾਂਕ_ਬਾਹਰਲਾ_ਦਾ_range_nr), lidx);
		ਵਾਪਸ FAIL;
	    }
	    ਜੇ (ਮੁੱਲ_check_ਤਾਲਾ(blob->bv_ਤਾਲਾ, (char_u *)"blob", FALSE))
		ਵਾਪਸ FAIL;
	    nr = tv_get_ਸੰਖਿਆ_chk(tv, &error);
	    ਜੇ (error)
		ਵਾਪਸ FAIL;
	    ਜੇ (nr < 0 || nr > 255)
	    {
		semsg(_(e_invalid_ਮੁੱਲ_for_blob_nr), nr);
		ਵਾਪਸ FAIL;
	    }
	    blob_ਸੈੱਟ_append(blob, lidx, nr);
	}
	ਨਹੀਂਤਾਂ ਜੇ (dest_type == VAR_CLASS || dest_type == VAR_OBJECT)
	{
	    typval_T	    *otv;

	    ਜੇ (dest_type == VAR_OBJECT)
	    {
		object_T	*obj = tv_dest->vval.v_object;

		otv = (typval_T *)(obj + 1);
		class_T	    *itf = iptr->isn_arg.storeindex.si_class;
		ਜੇ (itf != ਨਿਰਾ)
		    // convert interface member index to class member index
		    lidx = object_ਸੂਚਕਾਂਕ_from_itf_ਸੂਚਕਾਂਕ(itf, FALSE, lidx,
						       obj->obj_class);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// VAR_CLASS
		class_T	*class = tv_dest->vval.v_class;
		otv = class->class_members_tv;
	    }

	    clear_tv(&otv[lidx]);
	    otv[lidx] = *tv;
	}
	ਨਹੀਂਤਾਂ ਜੇ (dest_type == VAR_TUPLE)
	{
	    emsg(_(e_tuple_is_immutable));
	    ਸਥਿਤੀ = FAIL;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਸਥਿਤੀ = FAIL;
	    semsg(_(e_cannot_ਸੂਚਕਾਂਕ_str), vartype_name(dest_type));
	}
    }

    clear_tv(tv_idx);
    clear_tv(tv_dest);
    ectx->ec_ਢੇਰ.ga_len -= 3;
    ਜੇ (ਸਥਿਤੀ == FAIL)
    {
	clear_tv(tv);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Store a value in a list or blob range.
 */
    ਸਥਿਰ ਪੂਰਨ
execute_storerange(isn_T *iptr, ectx_T *ectx)
{
    typval_T	*tv;
    typval_T	*tv_idx1 = ਢੇਰ_TV_BOT(-3);
    typval_T	*tv_idx2 = ਢੇਰ_TV_BOT(-2);
    typval_T	*tv_dest = ਢੇਰ_TV_BOT(-1);
    ਪੂਰਨ		ਸਥਿਤੀ = OK;

    // Stack contains:
    // -4 value to be stored
    // -3 first index or "none"
    // -2 second index or "none"
    // -1 destination list or blob
    tv = ਢੇਰ_TV_BOT(-4);
    SOURCING_LNUM = iptr->isn_lnum;
    ਜੇ (tv_dest->v_type == VAR_LIST)
    {
	ਲੰਮਾ	    n1;
	ਲੰਮਾ	    n2;
	listitem_T  *li1;

	n1 = (ਲੰਮਾ)tv_get_ਸੰਖਿਆ_chk(tv_idx1, ਨਿਰਾ);
	ਜੇ (tv_idx2->v_type == VAR_SPECIAL
		    && tv_idx2->vval.v_ਸੰਖਿਆ == VVAL_NONE)
	    n2 = list_len(tv_dest->vval.v_list) - 1;
	ਨਹੀਂਤਾਂ
	    n2 = (ਲੰਮਾ)tv_get_ਸੰਖਿਆ_chk(tv_idx2, ਨਿਰਾ);

	li1 = check_range_ਸੂਚਕਾਂਕ_one(tv_dest->vval.v_list, &n1, TRUE, FALSE);
	ਜੇ (li1 == ਨਿਰਾ)
	    ਸਥਿਤੀ = FAIL;
	ਨਹੀਂਤਾਂ
	{
	    ਸਥਿਤੀ = check_range_ਸੂਚਕਾਂਕ_two(tv_dest->vval.v_list,
							 &n1, li1, &n2, FALSE);
	    ਜੇ (ਸਥਿਤੀ != FAIL)
		ਸਥਿਤੀ = list_ਸੌਂਪੋ_range(
			tv_dest->vval.v_list,
			tv->vval.v_list,
			n1,
			n2,
			tv_idx2->v_type == VAR_SPECIAL,
			(char_u *)"=",
			(char_u *)"[unknown]");
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (tv_dest->v_type == VAR_BLOB)
    {
	varnumber_T n1;
	varnumber_T n2;
	ਲੰਮਾ	    bloblen;

	n1 = tv_get_ਸੰਖਿਆ_chk(tv_idx1, ਨਿਰਾ);
	ਜੇ (tv_idx2->v_type == VAR_SPECIAL
					&& tv_idx2->vval.v_ਸੰਖਿਆ == VVAL_NONE)
	    n2 = blob_len(tv_dest->vval.v_blob) - 1;
	ਨਹੀਂਤਾਂ
	    n2 = tv_get_ਸੰਖਿਆ_chk(tv_idx2, ਨਿਰਾ);
	bloblen = blob_len(tv_dest->vval.v_blob);

	ਜੇ (check_blob_ਸੂਚਕਾਂਕ(bloblen, n1, FALSE) == FAIL
		|| check_blob_range(bloblen, n1, n2, FALSE) == FAIL)
	    ਸਥਿਤੀ = FAIL;
	ਨਹੀਂਤਾਂ
	    ਸਥਿਤੀ = blob_ਸੈੱਟ_range(tv_dest->vval.v_blob, n1, n2, tv);
    }
    ਨਹੀਂਤਾਂ
    {
	ਸਥਿਤੀ = FAIL;
	emsg(_(e_list_or_blob_required));
    }

    clear_tv(tv_idx1);
    clear_tv(tv_idx2);
    clear_tv(tv_dest);
    ectx->ec_ਢੇਰ.ga_len -= 4;
    clear_tv(tv);

    ਵਾਪਸ ਸਥਿਤੀ;
}

/*
 * Unlet item in list or dict variable.
 */
    ਸਥਿਰ ਪੂਰਨ
execute_unletindex(isn_T *iptr, ectx_T *ectx)
{
    typval_T	*tv_idx = ਢੇਰ_TV_BOT(-2);
    typval_T	*tv_dest = ਢੇਰ_TV_BOT(-1);
    ਪੂਰਨ		ਸਥਿਤੀ = OK;

    // Stack contains:
    // -2 index
    // -1 dict or list
    SOURCING_LNUM = iptr->isn_lnum;
    ਜੇ (tv_dest->v_type == VAR_DICT)
    {
	// unlet a dict item, index must be a string
	ਜੇ (tv_idx->v_type != VAR_STRING && tv_idx->v_type != VAR_ਸੰਖਿਆ)
	{
	    semsg(_(e_expected_str_but_got_str),
			vartype_name(VAR_STRING),
			vartype_name(tv_idx->v_type));
	    ਸਥਿਤੀ = FAIL;
	}
	ਨਹੀਂਤਾਂ
	{
	    dict_T	*d = tv_dest->vval.v_dict;
	    char_u	*ਕੁੰਜੀ;
	    dictitem_T  *di = ਨਿਰਾ;

	    ਜੇ (d != ਨਿਰਾ && ਮੁੱਲ_check_ਤਾਲਾ(
				      d->dv_ਤਾਲਾ, ਨਿਰਾ, FALSE))
		ਸਥਿਤੀ = FAIL;
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (tv_idx->v_type == VAR_STRING)
		{
		    ਕੁੰਜੀ = tv_idx->vval.v_string;
		    ਜੇ (ਕੁੰਜੀ == ਨਿਰਾ)
			ਕੁੰਜੀ = (char_u *)"";
		}
		ਨਹੀਂਤਾਂ
		{
		    ਕੁੰਜੀ = tv_get_string(tv_idx);
		}
		ਜੇ (d != ਨਿਰਾ)
		    di = dict_ਲੱਭੋ(d, ਕੁੰਜੀ, (ਪੂਰਨ)STRLEN(ਕੁੰਜੀ));
		ਜੇ (di == ਨਿਰਾ)
		{
		    // NULL dict is equivalent to empty dict
		    semsg(_(e_ਕੁੰਜੀ_not_present_ਵਿੱਚ_dictionary_str), ਕੁੰਜੀ);
		    ਸਥਿਤੀ = FAIL;
		}
		ਨਹੀਂਤਾਂ ਜੇ (var_check_fixed(di->di_flags,
						   ਨਿਰਾ, FALSE)
			|| var_check_ro(di->di_flags,
						  ਨਿਰਾ, FALSE))
		    ਸਥਿਤੀ = FAIL;
		ਨਹੀਂਤਾਂ
		    dictitem_remove(d, di, "unlet");
	    }
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (tv_dest->v_type == VAR_LIST)
    {
	// unlet a List item, index must be a number
	ਜੇ (check_for_ਸੰਖਿਆ(tv_idx) == FAIL)
	{
	    ਸਥਿਤੀ = FAIL;
	}
	ਨਹੀਂਤਾਂ
	{
	    list_T	*l = tv_dest->vval.v_list;
	    ਲੰਮਾ	n = (ਲੰਮਾ)tv_idx->vval.v_ਸੰਖਿਆ;

	    ਜੇ (l != ਨਿਰਾ && ਮੁੱਲ_check_ਤਾਲਾ(
				      l->lv_ਤਾਲਾ, ਨਿਰਾ, FALSE))
		ਸਥਿਤੀ = FAIL;
	    ਨਹੀਂਤਾਂ
	    {
		listitem_T	*li = list_ਲੱਭੋ(l, n);

		ਜੇ (li == ਨਿਰਾ)
		{
		    semsg(_(e_list_ਸੂਚਕਾਂਕ_ਬਾਹਰਲਾ_ਦਾ_range_nr), n);
		    ਸਥਿਤੀ = FAIL;
		}
		ਨਹੀਂਤਾਂ
		    listitem_remove(l, li);
	    }
	}
    }
    ਨਹੀਂਤਾਂ
    {
	ਸਥਿਤੀ = FAIL;
	semsg(_(e_cannot_ਸੂਚਕਾਂਕ_str),
				vartype_name(tv_dest->v_type));
    }

    clear_tv(tv_idx);
    clear_tv(tv_dest);
    ectx->ec_ਢੇਰ.ga_len -= 2;

    ਵਾਪਸ ਸਥਿਤੀ;
}

/*
 * Unlet a range of items in a list variable.
 */
    ਸਥਿਰ ਪੂਰਨ
execute_unletrange(isn_T *iptr, ectx_T *ectx)
{
    // Stack contains:
    // -3 index1
    // -2 index2
    // -1 dict or list
    typval_T	*tv_idx1 = ਢੇਰ_TV_BOT(-3);
    typval_T	*tv_idx2 = ਢੇਰ_TV_BOT(-2);
    typval_T	*tv_dest = ਢੇਰ_TV_BOT(-1);
    ਪੂਰਨ		ਸਥਿਤੀ = OK;

    ਜੇ (tv_dest->v_type == VAR_LIST)
    {
	// indexes must be a number
	SOURCING_LNUM = iptr->isn_lnum;
	ਜੇ (check_for_ਸੰਖਿਆ(tv_idx1) == FAIL
		|| (tv_idx2->v_type != VAR_SPECIAL
			 && check_for_ਸੰਖਿਆ(tv_idx2) == FAIL))
	{
	    ਸਥਿਤੀ = FAIL;
	}
	ਨਹੀਂਤਾਂ
	{
	    list_T	*l = tv_dest->vval.v_list;
	    ਲੰਮਾ	n1 = (ਲੰਮਾ)tv_idx1->vval.v_ਸੰਖਿਆ;
	    ਲੰਮਾ	n2 = tv_idx2->v_type == VAR_SPECIAL
			    ? 0 : (ਲੰਮਾ)tv_idx2->vval.v_ਸੰਖਿਆ;
	    listitem_T	*li;

	    li = list_ਲੱਭੋ_ਸੂਚਕਾਂਕ(l, &n1);
	    ਜੇ (li == ਨਿਰਾ)
	    {
		semsg(_(e_list_ਸੂਚਕਾਂਕ_ਬਾਹਰਲਾ_ਦਾ_range_nr),
						 (ਲੰਮਾ)tv_idx1->vval.v_ਸੰਖਿਆ);
		ਸਥਿਤੀ = FAIL;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (n1 < 0)
		    n1 = list_idx_ਦਾ_ਵਸਤੂ(l, li);
		ਜੇ (n2 < 0)
		{
		    listitem_T *li2 = list_ਲੱਭੋ(l, n2);

		    ਜੇ (li2 == ਨਿਰਾ)
		    {
			semsg(_(e_list_ਸੂਚਕਾਂਕ_ਬਾਹਰਲਾ_ਦਾ_range_nr), n2);
			ਸਥਿਤੀ = FAIL;
		    }
		    ਨਹੀਂਤਾਂ
			n2 = list_idx_ਦਾ_ਵਸਤੂ(l, li2);
		}
		ਜੇ (ਸਥਿਤੀ != FAIL
			&& tv_idx2->v_type != VAR_SPECIAL
			&& n2 < n1)
		{
		    semsg(_(e_list_ਸੂਚਕਾਂਕ_ਬਾਹਰਲਾ_ਦਾ_range_nr), n2);
		    ਸਥਿਤੀ = FAIL;
		}
		ਜੇ (ਸਥਿਤੀ != FAIL)
		    list_unlet_range(l, li, n1,
					   tv_idx2->v_type != VAR_SPECIAL, n2);
	    }
	}
    }
    ਨਹੀਂਤਾਂ
    {
	ਸਥਿਤੀ = FAIL;
	SOURCING_LNUM = iptr->isn_lnum;
	semsg(_(e_cannot_ਸੂਚਕਾਂਕ_str),
				vartype_name(tv_dest->v_type));
    }

    clear_tv(tv_idx1);
    clear_tv(tv_idx2);
    clear_tv(tv_dest);
    ectx->ec_ਢੇਰ.ga_len -= 3;

    ਵਾਪਸ ਸਥਿਤੀ;
}

/*
 * Top of a for loop.
 */
    ਸਥਿਰ ਪੂਰਨ
execute_for(isn_T *iptr, ectx_T *ectx)
{
    typval_T	*tv;
    ਪੂਰਨ		jump = FALSE;
    typval_T	*ltv = ਢੇਰ_TV_BOT(-1);
    typval_T	*idxtv =
		   ਢੇਰ_TV_VAR(iptr->isn_arg.forloop.for_loop_idx);

    ਜੇ (GA_GROW_FAILS(&ectx->ec_ਢੇਰ, 1))
	ਵਾਪਸ FAIL;
    ਜੇ (ltv->v_type == VAR_LIST)
    {
	list_T *list = ltv->vval.v_list;

	// push the next item from the list
	++idxtv->vval.v_ਸੰਖਿਆ;
	ਜੇ (list == ਨਿਰਾ
		       || idxtv->vval.v_ਸੰਖਿਆ >= list->lv_len)
	{
	    jump = TRUE;
	}
	ਨਹੀਂਤਾਂ ਜੇ (list->lv_ਪਹਿਲਾ == &range_list_ਵਸਤੂ)
	{
	    // non-materialized range() list
	    tv = ਢੇਰ_TV_BOT(0);
	    tv->v_type = VAR_ਸੰਖਿਆ;
	    tv->v_ਤਾਲਾ = 0;
	    tv->vval.v_ਸੰਖਿਆ = list_ਲੱਭੋ_nr(
			     list, idxtv->vval.v_ਸੰਖਿਆ, ਨਿਰਾ);
	    ++ectx->ec_ਢੇਰ.ga_len;
	}
	ਨਹੀਂਤਾਂ
	{
	    listitem_T *li = list_ਲੱਭੋ(list,
					 idxtv->vval.v_ਸੰਖਿਆ);
	    ਜੇ (li == ਨਿਰਾ)
		ਵਾਪਸ FAIL;
	    copy_tv(&li->li_tv, ਢੇਰ_TV_BOT(0));
	    ++ectx->ec_ਢੇਰ.ga_len;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (ltv->v_type == VAR_TUPLE)
    {
	tuple_T *tuple = ltv->vval.v_tuple;

	// push the next item from the tuple
	++idxtv->vval.v_ਸੰਖਿਆ;
	ਜੇ (tuple == ਨਿਰਾ || idxtv->vval.v_ਸੰਖਿਆ >= TUPLE_LEN(tuple))
	    jump = TRUE;
	ਨਹੀਂਤਾਂ
	{
	    copy_tv(TUPLE_ਵਸਤੂ(tuple, idxtv->vval.v_ਸੰਖਿਆ), ਢੇਰ_TV_BOT(0));
	    ++ectx->ec_ਢੇਰ.ga_len;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (ltv->v_type == VAR_STRING)
    {
	char_u	*str = ltv->vval.v_string;

	// The index is for the last byte of the previous
	// character.
	++idxtv->vval.v_ਸੰਖਿਆ;
	ਜੇ (str == ਨਿਰਾ || str[idxtv->vval.v_ਸੰਖਿਆ] == NUL)
	{
	    jump = TRUE;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਪੂਰਨ	clen = mb_ptr2len(str + idxtv->vval.v_ਸੰਖਿਆ);

	    // Push the next character from the string.
	    tv = ਢੇਰ_TV_BOT(0);
	    tv->v_type = VAR_STRING;
	    tv->vval.v_string = vim_strnsave(
			     str + idxtv->vval.v_ਸੰਖਿਆ, clen);
	    ++ectx->ec_ਢੇਰ.ga_len;
	    idxtv->vval.v_ਸੰਖਿਆ += clen - 1;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (ltv->v_type == VAR_BLOB)
    {
	blob_T	*blob = ltv->vval.v_blob;

	// When we get here the first time make a copy of the
	// blob, so that the iteration still works when it is
	// changed.
	ਜੇ (idxtv->vval.v_ਸੰਖਿਆ == -1 && blob != ਨਿਰਾ)
	{
	    ltv->v_ਤਾਲਾ = 0;
	    ltv->vval.v_blob = blob_copy(blob);
	    blob_unref(blob);
	    blob = ltv->vval.v_blob;
	}

	// The index is for the previous byte.
	++idxtv->vval.v_ਸੰਖਿਆ;
	ਜੇ (blob == ਨਿਰਾ || idxtv->vval.v_ਸੰਖਿਆ >= blob_len(blob))
	{
	    jump = TRUE;
	}
	ਨਹੀਂਤਾਂ
	{
	    // Push the next byte from the blob.
	    tv = ਢੇਰ_TV_BOT(0);
	    tv->v_type = VAR_ਸੰਖਿਆ;
	    tv->vval.v_ਸੰਖਿਆ = blob_get(blob,
					 idxtv->vval.v_ਸੰਖਿਆ);
	    ++ectx->ec_ਢੇਰ.ga_len;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	semsg(_(e_for_loop_on_str_not_supported),
				    vartype_name(ltv->v_type));
	ਵਾਪਸ FAIL;
    }

    ਜੇ (jump)
    {
	// past the end of the list/string/blob, jump to "endfor"
	ectx->ec_iidx = iptr->isn_arg.forloop.for_ਸਿਰਾ;
	may_restore_cmdmod(&ectx->ec_funclocal);
    }
    ਨਹੀਂਤਾਂ
    {
	// Store the current number of funcrefs, this may be used in
	// ISN_LOOPEND.  The variable index is always one more than the loop
	// variable index.
	tv = ਢੇਰ_TV_VAR(iptr->isn_arg.forloop.for_loop_idx + 1);
	tv->vval.v_ਸੰਖਿਆ = ectx->ec_funcrefs.ga_len;
    }

    ਵਾਪਸ OK;
}

/*
 * Code for handling variables declared inside a loop and used in a closure.
 * This is very similar to what is done with funcstack_T.  The difference is
 * that the funcstack_T has the scope of a function, while a loopvars_T has the
 * scope of the block inside a loop and each loop may have its own.
 */

// Double linked list of loopvars_T in use.
ਸਥਿਰ loopvars_T *ਪਹਿਲਾ_loopvars = ਨਿਰਾ;

    ਸਥਿਰ ਖਾਲੀ
ਜੋੜੋ_loopvars_ਮੰਜ਼ਿਲ_list(loopvars_T *loopvars)
{
	// Link in list of loopvarss.
    ਜੇ (ਪਹਿਲਾ_loopvars != ਨਿਰਾ)
	ਪਹਿਲਾ_loopvars->lvs_prev = loopvars;
    loopvars->lvs_ਅਗਲਾ = ਪਹਿਲਾ_loopvars;
    loopvars->lvs_prev = ਨਿਰਾ;
    ਪਹਿਲਾ_loopvars = loopvars;
}

    ਸਥਿਰ ਖਾਲੀ
remove_loopvars_from_list(loopvars_T *loopvars)
{
    ਜੇ (loopvars->lvs_prev == ਨਿਰਾ)
	ਪਹਿਲਾ_loopvars = loopvars->lvs_ਅਗਲਾ;
    ਨਹੀਂਤਾਂ
	loopvars->lvs_prev->lvs_ਅਗਲਾ = loopvars->lvs_ਅਗਲਾ;
    ਜੇ (loopvars->lvs_ਅਗਲਾ != ਨਿਰਾ)
	loopvars->lvs_ਅਗਲਾ->lvs_prev = loopvars->lvs_prev;
}

/*
 * End of a for or while loop: Handle any variables used by a closure.
 */
    ਸਥਿਰ ਪੂਰਨ
execute_endloop(isn_T *iptr, ectx_T *ectx)
{
    endloop_T	*endloop = &iptr->isn_arg.endloop;
    typval_T	*tv_refcount = ਢੇਰ_TV_VAR(endloop->ਸਿਰਾ_funcref_idx);
    ਪੂਰਨ		prev_ਘੇਰਾ_ਗਿਣਤੀ = tv_refcount->vval.v_ਸੰਖਿਆ;
    ਪੂਰਨ		ਡੂੰਘਾਈ = endloop->ਸਿਰਾ_ਡੂੰਘਾਈ;
    garray_T	*gap = &ectx->ec_funcrefs;
    ਪੂਰਨ		ਘੇਰਾ_ਵਿੱਚ_use = FALSE;
    loopvars_T  *loopvars;
    typval_T    *ਢੇਰ;
    ਪੂਰਨ		idx;

    // Check if any created closure is still being referenced and loopvars have
    // not been saved yet for the current depth.
    ਲਈ (idx = prev_ਘੇਰਾ_ਗਿਣਤੀ; idx < gap->ga_len; ++idx)
    {
	partial_T   *pt = ((partial_T **)gap->ga_ਅੰਕੜੇ)[idx];

	ਜੇ (pt->pt_refcount > 1 && pt->pt_loopvars[ਡੂੰਘਾਈ] == ਨਿਰਾ)
	{
	    ਪੂਰਨ refcount = pt->pt_refcount;
	    ਪੂਰਨ i;

	    // A Reference in a variable inside the loop doesn't count, it gets
	    // unreferenced at the end of the loop.
	    ਲਈ (i = 0; i < endloop->ਸਿਰਾ_var_ਗਿਣਤੀ; ++i)
	    {
		typval_T *stv = ਢੇਰ_TV_VAR(endloop->ਸਿਰਾ_var_idx + i);

		ਜੇ (stv->v_type == VAR_PARTIAL && pt == stv->vval.v_partial)
		    --refcount;
	    }
	    ਜੇ (refcount > 1)
	    {
		ਘੇਰਾ_ਵਿੱਚ_use = TRUE;
		ਰੋਕੋ;
	    }
	}
    }

    // If no function reference were created since the start of the loop block
    // or it is no longer referenced there is nothing to do.
    ਜੇ (!ਘੇਰਾ_ਵਿੱਚ_use)
	ਵਾਪਸ OK;

    // A closure is using variables declared inside the loop.
    // Move them to the called function.
    loopvars = ALLOC_CLEAR_ONE(loopvars_T);
    ਜੇ (loopvars == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    loopvars->lvs_ga.ga_len = endloop->ਸਿਰਾ_var_ਗਿਣਤੀ;
    ਢੇਰ = ALLOC_CLEAR_MULT(typval_T, loopvars->lvs_ga.ga_len);
    loopvars->lvs_ga.ga_ਅੰਕੜੇ = ਢੇਰ;
    ਜੇ (ਢੇਰ == ਨਿਰਾ)
    {
	vim_free(loopvars);
	ਵਾਪਸ FAIL;
    }
    ਜੋੜੋ_loopvars_ਮੰਜ਼ਿਲ_list(loopvars);

    // Move the variable values.
    ਲਈ (idx = 0; idx < endloop->ਸਿਰਾ_var_ਗਿਣਤੀ; ++idx)
    {
	typval_T *tv = ਢੇਰ_TV_VAR(endloop->ਸਿਰਾ_var_idx + idx);

	*(ਢੇਰ + idx) = *tv;
	tv->v_type = VAR_UNKNOWN;
    }

    ਲਈ (idx = prev_ਘੇਰਾ_ਗਿਣਤੀ; idx < gap->ga_len; ++idx)
    {
	partial_T   *pt = ((partial_T **)gap->ga_ਅੰਕੜੇ)[idx];

	ਜੇ (pt->pt_refcount > 1 && pt->pt_loopvars[ਡੂੰਘਾਈ] == ਨਿਰਾ)
	{
	    ++loopvars->lvs_refcount;
	    pt->pt_loopvars[ਡੂੰਘਾਈ] = loopvars;

	    pt->pt_outer.ਬਾਹਰਲਾ_loop[ਡੂੰਘਾਈ].ਢੇਰ = &loopvars->lvs_ga;
	    pt->pt_outer.ਬਾਹਰਲਾ_loop[ਡੂੰਘਾਈ].var_idx -=
		  ectx->ec_ਚੌਖਟਾ_idx + ਢੇਰ_ਚੌਖਟਾ_ਮਾਪ + endloop->ਸਿਰਾ_var_idx;
	}
    }

    ਵਾਪਸ OK;
}

/*
 * Called when a partial is freed or its reference count goes down to one.  The
 * loopvars may be the only reference to the partials in the local variables.
 * Go over all of them, the funcref and can be freed if all partials
 * referencing the loopvars have a reference count of one.
 * Return TRUE if it was freed.
 */
    ਪੂਰਨ
loopvars_check_refcount(loopvars_T *loopvars)
{
    ਪੂਰਨ		    i;
    garray_T	    *gap = &loopvars->lvs_ga;
    ਪੂਰਨ		    ਪੂਰਾ = 0;
	typval_T	*ਢੇਰ = gap->ga_ਅੰਕੜੇ;

    ਜੇ (loopvars->lvs_refcount > loopvars->lvs_min_refcount)
	ਵਾਪਸ FALSE;
    ਲਈ (i = 0; i < gap->ga_len; ++i)
    {
	typval_T    *tv = ((typval_T *)gap->ga_ਅੰਕੜੇ) + i;

	ਜੇ (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != ਨਿਰਾ
		&& tv->vval.v_partial->pt_refcount == 1)
	{
	    ਪੂਰਨ	    ਡੂੰਘਾਈ;

	    ਲਈ (ਡੂੰਘਾਈ = 0; ਡੂੰਘਾਈ < MAX_LOOP_ਡੂੰਘਾਈ; ++ਡੂੰਘਾਈ)
		ਜੇ (tv->vval.v_partial->pt_loopvars[ਡੂੰਘਾਈ] == loopvars)
		    ++ਪੂਰਾ;
	}
    }
    ਜੇ (ਪੂਰਾ != loopvars->lvs_min_refcount)
	ਵਾਪਸ FALSE;

    // All partials referencing the loopvars have a reference count of
    // one, thus the loopvars is no longer of use.
    ਢੇਰ = gap->ga_ਅੰਕੜੇ;
    ਲਈ (i = 0; i < gap->ga_len; ++i)
	clear_tv(ਢੇਰ + i);
    vim_free(ਢੇਰ);
    remove_loopvars_from_list(loopvars);
    vim_free(loopvars);
    ਵਾਪਸ TRUE;
}

/*
 * For garbage collecting: set references in all variables referenced by
 * all loopvars.
 */
    ਪੂਰਨ
ਸੈੱਟ_ref_ਵਿੱਚ_loopvars(ਪੂਰਨ copyID)
{
    loopvars_T *loopvars;

    ਲਈ (loopvars = ਪਹਿਲਾ_loopvars; loopvars != ਨਿਰਾ;
						 loopvars = loopvars->lvs_ਅਗਲਾ)
    {
	typval_T    *ਢੇਰ = loopvars->lvs_ga.ga_ਅੰਕੜੇ;
	ਪੂਰਨ	    i;

	ਲਈ (i = 0; i < loopvars->lvs_ga.ga_len; ++i)
	    ਜੇ (ਸੈੱਟ_ref_ਵਿੱਚ_ਵਸਤੂ(ਢੇਰ + i, copyID, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ))
		ਵਾਪਸ TRUE;  // abort
    }
    ਵਾਪਸ FALSE;
}

/*
 * Load instruction for w:/b:/g:/t: variable.
 * "isn_type" is used instead of "iptr->isn_type".
 */
    ਸਥਿਰ ਪੂਰਨ
ਲੱਦੋ_namespace_var(ectx_T *ectx, isntype_T isn_type, isn_T *iptr)
{
    dictitem_T	*di = ਨਿਰਾ;
    hashtab_T	*ht = ਨਿਰਾ;
    ਅੱਖਰ	namespace;

    ਜੇ (GA_GROW_FAILS(&ectx->ec_ਢੇਰ, 1))
	ਵਾਪਸ NOTDONE;
    ਚੋਣ (isn_type)
    {
	ਹਾਲਤ ISN_LOADG:
	    ht = get_globvar_ht();
	    namespace = 'g';
	    ਰੋਕੋ;
	ਹਾਲਤ ISN_LOADB:
	    ht = &curbuf->b_vars->dv_hashtab;
	    namespace = 'b';
	    ਰੋਕੋ;
	ਹਾਲਤ ISN_LOADW:
	    ht = &curwin->w_vars->dv_hashtab;
	    namespace = 'w';
	    ਰੋਕੋ;
	ਹਾਲਤ ISN_LOADT:
	    ht = &curtab->tp_vars->dv_hashtab;
	    namespace = 't';
	    ਰੋਕੋ;
	ਮੂਲ:  // Cannot reach here
	    ਵਾਪਸ NOTDONE;
    }
    di = ਲੱਭੋ_var_ਵਿੱਚ_ht(ht, 0, iptr->isn_arg.string, TRUE);

    ਜੇ (di == ਨਿਰਾ)
    {
	ਜੇ (isn_type == ISN_LOADG)
	{
	    ufunc_T *ufunc = ਲੱਭੋ_func(iptr->isn_arg.string, TRUE);

	    // g:Something could be a function
	    ਜੇ (ufunc != ਨਿਰਾ)
	    {
		typval_T    *tv = ਢੇਰ_TV_BOT(0);

		++ectx->ec_ਢੇਰ.ga_len;
		tv->v_type = VAR_FUNC;
		tv->vval.v_string = alloc(STRLEN(iptr->isn_arg.string) + 3);
		ਜੇ (tv->vval.v_string == ਨਿਰਾ)
		    ਵਾਪਸ FAIL;
		STRCPY(tv->vval.v_string, "g:");
		STRCPY(tv->vval.v_string + 2, iptr->isn_arg.string);
		ਵਾਪਸ OK;
	    }
	}
	SOURCING_LNUM = iptr->isn_lnum;
	ਜੇ (vim_strchr(iptr->isn_arg.string, AUTOLOAD_CHAR) != ਨਿਰਾ)
	    // no check if the item exists in the script but
	    // isn't exported, it is too complicated
	    semsg(_(e_ਵਸਤੂ_not_ਮਿਲਿਆ_ਵਿੱਚ_script_str), iptr->isn_arg.string);
	ਨਹੀਂਤਾਂ
	    semsg(_(e_undefined_variable_char_str),
					      namespace, iptr->isn_arg.string);
	// Push a value anyway, the stack must have the expected size when the
	// error is ignored because of ":silent!" in a calling function.
	typval_T *tv = ਢੇਰ_TV_BOT(0);
	tv->v_type = VAR_ਸੰਖਿਆ;
	tv->v_ਤਾਲਾ = 0;
	tv->vval.v_ਸੰਖਿਆ = 0;
	++ectx->ec_ਢੇਰ.ga_len;
	ਵਾਪਸ FAIL;
    }
    ਨਹੀਂਤਾਂ
    {
	copy_tv(&di->di_tv, ਢੇਰ_TV_BOT(0));
	++ectx->ec_ਢੇਰ.ga_len;
    }
    ਵਾਪਸ OK;
}


    ਸਥਿਰ ਖਾਲੀ
object_required_error(typval_T *tv)
{
    garray_T type_list;
    ga_init2(&type_list, ਆਕਾਰ(type_T *), 10);
    type_T *type = typval2type(tv, get_copyID(), &type_list, TVTT_DO_MEMBER);
    ਅੱਖਰ *tofree = ਨਿਰਾ;
    ਅੱਖਰ *typename = type_name(type, &tofree);
    semsg(_(e_object_required_ਮਿਲਿਆ_str), typename);
    vim_free(tofree);
    clear_type_list(&type_list);
}

/*
 * Accessing the variable or method of an object or a class stored in a
 * variable of type "any".
 * Returns OK if the member variable is present.
 * Returns FAIL if the variable is not found.
 */
    ਸਥਿਰ ਪੂਰਨ
var_any_get_oc_member(class_T *current_class, isn_T *iptr, typval_T *tv)
{
    ਪੂਰਨ		is_object = tv->v_type == VAR_OBJECT;
    class_T	*tv_cl;
    object_T	*obj = ਨਿਰਾ;
    typval_T	mtv;

    ਜੇ (is_object)
    {
	obj = tv->vval.v_object;
	ਜੇ (obj == ਨਿਰਾ)
	{
	    SOURCING_LNUM = iptr->isn_lnum;
	    emsg(_(e_using_null_object));
	    ਵਾਪਸ FAIL;
	}
	tv_cl = obj->obj_class;
    }
    ਨਹੀਂਤਾਂ
    {
	tv_cl = tv->vval.v_class;
	ਜੇ (tv_cl == ਨਿਰਾ)
	{
	    SOURCING_LNUM = iptr->isn_lnum;
	    emsg(_(e_using_null_class));
	    ਵਾਪਸ FAIL;
	}
    }

    // get_member_tv() needs the class/object information in the typval
    // argument.  So set the object information.
    copy_tv(tv, &mtv);

    // 'name' can either be an instance or class variable or method
    ਪੂਰਨ		namelen = (ਪੂਰਨ)STRLEN(iptr->isn_arg.string);
    ਪੂਰਨ		save_did_emsg = did_emsg;

    ਜੇ (get_member_tv(tv_cl, is_object, iptr->isn_arg.string, namelen,
						current_class, &mtv) == OK)
    {
	// instance or class variable
	copy_tv(&mtv, tv);
	clear_tv(&mtv);
	ਵਾਪਸ OK;
    }

    ਜੇ (did_emsg != save_did_emsg)
	ਵਾਪਸ FAIL;

    // could be a class or instance method
    ufunc_T	*oc_ਵਿਧੀ;
    ਪੂਰਨ		oc_ਵਿਧੀ_idx;

    oc_ਵਿਧੀ = ਵਿਧੀ_lookup(tv_cl, tv->v_type, iptr->isn_arg.string,
						namelen, &oc_ਵਿਧੀ_idx);
    ਜੇ (oc_ਵਿਧੀ == ਨਿਰਾ)
    {
	ਅੱਖਰ	*ਸੁਨੇਹਾ;

	SOURCING_LNUM = iptr->isn_lnum;
	ਜੇ (is_object)
	    ਸੁਨੇਹਾ = e_variable_not_ਮਿਲਿਆ_on_object_str_str;
	ਨਹੀਂਤਾਂ
	    ਸੁਨੇਹਾ = e_class_variable_str_not_ਮਿਲਿਆ_ਵਿੱਚ_class_str;
	semsg(_(ਸੁਨੇਹਾ), iptr->isn_arg.string, tv_cl->class_name.string);
	ਵਾਪਸ FAIL;
    }

    // Protected methods are not accessible outside the class
    ਜੇ (*oc_ਵਿਧੀ->uf_name == '_'
			&& !class_instance_ਦਾ(current_class, tv_cl))
    {
	semsg(_(e_cannot_access_protected_ਵਿਧੀ_str), oc_ਵਿਧੀ->uf_name);
	ਵਾਪਸ FAIL;
    }

    // Create a partial for the instance or class method
    ਜੇ (obj_ਵਿਧੀ_ਮੰਜ਼ਿਲ_partial_tv(is_object ? obj : ਨਿਰਾ, oc_ਵਿਧੀ, tv)
								== FAIL)
	ਵਾਪਸ FAIL;

    ਵਾਪਸ OK;
}

/*
 * do ISN_PUT or ISN_IPUT instruction depending on fixindent parameter
 */
    ਸਥਿਰ ਖਾਲੀ
isn_put_do(ectx_T *ectx, isn_T *iptr, typval_T *tv, ਪੂਰਨ fixindent)
{
    ਪੂਰਨ		regname = iptr->isn_arg.put.put_regname;
    linenr_T	lnum = iptr->isn_arg.put.put_lnum;
    char_u	*ਵਿਅੰਜਕ = ਨਿਰਾ;
    ਪੂਰਨ		dir = FORWARD;

    ਜੇ (lnum < -2)
    {
	// line number was put on the stack by ISN_RANGE
	tv = ਢੇਰ_TV_BOT(-1);
	curwin->w_cursor.lnum = tv->vval.v_ਸੰਖਿਆ;
	ਜੇ (lnum == LNUM_VARIABLE_RANGE_ABOVE)
	    dir = BACKWARD;
	--ectx->ec_ਢੇਰ.ga_len;
    }
    ਨਹੀਂਤਾਂ ਜੇ (lnum == -2)
	// :put! above cursor
	dir = BACKWARD;
    ਨਹੀਂਤਾਂ ਜੇ (lnum >= 0)
    {
	curwin->w_cursor.lnum = lnum;
	ਜੇ (lnum == 0)
	    // check_cursor() below will move to line 1
	    dir = BACKWARD;
    }

    ਜੇ (regname == '=')
    {
	tv = ਢੇਰ_TV_BOT(-1);
	ਜੇ (tv->v_type == VAR_STRING)
	    ਵਿਅੰਜਕ = tv->vval.v_string;
	ਨਹੀਂਤਾਂ
	{
	    ਵਿਅੰਜਕ = typval2string(tv, TRUE); // allocates value
	    clear_tv(tv);
	}
	--ectx->ec_ਢੇਰ.ga_len;
    }
    check_cursor();

    ਜੇ (fixindent)
	do_put(regname, ਵਿਅੰਜਕ, dir, 1L, PUT_ਸਤਰ|PUT_CURSLINE|PUT_FIXINDENT);
    ਨਹੀਂਤਾਂ
	do_put(regname, ਵਿਅੰਜਕ, dir, 1L, PUT_ਸਤਰ|PUT_CURSLINE);
    vim_free(ਵਿਅੰਜਕ);
}

/*
 * Execute the ISN_UNPACK instruction for a List
 */
    ਸਥਿਰ ਪੂਰਨ
exec_unpack_list(ectx_T *ectx, isn_T *iptr, typval_T *tv)
{
    ਪੂਰਨ		ਗਿਣਤੀ_2 = iptr->isn_arg.unpack.unp_ਗਿਣਤੀ;
    ਪੂਰਨ		semicolon = iptr->isn_arg.unpack.unp_semicolon;
    list_T	*l;
    listitem_T	*li;
    ਪੂਰਨ		i;

    l = tv->vval.v_list;
    ਜੇ (l == ਨਿਰਾ
	    || l->lv_len < (semicolon ? ਗਿਣਤੀ_2 - 1 : ਗਿਣਤੀ_2))
    {
	SOURCING_LNUM = iptr->isn_lnum;
	emsg(_(e_list_ਮੁੱਲ_does_not_have_enough_ਚੀਜ਼ਾਂ));
	ਵਾਪਸ EXEC_FAIL;
    }
    ਨਹੀਂਤਾਂ ਜੇ (!semicolon && l->lv_len > ਗਿਣਤੀ_2)
    {
	SOURCING_LNUM = iptr->isn_lnum;
	emsg(_(e_list_ਮੁੱਲ_has_ਹੋਰ_ਚੀਜ਼ਾਂ_than_targets));
	ਵਾਪਸ EXEC_FAIL;
    }

    CHECK_LIST_MATERIALIZE(l);
    ਜੇ (GA_GROW_FAILS(&ectx->ec_ਢੇਰ, ਗਿਣਤੀ_2 - 1))
	ਵਾਪਸ EXEC_ਪੂਰਾ;
    ectx->ec_ਢੇਰ.ga_len += ਗਿਣਤੀ_2 - 1;

    // Variable after semicolon gets a list with the remaining
    // items.
    ਜੇ (semicolon)
    {
	list_T	*rem_list =
	    list_alloc_with_ਚੀਜ਼ਾਂ(l->lv_len - ਗਿਣਤੀ_2 + 1);

	ਜੇ (rem_list == ਨਿਰਾ)
	    ਵਾਪਸ EXEC_ਪੂਰਾ;
	tv = ਢੇਰ_TV_BOT(-ਗਿਣਤੀ_2);
	tv->vval.v_list = rem_list;
	++rem_list->lv_refcount;
	tv->v_ਤਾਲਾ = 0;
	li = l->lv_ਪਹਿਲਾ;
	ਲਈ (i = 0; i < ਗਿਣਤੀ_2 - 1; ++i)
	    li = li->li_ਅਗਲਾ;
	ਲਈ (i = 0; li != ਨਿਰਾ; ++i)
	{
	    typval_T tvcopy;

	    copy_tv(&li->li_tv, &tvcopy);
	    list_ਸੈੱਟ_ਵਸਤੂ(rem_list, i, &tvcopy);
	    li = li->li_ਅਗਲਾ;
	}
	--ਗਿਣਤੀ_2;
    }

    // Produce the values in reverse order, first item last.
    li = l->lv_ਪਹਿਲਾ;
    ਲਈ (i = 0; i < ਗਿਣਤੀ_2; ++i)
    {
	tv = ਢੇਰ_TV_BOT(-i - 1);
	copy_tv(&li->li_tv, tv);
	li = li->li_ਅਗਲਾ;
    }

    list_unref(l);

    ਵਾਪਸ EXEC_OK;
}

/*
 * Execute the ISN_UNPACK instruction for a Tuple
 */
    ਸਥਿਰ ਪੂਰਨ
exec_unpack_tuple(ectx_T *ectx, isn_T *iptr, typval_T *tv)
{
    ਪੂਰਨ		ਗਿਣਤੀ_2 = iptr->isn_arg.unpack.unp_ਗਿਣਤੀ;
    ਪੂਰਨ		semicolon = iptr->isn_arg.unpack.unp_semicolon;
    tuple_T	*tuple;
    ਪੂਰਨ		i;

    tuple = tv->vval.v_tuple;
    ਜੇ (tuple == ਨਿਰਾ
	    || TUPLE_LEN(tuple) < (semicolon ? ਗਿਣਤੀ_2 - 1 : ਗਿਣਤੀ_2))
    {
	SOURCING_LNUM = iptr->isn_lnum;
	emsg(_(e_ਹੋਰ_targets_than_tuple_ਚੀਜ਼ਾਂ));
	ਵਾਪਸ EXEC_FAIL;
    }
    ਨਹੀਂਤਾਂ ਜੇ (!semicolon && TUPLE_LEN(tuple) > ਗਿਣਤੀ_2)
    {
	SOURCING_LNUM = iptr->isn_lnum;
	emsg(_(e_less_targets_than_tuple_ਚੀਜ਼ਾਂ));
	ਵਾਪਸ EXEC_FAIL;
    }

    ਜੇ (GA_GROW_FAILS(&ectx->ec_ਢੇਰ, ਗਿਣਤੀ_2 - 1))
	ਵਾਪਸ EXEC_ਪੂਰਾ;
    ectx->ec_ਢੇਰ.ga_len += ਗਿਣਤੀ_2 - 1;

    // Variable after semicolon gets a list with the remaining
    // items.
    ਜੇ (semicolon)
    {
	list_T	*rem_list =
	    list_alloc_with_ਚੀਜ਼ਾਂ(TUPLE_LEN(tuple) - ਗਿਣਤੀ_2 + 1);

	ਜੇ (rem_list == ਨਿਰਾ)
	    ਵਾਪਸ EXEC_ਪੂਰਾ;
	tv = ਢੇਰ_TV_BOT(-ਗਿਣਤੀ_2);
	tv->v_type = VAR_LIST;
	tv->vval.v_list = rem_list;
	++rem_list->lv_refcount;
	tv->v_ਤਾਲਾ = 0;
	ਲਈ (i = ਗਿਣਤੀ_2 - 1; i < TUPLE_LEN(tuple); ++i)
	{
	    typval_T tvcopy;

	    copy_tv(TUPLE_ਵਸਤੂ(tuple, i), &tvcopy);
	    list_ਸੈੱਟ_ਵਸਤੂ(rem_list, i - (ਗਿਣਤੀ_2 - 1), &tvcopy);
	}
	--ਗਿਣਤੀ_2;
    }

    // Produce the values in reverse order, first item last.
    ਲਈ (i = 0; i < ਗਿਣਤੀ_2; ++i)
    {
	tv = ਢੇਰ_TV_BOT(-i - 1);
	copy_tv(TUPLE_ਵਸਤੂ(tuple, i), tv);
    }

    tuple_unref(tuple);

    ਵਾਪਸ EXEC_OK;
}

/*
 * Execute instructions in execution context "ectx".
 * Return OK or FAIL;
 */
    ਸਥਿਰ ਪੂਰਨ
exec_instructions(ectx_T *ectx)
{
    ਪੂਰਨ		ret = FAIL;
    ਪੂਰਨ		save_trylevel_at_ਸ਼ੁਰੂ = ectx->ec_trylevel_at_ਸ਼ੁਰੂ;
    ਪੂਰਨ		dict_ਢੇਰ_len_at_ਸ਼ੁਰੂ = dict_ਢੇਰ.ga_len;

    // Start execution at the first instruction.
    ectx->ec_iidx = 0;

    // Only catch exceptions in this instruction list.
    ectx->ec_trylevel_at_ਸ਼ੁਰੂ = trylevel;

    ਲਈ (;;)
    {
	ਸਥਿਰ ਪੂਰਨ  breakcheck_ਗਿਣਤੀ = 0;  // using "static" makes it faster
	isn_T	    *iptr;
	typval_T    *tv;

	ਜੇ (unlikely(++breakcheck_ਗਿਣਤੀ >= 100))
	{
	    ਸਤਰ_breakcheck();
	    breakcheck_ਗਿਣਤੀ = 0;
	}
	ਜੇ (unlikely(got_int))
	{
	    // Turn CTRL-C into an exception.
	    got_int = FALSE;
	    ਜੇ (throw_exception("Vim:Interrupt", ET_ਰੁਕਾਵਟ, ਨਿਰਾ) == FAIL)
		ਜਾਓ theend;
	    did_throw = TRUE;
	}

	ਜੇ (unlikely(did_emsg && ਸੁਨੇਹਾ_list != ਨਿਰਾ && *ਸੁਨੇਹਾ_list != ਨਿਰਾ))
	{
	    // Turn an error message into an exception.
	    did_emsg = FALSE;
	    ਜੇ (throw_exception(*ਸੁਨੇਹਾ_list, ET_ERROR, ਨਿਰਾ) == FAIL)
		ਜਾਓ theend;
	    did_throw = TRUE;
	    *ਸੁਨੇਹਾ_list = ਨਿਰਾ;

	    // This exception was not caught (yet).
	    garray_T	*trystack = &ectx->ec_trystack;
	    ਜੇ (trystack->ga_len > 0)
	    {
		trycmd_T *trycmd = ((trycmd_T *)trystack->ga_ਅੰਕੜੇ)
							+ trystack->ga_len - 1;
		ਜੇ (trycmd->tcd_ਚੌਖਟਾ_idx == ectx->ec_ਚੌਖਟਾ_idx)
		{
		    ਜੇ (trycmd->tcd_caught)
		    {
			// Inside a "catch" we need to first discard the caught
			// exception.
			ਅੰਤ_exception(caught_ਢੇਰ);
			trycmd->tcd_caught = FALSE;
		    }
		}
	    }
	}

	ਜੇ (unlikely(did_throw))
	{
	    garray_T	*trystack = &ectx->ec_trystack;
	    trycmd_T    *trycmd = ਨਿਰਾ;
	    ਪੂਰਨ		ਸੂਚਕਾਂਕ = trystack->ga_len;

	    // An exception jumps to the first catch, finally, or returns from
	    // the current function.
	    ਜਦੋਂ (ਸੂਚਕਾਂਕ > 0)
	    {
		trycmd = ((trycmd_T *)trystack->ga_ਅੰਕੜੇ) + ਸੂਚਕਾਂਕ - 1;
		// 1. after :try and before :catch - jump to first :catch
		// 2. in :catch block - jump to :finally
		// 3. in :catch block and no finally - jump to :endtry
		ਜੇ (!trycmd->tcd_ਵਿੱਚ_catch || trycmd->tcd_finally_idx != 0
				|| trycmd->tcd_ਚੌਖਟਾ_idx == ectx->ec_ਚੌਖਟਾ_idx)
		    ਰੋਕੋ;
		// In the catch and finally block of this try we have to go up
		// one level.
		--ਸੂਚਕਾਂਕ;
		trycmd = ਨਿਰਾ;
	    }
	    ਜੇ (trycmd != ਨਿਰਾ && trycmd->tcd_ਚੌਖਟਾ_idx == ectx->ec_ਚੌਖਟਾ_idx)
	    {
		ਜੇ (trycmd->tcd_ਵਿੱਚ_catch)
		{
		    ਜੇ (trycmd->tcd_finally_idx > 0)
		    {
			// exception inside ":catch", jump to ":finally" once
			ectx->ec_iidx = trycmd->tcd_finally_idx;
			trycmd->tcd_finally_idx = 0;
		    }
		    ਨਹੀਂਤਾਂ
		    {
			// exception inside ":catch" or ":finally", jump to
			// ":endtry"
			ectx->ec_iidx = trycmd->tcd_endtry_idx;
		    }
		}
		ਨਹੀਂਤਾਂ
		{
		    // jump to first ":catch"
		    ectx->ec_iidx = trycmd->tcd_catch_idx;
		    trycmd->tcd_ਵਿੱਚ_catch = TRUE;
		}
		did_throw = FALSE;  // don't come back here until :endtry
		trycmd->tcd_did_throw = TRUE;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// Not inside try or need to return from current function.
		// Push a dummy return value.
		ਜੇ (GA_GROW_FAILS(&ectx->ec_ਢੇਰ, 1))
		    ਜਾਓ theend;
		tv = ਢੇਰ_TV_BOT(0);
		tv->v_type = VAR_ਸੰਖਿਆ;
		tv->vval.v_ਸੰਖਿਆ = 0;
		++ectx->ec_ਢੇਰ.ga_len;
		ਜੇ (ectx->ec_ਚੌਖਟਾ_idx == ectx->ec_initial_ਚੌਖਟਾ_idx)
		{
		    // At the toplevel we are done.
		    need_rethrow = TRUE;
		    ਜੇ (handle_ਘੇਰਾ_ਵਿੱਚ_use(ectx, FALSE) == FAIL)
			ਜਾਓ theend;
		    ਜਾਓ ਪੂਰਾ;
		}

		ਜੇ (func_return(ectx) == FAIL)
		    ਜਾਓ theend;
	    }
	    ਜਾਰੀ;
	}

	/*
	 * Big switch on the instruction.  Most compilers will be turning this
	 * into an efficient lookup table, since the "case" values are an enum
	 * with sequential numbers.  It may look ugly, but it should be the
	 * most efficient way.
	 */
	iptr = &ectx->ec_instr[ectx->ec_iidx++];
	ਚੋਣ (iptr->isn_type)
	{
	    // Constructor, first instruction in a new() method.
	    ਹਾਲਤ ISN_CONSTRUCT:
		// "this" is always the local variable at index zero
		tv = ਢੇਰ_TV_VAR(0);
		tv->v_type = VAR_OBJECT;
		tv->vval.v_object =
		    alloc_object(iptr->isn_arg.construct.construct_class);
		ਰੋਕੋ;

	    // execute Ex command line
	    ਹਾਲਤ ISN_EXEC:
		ਜੇ (exec_command(iptr, iptr->isn_arg.string) == FAIL)
		    ਜਾਓ on_error;
		ਰੋਕੋ;

	    // execute Ex command line split at NL characters.
	    ਹਾਲਤ ISN_EXEC_ਵੰਡੋ:
		{
		    ਸਰੋਤ_cookie_T cookie;
		    char_u	    *ਸਤਰ;

		    SOURCING_LNUM = iptr->isn_lnum;
		    CLEAR_ਖੇਤਰ(cookie);
		    cookie.sourcing_lnum = iptr->isn_lnum - 1;
		    cookie.nextline = iptr->isn_arg.string;
		    ਸਤਰ = get_ਵੰਡੋ_sourceline(0, &cookie, 0, 0);
		    ਜੇ (do_cmdline(ਸਤਰ,
				get_ਵੰਡੋ_sourceline, &cookie,
				   DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED)
									== FAIL
				|| did_emsg)
		    {
			vim_free(ਸਤਰ);
			ਜਾਓ on_error;
		    }
		    vim_free(ਸਤਰ);
		}
		ਰੋਕੋ;

	    // execute Ex command line that is only a range
	    ਹਾਲਤ ISN_EXECRANGE:
		{
		    exarg_T	ea;
		    ਅੱਖਰ	*error = ਨਿਰਾ;

		    CLEAR_ਖੇਤਰ(ea);
		    ea.cmdidx = CMD_ਮਾਪ;
		    ea.addr_type = ADDR_ਸਤਰਾਂ;
		    ea.cmd = iptr->isn_arg.string;
		    SOURCING_LNUM = iptr->isn_lnum;
		    parse_cmd_ਪਤਾ(&ea, &error, FALSE);
		    ਜੇ (ea.cmd == ਨਿਰਾ)
			ਜਾਓ on_error;
		    // error is always NULL when using ADDR_LINES
		    error = ex_range_without_command(&ea);
		    ਜੇ (error != ਨਿਰਾ)
		    {
			emsg(error);
			ਜਾਓ on_error;
		    }
		}
		ਰੋਕੋ;

	    // Evaluate an expression with legacy syntax, push it onto the
	    // stack.
	    ਹਾਲਤ ISN_LEGACY_EVAL:
		{
		    char_u  *arg = iptr->isn_arg.string;
		    ਪੂਰਨ	    res;
		    ਪੂਰਨ	    save_flags = cmdmod.cmod_flags;

		    ਜੇ (GA_GROW_FAILS(&ectx->ec_ਢੇਰ, 1))
			ਜਾਓ theend;
		    tv = ਢੇਰ_TV_BOT(0);
		    init_tv(tv);
		    cmdmod.cmod_flags |= CMOD_LEGACY;
		    res = eval0(arg, tv, ਨਿਰਾ, &EVALARG_EVALUATE);
		    cmdmod.cmod_flags = save_flags;
		    ਜੇ (res == FAIL)
			ਜਾਓ on_error;
		    ++ectx->ec_ਢੇਰ.ga_len;
		}
		ਰੋਕੋ;

	    // push typeval VAR_INSTR with instructions to be executed
	    ਹਾਲਤ ISN_INSTR:
		{
		    ਜੇ (GA_GROW_FAILS(&ectx->ec_ਢੇਰ, 1))
			ਜਾਓ theend;
		    tv = ਢੇਰ_TV_BOT(0);
		    tv->vval.v_instr = ALLOC_ONE(instr_T);
		    ਜੇ (tv->vval.v_instr == ਨਿਰਾ)
			ਜਾਓ on_error;
		    ++ectx->ec_ਢੇਰ.ga_len;

		    tv->v_type = VAR_INSTR;
		    tv->vval.v_instr->instr_ectx = ectx;
		    tv->vval.v_instr->instr_instr = iptr->isn_arg.instr;
		}
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_ਸਰੋਤ:
		{
		    ਪੂਰਨ notused;

		    SOURCING_LNUM = iptr->isn_lnum;
		    ਜੇ (may_ਲੱਦੋ_script((ਪੂਰਨ)iptr->isn_arg.ਸੰਖਿਆ, &notused)
								       == FAIL)
			ਜਾਓ on_error;
		}
		ਰੋਕੋ;

	    // execute :substitute with an expression
	    ਹਾਲਤ ISN_SUBSTITUTE:
		{
		    subs_T		*subs = &iptr->isn_arg.subs;
		    ਸਰੋਤ_cookie_T	cookie;
		    ਬਣਤਰ subs_ਵਿਅੰਜਕ_S	*save_instr = substitute_instr;
		    ਬਣਤਰ subs_ਵਿਅੰਜਕ_S	subs_instr;
		    ਪੂਰਨ			res;

		    subs_instr.subs_ectx = ectx;
		    subs_instr.subs_instr = subs->subs_instr;
		    subs_instr.subs_ਸਥਿਤੀ = OK;
		    substitute_instr = &subs_instr;

		    SOURCING_LNUM = iptr->isn_lnum;
		    // This is very much like ISN_EXEC
		    CLEAR_ਖੇਤਰ(cookie);
		    cookie.sourcing_lnum = iptr->isn_lnum - 1;
		    res = do_cmdline(subs->subs_cmd,
				getsourceline, &cookie,
				   DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED);
		    substitute_instr = save_instr;

		    ਜੇ (res == FAIL || did_emsg
					     || subs_instr.subs_ਸਥਿਤੀ == FAIL)
			ਜਾਓ on_error;
		}
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_ਅੰਤ:
		ਜਾਓ ਪੂਰਾ;

	    ਹਾਲਤ ISN_REDIRSTART:
		// create a dummy entry for var_redir_str()
		ਜੇ (alloc_redir_lval() == FAIL)
		    ਜਾਓ on_error;

		// The output is stored in growarray "redir_ga" until
		// redirection ends.
		init_redir_ga();
		redir_vname = 1;
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_REDIREND:
		{
		    char_u *res = get_clear_redir_ga();

		    // End redirection, put redirected text on the stack.
		    clear_redir_lval();
		    redir_vname = 0;

		    ਜੇ (GA_GROW_FAILS(&ectx->ec_ਢੇਰ, 1))
		    {
			vim_free(res);
			ਜਾਓ theend;
		    }
		    tv = ਢੇਰ_TV_BOT(0);
		    tv->v_type = VAR_STRING;
		    tv->vval.v_string = res;
		    ++ectx->ec_ਢੇਰ.ga_len;
		}
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_CEXPR_AUCMD:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
		force_abort = TRUE;
		ਜੇ (trigger_cexpr_autocmd(iptr->isn_arg.ਸੰਖਿਆ) == FAIL)
		    ਜਾਓ on_error;
		force_abort = FALSE;
#ਜੇ_ਅੰਤ
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_CEXPR_CORE:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
		{
		    exarg_T ea;
		    ਪੂਰਨ	    res;

		    CLEAR_ਖੇਤਰ(ea);
		    ea.cmdidx = iptr->isn_arg.cexpr.cexpr_ref->cer_cmdidx;
		    ea.forceit = iptr->isn_arg.cexpr.cexpr_ref->cer_forceit;
		    ea.cmdlinep = &iptr->isn_arg.cexpr.cexpr_ref->cer_cmdline;
		    --ectx->ec_ਢੇਰ.ga_len;
		    tv = ਢੇਰ_TV_BOT(0);
		    SOURCING_LNUM = iptr->isn_lnum;
		    res = cexpr_core(&ea, tv);
		    clear_tv(tv);
		    ਜੇ (res == FAIL)
			ਜਾਓ on_error;
		}
#ਜੇ_ਅੰਤ
		ਰੋਕੋ;

	    // execute Ex command from pieces on the stack
	    ਹਾਲਤ ISN_EXECCONCAT:
		{
		    ਪੂਰਨ	    ਗਿਣਤੀ_2 = iptr->isn_arg.ਸੰਖਿਆ;
		    size_t  len = 0;
		    ਪੂਰਨ	    pass;
		    ਪੂਰਨ	    i;
		    char_u  *cmd = ਨਿਰਾ;
		    char_u  *str;

		    ਲਈ (pass = 1; pass <= 2; ++pass)
		    {
			ਲਈ (i = 0; i < ਗਿਣਤੀ_2; ++i)
			{
			    tv = ਢੇਰ_TV_BOT(i - ਗਿਣਤੀ_2);
			    str = tv->vval.v_string;
			    ਜੇ (str != ਨਿਰਾ && *str != NUL)
			    {
				ਜੇ (pass == 2)
				    STRCPY(cmd + len, str);
				len += STRLEN(str);
			    }
			    ਜੇ (pass == 2)
				clear_tv(tv);
			}
			ਜੇ (pass == 1)
			{
			    cmd = alloc(len + 1);
			    ਜੇ (unlikely(cmd == ਨਿਰਾ))
				ਜਾਓ theend;
			    len = 0;
			}
		    }

		    SOURCING_LNUM = iptr->isn_lnum;
		    do_cmdline_cmd(cmd);
		    vim_free(cmd);
		}
		ਰੋਕੋ;

	    // execute :echo {string} ...
	    ਹਾਲਤ ISN_ECHO:
		{
		    ਪੂਰਨ ਗਿਣਤੀ_2 = iptr->isn_arg.echo.echo_ਗਿਣਤੀ;
		    ਪੂਰਨ	atstart = TRUE;
		    ਪੂਰਨ needclr = TRUE;
		    ਪੂਰਨ	idx;

		    ਲਈ (idx = 0; idx < ਗਿਣਤੀ_2; ++idx)
		    {
			tv = ਢੇਰ_TV_BOT(idx - ਗਿਣਤੀ_2);
			echo_one(tv, iptr->isn_arg.echo.echo_with_white,
							   &atstart, &needclr);
			clear_tv(tv);
		    }
		    ਜੇ (needclr)
			ਸੁਨੇਹਾ_clr_eos();
		    ectx->ec_ਢੇਰ.ga_len -= ਗਿਣਤੀ_2;
		}
		ਰੋਕੋ;

	    // :execute {string} ...
	    // :echomsg {string} ...
	    // :echowindow {string} ...
	    // :echoconsole {string} ...
	    // :echoerr {string} ...
	    ਹਾਲਤ ISN_EXECUTE:
	    ਹਾਲਤ ISN_ECHOMSG:
	    ਹਾਲਤ ISN_ECHOWINDOW:
	    ਹਾਲਤ ISN_ECHOCONSOLE:
	    ਹਾਲਤ ISN_ECHOERR:
		{
		    ਪੂਰਨ		ਗਿਣਤੀ_2;
		    garray_T	ga;
		    char_u	buf[NUMBUFLEN];
		    char_u	*p;
		    ਪੂਰਨ		len;
		    ਪੂਰਨ		failed = FALSE;
		    ਪੂਰਨ		idx;

		    ਜੇ (iptr->isn_type == ISN_ECHOWINDOW)
			ਗਿਣਤੀ_2 = iptr->isn_arg.echowin.ewin_ਗਿਣਤੀ;
		    ਨਹੀਂਤਾਂ
			ਗਿਣਤੀ_2 = iptr->isn_arg.ਸੰਖਿਆ;
		    ga_init2(&ga, 1, 80);
		    ਲਈ (idx = 0; idx < ਗਿਣਤੀ_2; ++idx)
		    {
			tv = ਢੇਰ_TV_BOT(idx - ਗਿਣਤੀ_2);
			ਜੇ (iptr->isn_type == ISN_EXECUTE)
			{
			    ਜੇ (tv->v_type == VAR_CHANNEL
						      || tv->v_type == VAR_ਕੰਮ)
			    {
				SOURCING_LNUM = iptr->isn_lnum;
				semsg(_(e_using_invalid_ਮੁੱਲ_as_string_str),
						    vartype_name(tv->v_type));
				ਰੋਕੋ;
			    }
			    ਨਹੀਂਤਾਂ
				p = tv_get_string_buf(tv, buf);
			}
			ਨਹੀਂਤਾਂ
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    p = tv_stringify(tv, buf);
			}

			len = (ਪੂਰਨ)STRLEN(p);
			ਜੇ (GA_GROW_FAILS(&ga, len + 2))
			    failed = TRUE;
			ਨਹੀਂਤਾਂ
			{
			    ਜੇ (ga.ga_len > 0)
				((char_u *)(ga.ga_ਅੰਕੜੇ))[ga.ga_len++] = ' ';
			    STRCPY((char_u *)(ga.ga_ਅੰਕੜੇ) + ga.ga_len, p);
			    ga.ga_len += len;
			}
			clear_tv(tv);
		    }
		    ectx->ec_ਢੇਰ.ga_len -= ਗਿਣਤੀ_2;
		    ਜੇ (failed)
		    {
			ga_clear(&ga);
			ਜਾਓ on_error;
		    }

		    ਜੇ (ga.ga_ਅੰਕੜੇ != ਨਿਰਾ)
		    {
			ਜੇ (iptr->isn_type == ISN_EXECUTE)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    do_cmdline_cmd((char_u *)ga.ga_ਅੰਕੜੇ);
			    ਜੇ (did_emsg)
			    {
				ga_clear(&ga);
				ਜਾਓ on_error;
			    }
			}
			ਨਹੀਂਤਾਂ
			{
			    ਸੁਨੇਹਾ_sb_eol();
			    ਜੇ (iptr->isn_type == ISN_ECHOMSG)
			    {
				ਸੁਨੇਹਾ_attr(ga.ga_ਅੰਕੜੇ, echo_attr);
				ਬਾਹਰਲਾ_flush();
			    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAS_ਸੁਨੇਹਾ_ਖਿੜਕੀ
			    ਨਹੀਂਤਾਂ ਜੇ (iptr->isn_type == ISN_ECHOWINDOW)
			    {
				ਸ਼ੁਰੂ_echowindow(
					      iptr->isn_arg.echowin.ewin_ਸਮਾਂ);
				ਸੁਨੇਹਾ_attr(ga.ga_ਅੰਕੜੇ, echo_attr);
				ਸਿਰਾ_echowindow();
			    }
#ਜੇ_ਅੰਤ
			    ਨਹੀਂਤਾਂ ਜੇ (iptr->isn_type == ISN_ECHOCONSOLE)
			    {
				ui_write(ga.ga_ਅੰਕੜੇ, (ਪੂਰਨ)STRLEN(ga.ga_ਅੰਕੜੇ),
									 TRUE);
				ui_write((char_u *)"\r\n", 2, TRUE);
			    }
			    ਨਹੀਂਤਾਂ
			    {
				SOURCING_LNUM = iptr->isn_lnum;
				emsg(ga.ga_ਅੰਕੜੇ);
			    }
			}
		    }
		    ga_clear(&ga);
		}
		ਰੋਕੋ;

	    // load local variable or argument
	    ਹਾਲਤ ISN_ਲੱਦੋ:
		ਜੇ (GA_GROW_FAILS(&ectx->ec_ਢੇਰ, 1))
		    ਜਾਓ theend;
		tv = ਢੇਰ_TV_VAR(iptr->isn_arg.ਸੰਖਿਆ);
		ਜੇ (tv->v_type == VAR_UNKNOWN)
		{
		    // missing argument or default value v:none
		    ਢੇਰ_TV_BOT(0)->v_type = VAR_SPECIAL;
		    ਢੇਰ_TV_BOT(0)->vval.v_ਸੰਖਿਆ = VVAL_NONE;
		}
		ਨਹੀਂਤਾਂ
		    copy_tv(tv, ਢੇਰ_TV_BOT(0));
		++ectx->ec_ਢੇਰ.ga_len;
		ਰੋਕੋ;

	    // load v: variable
	    ਹਾਲਤ ISN_LOADV:
		ਜੇ (GA_GROW_FAILS(&ectx->ec_ਢੇਰ, 1))
		    ਜਾਓ theend;
		copy_tv(get_vim_var_tv(iptr->isn_arg.ਸੰਖਿਆ), ਢੇਰ_TV_BOT(0));
		++ectx->ec_ਢੇਰ.ga_len;
		ਰੋਕੋ;

	    // load s: variable in Vim9 script
	    ਹਾਲਤ ISN_LOADSCRIPT:
		{
		    scriptref_T	*sref = iptr->isn_arg.script.scriptref;
		    svar_T	 *sv;

		    sv = get_script_svar(sref, ectx->ec_dfunc_idx);
		    ਜੇ (sv == ਨਿਰਾ)
			ਜਾਓ theend;
		    allocate_if_null(sv);
		    ਜੇ (GA_GROW_FAILS(&ectx->ec_ਢੇਰ, 1))
			ਜਾਓ theend;
		    copy_tv(sv->sv_tv, ਢੇਰ_TV_BOT(0));
		    ++ectx->ec_ਢੇਰ.ga_len;
		}
		ਰੋਕੋ;

	    // load s: variable in old script or autoload import
	    ਹਾਲਤ ISN_LOADS:
	    ਹਾਲਤ ISN_LOADEXPORT:
		{
		    ਪੂਰਨ		sid = iptr->isn_arg.loadstore.ls_sid;
		    hashtab_T	*ht = &SCRIPT_VARS(sid);
		    char_u	*name = iptr->isn_arg.loadstore.ls_name;
		    dictitem_T	*di = ਲੱਭੋ_var_ਵਿੱਚ_ht(ht, 0, name, TRUE);

		    ਜੇ (di == ਨਿਰਾ)
		    {
			ufunc_T	*ufunc = ਨਿਰਾ;

			ਜੇ (iptr->isn_type == ISN_LOADEXPORT)
			{
			    type_T	*type;

			    // Not a variable, it can be an exported function
			    // used as a value.
			    (ਖਾਲੀ)ਲੱਭੋ_exported(sid, name, &ufunc, &type,
							 ਨਿਰਾ, ਨਿਰਾ, FALSE);
			}
			ਜੇ (ufunc == ਨਿਰਾ)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    semsg(_(e_undefined_variable_str), name);
			    ਜਾਓ on_error;
			}
			ਜੇ (GA_GROW_FAILS(&ectx->ec_ਢੇਰ, 1))
			    ਜਾਓ theend;
			tv = ਢੇਰ_TV_BOT(0);
			tv->v_ਤਾਲਾ = 0;
			++ectx->ec_ਢੇਰ.ga_len;
			tv->v_type = VAR_FUNC;
			tv->vval.v_string = vim_strsave(ufunc->uf_name);
		    }
		    ਨਹੀਂਤਾਂ
		    {
			ਜੇ (iptr->isn_type == ISN_LOADEXPORT)
			{
			    ਪੂਰਨ idx = get_script_ਵਸਤੂ_idx(sid, name, 0,
								   ਨਿਰਾ, ਨਿਰਾ);
			    svar_T	*sv;

			    ਜੇ (idx >= 0)
			    {
				sv = ((svar_T *)SCRIPT_ਵਸਤੂ(sid)
						  ->sn_var_vals.ga_ਅੰਕੜੇ) + idx;
				ਜੇ ((sv->sv_flags & SVFLAG_EXPORTED) == 0)
				{
				    SOURCING_LNUM = iptr->isn_lnum;
				    semsg(_(e_ਵਸਤੂ_not_exported_ਵਿੱਚ_script_str),
									 name);
				    ਜਾਓ on_error;
				}
			    }
			}
			ਜੇ (GA_GROW_FAILS(&ectx->ec_ਢੇਰ, 1))
			    ਜਾਓ theend;
			copy_tv(&di->di_tv, ਢੇਰ_TV_BOT(0));
			++ectx->ec_ਢੇਰ.ga_len;
		    }
		}
		ਰੋਕੋ;

	    // load g:/b:/w:/t: variable
	    ਹਾਲਤ ISN_LOADG:
	    ਹਾਲਤ ISN_LOADB:
	    ਹਾਲਤ ISN_LOADW:
	    ਹਾਲਤ ISN_LOADT:
		{
		    ਪੂਰਨ res = ਲੱਦੋ_namespace_var(ectx, iptr->isn_type, iptr);

		    ਜੇ (res == NOTDONE)
			ਜਾਓ theend;
		    ਜੇ (res == FAIL)
			ਜਾਓ on_error;
		}

		ਰੋਕੋ;

	    // load autoload variable
	    ਹਾਲਤ ISN_LOADAUTO:
		{
		    char_u *name = iptr->isn_arg.string;

		    ਜੇ (GA_GROW_FAILS(&ectx->ec_ਢੇਰ, 1))
			ਜਾਓ theend;
		    SOURCING_LNUM = iptr->isn_lnum;
		    ਜੇ (eval_variable(name, (ਪੂਰਨ)STRLEN(name), 0,
			      ਢੇਰ_TV_BOT(0), ਨਿਰਾ, EVAL_VAR_VERBOSE) == FAIL)
		    {
			tv = ਢੇਰ_TV_BOT(0);
			tv->v_type = VAR_ਸੰਖਿਆ;
			tv->v_ਤਾਲਾ = 0;
			tv->vval.v_ਸੰਖਿਆ = 0;
			++ectx->ec_ਢੇਰ.ga_len;
			ਜਾਓ on_error;
		    }
		    ++ectx->ec_ਢੇਰ.ga_len;
		}
		ਰੋਕੋ;

	    // load g:/b:/w:/t: namespace
	    ਹਾਲਤ ISN_LOADGDICT:
	    ਹਾਲਤ ISN_LOADBDICT:
	    ਹਾਲਤ ISN_LOADWDICT:
	    ਹਾਲਤ ISN_LOADTDICT:
		{
		    dict_T *d = ਨਿਰਾ;

		    ਚੋਣ (iptr->isn_type)
		    {
			ਹਾਲਤ ISN_LOADGDICT: d = get_globvar_dict(); ਰੋਕੋ;
			ਹਾਲਤ ISN_LOADBDICT: d = curbuf->b_vars; ਰੋਕੋ;
			ਹਾਲਤ ISN_LOADWDICT: d = curwin->w_vars; ਰੋਕੋ;
			ਹਾਲਤ ISN_LOADTDICT: d = curtab->tp_vars; ਰੋਕੋ;
			ਮੂਲ:  // Cannot reach here
			    ਜਾਓ theend;
		    }
		    ਜੇ (GA_GROW_FAILS(&ectx->ec_ਢੇਰ, 1))
			ਜਾਓ theend;
		    tv = ਢੇਰ_TV_BOT(0);
		    tv->v_type = VAR_DICT;
		    tv->v_ਤਾਲਾ = 0;
		    tv->vval.v_dict = d;
		    ++d->dv_refcount;
		    ++ectx->ec_ਢੇਰ.ga_len;
		}
		ਰੋਕੋ;

	    // load &option
	    ਹਾਲਤ ISN_LOADOPT:
		{
		    typval_T	optval;
		    char_u	*name = iptr->isn_arg.string;

		    // This is not expected to fail, name is checked during
		    // compilation: don't set SOURCING_LNUM.
		    ਜੇ (GA_GROW_FAILS(&ectx->ec_ਢੇਰ, 1))
			ਜਾਓ theend;
		    ਜੇ (eval_option(&name, &optval, TRUE) == FAIL)
			ਜਾਓ theend;
		    *ਢੇਰ_TV_BOT(0) = optval;
		    ++ectx->ec_ਢੇਰ.ga_len;
		}
		ਰੋਕੋ;

	    // load $ENV
	    ਹਾਲਤ ISN_LOADENV:
		{
		    typval_T	optval;
		    char_u	*name = iptr->isn_arg.string;

		    ਜੇ (GA_GROW_FAILS(&ectx->ec_ਢੇਰ, 1))
			ਜਾਓ theend;
		    // name is always valid, checked when compiling
		    (ਖਾਲੀ)eval_env_var(&name, &optval, TRUE);
		    *ਢੇਰ_TV_BOT(0) = optval;
		    ++ectx->ec_ਢੇਰ.ga_len;
		}
		ਰੋਕੋ;

	    // load @register
	    ਹਾਲਤ ISN_LOADREG:
		ਜੇ (GA_GROW_FAILS(&ectx->ec_ਢੇਰ, 1))
		    ਜਾਓ theend;
		tv = ਢੇਰ_TV_BOT(0);
		tv->v_type = VAR_STRING;
		tv->v_ਤਾਲਾ = 0;
		// This may result in NULL, which should be equivalent to an
		// empty string.
		tv->vval.v_string = get_reg_contents(
					  iptr->isn_arg.ਸੰਖਿਆ, GREG_ਵਿਅੰਜਕ_SRC);
		++ectx->ec_ਢੇਰ.ga_len;
		ਰੋਕੋ;

	    // store local variable
	    ਹਾਲਤ ISN_STORE:
		--ectx->ec_ਢੇਰ.ga_len;
		tv = ਢੇਰ_TV_VAR(iptr->isn_arg.ਸੰਖਿਆ);
		ਜੇ (check_typval_is_ਮੁੱਲ(ਢੇਰ_TV_BOT(0)) == FAIL
			|| ਢੇਰ_TV_BOT(0)->v_type == VAR_VOID)
		{
		    ਜੇ (ਢੇਰ_TV_BOT(0)->v_type == VAR_VOID)
			emsg(_(e_cannot_use_void_ਮੁੱਲ));
		    clear_tv(ਢੇਰ_TV_BOT(0));
		    ਜਾਓ on_error;
		}
		clear_tv(tv);
		*tv = *ਢੇਰ_TV_BOT(0);
		ਰੋਕੋ;

	    // store s: variable in old script or autoload import
	    ਹਾਲਤ ISN_STORES:
	    ਹਾਲਤ ISN_STOREEXPORT:
		{
		    ਪੂਰਨ		sid = iptr->isn_arg.loadstore.ls_sid;
		    char_u	*name = iptr->isn_arg.loadstore.ls_name;
		    dictitem_T	*di = ਨਿਰਾ;
		    // First check for a variable from an exported autoload
		    // with an autoload_prefix; it would be in globals.
		    ਜੇ (iptr->isn_type == ISN_STOREEXPORT)
			di = ਲੱਭੋ_var_autoload_ਅਗੇਤਰ(name, sid, ਨਿਰਾ, ਨਿਰਾ);
		    // Then look for a variable in the script's variables.
		    ਜੇ (di == ਨਿਰਾ)
		    {
			hashtab_T	*ht = &SCRIPT_VARS(sid);
			di = ਲੱਭੋ_var_ਵਿੱਚ_ht(ht, 0, STRNCMP("s:", name, 2) == 0
						     ? name + 2 : name, TRUE);
		    }

		    --ectx->ec_ਢੇਰ.ga_len;
		    SOURCING_LNUM = iptr->isn_lnum;
		    ਜੇ (di == ਨਿਰਾ)
		    {
			ਜੇ (iptr->isn_type == ISN_STOREEXPORT)
			{
			    semsg(_(e_undefined_variable_str), name);
			    clear_tv(ਢੇਰ_TV_BOT(0));
			    ਜਾਓ on_error;
			}
			store_var(name, ਢੇਰ_TV_BOT(0));
		    }
		    ਨਹੀਂਤਾਂ
		    {
			ਜੇ (iptr->isn_type == ISN_STOREEXPORT)
			{
			    ਪੂਰਨ idx = get_script_ਵਸਤੂ_idx(sid, name, 0,
								   ਨਿਰਾ, ਨਿਰਾ);

			    // can this ever fail?
			    ਜੇ (idx >= 0)
			    {
				svar_T	*sv = ((svar_T *)SCRIPT_ਵਸਤੂ(sid)
						  ->sn_var_vals.ga_ਅੰਕੜੇ) + idx;

				ਜੇ ((sv->sv_flags & SVFLAG_EXPORTED) == 0)
				{
				    semsg(_(e_ਵਸਤੂ_not_exported_ਵਿੱਚ_script_str),
									 name);
				    clear_tv(ਢੇਰ_TV_BOT(0));
				    ਜਾਓ on_error;
				}
			    }
			}
			ਜੇ (var_check_permission(di, name) == FAIL)
			{
			    clear_tv(ਢੇਰ_TV_BOT(0));
			    ਜਾਓ on_error;
			}
			clear_tv(&di->di_tv);
			di->di_tv = *ਢੇਰ_TV_BOT(0);
		    }
		}
		ਰੋਕੋ;

	    // store script-local variable in Vim9 script
	    ਹਾਲਤ ISN_STORESCRIPT:
		{
		    scriptref_T	    *sref = iptr->isn_arg.script.scriptref;
		    svar_T	    *sv;

		    sv = get_script_svar(sref, ectx->ec_dfunc_idx);
		    ਜੇ (sv == ਨਿਰਾ)
			ਜਾਓ theend;
		    --ectx->ec_ਢੇਰ.ga_len;

		    // "const" and "final" are checked at compile time, locking
		    // the value needs to be checked here.
		    SOURCING_LNUM = iptr->isn_lnum;
		    ਜੇ (ਮੁੱਲ_check_ਤਾਲਾ(sv->sv_tv->v_ਤਾਲਾ, sv->sv_name, FALSE))
		    {
			clear_tv(ਢੇਰ_TV_BOT(0));
			ਜਾਓ on_error;
		    }

		    clear_tv(sv->sv_tv);
		    *sv->sv_tv = *ਢੇਰ_TV_BOT(0);
		}
		ਰੋਕੋ;

	    // store option
	    ਹਾਲਤ ISN_STOREOPT:
	    ਹਾਲਤ ISN_STOREFUNCOPT:
		{
		    char_u	*opt_name = iptr->isn_arg.storeopt.so_name;
		    ਪੂਰਨ		opt_flags = iptr->isn_arg.storeopt.so_flags;
		    ਲੰਮਾ	n = 0;
		    char_u	*s = ਨਿਰਾ;
		    ਅੱਖਰ	*ਸੁਨੇਹਾ;
		    char_u	numbuf[NUMBUFLEN];
		    char_u	*tofree = ਨਿਰਾ;

		    --ectx->ec_ਢੇਰ.ga_len;
		    SOURCING_LNUM = iptr->isn_lnum;
		    tv = ਢੇਰ_TV_BOT(0);
		    ਜੇ (tv->v_type == VAR_STRING)
		    {
			s = tv->vval.v_string;
			ਜੇ (s == ਨਿਰਾ)
			    s = (char_u *)"";
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (iptr->isn_type == ISN_STOREFUNCOPT)
		    {
			// If the option can be set to a function reference or
			// a lambda and the passed value is a function
			// reference, then convert it to the name (string) of
			// the function reference.
			s = tv2string(tv, &tofree, numbuf, 0);
			ਜੇ (s == ਨਿਰਾ || *s == NUL)
			{
			    // cannot happen?
			    clear_tv(tv);
			    vim_free(tofree);
			    ਜਾਓ on_error;
			}
		    }
		    ਨਹੀਂਤਾਂ
			// must be VAR_NUMBER, CHECKTYPE makes sure
			n = tv->vval.v_ਸੰਖਿਆ;
		    ਸੁਨੇਹਾ = ਸੈੱਟ_option_ਮੁੱਲ(opt_name, n, s, opt_flags);
		    clear_tv(tv);
		    vim_free(tofree);
		    ਜੇ (ਸੁਨੇਹਾ != ਨਿਰਾ)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(ਸੁਨੇਹਾ));
			ਜਾਓ on_error;
		    }
		}
		ਰੋਕੋ;

	    // store $ENV
	    ਹਾਲਤ ISN_STOREENV:
		ਜੇ (check_secure() || check_restricted())
		    ਜਾਓ theend;
		--ectx->ec_ਢੇਰ.ga_len;
		tv = ਢੇਰ_TV_BOT(0);
		vim_setenv_ext(iptr->isn_arg.string, tv_get_string(tv));
		clear_tv(tv);
		ਰੋਕੋ;

	    // store @r
	    ਹਾਲਤ ISN_STOREREG:
		{
		    ਪੂਰਨ	reg = iptr->isn_arg.ਸੰਖਿਆ;

		    --ectx->ec_ਢੇਰ.ga_len;
		    tv = ਢੇਰ_TV_BOT(0);
		    write_reg_contents(reg, tv_get_string(tv), -1, FALSE);
		    clear_tv(tv);
		}
		ਰੋਕੋ;

	    // store v: variable
	    ਹਾਲਤ ISN_STOREV:
		--ectx->ec_ਢੇਰ.ga_len;
		ਜੇ (ਸੈੱਟ_vim_var_tv(iptr->isn_arg.ਸੰਖਿਆ, ਢੇਰ_TV_BOT(0))
								       == FAIL)
		    // should not happen, type is checked when compiling
		    ਜਾਓ on_error;
		ਰੋਕੋ;

	    // store g:/b:/w:/t: variable
	    ਹਾਲਤ ISN_STOREG:
	    ਹਾਲਤ ISN_STOREB:
	    ਹਾਲਤ ISN_STOREW:
	    ਹਾਲਤ ISN_STORET:
		{
		    dictitem_T	*di;
		    hashtab_T	*ht;
		    char_u	*name = iptr->isn_arg.string + 2;

		    ਚੋਣ (iptr->isn_type)
		    {
			ਹਾਲਤ ISN_STOREG:
			    ht = get_globvar_ht();
			    ਰੋਕੋ;
			ਹਾਲਤ ISN_STOREB:
			    ht = &curbuf->b_vars->dv_hashtab;
			    ਰੋਕੋ;
			ਹਾਲਤ ISN_STOREW:
			    ht = &curwin->w_vars->dv_hashtab;
			    ਰੋਕੋ;
			ਹਾਲਤ ISN_STORET:
			    ht = &curtab->tp_vars->dv_hashtab;
			    ਰੋਕੋ;
			ਮੂਲ:  // Cannot reach here
			    ਜਾਓ theend;
		    }

		    --ectx->ec_ਢੇਰ.ga_len;
		    di = ਲੱਭੋ_var_ਵਿੱਚ_ht(ht, 0, name, TRUE);
		    ਜੇ (di == ਨਿਰਾ)
			store_var(iptr->isn_arg.string, ਢੇਰ_TV_BOT(0));
		    ਨਹੀਂਤਾਂ
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			ਜੇ (var_check_permission(di, name) == FAIL)
			    ਜਾਓ on_error;
			clear_tv(&di->di_tv);
			di->di_tv = *ਢੇਰ_TV_BOT(0);
		    }
		}
		ਰੋਕੋ;

	    // store an autoload variable
	    ਹਾਲਤ ISN_STOREAUTO:
		SOURCING_LNUM = iptr->isn_lnum;
		ਸੈੱਟ_var(iptr->isn_arg.string, ਢੇਰ_TV_BOT(-1), TRUE);
		clear_tv(ਢੇਰ_TV_BOT(-1));
		--ectx->ec_ਢੇਰ.ga_len;
		ਰੋਕੋ;

	    // store number in local variable
	    ਹਾਲਤ ISN_STORENR:
		tv = ਢੇਰ_TV_VAR(iptr->isn_arg.storenr.stnr_idx);
		clear_tv(tv);
		tv->v_type = VAR_ਸੰਖਿਆ;
		tv->vval.v_ਸੰਖਿਆ = iptr->isn_arg.storenr.stnr_val;
		ਰੋਕੋ;

	    // Store a value in a list, tuple, dict, blob or object variable.
	    ਹਾਲਤ ISN_STOREINDEX:
		{
		    ਪੂਰਨ res = execute_storeindex(iptr, ectx);

		    ਜੇ (res == FAIL)
			ਜਾਓ on_error;
		    ਜੇ (res == NOTDONE)
			ਜਾਓ theend;
		}
		ਰੋਕੋ;

	    // store value in list or blob range
	    ਹਾਲਤ ISN_STORERANGE:
		ਜੇ (execute_storerange(iptr, ectx) == FAIL)
		    ਜਾਓ on_error;
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_ਲੱਦੋ_CLASSMEMBER:
		{
		    ਜੇ (GA_GROW_FAILS(&ectx->ec_ਢੇਰ, 1))
			ਜਾਓ theend;
		    classmember_T *cm = &iptr->isn_arg.classmember;
		    copy_tv(cm->cm_class->class_members_tv + cm->cm_idx,
							      ਢੇਰ_TV_BOT(0));
		    ++ectx->ec_ਢੇਰ.ga_len;
		}
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_STORE_CLASSMEMBER:
		{
		    classmember_T *cm = &iptr->isn_arg.classmember;
		    tv = &cm->cm_class->class_members_tv[cm->cm_idx];
		    clear_tv(tv);
		    *tv = *ਢੇਰ_TV_BOT(-1);
		    --ectx->ec_ਢੇਰ.ga_len;
		}
		ਰੋਕੋ;

	    // Load or store variable or argument from outer scope.
	    ਹਾਲਤ ISN_LOADOUTER:
	    ਹਾਲਤ ISN_STOREOUTER:
		{
		    ਪੂਰਨ		ਡੂੰਘਾਈ = iptr->isn_arg.outer.outer_ਡੂੰਘਾਈ;
		    outer_T	*outer = ectx->ec_outer_ref == ਨਿਰਾ ? ਨਿਰਾ
						: ectx->ec_outer_ref->or_outer;

		    ਜਦੋਂ (ਡੂੰਘਾਈ > 1 && outer != ਨਿਰਾ)
		    {
			outer = outer->ਬਾਹਰਲਾ_ਉੱਪਰ;
			--ਡੂੰਘਾਈ;
		    }
		    ਜੇ (outer == ਨਿਰਾ)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			ਜੇ (ectx->ec_ਚੌਖਟਾ_idx == ectx->ec_initial_ਚੌਖਟਾ_idx
						 || ectx->ec_outer_ref == ਨਿਰਾ)
			    // Possibly :def function called from legacy
			    // context.
			    emsg(_(e_ਘੇਰਾ_called_from_invalid_context));
			ਨਹੀਂਤਾਂ
			    iemsg("LOADOUTER depth more than scope levels");
			ਜਾਓ theend;
		    }
		    ਜੇ (ਡੂੰਘਾਈ < 0)
			// Variable declared in loop.  May be copied if the
			// loop block has already ended.
			tv = ((typval_T *)outer->ਬਾਹਰਲਾ_loop[-ਡੂੰਘਾਈ - 1]
							       .ਢੇਰ->ga_ਅੰਕੜੇ)
					  + outer->ਬਾਹਰਲਾ_loop[-ਡੂੰਘਾਈ - 1].var_idx
					  + iptr->isn_arg.outer.outer_idx;
		    ਨਹੀਂਤਾਂ
			// Variable declared in a function.  May be copied if
			// the function has already returned.
			tv = ((typval_T *)outer->ਬਾਹਰਲਾ_ਢੇਰ->ga_ਅੰਕੜੇ)
				      + outer->ਬਾਹਰਲਾ_ਚੌਖਟਾ_idx + ਢੇਰ_ਚੌਖਟਾ_ਮਾਪ
				      + iptr->isn_arg.outer.outer_idx;
		    ਜੇ (iptr->isn_type == ISN_LOADOUTER)
		    {
			typval_T *copy;
			ਜੇ (GA_GROW_FAILS(&ectx->ec_ਢੇਰ, 1))
			    ਜਾਓ theend;
			// careful: ga_grow_inner may re-alloc the stack
			ਜੇ (ਡੂੰਘਾਈ < 0)
			    copy = ((typval_T *)outer->ਬਾਹਰਲਾ_loop[-ਡੂੰਘਾਈ - 1]
								   .ਢੇਰ->ga_ਅੰਕੜੇ)
					      + outer->ਬਾਹਰਲਾ_loop[-ਡੂੰਘਾਈ - 1].var_idx
					      + iptr->isn_arg.outer.outer_idx;
			ਨਹੀਂਤਾਂ
			    copy = ((typval_T *)outer->ਬਾਹਰਲਾ_ਢੇਰ->ga_ਅੰਕੜੇ)
					  + outer->ਬਾਹਰਲਾ_ਚੌਖਟਾ_idx + ਢੇਰ_ਚੌਖਟਾ_ਮਾਪ
					  + iptr->isn_arg.outer.outer_idx;
			// memory was freed, get tv again
			ਜੇ (copy != tv)
			    tv = copy;
			copy_tv(tv, ਢੇਰ_TV_BOT(0));
			++ectx->ec_ਢੇਰ.ga_len;
		    }
		    ਨਹੀਂਤਾਂ
		    {
			--ectx->ec_ਢੇਰ.ga_len;
			clear_tv(tv);
			*tv = *ਢੇਰ_TV_BOT(0);
		    }
		}
		ਰੋਕੋ;

	    // unlet item in list or dict variable
	    ਹਾਲਤ ISN_UNLETINDEX:
		ਜੇ (execute_unletindex(iptr, ectx) == FAIL)
		    ਜਾਓ on_error;
		ਰੋਕੋ;

	    // unlet range of items in list variable
	    ਹਾਲਤ ISN_UNLETRANGE:
		ਜੇ (execute_unletrange(iptr, ectx) == FAIL)
		    ਜਾਓ on_error;
		ਰੋਕੋ;

	    // push constant
	    ਹਾਲਤ ISN_PUSHNR:
	    ਹਾਲਤ ISN_PUSHBOOL:
	    ਹਾਲਤ ISN_PUSHSPEC:
	    ਹਾਲਤ ISN_PUSHF:
	    ਹਾਲਤ ISN_PUSHS:
	    ਹਾਲਤ ISN_PUSHBLOB:
	    ਹਾਲਤ ISN_PUSHFUNC:
	    ਹਾਲਤ ISN_PUSHCHANNEL:
	    ਹਾਲਤ ISN_PUSHJOB:
	    ਹਾਲਤ ISN_PUSHOBJ:
	    ਹਾਲਤ ISN_PUSHCLASS:
		ਜੇ (GA_GROW_FAILS(&ectx->ec_ਢੇਰ, 1))
		    ਜਾਓ theend;
		tv = ਢੇਰ_TV_BOT(0);
		tv->v_ਤਾਲਾ = 0;
		++ectx->ec_ਢੇਰ.ga_len;
		ਚੋਣ (iptr->isn_type)
		{
		    ਹਾਲਤ ISN_PUSHNR:
			tv->v_type = VAR_ਸੰਖਿਆ;
			tv->vval.v_ਸੰਖਿਆ = iptr->isn_arg.ਸੰਖਿਆ;
			ਰੋਕੋ;
		    ਹਾਲਤ ISN_PUSHBOOL:
			tv->v_type = VAR_BOOL;
			tv->vval.v_ਸੰਖਿਆ = iptr->isn_arg.ਸੰਖਿਆ;
			ਰੋਕੋ;
		    ਹਾਲਤ ISN_PUSHSPEC:
			tv->v_type = VAR_SPECIAL;
			tv->vval.v_ਸੰਖਿਆ = iptr->isn_arg.ਸੰਖਿਆ;
			ਰੋਕੋ;
		    ਹਾਲਤ ISN_PUSHF:
			tv->v_type = VAR_FLOAT;
			tv->vval.v_float = iptr->isn_arg.fnumber;
			ਰੋਕੋ;
		    ਹਾਲਤ ISN_PUSHBLOB:
			tv->v_type = VAR_BLOB;
			tv->vval.v_blob = blob_copy(iptr->isn_arg.blob);
			ਰੋਕੋ;
		    ਹਾਲਤ ISN_PUSHFUNC:
			tv->v_type = VAR_FUNC;
			ਜੇ (iptr->isn_arg.string == ਨਿਰਾ)
			    tv->vval.v_string = ਨਿਰਾ;
			ਨਹੀਂਤਾਂ
			    tv->vval.v_string =
					     vim_strsave(iptr->isn_arg.string);
			ਰੋਕੋ;
		    ਹਾਲਤ ISN_PUSHCHANNEL:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
			tv->v_type = VAR_CHANNEL;
			tv->vval.v_channel = ਨਿਰਾ;
#ਜੇ_ਅੰਤ
			ਰੋਕੋ;
		    ਹਾਲਤ ISN_PUSHJOB:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
			tv->v_type = VAR_ਕੰਮ;
			tv->vval.v_ਕੰਮ = ਨਿਰਾ;
#ਜੇ_ਅੰਤ
			ਰੋਕੋ;
		    ਹਾਲਤ ISN_PUSHOBJ:
			tv->v_type = VAR_OBJECT;
			tv->vval.v_object = ਨਿਰਾ;
			ਰੋਕੋ;
		    ਹਾਲਤ ISN_PUSHCLASS:
			tv->v_type = VAR_CLASS;
			tv->vval.v_class = iptr->isn_arg.classarg;
			ਰੋਕੋ;
		    ਮੂਲ:
			tv->v_type = VAR_STRING;
			tv->vval.v_string = iptr->isn_arg.string == ਨਿਰਾ
				    ? ਨਿਰਾ : vim_strsave(iptr->isn_arg.string);
		}
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_AUTOLOAD:
		{
		    char_u  *name = iptr->isn_arg.string;

		    (ਖਾਲੀ)script_autoload(name, FALSE);
		    ਜੇ (ਲੱਭੋ_func(name, TRUE))
		    {
			ਜੇ (GA_GROW_FAILS(&ectx->ec_ਢੇਰ, 1))
			    ਜਾਓ theend;
			tv = ਢੇਰ_TV_BOT(0);
			tv->v_ਤਾਲਾ = 0;
			++ectx->ec_ਢੇਰ.ga_len;
			tv->v_type = VAR_FUNC;
			tv->vval.v_string = vim_strsave(name);
		    }
		    ਨਹੀਂਤਾਂ
		    {
			ਪੂਰਨ res = ਲੱਦੋ_namespace_var(ectx, ISN_LOADG, iptr);

			ਜੇ (res == NOTDONE)
			    ਜਾਓ theend;
			ਜੇ (res == FAIL)
			    ਜਾਓ on_error;
		    }
		}
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_UNLET:
		ਜੇ (do_unlet(iptr->isn_arg.unlet.ul_name,
				       iptr->isn_arg.unlet.ul_forceit) == FAIL)
		    ਜਾਓ on_error;
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_UNLETENV:
		vim_unsetenv_ext(iptr->isn_arg.unlet.ul_name);
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_LOCKUNLOCK:
		{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ LOG_LOCKVAR
		    ch_log(ਨਿਰਾ, "LKVAR: execute INS_LOCKUNLOCK isn_arg %s",
							iptr->isn_arg.string);
#ਜੇ_ਅੰਤ
		    lval_ਜੜ੍ਹ_T	*lval_ਜੜ੍ਹ_save = lval_ਜੜ੍ਹ;

		    // Stack has the local variable, argument the whole :lock
		    // or :unlock command, like ISN_EXEC.
		    --ectx->ec_ਢੇਰ.ga_len;
		    lval_ਜੜ੍ਹ_T ਜੜ੍ਹ;
		    ਜੜ੍ਹ.lr_tv      = ਢੇਰ_TV_BOT(0);
		    ਜੜ੍ਹ.lr_cl_exec = iptr->isn_arg.lockunlock.lu_cl_exec;
		    ਜੜ੍ਹ.lr_is_arg  = iptr->isn_arg.lockunlock.lu_is_arg;
		    lval_ਜੜ੍ਹ = &ਜੜ੍ਹ;
		    ਪੂਰਨ res = exec_command(iptr,
					iptr->isn_arg.lockunlock.lu_string);
		    clear_tv(ਜੜ੍ਹ.lr_tv);
		    lval_ਜੜ੍ਹ = lval_ਜੜ੍ਹ_save;
		    ਜੇ (res == FAIL)
			ਜਾਓ on_error;
		}
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_LOCKCONST:
		ਵਸਤੂ_ਤਾਲਾ(ਢੇਰ_TV_BOT(-1), 100, TRUE, TRUE);
		ਰੋਕੋ;

	    // create a list from items on the stack; uses a single allocation
	    // for the list header and the items
	    ਹਾਲਤ ISN_NEWLIST:
		ਜੇ (exe_newlist(iptr->isn_arg.ਸੰਖਿਆ, ectx) == FAIL)
		    ਜਾਓ theend;
		ਰੋਕੋ;

	    // create a tuple from items on the stack
	    ਹਾਲਤ ISN_NEWTUPLE:
		ਜੇ (exe_newtuple(iptr->isn_arg.ਸੰਖਿਆ, ectx) == FAIL)
		    ਜਾਓ theend;
		ਰੋਕੋ;

	    // create a dict from items on the stack
	    ਹਾਲਤ ISN_NEWDICT:
		{
		    ਪੂਰਨ res;

		    SOURCING_LNUM = iptr->isn_lnum;
		    res = exe_newdict(iptr->isn_arg.ਸੰਖਿਆ, ectx);
		    ਜੇ (res == FAIL)
			ਜਾਓ theend;
		    ਜੇ (res == MAYBE)
			ਜਾਓ on_error;
		}
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_CONCAT:
		ਜੇ (exe_concat(iptr->isn_arg.ਸੰਖਿਆ, ectx) == FAIL)
		    ਜਾਓ theend;
		ਰੋਕੋ;

	    // create a partial with NULL value
	    ਹਾਲਤ ISN_NEWPARTIAL:
		ਜੇ (GA_GROW_FAILS(&ectx->ec_ਢੇਰ, 1))
		    ਜਾਓ theend;
		++ectx->ec_ਢੇਰ.ga_len;
		tv = ਢੇਰ_TV_BOT(-1);
		tv->v_type = VAR_PARTIAL;
		tv->v_ਤਾਲਾ = 0;
		tv->vval.v_partial = ਨਿਰਾ;
		ਰੋਕੋ;

	    // call a :def function
	    ਹਾਲਤ ISN_DCALL:
		SOURCING_LNUM = iptr->isn_lnum;
		ਜੇ (call_dfunc(iptr->isn_arg.dfunc.cdf_idx,
				ਨਿਰਾ,
				iptr->isn_arg.dfunc.cdf_argcount,
				ectx) == FAIL)
		    ਜਾਓ on_error;
		ਰੋਕੋ;

	    // call a method on an interface
	    ਹਾਲਤ ISN_METHODCALL:
		{
		    cmfunc_T *mfunc = iptr->isn_arg.mfunc;

		    SOURCING_LNUM = iptr->isn_lnum;
		    tv = ਢੇਰ_TV_BOT(-1 - mfunc->cmf_argcount);
		    ਜੇ (tv->v_type != VAR_OBJECT)
		    {
			object_required_error(tv);
			ਜਾਓ on_error;
		    }

		    object_T *obj = tv->vval.v_object;
		    ਜੇ (obj == ਨਿਰਾ)
		    {
			emsg(_(e_using_null_object));
			ਜਾਓ on_error;
		    }

		    ufunc_T *ufunc;
		    class_T *cl;
		    ਜੇ (mfunc->cmf_is_super)
		    {
			// Doing "super.Func", use the specific ufunc.
			cl = mfunc->cmf_itf;
			ufunc = cl->class_obj_methods[mfunc->cmf_idx];
		    }
		    ਨਹੀਂਤਾਂ
		    {
			cl = obj->obj_class;

			// convert the interface index to the object index
			ਪੂਰਨ idx = object_ਸੂਚਕਾਂਕ_from_itf_ਸੂਚਕਾਂਕ(mfunc->cmf_itf,
						     TRUE, mfunc->cmf_idx, cl);
			ufunc = cl->class_obj_methods[idx];
		    }

		    // A generic method: use the method with the type arguments
		    // used when compiling.  The last one is cached, since
		    // looking it up is slow.
		    ਜੇ (mfunc->cmf_ufunc != ਨਿਰਾ && IS_GENERIC_FUNC(ufunc))
		    {
			ਜੇ (mfunc->cmf_cache_cl == cl
				&& mfunc->cmf_cache_generic == ufunc)
			    ufunc = mfunc->cmf_cache_ufunc;
			ਨਹੀਂਤਾਂ
			{
			    ufunc_T *generic_ufunc = ufunc;

			    ufunc = generic_func_get_same_types(generic_ufunc,
							    mfunc->cmf_ufunc);
			    ਜੇ (ufunc == ਨਿਰਾ)
				ਜਾਓ on_error;

			    ++cl->class_refcount;
			    class_unref(mfunc->cmf_cache_cl);
			    mfunc->cmf_cache_cl = cl;
			    mfunc->cmf_cache_generic = generic_ufunc;
			    mfunc->cmf_cache_ufunc = ufunc;
			}
		    }

		    ਜੇ (call_ufunc(ufunc, ਨਿਰਾ, mfunc->cmf_argcount, ectx,
							   ਨਿਰਾ, ਨਿਰਾ) == FAIL)
			ਜਾਓ on_error;
		}
		ਰੋਕੋ;

	    // call a builtin function
	    ਹਾਲਤ ISN_BCALL:
		SOURCING_LNUM = iptr->isn_lnum;
		ਜੇ (call_bfunc(iptr->isn_arg.bfunc.cbf_idx,
			      iptr->isn_arg.bfunc.cbf_argcount,
			      ectx) == FAIL)
		    ਜਾਓ on_error;
		ਰੋਕੋ;

	    // call a funcref or partial
	    ਹਾਲਤ ISN_PCALL:
		{
		    cpfunc_T	*pfunc = &iptr->isn_arg.pfunc;
		    ਪੂਰਨ		r;
		    typval_T	partial_tv;

		    SOURCING_LNUM = iptr->isn_lnum;
		    ਜੇ (pfunc->cpf_ਸਿਖਰ)
		    {
			// funcref is above the arguments
			tv = ਢੇਰ_TV_BOT(-pfunc->cpf_argcount - 1);
		    }
		    ਨਹੀਂਤਾਂ
		    {
			// Get the funcref from the stack.
			--ectx->ec_ਢੇਰ.ga_len;
			partial_tv = *ਢੇਰ_TV_BOT(0);
			tv = &partial_tv;
		    }
		    r = call_partial(tv, pfunc->cpf_argcount, ectx);
		    ਜੇ (tv == &partial_tv)
			clear_tv(&partial_tv);
		    ਜੇ (r == FAIL)
			ਜਾਓ on_error;
		}
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_PCALL_ਸਿਰਾ:
		// PCALL finished, arguments have been consumed and replaced by
		// the return value.  Now clear the funcref from the stack,
		// and move the return value in its place.
		--ectx->ec_ਢੇਰ.ga_len;
		clear_tv(ਢੇਰ_TV_BOT(-1));
		*ਢੇਰ_TV_BOT(-1) = *ਢੇਰ_TV_BOT(0);
		ਰੋਕੋ;

	    // call a user defined function or funcref/partial
	    ਹਾਲਤ ISN_UCALL:
		{
		    cufunc_T	*cufunc = &iptr->isn_arg.ufunc;

		    SOURCING_LNUM = iptr->isn_lnum;
		    ਜੇ (call_eval_func(cufunc->cuf_name, cufunc->cuf_argcount,
							   ectx, iptr) == FAIL)
			ਜਾਓ on_error;
		}
		ਰੋਕੋ;

	    // :defer func(arg)
	    ਹਾਲਤ ISN_DEFER:
		ਜੇ (defer_command(iptr->isn_arg.defer.defer_var_idx,
			     iptr->isn_arg.defer.defer_argcount, ectx) == FAIL)
		    ਜਾਓ on_error;
		ਰੋਕੋ;

	    // Return from a :def function call without a value.
	    // Return from a constructor.
	    ਹਾਲਤ ISN_RETURN_VOID:
	    ਹਾਲਤ ISN_RETURN_OBJECT:
		ਜੇ (GA_GROW_FAILS(&ectx->ec_ਢੇਰ, 1))
		    ਜਾਓ theend;
		tv = ਢੇਰ_TV_BOT(0);
		++ectx->ec_ਢੇਰ.ga_len;
		ਜੇ (iptr->isn_type == ISN_RETURN_VOID)
		{
		    tv->v_type = VAR_VOID;
		    tv->vval.v_ਸੰਖਿਆ = 0;
		    tv->v_ਤਾਲਾ = 0;
		}
		ਨਹੀਂਤਾਂ
		{
		    *tv = *ਢੇਰ_TV_VAR(0);
		    object_T *obj = tv->vval.v_object;
		    ++obj->obj_refcount;

		    // Lock all the constant object variables
		    obj_ਤਾਲਾ_const_vars(obj);
		}
		// FALLTHROUGH

	    // return from a :def function call with what is on the stack
	    ਹਾਲਤ ISN_RETURN:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycmd_T    *trycmd = ਨਿਰਾ;

		    ///////////////////////////////////////////////////
		    // TODO: If FAIL, line number in output not correct
		    ///////////////////////////////////////////////////
		    ਜੇ (check_typval_is_ਮੁੱਲ(ਢੇਰ_TV_BOT(-1)) == FAIL)
			ਜਾਓ theend;

		    ਜੇ (trystack->ga_len > 0)
			trycmd = ((trycmd_T *)trystack->ga_ਅੰਕੜੇ)
							+ trystack->ga_len - 1;
		    ਜੇ (trycmd != ਨਿਰਾ
				 && trycmd->tcd_ਚੌਖਟਾ_idx == ectx->ec_ਚੌਖਟਾ_idx)
		    {
			// jump to ":finally" or ":endtry"
			ਜੇ (trycmd->tcd_finally_idx != 0)
			    ectx->ec_iidx = trycmd->tcd_finally_idx;
			ਨਹੀਂਤਾਂ
			    ectx->ec_iidx = trycmd->tcd_endtry_idx;
			trycmd->tcd_return = TRUE;
		    }
		    ਨਹੀਂਤਾਂ
			ਜਾਓ func_return;
		}
		ਰੋਕੋ;

	    // push a partial, a reference to a compiled function
	    ਹਾਲਤ ISN_FUNCREF:
		{
		    partial_T	    *pt = ALLOC_CLEAR_ONE(partial_T);
		    ufunc_T	    *ufunc;
		    funcref_T	    *funcref = &iptr->isn_arg.funcref;
		    funcref_extra_T *extra = funcref->fr_extra;

		    ਜੇ (pt == ਨਿਰਾ)
			ਜਾਓ theend;
		    ਜੇ (GA_GROW_FAILS(&ectx->ec_ਢੇਰ, 1))
		    {
			vim_free(pt);
			ਜਾਓ theend;
		    }
		    ਜੇ (extra != ਨਿਰਾ && extra->fre_class != ਨਿਰਾ)
		    {
			class_T	*cl;
			ਜੇ (extra->fre_object_ਵਿਧੀ)
			{
			    tv = ਢੇਰ_TV_BOT(-1);
			    ਜੇ (tv->v_type != VAR_OBJECT)
			    {
				SOURCING_LNUM = iptr->isn_lnum;
				object_required_error(tv);
				vim_free(pt);
				ਜਾਓ on_error;
			    }

			    object_T *obj = tv->vval.v_object;
			    ਜੇ (obj == ਨਿਰਾ)
			    {
				SOURCING_LNUM = iptr->isn_lnum;
				emsg(_(e_using_null_object));
				vim_free(pt);
				ਜਾਓ on_error;
			    }

			    cl = obj->obj_class;
			    // drop the value from the stack
			    clear_tv(tv);
			    --ectx->ec_ਢੇਰ.ga_len;

			    pt->pt_obj = obj;
			    ++obj->obj_refcount;
			}
			ਨਹੀਂਤਾਂ
			    cl = extra->fre_class;

			ਜੇ (extra->fre_object_ਵਿਧੀ)
			{
			    // object method
			    // convert the interface index to the object index
			    ਪੂਰਨ idx =
				object_ਸੂਚਕਾਂਕ_from_itf_ਸੂਚਕਾਂਕ(extra->fre_class,
					TRUE, extra->fre_ਵਿਧੀ_idx, cl);
			    ufunc = cl->class_obj_methods[idx];
			}
			ਨਹੀਂਤਾਂ
			{
			    // class method
			    ufunc =
				cl->class_class_functions[extra->fre_ਵਿਧੀ_idx];
			}
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (extra == ਨਿਰਾ || extra->fre_func_name == ਨਿਰਾ)
		    {
			dfunc_T	*pt_dfunc = ((dfunc_T *)def_functions.ga_ਅੰਕੜੇ)
						       + funcref->fr_dfunc_idx;

			ufunc = pt_dfunc->df_ufunc;
		    }
		    ਨਹੀਂਤਾਂ
		    {
			ufunc = ਲੱਭੋ_func(extra->fre_func_name, FALSE);
		    }
		    ਜੇ (ufunc == ਨਿਰਾ)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			iemsg("ufunc unexpectedly NULL for FUNCREF");
			vim_free(pt);
			ਜਾਓ theend;
		    }
		    ਜੇ (extra != ਨਿਰਾ && extra->fre_outer_ਡੂੰਘਾਈ > 0
					   && (ufunc->uf_flags & FC_ਘੇਰਾ))
		    {
			// Use the context of the function where the nested
			// generic function is defined.
			partial_T *outer_pt = ectx->ec_outer_ref == ਨਿਰਾ
				    ? ਨਿਰਾ : ectx->ec_outer_ref->or_partial;

			ਲਈ (ਪੂਰਨ ਡੂੰਘਾਈ = 1; outer_pt != ਨਿਰਾ
				    && ਡੂੰਘਾਈ < extra->fre_outer_ਡੂੰਘਾਈ; ++ਡੂੰਘਾਈ)
			    outer_pt = get_pt_outer(outer_pt)->ਬਾਹਰਲਾ_ਉੱਪਰ_partial;
			ਜੇ (outer_pt == ਨਿਰਾ)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    iemsg("outer partial unexpectedly NULL for FUNCREF");
			    vim_free(pt);
			    ਜਾਓ theend;
			}
			pt->pt_func = ufunc;
			pt->pt_refcount = 1;
			pt->pt_outer_partial = outer_pt;
			++outer_pt->pt_refcount;
			++ufunc->uf_refcount;
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (fill_partial_and_ਘੇਰਾ(pt, ufunc,
			       extra == ਨਿਰਾ ? ਨਿਰਾ : &extra->fre_loopvar_info,
								 ectx) == FAIL)
		    {
			vim_free(pt);
			ਜਾਓ theend;
		    }
		    tv = ਢੇਰ_TV_BOT(0);
		    ++ectx->ec_ਢੇਰ.ga_len;
		    tv->vval.v_partial = pt;
		    tv->v_type = VAR_PARTIAL;
		    tv->v_ਤਾਲਾ = 0;
		}
		ਰੋਕੋ;

	    // Create a global function from a lambda.
	    ਹਾਲਤ ISN_NEWFUNC:
		{
		    newfuncarg_T    *arg = iptr->isn_arg.newfunc.nf_arg;

		    ਜੇ (copy_lambda_ਮੰਜ਼ਿਲ_global_func(arg->nfa_lambda,
				       arg->nfa_global, &arg->nfa_loopvar_info,
				       ectx) == FAIL)
			ਜਾਓ theend;
		}
		ਰੋਕੋ;

	    // List functions
	    ਹਾਲਤ ISN_DEF:
		ਜੇ (iptr->isn_arg.string == ਨਿਰਾ)
		    list_functions(ਨਿਰਾ);
		ਨਹੀਂਤਾਂ
		{
		    exarg_T	ea;
		    garray_T	ਸਤਰਾਂ_ਮੰਜ਼ਿਲ_free;

		    CLEAR_ਖੇਤਰ(ea);
		    ea.cmd = ea.arg = iptr->isn_arg.string;
		    ga_init2(&ਸਤਰਾਂ_ਮੰਜ਼ਿਲ_free, ਆਕਾਰ(char_u *), 50);
		    SOURCING_LNUM = iptr->isn_lnum;
		    define_function(&ea, ਨਿਰਾ, &ਸਤਰਾਂ_ਮੰਜ਼ਿਲ_free, 0, ਨਿਰਾ, 0, ਨਿਰਾ);
		    ga_clear_ਲੜੀਆਂ(&ਸਤਰਾਂ_ਮੰਜ਼ਿਲ_free);
		}
		ਰੋਕੋ;

	    // jump if a condition is met
	    ਹਾਲਤ ISN_JUMP:
		{
		    jumpwhen_T	when = iptr->isn_arg.jump.jump_when;
		    ਪੂਰਨ		error = FALSE;
		    ਪੂਰਨ		jump = TRUE;

		    ਜੇ (when != JUMP_ALWAYS)
		    {
			tv = ਢੇਰ_TV_BOT(-1);
			ਜੇ (when == JUMP_IF_COND_FALSE
				|| when == JUMP_IF_FALSE
				|| when == JUMP_IF_COND_TRUE)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    jump = tv_get_bool_chk(tv, &error);
			    ਜੇ (error)
				ਜਾਓ on_error;
			}
			ਨਹੀਂਤਾਂ
			    jump = tv2bool(tv);
			ਜੇ (when == JUMP_IF_FALSE || when == JUMP_IF_COND_FALSE)
			    jump = !jump;
			ਜੇ (when == JUMP_IF_FALSE || !jump)
			{
			    // drop the value from the stack
			    clear_tv(tv);
			    --ectx->ec_ਢੇਰ.ga_len;
			}
		    }
		    ਜੇ (jump)
			ectx->ec_iidx = iptr->isn_arg.jump.jump_where;
		}
		ਰੋਕੋ;

	    // "while": jump to end if a condition is false
	    ਹਾਲਤ ISN_WHILE:
		{
		    ਪੂਰਨ		error = FALSE;
		    ਪੂਰਨ		jump = TRUE;

		    tv = ਢੇਰ_TV_BOT(-1);
		    SOURCING_LNUM = iptr->isn_lnum;
		    jump = !tv_get_bool_chk(tv, &error);
		    ਜੇ (error)
			ਜਾਓ on_error;
		    // drop the value from the stack
		    clear_tv(tv);
		    --ectx->ec_ਢੇਰ.ga_len;
		    ਜੇ (jump)
			ectx->ec_iidx = iptr->isn_arg.whileloop.while_ਸਿਰਾ;

		    // Store the current funcref count, may be used by
		    // ISN_ENDLOOP later
		    tv = ਢੇਰ_TV_VAR(
				    iptr->isn_arg.whileloop.while_funcref_idx);
		    tv->vval.v_ਸੰਖਿਆ = ectx->ec_funcrefs.ga_len;
		}
		ਰੋਕੋ;

	    // Jump if an argument with a default value was already set and not
	    // v:none.
	    ਹਾਲਤ ISN_JUMP_IF_ARG_ਸੈੱਟ:
	    ਹਾਲਤ ISN_JUMP_IF_ARG_NOT_ਸੈੱਟ:
		tv = ਢੇਰ_TV_VAR(iptr->isn_arg.jumparg.jump_arg_off);
		ਪੂਰਨ arg_ਸੈੱਟ = tv->v_type != VAR_UNKNOWN
				&& !(tv->v_type == VAR_SPECIAL
					    && tv->vval.v_ਸੰਖਿਆ == VVAL_NONE);

		ਜੇ (iptr->isn_type == ISN_JUMP_IF_ARG_NOT_ਸੈੱਟ && !arg_ਸੈੱਟ)
		{
		    dfunc_T *df = ((dfunc_T *)def_functions.ga_ਅੰਕੜੇ)
							+ ectx->ec_dfunc_idx;
		    ufunc_T *ufunc = df->df_ufunc;
		    // jump_arg_off is negative for arguments
		    size_t argidx = ufunc->uf_def_ਦਲੀਲਾਂ.ga_len
					+ iptr->isn_arg.jumparg.jump_arg_off
					+ ਢੇਰ_ਚੌਖਟਾ_ਮਾਪ;
		    type_T *type = ufunc->uf_arg_types[argidx];
		    CLEAR_ਸੂਚਕ(tv);
		    // "any" is not a type a value can have, leave the
		    // argument marked as not set.
		    ਜੇ (type->tt_type != VAR_ANY)
			tv->v_type = type->tt_type;
		}

		ਜੇ (iptr->isn_type == ISN_JUMP_IF_ARG_ਸੈੱਟ ? arg_ਸੈੱਟ : !arg_ਸੈੱਟ)
		    ectx->ec_iidx = iptr->isn_arg.jumparg.jump_where;
		ਰੋਕੋ;

	    // top of a for loop
	    ਹਾਲਤ ISN_FOR:
		ਜੇ (execute_for(iptr, ectx) == FAIL)
		    ਜਾਓ theend;
		ਰੋਕੋ;

	    // end of a for or while loop
	    ਹਾਲਤ ISN_ENDLOOP:
		ਜੇ (execute_endloop(iptr, ectx) == FAIL)
		    ਜਾਓ theend;
		ਰੋਕੋ;

	    // start of ":try" block
	    ਹਾਲਤ ISN_TRY:
		{
		    trycmd_T    *trycmd = ਨਿਰਾ;

		    ਜੇ (GA_GROW_FAILS(&ectx->ec_trystack, 1))
			ਜਾਓ theend;
		    trycmd = ((trycmd_T *)ectx->ec_trystack.ga_ਅੰਕੜੇ)
						     + ectx->ec_trystack.ga_len;
		    ++ectx->ec_trystack.ga_len;
		    ++trylevel;
		    CLEAR_ਸੂਚਕ(trycmd);
		    trycmd->tcd_ਚੌਖਟਾ_idx = ectx->ec_ਚੌਖਟਾ_idx;
		    trycmd->tcd_ਢੇਰ_len = ectx->ec_ਢੇਰ.ga_len;
		    trycmd->tcd_catch_idx =
				       iptr->isn_arg.tryref.try_ref->try_catch;
		    trycmd->tcd_finally_idx =
				     iptr->isn_arg.tryref.try_ref->try_finally;
		    trycmd->tcd_endtry_idx =
				      iptr->isn_arg.tryref.try_ref->try_endtry;
		}
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_PUSHEXC:
		ਜੇ (current_exception == ਨਿਰਾ)
		{
		    SOURCING_LNUM = iptr->isn_lnum;
		    iemsg("Evaluating catch while current_exception is NULL");
		    ਜਾਓ theend;
		}
		ਜੇ (GA_GROW_FAILS(&ectx->ec_ਢੇਰ, 1))
		    ਜਾਓ theend;
		tv = ਢੇਰ_TV_BOT(0);
		++ectx->ec_ਢੇਰ.ga_len;
		tv->v_type = VAR_STRING;
		tv->v_ਤਾਲਾ = 0;
		tv->vval.v_string = vim_strsave(
					   (char_u *)current_exception->ਮੁੱਲ);
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_CATCH:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycmd_T    *trycmd;

		    may_restore_cmdmod(&ectx->ec_funclocal);
		    trycmd = ((trycmd_T *)trystack->ga_ਅੰਕੜੇ)
							+ trystack->ga_len - 1;
		    trycmd->tcd_caught = TRUE;
		    trycmd->tcd_did_throw = FALSE;

		    did_emsg = got_int = did_throw = FALSE;
		    force_abort = need_rethrow = FALSE;
		    catch_exception(current_exception);
		}
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_TRYCONT:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycont_T	*trycont = &iptr->isn_arg.trycont;
		    ਪੂਰਨ		i;
		    trycmd_T    *trycmd;
		    ਪੂਰਨ		iidx = trycont->tct_where;

		    ਜੇ (trystack->ga_len < trycont->tct_ਪੱਧਰ)
		    {
			siemsg("TRYCONT: expected %d levels, found %d",
					trycont->tct_ਪੱਧਰ, trystack->ga_len);
			ਜਾਓ theend;
		    }
		    // Make :endtry jump to any outer try block and the last
		    // :endtry inside the loop to the loop start.
		    ਲਈ (i = trycont->tct_ਪੱਧਰ; i > 0; --i)
		    {
			trycmd = ((trycmd_T *)trystack->ga_ਅੰਕੜੇ)
							+ trystack->ga_len - i;
			// Add one to tcd_cont to be able to jump to
			// instruction with index zero.
			trycmd->tcd_cont = iidx + 1;
			iidx = trycmd->tcd_finally_idx == 0
			    ? trycmd->tcd_endtry_idx : trycmd->tcd_finally_idx;
		    }
		    // jump to :finally or :endtry of current try statement
		    ectx->ec_iidx = iidx;
		}
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_FINALLY:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycmd_T    *trycmd = ((trycmd_T *)trystack->ga_ਅੰਕੜੇ)
							+ trystack->ga_len - 1;

		    // Reset the index to avoid a return statement jumps here
		    // again.
		    trycmd->tcd_finally_idx = 0;
		    ਜੇ (trycmd->tcd_caught)
		    {
			// discard the exception
			ਅੰਤ_exception(caught_ਢੇਰ);
			trycmd->tcd_caught = FALSE;
		    }
		    ਰੋਕੋ;
		}

	    // end of ":try" block
	    ਹਾਲਤ ISN_ENDTRY:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycmd_T    *trycmd;

		    --trystack->ga_len;
		    --trylevel;
		    trycmd = ((trycmd_T *)trystack->ga_ਅੰਕੜੇ) + trystack->ga_len;
		    ਜੇ (trycmd->tcd_did_throw)
			did_throw = TRUE;
		    ਜੇ (trycmd->tcd_caught)
		    {
			// discard the exception
			ਅੰਤ_exception(caught_ਢੇਰ);
		    }

		    ਜੇ (trycmd->tcd_return)
			ਜਾਓ func_return;

		    ਜਦੋਂ (ectx->ec_ਢੇਰ.ga_len > trycmd->tcd_ਢੇਰ_len)
		    {
			--ectx->ec_ਢੇਰ.ga_len;
			clear_tv(ਢੇਰ_TV_BOT(0));
		    }
		    ਜੇ (trycmd->tcd_cont != 0)
			// handling :continue: jump to outer try block or
			// start of the loop
			ectx->ec_iidx = trycmd->tcd_cont - 1;
		}
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_THROW:
		{
		    garray_T	*trystack = &ectx->ec_trystack;

		    ਜੇ (trystack->ga_len == 0 && trylevel == 0 && emsg_silent)
		    {
			// Throwing an exception while using "silent!" causes
			// the function to abort but not display an error.
			tv = ਢੇਰ_TV_BOT(-1);
			clear_tv(tv);
			tv->v_type = VAR_ਸੰਖਿਆ;
			tv->vval.v_ਸੰਖਿਆ = 0;
			ਜਾਓ ਪੂਰਾ;
		    }
		    --ectx->ec_ਢੇਰ.ga_len;
		    tv = ਢੇਰ_TV_BOT(0);
		    ਜੇ (tv->vval.v_string == ਨਿਰਾ
				       || *skipwhite(tv->vval.v_string) == NUL)
		    {
			vim_free(tv->vval.v_string);
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(e_throw_with_ਖਾਲੀ_ਥਾਵਾਂ_string));
			ਜਾਓ theend;
		    }

		    // Inside a "catch" we need to first discard the caught
		    // exception.
		    ਜੇ (trystack->ga_len > 0)
		    {
			trycmd_T    *trycmd = ((trycmd_T *)trystack->ga_ਅੰਕੜੇ)
							+ trystack->ga_len - 1;
			ਜੇ (trycmd->tcd_caught)
			{
			    // discard the exception
			    ਅੰਤ_exception(caught_ਢੇਰ);
			    trycmd->tcd_caught = FALSE;
			}
		    }

		    SOURCING_LNUM = iptr->isn_lnum;
		    ਜੇ (throw_exception(tv->vval.v_string, ET_USER, ਨਿਰਾ)
								       == FAIL)
		    {
			vim_free(tv->vval.v_string);
			ਜਾਓ theend;
		    }
		    did_throw = TRUE;
		}
		ਰੋਕੋ;

	    // compare with special values
	    ਹਾਲਤ ISN_COMPAREBOOL:
	    ਹਾਲਤ ISN_COMPARESPECIAL:
		{
		    typval_T	*tv1 = ਢੇਰ_TV_BOT(-2);
		    typval_T	*tv2 = ਢੇਰ_TV_BOT(-1);
		    varnumber_T arg1 = tv1->vval.v_ਸੰਖਿਆ;
		    varnumber_T arg2 = tv2->vval.v_ਸੰਖਿਆ;
		    ਪੂਰਨ		res;

		    ਜੇ (iptr->isn_arg.op.op_type == ਵਿਅੰਜਕ_EQUAL)
			res = arg1 == arg2;
		    ਨਹੀਂਤਾਂ
			res = arg1 != arg2;

		    --ectx->ec_ਢੇਰ.ga_len;
		    tv1->v_type = VAR_BOOL;
		    tv1->vval.v_ਸੰਖਿਆ = res ? VVAL_TRUE : VVAL_FALSE;
		}
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_COMPARENULL:
		{
		    typval_T	*tv1 = ਢੇਰ_TV_BOT(-2);
		    typval_T	*tv2 = ਢੇਰ_TV_BOT(-1);
		    ਪੂਰਨ		res;

		    res = typval_compare_null(tv1, tv2);
		    ਜੇ (res == MAYBE)
			ਜਾਓ on_error;
		    ਜੇ (iptr->isn_arg.op.op_type == ਵਿਅੰਜਕ_NEQUAL)
			res = !res;
		    clear_tv(tv1);
		    clear_tv(tv2);
		    --ectx->ec_ਢੇਰ.ga_len;
		    tv1->v_type = VAR_BOOL;
		    tv1->vval.v_ਸੰਖਿਆ = res ? VVAL_TRUE : VVAL_FALSE;
		}
		ਰੋਕੋ;

	    // Operation with two number arguments
	    ਹਾਲਤ ISN_OPNR:
	    ਹਾਲਤ ISN_COMPARENR:
		{
		    typval_T	*tv1 = ਢੇਰ_TV_BOT(-2);
		    typval_T	*tv2 = ਢੇਰ_TV_BOT(-1);
		    varnumber_T arg1 = tv1->vval.v_ਸੰਖਿਆ;
		    varnumber_T arg2 = tv2->vval.v_ਸੰਖਿਆ;
		    varnumber_T res = 0;
		    ਪੂਰਨ		div_zero = FALSE;

		    ਜੇ (iptr->isn_arg.op.op_type == ਵਿਅੰਜਕ_LSHIFT
			    || iptr->isn_arg.op.op_type == ਵਿਅੰਜਕ_RSHIFT)
		    {
			ਜੇ (arg2 < 0)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    emsg(_(e_bitshift_ops_must_be_ਸਕਾਰਾਤਮਕ));
			    ਜਾਓ on_error;
			}
		    }

		    ਚੋਣ (iptr->isn_arg.op.op_type)
		    {
			ਹਾਲਤ ਵਿਅੰਜਕ_MULT: res = arg1 * arg2; ਰੋਕੋ;
			ਹਾਲਤ ਵਿਅੰਜਕ_DIV:  ਜੇ (arg2 == 0)
					    div_zero = TRUE;
					ਨਹੀਂਤਾਂ
					    res = arg1 / arg2;
					ਰੋਕੋ;
			ਹਾਲਤ ਵਿਅੰਜਕ_REM:  ਜੇ (arg2 == 0)
					    div_zero = TRUE;
					ਨਹੀਂਤਾਂ
					    res = arg1 % arg2;
					ਰੋਕੋ;
			ਹਾਲਤ ਵਿਅੰਜਕ_SUB: res = arg1 - arg2; ਰੋਕੋ;
			ਹਾਲਤ ਵਿਅੰਜਕ_ਜੋੜੋ: res = arg1 + arg2; ਰੋਕੋ;

			ਹਾਲਤ ਵਿਅੰਜਕ_EQUAL: res = arg1 == arg2; ਰੋਕੋ;
			ਹਾਲਤ ਵਿਅੰਜਕ_NEQUAL: res = arg1 != arg2; ਰੋਕੋ;
			ਹਾਲਤ ਵਿਅੰਜਕ_GREATER: res = arg1 > arg2; ਰੋਕੋ;
			ਹਾਲਤ ਵਿਅੰਜਕ_GEQUAL: res = arg1 >= arg2; ਰੋਕੋ;
			ਹਾਲਤ ਵਿਅੰਜਕ_SMALLER: res = arg1 < arg2; ਰੋਕੋ;
			ਹਾਲਤ ਵਿਅੰਜਕ_SEQUAL: res = arg1 <= arg2; ਰੋਕੋ;
			ਹਾਲਤ ਵਿਅੰਜਕ_LSHIFT: ਜੇ (arg2 > MAX_LSHIFT_ਬਿੱਟ)
					      res = 0;
					  ਨਹੀਂਤਾਂ
					      res = (uvarnumber_T)arg1 << arg2;
					  ਰੋਕੋ;
			ਹਾਲਤ ਵਿਅੰਜਕ_RSHIFT: ਜੇ (arg2 > MAX_LSHIFT_ਬਿੱਟ)
					      res = 0;
					  ਨਹੀਂਤਾਂ
					      res = (uvarnumber_T)arg1 >> arg2;
					  ਰੋਕੋ;
			ਮੂਲ: ਰੋਕੋ;
		    }

		    --ectx->ec_ਢੇਰ.ga_len;
		    ਜੇ (iptr->isn_type == ISN_COMPARENR)
		    {
			tv1->v_type = VAR_BOOL;
			tv1->vval.v_ਸੰਖਿਆ = res ? VVAL_TRUE : VVAL_FALSE;
		    }
		    ਨਹੀਂਤਾਂ
			tv1->vval.v_ਸੰਖਿਆ = res;
		    ਜੇ (div_zero)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(e_divide_by_zero));
			ਜਾਓ on_error;
		    }
		}
		ਰੋਕੋ;

	    // Computation with two float arguments
	    ਹਾਲਤ ISN_OPFLOAT:
	    ਹਾਲਤ ISN_COMPAREFLOAT:
		{
		    typval_T	*tv1 = ਢੇਰ_TV_BOT(-2);
		    typval_T	*tv2 = ਢੇਰ_TV_BOT(-1);
		    float_T	arg1 = tv1->vval.v_float;
		    float_T	arg2 = tv2->vval.v_float;
		    float_T	res = 0;
		    ਪੂਰਨ		cmp = FALSE;

		    ਚੋਣ (iptr->isn_arg.op.op_type)
		    {
			ਹਾਲਤ ਵਿਅੰਜਕ_MULT: res = arg1 * arg2; ਰੋਕੋ;
			ਹਾਲਤ ਵਿਅੰਜਕ_DIV: res = arg1 / arg2; ਰੋਕੋ;
			ਹਾਲਤ ਵਿਅੰਜਕ_SUB: res = arg1 - arg2; ਰੋਕੋ;
			ਹਾਲਤ ਵਿਅੰਜਕ_ਜੋੜੋ: res = arg1 + arg2; ਰੋਕੋ;

			ਹਾਲਤ ਵਿਅੰਜਕ_EQUAL: cmp = arg1 == arg2; ਰੋਕੋ;
			ਹਾਲਤ ਵਿਅੰਜਕ_NEQUAL: cmp = arg1 != arg2; ਰੋਕੋ;
			ਹਾਲਤ ਵਿਅੰਜਕ_GREATER: cmp = arg1 > arg2; ਰੋਕੋ;
			ਹਾਲਤ ਵਿਅੰਜਕ_GEQUAL: cmp = arg1 >= arg2; ਰੋਕੋ;
			ਹਾਲਤ ਵਿਅੰਜਕ_SMALLER: cmp = arg1 < arg2; ਰੋਕੋ;
			ਹਾਲਤ ਵਿਅੰਜਕ_SEQUAL: cmp = arg1 <= arg2; ਰੋਕੋ;
			ਮੂਲ: cmp = 0; ਰੋਕੋ;
		    }
		    --ectx->ec_ਢੇਰ.ga_len;
		    ਜੇ (iptr->isn_type == ISN_COMPAREFLOAT)
		    {
			tv1->v_type = VAR_BOOL;
			tv1->vval.v_ਸੰਖਿਆ = cmp ? VVAL_TRUE : VVAL_FALSE;
		    }
		    ਨਹੀਂਤਾਂ
			tv1->vval.v_float = res;
		}
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_COMPARELIST:
	    ਹਾਲਤ ISN_COMPARETUPLE:
	    ਹਾਲਤ ISN_COMPAREDICT:
	    ਹਾਲਤ ISN_COMPAREFUNC:
	    ਹਾਲਤ ISN_COMPARESTRING:
	    ਹਾਲਤ ISN_COMPAREBLOB:
	    ਹਾਲਤ ISN_COMPAREOBJECT:
		{
		    typval_T	*tv1 = ਢੇਰ_TV_BOT(-2);
		    typval_T	*tv2 = ਢੇਰ_TV_BOT(-1);
		    exprtype_T	exprtype = iptr->isn_arg.op.op_type;
		    ਪੂਰਨ		ic = iptr->isn_arg.op.op_ic;
		    ਪੂਰਨ		res = FALSE;
		    ਪੂਰਨ		ਸਥਿਤੀ = OK;

		    SOURCING_LNUM = iptr->isn_lnum;
		    ਜੇ (iptr->isn_type == ISN_COMPARELIST)
		    {
			ਸਥਿਤੀ = typval_compare_list(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (iptr->isn_type == ISN_COMPARETUPLE)
		    {
			ਸਥਿਤੀ = typval_compare_tuple(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (iptr->isn_type == ISN_COMPAREDICT)
		    {
			ਸਥਿਤੀ = typval_compare_dict(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (iptr->isn_type == ISN_COMPAREFUNC)
		    {
			ਸਥਿਤੀ = typval_compare_func(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (iptr->isn_type == ISN_COMPARESTRING)
		    {
			ਸਥਿਤੀ = typval_compare_string(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (iptr->isn_type == ISN_COMPAREBLOB)
		    {
			ਸਥਿਤੀ = typval_compare_blob(tv1, tv2, exprtype, &res);
		    }
		    ਨਹੀਂਤਾਂ // ISN_COMPAREOBJECT
		    {
			ਸਥਿਤੀ = typval_compare_object(tv1, tv2,
							exprtype, FALSE, &res);
		    }
		    --ectx->ec_ਢੇਰ.ga_len;
		    clear_tv(tv1);
		    clear_tv(tv2);
		    tv1->v_type = VAR_BOOL;
		    tv1->vval.v_ਸੰਖਿਆ = res ? VVAL_TRUE : VVAL_FALSE;
		    ਜੇ (ਸਥਿਤੀ == FAIL)
			ਜਾਓ theend;
		}
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_COMPAREANY:
		{
		    typval_T	*tv1 = ਢੇਰ_TV_BOT(-2);
		    typval_T	*tv2 = ਢੇਰ_TV_BOT(-1);
		    exprtype_T	exprtype = iptr->isn_arg.op.op_type;
		    ਪੂਰਨ		ic = iptr->isn_arg.op.op_ic;
		    ਪੂਰਨ		ਸਥਿਤੀ;

		    SOURCING_LNUM = iptr->isn_lnum;
		    ਸਥਿਤੀ = typval_compare(tv1, tv2, exprtype, ic);
		    clear_tv(tv2);
		    --ectx->ec_ਢੇਰ.ga_len;
		    ਜੇ (ਸਥਿਤੀ == FAIL)
			ਜਾਓ theend;
		}
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_ADDLIST:
	    ਹਾਲਤ ISN_ADDTUPLE:
	    ਹਾਲਤ ISN_ADDBLOB:
		{
		    typval_T *tv1 = ਢੇਰ_TV_BOT(-2);
		    typval_T *tv2 = ਢੇਰ_TV_BOT(-1);

		    // add two lists or blobs
		    ਜੇ (iptr->isn_type == ISN_ADDLIST)
		    {
			ਜੇ (iptr->isn_arg.op.op_type == ਵਿਅੰਜਕ_APPEND
						   && tv1->vval.v_list != ਨਿਰਾ)
			    list_extend(tv1->vval.v_list, tv2->vval.v_list,
									 ਨਿਰਾ);
			ਨਹੀਂਤਾਂ
			    eval_addlist(tv1, tv2);
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (iptr->isn_type == ISN_ADDTUPLE)
			eval_addtuple(tv1, tv2);
		    ਨਹੀਂਤਾਂ
			eval_addblob(tv1, tv2);
		    clear_tv(tv2);
		    --ectx->ec_ਢੇਰ.ga_len;
		}
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_LISTAPPEND:
		{
		    typval_T	*tv1 = ਢੇਰ_TV_BOT(-2);
		    typval_T	*tv2 = ਢੇਰ_TV_BOT(-1);
		    list_T	*l = tv1->vval.v_list;

		    // add an item to a list
		    SOURCING_LNUM = iptr->isn_lnum;
		    ਜੇ (l == ਨਿਰਾ)
		    {
			emsg(_(e_cannot_ਜੋੜੋ_ਮੰਜ਼ਿਲ_null_list));
			ਜਾਓ on_error;
		    }
		    ਜੇ (ਮੁੱਲ_check_ਤਾਲਾ(l->lv_ਤਾਲਾ, ਨਿਰਾ, FALSE))
			ਜਾਓ on_error;
		    ਜੇ (list_append_tv(l, tv2) == FAIL)
			ਜਾਓ theend;
		    clear_tv(tv2);
		    --ectx->ec_ਢੇਰ.ga_len;
		}
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_BLOBAPPEND:
		{
		    typval_T	*tv1 = ਢੇਰ_TV_BOT(-2);
		    typval_T	*tv2 = ਢੇਰ_TV_BOT(-1);
		    blob_T	*b = tv1->vval.v_blob;
		    ਪੂਰਨ		error = FALSE;
		    varnumber_T n;

		    // add a number to a blob
		    ਜੇ (b == ਨਿਰਾ)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(e_cannot_ਜੋੜੋ_ਮੰਜ਼ਿਲ_null_blob));
			ਜਾਓ on_error;
		    }
		    n = tv_get_ਸੰਖਿਆ_chk(tv2, &error);
		    ਜੇ (error)
			ਜਾਓ on_error;
		    ga_append(&b->bv_ga, (ਪੂਰਨ)n);
		    --ectx->ec_ਢੇਰ.ga_len;
		}
		ਰੋਕੋ;

	    // Computation with two arguments of unknown type
	    ਹਾਲਤ ISN_OPANY:
		{
		    typval_T	*tv1 = ਢੇਰ_TV_BOT(-2);
		    typval_T	*tv2 = ਢੇਰ_TV_BOT(-1);
		    varnumber_T	n1, n2;
		    float_T	f1 = 0, f2 = 0;
		    ਪੂਰਨ		error = FALSE;

		    ਜੇ (iptr->isn_arg.op.op_type == ਵਿਅੰਜਕ_ਜੋੜੋ)
		    {
			ਜੇ (tv1->v_type == VAR_LIST && tv2->v_type == VAR_LIST)
			{
			    eval_addlist(tv1, tv2);
			    clear_tv(tv2);
			    --ectx->ec_ਢੇਰ.ga_len;
			    ਰੋਕੋ;
			}
			ਨਹੀਂਤਾਂ ਜੇ (tv1->v_type == VAR_TUPLE
						&& tv2->v_type == VAR_TUPLE)
			{
			    eval_addtuple(tv1, tv2);
			    clear_tv(tv2);
			    --ectx->ec_ਢੇਰ.ga_len;
			    ਰੋਕੋ;
			}
			ਨਹੀਂਤਾਂ ਜੇ (tv1->v_type == VAR_BLOB
						    && tv2->v_type == VAR_BLOB)
			{
			    eval_addblob(tv1, tv2);
			    clear_tv(tv2);
			    --ectx->ec_ਢੇਰ.ga_len;
			    ਰੋਕੋ;
			}
		    }
		    ਜੇ (tv1->v_type == VAR_FLOAT)
		    {
			f1 = tv1->vval.v_float;
			n1 = 0;
		    }
		    ਨਹੀਂਤਾਂ
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			n1 = tv_get_ਸੰਖਿਆ_chk(tv1, &error);
			ਜੇ (error)
			    ਜਾਓ on_error;
			ਜੇ (tv2->v_type == VAR_FLOAT)
			    f1 = n1;
		    }
		    ਜੇ (tv2->v_type == VAR_FLOAT)
		    {
			f2 = tv2->vval.v_float;
			n2 = 0;
		    }
		    ਨਹੀਂਤਾਂ
		    {
			n2 = tv_get_ਸੰਖਿਆ_chk(tv2, &error);
			ਜੇ (error)
			    ਜਾਓ on_error;
			ਜੇ (tv1->v_type == VAR_FLOAT)
			    f2 = n2;
		    }
		    // if there is a float on either side the result is a float
		    ਜੇ (tv1->v_type == VAR_FLOAT || tv2->v_type == VAR_FLOAT)
		    {
			ਚੋਣ (iptr->isn_arg.op.op_type)
			{
			    ਹਾਲਤ ਵਿਅੰਜਕ_MULT: f1 = f1 * f2; ਰੋਕੋ;
			    ਹਾਲਤ ਵਿਅੰਜਕ_DIV:  f1 = f1 / f2; ਰੋਕੋ;
			    ਹਾਲਤ ਵਿਅੰਜਕ_SUB:  f1 = f1 - f2; ਰੋਕੋ;
			    ਹਾਲਤ ਵਿਅੰਜਕ_ਜੋੜੋ:  f1 = f1 + f2; ਰੋਕੋ;
			    ਮੂਲ: SOURCING_LNUM = iptr->isn_lnum;
				     emsg(_(e_cannot_use_percent_with_float));
				     ਜਾਓ on_error;
			}
			clear_tv(tv1);
			clear_tv(tv2);
			tv1->v_type = VAR_FLOAT;
			tv1->vval.v_float = f1;
			--ectx->ec_ਢੇਰ.ga_len;
		    }
		    ਨਹੀਂਤਾਂ
		    {
			ਪੂਰਨ failed = FALSE;

			ਚੋਣ (iptr->isn_arg.op.op_type)
			{
			    ਹਾਲਤ ਵਿਅੰਜਕ_MULT: n1 = n1 * n2; ਰੋਕੋ;
			    ਹਾਲਤ ਵਿਅੰਜਕ_DIV:  n1 = ਅੰਕ_divide(n1, n2, &failed);
					    ਜੇ (failed)
						ਜਾਓ on_error;
					    ਰੋਕੋ;
			    ਹਾਲਤ ਵਿਅੰਜਕ_SUB:  n1 = n1 - n2; ਰੋਕੋ;
			    ਹਾਲਤ ਵਿਅੰਜਕ_ਜੋੜੋ:  n1 = n1 + n2; ਰੋਕੋ;
			    ਮੂਲ:	    n1 = ਅੰਕ_modulus(n1, n2, &failed);
					    ਜੇ (failed)
						ਜਾਓ on_error;
					    ਰੋਕੋ;
			}
			clear_tv(tv1);
			clear_tv(tv2);
			tv1->v_type = VAR_ਸੰਖਿਆ;
			tv1->vval.v_ਸੰਖਿਆ = n1;
			--ectx->ec_ਢੇਰ.ga_len;
		    }
		}
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_STRINDEX:
	    ਹਾਲਤ ISN_STRSLICE:
		{
		    ਪੂਰਨ		is_ਟੁਕੜਾ = iptr->isn_type == ISN_STRSLICE;
		    varnumber_T	n1 = 0, n2;
		    char_u	*res;

		    // string index: string is at stack-2, index at stack-1
		    // string slice: string is at stack-3, first index at
		    // stack-2, second index at stack-1
		    ਜੇ (is_ਟੁਕੜਾ)
		    {
			tv = ਢੇਰ_TV_BOT(-2);
			n1 = tv->vval.v_ਸੰਖਿਆ;
		    }

		    tv = ਢੇਰ_TV_BOT(-1);
		    n2 = tv->vval.v_ਸੰਖਿਆ;

		    ectx->ec_ਢੇਰ.ga_len -= is_ਟੁਕੜਾ ? 2 : 1;
		    tv = ਢੇਰ_TV_BOT(-1);
		    ਜੇ (is_ਟੁਕੜਾ)
			// Slice: Select the characters from the string
			res = string_ਟੁਕੜਾ(tv->vval.v_string, n1, n2, FALSE);
		    ਨਹੀਂਤਾਂ
			// Index: The resulting variable is a string of a
			// single character (including composing characters).
			// If the index is too big or negative the result is
			// empty.
			res = char_from_string(tv->vval.v_string, n2);
		    vim_free(tv->vval.v_string);
		    tv->vval.v_string = res;
		}
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_LISTINDEX:
	    ਹਾਲਤ ISN_LISTSLICE:
	    ਹਾਲਤ ISN_TUPLEINDEX:
	    ਹਾਲਤ ISN_TUPLESLICE:
	    ਹਾਲਤ ISN_BLOBINDEX:
	    ਹਾਲਤ ISN_BLOBSLICE:
		{
		    ਪੂਰਨ		is_ਟੁਕੜਾ = iptr->isn_type == ISN_LISTSLICE
				    || iptr->isn_type == ISN_TUPLESLICE
				    || iptr->isn_type == ISN_BLOBSLICE;
		    ਪੂਰਨ		is_blob = iptr->isn_type == ISN_BLOBINDEX
					    || iptr->isn_type == ISN_BLOBSLICE;
		    ਪੂਰਨ		is_tuple = iptr->isn_type == ISN_TUPLEINDEX
				    || iptr->isn_type == ISN_TUPLESLICE;
		    varnumber_T	n1, n2;
		    typval_T	*val_tv;

		    // list index: list is at stack-2, index at stack-1
		    // list slice: list is at stack-3, indexes at stack-2 and
		    // stack-1
		    // Same for tuple and blob.
		    val_tv = is_ਟੁਕੜਾ ? ਢੇਰ_TV_BOT(-3) : ਢੇਰ_TV_BOT(-2);

		    tv = ਢੇਰ_TV_BOT(-1);
		    n1 = n2 = tv->vval.v_ਸੰਖਿਆ;
		    clear_tv(tv);

		    ਜੇ (is_ਟੁਕੜਾ)
		    {
			tv = ਢੇਰ_TV_BOT(-2);
			n1 = tv->vval.v_ਸੰਖਿਆ;
			clear_tv(tv);
		    }

		    ectx->ec_ਢੇਰ.ga_len -= is_ਟੁਕੜਾ ? 2 : 1;
		    tv = ਢੇਰ_TV_BOT(-1);
		    SOURCING_LNUM = iptr->isn_lnum;
		    ਜੇ (is_blob)
		    {
			ਜੇ (blob_ਟੁਕੜਾ_or_ਸੂਚਕਾਂਕ(val_tv->vval.v_blob, is_ਟੁਕੜਾ,
						    n1, n2, FALSE, tv) == FAIL)
			    ਜਾਓ on_error;
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (is_tuple)
		    {
			ਜੇ (tuple_ਟੁਕੜਾ_or_ਸੂਚਕਾਂਕ(val_tv->vval.v_tuple,
				is_ਟੁਕੜਾ, n1, n2, FALSE, tv, TRUE) == FAIL)
			    ਜਾਓ on_error;
		    }
		    ਨਹੀਂਤਾਂ
		    {
			ਜੇ (list_ਟੁਕੜਾ_or_ਸੂਚਕਾਂਕ(val_tv->vval.v_list, is_ਟੁਕੜਾ,
					      n1, n2, FALSE, tv, TRUE) == FAIL)
			    ਜਾਓ on_error;
		    }
		}
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_ANYINDEX:
	    ਹਾਲਤ ISN_ANYSLICE:
		{
		    ਪੂਰਨ		is_ਟੁਕੜਾ = iptr->isn_type == ISN_ANYSLICE;
		    typval_T	*var1, *var2;
		    ਪੂਰਨ		res;

		    // index: composite is at stack-2, index at stack-1
		    // slice: composite is at stack-3, indexes at stack-2 and
		    // stack-1
		    tv = is_ਟੁਕੜਾ ? ਢੇਰ_TV_BOT(-3) : ਢੇਰ_TV_BOT(-2);
		    SOURCING_LNUM = iptr->isn_lnum;
		    ਜੇ (check_can_ਸੂਚਕਾਂਕ(tv, TRUE, TRUE) == FAIL)
			ਜਾਓ on_error;
		    var1 = is_ਟੁਕੜਾ ? ਢੇਰ_TV_BOT(-2) : ਢੇਰ_TV_BOT(-1);
		    var2 = is_ਟੁਕੜਾ ? ਢੇਰ_TV_BOT(-1) : ਨਿਰਾ;
		    res = eval_ਸੂਚਕਾਂਕ_inner(tv, is_ਟੁਕੜਾ, var1, var2,
							FALSE, ਨਿਰਾ, -1, TRUE);
		    clear_tv(var1);
		    ਜੇ (is_ਟੁਕੜਾ)
			clear_tv(var2);
		    ectx->ec_ਢੇਰ.ga_len -= is_ਟੁਕੜਾ ? 2 : 1;
		    ਜੇ (res == FAIL)
			ਜਾਓ on_error;
		}
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_ਟੁਕੜਾ:
		{
		    ਪੂਰਨ		ਗਿਣਤੀ_2 = iptr->isn_arg.ਸੰਖਿਆ;

		    // type will have been checked to be a list
		    tv = ਢੇਰ_TV_BOT(-1);
		    ਜੇ (tv->v_type == VAR_LIST)
		    {
			list_T *list = tv->vval.v_list;

			// no error for short list, expect it to be checked
			// earlier
			ਜੇ (list != ਨਿਰਾ && list->lv_len >= ਗਿਣਤੀ_2)
			{
			    list_T	*newlist = list_ਟੁਕੜਾ(list,
				    ਗਿਣਤੀ_2, list->lv_len - 1);

			    ਜੇ (newlist != ਨਿਰਾ)
			    {
				list_unref(list);
				tv->vval.v_list = newlist;
				++newlist->lv_refcount;
			    }
			}
		    }
		    ਨਹੀਂਤਾਂ
		    {
			tuple_T *tuple = tv->vval.v_tuple;

			// no error for short tuple, expect it to be checked
			// earlier
			ਜੇ (tuple != ਨਿਰਾ && TUPLE_LEN(tuple) >= ਗਿਣਤੀ_2)
			{
			    tuple_T *newtuple;

			    newtuple = tuple_ਟੁਕੜਾ(tuple, ਗਿਣਤੀ_2,
							TUPLE_LEN(tuple) - 1);
			    ਜੇ (newtuple != ਨਿਰਾ)
			    {
				tuple_unref(tuple);
				tv->v_type = VAR_TUPLE;
				tv->vval.v_tuple = newtuple;
				++newtuple->tv_refcount;
			    }
			}
		    }
		}
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_GETITEM:
		{
		    listitem_T	*li;
		    typval_T	*ਵਸਤੂ_tv;
		    getitem_T	*gi = &iptr->isn_arg.getitem;

		    // Get list item: list is at stack-1, push item.
		    // List type and length is checked for when compiling.
		    tv = ਢੇਰ_TV_BOT(-1 - gi->gi_with_op);
		    ਜੇ (tv->v_type == VAR_LIST)
		    {
			li = list_ਲੱਭੋ(tv->vval.v_list, gi->gi_ਸੂਚਕਾਂਕ);
			ਜੇ (li == ਨਿਰਾ)
			    ਜਾਓ on_error;
			ਵਸਤੂ_tv = &li->li_tv;
		    }
		    ਨਹੀਂਤਾਂ
			ਵਸਤੂ_tv = TUPLE_ਵਸਤੂ(tv->vval.v_tuple, gi->gi_ਸੂਚਕਾਂਕ);

		    ਜੇ (GA_GROW_FAILS(&ectx->ec_ਢੇਰ, 1))
			ਜਾਓ theend;
		    ++ectx->ec_ਢੇਰ.ga_len;
		    copy_tv(ਵਸਤੂ_tv, ਢੇਰ_TV_BOT(-1));

		    // Useful when used in unpack assignment.  Reset at
		    // ISN_DROP.
		    ectx->ec_where.wt_ਸੂਚਕਾਂਕ = gi->gi_ਸੂਚਕਾਂਕ + 1;
		    ectx->ec_where.wt_ਕਿਸਮ = WT_VARIABLE;
		}
		ਰੋਕੋ;

	    // dict member with string key (dict['member'])
	    ਹਾਲਤ ISN_MEMBER:
		{
		    dict_T	*dict;
		    char_u	*ਕੁੰਜੀ;
		    dictitem_T	*di;

		    // dict member: dict is at stack-2, key at stack-1
		    tv = ਢੇਰ_TV_BOT(-2);
		    // no need to check for VAR_DICT, CHECKTYPE will check.
		    dict = tv->vval.v_dict;

		    tv = ਢੇਰ_TV_BOT(-1);
		    // no need to check for VAR_STRING, 2STRING will check.
		    ਕੁੰਜੀ = tv->vval.v_string;
		    ਜੇ (ਕੁੰਜੀ == ਨਿਰਾ)
			ਕੁੰਜੀ = (char_u *)"";

		    ਜੇ ((di = dict_ਲੱਭੋ(dict, ਕੁੰਜੀ, -1)) == ਨਿਰਾ)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			semsg(_(e_ਕੁੰਜੀ_not_present_ਵਿੱਚ_dictionary_str), ਕੁੰਜੀ);

			// If :silent! is used we will continue, make sure the
			// stack contents makes sense and the dict stack is
			// updated.
			clear_tv(tv);
			--ectx->ec_ਢੇਰ.ga_len;
			tv = ਢੇਰ_TV_BOT(-1);
			(ਖਾਲੀ) dict_ਢੇਰ_save(tv);
			tv->v_type = VAR_ਸੰਖਿਆ;
			tv->vval.v_ਸੰਖਿਆ = 0;
			ਜਾਓ on_fatal_error;
		    }
		    clear_tv(tv);
		    --ectx->ec_ਢੇਰ.ga_len;
		    // Put the dict used on the dict stack, it might be used by
		    // a dict function later.
		    tv = ਢੇਰ_TV_BOT(-1);
		    ਜੇ (dict_ਢੇਰ_save(tv) == FAIL)
			ਜਾਓ on_fatal_error;
		    copy_tv(&di->di_tv, tv);
		}
		ਰੋਕੋ;

	    // dict member with string key (dict.member)
	    // or can be an object
	    ਹਾਲਤ ISN_STRINGMEMBER:
		{
		    dict_T	*dict;
		    dictitem_T	*di;

		    tv = ਢੇਰ_TV_BOT(-1);

		    ਜੇ (tv->v_type == VAR_OBJECT
			    || tv->v_type == VAR_CLASS)
		    {
			ਜੇ (dict_ਢੇਰ_save(tv) == FAIL)
			    ਜਾਓ on_fatal_error;

			ufunc_T *ufunc = (((dfunc_T *)def_functions.ga_ਅੰਕੜੇ)
					+ ectx->ec_dfunc_idx)->df_ufunc;
			// Class or an object (not a Dict)
			ਜੇ (var_any_get_oc_member(ufunc->uf_class, iptr, tv) == FAIL)
			    ਜਾਓ on_error;
		    }
		    ਨਹੀਂਤਾਂ
		    {
			ਜੇ (tv->v_type != VAR_DICT || tv->vval.v_dict == ਨਿਰਾ)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    emsg(_(e_dictionary_required));
			    ਜਾਓ on_error;
			}
			dict = tv->vval.v_dict;

			ਜੇ ((di = dict_ਲੱਭੋ(dict, iptr->isn_arg.string, -1))
								   == ਨਿਰਾ)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    semsg(_(e_ਕੁੰਜੀ_not_present_ਵਿੱਚ_dictionary_str),
						     iptr->isn_arg.string);
			    ਜਾਓ on_error;
			}
			// Put the dict used on the dict stack, it might be
			// used by a dict function later.
			ਜੇ (dict_ਢੇਰ_save(tv) == FAIL)
			    ਜਾਓ on_fatal_error;

			copy_tv(&di->di_tv, tv);
		    }
		}
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_GET_OBJ_MEMBER:
	    ਹਾਲਤ ISN_GET_ITF_MEMBER:
		{
		    tv = ਢੇਰ_TV_BOT(-1);
		    ਜੇ (tv->v_type != VAR_OBJECT)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			object_required_error(tv);
			ਜਾਓ on_error;
		    }

		    object_T *obj = tv->vval.v_object;
		    ਜੇ (obj == ਨਿਰਾ)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(e_using_null_object));
			ਜਾਓ on_error;
		    }

		    ਪੂਰਨ idx;
		    ਜੇ (iptr->isn_type == ISN_GET_OBJ_MEMBER)
			idx = iptr->isn_arg.classmember.cm_idx;
		    ਨਹੀਂਤਾਂ
		    {
			idx = iptr->isn_arg.classmember.cm_idx;
			// convert the interface index to the object index
			idx = object_ਸੂਚਕਾਂਕ_from_itf_ਸੂਚਕਾਂਕ(
					iptr->isn_arg.classmember.cm_class,
					FALSE, idx, obj->obj_class);
		    }

		    // The members are located right after the object struct.
		    typval_T *mtv = ((typval_T *)(obj + 1)) + idx;
		    ਜੇ (mtv->v_type == VAR_UNKNOWN)
		    {
			// Referencing an object variable (without a type)
			// which is not yet initialized.  So the type is not
			// yet known.
			ocmember_T *m = &obj->obj_class->class_obj_members[idx];
			SOURCING_LNUM = iptr->isn_lnum;
			semsg(_(e_uninitialized_object_var_reference),
				m->ocm_name.string);
			ਜਾਓ on_error;
		    }
		    copy_tv(mtv, tv);

		    // Unreference the object after getting the member, it may
		    // be freed.
		    object_unref(obj);
		}
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_STORE_THIS:
		{
		    ਪੂਰਨ idx = iptr->isn_arg.ਸੰਖਿਆ;
		    object_T *obj = ਢੇਰ_TV_VAR(0)->vval.v_object;
		    // the members are located right after the object struct
		    typval_T *mtv = ((typval_T *)(obj + 1)) + idx;
		    clear_tv(mtv);
		    *mtv = *ਢੇਰ_TV_BOT(-1);
		    --ectx->ec_ਢੇਰ.ga_len;
		}
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_CLEARDICT:
		dict_ਢੇਰ_drop();
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_USEDICT:
		{
		    typval_T *dict_tv = dict_ਢੇਰ_get_tv();

		    // Turn "dict.Func" into a partial for "Func" bound to
		    // "dict".  Don't do this when "Func" is already a partial
		    // that was bound explicitly (pt_auto is FALSE).
		    tv = ਢੇਰ_TV_BOT(-1);
		    ਜੇ (dict_tv != ਨਿਰਾ
			    && dict_tv->v_type == VAR_DICT
			    && dict_tv->vval.v_dict != ਨਿਰਾ
			    && (tv->v_type == VAR_FUNC
				|| (tv->v_type == VAR_PARTIAL
				    && (tv->vval.v_partial->pt_auto
				     || tv->vval.v_partial->pt_dict == ਨਿਰਾ))))
		    dict_tv->vval.v_dict =
					make_partial(dict_tv->vval.v_dict, tv);
		    dict_ਢੇਰ_drop();
		}
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_NEGATENR:
		tv = ਢੇਰ_TV_BOT(-1);
		// CHECKTYPE should have checked the variable type
		ਜੇ (tv->v_type == VAR_FLOAT)
		    tv->vval.v_float = -tv->vval.v_float;
		ਨਹੀਂਤਾਂ
		    tv->vval.v_ਸੰਖਿਆ = -tv->vval.v_ਸੰਖਿਆ;
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_CHECKTYPE:
		{
		    checktype_T *ct = &iptr->isn_arg.type;
		    ਪੂਰਨ		r;
		    where_T	where = WHERE_INIT;

		    tv = ਢੇਰ_TV_BOT((ਪੂਰਨ)ct->ct_off);
		    SOURCING_LNUM = iptr->isn_lnum;
		    ਜੇ (ct->ct_arg_idx > 0)
		    {
			where.wt_ਸੂਚਕਾਂਕ = ct->ct_arg_idx;
			where.wt_ਕਿਸਮ = ct->ct_is_var ? WT_VARIABLE : WT_ਦਲੀਲ;
		    }
		    where.wt_func_name = ectx->ec_where.wt_func_name;
		    r = check_typval_type(ct->ct_type, tv, where);
		    ਜੇ (r == FAIL)
			ਜਾਓ on_error;

		    // number 0 is FALSE, number 1 is TRUE
		    ਜੇ (tv->v_type == VAR_ਸੰਖਿਆ
			    && ct->ct_type->tt_type == VAR_BOOL
			    && (tv->vval.v_ਸੰਖਿਆ == 0
						|| tv->vval.v_ਸੰਖਿਆ == 1))
		    {
			tv->v_type = VAR_BOOL;
			tv->vval.v_ਸੰਖਿਆ = tv->vval.v_ਸੰਖਿਆ
						      ? VVAL_TRUE : VVAL_FALSE;
		    }
		}
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_CHECKLEN:
		{
		    ਪੂਰਨ	    min_len = iptr->isn_arg.checklen.cl_min_len;
		    list_T  *list = ਨਿਰਾ;
		    tuple_T *tuple = ਨਿਰਾ;
		    ਪੂਰਨ	    len = 0;

		    tv = ਢੇਰ_TV_BOT(-1);

		    ਪੂਰਨ		len_check_failed = FALSE;
		    ਜੇ (tv->v_type == VAR_LIST)
		    {
			list = tv->vval.v_list;
			ਜੇ (list == ਨਿਰਾ || list->lv_len < min_len
			    || (list->lv_len > min_len
					&& !iptr->isn_arg.checklen.cl_ਹੋਰ_OK))
			    len_check_failed = TRUE;
			ਜੇ (list != ਨਿਰਾ)
			    len = list->lv_len;
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (tv->v_type == VAR_TUPLE)
		    {
			tuple = tv->vval.v_tuple;
			ਜੇ (tuple == ਨਿਰਾ || TUPLE_LEN(tuple) < min_len
			    || (TUPLE_LEN(tuple) > min_len
					&& !iptr->isn_arg.checklen.cl_ਹੋਰ_OK))
			    len_check_failed = TRUE;
			ਜੇ (tuple != ਨਿਰਾ)
			    len = TUPLE_LEN(tuple);
		    }
		    ਨਹੀਂਤਾਂ
			len_check_failed = TRUE;

		    ਜੇ (len_check_failed)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			semsg(_(e_expected_nr_ਚੀਜ਼ਾਂ_but_got_nr),
				     min_len, len);
			ਜਾਓ on_error;
		    }
		}
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_SETTYPE:
		ਸੈੱਟ_tv_type(ਢੇਰ_TV_BOT(-1), iptr->isn_arg.type.ct_type);
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_2BOOL:
	    ਹਾਲਤ ISN_COND2BOOL:
		{
		    ਪੂਰਨ n;
		    ਪੂਰਨ error = FALSE;

		    ਜੇ (iptr->isn_type == ISN_2BOOL)
		    {
			tv = ਢੇਰ_TV_BOT(iptr->isn_arg.tobool.ਵਿੱਥ);
			n = tv2bool(tv);
			ਜੇ (iptr->isn_arg.tobool.invert)
			    n = !n;
		    }
		    ਨਹੀਂਤਾਂ
		    {
			tv = ਢੇਰ_TV_BOT(-1);
			SOURCING_LNUM = iptr->isn_lnum;
			n = tv_get_bool_chk(tv, &error);
			ਜੇ (error)
			    ਜਾਓ on_error;
		    }
		    clear_tv(tv);
		    tv->v_type = VAR_BOOL;
		    tv->vval.v_ਸੰਖਿਆ = n ? VVAL_TRUE : VVAL_FALSE;
		}
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_2STRING:
	    ਹਾਲਤ ISN_2STRING_ANY:
		SOURCING_LNUM = iptr->isn_lnum;
		ਜੇ (do_2string(ਢੇਰ_TV_BOT(iptr->isn_arg.tostring.ਵਿੱਥ),
				iptr->isn_type == ISN_2STRING_ANY,
				      iptr->isn_arg.tostring.flags) == FAIL)
			    ਜਾਓ on_error;
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_RANGE:
		{
		    exarg_T	ea;
		    ਅੱਖਰ	*errormsg;

		    ea.line2 = 0;
		    ea.addr_ਗਿਣਤੀ = 0;
		    ea.addr_type = ADDR_ਸਤਰਾਂ;
		    ea.cmd = iptr->isn_arg.string;
		    ea.skip = FALSE;
		    ਜੇ (parse_cmd_ਪਤਾ(&ea, &errormsg, FALSE) == FAIL)
			ਜਾਓ on_error;

		    ਜੇ (GA_GROW_FAILS(&ectx->ec_ਢੇਰ, 1))
			ਜਾਓ theend;
		    ++ectx->ec_ਢੇਰ.ga_len;
		    tv = ਢੇਰ_TV_BOT(-1);
		    tv->v_type = VAR_ਸੰਖਿਆ;
		    tv->v_ਤਾਲਾ = 0;
		    tv->vval.v_ਸੰਖਿਆ = ea.line2;
		}
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_PUT:
		tv = ਨਿਰਾ; // initialize it so we don't get a warning
		isn_put_do(ectx, iptr, tv, FALSE);
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_IPUT:
		tv = ਨਿਰਾ;
		isn_put_do(ectx, iptr, tv, TRUE);
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_CMDMOD:
		ectx->ec_funclocal.floc_save_cmdmod = cmdmod;
		ectx->ec_funclocal.floc_restore_cmdmod = TRUE;
		ectx->ec_funclocal.floc_restore_cmdmod_stacklen =
							 ectx->ec_ਢੇਰ.ga_len;
		cmdmod = *iptr->isn_arg.cmdmod.cf_cmdmod;
		apply_cmdmod(&cmdmod);
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_CMDMOD_REV:
		// filter regprog is owned by the instruction, don't free it
		cmdmod.cmod_filter_regmatch.regprog = ਨਿਰਾ;
		undo_cmdmod(&cmdmod);
		cmdmod = ectx->ec_funclocal.floc_save_cmdmod;
		ectx->ec_funclocal.floc_restore_cmdmod = FALSE;
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_UNPACK:
		// Check there is a valid list to unpack.
		tv = ਢੇਰ_TV_BOT(-1);
		ਜੇ (tv->v_type != VAR_LIST && tv->v_type != VAR_TUPLE)
		{
		    SOURCING_LNUM = iptr->isn_lnum;
		    emsg(_(e_for_ਦਲੀਲ_must_be_sequence_ਦਾ_lists_or_tuples));
		    ਜਾਓ on_error;
		}

		ਪੂਰਨ rc;
		ਜੇ (tv->v_type == VAR_LIST)
		    rc = exec_unpack_list(ectx, iptr, tv);
		ਨਹੀਂਤਾਂ
		    rc = exec_unpack_tuple(ectx, iptr, tv);
		ਜੇ (rc != EXEC_OK)
		{
		    ਜੇ (rc == EXEC_FAIL)
			ਜਾਓ on_error;
		    ਜਾਓ theend;
		}
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_PROF_ਸ਼ੁਰੂ:
	    ਹਾਲਤ ISN_PROF_ਸਿਰਾ:
		{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
		    funccall_T	cookie;
		    ufunc_T	*cur_ufunc =
				    (((dfunc_T *)def_functions.ga_ਅੰਕੜੇ)
					       + ectx->ec_dfunc_idx)->df_ufunc;

		    cookie.fc_func = cur_ufunc;
		    ਜੇ (iptr->isn_type == ISN_PROF_ਸ਼ੁਰੂ)
		    {
			func_ਸਤਰ_ਸ਼ੁਰੂ(&cookie, iptr->isn_lnum);
			// if we get here the instruction is executed
			func_ਸਤਰ_exec(&cookie);
		    }
		    ਨਹੀਂਤਾਂ
			func_ਸਤਰ_ਸਿਰਾ(&cookie);
#ਜੇ_ਅੰਤ
		}
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_DEBUG:
		handle_debug(iptr, ectx);
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_SHUFFLE:
		{
		    typval_T	tmp_tv;
		    ਪੂਰਨ		ਵਸਤੂ = iptr->isn_arg.shuffle.shfl_ਵਸਤੂ;
		    ਪੂਰਨ		ਉੱਪਰ = iptr->isn_arg.shuffle.shfl_ਉੱਪਰ;

		    tmp_tv = *ਢੇਰ_TV_BOT(-ਵਸਤੂ);
		    ਲਈ ( ; ਉੱਪਰ > 0 && ਵਸਤੂ > 1; --ਉੱਪਰ)
		    {
			*ਢੇਰ_TV_BOT(-ਵਸਤੂ) = *ਢੇਰ_TV_BOT(-ਵਸਤੂ + 1);
			--ਵਸਤੂ;
		    }
		    *ਢੇਰ_TV_BOT(-ਵਸਤੂ) = tmp_tv;
		}
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_DROP:
		--ectx->ec_ਢੇਰ.ga_len;
		clear_tv(ਢੇਰ_TV_BOT(0));
		ectx->ec_where = (where_T)WHERE_INIT;
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_SCRIPTCTX_ਸੈੱਟ:
		// change the script context.  Used to evaluate an object
		// member variable initialization expression in the context of
		// the script where the class is defined.
		current_sctx = iptr->isn_arg.setsctx;
		ਰੋਕੋ;
	}
	ਜਾਰੀ;

func_return:
	// Restore previous function. If the frame pointer is where we started
	// then there is none and we are done.
	ਜੇ (ectx->ec_ਚੌਖਟਾ_idx == ectx->ec_initial_ਚੌਖਟਾ_idx)
	    ਜਾਓ ਪੂਰਾ;

	ਜੇ (func_return(ectx) == FAIL)
	    // only fails when out of memory
	    ਜਾਓ theend;
	ਜਾਰੀ;

on_error:
	// Jump here for an error that does not require aborting execution.
	// If "emsg_silent" is set then ignore the error, unless it was set
	// when calling the function.
	ਜੇ (did_emsg_cumul + did_emsg == ectx->ec_did_emsg_before
					   && emsg_silent && did_emsg_def == 0)
	{
	    // If a sequence of instructions causes an error while ":silent!"
	    // was used, restore the stack length and jump ahead to restoring
	    // the cmdmod.
	    ਜੇ (ectx->ec_funclocal.floc_restore_cmdmod)
	    {
		ਜਦੋਂ (ectx->ec_ਢੇਰ.ga_len
			     > ectx->ec_funclocal.floc_restore_cmdmod_stacklen)
		{
		    --ectx->ec_ਢੇਰ.ga_len;
		    clear_tv(ਢੇਰ_TV_BOT(0));
		}
		ਜਦੋਂ (ectx->ec_instr[ectx->ec_iidx].isn_type != ISN_CMDMOD_REV)
		    ++ectx->ec_iidx;
	    }
	    ਜਾਰੀ;
	}
on_fatal_error:
	// Jump here for an error that messes up the stack.
	// If we are not inside a try-catch started here, abort execution.
	ਜੇ (trylevel <= ectx->ec_trylevel_at_ਸ਼ੁਰੂ)
	    ਜਾਓ theend;
    }

ਪੂਰਾ:
    ret = OK;
theend:
    may_invoke_defer_funcs(ectx);

    dict_ਢੇਰ_clear(dict_ਢੇਰ_len_at_ਸ਼ੁਰੂ);
    ectx->ec_trylevel_at_ਸ਼ੁਰੂ = save_trylevel_at_ਸ਼ੁਰੂ;
    ਵਾਪਸ ret;
}

/*
 * Execute the instructions from a VAR_INSTR typval and put the result in
 * "rettv".
 * Return OK or FAIL.
 */
    ਪੂਰਨ
exe_typval_instr(typval_T *tv, typval_T *rettv)
{
    ectx_T	*ectx = tv->vval.v_instr->instr_ectx;
    isn_T	*save_instr = ectx->ec_instr;
    ਪੂਰਨ		save_iidx = ectx->ec_iidx;
    ਪੂਰਨ		res;

    // Initialize rettv so that it is safe for caller to invoke clear_tv(rettv)
    // even when the compilation fails.
    rettv->v_type = VAR_UNKNOWN;

    ectx->ec_instr = tv->vval.v_instr->instr_instr;
    res = exec_instructions(ectx);
    ਜੇ (res == OK)
    {
	*rettv = *ਢੇਰ_TV_BOT(-1);
	--ectx->ec_ਢੇਰ.ga_len;
    }

    ectx->ec_instr = save_instr;
    ectx->ec_iidx = save_iidx;

    ਵਾਪਸ res;
}

/*
 * Execute the instructions from an ISN_SUBSTITUTE command, which are in
 * "substitute_instr".
 */
    char_u *
exe_substitute_instr(ਖਾਲੀ)
{
    ectx_T	*ectx = substitute_instr->subs_ectx;
    isn_T	*save_instr = ectx->ec_instr;
    ਪੂਰਨ		save_iidx = ectx->ec_iidx;
    char_u	*res;

    ectx->ec_instr = substitute_instr->subs_instr;
    ਜੇ (exec_instructions(ectx) == OK)
    {
	typval_T *tv = ਢੇਰ_TV_BOT(-1);

	res = typval2string(tv, TRUE);
	--ectx->ec_ਢੇਰ.ga_len;
	clear_tv(tv);
    }
    ਨਹੀਂਤਾਂ
    {
	substitute_instr->subs_ਸਥਿਤੀ = FAIL;
	res = vim_strsave((char_u *)"");
    }

    ectx->ec_instr = save_instr;
    ectx->ec_iidx = save_iidx;

    ਵਾਪਸ res;
}

/*
 * Call a "def" function from old Vim script.
 * Return OK or FAIL.
 */
    ਪੂਰਨ
call_def_function(
    ufunc_T	*ufunc,
    ਪੂਰਨ		argc_arg,	// nr of arguments
    typval_T	*argv,		// arguments
    ਪੂਰਨ		flags,		// DEF_ flags
    partial_T	*partial,	// optional partial for context
    object_T	*object,	// object, e.g. for this.Func()
    funccall_T	*funccal,
    typval_T	*rettv)		// return value
{
    ectx_T	ectx;		// execution context
    ਪੂਰਨ		argc = argc_arg;
    ਪੂਰਨ		partial_argc = partial == ਨਿਰਾ
				  || (flags & DEF_USE_PT_ARGV) == 0
							? 0 : partial->pt_argc;
    ਪੂਰਨ		ਕੁੱਲ_argc = argc + partial_argc;
    typval_T	*tv;
    ਪੂਰਨ		idx;
    ਪੂਰਨ		ret = FAIL;
    ਪੂਰਨ		defcount = ufunc->uf_ਦਲੀਲਾਂ.ga_len - ਕੁੱਲ_argc;
    sctx_T	save_current_sctx = current_sctx;
    ਪੂਰਨ		did_emsg_before = did_emsg_cumul + did_emsg;
    ਪੂਰਨ		save_suppress_errthrow = suppress_errthrow;
    msglist_T	**saved_ਸੁਨੇਹਾ_list = ਨਿਰਾ;
    msglist_T	*ਨਿੱਜੀ_ਸੁਨੇਹਾ_list = ਨਿਰਾ;
    ਪੂਰਨ		save_emsg_silent_def = emsg_silent_def;
    ਪੂਰਨ		save_did_emsg_def = did_emsg_def;
    ਪੂਰਨ		orig_funcdepth;
    ਪੂਰਨ		orig_nesting_level = ex_nesting_level;

// Get pointer to item in the stack.
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ ਢੇਰ_TV
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਢੇਰ_TV(idx) (((typval_T *)ectx.ec_ਢੇਰ.ga_ਅੰਕੜੇ) + idx)

// Get pointer to item at the bottom of the stack, -1 is the bottom.
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ ਢੇਰ_TV_BOT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਢੇਰ_TV_BOT(idx) (((typval_T *)ectx.ec_ਢੇਰ.ga_ਅੰਕੜੇ) + ectx.ec_ਢੇਰ.ga_len + idx)

// Get pointer to a local variable on the stack.  Negative for arguments.
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ ਢੇਰ_TV_VAR
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਢੇਰ_TV_VAR(idx) (((typval_T *)ectx.ec_ਢੇਰ.ga_ਅੰਕੜੇ) + ectx.ec_ਚੌਖਟਾ_idx + ਢੇਰ_ਚੌਖਟਾ_ਮਾਪ + idx)

    ਜੇ (ufunc->uf_def_ਸਥਿਤੀ == UF_NOT_COMPILED
	    || ufunc->uf_def_ਸਥਿਤੀ == UF_COMPILE_ERROR
	    || (func_needs_compiling(ufunc, get_compile_type(ufunc))
		&& compile_def_function(ufunc, FALSE,
				       get_compile_type(ufunc), ਨਿਰਾ) == FAIL))
    {
	ਜੇ (did_emsg_cumul + did_emsg == did_emsg_before)
	    semsg(_(e_function_is_not_compiled_str),
						   printable_func_name(ufunc));
	ਵਾਪਸ FAIL;
    }

    {
	// Check the function was really compiled.
	dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_ਅੰਕੜੇ)
							 + ufunc->uf_dfunc_idx;
	ਜੇ (dfunc->df_ufunc == ਨਿਰਾ)
	{
	    semsg(_(e_function_was_deleted_str), printable_func_name(ufunc));
	    ਵਾਪਸ FAIL;
	}
	ਜੇ (INSTRUCTIONS(dfunc) == ਨਿਰਾ)
	{
	    iemsg("using call_def_function() on not compiled function");
	    ਵਾਪਸ FAIL;
	}
    }

    // If depth of calling is getting too high, don't execute the function.
    orig_funcdepth = funcdepth_get();
    ਜੇ (funcdepth_increment() == FAIL)
	ਵਾਪਸ FAIL;

    CLEAR_ਖੇਤਰ(ectx);
    ectx.ec_dfunc_idx = ufunc->uf_dfunc_idx;
    ga_init2(&ectx.ec_ਢੇਰ, ਆਕਾਰ(typval_T), 500);
    ਜੇ (GA_GROW_FAILS(&ectx.ec_ਢੇਰ, 20))
    {
	funcdepth_decrement();
	ਵਾਪਸ FAIL;
    }
    ga_init2(&ectx.ec_trystack, ਆਕਾਰ(trycmd_T), 10);
    ga_init2(&ectx.ec_funcrefs, ਆਕਾਰ(partial_T *), 10);
    ectx.ec_did_emsg_before = did_emsg_before;
    ++ex_nesting_level;

    idx = ਕੁੱਲ_argc - ufunc->uf_ਦਲੀਲਾਂ.ga_len;
    ਜੇ (idx > 0 && ufunc->uf_va_name == ਨਿਰਾ)
    {
	semsg(NGETTEXT(e_one_ਦਲੀਲ_too_many, e_nr_arguments_too_many,
								    idx), idx);
	ਜਾਓ failed_early;
    }
    idx = ਕੁੱਲ_argc - ufunc->uf_ਦਲੀਲਾਂ.ga_len + ufunc->uf_def_ਦਲੀਲਾਂ.ga_len;
    ਜੇ (idx < 0)
    {
	semsg(NGETTEXT(e_one_ਦਲੀਲ_too_few, e_nr_arguments_too_few,
								  -idx), -idx);
	ਜਾਓ failed_early;
    }

    // Put values from the partial and arguments on the stack, but no more than
    // what the function expects.  A lambda can be called with more arguments
    // than it uses.
    ਲਈ (idx = 0; idx < ਕੁੱਲ_argc
	    && (ufunc->uf_va_name != ਨਿਰਾ || idx < ufunc->uf_ਦਲੀਲਾਂ.ga_len);
									 ++idx)
    {
	ਪੂਰਨ argv_idx = idx - partial_argc;

	tv = idx < partial_argc ? partial->pt_argv + idx : argv + argv_idx;
	ਜੇ (idx >= ufunc->uf_ਦਲੀਲਾਂ.ga_len - ufunc->uf_def_ਦਲੀਲਾਂ.ga_len
		&& tv->v_type == VAR_SPECIAL
		&& tv->vval.v_ਸੰਖਿਆ == VVAL_NONE)
	{
	    // Use the default value.
	    ਢੇਰ_TV_BOT(0)->v_type = VAR_UNKNOWN;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਪੂਰਨ ਪੂਰਾ = FALSE;
	    ਜੇ (ufunc->uf_arg_types != ਨਿਰਾ && idx < ufunc->uf_ਦਲੀਲਾਂ.ga_len)
	    {
		type_T *expected = ufunc->uf_arg_types[idx];
		ਜੇ (expected->tt_type == VAR_FLOAT && tv->v_type == VAR_ਸੰਖਿਆ)
		{
		    // When a float is expected and a number was given, convert
		    // the value.
		    ਢੇਰ_TV_BOT(0)->v_type = VAR_FLOAT;
		    ਢੇਰ_TV_BOT(0)->v_ਤਾਲਾ = 0;
		    ਢੇਰ_TV_BOT(0)->vval.v_float = tv->vval.v_ਸੰਖਿਆ;
		    ਪੂਰਾ = TRUE;
		}
		ਨਹੀਂਤਾਂ ਜੇ (check_typval_arg_type(expected, tv,
						   ਨਿਰਾ, argv_idx + 1) == FAIL)
		    ਜਾਓ failed_early;
	    }
	    ਜੇ (!ਪੂਰਾ)
		copy_tv(tv, ਢੇਰ_TV_BOT(0));
	}
	++ectx.ec_ਢੇਰ.ga_len;
    }

    // Turn varargs into a list.  Empty list if no args.
    ਜੇ (ufunc->uf_va_name != ਨਿਰਾ)
    {
	ਪੂਰਨ vararg_ਗਿਣਤੀ = argc - ufunc->uf_ਦਲੀਲਾਂ.ga_len;

	ਜੇ (vararg_ਗਿਣਤੀ < 0)
	    vararg_ਗਿਣਤੀ = 0;
	ਨਹੀਂਤਾਂ
	    argc -= vararg_ਗਿਣਤੀ;
	ਜੇ (exe_newlist(vararg_ਗਿਣਤੀ, &ectx) == FAIL)
	    ਜਾਓ failed_early;

	// Check the type of the list items.
	tv = ਢੇਰ_TV_BOT(-1);
	ਜੇ (ufunc->uf_va_type != ਨਿਰਾ
		&& ufunc->uf_va_type != &t_list_any
		&& ufunc->uf_va_type->tt_member != &t_any
		&& tv->vval.v_list != ਨਿਰਾ)
	{
	    type_T	*expected = ufunc->uf_va_type->tt_member;
	    listitem_T	*li = tv->vval.v_list->lv_ਪਹਿਲਾ;

	    ਲਈ (idx = 0; idx < vararg_ਗਿਣਤੀ; ++idx)
	    {
		ਜੇ (check_typval_arg_type(expected, &li->li_tv,
						 ਨਿਰਾ, argc + idx + 1) == FAIL)
		    ਜਾਓ failed_early;
		li = li->li_ਅਗਲਾ;
	    }
	}

	ਜੇ (defcount > 0)
	    // Move varargs list to below missing default arguments.
	    *ਢੇਰ_TV_BOT(defcount - 1) = *ਢੇਰ_TV_BOT(-1);
	--ectx.ec_ਢੇਰ.ga_len;
    }

    // Make space for omitted arguments, will store default value below.
    // Any varargs list goes after them.
    ਜੇ (defcount > 0)
	ਲਈ (idx = 0; idx < defcount; ++idx)
	{
	    ਢੇਰ_TV_BOT(0)->v_type = VAR_UNKNOWN;
	    ++ectx.ec_ਢੇਰ.ga_len;
	}
    ਜੇ (ufunc->uf_va_name != ਨਿਰਾ)
	++ectx.ec_ਢੇਰ.ga_len;

    // Frame pointer points to just after arguments.
    ectx.ec_ਚੌਖਟਾ_idx = ectx.ec_ਢੇਰ.ga_len;
    ectx.ec_initial_ਚੌਖਟਾ_idx = ectx.ec_ਚੌਖਟਾ_idx;

    {
	dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_ਅੰਕੜੇ)
							 + ufunc->uf_dfunc_idx;
	ufunc_T *ਅਧਾਰ_ufunc = dfunc->df_ufunc;

	// "uf_partial" is on the ufunc that "df_ufunc" points to, as is done
	// by copy_lambda_to_global_func().
	ਜੇ (partial != ਨਿਰਾ || ਅਧਾਰ_ufunc->uf_partial != ਨਿਰਾ)
	{
	    ectx.ec_outer_ref = ALLOC_CLEAR_ONE(outer_ref_T);
	    ਜੇ (ectx.ec_outer_ref == ਨਿਰਾ)
		ਜਾਓ failed_early;
	    ਜੇ (partial != ਨਿਰਾ)
	    {
		outer_T *outer = get_pt_outer(partial);

		ਜੇ (outer->ਬਾਹਰਲਾ_ਢੇਰ == ਨਿਰਾ && outer->ਬਾਹਰਲਾ_loop_ਮਾਪ == 0)
		{
		    // no stack was set
		    ਜੇ (current_ectx != ਨਿਰਾ)
		    {
			ਜੇ (current_ectx->ec_outer_ref != ਨਿਰਾ
			       && current_ectx->ec_outer_ref->or_outer != ਨਿਰਾ)
			    ectx.ec_outer_ref->or_outer =
					  current_ectx->ec_outer_ref->or_outer;
		    }
		    // else: should there be an error here?
		}
		ਨਹੀਂਤਾਂ
		{
		    ectx.ec_outer_ref->or_outer = outer;
		    ++partial->pt_refcount;
		    ectx.ec_outer_ref->or_partial = partial;
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ectx.ec_outer_ref->or_outer = &ਅਧਾਰ_ufunc->uf_partial->pt_outer;
		++ਅਧਾਰ_ufunc->uf_partial->pt_refcount;
		ectx.ec_outer_ref->or_partial = ਅਧਾਰ_ufunc->uf_partial;
	    }
	}
    }

    // dummy frame entries
    ਲਈ (idx = 0; idx < ਢੇਰ_ਚੌਖਟਾ_ਮਾਪ; ++idx)
    {
	ਢੇਰ_TV(ectx.ec_ਢੇਰ.ga_len)->v_type = VAR_UNKNOWN;
	++ectx.ec_ਢੇਰ.ga_len;
    }

    {
	// Reserve space for local variables and any closure reference count.
	dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_ਅੰਕੜੇ)
							 + ufunc->uf_dfunc_idx;

	// Initialize variables to zero.  That avoids having to generate
	// initializing instructions for "var nr: number", "var x: any", etc.
	ਲਈ (idx = 0; idx < dfunc->df_varcount; ++idx)
	{
	    ਢੇਰ_TV_VAR(idx)->v_type = VAR_ਸੰਖਿਆ;
	    ਢੇਰ_TV_VAR(idx)->vval.v_ਸੰਖਿਆ = 0;
	}
	ectx.ec_ਢੇਰ.ga_len += dfunc->df_varcount;

	ਜੇ (object != ਨਿਰਾ)
	{
	    // the object is always the variable at index zero
	    tv = ਢੇਰ_TV_VAR(0);
	    tv->v_type = VAR_OBJECT;
	    tv->vval.v_object = object;
	}

	ਜੇ (dfunc->df_has_ਘੇਰਾ)
	{
	    // Initialize the variable that counts how many closures were
	    // created.  This is used in handle_closure_in_use().
	    ਢੇਰ_TV_VAR(idx)->v_type = VAR_ਸੰਖਿਆ;
	    ਢੇਰ_TV_VAR(idx)->vval.v_ਸੰਖਿਆ = 0;
	    ++ectx.ec_ਢੇਰ.ga_len;
	}

	ectx.ec_instr = INSTRUCTIONS(dfunc);
    }

    // Store the execution context in funccal, used by invoke_all_defer().
    ਜੇ (funccal != ਨਿਰਾ)
	funccal->fc_ectx = &ectx;

    // Following errors are in the function, not the caller.
    // Commands behave like vim9script.
    estack_ਧੱਕੋ_ufunc(ufunc, 1);
    current_sctx = ufunc->uf_script_ctx;
    current_sctx.sc_ਸੰਸਕਰਣ = SCRIPT_ਸੰਸਕਰਣ_VIM9;

    // Use a specific location for storing error messages to be converted to an
    // exception.
    saved_ਸੁਨੇਹਾ_list = ਸੁਨੇਹਾ_list;
    ਸੁਨੇਹਾ_list = &ਨਿੱਜੀ_ਸੁਨੇਹਾ_list;

    // Do turn errors into exceptions.
    suppress_errthrow = FALSE;

    // Do not delete the function while executing it.
    ++ufunc->uf_calls;

    // When ":silent!" was used before calling then we still abort the
    // function.  If ":silent!" is used in the function then we don't.
    emsg_silent_def = emsg_silent;
    did_emsg_def = 0;

    ectx.ec_where = (where_T)WHERE_INIT;

    /*
     * Execute the instructions until done.
     */
    ret = exec_instructions(&ectx);
    ਜੇ (ret == OK)
    {
	// function finished, get result from the stack.
	ਜੇ (ufunc->uf_ret_type == &t_void)
	{
	    rettv->v_type = VAR_VOID;
	}
	ਨਹੀਂਤਾਂ
	{
	    tv = ਢੇਰ_TV_BOT(-1);
	    *rettv = *tv;
	    tv->v_type = VAR_UNKNOWN;
	}
    }

    // When failed need to unwind the call stack.
    unwind_def_callstack(&ectx);

    // Deal with any remaining closures, they may be in use somewhere.
    ਜੇ (ectx.ec_funcrefs.ga_len > 0)
    {
	handle_ਘੇਰਾ_ਵਿੱਚ_use(&ectx, FALSE);
	ga_clear(&ectx.ec_funcrefs);
    }

    estack_ਕੱਢੋ();
    current_sctx = save_current_sctx;

    ਜੇ (--ufunc->uf_calls <= 0 && ufunc->uf_refcount <= 0)
	// Function was unreferenced while being used, free it now.
	func_clear_free(ufunc, FALSE);

    ਜੇ (*ਸੁਨੇਹਾ_list != ਨਿਰਾ && saved_ਸੁਨੇਹਾ_list != ਨਿਰਾ)
    {
	msglist_T **plist = saved_ਸੁਨੇਹਾ_list;

	// Append entries from the current msg_list (uncaught exceptions) to
	// the saved msg_list.
	ਜਦੋਂ (*plist != ਨਿਰਾ)
	    plist = &(*plist)->ਅਗਲਾ;

	*plist = *ਸੁਨੇਹਾ_list;
    }
    ਸੁਨੇਹਾ_list = saved_ਸੁਨੇਹਾ_list;

    ਜੇ (ectx.ec_funclocal.floc_restore_cmdmod)
    {
	cmdmod.cmod_filter_regmatch.regprog = ਨਿਰਾ;
	undo_cmdmod(&cmdmod);
	cmdmod = ectx.ec_funclocal.floc_save_cmdmod;
    }
    emsg_silent_def = save_emsg_silent_def;
    did_emsg_def += save_did_emsg_def;

failed_early:
    // Free all arguments and local variables.
    ਲਈ (idx = 0; idx < ectx.ec_ਢੇਰ.ga_len; ++idx)
    {
	tv = ਢੇਰ_TV(idx);
	ਜੇ (tv->v_type != VAR_ਸੰਖਿਆ && tv->v_type != VAR_UNKNOWN)
	    clear_tv(tv);
    }
    ex_nesting_level = orig_nesting_level;

    vim_free(ectx.ec_ਢੇਰ.ga_ਅੰਕੜੇ);
    vim_free(ectx.ec_trystack.ga_ਅੰਕੜੇ);
    ਜੇ (ectx.ec_outer_ref != ਨਿਰਾ)
    {
	ਜੇ (ectx.ec_outer_ref->or_outer_allocated)
	    vim_free(ectx.ec_outer_ref->or_outer);
	partial_unref(ectx.ec_outer_ref->or_partial);
	vim_free(ectx.ec_outer_ref);
    }

    // Not sure if this is necessary.
    suppress_errthrow = save_suppress_errthrow;

    ਜੇ (ret != OK && did_emsg_cumul + did_emsg == did_emsg_before
							      && !need_rethrow)
	semsg(_(e_unknown_error_while_executing_str),
						   printable_func_name(ufunc));
    funcdepth_restore(orig_funcdepth);
    ਵਾਪਸ ret;
}

/*
 * Called when a def function has finished (possibly failed).
 * Invoke all the function returns to clean up and invoke deferred functions,
 * except the toplevel one.
 */
    ਖਾਲੀ
unwind_def_callstack(ectx_T *ectx)
{
    ਜਦੋਂ (ectx->ec_ਚੌਖਟਾ_idx != ectx->ec_initial_ਚੌਖਟਾ_idx)
	func_return(ectx);
}

/*
 * Invoke any deferred functions for the top function in "ectx".
 */
    ਖਾਲੀ
may_invoke_defer_funcs(ectx_T *ectx)
{
    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_ਅੰਕੜੇ) + ectx->ec_dfunc_idx;

    ਜੇ (dfunc->df_defer_var_idx > 0)
	invoke_defer_funcs(ectx);
}

/*
 * Return loopvarinfo in a printable form in allocated memory.
 */
    ਸਥਿਰ char_u *
printable_loopvarinfo(loopvarinfo_T *lvi)
{
    garray_T	ga;
    ਪੂਰਨ		ਡੂੰਘਾਈ;

    ga_init2(&ga, 1, 100);
    ਲਈ (ਡੂੰਘਾਈ = 0; ਡੂੰਘਾਈ < lvi->lvi_ਡੂੰਘਾਈ; ++ਡੂੰਘਾਈ)
    {
	ਜੇ (ga_grow(&ga, 50) == FAIL)
	    ਰੋਕੋ;
	ਜੇ (lvi->lvi_loop[ਡੂੰਘਾਈ].var_idx == 0)
	    STRCPY((ਅੱਖਰ *)ga.ga_ਅੰਕੜੇ + ga.ga_len, " -");
	ਨਹੀਂਤਾਂ
	    vim_snprintf((ਅੱਖਰ *)ga.ga_ਅੰਕੜੇ + ga.ga_len, 50, " $%d-$%d",
			    lvi->lvi_loop[ਡੂੰਘਾਈ].var_idx,
			    lvi->lvi_loop[ਡੂੰਘਾਈ].var_idx
					 + lvi->lvi_loop[ਡੂੰਘਾਈ].var_ਗਿਣਤੀ - 1);
	ga.ga_len = (ਪੂਰਨ)STRLEN(ga.ga_ਅੰਕੜੇ);
    }
    ਵਾਪਸ ga.ga_ਅੰਕੜੇ;
}

/*
 * List instructions "instr" up to "instr_count" or until ISN_FINISH.
 * "ufunc" has the source lines, NULL for the instructions of ISN_SUBSTITUTE.
 * "pfx" is prefixed to every line.
 */
    ਸਥਿਰ ਖਾਲੀ
list_instructions(ਅੱਖਰ *pfx, isn_T *instr, ਪੂਰਨ instr_ਗਿਣਤੀ, ufunc_T *ufunc)
{
    ਪੂਰਨ		ਸਤਰ_idx = 0;
    ਪੂਰਨ		prev_current = 0;
    ਪੂਰਨ		current;
    ਪੂਰਨ		def_arg_idx = 0;
    sctx_T	script_ctx = current_sctx;

    ਲਈ (current = 0; current < instr_ਗਿਣਤੀ; ++current)
    {
	isn_T	    *iptr = &instr[current];
	ਅੱਖਰ	    *ਸਤਰ;

	ਜੇ (ufunc != ਨਿਰਾ)
	{
	    ਜਦੋਂ (ਸਤਰ_idx < iptr->isn_lnum
					  && ਸਤਰ_idx < ufunc->uf_ਸਤਰਾਂ.ga_len)
	    {
		ਜੇ (current > prev_current)
		{
		    ਸੁਨੇਹਾ_puts("\n\n");
		    prev_current = current;
		}
		ਸਤਰ = ((ਅੱਖਰ **)ufunc->uf_ਸਤਰਾਂ.ga_ਅੰਕੜੇ)[ਸਤਰ_idx++];
		ਜੇ (ਸਤਰ != ਨਿਰਾ)
		    ਸੁਨੇਹਾ(ਸਤਰ);
	    }
	    ਜੇ (iptr->isn_type == ISN_JUMP_IF_ARG_ਸੈੱਟ)
	    {
		ਪੂਰਨ	ਪਹਿਲਾ_def_arg = ufunc->uf_ਦਲੀਲਾਂ.ga_len
						   - ufunc->uf_def_ਦਲੀਲਾਂ.ga_len;

		ਜੇ (def_arg_idx > 0)
		    ਸੁਨੇਹਾ_puts("\n\n");
		ਸੁਨੇਹਾ_ਸ਼ੁਰੂ();
		ਸੁਨੇਹਾ_puts("  ");
		ਸੁਨੇਹਾ_puts(((ਅੱਖਰ **)(ufunc->uf_ਦਲੀਲਾਂ.ga_ਅੰਕੜੇ))[
						 ਪਹਿਲਾ_def_arg + def_arg_idx]);
		ਸੁਨੇਹਾ_puts(" = ");
		ਸੁਨੇਹਾ_puts(((ਅੱਖਰ **)(ufunc->uf_def_ਦਲੀਲਾਂ.ga_ਅੰਕੜੇ))[def_arg_idx++]);
		ਸੁਨੇਹਾ_clr_eos();
		ਸੁਨੇਹਾ_ਸਿਰਾ();
	    }
	}

	ਚੋਣ (iptr->isn_type)
	{
	    ਹਾਲਤ ISN_CONSTRUCT:
		smsg("%s%4d NEW %s size %d", pfx, current,
			iptr->isn_arg.construct.construct_class->class_name.string,
				  (ਪੂਰਨ)iptr->isn_arg.construct.construct_ਮਾਪ);
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_EXEC:
		smsg("%s%4d EXEC %s", pfx, current, iptr->isn_arg.string);
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_EXEC_ਵੰਡੋ:
		smsg("%s%4d EXEC_SPLIT %s", pfx, current, iptr->isn_arg.string);
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_EXECRANGE:
		smsg("%s%4d EXECRANGE %s", pfx, current, iptr->isn_arg.string);
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_LEGACY_EVAL:
		smsg("%s%4d EVAL legacy %s", pfx, current,
							 iptr->isn_arg.string);
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_REDIRSTART:
		smsg("%s%4d REDIR", pfx, current);
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_REDIREND:
		smsg("%s%4d REDIR END%s", pfx, current,
					iptr->isn_arg.ਸੰਖਿਆ ? " append" : "");
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_CEXPR_AUCMD:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
		smsg("%s%4d CEXPR pre %s", pfx, current,
				       cexpr_get_auname(iptr->isn_arg.ਸੰਖਿਆ));
#ਜੇ_ਅੰਤ
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_CEXPR_CORE:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
		{
		    cexprref_T	    *cer = iptr->isn_arg.cexpr.cexpr_ref;

		    smsg("%s%4d CEXPR core %s%s \"%s\"", pfx, current,
				       cexpr_get_auname(cer->cer_cmdidx),
				       cer->cer_forceit ? "!" : "",
				       cer->cer_cmdline);
		}
#ਜੇ_ਅੰਤ
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_INSTR:
		smsg("%s%4d INSTR", pfx, current);
		list_instructions("    ", iptr->isn_arg.instr, ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ, ਨਿਰਾ);
		ਸੁਨੇਹਾ("     -------------");
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_ਸਰੋਤ:
		{
		    scriptitem_T    *si = SCRIPT_ਵਸਤੂ(iptr->isn_arg.ਸੰਖਿਆ);

		    smsg("%s%4d SOURCE %s", pfx, current, si->sn_name);
		}
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_SUBSTITUTE:
		{
		    subs_T *subs = &iptr->isn_arg.subs;

		    smsg("%s%4d SUBSTITUTE %s", pfx, current, subs->subs_cmd);
		    list_instructions("    ", subs->subs_instr, ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ, ਨਿਰਾ);
		    ਸੁਨੇਹਾ("     -------------");
		}
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_EXECCONCAT:
		smsg("%s%4d EXECCONCAT %lld", pfx, current,
					      (varnumber_T)iptr->isn_arg.ਸੰਖਿਆ);
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_ECHO:
		{
		    echo_T *echo = &iptr->isn_arg.echo;

		    smsg("%s%4d %s %d", pfx, current,
			    echo->echo_with_white ? "ECHO" : "ECHON",
			    echo->echo_ਗਿਣਤੀ);
		}
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_EXECUTE:
		smsg("%s%4d EXECUTE %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.ਸੰਖਿਆ));
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_ECHOMSG:
		smsg("%s%4d ECHOMSG %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.ਸੰਖਿਆ));
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_ECHOWINDOW:
		ਜੇ (iptr->isn_arg.echowin.ewin_ਸਮਾਂ > 0)
		    smsg("%s%4d ECHOWINDOW %d (%ld sec)", pfx, current,
				      iptr->isn_arg.echowin.ewin_ਗਿਣਤੀ,
				      iptr->isn_arg.echowin.ewin_ਸਮਾਂ);
		ਨਹੀਂਤਾਂ
		    smsg("%s%4d ECHOWINDOW %d", pfx, current,
					     iptr->isn_arg.echowin.ewin_ਗਿਣਤੀ);
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_ECHOCONSOLE:
		smsg("%s%4d ECHOCONSOLE %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.ਸੰਖਿਆ));
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_ECHOERR:
		smsg("%s%4d ECHOERR %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.ਸੰਖਿਆ));
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_ਲੱਦੋ:
		{
		    ਜੇ (iptr->isn_arg.ਸੰਖਿਆ < 0)
			smsg("%s%4d LOAD arg[%lld]", pfx, current,
				(varnumber_T)(iptr->isn_arg.ਸੰਖਿਆ
							  + ਢੇਰ_ਚੌਖਟਾ_ਮਾਪ));
		    ਨਹੀਂਤਾਂ
			smsg("%s%4d LOAD $%lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.ਸੰਖਿਆ));
		}
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_LOADOUTER:
		{
		    isn_outer_T *outer = &iptr->isn_arg.outer;

		    ਜੇ (outer->outer_idx < 0)
			smsg("%s%4d LOADOUTER level %d arg[%d]", pfx, current,
					outer->outer_ਡੂੰਘਾਈ,
					outer->outer_idx + ਢੇਰ_ਚੌਖਟਾ_ਮਾਪ);
		    ਨਹੀਂਤਾਂ ਜੇ (outer->outer_ਡੂੰਘਾਈ < 0)
			smsg("%s%4d LOADOUTER $%d in loop level %d",
					       pfx, current,
					       outer->outer_idx,
					       -outer->outer_ਡੂੰਘਾਈ);
		    ਨਹੀਂਤਾਂ
			smsg("%s%4d LOADOUTER level %d $%d", pfx, current,
					      outer->outer_ਡੂੰਘਾਈ,
					      outer->outer_idx);
		}
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_LOADV:
		smsg("%s%4d LOADV v:%s", pfx, current,
				       get_vim_var_name(iptr->isn_arg.ਸੰਖਿਆ));
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_LOADSCRIPT:
		{
		    scriptref_T	    *sref = iptr->isn_arg.script.scriptref;
		    scriptitem_T    *si = SCRIPT_ਵਸਤੂ(sref->sref_sid);
		    svar_T	    *sv;
		    sctx_T	    save_sctx = current_sctx;

		    current_sctx = script_ctx;
		    sv = get_script_svar(sref, -1);
		    current_sctx = save_sctx;
		    ਜੇ (sv == ਨਿਰਾ)
			smsg("%s%4d LOADSCRIPT [deleted] from %s",
						    pfx, current, si->sn_name);
		    ਨਹੀਂਤਾਂ
			smsg("%s%4d LOADSCRIPT %s-%d from %s", pfx, current,
					    sv->sv_name,
					    sref->sref_idx,
					    si->sn_name);
		}
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_LOADS:
	    ਹਾਲਤ ISN_LOADEXPORT:
		{
		    scriptitem_T *si = SCRIPT_ਵਸਤੂ(
					       iptr->isn_arg.loadstore.ls_sid);

		    smsg("%s%4d %s s:%s from %s", pfx, current,
			    iptr->isn_type == ISN_LOADS ? "LOADS"
								: "LOADEXPORT",
			    iptr->isn_arg.loadstore.ls_name, si->sn_name);
		}
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_LOADAUTO:
		smsg("%s%4d LOADAUTO %s", pfx, current, iptr->isn_arg.string);
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_LOADG:
		smsg("%s%4d LOADG g:%s", pfx, current, iptr->isn_arg.string);
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_LOADB:
		smsg("%s%4d LOADB b:%s", pfx, current, iptr->isn_arg.string);
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_LOADW:
		smsg("%s%4d LOADW w:%s", pfx, current, iptr->isn_arg.string);
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_LOADT:
		smsg("%s%4d LOADT t:%s", pfx, current, iptr->isn_arg.string);
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_LOADGDICT:
		smsg("%s%4d LOAD g:", pfx, current);
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_LOADBDICT:
		smsg("%s%4d LOAD b:", pfx, current);
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_LOADWDICT:
		smsg("%s%4d LOAD w:", pfx, current);
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_LOADTDICT:
		smsg("%s%4d LOAD t:", pfx, current);
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_LOADOPT:
		smsg("%s%4d LOADOPT %s", pfx, current, iptr->isn_arg.string);
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_LOADENV:
		smsg("%s%4d LOADENV %s", pfx, current, iptr->isn_arg.string);
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_LOADREG:
		smsg("%s%4d LOADREG @%c", pfx, current,
						  (ਪੂਰਨ)(iptr->isn_arg.ਸੰਖਿਆ));
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_STORE:
		ਜੇ (iptr->isn_arg.ਸੰਖਿਆ < 0)
		    smsg("%s%4d STORE arg[%lld]", pfx, current,
				      iptr->isn_arg.ਸੰਖਿਆ + ਢੇਰ_ਚੌਖਟਾ_ਮਾਪ);
		ਨਹੀਂਤਾਂ
		    smsg("%s%4d STORE $%lld", pfx, current,
							 iptr->isn_arg.ਸੰਖਿਆ);
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_STOREOUTER:
		{
		    isn_outer_T *outer = &iptr->isn_arg.outer;

		    ਜੇ (outer->outer_ਡੂੰਘਾਈ < 0)
			smsg("%s%4d STOREOUTER $%d in loop level %d",
				pfx, current, outer->outer_idx,
				-outer->outer_ਡੂੰਘਾਈ);
		    ਨਹੀਂਤਾਂ
			smsg("%s%4d STOREOUTER level %d $%d", pfx, current,
				outer->outer_ਡੂੰਘਾਈ, outer->outer_idx);
		}
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_STOREV:
		smsg("%s%4d STOREV v:%s", pfx, current,
				       get_vim_var_name(iptr->isn_arg.ਸੰਖਿਆ));
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_STOREAUTO:
		smsg("%s%4d STOREAUTO %s", pfx, current, iptr->isn_arg.string);
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_STOREG:
		smsg("%s%4d STOREG %s", pfx, current, iptr->isn_arg.string);
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_STOREB:
		smsg("%s%4d STOREB %s", pfx, current, iptr->isn_arg.string);
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_STOREW:
		smsg("%s%4d STOREW %s", pfx, current, iptr->isn_arg.string);
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_STORET:
		smsg("%s%4d STORET %s", pfx, current, iptr->isn_arg.string);
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_STORES:
	    ਹਾਲਤ ISN_STOREEXPORT:
		{
		    scriptitem_T *si = SCRIPT_ਵਸਤੂ(
					       iptr->isn_arg.loadstore.ls_sid);

		    smsg("%s%4d %s %s in %s", pfx, current,
				iptr->isn_type == ISN_STORES
						    ? "STORES" : "STOREEXPORT",
				 iptr->isn_arg.loadstore.ls_name, si->sn_name);
		}
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_STORESCRIPT:
		{
		    scriptref_T	    *sref = iptr->isn_arg.script.scriptref;
		    scriptitem_T    *si = SCRIPT_ਵਸਤੂ(sref->sref_sid);
		    svar_T	    *sv;
		    sctx_T	    save_sctx = current_sctx;

		    current_sctx = script_ctx;
		    sv = get_script_svar(sref, -1);
		    current_sctx = save_sctx;
		    ਜੇ (sv == ਨਿਰਾ)
			smsg("%s%4d STORESCRIPT [deleted] in %s",
						    pfx, current, si->sn_name);
		    ਨਹੀਂਤਾਂ
			smsg("%s%4d STORESCRIPT %s-%d in %s", pfx, current,
					     sv->sv_name,
					     sref->sref_idx,
					     si->sn_name);
		}
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_STOREOPT:
	    ਹਾਲਤ ISN_STOREFUNCOPT:
		smsg("%s%4d %s &%s", pfx, current,
		  iptr->isn_type == ISN_STOREOPT ? "STOREOPT" : "STOREFUNCOPT",
					       iptr->isn_arg.storeopt.so_name);
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_STOREENV:
		smsg("%s%4d STOREENV $%s", pfx, current, iptr->isn_arg.string);
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_STOREREG:
		smsg("%s%4d STOREREG @%c", pfx, current,
						    (ਪੂਰਨ)iptr->isn_arg.ਸੰਖਿਆ);
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_STORENR:
		smsg("%s%4d STORE %lld in $%d", pfx, current,
				iptr->isn_arg.storenr.stnr_val,
				iptr->isn_arg.storenr.stnr_idx);
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_STOREINDEX:
		smsg("%s%4d STOREINDEX %s", pfx, current,
			    vartype_name(iptr->isn_arg.storeindex.si_vartype));
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_STORERANGE:
		smsg("%s%4d STORERANGE", pfx, current);
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_ਲੱਦੋ_CLASSMEMBER:
	    ਹਾਲਤ ISN_STORE_CLASSMEMBER:
		{
		    class_T *cl = iptr->isn_arg.classmember.cm_class;
		    ਪੂਰਨ	    idx = iptr->isn_arg.classmember.cm_idx;
		    ocmember_T *ocm = &cl->class_class_members[idx];
		    smsg("%s%4d %s CLASSMEMBER %s.%s", pfx, current,
			    iptr->isn_type == ISN_ਲੱਦੋ_CLASSMEMBER
							    ? "LOAD" : "STORE",
			    cl->class_name.string, ocm->ocm_name.string);
		}
		ਰੋਕੋ;

	    // constants
	    ਹਾਲਤ ISN_PUSHNR:
		smsg("%s%4d PUSHNR %lld", pfx, current,
					    (varnumber_T)(iptr->isn_arg.ਸੰਖਿਆ));
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_PUSHBOOL:
	    ਹਾਲਤ ISN_PUSHSPEC:
		smsg("%s%4d PUSH %s", pfx, current,
				   get_var_special_name(iptr->isn_arg.ਸੰਖਿਆ));
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_PUSHF:
		smsg("%s%4d PUSHF %g", pfx, current, iptr->isn_arg.fnumber);
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_PUSHS:
		smsg("%s%4d PUSHS \"%s\"", pfx, current, iptr->isn_arg.string);
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_PUSHBLOB:
		{
		    char_u	*r;
		    char_u	numbuf[NUMBUFLEN];
		    char_u	*tofree;

		    r = blob2string(iptr->isn_arg.blob, &tofree, numbuf);
		    smsg("%s%4d PUSHBLOB %s", pfx, current, r);
		    vim_free(tofree);
		}
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_PUSHFUNC:
		{
		    ਅੱਖਰ *name = (ਅੱਖਰ *)iptr->isn_arg.string;

		    smsg("%s%4d PUSHFUNC \"%s\"", pfx, current,
					       name == ਨਿਰਾ ? "[none]" : name);
		}
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_PUSHCHANNEL:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
		smsg("%s%4d PUSHCHANNEL 0", pfx, current);
#ਜੇ_ਅੰਤ
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_PUSHJOB:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
		smsg("%s%4d PUSHJOB \"no process\"", pfx, current);
#ਜੇ_ਅੰਤ
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_PUSHOBJ:
		smsg("%s%4d PUSHOBJ null", pfx, current);
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_PUSHCLASS:
		smsg("%s%4d PUSHCLASS %s", pfx, current,
			iptr->isn_arg.classarg == ਨਿਰਾ ? "null"
				 : (ਅੱਖਰ *)iptr->isn_arg.classarg->class_name.string);
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_PUSHEXC:
		smsg("%s%4d PUSH v:exception", pfx, current);
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_AUTOLOAD:
		smsg("%s%4d AUTOLOAD %s", pfx, current, iptr->isn_arg.string);
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_UNLET:
		smsg("%s%4d UNLET%s %s", pfx, current,
			iptr->isn_arg.unlet.ul_forceit ? "!" : "",
			iptr->isn_arg.unlet.ul_name);
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_UNLETENV:
		smsg("%s%4d UNLETENV%s $%s", pfx, current,
			iptr->isn_arg.unlet.ul_forceit ? "!" : "",
			iptr->isn_arg.unlet.ul_name);
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_UNLETINDEX:
		smsg("%s%4d UNLETINDEX", pfx, current);
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_UNLETRANGE:
		smsg("%s%4d UNLETRANGE", pfx, current);
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_LOCKUNLOCK:
		smsg("%s%4d LOCKUNLOCK %s", pfx, current, iptr->isn_arg.string);
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_LOCKCONST:
		smsg("%s%4d LOCKCONST", pfx, current);
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_NEWLIST:
		smsg("%s%4d NEWLIST size %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.ਸੰਖਿਆ));
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_NEWTUPLE:
		smsg("%s%4d NEWTUPLE size %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.ਸੰਖਿਆ));
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_NEWDICT:
		smsg("%s%4d NEWDICT size %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.ਸੰਖਿਆ));
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_NEWPARTIAL:
		smsg("%s%4d NEWPARTIAL", pfx, current);
		ਰੋਕੋ;

	    // function call
	    ਹਾਲਤ ISN_BCALL:
		{
		    cbfunc_T	*cbfunc = &iptr->isn_arg.bfunc;

		    smsg("%s%4d BCALL %s(argc %d)", pfx, current,
			    ਅੰਦਰੂਨੀ_func_name(cbfunc->cbf_idx),
			    cbfunc->cbf_argcount);
		}
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_DCALL:
		{
		    cdfunc_T	*cdfunc = &iptr->isn_arg.dfunc;
		    dfunc_T	*df = ((dfunc_T *)def_functions.ga_ਅੰਕੜੇ)
							     + cdfunc->cdf_idx;

		    smsg("%s%4d DCALL %s(argc %d)", pfx, current,
					    printable_func_name(df->df_ufunc),
							 cdfunc->cdf_argcount);
		}
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_METHODCALL:
		{
		    cmfunc_T	*mfunc = iptr->isn_arg.mfunc;

		    smsg("%s%4d METHODCALL %s.%s(argc %d)", pfx, current,
			    mfunc->cmf_itf->class_name.string,
			    mfunc->cmf_ufunc != ਨਿਰਾ
				? mfunc->cmf_ufunc->uf_name
				: mfunc->cmf_itf->class_obj_methods[
						      mfunc->cmf_idx]->uf_name,
			    mfunc->cmf_argcount);
		}
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_UCALL:
		{
		    cufunc_T	*cufunc = &iptr->isn_arg.ufunc;

		    smsg("%s%4d UCALL %s(argc %d)", pfx, current,
				       cufunc->cuf_name, cufunc->cuf_argcount);
		}
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_PCALL:
		{
		    cpfunc_T	*cpfunc = &iptr->isn_arg.pfunc;

		    smsg("%s%4d PCALL%s (argc %d)", pfx, current,
			   cpfunc->cpf_ਸਿਖਰ ? " top" : "", cpfunc->cpf_argcount);
		}
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_PCALL_ਸਿਰਾ:
		smsg("%s%4d PCALL end", pfx, current);
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_DEFER:
		smsg("%s%4d DEFER %d args", pfx, current,
				      (ਪੂਰਨ)iptr->isn_arg.defer.defer_argcount);
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_RETURN:
		smsg("%s%4d RETURN", pfx, current);
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_RETURN_VOID:
		smsg("%s%4d RETURN void", pfx, current);
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_RETURN_OBJECT:
		smsg("%s%4d RETURN object", pfx, current);
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_FUNCREF:
		{
		    funcref_T		*funcref = &iptr->isn_arg.funcref;
		    funcref_extra_T	*extra = funcref->fr_extra;
		    char_u		*name;

		    ਜੇ (extra == ਨਿਰਾ || extra->fre_func_name == ਨਿਰਾ)
		    {
			dfunc_T	*df = ((dfunc_T *)def_functions.ga_ਅੰਕੜੇ)
						       + funcref->fr_dfunc_idx;
			name = df->df_ufunc->uf_name;
		    }
		    ਨਹੀਂਤਾਂ
			name = extra->fre_func_name;
		    ਜੇ (extra != ਨਿਰਾ && extra->fre_class != ਨਿਰਾ)
		    {
			smsg("%s%4d FUNCREF %s.%s", pfx, current,
					   extra->fre_class->class_name.string, name);
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (extra == ਨਿਰਾ
				     || extra->fre_loopvar_info.lvi_ਡੂੰਘਾਈ == 0)
		    {
			smsg("%s%4d FUNCREF %s", pfx, current, name);
		    }
		    ਨਹੀਂਤਾਂ
		    {
			char_u	*info = printable_loopvarinfo(
						     &extra->fre_loopvar_info);

			smsg("%s%4d FUNCREF %s vars %s", pfx, current,
								   name, info);
			vim_free(info);
		    }
		}
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_NEWFUNC:
		{
		    newfuncarg_T    *arg = iptr->isn_arg.newfunc.nf_arg;

		    ਜੇ (arg->nfa_loopvar_info.lvi_ਡੂੰਘਾਈ == 0)
			smsg("%s%4d NEWFUNC %s %s", pfx, current,
					     arg->nfa_lambda, arg->nfa_global);
		    ਨਹੀਂਤਾਂ
		    {
			char_u	*info = printable_loopvarinfo(
						       &arg->nfa_loopvar_info);

			smsg("%s%4d NEWFUNC %s %s vars %s", pfx, current,
				       arg->nfa_lambda, arg->nfa_global, info);
			vim_free(info);
		    }
		}
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_DEF:
		{
		    char_u *name = iptr->isn_arg.string;

		    smsg("%s%4d DEF %s", pfx, current,
					   name == ਨਿਰਾ ? (char_u *)"" : name);
		}
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_JUMP:
		{
		    ਅੱਖਰ *when = "?";

		    ਚੋਣ (iptr->isn_arg.jump.jump_when)
		    {
			ਹਾਲਤ JUMP_ALWAYS:
			    when = "JUMP";
			    ਰੋਕੋ;
			ਹਾਲਤ JUMP_NEVER:
			    iemsg("JUMP_NEVER should not be used");
			    ਰੋਕੋ;
			ਹਾਲਤ JUMP_AND_KEEP_IF_TRUE:
			    when = "JUMP_AND_KEEP_IF_TRUE";
			    ਰੋਕੋ;
			ਹਾਲਤ JUMP_IF_FALSE:
			    when = "JUMP_IF_FALSE";
			    ਰੋਕੋ;
			ਹਾਲਤ JUMP_WHILE_FALSE:
			    when = "JUMP_WHILE_FALSE";  //ਅਣਵਰਤਿਆ
			    ਰੋਕੋ;
			ਹਾਲਤ JUMP_IF_COND_FALSE:
			    when = "JUMP_IF_COND_FALSE";
			    ਰੋਕੋ;
			ਹਾਲਤ JUMP_IF_COND_TRUE:
			    when = "JUMP_IF_COND_TRUE";
			    ਰੋਕੋ;
		    }
		    smsg("%s%4d %s -> %d", pfx, current, when,
						iptr->isn_arg.jump.jump_where);
		}
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_JUMP_IF_ARG_ਸੈੱਟ:
		smsg("%s%4d JUMP_IF_ARG_SET arg[%d] -> %d", pfx, current,
			 iptr->isn_arg.jumparg.jump_arg_off + ਢੇਰ_ਚੌਖਟਾ_ਮਾਪ,
						iptr->isn_arg.jump.jump_where);
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_JUMP_IF_ARG_NOT_ਸੈੱਟ:
		smsg("%s%4d JUMP_IF_ARG_NOT_SET arg[%d] -> %d", pfx, current,
			 iptr->isn_arg.jumparg.jump_arg_off + ਢੇਰ_ਚੌਖਟਾ_ਮਾਪ,
						iptr->isn_arg.jump.jump_where);
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_FOR:
		{
		    forloop_T *forloop = &iptr->isn_arg.forloop;

		    smsg("%s%4d FOR $%d -> %d", pfx, current,
				      forloop->for_loop_idx, forloop->for_ਸਿਰਾ);
		}
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_ENDLOOP:
		{
		    endloop_T *endloop = &iptr->isn_arg.endloop;

		    smsg("%s%4d ENDLOOP ref $%d save $%d-$%d depth %d",
								  pfx, current,
			    endloop->ਸਿਰਾ_funcref_idx,
			    endloop->ਸਿਰਾ_var_idx,
			    endloop->ਸਿਰਾ_var_idx + endloop->ਸਿਰਾ_var_ਗਿਣਤੀ - 1,
			    endloop->ਸਿਰਾ_ਡੂੰਘਾਈ);
		}
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_WHILE:
		{
		    whileloop_T *whileloop = &iptr->isn_arg.whileloop;

		    smsg("%s%4d WHILE $%d -> %d", pfx, current,
					       whileloop->while_funcref_idx,
					       whileloop->while_ਸਿਰਾ);
		}
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_TRY:
		{
		    try_T *try = &iptr->isn_arg.tryref;

		    ਜੇ (try->try_ref->try_finally == 0)
			smsg("%s%4d TRY catch -> %d, endtry -> %d",
				pfx, current,
				try->try_ref->try_catch,
				try->try_ref->try_endtry);
		    ਨਹੀਂਤਾਂ
			smsg("%s%4d TRY catch -> %d, finally -> %d, endtry -> %d",
				pfx, current,
				try->try_ref->try_catch,
				try->try_ref->try_finally,
				try->try_ref->try_endtry);
		}
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_CATCH:
		smsg("%s%4d CATCH", pfx, current);
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_TRYCONT:
		{
		    trycont_T *trycont = &iptr->isn_arg.trycont;

		    smsg("%s%4d TRY-CONTINUE %d level%s -> %d", pfx, current,
				      trycont->tct_ਪੱਧਰ,
				      trycont->tct_ਪੱਧਰ == 1 ? "" : "s",
				      trycont->tct_where);
		}
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_FINALLY:
		smsg("%s%4d FINALLY", pfx, current);
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_ENDTRY:
		smsg("%s%4d ENDTRY", pfx, current);
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_THROW:
		smsg("%s%4d THROW", pfx, current);
		ਰੋਕੋ;

	    // expression operations on number
	    ਹਾਲਤ ISN_OPNR:
	    ਹਾਲਤ ISN_OPFLOAT:
	    ਹਾਲਤ ISN_OPANY:
		{
		    ਅੱਖਰ *what;
		    ਅੱਖਰ *ins;

		    ਚੋਣ (iptr->isn_arg.op.op_type)
		    {
			ਹਾਲਤ ਵਿਅੰਜਕ_MULT: what = "*"; ਰੋਕੋ;
			ਹਾਲਤ ਵਿਅੰਜਕ_DIV: what = "/"; ਰੋਕੋ;
			ਹਾਲਤ ਵਿਅੰਜਕ_REM: what = "%"; ਰੋਕੋ;
			ਹਾਲਤ ਵਿਅੰਜਕ_SUB: what = "-"; ਰੋਕੋ;
			ਹਾਲਤ ਵਿਅੰਜਕ_ਜੋੜੋ: what = "+"; ਰੋਕੋ;
			ਹਾਲਤ ਵਿਅੰਜਕ_LSHIFT: what = "<<"; ਰੋਕੋ;
			ਹਾਲਤ ਵਿਅੰਜਕ_RSHIFT: what = ">>"; ਰੋਕੋ;
			ਮੂਲ:       what = "???"; ਰੋਕੋ;
		    }
		    ਚੋਣ (iptr->isn_type)
		    {
			ਹਾਲਤ ISN_OPNR: ins = "OPNR"; ਰੋਕੋ;
			ਹਾਲਤ ISN_OPFLOAT: ins = "OPFLOAT"; ਰੋਕੋ;
			ਹਾਲਤ ISN_OPANY: ins = "OPANY"; ਰੋਕੋ;
			ਮੂਲ: ins = "???"; ਰੋਕੋ;
		    }
		    smsg("%s%4d %s %s", pfx, current, ins, what);
		}
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_COMPAREBOOL:
	    ਹਾਲਤ ISN_COMPARESPECIAL:
	    ਹਾਲਤ ISN_COMPARENULL:
	    ਹਾਲਤ ISN_COMPARENR:
	    ਹਾਲਤ ISN_COMPAREFLOAT:
	    ਹਾਲਤ ISN_COMPARESTRING:
	    ਹਾਲਤ ISN_COMPAREBLOB:
	    ਹਾਲਤ ISN_COMPARELIST:
	    ਹਾਲਤ ISN_COMPARETUPLE:
	    ਹਾਲਤ ISN_COMPAREDICT:
	    ਹਾਲਤ ISN_COMPAREFUNC:
	    ਹਾਲਤ ISN_COMPAREOBJECT:
	    ਹਾਲਤ ISN_COMPAREANY:
		   {
		       ਅੱਖਰ *p;
		       ਅੱਖਰ buf[10];
		       ਅੱਖਰ *type;

		       ਚੋਣ (iptr->isn_arg.op.op_type)
		       {
			   ਹਾਲਤ ਵਿਅੰਜਕ_EQUAL:	 p = "=="; ਰੋਕੋ;
			   ਹਾਲਤ ਵਿਅੰਜਕ_NEQUAL:    p = "!="; ਰੋਕੋ;
			   ਹਾਲਤ ਵਿਅੰਜਕ_GREATER:   p = ">"; ਰੋਕੋ;
			   ਹਾਲਤ ਵਿਅੰਜਕ_GEQUAL:    p = ">="; ਰੋਕੋ;
			   ਹਾਲਤ ਵਿਅੰਜਕ_SMALLER:   p = "<"; ਰੋਕੋ;
			   ਹਾਲਤ ਵਿਅੰਜਕ_SEQUAL:    p = "<="; ਰੋਕੋ;
			   ਹਾਲਤ ਵਿਅੰਜਕ_ਮੇਲ:	 p = "=~"; ਰੋਕੋ;
			   ਹਾਲਤ ਵਿਅੰਜਕ_IS:	 p = "is"; ਰੋਕੋ;
			   ਹਾਲਤ ਵਿਅੰਜਕ_ISNOT:	 p = "isnot"; ਰੋਕੋ;
			   ਹਾਲਤ ਵਿਅੰਜਕ_NOMATCH:	 p = "!~"; ਰੋਕੋ;
			   ਮੂਲ:  p = "???"; ਰੋਕੋ;
		       }
		       STRCPY(buf, p);
		       ਜੇ (iptr->isn_arg.op.op_ic == TRUE)
			   strcat(buf, "?");
		       ਚੋਣ(iptr->isn_type)
		       {
			   ਹਾਲਤ ISN_COMPAREBOOL: type = "COMPAREBOOL"; ਰੋਕੋ;
			   ਹਾਲਤ ISN_COMPARESPECIAL:
						 type = "COMPARESPECIAL"; ਰੋਕੋ;
			   ਹਾਲਤ ISN_COMPARENULL: type = "COMPARENULL"; ਰੋਕੋ;
			   ਹਾਲਤ ISN_COMPARENR: type = "COMPARENR"; ਰੋਕੋ;
			   ਹਾਲਤ ISN_COMPAREFLOAT: type = "COMPAREFLOAT"; ਰੋਕੋ;
			   ਹਾਲਤ ISN_COMPARESTRING:
						  type = "COMPARESTRING"; ਰੋਕੋ;
			   ਹਾਲਤ ISN_COMPAREBLOB: type = "COMPAREBLOB"; ਰੋਕੋ;
			   ਹਾਲਤ ISN_COMPARELIST: type = "COMPARELIST"; ਰੋਕੋ;
			   ਹਾਲਤ ISN_COMPARETUPLE: type = "COMPARETUPLE"; ਰੋਕੋ;
			   ਹਾਲਤ ISN_COMPAREDICT: type = "COMPAREDICT"; ਰੋਕੋ;
			   ਹਾਲਤ ISN_COMPAREFUNC: type = "COMPAREFUNC"; ਰੋਕੋ;
			   ਹਾਲਤ ISN_COMPAREOBJECT:
						 type = "COMPAREOBJECT"; ਰੋਕੋ;
			   ਹਾਲਤ ISN_COMPAREANY: type = "COMPAREANY"; ਰੋਕੋ;
			   ਮੂਲ: type = "???"; ਰੋਕੋ;
		       }

		       smsg("%s%4d %s %s", pfx, current, type, buf);
		   }
		   ਰੋਕੋ;

	    ਹਾਲਤ ISN_ADDLIST: smsg("%s%4d ADDLIST", pfx, current); ਰੋਕੋ;
	    ਹਾਲਤ ISN_ADDTUPLE: smsg("%s%4d ADDTUPLE", pfx, current); ਰੋਕੋ;
	    ਹਾਲਤ ISN_ADDBLOB: smsg("%s%4d ADDBLOB", pfx, current); ਰੋਕੋ;

	    // expression operations
	    ਹਾਲਤ ISN_CONCAT:
		smsg("%s%4d CONCAT size %lld", pfx, current,
					    (varnumber_T)(iptr->isn_arg.ਸੰਖਿਆ));
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_STRINDEX: smsg("%s%4d STRINDEX", pfx, current); ਰੋਕੋ;
	    ਹਾਲਤ ISN_STRSLICE: smsg("%s%4d STRSLICE", pfx, current); ਰੋਕੋ;
	    ਹਾਲਤ ISN_BLOBINDEX: smsg("%s%4d BLOBINDEX", pfx, current); ਰੋਕੋ;
	    ਹਾਲਤ ISN_BLOBSLICE: smsg("%s%4d BLOBSLICE", pfx, current); ਰੋਕੋ;
	    ਹਾਲਤ ISN_LISTAPPEND: smsg("%s%4d LISTAPPEND", pfx, current); ਰੋਕੋ;
	    ਹਾਲਤ ISN_BLOBAPPEND: smsg("%s%4d BLOBAPPEND", pfx, current); ਰੋਕੋ;
	    ਹਾਲਤ ISN_LISTINDEX: smsg("%s%4d LISTINDEX", pfx, current); ਰੋਕੋ;
	    ਹਾਲਤ ISN_LISTSLICE: smsg("%s%4d LISTSLICE", pfx, current); ਰੋਕੋ;
	    ਹਾਲਤ ISN_TUPLEINDEX: smsg("%s%4d TUPLEINDEX", pfx, current); ਰੋਕੋ;
	    ਹਾਲਤ ISN_TUPLESLICE: smsg("%s%4d TUPLESLICE", pfx, current); ਰੋਕੋ;
	    ਹਾਲਤ ISN_ANYINDEX: smsg("%s%4d ANYINDEX", pfx, current); ਰੋਕੋ;
	    ਹਾਲਤ ISN_ANYSLICE: smsg("%s%4d ANYSLICE", pfx, current); ਰੋਕੋ;
	    ਹਾਲਤ ISN_ਟੁਕੜਾ: smsg("%s%4d SLICE %lld",
				    pfx, current, iptr->isn_arg.ਸੰਖਿਆ); ਰੋਕੋ;
	    ਹਾਲਤ ISN_GETITEM: smsg("%s%4d ITEM %lld%s", pfx, current,
					 iptr->isn_arg.getitem.gi_ਸੂਚਕਾਂਕ,
					 iptr->isn_arg.getitem.gi_with_op ?
						       " with op" : ""); ਰੋਕੋ;
	    ਹਾਲਤ ISN_MEMBER: smsg("%s%4d MEMBER", pfx, current); ਰੋਕੋ;
	    ਹਾਲਤ ISN_STRINGMEMBER: smsg("%s%4d MEMBER %s", pfx, current,
						  iptr->isn_arg.string); ਰੋਕੋ;
	    ਹਾਲਤ ISN_GET_OBJ_MEMBER: smsg("%s%4d OBJ_MEMBER %d", pfx, current,
			     (ਪੂਰਨ)iptr->isn_arg.classmember.cm_idx);
				     ਰੋਕੋ;
	    ਹਾਲਤ ISN_GET_ITF_MEMBER: smsg("%s%4d ITF_MEMBER %d on %s",
			     pfx, current,
			     (ਪੂਰਨ)iptr->isn_arg.classmember.cm_idx,
			     iptr->isn_arg.classmember.cm_class->class_name.string);
				     ਰੋਕੋ;
	    ਹਾਲਤ ISN_STORE_THIS: smsg("%s%4d STORE_THIS %d", pfx, current,
					     (ਪੂਰਨ)iptr->isn_arg.ਸੰਖਿਆ); ਰੋਕੋ;
	    ਹਾਲਤ ISN_CLEARDICT: smsg("%s%4d CLEARDICT", pfx, current); ਰੋਕੋ;
	    ਹਾਲਤ ISN_USEDICT: smsg("%s%4d USEDICT", pfx, current); ਰੋਕੋ;

	    ਹਾਲਤ ISN_NEGATENR: smsg("%s%4d NEGATENR", pfx, current); ਰੋਕੋ;

	    ਹਾਲਤ ISN_CHECKTYPE:
		  {
		      checktype_T   *ct = &iptr->isn_arg.type;
		      ਅੱਖਰ	    *tofree = ਨਿਰਾ;
		      ਅੱਖਰ	    *typename;

		      ਜੇ (ct->ct_type->tt_type == VAR_FLOAT
			      && (ct->ct_type->tt_flags & TTFLAG_ਸੰਖਿਆ_OK))
			  typename = "float|number";
		      ਨਹੀਂਤਾਂ ਜੇ (ct->ct_type->tt_type == VAR_LIST
			      && (ct->ct_type->tt_flags & TTFLAG_TUPLE_OK))
			  typename = "list<any>|tuple<any>";
		      ਨਹੀਂਤਾਂ
			  typename = type_name(ct->ct_type, &tofree);

		      ਜੇ (ct->ct_arg_idx == 0)
			  smsg("%s%4d CHECKTYPE %s stack[%d]", pfx, current,
					  typename,
					  (ਪੂਰਨ)ct->ct_off);
		      ਨਹੀਂਤਾਂ
			  smsg("%s%4d CHECKTYPE %s stack[%d] %s %d",
					  pfx, current,
					  typename,
					  (ਪੂਰਨ)ct->ct_off,
					  ct->ct_is_var ? "var": "arg",
					  (ਪੂਰਨ)ct->ct_arg_idx);
		      vim_free(tofree);
		      ਰੋਕੋ;
		  }
	    ਹਾਲਤ ISN_CHECKLEN: smsg("%s%4d CHECKLEN %s%d", pfx, current,
				iptr->isn_arg.checklen.cl_ਹੋਰ_OK ? ">= " : "",
				iptr->isn_arg.checklen.cl_min_len);
			       ਰੋਕੋ;
	    ਹਾਲਤ ISN_SETTYPE:
		  {
		      ਅੱਖਰ *tofree;

		      smsg("%s%4d SETTYPE %s", pfx, current,
			      type_name(iptr->isn_arg.type.ct_type, &tofree));
		      vim_free(tofree);
		      ਰੋਕੋ;
		  }
	    ਹਾਲਤ ISN_COND2BOOL: smsg("%s%4d COND2BOOL", pfx, current); ਰੋਕੋ;
	    ਹਾਲਤ ISN_2BOOL: ਜੇ (iptr->isn_arg.tobool.invert)
				smsg("%s%4d INVERT %d (!val)", pfx, current,
					 iptr->isn_arg.tobool.ਵਿੱਥ);
			    ਨਹੀਂਤਾਂ
				smsg("%s%4d 2BOOL %d (!!val)", pfx, current,
					 iptr->isn_arg.tobool.ਵਿੱਥ);
			    ਰੋਕੋ;
	    ਹਾਲਤ ISN_2STRING: smsg("%s%4d 2STRING stack[%lld]", pfx, current,
				 (varnumber_T)(iptr->isn_arg.tostring.ਵਿੱਥ));
			      ਰੋਕੋ;
	    ਹਾਲਤ ISN_2STRING_ANY: smsg("%s%4d 2STRING_ANY stack[%lld]",
								  pfx, current,
				 (varnumber_T)(iptr->isn_arg.tostring.ਵਿੱਥ));
			      ਰੋਕੋ;
	    ਹਾਲਤ ISN_RANGE: smsg("%s%4d RANGE %s", pfx, current,
							 iptr->isn_arg.string);
			    ਰੋਕੋ;
	    ਹਾਲਤ ISN_PUT:
		ਜੇ (iptr->isn_arg.put.put_lnum == LNUM_VARIABLE_RANGE_ABOVE)
		    smsg("%s%4d PUT %c above range",
				  pfx, current, iptr->isn_arg.put.put_regname);
		ਨਹੀਂਤਾਂ ਜੇ (iptr->isn_arg.put.put_lnum == LNUM_VARIABLE_RANGE)
		    smsg("%s%4d PUT %c range",
				  pfx, current, iptr->isn_arg.put.put_regname);
		ਨਹੀਂਤਾਂ
		    smsg("%s%4d PUT %c %ld", pfx, current,
						 iptr->isn_arg.put.put_regname,
					     (ਲੰਮਾ)iptr->isn_arg.put.put_lnum);
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_IPUT:
		ਜੇ (iptr->isn_arg.put.put_lnum == LNUM_VARIABLE_RANGE_ABOVE)
		    smsg("%s%4d IPUT %c above range",
				  pfx, current, iptr->isn_arg.put.put_regname);
		ਨਹੀਂਤਾਂ ਜੇ (iptr->isn_arg.put.put_lnum == LNUM_VARIABLE_RANGE)
		    smsg("%s%4d IPUT %c range",
				  pfx, current, iptr->isn_arg.put.put_regname);
		ਨਹੀਂਤਾਂ
		    smsg("%s%4d IPUT %c %ld", pfx, current,
						 iptr->isn_arg.put.put_regname,
					     (ਲੰਮਾ)iptr->isn_arg.put.put_lnum);
		ਰੋਕੋ;
	    ਹਾਲਤ ISN_CMDMOD:
		{
		    char_u  *buf;
		    size_t  len = produce_cmdmods(
				  ਨਿਰਾ, iptr->isn_arg.cmdmod.cf_cmdmod, FALSE);

		    buf = alloc(len + 1);
		    ਜੇ (likely(buf != ਨਿਰਾ))
		    {
			(ਖਾਲੀ)produce_cmdmods(
				   buf, iptr->isn_arg.cmdmod.cf_cmdmod, FALSE);
			smsg("%s%4d CMDMOD %s", pfx, current, buf);
			vim_free(buf);
		    }
		    ਰੋਕੋ;
		}
	    ਹਾਲਤ ISN_CMDMOD_REV: smsg("%s%4d CMDMOD_REV", pfx, current); ਰੋਕੋ;

	    ਹਾਲਤ ISN_PROF_ਸ਼ੁਰੂ:
		 smsg("%s%4d PROFILE START line %d", pfx, current,
							       iptr->isn_lnum);
		 ਰੋਕੋ;

	    ਹਾਲਤ ISN_PROF_ਸਿਰਾ:
		smsg("%s%4d PROFILE END", pfx, current);
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_DEBUG:
		smsg("%s%4d DEBUG line %d-%d varcount %lld", pfx, current,
			iptr->isn_arg.debug.dbg_break_lnum + 1,
			iptr->isn_lnum,
			iptr->isn_arg.debug.dbg_var_ਨਾਂ_len);
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_UNPACK: smsg("%s%4d UNPACK %d%s", pfx, current,
			iptr->isn_arg.unpack.unp_ਗਿਣਤੀ,
			iptr->isn_arg.unpack.unp_semicolon ? " semicolon" : "");
			      ਰੋਕੋ;
	    ਹਾਲਤ ISN_SHUFFLE: smsg("%s%4d SHUFFLE %d up %d", pfx, current,
					 iptr->isn_arg.shuffle.shfl_ਵਸਤੂ,
					 iptr->isn_arg.shuffle.shfl_ਉੱਪਰ);
			      ਰੋਕੋ;
	    ਹਾਲਤ ISN_DROP: smsg("%s%4d DROP", pfx, current); ਰੋਕੋ;

	    ਹਾਲਤ ISN_SCRIPTCTX_ਸੈੱਟ:
		{
		    ਪੂਰਨ	sid = iptr->isn_arg.setsctx.sc_sid;
		    scriptitem_T *si = SCRIPT_ਵਸਤੂ(sid);
		    smsg("%s%4d SCRIPTCTX_SET %s", pfx, current, si->sn_name);
		    script_ctx = iptr->isn_arg.setsctx;
		}
		ਰੋਕੋ;

	    ਹਾਲਤ ISN_ਅੰਤ: // End of list of instructions for ISN_SUBSTITUTE.
			   ਵਾਪਸ;
	}

	ਬਾਹਰਲਾ_flush();	    // output one line at a time
	ui_breakcheck();
	ਜੇ (got_int)
	    ਰੋਕੋ;
    }
}

/*
 * Handle command line completion for the :disassemble command.
 */
    ਖਾਲੀ
ਸੈੱਟ_context_ਵਿੱਚ_disassemble_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;

    // Default: expand user functions, "debug" and "profile"
    xp->xp_context = EXPAND_DISASSEMBLE;
    xp->xp_ਨਮੂਨਾ = arg;

    // first argument already typed: only user function names
    ਜੇ (*arg != NUL && *(p = skiptowhite(arg)) != NUL)
    {
	xp->xp_context = EXPAND_USER_FUNC;
	xp->xp_ਨਮੂਨਾ = skipwhite(p);
    }
}

/*
 * Function given to ExpandGeneric() to obtain the list of :disassemble
 * arguments.
 */
    char_u *
get_disassemble_ਦਲੀਲ(expand_T *xp, ਪੂਰਨ idx)
{
    ਜੇ (idx == 0)
	ਵਾਪਸ (char_u *)"debug";
    ਜੇ (idx == 1)
	ਵਾਪਸ (char_u *)"profile";
    ਵਾਪਸ get_user_func_name(xp, idx - 2);
}

/*
 * ":disassemble".
 * We don't really need this at runtime, but we do have tests that require it,
 * so always include this.
 */
    ਖਾਲੀ
ex_disassemble(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    ufunc_T	*ufunc;
    dfunc_T	*dfunc;
    isn_T	*instr = ਨਿਰਾ;  // init to shut up gcc warning
    ਪੂਰਨ		instr_ਗਿਣਤੀ = 0;  // init to shut up gcc warning
    compiletype_T compile_type = CT_NONE;

    ufunc = ਲੱਭੋ_func_by_name(arg, &compile_type);
    ਜੇ (ufunc == ਨਿਰਾ)
	ਵਾਪਸ;
    ਜੇ (func_needs_compiling(ufunc, compile_type)
	    && compile_def_function(ufunc, FALSE, compile_type, ਨਿਰਾ) == FAIL)
	ਵਾਪਸ;
    ਜੇ (ufunc->uf_def_ਸਥਿਤੀ != UF_COMPILED)
    {
	semsg(_(e_function_is_not_compiled_str), eap->arg);
	ਵਾਪਸ;
    }
    ਸੁਨੇਹਾ((ਅੱਖਰ *)printable_func_name(ufunc));

    dfunc = ((dfunc_T *)def_functions.ga_ਅੰਕੜੇ) + ufunc->uf_dfunc_idx;
    ਚੋਣ (compile_type)
    {
	ਹਾਲਤ CT_PROFILE:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
	    instr = dfunc->df_instr_prof;
	    instr_ਗਿਣਤੀ = dfunc->df_instr_prof_ਗਿਣਤੀ;
	    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
	    // FALLTHROUGH
	ਹਾਲਤ CT_NONE:
	    instr = dfunc->df_instr;
	    instr_ਗਿਣਤੀ = dfunc->df_instr_ਗਿਣਤੀ;
	    ਰੋਕੋ;
	ਹਾਲਤ CT_DEBUG:
	    instr = dfunc->df_instr_debug;
	    instr_ਗਿਣਤੀ = dfunc->df_instr_debug_ਗਿਣਤੀ;
	    ਰੋਕੋ;
    }

    list_instructions("", instr, instr_ਗਿਣਤੀ, ufunc);
}

/*
 * Return TRUE when "tv" is not falsy: non-zero, non-empty string, non-empty
 * list, etc.  Mostly like what JavaScript does, except that empty list and
 * empty dictionary are FALSE.
 */
    ਪੂਰਨ
tv2bool(typval_T *tv)
{
    ਚੋਣ (tv->v_type)
    {
	ਹਾਲਤ VAR_ਸੰਖਿਆ:
	    ਵਾਪਸ tv->vval.v_ਸੰਖਿਆ != 0;
	ਹਾਲਤ VAR_FLOAT:
	    ਵਾਪਸ tv->vval.v_float != 0.0;
	ਹਾਲਤ VAR_PARTIAL:
	    ਵਾਪਸ tv->vval.v_partial != ਨਿਰਾ;
	ਹਾਲਤ VAR_FUNC:
	ਹਾਲਤ VAR_STRING:
	    ਵਾਪਸ tv->vval.v_string != ਨਿਰਾ && *tv->vval.v_string != NUL;
	ਹਾਲਤ VAR_LIST:
	    ਵਾਪਸ tv->vval.v_list != ਨਿਰਾ && tv->vval.v_list->lv_len > 0;
	ਹਾਲਤ VAR_TUPLE:
	    ਵਾਪਸ tuple_len(tv->vval.v_tuple) > 0;
	ਹਾਲਤ VAR_DICT:
	    ਵਾਪਸ tv->vval.v_dict != ਨਿਰਾ
				    && tv->vval.v_dict->dv_hashtab.ht_ਵਰਤਿਆ > 0;
	ਹਾਲਤ VAR_BOOL:
	ਹਾਲਤ VAR_SPECIAL:
	    ਵਾਪਸ tv->vval.v_ਸੰਖਿਆ == VVAL_TRUE ? TRUE : FALSE;
	ਹਾਲਤ VAR_ਕੰਮ:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
	    ਵਾਪਸ tv->vval.v_ਕੰਮ != ਨਿਰਾ;
#ਨਹੀਂਤਾਂ
	    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
	ਹਾਲਤ VAR_CHANNEL:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
	    ਵਾਪਸ tv->vval.v_channel != ਨਿਰਾ;
#ਨਹੀਂਤਾਂ
	    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
	ਹਾਲਤ VAR_BLOB:
	    ਵਾਪਸ tv->vval.v_blob != ਨਿਰਾ && tv->vval.v_blob->bv_ga.ga_len > 0;

	ਹਾਲਤ VAR_OBJECT:
	    ਵਾਪਸ tv->vval.v_object != ਨਿਰਾ;

	ਹਾਲਤ VAR_CLASS:
	ਹਾਲਤ VAR_TYPEALIAS:
	    check_typval_is_ਮੁੱਲ(tv);
	    ਰੋਕੋ;

	ਹਾਲਤ VAR_UNKNOWN:
	ਹਾਲਤ VAR_ANY:
	ਹਾਲਤ VAR_VOID:
	ਹਾਲਤ VAR_INSTR:
	    ਰੋਕੋ;
    }
    ਵਾਪਸ FALSE;
}

    ਖਾਲੀ
emsg_using_string_as(typval_T *tv, ਪੂਰਨ as_ਸੰਖਿਆ)
{
    semsg(_(as_ਸੰਖਿਆ ? e_using_string_as_ਸੰਖਿਆ_str
						 : e_using_string_as_bool_str),
		       tv->vval.v_string == ਨਿਰਾ
					   ? (char_u *)"" : tv->vval.v_string);
}

/*
 * If "tv" is a string give an error and return FAIL.
 */
    ਪੂਰਨ
check_not_string(typval_T *tv)
{
    ਜੇ (tv->v_type == VAR_STRING)
    {
	emsg_using_string_as(tv, TRUE);
	clear_tv(tv);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

#ਜੇ_ਅੰਤ // FEAT_EVAL
